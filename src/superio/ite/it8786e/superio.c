/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/device.h>
#include <device/pnp.h>
#include <pc80/keyboard.h>
#include <superio/conf_mode.h>
#include <superio/ite/common/env_ctrl.h>

#include "it8786e.h"
#include "chip.h"

static void it8786e_init(struct device *const dev)
{
	const struct superio_ite_it8786e_config *conf;
	const struct resource *res;

	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {
	case IT8786E_EC:
		conf = dev->chip_info;
		res = probe_resource(dev, PNP_IDX_IO0);
		if (!conf || !res)
			break;
		ite_ec_init(res->base, &conf->ec);
		break;
	case IT8786E_KBCK:
		pc_keyboard_init(NO_AUX_DEVICE);
		break;
	default:
		break;
	}
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = it8786e_init,
	.ops_pnp_mode     = &pnp_conf_mode_870155_aa,
};

static struct pnp_info pnp_dev_info[] = {
	/* Serial Port 1 */
	{ NULL, IT8786E_SP1, PNP_IO0 | PNP_IRQ0 | PNP_MSC0 | PNP_MSC1 |
			     PNP_MSC2,
	  0x0ff8, },
	/* Serial Port 2 */
	{ NULL, IT8786E_SP2, PNP_IO0 | PNP_IRQ0 | PNP_MSC0 | PNP_MSC1 |
			     PNP_MSC2,
	  0x0ff8, },
	/* Printer Port */
	{ NULL, IT8786E_PP,  PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_DRQ0 |
			     PNP_MSC0,
	  0x0ff8, 0x0ffc, },
	/* Environmental Controller */
	{ NULL, IT8786E_EC,  PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_MSC0 |
			     PNP_MSC1 | PNP_MSC2 | PNP_MSC3 | PNP_MSC4 |
			     PNP_MSC5 | PNP_MSC6 | PNP_MSCA | PNP_MSCB |
			     PNP_MSCC,
	  0x0ff8, 0x0ffc, },
	/* KBC Keyboard */
	{ NULL, IT8786E_KBCK, PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_MSC0,
	  0x0fff, 0x0fff, },
	/* KBC Mouse */
	{ NULL, IT8786E_KBCM, PNP_IRQ0 | PNP_MSC0, },
	/* GPIO */
	{ NULL, IT8786E_GPIO, PNP_IO0 | PNP_IO1 | PNP_IRQ0 |
			      PNP_MSC0 | PNP_MSC1 | PNP_MSC2 | PNP_MSC3 |
			      PNP_MSC4 | PNP_MSC5 | PNP_MSC6 | PNP_MSC7 |
			      PNP_MSC8 | PNP_MSC9 | PNP_MSCA | PNP_MSCB,
	  0x0ffc, 0x0fff, },
	/* Serial Port 3 */
	{ NULL, IT8786E_SP3, PNP_IO0 | PNP_IRQ0 | PNP_MSC0 | PNP_MSC1 |
			     PNP_MSC2,
	  0x0ff8, },
	/* Serial Port 4 */
	{ NULL, IT8786E_SP4, PNP_IO0 | PNP_IRQ0 | PNP_MSC0 | PNP_MSC1 |
			     PNP_MSC2,
	  0x0ff8, },
	/* Consumer Infrared */
	{ NULL, IT8786E_CIR, PNP_IO0 | PNP_IRQ0 | PNP_MSC0, 0x0ff8, },
	/* Serial Port 5 */
	{ NULL, IT8786E_SP5, PNP_IO0 | PNP_IRQ0 | PNP_MSC0 | PNP_MSC1 |
			     PNP_MSC2,
	  0x0ff8, },
	/* Serial Port 6 */
	{ NULL, IT8786E_SP6, PNP_IO0 | PNP_IRQ0 | PNP_MSC0 | PNP_MSC1 |
			     PNP_MSC2,
	  0x0ff8, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_ite_it8786e_ops = {
	CHIP_NAME("ITE IT8786E Super I/O")
	.enable_dev = enable_dev,
};
