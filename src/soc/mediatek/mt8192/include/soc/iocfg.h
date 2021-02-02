/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_MEDIATEK_MT8192_IOCFG_H__
#define __SOC_MEDIATEK_MT8192_IOCFG_H__

#include <soc/addressmap.h>
#include <types.h>

struct mt8192_iocfg_lm_regs {
	u32 reserved[4];
	u32 drv_cfg1;
};
check_member(mt8192_iocfg_lm_regs, drv_cfg1, 0x10);

enum {
	IO_4_MA = 0x1,
};

#define mtk_iocfg_lm    ((struct mt8192_iocfg_lm_regs *)IOCFG_LM_BASE)

struct mt8192_iocfg_bm_regs {
	u32 reserved[8];
	u32 drv_cfg2;
};
check_member(mt8192_iocfg_bm_regs, drv_cfg2, 0x20);

#define mtk_iocfg_bm	((struct mt8192_iocfg_bm_regs *)IOCFG_BM_BASE)

#endif /* __SOC_MEDIATEK_MT8192_IOCFG_H__ */
