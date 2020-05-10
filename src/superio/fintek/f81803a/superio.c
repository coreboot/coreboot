/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/device.h>
#include <device/pnp.h>
#include <superio/conf_mode.h>
#include <pc80/keyboard.h>
#include "f81803a.h"

static void f81803a_pme_init(struct device *dev)
{
	pnp_enter_conf_mode(dev);
	pnp_write_config(dev, LDN_REG, F81803A_PME);
	/* enable ERP function*/
	/* also set PSIN to generate PSOUT*/
	pnp_write_config(dev, PME_ERP_ENABLE_REG, ERP_ENABLE | ERP_PSOUT_EN);
	pnp_exit_conf_mode(dev);
}

static void f81803a_init(struct device *dev)
{
	if (!dev->enabled)
		return;
	switch (dev->path.pnp.device) {
	/* TODO: Might potentially need code for GPIO or WDT. */
	case F81803A_KBC:
		pc_keyboard_init(NO_AUX_DEVICE);
		break;
	case F81803A_PME:
		f81803a_pme_init(dev);
		break;
	}

}

static struct device_operations ops = {
	.read_resources		= pnp_read_resources,
	.set_resources		= pnp_set_resources,
	.enable_resources	= pnp_enable_resources,
	.enable			= pnp_alt_enable,
	.init			= f81803a_init,
	.ops_pnp_mode		= &pnp_conf_mode_8787_aa,
};

static struct pnp_info pnp_dev_info[] = {
	{ &ops, F81803A_SP1,  PNP_IO0 | PNP_IRQ0, 0x7f8, },
	{ &ops, F81803A_SP2,  PNP_IO0 | PNP_IRQ0, 0x7f8, },
	{ &ops, F81803A_HWM,  PNP_IO0 | PNP_IRQ0, 0xff8, },
	{ &ops, F81803A_KBC,  PNP_IO0 | PNP_IRQ0 | PNP_IRQ1, 0x07f8, },
	{ &ops, F81803A_GPIO, PNP_IO0 | PNP_IRQ0, 0x7f8, },
	{ &ops, F81803A_WDT,  PNP_IO0, 0x7f8 },
	{ &ops, F81803A_PME, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_fintek_f81803a_ops = {
	CHIP_NAME("Fintek F81803A Super I/O")
	.enable_dev = enable_dev
};
