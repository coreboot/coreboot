/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8188 Functional Specification
 * Chapter number: 5.2
 */

#ifndef __SOC_MEDIATEK_MT8188_IOCFG_H__
#define __SOC_MEDIATEK_MT8188_IOCFG_H__

#include <soc/addressmap.h>
#include <types.h>

struct mt8188_iocfg_lt_regs {
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
	u32 reserved4[5];
	u32 eh_cfg;
	u32 eh_cfg_set;
	u32 eh_cfg_clr;
	u32 reserved5[9];
	u32 ies_cfg1;
	u32 ies_cfg1_set;
	u32 ies_cfg1_clr;
	u32 reserved6[9];
	u32 pd_cfg1;
	u32 pd_cfg1_set;
	u32 pd_cfg1_clr;
	u32 reserved7[9];
	u32 pu_cfg1;
	u32 pu_cfg1_set;
	u32 pu_cfg1_clr;
	u32 reserved8[21];
	u32 rdsel_cfg3;
	u32 rdsel_cfg3_set;
	u32 rdsel_cfg3_clr;
	u32 reserved9[5];
	u32 smt_cfg0;
	u32 smt_cfg0_set;
	u32 smt_cfg0_clr;
	u32 reserved10[17];
	u32 tdsel_cfg3;
	u32 tdsel_cfg3_set;
	u32 tdsel_cfg3_clr;
};
check_member(mt8188_iocfg_lt_regs, drv_cfg1, 0x10);
check_member(mt8188_iocfg_lt_regs, drv_cfg2, 0x20);
check_member(mt8188_iocfg_lt_regs, drv_cfg3, 0x30);
check_member(mt8188_iocfg_lt_regs, eh_cfg, 0x50);
check_member(mt8188_iocfg_lt_regs, ies_cfg1, 0x80);
check_member(mt8188_iocfg_lt_regs, pd_cfg1, 0xB0);
check_member(mt8188_iocfg_lt_regs, pu_cfg1, 0xE0);
check_member(mt8188_iocfg_lt_regs, rdsel_cfg3, 0x140);
check_member(mt8188_iocfg_lt_regs, smt_cfg0, 0x160);
check_member(mt8188_iocfg_lt_regs, tdsel_cfg3, 0x1B0);

#define mtk_iocfg_lt    ((struct mt8188_iocfg_lt_regs *)IOCFG_LT_BASE)

enum {
	IO_4_MA = 0x1,
	IO_6_MA = 0x2,
};

#endif /* __SOC_MEDIATEK_MT8188_IOCFG_H__ */
