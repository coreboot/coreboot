/*
 * This file is part of the coreboot project.
 *
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

#include <types.h>

enum {
	PMIC_SWCID                    = 0x000a,
	PMIC_VM_MODE                  = 0x004e,
	PMIC_TOP_CKPDN_CON0_SET       = 0x010e,
	PMIC_TOP_CKPDN_CON0_CLR       = 0x0110,
	PMIC_TOP_CKHWEN_CON0_SET      = 0x012c,
	PMIC_TOP_CKHWEN_CON0_CLR      = 0x012e,
	PMIC_TOP_RST_MISC             = 0x014c,
	PMIC_TOP_RST_MISC_SET         = 0x014e,
	PMIC_TOP_RST_MISC_CLR         = 0x0150,
	PMIC_OTP_CON0                 = 0x038a,
	PMIC_OTP_CON8                 = 0x039a,
	PMIC_OTP_CON11                = 0x03a0,
	PMIC_OTP_CON12                = 0x03a2,
	PMIC_OTP_CON13                = 0x03a4,
	PMIC_TOP_TMA_KEY              = 0x03a8,
	PMIC_PWRHOLD                  = 0x0a08,
	PMIC_CPSDSA4                  = 0x0a2e,
	PMIC_VCORE_OP_EN              = 0x1490,
	PMIC_VCORE_DBG0               = 0x149e,
	PMIC_VCORE_VOSEL              = 0x14aa,
	PMIC_VDRAM1_VOSEL_SLEEP       = 0x160a,
	PMIC_VDRAM1_OP_EN             = 0x1610,
	PMIC_VDRAM1_DBG0              = 0x161e,
	PMIC_VDRAM1_VOSEL             = 0x1626,
	PMIC_SMPS_ANA_CON0            = 0x1808,
	PMIC_VDDQ_OP_EN               = 0x1b16,
	PMIC_VSIM2_ANA_CON0           = 0x1e30,
	PMIC_VDDQ_ELR_0               = 0x1ec4,
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
void pmic_init_scp_voltage(void);
unsigned int pmic_get_vcore_vol(void);
void pmic_set_vcore_vol(unsigned int vcore_uv);
unsigned int pmic_get_vdram1_vol(void);
void pmic_set_vdram1_vol(unsigned int vdram_uv);
unsigned int pmic_get_vddq_vol(void);
void pmic_set_vddq_vol(unsigned int vddq_uv);

#endif /* __SOC_MEDIATEK_MT6358_H__ */
