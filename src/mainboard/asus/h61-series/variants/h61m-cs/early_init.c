/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <device/pnp_ops.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6779d/nct6779d.h>

#define SIO_PORT	0x2e
#define SIO_DEV		PNP_DEV(SIO_PORT, 0)
#define ACPI_DEV	PNP_DEV(SIO_PORT, NCT6779D_ACPI)

void bootblock_mainboard_early_init(void)
{
	nuvoton_pnp_enter_conf_state(SIO_DEV);
	pnp_set_logical_device(ACPI_DEV);
	pnp_write_config(ACPI_DEV, 0xe4, 0x10);
	nuvoton_pnp_exit_conf_state(SIO_DEV);
}
