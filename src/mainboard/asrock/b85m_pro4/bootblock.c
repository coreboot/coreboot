/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2010 coresystems GmbH
 * Copyright (C) 2012 Google Inc.
 * Copyright (C) 2018 Tristan Corrick <tristan@corrick.kiwi>
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

#define GLOBAL_DEV PNP_DEV(0x2e, 0)
#define SERIAL_DEV PNP_DEV(0x2e, NCT6776_SP1)
#define ACPI_DEV   PNP_DEV(0x2e, NCT6776_ACPI)

void mainboard_config_superio(void)
{
	nuvoton_pnp_enter_conf_state(GLOBAL_DEV);

	/* Select HWM/LED functions instead of floppy functions */
	pnp_write_config(GLOBAL_DEV, 0x1c, 0x03);
	pnp_write_config(GLOBAL_DEV, 0x24, 0x24);

	/* Power RAM in S3 and let the PCH handle power failure actions */
	pnp_set_logical_device(ACPI_DEV);
	pnp_write_config(ACPI_DEV, 0xe4, 0x70);

	nuvoton_pnp_exit_conf_state(GLOBAL_DEV);

	/* Enable UART */
	nuvoton_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
