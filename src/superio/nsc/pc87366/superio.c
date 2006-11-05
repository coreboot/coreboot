/* Copyright 2000  AG Electronics Ltd. */
/* Copyright 2003-2004 Linux Networx */
/* This code is distributed without warranty under the GPL v2 (see COPYING) */

#include <arch/io.h>
#include <device/device.h>
#include <device/pnp.h>
#include <console/console.h>
#include <string.h>
#include <bitops.h>
#include <uart8250.h>
#include <pc80/keyboard.h>
#include "chip.h"
#include "pc87366.h"

static void init(device_t dev)
{
	struct superio_nsc_pc87366_config *conf;
	struct resource *res0, *res1;
	/* Wishlist handle well known programming interfaces more
	 * generically.
	 */
	if (!dev->enabled) {
		return;
	}
	conf = dev->chip_info;
	switch(dev->path.u.pnp.device) {
	case PC87366_SP1: 
		res0 = find_resource(dev, PNP_IDX_IO0);
		init_uart8250(res0->base, &conf->com1);
		break;
	case PC87366_SP2:
		res0 = find_resource(dev, PNP_IDX_IO0);
		init_uart8250(res0->base, &conf->com2);
		break;
	case PC87366_KBCK:
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
 { &ops, PC87366_FDC,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, { 0x07fa, 0}, },
 { &ops, PC87366_PP,   PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, { 0x04f8, 0}, },
 { &ops, PC87366_SP2,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0 | PNP_DRQ1, { 0x7f8, 0 }, },
 { &ops, PC87366_SP1,  PNP_IO0 | PNP_IRQ0, { 0x7f8, 0 }, },
 { &ops, PC87366_SWC,  PNP_IO0 | PNP_IRQ0, { 0xfff0, 0 }, },
 { &ops, PC87366_KBCM, PNP_IRQ0 },
 { &ops, PC87366_KBCK, PNP_IO0 | PNP_IO1 | PNP_IRQ0, { 0x7f8, 0 }, { 0x7f8, 0x4}, },
 { &ops, PC87366_GPIO, PNP_IO0 | PNP_IRQ0, { 0xfff8, 0 } },
 { &ops, PC87366_ACB,  PNP_IO0 | PNP_IRQ0, { 0xfff8, 0 } },
 { &ops, PC87366_FSCM, PNP_IO0 | PNP_IRQ0, { 0xfff8, 0 } },
 { &ops, PC87366_WDT,  PNP_IO0 | PNP_IRQ0, { 0xfffc, 0 } },
};


static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &pnp_ops, 
		sizeof(pnp_dev_info)/sizeof(pnp_dev_info[0]), pnp_dev_info);
}

struct chip_operations superio_nsc_pc87366_ops = {
	CHIP_NAME("NSC PC87366 Super I/O")
	.enable_dev = enable_dev,
};
