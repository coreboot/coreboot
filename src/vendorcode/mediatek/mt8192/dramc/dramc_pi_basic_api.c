/* SPDX-License-Identifier: BSD-3-Clause */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
//#include "..\Common\pd_common.h"
//#include "Register.h"
#include "dramc_common.h"
#include "dramc_dv_init.h"
#include "dramc_int_global.h"
#include "x_hal_io.h"
#include "dramc_actiming.h"

#include "dramc_reg_base_addr.h"

#include "dramc_top.h"

//#include "DramC_reg.h"
//#include "System_reg.h"
//#include "string.h"

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
U8 u1PrintModeRegWrite = 0;

#if ENABLE_RODT_TRACKING_SAVE_MCK
// global variables for RODT tracking & ROEN
U8 u1ODT_ON;		// infor of p->odt_onoff
U8 u1WDQS_ON = 0;	// infor of WDQS on(ROEN=1)
U8 u1RODT_TRACK = 0;  // infor of rodt tracking enable
U8 u1ROEN, u1ModeSel;//status of ROEN, MODESEL setting
#endif

//MRR DRAM->DRAMC
const U8 uiLPDDR4_MRR_DRAM_Pinmux[PINMUX_MAX][CHANNEL_NUM][16] =
{
	{
	// for DSC
		//CH-A
		{
			0, 1, 7, 6, 4, 5, 2, 3,
			9, 8, 11, 10, 14, 15, 13, 12
		},
		#if (CHANNEL_NUM>1)
		//CH-B
		{
			1, 0, 5, 6, 3, 2, 7, 4,
			8, 9, 11, 10, 12, 14, 13, 15
		},
		#endif
		#if (CHANNEL_NUM>2)
		//CH-C
		{
			0, 1, 7, 6, 4, 5, 2, 3,
			9, 8, 11, 10, 14, 15, 13, 12
		},
		//CH-D
		{
			1, 0, 5, 6, 3, 2, 7, 4,
			8, 9, 11, 10, 12, 14, 13, 15
		},
		#endif

	},
	{
	// for LPBK
		// TODO: need porting
	},
	{
	// for EMCP
		//CH-A
		{
			1, 0, 3, 2, 4, 7, 6, 5,
			8, 9, 10, 14, 11, 15, 13, 12
		},
		#if (CHANNEL_NUM>1)
		//CH-B
		{
			0, 1, 4, 7, 3, 5, 6, 2,
			9, 8, 10, 12, 11, 14, 13, 15
		},
		#endif
		#if (CHANNEL_NUM>2)
		//CH-C
		{
			1, 0, 3, 2, 4, 7, 6, 5,
			8, 9, 10, 14, 11, 15, 13, 12
		},
		//CH-D
		{
			0, 1, 4, 7, 3, 5, 6, 2,
			9, 8, 10, 12, 11, 14, 13, 15
		},
		#endif
	},
};

#if (__LP5_COMBO__)
const U8 uiLPDDR5_MRR_Mapping_POP[CHANNEL_NUM][16] =
{
	{
		8, 9, 10, 11, 12, 15, 14, 13,
		0, 1, 2, 3, 4, 7, 6, 5,
	},

	#if (CHANNEL_NUM>1)
	{
		8, 9, 10, 11, 12, 15, 14, 13,
		0, 1, 2, 3, 4, 7, 6, 5,
	},
	#endif
};
#endif

//MRR DRAM->DRAMC
U8 uiLPDDR4_MRR_Mapping_POP[CHANNEL_NUM][16] =
{
	//CH-A
	{
		1, 0, 3, 2, 4, 7, 6, 5,
		8, 9, 10, 14, 11, 15, 13, 12
	},
	#if (CHANNEL_NUM>1)
	//CH-B
	{
		0, 1, 4, 7, 3, 5, 6, 2,
		9, 8, 10, 12, 11, 14, 13, 15
	},
	#endif
	#if (CHANNEL_NUM>2)
	//CH-C
	{
		1, 0, 3, 2, 4, 7, 6, 5,
		8, 9, 10, 14, 11, 15, 13, 12
	},
	//CH-D
	{
		0, 1, 4, 7, 3, 5, 6, 2,
		9, 8, 10, 12, 11, 14, 13, 15
	},
	#endif
};

#if (fcFOR_CHIP_ID == fcMargaux)
static void Set_DRAM_Pinmux_Sel(DRAMC_CTX_T *p)
{
	#if (__LP5_COMBO__)
	if (is_lp5_family(p))
		return;
	#endif

#if !FOR_DV_SIMULATION_USED
	if (is_discrete_lpddr4())
		p->DRAMPinmux = PINMUX_DSC;
	else
#endif
		p->DRAMPinmux = PINMUX_EMCP;

	mcSHOW_DBG_MSG(("[Set_DRAM_Pinmux_Sel] DRAMPinmux = %d\n", p->DRAMPinmux));

	memcpy(&uiLPDDR4_MRR_Mapping_POP, uiLPDDR4_MRR_DRAM_Pinmux[p->DRAMPinmux], sizeof(uiLPDDR4_MRR_Mapping_POP));
	memcpy(&uiLPDDR4_O1_Mapping_POP, uiLPDDR4_O1_DRAM_Pinmux[p->DRAMPinmux], sizeof(uiLPDDR4_O1_Mapping_POP));
	memcpy(&uiLPDDR4_CA_Mapping_POP, uiLPDDR4_CA_DRAM_Pinmux[p->DRAMPinmux], sizeof(uiLPDDR4_CA_Mapping_POP));
}
#endif

void Set_MRR_Pinmux_Mapping(DRAMC_CTX_T *p)
{
	U8 *uiLPDDR_MRR_Mapping = NULL;
	U8 backup_channel;
	U32 backup_broadcast;
	DRAM_CHANNEL_T chIdx = CHANNEL_A;

	//Backup channel & broadcast
	backup_channel = vGetPHY2ChannelMapping(p);
	backup_broadcast = GetDramcBroadcast();

	DramcBroadcastOnOff(DRAMC_BROADCAST_OFF); //Disable broadcast

	//LP4: Set each channel's pinmux individually, LP3: Only has 1 channel (support_channel_num == 1)
	for (chIdx = CHANNEL_A; chIdx < (int)p->support_channel_num; chIdx++)
	{
		vSetPHY2ChannelMapping(p, chIdx);

	#if (__LP5_COMBO__)
		if (is_lp5_family(p))
			uiLPDDR_MRR_Mapping = (U8 *)uiLPDDR5_MRR_Mapping_POP[chIdx];
		else
	#endif
		uiLPDDR_MRR_Mapping = (U8 *)uiLPDDR4_MRR_Mapping_POP[chIdx];

		//Set MRR pin mux
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_MRR_BIT_MUX1), P_Fld(uiLPDDR_MRR_Mapping[0], MRR_BIT_MUX1_MRR_BIT0_SEL) | P_Fld(uiLPDDR_MRR_Mapping[1], MRR_BIT_MUX1_MRR_BIT1_SEL) |
																   P_Fld(uiLPDDR_MRR_Mapping[2], MRR_BIT_MUX1_MRR_BIT2_SEL) | P_Fld(uiLPDDR_MRR_Mapping[3], MRR_BIT_MUX1_MRR_BIT3_SEL));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_MRR_BIT_MUX2), P_Fld(uiLPDDR_MRR_Mapping[4], MRR_BIT_MUX2_MRR_BIT4_SEL) | P_Fld(uiLPDDR_MRR_Mapping[5], MRR_BIT_MUX2_MRR_BIT5_SEL) |
																   P_Fld(uiLPDDR_MRR_Mapping[6], MRR_BIT_MUX2_MRR_BIT6_SEL) | P_Fld(uiLPDDR_MRR_Mapping[7], MRR_BIT_MUX2_MRR_BIT7_SEL));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_MRR_BIT_MUX3), P_Fld(uiLPDDR_MRR_Mapping[8], MRR_BIT_MUX3_MRR_BIT8_SEL) | P_Fld(uiLPDDR_MRR_Mapping[9], MRR_BIT_MUX3_MRR_BIT9_SEL) |
																   P_Fld(uiLPDDR_MRR_Mapping[10], MRR_BIT_MUX3_MRR_BIT10_SEL) | P_Fld(uiLPDDR_MRR_Mapping[11], MRR_BIT_MUX3_MRR_BIT11_SEL));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_MRR_BIT_MUX4), P_Fld(uiLPDDR_MRR_Mapping[12], MRR_BIT_MUX4_MRR_BIT12_SEL) | P_Fld(uiLPDDR_MRR_Mapping[13], MRR_BIT_MUX4_MRR_BIT13_SEL) |
																   P_Fld(uiLPDDR_MRR_Mapping[14], MRR_BIT_MUX4_MRR_BIT14_SEL) | P_Fld(uiLPDDR_MRR_Mapping[15], MRR_BIT_MUX4_MRR_BIT15_SEL));
	}

	//Recover channel & broadcast
	vSetPHY2ChannelMapping(p, backup_channel);
	DramcBroadcastOnOff(backup_broadcast);
}


void Set_DQO1_Pinmux_Mapping(DRAMC_CTX_T *p)
{
	U8 *uiLPDDR_DQO1_Mapping = NULL;
	U8 backup_channel;
	U32 backup_broadcast;
	DRAM_CHANNEL_T chIdx = CHANNEL_A;

	//Backup channel & broadcast
	backup_channel = vGetPHY2ChannelMapping(p);
	backup_broadcast = GetDramcBroadcast();

	DramcBroadcastOnOff(DRAMC_BROADCAST_OFF); //Disable broadcast

	//LP4: Set each channel's pinmux individually, LP3: Only has 1 channel (support_channel_num == 1)
	for (chIdx = CHANNEL_A; chIdx < (int)p->support_channel_num; chIdx++)
	{
		vSetPHY2ChannelMapping(p, chIdx);

	#if (__LP5_COMBO__)
		if (is_lp5_family(p))
			uiLPDDR_DQO1_Mapping = (U8 *)uiLPDDR5_O1_Mapping_POP[chIdx];
		else
	#endif
		uiLPDDR_DQO1_Mapping = (U8 *)uiLPDDR4_O1_Mapping_POP[chIdx];

		//Set MRR pin mux
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DQ_SE_PINMUX_CTRL0), P_Fld(uiLPDDR_DQO1_Mapping[0], MISC_DQ_SE_PINMUX_CTRL0_DQ_PINMUX_SEL_DQ0)
																			| P_Fld(uiLPDDR_DQO1_Mapping[1], MISC_DQ_SE_PINMUX_CTRL0_DQ_PINMUX_SEL_DQ1)
																			| P_Fld(uiLPDDR_DQO1_Mapping[2], MISC_DQ_SE_PINMUX_CTRL0_DQ_PINMUX_SEL_DQ2)
																			| P_Fld(uiLPDDR_DQO1_Mapping[3], MISC_DQ_SE_PINMUX_CTRL0_DQ_PINMUX_SEL_DQ3)
																			| P_Fld(uiLPDDR_DQO1_Mapping[4], MISC_DQ_SE_PINMUX_CTRL0_DQ_PINMUX_SEL_DQ4)
																			| P_Fld(uiLPDDR_DQO1_Mapping[5], MISC_DQ_SE_PINMUX_CTRL0_DQ_PINMUX_SEL_DQ5)
																			| P_Fld(uiLPDDR_DQO1_Mapping[6], MISC_DQ_SE_PINMUX_CTRL0_DQ_PINMUX_SEL_DQ6)
																			| P_Fld(uiLPDDR_DQO1_Mapping[7], MISC_DQ_SE_PINMUX_CTRL0_DQ_PINMUX_SEL_DQ7));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DQ_SE_PINMUX_CTRL1), P_Fld(uiLPDDR_DQO1_Mapping[8], MISC_DQ_SE_PINMUX_CTRL1_DQ_PINMUX_SEL_DQ8)
																			| P_Fld(uiLPDDR_DQO1_Mapping[9], MISC_DQ_SE_PINMUX_CTRL1_DQ_PINMUX_SEL_DQ9)
																			| P_Fld(uiLPDDR_DQO1_Mapping[10], MISC_DQ_SE_PINMUX_CTRL1_DQ_PINMUX_SEL_DQ10)
																			| P_Fld(uiLPDDR_DQO1_Mapping[11], MISC_DQ_SE_PINMUX_CTRL1_DQ_PINMUX_SEL_DQ11)
																			| P_Fld(uiLPDDR_DQO1_Mapping[12], MISC_DQ_SE_PINMUX_CTRL1_DQ_PINMUX_SEL_DQ12)
																			| P_Fld(uiLPDDR_DQO1_Mapping[13], MISC_DQ_SE_PINMUX_CTRL1_DQ_PINMUX_SEL_DQ13)
																			| P_Fld(uiLPDDR_DQO1_Mapping[14], MISC_DQ_SE_PINMUX_CTRL1_DQ_PINMUX_SEL_DQ14)
																			| P_Fld(uiLPDDR_DQO1_Mapping[15], MISC_DQ_SE_PINMUX_CTRL1_DQ_PINMUX_SEL_DQ15));
	}

	//Recover channel & broadcast
	vSetPHY2ChannelMapping(p, backup_channel);
	DramcBroadcastOnOff(backup_broadcast);
}


static void SetRankInfoToConf(DRAMC_CTX_T *p)
{
#if (FOR_DV_SIMULATION_USED == 0 && SW_CHANGE_FOR_SIMULATION == 0)
	EMI_SETTINGS *emi_set;
	U32 u4value = 0;

#if (!__ETT__)//preloader
	if (emi_setting_index == -1)
		emi_set = &default_emi_setting;
	else
		emi_set = &emi_settings[emi_setting_index];
#else//ett
	emi_set = &default_emi_setting;
#endif

	u4value = ((emi_set->EMI_CONA_VAL >> 17) & 0x1)? 0: 1;//CONA 17th bit 0: Disable dual rank mode 1: Enable dual rank mode

	vIO32WriteFldAlign(DRAMC_REG_SA_RESERVE, u4value, SA_RESERVE_SINGLE_RANK);

	mcSHOW_JV_LOG_MSG(("Rank info: %d emi_setting_index: %d CONA[0x%x]\n", u4value, emi_setting_index, emi_set->EMI_CONA_VAL));
#endif
	return;
}

static void SetDramInfoToConf(DRAMC_CTX_T *p)
{
	vIO32WriteFldMulti_All(DRAMC_REG_SA_RESERVE,
		P_Fld(p->dram_cbt_mode[RANK_0], SA_RESERVE_MODE_RK0) |
		P_Fld(p->dram_cbt_mode[RANK_1], SA_RESERVE_MODE_RK1));

	if(u2DFSGetHighestFreq(p) >= 2133)
	{
		vIO32WriteFldAlign_All(DRAMC_REG_SA_RESERVE, 1, SA_RESERVE_SUPPORT_4266);
	}
}

void UpdateDFSTbltoDDR3200(DRAMC_CTX_T *p)
{
#if(FOR_DV_SIMULATION_USED==0 && SW_CHANGE_FOR_SIMULATION==0)
	EMI_SETTINGS *emi_set;
	U16 u2HighestFreq = u2DFSGetHighestFreq(p);
	DRAM_PLL_FREQ_SEL_T highestfreqsel = 0;
	U8 u1ShuffleIdx = 0;

#if (!__ETT__)//preloader
	if(emi_setting_index == -1)
		emi_set = &default_emi_setting;
	else
		emi_set = &emi_settings[emi_setting_index];
#else//ett
	emi_set = &default_emi_setting;
#endif

	// lookup table to find highest freq
	highestfreqsel = GetSelByFreq(p, u2HighestFreq);
	for (u1ShuffleIdx = DRAM_DFS_SHUFFLE_1; u1ShuffleIdx < DRAM_DFS_SHUFFLE_MAX; u1ShuffleIdx++)
		if (gFreqTbl[u1ShuffleIdx].freq_sel == highestfreqsel)
			break;

	gFreqTbl[u1ShuffleIdx].freq_sel = LP4_DDR3200; // for DSC DRAM

	gUpdateHighestFreq = TRUE;
	u2HighestFreq = u2DFSGetHighestFreq(p); // @Darren, Update u2FreqMax variables
	#if __ETT__
	UpdateEttDFVSTbltoDDR3200(p, u2HighestFreq); //@Darren, Update for DDR3200 ETT DVFS stress
	#endif
	mcSHOW_DBG_MSG(("[UpdateDFSTbltoDDR3200] Get Highest Freq is %d\n", u2HighestFreq));
#endif
}

void Global_Option_Init(DRAMC_CTX_T *p)
{
	//SaveCurrDramCtx(p);
	vSetChannelNumber(p);
	SetRankInfoToConf(p);
	vSetRankNumber(p);
	vSetFSPNumber(p);
#if (fcFOR_CHIP_ID == fcMargaux)
	Set_DRAM_Pinmux_Sel(p);
#endif
	Set_MRR_Pinmux_Mapping(p);
	Set_DQO1_Pinmux_Mapping(p);

	vInitGlobalVariablesByCondition(p);
	vInitMappingFreqArray(p);
#if ENABLE_TX_TRACKING
	DramcDQSOSCInit();
#endif

#ifdef FOR_HQA_TEST_USED
	HQA_measure_message_reset_all_data(p);
#endif
}

#if CBT_MOVE_CA_INSTEAD_OF_CLK
void DramcCmdUIDelaySetting(DRAMC_CTX_T *p, U8 value)
{
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_CA7), P_Fld(value, SHU_SELPH_CA7_DLY_RA0) |
																P_Fld(value, SHU_SELPH_CA7_DLY_RA1) |
																P_Fld(value, SHU_SELPH_CA7_DLY_RA2) |
																P_Fld(value, SHU_SELPH_CA7_DLY_RA3) |
																P_Fld(value, SHU_SELPH_CA7_DLY_RA4) |
																P_Fld(value, SHU_SELPH_CA7_DLY_RA5) |
																P_Fld(value, SHU_SELPH_CA7_DLY_RA6));

	// Note: CKE UI must sync CA UI (CA and CKE delay circuit are same) @Lin-Yi
	// To avoid tXP timing margin issue
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_CA5), value, SHU_SELPH_CA5_DLY_CKE);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_CA6), value, SHU_SELPH_CA6_DLY_CKE1);

	///TODO: Yirong : new calibration flow : change CS UI to 0
//	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_CA5), P_Fld(0x1, SHU_SELPH_CA5_DLY_CS) | P_Fld(0x1, SHU_SELPH_CA5_DLY_CS1));
}
#endif

void cbt_dfs_mr13_global(DRAMC_CTX_T *p, U8 freq)
{
	U8 u1RankIdx;
	U8 backup_rank;

	backup_rank = u1GetRank(p);

	for(u1RankIdx =0; u1RankIdx < p->support_rank_num; u1RankIdx++)
	{
		vSetRank(p, u1RankIdx);

		if (freq == CBT_LOW_FREQ)
		{
			DramcMRWriteFldAlign(p, 13, 0, MR13_FSP_OP, JUST_TO_GLOBAL_VALUE);
			DramcMRWriteFldAlign(p, 13, 0, MR13_FSP_WR, JUST_TO_GLOBAL_VALUE);
		}
		else // CBT_HIGH_FREQ
		{
			DramcMRWriteFldAlign(p, 13, 1, MR13_FSP_OP, JUST_TO_GLOBAL_VALUE);
			DramcMRWriteFldAlign(p, 13, 1, MR13_FSP_WR, JUST_TO_GLOBAL_VALUE);
		}
	}

	vSetRank(p, backup_rank);
}

void cbt_switch_freq(DRAMC_CTX_T *p, U8 freq)
{
#if (FOR_DV_SIMULATION_USED == TRUE) // @Darren+ for DV sim
	return;
#endif

#if MR_CBT_SWITCH_FREQ
#if (fcFOR_CHIP_ID == fcMargaux)
	static U8 _CurFreq = CBT_UNKNOWN_FREQ;
	if (_CurFreq == freq)
	{
		return; // Do nothing no meter the frequency is.
	}
	_CurFreq = freq;

	EnableDFSHwModeClk(p);

	if (freq == CBT_LOW_FREQ)
	{
		DramcDFSDirectJump_RGMode(p, DRAM_DFS_SHUFFLE_2); // Darren NOTE: Dramc shu1 for MRW (DramcModeRegInit_LP4 and CBT)
	}
	else
	{
		DramcDFSDirectJump_RGMode(p, DRAM_DFS_SHUFFLE_1);
	}

	vIO32WriteFldMulti_All(DDRPHY_REG_MISC_CLK_CTRL, P_Fld(0, MISC_CLK_CTRL_DVFS_CLK_MEM_SEL)
													| P_Fld(0, MISC_CLK_CTRL_DVFS_MEM_CK_MUX_UPDATE_EN));

	DDRPhyFreqMeter();
#else
	#error Need check of the DRAM_DFS_SHUFFLE_X for your chip !!!
#endif
#endif
}


void DramcPowerOnSequence(DRAMC_CTX_T *p)
{
#ifdef DUMP_INIT_RG_LOG_TO_DE
	//CKE high
	CKEFixOnOff(p, CKE_WRITE_TO_ALL_RANK, CKE_FIXON, CKE_WRITE_TO_ALL_CHANNEL);
	return;
#endif

	#if APPLY_LP4_POWER_INIT_SEQUENCE
	//static U8 u1PowerOn=0;
	//if(u1PowerOn ==0)
	{
	//reset dram = low
	vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CTRL1), 0x0, MISC_CTRL1_R_DMDA_RRESETB_I);

	//vIO32WriteFldAlign(DRAMC_REG_RKCFG, 0, RKCFG_CKE2RANK_OPT2);

	//CKE low
	CKEFixOnOff(p, CKE_WRITE_TO_ALL_RANK, CKE_FIXOFF, CKE_WRITE_TO_ALL_CHANNEL);

	// delay tINIT1=200us(min) & tINIT2=10ns(min)
	mcDELAY_US(200);

	//reset dram = low
	vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CTRL1), 0x1, MISC_CTRL1_R_DMDA_RRESETB_I);

	// Disable HW MIOCK control to make CLK always on
	vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), 1, DRAMC_PD_CTRL_APHYCKCG_FIXOFF);
	vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), 1, DRAMC_PD_CTRL_TCKFIXON);

	//tINIT3=2ms(min)
	mcDELAY_MS(2);

	//CKE high
	CKEFixOnOff(p, CKE_WRITE_TO_ALL_RANK, CKE_FIXON, CKE_WRITE_TO_ALL_CHANNEL);

	// tINIT5=2us(min)
	mcDELAY_US(2);
	//u1PowerOn=1;

	//// Enable  HW MIOCK control to make CLK dynamic
	vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), 0, DRAMC_PD_CTRL_TCKFIXON);
	vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), 0, DRAMC_PD_CTRL_APHYCKCG_FIXOFF);
	mcSHOW_DBG_MSG3(("APPLY_LP4_POWER_INIT_SEQUENCE\n"));
	}
	#endif
}

DRAM_STATUS_T DramcModeRegInit_CATerm(DRAMC_CTX_T *p, U8 bWorkAround)
{
	static U8 CATermWA[CHANNEL_NUM] = {0};
	U8 u1ChannelIdx, u1RankIdx, u1RankIdxBak;
	U32 backup_broadcast;
	U8 u1MR11_Value;
	U8 u1MR22_Value;

	u1ChannelIdx = vGetPHY2ChannelMapping(p);

	if (CATermWA[u1ChannelIdx] == bWorkAround)
		return DRAM_OK;

	CATermWA[u1ChannelIdx] = bWorkAround;

	backup_broadcast = GetDramcBroadcast();

	DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);

	u1RankIdxBak = u1GetRank(p);

	for (u1RankIdx = 0; u1RankIdx < (U32)(p->support_rank_num); u1RankIdx++)
	{
		vSetRank(p, u1RankIdx);

		mcSHOW_DBG_MSG(("[DramcModeRegInit_CATerm] CH%u RK%u bWorkAround=%d\n", u1ChannelIdx, u1RankIdx, bWorkAround));
		/* FSP_1: 1. For term freqs   2. Assumes "data rate >= DDR2667" are terminated */
		#if MRW_CHECK_ONLY
		mcSHOW_MRW_MSG(("\n==[MR Dump] %s==\n", __func__));
		#endif
		DramcMRWriteFldAlign(p, 13, 0, MR13_FSP_OP, TO_MR);
		DramcMRWriteFldAlign(p, 13, 1, MR13_FSP_WR, TO_MR);

		//MR12 use previous value

		if (p->dram_type == TYPE_LPDDR4P)
		{
			u1MR11_Value = 0x0; 				 //ODT disable
		}
		else
		{
#if ENABLE_SAMSUNG_NT_ODT
			if ((p->vendor_id == VENDOR_SAMSUNG) && (p->revision_id == 0x7)) // 1ynm process for NT-ODT
			{
				u1MR11_Value = 0x2; 			 //@Darren, DQ ODT:120ohm -> parallel to 60ohm
				u1MR11_Value |= (0x1 << 3);   //@Darren, MR11[3]=1 to enable NT-ODT for B707
			}
			else
#endif
				u1MR11_Value = 0x3; 			 //DQ ODT:80ohm

		#if FSP1_CLKCA_TERM
			if (p->dram_cbt_mode[u1RankIdx] == CBT_NORMAL_MODE)
			{
				u1MR11_Value |= 0x40;			   //CA ODT:60ohm for byte mode
			}
			else
			{
				u1MR11_Value |= 0x20;			   //CA ODT:120ohm for byte mode
			}
		#endif
		}
	#if APPLY_SIGNAL_WAVEFORM_SETTINGS_ADJUST
		if (gDramcDqOdtRZQAdjust >= 0)
			u1MR11_Value = gDramcDqOdtRZQAdjust;
	#endif
		u1MR11Value[p->dram_fsp] = u1MR11_Value;
		DramcModeRegWriteByRank(p, u1RankIdx, 11, u1MR11Value[p->dram_fsp]); //ODT

		if (p->dram_type == TYPE_LPDDR4)
		{
			u1MR22_Value = 0x24;	 //SOC-ODT, ODTE-CK, ODTE-CS, Disable ODTD-CA
		}
		else  //TYPE_LPDDR4x, LP4P
		{
			u1MR22_Value = 0x3c;	 //Disable CA-CS-CLK ODT, SOC ODT=RZQ/4
		#if FSP1_CLKCA_TERM
			if (bWorkAround)
			{
				u1MR22_Value = 0x4;
			}
			else
			{
				if (u1RankIdx == RANK_0)
				{
					u1MR22_Value = 0x4; 	//Enable CA-CS-CLK ODT, SOC ODT=RZQ/4
				}
				else
				{
					u1MR22_Value = 0x2c;	 //Enable CS ODT, SOC ODT=RZQ/4
				}
			}
		#endif
		}
	#if APPLY_SIGNAL_WAVEFORM_SETTINGS_ADJUST
		if (gDramcMR22SoCODTAdjust[u1MRFsp] >= 0)
		{
			u1MR22_Value = (u1MR22_Value & ~(0x7)) | gDramcMR22SoCODTAdjust[u1MRFsp];
		}
	#endif
		u1MR22Value[p->dram_fsp] = u1MR22_Value;
		DramcModeRegWriteByRank(p, u1RankIdx, 22, u1MR22_Value);
	}

	vSetRank(p, u1RankIdxBak);
//	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_MRS), RANK_0, MRS_MRSRK);

	DramcBroadcastOnOff(backup_broadcast);

	return DRAM_OK;
}

DRAM_STATUS_T DramcModeRegInit_LP4(DRAMC_CTX_T *p)
{
	U32 u4RankIdx;//, u4CKE0Bak, u4CKE1Bak, u4MIOCKBak, u4AutoRefreshBak;
	U8 u1MRFsp= FSP_0;
	U8 u1ChannelIdx;
	U8 backup_channel, backup_rank;
	U8 operating_fsp = p->dram_fsp;
	U32 backup_broadcast;
	U8 u1MR11_Value;
	U8 u1MR22_Value;
	U8 u1nWR=0;
	U16 u2FreqMax = u2DFSGetHighestFreq(p);
	U8 u1set_mrsrk=0;

	backup_broadcast = GetDramcBroadcast();

	DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);


	DramcPowerOnSequence(p);

	backup_channel = p->channel;
	backup_rank = p->rank;


#if VENDER_JV_LOG
	vPrintCalibrationBasicInfo_ForJV(p);
#endif

	/* Fix nWR value to 30 (MR01[6:4] = 101B) for DDR3200
	 * Fix nWR value to 34 (MR01[6:4] = 110B) for DDR3733
	 * Other vendors: Use default MR01 for each FSP (Set in vInitGlobalVariablesByCondition() )
	 */
	{
		/* Clear MR01 OP[6:4] */
		u1MR01Value[FSP_0] &= 0x8F;
		u1MR01Value[FSP_1] &= 0x8F;
		if (u2FreqMax == 2133)
		{
			/* Set MR01 OP[6:4] to 111B = 7 */
			u1MR01Value[FSP_0] |= (0x7 << 4);
			u1MR01Value[FSP_1] |= (0x7 << 4);
			u1nWR = 40;
		}
		else if (u2FreqMax == 1866)
		{
			/* Set MR01 OP[6:4] to 110B = 6 */
			u1MR01Value[FSP_0] |= (0x6 << 4);
			u1MR01Value[FSP_1] |= (0x6 << 4);
			u1nWR = 34;
		}
		else // Freq <= 1600
		{
			/* Set MR01 OP[6:4] to 101B = 5 */
			u1MR01Value[FSP_0] |= (0x5 << 4);
			u1MR01Value[FSP_1] |= (0x5 << 4);
			u1nWR = 30;
		}

		mcSHOW_DBG_MSG(("nWR fixed to %d\n", u1nWR));
		mcDUMP_REG_MSG(("nWR fixed to %d\n", u1nWR));
	}

#ifndef DUMP_INIT_RG_LOG_TO_DE
	if(p->dram_fsp == FSP_1)
	{
		// @Darren, VDDQ = 600mv + CaVref default is 301mV (no impact)
		// Fix high freq keep FSP0 for CA term (PPR abnormal)
		vIO32WriteFldMulti_All(DDRPHY_REG_CA_CMD2, P_Fld(1, CA_CMD2_RG_TX_ARCMD_OE_DIS_CA)
												| P_Fld(0, CA_CMD2_RG_TX_ARCA_OE_TIE_SEL_CA)
												| P_Fld(0xff, CA_CMD2_RG_TX_ARCA_OE_TIE_EN_CA));
		cbt_switch_freq(p, CBT_LOW_FREQ);
		vIO32WriteFldMulti_All(DDRPHY_REG_CA_CMD2, P_Fld(0, CA_CMD2_RG_TX_ARCMD_OE_DIS_CA)
												| P_Fld(1, CA_CMD2_RG_TX_ARCA_OE_TIE_SEL_CA)
												| P_Fld(0xff, CA_CMD2_RG_TX_ARCA_OE_TIE_EN_CA));
	}
#endif

	for(u1ChannelIdx=0; u1ChannelIdx<(p->support_channel_num); u1ChannelIdx++)
	{
		vSetPHY2ChannelMapping(p, u1ChannelIdx);

		for(u4RankIdx =0; u4RankIdx < (U32)(p->support_rank_num); u4RankIdx++)
		{
			vSetRank(p, u4RankIdx);

			mcSHOW_DBG_MSG(("[ModeRegInit_LP4] CH%u RK%u\n", u1ChannelIdx, u4RankIdx));
			mcDUMP_REG_MSG(("[ModeRegInit_LP4] CH%u RK%u\n", u1ChannelIdx, u4RankIdx));
		#if VENDER_JV_LOG
			mcSHOW_JV_LOG_MSG(("\n[ModeRegInit_LP4] CH%u RK%d\n", u1ChannelIdx, u4RankIdx));
		#endif
		#if MRW_CHECK_ONLY
			mcSHOW_MRW_MSG(("\n==[MR Dump] %s==\n", __func__));
		#endif

			//vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_MRS), u4RankIdx, MRS_MRSRK);

			// Note : MR37 for LP4P should be set before any Mode register.
			// MR37 is not shadow register, just need to set by channel and rank. No need to set by FSP
			if(p->dram_type == TYPE_LPDDR4P)
			{
				//temp solution, need remove later
			#ifndef MT6779_FPGA
			#if (FOR_DV_SIMULATION_USED==0 && SW_CHANGE_FOR_SIMULATION==0)
				#if __Petrus_TO_BE_PORTING__
					dramc_set_vddq_voltage(p->dram_type, 600000);
				#endif
			#endif
			#endif

				DramcModeRegWriteByRank(p, u4RankIdx, 37, 0x1);

				//temp solution, need remove later
			#ifndef MT6779_FPGA
			#if (FOR_DV_SIMULATION_USED==0 && SW_CHANGE_FOR_SIMULATION==0)
				#if __Petrus_TO_BE_PORTING__
					dramc_set_vddq_voltage(p->dram_type, 400000);
				#endif
			#endif
			#endif
			}


		   // if(p->frequency<=1200)
			{
				/* FSP_0: 1. For un-term freqs	 2. Assumes "data rate < DDR2667" are un-term */
				u1MRFsp = FSP_0;
				mcSHOW_DBG_MSG3(("\tFsp%d\n", u1MRFsp));
			#if VENDER_JV_LOG
				mcSHOW_JV_LOG_MSG(("\tFsp%d\n", u1MRFsp));
			#endif

				u1MR13Value[u4RankIdx] = 0;
				MRWriteFldMulti(p, 13, P_Fld(1, MR13_PRO) |
									   P_Fld(1, MR13_VRCG),
									   TO_MR);

				//MR12 use previous value
				DramcModeRegWriteByRank(p, u4RankIdx, 12, u1MR12Value[u1ChannelIdx][u4RankIdx][u1MRFsp]);
				DramcModeRegWriteByRank(p, u4RankIdx, 1, u1MR01Value[u1MRFsp]);

				//MR2 set Read/Write Latency
				if (p->freqGroup <= 400) // DDR800, DDR400
				{
					u1MR02Value[u1MRFsp] = 0x12; // the minimum of WL is 8, and the minimum of RL is 14 (Jouling and HJ)
				}
				else if ((p->freqGroup == 800) || (p->freqGroup == 600)) // DDR1600, DDR1200
				{
					u1MR02Value[u1MRFsp] = 0x12;
				}
				else if (p->freqGroup == 933) // DDR1866
				{
					u1MR02Value[u1MRFsp] = 0x1b; //RL=20, WL=10
				}
				else if (p->freqGroup == 1200) // DDR2280, DDR2400 (DDR2667 uses FSP_1)
				{
					u1MR02Value[u1MRFsp] = 0x24;
				}

				DramcModeRegWriteByRank(p, u4RankIdx, 2, u1MR02Value[u1MRFsp]);
				//if(p->odt_onoff)
				u1MR11Value[u1MRFsp] = 0x0;
				DramcModeRegWriteByRank(p, u4RankIdx, 11, u1MR11Value[u1MRFsp]); //ODT disable

#if ENABLE_LP4Y_DFS
				// For LPDDR4Y <= DDR1600 un-term
				if (p->freqGroup <= 800)
				{
					u1MR21Value[u1MRFsp] |= (0x1 << 5); // Low Speed CA buffer for LP4Y
					u1MR51Value[u1MRFsp] |= (0x3 << 1); // CLK[3]=0, WDQS[2]=1 and RDQS[1]=1 Single-End mode for LP4Y
				}
				else
#endif
				{
					u1MR21Value[u1MRFsp] = 0;
					u1MR51Value[u1MRFsp] = 0;
				}
				DramcModeRegWriteByRank(p, u4RankIdx, 21, u1MR21Value[u1MRFsp]);
				DramcModeRegWriteByRank(p, u4RankIdx, 51, u1MR51Value[u1MRFsp]);

				if(p->dram_type == TYPE_LPDDR4)
				{
					u1MR22_Value = 0x20;	 //SOC-ODT, ODTE-CK, ODTE-CS, Disable ODTD-CA
				}
				else  //TYPE_LPDDR4x, LP4P
				{
					u1MR22_Value = 0x38;	 //SOC-ODT, ODTE-CK, ODTE-CS, Disable ODTD-CA
				}
			#if APPLY_SIGNAL_WAVEFORM_SETTINGS_ADJUST
				if (gDramcMR22SoCODTAdjust[u1MRFsp]>=0)
				{
					u1MR22_Value = (u1MR22_Value&~(0x7))|gDramcMR22SoCODTAdjust[u1MRFsp];
				}
			#endif
				u1MR22Value[u1MRFsp] = u1MR22_Value;
				DramcModeRegWriteByRank(p, u4RankIdx, 22, u1MR22Value[u1MRFsp]);

				//MR14 use previous value
				DramcModeRegWriteByRank(p, u4RankIdx, 14, u1MR14Value[u1ChannelIdx][u4RankIdx][u1MRFsp]); //MR14 VREF-DQ

			#if CALIBRATION_SPEED_UP_DEBUG
				mcSHOW_DBG_MSG(("CBT Vref Init: CH%d Rank%d FSP%d, Range %d Vref %d\n\n",p->channel, p->rank, u1MRFsp, u1MR12Value[u1ChannelIdx][u4RankIdx][u1MRFsp]>>6, (u1MR12Value[u1ChannelIdx][u4RankIdx][u1MRFsp] & 0x3f)));
				mcSHOW_DBG_MSG(("TX Vref Init: CH%d Rank%d FSP%d, TX Range %d Vref %d\n\n",p->channel, p->rank, u1MRFsp,u1MR14Value[u1ChannelIdx][u4RankIdx][u1MRFsp]>>6, (u1MR14Value[u1ChannelIdx][u4RankIdx][u1MRFsp] & 0x3f)));
			#endif

				//MR3 set write-DBI and read-DBI (Disabled during calibration, enabled after K)
				u1MR03Value[u1MRFsp] = (u1MR03Value[u1MRFsp]&0x3F);

				if(p->dram_type == TYPE_LPDDR4X || p->dram_type == TYPE_LPDDR4P)
				{
					u1MR03Value[u1MRFsp] &= 0xfe;
				}
			#if APPLY_SIGNAL_WAVEFORM_SETTINGS_ADJUST
				if (gDramcMR03PDDSAdjust[u1MRFsp]>=0)
				{
					u1MR03Value[u1MRFsp] = (u1MR03Value[u1MRFsp]&~(0x7<<3))|(gDramcMR03PDDSAdjust[u1MRFsp]<<3);
				}
			#endif
				DramcModeRegWriteByRank(p, u4RankIdx, 3, u1MR03Value[u1MRFsp]);
				DramcModeRegWriteByRank(p, u4RankIdx, 4, u1MR04Value[u4RankIdx]);
				DramcModeRegWriteByRank(p, u4RankIdx, 3, u1MR03Value[u1MRFsp]);
			}
			//else
			{
				/* FSP_1: 1. For term freqs   2. Assumes "data rate >= DDR2667" are terminated */
				u1MRFsp = FSP_1;
				mcSHOW_DBG_MSG3(("\tFsp%d\n", u1MRFsp));
			#if VENDER_JV_LOG
				mcSHOW_JV_LOG_MSG(("\tFsp%d\n", u1MRFsp));
			#endif

				DramcMRWriteFldAlign(p, 13, 1, MR13_FSP_WR, TO_MR);

				//MR12 use previous value
				DramcModeRegWriteByRank(p, u4RankIdx, 12, u1MR12Value[u1ChannelIdx][u4RankIdx][u1MRFsp]); //MR12 VREF-CA
				DramcModeRegWriteByRank(p, u4RankIdx, 1, u1MR01Value[u1MRFsp]);

				//MR2 set Read/Write Latency
				if (p->freqGroup == 2133)
				{
					u1MR02Value[u1MRFsp] = 0x3f;
				}
				else if (p->freqGroup == 1866)
				{
					u1MR02Value[u1MRFsp] = 0x36;
				}
				else if (p->freqGroup == 1600)
				{
					u1MR02Value[u1MRFsp] = 0x2d;
				}
				else if (p->freqGroup == 1333)
				{
					u1MR02Value[u1MRFsp] = 0x24;
				}
				DramcModeRegWriteByRank(p, u4RankIdx, 2, u1MR02Value[u1MRFsp]);

				if(p->dram_type == TYPE_LPDDR4P)
					u1MR11_Value = 0x0; 				 //ODT disable
				else
				{
#if ENABLE_SAMSUNG_NT_ODT
					if ((p->vendor_id == VENDOR_SAMSUNG) && (p->revision_id == 0x7)) // 1ynm process for NT-ODT
					{
						u1MR11_Value = 0x2; 			 //@Darren, DQ ODT:120ohm -> parallel to 60ohm
						u1MR11_Value |= (0x1 << 3);   //@Darren, MR11[3]=1 to enable NT-ODT for B707
					}
					else
#endif
						u1MR11_Value = 0x3; 			 //DQ ODT:80ohm

				#if FSP1_CLKCA_TERM
					if(p->dram_cbt_mode[u4RankIdx]==CBT_NORMAL_MODE)
					{
						u1MR11_Value |= 0x40;			   //CA ODT:60ohm for normal mode die
					}
					else
					{
						u1MR11_Value |= 0x20;			   //CA ODT:120ohm for byte mode die
					}
				#endif
				}
			#if APPLY_SIGNAL_WAVEFORM_SETTINGS_ADJUST
					if (gDramcDqOdtRZQAdjust>=0)
					{
						u1MR11_Value &= ~(0x7);
						u1MR11_Value = gDramcDqOdtRZQAdjust;
					}
			#endif
					u1MR11Value[u1MRFsp] = u1MR11_Value;
					DramcModeRegWriteByRank(p, u4RankIdx, 11, u1MR11Value[u1MRFsp]); //ODT

				u1MR21Value[u1MRFsp] = 0;
				u1MR51Value[u1MRFsp] = 0;
				DramcModeRegWriteByRank(p, u4RankIdx, 21, u1MR21Value[u1MRFsp]);
				DramcModeRegWriteByRank(p, u4RankIdx, 51, u1MR51Value[u1MRFsp]);

				if(p->dram_type == TYPE_LPDDR4)
				{
					u1MR22_Value = 0x24;	 //SOC-ODT, ODTE-CK, ODTE-CS, Disable ODTD-CA
				}
				else  //TYPE_LPDDR4x, LP4P
				{
					u1MR22_Value = 0x3c;	 //Disable CA-CS-CLK ODT, SOC ODT=RZQ/4
				#if FSP1_CLKCA_TERM
					if(u4RankIdx==RANK_0)
					{
						u1MR22_Value = 0x4; 	//Enable CA-CS-CLK ODT, SOC ODT=RZQ/4
					}
					else
					{
						u1MR22_Value = 0x2c;	 //Enable CS ODT, SOC ODT=RZQ/4
					}
				#endif
				}
			#if APPLY_SIGNAL_WAVEFORM_SETTINGS_ADJUST
				if (gDramcMR22SoCODTAdjust[u1MRFsp]>=0)
				{
					u1MR22_Value = (u1MR22_Value&~(0x7))|gDramcMR22SoCODTAdjust[u1MRFsp];
				}
			#endif
				u1MR22Value[u1MRFsp] = u1MR22_Value;
				DramcModeRegWriteByRank(p, u4RankIdx, 22, u1MR22Value[u1MRFsp]);

				//MR14 use previous value
				DramcModeRegWriteByRank(p, u4RankIdx, 14, u1MR14Value[u1ChannelIdx][u4RankIdx][u1MRFsp]); //MR14 VREF-DQ

			#if CALIBRATION_SPEED_UP_DEBUG
				mcSHOW_DBG_MSG(("CBT Vref Init: CH%d Rank%d FSP%d, Range %d Vref %d\n\n",p->channel, p->rank, u1MRFsp, u1MR12Value[u1ChannelIdx][u4RankIdx][u1MRFsp]>>6, (u1MR12Value[u1ChannelIdx][u4RankIdx][u1MRFsp] & 0x3f)));
				mcSHOW_DBG_MSG(("TX Vref Init: CH%d Rank%d FSP%d, TX Range %d Vref %d\n\n",p->channel, p->rank, u1MRFsp, u1MR14Value[u1ChannelIdx][u4RankIdx][u1MRFsp]>>6, (u1MR14Value[u1ChannelIdx][u4RankIdx][u1MRFsp] & 0x3f)));
			#endif

				//MR3 set write-DBI and read-DBI (Disabled during calibration, enabled after K)
				u1MR03Value[u1MRFsp] = (u1MR03Value[u1MRFsp]&0x3F);

				if(p->dram_type == TYPE_LPDDR4X || p->dram_type == TYPE_LPDDR4P)
				{
					u1MR03Value[u1MRFsp] &= 0xfe;
				}
			#if APPLY_SIGNAL_WAVEFORM_SETTINGS_ADJUST
				if (gDramcMR03PDDSAdjust[u1MRFsp]>=0)
				{
					u1MR03Value[u1MRFsp] = (u1MR03Value[u1MRFsp]&~(0x7<<3))|(gDramcMR03PDDSAdjust[u1MRFsp]<<3);
				}
			#endif
				DramcModeRegWriteByRank(p, u4RankIdx, 3, u1MR03Value[u1MRFsp]);
				DramcModeRegWriteByRank(p, u4RankIdx, 4, u1MR04Value[u4RankIdx]);
				DramcModeRegWriteByRank(p, u4RankIdx, 3, u1MR03Value[u1MRFsp]);
			}


#if ENABLE_LP4_ZQ_CAL
			DramcZQCalibration(p, u4RankIdx); //ZQ calobration should be done before CBT calibration by switching to low frequency
#endif

			/* freq < 1333 is assumed to be odt_off -> uses FSP_0 */
			//if (p->frequency < MRFSP_TERM_FREQ)
			if(operating_fsp == FSP_0)
			{
				DramcMRWriteFldAlign(p, 13, 0, MR13_FSP_OP, JUST_TO_GLOBAL_VALUE);
				DramcMRWriteFldAlign(p, 13, 0, MR13_FSP_WR, JUST_TO_GLOBAL_VALUE);
			}
			else
			{
				DramcMRWriteFldAlign(p, 13, 1, MR13_FSP_OP, JUST_TO_GLOBAL_VALUE);
				DramcMRWriteFldAlign(p, 13, 1, MR13_FSP_WR, JUST_TO_GLOBAL_VALUE);
			}

		}
		vSetRank(p, backup_rank);

#if 0
		for(u4RankIdx =0; u4RankIdx < (U32)(p->support_rank_num); u4RankIdx++)
		{
			DramcModeRegWriteByRank(p, u4RankIdx, 13, u1MR13Value[RANK_0]);
		}
#else

		/* MRS two ranks simutaniously */
		if (p->support_rank_num == RANK_DUAL)
			u1set_mrsrk = 0x3;
		else
			u1set_mrsrk = RANK_0;
		DramcModeRegWriteByRank(p, u1set_mrsrk, 13, u1MR13Value[RANK_0]);
#endif

		/* Auto-MRW related register write (Used during HW DVFS frequency switch flow)
		 * VRCG seems to be enabled/disabled even when switching to same FSP(but different freq) to simplify HW DVFS flow
		 */
		// 1. MR13 OP[3] = 1 : Enable "high current mode" to reduce the settling time when changing FSP(freq) during operation
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_HWSET_MR13), P_Fld(u1MR13Value[RANK_0] | (0x1 << 3), SHU_HWSET_MR13_HWSET_MR13_OP)
																	| P_Fld(13, SHU_HWSET_MR13_HWSET_MR13_MRSMA));
		// 2. MR13 OP[3] = 1 : Enable "high current mode" after FSP(freq) switch operation for calibration
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_HWSET_VRCG), P_Fld(u1MR13Value[RANK_0] | (0x1 << 3), SHU_HWSET_VRCG_HWSET_VRCG_OP)
																	| P_Fld(13, SHU_HWSET_VRCG_HWSET_VRCG_MRSMA));
		// 3. MR2 : Set RL/WL after FSP(freq) switch
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_HWSET_MR2), P_Fld(u1MR02Value[operating_fsp], SHU_HWSET_MR2_HWSET_MR2_OP)
																	| P_Fld(2, SHU_HWSET_MR2_HWSET_MR2_MRSMA));
	}

#ifndef DUMP_INIT_RG_LOG_TO_DE
	//switch to high freq
	if(p->dram_fsp == FSP_1)
	{
		// @Darren, no effect via DDR1600 (purpose to keep @FSP0)
		// Fix high freq keep FSP0 for CA term (PPR abnormal)
		vIO32WriteFldMulti_All(DDRPHY_REG_CA_CMD2, P_Fld(1, CA_CMD2_RG_TX_ARCMD_OE_DIS_CA)
												| P_Fld(0, CA_CMD2_RG_TX_ARCA_OE_TIE_SEL_CA)
												| P_Fld(0xff, CA_CMD2_RG_TX_ARCA_OE_TIE_EN_CA));
		cbt_switch_freq(p, CBT_HIGH_FREQ);
		vIO32WriteFldMulti_All(DDRPHY_REG_CA_CMD2, P_Fld(0, CA_CMD2_RG_TX_ARCMD_OE_DIS_CA)
												| P_Fld(1, CA_CMD2_RG_TX_ARCA_OE_TIE_SEL_CA)
												| P_Fld(0xff, CA_CMD2_RG_TX_ARCA_OE_TIE_EN_CA));
	}
#endif
	vSetPHY2ChannelMapping(p, backup_channel);

	vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL0), RANK_0, SWCMD_CTRL0_MRSRK);

	DramcBroadcastOnOff(backup_broadcast);

#if SAMSUNG_TEST_MODE_MRS_FOR_PRELOADER
	vApplyProgramSequence(p);
#endif

	return DRAM_OK;
}

void MPLLInit(void)
{
#if (FOR_DV_SIMULATION_USED==0)
#if 0//__A60868_TO_BE_PORTING__

#if (FOR_DV_SIMULATION_USED == 0)
	unsigned int tmp;

	DRV_WriteReg32(AP_PLL_CON0, 0x11); // CLKSQ Enable
	mcDELAY_US(100);
	DRV_WriteReg32(AP_PLL_CON0, 0x13); // CLKSQ LPF Enable
	mcDELAY_MS(1);
	DRV_WriteReg32(MPLL_PWR_CON0, 0x3); // power on MPLL
	mcDELAY_US(30);
	DRV_WriteReg32(MPLL_PWR_CON0, 0x1); // turn off ISO of	MPLL
	mcDELAY_US(1);
	tmp = DRV_Reg32(MPLL_CON1);
	DRV_WriteReg32(MPLL_CON1, tmp | 0x80000000); // Config MPLL freq
	DRV_WriteReg32(MPLL_CON0, 0x181); // enable MPLL
	mcDELAY_US(20);
#endif
#else
	unsigned int tmp;

	DRV_WriteReg32(MPLL_CON3, 0x3); // power on MPLL

	mcDELAY_US(30);

	tmp = DRV_Reg32(MPLL_CON3);
	DRV_WriteReg32(MPLL_CON3, tmp & 0xFFFFFFFD); // turn off ISO of  MPLL

	mcDELAY_US(1);

	DRV_WriteReg32(MPLL_CON1, 0x84200000); // Config MPLL freq

	tmp = DRV_Reg32(MPLL_CON0);
	DRV_WriteReg32(MPLL_CON0, tmp | 0x1); // enable MPLL

	mcDELAY_US(20);

	tmp = DRV_Reg32(PLLON_CON0);
	DRV_WriteReg32(PLLON_CON0, tmp & ~(0x1 << 2)); // PLL_ISO  from SPM

	tmp = DRV_Reg32(PLLON_CON0);
	DRV_WriteReg32(PLLON_CON0, tmp & ~(0x1 << 11)); // PLL_EN  from SPM

	tmp = DRV_Reg32(PLLON_CON1);
	DRV_WriteReg32(PLLON_CON1, tmp & ~(0x1 << 20)); // PLL_PWR	from SPM

	tmp = DRV_Reg32(PLLON_CON2);
	DRV_WriteReg32(PLLON_CON2, tmp & ~(0x1 << 2)); // PLL SPEC

	tmp = DRV_Reg32(PLLON_CON3);
	DRV_WriteReg32(PLLON_CON3, tmp & ~(0x1 << 2)); // PLL SPEC
#endif
#endif
}


#if ENABLE_RODT_TRACKING_SAVE_MCK
void SetTxWDQSStatusOnOff(U8 u1OnOff)
{
	  u1WDQS_ON = u1OnOff;
}
#endif


#if XRTRTR_NEW_CROSS_RK_MODE
void XRTRTR_SHU_Setting(DRAMC_CTX_T * p)
{
	U8 u1RkSelUIMinus = 0, u1RkSelMCKMinus = 0;
	//U8 u1RankIdx = 0;
	//U8 u1Rank_backup = u1GetRank(p);

	if (vGet_DDR_Loop_Mode(p) == SEMI_OPEN_LOOP_MODE) // DDR800semi
		u1RkSelMCKMinus = 1;
	else if (p->frequency >= 1600) //DDR3200 up
		u1RkSelUIMinus = 2;

	// DV codes is included
	/*vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B0_DQ6, u1ShuRkMode, SHU_B0_DQ6_RG_RX_ARDQ_RANK_SEL_SER_MODE_B0);
	vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B1_DQ6, u1ShuRkMode, SHU_B1_DQ6_RG_RX_ARDQ_RANK_SEL_SER_MODE_B1);*/

	//DRAMC setting - @Darren, DV no set (double confirm)
	vIO32WriteFldMulti_All(DDRPHY_REG_SHU_MISC_RANK_SEL_STB, P_Fld(u1RkSelMCKMinus, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_MCK_MINUS)
									   | P_Fld(u1RkSelUIMinus, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_UI_MINUS)
									   | P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_MCK_PLUS)
									   | P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_UI_PLUS)
									   | P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_PHASE_EN)
									   | P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_RXDLY_TRACK)
									   | P_Fld(0x1, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_TRACK)
									   | P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_SERMODE) // @HJ, no use
									   | P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_EN_B23)
									   | P_Fld(0x1, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_EN));
	//Darren-vIO32WriteFldAlign_All(DRAMC_REG_SHU_STBCAL, 0x1, SHU_STBCAL_DQSIEN_RX_SELPH_OPT); //@HJ, internal wire assign to 1'b1

	/*for (u1RankIdx = RANK_0; u1RankIdx < p->support_rank_num; u1RankIdx++)
	{
		vSetRank(p, u1RankIdx);
		vIO32WriteFldMulti_All(DDRPHY_REG_MISC_SHU_RK_DQSCAL, P_Fld(0x1, MISC_SHU_RK_DQSCAL_DQSIENHLMTEN)
										   | P_Fld(0x3f, MISC_SHU_RK_DQSCAL_DQSIENHLMT)
										   | P_Fld(0x1, MISC_SHU_RK_DQSCAL_DQSIENLLMTEN)
										   | P_Fld(0x60, MISC_SHU_RK_DQSCAL_DQSIENLLMT));
	}
	vSetRank(p, u1Rank_backup);*/
}
#endif

#if XRTWTW_NEW_CROSS_RK_MODE
void XRTWTW_SHU_Setting(DRAMC_CTX_T * p)
{
	U8 u1RankIdx, u1ByteIdx;
	U8 u1Rank_bak = u1GetRank(p);
	U16 u2TxDly_OEN_RK[2][2] = {0}, u2TxPI_UPD[2] = {0}, u2TxRankINCTL, u2TxDly_OEN_RK_max, u2TxPI_UPD_max;

	for (u1RankIdx = RANK_0; u1RankIdx < p->support_rank_num; u1RankIdx++)
	{
		vSetRank(p, u1RankIdx);

		u2TxDly_OEN_RK[u1RankIdx][0] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ0), SHURK_SELPH_DQ0_TXDLY_OEN_DQ0); //Byte0
		u2TxDly_OEN_RK[u1RankIdx][1] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ0), SHURK_SELPH_DQ0_TXDLY_OEN_DQ1); //Byte1
	}
	vSetRank(p, u1Rank_bak);

	for (u1ByteIdx = 0; u1ByteIdx < (p->data_width / DQS_BIT_NUMBER); u1ByteIdx++)
	{
		u2TxDly_OEN_RK_max = (u2TxDly_OEN_RK[0][u1ByteIdx] > u2TxDly_OEN_RK[1][u1ByteIdx])? u2TxDly_OEN_RK[0][u1ByteIdx]: u2TxDly_OEN_RK[1][u1ByteIdx];
		if (p->frequency >= 1200)
			u2TxPI_UPD[u1ByteIdx] = (u2TxDly_OEN_RK_max > 2)? (u2TxDly_OEN_RK_max - 2): 0; //Byte0
		else
			u2TxPI_UPD[u1ByteIdx] = (u2TxDly_OEN_RK_max > 1)? (u2TxDly_OEN_RK_max - 1): 0; //Byte0
	}

	u2TxPI_UPD_max = (u2TxPI_UPD[0] > u2TxPI_UPD[1])? u2TxPI_UPD[0]: u2TxPI_UPD[1];
	u2TxRankINCTL = (u2TxPI_UPD_max > 1)? (u2TxPI_UPD_max - 1): 0;

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_NEW_XRW2W_CTRL), P_Fld(0x0, SHU_NEW_XRW2W_CTRL_TXPI_UPD_MODE)
										   | P_Fld(u2TxPI_UPD[0], SHU_NEW_XRW2W_CTRL_TX_PI_UPDCTL_B0)
										   | P_Fld(u2TxPI_UPD[1], SHU_NEW_XRW2W_CTRL_TX_PI_UPDCTL_B1));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_TX_RANKCTL), P_Fld(0x0, SHU_TX_RANKCTL_TXRANKINCTL_ROOT)
											| P_Fld(u2TxRankINCTL, SHU_TX_RANKCTL_TXRANKINCTL)
											| P_Fld(u2TxRankINCTL, SHU_TX_RANKCTL_TXRANKINCTL_TXDLY));
}
#endif


#if CMD_CKE_WORKAROUND_FIX
void CMD_CKE_Modified_txp_Setting(DRAMC_CTX_T * p)
{
#if __A60868_TO_BE_PORTING__

	U8 u1CmdTxPipe = 0, u1CkTxPipe = 0, u1SrefPdSel = 0;

	if (vGet_Div_Mode(p) == DIV4_MODE)
	{
		u1CkTxPipe = 1;
		u1SrefPdSel = 1;
	}
	else if (p->frequency >= 1866)
	{
		u1CmdTxPipe = 1;
		u1CkTxPipe = 1;
	}

	vIO32WriteFldAlign(DDRPHY_SHU_MISC0, u1CkTxPipe, SHU_MISC0_RG_CK_TXPIPE_BYPASS_EN);
	vIO32WriteFldAlign(DDRPHY_SHU_MISC0, u1CmdTxPipe, SHU_MISC0_RG_CMD_TXPIPE_BYPASS_EN);
	//vIO32WriteFldAlign(DRAMC_REG_SHU_CONF0, u1SrefPdSel, SHU_CONF0_SREF_PD_SEL);
#endif //__A60868_TO_BE_PORTING__
}
#endif

#if TX_OE_EXTEND
static void UpdateTxOEN(DRAMC_CTX_T *p)
{
	U8 u1ByteIdx, backup_rank, ii;
	U8 u1DQ_OE_CNT;

	// For LP4
	// 1. R_DMDQOE_OPT (dramc_conf 0x8C0[11])
	//	  set 1'b1: adjust DQSOE/DQOE length with R_DMDQOE_CNT
	// 2. R_DMDQOE_CNT (dramc_conf 0x8C0[10:8])
	//	  set 3'h3
	// 3. Initial TX setting OE/DATA
	//	  OE = DATA - 4 UI

	// For LP3
	// 1. R_DMDQOE_OPT (dramc_conf 0x8C0[11])
	//	  set 1'b1: adjust DQSOE/DQOE length with R_DMDQOE_CNT
	// 2. R_DMDQOE_CNT (dramc_conf 0x8C0[10:8])
	//	  set 3'h2
	// 3. Initial TX setting OE/DATA
	//	  OE = DATA - 2 UI

	u1DQ_OE_CNT = 3;

	vIO32WriteFldMulti(DRAMC_REG_SHU_TX_SET0, P_Fld(1, SHU_TX_SET0_DQOE_OPT) | P_Fld(u1DQ_OE_CNT, SHU_TX_SET0_DQOE_CNT));

	backup_rank = u1GetRank(p);

	LP4_ShiftDQS_OENUI(p, -1, ALL_BYTES);
	ShiftDQ_OENUI_AllRK(p, -1, ALL_BYTES);

}
#endif

#if ENABLE_DUTY_SCAN_V2
#ifdef DDR_INIT_TIME_PROFILING
U32 gu4DutyCalibrationTime;
#endif
#endif

static void vReplaceDVInit(DRAMC_CTX_T *p)
{
	U8 u1RandIdx, backup_rank = 0;

	backup_rank = p->rank;

	//Disable RX Tracking
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ5), P_Fld(0, B1_DQ5_RG_RX_ARDQS0_DVS_EN_B1));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ5), P_Fld(0, B0_DQ5_RG_RX_ARDQS0_DVS_EN_B0));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_RXDVS0), P_Fld(0, B0_RXDVS0_R_RX_DLY_TRACK_ENA_B0)
															| P_Fld(0, B0_RXDVS0_R_RX_DLY_TRACK_CG_EN_B0 ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_RXDVS0), P_Fld(0, B1_RXDVS0_R_RX_DLY_TRACK_ENA_B1)
															| P_Fld(0, B1_RXDVS0_R_RX_DLY_TRACK_CG_EN_B1 ));

	for(u1RandIdx = RANK_0; u1RandIdx < p->support_rank_num; u1RandIdx++)
	{
		vSetRank(p, u1RandIdx);
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_RK_B0_RXDVS2), P_Fld(0, RK_B0_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B0)
																	| P_Fld(0, RK_B0_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B0)
																	| P_Fld(0, RK_B0_RXDVS2_R_RK0_DVS_MODE_B0));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_RK_B1_RXDVS2), P_Fld(0, RK_B1_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B1)
																	| P_Fld(0, RK_B1_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B1)
																	| P_Fld(0, RK_B1_RXDVS2_R_RK0_DVS_MODE_B1));
	}
	vSetRank(p, backup_rank);

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL1), 0, CBT_WLEV_CTRL1_CATRAINLAT);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL1), 0, SWCMD_CTRL1_WRFIFO_MODE2);


	//Bringup setting review

	{
		U32 backup_broadcast = GetDramcBroadcast();
		DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);

		U8 u1DQ_HYST_SEL=0x1, u1CA_HYST_SEL=0x1;
		U8 u1DQ_CAP_SEL=0x18, u1CA_CAP_SEL=0x18;
		//Critical
		//APHY
		if(p->frequency<=933)
		{
			u1DQ_HYST_SEL = 0x1;
			u1CA_HYST_SEL = 0x1;
		}
		else
		{
			u1DQ_HYST_SEL = 0x0;
			u1CA_HYST_SEL = 0x0;
		}

		if(p->frequency<=933)
		{
			u1DQ_CAP_SEL= 0x18;
			u1CA_CAP_SEL= 0x18;
		}
		else if(p->frequency<=1200)
		{
			u1DQ_CAP_SEL= 0x14;
			u1CA_CAP_SEL= 0x14;
		}
		else if(p->frequency<=1600)
		{
			u1DQ_CAP_SEL= 0x4;
			u1CA_CAP_SEL= 0x4;
		}
		else if(p->frequency<=2133)
		{
			u1DQ_CAP_SEL= 0x2;
			u1CA_CAP_SEL= 0x2;
		}

		vIO32WriteFldMulti_All(DDRPHY_REG_SHU_B0_DQ6, P_Fld(u1DQ_HYST_SEL, SHU_B0_DQ6_RG_ARPI_HYST_SEL_B0)
													| P_Fld(u1DQ_CAP_SEL, SHU_B0_DQ6_RG_ARPI_CAP_SEL_B0));
		vIO32WriteFldMulti_All(DDRPHY_REG_SHU_B1_DQ6, P_Fld(u1DQ_HYST_SEL, SHU_B1_DQ6_RG_ARPI_HYST_SEL_B1)
													| P_Fld(u1DQ_CAP_SEL, SHU_B1_DQ6_RG_ARPI_CAP_SEL_B1));
		vIO32WriteFldMulti_All(DDRPHY_REG_SHU_CA_CMD6, P_Fld(u1CA_HYST_SEL, SHU_CA_CMD6_RG_ARPI_HYST_SEL_CA)
													| P_Fld(u1CA_CAP_SEL, SHU_CA_CMD6_RG_ARPI_CAP_SEL_CA));

		//Jeremy
		vIO32WriteFldMulti_All(DDRPHY_REG_SHU_B0_DQ2,P_Fld((p->frequency>=2133), SHU_B0_DQ2_RG_ARPI_PSMUX_XLATCH_FORCE_DQS_B0)
													| P_Fld((p->frequency>=2133), SHU_B0_DQ2_RG_ARPI_PSMUX_XLATCH_FORCE_DQ_B0)
													| P_Fld(0, SHU_B0_DQ2_RG_ARPI_SMT_XLATCH_FORCE_DQS_B0)
													| P_Fld(0, SHU_B0_DQ2_RG_ARPI_SMT_XLATCH_DQ_FORCE_B0));
		vIO32WriteFldMulti_All(DDRPHY_REG_SHU_B1_DQ2,P_Fld((p->frequency>=2133), SHU_B1_DQ2_RG_ARPI_PSMUX_XLATCH_FORCE_DQS_B1)
													| P_Fld((p->frequency>=2133), SHU_B1_DQ2_RG_ARPI_PSMUX_XLATCH_FORCE_DQ_B1)
													| P_Fld(0, SHU_B1_DQ2_RG_ARPI_SMT_XLATCH_FORCE_DQS_B1)
													| P_Fld(0, SHU_B1_DQ2_RG_ARPI_SMT_XLATCH_DQ_FORCE_B1));
		vIO32WriteFldMulti_All(DDRPHY_REG_SHU_CA_CMD2,P_Fld((p->frequency>=2133), SHU_CA_CMD2_RG_ARPI_PSMUX_XLATCH_FORCE_CLK_CA)
													| P_Fld((p->frequency>=2133), SHU_CA_CMD2_RG_ARPI_PSMUX_XLATCH_FORCE_CA_CA)
													| P_Fld((p->frequency<=300), SHU_CA_CMD2_RG_ARPI_SMT_XLATCH_FORCE_CLK_CA)
													| P_Fld((p->frequency<=300), SHU_CA_CMD2_RG_ARPI_SMT_XLATCH_CA_FORCE_CA));

		//disable RX PIPE for RX timing pass
		vIO32WriteFldAlign_All(DDRPHY_REG_SHU_MISC_RX_PIPE_CTRL, 0x0, SHU_MISC_RX_PIPE_CTRL_RX_PIPE_BYPASS_EN);

		//Disable MD32 IRQ
		vIO32Write4B_All(DDRPHY_REG_MISC_DBG_IRQ_CTRL1, 0x0);
		vIO32Write4B_All(DDRPHY_REG_MISC_DBG_IRQ_CTRL4, 0x0);
		vIO32Write4B_All(DDRPHY_REG_MISC_DBG_IRQ_CTRL7, 0x0);

		//Disable NEW RX DCM mode
		vIO32WriteFldMulti_All(DDRPHY_REG_MISC_SHU_RX_CG_CTRL, P_Fld(0, MISC_SHU_RX_CG_CTRL_RX_DCM_WAIT_DLE_EXT_DLY)
															| P_Fld(2, MISC_SHU_RX_CG_CTRL_RX_DCM_EXT_DLY)
															| P_Fld(0, MISC_SHU_RX_CG_CTRL_RX_APHY_CTRL_DCM_OPT)
															| P_Fld(0, MISC_SHU_RX_CG_CTRL_RX_DCM_OPT));

		vIO32WriteFldAlign_All(DRAMC_REG_HMR4, 0, HMR4_MR4INT_LIMITEN);
		vIO32WriteFldAlign_All(DRAMC_REG_REFCTRL1, 0, REFCTRL1_REFPEND_OPT1);
		vIO32WriteFldAlign_All(DRAMC_REG_REFCTRL3, 0, REFCTRL3_REF_DERATING_EN);

		vIO32WriteFldMulti_All(DRAMC_REG_DRAMC_IRQ_EN, P_Fld(0x3fff, DRAMC_IRQ_EN_DRAMC_IRQ_EN_RSV)
													| P_Fld(0x0, DRAMC_IRQ_EN_MR4INT_EN));
		vIO32WriteFldAlign_All(DRAMC_REG_SHU_CONF0, 0, SHU_CONF0_PBREFEN);



		vIO32WriteFldAlign_All(DDRPHY_REG_CA_TX_MCK, 0x1, CA_TX_MCK_R_DMRESET_FRPHY_OPT);
		vIO32WriteFldAlign_All(DDRPHY_REG_MISC_DVFSCTL2, 0x1, MISC_DVFSCTL2_RG_ADA_MCK8X_EN_SHUFFLE);
		vIO32WriteFldAlign_All(DDRPHY_REG_MISC_IMPCAL, 0x1, MISC_IMPCAL_IMPBINARY);

		vIO32WriteFldMulti_All(DDRPHY_REG_SHU_B0_DQ10, P_Fld(0x1, SHU_B0_DQ10_RG_RX_ARDQS_DQSSTB_RPST_HS_EN_B0)
													| P_Fld(0x1, SHU_B0_DQ10_RG_RX_ARDQS_DQSSTB_CG_EN_B0));
		vIO32WriteFldMulti_All(DDRPHY_REG_SHU_B1_DQ10, P_Fld(0x1, SHU_B1_DQ10_RG_RX_ARDQS_DQSSTB_RPST_HS_EN_B1)
													| P_Fld(0x1, SHU_B1_DQ10_RG_RX_ARDQS_DQSSTB_CG_EN_B1));

		vIO32WriteFldMulti_All(DDRPHY_REG_SHU_B0_DQ8, P_Fld(1, SHU_B0_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B0)
													| P_Fld(1, SHU_B0_DQ8_R_DMRANK_PIPE_CG_IG_B0)
													| P_Fld(1, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B0)
													| P_Fld(1, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B0)
													| P_Fld(1, SHU_B0_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B0)
													| P_Fld(1, SHU_B0_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B0)
													| P_Fld(1, SHU_B0_DQ8_R_DMRXDLY_CG_IG_B0)
													| P_Fld(1, SHU_B0_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B0)
													| P_Fld(1, SHU_B0_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B0)
													| P_Fld(1, SHU_B0_DQ8_R_RMRODTEN_CG_IG_B0)
													| P_Fld(1, SHU_B0_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B0));
		vIO32WriteFldMulti_All(DDRPHY_REG_SHU_B1_DQ8, P_Fld(1, SHU_B1_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B1)
													| P_Fld(1, SHU_B1_DQ8_R_DMRANK_PIPE_CG_IG_B1)
													| P_Fld(1, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B1)
													| P_Fld(1, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B1)
													| P_Fld(1, SHU_B1_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B1)
													| P_Fld(1, SHU_B1_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B1)
													| P_Fld(1, SHU_B1_DQ8_R_DMRXDLY_CG_IG_B1)
													| P_Fld(1, SHU_B1_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B1)
													| P_Fld(1, SHU_B1_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B1)
													| P_Fld(1, SHU_B1_DQ8_R_RMRODTEN_CG_IG_B1)
													| P_Fld(1, SHU_B1_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B1));

		vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B0_DLL2, 0x1, SHU_B0_DLL2_RG_ARDQ_REV_B0);
		vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B1_DLL2, 0x1, SHU_B1_DLL2_RG_ARDQ_REV_B1);
		vIO32WriteFldAlign_All(DDRPHY_REG_SHU_CA_DLL2, 0x1, SHU_CA_DLL2_RG_ARCMD_REV);	  //Jeremy

		#if 1
		//Follow DE - DRAMC
		//vIO32WriteFldAlign_All(DRAMC_REG_DDRCOMMON0, 1, DDRCOMMON0_DISSTOP26M);
		//vIO32WriteFldAlign_All(DRAMC_REG_TEST2_A3, 1, TEST2_A3_TEST_AID_EN);
		//vIO32WriteFldAlign_All(DRAMC_REG_TEST2_A4, 0, TEST2_A4_TESTAGENTRKSEL);
		vIO32WriteFldAlign_All(DRAMC_REG_DUMMY_RD, 0, DUMMY_RD_DQSG_DMYRD_EN);
		vIO32WriteFldAlign_All(DRAMC_REG_DRAMC_DBG_SEL1, 0x1e, DRAMC_DBG_SEL1_DEBUG_SEL_0);
		vIO32WriteFldAlign_All(DRAMC_REG_SWCMD_CTRL2, 0x20, SWCMD_CTRL2_RTSWCMD_AGE);
		vIO32WriteFldAlign_All(DRAMC_REG_RTMRW_CTRL0, 0x20, RTMRW_CTRL0_RTMRW_AGE);


		vIO32WriteFldMulti_All(DRAMC_REG_DLLFRZ_CTRL, P_Fld(0, DLLFRZ_CTRL_DLLFRZ) | P_Fld(0, DLLFRZ_CTRL_DLLFRZ_MON_PBREF_OPT));
		vIO32WriteFldMulti_All(DRAMC_REG_MPC_CTRL, P_Fld(1, MPC_CTRL_RTSWCMD_HPRI_EN) | P_Fld(1, MPC_CTRL_RTMRW_HPRI_EN));
		vIO32WriteFldMulti_All(DRAMC_REG_HW_MRR_FUN, P_Fld(0, HW_MRR_FUN_R2MRRHPRICTL) | P_Fld(0, HW_MRR_FUN_TR2MRR_ENA));
		vIO32WriteFldMulti_All(DRAMC_REG_ACTIMING_CTRL, P_Fld(1, ACTIMING_CTRL_REFNA_OPT) | P_Fld(1, ACTIMING_CTRL_SEQCLKRUN3));
		vIO32WriteFldAlign_All(DRAMC_REG_CKECTRL, 1, CKECTRL_RUNTIMEMRRCKEFIX);
		vIO32WriteFldMulti_All(DRAMC_REG_DVFS_CTRL0, P_Fld(0, DVFS_CTRL0_DVFS_SYNC_MASK) | P_Fld(1, DVFS_CTRL0_R_DVFS_SREF_OPT));
		vIO32WriteFldAlign_All(DRAMC_REG_DVFS_TIMING_CTRL1, 1, DVFS_TIMING_CTRL1_SHU_PERIOD_GO_ZERO_CNT);
		vIO32WriteFldMulti_All(DRAMC_REG_HMR4, P_Fld(1, HMR4_REFRCNT_OPT)
											| P_Fld(0, HMR4_REFR_PERIOD_OPT)
											| P_Fld(1, HMR4_SPDR_MR4_OPT)//Resume from S0, trigger HW MR4
											| P_Fld(0, HMR4_HMR4_TOG_OPT));
		vIO32WriteFldAlign_All(DRAMC_REG_RX_SET0, 0, RX_SET0_SMRR_UPD_OLD);
		vIO32WriteFldAlign_All(DRAMC_REG_DRAMCTRL, 1, DRAMCTRL_SHORTQ_OPT);
		vIO32WriteFldAlign_All(DRAMC_REG_MISCTL0, 1, MISCTL0_REFP_ARBMASK_PBR2PBR_PA_DIS);
#if ENABLE_EARLY_BG_CMD==0
		vIO32WriteFldAlign_All(DRAMC_REG_PERFCTL0, 0, PERFCTL0_EBG_EN);
#endif
		vIO32WriteFldMulti_All(DRAMC_REG_CLKAR, P_Fld(1, CLKAR_REQQUECLKRUN) | P_Fld(0x7fff, CLKAR_REQQUE_PACG_DIS));
		vIO32WriteFldMulti_All(DRAMC_REG_REFCTRL0, P_Fld(0, REFCTRL0_PBREF_BK_REFA_ENA) | P_Fld(0, REFCTRL0_PBREF_BK_REFA_NUM));
		vIO32WriteFldAlign_All(DRAMC_REG_REFCTRL1, 0, REFCTRL1_REF_OVERHEAD_SLOW_REFPB_ENA);
		vIO32WriteFldAlign_All(DRAMC_REG_REFCTRL1, 0, REFCTRL1_REFPB2AB_IGZQCS);
		vIO32WriteFldAlign_All(DRAMC_REG_REFCTRL1, 1, REFCTRL1_REFPENDINGINT_OPT1); // @Darren, sync MP settings from Derping
		vIO32WriteFldAlign_All(DRAMC_REG_REF_BOUNCE1,5, REF_BOUNCE1_REFRATE_DEBOUNCE_TH);
		vIO32WriteFldAlign_All(DRAMC_REG_REFPEND2, 8, REFPEND2_MPENDREFCNT_TH8);
		vIO32WriteFldAlign_All(DRAMC_REG_SCSMCTRL, 0, SCSMCTRL_SC_PG_MAN_DIS);
		vIO32WriteFldMulti_All(DRAMC_REG_SCSMCTRL_CG, P_Fld(1, SCSMCTRL_CG_SCSM_CGAR)
													| P_Fld(1, SCSMCTRL_CG_SCARB_SM_CGAR));
		vIO32WriteFldAlign_All(DRAMC_REG_RTSWCMD_CNT, 0x30, RTSWCMD_CNT_RTSWCMD_CNT);
		vIO32WriteFldAlign_All(DRAMC_REG_DRAMC_IRQ_EN, 0x3fff, DRAMC_IRQ_EN_DRAMC_IRQ_EN_RSV);
		vIO32WriteFldAlign_All(DRAMC_REG_SHU_DCM_CTRL0, 1, SHU_DCM_CTRL0_DDRPHY_CLK_EN_OPT);
		vIO32WriteFldMulti_All(DRAMC_REG_SHU_HMR4_DVFS_CTRL0, P_Fld(0x1ff, SHU_HMR4_DVFS_CTRL0_REFRCNT) | P_Fld(0, SHU_HMR4_DVFS_CTRL0_FSPCHG_PRDCNT));
		vIO32WriteFldAlign_All(DRAMC_REG_SHU_HWSET_VRCG, 11, SHU_HWSET_VRCG_VRCGDIS_PRDCNT);
		vIO32WriteFldAlign_All(DRAMC_REG_SHU_MISC, 2, SHU_MISC_REQQUE_MAXCNT);


		//Follow DE - DDRPHY
		vIO32WriteFldMulti_All(DDRPHY_REG_B0_DLL_ARPI4, P_Fld(1, B0_DLL_ARPI4_RG_ARPI_BYPASS_SR_DQS_B0) | P_Fld(1, B0_DLL_ARPI4_RG_ARPI_BYPASS_SR_DQ_B0));
		vIO32WriteFldMulti_All(DDRPHY_REG_B1_DLL_ARPI4, P_Fld(1, B1_DLL_ARPI4_RG_ARPI_BYPASS_SR_DQS_B1) | P_Fld(1, B1_DLL_ARPI4_RG_ARPI_BYPASS_SR_DQ_B1));
		vIO32WriteFldMulti_All(DDRPHY_REG_CA_DLL_ARPI4, P_Fld(1, CA_DLL_ARPI4_RG_ARPI_BYPASS_SR_CLK_CA) | P_Fld(1, CA_DLL_ARPI4_RG_ARPI_BYPASS_SR_CA_CA));
		vIO32WriteFldMulti_All(DDRPHY_REG_CA_CMD11, P_Fld(0xa, CA_CMD11_RG_RRESETB_DRVN) | P_Fld(0xa, CA_CMD11_RG_RRESETB_DRVP));
		vIO32WriteFldAlign_All(DDRPHY_REG_MISC_CG_CTRL2, 0x1f, MISC_CG_CTRL2_RG_MEM_DCM_IDLE_FSEL);

#if 1 // Darren- for DDR400 open loop mode disable
		vIO32WriteFldMulti_All(DDRPHY_REG_MISC_CG_CTRL9, P_Fld(0, MISC_CG_CTRL9_RG_MCK4X_O_FB_CK_CG_OFF)
														| P_Fld(0, MISC_CG_CTRL9_RG_CG_DDR400_MCK4X_O_OFF)
														| P_Fld(0, MISC_CG_CTRL9_RG_MCK4X_O_OPENLOOP_MODE_EN)
														| P_Fld(0, MISC_CG_CTRL9_RG_MCK4X_Q_FB_CK_CG_OFF)
														| P_Fld(0, MISC_CG_CTRL9_RG_CG_DDR400_MCK4X_Q_OFF)
														| P_Fld(0, MISC_CG_CTRL9_RG_MCK4X_Q_OPENLOOP_MODE_EN)
														| P_Fld(0, MISC_CG_CTRL9_RG_MCK4X_I_FB_CK_CG_OFF)
														| P_Fld(0, MISC_CG_CTRL9_RG_CG_DDR400_MCK4X_I_OFF)
														| P_Fld(0, MISC_CG_CTRL9_RG_MCK4X_I_OPENLOOP_MODE_EN)
														| P_Fld(0, MISC_CG_CTRL9_RG_M_CK_OPENLOOP_MODE_EN));
#endif
		//Darren-vIO32WriteFldAlign_All(DDRPHY_REG_MISC_DVFSCTL, 1, MISC_DVFSCTL_R_SHUFFLE_PI_RESET_ENABLE);
		vIO32WriteFldMulti_All(DDRPHY_REG_MISC_DVFSCTL2, P_Fld(1, MISC_DVFSCTL2_RG_ADA_MCK8X_EN_SHUFFLE)
														| P_Fld(0, MISC_DVFSCTL2_RG_DLL_SHUFFLE)); // Darren-

		vIO32WriteFldMulti_All(DDRPHY_REG_MISC_DVFSCTL3, P_Fld(0x10, MISC_DVFSCTL3_RG_CNT_PHY_ST_DELAY_AFT_CHG_TO_BCLK)
														| P_Fld(1, MISC_DVFSCTL3_RG_DVFS_MEM_CK_SEL_SOURCE)
														| P_Fld(3, MISC_DVFSCTL3_RG_DVFS_MEM_CK_SEL_DESTI)
														| P_Fld(1, MISC_DVFSCTL3_RG_PHY_ST_DELAY_BEF_CHG_TO_BCLK)
														| P_Fld(1, MISC_DVFSCTL3_RG_PHY_ST_DELAY_AFT_CHG_TO_MCLK));

		//Darren-vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 0, MISC_RG_DFS_CTRL_SPM_DVFS_CONTROL_SEL);
		vIO32WriteFldAlign_All(DDRPHY_REG_MISC_DDR_RESERVE, 0xf, MISC_DDR_RESERVE_WDT_CONF_ISO_CNT);
		vIO32WriteFldMulti_All(DDRPHY_REG_MISC_IMP_CTRL1, P_Fld(1, MISC_IMP_CTRL1_RG_RIMP_SUS_ECO_OPT) | P_Fld(1, MISC_IMP_CTRL1_IMP_ABN_LAT_CLR));
		vIO32WriteFldMulti_All(DDRPHY_REG_MISC_IMPCAL, P_Fld(1, MISC_IMPCAL_IMPCAL_BYPASS_UP_CA_DRV)
													| P_Fld(1, MISC_IMPCAL_IMPCAL_DRVUPDOPT)
													| P_Fld(1, MISC_IMPCAL_IMPBINARY)
													| P_Fld(1, MISC_IMPCAL_DQDRVSWUPD)
													| P_Fld(0, MISC_IMPCAL_DRVCGWREF));

		vIO32WriteFldMulti_All(DDRPHY_REG_MISC_DUTYSCAN1, P_Fld(1, MISC_DUTYSCAN1_EYESCAN_DQS_OPT) | P_Fld(1, MISC_DUTYSCAN1_RX_EYE_SCAN_CG_EN));
		vIO32WriteFldAlign_All(DDRPHY_REG_MISC_DVFS_EMI_CLK, 0, MISC_DVFS_EMI_CLK_RG_DLL_SHUFFLE_DDRPHY);
		vIO32WriteFldMulti_All(DDRPHY_REG_MISC_CTRL0, P_Fld(0, MISC_CTRL0_IDLE_DCM_CHB_CDC_ECO_OPT)
													| P_Fld(1, MISC_CTRL0_IMPCAL_CDC_ECO_OPT)
													| P_Fld(1, MISC_CTRL0_IMPCAL_LP_ECO_OPT));

		vIO32WriteFldMulti_All(DDRPHY_REG_MISC_CTRL4, P_Fld(0, MISC_CTRL4_R_OPT2_CG_CS)
													| P_Fld(0, MISC_CTRL4_R_OPT2_CG_CLK)
													| P_Fld(0, MISC_CTRL4_R_OPT2_CG_CMD)
													| P_Fld(0, MISC_CTRL4_R_OPT2_CG_DQSIEN)
													| P_Fld(0, MISC_CTRL4_R_OPT2_CG_DQ)
													| P_Fld(0, MISC_CTRL4_R_OPT2_CG_DQS)
													| P_Fld(0, MISC_CTRL4_R_OPT2_CG_DQM)
													| P_Fld(0, MISC_CTRL4_R_OPT2_CG_MCK)
													| P_Fld(0, MISC_CTRL4_R_OPT2_MPDIV_CG));
		vIO32WriteFldMulti_All(DDRPHY_REG_MISC_CTRL6, P_Fld(1, MISC_CTRL6_RG_ADA_MCK8X_EN_SHU_OPT) | P_Fld(1, MISC_CTRL6_RG_PHDET_EN_SHU_OPT));

		vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RX_AUTOK_CFG0, 1, MISC_RX_AUTOK_CFG0_RX_CAL_CG_EN);

		vIO32WriteFldMulti_All(DDRPHY_REG_SHU_B0_DQ1, P_Fld(1, SHU_B0_DQ1_RG_ARPI_MIDPI_BYPASS_EN_B0)
													| P_Fld(1, SHU_B0_DQ1_RG_ARPI_MIDPI_DUMMY_EN_B0)
													| P_Fld(1, SHU_B0_DQ1_RG_ARPI_8PHASE_XLATCH_FORCE_B0));
		vIO32WriteFldMulti_All(DDRPHY_REG_SHU_B1_DQ1, P_Fld(1, SHU_B1_DQ1_RG_ARPI_MIDPI_BYPASS_EN_B1)
													| P_Fld(1, SHU_B1_DQ1_RG_ARPI_MIDPI_DUMMY_EN_B1)
													| P_Fld(1, SHU_B1_DQ1_RG_ARPI_8PHASE_XLATCH_FORCE_B1));

		vIO32WriteFldMulti_All(DDRPHY_REG_SHU_B0_DQ10, P_Fld(1, SHU_B0_DQ10_RG_RX_ARDQS_BW_SEL_B0)
													| P_Fld(1, SHU_B0_DQ10_RG_RX_ARDQS_DQSSTB_RPST_HS_EN_B0)	//Critical ?
													| P_Fld(1, SHU_B0_DQ10_RG_RX_ARDQS_DQSSTB_CG_EN_B0));		//Critical ?
		vIO32WriteFldMulti_All(DDRPHY_REG_SHU_B1_DQ10, P_Fld(1, SHU_B1_DQ10_RG_RX_ARDQS_BW_SEL_B1)
													| P_Fld(1, SHU_B1_DQ10_RG_RX_ARDQS_DQSSTB_RPST_HS_EN_B1)	//Critical ?
													| P_Fld(1, SHU_B1_DQ10_RG_RX_ARDQS_DQSSTB_CG_EN_B1));		//Critical ?

		{
			U8 u1DQ_BW_SEL_B0=0, u1DQ_BW_SEL_B1=0, u1CA_BW_SEL_CA=0, u1CLK_BW_SEL_CA=0;
			if (p->frequency <= 1200)
			{
				u1CLK_BW_SEL_CA = 1;
			}
			if (p->frequency >= 2133)
			{
				u1DQ_BW_SEL_B0 = 1;
				u1DQ_BW_SEL_B1 = 1;
				u1CA_BW_SEL_CA = 1;
			}
			vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B0_DQ11, u1DQ_BW_SEL_B0, SHU_B0_DQ11_RG_RX_ARDQ_BW_SEL_B0);
			vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B1_DQ11, u1DQ_BW_SEL_B1, SHU_B1_DQ11_RG_RX_ARDQ_BW_SEL_B1);
			vIO32WriteFldAlign_All(DDRPHY_REG_SHU_CA_CMD11, u1CA_BW_SEL_CA, SHU_CA_CMD11_RG_RX_ARCA_BW_SEL_CA);
			vIO32WriteFldAlign_All(DDRPHY_REG_SHU_CA_CMD10, u1CLK_BW_SEL_CA, SHU_CA_CMD10_RG_RX_ARCLK_BW_SEL_CA);
		}

		vIO32WriteFldMulti_All(DDRPHY_REG_SHU_CA_CMD1, P_Fld(1, SHU_CA_CMD1_RG_ARPI_MIDPI_BYPASS_EN_CA) | P_Fld(1, SHU_CA_CMD1_RG_ARPI_MIDPI_DUMMY_EN_CA));
		//Darren-vIO32WriteFldAlign_All(DDRPHY_REG_SHU_CA_CMD10, 1, SHU_CA_CMD10_RG_RX_ARCLK_DLY_LAT_EN_CA);
		vIO32WriteFldMulti_All(DDRPHY_REG_SHU_CA_CMD8, P_Fld(1, SHU_CA_CMD8_R_DMRANK_CHG_PIPE_CG_IG_CA)
													| P_Fld(1, SHU_CA_CMD8_R_DMRANK_PIPE_CG_IG_CA)
													| P_Fld(1, SHU_CA_CMD8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_CA)
													| P_Fld(1, SHU_CA_CMD8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_CA)
													| P_Fld(1, SHU_CA_CMD8_R_DMDQSIEN_FLAG_PIPE_CG_IG_CA)
													| P_Fld(1, SHU_CA_CMD8_R_DMDQSIEN_FLAG_SYNC_CG_IG_CA)
													| P_Fld(1, SHU_CA_CMD8_R_RMRX_TOPHY_CG_IG_CA)
													| P_Fld(1, SHU_CA_CMD8_R_RMRODTEN_CG_IG_CA));
		vIO32WriteFldAlign_All(DDRPHY_REG_SHU_CA_CMD12, 0, SHU_CA_CMD12_RG_RIMP_REV);


		vIO32WriteFldMulti_All(DDRPHY_REG_MISC_SHU_IMPEDAMCE_UPD_DIS1, P_Fld(1, MISC_SHU_IMPEDAMCE_UPD_DIS1_CMD1_ODTN_UPD_DIS)
																	| P_Fld(1, MISC_SHU_IMPEDAMCE_UPD_DIS1_CMD1_DRVN_UPD_DIS)
																	| P_Fld(1, MISC_SHU_IMPEDAMCE_UPD_DIS1_CMD1_DRVP_UPD_DIS)
																	| P_Fld(1, MISC_SHU_IMPEDAMCE_UPD_DIS1_CS_ODTN_UPD_DIS)
																	| P_Fld(1, MISC_SHU_IMPEDAMCE_UPD_DIS1_CS_DRVN_UPD_DIS)
																	| P_Fld(1, MISC_SHU_IMPEDAMCE_UPD_DIS1_CS_DRVP_UPD_DIS));

		//Darren-vIO32WriteFldMulti_All(DDRPHY_REG_MISC_SHU_DVFSDLL, P_Fld(67, MISC_SHU_DVFSDLL_R_2ND_DLL_IDLE) | P_Fld(43, MISC_SHU_DVFSDLL_R_DLL_IDLE));

		//Darren-vIO32WriteFldAlign_All(DDRPHY_REG_SHU_MISC_IMPCAL1, 0, SHU_MISC_IMPCAL1_IMPCALCNT);
		//Darren-vIO32WriteFldAlign_All(DDRPHY_REG_SHU_MISC_DRVING2, 0, SHU_MISC_DRVING2_DIS_IMPCAL_ODT_EN);
		//Darren-vIO32WriteFldAlign_All(DDRPHY_REG_SHU_MISC_DRVING6, 7, SHU_MISC_DRVING6_IMP_TXDLY_CMD);

		vIO32WriteFldMulti_All(DDRPHY_REG_MISC_SHU_RX_CG_CTRL, P_Fld(0, MISC_SHU_RX_CG_CTRL_RX_DCM_WAIT_DLE_EXT_DLY)
															| P_Fld(2, MISC_SHU_RX_CG_CTRL_RX_DCM_EXT_DLY)
															| P_Fld(0, MISC_SHU_RX_CG_CTRL_RX_APHY_CTRL_DCM_OPT)
															| P_Fld(0, MISC_SHU_RX_CG_CTRL_RX_DCM_OPT));
		#endif
		DramcBroadcastOnOff(backup_broadcast);
	}
}


void vApplyConfigBeforeCalibration(DRAMC_CTX_T *p)
{
#if __A60868_TO_BE_PORTING__

	U8 read_xrtw2w, shu_index;
	U8 u1RankIdx, u1RankIdxBak;
	u1RankIdxBak = u1GetRank(p);

	//Clk free run {Move to Init_DRAM() and only call once}
#if (SW_CHANGE_FOR_SIMULATION == 0)
	EnableDramcPhyDCM(p, 0);
#endif

	//Set LP3/LP4 Rank0/1 CA/TX delay chain to 0
#if (FOR_DV_SIMULATION_USED == 0)
	//CA0~9 per bit delay line -> CHA_CA0 CHA_CA3 CHA_B0_DQ6 CHA_B0_DQ7 CHA_B0_DQ2 CHA_B0_DQ5 CHA_B0_DQ4 CHA_B0_DQ1 CHA_B0_DQ0 CHA_B0_DQ3
	vResetDelayChainBeforeCalibration(p);
#endif

	//MR4 refresh cnt set to 0x1ff (2ms update)
	vIO32WriteFldAlign_All(DRAMC_REG_SHU_CONF3, 0x1ff, SHU_CONF3_REFRCNT);

	//The counter for Read MR4 cannot be reset after SREF if DRAMC no power down.
	vIO32WriteFldAlign_All(DRAMC_REG_SPCMDCTRL, 1, SPCMDCTRL_SRFMR4_CNTKEEP_B);

	//---- ZQ CS init --------
	vIO32WriteFldAlign_All(DRAMC_REG_SHU_SCINTV, 0x1B, SHU_SCINTV_TZQLAT); //ZQ Calibration Time, unit: 38.46ns, tZQCAL min is 1 us. need to set larger than 0x1b
	//for(shu_index = DRAM_DFS_SHUFFLE_1; shu_index < DRAM_DFS_SHUFFLE_MAX; shu_index++)
		//vIO32WriteFldAlign_All(DRAMC_REG_SHU_CONF3 + SHU_GRP_DRAMC_OFFSET*shu_index, 0x1ff, SHU_CONF3_ZQCSCNT); //Every refresh number to issue ZQCS commands, only for DDR3/LPDDR2/LPDDR3/LPDDR4
	vIO32WriteFldAlign_All(DRAMC_REG_SHU_CONF3, 0x1ff, SHU_CONF3_ZQCSCNT); //Every refresh number to issue ZQCS commands, only for DDR3/LPDDR2/LPDDR3/LPDDR4
	//vIO32WriteFldAlign_All(DRAMC_REG_SHU2_CONF3, 0x1ff, SHU_CONF3_ZQCSCNT); //Every refresh number to issue ZQCS commands, only for DDR3/LPDDR2/LPDDR3/LPDDR4
	//vIO32WriteFldAlign_All(DRAMC_REG_SHU3_CONF3, 0x1ff, SHU_CONF3_ZQCSCNT); //Every refresh number to issue ZQCS commands, only for DDR3/LPDDR2/LPDDR3/LPDDR4
	vIO32WriteFldAlign_All(DRAMC_REG_DRAMCTRL, 0, DRAMCTRL_ZQCALL);  // HW send ZQ command for both rank, disable it due to some dram only have 1 ZQ pin for two rank.

	//Dual channel ZQCS interlace,	0: disable, 1: enable
	if (p->support_channel_num == CHANNEL_SINGLE)
	{
		//single channel, ZQCSDUAL=0, ZQCSMASK=0
		vIO32WriteFldMulti(DRAMC_REG_ZQCS, P_Fld(0, ZQCS_ZQCSDUAL) | P_Fld(0x0, ZQCS_ZQCSMASK));
	}
	else if (p->support_channel_num == CHANNEL_DUAL)
	{
		// HW ZQ command is channel interleaving since 2 channel share the same ZQ pin.
		#ifdef ZQCS_ENABLE_LP4
		// dual channel, ZQCSDUAL =1, and CHA ZQCSMASK=0, CHB ZQCSMASK=1

		vIO32WriteFldMulti_All(DRAMC_REG_ZQCS, P_Fld(1, ZQCS_ZQCSDUAL) | \
											   P_Fld(0, ZQCS_ZQCSMASK_OPT) | \
											   P_Fld(0, ZQCS_ZQMASK_CGAR) | \
											   P_Fld(0, ZQCS_ZQCS_MASK_SEL_CGAR));

		/* DRAMC CHA(CHN0):ZQCSMASK=1, DRAMC CHB(CHN1):ZQCSMASK=0.
		 * ZQCSMASK setting: (Ch A, Ch B) = (1,0) or (0,1)
		 * if CHA.ZQCSMASK=1, and then set CHA.ZQCALDISB=1 first, else set CHB.ZQCALDISB=1 first
		 */
		vIO32WriteFldAlign(DRAMC_REG_ZQCS + (CHANNEL_A << POS_BANK_NUM), 1, ZQCS_ZQCSMASK);
		vIO32WriteFldAlign(DRAMC_REG_ZQCS + SHIFT_TO_CHB_ADDR, 0, ZQCS_ZQCSMASK);

		// DRAMC CHA(CHN0):ZQCS_ZQCS_MASK_SEL=0, DRAMC CHB(CHN1):ZQCS_ZQCS_MASK_SEL=0.
		vIO32WriteFldAlign_All(DRAMC_REG_ZQCS, 0, ZQCS_ZQCS_MASK_SEL);
		#endif
	}

	// Disable LP3 HW ZQ
	vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DRAMC_REG_SPCMDCTRL), 0, SPCMDCTRL_ZQCSDISB);   //LP3 ZQCSDISB=0
	// Disable LP4 HW ZQ
	vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DRAMC_REG_SPCMDCTRL), 0, SPCMDCTRL_ZQCALDISB);  //LP4 ZQCALDISB=0
	// ---- End of ZQ CS init -----

	//Disable write-DBI of DRAMC (Avoids pre-defined data pattern being modified)
	DramcWriteDBIOnOff(p, DBI_OFF);
	//Disable read-DBI of DRAMC (Avoids pre-defined data pattern being modified)
	DramcReadDBIOnOff(p, DBI_OFF);
	//disable MR4 read, REFRDIS=1
	vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DRAMC_REG_SPCMDCTRL), 1, SPCMDCTRL_REFRDIS);
	vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DRAMC_REG_DQSOSCR), 0x1, DQSOSCR_DQSOSCRDIS);  //MR18, MR19 Disable
	//for(shu_index = DRAM_DFS_SHUFFLE_1; shu_index < DRAM_DFS_SHUFFLE_MAX; shu_index++)
		//vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DRAMC_REG_SHU_SCINTV) + SHU_GRP_DRAMC_OFFSET*shu_index, 0x1, SHU_SCINTV_DQSOSCENDIS);
	vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DRAMC_REG_SHU_SCINTV), 0x1, SHU_SCINTV_DQSOSCENDIS);
	//vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DRAMC_REG_SHU2_SCINTV), 0x1, SHU2_SCINTV_DQSOSCENDIS);
	//vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DRAMC_REG_SHU3_SCINTV), 0x1, SHU3_SCINTV_DQSOSCENDIS);
	vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DRAMC_REG_DUMMY_RD), P_Fld(0x0, DUMMY_RD_DUMMY_RD_EN)
											| P_Fld(0x0, DUMMY_RD_SREF_DMYRD_EN)
											| P_Fld(0x0, DUMMY_RD_DQSG_DMYRD_EN)
											| P_Fld(0x0, DUMMY_RD_DMY_RD_DBG));

	// Disable HW gating tracking first, 0x1c0[31], need to disable both UI and PI tracking or the gating delay reg won't be valid.
	DramcHWGatingOnOff(p, 0);

	// Disable gating debug
	vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DRAMC_REG_STBCAL2), 0, STBCAL2_STB_GERRSTOP);

	for (u1RankIdx = RANK_0; u1RankIdx < RANK_MAX; u1RankIdx++)
	{
		vSetRank(p, u1RankIdx);

		// Disable RX delay tracking
		vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_R0_B0_RXDVS2), 0x0, R0_B0_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B0);
		vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_R0_B1_RXDVS2), 0x0, R0_B1_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B1);

		vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_R0_B0_RXDVS2), 0x0, R0_B0_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B0);
		vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_R0_B1_RXDVS2), 0x0, R0_B1_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B1);

		//RX delay mux, delay vlaue from reg.
		vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_R0_B0_RXDVS2), 0x0, R0_B0_RXDVS2_R_RK0_DVS_MODE_B0);
		vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_R0_B1_RXDVS2), 0x0, R0_B1_RXDVS2_R_RK0_DVS_MODE_B1);
		vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_R0_CA_RXDVS2), 0x0, R0_CA_RXDVS2_R_RK0_DVS_MODE_CA);
	}
	vSetRank(p, u1RankIdxBak);

	// Set to all-bank refresh
	vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DRAMC_REG_REFCTRL0), 0, REFCTRL0_PBREFEN);

	// set MRSRK to 0, MPCRKEN always set 1 (Derping)
	vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DRAMC_REG_MRS), 0, MRS_MRSRK);
	vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DRAMC_REG_MPC_OPTION), 1, MPC_OPTION_MPCRKEN);

	//RG mode
	vIO32WriteFldAlign_All(DDRPHY_B0_DQ6, 0x1, B0_DQ6_RG_RX_ARDQ_BIAS_PS_B0);
	vIO32WriteFldAlign_All(DDRPHY_B1_DQ6, 0x1, B1_DQ6_RG_RX_ARDQ_BIAS_PS_B1);
	vIO32WriteFldAlign_All(DDRPHY_CA_CMD6, 0x1, CA_CMD6_RG_RX_ARCMD_BIAS_PS);

#if ENABLE_RX_TRACKING
	DramcRxInputDelayTrackingInit_byFreq(p);
#endif

#ifdef LOOPBACK_TEST
#ifdef LPBK_INTERNAL_EN
	DramcLoopbackTest_settings(p, 0);	//0: internal loopback test 1: external loopback test
#else
	DramcLoopbackTest_settings(p, 1);	//0: internal loopback test 1: external loopback test
#endif
#endif

#if ENABLE_TMRRI_NEW_MODE
	SetCKE2RankIndependent(p);
#endif

#ifdef DUMMY_READ_FOR_TRACKING
	vIO32WriteFldAlign_All(DRAMC_REG_DUMMY_RD, 1, DUMMY_RD_DMY_RD_RX_TRACK);
#endif

	vIO32WriteFldAlign_All(DRAMC_REG_DRSCTRL, 1, DRSCTRL_DRSDIS);

#ifdef IMPEDANCE_TRACKING_ENABLE
	// set correct setting to control IMPCAL HW Tracking in shuffle RG
	// if p->freq >= 1333, enable IMP HW tracking(SHU_DRVING1_DIS_IMPCAL_HW=0), else SHU_DRVING1_DIS_IMPCAL_HW = 1
	U8 u1DisImpHw;
	U32 u4TermFreq;

#if (__LP5_COMBO__ == TRUE)
	if (TRUE == is_lp5_family(p))
		u4TermFreq = LP5_MRFSP_TERM_FREQ;
	else
#endif
		u4TermFreq = LP4_MRFSP_TERM_FREQ;

	u1DisImpHw = (p->frequency >= u4TermFreq)? 0: 1;
	vIO32WriteFldAlign_All(DDRPHY_REG_SHU_MISC_DRVING1, u1DisImpHw, SHU_MISC_DRVING1_DIS_IMPCAL_HW);
	vIO32WriteFldAlign_All(DDRPHY_REG_SHU_MISC_DRVING1, u1DisImpHw, SHU_MISC_DRVING1_DIS_IMP_ODTN_TRACK);
	vIO32WriteFldAlign_All(DDRPHY_REG_SHU_MISC_DRVING2, u1DisImpHw, SHU_MISC_DRVING2_DIS_IMPCAL_ODT_EN);
	vIO32WriteFldAlign_All(DDRPHY_REG_SHU_CA_CMD12, u1DisImpHw, SHU_CA_CMD12_RG_RIMP_UNTERM_EN);
#endif


#if SUPPORT_SAVE_TIME_FOR_CALIBRATION && RX_DELAY_PRE_CAL
	s2RxDelayPreCal = PASS_RANGE_NA; // reset variable for fast k test
#endif
#endif
}

static void DramcInit_DutyCalibration(DRAMC_CTX_T *p)
{
#if ENABLE_DUTY_SCAN_V2
	U32 u4backup_broadcast= GetDramcBroadcast();
#ifdef DDR_INIT_TIME_PROFILING
	U32 u4low_tick0, u4high_tick0, u4low_tick1, u4high_tick1;
#if __ETT__
	u4low_tick0 = GPT_GetTickCount(&u4high_tick0);
#else
	u4low_tick0 = get_timer(0);
#endif
#endif

	DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
#ifndef DUMP_INIT_RG_LOG_TO_DE
	if (Get_MDL_Used_Flag()==NORMAL_USED)
	{
		DramcNewDutyCalibration(p);
	}
#endif
	DramcBroadcastOnOff(u4backup_broadcast);

#ifdef DDR_INIT_TIME_PROFILING
#if __ETT__
	u4low_tick1 = GPT_GetTickCount(&u4high_tick1);
	gu4DutyCalibrationTime = ((u4low_tick1 - u4low_tick0) * 76) / 1000000;
#else
	u4low_tick1 = get_timer(u4low_tick0);
	gu4DutyCalibrationTime = u4low_tick1;
#endif
#endif
#endif
}

static void SV_BroadcastOn_DramcInit(DRAMC_CTX_T *p)
{


	//CInit_ConfigFromTBA();
	DramcBroadcastOnOff(DRAMC_BROADCAST_ON);

	//if(LPDDR4_EN_S && DramcConfig->freq_sel == LP4_DDR1600)
	if(!is_lp5_family(p))
	{
		if(p->frequency>=2133)	//Term
		{
			mcSHOW_DBG_MSG(("sv_algorithm_assistance_LP4_4266 \n"));
			sv_algorithm_assistance_LP4_4266(p);
		}
		else if(p->frequency>=1333)  //Term
		{
			mcSHOW_DBG_MSG(("sv_algorithm_assistance_LP4_3733 \n"));
			sv_algorithm_assistance_LP4_3733(p);
		}
		else if(p->frequency>400) //Unterm
		{
			mcSHOW_DBG_MSG(("sv_algorithm_assistance_LP4_1600 \n"));
			sv_algorithm_assistance_LP4_1600(p);
		}
		else /*if(p->frequency==400)*/	//DDR800 Semi-Open
		{
			//mcSHOW_DBG_MSG(("CInit_golden_mini_freq_related_vseq_LP4_1600 \n"));
			//CInit_golden_mini_freq_related_vseq_LP4_1600(p);
			//CInit_golden_mini_freq_related_vseq_LP4_1600_SHU1(DramcConfig);
			mcSHOW_DBG_MSG(("sv_algorithm_assistance_LP4_800 \n"));
			sv_algorithm_assistance_LP4_800(p);
		}
		/*else //DDR250 Open Loop (DV random seed not ready)
		{
			mcSHOW_DBG_MSG(("sv_algorithm_assistance_LP4_250 \n"));
			sv_algorithm_assistance_LP4_250(p);
		}*/
	}
	#if __LP5_COMBO__
	else
	{
		if(p->freq_sel==LP5_DDR4266)
		{
			mcSHOW_DBG_MSG(("CInit_golden_mini_freq_related_vseq_LP5_4266 \n"));
			CInit_golden_mini_freq_related_vseq_LP5_4266(p);
		}
		else if(p->freq_sel==LP5_DDR5500)
		{
			mcSHOW_DBG_MSG(("CInit_golden_mini_freq_related_vseq_LP5_5500 \n"));
			CInit_golden_mini_freq_related_vseq_LP5_5500(p);
		}
		else
		{
			mcSHOW_DBG_MSG(("CInit_golden_mini_freq_related_vseq_LP5_3200 \n"));
			CInit_golden_mini_freq_related_vseq_LP5_3200(p);
			CInit_golden_mini_freq_related_vseq_LP5_3200_SHU1(p);
		}
	}
	#endif

	RESETB_PULL_DN(p);
	ANA_init(p);
	DIG_STATIC_SETTING(p);
	DIG_CONFIG_SHUF(p,0,0); //temp ch0 group 0

	if(!is_lp5_family(p))
	{
		LP4_UpdateInitialSettings(p);
	}
	else
	{
		LP5_UpdateInitialSettings(p);
	}
	DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
}
DRAM_STATUS_T DramcInit(DRAMC_CTX_T *p)
{
#ifdef FOR_HQA_REPORT_USED
	if (gHQALog_flag==1)
	{
		mcSHOW_DBG_MSG(("[HQA] Log parsing, "));
		mcSHOW_DBG_MSG(("\tDram Data rate = ")); HQA_LOG_Print_Freq_String(p); mcSHOW_DBG_MSG(("\n"));
	}
#endif
	mcSHOW_DBG_MSG(("MEM_TYPE=%d, freq_sel=%d\n", MEM_TYPE, p->freq_sel));
	SV_BroadcastOn_DramcInit(p); // @Darren, Broadcast Off after SV_BroadcastOn_DramcInit done
	#if PRINT_CALIBRATION_SUMMARY
	//default set DRAM status = NO K
	memset(p->aru4CalResultFlag, 0xffff, sizeof(p->aru4CalResultFlag));
	memset(p->aru4CalExecuteFlag, 0, sizeof(p->aru4CalExecuteFlag));
	#if PRINT_CALIBRATION_SUMMARY_FASTK_CHECK
	memset(p->FastKResultFlag, 0xffff, sizeof(p->FastKResultFlag));
	memset(p->FastKExecuteFlag, 0, sizeof(p->FastKExecuteFlag));
	#endif
	#endif

	EnableDramcPhyDCM(p, DCM_OFF); //Let CLK always free-run
	vResetDelayChainBeforeCalibration(p);

	if(!is_lp5_family(p))
		DVFSSettings(p);

	vSetRank(p, RANK_0);
		//LP4_DRAM_INIT(p);
		Dramc8PhaseCal(p);
		DramcInit_DutyCalibration(p);
		DramcModeRegInit_LP4(p);

	DDRPhyFreqMeter();

	DdrUpdateACTiming(p);

	memset(p->isWLevInitShift, FALSE, sizeof(p->isWLevInitShift));

	#if BYPASS_CALIBRATION
	if(p->freq_sel==LP4_DDR4266 || p->freq_sel==LP4_DDR3200)
	{
		Apply_LP4_4266_Calibraton_Result(p);
	}

	else if(p->freq_sel==LP4_DDR1600)
	{
		mcSHOW_DBG_MSG(("BYPASS CALIBRATION LP4 1600 \n"));
		Apply_LP4_1600_Calibraton_Result(p);
	}
	#endif

#if 0//__A60868_TO_BE_PORTING__

	U32 save_ch, dram_t; //Darren
	#if (!__ETT__ && !FOR_DV_SIMULATION_USED && SW_CHANGE_FOR_SIMULATION == 0)
	EMI_SETTINGS *emi_set; //Darren
	#endif
	U8 dram_cbt_mode;

	mcSHOW_DBG_MSG(("\n[DramcInit]\n"));

	vSetPHY2ChannelMapping(p, CHANNEL_A);

	//default set DRAM status = NO K
	memset(p->aru4CalResultFlag, 0xffff, sizeof(p->aru4CalResultFlag));
	memset(p->aru4CalExecuteFlag, 0, sizeof(p->aru4CalExecuteFlag));

	DramcSetting_Olympus_LP4_ByteMode(p);

	DramcInit_DutyCalibration(p);

	DramcModeRegInit_LP4(p);

	//DdrUpdateACTiming(p);

#if 0  //update refresh rate
	// for free-run clk 26MHz, 0x62 * (1/26) = 3.8ns
	vIO32WriteFldAlign_All(DRAMC_REG_DRAMC_PD_CTRL, 0x62, DRAMC_PD_CTRL_REFCNT_FR_CLK);
	// for non-fre-run clk,  reg = 3.8 ns * f / 4 / 16;
	u4RefreshRate = 38 * p->frequency / 640;
	vIO32WriteFldAlign_All(DRAMC_REG_CONF2, u4RefreshRate, CONF2_REFCNT);
#endif

#if (fcFOR_CHIP_ID == fcLafite)
	// For kernel api for check LPDDR3/4/4X (Darren), only for fcOlympus and fcElbrus.
	// For Other chip, please confirm the register is free for SW use.
	save_ch = vGetPHY2ChannelMapping(p);
	vSetPHY2ChannelMapping(p, CHANNEL_A);

	switch (p->dram_type)
	{
		case TYPE_LPDDR4:
			dram_t = 2;
			break;
		case TYPE_LPDDR4X:
			dram_t = 3;
			break;
		case TYPE_LPDDR4P:
			dram_t = 4;
			break;
		default:
			dram_t = 0;
			mcSHOW_ERR_MSG(("Incorrect DRAM Type!\n"));
			break;
	}
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_ARBCTL), dram_t, ARBCTL_RSV_DRAM_TYPE);

	// For DRAM normal, byte and mixed mode
	if ((p->dram_cbt_mode[RANK_0] == CBT_NORMAL_MODE) && (p->dram_cbt_mode[RANK_1] == CBT_NORMAL_MODE))
		dram_cbt_mode = CBT_R0_R1_NORMAL;
	else if ((p->dram_cbt_mode[RANK_0] == CBT_BYTE_MODE1) && (p->dram_cbt_mode[RANK_1] == CBT_BYTE_MODE1))
		dram_cbt_mode = CBT_R0_R1_BYTE;
	else if ((p->dram_cbt_mode[RANK_0] == CBT_NORMAL_MODE) && (p->dram_cbt_mode[RANK_1] == CBT_BYTE_MODE1))
		dram_cbt_mode = CBT_R0_NORMAL_R1_BYTE;
	else if ((p->dram_cbt_mode[RANK_0] == CBT_BYTE_MODE1) && (p->dram_cbt_mode[RANK_1] == CBT_NORMAL_MODE))
		dram_cbt_mode = CBT_R0_BYTE_R1_NORMAL;
	else
		dram_cbt_mode = CBT_R0_R1_NORMAL;

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_RSTMASK), dram_cbt_mode, RSTMASK_RSV_DRAM_CBT_MIXED);

	// Sagy: Keep original setting till OS kernel ready, if ready, remove it
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_ARBCTL), (p->dram_cbt_mode[RANK_0] | p->dram_cbt_mode[RANK_1]), ARBCTL_RSV_DRAM_CBT);

	vSetPHY2ChannelMapping(p, save_ch);
#endif

	mcSHOW_DBG_MSG3(("[DramcInit] Done\n"));
#endif//__A60868_TO_BE_PORTING__
	return DRAM_OK;
}

#if ENABLE_TMRRI_NEW_MODE
void SetCKE2RankIndependent(DRAMC_CTX_T *p)
{
	#if ENABLE_TMRRI_NEW_MODE//Newly added CKE control mode API
	mcSHOW_DBG_MSG(("SET_CKE_2_RANK_INDEPENDENT_RUN_TIME: ON\n"));
	vCKERankCtrl(p, CKE_RANK_INDEPENDENT);
	#else //Legacy individual CKE control register settings
	mcSHOW_DBG_MSG(("SET_CKE_2_RANK_INDEPENDENT_RUN_TIME: OFF\n"));
	vCKERankCtrl(p, CKE_RANK_DEPENDENT);
	#endif
}
#endif


#if ENABLE_WRITE_DBI
void EnableDRAMModeRegWriteDBIAfterCalibration(DRAMC_CTX_T *p)
{
	U8 channel_idx, rank_idx;
	U8 ch_backup, rank_backup, u1FSPIdx = 0;

	ch_backup = p->channel;
	rank_backup = p->rank;

	for (channel_idx = CHANNEL_A; channel_idx < p->support_channel_num; channel_idx++)
	{
		vSetPHY2ChannelMapping(p, channel_idx);
		for (rank_idx = RANK_0; rank_idx < p->support_rank_num; rank_idx++)
		{
			vSetRank(p, rank_idx);
			for (u1FSPIdx = FSP_0; u1FSPIdx < p->support_fsp_num; u1FSPIdx++)
			{
				DramcMRWriteFldAlign(p, 13, u1FSPIdx, MR13_FSP_WR, TO_MR);
				SetDramModeRegForWriteDBIOnOff(p, u1FSPIdx, p->DBI_W_onoff[u1FSPIdx]);
			}
		}
	}

	vSetRank(p, rank_backup);
	vSetPHY2ChannelMapping(p, ch_backup);
}
#endif

#if ENABLE_READ_DBI
void EnableDRAMModeRegReadDBIAfterCalibration(DRAMC_CTX_T *p)
{
	U8 channel_idx, rank_idx;
	U8 ch_backup, rank_backup, u1FSPIdx = 0;
	S8 u1ShuffleIdx;

	ch_backup = p->channel;
	rank_backup = p->rank;

	for (channel_idx = CHANNEL_A; channel_idx < p->support_channel_num; channel_idx++)
	{
		vSetPHY2ChannelMapping(p, channel_idx);
		for (rank_idx = RANK_0; rank_idx < p->support_rank_num; rank_idx++)
		{
			vSetRank(p, rank_idx);
			for (u1FSPIdx = FSP_0; u1FSPIdx < p->support_fsp_num; u1FSPIdx++)
			{
				DramcMRWriteFldAlign(p, 13, u1FSPIdx, MR13_FSP_WR, TO_MR);
				SetDramModeRegForReadDBIOnOff(p, u1FSPIdx, p->DBI_R_onoff[u1FSPIdx]);
			}
		}
	}

	//[Ei_ger] DVT item RD2MRR & MRR2RD
	vIO32WriteFldMulti_All(DRAMC_REG_HW_MRR_FUN, P_Fld(0x1, HW_MRR_FUN_TR2MRR_ENA)
										   | P_Fld(0x1, HW_MRR_FUN_R2MRRHPRICTL)
										   | P_Fld(0x1, HW_MRR_FUN_MANTMRR_EN));

	vSetRank(p, rank_backup);
	vSetPHY2ChannelMapping(p, ch_backup);
}
#endif


static void SetMr13VrcgToNormalOperationShuffle(DRAMC_CTX_T *p)//Only set DRAM_DFS_SHUFFLE_1
{

	U32 u4Value = 0;

		//DVFS MRW
	u4Value = u4IO32ReadFldAlign(DRAMC_REG_SHU_HWSET_VRCG, SHU_HWSET_VRCG_HWSET_VRCG_OP);
	vIO32WriteFldAlign_All(DRAMC_REG_SHU_HWSET_VRCG, u4Value & ~(0x1 << 3), SHU_HWSET_VRCG_HWSET_VRCG_OP);
	return;
}

void SetMr13VrcgToNormalOperation(DRAMC_CTX_T *p)
{

	DRAM_CHANNEL_T eOriChannel = vGetPHY2ChannelMapping(p);
	DRAM_RANK_T eOriRank = u1GetRank(p);
	U8 u1ChIdx = CHANNEL_A;
	U8 u1RankIdx = 0;

#if MRW_CHECK_ONLY
	mcSHOW_MRW_MSG(("\n==[MR Dump] %s==\n", __func__));
#endif


	for (u1ChIdx = CHANNEL_A; u1ChIdx < p->support_channel_num; u1ChIdx++)
	{
		vSetPHY2ChannelMapping(p, u1ChIdx);
		//To DRAM: MR13[3] = 0
		for (u1RankIdx = 0; u1RankIdx < p->support_rank_num; u1RankIdx++)
		{
			vSetRank(p, u1RankIdx);
			DramcMRWriteFldAlign(p, 13, 0, MR13_VRCG, TO_MR);
		}
	}
	vSetPHY2ChannelMapping(p, (U8)eOriChannel);
	vSetRank(p, (U8)eOriRank);
	return;
}

static void DramcShuTrackingDcmEnBySRAM(DRAMC_CTX_T *p)
{
#if (fcFOR_CHIP_ID == fcA60868)
	U8 u1ShuffleIdx, ShuRGAccessIdxBak;
	U32 u4DramcShuOffset = 0;
	U32 u4DDRPhyShuOffset = 0;

	ShuRGAccessIdxBak = p->ShuRGAccessIdx;
	mcSHOW_DBG_MSG(("\n==[DramcShuTrackingDcmEnBySRAM]==\n"));
	for (u1ShuffleIdx = 0; u1ShuffleIdx <= 1; u1ShuffleIdx++) //fill SHU1 of conf while (u1ShuffleIdx==DRAM_DFS_SRAM_MAX)
	{
		//Aceess DMA SRAM by APB bus use debug mode by conf SHU3
		u4DramcShuOffset = 0;
		u4DDRPhyShuOffset = 0;
		p->ShuRGAccessIdx = u1ShuffleIdx;
		#ifdef HW_GATING
		//DramcHWGatingOnOff(p, 1, u4DramcShuOffset); // Enable HW gating tracking
		#endif

		#if ENABLE_TX_TRACKING
		Enable_TX_Tracking(p, u4DramcShuOffset);
		#endif

		#if RDSEL_TRACKING_EN
		Enable_RDSEL_Tracking(p, u4DramcShuOffset);
		#endif

		#ifdef HW_GATING
		Enable_Gating_Tracking(p, u4DDRPhyShuOffset); // Enable HW gating tracking
		#endif
	}
	p->ShuRGAccessIdx = ShuRGAccessIdxBak;
#else
	DRAM_DFS_FREQUENCY_TABLE_T *pFreqTable = p->pDFSTable; // from dramc conf shu0
	U8 u1ShuffleIdx;
	U32 u4DramcShuOffset = 0;
	U32 u4DDRPhyShuOffset = 0;
	U16 u2Freq = 0;

	U32 u4RegBackupAddress[] =
	{
		(DDRPHY_REG_MISC_SRAM_DMA0),
		(DDRPHY_REG_MISC_SRAM_DMA0 + SHIFT_TO_CHB_ADDR),
		(DDRPHY_REG_MISC_SRAM_DMA1),
		(DDRPHY_REG_MISC_SRAM_DMA1 + SHIFT_TO_CHB_ADDR),
#if (CHANNEL_NUM==4)
		(DDRPHY_REG_MISC_SRAM_DMA0 + SHIFT_TO_CHC_ADDR),
		(DDRPHY_REG_MISC_SRAM_DMA0 + SHIFT_TO_CHD_ADDR),
		(DDRPHY_REG_MISC_SRAM_DMA1 + SHIFT_TO_CHC_ADDR),
		(DDRPHY_REG_MISC_SRAM_DMA1 + SHIFT_TO_CHD_ADDR),
#endif
	};

	//Backup regs
	DramcBackupRegisters(p, u4RegBackupAddress, sizeof(u4RegBackupAddress) / sizeof(U32));

	#if (ENABLE_TX_TRACKING && TX_RETRY_ENABLE)
	Enable_and_Trigger_TX_Retry(p);
	#endif

	//Aceess DMA SRAM by APB bus use debug mode by conf SHU3
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SRAM_DMA0, 0x0, MISC_SRAM_DMA0_APB_SLV_SEL);//before setting
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SRAM_DMA1, 0x1, MISC_SRAM_DMA1_R_APB_DMA_DBG_ACCESS);

	for (u1ShuffleIdx = 0; u1ShuffleIdx <= DRAM_DFS_SRAM_MAX; u1ShuffleIdx++) //fill SHU1 of conf while (u1ShuffleIdx==DRAM_DFS_SRAM_MAX)
	{
		if (u1ShuffleIdx == DRAM_DFS_SRAM_MAX)
		{
			//for SHU0 restore to SRAM
			vSetDFSTable(p, pFreqTable);//Restore DFS table
			//Restore regs, or SHU0 RG cannot be set
			DramcRestoreRegisters(p, u4RegBackupAddress, sizeof(u4RegBackupAddress) / sizeof(U32));
			p->ShuRGAccessIdx = DRAM_DFS_REG_SHU0;//Since access conf SHU0
		}
		else
		{
			//Aceess DMA SRAM by APB bus use debug mode by conf SHU1
			vSetDFSTable(p, get_FreqTbl_by_shuffleIndex(p, u1ShuffleIdx));//Update DFS table
			u2Freq = GetFreqBySel(p, p->pDFSTable->freq_sel);
			vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SRAM_DMA0, 0x0, MISC_SRAM_DMA0_APB_SLV_SEL);//before setting
			vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SRAM_DMA1, u1ShuffleIdx, MISC_SRAM_DMA1_R_APB_DMA_DBG_LEVEL);
			//APB bus use debug mode by conf SHU1
			vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SRAM_DMA0, 0x1, MISC_SRAM_DMA0_APB_SLV_SEL);//Trigger DEBUG MODE
			p->ShuRGAccessIdx = DRAM_DFS_REG_SHU1;
		}
		// add your function
		// For example: EnableDramcPhyDCMShuffle(p, enable_dcm, u4DramcShuOffset, u4DDRPhyShuOffset, u1ShuffleIdx);
#if ENABLE_TX_TRACKING
		Enable_TX_Tracking(p, u4DramcShuOffset);
#endif
#if RDSEL_TRACKING_EN
		Enable_RDSEL_Tracking(p, u4DramcShuOffset);
#endif
#ifdef HW_GATING
		Enable_Gating_Tracking(p, u4DDRPhyShuOffset); // Enable HW gating tracking
#endif
#if ENABLE_RX_DCM_DPHY
		EnableRxDcmDPhy(p, u4DDRPhyShuOffset, u2Freq);
#endif
		Enable_ClkTxRxLatchEn(p, u4DDRPhyShuOffset); // for new xrank mode
#if ENABLE_TX_WDQS // @Darren, To avoid unexpected DQS toggle during calibration
		Enable_TxWDQS(p, u4DDRPhyShuOffset, u2Freq);
#endif

#if (SW_CHANGE_FOR_SIMULATION == 0)
#if APPLY_LOWPOWER_GOLDEN_SETTINGS
		int enable_dcm = (doe_get_config("dramc_dcm")) ? 0 : 1;
		EnableDramcPhyDCMShuffle(p, enable_dcm, u4DramcShuOffset, u4DDRPhyShuOffset);
#else
		EnableDramcPhyDCMShuffle(p, 0, u4DramcShuOffset, u4DDRPhyShuOffset);
#endif
#endif
		SetMr13VrcgToNormalOperationShuffle(p);
		p->ShuRGAccessIdx = DRAM_DFS_REG_SHU0;
	}
#endif
}

#if (ENABLE_PER_BANK_REFRESH == 1)
void DramcEnablePerBankRefresh(DRAMC_CTX_T *p, bool en)
{
	if (en)
	{
		vIO32WriteFldMulti_All(DRAMC_REG_REFCTRL0, P_Fld(1, REFCTRL0_PBREF_BK_REFA_ENA) | P_Fld(2, REFCTRL0_PBREF_BK_REFA_NUM));

	#if PER_BANK_REFRESH_USE_MODE==0
			vIO32WriteFldMulti_All(DRAMC_REG_REFCTRL0, P_Fld(0, REFCTRL0_KEEP_PBREF) | P_Fld(0, REFCTRL0_KEEP_PBREF_OPT)); //Original mode
			mcSHOW_DBG_MSG(("\tPER_BANK_REFRESH: Original Mode\n"));
	#endif

	#if PER_BANK_REFRESH_USE_MODE==1
			vIO32WriteFldMulti_All(DRAMC_REG_REFCTRL0, P_Fld(0, REFCTRL0_KEEP_PBREF) | P_Fld(1, REFCTRL0_KEEP_PBREF_OPT)); //Hybrid mode
			mcSHOW_DBG_MSG(("\tPER_BANK_REFRESH: Hybrid Mode\n"));
	#endif

	#if PER_BANK_REFRESH_USE_MODE==2
			vIO32WriteFldMulti_All(DRAMC_REG_REFCTRL0, P_Fld(1, REFCTRL0_KEEP_PBREF) | P_Fld(0, REFCTRL0_KEEP_PBREF_OPT)); //Always per-bank mode
			mcSHOW_DBG_MSG(("\tPER_BANK_REFRESH: Always Per-Bank Mode\n"));
	#endif

		vIO32WriteFldAlign_All(DRAMC_REG_REFCTRL1, 1, REFCTRL1_REFPB2AB_IGZQCS);
	}

	#if IMP_TRACKING_PB_TO_AB_REFRESH_WA
	// disable all shu pb-ref
	vIO32WriteFldAlign_All(DRAMC_REG_SHU_CONF0, 0, SHU_CONF0_PBREFEN);
	#else
	vIO32WriteFldAlign_All(DRAMC_REG_SHU_CONF0, en, SHU_CONF0_PBREFEN);
	#endif
}
#endif

#ifdef TEMP_SENSOR_ENABLE
void DramcHMR4_Presetting(DRAMC_CTX_T *p)
{
	U8 backup_channel = p->channel;
	U8 channelIdx;

	for (channelIdx = CHANNEL_A; channelIdx < p->support_channel_num; channelIdx++)
	{
		vSetPHY2ChannelMapping(p, channelIdx);
	//	  vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_REFCTRL1), Refr_rate_manual_trigger, REFCTRL1_REFRATE_MANUAL_RATE_TRIG);
	//	  vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_REFCTRL1), Refr_rate_manual, REFCTRL1_REFRATE_MANUAL);

		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_HMR4), 1, HMR4_REFR_PERIOD_OPT);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_HMR4), 0, HMR4_REFRCNT_OPT);	// 0: 3.9us * cnt, 1: 15.6us * cnt
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_HMR4_DVFS_CTRL0), 0x80, SHU_HMR4_DVFS_CTRL0_REFRCNT);

		// Support byte mode, default disable
		// Support byte/normal mode
		if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_HMR4), 1, HMR4_HMR4_BYTEMODE_EN);
		else
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_HMR4), 0, HMR4_HMR4_BYTEMODE_EN);

		// Toggle to clear record
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_REFCTRL1), 0, REFCTRL1_REFRATE_MON_CLR);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_REFCTRL1), 1, REFCTRL1_REFRATE_MON_CLR);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_REFCTRL1), 0, REFCTRL1_REFRATE_MON_CLR);
	}
	vSetPHY2ChannelMapping(p, backup_channel);

}
#endif

static void SwitchHMR4(DRAMC_CTX_T *p, bool en)
{
#ifdef __LP5_COMBO__
	if (is_lp5_family(p))
	{
		vIO32WriteFldAlign_All(DRAMC_REG_REF_BOUNCE2, 9, REF_BOUNCE2_PRE_MR4INT_TH);

		vIO32WriteFldAlign_All(DRAMC_REG_REFCTRL2, 9, REFCTRL2_MR4INT_TH);

	}
	else
#endif
	{
		vIO32WriteFldAlign_All(DRAMC_REG_REF_BOUNCE2, 5, REF_BOUNCE2_PRE_MR4INT_TH);

		vIO32WriteFldAlign_All(DRAMC_REG_REFCTRL2, 5, REFCTRL2_MR4INT_TH);
	}

	// TOG_OPT, 0: Read rank0 only, 1: read both rank0 and rank1
	if (en && p->support_rank_num == RANK_DUAL)
		vIO32WriteFldAlign_All(DRAMC_REG_HMR4, 1, HMR4_HMR4_TOG_OPT);	// Read both rank0 and rank1
	else
		vIO32WriteFldAlign_All(DRAMC_REG_HMR4, 0, HMR4_HMR4_TOG_OPT);	// Read rank0 only (need for manual/SW MRR)

	vIO32WriteFldAlign_All(DRAMC_REG_HMR4, !en, HMR4_REFRDIS);

#if 0	// Reading HMR4 repeatedly for debugging
	while(1)
	{
		mcSHOW_DBG_MSG(("@@ --------------------\n"));
		mcSHOW_DBG_MSG(("@@ MISC_STATUSA_REFRESH_RATE: %d\n",
			u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_MISC_STATUSA), MISC_STATUSA_REFRESH_RATE)));
		mcSHOW_DBG_MSG(("@@ MIN: %d, MAX: %d\n",
			u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_HW_REFRATE_MON), HW_REFRATE_MON_REFRESH_RATE_MIN_MON),
			u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_HW_REFRATE_MON), HW_REFRATE_MON_REFRESH_RATE_MAX_MON)));

		// if HMR4_HMR4_TOG_OPT == 1
		{
			mcSHOW_DBG_MSG(("@@ 		MIN    MAX\n"));
			mcSHOW_DBG_MSG(("@@ RK0_B0:  %d 	%d\n",
				u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_HW_REFRATE_MON3), HW_REFRATE_MON3_REFRESH_RATE_MIN_MON_RK0_B0),
				u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_HW_REFRATE_MON3), HW_REFRATE_MON3_REFRESH_RATE_MAX_MON_RK0_B0)));
			mcSHOW_DBG_MSG(("@@ RK1_B0:  %d 	%d\n",
				u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_HW_REFRATE_MON3), HW_REFRATE_MON3_REFRESH_RATE_MIN_MON_RK1_B0),
				u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_HW_REFRATE_MON3), HW_REFRATE_MON3_REFRESH_RATE_MAX_MON_RK1_B0)));
			mcSHOW_DBG_MSG(("@@ RK0_B1:  %d 	%d\n",
				u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_HW_REFRATE_MON2), HW_REFRATE_MON2_REFRESH_RATE_MIN_MON_RK0_B1),
				u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_HW_REFRATE_MON2), HW_REFRATE_MON2_REFRESH_RATE_MAX_MON_RK0_B1)));
			mcSHOW_DBG_MSG(("@@ RK1_B1:  %d 	%d\n",
				u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_HW_REFRATE_MON2), HW_REFRATE_MON2_REFRESH_RATE_MIN_MON_RK1_B1),
				u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_HW_REFRATE_MON2), HW_REFRATE_MON2_REFRESH_RATE_MAX_MON_RK1_B1)));
		}

		mcSHOW_DBG_MSG(("@@ Wait to measure!!\n\n"));
		Sleep(500);
	}
#endif
}

#if ENABLE_REFRESH_RATE_DEBOUNCE
static void DramcRefreshRateDeBounceEnable(DRAMC_CTX_T *p)
{
	vIO32WriteFldMulti_All(DRAMC_REG_REF_BOUNCE1, P_Fld(0x4 , REF_BOUNCE1_REFRATE_DEBOUNCE_COUNT) | // when De-bounce counter >= this count, then dramc apply new dram's MR4 value
												  P_Fld(5 , REF_BOUNCE1_REFRATE_DEBOUNCE_TH) |	// MR4 value >= 0.5X refresh rate, then de-bounce count active
												  P_Fld(0 , REF_BOUNCE1_REFRATE_DEBOUNCE_OPT) |
												  P_Fld(0xff1f , REF_BOUNCE1_REFRATE_DEBOUNCE_DIS) ); //all bits set 1 to disable debounce function
}
#endif

#if DRAMC_MODIFIED_REFRESH_MODE
void DramcModifiedRefreshMode(DRAMC_CTX_T *p)
{
	vIO32WriteFldMulti_All(DRAMC_REG_REFPEND1, P_Fld(2, REFPEND1_MPENDREFCNT_TH0)
											| P_Fld(2, REFPEND1_MPENDREFCNT_TH1)
											| P_Fld(4, REFPEND1_MPENDREFCNT_TH2)
											| P_Fld(5, REFPEND1_MPENDREFCNT_TH3)
											| P_Fld(5, REFPEND1_MPENDREFCNT_TH4)
											| P_Fld(3, REFPEND1_MPENDREFCNT_TH5)
											| P_Fld(3, REFPEND1_MPENDREFCNT_TH6)
											| P_Fld(3, REFPEND1_MPENDREFCNT_TH7));
	vIO32WriteFldMulti_All(DRAMC_REG_REFCTRL1, P_Fld(1, REFCTRL1_REFPEND_OPT1) | P_Fld(1, REFCTRL1_REFPEND_OPT2));
	vIO32WriteFldAlign_All(DRAMC_REG_SHU_REF0, 4, SHU_REF0_MPENDREF_CNT);
}
#endif

#if DRAMC_CKE_DEBOUNCE
void DramcCKEDebounce(DRAMC_CTX_T *p)
{
	U8 u1CKE_DBECnt = 15;
	U8 rank_backup, u1RKIdx=0;
	if(p->frequency>=1866)
	{
		rank_backup = p->rank;
		for(u1RKIdx=0; u1RKIdx<p->support_rank_num; u1RKIdx++)
		{
			vSetRank(p, u1RKIdx);
			vIO32WriteFldAlign_All(DRAMC_REG_SHURK_CKE_CTRL, u1CKE_DBECnt, SHURK_CKE_CTRL_CKE_DBE_CNT);
			mcSHOW_DBG_MSG(("CKE Debounce cnt = %d\n", u1CKE_DBECnt));
		}
		vSetRank(p, rank_backup);
	}
}
#endif

//1.Some RG setting will need to be DCM on, since not consider S0 2.ENABLE_RX_DCM_DPHY should be 1
static void S0_DCMOffWA(DRAMC_CTX_T *p)//For S0 + DCM off
{
	vIO32WriteFldMulti_All(DDRPHY_REG_MISC_CG_CTRL0,
			P_Fld(0x0, MISC_CG_CTRL0_RG_CG_RX_COMB1_OFF_DISABLE) |
			P_Fld(0x0, MISC_CG_CTRL0_RG_CG_RX_COMB0_OFF_DISABLE) |
			P_Fld(0x0, MISC_CG_CTRL0_RG_CG_RX_CMD_OFF_DISABLE) |
			P_Fld(0x0, MISC_CG_CTRL0_RG_CG_COMB1_OFF_DISABLE) |
			P_Fld(0x0, MISC_CG_CTRL0_RG_CG_COMB0_OFF_DISABLE) |
			P_Fld(0x0, MISC_CG_CTRL0_RG_CG_CMD_OFF_DISABLE) |
			P_Fld(0x0, MISC_CG_CTRL0_RG_CG_COMB_OFF_DISABLE) |
			P_Fld(0x0, MISC_CG_CTRL0_RG_CG_PHY_OFF_DIABLE) |
			P_Fld(0x0, MISC_CG_CTRL0_RG_CG_DRAMC_OFF_DISABLE));
}

void DramcRunTimeConfig(DRAMC_CTX_T *p)
{
#if (fcFOR_CHIP_ID == fcA60868)
	u1EnterRuntime = 1;
#endif

	mcSHOW_DBG_MSG(("[DramcRunTimeConfig]\n"));

	SetDramInfoToConf(p);

#if defined(DPM_CONTROL_AFTERK) && ((DRAMC_DFS_MODE%2) != 0) // for MD32 RG/PST mode
	DPMInit(p);
	mcSHOW_DBG_MSG(("DPM_CONTROL_AFTERK: ON\n"));
#else
	mcSHOW_DBG_MSG(("!!! DPM_CONTROL_AFTERK: OFF\n"));
	mcSHOW_DBG_MSG(("!!! DPM could not control APHY\n"));
#endif

#if ENABLE_PER_BANK_REFRESH
	#if IMP_TRACKING_PB_TO_AB_REFRESH_WA
	// enable pb-ref for current shu
	vIO32WriteFldAlign_All(DRAMC_REG_SHU_CONF0, 0x1, SHU_CONF0_PBREFEN);
	#endif
	mcSHOW_DBG_MSG(("PER_BANK_REFRESH: ON\n"));
#else
	mcSHOW_DBG_MSG(("PER_BANK_REFRESH: OFF\n"));
#endif

///TODO:KIWI
#if __A60868_TO_BE_PORTING__

#if ENABLE_DDR800_OPEN_LOOP_MODE_OPTION
	if (vGet_DDR800_Mode(p) == DDR800_SEMI_LOOP)
	{
		EnableDllCg(p, ENABLE); //open CG to save power
	}
#endif

#endif //__A60868_TO_BE_PORTING__

#if REFRESH_OVERHEAD_REDUCTION
	vIO32WriteFldAlign_All(DRAMC_REG_REFCTRL1, 0x1, REFCTRL1_REF_OVERHEAD_SLOW_REFPB_ENA);
	mcSHOW_DBG_MSG(("REFRESH_OVERHEAD_REDUCTION: ON\n"));
#else
	mcSHOW_DBG_MSG(("REFRESH_OVERHEAD_REDUCTION: OFF\n"));
#endif

#if CMD_PICG_NEW_MODE
	mcSHOW_DBG_MSG(("CMD_PICG_NEW_MODE: ON\n"));
#else
	mcSHOW_DBG_MSG(("CMD_PICG_NEW_MODE: OFF\n"));
#endif

#if XRTWTW_NEW_CROSS_RK_MODE
	if (p->support_rank_num == RANK_DUAL)
	{
		//ENABLE_XRTWTW_Setting(p); // @Darren, DV codes is included
		mcSHOW_DBG_MSG(("XRTWTW_NEW_MODE: ON\n"));
	}
#else
	mcSHOW_DBG_MSG(("XRTWTW_NEW_MODE: OFF\n"));
#endif

#if XRTRTR_NEW_CROSS_RK_MODE
	if (p->support_rank_num == RANK_DUAL)
	{
		//ENABLE_XRTRTR_Setting(p); // @Darren, DV codes is included
		mcSHOW_DBG_MSG(("XRTRTR_NEW_MODE: ON\n"));
	}
#else
	mcSHOW_DBG_MSG(("XRTRTR_NEW_MODE: OFF\n"));
#endif

#if ENABLE_TX_TRACKING
	mcSHOW_DBG_MSG(("TX_TRACKING: ON\n"));
#else
	mcSHOW_DBG_MSG(("TX_TRACKING: OFF\n"));
#endif

#if RDSEL_TRACKING_EN
	mcSHOW_DBG_MSG(("RDSEL_TRACKING: ON\n"));
#else
	mcSHOW_DBG_MSG(("RDSEL_TRACKING: OFF\n"));
#endif

#if TDQSCK_PRECALCULATION_FOR_DVFS
	mcSHOW_DBG_MSG(("DQS Precalculation for DVFS: "));
	/* Maoauo: Enable DQS precalculation for LP4, disable for LP3(same as Kibo) */
	DramcDQSPrecalculation_enable(p);
	mcSHOW_DBG_MSG(("ON\n"));
#else
	mcSHOW_DBG_MSG(("DQS Precalculation for DVFS: OFF\n"));
#endif

#if ENABLE_RX_TRACKING
	DramcRxInputDelayTrackingInit_Common(p);
	DramcRxInputDelayTrackingHW(p);
	mcSHOW_DBG_MSG(("RX_TRACKING: ON\n"));
#else
	mcSHOW_DBG_MSG(("RX_TRACKING: OFF\n"));
#endif

#if (ENABLE_RX_TRACKING && RX_DLY_TRACK_ONLY_FOR_DEBUG && defined(DUMMY_READ_FOR_TRACKING))
	mcSHOW_DBG_MSG(("RX_DLY_TRACK_DBG: ON\n"));
	DramcRxDlyTrackDebug(p);
#endif

/* HW gating - Disabled by default(in preloader) to save power (DE: HJ Huang) */
#if (defined(HW_GATING))
	mcSHOW_DBG_MSG(("HW_GATING DBG: ON\n"));
	DramcHWGatingDebugOnOff(p, ENABLE);
#else
	mcSHOW_DBG_MSG(("HW_GATING DBG: OFF\n"));
	DramcHWGatingDebugOnOff(p, DISABLE);
#endif

#ifdef ZQCS_ENABLE_LP4
	// if CHA.ZQCSMASK=1, and then set CHA.ZQCALDISB=1 first, else set CHB.ZQCALDISB=1 first
#if (fcFOR_CHIP_ID == fcPetrus)
	vIO32WriteFldAlign(DRAMC_REG_ZQ_SET1 + (CHANNEL_A << POS_BANK_NUM), 1, ZQ_SET1_ZQCALDISB);
	vIO32WriteFldAlign(DRAMC_REG_ZQ_SET1 + (CHANNEL_D << POS_BANK_NUM), 1, ZQ_SET1_ZQCALDISB);

	mcDELAY_US(1);

	vIO32WriteFldAlign(DRAMC_REG_ZQ_SET1 + (CHANNEL_B << POS_BANK_NUM), 1, ZQ_SET1_ZQCALDISB);
	vIO32WriteFldAlign(DRAMC_REG_ZQ_SET1 + (CHANNEL_C << POS_BANK_NUM), 1, ZQ_SET1_ZQCALDISB);
#elif (fcFOR_CHIP_ID == fcMargaux)
	vIO32WriteFldAlign_All(DRAMC_REG_ZQ_SET1, 1, ZQ_SET1_ZQCALDISB);// LP3 and LP4 are different, be careful.
#endif
	mcSHOW_DBG_MSG(("ZQCS_ENABLE_LP4: ON\n"));
#else
	vIO32WriteFldAlign_All(DRAMC_REG_ZQ_SET1, 0, ZQ_SET1_ZQCALDISB);// LP3 and LP4 are different, be careful.
	mcSHOW_DBG_MSG(("ZQCS_ENABLE_LP4: OFF\n"));
#endif

///TODO:JEREMY
#if 0
#ifdef DUMMY_READ_FOR_DQS_GATING_RETRY
	DummyReadForDqsGatingRetryNonShuffle(p, 1);
	mcSHOW_DBG_MSG(("DUMMY_READ_FOR_DQS_GATING_RETRY: ON\n"));
#else
	DummyReadForDqsGatingRetryNonShuffle(p, 0);
	mcSHOW_DBG_MSG(("DUMMY_READ_FOR_DQS_GATING_RETRY: OFF\n"));
#endif
#endif

#if RX_PICG_NEW_MODE
	mcSHOW_DBG_MSG(("RX_PICG_NEW_MODE: ON\n"));
#else
	mcSHOW_DBG_MSG(("RX_PICG_NEW_MODE: OFF\n"));
#endif

#if TX_PICG_NEW_MODE
	TXPICGNewModeEnable(p);
	mcSHOW_DBG_MSG(("TX_PICG_NEW_MODE: ON\n"));
#else
	mcSHOW_DBG_MSG(("TX_PICG_NEW_MODE: OFF\n"));
#endif

#if ENABLE_RX_DCM_DPHY
	mcSHOW_DBG_MSG(("ENABLE_RX_DCM_DPHY: ON\n"));
#else
	mcSHOW_DBG_MSG(("ENABLE_RX_DCM_DPHY: OFF\n"));
#endif

#if (SW_CHANGE_FOR_SIMULATION == 0)
#if APPLY_LOWPOWER_GOLDEN_SETTINGS
	int enable_dcm = (doe_get_config("dramc_dcm"))? 0: 1;
	const char *str = (enable_dcm == 1)? ("ON") : ("OFF");
//	  EnableDramcPhyDCM(p, enable_dcm);
	EnableDramcPhyDCMNonShuffle(p, enable_dcm);
	mcSHOW_DBG_MSG(("LOWPOWER_GOLDEN_SETTINGS(DCM): %s\n", str));

	if(enable_dcm == 0)
	{
		S0_DCMOffWA(p);//For S0 + DCM off
	}

#else
//	  EnableDramcPhyDCM(p, DCM_OFF);
	EnableDramcPhyDCMNonShuffle(p, 0);
	mcSHOW_DBG_MSG(("LOWPOWER_GOLDEN_SETTINGS(DCM): OFF\n"));

	S0_DCMOffWA(p);//For S0 + DCM off
#endif
#endif

//DumpShuRG(p);



#if 1
	DramcShuTrackingDcmEnBySRAM(p);
#endif


//Dummy read should NOT be enabled before gating tracking
#ifdef DUMMY_READ_FOR_TRACKING
	DramcDummyReadForTrackingEnable(p);
#else
	mcSHOW_DBG_MSG(("DUMMY_READ_FOR_TRACKING: OFF\n"));
#endif


#ifdef SPM_CONTROL_AFTERK
	DVFS_PRE_config(p);
	TransferToSPMControl(p);
	mcSHOW_DBG_MSG(("SPM_CONTROL_AFTERK: ON\n"));
#else
	mcSHOW_DBG_MSG(("!!! SPM_CONTROL_AFTERK: OFF\n"));
	mcSHOW_DBG_MSG(("!!! SPM could not control APHY\n"));
#endif

// when time profiling multi times, SW impedance tracking will fail when trakcing enable.
// ignor SW impedance tracking when doing time profling
#ifndef DDR_INIT_TIME_PROFILING
#ifdef IMPEDANCE_TRACKING_ENABLE
	if (p->dram_type == TYPE_LPDDR4 || p->dram_type == TYPE_LPDDR4X)
	{
		DramcImpedanceTrackingEnable(p);
		mcSHOW_DBG_MSG(("IMPEDANCE_TRACKING: ON\n"));

#ifdef IMPEDANCE_HW_SAVING
		DramcImpedanceHWSaving(p);
#endif
	}
#else
	mcSHOW_DBG_MSG(("IMPEDANCE_TRACKING: OFF\n"));
#endif
#endif

	//0x1c0[31]
	//vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DQSCAL0), 0, DQSCAL0_STBCALEN);

#ifdef TEMP_SENSOR_ENABLE
	SwitchHMR4(p, ON);
	mcSHOW_DBG_MSG(("TEMP_SENSOR: ON\n"));
#else
	SwitchHMR4(p, OFF);
	mcSHOW_DBG_MSG(("TEMP_SENSOR: OFF\n"));
#endif

#ifdef HW_SAVE_FOR_SR
	mcSHOW_DBG_MSG(("HW_SAVE_FOR_SR: ON, no implementation\n"));
#else
	mcSHOW_DBG_MSG(("HW_SAVE_FOR_SR: OFF\n"));
#endif

#ifdef CLK_FREE_FUN_FOR_DRAMC_PSEL
	ClkFreeRunForDramcPsel(p);
	mcSHOW_DBG_MSG(("CLK_FREE_FUN_FOR_DRAMC_PSEL: ON\n"));
#else
	mcSHOW_DBG_MSG(("CLK_FREE_FUN_FOR_DRAMC_PSEL: OFF\n"));
#endif

#if PA_IMPROVEMENT_FOR_DRAMC_ACTIVE_POWER
	DramcPAImprove(p);
	mcSHOW_DBG_MSG(("PA_IMPROVEMENT_FOR_DRAMC_ACTIVE_POWER: ON\n"));
#else
	mcSHOW_DBG_MSG(("PA_IMPROVEMENT_FOR_DRAMC_ACTIVE_POWER: OFF\n"));
#endif

#if ENABLE_RODT_TRACKING
	mcSHOW_DBG_MSG(("Read ODT Tracking: ON\n"));
#else
	mcSHOW_DBG_MSG(("Read ODT Tracking: OFF\n"));
#endif

#if ENABLE_REFRESH_RATE_DEBOUNCE
	mcSHOW_DBG_MSG(("Refresh Rate DeBounce: ON\n"));
	DramcRefreshRateDeBounceEnable(p);
#endif

#if ENABLE_DVFS_BYPASS_MR13_FSP
	DFSBypassMR13HwSet(p);
#endif


#if (CHECK_GOLDEN_SETTING == TRUE)
	DRAM_STATUS_T stResult = CheckGoldenSetting(p);
	mcSHOW_DBG_MSG(("End of run time ==>Golden setting check: %s\n", (stResult == DRAM_OK)? ("OK") : ("NG")));
#endif

#if DFS_NOQUEUE_FLUSH_WA
	EnableDFSNoQueueFlush(p);
	mcSHOW_DBG_MSG(("DFS_NO_QUEUE_FLUSH: ON\n"));
#else
	mcSHOW_DBG_MSG(("DFS_NO_QUEUE_FLUSH: OFF\n"));
#endif

#if DFS_NOQUEUE_FLUSH_LATENCY_CNT
	vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_FSM_CFG, 1, LPIF_FSM_CFG_DBG_LATENCY_CNT_EN);
	// MD32 clock is 208M
	vIO32WriteFldMulti_All(DDRPHY_MD32_REG_SSPM_MCLK_DIV, P_Fld(0, SSPM_MCLK_DIV_MCLK_SRC)
		| P_Fld(0, SSPM_MCLK_DIV_MCLK_DIV));
	mcSHOW_DBG_MSG(("DFS_NO_QUEUE_FLUSH_LATENCY_CNT: ON\n"));
#else
	mcSHOW_DBG_MSG(("DFS_NO_QUEUE_FLUSH_LATENCY_CNT: OFF\n"));
#endif

#if ENABLE_DFS_RUNTIME_MRW
	DFSRuntimeFspMRW(p);
	mcSHOW_DBG_MSG(("ENABLE_DFS_RUNTIME_MRW: ON\n"));
#else
	mcSHOW_DBG_MSG(("ENABLE_DFS_RUNTIME_MRW: OFF\n"));
#endif

	//CheckRxPICGNewModeSetting(p);
	vIO32WriteFldAlign_All(DRAMC_REG_REFCTRL0, 0x0, REFCTRL0_REFDIS); //After k, auto refresh should be enable

#if DDR_RESERVE_NEW_MODE
	mcSHOW_DBG_MSG(("DDR_RESERVE_NEW_MODE: ON\n"));
	vIO32WriteFldMulti_All(DDRPHY_REG_MISC_DDR_RESERVE, P_Fld(1, MISC_DDR_RESERVE_WDT_LITE_EN) | P_Fld(0, MISC_DDR_RESERVE_WDT_SM_CLR));
#else
	mcSHOW_DBG_MSG(("DDR_RESERVE_NEW_MODE: OFF\n"));
	vIO32WriteFldMulti_All(DDRPHY_REG_MISC_DDR_RESERVE, P_Fld(0, MISC_DDR_RESERVE_WDT_LITE_EN) | P_Fld(1, MISC_DDR_RESERVE_WDT_SM_CLR));
#endif

#if MR_CBT_SWITCH_FREQ
	mcSHOW_DBG_MSG(("MR_CBT_SWITCH_FREQ: ON\n"));
#else
	mcSHOW_DBG_MSG(("MR_CBT_SWITCH_FREQ: OFF\n"));
#endif

	mcSHOW_DBG_MSG(("=========================\n"));
}

#if 0  //no use?
void DramcTest_DualSch_stress(DRAMC_CTX_T *p)
{
	U32 count = 0;
	U16 u2Value = 0;

#if MRW_CHECK_ONLY
	mcSHOW_MRW_MSG(("\n==[MR Dump] %s==\n", __func__));
#endif

	//vIO32WriteFldAlign_All(DRAMC_REG_PERFCTL0, 1, PERFCTL0_DUALSCHEN);
	vIO32WriteFldAlign_All(DRAMC_REG_SHU_SCHEDULER, 1, SHU_SCHEDULER_DUALSCHEN);

	while (count < 10)
	{
		count++;

		u1MR12Value[p->channel][p->rank][p->dram_fsp] = 0x14;
		DramcModeRegWriteByRank(p, p->rank, 12, u1MR12Value[p->channel][p->rank][p->dram_fsp]);
		DramcModeRegReadByRank(p, p->rank, 12, &u2Value);
		//mcSHOW_DBG_MSG(("MR12 = 0x%0X\n", u1Value));
	}
}
#endif

#if (ENABLE_TX_TRACKING && TX_RETRY_ENABLE)
void SPMTx_Track_Retry_OnOff(DRAMC_CTX_T *p, U8 shu_level, U8 onoff)
{
	static U8 gIsddr800TxRetry = 0;

	// MCK still available for DRAMC RG access from Joe comment
	if (shu_level == SRAM_SHU6)
	{
		gIsddr800TxRetry = 1;
	}

	if ((gIsddr800TxRetry == 1) && (shu_level != SRAM_SHU6)) //Need to do tx retry when DDR800 -> DDr1200
	{
		if (onoff == ENABLE)
		{
			mcSHOW_DBG_MSG(("TX track retry: ENABLE! (DDR800 to DDR1200)\n"));
			vIO32WriteFldAlign_All(DRAMC_REG_TX_RETRY_SET0, 1, TX_RETRY_SET0_XSR_TX_RETRY_BLOCK_ALE_MASK);
			mcDELAY_US(1);
			#if TX_RETRY_CONTROL_BY_SPM
			vIO32WriteFldAlign(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_1, 1, LPIF_LOW_POWER_CFG_1_TX_TRACKING_RETRY_EN);
			#else //control by DRAMC
			vIO32WriteFldAlign_All(DRAMC_REG_TX_RETRY_SET0, 1, TX_RETRY_SET0_XSR_TX_RETRY_EN);
			#endif
		}
		else //DISABLE
		{
			mcSHOW_DBG_MSG(("TX track retry: DISABLE! (DDR800 to DDR1200)\n"));
			#if TX_RETRY_CONTROL_BY_SPM
			vIO32WriteFldAlign(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_1, 0, LPIF_LOW_POWER_CFG_1_TX_TRACKING_RETRY_EN);
			#else //control by DRAMC
			vIO32WriteFldAlign_All(DRAMC_REG_TX_RETRY_SET0, 0, TX_RETRY_SET0_XSR_TX_RETRY_EN);
			#endif
			mcDELAY_US(1); //add 1us delay to wait emi and tx retry be done (because PPR_CTRL_TX_RETRY_SHU_RESP_OPT=1)
			vIO32WriteFldAlign_All(DRAMC_REG_TX_RETRY_SET0, 0, TX_RETRY_SET0_XSR_TX_RETRY_BLOCK_ALE_MASK); //enable block emi to let tx retry be finish
			gIsddr800TxRetry = 0;
		}
	}
}

#if SW_TX_RETRY_ENABLE
void SWTx_Track_Retry_OnOff(DRAMC_CTX_T *p)
{
	U8 u4Response;

	mcSHOW_DBG_MSG(("SW TX track retry!\n"));
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TX_RETRY_SET0), 0, TX_RETRY_SET0_XSR_TX_RETRY_SW_EN);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TX_RETRY_SET0), 1, TX_RETRY_SET0_XSR_TX_RETRY_BLOCK_ALE_MASK);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TX_RETRY_SET0), 1, TX_RETRY_SET0_XSR_TX_RETRY_SW_EN);
	do
	{
		u4Response = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SPCMDRESP), SPCMDRESP_TX_RETRY_DONE_RESPONSE);
		mcDELAY_US(1);	// Wait tZQCAL(min) 1us or wait next polling
		mcSHOW_DBG_MSG3(("still wait tx retry be done\n", u4Response));
	}while (u4Response == 0);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TX_RETRY_SET0), 0, TX_RETRY_SET0_XSR_TX_RETRY_SW_EN);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TX_RETRY_SET0), 0, TX_RETRY_SET0_XSR_TX_RETRY_BLOCK_ALE_MASK);
}
#endif
#endif

// The "ENABLE_RANK_NUMBER_AUTO_DETECTION" use this API
void DFSInitForCalibration(DRAMC_CTX_T *p)
{
#ifdef DDR_INIT_TIME_PROFILING
	U32 CPU_Cycle;
	mcSHOW_TIME_MSG(("*** Data rate %d ***\n\n", p->frequency << 1));

	TimeProfileBegin();
#endif

	u1PrintModeRegWrite = 1;

#if MRW_BACKUP
	U8 u1RKIdx;

	for(u1RKIdx=0; u1RKIdx<p->support_rank_num; u1RKIdx++)
	{
		gFSPWR_Flag[u1RKIdx]=p->dram_fsp;
	}
#endif

	DramcInit(p);
	u1PrintModeRegWrite = 0;
	vBeforeCalibration(p);

#ifdef DUMP_INIT_RG_LOG_TO_DE
	while (1);
#endif


#if ENABLE_DUTY_SCAN_V2
#ifdef DDR_INIT_TIME_PROFILING
	CPU_Cycle = TimeProfileEnd();
	mcSHOW_TIME_MSG(("	(1) DFSInitForCalibration() take %d ms\n\n", (CPU_Cycle / 1000) - gu4DutyCalibrationTime));
	mcSHOW_TIME_MSG(("	(2) DramcNewDutyCalibration take %d ms\n\r", gu4DutyCalibrationTime));
#endif
#endif

#ifndef DUMP_INIT_RG_LOG_TO_DE
	#ifdef ENABLE_MIOCK_JMETER
	if ((Get_MDL_Used_Flag()==NORMAL_USED) && (p->frequency >= 800))
	{
		Get_RX_DelayCell(p);
	}
	#endif
#endif

#if !__ETT__
	if (p->frequency >= 1333)
#endif
	{
#ifdef DDR_INIT_TIME_PROFILING
		TimeProfileBegin();
#endif

#ifndef DUMP_INIT_RG_LOG_TO_DE
	#ifdef ENABLE_MIOCK_JMETER
	if (Get_MDL_Used_Flag()==NORMAL_USED)
	{
		PRE_MIOCK_JMETER_HQA_USED(p);
	}
	#endif
#endif

#ifdef DDR_INIT_TIME_PROFILING
	  CPU_Cycle=TimeProfileEnd();
	  mcSHOW_TIME_MSG(("  (3) JMeter takes %d ms\n\r", CPU_Cycle / 1000));
#endif
	}

}

#if 0 /* cc mark to use DV initial setting */
void DramcHWDQSGatingTracking_ModeSetting(DRAMC_CTX_T *p)
{
#ifdef HW_GATING
#if DramcHWDQSGatingTracking_FIFO_MODE
	//REFUICHG=0, STB_SHIFT_DTCOUT_IG=0, DQSG_MODE=1, NARROW_IG=0
	vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL),
		P_Fld(1, MISC_STBCAL_STB_DQIEN_IG) |
		P_Fld(1, MISC_STBCAL_PICHGBLOCK_NORD) |
		P_Fld(0, MISC_STBCAL_REFUICHG) |
		P_Fld(0, MISC_STBCAL_PHYVALID_IG) |
		P_Fld(0, MISC_STBCAL_STBSTATE_OPT) |
		P_Fld(0, MISC_STBCAL_STBDLELAST_FILTER) |
		P_Fld(0, MISC_STBCAL_STBDLELAST_PULSE) |
		P_Fld(0, MISC_STBCAL_STBDLELAST_OPT) |
		P_Fld(1, MISC_STBCAL_PIMASK_RKCHG_OPT));

	vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL1),
		P_Fld(1, MISC_STBCAL1_STBCAL_FILTER) |
		//cc mark P_Fld(1, MISC_STBCAL1_STB_FLAGCLR) |
		P_Fld(1, MISC_STBCAL1_STB_SHIFT_DTCOUT_IG));

	vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CTRL0),
		P_Fld(1, MISC_CTRL0_R_DMDQSIEN_FIFO_EN) |
		P_Fld(0, MISC_CTRL0_R_DMVALID_DLY) |
		P_Fld(0, MISC_CTRL0_R_DMVALID_DLY_OPT) |
		P_Fld(0, MISC_CTRL0_R_DMVALID_NARROW_IG));
		//cc mark P_Fld(0, MISC_CTRL0_R_DMDQSIEN_SYNCOPT));

	vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ6),
		0, B0_DQ6_RG_RX_ARDQ_DMRANK_OUTSEL_B0);
	vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ6),
		0, B1_DQ6_RG_RX_ARDQ_DMRANK_OUTSEL_B1);
	vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_REG_CA_CMD6),
		0, CA_CMD6_RG_RX_ARCMD_DMRANK_OUTSEL);

#else
	//REFUICHG=0, STB_SHIFT_DTCOUT_IG=0, DQSG_MODE=1, NARROW_IG=0
	vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL),
		P_Fld(1, MISC_STBCAL_STB_DQIEN_IG) |
		P_Fld(1, MISC_STBCAL_PICHGBLOCK_NORD) |
		P_Fld(0, MISC_STBCAL_REFUICHG) |
		P_Fld(0, MISC_STBCAL_PHYVALID_IG) |
		P_Fld(0, MISC_STBCAL_STBSTATE_OPT) |
		P_Fld(0, MISC_STBCAL_STBDLELAST_FILTER) |
		P_Fld(0, MISC_STBCAL_STBDLELAST_PULSE) |
		P_Fld(0, MISC_STBCAL_STBDLELAST_OPT) |
		P_Fld(1, MISC_STBCAL_PIMASK_RKCHG_OPT));

	vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL1),
		P_Fld(1, MISC_STBCAL1_STBCAL_FILTER) |
		//cc mark P_Fld(1, MISC_STBCAL1_STB_FLAGCLR) |
		P_Fld(0, MISC_STBCAL1_STB_SHIFT_DTCOUT_IG));


	vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CTRL0),
		P_Fld(0, MISC_CTRL0_R_DMDQSIEN_FIFO_EN) |
		P_Fld(3, MISC_CTRL0_R_DMVALID_DLY) |
		P_Fld(1, MISC_CTRL0_R_DMVALID_DLY_OPT) |
		P_Fld(0, MISC_CTRL0_R_DMVALID_NARROW_IG));
		//cc mark P_Fld(0xf, MISC_CTRL0_R_DMDQSIEN_SYNCOPT));

	vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ6),
		1, B0_DQ6_RG_RX_ARDQ_DMRANK_OUTSEL_B0);
	vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ6),
		1, B1_DQ6_RG_RX_ARDQ_DMRANK_OUTSEL_B1);
	vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_REG_CA_CMD6),
		1, CA_CMD6_RG_RX_ARCMD_DMRANK_OUTSEL);
#endif
#endif
}
#endif

#if TX_PICG_NEW_MODE
#if 0
void GetTXPICGSetting(DRAMC_CTX_T * p)
{
	U32 u4DQS_OEN_final, u4DQ_OEN_final;
	U16 u2DQS_OEN_2T[2], u2DQS_OEN_05T[2], u2DQS_OEN_Delay[2];
	U16 u2DQ_OEN_2T[2], u2DQ_OEN_05T[2], u2DQ_OEN_Delay[2];
	U16 u2COMB_TX_SEL[2];
	U16 u2Shift_Div[2];
	U16 u2COMB_TX_PICG_CNT;
	U8 u1CHIdx, u1RankIdx, u1Rank_bak = u1GetRank(p), u1backup_CH = vGetPHY2ChannelMapping(p), u1Div_ratio;

	mcSHOW_DBG_MSG(("****** GetTXPICGSetting DDR[%d] @@@\n", p->frequency * 2));

	for (u1CHIdx = 0; u1CHIdx < p->support_channel_num; u1CHIdx++)
	{
		vSetPHY2ChannelMapping(p, u1CHIdx);
		//Set TX DQS PICG
		//DQS0
		u2DQS_OEN_2T[0] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_DQS0), SHU_SELPH_DQS0_TXDLY_OEN_DQS0);//m
		u2DQS_OEN_05T[0] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_DQS1), SHU_SELPH_DQS1_DLY_OEN_DQS0);//n
		//DQS1
		u2DQS_OEN_2T[1] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_DQS0), SHU_SELPH_DQS0_TXDLY_OEN_DQS1);//m
		u2DQS_OEN_05T[1] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_DQS1), SHU_SELPH_DQS1_DLY_OEN_DQS1);//n

		mcSHOW_DBG_MSG(("CH%d\n", u1CHIdx));
		mcSHOW_DBG_MSG(("DQS0 m=%d n=%d \n", u2DQS_OEN_2T[0], u2DQS_OEN_05T[0]));
		mcSHOW_DBG_MSG(("DQS1 m=%d n=%d \n", u2DQS_OEN_2T[1], u2DQS_OEN_05T[1]));



		u2COMB_TX_SEL[0] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_APHY_TX_PICG_CTRL), SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQS_SEL_P0);
		u2COMB_TX_SEL[1] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_APHY_TX_PICG_CTRL), SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQS_SEL_P1);
		u2COMB_TX_PICG_CNT = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_APHY_TX_PICG_CTRL), SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_PICG_CNT);

		mcSHOW_DBG_MSG(("TX_DQS_SEL_P0 %d \n", u2COMB_TX_SEL[0]));
		mcSHOW_DBG_MSG(("TX_DQS_SEL_P1 %d \n", u2COMB_TX_SEL[1]));
		mcSHOW_DBG_MSG(("COMB_TX_PICG_CNT %d \n", u2COMB_TX_PICG_CNT));

		//Set TX RK0 and RK1 DQ PICG
		for (u1RankIdx = RANK_0; u1RankIdx < p->support_rank_num; u1RankIdx++)
		{
			mcSHOW_DBG_MSG(("Rank%d\n", u1RankIdx));

			vSetRank(p, u1RankIdx);
			//DQ0
			u2DQ_OEN_2T[0] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ0), SHURK_SELPH_DQ0_TXDLY_OEN_DQ0);//p
			u2DQ_OEN_05T[0] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ2), SHURK_SELPH_DQ2_DLY_OEN_DQ0);//q
			//DQ1
			u2DQ_OEN_2T[1] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ0), SHURK_SELPH_DQ0_TXDLY_OEN_DQ1);//p
			u2DQ_OEN_05T[1] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ2), SHURK_SELPH_DQ2_DLY_OEN_DQ1);//q

			mcSHOW_DBG_MSG(("DQ0 p=%d q=%d \n", u2DQ_OEN_2T[0], u2DQ_OEN_05T[0]));
			mcSHOW_DBG_MSG(("DQ1 p=%d q=%d \n", u2DQ_OEN_2T[1], u2DQ_OEN_05T[1]));

			u2COMB_TX_SEL[0] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_APHY_TX_PICG_CTRL), SHURK_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQ_RK_SEL_P0);
			u2COMB_TX_SEL[1] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_APHY_TX_PICG_CTRL), SHURK_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQ_RK_SEL_P1);

			mcSHOW_DBG_MSG(("TX_DQ_RK_SEL_P0 %d \n", u2COMB_TX_SEL[0]));
			mcSHOW_DBG_MSG(("TX_DQ_RK_SEL_P1 %d \n", u2COMB_TX_SEL[1]));
		}
		vSetRank(p, u1Rank_bak);
	}
	vSetPHY2ChannelMapping(p, u1backup_CH);
}
#endif

#define ADD_1UI_TO_APHY 1 //After A60-868/Pe-trus
void TXPICGSetting(DRAMC_CTX_T * p)
{
	U32 u4DQS_OEN_final, u4DQ_OEN_final;
	U16 u2DQS_OEN_2T[2], u2DQS_OEN_05T[2], u2DQS_OEN_Delay[2];
	U16 u2DQ_OEN_2T[2], u2DQ_OEN_05T[2], u2DQ_OEN_Delay[2];
	U16 u2COMB_TX_SEL[2];
	U16 u2Shift_DQS_Div[2];
	U16 u2Shift_DQ_Div[2];
	U16 u2COMB_TX_PICG_CNT;
	U8 u1CHIdx, u1RankIdx, u1Rank_bak = u1GetRank(p), u1backup_CH = vGetPHY2ChannelMapping(p), u1Div_ratio;

	u2COMB_TX_PICG_CNT = 3;//After Pe-trus, could detect HW OE=1 -> 0 automatically, and prolong TX picg
	if (vGet_Div_Mode(p) == DIV8_MODE)
	{
		u2Shift_DQS_Div[0] = 10;//phase 0
		u2Shift_DQS_Div[1] = 6;//phase 1
		u2Shift_DQ_Div[0] = 8;//phase 0
		u2Shift_DQ_Div[1] = 4;//phase 1
		u1Div_ratio = 3;
	}
	else //DIV4_MODE
	{
		u2Shift_DQS_Div[0] = 2;//phase 0
		u2Shift_DQS_Div[1] = 0;//phase 1, no use
		u2Shift_DQ_Div[0] = 0;//phase 0
		u2Shift_DQ_Div[1] = 0;//phase 1, no use
		u1Div_ratio = 2;
	}

	for (u1CHIdx = 0; u1CHIdx < p->support_channel_num; u1CHIdx++)
	{
		vSetPHY2ChannelMapping(p, u1CHIdx);
		//Set TX DQS PICG
		//DQS0
		u2DQS_OEN_2T[0] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_DQS0), SHU_SELPH_DQS0_TXDLY_OEN_DQS0);//m
		u2DQS_OEN_05T[0] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_DQS1), SHU_SELPH_DQS1_DLY_OEN_DQS0);//n
		u2DQS_OEN_Delay[0] = (u2DQS_OEN_2T[0] << u1Div_ratio) + u2DQS_OEN_05T[0];
		//DQS1
		u2DQS_OEN_2T[1] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_DQS0), SHU_SELPH_DQS0_TXDLY_OEN_DQS1);//m
		u2DQS_OEN_05T[1] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_DQS1), SHU_SELPH_DQS1_DLY_OEN_DQS1);//n
		u2DQS_OEN_Delay[1] = (u2DQS_OEN_2T[1] << u1Div_ratio) + u2DQS_OEN_05T[1];

		u4DQS_OEN_final = (u2DQS_OEN_Delay[0] > u2DQS_OEN_Delay[1])? u2DQS_OEN_Delay[1]: u2DQS_OEN_Delay[0]; //choose minimum value
		u4DQS_OEN_final += ADD_1UI_TO_APHY;


		u2COMB_TX_SEL[0] = (u4DQS_OEN_final > u2Shift_DQS_Div[0])? ((u4DQS_OEN_final - u2Shift_DQS_Div[0]) >> u1Div_ratio): 0;

		if (vGet_Div_Mode(p) == DIV4_MODE)
			u2COMB_TX_SEL[1] = 0;
		else
			u2COMB_TX_SEL[1] = (u4DQS_OEN_final > u2Shift_DQS_Div[1])? ((u4DQS_OEN_final - u2Shift_DQS_Div[1]) >> u1Div_ratio): 0;

		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_APHY_TX_PICG_CTRL), P_Fld(u2COMB_TX_SEL[0], SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQS_SEL_P0)
											 | P_Fld(u2COMB_TX_SEL[1], SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQS_SEL_P1)
											 | P_Fld(u2COMB_TX_PICG_CNT, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_PICG_CNT));
		//Set TX RK0 and RK1 DQ PICG
		for (u1RankIdx = RANK_0; u1RankIdx < p->support_rank_num; u1RankIdx++)
		{
			vSetRank(p, u1RankIdx);
			//DQ0
			u2DQ_OEN_2T[0] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ0), SHURK_SELPH_DQ0_TXDLY_OEN_DQ0);
			u2DQ_OEN_05T[0] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ2), SHURK_SELPH_DQ2_DLY_OEN_DQ0);
			u2DQ_OEN_Delay[0] = (u2DQ_OEN_2T[0] << u1Div_ratio) + u2DQ_OEN_05T[0];
			//DQ1
			u2DQ_OEN_2T[1] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ0), SHURK_SELPH_DQ0_TXDLY_OEN_DQ1);
			u2DQ_OEN_05T[1] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ2), SHURK_SELPH_DQ2_DLY_OEN_DQ1);
			u2DQ_OEN_Delay[1] = (u2DQ_OEN_2T[1] << u1Div_ratio) + u2DQ_OEN_05T[1];


			u4DQ_OEN_final = (u2DQ_OEN_Delay[0] > u2DQ_OEN_Delay[1])? u2DQ_OEN_Delay[1]: u2DQ_OEN_Delay[0]; //choose minimum value
			u4DQ_OEN_final += ADD_1UI_TO_APHY;

			u2COMB_TX_SEL[0] = (u4DQ_OEN_final > u2Shift_DQ_Div[0])? ((u4DQ_OEN_final - u2Shift_DQ_Div[0]) >> u1Div_ratio): 0;

			if (vGet_Div_Mode(p) == DIV4_MODE)
			   u2COMB_TX_SEL[1] = 0;
			else
			   u2COMB_TX_SEL[1] = (u4DQ_OEN_final > u2Shift_DQ_Div[1])? ((u4DQ_OEN_final - u2Shift_DQ_Div[1]) >> u1Div_ratio): 0;

			   vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHURK_APHY_TX_PICG_CTRL), P_Fld(u2COMB_TX_SEL[0], SHURK_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQ_RK_SEL_P0)
												 | P_Fld(u2COMB_TX_SEL[1], SHURK_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQ_RK_SEL_P1));
		}
		vSetRank(p, u1Rank_bak);
	}
	vSetPHY2ChannelMapping(p, u1backup_CH);
}
#endif


#if RX_PICG_NEW_MODE
void RXPICGSetting(DRAMC_CTX_T * p)
{
	DRAM_RANK_T bkRank = u1GetRank(p);
	U8 u1RankIdx = 0;

	vIO32WriteFldAlign(DDRPHY_REG_MISC_SHU_STBCAL, 0, MISC_SHU_STBCAL_STBCALEN);
	vIO32WriteFldAlign(DDRPHY_REG_MISC_SHU_STBCAL, 0, MISC_SHU_STBCAL_STB_SELPHCALEN);

	//PI_CG_DQSIEN new mode
	vIO32WriteFldAlign(DDRPHY_REG_MISC_STBCAL1, 1, MISC_STBCAL1_STBCNT_SHU_RST_EN);
	vIO32WriteFldAlign(DDRPHY_REG_MISC_STBCAL2, 1, MISC_STBCAL2_DQSIEN_SELPH_BY_RANK_EN);
	vIO32WriteFldAlign(DDRPHY_REG_MISC_SHU_STBCAL, 1, MISC_SHU_STBCAL_DQSIEN_PICG_MODE);

	//APHY control new mode
	vIO32WriteFldAlign(DDRPHY_REG_MISC_RX_IN_GATE_EN_CTRL, 1, MISC_RX_IN_GATE_EN_CTRL_RX_IN_GATE_EN_OPT);
	vIO32WriteFldAlign(DDRPHY_REG_MISC_RX_IN_BUFF_EN_CTRL, 1, MISC_RX_IN_BUFF_EN_CTRL_RX_IN_BUFF_EN_OPT);

	//Dummy code (based on DVT document Verification plan of RX PICG efficiency improvment.docx)
	//No need to set since HW setting or setting in other place
	//Pls. don't remove for the integrity
	{
		U8 u1TAIL_LAT = (vGet_Div_Mode(p) == DIV4_MODE) ? 1: 0;
		vIO32WriteFldAlign(DDRPHY_REG_MISC_STBCAL2, 0, MISC_STBCAL2_STB_STBENRST_EARLY_1T_EN);

		for (u1RankIdx = 0; u1RankIdx < p->support_rank_num; u1RankIdx++)//Should set 2 rank
		{
			vSetRank(p, u1RankIdx);
			vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RK_DQSIEN_PICG_CTRL, P_Fld(u1TAIL_LAT, MISC_SHU_RK_DQSIEN_PICG_CTRL_DQSIEN_PICG_TAIL_EXT_LAT)
					| P_Fld(0, MISC_SHU_RK_DQSIEN_PICG_CTRL_DQSIEN_PICG_HEAD_EXT_LAT));
		}
		vSetRank(p, bkRank);

		vIO32WriteFldMulti(DDRPHY_REG_MISC_RX_IN_BUFF_EN_CTRL, P_Fld(0, MISC_RX_IN_BUFF_EN_CTRL_DIS_IN_BUFF_EN)
					| P_Fld(0, MISC_RX_IN_BUFF_EN_CTRL_FIX_IN_BUFF_EN)
					| P_Fld(0, MISC_RX_IN_BUFF_EN_CTRL_RX_IN_BUFF_EN_4BYTE_EN));

		vIO32WriteFldMulti(DDRPHY_REG_MISC_RX_IN_GATE_EN_CTRL, P_Fld(0, MISC_RX_IN_GATE_EN_CTRL_DIS_IN_GATE_EN)
					| P_Fld(0, MISC_RX_IN_GATE_EN_CTRL_FIX_IN_GATE_EN)
					| P_Fld(0, MISC_RX_IN_GATE_EN_CTRL_RX_IN_GATE_EN_4BYTE_EN));

	}
#if 0
		vIO32WriteFldAlign(DRAMC_REG_STBCAL2, 0, STBCAL2_STB_STBENRST_EARLY_1T_EN);
		vIO32WriteFldMulti(DRAMC_REG_SHU_STBCAL, P_Fld(u1TAIL_LAT, SHU_STBCAL_R1_DQSIEN_PICG_TAIL_EXT_LAT)
					| P_Fld(0, SHU_STBCAL_R1_DQSIEN_PICG_HEAD_EXT_LAT)
					| P_Fld(u1TAIL_LAT, SHU_STBCAL_R0_DQSIEN_PICG_TAIL_EXT_LAT)
					| P_Fld(0, SHU_STBCAL_R0_DQSIEN_PICG_HEAD_EXT_LAT));
		vIO32WriteFldMulti(DRAMC_REG_PHY_RX_INCTL, P_Fld(0, PHY_RX_INCTL_DIS_IN_BUFF_EN)
					| P_Fld(0, PHY_RX_INCTL_FIX_IN_BUFF_EN)
					| P_Fld(0, PHY_RX_INCTL_RX_IN_BUFF_EN_4BYTE_EN)
					| P_Fld(0, PHY_RX_INCTL_DIS_IN_GATE_EN)
					| P_Fld(0, PHY_RX_INCTL_FIX_IN_GATE_EN)
					| P_Fld(0, PHY_RX_INCTL_RX_IN_GATE_EN_4BYTE_EN));
#endif
}
#endif

#ifndef DPM_CONTROL_AFTERK
void dramc_exit_with_DFS_legacy_mode(DRAMC_CTX_T * p)
{
#if !__ETT__
	//set for SPM DRAM self refresh
	vIO32WriteFldAlign(SPM_POWERON_CONFIG_EN, 1, POWERON_CONFIG_EN_BCLK_CG_EN);
	vIO32WriteFldAlign(SPM_DRAMC_DPY_CLK_SW_CON_2, 1, SPM_DRAMC_DPY_CLK_SW_CON_2_SW_PHYPLL_MODE_SW);
	vIO32WriteFldAlign(SPM_POWER_ON_VAL0, 1, SPM_POWER_ON_VAL0_SC_PHYPLL_MODE_SW);
#endif
	//Preloader exit with legacy mode for CTP load used
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 0x0, MISC_RG_DFS_CTRL_SPM_DVFS_CONTROL_SEL);
	vIO32WriteFldAlign_All(DDRPHY_REG_PHYPLL0, 0x0, PHYPLL0_RG_RPHYPLL_EN);
	vIO32WriteFldAlign_All(DDRPHY_REG_CLRPLL0, 0x0, CLRPLL0_RG_RCLRPLL_EN);
}
#endif

#if TX_PICG_NEW_MODE
void TXPICGNewModeEnable(DRAMC_CTX_T * p)
{
	//Switch TX PICG to new mode
	vIO32WriteFldMulti_All(DDRPHY_REG_MISC_CTRL3, P_Fld(0, MISC_CTRL3_ARPI_CG_MCK_DQ_OPT)
										 | P_Fld(0, MISC_CTRL3_ARPI_MPDIV_CG_DQ_OPT)
										 | P_Fld(0, MISC_CTRL3_ARPI_CG_DQS_OPT)
										 | P_Fld(0, MISC_CTRL3_ARPI_CG_DQ_OPT));
}
#endif

#if ENABLE_WRITE_DBI_Protect
void ApplyWriteDBIProtect(DRAMC_CTX_T *p, U8 onoff)
{
#if __A60868_TO_BE_PORTING__
	U8 *uiLPDDR_O1_Mapping;
	U16 Temp_PinMux_MaskWrite_WriteDBIOn = 0;
	U8 B0_PinMux_MaskWrite_WriteDBIOn = 0, B1_PinMux_MaskWrite_WriteDBIOn = 0;
	int DQ_index;

	uiLPDDR_O1_Mapping = (U8 *)uiLPDDR4_O1_Mapping_POP[p->channel];

	// Write DMI/DBI Protect Function
	// Byte0 can not have bit swap between Group1(DQ0/1) and Group2(DQ02~DQ07).
	// Byte1 can not have bit swap between Group1(DQ8/9) and Group2(DQ10~DQ15).
	// DBIWR_IMP_EN=1 and DBIWR_PINMUX_EN=1
	// set DBIWR_OPTB0[7:0] meet with Byte0 pin MUX table.
	// set DBIWR_OPTB1[7:0] meet with Byte1 pin MUX table.

	for (DQ_index = 0; DQ_index < 16; DQ_index++)
	{
		Temp_PinMux_MaskWrite_WriteDBIOn |= ((0x7C7C >> uiLPDDR_O1_Mapping[DQ_index]) & 0x1) << DQ_index;
	}
	B1_PinMux_MaskWrite_WriteDBIOn = (U8)(Temp_PinMux_MaskWrite_WriteDBIOn >> 8) & 0xff;
	B0_PinMux_MaskWrite_WriteDBIOn = (U8) Temp_PinMux_MaskWrite_WriteDBIOn & 0xff;

	vIO32WriteFldMulti_All(DRAMC_REG_ARBCTL, P_Fld(B1_PinMux_MaskWrite_WriteDBIOn, ARBCTL_DBIWR_OPT_B1)
										   | P_Fld(B0_PinMux_MaskWrite_WriteDBIOn, ARBCTL_DBIWR_OPT_B0)
										   | P_Fld(onoff, ARBCTL_DBIWR_PINMUX_EN)
										   | P_Fld(onoff, ARBCTL_DBIWR_IMP_EN));
#endif
}
#endif

#if ENABLE_WRITE_DBI
void ApplyWriteDBIPowerImprove(DRAMC_CTX_T *p, U8 onoff)
{

	// set DBIWR_IMP_EN = 1
	// DBIWR_OPTB0[1:0]=0, DBIWR_OPT_B0[7]=0
	// DBIWR_OPTB1[1:0]=0, DBIWR_OPT_B1[7]=0
	vIO32WriteFldMulti_All(DRAMC_REG_DBIWR_PROTECT, P_Fld(0, DBIWR_PROTECT_DBIWR_OPT_B1)
									   | P_Fld(0, DBIWR_PROTECT_DBIWR_OPT_B0)
									   | P_Fld(0, DBIWR_PROTECT_DBIWR_PINMUX_EN)
									   | P_Fld(onoff, DBIWR_PROTECT_DBIWR_IMP_EN));
}
/* DDR800 mode struct declaration (declared here due Fld_wid for each register type) */
/*
typedef struct _DDR800Mode_T
{
	U8 dll_phdet_en_b0: Fld_wid(SHU_B0_DLL0_RG_ARDLL_PHDET_EN_B0_SHU);
	U8 dll_phdet_en_b1: Fld_wid(SHU_B1_DLL0_RG_ARDLL_PHDET_EN_B1_SHU);
	U8 dll_phdet_en_ca_cha: Fld_wid(SHU_CA_DLL0_RG_ARDLL_PHDET_EN_CA_SHU);
	U8 dll_phdet_en_ca_chb: Fld_wid(SHU_CA_DLL0_RG_ARDLL_PHDET_EN_CA_SHU);
	U8 phypll_ada_mck8x_en: Fld_wid(SHU_PLL22_RG_RPHYPLL_ADA_MCK8X_EN_SHU);
	U8 ddr400_en_b0: Fld_wid(SHU_B0_DQ6_RG_ARPI_DDR400_EN_B0);
	U8 ddr400_en_b1: Fld_wid(SHU_B1_DQ6_RG_ARPI_DDR400_EN_B1);
	U8 ddr400_en_ca: Fld_wid(SHU_CA_CMD6_RG_ARPI_DDR400_EN_CA);
	U8 phypll_ddr400_en: Fld_wid(SHU_PLL1_RG_RPHYPLL_DDR400_EN);
	U8 ddr400_dqs_ps_b0: Fld_wid(SHU_B0_DQ9_RG_DDR400_DQS_PS_B0);
	U8 ddr400_dqs_ps_b1: Fld_wid(SHU_B1_DQ9_RG_DDR400_DQS_PS_B1);
	U8 ddr400_dq_ps_b0: Fld_wid(SHU_B0_DQ9_RG_DDR400_DQ_PS_B0);
	U8 ddr400_dq_ps_b1: Fld_wid(SHU_B1_DQ9_RG_DDR400_DQ_PS_B1);
	U8 ddr400_dqs_ps_ca: Fld_wid(SHU_CA_CMD9_RG_DDR400_DQS_PS_CA);
	U8 ddr400_dq_ps_ca: Fld_wid(SHU_CA_CMD9_RG_DDR400_DQ_PS_CA);
	U8 ddr400_semi_en_b0: Fld_wid(SHU_B0_DQ9_RG_DDR400_SEMI_EN_B0);
	U8 ddr400_semi_en_b1: Fld_wid(SHU_B1_DQ9_RG_DDR400_SEMI_EN_B1);
	U8 ddr400_semi_en_ca: Fld_wid(SHU_CA_CMD9_RG_DDR400_SEMI_EN_CA);
	U8 ddr400_semi_open_en: Fld_wid(SHU_PLL0_RG_DDR400_SEMI_OPEN_EN);
	U8 pll0_ada_mck8x_chb_en: Fld_wid(SHU_PLL0_ADA_MCK8X_CHB_EN);
	U8 pll0_ada_mck8x_cha_en: Fld_wid(SHU_PLL0_ADA_MCK8X_CHA_EN);
} DDR800Mode_T;
*/
#endif


void RODTSettings(DRAMC_CTX_T *p)
{
	U8 u1VrefSel;
	U8 u1RankIdx, u1RankIdxBak;

	//VREF_EN
	vIO32WriteFldAlign(DDRPHY_REG_B0_DQ5, 1, B0_DQ5_RG_RX_ARDQ_VREF_EN_B0);
	vIO32WriteFldAlign(DDRPHY_REG_B1_DQ5, 1, B1_DQ5_RG_RX_ARDQ_VREF_EN_B1);

	//Update related setting of APHY RX and ODT
	vIO32WriteFldAlign(DDRPHY_REG_SHU_B0_VREF, !(p->odt_onoff), SHU_B0_VREF_RG_RX_ARDQ_VREF_UNTERM_EN_B0);
	vIO32WriteFldAlign(DDRPHY_REG_SHU_B1_VREF, !(p->odt_onoff), SHU_B1_VREF_RG_RX_ARDQ_VREF_UNTERM_EN_B1);

	if(p->odt_onoff==ODT_ON)
	{
		if (p->dram_type==TYPE_LPDDR5)
			u1VrefSel = 0x46;//term LP5
		else
			u1VrefSel = 0x2c;//term LP4
	}
	else
	{
		if (p->dram_type==TYPE_LPDDR5)
			u1VrefSel = 0x37;//unterm LP5
		else
			u1VrefSel = 0x37;//unterm LP4
	}

	u1RankIdxBak = u1GetRank(p);
	for (u1RankIdx = 0; u1RankIdx < (U32)(p->support_rank_num); u1RankIdx++)
	{
		vSetRank(p, u1RankIdx);
		vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_PHY_VREF_SEL,
					P_Fld(u1VrefSel, SHU_B0_PHY_VREF_SEL_RG_RX_ARDQ_VREF_SEL_LB_B0) |
					P_Fld(u1VrefSel, SHU_B0_PHY_VREF_SEL_RG_RX_ARDQ_VREF_SEL_UB_B0));
		vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_PHY_VREF_SEL,
					P_Fld(u1VrefSel, SHU_B1_PHY_VREF_SEL_RG_RX_ARDQ_VREF_SEL_LB_B1) |
					P_Fld(u1VrefSel, SHU_B1_PHY_VREF_SEL_RG_RX_ARDQ_VREF_SEL_UB_B1));
	}
	vSetRank(p, u1RankIdxBak);

	vIO32WriteFldAlign(DDRPHY_REG_SHU_B0_VREF, 1, SHU_B0_VREF_RG_RX_ARDQ_VREF_RANK_SEL_EN_B0);
	vIO32WriteFldAlign(DDRPHY_REG_SHU_B1_VREF, 1, SHU_B1_VREF_RG_RX_ARDQ_VREF_RANK_SEL_EN_B1);

#if ENABLE_TX_WDQS
	vIO32WriteFldAlign(DDRPHY_REG_MISC_SHU_ODTCTRL, 1, MISC_SHU_ODTCTRL_RODTEN);
	vIO32WriteFldAlign(DDRPHY_REG_SHU_B0_DQ7, 1, SHU_B0_DQ7_R_DMRODTEN_B0);
	vIO32WriteFldAlign(DDRPHY_REG_SHU_B1_DQ7, 1, SHU_B1_DQ7_R_DMRODTEN_B1);
#else
	vIO32WriteFldAlign(DDRPHY_REG_MISC_SHU_ODTCTRL, p->odt_onoff, MISC_SHU_ODTCTRL_RODTEN);
	vIO32WriteFldAlign(DDRPHY_REG_SHU_B0_DQ7, p->odt_onoff, SHU_B0_DQ7_R_DMRODTEN_B0);
	vIO32WriteFldAlign(DDRPHY_REG_SHU_B1_DQ7, p->odt_onoff, SHU_B1_DQ7_R_DMRODTEN_B1);
#endif

#if ENABLE_RODT_TRACKING
	//RODT tracking
	vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RODTENSTB, P_Fld(1, MISC_SHU_RODTENSTB_RODTENSTB_TRACK_EN)
																	| P_Fld(1, MISC_SHU_RODTENSTB_RODTENSTB_TRACK_UDFLWCTRL)
																	| P_Fld(0, MISC_SHU_RODTENSTB_RODTENSTB_SELPH_BY_BITTIME));
#endif

	//Darren-vIO32WriteFldAlign(DDRPHY_REG_B0_DQ6, !(p->odt_onoff), B0_DQ6_RG_TX_ARDQ_ODTEN_EXT_DIS_B0);
	//Darren-vIO32WriteFldAlign(DDRPHY_REG_B1_DQ6, !(p->odt_onoff), B1_DQ6_RG_TX_ARDQ_ODTEN_EXT_DIS_B1);
	//Darren-vIO32WriteFldAlign(DDRPHY_REG_CA_CMD6, !(p->odt_onoff), CA_CMD6_RG_TX_ARCMD_ODTEN_EXT_DIS);
	vIO32WriteFldAlign(DDRPHY_REG_SHU_B0_DQ13, !(p->odt_onoff), SHU_B0_DQ13_RG_TX_ARDQ_IO_ODT_DIS_B0);
	vIO32WriteFldAlign(DDRPHY_REG_SHU_B1_DQ13, !(p->odt_onoff), SHU_B1_DQ13_RG_TX_ARDQ_IO_ODT_DIS_B1);
	vIO32WriteFldAlign(DDRPHY_REG_SHU_CA_CMD13, !(p->odt_onoff), SHU_CA_CMD13_RG_TX_ARCA_IO_ODT_DIS_CA);

	//APHY CG disable
	vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ13, P_Fld(0, SHU_B0_DQ13_RG_TX_ARDQS_OE_ODTEN_CG_EN_B0)
											| P_Fld(0, SHU_B0_DQ13_RG_TX_ARDQM_OE_ODTEN_CG_EN_B0));
	vIO32WriteFldAlign(DDRPHY_REG_SHU_B0_DQ14, 0, SHU_B0_DQ14_RG_TX_ARDQ_OE_ODTEN_CG_EN_B0);

	vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ13, P_Fld(0, SHU_B1_DQ13_RG_TX_ARDQS_OE_ODTEN_CG_EN_B1)
											| P_Fld(0, SHU_B1_DQ13_RG_TX_ARDQM_OE_ODTEN_CG_EN_B1));
	vIO32WriteFldAlign(DDRPHY_REG_SHU_B1_DQ14, 0, SHU_B1_DQ14_RG_TX_ARDQ_OE_ODTEN_CG_EN_B1);
}

/* LP4 use 7UI mode (1)
 * LP5 lower than 4266 use 7UI mode (1)
 * LP5 higher than 4266 use 11UI mode (2)
 * LP5 higher than 4266 with better SI use 11/24UI mode (3)
 */
void DQSSTBSettings(DRAMC_CTX_T *p)
{
	unsigned int dqsien_mode = 1;

#if (__LP5_COMBO__)
	U8 rpre_mode = LPDDR5_RPRE_4S_0T;

	if (is_lp5_family(p))
	{
		if (p->frequency > 1600)
			rpre_mode = LPDDR5_RPRE_2S_2T;
	}

	if (rpre_mode == LPDDR5_RPRE_2S_2T)
		dqsien_mode = 2;
	else if (rpre_mode == LPDDR5_RPRE_XS_4T)
		dqsien_mode = 3;
#endif

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_STBCAL),
		dqsien_mode, MISC_SHU_STBCAL_DQSIEN_DQSSTB_MODE);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ10),
		dqsien_mode, SHU_B0_DQ10_RG_RX_ARDQS_DQSIEN_MODE_B0);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ10),
		dqsien_mode, SHU_B1_DQ10_RG_RX_ARDQS_DQSIEN_MODE_B1);
}

static void SetMck8xLowPwrOption(DRAMC_CTX_T *p)
{
#if ENABLE_REMOVE_MCK8X_UNCERT_LOWPOWER_OPTION
	U32 u4Mck8xMode = 1;
#else
	U32 u4Mck8xMode = 0;
#endif

	vIO32WriteFldMulti(DDRPHY_REG_MISC_LP_CTRL,  P_Fld( u4Mck8xMode , MISC_LP_CTRL_RG_SC_ARPI_RESETB_8X_SEQ_LP_SEL ) \
															  | P_Fld( u4Mck8xMode , MISC_LP_CTRL_RG_ADA_MCK8X_8X_SEQ_LP_SEL	  ) \
															  | P_Fld( u4Mck8xMode, MISC_LP_CTRL_RG_AD_MCK8X_8X_SEQ_LP_SEL		 ) \
															  | P_Fld( u4Mck8xMode , MISC_LP_CTRL_RG_MIDPI_EN_8X_SEQ_LP_SEL 	  ) \
															  | P_Fld( u4Mck8xMode , MISC_LP_CTRL_RG_MIDPI_CKDIV4_EN_8X_SEQ_LP_SEL) \
															  | P_Fld( u4Mck8xMode, MISC_LP_CTRL_RG_MCK8X_CG_SRC_LP_SEL 		 ) \
															  | P_Fld( u4Mck8xMode , MISC_LP_CTRL_RG_MCK8X_CG_SRC_AND_LP_SEL	  ));

}

void LP4_UpdateInitialSettings(DRAMC_CTX_T *p)
{
	U8 u1RankIdx, u1RankIdxBak;
	///TODO:
	//BRINGUP-TEST
	vIO32WriteFldAlign(DDRPHY_REG_MISC_CTRL3, 0, MISC_CTRL3_ARPI_CG_CLK_OPT);
	vIO32WriteFldAlign(DDRPHY_REG_MISC_CTRL4, 0, MISC_CTRL4_R_OPT2_CG_CLK);

	//vIO32WriteFldMulti_All(DDRPHY_REG_CA_CMD2, P_Fld(1, CA_CMD2_RG_TX_ARCLK_OE_TIE_EN_CA) | P_Fld(0, CA_CMD2_RG_TX_ARCLK_OE_TIE_SEL_CA));
	//vIO32WriteFldMulti_All(DDRPHY_REG_CA_CMD2, P_Fld(1, CA_CMD2_RG_TX_ARCLKB_OE_TIE_EN_CA) | P_Fld(0, CA_CMD2_RG_TX_ARCLKB_OE_TIE_SEL_CA));
	//Set_MRR_Pinmux_Mapping(p); //Update MRR pinmux

	vReplaceDVInit(p);

	//Let CA and CS be independent
	vIO32WriteFldAlign(DDRPHY_REG_SHU_CA_CMD14, 0xC0, SHU_CA_CMD14_RG_TX_ARCA_MCKIO_SEL_CA); //@Darren, confirm with Alucary
	//Disable perbyte option
	vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ7, P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B0)
											| P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B0)
											| P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_DQM_FLAGSEL_B0));
	vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ7, P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B1)
											| P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B1)
											| P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_DQM_FLAGSEL_B1));

#if RX_PICG_NEW_MODE
	RXPICGSetting(p);
#endif

#if SIMULATION_SW_IMPED // Darren: Need porting by E2 IMP Calib DVT owner
	#if FSP1_CLKCA_TERM
		U8 u1CASwImpFreqRegion = (p->dram_fsp == FSP_0)? IMP_LOW_FREQ: IMP_HIGH_FREQ;
	#else
		U8 u1CASwImpFreqRegion = (p->frequency <= 1866)? IMP_LOW_FREQ: IMP_HIGH_FREQ;
	#endif
		U8 u1DQSwImpFreqRegion = (p->frequency <= 1866)? IMP_LOW_FREQ: IMP_HIGH_FREQ;

	if (p->dram_type == TYPE_LPDDR4X)
		DramcSwImpedanceSaveRegister(p, u1CASwImpFreqRegion, u1DQSwImpFreqRegion, DRAM_DFS_SHUFFLE_1);
#endif

	DQSSTBSettings(p);

	RODTSettings(p);

	//WDBI-OFF
	vIO32WriteFldAlign(DRAMC_REG_SHU_TX_SET0, 0x0, SHU_TX_SET0_DBIWR);

#if CBT_MOVE_CA_INSTEAD_OF_CLK
	U8 u1CaPI = 0, u1CaUI = 0;

	u1CaUI = 1;
	u1CaPI = 0;

	// CA delay shift u1CaUI*UI
	DramcCmdUIDelaySetting(p, u1CaUI);
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_CA5), P_Fld(0x1, SHU_SELPH_CA5_DLY_CS) | P_Fld(0x1, SHU_SELPH_CA5_DLY_CS1));

	// Rank0/1 u1CaPI*PI CA delay

	u1RankIdxBak = u1GetRank(p);

	for (u1RankIdx = 0; u1RankIdx < (U32)(p->support_rank_num); u1RankIdx++)
	{
		vSetRank(p, u1RankIdx);

		CBTDelayCACLK(p, u1CaPI);
	}

	vSetRank(p, u1RankIdxBak);
#endif

#if ENABLE_TPBR2PBR_REFRESH_TIMING
	vIO32WriteFldAlign(DRAMC_REG_REFCTRL1, 0x1, REFCTRL1_REF_OVERHEAD_PBR2PB_ENA); //@Derping
	vIO32WriteFldAlign(DRAMC_REG_MISCTL0, 0x1, MISCTL0_REFP_ARBMASK_PBR2PBR_ENA); //@Unique
	vIO32WriteFldAlign(DRAMC_REG_SCHEDULER_COM, 0x1, SCHEDULER_COM_PBR2PBR_OPT); //@YH
#endif

#if RDSEL_TRACKING_EN
	vIO32WriteFldAlign(DDRPHY_REG_SHU_MISC_RDSEL_TRACK, 0, SHU_MISC_RDSEL_TRACK_DMDATLAT_I); //DMDATLAT_I should be set as 0 before set datlat k value, otherwise the status flag wil be set as 1
#endif

#if ENABLE_WRITE_POST_AMBLE_1_POINT_5_TCK
	vIO32WriteFldAlign(DRAMC_REG_SHU_TX_SET0, p->dram_fsp, SHU_TX_SET0_WPST1P5T); //Set write post-amble by FSP with MR3
#else
	vIO32WriteFldAlign(DRAMC_REG_SHU_TX_SET0, 0x0, SHU_TX_SET0_WPST1P5T); //Set write post-amble by FSP with MR3
#endif

#if (!XRTRTR_NEW_CROSS_RK_MODE)
	vIO32WriteFldAlign(DDRPHY_REG_SHU_MISC_RANK_SEL_STB, 0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_EN);
#endif

	//MP Setting
	vIO32WriteFldMulti(DRAMC_REG_DUMMY_RD, P_Fld(0x1, DUMMY_RD_DMYRD_REORDER_DIS) | P_Fld(0x1, DUMMY_RD_SREF_DMYRD_EN));
	vIO32WriteFldMulti(DRAMC_REG_DRAMCTRL, P_Fld(0x0, DRAMCTRL_ALL_BLOCK_CTO_ALE_DBG_EN)
									 | P_Fld(0x1, DRAMCTRL_DVFS_BLOCK_CTO_ALE_DBG_EN)
									 | P_Fld(0x1, DRAMCTRL_SELFREF_BLOCK_CTO_ALE_DBG_EN));
	vIO32WriteFldAlign(DDRPHY_REG_MISC_STBCAL2, 1, MISC_STBCAL2_DQSGCNT_BYP_REF);
	//@Darren- enable bit11 via FMeter, vIO32WriteFldAlign(DDRPHY_REG_MISC_CG_CTRL7, 0, MISC_CG_CTRL7_CK_BFE_DCM_EN);

	//1:8 --> data rate<=1600 set 0,  data rate<=3200 set 1, else 2
	//1:4 --> data rate<= 800 set 0,  data rate<=1600 set 1, else 2
	if(p->frequency<=800)
		vIO32WriteFldAlign(DDRPHY_REG_MISC_SHU_PHY_RX_CTRL, 0, MISC_SHU_PHY_RX_CTRL_RX_IN_BUFF_EN_HEAD);
	else if(p->frequency<=1200)
		vIO32WriteFldAlign(DDRPHY_REG_MISC_SHU_PHY_RX_CTRL, 1, MISC_SHU_PHY_RX_CTRL_RX_IN_BUFF_EN_HEAD);
	else
		vIO32WriteFldAlign(DDRPHY_REG_MISC_SHU_PHY_RX_CTRL, 2, MISC_SHU_PHY_RX_CTRL_RX_IN_BUFF_EN_HEAD);
	vIO32WriteFldAlign(DDRPHY_REG_MISC_CTRL1, 1, MISC_CTRL1_R_DMARPIDQ_SW);
	vIO32WriteFldMulti(DDRPHY_REG_CA_TX_MCK, P_Fld(0xa, CA_TX_MCK_R_DMRESETB_DRVP_FRPHY) | P_Fld(0xa, CA_TX_MCK_R_DMRESETB_DRVN_FRPHY));
	vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RANK_SEL_LAT, P_Fld(0x3, MISC_SHU_RANK_SEL_LAT_RANK_SEL_LAT_B0) |
			P_Fld(0x3, MISC_SHU_RANK_SEL_LAT_RANK_SEL_LAT_B1) | P_Fld(0x3, MISC_SHU_RANK_SEL_LAT_RANK_SEL_LAT_CA));

	SetMck8xLowPwrOption(p);
}


void LP5_UpdateInitialSettings(DRAMC_CTX_T *p)
{
	vIO32WriteFldAlign_All(DDRPHY_REG_SHU_CA_CMD14, 0x0, SHU_CA_CMD14_RG_TX_ARCA_MCKIO_SEL_CA); //Let CA and CS be independent
	//Set_MRR_Pinmux_Mapping(p); //Update MRR pinmux

	//Disable perbyte option
	vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ7, P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B0)
											| P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B0)
											| P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_DQM_FLAGSEL_B0));
	vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ7, P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B1)
											| P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B1)
											| P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_DQM_FLAGSEL_B1));

	///TODO: Temp solution. May need to resolve in init flow
	vIO32WriteFldMulti_All(DDRPHY_REG_MISC_CG_CTRL5, /* Will cause PI un-adjustable */
		P_Fld(0x0, MISC_CG_CTRL5_R_CA_DLY_DCM_EN) |
		P_Fld(0x0, MISC_CG_CTRL5_R_CA_PI_DCM_EN) |
		P_Fld(0x0, MISC_CG_CTRL5_R_DQ0_DLY_DCM_EN) |
		P_Fld(0x0, MISC_CG_CTRL5_R_DQ0_PI_DCM_EN) |
		P_Fld(0x0, MISC_CG_CTRL5_R_DQ1_DLY_DCM_EN) |
		P_Fld(0x0, MISC_CG_CTRL5_R_DQ1_PI_DCM_EN));

	DQSSTBSettings(p);

	RODTSettings(p);

#if SIMULATION_SW_IMPED
	#if FSP1_CLKCA_TERM
		U8 u1CASwImpFreqRegion = (p->dram_fsp == FSP_0)? IMP_LOW_FREQ: IMP_HIGH_FREQ;
	#else
		U8 u1CASwImpFreqRegion = (p->frequency <= 1866)? IMP_LOW_FREQ: IMP_HIGH_FREQ;
	#endif
		U8 u1DQSwImpFreqRegion = (p->frequency <= 1866)? IMP_LOW_FREQ: IMP_HIGH_FREQ;

	if (p->dram_type == TYPE_LPDDR5)
		DramcSwImpedanceSaveRegister(p, u1CASwImpFreqRegion, u1DQSwImpFreqRegion, DRAM_DFS_SHUFFLE_1);
#endif

#if RDSEL_TRACKING_EN
	vIO32WriteFldAlign(DDRPHY_REG_SHU_MISC_RDSEL_TRACK, 0, SHU_MISC_RDSEL_TRACK_DMDATLAT_I); //DMDATLAT_I should be set as 0 before set datlat k value, otherwise the status flag wil be set as 1
#endif

#if (!XRTRTR_NEW_CROSS_RK_MODE)
	vIO32WriteFldAlign(DDRPHY_REG_SHU_MISC_RANK_SEL_STB, 0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_EN);
#endif
	SetMck8xLowPwrOption(p);
}

#define CKGEN_FMETER 0x0
#define ABIST_FMETER 0x1

unsigned int DDRPhyFreqMeter(void)
{
#if 0 //temp remove for bringup
#if (FOR_DV_SIMULATION_USED==0)

	unsigned int reg0=0, reg1=0;
	unsigned int before_value=0, after_value=0;
	unsigned int frq_result=0;
#if (fcFOR_CHIP_ID == fcMargaux)
	unsigned int chb_mctl_ca_en = (DRV_Reg32(Channel_B_DDRPHY_AO_BASE_ADDRESS + 0xBA8) >> 19) & 0x1;
#endif

#if 1//def HJ_SIM
	/*TINFO="\n[PhyFreqMeter]"*/

	reg0 = DRV_Reg32(Channel_A_DDRPHY_AO_BASE_ADDRESS + 0x70c) ;
	DRV_WriteReg32	(Channel_A_DDRPHY_AO_BASE_ADDRESS + 0x70c  , reg0 | (1 << 16));
	reg0 = DRV_Reg32(Channel_A_DDRPHY_AO_BASE_ADDRESS + 0x72c) ;
	DRV_WriteReg32	(Channel_A_DDRPHY_AO_BASE_ADDRESS + 0x72c  , reg0 | (1 << 16));

	// abist_clk29: AD_MPLL_CK
	frq_result = FMeter(ABIST_FMETER, 29) ;
	mcSHOW_DBG_MSG(("AD_MPLL_CK FREQ=%d\n", frq_result));
	/*TINFO="AD_MPLL_CK FREQ=%d\n", frq_result*/

	// abist_clk40: DA_MPLL_52M_DIV_CK
	//! frq_result = FMeter(ABIST_FMETER, 40) ;
	/*TINFO="DA_MPLL_52M_DIV_CK FREQ=%d\n", frq_result*/
	#if 1
	if((DRV_Reg32(Channel_A_DDRPHY_NAO_BASE_ADDRESS + 0x50c) & (1<<8))==0)
	{
		// abist_clk31: AD_RCLRPLL_DIV4_CK_ch02
		//frq_result = FMeter(ABIST_FMETER, 31) ;
		mcSHOW_DBG_MSG(("AD_RCLRPLL_DIV4_CK_ch02 FREQ=%d\n", frq_result));
		/*TINFO="AD_RCLRPLL_DIV4_CK_ch02 FREQ=%d\n", frq_result*/
	}
	else
	{
		// abist_clk33: AD_RPHYRPLL_DIV4_CK_ch02
		frq_result = FMeter(ABIST_FMETER, 33) ;
		mcSHOW_DBG_MSG(("AD_RPHYPLL_DIV4_CK_ch02 FREQ=%d\n", frq_result));
		/*TINFO="AD_RPHYPLL_DIV4_CK_ch02 FREQ=%d\n", frq_result*/
	}
	#endif
	//! ch0
	reg0 = DRV_Reg32(Channel_A_DDRPHY_AO_BASE_ADDRESS + 0x504) ;
	DRV_WriteReg32	(Channel_A_DDRPHY_AO_BASE_ADDRESS + 0x504  , reg0 | (1 << 11));

	// abistgen_clk44: fmem_ck_aft_dcm_ch0 (DRAMC CHA's clock after idle mask)
	before_value = FMeter(ABIST_FMETER, 44);
	mcSHOW_DBG_MSG(("fmem_ck_aft_dcm_ch0 FREQ=%d\n", before_value));
	/*TINFO="fmem_ck_aft_dcm_ch0 FREQ=%d\n", after_value*/

#if (fcFOR_CHIP_ID == fcMargaux)
	if (chb_mctl_ca_en == 1)
#endif
	{
		reg0 = DRV_Reg32(Channel_B_DDRPHY_AO_BASE_ADDRESS + 0x504) ;
		DRV_WriteReg32	(Channel_B_DDRPHY_AO_BASE_ADDRESS + 0x504  , reg0 | (1 << 11));
	// abistgen_clk45: fmem_ck_aft_dcm_ch1 (DRAMC CHB's clock after idle mask)
	after_value = FMeter(ABIST_FMETER, 45);
	mcSHOW_DBG_MSG(("fmem_ck_aft_dcm_ch1 FREQ=%d\n", after_value));
	}
	/*TINFO="fmem_ck_aft_dcm_ch1 FREQ=%d\n", after_value*/

	gddrphyfmeter_value = after_value<<2;

	reg0 = DRV_Reg32(Channel_A_DDRPHY_AO_BASE_ADDRESS + 0x70c) ;
	DRV_WriteReg32	(Channel_A_DDRPHY_AO_BASE_ADDRESS + 0x70c  , reg0 & ~(1 << 16));
	reg0 = DRV_Reg32(Channel_A_DDRPHY_AO_BASE_ADDRESS + 0x72c) ;
	DRV_WriteReg32	(Channel_A_DDRPHY_AO_BASE_ADDRESS + 0x72c  , reg0 & ~(1 << 16));


	#if (CHANNEL_NUM>2)
	reg0 = DRV_Reg32(Channel_C_DDRPHY_AO_BASE_ADDRESS + 0x70c) ;
	DRV_WriteReg32	(Channel_C_DDRPHY_AO_BASE_ADDRESS + 0x70c  , reg0 | (1 << 16));
	reg0 = DRV_Reg32(Channel_C_DDRPHY_AO_BASE_ADDRESS + 0x72c) ;
	DRV_WriteReg32	(Channel_C_DDRPHY_AO_BASE_ADDRESS + 0x72c  , reg0 | (1 << 16));
	#if 1
	if((DRV_Reg32(Channel_C_DDRPHY_NAO_BASE_ADDRESS + 0x50c) & (1<<8))==0)
	{
		// abist_clk32: AD_RCLRPLL_DIV4_CK_ch13
		//frq_result = FMeter(ABIST_FMETER, 32) ;
		mcSHOW_DBG_MSG(("AD_RCLRPLL_DIV4_CK_ch13 FREQ=%d\n", frq_result));
		/*TINFO="AD_RCLRPLL_DIV4_CK_ch13 FREQ=%d\n", frq_result*/
	}
	else
	{
		// abist_clk34: AD_RPHYRPLL_DIV4_CK_ch13
		frq_result = FMeter(ABIST_FMETER, 34) ;
		mcSHOW_DBG_MSG(("AD_RPHYPLL_DIV4_CK_ch13 FREQ=%d\n", frq_result));
		/*TINFO="AD_RPHYPLL_DIV4_CK_ch13 FREQ=%d\n", frq_result*/
	}
	#endif

	reg0 = DRV_Reg32(Channel_C_DDRPHY_AO_BASE_ADDRESS + 0x504) ;
	DRV_WriteReg32	(Channel_C_DDRPHY_AO_BASE_ADDRESS + 0x504  , reg0 | (1 << 11));
	reg0 = DRV_Reg32(Channel_D_DDRPHY_AO_BASE_ADDRESS + 0x504) ;
	DRV_WriteReg32	(Channel_D_DDRPHY_AO_BASE_ADDRESS + 0x504  , reg0 | (1 << 11));

	// abistgen_clk46: fmem_ck_aft_dcm_ch2 (DRAMC CHC's clock after idle mask)
	before_value = FMeter(ABIST_FMETER, 46);
	mcSHOW_DBG_MSG(("fmem_ck_aft_dcm_ch2 FREQ=%d\n", before_value));
	/*TINFO="fmem_ck_aft_dcm_ch2 FREQ=%d\n", after_value*/

	// abistgen_clk47: fmem_ck_aft_dcm_ch3 (DRAMC CHC's clock after idle mask)
	after_value = FMeter(ABIST_FMETER, 47);
	mcSHOW_DBG_MSG(("fmem_ck_aft_dcm_ch3 FREQ=%d\n", after_value));
	/*TINFO="fmem_ck_aft_dcm_ch3 FREQ=%d\n", after_value*/

	reg0 = DRV_Reg32(Channel_C_DDRPHY_AO_BASE_ADDRESS + 0x70c) ;
	DRV_WriteReg32	(Channel_C_DDRPHY_AO_BASE_ADDRESS + 0x70c  , reg0 & ~(1 << 16));
	reg0 = DRV_Reg32(Channel_C_DDRPHY_AO_BASE_ADDRESS + 0x72c) ;
	DRV_WriteReg32	(Channel_C_DDRPHY_AO_BASE_ADDRESS + 0x72c  , reg0 & ~(1 << 16));
	#endif

	return (before_value<<16 | after_value);
#endif //! end DSIM
#endif

#endif
	return 0;
}
