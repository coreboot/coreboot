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
#include <arch/io.h>
#include <console/console.h>
#include <soc/sdram.h>
#include <soc/gpio.h>
#include <string.h>
#include <types.h>

static struct rk3288_sdram_params sdram_configs[] = {
#include "sdram_inf/sdram-lpddr3-samsung-2GB.inc"	/* ram_code = 0000 */
#include "sdram_inf/sdram-unused.inc"			/* ram_code = 0001 */
#include "sdram_inf/sdram-unused.inc"			/* ram_code = 0010 */
#include "sdram_inf/sdram-unused.inc"			/* ram_code = 0011 */
#include "sdram_inf/sdram-ddr3-samsung-2GB.inc"		/* ram_code = 0100 */
#include "sdram_inf/sdram-unused.inc"			/* ram_code = 0101 */
#include "sdram_inf/sdram-unused.inc"			/* ram_code = 0110 */
#include "sdram_inf/sdram-unused.inc"			/* ram_code = 0111 */
#include "sdram_inf/sdram-unused.inc"			/* ram_code = 1000 */
#include "sdram_inf/sdram-unused.inc"			/* ram_code = 1001 */
#include "sdram_inf/sdram-unused.inc"			/* ram_code = 1010 */
#include "sdram_inf/sdram-unused.inc"			/* ram_code = 1011 */
#include "sdram_inf/sdram-unused.inc"			/* ram_code = 1100 */
#include "sdram_inf/sdram-unused.inc"			/* ram_code = 1101 */
#include "sdram_inf/sdram-unused.inc"			/* ram_code = 1110 */
#include "sdram_inf/sdram-unused.inc"			/* ram_code = 1111 */
};

#define GPIO_RAMCODE0	GPIO(8, A, 0)
#define GPIO_RAMCODE1	GPIO(8, A, 1)
#define GPIO_RAMCODE2	GPIO(8, A, 2)
#define GPIO_RAMCODE3	GPIO(8, A, 3)

u32 sdram_get_ram_code(void)
{
	u32 code = 0;

	gpio_input(GPIO_RAMCODE0);
	gpio_input(GPIO_RAMCODE1);
	gpio_input(GPIO_RAMCODE2);
	gpio_input(GPIO_RAMCODE3);

	code = gpio_get_in_value(GPIO_RAMCODE3) << 3
			| gpio_get_in_value(GPIO_RAMCODE2) << 2
			| gpio_get_in_value(GPIO_RAMCODE1) << 1
			| gpio_get_in_value(GPIO_RAMCODE0) << 0;

	return code;
}

const struct rk3288_sdram_params *get_sdram_config()
{
	u32 ramcode = sdram_get_ram_code();

	if (ramcode >= ARRAY_SIZE(sdram_configs)
			|| sdram_configs[ramcode].dramtype == UNUSED)
		die("Invalid RAMCODE.");
	return &sdram_configs[ramcode];
}
