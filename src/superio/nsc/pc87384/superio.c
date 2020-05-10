/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/device.h>
#include <device/pnp.h>

#include "pc87384.h"

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_enable,
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, PC87384_PP,   PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x07f8 },
	{ NULL, PC87384_SP1,  PNP_IO0 | PNP_IRQ0, 0x07f8 },
	{ NULL, PC87384_SP2,  PNP_IO0 | PNP_IRQ0, 0x07f8 },
	{ NULL, PC87384_GPIO, PNP_IO0 | PNP_IRQ0, 0xfff0 },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_nsc_pc87384_ops = {
	CHIP_NAME("NSC PC87384 Super I/O")
	.enable_dev = enable_dev,
};
