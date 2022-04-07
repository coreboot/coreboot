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
#include <assert.h>

static uint32_t fsp_hdr_get_expected_min_length(void)
{
	if (CONFIG(PLATFORM_USES_FSP2_3))
		return 80;
	else if (CONFIG(PLATFORM_USES_FSP2_2))
		return 76;
	else if (CONFIG(PLATFORM_USES_FSP2_1))
		return 72;
	else if (CONFIG(PLATFORM_USES_FSP2_0))
		return 72;
	else
		return dead_code_t(uint32_t);
}

static bool looks_like_fsp_header(struct fsp_header *hdr)
{
	if (memcmp(&hdr->signature, FSP_HDR_SIGNATURE, 4)) {
		printk(BIOS_ALERT, "Did not find a valid FSP signature\n");
		return false;
	}

	/* It is possible to build FSP with any version of EDK2 which could have introduced new
	   fields in FSP_INFO_HEADER. The new fields will be ignored based on the reported FSP
	   version. This check ensures that the reported header length is at least what the
	   reported FSP version requires so that we do not access any out-of-bound bytes. */
	if (hdr->header_length < fsp_hdr_get_expected_min_length()) {
		printk(BIOS_ALERT, "FSP header has invalid length: %d\n", hdr->header_length);
		return false;
	}

	return true;
}

enum cb_err fsp_identify(struct fsp_header *hdr, const void *fsp_blob)
{
	memcpy(hdr, fsp_blob, sizeof(struct fsp_header));
	if (!looks_like_fsp_header(hdr))
		return CB_ERR;

	return CB_SUCCESS;
}

enum cb_err fsp_validate_component(struct fsp_header *hdr, void *fsp_file, size_t file_size)
{
	void *raw_hdr = fsp_file + FSP_HDR_OFFSET;

	if (file_size < FSP_HDR_OFFSET + fsp_hdr_get_expected_min_length()) {
		printk(BIOS_CRIT, "FSP blob too small.\n");
		return CB_ERR;
	}

	if (fsp_identify(hdr, raw_hdr) != CB_SUCCESS) {
		printk(BIOS_CRIT, "No valid FSP header\n");
		return CB_ERR;
	}

	if (CONFIG(DISPLAY_FSP_HEADER))
		fsp_print_header_info(hdr);

	/* Check if size specified in the header matches the cbfs file size */
	if (file_size < hdr->image_size) {
		printk(BIOS_CRIT, "Component size bigger than cbfs file.\n");
		return CB_ERR;
	}

	if (ENV_RAMINIT)
		soc_validate_fspm_header(hdr);

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
	if (ENV_RAMINIT)
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

/* Load the FSP component described by fsp_load_descriptor from cbfs. The FSP
 * header object will be validated and filled in on successful load. */
enum cb_err fsp_load_component(struct fsp_load_descriptor *fspld, struct fsp_header *hdr)
{
	size_t output_size;
	void *dest;
	struct prog *fsp_prog = &fspld->fsp_prog;

	dest = cbfs_alloc(prog_name(fsp_prog), fspld->alloc, fspld, &output_size);
	if (!dest)
		return CB_ERR;

	/* Don't allow FSP-M relocation when XIP. */
	if (!fspm_xip() && fsp_component_relocate((uintptr_t)dest, dest, output_size) < 0) {
		printk(BIOS_ERR, "Unable to relocate FSP component!\n");
		return CB_ERR;
	}

	prog_set_area(fsp_prog, dest, output_size);

	if (fsp_validate_component(hdr, dest, output_size) != CB_SUCCESS) {
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

	revision.val = hdr->image_revision;
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

__weak void soc_validate_fspm_header(const struct fsp_header *hdr)
{
}
