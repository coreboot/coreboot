/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/io.h>
#include <device/pnp_def.h>
#include <device/pnp_ops.h>

#include "it8772f.h"

/* NOTICE: This file is deprecated, use ite/common instead */

void it8772f_enter_conf(pnp_devfn_t dev)
{
	u16 port = dev >> 8;

	outb(0x87, port);
	outb(0x01, port);
	outb(0x55, port);
	outb((port == 0x4e) ? 0xaa : 0x55, port);
}

void it8772f_exit_conf(pnp_devfn_t dev)
{
	pnp_write_config(dev, IT8772F_CONFIG_REG_CC, 0x02);
}

/* Set AC resume to be up to the Southbridge */
void it8772f_ac_resume_southbridge(pnp_devfn_t dev)
{
	it8772f_enter_conf(dev);
	pnp_write_config(dev, IT8772F_CONFIG_REG_LDN, IT8772F_EC);
	pnp_write_config(dev, PNP_IDX_MSC4, 0x60);
	it8772f_exit_conf(dev);
}

/* Configure a set of GPIOs */
void it8772f_gpio_setup(pnp_devfn_t dev, int set, u8 select, u8 polarity,
			u8 pullup, u8 output, u8 enable)
{
	set--; /* Set 1 is offset 0 */
	it8772f_enter_conf(dev);
	pnp_write_config(dev, IT8772F_CONFIG_REG_LDN, IT8772F_GPIO);
	if (set < 5) {
		pnp_write_config(dev, GPIO_REG_SELECT(set), select);
		pnp_write_config(dev, GPIO_REG_ENABLE(set), enable);
		pnp_write_config(dev, GPIO_REG_POLARITY(set), polarity);
	}
	pnp_write_config(dev, GPIO_REG_OUTPUT(set), output);
	pnp_write_config(dev, GPIO_REG_PULLUP(set), pullup);
	it8772f_exit_conf(dev);
}

/* Configure LED GPIOs */
void it8772f_gpio_led(pnp_devfn_t dev,int set, u8 select, u8 polarity, u8 pullup,
			u8 output, u8 enable, u8 led_pin_map, u8 led_freq)
{
	set--; /* Set 1 is offset 0 */
	it8772f_enter_conf(dev);
	pnp_write_config(dev, IT8772F_CONFIG_REG_LDN, IT8772F_GPIO);
	if (set < 5) {
		pnp_write_config(dev, IT8772F_GPIO_LED_BLINK1_PINMAP, led_pin_map);
		pnp_write_config(dev, IT8772F_GPIO_LED_BLINK1_CONTROL, led_freq);
		pnp_write_config(dev, GPIO_REG_SELECT(set), select);
		pnp_write_config(dev, GPIO_REG_ENABLE(set), enable);
		pnp_write_config(dev, GPIO_REG_POLARITY(set), polarity);
	}
	pnp_write_config(dev, GPIO_REG_OUTPUT(set), output);
	pnp_write_config(dev, GPIO_REG_PULLUP(set), pullup);
	it8772f_exit_conf(dev);
}
