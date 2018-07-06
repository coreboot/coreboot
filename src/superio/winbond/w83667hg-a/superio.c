/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Felix Held <felix-coreboot@felixheld.de>
 * Copyright (C) 2014 Edward O'Callaghan <eocallaghan@alterapraxis.com>
 * Copyright (C) 2015 - 2016 Raptor Engineering
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
 */

#include <console/console.h>
#include <arch/io.h>
#include <device/device.h>
#include <device/pnp.h>
#include <pc80/keyboard.h>
#include <pc80/mc146818rtc.h>
#include <stdlib.h>
#include <arch/acpi.h>
#include <superio/conf_mode.h>

#include "w83667hg-a.h"

#define MAINBOARD_POWER_OFF 0
#define MAINBOARD_POWER_ON 1

#ifndef CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL
#define CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL MAINBOARD_POWER_ON
#endif

static void w83667hg_a_init(struct device *dev)
{
	uint8_t byte;
	uint8_t power_status;
	uint8_t mouse_detected;

	if (!dev->enabled)
		return;

	switch(dev->path.pnp.device) {
	/* TODO: Might potentially need code for HWM or FDC etc. */
	case W83667HG_A_KBC:
		/* Enable mouse controller */
		pnp_enter_conf_mode_8787(dev);
		byte = pnp_read_config(dev, 0x2a);
		byte &= ~(0x1 << 1);
		pnp_write_config(dev, 0x2a, byte);
		pnp_exit_conf_mode_aa(dev);

		mouse_detected = pc_keyboard_init(PROBE_AUX_DEVICE);

		if (!mouse_detected && !acpi_is_wakeup_s3()) {
			printk(BIOS_INFO, "%s: Disable mouse controller.",
					__func__);
			pnp_enter_conf_mode_8787(dev);
			byte = pnp_read_config(dev, 0x2a);
			byte |= 0x1 << 1;
			pnp_write_config(dev, 0x2a, byte);
			pnp_exit_conf_mode_aa(dev);
		}
		break;
	case W83667HG_A_ACPI:
		/* Set power state after power fail */
		power_status = CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL;
		get_option(&power_status, "power_on_after_fail");
		pnp_enter_conf_mode_8787(dev);
		pnp_set_logical_device(dev);
		byte = pnp_read_config(dev, 0xe4);
		byte &= ~0x60;
		if (power_status == 1)
			byte |= (0x1 << 5);    /* Force power on */
		else if (power_status == 2)
			byte |= (0x2 << 5);    /* Use last power state */
		pnp_write_config(dev, 0xe4, byte);
		pnp_exit_conf_mode_aa(dev);
		printk(BIOS_INFO, "set power %s after power fail\n", power_status ? "on" : "off");
		break;
	}
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = w83667hg_a_init,
	.ops_pnp_mode     = &pnp_conf_mode_8787_aa,
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, W83667HG_A_FDC, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x0ff8, },
	{ NULL, W83667HG_A_PP, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x0ff8, },
	{ NULL, W83667HG_A_SP1, PNP_IO0 | PNP_IRQ0, 0x0ff8, },
	{ NULL, W83667HG_A_SP2, PNP_IO0 | PNP_IRQ0, 0x0ff8, },
	{ NULL, W83667HG_A_KBC, PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_IRQ1,
		0x0fff, 0x0fff, },
	{ NULL, W83667HG_A_SPI1, PNP_IO1, 0, 0x0ff8},
	{ NULL, W83667HG_A_WDT1},
	{ NULL, W83667HG_A_ACPI},
	{ NULL, W83667HG_A_HWM_TSI, PNP_IO0 | PNP_IRQ0, 0x0ffe, },
	{ NULL, W83667HG_A_PECI},
	{ NULL, W83667HG_A_VID_BUSSEL},
	{ NULL, W83667HG_A_GPIO_PP_OD},
	{ NULL, W83667HG_A_GPIO1},
	{ NULL, W83667HG_A_GPIO2},
	{ NULL, W83667HG_A_GPIO3},
	{ NULL, W83667HG_A_GPIO4},
	{ NULL, W83667HG_A_GPIO5},
	{ NULL, W83667HG_A_GPIO6},
	{ NULL, W83667HG_A_GPIO7},
	{ NULL, W83667HG_A_GPIO8},
	{ NULL, W83667HG_A_GPIO9},
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_winbond_w83667hg_a_ops = {
	CHIP_NAME("WINBOND W83667HG-A Super I/O")
	.enable_dev = enable_dev,
};
