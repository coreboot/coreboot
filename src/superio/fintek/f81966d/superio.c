/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/device.h>
#include <device/pnp.h>
#include <superio/conf_mode.h>
#include <pc80/keyboard.h>

#include "chip.h"
#include "fintek_internal.h"
#include "f81966d.h"

static void f81966d_init(struct device *dev)
{
	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {
	case F81966D_KBC:
		pc_keyboard_init(NO_AUX_DEVICE);
		break;
	case F81966D_HWM:
		/* Fixing temp sensor read out and init Fan control */
		f81966d_hwm_init(dev);
		break;
	case F81966D_SP1:
		/* Enable Uart1 and IRQ share register */
		f81966d_uart_init(dev);
		break;
	case F81966D_SP2:
		/* Enable Uart2 and IRQ share register */
		f81966d_uart_init(dev);
		break;
	case F81966D_SP3:
		/* Enable Uart3 and IRQ share register */
		f81966d_uart_init(dev);
		break;
	case F81966D_SP4:
		/* Enable Uart4 and IRQ share register */
		f81966d_uart_init(dev);
		break;
	case F81966D_GPIO:
		/* Enable GPIO */
		f81966d_gpio_init(dev);
		break;
	}
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = f81966d_init,
	.ops_pnp_mode     = &pnp_conf_mode_8787_aa,
};

static struct pnp_info pnp_dev_info[] = {
	/* TODO: some of the 0x7f8 values may need to be corrected. */
	{ NULL, F81966D_SP1,  PNP_IO0 | PNP_IRQ0, 0x7f8, },
	{ NULL, F81966D_SP2,  PNP_IO0 | PNP_IRQ0, 0x7f8, },
	{ NULL, F81966D_SP3,  PNP_IO0 | PNP_IRQ0, 0x7f8, },
	{ NULL, F81966D_SP4,  PNP_IO0 | PNP_IRQ0, 0x7f8, },
	{ NULL, F81966D_SP5,  PNP_IO0 | PNP_IRQ0, 0x7f8, },
	{ NULL, F81966D_SP6,  PNP_IO0 | PNP_IRQ0, 0x7f8, },
	{ NULL, F81966D_KBC,  PNP_IO0 | PNP_IRQ0 | PNP_IRQ1, 0x07ff, },
	{ NULL, F81966D_PP,   PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x07f8, },
	{ NULL, F81966D_HWM,  PNP_IO0 | PNP_IRQ0, 0xff8, },
	{ NULL, F81966D_GPIO, PNP_IO0, 0xff8, },
	{ NULL, F81966D_PME, },
	{ NULL, F81966D_WDT, },
	{ NULL, F81966D_SPI, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_fintek_f81966d_ops = {
	.name = "Fintek F81966D-I Super I/O",
	.enable_dev = enable_dev
};
