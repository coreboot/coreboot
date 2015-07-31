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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */
#include <arch/io.h>
#include <assert.h>
#include <console/console.h>
#include <gpio.h>
#include <types.h>
#include <soc/addressmap.h>
#include <soc/gpio.h>

enum {
	MAX_8173_GPIO = 134,
	MAX_GPIO_REG_BITS = 16,
	MAX_GPIO_MODE_PER_REG = 5,
	GPIO_MODE_BITS = 3,
};

enum {
	GPIO_DIRECTION_IN = 0,
	GPIO_DIRECTION_OUT = 1,
};

enum {
	GPIO_MODE = 0,
};

static void pos_bit_calc(u32 pin, u32 *pos, u32 *bit)
{
	*pos = pin / MAX_GPIO_REG_BITS;
	*bit = pin % MAX_GPIO_REG_BITS;
}

static void pos_bit_calc_for_mode(u32 pin, u32 *pos, u32 *bit)
{
	*pos = pin / MAX_GPIO_MODE_PER_REG;
	*bit = (pin % MAX_GPIO_MODE_PER_REG) * GPIO_MODE_BITS;
}

static s32 gpio_set_dir(u32 pin, u32 dir)
{
	u32 pos;
	u32 bit;
	u16 *reg;

	assert(pin <= MAX_8173_GPIO);

	pos_bit_calc(pin, &pos, &bit);

	if (dir == GPIO_DIRECTION_IN)
		reg = &mt8173_gpio->dir[pos].rst;
	else
		reg = &mt8173_gpio->dir[pos].set;

	write16(reg, 1L << bit);

	return 0;
}

void gpio_set_pull(gpio_t pin, enum pull_enable enable,
		   enum pull_select select)
{
	u32 pos;
	u32 bit;
	u16 *en_reg, *sel_reg;

	assert(pin <= MAX_8173_GPIO);

	pos_bit_calc(pin, &pos, &bit);

	if (enable == GPIO_PULL_DISABLE) {
		en_reg = &mt8173_gpio->pullen[pos].rst;
	} else {
	/* These pins' pulls can't be set through GPIO controller. */
		assert(pin < 22 || pin > 27);
		assert(pin < 47 || pin > 56);
		assert(pin < 57 || pin > 68);
		assert(pin < 73 || pin > 78);
		assert(pin < 100 || pin > 105);
		assert(pin < 119 || pin > 124);

		en_reg = &mt8173_gpio->pullen[pos].set;
		sel_reg = (select == GPIO_PULL_DOWN) ?
			  (&mt8173_gpio->pullsel[pos].rst) :
			  (&mt8173_gpio->pullsel[pos].set);
		write16(sel_reg, 1L << bit);
	}
	write16(en_reg, 1L << bit);
}

int gpio_get(gpio_t pin)
{
	u32 pos;
	u32 bit;
	u16 *reg;
	s32 data;

	assert(pin <= MAX_8173_GPIO);

	pos_bit_calc(pin, &pos, &bit);

	reg = &mt8173_gpio->din[pos].val;
	data = read32(reg);

	return (data & (1L << bit)) ? 1 : 0;
}

void gpio_set(gpio_t pin, int output)
{
	u32 pos;
	u32 bit;
	u16 *reg;

	assert(pin <= MAX_8173_GPIO);

	pos_bit_calc(pin, &pos, &bit);

	if (output == 0)
		reg = &mt8173_gpio->dout[pos].rst;
	else
		reg = &mt8173_gpio->dout[pos].set;
	write16(reg, 1L << bit);
}

void gpio_set_mode(gpio_t pin, int mode)
{
	u32 pos;
	u32 bit;
	u32 mask = (1L << GPIO_MODE_BITS) - 1;

	assert(pin <= MAX_8173_GPIO);

	pos_bit_calc_for_mode(pin, &pos, &bit);

	clrsetbits_le32(&mt8173_gpio->mode[pos].val,
			mask << bit, mode << bit);
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
