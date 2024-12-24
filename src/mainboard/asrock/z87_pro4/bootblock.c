/* SPDX-License-Identifier: GPL-2.0-only */

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

	/* Select SIO pin mux states */
	pnp_write_config(GLOBAL_DEV, 0x24, 0x5c);
	pnp_write_config(GLOBAL_DEV, 0x27, 0xd0);
	pnp_write_config(GLOBAL_DEV, 0x2a, 0x60);
	pnp_write_config(GLOBAL_DEV, 0x2c, 0x80);
	pnp_write_config(GLOBAL_DEV, 0x2f, 0x01);

	/* Power RAM in S3 and let the PCH handle power failure actions */
	pnp_set_logical_device(ACPI_DEV);
	pnp_write_config(ACPI_DEV, 0xe4, 0x70);

	nuvoton_pnp_exit_conf_state(GLOBAL_DEV);

	/* Enable UART */
	nuvoton_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
