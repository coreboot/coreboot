/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 Dynon Avionics
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 $
 */

#include <arch/io.h>
#include <device/device.h>
#include <device/pnp.h>
#include <superio/conf_mode.h>
#include <console/console.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <uart8250.h>
#include <pc80/keyboard.h>
#include "chip.h"
#include "w83627uhg.h"

/*
 * Set the UART clock source.
 *
 * Possible UART clock source speeds are:
 *
 *   0 = 1.8462 MHz (default)
 *   1 = 2 MHz
 *   2 = 24 MHz
 *   3 = 14.769 MHz
 *
 * The faster clocks allow for BAUD rates up to 2mbits.
 *
 * Warning: The kernel will need to be adjusted since it assumes
 * a 1.8462 MHz clock.
 */
static void set_uart_clock_source(device_t dev, u8 uart_clock)
{
	u8 value;

	pnp_enter_conf_mode(dev);
	pnp_set_logical_device(dev);
	value = pnp_read_config(dev, 0xf0);
	value &= ~0x03;
	value |= (uart_clock & 0x03);
	pnp_write_config(dev, 0xf0, value);
	pnp_exit_conf_mode(dev);
}

static void w83627uhg_init(device_t dev)
{
	struct superio_winbond_w83627uhg_config *conf = dev->chip_info;

	if (!dev->enabled)
		return;

	switch(dev->path.pnp.device) {
	case W83627UHG_SP1:
		/* set_uart_clock_source(dev, 0); */
		break;
	case W83627UHG_SP2:
		/* set_uart_clock_source(dev, 0); */
		break;
	case W83627UHG_SP3:
		/* set_uart_clock_source(dev, 0); */
		break;
	case W83627UHG_SP4:
		/* set_uart_clock_source(dev, 0); */
		break;
	case W83627UHG_SP5:
		/* set_uart_clock_source(dev, 0); */
		break;
	case W83627UHG_SP6:
		/* set_uart_clock_source(dev, 0); */
		break;
	case W83627UHG_KBC:
		pc_keyboard_init(&conf->keyboard);
		break;
	}
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_enable,
	.init             = w83627uhg_init,
	.ops_pnp_mode     = &pnp_conf_mode_8787_aa,
};

static struct pnp_info pnp_dev_info[] = {
	{ &ops, W83627UHG_FDC,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, {0x07f8, 0}, },
	{ &ops, W83627UHG_PP,   PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, {0x07f8, 0}, },
	{ &ops, W83627UHG_SP1,  PNP_IO0 | PNP_IRQ0, {0x07f8, 0}, },
	{ &ops, W83627UHG_SP2,  PNP_IO0 | PNP_IRQ0, {0x07f8, 0}, },
	{ &ops, W83627UHG_KBC,  PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_IRQ1, {0x07ff, 0}, {0x07ff, 4}, },
	{ &ops, W83627UHG_SP3,  PNP_IO0 | PNP_IRQ0, {0x07f8, 0}, },
	{ &ops, W83627UHG_GPIO3_4, },
	{ &ops, W83627UHG_WDTO_PLED_GPIO5_6, },
	{ &ops, W83627UHG_GPIO1_2, },
	{ &ops, W83627UHG_ACPI, PNP_IRQ0, },
	{ &ops, W83627UHG_HWM,  PNP_IO0 | PNP_IRQ0, {0x0ff8, 0}, },
	{ &ops, W83627UHG_PECI_SST, },
	{ &ops, W83627UHG_SP4,  PNP_IO0 | PNP_IRQ0, {0x07f8, 0}, },
	{ &ops, W83627UHG_SP5,  PNP_IO0 | PNP_IRQ0, {0x07f8, 0}, },
	{ &ops, W83627UHG_SP6,  PNP_IO0 | PNP_IRQ0, {0x07f8, 0}, },
};

static void enable_dev(device_t dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_winbond_w83627uhg_ops = {
	CHIP_NAME("Winbond W83627UHG Super I/O")
	.enable_dev = enable_dev,
};
