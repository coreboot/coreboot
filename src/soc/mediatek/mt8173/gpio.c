/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <arch/io.h>
#include <assert.h>
#include <gpio.h>
#include <types.h>

enum {
	MAX_GPIO_NUMBER = 134,
	MAX_EINT_REG_BITS = 32,
};

static void pos_bit_calc(gpio_t gpio, u32 *pos, u32 *bit)
{
	*pos = gpio.id / MAX_GPIO_REG_BITS;
	*bit = gpio.id % MAX_GPIO_REG_BITS;
}

static void pos_bit_calc_for_eint(gpio_t gpio, u32 *pos, u32 *bit)
{
	*pos = gpio.id / MAX_EINT_REG_BITS;
	*bit = gpio.id % MAX_EINT_REG_BITS;
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

int gpio_eint_poll(gpio_t gpio)
{
	u32 pos;
	u32 bit;
	u32 status;

	pos_bit_calc_for_eint(gpio, &pos, &bit);

	status = (read32(&mt8173_eint->sta.regs[pos]) >> bit) & 0x1;

	if (status)
		write32(&mt8173_eint->ack.regs[pos], 1 << bit);

	return status;
}

void gpio_eint_configure(gpio_t gpio, enum gpio_irq_type type)
{
	u32 pos;
	u32 bit, mask;

	pos_bit_calc_for_eint(gpio, &pos, &bit);
	mask = 1 << bit;

	/* Make it an input first. */
	gpio_input_pullup(gpio);

	write32(&mt8173_eint->d0en[pos], mask);

	switch (type) {
	case IRQ_TYPE_EDGE_FALLING:
		write32(&mt8173_eint->sens_clr.regs[pos], mask);
		write32(&mt8173_eint->pol_clr.regs[pos], mask);
		break;
	case IRQ_TYPE_EDGE_RISING:
		write32(&mt8173_eint->sens_clr.regs[pos], mask);
		write32(&mt8173_eint->pol_set.regs[pos], mask);
		break;
	case IRQ_TYPE_LEVEL_LOW:
		write32(&mt8173_eint->sens_set.regs[pos], mask);
		write32(&mt8173_eint->pol_clr.regs[pos], mask);
		break;
	case IRQ_TYPE_LEVEL_HIGH:
		write32(&mt8173_eint->sens_set.regs[pos], mask);
		write32(&mt8173_eint->pol_set.regs[pos], mask);
		break;
	}

	write32(&mt8173_eint->mask_clr.regs[pos], mask);
}
