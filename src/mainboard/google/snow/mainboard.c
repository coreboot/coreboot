/*
 * Copyright (C) 2012 Google Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
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

#include <stdlib.h>
#include <gpio.h>
#include <device/device.h>
#include <console/console.h>

#include <cpu/samsung/exynos5-common/gpio.h>
#include <cpu/samsung/exynos5250/gpio.h>

#include "mainboard.h"

#define SNOW_BOARD_ID0_GPIO	88	/* GPD0, pin 0 */
#define SNOW_BOARD_ID1_GPIO	89	/* GPD0, pin 1 */

struct {
	enum mvl3 id0, id1;
	enum snow_board_config config;
} snow_id_map[] = {
	/*  ID0      ID1         config */
	{ LOGIC_0, LOGIC_0, SNOW_CONFIG_SAMSUNG_MP },
	{ LOGIC_0, LOGIC_1, SNOW_CONFIG_ELPIDA_MP },
	{ LOGIC_1, LOGIC_0, SNOW_CONFIG_SAMSUNG_DVT },
	{ LOGIC_1, LOGIC_1, SNOW_CONFIG_ELPIDA_DVT },
	{ LOGIC_0, LOGIC_Z, SNOW_CONFIG_SAMSUNG_PVT },
	{ LOGIC_1, LOGIC_Z, SNOW_CONFIG_ELPIDA_PVT },
	{ LOGIC_Z, LOGIC_0, SNOW_CONFIG_SAMSUNG_MP },
	{ LOGIC_Z, LOGIC_Z, SNOW_CONFIG_ELPIDA_MP },
	{ LOGIC_Z, LOGIC_1, SNOW_CONFIG_RSVD },
};

int board_get_config(void)
{
	int i;
	int id0, id1;
	enum snow_board_config config = SNOW_CONFIG_UNKNOWN;

	id0 = gpio_read_mvl3(SNOW_BOARD_ID0_GPIO);
	id1 = gpio_read_mvl3(SNOW_BOARD_ID1_GPIO);
	if (id0 < 0 || id1 < 0)
		return -1;
	printk(BIOS_DEBUG, "%s: id0: %u, id1: %u\n", __func__, id0, id1);

	for (i = 0; i < ARRAY_SIZE(snow_id_map); i++) {
		if (id0 == snow_id_map[i].id0 && id1 == snow_id_map[i].id1) {
			config = snow_id_map[i].config;
			break;
		}
	}

	return config;
}

#if 0
struct chip_operations mainboard_ops = {
	.name	= "Samsung/Google ARM Chromebook",
	.enable_dev = mainboard_enable,
};
#endif
