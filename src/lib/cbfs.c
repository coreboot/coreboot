/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <boot_device.h>
#include <cbfs.h>
#include <commonlib/bsd/compression.h>
#include <console/console.h>
#include <endian.h>
#include <fmap.h>
#include <lib.h>
#include <security/tpm/tspi/crtm.h>
#include <security/vboot/vboot_common.h>
#include <stdlib.h>
#include <string.h>
#include <symbols.h>
#include <timestamp.h>

#define ERROR(x...) printk(BIOS_ERR, "CBFS: " x)
#define LOG(x...) printk(BIOS_INFO, "CBFS: " x)
#if CONFIG(DEBUG_CBFS)
#define DEBUG(x...) printk(BIOS_SPEW, "CBFS: " x)
#else
#define DEBUG(x...)
#endif

int cbfs_boot_locate(struct cbfsf *fh, const char *name, uint32_t *type)
{
	struct region_device rdev;

	if (cbfs_boot_region_device(&rdev))
		return -1;

	int ret = cbfs_locate(fh, &rdev, name, type);

	if (CONFIG(VBOOT_ENABLE_CBFS_FALLBACK) && ret) {

		/*
		 * When VBOOT_ENABLE_CBFS_FALLBACK is enabled and a file is not available in the
		 * active RW region, the RO (COREBOOT) region will be used to locate the file.
		 *
		 * This functionality makes it possible to avoid duplicate files in the RO
		 * and RW partitions while maintaining updateability.
		 *
		 * Files can be added to the RO_REGION_ONLY config option to use this feature.
		 */
		printk(BIOS_DEBUG, "Fall back to RO region for %s\n", name);
		if (fmap_locate_area_as_rdev("COREBOOT", &rdev))
			ERROR("RO region not found\n");
		else
			ret = cbfs_locate(fh, &rdev, name, type);
	}

	if (!ret)
		if (tspi_measure_cbfs_hook(fh, name))
			return -1;

	return ret;
}

void *cbfs_boot_map_with_leak(const char *name, uint32_t type, size_t *size)
{
	struct cbfsf fh;
	size_t fsize;

	if (cbfs_boot_locate(&fh, name, &type))
		return NULL;

	fsize = region_device_sz(&fh.data);

	if (size != NULL)
		*size = fsize;

	return rdev_mmap(&fh.data, 0, fsize);
}

int cbfs_locate_file_in_region(struct cbfsf *fh, const char *region_name,
			       const char *name, uint32_t *type)
{
	struct region_device rdev;
	int ret = 0;
	if (fmap_locate_area_as_rdev(region_name, &rdev)) {
		LOG("%s region not found while looking for %s\n",
		    region_name, name);
		return -1;
	}

	ret = cbfs_locate(fh, &rdev, name, type);
	if (!ret)
		if (tspi_measure_cbfs_hook(fh, name))
			return -1;
	return ret;
}

size_t cbfs_load_and_decompress(const struct region_device *rdev, size_t offset,
	size_t in_size, void *buffer, size_t buffer_size, uint32_t compression)
{
	size_t out_size;

	switch (compression) {
	case CBFS_COMPRESS_NONE:
		if (buffer_size < in_size)
			return 0;
		if (rdev_readat(rdev, buffer, offset, in_size) != in_size)
			return 0;
		return in_size;

	case CBFS_COMPRESS_LZ4:
		if ((ENV_BOOTBLOCK || ENV_SEPARATE_VERSTAGE) &&
			!CONFIG(COMPRESS_PRERAM_STAGES))
			return 0;

		/* Load the compressed image to the end of the available memory
		 * area for in-place decompression. It is the responsibility of
		 * the caller to ensure that buffer_size is large enough
		 * (see compression.h, guaranteed by cbfstool for stages). */
		void *compr_start = buffer + buffer_size - in_size;
		if (rdev_readat(rdev, compr_start, offset, in_size) != in_size)
			return 0;

		timestamp_add_now(TS_START_ULZ4F);
		out_size = ulz4fn(compr_start, in_size, buffer, buffer_size);
		timestamp_add_now(TS_END_ULZ4F);
		return out_size;

	case CBFS_COMPRESS_LZMA:
		/* We assume here romstage and postcar are never compressed. */
		if (ENV_BOOTBLOCK || ENV_SEPARATE_VERSTAGE)
			return 0;
		if (ENV_ROMSTAGE && CONFIG(POSTCAR_STAGE))
			return 0;
		if ((ENV_ROMSTAGE || ENV_POSTCAR)
		    && !CONFIG(COMPRESS_RAMSTAGE))
			return 0;
		void *map = rdev_mmap(rdev, offset, in_size);
		if (map == NULL)
			return 0;

		/* Note: timestamp not useful for memory-mapped media (x86) */
		timestamp_add_now(TS_START_ULZMA);
		out_size = ulzman(map, in_size, buffer, buffer_size);
		timestamp_add_now(TS_END_ULZMA);

		rdev_munmap(rdev, map);

		return out_size;

	default:
		return 0;
	}
}

static inline int tohex4(unsigned int c)
{
	return (c <= 9) ? (c + '0') : (c - 10 + 'a');
}

static void tohex8(unsigned int val, char *dest)
{
	dest[0] = tohex4((val >> 4) & 0xf);
	dest[1] = tohex4(val & 0xf);
}

static void tohex16(unsigned int val, char *dest)
{
	dest[0] = tohex4(val >> 12);
	dest[1] = tohex4((val >> 8) & 0xf);
	dest[2] = tohex4((val >> 4) & 0xf);
	dest[3] = tohex4(val & 0xf);
}

void *cbfs_boot_map_optionrom(uint16_t vendor, uint16_t device)
{
	char name[17] = "pciXXXX,XXXX.rom";

	tohex16(vendor, name + 3);
	tohex16(device, name + 8);

	return cbfs_boot_map_with_leak(name, CBFS_TYPE_OPTIONROM, NULL);
}

void *cbfs_boot_map_optionrom_revision(uint16_t vendor, uint16_t device, uint8_t rev)
{
	char name[20] = "pciXXXX,XXXX,XX.rom";

	tohex16(vendor, name + 3);
	tohex16(device, name + 8);
	tohex8(rev, name + 13);

	return cbfs_boot_map_with_leak(name, CBFS_TYPE_OPTIONROM, NULL);
}

size_t cbfs_boot_load_file(const char *name, void *buf, size_t buf_size,
			   uint32_t type)
{
	struct cbfsf fh;
	uint32_t compression_algo;
	size_t decompressed_size;

	if (cbfs_boot_locate(&fh, name, &type) < 0)
		return 0;

	if (cbfsf_decompression_info(&fh, &compression_algo,
				     &decompressed_size)
		    < 0
	    || decompressed_size > buf_size)
		return 0;

	return cbfs_load_and_decompress(&fh.data, 0, region_device_sz(&fh.data),
					buf, buf_size, compression_algo);
}

int cbfs_prog_stage_load(struct prog *pstage)
{
	struct cbfs_stage stage;
	uint8_t *load;
	void *entry;
	size_t fsize;
	size_t foffset;
	const struct region_device *fh = prog_rdev(pstage);

	if (rdev_readat(fh, &stage, 0, sizeof(stage)) != sizeof(stage))
		return -1;

	fsize = region_device_sz(fh);
	fsize -= sizeof(stage);
	foffset = 0;
	foffset += sizeof(stage);

	assert(fsize == stage.len);

	/* Note: cbfs_stage fields are currently in the endianness of the
	 * running processor. */
	load = (void *)(uintptr_t)stage.load;
	entry = (void *)(uintptr_t)stage.entry;

	/* Hacky way to not load programs over read only media. The stages
	 * that would hit this path initialize themselves. */
	if ((ENV_BOOTBLOCK || ENV_SEPARATE_VERSTAGE) &&
	    !CONFIG(NO_XIP_EARLY_STAGES) && CONFIG(BOOT_DEVICE_MEMORY_MAPPED)) {
		void *mapping = rdev_mmap(fh, foffset, fsize);
		rdev_munmap(fh, mapping);
		if (mapping == load)
			goto out;
	}

	fsize = cbfs_load_and_decompress(fh, foffset, fsize, load,
					 stage.memlen, stage.compression);
	if (!fsize)
		return -1;

	/* Clear area not covered by file. */
	memset(&load[fsize], 0, stage.memlen - fsize);

	prog_segment_loaded((uintptr_t)load, stage.memlen, SEG_FINAL);

out:
	prog_set_area(pstage, load, stage.memlen);
	prog_set_entry(pstage, entry, NULL);

	return 0;
}

int cbfs_boot_region_device(struct region_device *rdev)
{
	boot_device_init();
	return vboot_locate_cbfs(rdev) &&
	       fmap_locate_area_as_rdev("COREBOOT", rdev);
}
