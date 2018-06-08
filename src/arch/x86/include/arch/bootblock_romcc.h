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

#include <cpu/x86/lapic/boot_cpu.c>

#ifdef CONFIG_BOOTBLOCK_RESETS
#include CONFIG_BOOTBLOCK_RESETS
#endif

#ifdef CONFIG_BOOTBLOCK_CPU_INIT
#include CONFIG_BOOTBLOCK_CPU_INIT
#endif
#ifdef CONFIG_BOOTBLOCK_NORTHBRIDGE_INIT
#include CONFIG_BOOTBLOCK_NORTHBRIDGE_INIT
#endif
#ifdef CONFIG_BOOTBLOCK_SOUTHBRIDGE_INIT
#include CONFIG_BOOTBLOCK_SOUTHBRIDGE_INIT
#endif

#ifdef CONFIG_BOOTBLOCK_MAINBOARD_INIT
#include CONFIG_BOOTBLOCK_MAINBOARD_INIT
#else
static void bootblock_mainboard_init(void)
{
#ifdef CONFIG_BOOTBLOCK_NORTHBRIDGE_INIT
	bootblock_northbridge_init();
#endif
#ifdef CONFIG_BOOTBLOCK_SOUTHBRIDGE_INIT
	bootblock_southbridge_init();
#endif
#ifdef CONFIG_BOOTBLOCK_CPU_INIT
	bootblock_cpu_init();
#endif
}
#endif
