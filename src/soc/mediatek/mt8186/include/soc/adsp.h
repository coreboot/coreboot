/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_MT8186_ADSP_H
#define SOC_MEDIATEK_MT8186_ADSP_H

#include <soc/addressmap.h>

struct mt8186_audiodsp_regs {
	u32 reserved1[1024];
	u32 audiodsp_adsp_ck_en;
};
check_member(mt8186_audiodsp_regs, audiodsp_adsp_ck_en, 0x1000);

static struct mt8186_audiodsp_regs *const mt8186_audiodsp = (void *)AUDIODSP_BASE;

void mtk_adsp_init(void);

#endif
