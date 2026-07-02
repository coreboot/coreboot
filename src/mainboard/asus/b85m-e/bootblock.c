/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pnp_ops.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6791d/nct6791d.h>
#include <southbridge/intel/lynxpoint/pch.h>

void mainboard_config_superio(void)
{
	const pnp_devfn_t serial_dev = PNP_DEV(0x2e, NCT6791D_SP1);
	nuvoton_enable_serial(serial_dev, CONFIG_TTYS0_BASE);
}
