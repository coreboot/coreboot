/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <boot/coreboot_tables.h>
#include <device/mmio.h>
#include <cbfs.h>
#include <cf9_reset.h>
#include <commonlib/bsd/compression.h>
#include <commonlib/fsp.h>
#include <console/console.h>
#include <fsp/util.h>
#include <string.h>
#include <types.h>

static bool looks_like_fsp_header(const uint8_t *raw_hdr)
{
	if (memcmp(raw_hdr, FSP_HDR_SIGNATURE, 4)) {
		printk(BIOS_ALERT, "Did not find a valid FSP signature\n");
		return false;
	}

	if (read32(raw_hdr + 4) != FSP_HDR_LEN) {
		printk(BIOS_ALERT, "FSP header has invalid length\n");
		return false;
	}

	return true;
}

enum cb_err fsp_identify(struct fsp_header *hdr, const void *fsp_blob)
{
	const uint8_t *raw_hdr = fsp_blob;

	if (!looks_like_fsp_header(raw_hdr))
		return CB_ERR;

	hdr->spec_version = read8(raw_hdr + 10);
	hdr->revision = read8(raw_hdr + 11);
	hdr->fsp_revision = read32(raw_hdr + 12);
	memcpy(hdr->image_id, raw_hdr + 16, ARRAY_SIZE(hdr->image_id));
	hdr->image_id[ARRAY_SIZE(hdr->image_id) - 1] = '\0';
	hdr->image_size = read32(raw_hdr + 24);
	hdr->image_base = read32(raw_hdr + 28);
	hdr->image_attribute = read16(raw_hdr + 32);
	hdr->component_attribute = read16(raw_hdr + 34);
	hdr->cfg_region_offset = read32(raw_hdr + 36);
	hdr->cfg_region_size = read32(raw_hdr + 40);
	hdr->temp_ram_init_entry = read32(raw_hdr + 48);
	hdr->temp_ram_exit_entry = read32(raw_hdr + 64);
	hdr->notify_phase_entry_offset = read32(raw_hdr + 56);
	hdr->memory_init_entry_offset = read32(raw_hdr + 60);
	hdr->silicon_init_entry_offset = read32(raw_hdr + 68);
	if (CONFIG(PLATFORM_USES_FSP2_2))
		hdr->multi_phase_si_init_entry_offset = read32(raw_hdr + 72);

	return CB_SUCCESS;
}

enum cb_err fsp_validate_component(struct fsp_header *hdr,
					const struct region_device *rdev)
{
	void *membase;

	/* Map just enough of the file to be able to parse the header. */
	membase = rdev_mmap(rdev, FSP_HDR_OFFSET, FSP_HDR_LEN);

	if (membase == NULL) {
		printk(BIOS_CRIT, "Could not mmap() FSP header.\n");
		return CB_ERR;
	}

	if (fsp_identify(hdr, membase) != CB_SUCCESS) {
		rdev_munmap(rdev, membase);
		printk(BIOS_CRIT, "No valid FSP header\n");
		return CB_ERR;
	}

	rdev_munmap(rdev, membase);

	if (CONFIG(DISPLAY_FSP_HEADER))
		fsp_print_header_info(hdr);

	/* Check if size specified in the header matches the cbfs file size */
	if (region_device_sz(rdev) < hdr->image_size) {
		printk(BIOS_CRIT, "Component size bigger than cbfs file.\n");
		return CB_ERR;
	}

	if (ENV_ROMSTAGE)
		soc_validate_fsp_version(hdr);

	return CB_SUCCESS;
}

static bool fsp_reset_requested(uint32_t status)
{
	return (status >= FSP_STATUS_RESET_REQUIRED_COLD &&
		status <= FSP_STATUS_RESET_REQUIRED_8);
}

void fsp_handle_reset(uint32_t status)
{
	if (!fsp_reset_requested(status))
		return;

	printk(BIOS_SPEW, "FSP: handling reset type %x\n", status);

	switch (status) {
	case FSP_STATUS_RESET_REQUIRED_COLD:
		full_reset();
		break;
	case FSP_STATUS_RESET_REQUIRED_WARM:
		system_reset();
		break;
	case FSP_STATUS_RESET_REQUIRED_3:
	case FSP_STATUS_RESET_REQUIRED_4:
	case FSP_STATUS_RESET_REQUIRED_5:
	case FSP_STATUS_RESET_REQUIRED_6:
	case FSP_STATUS_RESET_REQUIRED_7:
	case FSP_STATUS_RESET_REQUIRED_8:
		chipset_handle_reset(status);
		break;
	default:
		break;
	}
}

static inline bool fspm_env(void)
{
	if (ENV_ROMSTAGE)
		return true;
	return false;
}

static inline bool fspm_xip(void)
{
	/* FSP-M is assumed to be loaded in romstage. */
	if (fspm_env() && CONFIG(FSP_M_XIP))
		return true;
	return false;
}

static void *fsp_get_dest_and_load(struct fsp_load_descriptor *fspld, size_t size,
				const struct region_device *source_rdev,
				uint32_t compression_algo)
{
	void *dest;

	if (fspld->get_destination(fspld, &dest, size, source_rdev) < 0) {
		printk(BIOS_ERR, "FSP Destination not obtained.\n");
		return NULL;
	}

	/* Don't load when executing in place. */
	if (fspm_xip())
		return dest;

	if (cbfs_load_and_decompress(source_rdev, 0, region_device_sz(source_rdev),
			dest, size, compression_algo) != size) {
		printk(BIOS_ERR, "Failed to load FSP component.\n");
		return NULL;
	}

	/* Don't allow FSP-M relocation. */
	if (fspm_env())
		return dest;

	if (fsp_component_relocate((uintptr_t)dest, dest, size) < 0) {
		printk(BIOS_ERR, "Unable to relocate FSP component!\n");
		return NULL;
	}

	return dest;
}

/* Load the FSP component described by fsp_load_descriptor from cbfs. The FSP
 * header object will be validated and filled in on successful load. */
enum cb_err fsp_load_component(struct fsp_load_descriptor *fspld, struct fsp_header *hdr)
{
	struct cbfsf file_desc;
	uint32_t compression_algo;
	size_t output_size;
	void *dest;
	struct region_device source_rdev;
	struct prog *fsp_prog = &fspld->fsp_prog;

	if (fspld->get_destination == NULL)
		return CB_ERR;

	if (cbfs_boot_locate(&file_desc, prog_name(fsp_prog), &fsp_prog->cbfs_type) < 0)
		return CB_ERR;

	if (cbfsf_decompression_info(&file_desc, &compression_algo, &output_size) < 0)
		return CB_ERR;

	cbfs_file_data(&source_rdev, &file_desc);

	dest = fsp_get_dest_and_load(fspld, output_size, &source_rdev, compression_algo);

	if (dest == NULL)
		return CB_ERR;

	prog_set_area(fsp_prog, dest, output_size);

	if (fsp_validate_component(hdr, prog_rdev(fsp_prog)) != CB_SUCCESS) {
		printk(BIOS_ERR, "Invalid FSP header after load!\n");
		return CB_ERR;
	}

	/* Signal that FSP component has been loaded. */
	prog_segment_loaded(hdr->image_base, hdr->image_size, SEG_FINAL);

	return CB_SUCCESS;
}

/* Only call this function when FSP header has been read and validated */
void fsp_get_version(char *buf)
{
	struct fsp_header *hdr = &fsps_hdr;
	union fsp_revision revision;

	revision.val = hdr->fsp_revision;
	snprintf(buf, FSP_VER_LEN, "%u.%u-%u.%u.%u.%u", (hdr->spec_version >> 4),
		hdr->spec_version & 0xf, revision.rev.major,
		revision.rev.minor, revision.rev.revision, revision.rev.bld_num);
}

/* Check if the signature in the UPD header matches the expected one. If it doesn't match, the
   FSP binaries in CBFS are for a different platform than the platform code trying to use it
   in which case the function calls die(). */
void fsp_verify_upd_header_signature(uint64_t upd_signature, uint64_t expected_signature)
{
	if (upd_signature != expected_signature) {
		/* The UPD signatures are non-zero-terminated ASCII stored as a little endian
		   uint64_t, so this needs some casts. */
		die_with_post_code(POST_INVALID_VENDOR_BINARY,
			"Invalid UPD signature! FSP provided \"%8s\", expected was \"%8s\".\n",
			(char *)&upd_signature,
			(char *)&expected_signature);
	}
}

/* Add FSP version to coreboot table LB_TAG_PLATFORM_BLOB_VERSION */
void lb_string_platform_blob_version(struct lb_header *header)
{
	struct lb_string *rec;
	size_t len;
	char fsp_version[FSP_VER_LEN] = {0};

	fsp_get_version(fsp_version);
	rec = (struct lb_string *)lb_new_record(header);
	rec->tag = LB_TAG_PLATFORM_BLOB_VERSION;
	len = strlen(fsp_version);
	rec->size = ALIGN_UP(sizeof(*rec) + len + 1, 8);
	memcpy(rec->string, fsp_version, len+1);
}

__weak void soc_validate_fsp_version(const struct fsp_header *hdr)
{
}
