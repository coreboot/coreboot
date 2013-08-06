/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
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


#include <assert.h>
#include <cbfs.h>  /* This driver serves as a CBFS media source. */
#include <stdlib.h>
#include <string.h>
#include <console/console.h>
#include "alternate_cbfs.h"
#include "spi.h"

/* This allows USB A-A firmware upload from a compatible host in four parts:
 * The first two are the bare BL1 and the Coreboot boot block, which are just
 * written to their respective loading addresses. These transfers are initiated
 * by the IROM / BL1, so this code has nothing to do with them.
 *
 * The third transfer is a valid CBFS image that contains only the romstage,
 * and must be small enough to fit into alternate_cbfs_size[__BOOT_BLOCK__] in
 * IRAM. It is loaded when this function gets called in the boot block, and
 * the normal CBFS code extracts the romstage from it.
 *
 * The fourth transfer is also a CBFS image, but can be of arbitrary size and
 * should contain all available stages/payloads/etc. It is loaded when this
 * function is called a second time at the end of the romstage, and copied to
 * alternate_cbfs_buffer[!__BOOT_BLOCK__] in DRAM. It will reside there for the
 * rest of the firmware's lifetime and all subsequent stages (which will not
 * have __PRE_RAM__ defined) can just directly reference it there.
 */
static int usb_cbfs_open(struct cbfs_media *media) {
#ifdef __PRE_RAM__
	static int first_run = 1;
	int (*irom_load_usb)(void) = *irom_load_image_from_usb_ptr;

	if (!first_run)
		return 0;

	if (!irom_load_usb()) {
		printk(BIOS_ERR, "Unable to load CBFS image via USB!\n");
		return -1;
	}

	/*
	 * We need to trust the host/irom to copy the image to our
	 * alternate_cbfs_buffer address... there is no way to control or even
	 * check the transfer size or target address from our side.
	 */

	printk(BIOS_DEBUG, "USB A-A transfer successful, CBFS image should now"
		" be at %p\n", alternate_cbfs_buffer);
	first_run = 0;
#endif
	return 0;
}

static int alternate_cbfs_close(struct cbfs_media *media) { return 0; }

static size_t alternate_cbfs_read(struct cbfs_media *media, void *dest,
				  size_t offset, size_t count) {
	ASSERT(offset + count < alternate_cbfs_size);
	memcpy(dest, alternate_cbfs_buffer + offset, count);
	return count;
}

static void *alternate_cbfs_map(struct cbfs_media *media, size_t offset,
				   size_t count) {
	ASSERT(offset + count < alternate_cbfs_size);
	return alternate_cbfs_buffer + offset;
}

static void *alternate_cbfs_unmap(struct cbfs_media *media,
				  const void *buffer) { return 0; }

static int initialize_exynos_usb_cbfs_media(struct cbfs_media *media) {
	printk(BIOS_DEBUG, "Using Exynos alternate boot mode USB A-A\n");

	media->open = usb_cbfs_open;
	media->close = alternate_cbfs_close;
	media->read = alternate_cbfs_read;
	media->map = alternate_cbfs_map;
	media->unmap = alternate_cbfs_unmap;

	return 0;
}

int init_default_cbfs_media(struct cbfs_media *media) {
	if (*iram_secondary_base == SECONDARY_BASE_BOOT_USB)
		return initialize_exynos_usb_cbfs_media(media);

	/* TODO: implement SDMMC (and possibly other) boot mode */

	return initialize_exynos_spi_cbfs_media(media,
		(void*)CONFIG_CBFS_CACHE_ADDRESS, CONFIG_CBFS_CACHE_SIZE);
}
