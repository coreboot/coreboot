/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DEVICE_GPIO_H__
#define __DEVICE_GPIO_H__

#include <types.h>

struct gpio_operations {
	int  (*get)(uint32_t gpio);
	void (*set)(uint32_t gpio, int value);
	void (*input_pulldown)(uint32_t gpio);
	void (*input_pullup)(uint32_t gpio);
	void (*input)(uint32_t gpio);
	void (*output)(uint32_t gpio, int value);
};

/* Helper for getting gpio operations from a device */
const struct gpio_operations *dev_get_gpio_ops(struct device *dev);

#endif	/* __DEVICE_GPIO_H__ */
