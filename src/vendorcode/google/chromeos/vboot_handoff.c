/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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
#include "chromeos.h"
#include <boot/coreboot_tables.h>
#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include <payload_loader.h>
#include "vboot_handoff.h"

int vboot_enable_developer(void)
{
	struct vboot_handoff *vbho;

	vbho = cbmem_find(CBMEM_ID_VBOOT_HANDOFF);

	if (vbho == NULL) {
		printk(BIOS_ERR, "%s: Couldn't find vboot_handoff structure!\n",
		        __func__);
		return 0;
	}

	return !!(vbho->init_params.out_flags & VB_INIT_OUT_ENABLE_DEVELOPER);
}

int vboot_enable_recovery(void)
{
	struct vboot_handoff *vbho;

	vbho = cbmem_find(CBMEM_ID_VBOOT_HANDOFF);

	if (vbho == NULL)
		return 0;

	return !!(vbho->init_params.out_flags & VB_INIT_OUT_ENABLE_RECOVERY);
}

int __attribute__((weak)) clear_recovery_mode_switch(void)
{
	// Can be implemented by a mainboard
	return 0;
}

int vboot_skip_display_init(void)
{
	struct vboot_handoff *vbho;

	vbho = cbmem_find(CBMEM_ID_VBOOT_HANDOFF);

	if (vbho == NULL)
		return 0;

	return !(vbho->init_params.out_flags & VB_INIT_OUT_ENABLE_DISPLAY);
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

int __attribute((weak)) vboot_get_sw_write_protect(void)
{
	// Can be implemented by a platform / mainboard
	return 0;
}

#if CONFIG_VBOOT2_VERIFY_FIRMWARE

void *vboot_get_payload(int *len) { return NULL; }

#else /* CONFIG_VBOOT2_VERIFY_FIRMWARE */

static void *vboot_get_payload(size_t *len)
{
	struct vboot_handoff *vboot_handoff;
	struct firmware_component *fwc;

	vboot_handoff = cbmem_find(CBMEM_ID_VBOOT_HANDOFF);

	if (vboot_handoff == NULL)
		return NULL;

	if (CONFIG_VBOOT_BOOT_LOADER_INDEX >= MAX_PARSED_FW_COMPONENTS) {
		printk(BIOS_ERR, "Invalid boot loader index: %d\n",
		       CONFIG_VBOOT_BOOT_LOADER_INDEX);
		return NULL;
	}

	fwc = &vboot_handoff->components[CONFIG_VBOOT_BOOT_LOADER_INDEX];

	if (len != NULL)
		*len = fwc->size;

	printk(BIOS_DEBUG, "Booting 0x%x byte payload at 0x%08x.\n",
	       fwc->size, fwc->address);

	return (void *)fwc->address;
}
#endif

static int vboot_locate_payload(struct payload *payload)
{
	void *buffer;
	size_t size;

	buffer = vboot_get_payload(&size);

	if (buffer == NULL)
		return -1;

	payload->backing_store.data = buffer;
	payload->backing_store.size = size;

	return 0;
}

const struct payload_loader_ops vboot_payload_loader = {
	.name = "VBOOT",
	.locate = vboot_locate_payload,
};

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
