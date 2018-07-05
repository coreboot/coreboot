/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2014 Vladimir Serbinenko
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

#include <arch/io.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <ec/hp/kbc1126/ec.h>

void pch_enable_lpc(void)
{
	/*
	 * CNF2 and CNF1 for Super I/O
	 * MC and LPC (0x60,0x64,0x62,0x66) for KBC and EC
	 */
	pci_write_config16(PCH_LPC_DEV, LPC_EN,
			CNF2_LPC_EN | CNF1_LPC_EN | MC_LPC_EN | KBC_LPC_EN);
	pci_write_config16(PCH_LPC_DEV, LPC_IO_DEC, 0x0010);
	/* Enable mailbox at 0x200/0x201 and PM1 at 0x220 */
	pci_write_config32(PCH_LPC_DEV, LPC_GEN1_DEC, 0x007c0201);
}

void mainboard_rcba_config(void)
{
}

const struct southbridge_usb_port mainboard_usb_ports[] = {
	{ 1, 1, 0 }, /* SSP1: dock */
	{ 1, 1, 0 }, /* SSP2: left, EHCI Debug */
	{ 1, 1, 1 }, /* SSP3: right back side */
	{ 1, 1, 1 }, /* SSP4: right front side */
	{ 1, 0, 2 }, /* B0P5 */
	{ 1, 0, 2 }, /* B0P6: wlan USB */
	{ 0, 0, 3 }, /* B0P7 */
	{ 1, 1, 3 }, /* B0P8: smart card reader */
	{ 1, 1, 4 }, /* B1P1: fingerprint reader */
	{ 0, 0, 4 }, /* B1P2: (EHCI Debug, not connected) */
	{ 1, 1, 5 }, /* B1P3: Camera */
	{ 0, 0, 5 }, /* B1P4 */
	{ 1, 1, 6 }, /* B1P5: wwan USB */
	{ 0, 0, 6 }, /* B1P6 */
};

void mainboard_early_init(int s3resume)
{
}

void mainboard_config_superio(void)
{
	kbc1126_enter_conf();
	kbc1126_mailbox_init();
	kbc1126_kbc_init();
	kbc1126_ec_init();
	kbc1126_pm1_init();
	kbc1126_exit_conf();
}

void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
	read_spd(&spd[0], 0x50, id_only);
	read_spd(&spd[2], 0x52, id_only);
}
