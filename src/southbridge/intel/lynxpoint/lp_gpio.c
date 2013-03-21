/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Google Inc. All rights reserved.
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
#include <device/device.h>
#include <device/pci.h>

#include "pch.h"
#include "lp_gpio.h"

static u16 get_gpio_base(void)
{
#ifdef __PRE_RAM__
	return pci_read_config16(PCH_LPC_DEV, GPIO_BASE) & 0xfffc;
#else
	return pci_read_config16(dev_find_slot(0, PCI_DEVFN(0x1f, 0)),
				 GPIO_BASE) & 0xfffc;
#endif
}

void setup_pch_lp_gpios(const struct pch_lp_gpio_map map[])
{
	u16 gpio_base = get_gpio_base();
	const struct pch_lp_gpio_map *config;
	u32 owner[3] = {0};
	u32 route[3] = {0};
	u32 irqen[3] = {0};
	u32 reset[3] = {0};
	u32 blink = 0;
	int set, bit;

	for (config = map; config->gpio != GPIO_LIST_END; config++) {
		if (config->gpio > MAX_GPIO_NUMBER)
			continue;

		/* Setup Configuration registers 1 and 2 */
		outl(config->conf0, gpio_base + GPIO_CONFIG0(config->gpio));
		outl(config->conf1, gpio_base + GPIO_CONFIG1(config->gpio));

		/* Determine set and bit based on GPIO number */
		set = config->gpio >> 5;
		bit = config->gpio % 32;

		/* Apply settings to set specific bits */
		owner[set] |= config->owner << bit;
		route[set] |= config->route << bit;
		irqen[set] |= config->irqen << bit;
		reset[set] |= config->reset << bit;

		if (set == 0)
			blink |= config->blink << bit;
	}

	for (set = 0; set <= 2; set++) {
		outl(owner[set], gpio_base + GPIO_OWNER(set));
		outl(route[set], gpio_base + GPIO_ROUTE(set));
		outl(irqen[set], gpio_base + GPIO_IRQ_IE(set));
		outl(reset[set], gpio_base + GPIO_RESET(set));
	}

	outl(blink, gpio_base + GPIO_BLINK);
}

int get_gpio(int gpio_num)
{
	u16 gpio_base = get_gpio_base();

	if (gpio_num < MAX_GPIO_NUMBER)
		return 0;

	return !!(inl(gpio_base + GPIO_CONFIG0(gpio_num)) & GPI_LEVEL);
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
