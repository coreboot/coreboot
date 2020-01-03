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
#if CONFIG(POST_IO)
#include <arch/io.h>
#endif

/* Write POST information */

/* Some mainboards have very nice features beyond just a simple display.
 * They can override this function.
 */
void __weak mainboard_post(uint8_t value) { }

void post_code(uint8_t value)
{
#if !CONFIG(NO_POST)
#if CONFIG(CONSOLE_POST)
	printk(BIOS_EMERG, "POST: 0x%02x\n", value);
#endif
#if CONFIG(CMOS_POST)
	cmos_post_code(value);
#endif
#if CONFIG(POST_IO)
	outb(value, CONFIG_POST_IO_PORT);
#endif
#endif
	mainboard_post(value);
}
