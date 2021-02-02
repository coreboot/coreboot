/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpimmio.h>
#include <bootblock_common.h>
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
