/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pnp_ops.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6791d/nct6791d.h>

#define GLOBAL_DEV	PNP_DEV(0x2e, 0)
#define SERIAL_DEV	PNP_DEV(0x2e, NCT6791D_SP1)
#define ACPI_DEV	PNP_DEV(0x2e, NCT6791D_ACPI)

void mainboard_config_superio(void)
{
	nuvoton_pnp_enter_conf_state(GLOBAL_DEV);

	/* Select SIO pin mux states */
	pnp_write_config(GLOBAL_DEV, 0x1b, 0xf0);
	pnp_write_config(GLOBAL_DEV, 0x24, 0xfc);
	pnp_write_config(GLOBAL_DEV, 0x2a, 0x40);
	pnp_write_config(GLOBAL_DEV, 0x2b, 0x20);
	pnp_write_config(GLOBAL_DEV, 0x2c, 0x00);

	/* Power RAM in S3 */
	pnp_set_logical_device(ACPI_DEV);
	pnp_write_config(ACPI_DEV, 0xe4, 0x10);

	nuvoton_pnp_exit_conf_state(GLOBAL_DEV);

	nuvoton_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
