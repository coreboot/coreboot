/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Intel Corp.
 * (Written by Andrey Petrov <andrey.petrov@intel.com> for Intel Corp.)
 * (Written by Alexandru Gagniuc <alexandrux.gagniuc@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/early_variables.h>
#include <boot_device.h>
#include <cbfs.h>
#include <commonlib/region.h>
#include <console/console.h>
#include <fmap.h>
#include <soc/flash_ctrlr.h>
#include <soc/mmap_boot.h>

/*
 * BIOS region on the flash is mapped right below 4GiB in the address
 * space. However, 256KiB right below 4GiB is decoded by read-only SRAM and not
 * boot media.
 *
 *                                                            +----------------+ 0
 *                                                            |                |
 *                                                            |                |
 *                                                            |                |
 *                                                            |                |
 *                                                            |                |
 *                                                            |                |
 *                                                            |                |
 *                                                            |                |
 *                  +------------+                            |                |
 *                  |    IFD     |                            |                |
 * bios_start +---> +------------+--------------------------> +----------------+ 4GiB - bios_size
 *     ^            |            |              ^             |                |
 *     |            |            |              |             |                |
 *     |            |            |       bios_mapped_size     |      BIOS      |
 *     |            |    BIOS    |              |             |                |
 * bios_size        |            |              |             |                |
 *     |            |            |              v             |                |
 *     |            |            +--------------------------> +----------------+ 4GiB - 256KiB
 *     v            |            |                            | Read only SRAM |
 * bios_end   +---> +------------+                            +----------------+ 4GiB
 *                  | Device ext |
 *                  +------------+
 *
 */

static size_t bios_start CAR_GLOBAL;
static size_t bios_size CAR_GLOBAL;

static struct mem_region_device shadow_dev CAR_GLOBAL;
static struct xlate_region_device real_dev CAR_GLOBAL;

static void bios_mmap_init(void)
{
	size_t size;

	size = car_get_var(bios_size);

	/* If bios_size is initialized, then bail out. */
	if (size != 0)
		return;

	size_t start, bios_end, bios_mapped_size;
	uintptr_t base;

	/*
	 * BIOS_BFPREG provides info about BIOS Flash Primary Region
	 * Base and Limit.
	 * Base and Limit fields are in units of 4KiB.
	 */
	uint32_t val = spi_flash_ctrlr_reg_read(SPIBAR_BIOS_BFPREG);

	start = (val & SPIBAR_BFPREG_PRB_MASK) * 4 * KiB;
	bios_end = (((val & SPIBAR_BFPREG_PRL_MASK) >>
		     SPIBAR_BFPREG_PRL_SHIFT) + 1) * 4 * KiB;
	size = bios_end - start;

	/* BIOS region is mapped right below 4G. */
	base = 4ULL * GiB - size;

	/*
	 * The 256 KiB right below 4G are decoded by readonly SRAM,
	 * not boot media.
	 */
	bios_mapped_size = size - 256 * KiB;

	struct mem_region_device *shadow_dev_ptr;
	struct xlate_region_device *real_dev_ptr;
	shadow_dev_ptr = car_get_var_ptr(&shadow_dev);
	real_dev_ptr = car_get_var_ptr(&real_dev);

	mem_region_device_ro_init(shadow_dev_ptr, (void *)base,
			       bios_mapped_size);

	xlate_region_device_ro_init(real_dev_ptr, &shadow_dev_ptr->rdev,
				 start, bios_mapped_size,
				 CONFIG_ROM_SIZE);

	car_set_var(bios_start, start);
	car_set_var(bios_size, size);
}

const struct region_device *boot_device_ro(void)
{
	bios_mmap_init();

	struct xlate_region_device *real_dev_ptr;
	real_dev_ptr = car_get_var_ptr(&real_dev);

	return &real_dev_ptr->rdev;
}

static int iafw_boot_region_properties(struct cbfs_props *props)
{
	struct region regn;

	/* use fmap to locate CBFS area */
	if (fmap_locate_area("COREBOOT", &regn))
		return -1;

	props->offset = region_offset(&regn);
	props->size = region_sz(&regn);

	printk(BIOS_DEBUG, "CBFS @ %zx size %zx\n", props->offset, props->size);

	return 0;
}

/*
 * Named cbfs_master_header_locator so that it overrides the default, but
 * incompatible locator in cbfs.c
 */
const struct cbfs_locator cbfs_master_header_locator = {
	.name = "IAFW Locator",
	.locate = iafw_boot_region_properties,
};

size_t get_bios_size(void)
{
	bios_mmap_init();
	return car_get_var(bios_size);
}
