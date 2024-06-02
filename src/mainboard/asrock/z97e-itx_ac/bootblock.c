/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pnp_ops.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6776/nct6776.h>
#include <southbridge/intel/lynxpoint/pch.h>
#define GLOBAL_DEV PNP_DEV(0x2e, 0)
#define SERIAL_DEV PNP_DEV(0x2e, NCT6776_SP1)

void mainboard_config_superio(void)
{
	nuvoton_pnp_enter_conf_state(GLOBAL_DEV);
	/* Select SIO pin mux states */
	pnp_write_config(GLOBAL_DEV, 0x1b, 0x48);
	pnp_write_config(GLOBAL_DEV, 0x24, 0x5c);
	pnp_write_config(GLOBAL_DEV, 0x27, 0xd0);
	pnp_write_config(GLOBAL_DEV, 0x2a, 0xe0);
	pnp_write_config(GLOBAL_DEV, 0x2c, 0x80);
	pnp_write_config(GLOBAL_DEV, 0x2f, 0x01);
	nuvoton_pnp_exit_conf_state(GLOBAL_DEV);
}
