/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot_device.h>
#include <console/console.h>
#include <soc/alternate_cbfs.h>
#include <soc/power.h>
#include <soc/spi.h>
#include <symbols.h>

/* This allows USB A-A firmware upload from a compatible host in four parts:
 * The first two are the bare BL1 and the coreboot boot block, which are just
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
 * rest of the firmware's lifetime and all subsequent stages can just directly
 * reference it there.
 */
static int usb_cbfs_open(void)
{
	if (!ENV_ROMSTAGE_OR_BEFORE)
		return 0;

	static int first_run = 1;
	int (*irom_load_usb)(void) = *irom_load_image_from_usb_ptr;

	if (!first_run)
		return 0;

	if (!irom_load_usb()) {
		printk(BIOS_EMERG, "Unable to load CBFS image via USB!\n");
		return -1;
	}

	/*
	 * We need to trust the host/irom to copy the image to our
	 * _cbfs_cache address... there is no way to control or even
	 * check the transfer size or target address from our side.
	 */

	printk(BIOS_DEBUG, "USB A-A transfer successful, CBFS image should now"
		" be at %p\n", _cbfs_cache);
	first_run = 0;
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
static int sdmmc_cbfs_open(void)
{
	if (!ENV_ROMSTAGE_OR_BEFORE)
		return 0;

	/*
	 * In the bootblock, we just copy the small part that fits in the buffer
	 * and hope that it's enough (since the romstage is currently always the
	 * first component in the image, this should work out). In the romstage,
	 * we copy until our cache is full (currently 12M) to avoid the pain of
	 * figuring out the true image size from in here. Since this is mainly a
	 * developer/debug boot mode, those shortcomings should be bearable.
	 */
	const u32 count = REGION_SIZE(cbfs_cache) / 512;
	static int first_run = 1;
	int (*irom_load_sdmmc)(u32 start, u32 count, void *dst) =
		*irom_sdmmc_read_blocks_ptr;

	if (!first_run)
		return 0;

	if (!irom_load_sdmmc(1, count, _cbfs_cache)) {
		printk(BIOS_EMERG, "Unable to load CBFS image from SDMMC!\n");
		return -1;
	}

	printk(BIOS_DEBUG, "SDMMC read successful, CBFS image should now be"
		" at %p\n", _cbfs_cache);
	first_run = 0;
	return 0;
}

static const struct mem_region_device alternate_rdev =
	MEM_REGION_DEV_RO_INIT(_cbfs_cache, REGION_SIZE(cbfs_cache));

const struct region_device *boot_device_ro(void)
{
	if (*iram_secondary_base == SECONDARY_BASE_BOOT_USB)
		return &alternate_rdev.rdev;

	switch (exynos_power->om_stat & OM_STAT_MASK) {
	case OM_STAT_SDMMC:
		return &alternate_rdev.rdev;
	case OM_STAT_SPI:
		return exynos_spi_boot_device();
	default:
		printk(BIOS_EMERG, "Exynos OM_STAT value 0x%x not supported!\n",
			exynos_power->om_stat);
		return NULL;
	}
}

void boot_device_init(void)
{
	if (*iram_secondary_base == SECONDARY_BASE_BOOT_USB) {
		printk(BIOS_DEBUG, "Using Exynos alternate boot mode USB A-A\n");
		usb_cbfs_open();
		return;
	}

	switch (exynos_power->om_stat & OM_STAT_MASK) {
	case OM_STAT_SDMMC:
		printk(BIOS_DEBUG, "Using Exynos alternate boot mode SDMMC\n");
		sdmmc_cbfs_open();
		break;
	case OM_STAT_SPI:
		exynos_init_spi_boot_device();
		break;
	default:
		printk(BIOS_EMERG, "Exynos OM_STAT value 0x%x not supported!\n",
			exynos_power->om_stat);
	}
}
