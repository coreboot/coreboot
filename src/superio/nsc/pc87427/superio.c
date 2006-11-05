/* Copyright 2000  AG Electronics Ltd. */
/* Copyright 2003-2004 Linux Networx */
/* This code is distributed without warranty under the GPL v2 (see COPYING) */

#include <arch/io.h>
#include <device/device.h>
#include <device/pnp.h>
#include <console/console.h>
#include <string.h>
#include <bitops.h>
#include "chip.h"
#include "pc87427.h"


static void init(device_t dev)
{
	struct superio_nsc_pc87427_config *conf;
	struct resource *res0, *res1;
	/* Wishlist handle well known programming interfaces more
	 * generically.
	 */
	if (!dev->enabled) {
		return;
	}
	conf = dev->chip_info;
	switch(dev->path.u.pnp.device) {
	case PC87427_SP1: 
		res0 = find_resource(dev, PNP_IDX_IO0);
		init_uart8250(res0->base, &conf->com1);
		break;
	case PC87427_SP2:
		res0 = find_resource(dev, PNP_IDX_IO0);
		init_uart8250(res0->base, &conf->com2);
		break;
	case PC87427_KBCK:
		res0 = find_resource(dev, PNP_IDX_IO0);
		res1 = find_resource(dev, PNP_IDX_IO1);
		init_pc_keyboard(res0->base, res1->base, &conf->keyboard);
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
 { &ops,  PC87427_FDC,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, { 0x07fa, 0}, },
 { &ops,  PC87427_SP2,  PNP_IO0 | PNP_IRQ0, { 0x7f8, 0 }, },
 { &ops,  PC87427_SP1,  PNP_IO0 | PNP_IRQ0, { 0x7f8, 0 }, },
 { &ops,  PC87427_SWC,  PNP_IO0 | PNP_IO1 | PNP_IO2 | PNP_IO3 | PNP_IRQ0, 
   { 0xfff0, 0 }, { 0xfffc, 0 }, { 0xfffc, 0 }, { 0xfff8, 0 } },
 { &ops,  PC87427_KBCM, PNP_IRQ0 },
 { &ops,  PC87427_KBCK, PNP_IO0 | PNP_IO1 | PNP_IRQ0, { 0x7f8, 0 }, { 0x7f8, 0x4}, },
 { &ops,  PC87427_GPIO, PNP_IO0 | PNP_IRQ0, { 0xffe0, 0 } },
 { &ops,  PC87427_WDT,  PNP_IO0 | PNP_IRQ0, { 0xfff0, 0 } },
 { &ops,  PC87427_FMC,  PNP_IO0 | PNP_IRQ0, { 0xffe0, 0 } },
 { &ops,  PC87427_XBUS, PNP_IO0 | PNP_IRQ0, { 0xffe0, 0 } },
 { &ops,  PC87427_RTC,  PNP_IO0 | PNP_IO1 | PNP_IRQ0, { 0xfffe, 0 }, { 0xfffe, 0 } },
 { &ops,  PC87427_MHC,  PNP_IO0 | PNP_IO1 | PNP_IRQ0, { 0xffe0, 0 }, { 0xffe0, 0 } },
};


static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops,
		sizeof(pnp_dev_info)/sizeof(pnp_dev_info[0]), pnp_dev_info); 
}

struct chip_operations superio_nsc_pc87427_ops = {
	CHIP_NAME("NSC PC87427 Super I/O")
	.enable_dev = enable_dev,
};
