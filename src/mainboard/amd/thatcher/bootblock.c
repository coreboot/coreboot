/*
 * This file is part of the coreboot project.
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
#include <console/console.h>
#include <superio/smsc/lpc47n217/lpc47n217.h>

#define SERIAL_DEV PNP_DEV(0x2e, LPC47N217_SP1)

void bootblock_mainboard_early_init(void)
{
	post_code(0x30);
	post_code(0x31);

	gpio_100_write8(0x1, 0x98);

	/* Disable PCI-PCI bridge and release GPIO32/33 for other uses. */
	pm_write8(0xea, 0x1);

	lpc47n217_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
