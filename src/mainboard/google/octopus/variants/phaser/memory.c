/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <boardid.h>
#include <gpio.h>
#include <variant/gpio.h>

size_t variant_memory_sku(void)
{
	size_t rt;
	gpio_t pads[] = {
		[3] = MEM_CONFIG3, [2] = MEM_CONFIG2,
		[1] = MEM_CONFIG1, [0] = MEM_CONFIG0,
	};

	rt = gpio_base2_value(pads, ARRAY_SIZE(pads));

	if (board_id() == 7)
		return (rt == 3) ? 1 : rt;  // If RAM ID = 3, return 1
	else
		return rt;
}
