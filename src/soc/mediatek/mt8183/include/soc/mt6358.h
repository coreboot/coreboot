/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 MediaTek Inc.
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

#ifndef __SOC_MEDIATEK_MT6358_H__
#define __SOC_MEDIATEK_MT6358_H__

enum {
	PMIC_SWCID                    = 0x000a,
	PMIC_VM_MODE                  = 0x004e,
	PMIC_TOP_RST_MISC             = 0x014c,
	PMIC_TOP_RST_MISC_SET         = 0x014e,
	PMIC_TOP_RST_MISC_CLR         = 0x0150,
	PMIC_TOP_TMA_KEY              = 0x03a8,
	PMIC_PWRHOLD                  = 0x0a08,
	PMIC_CPSDSA4                  = 0x0a2e,
	PMIC_VDRAM1_VOSEL_SLEEP       = 0x160a,
	PMIC_SMPS_ANA_CON0            = 0x1808,
	PMIC_VSIM2_ANA_CON0           = 0x1e30,
};

struct pmic_setting {
	unsigned short addr;
	unsigned short val;
	unsigned short mask;
	unsigned char shift;
};

void mt6358_init(void);
void pmic_set_power_hold(bool enable);
void pmic_set_vsim2_cali(unsigned int vsim2_mv);

#endif /* __SOC_MEDIATEK_MT6358_H__ */
