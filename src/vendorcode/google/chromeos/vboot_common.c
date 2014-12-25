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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stddef.h>
#include <string.h>
#include <boot/coreboot_tables.h>
#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include "chromeos.h"
#include "vboot_common.h"
#include "vboot_handoff.h"

void vboot_locate_region(const char *name, struct vboot_region *region)
{
	region->size = find_fmap_entry(name, (void **)&region->offset_addr);
}

void *vboot_get_region(uintptr_t offset_addr, size_t size, void *dest)
{
	if (IS_ENABLED(CONFIG_SPI_FLASH_MEMORY_MAPPED)) {
		if (dest != NULL)
			return memcpy(dest, (void *)offset_addr, size);
		else
			return (void *)offset_addr;
	} else {
		struct cbfs_media default_media, *media = &default_media;
		void *cache;

		init_default_cbfs_media(media);
		media->open(media);
		if (dest != NULL) {
			cache = dest;
			if (media->read(media, dest, offset_addr, size) != size)
				cache = NULL;
		} else {
			cache = media->map(media, offset_addr, size);
			if (cache == CBFS_MEDIA_INVALID_MAP_ADDRESS)
				cache = NULL;
		}
		media->close(media);
		return cache;
	}
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
struct vboot_components *vboot_locate_components(struct vboot_region *region)
{
	size_t req_size;
	struct vboot_components *vbc;

	req_size = sizeof(*vbc);
	req_size += sizeof(struct vboot_component_entry) *
			MAX_PARSED_FW_COMPONENTS;

	vbc = vboot_get_region(region->offset_addr, req_size, NULL);
	if (vbc && vbc->num_components > MAX_PARSED_FW_COMPONENTS)
		vbc = NULL;

	return vbc;
}

void *vboot_get_payload(int *len)
{
	struct vboot_handoff *vboot_handoff;
	struct firmware_component *fwc;

	if (IS_ENABLED(CONFIG_MULTIPLE_CBFS_INSTANCES))
		return NULL; /* Let CBFS figure it out. */

	vboot_handoff = cbmem_find(CBMEM_ID_VBOOT_HANDOFF);

	if (vboot_handoff == NULL)
		return NULL;

	if (CONFIG_VBOOT_BOOT_LOADER_INDEX >= MAX_PARSED_FW_COMPONENTS) {
		printk(BIOS_ERR, "Invalid boot loader index: %d\n",
		       CONFIG_VBOOT_BOOT_LOADER_INDEX);
		return NULL;
	}

	fwc = &vboot_handoff->components[CONFIG_VBOOT_BOOT_LOADER_INDEX];

	/* If payload size is zero fall back to cbfs path. */
	if (fwc->size == 0)
		return NULL;

	if (len != NULL)
		*len = fwc->size;

	printk(BIOS_DEBUG, "Booting 0x%x byte verified payload at 0x%08x.\n",
	       fwc->size, fwc->address);

	/* This will leak a mapping. */
	return vboot_get_region(fwc->address, fwc->size, NULL);
}
