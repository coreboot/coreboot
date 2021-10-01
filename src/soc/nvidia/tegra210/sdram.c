/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <console/console.h>
#include <delay.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/emc.h>
#include <soc/mc.h>
#include <soc/pmc.h>
#include <soc/sdram.h>
#include <soc/nvidia/tegra/apbmisc.h>

static void sdram_patch(uintptr_t addr, uint32_t value)
{
	if (addr)
		write32((uint32_t *)addr, value);
}

static void writebits(uint32_t value, uint32_t *addr, uint32_t mask)
{
	clrsetbits32(addr, mask, (value & mask));
}

static void sdram_trigger_emc_timing_update(struct tegra_emc_regs *regs)
{
	write32(&regs->timing_control, EMC_TIMING_CONTROL_TIMING_UPDATE);
}

/* PMC must be configured before clock-enable and de-reset of MC/EMC. */
static void sdram_configure_pmc(const struct sdram_params *param,
				struct tegra_pmc_regs *regs)
{
	/* VDDP Select */
	write32(&regs->vddp_sel, param->PmcVddpSel);
	udelay(param->PmcVddpSelWait);

	/* Set DDR pad voltage */
	writebits(param->PmcDdrPwr, &regs->ddr_pwr, PMC_DDR_PWR_VAL_MASK);

	/* Turn on MEM IO Power */
	writebits(param->PmcNoIoPower, &regs->no_iopower,
		  (PMC_NO_IOPOWER_MEM_MASK | PMC_NO_IOPOWER_MEM_COMP_MASK));

	write32(&regs->reg_short, param->PmcRegShort);
	write32(&regs->ddr_cntrl, param->PmcDdrCntrl);
}

static void sdram_set_ddr_control(const struct sdram_params *param,
				  struct tegra_pmc_regs *regs)
{
	u32 ddrcntrl = read32(&regs->ddr_cntrl);

	/* Deassert HOLD_CKE_LOW */
	ddrcntrl &= ~PMC_CMD_HOLD_LOW_BR00_11_MASK;
	write32(&regs->ddr_cntrl, ddrcntrl);
	udelay(param->PmcDdrCntrlWait);
}

static void sdram_start_clocks(const struct sdram_params *param,
			       struct tegra_emc_regs *regs)
{
	struct clk_rst_ctlr *clk_rst = (void *)TEGRA_CLK_RST_BASE;

	u32 is_same_freq = (param->McEmemArbMisc0 &
			    MC_EMEM_ARB_MISC0_MC_EMC_SAME_FREQ_MASK) ? 1 : 0;
	u32 clk_source_emc = param->EmcClockSource;

	/* Enable the clocks for EMC and MC */
	setbits32(&clk_rst->clk_enb_h_set, (1 << 25));	// ENB_EMC
	setbits32(&clk_rst->clk_enb_h_set, (1 << 0));	// ENB_MC

	if ((clk_source_emc >> EMC_2X_CLK_SRC_SHIFT) != PLLM_UD)
		setbits32(&clk_rst->clk_enb_x_set, CLK_ENB_EMC_DLL);

	/* Remove the EMC and MC controllers from reset */
	clrbits32(&clk_rst->rst_dev_h, (1 << 25));		// SWR_EMC
	clrbits32(&clk_rst->rst_dev_h, (1 << 0));		// SWR_MC

	clk_source_emc |= (is_same_freq << 16);

	write32(&clk_rst->clk_src_emc, clk_source_emc);
	write32(&clk_rst->clk_src_emc_dll, param->EmcClockSourceDll);

	clock_sdram(param->PllMInputDivider, param->PllMFeedbackDivider,
		    param->PllMPostDivider, param->PllMSetupControl,
		    param->PllMKVCO, param->PllMKCP, param->PllMStableTime,
		    param->EmcClockSource, is_same_freq);

	if (param->ClkRstControllerPllmMisc2OverrideEnable)
		write32(&clk_rst->pllm_misc2,
			param->ClkRstControllerPllmMisc2Override);

	/* Wait for enough time for clk switch to take place */
	udelay(5);

	write32(&clk_rst->clk_enb_w_clr, param->ClearClk2Mc1);
}

static void sdram_set_swizzle(const struct sdram_params *param,
			      struct tegra_emc_regs *regs)
{
	write32(&regs->swizzle_rank0_byte0, param->EmcSwizzleRank0Byte0);
	write32(&regs->swizzle_rank0_byte1, param->EmcSwizzleRank0Byte1);
	write32(&regs->swizzle_rank0_byte2, param->EmcSwizzleRank0Byte2);
	write32(&regs->swizzle_rank0_byte3, param->EmcSwizzleRank0Byte3);

	write32(&regs->swizzle_rank1_byte0, param->EmcSwizzleRank1Byte0);
	write32(&regs->swizzle_rank1_byte1, param->EmcSwizzleRank1Byte1);
	write32(&regs->swizzle_rank1_byte2, param->EmcSwizzleRank1Byte2);
	write32(&regs->swizzle_rank1_byte3, param->EmcSwizzleRank1Byte3);
}

static void sdram_set_pad_controls(const struct sdram_params *param,
				   struct tegra_emc_regs *regs)
{
	/* Program the pad controls */
	write32(&regs->xm2comppadctrl, param->EmcXm2CompPadCtrl);
	write32(&regs->xm2comppadctrl2, param->EmcXm2CompPadCtrl2);
	write32(&regs->xm2comppadctrl3, param->EmcXm2CompPadCtrl3);
}

static void sdram_set_pad_macros(const struct sdram_params *param,
				 struct tegra_emc_regs *regs)
{
	u32 rfu_reset, rfu_mask1, rfu_mask2, rfu_step1, rfu_step2;
	u32 cpm_reset_settings, cpm_mask1, cpm_step1;

	write32(&regs->pmacro_vttgen_ctrl0, param->EmcPmacroVttgenCtrl0);
	write32(&regs->pmacro_vttgen_ctrl1, param->EmcPmacroVttgenCtrl1);
	write32(&regs->pmacro_vttgen_ctrl2, param->EmcPmacroVttgenCtrl2);
	/* Trigger timing update so above writes take place */
	sdram_trigger_emc_timing_update(regs);
	/* Add a wait to ensure the regulators settle */
	udelay(10);

	write32(&regs->dbg,
		param->EmcDbg | (param->EmcDbgWriteMux & WRITE_MUX_ACTIVE));

	rfu_reset = EMC_PMACRO_BRICK_CTRL_RFU1_RESET_VAL;
	rfu_mask1 = 0x01120112;
	rfu_mask2 = 0x01BF01BF;

	rfu_step1 = rfu_reset & (param->EmcPmacroBrickCtrlRfu1 | ~rfu_mask1);
	rfu_step2 = rfu_reset & (param->EmcPmacroBrickCtrlRfu1 | ~rfu_mask2);

	/* common pad macro (cpm) */
	cpm_reset_settings = 0x0000000F;
	cpm_mask1 = 0x00000001;
	cpm_step1 = cpm_reset_settings;
	cpm_step1 &= (param->EmcPmacroCommonPadTxCtrl | ~cpm_mask1);

	/* Patch 2 using BCT spare variables */
	sdram_patch(param->EmcBctSpare2, param->EmcBctSpare3);

	/*
	 * Program CMD mapping. Required before brick mapping, else
	 * we can't guarantee CK will be differential at all times.
	 */
	write32(&regs->fbio_cfg7, param->EmcFbioCfg7);

	write32(&regs->cmd_mapping_cmd0_0, param->EmcCmdMappingCmd0_0);
	write32(&regs->cmd_mapping_cmd0_1, param->EmcCmdMappingCmd0_1);
	write32(&regs->cmd_mapping_cmd0_2, param->EmcCmdMappingCmd0_2);
	write32(&regs->cmd_mapping_cmd1_0, param->EmcCmdMappingCmd1_0);
	write32(&regs->cmd_mapping_cmd1_1, param->EmcCmdMappingCmd1_1);
	write32(&regs->cmd_mapping_cmd1_2, param->EmcCmdMappingCmd1_2);
	write32(&regs->cmd_mapping_cmd2_0, param->EmcCmdMappingCmd2_0);
	write32(&regs->cmd_mapping_cmd2_1, param->EmcCmdMappingCmd2_1);
	write32(&regs->cmd_mapping_cmd2_2, param->EmcCmdMappingCmd2_2);
	write32(&regs->cmd_mapping_cmd3_0, param->EmcCmdMappingCmd3_0);
	write32(&regs->cmd_mapping_cmd3_1, param->EmcCmdMappingCmd3_1);
	write32(&regs->cmd_mapping_cmd3_2, param->EmcCmdMappingCmd3_2);
	write32(&regs->cmd_mapping_byte, param->EmcCmdMappingByte);

	/* Program brick mapping. */
	write32(&regs->pmacro_brick_mapping0, param->EmcPmacroBrickMapping0);
	write32(&regs->pmacro_brick_mapping1, param->EmcPmacroBrickMapping1);
	write32(&regs->pmacro_brick_mapping2, param->EmcPmacroBrickMapping2);

	write32(&regs->pmacro_brick_ctrl_rfu1, rfu_step1);

	/* This is required to do any reads from the pad macros */
	write32(&regs->config_sample_delay, param->EmcConfigSampleDelay);

	write32(&regs->fbio_cfg8, param->EmcFbioCfg8);

	sdram_set_swizzle(param, regs);

	/* Patch 4 using BCT spare variables */
	sdram_patch(param->EmcBctSpare6, param->EmcBctSpare7);

	sdram_set_pad_controls(param, regs);

	/* Program Autocal controls with shadowed register fields */
	write32(&regs->auto_cal_config2, param->EmcAutoCalConfig2);
	write32(&regs->auto_cal_config3, param->EmcAutoCalConfig3);
	write32(&regs->auto_cal_config4, param->EmcAutoCalConfig4);
	write32(&regs->auto_cal_config5, param->EmcAutoCalConfig5);
	write32(&regs->auto_cal_config6, param->EmcAutoCalConfig6);
	write32(&regs->auto_cal_config7, param->EmcAutoCalConfig7);
	write32(&regs->auto_cal_config8, param->EmcAutoCalConfig8);

	write32(&regs->pmacro_rx_term, param->EmcPmacroRxTerm);
	write32(&regs->pmacro_dq_tx_drv, param->EmcPmacroDqTxDrv);
	write32(&regs->pmacro_ca_tx_drv, param->EmcPmacroCaTxDrv);
	write32(&regs->pmacro_cmd_tx_drv, param->EmcPmacroCmdTxDrv);
	write32(&regs->pmacro_autocal_cfg_common,
		param->EmcPmacroAutocalCfgCommon);
	write32(&regs->auto_cal_channel, param->EmcAutoCalChannel);
	write32(&regs->pmacro_zctrl, param->EmcPmacroZctrl);

	write32(&regs->dll_cfg0, param->EmcDllCfg0);
	write32(&regs->dll_cfg1, param->EmcDllCfg1);
	write32(&regs->cfg_dig_dll_1, param->EmcCfgDigDll_1);

	write32(&regs->data_brlshft_0, param->EmcDataBrlshft0);
	write32(&regs->data_brlshft_1, param->EmcDataBrlshft1);
	write32(&regs->dqs_brlshft_0, param->EmcDqsBrlshft0);
	write32(&regs->dqs_brlshft_1, param->EmcDqsBrlshft1);
	write32(&regs->cmd_brlshft_0, param->EmcCmdBrlshft0);
	write32(&regs->cmd_brlshft_1, param->EmcCmdBrlshft1);
	write32(&regs->cmd_brlshft_2, param->EmcCmdBrlshft2);
	write32(&regs->cmd_brlshft_3, param->EmcCmdBrlshft3);
	write32(&regs->quse_brlshft_0, param->EmcQuseBrlshft0);
	write32(&regs->quse_brlshft_1, param->EmcQuseBrlshft1);
	write32(&regs->quse_brlshft_2, param->EmcQuseBrlshft2);
	write32(&regs->quse_brlshft_3, param->EmcQuseBrlshft3);

	write32(&regs->pmacro_brick_ctrl_rfu1, rfu_step2);
	write32(&regs->pmacro_pad_cfg_ctrl, param->EmcPmacroPadCfgCtrl);

	write32(&regs->pmacro_pad_cfg_ctrl, param->EmcPmacroPadCfgCtrl);
	write32(&regs->pmacro_cmd_brick_ctrl_fdpd,
		param->EmcPmacroCmdBrickCtrlFdpd);
	write32(&regs->pmacro_brick_ctrl_rfu2,
		param->EmcPmacroBrickCtrlRfu2 & 0xFF7FFF7F);
	write32(&regs->pmacro_data_brick_ctrl_fdpd,
		param->EmcPmacroDataBrickCtrlFdpd);
	write32(&regs->pmacro_bg_bias_ctrl_0, param->EmcPmacroBgBiasCtrl0);
	write32(&regs->pmacro_data_pad_rx_ctrl, param->EmcPmacroDataPadRxCtrl);
	write32(&regs->pmacro_cmd_pad_rx_ctrl, param->EmcPmacroCmdPadRxCtrl);
	write32(&regs->pmacro_data_pad_tx_ctrl, param->EmcPmacroDataPadTxCtrl);
	write32(&regs->pmacro_data_rx_term_mode,
		param->EmcPmacroDataRxTermMode);
	write32(&regs->pmacro_cmd_rx_term_mode, param->EmcPmacroCmdRxTermMode);
	write32(&regs->pmacro_cmd_pad_tx_ctrl, param->EmcPmacroCmdPadTxCtrl);

	write32(&regs->cfg_3, param->EmcCfg3);
	write32(&regs->pmacro_tx_pwrd_0, param->EmcPmacroTxPwrd0);
	write32(&regs->pmacro_tx_pwrd_1, param->EmcPmacroTxPwrd1);
	write32(&regs->pmacro_tx_pwrd_2, param->EmcPmacroTxPwrd2);
	write32(&regs->pmacro_tx_pwrd_3, param->EmcPmacroTxPwrd3);
	write32(&regs->pmacro_tx_pwrd_4, param->EmcPmacroTxPwrd4);
	write32(&regs->pmacro_tx_pwrd_5, param->EmcPmacroTxPwrd5);
	write32(&regs->pmacro_tx_sel_clk_src_0, param->EmcPmacroTxSelClkSrc0);
	write32(&regs->pmacro_tx_sel_clk_src_1, param->EmcPmacroTxSelClkSrc1);
	write32(&regs->pmacro_tx_sel_clk_src_2, param->EmcPmacroTxSelClkSrc2);
	write32(&regs->pmacro_tx_sel_clk_src_3, param->EmcPmacroTxSelClkSrc3);
	write32(&regs->pmacro_tx_sel_clk_src_4, param->EmcPmacroTxSelClkSrc4);
	write32(&regs->pmacro_tx_sel_clk_src_5, param->EmcPmacroTxSelClkSrc5);
	write32(&regs->pmacro_ddll_bypass, param->EmcPmacroDdllBypass);
	write32(&regs->pmacro_ddll_pwrd_0, param->EmcPmacroDdllPwrd0);
	write32(&regs->pmacro_ddll_pwrd_1, param->EmcPmacroDdllPwrd1);
	write32(&regs->pmacro_ddll_pwrd_2, param->EmcPmacroDdllPwrd2);
	write32(&regs->pmacro_cmd_ctrl_0, param->EmcPmacroCmdCtrl0);
	write32(&regs->pmacro_cmd_ctrl_1, param->EmcPmacroCmdCtrl1);
	write32(&regs->pmacro_cmd_ctrl_2, param->EmcPmacroCmdCtrl2);
	write32(&regs->pmacro_ib_vref_dq_0, param->EmcPmacroIbVrefDq_0);
	write32(&regs->pmacro_ib_vref_dq_1, param->EmcPmacroIbVrefDq_1);
	write32(&regs->pmacro_ib_vref_dqs_0, param->EmcPmacroIbVrefDqs_0);
	write32(&regs->pmacro_ib_vref_dqs_1, param->EmcPmacroIbVrefDqs_1);
	write32(&regs->pmacro_ib_rxrt, param->EmcPmacroIbRxrt);
	write32(&regs->pmacro_quse_ddll_rank0_0,
		param->EmcPmacroQuseDdllRank0_0);
	write32(&regs->pmacro_quse_ddll_rank0_1,
		param->EmcPmacroQuseDdllRank0_1);
	write32(&regs->pmacro_quse_ddll_rank0_2,
		param->EmcPmacroQuseDdllRank0_2);
	write32(&regs->pmacro_quse_ddll_rank0_3,
		param->EmcPmacroQuseDdllRank0_3);
	write32(&regs->pmacro_quse_ddll_rank0_4,
		param->EmcPmacroQuseDdllRank0_4);
	write32(&regs->pmacro_quse_ddll_rank0_5,
		param->EmcPmacroQuseDdllRank0_5);
	write32(&regs->pmacro_quse_ddll_rank1_0,
		param->EmcPmacroQuseDdllRank1_0);
	write32(&regs->pmacro_quse_ddll_rank1_1,
		param->EmcPmacroQuseDdllRank1_1);
	write32(&regs->pmacro_quse_ddll_rank1_2,
		param->EmcPmacroQuseDdllRank1_2);
	write32(&regs->pmacro_quse_ddll_rank1_3,
		param->EmcPmacroQuseDdllRank1_3);
	write32(&regs->pmacro_quse_ddll_rank1_4,
		param->EmcPmacroQuseDdllRank1_4);
	write32(&regs->pmacro_quse_ddll_rank1_5,
		param->EmcPmacroQuseDdllRank1_5);
	write32(&regs->pmacro_brick_ctrl_rfu1, param->EmcPmacroBrickCtrlRfu1);
	write32(&regs->pmacro_ob_ddll_long_dq_rank0_0,
		param->EmcPmacroObDdllLongDqRank0_0);
	write32(&regs->pmacro_ob_ddll_long_dq_rank0_1,
		param->EmcPmacroObDdllLongDqRank0_1);
	write32(&regs->pmacro_ob_ddll_long_dq_rank0_2,
		param->EmcPmacroObDdllLongDqRank0_2);
	write32(&regs->pmacro_ob_ddll_long_dq_rank0_3,
		param->EmcPmacroObDdllLongDqRank0_3);
	write32(&regs->pmacro_ob_ddll_long_dq_rank0_4,
		param->EmcPmacroObDdllLongDqRank0_4);
	write32(&regs->pmacro_ob_ddll_long_dq_rank0_5,
		param->EmcPmacroObDdllLongDqRank0_5);
	write32(&regs->pmacro_ob_ddll_long_dq_rank1_0,
		param->EmcPmacroObDdllLongDqRank1_0);
	write32(&regs->pmacro_ob_ddll_long_dq_rank1_1,
		param->EmcPmacroObDdllLongDqRank1_1);
	write32(&regs->pmacro_ob_ddll_long_dq_rank1_2,
		param->EmcPmacroObDdllLongDqRank1_2);
	write32(&regs->pmacro_ob_ddll_long_dq_rank1_3,
		param->EmcPmacroObDdllLongDqRank1_3);
	write32(&regs->pmacro_ob_ddll_long_dq_rank1_4,
		param->EmcPmacroObDdllLongDqRank1_4);
	write32(&regs->pmacro_ob_ddll_long_dq_rank1_5,
		param->EmcPmacroObDdllLongDqRank1_5);

	write32(&regs->pmacro_ob_ddll_long_dqs_rank0_0,
		param->EmcPmacroObDdllLongDqsRank0_0);
	write32(&regs->pmacro_ob_ddll_long_dqs_rank0_1,
		param->EmcPmacroObDdllLongDqsRank0_1);
	write32(&regs->pmacro_ob_ddll_long_dqs_rank0_2,
		param->EmcPmacroObDdllLongDqsRank0_2);
	write32(&regs->pmacro_ob_ddll_long_dqs_rank0_3,
		param->EmcPmacroObDdllLongDqsRank0_3);
	write32(&regs->pmacro_ob_ddll_long_dqs_rank0_4,
		param->EmcPmacroObDdllLongDqsRank0_4);
	write32(&regs->pmacro_ob_ddll_long_dqs_rank0_5,
		param->EmcPmacroObDdllLongDqsRank0_5);
	write32(&regs->pmacro_ob_ddll_long_dqs_rank1_0,
		param->EmcPmacroObDdllLongDqsRank1_0);
	write32(&regs->pmacro_ob_ddll_long_dqs_rank1_1,
		param->EmcPmacroObDdllLongDqsRank1_1);
	write32(&regs->pmacro_ob_ddll_long_dqs_rank1_2,
		param->EmcPmacroObDdllLongDqsRank1_2);
	write32(&regs->pmacro_ob_ddll_long_dqs_rank1_3,
		param->EmcPmacroObDdllLongDqsRank1_3);
	write32(&regs->pmacro_ob_ddll_long_dqs_rank1_4,
		param->EmcPmacroObDdllLongDqsRank1_4);
	write32(&regs->pmacro_ob_ddll_long_dqs_rank1_5,
		param->EmcPmacroObDdllLongDqsRank1_5);
	write32(&regs->pmacro_ib_ddll_long_dqs_rank0_0,
		param->EmcPmacroIbDdllLongDqsRank0_0);
	write32(&regs->pmacro_ib_ddll_long_dqs_rank0_1,
		param->EmcPmacroIbDdllLongDqsRank0_1);
	write32(&regs->pmacro_ib_ddll_long_dqs_rank0_2,
		param->EmcPmacroIbDdllLongDqsRank0_2);
	write32(&regs->pmacro_ib_ddll_long_dqs_rank0_3,
		param->EmcPmacroIbDdllLongDqsRank0_3);
	write32(&regs->pmacro_ib_ddll_long_dqs_rank1_0,
		param->EmcPmacroIbDdllLongDqsRank1_0);
	write32(&regs->pmacro_ib_ddll_long_dqs_rank1_1,
		param->EmcPmacroIbDdllLongDqsRank1_1);
	write32(&regs->pmacro_ib_ddll_long_dqs_rank1_2,
		param->EmcPmacroIbDdllLongDqsRank1_2);
	write32(&regs->pmacro_ib_ddll_long_dqs_rank1_3,
		param->EmcPmacroIbDdllLongDqsRank1_3);
	write32(&regs->pmacro_ddll_long_cmd_0, param->EmcPmacroDdllLongCmd_0);
	write32(&regs->pmacro_ddll_long_cmd_1, param->EmcPmacroDdllLongCmd_1);
	write32(&regs->pmacro_ddll_long_cmd_2, param->EmcPmacroDdllLongCmd_2);
	write32(&regs->pmacro_ddll_long_cmd_3, param->EmcPmacroDdllLongCmd_3);
	write32(&regs->pmacro_ddll_long_cmd_4, param->EmcPmacroDdllLongCmd_4);
	write32(&regs->pmacro_ddll_short_cmd_0, param->EmcPmacroDdllShortCmd_0);
	write32(&regs->pmacro_ddll_short_cmd_1, param->EmcPmacroDdllShortCmd_1);
	write32(&regs->pmacro_ddll_short_cmd_2, param->EmcPmacroDdllShortCmd_2);
	write32(&regs->pmacro_common_pad_tx_ctrl, cpm_step1);
}

static void sdram_setup_wpr_carveouts(const struct sdram_params *param,
				      struct tegra_mc_regs *regs)
{
	/* Program the 5 WPR carveouts with initial BCT settings. */
	write32(&regs->security_carveout1_bom,
		param->McGeneralizedCarveout1Bom);
	write32(&regs->security_carveout1_bom_hi,
		param->McGeneralizedCarveout1BomHi);
	write32(&regs->security_carveout1_size_128kb,
		param->McGeneralizedCarveout1Size128kb);
	write32(&regs->security_carveout1_ca0,
		param->McGeneralizedCarveout1Access0);
	write32(&regs->security_carveout1_ca1,
		param->McGeneralizedCarveout1Access1);
	write32(&regs->security_carveout1_ca2,
		param->McGeneralizedCarveout1Access2);
	write32(&regs->security_carveout1_ca3,
		param->McGeneralizedCarveout1Access3);
	write32(&regs->security_carveout1_ca4,
		param->McGeneralizedCarveout1Access4);
	write32(&regs->security_carveout1_cfia0,
		param->McGeneralizedCarveout1ForceInternalAccess0);
	write32(&regs->security_carveout1_cfia1,
		param->McGeneralizedCarveout1ForceInternalAccess1);
	write32(&regs->security_carveout1_cfia2,
		param->McGeneralizedCarveout1ForceInternalAccess2);
	write32(&regs->security_carveout1_cfia3,
		param->McGeneralizedCarveout1ForceInternalAccess3);
	write32(&regs->security_carveout1_cfia4,
		param->McGeneralizedCarveout1ForceInternalAccess4);
	write32(&regs->security_carveout1_cfg0,
		param->McGeneralizedCarveout1Cfg0);

	write32(&regs->security_carveout2_bom,
		param->McGeneralizedCarveout2Bom);
	write32(&regs->security_carveout2_bom_hi,
		param->McGeneralizedCarveout2BomHi);
	write32(&regs->security_carveout2_size_128kb,
		param->McGeneralizedCarveout2Size128kb);
	write32(&regs->security_carveout2_ca0,
		param->McGeneralizedCarveout2Access0);
	write32(&regs->security_carveout2_ca1,
		param->McGeneralizedCarveout2Access1);
	write32(&regs->security_carveout2_ca2,
		param->McGeneralizedCarveout2Access2);
	write32(&regs->security_carveout2_ca3,
		param->McGeneralizedCarveout2Access3);
	write32(&regs->security_carveout2_ca4,
		param->McGeneralizedCarveout2Access4);
	write32(&regs->security_carveout2_cfia0,
		param->McGeneralizedCarveout2ForceInternalAccess0);
	write32(&regs->security_carveout2_cfia1,
		param->McGeneralizedCarveout2ForceInternalAccess1);
	write32(&regs->security_carveout2_cfia2,
		param->McGeneralizedCarveout2ForceInternalAccess2);
	write32(&regs->security_carveout2_cfia3,
		param->McGeneralizedCarveout2ForceInternalAccess3);
	write32(&regs->security_carveout2_cfia4,
		param->McGeneralizedCarveout2ForceInternalAccess4);
	write32(&regs->security_carveout2_cfg0,
		param->McGeneralizedCarveout2Cfg0);

	write32(&regs->security_carveout3_bom,
		param->McGeneralizedCarveout3Bom);
	write32(&regs->security_carveout3_bom_hi,
		param->McGeneralizedCarveout3BomHi);
	write32(&regs->security_carveout3_size_128kb,
		param->McGeneralizedCarveout3Size128kb);
	write32(&regs->security_carveout3_ca0,
		param->McGeneralizedCarveout3Access0);
	write32(&regs->security_carveout3_ca1,
		param->McGeneralizedCarveout3Access1);
	write32(&regs->security_carveout3_ca2,
		param->McGeneralizedCarveout3Access2);
	write32(&regs->security_carveout3_ca3,
		param->McGeneralizedCarveout3Access3);
	write32(&regs->security_carveout3_ca4,
		param->McGeneralizedCarveout3Access4);
	write32(&regs->security_carveout3_cfia0,
		param->McGeneralizedCarveout3ForceInternalAccess0);
	write32(&regs->security_carveout3_cfia1,
		param->McGeneralizedCarveout3ForceInternalAccess1);
	write32(&regs->security_carveout3_cfia2,
		param->McGeneralizedCarveout3ForceInternalAccess2);
	write32(&regs->security_carveout3_cfia3,
		param->McGeneralizedCarveout3ForceInternalAccess3);
	write32(&regs->security_carveout3_cfia4,
		param->McGeneralizedCarveout3ForceInternalAccess4);
	write32(&regs->security_carveout3_cfg0,
		param->McGeneralizedCarveout3Cfg0);

	write32(&regs->security_carveout4_bom,
		param->McGeneralizedCarveout4Bom);
	write32(&regs->security_carveout4_bom_hi,
		param->McGeneralizedCarveout4BomHi);
	write32(&regs->security_carveout4_size_128kb,
		param->McGeneralizedCarveout4Size128kb);
	write32(&regs->security_carveout4_ca0,
		param->McGeneralizedCarveout4Access0);
	write32(&regs->security_carveout4_ca1,
		param->McGeneralizedCarveout4Access1);
	write32(&regs->security_carveout4_ca2,
		param->McGeneralizedCarveout4Access2);
	write32(&regs->security_carveout4_ca3,
		param->McGeneralizedCarveout4Access3);
	write32(&regs->security_carveout4_ca4,
		param->McGeneralizedCarveout4Access4);
	write32(&regs->security_carveout4_cfia0,
		param->McGeneralizedCarveout4ForceInternalAccess0);
	write32(&regs->security_carveout4_cfia1,
		param->McGeneralizedCarveout4ForceInternalAccess1);
	write32(&regs->security_carveout4_cfia2,
		param->McGeneralizedCarveout4ForceInternalAccess2);
	write32(&regs->security_carveout4_cfia3,
		param->McGeneralizedCarveout4ForceInternalAccess3);
	write32(&regs->security_carveout4_cfia4,
		param->McGeneralizedCarveout4ForceInternalAccess4);
	write32(&regs->security_carveout4_cfg0,
		param->McGeneralizedCarveout4Cfg0);

	write32(&regs->security_carveout5_bom,
		param->McGeneralizedCarveout5Bom);
	write32(&regs->security_carveout5_bom_hi,
		param->McGeneralizedCarveout5BomHi);
	write32(&regs->security_carveout5_size_128kb,
		param->McGeneralizedCarveout5Size128kb);
	write32(&regs->security_carveout5_ca0,
		param->McGeneralizedCarveout5Access0);
	write32(&regs->security_carveout5_ca1,
		param->McGeneralizedCarveout5Access1);
	write32(&regs->security_carveout5_ca2,
		param->McGeneralizedCarveout5Access2);
	write32(&regs->security_carveout5_ca3,
		param->McGeneralizedCarveout5Access3);
	write32(&regs->security_carveout5_ca4,
		param->McGeneralizedCarveout5Access4);
	write32(&regs->security_carveout5_cfia0,
		param->McGeneralizedCarveout5ForceInternalAccess0);
	write32(&regs->security_carveout5_cfia1,
		param->McGeneralizedCarveout5ForceInternalAccess1);
	write32(&regs->security_carveout5_cfia2,
		param->McGeneralizedCarveout5ForceInternalAccess2);
	write32(&regs->security_carveout5_cfia3,
		param->McGeneralizedCarveout5ForceInternalAccess3);
	write32(&regs->security_carveout5_cfia4,
		param->McGeneralizedCarveout5ForceInternalAccess4);
	write32(&regs->security_carveout5_cfg0,
		param->McGeneralizedCarveout5Cfg0);
}

static void sdram_init_mc(const struct sdram_params *param,
			  struct tegra_mc_regs *regs)
{
	/* Initialize MC VPR settings */
	write32(&regs->video_protect_bom, param->McVideoProtectBom);
	write32(&regs->video_protect_bom_adr_hi,
		param->McVideoProtectBomAdrHi);
	write32(&regs->video_protect_size_mb, param->McVideoProtectSizeMb);
	write32(&regs->video_protect_vpr_override,
		param->McVideoProtectVprOverride);
	write32(&regs->video_protect_vpr_override1,
		param->McVideoProtectVprOverride1);
	write32(&regs->video_protect_gpu_override_0,
		param->McVideoProtectGpuOverride0);
	write32(&regs->video_protect_gpu_override_1,
		param->McVideoProtectGpuOverride1);

	/* Program SDRAM geometry paarameters */
	write32(&regs->emem_adr_cfg, param->McEmemAdrCfg);
	write32(&regs->emem_adr_cfg_dev0, param->McEmemAdrCfgDev0);
	write32(&regs->emem_adr_cfg_dev1, param->McEmemAdrCfgDev1);
	write32(&regs->emem_adr_cfg_channel_mask,
		param->McEmemAdrCfgChannelMask);

	/* Program bank swizzling */
	write32(&regs->emem_adr_cfg_bank_mask_0, param->McEmemAdrCfgBankMask0);
	write32(&regs->emem_adr_cfg_bank_mask_1, param->McEmemAdrCfgBankMask1);
	write32(&regs->emem_adr_cfg_bank_mask_2, param->McEmemAdrCfgBankMask2);

	/* Program external memory aperature (base and size) */
	write32(&regs->emem_cfg, param->McEmemCfg);

	/* Program SEC carveout (base and size) */
	write32(&regs->sec_carveout_bom, param->McSecCarveoutBom);
	write32(&regs->sec_carveout_adr_hi, param->McSecCarveoutAdrHi);
	write32(&regs->sec_carveout_size_mb, param->McSecCarveoutSizeMb);

	/* Program MTS carveout (base and size) */
	write32(&regs->mts_carveout_bom, param->McMtsCarveoutBom);
	write32(&regs->mts_carveout_adr_hi, param->McMtsCarveoutAdrHi);
	write32(&regs->mts_carveout_size_mb, param->McMtsCarveoutSizeMb);

	/* Initialize the WPR carveouts */
	sdram_setup_wpr_carveouts(param, regs);

	/* Program the memory arbiter */
	write32(&regs->emem_arb_cfg, param->McEmemArbCfg);
	write32(&regs->emem_arb_outstanding_req,
		param->McEmemArbOutstandingReq);
	write32(&regs->emem_arb_refpb_hp_ctrl, param->McEmemArbRefpbHpCtrl);
	write32(&regs->emem_arb_refpb_bank_ctrl, param->McEmemArbRefpbBankCtrl);
	write32(&regs->emem_arb_timing_rcd, param->McEmemArbTimingRcd);
	write32(&regs->emem_arb_timing_rp, param->McEmemArbTimingRp);
	write32(&regs->emem_arb_timing_rc, param->McEmemArbTimingRc);
	write32(&regs->emem_arb_timing_ras, param->McEmemArbTimingRas);
	write32(&regs->emem_arb_timing_faw, param->McEmemArbTimingFaw);
	write32(&regs->emem_arb_timing_rrd, param->McEmemArbTimingRrd);
	write32(&regs->emem_arb_timing_rap2pre, param->McEmemArbTimingRap2Pre);
	write32(&regs->emem_arb_timing_wap2pre, param->McEmemArbTimingWap2Pre);
	write32(&regs->emem_arb_timing_r2r, param->McEmemArbTimingR2R);
	write32(&regs->emem_arb_timing_w2w, param->McEmemArbTimingW2W);
	write32(&regs->emem_arb_timing_ccdmw, param->McEmemArbTimingCcdmw);
	write32(&regs->emem_arb_timing_r2w, param->McEmemArbTimingR2W);
	write32(&regs->emem_arb_timing_w2r, param->McEmemArbTimingW2R);
	write32(&regs->emem_arb_timing_rfcpb, param->McEmemArbTimingRFCPB);
	write32(&regs->emem_arb_da_turns, param->McEmemArbDaTurns);
	write32(&regs->emem_arb_da_covers, param->McEmemArbDaCovers);
	write32(&regs->emem_arb_misc0, param->McEmemArbMisc0);
	write32(&regs->emem_arb_misc1, param->McEmemArbMisc1);
	write32(&regs->emem_arb_misc2, param->McEmemArbMisc2);
	write32(&regs->emem_arb_ring1_throttle, param->McEmemArbRing1Throttle);
	write32(&regs->emem_arb_override, param->McEmemArbOverride);
	write32(&regs->emem_arb_override_1, param->McEmemArbOverride1);
	write32(&regs->emem_arb_rsv, param->McEmemArbRsv);
	write32(&regs->da_config0, param->McDaCfg0);

	/* Trigger MC timing update */
	write32(&regs->timing_control, EMC_TIMING_CONTROL_TIMING_UPDATE);

	/* Program second-level clock enable overrides */
	write32(&regs->clken_override, param->McClkenOverride);

	/* Program statistics gathering */
	write32(&regs->stat_control, param->McStatControl);
}

static void sdram_init_emc(const struct sdram_params *param,
			   struct tegra_emc_regs *regs)
{
	/* Program SDRAM geometry parameters */
	write32(&regs->adr_cfg, param->EmcAdrCfg);

	/* Program second-level clock enable overrides */
	write32(&regs->clken_override, param->EmcClkenOverride);

	/* Program EMC pad auto calibration */
	write32(&regs->pmacro_autocal_cfg0, param->EmcPmacroAutocalCfg0);
	write32(&regs->pmacro_autocal_cfg1, param->EmcPmacroAutocalCfg1);
	write32(&regs->pmacro_autocal_cfg2, param->EmcPmacroAutocalCfg2);

	write32(&regs->auto_cal_vref_sel0, param->EmcAutoCalVrefSel0);
	write32(&regs->auto_cal_vref_sel1, param->EmcAutoCalVrefSel1);

	write32(&regs->auto_cal_interval, param->EmcAutoCalInterval);
	write32(&regs->auto_cal_config, param->EmcAutoCalConfig);
	udelay(param->EmcAutoCalWait);
}

static void sdram_set_emc_timing(const struct sdram_params *param,
				 struct tegra_emc_regs *regs)
{
	/* Program EMC timing configuration */
	write32(&regs->cfg_2, param->EmcCfg2);
	write32(&regs->cfg_pipe, param->EmcCfgPipe);
	write32(&regs->cfg_pipe1, param->EmcCfgPipe1);
	write32(&regs->cfg_pipe2, param->EmcCfgPipe2);
	write32(&regs->cmdq, param->EmcCmdQ);
	write32(&regs->mc2emcq, param->EmcMc2EmcQ);
	write32(&regs->mrs_wait_cnt, param->EmcMrsWaitCnt);
	write32(&regs->mrs_wait_cnt2, param->EmcMrsWaitCnt2);
	write32(&regs->fbio_cfg5, param->EmcFbioCfg5);
	write32(&regs->rc, param->EmcRc);
	write32(&regs->rfc, param->EmcRfc);
	write32(&regs->rfcpb, param->EmcRfcPb);
	write32(&regs->refctrl2, param->EmcRefctrl2);
	write32(&regs->rfc_slr, param->EmcRfcSlr);
	write32(&regs->ras, param->EmcRas);
	write32(&regs->rp, param->EmcRp);
	write32(&regs->tppd, param->EmcTppd);
	write32(&regs->r2r, param->EmcR2r);
	write32(&regs->w2w, param->EmcW2w);
	write32(&regs->r2w, param->EmcR2w);
	write32(&regs->w2r, param->EmcW2r);
	write32(&regs->r2p, param->EmcR2p);
	write32(&regs->w2p, param->EmcW2p);
	write32(&regs->ccdmw, param->EmcCcdmw);
	write32(&regs->rd_rcd, param->EmcRdRcd);
	write32(&regs->wr_rcd, param->EmcWrRcd);
	write32(&regs->rrd, param->EmcRrd);
	write32(&regs->rext, param->EmcRext);
	write32(&regs->wext, param->EmcWext);
	write32(&regs->wdv, param->EmcWdv);
	write32(&regs->wdv_chk, param->EmcWdvChk);
	write32(&regs->wsv, param->EmcWsv);
	write32(&regs->wev, param->EmcWev);
	write32(&regs->wdv_mask, param->EmcWdvMask);
	write32(&regs->ws_duration, param->EmcWsDuration);
	write32(&regs->we_duration, param->EmcWeDuration);
	write32(&regs->quse, param->EmcQUse);
	write32(&regs->quse_width, param->EmcQuseWidth);
	write32(&regs->ibdly, param->EmcIbdly);
	write32(&regs->obdly, param->EmcObdly);
	write32(&regs->einput, param->EmcEInput);
	write32(&regs->einput_duration, param->EmcEInputDuration);
	write32(&regs->puterm_extra, param->EmcPutermExtra);
	write32(&regs->puterm_width, param->EmcPutermWidth);

	write32(&regs->pmacro_common_pad_tx_ctrl,
		param->EmcPmacroCommonPadTxCtrl);
	write32(&regs->dbg, param->EmcDbg);
	write32(&regs->qrst, param->EmcQRst);
	write32(&regs->issue_qrst, 1);
	write32(&regs->issue_qrst, 0);
	write32(&regs->qsafe, param->EmcQSafe);
	write32(&regs->rdv, param->EmcRdv);
	write32(&regs->rdv_mask, param->EmcRdvMask);
	write32(&regs->rdv_early, param->EmcRdvEarly);
	write32(&regs->rdv_early_mask, param->EmcRdvEarlyMask);
	write32(&regs->qpop, param->EmcQpop);
	write32(&regs->refresh, param->EmcRefresh);
	write32(&regs->burst_refresh_num, param->EmcBurstRefreshNum);
	write32(&regs->pre_refresh_req_cnt, param->EmcPreRefreshReqCnt);
	write32(&regs->pdex2wr, param->EmcPdEx2Wr);
	write32(&regs->pdex2rd, param->EmcPdEx2Rd);
	write32(&regs->pchg2pden, param->EmcPChg2Pden);
	write32(&regs->act2pden, param->EmcAct2Pden);
	write32(&regs->ar2pden, param->EmcAr2Pden);
	write32(&regs->rw2pden, param->EmcRw2Pden);
	write32(&regs->cke2pden, param->EmcCke2Pden);
	write32(&regs->pdex2cke, param->EmcPdex2Cke);
	write32(&regs->pdex2mrr, param->EmcPdex2Mrr);
	write32(&regs->txsr, param->EmcTxsr);
	write32(&regs->txsrdll, param->EmcTxsrDll);
	write32(&regs->tcke, param->EmcTcke);
	write32(&regs->tckesr, param->EmcTckesr);
	write32(&regs->tpd, param->EmcTpd);
	write32(&regs->tfaw, param->EmcTfaw);
	write32(&regs->trpab, param->EmcTrpab);
	write32(&regs->tclkstable, param->EmcTClkStable);
	write32(&regs->tclkstop, param->EmcTClkStop);
	write32(&regs->trefbw, param->EmcTRefBw);
	write32(&regs->odt_write, param->EmcOdtWrite);
	write32(&regs->cfg_dig_dll, param->EmcCfgDigDll);
	write32(&regs->cfg_dig_dll_period, param->EmcCfgDigDllPeriod);

	/* Don't write CFG_ADR_EN (bit 1) here - lock bit written later */
	write32(&regs->fbio_spare, param->EmcFbioSpare & ~CFG_ADR_EN_LOCKED);
	write32(&regs->cfg_rsv, param->EmcCfgRsv);
	write32(&regs->pmc_scratch1, param->EmcPmcScratch1);
	write32(&regs->pmc_scratch2, param->EmcPmcScratch2);
	write32(&regs->pmc_scratch3, param->EmcPmcScratch3);
	write32(&regs->acpd_control, param->EmcAcpdControl);
	write32(&regs->txdsrvttgen, param->EmcTxdsrvttgen);

	/*
	 * Set pipe bypass enable bits before sending any DRAM commands.
	 * Note other bits in EMC_CFG must be set AFTER REFCTRL is configured.
	 */
	writebits(param->EmcCfg, &regs->cfg,
		  (EMC_CFG_EMC2PMACRO_CFG_BYPASS_ADDRPIPE_MASK |
		   EMC_CFG_EMC2PMACRO_CFG_BYPASS_DATAPIPE1_MASK |
		   EMC_CFG_EMC2PMACRO_CFG_BYPASS_DATAPIPE2_MASK));
}

static void sdram_patch_bootrom(const struct sdram_params *param,
				struct tegra_mc_regs *regs)
{
	if (param->BootRomPatchControl & BOOT_ROM_PATCH_CONTROL_ENABLE_MASK) {
		uintptr_t addr = ((param->BootRomPatchControl &
				  BOOT_ROM_PATCH_CONTROL_OFFSET_MASK) >>
				  BOOT_ROM_PATCH_CONTROL_OFFSET_SHIFT);
		addr = BOOT_ROM_PATCH_CONTROL_BASE_ADDRESS + (addr << 2);
		write32((uint32_t *)addr, param->BootRomPatchData);
		write32(&regs->timing_control,
			EMC_TIMING_CONTROL_TIMING_UPDATE);
	}
}

static void sdram_rel_dpd(const struct sdram_params *param,
			  struct tegra_pmc_regs *regs)
{
	u32 dpd3_val, dpd3_val_sel_dpd;

	/* Release SEL_DPD_CMD */
	dpd3_val = (param->EmcPmcScratch1 & 0x3FFFFFFF) | DPD_OFF;
	dpd3_val_sel_dpd = dpd3_val & 0xCFFF0000;
	write32(&regs->io_dpd3_req, dpd3_val_sel_dpd);
	udelay(param->PmcIoDpd3ReqWait);
}

/* Program DPD3/DPD4 regs (coldboot path) */
static void sdram_set_dpd(const struct sdram_params *param,
			  struct tegra_pmc_regs *regs)
{
	u32 dpd3_val, dpd3_val_sel_dpd;
	u32 dpd4_val, dpd4_val_e_dpd, dpd4_val_e_dpd_vttgen;

	/* Enable sel_dpd on unused pins */
	dpd3_val = (param->EmcPmcScratch1 & 0x3FFFFFFF) | DPD_ON;
	dpd3_val_sel_dpd = (dpd3_val ^ 0x0000FFFF) & 0xC000FFFF;
	write32(&regs->io_dpd3_req, dpd3_val_sel_dpd);
	udelay(param->PmcIoDpd3ReqWait);

	dpd4_val = dpd3_val;
	/* Disable e_dpd_vttgen */
	dpd4_val_e_dpd_vttgen = (dpd4_val ^ 0x3FFF0000) & 0xFFFF0000;
	write32(&regs->io_dpd4_req, dpd4_val_e_dpd_vttgen);
	udelay(param->PmcIoDpd4ReqWait);

	/* Disable e_dpd_bg */
	dpd4_val_e_dpd = (dpd4_val ^ 0x0000FFFF) & 0xC000FFFF;
	write32(&regs->io_dpd4_req, dpd4_val_e_dpd);
	udelay(param->PmcIoDpd4ReqWait);

	write32(&regs->weak_bias, 0);
	/* Add a wait to make sure clock switch takes place */
	udelay(1);
}

static void sdram_set_clock_enable_signal(const struct sdram_params *param,
					  struct tegra_emc_regs *regs)
{
	volatile uint32_t dummy = 0;
	uint32_t val = 0;

	if (param->MemoryType == NvBootMemoryType_LpDdr4) {

		val = (param->EmcPinGpioEn << EMC_PIN_GPIOEN_SHIFT) |
		      (param->EmcPinGpio << EMC_PIN_GPIO_SHIFT);
		write32(&regs->pin, val);

		clrbits32(&regs->pin,
			  (EMC_PIN_RESET_MASK | EMC_PIN_DQM_MASK |
			   EMC_PIN_CKE_MASK));
		/*
		 * Assert dummy read of PIN register to ensure above write goes
		 * through. Wait an additional 200us here as per NVIDIA.
		 */
		dummy |= read32(&regs->pin);
		udelay(param->EmcPinExtraWait + 200);

		/* Deassert reset */
		setbits32(&regs->pin, EMC_PIN_RESET_INACTIVE);

		/*
		 * Assert dummy read of PIN register to ensure above write goes
		 * through. Wait an additional 2000us here as per NVIDIA.
		 */
		dummy |= read32(&regs->pin);
		udelay(param->EmcPinExtraWait + 2000);
	}

	/* Enable clock enable signal */
	setbits32(&regs->pin, EMC_PIN_CKE_NORMAL);

	/* Dummy read of PIN register to ensure final write goes through */
	dummy |= read32(&regs->pin);
	udelay(param->EmcPinProgramWait);

	if (!dummy)
		die("Failed to program EMC pin.");

	if (param->MemoryType != NvBootMemoryType_LpDdr4) {

		/* Send NOP (trigger just needs to be non-zero) */
		writebits(((1 << EMC_NOP_CMD_SHIFT) |
			  (param->EmcDevSelect << EMC_NOP_DEV_SELECTN_SHIFT)),
			  &regs->nop,
			  EMC_NOP_CMD_MASK | EMC_NOP_DEV_SELECTN_MASK);
	}

	/* On coldboot w/LPDDR2/3, wait 200 uSec after asserting CKE high */
	if (param->MemoryType == NvBootMemoryType_LpDdr2)
		udelay(param->EmcPinExtraWait + 200);
}

static void sdram_init_lpddr3(const struct sdram_params *param,
			      struct tegra_emc_regs *regs)
{
	/* Precharge all banks. DEV_SELECTN = 0 => Select all devices */
	write32(&regs->pre,
		((param->EmcDevSelect << EMC_REF_DEV_SELECTN_SHIFT) | 1));

	/* Send Reset MRW command */
	write32(&regs->mrw, param->EmcMrwResetCommand);
	udelay(param->EmcMrwResetNInitWait);

	write32(&regs->mrw, param->EmcZcalInitDev0);
	udelay(param->EmcZcalInitWait);

	if ((param->EmcDevSelect & 2) == 0) {
		write32(&regs->mrw, param->EmcZcalInitDev1);
		udelay(param->EmcZcalInitWait);
	}

	/* Write mode registers */
	write32(&regs->mrw2, param->EmcMrw2);
	write32(&regs->mrw, param->EmcMrw1);
	write32(&regs->mrw3, param->EmcMrw3);
	write32(&regs->mrw4, param->EmcMrw4);

	/* Patch 6 using BCT spare variables */
	sdram_patch(param->EmcBctSpare10, param->EmcBctSpare11);

	if (param->EmcExtraModeRegWriteEnable)
		write32(&regs->mrw, param->EmcMrwExtra);
}

static void sdram_init_lpddr4(const struct sdram_params *param,
			      struct tegra_emc_regs *regs)
{
	/* Patch 6 using BCT spare variables */
	sdram_patch(param->EmcBctSpare10, param->EmcBctSpare11);

	/* Write mode registers */
	write32(&regs->mrw2, param->EmcMrw2);
	write32(&regs->mrw, param->EmcMrw1);
	write32(&regs->mrw3, param->EmcMrw3);
	write32(&regs->mrw4, param->EmcMrw4);
	write32(&regs->mrw6, param->EmcMrw6);
	write32(&regs->mrw14, param->EmcMrw14);

	write32(&regs->mrw8, param->EmcMrw8);
	write32(&regs->mrw12, param->EmcMrw12);
	write32(&regs->mrw9, param->EmcMrw9);
	write32(&regs->mrw13, param->EmcMrw13);

	/* Issue ZQCAL start, device 0 */
	write32(&regs->zq_cal, param->EmcZcalInitDev0);
	udelay(param->EmcZcalInitWait);
	/* Issue ZQCAL latch */
	write32(&regs->zq_cal, (param->EmcZcalInitDev0 ^ 0x3));

	if ((param->EmcDevSelect & 2) == 0) {
		/* Same for device 1 */
		write32(&regs->zq_cal, param->EmcZcalInitDev1);
		udelay(param->EmcZcalInitWait);
		write32(&regs->zq_cal, (param->EmcZcalInitDev1 ^ 0x3));
	}
}

static void sdram_init_zq_calibration(const struct sdram_params *param,
				      struct tegra_emc_regs *regs)
{
	if (param->MemoryType == NvBootMemoryType_LpDdr2)
		sdram_init_lpddr3(param, regs);
	else if (param->MemoryType == NvBootMemoryType_LpDdr4)
		sdram_init_lpddr4(param, regs);
}

static void sdram_set_zq_calibration(const struct sdram_params *param,
				     struct tegra_emc_regs *regs)
{
	if (param->EmcAutoCalInterval == 0)
		write32(&regs->auto_cal_config,
			param->EmcAutoCalConfig | AUTOCAL_MEASURE_STALL_ENABLE);

	write32(&regs->pmacro_brick_ctrl_rfu2, param->EmcPmacroBrickCtrlRfu2);

	/* ZQ CAL setup (not actually issuing ZQ CAL now) */
	if (param->MemoryType == NvBootMemoryType_LpDdr4) {
		write32(&regs->zcal_wait_cnt, param->EmcZcalWaitCnt);
		write32(&regs->zcal_mrw_cmd, param->EmcZcalMrwCmd);
	}

	sdram_trigger_emc_timing_update(regs);
	udelay(param->EmcTimingControlWait);
}

static void sdram_set_refresh(const struct sdram_params *param,
			      struct tegra_emc_regs *regs)
{
	/* Insert burst refresh */
	if (param->EmcExtraRefreshNum > 0) {
		uint32_t refresh_num = (1 << param->EmcExtraRefreshNum) - 1;

		writebits((EMC_REF_CMD_REFRESH | EMC_REF_NORMAL_ENABLED |
			   (refresh_num << EMC_REF_NUM_SHIFT) |
			   (param->EmcDevSelect << EMC_REF_DEV_SELECTN_SHIFT)),
			  &regs->ref, (EMC_REF_CMD_MASK | EMC_REF_NORMAL_MASK |
				       EMC_REF_NUM_MASK |
				       EMC_REF_DEV_SELECTN_MASK));
	}

	/* Enable refresh */
	write32(&regs->refctrl,
		(param->EmcDevSelect | EMC_REFCTRL_REF_VALID_ENABLED));

	/*
	 * NOTE: Programming CFG must happen after REFCTRL to delay
	 * active power-down to after init (DDR2 constraint).
	 */
	write32(&regs->dyn_self_ref_control, param->EmcDynSelfRefControl);
	write32(&regs->cfg_update, param->EmcCfgUpdate);
	write32(&regs->cfg, param->EmcCfg);
	write32(&regs->fdpd_ctrl_dq, param->EmcFdpdCtrlDq);
	write32(&regs->fdpd_ctrl_cmd, param->EmcFdpdCtrlCmd);
	write32(&regs->sel_dpd_ctrl, param->EmcSelDpdCtrl);

	/* Write addr swizzle lock bit */
	write32(&regs->fbio_spare, param->EmcFbioSpare | CFG_ADR_EN_LOCKED);

	/* Re-trigger timing to latch power saving functions */
	sdram_trigger_emc_timing_update(regs);

	/* Enable EMC pipe clock gating */
	write32(&regs->cfg_pipe_clk, param->EmcCfgPipeClk);
	/* Depending on frequency, enable CMD/CLK fdpd */
	write32(&regs->fdpd_ctrl_cmd_no_ramp, param->EmcFdpdCtrlCmdNoRamp);
}

#define AHB_ARB_XBAR_CTRL	0x6000C0E0

static void sdram_enable_arbiter(const struct sdram_params *param)
{
	/* TODO(hungte) Move values here to standalone header file. */
	uint32_t *ahb_arbitration_xbar_ctrl = (uint32_t *)(AHB_ARB_XBAR_CTRL);

	setbits32(ahb_arbitration_xbar_ctrl,
		     param->AhbArbitrationXbarCtrlMemInitDone << 16);
}

static void sdram_lock_carveouts(const struct sdram_params *param,
				 struct tegra_mc_regs *regs)
{
	/* Lock carveouts, and emem_cfg registers */
	write32(&regs->video_protect_reg_ctrl,
		param->McVideoProtectWriteAccess);
	write32(&regs->sec_carveout_reg_ctrl,
		param->McSecCarveoutProtectWriteAccess);
	write32(&regs->mts_carveout_reg_ctrl, param->McMtsCarveoutRegCtrl);

	/* Write this last, locks access */
	write32(&regs->emem_cfg_access_ctrl,
		MC_EMEM_CFG_ACCESS_CTRL_WRITE_ACCESS_DISABLED);
}

void sdram_init(const struct sdram_params *param)
{
	struct tegra_pmc_regs *pmc = (struct tegra_pmc_regs *)TEGRA_PMC_BASE;
	struct tegra_mc_regs *mc = (struct tegra_mc_regs *)TEGRA_MC_BASE;
	struct tegra_emc_regs *emc = (struct tegra_emc_regs *)TEGRA_EMC_BASE;

	printk(BIOS_DEBUG, "Initializing SDRAM of type %d with %dKHz\n",
		param->MemoryType, clock_get_pll_input_khz() *
		param->PllMFeedbackDivider / param->PllMInputDivider /
		(1 + param->PllMPostDivider));
	if (param->MemoryType != NvBootMemoryType_LpDdr4 &&
	    param->MemoryType != NvBootMemoryType_LpDdr2)
		die("Unsupported memory type!\n");

	sdram_configure_pmc(param, pmc);
	sdram_patch(param->EmcBctSpare0, param->EmcBctSpare1);

	sdram_set_dpd(param, pmc);
	sdram_start_clocks(param, emc);
	sdram_set_pad_macros(param, emc);
	sdram_patch(param->EmcBctSpare4, param->EmcBctSpare5);

	sdram_trigger_emc_timing_update(emc);
	sdram_init_mc(param, mc);
	sdram_init_emc(param, emc);
	sdram_patch(param->EmcBctSpare8, param->EmcBctSpare9);

	sdram_set_emc_timing(param, emc);
	sdram_patch_bootrom(param, mc);
	sdram_rel_dpd(param, pmc);
	sdram_set_zq_calibration(param, emc);
	sdram_set_ddr_control(param, pmc);
	sdram_set_clock_enable_signal(param, emc);

	sdram_init_zq_calibration(param, emc);

	/* Set package and DPD pad control */
	write32(&pmc->ddr_cfg, param->PmcDdrCfg);

	/* Start periodic ZQ calibration (LPDDRx only) */
	if (param->MemoryType == NvBootMemoryType_LpDdr4 ||
	    param->MemoryType == NvBootMemoryType_LpDdr2) {
		write32(&emc->zcal_interval, param->EmcZcalInterval);
		write32(&emc->zcal_wait_cnt, param->EmcZcalWaitCnt);
		write32(&emc->zcal_mrw_cmd, param->EmcZcalMrwCmd);
	}
	sdram_patch(param->EmcBctSpare12, param->EmcBctSpare13);

	sdram_trigger_emc_timing_update(emc);
	sdram_set_refresh(param, emc);
	sdram_enable_arbiter(param);
	sdram_lock_carveouts(param, mc);
}
