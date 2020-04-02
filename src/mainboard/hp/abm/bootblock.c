/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <amdblocks/acpimmio.h>
#include <bootblock_common.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct5104d/nct5104d.h>

#define SERIAL_DEV PNP_DEV(0x4E, NCT5104D_SP4)

void bootblock_mainboard_early_init(void)
{
	u32 reg32;

	/* Set auxiliary output clock frequency on OSCOUT1 pin to be 25MHz */
	/* Set auxiliary output clock frequency on OSCOUT2 pin to be 48MHz */
	reg32 = misc_read32(0x28);
	reg32 &= 0xffc0ffff; // Clr bits [21:19] & [18:16]
	reg32 |= 0x00010000; // Set bit 16 for 25MHz
	misc_write32(0x28, reg32);

	/* Enable Auxiliary OSCOUT1/OSCOUT2 */
	reg32 = misc_read32(0x40);
	reg32 &= 0xffffff7b; // clear 2, 7
	misc_write32(0x40, reg32);

	nct5104d_enable_uartd(SERIAL_DEV);
	nuvoton_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
