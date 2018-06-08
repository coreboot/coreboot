/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <smp/node.h>
#include <arch/bootblock_romcc.h>
#include <pc80/mc146818rtc.h>
#include <halt.h>

static void main(unsigned long bist)
{
	if (boot_cpu()) {
		bootblock_mainboard_init();

		sanitize_cmos();
#if IS_ENABLED(CONFIG_CMOS_POST)
		cmos_post_init();
#endif
	}

#if IS_ENABLED(CONFIG_VBOOT_SEPARATE_VERSTAGE)
	const char *target1 = "fallback/verstage";
#else
	const char *target1 = "fallback/romstage";
#endif

	unsigned long entry;
	entry = findstage(target1);
	if (entry)
		call(entry, bist);
	halt();
}
