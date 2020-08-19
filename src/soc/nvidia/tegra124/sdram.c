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
#include <symbols.h>

static void sdram_patch(uintptr_t addr, uint32_t value)
{
	if (addr)
		write32((uint32_t *)addr, value);
}

static void writebits(uint32_t value, uint32_t *addr, uint32_t mask)
{
	clrsetbits32(addr, mask, (value & mask));
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

	/* Set package and DPD pad control */
	writebits(param->PmcDdrCfg, &regs->ddr_cfg,
		  (PMC_DDR_CFG_PKG_MASK | PMC_DDR_CFG_IF_MASK |
		   PMC_DDR_CFG_XM0_RESET_TRI_MASK |
		   PMC_DDR_CFG_XM0_RESET_DPDIO_MASK));

	/* Turn on MEM IO Power */
	writebits(param->PmcNoIoPower, &regs->no_iopower,
		  (PMC_NO_IOPOWER_MEM_MASK | PMC_NO_IOPOWER_MEM_COMP_MASK));

	write32(&regs->reg_short, param->PmcRegShort);
}

static void sdram_start_clocks(const struct sdram_params *param)
{
	u32 is_same_freq = (param->McEmemArbMisc0 &
			    MC_EMEM_ARB_MISC0_MC_EMC_SAME_FREQ_MASK) ? 1 : 0;

	clock_sdram(param->PllMInputDivider, param->PllMFeedbackDivider,
		    param->PllMSelectDiv2, param->PllMSetupControl,
		    param->PllMPDLshiftPh45, param->PllMPDLshiftPh90,
		    param->PllMPDLshiftPh135, param->PllMKVCO,
		    param->PllMKCP, param->PllMStableTime,
		    param->EmcClockSource, is_same_freq);
}

static void sdram_deassert_clock_enable_signal(const struct sdram_params *param,
					       struct tegra_pmc_regs *regs)
{
	clrbits32(&regs->por_dpd_ctrl,
		  PMC_POR_DPD_CTRL_MEM0_HOLD_CKE_LOW_OVR_MASK);
	udelay(param->PmcPorDpdCtrlWait);
}

static void sdram_deassert_sel_dpd(const struct sdram_params *param,
				   struct tegra_pmc_regs *regs)
{
	clrbits32(&regs->por_dpd_ctrl,
		  (PMC_POR_DPD_CTRL_MEM0_ADDR0_CLK_SEL_DPD_MASK |
		   PMC_POR_DPD_CTRL_MEM0_ADDR1_CLK_SEL_DPD_MASK));
	/*
	 * Note NVIDIA recommended to always do 10us delay here and ignore
	 * BCT.PmcPorDpdCtrlWait.
	 * */
	udelay(10);
}

static void sdram_set_swizzle(const struct sdram_params *param,
			      struct tegra_emc_regs *regs)
{
	write32(&regs->swizzle_rank0_byte_cfg, param->EmcSwizzleRank0ByteCfg);
	write32(&regs->swizzle_rank0_byte0, param->EmcSwizzleRank0Byte0);
	write32(&regs->swizzle_rank0_byte1, param->EmcSwizzleRank0Byte1);
	write32(&regs->swizzle_rank0_byte2, param->EmcSwizzleRank0Byte2);
	write32(&regs->swizzle_rank0_byte3, param->EmcSwizzleRank0Byte3);
	write32(&regs->swizzle_rank1_byte_cfg, param->EmcSwizzleRank1ByteCfg);
	write32(&regs->swizzle_rank1_byte0, param->EmcSwizzleRank1Byte0);
	write32(&regs->swizzle_rank1_byte1, param->EmcSwizzleRank1Byte1);
	write32(&regs->swizzle_rank1_byte2, param->EmcSwizzleRank1Byte2);
	write32(&regs->swizzle_rank1_byte3, param->EmcSwizzleRank1Byte3);
}

static void sdram_set_pad_controls(const struct sdram_params *param,
				   struct tegra_emc_regs *regs)
{
	/* Program the pad controls */
	write32(&regs->xm2cmdpadctrl, param->EmcXm2CmdPadCtrl);
	write32(&regs->xm2cmdpadctrl2, param->EmcXm2CmdPadCtrl2);
	write32(&regs->xm2cmdpadctrl3, param->EmcXm2CmdPadCtrl3);
	write32(&regs->xm2cmdpadctrl4, param->EmcXm2CmdPadCtrl4);
	write32(&regs->xm2cmdpadctrl5, param->EmcXm2CmdPadCtrl5);

	write32(&regs->xm2dqspadctrl, param->EmcXm2DqsPadCtrl);
	write32(&regs->xm2dqspadctrl2, param->EmcXm2DqsPadCtrl2);
	write32(&regs->xm2dqspadctrl3, param->EmcXm2DqsPadCtrl3);
	write32(&regs->xm2dqspadctrl4, param->EmcXm2DqsPadCtrl4);
	write32(&regs->xm2dqspadctrl5, param->EmcXm2DqsPadCtrl5);
	write32(&regs->xm2dqspadctrl6, param->EmcXm2DqsPadCtrl6);

	write32(&regs->xm2dqpadctrl, param->EmcXm2DqPadCtrl);
	write32(&regs->xm2dqpadctrl2, param->EmcXm2DqPadCtrl2);
	write32(&regs->xm2dqpadctrl3, param->EmcXm2DqPadCtrl3);

	write32(&regs->xm2clkpadctrl, param->EmcXm2ClkPadCtrl);
	write32(&regs->xm2clkpadctrl2, param->EmcXm2ClkPadCtrl2);

	write32(&regs->xm2comppadctrl, param->EmcXm2CompPadCtrl);

	write32(&regs->xm2vttgenpadctrl, param->EmcXm2VttGenPadCtrl);
	write32(&regs->xm2vttgenpadctrl2, param->EmcXm2VttGenPadCtrl2);
	write32(&regs->xm2vttgenpadctrl3, param->EmcXm2VttGenPadCtrl3);

	write32(&regs->ctt_term_ctrl, param->EmcCttTermCtrl);
}

static void sdram_trigger_emc_timing_update(struct tegra_emc_regs *regs)
{
	write32(&regs->timing_control, EMC_TIMING_CONTROL_TIMING_UPDATE);
}

static void sdram_init_mc(const struct sdram_params *param,
			  struct tegra_mc_regs *regs)
{
	/* Initialize MC VPR settings */
	write32(&regs->display_snap_ring, param->McDisplaySnapRing);
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

	/* Program bank swizzling */
	write32(&regs->emem_bank_swizzle_cfg0, param->McEmemAdrCfgBankMask0);
	write32(&regs->emem_bank_swizzle_cfg1, param->McEmemAdrCfgBankMask1);
	write32(&regs->emem_bank_swizzle_cfg2, param->McEmemAdrCfgBankMask2);
	write32(&regs->emem_bank_swizzle_cfg3,
		param->McEmemAdrCfgBankSwizzle3);

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

	/* Program the memory arbiter */
	write32(&regs->emem_arb_cfg, param->McEmemArbCfg);
	write32(&regs->emem_arb_outstanding_req,
		param->McEmemArbOutstandingReq);
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
	write32(&regs->emem_arb_timing_r2w, param->McEmemArbTimingR2W);
	write32(&regs->emem_arb_timing_w2r, param->McEmemArbTimingW2R);
	write32(&regs->emem_arb_da_turns, param->McEmemArbDaTurns);
	write32(&regs->emem_arb_da_covers, param->McEmemArbDaCovers);
	write32(&regs->emem_arb_misc0, param->McEmemArbMisc0);
	write32(&regs->emem_arb_misc1, param->McEmemArbMisc1);
	write32(&regs->emem_arb_ring1_throttle, param->McEmemArbRing1Throttle);
	write32(&regs->emem_arb_override, param->McEmemArbOverride);
	write32(&regs->emem_arb_override_1, param->McEmemArbOverride1);
	write32(&regs->emem_arb_rsv, param->McEmemArbRsv);

	/* Program extra snap levels for display client */
	write32(&regs->dis_extra_snap_levels, param->McDisExtraSnapLevels);

	/* Trigger MC timing update */
	write32(&regs->timing_control, MC_TIMING_CONTROL_TIMING_UPDATE);

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
	write32(&regs->auto_cal_interval, param->EmcAutoCalInterval);
	write32(&regs->auto_cal_config2, param->EmcAutoCalConfig2);
	write32(&regs->auto_cal_config3, param->EmcAutoCalConfig3);
	write32(&regs->auto_cal_config, param->EmcAutoCalConfig);
	udelay(param->EmcAutoCalWait);
}

static void sdram_set_emc_timing(const struct sdram_params *param,
				 struct tegra_emc_regs *regs)
{
	/* Program EMC timing configuration */
	write32(&regs->cfg_2, param->EmcCfg2);
	write32(&regs->cfg_pipe, param->EmcCfgPipe);
	write32(&regs->dbg, param->EmcDbg);
	write32(&regs->cmdq, param->EmcCmdQ);
	write32(&regs->mc2emcq, param->EmcMc2EmcQ);
	write32(&regs->mrs_wait_cnt, param->EmcMrsWaitCnt);
	write32(&regs->mrs_wait_cnt2, param->EmcMrsWaitCnt2);
	write32(&regs->fbio_cfg5, param->EmcFbioCfg5);
	write32(&regs->rc, param->EmcRc);
	write32(&regs->rfc, param->EmcRfc);
	write32(&regs->rfc_slr, param->EmcRfcSlr);
	write32(&regs->ras, param->EmcRas);
	write32(&regs->rp, param->EmcRp);
	write32(&regs->r2r, param->EmcR2r);
	write32(&regs->w2w, param->EmcW2w);
	write32(&regs->r2w, param->EmcR2w);
	write32(&regs->w2r, param->EmcW2r);
	write32(&regs->r2p, param->EmcR2p);
	write32(&regs->w2p, param->EmcW2p);
	write32(&regs->rd_rcd, param->EmcRdRcd);
	write32(&regs->wr_rcd, param->EmcWrRcd);
	write32(&regs->rrd, param->EmcRrd);
	write32(&regs->rext, param->EmcRext);
	write32(&regs->wext, param->EmcWext);
	write32(&regs->wdv, param->EmcWdv);
	write32(&regs->wdv_mask, param->EmcWdvMask);
	write32(&regs->quse, param->EmcQUse);
	write32(&regs->quse_width, param->EmcQuseWidth);
	write32(&regs->ibdly, param->EmcIbdly);
	write32(&regs->einput, param->EmcEInput);
	write32(&regs->einput_duration, param->EmcEInputDuration);
	write32(&regs->puterm_extra, param->EmcPutermExtra);
	write32(&regs->puterm_width, param->EmcPutermWidth);
	write32(&regs->puterm_adj, param->EmcPutermAdj);
	write32(&regs->cdb_cntl_1, param->EmcCdbCntl1);
	write32(&regs->cdb_cntl_2, param->EmcCdbCntl2);
	write32(&regs->cdb_cntl_3, param->EmcCdbCntl3);
	write32(&regs->qrst, param->EmcQRst);
	write32(&regs->qsafe, param->EmcQSafe);
	write32(&regs->rdv, param->EmcRdv);
	write32(&regs->rdv_mask, param->EmcRdvMask);
	write32(&regs->qpop, param->EmcQpop);
	write32(&regs->ctt, param->EmcCtt);
	write32(&regs->ctt_duration, param->EmcCttDuration);
	write32(&regs->refresh, param->EmcRefresh);
	write32(&regs->burst_refresh_num, param->EmcBurstRefreshNum);
	write32(&regs->pre_refresh_req_cnt, param->EmcPreRefreshReqCnt);
	write32(&regs->pdex2wr, param->EmcPdEx2Wr);
	write32(&regs->pdex2rd, param->EmcPdEx2Rd);
	write32(&regs->pchg2pden, param->EmcPChg2Pden);
	write32(&regs->act2pden, param->EmcAct2Pden);
	write32(&regs->ar2pden, param->EmcAr2Pden);
	write32(&regs->rw2pden, param->EmcRw2Pden);
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
	write32(&regs->odt_read, param->EmcOdtRead);
	write32(&regs->fbio_cfg6, param->EmcFbioCfg6);
	write32(&regs->cfg_dig_dll, param->EmcCfgDigDll);
	write32(&regs->cfg_dig_dll_period, param->EmcCfgDigDllPeriod);

	/* Don't write bit 1: addr swizzle lock bit. Written at end of sequence. */
	write32(&regs->fbio_spare, param->EmcFbioSpare & 0xfffffffd);

	write32(&regs->cfg_rsv, param->EmcCfgRsv);
	write32(&regs->dll_xform_dqs0, param->EmcDllXformDqs0);
	write32(&regs->dll_xform_dqs1, param->EmcDllXformDqs1);
	write32(&regs->dll_xform_dqs2, param->EmcDllXformDqs2);
	write32(&regs->dll_xform_dqs3, param->EmcDllXformDqs3);
	write32(&regs->dll_xform_dqs4, param->EmcDllXformDqs4);
	write32(&regs->dll_xform_dqs5, param->EmcDllXformDqs5);
	write32(&regs->dll_xform_dqs6, param->EmcDllXformDqs6);
	write32(&regs->dll_xform_dqs7, param->EmcDllXformDqs7);
	write32(&regs->dll_xform_dqs8, param->EmcDllXformDqs8);
	write32(&regs->dll_xform_dqs9, param->EmcDllXformDqs9);
	write32(&regs->dll_xform_dqs10, param->EmcDllXformDqs10);
	write32(&regs->dll_xform_dqs11, param->EmcDllXformDqs11);
	write32(&regs->dll_xform_dqs12, param->EmcDllXformDqs12);
	write32(&regs->dll_xform_dqs13, param->EmcDllXformDqs13);
	write32(&regs->dll_xform_dqs14, param->EmcDllXformDqs14);
	write32(&regs->dll_xform_dqs15, param->EmcDllXformDqs15);
	write32(&regs->dll_xform_quse0, param->EmcDllXformQUse0);
	write32(&regs->dll_xform_quse1, param->EmcDllXformQUse1);
	write32(&regs->dll_xform_quse2, param->EmcDllXformQUse2);
	write32(&regs->dll_xform_quse3, param->EmcDllXformQUse3);
	write32(&regs->dll_xform_quse4, param->EmcDllXformQUse4);
	write32(&regs->dll_xform_quse5, param->EmcDllXformQUse5);
	write32(&regs->dll_xform_quse6, param->EmcDllXformQUse6);
	write32(&regs->dll_xform_quse7, param->EmcDllXformQUse7);
	write32(&regs->dll_xform_quse8, param->EmcDllXformQUse8);
	write32(&regs->dll_xform_quse9, param->EmcDllXformQUse9);
	write32(&regs->dll_xform_quse10, param->EmcDllXformQUse10);
	write32(&regs->dll_xform_quse11, param->EmcDllXformQUse11);
	write32(&regs->dll_xform_quse12, param->EmcDllXformQUse12);
	write32(&regs->dll_xform_quse13, param->EmcDllXformQUse13);
	write32(&regs->dll_xform_quse14, param->EmcDllXformQUse14);
	write32(&regs->dll_xform_quse15, param->EmcDllXformQUse15);
	write32(&regs->dll_xform_dq0, param->EmcDllXformDq0);
	write32(&regs->dll_xform_dq1, param->EmcDllXformDq1);
	write32(&regs->dll_xform_dq2, param->EmcDllXformDq2);
	write32(&regs->dll_xform_dq3, param->EmcDllXformDq3);
	write32(&regs->dll_xform_dq4, param->EmcDllXformDq4);
	write32(&regs->dll_xform_dq5, param->EmcDllXformDq5);
	write32(&regs->dll_xform_dq6, param->EmcDllXformDq6);
	write32(&regs->dll_xform_dq7, param->EmcDllXformDq7);
	write32(&regs->dll_xform_addr0, param->EmcDllXformAddr0);
	write32(&regs->dll_xform_addr1, param->EmcDllXformAddr1);
	write32(&regs->dll_xform_addr2, param->EmcDllXformAddr2);
	write32(&regs->dll_xform_addr3, param->EmcDllXformAddr3);
	write32(&regs->dll_xform_addr4, param->EmcDllXformAddr4);
	write32(&regs->dll_xform_addr5, param->EmcDllXformAddr5);
	write32(&regs->acpd_control, param->EmcAcpdControl);
	write32(&regs->dsr_vttgen_drv, param->EmcDsrVttgenDrv);
	write32(&regs->txdsrvttgen, param->EmcTxdsrvttgen);
	write32(&regs->bgbias_ctl0, param->EmcBgbiasCtl0);

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
		write32(&regs->timing_control, 1);
	}
}

static void sdram_set_dpd3(const struct sdram_params *param,
			   struct tegra_pmc_regs *regs)
{
	/* Program DPD request */
	write32(&regs->io_dpd3_req, param->PmcIoDpd3Req);
	udelay(param->PmcIoDpd3ReqWait);
}

static void sdram_set_dli_trims(const struct sdram_params *param,
				struct tegra_emc_regs *regs)
{
	/* Program DLI trims */
	write32(&regs->dli_trim_txdqs0, param->EmcDliTrimTxDqs0);
	write32(&regs->dli_trim_txdqs1, param->EmcDliTrimTxDqs1);
	write32(&regs->dli_trim_txdqs2, param->EmcDliTrimTxDqs2);
	write32(&regs->dli_trim_txdqs3, param->EmcDliTrimTxDqs3);
	write32(&regs->dli_trim_txdqs4, param->EmcDliTrimTxDqs4);
	write32(&regs->dli_trim_txdqs5, param->EmcDliTrimTxDqs5);
	write32(&regs->dli_trim_txdqs6, param->EmcDliTrimTxDqs6);
	write32(&regs->dli_trim_txdqs7, param->EmcDliTrimTxDqs7);
	write32(&regs->dli_trim_txdqs8, param->EmcDliTrimTxDqs8);
	write32(&regs->dli_trim_txdqs9, param->EmcDliTrimTxDqs9);
	write32(&regs->dli_trim_txdqs10, param->EmcDliTrimTxDqs10);
	write32(&regs->dli_trim_txdqs11, param->EmcDliTrimTxDqs11);
	write32(&regs->dli_trim_txdqs12, param->EmcDliTrimTxDqs12);
	write32(&regs->dli_trim_txdqs13, param->EmcDliTrimTxDqs13);
	write32(&regs->dli_trim_txdqs14, param->EmcDliTrimTxDqs14);
	write32(&regs->dli_trim_txdqs15, param->EmcDliTrimTxDqs15);

	write32(&regs->ca_training_timing_cntl1,
		param->EmcCaTrainingTimingCntl1);
	write32(&regs->ca_training_timing_cntl2,
		param->EmcCaTrainingTimingCntl2);

	sdram_trigger_emc_timing_update(regs);
	udelay(param->EmcTimingControlWait);
}

static void sdram_set_clock_enable_signal(const struct sdram_params *param,
					  struct tegra_emc_regs *regs)
{
	volatile uint32_t dummy = 0;
	clrbits32(&regs->pin, (EMC_PIN_RESET_MASK | EMC_PIN_DQM_MASK |
			       EMC_PIN_CKE_MASK));
	/*
	 * Assert dummy read of PIN register to ensure above write to PIN
	 * register went through. 200 is the recommended value by NVIDIA.
	 */
	dummy |= read32(&regs->pin);
	udelay(200 + param->EmcPinExtraWait);

	/* Deassert reset */
	setbits32(&regs->pin, EMC_PIN_RESET_INACTIVE);
	/*
	 * Assert dummy read of PIN register to ensure above write to PIN
	 * register went through. 200 is the recommended value by NVIDIA.
	 */
	dummy |= read32(&regs->pin);
	udelay(500 + param->EmcPinExtraWait);

	/* Enable clock enable signal */
	setbits32(&regs->pin, EMC_PIN_CKE_NORMAL);
	/*
	 * Assert dummy read of PIN register to ensure above write to PIN
	 * register went through. 200 is the recommended value by NVIDIA.
	 */
	dummy |= read32(&regs->pin);
	udelay(param->EmcPinProgramWait);

	if (!dummy) {
		die("Failed to program EMC pin.");
	}

	/* Send NOP (trigger) */
	writebits(((1 << EMC_NOP_NOP_CMD_SHIFT) |
		   (param->EmcDevSelect << EMC_NOP_NOP_DEV_SELECTN_SHIFT)),
		  &regs->nop,
		  EMC_NOP_NOP_CMD_MASK | EMC_NOP_NOP_DEV_SELECTN_MASK);

	/* Write mode registers */
	write32(&regs->emrs2, param->EmcEmrs2);
	write32(&regs->emrs3, param->EmcEmrs3);
	write32(&regs->emrs, param->EmcEmrs);
	write32(&regs->mrs, param->EmcMrs);

	if (param->EmcExtraModeRegWriteEnable) {
		write32(&regs->mrs, param->EmcMrwExtra);
	}
}

static void sdram_init_zq_calibration(const struct sdram_params *param,
				      struct tegra_emc_regs *regs)
{
	if ((param->EmcZcalWarmColdBootEnables &
	     EMC_ZCAL_WARM_COLD_BOOT_ENABLES_COLDBOOT_MASK) == 1) {
		/* Need to initialize ZCAL on coldboot. */
		write32(&regs->zq_cal, param->EmcZcalInitDev0);
		udelay(param->EmcZcalInitWait);

		if ((param->EmcDevSelect & 2) == 0) {
			write32(&regs->zq_cal, param->EmcZcalInitDev1);
			udelay(param->EmcZcalInitWait);
		}
	} else {
		udelay(param->EmcZcalInitWait);
	}
}

static void sdram_set_zq_calibration(const struct sdram_params *param,
				     struct tegra_emc_regs *regs)
{
	/* Start periodic ZQ calibration */
	write32(&regs->zcal_interval, param->EmcZcalInterval);
	write32(&regs->zcal_wait_cnt, param->EmcZcalWaitCnt);
	write32(&regs->zcal_mrw_cmd, param->EmcZcalMrwCmd);
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

	write32(&regs->dyn_self_ref_control, param->EmcDynSelfRefControl);
	write32(&regs->cfg, param->EmcCfg);
	write32(&regs->sel_dpd_ctrl, param->EmcSelDpdCtrl);

	/* Write addr swizzle lock bit */
	write32(&regs->fbio_spare, param->EmcFbioSpare);

	/* Re-trigger timing to latch power saving functions */
	sdram_trigger_emc_timing_update(regs);
}

static void sdram_enable_arbiter(const struct sdram_params *param)
{
	/* TODO(hungte) Move values here to standalone header file. */
	uint32_t *AHB_ARBITRATION_XBAR_CTRL = (uint32_t*)(0x6000c000 + 0xe0);
	setbits32(AHB_ARBITRATION_XBAR_CTRL,
		  param->AhbArbitrationXbarCtrlMemInitDone << 16);
}

static void sdram_lock_carveouts(const struct sdram_params *param,
				 struct tegra_mc_regs *regs)
{
	/* Lock carveouts, and emem_cfg registers */
	write32(&regs->video_protect_reg_ctrl,
		param->McVideoProtectWriteAccess);
	write32(&regs->emem_cfg_access_ctrl,
		MC_EMEM_CFG_ACCESS_CTRL_WRITE_ACCESS_DISABLED);
	write32(&regs->sec_carveout_reg_ctrl,
		param->McSecCarveoutProtectWriteAccess);
	write32(&regs->mts_carveout_reg_ctrl, param->McMtsCarveoutRegCtrl);
}

void sdram_init(const struct sdram_params *param)
{
	struct tegra_pmc_regs *pmc = (struct tegra_pmc_regs*)TEGRA_PMC_BASE;
	struct tegra_mc_regs *mc = (struct tegra_mc_regs*)TEGRA_MC_BASE;
	struct tegra_emc_regs *emc = (struct tegra_emc_regs*)TEGRA_EMC_BASE;

	printk(BIOS_DEBUG, "Initializing SDRAM of type %d with %dKHz\n",
		param->MemoryType, clock_get_pll_input_khz() *
		param->PllMFeedbackDivider / param->PllMInputDivider /
		(1 + param->PllMSelectDiv2));
	if (param->MemoryType != NvBootMemoryType_Ddr3)
		die("Unsupported memory type!\n");

	sdram_configure_pmc(param, pmc);
	sdram_patch(param->EmcBctSpare0, param->EmcBctSpare1);

	sdram_start_clocks(param);
	sdram_patch(param->EmcBctSpare2, param->EmcBctSpare3);

	sdram_deassert_sel_dpd(param, pmc);
	sdram_set_swizzle(param, emc);
	sdram_set_pad_controls(param, emc);
	sdram_patch(param->EmcBctSpare4, param->EmcBctSpare5);

	sdram_trigger_emc_timing_update(emc);
	sdram_init_mc(param, mc);
	sdram_init_emc(param, emc);
	sdram_patch(param->EmcBctSpare6, param->EmcBctSpare7);

	sdram_set_emc_timing(param, emc);
	sdram_patch_bootrom(param, mc);
	sdram_set_dpd3(param, pmc);
	sdram_set_dli_trims(param, emc);
	sdram_deassert_clock_enable_signal(param, pmc);
	sdram_set_clock_enable_signal(param, emc);
	sdram_init_zq_calibration(param, emc);
	sdram_patch(param->EmcBctSpare8, param->EmcBctSpare9);

	sdram_set_zq_calibration(param, emc);
	sdram_patch(param->EmcBctSpare10, param->EmcBctSpare11);

	sdram_trigger_emc_timing_update(emc);
	sdram_set_refresh(param, emc);
	sdram_enable_arbiter(param);
	sdram_lock_carveouts(param, mc);

	sdram_lp0_save_params(param);
}

uint32_t sdram_get_ram_code(void)
{
	struct tegra_pmc_regs *pmc = (struct tegra_pmc_regs*)TEGRA_PMC_BASE;
	return ((read32(&pmc->strapping_opt_a) &
		 PMC_STRAPPING_OPT_A_RAM_CODE_MASK) >>
		PMC_STRAPPING_OPT_A_RAM_CODE_SHIFT);
}

/* returns total amount of DRAM (in MB) from memory controller registers */
int sdram_size_mb(void)
{
	struct tegra_mc_regs *mc = (struct tegra_mc_regs *)TEGRA_MC_BASE;
	static int total_size = 0;

	if (total_size)
		return total_size;

	/*
	 * This obtains memory size from the External Memory Aperture
	 * Configuration register. Nvidia confirmed that it is safe to assume
	 * this value represents the total physical DRAM size.
	 */
	total_size = (read32(&mc->emem_cfg) >>
			MC_EMEM_CFG_SIZE_MB_SHIFT) & MC_EMEM_CFG_SIZE_MB_MASK;

	printk(BIOS_DEBUG, "%s: Total SDRAM (MB): %u\n", __func__, total_size);
	return total_size;
}

uintptr_t sdram_max_addressable_mb(void)
{
	return MIN(((uintptr_t)_dram/MiB) + sdram_size_mb(), 4096);
}
