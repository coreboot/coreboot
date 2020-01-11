/*
 * This file is part of the coreboot project.
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
 */

#include <stdint.h>
#include <console/console.h>
#include <arch/io.h>

void arch_post_code(uint8_t value)
{
	if (CONFIG(POST_IO))
		outb(value, CONFIG_POST_IO_PORT);

	if (CONFIG(CMOS_POST) && !ENV_SMM)
		cmos_post_code(value);
}
