/* DDR3 mem setup file for EXYNOS5 based board */
/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/mmio.h>
#include <delay.h>
#include <soc/clk.h>
#include <soc/dmc.h>
#include <soc/power.h>
#include <soc/setup.h>

#define TIMEOUT	10000

/* 'reset' field is currently ignored. */

int ddr3_mem_ctrl_init(struct mem_timings *mem, int interleave_size, int reset)
{
	u32 val, nLockR, nLockW_phy0, nLockW_phy1;
	int i, chip;

	/* Enable PAUSE for DREX */
	setbits32(&exynos_clock->pause, ENABLE_BIT);

	/* Enable BYPASS mode */
	setbits32(&exynos_clock->bpll_con1, BYPASS_EN);

	write32(&exynos_clock->clk_src_cdrex, MUX_BPLL_SEL_FOUTBPLL);
	do {
		val = read32(&exynos_clock->clk_mux_stat_cdrex);
		val &= BPLL_SEL_MASK;
	} while (val != FOUTBPLL);

	clrbits32(&exynos_clock->bpll_con1, BYPASS_EN);

	/* Specify the DDR memory type as DDR3 */
	val = read32(&exynos_phy0_control->phy_con0);
	val &= ~(PHY_CON0_CTRL_DDR_MODE_MASK << PHY_CON0_CTRL_DDR_MODE_SHIFT);
	val |= (mem->mem_type << PHY_CON0_CTRL_DDR_MODE_SHIFT);
	write32(&exynos_phy0_control->phy_con0, val);

	val = read32(&exynos_phy1_control->phy_con0);
	val &= ~(PHY_CON0_CTRL_DDR_MODE_MASK << PHY_CON0_CTRL_DDR_MODE_SHIFT);
	val |= (mem->mem_type << PHY_CON0_CTRL_DDR_MODE_SHIFT);
	write32(&exynos_phy1_control->phy_con0, val);

	/* Set Read Latency and Burst Length for PHY0 and PHY1 */
	val = (mem->ctrl_bstlen << PHY_CON42_CTRL_BSTLEN_SHIFT) |
		(mem->ctrl_rdlat << PHY_CON42_CTRL_RDLAT_SHIFT);
	write32(&exynos_phy0_control->phy_con42, val);
	write32(&exynos_phy1_control->phy_con42, val);

	val = read32(&exynos_phy0_control->phy_con26);
	val &= ~(T_WRDATA_EN_MASK << T_WRDATA_EN_OFFSET);
	val |= (T_WRDATA_EN_DDR3 << T_WRDATA_EN_OFFSET);
	write32(&exynos_phy0_control->phy_con26, val);

	val = read32(&exynos_phy1_control->phy_con26);
	val &= ~(T_WRDATA_EN_MASK << T_WRDATA_EN_OFFSET);
	val |= (T_WRDATA_EN_DDR3 << T_WRDATA_EN_OFFSET);
	write32(&exynos_phy1_control->phy_con26, val);

	/* Set Driver strength for CK, CKE, CS & CA to 0x7
	 * Set Driver strength for Data Slice 0~3 to 0x6
	 */
	val = (0x7 << CA_CK_DRVR_DS_OFFSET) | (0x7 << CA_CKE_DRVR_DS_OFFSET) |
		(0x7 << CA_CS_DRVR_DS_OFFSET) | (0x7 << CA_ADR_DRVR_DS_OFFSET);
	val |= (0x7 << DA_3_DS_OFFSET) | (0x7 << DA_2_DS_OFFSET) |
		(0x7 << DA_1_DS_OFFSET) | (0x7 << DA_0_DS_OFFSET);
	write32(&exynos_phy0_control->phy_con39, val);
	write32(&exynos_phy1_control->phy_con39, val);

	/* ZQ Calibration */
	if (dmc_config_zq(mem, exynos_phy0_control, exynos_phy1_control))
		return SETUP_ERR_ZQ_CALIBRATION_FAILURE;

	clrbits32(&exynos_phy0_control->phy_con16, ZQ_CLK_DIV_EN);
	clrbits32(&exynos_phy1_control->phy_con16, ZQ_CLK_DIV_EN);

	/* DQ Signal */
	val = read32(&exynos_phy0_control->phy_con14);
	val |= mem->phy0_pulld_dqs;
	write32(&exynos_phy0_control->phy_con14, val);
	val = read32(&exynos_phy1_control->phy_con14);
	val |= mem->phy1_pulld_dqs;
	write32(&exynos_phy1_control->phy_con14, val);

	val = MEM_TERM_EN | PHY_TERM_EN;
	write32(&exynos_drex0->phycontrol0, val);
	write32(&exynos_drex1->phycontrol0, val);

	write32(&exynos_drex0->concontrol, mem->concontrol |
		(mem->dfi_init_start << CONCONTROL_DFI_INIT_START_SHIFT) |
		(mem->rd_fetch << CONCONTROL_RD_FETCH_SHIFT));
	write32(&exynos_drex1->concontrol, mem->concontrol |
		(mem->dfi_init_start << CONCONTROL_DFI_INIT_START_SHIFT) |
		(mem->rd_fetch << CONCONTROL_RD_FETCH_SHIFT));

	do {
		val = read32(&exynos_drex0->phystatus);
	} while ((val & DFI_INIT_COMPLETE) != DFI_INIT_COMPLETE);
	do {
		val = read32(&exynos_drex1->phystatus);
	} while ((val & DFI_INIT_COMPLETE) != DFI_INIT_COMPLETE);

	clrbits32(&exynos_drex0->concontrol, DFI_INIT_START);
	clrbits32(&exynos_drex1->concontrol, DFI_INIT_START);

	update_reset_dll(exynos_drex0, mem->mem_type);
	update_reset_dll(exynos_drex1, mem->mem_type);

	/* MEMBASECONFIG0 (CS0) */
	write32(&exynos_tzasc0->membaseconfig0, mem->membaseconfig0);
	write32(&exynos_tzasc1->membaseconfig0, mem->membaseconfig0);

	/* MEMBASECONFIG1 (CS1) */
	if (mem->chips_per_channel == 2) {
		write32(&exynos_tzasc0->membaseconfig1, mem->membaseconfig1);
		write32(&exynos_tzasc1->membaseconfig1, mem->membaseconfig1);
	}

	/* Memory Channel Interleaving Size
	 * Exynos5420 Channel interleaving = 128 bytes
	 */
	/* MEMCONFIG0/1 */
	write32(&exynos_tzasc0->memconfig0, mem->memconfig);
	write32(&exynos_tzasc1->memconfig0, mem->memconfig);
	write32(&exynos_tzasc0->memconfig1, mem->memconfig);
	write32(&exynos_tzasc1->memconfig1, mem->memconfig);

	/* Precharge Configuration */
	write32(&exynos_drex0->prechconfig0,
		mem->prechconfig_tp_cnt << PRECHCONFIG_TP_CNT_SHIFT);
	write32(&exynos_drex1->prechconfig0,
		mem->prechconfig_tp_cnt << PRECHCONFIG_TP_CNT_SHIFT);

	/* TimingRow, TimingData, TimingPower and Timingref
	 * values as per Memory AC parameters
	 */
	write32(&exynos_drex0->timingref, mem->timing_ref);
	write32(&exynos_drex1->timingref, mem->timing_ref);
	write32(&exynos_drex0->timingrow, mem->timing_row);
	write32(&exynos_drex1->timingrow, mem->timing_row);
	write32(&exynos_drex0->timingdata, mem->timing_data);
	write32(&exynos_drex1->timingdata, mem->timing_data);
	write32(&exynos_drex0->timingpower, mem->timing_power);
	write32(&exynos_drex1->timingpower, mem->timing_power);

	if (reset) {
		/* Send NOP, MRS and ZQINIT commands.
		 * Sending MRS command will reset the DRAM. We should not be
		 * resetting the DRAM after resume, this will lead to memory
		 * corruption as DRAM content is lost after DRAM reset.
		 */
		dmc_config_mrs(mem, exynos_drex0);
		dmc_config_mrs(mem, exynos_drex1);
	} else {
		u32 ret;

		/*
		 * During Suspend-Resume & S/W-Reset, as soon as PMU releases
		 * pad retention, CKE goes high. This causes memory contents
		 * not to be retained during DRAM initialization. Therefore,
		 * there is a new control register(0x100431e8[28]) which lets us
		 * release pad retention and retain the memory content until the
		 * initialization is complete.
		 */
		write32(&exynos_power->padret_dram_cblk_opt,
			PAD_RETENTION_DRAM_COREBLK_VAL);
		do {
			ret = read32(&exynos_power->padret_dram_status);
		} while (ret != 0x1);

		/*
		 * CKE PAD retention disables DRAM self-refresh mode.
		 * Send auto refresh command for DRAM refresh.
		 */
		for (i = 0; i < 128; i++) {
			for (chip = 0; chip < mem->chips_to_configure; chip++) {
				write32(&exynos_drex0->directcmd,
					DIRECT_CMD_REFA |
					(chip << DIRECT_CMD_CHIP_SHIFT));
				write32(&exynos_drex1->directcmd,
					DIRECT_CMD_REFA |
					(chip << DIRECT_CMD_CHIP_SHIFT));
			}
		}
	}

	if (mem->gate_leveling_enable) {

		write32(&exynos_phy0_control->phy_con0, PHY_CON0_RESET_VAL);
		write32(&exynos_phy1_control->phy_con0, PHY_CON0_RESET_VAL);

		setbits32(&exynos_phy0_control->phy_con0, P0_CMD_EN);
		setbits32(&exynos_phy1_control->phy_con0, P0_CMD_EN);

		val = PHY_CON2_RESET_VAL;
		val |= INIT_DESKEW_EN;
		write32(&exynos_phy0_control->phy_con2, val);
		write32(&exynos_phy1_control->phy_con2, val);

		val =  read32(&exynos_phy0_control->phy_con1);
		val |= (RDLVL_PASS_ADJ_VAL << RDLVL_PASS_ADJ_OFFSET);
		write32(&exynos_phy0_control->phy_con1, val);

		val =  read32(&exynos_phy1_control->phy_con1);
		val |= (RDLVL_PASS_ADJ_VAL << RDLVL_PASS_ADJ_OFFSET);
		write32(&exynos_phy1_control->phy_con1, val);

		nLockR = read32(&exynos_phy0_control->phy_con13);
		nLockW_phy0 = (nLockR & CTRL_LOCK_COARSE_MASK) >> 2;
		nLockR = read32(&exynos_phy0_control->phy_con12);
		nLockR &= ~CTRL_DLL_ON;
		nLockR |= nLockW_phy0;
		write32(&exynos_phy0_control->phy_con12, nLockR);

		nLockR = read32(&exynos_phy1_control->phy_con13);
		nLockW_phy1 = (nLockR & CTRL_LOCK_COARSE_MASK) >> 2;
		nLockR = read32(&exynos_phy1_control->phy_con12);
		nLockR &= ~CTRL_DLL_ON;
		nLockR |= nLockW_phy1;
		write32(&exynos_phy1_control->phy_con12, nLockR);

		val = (0x3 << DIRECT_CMD_BANK_SHIFT) | 0x4;
		for (chip = 0; chip < mem->chips_to_configure; chip++) {
			write32(&exynos_drex0->directcmd,
				val | (chip << DIRECT_CMD_CHIP_SHIFT));
			write32(&exynos_drex1->directcmd,
				val | (chip << DIRECT_CMD_CHIP_SHIFT));
		}

		setbits32(&exynos_phy0_control->phy_con2, RDLVL_GATE_EN);
		setbits32(&exynos_phy1_control->phy_con2, RDLVL_GATE_EN);

		setbits32(&exynos_phy0_control->phy_con0, CTRL_SHGATE);
		setbits32(&exynos_phy1_control->phy_con0, CTRL_SHGATE);

		val = read32(&exynos_phy0_control->phy_con1);
		val &= ~(CTRL_GATEDURADJ_MASK);
		write32(&exynos_phy0_control->phy_con1, val);

		val = read32(&exynos_phy1_control->phy_con1);
		val &= ~(CTRL_GATEDURADJ_MASK);
		write32(&exynos_phy1_control->phy_con1, val);

		write32(&exynos_drex0->rdlvl_config, CTRL_RDLVL_GATE_ENABLE);
		i = TIMEOUT;
		while (((read32(&exynos_drex0->phystatus) & RDLVL_COMPLETE_CHO)
			!= RDLVL_COMPLETE_CHO) && (i > 0)) {
			/*
			 * TODO(waihong): Comment on how long this take to
			 * timeout
			 */
			udelay(1);
			i--;
		}
		if (!i)
			return SETUP_ERR_RDLV_COMPLETE_TIMEOUT;
		write32(&exynos_drex0->rdlvl_config, CTRL_RDLVL_GATE_DISABLE);

		write32(&exynos_drex1->rdlvl_config, CTRL_RDLVL_GATE_ENABLE);
		i = TIMEOUT;
		while (((read32(&exynos_drex1->phystatus) & RDLVL_COMPLETE_CHO)
			!= RDLVL_COMPLETE_CHO) && (i > 0)) {
			/*
			 * TODO(waihong): Comment on how long this take to
			 * timeout
			 */
			udelay(1);
			i--;
		}
		if (!i)
			return SETUP_ERR_RDLV_COMPLETE_TIMEOUT;
		write32(&exynos_drex1->rdlvl_config, CTRL_RDLVL_GATE_DISABLE);

		write32(&exynos_phy0_control->phy_con14, 0);
		write32(&exynos_phy1_control->phy_con14, 0);

		val = (0x3 << DIRECT_CMD_BANK_SHIFT);
		for (chip = 0; chip < mem->chips_to_configure; chip++) {
			write32(&exynos_drex0->directcmd,
				val | (chip << DIRECT_CMD_CHIP_SHIFT));
			write32(&exynos_drex1->directcmd,
				val | (chip << DIRECT_CMD_CHIP_SHIFT));
		}

		/* Common Settings for Leveling */
		val = PHY_CON12_RESET_VAL;
		write32(&exynos_phy0_control->phy_con12, (val + nLockW_phy0));
		write32(&exynos_phy1_control->phy_con12, (val + nLockW_phy1));

		setbits32(&exynos_phy0_control->phy_con2, DLL_DESKEW_EN);
		setbits32(&exynos_phy1_control->phy_con2, DLL_DESKEW_EN);
	}

	/* Send PALL command */
	dmc_config_prech(mem, exynos_drex0);
	dmc_config_prech(mem, exynos_drex1);

	write32(&exynos_drex0->memcontrol, mem->memcontrol);
	write32(&exynos_drex1->memcontrol, mem->memcontrol);

	/*
	 * Set DMC Concontrol: Enable auto-refresh counter, provide
	 * read data fetch cycles and enable DREX auto set powerdown
	 * for input buffer of I/O in none read memory state.
	 */
	write32(&exynos_drex0->concontrol, mem->concontrol |
		(mem->aref_en << CONCONTROL_AREF_EN_SHIFT) |
		(mem->rd_fetch << CONCONTROL_RD_FETCH_SHIFT) |
		DMC_CONCONTROL_IO_PD_CON(0x2));
	write32(&exynos_drex1->concontrol, mem->concontrol |
		(mem->aref_en << CONCONTROL_AREF_EN_SHIFT) |
		(mem->rd_fetch << CONCONTROL_RD_FETCH_SHIFT) |
		DMC_CONCONTROL_IO_PD_CON(0x2));

	/* Enable Clock Gating Control for DMC
	 * this saves around 25 mw dmc power as compared to the power
	 * consumption without these bits enabled
	 */
	setbits32(&exynos_drex0->cgcontrol, DMC_INTERNAL_CG);
	setbits32(&exynos_drex1->cgcontrol, DMC_INTERNAL_CG);

	return 0;
}
