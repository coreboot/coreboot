/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef __SOC_MEDIATEK_MT8195_IOCFG_H__
#define __SOC_MEDIATEK_MT8195_IOCFG_H__

#include <soc/addressmap.h>
#include <types.h>

struct mt8195_iocfg_bm_regs {
	u32 reserved1[4];
	u32 drv_cfg1;
	u32 drv_cfg1_set;
	u32 drv_cfg1_clr;
	u32 reserved2;
	u32 drv_cfg2;
	u32 drv_cfg2_set;
	u32 drv_cfg2_clr;
	u32 reserved3;
	u32 drv_cfg3;
	u32 drv_cfg3_set;
	u32 drv_cfg3_clr;
	u32 reserved4[1];
	u32 eh_cfg;
	u32 eh_cfg_set;
	u32 eh_cfg_clr;
	u32 reserved5[9];
	u32 ies_cfg1;
	u32 ies_cfg1_set;
	u32 ies_cfg1_clr;
	u32 reserved6[5];
	u32 pd_cfg1;
	u32 pd_cfg1_set;
	u32 pd_cfg1_clr;
	u32 reserved7[5];
	u32 pu_cfg1;
	u32 pu_cfg1_set;
	u32 pu_cfg1_clr;
	u32 reserved8[1];
	u32 rdsel_cfg0;
	u32 rdsel_cfg0_set;
	u32 rdsel_cfg0_clr;
	u32 reserved9[9];
	u32 smt_cfg0;
	u32 smt_cfg0_set;
	u32 smt_cfg0_clr;
	u32 reserved10[5];
	u32 tdsel_cfg1;
	u32 tdsel_cfg1_set;
	u32 tdsel_cfg1_clr;
};
check_member(mt8195_iocfg_bm_regs, drv_cfg1, 0x10);
check_member(mt8195_iocfg_bm_regs, drv_cfg2, 0x20);
check_member(mt8195_iocfg_bm_regs, drv_cfg3, 0x30);
check_member(mt8195_iocfg_bm_regs, eh_cfg, 0x40);
check_member(mt8195_iocfg_bm_regs, ies_cfg1, 0x70);
check_member(mt8195_iocfg_bm_regs, pd_cfg1, 0x90);
check_member(mt8195_iocfg_bm_regs, pu_cfg1, 0xB0);
check_member(mt8195_iocfg_bm_regs, rdsel_cfg0, 0xC0);
check_member(mt8195_iocfg_bm_regs, smt_cfg0, 0xF0);
check_member(mt8195_iocfg_bm_regs, tdsel_cfg1, 0x110);

#define mtk_iocfg_bm    ((struct mt8195_iocfg_bm_regs *)IOCFG_BM_BASE)

enum {
	IO_4_MA = 0x9,
	IO_6_MA = 0x1b,
};
#endif /* __SOC_MEDIATEK_MT8195_IOCFG_H__ */
