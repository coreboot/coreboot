/* SPDX-License-Identifier: GPL-2.0-or-later */
/* This file is part of the coreboot project. */

#include <device/device.h>
#include <device/pnp.h>
#include <pc80/keyboard.h>
#include <superio/conf_mode.h>
#include "nct5539d.h"

#if CONFIG(HAVE_ACPI_TABLES)
#include <superio/common/ssdt.h>
#include <arch/acpi.h>
#endif

static void nct5539d_init(struct device *dev)
{
	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {
	case NCT5539D_KBC:
		pc_keyboard_init(NO_AUX_DEVICE);
		break;
	}
}

#if CONFIG(HAVE_ACPI_TABLES)
/* Provide ACPI HIDs for generic Super I/O SSDT */
static const char *nct5539d_acpi_hid(const struct device *dev)
{
	if ((dev->path.type != DEVICE_PATH_PNP) ||
		(dev->path.pnp.port == 0) ||
		((dev->path.pnp.device & 0xff) > NCT5539D_DS))
		return NULL;

	switch (dev->path.pnp.device & 0xff) {
	case NCT5539D_SP1:
		return ACPI_HID_COM;
	case NCT5539D_KBC:
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
	.init             = nct5539d_init,
	.ops_pnp_mode     = &pnp_conf_mode_8787_aa,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_fill_ssdt   = superio_common_fill_ssdt_generator,
	.acpi_name        = superio_common_ldn_acpi_name,
	.acpi_hid         = nct5539d_acpi_hid,
#endif
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, NCT5539D_SP1, PNP_IO0 | PNP_IRQ0,
		0x0ff8, },
	{ NULL, NCT5539D_KBC, PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_IRQ1,
		0x0fff, 0x0fff, },
	{ NULL, NCT5539D_CIR, PNP_IO0 | PNP_IRQ0,
		0x0ff8, },
	{ NULL, NCT5539D_ACPI},
	{ NULL, NCT5539D_HWM_FPLED, PNP_IO0 | PNP_IRQ0,
		0x0ffe, 0x0ffe, },
	{ NULL, NCT5539D_WDT2},
	{ NULL, NCT5539D_CIRWUP, PNP_IO0 | PNP_IRQ0,
		0x0ff8, },
	{ NULL, NCT5539D_GPIO_PP_OD},
	{ NULL, NCT5539D_WDT1},
	{ NULL, NCT5539D_WDT3},
	{ NULL, NCT5539D_GPIOBASE, PNP_IO0,
		0x0ff8, },
	{ NULL, NCT5539D_GPIO0},
	{ NULL, NCT5539D_GPIO2},
	{ NULL, NCT5539D_GPIO3},
	{ NULL, NCT5539D_GPIO4},
	{ NULL, NCT5539D_GPIO5},
	{ NULL, NCT5539D_GPIO7},
	{ NULL, NCT5539D_GPIO8},
	{ NULL, NCT5539D_GPIO_PSO},
	{ NULL, NCT5539D_SWEC},
	{ NULL, NCT5539D_FLED},
	{ NULL, NCT5539D_DS5},
	{ NULL, NCT5539D_DS3},
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_nuvoton_nct5539d_ops = {
	CHIP_NAME("NUVOTON NCT5539D Super I/O")
	.enable_dev = enable_dev,
};
