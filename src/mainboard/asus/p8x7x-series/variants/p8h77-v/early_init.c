/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <device/pnp_ops.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6779d/nct6779d.h>

#define GLOBAL_DEV PNP_DEV(CONFIG_SUPERIO_PNP_BASE, 0)
#define SERIAL_DEV PNP_DEV(CONFIG_SUPERIO_PNP_BASE, NCT6779D_SP1)
#define ACPI_DEV   PNP_DEV(CONFIG_SUPERIO_PNP_BASE, NCT6779D_ACPI)

void bootblock_mainboard_early_init(void)
{
	nuvoton_pnp_enter_conf_state(GLOBAL_DEV);

	/* Select SIO pin states */
	pnp_write_config(GLOBAL_DEV, 0x1a, 0x02);
	pnp_write_config(GLOBAL_DEV, 0x1b, 0x70);
	pnp_write_config(GLOBAL_DEV, 0x1c, 0x10);
	pnp_write_config(GLOBAL_DEV, 0x1d, 0x0e);
	pnp_write_config(GLOBAL_DEV, 0x22, 0xd7);
	pnp_write_config(GLOBAL_DEV, 0x2a, 0x48);
	pnp_write_config(GLOBAL_DEV, 0x2c, 0x00);

	/* Power RAM in S3 */
	pnp_set_logical_device(ACPI_DEV);
	pnp_write_config(ACPI_DEV, 0xe4, 0x10);

	nuvoton_pnp_exit_conf_state(GLOBAL_DEV);

	/* Enable UART */
	nuvoton_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
