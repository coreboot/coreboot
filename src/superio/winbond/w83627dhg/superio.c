/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Uwe Hermann <uwe@hermann-uwe.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/io.h>
#include <device/device.h>
#include <device/pnp.h>
#include <superio/conf_mode.h>
#include <pc80/keyboard.h>
#include <stdlib.h>
#include "chip.h"
#include "w83627dhg.h"

static void w83627dhg_enable_UR2(device_t dev)
{
	u8 reg8;

	pnp_enter_conf_mode(dev);
	reg8 = pnp_read_config(dev, 0x2c);
	reg8 |= (0x3);
	pnp_write_config(dev, 0x2c, reg8); // Set pins 78-85-> UART B
	pnp_exit_conf_mode(dev);
}

static void w83627dhg_init(device_t dev)
{
	struct superio_winbond_w83627dhg_config *conf = dev->chip_info;

	if (!dev->enabled)
		return;

	switch(dev->path.pnp.device) {
	case W83627DHG_SP2:
		w83627dhg_enable_UR2(dev);
		break;
	case W83627DHG_KBC:
		pc_keyboard_init(&conf->keyboard);
		break;
	}
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = w83627dhg_init,
	.ops_pnp_mode     = &pnp_conf_mode_8787_aa,
};

static struct pnp_info pnp_dev_info[] = {
	{ &ops, W83627DHG_FDC, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, {0x07f8, 0}, },
	{ &ops, W83627DHG_PP,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, {0x07f8, 0}, },
	{ &ops, W83627DHG_SP1, PNP_IO0 | PNP_IRQ0, {0x07f8, 0}, },
	{ &ops, W83627DHG_SP2, PNP_IO0 | PNP_IRQ0, {0x07f8, 0}, },
	{ &ops, W83627DHG_KBC, PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_IRQ1, {0x07ff, 0}, {0x07ff, 4}, },
	{ &ops, W83627DHG_SPI, PNP_IO1, {}, { 0x7f8, 0 }, },
	{ &ops, W83627DHG_GPIO6, },
	{ &ops, W83627DHG_WDTO_PLED, },
	{ &ops, W83627DHG_GPIO2, },
	{ &ops, W83627DHG_GPIO3, },
	{ &ops, W83627DHG_GPIO4, },
	{ &ops, W83627DHG_GPIO5, },
	{ &ops, W83627DHG_ACPI, PNP_IRQ0, },
	{ &ops, W83627DHG_HWM, PNP_IO0 | PNP_IRQ0, {0x07fe, 0}, },
	{ &ops, W83627DHG_PECI_SST, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_winbond_w83627dhg_ops = {
	CHIP_NAME("Winbond W83627DHG Super I/O")
	.enable_dev = enable_dev,
};
