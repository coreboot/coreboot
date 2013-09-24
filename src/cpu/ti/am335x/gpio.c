/*
 * Copyright 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <arch/io.h>
#include <console/console.h>
#include <cpu/ti/am335x/gpio.h>
#include <stdint.h>
#include <stdlib.h>

static struct am335x_gpio_regs *gpio_regs_and_bit(unsigned gpio, uint32_t *bit)
{
	unsigned bank = gpio / AM335X_GPIO_BITS_PER_BANK;
	if (bank > ARRAY_SIZE(am335x_gpio_banks)) {
		printk(BIOS_ERR, "Bad gpio index %d.\n", gpio);
		return NULL;
	}
	*bit = 1 << (gpio % 32);
	return am335x_gpio_banks[bank];
}

void am335x_disable_gpio_irqs(void)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(am335x_gpio_banks); i++) 
		writel(0xffffffff, &am335x_gpio_banks[i]->irqstatus_clr_0);
}

int gpio_direction_input(unsigned gpio)
{
	uint32_t bit;
	struct am335x_gpio_regs *regs = gpio_regs_and_bit(gpio, &bit);
	if (!regs)
		return -1;
	setbits_le32(&regs->oe, bit);
	return 0;
}

int gpio_direction_output(unsigned gpio, int value)
{
	uint32_t bit;
	struct am335x_gpio_regs *regs = gpio_regs_and_bit(gpio, &bit);
	if (!regs)
		return -1;
	if (value)
		writel(bit, &regs->setdataout);
	else
		writel(bit, &regs->cleardataout);
	clrbits_le32(&regs->oe, bit);
	return 0;
}

int gpio_get_value(unsigned gpio)
{
	uint32_t bit;
	struct am335x_gpio_regs *regs = gpio_regs_and_bit(gpio, &bit);
	if (!regs)
		return -1;
	return (readl(&regs->datain) & bit) ? 1 : 0;
}

int gpio_set_value(unsigned gpio, int value)
{
	uint32_t bit;
	struct am335x_gpio_regs *regs = gpio_regs_and_bit(gpio, &bit);
	if (!regs)
		return -1;
	if (value)
		writel(bit, &regs->setdataout);
	else
		writel(bit, &regs->cleardataout);
	return 0;
}
