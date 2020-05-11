/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pnp_ops.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6779d/nct6779d.h>

#define GLOBAL_DEV PNP_DEV(0x4e, 0)
#define SERIAL_DEV PNP_DEV(0x4e, NCT6779D_SP1)
#define ACPI_DEV   PNP_DEV(0x4e, NCT6779D_ACPI)

void mainboard_config_superio(void)
{
	nuvoton_pnp_enter_conf_state(GLOBAL_DEV);

	/* Select SIO pin mux states */
	pnp_write_config(GLOBAL_DEV, 0x1a, 0x30);
	pnp_write_config(GLOBAL_DEV, 0x1b, 0x70);
	pnp_write_config(GLOBAL_DEV, 0x1c, 0x08);
	pnp_write_config(GLOBAL_DEV, 0x1d, 0x00);
	pnp_write_config(GLOBAL_DEV, 0x22, 0xff);
	pnp_write_config(GLOBAL_DEV, 0x24, 0x04);
	pnp_write_config(GLOBAL_DEV, 0x2a, 0x08);
	pnp_write_config(GLOBAL_DEV, 0x2c, 0x00);

	/* Enable keyboard wakeup, 3VSBSW# is not connected */
	pnp_set_logical_device(ACPI_DEV);
	pnp_write_config(ACPI_DEV, 0xe4, 0x08);

	nuvoton_pnp_exit_conf_state(GLOBAL_DEV);

	nuvoton_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
