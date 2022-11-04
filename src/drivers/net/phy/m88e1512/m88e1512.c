/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>

static void m88e1512_init(struct device *dev)
{
}

struct device_operations m88e1512_ops = {
	.read_resources = noop_read_resources,
	.set_resources  = noop_set_resources,
	.init           = m88e1512_init,
};

struct chip_operations drivers_net_phy_m88e1512_ops = {
	CHIP_NAME("88E1512")
};
