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

	/* Enable downshift. */
	if (config->downshift_cnt) {
		if (config->downshift_cnt > DOWNSHIFT_CNT_MAX) {
			printk(BIOS_INFO, "%s: Downshift counter for %s is too large.\n",
					dev_path(dev->bus->dev), dev->chip_ops->name);
		} else {
			printk(BIOS_DEBUG, "%s: Enable downshift after %d attempts for %s.\n",
					dev_path(dev->bus->dev), config->downshift_cnt,
					dev->chip_ops->name);

			reg = mdio_read(dev, COPPER_SPEC_CTRL_REG_1);
			clrsetbits16(&reg, DOWNSHIFT_CNT_MASK,
					DOWNSHIFT_CNT(config->downshift_cnt) | DOWNSHIFT_EN);
			mdio_write(dev, COPPER_SPEC_CTRL_REG_1, reg);

			/* Downshift enable requires a software reset to take effect. */
			reg = mdio_read(dev, COPPER_CTRL_REG);
			setbits16(&reg, SOFTWARE_RESET);
			mdio_write(dev, COPPER_CTRL_REG, reg);
		}
	}

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

	/* INTn can be routed to LED[2] pin. */
	if (config->enable_int) {
		printk(BIOS_DEBUG, "%s: INTn is routed to LED[2] pin %s.\n",
				dev_path(dev->bus->dev), dev->chip_ops->name);

		/* Select page 3 to access LED function control register. */
		switch_page(dev, 3);

		reg = mdio_read(dev, LED_TIMER_CTRL_REG);
		setbits16(&reg, LED_IRQ_ENABLE);
		mdio_write(dev, LED_TIMER_CTRL_REG, reg);

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
