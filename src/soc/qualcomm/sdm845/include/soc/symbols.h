/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SOC_QUALCOMM_SDM845_SYMBOLS_H_
#define _SOC_QUALCOMM_SDM845_SYMBOLS_H_

#include <symbols.h>

DECLARE_REGION(ssram)
DECLARE_REGION(bsram)
DECLARE_REGION(dram_reserved)
DECLARE_REGION(dcb);
DECLARE_REGION(pmic);
DECLARE_REGION(limits_cfg);
DECLARE_REGION(aop);
DECLARE_REGION(aop_ss_msg_ram_drv15);

#endif // _SOC_QUALCOMM_SDM845_SYMBOLS_H_
