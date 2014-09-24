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

#include <base3.h>
#include <boardid.h>
#include <console/console.h>
#include <stdlib.h>

#include "gpio.h"

struct id_to_str {
	const char *str;
	int tri_state_value;
	int normalized_id;
};

static const struct id_to_str bdid_map[] = {
	{ "PROTO 0",	BASE3(0, 0),	BOARD_ID_PROTO_0 },
	{ "PROTO 1",	BASE3(0, 1),	BOARD_ID_PROTO_1 },
	{ "EVT",	BASE3(0, Z),	BOARD_ID_EVT },
	{ "DVT",	BASE3(1, 0),	BOARD_ID_DVT },
	{ "PVT",	BASE3(1, 1),	BOARD_ID_PVT },
	{ "MP",		BASE3(1, Z),	BOARD_ID_MP },
	{ "Z0",		BASE3(Z, 0),	-1 },
	{ "Z1",		BASE3(Z, 1),	-1 },
	{ "ZZ",		BASE3(Z, Z),	-1 },
};

uint8_t board_id(void)
{
	static int id = -1;

	if (id < 0) {
		const char *idstr = "Unknown";
		int i;
		int tristate_id;
		gpio_t gpio[] = {[1] = BD_ID1, [0] = BD_ID0};	/* ID0 is LSB */

		tristate_id = gpio_get_tristates(gpio, ARRAY_SIZE(gpio));

		for (i = 0; i < ARRAY_SIZE(bdid_map); i++) {
			if (tristate_id != bdid_map[i].tri_state_value)
				continue;
			idstr = bdid_map[i].str;
			id = bdid_map[i].normalized_id;
			break;
		}

		printk(BIOS_SPEW, "Board ID: '%s' %d (%#x)\n", idstr, id,
			tristate_id);
	}
	return id;
}
