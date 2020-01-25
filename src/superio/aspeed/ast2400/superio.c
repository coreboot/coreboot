/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Corey Osgood <corey.osgood@gmail.com>
 * Copyright (C) 2018 Eltan B.V.
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

#include <device/device.h>
#include <device/pnp.h>
#include <superio/conf_mode.h>
#include <pc80/keyboard.h>
#include <superio/common/ssdt.h>
#include <arch/acpi.h>
#include "ast2400.h"
#include "chip.h"

static void ast2400_init(struct device *dev)
{
	struct superio_aspeed_ast2400_config *conf = dev->chip_info;

	if (!dev->enabled)
		return;

	if (conf && conf->use_espi) {
		pnp_enter_conf_mode(dev);
		pnp_set_logical_device(dev);
		/* In ESPI mode must write 0 to IRQ level on every LDN */
		pnp_write_config(dev, 0x71, 0);
		pnp_exit_conf_mode(dev);
	}

	switch (dev->path.pnp.device) {
	case AST2400_KBC:
		pc_keyboard_init(NO_AUX_DEVICE);
		break;
	}
}

#if CONFIG(HAVE_ACPI_TABLES)
/* Provide ACPI HIDs for generic Super I/O SSDT */
static const char *ast2400_acpi_hid(const struct device *dev)
{
	/* Sanity checks */
	if (dev->path.type != DEVICE_PATH_PNP)
		return NULL;
	if (dev->path.pnp.port == 0)
		return NULL;
	if ((dev->path.pnp.device & 0xff) > AST2400_MAILBOX)
		return NULL;

	switch (dev->path.pnp.device & 0xff) {
	case AST2400_SUART1: /* fallthrough */
	case AST2400_SUART2: /* fallthrough */
	case AST2400_SUART3: /* fallthrough */
	case AST2400_SUART4:
		return ACPI_HID_COM;
	case AST2400_KBC:
		return ACPI_HID_KEYBOARD;
	default:
		return ACPI_HID_PNP;
	}
}
#endif

static struct device_operations ops = {
	.read_resources = pnp_read_resources,
	.set_resources = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable = pnp_enable,
	.init = ast2400_init,
	.ops_pnp_mode = &pnp_conf_mode_a5a5_aa,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_fill_ssdt_generator = superio_common_fill_ssdt_generator,
	.acpi_name = superio_common_ldn_acpi_name,
	.acpi_hid = ast2400_acpi_hid,
#endif
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, AST2400_SUART1,   PNP_IO0 | PNP_IRQ0 | PNP_MSC0, 0xfff8, },
	{ NULL, AST2400_SUART2,   PNP_IO0 | PNP_IRQ0 | PNP_MSC0, 0xfff8, },
	{ NULL, AST2400_SWAK,     PNP_IO0 | PNP_IO1 | PNP_IO2 | PNP_IO3
		| PNP_IRQ0, 0xfff8, 0xfff8, 0xfff8, 0xfff8, },
	{ NULL, AST2400_KBC,      PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_IRQ1
		| PNP_MSC0, 0xffff, 0xffff, },
	{ NULL, AST2400_GPIO,     PNP_IRQ0, }, // GPIO LDN has no IO Region
	{ NULL, AST2400_SUART3,   PNP_IO0 | PNP_IRQ0 | PNP_MSC0, 0xfff8, },
	{ NULL, AST2400_SUART4,   PNP_IO0 | PNP_IRQ0 | PNP_MSC0, 0xfff8, },
	{ NULL, AST2400_ILPC2AHB, PNP_IRQ0 | PNP_MSC0 | PNP_MSC1 | PNP_MSC2
		| PNP_MSC3 | PNP_MSC4 | PNP_MSC5 | PNP_MSC6 | PNP_MSC7
		| PNP_MSC8 | PNP_MSC9 | PNP_MSCA | PNP_MSCB | PNP_MSCC
		| PNP_MSCD | PNP_MSCE, },
	{ NULL, AST2400_MAILBOX,  PNP_IO0 | PNP_IRQ0, 0xfffe, },
};

static void enable_dev(struct device *dev)
{
	struct superio_aspeed_ast2400_config *conf = dev->chip_info;

	if (conf && conf->use_espi) {
		/* UART3 and UART4 are not usable in ESPI mode */
		for (size_t i = 0; i < ARRAY_SIZE(pnp_dev_info); i++) {
			if ((pnp_dev_info[i].function == AST2400_SUART3) ||
			    (pnp_dev_info[i].function == AST2400_SUART4))
				pnp_dev_info[i].function = PNP_SKIP_FUNCTION;
		}
	}

	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info),
		pnp_dev_info);
}

struct chip_operations superio_aspeed_ast2400_ops = {
	CHIP_NAME("ASpeed AST2400/AST2500 Super I/O")
	.enable_dev = enable_dev,
};
