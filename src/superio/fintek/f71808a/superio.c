/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/device.h>
#include <device/pnp.h>
#include <superio/conf_mode.h>
#include <pc80/keyboard.h>

#include "f71808a.h"
#include "fintek_internal.h"

static void f71808a_init(struct device *dev)
{
	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {
	/* TODO: Might potentially need code for UART, GPIO... */
	case F71808A_KBC:
		pc_keyboard_init(NO_AUX_DEVICE);
		break;
	case F71808A_HWM:
		f71808a_multifunc_init(dev);
		f71808a_hwm_init(dev);
		break;
	}
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = f71808a_init,
	.ops_pnp_mode     = &pnp_conf_mode_8787_aa,
};

static struct pnp_info pnp_dev_info[] = {
	/* TODO: Some of the 0x07f8 etc. values may not be correct. */
	{ NULL, F71808A_SP1,  PNP_IO0 | PNP_IRQ0, 0x07f8, },
	{ NULL, F71808A_HWM,  PNP_IO0 | PNP_IRQ0, 0x07f8, },
	{ NULL, F71808A_KBC,  PNP_IO0 | PNP_IRQ0 | PNP_IRQ1, 0x07ff, },
	{ NULL, F71808A_GPIO, PNP_IRQ0, },
	{ NULL, F71808A_WDT,  PNP_IO0, 0x07f8,},
	{ NULL, F71808A_CIR,  PNP_IO0 | PNP_IRQ0, 0x07f8, },
	{ NULL, F71808A_PME, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_fintek_f71808a_ops = {
	.name = "Fintek F71808A Super I/O",
	.enable_dev = enable_dev
};
