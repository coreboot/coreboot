/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef MAINBOARD_STARLABS_COMMON_MEMORY_H
#define MAINBOARD_STARLABS_COMMON_MEMORY_H

#include <console/console.h>
#include <option.h>

#define STARLABS_MEMORY_SPEED_MAX 2

static inline unsigned int starlabs_get_memory_speed_option(unsigned int default_speed)
{
	unsigned int speed = get_uint_option("memory_speed", default_speed);

	if (speed > STARLABS_MEMORY_SPEED_MAX) {
		printk(BIOS_WARNING, "Invalid memory_speed option %u, using %u\n", speed,
		       STARLABS_MEMORY_SPEED_MAX);
		speed = STARLABS_MEMORY_SPEED_MAX;
	}

	return speed;
}

#endif /* MAINBOARD_STARLABS_COMMON_MEMORY_H */
