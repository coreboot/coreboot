/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _MAINBOARD_GPIO_DEFS_H
#define _MAINBOARD_GPIO_DEFS_H

#include <soc/gpio_defs.h>

// _GPIO_0 : LFFF: DVT_GPIO<0> : BOOTED
#define GPIO_GPIO_0				0
#define R_PAD_CFG_DW0_GPIO_0			0x4d8
#define PID_GPIO_0				PID_NorthCommunity

// _GPIO_4 : LFFF: M2A_CFGn : M2A_SATAn
#define GPIO_GPIO_4				4
#define R_PAD_CFG_DW0_GPIO_4			0x568
#define PID_GPIO_4				PID_SouthCommunity

// _GPIO_5 : LFFF: M2B_CFGn : M2B_SATAn
#define GPIO_GPIO_5				5
#define R_PAD_CFG_DW0_GPIO_5			0x570
#define PID_GPIO_5				PID_SouthCommunity


// _GPIO_8 : LFFF: DVT_GPIO<1> : Baud select
#define GPIO_GPIO_8				8
#define R_PAD_CFG_DW0_GPIO_8			0x5c8
#define PID_GPIO_8				PID_SouthCommunity

// _GPIO_9 : LFFF: DVT_GPIO<2> : BIOS Verbose
#define GPIO_GPIO_9				9
#define R_PAD_CFG_DW0_GPIO_9			0x5d0
#define PID_GPIO_9				PID_SouthCommunity

#endif /* _MAINBOARD_GPIO_DEFS_H */
