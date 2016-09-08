/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Marvell, Inc.
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

#ifndef __SOC_MARVELL_MVMAP2315_WDT_H__
#define __SOC_MARVELL_MVMAP2315_WDT_H__

#include <stdint.h>

#include <soc/addressmap.h>
#include <types.h>

#define MVMAP2315_R4_WDT			0
#define MVMAP2315_AP_WDT			1

#define MVMAP2315_WDT_CR_RPL_SHIFT		2
#define MVMAP2315_WDT_CR_RMOD			BIT(1)
#define MVMAP2315_WDT_CR_EN			BIT(0)
#define MVMAP2315_WDT_TORR_TOP_SHIFT		0
#define MVMAP2315_WDT_CRR_SHIFT			0
struct mvmap2315_wdt_regs {
	u32 wdt_cr;
	u32 wdt_torr;
	u32 wdt_ccvr;
	u32 wdt_crr;
	u32 wdt_stat;
	u32 wdt_eoi;
	u8 _reserved0[0xcc];
	u32 wdt_comp_params_5;
	u32 wdt_comp_params_4;
	u32 wdt_comp_params_3;
	u32 wdt_comp_params_2;
	u32 wdt_comp_params_1;
	u32 wdt_comp_version;
	u32 wdt_comp_type;
};

check_member(mvmap2315_wdt_regs, wdt_comp_type, 0xfc);


void enable_sp_watchdog(u32 timer, u32 timeout_value);
void reset_sp_watchdog(u32 timer, u32 timeout_value);

#endif /* __SOC_MARVELL_MVMAP2315_WDT_H__ */
