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

#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <ec/lenovo/pmh7/pmh7.h>
#include <drivers/lenovo/hybrid_graphics/hybrid_graphics.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <device/device.h>

static void hybrid_graphics_init(void)
{
	bool peg, igd;
	u32 reg32;

	early_hybrid_graphics(&igd, &peg);

	if (peg && igd)
		return;

	/* Hide disabled devices */
	reg32 = pci_read_config32(PCI_DEV(0, 0, 0), DEVEN);
	reg32 &= ~(DEVEN_PEG10 | DEVEN_IGD);

	if (peg)
		reg32 |= DEVEN_PEG10;

	if (igd)
		reg32 |= DEVEN_IGD;
	else
		/* Disable IGD VGA decode, no GTT or GFX stolen */
		pci_write_config16(PCI_DEV(0, 0, 0), GGC, 2);

	pci_write_config32(PCI_DEV(0, 0, 0), DEVEN, reg32);
}

/* FIXME: used T530 values here */
const struct southbridge_usb_port mainboard_usb_ports[] = {
	{ 1, 1,  0 },
	{ 1, 1,  1 },
	{ 1, 2,  3 },
	{ 1, 1, -1 },
	{ 1, 1,  2 },
	{ 1, 0, -1 },
	{ 0, 0, -1 },
	{ 1, 2, -1 },
	{ 1, 0, -1 },
	{ 1, 1,  5 },
	{ 1, 0, -1 },
	{ 1, 0, -1 },
	{ 1, 3, -1 },
	{ 1, 1, -1 },
};

void mainboard_early_init(int s3resume)
{
	hybrid_graphics_init();
}

void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
	read_spd(&spd[0], 0x50, id_only);
	read_spd(&spd[2], 0x51, id_only);
}
