/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _XEON_SP_SOC_UTIL_H_
#define _XEON_SP_SOC_UTIL_H_

#include <console/console.h>
#include <hob_iiouds.h>

void get_cpubusnos(uint32_t *bus0, uint32_t *bus1, uint32_t *bus2, uint32_t *bus3);
void unlock_pam_regions(void);
void get_stack_busnos(uint32_t *bus);

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
