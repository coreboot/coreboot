/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef SOC_MEDIATEK_MT8195_APUSYS_H
#define SOC_MEDIATEK_MT8195_APUSYS_H

#include <soc/addressmap.h>
#include <types.h>

struct mt8195_apu_mbox_regs {
	u32 mbox_in[8];
	u32 mbox_out[8];
	u32 mbox_reserved1[28];
	u32 mbox_func_cfg;
	u32 mbox0_reserved2[19];
};

check_member(mt8195_apu_mbox_regs, mbox_func_cfg, 0x0b0);

static struct mt8195_apu_mbox_regs * const mt8195_apu_mbox[] = {
	(void *)APU_MBOX_BASE,
	(void *)(APU_MBOX_BASE + 0x100),
	(void *)(APU_MBOX_BASE + 0x200),
	(void *)(APU_MBOX_BASE + 0x300),
	(void *)(APU_MBOX_BASE + 0x400),
	(void *)(APU_MBOX_BASE + 0x500),
	(void *)(APU_MBOX_BASE + 0x600),
	(void *)(APU_MBOX_BASE + 0x700),
};

void apusys_init(void);
#endif  /* SOC_MEDIATEK_MT8195_APUSYS_H */
