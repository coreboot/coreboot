/* SPDX-License-Identifier: GPL-2.0-only */

/* DDR3 mem setup file for SMDK5250 board based on EXYNOS5 */

#include <device/mmio.h>
#include <console/console.h>
#include <delay.h>
#include <soc/clk.h>
#include <soc/cpu.h>
#include <soc/dmc.h>
#include <soc/setup.h>

#define RDLVL_COMPLETE_TIMEOUT	10000

static void reset_phy_ctrl(void)
{
	write32(&exynos_clock->lpddr3phy_ctrl,
		LPDDR3PHY_CTRL_PHY_RESET_ENABLE);
	write32(&exynos_clock->lpddr3phy_ctrl,
		LPDDR3PHY_CTRL_PHY_RESET_DISABLE);

	udelay(500);
}

int ddr3_mem_ctrl_init(struct mem_timings *mem, unsigned long mem_iv_size,
		       int mem_reset)
{
	unsigned int val;
	int i;

	if (mem_reset)
		reset_phy_ctrl();

	/* Set Impedance Output Driver */
	val = (mem->impedance << CA_CK_DRVR_DS_OFFSET) |
		(mem->impedance << CA_CKE_DRVR_DS_OFFSET) |
		(mem->impedance << CA_CS_DRVR_DS_OFFSET) |
		(mem->impedance << CA_ADR_DRVR_DS_OFFSET);
	write32(&exynos_phy0_control->phy_con39, val);
	write32(&exynos_phy1_control->phy_con39, val);

	/* Set Read Latency and Burst Length for PHY0 and PHY1 */
	val = (mem->ctrl_bstlen << PHY_CON42_CTRL_BSTLEN_SHIFT) |
		(mem->ctrl_rdlat << PHY_CON42_CTRL_RDLAT_SHIFT);
	write32(&exynos_phy0_control->phy_con42, val);
	write32(&exynos_phy1_control->phy_con42, val);

	/* ZQ Calibration */
	if (dmc_config_zq(mem, exynos_phy0_control, exynos_phy1_control)){
		printk(BIOS_EMERG, "DRAM ZQ CALIBRATION FAILURE\n");
		return SETUP_ERR_ZQ_CALIBRATION_FAILURE;
	}

	/* DQ Signal */
	write32(&exynos_phy0_control->phy_con14, mem->phy0_pulld_dqs);
	write32(&exynos_phy1_control->phy_con14, mem->phy1_pulld_dqs);

	write32(&exynos_dmc->concontrol, mem->concontrol |
		(mem->rd_fetch << CONCONTROL_RD_FETCH_SHIFT) |
		(mem->dfi_init_start << CONCONTROL_DFI_INIT_START_SHIFT));

	update_reset_dll(exynos_dmc, DDR_MODE_DDR3);

	/* DQS Signal */
	write32(&exynos_phy0_control->phy_con4, mem->phy0_dqs);
	write32(&exynos_phy1_control->phy_con4, mem->phy1_dqs);

	write32(&exynos_phy0_control->phy_con6, mem->phy0_dq);
	write32(&exynos_phy1_control->phy_con6, mem->phy1_dq);

	write32(&exynos_phy0_control->phy_con10, mem->phy0_tFS);
	write32(&exynos_phy1_control->phy_con10, mem->phy1_tFS);

	val = (mem->ctrl_start_point << PHY_CON12_CTRL_START_POINT_SHIFT) |
		(mem->ctrl_inc << PHY_CON12_CTRL_INC_SHIFT) |
		(mem->ctrl_dll_on << PHY_CON12_CTRL_DLL_ON_SHIFT) |
		(mem->ctrl_ref << PHY_CON12_CTRL_REF_SHIFT);
	write32(&exynos_phy0_control->phy_con12, val);
	write32(&exynos_phy1_control->phy_con12, val);

	/* Start DLL locking */
	write32(&exynos_phy0_control->phy_con12,
		val | (mem->ctrl_start << PHY_CON12_CTRL_START_SHIFT));
	write32(&exynos_phy1_control->phy_con12,
		val | (mem->ctrl_start << PHY_CON12_CTRL_START_SHIFT));

	update_reset_dll(exynos_dmc, DDR_MODE_DDR3);

	write32(&exynos_dmc->concontrol,
		mem->concontrol | (mem->rd_fetch << CONCONTROL_RD_FETCH_SHIFT));

	/* Memory Channel Inteleaving Size */
	write32(&exynos_dmc->ivcontrol, mem->iv_size);

	/* Set DMC MEMCONTROL register */
	val = mem->memcontrol & ~DMC_MEMCONTROL_DSREF_ENABLE;
	write32(&exynos_dmc->memcontrol, val);

	write32(&exynos_dmc->memconfig0, mem->memconfig);
	write32(&exynos_dmc->memconfig1, mem->memconfig);
	write32(&exynos_dmc->membaseconfig0, mem->membaseconfig0);
	write32(&exynos_dmc->membaseconfig1, mem->membaseconfig1);

	/* Precharge Configuration */
	write32(&exynos_dmc->prechconfig,
		mem->prechconfig_tp_cnt << PRECHCONFIG_TP_CNT_SHIFT);

	/* Power Down mode Configuration */
	write32(&exynos_dmc->pwrdnconfig,
		mem->dpwrdn_cyc << PWRDNCONFIG_DPWRDN_CYC_SHIFT |
		mem->dsref_cyc << PWRDNCONFIG_DSREF_CYC_SHIFT);

	/* TimingRow, TimingData, TimingPower and Timingaref
	 * values as per Memory AC parameters
	 */
	write32(&exynos_dmc->timingref, mem->timing_ref);
	write32(&exynos_dmc->timingrow, mem->timing_row);
	write32(&exynos_dmc->timingdata, mem->timing_data);
	write32(&exynos_dmc->timingpower, mem->timing_power);

	/* Send PALL command */
	dmc_config_prech(mem, exynos_dmc);

	if (mem_reset) {
		/* Send NOP, MRS and ZQINIT commands.
		 * Sending MRS command will reset the DRAM. We should not be
		 * resetting the DRAM after resume, this will lead to memory
		 * corruption as DRAM content is lost after DRAM reset
		 */
		dmc_config_mrs(mem, exynos_dmc);
	}

	if (mem->gate_leveling_enable) {
		val = PHY_CON0_RESET_VAL;
		val |= P0_CMD_EN;
		write32(&exynos_phy0_control->phy_con0, val);
		write32(&exynos_phy1_control->phy_con0, val);

		val = PHY_CON2_RESET_VAL;
		val |= INIT_DESKEW_EN;
		write32(&exynos_phy0_control->phy_con2, val);
		write32(&exynos_phy1_control->phy_con2, val);

		val = PHY_CON0_RESET_VAL;
		val |= P0_CMD_EN;
		val |= BYTE_RDLVL_EN;
		write32(&exynos_phy0_control->phy_con0, val);
		write32(&exynos_phy1_control->phy_con0, val);

		val = (mem->ctrl_start_point <<
				PHY_CON12_CTRL_START_POINT_SHIFT) |
			(mem->ctrl_inc << PHY_CON12_CTRL_INC_SHIFT) |
			(mem->ctrl_force << PHY_CON12_CTRL_FORCE_SHIFT) |
			(mem->ctrl_start << PHY_CON12_CTRL_START_SHIFT) |
			(mem->ctrl_ref << PHY_CON12_CTRL_REF_SHIFT);
		write32(&exynos_phy0_control->phy_con12, val);
		write32(&exynos_phy1_control->phy_con12, val);

		val = PHY_CON2_RESET_VAL;
		val |= INIT_DESKEW_EN;
		val |= RDLVL_GATE_EN;
		write32(&exynos_phy0_control->phy_con2, val);
		write32(&exynos_phy1_control->phy_con2, val);

		val = PHY_CON0_RESET_VAL;
		val |= P0_CMD_EN;
		val |= BYTE_RDLVL_EN;
		val |= CTRL_SHGATE;
		write32(&exynos_phy0_control->phy_con0, val);
		write32(&exynos_phy1_control->phy_con0, val);

		val = PHY_CON1_RESET_VAL;
		val &= ~(CTRL_GATEDURADJ_MASK);
		write32(&exynos_phy0_control->phy_con1, val);
		write32(&exynos_phy1_control->phy_con1, val);

		write32(&exynos_dmc->rdlvl_config, CTRL_RDLVL_GATE_ENABLE);
		i = RDLVL_COMPLETE_TIMEOUT;
		while ((read32(&exynos_dmc->phystatus) &
			(RDLVL_COMPLETE_CHO | RDLVL_COMPLETE_CH1)) !=
			(RDLVL_COMPLETE_CHO | RDLVL_COMPLETE_CH1) && i > 0) {
			/*
			 * TODO(waihong): Comment on how long this take to
			 * timeout
			 */
			udelay(1);
			i--;
		}
		if (!i){
			printk(BIOS_EMERG, "Timeout on RDLVL. No DRAM.\n");
			return SETUP_ERR_RDLV_COMPLETE_TIMEOUT;
		}
		write32(&exynos_dmc->rdlvl_config, CTRL_RDLVL_GATE_DISABLE);

		write32(&exynos_phy0_control->phy_con14, 0);
		write32(&exynos_phy1_control->phy_con14, 0);

		val = (mem->ctrl_start_point <<
				PHY_CON12_CTRL_START_POINT_SHIFT) |
			(mem->ctrl_inc << PHY_CON12_CTRL_INC_SHIFT) |
			(mem->ctrl_force << PHY_CON12_CTRL_FORCE_SHIFT) |
			(mem->ctrl_start << PHY_CON12_CTRL_START_SHIFT) |
			(mem->ctrl_dll_on << PHY_CON12_CTRL_DLL_ON_SHIFT) |
			(mem->ctrl_ref << PHY_CON12_CTRL_REF_SHIFT);
		write32(&exynos_phy0_control->phy_con12, val);
		write32(&exynos_phy1_control->phy_con12, val);

		update_reset_dll(exynos_dmc, DDR_MODE_DDR3);
	}

	/* Send PALL command */
	dmc_config_prech(mem, exynos_dmc);

	write32(&exynos_dmc->memcontrol, mem->memcontrol);

	/* Set DMC Concontrol and enable auto-refresh counter */
	write32(&exynos_dmc->concontrol, mem->concontrol |
		(mem->rd_fetch << CONCONTROL_RD_FETCH_SHIFT) |
		(mem->aref_en << CONCONTROL_AREF_EN_SHIFT));
	return 0;
}
