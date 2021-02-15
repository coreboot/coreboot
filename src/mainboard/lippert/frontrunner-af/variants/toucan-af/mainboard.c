/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpimmio.h>
#include <console/console.h>
#include <device/device.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <southbridge/amd/cimx/sb800/SBPLATFORM.h>
#include <vendorcode/amd/cimx/sb800/OEM.h> /* SMBUS0_BASE_ADDRESS */
#include <southbridge/amd/cimx/sb800/gpio_oem.h>
#include "mainboard/lippert/frontrunner-af/sema.h"

static void init(struct device *dev)
{
	volatile u8 *spi_base;	/* base addr of Hudson's SPI host controller */

	/* Init Hudson GPIOs. */
	printk(BIOS_DEBUG, "Init FCH GPIOs @ 0x%08x\n", ACPI_MMIO_BASE+GPIO_BASE);
	/* GPIO50: FCH_ARST#_GATE resets stuck PCIe devices */
	iomux_write8(50, 2);
	/* output set to 1 as it's never needed */
	iomux_write8(50, 0xc0);
	/* GPIO197: BIOS_DEFAULTS# = input (int. PU) */
	iomux_write8(197, 2);
	/* input, disable int. pull-up */
	gpio_100_write8(197, 0x28);
	/* GPIO58-56: REV_ID2-0 */
	iomux_write8(56, 1);
	/* inputs, disable int. pull-ups */
	gpio_100_write8(56, 0x28);
	iomux_write8(57, 1);
	gpio_100_write8(57, 0x28);
	iomux_write8(58, 1);
	gpio_100_write8(58, 0x28);
	/* GPIO187,188,166,GPO160: GPO0-3 on COM Express connector */
	iomux_write8(187, 2);
	/* outputs, disable PUs, default to 0 */
	gpio_100_write8(187, 0x08);
	iomux_write8(188, 2);
	gpio_100_write8(188, 0x08);
	iomux_write8(166, 2);
	gpio_100_write8(166, 0x08);
	/*
	 * needed to make GPO160 work (Hudson Register Reference
	 * section 2.3.6.1)
	 */
	pm_write8(0xdc, pm_read8(0xdc) & (~0x80));
	pm_write8(0xe6, (pm_read8(0xe6) & (~0x02)) | 1);
	iomux_write8(160, 1);
	gpio_100_write8(160, 0x08);
	/* GPIO189-192: GPI0-3 on COM Express connector */
	iomux_write8(189, 1);
	/* default to inputs with int. PU */
	iomux_write8(190, 1);
	iomux_write8(191, 1);
	iomux_write8(192, 1);

	/* just in case anyone cares */
	if (!fch_gpio_state(197))
		printk(BIOS_INFO, "BIOS_DEFAULTS jumper is present.\n");
	printk(BIOS_INFO, "Board revision ID: %u\n",
	       fch_gpio_state(58)<<2 | fch_gpio_state(57)<<1 | fch_gpio_state(56));

	/* Lower SPI speed from default 66 to 22 MHz for SST 25VF032B */
	spi_base = (u8 *)((uintptr_t)pci_read_config32(pcidev_on_root(0x14, 3),
							   0xA0) & 0xFFFFFFE0);
	/* NormSpeed in SPI_Cntrl1 register */
	spi_base[0x0D] = (spi_base[0x0D] & ~0x30) | 0x20;

	/*
	 * Notify the SMC we're alive and kicking, or after a while it will
	 * effect a power cycle and switch to the alternate BIOS chip.
	 * Should be done as late as possible.
	 * Failure here does not matter if watchdog was already disabled,
	 * by configuration or previous boot, so ignore return value.
	 */
	sema_send_alive();

}

/**********************************************
 * Enable the dedicated functions of the board.
 **********************************************/
static void mainboard_enable(struct device *dev)
{
	dev->ops->init = init;

	/* enable GPP CLK0 thru CLK1 */
	/* disable GPP CLK2 thru SLT_GFX_CLK */
	misc_write8(0, 0xff);
	misc_write8(1, 0);
	misc_write8(2, 0);
	misc_write8(3, 0);
	misc_write8(4, 0);

	/*
	 * Initialize ASF registers to an arbitrary address because someone
	 * long ago set things up this way inside the SPD read code.  The
	 * SPD read code has been made generic and moved out of the board
	 * directory, so the ASF init is being done here.
	 */
	pm_write8(0x29, 0x80);
	pm_write8(0x28, 0x61);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
