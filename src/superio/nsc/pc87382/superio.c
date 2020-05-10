/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/device.h>
#include <device/pnp.h>

#include "pc87382.h"

static void init(struct device *dev)
{
	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {
	case PC87382_DOCK:
		break;

	case PC87382_GPIO:
		break;
	}
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_enable,
	.init             = init,
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, PC87382_IR,   PNP_IO0 | PNP_IRQ0 | PNP_DRQ0 | PNP_DRQ1,
		0x07f8 },
	{ NULL, PC87382_SP1,  PNP_IO0 | PNP_IRQ0, 0x07f8 },
	{ NULL, PC87382_GPIO, PNP_IO0 | PNP_IRQ0, 0xfff0 },
	{ NULL, PC87382_DOCK, PNP_IO0 | PNP_IRQ0, 0xfffe },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_nsc_pc87382_ops = {
	CHIP_NAME("NSC PC87382 Docking LPC Switch")
	.enable_dev = enable_dev,
};
