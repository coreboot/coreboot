/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2014 Vladimir Serbinenko
 * Copyright (C) 2018 Patrick Rudolph <siro@das-labor.org>
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
#include <arch/byteorder.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <device/pnp_def.h>
#include <superio/nuvoton/npcd378/npcd378.h>
#include <superio/nuvoton/common/nuvoton.h>
#include "northbridge/intel/sandybridge/sandybridge.h"
#include "northbridge/intel/sandybridge/raminit_native.h"
#include "southbridge/intel/bd82x6x/pch.h"

#define SERIAL_DEV PNP_DEV(0x2e, NPCD378_SP2)

void pch_enable_lpc(void)
{
	/*
	 * Enable SuperIO, TPM, Keyboard, LPT, COMA
	 * (COMB can be equip on expansion header)
	 */
	pci_write_config16(PCH_LPC_DEV, LPC_EN,
	    CNF2_LPC_EN |CNF1_LPC_EN | KBC_LPC_EN | LPT_LPC_EN |
	    COMB_LPC_EN | COMA_LPC_EN);

	/* COMA: 3F8h, COMB: 2F8h */
	pci_write_config16(PCH_LPC_DEV, LPC_IO_DEC, 0x0010);
}

void mainboard_rcba_config(void)
{
}

const struct southbridge_usb_port mainboard_usb_ports[] = {
	{ 1, 0, -1 },
	{ 1, 0, -1 },
	{ 1, 0, -1 },
	{ 1, 0, -1 },
	{ 1, 0, -1 },
	{ 1, 0, -1 },
	{ 1, 0, -1 },
	{ 1, 0, -1 },
	{ 1, 0, -1 },
	{ 1, 0, -1 },
	{ 1, 0, -1 },
	{ 1, 0, -1 },
	{ 1, 0, -1 },
	{ 1, 0, -1 },
};

void mainboard_early_init(int s3resume)
{
}

void mainboard_config_superio(void)
{
	if (IS_ENABLED(CONFIG_CONSOLE_SERIAL))
		nuvoton_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}

void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
	/* BTX mainboard: Reversed mapping */
	read_spd(&spd[3], 0x50, id_only);
	read_spd(&spd[2], 0x51, id_only);
	read_spd(&spd[1], 0x52, id_only);
	read_spd(&spd[0], 0x53, id_only);
}
