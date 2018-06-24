/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Angel Pons <th3fanbus@gmail.com>
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

#include <northbridge/intel/sandybridge/raminit_native.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8728f/it8728f.h>

#define SUPERIO_GPIO PNP_DEV(0x2e, IT8728F_GPIO)
#define SERIAL_DEV PNP_DEV(0x2e, 0x01)

void pch_enable_lpc(void)
{
	pci_write_config16(PCH_LPC_DEV, LPC_EN, KBC_LPC_EN |
			CNF1_LPC_EN | CNF2_LPC_EN | COMA_LPC_EN);

	pci_write_config16(PCH_LPC_DEV, LPC_IO_DEC, 0x10);
}

void mainboard_rcba_config(void)
{
}

const struct southbridge_usb_port mainboard_usb_ports[] = {
	{ 1, 0, 0 },
	{ 1, 0, 0 },
	{ 1, 0, 1 },
	{ 1, 0, 1 },
	{ 1, 0, 2 },
	{ 1, 0, 2 },
	{ 1, 0, 3 },
	{ 1, 0, 3 },
	{ 1, 0, 4 },
	{ 1, 0, 4 },
	{ 1, 0, 6 },
	{ 1, 0, 5 },
	{ 1, 0, 5 },
	{ 1, 0, 6 },
};

void mainboard_early_init(int s3resume)
{
}

void mainboard_config_superio(void)
{
	/* Enable serial port */
	ite_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);

	/* Disable SIO WDT which kicks in DualBIOS */
	ite_reg_write(SUPERIO_GPIO, 0xEF, 0x7E);
}

void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
	read_spd(&spd[0], 0x50, id_only);
	read_spd(&spd[2], 0x52, id_only);
}
