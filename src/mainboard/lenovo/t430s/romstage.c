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

#include <option.h>
#include <arch/byteorder.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <console/console.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <ec/lenovo/pmh7/pmh7.h>

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

const struct southbridge_usb_port mainboard_usb_ports[] = {
	{ 1, 0,  0 }, /* P0: , OC 0 */
	{ 1, 1,  1 }, /* P1: (EHCI debug), OC 1 */
	{ 1, 1,  3 }, /* P2:  OC 3 */
	{ 1, 0, -1 }, /* P3: no OC */
	{ 1, 2, -1 }, /* P4: no OC */
	{ 1, 1, -1 }, /* P5: no OC */
	{ 1, 1, -1 }, /* P6: no OC */
	{ 0, 1, -1 }, /* P7: empty, no OC */
	{ 1, 1, -1 }, /* P8: smart card reader, no OC */
	{ 1, 0,  5 }, /* P9:  (EHCI debug), OC 5 */
	{ 1, 0, -1 }, /* P10: fingerprint reader, no OC */
	{ 1, 1, -1 }, /* P11: bluetooth, no OC. */
	{ 0, 0, -1 }, /* P12: wlan, no OC */
	{ 1, 1, -1 }, /* P13: camera, no OC */
};

void mainboard_get_spd(spd_raw_data *spd, bool id_only) {
	read_spd(&spd[0], 0x50, id_only);
	read_spd(&spd[2], 0x51, id_only);
}

void mainboard_early_init(int s3resume) {
	u8 enable_peg;
	if (get_option(&enable_peg, "enable_dual_graphics") != CB_SUCCESS)
		enable_peg = 0;

	bool power_en = pmh7_dgpu_power_state();

	if (enable_peg != power_en)
		pmh7_dgpu_power_enable(!power_en);

	if (!enable_peg) {
		// Hide disabled dGPU device
		u32 reg32 = pci_read_config32(PCI_DEV(0, 0, 0), DEVEN);
		reg32 &= ~DEVEN_PEG10;

		pci_write_config32(PCI_DEV(0, 0, 0), DEVEN, reg32);
	}
}

void mainboard_config_superio(void)
{
}
