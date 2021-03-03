/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_MEDIATEK_DRAMC_COMMON_H__
#define __SOC_MEDIATEK_DRAMC_COMMON_H__

#include <console/console.h>

#define dramc_err(_x_...)  printk(BIOS_ERR, _x_)
#define dramc_info(_x_...) printk(BIOS_INFO, _x_)
#define dramc_show         dramc_info
#define dramc_dbg(_x_...) \
	do { \
		if (CONFIG(DEBUG_RAM_SETUP)) \
			printk(BIOS_INFO, _x_); \
	} while (0)

#endif
