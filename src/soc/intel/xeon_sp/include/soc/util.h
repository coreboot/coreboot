/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _XEON_SP_SOC_UTIL_H_
#define _XEON_SP_SOC_UTIL_H_

#include <console/console.h>

void get_cpubusnos(uint32_t *bus0, uint32_t *bus1, uint32_t *bus2, uint32_t *bus3);
void unlock_pam_regions(void);
void get_stack_busnos(uint32_t *bus);

#define LOG_MEM_RESOURCE(type, dev, index, base_kb, size_kb) \
	printk(BIOS_SPEW, "%s:%d res: %s, dev: %s, index: 0x%x, base: 0x%llx, " \
		"end: 0x%llx, size_kb: 0x%llx\n", \
		__func__, __LINE__, type, dev_path(dev), index, (base_kb << 10), \
		(base_kb << 10) + (size_kb << 10) - 1, size_kb)

#define LOG_IO_RESOURCE(type, dev, index, base, size) \
	printk(BIOS_SPEW, "%s:%d res: %s, dev: %s, index: 0x%x, base: 0x%llx, " \
		"end: 0x%llx, size: 0x%llx\n", \
		__func__, __LINE__, type, dev_path(dev), index, base, base + size - 1, size)

#define DEV_FUNC_ENTER(dev) \
	printk(BIOS_SPEW, "%s:%s:%d: ENTER (dev: %s)\n", \
		__FILE__, __func__, __LINE__, dev_path(dev))

#define DEV_FUNC_EXIT(dev) \
	printk(BIOS_SPEW, "%s:%s:%d: EXIT (dev: %s)\n", __FILE__, \
		__func__, __LINE__, dev_path(dev))

#define FUNC_ENTER() \
	printk(BIOS_SPEW, "%s:%s:%d: ENTER\n", __FILE__, __func__, __LINE__)

#define FUNC_EXIT() \
	printk(BIOS_SPEW, "%s:%s:%d: EXIT\n", __FILE__, __func__, __LINE__)

#endif
