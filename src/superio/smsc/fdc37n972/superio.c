/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pnp.h>
#include <superio/conf_mode.h>
#include <pc80/keyboard.h>

#include "fdc37n972.h"

static void init(struct device *dev)
{
	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {
	case FDC37N972_FDC: /* TODO. */
		break;
	case FDC37N972_PP: /* TODO. */
		break;
	case FDC37N972_KBDC:
		pc_keyboard_init(NO_AUX_DEVICE);
		break;
	// [..] The rest: TODO
	}
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_enable,
	.init             = init,
	.ops_pnp_mode     = &pnp_conf_mode_55_aa,
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, FDC37N972_SP1, PNP_IO0 | PNP_IRQ0, 0x07f8, },
	{ NULL, FDC37N972_SP2,
		PNP_IO0 | PNP_IRQ0 | PNP_DRQ0 | PNP_DRQ1, 0x07f8, },
	{ NULL, FDC37N972_KBDC, PNP_IO0 | PNP_IO1 | PNP_IRQ0, 0x07f8, 0x07f8, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_smsc_fdc37n972_ops = {
	CHIP_NAME("SMSC FDC37N972 Super I/O")
	.enable_dev = enable_dev,
};
