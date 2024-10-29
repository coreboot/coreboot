/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <console/console.h>
#include <delay.h>
#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/mcu_common.h>
#include <soc/mcupm.h>
#include <soc/mcupm_plat.h>
#include <soc/spm.h>
#include <soc/symbols.h>

static void reset_mcupm(struct mtk_mcu *mcu)
{
	/* Clear abnormal boot register */
	write32p(ABNORMALBOOT_REG, ABNORMALBOOT_REG_STATUS);
	write32p(WARMBOOT_REG, WARMBOOT_REG_STATUS);
	write32(&mcupm_reg->sw_rstn, MCUPM_RSTN_RESET);
}

static struct mtk_mcu mcupm = {
	.firmware_name = CONFIG_MCUPM_FIRMWARE,
	.run_address = (void *)MCUPM_SRAM_BASE,
	.reset = reset_mcupm,
};

void mcupm_init(void)
{
	mcupm.load_buffer = _dram_dma;
	mcupm.buffer_size = REGION_SIZE(dram_dma);

	/* Set CPUEB as secure master */
	setbits32(&mtk_spm->poweron_config_set, BIT(15));

	/* Unlock SPM POWERON_CONFIG_EN */
	setbits32(&mtk_spm->poweron_config_set, SPM_REGWR_CFG_KEY | BCLK_CG_EN_LSB);

	/* Reinit CPUEB_PWR_CON */
	/* (1) CPUEB MTCMOS */
	setbits32(&mtk_spm->cpueb_pwr_con, CPUEB_PWR_ON_LSB);
	setbits32(&mtk_spm->cpueb_pwr_con, CPUEB_PWR_ON_2ND_LSB);
	u32 pwr_con_mask = SC_CPUEB_PWR_ACK_LSB | SC_CPUEB_PWR_ACK_2ND_LSB;
	if (!retry(POLLING_ACK_RETRY_COUNTS,
		   ((read32(&mtk_spm->cpueb_pwr_con) & pwr_con_mask) == pwr_con_mask),
		   udelay(1))) {
		printk(BIOS_ERR, "%s Polling ACK timeout, %#x\n", __func__,
		       read32(&mtk_spm->cpueb_pwr_con));
		return;
	}

	clrbits32(&mtk_spm->cpueb_pwr_con, CPUEB_PWR_CLK_DIS_LSB);
	clrbits32(&mtk_spm->cpueb_pwr_con, CPUEB_PWR_ISO_LSB);
	setbits32(&mtk_spm->cpueb_pwr_con, CPUEB_PWR_RST_B_LSB);

	/* (2) SRAM leave DORMANT mode */
	setbits32(&mtk_spm->cpueb_pwr_con, CPUEB_SRAM_SLEEP_B_LSB);
	if (!retry(POLLING_ACK_RETRY_COUNTS,
		   ((read32(&mtk_spm->cpueb_pwr_con) & SC_CPUEB_SRAM_SLEEP_B_ACK_LSB) != 0),
		   udelay(1))) {
		printk(BIOS_ERR, "%s Polling SRAM Sleep B ACK timeout, %#x\n", __func__,
		       read32(&mtk_spm->cpueb_pwr_con));
		return;
	}

	udelay(1);
	setbits32(&mtk_spm->cpueb_pwr_con, CPUEB_SRAM_ISOINT_B_LSB);
	clrbits32(&mtk_spm->cpueb_pwr_con, CPUEB_SRAM_CKISO_LSB);

	/* Power on MCUPM sram */
	clrbits32(&mtk_spm->cpueb_pwr_con, CPUEB_SRAM_PDN_LSB);
	pwr_con_mask = SC_CPUEB_SRAM_PDN_ACK_LSB;
	if (!retry(POLLING_SRAM_RETRY_COUNTS,
		   (read32(&mtk_spm->cpueb_pwr_con) & pwr_con_mask) != pwr_con_mask,
		   udelay(1))) {
		printk(BIOS_ERR, "%s Polling SRAM Power on timeout, %#x\n", __func__,
		       read32(&mtk_spm->cpueb_pwr_con));
		return;
	}

	printk(BIOS_INFO, "%s Power on MCUPM SRAM success\n", __func__);

	write32(&mcupm_reg->sw_rstn, MCUPM_RSTN_RSTN_INIT);
	if (mtk_init_mcu(&mcupm))
		die("%s() mtk_init_mcu failed\n", __func__);

	setbits32p(MCUPM_SW_RSTN,
		   MCUPM_CFGREG_SW_RSTN_SW_RSTN | MCUPM_CFGREG_SW_RSTN_DMA_BUSY_MASK);

	printk(BIOS_DEBUG, "mcupm: MCUPM_SRAM_GPR0: %#x, %#x\n",
	       MCUPM_INFO, read32p(MCUPM_INFO));
	printk(BIOS_DEBUG, "mcupm: MCUPM_SRAM_GPR1: %#x, %#x\n",
	       ABNORMALBOOT_REG, read32p(ABNORMALBOOT_REG));
	printk(BIOS_DEBUG, "mcupm: MCUPM_SRAM_GPR23: %#x, %#x\n",
	       WARMBOOT_REG, read32p(WARMBOOT_REG));
	printk(BIOS_DEBUG, "mcupm: MCUPM_SW_RSTN: %#x, %#x\n",
	       MCUPM_SW_RSTN, read32p(MCUPM_SW_RSTN));
	printk(BIOS_DEBUG, "mcupm: MCUPM_SRAM_BASE: %#x, %#x\n",
	       MCUPM_SRAM_BASE, read32p(MCUPM_SRAM_BASE));
	printk(BIOS_DEBUG, "%s MCUPM part. load & reset finished\n", __func__);
}
