/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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

#include <boot/coreboot_tables.h>
#include <console/console.h>
#include <delay.h>
#include <gpio.h>
#include <string.h>
#include <vendorcode/google/chromeos/chromeos.h>

#define DEV_SW 15
#define DEV_POL ACTIVE_LOW
#define REC_SW 16
#define REC_POL ACTIVE_LOW
#define WP_SW  17
#define WP_POL ACTIVE_LOW

static int read_gpio(gpio_t gpio_num)
{
	gpio_tlmm_config_set(gpio_num, GPIO_FUNC_DISABLE,
			     GPIO_NO_PULL, GPIO_2MA, GPIO_DISABLE);
	udelay(10); /* Should be enough to settle. */
	return gpio_get(gpio_num);
}

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio *gpio;
	const int GPIO_COUNT = 5;

	gpios->size = sizeof(*gpios) + (GPIO_COUNT * sizeof(struct lb_gpio));
	gpios->count = GPIO_COUNT;

	gpio = gpios->gpios;
	fill_lb_gpio(gpio++, DEV_SW, ACTIVE_LOW, "developer", read_gpio(DEV_SW));
	fill_lb_gpio(gpio++, REC_SW, ACTIVE_LOW, "recovery", read_gpio(REC_SW));
	fill_lb_gpio(gpio++, WP_SW, ACTIVE_LOW, "write protect", read_gpio(WP_SW));
	fill_lb_gpio(gpio++, -1, ACTIVE_LOW, "power", 1);
	fill_lb_gpio(gpio++, -1, ACTIVE_LOW, "lid", 0);
}

int get_developer_mode_switch(void)
{
	return read_gpio(DEV_SW) ^ !DEV_POL;
}

int get_recovery_mode_switch(void)
{
	return read_gpio(REC_SW) ^ !REC_POL;
}

int get_write_protect_state(void)
{
	return read_gpio(WP_SW) ^ !WP_POL;
}
