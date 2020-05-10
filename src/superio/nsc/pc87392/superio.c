/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/device.h>
#include <device/pnp.h>

#include "pc87392.h"

static void init(struct device *dev)
{
	if (!dev->enabled)
		return;
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_enable,
	.init             = init,
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, PC87392_FDC,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x07f8 },
	{ NULL, PC87392_PP,   PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x07f8 },
	{ NULL, PC87392_SP2,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0 | PNP_DRQ1,
		0x07f8 },
	{ NULL, PC87392_SP1,  PNP_IO0 | PNP_IRQ0, 0x07f8 },
	{ NULL, PC87392_GPIO, PNP_IO0 | PNP_IRQ0, 0xfff8 },
	{ NULL, PC87392_WDT,  PNP_IO0 | PNP_IRQ0, 0xfffc },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_nsc_pc87392_ops = {
	CHIP_NAME("NSC PC87392 Super I/O")
	.enable_dev = enable_dev,
};
