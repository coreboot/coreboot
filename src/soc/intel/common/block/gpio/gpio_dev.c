/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <assert.h>
#include <device/device.h>
#include <device/gpio.h>
#include <gpio.h>
#include <intelblocks/gpio.h>

static struct gpio_operations gpio_ops = {
	.get		= gpio_get,
	.set		= gpio_set,
	.input_pulldown	= gpio_input_pulldown,
	.input_pullup	= gpio_input_pullup,
	.input		= gpio_input,
	.output		= gpio_output,
};

struct device_operations block_gpio_ops = {
	.read_resources	= noop_read_resources,
	.set_resources	= noop_set_resources,
	.ops_gpio	= &gpio_ops,
};

void block_gpio_enable(struct device *dev)
{
	assert(dev->path.type == DEVICE_PATH_GPIO);
	dev->ops = &block_gpio_ops;
}
