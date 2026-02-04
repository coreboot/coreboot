/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/device.h>
#include <device/pnp.h>
#include <pc80/keyboard.h>
#include <superio/conf_mode.h>
#include <superio/common/ssdt.h>
#include <acpi/acpi.h>
#include "nct6796d.h"

static void nct6796d_init(struct device *dev)
{
	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {
	case NCT6796D_KBC:
		pc_keyboard_init(NO_AUX_DEVICE);
		break;
	}
}

#if CONFIG(HAVE_ACPI_TABLES)
/* Provide ACPI HIDs for generic Super I/O SSDT */
static const char *nct6796d_acpi_hid(const struct device *dev)
{
	if ((dev->path.type != DEVICE_PATH_PNP) ||
		(dev->path.pnp.port == 0) ||
		((dev->path.pnp.device & 0xff) > NCT6796D_DS))
		return NULL;

	switch (dev->path.pnp.device & 0xff) {
	case NCT6796D_SP1:
		__fallthrough;
	case NCT6796D_SP2:
		return ACPI_HID_COM;
	case NCT6796D_KBC:
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
	.init             = nct6796d_init,
	.ops_pnp_mode     = &pnp_conf_mode_8787_aa,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_fill_ssdt   = superio_common_fill_ssdt_generator,
	.acpi_name        = superio_common_ldn_acpi_name,
	.acpi_hid         = nct6796d_acpi_hid,
#endif
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, NCT6796D_PP, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0,
		0x0ff8, },
	{ NULL, NCT6796D_SP1, PNP_IO0 | PNP_IRQ0,
		0x0ff8, },
	{ NULL, NCT6796D_SP2, PNP_IO0 | PNP_IRQ0,
		0x0ff8, },
	{ NULL, NCT6796D_KBC, PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_IRQ1,
		0x0fff, 0x0fff, },
	{ NULL, NCT6796D_CIR, PNP_IO0 | PNP_IRQ0,
		0x0ff8, },
	{ NULL, NCT6796D_ACPI},
	{ NULL, NCT6796D_HWM_FPLED, PNP_IO0 | PNP_IO1 | PNP_IRQ0,
		0x0ffe, 0x0ffe, },
	{ NULL, NCT6796D_BCLK_WDT2_WDTMEM},
	{ NULL, NCT6796D_CIRWUP, PNP_IO0 | PNP_IRQ0,
		0x0ff8, },
	{ NULL, NCT6796D_GPIO_PP_OD},
	{ NULL, NCT6796D_PORT80},
	{ NULL, NCT6796D_WDT1},
	{ NULL, NCT6796D_WDTMEM},
	{ NULL, NCT6796D_GPIOBASE, PNP_IO0,
		0x0ff8, },
	{ NULL, NCT6796D_GPIO0},
	{ NULL, NCT6796D_GPIO1},
	{ NULL, NCT6796D_GPIO2},
	{ NULL, NCT6796D_GPIO3},
	{ NULL, NCT6796D_GPIO4},
	{ NULL, NCT6796D_GPIO5},
	{ NULL, NCT6796D_GPIO6},
	{ NULL, NCT6796D_GPIO7},
	{ NULL, NCT6796D_GPIO8},
	{ NULL, NCT6796D_DS5},
	{ NULL, NCT6796D_DS3},
	{ NULL, NCT6796D_PCHDSW},
	{ NULL, NCT6796D_DSWWOPT},
	{ NULL, NCT6796D_DS3OPT},
	{ NULL, NCT6796D_DSDSS},
	{ NULL, NCT6796D_DSPU},
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_nuvoton_nct6796d_ops = {
	.name = "NUVOTON NCT6796D Super I/O",
	.enable_dev = enable_dev,
};
