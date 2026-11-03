/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pnp_ops.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6779d/nct6779d.h>

void mainboard_config_superio(void)
{
	const pnp_devfn_t serial_dev = PNP_DEV(0x2e, NCT6779D_SP1);
	nuvoton_enable_serial(serial_dev, CONFIG_TTYS0_BASE);
}
