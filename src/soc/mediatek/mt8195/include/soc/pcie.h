/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef SOC_MEDIATEK_MT8195_PCIE_H
#define SOC_MEDIATEK_MT8195_PCIE_H

#include <soc/pcie_common.h>
#include <types.h>

void mtk_pcie_reset(uintptr_t reg, bool enable);
void mtk_pcie_pre_init(void);

bool mainboard_needs_pcie_init(void);

#endif
