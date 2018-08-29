/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2010 coresystems GmbH
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
 * Copyright (C) 2014 Vladimir Serbinenko
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

#include <arch/io.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <drivers/lenovo/hybrid_graphics/hybrid_graphics.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <device/device.h>
#include <device/pci.h>

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

void pch_enable_lpc(void)
{
	/* EC Decode Range Port60/64, Port62/66 */
	/* Enable EC, PS/2 Keyboard/Mouse */
	pci_write_config16(PCH_LPC_DEV, LPC_EN,
			   CNF2_LPC_EN | CNF1_LPC_EN | MC_LPC_EN | KBC_LPC_EN);

	pci_write_config32(PCH_LPC_DEV, LPC_GEN1_DEC, 0x7c1601);
	pci_write_config32(PCH_LPC_DEV, LPC_GEN2_DEC, 0xc15e1);
	pci_write_config32(PCH_LPC_DEV, LPC_GEN4_DEC, 0x0c06a1);

	pci_write_config32(PCH_LPC_DEV, ETR3, 0x10000);
}

void mainboard_rcba_config(void)
{
	RCBA32(BUC) = 0;
}

// OC3 set in bios to port 2-7, OC7 set in bios to port 10-13
const struct southbridge_usb_port mainboard_usb_ports[] = {
	{ 1, 1,  0 }, /* P0: system port 4, OC0 */
	{ 1, 1,  1 }, /* P1: system port 2 (EHCI debug), OC 1 */
	{ 1, 1, -1 }, /* P2: HALF MINICARD (WLAN) no oc */
	{ 1, 0, -1 }, /* P3: WWAN, no OC */
	{ 1, 0, -1 }, /* P4: smartcard, no OC */
	{ 1, 1, -1 }, /* P5: ExpressCard, no OC */
	{ 0, 0, -1 }, /* P6: empty */
	{ 0, 0, -1 }, /* P7: empty */
	{ 1, 1,  4 }, /* P8: system port 3, OC4*/
	{ 1, 1,  5 }, /* P9: system port 1 (EHCI debug), OC 5 */
	{ 1, 0, -1 }, /* P10: fingerprint reader, no OC */
	{ 1, 0, -1 }, /* P11: bluetooth, no OC. */
	{ 1, 1, -1 }, /* P12: docking, no OC */
	{ 1, 1, -1 }, /* P13: camera (LCD), no OC */
};

void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
	read_spd(&spd[0], 0x50, id_only);
	read_spd(&spd[2], 0x51, id_only);
}

void mainboard_early_init(int s3resume)
{
	hybrid_graphics_init();
}

void mainboard_config_superio(void)
{
}
