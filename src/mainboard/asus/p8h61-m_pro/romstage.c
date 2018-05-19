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
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6776/nct6776.h>

#define GLOBAL_DEV PNP_DEV(0x2e, 0)
#define SERIAL_DEV PNP_DEV(0x2e, NCT6776_SP1)
#define ACPI_DEV PNP_DEV(0x2e, NCT6776_ACPI)

void pch_enable_lpc(void)
{
	/* Enable the Super IO */
	pci_write_config16(PCH_LPC_DEV, LPC_EN, CNF1_LPC_EN |
			KBC_LPC_EN | LPT_LPC_EN | COMA_LPC_EN);
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
	/* Enable UART */
	nuvoton_pnp_enter_conf_state(GLOBAL_DEV);

	/* Select SIO pin states. */
	pnp_write_config(GLOBAL_DEV, 0x1c, 0x83);
	pnp_write_config(GLOBAL_DEV, 0x24, 0x30);
	pnp_write_config(GLOBAL_DEV, 0x27, 0x40);
	pnp_write_config(GLOBAL_DEV, 0x2a, 0x20);

	/* Power RAM in S3. */
	pnp_set_logical_device(ACPI_DEV);
	pnp_write_config(ACPI_DEV, 0xe4, 0x10);

	pnp_set_logical_device(SERIAL_DEV);

	nuvoton_pnp_exit_conf_state(GLOBAL_DEV);

	nuvoton_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}

void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
	read_spd(&spd[0], 0x50, id_only);
	read_spd(&spd[2], 0x52, id_only);
}
