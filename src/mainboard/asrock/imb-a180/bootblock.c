/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpimmio.h>
#include <bootblock_common.h>
#include <device/pnp_type.h>
#include <superio/winbond/common/winbond.h>
#include <superio/winbond/w83627uhg/w83627uhg.h>

#define SERIAL_DEV PNP_DEV(0x2e, W83627UHG_SP1)

void bootblock_mainboard_early_init(void)
{
	/* Disable PCI-PCI bridge and release GPIO32/33 for other uses. */
	pm_write8(0xea, 0x1);

	/* Set auxiliary output clock frequency on OSCOUT1 pin to be 48MHz */
	misc_write32(0x28, misc_read32(0x28) & 0xfff8ffff);

	/* Enable Auxiliary Clock1, disable FCH 14 MHz OscClk */
	misc_write32(0x40, misc_read32(0x40) & 0xffffbffb);

	/* w83627uhg has a default clk of 48MHz, p.9 of data-sheet */
	winbond_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
