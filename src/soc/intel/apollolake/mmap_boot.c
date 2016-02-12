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
 */

#include <boot_device.h>
#include <cbfs.h>
#include <commonlib/region.h>
#include <console/console.h>
#include <fmap.h>

/* The 128 KiB right below 4G are decoded by readonly SRAM, not boot media */
#define IFD_BIOS_MAX_MAPPED	(CONFIG_IFD_BIOS_END - 128 * KiB)
#define IFD_MAPPED_SIZE		(IFD_BIOS_MAX_MAPPED - CONFIG_IFD_BIOS_START)
#define IFD_BIOS_SIZE		(CONFIG_IFD_BIOS_END - CONFIG_IFD_BIOS_START)

/*
 *  If Apollo Lake is configured to boot from SPI flash "BIOS" region
 *  (as defined in descriptor) is mapped below 4GiB.  Form a pointer for
 *  the base.
 */
#define VIRTUAL_ROM_BASE ((uintptr_t)(0x100000000ULL - IFD_BIOS_SIZE))

static const struct mem_region_device shadow_dev = MEM_REGION_DEV_INIT(
	VIRTUAL_ROM_BASE, IFD_BIOS_MAX_MAPPED
);

/*
 * This is how we translate physical SPI flash address space into CPU memory-mapped space. In
 * essence this means "BIOS" region (usually starts at flash physical 0x1000 is mapped to
 * 4G - IFD_BIOS_SIZE.
 */
static const struct xlate_region_device real_dev = XLATE_REGION_INIT(
		&shadow_dev.rdev, CONFIG_IFD_BIOS_START,
		IFD_MAPPED_SIZE, CONFIG_ROM_SIZE
);

const struct region_device *boot_device_ro(void)
{
	return &real_dev.rdev;
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
