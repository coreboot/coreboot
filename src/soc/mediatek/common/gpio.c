/*
 * This file is part of the coreboot project.
 *
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

#include <device/mmio.h>
#include <gpio.h>

enum {
	GPIO_DIRECTION_IN = 0,
	GPIO_DIRECTION_OUT = 1,
};

enum {
	GPIO_MODE = 0,
};

static void pos_bit_calc(gpio_t gpio, u32 *pos, u32 *bit)
{
	*pos = gpio.id / MAX_GPIO_REG_BITS;
	*bit = gpio.id % MAX_GPIO_REG_BITS;
}

static void pos_bit_calc_for_mode(gpio_t gpio, u32 *pos, u32 *bit)
{
	*pos = gpio.id / MAX_GPIO_MODE_PER_REG;
	*bit = (gpio.id % MAX_GPIO_MODE_PER_REG) * GPIO_MODE_BITS;
}

static s32 gpio_set_dir(gpio_t gpio, u32 dir)
{
	u32 pos;
	u32 bit;
	u32 *reg;

	pos_bit_calc(gpio, &pos, &bit);

	if (dir == GPIO_DIRECTION_IN)
		reg = &mtk_gpio->dir[pos].rst;
	else
		reg = &mtk_gpio->dir[pos].set;

	write32(reg, 1L << bit);

	return 0;
}

void gpio_set_mode(gpio_t gpio, int mode)
{
	u32 pos;
	u32 bit;
	u32 mask = (1L << GPIO_MODE_BITS) - 1;

	pos_bit_calc_for_mode(gpio, &pos, &bit);

	clrsetbits32(&mtk_gpio->mode[pos].val, mask << bit, mode << bit);
}

int gpio_get(gpio_t gpio)
{
	u32 pos;
	u32 bit;
	u32 *reg;
	u32 data;

	pos_bit_calc(gpio, &pos, &bit);

	reg = &mtk_gpio->din[pos].val;
	data = read32(reg);

	return (data & (1L << bit)) ? 1 : 0;
}

void gpio_set(gpio_t gpio, int output)
{
	u32 pos;
	u32 bit;
	u32 *reg;

	pos_bit_calc(gpio, &pos, &bit);

	if (output == 0)
		reg = &mtk_gpio->dout[pos].rst;
	else
		reg = &mtk_gpio->dout[pos].set;

	write32(reg, 1L << bit);
}

void gpio_input_pulldown(gpio_t gpio)
{
	gpio_set_pull(gpio, GPIO_PULL_ENABLE, GPIO_PULL_DOWN);
	gpio_set_dir(gpio, GPIO_DIRECTION_IN);
	gpio_set_mode(gpio, GPIO_MODE);
}

void gpio_input_pullup(gpio_t gpio)
{
	gpio_set_pull(gpio, GPIO_PULL_ENABLE, GPIO_PULL_UP);
	gpio_set_dir(gpio, GPIO_DIRECTION_IN);
	gpio_set_mode(gpio, GPIO_MODE);
}

void gpio_input(gpio_t gpio)
{
	gpio_set_pull(gpio, GPIO_PULL_DISABLE, GPIO_PULL_DOWN);
	gpio_set_dir(gpio, GPIO_DIRECTION_IN);
	gpio_set_mode(gpio, GPIO_MODE);
}

void gpio_output(gpio_t gpio, int value)
{
	gpio_set_pull(gpio, GPIO_PULL_DISABLE, GPIO_PULL_DOWN);
	gpio_set(gpio, value);
	gpio_set_dir(gpio, GPIO_DIRECTION_OUT);
	gpio_set_mode(gpio, GPIO_MODE);
}

enum {
	MAX_EINT_REG_BITS = 32,
};

static void pos_bit_calc_for_eint(gpio_t gpio, u32 *pos, u32 *bit)
{
	*pos = gpio.id / MAX_EINT_REG_BITS;
	*bit = gpio.id % MAX_EINT_REG_BITS;
}

int gpio_eint_poll(gpio_t gpio)
{
	u32 pos;
	u32 bit;
	u32 status;

	pos_bit_calc_for_eint(gpio, &pos, &bit);

	status = (read32(&mtk_eint->sta.regs[pos]) >> bit) & 0x1;

	if (status)
		write32(&mtk_eint->ack.regs[pos], 1 << bit);

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

	write32(&mtk_eint->d0en[pos], mask);

	switch (type) {
	case IRQ_TYPE_EDGE_FALLING:
		write32(&mtk_eint->sens_clr.regs[pos], mask);
		write32(&mtk_eint->pol_clr.regs[pos], mask);
		break;
	case IRQ_TYPE_EDGE_RISING:
		write32(&mtk_eint->sens_clr.regs[pos], mask);
		write32(&mtk_eint->pol_set.regs[pos], mask);
		break;
	case IRQ_TYPE_LEVEL_LOW:
		write32(&mtk_eint->sens_set.regs[pos], mask);
		write32(&mtk_eint->pol_clr.regs[pos], mask);
		break;
	case IRQ_TYPE_LEVEL_HIGH:
		write32(&mtk_eint->sens_set.regs[pos], mask);
		write32(&mtk_eint->pol_set.regs[pos], mask);
		break;
	}

	write32(&mtk_eint->mask_clr.regs[pos], mask);
}
