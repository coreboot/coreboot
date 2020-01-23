/* SPDX-License-Identifier: GPL-2.0-or-later */
/* This file is part of the coreboot project. */

#include <device/device.h>
#include <device/pnp.h>
#include <pc80/keyboard.h>
#include <superio/conf_mode.h>
#include <superio/common/ssdt.h>
#include <arch/acpi.h>
#include "nct6791d.h"

static void nct6791d_init(struct device *dev)
{
	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {
	case NCT6791D_KBC:
		pc_keyboard_init(NO_AUX_DEVICE);
		break;
	}
}

#if CONFIG(HAVE_ACPI_TABLES)
/* Provide ACPI HIDs for generic Super I/O SSDT */
static const char *nct6791d_acpi_hid(const struct device *dev)
{
	if ((dev->path.type != DEVICE_PATH_PNP) ||
		(dev->path.pnp.port == 0) ||
		((dev->path.pnp.device & 0xff) > NCT6791D_DS))
		return NULL;

	switch (dev->path.pnp.device & 0xff) {
	case NCT6791D_SP1: /* fallthrough */
	case NCT6791D_SP2:
		return ACPI_HID_COM;
	case NCT6791D_KBC:
		return ACPI_HID_KEYBOARD;
	default:
		return ACPI_HID_PNP;
	}
}
#endif

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = nct6791d_init,
	.ops_pnp_mode     = &pnp_conf_mode_8787_aa,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_fill_ssdt_generator = superio_common_fill_ssdt_generator,
	.acpi_name = superio_common_ldn_acpi_name,
	.acpi_hid = nct6791d_acpi_hid,
#endif
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, NCT6791D_PP, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0,
		0x0ff8, },
	{ NULL, NCT6791D_SP1, PNP_IO0 | PNP_IRQ0,
		0x0ff8, },
	{ NULL, NCT6791D_SP2, PNP_IO0 | PNP_IRQ0,
		0x0ff8, },
	{ NULL, NCT6791D_KBC, PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_IRQ1,
		0x0fff, 0x0fff, },
	{ NULL, NCT6791D_CIR, PNP_IO0 | PNP_IRQ0,
		0x0ff8, },
	{ NULL, NCT6791D_ACPI},
	{ NULL, NCT6791D_HWM_FPLED, PNP_IO0 | PNP_IO1 | PNP_IRQ0,
		0x0ffe, 0x0ffe, },
	{ NULL, NCT6791D_BCLK_WDT2_WDTMEM},
	{ NULL, NCT6791D_CIRWUP, PNP_IO0 | PNP_IRQ0,
		0x0ff8, },
	{ NULL, NCT6791D_GPIO_PP_OD},
	{ NULL, NCT6791D_PORT80},
	{ NULL, NCT6791D_WDT1},
	{ NULL, NCT6791D_WDTMEM},
	{ NULL, NCT6791D_GPIOBASE, PNP_IO0,
		0x0ff8, },
	{ NULL, NCT6791D_GPIO0},
	{ NULL, NCT6791D_GPIO1},
	{ NULL, NCT6791D_GPIO2},
	{ NULL, NCT6791D_GPIO3},
	{ NULL, NCT6791D_GPIO4},
	{ NULL, NCT6791D_GPIO5},
	{ NULL, NCT6791D_GPIO6},
	{ NULL, NCT6791D_GPIO7},
	{ NULL, NCT6791D_GPIO8},
	{ NULL, NCT6791D_DS5},
	{ NULL, NCT6791D_DS3},
	{ NULL, NCT6791D_PCHDSW},
	{ NULL, NCT6791D_DSWWOPT},
	{ NULL, NCT6791D_DS3OPT},
	{ NULL, NCT6791D_DSDSS},
	{ NULL, NCT6791D_DSPU},
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_nuvoton_nct6791d_ops = {
	CHIP_NAME("NUVOTON NCT6791D Super I/O")
	.enable_dev = enable_dev,
};
