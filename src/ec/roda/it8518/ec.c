/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pnp.h>
#include <pc80/keyboard.h>
#include <ec/acpi/ec.h>
#include <delay.h>

#include "chip.h"

static void it8518_init(struct device *dev)
{
	const struct ec_roda_it8518_config *const conf = dev->chip_info;

	if (!dev->enabled)
		return;

	printk(BIOS_DEBUG, "Roda IT8518: Initializing keyboard.\n");
	pc_keyboard_init(NO_AUX_DEVICE);

	if (conf && conf->cpuhot_limit) {
		/* The EC may take very long for the first command on a
		   cold boot (~180ms witnessed). Since we need an incre-
		   dibly long timeout, we do this EC RAM write manually. */
		int timeout = 50000;	/* 50,000 * 10us = 500ms */
		send_ec_command(0x81);
		while (ec_status() & EC_IBF && --timeout)
			udelay(10);
		send_ec_data(0xb2);
		send_ec_data(conf->cpuhot_limit);
	}
}

static struct device_operations ops = {
	.init             = it8518_init,
	.read_resources   = noop_read_resources,
	.set_resources    = noop_set_resources,
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, 0, 0, 0, }
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations ec_roda_it8518_ops = {
	.name = "Roda IT8518 EC",
	.enable_dev = enable_dev
};
