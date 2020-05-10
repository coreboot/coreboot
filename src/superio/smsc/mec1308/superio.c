/* SPDX-License-Identifier: GPL-2.0-only */

/* RAM driver for the SMSC MEC1308 Super I/O chip */

#include <device/device.h>
#include <device/pnp.h>
#include <superio/conf_mode.h>
#include <pc80/keyboard.h>

#include "mec1308.h"

static void mec1308_init(struct device *dev)
{

	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {
	case MEC1308_KBC:
		pc_keyboard_init(NO_AUX_DEVICE);
		break;
	}
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = mec1308_init,
	.ops_pnp_mode     = &pnp_conf_mode_55_aa,
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, MEC1308_PM1,  PNP_IO0, 0x7ff },
	{ NULL, MEC1308_EC1,  PNP_IO0, 0x7ff },
	{ NULL, MEC1308_EC2,  PNP_IO0, 0x7ff },
	{ NULL, MEC1308_UART, PNP_IO0 | PNP_IRQ0, 0x0ff8, },
	{ NULL, MEC1308_KBC,  PNP_IRQ0, 0 /* IO Fixed at 0x60/0x64 */ },
	{ NULL, MEC1308_EC0,  PNP_IO0, 0x7ff },
	{ NULL, MEC1308_MBX,  PNP_IO0, 0x7ff },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_smsc_mec1308_ops = {
	CHIP_NAME("SMSC MEC1308 EC SuperIO Interface")
	.enable_dev = enable_dev
};
