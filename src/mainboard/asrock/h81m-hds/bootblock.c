/*
 * This file is part of the coreboot project.
 *
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

#include <device/pnp_ops.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6776/nct6776.h>
#include <southbridge/intel/lynxpoint/pch.h>

void mainboard_config_superio(void)
{
	const pnp_devfn_t GLOBAL_PSEUDO_DEV = PNP_DEV(0x2e, 0);
	const pnp_devfn_t SERIAL_DEV = PNP_DEV(0x2e, NCT6776_SP1);
	const pnp_devfn_t ACPI_DEV = PNP_DEV(0x2e, NCT6776_ACPI);
	const pnp_devfn_t IR_DEV = PNP_DEV(0x2e, NCT6776_SP2);

	nuvoton_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);

	nuvoton_pnp_enter_conf_state(GLOBAL_PSEUDO_DEV);

	/* Select HWM/LED functions instead of floppy functions. */
	pnp_write_config(GLOBAL_PSEUDO_DEV, 0x1c, 0x03);
	pnp_write_config(GLOBAL_PSEUDO_DEV, 0x24, 0x24);

	/* Power RAM in S3 and let the PCH handle power failure actions. */
	pnp_set_logical_device(ACPI_DEV);
	pnp_write_config(ACPI_DEV, 0xe4, 0x70);

	/*
	 * Don't know what's needed here, just set the same as the vendor
	 * firmware.
	 */
	pnp_set_logical_device(IR_DEV);
	pnp_write_config(IR_DEV, 0xf1, 0x5c);

	nuvoton_pnp_exit_conf_state(GLOBAL_PSEUDO_DEV);
}
