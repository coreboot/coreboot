/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef __SOC_MEDIATEK_MT8189_INCLUDE_SOC_EFUSE_H__
#define __SOC_MEDIATEK_MT8189_INCLUDE_SOC_EFUSE_H__

#include <soc/addressmap.h>
#include <stdint.h>

struct efuse_regs {
	u32 reserved1[488];
	u32 cpu_id_reg;
	u32 reserved2[15];
	u32 cpu_seg_id_reg;
};
check_member(efuse_regs, cpu_id_reg, 0x7A0);
check_member(efuse_regs, cpu_seg_id_reg, 0x7E0);

static struct efuse_regs *const mtk_efuse = (void *)EFUSEC_BASE;

#endif /*__SOC_MEDIATEK_MT8189_INCLUDE_SOC_EFUSE_H__*/
