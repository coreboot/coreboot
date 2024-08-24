/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <device/pnp_ops.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6776/nct6776.h>

#define GLOBAL_DEV PNP_DEV(0x2e, 0)
#define SERIAL_DEV PNP_DEV(0x2e, NCT6776_SP1)
#define ACPI_DEV PNP_DEV(0x2e, NCT6776_ACPI)

void bootblock_mainboard_early_init(void)
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
