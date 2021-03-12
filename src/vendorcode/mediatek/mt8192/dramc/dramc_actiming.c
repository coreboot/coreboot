/* SPDX-License-Identifier: BSD-3-Clause */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "dramc_common.h"
#include "x_hal_io.h"
#include "dramc_actiming.h"
#include "dramc_int_global.h"

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

//-------------------------------------------------------------------------
/** u1GetACTimingIdx()
 *	Retrieve internal ACTimingTbl's index according to dram type, freqGroup, Read DBI status
 *	@param p				Pointer of context created by DramcCtxCreate.
 *	@retval u1TimingIdx 	Return ACTimingTbl entry's index
 */
//-------------------------------------------------------------------------
static U8 u1GetACTimingIdx(DRAMC_CTX_T *p)
{
	U8 u1TimingIdx = 0xff, u1TmpIdx;
	U8 u1TmpDramType = p->dram_type;

#if (__LP5_COMBO__ == TRUE)
	if (TRUE == is_lp5_family(p))
	{
		u1TmpDramType = TYPE_LPDDR5;
	}
	else
#endif
	{
		// LP4/LP4P/LP4X use same table
		if (u1TmpDramType == TYPE_LPDDR4X || u1TmpDramType == TYPE_LPDDR4P)
			u1TmpDramType = TYPE_LPDDR4;
	}

#if (__LP5_COMBO__ == TRUE)
	if (TRUE == is_lp5_family(p))
	{
		for (u1TmpIdx = 0; u1TmpIdx < AC_TIMING_NUMBER_LP5; u1TmpIdx++)
		{
			if ((ACTimingTbl_LP5[u1TmpIdx].dramType == u1TmpDramType) &&
				/* p->frequency may not be in ACTimingTable, use p->freqGroup */
				(ACTimingTbl_LP5[u1TmpIdx].freq == p->freqGroup) &&
				(ACTimingTbl_LP5[u1TmpIdx].readDBI == p->DBI_R_onoff[p->dram_fsp]) &&
				(ACTimingTbl_LP5[u1TmpIdx].DivMode == vGet_Div_Mode(p)) && // Darren for LP4 1:4 and 1:8 mode
				(ACTimingTbl_LP5[u1TmpIdx].cbtMode == vGet_Dram_CBT_Mode(p)) //LP4 byte/mixed mode dram both use byte mode ACTiming
			   )
			{
				u1TimingIdx = u1TmpIdx;
				msg("match AC timing %d\n", u1TimingIdx);
				reg_msg("match AC timing %d\n", u1TimingIdx);
				break;
			}
		}
	}
	else
#endif
	{
		for (u1TmpIdx = 0; u1TmpIdx < AC_TIMING_NUMBER_LP4; u1TmpIdx++)
		{
			if ((ACTimingTbl_LP4[u1TmpIdx].dramType == u1TmpDramType) &&
				/* p->frequency may not be in ACTimingTable, use p->freqGroup */
				(ACTimingTbl_LP4[u1TmpIdx].freq == p->freqGroup) &&
				(ACTimingTbl_LP4[u1TmpIdx].readDBI == p->DBI_R_onoff[p->dram_fsp]) &&
				(ACTimingTbl_LP4[u1TmpIdx].DivMode == vGet_Div_Mode(p)) && // Darren for LP4 1:4 and 1:8 mode
				(ACTimingTbl_LP4[u1TmpIdx].cbtMode == vGet_Dram_CBT_Mode(p)) //LP4 byte/mixed mode dram both use byte mode ACTiming
			   )
			{
				u1TimingIdx = u1TmpIdx;
				msg("match AC timing %d\n", u1TimingIdx);
				reg_msg("match AC timing %d\n", u1TimingIdx);
				msg("dramType %d, freq %d, readDBI %d, DivMode %d, cbtMode %d\n", u1TmpDramType, p->freqGroup, p->DBI_R_onoff[p->dram_fsp], vGet_Div_Mode(p), vGet_Dram_CBT_Mode(p));
				break;
			}
		}
	}

	return u1TimingIdx;
}

//-------------------------------------------------------------------------
/** UpdateACTimingReg()
 *	ACTiming related register field update
 *	@param p				Pointer of context created by DramcCtxCreate.
 *	@param	ACTbl			Pointer to correct ACTiming table struct
 *	@retval status			(DRAM_STATUS_T): DRAM_OK or DRAM_FAIL
 */
//-------------------------------------------------------------------------
#if __LP5_COMBO__
DRAM_STATUS_T DdrUpdateACTimingReg_LP5(DRAMC_CTX_T *p, const ACTime_T_LP5 *ACTbl)
{
	ACTime_T_LP5 ACTblFinal;
	U8 backup_rank = p->rank;
	DRAM_ODT_MODE_T r2w_odt_onoff = p->odt_onoff; //Variable used in step 1 (decide to use odt on or off ACTiming)
	// ACTiming regs that have ODT on/off values -> declare variables to save the wanted value
	// -> Used to retrieve correct SHU_ACTIM2_TR2W value and write into final register field
#ifdef XRTR2R_PERFORM_ENHANCE_DQSG_RX_DLY
	U8 u1RANKINCTL = 0;
#endif
	U8 RODT_TRACKING_SAVEING_MCK = 0, u1ROOT = 0, u1TXRANKINCTL = 0, u1TXDLY = 0, u1DATLAT_DSEL = 0; //Used to store tmp ACTiming values

#if SAMSUNG_LP4_NWR_WORKAROUND
	U8 u1TWTR = 0, u1TWTR_05T = 0, u1TWTR_TMP = 0;
#endif
	// ACTiming regs that aren't currently in ACTime_T struct
	U8 u1TREFBW = 0; //REFBW_FR (tREFBW) for LP3, REFBW_FR=0 & TREFBWIG=1 (by CF)
	U8 u1TFAW_05T=0, u1TRRD_05T=0;
	U16 u2XRTWTW = 0, u2XTRTRT = 0, u2XRTW2R = 0, u2XRTR2W = 0, u2TFAW = 0;
	U16 u2TRTW=0, u2TRTW_05T=0, u2TMRR2W=0, u2TRRD=0;

#if XRTRTR_NEW_CROSS_RK_MODE
	U16 u2PHSINCTL = 0;
#endif

	U32 u4RankINCTL_ROOT;

	if(ACTbl == NULL)
		return DRAM_FAIL;
	ACTblFinal = *ACTbl;

	// ----Step 1: Perform ACTiming table adjustments according to different usage/scenarios--------------------------
#if ENABLE_TX_WDQS
	r2w_odt_onoff = ODT_ON;
#else
	r2w_odt_onoff = p->odt_onoff;
#endif

	// ACTimings that have different values for odt on/off, retrieve the correct one and store in local variable
	if (r2w_odt_onoff == ODT_ON) //odt_on
	{
		u2TRTW = ACTblFinal.tr2w_odt_on;
		u2TRTW_05T = ACTblFinal.tr2w_odt_on_05T;
		u2XRTW2R = ACTblFinal.xrtw2r_odt_on_otf_off;
		u2XRTR2W = ACTblFinal.xrtr2w_odt_on;
	}
	else //odt_off
	{
		u2TRTW = ACTblFinal.tr2w_odt_off;
		u2TRTW_05T = ACTblFinal.tr2w_odt_off_05T;
		u2XRTW2R = ACTblFinal.xrtw2r_odt_off_otf_off;
		u2XRTR2W = ACTblFinal.xrtr2w_odt_off;
	}

	// Override the above tRTW & tRTW_05T selection for Hynix LPDDR4P dram (always use odt_on's value for tRTW)
	if ((p->dram_type == TYPE_LPDDR4P) && (p->vendor_id == VENDOR_HYNIX)) //!SUPPORT_HYNIX_RX_DQS_WEAK_PULL (temp solution, need to discuss with SY)
	{
		u2TRTW = ACTblFinal.tr2w_odt_on;
		u2TRTW_05T = ACTblFinal.tr2w_odt_on_05T;
	}

	if (r2w_odt_onoff == ODT_ON)
	{
		u2XTRTRT = ACTblFinal.xrtr2r_odt_on;
		u2XRTWTW = ACTblFinal.xrtw2w_odt_on;
	}
	else
	{
		u2XTRTRT = ACTblFinal.xrtr2r_odt_off;
		u2XRTWTW = ACTblFinal.xrtw2w_odt_off;
	}

#if ENABLE_RODT_TRACKING_SAVE_MCK
	// for rodt tracking save 1 MCK and rodt tracking enable or not(RODTENSTB_TRACK_EN)
	u1ODT_ON = p->odt_onoff;
	u1RODT_TRACK = ENABLE_RODT_TRACKING;
	u1ROEN = u1WDQS_ON | u1ODT_ON;
	u1ModeSel = u1RODT_TRACK & u1ROEN;

	// when WDQS on and RODT Track define open and un-term, RODT_TRACKING_SAVEING_MCK = 1 for the future setting
	// Maybe "Save 1 MCK" will be set after Vins_on project, but Bian_co & Vins_on can not.(different with performance team)
	//if (u1RODT_TRACK && (u1ROEN==1))
	//	  RODT_TRACKING_SAVEING_MCK = 1;
#endif

#if (ENABLE_RODT_TRACKING || defined(XRTR2W_PERFORM_ENHANCE_RODTEN))
	/* yr: same code
	// set to 0, let TRTW & XRTR2W setting values are the smae with DV-sim's value that DE provided
	if (r2w_odt_onoff == ODT_ON) RODT_TRACKING_SAVEING_MCK = 0; //RODT_TRACKING eanble can save r2w 1 MCK
	else RODT_TRACKING_SAVEING_MCK = 0;
	*/
	RODT_TRACKING_SAVEING_MCK = 0;
#endif

	// Update values that are used by RODT_TRACKING_SAVEING_MCK
	u2TRTW = u2TRTW - RODT_TRACKING_SAVEING_MCK;
	u2XRTR2W = u2XRTR2W - RODT_TRACKING_SAVEING_MCK;

#if SAMSUNG_LP4_NWR_WORKAROUND
	// If nWR is fixed to 30 for all freqs, tWTR@800Mhz should add 2tCK gap, allowing sufficient Samsung DRAM internal IO precharge time
	if ((p->vendor_id == VENDOR_SAMSUNG) && (p->frequency == 800)) //LP4X, Samsung, DDR1600
	{
		u1TWTR_TMP = (ACTblFinal.twtr * 4 - ACTblFinal.twtr_05T * 2) + 2; //Convert TWTR to tCK, and add 2tCK
		if ((u1TWTR_TMP % 4) == 0) //TWTR can be transferred to TWTR directly
		{
			u1TWTR = u1TWTR_TMP >> 2;
			u1TWTR_05T = 0;
		}
		else //Can't be transfered to TWTR directly
		{
			u1TWTR = (u1TWTR_TMP + 2) >> 2; //Add 2 tCK and set TWTR value (Then minus 2tCK using 05T)
			u1TWTR_05T = 1;  //05T means minus 2tCK
		}

		ACTblFinal.twtr = u1TWTR;
		ACTblFinal.twtr_05T = u1TWTR_05T;
	}
#endif

//DATLAT related
if (u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_MISC_RX_PIPE_CTRL), SHU_MISC_RX_PIPE_CTRL_RX_PIPE_BYPASS_EN))
	u1DATLAT_DSEL = ACTblFinal.datlat;
else
	u1DATLAT_DSEL = ACTblFinal.datlat - 1;

#if TX_OE_EXTEND
	u2XRTWTW += 1;
	u2XRTW2R += 1;
#endif

#if 0//(!CMD_CKE_WORKAROUND_FIX)
	U8 u1Txp = 0, u1Txp0p5 = 0;

	if (((p->frequency <= 1866) && (p->frequency >= 1600)) || ((vGet_Div_Mode(p) == DIV4_MODE) && (p->frequency == 400)))
	{
		u1Txp = 1;
	}
	else if ((p->frequency == 2133) || ((vGet_Div_Mode(p) == DIV4_MODE) && ((p->frequency <= 800) || (p->frequency >= 600))))
	{
		u1Txp = 2;
	}

	if ((p->frequency == 1866) || ((p->frequency <= 1333) && (p->frequency >= 1200)))
	{
		u1Txp0p5 = 1;
	}

	ACTblFinal.txp = u1Txp;
	ACTblFinal.txp_05T = u1Txp0p5;
	ACTblFinal.ckelckcnt = 4;
	ACTblFinal.earlyckecnt = 0;
	ACTblFinal.ckeprd -= 1;
#endif

	// ----Step 2: Perform register writes for entries in ACTblFinal struct & ACTiming excel file (all actiming adjustments should be done in Step 1)-------

	vIO32WriteFldMulti_All(DRAMC_REG_SHU_ACTIM1, P_Fld(ACTblFinal.tras, SHU_ACTIM1_TRAS)
												| P_Fld(ACTblFinal.trp, SHU_ACTIM1_TRP)
												| P_Fld(ACTblFinal.trpab, SHU_ACTIM1_TRPAB)
												| P_Fld(ACTblFinal.tmrwckel, SHU_ACTIM1_TMRWCKEL)
												| P_Fld(ACTblFinal.trc, SHU_ACTIM1_TRC));

	vIO32WriteFldMulti_All(DRAMC_REG_SHU_ACTIM3, P_Fld(ACTblFinal.trfc, SHU_ACTIM3_TRFC)
												| P_Fld(ACTblFinal.tr2mrr, SHU_ACTIM3_TR2MRR)
												| P_Fld(ACTblFinal.trfcpb, SHU_ACTIM3_TRFCPB));

	vIO32WriteFldMulti_All(DRAMC_REG_SHU_ACTIM2, P_Fld(ACTblFinal.txp, SHU_ACTIM2_TXP)
												| P_Fld(ACTblFinal.tmrri, SHU_ACTIM2_TMRRI)
												| P_Fld(ACTblFinal.tfaw, SHU_ACTIM2_TFAW)
												| P_Fld(u2TRTW, SHU_ACTIM2_TR2W) // Value has odt_on/off difference, use local variable u1TRTW
												| P_Fld(ACTblFinal.trtp, SHU_ACTIM2_TRTP));

	vIO32WriteFldMulti_All(DRAMC_REG_SHU_ACTIM0, P_Fld(ACTblFinal.trcd, SHU_ACTIM0_TRCD)
												| P_Fld(ACTblFinal.twr, SHU_ACTIM0_TWR)
												| P_Fld(ACTblFinal.trrd, SHU_ACTIM0_TRRD));

	vIO32WriteFldMulti_All(DRAMC_REG_SHU_ACTIM5, P_Fld(ACTblFinal.tpbr2pbr, SHU_ACTIM5_TPBR2PBR)
												| P_Fld(ACTblFinal.twtpd, SHU_ACTIM5_TWTPD)
												| P_Fld(ACTblFinal.tpbr2act, SHU_ACTIM5_TPBR2ACT));

	vIO32WriteFldMulti_All(DRAMC_REG_SHU_ACTIM6, P_Fld(ACTblFinal.tr2mrw, SHU_ACTIM6_TR2MRW)
												| P_Fld(ACTblFinal.tw2mrw, SHU_ACTIM6_TW2MRW)
												| P_Fld(ACTblFinal.tmrd, SHU_ACTIM6_TMRD)
												| P_Fld(ACTblFinal.zqlat2, SHU_ACTIM6_TZQLAT2)
												| P_Fld(ACTblFinal.tmrw, SHU_ACTIM6_TMRW));

	vIO32WriteFldMulti_All(DRAMC_REG_SHU_ACTIM4, P_Fld(ACTblFinal.tmrr2mrw, SHU_ACTIM4_TMRR2MRW)
												| P_Fld(ACTblFinal.tmrr2w, SHU_ACTIM4_TMRR2W)
												| P_Fld(ACTblFinal.tzqcs, SHU_ACTIM4_TZQCS)
												| P_Fld(ACTblFinal.txrefcnt, SHU_ACTIM4_TXREFCNT));

	vIO32WriteFldAlign_All(DRAMC_REG_SHU_CKECTRL, ACTblFinal.ckeprd, SHU_CKECTRL_TCKEPRD);

	vIO32WriteFldMulti_All(DRAMC_REG_SHU_ACTIM_XRT, P_Fld(u2XRTWTW, SHU_ACTIM_XRT_XRTW2W)
												| P_Fld(u2XRTW2R, SHU_ACTIM_XRT_XRTW2R)
												| P_Fld(u2XRTR2W, SHU_ACTIM_XRT_XRTR2W)
												| P_Fld(u2XTRTRT, SHU_ACTIM_XRT_XRTR2R));

	vIO32WriteFldAlign_All(DRAMC_REG_SHU_HWSET_VRCG, ACTblFinal.vrcgdis_prdcnt, SHU_HWSET_VRCG_VRCGDIS_PRDCNT);
//	  vIO32WriteFldMulti_All(DRAMC_REG_SHU_HWSET_VRCG, P_Fld(ACTblFinal.vrcgdis_prdcnt, SHU_HWSET_VRCG_VRCGDIS_PRDCNT)
//												  | P_Fld(ACTblFinal.hwset_vrcg_op, SHU_HWSET_VRCG_HWSET_VRCG_OP));
	vIO32WriteFldAlign_All(DRAMC_REG_SHU_HWSET_MR2, ACTblFinal.hwset_mr2_op, SHU_HWSET_MR2_HWSET_MR2_OP);
	vIO32WriteFldAlign_All(DRAMC_REG_SHU_HWSET_MR13, ACTblFinal.hwset_mr13_op, SHU_HWSET_MR13_HWSET_MR13_OP);

	// AC timing 0.5T
	vIO32WriteFldMulti_All(DRAMC_REG_SHU_AC_TIME_05T, P_Fld(ACTblFinal.twtr_05T, SHU_AC_TIME_05T_TWTR_M05T)
													| P_Fld(ACTblFinal.twtr_l_05T, SHU_AC_TIME_05T_BGTWTR_M05T)
													| P_Fld(u2TRTW_05T, SHU_AC_TIME_05T_TR2W_05T) // Value has odt_on/off difference, use local variable u1TRTW
													| P_Fld(ACTblFinal.twtpd_05T, SHU_AC_TIME_05T_TWTPD_M05T)
													| P_Fld(ACTblFinal.tfaw_05T, SHU_AC_TIME_05T_TFAW_05T)
													| P_Fld(ACTblFinal.trrd_05T, SHU_AC_TIME_05T_TRRD_05T)
													| P_Fld(ACTblFinal.twr_05T, SHU_AC_TIME_05T_TWR_M05T)
													| P_Fld(ACTblFinal.tras_05T, SHU_AC_TIME_05T_TRAS_05T)
													| P_Fld(ACTblFinal.trpab_05T, SHU_AC_TIME_05T_TRPAB_05T)
													| P_Fld(ACTblFinal.trp_05T, SHU_AC_TIME_05T_TRP_05T)
													| P_Fld(ACTblFinal.trcd_05T, SHU_AC_TIME_05T_TRCD_05T)
													| P_Fld(ACTblFinal.trtp_05T, SHU_AC_TIME_05T_TRTP_05T)
													| P_Fld(ACTblFinal.txp_05T, SHU_AC_TIME_05T_TXP_05T)
													| P_Fld(ACTblFinal.trfc_05T, SHU_AC_TIME_05T_TRFC_05T)
													| P_Fld(ACTblFinal.trfcpb_05T, SHU_AC_TIME_05T_TRFCPB_05T)
													| P_Fld(ACTblFinal.tpbr2pbr_05T, SHU_AC_TIME_05T_TPBR2PBR_05T)
													| P_Fld(ACTblFinal.tpbr2act_05T, SHU_AC_TIME_05T_TPBR2ACT_05T)
													| P_Fld(ACTblFinal.tr2mrw_05T, SHU_AC_TIME_05T_TR2MRW_05T)
													| P_Fld(ACTblFinal.tw2mrw_05T, SHU_AC_TIME_05T_TW2MRW_05T)
													| P_Fld(ACTblFinal.tmrr2mrw_05T, SHU_AC_TIME_05T_TMRR2MRW_05T)
													| P_Fld(ACTblFinal.tmrw_05T, SHU_AC_TIME_05T_TMRW_05T)
													| P_Fld(ACTblFinal.tmrd_05T, SHU_AC_TIME_05T_TMRD_05T)
													| P_Fld(ACTblFinal.tmrwckel_05T, SHU_AC_TIME_05T_TMRWCKEL_05T)
													| P_Fld(ACTblFinal.tmrri_05T, SHU_AC_TIME_05T_TMRRI_05T)
													| P_Fld(ACTblFinal.trc_05T, SHU_AC_TIME_05T_TRC_05T));

	{
		vIO32WriteFldAlign_All(DRAMC_REG_SHU_ACTIM0, ACTblFinal.twtr_l, SHU_ACTIM0_TWTR_L);
		vIO32WriteFldMulti_All(DRAMC_REG_SHU_ACTIM7, P_Fld(ACTblFinal.tcsh_cscal, SHU_ACTIM7_TCSH_CSCAL)
														   | P_Fld(ACTblFinal.tcacsh, SHU_ACTIM7_TCACSH));
	}
	{
		vIO32WriteFldAlign_All(DRAMC_REG_SHU_ACTIM0, ACTblFinal.twtr, SHU_ACTIM0_TWTR);
		vIO32WriteFldMulti_All(DRAMC_REG_SHU_CKECTRL, P_Fld(ACTblFinal.tpde, SHU_CKECTRL_TPDE)
														| P_Fld(ACTblFinal.tpdx, SHU_CKECTRL_TPDX)
														| P_Fld(ACTblFinal.tpde_05T, SHU_CKECTRL_TPDE_05T)
														| P_Fld(ACTblFinal.tpdx_05T, SHU_CKECTRL_TPDX_05T));
		vIO32WriteFldMulti_All(DRAMC_REG_SHU_WCKCTRL, P_Fld(ACTblFinal.wckrdoff, SHU_WCKCTRL_WCKRDOFF)
														| P_Fld(ACTblFinal.wckrdoff_05T, SHU_WCKCTRL_WCKRDOFF_05T)
														| P_Fld(ACTblFinal.wckwroff, SHU_WCKCTRL_WCKWROFF)
														| P_Fld(ACTblFinal.wckwroff_05T, SHU_WCKCTRL_WCKWROFF_05T));
		vIO32WriteFldAlign_All(DRAMC_REG_SHU_ACTIM5, ACTblFinal.trtpd, SHU_ACTIM5_TR2PD);
		vIO32WriteFldAlign_All(DRAMC_REG_SHU_AC_TIME_05T, ACTblFinal.trtpd_05T, SHU_AC_TIME_05T_TR2PD_05T);
		vIO32WriteFldAlign_All(DRAMC_REG_SHU_LP5_CMD, ACTblFinal.tcsh, SHU_LP5_CMD_TCSH);
	}

#if AC_TIMING_DERATE_ENABLE
	if (u1IsLP4Family(p->dram_type))
	{
		vIO32WriteFldMulti_All(DRAMC_REG_SHU_AC_DERATING0, P_Fld(ACTblFinal.trcd_derate, SHU_AC_DERATING0_TRCD_DERATE)
														| P_Fld(ACTblFinal.trrd_derate, SHU_AC_DERATING0_TRRD_DERATE));
		vIO32WriteFldMulti_All(DRAMC_REG_SHU_AC_DERATING1, P_Fld(ACTblFinal.trc_derate, SHU_AC_DERATING1_TRC_DERATE)
														| P_Fld(ACTblFinal.tras_derate, SHU_AC_DERATING1_TRAS_DERATE)
														| P_Fld(ACTblFinal.trp_derate, SHU_AC_DERATING1_TRP_DERATE)
														| P_Fld(ACTblFinal.trpab_derate, SHU_AC_DERATING1_TRPAB_DERATE));
		vIO32WriteFldMulti_All(DRAMC_REG_SHU_AC_DERATING_05T, P_Fld(ACTblFinal.trrd_05T_derate, SHU_AC_DERATING_05T_TRRD_05T_DERATE)
														| P_Fld(ACTblFinal.tras_05T_derate, SHU_AC_DERATING_05T_TRAS_05T_DERATE)
														| P_Fld(ACTblFinal.trpab_05T_derate, SHU_AC_DERATING_05T_TRPAB_05T_DERATE)
														| P_Fld(ACTblFinal.trp_05T_derate, SHU_AC_DERATING_05T_TRP_05T_DERATE)
														| P_Fld(ACTblFinal.trcd_05T_derate, SHU_AC_DERATING_05T_TRCD_05T_DERATE)
														| P_Fld(ACTblFinal.trc_05T_derate, SHU_AC_DERATING_05T_TRC_05T_DERATE));
		vIO32WriteFldAlign_All(DRAMC_REG_REFCTRL3, 0xc0, REFCTRL3_REF_DERATING_EN);
		vIO32WriteFldAlign_All(DRAMC_REG_SHU_AC_DERATING0, 0x1, SHU_AC_DERATING0_ACDERATEEN); //enable derating for AC timing
	}
#endif

	// DQSINCTL related
	vSetRank(p, RANK_0);
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SHU_RK_DQSCTL, ACTblFinal.dqsinctl, MISC_SHU_RK_DQSCTL_DQSINCTL);// Rank 0 DQSINCTL
	vSetRank(p, RANK_1);
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SHU_RK_DQSCTL, ACTblFinal.dqsinctl, MISC_SHU_RK_DQSCTL_DQSINCTL);// Rank 1 DQSINCTL
	vSetRank(p, backup_rank);
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SHU_ODTCTRL, ACTblFinal.dqsinctl, MISC_SHU_ODTCTRL_RODT_LAT);

	if (ACTblFinal.dqsinctl >= 2)
	{
		u4RankINCTL_ROOT = ACTblFinal.dqsinctl - 2;
	}
	else
	{
		err("u4RankINCTL_ROOT <2, Please check\n");
		u4RankINCTL_ROOT = 0;
	}

	vIO32WriteFldMulti_All(DDRPHY_REG_MISC_SHU_RANKCTL, P_Fld(ACTblFinal.dqsinctl, MISC_SHU_RANKCTL_RANKINCTL_PHY)
													   | P_Fld(u4RankINCTL_ROOT, MISC_SHU_RANKCTL_RANKINCTL_ROOT1)
													   | P_Fld(u4RankINCTL_ROOT, MISC_SHU_RANKCTL_RANKINCTL));

#if XRTRTR_NEW_CROSS_RK_MODE
	u2PHSINCTL = (ACTblFinal.dqsinctl == 0)? 0: (ACTblFinal.dqsinctl - 1);
	vIO32WriteFldAlign_All(DDRPHY_REG_SHU_MISC_RANK_SEL_STB, u2PHSINCTL, SHU_MISC_RANK_SEL_STB_RANK_SEL_PHSINCTL);
#endif

	// DATLAT related, tREFBW
	vIO32WriteFldMulti_All(DDRPHY_REG_MISC_SHU_RDAT, P_Fld(ACTblFinal.datlat, MISC_SHU_RDAT_DATLAT)
											| P_Fld(u1DATLAT_DSEL, MISC_SHU_RDAT_DATLAT_DSEL)
											| P_Fld(u1DATLAT_DSEL, MISC_SHU_RDAT_DATLAT_DSEL_PHY));

	vIO32WriteFldAlign_All(DRAMC_REG_SHU_ACTIMING_CONF, u1TREFBW, SHU_ACTIMING_CONF_REFBW_FR);

	// ----Step 3: Perform register writes/calculation for other regs (That aren't in ACTblFinal struct)------------------------------------------------
#ifdef XRTR2R_PERFORM_ENHANCE_DQSG_RX_DLY
	//Wei-Jen: Ininital setting values are the same, RANKINCTL_RXDLY = RANKINCTL = RANKINCTL_ROOT1
	//XRTR2R setting will be updated in RxdqsGatingPostProcess
	u1RANKINCTL = u4IO32ReadFldAlign(DDRPHY_REG_MISC_SHU_RANKCTL, MISC_SHU_RANKCTL_RANKINCTL);
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SHU_RANKCTL, u1RANKINCTL, MISC_SHU_RANKCTL_RANKINCTL_RXDLY);
#endif

	//Update releated RG of XRTW2W
	if (p->frequency <= 800)
	{
		if (vGet_Div_Mode(p) == DIV4_MODE)
		{
			u1ROOT = 0; u1TXRANKINCTL = 1; u1TXDLY = 2;
		}
		else
		{
			u1ROOT = 0; u1TXRANKINCTL = 0; u1TXDLY = 1;
		}
	}
	else
	{
		u1ROOT = (p->frequency == 1866)? 1: 0;
		u1TXRANKINCTL = 1; u1TXDLY = 2;
	}
	#if TX_OE_EXTEND
	if (p->frequency >= 1333)
	{
		u1TXRANKINCTL += 1;
		u1TXDLY += 1;
	}
	#endif

	vIO32WriteFldMulti_All(DRAMC_REG_SHU_TX_RANKCTL, P_Fld(u1ROOT, SHU_TX_RANKCTL_TXRANKINCTL_ROOT)
												| P_Fld(u1TXRANKINCTL, SHU_TX_RANKCTL_TXRANKINCTL)
												| P_Fld(u1TXDLY, SHU_TX_RANKCTL_TXRANKINCTL_TXDLY));

	return DRAM_OK;
}
#endif

#if ENABLE_WDQS_MODE_2
static void WDQSMode2AcTimingEnlarge(DRAMC_CTX_T *p, U16 *u2_XRTW2W, U16 *u2_XRTR2W, U16 *u2_XRTW2R, U16 *u2_TRTW)
{
	U16 u2XRTW2W_enlarge = 0, u2XRTR2W_enlarge = 0;
	U16 u2XRTW2R_enlarge = 0, u2TRTW_enlarge = 0;

	switch (p->frequency)
	{
		case 1866:
			u2XRTW2W_enlarge = 3;
			break;
		case 1600:
			u2XRTW2W_enlarge = 2;
			u2XRTR2W_enlarge = 1;
			break;
		case 1200:
			u2XRTW2W_enlarge = 2;
			if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
				u2XRTR2W_enlarge = 1;
			break;
		case 933:
			u2XRTW2W_enlarge = 1;
			if (vGet_Dram_CBT_Mode(p) == CBT_NORMAL_MODE)
				u2XRTR2W_enlarge = 1;
			break;
		case 800:
			u2XRTW2W_enlarge = 2;
			u2TRTW_enlarge = 1;
			if (vGet_Dram_CBT_Mode(p) == CBT_NORMAL_MODE)
				u2XRTW2R_enlarge = 1;
			break;
		case 600:
			u2XRTW2W_enlarge = 2;
			u2TRTW_enlarge = 1;
			if (vGet_Dram_CBT_Mode(p) == CBT_NORMAL_MODE)
				u2XRTW2R_enlarge = 1;
			break;
		case 400:
			u2XRTW2W_enlarge = 3;
			u2TRTW_enlarge = 2;
			u2XRTR2W_enlarge = 1;
			break;
		default:
			err("[WDQSMode2AcTimingEnlarge] frequency err!\n");
			#if __ETT__
			while (1);
			#endif
	}

	*u2_XRTW2W += u2XRTW2W_enlarge;
	*u2_XRTR2W += u2XRTR2W_enlarge;
	*u2_XRTW2R += u2XRTW2R_enlarge;
	*u2_TRTW += u2TRTW_enlarge;
}
#endif

static DRAM_STATUS_T DdrUpdateACTimingReg_LP4(DRAMC_CTX_T *p, const ACTime_T_LP4 *ACTbl)
{
	ACTime_T_LP4 ACTblFinal;
	U8 backup_rank = p->rank;
	DRAM_ODT_MODE_T r2w_odt_onoff = p->odt_onoff; //Variable used in step 1 (decide to use odt on or off ACTiming)
	// ACTiming regs that have ODT on/off values -> declare variables to save the wanted value
	// -> Used to retrieve correct SHU_ACTIM2_TR2W value and write into final register field
#ifdef XRTR2R_PERFORM_ENHANCE_DQSG_RX_DLY
	U8 u1RANKINCTL = 0;
#endif
	U8 RODT_TRACKING_SAVEING_MCK = 0, u1ROOT = 0, u1TXRANKINCTL = 0, u1TXDLY = 0, u1DATLAT_DSEL = 0; //Used to store tmp ACTiming values

#if SAMSUNG_LP4_NWR_WORKAROUND
	U8 u1TWTR = 0, u1TWTR_05T = 0, u1TWTR_TMP = 0;
#endif
	// ACTiming regs that aren't currently in ACTime_T struct
	U8 u1TREFBW = 0; //REFBW_FR (tREFBW) for LP3, REFBW_FR=0 & TREFBWIG=1 (by CF)
	U8 u1TFAW_05T=0, u1TRRD_05T=0;
	U16 u2XRTWTW = 0, u2XTRTRT = 0, u2XRTW2R = 0, u2XRTR2W = 0, u2TFAW = 0;
	U16 u2TRTW=0, u2TRTW_05T=0, u2TMRR2W=0, u2TRRD=0;

#if XRTRTR_NEW_CROSS_RK_MODE
	U16 u2PHSINCTL = 0;
#endif

	U32 u4RankINCTL_ROOT;

	if(ACTbl == NULL)
		return DRAM_FAIL;
	ACTblFinal = *ACTbl;

	// ----Step 1: Perform ACTiming table adjustments according to different usage/scenarios--------------------------
#if ENABLE_TX_WDQS
	r2w_odt_onoff = ODT_ON;
#else
	r2w_odt_onoff = p->odt_onoff;
#endif
	// ACTimings that have different values for odt on/off, retrieve the correct one and store in local variable
	if (r2w_odt_onoff == ODT_ON) //odt_on
	{
		u2TRTW = ACTblFinal.trtw_odt_on;
		u2TRTW_05T = ACTblFinal.trtw_odt_on_05T;
		u2XRTW2R = ACTblFinal.xrtw2r_odt_on;
		u2XRTR2W = ACTblFinal.xrtr2w_odt_on;
	}
	else //odt_off
	{
		u2TRTW = ACTblFinal.trtw_odt_off;
		u2TRTW_05T = ACTblFinal.trtw_odt_off_05T;
		u2XRTW2R = ACTblFinal.xrtw2r_odt_off;
		u2XRTR2W = ACTblFinal.xrtr2w_odt_off;
	}

	// Override the above tRTW & tRTW_05T selection for Hynix LPDDR4P dram (always use odt_on's value for tRTW)
	if ((p->dram_type == TYPE_LPDDR4P) && (p->vendor_id == VENDOR_HYNIX)) //!SUPPORT_HYNIX_RX_DQS_WEAK_PULL (temp solution, need to discuss with SY)
	{
		u2TRTW = ACTblFinal.trtw_odt_on;
		u2TRTW_05T = ACTblFinal.trtw_odt_on_05T;
	}

	{
		u2TFAW = ACTblFinal.tfaw_4266;
		u1TFAW_05T = ACTblFinal.tfaw_4266_05T;
		u2TRRD = ACTblFinal.trrd_4266;
		u1TRRD_05T = ACTblFinal.trrd_4266_05T;
	#if XRTRTR_NEW_CROSS_RK_MODE
		u2XTRTRT = ACTblFinal.xrtr2r_new_mode;
	#else
		u2XTRTRT = ACTblFinal.xrtr2r_old_mode;
	#endif

	#if XRTWTW_NEW_CROSS_RK_MODE
		u2XRTWTW = ACTblFinal.xrtw2w_new_mode;
	#else
		u2XRTWTW = ACTblFinal.xrtw2w_old_mode;
	#endif

	#if ENABLE_WDQS_MODE_2
		WDQSMode2AcTimingEnlarge(p, &u2XRTWTW, &u2XRTR2W, &u2XRTW2R, &u2TRTW);
	#endif

		if (r2w_odt_onoff == ODT_ON)
			u2TMRR2W = ACTblFinal.tmrr2w_odt_on;
		else
			u2TMRR2W = ACTblFinal.tmrr2w_odt_off;
	}

#if ENABLE_RODT_TRACKING_SAVE_MCK
	// for rodt tracking save 1 MCK and rodt tracking enable or not(RODTENSTB_TRACK_EN)
	u1ODT_ON = p->odt_onoff;
	u1RODT_TRACK = ENABLE_RODT_TRACKING;
	u1ROEN = u1WDQS_ON | u1ODT_ON;
	u1ModeSel = u1RODT_TRACK & u1ROEN;

	// when WDQS on and RODT Track define open and un-term, RODT_TRACKING_SAVEING_MCK = 1 for the future setting
	// Maybe "Save 1 MCK" will be set after Vins_on project, but Bian_co & Vins_on can not.(different with performance team)
	//if (u1RODT_TRACK && (u1ROEN==1))
	//	  RODT_TRACKING_SAVEING_MCK = 1;
#endif

#if (ENABLE_RODT_TRACKING || defined(XRTR2W_PERFORM_ENHANCE_RODTEN))
	/* yr: same code
	// set to 0, let TRTW & XRTR2W setting values are the smae with DV-sim's value that DE provided
	if (r2w_odt_onoff == ODT_ON) RODT_TRACKING_SAVEING_MCK = 0; //RODT_TRACKING eanble can save r2w 1 MCK
	else RODT_TRACKING_SAVEING_MCK = 0;
	*/
	RODT_TRACKING_SAVEING_MCK = 0;
#endif

	// Update values that are used by RODT_TRACKING_SAVEING_MCK
	u2TRTW = u2TRTW - RODT_TRACKING_SAVEING_MCK;
	u2XRTR2W = u2XRTR2W - RODT_TRACKING_SAVEING_MCK;

#if SAMSUNG_LP4_NWR_WORKAROUND
	// If nWR is fixed to 30 for all freqs, tWTR@800Mhz should add 2tCK gap, allowing sufficient Samsung DRAM internal IO precharge time
	if ((p->vendor_id == VENDOR_SAMSUNG) && (p->frequency == 800)) //LP4X, Samsung, DDR1600
	{
		u1TWTR_TMP = (ACTblFinal.twtr * 4 - ACTblFinal.twtr_05T * 2) + 2; //Convert TWTR to tCK, and add 2tCK
		if ((u1TWTR_TMP % 4) == 0) //TWTR can be transferred to TWTR directly
		{
			u1TWTR = u1TWTR_TMP >> 2;
			u1TWTR_05T = 0;
		}
		else //Can't be transfered to TWTR directly
		{
			u1TWTR = (u1TWTR_TMP + 2) >> 2; //Add 2 tCK and set TWTR value (Then minus 2tCK using 05T)
			u1TWTR_05T = 1;  //05T means minus 2tCK
		}

		ACTblFinal.twtr = u1TWTR;
		ACTblFinal.twtr_05T = u1TWTR_05T;
	}
#endif

//DATLAT related
if (u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_MISC_RX_PIPE_CTRL), SHU_MISC_RX_PIPE_CTRL_RX_PIPE_BYPASS_EN))
	u1DATLAT_DSEL = ACTblFinal.datlat;
else
	u1DATLAT_DSEL = ACTblFinal.datlat - 1;

#if TX_OE_EXTEND
	u2XRTWTW += 1;
	u2XRTW2R += 1;
#endif

#if 0//(!CMD_CKE_WORKAROUND_FIX)
	U8 u1Txp = 0, u1Txp0p5 = 0;

	if (((p->frequency <= 1866) && (p->frequency >= 1600)) || ((vGet_Div_Mode(p) == DIV4_MODE) && (p->frequency == 400)))
	{
		u1Txp = 1;
	}
	else if ((p->frequency == 2133) || ((vGet_Div_Mode(p) == DIV4_MODE) && ((p->frequency <= 800) || (p->frequency >= 600))))
	{
		u1Txp = 2;
	}

	if ((p->frequency == 1866) || ((p->frequency <= 1333) && (p->frequency >= 1200)))
	{
		u1Txp0p5 = 1;
	}

	ACTblFinal.txp = u1Txp;
	ACTblFinal.txp_05T = u1Txp0p5;
	ACTblFinal.ckelckcnt = 4;
	ACTblFinal.earlyckecnt = 0;
	ACTblFinal.ckeprd -= 1;
#endif

	// ----Step 2: Perform register writes for entries in ACTblFinal struct & ACTiming excel file (all actiming adjustments should be done in Step 1)-------

	vIO32WriteFldMulti_All(DRAMC_REG_SHU_ACTIM1, P_Fld(ACTblFinal.tras, SHU_ACTIM1_TRAS)
												| P_Fld(ACTblFinal.trp, SHU_ACTIM1_TRP)
												| P_Fld(ACTblFinal.trpab, SHU_ACTIM1_TRPAB)
												| P_Fld(ACTblFinal.tmrwckel, SHU_ACTIM1_TMRWCKEL)
												| P_Fld(ACTblFinal.trc, SHU_ACTIM1_TRC));

	vIO32WriteFldMulti_All(DRAMC_REG_SHU_ACTIM3, P_Fld(ACTblFinal.trfc, SHU_ACTIM3_TRFC)
												| P_Fld(ACTblFinal.tr2mrr, SHU_ACTIM3_TR2MRR)
												| P_Fld(ACTblFinal.trfcpb, SHU_ACTIM3_TRFCPB));

	vIO32WriteFldMulti_All(DRAMC_REG_SHU_ACTIM2, P_Fld(ACTblFinal.txp, SHU_ACTIM2_TXP)
												| P_Fld(ACTblFinal.tmrri, SHU_ACTIM2_TMRRI)
												| P_Fld(u2TFAW, SHU_ACTIM2_TFAW)
												| P_Fld(u2TRTW, SHU_ACTIM2_TR2W) // Value has odt_on/off difference, use local variable u1TRTW
												| P_Fld(ACTblFinal.trtp, SHU_ACTIM2_TRTP));

	vIO32WriteFldMulti_All(DRAMC_REG_SHU_ACTIM0, P_Fld(ACTblFinal.trcd, SHU_ACTIM0_TRCD)
												| P_Fld(ACTblFinal.twr, SHU_ACTIM0_TWR)
												| P_Fld(u2TRRD, SHU_ACTIM0_TRRD));

	vIO32WriteFldMulti_All(DRAMC_REG_SHU_ACTIM5, P_Fld(ACTblFinal.tpbr2pbr, SHU_ACTIM5_TPBR2PBR)
												| P_Fld(ACTblFinal.twtpd, SHU_ACTIM5_TWTPD)
												| P_Fld(ACTblFinal.tpbr2act, SHU_ACTIM5_TPBR2ACT));

	vIO32WriteFldMulti_All(DRAMC_REG_SHU_ACTIM6, P_Fld(ACTblFinal.tr2mrw, SHU_ACTIM6_TR2MRW)
												| P_Fld(ACTblFinal.tw2mrw, SHU_ACTIM6_TW2MRW)
												| P_Fld(ACTblFinal.tmrd, SHU_ACTIM6_TMRD)
												| P_Fld(ACTblFinal.zqlat2, SHU_ACTIM6_TZQLAT2)
												| P_Fld(ACTblFinal.tmrw, SHU_ACTIM6_TMRW));

	vIO32WriteFldMulti_All(DRAMC_REG_SHU_ACTIM4, P_Fld(ACTblFinal.tmrr2mrw, SHU_ACTIM4_TMRR2MRW)
												| P_Fld(u2TMRR2W, SHU_ACTIM4_TMRR2W)
												| P_Fld(ACTblFinal.tzqcs, SHU_ACTIM4_TZQCS)
												| P_Fld(ACTblFinal.txrefcnt, SHU_ACTIM4_TXREFCNT));

	vIO32WriteFldAlign_All(DRAMC_REG_SHU_CKECTRL, ACTblFinal.ckeprd, SHU_CKECTRL_TCKEPRD);

	vIO32WriteFldMulti_All(DRAMC_REG_SHU_ACTIM_XRT, P_Fld(u2XRTWTW, SHU_ACTIM_XRT_XRTW2W)
												| P_Fld(u2XRTW2R, SHU_ACTIM_XRT_XRTW2R)
												| P_Fld(u2XRTR2W, SHU_ACTIM_XRT_XRTR2W)
												| P_Fld(u2XTRTRT, SHU_ACTIM_XRT_XRTR2R));

	vIO32WriteFldAlign_All(DRAMC_REG_SHU_HWSET_VRCG, ACTblFinal.vrcgdis_prdcnt, SHU_HWSET_VRCG_VRCGDIS_PRDCNT);
//	  vIO32WriteFldMulti_All(DRAMC_REG_SHU_HWSET_VRCG, P_Fld(ACTblFinal.vrcgdis_prdcnt, SHU_HWSET_VRCG_VRCGDIS_PRDCNT)
//												  | P_Fld(ACTblFinal.hwset_vrcg_op, SHU_HWSET_VRCG_HWSET_VRCG_OP));
	vIO32WriteFldAlign_All(DRAMC_REG_SHU_HWSET_MR2, ACTblFinal.hwset_mr2_op, SHU_HWSET_MR2_HWSET_MR2_OP);
	vIO32WriteFldAlign_All(DRAMC_REG_SHU_HWSET_MR13, ACTblFinal.hwset_mr13_op, SHU_HWSET_MR13_HWSET_MR13_OP);

	// AC timing 0.5T
	vIO32WriteFldMulti_All(DRAMC_REG_SHU_AC_TIME_05T, P_Fld(ACTblFinal.twtr_05T, SHU_AC_TIME_05T_TWTR_M05T)
													| P_Fld(u2TRTW_05T, SHU_AC_TIME_05T_TR2W_05T) // Value has odt_on/off difference, use local variable u1TRTW
													| P_Fld(ACTblFinal.twtpd_05T, SHU_AC_TIME_05T_TWTPD_M05T)
													| P_Fld(u1TFAW_05T, SHU_AC_TIME_05T_TFAW_05T)
													| P_Fld(u1TRRD_05T, SHU_AC_TIME_05T_TRRD_05T)
													| P_Fld(ACTblFinal.twr_05T, SHU_AC_TIME_05T_TWR_M05T)
													| P_Fld(ACTblFinal.tras_05T, SHU_AC_TIME_05T_TRAS_05T)
													| P_Fld(ACTblFinal.trpab_05T, SHU_AC_TIME_05T_TRPAB_05T)
													| P_Fld(ACTblFinal.trp_05T, SHU_AC_TIME_05T_TRP_05T)
													| P_Fld(ACTblFinal.trcd_05T, SHU_AC_TIME_05T_TRCD_05T)
													| P_Fld(ACTblFinal.trtp_05T, SHU_AC_TIME_05T_TRTP_05T)
													| P_Fld(ACTblFinal.txp_05T, SHU_AC_TIME_05T_TXP_05T)
													| P_Fld(ACTblFinal.trfc_05T, SHU_AC_TIME_05T_TRFC_05T)
													| P_Fld(ACTblFinal.trfcpb_05T, SHU_AC_TIME_05T_TRFCPB_05T)
													| P_Fld(ACTblFinal.tpbr2pbr_05T, SHU_AC_TIME_05T_TPBR2PBR_05T)
													| P_Fld(ACTblFinal.tpbr2act_05T, SHU_AC_TIME_05T_TPBR2ACT_05T)
													| P_Fld(ACTblFinal.tr2mrw_05T, SHU_AC_TIME_05T_TR2MRW_05T)
													| P_Fld(ACTblFinal.tw2mrw_05T, SHU_AC_TIME_05T_TW2MRW_05T)
													| P_Fld(ACTblFinal.tmrr2mrw_05T, SHU_AC_TIME_05T_TMRR2MRW_05T)
													| P_Fld(ACTblFinal.tmrw_05T, SHU_AC_TIME_05T_TMRW_05T)
													| P_Fld(ACTblFinal.tmrd_05T, SHU_AC_TIME_05T_TMRD_05T)
													| P_Fld(ACTblFinal.tmrwckel_05T, SHU_AC_TIME_05T_TMRWCKEL_05T)
													| P_Fld(ACTblFinal.tmrri_05T, SHU_AC_TIME_05T_TMRRI_05T)
													| P_Fld(ACTblFinal.trc_05T, SHU_AC_TIME_05T_TRC_05T));

	{
		vIO32WriteFldAlign_All(DRAMC_REG_SHU_ACTIM0, ACTblFinal.twtr, SHU_ACTIM0_TWTR);
		vIO32WriteFldMulti_All(DRAMC_REG_SHU_CKECTRL, P_Fld(ACTblFinal.tpde, SHU_CKECTRL_TPDE)
														| P_Fld(ACTblFinal.tpdx, SHU_CKECTRL_TPDX)
														| P_Fld(ACTblFinal.tpde_05T, SHU_CKECTRL_TPDE_05T)
														| P_Fld(ACTblFinal.tpdx_05T, SHU_CKECTRL_TPDX_05T));
		vIO32WriteFldAlign_All(DRAMC_REG_SHU_ACTIM5, ACTblFinal.trtpd, SHU_ACTIM5_TR2PD);
		vIO32WriteFldAlign_All(DRAMC_REG_SHU_AC_TIME_05T, ACTblFinal.trtpd_05T, SHU_AC_TIME_05T_TR2PD_05T);
	}

#if AC_TIMING_DERATE_ENABLE
	if (u1IsLP4Family(p->dram_type))
	{
		vIO32WriteFldMulti_All(DRAMC_REG_SHU_AC_DERATING0, P_Fld(ACTblFinal.trcd_derate, SHU_AC_DERATING0_TRCD_DERATE)
														| P_Fld(ACTblFinal.trrd_derate, SHU_AC_DERATING0_TRRD_DERATE));
		vIO32WriteFldMulti_All(DRAMC_REG_SHU_AC_DERATING1, P_Fld(ACTblFinal.trc_derate, SHU_AC_DERATING1_TRC_DERATE)
														| P_Fld(ACTblFinal.tras_derate, SHU_AC_DERATING1_TRAS_DERATE)
														| P_Fld(ACTblFinal.trp_derate, SHU_AC_DERATING1_TRP_DERATE)
														| P_Fld(ACTblFinal.trpab_derate, SHU_AC_DERATING1_TRPAB_DERATE));
		vIO32WriteFldMulti_All(DRAMC_REG_SHU_AC_DERATING_05T, P_Fld(ACTblFinal.trrd_derate_05T, SHU_AC_DERATING_05T_TRRD_05T_DERATE)
														| P_Fld(ACTblFinal.tras_derate_05T, SHU_AC_DERATING_05T_TRAS_05T_DERATE)
														| P_Fld(ACTblFinal.trpab_derate_05T, SHU_AC_DERATING_05T_TRPAB_05T_DERATE)
														| P_Fld(ACTblFinal.trp_derate_05T, SHU_AC_DERATING_05T_TRP_05T_DERATE)
														| P_Fld(ACTblFinal.trcd_derate_05T, SHU_AC_DERATING_05T_TRCD_05T_DERATE)
														| P_Fld(ACTblFinal.trc_derate_05T, SHU_AC_DERATING_05T_TRC_05T_DERATE));
		vIO32WriteFldAlign_All(DRAMC_REG_REFCTRL3, 0xc0, REFCTRL3_REF_DERATING_EN);
		vIO32WriteFldAlign_All(DRAMC_REG_SHU_AC_DERATING0, 0x1, SHU_AC_DERATING0_ACDERATEEN); //enable derating for AC timing
	}
#endif

	// DQSINCTL related
	vSetRank(p, RANK_0);
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SHU_RK_DQSCTL, ACTblFinal.dqsinctl, MISC_SHU_RK_DQSCTL_DQSINCTL);// Rank 0 DQSINCTL
	vSetRank(p, RANK_1);
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SHU_RK_DQSCTL, ACTblFinal.dqsinctl, MISC_SHU_RK_DQSCTL_DQSINCTL);// Rank 1 DQSINCTL
	vSetRank(p, backup_rank);
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SHU_ODTCTRL, ACTblFinal.dqsinctl, MISC_SHU_ODTCTRL_RODT_LAT);

	if (ACTblFinal.dqsinctl >= 2)
	{
		u4RankINCTL_ROOT = ACTblFinal.dqsinctl - 2;
	}
	else
	{
		err("u4RankINCTL_ROOT <2, Please check\n");
		u4RankINCTL_ROOT = 0;
	}

	vIO32WriteFldMulti_All(DDRPHY_REG_MISC_SHU_RANKCTL, P_Fld(ACTblFinal.dqsinctl, MISC_SHU_RANKCTL_RANKINCTL_PHY)
													   | P_Fld(u4RankINCTL_ROOT, MISC_SHU_RANKCTL_RANKINCTL_ROOT1)
													   | P_Fld(u4RankINCTL_ROOT, MISC_SHU_RANKCTL_RANKINCTL));

#if XRTRTR_NEW_CROSS_RK_MODE
	u2PHSINCTL = (ACTblFinal.dqsinctl == 0)? 0: (ACTblFinal.dqsinctl - 1);
	vIO32WriteFldAlign_All(DDRPHY_REG_SHU_MISC_RANK_SEL_STB, u2PHSINCTL, SHU_MISC_RANK_SEL_STB_RANK_SEL_PHSINCTL);
#endif

	// DATLAT related, tREFBW
	vIO32WriteFldMulti_All(DDRPHY_REG_MISC_SHU_RDAT, P_Fld(ACTblFinal.datlat, MISC_SHU_RDAT_DATLAT)
											| P_Fld(u1DATLAT_DSEL, MISC_SHU_RDAT_DATLAT_DSEL)
											| P_Fld(u1DATLAT_DSEL, MISC_SHU_RDAT_DATLAT_DSEL_PHY));

	vIO32WriteFldAlign_All(DRAMC_REG_SHU_ACTIMING_CONF, u1TREFBW, SHU_ACTIMING_CONF_REFBW_FR);

	// ----Step 3: Perform register writes/calculation for other regs (That aren't in ACTblFinal struct)------------------------------------------------
#ifdef XRTR2R_PERFORM_ENHANCE_DQSG_RX_DLY
	//Wei-Jen: Ininital setting values are the same, RANKINCTL_RXDLY = RANKINCTL = RANKINCTL_ROOT1
	//XRTR2R setting will be updated in RxdqsGatingPostProcess
	u1RANKINCTL = u4IO32ReadFldAlign(DDRPHY_REG_MISC_SHU_RANKCTL, MISC_SHU_RANKCTL_RANKINCTL);
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SHU_RANKCTL, u1RANKINCTL, MISC_SHU_RANKCTL_RANKINCTL_RXDLY);
#endif

	//Update releated RG of XRTW2W
	if (p->frequency <= 800)
	{
		if (vGet_Div_Mode(p) == DIV4_MODE)
		{
			u1ROOT = 0; u1TXRANKINCTL = 1; u1TXDLY = 2;
		}
		else
		{
			u1ROOT = 0; u1TXRANKINCTL = 0; u1TXDLY = 1;
		}
	}
	else
	{
		u1ROOT = (p->frequency == 1866)? 1: 0;
		u1TXRANKINCTL = 1; u1TXDLY = 2;
	}
	#if TX_OE_EXTEND
	if (p->frequency >= 1333)
	{
		u1TXRANKINCTL += 1;
		u1TXDLY += 1;
	}
	#endif

	vIO32WriteFldMulti_All(DRAMC_REG_SHU_TX_RANKCTL, P_Fld(u1ROOT, SHU_TX_RANKCTL_TXRANKINCTL_ROOT)
												| P_Fld(u1TXRANKINCTL, SHU_TX_RANKCTL_TXRANKINCTL)
												| P_Fld(u1TXDLY, SHU_TX_RANKCTL_TXRANKINCTL_TXDLY));

	return DRAM_OK;
}


DRAM_STATUS_T DdrUpdateACTiming(DRAMC_CTX_T *p)
{
	U8 u1TimingIdx = 0;

	msg3("[UpdateACTiming]\n");

	//Retrieve ACTimingTable's corresponding index
	u1TimingIdx = u1GetACTimingIdx(p);

	if (u1TimingIdx == 0xff)
	{
		#if 0
		if (u1TmpDramType = TYPE_LPDDR4)
			u1TimingIdx = 0;
		else // LPDDR3
			u1TimingIdx = 6;
		err("Error, no match AC timing, use default timing %d\n", u1TimingIdx);
		#else
		err("Error, no match AC timing, not apply table\n");
		return DRAM_FAIL;
		#endif
	}

	//Set ACTiming registers
#if (__LP5_COMBO__ == TRUE)
	if (TRUE == is_lp5_family(p))
	{
		DdrUpdateACTimingReg_LP5(p, &ACTimingTbl_LP5[u1TimingIdx]);
	}
	else
#endif
	{
		DdrUpdateACTimingReg_LP4(p, &ACTimingTbl_LP4[u1TimingIdx]);
	}

	return DRAM_OK;
}
#if 0
#if ((!SW_CHANGE_FOR_SIMULATION) && (!FOR_DV_SIMULATION_USED && SW_CHANGE_FOR_SIMULATION == 0) && (!__ETT__))
DRAM_STATUS_T DdrUpdateACTiming_EMI(DRAMC_CTX_T *p, AC_TIMING_EXTERNAL_T *ACRegFromEmi)
{
	U8 u1TimingIdx = 0;
	#if (__LP5_COMBO__ == TRUE)
	ACTime_T_LP5 ACTime_LP5;
	#endif
	ACTime_T_LP4 ACTime_LP4;
	msg3("[DdrUpdateACTiming_EMI]\n");

   if (ACRegFromEmi == NULL)
		return DRAM_FAIL;

	//Retrieve ACTimingTable's corresponding index
	u1TimingIdx = u1GetACTimingIdx(p);
#if (__LP5_COMBO__ == TRUE)
	if (TRUE == is_lp5_family(p))
	{
		ACTime_LP5 = ACTimingTbl_LP5[u1TimingIdx];
	}
#endif
	ACTime_LP4 = ACTimingTbl_LP4[u1TimingIdx];

	//Overwrite AC timing from emi settings
	ACTime.dramType = p->dram_type;
#if 1 // Will use MDL ac timing, Others from internal ac timing
	ACTime.trp = ACRegFromEmi->AC_TIME_EMI_TRP;
	ACTime.trpab = ACRegFromEmi->AC_TIME_EMI_TRPAB;
	ACTime.trc = ACRegFromEmi->AC_TIME_EMI_TRC;
	ACTime.trcd = ACRegFromEmi->AC_TIME_EMI_TRCD;

	ACTime.trp_05T = ACRegFromEmi->AC_TIME_EMI_TRP_05T;
	ACTime.trpab_05T = ACRegFromEmi->AC_TIME_EMI_TRPAB_05T;
	ACTime.trc_05T = ACRegFromEmi->AC_TIME_EMI_TRC_05T;
	ACTime.trcd_05T = ACRegFromEmi->AC_TIME_EMI_TRCD_05T;
#else
	ACTime.freq = ACRegFromEmi->AC_TIME_EMI_FREQUENCY;
	ACTime.tras = ACRegFromEmi->AC_TIME_EMI_TRAS;
	ACTime.trp = ACRegFromEmi->AC_TIME_EMI_TRP;

	ACTime.trpab = ACRegFromEmi->AC_TIME_EMI_TRPAB;
	ACTime.trc = ACRegFromEmi->AC_TIME_EMI_TRC;
	ACTime.trfc = ACRegFromEmi->AC_TIME_EMI_TRFC;
	ACTime.trfcpb = ACRegFromEmi->AC_TIME_EMI_TRFCPB;

	ACTime.txp = ACRegFromEmi->AC_TIME_EMI_TXP;
	ACTime.trtp = ACRegFromEmi->AC_TIME_EMI_TRTP;
	ACTime.trcd = ACRegFromEmi->AC_TIME_EMI_TRCD;
	ACTime.twr = ACRegFromEmi->AC_TIME_EMI_TWR;

	ACTime.twtr = ACRegFromEmi->AC_TIME_EMI_TWTR;
	ACTime.trrd = ACRegFromEmi->AC_TIME_EMI_TRRD;
	ACTime.tfaw = ACRegFromEmi->AC_TIME_EMI_TFAW;
	ACTime.trtw_ODT_off = ACRegFromEmi->AC_TIME_EMI_TRTW_ODT_OFF;
	ACTime.trtw_ODT_on = ACRegFromEmi->AC_TIME_EMI_TRTW_ODT_ON;

	ACTime.refcnt = ACRegFromEmi->AC_TIME_EMI_REFCNT; //(REFFRERUN = 0)
	ACTime.refcnt_fr_clk = ACRegFromEmi->AC_TIME_EMI_REFCNT_FR_CLK; //(REFFRERUN = 1)
	ACTime.txrefcnt = ACRegFromEmi->AC_TIME_EMI_TXREFCNT;
	ACTime.tzqcs = ACRegFromEmi->AC_TIME_EMI_TZQCS;

	ACTime.trtpd = ACRegFromEmi->AC_TIME_EMI_TRTPD;
	ACTime.twtpd = ACRegFromEmi->AC_TIME_EMI_TWTPD;
	ACTime.tmrr2w_ODT_off = ACRegFromEmi->AC_TIME_EMI_TMRR2W_ODT_OFF;
	ACTime.tmrr2w_ODT_on = ACRegFromEmi->AC_TIME_EMI_TMRR2W_ODT_ON;

	ACTime.tras_05T = ACRegFromEmi->AC_TIME_EMI_TRAS_05T;
	ACTime.trp_05T = ACRegFromEmi->AC_TIME_EMI_TRP_05T;
	ACTime.trpab_05T = ACRegFromEmi->AC_TIME_EMI_TRPAB_05T;
	ACTime.trc_05T = ACRegFromEmi->AC_TIME_EMI_TRC_05T;
	ACTime.trfc_05T = ACRegFromEmi->AC_TIME_EMI_TRFC_05T;
	ACTime.trfcpb_05T = ACRegFromEmi->AC_TIME_EMI_TRFCPB_05T;
	ACTime.txp_05T = ACRegFromEmi->AC_TIME_EMI_TXP_05T;
	ACTime.trtp_05T = ACRegFromEmi->AC_TIME_EMI_TRTP_05T;
	ACTime.trcd_05T = ACRegFromEmi->AC_TIME_EMI_TRCD_05T;
	ACTime.twr_05T = ACRegFromEmi->AC_TIME_EMI_TWR_05T;
	ACTime.twtr_05T = ACRegFromEmi->AC_TIME_EMI_TWTR_05T;
	ACTime.trrd_05T = ACRegFromEmi->AC_TIME_EMI_TRRD_05T;
	ACTime.tfaw_05T = ACRegFromEmi->AC_TIME_EMI_TFAW_05T;
	ACTime.trtw_ODT_off_05T = ACRegFromEmi->AC_TIME_EMI_TRTW_ODT_OFF_05T;
	ACTime.trtw_ODT_on_05T = ACRegFromEmi->AC_TIME_EMI_TRTW_ODT_ON_05T;
	ACTime.trtpd_05T = ACRegFromEmi->AC_TIME_EMI_TRTPD_05T;
	ACTime.twtpd_05T = ACRegFromEmi->AC_TIME_EMI_TWTPD_05T;
#endif

	//Set ACTiming registers
	DdrUpdateACTimingReg(p, &ACTime);

	return DRAM_OK;
}
#endif
#endif
///TODO: wait for porting +++
#if __A60868_TO_BE_PORTING__

U8 vDramcACTimingGetDatLat(DRAMC_CTX_T *p)
{
	U8 u1TimingIdx = u1GetACTimingIdx(p);
#if (__LP5_COMBO__ == TRUE)
	ACTime_T_LP5 ACTime = ACTimingTbl_LP5[u1TimingIdx];
#else
	ACTime_T_LP4 ACTime = ACTimingTbl_LP4[u1TimingIdx];
#endif

	return ACTime.datlat;
}
#endif // __A60868_TO_BE_PORTING__
///TODO: wait for porting +++

/* Optimize all-bank refresh parameters (by density) for LP4 */
void vDramcACTimingOptimize(DRAMC_CTX_T *p)
{
	/* TRFC: tRFCab
	 *		 Refresh Cycle Time (All Banks)
	 * TXREFCNT: tXSR max((tRFCab + 7.5ns), 2nCK)
	 *			 Min self refresh time (Entry to Exit)
	 * u1ExecuteOptimize: Indicate if ACTimings are updated at the end of this function
	 */
	U8 u1RFCabGrpIdx = 0, u1FreqGrpIdx = 0, u1ExecuteOptimize = ENABLE;
	U8 u1TRFC=101, u1TRFC_05T=0, u1TRFCpb=44, u1TRFCpb_05T=0,u1TXREFCNT=118;
	typedef struct
	{	/* Bitfield sizes set to current project register field's size */
		U8 u1TRFC	   : 8;
		U8 u1TRFRC_05T : 1;
		U8 u1TRFCpb 	 : 8;
		U8 u1TRFRCpb_05T : 1;
		U16 u2TXREFCNT : 10;
	} optimizeACTime;
	/* JESD209-4B: tRFCab has 4 settings for 7 density settings (130, 180, 280, 380)
	 * tRFCAB_NUM: Used to indicate tRFCab group (since some densities share the same tRFCab)
	 */
	enum tRFCABIdx{tRFCAB_130 = 0, tRFCAB_180, tRFCAB_280, tRFCAB_380, tRFCAB_NUM};
	enum ACTimeIdx{GRP_DDR1200_ACTIM, GRP_DDR1600_ACTIM, GRP_DDR1866_ACTIM, GRP_DDR2400_ACTIM, GRP_DDR2667_ACTIM, GRP_DDR3200_ACTIM, GRP_DDR3733_ACTIM, GRP_DDR4266_ACTIM, GRP_ACTIM_NUM};
	enum ACTimeIdxDiv4{GRP_DDR800_DIV4_ACTIM = 0, GRP_DDR1200_DIV4_ACTIM, GRP_DDR1600_DIV4_ACTIM, GRP_ACTIM_NUM_DIV4};
	/* Values retrieved from 1. Alaska ACTiming excel file 2. JESD209-4B Refresh requirement table */

	optimizeACTime *ptRFCab_Opt;

	optimizeACTime tRFCab_Opt [GRP_ACTIM_NUM][tRFCAB_NUM] =
	{
		//For freqGroup DDR1200
		{{.u1TRFC = 8, .u1TRFRC_05T = 0, .u1TRFCpb = 0, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 21}, //tRFCab = 130, tRFCpb = 60, @Robert Not enough to Optimize
		 {.u1TRFC = 15, .u1TRFRC_05T = 1, .u1TRFCpb = 2, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 29}, //tRFCab = 180, tRFCpb = 90
		 {.u1TRFC = 30, .u1TRFRC_05T = 1, .u1TRFCpb = 9, .u1TRFRCpb_05T = 1, .u2TXREFCNT = 44}, //tRFCab = 280, tRFCpb = 140
		 {.u1TRFC = 45, .u1TRFRC_05T = 1, .u1TRFCpb = 17, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 59}},//tRFCab = 380, tRFCpb = 190
		//For freqGroup DDR1600
		{{.u1TRFC = 14, .u1TRFRC_05T = 0, .u1TRFCpb = 0, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 28}, //tRFCab = 130, tRFCpb = 60
		 {.u1TRFC = 24, .u1TRFRC_05T = 0, .u1TRFCpb = 6, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 38}, //tRFCab = 180, tRFCpb = 90
		 {.u1TRFC = 44, .u1TRFRC_05T = 0, .u1TRFCpb = 16, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 58}, //tRFCab = 280, tRFCpb = 140
		 {.u1TRFC = 64, .u1TRFRC_05T = 0, .u1TRFCpb = 26, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 78}},//tRFCab = 380, tRFCpb = 190
		//For freqGroup DDR1866
		{{.u1TRFC = 18, .u1TRFRC_05T = 1, .u1TRFCpb = 2, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 33}, //tRFCab = 130, tRFCpb = 60
		 {.u1TRFC = 30, .u1TRFRC_05T = 0, .u1TRFCpb = 9, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 44}, //tRFCab = 180, tRFCpb = 90
		 {.u1TRFC = 53, .u1TRFRC_05T = 1, .u1TRFCpb = 21, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 68}, //tRFCab = 280, tRFCpb = 140
		 {.u1TRFC = 77, .u1TRFRC_05T = 0, .u1TRFCpb = 32, .u1TRFRCpb_05T = 1, .u2TXREFCNT = 91}},//tRFCab = 380, tRFCpb = 190
		//For freqGroup DDR2400
		{{.u1TRFC = 27, .u1TRFRC_05T = 1,  .u1TRFCpb = 6, .u1TRFRCpb_05T = 1, .u2TXREFCNT = 42},   //tRFCab = 130, tRFCpb = 60
		 {.u1TRFC = 42, .u1TRFRC_05T = 1,  .u1TRFCpb = 15, .u1TRFRCpb_05T = 1, .u2TXREFCNT = 57},	//tRFCab = 180, tRFCpb = 90
		 {.u1TRFC = 72, .u1TRFRC_05T = 1,  .u1TRFCpb = 30, .u1TRFRCpb_05T = 1, .u2TXREFCNT = 87},	//tRFCab = 280, tRFCpb = 140
		 {.u1TRFC = 102, .u1TRFRC_05T = 1,	.u1TRFCpb = 45, .u1TRFRCpb_05T = 1, .u2TXREFCNT = 117}},//tRFCab = 380, tRFCpb = 190
		//For freqGroup DDR2667
		{{.u1TRFC = 31, .u1TRFRC_05T = 1,  .u1TRFCpb = 8, .u1TRFRCpb_05T = 1, .u2TXREFCNT = 46},	//tRFCab = 130, tRFCpb = 60
		 {.u1TRFC = 48, .u1TRFRC_05T = 1,  .u1TRFCpb = 18, .u1TRFRCpb_05T = 1, .u2TXREFCNT = 63},	 //tRFCab = 180, tRFCpb = 90
		 {.u1TRFC = 81, .u1TRFRC_05T = 1,  .u1TRFCpb = 35, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 96},	//tRFCab = 280, tRFCpb = 140
		 {.u1TRFC = 115, .u1TRFRC_05T = 0,	.u1TRFCpb = 51, .u1TRFRCpb_05T = 1, .u2TXREFCNT = 130}}, //tRFCab = 380, tRFCpb = 190
		//For freqGroup DDR3200
	   {{.u1TRFC = 40, .u1TRFRC_05T = 0,  .u1TRFCpb = 12, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 55},	//tRFCab = 130, tRFCpb = 60
		 {.u1TRFC = 60, .u1TRFRC_05T = 0,  .u1TRFCpb = 24, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 75},	 //tRFCab = 180, tRFCpb = 90
		 {.u1TRFC = 100, .u1TRFRC_05T = 0, .u1TRFCpb = 44, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 115},	 //tRFCab = 280, tRFCpb = 140
		 {.u1TRFC = 140, .u1TRFRC_05T = 0, .u1TRFCpb = 64, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 155}}, //tRFCab = 380, tRFCpb = 190
		//For freqGroup DDR3733
		{{.u1TRFC = 49, .u1TRFRC_05T = 0, .u1TRFCpb = 16, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 65},	//tRFCab = 130, tRFCpb = 60
		 {.u1TRFC = 72, .u1TRFRC_05T = 0, .u1TRFCpb = 30, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 88},	//tRFCab = 180, tRFCpb = 90
		 {.u1TRFC = 119, .u1TRFRC_05T = 0, .u1TRFCpb = 53, .u1TRFRCpb_05T = 1, .u2TXREFCNT = 135},	//tRFCab = 280, tRFCpb = 140
		 {.u1TRFC = 165, .u1TRFRC_05T = 1, .u1TRFCpb = 77, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 181}}, //tRFCab = 380, tRFCpb = 190
		//For freqGroup DDR4266
		{{.u1TRFC = 57, .u1TRFRC_05T = 1, .u1TRFCpb = 20, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 74},	//tRFCab = 130, tRFCpb = 60
		 {.u1TRFC = 84, .u1TRFRC_05T = 0, .u1TRFCpb = 36, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 100},	 //tRFCab = 180, tRFCpb = 90
		 {.u1TRFC = 137, .u1TRFRC_05T = 1, .u1TRFCpb = 63, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 154},	//tRFCab = 280, tRFCpb = 140
		 {.u1TRFC = 191, .u1TRFRC_05T = 0, .u1TRFCpb = 89, .u1TRFRCpb_05T = 1, .u2TXREFCNT = 207}} //tRFCab = 380, tRFCpb = 190
	};

	optimizeACTime tRFCab_Opt_Div4 [GRP_ACTIM_NUM_DIV4][tRFCAB_NUM] =
	{
		//NOTE: @Darren, For freqGroup DDR816
		{{.u1TRFC = 14, .u1TRFRC_05T = 0, .u1TRFCpb = 0, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 28}, //tRFCab = 130, tRFCpb = 60
		 {.u1TRFC = 24, .u1TRFRC_05T = 0, .u1TRFCpb = 6, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 38}, //tRFCab = 180, tRFCpb = 90
		 {.u1TRFC = 44, .u1TRFRC_05T = 0, .u1TRFCpb = 16, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 58}, //tRFCab = 280, tRFCpb = 140
		 {.u1TRFC = 64, .u1TRFRC_05T = 0, .u1TRFCpb = 26, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 78}},//tRFCab = 380, tRFCpb = 190
		//For freqGroup DDR1200
		{{.u1TRFC = 28, .u1TRFRC_05T = 0,  .u1TRFCpb = 7, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 42},   //tRFCab = 130, tRFCpb = 60
		 {.u1TRFC = 43, .u1TRFRC_05T = 0,  .u1TRFCpb = 16, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 57},	//tRFCab = 180, tRFCpb = 90
		 {.u1TRFC = 73, .u1TRFRC_05T = 0,  .u1TRFCpb = 31, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 87},	//tRFCab = 280, tRFCpb = 140
		 {.u1TRFC = 103, .u1TRFRC_05T = 0,	.u1TRFCpb = 46, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 117}},//tRFCab = 380, tRFCpb = 190
		//For freqGroup DDR1600
	   {{.u1TRFC = 40, .u1TRFRC_05T = 0,  .u1TRFCpb = 12, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 55},	//tRFCab = 130, tRFCpb = 60
		 {.u1TRFC = 60, .u1TRFRC_05T = 0,  .u1TRFCpb = 24, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 75},	 //tRFCab = 180, tRFCpb = 90
		 {.u1TRFC = 100, .u1TRFRC_05T = 0, .u1TRFCpb = 44, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 115},	 //tRFCab = 280, tRFCpb = 140
		 {.u1TRFC = 140, .u1TRFRC_05T = 0, .u1TRFCpb = 64, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 155}}, //tRFCab = 380, tRFCpb = 190
	};


	msg("[ACTimingOptimize]");

#if __ETT__
	if (p->density==0xff)
	{
		err("Error : No call MR8 to get density!!\n");
		while(1);
	}
#endif

	/* Set tRFCab group idx p->density = MR8 OP[5:2]*/
	switch (p->density)
	{
		case 0x0:	//4Gb per die  (2Gb per channel),  tRFCab=130
			u1RFCabGrpIdx = tRFCAB_130;
			break;
		case 0x1:	//6Gb per die  (3Gb per channel),  tRFCab=180
		case 0x2:	//8Gb per die  (4Gb per channel),  tRFCab=180
			u1RFCabGrpIdx = tRFCAB_180;
			break;
		case 0x3:	//12Gb per die (6Gb per channel),  tRFCab=280
		case 0x4:	//16Gb per die (8Gb per channel),  tRFCab=280
			u1RFCabGrpIdx = tRFCAB_280;
			break;
		case 0x5:	//24Gb per die (12Gb per channel), tRFCab=380
		case 0x6:	//32Gb per die (16Gb per channel), tRFCab=380
			u1RFCabGrpIdx = tRFCAB_380;
			break;
		default:
			u1ExecuteOptimize = DISABLE;
			err("MR8 density err!\n");
	}
	/* Set freqGroup Idx */
	switch (p->freqGroup)
	{
		case 400:
			if (vGet_Div_Mode(p) == DIV4_MODE)
				u1FreqGrpIdx = GRP_DDR800_DIV4_ACTIM;
			else
			{
				u1ExecuteOptimize = DISABLE;
				err("freqGroup err!\n");
				#if __ETT__
				while(1);
				#endif
			}
			break;
		case 600:
			if (vGet_Div_Mode(p) == DIV4_MODE)
				u1FreqGrpIdx = GRP_DDR1200_DIV4_ACTIM;
			else
				u1FreqGrpIdx = GRP_DDR1200_ACTIM;
			break;
		case 800:
			if (vGet_Div_Mode(p) == DIV4_MODE)
				u1FreqGrpIdx = GRP_DDR1600_DIV4_ACTIM;
			else
				u1FreqGrpIdx = GRP_DDR1600_ACTIM;
			break;
		case 933:
			u1FreqGrpIdx = GRP_DDR1866_ACTIM;
			break;
		case 1200:
			u1FreqGrpIdx = GRP_DDR2400_ACTIM;
			break;
		case 1333:
			u1FreqGrpIdx = GRP_DDR2667_ACTIM;
			break;
		case 1600:
			u1FreqGrpIdx = GRP_DDR3200_ACTIM;
			break;
		case 1866:
			u1FreqGrpIdx = GRP_DDR3733_ACTIM;
			break;
		case 2133:
			u1FreqGrpIdx = GRP_DDR4266_ACTIM;
			break;
		default:
			u1ExecuteOptimize = DISABLE;
			err("freqGroup err!\n");
			#if __ETT__
			while(1);
			#endif
	}

	if (vGet_Div_Mode(p) == DIV4_MODE && u1FreqGrpIdx >= GRP_ACTIM_NUM_DIV4)
	{
			u1ExecuteOptimize = DISABLE;
			err("freqGroup err!\n");
			#if __ETT__
			while(1);
			#endif
	}
	if (vGet_Div_Mode(p) == DIV4_MODE && u1FreqGrpIdx < GRP_ACTIM_NUM_DIV4)
		ptRFCab_Opt = &tRFCab_Opt_Div4[u1FreqGrpIdx][0];
	else
		ptRFCab_Opt = &tRFCab_Opt[u1FreqGrpIdx][0];

	u1TRFC = ptRFCab_Opt[u1RFCabGrpIdx].u1TRFC;
	u1TRFC_05T = ptRFCab_Opt[u1RFCabGrpIdx].u1TRFRC_05T;
	u1TRFCpb = ptRFCab_Opt[u1RFCabGrpIdx].u1TRFCpb;
	u1TRFCpb_05T = ptRFCab_Opt[u1RFCabGrpIdx].u1TRFRCpb_05T;
	u1TXREFCNT = ptRFCab_Opt[u1RFCabGrpIdx].u2TXREFCNT;

	/* Only execute ACTimingOptimize(write to regs) when corresponding values have been found */
	if (u1ExecuteOptimize == ENABLE)
	{
		vIO32WriteFldAlign_All(DRAMC_REG_SHU_ACTIM3, u1TRFC, SHU_ACTIM3_TRFC);
		vIO32WriteFldAlign_All(DRAMC_REG_SHU_AC_TIME_05T, u1TRFC_05T, SHU_AC_TIME_05T_TRFC_05T);
		vIO32WriteFldAlign_All(DRAMC_REG_SHU_ACTIM4, u1TXREFCNT, SHU_ACTIM4_TXREFCNT);
		vIO32WriteFldAlign_All(DRAMC_REG_SHU_ACTIM3, u1TRFCpb, SHU_ACTIM3_TRFCPB);
		vIO32WriteFldAlign_All(DRAMC_REG_SHU_AC_TIME_05T, u1TRFCpb_05T, SHU_AC_TIME_05T_TRFCPB_05T);

		msg("Density (MR8 OP[5:2]) %u, TRFC %u, TRFC_05T %u, TXREFCNT %u, TRFCpb %u, TRFCpb_05T %u\n", p->density, u1TRFC, u1TRFC_05T, u1TXREFCNT, u1TRFCpb, u1TRFCpb_05T);
	}

	return;
}

/* ACTimingTbl: All freq's ACTiming from ACTiming excel file
 * (Some fields don't exist for LP3 -> set to 0)
 * Note: !!All ACTiming adjustments should not be set in-table should be moved into UpdateACTimingReg()!!
 *		 Or else preloader's highest freq ACTimings may be set to different values than expected.
 */
const ACTime_T_LP4 ACTimingTbl_LP4[AC_TIMING_NUMBER_LP4] = {
	//----------LPDDR4---------------------------
#if SUPPORT_LP4_DDR4266_ACTIM
	//LP4_DDR4266 ACTiming---------------------------------
#if (ENABLE_READ_DBI == 1)
//LPDDR4 4X_4266_Div 8_DBI1.csv Read 1
{
	.dramType = TYPE_LPDDR4, .freq = 2133, .cbtMode = CBT_NORMAL_MODE, .readDBI = 1,
	.readLat = 40,	.writeLat =  18,	.DivMode =	DIV8_MODE,

	.tras = 14,	.tras_05T = 0,
	.trp = 8,	.trp_05T = 1,
	.trpab = 10,	.trpab_05T = 0,
	.trc = 23,	.trc_05T = 0,
	.trfc = 137,	.trfc_05T = 1,
	.trfcpb = 63,	.trfcpb_05T = 0,
	.txp = 1,	.txp_05T = 0,
	.trtp = 2,	.trtp_05T = 1,
	.trcd = 10,	.trcd_05T = 0,
	.twr = 15,	.twr_05T = 0,
	.twtr = 10,	.twtr_05T = 1,
	.tpbr2pbr = 41,	.tpbr2pbr_05T = 0,
	.tpbr2act = 0,	.tpbr2act_05T = 0,
	.tr2mrw = 18,	.tr2mrw_05T = 0,
	.tw2mrw = 11,	.tw2mrw_05T = 0,
	.tmrr2mrw = 15,	.tmrr2mrw_05T = 0,
	.tmrw = 6,	.tmrw_05T = 0,
	.tmrd = 8,	.tmrd_05T = 0,
	.tmrwckel = 9,	.tmrwckel_05T = 0,
	.tpde = 1,	.tpde_05T = 1,
	.tpdx = 1,	.tpdx_05T = 0,
	.tmrri = 14,	.tmrri_05T = 0,
	.trrd = 4,	.trrd_05T = 1,
	.trrd_4266 = 3,	.trrd_4266_05T = 0,
	.tfaw = 13,	.tfaw_05T = 1,
	.tfaw_4266 = 8,	.tfaw_4266_05T = 0,
	.trtw_odt_off = 7,	.trtw_odt_off_05T = 0,
	.trtw_odt_on = 9,	.trtw_odt_on_05T = 0,
	.txrefcnt = 154,
	.tzqcs = 46,
	.xrtw2w_new_mode = 5,
	.xrtw2w_old_mode = 6,
	.xrtw2r_odt_on = 1,
	.xrtw2r_odt_off = 1,
	.xrtr2w_odt_on = 8,
	.xrtr2w_odt_off = 8,
	.xrtr2r_new_mode = 3,
	.xrtr2r_old_mode = 7,
	.tr2mrr = 4,
	.vrcgdis_prdcnt = 54,
	.hwset_mr2_op = 63,
	.hwset_mr13_op = 216,
	.hwset_vrcg_op = 208,
	.trcd_derate = 11,	.trcd_derate_05T = 0,
	.trc_derate = 26,	.trc_derate_05T = 0,
	.tras_derate = 15,	.tras_derate_05T = 0,
	.trpab_derate = 11,	.trpab_derate_05T = 0,
	.trp_derate = 9,	.trp_derate_05T = 1,
	.trrd_derate = 5,	.trrd_derate_05T = 1,
	.trtpd = 15,	.trtpd_05T = 1,
	.twtpd = 18,	.twtpd_05T = 0,
	.tmrr2w_odt_off = 11,
	.tmrr2w_odt_on = 13,
	.ckeprd = 3,
	.ckelckcnt = 3,
	.zqlat2 = 16,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 7,	 .datlat = 18
},
//LPDDR4 4X_4266_BT_Div 8_DBI1.csv Read 1
{
	.dramType = TYPE_LPDDR4, .freq = 2133, .cbtMode = CBT_BYTE_MODE1, .readDBI = 1,
	.readLat = 44,	.writeLat =  18,	.DivMode =	DIV8_MODE,

	.tras = 14,	.tras_05T = 0,
	.trp = 8,	.trp_05T = 1,
	.trpab = 10,	.trpab_05T = 0,
	.trc = 23,	.trc_05T = 0,
	.trfc = 137,	.trfc_05T = 1,
	.trfcpb = 63,	.trfcpb_05T = 0,
	.txp = 1,	.txp_05T = 0,
	.trtp = 2,	.trtp_05T = 1,
	.trcd = 10,	.trcd_05T = 0,
	.twr = 16,	.twr_05T = 0,
	.twtr = 11,	.twtr_05T = 1,
	.tpbr2pbr = 41,	.tpbr2pbr_05T = 0,
	.tpbr2act = 0,	.tpbr2act_05T = 0,
	.tr2mrw = 19,	.tr2mrw_05T = 0,
	.tw2mrw = 11,	.tw2mrw_05T = 0,
	.tmrr2mrw = 16,	.tmrr2mrw_05T = 0,
	.tmrw = 6,	.tmrw_05T = 0,
	.tmrd = 8,	.tmrd_05T = 0,
	.tmrwckel = 9,	.tmrwckel_05T = 0,
	.tpde = 1,	.tpde_05T = 1,
	.tpdx = 1,	.tpdx_05T = 0,
	.tmrri = 14,	.tmrri_05T = 0,
	.trrd = 4,	.trrd_05T = 1,
	.trrd_4266 = 3,	.trrd_4266_05T = 0,
	.tfaw = 13,	.tfaw_05T = 1,
	.tfaw_4266 = 8,	.tfaw_4266_05T = 0,
	.trtw_odt_off = 8,	.trtw_odt_off_05T = 0,
	.trtw_odt_on = 10,	.trtw_odt_on_05T = 0,
	.txrefcnt = 154,
	.tzqcs = 46,
	.xrtw2w_new_mode = 5,
	.xrtw2w_old_mode = 6,
	.xrtw2r_odt_on = 1,
	.xrtw2r_odt_off = 1,
	.xrtr2w_odt_on = 9,
	.xrtr2w_odt_off = 9,
	.xrtr2r_new_mode = 3,
	.xrtr2r_old_mode = 7,
	.tr2mrr = 4,
	.vrcgdis_prdcnt = 54,
	.hwset_mr2_op = 63,
	.hwset_mr13_op = 216,
	.hwset_vrcg_op = 208,
	.trcd_derate = 11,	.trcd_derate_05T = 0,
	.trc_derate = 26,	.trc_derate_05T = 0,
	.tras_derate = 15,	.tras_derate_05T = 0,
	.trpab_derate = 11,	.trpab_derate_05T = 0,
	.trp_derate = 9,	.trp_derate_05T = 1,
	.trrd_derate = 5,	.trrd_derate_05T = 1,
	.trtpd = 16,	.trtpd_05T = 1,
	.twtpd = 19,	.twtpd_05T = 0,
	.tmrr2w_odt_off = 12,
	.tmrr2w_odt_on = 14,
	.ckeprd = 3,
	.ckelckcnt = 3,
	.zqlat2 = 16,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 7,	 .datlat = 18
},
#else  //ENABLE_READ_DBI == 0)
//LPDDR4 4X_4266_Div 8_DBI0.csv Read 0
{
	.dramType = TYPE_LPDDR4, .freq = 2133, .cbtMode = CBT_NORMAL_MODE, .readDBI = 0,
	.readLat = 36,	.writeLat =  18,	.DivMode =	DIV8_MODE,

	.tras = 14,	.tras_05T = 0,
	.trp = 8,	.trp_05T = 1,
	.trpab = 10,	.trpab_05T = 0,
	.trc = 23,	.trc_05T = 0,
	.trfc = 137,	.trfc_05T = 1,
	.trfcpb = 63,	.trfcpb_05T = 0,
	.txp = 1,	.txp_05T = 0,
	.trtp = 2,	.trtp_05T = 1,
	.trcd = 10,	.trcd_05T = 0,
	.twr = 15,	.twr_05T = 0,
	.twtr = 10,	.twtr_05T = 1,
	.tpbr2pbr = 41,	.tpbr2pbr_05T = 0,
	.tpbr2act = 0,	.tpbr2act_05T = 0,
	.tr2mrw = 17,	.tr2mrw_05T = 0,
	.tw2mrw = 11,	.tw2mrw_05T = 0,
	.tmrr2mrw = 14,	.tmrr2mrw_05T = 0,
	.tmrw = 6,	.tmrw_05T = 0,
	.tmrd = 8,	.tmrd_05T = 0,
	.tmrwckel = 9,	.tmrwckel_05T = 0,
	.tpde = 1,	.tpde_05T = 1,
	.tpdx = 1,	.tpdx_05T = 0,
	.tmrri = 14,	.tmrri_05T = 0,
	.trrd = 4,	.trrd_05T = 1,
	.trrd_4266 = 3,	.trrd_4266_05T = 0,
	.tfaw = 13,	.tfaw_05T = 1,
	.tfaw_4266 = 8,	.tfaw_4266_05T = 0,
	.trtw_odt_off = 6,	.trtw_odt_off_05T = 0,
	.trtw_odt_on = 8,	.trtw_odt_on_05T = 0,
	.txrefcnt = 154,
	.tzqcs = 46,
	.xrtw2w_new_mode = 5,
	.xrtw2w_old_mode = 6,
	.xrtw2r_odt_on = 1,
	.xrtw2r_odt_off = 1,
	.xrtr2w_odt_on = 7,
	.xrtr2w_odt_off = 7,
	.xrtr2r_new_mode = 3,
	.xrtr2r_old_mode = 7,
	.tr2mrr = 4,
	.vrcgdis_prdcnt = 54,
	.hwset_mr2_op = 63,
	.hwset_mr13_op = 216,
	.hwset_vrcg_op = 208,
	.trcd_derate = 11,	.trcd_derate_05T = 0,
	.trc_derate = 26,	.trc_derate_05T = 0,
	.tras_derate = 15,	.tras_derate_05T = 0,
	.trpab_derate = 11,	.trpab_derate_05T = 0,
	.trp_derate = 9,	.trp_derate_05T = 1,
	.trrd_derate = 5,	.trrd_derate_05T = 1,
	.trtpd = 14,	.trtpd_05T = 1,
	.twtpd = 18,	.twtpd_05T = 0,
	.tmrr2w_odt_off = 10,
	.tmrr2w_odt_on = 12,
	.ckeprd = 3,
	.ckelckcnt = 3,
	.zqlat2 = 16,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 7,	 .datlat = 18
},
//LPDDR4 4X_4266_BT_Div 8_DBI0.csv Read 0
{
	.dramType = TYPE_LPDDR4, .freq = 2133, .cbtMode = CBT_BYTE_MODE1, .readDBI = 0,
	.readLat = 40,	.writeLat =  18,	.DivMode =	DIV8_MODE,

	.tras = 14,	.tras_05T = 0,
	.trp = 8,	.trp_05T = 1,
	.trpab = 10,	.trpab_05T = 0,
	.trc = 23,	.trc_05T = 0,
	.trfc = 137,	.trfc_05T = 1,
	.trfcpb = 63,	.trfcpb_05T = 0,
	.txp = 1,	.txp_05T = 0,
	.trtp = 2,	.trtp_05T = 1,
	.trcd = 10,	.trcd_05T = 0,
	.twr = 16,	.twr_05T = 0,
	.twtr = 11,	.twtr_05T = 1,
	.tpbr2pbr = 41,	.tpbr2pbr_05T = 0,
	.tpbr2act = 0,	.tpbr2act_05T = 0,
	.tr2mrw = 18,	.tr2mrw_05T = 0,
	.tw2mrw = 11,	.tw2mrw_05T = 0,
	.tmrr2mrw = 15,	.tmrr2mrw_05T = 0,
	.tmrw = 6,	.tmrw_05T = 0,
	.tmrd = 8,	.tmrd_05T = 0,
	.tmrwckel = 9,	.tmrwckel_05T = 0,
	.tpde = 1,	.tpde_05T = 1,
	.tpdx = 1,	.tpdx_05T = 0,
	.tmrri = 14,	.tmrri_05T = 0,
	.trrd = 4,	.trrd_05T = 1,
	.trrd_4266 = 3,	.trrd_4266_05T = 0,
	.tfaw = 13,	.tfaw_05T = 1,
	.tfaw_4266 = 8,	.tfaw_4266_05T = 0,
	.trtw_odt_off = 7,	.trtw_odt_off_05T = 0,
	.trtw_odt_on = 9,	.trtw_odt_on_05T = 0,
	.txrefcnt = 154,
	.tzqcs = 46,
	.xrtw2w_new_mode = 5,
	.xrtw2w_old_mode = 6,
	.xrtw2r_odt_on = 1,
	.xrtw2r_odt_off = 1,
	.xrtr2w_odt_on = 8,
	.xrtr2w_odt_off = 8,
	.xrtr2r_new_mode = 3,
	.xrtr2r_old_mode = 7,
	.tr2mrr = 4,
	.vrcgdis_prdcnt = 54,
	.hwset_mr2_op = 63,
	.hwset_mr13_op = 216,
	.hwset_vrcg_op = 208,
	.trcd_derate = 11,	.trcd_derate_05T = 0,
	.trc_derate = 26,	.trc_derate_05T = 0,
	.tras_derate = 15,	.tras_derate_05T = 0,
	.trpab_derate = 11,	.trpab_derate_05T = 0,
	.trp_derate = 9,	.trp_derate_05T = 1,
	.trrd_derate = 5,	.trrd_derate_05T = 1,
	.trtpd = 15,	.trtpd_05T = 1,
	.twtpd = 19,	.twtpd_05T = 0,
	.tmrr2w_odt_off = 11,
	.tmrr2w_odt_on = 13,
	.ckeprd = 3,
	.ckelckcnt = 3,
	.zqlat2 = 16,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 7,	 .datlat = 18
},
#endif
#endif
#if SUPPORT_LP4_DDR3733_ACTIM
	//LP4_DDR3733 ACTiming---------------------------------
#if (ENABLE_READ_DBI == 1)
//LPDDR4 4X_3733_Div 8_DBI1.csv Read 1
{
	.dramType = TYPE_LPDDR4, .freq = 1866, .cbtMode = CBT_NORMAL_MODE, .readDBI = 1,
	.readLat = 36,	.writeLat =  16,	.DivMode =	DIV8_MODE,

	.tras = 11,	.tras_05T = 1,
	.trp = 7,	.trp_05T = 0,
	.trpab = 8,	.trpab_05T = 1,
	.trc = 19,	.trc_05T = 0,
	.trfc = 119,	.trfc_05T = 0,
	.trfcpb = 53,	.trfcpb_05T = 1,
	.txp = 0,	.txp_05T = 1,
	.trtp = 2,	.trtp_05T = 0,
	.trcd = 8,	.trcd_05T = 1,
	.twr = 13,	.twr_05T = 1,
	.twtr = 8,	.twtr_05T = 0,
	.tpbr2pbr = 35,	.tpbr2pbr_05T = 0,
	.tpbr2act = 0,	.tpbr2act_05T = 0,
	.tr2mrw = 16,	.tr2mrw_05T = 1,
	.tw2mrw = 10,	.tw2mrw_05T = 0,
	.tmrr2mrw = 13,	.tmrr2mrw_05T = 1,
	.tmrw = 5,	.tmrw_05T = 1,
	.tmrd = 7,	.tmrd_05T = 1,
	.tmrwckel = 8,	.tmrwckel_05T = 1,
	.tpde = 1,	.tpde_05T = 1,
	.tpdx = 1,	.tpdx_05T = 0,
	.tmrri = 12,	.tmrri_05T = 0,
	.trrd = 4,	.trrd_05T = 0,
	.trrd_4266 = 2,	.trrd_4266_05T = 1,
	.tfaw = 11,	.tfaw_05T = 0,
	.tfaw_4266 = 6,	.tfaw_4266_05T = 0,
	.trtw_odt_off = 6,	.trtw_odt_off_05T = 0,
	.trtw_odt_on = 9,	.trtw_odt_on_05T = 0,
	.txrefcnt = 135,
	.tzqcs = 40,
	.xrtw2w_new_mode = 5,
	.xrtw2w_old_mode = 6,
	.xrtw2r_odt_on = 1,
	.xrtw2r_odt_off = 1,
	.xrtr2w_odt_on = 8,
	.xrtr2w_odt_off = 8,
	.xrtr2r_new_mode = 3,
	.xrtr2r_old_mode = 7,
	.tr2mrr = 4,
	.vrcgdis_prdcnt = 47,
	.hwset_mr2_op = 54,
	.hwset_mr13_op = 216,
	.hwset_vrcg_op = 208,
	.trcd_derate = 9,	.trcd_derate_05T = 1,
	.trc_derate = 21,	.trc_derate_05T = 1,
	.tras_derate = 12,	.tras_derate_05T = 0,
	.trpab_derate = 9,	.trpab_derate_05T = 1,
	.trp_derate = 8,	.trp_derate_05T = 0,
	.trrd_derate = 5,	.trrd_derate_05T = 0,
	.trtpd = 14,	.trtpd_05T = 0,
	.twtpd = 16,	.twtpd_05T = 1,
	.tmrr2w_odt_off = 10,
	.tmrr2w_odt_on = 12,
	.ckeprd = 3,
	.ckelckcnt = 3,
	.zqlat2 = 14,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 6,	 .datlat = 16
},
//LPDDR4 4X_3733_BT_Div 8_DBI1.csv Read 1
{
	.dramType = TYPE_LPDDR4, .freq = 1866, .cbtMode = CBT_BYTE_MODE1, .readDBI = 1,
	.readLat = 40,	.writeLat =  16,	.DivMode =	DIV8_MODE,

	.tras = 11,	.tras_05T = 1,
	.trp = 7,	.trp_05T = 0,
	.trpab = 8,	.trpab_05T = 1,
	.trc = 19,	.trc_05T = 0,
	.trfc = 119,	.trfc_05T = 0,
	.trfcpb = 53,	.trfcpb_05T = 1,
	.txp = 0,	.txp_05T = 1,
	.trtp = 2,	.trtp_05T = 0,
	.trcd = 8,	.trcd_05T = 1,
	.twr = 14,	.twr_05T = 1,
	.twtr = 9,	.twtr_05T = 0,
	.tpbr2pbr = 35,	.tpbr2pbr_05T = 0,
	.tpbr2act = 0,	.tpbr2act_05T = 0,
	.tr2mrw = 17,	.tr2mrw_05T = 1,
	.tw2mrw = 10,	.tw2mrw_05T = 0,
	.tmrr2mrw = 14,	.tmrr2mrw_05T = 1,
	.tmrw = 5,	.tmrw_05T = 1,
	.tmrd = 7,	.tmrd_05T = 1,
	.tmrwckel = 8,	.tmrwckel_05T = 1,
	.tpde = 1,	.tpde_05T = 1,
	.tpdx = 1,	.tpdx_05T = 0,
	.tmrri = 12,	.tmrri_05T = 0,
	.trrd = 4,	.trrd_05T = 0,
	.trrd_4266 = 2,	.trrd_4266_05T = 1,
	.tfaw = 11,	.tfaw_05T = 0,
	.tfaw_4266 = 6,	.tfaw_4266_05T = 0,
	.trtw_odt_off = 7,	.trtw_odt_off_05T = 0,
	.trtw_odt_on = 10,	.trtw_odt_on_05T = 0,
	.txrefcnt = 135,
	.tzqcs = 40,
	.xrtw2w_new_mode = 5,
	.xrtw2w_old_mode = 6,
	.xrtw2r_odt_on = 1,
	.xrtw2r_odt_off = 1,
	.xrtr2w_odt_on = 9,
	.xrtr2w_odt_off = 9,
	.xrtr2r_new_mode = 3,
	.xrtr2r_old_mode = 7,
	.tr2mrr = 4,
	.vrcgdis_prdcnt = 47,
	.hwset_mr2_op = 54,
	.hwset_mr13_op = 216,
	.hwset_vrcg_op = 208,
	.trcd_derate = 9,	.trcd_derate_05T = 1,
	.trc_derate = 21,	.trc_derate_05T = 1,
	.tras_derate = 12,	.tras_derate_05T = 0,
	.trpab_derate = 9,	.trpab_derate_05T = 1,
	.trp_derate = 8,	.trp_derate_05T = 0,
	.trrd_derate = 5,	.trrd_derate_05T = 0,
	.trtpd = 15,	.trtpd_05T = 0,
	.twtpd = 17,	.twtpd_05T = 1,
	.tmrr2w_odt_off = 11,
	.tmrr2w_odt_on = 13,
	.ckeprd = 3,
	.ckelckcnt = 3,
	.zqlat2 = 14,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 6,	 .datlat = 16
},
#else  //ENABLE_READ_DBI == 0)
//LPDDR4 4X_3733_Div 8_DBI0.csv Read 0
{
	.dramType = TYPE_LPDDR4, .freq = 1866, .cbtMode = CBT_NORMAL_MODE, .readDBI = 0,
	.readLat = 32,	.writeLat =  16,	.DivMode =	DIV8_MODE,

	.tras = 11,	.tras_05T = 1,
	.trp = 7,	.trp_05T = 0,
	.trpab = 8,	.trpab_05T = 1,
	.trc = 19,	.trc_05T = 0,
	.trfc = 119,	.trfc_05T = 0,
	.trfcpb = 53,	.trfcpb_05T = 1,
	.txp = 0,	.txp_05T = 1,
	.trtp = 2,	.trtp_05T = 0,
	.trcd = 8,	.trcd_05T = 1,
	.twr = 13,	.twr_05T = 1,
	.twtr = 8,	.twtr_05T = 0,
	.tpbr2pbr = 35,	.tpbr2pbr_05T = 0,
	.tpbr2act = 0,	.tpbr2act_05T = 0,
	.tr2mrw = 15,	.tr2mrw_05T = 1,
	.tw2mrw = 10,	.tw2mrw_05T = 0,
	.tmrr2mrw = 12,	.tmrr2mrw_05T = 1,
	.tmrw = 5,	.tmrw_05T = 1,
	.tmrd = 7,	.tmrd_05T = 1,
	.tmrwckel = 8,	.tmrwckel_05T = 1,
	.tpde = 1,	.tpde_05T = 1,
	.tpdx = 1,	.tpdx_05T = 0,
	.tmrri = 12,	.tmrri_05T = 0,
	.trrd = 4,	.trrd_05T = 0,
	.trrd_4266 = 2,	.trrd_4266_05T = 1,
	.tfaw = 11,	.tfaw_05T = 0,
	.tfaw_4266 = 6,	.tfaw_4266_05T = 0,
	.trtw_odt_off = 5,	.trtw_odt_off_05T = 0,
	.trtw_odt_on = 8,	.trtw_odt_on_05T = 0,
	.txrefcnt = 135,
	.tzqcs = 40,
	.xrtw2w_new_mode = 5,
	.xrtw2w_old_mode = 6,
	.xrtw2r_odt_on = 1,
	.xrtw2r_odt_off = 1,
	.xrtr2w_odt_on = 7,
	.xrtr2w_odt_off = 7,
	.xrtr2r_new_mode = 3,
	.xrtr2r_old_mode = 7,
	.tr2mrr = 4,
	.vrcgdis_prdcnt = 47,
	.hwset_mr2_op = 54,
	.hwset_mr13_op = 216,
	.hwset_vrcg_op = 208,
	.trcd_derate = 9,	.trcd_derate_05T = 1,
	.trc_derate = 21,	.trc_derate_05T = 1,
	.tras_derate = 12,	.tras_derate_05T = 0,
	.trpab_derate = 9,	.trpab_derate_05T = 1,
	.trp_derate = 8,	.trp_derate_05T = 0,
	.trrd_derate = 5,	.trrd_derate_05T = 0,
	.trtpd = 13,	.trtpd_05T = 0,
	.twtpd = 16,	.twtpd_05T = 1,
	.tmrr2w_odt_off = 9,
	.tmrr2w_odt_on = 11,
	.ckeprd = 3,
	.ckelckcnt = 3,
	.zqlat2 = 14,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 6,	 .datlat = 16
},
//LPDDR4 4X_3733_BT_Div 8_DBI0.csv Read 0
{
	.dramType = TYPE_LPDDR4, .freq = 1866, .cbtMode = CBT_BYTE_MODE1, .readDBI = 0,
	.readLat = 36,	.writeLat =  16,	.DivMode =	DIV8_MODE,

	.tras = 11,	.tras_05T = 1,
	.trp = 7,	.trp_05T = 0,
	.trpab = 8,	.trpab_05T = 1,
	.trc = 19,	.trc_05T = 0,
	.trfc = 119,	.trfc_05T = 0,
	.trfcpb = 53,	.trfcpb_05T = 1,
	.txp = 0,	.txp_05T = 1,
	.trtp = 2,	.trtp_05T = 0,
	.trcd = 8,	.trcd_05T = 1,
	.twr = 14,	.twr_05T = 1,
	.twtr = 9,	.twtr_05T = 0,
	.tpbr2pbr = 35,	.tpbr2pbr_05T = 0,
	.tpbr2act = 0,	.tpbr2act_05T = 0,
	.tr2mrw = 16,	.tr2mrw_05T = 1,
	.tw2mrw = 10,	.tw2mrw_05T = 0,
	.tmrr2mrw = 13,	.tmrr2mrw_05T = 1,
	.tmrw = 5,	.tmrw_05T = 1,
	.tmrd = 7,	.tmrd_05T = 1,
	.tmrwckel = 8,	.tmrwckel_05T = 1,
	.tpde = 1,	.tpde_05T = 1,
	.tpdx = 1,	.tpdx_05T = 0,
	.tmrri = 12,	.tmrri_05T = 0,
	.trrd = 4,	.trrd_05T = 0,
	.trrd_4266 = 2,	.trrd_4266_05T = 1,
	.tfaw = 11,	.tfaw_05T = 0,
	.tfaw_4266 = 6,	.tfaw_4266_05T = 0,
	.trtw_odt_off = 6,	.trtw_odt_off_05T = 0,
	.trtw_odt_on = 9,	.trtw_odt_on_05T = 0,
	.txrefcnt = 135,
	.tzqcs = 40,
	.xrtw2w_new_mode = 5,
	.xrtw2w_old_mode = 6,
	.xrtw2r_odt_on = 1,
	.xrtw2r_odt_off = 1,
	.xrtr2w_odt_on = 8,
	.xrtr2w_odt_off = 8,
	.xrtr2r_new_mode = 3,
	.xrtr2r_old_mode = 7,
	.tr2mrr = 4,
	.vrcgdis_prdcnt = 47,
	.hwset_mr2_op = 54,
	.hwset_mr13_op = 216,
	.hwset_vrcg_op = 208,
	.trcd_derate = 9,	.trcd_derate_05T = 1,
	.trc_derate = 21,	.trc_derate_05T = 1,
	.tras_derate = 12,	.tras_derate_05T = 0,
	.trpab_derate = 9,	.trpab_derate_05T = 1,
	.trp_derate = 8,	.trp_derate_05T = 0,
	.trrd_derate = 5,	.trrd_derate_05T = 0,
	.trtpd = 14,	.trtpd_05T = 0,
	.twtpd = 17,	.twtpd_05T = 1,
	.tmrr2w_odt_off = 10,
	.tmrr2w_odt_on = 12,
	.ckeprd = 3,
	.ckelckcnt = 3,
	.zqlat2 = 14,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 6,	 .datlat = 16
},
#endif
#endif
#if SUPPORT_LP4_DDR3200_ACTIM
	//LP4_DDR3200 ACTiming---------------------------------
#if (ENABLE_READ_DBI == 1)
//LPDDR4 4X_3200_Div 8_DBI1.csv Read 1
{
	.dramType = TYPE_LPDDR4, .freq = 1600, .cbtMode = CBT_NORMAL_MODE, .readDBI = 1,
	.readLat = 32,	.writeLat =  14,	.DivMode =	DIV8_MODE,

	.tras = 8,	.tras_05T = 1,
	.trp = 6,	.trp_05T = 0,
	.trpab = 7,	.trpab_05T = 0,
	.trc = 15,	.trc_05T = 0,
	.trfc = 100,	.trfc_05T = 0,
	.trfcpb = 44,	.trfcpb_05T = 0,
	.txp = 0,	.txp_05T = 0,
	.trtp = 1,	.trtp_05T = 1,
	.trcd = 7,	.trcd_05T = 1,
	.twr = 12,	.twr_05T = 1,
	.twtr = 7,	.twtr_05T = 0,
	.tpbr2pbr = 29,	.tpbr2pbr_05T = 0,
	.tpbr2act = 0,	.tpbr2act_05T = 0,
	.tr2mrw = 14,	.tr2mrw_05T = 1,
	.tw2mrw = 9,	.tw2mrw_05T = 0,
	.tmrr2mrw = 12,	.tmrr2mrw_05T = 1,
	.tmrw = 4,	.tmrw_05T = 1,
	.tmrd = 6,	.tmrd_05T = 1,
	.tmrwckel = 7,	.tmrwckel_05T = 1,
	.tpde = 1,	.tpde_05T = 1,
	.tpdx = 1,	.tpdx_05T = 0,
	.tmrri = 10,	.tmrri_05T = 1,
	.trrd = 3,	.trrd_05T = 0,
	.trrd_4266 = 2,	.trrd_4266_05T = 0,
	.tfaw = 8,	.tfaw_05T = 0,
	.tfaw_4266 = 4,	.tfaw_4266_05T = 0,
	.trtw_odt_off = 5,	.trtw_odt_off_05T = 0,
	.trtw_odt_on = 7,	.trtw_odt_on_05T = 0,
	.txrefcnt = 115,
	.tzqcs = 34,
	.xrtw2w_new_mode = 4,
	.xrtw2w_old_mode = 6,
	.xrtw2r_odt_on = 1,
	.xrtw2r_odt_off = 1,
	.xrtr2w_odt_on = 6,
	.xrtr2w_odt_off = 6,
	.xrtr2r_new_mode = 3,
	.xrtr2r_old_mode = 7,
	.tr2mrr = 4,
	.vrcgdis_prdcnt = 40,
	.hwset_mr2_op = 45,
	.hwset_mr13_op = 216,
	.hwset_vrcg_op = 208,
	.trcd_derate = 8,	.trcd_derate_05T = 0,
	.trc_derate = 17,	.trc_derate_05T = 0,
	.tras_derate = 9,	.tras_derate_05T = 1,
	.trpab_derate = 8,	.trpab_derate_05T = 0,
	.trp_derate = 6,	.trp_derate_05T = 1,
	.trrd_derate = 4,	.trrd_derate_05T = 0,
	.trtpd = 13,	.trtpd_05T = 0,
	.twtpd = 14,	.twtpd_05T = 1,
	.tmrr2w_odt_off = 9,
	.tmrr2w_odt_on = 11,
	.ckeprd = 2,
	.ckelckcnt = 2,
	.zqlat2 = 12,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 5,	 .datlat = 15
},
//LPDDR4 4X_3200_BT_Div 8_DBI1.csv Read 1
{
	.dramType = TYPE_LPDDR4, .freq = 1600, .cbtMode = CBT_BYTE_MODE1, .readDBI = 1,
	.readLat = 36,	.writeLat =  14,	.DivMode =	DIV8_MODE,

	.tras = 8,	.tras_05T = 1,
	.trp = 6,	.trp_05T = 0,
	.trpab = 7,	.trpab_05T = 0,
	.trc = 15,	.trc_05T = 0,
	.trfc = 100,	.trfc_05T = 0,
	.trfcpb = 44,	.trfcpb_05T = 0,
	.txp = 0,	.txp_05T = 0,
	.trtp = 1,	.trtp_05T = 1,
	.trcd = 7,	.trcd_05T = 1,
	.twr = 12,	.twr_05T = 1,
	.twtr = 8,	.twtr_05T = 0,
	.tpbr2pbr = 29,	.tpbr2pbr_05T = 0,
	.tpbr2act = 0,	.tpbr2act_05T = 0,
	.tr2mrw = 15,	.tr2mrw_05T = 1,
	.tw2mrw = 9,	.tw2mrw_05T = 0,
	.tmrr2mrw = 13,	.tmrr2mrw_05T = 1,
	.tmrw = 4,	.tmrw_05T = 1,
	.tmrd = 6,	.tmrd_05T = 1,
	.tmrwckel = 7,	.tmrwckel_05T = 1,
	.tpde = 1,	.tpde_05T = 1,
	.tpdx = 1,	.tpdx_05T = 0,
	.tmrri = 10,	.tmrri_05T = 1,
	.trrd = 3,	.trrd_05T = 0,
	.trrd_4266 = 2,	.trrd_4266_05T = 0,
	.tfaw = 8,	.tfaw_05T = 0,
	.tfaw_4266 = 4,	.tfaw_4266_05T = 0,
	.trtw_odt_off = 6,	.trtw_odt_off_05T = 0,
	.trtw_odt_on = 8,	.trtw_odt_on_05T = 0,
	.txrefcnt = 115,
	.tzqcs = 34,
	.xrtw2w_new_mode = 4,
	.xrtw2w_old_mode = 6,
	.xrtw2r_odt_on = 1,
	.xrtw2r_odt_off = 1,
	.xrtr2w_odt_on = 7,
	.xrtr2w_odt_off = 7,
	.xrtr2r_new_mode = 3,
	.xrtr2r_old_mode = 7,
	.tr2mrr = 4,
	.vrcgdis_prdcnt = 40,
	.hwset_mr2_op = 45,
	.hwset_mr13_op = 216,
	.hwset_vrcg_op = 208,
	.trcd_derate = 8,	.trcd_derate_05T = 0,
	.trc_derate = 17,	.trc_derate_05T = 0,
	.tras_derate = 9,	.tras_derate_05T = 1,
	.trpab_derate = 8,	.trpab_derate_05T = 0,
	.trp_derate = 6,	.trp_derate_05T = 1,
	.trrd_derate = 4,	.trrd_derate_05T = 0,
	.trtpd = 14,	.trtpd_05T = 0,
	.twtpd = 15,	.twtpd_05T = 1,
	.tmrr2w_odt_off = 10,
	.tmrr2w_odt_on = 12,
	.ckeprd = 2,
	.ckelckcnt = 2,
	.zqlat2 = 12,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 5,	 .datlat = 15
},
#else  //ENABLE_READ_DBI == 0)
//LPDDR4 4X_3200_Div 8_DBI0.csv Read 0
{
	.dramType = TYPE_LPDDR4, .freq = 1600, .cbtMode = CBT_NORMAL_MODE, .readDBI = 0,
	.readLat = 28,	.writeLat =  14,	.DivMode =	DIV8_MODE,

	.tras = 8,	.tras_05T = 1,
	.trp = 6,	.trp_05T = 0,
	.trpab = 7,	.trpab_05T = 0,
	.trc = 15,	.trc_05T = 0,
	.trfc = 100,	.trfc_05T = 0,
	.trfcpb = 44,	.trfcpb_05T = 0,
	.txp = 0,	.txp_05T = 0,
	.trtp = 1,	.trtp_05T = 1,
	.trcd = 7,	.trcd_05T = 1,
	.twr = 12,	.twr_05T = 1,
	.twtr = 7,	.twtr_05T = 0,
	.tpbr2pbr = 29,	.tpbr2pbr_05T = 0,
	.tpbr2act = 0,	.tpbr2act_05T = 0,
	.tr2mrw = 13,	.tr2mrw_05T = 1,
	.tw2mrw = 9,	.tw2mrw_05T = 0,
	.tmrr2mrw = 11,	.tmrr2mrw_05T = 1,
	.tmrw = 4,	.tmrw_05T = 1,
	.tmrd = 6,	.tmrd_05T = 1,
	.tmrwckel = 7,	.tmrwckel_05T = 1,
	.tpde = 1,	.tpde_05T = 1,
	.tpdx = 1,	.tpdx_05T = 0,
	.tmrri = 10,	.tmrri_05T = 1,
	.trrd = 3,	.trrd_05T = 0,
	.trrd_4266 = 2,	.trrd_4266_05T = 0,
	.tfaw = 8,	.tfaw_05T = 0,
	.tfaw_4266 = 4,	.tfaw_4266_05T = 0,
	.trtw_odt_off = 4,	.trtw_odt_off_05T = 0,
	.trtw_odt_on = 6,	.trtw_odt_on_05T = 0,
	.txrefcnt = 115,
	.tzqcs = 34,
	.xrtw2w_new_mode = 4,
	.xrtw2w_old_mode = 6,
	.xrtw2r_odt_on = 1,
	.xrtw2r_odt_off = 1,
	.xrtr2w_odt_on = 5,
	.xrtr2w_odt_off = 5,
	.xrtr2r_new_mode = 3,
	.xrtr2r_old_mode = 7,
	.tr2mrr = 4,
	.vrcgdis_prdcnt = 40,
	.hwset_mr2_op = 45,
	.hwset_mr13_op = 216,
	.hwset_vrcg_op = 208,
	.trcd_derate = 8,	.trcd_derate_05T = 0,
	.trc_derate = 17,	.trc_derate_05T = 0,
	.tras_derate = 9,	.tras_derate_05T = 1,
	.trpab_derate = 8,	.trpab_derate_05T = 0,
	.trp_derate = 6,	.trp_derate_05T = 1,
	.trrd_derate = 4,	.trrd_derate_05T = 0,
	.trtpd = 12,	.trtpd_05T = 0,
	.twtpd = 14,	.twtpd_05T = 1,
	.tmrr2w_odt_off = 8,
	.tmrr2w_odt_on = 10,
	.ckeprd = 2,
	.ckelckcnt = 2,
	.zqlat2 = 12,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 5,	 .datlat = 15
},
//LPDDR4 4X_3200_BT_Div 8_DBI0.csv Read 0
{
	.dramType = TYPE_LPDDR4, .freq = 1600, .cbtMode = CBT_BYTE_MODE1, .readDBI = 0,
	.readLat = 32,	.writeLat =  14,	.DivMode =	DIV8_MODE,

	.tras = 8,	.tras_05T = 1,
	.trp = 6,	.trp_05T = 0,
	.trpab = 7,	.trpab_05T = 0,
	.trc = 15,	.trc_05T = 0,
	.trfc = 100,	.trfc_05T = 0,
	.trfcpb = 44,	.trfcpb_05T = 0,
	.txp = 0,	.txp_05T = 0,
	.trtp = 1,	.trtp_05T = 1,
	.trcd = 7,	.trcd_05T = 1,
	.twr = 12,	.twr_05T = 1,
	.twtr = 8,	.twtr_05T = 0,
	.tpbr2pbr = 29,	.tpbr2pbr_05T = 0,
	.tpbr2act = 0,	.tpbr2act_05T = 0,
	.tr2mrw = 14,	.tr2mrw_05T = 1,
	.tw2mrw = 9,	.tw2mrw_05T = 0,
	.tmrr2mrw = 12,	.tmrr2mrw_05T = 1,
	.tmrw = 4,	.tmrw_05T = 1,
	.tmrd = 6,	.tmrd_05T = 1,
	.tmrwckel = 7,	.tmrwckel_05T = 1,
	.tpde = 1,	.tpde_05T = 1,
	.tpdx = 1,	.tpdx_05T = 0,
	.tmrri = 10,	.tmrri_05T = 1,
	.trrd = 3,	.trrd_05T = 0,
	.trrd_4266 = 2,	.trrd_4266_05T = 0,
	.tfaw = 8,	.tfaw_05T = 0,
	.tfaw_4266 = 4,	.tfaw_4266_05T = 0,
	.trtw_odt_off = 5,	.trtw_odt_off_05T = 0,
	.trtw_odt_on = 7,	.trtw_odt_on_05T = 0,
	.txrefcnt = 115,
	.tzqcs = 34,
	.xrtw2w_new_mode = 4,
	.xrtw2w_old_mode = 6,
	.xrtw2r_odt_on = 1,
	.xrtw2r_odt_off = 1,
	.xrtr2w_odt_on = 6,
	.xrtr2w_odt_off = 6,
	.xrtr2r_new_mode = 3,
	.xrtr2r_old_mode = 7,
	.tr2mrr = 4,
	.vrcgdis_prdcnt = 40,
	.hwset_mr2_op = 45,
	.hwset_mr13_op = 216,
	.hwset_vrcg_op = 208,
	.trcd_derate = 8,	.trcd_derate_05T = 0,
	.trc_derate = 17,	.trc_derate_05T = 0,
	.tras_derate = 9,	.tras_derate_05T = 1,
	.trpab_derate = 8,	.trpab_derate_05T = 0,
	.trp_derate = 6,	.trp_derate_05T = 1,
	.trrd_derate = 4,	.trrd_derate_05T = 0,
	.trtpd = 13,	.trtpd_05T = 0,
	.twtpd = 15,	.twtpd_05T = 1,
	.tmrr2w_odt_off = 9,
	.tmrr2w_odt_on = 11,
	.ckeprd = 2,
	.ckelckcnt = 2,
	.zqlat2 = 12,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 5,	 .datlat = 15
},
#endif
#endif
#if SUPPORT_LP4_DDR2667_ACTIM
	//LP4_DDR2667 ACTiming---------------------------------
//LPDDR4 4X_2667_Div 8_DBI0.csv Read 0
{
	.dramType = TYPE_LPDDR4, .freq = 1333, .cbtMode = CBT_NORMAL_MODE, .readDBI = 0,
	.readLat = 24,	.writeLat =  12,	.DivMode =	DIV8_MODE,

	.tras = 6,	.tras_05T = 0,
	.trp = 5,	.trp_05T = 0,
	.trpab = 6,	.trpab_05T = 0,
	.trc = 11,	.trc_05T = 1,
	.trfc = 81,	.trfc_05T = 1,
	.trfcpb = 35,	.trfcpb_05T = 0,
	.txp = 0,	.txp_05T = 0,
	.trtp = 1,	.trtp_05T = 1,
	.trcd = 6,	.trcd_05T = 1,
	.twr = 10,	.twr_05T = 0,
	.twtr = 6,	.twtr_05T = 0,
	.tpbr2pbr = 23,	.tpbr2pbr_05T = 1,
	.tpbr2act = 0,	.tpbr2act_05T = 0,
	.tr2mrw = 12,	.tr2mrw_05T = 0,
	.tw2mrw = 8,	.tw2mrw_05T = 0,
	.tmrr2mrw = 10,	.tmrr2mrw_05T = 0,
	.tmrw = 4,	.tmrw_05T = 0,
	.tmrd = 5,	.tmrd_05T = 1,
	.tmrwckel = 6,	.tmrwckel_05T = 1,
	.tpde = 1,	.tpde_05T = 1,
	.tpdx = 1,	.tpdx_05T = 0,
	.tmrri = 9,	.tmrri_05T = 0,
	.trrd = 2,	.trrd_05T = 1,
	.trrd_4266 = 2,	.trrd_4266_05T = 0,
	.tfaw = 5,	.tfaw_05T = 1,
	.tfaw_4266 = 2,	.tfaw_4266_05T = 1,
	.trtw_odt_off = 3,	.trtw_odt_off_05T = 0,
	.trtw_odt_on = 6,	.trtw_odt_on_05T = 0,
	.txrefcnt = 96,
	.tzqcs = 29,
	.xrtw2w_new_mode = 4,
	.xrtw2w_old_mode = 6,
	.xrtw2r_odt_on = 1,
	.xrtw2r_odt_off = 1,
	.xrtr2w_odt_on = 5,
	.xrtr2w_odt_off = 5,
	.xrtr2r_new_mode = 3,
	.xrtr2r_old_mode = 6,
	.tr2mrr = 4,
	.vrcgdis_prdcnt = 34,
	.hwset_mr2_op = 36,
	.hwset_mr13_op = 216,
	.hwset_vrcg_op = 208,
	.trcd_derate = 7,	.trcd_derate_05T = 0,
	.trc_derate = 13,	.trc_derate_05T = 0,
	.tras_derate = 6,	.tras_derate_05T = 1,
	.trpab_derate = 6,	.trpab_derate_05T = 1,
	.trp_derate = 5,	.trp_derate_05T = 1,
	.trrd_derate = 3,	.trrd_derate_05T = 0,
	.trtpd = 10,	.trtpd_05T = 1,
	.twtpd = 13,	.twtpd_05T = 0,
	.tmrr2w_odt_off = 6,
	.tmrr2w_odt_on = 8,
	.ckeprd = 2,
	.ckelckcnt = 2,
	.zqlat2 = 11,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = TBD,	 .datlat = TBD
},
//LPDDR4 4X_2667_BT_Div 8_DBI0.csv Read 0
{
	.dramType = TYPE_LPDDR4, .freq = 1333, .cbtMode = CBT_BYTE_MODE1, .readDBI = 0,
	.readLat = 26,	.writeLat =  12,	.DivMode =	DIV8_MODE,

	.tras = 6,	.tras_05T = 0,
	.trp = 5,	.trp_05T = 0,
	.trpab = 6,	.trpab_05T = 0,
	.trc = 11,	.trc_05T = 1,
	.trfc = 81,	.trfc_05T = 1,
	.trfcpb = 35,	.trfcpb_05T = 0,
	.txp = 0,	.txp_05T = 0,
	.trtp = 1,	.trtp_05T = 1,
	.trcd = 6,	.trcd_05T = 1,
	.twr = 11,	.twr_05T = 1,
	.twtr = 7,	.twtr_05T = 1,
	.tpbr2pbr = 23,	.tpbr2pbr_05T = 1,
	.tpbr2act = 0,	.tpbr2act_05T = 0,
	.tr2mrw = 12,	.tr2mrw_05T = 1,
	.tw2mrw = 8,	.tw2mrw_05T = 0,
	.tmrr2mrw = 10,	.tmrr2mrw_05T = 1,
	.tmrw = 4,	.tmrw_05T = 0,
	.tmrd = 5,	.tmrd_05T = 1,
	.tmrwckel = 6,	.tmrwckel_05T = 1,
	.tpde = 1,	.tpde_05T = 1,
	.tpdx = 1,	.tpdx_05T = 0,
	.tmrri = 9,	.tmrri_05T = 0,
	.trrd = 2,	.trrd_05T = 1,
	.trrd_4266 = 2,	.trrd_4266_05T = 0,
	.tfaw = 5,	.tfaw_05T = 1,
	.tfaw_4266 = 2,	.tfaw_4266_05T = 1,
	.trtw_odt_off = 4,	.trtw_odt_off_05T = 0,
	.trtw_odt_on = 6,	.trtw_odt_on_05T = 0,
	.txrefcnt = 96,
	.tzqcs = 29,
	.xrtw2w_new_mode = 4,
	.xrtw2w_old_mode = 6,
	.xrtw2r_odt_on = 1,
	.xrtw2r_odt_off = 1,
	.xrtr2w_odt_on = 5,
	.xrtr2w_odt_off = 5,
	.xrtr2r_new_mode = 3,
	.xrtr2r_old_mode = 6,
	.tr2mrr = 4,
	.vrcgdis_prdcnt = 34,
	.hwset_mr2_op = 36,
	.hwset_mr13_op = 216,
	.hwset_vrcg_op = 208,
	.trcd_derate = 7,	.trcd_derate_05T = 0,
	.trc_derate = 13,	.trc_derate_05T = 0,
	.tras_derate = 6,	.tras_derate_05T = 1,
	.trpab_derate = 6,	.trpab_derate_05T = 1,
	.trp_derate = 5,	.trp_derate_05T = 1,
	.trrd_derate = 3,	.trrd_derate_05T = 0,
	.trtpd = 11,	.trtpd_05T = 0,
	.twtpd = 13,	.twtpd_05T = 1,
	.tmrr2w_odt_off = 7,
	.tmrr2w_odt_on = 9,
	.ckeprd = 2,
	.ckelckcnt = 2,
	.zqlat2 = 11,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = TBD,	 .datlat = TBD
},
#endif
#if SUPPORT_LP4_DDR2400_ACTIM
	//LP4_DDR2400 ACTiming---------------------------------
//LPDDR4 4X_2400_Div 8_DBI0.csv Read 0
{
	.dramType = TYPE_LPDDR4, .freq = 1200, .cbtMode = CBT_NORMAL_MODE, .readDBI = 0,
	.readLat = 24,	.writeLat =  12,	.DivMode =	DIV8_MODE,

	.tras = 4,	.tras_05T = 1,
	.trp = 4,	.trp_05T = 0,
	.trpab = 5,	.trpab_05T = 0,
	.trc = 9,	.trc_05T = 1,
	.trfc = 72,	.trfc_05T = 1,
	.trfcpb = 30,	.trfcpb_05T = 1,
	.txp = 0,	.txp_05T = 1,
	.trtp = 1,	.trtp_05T = 0,
	.trcd = 5,	.trcd_05T = 1,
	.twr = 9,	.twr_05T = 1,
	.twtr = 6,	.twtr_05T = 1,
	.tpbr2pbr = 20,	.tpbr2pbr_05T = 1,
	.tpbr2act = 0,	.tpbr2act_05T = 0,
	.tr2mrw = 12,	.tr2mrw_05T = 0,
	.tw2mrw = 8,	.tw2mrw_05T = 0,
	.tmrr2mrw = 10,	.tmrr2mrw_05T = 0,
	.tmrw = 4,	.tmrw_05T = 0,
	.tmrd = 5,	.tmrd_05T = 0,
	.tmrwckel = 6,	.tmrwckel_05T = 0,
	.tpde = 1,	.tpde_05T = 1,
	.tpdx = 1,	.tpdx_05T = 0,
	.tmrri = 8,	.tmrri_05T = 0,
	.trrd = 2,	.trrd_05T = 1,
	.trrd_4266 = 1,	.trrd_4266_05T = 1,
	.tfaw = 4,	.tfaw_05T = 1,
	.tfaw_4266 = 1,	.tfaw_4266_05T = 1,
	.trtw_odt_off = 3,	.trtw_odt_off_05T = 0,
	.trtw_odt_on = 6,	.trtw_odt_on_05T = 0,
	.txrefcnt = 87,
	.tzqcs = 26,
	.xrtw2w_new_mode = 4,
	.xrtw2w_old_mode = 6,
	.xrtw2r_odt_on = 2,
	.xrtw2r_odt_off = 2,
	.xrtr2w_odt_on = 5,
	.xrtr2w_odt_off = 5,
	.xrtr2r_new_mode = 3,
	.xrtr2r_old_mode = 6,
	.tr2mrr = 4,
	.vrcgdis_prdcnt = 31,
	.hwset_mr2_op = 36,
	.hwset_mr13_op = 24,
	.hwset_vrcg_op = 16,
	.trcd_derate = 6,	.trcd_derate_05T = 0,
	.trc_derate = 10,	.trc_derate_05T = 1,
	.tras_derate = 5,	.tras_derate_05T = 0,
	.trpab_derate = 5,	.trpab_derate_05T = 1,
	.trp_derate = 4,	.trp_derate_05T = 1,
	.trrd_derate = 3,	.trrd_derate_05T = 0,
	.trtpd = 10,	.trtpd_05T = 1,
	.twtpd = 12,	.twtpd_05T = 0,
	.tmrr2w_odt_off = 6,
	.tmrr2w_odt_on = 8,
	.ckeprd = 2,
	.ckelckcnt = 2,
	.zqlat2 = 10,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 4,	 .datlat = 13
},
//LPDDR4 4X_2400_BT_Div 8_DBI0.csv Read 0
{
	.dramType = TYPE_LPDDR4, .freq = 1200, .cbtMode = CBT_BYTE_MODE1, .readDBI = 0,
	.readLat = 26,	.writeLat =  12,	.DivMode =	DIV8_MODE,

	.tras = 4,	.tras_05T = 1,
	.trp = 4,	.trp_05T = 0,
	.trpab = 5,	.trpab_05T = 0,
	.trc = 9,	.trc_05T = 1,
	.trfc = 72,	.trfc_05T = 1,
	.trfcpb = 30,	.trfcpb_05T = 1,
	.txp = 0,	.txp_05T = 1,
	.trtp = 1,	.trtp_05T = 0,
	.trcd = 5,	.trcd_05T = 1,
	.twr = 10,	.twr_05T = 0,
	.twtr = 6,	.twtr_05T = 0,
	.tpbr2pbr = 20,	.tpbr2pbr_05T = 1,
	.tpbr2act = 0,	.tpbr2act_05T = 0,
	.tr2mrw = 12,	.tr2mrw_05T = 1,
	.tw2mrw = 8,	.tw2mrw_05T = 0,
	.tmrr2mrw = 10,	.tmrr2mrw_05T = 1,
	.tmrw = 4,	.tmrw_05T = 0,
	.tmrd = 5,	.tmrd_05T = 0,
	.tmrwckel = 6,	.tmrwckel_05T = 0,
	.tpde = 1,	.tpde_05T = 1,
	.tpdx = 1,	.tpdx_05T = 0,
	.tmrri = 8,	.tmrri_05T = 0,
	.trrd = 2,	.trrd_05T = 1,
	.trrd_4266 = 1,	.trrd_4266_05T = 1,
	.tfaw = 4,	.tfaw_05T = 1,
	.tfaw_4266 = 1,	.tfaw_4266_05T = 1,
	.trtw_odt_off = 4,	.trtw_odt_off_05T = 0,
	.trtw_odt_on = 6,	.trtw_odt_on_05T = 0,
	.txrefcnt = 87,
	.tzqcs = 26,
	.xrtw2w_new_mode = 4,
	.xrtw2w_old_mode = 6,
	.xrtw2r_odt_on = 1,
	.xrtw2r_odt_off = 1,
	.xrtr2w_odt_on = 5,
	.xrtr2w_odt_off = 5,
	.xrtr2r_new_mode = 3,
	.xrtr2r_old_mode = 6,
	.tr2mrr = 4,
	.vrcgdis_prdcnt = 31,
	.hwset_mr2_op = 36,
	.hwset_mr13_op = 24,
	.hwset_vrcg_op = 16,
	.trcd_derate = 6,	.trcd_derate_05T = 0,
	.trc_derate = 10,	.trc_derate_05T = 1,
	.tras_derate = 5,	.tras_derate_05T = 0,
	.trpab_derate = 5,	.trpab_derate_05T = 1,
	.trp_derate = 4,	.trp_derate_05T = 1,
	.trrd_derate = 3,	.trrd_derate_05T = 0,
	.trtpd = 11,	.trtpd_05T = 0,
	.twtpd = 13,	.twtpd_05T = 0,
	.tmrr2w_odt_off = 7,
	.tmrr2w_odt_on = 9,
	.ckeprd = 2,
	.ckelckcnt = 2,
	.zqlat2 = 10,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 4,	 .datlat = 13
},
#endif
#if SUPPORT_LP4_DDR1866_ACTIM
	//LP4_DDR1866 ACTiming---------------------------------
//LPDDR4 4X_1866_Div 8_DBI0.csv Read 0
{
	.dramType = TYPE_LPDDR4, .freq = 933, .cbtMode = CBT_NORMAL_MODE, .readDBI = 0,
	.readLat = 20,	.writeLat =  10,	.DivMode =	DIV8_MODE,

	.tras = 1,	.tras_05T = 1,
	.trp = 3,	.trp_05T = 0,
	.trpab = 3,	.trpab_05T = 1,
	.trc = 5,	.trc_05T = 0,
	.trfc = 53,	.trfc_05T = 1,
	.trfcpb = 21,	.trfcpb_05T = 0,
	.txp = 0,	.txp_05T = 0,
	.trtp = 0,	.trtp_05T = 1,
	.trcd = 4,	.trcd_05T = 1,
	.twr = 8,	.twr_05T = 1,
	.twtr = 5,	.twtr_05T = 1,
	.tpbr2pbr = 14,	.tpbr2pbr_05T = 0,
	.tpbr2act = 0,	.tpbr2act_05T = 0,
	.tr2mrw = 10,	.tr2mrw_05T = 0,
	.tw2mrw = 7,	.tw2mrw_05T = 0,
	.tmrr2mrw = 9,	.tmrr2mrw_05T = 0,
	.tmrw = 3,	.tmrw_05T = 0,
	.tmrd = 4,	.tmrd_05T = 0,
	.tmrwckel = 5,	.tmrwckel_05T = 0,
	.tpde = 1,	.tpde_05T = 1,
	.tpdx = 1,	.tpdx_05T = 0,
	.tmrri = 6,	.tmrri_05T = 0,
	.trrd = 1,	.trrd_05T = 1,
	.trrd_4266 = 1,	.trrd_4266_05T = 0,
	.tfaw = 1,	.tfaw_05T = 1,
	.tfaw_4266 = 0,	.tfaw_4266_05T = 0,
	.trtw_odt_off = 3,	.trtw_odt_off_05T = 0,
	.trtw_odt_on = 5,	.trtw_odt_on_05T = 0,
	.txrefcnt = 68,
	.tzqcs = 19,
	.xrtw2w_new_mode = 4,
	.xrtw2w_old_mode = 6,
	.xrtw2r_odt_on = 2,
	.xrtw2r_odt_off = 2,
	.xrtr2w_odt_on = 3,
	.xrtr2w_odt_off = 3,
	.xrtr2r_new_mode = 3,
	.xrtr2r_old_mode = 6,
	.tr2mrr = 4,
	.vrcgdis_prdcnt = 24,
	.hwset_mr2_op = 27,
	.hwset_mr13_op = 24,
	.hwset_vrcg_op = 16,
	.trcd_derate = 5,	.trcd_derate_05T = 0,
	.trc_derate = 6,	.trc_derate_05T = 1,
	.tras_derate = 2,	.tras_derate_05T = 0,
	.trpab_derate = 4,	.trpab_derate_05T = 0,
	.trp_derate = 3,	.trp_derate_05T = 1,
	.trrd_derate = 2,	.trrd_derate_05T = 0,
	.trtpd = 9,	.trtpd_05T = 1,
	.twtpd = 10,	.twtpd_05T = 1,
	.tmrr2w_odt_off = 5,
	.tmrr2w_odt_on = 7,
	.ckeprd = 1,
	.ckelckcnt = 2,
	.zqlat2 = 7,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 3,	 .datlat = 13
},
//LPDDR4 4X_1866_BT_Div 8_DBI0.csv Read 0
{
	.dramType = TYPE_LPDDR4, .freq = 933, .cbtMode = CBT_BYTE_MODE1, .readDBI = 0,
	.readLat = 22,	.writeLat =  10,	.DivMode =	DIV8_MODE,

	.tras = 1,	.tras_05T = 1,
	.trp = 3,	.trp_05T = 0,
	.trpab = 3,	.trpab_05T = 1,
	.trc = 5,	.trc_05T = 0,
	.trfc = 53,	.trfc_05T = 1,
	.trfcpb = 21,	.trfcpb_05T = 0,
	.txp = 0,	.txp_05T = 0,
	.trtp = 0,	.trtp_05T = 1,
	.trcd = 4,	.trcd_05T = 1,
	.twr = 8,	.twr_05T = 0,
	.twtr = 5,	.twtr_05T = 0,
	.tpbr2pbr = 14,	.tpbr2pbr_05T = 0,
	.tpbr2act = 0,	.tpbr2act_05T = 0,
	.tr2mrw = 10,	.tr2mrw_05T = 1,
	.tw2mrw = 7,	.tw2mrw_05T = 0,
	.tmrr2mrw = 9,	.tmrr2mrw_05T = 1,
	.tmrw = 3,	.tmrw_05T = 0,
	.tmrd = 4,	.tmrd_05T = 0,
	.tmrwckel = 5,	.tmrwckel_05T = 0,
	.tpde = 1,	.tpde_05T = 1,
	.tpdx = 1,	.tpdx_05T = 0,
	.tmrri = 6,	.tmrri_05T = 0,
	.trrd = 1,	.trrd_05T = 1,
	.trrd_4266 = 1,	.trrd_4266_05T = 0,
	.tfaw = 1,	.tfaw_05T = 1,
	.tfaw_4266 = 0,	.tfaw_4266_05T = 0,
	.trtw_odt_off = 3,	.trtw_odt_off_05T = 0,
	.trtw_odt_on = 5,	.trtw_odt_on_05T = 0,
	.txrefcnt = 68,
	.tzqcs = 19,
	.xrtw2w_new_mode = 4,
	.xrtw2w_old_mode = 6,
	.xrtw2r_odt_on = 2,
	.xrtw2r_odt_off = 1,
	.xrtr2w_odt_on = 4,
	.xrtr2w_odt_off = 4,
	.xrtr2r_new_mode = 3,
	.xrtr2r_old_mode = 6,
	.tr2mrr = 4,
	.vrcgdis_prdcnt = 24,
	.hwset_mr2_op = 27,
	.hwset_mr13_op = 24,
	.hwset_vrcg_op = 16,
	.trcd_derate = 5,	.trcd_derate_05T = 0,
	.trc_derate = 6,	.trc_derate_05T = 1,
	.tras_derate = 2,	.tras_derate_05T = 0,
	.trpab_derate = 4,	.trpab_derate_05T = 0,
	.trp_derate = 3,	.trp_derate_05T = 1,
	.trrd_derate = 2,	.trrd_derate_05T = 0,
	.trtpd = 10,	.trtpd_05T = 0,
	.twtpd = 11,	.twtpd_05T = 0,
	.tmrr2w_odt_off = 6,
	.tmrr2w_odt_on = 8,
	.ckeprd = 1,
	.ckelckcnt = 2,
	.zqlat2 = 7,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 3,	 .datlat = 13
},
#endif
#if SUPPORT_LP4_DDR1600_ACTIM
	//LP4_DDR1600 ACTiming---------------------------------
//LPDDR4 4X_1600_Div 4_DBI0.csv Read 0
{
	.dramType = TYPE_LPDDR4, .freq = 800, .cbtMode = CBT_NORMAL_MODE, .readDBI = 0,
	.readLat = 14,	.writeLat =  8,	.DivMode =	DIV4_MODE,

	.tras = 9,	.tras_05T = 0,
	.trp = 6,	.trp_05T = 0,
	.trpab = 7,	.trpab_05T = 0,
	.trc = 15,	.trc_05T = 0,
	.trfc = 100,	.trfc_05T = 0,
	.trfcpb = 44,	.trfcpb_05T = 0,
	.txp = 0,	.txp_05T = 0,
	.trtp = 3,	.trtp_05T = 0,
	.trcd = 8,	.trcd_05T = 0,
	.twr = 15,	.twr_05T = 0,
	.twtr = 10,	.twtr_05T = 0,
	.tpbr2pbr = 29,	.tpbr2pbr_05T = 0,
	.tpbr2act = 0,	.tpbr2act_05T = 0,
	.tr2mrw = 17,	.tr2mrw_05T = 0,
	.tw2mrw = 13,	.tw2mrw_05T = 0,
	.tmrr2mrw = 14,	.tmrr2mrw_05T = 0,
	.tmrw = 6,	.tmrw_05T = 0,
	.tmrd = 7,	.tmrd_05T = 0,
	.tmrwckel = 9,	.tmrwckel_05T = 0,
	.tpde = 3,	.tpde_05T = 0,
	.tpdx = 3,	.tpdx_05T = 0,
	.tmrri = 11,	.tmrri_05T = 0,
	.trrd = 3,	.trrd_05T = 0,
	.trrd_4266 = 2,	.trrd_4266_05T = 0,
	.tfaw = 8,	.tfaw_05T = 0,
	.tfaw_4266 = 4,	.tfaw_4266_05T = 0,
	.trtw_odt_off = 7,	.trtw_odt_off_05T = 0,
	.trtw_odt_on = 11,	.trtw_odt_on_05T = 0,
	.txrefcnt = 115,
	.tzqcs = 34,
	.xrtw2w_new_mode = 9,
	.xrtw2w_old_mode = 10,
	.xrtw2r_odt_on = 7,
	.xrtw2r_odt_off = 6,
	.xrtr2w_odt_on = 10,
	.xrtr2w_odt_off = 10,
	.xrtr2r_new_mode = 7,
	.xrtr2r_old_mode = 9,
	.tr2mrr = 8,
	.vrcgdis_prdcnt = 40,
	.hwset_mr2_op = 18,
	.hwset_mr13_op = 24,
	.hwset_vrcg_op = 16,
	.trcd_derate = 8,	.trcd_derate_05T = 0,
	.trc_derate = 17,	.trc_derate_05T = 0,
	.tras_derate = 10,	.tras_derate_05T = 0,
	.trpab_derate = 8,	.trpab_derate_05T = 0,
	.trp_derate = 6,	.trp_derate_05T = 0,
	.trrd_derate = 4,	.trrd_derate_05T = 0,
	.trtpd = 15,	.trtpd_05T = 0,
	.twtpd = 19,	.twtpd_05T = 0,
	.tmrr2w_odt_off = 10,
	.tmrr2w_odt_on = 12,
	.ckeprd = 2,
	.ckelckcnt = 3,
	.zqlat2 = 12,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 2,	 .datlat = 10
},
//LPDDR4 4X_1600_BT_Div 4_DBI0.csv Read 0
{
	.dramType = TYPE_LPDDR4, .freq = 800, .cbtMode = CBT_BYTE_MODE1, .readDBI = 0,
	.readLat = 16,	.writeLat =  8,	.DivMode =	DIV4_MODE,

	.tras = 9,	.tras_05T = 0,
	.trp = 6,	.trp_05T = 0,
	.trpab = 7,	.trpab_05T = 0,
	.trc = 15,	.trc_05T = 0,
	.trfc = 100,	.trfc_05T = 0,
	.trfcpb = 44,	.trfcpb_05T = 0,
	.txp = 0,	.txp_05T = 0,
	.trtp = 3,	.trtp_05T = 0,
	.trcd = 8,	.trcd_05T = 0,
	.twr = 16,	.twr_05T = 0,
	.twtr = 11,	.twtr_05T = 0,
	.tpbr2pbr = 29,	.tpbr2pbr_05T = 0,
	.tpbr2act = 0,	.tpbr2act_05T = 0,
	.tr2mrw = 18,	.tr2mrw_05T = 0,
	.tw2mrw = 13,	.tw2mrw_05T = 0,
	.tmrr2mrw = 15,	.tmrr2mrw_05T = 0,
	.tmrw = 6,	.tmrw_05T = 0,
	.tmrd = 7,	.tmrd_05T = 0,
	.tmrwckel = 9,	.tmrwckel_05T = 0,
	.tpde = 3,	.tpde_05T = 0,
	.tpdx = 3,	.tpdx_05T = 0,
	.tmrri = 11,	.tmrri_05T = 0,
	.trrd = 3,	.trrd_05T = 0,
	.trrd_4266 = 2,	.trrd_4266_05T = 0,
	.tfaw = 8,	.tfaw_05T = 0,
	.tfaw_4266 = 4,	.tfaw_4266_05T = 0,
	.trtw_odt_off = 8,	.trtw_odt_off_05T = 0,
	.trtw_odt_on = 12,	.trtw_odt_on_05T = 0,
	.txrefcnt = 115,
	.tzqcs = 34,
	.xrtw2w_new_mode = 9,
	.xrtw2w_old_mode = 10,
	.xrtw2r_odt_on = 6,
	.xrtw2r_odt_off = 5,
	.xrtr2w_odt_on = 11,
	.xrtr2w_odt_off = 11,
	.xrtr2r_new_mode = 7,
	.xrtr2r_old_mode = 10,
	.tr2mrr = 8,
	.vrcgdis_prdcnt = 40,
	.hwset_mr2_op = 18,
	.hwset_mr13_op = 24,
	.hwset_vrcg_op = 16,
	.trcd_derate = 8,	.trcd_derate_05T = 0,
	.trc_derate = 17,	.trc_derate_05T = 0,
	.tras_derate = 10,	.tras_derate_05T = 0,
	.trpab_derate = 8,	.trpab_derate_05T = 0,
	.trp_derate = 6,	.trp_derate_05T = 0,
	.trrd_derate = 4,	.trrd_derate_05T = 0,
	.trtpd = 16,	.trtpd_05T = 0,
	.twtpd = 19,	.twtpd_05T = 0,
	.tmrr2w_odt_off = 11,
	.tmrr2w_odt_on = 13,
	.ckeprd = 2,
	.ckelckcnt = 3,
	.zqlat2 = 12,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 2,	 .datlat = 10
},
//LPDDR4 4X_1600_Div 8_DBI0.csv Read 0
{
	.dramType = TYPE_LPDDR4, .freq = 800, .cbtMode = CBT_NORMAL_MODE, .readDBI = 0,
	.readLat = 14,	.writeLat =  8,	.DivMode =	DIV8_MODE,

	.tras = 0,	.tras_05T = 0,
	.trp = 2,	.trp_05T = 1,
	.trpab = 3,	.trpab_05T = 0,
	.trc = 3,	.trc_05T = 0,
	.trfc = 44,	.trfc_05T = 0,
	.trfcpb = 16,	.trfcpb_05T = 0,
	.txp = 0,	.txp_05T = 0,
	.trtp = 0,	.trtp_05T = 1,
	.trcd = 4,	.trcd_05T = 0,
	.twr = 7,	.twr_05T = 1,
	.twtr = 4,	.twtr_05T = 1,
	.tpbr2pbr = 11,	.tpbr2pbr_05T = 0,
	.tpbr2act = 0,	.tpbr2act_05T = 0,
	.tr2mrw = 8,	.tr2mrw_05T = 1,
	.tw2mrw = 6,	.tw2mrw_05T = 1,
	.tmrr2mrw = 7,	.tmrr2mrw_05T = 0,
	.tmrw = 3,	.tmrw_05T = 0,
	.tmrd = 3,	.tmrd_05T = 1,
	.tmrwckel = 4,	.tmrwckel_05T = 1,
	.tpde = 1,	.tpde_05T = 1,
	.tpdx = 1,	.tpdx_05T = 0,
	.tmrri = 5,	.tmrri_05T = 1,
	.trrd = 1,	.trrd_05T = 0,
	.trrd_4266 = 0,	.trrd_4266_05T = 1,
	.tfaw = 0,	.tfaw_05T = 0,
	.tfaw_4266 = 0,	.tfaw_4266_05T = 0,
	.trtw_odt_off = 1,	.trtw_odt_off_05T = 0,
	.trtw_odt_on = 4,	.trtw_odt_on_05T = 0,
	.txrefcnt = 58,
	.tzqcs = 16,
	.xrtw2w_new_mode = 4,
	.xrtw2w_old_mode = 6,
	.xrtw2r_odt_on = 3,
	.xrtw2r_odt_off = 3,
	.xrtr2w_odt_on = 3,
	.xrtr2w_odt_off = 3,
	.xrtr2r_new_mode = 3,
	.xrtr2r_old_mode = 6,
	.tr2mrr = 4,
	.vrcgdis_prdcnt = 20,
	.hwset_mr2_op = 18,
	.hwset_mr13_op = 24,
	.hwset_vrcg_op = 16,
	.trcd_derate = 4,	.trcd_derate_05T = 0,
	.trc_derate = 4,	.trc_derate_05T = 0,
	.tras_derate = 0,	.tras_derate_05T = 1,
	.trpab_derate = 3,	.trpab_derate_05T = 1,
	.trp_derate = 2,	.trp_derate_05T = 1,
	.trrd_derate = 1,	.trrd_derate_05T = 1,
	.trtpd = 7,	.trtpd_05T = 1,
	.twtpd = 9,	.twtpd_05T = 1,
	.tmrr2w_odt_off = 3,
	.tmrr2w_odt_on = 5,
	.ckeprd = 1,
	.ckelckcnt = 2,
	.zqlat2 = 6,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 2,	 .datlat = 10
},
//LPDDR4 4X_1600_BT_Div 8_DBI0.csv Read 0
{
	.dramType = TYPE_LPDDR4, .freq = 800, .cbtMode = CBT_BYTE_MODE1, .readDBI = 0,
	.readLat = 16,	.writeLat =  8,	.DivMode =	DIV8_MODE,

	.tras = 0,	.tras_05T = 0,
	.trp = 2,	.trp_05T = 1,
	.trpab = 3,	.trpab_05T = 0,
	.trc = 3,	.trc_05T = 0,
	.trfc = 44,	.trfc_05T = 0,
	.trfcpb = 16,	.trfcpb_05T = 0,
	.txp = 0,	.txp_05T = 0,
	.trtp = 0,	.trtp_05T = 1,
	.trcd = 4,	.trcd_05T = 0,
	.twr = 7,	.twr_05T = 0,
	.twtr = 4,	.twtr_05T = 0,
	.tpbr2pbr = 11,	.tpbr2pbr_05T = 0,
	.tpbr2act = 0,	.tpbr2act_05T = 0,
	.tr2mrw = 9,	.tr2mrw_05T = 0,
	.tw2mrw = 6,	.tw2mrw_05T = 1,
	.tmrr2mrw = 7,	.tmrr2mrw_05T = 1,
	.tmrw = 3,	.tmrw_05T = 0,
	.tmrd = 3,	.tmrd_05T = 1,
	.tmrwckel = 4,	.tmrwckel_05T = 1,
	.tpde = 1,	.tpde_05T = 1,
	.tpdx = 1,	.tpdx_05T = 0,
	.tmrri = 5,	.tmrri_05T = 1,
	.trrd = 1,	.trrd_05T = 0,
	.trrd_4266 = 0,	.trrd_4266_05T = 1,
	.tfaw = 0,	.tfaw_05T = 0,
	.tfaw_4266 = 0,	.tfaw_4266_05T = 0,
	.trtw_odt_off = 2,	.trtw_odt_off_05T = 0,
	.trtw_odt_on = 4,	.trtw_odt_on_05T = 0,
	.txrefcnt = 58,
	.tzqcs = 16,
	.xrtw2w_new_mode = 4,
	.xrtw2w_old_mode = 6,
	.xrtw2r_odt_on = 3,
	.xrtw2r_odt_off = 2,
	.xrtr2w_odt_on = 3,
	.xrtr2w_odt_off = 3,
	.xrtr2r_new_mode = 3,
	.xrtr2r_old_mode = 6,
	.tr2mrr = 4,
	.vrcgdis_prdcnt = 20,
	.hwset_mr2_op = 18,
	.hwset_mr13_op = 24,
	.hwset_vrcg_op = 16,
	.trcd_derate = 4,	.trcd_derate_05T = 0,
	.trc_derate = 4,	.trc_derate_05T = 0,
	.tras_derate = 0,	.tras_derate_05T = 1,
	.trpab_derate = 3,	.trpab_derate_05T = 1,
	.trp_derate = 2,	.trp_derate_05T = 1,
	.trrd_derate = 1,	.trrd_derate_05T = 1,
	.trtpd = 8,	.trtpd_05T = 0,
	.twtpd = 9,	.twtpd_05T = 1,
	.tmrr2w_odt_off = 4,
	.tmrr2w_odt_on = 6,
	.ckeprd = 1,
	.ckelckcnt = 2,
	.zqlat2 = 6,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 2,	 .datlat = 10
},
#endif
#if SUPPORT_LP4_DDR1333_ACTIM
	//LP4_DDR1333 ACTiming---------------------------------
//LPDDR4 4X_1333_Div 4_DBI0.csv Read 0
{
	.dramType = TYPE_LPDDR4, .freq = 666, .cbtMode = CBT_NORMAL_MODE, .readDBI = 0,
	.readLat = 14,	.writeLat =  8,	.DivMode =	DIV4_MODE,

	.tras = 6,	.tras_05T = 0,
	.trp = 4,	.trp_05T = 0,
	.trpab = 5,	.trpab_05T = 0,
	.trc = 11,	.trc_05T = 0,
	.trfc = 82,	.trfc_05T = 0,
	.trfcpb = 35,	.trfcpb_05T = 0,
	.txp = 0,	.txp_05T = 0,
	.trtp = 3,	.trtp_05T = 0,
	.trcd = 6,	.trcd_05T = 0,
	.twr = 14,	.twr_05T = 0,
	.twtr = 10,	.twtr_05T = 0,
	.tpbr2pbr = 23,	.tpbr2pbr_05T = 0,
	.tpbr2act = 0,	.tpbr2act_05T = 0,
	.tr2mrw = 17,	.tr2mrw_05T = 0,
	.tw2mrw = 13,	.tw2mrw_05T = 0,
	.tmrr2mrw = 14,	.tmrr2mrw_05T = 0,
	.tmrw = 6,	.tmrw_05T = 0,
	.tmrd = 6,	.tmrd_05T = 0,
	.tmrwckel = 8,	.tmrwckel_05T = 0,
	.tpde = 3,	.tpde_05T = 0,
	.tpdx = 3,	.tpdx_05T = 0,
	.tmrri = 9,	.tmrri_05T = 0,
	.trrd = 3,	.trrd_05T = 0,
	.trrd_4266 = 2,	.trrd_4266_05T = 0,
	.tfaw = 6,	.tfaw_05T = 0,
	.tfaw_4266 = 2,	.tfaw_4266_05T = 0,
	.trtw_odt_off = 7,	.trtw_odt_off_05T = 0,
	.trtw_odt_on = 11,	.trtw_odt_on_05T = 0,
	.txrefcnt = 96,
	.tzqcs = 28,
	.xrtw2w_new_mode = 9,
	.xrtw2w_old_mode = 10,
	.xrtw2r_odt_on = 7,
	.xrtw2r_odt_off = 6,
	.xrtr2w_odt_on = 10,
	.xrtr2w_odt_off = 10,
	.xrtr2r_new_mode = 7,
	.xrtr2r_old_mode = 9,
	.tr2mrr = 8,
	.vrcgdis_prdcnt = 34,
	.hwset_mr2_op = 18,
	.hwset_mr13_op = 24,
	.hwset_vrcg_op = 16,
	.trcd_derate = 7,	.trcd_derate_05T = 0,
	.trc_derate = 13,	.trc_derate_05T = 0,
	.tras_derate = 7,	.tras_derate_05T = 0,
	.trpab_derate = 6,	.trpab_derate_05T = 0,
	.trp_derate = 5,	.trp_derate_05T = 0,
	.trrd_derate = 3,	.trrd_derate_05T = 0,
	.trtpd = 15,	.trtpd_05T = 0,
	.twtpd = 17,	.twtpd_05T = 0,
	.tmrr2w_odt_off = 10,
	.tmrr2w_odt_on = 12,
	.ckeprd = 2,
	.ckelckcnt = 3,
	.zqlat2 = 10,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = TBD,	 .datlat = TBD
},
//LPDDR4 4X_1333_BT_Div 4_DBI0.csv Read 0
{
	.dramType = TYPE_LPDDR4, .freq = 666, .cbtMode = CBT_BYTE_MODE1, .readDBI = 0,
	.readLat = 16,	.writeLat =  8,	.DivMode =	DIV4_MODE,

	.tras = 6,	.tras_05T = 0,
	.trp = 4,	.trp_05T = 0,
	.trpab = 5,	.trpab_05T = 0,
	.trc = 11,	.trc_05T = 0,
	.trfc = 82,	.trfc_05T = 0,
	.trfcpb = 35,	.trfcpb_05T = 0,
	.txp = 0,	.txp_05T = 0,
	.trtp = 3,	.trtp_05T = 0,
	.trcd = 6,	.trcd_05T = 0,
	.twr = 15,	.twr_05T = 0,
	.twtr = 10,	.twtr_05T = 0,
	.tpbr2pbr = 23,	.tpbr2pbr_05T = 0,
	.tpbr2act = 0,	.tpbr2act_05T = 0,
	.tr2mrw = 18,	.tr2mrw_05T = 0,
	.tw2mrw = 13,	.tw2mrw_05T = 0,
	.tmrr2mrw = 15,	.tmrr2mrw_05T = 0,
	.tmrw = 6,	.tmrw_05T = 0,
	.tmrd = 6,	.tmrd_05T = 0,
	.tmrwckel = 8,	.tmrwckel_05T = 0,
	.tpde = 3,	.tpde_05T = 0,
	.tpdx = 3,	.tpdx_05T = 0,
	.tmrri = 9,	.tmrri_05T = 0,
	.trrd = 3,	.trrd_05T = 0,
	.trrd_4266 = 2,	.trrd_4266_05T = 0,
	.tfaw = 6,	.tfaw_05T = 0,
	.tfaw_4266 = 2,	.tfaw_4266_05T = 0,
	.trtw_odt_off = 8,	.trtw_odt_off_05T = 0,
	.trtw_odt_on = 12,	.trtw_odt_on_05T = 0,
	.txrefcnt = 96,
	.tzqcs = 28,
	.xrtw2w_new_mode = 9,
	.xrtw2w_old_mode = 10,
	.xrtw2r_odt_on = 6,
	.xrtw2r_odt_off = 5,
	.xrtr2w_odt_on = 11,
	.xrtr2w_odt_off = 11,
	.xrtr2r_new_mode = 7,
	.xrtr2r_old_mode = 10,
	.tr2mrr = 8,
	.vrcgdis_prdcnt = 34,
	.hwset_mr2_op = 18,
	.hwset_mr13_op = 24,
	.hwset_vrcg_op = 16,
	.trcd_derate = 7,	.trcd_derate_05T = 0,
	.trc_derate = 13,	.trc_derate_05T = 0,
	.tras_derate = 7,	.tras_derate_05T = 0,
	.trpab_derate = 6,	.trpab_derate_05T = 0,
	.trp_derate = 5,	.trp_derate_05T = 0,
	.trrd_derate = 3,	.trrd_derate_05T = 0,
	.trtpd = 16,	.trtpd_05T = 0,
	.twtpd = 18,	.twtpd_05T = 0,
	.tmrr2w_odt_off = 11,
	.tmrr2w_odt_on = 13,
	.ckeprd = 2,
	.ckelckcnt = 3,
	.zqlat2 = 10,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = TBD,	 .datlat = TBD
},
//LPDDR4 4X_1333_Div 8_DBI0.csv Read 0
{
	.dramType = TYPE_LPDDR4, .freq = 666, .cbtMode = CBT_NORMAL_MODE, .readDBI = 0,
	.readLat = 14,	.writeLat =  8,	.DivMode =	DIV8_MODE,

	.tras = 0,	.tras_05T = 0,
	.trp = 1,	.trp_05T = 1,
	.trpab = 2,	.trpab_05T = 0,
	.trc = 1,	.trc_05T = 0,
	.trfc = 35,	.trfc_05T = 0,
	.trfcpb = 11,	.trfcpb_05T = 1,
	.txp = 0,	.txp_05T = 0,
	.trtp = 0,	.trtp_05T = 1,
	.trcd = 3,	.trcd_05T = 0,
	.twr = 6,	.twr_05T = 0,
	.twtr = 4,	.twtr_05T = 1,
	.tpbr2pbr = 8,	.tpbr2pbr_05T = 0,
	.tpbr2act = 0,	.tpbr2act_05T = 0,
	.tr2mrw = 8,	.tr2mrw_05T = 1,
	.tw2mrw = 6,	.tw2mrw_05T = 1,
	.tmrr2mrw = 7,	.tmrr2mrw_05T = 0,
	.tmrw = 3,	.tmrw_05T = 0,
	.tmrd = 3,	.tmrd_05T = 0,
	.tmrwckel = 4,	.tmrwckel_05T = 0,
	.tpde = 1,	.tpde_05T = 1,
	.tpdx = 1,	.tpdx_05T = 0,
	.tmrri = 4,	.tmrri_05T = 1,
	.trrd = 1,	.trrd_05T = 0,
	.trrd_4266 = 0,	.trrd_4266_05T = 1,
	.tfaw = 0,	.tfaw_05T = 0,
	.tfaw_4266 = 0,	.tfaw_4266_05T = 0,
	.trtw_odt_off = 1,	.trtw_odt_off_05T = 0,
	.trtw_odt_on = 4,	.trtw_odt_on_05T = 0,
	.txrefcnt = 48,
	.tzqcs = 13,
	.xrtw2w_new_mode = 4,
	.xrtw2w_old_mode = 6,
	.xrtw2r_odt_on = 3,
	.xrtw2r_odt_off = 3,
	.xrtr2w_odt_on = 3,
	.xrtr2w_odt_off = 3,
	.xrtr2r_new_mode = 3,
	.xrtr2r_old_mode = 6,
	.tr2mrr = 4,
	.vrcgdis_prdcnt = 17,
	.hwset_mr2_op = 18,
	.hwset_mr13_op = 24,
	.hwset_vrcg_op = 16,
	.trcd_derate = 3,	.trcd_derate_05T = 1,
	.trc_derate = 2,	.trc_derate_05T = 0,
	.tras_derate = 0,	.tras_derate_05T = 0,
	.trpab_derate = 2,	.trpab_derate_05T = 1,
	.trp_derate = 2,	.trp_derate_05T = 0,
	.trrd_derate = 1,	.trrd_derate_05T = 0,
	.trtpd = 7,	.trtpd_05T = 1,
	.twtpd = 8,	.twtpd_05T = 1,
	.tmrr2w_odt_off = 3,
	.tmrr2w_odt_on = 5,
	.ckeprd = 1,
	.ckelckcnt = 2,
	.zqlat2 = 5,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = TBD,	 .datlat = TBD
},
//LPDDR4 4X_1333_BT_Div 8_DBI0.csv Read 0
{
	.dramType = TYPE_LPDDR4, .freq = 666, .cbtMode = CBT_BYTE_MODE1, .readDBI = 0,
	.readLat = 16,	.writeLat =  8,	.DivMode =	DIV8_MODE,

	.tras = 0,	.tras_05T = 0,
	.trp = 1,	.trp_05T = 1,
	.trpab = 2,	.trpab_05T = 0,
	.trc = 1,	.trc_05T = 0,
	.trfc = 35,	.trfc_05T = 0,
	.trfcpb = 11,	.trfcpb_05T = 1,
	.txp = 0,	.txp_05T = 0,
	.trtp = 0,	.trtp_05T = 1,
	.trcd = 3,	.trcd_05T = 0,
	.twr = 6,	.twr_05T = 1,
	.twtr = 4,	.twtr_05T = 1,
	.tpbr2pbr = 8,	.tpbr2pbr_05T = 0,
	.tpbr2act = 0,	.tpbr2act_05T = 0,
	.tr2mrw = 9,	.tr2mrw_05T = 0,
	.tw2mrw = 6,	.tw2mrw_05T = 1,
	.tmrr2mrw = 7,	.tmrr2mrw_05T = 1,
	.tmrw = 3,	.tmrw_05T = 0,
	.tmrd = 3,	.tmrd_05T = 0,
	.tmrwckel = 4,	.tmrwckel_05T = 0,
	.tpde = 1,	.tpde_05T = 1,
	.tpdx = 1,	.tpdx_05T = 0,
	.tmrri = 4,	.tmrri_05T = 1,
	.trrd = 1,	.trrd_05T = 0,
	.trrd_4266 = 0,	.trrd_4266_05T = 1,
	.tfaw = 0,	.tfaw_05T = 0,
	.tfaw_4266 = 0,	.tfaw_4266_05T = 0,
	.trtw_odt_off = 2,	.trtw_odt_off_05T = 0,
	.trtw_odt_on = 5,	.trtw_odt_on_05T = 0,
	.txrefcnt = 48,
	.tzqcs = 13,
	.xrtw2w_new_mode = 4,
	.xrtw2w_old_mode = 6,
	.xrtw2r_odt_on = 3,
	.xrtw2r_odt_off = 2,
	.xrtr2w_odt_on = 3,
	.xrtr2w_odt_off = 3,
	.xrtr2r_new_mode = 3,
	.xrtr2r_old_mode = 6,
	.tr2mrr = 4,
	.vrcgdis_prdcnt = 17,
	.hwset_mr2_op = 18,
	.hwset_mr13_op = 24,
	.hwset_vrcg_op = 16,
	.trcd_derate = 3,	.trcd_derate_05T = 1,
	.trc_derate = 2,	.trc_derate_05T = 0,
	.tras_derate = 0,	.tras_derate_05T = 0,
	.trpab_derate = 2,	.trpab_derate_05T = 1,
	.trp_derate = 2,	.trp_derate_05T = 0,
	.trrd_derate = 1,	.trrd_derate_05T = 0,
	.trtpd = 8,	.trtpd_05T = 0,
	.twtpd = 9,	.twtpd_05T = 0,
	.tmrr2w_odt_off = 4,
	.tmrr2w_odt_on = 6,
	.ckeprd = 1,
	.ckelckcnt = 2,
	.zqlat2 = 5,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = TBD,	 .datlat = TBD
},
#endif
#if SUPPORT_LP4_DDR1200_ACTIM
	//LP4_DDR1200 ACTiming---------------------------------
//LPDDR4 4X_1200_Div 4_DBI0.csv Read 0
{
	.dramType = TYPE_LPDDR4, .freq = 600, .cbtMode = CBT_NORMAL_MODE, .readDBI = 0,
	.readLat = 14,	.writeLat =  8,	.DivMode =	DIV4_MODE,

	.tras = 5,	.tras_05T = 0,
	.trp = 4,	.trp_05T = 0,
	.trpab = 5,	.trpab_05T = 0,
	.trc = 10,	.trc_05T = 0,
	.trfc = 73,	.trfc_05T = 0,
	.trfcpb = 31,	.trfcpb_05T = 0,
	.txp = 0,	.txp_05T = 0,
	.trtp = 3,	.trtp_05T = 0,
	.trcd = 6,	.trcd_05T = 0,
	.twr = 13,	.twr_05T = 0,
	.twtr = 10,	.twtr_05T = 0,
	.tpbr2pbr = 21,	.tpbr2pbr_05T = 0,
	.tpbr2act = 0,	.tpbr2act_05T = 0,
	.tr2mrw = 17,	.tr2mrw_05T = 0,
	.tw2mrw = 13,	.tw2mrw_05T = 0,
	.tmrr2mrw = 14,	.tmrr2mrw_05T = 0,
	.tmrw = 6,	.tmrw_05T = 0,
	.tmrd = 6,	.tmrd_05T = 0,
	.tmrwckel = 8,	.tmrwckel_05T = 0,
	.tpde = 3,	.tpde_05T = 0,
	.tpdx = 3,	.tpdx_05T = 0,
	.tmrri = 8,	.tmrri_05T = 0,
	.trrd = 3,	.trrd_05T = 0,
	.trrd_4266 = 2,	.trrd_4266_05T = 0,
	.tfaw = 5,	.tfaw_05T = 0,
	.tfaw_4266 = 2,	.tfaw_4266_05T = 0,
	.trtw_odt_off = 7,	.trtw_odt_off_05T = 0,
	.trtw_odt_on = 11,	.trtw_odt_on_05T = 0,
	.txrefcnt = 87,
	.tzqcs = 26,
	.xrtw2w_new_mode = 9,
	.xrtw2w_old_mode = 10,
	.xrtw2r_odt_on = 7,
	.xrtw2r_odt_off = 6,
	.xrtr2w_odt_on = 10,
	.xrtr2w_odt_off = 10,
	.xrtr2r_new_mode = 7,
	.xrtr2r_old_mode = 9,
	.tr2mrr = 8,
	.vrcgdis_prdcnt = 31,
	.hwset_mr2_op = 18,
	.hwset_mr13_op = 24,
	.hwset_vrcg_op = 16,
	.trcd_derate = 6,	.trcd_derate_05T = 0,
	.trc_derate = 11,	.trc_derate_05T = 0,
	.tras_derate = 6,	.tras_derate_05T = 0,
	.trpab_derate = 5,	.trpab_derate_05T = 0,
	.trp_derate = 4,	.trp_derate_05T = 0,
	.trrd_derate = 3,	.trrd_derate_05T = 0,
	.trtpd = 15,	.trtpd_05T = 0,
	.twtpd = 17,	.twtpd_05T = 0,
	.tmrr2w_odt_off = 10,
	.tmrr2w_odt_on = 12,
	.ckeprd = 2,
	.ckelckcnt = 3,
	.zqlat2 = 10,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 2,	 .datlat = 9
},
//LPDDR4 4X_1200_BT_Div 4_DBI0.csv Read 0
{
	.dramType = TYPE_LPDDR4, .freq = 600, .cbtMode = CBT_BYTE_MODE1, .readDBI = 0,
	.readLat = 16,	.writeLat =  8,	.DivMode =	DIV4_MODE,

	.tras = 5,	.tras_05T = 0,
	.trp = 4,	.trp_05T = 0,
	.trpab = 5,	.trpab_05T = 0,
	.trc = 10,	.trc_05T = 0,
	.trfc = 73,	.trfc_05T = 0,
	.trfcpb = 31,	.trfcpb_05T = 0,
	.txp = 0,	.txp_05T = 0,
	.trtp = 3,	.trtp_05T = 0,
	.trcd = 6,	.trcd_05T = 0,
	.twr = 14,	.twr_05T = 0,
	.twtr = 10,	.twtr_05T = 0,
	.tpbr2pbr = 21,	.tpbr2pbr_05T = 0,
	.tpbr2act = 0,	.tpbr2act_05T = 0,
	.tr2mrw = 18,	.tr2mrw_05T = 0,
	.tw2mrw = 13,	.tw2mrw_05T = 0,
	.tmrr2mrw = 15,	.tmrr2mrw_05T = 0,
	.tmrw = 6,	.tmrw_05T = 0,
	.tmrd = 6,	.tmrd_05T = 0,
	.tmrwckel = 8,	.tmrwckel_05T = 0,
	.tpde = 3,	.tpde_05T = 0,
	.tpdx = 3,	.tpdx_05T = 0,
	.tmrri = 8,	.tmrri_05T = 0,
	.trrd = 3,	.trrd_05T = 0,
	.trrd_4266 = 2,	.trrd_4266_05T = 0,
	.tfaw = 5,	.tfaw_05T = 0,
	.tfaw_4266 = 2,	.tfaw_4266_05T = 0,
	.trtw_odt_off = 8,	.trtw_odt_off_05T = 0,
	.trtw_odt_on = 12,	.trtw_odt_on_05T = 0,
	.txrefcnt = 87,
	.tzqcs = 26,
	.xrtw2w_new_mode = 9,
	.xrtw2w_old_mode = 10,
	.xrtw2r_odt_on = 6,
	.xrtw2r_odt_off = 5,
	.xrtr2w_odt_on = 11,
	.xrtr2w_odt_off = 11,
	.xrtr2r_new_mode = 7,
	.xrtr2r_old_mode = 10,
	.tr2mrr = 8,
	.vrcgdis_prdcnt = 31,
	.hwset_mr2_op = 18,
	.hwset_mr13_op = 24,
	.hwset_vrcg_op = 16,
	.trcd_derate = 6,	.trcd_derate_05T = 0,
	.trc_derate = 11,	.trc_derate_05T = 0,
	.tras_derate = 6,	.tras_derate_05T = 0,
	.trpab_derate = 5,	.trpab_derate_05T = 0,
	.trp_derate = 4,	.trp_derate_05T = 0,
	.trrd_derate = 3,	.trrd_derate_05T = 0,
	.trtpd = 16,	.trtpd_05T = 0,
	.twtpd = 18,	.twtpd_05T = 0,
	.tmrr2w_odt_off = 11,
	.tmrr2w_odt_on = 13,
	.ckeprd = 2,
	.ckelckcnt = 3,
	.zqlat2 = 10,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 2,	 .datlat = 9
},
//LPDDR4 4X_1200_Div 8_DBI0.csv Read 0
{
	.dramType = TYPE_LPDDR4, .freq = 600, .cbtMode = CBT_NORMAL_MODE, .readDBI = 0,
	.readLat = 14,	.writeLat =  8,	.DivMode =	DIV8_MODE,

	.tras = 0,	.tras_05T = 0,
	.trp = 1,	.trp_05T = 1,
	.trpab = 2,	.trpab_05T = 0,
	.trc = 0,	.trc_05T = 1,
	.trfc = 30,	.trfc_05T = 1,
	.trfcpb = 9,	.trfcpb_05T = 1,
	.txp = 0,	.txp_05T = 0,
	.trtp = 0,	.trtp_05T = 1,
	.trcd = 3,	.trcd_05T = 0,
	.twr = 6,	.twr_05T = 1,
	.twtr = 4,	.twtr_05T = 1,
	.tpbr2pbr = 7,	.tpbr2pbr_05T = 0,
	.tpbr2act = 0,	.tpbr2act_05T = 0,
	.tr2mrw = 8,	.tr2mrw_05T = 1,
	.tw2mrw = 6,	.tw2mrw_05T = 1,
	.tmrr2mrw = 7,	.tmrr2mrw_05T = 0,
	.tmrw = 3,	.tmrw_05T = 0,
	.tmrd = 3,	.tmrd_05T = 0,
	.tmrwckel = 4,	.tmrwckel_05T = 0,
	.tpde = 1,	.tpde_05T = 1,
	.tpdx = 1,	.tpdx_05T = 0,
	.tmrri = 4,	.tmrri_05T = 0,
	.trrd = 1,	.trrd_05T = 0,
	.trrd_4266 = 0,	.trrd_4266_05T = 1,
	.tfaw = 0,	.tfaw_05T = 0,
	.tfaw_4266 = 0,	.tfaw_4266_05T = 0,
	.trtw_odt_off = 1,	.trtw_odt_off_05T = 0,
	.trtw_odt_on = 4,	.trtw_odt_on_05T = 0,
	.txrefcnt = 44,
	.tzqcs = 12,
	.xrtw2w_new_mode = 4,
	.xrtw2w_old_mode = 6,
	.xrtw2r_odt_on = 3,
	.xrtw2r_odt_off = 3,
	.xrtr2w_odt_on = 3,
	.xrtr2w_odt_off = 3,
	.xrtr2r_new_mode = 3,
	.xrtr2r_old_mode = 6,
	.tr2mrr = 4,
	.vrcgdis_prdcnt = 16,
	.hwset_mr2_op = 18,
	.hwset_mr13_op = 24,
	.hwset_vrcg_op = 16,
	.trcd_derate = 3,	.trcd_derate_05T = 0,
	.trc_derate = 1,	.trc_derate_05T = 0,
	.tras_derate = 0,	.tras_derate_05T = 0,
	.trpab_derate = 2,	.trpab_derate_05T = 0,
	.trp_derate = 1,	.trp_derate_05T = 1,
	.trrd_derate = 1,	.trrd_derate_05T = 0,
	.trtpd = 7,	.trtpd_05T = 1,
	.twtpd = 8,	.twtpd_05T = 1,
	.tmrr2w_odt_off = 3,
	.tmrr2w_odt_on = 5,
	.ckeprd = 1,
	.ckelckcnt = 2,
	.zqlat2 = 5,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 2,	 .datlat = 9
},
//LPDDR4 4X_1200_BT_Div 8_DBI0.csv Read 0
{
	.dramType = TYPE_LPDDR4, .freq = 600, .cbtMode = CBT_BYTE_MODE1, .readDBI = 0,
	.readLat = 16,	.writeLat =  8,	.DivMode =	DIV8_MODE,

	.tras = 0,	.tras_05T = 0,
	.trp = 1,	.trp_05T = 1,
	.trpab = 2,	.trpab_05T = 0,
	.trc = 0,	.trc_05T = 1,
	.trfc = 30,	.trfc_05T = 1,
	.trfcpb = 9,	.trfcpb_05T = 1,
	.txp = 0,	.txp_05T = 0,
	.trtp = 0,	.trtp_05T = 1,
	.trcd = 3,	.trcd_05T = 0,
	.twr = 6,	.twr_05T = 0,
	.twtr = 4,	.twtr_05T = 1,
	.tpbr2pbr = 7,	.tpbr2pbr_05T = 0,
	.tpbr2act = 0,	.tpbr2act_05T = 0,
	.tr2mrw = 9,	.tr2mrw_05T = 0,
	.tw2mrw = 6,	.tw2mrw_05T = 1,
	.tmrr2mrw = 7,	.tmrr2mrw_05T = 1,
	.tmrw = 3,	.tmrw_05T = 0,
	.tmrd = 3,	.tmrd_05T = 0,
	.tmrwckel = 4,	.tmrwckel_05T = 0,
	.tpde = 1,	.tpde_05T = 1,
	.tpdx = 1,	.tpdx_05T = 0,
	.tmrri = 4,	.tmrri_05T = 0,
	.trrd = 1,	.trrd_05T = 0,
	.trrd_4266 = 0,	.trrd_4266_05T = 1,
	.tfaw = 0,	.tfaw_05T = 0,
	.tfaw_4266 = 0,	.tfaw_4266_05T = 0,
	.trtw_odt_off = 2,	.trtw_odt_off_05T = 0,
	.trtw_odt_on = 5,	.trtw_odt_on_05T = 0,
	.txrefcnt = 44,
	.tzqcs = 12,
	.xrtw2w_new_mode = 4,
	.xrtw2w_old_mode = 6,
	.xrtw2r_odt_on = 3,
	.xrtw2r_odt_off = 2,
	.xrtr2w_odt_on = 3,
	.xrtr2w_odt_off = 3,
	.xrtr2r_new_mode = 3,
	.xrtr2r_old_mode = 6,
	.tr2mrr = 4,
	.vrcgdis_prdcnt = 16,
	.hwset_mr2_op = 18,
	.hwset_mr13_op = 24,
	.hwset_vrcg_op = 16,
	.trcd_derate = 3,	.trcd_derate_05T = 0,
	.trc_derate = 1,	.trc_derate_05T = 0,
	.tras_derate = 0,	.tras_derate_05T = 0,
	.trpab_derate = 2,	.trpab_derate_05T = 0,
	.trp_derate = 1,	.trp_derate_05T = 1,
	.trrd_derate = 1,	.trrd_derate_05T = 0,
	.trtpd = 8,	.trtpd_05T = 0,
	.twtpd = 9,	.twtpd_05T = 0,
	.tmrr2w_odt_off = 4,
	.tmrr2w_odt_on = 6,
	.ckeprd = 1,
	.ckelckcnt = 2,
	.zqlat2 = 5,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 2,	 .datlat = 9
},
#endif
#if SUPPORT_LP4_DDR800_ACTIM
	//LP4_DDR800 ACTiming---------------------------------
//LPDDR4 4X_800_Div 4_DBI0.csv Read 0
{
	.dramType = TYPE_LPDDR4, .freq = 400, .cbtMode = CBT_NORMAL_MODE, .readDBI = 0,
	.readLat = 14,	.writeLat =  8,	.DivMode =	DIV4_MODE,

	.tras = 1,	.tras_05T = 0,
	.trp = 2,	.trp_05T = 0,
	.trpab = 3,	.trpab_05T = 0,
	.trc = 3,	.trc_05T = 0,
	.trfc = 44,	.trfc_05T = 0,
	.trfcpb = 16,	.trfcpb_05T = 0,
	.txp = 0,	.txp_05T = 0,
	.trtp = 3,	.trtp_05T = 0,
	.trcd = 4,	.trcd_05T = 0,
	.twr = 12,	.twr_05T = 0,
	.twtr = 10,	.twtr_05T = 0,
	.tpbr2pbr = 11,	.tpbr2pbr_05T = 0,
	.tpbr2act = 0,	.tpbr2act_05T = 0,
	.tr2mrw = 16,	.tr2mrw_05T = 0,
	.tw2mrw = 13,	.tw2mrw_05T = 0,
	.tmrr2mrw = 14,	.tmrr2mrw_05T = 0,
	.tmrw = 6,	.tmrw_05T = 0,
	.tmrd = 6,	.tmrd_05T = 0,
	.tmrwckel = 8,	.tmrwckel_05T = 0,
	.tpde = 3,	.tpde_05T = 0,
	.tpdx = 3,	.tpdx_05T = 0,
	.tmrri = 7,	.tmrri_05T = 0,
	.trrd = 1,	.trrd_05T = 0,
	.trrd_4266 = 1,	.trrd_4266_05T = 0,
	.tfaw = 0,	.tfaw_05T = 0,
	.tfaw_4266 = 0,	.tfaw_4266_05T = 0,
	.trtw_odt_off = 6,	.trtw_odt_off_05T = 0,
	.trtw_odt_on = 11,	.trtw_odt_on_05T = 0,
	.txrefcnt = 58,
	.tzqcs = 16,
	.xrtw2w_new_mode = 9,
	.xrtw2w_old_mode = 10,
	.xrtw2r_odt_on = 7,
	.xrtw2r_odt_off = 5,
	.xrtr2w_odt_on = 9,
	.xrtr2w_odt_off = 9,
	.xrtr2r_new_mode = 6,
	.xrtr2r_old_mode = 8,
	.tr2mrr = 8,
	.vrcgdis_prdcnt = 20,
	.hwset_mr2_op = 18,
	.hwset_mr13_op = 24,
	.hwset_vrcg_op = 16,
	.trcd_derate = 4,	.trcd_derate_05T = 0,
	.trc_derate = 4,	.trc_derate_05T = 0,
	.tras_derate = 1,	.tras_derate_05T = 0,
	.trpab_derate = 3,	.trpab_derate_05T = 0,
	.trp_derate = 2,	.trp_derate_05T = 0,
	.trrd_derate = 2,	.trrd_derate_05T = 0,
	.trtpd = 15,	.trtpd_05T = 0,
	.twtpd = 15,	.twtpd_05T = 0,
	.tmrr2w_odt_off = 10,
	.tmrr2w_odt_on = 12,
	.ckeprd = 2,
	.ckelckcnt = 3,
	.zqlat2 = 6,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 5,	 .datlat = 15
},
//LPDDR4 4X_800_BT_Div 4_DBI0.csv Read 0
{
	.dramType = TYPE_LPDDR4, .freq = 400, .cbtMode = CBT_BYTE_MODE1, .readDBI = 0,
	.readLat = 16,	.writeLat =  8,	.DivMode =	DIV4_MODE,

	.tras = 1,	.tras_05T = 0,
	.trp = 2,	.trp_05T = 0,
	.trpab = 3,	.trpab_05T = 0,
	.trc = 3,	.trc_05T = 0,
	.trfc = 44,	.trfc_05T = 0,
	.trfcpb = 16,	.trfcpb_05T = 0,
	.txp = 0,	.txp_05T = 0,
	.trtp = 3,	.trtp_05T = 0,
	.trcd = 4,	.trcd_05T = 0,
	.twr = 12,	.twr_05T = 0,
	.twtr = 10,	.twtr_05T = 0,
	.tpbr2pbr = 11,	.tpbr2pbr_05T = 0,
	.tpbr2act = 0,	.tpbr2act_05T = 0,
	.tr2mrw = 17,	.tr2mrw_05T = 0,
	.tw2mrw = 13,	.tw2mrw_05T = 0,
	.tmrr2mrw = 15,	.tmrr2mrw_05T = 0,
	.tmrw = 6,	.tmrw_05T = 0,
	.tmrd = 6,	.tmrd_05T = 0,
	.tmrwckel = 8,	.tmrwckel_05T = 0,
	.tpde = 3,	.tpde_05T = 0,
	.tpdx = 3,	.tpdx_05T = 0,
	.tmrri = 7,	.tmrri_05T = 0,
	.trrd = 1,	.trrd_05T = 0,
	.trrd_4266 = 1,	.trrd_4266_05T = 0,
	.tfaw = 0,	.tfaw_05T = 0,
	.tfaw_4266 = 0,	.tfaw_4266_05T = 0,
	.trtw_odt_off = 7,	.trtw_odt_off_05T = 0,
	.trtw_odt_on = 12,	.trtw_odt_on_05T = 0,
	.txrefcnt = 58,
	.tzqcs = 16,
	.xrtw2w_new_mode = 9,
	.xrtw2w_old_mode = 10,
	.xrtw2r_odt_on = 6,
	.xrtw2r_odt_off = 4,
	.xrtr2w_odt_on = 10,
	.xrtr2w_odt_off = 10,
	.xrtr2r_new_mode = 6,
	.xrtr2r_old_mode = 9,
	.tr2mrr = 8,
	.vrcgdis_prdcnt = 20,
	.hwset_mr2_op = 18,
	.hwset_mr13_op = 24,
	.hwset_vrcg_op = 16,
	.trcd_derate = 4,	.trcd_derate_05T = 0,
	.trc_derate = 4,	.trc_derate_05T = 0,
	.tras_derate = 1,	.tras_derate_05T = 0,
	.trpab_derate = 3,	.trpab_derate_05T = 0,
	.trp_derate = 2,	.trp_derate_05T = 0,
	.trrd_derate = 2,	.trrd_derate_05T = 0,
	.trtpd = 16,	.trtpd_05T = 0,
	.twtpd = 15,	.twtpd_05T = 0,
	.tmrr2w_odt_off = 11,
	.tmrr2w_odt_on = 13,
	.ckeprd = 2,
	.ckelckcnt = 3,
	.zqlat2 = 6,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 5,	 .datlat = 15
},
//LPDDR4 4X_800_Div 8_DBI0.csv Read 0
{
	.dramType = TYPE_LPDDR4, .freq = 400, .cbtMode = CBT_NORMAL_MODE, .readDBI = 0,
	.readLat = 14,	.writeLat =  8,	.DivMode =	DIV8_MODE,

	.tras = 0,	.tras_05T = 0,
	.trp = 0,	.trp_05T = 1,
	.trpab = 1,	.trpab_05T = 0,
	.trc = 0,	.trc_05T = 0,
	.trfc = 16,	.trfc_05T = 0,
	.trfcpb = 2,	.trfcpb_05T = 0,
	.txp = 0,	.txp_05T = 0,
	.trtp = 0,	.trtp_05T = 1,
	.trcd = 2,	.trcd_05T = 0,
	.twr = 5,	.twr_05T = 0,
	.twtr = 4,	.twtr_05T = 1,
	.tpbr2pbr = 2,	.tpbr2pbr_05T = 0,
	.tpbr2act = 0,	.tpbr2act_05T = 0,
	.tr2mrw = 8,	.tr2mrw_05T = 0,
	.tw2mrw = 6,	.tw2mrw_05T = 1,
	.tmrr2mrw = 7,	.tmrr2mrw_05T = 0,
	.tmrw = 3,	.tmrw_05T = 0,
	.tmrd = 3,	.tmrd_05T = 0,
	.tmrwckel = 4,	.tmrwckel_05T = 0,
	.tpde = 1,	.tpde_05T = 1,
	.tpdx = 1,	.tpdx_05T = 0,
	.tmrri = 3,	.tmrri_05T = 1,
	.trrd = 0,	.trrd_05T = 0,
	.trrd_4266 = 0,	.trrd_4266_05T = 0,
	.tfaw = 0,	.tfaw_05T = 0,
	.tfaw_4266 = 0,	.tfaw_4266_05T = 0,
	.trtw_odt_off = 1,	.trtw_odt_off_05T = 0,
	.trtw_odt_on = 4,	.trtw_odt_on_05T = 0,
	.txrefcnt = 29,
	.tzqcs = 7,
	.xrtw2w_new_mode = 4,
	.xrtw2w_old_mode = 6,
	.xrtw2r_odt_on = 3,
	.xrtw2r_odt_off = 2,
	.xrtr2w_odt_on = 2,
	.xrtr2w_odt_off = 2,
	.xrtr2r_new_mode = 3,
	.xrtr2r_old_mode = 6,
	.tr2mrr = 4,
	.vrcgdis_prdcnt = 10,
	.hwset_mr2_op = 18,
	.hwset_mr13_op = 24,
	.hwset_vrcg_op = 16,
	.trcd_derate = 2,	.trcd_derate_05T = 0,
	.trc_derate = 0,	.trc_derate_05T = 0,
	.tras_derate = 0,	.tras_derate_05T = 0,
	.trpab_derate = 1,	.trpab_derate_05T = 0,
	.trp_derate = 0,	.trp_derate_05T = 1,
	.trrd_derate = 0,	.trrd_derate_05T = 1,
	.trtpd = 7,	.trtpd_05T = 1,
	.twtpd = 7,	.twtpd_05T = 1,
	.tmrr2w_odt_off = 3,
	.tmrr2w_odt_on = 5,
	.ckeprd = 1,
	.ckelckcnt = 2,
	.zqlat2 = 3,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 5,	 .datlat = 15
},
//LPDDR4 4X_800_BT_Div 8_DBI0.csv Read 0
{
	.dramType = TYPE_LPDDR4, .freq = 400, .cbtMode = CBT_BYTE_MODE1, .readDBI = 0,
	.readLat = 16,	.writeLat =  8,	.DivMode =	DIV8_MODE,

	.tras = 0,	.tras_05T = 0,
	.trp = 0,	.trp_05T = 1,
	.trpab = 1,	.trpab_05T = 0,
	.trc = 0,	.trc_05T = 0,
	.trfc = 16,	.trfc_05T = 0,
	.trfcpb = 2,	.trfcpb_05T = 0,
	.txp = 0,	.txp_05T = 0,
	.trtp = 0,	.trtp_05T = 1,
	.trcd = 2,	.trcd_05T = 0,
	.twr = 5,	.twr_05T = 0,
	.twtr = 4,	.twtr_05T = 1,
	.tpbr2pbr = 2,	.tpbr2pbr_05T = 0,
	.tpbr2act = 0,	.tpbr2act_05T = 0,
	.tr2mrw = 8,	.tr2mrw_05T = 1,
	.tw2mrw = 6,	.tw2mrw_05T = 1,
	.tmrr2mrw = 7,	.tmrr2mrw_05T = 1,
	.tmrw = 3,	.tmrw_05T = 0,
	.tmrd = 3,	.tmrd_05T = 0,
	.tmrwckel = 4,	.tmrwckel_05T = 0,
	.tpde = 1,	.tpde_05T = 1,
	.tpdx = 1,	.tpdx_05T = 0,
	.tmrri = 3,	.tmrri_05T = 1,
	.trrd = 0,	.trrd_05T = 0,
	.trrd_4266 = 0,	.trrd_4266_05T = 0,
	.tfaw = 0,	.tfaw_05T = 0,
	.tfaw_4266 = 0,	.tfaw_4266_05T = 0,
	.trtw_odt_off = 2,	.trtw_odt_off_05T = 0,
	.trtw_odt_on = 4,	.trtw_odt_on_05T = 0,
	.txrefcnt = 29,
	.tzqcs = 7,
	.xrtw2w_new_mode = 4,
	.xrtw2w_old_mode = 6,
	.xrtw2r_odt_on = 3,
	.xrtw2r_odt_off = 2,
	.xrtr2w_odt_on = 3,
	.xrtr2w_odt_off = 3,
	.xrtr2r_new_mode = 3,
	.xrtr2r_old_mode = 6,
	.tr2mrr = 4,
	.vrcgdis_prdcnt = 10,
	.hwset_mr2_op = 18,
	.hwset_mr13_op = 24,
	.hwset_vrcg_op = 16,
	.trcd_derate = 2,	.trcd_derate_05T = 0,
	.trc_derate = 0,	.trc_derate_05T = 0,
	.tras_derate = 0,	.tras_derate_05T = 0,
	.trpab_derate = 1,	.trpab_derate_05T = 0,
	.trp_derate = 0,	.trp_derate_05T = 1,
	.trrd_derate = 0,	.trrd_derate_05T = 1,
	.trtpd = 8,	.trtpd_05T = 0,
	.twtpd = 7,	.twtpd_05T = 1,
	.tmrr2w_odt_off = 4,
	.tmrr2w_odt_on = 6,
	.ckeprd = 1,
	.ckelckcnt = 2,
	.zqlat2 = 3,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 5,	 .datlat = 15
},
#endif
#if SUPPORT_LP4_DDR400_ACTIM
	//LP4_DDR400 ACTiming---------------------------------
//LPDDR4 4X_400_Div 4_DBI0.csv Read 0
{
	.dramType = TYPE_LPDDR4, .freq = 200, .cbtMode = CBT_NORMAL_MODE, .readDBI = 0,
	.readLat = 14,	.writeLat =  8,	.DivMode =	DIV4_MODE,

	.tras = 0,	.tras_05T = 0,
	.trp = 0,	.trp_05T = 0,
	.trpab = 1,	.trpab_05T = 0,
	.trc = 0,	.trc_05T = 0,
	.trfc = 16,	.trfc_05T = 0,
	.trfcpb = 2,	.trfcpb_05T = 0,
	.txp = 0,	.txp_05T = 0,
	.trtp = 3,	.trtp_05T = 0,
	.trcd = 2,	.trcd_05T = 0,
	.twr = 11,	.twr_05T = 0,
	.twtr = 10,	.twtr_05T = 0,
	.tpbr2pbr = 2,	.tpbr2pbr_05T = 0,
	.tpbr2act = 0,	.tpbr2act_05T = 0,
	.tr2mrw = 16,	.tr2mrw_05T = 0,
	.tw2mrw = 13,	.tw2mrw_05T = 0,
	.tmrr2mrw = 13,	.tmrr2mrw_05T = 0,
	.tmrw = 6,	.tmrw_05T = 0,
	.tmrd = 6,	.tmrd_05T = 0,
	.tmrwckel = 8,	.tmrwckel_05T = 0,
	.tpde = 3,	.tpde_05T = 0,
	.tpdx = 3,	.tpdx_05T = 0,
	.tmrri = 5,	.tmrri_05T = 0,
	.trrd = 1,	.trrd_05T = 0,
	.trrd_4266 = 1,	.trrd_4266_05T = 0,
	.tfaw = 0,	.tfaw_05T = 0,
	.tfaw_4266 = 0,	.tfaw_4266_05T = 0,
	.trtw_odt_off = 6,	.trtw_odt_off_05T = 0,
	.trtw_odt_on = 10,	.trtw_odt_on_05T = 0,
	.txrefcnt = 29,
	.tzqcs = 7,
	.xrtw2w_new_mode = 10,
	.xrtw2w_old_mode = 10,
	.xrtw2r_odt_on = 8,
	.xrtw2r_odt_off = 8,
	.xrtr2w_odt_on = 9,
	.xrtr2w_odt_off = 9,
	.xrtr2r_new_mode = 6,
	.xrtr2r_old_mode = 8,
	.tr2mrr = 8,
	.vrcgdis_prdcnt = 10,
	.hwset_mr2_op = 18,
	.hwset_mr13_op = 24,
	.hwset_vrcg_op = 16,
	.trcd_derate = 2,	.trcd_derate_05T = 0,
	.trc_derate = 0,	.trc_derate_05T = 0,
	.tras_derate = 0,	.tras_derate_05T = 0,
	.trpab_derate = 1,	.trpab_derate_05T = 0,
	.trp_derate = 0,	.trp_derate_05T = 0,
	.trrd_derate = 1,	.trrd_derate_05T = 0,
	.trtpd = 14,	.trtpd_05T = 0,
	.twtpd = 14,	.twtpd_05T = 0,
	.tmrr2w_odt_off = 9,
	.tmrr2w_odt_on = 11,
	.ckeprd = 2,
	.ckelckcnt = 3,
	.zqlat2 = 4,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 2,	 .datlat = 15
},
//LPDDR4 4X_400_BT_Div 4_DBI0.csv Read 0
{
	.dramType = TYPE_LPDDR4, .freq = 200, .cbtMode = CBT_BYTE_MODE1, .readDBI = 0,
	.readLat = 16,	.writeLat =  8,	.DivMode =	DIV4_MODE,

	.tras = 0,	.tras_05T = 0,
	.trp = 0,	.trp_05T = 0,
	.trpab = 1,	.trpab_05T = 0,
	.trc = 0,	.trc_05T = 0,
	.trfc = 16,	.trfc_05T = 0,
	.trfcpb = 2,	.trfcpb_05T = 0,
	.txp = 0,	.txp_05T = 0,
	.trtp = 3,	.trtp_05T = 0,
	.trcd = 2,	.trcd_05T = 0,
	.twr = 11,	.twr_05T = 0,
	.twtr = 10,	.twtr_05T = 0,
	.tpbr2pbr = 2,	.tpbr2pbr_05T = 0,
	.tpbr2act = 0,	.tpbr2act_05T = 0,
	.tr2mrw = 17,	.tr2mrw_05T = 0,
	.tw2mrw = 13,	.tw2mrw_05T = 0,
	.tmrr2mrw = 14,	.tmrr2mrw_05T = 0,
	.tmrw = 6,	.tmrw_05T = 0,
	.tmrd = 6,	.tmrd_05T = 0,
	.tmrwckel = 8,	.tmrwckel_05T = 0,
	.tpde = 3,	.tpde_05T = 0,
	.tpdx = 3,	.tpdx_05T = 0,
	.tmrri = 5,	.tmrri_05T = 0,
	.trrd = 1,	.trrd_05T = 0,
	.trrd_4266 = 1,	.trrd_4266_05T = 0,
	.tfaw = 0,	.tfaw_05T = 0,
	.tfaw_4266 = 0,	.tfaw_4266_05T = 0,
	.trtw_odt_off = 7,	.trtw_odt_off_05T = 0,
	.trtw_odt_on = 11,	.trtw_odt_on_05T = 0,
	.txrefcnt = 29,
	.tzqcs = 7,
	.xrtw2w_new_mode = 10,
	.xrtw2w_old_mode = 10,
	.xrtw2r_odt_on = 7,
	.xrtw2r_odt_off = 7,
	.xrtr2w_odt_on = 10,
	.xrtr2w_odt_off = 10,
	.xrtr2r_new_mode = 6,
	.xrtr2r_old_mode = 9,
	.tr2mrr = 8,
	.vrcgdis_prdcnt = 10,
	.hwset_mr2_op = 18,
	.hwset_mr13_op = 24,
	.hwset_vrcg_op = 16,
	.trcd_derate = 2,	.trcd_derate_05T = 0,
	.trc_derate = 0,	.trc_derate_05T = 0,
	.tras_derate = 0,	.tras_derate_05T = 0,
	.trpab_derate = 1,	.trpab_derate_05T = 0,
	.trp_derate = 0,	.trp_derate_05T = 0,
	.trrd_derate = 1,	.trrd_derate_05T = 0,
	.trtpd = 15,	.trtpd_05T = 0,
	.twtpd = 14,	.twtpd_05T = 0,
	.tmrr2w_odt_off = 10,
	.tmrr2w_odt_on = 12,
	.ckeprd = 2,
	.ckelckcnt = 3,
	.zqlat2 = 4,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 2,	 .datlat = 15
},
#endif
};

#if (__LP5_COMBO__)
	const ACTime_T_LP5 ACTimingTbl_LP5[AC_TIMING_NUMBER_LP5] = {
	//----------LPDDR5---------------------------
	#if SUPPORT_LP5_DDR6400_ACTIM
	//LP5_DDR6400 ACTiming---------------------------------
	#if (ENABLE_READ_DBI == 1)
	//LPDDR5_6400_Div 16_DBI1.csv Read 1
	{
	.dramType = TYPE_LPDDR5, .freq = 3200, .cbtMode = CBT_BYTE_MODE1, .readDBI = 1,

	//BL (burst length) = 16, DRMC_Clock_Rate = 400.0
	.readLat = 20, .writeLat =	9, .DivMode = DIV16_MODE,

	.tras = 8,	.tras_05T = 0,
	.trp = 7,	.trp_05T = 1,
	.trpab = 8,	.trpab_05T = 1,
	.trc = 16,	.trc_05T = 1,
	.trfc = 100,	.trfc_05T = 0,
	.trfcpb = 44,	.trfcpb_05T = 0,
	.txp = 2,	.txp_05T = 1,
	.trtp = 1,	.trtp_05T = 1,
	.trcd = 7,	.trcd_05T = 1,
	.twr = 19,	.twr_05T = 0,
	.twtr = 6,	.twtr_05T = 0,
	.twtr_l = 10,	.twtr_l_05T = 0,
	.tpbr2pbr = 28,	.tpbr2pbr_05T = 0,
	.tpbr2act = 2,	.tpbr2act_05T = 0,
	.tr2mrw = 15,	.tr2mrw_05T = 0,
	.tw2mrw = 8,	.tw2mrw_05T = 1,
	.tmrr2mrw = 13,	.tmrr2mrw_05T = 0,
	.tmrw = 4,	.tmrw_05T = 0,
	.tmrd = 6,	.tmrd_05T = 1,
	.tmrwckel = 8,	.tmrwckel_05T = 1,
	.tpde = 2,	.tpde_05T = 1,
	.tpdx = 1,	.tpdx_05T = 1,
	.tmrri = 12,	.tmrri_05T = 0,
	.trrd = 2,	.trrd_05T = 0,
	.tfaw = 0,	.tfaw_05T = 0,
	.tr2w_odt_off = 5,	.tr2w_odt_off_05T = 1,
	.tr2w_odt_on = 7,	.tr2w_odt_on_05T = 0,
	.txrefcnt = 115,
	.wckrdoff = 13,	.wckrdoff_05T = 0,
	.wckwroff = 7,	.wckwroff_05T = 1,
	.tzqcs = 34,
	.xrtw2w_odt_off = 2,
	.xrtw2w_odt_on = 3,
	.xrtw2r_odt_off_otf_off = 0,
	.xrtw2r_odt_on_otf_off = 0,
	.xrtw2r_odt_off_otf_on = 3,
	.xrtw2r_odt_on_otf_on = 3,
	.xrtr2w_odt_off = 8,
	.xrtr2w_odt_on = 9,
	.xrtr2r_odt_off = 6,
	.xrtr2r_odt_on = 6,
	.xrtw2w_odt_off_wck = 6,
	.xrtw2w_odt_on_wck = 8,
	.xrtw2r_odt_off_wck = 3,
	.xrtw2r_odt_on_wck = 4,
	.xrtr2w_odt_off_wck = 11,
	.xrtr2w_odt_on_wck = 11,
	.xrtr2r_wck = 8,
	.tr2mrr = 4,
	.hwset_mr2_op = 187,
	.hwset_mr13_op = 74,
	.hwset_vrcg_op = 176,
	.vrcgdis_prdcnt = 40,
	.lp5_cmd1to2en = 0,
	.trtpd = 13,	.trtpd_05T = 1,
	.twtpd = 21,	.twtpd_05T = 1,
	.tmrr2w = 16,
	.ckeprd = 2,
	.ckelckcnt = 3,
	.tcsh_cscal = 3,
	.tcacsh = 2,
	.tcsh = 5,
	.trcd_derate = 8,	.trcd_derate_05T = 0,
	.trc_derate = 17,	.trc_derate_05T = 0,
	.tras_derate = 10,	.tras_derate_05T = 0,
	.trpab_derate = 7,	.trpab_derate_05T = 1,
	.trp_derate = 6,	.trp_derate_05T = 0,
	.trrd_derate = 2,	.trrd_derate_05T = 0,
	.zqlat2 = 12,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 7,	.datlat = 10
	},
	//LPDDR5_6400_BT_Div 16_DBI1.csv Read 1
	{
	.dramType = TYPE_LPDDR5, .freq = 3200, .cbtMode = CBT_NORMAL_MODE, .readDBI = 1,
	//BL (burst length) = 16, DRMC_Clock_Rate = 400.0
	.readLat = 18, .writeLat =	9, .DivMode = DIV16_MODE,

	.tras = 8,	.tras_05T = 0,
	.trp = 7,	.trp_05T = 1,
	.trpab = 8,	.trpab_05T = 1,
	.trc = 16,	.trc_05T = 1,
	.trfc = 100,	.trfc_05T = 0,
	.trfcpb = 44,	.trfcpb_05T = 0,
	.txp = 2,	.txp_05T = 1,
	.trtp = 1,	.trtp_05T = 1,
	.trcd = 7,	.trcd_05T = 1,
	.twr = 18,	.twr_05T = 1,
	.twtr = 5,	.twtr_05T = 0,
	.twtr_l = 9,	.twtr_l_05T = 0,
	.tpbr2pbr = 28,	.tpbr2pbr_05T = 0,
	.tpbr2act = 2,	.tpbr2act_05T = 0,
	.tr2mrw = 14,	.tr2mrw_05T = 0,
	.tw2mrw = 8,	.tw2mrw_05T = 1,
	.tmrr2mrw = 12,	.tmrr2mrw_05T = 0,
	.tmrw = 4,	.tmrw_05T = 0,
	.tmrd = 6,	.tmrd_05T = 1,
	.tmrwckel = 8,	.tmrwckel_05T = 1,
	.tpde = 2,	.tpde_05T = 1,
	.tpdx = 1,	.tpdx_05T = 1,
	.tmrri = 12,	.tmrri_05T = 0,
	.trrd = 2,	.trrd_05T = 0,
	.tfaw = 0,	.tfaw_05T = 0,
	.tr2w_odt_off = 4,	.tr2w_odt_off_05T = 1,
	.tr2w_odt_on = 6,	.tr2w_odt_on_05T = 0,
	.txrefcnt = 115,
	.wckrdoff = 12,	.wckrdoff_05T = 0,
	.wckwroff = 7,	.wckwroff_05T = 1,
	.tzqcs = 34,
	.xrtw2w_odt_off = 2,
	.xrtw2w_odt_on = 3,
	.xrtw2r_odt_off_otf_off = 0,
	.xrtw2r_odt_on_otf_off = 0,
	.xrtw2r_odt_off_otf_on = 3,
	.xrtw2r_odt_on_otf_on = 3,
	.xrtr2w_odt_off = 7,
	.xrtr2w_odt_on = 8,
	.xrtr2r_odt_off = 6,
	.xrtr2r_odt_on = 6,
	.xrtw2w_odt_off_wck = 6,
	.xrtw2w_odt_on_wck = 8,
	.xrtw2r_odt_off_wck = 4,
	.xrtw2r_odt_on_wck = 5,
	.xrtr2w_odt_off_wck = 10,
	.xrtr2w_odt_on_wck = 10,
	.xrtr2r_wck = 8,
	.tr2mrr = 3,
	.hwset_mr2_op = 187,
	.hwset_mr13_op = 74,
	.hwset_vrcg_op = 176,
	.vrcgdis_prdcnt = 40,
	.lp5_cmd1to2en = 0,
	.trtpd = 12,	.trtpd_05T = 1,
	.twtpd = 21,	.twtpd_05T = 0,
	.tmrr2w = 15,
	.ckeprd = 2,
	.ckelckcnt = 3,
	.tcsh_cscal = 3,
	.tcacsh = 2,
	.tcsh = 5,
	.trcd_derate = 8,	.trcd_derate_05T = 0,
	.trc_derate = 17,	.trc_derate_05T = 0,
	.tras_derate = 10,	.tras_derate_05T = 0,
	.trpab_derate = 7,	.trpab_derate_05T = 1,
	.trp_derate = 6,	.trp_derate_05T = 0,
	.trrd_derate = 2,	.trrd_derate_05T = 0,
	.zqlat2 = 12,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 7,	.datlat = 10
	},
	#else  //ENABLE_READ_DBI == 0)
	//LPDDR5_6400_Div 16_DBI0.csv Read 0
	{
	.dramType = TYPE_LPDDR5, .freq = 3200, .cbtMode = CBT_BYTE_MODE1, .readDBI = 0,
	//BL (burst length) = 16, DRMC_Clock_Rate = 400.0
	.readLat = 18, .writeLat =	9, .DivMode = DIV16_MODE,

	.tras = 8,	.tras_05T = 0,
	.trp = 7,	.trp_05T = 1,
	.trpab = 8,	.trpab_05T = 1,
	.trc = 16,	.trc_05T = 1,
	.trfc = 100,	.trfc_05T = 0,
	.trfcpb = 44,	.trfcpb_05T = 0,
	.txp = 2,	.txp_05T = 1,
	.trtp = 1,	.trtp_05T = 1,
	.trcd = 7,	.trcd_05T = 1,
	.twr = 19,	.twr_05T = 0,
	.twtr = 6,	.twtr_05T = 0,
	.twtr_l = 10,	.twtr_l_05T = 0,
	.tpbr2pbr = 28,	.tpbr2pbr_05T = 0,
	.tpbr2act = 2,	.tpbr2act_05T = 0,
	.tr2mrw = 14,	.tr2mrw_05T = 0,
	.tw2mrw = 8,	.tw2mrw_05T = 1,
	.tmrr2mrw = 12,	.tmrr2mrw_05T = 0,
	.tmrw = 4,	.tmrw_05T = 0,
	.tmrd = 6,	.tmrd_05T = 1,
	.tmrwckel = 8,	.tmrwckel_05T = 1,
	.tpde = 2,	.tpde_05T = 1,
	.tpdx = 1,	.tpdx_05T = 1,
	.tmrri = 12,	.tmrri_05T = 0,
	.trrd = 2,	.trrd_05T = 0,
	.tfaw = 0,	.tfaw_05T = 0,
	.tr2w_odt_off = 4,	.tr2w_odt_off_05T = 1,
	.tr2w_odt_on = 6,	.tr2w_odt_on_05T = 0,
	.txrefcnt = 115,
	.wckrdoff = 12,	.wckrdoff_05T = 0,
	.wckwroff = 7,	.wckwroff_05T = 1,
	.tzqcs = 34,
	.xrtw2w_odt_off = 2,
	.xrtw2w_odt_on = 3,
	.xrtw2r_odt_off_otf_off = 0,
	.xrtw2r_odt_on_otf_off = 0,
	.xrtw2r_odt_off_otf_on = 3,
	.xrtw2r_odt_on_otf_on = 3,
	.xrtr2w_odt_off = 7,
	.xrtr2w_odt_on = 8,
	.xrtr2r_odt_off = 6,
	.xrtr2r_odt_on = 6,
	.xrtw2w_odt_off_wck = 6,
	.xrtw2w_odt_on_wck = 8,
	.xrtw2r_odt_off_wck = 4,
	.xrtw2r_odt_on_wck = 5,
	.xrtr2w_odt_off_wck = 10,
	.xrtr2w_odt_on_wck = 10,
	.xrtr2r_wck = 8,
	.tr2mrr = 3,
	.hwset_mr2_op = 187,
	.hwset_mr13_op = 74,
	.hwset_vrcg_op = 176,
	.vrcgdis_prdcnt = 40,
	.lp5_cmd1to2en = 0,
	.trtpd = 12,	.trtpd_05T = 1,
	.twtpd = 21,	.twtpd_05T = 1,
	.tmrr2w = 15,
	.ckeprd = 2,
	.ckelckcnt = 3,
	.tcsh_cscal = 3,
	.tcacsh = 2,
	.tcsh = 5,
	.trcd_derate = 8,	.trcd_derate_05T = 0,
	.trc_derate = 17,	.trc_derate_05T = 0,
	.tras_derate = 10,	.tras_derate_05T = 0,
	.trpab_derate = 7,	.trpab_derate_05T = 1,
	.trp_derate = 6,	.trp_derate_05T = 0,
	.trrd_derate = 2,	.trrd_derate_05T = 0,
	.zqlat2 = 12,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 7,	.datlat = 10
	},
	//LPDDR5_6400_BT_Div 16_DBI0.csv Read 0
	{
	.dramType = TYPE_LPDDR5, .freq = 3200, .cbtMode = CBT_NORMAL_MODE, .readDBI = 0,
	//BL (burst length) = 16, DRMC_Clock_Rate = 400.0
	.readLat = 17, .writeLat =	9, .DivMode = DIV16_MODE,

	.tras = 8,	.tras_05T = 0,
	.trp = 7,	.trp_05T = 1,
	.trpab = 8,	.trpab_05T = 1,
	.trc = 16,	.trc_05T = 1,
	.trfc = 100,	.trfc_05T = 0,
	.trfcpb = 44,	.trfcpb_05T = 0,
	.txp = 2,	.txp_05T = 1,
	.trtp = 1,	.trtp_05T = 1,
	.trcd = 7,	.trcd_05T = 1,
	.twr = 18,	.twr_05T = 1,
	.twtr = 5,	.twtr_05T = 0,
	.twtr_l = 9,	.twtr_l_05T = 0,
	.tpbr2pbr = 28,	.tpbr2pbr_05T = 0,
	.tpbr2act = 2,	.tpbr2act_05T = 0,
	.tr2mrw = 13,	.tr2mrw_05T = 1,
	.tw2mrw = 8,	.tw2mrw_05T = 1,
	.tmrr2mrw = 11,	.tmrr2mrw_05T = 1,
	.tmrw = 4,	.tmrw_05T = 0,
	.tmrd = 6,	.tmrd_05T = 1,
	.tmrwckel = 8,	.tmrwckel_05T = 1,
	.tpde = 2,	.tpde_05T = 1,
	.tpdx = 1,	.tpdx_05T = 1,
	.tmrri = 12,	.tmrri_05T = 0,
	.trrd = 2,	.trrd_05T = 0,
	.tfaw = 0,	.tfaw_05T = 0,
	.tr2w_odt_off = 4,	.tr2w_odt_off_05T = 0,
	.tr2w_odt_on = 5,	.tr2w_odt_on_05T = 1,
	.txrefcnt = 115,
	.wckrdoff = 11,	.wckrdoff_05T = 1,
	.wckwroff = 7,	.wckwroff_05T = 1,
	.tzqcs = 34,
	.xrtw2w_odt_off = 2,
	.xrtw2w_odt_on = 3,
	.xrtw2r_odt_off_otf_off = 0,
	.xrtw2r_odt_on_otf_off = 0,
	.xrtw2r_odt_off_otf_on = 3,
	.xrtw2r_odt_on_otf_on = 3,
	.xrtr2w_odt_off = 7,
	.xrtr2w_odt_on = 7,
	.xrtr2r_odt_off = 6,
	.xrtr2r_odt_on = 6,
	.xrtw2w_odt_off_wck = 6,
	.xrtw2w_odt_on_wck = 8,
	.xrtw2r_odt_off_wck = 4,
	.xrtw2r_odt_on_wck = 5,
	.xrtr2w_odt_off_wck = 10,
	.xrtr2w_odt_on_wck = 10,
	.xrtr2r_wck = 8,
	.tr2mrr = 2,
	.hwset_mr2_op = 187,
	.hwset_mr13_op = 74,
	.hwset_vrcg_op = 176,
	.vrcgdis_prdcnt = 40,
	.lp5_cmd1to2en = 0,
	.trtpd = 12,	.trtpd_05T = 0,
	.twtpd = 21,	.twtpd_05T = 0,
	.tmrr2w = 15,
	.ckeprd = 2,
	.ckelckcnt = 3,
	.tcsh_cscal = 3,
	.tcacsh = 2,
	.tcsh = 5,
	.trcd_derate = 8,	.trcd_derate_05T = 0,
	.trc_derate = 17,	.trc_derate_05T = 0,
	.tras_derate = 10,	.tras_derate_05T = 0,
	.trpab_derate = 7,	.trpab_derate_05T = 1,
	.trp_derate = 6,	.trp_derate_05T = 0,
	.trrd_derate = 2,	.trrd_derate_05T = 0,
	.zqlat2 = 12,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 7,	.datlat = 10
	},
	#endif
	#endif
	#if SUPPORT_LP5_DDR5500_ACTIM
	//LP5_DDR5500 ACTiming---------------------------------
	#if ((ENABLE_READ_DBI == 1) || (LP5_DDR4266_RDBI_WORKAROUND == 1))
	//F5500_Div16_DB1_NT0_RG0_EC0.csv Read 1
	{
	.dramType = TYPE_LPDDR5, .freq = 2750, .cbtMode = CBT_BYTE_MODE1, .readDBI = 1,

	//BL (burst length) = 16, DRMC_Clock_Rate = 343.75
	.readLat = 17, .writeLat =	8, .DivMode = DIV16_MODE,

	.tras = 5,	.tras_05T = 1,
	.trp = 6,	.trp_05T = 1,
	.trpab = 7,	.trpab_05T = 1,
	.trc = 13,	.trc_05T = 0,
	.trfc = 84,	.trfc_05T = 1,
	.trfcpb = 36,	.trfcpb_05T = 1,
	.txp = 2,	.txp_05T = 0,
	.trtp = 1,	.trtp_05T = 1,
	.trcd = 6,	.trcd_05T = 1,
	.twr = 16,	.twr_05T = 1,
	.twtr = 5,	.twtr_05T = 0,
	.twtr_l = 8,	.twtr_l_05T = 1,
	.tpbr2pbr = 23,	.tpbr2pbr_05T = 0,
	.tpbr2act = 2,	.tpbr2act_05T = 0,
	.tr2mrw = 13,	.tr2mrw_05T = 1,
	.tw2mrw = 8,	.tw2mrw_05T = 0,
	.tmrr2mrw = 11,	.tmrr2mrw_05T = 1,
	.tmrw = 3,	.tmrw_05T = 1,
	.tmrd = 5,	.tmrd_05T = 1,
	.tmrwckel = 7,	.tmrwckel_05T = 1,
	.tpde = 2,	.tpde_05T = 1,
	.tpdx = 1,	.tpdx_05T = 1,
	.tmrri = 10,	.tmrri_05T = 1,
	.trrd = 2,	.trrd_05T = 0,
	.tfaw = 0,	.tfaw_05T = 0,
	.tr2w_odt_off = 4,	.tr2w_odt_off_05T = 1,
	.tr2w_odt_on = 6,	.tr2w_odt_on_05T = 0,
	.txrefcnt = 99,
	.wckrdoff = 11,	.wckrdoff_05T = 1,
	.wckwroff = 7,	.wckwroff_05T = 0,
	.tzqcs = 29,
	.xrtw2w_odt_off = 2,
	.xrtw2w_odt_on = 3,
	.xrtw2r_odt_off_otf_off = 0,
	.xrtw2r_odt_on_otf_off = 0,
	.xrtw2r_odt_off_otf_on = 3,
	.xrtw2r_odt_on_otf_on = 3,
	.xrtr2w_odt_off = 7,
	.xrtr2w_odt_on = 8,
	.xrtr2r_odt_off = 6,
	.xrtr2r_odt_on = 6,
	.xrtw2w_odt_off_wck = 6,
	.xrtw2w_odt_on_wck = 8,
	.xrtw2r_odt_off_wck = 3,
	.xrtw2r_odt_on_wck = 4,
	.xrtr2w_odt_off_wck = 10,
	.xrtr2w_odt_on_wck = 10,
	.xrtr2r_wck = 8,
	.tr2mrr = 2,
	.hwset_mr2_op = 153,
	.hwset_mr13_op = 74,
	.hwset_vrcg_op = 144,
	.vrcgdis_prdcnt = 35,
	.lp5_cmd1to2en = 0,
	.trtpd = 12,	.trtpd_05T = 0,
	.twtpd = 19,	.twtpd_05T = 0,
	.tmrr2w = 15,
	.ckeprd = 2,
	.ckelckcnt = 3,
	.tcsh_cscal = 3,
	.tcacsh = 2,
	.tcsh = 5,
	.trcd_derate = 7,	.trcd_derate_05T = 0,
	.trc_derate = 13,	.trc_derate_05T = 1,
	.tras_derate = 7,	.tras_derate_05T = 0,
	.trpab_derate = 6,	.trpab_derate_05T = 0,
	.trp_derate = 5,	.trp_derate_05T = 0,
	.trrd_derate = 1,	.trrd_derate_05T = 1,
	.zqlat2 = 11,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 6,	.datlat = 19
	},
	//LPDDR5_DDR5500_Div 16_RDBI_ON_CBT_NORMAL_MODE
	{
	.dramType = TYPE_LPDDR5, .freq = 2750, .cbtMode = CBT_NORMAL_MODE, .readDBI = 1,
	//BL (burst length) = 16, DRMC_Clock_Rate = 343.75
	.readLat = 16, .writeLat =	8, .DivMode = DIV16_MODE,

	.tras = 5,	.tras_05T = 1,
	.trp = 6,	.trp_05T = 1,
	.trpab = 7,	.trpab_05T = 1,
	.trc = 13,	.trc_05T = 0,
	.trfc = 84,	.trfc_05T = 1,
	.trfcpb = 36,	.trfcpb_05T = 1,
	.txp = 2,	.txp_05T = 0,
	.trtp = 1,	.trtp_05T = 1,
	.trcd = 6,	.trcd_05T = 1,
	.twr = 16,	.twr_05T = 0,
	.twtr = 4,	.twtr_05T = 1,
	.twtr_l = 8,	.twtr_l_05T = 0,
	.tpbr2pbr = 23,	.tpbr2pbr_05T = 0,
	.tpbr2act = 2,	.tpbr2act_05T = 0,
	.tr2mrw = 13,	.tr2mrw_05T = 0,
	.tw2mrw = 8,	.tw2mrw_05T = 0,
	.tmrr2mrw = 11,	.tmrr2mrw_05T = 0,
	.tmrw = 3,	.tmrw_05T = 1,
	.tmrd = 5,	.tmrd_05T = 1,
	.tmrwckel = 7,	.tmrwckel_05T = 1,
	.tpde = 2,	.tpde_05T = 1,
	.tpdx = 1,	.tpdx_05T = 1,
	.tmrri = 10,	.tmrri_05T = 1,
	.trrd = 2,	.trrd_05T = 0,
	.tfaw = 0,	.tfaw_05T = 0,
	.tr2w_odt_off = 4,	.tr2w_odt_off_05T = 0,
	.tr2w_odt_on = 5,	.tr2w_odt_on_05T = 1,
	.txrefcnt = 99,
	.wckrdoff = 11,	.wckrdoff_05T = 0,
	.wckwroff = 7,	.wckwroff_05T = 0,
	.tzqcs = 29,
	.xrtw2w_odt_off = 2,
	.xrtw2w_odt_on = 3,
	.xrtw2r_odt_off_otf_off = 0,
	.xrtw2r_odt_on_otf_off = 0,
	.xrtw2r_odt_off_otf_on = 3,
	.xrtw2r_odt_on_otf_on = 3,
	.xrtr2w_odt_off = 7,
	.xrtr2w_odt_on = 7,
	.xrtr2r_odt_off = 6,
	.xrtr2r_odt_on = 6,
	.xrtw2w_odt_off_wck = 6,
	.xrtw2w_odt_on_wck = 8,
	.xrtw2r_odt_off_wck = 4,
	.xrtw2r_odt_on_wck = 5,
	.xrtr2w_odt_off_wck = 10,
	.xrtr2w_odt_on_wck = 10,
	.xrtr2r_wck = 8,
	.tr2mrr = 2,
	.hwset_mr2_op = 153,
	.hwset_mr13_op = 74,
	.hwset_vrcg_op = 144,
	.vrcgdis_prdcnt = 35,
	.lp5_cmd1to2en = 0,
	.trtpd = 11,	.trtpd_05T = 1,
	.twtpd = 18,	.twtpd_05T = 1,
	.tmrr2w = 14,
	.ckeprd = 2,
	.ckelckcnt = 3,
	.tcsh_cscal = 3,
	.tcacsh = 2,
	.tcsh = 5,
	.trcd_derate = 7,	.trcd_derate_05T = 0,
	.trc_derate = 13,	.trc_derate_05T = 1,
	.tras_derate = 7,	.tras_derate_05T = 0,
	.trpab_derate = 6,	.trpab_derate_05T = 0,
	.trp_derate = 5,	.trp_derate_05T = 0,
	.trrd_derate = 1,	.trrd_derate_05T = 1,
	.zqlat2 = 11,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 6,	.datlat = 19
	},
	#else //ENABLE_READ_DBI == 0
	//LPDDR5_DDR5500_Div 16_RDBI_OFF_CBT_BYTE_MODE1
	{
	.dramType = TYPE_LPDDR5, .freq = 2750, .cbtMode = CBT_BYTE_MODE1, .readDBI = 0,
	//BL (burst length) = 16, DRMC_Clock_Rate = 343.75
	.readLat = 16, .writeLat =	8, .DivMode = DIV16_MODE,

	.tras = 5,	.tras_05T = 1,
	.trp = 6,	.trp_05T = 1,
	.trpab = 7,	.trpab_05T = 1,
	.trc = 13,	.trc_05T = 0,
	.trfc = 84,	.trfc_05T = 1,
	.trfcpb = 36,	.trfcpb_05T = 1,
	.txp = 2,	.txp_05T = 0,
	.trtp = 1,	.trtp_05T = 1,
	.trcd = 6,	.trcd_05T = 1,
	.twr = 16,	.twr_05T = 1,
	.twtr = 5,	.twtr_05T = 0,
	.twtr_l = 8,	.twtr_l_05T = 1,
	.tpbr2pbr = 23,	.tpbr2pbr_05T = 0,
	.tpbr2act = 2,	.tpbr2act_05T = 0,
	.tr2mrw = 13,	.tr2mrw_05T = 0,
	.tw2mrw = 8,	.tw2mrw_05T = 0,
	.tmrr2mrw = 11,	.tmrr2mrw_05T = 0,
	.tmrw = 3,	.tmrw_05T = 1,
	.tmrd = 5,	.tmrd_05T = 1,
	.tmrwckel = 7,	.tmrwckel_05T = 1,
	.tpde = 2,	.tpde_05T = 1,
	.tpdx = 1,	.tpdx_05T = 1,
	.tmrri = 10,	.tmrri_05T = 1,
	.trrd = 2,	.trrd_05T = 0,
	.tfaw = 0,	.tfaw_05T = 0,
	.tr2w_odt_off = 4,	.tr2w_odt_off_05T = 0,
	.tr2w_odt_on = 5,	.tr2w_odt_on_05T = 1,
	.txrefcnt = 99,
	.wckrdoff = 11,	.wckrdoff_05T = 0,
	.wckwroff = 7,	.wckwroff_05T = 0,
	.tzqcs = 29,
	.xrtw2w_odt_off = 2,
	.xrtw2w_odt_on = 3,
	.xrtw2r_odt_off_otf_off = 0,
	.xrtw2r_odt_on_otf_off = 0,
	.xrtw2r_odt_off_otf_on = 3,
	.xrtw2r_odt_on_otf_on = 3,
	.xrtr2w_odt_off = 7,
	.xrtr2w_odt_on = 7,
	.xrtr2r_odt_off = 6,
	.xrtr2r_odt_on = 6,
	.xrtw2w_odt_off_wck = 6,
	.xrtw2w_odt_on_wck = 8,
	.xrtw2r_odt_off_wck = 4,
	.xrtw2r_odt_on_wck = 5,
	.xrtr2w_odt_off_wck = 10,
	.xrtr2w_odt_on_wck = 10,
	.xrtr2r_wck = 8,
	.tr2mrr = 2,
	.hwset_mr2_op = 153,
	.hwset_mr13_op = 74,
	.hwset_vrcg_op = 144,
	.vrcgdis_prdcnt = 35,
	.lp5_cmd1to2en = 0,
	.trtpd = 11,	.trtpd_05T = 1,
	.twtpd = 19,	.twtpd_05T = 0,
	.tmrr2w = 14,
	.ckeprd = 2,
	.ckelckcnt = 3,
	.tcsh_cscal = 3,
	.tcacsh = 2,
	.tcsh = 5,
	.trcd_derate = 7,	.trcd_derate_05T = 0,
	.trc_derate = 13,	.trc_derate_05T = 1,
	.tras_derate = 7,	.tras_derate_05T = 0,
	.trpab_derate = 6,	.trpab_derate_05T = 0,
	.trp_derate = 5,	.trp_derate_05T = 0,
	.trrd_derate = 1,	.trrd_derate_05T = 1,
	.zqlat2 = 11,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 6,	.datlat = 19
	},
	{
	.dramType = TYPE_LPDDR5, .freq = 2750, .cbtMode = CBT_NORMAL_MODE, .readDBI = 0,
	//BL (burst length) = 16, DRMC_Clock_Rate = 343.75
	.readLat = 15, .writeLat =	8, .DivMode = DIV16_MODE,

	.tras = 5,	.tras_05T = 1,
	.trp = 6,	.trp_05T = 1,
	.trpab = 7,	.trpab_05T = 1,
	.trc = 13,	.trc_05T = 0,
	.trfc = 84,	.trfc_05T = 1,
	.trfcpb = 36,	.trfcpb_05T = 1,
	.txp = 2,	.txp_05T = 0,
	.trtp = 1,	.trtp_05T = 1,
	.trcd = 6,	.trcd_05T = 1,
	.twr = 16,	.twr_05T = 0,
	.twtr = 4,	.twtr_05T = 1,
	.twtr_l = 8,	.twtr_l_05T = 0,
	.tpbr2pbr = 23,	.tpbr2pbr_05T = 0,
	.tpbr2act = 2,	.tpbr2act_05T = 0,
	.tr2mrw = 12,	.tr2mrw_05T = 1,
	.tw2mrw = 8,	.tw2mrw_05T = 0,
	.tmrr2mrw = 10,	.tmrr2mrw_05T = 1,
	.tmrw = 3,	.tmrw_05T = 1,
	.tmrd = 5,	.tmrd_05T = 1,
	.tmrwckel = 7,	.tmrwckel_05T = 1,
	.tpde = 2,	.tpde_05T = 1,
	.tpdx = 1,	.tpdx_05T = 1,
	.tmrri = 10,	.tmrri_05T = 1,
	.trrd = 2,	.trrd_05T = 0,
	.tfaw = 0,	.tfaw_05T = 0,
	.tr2w_odt_off = 3,	.tr2w_odt_off_05T = 1,
	.tr2w_odt_on = 5,	.tr2w_odt_on_05T = 0,
	.txrefcnt = 99,
	.wckrdoff = 10,	.wckrdoff_05T = 1,
	.wckwroff = 7,	.wckwroff_05T = 0,
	.tzqcs = 29,
	.xrtw2w_odt_off = 2,
	.xrtw2w_odt_on = 3,
	.xrtw2r_odt_off_otf_off = 0,
	.xrtw2r_odt_on_otf_off = 1,
	.xrtw2r_odt_off_otf_on = 3,
	.xrtw2r_odt_on_otf_on = 3,
	.xrtr2w_odt_off = 6,
	.xrtr2w_odt_on = 7,
	.xrtr2r_odt_off = 6,
	.xrtr2r_odt_on = 6,
	.xrtw2w_odt_off_wck = 6,
	.xrtw2w_odt_on_wck = 8,
	.xrtw2r_odt_off_wck = 4,
	.xrtw2r_odt_on_wck = 5,
	.xrtr2w_odt_off_wck = 9,
	.xrtr2w_odt_on_wck = 9,
	.xrtr2r_wck = 8,
	.tr2mrr = 1,
	.hwset_mr2_op = 153,
	.hwset_mr13_op = 74,
	.hwset_vrcg_op = 144,
	.vrcgdis_prdcnt = 35,
	.lp5_cmd1to2en = 0,
	.trtpd = 11,	.trtpd_05T = 0,
	.twtpd = 18,	.twtpd_05T = 1,
	.tmrr2w = 14,
	.ckeprd = 2,
	.ckelckcnt = 3,
	.tcsh_cscal = 3,
	.tcacsh = 2,
	.tcsh = 5,
	.trcd_derate = 7,	.trcd_derate_05T = 0,
	.trc_derate = 13,	.trc_derate_05T = 1,
	.tras_derate = 7,	.tras_derate_05T = 0,
	.trpab_derate = 6,	.trpab_derate_05T = 0,
	.trp_derate = 5,	.trp_derate_05T = 0,
	.trrd_derate = 1,	.trrd_derate_05T = 1,
	.zqlat2 = 11,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 6,	.datlat = 19
	},
	#endif
	#endif
	#if SUPPORT_LP5_DDR4266_ACTIM
	//LP5_DDR4266 ACTiming---------------------------------
	#if ((ENABLE_READ_DBI == 1) || (LP5_DDR4266_RDBI_WORKAROUND == 1))
	//LPDDR5_4266_Div 8_CKR4_DBI1.csv Read 1
	{
	.dramType = TYPE_LPDDR5, .freq = 2133, .cbtMode = CBT_BYTE_MODE1, .readDBI = 1,

	//BL (burst length) = 16, DRMC_Clock_Rate = 533.25
	.readLat = 14, .writeLat =	6, .DivMode = DIV8_MODE,

	.tras = 14,	.tras_05T = 0,
	.trp = 10,	.trp_05T = 0,
	.trpab = 12,	.trpab_05T = 0,
	.trc = 25,	.trc_05T = 0,
	.trfc = 138,	.trfc_05T = 0,
	.trfcpb = 63,	.trfcpb_05T = 0,
	.txp = 5,	.txp_05T = 0,
	.trtp = 4,	.trtp_05T = 0,
	.trcd = 10,	.trcd_05T = 0,
	.twr = 27,	.twr_05T = 0,
	.twtr = 10,	.twtr_05T = 0,
	.twtr_l = 16,	.twtr_l_05T = 0,
	.tpbr2pbr = 40,	.tpbr2pbr_05T = 0,
	.tpbr2act = 3,	.tpbr2act_05T = 0,
	.tr2mrw = 23,	.tr2mrw_05T = 0,
	.tw2mrw = 14,	.tw2mrw_05T = 0,
	.tmrr2mrw = 21,	.tmrr2mrw_05T = 0,
	.tmrw = 6,	.tmrw_05T = 0,
	.tmrd = 9,	.tmrd_05T = 0,
	.tmrwckel = 13,	.tmrwckel_05T = 0,
	.tpde = 5,	.tpde_05T = 0,
	.tpdx = 3,	.tpdx_05T = 0,
	.tmrri = 17,	.tmrri_05T = 0,
	.trrd = 3,	.trrd_05T = 0,
	.tfaw = 3,	.tfaw_05T = 0,
	.tr2w_odt_off = 11,	.tr2w_odt_off_05T = 0,
	.tr2w_odt_on = 15,	.tr2w_odt_on_05T = 0,
	.txrefcnt = 154,
	.wckrdoff = 20,	.wckrdoff_05T = 0,
	.wckwroff = 12,	.wckwroff_05T = 0,
	.tzqcs = 46,
	.xrtw2w_odt_off = 5,
	.xrtw2w_odt_on = 7,
	.xrtw2r_odt_off_otf_off = 0,
	.xrtw2r_odt_on_otf_off = 0,
	.xrtw2r_odt_off_otf_on = 3,
	.xrtw2r_odt_on_otf_on = 3,
	.xrtr2w_odt_off = 12,
	.xrtr2w_odt_on = 14,
	.xrtr2r_odt_off = 9,
	.xrtr2r_odt_on = 9,
	.xrtw2w_odt_off_wck = 10,
	.xrtw2w_odt_on_wck = 12,
	.xrtw2r_odt_off_wck = 6,
	.xrtw2r_odt_on_wck = 7,
	.xrtr2w_odt_off_wck = 18,
	.xrtr2w_odt_on_wck = 18,
	.xrtr2r_wck = 14,
	.tr2mrr = 13,
	.hwset_mr2_op = 119,
	.hwset_mr13_op = 74,
	.hwset_vrcg_op = 112,
	.vrcgdis_prdcnt = 54,
	.lp5_cmd1to2en = 1,
	.trtpd = 19,	.trtpd_05T = 0,
	.twtpd = 31,	.twtpd_05T = 0,
	.tmrr2w = 23,
	.ckeprd = 3,
	.ckelckcnt = 5,
	.tcsh_cscal = 5,
	.tcacsh = 3,
	.tcsh = 4,
	.trcd_derate = 11,	.trcd_derate_05T = 0,
	.trc_derate = 26,	.trc_derate_05T = 0,
	.tras_derate = 17,	.tras_derate_05T = 0,
	.trpab_derate = 10,	.trpab_derate_05T = 0,
	.trp_derate = 8,	.trp_derate_05T = 0,
	.trrd_derate = 3,	.trrd_derate_05T = 0,
	.zqlat2 = 16,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 10,	.datlat = 19
	},
	//LPDDR5_4266_BT_Div 8_CKR4_DBI1.csv Read 1
	{
	.dramType = TYPE_LPDDR5, .freq = 2133, .cbtMode = CBT_NORMAL_MODE, .readDBI = 1,
	//BL (burst length) = 16, DRMC_Clock_Rate = 533.25
	.readLat = 13, .writeLat =	6, .DivMode = DIV8_MODE,

	.tras = 14,	.tras_05T = 0,
	.trp = 10,	.trp_05T = 0,
	.trpab = 12,	.trpab_05T = 0,
	.trc = 25,	.trc_05T = 0,
	.trfc = 138,	.trfc_05T = 0,
	.trfcpb = 63,	.trfcpb_05T = 0,
	.txp = 5,	.txp_05T = 0,
	.trtp = 4,	.trtp_05T = 0,
	.trcd = 10,	.trcd_05T = 0,
	.twr = 26,	.twr_05T = 0,
	.twtr = 9,	.twtr_05T = 0,
	.twtr_l = 15,	.twtr_l_05T = 0,
	.tpbr2pbr = 40,	.tpbr2pbr_05T = 0,
	.tpbr2act = 3,	.tpbr2act_05T = 0,
	.tr2mrw = 22,	.tr2mrw_05T = 0,
	.tw2mrw = 14,	.tw2mrw_05T = 0,
	.tmrr2mrw = 20,	.tmrr2mrw_05T = 0,
	.tmrw = 6,	.tmrw_05T = 0,
	.tmrd = 9,	.tmrd_05T = 0,
	.tmrwckel = 13,	.tmrwckel_05T = 0,
	.tpde = 5,	.tpde_05T = 0,
	.tpdx = 3,	.tpdx_05T = 0,
	.tmrri = 17,	.tmrri_05T = 0,
	.trrd = 3,	.trrd_05T = 0,
	.tfaw = 3,	.tfaw_05T = 0,
	.tr2w_odt_off = 10,	.tr2w_odt_off_05T = 0,
	.tr2w_odt_on = 14,	.tr2w_odt_on_05T = 0,
	.txrefcnt = 154,
	.wckrdoff = 19,	.wckrdoff_05T = 0,
	.wckwroff = 12,	.wckwroff_05T = 0,
	.tzqcs = 46,
	.xrtw2w_odt_off = 5,
	.xrtw2w_odt_on = 7,
	.xrtw2r_odt_off_otf_off = 0,
	.xrtw2r_odt_on_otf_off = 0,
	.xrtw2r_odt_off_otf_on = 3,
	.xrtw2r_odt_on_otf_on = 3,
	.xrtr2w_odt_off = 11,
	.xrtr2w_odt_on = 13,
	.xrtr2r_odt_off = 9,
	.xrtr2r_odt_on = 9,
	.xrtw2w_odt_off_wck = 10,
	.xrtw2w_odt_on_wck = 12,
	.xrtw2r_odt_off_wck = 7,
	.xrtw2r_odt_on_wck = 8,
	.xrtr2w_odt_off_wck = 17,
	.xrtr2w_odt_on_wck = 17,
	.xrtr2r_wck = 14,
	.tr2mrr = 12,
	.hwset_mr2_op = 119,
	.hwset_mr13_op = 74,
	.hwset_vrcg_op = 112,
	.vrcgdis_prdcnt = 54,
	.lp5_cmd1to2en = 1,
	.trtpd = 18,	.trtpd_05T = 0,
	.twtpd = 30,	.twtpd_05T = 0,
	.tmrr2w = 22,
	.ckeprd = 3,
	.ckelckcnt = 5,
	.tcsh_cscal = 5,
	.tcacsh = 3,
	.tcsh = 4,
	.trcd_derate = 11,	.trcd_derate_05T = 0,
	.trc_derate = 26,	.trc_derate_05T = 0,
	.tras_derate = 17,	.tras_derate_05T = 0,
	.trpab_derate = 10,	.trpab_derate_05T = 0,
	.trp_derate = 8,	.trp_derate_05T = 0,
	.trrd_derate = 3,	.trrd_derate_05T = 0,
	.zqlat2 = 16,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 10,	.datlat = 19
	},
	#else  //ENABLE_READ_DBI == 0)
	//LPDDR5_4266_Div 8_CKR4_DBI0.csv Read 0
	{
	.dramType = TYPE_LPDDR5, .freq = 2133, .cbtMode = CBT_BYTE_MODE1, .readDBI = 0,
	//BL (burst length) = 16, DRMC_Clock_Rate = 533.25
	.readLat = 13, .writeLat =	6, .DivMode = DIV8_MODE,

	.tras = 14,	.tras_05T = 0,
	.trp = 10,	.trp_05T = 0,
	.trpab = 12,	.trpab_05T = 0,
	.trc = 25,	.trc_05T = 0,
	.trfc = 138,	.trfc_05T = 0,
	.trfcpb = 63,	.trfcpb_05T = 0,
	.txp = 5,	.txp_05T = 0,
	.trtp = 4,	.trtp_05T = 0,
	.trcd = 10,	.trcd_05T = 0,
	.twr = 27,	.twr_05T = 0,
	.twtr = 10,	.twtr_05T = 0,
	.twtr_l = 16,	.twtr_l_05T = 0,
	.tpbr2pbr = 40,	.tpbr2pbr_05T = 0,
	.tpbr2act = 3,	.tpbr2act_05T = 0,
	.tr2mrw = 22,	.tr2mrw_05T = 0,
	.tw2mrw = 14,	.tw2mrw_05T = 0,
	.tmrr2mrw = 20,	.tmrr2mrw_05T = 0,
	.tmrw = 6,	.tmrw_05T = 0,
	.tmrd = 9,	.tmrd_05T = 0,
	.tmrwckel = 13,	.tmrwckel_05T = 0,
	.tpde = 5,	.tpde_05T = 0,
	.tpdx = 3,	.tpdx_05T = 0,
	.tmrri = 17,	.tmrri_05T = 0,
	.trrd = 3,	.trrd_05T = 0,
	.tfaw = 3,	.tfaw_05T = 0,
	.tr2w_odt_off = 10,	.tr2w_odt_off_05T = 0,
	.tr2w_odt_on = 14,	.tr2w_odt_on_05T = 0,
	.txrefcnt = 154,
	.wckrdoff = 19,	.wckrdoff_05T = 0,
	.wckwroff = 12,	.wckwroff_05T = 0,
	.tzqcs = 46,
	.xrtw2w_odt_off = 5,
	.xrtw2w_odt_on = 7,
	.xrtw2r_odt_off_otf_off = 0,
	.xrtw2r_odt_on_otf_off = 0,
	.xrtw2r_odt_off_otf_on = 3,
	.xrtw2r_odt_on_otf_on = 3,
	.xrtr2w_odt_off = 11,
	.xrtr2w_odt_on = 13,
	.xrtr2r_odt_off = 9,
	.xrtr2r_odt_on = 9,
	.xrtw2w_odt_off_wck = 10,
	.xrtw2w_odt_on_wck = 12,
	.xrtw2r_odt_off_wck = 7,
	.xrtw2r_odt_on_wck = 8,
	.xrtr2w_odt_off_wck = 17,
	.xrtr2w_odt_on_wck = 17,
	.xrtr2r_wck = 14,
	.tr2mrr = 12,
	.hwset_mr2_op = 119,
	.hwset_mr13_op = 74,
	.hwset_vrcg_op = 112,
	.vrcgdis_prdcnt = 54,
	.lp5_cmd1to2en = 1,
	.trtpd = 18,	.trtpd_05T = 0,
	.twtpd = 31,	.twtpd_05T = 0,
	.tmrr2w = 22,
	.ckeprd = 3,
	.ckelckcnt = 5,
	.tcsh_cscal = 5,
	.tcacsh = 3,
	.tcsh = 4,
	.trcd_derate = 11,	.trcd_derate_05T = 0,
	.trc_derate = 26,	.trc_derate_05T = 0,
	.tras_derate = 17,	.tras_derate_05T = 0,
	.trpab_derate = 10,	.trpab_derate_05T = 0,
	.trp_derate = 8,	.trp_derate_05T = 0,
	.trrd_derate = 3,	.trrd_derate_05T = 0,
	.zqlat2 = 16,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 10,	.datlat = 19
	},
	//LPDDR5_4266_BT_Div 8_CKR4_DBI0.csv Read 0
	{
	.dramType = TYPE_LPDDR5, .freq = 2133, .cbtMode = CBT_NORMAL_MODE, .readDBI = 0,
	//BL (burst length) = 16, DRMC_Clock_Rate = 533.25
	.readLat = 12, .writeLat =	6, .DivMode = DIV8_MODE,

	.tras = 14,	.tras_05T = 0,
	.trp = 10,	.trp_05T = 0,
	.trpab = 12,	.trpab_05T = 0,
	.trc = 25,	.trc_05T = 0,
	.trfc = 138,	.trfc_05T = 0,
	.trfcpb = 63,	.trfcpb_05T = 0,
	.txp = 5,	.txp_05T = 0,
	.trtp = 4,	.trtp_05T = 0,
	.trcd = 10,	.trcd_05T = 0,
	.twr = 26,	.twr_05T = 0,
	.twtr = 9,	.twtr_05T = 0,
	.twtr_l = 15,	.twtr_l_05T = 0,
	.tpbr2pbr = 40,	.tpbr2pbr_05T = 0,
	.tpbr2act = 3,	.tpbr2act_05T = 0,
	.tr2mrw = 21,	.tr2mrw_05T = 0,
	.tw2mrw = 14,	.tw2mrw_05T = 0,
	.tmrr2mrw = 19,	.tmrr2mrw_05T = 0,
	.tmrw = 6,	.tmrw_05T = 0,
	.tmrd = 9,	.tmrd_05T = 0,
	.tmrwckel = 13,	.tmrwckel_05T = 0,
	.tpde = 5,	.tpde_05T = 0,
	.tpdx = 3,	.tpdx_05T = 0,
	.tmrri = 17,	.tmrri_05T = 0,
	.trrd = 3,	.trrd_05T = 0,
	.tfaw = 3,	.tfaw_05T = 0,
	.tr2w_odt_off = 9,	.tr2w_odt_off_05T = 0,
	.tr2w_odt_on = 13,	.tr2w_odt_on_05T = 0,
	.txrefcnt = 154,
	.wckrdoff = 18,	.wckrdoff_05T = 0,
	.wckwroff = 12,	.wckwroff_05T = 0,
	.tzqcs = 46,
	.xrtw2w_odt_off = 5,
	.xrtw2w_odt_on = 7,
	.xrtw2r_odt_off_otf_off = 0,
	.xrtw2r_odt_on_otf_off = 1,
	.xrtw2r_odt_off_otf_on = 3,
	.xrtw2r_odt_on_otf_on = 3,
	.xrtr2w_odt_off = 10,
	.xrtr2w_odt_on = 12,
	.xrtr2r_odt_off = 9,
	.xrtr2r_odt_on = 9,
	.xrtw2w_odt_off_wck = 10,
	.xrtw2w_odt_on_wck = 12,
	.xrtw2r_odt_off_wck = 8,
	.xrtw2r_odt_on_wck = 9,
	.xrtr2w_odt_off_wck = 16,
	.xrtr2w_odt_on_wck = 16,
	.xrtr2r_wck = 14,
	.tr2mrr = 11,
	.hwset_mr2_op = 119,
	.hwset_mr13_op = 74,
	.hwset_vrcg_op = 112,
	.vrcgdis_prdcnt = 54,
	.lp5_cmd1to2en = 1,
	.trtpd = 17,	.trtpd_05T = 0,
	.twtpd = 30,	.twtpd_05T = 0,
	.tmrr2w = 21,
	.ckeprd = 3,
	.ckelckcnt = 5,
	.tcsh_cscal = 5,
	.tcacsh = 3,
	.tcsh = 4,
	.trcd_derate = 11,	.trcd_derate_05T = 0,
	.trc_derate = 26,	.trc_derate_05T = 0,
	.tras_derate = 17,	.tras_derate_05T = 0,
	.trpab_derate = 10,	.trpab_derate_05T = 0,
	.trp_derate = 8,	.trp_derate_05T = 0,
	.trrd_derate = 3,	.trrd_derate_05T = 0,
	.zqlat2 = 16,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 10,	.datlat = 19
	},
	#endif
	#endif
	#if SUPPORT_LP5_DDR3200_ACTIM
	//LP5_DDR3200 ACTiming---------------------------------
	//LPDDR5_3200_Div 8_CKR2_DBI1.csv Read 0
	{
	.dramType = TYPE_LPDDR5, .freq = 1600, .cbtMode = CBT_BYTE_MODE1, .readDBI = 0,

	//BL (burst length) = 16, DRMC_Clock_Rate = 400.0
	.readLat = 20, .writeLat =	10, .DivMode = DIV8_MODE,

	.tras = 8,	.tras_05T = 0,
	.trp = 7,	.trp_05T = 1,
	.trpab = 8,	.trpab_05T = 1,
	.trc = 16,	.trc_05T = 1,
	.trfc = 140,	.trfc_05T = 0,
	.trfcpb = 64,	.trfcpb_05T = 0,
	.txp = 2,	.txp_05T = 1,
	.trtp = 2,	.trtp_05T = 0,
	.trcd = 7,	.trcd_05T = 1,
	.twr = 19,	.twr_05T = 0,
	.twtr = 10,	.twtr_05T = 0,
	.twtr_l = 10,	.twtr_l_05T = 0,
	.tpbr2pbr = 36,	.tpbr2pbr_05T = 0,
	.tpbr2act = 3,	.tpbr2act_05T = 0,
	.tr2mrw = 15,	.tr2mrw_05T = 0,
	.tw2mrw = 10,	.tw2mrw_05T = 0,
	.tmrr2mrw = 14,	.tmrr2mrw_05T = 0,
	.tmrw = 4,	.tmrw_05T = 0,
	.tmrd = 6,	.tmrd_05T = 1,
	.tmrwckel = 8,	.tmrwckel_05T = 1,
	.tpde = 2,	.tpde_05T = 1,
	.tpdx = 1,	.tpdx_05T = 1,
	.tmrri = 12,	.tmrri_05T = 0,
	.trrd = 2,	.trrd_05T = 0,
	.tfaw = 0,	.tfaw_05T = 0,
	.tr2w_odt_off = 4,	.tr2w_odt_off_05T = 0,
	.tr2w_odt_on = 6,	.tr2w_odt_on_05T = 0,
	.txrefcnt = 115,
	.wckrdoff = 13,	.wckrdoff_05T = 0,
	.wckwroff = 8,	.wckwroff_05T = 0,
	.tzqcs = 34,
	.xrtw2w_odt_off = 6,
	.xrtw2w_odt_on = 9,
	.xrtw2r_odt_off_otf_off = 3,
	.xrtw2r_odt_on_otf_off = 3,
	.xrtw2r_odt_off_otf_on = 3,
	.xrtw2r_odt_on_otf_on = 3,
	.xrtr2w_odt_off = 9,
	.xrtr2w_odt_on = 10,
	.xrtr2r_odt_off = 8,
	.xrtr2r_odt_on = 8,
	.xrtw2w_odt_off_wck = 7,
	.xrtw2w_odt_on_wck = 9,
	.xrtw2r_odt_off_wck = 5,
	.xrtw2r_odt_on_wck = 6,
	.xrtr2w_odt_off_wck = 12,
	.xrtr2w_odt_on_wck = 12,
	.xrtr2r_wck = 10,
	.tr2mrr = 3,
	.hwset_mr2_op = 45,
	.hwset_mr13_op = 216,
	.hwset_vrcg_op = 208,
	.vrcgdis_prdcnt = 40,
	.lp5_cmd1to2en = 0,
	.trtpd = 13,	.trtpd_05T = 1,
	.twtpd = 13,	.twtpd_05T = 0,
	.tmrr2w = 16,
	.ckeprd = 4,
	.ckelckcnt = 3,
	.tcsh_cscal = 3,
	.tcacsh = 2,
	.tcsh = 5,
	.trcd_derate = 8,	.trcd_derate_05T = 0,
	.trc_derate = 17,	.trc_derate_05T = 0,
	.tras_derate = 10,	.tras_derate_05T = 0,
	.trpab_derate = 7,	.trpab_derate_05T = 1,
	.trp_derate = 6,	.trp_derate_05T = 0,
	.trrd_derate = 2,	.trrd_derate_05T = 0,
	.zqlat2 = 12,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 7,	.datlat = 15
	},
	//LPDDR5_3200_BT_Div 8_CKR2_DBI1.csv Read 0
	{
	.dramType = TYPE_LPDDR5, .freq = 1600, .cbtMode = CBT_NORMAL_MODE, .readDBI = 0,
	//BL (burst length) = 16, DRMC_Clock_Rate = 400.0
	.readLat = 18, .writeLat =	10, .DivMode = DIV8_MODE,

	.tras = 8,	.tras_05T = 0,
	.trp = 7,	.trp_05T = 1,
	.trpab = 8,	.trpab_05T = 1,
	.trc = 16,	.trc_05T = 1,
	.trfc = 140,	.trfc_05T = 0,
	.trfcpb = 64,	.trfcpb_05T = 0,
	.txp = 2,	.txp_05T = 1,
	.trtp = 2,	.trtp_05T = 0,
	.trcd = 7,	.trcd_05T = 1,
	.twr = 19,	.twr_05T = -2,
	.twtr = 9,	.twtr_05T = 0,
	.twtr_l = 9,	.twtr_l_05T = 0,
	.tpbr2pbr = 36,	.tpbr2pbr_05T = 0,
	.tpbr2act = 3,	.tpbr2act_05T = 0,
	.tr2mrw = 14,	.tr2mrw_05T = 0,
	.tw2mrw = 10,	.tw2mrw_05T = 0,
	.tmrr2mrw = 13,	.tmrr2mrw_05T = 0,
	.tmrw = 4,	.tmrw_05T = 0,
	.tmrd = 6,	.tmrd_05T = 1,
	.tmrwckel = 8,	.tmrwckel_05T = 1,
	.tpde = 2,	.tpde_05T = 1,
	.tpdx = 1,	.tpdx_05T = 1,
	.tmrri = 12,	.tmrri_05T = 0,
	.trrd = 2,	.trrd_05T = 0,
	.tfaw = 0,	.tfaw_05T = 0,
	.tr2w_odt_off = 3,	.tr2w_odt_off_05T = 0,
	.tr2w_odt_on = 5,	.tr2w_odt_on_05T = 0,
	.txrefcnt = 115,
	.wckrdoff = 12,	.wckrdoff_05T = 0,
	.wckwroff = 8,	.wckwroff_05T = 0,
	.tzqcs = 34,
	.xrtw2w_odt_off = 6,
	.xrtw2w_odt_on = 9,
	.xrtw2r_odt_off_otf_off = 3,
	.xrtw2r_odt_on_otf_off = 3,
	.xrtw2r_odt_off_otf_on = 3,
	.xrtw2r_odt_on_otf_on = 3,
	.xrtr2w_odt_off = 8,
	.xrtr2w_odt_on = 9,
	.xrtr2r_odt_off = 8,
	.xrtr2r_odt_on = 8,
	.xrtw2w_odt_off_wck = 7,
	.xrtw2w_odt_on_wck = 9,
	.xrtw2r_odt_off_wck = 6,
	.xrtw2r_odt_on_wck = 7,
	.xrtr2w_odt_off_wck = 11,
	.xrtr2w_odt_on_wck = 11,
	.xrtr2r_wck = 10,
	.tr2mrr = 2,
	.hwset_mr2_op = 45,
	.hwset_mr13_op = 216,
	.hwset_vrcg_op = 208,
	.vrcgdis_prdcnt = 40,
	.lp5_cmd1to2en = 0,
	.trtpd = 12,	.trtpd_05T = 1,
	.twtpd = 12,	.twtpd_05T = 0,
	.tmrr2w = 15,
	.ckeprd = 4,
	.ckelckcnt = 3,
	.tcsh_cscal = 3,
	.tcacsh = 2,
	.tcsh = 5,
	.trcd_derate = 8,	.trcd_derate_05T = 0,
	.trc_derate = 17,	.trc_derate_05T = 0,
	.tras_derate = 10,	.tras_derate_05T = 0,
	.trpab_derate = 7,	.trpab_derate_05T = 1,
	.trp_derate = 6,	.trp_derate_05T = 0,
	.trrd_derate = 2,	.trrd_derate_05T = 0,
	.zqlat2 = 12,

	//DQSINCTL, DATLAT aren't in ACTiming excel file
	.dqsinctl = 7,	.datlat = 15
	},
	#endif
	};
#endif
