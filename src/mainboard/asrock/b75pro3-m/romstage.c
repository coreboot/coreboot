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

#include <northbridge/intel/sandybridge/raminit_native.h>
#include <superio/nuvoton/nct6776/nct6776.h>
#include <superio/nuvoton/common/nuvoton.h>

#define SERIAL_DEV PNP_DEV(0x2e, NCT6776_SP1)

void pch_enable_lpc(void)
{
	pci_write_config16(PCH_LPC_DEV, LPC_EN,
			CNF1_LPC_EN | KBC_LPC_EN | LPT_LPC_EN | COMA_LPC_EN);
	pci_write_config16(PCH_LPC_DEV, LPC_IO_DEC, 0x0000);
}

void mainboard_rcba_config(void)
{
}

const struct southbridge_usb_port mainboard_usb_ports[] = {
	{ 1, 0, 0 },
	{ 1, 0, 0 },
	{ 1, 1, 1 },
	{ 1, 1, 1 },
	{ 1, 1, 2 },
	{ 1, 1, 2 },
	{ 1, 0, 3 },
	{ 1, 0, 3 },
	{ 1, 0, 4 },
	{ 1, 0, 4 },
	{ 1, 0, 6 },
	{ 1, 1, 5 },
	{ 1, 1, 5 },
	{ 1, 0, 6 },
};

void mainboard_early_init(int s3resume)
{
}

void mainboard_config_superio(void)
{
	/* Set GPIOs on superio, enable UART */
	nuvoton_pnp_enter_conf_state(SERIAL_DEV);
	pnp_set_logical_device(SERIAL_DEV);

	pnp_write_config(SERIAL_DEV, 0x1c, 0x80);
	pnp_write_config(SERIAL_DEV, 0x27, 0x80);
	pnp_write_config(SERIAL_DEV, 0x2a, 0x60);

	nuvoton_pnp_exit_conf_state(SERIAL_DEV);

	nuvoton_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}

void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
	read_spd(&spd[0], 0x50, id_only);
	read_spd(&spd[1], 0x51, id_only);
	read_spd(&spd[2], 0x52, id_only);
	read_spd(&spd[3], 0x53, id_only);
}
