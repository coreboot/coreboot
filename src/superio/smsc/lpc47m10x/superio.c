/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/device.h>
#include <device/pnp.h>
#include <superio/conf_mode.h>
#include <pc80/keyboard.h>

#include "lpc47m10x.h"

/**
 * Initialize the specified Super I/O device.
 *
 * Devices other than COM ports and the keyboard controller are ignored.
 * For COM ports, we configure the baud rate.
 *
 * @param dev Pointer to structure describing a Super I/O device.
 */
static void lpc47m10x_init(struct device *dev)
{
	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {
	case LPC47M10X2_KBC:
		pc_keyboard_init(NO_AUX_DEVICE);
		break;
	}
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = lpc47m10x_init,
	.ops_pnp_mode     = &pnp_conf_mode_55_aa,
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, LPC47M10X2_FDC, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x07f8, },
	{ NULL, LPC47M10X2_PP,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x07f8, },
	{ NULL, LPC47M10X2_SP1, PNP_IO0 | PNP_IRQ0, 0x07f8, },
	{ NULL, LPC47M10X2_SP2, PNP_IO0 | PNP_IRQ0, 0x07f8, },
	{ NULL, LPC47M10X2_KBC, PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_IRQ1,
		0x07ff, 0x07ff, },
	{ NULL, LPC47M10X2_PME, PNP_IO0, 0x0f80, },
};

/**
 * Create device structures and allocate resources to devices specified in the
 * pnp_dev_info array (above).
 *
 * @param dev Pointer to structure describing a Super I/O device.
 */
static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_smsc_lpc47m10x_ops = {
	CHIP_NAME("SMSC LPC47M10x Super I/O")
	.enable_dev = enable_dev
};
