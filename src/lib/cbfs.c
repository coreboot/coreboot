/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 secunet Security Networks AG
 * Copyright 2015 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <boot_device.h>
#include <cbfs.h>
#include <commonlib/compression.h>
#include <compiler.h>
#include <endian.h>
#include <lib.h>
#include <symbols.h>
#include <timestamp.h>
#include <fmap.h>
#include "fmap_config.h"

#define ERROR(x...) printk(BIOS_ERR, "CBFS: " x)
#define LOG(x...) printk(BIOS_INFO, "CBFS: " x)
#if IS_ENABLED(CONFIG_DEBUG_CBFS)
#define DEBUG(x...) printk(BIOS_SPEW, "CBFS: " x)
#else
#define DEBUG(x...)
#endif

int cbfs_boot_locate(struct cbfsf *fh, const char *name, uint32_t *type)
{
	struct region_device rdev;
	const struct region_device *boot_dev;
	struct cbfs_props props;

	if (cbfs_boot_region_properties(&props))
		return -1;

	/* All boot CBFS operations are performed using the RO device. */
	boot_dev = boot_device_ro();

	if (boot_dev == NULL)
		return -1;

	if (rdev_chain(&rdev, boot_dev, props.offset, props.size))
		return -1;

	return cbfs_locate(fh, &rdev, name, type);
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

	if (fmap_locate_area_as_rdev(region_name, &rdev)) {
		LOG("%s region not found while looking for %s\n",
			region_name, name);
		return -1;
	}

	return cbfs_locate(fh, &rdev, name, type);
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
		if ((ENV_BOOTBLOCK || ENV_VERSTAGE) &&
		    !IS_ENABLED(CONFIG_COMPRESS_PRERAM_STAGES))
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
		if (ENV_BOOTBLOCK || ENV_VERSTAGE)
			return 0;
		if (ENV_ROMSTAGE && IS_ENABLED(CONFIG_POSTCAR_STAGE))
			return 0;
		if ((ENV_ROMSTAGE || ENV_POSTCAR)
			&& !IS_ENABLED(CONFIG_COMPRESS_RAMSTAGE))
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

static void tohex16(unsigned int val, char *dest)
{
	dest[0] = tohex4(val>>12);
	dest[1] = tohex4((val>>8) & 0xf);
	dest[2] = tohex4((val>>4) & 0xf);
	dest[3] = tohex4(val & 0xf);
}

void *cbfs_boot_map_optionrom(uint16_t vendor, uint16_t device)
{
	char name[17] = "pciXXXX,XXXX.rom";

	tohex16(vendor, name+3);
	tohex16(device, name+8);

	return cbfs_boot_map_with_leak(name, CBFS_TYPE_OPTIONROM, NULL);
}

void *cbfs_boot_load_stage_by_name(const char *name)
{
	struct cbfsf fh;
	struct prog stage = PROG_INIT(PROG_UNKNOWN, name);
	uint32_t type = CBFS_TYPE_STAGE;

	if (cbfs_boot_locate(&fh, name, &type))
		return NULL;

	/* Chain data portion in the prog. */
	cbfs_file_data(prog_rdev(&stage), &fh);

	if (cbfs_prog_stage_load(&stage))
		return NULL;

	return prog_entry(&stage);
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
				     &decompressed_size) < 0
				     || decompressed_size > buf_size)
		return 0;

	return cbfs_load_and_decompress(&fh.data, 0, region_device_sz(&fh.data),
					buf, buf_size, compression_algo);
}

size_t cbfs_prog_stage_section(struct prog *pstage, uintptr_t *base)
{
	struct cbfs_stage stage;
	const struct region_device *fh = prog_rdev(pstage);

	if (rdev_readat(fh, &stage, 0, sizeof(stage)) != sizeof(stage))
		return 0;

	*base = (uintptr_t)stage.load;
	return stage.memlen;
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
	if (ENV_VERSTAGE && !IS_ENABLED(CONFIG_NO_XIP_EARLY_STAGES) &&
	    IS_ENABLED(CONFIG_BOOT_DEVICE_MEMORY_MAPPED)) {
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

/* This only supports the "COREBOOT" fmap region. */
static int cbfs_master_header_props(struct cbfs_props *props)
{
	struct cbfs_header header;
	const struct region_device *bdev;
	int32_t rel_offset;
	size_t offset;

	bdev = boot_device_ro();

	if (bdev == NULL)
		return -1;

	size_t fmap_top = ___FMAP__COREBOOT_BASE + ___FMAP__COREBOOT_SIZE;

	/* Find location of header using signed 32-bit offset from
	 * end of CBFS region. */
	offset = fmap_top - sizeof(int32_t);
	if (rdev_readat(bdev, &rel_offset, offset, sizeof(int32_t)) < 0)
		return -1;

	offset = fmap_top + rel_offset;
	if (rdev_readat(bdev, &header, offset, sizeof(header)) < 0)
		return -1;

	header.magic = ntohl(header.magic);
	header.romsize = ntohl(header.romsize);
	header.offset = ntohl(header.offset);

	if (header.magic != CBFS_HEADER_MAGIC)
		return -1;

	props->offset = header.offset;
	props->size = header.romsize;
	props->size -= props->offset;

	printk(BIOS_SPEW, "CBFS @ %zx size %zx\n", props->offset, props->size);

	return 0;
}

/* This struct is marked as weak to allow a particular platform to
 * override the master header logic. This implementation should work for most
 * devices. */
const struct cbfs_locator __weak cbfs_master_header_locator = {
	.name = "Master Header Locator",
	.locate = cbfs_master_header_props,
};

extern const struct cbfs_locator vboot_locator;

static const struct cbfs_locator *locators[] = {
#if IS_ENABLED(CONFIG_VBOOT)
	&vboot_locator,
#endif
	&cbfs_master_header_locator,
};

int cbfs_boot_region_properties(struct cbfs_props *props)
{
	int i;

	boot_device_init();

	for (i = 0; i < ARRAY_SIZE(locators); i++) {
		const struct cbfs_locator *ops;

		ops = locators[i];

		if (ops->locate == NULL)
			continue;

		if (ops->locate(props))
			continue;

		LOG("'%s' located CBFS at [%zx:%zx)\n",
			ops->name, props->offset, props->offset + props->size);

		return 0;
	}

	return -1;
}

void cbfs_prepare_program_locate(void)
{
	int i;

	boot_device_init();

	for (i = 0; i < ARRAY_SIZE(locators); i++) {
		if (locators[i]->prepare == NULL)
			continue;
		locators[i]->prepare();
	}
}
