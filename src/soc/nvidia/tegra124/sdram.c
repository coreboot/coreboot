/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/io.h>
#include <console/console.h>
#include <delay.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <stdlib.h>
#include <symbols.h>

#include "emc.h"
#include "mc.h"
#include "pmc.h"
#include "sdram.h"

static void sdram_patch(uintptr_t addr, uint32_t value)
{
	if (addr)
		writel(value, (uint32_t*)addr);
}

static void writebits(uint32_t value, uint32_t *addr, uint32_t mask)
{
	clrsetbits_le32(addr, mask, (value & mask));
}

/* PMC must be configured before clock-enable and de-reset of MC/EMC. */
static void sdram_configure_pmc(const struct sdram_params *param,
				struct tegra_pmc_regs *regs)
{
	/* VDDP Select */
	writel(param->PmcVddpSel, &regs->vddp_sel);
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

	writel(param->PmcRegShort, &regs->reg_short);
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
	clrbits_le32(&regs->por_dpd_ctrl,
		     PMC_POR_DPD_CTRL_MEM0_HOLD_CKE_LOW_OVR_MASK);
	udelay(param->PmcPorDpdCtrlWait);
}

static void sdram_deassert_sel_dpd(const struct sdram_params *param,
				   struct tegra_pmc_regs *regs)
{
	clrbits_le32(&regs->por_dpd_ctrl,
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
	writel(param->EmcSwizzleRank0ByteCfg, &regs->swizzle_rank0_byte_cfg);
	writel(param->EmcSwizzleRank0Byte0, &regs->swizzle_rank0_byte0);
	writel(param->EmcSwizzleRank0Byte1, &regs->swizzle_rank0_byte1);
	writel(param->EmcSwizzleRank0Byte2, &regs->swizzle_rank0_byte2);
	writel(param->EmcSwizzleRank0Byte3, &regs->swizzle_rank0_byte3);
	writel(param->EmcSwizzleRank1ByteCfg, &regs->swizzle_rank1_byte_cfg);
	writel(param->EmcSwizzleRank1Byte0, &regs->swizzle_rank1_byte0);
	writel(param->EmcSwizzleRank1Byte1, &regs->swizzle_rank1_byte1);
	writel(param->EmcSwizzleRank1Byte2, &regs->swizzle_rank1_byte2);
	writel(param->EmcSwizzleRank1Byte3, &regs->swizzle_rank1_byte3);
}

static void sdram_set_pad_controls(const struct sdram_params *param,
				   struct tegra_emc_regs *regs)
{
	/* Program the pad controls */
	writel(param->EmcXm2CmdPadCtrl, &regs->xm2cmdpadctrl);
	writel(param->EmcXm2CmdPadCtrl2, &regs->xm2cmdpadctrl2);
	writel(param->EmcXm2CmdPadCtrl3, &regs->xm2cmdpadctrl3);
	writel(param->EmcXm2CmdPadCtrl4, &regs->xm2cmdpadctrl4);
	writel(param->EmcXm2CmdPadCtrl5, &regs->xm2cmdpadctrl5);

	writel(param->EmcXm2DqsPadCtrl, &regs->xm2dqspadctrl);
	writel(param->EmcXm2DqsPadCtrl2, &regs->xm2dqspadctrl2);
	writel(param->EmcXm2DqsPadCtrl3, &regs->xm2dqspadctrl3);
	writel(param->EmcXm2DqsPadCtrl4, &regs->xm2dqspadctrl4);
	writel(param->EmcXm2DqsPadCtrl5, &regs->xm2dqspadctrl5);
	writel(param->EmcXm2DqsPadCtrl6, &regs->xm2dqspadctrl6);

	writel(param->EmcXm2DqPadCtrl, &regs->xm2dqpadctrl);
	writel(param->EmcXm2DqPadCtrl2, &regs->xm2dqpadctrl2);
	writel(param->EmcXm2DqPadCtrl3, &regs->xm2dqpadctrl3);

	writel(param->EmcXm2ClkPadCtrl, &regs->xm2clkpadctrl);
	writel(param->EmcXm2ClkPadCtrl2, &regs->xm2clkpadctrl2);

	writel(param->EmcXm2CompPadCtrl, &regs->xm2comppadctrl);

	writel(param->EmcXm2VttGenPadCtrl, &regs->xm2vttgenpadctrl);
	writel(param->EmcXm2VttGenPadCtrl2, &regs->xm2vttgenpadctrl2);
	writel(param->EmcXm2VttGenPadCtrl3, &regs->xm2vttgenpadctrl3);

	writel(param->EmcCttTermCtrl, &regs->ctt_term_ctrl);
}

static void sdram_trigger_emc_timing_update(struct tegra_emc_regs *regs)
{
	writel(EMC_TIMING_CONTROL_TIMING_UPDATE, &regs->timing_control);
}

static void sdram_init_mc(const struct sdram_params *param,
			  struct tegra_mc_regs *regs)
{
	/* Initialize MC VPR settings */
	writel(param->McDisplaySnapRing, &regs->display_snap_ring);
	writel(param->McVideoProtectBom, &regs->video_protect_bom);
	writel(param->McVideoProtectBomAdrHi, &regs->video_protect_bom_adr_hi);
	writel(param->McVideoProtectSizeMb, &regs->video_protect_size_mb);
	writel(param->McVideoProtectVprOverride,
	       &regs->video_protect_vpr_override);
	writel(param->McVideoProtectVprOverride1,
	       &regs->video_protect_vpr_override1);
	writel(param->McVideoProtectGpuOverride0,
	       &regs->video_protect_gpu_override_0);
	writel(param->McVideoProtectGpuOverride1,
	       &regs->video_protect_gpu_override_1);

	/* Program SDRAM geometry paarameters */
	writel(param->McEmemAdrCfg, &regs->emem_adr_cfg);
	writel(param->McEmemAdrCfgDev0, &regs->emem_adr_cfg_dev0);
	writel(param->McEmemAdrCfgDev1, &regs->emem_adr_cfg_dev1);

	/* Program bank swizzling */
	writel(param->McEmemAdrCfgBankMask0, &regs->emem_bank_swizzle_cfg0);
	writel(param->McEmemAdrCfgBankMask1, &regs->emem_bank_swizzle_cfg1);
	writel(param->McEmemAdrCfgBankMask2, &regs->emem_bank_swizzle_cfg2);
	writel(param->McEmemAdrCfgBankSwizzle3, &regs->emem_bank_swizzle_cfg3);

	/* Program external memory aperature (base and size) */
	writel(param->McEmemCfg, &regs->emem_cfg);

	/* Program SEC carveout (base and size) */
	writel(param->McSecCarveoutBom, &regs->sec_carveout_bom);
	writel(param->McSecCarveoutAdrHi, &regs->sec_carveout_adr_hi);
	writel(param->McSecCarveoutSizeMb, &regs->sec_carveout_size_mb);

	/* Program MTS carveout (base and size) */
	writel(param->McMtsCarveoutBom, &regs->mts_carveout_bom);
	writel(param->McMtsCarveoutAdrHi, &regs->mts_carveout_adr_hi);
	writel(param->McMtsCarveoutSizeMb, &regs->mts_carveout_size_mb);

	/* Program the memory arbiter */
	writel(param->McEmemArbCfg, &regs->emem_arb_cfg);
	writel(param->McEmemArbOutstandingReq, &regs->emem_arb_outstanding_req);
	writel(param->McEmemArbTimingRcd, &regs->emem_arb_timing_rcd);
	writel(param->McEmemArbTimingRp, &regs->emem_arb_timing_rp);
	writel(param->McEmemArbTimingRc, &regs->emem_arb_timing_rc);
	writel(param->McEmemArbTimingRas, &regs->emem_arb_timing_ras);
	writel(param->McEmemArbTimingFaw, &regs->emem_arb_timing_faw);
	writel(param->McEmemArbTimingRrd, &regs->emem_arb_timing_rrd);
	writel(param->McEmemArbTimingRap2Pre, &regs->emem_arb_timing_rap2pre);
	writel(param->McEmemArbTimingWap2Pre, &regs->emem_arb_timing_wap2pre);
	writel(param->McEmemArbTimingR2R, &regs->emem_arb_timing_r2r);
	writel(param->McEmemArbTimingW2W, &regs->emem_arb_timing_w2w);
	writel(param->McEmemArbTimingR2W, &regs->emem_arb_timing_r2w);
	writel(param->McEmemArbTimingW2R, &regs->emem_arb_timing_w2r);
	writel(param->McEmemArbDaTurns, &regs->emem_arb_da_turns);
	writel(param->McEmemArbDaCovers, &regs->emem_arb_da_covers);
	writel(param->McEmemArbMisc0, &regs->emem_arb_misc0);
	writel(param->McEmemArbMisc1, &regs->emem_arb_misc1);
	writel(param->McEmemArbRing1Throttle, &regs->emem_arb_ring1_throttle);
	writel(param->McEmemArbOverride, &regs->emem_arb_override);
	writel(param->McEmemArbOverride1, &regs->emem_arb_override_1);
	writel(param->McEmemArbRsv, &regs->emem_arb_rsv);

	/* Program extra snap levels for display client */
	writel(param->McDisExtraSnapLevels, &regs->dis_extra_snap_levels);

	/* Trigger MC timing update */
	writel(MC_TIMING_CONTROL_TIMING_UPDATE, &regs->timing_control);

	/* Program second-level clock enable overrides */
	writel(param->McClkenOverride, &regs->clken_override);

	/* Program statistics gathering */
	writel(param->McStatControl, &regs->stat_control);
}

static void sdram_init_emc(const struct sdram_params *param,
			   struct tegra_emc_regs *regs)
{
	/* Program SDRAM geometry parameters */
	writel(param->EmcAdrCfg, &regs->adr_cfg);

	/* Program second-level clock enable overrides */
	writel(param->EmcClkenOverride, &regs->clken_override);

	/* Program EMC pad auto calibration */
	writel(param->EmcAutoCalInterval, &regs->auto_cal_interval);
	writel(param->EmcAutoCalConfig2, &regs->auto_cal_config2);
	writel(param->EmcAutoCalConfig3, &regs->auto_cal_config3);
	writel(param->EmcAutoCalConfig, &regs->auto_cal_config);
	udelay(param->EmcAutoCalWait);
}

static void sdram_set_emc_timing(const struct sdram_params *param,
				 struct tegra_emc_regs *regs)
{
	/* Program EMC timing configuration */
	writel(param->EmcCfg2, &regs->cfg_2);
	writel(param->EmcCfgPipe, &regs->cfg_pipe);
	writel(param->EmcDbg, &regs->dbg);
	writel(param->EmcCmdQ, &regs->cmdq);
	writel(param->EmcMc2EmcQ, &regs->mc2emcq);
	writel(param->EmcMrsWaitCnt, &regs->mrs_wait_cnt);
	writel(param->EmcMrsWaitCnt2, &regs->mrs_wait_cnt2);
	writel(param->EmcFbioCfg5, &regs->fbio_cfg5);
	writel(param->EmcRc, &regs->rc);
	writel(param->EmcRfc, &regs->rfc);
	writel(param->EmcRfcSlr, &regs->rfc_slr);
	writel(param->EmcRas, &regs->ras);
	writel(param->EmcRp, &regs->rp);
	writel(param->EmcR2r, &regs->r2r);
	writel(param->EmcW2w, &regs->w2w);
	writel(param->EmcR2w, &regs->r2w);
	writel(param->EmcW2r, &regs->w2r);
	writel(param->EmcR2p, &regs->r2p);
	writel(param->EmcW2p, &regs->w2p);
	writel(param->EmcRdRcd, &regs->rd_rcd);
	writel(param->EmcWrRcd, &regs->wr_rcd);
	writel(param->EmcRrd, &regs->rrd);
	writel(param->EmcRext, &regs->rext);
	writel(param->EmcWext, &regs->wext);
	writel(param->EmcWdv, &regs->wdv);
	writel(param->EmcWdvMask, &regs->wdv_mask);
	writel(param->EmcQUse, &regs->quse);
	writel(param->EmcQuseWidth, &regs->quse_width);
	writel(param->EmcIbdly, &regs->ibdly);
	writel(param->EmcEInput, &regs->einput);
	writel(param->EmcEInputDuration, &regs->einput_duration);
	writel(param->EmcPutermExtra, &regs->puterm_extra);
	writel(param->EmcPutermWidth, &regs->puterm_width);
	writel(param->EmcPutermAdj, &regs->puterm_adj);
	writel(param->EmcCdbCntl1, &regs->cdb_cntl_1);
	writel(param->EmcCdbCntl2, &regs->cdb_cntl_2);
	writel(param->EmcCdbCntl3, &regs->cdb_cntl_3);
	writel(param->EmcQRst, &regs->qrst);
	writel(param->EmcQSafe, &regs->qsafe);
	writel(param->EmcRdv, &regs->rdv);
	writel(param->EmcRdvMask, &regs->rdv_mask);
	writel(param->EmcQpop, &regs->qpop);
	writel(param->EmcCtt, &regs->ctt);
	writel(param->EmcCttDuration, &regs->ctt_duration);
	writel(param->EmcRefresh, &regs->refresh);
	writel(param->EmcBurstRefreshNum, &regs->burst_refresh_num);
	writel(param->EmcPreRefreshReqCnt, &regs->pre_refresh_req_cnt);
	writel(param->EmcPdEx2Wr, &regs->pdex2wr);
	writel(param->EmcPdEx2Rd, &regs->pdex2rd);
	writel(param->EmcPChg2Pden, &regs->pchg2pden);
	writel(param->EmcAct2Pden, &regs->act2pden);
	writel(param->EmcAr2Pden, &regs->ar2pden);
	writel(param->EmcRw2Pden, &regs->rw2pden);
	writel(param->EmcTxsr, &regs->txsr);
	writel(param->EmcTxsrDll, &regs->txsrdll);
	writel(param->EmcTcke, &regs->tcke);
	writel(param->EmcTckesr, &regs->tckesr);
	writel(param->EmcTpd, &regs->tpd);
	writel(param->EmcTfaw, &regs->tfaw);
	writel(param->EmcTrpab, &regs->trpab);
	writel(param->EmcTClkStable, &regs->tclkstable);
	writel(param->EmcTClkStop, &regs->tclkstop);
	writel(param->EmcTRefBw, &regs->trefbw);
	writel(param->EmcOdtWrite, &regs->odt_write);
	writel(param->EmcOdtRead, &regs->odt_read);
	writel(param->EmcFbioCfg6, &regs->fbio_cfg6);
	writel(param->EmcCfgDigDll, &regs->cfg_dig_dll);
	writel(param->EmcCfgDigDllPeriod, &regs->cfg_dig_dll_period);

	/* Don't write bit 1: addr swizzle lock bit. Written at end of sequence. */
	writel(param->EmcFbioSpare & 0xfffffffd, &regs->fbio_spare);

	writel(param->EmcCfgRsv, &regs->cfg_rsv);
	writel(param->EmcDllXformDqs0, &regs->dll_xform_dqs0);
	writel(param->EmcDllXformDqs1, &regs->dll_xform_dqs1);
	writel(param->EmcDllXformDqs2, &regs->dll_xform_dqs2);
	writel(param->EmcDllXformDqs3, &regs->dll_xform_dqs3);
	writel(param->EmcDllXformDqs4, &regs->dll_xform_dqs4);
	writel(param->EmcDllXformDqs5, &regs->dll_xform_dqs5);
	writel(param->EmcDllXformDqs6, &regs->dll_xform_dqs6);
	writel(param->EmcDllXformDqs7, &regs->dll_xform_dqs7);
	writel(param->EmcDllXformDqs8, &regs->dll_xform_dqs8);
	writel(param->EmcDllXformDqs9, &regs->dll_xform_dqs9);
	writel(param->EmcDllXformDqs10, &regs->dll_xform_dqs10);
	writel(param->EmcDllXformDqs11, &regs->dll_xform_dqs11);
	writel(param->EmcDllXformDqs12, &regs->dll_xform_dqs12);
	writel(param->EmcDllXformDqs13, &regs->dll_xform_dqs13);
	writel(param->EmcDllXformDqs14, &regs->dll_xform_dqs14);
	writel(param->EmcDllXformDqs15, &regs->dll_xform_dqs15);
	writel(param->EmcDllXformQUse0, &regs->dll_xform_quse0);
	writel(param->EmcDllXformQUse1, &regs->dll_xform_quse1);
	writel(param->EmcDllXformQUse2, &regs->dll_xform_quse2);
	writel(param->EmcDllXformQUse3, &regs->dll_xform_quse3);
	writel(param->EmcDllXformQUse4, &regs->dll_xform_quse4);
	writel(param->EmcDllXformQUse5, &regs->dll_xform_quse5);
	writel(param->EmcDllXformQUse6, &regs->dll_xform_quse6);
	writel(param->EmcDllXformQUse7, &regs->dll_xform_quse7);
	writel(param->EmcDllXformQUse8, &regs->dll_xform_quse8);
	writel(param->EmcDllXformQUse9, &regs->dll_xform_quse9);
	writel(param->EmcDllXformQUse10, &regs->dll_xform_quse10);
	writel(param->EmcDllXformQUse11, &regs->dll_xform_quse11);
	writel(param->EmcDllXformQUse12, &regs->dll_xform_quse12);
	writel(param->EmcDllXformQUse13, &regs->dll_xform_quse13);
	writel(param->EmcDllXformQUse14, &regs->dll_xform_quse14);
	writel(param->EmcDllXformQUse15, &regs->dll_xform_quse15);
	writel(param->EmcDllXformDq0, &regs->dll_xform_dq0);
	writel(param->EmcDllXformDq1, &regs->dll_xform_dq1);
	writel(param->EmcDllXformDq2, &regs->dll_xform_dq2);
	writel(param->EmcDllXformDq3, &regs->dll_xform_dq3);
	writel(param->EmcDllXformDq4, &regs->dll_xform_dq4);
	writel(param->EmcDllXformDq5, &regs->dll_xform_dq5);
	writel(param->EmcDllXformDq6, &regs->dll_xform_dq6);
	writel(param->EmcDllXformDq7, &regs->dll_xform_dq7);
	writel(param->EmcDllXformAddr0, &regs->dll_xform_addr0);
	writel(param->EmcDllXformAddr1, &regs->dll_xform_addr1);
	writel(param->EmcDllXformAddr2, &regs->dll_xform_addr2);
	writel(param->EmcDllXformAddr3, &regs->dll_xform_addr3);
	writel(param->EmcDllXformAddr4, &regs->dll_xform_addr4);
	writel(param->EmcDllXformAddr5, &regs->dll_xform_addr5);
	writel(param->EmcAcpdControl, &regs->acpd_control);
	writel(param->EmcDsrVttgenDrv, &regs->dsr_vttgen_drv);
	writel(param->EmcTxdsrvttgen, &regs->txdsrvttgen);
	writel(param->EmcBgbiasCtl0, &regs->bgbias_ctl0);

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
		writel(param->BootRomPatchData, (uint32_t *)addr);
		writel(1, &regs->timing_control);
	}
}

static void sdram_set_dpd3(const struct sdram_params *param,
			   struct tegra_pmc_regs *regs)
{
	/* Program DPD request */
	writel(param->PmcIoDpd3Req, &regs->io_dpd3_req);
	udelay(param->PmcIoDpd3ReqWait);
}

static void sdram_set_dli_trims(const struct sdram_params *param,
				struct tegra_emc_regs *regs)
{
	/* Program DLI trims */
	writel(param->EmcDliTrimTxDqs0, &regs->dli_trim_txdqs0);
	writel(param->EmcDliTrimTxDqs1, &regs->dli_trim_txdqs1);
	writel(param->EmcDliTrimTxDqs2, &regs->dli_trim_txdqs2);
	writel(param->EmcDliTrimTxDqs3, &regs->dli_trim_txdqs3);
	writel(param->EmcDliTrimTxDqs4, &regs->dli_trim_txdqs4);
	writel(param->EmcDliTrimTxDqs5, &regs->dli_trim_txdqs5);
	writel(param->EmcDliTrimTxDqs6, &regs->dli_trim_txdqs6);
	writel(param->EmcDliTrimTxDqs7, &regs->dli_trim_txdqs7);
	writel(param->EmcDliTrimTxDqs8, &regs->dli_trim_txdqs8);
	writel(param->EmcDliTrimTxDqs9, &regs->dli_trim_txdqs9);
	writel(param->EmcDliTrimTxDqs10, &regs->dli_trim_txdqs10);
	writel(param->EmcDliTrimTxDqs11, &regs->dli_trim_txdqs11);
	writel(param->EmcDliTrimTxDqs12, &regs->dli_trim_txdqs12);
	writel(param->EmcDliTrimTxDqs13, &regs->dli_trim_txdqs13);
	writel(param->EmcDliTrimTxDqs14, &regs->dli_trim_txdqs14);
	writel(param->EmcDliTrimTxDqs15, &regs->dli_trim_txdqs15);

	writel(param->EmcCaTrainingTimingCntl1,
	       &regs->ca_training_timing_cntl1);
	writel(param->EmcCaTrainingTimingCntl2,
	       &regs->ca_training_timing_cntl2);

	sdram_trigger_emc_timing_update(regs);
	udelay(param->EmcTimingControlWait);
}

static void sdram_set_clock_enable_signal(const struct sdram_params *param,
					  struct tegra_emc_regs *regs)
{
	volatile uint32_t dummy = 0;
	clrbits_le32(&regs->pin, (EMC_PIN_RESET_MASK | EMC_PIN_DQM_MASK |
				  EMC_PIN_CKE_MASK));
	/*
	 * Assert dummy read of PIN register to ensure above write to PIN
	 * register went through. 200 is the recommended value by NVIDIA.
	 */
	dummy |= readl(&regs->pin);
	udelay(200 + param->EmcPinExtraWait);

	/* Deassert reset */
	setbits_le32(&regs->pin, EMC_PIN_RESET_INACTIVE);
	/*
	 * Assert dummy read of PIN register to ensure above write to PIN
	 * register went through. 200 is the recommended value by NVIDIA.
	 */
	dummy |= readl(&regs->pin);
	udelay(500 + param->EmcPinExtraWait);

	/* Enable clock enable signal */
	setbits_le32(&regs->pin, EMC_PIN_CKE_NORMAL);
	/*
	 * Assert dummy read of PIN register to ensure above write to PIN
	 * register went through. 200 is the recommended value by NVIDIA.
	 */
	dummy |= readl(&regs->pin);
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
	writel(param->EmcEmrs2, &regs->emrs2);
	writel(param->EmcEmrs3, &regs->emrs3);
	writel(param->EmcEmrs, &regs->emrs);
	writel(param->EmcMrs, &regs->mrs);

	if (param->EmcExtraModeRegWriteEnable) {
		writel(param->EmcMrwExtra, &regs->mrs);
	}
}

static void sdram_init_zq_calibration(const struct sdram_params *param,
				      struct tegra_emc_regs *regs)
{
	if ((param->EmcZcalWarmColdBootEnables &
	     EMC_ZCAL_WARM_COLD_BOOT_ENABLES_COLDBOOT_MASK) == 1) {
		/* Need to initialize ZCAL on coldboot. */
		writel(param->EmcZcalInitDev0, &regs->zq_cal);
		udelay(param->EmcZcalInitWait);

		if ((param->EmcDevSelect & 2) == 0) {
			writel(param->EmcZcalInitDev1, &regs->zq_cal);
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
	writel(param->EmcZcalInterval, &regs->zcal_interval);
	writel(param->EmcZcalWaitCnt, &regs->zcal_wait_cnt);
	writel(param->EmcZcalMrwCmd, &regs->zcal_mrw_cmd);
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
	writel((param->EmcDevSelect | EMC_REFCTRL_REF_VALID_ENABLED),
	       &regs->refctrl);

	writel(param->EmcDynSelfRefControl, &regs->dyn_self_ref_control);
	writel(param->EmcCfg, &regs->cfg);
	writel(param->EmcSelDpdCtrl, &regs->sel_dpd_ctrl);

	/* Write addr swizzle lock bit */
	writel(param->EmcFbioSpare, &regs->fbio_spare);

	/* Re-trigger timing to latch power saving functions */
	sdram_trigger_emc_timing_update(regs);
}

static void sdram_enable_arbiter(const struct sdram_params *param)
{
	/* TODO(hungte) Move values here to standalone header file. */
	uint32_t *AHB_ARBITRATION_XBAR_CTRL = (uint32_t*)(0x6000c000 + 0xe0);
	setbits_le32(AHB_ARBITRATION_XBAR_CTRL,
		     param->AhbArbitrationXbarCtrlMemInitDone << 16);
}

static void sdram_lock_carveouts(const struct sdram_params *param,
				 struct tegra_mc_regs *regs)
{
	/* Lock carveouts, and emem_cfg registers */
	writel(param->McVideoProtectWriteAccess, &regs->video_protect_reg_ctrl);
	writel(MC_EMEM_CFG_ACCESS_CTRL_WRITE_ACCESS_DISABLED,
	       &regs->emem_cfg_access_ctrl);
	writel(param->McSecCarveoutProtectWriteAccess,
	       &regs->sec_carveout_reg_ctrl);
	writel(param->McMtsCarveoutRegCtrl, &regs->mts_carveout_reg_ctrl);
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
	return ((readl(&pmc->strapping_opt_a) &
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
