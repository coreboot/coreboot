/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <device/mmio.h>
#include <soc/dpm.h>
#include <soc/dramc_soc.h>
#include <soc/spm.h>
#include <soc/symbols.h>

static struct dpm_regs *const mtk_dpm2 = (void *)DPM_CFG_BASE2;

static int wake_dpm_sram_up(void)
{
	int loop = 100;

	/* TODO: convert to new APIs (SET32_BITFIELDS/READ32_BITFIELD) */
	setbits32(&mtk_spm->dramc_mcu_sram_con, DRAMC_MCU_SRAM_SLEEP_B_LSB);
	setbits32(&mtk_spm->dramc_mcu2_sram_con, DRAMC_MCU2_SRAM_SLEEP_B_LSB);

	while (loop > 0 &&
	       ((read32(&mtk_spm->dramc_mcu_sram_con) &
		 DRAMC_MCU_SRAM_SLEEP_B_LSB) == 0 ||
		(read32(&mtk_spm->dramc_mcu2_sram_con) &
		 DRAMC_MCU2_SRAM_SLEEP_B_LSB) == 0)) {
		mdelay(1);
		--loop;
	}

	if (loop == 0) {
		printk(BIOS_ERR, "failed to wake DPM up.\n");
		return -1;
	}

	setbits32(&mtk_spm->dramc_mcu_sram_con, DRAMC_MCU_SRAM_ISOINT_B_LSB);
	setbits32(&mtk_spm->dramc_mcu2_sram_con, DRAMC_MCU2_SRAM_ISOINT_B_LSB);

	return 0;
}

static void dpm_mtcoms_sleep_on(void)
{
	/* DPM MTCMOS sleep on */
	write32(&mtk_spm->dpm0_pwr_con, 0x0000204d);
	write32(&mtk_spm->dpm1_pwr_con, 0x0000204d);
	mdelay(1);
	write32(&mtk_spm->dpm0_pwr_con, 0x0000224d);
	write32(&mtk_spm->dpm1_pwr_con, 0x0000224d);
	mdelay(1);
	clrbits32(&mtk_dpm->sw_rstn, DPM_SW_RSTN_RESET);
	clrbits32(&mtk_dpm2->sw_rstn, DPM_SW_RSTN_RESET);
}

static struct mtk_mcu dpm_mcu_4ch[] = {
	{
		.firmware_name = CONFIG_DPM_DM_FIRMWARE,
		.run_address = (void *)DPM_DM_SRAM_BASE2,
	},
	{
		.firmware_name = CONFIG_DPM_PM_FIRMWARE,
		.run_address = (void *)DPM_PM_SRAM_BASE2,
		.priv = mtk_dpm2,
		.reset = dpm_reset,
	},
};

int dpm_4ch_init(void)
{
	dpm_mtcoms_sleep_on();
	if (wake_dpm_sram_up())
		return -1;
	return 0;
}

int dpm_4ch_para_setting(void)
{
	int i;
	struct mtk_mcu *dpm;

	for (i = 0; i < ARRAY_SIZE(dpm_mcu_4ch); i++) {
		dpm = &dpm_mcu_4ch[i];
		dpm->load_buffer = _dram_dma;
		dpm->buffer_size = REGION_SIZE(dram_dma);
		if (mtk_init_mcu(dpm))
			return -1;
	}

	return 0;
}
