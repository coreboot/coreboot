/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <device/pnp_ops.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6776/nct6776.h>

#define GLOBAL_DEV PNP_DEV(0x2e, 0)
#define SERIAL_DEV PNP_DEV(0x2e, NCT6776_SP1)
#define ACPI_DEV   PNP_DEV(0x2e, NCT6776_ACPI)

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
	{ 1, 0, 6 },
	{ 1, 0, 5 },
	{ 1, 0, 5 },
	{ 1, 0, 6 },
};

void bootblock_mainboard_early_init(void)
{
	nuvoton_pnp_enter_conf_state(GLOBAL_DEV);

	/* Select SIO pin states */
	pnp_write_config(GLOBAL_DEV, 0x1a, 0xc8);
	pnp_write_config(GLOBAL_DEV, 0x1b, 0x6d);
	pnp_write_config(GLOBAL_DEV, 0x1c, 0x83);
	pnp_write_config(GLOBAL_DEV, 0x24, 0x24);
	pnp_write_config(GLOBAL_DEV, 0x2a, 0x00);
	pnp_write_config(GLOBAL_DEV, 0x2b, 0x02);
	pnp_write_config(GLOBAL_DEV, 0x2c, 0x80);

	/* Power RAM in S3 */
	pnp_set_logical_device(ACPI_DEV);
	pnp_write_config(ACPI_DEV, 0xe4, 0x10);

	pnp_set_logical_device(SERIAL_DEV);

	nuvoton_pnp_exit_conf_state(GLOBAL_DEV);

	/* Enable UART */
	nuvoton_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}

void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
	read_spd(&spd[0], 0x50, id_only);
	read_spd(&spd[1], 0x51, id_only);
	read_spd(&spd[2], 0x52, id_only);
	read_spd(&spd[3], 0x53, id_only);
}
