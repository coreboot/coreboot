/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef __SOC_MEDIATEK_MT8189_INCLUDE_SOC_EFUSE_H__
#define __SOC_MEDIATEK_MT8189_INCLUDE_SOC_EFUSE_H__

#include <soc/addressmap.h>
#include <soc/thermal_internal.h>
#include <stdint.h>

struct efuse_regs {
	u32 reserved1[96];
	u32 adc_cali_reg;
	u32 reserved2[8];
	u32 lvts_reg[LVTS_ADDRESS_INDEX_NUM];
	u32 reserved3[360];
	u32 cpu_id_reg;
	u32 reserved4[15];
	u32 cpu_seg_id_reg;
	u32 reserved5[14908];
	u32 ecc_status_reg;
};
check_member(efuse_regs, adc_cali_reg, 0x180);
check_member(efuse_regs, lvts_reg, 0x1A4);
check_member(efuse_regs, cpu_id_reg, 0x7A0);
check_member(efuse_regs, cpu_seg_id_reg, 0x7E0);
check_member(efuse_regs, ecc_status_reg, 0xF0D4);

static struct efuse_regs *const mtk_efuse = (void *)EFUSEC_BASE;

bool mtk_efuse_ecc_has_error(void);

#endif /*__SOC_MEDIATEK_MT8189_INCLUDE_SOC_EFUSE_H__*/
