/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <console/console.h>
#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/cksys.h>

#define CKSYS_OFFSET	0x270
#define PERI_MASK_IDLE_TO_CKSYS	(PERICFG_AO_SEC_BASE + CKSYS_OFFSET)

void mtk_cksys_init(void)
{
	setbits32p(PERI_MASK_IDLE_TO_CKSYS, BIT(0));
	printk(BIOS_INFO, "%s = %#x\n", __func__, read32p(PERI_MASK_IDLE_TO_CKSYS));
}
