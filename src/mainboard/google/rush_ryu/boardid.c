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

#include <console/console.h>
#include <soc/nvidia/tegra132/gpio.h>
#include <stdlib.h>

#include <boardid.h>
#include "gpio.h"

/*
 * +------------------+---------+
 * | BD_ID_STRAP[1:0] |  PHASE  |
 * +------------------+---------+
 * |         00       |  PROTO0 |
 * +------------------+---------+
 * |         01       |  PROTO1 |
 * +------------------+---------+
 * |         0Z       |   EVT   |
 * +------------------+---------+
 * |         10       |   DVT   |
 * +------------------+---------+
 * |         11       |   PVT   |
 * +------------------+---------+
 * |         1Z       |   MP    |
 * +------------------+---------+
 * |         Z0       |         |
 * +------------------+---------+
 * |         Z1       |         |
 * +------------------+---------+
 * |         ZZ       |         |
 * +------------------+---------+
 */
struct id_to_str {
	const char *str;
	int tri_state_value;
	int normalized_id;
};

static const struct id_to_str bdid_map[] = {
	{ "PROTO 0", 0x00, BOARD_ID_PROTO_0 },
	{ "PROTO 1", 0x01, BOARD_ID_PROTO_1 },
	{ "EVT", 0x02, BOARD_ID_EVT },
	{ "DVT", 0x04, BOARD_ID_DVT },
	{ "PVT", 0x05, BOARD_ID_PVT },
	{ "MP", 0x06, BOARD_ID_MP },
	{ "Z0", 0x08, -1 },
	{ "Z1", 0x09, -1 },
	{ "ZZ", 0x0a, -1 },
};

uint8_t board_id(void)
{
	static int id = -1;

	if (id < 0) {
		const char *idstr = "Unknown";
		int i;
		int tristate_id;
		gpio_t gpio[] = { BD_ID0, BD_ID1 };

		tristate_id = gpio_get_in_tristate_values(gpio,
							ARRAY_SIZE(gpio), 0);

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
