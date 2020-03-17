/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdint.h>
#include <device/pci_ops.h>
#include <southbridge/intel/ibexpeak/pch.h>
#include <drivers/lenovo/hybrid_graphics/hybrid_graphics.h>
#include <northbridge/intel/ironlake/ironlake.h>

const struct southbridge_usb_port mainboard_usb_ports[] = {
	/* Enabled, Current table lookup index, OC map */
	{ 1, IF1_557, 0 },
	{ 1, IF1_55F, 1 },
	{ 1, IF1_74B, 3 },
	{ 1, IF1_14B, 3 },
	{ 1, IF1_14B, 3 },
	{ 1, IF1_74B, 3 },
	{ 1, IF1_74B, 3 },
	{ 1, IF1_74B, 3 },
	{ 1, IF1_55F, 4 },
	{ 1, IF1_55F, 5 },
	{ 1, IF1_74B, 7 },
	{ 1, IF1_74B, 7 },
	{ 1, IF1_557, 7 },
	{ 1, IF1_55F, 7 },
};

static void hybrid_graphics_init(void)
{
	bool peg, igd;
	u32 reg32;

	early_hybrid_graphics(&igd, &peg);

	/* Hide disabled devices */
	reg32 = pci_read_config32(PCI_DEV(0, 0, 0), D0F0_DEVEN);
	reg32 &= ~(DEVEN_PEG10 | DEVEN_IGD);

	if (peg)
		reg32 |= DEVEN_PEG10;

	if (igd)
		reg32 |= DEVEN_IGD;
	else
		/* Disable IGD VGA decode, no GTT or GFX stolen */
		pci_write_config16(PCI_DEV(0, 0, 0), D0F0_GGC, 2);

	pci_write_config32(PCI_DEV(0, 0, 0), D0F0_DEVEN, reg32);
}

void mainboard_pre_raminit(void)
{
	hybrid_graphics_init();
}

void mainboard_get_spd_map(u8 *spd_addrmap)
{
	spd_addrmap[0] = 0x50;
	spd_addrmap[2] = 0x51;
}
