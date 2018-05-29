/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Nick Barker <nick.barker9@btinternet.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pnp.h>
#include <superio/conf_mode.h>
#include <stdlib.h>
#include "vt1211.h"

static u8 hwm_io_regs[] = {
	0x10,0x03, 0x11,0x10, 0x12,0x0d, 0x13,0x7f,
	0x14,0x21, 0x15,0x81, 0x16,0xbd, 0x17,0x8a,
	0x18,0x00, 0x19,0x00, 0x1a,0x00, 0x1b,0x00,
	0x1d,0xff, 0x1e,0x00, 0x1f,0x73, 0x20,0x67,
	0x21,0xc1, 0x22,0xca, 0x23,0x74, 0x24,0xc2,
	0x25,0xc7, 0x26,0xc9, 0x27,0x7f, 0x29,0x00,
	0x2a,0x00, 0x2b,0xff, 0x2c,0x00, 0x2d,0xff,
	0x2e,0x00, 0x2f,0xff, 0x30,0x00, 0x31,0xff,
	0x32,0x00, 0x33,0xff, 0x34,0x00, 0x39,0xff,
	0x3a,0x00, 0x3b,0xff, 0x3c,0xff, 0x3d,0xff,
	0x3e,0x00, 0x3f,0xb0, 0x43,0xff, 0x44,0xff,
	0x46,0xff, 0x47,0x50, 0x4a,0x03, 0x4b,0xc0,
	0x4c,0x00, 0x4d,0x00, 0x4e,0x0f, 0x5d,0x77,
	0x5c,0x00, 0x5f,0x33, 0x40,0x01,
};

static void vt1211_set_iobase(struct device *dev, u8 index, u16 iobase)
{
	switch (dev->path.pnp.device) {
	case VT1211_FDC:
	case VT1211_PP:
	case VT1211_SP1:
	case VT1211_SP2:
		pnp_write_config(dev, index + 0, (iobase >> 2) & 0xff);
		break;
	case VT1211_ROM:
		/* TODO: Error. VT1211_ROM doesn't have an I/O base. */
		break;
	case VT1211_MIDI:
	case VT1211_GAME:
	case VT1211_GPIO:
	case VT1211_WDG:
	case VT1211_WUC:
	case VT1211_HWM:
	case VT1211_FIR:
	default:
		pnp_write_config(dev, index + 0, (iobase >> 8) & 0xff);
		pnp_write_config(dev, index + 1, iobase & 0xff);
		break;
	}
}

/* Initialize VT1211 hardware monitor registers, which are at 0xECXX. */
static void init_hwm(u16 base)
{
	int i;

	for (i = 0; i < sizeof(hwm_io_regs); i += 2)
		outb(hwm_io_regs[i + 1], base + hwm_io_regs[i]);
}

static void vt1211_init(struct device *dev)
{
	struct resource *res0;

	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {
	case VT1211_HWM:
		res0 = find_resource(dev, PNP_IDX_IO0);
		init_hwm(res0->base);
		break;
	case VT1211_FDC:
	case VT1211_PP:
	case VT1211_MIDI:
	case VT1211_GAME:
	case VT1211_GPIO:
	case VT1211_WDG:
	case VT1211_WUC:
	case VT1211_FIR:
	case VT1211_ROM:
		/* TODO: Any init needed for these LDNs? */
		break;
	default:
		printk(BIOS_INFO, "VT1211: Cannot init unknown device!\n");
	}
}

static void vt1211_pnp_enable_resources(struct device *dev)
{
	printk(BIOS_DEBUG, "%s - enabling\n", dev_path(dev));
	pnp_enable_resources(dev);
}

static void vt1211_pnp_set_resources(struct device *dev)
{
	struct resource *res;

#if IS_ENABLED(CONFIG_CONSOLE_SERIAL) && IS_ENABLED(CONFIG_DRIVERS_UART_8250IO)
	/* TODO: Do the same for SP2? */
	if (dev->path.pnp.device == VT1211_SP1) {
		for (res = dev->resource_list; res; res = res->next) {
			res->flags |= IORESOURCE_STORED;
			report_resource_stored(dev, res, "");
		}
		return;
	}
#endif

	pnp_enter_conf_mode(dev);

	pnp_set_logical_device(dev);

	/* Paranoia says I should disable the device here... */
	for (res = dev->resource_list; res; res = res->next) {
		if (!(res->flags & IORESOURCE_ASSIGNED)) {
			printk(BIOS_ERR, "ERROR: %s %02lx %s size: 0x%010Lx "
			       "not assigned\n", dev_path(dev), res->index,
			       resource_type(res), res->size);
			continue;
		}

		/* Now store the resource. */
		if (res->flags & IORESOURCE_IO) {
			vt1211_set_iobase(dev, res->index, res->base);
		} else if (res->flags & IORESOURCE_DRQ) {
			pnp_set_drq(dev, res->index, res->base);
		} else if (res->flags  & IORESOURCE_IRQ) {
			pnp_set_irq(dev, res->index, res->base);
		} else {
			printk(BIOS_ERR, "ERROR: %s %02lx unknown resource "
			       "type\n", dev_path(dev), res->index);
			return;
		}
		res->flags |= IORESOURCE_STORED;

		report_resource_stored(dev, res, "");
	}

	pnp_exit_conf_mode(dev);
}

struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = vt1211_pnp_set_resources,
	.enable_resources = vt1211_pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = vt1211_init,
	.ops_pnp_mode     = &pnp_conf_mode_8787_aa,
};

/* TODO: Check if 0x07f8 is correct for FDC/PP/SP1/SP2, the rest is correct. */
static struct pnp_info pnp_dev_info[] = {
	{ &ops, VT1211_FDC,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x07f8, },
	{ &ops, VT1211_PP,   PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x07f8, },
	{ &ops, VT1211_SP1,  PNP_IO0 | PNP_IRQ0, 0x07f8, },
	{ &ops, VT1211_SP2,  PNP_IO0 | PNP_IRQ0, 0x07f8, },
	{ &ops, VT1211_MIDI, PNP_IO0 | PNP_IRQ0, 0xfffc, },
	{ &ops, VT1211_GAME, PNP_IO0, 0xfff8, },
	{ &ops, VT1211_GPIO, PNP_IO0 | PNP_IRQ0, 0xfff0, },
	{ &ops, VT1211_WDG,  PNP_IO0 | PNP_IRQ0, 0xfff0, },
	{ &ops, VT1211_WUC,  PNP_IO0 | PNP_IRQ0, 0xfff0, },
	{ &ops, VT1211_HWM,  PNP_IO0 | PNP_IRQ0, 0xff00, },
	{ &ops, VT1211_FIR,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0xff00, },
	{ &ops, VT1211_ROM, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_via_vt1211_ops = {
	CHIP_NAME("VIA VT1211 Super I/O")
	.enable_dev = enable_dev,
};
