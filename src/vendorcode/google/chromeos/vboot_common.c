/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 The ChromiumOS Authors.  All rights reserved.
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

#include <boot/coreboot_tables.h>
#include <boot_device.h>
#include <cbmem.h>
#include <console/cbmem_console.h>
#include <console/console.h>
#include <reset.h>
#include <stddef.h>
#include <string.h>

#include "chromeos.h"
#include "fmap.h"
#include "vboot_common.h"
#include "vboot_handoff.h"

void vboot_locate_region(const char *name, struct region *region)
{
	const struct fmap_area *area;

	region->size = 0;

	area = find_fmap_area(fmap_find(), name);

	if (area != NULL) {
		region->offset = area->offset;
		region->size = area->size;
	}
}

void *vboot_get_region(size_t offset, size_t size, void *dest)
{
	const struct region_device *boot_dev;
	struct region_device rdev;

	boot_device_init();
	boot_dev = boot_device_ro();

	if (boot_dev == NULL)
		return NULL;

	if (rdev_chain(&rdev, boot_dev, offset, size))
		return NULL;

	/* Each call will leak a mapping. */
	if (dest == NULL)
		return rdev_mmap_full(&rdev);

	if (rdev_readat(&rdev, dest, 0, size) != size)
		return NULL;

	return dest;
}

int vboot_get_handoff_info(void **addr, uint32_t *size)
{
	struct vboot_handoff *vboot_handoff;

	vboot_handoff = cbmem_find(CBMEM_ID_VBOOT_HANDOFF);

	if (vboot_handoff == NULL)
		return -1;

	*addr = vboot_handoff;
	*size = sizeof(*vboot_handoff);
	return 0;
}

/* This will leak a mapping of a fw region */
struct vboot_components *vboot_locate_components(struct region *region)
{
	size_t req_size;
	struct vboot_components *vbc;

	req_size = sizeof(*vbc);
	req_size += sizeof(struct vboot_component_entry) *
			MAX_PARSED_FW_COMPONENTS;

	vbc = vboot_get_region(region_offset(region), req_size, NULL);
	if (vbc && vbc->num_components > MAX_PARSED_FW_COMPONENTS)
		vbc = NULL;

	return vbc;
}

void vboot_reboot(void)
{
	if (IS_ENABLED(CONFIG_CONSOLE_CBMEM_DUMP_TO_UART))
		cbmem_dump_console();
	hard_reset();
	die("failed to reboot");
}
