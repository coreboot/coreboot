/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <device/pnp_ops.h>
#include <superio/nuvoton/nct6776/nct6776.h>
#include <superio/nuvoton/common/nuvoton.h>

#define SERIAL_DEV PNP_DEV(0x2e, NCT6776_SP1)

void bootblock_mainboard_early_init(void)
{
	/* Set GPIOs on superio, enable UART */
	nuvoton_pnp_enter_conf_state(SERIAL_DEV);
	pnp_set_logical_device(SERIAL_DEV);

	pnp_write_config(SERIAL_DEV, 0x1c, 0x80);
	pnp_write_config(SERIAL_DEV, 0x27, 0x80);
	pnp_write_config(SERIAL_DEV, 0x2a, 0x60);

	nuvoton_pnp_exit_conf_state(SERIAL_DEV);

	nuvoton_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
