/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/mdio.h>
#include <device/pci.h>
#include "chip.h"
#include "m88e1512.h"

static void switch_page(struct device *dev, uint8_t page)
{
	mdio_write(dev, PAGE_REG, page);
}

static void m88e1512_init(struct device *dev)
{
	struct drivers_net_phy_m88e1512_config *config = dev->chip_info;
	uint16_t reg;

	/* Configure LEDs if requested. */
	if (config->configure_leds) {
		printk(BIOS_DEBUG, "%s: Set a customized LED mode for %s.\n",
				dev_path(dev->bus->dev), dev->chip_ops->name);

		/* Select page 3 to access LED function control register. */
		switch_page(dev, 3);

		/* Modify PHY LED mode. */
		reg = mdio_read(dev, LED_FUNC_CTRL_REG);
		clrsetbits16(&reg, LED_FUNC_CTRL_MASK, config->led_0_ctrl |
				(config->led_1_ctrl << 4) | (config->led_2_ctrl << 8));
		mdio_write(dev, LED_FUNC_CTRL_REG, reg);

		/* Switch back to page 0. */
		switch_page(dev, 0);
	}
}

struct device_operations m88e1512_ops = {
	.read_resources = noop_read_resources,
	.set_resources  = noop_set_resources,
	.init           = m88e1512_init,
};

struct chip_operations drivers_net_phy_m88e1512_ops = {
	CHIP_NAME("88E1512")
};
