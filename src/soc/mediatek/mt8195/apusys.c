/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <console/console.h>
#include <device/mmio.h>
#include <soc/apusys.h>

/* MBOX Functional Configuration */
DEFINE_BITFIELD(LOCK, 0, 0)
DEFINE_BITFIELD(NO_MPU, 16, 16)

static void dump_apusys_reg(void)
{
	size_t i;

	for (i = 0; i < ARRAY_SIZE(mt8195_apu_mbox); i++) {
		printk(BIOS_DEBUG, "APU_MBOX %p = %#x\n",
		       (void *)&mt8195_apu_mbox[i]->mbox_func_cfg,
		       read32(&mt8195_apu_mbox[i]->mbox_func_cfg));
	}
}

void apusys_init(void)
{
	size_t i;

	/* Set up MBOX MPU for non secure access */
	for (i = 0; i < ARRAY_SIZE(mt8195_apu_mbox); i++)
		SET32_BITFIELDS(&mt8195_apu_mbox[i]->mbox_func_cfg, NO_MPU, 1, LOCK, 1);

	dump_apusys_reg();
}
