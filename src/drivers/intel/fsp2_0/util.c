/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Intel Corp.
 * (Written by Alexandru Gagniuc <alexandrux.gagniuc@intel.com> for Intel Corp.)
 * (Written by Andrey Petrov <andrey.petrov@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <arch/io.h>
#include <cbfs.h>
#include <console/console.h>
#include <fsp/util.h>
#include <lib.h>
#include <memrange.h>
#include <program_loading.h>
#include <string.h>

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
	hdr->notify_phase_entry_offset = read32(raw_hdr + 56);
	hdr->memory_init_entry_offset = read32(raw_hdr + 60);
	hdr->silicon_init_entry_offset = read32(raw_hdr + 68);

	return CB_SUCCESS;
}

void fsp_print_header_info(const struct fsp_header *hdr)
{
	union {
		uint32_t val;
		struct {
			uint8_t bld_num;
			uint8_t revision;
			uint8_t minor;
			uint8_t major;
		} rev;
	} revision;

	revision.val = hdr->fsp_revision;

	printk(BIOS_DEBUG, "Spec version: v%u.%u\n", (hdr->spec_version >> 4 ),
							hdr->spec_version & 0xf);
	printk(BIOS_DEBUG, "Revision: %u.%u.%u, Build Number %u\n",
							revision.rev.major,
							revision.rev.minor,
							revision.rev.revision,
							revision.rev.bld_num);
	printk(BIOS_DEBUG, "Type: %s/%s\n",
			(hdr->component_attribute & 1 ) ? "release" : "debug",
			(hdr->component_attribute & 2 ) ? "test" : "official");
	printk(BIOS_DEBUG, "image ID: %s, base 0x%lx + 0x%zx\n",
		hdr->image_id, hdr->image_base, hdr->image_size);
	printk(BIOS_DEBUG, "\tConfig region        0x%zx + 0x%zx\n",
		hdr->cfg_region_offset, hdr->cfg_region_size);

	if ((hdr->component_attribute >> 12) == FSP_HDR_ATTRIB_FSPM) {
		printk(BIOS_DEBUG, "\tMemory init offset   0x%zx\n",
						hdr->memory_init_entry_offset);
	}

	if ((hdr->component_attribute >> 12) == FSP_HDR_ATTRIB_FSPS) {
		printk(BIOS_DEBUG, "\tSilicon init offset  0x%zx\n",
						hdr->silicon_init_entry_offset);
		printk(BIOS_DEBUG, "\tNotify phase offset  0x%zx\n",
						hdr->notify_phase_entry_offset);
	}

}

/* TODO: this won't work for SoC's that need to XIP certain modules. */
enum cb_err fsp_load_binary(struct fsp_header *hdr,
			    const char *name,
			    struct range_entry *range)
{
	struct cbfsf file_desc;
	struct region_device file_data;
	void *membase;

	if (cbfs_boot_locate(&file_desc, name, NULL)) {
		printk(BIOS_ERR, "Could not locate %s in CBFS\n", name);
		return CB_ERR;
	}

	cbfs_file_data(&file_data, &file_desc);

	/* Map just enough of the file to be able to parse the header. */
	membase = rdev_mmap(&file_data, FSP_HDR_OFFSET, FSP_HDR_LEN);

	if (membase == NULL) {
		printk(BIOS_ERR, "Could not mmap() '%s' FSP header.\n", name);
		return CB_ERR;
	}

	if (fsp_identify(hdr, membase) != CB_SUCCESS) {
		rdev_munmap(&file_data, membase);
		printk(BIOS_ERR, "%s did not have a valid FSP header\n", name);
		return CB_ERR;
	}

	rdev_munmap(&file_data, membase);

	fsp_print_header_info(hdr);

	/* Check if size specified in the header matches the cbfs file size */
	if (region_device_sz(&file_data) < hdr->image_size) {
		printk(BIOS_ERR, "%s size bigger than cbfs file.\n", name);
		return CB_ERR;
	}

	/* Check if the binary load address is within expected range */
	/* TODO: this doesn't check the current running program footprint. */
	if (range_entry_base(range) > hdr->image_base ||
	    range_entry_end(range) <= hdr->image_base + hdr->image_size) {
		printk(BIOS_ERR, "%s is outside of allowed range\n", name);
		return CB_ERR;
	}

	/* Load binary into memory. */
	if (rdev_readat(&file_data, (void *)hdr->image_base, 0, hdr->image_size) < 0)
		return CB_ERR;

	/* Signal that FSP component has been loaded. */
	prog_segment_loaded(hdr->image_base, hdr->image_size, SEG_FINAL);

	return CB_SUCCESS;
}
