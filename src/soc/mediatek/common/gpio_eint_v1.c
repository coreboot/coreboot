/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/addressmap.h>
#include <gpio.h>

void gpio_calc_eint_pos_bit(gpio_t gpio, u32 *pos, u32 *bit)
{
	*pos = gpio.id / MAX_EINT_REG_BITS;
	*bit = gpio.id % MAX_EINT_REG_BITS;
}

struct eint_regs *gpio_get_eint_reg(gpio_t gpio)
{
	return (struct eint_regs *)(EINT_BASE);
}
