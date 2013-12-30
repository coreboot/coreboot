/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Alec Ari <neotheuser@ymail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <arch/io.h>
#include <device/pnp.h>
#include <superio/conf_mode.h>
#include <stdlib.h>
#include "nct6779d.h"

static void nct6779d_init(device_t dev)
{
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = nct6779d_init,
	.ops_pnp_mode     = &pnp_conf_mode_8787_aa,
};

static struct pnp_info pnp_dev_info[] = {
	/* TODO: Some of the 0x07f8 etc. values may not be correct. */
	{ &ops, NCT6779D_PAR,  PNP_IO0 | PNP_IRQ0 | PNP_MSC0, {0x07f8, 0}, },
	{ &ops, NCT6779D_SP1,  PNP_IO0 | PNP_IRQ0 | PNP_MSC0, {0x07f8, 0}, },
	{ &ops, NCT6779D_SP2,  PNP_IO0 | PNP_IRQ0 | PNP_MSC0 | PNP_MSC1, {0x07f8, 0}, },
	{ &ops, NCT6779D_KBC,  PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_IRQ1 | PNP_MSC0, {0x07f8, 0}, },
	{ &ops, NCT6779D_CIR,  PNP_IO0 | PNP_IRQ0 | PNP_MSC0 | PNP_MSC1, {0x07f8, 0}, },
	{ &ops, NCT6779D_HWMN,  PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_MSC0 | PNP_MSC1, {0x07f8, 0}, },
	{ &ops, NCT6779D_WDT1, PNP_IO0 | PNP_MSC0 | PNP_MSC1, {0x07f8, 0}, },
	{ &ops, NCT6779D_GPIO0, PNP_EN | PNP_MSC0 | PNP_MSC1, },
	{ &ops, NCT6779D_GPIO1, PNP_EN | PNP_MSC0 | PNP_MSC1, },
	{ &ops, NCT6779D_GPIO2, PNP_EN | PNP_MSC0 | PNP_MSC1, },
	{ &ops, NCT6779D_GPIO3, PNP_EN | PNP_MSC0 | PNP_MSC1, },
	{ &ops, NCT6779D_GPIO4, PNP_EN | PNP_MSC0 | PNP_MSC1, },
	{ &ops, NCT6779D_GPIO5, PNP_EN | PNP_MSC0 | PNP_MSC1, },
	{ &ops, NCT6779D_GPIO6, PNP_EN | PNP_MSC0 | PNP_MSC1, },
	{ &ops, NCT6779D_GPIO7, PNP_EN | PNP_MSC0 | PNP_MSC1, },
	{ &ops, NCT6779D_GPIO8, PNP_EN | PNP_MSC0 | PNP_MSC1, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_nuvoton_nct6779d_ops = {
	CHIP_NAME("NUVOTON NCT6779D Super I/O")
	.enable_dev = enable_dev,
};
