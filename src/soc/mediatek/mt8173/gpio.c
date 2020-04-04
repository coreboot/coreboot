/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */
#include <device/mmio.h>
#include <assert.h>
#include <gpio.h>
#include <types.h>

enum {
	MAX_GPIO_NUMBER = 134,
};

static void pos_bit_calc(gpio_t gpio, u32 *pos, u32 *bit)
{
	*pos = gpio.id / MAX_GPIO_REG_BITS;
	*bit = gpio.id % MAX_GPIO_REG_BITS;
}

void gpio_set_pull(gpio_t gpio, enum pull_enable enable,
		   enum pull_select select)
{
	u32 pos;
	u32 bit;
	u32 *en_reg, *sel_reg;
	u32 pin = gpio.id;

	pos_bit_calc(gpio, &pos, &bit);

	if (enable == GPIO_PULL_DISABLE) {
		en_reg = &mtk_gpio->pullen[pos].rst;
	} else {
	/* These pins' pulls can't be set through GPIO controller. */
		assert(pin < 22 || pin > 27);
		assert(pin < 47 || pin > 56);
		assert(pin < 57 || pin > 68);
		assert(pin < 73 || pin > 78);
		assert(pin < 100 || pin > 105);
		assert(pin < 119 || pin > 124);

		en_reg = &mtk_gpio->pullen[pos].set;
		sel_reg = (select == GPIO_PULL_DOWN) ?
			  (&mtk_gpio->pullsel[pos].rst) :
			  (&mtk_gpio->pullsel[pos].set);
		write16(sel_reg, 1L << bit);
	}
	write16(en_reg, 1L << bit);
}
