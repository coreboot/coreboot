/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The Chromium OS Authors. All rights reserved.
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

#include <stdint.h>
#include <string.h>
#include <arch/io.h>

#include "pch.h"
#include "gpio.h"

#define MAX_GPIO_NUMBER 75 /* zero based */

void setup_pch_gpios(const struct pch_gpio_map *gpio)
{
	u16 gpiobase = pci_read_config16(PCH_LPC_DEV, GPIO_BASE) & 0xfffc;

	/* GPIO Set 1 */
	if (gpio->set1.level)
		outl(*((u32*)gpio->set1.level), gpiobase + GP_LVL);
	if (gpio->set1.mode)
		outl(*((u32*)gpio->set1.mode), gpiobase + GPIO_USE_SEL);
	if (gpio->set1.direction)
		outl(*((u32*)gpio->set1.direction), gpiobase + GP_IO_SEL);
	if (gpio->set1.reset)
		outl(*((u32*)gpio->set1.reset), gpiobase + GP_RST_SEL1);
	if (gpio->set1.invert)
		outl(*((u32*)gpio->set1.invert), gpiobase + GPI_INV);
	if (gpio->set1.blink)
		outl(*((u32*)gpio->set1.blink), gpiobase + GPO_BLINK);

	/* GPIO Set 2 */
	if (gpio->set2.level)
		outl(*((u32*)gpio->set2.level), gpiobase + GP_LVL2);
	if (gpio->set2.mode)
		outl(*((u32*)gpio->set2.mode), gpiobase + GPIO_USE_SEL2);
	if (gpio->set2.direction)
		outl(*((u32*)gpio->set2.direction), gpiobase + GP_IO_SEL2);
	if (gpio->set2.reset)
		outl(*((u32*)gpio->set2.reset), gpiobase + GP_RST_SEL2);

	/* GPIO Set 3 */
	if (gpio->set3.level)
		outl(*((u32*)gpio->set3.level), gpiobase + GP_LVL3);
	if (gpio->set3.mode)
		outl(*((u32*)gpio->set3.mode), gpiobase + GPIO_USE_SEL3);
	if (gpio->set3.direction)
		outl(*((u32*)gpio->set3.direction), gpiobase + GP_IO_SEL3);
	if (gpio->set3.reset)
		outl(*((u32*)gpio->set3.reset), gpiobase + GP_RST_SEL3);
}

int get_gpio(int gpio_num)
{
	static const int gpio_reg_offsets[] = {0xc, 0x38, 0x48};
	u16 gpio_base = pci_read_config16(PCH_LPC_DEV, GPIO_BASE) & 0xfffc;
	int index, bit;

	if (gpio_num > MAX_GPIO_NUMBER)
		return 0; /* Just ignore wrong gpio numbers. */

	index = gpio_num / 32;
	bit = gpio_num % 32;

	return (inl(gpio_base + gpio_reg_offsets[index]) >> bit) & 1;
}

/*
 * get a number comprised of multiple GPIO values. gpio_num_array points to
 * the array of gpio pin numbers to scan, terminated by -1.
 */
unsigned get_gpios(const int *gpio_num_array)
{
	int gpio;
	unsigned bitmask = 1;
	unsigned vector = 0;

	while (bitmask &&
	       ((gpio = *gpio_num_array++) != -1)) {
		if (get_gpio(gpio))
			vector |= bitmask;
		bitmask <<= 1;
	}
	return vector;
}
