/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/adsp.h>

void mtk_adsp_init(void)
{
	/* [0] CORE_CLK_EN, [1] COREDBG_EN, [3] TIMER_EN, [4] DMA_EN, [5] UART_EN */
	setbits32(&mt8186_audiodsp->audiodsp_adsp_ck_en, 0x0000003B);
}
