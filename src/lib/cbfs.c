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
#include <endian.h>
#include <lib.h>
#include <symbols.h>

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

	/* All boot CBFS operations are performed using the RO devie. */
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

static size_t inflate(void *src, void *dst)
{
	if (ENV_BOOTBLOCK || ENV_VERSTAGE)
		return 0;
	if (ENV_ROMSTAGE && !IS_ENABLED(CONFIG_COMPRESS_RAMSTAGE))
		return 0;
	return ulzma(src, dst);
}

static inline int tohex4(unsigned int c)
{
	return (c <= 9) ? (c + '0') : (c - 10 + 'a');
}

static void tohex16(unsigned int val, char* dest)
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
	if (ENV_VERSTAGE && IS_ENABLED(CONFIG_ARCH_X86) &&
	    IS_ENABLED(CONFIG_SPI_FLASH_MEMORY_MAPPED)) {
		void *mapping = rdev_mmap(fh, foffset, fsize);
		rdev_munmap(fh, mapping);
		if (mapping == load)
			goto out;
	}

	if (stage.compression == CBFS_COMPRESS_NONE) {
		if (rdev_readat(fh, load, foffset, fsize) != fsize)
			return -1;
	} else if (stage.compression == CBFS_COMPRESS_LZMA) {
		void *map = rdev_mmap(fh, foffset, fsize);

		if (map == NULL)
			return -1;

		fsize = inflate(map, load);

		rdev_munmap(fh, map);

		if (!fsize)
			return -1;
	} else
		return -1;

	/* Clear area not covered by file. */
	memset(&load[fsize], 0, stage.memlen - fsize);

	arch_segment_loaded((uintptr_t)load, stage.memlen, SEG_FINAL);

out:
	prog_set_area(pstage, load, stage.memlen);
	prog_set_entry(pstage, entry, NULL);

	return 0;
}

static int cbfs_master_header_props(struct cbfs_props *props)
{
	struct cbfs_header header;
	const struct region_device *bdev;
	int32_t rel_offset;
	size_t offset;

	bdev = boot_device_ro();

	if (bdev == NULL)
		return -1;

	/* Find location of header using signed 32-bit offset from
	 * end of CBFS region. */
	offset = CONFIG_CBFS_SIZE - sizeof(int32_t);
	if (rdev_readat(bdev, &rel_offset, offset, sizeof(int32_t)) < 0)
		return -1;

	offset = CONFIG_CBFS_SIZE + rel_offset;
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
const struct cbfs_locator __attribute__((weak)) cbfs_master_header_locator = {
	.name = "Master Header Locator",
	.locate = cbfs_master_header_props,
};

extern const struct cbfs_locator vboot_locator;

static const struct cbfs_locator *locators[] = {
#if CONFIG_VBOOT_VERIFY_FIRMWARE
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
