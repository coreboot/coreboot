/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/mmio.h>
#include <soc/apusys.h>

/* MBOX Functional Configuration */
DEFINE_BITFIELD(LOCK, 0, 0)
DEFINE_BITFIELD(NO_MPU, 16, 16)

static void dump_apusys_reg(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(mt8195_apu_mbox); i++) {
		printk(BIOS_INFO, "APU_MBOX %p = %#x\n",
		       (void *)&mt8195_apu_mbox[i]->mbox_func_cfg,
		       read32(&mt8195_apu_mbox[i]->mbox_func_cfg));
	}
}

void apusys_init(void)
{
	int i;

	/* Setup MBOX MPU for non secure access */
	for (i = 0; i < ARRAY_SIZE(mt8195_apu_mbox); i++)
		SET32_BITFIELDS(&mt8195_apu_mbox[i]->mbox_func_cfg, NO_MPU, 1, LOCK, 1);

	dump_apusys_reg();
}
