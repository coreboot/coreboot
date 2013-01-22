/*
 * Copyright (C) 2011 Samsung Electronics
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/* FIXME(dhendrix): pulled in a lot of extra crap such as partition and string
   libs*/
#include <assert.h>
#include <common.h>
#include <stdlib.h>
#include <string.h>

#include <console/console.h>
#include <console/loglevel.h>
//#include <asm/arch/board.h>

#include <config.h>
#include <spi.h>
#if 0
#include <asm/arch/clock.h>
#include <asm/arch-exynos/spi.h>
#include <asm/arch/pinmux.h>
#include <asm/arch/power.h>
#endif
#include <arch/hlt.h>
#include <cpu/samsung/exynos5250/clk.h>
#include <cpu/samsung/exynos5250/cpu.h>
#include <cpu/samsung/exynos5250/periph.h>
#include <cpu/samsung/exynos5250/pinmux.h>
#include <cpu/samsung/exynos5250/power.h>
#include <cpu/samsung/exynos5250/spi.h>

#include <cpu/samsung/exynos5-common/cpu.h>
#include <cpu/samsung/exynos5-common/exynos5-common.h>

//#include <asm/system.h>
#include <system.h>

#include <arch/io.h>

#define OM_STAT		(0x1f << 1)

/**
 * Copy data from SD or MMC device to RAM.
 *
 * @param offset	Block offset of the data
 * @param nblock	Number of blocks
 * @param dst		Destination address
 * @return 1 = True or 0 = False
 */
typedef u32 (*mmc_copy_func_t)(u32 offset, u32 nblock, u32 dst);

/**
 * Copy data from SPI flash to RAM.
 *
 * @param offset	Block offset of the data
 * @param nblock	Number of blocks
 * @param dst		Destination address
 * @return 1 = True or 0 = False
 */
typedef u32 (*spi_copy_func_t)(u32 offset, u32 nblock, u32 dst);


/**
 * Copy data through USB.
 *
 * @return 1 = True or 0 = False
 */
typedef u32 (*usb_copy_func_t)(void);

/*
 * Set/clear program flow prediction and return the previous state.
 */
static int config_branch_prediction(int set_cr_z)
{
	unsigned int cr;

	/* System Control Register: 11th bit Z Branch prediction enable */
	cr = get_cr();
	set_cr(set_cr_z ? cr | CR_Z : cr & ~CR_Z);

	return cr & CR_Z;
}

/* Copy U-Boot image to RAM */
static void copy_uboot_to_ram(void)
{
	unsigned int sec_boot_check;
	unsigned int uboot_size = CONFIG_COREBOOT_ROMSIZE_KB_4096;
	int is_cr_z_set;
	enum boot_mode boot_mode = BOOT_MODE_OM;
	mmc_copy_func_t mmc_copy;

	usb_copy_func_t usb_copy;

#if 0
	uboot_size = exynos_get_uboot_size();
	boot_mode = exynos_get_boot_device();
#endif

	if (boot_mode == BOOT_MODE_OM) {
		/* Read iRAM location to check for secondary USB boot mode */
		sec_boot_check = readl(EXYNOS_IRAM_SECONDARY_BASE);
		if (sec_boot_check == EXYNOS_USB_SECONDARY_BOOT)
			boot_mode = BOOT_MODE_USB;
	}
	debug("U-Boot size %u\n", uboot_size);

	if (boot_mode == BOOT_MODE_OM)
		boot_mode = readl(EXYNOS_POWER_BASE) & OM_STAT;

	switch (boot_mode) {
#if defined(CONFIG_EXYNOS_SPI_BOOT)
	case BOOT_MODE_SERIAL:
		/* let us our own function to copy u-boot from SF */
		exynos_spi_copy(uboot_size);
		break;
#endif
	case BOOT_MODE_MMC:
		mmc_copy = *(mmc_copy_func_t *)EXYNOS_COPY_MMC_FNPTR_ADDR;
		assert(!(uboot_size & 511));
		mmc_copy(BL2_START_OFFSET, uboot_size / 512,
				CONFIG_SYS_TEXT_BASE);
		break;
	case BOOT_MODE_USB:
		/*
		 * iROM needs program flow prediction to be disabled
		 * before copy from USB device to RAM
		 */
		is_cr_z_set = config_branch_prediction(0);
		usb_copy = *(usb_copy_func_t *)
				EXYNOS_COPY_USB_FNPTR_ADDR;
		usb_copy();
		config_branch_prediction(is_cr_z_set);
		break;
	default:
		printk(BIOS_ERR, "Invalid boot mode selection\n");
		hlt();
		break;
	}
	debug("U-Boot copied\n");
}

/* Board-specific call to see if wakeup is allowed. */
static int __def_board_wakeup_permitted(void)
{
	return 1;
}
int board_wakeup_permitted(void)
	__attribute__((weak, alias("__def_board_wakeup_permitted")));

