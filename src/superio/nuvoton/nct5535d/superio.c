/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/device.h>
#include <device/pnp.h>
#include <superio/nuvoton/common/nuvoton.h>

#include "nct5535d.h"

/* Initialization C code is provided by common code */

static struct pnp_info pnp_dev_info[] = {
	{ NULL, NCT5535D_SP1, PNP_IO0 | PNP_IRQ0, 0x0ff8, },
	{ NULL, NCT5535D_KBC, PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_IRQ1,
		0x0fff, 0x0fff, },
	{ NULL, NCT5535D_HWM_FPLED, PNP_IO0 | PNP_IO1 | PNP_IRQ0,
		0x0ffe, 0x0ffe, },
	{ NULL, NCT5535D_GPIOBASE, PNP_IO0, 0x0ff8, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &nuvoton_common_ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_nuvoton_nct5535d_ops = {
	.name = "NUVOTON NCT5535D Super I/O",
	.enable_dev = enable_dev,
};
