/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017–2018 Tristan Corrick <tristan@corrick.kiwi>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <device/dram/ddr3.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6776/nct6776.h>

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
	{ 1, 0, 5 },
	{ 1, 0, 5 },
	{ 1, 0, 6 },
	{ 1, 0, 6 },
};

void pch_enable_lpc(void)
{
	pci_or_config16(PCH_LPC_DEV, LPC_EN, CNF1_LPC_EN | KBC_LPC_EN);
}

void mainboard_rcba_config(void)
{
}

void mainboard_early_init(int s3resume)
{
}

void mainboard_config_superio(void)
{
	static const pnp_devfn_t GLOBAL_PSEUDO_DEV = PNP_DEV(0x2e, 0);
	static const pnp_devfn_t ACPI_DEV = PNP_DEV(0x2e, NCT6776_ACPI);

	nuvoton_pnp_enter_conf_state(GLOBAL_PSEUDO_DEV);

	/* Select HWM/LED functions instead of floppy functions. */
	pnp_write_config(GLOBAL_PSEUDO_DEV, 0x1c, 0x03);
	pnp_write_config(GLOBAL_PSEUDO_DEV, 0x24, 0x24);

	/* Power RAM in S3. */
	pnp_set_logical_device(ACPI_DEV);
	pnp_write_config(ACPI_DEV, 0xe4, 0x10);

	nuvoton_pnp_exit_conf_state(GLOBAL_PSEUDO_DEV);
}

void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
	read_spd(&spd[0], 0x50, id_only);
	read_spd(&spd[1], 0x51, id_only);
	read_spd(&spd[2], 0x52, id_only);
	read_spd(&spd[3], 0x53, id_only);
}
