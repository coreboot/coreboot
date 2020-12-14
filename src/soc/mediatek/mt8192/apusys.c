/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/mmio.h>
#include <soc/apusys.h>
#include <soc/infracfg.h>

/* INFRA2APU_SRAM_PROT_EN */
DEFINE_BITFIELD(PROT_EN, 31, 30)

/* MBOX Functional Configuration */
DEFINE_BITFIELD(LOCK, 0, 0)
DEFINE_BITFIELD(NO_MPU, 16, 16)

static void dump_apusys_reg(void)
{
	int i;

	printk(BIOS_INFO, "INFRA2APU_SRAM_PROT_EN %p = %#x\n",
	       (void *)&mt8192_infracfg->infra_ao_mm_hang_free,
	       read32(&mt8192_infracfg->infra_ao_mm_hang_free));

	for (i = 0; i < ARRAY_SIZE(mt8192_apu_mbox); i++) {
		printk(BIOS_INFO, "APU_MBOX %p = %#x\n",
		       (void *)&mt8192_apu_mbox[i]->mbox_func_cfg,
		       read32(&mt8192_apu_mbox[i]->mbox_func_cfg));
	}
}

void apusys_init(void)
{
	int i;

	SET32_BITFIELDS(&mt8192_infracfg->infra_ao_mm_hang_free, PROT_EN, 0);

	/* Setup MBOX MPU for non secure access */
	for (i = 0; i < ARRAY_SIZE(mt8192_apu_mbox); i++)
		SET32_BITFIELDS(&mt8192_apu_mbox[i]->mbox_func_cfg, NO_MPU, 1, LOCK, 1);

	dump_apusys_reg();
}
