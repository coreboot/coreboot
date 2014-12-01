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


#include <arch/cache.h>
#include <assert.h>
#include <cbfs.h>  /* This driver serves as a CBFS media source. */
#include <console/console.h>
#include <soc/alternate_cbfs.h>
#include <soc/power.h>
#include <soc/spi.h>
#include <stdlib.h>
#include <string.h>
#include <symbols.h>

/* This allows USB A-A firmware upload from a compatible host in four parts:
 * The first two are the bare BL1 and the Coreboot boot block, which are just
 * written to their respective loading addresses. These transfers are initiated
 * by the IROM / BL1, so this code has nothing to do with them.
 *
 * The third transfer is a valid CBFS image that contains only the romstage,
 * and must be small enough to fit into the PRE_RAM CBFS cache in
 * IRAM. It is loaded when this function gets called in the boot block, and
 * the normal CBFS code extracts the romstage from it.
 *
 * The fourth transfer is also a CBFS image, but can be of arbitrary size and
 * should contain all available stages/payloads/etc. It is loaded when this
 * function is called a second time at the end of the romstage, and copied to
 * the romstage/ramstage CBFS cache in DRAM. It will reside there for the
 * rest of the firmware's lifetime and all subsequent stages (which will not
 * have __PRE_RAM__ defined) can just directly reference it there.
 */
static int usb_cbfs_open(struct cbfs_media *media)
{
#ifdef __PRE_RAM__
	static int first_run = 1;
	int (*irom_load_usb)(void) = *irom_load_image_from_usb_ptr;

	if (!first_run)
		return 0;

	dcache_mmu_disable();
	if (!irom_load_usb()) {
		dcache_mmu_enable();
		printk(BIOS_EMERG, "Unable to load CBFS image via USB!\n");
		return -1;
	}
	dcache_mmu_enable();

	/*
	 * We need to trust the host/irom to copy the image to our
	 * _cbfs_cache address... there is no way to control or even
	 * check the transfer size or target address from our side.
	 */

	printk(BIOS_DEBUG, "USB A-A transfer successful, CBFS image should now"
		" be at %p\n", _cbfs_cache);
	first_run = 0;
#endif
	return 0;
}

/*
 * SDMMC works very similar to USB A-A: we copy the CBFS image into memory
 * and read it from there. While SDMMC would also allow direct block by block
 * on-demand reading, we might run into problems if we call back into the IROM
 * in very late boot stages (e.g. after initializing/changing MMC clocks)... so
 * this seems like a safer approach. It also makes it easy to pass our image
 * down to payloads.
 */
static int sdmmc_cbfs_open(struct cbfs_media *media)
{
#ifdef __PRE_RAM__
	/*
	 * In the bootblock, we just copy the small part that fits in the buffer
	 * and hope that it's enough (since the romstage is currently always the
	 * first component in the image, this should work out). In the romstage,
	 * we copy until our cache is full (currently 12M) to avoid the pain of
	 * figuring out the true image size from in here. Since this is mainly a
	 * developer/debug boot mode, those shortcomings should be bearable.
	 */
	const u32 count = _cbfs_cache_size / 512;
	static int first_run = 1;
	int (*irom_load_sdmmc)(u32 start, u32 count, void *dst) =
		*irom_sdmmc_read_blocks_ptr;

	if (!first_run)
		return 0;

	dcache_mmu_disable();
	if (!irom_load_sdmmc(1, count, _cbfs_cache)) {
		dcache_mmu_enable();
		printk(BIOS_EMERG, "Unable to load CBFS image from SDMMC!\n");
		return -1;
	}
	dcache_mmu_enable();

	printk(BIOS_DEBUG, "SDMMC read successful, CBFS image should now be"
		" at %p\n", _cbfs_cache);
	first_run = 0;
#endif
	return 0;
}

static int alternate_cbfs_close(struct cbfs_media *media) { return 0; }

static size_t alternate_cbfs_read(struct cbfs_media *media, void *dest,
				  size_t offset, size_t count)
{
	ASSERT(offset + count < _cbfs_cache_size);
	memcpy(dest, _cbfs_cache + offset, count);
	return count;
}

static void *alternate_cbfs_map(struct cbfs_media *media, size_t offset,
				   size_t count)
{
	ASSERT(offset + count < _cbfs_cache_size);
	return _cbfs_cache + offset;
}

static void *alternate_cbfs_unmap(struct cbfs_media *media,
				  const void *buffer) { return 0; }

static int initialize_exynos_sdmmc_cbfs_media(struct cbfs_media *media)
{
	printk(BIOS_DEBUG, "Using Exynos alternate boot mode SDMMC\n");

	media->open = sdmmc_cbfs_open;
	media->close = alternate_cbfs_close;
	media->read = alternate_cbfs_read;
	media->map = alternate_cbfs_map;
	media->unmap = alternate_cbfs_unmap;

	return 0;
}

static int initialize_exynos_usb_cbfs_media(struct cbfs_media *media)
{
	printk(BIOS_DEBUG, "Using Exynos alternate boot mode USB A-A\n");

	media->open = usb_cbfs_open;
	media->close = alternate_cbfs_close;
	media->read = alternate_cbfs_read;
	media->map = alternate_cbfs_map;
	media->unmap = alternate_cbfs_unmap;

	return 0;
}

int init_default_cbfs_media(struct cbfs_media *media)
{
	if (*iram_secondary_base == SECONDARY_BASE_BOOT_USB)
		return initialize_exynos_usb_cbfs_media(media);

	switch (exynos_power->om_stat & OM_STAT_MASK) {
	case OM_STAT_SDMMC:
		return initialize_exynos_sdmmc_cbfs_media(media);
	case OM_STAT_SPI:
		return initialize_exynos_spi_cbfs_media(media,
			_cbfs_cache, _cbfs_cache_size);
	default:
		printk(BIOS_EMERG, "Exynos OM_STAT value 0x%x not supported!\n",
			exynos_power->om_stat);
		return 1;
	}
}
