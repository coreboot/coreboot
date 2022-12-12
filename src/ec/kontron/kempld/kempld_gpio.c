/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>

#include "chip.h"
#include "kempld.h"
#include "kempld_internal.h"

enum kempld_gpio_direction {
	KEMPLD_GPIO_DIR_IN  = 0,
	KEMPLD_GPIO_DIR_OUT = 1
};

static void kempld_gpio_value_set(u8 pin_num, u8 value)
{
	const u8 mask = KEMPLD_GPIO_MASK(pin_num);
	u8 reg_val = kempld_read8(KEMPLD_GPIO_LVL(pin_num));
	reg_val = value ? reg_val | mask : reg_val & ~mask;
	kempld_write8(KEMPLD_GPIO_LVL(pin_num), reg_val);
}

static void kempld_gpio_direction_set(u8 pin_num, enum kempld_gpio_direction dir)
{
	const u8 mask = KEMPLD_GPIO_MASK(pin_num);
	u8 reg_val = kempld_read8(KEMPLD_GPIO_DIR(pin_num));
	reg_val = dir == KEMPLD_GPIO_DIR_OUT ? reg_val | mask : reg_val & ~mask;
	kempld_write8(KEMPLD_GPIO_DIR(pin_num), reg_val);
}

static int kempld_configure_gpio(u8 pin_num, enum kempld_gpio_mode mode)
{
	if (kempld_get_mutex(100) < 0)
		return -1;

	switch (mode) {
	case KEMPLD_GPIO_DEFAULT:
		break;

	case KEMPLD_GPIO_INPUT:
		kempld_gpio_direction_set(pin_num, KEMPLD_GPIO_DIR_IN);
		break;

	case KEMPLD_GPIO_OUTPUT_LOW:
		kempld_gpio_value_set(pin_num, 0);
		kempld_gpio_direction_set(pin_num, KEMPLD_GPIO_DIR_OUT);
		break;

	case KEMPLD_GPIO_OUTPUT_HIGH:
		kempld_gpio_value_set(pin_num, 1);
		kempld_gpio_direction_set(pin_num, KEMPLD_GPIO_DIR_OUT);
		break;
	}

	kempld_release_mutex();
	return 0;
}

int kempld_gpio_pads_config(struct device *dev)
{
	const struct ec_kontron_kempld_config *config = dev->chip_info;

	if (!config)
		return -1;

	for (u8 i = 0; i < KEMPLD_NUM_GPIOS; ++i) {
		if (kempld_configure_gpio(i, config->gpio[i]) < 0)
			return -1;
	}
	return 0;
}
