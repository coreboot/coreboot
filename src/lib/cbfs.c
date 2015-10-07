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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
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

	boot_device_init();

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

int cbfs_locate(struct cbfsf *fh, const struct region_device *cbfs,
		const char *name, uint32_t *type)
{
	size_t offset = 0;

	LOG("Locating '%s'\n", name);

	/* Try to scan the entire cbfs region looking for file name. */
	while (1) {
		struct cbfs_file file;
		const size_t fsz = sizeof(file);
		char *fname;
		int name_match;
		size_t datasz;

		DEBUG("Checking offset %zx\n", offset);

		/* Can't read file. Nothing else to do but bail out. */
		if (rdev_readat(cbfs, &file, offset, fsz) != fsz)
			break;

		if (memcmp(file.magic, CBFS_FILE_MAGIC, sizeof(file.magic))) {
			offset++;
			offset = ALIGN_UP(offset, CBFS_ALIGNMENT);
			continue;
		}

		file.len = ntohl(file.len);
		file.type = ntohl(file.type);
		file.offset = ntohl(file.offset);

		/* See if names match. */
		fname = rdev_mmap(cbfs, offset + fsz, file.offset - fsz);

		if (fname == NULL)
			break;

		name_match = !strcmp(fname, name);
		rdev_munmap(cbfs, fname);

		if (!name_match) {
			DEBUG(" Unmatched '%s' at %zx\n", fname, offset);
			offset += file.offset + file.len;
			offset = ALIGN_UP(offset, CBFS_ALIGNMENT);
			continue;
		}

		if (type != NULL && *type != file.type) {
			DEBUG(" Unmatched type %x at %zx\n", file.type, offset);
			offset += file.offset + file.len;
			offset = ALIGN_UP(offset, CBFS_ALIGNMENT);
			continue;
		}

		LOG("Found @ offset %zx size %x\n", offset, file.len);
		/* File and type match. Keep track of both the metadata and
		 * the data for the file. */
		if (rdev_chain(&fh->metadata, cbfs, offset, file.offset))
			break;
		offset += file.offset;
		datasz = file.len;
		if (rdev_chain(&fh->data, cbfs, offset, datasz))
			break;

		/* Success. */
		return 0;
	}

	LOG("'%s' not found.\n", name);
	return -1;
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
	struct prog stage = PROG_INIT(ASSET_UNKNOWN, name);
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
		return 0;

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
