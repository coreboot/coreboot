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

#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <timestamp.h>
#include <arch/byteorder.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <pc80/mc146818rtc.h>
#include <arch/acpi.h>
#include <cbmem.h>
#include <console/console.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
#include <southbridge/intel/common/rcba.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/common/gpio.h>
#include <arch/cpu.h>
#include <cpu/x86/msr.h>
#include <cbfs.h>

void pch_enable_lpc(void)
{
	/* X230 EC Decode Range Port60/64, Port62/66 */
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
	{ 1, 0, 0 }, /* P0 (left, fan side), OC 0 */
	{ 1, 0, 1 }, /* P1 (left touchpad side), OC 1 */
	{ 1, 1, 3 }, /* P2: dock, OC 3 */
	{ 1, 1, -1 }, /* P3: wwan, no OC */
	{ 1, 1, -1 }, /* P4: Wacom tablet on X230t, otherwise empty */
	{ 1, 1, -1 }, /* P5: Expresscard, no OC */
	{ 0, 0, -1 }, /* P6: Empty */
	{ 1, 2, -1 }, /* P7: dock, no OC */
	{ 1, 0, -1 },
	{ 1, 2, 5 }, /* P9: Right (EHCI debug), OC 5 */
	{ 1, 1, -1 }, /* P10: fingerprint reader, no OC */
	{ 1, 1, -1 }, /* P11: bluetooth, no OC. */
	{ 1, 1, -1 }, /* P12: wlan, no OC */
	{ 1, 1, -1 }, /* P13: webcam, no OC */
};

void mainboard_get_spd(spd_raw_data *spd, bool id_only) {
	read_spd (&spd[0], 0x50, id_only);
	read_spd (&spd[2], 0x51, id_only);
}

void mainboard_early_init(int s3resume) {
}

void mainboard_config_superio(void)
{
}
