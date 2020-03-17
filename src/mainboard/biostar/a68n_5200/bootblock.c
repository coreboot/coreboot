/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <amdblocks/acpimmio.h>
#include <bootblock_common.h>
#include <stdint.h>
#include <device/pci_ops.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8728f/it8728f.h>

#define SERIAL_DEV PNP_DEV(0x2e, IT8728F_SP1)
#define GPIO_DEV PNP_DEV(0x2e, IT8728F_GPIO)
#define CLKIN_DEV PNP_DEV(0x2e, IT8728F_GPIO)

static void sbxxx_enable_48mhzout(void)
{
	u32 reg32;

	/* Set auxiliary output clock frequency on OSCOUT1 pin to be 48MHz */
	reg32 = misc_read32(0x28);
	reg32 &= 0xfff8ffff;
	misc_write32(0x28, reg32);

	/* Enable Auxiliary Clock1, disable FCH 14 MHz OscClk */
	reg32 = misc_read32(0x40);
	reg32 &= 0xffffbffb;
	misc_write32(0x40, reg32);
}

void bootblock_mainboard_early_init(void)
{
	u8 byte;

	/* Enable the AcpiMmio space */
	pm_io_write8(0x24, 1);

	/* Set LPC decode enables. */
	pci_devfn_t dev = PCI_DEV(0, 0x14, 3);
	pci_write_config32(dev, 0x44, 0xff03ffd5);

	/* enable SIO LPC decode */
	byte = pci_read_config8(dev, 0x48);
	byte |= 3;	/* 2e, 2f */
	pci_write_config8(dev, 0x48, byte);

	/* enable serial decode */
	byte = pci_read_config8(dev, 0x44);
	byte |= (1 << 6);  /* 0x3f8 */
	pci_write_config8(dev, 0x44, byte);

	/* enable SIO clock */
	sbxxx_enable_48mhzout();

	/* Enable serial output on it8728f */
	ite_conf_clkin(CLKIN_DEV, ITE_UART_CLK_PREDIVIDE_48);
	ite_kill_watchdog(GPIO_DEV);
	ite_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
