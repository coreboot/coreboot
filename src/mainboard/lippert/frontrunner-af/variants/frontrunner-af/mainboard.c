/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdlib.h>
#include <amdblocks/acpimmio.h>
#include <console/console.h>
#include <device/device.h>
#include <arch/io.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <southbridge/amd/cimx/sb800/SBPLATFORM.h>
#include <vendorcode/amd/cimx/sb800/OEM.h> /* SMBUS0_BASE_ADDRESS */
#include <southbridge/amd/cimx/sb800/gpio_oem.h>
#include "sema.h"

/* Init SIO GPIOs. */
#define SIO_RUNTIME_BASE 0x0E00
static const u16 sio_init_table[] = { // hi = offset, lo = value
	0x4BA0, // GP1x: COM1/2 control   = RS232, no term, max 115200
	0x2300, // GP10: COM1 termination = push/pull output
	0x2400, // GP11: COM2 termination = push/pull output
	0x2500, // GP12: COM1 RS485 mode  = push/pull output
	0x2600, // GP13: COM2 RS485 mode  = push/pull output
	0x2700, // GP14: COM1 speed A     = push/pull output
	0x2900, // GP15: COM1 speed B     = push/pull output
	0x2A00, // GP16: COM2 speed A     = push/pull output
	0x2B00, // GP17: COM2 speed B     = push/pull output

	0x3904, // GP36                   = KBDRST# function

	0x4E74, // GP4x: Ethernet enable  = on
	0x6E84, // GP44: Ethernet enable  = open drain output

	// GP5x = COM2 function instead of GPIO
	0x3F05, 0x4005, 0x4105, 0x4204, 0x4305, 0x4404, 0x4505, 0x4604,

	0x470C, // GP60                   = WDT function
	0x5E00, // LED2: Live LED         = off
	0x4884, // GP61: Live LED         = LED2 function

	0x5038, // GP6x: USB power        = 3x on
	0x5580, // GP63: USB power 0/1    = open drain output
	0x5680, // GP64: USB power 2/3    = open drain output
	0x5780, // GP65: USB power 4/5    = open drain output
};

static void init(struct device *dev)
{
	volatile u8 *spi_base;	/* base addr of Hudson's SPI host controller */
	int i;
	printk(BIOS_DEBUG, CONFIG_MAINBOARD_PART_NUMBER " ENTER %s\n", __func__);

	/* Init Hudson GPIOs. */
	printk(BIOS_DEBUG, "Init FCH GPIOs @ 0x%08x\n", ACPI_MMIO_BASE+GPIO_BASE);
	/* GPIO50: FCH_ARST#_GATE resets stuck PCIe devices */
	iomux_write8(50, 2);
	/* output set to 1 as it's never needed */
	iomux_write8(50, 0xc0);
	/* GPIO197: BIOS_DEFAULTS# = input (int. PU) */
	iomux_write8(197, 2);
	/* GPIO58-56: REV_ID2-0 */
	iomux_write8(56, 1);
	/* inputs, disable int. pull-ups */
	gpio_100_write8(56, 0x28);
	iomux_write8(57, 1);
	gpio_100_write8(57, 0x28);
	iomux_write8(58, 1);
	gpio_100_write8(58, 0x28);
	/* "Gpio96": GEVENT0# signal on X2 connector (int. PU) */
	iomux_write8(96, 1);
	/* GPIO52,61,62,187-192 free to use on X2 connector */
	iomux_write8(52, 1);
	/* default to inputs with int. PU */
	iomux_write8(61, 2);
	iomux_write8(62, 2);
	iomux_write8(187, 2);
	iomux_write8(188, 2);
	iomux_write8(189, 1);
	iomux_write8(190, 1);
	iomux_write8(191, 1);
	iomux_write8(192, 1);

	/* just in case anyone cares */
	if (!fch_gpio_state(197))
		printk(BIOS_INFO, "BIOS_DEFAULTS jumper is present.\n");
	printk(BIOS_INFO, "Board revision ID: %u\n",
	       fch_gpio_state(58)<<2 | fch_gpio_state(57)<<1 | fch_gpio_state(56));

	/* Init SIO GPIOs. */
	printk(BIOS_DEBUG, "Init SIO GPIOs @ 0x%04x\n", SIO_RUNTIME_BASE);
	for (i = 0; i < ARRAY_SIZE(sio_init_table); i++) {
		u16 val = sio_init_table[i];
		outb((u8)val, SIO_RUNTIME_BASE + (val >> 8));
	}

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

	printk(BIOS_DEBUG, CONFIG_MAINBOARD_PART_NUMBER " EXIT %s\n", __func__);
}

/**********************************************
 * Enable the dedicated functions of the board.
 **********************************************/
static void mainboard_enable(struct device *dev)
{
	printk(BIOS_INFO, "Mainboard " CONFIG_MAINBOARD_PART_NUMBER " Enable.\n");
	dev->ops->init = init;

	/* enable GPP CLK0 */
	/* disable GPP CLK1 thru SLT_GFX_CLK */
	misc_write8(0, 0x0f);
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
