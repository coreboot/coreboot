/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8186 Functional Specification
 * Chapter number: 4.3
 */

#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/gic.h>

void mtk_gic_preinit(void)
{
	int i;

	for (i = 3; i < 15; i++) {
		write32((void *)((uintptr_t)MCUSYS_BASE + 0xA600 + i * 4), 0);
		write32((void *)((uintptr_t)MCUSYS_BASE + 0xA650 + i * 4), 0xFFFFFFFF);
	}
}
