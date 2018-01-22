/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Iru Cai <mytbk920423@gmail.com>
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
#include <superio/smsc/lpc47n217/lpc47n217.h>
#include <ec/hp/kbc1126/ec.h>

#define SERIAL_DEV PNP_DEV(0x4e, LPC47N217_SP1)

void pch_enable_lpc(void)
{
	/*
	 * CNF2 and CNF1 for Super I/O
	 * MC and LPC (0x60,0x64,0x62,0x66) for KBC and EC
	 * Enable parallel port and serial port
	 */
	pci_write_config16(PCH_LPC_DEV, LPC_EN,
			CNF2_LPC_EN | CNF1_LPC_EN | MC_LPC_EN | KBC_LPC_EN |
			LPT_LPC_EN | COMA_LPC_EN);
	pci_write_config16(PCH_LPC_DEV, LPC_IO_DEC, 0x0010);
	/* Enable mailbox at 0x200/0x201 and PM1 at 0x220 */
	pci_write_config32(PCH_LPC_DEV, LPC_GEN1_DEC, 0x007c0201);
}

void mainboard_rcba_config(void)
{
}

const struct southbridge_usb_port mainboard_usb_ports[] = {
	{ 1, 1, 0 }, /* USB0, eSATA */
	{ 1, 0, 0 }, /* USB charger */
	{ 0, 1, 1 },
	{ 1, 1, 1 }, /* camera */
	{ 1, 0, 2 }, /* USB4 expresscard */
	{ 1, 0, 2 }, /* bluetooth */
	{ 0, 0, 3 },
	{ 1, 0, 3 }, /* smartcard */
	{ 1, 1, 4 }, /* fingerprint */
	{ 1, 1, 4 }, /* WWAN */
	{ 1, 0, 5 }, /* CONN */
	{ 1, 0, 5 }, /* docking */
	{ 1, 0, 6 }, /* CONN */
	{ 1, 0, 6 }, /* docking */
};

void mainboard_early_init(int s3resume)
{
}

void mainboard_config_superio(void)
{
	lpc47n217_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
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
