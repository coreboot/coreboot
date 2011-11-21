/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Sean Nelson <snelson@nmt.edu>
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
#include <device/device.h>
#include <device/pnp.h>
#include <console/console.h>
#include <string.h>
#include <bitops.h>
#include <uart8250.h>
#include <pc80/mc146818rtc.h>
#include <stdlib.h>
#include "chip.h"
#include "w83697hf.h"

static void pnp_enter_ext_func_mode(device_t dev)
{
	outb(0x87, dev->path.pnp.port);
	outb(0x87, dev->path.pnp.port);
}

static void pnp_exit_ext_func_mode(device_t dev)
{
	outb(0xaa, dev->path.pnp.port);
}

static void hwmon_set_fan_divisor(unsigned int base, int num, unsigned int divisor) {
	unsigned char enc, buf;

	if (divisor) {
		enc = log2(divisor);
		if (1 << enc != divisor || enc > 7)
			die("invalid fan divisor");
		outb(0x4e, base + 5);
		outb(0x00, base + 6);
		outb(0x47, base + 5);
		outb((inb(base + 6) & ~(0x30 << (num * 2))) | ((enc & 3) << (4 + num * 2)), base + 6);
		outb(0x5d, base + 5);
		buf = inb(base + 6);
		/* the above inb() auto-increments the address pointer ... */
		outb(0x5d, base + 5);
		outb((buf & ~(0x20 << num)) | ((enc & 4) << (3 + num)), base + 6);
	}
}

static void w83697hf_init(device_t dev)
{
	struct resource *res0;
	struct superio_winbond_w83697hf_config *cfg;

	if (!dev->enabled)
		return;

	cfg = dev->chip_info;

	switch (dev->path.pnp.device) {
	case W83697HF_HWM:
		if (cfg) {
			res0 = find_resource(dev, PNP_IDX_IO0);
			hwmon_set_fan_divisor(res0->base, 0, cfg->hwmon_fan1_divisor);
			hwmon_set_fan_divisor(res0->base, 1, cfg->hwmon_fan2_divisor);
		}
		break;
	}
}

static void w83697hf_pnp_set_resources(device_t dev)
{
	pnp_enter_ext_func_mode(dev);
	pnp_set_resources(dev);
	pnp_exit_ext_func_mode(dev);
}

static void w83697hf_pnp_enable(device_t dev)
{
	pnp_enter_ext_func_mode(dev);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, !!dev->enabled);
	pnp_exit_ext_func_mode(dev);
}

static void w83697hf_pnp_enable_resources(device_t dev)
{
	pnp_enter_ext_func_mode(dev);
	pnp_enable_resources(dev);
	pnp_exit_ext_func_mode(dev);
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = w83697hf_pnp_set_resources,
	.enable_resources = w83697hf_pnp_enable_resources,
	.enable           = w83697hf_pnp_enable,
	.init             = w83697hf_init,
};

static struct pnp_info pnp_dev_info[] = {
	{ &ops, W83697HF_FDC,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, {0x07f8, 0}, },
	{ &ops, W83697HF_PP,   PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, {0x07f8, 0}, },
	{ &ops, W83697HF_SP1,  PNP_IO0 | PNP_IRQ0, {0x07f8, 0}, },
	{ &ops, W83697HF_SP2,  PNP_IO0 | PNP_IRQ0, {0x07f8, 0}, },
	{ &ops, W83697HF_CIR,  PNP_IO0 | PNP_IRQ0, {0x07f8, 0}, },
	{ &ops, W83697HF_GAME_GPIO1, PNP_IO0 | PNP_IO1 | PNP_IRQ0, {0x07ff, 0}, {0x07fe, 4}, },
	{ &ops, W83697HF_MIDI_GPIO5, },
	{ &ops, W83697HF_GPIO234, },
	{ &ops, W83697HF_ACPI, },
	{ &ops, W83697HF_HWM,  PNP_IO0 | PNP_IRQ0, {0x0ff8, 0}, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_winbond_w83697hf_ops = {
	CHIP_NAME("Winbond W83697HF Super I/O")
	.enable_dev = enable_dev,
};
