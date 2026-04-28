/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pnp_ops.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6779d/nct6779d.h>
#include <southbridge/intel/lynxpoint/pch.h>

#define GLOBAL_DEV PNP_DEV(0x4e, 0)

void mainboard_config_superio(void)
{
	nuvoton_pnp_enter_conf_state(GLOBAL_DEV);

	/* Select SIO pin mux states */
	pnp_write_config(GLOBAL_DEV, 0x13, 0xff);
	pnp_write_config(GLOBAL_DEV, 0x14, 0xff);
	pnp_write_config(GLOBAL_DEV, 0x1c, 0x08);
	pnp_write_config(GLOBAL_DEV, 0x21, 0x63);
	pnp_write_config(GLOBAL_DEV, 0x26, 0x40);
	pnp_write_config(GLOBAL_DEV, 0x2a, 0x00);
	pnp_write_config(GLOBAL_DEV, 0x2c, 0x00);
	pnp_write_config(GLOBAL_DEV, 0x2f, 0x0b);
}
