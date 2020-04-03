/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_UTIL_H_
#define _SOC_UTIL_H_

#include <console/console.h>
#include <hob_iiouds.h>
#include <hob_memmap.h>
#include <stdint.h>

#define DEV_FUNC_ENTER(dev) \
	printk(BIOS_SPEW, "%s:%s:%d: ENTER (dev: %s)\n", \
		__FILE__, __func__, __LINE__, dev_path(dev))

#define DEV_FUNC_EXIT(dev) \
	printk(BIOS_SPEW, "%s:%s:%d: EXIT (dev: %s)\n", __FILE__, \
		__func__, __LINE__, dev_path(dev))

struct iiostack_resource {
	uint8_t     no_of_stacks;
	STACK_RES   res[MAX_SOCKET * MAX_LOGIC_IIO_STACK];
};

uint8_t get_iiostack_info(struct iiostack_resource *info);
#endif /* _SOC_UTIL_H_ */
