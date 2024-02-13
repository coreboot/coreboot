/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pnp_ops.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6776/nct6776.h>
#include <southbridge/intel/lynxpoint/pch.h>

void mainboard_config_superio(void)
{
	const pnp_devfn_t GLOBAL_PSEUDO_DEV = PNP_DEV(0x2e, 0);
	const pnp_devfn_t SERIAL_DEV = PNP_DEV(0x2e, NCT6776_SP1);
	const pnp_devfn_t ACPI_DEV = PNP_DEV(0x2e, NCT6776_ACPI);

	nuvoton_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);

	nuvoton_pnp_enter_conf_state(GLOBAL_PSEUDO_DEV);

	/* Power RAM in S3 and let the PCH handle power failure actions. */
	pnp_set_logical_device(ACPI_DEV);
	pnp_write_config(ACPI_DEV, 0xe4, 0x70);

	nuvoton_pnp_exit_conf_state(GLOBAL_PSEUDO_DEV);
}
