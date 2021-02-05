/* SPDX-License-Identifier: BSD-3-Clause */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "dramc_common.h"
#include "dramc_int_global.h"
#include "x_hal_io.h"
#include "sv_c_data_traffic.h"

#define BITMAP_BITS_MAX 		128

#if CBT_MOVE_CA_INSTEAD_OF_CLK
#define MAX_CA_PI_DELAY 		95
#else
#define MAX_CA_PI_DELAY 		63
#endif
#define MAX_CS_PI_DELAY 		63
#define MAX_CLK_PI_DELAY		31

#define PASS_RANGE_NA	0x7fff

#define DIE_NUM_MAX 		1 //LP4 only
static U8 fgwrlevel_done = 0;


#if __ETT__
U8 gETT_WHILE_1_flag = 1;
#endif

U8 u1MR01Value[FSP_MAX];
U8 u1MR02Value[FSP_MAX];
U8 u1MR03Value[FSP_MAX];
U8 u1MR11Value[FSP_MAX];
U8 u1MR18Value[FSP_MAX];
U8 u1MR19Value[FSP_MAX];
U8 u1MR20Value[FSP_MAX];
U8 u1MR21Value[FSP_MAX];
U8 u1MR22Value[FSP_MAX];
U8 u1MR51Value[FSP_MAX];

U8 u1MR04Value[RANK_MAX];
U8 u1MR13Value[RANK_MAX];
U8 u1MR26Value[RANK_MAX];
U8 u1MR30Value[RANK_MAX];

U8 u1MR12Value[CHANNEL_NUM][RANK_MAX][FSP_MAX];
U8 u1MR14Value[CHANNEL_NUM][RANK_MAX][FSP_MAX];
U16 gu2MR0_Value[RANK_MAX] = {0xffff, 0xffff};

#if PINMUX_AUTO_TEST_PER_BIT_RX
S16 gFinalRXPerbitFirstPass[CHANNEL_NUM][DQ_DATA_WIDTH];
#endif
#if PINMUX_AUTO_TEST_PER_BIT_TX
S16 gFinalTXPerbitFirstPass[CHANNEL_NUM][DQ_DATA_WIDTH];
#endif
#if PINMUX_AUTO_TEST_PER_BIT_CA
S16 gFinalCAPerbitFirstPass[CHANNEL_NUM][RANK_MAX][CATRAINING_NUM_LP4];
#endif

#ifdef FOR_HQA_TEST_USED
U16 gFinalCBTVrefCA[CHANNEL_NUM][RANK_MAX];
U16 gFinalCBTCA[CHANNEL_NUM][RANK_MAX][10];
U16 gFinalRXPerbitWin[CHANNEL_NUM][RANK_MAX][DQ_DATA_WIDTH];
U16 gFinalTXPerbitWin[CHANNEL_NUM][RANK_MAX][DQ_DATA_WIDTH];
U16 gFinalTXPerbitWin_min_max[CHANNEL_NUM][RANK_MAX];
U16 gFinalTXPerbitWin_min_margin[CHANNEL_NUM][RANK_MAX];
U16 gFinalTXPerbitWin_min_margin_bit[CHANNEL_NUM][RANK_MAX];
S8 gFinalClkDuty[CHANNEL_NUM];
U32 gFinalClkDutyMinMax[CHANNEL_NUM][2];
S8 gFinalDQSDuty[CHANNEL_NUM][DQS_NUMBER];
U32 gFinalDQSDutyMinMax[CHANNEL_NUM][DQS_NUMBER][2];
#endif

U8 gFinalCBTVrefDQ[CHANNEL_NUM][RANK_MAX];
U8 gFinalRXVrefDQ[CHANNEL_NUM][RANK_MAX][2];
U8 gFinalTXVrefDQ[CHANNEL_NUM][RANK_MAX];

#if defined(RELEASE)
U8 gEye_Scan_color_flag = 0;
U8 gCBT_EYE_Scan_flag = 0;
U8 gCBT_EYE_Scan_only_higheset_freq_flag = 1;
U8 gRX_EYE_Scan_flag = 0;
U8 gRX_EYE_Scan_only_higheset_freq_flag = 1;
U8 gTX_EYE_Scan_flag = 1;
U8 gTX_EYE_Scan_only_higheset_freq_flag = 1;
U8 gEye_Scan_unterm_highest_flag = 0;
#elif  (CFG_DRAM_LOG_TO_STORAGE)
U8 gEye_Scan_color_flag = 0;
U8 gCBT_EYE_Scan_flag = 0;
U8 gCBT_EYE_Scan_only_higheset_freq_flag = 1;
U8 gRX_EYE_Scan_flag = 1;
U8 gRX_EYE_Scan_only_higheset_freq_flag = 1;
U8 gTX_EYE_Scan_flag = 1;
U8 gTX_EYE_Scan_only_higheset_freq_flag = 1;
U8 gEye_Scan_unterm_highest_flag = 0;
#else
U8 gEye_Scan_color_flag = 1;
U8 gCBT_EYE_Scan_flag = 0;
U8 gCBT_EYE_Scan_only_higheset_freq_flag = 1;
U8 gRX_EYE_Scan_flag = 0;
U8 gRX_EYE_Scan_only_higheset_freq_flag = 1;
U8 gTX_EYE_Scan_flag = 0;
U8 gTX_EYE_Scan_only_higheset_freq_flag = 1;
U8 gEye_Scan_unterm_highest_flag = 0;
#endif

#ifdef FOR_HQA_REPORT_USED
#if CFG_DRAM_LOG_TO_STORAGE
U8 gHQALog_flag = 1;
#else
U8 gHQALog_flag = 0;
#endif
U16 gHQALOG_RX_delay_cell_ps_075V = 0;
#endif

#if (TX_AUTO_K_ENABLE && TX_AUTO_K_WORKAROUND)
U32 u4DQM_MCK_RK1_backup;
U32 u4DQM_UI_RK1_backup;
U32 u4DQM_PI_RK1_backup[2];
U32 u4DQ_MCK_RK1_backup;
U32 u4DQ_UI_RK1_backup;
U32 u4DQ_PI_RK1_backup[2];
#endif

#if SIMULATION_RX_DVS
U8 u1DVS_increase[RANK_MAX][DQS_NUMBER_LP4];
#endif

static S32 CATrain_CmdDelay[CHANNEL_NUM][RANK_MAX];
static U32 CATrain_CsDelay[CHANNEL_NUM][RANK_MAX];

static S32 wrlevel_dqs_final_delay[RANK_MAX][DQS_NUMBER]; // 3 is channel number
static U16 u2g_num_dlycell_perT = 49;
U16 u2gdelay_cell_ps;
U16 u2g_num_dlycell_perT_all[DRAM_DFS_SHUFFLE_MAX][CHANNEL_NUM];///TODO: to be removed by Francis
U16 u2gdelay_cell_ps_all[DRAM_DFS_SHUFFLE_MAX][CHANNEL_NUM];///TODO: to be removed by Francis
U32 u4gVcore[DRAM_DFS_SHUFFLE_MAX];

U8 gFinalRXVrefDQForSpeedUp[CHANNEL_NUM][RANK_MAX][2/*ODT_onoff*/][2/*2bytes*/] = {0};
U32 gDramcSwImpedanceResult[IMP_VREF_MAX][IMP_DRV_MAX] = {{0,0,0,0},{0,0,0,0},{0,0,0,0}};//ODT_ON/OFF x DRVP/DRVN/ODTP/ODTN

S16 gu2RX_DQS_Duty_Offset[DQS_NUMBER][2];

#define RX_DELAY_PRE_CAL 1
#if RX_DELAY_PRE_CAL
S16 s2RxDelayPreCal=PASS_RANGE_NA;
#endif

#if MRW_CHECK_ONLY
U16 u2MRRecord[CHANNEL_NUM][RANK_MAX][FSP_MAX][MR_NUM];
#endif
#if MRW_CHECK_ONLY || MRW_BACKUP
U8 gFSPWR_Flag[RANK_MAX]={FSP_0};
#endif

#define IN_CBT	(0)
#define OUT_CBT (1)

#if PRINT_CALIBRATION_SUMMARY
static void vSetCalibrationResult(DRAMC_CTX_T *p, U8 ucCalType, U8 ucResult)
{
	U32 *Pointer_CalExecute,*Pointer_CalResult;
	if (ucCalType == DRAM_CALIBRATION_SW_IMPEDANCE)
	{
		Pointer_CalExecute = &p->SWImpCalExecute;
		Pointer_CalResult = &p->SWImpCalResult;
	}
	else
	{
		Pointer_CalExecute = &p->aru4CalExecuteFlag[p->channel][p->rank];
		Pointer_CalResult = &p->aru4CalResultFlag[p->channel][p->rank];
	}

	if (ucResult == DRAM_FAIL)	// Calibration FAIL
	{
		*Pointer_CalExecute |= (1<<ucCalType); // ececution done
		*Pointer_CalResult |= (1<<ucCalType); // no result found
			 }
	else if(ucResult == DRAM_OK)  // Calibration OK
	{
		*Pointer_CalExecute |= (1<<ucCalType); // ececution done
		*Pointer_CalResult &= (~(1<<ucCalType)); // result found
	}
	else if(ucResult == DRAM_FAST_K)  // FAST K
	{
		 *Pointer_CalExecute &= (~(1<<ucCalType)); // no ececution
		 *Pointer_CalResult &= (~(1<<ucCalType)); // result found
	}
	else  // NO K
	{
		  *Pointer_CalExecute &= (~(1<<ucCalType)); // no ececution
		  *Pointer_CalResult |= (1<<ucCalType); // no result found
	}
}

#if PRINT_CALIBRATION_SUMMARY_FASTK_CHECK
void Fast_K_CheckResult(DRAMC_CTX_T *p, U8 ucCalType)
{
	U32 CheckResult=0xFFFFFFFF;
	U32 debug_cnt[2], u4all_result_R, u4all_result_F;
	BOOL FastK_Check_flag=0;
	U32 *Pointer_FastKExecute,*Pointer_FastKResult;

	Pointer_FastKExecute = &p->FastKExecuteFlag[p->channel][p->rank];
	Pointer_FastKResult = &p->FastKResultFlag[p->channel][p->rank];

	if ((ucCalType==DRAM_CALIBRATION_TX_PERBIT)||(ucCalType==DRAM_CALIBRATION_DATLAT)||(ucCalType==DRAM_CALIBRATION_RX_PERBIT))
	{
		DramcEngine2Init(p, p->test2_1, p->test2_2, TEST_XTALK_PATTERN, 0, TE_NO_UI_SHIFT);
		CheckResult = DramcEngine2Run(p,TE_OP_WRITE_READ_CHECK , TEST_XTALK_PATTERN);
		DramcEngine2End(p);
		FastK_Check_flag=1;
	}
	else if (ucCalType==DRAM_CALIBRATION_RX_RDDQC)
	{
		DramcRxWinRDDQCInit(p);
		CheckResult = DramcRxWinRDDQCRun(p);
		DramcRxWinRDDQCEnd(p);
		FastK_Check_flag=1;
	}
	else if (ucCalType==DRAM_CALIBRATION_GATING)
	{
		DramcEngine2Init(p, 0x55000000, 0xaa000000 |0x23, TEST_AUDIO_PATTERN, 0, TE_NO_UI_SHIFT);

		//Gating Counter Reset
		DramPhyReset(p);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL2), 1,
				MISC_STBCAL2_DQSG_CNT_RST);
		mcDELAY_US(1);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL2), 0,
				MISC_STBCAL2_DQSG_CNT_RST);

		DramcEngine2Run(p, TE_OP_READ_CHECK, TEST_AUDIO_PATTERN);

		debug_cnt[0] = u4IO32Read4B(DRAMC_REG_ADDR(DDRPHY_REG_CAL_DQSG_CNT_B0));
		debug_cnt[1] = u4IO32Read4B(DRAMC_REG_ADDR(DDRPHY_REG_CAL_DQSG_CNT_B1));

		//mcSHOW_DBG_MSG((" 0x%X  ",u4DebugCnt))
		if (debug_cnt[0]==0x4646 && debug_cnt[1]==0x4646)
			CheckResult=0;

		DramcEngine2End(p);
		FastK_Check_flag=1;
	}


	if ((FastK_Check_flag==1)&&(CheckResult==0))
	{
		//mcSHOW_DBG_MSG((" [FAST K CHECK]->PASS\n"))
		*Pointer_FastKResult &= (~(1<<ucCalType)); // result PASS
		*Pointer_FastKExecute |= (1<<ucCalType);; // Excuted
	}
	else if ((FastK_Check_flag==1)&&(CheckResult !=0))
	{
		//mcSHOW_DBG_MSG((" [FAST K CHECK]->FAIL\n"))
		*Pointer_FastKResult |= (1<<ucCalType); // result FAIL
		*Pointer_FastKExecute |= (1<<ucCalType);; // Excuted
	}
}
#endif

const char *szCalibStatusName[DRAM_CALIBRATION_MAX]=
{
	"SW Impedance	  ",
	"DUTY Scan		  ",
	"ZQ Calibration   ",
	"Jitter Meter	  ",
	"CBT Training	  ",
	"Write leveling   ",
	"RX DQS gating	  ",
	"RX DQ/DQS(RDDQC) ",
	"TX DQ/DQS		  ",
	"RX DATLAT		  ",
	"RX DQ/DQS(Engine)",
	"TX OE			  ",
};

void vPrintCalibrationResult(DRAMC_CTX_T *p)
{
	U8 ucCHIdx, ucRankIdx, ucCalIdx;
	U32 ucCalResult_All, ucCalExecute_All;
	U8 ucCalResult, ucCalExecute;
	U8 u1CalibrationFail;

	mcSHOW_DBG_MSG(("\n\n[Calibration Summary] %d Mbps\n", p->frequency * 2));

	//for(ucFreqIdx=0; ucFreqIdx<DRAM_DFS_SHUFFLE_MAX; ucFreqIdx++)
	{
		//mcSHOW_DBG_MSG(("==Freqency = %d==\n", get_FreqTbl_by_shuffleIndex(p,ucFreqIdx)->frequency));
		for(ucCHIdx=0; ucCHIdx<p->support_channel_num; ucCHIdx++)
		{
			for(ucRankIdx=0; ucRankIdx<p->support_rank_num; ucRankIdx++)
			{
				u1CalibrationFail =0;
				ucCalExecute_All = p->aru4CalExecuteFlag[ucCHIdx][ucRankIdx];
				ucCalResult_All = p->aru4CalResultFlag[ucCHIdx][ucRankIdx];
				mcSHOW_DBG_MSG(("CH %d, Rank %d\n", ucCHIdx, ucRankIdx));
				//mcSHOW_DBG_MSG(("[vPrintCalibrationResult] Channel = %d, Rank= %d, Freq.= %d, (ucCalExecute_All 0x%x, ucCalResult_All 0x%x)\n", ucCHIdx, ucRankIdx, ucFreqIdx, ucCalExecute_All, ucCalResult_All));

				for(ucCalIdx =0; ucCalIdx<DRAM_CALIBRATION_MAX; ucCalIdx++)
				{
					if(ucCalIdx==0)
						{
							ucCalExecute = (U8)p->SWImpCalExecute; //for SW Impedence
							ucCalResult = (U8)p->SWImpCalResult; //for SW Impedence
						}
					else
						{
							ucCalExecute = (U8)((ucCalExecute_All >>ucCalIdx) & 0x1);
							ucCalResult =  (U8)((ucCalResult_All >>ucCalIdx) & 0x1);
						}

					#if PRINT_CALIBRATION_SUMMARY_DETAIL
					mcSHOW_DBG_MSG(("%s: ", szCalibStatusName[ucCalIdx]))
					if(ucCalExecute==1 && ucCalResult ==1) // excuted and fail
					{
						u1CalibrationFail =1;
						mcSHOW_DBG_MSG(("%s\n", "@_@FAIL@_@"))
					}
					else if (ucCalExecute==1 && ucCalResult ==0) // DRAM_OK
					{
						mcSHOW_DBG_MSG(("%s\n", "PASS"))
					}
					else if (ucCalExecute==0 && ucCalResult ==0) // DRAM_FAST K
					{
						mcSHOW_DBG_MSG(("%s\n", "FAST K"))
					}
					else //DRAM_NO K
					{
						mcSHOW_DBG_MSG(("%s\n", "NO K"))
					}

					#else
					if(ucCalExecute==1 && ucCalResult ==1) // excuted and fail
					{
						u1CalibrationFail =1;
						mcSHOW_DBG_MSG(("%s: %s\n", szCalibStatusName[ucCalIdx],"@_@FAIL@_@"))
					}
					#endif
				}

				if(u1CalibrationFail ==0)
				{
					mcSHOW_DBG_MSG(("All Pass.\n"));
				}
				mcSHOW_DBG_MSG(("\n"));
			}
		}
	}

}
#endif

#if __FLASH_TOOL_DA__
#define CA_THRESHOLD 20
#define RX_THRESHOLD 150
#define TX_THRESHOLD 20
#define PERCENTAGE_THRESHOLD 50
#define PRINT_WIN_SIZE 0
U8* print_Impedence_LOG_type(U8 print_type)
{
	switch (print_type)
	{
		case 0: return "DRVP";
		case 1: return "DRVN";
		case 2: return "ODTP";
		case 3: return "ODTN";
		default: return "ERROR";
	}
}
void vPrintPinInfoResult(DRAMC_CTX_T *p)
{
	U8 u1CHIdx, u1RankIdx, u1CAIdx, u1ByteIdx, u1ByteIdx_DQ, u1BitIdx, u1BitIdx_DQ, u1FreqRegionIdx, u1ImpIdx;
	U8 u1PinError=0;
	mcSHOW_DBG_MSG(("\n\n[Pin Info Summary] Freqency %d\n", p->frequency));
	for (u1FreqRegionIdx=0;u1FreqRegionIdx<2/*IMP_VREF_MAX*/;u1FreqRegionIdx++)
	{
		for (u1ImpIdx=0;u1ImpIdx<IMP_DRV_MAX;u1ImpIdx++)
		{
			mcSHOW_DBG_MSG(("IMP %s type:%s %s\n", u1FreqRegionIdx?"Region1":"Region0", print_Impedence_LOG_type(u1ImpIdx), ((PINInfo_flashtool.IMP_ERR_FLAG>>(u1FreqRegionIdx*4+u1ImpIdx)&0x1)?"ERROR":"PASS")));
		}
	}
	{
		for(u1CHIdx=0; u1CHIdx<p->support_channel_num; u1CHIdx++)
		{
			for(u1RankIdx=0; u1RankIdx<p->support_rank_num; u1RankIdx++)
			{
				mcSHOW_DBG_MSG(("CH %d, Rank %d\n", u1CHIdx, u1RankIdx));
				for (u1CAIdx =0; u1CAIdx <CATRAINING_NUM_LP4; u1CAIdx++)
				{
					#if 1//Transfer to Percentage
					PINInfo_flashtool.CA_WIN_SIZE[u1CHIdx][u1RankIdx][u1CAIdx]= (PINInfo_flashtool.CA_WIN_SIZE[u1CHIdx][u1RankIdx][u1CAIdx]* 100 + 63) /64;
					if ((PINInfo_flashtool.CA_WIN_SIZE[u1CHIdx][u1RankIdx][u1CAIdx]==0)||(PINInfo_flashtool.CA_WIN_SIZE[u1CHIdx][u1RankIdx][u1CAIdx]<=PERCENTAGE_THRESHOLD))
					#else
					if ((PINInfo_flashtool.CA_WIN_SIZE[u1CHIdx][u1RankIdx][u1CAIdx]==0)||(PINInfo_flashtool.CA_WIN_SIZE[u1CHIdx][u1RankIdx][u1CAIdx]<=CA_THRESHOLD))
					#endif
					{
						PINInfo_flashtool.CA_ERR_FLAG[u1CHIdx][u1RankIdx] |= (1<<u1CAIdx);
						PINInfo_flashtool.TOTAL_ERR |= (0x1<<(u1CHIdx*4+u1RankIdx*2));
					}
					mcSHOW_DBG_MSG(("CA %d: %s ", u1CAIdx, ((PINInfo_flashtool.CA_ERR_FLAG[u1CHIdx][u1RankIdx]>>u1CAIdx)&0x1)?"ERROR":"PASS"));
					#if PRINT_WIN_SIZE
					mcSHOW_DBG_MSG(("(WIN_SIZE: %d %% )", (PINInfo_flashtool.CA_WIN_SIZE[u1CHIdx][u1RankIdx][u1CAIdx])));
					#endif
					mcSHOW_DBG_MSG(("\n"));
				}
				for (u1BitIdx =0; u1BitIdx <DQ_DATA_WIDTH_LP4; u1BitIdx++)
				{
					u1ByteIdx = (u1BitIdx>=8?1:0);
					u1BitIdx_DQ = uiLPDDR4_O1_Mapping_POP[p->channel][u1BitIdx];
					u1ByteIdx_DQ = (u1BitIdx_DQ>=8?1:0);
				#if 1//Transfer to Percentage
					PINInfo_flashtool.DQ_RX_WIN_SIZE[u1CHIdx][u1RankIdx][u1BitIdx] = ((PINInfo_flashtool.DQ_RX_WIN_SIZE[u1CHIdx][u1RankIdx][u1BitIdx]* gHQALOG_RX_delay_cell_ps_075V * p->frequency * 2)+ (1000000 - 1)) / 1000000;
					if (PINInfo_flashtool.DQ_RX_WIN_SIZE[u1CHIdx][u1RankIdx][u1BitIdx]<=PERCENTAGE_THRESHOLD)
				#else
					if ((PINInfo_flashtool.DQ_RX_WIN_SIZE[u1CHIdx][u1RankIdx][u1BitIdx]==0)||(PINInfo_flashtool.DQ_RX_WIN_SIZE[u1CHIdx][u1RankIdx][u1BitIdx]<=RX_THRESHOLD)\
						 ||(PINInfo_flashtool.DQ_TX_WIN_SIZE[u1CHIdx][u1RankIdx][u1BitIdx]==0)||(PINInfo_flashtool.DQ_TX_WIN_SIZE[u1CHIdx][u1RankIdx][u1BitIdx]<=TX_THRESHOLD))
				#endif
					{
						PINInfo_flashtool.DQ_RX_ERR_FLAG[u1CHIdx][u1RankIdx][u1ByteIdx] |= (1<<(u1BitIdx-(u1ByteIdx==1?8:0)));
						PINInfo_flashtool.DRAM_PIN_RX_ERR_FLAG[u1CHIdx][u1RankIdx][u1ByteIdx_DQ] |= (1<<(u1BitIdx_DQ-(u1ByteIdx_DQ==1?8:0)));
						PINInfo_flashtool.TOTAL_ERR |= (0x1<<(u1CHIdx*4+u1RankIdx*2+1));
					}
				#if 1//Transfer to Percentage
					PINInfo_flashtool.DQ_TX_WIN_SIZE[u1CHIdx][u1RankIdx][u1BitIdx] = (PINInfo_flashtool.DQ_TX_WIN_SIZE[u1CHIdx][u1RankIdx][u1BitIdx]* 100+ (vGet_DDR_Loop_Mode(p) == DDR800_CLOSE_LOOP? 63: 31)) / (vGet_DDR_Loop_Mode(p) == DDR800_CLOSE_LOOP? 64: 32);
					if (PINInfo_flashtool.DQ_TX_WIN_SIZE[u1CHIdx][u1RankIdx][u1BitIdx]<=PERCENTAGE_THRESHOLD)
				#else
					if ((PINInfo_flashtool.DQ_RX_WIN_SIZE[u1CHIdx][u1RankIdx][u1BitIdx]==0)||(PINInfo_flashtool.DQ_RX_WIN_SIZE[u1CHIdx][u1RankIdx][u1BitIdx]<=RX_THRESHOLD)\
						 ||(PINInfo_flashtool.DQ_TX_WIN_SIZE[u1CHIdx][u1RankIdx][u1BitIdx]==0)||(PINInfo_flashtool.DQ_TX_WIN_SIZE[u1CHIdx][u1RankIdx][u1BitIdx]<=TX_THRESHOLD))
				#endif
					{
						PINInfo_flashtool.DQ_TX_ERR_FLAG[u1CHIdx][u1RankIdx][u1ByteIdx] |= (1<<(u1BitIdx-(u1ByteIdx==1?8:0)));
						PINInfo_flashtool.DRAM_PIN_TX_ERR_FLAG[u1CHIdx][u1RankIdx][u1ByteIdx_DQ] |= (1<<(u1BitIdx_DQ-(u1ByteIdx_DQ==1?8:0)));
						PINInfo_flashtool.TOTAL_ERR |= (0x1<<(u1CHIdx*4+u1RankIdx*2+1));
					}
				}
				for (u1BitIdx_DQ=0; u1BitIdx_DQ<DQ_DATA_WIDTH_LP4; u1BitIdx_DQ++)
				{
					u1ByteIdx_DQ = (u1BitIdx_DQ>=8?1:0);
					mcSHOW_DBG_MSG(("DRAM DQ %d: RX %s, TX %s ", u1BitIdx_DQ, (((PINInfo_flashtool.DRAM_PIN_RX_ERR_FLAG[u1CHIdx][u1RankIdx][u1ByteIdx_DQ]>>(u1BitIdx_DQ-(u1ByteIdx_DQ==1?8:0)))&0x1)?"ERROR":"PASS"),\
																														(((PINInfo_flashtool.DRAM_PIN_TX_ERR_FLAG[u1CHIdx][u1RankIdx][u1ByteIdx_DQ]>>(u1BitIdx_DQ-(u1ByteIdx_DQ==1?8:0)))&0x1)?"ERROR":"PASS")));
					#if PRINT_WIN_SIZE
					mcSHOW_DBG_MSG(("(RX WIN SIZE: %d %%, TX WIN SIZE: %d %% )", PINInfo_flashtool.DQ_RX_WIN_SIZE[u1CHIdx][u1RankIdx][uiLPDDR4_O1_Mapping_POP[u1CHIdx][u1BitIdx_DQ]], PINInfo_flashtool.DQ_TX_WIN_SIZE[u1CHIdx][u1RankIdx][uiLPDDR4_O1_Mapping_POP[u1CHIdx][u1BitIdx_DQ]]));
					#endif
					mcSHOW_DBG_MSG(("\n"));
				}
			}
		}
	}
}
void vGetErrorTypeResult(DRAMC_CTX_T *p)
{
	U8 u1CHIdx, u1CHIdx_EMI, u1RankIdx, u1CAIdx, u1ByteIdx, u1BitIdx, u1FreqRegionIdx, u1ImpIdx;
	mcSHOW_DBG_MSG(("\n[Get Pin Error Type Result]\n"));
	if (PINInfo_flashtool.TOTAL_ERR==0 && PINInfo_flashtool.IMP_ERR_FLAG==0)//ALL PASS
	{
		mcSHOW_DBG_MSG(("ALL PASS\n"));
	}
	if (PINInfo_flashtool.IMP_ERR_FLAG)
	{
		mcSHOW_DBG_MSG(("[CHECK RESULT] FAIL: Impedance calibration fail\n"));
		mcSHOW_DBG_MSG(("Suspect EXTR contact issue\n"));
		mcSHOW_DBG_MSG(("Suspect EXTR related resistor contact issue\n"));
	}
	if ((PINInfo_flashtool.TOTAL_ERR == 0xffff) && (PINInfo_flashtool.WL_ERR_FLAG== 0xff))
	{
		mcSHOW_DBG_MSG(("[CHECK RESULT] FAIL: ALL calibration fail\n"));
		mcSHOW_DBG_MSG(("Suspect RESET_N contact issue\n"));
		mcSHOW_DBG_MSG(("Suspect DRAM Power (VDD1/VDD2/VDDQ) contact issue\n"));
	}
	else
	{
		for (u1CHIdx = 0; u1CHIdx < p->support_channel_num; u1CHIdx++)
		{
			#if (CHANNEL_NUM > 2)
			if(u1CHIdx == CHANNEL_B)
				u1CHIdx_EMI = CHANNEL_C;
			else if(u1CHIdx == CHANNEL_C)
				u1CHIdx_EMI = CHANNEL_B;
			else //CHANNEL_A,CHANNEL_D
			#endif
				u1CHIdx_EMI = u1CHIdx;
			if ((PINInfo_flashtool.TOTAL_ERR>>(u1CHIdx*4) & 0xf) == 0xf)
			{
				mcSHOW_DBG_MSG(("[CHECK RESULT] FAIL: CH%d all calibration fail\n",u1CHIdx));
				mcSHOW_DBG_MSG(("Suspect EMI%d_CK_T contact issue\n",u1CHIdx_EMI));
				mcSHOW_DBG_MSG(("Suspect EMI%d_CK_C contact issue\n",u1CHIdx_EMI));
				for (u1CAIdx =0; u1CAIdx <CATRAINING_NUM_LP4; u1CAIdx++)
				{
					mcSHOW_DBG_MSG(("Suspect EMI%d_CA%d contact issue\n",u1CHIdx_EMI,u1CAIdx));
				}
			}
			else
			{
				for(u1RankIdx = 0; u1RankIdx < p->support_rank_num; u1RankIdx++)
				{
					if ((((PINInfo_flashtool.TOTAL_ERR>>(u1CHIdx*4+u1RankIdx*2)) & 0x3)==0x3) && \
						 (PINInfo_flashtool.DRAM_PIN_RX_ERR_FLAG[u1CHIdx][u1RankIdx][BYTE_0] == 0xff) && \
						 (PINInfo_flashtool.DRAM_PIN_RX_ERR_FLAG[u1CHIdx][u1RankIdx][BYTE_1] == 0xff)&& \
						 (PINInfo_flashtool.DRAM_PIN_TX_ERR_FLAG[u1CHIdx][u1RankIdx][BYTE_0] == 0xff) && \
						 (PINInfo_flashtool.DRAM_PIN_TX_ERR_FLAG[u1CHIdx][u1RankIdx][BYTE_1] == 0xff))
					{
						mcSHOW_DBG_MSG(("[CHECK RESULT] FAIL: CH%d RK%d all calibration fail\n",u1CHIdx,u1RankIdx));
						mcSHOW_DBG_MSG(("Suspect EMI%d_CKE_%d contact issue\n",u1CHIdx_EMI,u1RankIdx));
						mcSHOW_DBG_MSG(("Suspect EMI%d_CS_%d contact issue\n",u1CHIdx_EMI,u1RankIdx));
					}
					else
					{
						for (u1ByteIdx = 0; u1ByteIdx < DQS_NUMBER_LP4; u1ByteIdx++)
						{
							if((PINInfo_flashtool.DRAM_PIN_RX_ERR_FLAG[u1CHIdx][u1RankIdx][u1ByteIdx] == 0xff) &&\
									  (PINInfo_flashtool.DRAM_PIN_TX_ERR_FLAG[u1CHIdx][u1RankIdx][u1ByteIdx] == 0xff))
							{
								mcSHOW_DBG_MSG(("[CHECK RESULT] FAIL: CH%d RK%d Byte%d WL/Read/Write calibration fail\n",u1CHIdx,u1RankIdx,u1ByteIdx));
								mcSHOW_DBG_MSG(("Suspect EMI%d_DQS%d_T contact issue\n",u1CHIdx_EMI,u1ByteIdx));
								mcSHOW_DBG_MSG(("Suspect EMI%d_DQS%d_C contact issue\n",u1CHIdx_EMI,u1ByteIdx));
							}
							else if (PINInfo_flashtool.DRAM_PIN_RX_ERR_FLAG[u1CHIdx][u1RankIdx][u1ByteIdx]&&\
									   PINInfo_flashtool.DRAM_PIN_TX_ERR_FLAG[u1CHIdx][u1RankIdx][u1ByteIdx])
							{
								for (u1BitIdx = 0; u1BitIdx < DQS_BIT_NUMBER; u1BitIdx++)
								{
									if (((PINInfo_flashtool.DRAM_PIN_RX_ERR_FLAG[u1CHIdx][u1RankIdx][u1ByteIdx]>>u1BitIdx)&0x1)&&\
										 ((PINInfo_flashtool.DRAM_PIN_TX_ERR_FLAG[u1CHIdx][u1RankIdx][u1ByteIdx]>>u1BitIdx)&0x1))
									{
											mcSHOW_DBG_MSG(("[CHECK RESULT] FAIL: CH%d RK%d DRAM DQ%d Read/Write fail\n",u1CHIdx,u1RankIdx,u1ByteIdx*8+u1BitIdx));
											mcSHOW_DBG_MSG(("Suspect EMI%d_DQ%d contact issue\n",u1CHIdx_EMI,u1ByteIdx*8+u1BitIdx));
									}
								}
							}
							else if((PINInfo_flashtool.DRAM_PIN_RX_ERR_FLAG[u1CHIdx][u1RankIdx][u1ByteIdx] == 0xff) ||\
									  (PINInfo_flashtool.DRAM_PIN_TX_ERR_FLAG[u1CHIdx][u1RankIdx][u1ByteIdx] == 0xff))
							{
								mcSHOW_DBG_MSG(("[CHECK RESULT] FAIL: CH%d RK%d Byte%d Suspect other special contact or calibration issue\n",u1CHIdx_EMI,u1RankIdx,u1ByteIdx));
							}
						}
					}
				}
			}
		}
	}
	mcSHOW_DBG_MSG(("\n"));
	return;
}
#endif
void vInitGlobalVariablesByCondition(DRAMC_CTX_T *p)
{
	U8 u1CHIdx, u1RankIdx, u1FSPIdx;

	u1MR01Value[FSP_0] = 0x26;
	u1MR01Value[FSP_1] = 0x56;

	u1MR03Value[FSP_0] = 0x31; //Set write post-amble as 0.5 tck
	u1MR03Value[FSP_1] = 0x31; //Set write post-amble as 0.5 tck
#ifndef ENABLE_POST_PACKAGE_REPAIR
	u1MR03Value[FSP_0] |= 0x4; //MR3 OP[2]=1 for PPR protection enabled
	u1MR03Value[FSP_1] |= 0x4; //MR3 OP[2]=1 for PPR protection enabled
#endif
#if ENABLE_WRITE_POST_AMBLE_1_POINT_5_TCK
	u1MR03Value[FSP_1] |= 0x2; //MR3 OP[1]=1 for Set write post-amble as 1.5 tck, support after Eig_er E2
#endif
	u1MR04Value[RANK_0] = 0x3;
	u1MR04Value[RANK_1] = 0x3;

	// @Darren, for LP4Y single-end mode
	u1MR21Value[FSP_0] = 0x0;
	u1MR21Value[FSP_1] = 0x0;
	u1MR51Value[FSP_0] = 0x0;
	u1MR51Value[FSP_1] = 0x0;

	for (u1FSPIdx = 0; u1FSPIdx < p->support_fsp_num; u1FSPIdx++)
	{
		u1MR02Value[u1FSPIdx] = 0x1a;
	}

	for (u1CHIdx = 0; u1CHIdx < CHANNEL_NUM; u1CHIdx++)
		for (u1RankIdx = 0; u1RankIdx < RANK_MAX; u1RankIdx++)
			for (u1FSPIdx = 0; u1FSPIdx < p->support_fsp_num; u1FSPIdx++)
			{
				// MR14 default value, LP4 default 0x4d, LP4X 0x5d
				u1MR14Value[u1CHIdx][u1RankIdx][u1FSPIdx] = (u1FSPIdx == FSP_0)? 0x5d: 0x18;  //0x18: customize for Eig_er
				#if FSP1_CLKCA_TERM
				u1MR12Value[u1CHIdx][u1RankIdx][u1FSPIdx] = (u1FSPIdx == FSP_0)? 0x5d: 0x1b;
				#else
				u1MR12Value[u1CHIdx][u1RankIdx][u1FSPIdx] = 0x5d;
				#endif
				#if MRW_CHECK_ONLY
				for (u1MRIdx = 0; u1MRIdx < MR_NUM; u1MRIdx++)
					u2MRRecord[u1CHIdx][u1RankIdx][u1FSPIdx][u1MRIdx] = 0xffff;
				#endif
			}

	memset(gu2RX_DQS_Duty_Offset, 0, sizeof(gu2RX_DQS_Duty_Offset));
}

const U8 uiLPDDR4_CA_DRAM_Pinmux[PINMUX_MAX][CHANNEL_NUM][6] =
{
	{
	// for DSC
		//CH-A
		{
			1, 4, 5, 3, 2, 0
		},

	#if (CHANNEL_NUM>1)
		//CH-B
		{
			3, 5, 0, 2, 4, 1
		},
	#endif
	#if (CHANNEL_NUM>2)
		//CH-C
		{
			5, 0, 4, 3, 1, 2
		},
		//CH-D
		{
			2, 5, 3, 0, 4, 1
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
			2, 4, 3, 5, 1, 0
		},

	#if (CHANNEL_NUM>1)
		//CH-B
		{
			4, 5, 2, 0, 3, 1
		},
	#endif
	#if (CHANNEL_NUM>2)
		//CH-C
		{
			5, 4, 0, 2, 1, 3
		},
		//CH-D
		{
			3, 5, 2, 4, 0, 1
		},
	#endif
	}
};

//O1 DRAM->APHY
const U8 uiLPDDR4_O1_DRAM_Pinmux[PINMUX_MAX][CHANNEL_NUM][16] =
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
	}
};

//CA APHY->DRAM
#if (CA_PER_BIT_DELAY_CELL || PINMUX_AUTO_TEST_PER_BIT_CA)
const U8 uiLPDDR5_CA_Mapping_POP[CHANNEL_NUM][7] =
{
	//CH-A
	{
		0, 1, 2, 3, 4, 5, 6
	},

#if (CHANNEL_NUM>1)
	//CH-B
	{
		0, 4, 2, 3, 1, 5, 6
	}
#endif
};

U8 uiLPDDR4_CA_Mapping_POP[CHANNEL_NUM][6] =
{
	//CH-A
	{
		5, 4, 0, 2, 1, 3
	},

#if (CHANNEL_NUM>1)
	//CH-B
	{
		3, 5, 2, 4, 0, 1
	},
#endif
#if (CHANNEL_NUM>2)
	//CH-C
	{
		5, 4, 0, 2, 1, 3
	},
	//CH-D
	{
		3, 5, 2, 4, 0, 1
	},
#endif
};
#endif

#if (__LP5_COMBO__)
const U8 uiLPDDR5_O1_Mapping_POP[CHANNEL_NUM][16] =
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

//O1 DRAM->APHY
U8 uiLPDDR4_O1_Mapping_POP[CHANNEL_NUM][16] =
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

void vBeforeCalibration(DRAMC_CTX_T *p)
{
#if (__LP5_COMBO__ == TRUE)
	if (TRUE == is_lp5_family(p))
	{
		DramcMRInit_LP5(p);
	}
	else
#endif
	{
		//DramcMRInit_LP4(p);
	}

#if SIMULATION_RX_DVS || ENABLE_RX_TRACKING
	DramcRxInputDelayTrackingInit_byFreq(p);
#endif

	DramcHWGatingOnOff(p, 0); //disable gating tracking

	CKEFixOnOff(p, CKE_WRITE_TO_ALL_RANK, CKE_FIXON, CKE_WRITE_TO_ALL_CHANNEL); //Let CLK always on during calibration

#if ENABLE_TMRRI_NEW_MODE
	SetCKE2RankIndependent(p); //CKE should be controlled independently
#endif

	//WDBI-OFF
	vIO32WriteFldAlign_All(DRAMC_REG_SHU_TX_SET0, 0x0, SHU_TX_SET0_DBIWR);

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

	vIO32WriteFldMulti_All(DDRPHY_REG_MISC_SHU_IMPEDAMCE_UPD_DIS1, P_Fld(u1DisImpHw, MISC_SHU_IMPEDAMCE_UPD_DIS1_ODTN_UPD_DIS)
																	| P_Fld(u1DisImpHw, MISC_SHU_IMPEDAMCE_UPD_DIS1_DRVN_UPD_DIS)
																	| P_Fld(u1DisImpHw, MISC_SHU_IMPEDAMCE_UPD_DIS1_DRVP_UPD_DIS)
																	| P_Fld(u1DisImpHw, MISC_SHU_IMPEDAMCE_UPD_DIS1_WCK_ODTN_UPD_DIS)
																	| P_Fld(u1DisImpHw, MISC_SHU_IMPEDAMCE_UPD_DIS1_WCK_DRVN_UPD_DIS)
																	| P_Fld(u1DisImpHw, MISC_SHU_IMPEDAMCE_UPD_DIS1_WCK_DRVP_UPD_DIS)
																	| P_Fld(u1DisImpHw, MISC_SHU_IMPEDAMCE_UPD_DIS1_DQ_ODTN_UPD_DIS)
																	| P_Fld(u1DisImpHw, MISC_SHU_IMPEDAMCE_UPD_DIS1_DQ_DRVN_UPD_DIS)
																	| P_Fld(u1DisImpHw, MISC_SHU_IMPEDAMCE_UPD_DIS1_DQ_DRVP_UPD_DIS)
																	| P_Fld(u1DisImpHw, MISC_SHU_IMPEDAMCE_UPD_DIS1_DQS_ODTN_UPD_DIS)
																	| P_Fld(u1DisImpHw, MISC_SHU_IMPEDAMCE_UPD_DIS1_DQS_DRVN_UPD_DIS)
																	| P_Fld(u1DisImpHw, MISC_SHU_IMPEDAMCE_UPD_DIS1_DQS_DRVP_UPD_DIS)
																	| P_Fld(1, MISC_SHU_IMPEDAMCE_UPD_DIS1_WCK_DRVP_UPD_DIS)
																	| P_Fld(1, MISC_SHU_IMPEDAMCE_UPD_DIS1_WCK_DRVN_UPD_DIS)
																	| P_Fld(1, MISC_SHU_IMPEDAMCE_UPD_DIS1_WCK_ODTN_UPD_DIS));

	vIO32WriteFldAlign_All(DDRPHY_REG_SHU_MISC_IMPCAL1, (u1DisImpHw? 0x0:0x40), SHU_MISC_IMPCAL1_IMPCALCNT);

	vIO32WriteFldAlign_All(DDRPHY_REG_SHU_MISC_DRVING1, u1DisImpHw, SHU_MISC_DRVING1_DIS_IMPCAL_HW);
	vIO32WriteFldAlign_All(DDRPHY_REG_SHU_MISC_DRVING1, u1DisImpHw, SHU_MISC_DRVING1_DIS_IMP_ODTN_TRACK);
	vIO32WriteFldAlign_All(DDRPHY_REG_SHU_MISC_DRVING2, u1DisImpHw, SHU_MISC_DRVING2_DIS_IMPCAL_ODT_EN);
	vIO32WriteFldAlign_All(DDRPHY_REG_SHU_CA_CMD12, u1DisImpHw, SHU_CA_CMD12_RG_RIMP_UNTERM_EN);
#endif

	vIO32WriteFldMulti_All(DDRPHY_REG_MISC_CLK_CTRL, P_Fld(0, MISC_CLK_CTRL_DVFS_CLK_MEM_SEL)
													| P_Fld(0, MISC_CLK_CTRL_DVFS_MEM_CK_MUX_UPDATE_EN));


	vIO32WriteFldMulti_All(DRAMC_REG_SHU_ZQ_SET0,
			P_Fld(0x1ff, SHU_ZQ_SET0_ZQCSCNT) | //Every refresh number to issue ZQCS commands, only for DDR3/LPDDR2/LPDDR3/LPDDR4
			P_Fld(0x1b, SHU_ZQ_SET0_TZQLAT));

	if (p->support_channel_num == CHANNEL_SINGLE)
	{
		//single channel, ZQCSDUAL=0, ZQCSMASK=0
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_ZQ_SET0), P_Fld(0, ZQ_SET0_ZQCSDUAL) | P_Fld(0x0, ZQ_SET0_ZQCSMASK));
	}
	else if (p->support_channel_num == CHANNEL_DUAL)
	{
		// HW ZQ command is channel interleaving since 2 channel share the same ZQ pin.
		#ifdef ZQCS_ENABLE_LP4
		// dual channel, ZQCSDUAL =1, and CHA ZQCSMASK=0, CHB ZQCSMASK=1

		vIO32WriteFldMulti_All(DRAMC_REG_ZQ_SET0, P_Fld(1, ZQ_SET0_ZQCSDUAL) |
											   P_Fld(0, ZQ_SET0_ZQCSMASK_OPT) |
											   P_Fld(0, ZQ_SET0_ZQMASK_CGAR) |
											   P_Fld(0, ZQ_SET0_ZQCS_MASK_SEL_CGAR));

		// DRAMC CHA(CHN0):ZQCSMASK=1, DRAMC CHB(CHN1):ZQCSMASK=0.
		// ZQCSMASK setting: (Ch A, Ch B) = (1,0) or (0,1)
		// if CHA.ZQCSMASK=1, and then set CHA.ZQCALDISB=1 first, else set CHB.ZQCALDISB=1 first
		vIO32WriteFldAlign(DRAMC_REG_ZQ_SET0 + (CHANNEL_A << POS_BANK_NUM), 1, ZQ_SET0_ZQCSMASK);
		vIO32WriteFldAlign(DRAMC_REG_ZQ_SET0 + SHIFT_TO_CHB_ADDR, 0, ZQ_SET0_ZQCSMASK);

		// DRAMC CHA(CHN0):ZQ_SET0_ZQCS_MASK_SEL=0, DRAMC CHB(CHN1):ZQ_SET0_ZQCS_MASK_SEL=0.
		vIO32WriteFldAlign_All(DRAMC_REG_ZQ_SET0, 0, ZQ_SET0_ZQCS_MASK_SEL);
		#endif
	}
#if (CHANNEL_NUM > 2)
	else if (p->support_channel_num == CHANNEL_FOURTH)
	{
		// HW ZQ command is channel interleaving since 2 channel share the same ZQ pin.
		#ifdef ZQCS_ENABLE_LP4
		// dual channel, ZQCSDUAL =1, and CHA ZQCSMASK=0, CHB ZQCSMASK=1

		vIO32WriteFldMulti_All(DRAMC_REG_ZQ_SET0, P_Fld(1, ZQ_SET0_ZQCSDUAL) |
											   P_Fld(0, ZQ_SET0_ZQCALL) |
											   P_Fld(0, ZQ_SET0_ZQ_SRF_OPT) |
											   P_Fld(0, ZQ_SET0_ZQCSMASK_OPT) |
											   P_Fld(0, ZQ_SET0_ZQMASK_CGAR) |
											   P_Fld(0, ZQ_SET0_ZQCS_MASK_SEL_CGAR));

		// DRAMC CHA(CHN0):ZQCSMASK=1, DRAMC CHB(CHN1):ZQCSMASK=0.
		// ZQCSMASK setting: (Ch A, Ch C) = (1,0) or (0,1), (Ch B, Ch D) = (1,0) or (0,1)
		// if CHA.ZQCSMASK=1, and then set CHA.ZQCALDISB=1 first, else set CHB.ZQCALDISB=1 first
	#if fcFOR_CHIP_ID == fcPetrus
		vIO32WriteFldAlign(DRAMC_REG_ZQ_SET0 + (CHANNEL_A << POS_BANK_NUM), 1, ZQ_SET0_ZQCSMASK);
		vIO32WriteFldAlign(DRAMC_REG_ZQ_SET0 + (CHANNEL_B << POS_BANK_NUM), 0, ZQ_SET0_ZQCSMASK);
		vIO32WriteFldAlign(DRAMC_REG_ZQ_SET0 + (CHANNEL_C << POS_BANK_NUM), 0, ZQ_SET0_ZQCSMASK);
		vIO32WriteFldAlign(DRAMC_REG_ZQ_SET0 + (CHANNEL_D << POS_BANK_NUM), 1, ZQ_SET0_ZQCSMASK);
	#endif

		// DRAMC CHA(CHN0):ZQ_SET0_ZQCS_MASK_SEL=0, DRAMC CHB(CHN1):ZQ_SET0_ZQCS_MASK_SEL=0.
		vIO32WriteFldAlign_All(DRAMC_REG_ZQ_SET0, 0, ZQ_SET0_ZQCS_MASK_SEL);
		#endif
	}
#endif

	// Set 0 to be able to adjust TX DQS/DQ/DQM PI during calibration, for new cross rank mode.
	vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B0_DQ2, 0, SHU_B0_DQ2_RG_ARPI_OFFSET_LAT_EN_B0);
	vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B1_DQ2, 0, SHU_B1_DQ2_RG_ARPI_OFFSET_LAT_EN_B1);

#if ENABLE_PA_IMPRO_FOR_TX_AUTOK
	vIO32WriteFldAlign_All(DRAMC_REG_DCM_SUB_CTRL, 0x0, DCM_SUB_CTRL_SUBCLK_CTRL_TX_AUTOK);
#endif
	// ARPI_DQ SW mode mux, TX DQ use 1: PHY Reg 0: DRAMC Reg
	#if ENABLE_PA_IMPRO_FOR_TX_TRACKING
	vIO32WriteFldAlign_All(DRAMC_REG_DCM_SUB_CTRL, 0, DCM_SUB_CTRL_SUBCLK_CTRL_TX_TRACKING);
	#endif
	//Darren-vIO32WriteFldAlign_All(DDRPHY_REG_MISC_CTRL1, 1, MISC_CTRL1_R_DMARPIDQ_SW); @Darren, remove to LP4_UpdateInitialSettings
	//Disable HW MR18/19 to prevent fail case when doing SW MR18/19 in DQSOSCAuto
	vIO32WriteFldAlign_All(DRAMC_REG_DQSOSCR, 0x1, DQSOSCR_DQSOSCRDIS);

	vIO32WriteFldAlign_All(DRAMC_REG_REFCTRL0, 0x1, REFCTRL0_REFDIS); //disable refresh

	vIO32WriteFldAlign_All(DRAMC_REG_SHU_MATYPE, u1MaType, SHU_MATYPE_MATYPE);

	TX_Path_Algorithm(p);
}

void vAfterCalibration(DRAMC_CTX_T *p)
{
#if ENABLE_READ_DBI
	EnableDRAMModeRegReadDBIAfterCalibration(p);
#endif

#if ENABLE_WRITE_DBI
	EnableDRAMModeRegWriteDBIAfterCalibration(p);
#endif

	SetMr13VrcgToNormalOperation(p);// Set VRCG{MR13[3]} to 0
	CKEFixOnOff(p, CKE_WRITE_TO_ALL_RANK, CKE_DYNAMIC, CKE_WRITE_TO_ALL_CHANNEL); //After CKE FIX on/off, CKE should be returned to dynamic (control by HW)

	vIO32WriteFldAlign_All(DRAMC_REG_DUMMY_RD, p->support_rank_num, DUMMY_RD_RANK_NUM);

#if FOR_DV_SIMULATION_USED == 1
	cal_sv_rand_args_t *psra = get_psra();

	if (psra) {
		u1MR03Value[p->dram_fsp] = psra->mr3_value;
	}
#endif

	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_CG_CTRL7, 0, MISC_CG_CTRL7_CK_BFE_DCM_EN);
	vIO32WriteFldAlign_All(DRAMC_REG_TEST2_A4, 4, TEST2_A4_TESTAGENTRKSEL); // Rank selection is controlled by Test Agent
	vIO32WriteFldAlign_All(DRAMC_REG_TEST2_A2, 0x20, TEST2_A2_TEST2_OFF); //@Chris, MP setting for runtime TA2 Length
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_DUTYSCAN1, 0, MISC_DUTYSCAN1_DQSERRCNT_DIS);
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_CTRL1, 0, MISC_CTRL1_R_DMSTBENCMP_RK_OPT);
}

static void O1PathOnOff(DRAMC_CTX_T *p, U8 u1OnOff)
{
	#if 0//O1_SETTING_RESTORE
	const U32 u4O1RegBackupAddress[] =
	{
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_VREF)),
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_VREF)),
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_PHY_VREF_SEL)),
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_PHY_VREF_SEL)),
		(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ5)),
		(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ5))
	};
	#endif

	U8 u1VrefSel;

	if (u1OnOff == ON)
	{
		// These RG will be restored when leaving each calibration flow
		// -------------------------------------------------------
		// VREF_UNTERM_EN
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_VREF), 1, SHU_B0_VREF_RG_RX_ARDQ_VREF_UNTERM_EN_B0);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_VREF), 1, SHU_B1_VREF_RG_RX_ARDQ_VREF_UNTERM_EN_B1);

		#if (__LP5_COMBO__ == TRUE)
		if (p->dram_type==TYPE_LPDDR5)
			u1VrefSel = 0x37;//unterm LP5
		else
		#endif
			u1VrefSel = 0x37;//unterm LP4

		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_PHY_VREF_SEL),
					P_Fld(u1VrefSel, SHU_B0_PHY_VREF_SEL_RG_RX_ARDQ_VREF_SEL_LB_B0) |
					P_Fld(u1VrefSel, SHU_B0_PHY_VREF_SEL_RG_RX_ARDQ_VREF_SEL_UB_B0));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_PHY_VREF_SEL),
					P_Fld(u1VrefSel, SHU_B1_PHY_VREF_SEL_RG_RX_ARDQ_VREF_SEL_LB_B1) |
					P_Fld(u1VrefSel, SHU_B1_PHY_VREF_SEL_RG_RX_ARDQ_VREF_SEL_UB_B1));
	}

	// DQ_O1 enable/release
	// -------------------------------------------------------
	// Actually this RG naming is O1_EN in APHY
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ6), u1OnOff, B0_DQ6_RG_RX_ARDQ_O1_SEL_B0);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ6), u1OnOff, B1_DQ6_RG_RX_ARDQ_O1_SEL_B1);

	// DQ_IN_BUFF_EN
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ3),
						P_Fld(u1OnOff, B0_DQ3_RG_RX_ARDQ_IN_BUFF_EN_B0) |
						P_Fld(u1OnOff, B0_DQ3_RG_RX_ARDQS0_IN_BUFF_EN_B0));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ3),
						P_Fld(u1OnOff, B1_DQ3_RG_RX_ARDQ_IN_BUFF_EN_B1) |
						P_Fld(u1OnOff, B1_DQ3_RG_RX_ARDQS0_IN_BUFF_EN_B1));

	// DQ_BUFF_EN_SEL
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B0_PHY3), u1OnOff, B0_PHY3_RG_RX_ARDQ_BUFF_EN_SEL_B0);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B1_PHY3), u1OnOff, B1_PHY3_RG_RX_ARDQ_BUFF_EN_SEL_B1);

	// Gating always ON
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_RX_IN_GATE_EN_CTRL),(u1OnOff << 1) | u1OnOff, MISC_RX_IN_GATE_EN_CTRL_FIX_IN_GATE_EN);

	mcDELAY_US(1);
}

/*
 * set_cbt_intv -- set interval related rg according to speed.
 *
 * TODO, move these to ACTimingTable ????!!!
 */

struct cbt_intv {
	DRAM_PLL_FREQ_SEL_T freq_sel;
	DIV_MODE_T divmode;
	u8 tcmdo1lat;
	u8 catrain_intv;
	u8 new_cbt_pat_intv;
	u8 wlev_dqspat_lat;
};

static void set_cbt_intv_rg(DRAMC_CTX_T *p, struct cbt_intv *pintv)
{
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL1),
		P_Fld(pintv->tcmdo1lat, CBT_WLEV_CTRL1_TCMDO1LAT) |
		P_Fld(pintv->catrain_intv, CBT_WLEV_CTRL1_CATRAIN_INTV));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL5),
		P_Fld(pintv->new_cbt_pat_intv, CBT_WLEV_CTRL5_NEW_CBT_PAT_INTV));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL0),
		P_Fld(pintv->wlev_dqspat_lat, CBT_WLEV_CTRL0_WLEV_DQSPAT_LAT));
}

static struct cbt_intv *lookup_cbt_intv(struct cbt_intv *intv, int cnt,
		DRAM_PLL_FREQ_SEL_T fsel, DIV_MODE_T dmode)
{
	struct cbt_intv *pintv = NULL;
	int i;

	for (i = 0; i < cnt; i++) {
		if (intv[i].freq_sel == fsel && intv[i].divmode == dmode) {
			pintv = &intv[i];
			break;
		}
	}

	return pintv;
}

static void set_cbt_wlev_intv_lp4(DRAMC_CTX_T *p)
{
	struct cbt_intv intv[] = {
		{
			LP4_DDR4266,
			DIV8_MODE,
			17, /*tcmdo1lat*/
			14, /* catrain_intv */
			19, /* new_cbt_pat_intv */
			19, /* wlev_dqspat_lat */
		}, {
			LP4_DDR3733,
			DIV8_MODE,
			16, /*tcmdo1lat*/
			13, /* catrain_intv */
			18, /* new_cbt_pat_intv */
			18, /* wlev_dqspat_lat */
		}, {
			LP4_DDR3200,
			DIV8_MODE,
			14, /*tcmdo1lat*/
			11, /* catrain_intv */
			16, /* new_cbt_pat_intv */
			16, /* wlev_dqspat_lat */
		}, {
			LP4_DDR2667,
			DIV8_MODE,
			13, /*tcmdo1lat*/
			10, /* catrain_intv */
			15, /* new_cbt_pat_intv */
			15, /* wlev_dqspat_lat */
		}, {
			LP4_DDR2400,
			DIV8_MODE,
			12, /*tcmdo1lat*/
			9, /* catrain_intv */
			14, /* new_cbt_pat_intv */
			14, /* wlev_dqspat_lat */
		}, {
			LP4_DDR1866,
			DIV8_MODE,
			11, /*tcmdo1lat*/
			9, /* catrain_intv */
			13, /* new_cbt_pat_intv */
			13, /* wlev_dqspat_lat */
		}, {
			LP4_DDR1600,
			DIV8_MODE,
			10, /*tcmdo1lat*/
			8, /* catrain_intv */
			12, /* new_cbt_pat_intv */
			12, /* wlev_dqspat_lat */
		}, {
			LP4_DDR1200,
			DIV8_MODE,
			9, /*tcmdo1lat*/
			8, /* catrain_intv */
			11, /* new_cbt_pat_intv */
			11, /* wlev_dqspat_lat */
		}, {
			LP4_DDR800,
			DIV8_MODE,
			8, /*tcmdo1lat*/
			8, /* catrain_intv */
			10, /* new_cbt_pat_intv */
			10, /* wlev_dqspat_lat */
		}, {
			LP4_DDR1600,
			DIV4_MODE,
			16, /*tcmdo1lat*/
			13, /* catrain_intv */
			16, /* new_cbt_pat_intv */
			16, /* wlev_dqspat_lat */
		}, {
			LP4_DDR1200,
			DIV4_MODE,
			14, /*tcmdo1lat*/
			13, /* catrain_intv */
			14, /* new_cbt_pat_intv */
			14, /* wlev_dqspat_lat */
		}, {
			LP4_DDR800,
			DIV4_MODE,
			12, /*tcmdo1lat*/
			13, /* catrain_intv */
			12, /* new_cbt_pat_intv */
			12, /* wlev_dqspat_lat */
		}, {
			LP4_DDR400,
			DIV4_MODE,
			12, /*tcmdo1lat*/
			13, /* catrain_intv */
			12, /* new_cbt_pat_intv */
			12, /* wlev_dqspat_lat */
		},
	};

	struct cbt_intv *pintv;

	pintv = lookup_cbt_intv(intv, ARRAY_SIZE(intv),
			p->freq_sel, vGet_Div_Mode(p));
	if (!pintv) {
		mcSHOW_DBG_MSG(("not found entry!\n"));
		return;
	}

	set_cbt_intv_rg(p, pintv);
}

#if __LP5_COMBO__
static void set_cbt_wlev_intv_lp5(DRAMC_CTX_T *p)
{
	struct cbt_intv intv[] = {
		{
			LP5_DDR6400,
			UNKNOWN_MODE,
			15, /*tcmdo1lat*/
			15, /* catrain_intv */
			17, /* new_cbt_pat_intv */
			17, /* wlev_dqspat_lat */
		}, {
			LP5_DDR6000,
			UNKNOWN_MODE,
			15, /*tcmdo1lat*/
			15, /* catrain_intv */
			17, /* new_cbt_pat_intv */
			17, /* wlev_dqspat_lat */
		}, {
			LP5_DDR5500,
			UNKNOWN_MODE,
			14, /*tcmdo1lat*/
			14, /* catrain_intv */
			16, /* new_cbt_pat_intv */
			16, /* wlev_dqspat_lat */
		}, {
			LP5_DDR4800,
			UNKNOWN_MODE,
			13, /*tcmdo1lat*/
			13, /* catrain_intv */
			15, /* new_cbt_pat_intv */
			15, /* wlev_dqspat_lat */
		}, {
			LP5_DDR4266,
			UNKNOWN_MODE,
			20, /*tcmdo1lat*/
			20, /* catrain_intv */
			22, /* new_cbt_pat_intv */
			20, /* wlev_dqspat_lat */
		}, {
			LP5_DDR3733,
			UNKNOWN_MODE,
			19, /*tcmdo1lat*/
			19, /* catrain_intv */
			21, /* new_cbt_pat_intv */
			19, /* wlev_dqspat_lat */
		}, {
			LP5_DDR3200,
			UNKNOWN_MODE,
			15, /*tcmdo1lat*/
			15, /* catrain_intv */
			17, /* new_cbt_pat_intv */
			17, /* wlev_dqspat_lat */
		}, {
			LP5_DDR2400,
			UNKNOWN_MODE,
			13, /*tcmdo1lat*/
			13, /* catrain_intv */
			15, /* new_cbt_pat_intv */
			15, /* wlev_dqspat_lat */
		}, {
			LP5_DDR1600,
			UNKNOWN_MODE,
			17, /*tcmdo1lat*/
			17, /* catrain_intv */
			19, /* new_cbt_pat_intv */
			17, /* wlev_dqspat_lat */
		}, {
			LP5_DDR1200,
			UNKNOWN_MODE,
			15, /*tcmdo1lat*/
			15, /* catrain_intv */
			17, /* new_cbt_pat_intv */
			15, /* wlev_dqspat_lat */
		}, {
			LP5_DDR800,
			UNKNOWN_MODE,
			13, /*tcmdo1lat*/
			13, /* catrain_intv */
			15, /* new_cbt_pat_intv */
			13, /* wlev_dqspat_lat */
		},
	};

	struct cbt_intv *pintv;

	pintv = lookup_cbt_intv(intv, ARRAY_SIZE(intv), p->freq_sel, UNKNOWN_MODE);
	if (!pintv) {
		mcSHOW_DBG_MSG(("not found entry!\n"));
		return;
	}

	set_cbt_intv_rg(p, pintv);
}
#endif /* __LP5_COMBO__ */

static void set_cbt_wlev_intv(DRAMC_CTX_T *p)
{
#if __LP5_COMBO__
	if (is_lp5_family(p))
		set_cbt_wlev_intv_lp5(p);
	else
#endif
		set_cbt_wlev_intv_lp4(p);
}

#if SIMUILATION_CBT == 1
/* To process LPDDR5 Pinmux */
struct cbt_pinmux {
	u8 dram_dq_b0; /* EMI_B0 is mapped to which DRAMC byte ?? */
	u8 dram_dq_b1;
	u8 dram_dmi_b0; /* EMI_DMI0 is mapped to which DRAMC DMI ?? */
	u8 dram_dmi_b1;

	u8 dram_dq7_b0; /* EMI_DQ7 is mapped to which DRAMC DQ ?? */
	u8 dram_dq7_b1; /* EMI_DQ15 is mapped to which DRAMC DQ ?? */
};

/* Per-project definition */
static struct cbt_pinmux lp4_cp[CHANNEL_NUM] = {
	{
		/* CHA */
		.dram_dq_b0 = 0,
		.dram_dq_b1 = 1,

		.dram_dmi_b0 = 0,
		.dram_dmi_b1 = 1,
	},
	#if (CHANNEL_NUM>1)
	{
		/* CHB */
		.dram_dq_b0 = 0,
		.dram_dq_b1 = 1,

		.dram_dmi_b0 = 0,
		.dram_dmi_b1 = 1,
	},
	#endif
	#if (CHANNEL_NUM>2)
	{
		/* CHC */
		.dram_dq_b0 = 0,
		.dram_dq_b1 = 1,

		.dram_dmi_b0 = 0,
		.dram_dmi_b1 = 1,
	},
	{
		/* CHD */
		.dram_dq_b0 = 0,
		.dram_dq_b1 = 1,

		.dram_dmi_b0 = 0,
		.dram_dmi_b1 = 1,
	},
	#endif
};

static inline u8 is_byte_mode(DRAMC_CTX_T *p)
{
	return p->dram_cbt_mode[p->rank] == CBT_BYTE_MODE1? 1: 0;
}

static void vSetDramMRCBTOnOff(DRAMC_CTX_T *p, U8 u1OnOff, U8 operating_fsp)
{
	if (u1OnOff)
	{
		// op[7] = !(p->dram_fsp), dram will switch to another FSP_OP automatically
		if (operating_fsp)
		{
			MRWriteFldMulti(p, 13, P_Fld(0, MR13_FSP_OP) |
								   P_Fld(1, MR13_FSP_WR) |
								   P_Fld(1, MR13_CBT),
								   TO_MR);
		}
		else
		{
			MRWriteFldMulti(p, 13, P_Fld(1, MR13_FSP_OP) |
								   P_Fld(0, MR13_FSP_WR) |
								   P_Fld(1, MR13_CBT),
								   TO_MR);
		}

		if (p->dram_cbt_mode[p->rank] == CBT_BYTE_MODE1)
		{
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL0), P_Fld(1, CBT_WLEV_CTRL0_BYTEMODECBTEN) |
				P_Fld(1, CBT_WLEV_CTRL0_CBT_CMP_BYTEMODE));    //BYTEMODECBTEN=1
		}
	}
	else
	{
		if (operating_fsp)
		{
			// !! Remain MR13_FSP_OP = 0, because of system is at low frequency now.
			MRWriteFldMulti(p, 13, P_Fld(0, MR13_FSP_OP) |
								   P_Fld(1, MR13_FSP_WR) |
								   P_Fld(0, MR13_CBT),
								   TO_MR);
		}
		else
		{
			MRWriteFldMulti(p, 13, P_Fld(0, MR13_FSP_OP) |
								   P_Fld(0, MR13_FSP_WR) |
								   P_Fld(0, MR13_CBT),
								   TO_MR);
		}
	}

}

static void CBTEntryLP4(DRAMC_CTX_T *p, U8 operating_fsp, U16 operation_frequency)
{
	struct cbt_pinmux *cp = &lp4_cp[p->channel];

	#if MR_CBT_SWITCH_FREQ
	if (p->dram_fsp == FSP_1)
		DramcModeRegInit_CATerm(p, 1);
	#endif

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL),
			0, MISC_STBCAL_DQSIENCG_NORMAL_EN);

	CKEFixOnOff(p, p->rank, CKE_FIXON, CKE_WRITE_TO_ONE_CHANNEL);

	// yr: CA train old mode and CS traing need to check MRSRK at this point
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL0), u1GetRank(p), SWCMD_CTRL0_MRSRK);

	//Step 0: MRW MR13 OP[0]=1 to enable CBT
	vSetDramMRCBTOnOff(p, ENABLE, operating_fsp);

	//Step 0.1: before CKE low, Let DQS=0 by R_DMwrite_level_en=1, spec: DQS_t has to retain a low level during tDQSCKE period
	if (p->dram_cbt_mode[p->rank] == CBT_NORMAL_MODE)
	{
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL0),
				1, CBT_WLEV_CTRL0_WRITE_LEVEL_EN);

		//TODO, pinmux
		//force byte0 tx
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL0),
			0x1, CBT_WLEV_CTRL0_DQSOEAOEN);

		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL0),
			(1 << cp->dram_dq_b0), CBT_WLEV_CTRL0_CBT_DQBYTE_OEAO_EN);
	}

	mcDELAY_US(1);

	//Step 1.0: let CKE go low
	CKEFixOnOff(p, p->rank, CKE_FIXOFF, CKE_WRITE_TO_ONE_CHANNEL);

	// Adjust u1MR13Value
	(operating_fsp == FSP_1)?
			DramcMRWriteFldAlign(p, 13, 1, MR13_FSP_OP, JUST_TO_GLOBAL_VALUE):
			DramcMRWriteFldAlign(p, 13, 0, MR13_FSP_OP, JUST_TO_GLOBAL_VALUE);

	// Step 1.1 : let IO to O1 path valid
	if (p->dram_cbt_mode[p->rank] == CBT_NORMAL_MODE)
	{
		// Let R_DMFIXDQIEN1=1 (byte1), 0xd8[13]  ==> Note: Do not enable again.
		//Currently set in O1PathOnOff
		//vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_PADCTRL), 0x3, PADCTRL_FIXDQIEN);

		// Let DDRPHY RG_RX_ARDQ_SMT_EN_B1=1 (byte1)
		//vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_B1_DQ3), 1, B1_DQ3_RG_RX_ARDQ_SMT_EN_B1);
		O1PathOnOff(p, ON);
	}

	if (p->dram_cbt_mode[p->rank] == CBT_BYTE_MODE1)
	{
		// let IO to O1 path valid by DDRPHY RG_RX_ARDQ_SMT_EN_B0=1
		//vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_B0_DQ3), 1, B0_DQ3_RG_RX_ARDQ_SMT_EN_B0);
		O1PathOnOff(p, ON);
	}

	// Wait tCAENT
	mcDELAY_US(1);
}

static void CBTExitLP4(DRAMC_CTX_T *p, U8 operating_fsp, U8 operation_frequency)
{
	if (p->dram_cbt_mode[p->rank] == CBT_NORMAL_MODE || p->dram_cbt_mode[p->rank] == CBT_BYTE_MODE1)
	{
		//Step 1: CKE go high (Release R_DMCKEFIXOFF, R_DMCKEFIXON=1)
		CKEFixOnOff(p, p->rank, CKE_FIXON, CKE_WRITE_TO_ONE_CHANNEL);

		//Step 2:wait tCATX, wait tFC
		mcDELAY_US(1);

		//Step 3: MRW to command bus training exit (MR13 OP[0]=0 to disable CBT)
		vSetDramMRCBTOnOff(p, DISABLE, operating_fsp);

		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL0),
				 0, CBT_WLEV_CTRL0_WRITE_LEVEL_EN);
	}

	//Step 4:
	//Disable O1 path output
	if (p->dram_cbt_mode[p->rank] == CBT_NORMAL_MODE)
	{
		//Let DDRPHY RG_RX_ARDQ_SMT_EN_B1=0
		//vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_B1_DQ3), 0, B1_DQ3_RG_RX_ARDQ_SMT_EN_B1);
		O1PathOnOff(p, OFF);
		//Let FIXDQIEN1=0 ==> Note: Do not enable again.
		//Moved into O1PathOnOff
		//vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_PADCTRL), 0, PADCTRL_FIXDQIEN);
	}

	if (p->dram_cbt_mode[p->rank] == CBT_BYTE_MODE1)
	{
		//Let DDRPHY RG_RX_ARDQ_SMT_EN_B0=0
		//vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_B0_DQ3), 0, B0_DQ3_RG_RX_ARDQ_SMT_EN_B0);
		O1PathOnOff(p, OFF);

		//Disable Byte mode CBT enable bit
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL0), P_Fld(0, CBT_WLEV_CTRL0_BYTEMODECBTEN) |
			P_Fld(0, CBT_WLEV_CTRL0_CBT_CMP_BYTEMODE));    //BYTEMODECBTEN=1
	}

	// Wait tCAENT
	mcDELAY_US(1);
}

/*
 * get_mck_ck_ratio -- get ratio of mck:ck
 *
 * TODO, remove later, get the ratio from dram ctx dfs table!!!!
 *
 *
 * return 1 means 1:1
 * return 0 means 1:2
 */
static u8 get_mck_ck_ratio(DRAMC_CTX_T *p)
{
	/*
	* as per DE's comments, LP5 mck:ck has only 1:1 and 1:2.
	* read SHU_LP5_CMD.LP5_CMD1TO2EN to decide which one.
	*/
	u32 ratio;

	ratio = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_LP5_CMD),
		SHU_LP5_CMD_LP5_CMD1TO2EN);

	mcSHOW_DBG_MSG5(("LP5 MCK:CK=%s\n", ratio == 1 ? "1:1" : "1:2"));

	return ratio;
}

static u8 get_cbtui_adjustable_maxvalue(DRAMC_CTX_T *p)
{
	u8 ratio;

	/*
	* MCK:CK=1:1,
	* ther are only 0~1 for ui adjust, if ui value is larger than 1, adjust MCK.
	*
	* MCK:CK=1:2,
	* ther are only 0~3 for ui adjust, if ui value is larger than 3, adjust MCK.
	*
	* MCK:CK=1:4, (for LP4)
	* ther are only 0~7 for ui adjust, if ui value is larger than 7, adjust MCK.
	*
	*/
	ratio = get_mck_ck_ratio(p);

	/* here just for LP5 */
	return ratio == 1? 1: 3;
}

static inline u32 get_ca_mck(DRAMC_CTX_T *p)
{
	u32 dly;

	dly = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_CA3));
	return dly & 0x0FFFFFFFU;
}

static inline void put_ca_mck(DRAMC_CTX_T *p, u32 ca_mck)
{
	u32 dly;

	dly = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_CA3));
	dly &= 0xF0000000U;
	ca_mck &= 0x0FFFFFFFU;
	dly |= ca_mck;

	vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_CA3), dly);
}

static inline u32 get_ca_ui(DRAMC_CTX_T *p)
{
	u32 dly;

	dly = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_CA7));
	return dly & 0x0FFFFFFFU;
}

static inline void put_ca_ui(DRAMC_CTX_T *p, u32 ca_ui)
{
	u32 dly;

	dly = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_CA7));
	dly &= 0xF0000000U;
	ca_ui &= 0x0FFFFFFFU;
	dly |= ca_ui;

	vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_CA7), dly);

	// Note: CKE UI must sync CA UI (CA and CKE delay circuit are same) @Lin-Yi
	// To avoid tXP timing margin issue
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_CA5), ca_ui & 0xF, SHU_SELPH_CA5_DLY_CKE);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_CA6), ca_ui & 0xF, SHU_SELPH_CA6_DLY_CKE1);
}

static void xlate_ca_mck_ui(DRAMC_CTX_T *p, u32 ui_delta,
		u32 mck_old, u32 ui_old, u32 *mck_new, u32 *ui_new)
{
	u8 i;
	u32 mask, max;
	u32 bit_ui, bit_mck;
	u32 ui_tmp = 0, mck_tmp = 0;

	max = get_cbtui_adjustable_maxvalue(p);
	mask = max;

	for (i = 0; i < CATRAINING_NUM_LP5; i++) {
		bit_mck = 0;
		bit_ui = ((ui_old >> (i * 4)) & mask) + ui_delta;
		if (bit_ui > max) {
			bit_mck = bit_ui / (max + 1);
			bit_ui = bit_ui % (max + 1);
		}

		mck_tmp += (bit_mck << (i * 4));
		ui_tmp += (bit_ui << (i * 4));
	}

	if (ui_new)
		*ui_new = ui_tmp;

	if (mck_new)
		*mck_new = mck_old + mck_tmp;
}

static inline u8 get_ca_pi_per_ui(DRAMC_CTX_T *p)
{
#if __LP5_COMBO__
	if (p->freq_sel == LP5_DDR4266)
		return 64;
	else
#endif
		return 32;
}

static int get_capi_max(DRAMC_CTX_T *p)
{
	if (u1IsPhaseMode(p) == TRUE)
	{
		return 32;
	}

	return 64;
}

static S16 adjust_ca_ui(DRAMC_CTX_T *p, U32 ca_mck,
		U32 ca_ui, S16 pi_dly)
{
	S16 p2u;
	S16 ui, pi;
	U32 ui_new = 0, mck_new = 0;

	if (pi_dly < get_capi_max(p))
	{
		return pi_dly;
	}

	p2u = get_ca_pi_per_ui(p);

	ui = pi_dly / p2u;
	pi = pi_dly % p2u;

	xlate_ca_mck_ui(p, ui, ca_mck, ca_ui, &mck_new, &ui_new);

	put_ca_ui(p, ui_new);
	put_ca_mck(p, mck_new);
	mcSHOW_DBG_MSG5(("mck_new: 0x%x, ui_new: 0x%x, pi:%d\n",
		mck_new, ui_new, pi));

	return pi;
}

static inline u32 get_cs_mck(DRAMC_CTX_T *p)
{
	if (p->rank == RANK_1)
		return u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_CA1),
			SHU_SELPH_CA1_TXDLY_CS1);
	else
		return u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_CA1),
			SHU_SELPH_CA1_TXDLY_CS);
}

static inline void put_cs_mck(DRAMC_CTX_T *p, u32 cs_ui)
{
	if (p->rank == RANK_1)
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_CA1),
			cs_ui, SHU_SELPH_CA1_TXDLY_CS1);
	else
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_CA1),
			cs_ui, SHU_SELPH_CA1_TXDLY_CS);
}

static inline u32 get_cs_ui(DRAMC_CTX_T *p)
{
	if (p->rank == RANK_1)
		return u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_CA5),
			SHU_SELPH_CA5_DLY_CS1);
	else
		return u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_CA5),
			SHU_SELPH_CA5_DLY_CS);
}

static inline void put_cs_ui(DRAMC_CTX_T *p, u32 cs_ui)
{
	if (p->rank == RANK_1)
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_CA5),
			cs_ui, SHU_SELPH_CA5_DLY_CS1);
	else
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_CA5),
			cs_ui, SHU_SELPH_CA5_DLY_CS);
}

//void LP5_ShiftCSUI(DRAMC_CTX_T *p, S8 iShiftUI)
//{
//	REG_TRANSFER_T TransferUIRegs  = {DRAMC_REG_SHU_SELPH_CA5, SHU_SELPH_CA5_DLY_CS};
//	REG_TRANSFER_T TransferMCKRegs = {DRAMC_REG_SHU_SELPH_CA1, SHU_SELPH_CA1_TXDLY_CS};
//
//	ExecuteMoveDramCDelay(p, TransferUIRegs[i], TransferMCKRegs[i], iShiftUI);
//}

static S16 adjust_cs_ui(DRAMC_CTX_T *p, u32 cs_mck, u32 cs_ui, S16 pi_dly)
{
	S16 p2u;
	S16 ui = 0, pi = 0;
	u8 ratio;
	u32 ui_max;
	u32 cs_bit_mask, cs_ui_tmp, cs_mck_tmp;

	if (pi_dly < get_capi_max(p))
	{
		return pi_dly;
	}

	p2u = get_ca_pi_per_ui(p);

	ui = pi_dly / p2u;
	pi = pi_dly % p2u;

	ratio = get_mck_ck_ratio(p);
	if (ratio) {
		/* 1:1 */
		cs_bit_mask = 1;
	} else {
		/* 1:2 */
		cs_bit_mask = 3;
	}

	ui_max = get_cbtui_adjustable_maxvalue(p);
	cs_ui_tmp = (cs_ui & cs_bit_mask) + ui;
	cs_mck_tmp = 0;
	if (cs_ui_tmp > ui_max) {
		cs_mck_tmp = cs_ui_tmp / (ui_max + 1);
		cs_ui_tmp = cs_ui_tmp % (ui_max + 1);
	}

	cs_mck_tmp += cs_mck;
	put_cs_ui(p, cs_ui_tmp);
	put_cs_mck(p, cs_mck_tmp);

	mcSHOW_DBG_MSG5(("csmck:%d, csui: %d, pi:%d before\n",
			cs_mck, cs_ui, 0));
	mcSHOW_DBG_MSG5(("csmck:%d, csui: %d, pi:%d after\n",
			cs_mck_tmp, cs_ui_tmp, pi));

	return pi;
}

static u32 get_capi_step(DRAMC_CTX_T *p)
{
	u32 step;

	switch (p->freq_sel) {
	case LP5_DDR800:
	case LP5_DDR1200:
	case LP5_DDR1600:
	case LP5_DDR3733:
		step = 8;
		break;
	default:
		if (vGet_DDR_Loop_Mode(p) == SEMI_OPEN_LOOP_MODE)
		{
			step = 8;
		}
		else if (vGet_DDR_Loop_Mode(p) == OPEN_LOOP_MODE)
		{
			step = 16;
		}
		else
		{
			step = 1;
		}
		break;
	}

#if FOR_DV_SIMULATION_USED
	return 8;
#else
	return step;
#endif
}

#if CBT_O1_PINMUX_WORKAROUND
static u32 CBTCompareWordaroundDecodeO1Pinmux(DRAMC_CTX_T *p, u32 o1_value, U8 *uiLPDDR_O1_Mapping)
{
	U8 u1Idx;
	U32 u4Result;

	u4Result = 0;

	for (u1Idx = 0;u1Idx < p->data_width;u1Idx++)
		u4Result |= ((o1_value >> uiLPDDR_O1_Mapping[u1Idx]) & 0x1) << u1Idx;

	return u4Result;
}

static u32 CBTDelayCACLKCompareWorkaround(DRAMC_CTX_T *p)
{
	u8 u1pattern_index, u1ca_index, u1dq_index, u1dq_start, u1dq_end, u1ca_number_per_bit, u1bit_num_per_byte, u1pattern_choose;
	U8 *uiLPDDR_O1_Mapping = NULL;
	u32 u4TimeCnt, rdy, u4dq_o1, u4data_receive, u4ca_pattern, u4Result, u4Ready;
	u8 u1pattern_num;

	const U8 u1LP5CBT_Pattern_Mapping[2][7] =
	{
		{
			1, 2, 4, 8, 16, 32, 64
		},

		{
			126, 125, 123, 119, 111, 95, 63
		},
	};
	const U8 u1LP4CBT_Pattern_Mapping[2][6] =
	{
		{
			1, 2, 4, 8, 16, 32
		},

		{
			62, 61, 59, 55, 47, 31
		},
	};

	u4Result = 0;
	u1bit_num_per_byte = 8;

#if (__LP5_COMBO__)
	if (is_lp5_family(p))
	{
		uiLPDDR_O1_Mapping = (U8 *)uiLPDDR5_O1_Mapping_POP[p->channel];
		u1pattern_num = 8;
		u1ca_number_per_bit = CATRAINING_NUM_LP5;
		if (p->dram_cbt_mode[p->rank] == CBT_NORMAL_MODE)
		{
			u1dq_start = 0;
			u1dq_end = 6;
		}
		else
		{
			u1dq_start = 0;
			u1dq_end = 14;
		}
	}
	else
#endif
	{
		uiLPDDR_O1_Mapping = (U8 *)uiLPDDR4_O1_Mapping_POP[p->channel];
		u1pattern_num = 4;
		u1ca_number_per_bit = CATRAINING_NUM_LP4;
		if (p->dram_cbt_mode[p->rank] == CBT_NORMAL_MODE)
		{
			u1dq_start = 8;
			u1dq_end = 13;
		}
		else
		{
			u1dq_start = 0;
			u1dq_end = 13;
		}
	}

	vIO32WriteFldMulti(DRAMC_REG_CBT_WLEV_CTRL3, P_Fld(0x1, CBT_WLEV_CTRL3_CATRAIN_PAT_STOP0)
		| P_Fld(0x1, CBT_WLEV_CTRL3_CATRAIN_PAT_STOP1));

	for (u1pattern_index = 0; u1pattern_index < u1pattern_num; u1pattern_index++)
	{
		u1pattern_choose = (u1pattern_index > 3) ? (u1pattern_index % 2) : /* LP5 mapping */
			((u1pattern_index > 1)? (3 - u1pattern_index) : u1pattern_index); /* LP5 & LP4 mapping */
		for (u1ca_index = 0; u1ca_index < u1ca_number_per_bit; u1ca_index++)
		{
		#if (__LP5_COMBO__)
			if (is_lp5_family(p))
			{
				u4ca_pattern = u1LP5CBT_Pattern_Mapping[u1pattern_choose][u1ca_index];
			}
			else
		#endif
			{
				u4ca_pattern = u1LP4CBT_Pattern_Mapping[u1pattern_choose][u1ca_index];
			}

			vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL3), P_Fld((u1pattern_index+1), CBT_WLEV_CTRL3_CATRAIN_1PAT_SEL0)
													   | P_Fld((u1ca_index+1), CBT_WLEV_CTRL3_CATRAIN_1PAT_SEL1));

			u4TimeCnt = TIME_OUT_CNT;

			vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL0), 1, CBT_WLEV_CTRL0_CBT_CAPATEN);

			//Check CA training compare ready (dramc_conf_nao 0x3fc , CATRAIN_CMP_CPT)
			do
			{
				u4Ready = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_STATUS1), CBT_WLEV_STATUS1_CATRAIN_CMP_CPT);
				u4TimeCnt --;
				mcDELAY_US(1);
			}while ((u4Ready == 0) && (u4TimeCnt > 0));

			if (u4TimeCnt == 0)//time out
			{
				mcSHOW_DBG_MSG(("[CBTDelayCACLKCompare] Resp fail (time out)\n"));
				mcFPRINTF((fp_A60868, "[CBTDelayCACLKCompare] Resp fail (time out)\n"));//Eddie Test
				//return DRAM_FAIL;
			}

			u4dq_o1 = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DQO1), MISC_DQO1_DQO1_RO);

			u4dq_o1 = CBTCompareWordaroundDecodeO1Pinmux(p, u4dq_o1, uiLPDDR_O1_Mapping);

			if (u1dq_end >= u1ca_number_per_bit)
				u4ca_pattern |= u4ca_pattern << u1bit_num_per_byte;

			u4dq_o1 ^= u4ca_pattern;

			for(u1dq_index=u1dq_start; u1dq_index<=u1dq_end; u1dq_index++)
			{
				if ((p->dram_cbt_mode[p->rank] == CBT_BYTE_MODE1) && (u1dq_index == u1ca_number_per_bit))
					u1dq_index = u1bit_num_per_byte;

				u4data_receive = (u4dq_o1 >> u1dq_index) & 0x1;

				if (u1dq_index < u1bit_num_per_byte)
					u4Result |= u4data_receive << u1dq_index;
				else
					u4Result |= u4data_receive << u1dq_index - u1bit_num_per_byte;
			}

			vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL0), 0, CBT_WLEV_CTRL0_CBT_CAPATEN);

		}
		if (u4Result == ((0x1 << u1ca_number_per_bit) - 1))
			break;
	}
	return u4Result;
}

static u32 new_cbt_pat_compare_workaround(DRAMC_CTX_T *p, new_cbt_pat_cfg_t *ncm)
{
	u8 u1pattern_index, u1ca_index, u1dq_index, u1dq_start, u1dq_end, u1ca_number_per_bit, u1bit_num_per_byte;
	U8 *uiLPDDR_O1_Mapping = NULL;
	u32 u4TimeCnt, rdy, u4dq_o1, u4data_receive, u4ca_pattern_a, u4ca_pattern, u4Result, u4Ready;
	u8 u1pattern_num;

	u4Result = 0;
	u1bit_num_per_byte = 8;

#if (__LP5_COMBO__)
	if (is_lp5_family(p))
	{
		uiLPDDR_O1_Mapping = (U8 *)uiLPDDR5_O1_Mapping_POP[p->channel];
		u1pattern_num = 8;
		u1ca_number_per_bit = 7;
		if (p->dram_cbt_mode[p->rank] == CBT_NORMAL_MODE)
		{
			u1dq_start = 0;
			u1dq_end = 6;
		}
		else
		{
			u1dq_start = 0;
			u1dq_end = 14;
		}
	}
	else
#endif
	{
		uiLPDDR_O1_Mapping = (U8 *)uiLPDDR4_O1_Mapping_POP[p->channel];
		u1pattern_num = 4;
		u1ca_number_per_bit = 6;
		if (p->dram_cbt_mode[p->rank] == CBT_NORMAL_MODE)
		{
			u1dq_start = 8;
			u1dq_end = 13;
		}
		else
		{
			u1dq_start = 0;
			u1dq_end = 13;
		}
	}

	for (u1pattern_index = 0; u1pattern_index < u1pattern_num; u1pattern_index++)
	{
		u4ca_pattern_a = ((ncm->pat_a[u1pattern_index] >> ncm->ca_golden_sel) & 0x1) ? ((0x1 << u1ca_number_per_bit) - 1) : 0x0;

		for (u1ca_index = 0; u1ca_index < u1ca_number_per_bit; u1ca_index++)
		{
			u4ca_pattern = u4ca_pattern_a & ~(0x1 << u1ca_index);

			if ((ncm->pat_v[u1pattern_index] >> ncm->ca_golden_sel) & 0x1)
				u4ca_pattern |= 0x1 << u1ca_index;

			if (ncm->invert_num)
				u4ca_pattern ^= (0x1 << u1ca_number_per_bit) - 1;

			vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL5), P_Fld(u1pattern_index, CBT_WLEV_CTRL5_NEW_CBT_PAT_NUM)
													   | P_Fld(u1ca_index, CBT_WLEV_CTRL5_NEW_CBT_CA_NUM));

			vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL5), 1, CBT_WLEV_CTRL5_NEW_CBT_CAPATEN);

			//Check CA training compare ready (dramc_conf_nao 0x3fc , CATRAIN_CMP_CPT)
			do
			{
				u4Ready = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_STATUS2), CBT_WLEV_STATUS2_CBT_PAT_CMP_CPT);
				u4TimeCnt --;
				mcDELAY_US(1);
			}while ((u4Ready == 0) && (u4TimeCnt > 0));

			if (u4TimeCnt == 0)//time out
			{
				mcSHOW_DBG_MSG(("[CBTDelayCACLKCompare] Resp fail (time out)\n"));
				mcFPRINTF((fp_A60868, "[CBTDelayCACLKCompare] Resp fail (time out)\n"));//Eddie Test
				//return DRAM_FAIL;
			}

			u4dq_o1 = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DQO1), MISC_DQO1_DQO1_RO);

			u4dq_o1 = CBTCompareWordaroundDecodeO1Pinmux(p, u4dq_o1, uiLPDDR_O1_Mapping);

			if (u1dq_end >= u1ca_number_per_bit)
				u4ca_pattern |= u4ca_pattern << u1bit_num_per_byte;

			u4dq_o1 ^= u4ca_pattern;

			for(u1dq_index=u1dq_start; u1dq_index<=u1dq_end; u1dq_index++)
			{
				if ((p->dram_cbt_mode[p->rank] == CBT_BYTE_MODE1) && (u1dq_index == u1ca_number_per_bit))
					u1dq_index = u1bit_num_per_byte;

				u4data_receive = (u4dq_o1 >> u1dq_index) & 0x1;

				if (u1dq_index < u1bit_num_per_byte)
					u4Result |= u4data_receive << u1dq_index;
				else
					u4Result |= u4data_receive << u1dq_index - u1bit_num_per_byte;
			}

			vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL5), 0, CBT_WLEV_CTRL5_NEW_CBT_CAPATEN);
		}
		if (u4Result == ((0x1 << u1ca_number_per_bit) - 1))
			break;
	}
	return u4Result;
}
#endif

void CBTDelayCACLK(DRAMC_CTX_T *p, S32 iDelay)
{
	if (iDelay < 0)
	{	/* Set CLK delay */
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_CA_CMD0),
			P_Fld(0, SHU_R0_CA_CMD0_RG_ARPI_CMD) |
			P_Fld(-iDelay, SHU_R0_CA_CMD0_RG_ARPI_CLK) |
			P_Fld(-iDelay, SHU_R0_CA_CMD0_RG_ARPI_CS));
	}
/*
	else if (iDelay >= 64)
	{
		DramcCmdUIDelaySetting(p, 2);

		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_CA_CMD0),
			P_Fld(iDelay - 64, SHU_R0_CA_CMD0_RG_ARPI_CMD) |
			P_Fld(0, SHU_R0_CA_CMD0_RG_ARPI_CLK) |
			P_Fld(0, SHU_R0_CA_CMD0_RG_ARPI_CS));
	}
*/
	else
	{	/* Set CA output delay */
//		DramcCmdUIDelaySetting(p, 0);

		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_CA_CMD0),
			P_Fld(iDelay, SHU_R0_CA_CMD0_RG_ARPI_CMD) |
			P_Fld(0, SHU_R0_CA_CMD0_RG_ARPI_CLK) |
			P_Fld(0, SHU_R0_CA_CMD0_RG_ARPI_CS));
	}
}

static void CBTAdjustCS(DRAMC_CTX_T *p, int autok)
{
	S32 iFirstCSPass = 0, iLastCSPass = 0, iCSFinalDelay;//iCSCenter

	U8 backup_rank, ii;
	u32 pi_dly;
	u32 cs_ui, cs_mck;

	backup_rank = u1GetRank(p);

	cs_ui = get_cs_ui(p);
	cs_mck = get_cs_mck(p);

#if (SUPPORT_SAVE_TIME_FOR_CALIBRATION && BYPASS_CBT)
	if (p->femmc_Ready == 1)
	{
		CATrain_CsDelay[p->channel][p->rank] = p->pSavetimeData->u1CBTCsDelay_Save[p->channel][p->rank];
	}
#endif

	// if dual rank, use average position of both rank
	if(backup_rank == RANK_1)
	{
		iCSFinalDelay = (CATrain_CsDelay[p->channel][RANK_0] + CATrain_CsDelay[p->channel][RANK_1]) >> 1;
	}
	else
	{
		iCSFinalDelay = CATrain_CsDelay[p->channel][p->rank];
	}

	//Set CS output delay after training
	/* p->rank = RANK_0, save to Reg Rank0 and Rank1, p->rank = RANK_1, save to Reg Rank1 */
	for (ii = RANK_0; ii <= backup_rank; ii++)
	{
		vSetRank(p, ii);

		pi_dly = adjust_cs_ui(p, cs_mck, cs_ui, iCSFinalDelay);

		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_CA_CMD0), pi_dly, SHU_R0_CA_CMD0_RG_ARPI_CS);
	}

	vSetRank(p, backup_rank);

	mcSHOW_DBG_MSG(("CS Dly: %d (%d~%d)\n", iCSFinalDelay, iFirstCSPass, iLastCSPass));
}

#if CA_PER_BIT_DELAY_CELL
static void CATrainingSetPerBitDelayCell(DRAMC_CTX_T *p, S16 *iCAFinalCenter, U8 ca_pin_num)
{
	U8 *uiLPDDR_CA_Mapping = NULL;
	U8 u1CA;
	S8 iCA_PerBit_DelayLine[8] = {0};

#if __LP5_COMBO__
	if (is_lp5_family(p))
	{
		uiLPDDR_CA_Mapping = (U8 *)uiLPDDR5_CA_Mapping_POP[p->channel];
	}
	else
#endif
	{
		uiLPDDR_CA_Mapping = (U8 *)uiLPDDR4_CA_Mapping_POP[p->channel];
	}

	for (u1CA = 0;u1CA < ca_pin_num;u1CA++)
	{
		iCA_PerBit_DelayLine[uiLPDDR_CA_Mapping[u1CA]] = iCAFinalCenter[u1CA];
	}

	// Set CA perbit delay line calibration results
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_CA_TXDLY0),
			P_Fld(iCA_PerBit_DelayLine[0], SHU_R0_CA_TXDLY0_TX_ARCA0_DLY) |
			P_Fld(iCA_PerBit_DelayLine[1], SHU_R0_CA_TXDLY0_TX_ARCA1_DLY) |
			P_Fld(iCA_PerBit_DelayLine[2], SHU_R0_CA_TXDLY0_TX_ARCA2_DLY) |
			P_Fld(iCA_PerBit_DelayLine[3], SHU_R0_CA_TXDLY0_TX_ARCA3_DLY));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_CA_TXDLY1),
			P_Fld(iCA_PerBit_DelayLine[4], SHU_R0_CA_TXDLY1_TX_ARCA4_DLY) |
			P_Fld(iCA_PerBit_DelayLine[5], SHU_R0_CA_TXDLY1_TX_ARCA5_DLY) |
			P_Fld(iCA_PerBit_DelayLine[6], SHU_R0_CA_TXDLY1_TX_ARCA6_DLY) |
			P_Fld(iCA_PerBit_DelayLine[7], SHU_R0_CA_TXDLY1_TX_ARCA7_DLY));
}
#endif// end of CA_PER_BIT_DELAY_CELL

static void CBTSetCACLKResult(DRAMC_CTX_T *p, U32 u4MCK, U32 u4UI, S8 iFinalCACLK, U8 ca_pin_num)
{
	U8 backup_rank, rank_i, uiCA;
	S16 iCAFinalCenter[CATRAINING_NUM]={0}; //for CA_PER_BIT

#if (SUPPORT_SAVE_TIME_FOR_CALIBRATION && BYPASS_CBT)
	if (p->femmc_Ready == 1)
	{
		CATrain_CmdDelay[p->channel][p->rank] = p->pSavetimeData->s1CBTCmdDelay_Save[p->channel][p->rank];
		vSetCalibrationResult(p, DRAM_CALIBRATION_CA_TRAIN, DRAM_FAST_K);
		#if CA_PER_BIT_DELAY_CELL
			for (uiCA = 0; uiCA < ca_pin_num; uiCA++)
				iCAFinalCenter[uiCA] = p->pSavetimeData->u1CBTCA_PerBit_DelayLine_Save[p->channel][p->rank][uiCA];
		#endif
	}
#endif

	iFinalCACLK = CATrain_CmdDelay[p->channel][p->rank];

	mcSHOW_DBG_MSG(("\n[CBTSetCACLKResult] CA Dly = %d\n", iFinalCACLK));

	iFinalCACLK = adjust_ca_ui(p, u4MCK, u4UI, iFinalCACLK);

	backup_rank = u1GetRank(p);

	for (rank_i = RANK_0; rank_i <= backup_rank;rank_i++)
	{
		vSetRank(p, rank_i);

		CBTDelayCACLK(p, iFinalCACLK);

#if CA_PER_BIT_DELAY_CELL
		CATrainingSetPerBitDelayCell(p, iCAFinalCenter, ca_pin_num);
#endif
	}

	vSetRank(p, backup_rank);
}

#if (__LP5_COMBO__)
/* Return (Vref_B0 | (Vref_B1 << 8) to support Byte mode */
static U8 GetCBTVrefPinMuxValue_lp5(DRAMC_CTX_T *p, U8 u1VrefLevel)
{
	U8 u2VrefBit, u2Vref_org;
	U16 u2Vref_new;

	u2Vref_org = u1VrefLevel & 0x7f;

	u2Vref_new = 0;

	for (u2VrefBit = 0; u2VrefBit < 8; u2VrefBit++)
	{
		//mcSHOW_DBG_MSG(("=== u2VrefBit: %d, %d\n",u2VrefBit,uiLPDDR4_O1_Mapping_POP[p->channel][u2VrefBit]));
		if (u2Vref_org & (1 << u2VrefBit))
		{
			u2Vref_new |= (1 << uiLPDDR5_O1_Mapping_POP[p->channel][u2VrefBit]);
		}
	}

	mcSHOW_DBG_MSG3(("=== u2Vref_new: 0x%x --> 0x%x\n", u2Vref_org, u2Vref_new));

	if (lp5_cp[p->channel].dram_dq_b0)
		u2Vref_new >>= 8;

	return u2Vref_new;
}

#endif

static U8 GetCBTVrefPinMuxValue(DRAMC_CTX_T *p, U8 u1VrefRange, U8 u1VrefLevel)
{
	U8 u2VrefBit, u2Vref_org;
	U16 u2Vref_new;

	if (p->dram_cbt_mode[p->rank] == CBT_BYTE_MODE1)
		return ((u1VrefRange & 0x1) << 6) | (u1VrefLevel & 0x3f);

	u2Vref_org = ((u1VrefRange & 0x1) << 6) | (u1VrefLevel & 0x3f);

	u2Vref_new = 0;
	for (u2VrefBit = 0; u2VrefBit < 8; u2VrefBit++)
	{
		//mcSHOW_DBG_MSG(("=== u2VrefBit: %d, %d\n",u2VrefBit,uiLPDDR4_O1_Mapping_POP[p->channel][u2VrefBit]));
		if (u2Vref_org & (1 << u2VrefBit))
		{
			u2Vref_new |= (1 << uiLPDDR4_O1_Mapping_POP[p->channel][u2VrefBit]);
		}
	}

	mcSHOW_DBG_MSG3(("=== u2Vref_new: 0x%x --> 0x%x\n", u2Vref_org, u2Vref_new));

	if (lp4_cp[p->channel].dram_dq_b0)
		u2Vref_new >>= 8;

	return u2Vref_new;
}

static void CBTSetVrefLP4(DRAMC_CTX_T *p, U8 u1VrefRange, U8 u1VrefLevel, U8 operating_fsp, U8 stateFlag)
{
	U32 fld;
	U8 u4DbgValue;
	U8 u1VrefValue_pinmux;
	struct cbt_pinmux *cp = &lp4_cp[p->channel];

	if ((p->dram_cbt_mode[p->rank] == CBT_NORMAL_MODE) &&
		(stateFlag == IN_CBT))
	{
		u1VrefValue_pinmux = GetCBTVrefPinMuxValue(p, u1VrefRange, u1VrefLevel);

#if !REDUCE_LOG_FOR_PRELOADER
		mcSHOW_DBG_MSG(("\nCH_%d, RK_%d, Range=%d, VrefValue_pinmux = 0x%x\n", p->channel, p->rank, u1VrefRange, u1VrefValue_pinmux));
#endif
		u1MR12Value[p->channel][p->rank][operating_fsp] = ((u1VrefRange & 0x1) << 6) | u1VrefLevel;

		fld = (cp->dram_dq_b0) ? CBT_WLEV_CTRL4_CBT_TXDQ_B1 : CBT_WLEV_CTRL4_CBT_TXDQ_B0;

		//vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_WRITE_LEV), ((u1VrefRange&0x1) <<6) | (u1VrefLevel & 0x3f), WRITE_LEV_DMVREFCA);	//MR12, bit[25:20]=OP[5:0]	bit 26=OP[6]
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL4),
			u1VrefValue_pinmux, fld);  //MR12, bit[25:20]=OP[5:0]  bit 26=OP[6]

		 //DQS_SEL=1, DQS_B1_G=1, Toggle R_DMDQS_WLEV (1 to 0)
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL0), (0x1 << cp->dram_dq_b0), CBT_WLEV_CTRL0_CBT_WLEV_DQS_SEL);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL3), 0xa, CBT_WLEV_CTRL3_DQSBX_G);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL0), 1, CBT_WLEV_CTRL0_CBT_WLEV_DQS_TRIG);
		mcDELAY_US(1);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL0), 0, CBT_WLEV_CTRL0_CBT_WLEV_DQS_TRIG);

	}
	else
	{
		if (operating_fsp == FSP_1)
		{
			DramcMRWriteFldAlign(p, 13, 1, MR13_FSP_WR, TO_MR);
		}

		u4DbgValue = (((u1VrefRange & 0x1) << 6) | (u1VrefLevel & 0x3f));
		u1MR12Value[p->channel][p->rank][operating_fsp] = u4DbgValue;
		mcSHOW_DBG_MSG3(("u4DbgValue = 0x%x\n", u4DbgValue));

		DramcModeRegWriteByRank(p, p->rank, 12, u4DbgValue);
	}

	//wait tVREF_LONG
	mcDELAY_US(1);
}


#if __LP5_COMBO__
static inline u8 is_training_mode1(DRAMC_CTX_T *p)
{
	return is_lp5_family(p) && p->lp5_training_mode == TRAINING_MODE1? 1: 0;
}

static inline u8 is_training_mode2(DRAMC_CTX_T *p)
{
	return is_lp5_family(p) && p->lp5_training_mode == TRAINING_MODE2? 1: 0;
}

static inline u8 is_phase_falling(DRAMC_CTX_T *p)
{
	return is_lp5_family(p) && p->lp5_cbt_phase == CBT_PHASE_FALLING? 1: 0;
}

static void force_dq7(DRAMC_CTX_T *p, u8 level)
{
	u32 fld_b0, fld_b1;
	u8 dq;
	u8 dramc_byte;
	struct cbt_pinmux *cp = &lp5_cp[p->channel];
	/*
	 * TODO
	 *
	 * pinmux to selec dq7
	 *
	 */

	fld_b0 = (cp->dram_dq_b0) ? CBT_WLEV_CTRL4_CBT_TXDQ_B1 : CBT_WLEV_CTRL4_CBT_TXDQ_B0;
	fld_b1 = (cp->dram_dq_b1) ? CBT_WLEV_CTRL4_CBT_TXDQ_B1 : CBT_WLEV_CTRL4_CBT_TXDQ_B0;

	dq = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL4),
		fld_b0);
	dq &= ~(1 << (cp->dram_dq7_b0 % 8));
	dq |= ((level & 1) << (cp->dram_dq7_b0 % 8));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL4),
		P_Fld(dq, fld_b0));

	if (is_byte_mode(p)) {
		dq = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL4),
			fld_b1);
		dq &= ~(1 << (cp->dram_dq7_b1 % 8));
		dq |= ((level & 1) << (cp->dram_dq7_b1 % 8));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL4),
			P_Fld(dq, fld_b1));
	}
}

static inline void force_dmi(DRAMC_CTX_T *p, u8 level)
{
	struct cbt_pinmux *cp = &lp5_cp[p->channel];

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL0),
		P_Fld(level, (cp->dram_dmi_b0) ? CBT_WLEV_CTRL0_CBT_SW_DQM_B1_LP5 : CBT_WLEV_CTRL0_CBT_SW_DQM_B0_LP5));

	if (is_byte_mode(p)) {
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL0),
		P_Fld(level, (cp->dram_dmi_b1 ? CBT_WLEV_CTRL0_CBT_SW_DQM_B1_LP5 : CBT_WLEV_CTRL0_CBT_SW_DQM_B0_LP5)));
	}
}

static void toggle_wck(DRAMC_CTX_T *p, u8 toggle)
{
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL0),
		P_Fld(toggle, CBT_WLEV_CTRL0_CBT_WLEV_WCKAO));
}

static void set_vref_by_mrw(DRAMC_CTX_T *p, u8 vref)
{
	DramcModeRegWriteByRank(p, p->rank, 12, vref);
}

static void set_vref_by_dq(DRAMC_CTX_T *p, u16 vref)
{
	u8 dq;
	struct cbt_pinmux *cp = &lp5_cp[p->channel];

	force_dmi(p, 0);
	/* wait tCBTRTW */
	mcDELAY_US(1);

	if (is_byte_mode(p)) {
		/* DRAMC B0/B1 as TX */
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL0),
			3, CBT_WLEV_CTRL0_CBT_DQBYTE_OEAO_EN);

		/* Set DRAM Byte 1 */
		dq = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL4),
			(cp->dram_dq_b1 ? CBT_WLEV_CTRL4_CBT_TXDQ_B1 : CBT_WLEV_CTRL4_CBT_TXDQ_B0));

		/* Shall be carefully processed in case DQ[7] is changed */
		dq &= (1 << (cp->dram_dq7_b1 % 8));
		dq |= ((vref >> 8) & 0xff);
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL4),
			P_Fld(dq, (cp->dram_dq_b1 ? CBT_WLEV_CTRL4_CBT_TXDQ_B1 : CBT_WLEV_CTRL4_CBT_TXDQ_B0)));
	} else {
		/* DRAMC B0 as TX */
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL0),
			(1 << cp->dram_dq_b0), CBT_WLEV_CTRL0_CBT_DQBYTE_OEAO_EN);
	}

	/* Set DRAM Byte 0 */
	dq = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL4),
		(cp->dram_dq_b0 ? CBT_WLEV_CTRL4_CBT_TXDQ_B1 : CBT_WLEV_CTRL4_CBT_TXDQ_B0));
	dq &= (1 << (cp->dram_dq7_b0 % 8));
	dq |= (vref & 0xff);
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL4),
		P_Fld(dq, (cp->dram_dq_b0 ? CBT_WLEV_CTRL4_CBT_TXDQ_B1 : CBT_WLEV_CTRL4_CBT_TXDQ_B0)));

	/* wait tDQStrain */
	mcDELAY_US(1);
	force_dmi(p, 1);
	mcDELAY_US(1);
	/* DRAMC B0/B1 as RX */
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL0),
		0, CBT_WLEV_CTRL0_CBT_DQBYTE_OEAO_EN);
	mcDELAY_US(1);
}

static void switch_oe_tie(DRAMC_CTX_T *p, u8 sw)
{
	u8 dq_oe;
	struct cbt_pinmux *cp = &lp5_cp[p->channel];

	if (sw) {
		/* Set DRAM Byte 0 */
		if (cp->dram_dq_b0) {
			dq_oe = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ2),
				B1_DQ2_RG_TX_ARDQ_OE_TIE_EN_B1);
			dq_oe |= (1 << (cp->dram_dq7_b0 % 8));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ2),
				P_Fld(dq_oe, B1_DQ2_RG_TX_ARDQ_OE_TIE_EN_B1) |
				P_Fld(1, B1_DQ2_RG_TX_ARDQ_OE_TIE_SEL_B1));

			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ2),
				P_Fld(1, B1_DQ2_RG_TX_ARDQM_OE_TIE_EN_B1) |
				P_Fld(1, B1_DQ2_RG_TX_ARDQM_OE_TIE_SEL_B1));
		} else {
			dq_oe = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ2),
				B0_DQ2_RG_TX_ARDQ_OE_TIE_EN_B0);
			dq_oe |= (1 << (cp->dram_dq7_b0 % 8));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ2),
				P_Fld(dq_oe, B0_DQ2_RG_TX_ARDQ_OE_TIE_EN_B0) |
				P_Fld(1, B0_DQ2_RG_TX_ARDQ_OE_TIE_SEL_B0));

			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ2),
				P_Fld(1, B0_DQ2_RG_TX_ARDQM_OE_TIE_EN_B0) |
				P_Fld(1, B0_DQ2_RG_TX_ARDQM_OE_TIE_SEL_B0));
		}

		/* Set DRAM Byte 1 */
		if (is_byte_mode(p)) {
			/* Set DRAM Byte 0 */
			if (cp->dram_dq_b1) {
				dq_oe = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ2),
					B1_DQ2_RG_TX_ARDQ_OE_TIE_EN_B1);
				dq_oe |= (1 << (cp->dram_dq7_b1 % 8));
				vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ2),
					P_Fld(dq_oe, B1_DQ2_RG_TX_ARDQ_OE_TIE_EN_B1) |
					P_Fld(1, B1_DQ2_RG_TX_ARDQ_OE_TIE_SEL_B1));

				vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ2),
					P_Fld(1, B1_DQ2_RG_TX_ARDQM_OE_TIE_EN_B1) |
					P_Fld(1, B1_DQ2_RG_TX_ARDQM_OE_TIE_SEL_B1));
			} else {
				dq_oe = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ2),
					B0_DQ2_RG_TX_ARDQ_OE_TIE_EN_B0);
				dq_oe |= (1 << (cp->dram_dq7_b1 % 8));
				vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ2),
					P_Fld(dq_oe, B0_DQ2_RG_TX_ARDQ_OE_TIE_EN_B0) |
					P_Fld(1, B0_DQ2_RG_TX_ARDQ_OE_TIE_SEL_B0));

				vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ2),
					P_Fld(1, B0_DQ2_RG_TX_ARDQM_OE_TIE_EN_B0) |
					P_Fld(1, B0_DQ2_RG_TX_ARDQM_OE_TIE_SEL_B0));
			}
		}
	} else {
		/* Set DRAM Byte 0 */
		if (cp->dram_dq_b0) {
			dq_oe = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ2),
				B1_DQ2_RG_TX_ARDQ_OE_TIE_EN_B1);
			dq_oe &= ~(1 << (cp->dram_dq7_b0 % 8));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ2),
				P_Fld(dq_oe, B1_DQ2_RG_TX_ARDQ_OE_TIE_EN_B1) |
				P_Fld(0, B1_DQ2_RG_TX_ARDQ_OE_TIE_SEL_B1));

			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ2),
				P_Fld(0, B1_DQ2_RG_TX_ARDQM_OE_TIE_EN_B1) |
				P_Fld(0, B1_DQ2_RG_TX_ARDQM_OE_TIE_SEL_B1));
		} else {
			dq_oe = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ2),
				B0_DQ2_RG_TX_ARDQ_OE_TIE_EN_B0);
			dq_oe &= ~(1 << (cp->dram_dq7_b0 % 8));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ2),
				P_Fld(dq_oe, B0_DQ2_RG_TX_ARDQ_OE_TIE_EN_B0) |
				P_Fld(0, B0_DQ2_RG_TX_ARDQ_OE_TIE_SEL_B0));

			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ2),
				P_Fld(0, B0_DQ2_RG_TX_ARDQM_OE_TIE_EN_B0) |
				P_Fld(0, B0_DQ2_RG_TX_ARDQM_OE_TIE_SEL_B0));
		}

		/* Set DRAM Byte 1 */
		if (is_byte_mode(p)) {
			/* Set DRAM Byte 0 */
			if (cp->dram_dq_b1) {
				dq_oe = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ2),
					B1_DQ2_RG_TX_ARDQ_OE_TIE_EN_B1);
				dq_oe &= ~(1 << (cp->dram_dq7_b1 % 8));
				vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ2),
					P_Fld(dq_oe, B1_DQ2_RG_TX_ARDQ_OE_TIE_EN_B1) |
					P_Fld(0, B1_DQ2_RG_TX_ARDQ_OE_TIE_SEL_B1));

				vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ2),
					P_Fld(0, B1_DQ2_RG_TX_ARDQM_OE_TIE_EN_B1) |
					P_Fld(0, B1_DQ2_RG_TX_ARDQM_OE_TIE_SEL_B1));
			} else {
				dq_oe = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ2),
					B0_DQ2_RG_TX_ARDQ_OE_TIE_EN_B0);
				dq_oe &= ~(0 << (cp->dram_dq7_b1 % 8));
				vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ2),
					P_Fld(dq_oe, B0_DQ2_RG_TX_ARDQ_OE_TIE_EN_B0) |
					P_Fld(0, B0_DQ2_RG_TX_ARDQ_OE_TIE_SEL_B0));

				vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ2),
					P_Fld(0, B0_DQ2_RG_TX_ARDQM_OE_TIE_EN_B0) |
					P_Fld(0, B0_DQ2_RG_TX_ARDQM_OE_TIE_SEL_B0));
			}
		}
	}
}

static void lp5_cbt_entry(DRAMC_CTX_T *p, u8 operating_fsp,
		u16 operation_frequency)
{
	lp5heff_save_disable(p);

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL),
		0, MISC_STBCAL_DQSIENCG_NORMAL_EN);

	/* TCMDEN and CATRAINEN use MRSRK */
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL0),
		p->rank, SWCMD_CTRL0_MRSRK);

	#if 0
	if (p->rank == RANK_0) {
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL),
			P_Fld(0, CKECTRL_CKEFIXOFF) |
			P_Fld(1, CKECTRL_CKEFIXON));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL),
			P_Fld(1, CKECTRL_CKE1FIXOFF) |
			P_Fld(0, CKECTRL_CKE1FIXON));
	} else if (p->rank == RANK_1) {
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL),
			P_Fld(0, CKECTRL_CKE1FIXOFF) |
			P_Fld(1, CKECTRL_CKE1FIXON));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL),
			P_Fld(1, CKECTRL_CKEFIXOFF) |
			P_Fld(0, CKECTRL_CKEFIXON));
	}
	#else
	if (p->rank == RANK_0) {
		CKEFixOnOff(p, RANK_0, CKE_FIXON, CKE_WRITE_TO_ONE_CHANNEL);
		CKEFixOnOff(p, RANK_1, CKE_FIXOFF, CKE_WRITE_TO_ONE_CHANNEL);
	} else if (p->rank == RANK_1){
		CKEFixOnOff(p, RANK_1, CKE_FIXON, CKE_WRITE_TO_ONE_CHANNEL);
		CKEFixOnOff(p, RANK_0, CKE_FIXOFF, CKE_WRITE_TO_ONE_CHANNEL);
	}
	#endif

	/*
	* APHY TX PI Spec mode option
	* for K RK1, if RK0/1 DQ UI setting is not the same, it will fail
	*/
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_NEW_XRW2W_CTRL),
		1, SHU_NEW_XRW2W_CTRL_TXPI_UPD_MODE);

	/*
	* APHY TX PI Spec mode option
	* for K RK1, if RK0/1 DQ UI setting is not the same, it will fail
	*/
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_NEW_XRW2W_CTRL),
		1, SHU_NEW_XRW2W_CTRL_TXPI_UPD_MODE);

	/*
	* APHY TX PI Spec mode option
	* for K RK1, if RK0/1 DQ UI setting is not the same, it will fail
	*/
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_NEW_XRW2W_CTRL),
		1, SHU_NEW_XRW2W_CTRL_TXPI_UPD_MODE);

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL0),
		P_Fld(0x1, CBT_WLEV_CTRL0_WRITE_LEVEL_EN));

	/*
	 * TODO
	 * BYTEMODE, PINMUX
	 */
	if (is_training_mode1(p)) {
		/* DRAMC B0 as RX */
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL0),
			0, CBT_WLEV_CTRL0_CBT_DQBYTE_OEAO_EN);
	}

	switch_oe_tie(p, 1);

	/*
	 * MR13 OP[6], cbt mode
	 * 0, training mode 1
	 * 1, training mode 2
	 *
	 * TODO
	 * MR13 values??
	 */
	DramcModeRegWriteByRank(p, p->rank, 13, p->lp5_training_mode << 6);

	if (operating_fsp == FSP_2) {
		/*
		 * dram will switch to another FSP_OP automatically
		 */
		DramcModeRegWriteByRank(p, p->rank, 16,
			(2 << MR16_FSP_WR_SHIFT) |
			(2 << MR16_FSP_OP_SHIFT) |
			(p->lp5_cbt_phase << MR16_CBT_PHASE) |
			/* CBT enabled fsp[2] */
			(3 << MR16_FSP_CBT) |
			(1 << MR16_VRCG));
	} else if (operating_fsp == FSP_1) {
		/*
		 * dram will switch to another FSP_OP automatically
		 */
		DramcModeRegWriteByRank(p, p->rank, 16,
			(1 << MR16_FSP_WR_SHIFT) |
			(1<< MR16_FSP_OP_SHIFT) |
			(p->lp5_cbt_phase << MR16_CBT_PHASE) |
			/* CBT enabled fsp[1] */
			(2 << MR16_FSP_CBT) |
			(1 << MR16_VRCG));
	} else {
		/* FSP_0 */
		DramcModeRegWriteByRank(p, p->rank, 16,
			(0 << MR16_FSP_WR_SHIFT) |
			(0 << MR16_FSP_OP_SHIFT) |
			(p->lp5_cbt_phase << MR16_CBT_PHASE) |
			/* CBT enabled fsp[0] */
			(1 << MR16_FSP_CBT) |
			(1 << MR16_VRCG));
	}

	/* wait tCBTWCKPRE_static */
	mcDELAY_US(1);

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL3),
		P_Fld(0x5, CBT_WLEV_CTRL3_DQSBX_G) |
		P_Fld(0x5, CBT_WLEV_CTRL3_DQSBY_G) |
		P_Fld(0x5, CBT_WLEV_CTRL3_DQSBX1_G) |
		P_Fld(0x5, CBT_WLEV_CTRL3_DQSBY1_G));

	if (is_byte_mode(p)) {
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL0), 3,
			CBT_WLEV_CTRL0_CBT_WLEV_DQS_SEL);
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL0), P_Fld(1, CBT_WLEV_CTRL0_BYTEMODECBTEN) |
			P_Fld(1, CBT_WLEV_CTRL0_CBT_CMP_BYTEMODE));    //BYTEMODECBTEN=1
	} else {
		if (lp5_cp[p->channel].dram_dq7_b0)
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL0), 0x2,
				CBT_WLEV_CTRL0_CBT_WLEV_DQS_SEL);
		else
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL0), 0x1,
				CBT_WLEV_CTRL0_CBT_WLEV_DQS_SEL);
	}

	/* toggle WCK */
	toggle_wck(p, 1);

	/* wait tWCK2DQ7H */
	mcDELAY_US(1);

	/* DQ[7] = High */
	force_dq7(p, 1);

	/* wait tDQ7HWCK to switch FSP */
	mcDELAY_US(1);

	/* stop toggle WCK */
	toggle_wck(p, 0);

	/* wait tDQ72DQ */
	mcDELAY_US(1);

	O1PathOnOff(p, 1);

	/* start toggle WCK */
	toggle_wck(p, 1);

	/* Wait tCAENT */
	mcDELAY_US(1);
}

static void lp5_cbt_exit(DRAMC_CTX_T *p, u8 operating_fsp,
		u8 operation_frequency)
{
	/* drive dq7 low */
	force_dq7(p, 0);

	/* wait tDQ7WCK */
	mcDELAY_US(1);

	/* stop wck toggle */
	toggle_wck(p, 0);

	/* wait tVREFCA_LOGNG */
	mcDELAY_US(1);

	if (operating_fsp == FSP_2) {
		DramcModeRegWriteByRank(p, p->rank, 16,
			(2 << MR16_FSP_WR_SHIFT) |
			(2 << MR16_FSP_OP_SHIFT) |
			(0 << MR16_CBT_PHASE) |
			/* normal operation */
			(0 << MR16_FSP_CBT) |
			(1 << MR16_VRCG));
	} else if (operating_fsp == FSP_1) {
		DramcModeRegWriteByRank(p, p->rank, 16,
			(1 << MR16_FSP_WR_SHIFT) |
			(1 << MR16_FSP_OP_SHIFT) |
			(0 << MR16_CBT_PHASE) |
			/* normal operation */
			(0 << MR16_FSP_CBT) |
			(1 << MR16_VRCG));
	} else {
		DramcModeRegWriteByRank(p, p->rank, 16,
			(0 << MR16_FSP_WR_SHIFT) |
			(0 << MR16_FSP_OP_SHIFT) |
			(0 << MR16_CBT_PHASE) |
			/* normal operation */
			(0 << MR16_FSP_CBT) |
			(1 << MR16_VRCG));
	}

	/* wait tMRD */
	mcDELAY_US(1);

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL0),
		P_Fld(0x0, CBT_WLEV_CTRL0_WRITE_LEVEL_EN));
	switch_oe_tie(p, 0);

	/*
	* APHY TX PI Spec mode option
	* for K RK1, if RK0/1 DQ UI setting is not the same, it will fail
	*/
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_NEW_XRW2W_CTRL),
		0, SHU_NEW_XRW2W_CTRL_TXPI_UPD_MODE);

	/* Disable O1 path output */
	O1PathOnOff(p, 0);

	if (is_byte_mode(p)) {
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL0), P_Fld(0, CBT_WLEV_CTRL0_BYTEMODECBTEN) |
			P_Fld(0, CBT_WLEV_CTRL0_CBT_CMP_BYTEMODE));    //BYTEMODECBTEN=1
	}

	#if 0
	if (p->rank == RANK_0) {
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL),
			P_Fld(0, CKECTRL_CKEFIXOFF) |
			P_Fld(0, CKECTRL_CKEFIXON));
	} else if (p->rank == RANK_1) {
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL),
			P_Fld(0, CKECTRL_CKE1FIXOFF) |
			P_Fld(0, CKECTRL_CKE1FIXON));
	}
	#else
	CKEFixOnOff(p, CKE_WRITE_TO_ALL_RANK, CKE_FIXON, CKE_WRITE_TO_ONE_CHANNEL);
	#endif

	lp5heff_restore(p);
}
#endif

static void CBTEntryLP45(DRAMC_CTX_T *p, U8 u1FSP, U16 u2Freq)
{
#if __LP5_COMBO__
	if (is_lp5_family(p))
	{
		lp5_cbt_entry(p, u1FSP, u2Freq);
	}
	else
#endif
	{
		if(p->dram_fsp == FSP_1)
		{
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_CA_CMD2), P_Fld(1, CA_CMD2_RG_TX_ARCMD_OE_DIS_CA)
						| P_Fld(0, CA_CMD2_RG_TX_ARCA_OE_TIE_SEL_CA)
						| P_Fld(0xff, CA_CMD2_RG_TX_ARCA_OE_TIE_EN_CA));
			cbt_switch_freq(p, CBT_LOW_FREQ);
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_CA_CMD2), P_Fld(0, CA_CMD2_RG_TX_ARCMD_OE_DIS_CA)
						| P_Fld(1, CA_CMD2_RG_TX_ARCA_OE_TIE_SEL_CA)
						| P_Fld(0xff, CA_CMD2_RG_TX_ARCA_OE_TIE_EN_CA));
		}
#if ENABLE_LP4Y_WA //@Darren, debugging for DFS stress
		CmdBusTrainingLP4YWA(p, DISABLE);
#endif
		CBTEntryLP4(p, u1FSP, u2Freq);
		if(p->dram_fsp == FSP_1)
		{
			cbt_switch_freq(p, CBT_HIGH_FREQ);
		}
	}
}

static void CBTExitLP45(DRAMC_CTX_T *p, U8 u1FSP, U8 u2Freq, U8 stateFlag)
{
	/* by yirong.wang
	 * if stateFlag == OUT_CBT, it means we finished CBT, exit CBT
	 * if stateFlag == IN_CBT, it means we are trying to setup vref by MRW
	 *	 IN_CBT case, only for LP5 mode 1 and LP4 byte mode
	 */
#if __LP5_COMBO__
	if (is_lp5_family(p))
	{
		if (stateFlag == OUT_CBT || is_training_mode1(p))
		{
			lp5_cbt_exit(p, u1FSP, u2Freq);
		}
	}
	else
#endif
	{
		if (stateFlag == OUT_CBT || p->dram_cbt_mode[p->rank] == CBT_BYTE_MODE1)
		{
			(p->dram_fsp == FSP_1)? cbt_switch_freq(p, CBT_LOW_FREQ): NULL;
			CBTExitLP4(p, u1FSP, u2Freq);
#if ENABLE_LP4Y_WA //@Darren, debugging for DFS stress
			CmdBusTrainingLP4YWA(p, ENABLE);
#endif
		}
	}
}

static void CBTSetVrefLP45(DRAMC_CTX_T *p, U8 u1VrefRange, U8 u1VrefLevel, U8 u1FSP, U16 u2Freq, U8 stateFlag)
{
	/* by yirong.wang
	 * if stateFlag == OUT_CBT, it means we are not in CBT, setup vref by MRW
	 * if stateFlag == IN_CBT, it means we are doing CBT
	 *	 LP5 training mode 1 and LP4 byte mode, exit CBT and setup vref by MRW, then re-enter CBT
	 *	 LP5 training mode 2 and LP4 normal mode, setup vref by DQ
	 */
#if __LP5_COMBO__
	if (is_lp5_family(p))
	{
		if (stateFlag == IN_CBT && is_training_mode2(p))
		{
			/*
			 * training mode2
			 * TODO, according to pinmux to adjust u1VrefLevel
			 */
			set_vref_by_dq(p, GetCBTVrefPinMuxValue_lp5(p, u1VrefLevel));
		}
		else
		{
			if (stateFlag == IN_CBT && is_training_mode1(p))
			{
				lp5_cbt_exit(p, u1FSP, u2Freq);
			}

			set_vref_by_mrw(p, u1VrefLevel);

			if (stateFlag == IN_CBT && is_training_mode1(p))
			{
				lp5_cbt_entry(p, u1FSP, u2Freq);
			}
		}
	}
	else
#endif
	{
		if (stateFlag == IN_CBT && p->dram_cbt_mode[p->rank] == CBT_BYTE_MODE1)
		{
			// BYTE MODE: We are not in CBT now, set Vref & enter CBT
			(p->dram_fsp == FSP_1)? cbt_switch_freq(p, CBT_LOW_FREQ): NULL;
			CBTExitLP4(p, u1FSP, u2Freq);

			CBTSetVrefLP4(p, u1VrefRange, u1VrefLevel, u1FSP, stateFlag);

			CBTEntryLP4(p, u1FSP, u2Freq);
			if(p->dram_fsp == FSP_1)
			{
				cbt_switch_freq(p, CBT_HIGH_FREQ);
			}
		}
		else
		{
			CBTSetVrefLP4(p, u1VrefRange, u1VrefLevel, u1FSP, stateFlag);
		}
	}
}

DRAM_STATUS_T CmdBusTrainingLP45(DRAMC_CTX_T *p, int autok)
{
	U8 u1FinalVref, u1FinalRange=0;
	S8 iFinalCACLK;
	U32 uiCAWinSumMax;
	U8 operating_fsp;
	U16 operation_frequency;
#if CA_PER_BIT_DELAY_CELL
	S16 iCAFinalCenter[CATRAINING_NUM] = {0}; //for CA_PER_BIT
#endif
#if ENABLE_EYESCAN_GRAPH
	U8 u1CBTEyeScanEnable;
	U8 EyeScan_index[CATRAINING_NUM];
#endif

	S16 pi_step;
	S16 pi_start, pi_end;
	u32 ca_ui, ca_ui_default;
	u32 ca_mck;
	u32 ca_cmd0;
	u8 ca_pin_num;
	u16 p2u;
	u8 step_respi = AUTOK_RESPI_1;

	U32 u4RegBackupAddress[] =
	{
		(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL)),
		(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL)),
		(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL)),
		(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ2)),
		(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ2)),
		(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL0)),
		(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL1)),
		(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL2)),
		(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL3)),
		(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL4)),
		(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL0)),
		(DRAMC_REG_ADDR(DRAMC_REG_REFCTRL0)),

		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_VREF)),			//in O1PathOnOff()
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_VREF)),			//in O1PathOnOff()
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_PHY_VREF_SEL)),	//in O1PathOnOff()
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_PHY_VREF_SEL)),	//in O1PathOnOff()
	};

	p2u = get_ca_pi_per_ui(p);

	pi_end = p2u * 2 - 1;

#if FOR_DV_SIMULATION_USED == 1
	pi_step = (vGet_DDR_Loop_Mode(p) == OPEN_LOOP_MODE) ? 16 : 8; //for simulation speed up
#else
	pi_step = get_capi_step(p);
#endif

	switch (p->freq_sel) {
	case LP5_DDR4266:
	case LP5_DDR800:
	case LP5_DDR1200:
	case LP5_DDR1600:
	case LP5_DDR3733:
	case LP5_DDR2400:
	case LP5_DDR3200:
	case LP5_DDR4800:
	case LP5_DDR5500:
	case LP5_DDR6000:
	case LP5_DDR6400:
		pi_start = -8;
		break;

	default:
		/* LPDDR4 */
		if (u1IsPhaseMode(p) == TRUE)
		{
			step_respi = AUTOK_RESPI_8;
		}

		#if CBT_MOVE_CA_INSTEAD_OF_CLK
			pi_start = -16;
			pi_end = p2u * 3 - 1;
		#else
			if (vGet_DDR_Loop_Mode(p) == SEMI_OPEN_LOOP_MODE)
			{
				pi_start = -24;
			}
			else if (vGet_DDR_Loop_Mode(p) == OPEN_LOOP_MODE)
			{
				pi_start = -16;
			}
			else
			{
				pi_start = -MAX_CLK_PI_DELAY;
			}
		#endif

		break;
	}

#if MRW_CHECK_ONLY
	mcSHOW_MRW_MSG(("\n==[MR Dump] %s==\n", __func__));
#endif

#if __LP5_COMBO__
	if (is_lp5_family(p))
	{
		u1FinalVref = u1MR12Value[p->channel][p->rank][p->dram_fsp];
		ca_pin_num = CATRAINING_NUM_LP5;
	}
	else
#endif
	{
		u1FinalRange = u1MR12Value[p->channel][p->rank][p->dram_fsp] >> 6;
		u1FinalVref = u1MR12Value[p->channel][p->rank][p->dram_fsp] & 0x3f;
		ca_pin_num = CATRAINING_NUM_LP4;
	}

#if ENABLE_EYESCAN_GRAPH
	u1CBTEyeScanEnable =GetEyeScanEnable(p, 0);

	for (u1vrefidx = 0; u1vrefidx < VREF_VOLTAGE_TABLE_NUM_LP5-1; u1vrefidx++)
	{
		for (uiCA = 0; uiCA < ca_pin_num; uiCA++)
		{
			for (ii = 0; ii < EYESCAN_BROKEN_NUM; ii++)
			{
				gEyeScan_Min[u1vrefidx][uiCA][ii] = EYESCAN_DATA_INVALID;
				gEyeScan_Max[u1vrefidx][uiCA][ii] = EYESCAN_DATA_INVALID;
			}
		}
	}
#endif

	vPrintCalibrationBasicInfo(p);
	mcSHOW_DBG_MSG(("pi_start=%d, pi_end=%d, pi_step=%d, new_cbt_mode=%d, autok=%d\n",
			pi_start, pi_end, pi_step, p->new_cbt_mode, autok));

#if __LP5_COMBO__
	if (is_lp5_family(p))
	{
		mcSHOW_DBG_MSG(("lp5_training_mode=%d, lp5_cbt_phase=%d\n", p->lp5_training_mode, p->lp5_cbt_phase));
	}
#endif

	//Back up dramC register
	DramcBackupRegisters(p, u4RegBackupAddress, ARRAY_SIZE(u4RegBackupAddress));

	//default set FAIL
	vSetCalibrationResult(p, DRAM_CALIBRATION_CA_TRAIN, DRAM_FAIL);

#if CA_PER_BIT_DELAY_CELL
	CATrainingSetPerBitDelayCell(p, iCAFinalCenter, ca_pin_num);
#endif

#if CBT_MOVE_CA_INSTEAD_OF_CLK
	if (u1IsLP4Family(p->dram_type))
	{
		U8 u1CaPI = 0, u1CaUI = 0;

		u1CaUI = 1;
		u1CaPI = 0;

		DramcCmdUIDelaySetting(p, u1CaUI);

		CBTDelayCACLK(p, u1CaPI);
	}
#endif

	/* read ca ui and mck */
	ca_ui_default = ca_ui = get_ca_ui(p);
	ca_mck = get_ca_mck(p);
	ca_cmd0 = u4IO32Read4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_CA_CMD0));

	vAutoRefreshSwitch(p, DISABLE); //When doing CA training, should make sure that auto refresh is disable

	/*
	 * TOOD
	 *
	 * here just pass simulation,
	 * remove after ACTiming OK(ACTiming Table includes CATRAIN_INTV)
	 */
	//vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL1),
	//		P_Fld(0x1F, CBT_WLEV_CTRL1_CATRAIN_INTV));
	set_cbt_wlev_intv(p);

	/*
	 * tx_rank_sel is selected by SW
	 * Lewis@20180509: tx_rank_sel is selected by SW in CBT if TMRRI design has changed.
	 */
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TX_SET0),
		p->rank, TX_SET0_TXRANK);
	/* TXRANKFIX should be write after TXRANK or the rank will be fix at rank 1 */
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TX_SET0),
		1, TX_SET0_TXRANKFIX);

	//SW variable initialization
	uiCAWinSumMax = 0;

	iFinalCACLK = 0;
	operating_fsp = p->dram_fsp;
	operation_frequency = p->frequency;

	// free-run dramc/ddrphy clk (DCMEN2=0, MIOCKCTRLOFF=1, PHYCLKDYNGEN=0, COMBCLKCTRL=0)
	// free-run dram clk(APHYCKCG_FIXOFF =1, TCKFIXON=1)
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL),
		P_Fld(0, DRAMC_PD_CTRL_DCMEN2) |
		P_Fld(1, DRAMC_PD_CTRL_MIOCKCTRLOFF) |
		P_Fld(0, DRAMC_PD_CTRL_PHYCLKDYNGEN) |
		P_Fld(0, DRAMC_PD_CTRL_COMBCLKCTRL) |
		P_Fld(1, DRAMC_PD_CTRL_APHYCKCG_FIXOFF) |
		P_Fld(1, DRAMC_PD_CTRL_TCKFIXON));

	//Note : Assume that there is a default CS value that can apply for CA.
	CBTEntryLP45(p, operating_fsp, operation_frequency);

#if PINMUX_AUTO_TEST_PER_BIT_CA
	CheckCADelayCell(p);
#endif

	//Step 3: set vref range and step by ddr type

#if (SUPPORT_SAVE_TIME_FOR_CALIBRATION && (BYPASS_VREF_CAL || BYPASS_CBT))
	if (p->femmc_Ready == 1)
	{
		u1FinalVref = p->pSavetimeData->u1CBTVref_Save[p->channel][p->rank];
	}
#endif

	mcSHOW_DBG_MSG(("\n[CmdBusTrainingLP45] Vref(ca) range %d: %d\n", u1FinalRange, u1FinalVref));

#ifdef FOR_HQA_TEST_USED
	gFinalCBTVrefCA[p->channel][p->rank] = u1FinalVref;
#endif

	//Set Vref after training
	// BYTE MODE: Set Vref & enter CBT
	CBTSetVrefLP45(p, u1FinalRange, u1FinalVref, operating_fsp, operation_frequency, IN_CBT);
#if (SUPPORT_SAVE_TIME_FOR_CALIBRATION && BYPASS_CBT)
	#if CBT_MOVE_CA_INSTEAD_OF_CLK
	// scan UI from 0, not from the UI we used to enter CBT
	DramcCmdUIDelaySetting(p, 0);
	ca_ui = get_ca_ui(p);
	#endif
#endif
	put_ca_ui(p, ca_ui);
	//Set CA_PI_Delay after training
	CBTSetCACLKResult(p, ca_mck, ca_ui, iFinalCACLK, ca_pin_num);

#if ENABLE_EYESCAN_GRAPH
	gEyeScan_CaliDelay[0] = CATrain_CmdDelay[p->channel][p->rank] -pi_start;
#endif

	//mcSHOW_DBG_MSG(("\nAverage CA Dly: %d\n", iFinalCACLK));

	/* -------------  CS and CLK ---------- */
	/* delay ca 1UI before K CS */
#if __LP5_COMBO__
	if (is_phase_falling(p)) {
		ca_mck = get_ca_mck(p);
		ca_ui = get_ca_ui(p);
		xlate_ca_mck_ui(p, 1,
				ca_mck, ca_ui,
				&ca_mck_tmp, &ca_ui_tmp);
		put_ca_mck(p, ca_mck_tmp);
		put_ca_ui(p, ca_ui_tmp);
	}
#endif

	CBTAdjustCS(p, autok);

	/* restore ca mck and ui */
#if __LP5_COMBO__
	if (is_phase_falling(p)) {
		put_ca_mck(p, ca_mck);
		put_ca_ui(p, ca_ui);
	}
#endif

//-------  Going to exit Command bus training(CBT) mode.-------------
	CBTExitLP45(p, operating_fsp, operation_frequency, OUT_CBT);
	CBTSetVrefLP45(p, u1FinalRange, u1FinalVref, operating_fsp, operation_frequency, OUT_CBT);

#if __LP5_COMBO__
	if (!is_lp5_family(p))
#endif
	{
		if (p->dram_fsp == FSP_1)
		{
			#if MR_CBT_SWITCH_FREQ
			DramcModeRegInit_CATerm(p, 0);
			#else
			DramcMRWriteFldAlign(p, 13, 1, MR13_FSP_OP, TO_MR);
			#endif
		}
	}

#if EYESCAN_LOG || defined(FOR_HQA_TEST_USED)
	gFinalCBTVrefDQ[p->channel][p->rank] = u1FinalVref;
#endif

	mcSHOW_DBG_MSG3(("\n[CmdBusTrainingLP45] Done\n"));

	//tx_rank_sel is selected by HW //Lewis@20180509: tx_rank_sel is selected by SW in CBT if TMRRI design has changed.
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TX_SET0), 0, TX_SET0_TXRANK);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TX_SET0), 0, TX_SET0_TXRANKFIX); //TXRANKFIX should be write after TXRANK or the rank will be fix at rank 1

	//Restore setting registers
	DramcRestoreRegisters(p, u4RegBackupAddress, ARRAY_SIZE(u4RegBackupAddress));

	return DRAM_OK;
}
#endif /* SIMUILATION_CBT */

//-------------------------------------------------------------------------
/** DramcWriteLeveling
 *	start Write Leveling Calibration.
 *	@param p				Pointer of context created by DramcCtxCreate.
 *	@param	apply			(U8): 0 don't apply the register we set  1 apply the register we set ,default don't apply.
 *	@retval status			(DRAM_STATUS_T): DRAM_OK or DRAM_FAIL
 */
//-------------------------------------------------------------------------
#define WRITE_LEVELING_MOVD_DQS 1//UI

U8 u1MCK2UI_DivShift(DRAMC_CTX_T *p)
{
#if (__LP5_COMBO__ == TRUE)
	if (TRUE == is_lp5_family(p))
	{
		//in LP4 1:8 mode, 8 small UI =  1 large UI
		if (vGet_Div_Mode(p) == DIV4_MODE)
		{
			return MCK_TO_4UI_SHIFT;
		}
		else if (vGet_Div_Mode(p) == DIV16_MODE)
		{
			return MCK_TO_16UI_SHIFT;
		}
		else
		{
			return MCK_TO_8UI_SHIFT;
		}
	}
	else
#endif
	{
		//in LP4 1:8 mode, 8 small UI =  1 large UI
		if (vGet_Div_Mode(p) == DIV4_MODE)
		{
			return MCK_TO_4UI_SHIFT;
		}
		else
		{
			return MCK_TO_8UI_SHIFT;
		}
	}
}

static DRAM_STATUS_T ExecuteMoveDramCDelay(DRAMC_CTX_T *p,
													REG_TRANSFER_T ui_reg,
													REG_TRANSFER_T mck_reg,
													S8 iShiftUI)
{
	S32 s4HighLevelDelay, s4DelaySum;
	U32 u4TmpUI, u4TmpMCK;
	U8 ucDataRateDivShift = 0;
	DRAM_STATUS_T MoveResult;

	ucDataRateDivShift = u1MCK2UI_DivShift(p);

	u4TmpUI = u4IO32ReadFldAlign(DRAMC_REG_ADDR(ui_reg.u4Addr), ui_reg.u4Fld) & (~(1 << ucDataRateDivShift));
	u4TmpMCK = u4IO32ReadFldAlign(DRAMC_REG_ADDR(mck_reg.u4Addr), mck_reg.u4Fld);
	//mcSHOW_DBG_MSG(("Base:  u4TmpMCK:%d,	u4TmpUI: %d,\n", u4TmpMCK, u4TmpUI));

	s4HighLevelDelay = (u4TmpMCK << ucDataRateDivShift) + u4TmpUI;
	s4DelaySum = (s4HighLevelDelay + iShiftUI);

	if (s4DelaySum < 0)
	{
		u4TmpUI = 0;
		u4TmpMCK = 0;
		MoveResult = DRAM_FAIL;
	}
	else
	{
		u4TmpMCK = s4DelaySum >> ucDataRateDivShift;
		u4TmpUI = s4DelaySum - (u4TmpMCK << ucDataRateDivShift);
		MoveResult = DRAM_OK;
	}

	vIO32WriteFldAlign(DRAMC_REG_ADDR(ui_reg.u4Addr), u4TmpUI, ui_reg.u4Fld);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(mck_reg.u4Addr), u4TmpMCK, mck_reg.u4Fld);
	//mcSHOW_DBG_MSG(("[%d]  Final ==> u4TmpMCK:%d,  u4TmpUI: %d,\n", iShiftUI, u4TmpMCK, u4TmpUI));

	return MoveResult;
}

static void _LoopAryToDelay(DRAMC_CTX_T *p,
								  REG_TRANSFER_T *ui_reg,
								  REG_TRANSFER_T *mck_reg,
								  U8 u8RG_num,
								  S8 iShiftUI,
								  BYTES_T eByteIdx)
{
	U8 idx = 0, step = 1;
	if (eByteIdx == BYTE_0)
	{
		idx = 0;
		step = 2;
	}
	else if (eByteIdx == BYTE_1)
	{
		idx = 1;
		step = 2;
	}

	for (; idx < u8RG_num; idx += step)
	{
		ExecuteMoveDramCDelay(p, ui_reg[idx], mck_reg[idx], iShiftUI);
	}
}

static void LP4_ShiftDQSUI(DRAMC_CTX_T *p, S8 iShiftUI, BYTES_T eByteIdx)
{
	// DQS / DQS_OEN
	REG_TRANSFER_T TransferUIRegs[]  = {{DRAMC_REG_SHU_SELPH_DQS1, SHU_SELPH_DQS1_DLY_DQS0},		// Byte0
										{DRAMC_REG_SHU_SELPH_DQS1, SHU_SELPH_DQS1_DLY_DQS1}};		// Byte1
	REG_TRANSFER_T TransferMCKRegs[] = {{DRAMC_REG_SHU_SELPH_DQS0, SHU_SELPH_DQS0_TXDLY_DQS0},
										{DRAMC_REG_SHU_SELPH_DQS0, SHU_SELPH_DQS0_TXDLY_DQS1}};

	_LoopAryToDelay(p, TransferUIRegs, TransferMCKRegs,
					   sizeof(TransferUIRegs) / sizeof(REG_TRANSFER_T),
					   iShiftUI, eByteIdx);
}

void LP4_ShiftDQS_OENUI(DRAMC_CTX_T *p, S8 iShiftUI, BYTES_T eByteIdx)
{
	// DQS / DQS_OEN
	REG_TRANSFER_T TransferUIRegs[]  = {{DRAMC_REG_SHU_SELPH_DQS1, SHU_SELPH_DQS1_DLY_OEN_DQS0},	// Byte0
										{DRAMC_REG_SHU_SELPH_DQS1, SHU_SELPH_DQS1_DLY_OEN_DQS1}};	// Byte1
	REG_TRANSFER_T TransferMCKRegs[] = {{DRAMC_REG_SHU_SELPH_DQS0, SHU_SELPH_DQS0_TXDLY_OEN_DQS0},
										{DRAMC_REG_SHU_SELPH_DQS0, SHU_SELPH_DQS0_TXDLY_OEN_DQS1}};

	_LoopAryToDelay(p, TransferUIRegs, TransferMCKRegs,
					   sizeof(TransferUIRegs) / sizeof(REG_TRANSFER_T),
					   iShiftUI, eByteIdx);
}

static void ShiftDQUI(DRAMC_CTX_T *p, S8 iShiftUI, BYTES_T eByteIdx)
{
	// Shift DQ / DQM / DQ_OEN / DQM_OEN
	REG_TRANSFER_T TransferUIRegs[]  = {{DRAMC_REG_SHURK_SELPH_DQ3, SHURK_SELPH_DQ3_DLY_DQM0},		// Byte0
										{DRAMC_REG_SHURK_SELPH_DQ3, SHURK_SELPH_DQ3_DLY_DQM1},		// Byte1
										{DRAMC_REG_SHURK_SELPH_DQ2, SHURK_SELPH_DQ2_DLY_DQ0},		// Byte0
										{DRAMC_REG_SHURK_SELPH_DQ2, SHURK_SELPH_DQ2_DLY_DQ1}};	// Byte1
	REG_TRANSFER_T TransferMCKRegs[] = {{DRAMC_REG_SHURK_SELPH_DQ1, SHURK_SELPH_DQ1_TXDLY_DQM0},
										{DRAMC_REG_SHURK_SELPH_DQ1, SHURK_SELPH_DQ1_TXDLY_DQM1},
										{DRAMC_REG_SHURK_SELPH_DQ0, SHURK_SELPH_DQ0_TXDLY_DQ0},
										{DRAMC_REG_SHURK_SELPH_DQ0, SHURK_SELPH_DQ0_TXDLY_DQ1}};

	_LoopAryToDelay(p, TransferUIRegs, TransferMCKRegs,
					sizeof(TransferUIRegs) / sizeof(REG_TRANSFER_T),
					iShiftUI, eByteIdx);
}

static void ShiftDQUI_AllRK(DRAMC_CTX_T *p, S8 iShiftUI, BYTES_T eByteIdx)
{
	U8 backup_rank, rk_i;
	backup_rank = u1GetRank(p);

	// Shift DQ / DQM / DQ_OEN / DQM_OEN
	for (rk_i = RANK_0; rk_i < p->support_rank_num; rk_i++)
	{
		vSetRank(p, rk_i);
		ShiftDQUI(p, iShiftUI, eByteIdx);
	}
	vSetRank(p, backup_rank);
}

static void ShiftDQ_OENUI(DRAMC_CTX_T *p, S8 iShiftUI, BYTES_T eByteIdx)
{
	REG_TRANSFER_T TransferUIRegs[]  = {{DRAMC_REG_SHURK_SELPH_DQ3, SHURK_SELPH_DQ3_DLY_OEN_DQM0},	// Byte0
										{DRAMC_REG_SHURK_SELPH_DQ3, SHURK_SELPH_DQ3_DLY_OEN_DQM1},	// Byte1
										{DRAMC_REG_SHURK_SELPH_DQ2, SHURK_SELPH_DQ2_DLY_OEN_DQ0},	// Byte0
										{DRAMC_REG_SHURK_SELPH_DQ2, SHURK_SELPH_DQ2_DLY_OEN_DQ1}};	// Byte1
	REG_TRANSFER_T TransferMCKRegs[] = {{DRAMC_REG_SHURK_SELPH_DQ1, SHURK_SELPH_DQ1_TXDLY_OEN_DQM0},
										{DRAMC_REG_SHURK_SELPH_DQ1, SHURK_SELPH_DQ1_TXDLY_OEN_DQM1},
										{DRAMC_REG_SHURK_SELPH_DQ0, SHURK_SELPH_DQ0_TXDLY_OEN_DQ0},
										{DRAMC_REG_SHURK_SELPH_DQ0, SHURK_SELPH_DQ0_TXDLY_OEN_DQ1}};

	_LoopAryToDelay(p, TransferUIRegs, TransferMCKRegs,
					sizeof(TransferUIRegs) / sizeof(REG_TRANSFER_T),
					iShiftUI, eByteIdx);
}

void ShiftDQ_OENUI_AllRK(DRAMC_CTX_T *p, S8 iShiftUI, BYTES_T eByteIdx)
{
	U8 backup_rank, rk_i;
	backup_rank = u1GetRank(p);

	// Shift DQ / DQM / DQ_OEN / DQM_OEN
	for (rk_i = RANK_0; rk_i < p->support_rank_num; rk_i++)
	{
		vSetRank(p, rk_i);
		ShiftDQ_OENUI(p, iShiftUI, eByteIdx);
	}
	vSetRank(p, backup_rank);
}

static void ShiftDQSWCK_UI(DRAMC_CTX_T *p, S8 iShiftUI, BYTES_T eByteIdx)
{
#if (__LP5_COMBO__ == TRUE)
		if (TRUE == is_lp5_family(p))
			LP5_ShiftWCKUI(p, iShiftUI, eByteIdx);
		else
#endif
		{
			LP4_ShiftDQSUI(p, iShiftUI, eByteIdx);
			LP4_ShiftDQS_OENUI(p, iShiftUI, eByteIdx);
		}
}

U8 u1IsLP4Div4DDR800(DRAMC_CTX_T *p)
{
	if ((vGet_Div_Mode(p) == DIV4_MODE) && (p->frequency == 400))
		return TRUE;
	else
		return FALSE;
}

//static void vSetDramMRWriteLevelingOnOff(DRAMC_CTX_T *p, U8 u1OnOff)
static void vSetDramMRWriteLevelingOnOff(DRAMC_CTX_T *p, U8 u1OnOff)
{
	// MR2 OP[7] to enable/disable write leveling
	if (u1OnOff)
		u1MR02Value[p->dram_fsp] |= 0x80;  // OP[7] WR LEV =1
	else
		u1MR02Value[p->dram_fsp] &= 0x7f;  // OP[7] WR LEV =0

	DramcModeRegWriteByRank(p, p->rank, 2, u1MR02Value[p->dram_fsp]);
}

U8 u1IsPhaseMode(DRAMC_CTX_T *p)
{
	if ((vGet_DDR_Loop_Mode(p) == OPEN_LOOP_MODE) || (vGet_DDR_Loop_Mode(p) == SEMI_OPEN_LOOP_MODE))
		return TRUE;
	else // DDR800_CLOSE_LOOP and NORMAL_CLOSE_LOOP
		return FALSE;
}

static DRAM_STATUS_T DramcTriggerAndWait(DRAMC_CTX_T *p, REG_TRANSFER_T TriggerReg, REG_TRANSFER_T RepondsReg)
{
//	  U32 u4TimeCnt = TIME_OUT_CNT;
	// @Darren, Rx HW AutoK simulation time
	U32 u4TimeCnt = DDR_HW_AUTOK_POLLING_CNT;
	DRAM_STATUS_T u4RespFlag = 0;

	vIO32WriteFldAlign(DRAMC_REG_ADDR(TriggerReg.u4Addr), 0, TriggerReg.u4Fld); // Init EN status
	vIO32WriteFldAlign(DRAMC_REG_ADDR(TriggerReg.u4Addr), 1, TriggerReg.u4Fld);
	do
	{
		u4RespFlag = u4IO32ReadFldAlign(DRAMC_REG_ADDR(RepondsReg.u4Addr), RepondsReg.u4Fld);
		u4TimeCnt --;
		mcDELAY_US(1);
	}while ((u4RespFlag == 0) && (u4TimeCnt > 0));

	if (u4TimeCnt == 0)//time out
	{
		mcSHOW_DBG_MSG(("[DramcTriggerAndWait] Wait 0x%x respond fail (time out)\n", RepondsReg.u4Addr));
		return DRAM_FAIL;
	}

	return DRAM_OK;
}

#if (SIMULATION_WRITE_LEVELING == 1)
#if !__ETT__
#undef ASSERT
#define ASSERT(x) \
		if (!(x)) \
			while (1)\
				mcSHOW_ERR_MSG(("ASSERT FAIL at %s[%d]!\n", __FUNCTION__, __LINE__));
#endif


#define DQPI_PER_UI (32)
#define STORAGED_DLY_UNIT (24)
static void WriteLevelingScanRange_PI(DRAMC_CTX_T *p, S32 *ps4DlyBegin, S32 *ps4DlyEnd, U8 *pu1PIStep, S16 *pPI_bound, WLEV_DELAY_BASED_T stDelayBase)
{
	S32 s4DlyBegin = 0, s4DlyEnd;
	U8 u1PIStep;
	S16 PI_bound;

	if (stDelayBase == PI_BASED)
	{
		// Giving PI scan range
		s4DlyBegin = WRITE_LEVELING_MOVD_DQS * 32 - MAX_CLK_PI_DELAY - 1;
		s4DlyEnd = s4DlyBegin + 64 - 1;

		if ((vGet_DDR_Loop_Mode(p) == OPEN_LOOP_MODE))
		{
			u1PIStep = 16;
			PI_bound = 32;
		}
		else if ((vGet_DDR_Loop_Mode(p) == SEMI_OPEN_LOOP_MODE))
		{
			u1PIStep = 8;
			PI_bound = 32;
		}
		else
		{
			u1PIStep = 1;
			PI_bound = 64;
		}
	}
	else // stDelayBase == DLY_BASED
	{
		// Giving delay cell scan range
		s4DlyBegin = 0;
		s4DlyEnd = 2 * STORAGED_DLY_UNIT;

		u1PIStep = 1;	 // One step is 1/4 delay cell
		PI_bound = 1024; // No bounadary as delay cell based
	}
	mcSHOW_DBG_MSG2(("Begin: %d, End: %d, Step: %d, Bound: %d\n", s4DlyBegin, s4DlyEnd, u1PIStep, PI_bound));

	*ps4DlyBegin = s4DlyBegin;
	*ps4DlyEnd = s4DlyEnd;
	*pu1PIStep = u1PIStep;
	*pPI_bound = PI_bound;

}

#if ENABLE_WDQS_MODE_2
DRAM_STATUS_T WriteLevelingPosCal(DRAMC_CTX_T *p, WLEV_DELAY_BASED_T stDelayBase)
{
	DRAM_RANK_T backup_rank = u1GetRank(p);
	U8 wrlevel_dqs_delay[DQS_NUMBER] = {0};
	U8 rank_i = 0;

	if((wrlevel_dqs_final_delay[RANK_0][0] - wrlevel_dqs_final_delay[RANK_1][0])>=9 ||
		(wrlevel_dqs_final_delay[RANK_0][0] - wrlevel_dqs_final_delay[RANK_1][0])<=-9 ||
		(wrlevel_dqs_final_delay[RANK_0][1] - wrlevel_dqs_final_delay[RANK_1][1])>=9 ||
		(wrlevel_dqs_final_delay[RANK_0][1] - wrlevel_dqs_final_delay[RANK_1][1])<=-9 )
	{
		mcSHOW_ERR_MSG(("[WARNING] Larger WL R2R !!\n"));
		#if CHECK_HQA_CRITERIA
		while(1);
		#endif
	}

	wrlevel_dqs_delay[0] = (wrlevel_dqs_final_delay[RANK_0][0] + wrlevel_dqs_final_delay[RANK_1][0]) >> 1;
	wrlevel_dqs_delay[1] = (wrlevel_dqs_final_delay[RANK_0][1] + wrlevel_dqs_final_delay[RANK_1][1]) >> 1;

	wrlevel_dqs_final_delay[RANK_0][0] = wrlevel_dqs_final_delay[RANK_1][0] = wrlevel_dqs_delay[0];
	wrlevel_dqs_final_delay[RANK_0][1] = wrlevel_dqs_final_delay[RANK_1][1] = wrlevel_dqs_delay[1];

	for (rank_i = p->rank; rank_i < p->support_rank_num; rank_i++)
	{
		vSetRank(p, rank_i);

		// set to best values for  DQS
		if (stDelayBase == PI_BASED)
		{
			// Adjust DQS output delay.
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_DQ0), wrlevel_dqs_delay[0], SHU_R0_B0_DQ0_ARPI_PBYTE_B0);
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_DQ0), wrlevel_dqs_delay[1], SHU_R0_B1_DQ0_ARPI_PBYTE_B1);
		}
		else // stDelayBase == DLY_BASED
		{
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_TXDLY3), wrlevel_dqs_delay[0], SHU_R0_B0_TXDLY3_TX_ARWCK_DLY_B0);
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_TXDLY3), wrlevel_dqs_delay[1], SHU_R0_B1_TXDLY3_TX_ARWCK_DLY_B1);
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_TXDLY3), wrlevel_dqs_delay[0], SHU_R0_B0_TXDLY3_TX_ARWCKB_DLY_B0);
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_TXDLY3), wrlevel_dqs_delay[1], SHU_R0_B1_TXDLY3_TX_ARWCKB_DLY_B1);
		}
	}

	vSetRank(p, backup_rank);

	mcSHOW_DBG_MSG(("[WriteLevelingPosCal] DQS PI B0/B1 = %d/%d\n", wrlevel_dqs_delay[0], wrlevel_dqs_delay[1]));
}
#endif

#define SET_PATTERN_MANUALLY_FOR_DEBUG 1
DRAM_STATUS_T DramcWriteLeveling(DRAMC_CTX_T *p, u8 isAutoK, WLEV_DELAY_BASED_T stDelayBase)
{
// Note that below procedure is based on "ODT off"
	DRAM_STATUS_T KResult = DRAM_FAIL;

	U8 byte_i, rank_i, ucDoneFlg;
	DRAM_RANK_T backup_rank;

	S32 wrlevel_dqs_delay[DQS_NUMBER]; // 3 is channel number

	S32 s4DlyBegin, s4DlyEnd;
	U8 u1PIStep;
	U8 u1OverBoundCnt = 0;
	S16 PI_bound = 64;

	//When doing WriteLeveling, should make sure that auto refresh is disable
	vAutoRefreshSwitch(p, DISABLE);

	// error handling
	if (!p)
	{
		mcSHOW_ERR_MSG(("context NULL\n"));
		return DRAM_FAIL;
	}

#if VENDER_JV_LOG
		vPrintCalibrationBasicInfo_ForJV(p);
#else
		vPrintCalibrationBasicInfo(p);
#endif


	fgwrlevel_done = 0;
	backup_rank = u1GetRank(p);

	//DramcRankSwap(p, p->rank);
	//tx_rank_sel is selected by SW //Lewis@20180604: tx_rank_sel is selected by SW in WL if TMRRI design has changed.
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TX_SET0), p->rank, TX_SET0_TXRANK);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TX_SET0), 1, TX_SET0_TXRANKFIX); //TXRANKFIX should be write after TXRANK

	// backup mode settings
	U32 u4RegBackupAddress[] =
	{
		(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL)),
		(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL0)),
		(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL1)),
		(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL3)),
		(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL5)),
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_VREF)),			//in O1PathOnOff()
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_VREF)),			//in O1PathOnOff()
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_PHY_VREF_SEL)),	//in O1PathOnOff()
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_PHY_VREF_SEL)),	//in O1PathOnOff()
		(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL))
	};
	DramcBackupRegisters(p, u4RegBackupAddress, sizeof(u4RegBackupAddress) / sizeof(U32));


	//default set DRAM FAIL
	vSetCalibrationResult(p, DRAM_CALIBRATION_WRITE_LEVEL, DRAM_FAIL);

#if MRW_CHECK_ONLY
	mcSHOW_MRW_MSG(("\n==[MR Dump] %s==\n", __func__));
#endif

	if (p->isWLevInitShift[p->channel] == FALSE)
	{
		// It must be PI_BASED or FAIL!!
		ASSERT(stDelayBase == PI_BASED);

		p->isWLevInitShift[p->channel] = TRUE;

		// This flow would be excuted just one time, so all ranks(maybe rank0/1) should be adjusted at once.
		ShiftDQUI_AllRK(p, -WRITE_LEVELING_MOVD_DQS, ALL_BYTES);
		ShiftDQ_OENUI_AllRK(p, -WRITE_LEVELING_MOVD_DQS, ALL_BYTES);
		ShiftDQSWCK_UI(p, -WRITE_LEVELING_MOVD_DQS, ALL_BYTES);

#if (__LP5_COMBO__ == TRUE)
		if (TRUE == is_lp5_family(p))
		{
			// For DLY based WCK leveling
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ13), 0, SHU_B0_DQ13_RG_TX_ARDQ_DLY_LAT_EN_B0);
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ13), 0, SHU_B1_DQ13_RG_TX_ARDQ_DLY_LAT_EN_B1);

			// Set DQS DLY-based delay to 16
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_TXDLY3), STORAGED_DLY_UNIT, SHU_R0_B0_TXDLY3_TX_ARWCK_DLY_B0);
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_TXDLY3), STORAGED_DLY_UNIT, SHU_R0_B1_TXDLY3_TX_ARWCK_DLY_B1);
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_TXDLY3), STORAGED_DLY_UNIT, SHU_R0_B0_TXDLY3_TX_ARWCKB_DLY_B0);
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_TXDLY3), STORAGED_DLY_UNIT, SHU_R0_B1_TXDLY3_TX_ARWCKB_DLY_B1);
		}
#endif
		// Set DQS PI-based delay to 0
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_DQ0), 0, SHU_R0_B0_DQ0_ARPI_PBYTE_B0);  //rank0, byte0, DQS delay
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_DQ0), 0, SHU_R0_B1_DQ0_ARPI_PBYTE_B1);  //rank0, byte1, DQS delay

	}

	// decide algorithm parameters according to freq.(PI mode/ phase mode)
	WriteLevelingScanRange_PI(p, &s4DlyBegin, &s4DlyEnd, &u1PIStep, &PI_bound, stDelayBase);

	// Not support autok to delay cell based mode.
	if (stDelayBase == DLY_BASED)
		isAutoK = FALSE;


#if (SUPPORT_SAVE_TIME_FOR_CALIBRATION && BYPASS_WRITELEVELING)
	if (p->femmc_Ready == 1)
	{
		wrlevel_dqs_final_delay[p->rank][0] = p->pSavetimeData->u1WriteLeveling_bypass_Save[p->channel][p->rank][0];
		wrlevel_dqs_final_delay[p->rank][1] = p->pSavetimeData->u1WriteLeveling_bypass_Save[p->channel][p->rank][1];

		ucDoneFlg = 0xff;
		KResult = DRAM_OK;
		vSetCalibrationResult(p, DRAM_CALIBRATION_WRITE_LEVEL, DRAM_FAST_K);
	}
#endif

	if (u1OverBoundCnt > 0)
		ShiftDQSWCK_UI(p, -u1OverBoundCnt * (PI_bound / DQPI_PER_UI), ALL_BYTES);

	if (ucDoneFlg == 0xff)
	{
		// all bytes are done
		fgwrlevel_done = 1;
		KResult = DRAM_OK;
	}
	else
	{
		KResult = DRAM_FAIL;
		#if __FLASH_TOOL_DA__
		PINInfo_flashtool.WL_ERR_FLAG|=(0x1<<(p->channel*2+p->rank));
		#endif
	}
	vSetCalibrationResult(p, DRAM_CALIBRATION_WRITE_LEVEL, KResult);
	mcSHOW_DBG_MSG2(("pass bytecount = 0x%x (0xff: all bytes pass) \n\n", ucDoneFlg));

#if defined(FOR_HQA_TEST_USED) && defined(FOR_HQA_REPORT_USED)
	if (gHQALog_flag == 1)
	{
		for (byte_i = 0; byte_i < (p->data_width / DQS_BIT_NUMBER); byte_i++)
		{
			HQA_Log_Message_for_Report(p, p->channel, p->rank, HQA_REPORT_FORMAT1, "", "WriteLeveling_DQS", byte_i, wrlevel_dqs_final_delay[p->rank][byte_i], NULL);
		}
	}
#endif

#if (__LP5_COMBO__ == TRUE)
	if (TRUE == is_lp5_family(p))

		vSetLP5Dram_WCK2CK_WlevOnOff(p, DISABLE);
	else
#endif
		vSetDramMRWriteLevelingOnOff(p, DISABLE); // Disable DDR write leveling mode:  issue MR2[7] to enable write leveling


	// Write leveling enable OFF
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL0), 0, CBT_WLEV_CTRL0_WRITE_LEVEL_EN);

	//Disable DQ_O1, SELO1ASO=0 for power saving
	O1PathOnOff(p, OFF);

	//tx_rank_sel is selected by HW
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TX_SET0), 0, TX_SET0_TXRANK);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TX_SET0), 0, TX_SET0_TXRANKFIX); //TXRANKFIX should be write after TXRANK

	//restore registers.
	DramcRestoreRegisters(p, u4RegBackupAddress, sizeof(u4RegBackupAddress) / sizeof(U32));

	// Calculate DQS "PI" delay, nothing to do with delay cell
	for (byte_i = 0; byte_i < (p->data_width / DQS_BIT_NUMBER); byte_i++)
	{
		mcSHOW_DBG_MSG(("Write leveling (Byte %d): %d", byte_i, wrlevel_dqs_final_delay[p->rank][byte_i]));
		mcDUMP_REG_MSG(("Write leveling (Byte %d): %d", byte_i, wrlevel_dqs_final_delay[p->rank][byte_i]));
		if (wrlevel_dqs_final_delay[p->rank][byte_i] >= PI_bound)
		{
			ShiftDQSWCK_UI(p, (wrlevel_dqs_final_delay[p->rank][byte_i] / PI_bound) * (PI_bound / DQPI_PER_UI), byte_i);

			wrlevel_dqs_final_delay[p->rank][byte_i] %= PI_bound;
		}

		wrlevel_dqs_delay[byte_i] = wrlevel_dqs_final_delay[p->rank][byte_i];
		mcSHOW_DBG_MSG((" => %d\n", wrlevel_dqs_delay[byte_i]));
		mcDUMP_REG_MSG((" => %d\n", wrlevel_dqs_delay[byte_i]));
	}

	for (rank_i = p->rank; rank_i < RANK_MAX; rank_i++)
	{
		vSetRank(p, rank_i);

		// set to best values for  DQS
		if (stDelayBase == PI_BASED)
		{
			// Adjust DQS output delay.
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_DQ0), wrlevel_dqs_delay[0], SHU_R0_B0_DQ0_ARPI_PBYTE_B0);
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_DQ0), wrlevel_dqs_delay[1], SHU_R0_B1_DQ0_ARPI_PBYTE_B1);
		}
		else // stDelayBase == DLY_BASED
		{
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_TXDLY3), wrlevel_dqs_delay[0], SHU_R0_B0_TXDLY3_TX_ARWCK_DLY_B0);
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_TXDLY3), wrlevel_dqs_delay[1], SHU_R0_B1_TXDLY3_TX_ARWCK_DLY_B1);
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_TXDLY3), wrlevel_dqs_delay[0], SHU_R0_B0_TXDLY3_TX_ARWCKB_DLY_B0);
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_TXDLY3), wrlevel_dqs_delay[1], SHU_R0_B1_TXDLY3_TX_ARWCKB_DLY_B1);
		}
	}

	vSetRank(p, backup_rank);

	mcSHOW_DBG_MSG3(("[DramcWriteLeveling] Done\n\n"));

	return KResult;
}
#endif //SIMULATION_WRITE_LEVELING

#if (SIMULATION_DUTY_CYC_MONITOR == 1)
static U8 FetchRGSettingVal(int step_val)
{
	if (step_val <= 0)
		return (U8)(-step_val);
	else
		return ((U8)step_val | 0x08);
}

DRAM_STATUS_T DramcDutyCycleMonitor(DRAMC_CTX_T *p)
{
	U8 backup_rank;
//	  U8 u8ResultDutyCycMonitor[WHOLE_STEPS_NUM] = {0};

	// error handling
	if (!p)
	{
		mcSHOW_ERR_MSG(("context NULL\n"));
		return DRAM_FAIL;
	}

	vAutoRefreshSwitch(p, DISABLE);
	//CKEFixOnOff(p, p->rank, CKE_FIXON, CKE_WRITE_TO_ONE_CHANNEL);

	backup_rank = u1GetRank(p);

	RunTime_SW_Cmd(p, RUNTIME_SWCMD_CAS_FS);

	int i = -7;
	for (i = -7; i <= 7; i++)
	{
		// MRW MR30 OP[7:4] = i(Set DCAU) and OP[3:0] = i(Set DCAL)
		U8 u8RGSettingVal = FetchRGSettingVal(i);
		mcSHOW_ERR_MSG(("Set value %d into MR30\n", u8RGSettingVal));
		MRWriteFldMulti(p, 30, P_Fld(u8RGSettingVal, MR30_DCAU) |
							   P_Fld(u8RGSettingVal, MR30_DCAL),
							   TO_MR);

		// Start duty cycle monitor
		DramcMRWriteFldAlign(p, 26, 1, MR26_DCM_START_STOP, TO_MR);

		// Delay tDCMM(2us)
		mcDELAY_US(2);

		// Duty cycle monitor Flip 0 -> 1, and store result of flip = 0
		DramcMRWriteFldAlign(p, 26, 1, MR26_DCM_FLIP, TO_MR);

		// Delay tDCMM(2us)
		mcDELAY_US(2);

		// Duty cycle monitor Flip 1 -> 0, and store result of flip = 1
		DramcMRWriteFldAlign(p, 26, 0, MR26_DCM_FLIP, TO_MR);

		// Delay tDCMM(2us)
		mcDELAY_US(2);

		// Stop Duty cycle monitor
		DramcMRWriteFldAlign(p, 26, 0, MR26_DCM_START_STOP, TO_MR);

		// Delay tMRD
		mcDELAY_US(2);

		mcSHOW_ERR_MSG(("Wait tMRD and MRR MR26\n"));

		///TODO:  Read back result MR25[5:2]
		// Store result into u8ResultDutyCycMonitor[]

	}
	///TODO:  Find and set a best MR30 variables

	RunTime_SW_Cmd(p, RUNTIME_SWCMD_CAS_OFF);

	vAutoRefreshSwitch(p, ENABLE);
	//CKEFixOnOff(p, p->rank, CKE_DYNAMIC, CKE_WRITE_TO_ONE_CHANNEL);

	vSetRank(p, backup_rank);
}
#endif // SIMULATION_DUTY_CYC_MONITOR

void vResetDelayChainBeforeCalibration(DRAMC_CTX_T *p)
{
	U8 u1RankIdx, u1RankIdxBak;
	U32 u4WbrBackup = GetDramcBroadcast();

	DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
	u1RankIdxBak = u1GetRank(p);

	for(u1RankIdx=RANK_0; u1RankIdx<RANK_MAX; u1RankIdx++)
	{
		vSetRank(p, u1RankIdx);

		vIO32WriteFldMulti_All(DDRPHY_REG_SHU_R0_CA_TXDLY0,
			P_Fld(0, SHU_R0_CA_TXDLY0_TX_ARCA0_DLY) |
			P_Fld(0, SHU_R0_CA_TXDLY0_TX_ARCA1_DLY) |
			P_Fld(0, SHU_R0_CA_TXDLY0_TX_ARCA2_DLY) |
			P_Fld(0, SHU_R0_CA_TXDLY0_TX_ARCA3_DLY));

		vIO32WriteFldMulti_All(DDRPHY_REG_SHU_R0_CA_TXDLY1,
			P_Fld(0, SHU_R0_CA_TXDLY1_TX_ARCA4_DLY) |
			P_Fld(0, SHU_R0_CA_TXDLY1_TX_ARCA5_DLY) |
			P_Fld(0, SHU_R0_CA_TXDLY1_TX_ARCA6_DLY) |
			P_Fld(0, SHU_R0_CA_TXDLY1_TX_ARCA7_DLY));

		vIO32WriteFldMulti_All(DDRPHY_REG_SHU_R0_B0_TXDLY0, P_Fld(0, SHU_R0_B0_TXDLY0_TX_ARDQ0_DLY_B0)
			| P_Fld(0, SHU_R0_B0_TXDLY0_TX_ARDQ1_DLY_B0)
			| P_Fld(0, SHU_R0_B0_TXDLY0_TX_ARDQ2_DLY_B0)
			| P_Fld(0, SHU_R0_B0_TXDLY0_TX_ARDQ3_DLY_B0));
		vIO32WriteFldMulti_All(DDRPHY_REG_SHU_R0_B0_TXDLY1, P_Fld(0, SHU_R0_B0_TXDLY1_TX_ARDQ4_DLY_B0)
			| P_Fld(0, SHU_R0_B0_TXDLY1_TX_ARDQ5_DLY_B0)
			| P_Fld(0, SHU_R0_B0_TXDLY1_TX_ARDQ6_DLY_B0)
			| P_Fld(0, SHU_R0_B0_TXDLY1_TX_ARDQ7_DLY_B0));

		vIO32WriteFldMulti_All(DDRPHY_REG_SHU_R0_B1_TXDLY0, P_Fld(0, SHU_R0_B1_TXDLY0_TX_ARDQ0_DLY_B1)
			| P_Fld(0, SHU_R0_B1_TXDLY0_TX_ARDQ1_DLY_B1)
			| P_Fld(0, SHU_R0_B1_TXDLY0_TX_ARDQ2_DLY_B1)
			| P_Fld(0, SHU_R0_B1_TXDLY0_TX_ARDQ3_DLY_B1));
		vIO32WriteFldMulti_All(DDRPHY_REG_SHU_R0_B1_TXDLY1, P_Fld(0, SHU_R0_B1_TXDLY1_TX_ARDQ4_DLY_B1)
			| P_Fld(0, SHU_R0_B1_TXDLY1_TX_ARDQ5_DLY_B1)
			| P_Fld(0, SHU_R0_B1_TXDLY1_TX_ARDQ6_DLY_B1)
			| P_Fld(0, SHU_R0_B1_TXDLY1_TX_ARDQ7_DLY_B1));
		vIO32WriteFldAlign_All(DDRPHY_REG_SHU_R0_B0_TXDLY3, 0x0, SHU_R0_B0_TXDLY3_TX_ARDQM0_DLY_B0);
		vIO32WriteFldAlign_All(DDRPHY_REG_SHU_R0_B1_TXDLY3, 0x0, SHU_R0_B1_TXDLY3_TX_ARDQM0_DLY_B1);
	}

	vSetRank(p, u1RankIdxBak);
	DramcBroadcastOnOff(u4WbrBackup);
}


//Reset PHY to prevent glitch when change DQS gating delay or RX DQS input delay
// [Lynx] Evere_st : cannot reset single channel. All DramC and All Phy have to reset together.
void DramPhyReset(DRAMC_CTX_T *p)
{
	// Evere_st change reset order : reset DQS before DQ, move PHY reset to final.
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_RX_SET0), 1, RX_SET0_RDATRST);// read data counter reset
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CTRL1), 1, MISC_CTRL1_R_DMPHYRST);

	//RG_ARCMD_RESETB & RG_ARDQ_RESETB_B0/1 only reset once at init, Justin Chan.
	///TODO: need to confirm RG_ARCMD_RESETB & RG_ARDQ_RESETB_B0/1 is reset at mem.c
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ9),
			P_Fld(0, B0_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B0) |
			P_Fld(0, B0_DQ9_RG_RX_ARDQ_STBEN_RESETB_B0));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ9),
			P_Fld(0, B1_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B1) |
			P_Fld(0, B1_DQ9_RG_RX_ARDQ_STBEN_RESETB_B1));
	mcDELAY_US(1);//delay 10ns
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ9),
			P_Fld(1, B1_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B1) |
			P_Fld(1, B1_DQ9_RG_RX_ARDQ_STBEN_RESETB_B1));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ9),
			P_Fld(1, B0_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B0) |
			P_Fld(1, B0_DQ9_RG_RX_ARDQ_STBEN_RESETB_B0));

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CTRL1), 0, MISC_CTRL1_R_DMPHYRST);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_RX_SET0), 0, RX_SET0_RDATRST);// read data counter reset
}

#if SIMULATION_LP4_ZQ
//-------------------------------------------------------------------------
/** DramcZQCalibration
 *	start Dram ZQ calibration.
 *	@param p				Pointer of context created by DramcCtxCreate.
 *	@retval status			(DRAM_STATUS_T): DRAM_OK or DRAM_FAIL
 */
//-------------------------------------------------------------------------
#if ZQ_SWCMD_MODE
static DRAM_STATUS_T ZQ_SWCMD_MODE_Cal(DRAMC_CTX_T *p, U8 rank)
{
	U32 u4Response;
	U32 u4TimeCnt = TIME_OUT_CNT;
	U32 u4SWCMDEN, u4SWCMDCTRL, u4SPDCTRL, u4CKECTRL;

	// Backup rank, CKE fix on/off, HW MIOCK control settings
	u4SWCMDEN = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN));
	u4SWCMDCTRL = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL0));
	u4SPDCTRL = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL));
	u4CKECTRL = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL));

	mcSHOW_DBG_MSG3(("[ZQCalibration]\n"));
	//mcFPRINTF((fp_A60501, "[ZQCalibration]\n"));

	// Disable HW MIOCK control to make CLK always on
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), 1, DRAMC_PD_CTRL_APHYCKCG_FIXOFF);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), 1, DRAMC_PD_CTRL_TCKFIXON);
	mcDELAY_US(1);

	//if CKE2RANK=1, only need to set CKEFIXON, it will apply to both rank.
	CKEFixOnOff(p, rank, CKE_FIXON, CKE_WRITE_TO_ONE_CHANNEL);

	//select rank
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL0), rank, SWCMD_CTRL0_SWTRIG_ZQ_RK);

	//ZQCAL Start
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), 1, SWCMD_EN_ZQCEN_SWTRIG);

	do
	{
		u4Response = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SPCMDRESP3), SPCMDRESP3_ZQC_SWTRIG_RESPONSE);
		u4TimeCnt --;
		mcDELAY_US(1);	// Wait tZQCAL(min) 1us or wait next polling

		mcSHOW_DBG_MSG3(("%d- ", u4TimeCnt));
		//mcFPRINTF((fp_A60501, "%d- ", u4TimeCnt));
	}while((u4Response==0) &&(u4TimeCnt>0));

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), 0, SWCMD_EN_ZQCEN_SWTRIG);

	if(u4TimeCnt==0)//time out
	{
		vSetCalibrationResult(p, DRAM_CALIBRATION_ZQ, DRAM_FAIL);
		mcSHOW_DBG_MSG(("ZQCAL Start fail (time out)\n"));
		//mcFPRINTF((fp_A60501, "ZQCAL Start fail (time out)\n"));
		return DRAM_FAIL;
	}

	// [JC] delay tZQCAL
	mcDELAY_US(1);
	u4TimeCnt = TIME_OUT_CNT;

	//ZQCAL Latch
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), 1, SWCMD_EN_ZQLATEN_SWTRIG);
	do
	{
		u4Response = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SPCMDRESP3), SPCMDRESP3_ZQLAT_SWTRIG_RESPONSE);
		u4TimeCnt --;
		mcDELAY_US(1);// Wait tZQLAT 30ns or wait next polling

		mcSHOW_DBG_MSG3(("%d=", u4TimeCnt));
		//mcFPRINTF((fp_A60501, "%d= ", u4TimeCnt));
	}while((u4Response==0) &&(u4TimeCnt>0));

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), 0, SWCMD_EN_ZQLATEN_SWTRIG);

	if(u4TimeCnt==0)//time out
	{
		vSetCalibrationResult(p, DRAM_CALIBRATION_ZQ, DRAM_FAIL);
		mcSHOW_DBG_MSG(("ZQCAL Latch fail (time out)\n"));
		//mcFPRINTF((fp_A60501, "ZQCAL Latch fail (time out)\n"));
		return DRAM_FAIL;
	}

	// [JC] delay tZQLAT
	mcDELAY_US(1);

	// Restore rank, CKE fix on, HW MIOCK control settings
	vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), u4SWCMDEN);
	vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL0), u4SWCMDCTRL);
	vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), u4SPDCTRL);
	vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL), u4CKECTRL);

	vSetCalibrationResult(p, DRAM_CALIBRATION_ZQ, DRAM_OK);
	mcSHOW_DBG_MSG3(("\n[DramcZQCalibration] Done\n\n"));
	//mcFPRINTF((fp_A60501, "\n[DramcZQCalibration] Done\n\n"));

	return DRAM_OK;
}
#endif
#if ZQ_RTSWCMD_MODE
DRAM_STATUS_T ZQ_RTSWCMD_MODE_Cal(DRAMC_CTX_T *p, U8 rank)
{
	U32 u4Response;
	U32 u4TimeCnt = TIME_OUT_CNT;
	U32 u4SWCMDEN, u4SWCMDCTRL, u4MPCCTRL, u4RTSWCMD, u4SPDCTRL, u4CKECTRL;

	// Backup rank, CKE fix on/off, HW MIOCK control settings
	u4SWCMDEN = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN));
	u4SWCMDCTRL = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL2));
	u4MPCCTRL = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_MPC_CTRL));
	u4RTSWCMD = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_RTSWCMD_CNT));
	u4SPDCTRL = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL));
	u4CKECTRL = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL));

	mcSHOW_DBG_MSG3(("[ZQCalibration]\n"));
	//mcFPRINTF((fp_A60501, "[ZQCalibration]\n"));

	// Disable HW MIOCK control to make CLK always on
	//vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), 1, DRAMC_PD_CTRL_APHYCKCG_FIXOFF);
	//vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), 1, DRAMC_PD_CTRL_TCKFIXON);
	mcDELAY_US(1);

	//if CKE2RANK=1, only need to set CKEFIXON, it will apply to both rank.
	//CKEFixOnOff(p, rank, CKE_FIXON, CKE_WRITE_TO_ONE_CHANNEL);

	//select rank
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL2),
		P_Fld(rank, SWCMD_CTRL2_RTSWCMD_RK) |
		P_Fld(0x20, SWCMD_CTRL2_RTSWCMD_AGE));

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_MPC_CTRL), 0x1, MPC_CTRL_RTSWCMD_HPRI_EN);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_RTSWCMD_CNT), 0x2a, RTSWCMD_CNT_RTSWCMD_CNT);

	//ZQCAL Start
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), 0x5, SWCMD_EN_RTSWCMD_SEL);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), 1, SWCMD_EN_RTSWCMDEN);

	do
	{
		u4Response = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SPCMDRESP3), SPCMDRESP3_RTSWCMD_RESPONSE);
		u4TimeCnt --;
		mcDELAY_US(1);	// Wait tZQCAL(min) 1us or wait next polling

		mcSHOW_DBG_MSG3(("%d- ", u4TimeCnt));
		//mcFPRINTF((fp_A60501, "%d- ", u4TimeCnt));
	}while((u4Response==0) &&(u4TimeCnt>0));

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), 0, SWCMD_EN_RTSWCMDEN);

	if(u4TimeCnt==0)//time out
	{
		vSetCalibrationResult(p, DRAM_CALIBRATION_ZQ, DRAM_FAIL);
		mcSHOW_DBG_MSG(("ZQCAL Start fail (time out)\n"));
		//mcFPRINTF((fp_A60501, "ZQCAL Start fail (time out)\n"));
		return DRAM_FAIL;
	}

	// [JC] delay tZQCAL
	mcDELAY_US(1);
	u4TimeCnt = TIME_OUT_CNT;

	//ZQCAL Latch
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), 0x6, SWCMD_EN_RTSWCMD_SEL);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), 1, SWCMD_EN_RTSWCMDEN);

	do
	{
		u4Response = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SPCMDRESP3), SPCMDRESP3_RTSWCMD_RESPONSE);
		u4TimeCnt --;
		mcDELAY_US(1);// Wait tZQLAT 30ns or wait next polling

		mcSHOW_DBG_MSG3(("%d=", u4TimeCnt));
		//mcFPRINTF((fp_A60501, "%d= ", u4TimeCnt));
	}while((u4Response==0) &&(u4TimeCnt>0));

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), 0, SWCMD_EN_RTSWCMDEN);

	if(u4TimeCnt==0)//time out
	{
		vSetCalibrationResult(p, DRAM_CALIBRATION_ZQ, DRAM_FAIL);
		mcSHOW_DBG_MSG(("ZQCAL Latch fail (time out)\n"));
		//mcFPRINTF((fp_A60501, "ZQCAL Latch fail (time out)\n"));
		return DRAM_FAIL;
	}

	// [JC] delay tZQLAT
	mcDELAY_US(1);

	// Restore rank, CKE fix on, HW MIOCK control settings
	vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), u4SWCMDEN);
	vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL2), u4SWCMDCTRL);
	vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_MPC_CTRL), u4MPCCTRL);
	vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_RTSWCMD_CNT), u4RTSWCMD);
	vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), u4SPDCTRL);
	vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL), u4CKECTRL);

	vSetCalibrationResult(p, DRAM_CALIBRATION_ZQ, DRAM_OK);
	mcSHOW_DBG_MSG3(("\n[DramcZQCalibration] Done\n\n"));
	//mcFPRINTF((fp_A60501, "\n[DramcZQCalibration] Done\n\n"));

	return DRAM_OK;
}
#endif
#if ZQ_SCSM_MODE
DRAM_STATUS_T ZQ_SCSM_MODE_Cal(DRAMC_CTX_T *p, U8 rank)
{
	U32 u4Response;
	U32 u4TimeCnt = TIME_OUT_CNT;
	U32 u4SWCMDEN, u4MPCCTRL, u4SWCMDCTRL, u4SPDCTRL, u4CKECTRL;

	// Backup rank, CKE fix on/off, HW MIOCK control settings
	u4SWCMDEN = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN));
	u4SWCMDCTRL = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL0));
	u4MPCCTRL = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_MPC_OPTION));
	u4SPDCTRL = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL));
	u4CKECTRL = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL));

	mcSHOW_DBG_MSG3(("[ZQCalibration]\n"));
	//mcFPRINTF((fp_A60501, "[ZQCalibration]\n"));

	// Disable HW MIOCK control to make CLK always on
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), 1, DRAMC_PD_CTRL_APHYCKCG_FIXOFF);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), 1, DRAMC_PD_CTRL_TCKFIXON);
	mcDELAY_US(1);

	//if CKE2RANK=1, only need to set CKEFIXON, it will apply to both rank.
	CKEFixOnOff(p, rank, CKE_FIXON, CKE_WRITE_TO_ONE_CHANNEL);

	//Use rank swap or MRSRK to select rank
	//DramcRankSwap(p, p->rank);
	//!!R_DMMRSRK(R_DMMPCRKEN=1) specify rank0 or rank1
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL0), rank, SWCMD_CTRL0_MRSRK);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_MPC_OPTION), 1, MPC_OPTION_MPCRKEN);

	//ZQCAL Start
	//R_DMZQCEN, 0x1E4[4]=1 for ZQCal Start
	//Wait zqc_response=1 (dramc_conf_nao, 0x3b8[4])
	//R_DMZQCEN, 0x1E4[4]=0
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), 1, SWCMD_EN_ZQCEN);
	do
	{
		u4Response = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SPCMDRESP), SPCMDRESP_ZQC_RESPONSE);
		u4TimeCnt --;
		mcDELAY_US(1);	// Wait tZQCAL(min) 1us or wait next polling

		mcSHOW_DBG_MSG3(("%d- ", u4TimeCnt));
		//mcFPRINTF((fp_A60501, "%d- ", u4TimeCnt));
	}while((u4Response==0) &&(u4TimeCnt>0));

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), 0, SWCMD_EN_ZQCEN);

	if(u4TimeCnt==0)//time out
	{
		vSetCalibrationResult(p, DRAM_CALIBRATION_ZQ, DRAM_FAIL);
		mcSHOW_DBG_MSG(("ZQCAL Start fail (time out)\n"));
		//mcFPRINTF((fp_A60501, "ZQCAL Start fail (time out)\n"));
		return DRAM_FAIL;
	}

	// [JC] delay tZQCAL
	mcDELAY_US(1);
	u4TimeCnt = TIME_OUT_CNT;

	//ZQCAL Latch
	//R_DMZQLATEN, 0x1E4[6]=1 for ZQCal latch
	//Wait zqlat_response=1 (dramc_conf_nao, 0x3b8[28])
	//R_DMZQLATEN, 0x1E4[6]=0
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), 1, SWCMD_EN_ZQLATEN);
	do
	{
		u4Response = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SPCMDRESP), SPCMDRESP_ZQLAT_RESPONSE);
		u4TimeCnt --;
		mcDELAY_US(1);// Wait tZQLAT 30ns or wait next polling

		mcSHOW_DBG_MSG3(("%d=", u4TimeCnt));
		//mcFPRINTF((fp_A60501, "%d= ", u4TimeCnt));
	}while((u4Response==0) &&(u4TimeCnt>0));

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), 0, SWCMD_EN_ZQLATEN);

	if(u4TimeCnt==0)//time out
	{
		vSetCalibrationResult(p, DRAM_CALIBRATION_ZQ, DRAM_FAIL);
		mcSHOW_DBG_MSG(("ZQCAL Latch fail (time out)\n"));
		//mcFPRINTF((fp_A60501, "ZQCAL Latch fail (time out)\n"));
		return DRAM_FAIL;
	}

	// [JC] delay tZQLAT
	mcDELAY_US(1);

	// Restore rank, CKE fix on, HW MIOCK control settings
	vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), u4SWCMDEN);
	vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL2), u4SWCMDCTRL);
	vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_MPC_CTRL), u4MPCCTRL);
	vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), u4SPDCTRL);
	vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL), u4CKECTRL);

	vSetCalibrationResult(p, DRAM_CALIBRATION_ZQ, DRAM_OK);
	mcSHOW_DBG_MSG3(("\n[DramcZQCalibration] Done\n\n"));
	//mcFPRINTF((fp_A60501, "\n[DramcZQCalibration] Done\n\n"));

	return DRAM_OK;
}
#endif

DRAM_STATUS_T DramcZQCalibration(DRAMC_CTX_T *p, U8 rank)
{
	#if ZQ_SWCMD_MODE
	return ZQ_SWCMD_MODE_Cal(p, rank);
	#elif ZQ_RTSWCMD_MODE
	return ZQ_RTSWCMD_MODE_Cal(p, rank);
	#else //ZQ_SCSM_MODE
	return ZQ_SCSM_MODE_Cal(p, rank);
	#endif
}
#endif

#if (SIMULATION_GATING == 1)
#define GATING_PATTERN_NUM_LP5		0x23
#define GATING_GOLDEND_DQSCNT_LP5	0x4646
#define RXDQS_GATING_AUTO_DBG_REG_NUM		6
/* Preamble & Postamble setting. Currently use macro to define.
 * Later may use speed or MR setting to decide
 * !!! REVIEW !!!
 */

#if GATING_ADJUST_TXDLY_FOR_TRACKING
U8 u1TXDLY_Cal_min =0xff, u1TXDLY_Cal_max=0;
U8 ucbest_coarse_mck_backup[RANK_MAX][DQS_NUMBER];
U8 ucbest_coarse_ui_backup[RANK_MAX][DQS_NUMBER];
U8 ucbest_coarse_mck_P1_backup[RANK_MAX][DQS_NUMBER];
U8 ucbest_coarse_ui_P1_backup[RANK_MAX][DQS_NUMBER];
#endif


struct rxdqs_gating_cal {
	U8 dqsien_dly_mck;
	U8 dqsien_dly_ui;
	U8 dqsien_dly_pi;

	U8 dqsien_dly_mck_p1;
	U8 dqsien_dly_ui_p1;

	U8 dqsien_pi_adj_step;

	U8 dqsien_pi_per_ui;
	U8 dqsien_ui_per_mck;
	U8 dqsien_freq_div;
};

struct rxdqs_gating_trans {
	U8 dqs_lead[DQS_NUMBER];
	U8 dqs_lag[DQS_NUMBER];
	U8 dqs_high[DQS_NUMBER];
#if GATING_LEADLAG_LOW_LEVEL_CHECK
	U8 dqs_low[DQS_NUMBER];
#endif
	U8 dqs_transition[DQS_NUMBER];
	U8 dqs_transitioned[DQS_NUMBER];
	U8 dqsien_dly_mck_leadlag[DQS_NUMBER];
	U8 dqsien_dly_ui_leadlag[DQS_NUMBER];
	U8 dqsien_dly_pi_leadlag[DQS_NUMBER];
};

struct rxdqs_gating_best_win {
	U8 best_dqsien_dly_mck[DQS_NUMBER];
	U8 best_dqsien_dly_ui[DQS_NUMBER];
	U8 best_dqsien_dly_pi[DQS_NUMBER];
	U8 best_dqsien_dly_mck_p1[DQS_NUMBER];
	U8 best_dqsien_dly_ui_p1[DQS_NUMBER];
	U8 best_dqsien_dly_pi_p1[DQS_NUMBER];
};

struct rxdqs_gating_auto_param {
	U8 early_break;
	U8 dbg_mode;

	U8 init_mck;
	U8 init_ui;
	U8 end_mck;
	U8 end_ui;
	U8 pi_offset;

	U8 burst_len;
};

#define ENABLE_GATING_AUTOK_WA			1

#if ENABLE_GATING_AUTOK_WA
U8 __wa__gating_swk_for_autok = 0;
U8 __wa__gating_autok_init_ui[RANK_MAX] = { 0 };
#endif

#if (__LP5_COMBO__)
static U8 u1GetLp5ReadLatency(DRAMC_CTX_T *p)
{
	U8 read_latency;
	U8 rl, ckr, dvfsc;

	const U8 au1MR2MappingToRL_wo_dvfsc[2][12] = {
		{3, 4, 5, 6, 8, 9, 10, 12, 13, 14, 15, 17}, /* CKR = 4:1 */
		{6, 8, 10, 12, 16, 18}, /* CKR = 2:1 */
	};

	///TODO: Spec has not specify these values
	const U8 au1MR2MappingToRL_wi_dvfsc[2][6] = {
		{0xff, 0xff, 0xff, 0xff, 0xff, 0xff}, /* CKR = 4:1 */
		{0xff, 0xff, 0xff, 0xff, 0xff, 0xff}, /* CKR = 2:1 */
	};

	ckr = (u1MR18Value[p->dram_fsp] >> 7) & 0x1;
	dvfsc = !!(u1MR19Value[p->dram_fsp] & 0x3);
	rl = (u1MR02Value[p->dram_fsp] & 0xf);

	if (dvfsc)
		read_latency = au1MR2MappingToRL_wi_dvfsc[ckr][rl];
	else
		read_latency = au1MR2MappingToRL_wo_dvfsc[ckr][rl];

	/* note that the uint of RL is nCK, convert to nWCK */
	if (ckr == 0)
		read_latency *= 4;
	else
		read_latency *= 2;

	mcSHOW_DBG_MSG(("ckr = %d, dvfsc = %d, rl = %d, read_latency = %d\n",
		ckr, dvfsc, rl, read_latency));

	return read_latency;
}
#endif

static U8 u1GetGatingStartPos(DRAMC_CTX_T *p, U8 u1AutoK)
{
	const U8 au1MR2MappingToRL[2][8] = {{6, 10, 14, 20, 24, 28, 32, 36},   //normal mode
										{6, 10, 16, 22, 26, 32, 36, 40}};  //byte mode
	U8 u1MR0_LatencyMode;
	U8 u1MR2RLValue;

	u1MR2RLValue = u1MR02Value[p->dram_fsp] & 0x7; //MR2 Op[2:0]
	U8 u1RX_Path_delay_UI, u1RealRL,u1StartUI, u1ExtraMCKfor1_4mode;
	U8 u1MCK2CK_UI, u1ReadDQSINCTL, u1DQSINCTL_UI;
	U8 u4TDQSCK_UI_min;
	U8 u1GatingAheadDQS_UI;

	/* LPDDR5 uses same bit */
	if(gu2MR0_Value[p->rank] == 0xffff)  //MR0 is not ready
	{
		u1MR0_LatencyMode = CBT_NORMAL_MODE;
	}
	else
	{
		u1MR0_LatencyMode = (gu2MR0_Value[p->rank]>>1) & 0x1; //MR0 OP[1],	0:normal mode,	1:byte mode
	}

#if (__LP5_COMBO__)
	if (is_lp5_family(p)) {
		u4TDQSCK_UI_min = 500 * p->frequency *2/ 1000000;
		u1RealRL = u1GetLp5ReadLatency(p);
	} else
#endif
	{
		u4TDQSCK_UI_min = 1500 * p->frequency *2/ 1000000;
		u1RealRL = au1MR2MappingToRL[u1MR0_LatencyMode][u1MR2RLValue];
	}

	///TODO: A60868 does not support LP5 DIV4, current setting is not provided for LP5
	if(vGet_Div_Mode(p) == DIV4_MODE)
	{
		u1MCK2CK_UI = 4;
		u1ExtraMCKfor1_4mode = 1;
		u1GatingAheadDQS_UI = 3;
	}
	else if (vGet_Div_Mode(p) == DIV8_MODE)
	{
		u1MCK2CK_UI = 8;
		u1ExtraMCKfor1_4mode = 0;
#if (__LP5_COMBO__)
	if (is_lp5_family(p)) {
		if (p->frequency <= 1600)
			u1GatingAheadDQS_UI = 1 * u1MCK2CK_UI;
		else if (p->frequency == 1866)
			u1GatingAheadDQS_UI = 4;
		else
			u1GatingAheadDQS_UI = 8;
	} else
#endif
		u1GatingAheadDQS_UI = 5;
	}
	else
	{
		/* DIV16, only for LP5 */
		u1MCK2CK_UI = 16;
		u1ExtraMCKfor1_4mode = 0;
		u1GatingAheadDQS_UI = 8;
	}

	// RX_Path_delay_UI = RL*2 + tDQSCK_UI<1500~3500ps> - PHY_interanl<skip 30ps> - GatingAheadDQS<2UI> + if(1:4 mod)+1MCK
	u1RX_Path_delay_UI = (u1RealRL<<1) + u4TDQSCK_UI_min - u1GatingAheadDQS_UI + (u1MCK2CK_UI*u1ExtraMCKfor1_4mode);

	u1ReadDQSINCTL = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_RK_DQSCTL), MISC_SHU_RK_DQSCTL_DQSINCTL);
	u1DQSINCTL_UI = u1ReadDQSINCTL * u1MCK2CK_UI;

	if(u1AutoK)
		u1RX_Path_delay_UI += 0; //HW K start position =  gating min position(1500ns)
	else
		u1RX_Path_delay_UI -= 3; //SW K start position = gating min position(1500ns) -3UI

	if(u1RX_Path_delay_UI >= u1DQSINCTL_UI)
		u1StartUI = u1RX_Path_delay_UI - u1DQSINCTL_UI;
	else
	{
		u1StartUI =0;
		mcSHOW_ERR_MSG(("GatingStartPos err! Need to fine-tune default DQSINCTL value.\n(RX_Path_delay_UI %d) < DQSINCTL_UI %d)\n", u1RX_Path_delay_UI, u1DQSINCTL_UI));
		#if __ETT__
		while(1);
		#endif
	}

	mcSHOW_DBG_MSG(("[GatingStartPos] MR0_LatencyMode %d, u1RealRL %d , u4TDQSCK_UI_min %d, 1:4ExtraMCK %d\n", u1MR0_LatencyMode, u1RealRL, u4TDQSCK_UI_min, u1ExtraMCKfor1_4mode));
	mcDUMP_REG_MSG(("[GatingStartPos] MR0_LatencyMode %d, u1RealRL %d , u4TDQSCK_UI_min %d, 1:4ExtraMCK %d\n", u1MR0_LatencyMode, u1RealRL, u4TDQSCK_UI_min, u1ExtraMCKfor1_4mode));

	if(u1AutoK)
	{
		mcSHOW_DBG_MSG(("RX_Path_delay_UI(%d) - DQSINCTL_UI(%d) = u1StartUI(%d)\n", u1RX_Path_delay_UI, u1DQSINCTL_UI, u1StartUI));
		mcDUMP_REG_MSG(("RX_Path_delay_UI(%d) - DQSINCTL_UI(%d) = u1StartUI(%d)\n", u1RX_Path_delay_UI, u1DQSINCTL_UI, u1StartUI));
	}
	else
	{
		mcSHOW_DBG_MSG(("RX_Path_delay_UI(%d) -3 - DQSINCTL_UI(%d) = u1StartUI(%d)\n", u1RX_Path_delay_UI, u1DQSINCTL_UI, u1StartUI));
		mcDUMP_REG_MSG(("RX_Path_delay_UI(%d) -3 - DQSINCTL_UI(%d) = u1StartUI(%d)\n", u1RX_Path_delay_UI, u1DQSINCTL_UI, u1StartUI));
	}

	return u1StartUI;
}

#if GATING_RODT_LATANCY_EN
U8 get_rodt_mck2ui(DRAMC_CTX_T *p)
{
	if (vGet_Div_Mode(p) == DIV16_MODE)
		return 8;
	else if (vGet_Div_Mode(p) == DIV8_MODE)
		return 4;
	else
		return 2;
}
#endif

static u8 rxdqs_gating_bypass(DRAMC_CTX_T *p)
{
#if SUPPORT_SAVE_TIME_FOR_CALIBRATION && BYPASS_GatingCal
		if (p->femmc_Ready == 1) {
			mcSHOW_DBG_MSG(("[FAST_K] Bypass Gating Calibration\n"));
			return 1;
		}
#endif

	return 0;
}

static void rxdqs_gating_fastk_save_restore(DRAMC_CTX_T *p,
	struct rxdqs_gating_best_win *best_win,
	struct rxdqs_gating_cal *gating_cal)
{
#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
		u8 ui_per_mck = gating_cal->dqsien_ui_per_mck;
		u8 freq_div = gating_cal->dqsien_freq_div;
		u8 ch = p->channel;
		u8 rk = p->rank;
		u8 dqs_i;

		if (p->femmc_Ready == 1) {
			for (dqs_i = 0; dqs_i < p->data_width/DQS_BIT_NUMBER; dqs_i++) {
				best_win->best_dqsien_dly_mck[dqs_i] =
					p->pSavetimeData->u1Gating_MCK_Save[ch][rk][dqs_i];
				best_win->best_dqsien_dly_ui[dqs_i] =
					p->pSavetimeData->u1Gating_UI_Save[ch][rk][dqs_i];
				best_win->best_dqsien_dly_pi[dqs_i] =
					p->pSavetimeData->u1Gating_PI_Save[ch][rk][dqs_i];

				/* Calculate P1 */
				best_win->best_dqsien_dly_ui_p1[dqs_i] =
					best_win->best_dqsien_dly_mck[dqs_i] * ui_per_mck +
					best_win->best_dqsien_dly_ui[dqs_i] + freq_div; /* Total UI for Phase1 */
				best_win->best_dqsien_dly_mck_p1[dqs_i] =
					best_win->best_dqsien_dly_ui_p1[dqs_i] / ui_per_mck;
				best_win->best_dqsien_dly_ui_p1[dqs_i] =
					best_win->best_dqsien_dly_ui_p1[dqs_i] % ui_per_mck;

				vSetCalibrationResult(p, DRAM_CALIBRATION_GATING, DRAM_FAST_K);

				mcSHOW_DBG_MSG(("[FAST_K] CH%d RK%d best DQS%d dly(MCK, UI, PI) = (%d, %d, %d)\n",
					ch, rk, dqs_i, best_win->best_dqsien_dly_mck[dqs_i],
					best_win->best_dqsien_dly_ui[dqs_i],
					best_win->best_dqsien_dly_pi[dqs_i]));
				mcSHOW_DBG_MSG(("[FAST_K] CH%d RK%d best DQS%d P1 dly(MCK, UI, PI) = (%d, %d, %d)\n",
					ch, rk, dqs_i, best_win->best_dqsien_dly_mck_p1[dqs_i],
					best_win->best_dqsien_dly_ui_p1[dqs_i],
					best_win->best_dqsien_dly_pi_p1[dqs_i]));

			}
		}
#endif
}

static void rxdqs_gating_misc_process(DRAMC_CTX_T *p,
	struct rxdqs_gating_best_win *rxdqs_best_win)
{
#if GATING_ADJUST_TXDLY_FOR_TRACKING
	U8 u1TX_dly_DQSgated = 0;
#endif
	U8 dqs_i;

	/* Set result of useless bytes (if any) as 0. */
	for (dqs_i = (p->data_width/DQS_BIT_NUMBER); dqs_i < DQS_NUMBER; dqs_i++) {
		rxdqs_best_win->best_dqsien_dly_mck[dqs_i] =
			rxdqs_best_win->best_dqsien_dly_ui[dqs_i] =
			rxdqs_best_win->best_dqsien_dly_pi[dqs_i]= 0;
		rxdqs_best_win->best_dqsien_dly_mck_p1[dqs_i] =
			rxdqs_best_win->best_dqsien_dly_ui_p1[dqs_i] =
			rxdqs_best_win->best_dqsien_dly_pi_p1[dqs_i]= 0;

#if GATING_ADJUST_TXDLY_FOR_TRACKING
		ucbest_coarse_mck_backup[p->rank][dqs_i] =
			ucbest_coarse_ui_backup[p->rank][dqs_i] = 0;
		ucbest_coarse_mck_P1_backup[p->rank][dqs_i] =
			ucbest_coarse_ui_P1_backup[p->rank][dqs_i] = 0;
#endif
	}

	for (dqs_i=0; dqs_i<(p->data_width/DQS_BIT_NUMBER); dqs_i++) {
#ifdef FOR_HQA_REPORT_USED
		HQA_Log_Message_for_Report(p, p->channel, p->rank, HQA_REPORT_FORMAT6, "DQSINCTL ", "", 0,
			u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_RK_DQSCTL), MISC_SHU_RK_DQSCTL_DQSINCTL), NULL);
		HQA_Log_Message_for_Report(p, p->channel, p->rank, HQA_REPORT_FORMAT0,
			"Gating_Center_", "2T", dqs_i, rxdqs_best_win->best_dqsien_dly_mck[dqs_i], NULL);
		HQA_Log_Message_for_Report(p, p->channel, p->rank, HQA_REPORT_FORMAT0,
			"Gating_Center_", "05T", dqs_i, rxdqs_best_win->best_dqsien_dly_ui[dqs_i], NULL);
		HQA_Log_Message_for_Report(p, p->channel, p->rank, HQA_REPORT_FORMAT0,
			"Gating_Center_", "PI", dqs_i, rxdqs_best_win->best_dqsien_dly_pi[dqs_i], NULL);
#endif

		/*TINFO="best DQS%d delay(2T, 0.5T, PI) = (%d, %d, %d)\n", dqs_i, rxdqs_best_win.best_dqsien_dly_mck[dqs_i], rxdqs_best_win.best_dqsien_dly_ui[dqs_i], rxdqs_best_win.best_dqsien_dly_pi[dqs_i])); */
		mcSHOW_DBG_MSG(("best DQS%d dly(MCK, UI, PI) = (%d, %d, %d)\n", dqs_i,
			rxdqs_best_win->best_dqsien_dly_mck[dqs_i],
			rxdqs_best_win->best_dqsien_dly_ui[dqs_i],
			rxdqs_best_win->best_dqsien_dly_pi[dqs_i]));
		mcDUMP_REG_MSG(("best DQS%d dly(MCK, UI, PI) = (%d, %d, %d)\n", dqs_i,
			rxdqs_best_win->best_dqsien_dly_mck[dqs_i],
			rxdqs_best_win->best_dqsien_dly_ui[dqs_i],
			rxdqs_best_win->best_dqsien_dly_pi[dqs_i]));
		/* cc mark mcFPRINTF((fp_A60501,"best DQS%d dly(MCK, UI, PI) = (%d, %d, %d)\n", dqs_i,
			rxdqs_best_win.best_dqsien_dly_mck[dqs_i],
			rxdqs_best_win.best_dqsien_dly_ui[dqs_i],
			rxdqs_best_win.best_dqsien_dly_pi[dqs_i]));
		*/

#if GATING_ADJUST_TXDLY_FOR_TRACKING
	u1TX_dly_DQSgated = (rxdqs_best_win->best_dqsien_dly_mck[dqs_i] << 4) +
		rxdqs_best_win->best_dqsien_dly_ui[dqs_i];

	if (vGet_Div_Mode(p) == DIV16_MODE)
		u1TX_dly_DQSgated >>= 4;
	else if (vGet_Div_Mode(p) == DIV8_MODE)
		u1TX_dly_DQSgated >>= 3;
	else
		u1TX_dly_DQSgated >>= 2;

	if (u1TX_dly_DQSgated < u1TXDLY_Cal_min)
		u1TXDLY_Cal_min = u1TX_dly_DQSgated;

	ucbest_coarse_ui_backup[p->rank][dqs_i] = rxdqs_best_win->best_dqsien_dly_ui[dqs_i];
	ucbest_coarse_mck_backup[p->rank][dqs_i] = rxdqs_best_win->best_dqsien_dly_mck[dqs_i];
#endif
	}

	mcSHOW_DBG_MSG(("\n"));
	//cc mark mcFPRINTF((fp_A60501,"\n"));

	for (dqs_i=0; dqs_i<(p->data_width/DQS_BIT_NUMBER); dqs_i++) {
		/*TINFO="best DQS%d P1 delay(2T, 0.5T, PI) = (%d, %d, %d)\n", dqs_i, rxdqs_best_win.best_dqsien_dly_mck_p1[dqs_i], rxdqs_best_win.best_dqsien_dly_ui_p1[dqs_i], rxdqs_best_win.best_dqsien_dly_pi_p1[dqs_i]*/
		mcSHOW_DBG_MSG(("best DQS%d P1 dly(MCK, UI, PI) = (%d, %d, %d)\n", dqs_i,
			rxdqs_best_win->best_dqsien_dly_mck_p1[dqs_i],
			rxdqs_best_win->best_dqsien_dly_ui_p1[dqs_i],
			rxdqs_best_win->best_dqsien_dly_pi_p1[dqs_i]));
		mcDUMP_REG_MSG(("best DQS%d P1 dly(MCK, UI, PI) = (%d, %d, %d)\n", dqs_i,
			rxdqs_best_win->best_dqsien_dly_mck_p1[dqs_i],
			rxdqs_best_win->best_dqsien_dly_ui_p1[dqs_i],
			rxdqs_best_win->best_dqsien_dly_pi_p1[dqs_i]));
		/* cc mark mcFPRINTF((fp_A60501,"best DQS%d P1 dly(2T, 0.5T, PI) = (%d, %d, %d)\n", dqs_i,
			rxdqs_best_win.best_dqsien_dly_mck_p1[dqs_i],
			rxdqs_best_win.best_dqsien_dly_ui_p1[dqs_i],
			rxdqs_best_win.best_dqsien_dly_pi_p1[dqs_i]));
		*/

#if GATING_ADJUST_TXDLY_FOR_TRACKING
		// find max gating TXDLY (should be in P1)
		u1TX_dly_DQSgated = (rxdqs_best_win->best_dqsien_dly_mck_p1[dqs_i] << 4) +
			rxdqs_best_win->best_dqsien_dly_ui_p1[dqs_i];

		if (vGet_Div_Mode(p) == DIV16_MODE)
			u1TX_dly_DQSgated >>= 4;
		else if (vGet_Div_Mode(p) == DIV8_MODE)
			u1TX_dly_DQSgated >>= 3;
		else
			u1TX_dly_DQSgated >>= 2;

		if(u1TX_dly_DQSgated > u1TXDLY_Cal_max)
			u1TXDLY_Cal_max = u1TX_dly_DQSgated;

		ucbest_coarse_ui_P1_backup[p->rank][dqs_i] = rxdqs_best_win->best_dqsien_dly_ui_p1[dqs_i];
		ucbest_coarse_mck_P1_backup[p->rank][dqs_i] = rxdqs_best_win->best_dqsien_dly_mck_p1[dqs_i];
#endif
	}

#if RDSEL_TRACKING_EN
	//Byte 0
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_INI_UIPI),
		(ucbest_coarse_mck_backup[p->rank][0] << 4) | (ucbest_coarse_ui_backup[p->rank][0]),
		SHU_R0_B0_INI_UIPI_CURR_INI_UI_B0);//UI
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_INI_UIPI), rxdqs_best_win->best_dqsien_dly_pi[0],
		SHU_R0_B0_INI_UIPI_CURR_INI_PI_B0); //PI
	//Byte 1
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_INI_UIPI),
		(ucbest_coarse_mck_backup[p->rank][1] << 4) | (ucbest_coarse_ui_backup[p->rank][1]),
		SHU_R0_B1_INI_UIPI_CURR_INI_UI_B1);//UI
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_INI_UIPI),
		rxdqs_best_win->best_dqsien_dly_pi[1], SHU_R0_B1_INI_UIPI_CURR_INI_PI_B1); //PI
#endif

}

static void rxdqs_gating_auto_cal_reset(DRAMC_CTX_T *p)
{
	/* Reset internal autok status and logic */
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DQSIEN_AUTOK_CFG0),
		P_Fld(0x1, MISC_DQSIEN_AUTOK_CFG0_DQSIEN_AUTOK_RK0_SW_RST) |
		P_Fld(0x1, MISC_DQSIEN_AUTOK_CFG0_DQSIEN_AUTOK_RK1_SW_RST) |
		P_Fld(0x1, MISC_DQSIEN_AUTOK_CFG0_DQSIEN_AUTOK_SW_RST));

	mcDELAY_US(1);

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DQSIEN_AUTOK_CFG0),
		P_Fld(0x0, MISC_DQSIEN_AUTOK_CFG0_DQSIEN_AUTOK_RK0_SW_RST) |
		P_Fld(0x0, MISC_DQSIEN_AUTOK_CFG0_DQSIEN_AUTOK_RK1_SW_RST) |
		P_Fld(0x0, MISC_DQSIEN_AUTOK_CFG0_DQSIEN_AUTOK_SW_RST));
}
static void rxdqs_gating_auto_cal_cfg(DRAMC_CTX_T *p,
	struct rxdqs_gating_auto_param *auto_param)
{
	/* Before start calibration, reset all state machine and all rank's state */
	rxdqs_gating_auto_cal_reset(p);


	/*-----------
	 * Normal Setting, Same as SW calibration
	 *---------------*/
	if (p->frequency == 800) {
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL1),
			0x1, MISC_STBCAL1_STBCNT_SW_RST);
	}

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL1),
		0x1, MISC_STBCAL1_STBCNT_SHU_RST_EN);

	/* SELPH_MODE = BY RANK */
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL2),
		0x1, MISC_STBCAL2_DQSIEN_SELPH_BY_RANK_EN);

	if (p->dram_type == TYPE_LPDDR5) {
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL2),
			0x1, MISC_STBCAL2_STB_PICG_EARLY_1T_EN);
	}

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL1),
		0x1, MISC_STBCAL1_DIS_PI_TRACK_AS_NOT_RD);

	/* PICG_EARLY_EN */
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ6),
		0x1, B0_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B0);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ6),
		0x1, B0_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B0);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL2),
		0x1, MISC_STBCAL2_STB_PICG_EARLY_1T_EN);

	/* BURST_MODE */
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_STBCAL),
		0x1, MISC_SHU_STBCAL_DQSIEN_BURST_MODE);

#if (__LP5_COMBO__)
	if (is_lp5_family(p)) {
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ9),
			0x1, B0_DQ9_RG_RX_ARDQS0_DQSIENMODE_B0);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ9),
			0x1, B1_DQ9_RG_RX_ARDQS0_DQSIENMODE_B1);
	} else
#endif
	{
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ9),
			0x1, B0_DQ9_RG_RX_ARDQS0_DQSIENMODE_B0);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ9),
			0x1, B1_DQ9_RG_RX_ARDQS0_DQSIENMODE_B1);
	}

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ6),
		0x2, B0_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B0);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ6),
		0x2, B1_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B1);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL),
		0x1, MISC_STBCAL_DQSIENMODE);

	/* New Rank Mode */
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL2),
		P_Fld(0x1, MISC_STBCAL2_STB_IG_XRANK_CG_RST) |
		P_Fld(0x1, MISC_STBCAL2_STB_RST_BY_RANK) |
		P_Fld(0x1, MISC_STBCAL2_DQSIEN_SELPH_BY_RANK_EN));

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B0_PHY2),
		0x1, B0_PHY2_RG_RX_ARDQS_DQSIEN_UI_LEAD_LAG_EN_B0);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B1_PHY2),
		0x1, B1_PHY2_RG_RX_ARDQS_DQSIEN_UI_LEAD_LAG_EN_B1);

	/* dummy read */
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DUMMY_RD),
		0x1, DUMMY_RD_DUMMY_RD_PA_OPT);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CG_CTRL0),
		0x1, MISC_CG_CTRL0_RG_CG_PHY_OFF_DIABLE);

	//Yulia add workaround for auto K pattern length. : Apply for all project before IPM_V2
	//Dummy read BL should be controlled by DQSIEN_AUTOK_BURST_LENGTH, but now we can only use dummy read length(DMY_RD_LEN)
	//DMY_RD_LEN (0 for BL8, 1 for BL16, 3 for BL32)
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_RK_DUMMY_RD_ADR), 3/*auto_param->burst_len*/, RK_DUMMY_RD_ADR_DMY_RD_LEN);

	/* Decide by HW  Although Dummy read used, but TA2 has higher priority */
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A4),
		0x4, TEST2_A4_TESTAGENTRKSEL);

	//vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL2), 1,
	//	MISC_STBCAL2_STBENCMPEN);

	/*-----------
	 * Auto calibration setting
	 *-------------------*/
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DQSIEN_AUTOK_CFG0),
		P_Fld(auto_param->init_mck, MISC_DQSIEN_AUTOK_CFG0_DQSIEN_AUTOK_INI_MCK) |
		P_Fld(auto_param->init_ui, MISC_DQSIEN_AUTOK_CFG0_DQSIEN_AUTOK_INI__UI) |
		P_Fld(auto_param->end_mck, MISC_DQSIEN_AUTOK_CFG0_DQSIEN_AUTOK_END_MCK) |
		P_Fld(auto_param->end_ui, MISC_DQSIEN_AUTOK_CFG0_DQSIEN_AUTOK_END__UI) |
		P_Fld(auto_param->pi_offset, MISC_DQSIEN_AUTOK_CFG0_DQSIEN_AUTOK_PI_OFFSET) |
		P_Fld(p->rank, MISC_DQSIEN_AUTOK_CFG0_DQSIEN_AUTOK_CUR_RANK) |
		P_Fld(auto_param->burst_len, MISC_DQSIEN_AUTOK_CFG0_DQSIEN_AUTOK_BURST_LENGTH) |
		P_Fld(0x1, MISC_DQSIEN_AUTOK_CFG0_DQSIEN_AUTOK_B0_EN) |
		P_Fld(0x1, MISC_DQSIEN_AUTOK_CFG0_DQSIEN_AUTOK_B1_EN));

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DQSIEN_AUTOK_CFG0),
		auto_param->dbg_mode, MISC_DQSIEN_AUTOK_CFG0_DQSIEN_AUTOK_DEBUG_MODE_EN);

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DQSIEN_AUTOK_CFG0),
		auto_param->early_break, MISC_DQSIEN_AUTOK_CFG0_DQSIEN_AUTOK_EARLY_BREAK_EN);

	/*---------
	 * DV settings
	 *-------------------*/
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL),
		0x0, MISC_STBCAL_PICGEN);
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_STBCAL),
		P_Fld(0x0, MISC_SHU_STBCAL_STBCALEN) |
		P_Fld(0x0, MISC_SHU_STBCAL_STB_SELPHCALEN));

	mcSHOW_DBG_MSG(("[Gating] AUTO K with param:\n"));
	mcSHOW_DBG_MSG(("\tinit_mck: %d, init_ui: %d, end_mck: %d, end_ui: %d\n",
		auto_param->init_mck, auto_param->init_ui,
		auto_param->end_mck, auto_param->end_ui));
	mcSHOW_DBG_MSG(("\tpi_offset: %d, early_break: %s\n", auto_param->pi_offset,
		(auto_param->early_break)? "ENABLE" : "DISABLE"));
}

static void rxdqs_gating_auto_cal_trigger(DRAMC_CTX_T *p)
{
	mcSHOW_DBG_MSG(("[Gating] AUTO K start...\n"));
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DQSIEN_AUTOK_CFG0),
		0x1, MISC_DQSIEN_AUTOK_CFG0_DQSIEN_AUTOK_GO);
}

static void rxdqs_gating_auto_cal_stop(DRAMC_CTX_T *p)
{
	mcSHOW_DBG_MSG(("[Gating] AUTO K stop...\n"));
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DQSIEN_AUTOK_CFG0),
		0x0, MISC_DQSIEN_AUTOK_CFG0_DQSIEN_AUTOK_GO);

	rxdqs_gating_auto_cal_reset(p);
}


static void rxdqs_gating_set_final_result(DRAMC_CTX_T *p, U8 mck2ui,
	struct rxdqs_gating_best_win *best_win)
{
#if GATING_RODT_LATANCY_EN
	U8 reg_mck, reg_ui;
	U8 value;
	U8 reg_mck_rodt[DQS_NUMBER], reg_ui_rodt[DQS_NUMBER];
	U8 reg_mck_rodt_p1[DQS_NUMBER], reg_ui_rodt_p1[DQS_NUMBER];
	U8 dqs_i;
#endif

#if GATING_RODT_LATANCY_EN
	for (dqs_i = 0; dqs_i < (p->data_width / DQS_BIT_NUMBER); dqs_i++) {
		reg_mck = best_win->best_dqsien_dly_mck[dqs_i];
		reg_ui = best_win->best_dqsien_dly_ui[dqs_i];

		value = (reg_mck * mck2ui) + reg_ui;

		if (value >= 11) {
			U8 rodt_mck2ui = get_rodt_mck2ui(p);

			value -= 11;
			reg_mck_rodt[dqs_i] = value / rodt_mck2ui;
			reg_ui_rodt[dqs_i] = value % rodt_mck2ui;

			reg_mck_rodt_p1[dqs_i] = reg_mck_rodt[dqs_i];
			reg_ui_rodt_p1[dqs_i] = reg_ui_rodt[dqs_i];
		} else {

			reg_mck_rodt[dqs_i] = 0;
			reg_ui_rodt[dqs_i] = 0;
			reg_mck_rodt_p1[dqs_i] = 4;
			reg_ui_rodt_p1[dqs_i] = 4;
			mcSHOW_DBG_MSG(("[Warning] RODT cannot be -11UI for B%d\n",
				dqs_i));
		}

		mcSHOW_DBG_MSG(("DQS%d Final RODTEN: (%2d, %2d)\n",
			dqs_i, reg_mck_rodt[dqs_i], reg_ui_rodt[dqs_i]));
		mcSHOW_DBG_MSG(("DQS%d Final RODTEN_P1: (%2d, %2d)\n",
			dqs_i, reg_mck_rodt_p1[dqs_i], reg_ui_rodt_p1[dqs_i]));
	}
#endif

	/* Set DQSIEN delay in MCK and UI */
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B0_DQSIEN_MCK_UI_DLY),
		P_Fld(best_win->best_dqsien_dly_mck[0],
		SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B0) |
		P_Fld(best_win->best_dqsien_dly_ui[0],
		SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B0) |
		P_Fld(best_win->best_dqsien_dly_mck_p1[0],
		SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B0) |
		P_Fld(best_win->best_dqsien_dly_ui_p1[0],
		SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B0));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B1_DQSIEN_MCK_UI_DLY),
		P_Fld(best_win->best_dqsien_dly_mck[1],
		SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B1) |
		P_Fld(best_win->best_dqsien_dly_ui[1],
		SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B1) |
		P_Fld(best_win->best_dqsien_dly_mck_p1[1],
		SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B1) |
		P_Fld(best_win->best_dqsien_dly_ui_p1[1],
		SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B1));

#if GATING_RODT_LATANCY_EN
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B0_RODTEN_MCK_UI_DLY),
		P_Fld(reg_mck_rodt[0],
		SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B0) |
		P_Fld(reg_ui_rodt[0],
		SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B0) |
		P_Fld(reg_mck_rodt_p1[0],
		SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B0) |
		P_Fld(reg_ui_rodt_p1[0],
		SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B0));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B1_RODTEN_MCK_UI_DLY),
		P_Fld(reg_mck_rodt[1],
		SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B1) |
		P_Fld(reg_ui_rodt[1],
		SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B1) |
		P_Fld(reg_mck_rodt_p1[1],
		SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B1) |
		P_Fld(reg_ui_rodt_p1[1],
		SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B1));
#endif

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B0_DQSIEN_PI_DLY),
		best_win->best_dqsien_dly_pi[0],
		SHU_RK_B0_DQSIEN_PI_DLY_DQSIEN_PI_B0);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B1_DQSIEN_PI_DLY),
		best_win->best_dqsien_dly_pi[1],
		SHU_RK_B1_DQSIEN_PI_DLY_DQSIEN_PI_B1);

}


/* By autoK: Set the result calibrated by HW to RG */
static void rxdqs_gating_auto_xlate(DRAMC_CTX_T *p,
	struct rxdqs_gating_best_win *best_win,
	struct rxdqs_gating_cal *rxdqs_cal)
{
	u8 mck, ui, pi;
	U8 mck_p1, ui_p1;
	u8 mck2ui, freq_div;
	U8 total_ui;
#if GATING_RODT_LATANCY_EN
	U8 mck_rodt, ui_rodt;
	U8 mck_rodt_p1, ui_rodt_p1;
#endif
	U16 value;
	u8 dqs_i;

	/* Transfer HW unit to RG unit */
	for (dqs_i = 0; dqs_i < p->data_width/DQS_BIT_NUMBER; dqs_i++) {
		mck = best_win->best_dqsien_dly_mck[dqs_i];
		ui = best_win->best_dqsien_dly_ui[dqs_i];
		pi = best_win->best_dqsien_dly_pi[dqs_i];
		mck2ui = rxdqs_cal->dqsien_ui_per_mck;
		freq_div = rxdqs_cal->dqsien_freq_div;

		if (vGet_Div_Mode(p) == DIV16_MODE)
			total_ui = (mck << 4) + ui; /* 1:16 mode */
		else if (vGet_Div_Mode(p) == DIV8_MODE)
			total_ui = (mck << 3) + ui; /* 1: 8 mode */
		else
			total_ui = (mck << 2) + ui; /* 1: 4 mode */

		/* RG is always 1:16 mode */
		mck = (total_ui >> 4);
		ui = (total_ui & 0xf);

		value = mck * mck2ui + ui; /* Total UI number */
		mck_p1 = (value + freq_div) / mck2ui;
		ui_p1 = (value + freq_div) % mck2ui;

		mcSHOW_DBG_MSG(("[Gating][RG] DQS%d Final result: (%d, %d, %d)\n", dqs_i, mck, ui, pi));
		mcSHOW_DBG_MSG(("[Gating][RG] DQS%d Final result P1: (%d, %d)\n", dqs_i, mck_p1, ui_p1));

		best_win->best_dqsien_dly_mck[dqs_i] = mck;
		best_win->best_dqsien_dly_ui[dqs_i] = ui;
		best_win->best_dqsien_dly_pi[dqs_i] = pi;

		best_win->best_dqsien_dly_mck_p1[dqs_i] = mck_p1;
		best_win->best_dqsien_dly_ui_p1[dqs_i] = ui_p1;
		best_win->best_dqsien_dly_pi_p1[dqs_i] = pi;
	}
}

#define RXDQS_GATING_AUTO_CAL_STATUS_BYTE_OFFSET		0x40

static DRAM_STATUS_T rxdqs_gating_auto_cal_status(DRAMC_CTX_T *p,
	struct rxdqs_gating_auto_param *auto_param,
	struct rxdqs_gating_best_win *best_win)
{
	U8 mck_center[DQS_NUMBER], ui_center[DQS_NUMBER], pi_center[DQS_NUMBER];
	U8 mck_left[DQS_NUMBER], ui_left[DQS_NUMBER], pi_left[DQS_NUMBER];
	U8 mck_right[DQS_NUMBER], ui_right[DQS_NUMBER], pi_right[DQS_NUMBER];
	U8 done[DQS_NUMBER] = { 0 }, error[DQS_NUMBER] = { 0 };
	DRAM_STATUS_T ret;
	U8 done_bytes, total_bytes;
	U8 byte_ofst;
	U8 dqs_i;

	total_bytes = p->data_width / DQS_BIT_NUMBER;
	done_bytes = 0;
	ret = DRAM_OK;

	while (done_bytes < total_bytes) {
		for (dqs_i = 0; dqs_i < (p->data_width / DQS_BIT_NUMBER); dqs_i++) {
			/* If already done, skip this byte */
			if (done[dqs_i])
				continue;

			byte_ofst = dqs_i * RXDQS_GATING_AUTO_CAL_STATUS_BYTE_OFFSET;

			done[dqs_i] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(
				DDRPHY_REG_DQSIEN_AUTOK_B0_RK0_STATUS0 + byte_ofst),
				DQSIEN_AUTOK_B0_RK0_STATUS0_AUTOK_DONE_B0_RK0);
			error[dqs_i] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(
				DDRPHY_REG_DQSIEN_AUTOK_B0_RK0_STATUS0 + byte_ofst),
				DQSIEN_AUTOK_B0_RK0_STATUS0_AUTOK_ERR_B0_RK0);

			/* If autok fail, done flag will not be asserted. */
			if (done[dqs_i] || error[dqs_i]) {
				/* Done and Pass */
				if (error[dqs_i] == 0) {
					mck_center[dqs_i] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(
						DDRPHY_REG_DQSIEN_AUTOK_B0_RK0_STATUS0 + byte_ofst),
						DQSIEN_AUTOK_B0_RK0_STATUS0_DQSIEN_AUTOK_C_MCK_B0_RK0);
					ui_center[dqs_i] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(
						DDRPHY_REG_DQSIEN_AUTOK_B0_RK0_STATUS0 + byte_ofst),
						DQSIEN_AUTOK_B0_RK0_STATUS0_DQSIEN_AUTOK_C__UI_B0_RK0);
					pi_center[dqs_i] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(
						DDRPHY_REG_DQSIEN_AUTOK_B0_RK0_STATUS0 + byte_ofst),
						DQSIEN_AUTOK_B0_RK0_STATUS0_DQSIEN_AUTOK_C__PI_B0_RK0);

					mck_left[dqs_i] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(
						DDRPHY_REG_DQSIEN_AUTOK_B0_RK0_STATUS1 + byte_ofst),
						DQSIEN_AUTOK_B0_RK0_STATUS1_DQSIEN_AUTOK_L_MCK_B0_RK0);
					ui_left[dqs_i] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(
						DDRPHY_REG_DQSIEN_AUTOK_B0_RK0_STATUS1 + byte_ofst),
						DQSIEN_AUTOK_B0_RK0_STATUS1_DQSIEN_AUTOK_L__UI_B0_RK0);
					pi_left[dqs_i] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(
						DDRPHY_REG_DQSIEN_AUTOK_B0_RK0_STATUS1 + byte_ofst),
						DQSIEN_AUTOK_B0_RK0_STATUS1_DQSIEN_AUTOK_L__PI_B0_RK0);

					/* If early break mode not enabled, right boundary could be found */
					if (auto_param->early_break == DISABLE) {
						mck_right[dqs_i] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(
							DDRPHY_REG_DQSIEN_AUTOK_B0_RK0_STATUS1 + byte_ofst),
							DQSIEN_AUTOK_B0_RK0_STATUS1_DQSIEN_AUTOK_R_MCK_B0_RK0);
						ui_right[dqs_i] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(
							DDRPHY_REG_DQSIEN_AUTOK_B0_RK0_STATUS1 + byte_ofst),
							DQSIEN_AUTOK_B0_RK0_STATUS1_DQSIEN_AUTOK_R__UI_B0_RK0);
						pi_right[dqs_i] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(
							DDRPHY_REG_DQSIEN_AUTOK_B0_RK0_STATUS1 + byte_ofst),
							DQSIEN_AUTOK_B0_RK0_STATUS1_DQSIEN_AUTOK_R__PI_B0_RK0);
					}
				}
				else
				{
					/* If error occurred for this byte, it will be treated as a DONE condition */
					done[dqs_i] = 1;
				}

				if (auto_param->dbg_mode == ENABLE) {
					U32 dbg_reg_addr;
					U32 dbg_reg_idx;
					U32 dbg_reg_val;

					dbg_reg_addr = DRAMC_REG_ADDR(
						DDRPHY_REG_DQSIEN_AUTOK_B0_RK0_DBG_STATUS0 + byte_ofst);
					for (dbg_reg_idx = 0;
						dbg_reg_idx < RXDQS_GATING_AUTO_DBG_REG_NUM;
						dbg_reg_idx++, dbg_reg_addr += 4) {
						dbg_reg_val = u4IO32Read4B(dbg_reg_addr);

						mcSHOW_ERR_MSG(("B%d Gating AUTOK DBG Status-%d: [0x%08x]\n",
							dqs_i, dbg_reg_idx, dbg_reg_val));
					}
				}
				done_bytes++;
			}
		}

		mcDELAY_MS(1);
	}

	/* Log it */
	for (dqs_i = 0; dqs_i < (p->data_width / DQS_BIT_NUMBER); dqs_i++) {
		mcSHOW_DBG_MSG(("[Gating][%s] AUTOK of CH-%d, Rk-%d, Byte-%d:\n",
			error[dqs_i]? "Fail" : "Pass", p->channel, p->rank, dqs_i));

		if (done[dqs_i]) {
			if (error[dqs_i] == 0) {
				mcSHOW_DBG_MSG(("\tcenter(%2d, %2d, %2d)\n",
							mck_center[dqs_i], ui_center[dqs_i], pi_center[dqs_i]));
				mcSHOW_DBG_MSG(("\tleft(%2d, %2d, %2d)\n",
							mck_left[dqs_i], ui_left[dqs_i], pi_left[dqs_i]));

				if (auto_param->early_break == DISABLE) {
					mcSHOW_DBG_MSG(("\tright(%2d, %2d, %2d)\n",
								mck_right[dqs_i], ui_right[dqs_i], pi_right[dqs_i]));
				}
			}
			if (error[dqs_i]) {
				ret = DRAM_FAIL;
			} else {
				/* If passed, shall set the result to RG */
				best_win->best_dqsien_dly_mck[dqs_i] = mck_center[dqs_i];
				best_win->best_dqsien_dly_ui[dqs_i] = ui_center[dqs_i];
				best_win->best_dqsien_dly_pi[dqs_i] = pi_center[dqs_i];
			}
		}
	}

	rxdqs_gating_auto_cal_stop(p);

	return ret;
}

static DRAM_STATUS_T dramc_rx_dqs_gating_auto_cal(DRAMC_CTX_T *p)
{
	struct rxdqs_gating_auto_param auto_param;
	struct rxdqs_gating_best_win rxdqs_best_win;
	struct rxdqs_gating_cal rxdqs_cal;
	DRAM_STATUS_T ret;
	U8 start_ui, end_ui;
	U8 mck2ui_hw;

	U32 reg_backup_address[ ] = {
		(DRAMC_REG_ADDR(DRAMC_REG_DUMMY_RD)),
		(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CG_CTRL0)),
		(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A4)),
		(DRAMC_REG_ADDR(DRAMC_REG_RK_DUMMY_RD_ADR))
	};

	DramcBackupRegisters(p, reg_backup_address,
		sizeof (reg_backup_address) / sizeof (U32));

	ret = DRAM_OK;

	memset((void *)&auto_param, 0, sizeof auto_param);
	memset((void *)&rxdqs_best_win, 0, sizeof rxdqs_best_win);
	memset((void *)&rxdqs_cal, 0, sizeof rxdqs_cal);

	if (vGet_Div_Mode(p) == DIV4_MODE)
		rxdqs_cal.dqsien_freq_div = 2;
	else
		rxdqs_cal.dqsien_freq_div = 4;
	rxdqs_cal.dqsien_ui_per_mck = DQS_GW_UI_PER_MCK;

	if (!rxdqs_gating_bypass(p)) {
		/* 60868 has different mck2ui relations for HW and RG */
		if (vGet_Div_Mode(p) == DIV16_MODE)
			mck2ui_hw = 16;
		else if (vGet_Div_Mode(p) == DIV8_MODE)
			mck2ui_hw = 8;
		else
			mck2ui_hw = 4;

#if ENABLE_GATING_AUTOK_WA
		if (__wa__gating_autok_init_ui[p->rank] > 3)
			start_ui = __wa__gating_autok_init_ui[p->rank] - 3;
		else
#endif
		start_ui = u1GetGatingStartPos(p, AUTOK_ON);
		end_ui = start_ui + 32;

		/* Set auto calibration params */
		auto_param.early_break = ENABLE;
		auto_param.dbg_mode = ENABLE;
		auto_param.init_mck = start_ui / mck2ui_hw;
		auto_param.init_ui = start_ui % mck2ui_hw;
		auto_param.end_mck = end_ui / mck2ui_hw;
		auto_param.end_ui = end_ui % mck2ui_hw;
		auto_param.pi_offset = 2; /* 2 ^ 2 = 4 */
		auto_param.burst_len = RXDQS_BURST_LEN_8;

#if FOR_DV_SIMULATION_USED == 1
		cal_sv_rand_args_t *psra = get_psra();

		if (psra) {
			auto_param.early_break =
					psra->dqsien_autok_early_break_en? ENABLE: DISABLE;
			auto_param.dbg_mode =
					psra->dqsien_autok_dbg_mode_en? ENABLE: DISABLE;
			auto_param.pi_offset =
					psra->dqsien_autok_pi_offset? ENABLE: DISABLE;
		}
#endif /* FOR_DV_SIMULATION_USED == 1 */

		rxdqs_gating_auto_cal_cfg(p, &auto_param);

		/* Trigger HW auto k */
		rxdqs_gating_auto_cal_trigger(p);

		ret = rxdqs_gating_auto_cal_status(p, &auto_param, &rxdqs_best_win);
		if (ret == DRAM_OK)
			vSetCalibrationResult(p, DRAM_CALIBRATION_GATING, DRAM_OK);

		rxdqs_gating_auto_xlate(p, &rxdqs_best_win, &rxdqs_cal);
	}

	rxdqs_gating_fastk_save_restore(p, &rxdqs_best_win, &rxdqs_cal);
	rxdqs_gating_set_final_result(p, rxdqs_cal.dqsien_ui_per_mck, &rxdqs_best_win);

	rxdqs_gating_misc_process(p, &rxdqs_best_win);
	DramcRestoreRegisters(p, reg_backup_address,
		sizeof (reg_backup_address) / sizeof (U32));

	DramPhyReset(p);

	return ret;
}

static void rxdqs_gating_sw_cal_init(DRAMC_CTX_T *p, U8 use_enhanced_rdqs)
{

	/* Disable Per-Bank ref */
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_CONF0),  0, SHU_CONF0_PBREFEN);

	/*----------------
	 * From DV
	 *------------------------*/
	if (p->frequency == 800) {
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL1),
			0x1, MISC_STBCAL1_STBCNT_SW_RST);
	}

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL1),
		0x1, MISC_STBCAL1_STBCNT_SHU_RST_EN);

	/* SELPH_MODE = BY RANK */
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL2),
		0x1, MISC_STBCAL2_DQSIEN_SELPH_BY_RANK_EN);

	if (p->dram_type == TYPE_LPDDR5) {
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL2),
			0x1, MISC_STBCAL2_STB_PICG_EARLY_1T_EN);
	}

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL1),
		0x1, MISC_STBCAL1_DIS_PI_TRACK_AS_NOT_RD);

	/* PICG_EARLY_EN */
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ6),
		0x1, B0_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B0);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ6),
		0x1, B0_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B0);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL2),
		0x1, MISC_STBCAL2_STB_PICG_EARLY_1T_EN);

	/* BURST_MODE */
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_STBCAL),
		0x1, MISC_SHU_STBCAL_DQSIEN_BURST_MODE);

#if (__LP5_COMBO__)
	if (is_lp5_family(p)) {
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ9),
			0x1, B0_DQ9_RG_RX_ARDQS0_DQSIENMODE_B0);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ9),
			0x1, B1_DQ9_RG_RX_ARDQS0_DQSIENMODE_B1);
	} else
#endif
	{
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ9),
			0x1, B0_DQ9_RG_RX_ARDQS0_DQSIENMODE_B0);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ9),
			0x1, B1_DQ9_RG_RX_ARDQS0_DQSIENMODE_B1);
	}

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ6),
		0x2, B0_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B0);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ6),
		0x2, B1_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B1);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL),
		0x1, MISC_STBCAL_DQSIENMODE);

	/* New Rank Mode */
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL2),
		P_Fld(0x1, MISC_STBCAL2_STB_IG_XRANK_CG_RST) |
		P_Fld(0x1, MISC_STBCAL2_STB_RST_BY_RANK) |
		P_Fld(0x1, MISC_STBCAL2_DQSIEN_SELPH_BY_RANK_EN));

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B0_PHY2),
		0x1, B0_PHY2_RG_RX_ARDQS_DQSIEN_UI_LEAD_LAG_EN_B0);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B1_PHY2),
		0x1, B1_PHY2_RG_RX_ARDQS_DQSIEN_UI_LEAD_LAG_EN_B1);

	//DramcHWGatingOnOff(p, 0);

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL2), 1,
		MISC_STBCAL2_STBENCMPEN);

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_RX_SET0), 0,
		RX_SET0_DM4TO1MODE);

	/* enable &reset DQS counter */
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL2), 1,
		MISC_STBCAL2_DQSG_CNT_EN);
	mcDELAY_US(4); /* wait 1 auto refresh after DQS Counter enable */

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL2), 1,
		MISC_STBCAL2_DQSG_CNT_RST);
	mcDELAY_US(1); /* delay 2T */
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL2), 0,
		MISC_STBCAL2_DQSG_CNT_RST);

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CTRL1),
		u1GetRank(p), MISC_CTRL1_R_DMSTBENCMP_RK_OPT);
	DramcEngine2Init(p, 0x55000000,
		0xaa000000 | GATING_PATTERN_NUM_LP5, TEST_AUDIO_PATTERN, 0, TE_NO_UI_SHIFT);

	if (use_enhanced_rdqs) {
		/* TBD. Enter Enhanced RDQS training mode */
	}
}

static void rxdqs_gating_set_dqsien_dly(DRAMC_CTX_T *p, U8 dly_ui,
	struct rxdqs_gating_cal *rxdqs_cal)
{
	U32 reg_mck, reg_ui;
	U32 reg_mck_p1, reg_ui_p1;
#if GATING_RODT_LATANCY_EN
	U32 reg_mck_rodt, reg_ui_rodt;
	U32 reg_mck_rodt_p1, reg_ui_rodt_p1;
#endif
	U8 mck2ui = rxdqs_cal->dqsien_ui_per_mck;

	rxdqs_cal->dqsien_dly_mck = dly_ui / rxdqs_cal->dqsien_ui_per_mck;
	rxdqs_cal->dqsien_dly_ui = dly_ui % rxdqs_cal->dqsien_ui_per_mck;
	rxdqs_cal->dqsien_dly_mck_p1 = (dly_ui + rxdqs_cal->dqsien_freq_div) / mck2ui;
	rxdqs_cal->dqsien_dly_ui_p1 = (dly_ui + rxdqs_cal->dqsien_freq_div) % mck2ui;

	reg_mck = rxdqs_cal->dqsien_dly_mck;
	reg_ui = rxdqs_cal->dqsien_dly_ui;
	reg_mck_p1 = rxdqs_cal->dqsien_dly_mck_p1;
	reg_ui_p1 = rxdqs_cal->dqsien_dly_ui_p1;

#if GATING_RODT_LATANCY_EN
	value = (reg_mck * mck2ui) + reg_ui;

	if (value >= 11) {
		/* For RODT, MCK2UI is different from Gating */
		U8 rodt_mck2ui = get_rodt_mck2ui(p);

		value -= 11;
		reg_mck_rodt = value / rodt_mck2ui;
		reg_ui_rodt = value % rodt_mck2ui;

		reg_mck_rodt_p1 = reg_mck_rodt;
		reg_ui_rodt_p1 = reg_ui_rodt;
	} else {

		reg_mck_rodt = 0;
		reg_ui_rodt = 0;
		reg_mck_rodt_p1 = 4;
		reg_ui_rodt_p1 = 4;
		mcSHOW_DBG_MSG(("[Warning] RODT cannot be -11UI\n"));
	}
#endif

	/* Set DQSIEN delay in MCK and UI */
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B0_DQSIEN_MCK_UI_DLY),
		P_Fld(reg_mck,
		SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B0) |
		P_Fld(reg_ui,
		SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B0) |
		P_Fld(reg_mck_p1,
		SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B0) |
		P_Fld(reg_ui_p1,
		SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B0));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B1_DQSIEN_MCK_UI_DLY),
		P_Fld(reg_mck,
		SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B1) |
		P_Fld(reg_ui,
		SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B1) |
		P_Fld(reg_mck_p1,
		SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B1) |
		P_Fld(reg_ui_p1,
		SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B1));

#if GATING_RODT_LATANCY_EN
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B0_RODTEN_MCK_UI_DLY),
		P_Fld(reg_mck_rodt,
		SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B0) |
		P_Fld(reg_ui_rodt,
		SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B0) |
		P_Fld(reg_mck_rodt_p1,
		SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B0) |
		P_Fld(reg_ui_rodt_p1,
		SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B0));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B1_RODTEN_MCK_UI_DLY),
		P_Fld(reg_mck_rodt,
		SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B1) |
		P_Fld(reg_ui_rodt,
		SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B1) |
		P_Fld(reg_mck_rodt_p1,
		SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B1) |
		P_Fld(reg_ui_rodt_p1,
		SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B1));
#endif
}

static void rxdqs_gating_sw_cal_trigger(DRAMC_CTX_T *p,
	struct rxdqs_gating_cal *rxdqs_cal)
{
#if 0//ENABLE_DDR800_OPEN_LOOP_MODE_OPTION -> No 0.5UI after A60868
	if (u1IsPhaseMode(p) == TRUE) {
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_DQ0),
			rxdqs_cal->dqsien_dly_pi >> 4, SHU_R0_B0_DQ0_DA_ARPI_DDR400_0D5UI_RK0_B0);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_DQ0),
			rxdqs_cal->dqsien_dly_pi >> 4, SHU_R0_B1_DQ0_DA_ARPI_DDR400_0D5UI_RK0_B1);
	} else
#endif
	{
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B0_DQSIEN_PI_DLY),
			rxdqs_cal->dqsien_dly_pi, SHU_RK_B0_DQSIEN_PI_DLY_DQSIEN_PI_B0);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B1_DQSIEN_PI_DLY),
			rxdqs_cal->dqsien_dly_pi, SHU_RK_B1_DQSIEN_PI_DLY_DQSIEN_PI_B1);
	}
	DramPhyReset(p);

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL2), 1,
		MISC_STBCAL2_DQSG_CNT_RST);
	mcDELAY_US(1);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL2), 0,
		MISC_STBCAL2_DQSG_CNT_RST);

	/* enable TE2, audio pattern */
	DramcEngine2Run(p, TE_OP_READ_CHECK, TEST_AUDIO_PATTERN);
}

static void rxdqs_gating_get_leadlag(DRAMC_CTX_T *p,
	struct rxdqs_gating_trans *rxdqs_trans,
	struct rxdqs_gating_cal *rxdqs_cal)
{
	U8 dqs_i;
	U8 debounce_thrd_PI = 16;

	for (dqs_i = 0; dqs_i < (p->data_width / DQS_BIT_NUMBER); dqs_i++) {
		if (dqs_i == 0) {
			rxdqs_trans->dqs_lead[0] = u4IO32ReadFldAlign(
				DRAMC_REG_ADDR(DDRPHY_REG_MISC_PHY_RGS_STBEN_B0),
				MISC_PHY_RGS_STBEN_B0_AD_RX_ARDQS0_STBEN_LEAD_B0);
			rxdqs_trans->dqs_lag[0] = u4IO32ReadFldAlign(
				DRAMC_REG_ADDR(DDRPHY_REG_MISC_PHY_RGS_STBEN_B0),
				MISC_PHY_RGS_STBEN_B0_AD_RX_ARDQS0_STBEN_LAG_B0);
		} else {
			rxdqs_trans->dqs_lead[1] = u4IO32ReadFldAlign(
				DRAMC_REG_ADDR(DDRPHY_REG_MISC_PHY_RGS_STBEN_B1),
				MISC_PHY_RGS_STBEN_B1_AD_RX_ARDQS0_STBEN_LEAD_B1);
			rxdqs_trans->dqs_lag[1] = u4IO32ReadFldAlign(
				DRAMC_REG_ADDR(DDRPHY_REG_MISC_PHY_RGS_STBEN_B1),
				MISC_PHY_RGS_STBEN_B1_AD_RX_ARDQS0_STBEN_LAG_B1);
		}

		if ((rxdqs_trans->dqs_lead[dqs_i] == 1) &&
			(rxdqs_trans->dqs_lag[dqs_i] == 1)) {
			rxdqs_trans->dqs_high[dqs_i]++;
			rxdqs_trans->dqs_transition[dqs_i] = 1;

			/* Record the latest value that causes (lead, lag) = (1, 1) */
			rxdqs_trans->dqsien_dly_mck_leadlag[dqs_i] =
				rxdqs_cal->dqsien_dly_mck;
			rxdqs_trans->dqsien_dly_ui_leadlag[dqs_i] =
				rxdqs_cal->dqsien_dly_ui;
			rxdqs_trans->dqsien_dly_pi_leadlag[dqs_i] =
				rxdqs_cal->dqsien_dly_pi;
		} else if ((rxdqs_trans->dqs_high[dqs_i] *
			rxdqs_cal->dqsien_pi_adj_step) >= debounce_thrd_PI) {
			/* Consecutive 16 PI DQS high for de-glitch */
			if (((rxdqs_trans->dqs_lead[dqs_i] == 1) &&
				(rxdqs_trans->dqs_lag[dqs_i] == 0)) ||
				((rxdqs_trans->dqs_lead[dqs_i] == 0) &&
				(rxdqs_trans->dqs_lag[dqs_i] == 1))) {
				rxdqs_trans->dqs_transition[dqs_i]++;
			}
		#if GATING_LEADLAG_LOW_LEVEL_CHECK
			else if ((rxdqs_trans->dqs_lead[dqs_i] == 0) &&
				(rxdqs_trans->dqs_lag[dqs_i] == 0)){
				if ((rxdqs_trans->dqs_low[dqs_i] *
					 rxdqs_cal->dqsien_pi_adj_step) >= debounce_thrd_PI) {
				/* (lead, lag) = (0, 0), transition done */
				rxdqs_trans->dqs_transitioned[dqs_i] = 1;
					}
				rxdqs_trans->dqs_low[dqs_i]++;
			}else {
				  rxdqs_trans->dqs_high[dqs_i] = 0;
				  rxdqs_trans->dqs_low[dqs_i] = 0;
				}
		#else
		else {
			/* (lead, lag) = (0, 0), transition done */
			rxdqs_trans->dqs_transitioned[dqs_i] = 1;
		}
		#endif
		} else {
			/* Lead/lag = (1, 1) number is too few. Reset dqs_high */
			rxdqs_trans->dqs_high[dqs_i] = 0;
		#if GATING_LEADLAG_LOW_LEVEL_CHECK
			rxdqs_trans->dqs_low[dqs_i] = 0;
		#endif
		}
	}
}

static U8 rxdqs_gating_sw_cal(DRAMC_CTX_T *p,
	struct rxdqs_gating_trans *rxdqs_trans,
	struct rxdqs_gating_cal *rxdqs_cal, U8 *pass_byte_count,
	struct rxdqs_gating_best_win *best_win, U8 dly_ui, U8 dly_ui_end)
{
	U8 gating_error[DQS_NUMBER];
	U32 debug_cnt[DQS_NUMBER];
	U32 debug_pass_cnt;
	U8 dqs_i;
	U8 passed_bytes;

	memset(debug_cnt, 0, sizeof(debug_cnt));
	passed_bytes = *pass_byte_count;

	rxdqs_gating_sw_cal_trigger(p, rxdqs_cal);

	if (p->rank == RANK_0) {
		gating_error[0] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(
			DDRPHY_REG_MISC_STBERR_ALL),
			MISC_STBERR_ALL_GATING_ERROR_B0_RK0);
		gating_error[1] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(
			DDRPHY_REG_MISC_STBERR_ALL),
			MISC_STBERR_ALL_GATING_ERROR_B1_RK0);
	} else {
		gating_error[0] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(
			DDRPHY_REG_MISC_STBERR_ALL),
			MISC_STBERR_ALL_GATING_ERROR_B0_RK1);
		gating_error[1] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(
			DDRPHY_REG_MISC_STBERR_ALL),
			MISC_STBERR_ALL_GATING_ERROR_B1_RK1);
	}

	/* read DQS counter
	 * Note: DQS counter is no longer used as pass condition. Here
	 * Read it and log it is just as debug method. Any way, DQS counter
	 * can still be used as a clue: it will be n*0x23 when gating is correct
	 */
	debug_cnt[0] = u4IO32Read4B(DRAMC_REG_ADDR(DDRPHY_REG_CAL_DQSG_CNT_B0));
	debug_cnt[1] = u4IO32Read4B(DRAMC_REG_ADDR(DDRPHY_REG_CAL_DQSG_CNT_B1));

	/* read (lead, lag) */
	rxdqs_gating_get_leadlag(p, rxdqs_trans, rxdqs_cal);

	mcSHOW_DBG_MSG(("%2d %2d %2d | ",
		rxdqs_cal->dqsien_dly_mck, rxdqs_cal->dqsien_dly_ui,
		rxdqs_cal->dqsien_dly_pi));
	mcSHOW_DBG_MSG(("B1->B0 | %x %x | %x %x | (%d %d) (%d %d)\n",
		debug_cnt[1], debug_cnt[0],
		gating_error[1], gating_error[0],
		rxdqs_trans->dqs_lead[1], rxdqs_trans->dqs_lag[1],
		rxdqs_trans->dqs_lead[0], rxdqs_trans->dqs_lag[0]));

#if (__LP5_COMBO__)
	if((is_lp5_family(p)) && (vGet_Div_Mode(p) == DIV16_MODE))
		debug_pass_cnt = (GATING_GOLDEND_DQSCNT_LP5 >> 1);
	else
#endif
		debug_pass_cnt = GATING_GOLDEND_DQSCNT_LP5;

	/* Decide the window center */
	for (dqs_i = 0; dqs_i < (p->data_width / DQS_BIT_NUMBER); dqs_i++) {
		if (passed_bytes & (1 << dqs_i))
			continue;

		if ((gating_error[dqs_i] == 0) && (debug_cnt[dqs_i] == debug_pass_cnt)) {
			/* Calcuate DQSIEN position */
			if (rxdqs_trans->dqs_transitioned[dqs_i] != 0) {
				U8 pass_count = rxdqs_trans->dqs_transition[dqs_i];
				U8 offset = (pass_count * rxdqs_cal->dqsien_pi_adj_step) / 2;
				U8 mck2ui, ui2pi, freq_div;
				U8 tmp;

				mck2ui = rxdqs_cal->dqsien_ui_per_mck;
				ui2pi = rxdqs_cal->dqsien_pi_per_ui;
				freq_div = rxdqs_cal->dqsien_freq_div;

				/* PI */
				tmp = rxdqs_trans->dqsien_dly_pi_leadlag[dqs_i] + offset;
				best_win->best_dqsien_dly_pi[dqs_i] = tmp % ui2pi;
				best_win->best_dqsien_dly_pi_p1[dqs_i] =
					best_win->best_dqsien_dly_pi[dqs_i];

				/* UI & MCK - P0 */
				tmp /= ui2pi;
				tmp = rxdqs_trans->dqsien_dly_ui_leadlag[dqs_i] + tmp;
				best_win->best_dqsien_dly_ui[dqs_i] = tmp % mck2ui;
				best_win->best_dqsien_dly_mck[dqs_i] =
					rxdqs_trans->dqsien_dly_mck_leadlag[dqs_i] + (tmp / mck2ui);

				/* UI & MCK - P1 */
				best_win->best_dqsien_dly_ui_p1[dqs_i] =
					best_win->best_dqsien_dly_mck[dqs_i] * mck2ui +
					best_win->best_dqsien_dly_ui[dqs_i] + freq_div; /* Total UI for Phase1 */
				mcSHOW_DBG_MSG(("Total UI for P1: %d, mck2ui %d\n",
					best_win->best_dqsien_dly_mck_p1[dqs_i], mck2ui));
				best_win->best_dqsien_dly_mck_p1[dqs_i] =
					best_win->best_dqsien_dly_ui_p1[dqs_i] / mck2ui;
				best_win->best_dqsien_dly_ui_p1[dqs_i] =
					best_win->best_dqsien_dly_ui_p1[dqs_i] % mck2ui;

				mcSHOW_DBG_MSG(("best dqsien dly found for B%d: "
					"(%2d, %2d, %2d)\n", dqs_i,
					best_win->best_dqsien_dly_mck[dqs_i],
					best_win->best_dqsien_dly_ui[dqs_i],
					best_win->best_dqsien_dly_pi[dqs_i]));
				passed_bytes |= 1 << dqs_i;

				if (((p->data_width == DATA_WIDTH_16BIT) &&
					(passed_bytes == 0x3)) ||
					((p->data_width == DATA_WIDTH_32BIT) &&
					(passed_bytes == 0xf))) {
					dly_ui = dly_ui_end;
					break;
				}
			}
		} else {
			/* Clear lead lag info in case lead/lag flag toggled
			 * while gating counter & gating error still incorrect
			 */
			rxdqs_trans->dqs_high[dqs_i] = 0;
			rxdqs_trans->dqs_transition[dqs_i] = 0;
			rxdqs_trans->dqs_transitioned[dqs_i] = 0;
		}
	}

	*pass_byte_count = passed_bytes;
	return dly_ui;
}

static DRAM_STATUS_T dramc_rx_dqs_gating_sw_cal(DRAMC_CTX_T *p,
	U8 use_enhance_rdqs)
{
	struct rxdqs_gating_cal rxdqs_cal;
	struct rxdqs_gating_trans rxdqs_trans;
	struct rxdqs_gating_best_win rxdqs_best_win;
	U8 dly_ui, dly_ui_start, dly_ui_end;
	U8 pi_per_ui, ui_per_mck, freq_div;
	U8 pass_byte_count;
	U8 dqs_i;
	U8 u1GatingErrorFlag=0;

	if (p == NULL) {
		mcSHOW_ERR_MSG(("[Error] Context NULL\n"));
		return DRAM_FAIL;
	}

	memset(&rxdqs_cal, 0, sizeof(struct rxdqs_gating_cal));
	memset(&rxdqs_trans, 0, sizeof(struct rxdqs_gating_trans));
	memset(&rxdqs_best_win, 0, sizeof(struct rxdqs_gating_best_win));

	pi_per_ui = DQS_GW_PI_PER_UI; /* 1 UI = ? PI. Sams as CBT, differ according to data rate?? */
	ui_per_mck = DQS_GW_UI_PER_MCK; /* 1 mck = ? UI. Decided by (Tmck/Tck) * (Tck/Twck) */
	if (vGet_Div_Mode(p) == DIV4_MODE)
		freq_div = 2;
	else
		freq_div = 4;

#if ENABLE_DDR800_OPEN_LOOP_MODE_OPTION
	if (u1IsPhaseMode(p) == TRUE)
		rxdqs_cal.dqsien_pi_adj_step = (0x1 << 4); // Divide by 16 (90 degree)
	else
#endif
	rxdqs_cal.dqsien_pi_adj_step = DQS_GW_FINE_STEP;
#if ENABLE_GATING_AUTOK_WA
	if (__wa__gating_swk_for_autok)
		rxdqs_cal.dqsien_pi_adj_step = pi_per_ui;
#endif
	rxdqs_cal.dqsien_pi_per_ui = pi_per_ui;
	rxdqs_cal.dqsien_ui_per_mck = ui_per_mck;
	rxdqs_cal.dqsien_freq_div = freq_div;

	U32 reg_backup_address[ ] = {
		(DRAMC_REG_ADDR(DRAMC_REG_REFCTRL0)),
		(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ6)),
		(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ6)),
		(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL1)),
		(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL2)),
	};

	/* Register backup */
	DramcBackupRegisters(p, reg_backup_address,
		sizeof (reg_backup_address) / sizeof (U32));

	if (!rxdqs_gating_bypass(p)) {
		rxdqs_gating_sw_cal_init(p, use_enhance_rdqs);

#if 1
	#if (LP5_DDR4266_RDBI_WORKAROUND)
	if((is_lp5_family(p)) && (p->frequency == 2133))
		dly_ui_start = 15;
	else if((is_lp5_family(p)) && (p->frequency == 2750))
		dly_ui_start = 12;
	else
		dly_ui_start = u1GetGatingStartPos(p, AUTOK_OFF);//7; //12;ly_ui_start + 32;
	#else
	if((is_lp5_family(p)) && ((p->frequency == 2133) || (p->frequency == 2750)))
		dly_ui_start = 5;
	else
		dly_ui_start = u1GetGatingStartPos(p, AUTOK_OFF);//7; //12;ly_ui_start + 32;
	#endif

	dly_ui_end = dly_ui_start+ 32;
	pass_byte_count = 0;
#else
	#if __LP5_COMBO__
	if (is_lp5_family(p))
	{
		if (p->frequency == 1600)
			dly_ui_start = 7; //12;
		else
			dly_ui_start = 8; //12;

		dly_ui_end = dly_ui_start + 32;
		pass_byte_count = 0;
	}
	else
	#endif
	{
		 dly_ui_start = 9; //12; Eddie change to 9 for Hynix Normal Mode
		 if(p->freq_sel==LP4_DDR4266)
		 {
			dly_ui_start = 16;
		 }
		 dly_ui_end = dly_ui_start + 32;
		 pass_byte_count = 0;
	}
#endif


	for (dly_ui = dly_ui_start; dly_ui < dly_ui_end;
		dly_ui += DQS_GW_COARSE_STEP) {
		rxdqs_gating_set_dqsien_dly(p, dly_ui, &rxdqs_cal);

		for (rxdqs_cal.dqsien_dly_pi = 0; rxdqs_cal.dqsien_dly_pi <
			pi_per_ui; rxdqs_cal.dqsien_dly_pi +=
			rxdqs_cal.dqsien_pi_adj_step) {
			dly_ui = rxdqs_gating_sw_cal(p, &rxdqs_trans, &rxdqs_cal,
				&pass_byte_count, &rxdqs_best_win, dly_ui, dly_ui_end);

			if (dly_ui == dly_ui_end)
				break;
		}
	}

		DramcEngine2End(p);

		//check if there is no pass taps for each DQS
		for (dqs_i=0; dqs_i<(p->data_width/DQS_BIT_NUMBER); dqs_i++)
		{
			if ((pass_byte_count<< dqs_i)==0)
			{
				u1GatingErrorFlag=1;
				/*TINFO="error, no pass taps in DQS_%d !!!\n", dqs_i*/
				mcSHOW_ERR_MSG(("error, no pass taps in DQS_%d!\n", dqs_i));
			}
		}
	if (u1GatingErrorFlag==0)
		vSetCalibrationResult(p, DRAM_CALIBRATION_GATING, DRAM_OK);

#if (ENABLE_GATING_AUTOK_WA)
	if (!u1GatingErrorFlag && __wa__gating_swk_for_autok) {
		U8 ui[DQS_NUMBER], ui_min = 0xff;
		U8 dqs_index;
		for (dqs_index = 0; dqs_index < (p->data_width/DQS_BIT_NUMBER); dqs_index++){
			ui[dqs_index] = rxdqs_best_win.best_dqsien_dly_mck[dqs_index] * ui_per_mck +
			rxdqs_best_win.best_dqsien_dly_ui[dqs_index];

			if (ui[dqs_index] < ui_min)
				ui_min = ui[dqs_index];
		}
		__wa__gating_autok_init_ui[p->rank] = ui_min;

			DramcRestoreRegisters(p, reg_backup_address,
				sizeof (reg_backup_address) / sizeof (U32));
		return DRAM_OK;
	}
#endif
	}

	rxdqs_gating_fastk_save_restore(p, &rxdqs_best_win, &rxdqs_cal);
	rxdqs_gating_misc_process(p, &rxdqs_best_win);

	mcSHOW_DBG_MSG(("[Gating] SW calibration Done\n"));

	/* Set MCK & UI */
	rxdqs_gating_set_final_result(p, ui_per_mck, &rxdqs_best_win);

	DramcRestoreRegisters(p, reg_backup_address,
		sizeof (reg_backup_address) / sizeof (U32));

	DramPhyReset(p);

	return DRAM_OK;
}

/* LPDDR5 Rx DQS Gating */
DRAM_STATUS_T dramc_rx_dqs_gating_cal(DRAMC_CTX_T *p,
		u8 autok, U8 use_enhanced_rdqs)
{
	DRAM_STATUS_T ret;

	vPrintCalibrationBasicInfo(p);

#if ENABLE_GATING_AUTOK_WA
			 if (autok) {
				 __wa__gating_swk_for_autok = 1;
				 dramc_rx_dqs_gating_sw_cal(p, use_enhanced_rdqs);
				 __wa__gating_swk_for_autok = 0;
			 }
#endif

	 // default set FAIL
	vSetCalibrationResult(p, DRAM_CALIBRATION_GATING, DRAM_FAIL);

	/* Try HW auto calibration first. If failed,
	 * will try SW mode.
	 */
	if (autok) {
#if ENABLE_GATING_AUTOK_WA
		if (rxdqs_gating_bypass(p)) /* Already done by SWK */
			return DRAM_OK;
#endif
		ret = dramc_rx_dqs_gating_auto_cal(p);
		if (ret == DRAM_OK) {
			vSetCalibrationResult(p, DRAM_CALIBRATION_GATING, DRAM_OK);
			return DRAM_OK;
		}

		mcSHOW_ERR_MSG(("[Error] Gating auto calibration fail!!\n"));
	}

	mcSHOW_DBG_MSG(("[Gating] SW mode calibration\n"));

	return dramc_rx_dqs_gating_sw_cal(p, use_enhanced_rdqs);
}

///TODO: wait for porting +++
#if GATING_ADJUST_TXDLY_FOR_TRACKING
void DramcRxdqsGatingPostProcess(DRAMC_CTX_T *p)
{
	U8 dqs_i;
	U8 u1RankIdx, u1RankMax;
	S8 s1ChangeDQSINCTL;
#if XRTRTR_NEW_CROSS_RK_MODE
	U16 u2PHSINCTL = 0;
	U32 u4Rank_Sel_MCK_P0[2], u4Rank_Sel_MCK_P1[2], u4RANKINCTL_STB;
#endif
#if RDSEL_TRACKING_EN
	U32 u4PI_value[2] = {0};
#endif
	U32 backup_rank;
	U32 u4ReadDQSINCTL, u4RankINCTL_ROOT, u4XRTR2R, reg_TX_dly_DQSgated_min = 0;
	U8 mck2ui_shift;

	backup_rank = u1GetRank(p);

#ifdef XRTR2R_PERFORM_ENHANCE_DQSG_RX_DLY
	if (vGet_Div_Mode(p) == DIV8_MODE)
	{
		// wei-jen: DQSgated_min should be 2 when freq >= 1333, 1 when freq < 1333
		if (p->frequency >= 1333)
		{
			reg_TX_dly_DQSgated_min = 2;
		}
		else
		{
			reg_TX_dly_DQSgated_min = 1;
		}
	}
	else // for LPDDR4 1:4 mode
	{
		// 1866,1600,1333,1200	: reg_TX_dly_DQSgated (min) =2
		reg_TX_dly_DQSgated_min = 2;
	}
#else
	// wei-jen: DQSgated_min should be 3 when freq >= 1333, 2 when freq < 1333
	if (p->frequency >= 1333)
	{
		reg_TX_dly_DQSgated_min = 3;
	}
	else
	{
		reg_TX_dly_DQSgated_min = 2;
	}
#endif

	//Sylv_ia MP setting is switched to new mode, so RANKRXDVS can be set as 0 (review by HJ Huang)
#if 0
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_SHU_B0_DQ7), u1RankRxDVS, SHU_B0_DQ7_R_DMRANKRXDVS_B0);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_SHU_B1_DQ7), u1RankRxDVS, SHU_B1_DQ7_R_DMRANKRXDVS_B1);
#endif
	// === End of DVS setting =====

	s1ChangeDQSINCTL = reg_TX_dly_DQSgated_min - u1TXDLY_Cal_min;

	mcSHOW_DBG_MSG(("[RxdqsGatingPostProcess] freq %d\n"
					"ChangeDQSINCTL %d, reg_TX_dly_DQSgated_min %d, u1TXDLY_Cal_min %d\n",
						p->frequency,
						s1ChangeDQSINCTL, reg_TX_dly_DQSgated_min, u1TXDLY_Cal_min));
	mcDUMP_REG_MSG(("[RxdqsGatingPostProcess] freq %d\n"
					   "ChangeDQSINCTL %d, reg_TX_dly_DQSgated_min %d, u1TXDLY_Cal_min %d\n",
						p->frequency,
						s1ChangeDQSINCTL, reg_TX_dly_DQSgated_min, u1TXDLY_Cal_min));

	if (vGet_Div_Mode(p) == DIV16_MODE)
		mck2ui_shift = 4;
	else if (vGet_Div_Mode(p) == DIV8_MODE)
		mck2ui_shift = 3;
	else
		mck2ui_shift = 2;

	if (s1ChangeDQSINCTL != 0)	// need to change DQSINCTL and TXDLY of each byte
	{
		u1TXDLY_Cal_min += s1ChangeDQSINCTL;
		u1TXDLY_Cal_max += s1ChangeDQSINCTL;

		if (p->support_rank_num == RANK_DUAL)
			u1RankMax = RANK_MAX;
		else
			 u1RankMax = RANK_1;

		for (u1RankIdx = 0; u1RankIdx < u1RankMax; u1RankIdx++)
		{
			mcSHOW_DBG_MSG2(("Rank: %d\n", u1RankIdx));
			mcDUMP_REG_MSG(("Rank: %d\n", u1RankIdx));

			for (dqs_i = 0; dqs_i < (p->data_width / DQS_BIT_NUMBER); dqs_i++)
			{
#if 1
				U8 total_ui, total_ui_P1;
				total_ui = (ucbest_coarse_mck_backup[u1RankIdx][dqs_i] << 4) + ucbest_coarse_ui_backup[u1RankIdx][dqs_i];
				total_ui_P1 = (ucbest_coarse_mck_P1_backup[u1RankIdx][dqs_i] << 4) + ucbest_coarse_ui_P1_backup[u1RankIdx][dqs_i];

				total_ui += (s1ChangeDQSINCTL << mck2ui_shift);
				total_ui_P1 += (s1ChangeDQSINCTL << mck2ui_shift);

				ucbest_coarse_mck_backup[u1RankIdx][dqs_i] = (total_ui >> 4);
				ucbest_coarse_ui_backup[u1RankIdx][dqs_i] = total_ui & 0xf;

				ucbest_coarse_mck_P1_backup[u1RankIdx][dqs_i] = (total_ui_P1 >> 4);
				ucbest_coarse_ui_P1_backup[u1RankIdx][dqs_i] = total_ui_P1 & 0xf;
#else
				if (vGet_Div_Mode(p) == DIV8_MODE)
				{
					u4ReadTXDLY[u1RankIdx][dqs_i] = ucbest_coarse_mck_backup[u1RankIdx][dqs_i];
					u4ReadTXDLY_P1[u1RankIdx][dqs_i] = ucbest_coarse_mck_P1_backup[u1RankIdx][dqs_i];

					u4ReadTXDLY[u1RankIdx][dqs_i] += s1ChangeDQSINCTL;
					u4ReadTXDLY_P1[u1RankIdx][dqs_i] += s1ChangeDQSINCTL;

					ucbest_coarse_mck_backup[u1RankIdx][dqs_i] = u4ReadTXDLY[u1RankIdx][dqs_i];
					ucbest_coarse_mck_P1_backup[u1RankIdx][dqs_i] = u4ReadTXDLY_P1[u1RankIdx][dqs_i];
				}
				else // LP3 or LP4 1:4 mode
				{
					u4ReadTXDLY[u1RankIdx][dqs_i] = ((ucbest_coarse_mck_backup[u1RankIdx][dqs_i] << 1) + ((ucbest_coarse_ui_backup[u1RankIdx][dqs_i] >> 2) & 0x1));
					u4ReadTXDLY_P1[u1RankIdx][dqs_i] = ((ucbest_coarse_mck_P1_backup[u1RankIdx][dqs_i] << 1) + ((ucbest_coarse_ui_P1_backup[u1RankIdx][dqs_i] >> 2) & 0x1));

					u4ReadTXDLY[u1RankIdx][dqs_i] += s1ChangeDQSINCTL;
					u4ReadTXDLY_P1[u1RankIdx][dqs_i] += s1ChangeDQSINCTL;

					ucbest_coarse_mck_backup[u1RankIdx][dqs_i] = (u4ReadTXDLY[u1RankIdx][dqs_i] >> 1);
					ucbest_coarse_ui_backup[u1RankIdx][dqs_i] = ((u4ReadTXDLY[u1RankIdx][dqs_i] & 0x1) << 2) + (ucbest_coarse_ui_backup[u1RankIdx][dqs_i] & 0x3);

					ucbest_coarse_mck_P1_backup[u1RankIdx][dqs_i] = (u4ReadTXDLY_P1[u1RankIdx][dqs_i] >> 1);
					ucbest_coarse_ui_P1_backup[u1RankIdx][dqs_i] = ((u4ReadTXDLY_P1[u1RankIdx][dqs_i] & 0x1) << 2) + (ucbest_coarse_ui_P1_backup[u1RankIdx][dqs_i] & 0x3);
				}
#endif
				mcSHOW_DBG_MSG(("best DQS%d dly(2T, 0.5T) = (%d, %d)\n", dqs_i, ucbest_coarse_mck_backup[u1RankIdx][dqs_i], ucbest_coarse_ui_backup[u1RankIdx][dqs_i]));
				mcDUMP_REG_MSG(("PostProcess best DQS%d dly(2T, 0.5T) = (%d, %d)\n", dqs_i, ucbest_coarse_mck_backup[u1RankIdx][dqs_i], ucbest_coarse_ui_backup[u1RankIdx][dqs_i]));
			}
			for (dqs_i = 0; dqs_i < (p->data_width / DQS_BIT_NUMBER); dqs_i++)
			{
				mcSHOW_DBG_MSG(("best DQS%d P1 dly(2T, 0.5T) = (%d, %d)\n", dqs_i, ucbest_coarse_mck_P1_backup[u1RankIdx][dqs_i], ucbest_coarse_ui_P1_backup[u1RankIdx][dqs_i]));
				mcDUMP_REG_MSG(("PostProcess best DQS%d P1 dly(2T, 0.5T) = (%d, %d)\n", dqs_i, ucbest_coarse_mck_P1_backup[u1RankIdx][dqs_i], ucbest_coarse_ui_P1_backup[u1RankIdx][dqs_i]));
			}
		}

		for (u1RankIdx = 0; u1RankIdx < u1RankMax; u1RankIdx++)
		{
			vSetRank(p, u1RankIdx);
			// 4T or 2T coarse tune
			/* Set DQSIEN delay in MCK and UI */
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B0_DQSIEN_MCK_UI_DLY),
				P_Fld(ucbest_coarse_mck_backup[u1RankIdx][0],
				SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B0) |
				P_Fld(ucbest_coarse_ui_backup[u1RankIdx][0],
				SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B0) |
				P_Fld(ucbest_coarse_mck_P1_backup[u1RankIdx][0],
				SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B0) |
				P_Fld(ucbest_coarse_ui_P1_backup[u1RankIdx][0],
				SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B0));

			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B1_DQSIEN_MCK_UI_DLY),
				P_Fld(ucbest_coarse_mck_backup[u1RankIdx][1],
				SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B1) |
				P_Fld(ucbest_coarse_ui_backup[u1RankIdx][1],
				SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B1) |
				P_Fld(ucbest_coarse_mck_P1_backup[u1RankIdx][1],
				SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B1) |
				P_Fld(ucbest_coarse_ui_P1_backup[u1RankIdx][1],
				SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B1));
		#if RDSEL_TRACKING_EN
			//Byte 0
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_INI_UIPI),
				(ucbest_coarse_mck_backup[u1RankIdx][0] << 4) | (ucbest_coarse_ui_backup[u1RankIdx][0]),
				SHU_R0_B0_INI_UIPI_CURR_INI_UI_B0);//UI
			//Byte 1
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_INI_UIPI),
				(ucbest_coarse_mck_backup[u1RankIdx][1] << 4) | (ucbest_coarse_ui_backup[u1RankIdx][1]),
				SHU_R0_B1_INI_UIPI_CURR_INI_UI_B1);//UI
		#endif
		}
	}
	vSetRank(p, backup_rank);

	u4ReadDQSINCTL = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_RK_DQSCTL),
		MISC_SHU_RK_DQSCTL_DQSINCTL);
	mcDUMP_REG_MSG(("u4ReadDQSINCTL=%d\n", u4ReadDQSINCTL));
	u4ReadDQSINCTL -= s1ChangeDQSINCTL;

	#if ENABLE_READ_DBI
	if (p->DBI_R_onoff[p->dram_fsp])
	{
		u4ReadDQSINCTL++;
		#if 0//cc mark for reg not found
		u4ReadRODT = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_ODTCTRL), SHU_ODTCTRL_RODT);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_ODTCTRL), u4ReadRODT + 1, SHU_ODTCTRL_RODT); //update RODT value when READ_DBI is on
		#endif
	}
	#endif

#if XRTRTR_NEW_CROSS_RK_MODE
		for (dqs_i = 0; dqs_i < (p->data_width / DQS_BIT_NUMBER); dqs_i++)
		{
			if (ucbest_coarse_mck_backup[RANK_0][dqs_i] > ucbest_coarse_mck_backup[RANK_1][dqs_i])
			{
				u4Rank_Sel_MCK_P0[dqs_i] = (ucbest_coarse_mck_backup[RANK_0][dqs_i] > 0)? (ucbest_coarse_mck_backup[RANK_0][dqs_i] - 1): 0;
				u4Rank_Sel_MCK_P1[dqs_i] = (ucbest_coarse_mck_P1_backup[RANK_0][dqs_i] > 0)? (ucbest_coarse_mck_P1_backup[RANK_0][dqs_i] - 1): 0;
			}
			else
			{
				u4Rank_Sel_MCK_P0[dqs_i] = (ucbest_coarse_mck_backup[RANK_1][dqs_i] > 0)? (ucbest_coarse_mck_backup[RANK_1][dqs_i] - 1): 0;
				u4Rank_Sel_MCK_P1[dqs_i] = (ucbest_coarse_mck_P1_backup[RANK_1][dqs_i] > 0)? (ucbest_coarse_mck_P1_backup[RANK_1][dqs_i] - 1): 0;
			}
		}
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_RANK_SELPH_UI_DLY),
			P_Fld(u4Rank_Sel_MCK_P0[0], SHU_B0_RANK_SELPH_UI_DLY_RANKSEL_MCK_DLY_P0_B0) |
			P_Fld(u4Rank_Sel_MCK_P1[0], SHU_B0_RANK_SELPH_UI_DLY_RANKSEL_MCK_DLY_P1_B0));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_RANK_SELPH_UI_DLY),
			P_Fld(u4Rank_Sel_MCK_P0[1], SHU_B1_RANK_SELPH_UI_DLY_RANKSEL_MCK_DLY_P0_B1) |
			P_Fld(u4Rank_Sel_MCK_P1[1], SHU_B1_RANK_SELPH_UI_DLY_RANKSEL_MCK_DLY_P1_B1));

		u4RANKINCTL_STB = (u4ReadDQSINCTL > 2)? (u4ReadDQSINCTL - 2): 0;
		u2PHSINCTL = (u4ReadDQSINCTL == 0)? 0: (u4ReadDQSINCTL - 1);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_RANKCTL), u4RANKINCTL_STB, MISC_SHU_RANKCTL_RANKINCTL_STB);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_MISC_RANK_SEL_STB), u2PHSINCTL, SHU_MISC_RANK_SEL_STB_RANK_SEL_PHSINCTL);
#endif

#ifdef XRTR2R_PERFORM_ENHANCE_DQSG_RX_DLY
	// Wei-Jen: RANKINCTL_RXDLY = RANKINCTL = RankINCTL_ROOT = u4ReadDQSINCTL-2, if XRTR2R_PERFORM_ENHANCE_DQSG_RX_DLY enable
	// Wei-Jen: New algorithm : u4ReadDQSINCTL-2 >= 0
	if (u4ReadDQSINCTL >= 2)
	{
		u4RankINCTL_ROOT = u4ReadDQSINCTL - 2;
	}
	else
	{
		u4RankINCTL_ROOT = 0;
		mcSHOW_ERR_MSG(("u4RankINCTL_ROOT <2, Please check\n"));
#if (__ETT__)
		while (1);
#endif
	}
#else
	//Modify for corner IC failed at HQA test XTLV
	if (u4ReadDQSINCTL >= 3)
	{
		u4RankINCTL_ROOT = u4ReadDQSINCTL - 3;
	}
	else
	{
		u4RankINCTL_ROOT = 0;
		mcSHOW_ERR_MSG(("u4RankINCTL_ROOT <3, Risk for supporting 1066/RL8\n"));
	}
#endif

	//DQSINCTL
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_RK_DQSCTL),
		u4ReadDQSINCTL, MISC_SHU_RK_DQSCTL_DQSINCTL);  //Rank0 DQSINCTL
	vSetRank(p, RANK_1);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_RK_DQSCTL),
		u4ReadDQSINCTL, MISC_SHU_RK_DQSCTL_DQSINCTL);  //Rank1 DQSINCTL
	vSetRank(p, backup_rank);

	//No need to update RODT. If we update RODT, also need to update SELPH_ODTEN0_TXDLY
	//vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_ODTCTRL), u4ReadDQSINCTL, SHU_ODTCTRL_RODT);			 //RODT = DQSINCTL

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_RANKCTL),
		u4ReadDQSINCTL, MISC_SHU_RANKCTL_RANKINCTL_PHY);  //RANKINCTL_PHY = DQSINCTL
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_RANKCTL),
		u4RankINCTL_ROOT, MISC_SHU_RANKCTL_RANKINCTL);	//RANKINCTL= DQSINCTL -3
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_RANKCTL),
		u4RankINCTL_ROOT, MISC_SHU_RANKCTL_RANKINCTL_ROOT1);  //RANKINCTL_ROOT1= DQSINCTL -3

#ifdef XRTR2R_PERFORM_ENHANCE_DQSG_RX_DLY
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_RANKCTL),
		u4RankINCTL_ROOT, MISC_SHU_RANKCTL_RANKINCTL_RXDLY);

	u4XRTR2R = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_ACTIM_XRT), SHU_ACTIM_XRT_XRTR2R);

	mcSHOW_DBG_MSG2(("TX_dly_DQSgated check: min %d  max %d, ChangeDQSINCTL=%d\n", u1TXDLY_Cal_min, u1TXDLY_Cal_max, s1ChangeDQSINCTL));
	mcSHOW_DBG_MSG2(("DQSINCTL=%d, RANKINCTL=%d, u4XRTR2R=%d\n", u4ReadDQSINCTL, u4RankINCTL_ROOT, u4XRTR2R));
	mcDUMP_REG_MSG(("DQSINCTL=%d, RANKINCTL=%d, u4XRTR2R=%d\n", u4ReadDQSINCTL, u4RankINCTL_ROOT, u4XRTR2R));
#else
	//XRTR2R=A-phy forbidden margin(6T) + reg_TX_dly_DQSgated (max) +Roundup(tDQSCKdiff/MCK+0.25MCK)+1(05T sel_ph margin)-1(forbidden margin overlap part)
	//Roundup(tDQSCKdiff/MCK+1UI) =1~2 all LP3 and LP4 timing
	//u4XRTR2R= 8 + u1TXDLY_Cal_max;  // 6+ u1TXDLY_Cal_max +2

	//Modify for corner IC failed at HQA test XTLV @ 3200MHz
	u4XRTR2R = 8 + u1TXDLY_Cal_max + 1;  // 6+ u1TXDLY_Cal_max +2
	if (u4XRTR2R > 12)
	{
		u4XRTR2R = 12;
		mcSHOW_ERR_MSG(("XRTR2R > 12, Max value is 12\n"));
	}
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_ACTIM_XRT), u4XRTR2R, SHU_ACTIM_XRT_XRTR2R);

	mcSHOW_DBG_MSG2(("TX_dly_DQSgated check: min %d  max %d, ChangeDQSINCTL=%d\n", u1TXDLY_Cal_min, u1TXDLY_Cal_max, s1ChangeDQSINCTL));
	mcSHOW_DBG_MSG2(("DQSINCTL=%d, RANKINCTL=%d, u4XRTR2R=%d\n", u4ReadDQSINCTL, u4RankINCTL_ROOT, u4XRTR2R));
	mcDUMP_REG_MSG(("DQSINCTL=%d, RANKINCTL=%d, u4XRTR2R=%d\n", u4ReadDQSINCTL, u4RankINCTL_ROOT, u4XRTR2R));
#endif

#if 0//ENABLE_RODT_TRACKING
	//Because Ki_bo+,WE2,Bi_anco,Vin_son...or behind project support WDQS, they need to apply the correct new setting
	//The following 2 items are indepentent
	//1. if TX_WDQS on(by vendor_id) or p->odt_onoff = 1, ROEN/RODTE/RODTE2 = 1
	//2. if ENABLE_RODT_TRACKING on, apply new setting and RODTENSTB_TRACK_EN = ROEN
	// LP4 support only
	U8 u1ReadROEN;
	u1ReadROEN = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_ODTCTRL), SHU_ODTCTRL_ROEN);
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_RODTENSTB), P_Fld(0xff, SHU_RODTENSTB_RODTENSTB_EXT) | \
															P_Fld(u1ReadROEN, SHU_RODTENSTB_RODTENSTB_TRACK_EN));
#endif

#ifdef XRTR2W_PERFORM_ENHANCE_RODTEN
	// LP4 support only
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_RODTENSTB),
		P_Fld(0x0fff, MISC_SHU_RODTENSTB_RODTENSTB_EXT) |
		P_Fld(1, MISC_SHU_RODTENSTB_RODTEN_P1_ENABLE) |
		P_Fld(1, MISC_SHU_RODTENSTB_RODTENSTB_TRACK_EN));
#endif

	vSetRank(p, backup_rank);


}
#endif

#if GATING_ADJUST_TXDLY_FOR_TRACKING
void DramcRxdqsGatingPreProcess(DRAMC_CTX_T *p)
{
	u1TXDLY_Cal_min = 0xff;
	u1TXDLY_Cal_max = 0;
}
#endif
///TODO: wait for porting ---

#endif

#if RDSEL_TRACKING_EN
void RDSELRunTimeTracking_preset(DRAMC_CTX_T *p)
{
	U8 u1RankIdx;
	S32 s4PosVH, s4NegVH;
	U32 u4Gating_shift=0, u4Gating_origin_B0=0, u4Gating_origin_B1=0;
	U32 u4Gating_origin_final=0xff;

	s4NegVH = divRoundClosest(400, ((1000000 / p->frequency) / 64));

	for (u1RankIdx = 0; u1RankIdx < p->support_rank_num; u1RankIdx++)
	{
		u4Gating_origin_B0 = ((ucbest_coarse_mck_backup[u1RankIdx][0] << 4) | (ucbest_coarse_ui_backup[u1RankIdx][0]));
		u4Gating_origin_B1 = ((ucbest_coarse_mck_backup[u1RankIdx][1] << 4) | (ucbest_coarse_ui_backup[u1RankIdx][1]));

		if (u4Gating_origin_B0 < u4Gating_origin_B1)
		{
			u4Gating_origin_final = (u4Gating_origin_B0 < u4Gating_origin_final) ? u4Gating_origin_B0 : u4Gating_origin_final;
		}
		else
		{
			u4Gating_origin_final = (u4Gating_origin_B1 < u4Gating_origin_final) ? u4Gating_origin_B1 : u4Gating_origin_final;
		}
	}

	u4Gating_shift = ((((u4Gating_origin_final >> 3) & 0x1f) << 4) | (u4Gating_origin_final & 7)) - u4Gating_origin_B0;

	s4PosVH = s4NegVH + (u4Gating_shift << 5);

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_MISC_RDSEL_TRACK), P_Fld(s4PosVH, SHU_MISC_RDSEL_TRACK_SHU_GW_THRD_POS)
													| P_Fld(-s4NegVH, SHU_MISC_RDSEL_TRACK_SHU_GW_THRD_NEG));
}
#endif

#if RDDQC_PINMUX_WORKAROUND
static void RDDQCPinmuxWorkaround(DRAMC_CTX_T *p)
{
	U8 *uiLPDDR_RDDQC_Mapping;
#if (__LP5_COMBO__)
	const U8 uiLPDDR5_RDDQC_Mapping_POP[CHANNEL_NUM][16] =
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
	const U8 uiLPDDR4_RDDQC_Mapping_POP[PINMUX_MAX][CHANNEL_NUM][16] =
	{
		{
		// for DSC
			//CH-A
			{
				0, 1, 6, 7, 4, 5, 3, 2,
				9, 8, 11, 10, 15, 14, 12, 13
			},
			#if (CHANNEL_NUM>1)
			//CH-B
			{
				1, 0, 5, 4, 7, 2, 3, 6,
				8, 9, 11, 10, 12, 14, 13, 15
			},
			#endif
			#if (CHANNEL_NUM>2)
			//CH-C
			{
				0, 1, 6, 7, 4, 5, 3, 2,
				9, 8, 11, 10, 15, 14, 12, 13
			},
			//CH-D
			{
				1, 0, 5, 4, 7, 2, 3, 6,
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
				8, 9, 10, 12, 15, 14, 11, 13
			},
			#if (CHANNEL_NUM>1)
			//CH-B
			{
				0, 1, 7, 4, 2, 5, 6, 3,
				9, 8, 10, 12, 11, 14, 13, 15
			},
			#endif
			#if (CHANNEL_NUM>2)
			//CH-C
			{
				1, 0, 3, 2, 4, 7, 6, 5,
				8, 9, 10, 12, 15, 14, 11, 13
			},
			//CH-D
			{
				0, 1, 7, 4, 2, 5, 6, 3,
				9, 8, 10, 12, 11, 14, 13, 15
			},
			#endif
		}
	};

	#if (__LP5_COMBO__)
	if (is_lp5_family(p))
	{
		uiLPDDR_RDDQC_Mapping = (U8 *)uiLPDDR5_RDDQC_Mapping_POP[p->channel];
	}
	else
	#endif
	{
		uiLPDDR_RDDQC_Mapping = (U8 *)uiLPDDR4_RDDQC_Mapping_POP[p->DRAMPinmux][p->channel];
	}


	//Set RDDQC pinmux
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_MRR_BIT_MUX1), P_Fld(uiLPDDR_RDDQC_Mapping[0], MRR_BIT_MUX1_MRR_BIT0_SEL) | P_Fld(uiLPDDR_RDDQC_Mapping[1], MRR_BIT_MUX1_MRR_BIT1_SEL) |
															   P_Fld(uiLPDDR_RDDQC_Mapping[2], MRR_BIT_MUX1_MRR_BIT2_SEL) | P_Fld(uiLPDDR_RDDQC_Mapping[3], MRR_BIT_MUX1_MRR_BIT3_SEL));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_MRR_BIT_MUX2), P_Fld(uiLPDDR_RDDQC_Mapping[4], MRR_BIT_MUX2_MRR_BIT4_SEL) | P_Fld(uiLPDDR_RDDQC_Mapping[5], MRR_BIT_MUX2_MRR_BIT5_SEL) |
															   P_Fld(uiLPDDR_RDDQC_Mapping[6], MRR_BIT_MUX2_MRR_BIT6_SEL) | P_Fld(uiLPDDR_RDDQC_Mapping[7], MRR_BIT_MUX2_MRR_BIT7_SEL));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_MRR_BIT_MUX3), P_Fld(uiLPDDR_RDDQC_Mapping[8], MRR_BIT_MUX3_MRR_BIT8_SEL) | P_Fld(uiLPDDR_RDDQC_Mapping[9], MRR_BIT_MUX3_MRR_BIT9_SEL) |
															   P_Fld(uiLPDDR_RDDQC_Mapping[10], MRR_BIT_MUX3_MRR_BIT10_SEL) | P_Fld(uiLPDDR_RDDQC_Mapping[11], MRR_BIT_MUX3_MRR_BIT11_SEL));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_MRR_BIT_MUX4), P_Fld(uiLPDDR_RDDQC_Mapping[12], MRR_BIT_MUX4_MRR_BIT12_SEL) | P_Fld(uiLPDDR_RDDQC_Mapping[13], MRR_BIT_MUX4_MRR_BIT13_SEL) |
															   P_Fld(uiLPDDR_RDDQC_Mapping[14], MRR_BIT_MUX4_MRR_BIT14_SEL) | P_Fld(uiLPDDR_RDDQC_Mapping[15], MRR_BIT_MUX4_MRR_BIT15_SEL));

}
#endif

#define RDDQCGOLDEN_LP5_MR30_BIT_CTRL_LOWER   RDDQCGOLDEN_MR15_GOLDEN
#define RDDQCGOLDEN_LP5_MR31_BIT_CTRL_UPPER   RDDQCGOLDEN_MR20_GOLDEN
#define RDDQCGOLDEN_LP5_MR32_PATTERN_A		  RDDQCGOLDEN_MR32_GOLDEN
#define RDDQCGOLDEN_LP5_MR33_PATTERN_B		  RDDQCGOLDEN_MR40_GOLDEN
U32 DramcRxWinRDDQCInit(DRAMC_CTX_T *p)
{
	U8 RDDQC_Bit_Ctrl_Lower = 0x55;
	U8 RDDQC_Bit_Ctrl_Upper = 0x55;
	U8 RDDQC_Pattern_A = 0x5A;
	U8 RDDQC_Pattern_B = 0x3C;

#if FOR_DV_SIMULATION_USED == 1
	cal_sv_rand_args_t *psra = get_psra();

	if (psra) {
		RDDQC_Bit_Ctrl_Lower = psra->low_byte_invert_golden & 0xFF;
		RDDQC_Bit_Ctrl_Upper = psra->upper_byte_invert_golden & 0xFF;
		RDDQC_Pattern_A = psra->mr_dq_a_golden;
		RDDQC_Pattern_B = psra->mr_dq_b_golden;

		/*
		 * TODO
		 *
		 * sv also passes mr20_6 and mr20_7 to sa.
		 * currently, sa does NOT use these two random arguments.
		 */
	}
#endif /* FOR_DV_SIMULATION_USED == 1 */

	// Disable Read DBI
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ7), 0, SHU_B0_DQ7_R_DMDQMDBI_SHU_B0);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ7), 0, SHU_B1_DQ7_R_DMDQMDBI_SHU_B1);

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL0), u1GetRank(p), SWCMD_CTRL0_MRSRK);

#if RDDQC_PINMUX_WORKAROUND
	// Translate pin order by MRR bit sel
	RDDQCPinmuxWorkaround(p);
#endif

	// Set golden values into dram MR
#if (__LP5_COMBO__ == TRUE)
	if (TRUE == is_lp5_family(p))
	{
		DramcModeRegWriteByRank(p, p->rank, 31, RDDQC_Bit_Ctrl_Lower);
		DramcModeRegWriteByRank(p, p->rank, 32, RDDQC_Bit_Ctrl_Upper);
		DramcModeRegWriteByRank(p, p->rank, 33, RDDQC_Pattern_A);
		DramcModeRegWriteByRank(p, p->rank, 34, RDDQC_Pattern_B);
	}
	else
#endif
	{
		DramcModeRegWriteByRank(p, p->rank, 15, RDDQC_Bit_Ctrl_Lower);
		DramcModeRegWriteByRank(p, p->rank, 20, RDDQC_Bit_Ctrl_Upper);
		DramcModeRegWriteByRank(p, p->rank, 32, RDDQC_Pattern_A);
		DramcModeRegWriteByRank(p, p->rank, 40, RDDQC_Pattern_B);
	}

	//Set golden values into RG, watch out the MR_index of RGs are reference LP4
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_RDDQCGOLDEN),
			P_Fld(RDDQC_Bit_Ctrl_Lower, RDDQCGOLDEN_LP5_MR30_BIT_CTRL_LOWER) |
			P_Fld(RDDQC_Bit_Ctrl_Upper, RDDQCGOLDEN_LP5_MR31_BIT_CTRL_UPPER) |
			P_Fld(RDDQC_Pattern_A, RDDQCGOLDEN_LP5_MR32_PATTERN_A) |
			P_Fld(RDDQC_Pattern_B, RDDQCGOLDEN_LP5_MR33_PATTERN_B));

	// Open gated clock, by KaiHsin   (DCM)
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ8),
			P_Fld(1, SHU_B0_DQ8_R_DMRXDLY_CG_IG_B0));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ8),
			P_Fld(1, SHU_B1_DQ8_R_DMRXDLY_CG_IG_B1));

#if (__LP5_COMBO__ == TRUE)
	if (is_lp5_family(p))
	{
		// Set function mode applied to DQ & DMI
//		  U8 RDDQC_RDC_DQ_mode = 0;
//		  U8 RDDQC_RDC_DMI_mode = 0;

//		  vSetLP5DramRDDQC_DQandDMI(p, RDDQC_RDC_DQ_mode, RDDQC_RDC_DMI_mode);

//		  vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_RDDQCGOLDEN1),
//				  P_Fld(RDDQC_RDC_DQ_mode, RDDQCGOLDEN1_LP5_MR20_7_GOLDEN) |
//				  P_Fld(RDDQC_RDC_DMI_mode, RDDQCGOLDEN1_LP5_MR20_6_GOLDEN));

		if (is_heff_mode(p) == FALSE)
		{
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_COMMON0), 1, SHU_COMMON0_LP5WCKON);

			// Enable MR18 "WCK always ON mode"
			vSetLP5Dram_WCKON_OnOff(p, ON);
		}

		RunTime_SW_Cmd(p, RUNTIME_SWCMD_CAS_FS);
	}
#endif
	return 0;
}

U32 DramcRxWinRDDQCEnd(DRAMC_CTX_T *p)
{
	// Recover MPC Rank
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL0), 0, SWCMD_CTRL0_MRSRK);

#if (__LP5_COMBO__ == TRUE)
	if (is_lp5_family(p))
	{
		RunTime_SW_Cmd(p, RUNTIME_SWCMD_CAS_OFF);

		if (is_heff_mode(p) == FALSE)
		{
			// Disable MR18 "WCK always ON mode"
			vSetLP5Dram_WCKON_OnOff(p, OFF);

			vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_COMMON0), 0, SHU_COMMON0_LP5WCKON);
		}
	}
#endif
	return 0;
}

/* Issue "RD DQ Calibration"
 * 1. SWCMD_CTRL1_RDDQC_LP_ENB = 1 to stop RDDQC burst
 * 2. RDDQCEN = 1 for RDDQC
 * 3. Wait rddqc_response = 1
 * 4. Read compare result
 * 5. RDDQCEN = 0
 */
 U32 DramcRxWinRDDQCRun(DRAMC_CTX_T *p)
{
	U32 u4Result = 0, u4TmpResult;
	DRAM_STATUS_T u4Response = DRAM_FAIL;

	//Issue RD DQ calibration
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL1), 1, SWCMD_CTRL1_RDDQC_LP_ENB);

	// Trigger and wait
	REG_TRANSFER_T TriggerReg = {DRAMC_REG_SWCMD_EN, SWCMD_EN_RDDQCEN};
	REG_TRANSFER_T RepondsReg = {DRAMC_REG_SPCMDRESP, SPCMDRESP_RDDQC_RESPONSE};
	u4Response = DramcTriggerAndWait(p, TriggerReg, RepondsReg);

	// Read RDDQC compare result
	u4TmpResult = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_RDQC_CMP));
	u4Result = (0xFFFF) & ((u4TmpResult) | (u4TmpResult >> 16)); // (BL0~7) | (BL8~15)

#if (FEATURE_RDDQC_K_DMI == TRUE)
	// Read DQM compare result
	u4TmpResult = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_RDQC_DQM_CMP), RDQC_DQM_CMP_RDDQC_DQM_CMP0_ERR);
	u4TmpResult |= u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_RDQC_DQM_CMP), RDQC_DQM_CMP_RDDQC_DQM_CMP1_ERR);
	u4Result |= (u4TmpResult << 16);
#endif

	//R_DMRDDQCEN -> 0
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), 0, SWCMD_EN_RDDQCEN);

	return u4Result;
}

DRAM_STATUS_T DramcRxWindowPerbitCal(DRAMC_CTX_T *p,
											  RX_PATTERN_OPTION_T u1UseTestEngine,
											  U8 *u1AssignedVref,
											  u8 isAutoK)
{
	U8 u1BitIdx, u1ByteIdx;
	U16 u16DelayStep = 1;
	PASS_WIN_DATA_T FinalWinPerBit[DQ_DATA_WIDTH + RDDQC_ADD_DMI_NUM];
	S32 iDQSDlyPerbyte[DQS_NUMBER], iDQMDlyPerbyte[DQS_NUMBER];//, iFinalDQSDly[DQS_NUMBER];
	U8 u1VrefScanEnable = FALSE;
	U16 u2FinalVref [DQS_NUMBER]= {0xe, 0xe};
	U16 u2VrefBegin, u2VrefEnd, u2VrefStep;
	U8	u1RXEyeScanEnable=0,u1PrintCalibrationProc;
	U8 u1CalDQMNum = 0;

#if ENABLE_EYESCAN_GRAPH
	U8 EyeScan_index[DQ_DATA_WIDTH_LP4 + RDDQC_ADD_DMI_NUM] = {0};
	U8 u1pass_in_this_vref_flag[DQ_DATA_WIDTH_LP4 + RDDQC_ADD_DMI_NUM];
#endif

	U8 backup_rank, rank_i, u1KnownVref[2]={0xff, 0xff};

	// error handling
	if (!p)
	{
		mcSHOW_ERR_MSG(("context NULL\n"));
		return DRAM_FAIL;
	}

#if RDDQC_PINMUX_WORKAROUND
	U32 u4RegBackupAddress[] =
	{
		(DRAMC_REG_ADDR(DRAMC_REG_MRR_BIT_MUX1)),
		(DRAMC_REG_ADDR(DRAMC_REG_MRR_BIT_MUX2)),
		(DRAMC_REG_ADDR(DRAMC_REG_MRR_BIT_MUX3)),
		(DRAMC_REG_ADDR(DRAMC_REG_MRR_BIT_MUX4)),
	};

	//Back up dramC register
	DramcBackupRegisters(p, u4RegBackupAddress, ARRAY_SIZE(u4RegBackupAddress));
#endif

	if (u1UseTestEngine == PATTERN_TEST_ENGINE)
		u1RXEyeScanEnable = GetEyeScanEnable(p, 1);

#if (FEATURE_RDDQC_K_DMI == TRUE)
	if (u1UseTestEngine == PATTERN_RDDQC)
	{
		u1CalDQMNum = 2;
		iDQMDlyPerbyte[0] = -0xFFFFFF;
		iDQMDlyPerbyte[1] = -0xFFFFFF;
	}
	else
#endif
	{
		u1CalDQMNum = 0;
		iDQMDlyPerbyte[0] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_RXDLY4), SHU_R0_B0_RXDLY4_RX_ARDQM0_R_DLY_B0);
		iDQMDlyPerbyte[1] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_RXDLY4), SHU_R0_B1_RXDLY4_RX_ARDQM0_R_DLY_B1);

	}

#if ENABLE_EYESCAN_GRAPH
	if (u1UseTestEngine == PATTERN_TEST_ENGINE)
	{
		for(u1vrefidx=0; u1vrefidx<EYESCAN_RX_VREF_RANGE_END;u1vrefidx++)
		{
			for (u1BitIdx = 0; u1BitIdx < DQ_DATA_WIDTH_LP4; u1BitIdx++)
			{
				for(ii=0; ii<EYESCAN_BROKEN_NUM; ii++)
				{
					gEyeScan_Min[u1vrefidx][u1BitIdx][ii] = EYESCAN_DATA_INVALID;
					gEyeScan_Max[u1vrefidx][u1BitIdx][ii] = EYESCAN_DATA_INVALID;

					gEyeScan_ContinueVrefHeight[u1BitIdx] = 0;
					gEyeScan_TotalPassCount[u1BitIdx] = 0;
				}
			}
		}
	}
#endif


	//When doing RxWindowPerbitCal, should make sure that auto refresh is disable
	vAutoRefreshSwitch(p, DISABLE);
	//CKEFixOnOff(p, p->rank, CKE_FIXON, CKE_WRITE_TO_ONE_CHANNEL);

	backup_rank = u1GetRank(p);

	//defult set result fail. When window found, update the result as oK
	if (u1UseTestEngine == PATTERN_TEST_ENGINE)
	{
		vSetCalibrationResult(p, DRAM_CALIBRATION_RX_PERBIT, DRAM_FAIL);

		// Something wrong with TA2 pattern -- SI, which causes RX autoK fail.
		if (isAutoK == TRUE)
		{
			DramcEngine2Init(p, p->test2_1, p->test2_2, TEST_XTALK_PATTERN, 0, TE_NO_UI_SHIFT);
		}
		else
		{
#if ENABLE_K_WITH_WORST_SI_UI_SHIFT
			DramcEngine2Init(p, p->test2_1, p->test2_2, p->test_pattern | 0x80, 0, TE_UI_SHIFT);//UI_SHIFT + LEN1
#else
			DramcEngine2Init(p, p->test2_1, p->test2_2, p->test_pattern, 0, TE_NO_UI_SHIFT);
#endif
		}
	}
	else
	{
		vSetCalibrationResult(p, DRAM_CALIBRATION_RX_RDDQC, DRAM_FAIL);
		DramcRxWinRDDQCInit(p);
	}

	// Intialize, diable RX Vref
	u2VrefBegin = 0;
	u2VrefEnd = 0;
	u2VrefStep = 1;

	if ((u1UseTestEngine == PATTERN_TEST_ENGINE))
	{
	#if (FOR_DV_SIMULATION_USED==0 && SW_CHANGE_FOR_SIMULATION==0)
		if ((p->rank==RANK_0) || (p->frequency >= RX_VREF_DUAL_RANK_K_FREQ) || (u1RXEyeScanEnable==1))
			u1VrefScanEnable =1;
	#else
			u1VrefScanEnable =0;
	#endif
	}

	u1PrintCalibrationProc = ((u1VrefScanEnable == 0) || (u1RXEyeScanEnable == 1) || (u1AssignedVref != NULL));

#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
	if (p->femmc_Ready == 1 && ((p->Bypass_RDDQC && u1UseTestEngine == PATTERN_RDDQC) || (p->Bypass_RXWINDOW && u1UseTestEngine == PATTERN_TEST_ENGINE)))
	{
		mcSHOW_DBG_MSG(("[FAST_K] Bypass RX Calibration\n"));
	}
	else
#endif
	{
#if VENDER_JV_LOG
#if 0 //BU don't want customer knows our RX's ability
		if (u1UseTestEngine == 1)
			vPrintCalibrationBasicInfo_ForJV(p);
#endif
#else
		vPrintCalibrationBasicInfo(p);
#endif
		mcSHOW_DBG_MSG2(("Start DQ dly to find pass range UseTestEngine =%d\n", u1UseTestEngine));
	}

	mcSHOW_DBG_MSG2(("UseTestEngine: %d\n", u1UseTestEngine));
	mcSHOW_DBG_MSG(("RX Vref Scan: %d\n", u1VrefScanEnable));

	if (u1VrefScanEnable)
	{
		if ((Get_Vref_Calibration_OnOff(p) == VREF_CALI_OFF) && (u1RXEyeScanEnable == 0))
		{
			u2VrefBegin = 0;
			u2VrefEnd = 0;
			u1KnownVref[0] = gFinalRXVrefDQForSpeedUp[p->channel][p->rank][p->odt_onoff][0];// byte 0
			u1KnownVref[1] = gFinalRXVrefDQForSpeedUp[p->channel][p->rank][p->odt_onoff][1];// byte 1

			if (u1UseTestEngine == PATTERN_TEST_ENGINE && ((u1KnownVref[0] == 0) || (u1KnownVref[1] == 0)))
			{
//				  mcSHOW_ERR_MSG(("\nWrong frequency K order= %d\n"));
				#if __ETT__
				while (1);
				#endif
			}
		}
		else if (u1AssignedVref != NULL)  // need to specify RX Vref and don't scan RX Vref.
		{
			u2VrefBegin = 0;
			u2VrefEnd = 0;
			u1KnownVref[0] = u1AssignedVref[0];  // byte 0
			u1KnownVref[1] = u1AssignedVref[1];  // byte 1
		}
		else
		{
			#if (SW_CHANGE_FOR_SIMULATION || FOR_DV_SIMULATION_USED)
			u2VrefBegin = RX_VREF_RANGE_BEGIN;
			#else
			if (u1RXEyeScanEnable == 0)
			{
				if (p->odt_onoff)
				{
					u2VrefBegin = RX_VREF_RANGE_BEGIN_ODT_ON;
				}
				else
				{
					u2VrefBegin = RX_VREF_RANGE_BEGIN_ODT_OFF;
				}
				u2VrefEnd = RX_VREF_RANGE_END-1;
				mcSHOW_DBG_MSG(("\nSet Vref Range= %d -> %d\n",u2VrefBegin,u2VrefEnd));
			}
			else
			{
				u2VrefBegin = 0;//Lewis@20160817: Enlarge RX Vref range for eye scan
				u2VrefEnd = EYESCAN_RX_VREF_RANGE_END-1;
				mcSHOW_DBG_MSG(("\nSet Eyescan Vref Range= %d -> %d\n",u2VrefBegin,u2VrefEnd));
			}
		#endif
		}

		if (u1RXEyeScanEnable == 0)
		{
			u2VrefStep = RX_VREF_RANGE_STEP;
		}
		else
		{
			u2VrefStep = EYESCAN_GRAPH_RX_VREF_STEP;
		}

		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ5), 1, B0_DQ5_RG_RX_ARDQ_VREF_EN_B0);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ5), 1, B1_DQ5_RG_RX_ARDQ_VREF_EN_B1);
	}
	else // Disable RX Vref
	{
		u2VrefBegin = 0;
		u2VrefEnd = 0;
		u2VrefStep = 1;
	}

	//if RDDQD, roughly calibration
	if (u1UseTestEngine == PATTERN_RDDQC)
		u16DelayStep <<= 1;

#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
	if (p->femmc_Ready == 1 && ((p->Bypass_RDDQC && u1UseTestEngine == PATTERN_RDDQC) || (p->Bypass_RXWINDOW && u1UseTestEngine == PATTERN_TEST_ENGINE)))
	{
		// load RX DQS and DQM delay from eMMC
		for (u1ByteIdx = 0; u1ByteIdx < (p->data_width / DQS_BIT_NUMBER); u1ByteIdx++)
		{
			if (u1VrefScanEnable)
			{
				// load RX Vref from eMMC
			#if ( SUPPORT_SAVE_TIME_FOR_CALIBRATION && BYPASS_VREF_CAL)
				u2FinalVref[u1ByteIdx] = p->pSavetimeData->u1RxWinPerbitVref_Save[p->channel][p->rank][u1ByteIdx];
			#endif
			}

			iDQSDlyPerbyte[u1ByteIdx] = p->pSavetimeData->u1RxWinPerbit_DQS[p->channel][p->rank][u1ByteIdx];
			iDQMDlyPerbyte[u1ByteIdx] = p->pSavetimeData->u1RxWinPerbit_DQM[p->channel][p->rank][u1ByteIdx];
		}

		// load RX DQ delay from eMMC
		for (u1BitIdx = 0; u1BitIdx < 16; u1BitIdx++)
		{
			FinalWinPerBit[u1BitIdx].best_dqdly = p->pSavetimeData->u1RxWinPerbit_DQ[p->channel][p->rank][u1BitIdx];
		}

		if (u1UseTestEngine == PATTERN_TEST_ENGINE)
			vSetCalibrationResult(p, DRAM_CALIBRATION_RX_PERBIT, DRAM_FAST_K);
		else
			vSetCalibrationResult(p, DRAM_CALIBRATION_RX_RDDQC, DRAM_FAST_K);
	}
#endif

	if (u1VrefScanEnable == TRUE)
	{
		// When only calibrate RX Vref for Rank 0, apply the same value for Rank 1.
		for (rank_i = p->rank; rank_i < p->support_rank_num; rank_i++)
		{
			vSetRank(p, rank_i);

			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_PHY_VREF_SEL),
					P_Fld(u2FinalVref[0], SHU_B0_PHY_VREF_SEL_RG_RX_ARDQ_VREF_SEL_LB_B0) |
					P_Fld(u2FinalVref[0], SHU_B0_PHY_VREF_SEL_RG_RX_ARDQ_VREF_SEL_UB_B0));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_PHY_VREF_SEL),
					P_Fld(u2FinalVref[1], SHU_B1_PHY_VREF_SEL_RG_RX_ARDQ_VREF_SEL_LB_B1) |
					P_Fld(u2FinalVref[1], SHU_B1_PHY_VREF_SEL_RG_RX_ARDQ_VREF_SEL_UB_B1));

			for (u1ByteIdx = 0; u1ByteIdx < (p->data_width / DQS_BIT_NUMBER); u1ByteIdx++)
			{
				mcSHOW_DBG_MSG(("\nFinal RX Vref Byte %d = %d to rank%d", u1ByteIdx, u2FinalVref[u1ByteIdx], rank_i));
				mcDUMP_REG_MSG(("\nFinal RX Vref Byte %d = %d to rank%d", u1ByteIdx, u2FinalVref[u1ByteIdx], rank_i));

				gFinalRXVrefDQ[p->channel][rank_i][u1ByteIdx] = (U8) u2FinalVref[u1ByteIdx];
				gFinalRXVrefDQForSpeedUp[p->channel][rank_i][p->odt_onoff][u1ByteIdx] = (U8) u2FinalVref[u1ByteIdx];
			}
		}
		vSetRank(p, backup_rank);
	}

#if DUMP_TA2_WINDOW_SIZE_RX_TX
	//RX
	if (u1UseTestEngine == PATTERN_TEST_ENGINE)
	{
		U32 u4B0Tatal =0;
		U32 u4B1Tatal =0;
		mcSHOW_DBG_MSG(("RX window per bit CH[%d] Rank[%d] window size\n", p->channel, p->rank));
		for (u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
		{
			mcSHOW_DBG_MSG(("DQ[%d] size = %d\n", u1BitIdx, gFinalRXPerbitWin[p->channel][p->rank][u1BitIdx]));
			if(u1BitIdx < 8)
			{
				u4B0Tatal += gFinalRXPerbitWin[p->channel][p->rank][u1BitIdx];
			}
			else
			{
				u4B1Tatal += gFinalRXPerbitWin[p->channel][p->rank][u1BitIdx];
			}
		}
		mcSHOW_DBG_MSG(("total rx window size B0: %d B1: %d\n", u4B0Tatal, u4B1Tatal));
	}
#endif

	// set dqs delay, (dqm delay)
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_RXDLY5),
			P_Fld((U32)iDQSDlyPerbyte[0], SHU_R0_B0_RXDLY5_RX_ARDQS0_R_DLY_B0));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_RXDLY4),
			P_Fld((U32)iDQMDlyPerbyte[0], SHU_R0_B0_RXDLY4_RX_ARDQM0_R_DLY_B0));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_RXDLY5),
			P_Fld((U32)iDQSDlyPerbyte[1], SHU_R0_B1_RXDLY5_RX_ARDQS0_R_DLY_B1));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_RXDLY4),
			P_Fld((U32)iDQMDlyPerbyte[1], SHU_R0_B1_RXDLY4_RX_ARDQM0_R_DLY_B1));

	// set dq delay
	for (u1BitIdx = 0; u1BitIdx < DQS_BIT_NUMBER; u1BitIdx += 2)
	{
		 vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_RXDLY0 + u1BitIdx * 2),
										P_Fld(((U32)FinalWinPerBit[u1BitIdx].best_dqdly), SHU_R0_B0_RXDLY0_RX_ARDQ0_R_DLY_B0) |
										P_Fld(((U32)FinalWinPerBit[u1BitIdx + 1].best_dqdly), SHU_R0_B0_RXDLY0_RX_ARDQ1_R_DLY_B0));

		 vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_RXDLY0 + u1BitIdx * 2),
										P_Fld((U32)FinalWinPerBit[u1BitIdx + 8].best_dqdly, SHU_R0_B1_RXDLY0_RX_ARDQ0_R_DLY_B1) |
										P_Fld((U32)FinalWinPerBit[u1BitIdx + 9].best_dqdly, SHU_R0_B1_RXDLY0_RX_ARDQ1_R_DLY_B1));

		//mcSHOW_DBG_MSG(("u1BitId %d  Addr 0x%2x = %2d %2d %2d %2d \n", u1BitIdx, DDRPHY_RXDQ1+u1BitIdx*2,
		//				  FinalWinPerBit[u1BitIdx].best_dqdly, FinalWinPerBit[u1BitIdx+1].best_dqdly,  FinalWinPerBit[u1BitIdx+8].best_dqdly, FinalWinPerBit[u1BitIdx+9].best_dqdly));
	}

	DramPhyReset(p);

#if RDDQC_PINMUX_WORKAROUND
	DramcRestoreRegisters(p, u4RegBackupAddress, sizeof(u4RegBackupAddress) / sizeof(U32));
#endif

	vSetRank(p, backup_rank);

	vPrintCalibrationBasicInfo(p);

#ifdef ETT_PRINT_FORMAT
	mcSHOW_DBG_MSG(("DQS Delay:\nDQS0 = %d, DQS1 = %d\n"
					"DQM Delay:\nDQM0 = %d, DQM1 = %d\n",
						iDQSDlyPerbyte[0], iDQSDlyPerbyte[1],
						iDQMDlyPerbyte[0], iDQMDlyPerbyte[1]));
	mcDUMP_REG_MSG(("DQS Delay:\nDQS0 = %d, DQS1 = %d\n"
					"DQM Delay:\nDQM0 = %d, DQM1 = %d\n",
						iDQSDlyPerbyte[0], iDQSDlyPerbyte[1],
						iDQMDlyPerbyte[0], iDQMDlyPerbyte[1]));
#else
	mcSHOW_DBG_MSG(("DQS Delay:\nDQS0 = %2d, DQS1 = %2d\n"
					"DQM Delay:\nDQM0 = %2d, DQM1 = %2d\n",
						iDQSDlyPerbyte[0], iDQSDlyPerbyte[1],
						iDQMDlyPerbyte[0], iDQMDlyPerbyte[1]));
	mcDUMP_REG_MSG(("DQS Delay:\nDQS0 = %2d, DQS1 = %2d\n"
					"DQM Delay:\nDQM0 = %2d, DQM1 = %2d\n",
						iDQSDlyPerbyte[0], iDQSDlyPerbyte[1],
						iDQMDlyPerbyte[0], iDQMDlyPerbyte[1]));
#endif
	mcSHOW_DBG_MSG(("DQ Delay:\n"));
	mcDUMP_REG_MSG(("DQ Delay:\n"));

	for (u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx = u1BitIdx + 4)
	{
#ifdef ETT_PRINT_FORMAT
		mcSHOW_DBG_MSG(("DQ%d =%d, DQ%d =%d, DQ%d =%d, DQ%d =%d\n", u1BitIdx, FinalWinPerBit[u1BitIdx].best_dqdly, u1BitIdx+1, FinalWinPerBit[u1BitIdx+1].best_dqdly, u1BitIdx+2, FinalWinPerBit[u1BitIdx+2].best_dqdly, u1BitIdx+3, FinalWinPerBit[u1BitIdx+3].best_dqdly));
		mcDUMP_REG_MSG(("DQ%d =%d, DQ%d =%d, DQ%d =%d, DQ%d =%d\n", u1BitIdx, FinalWinPerBit[u1BitIdx].best_dqdly, u1BitIdx+1, FinalWinPerBit[u1BitIdx+1].best_dqdly, u1BitIdx+2, FinalWinPerBit[u1BitIdx+2].best_dqdly, u1BitIdx+3, FinalWinPerBit[u1BitIdx+3].best_dqdly));
#else
		mcSHOW_DBG_MSG(("DQ%2d =%2d, DQ%2d =%2d, DQ%2d =%2d, DQ%2d =%2d\n", u1BitIdx, FinalWinPerBit[u1BitIdx].best_dqdly, u1BitIdx+1, FinalWinPerBit[u1BitIdx+1].best_dqdly, u1BitIdx+2, FinalWinPerBit[u1BitIdx+2].best_dqdly, u1BitIdx+3, FinalWinPerBit[u1BitIdx+3].best_dqdly));
		mcDUMP_REG_MSG(("DQ%2d =%2d, DQ%2d =%2d, DQ%2d =%2d, DQ%2d =%2d\n", u1BitIdx, FinalWinPerBit[u1BitIdx].best_dqdly, u1BitIdx+1, FinalWinPerBit[u1BitIdx+1].best_dqdly, u1BitIdx+2, FinalWinPerBit[u1BitIdx+2].best_dqdly, u1BitIdx+3, FinalWinPerBit[u1BitIdx+3].best_dqdly));
#endif
	}
	mcSHOW_DBG_MSG(("\n\n"));
	mcSHOW_DBG_MSG3(("[DramcRxWindowPerbitCal] Done\n"));

	#if LP5_DDR4266_RDBI_WORKAROUND
	if((is_lp5_family(p)) && (p->frequency >= 2133))
	{
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ7), 1, SHU_B0_DQ7_R_DMDQMDBI_SHU_B0);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ7), 1, SHU_B1_DQ7_R_DMDQMDBI_SHU_B1);
	}
	#endif

return DRAM_OK;

	// Log example	==> Neec to update
	/*
------------------------------------------------------
Start calculate dq time and dqs time /
Find max DQS delay per byte / Adjust DQ delay to align DQS...
------------------------------------------------------
bit# 0 : dq time=11 dqs time= 8
bit# 1 : dq time=11 dqs time= 8
bit# 2 : dq time=11 dqs time= 6
bit# 3 : dq time=10 dqs time= 8
bit# 4 : dq time=11 dqs time= 8
bit# 5 : dq time=10 dqs time= 8
bit# 6 : dq time=11 dqs time= 8
bit# 7 : dq time= 9 dqs time= 6
----seperate line----
bit# 8 : dq time=12 dqs time= 7
bit# 9 : dq time=10 dqs time= 8
bit#10 : dq time=11 dqs time= 8
bit#11 : dq time=10 dqs time= 8
bit#12 : dq time=11 dqs time= 8
bit#13 : dq time=11 dqs time= 8
bit#14 : dq time=11 dqs time= 8
bit#15 : dq time=12 dqs time= 8
----seperate line----
bit#16 : dq time=11 dqs time= 7
bit#17 : dq time=10 dqs time= 8
bit#18 : dq time=11 dqs time= 7
bit#19 : dq time=11 dqs time= 6
bit#20 : dq time=10 dqs time= 9
bit#21 : dq time=11 dqs time=10
bit#22 : dq time=11 dqs time=10
bit#23 : dq time= 9 dqs time= 9
----seperate line----
bit#24 : dq time=12 dqs time= 6
bit#25 : dq time=13 dqs time= 6
bit#26 : dq time=13 dqs time= 7
bit#27 : dq time=11 dqs time= 7
bit#28 : dq time=12 dqs time= 8
bit#29 : dq time=10 dqs time= 8
bit#30 : dq time=13 dqs time= 7
bit#31 : dq time=11 dqs time= 8
----seperate line----
==================================================
	dramc_rxdqs_perbit_swcal_v2
	channel=2(2:cha, 3:chb) apply = 1
==================================================
DQS Delay :
 DQS0 = 0 DQS1 = 0 DQS2 = 0 DQS3 = 0
DQ Delay :
DQ 0 =	1 DQ 1 =  1 DQ 2 =	2 DQ 3 =  1
DQ 4 =	1 DQ 5 =  1 DQ 6 =	1 DQ 7 =  1
DQ 8 =	2 DQ 9 =  1 DQ10 =	1 DQ11 =  1
DQ12 =	1 DQ13 =  1 DQ14 =	1 DQ15 =  2
DQ16 =	2 DQ17 =  1 DQ18 =	2 DQ19 =  2
DQ20 =	0 DQ21 =  0 DQ22 =	0 DQ23 =  0
DQ24 =	3 DQ25 =  3 DQ26 =	3 DQ27 =  2
DQ28 =	2 DQ29 =  1 DQ30 =	3 DQ31 =  1
_______________________________________________________________
   */
}

#if SIMULATION_RX_DVS
static U8 DramcRxDVSCal(DRAMC_CTX_T *p, U8 u1byte)
{
	U8 u1rising_lead, u1falling_lead, u1rising_lag, u1falling_lag, u1lead_lag;

	if (u1byte == 0)
	{
		u1rising_lead = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_FT_STATUS0), MISC_FT_STATUS0_AD_RX_ARDQ_DVS_R_LEAD_B0);
		u1falling_lead = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_FT_STATUS1), MISC_FT_STATUS1_AD_RX_ARDQ_DVS_F_LEAD_B0);
		u1rising_lag = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_FT_STATUS0), MISC_FT_STATUS0_AD_RX_ARDQ_DVS_R_LAG_B0);
		u1falling_lag = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_FT_STATUS1), MISC_FT_STATUS1_AD_RX_ARDQ_DVS_F_LAG_B0);
	}
	else //byte1
	{
		u1rising_lead = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_FT_STATUS0), MISC_FT_STATUS0_AD_RX_ARDQ_DVS_R_LAG_B1);
		u1falling_lead = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_FT_STATUS1), MISC_FT_STATUS1_AD_RX_ARDQ_DVS_F_LEAD_B1);
		u1rising_lag = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_FT_STATUS0), MISC_FT_STATUS0_AD_RX_ARDQ_DVS_R_LAG_B1);
		u1falling_lag = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_FT_STATUS1), MISC_FT_STATUS1_AD_RX_ARDQ_DVS_F_LAG_B1);
	}

	mcSHOW_DBG_MSG2(("Byte%d | LEAD(%d %d) | LAG(%d %d)\n", u1byte, u1rising_lead, u1falling_lead, u1rising_lag, u1falling_lag));

	u1lead_lag = (u1rising_lead | u1falling_lead | u1rising_lag | u1falling_lag);

	return u1lead_lag;
}

DRAM_STATUS_T DramcRxDVSWindowCal(DRAMC_CTX_T *p)
{
	U8 ii, u1ByteIdx;
	S16 iDelay = 0, S16DelayBegin = 0;
	U16 u16DelayEnd = 0, u16DelayStep = 1;
	U32 u4err_value;

	U8 u1lead_lag, u1DVS_first_flag[DQS_NUMBER_LP4]={0}, u1DVS_first_pass[DQS_NUMBER_LP4]={0}, u1DVS_pass_window[DQS_NUMBER_LP4]={0}, u1finish_flag[DQS_NUMBER_LP4]={0};
	U32 u4RegBackupAddress[] =
	{
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ11)),
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ11)),
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_RXDLY0)),
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_RXDLY0)),
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_RXDLY5)),
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_RXDLY5)),
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_RXDLY4)),
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_RXDLY4)),
	};

	// error handling
	if (!p)
	{
		mcSHOW_ERR_MSG(("context NULL\n"));
		return DRAM_FAIL;
	}

	mcSHOW_DBG_MSG(("\\\RX DVS calibration\\\\n"));

	//When doing RxWindowPerbitCal, should make sure that auto refresh is disable
	vAutoRefreshSwitch(p, DISABLE);
	//CKEFixOnOff(p, p->rank, CKE_FIXON, CKE_WRITE_TO_ONE_CHANNEL);

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ11), 1, SHU_B0_DQ11_RG_RX_ARDQ_DVS_EN_B0);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ11), 1, SHU_B1_DQ11_RG_RX_ARDQ_DVS_EN_B1);

	//defult set result fail. When window found, update the result as oK
#if ENABLE_K_WITH_WORST_SI_UI_SHIFT
	DramcEngine2Init(p, p->test2_1, p->test2_2, p->test_pattern | 0x80, 0, TE_UI_SHIFT);//PIC Need to check if need to use UI_SHIFT;//UI_SHIFT + LEN1
#else
	DramcEngine2Init(p, p->test2_1, p->test2_2, TEST_XTALK_PATTERN, 0, TE_NO_UI_SHIFT);
#endif


#if (__LP5_COMBO__ == TRUE)
	if (is_lp5_family(p))
	{
		// 1 step = 1/4 delay cell
		// Adjust step = 1/2/4(precision adjustment) by data-rate
		if (p->frequency <= GetFreqBySel(p,LP5_DDR3200))
			u16DelayStep = 4;
		else if (p->frequency <= GetFreqBySel(p,LP5_DDR4800)) // 3733, 4266, 4800
			u16DelayStep = 2;
		else // 5500, 6000, 6400
			u16DelayStep = 1;
	}
#endif
	else
	{
		u16DelayStep = 4;
	}
	// Just for DV SIM test
	S16DelayBegin = -80;
	u16DelayEnd = 100;

	mcSHOW_DBG_MSG(("\nRX Delay %d -> %d, step: %d\n", S16DelayBegin, u16DelayEnd, u16DelayStep));

	{
		// Adjust DQM output delay to 0
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_RXDLY4),
				P_Fld(0, SHU_R0_B0_RXDLY4_RX_ARDQM0_R_DLY_B0) |
				P_Fld(0, SHU_R0_B0_RXDLY4_RX_ARDQM0_F_DLY_B0));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_RXDLY4),
				P_Fld(0, SHU_R0_B1_RXDLY4_RX_ARDQM0_R_DLY_B1) |
				P_Fld(0, SHU_R0_B1_RXDLY4_RX_ARDQM0_F_DLY_B1));

		// Adjust DQ output delay to 0
		//every 2bit dq have the same delay register address
		for (ii = 0; ii < 4; ii++)
			SetRxDqDelay(p, ii, 0);
		{
			// non-autok flow
			for (iDelay = S16DelayBegin; iDelay <= u16DelayEnd; iDelay += u16DelayStep)
			{
				SetRxDqDqsDelay(p, iDelay);

				u4err_value = DramcEngine2Run(p, TE_OP_WRITE_READ_CHECK, p->test_pattern);

				mcSHOW_DBG_MSG2(("iDelay= %4d, err_value: 0x%x", iDelay, u4err_value));

				for(u1ByteIdx=0; u1ByteIdx<(p->data_width/DQS_BIT_NUMBER); u1ByteIdx++)
				{
					u1lead_lag = DramcRxDVSCal(p, u1ByteIdx);

					if ((u1lead_lag == 0) && (u1DVS_first_flag[u1ByteIdx] == 0) && (((u4err_value >> (u1ByteIdx<<3)) & 0xff) == 0))
					{
						u1DVS_first_pass[u1ByteIdx] = iDelay;
						u1DVS_first_flag[u1ByteIdx] = 1;
						mcSHOW_DBG_MSG(("Byte%d find first pass delay\n"))
					}
					else if (((u1lead_lag == 1) || (((u4err_value >> (u1ByteIdx<<3)) & 0xff) != 0)) && (u1DVS_first_flag[u1ByteIdx] == 1) && (u1finish_flag[u1ByteIdx] == 0))
					{
						u1DVS_pass_window[u1ByteIdx] = iDelay - u1DVS_first_pass[u1ByteIdx] - u16DelayStep;

						if (u1DVS_pass_window[u1ByteIdx] < 7) //if window size bigger than 7, consider as real pass window.
						{
							u1DVS_pass_window[u1ByteIdx] = 0;
							u1DVS_first_flag[u1ByteIdx] = 0;
							mcSHOW_DBG_MSG(("Byte%d find fake window\n"))
						}
						else
						{
							 u1finish_flag[u1ByteIdx] = 1;
							 mcSHOW_DBG_MSG(("Byte%d find pass window\n"))
						}
					}
				}

				if ((u1finish_flag[0]==1) && (u1finish_flag[1]==1))
				{
					mcSHOW_DBG_MSG(("Two byte DVS window find, early break!\n"));
					break;
				}
			}
		}
	}

	DramcEngine2End(p);

	for (u1ByteIdx = 0; u1ByteIdx < (p->data_width / DQS_BIT_NUMBER); u1ByteIdx++)
	{
		u1DVS_increase[p->rank][u1ByteIdx] = (u1DVS_pass_window[u1ByteIdx] > 8)? ((u1DVS_pass_window[u1ByteIdx] - 8) >> 3): 0;
		mcSHOW_DBG_MSG(("\nByte %d final DVS window size(M) %d, DVS increase %d\n", u1ByteIdx, u1DVS_pass_window[u1ByteIdx], u1DVS_increase[p->rank][u1ByteIdx]));
	}

	DramcRestoreRegisters(p, u4RegBackupAddress, sizeof(u4RegBackupAddress) / sizeof(U32));

	vAutoRefreshSwitch(p, ENABLE);

	DramPhyReset(p);

	vPrintCalibrationBasicInfo(p);

	mcSHOW_DBG_MSG(("\n\n"));
	mcSHOW_DBG_MSG3(("[DramcRxDVSWindowCal] Done\n"));

return DRAM_OK;
}

void DramcDramcRxDVSCalPostProcess(DRAMC_CTX_T *p)
{
	U8 rank_i, u1ByteIdx, u1DVS_increase_final, u1DVS_dly_final[DQS_NUMBER_LP4]={0};
	U8 backup_rank = p->rank;

	for (u1ByteIdx = 0; u1ByteIdx < (p->data_width / DQS_BIT_NUMBER); u1ByteIdx++)
	{
		if (p->support_rank_num == RANK_DUAL)
			u1DVS_increase_final = (u1DVS_increase[RANK_0][u1ByteIdx] < u1DVS_increase[RANK_1][u1ByteIdx])? u1DVS_increase[RANK_0][u1ByteIdx] : u1DVS_increase[RANK_1][u1ByteIdx];
		else
			u1DVS_increase_final = u1DVS_increase[p->rank][u1ByteIdx];

		if (u1ByteIdx == 0)
		{
			u1DVS_dly_final[u1ByteIdx] = u1DVS_increase_final + (u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ11), SHU_B0_DQ11_RG_RX_ARDQ_DVS_DLY_B0));
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ11), u1DVS_dly_final[u1ByteIdx], SHU_B0_DQ11_RG_RX_ARDQ_DVS_DLY_B0);
		}
		else //byte1
		{
			u1DVS_dly_final[u1ByteIdx] = u1DVS_increase_final + (u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ11), SHU_B1_DQ11_RG_RX_ARDQ_DVS_DLY_B1));
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ11), u1DVS_dly_final[u1ByteIdx], SHU_B1_DQ11_RG_RX_ARDQ_DVS_DLY_B1);
		}
		mcSHOW_DBG_MSG(("Byte%d final DVS delay: %d\n", u1ByteIdx, u1DVS_dly_final[u1ByteIdx]));
	}

	for(rank_i=RANK_0; rank_i< p->support_rank_num; rank_i++)
	{
		vSetRank(p, rank_i);
		DramcRxWindowPerbitCal(p, PATTERN_TEST_ENGINE, DVS_CAL_KEEP_VREF, AUTOK_OFF);
	}

	if ((DramcRxDVSCal(p, 0) == 1) || (DramcRxDVSCal(p, 1) == 1)) //Prevent set wrong DV dly
	{
		mcSHOW_ERR_MSG(("Final DVS delay is out of RX window\n"));
		for (u1ByteIdx = 0; u1ByteIdx < (p->data_width / DQS_BIT_NUMBER); u1ByteIdx++)
		{
			if (u1DVS_dly_final[u1ByteIdx] > 0)
			{
				u1DVS_dly_final[u1ByteIdx] -= 1;
				if (u1ByteIdx == 0)
				{
					vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ11), u1DVS_dly_final[u1ByteIdx], SHU_B0_DQ11_RG_RX_ARDQ_DVS_DLY_B0);
				}
				else //byte1
				{
					vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ11), u1DVS_dly_final[u1ByteIdx], SHU_B1_DQ11_RG_RX_ARDQ_DVS_DLY_B1);
				}
			}
			for(rank_i=RANK_0; rank_i< p->support_rank_num; rank_i++)
			{
				vSetRank(p, rank_i);
				DramcRxWindowPerbitCal(p, PATTERN_TEST_ENGINE, DVS_CAL_KEEP_VREF, AUTOK_OFF);
			}
		}
	}

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ11), 1, SHU_B0_DQ11_RG_RX_ARDQ_DVS_EN_B0);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ11), 1, SHU_B1_DQ11_RG_RX_ARDQ_DVS_EN_B1);

	vSetRank(p, backup_rank);
}
#endif

#if SIMULATION_DATLAT
static void dle_factor_handler(DRAMC_CTX_T *p, U8 curr_val)
{
	U8 u1DATLAT_DSEL = 0;
	U8 u1DLECG_OptionEXT1 = 0;
	U8 u1DLECG_OptionEXT2 = 0;
	U8 u1DLECG_OptionEXT3 = 0;

	// If (RX_PIPE_BYPASS_ENABLE == 1) bypass RX PIPE, so RG_DATLAT_DSEL = RG_DATLAT
	// else RG_DATLAT_DSEL = RG_DATLAT - 1
	if (u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_MISC_RX_PIPE_CTRL), SHU_MISC_RX_PIPE_CTRL_RX_PIPE_BYPASS_EN))
	{
		u1DATLAT_DSEL = curr_val;
	}
	else
	{
		if (curr_val < 1)
			u1DATLAT_DSEL = curr_val;
		else
			u1DATLAT_DSEL = curr_val - 1;
	}

//	  mcSHOW_DBG_MSG(("DATLAT: %d, u1DATLAT_DSEL: %d\n", curr_val, u1DATLAT_DSEL));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_RDAT),
			P_Fld(curr_val, MISC_SHU_RDAT_DATLAT) |
			P_Fld(u1DATLAT_DSEL, MISC_SHU_RDAT_DATLAT_DSEL) |
			P_Fld(u1DATLAT_DSEL, MISC_SHU_RDAT_DATLAT_DSEL_PHY));

	// Had been adjusted for 868 already.
	//(>=8 & <14) set EXT1 =1, EXT2=0, EXT3=0
	//(>= 14 & <19) set EXT1=1, EXT2=1, EXT3=0
	//(>=19) set EXT1=1, EXT2=1, EXT3=1
	u1DLECG_OptionEXT1 = (curr_val >= 8)? (1): (0);
	u1DLECG_OptionEXT2 = (curr_val >= 14)? (1): (0);
	u1DLECG_OptionEXT3 = (curr_val >= 19)? (1): (0);
//	  mcSHOW_DBG_MSG(("u1DLECG_OptionEXT1: %d, 2 for %d, 3 for %d\n", u1DLECG_OptionEXT1, u1DLECG_OptionEXT2, u1DLECG_OptionEXT3));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_RX_CG_SET0),
			P_Fld(u1DLECG_OptionEXT1, SHU_RX_CG_SET0_READ_START_EXTEND1) |
			P_Fld(u1DLECG_OptionEXT1, SHU_RX_CG_SET0_DLE_LAST_EXTEND1) |
			P_Fld((u1DLECG_OptionEXT2), SHU_RX_CG_SET0_READ_START_EXTEND2) |
			P_Fld((u1DLECG_OptionEXT2), SHU_RX_CG_SET0_DLE_LAST_EXTEND2) |
			P_Fld((u1DLECG_OptionEXT3), SHU_RX_CG_SET0_READ_START_EXTEND3) |
			P_Fld((u1DLECG_OptionEXT3), SHU_RX_CG_SET0_DLE_LAST_EXTEND3));

	DramPhyReset(p);

}

static U8 aru1RxDatlatResult[RANK_MAX];
DRAM_STATUS_T DramcRxdatlatCal(DRAMC_CTX_T *p)
{
	U32 u4prv_register_080;
	U8 ucfirst, ucbegin, ucsum, ucbest_step;
	U16 u2DatlatBegin;

	// error handling
	if (!p)
	{
		mcSHOW_ERR_MSG(("context NULL\n"));
		return DRAM_FAIL;
	}

	mcSHOW_DBG_MSG(("\n[DATLAT]\n"
					"Freq=%d, CH%d RK%d\n\n", p->frequency, p->channel, p->rank));

	// pre-save
	// 0x07c[6:4]	DATLAT bit2-bit0
	u4prv_register_080 = u4IO32Read4B(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_RDAT));

	//default set FAIL
	vSetCalibrationResult(p, DRAM_CALIBRATION_DATLAT, DRAM_FAIL);

	// init best_step to default
	ucbest_step = (U8) u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_RDAT), MISC_SHU_RDAT_DATLAT);
	mcSHOW_DBG_MSG(("DATLAT Default: 0x%x\n", ucbest_step));
	mcDUMP_REG_MSG(("DATLAT Default: 0x%x\n", ucbest_step));

	// 1.set DATLAT 0-15 (0-21 for MT6595)
	// 2.enable engine1 or engine2
	// 3.check result  ,3~4 taps pass
	// 4.set DATLAT 2nd value for optimal

	// Initialize
	ucfirst = 0xff;
	ucbegin = 0;
	ucsum = 0;

	DramcEngine2Init(p, p->test2_1, p->test2_2, p->test_pattern | 0x80, 0, TE_UI_SHIFT);//UI_SHIFT + LEN1
	u2DatlatBegin = 0;

#if (SUPPORT_SAVE_TIME_FOR_CALIBRATION && BYPASS_DATLAT)
	if (p->femmc_Ready == 1)
	{
			ucbest_step = p->pSavetimeData->u1RxDatlat_Save[p->channel][p->rank];
	}
#endif

	aru1RxDatlatResult[p->rank] = ucbest_step;

	mcSHOW_DBG_MSG(("best_step = %d\n\n", ucbest_step));
	mcDUMP_REG_MSG(("best_step=%d\n\n", ucbest_step));

#if __A60868_TO_BE_PORTING__
#if __ETT__
	U8 _init_Datlat_value = vDramcACTimingGetDatLat(p);
	if ((_init_Datlat_value > (ucbest_step + 1)) || (_init_Datlat_value < (ucbest_step - 1)))
	{
		mcSHOW_DBG_MSG(("[WARNING!!] Datlat initial value(%d) = best_step(%d) %c %d, out of range!\n\n",
						   _init_Datlat_value,
						   ucbest_step,
						   (ucbest_step > _init_Datlat_value)? '-': '+',
						   abs(ucbest_step - _init_Datlat_value)));
		while (1);
	}
#endif
#endif

#if defined(FOR_HQA_TEST_USED) && defined(FOR_HQA_REPORT_USED)
	HQA_Log_Message_for_Report(p, p->channel, p->rank, HQA_REPORT_FORMAT2, "DATLAT", "", 0, ucbest_step, NULL);
#endif

#if (SUPPORT_SAVE_TIME_FOR_CALIBRATION && BYPASS_DATLAT)
	if (p->femmc_Ready == 1)
	{
		dle_factor_handler(p, ucbest_step);
		vSetCalibrationResult(p, DRAM_CALIBRATION_DATLAT, DRAM_FAST_K);
	}
#endif

	mcSHOW_DBG_MSG3(("[DramcRxdatlatCal] Done\n"));
	return DRAM_OK;
}

DRAM_STATUS_T DramcDualRankRxdatlatCal(DRAMC_CTX_T *p)
{
	U8 u1FinalDatlat, u1Datlat0, u1Datlat1;

	u1Datlat0 = aru1RxDatlatResult[0];
	u1Datlat1 = aru1RxDatlatResult[1];

	if (p->support_rank_num == RANK_DUAL)
	{
		if (u1Datlat0 > u1Datlat1)
		{
			u1FinalDatlat = u1Datlat0;
		}
		else
		{
			u1FinalDatlat = u1Datlat1;
		}
	}
	else
	{
		u1FinalDatlat = u1Datlat0;
	}

#if ENABLE_READ_DBI
	if (p->DBI_R_onoff[p->dram_fsp])
	{
	  u1FinalDatlat++;
	}
#endif

	dle_factor_handler(p, u1FinalDatlat);

#if RDSEL_TRACKING_EN
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_MISC_RDSEL_TRACK), u1FinalDatlat, SHU_MISC_RDSEL_TRACK_DMDATLAT_I);
#endif

	mcSHOW_DBG_MSG(("[DualRankRxdatlatCal] RK0: %d, RK1: %d, Final_Datlat %d\n", u1Datlat0, u1Datlat1, u1FinalDatlat));

	return DRAM_OK;

}
#endif // SIMULATION_DATLAT

#if SIMULATION_TX_PERBIT

//=============================================================
///// DramC TX perbi calibration ----------Begin--------------
//=============================================================
//-------------------------------------------------------------------------
/** DramcTxWindowPerbitCal (v2)
 *	TX DQS per bit SW calibration.
 *	@param p				Pointer of context created by DramcCtxCreate.
 *	@param	apply			(U8): 0 don't apply the register we set  1 apply the register we set ,default don't apply.
 *	@retval status			(DRAM_STATUS_T): DRAM_OK or DRAM_FAIL
 */
//-------------------------------------------------------------------------
#if (SW_CHANGE_FOR_SIMULATION || FOR_DV_SIMULATION_USED)
#define TX_VREF_RANGE_BEGIN 	  0
#define TX_VREF_RANGE_END			2 // binary 110010
#define TX_VREF_RANGE_STEP		   2
#else
#define TX_VREF_RANGE_BEGIN 	  16
#define TX_VREF_RANGE_END			50 // binary 110010
#define TX_VREF_RANGE_STEP		   2
#endif

#define TX_DQ_UI_TO_PI_TAP		   64 // 1 PI = tCK/64, total 128 PI, 1UI = 32 PI
#define TX_PHASE_DQ_UI_TO_PI_TAP		 32 // 1 PI = tCK/64, total 128 PI, 1UI = 32 PI for DDR800 semi open loop mode
#define LP4_TX_VREF_DATA_NUM 50
#define LP4_TX_VREF_PASS_CONDITION 0
#define TX_PASS_WIN_CRITERIA	7
#define LP4_TX_VREF_BOUNDARY_NOT_READY 0xff

typedef struct _PASS_WIN_DATA_BY_VREF_T
{
	U16 u2VrefUsed;
	U16 u2WinSum_byVref;
	U8 u1WorseBitWinSize_byVref;
	U8 u1WorseBitIdx_byVref;
} PASS_WIN_DATA_BY_VREF_T;

static void TxWinTransferDelayToUIPI(DRAMC_CTX_T *p, U16 uiDelay, U8 u1AdjustPIToCenter, U8* pu1UILarge_DQ, U8* pu1UISmall_DQ, U8* pu1PI, U8* pu1UILarge_DQOE, U8* pu1UISmall_DQOE)
{
	U8 u1Small_ui_to_large, u1PI = 0, u164PIto1UI, u1TxDQOEShift = 0;
	U16 u2TmpValue, u2DQOE_shift;
	DDR800_MODE_T eDdr800Mode = vGet_DDR_Loop_Mode(p);
	U8 u1PiTap = (u1IsPhaseMode(p) == TRUE) ? TX_PHASE_DQ_UI_TO_PI_TAP : TX_DQ_UI_TO_PI_TAP;

	u1Small_ui_to_large = u1MCK2UI_DivShift(p);

	#if ENABLE_WDQS_MODE_2
	u1TxDQOEShift = WDQSMode2AcTxOEShift(p);
	#else
	u1TxDQOEShift = TX_DQ_OE_SHIFT_LP4;
	#endif

	if(pu1PI != NULL)
	{
		u1PI = uiDelay & (u1PiTap-1);
		*pu1PI =u1PI;
	}

	if (u1IsLP4Div4DDR800(p) /*DDR800 close loop mode*/ || u1IsPhaseMode(p))
		u164PIto1UI = 0;
	else
		u164PIto1UI = 1;

	u2TmpValue = (uiDelay /u1PiTap)<<u164PIto1UI; // 1:8 mode for 2UI carry, DDR800 1:4 mode for 1UI carry

	if (u1AdjustPIToCenter && (pu1PI != NULL) && (eDdr800Mode == CLOSE_LOOP_MODE))
	{
		if (u1PI < 10)
		{
			u1PI += (u1PiTap) >> 1;
			u2TmpValue --;
		}
		else if (u1PI > u1PiTap - 10)
		{
			u1PI -= (u1PiTap) >> 1;
			u2TmpValue ++;
		}

		*pu1PI = u1PI;
	}

	#if 0
	*pu1UISmall_DQ = u2TmpValue % u1Small_ui_to_large;
	*pu1UILarge_DQ = u2TmpValue / u1Small_ui_to_large;
	#else
	*pu1UISmall_DQ = u2TmpValue - ((u2TmpValue >> u1Small_ui_to_large) << u1Small_ui_to_large);
	*pu1UILarge_DQ = (u2TmpValue >> u1Small_ui_to_large);
	#endif
	// calculate DQ OE according to DQ UI
	#if (__LP5_COMBO__ == TRUE)
	if (TRUE == is_lp5_family(p))
	{
		u2TmpValue -= TX_DQ_OE_SHIFT_LP5;
	}
	else
	#endif
	{
		u2TmpValue -= u1TxDQOEShift;
	}

	if(((u1MR03Value[p->dram_fsp]&0x80)>>7)==1) //if WDBI is on, OE_DLY don't need to shift 1 MCK with DLY
	{
		if (vGet_Div_Mode(p) == DIV4_MODE)
			u2DQOE_shift = 4; //OE_shift = OE_shift - 3(original OE position) + 4 (MCK)
		else
			u2DQOE_shift = 8; //OE_shift = OE_shift - 3(original OE position) + 8 (MCK)

		u2TmpValue += u2DQOE_shift;
	}

	*pu1UISmall_DQOE = u2TmpValue - ((u2TmpValue >> u1Small_ui_to_large) << u1Small_ui_to_large);
	*pu1UILarge_DQOE = (u2TmpValue >> u1Small_ui_to_large);
}

static void TXPerbitCalibrationInit(DRAMC_CTX_T *p, U8 calType)
{
	//Set TX delay chain to 0
	if (calType != TX_DQ_DQS_MOVE_DQM_ONLY)
	{
	#if 1
	#if PINMUX_AUTO_TEST_PER_BIT_TX
		if(gTX_check_per_bit_flag == 1)
		{
			//not reset delay cell
		}
		else
	#endif
		{
			vIO32Write4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_TXDLY0), 0);
			vIO32Write4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_TXDLY1), 0);
			vIO32Write4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_TXDLY0), 0);
			vIO32Write4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_TXDLY1), 0);
		}
	#else
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B0_DQ0), P_Fld(0x0, SHU_R0_B0_DQ0_RK0_TX_ARDQ7_DLY_B0)
			| P_Fld(0x0, SHU_R0_B0_DQ0_RK0_TX_ARDQ6_DLY_B0)
			| P_Fld(0x0, SHU_R0_B0_DQ0_RK0_TX_ARDQ5_DLY_B0)
			| P_Fld(0x0, SHU_R0_B0_DQ0_RK0_TX_ARDQ4_DLY_B0)
			| P_Fld(0x0, SHU_R0_B0_DQ0_RK0_TX_ARDQ3_DLY_B0)
			| P_Fld(0x0, SHU_R0_B0_DQ0_RK0_TX_ARDQ2_DLY_B0)
			| P_Fld(0x0, SHU_R0_B0_DQ0_RK0_TX_ARDQ1_DLY_B0)
			| P_Fld(0x0, SHU_R0_B0_DQ0_RK0_TX_ARDQ0_DLY_B0));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B1_DQ0), P_Fld(0x0, SHU_R0_B1_DQ0_RK0_TX_ARDQ7_DLY_B1)
			| P_Fld(0x0, SHU_R0_B1_DQ0_RK0_TX_ARDQ6_DLY_B1)
			| P_Fld(0x0, SHU_R0_B1_DQ0_RK0_TX_ARDQ5_DLY_B1)
			| P_Fld(0x0, SHU_R0_B1_DQ0_RK0_TX_ARDQ4_DLY_B1)
			| P_Fld(0x0, SHU_R0_B1_DQ0_RK0_TX_ARDQ3_DLY_B1)
			| P_Fld(0x0, SHU_R0_B1_DQ0_RK0_TX_ARDQ2_DLY_B1)
			| P_Fld(0x0, SHU_R0_B1_DQ0_RK0_TX_ARDQ1_DLY_B1)
			| P_Fld(0x0, SHU_R0_B1_DQ0_RK0_TX_ARDQ0_DLY_B1));
	#endif
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_TXDLY3), 0x0, SHU_R0_B0_TXDLY3_TX_ARDQM0_DLY_B0);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_TXDLY3), 0x0, SHU_R0_B1_TXDLY3_TX_ARDQM0_DLY_B1);
	}


	//Use HW TX tracking value
	//R_DMARPIDQ_SW :drphy_conf (0x170[7])(default set 1)
	//	 0: DQS2DQ PI setting controlled by HW
	//R_DMARUIDQ_SW : Dramc_conf(0x156[15])(default set 1)
	//	  0: DQS2DQ UI setting controlled by HW
	///TODO: need backup original setting?
	//vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_MISC_CTRL1), 1, MISC_CTRL1_R_DMARPIDQ_SW);
	//vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DQSOSCR), 1, DQSOSCR_ARUIDQ_SW);

}

#define TX_TDQS2DQ_PRE_CAL 0
#if TX_TDQS2DQ_PRE_CAL
//	(1) DDR800 1:4 mode
//	(2) DDR1200/1600 1:4 mode
//	(3) 1:8 mode
// The 3 condition have different MCK2UI/UI2PI. Therefore, TX DQS2DQ should be record separately.
// Here, we record (2) and (3).  DDR800 1:4 skip recording DQS2DQ.
U16 u2DQS2DQ_Pre_Cal[CHANNEL_NUM][RANK_MAX][2/*DIV_Mode*/] = {0};
#endif

static void TXScanRange_PI(DRAMC_CTX_T *p, DRAM_TX_PER_BIT_CALIBRATION_TYTE_T calType, U16 *pu2Begin, U16 *pu2End)
{
	U8 u1MCK2UI, u1UI2PI, u1ByteIdx;
	U32 u4RegValue_TXDLY, u4RegValue_dly;
	U8 ucdq_ui_large_bak[DQS_NUMBER], ucdq_ui_small_bak[DQS_NUMBER];
	U16 u2TempVirtualDelay, u2SmallestVirtualDelay = 0xffff;
	U16 u2DQDelayBegin = 0, u2DQDelayEnd = 0;

#if (__LP5_COMBO__ == TRUE)
	if (TRUE == is_lp5_family(p))
	{
		u4RegValue_TXDLY = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_SHURK_WCK_WR_MCK));
		u4RegValue_dly = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_SHURK_WCK_WR_UI));
	}
	else
#endif
	{
		u4RegValue_TXDLY = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_DQS0));
		u4RegValue_dly = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_DQS1));
	}

	u1MCK2UI = u1MCK2UI_DivShift(p);

	if (vGet_DDR_Loop_Mode(p) == DDR800_CLOSE_LOOP)
		u1UI2PI = 6;
	else
		u1UI2PI = 5;


	// find smallest DQS delay
	for (u1ByteIdx = 0; u1ByteIdx < (p->data_width / DQS_BIT_NUMBER); u1ByteIdx++)
	{
		ucdq_ui_large_bak[u1ByteIdx] = (u4RegValue_TXDLY >> (u1ByteIdx << 2)) & 0x7;// MCK
		ucdq_ui_small_bak[u1ByteIdx] = (u4RegValue_dly >> (u1ByteIdx << 2)) & 0x7;// UI
		//wrlevel_dqs_final_delay[p->rank][u1ByteIdx]  ==> PI

		//LP4 : Virtual Delay = 256 * MCK + 32*UI + PI;
		//LP3 : Virtual Delay = 128 * MCK + 32*UI + PI;
		u2TempVirtualDelay = (((ucdq_ui_large_bak[u1ByteIdx] << u1MCK2UI) + ucdq_ui_small_bak[u1ByteIdx]) << u1UI2PI) + wrlevel_dqs_final_delay[p->rank][u1ByteIdx];

		if (u2TempVirtualDelay < u2SmallestVirtualDelay)
		{
			u2SmallestVirtualDelay = u2TempVirtualDelay;
		}
	}

	u2DQDelayBegin = u2SmallestVirtualDelay;

	#if TX_TDQS2DQ_PRE_CAL
	if (u1IsLP4Div4DDR800(p) == FALSE)
	{
		if (u2DQS2DQ_Pre_Cal[p->channel][p->rank][vGet_Div_Mode(p)] > 0)
		{
			U16 u2TmpShift;
			mcSHOW_DBG_MSG(("TX_TDQS2DQ_PRE_CAL : change DQ begin %d -->", u2DQDelayBegin));

			u2TmpShift = (u2DQS2DQ_Pre_Cal[p->channel][p->rank][vGet_Div_Mode(p)]* p->frequency) / 1000;
			if (u2TmpShift >= 15)
				u2TmpShift -= 15;
			else
				u2TmpShift = 0;

			u2DQDelayBegin += u2TmpShift;
			mcSHOW_DBG_MSG(("%d (+%d)\n", u2DQDelayBegin, u2TmpShift));
		}
	}
	#endif

	#if (__LP5_COMBO__)
	if (is_lp5_family(p)) {
		/* For DDR3200, +1.5 MCK */
		if (p->frequency == 1600)
			u2DQDelayBegin += (((1 << u1MCK2UI) + ((1 << u1MCK2UI) >> 1)) << u1UI2PI);
		else if (p->frequency == 2133)
			u2DQDelayBegin += ((1 << u1MCK2UI) << u1UI2PI);
		else if (p->frequency == 2750)
			u2DQDelayBegin += (9 << u1UI2PI);
	}
	#endif

	#if TX_K_DQM_WITH_WDBI
	if (calType == TX_DQ_DQS_MOVE_DQM_ONLY)
	{
		// DBI on, calibration range -1MCK
		u2DQDelayBegin -= (1 << (u1MCK2UI + 5));
	}
	#endif
	/* Scan range: 1MCK */
	u2DQDelayEnd = u2DQDelayBegin + ((1 << u1MCK2UI) << u1UI2PI);

	*pu2Begin = u2DQDelayBegin;
	*pu2End = u2DQDelayEnd;

	#if 0//TX_TDQS2DQ_PRE_CAL
	mcSHOW_DBG_MSG(("TXScanRange_PI %d~%d\n", u2DQDelayBegin, u2DQDelayEnd));
	#endif
}


static void TXScanRange_Vref(DRAMC_CTX_T *p, U8 u1VrefScanEnable, U16* pu2Range, U16 *pu2Begin, U16 *pu2End, U16 *pu2Setp)
{
	U16 u2VrefBegin = 0, u2VrefEnd = 0;

	if (u1VrefScanEnable)
	{
	#if (SUPPORT_SAVE_TIME_FOR_CALIBRATION && BYPASS_VREF_CAL)
		if (p->femmc_Ready == 1)
		{
			// if fast K, use TX Vref that saved.
			u2VrefBegin = p->pSavetimeData->u1TxWindowPerbitVref_Save[p->channel][p->rank];
			u2VrefEnd = u2VrefBegin + 1;
		}
	#endif
	}
	else //LPDDR3, the for loop will only excute u2VrefLevel=TX_VREF_RANGE_END/2.
	{
		u2VrefBegin = 0;
		u2VrefEnd = 0;
	}

	*pu2Range = (!p->odt_onoff);
	*pu2Begin = u2VrefBegin;
	*pu2End = u2VrefEnd;
	*pu2Setp = TX_VREF_RANGE_STEP;

}

static U16 TxChooseVref(DRAMC_CTX_T *p, PASS_WIN_DATA_BY_VREF_T pVrefInfo[], U8 u1VrefNum)
{
	U8 u1VrefIdx, u1WorseBitIdx = 0, u1WinSizeOfWorseBit = 0;
	U16 u2MaxWinSum = 0;
	U16 u2FinalVref = 0;

	for (u1VrefIdx = 0; u1VrefIdx < u1VrefNum; u1VrefIdx++)
	{
		mcSHOW_DBG_MSG(("TX Vref=%d, minBit %d, minWin=%d, winSum=%d\n",
			pVrefInfo[u1VrefIdx].u2VrefUsed,
			pVrefInfo[u1VrefIdx].u1WorseBitIdx_byVref,
			pVrefInfo[u1VrefIdx].u1WorseBitWinSize_byVref,
			pVrefInfo[u1VrefIdx].u2WinSum_byVref));

		#if LP4_TX_VREF_PASS_CONDITION
		if ((pVrefInfo[u1VrefIdx].u1WorseBitWinSize_byVref > LP4_TX_VREF_PASS_CONDITION))
		{
			if (u1VrefPassBegin == LP4_TX_VREF_BOUNDARY_NOT_READY)
			{
				u1VrefPassBegin = pVrefInfo[u1VrefIdx].u2VrefUsed;
				u1TempPassNum = 1;
			}
			else
				u1TempPassNum ++;

			if (u1VrefIdx == u1VrefNum - 1)
			{
				u1VrefPassEnd = pVrefInfo[u1VrefIdx].u2VrefUsed;
				if (u1TempPassNum > u1MaxVerfPassNum)
				{
					u1VrefPassBegin_Final = u1VrefPassBegin;
					u1VrefPassEnd_Final = u1VrefPassEnd;
					u1MaxVerfPassNum = u1TempPassNum;
				}
			}
		}
		else
		{
			if ((u1VrefPassBegin != LP4_TX_VREF_BOUNDARY_NOT_READY) && (u1VrefPassEnd == LP4_TX_VREF_BOUNDARY_NOT_READY))
			{
				u1VrefPassEnd = pVrefInfo[u1VrefIdx].u2VrefUsed - TX_VREF_RANGE_STEP;
				if (u1TempPassNum > u1MaxVerfPassNum)
				{
					u1VrefPassBegin_Final = u1VrefPassBegin;
					u1VrefPassEnd_Final = u1VrefPassEnd;
					u1MaxVerfPassNum = u1TempPassNum;
				}
				u1VrefPassBegin = 0xff;
				u1VrefPassEnd = 0xff;
				u1TempPassNum = 0;
			}
		}
		#endif
	}

	#if LP4_TX_VREF_PASS_CONDITION
	//if((u1VrefPassBegin_Final !=LP4_TX_VREF_BOUNDARY_NOT_READY) && (u1VrefPassEnd_Final!=LP4_TX_VREF_BOUNDARY_NOT_READY))
	if (u1MaxVerfPassNum > 0)
	{
		// vref pass window found
		u2FinalVref = (u1VrefPassBegin_Final + u1VrefPassEnd_Final) >> 1;
		mcSHOW_DBG_MSG(("[TxChooseVref] Window > %d, Vref (%d~%d), Final Vref %d\n", LP4_TX_VREF_PASS_CONDITION, u1VrefPassBegin_Final, u1VrefPassEnd_Final, u2FinalVref));
	}
	else
	#endif
	{
		// not vref found
		for (u1VrefIdx = 0; u1VrefIdx < u1VrefNum; u1VrefIdx++)
		{
			if ((pVrefInfo[u1VrefIdx].u1WorseBitWinSize_byVref > u1WinSizeOfWorseBit) ||
				((pVrefInfo[u1VrefIdx].u1WorseBitWinSize_byVref == u1WinSizeOfWorseBit) && (pVrefInfo[u1VrefIdx].u2WinSum_byVref > u2MaxWinSum)))
			{
				u1WinSizeOfWorseBit = pVrefInfo[u1VrefIdx].u1WorseBitWinSize_byVref;
				u1WorseBitIdx = pVrefInfo[u1VrefIdx].u1WorseBitIdx_byVref;
				u2MaxWinSum = pVrefInfo[u1VrefIdx].u2WinSum_byVref;
				u2FinalVref = pVrefInfo[u1VrefIdx].u2VrefUsed;
			}
		}

		mcSHOW_DBG_MSG(("[TxChooseVref] Worse bit %d, Min win %d, Win sum %d, Final Vref %d\n", u1WorseBitIdx, u1WinSizeOfWorseBit, u2MaxWinSum, u2FinalVref));
	}

	return u2FinalVref;
}


static void DramcTXSetVref(DRAMC_CTX_T *p, U8 u1VrefRange, U8 u1VrefValue)
{
	U8 u1TempOPValue;

#ifdef __LP5_COMBO__
	if (is_lp5_family(p))
		u1TempOPValue = ((u1VrefValue & 0x7f));
	else
#endif
		u1TempOPValue = ((u1VrefValue & 0x3f) | (u1VrefRange << 6));

	u1MR14Value[p->channel][p->rank][p->dram_fsp] = u1TempOPValue;
	//For TX VREF of different byte

	DramcModeRegWriteByRank(p, p->rank, 14, u1TempOPValue);
#ifdef __LP5_COMBO__
	if (is_lp5_family(p))
		DramcModeRegWriteByRank(p, p->rank, 15, u1TempOPValue);
#endif

	#if CALIBRATION_SPEED_UP_DEBUG
	mcSHOW_DBG_MSG(("Yulia TX Vref : CH%d Rank%d, TX Range %d Vref %d\n\n", p->channel, p->rank, u1VrefRange, (u1VrefValue & 0x3f)));
	#endif
}


static void TXSetFinalVref(DRAMC_CTX_T *p, U16 u2FinalRange, U16 u2FinalVref)
{
	DramcTXSetVref(p, u2FinalRange, u2FinalVref);

#ifdef FOR_HQA_TEST_USED
	gFinalTXVrefDQ[p->channel][p->rank] = (U8) u2FinalVref;
#endif

#if VENDER_JV_LOG
	mcSHOW_DBG_MSG5(("\nFinal TX Range %d Vref %d\n\n", u2FinalRange, u2FinalVref));
#else
	mcSHOW_DBG_MSG(("\nFinal TX Range %d Vref %d\n\n", u2FinalRange, u2FinalVref));
#endif

	#if CALIBRATION_SPEED_UP_DEBUG
	mcSHOW_DBG_MSG(("Yulia TX Vref Final: CH%d Rank%d, TX Range %d Vref %d\n\n", p->channel, p->rank, u2FinalRange, u2FinalVref));
	#endif
}


#if ENABLE_TX_TRACKING
#if !BYPASS_CALIBRATION
static
#endif
void TXUpdateTXTracking(DRAMC_CTX_T *p, DRAM_TX_PER_BIT_CALIBRATION_TYTE_T calType, U8 ucdq_pi[], U8 ucdqm_pi[])
{
	 if (calType == TX_DQ_DQS_MOVE_DQ_ONLY || calType == TX_DQ_DQS_MOVE_DQM_ONLY)
	 {
		 //make a copy to dramc reg for TX DQ tracking used
		 if (calType == TX_DQ_DQS_MOVE_DQ_ONLY)
		 {
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHURK_PI),
							P_Fld(ucdq_pi[0], SHURK_PI_RK0_ARPI_DQ_B0) | P_Fld(ucdq_pi[1], SHURK_PI_RK0_ARPI_DQ_B1));

			// Source DQ
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHURK_DQS2DQ_CAL1),
													P_Fld(ucdq_pi[1], SHURK_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ1) |
													P_Fld(ucdq_pi[0], SHURK_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ0));
			// Target DQ
			 vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHURK_DQS2DQ_CAL2),
													 P_Fld(ucdq_pi[1], SHURK_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ1) |
													 P_Fld(ucdq_pi[0], SHURK_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ0));
		 }

		 //if(calType ==TX_DQ_DQS_MOVE_DQM_ONLY || (calType ==TX_DQ_DQS_MOVE_DQ_ONLY))
		 {
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHURK_PI),
							P_Fld(ucdqm_pi[0], SHURK_PI_RK0_ARPI_DQM_B0) | P_Fld(ucdqm_pi[1], SHURK_PI_RK0_ARPI_DQM_B1));

			// Target DQM
			 vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHURK_DQS2DQ_CAL5),
												 P_Fld(ucdqm_pi[1], SHURK_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM1) |
												 P_Fld(ucdqm_pi[0], SHURK_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM0));
		 }
	 }


#if 0// for LP3 , TX tracking will be disable, don't need to set DQ delay in DramC.
	 ///TODO: check LP3 byte mapping of dramC
	 vIO32WriteFldMulti(DRAMC_REG_SHURK0_PI + (CHANNEL_A << POS_BANK_NUM), \
							  P_Fld(ucdq_final_pi[0], SHURK0_PI_RK0_ARPI_DQ_B0) | P_Fld(ucdq_final_pi[1], SHURK0_PI_RK0_ARPI_DQ_B1));

	 vIO32WriteFldMulti(DRAMC_REG_SHURK0_PI + SHIFT_TO_CHB_ADDR, \
							  P_Fld(ucdq_final_pi[2], SHURK0_PI_RK0_ARPI_DQ_B0) | P_Fld(ucdq_final_pi[3], SHURK0_PI_RK0_ARPI_DQ_B1));
#endif

}
#endif //End ENABLE_TX_TRACKING

#if !BYPASS_CALIBRATION
static
#endif
void TXSetDelayReg_DQ(DRAMC_CTX_T *p, U8 u1UpdateRegUI, U8 ucdq_ui_large[], U8 ucdq_oen_ui_large[], U8 ucdq_ui_small[], U8 ucdq_oen_ui_small[], U8 ucdql_pi[])
{
	if (u1UpdateRegUI)
	{
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ0), \
									 P_Fld(ucdq_ui_large[0], SHURK_SELPH_DQ0_TXDLY_DQ0) |
									 P_Fld(ucdq_ui_large[1], SHURK_SELPH_DQ0_TXDLY_DQ1) |
									 P_Fld(ucdq_ui_large[2], SHURK_SELPH_DQ0_TXDLY_DQ2) |
									 P_Fld(ucdq_ui_large[3], SHURK_SELPH_DQ0_TXDLY_DQ3) |
									 P_Fld(ucdq_oen_ui_large[0], SHURK_SELPH_DQ0_TXDLY_OEN_DQ0) |
									 P_Fld(ucdq_oen_ui_large[1], SHURK_SELPH_DQ0_TXDLY_OEN_DQ1) |
									 P_Fld(ucdq_oen_ui_large[2], SHURK_SELPH_DQ0_TXDLY_OEN_DQ2) |
									 P_Fld(ucdq_oen_ui_large[3], SHURK_SELPH_DQ0_TXDLY_OEN_DQ3));

		// DLY_DQ[2:0]
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ2), \
									 P_Fld(ucdq_ui_small[0], SHURK_SELPH_DQ2_DLY_DQ0) |
									 P_Fld(ucdq_ui_small[1], SHURK_SELPH_DQ2_DLY_DQ1) |
									 P_Fld(ucdq_ui_small[2], SHURK_SELPH_DQ2_DLY_DQ2) |
									 P_Fld(ucdq_ui_small[3], SHURK_SELPH_DQ2_DLY_DQ3) |
									 P_Fld(ucdq_oen_ui_small[0], SHURK_SELPH_DQ2_DLY_OEN_DQ0) |
									 P_Fld(ucdq_oen_ui_small[1], SHURK_SELPH_DQ2_DLY_OEN_DQ1) |
									 P_Fld(ucdq_oen_ui_small[2], SHURK_SELPH_DQ2_DLY_OEN_DQ2) |
									 P_Fld(ucdq_oen_ui_small[3], SHURK_SELPH_DQ2_DLY_OEN_DQ3));
	}


	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_DQ0), ucdql_pi[0], SHU_R0_B0_DQ0_SW_ARPI_DQ_B0);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_DQ0), ucdql_pi[1], SHU_R0_B1_DQ0_SW_ARPI_DQ_B1);
}

#if !BYPASS_CALIBRATION
static
#endif
void TXSetDelayReg_DQM(DRAMC_CTX_T *p, U8 u1UpdateRegUI, U8 ucdqm_ui_large[], U8 ucdqm_oen_ui_large[], U8 ucdqm_ui_small[], U8 ucdqm_oen_ui_small[], U8 ucdqm_pi[])
{
	if (u1UpdateRegUI)
	{
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ1),
									 P_Fld(ucdqm_ui_large[0], SHURK_SELPH_DQ1_TXDLY_DQM0) |
									 P_Fld(ucdqm_ui_large[1], SHURK_SELPH_DQ1_TXDLY_DQM1) |
									 P_Fld(ucdqm_ui_large[2], SHURK_SELPH_DQ1_TXDLY_DQM2) |
									 P_Fld(ucdqm_ui_large[3], SHURK_SELPH_DQ1_TXDLY_DQM3) |
									 P_Fld(ucdqm_oen_ui_large[0], SHURK_SELPH_DQ1_TXDLY_OEN_DQM0) |
									 P_Fld(ucdqm_oen_ui_large[1], SHURK_SELPH_DQ1_TXDLY_OEN_DQM1) |
									 P_Fld(ucdqm_oen_ui_large[2], SHURK_SELPH_DQ1_TXDLY_OEN_DQM2) |
									 P_Fld(ucdqm_oen_ui_large[3], SHURK_SELPH_DQ1_TXDLY_OEN_DQM3));

		 // DLY_DQM[2:0]
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ3),
									 P_Fld(ucdqm_ui_small[0], SHURK_SELPH_DQ3_DLY_DQM0) |
									 P_Fld(ucdqm_ui_small[1], SHURK_SELPH_DQ3_DLY_DQM1) |
									 P_Fld(ucdqm_ui_small[2], SHURK_SELPH_DQ3_DLY_DQM2) |
									 P_Fld(ucdqm_ui_small[3], SHURK_SELPH_DQ3_DLY_DQM3) |
									 P_Fld(ucdqm_oen_ui_small[0], SHURK_SELPH_DQ3_DLY_OEN_DQM0) |
									 P_Fld(ucdqm_oen_ui_small[1], SHURK_SELPH_DQ3_DLY_OEN_DQM1) |
									 P_Fld(ucdqm_oen_ui_small[2], SHURK_SELPH_DQ3_DLY_OEN_DQM2) |
									 P_Fld(ucdqm_oen_ui_small[3], SHURK_SELPH_DQ3_DLY_OEN_DQM3));
	}

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_DQ0), ucdqm_pi[0], SHU_R0_B0_DQ0_SW_ARPI_DQM_B0);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_DQ0), ucdqm_pi[1], SHU_R0_B1_DQ0_SW_ARPI_DQM_B1);
}

#if TX_AUTO_K_ENABLE
static void Tx_Auto_K_Init(DRAMC_CTX_T *p, DRAM_TX_PER_BIT_CALIBRATION_TYTE_T calType, U8 ucdq_pi, U8 u1PI_Len)
{
	u8 pi_thrd = 0xa;

#if FOR_DV_SIMULATION_USED == 1
	cal_sv_rand_args_t *psra = get_psra();

	if (psra) {
		pi_thrd = psra->tx_atk_pass_pi_thrd & 0xFF;
		early_break = psra->tx_atk_early_break & 0xFF;
	}
#endif

#if ENABLE_PA_IMPRO_FOR_TX_AUTOK
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DCM_SUB_CTRL), 0x1, DCM_SUB_CTRL_SUBCLK_CTRL_TX_AUTOK);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TX_CG_SET0), 0x1, TX_CG_SET0_TX_ATK_CLKRUN);
#endif

	if (calType == TX_DQ_DQS_MOVE_DQ_DQM)
	{
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TX_ATK_SET1),
									 P_Fld(0x1, TX_ATK_SET1_TX_ATK_DQ_PI_EN) |	  //enable TX DQ auto K
									 P_Fld(0x1, TX_ATK_SET1_TX_ATK_DQM_PI_EN));   //enable TX DQM auto K
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TX_ATK_SET0),
									 P_Fld(ucdq_pi, TX_ATK_SET0_TX_ATK_DQ_B0_PI_INIT) |    //Set begin position of DQ B0
									 P_Fld(ucdq_pi, TX_ATK_SET0_TX_ATK_DQ_B1_PI_INIT) |    //Set begin position of DQ B1
									 P_Fld(ucdq_pi, TX_ATK_SET0_TX_ATK_DQM_B0_PI_INIT) |   //Set begin position of DQM B0
									 P_Fld(ucdq_pi, TX_ATK_SET0_TX_ATK_DQM_B1_PI_INIT));   //Set begin position of DQM B1
	}
	else if (calType == TX_DQ_DQS_MOVE_DQM_ONLY)
	{
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TX_ATK_SET1), 0x1, TX_ATK_SET1_TX_ATK_DQM_PI_EN);
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TX_ATK_SET0),
									 P_Fld(ucdq_pi, TX_ATK_SET0_TX_ATK_DQM_B0_PI_INIT) |
									 P_Fld(ucdq_pi, TX_ATK_SET0_TX_ATK_DQM_B1_PI_INIT));
	}
	else
	{
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TX_ATK_SET1), 0x1, TX_ATK_SET1_TX_ATK_DQ_PI_EN);
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TX_ATK_SET0),
									 P_Fld(ucdq_pi, TX_ATK_SET0_TX_ATK_DQ_B0_PI_INIT) |
									 P_Fld(ucdq_pi, TX_ATK_SET0_TX_ATK_DQ_B1_PI_INIT));
	}

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CTRL1), 0, MISC_CTRL1_R_DMARPIDQ_SW); //Switch PI SW mode to HW mode (control by DRAMC not APHY)
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TX_ATK_SET1),
			P_Fld(u1PI_Len, TX_ATK_SET1_TX_ATK_PI_LEN) |	//enable TX auto k len
			P_Fld(pi_thrd, TX_ATK_SET1_TX_ATK_PASS_PI_THRD));  //Set threshold of PI pass window
#if (fcFOR_CHIP_ID == fcIPM) //Fix at Mar_gaux
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TX_ATK_SET1), early_break, TX_ATK_SET1_TX_ATK_EARLY_BREAK); //Enable early break
#endif

#if (__LP5_COMBO__ == TRUE)
	if (TRUE == is_lp5_family(p))
	{
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_TX_SET0),
				P_Fld(0x5, SHU_TX_SET0_TXOEN_AUTOSET_OFFSET) |
				P_Fld(0x1, SHU_TX_SET0_TXOEN_AUTOSET_EN));	 //Enable OE auto adjust
	}
	else
#endif
	{
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_TX_SET0),
				P_Fld(0x3, SHU_TX_SET0_TXOEN_AUTOSET_OFFSET) |
				P_Fld(0x1, SHU_TX_SET0_TXOEN_AUTOSET_EN));	 //Enable OE auto adjust
	}

#if TX_AUTO_K_DEBUG_ENABLE
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TX_ATK_SET1), 0x1, TX_ATK_SET1_TX_ATK_DBG_EN);
#endif
}

static void Tx_Auto_K_complete_check(DRAMC_CTX_T *p)
{
	U32 u4loop_count = 0;

	while ((u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TX_ATK_RESULT8), TX_ATK_RESULT8_TX_ATK_DONE) != 0x1))
	{
		mcDELAY_US(1);
		u4loop_count++;
		//mcSHOW_DBG_MSG(("Wait! TX auto K is not done!\n"));
		if (u4loop_count > 100000)
		{
			mcSHOW_ERR_MSG(("Error! TX auto K is not done!\n"));
			break;
		}
	}

	if ((u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TX_ATK_RESULT8), TX_ATK_RESULT8_TX_ATK_FIND_PW) == 0x1))
	{
		vSetCalibrationResult(p, DRAM_CALIBRATION_TX_PERBIT, DRAM_OK);
		mcSHOW_DBG_MSG(("Tx auto K, all bit find passs window\n"));
	}
	else
	{
		mcSHOW_ERR_MSG(("Error! TX auto K is fail!\n"));
	}
}

static void Tx_Auto_K_Clear(DRAMC_CTX_T *p)
{
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TX_ATK_SET1), 0x0, TX_ATK_SET1_TX_ATK_TRIG); //Disable Tx auto K
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_TX_SET0), 0x0, SHU_TX_SET0_TXOEN_AUTOSET_EN);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CTRL1), 0x1, MISC_CTRL1_R_DMARPIDQ_SW);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TX_ATK_SET1), 0x0, TX_ATK_SET1_TX_ATK_DBG_EN);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TX_ATK_SET1), 0x1, TX_ATK_SET1_TX_ATK_CLR); //Clear state machine
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TX_ATK_SET1), 0x0, TX_ATK_SET1_TX_ATK_CLR);
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TX_ATK_SET1),
								 P_Fld(0x0, TX_ATK_SET1_TX_ATK_PI_LEN) |
								 P_Fld(0x0, TX_ATK_SET1_TX_ATK_DQ_PI_EN) |
								 P_Fld(0x0, TX_ATK_SET1_TX_ATK_DQM_PI_EN));
#if ENABLE_PA_IMPRO_FOR_TX_AUTOK
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TX_CG_SET0), 0x0, TX_CG_SET0_TX_ATK_CLKRUN);
#endif
}

#if TX_AUTO_K_WORKAROUND
static void Tx_Auto_K_DQM_Workaround(DRAMC_CTX_T *p)
{
	//Set RK1 DQM DLY to RK0
	vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ1), u4DQM_MCK_RK1_backup);
	vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ3), u4DQM_UI_RK1_backup);
	vIO32Write4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_DQ0), u4DQM_PI_RK1_backup[0]);
	vIO32Write4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_DQ0), u4DQM_PI_RK1_backup[1]);
}
static void Tx_Auto_K_DQ_Workaround(DRAMC_CTX_T *p)
{
	//Set RK1 DQM DLY to RK0
	vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ0), u4DQ_MCK_RK1_backup);
	vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ2), u4DQ_UI_RK1_backup);
	vIO32Write4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_DQ0), u4DQ_PI_RK1_backup[0]);
	vIO32Write4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_DQ0), u4DQ_PI_RK1_backup[1]);
}
#endif

#if TX_AUTO_K_DEBUG_ENABLE
static void Tx_Auto_K_Debug_Message(DRAMC_CTX_T *p, U8 u1PI_Len)
{
	U8 u1bit_num = 0, u1BitIdx;
	U16 u2Length = 0, u2Length_max = 0;
	U32 u4status;
	U32 u4status_bit[4][DQ_DATA_WIDTH_LP4];

	if (u1PI_Len == 0)
		u2Length_max = 48;
	else
		u2Length_max = 32 * (1 + u1PI_Len);

	for (u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
	{
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TX_ATK_SET1), u1BitIdx, TX_ATK_SET1_TX_ATK_DBG_BIT_SEL);

		u4status_bit[0][u1BitIdx] = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_TX_ATK_DBG_BIT_STATUS1));
		u4status_bit[1][u1BitIdx] = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_TX_ATK_DBG_BIT_STATUS2));
		u4status_bit[2][u1BitIdx] = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_TX_ATK_DBG_BIT_STATUS3));
		u4status_bit[3][u1BitIdx] = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_TX_ATK_DBG_BIT_STATUS4));
	}

	mcSHOW_DBG_MSG2(("Debug TX DQ PASS/FAIL status:\n"));

	for (u2Length = 0; u2Length < u2Length_max; u2Length++)
	{
		mcSHOW_DBG_MSG2(("Delay=%3d ", u2Length));

		for (u1bit_num = 0; u1bit_num < p->data_width; u1bit_num++)
		{
			u4status = ((u4status_bit[u2Length / 32][u1bit_num] >> (u2Length % 32)) & 0x1);

			if (u4status == 0)
			{
				mcSHOW_DBG_MSG2(("x"));
			}
			else
			{
				mcSHOW_DBG_MSG2(("o"));
			}

			if (u1bit_num == (p->data_width - 1))
			{
				mcSHOW_DBG_MSG2((" \n"));
			}
		}
	}

	//mcSHOW_DBG_MSG(("Debug DQ PASS(1)/FAIL(0) bit: %d, STATUS1: 0x%x, STATUS2: 0x%x, STATUS3: 0x%x, STATUS4: 0x%x,\n",u1BitIdx,u4status_bit[0][u1BitIdx],u4status_bit[1][u1BitIdx],u4status_bit[2][u1BitIdx],u4status_bit[3][u1BitIdx]));
}
#endif
#endif

#if TX_K_DQM_WITH_WDBI
void vSwitchWriteDBISettings(DRAMC_CTX_T *p, U8 u1OnOff)
{
	S8 u1TXShiftMCK;

	u1TXShiftMCK = (u1OnOff)? -1: 1;
	DramcWriteShiftMCKForWriteDBI(p, u1TXShiftMCK); //Tx DQ/DQM -1 MCK for write DBI ON

	SetDramModeRegForWriteDBIOnOff(p, p->dram_fsp, u1OnOff);
	DramcWriteDBIOnOff(p, u1OnOff);

	#if (TX_AUTO_K_ENABLE && TX_AUTO_K_WORKAROUND)
	if (p->rank == RANK_1)
	{
		u4DQ_MCK_RK1_backup = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ0));
		u4DQ_UI_RK1_backup = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ2));
		u4DQ_PI_RK1_backup[0] = u4IO32Read4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_DQ0));
		u4DQ_PI_RK1_backup[1] = u4IO32Read4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_DQ0));
	}
	#endif
}
#endif

PASS_WIN_DATA_T WinPerBit[DQ_DATA_WIDTH], VrefWinPerBit[DQ_DATA_WIDTH], FinalWinPerBit[DQ_DATA_WIDTH];
DRAM_STATUS_T DramcTxWindowPerbitCal(DRAMC_CTX_T *p, DRAM_TX_PER_BIT_CALIBRATION_TYTE_T calType, U8 u1VrefScanEnable, u8 isAutoK)
{
	U8 u1BitTemp, u1BitIdx, u1ByteIdx, u1RankIdx, backup_rank;
	U32 uiFinishCount;

	U16 uiDelay, u2DQDelayBegin, u2DQDelayEnd, u2DQDelayStep = 1;

	U8 ucdq_pi, ucdq_ui_small, ucdq_ui_large, ucdq_oen_ui_small, ucdq_oen_ui_large;
	U8 ucdq_ui_small_reg_value, u1UpdateRegUI;	// for UI and TXDLY change check, if different , set reg.

	U8 ucdq_reg_pi[DQS_NUMBER], ucdq_reg_ui_large[DQS_NUMBER], ucdq_reg_ui_small[DQS_NUMBER];
	U8 ucdq_reg_oen_ui_large[DQS_NUMBER], ucdq_reg_oen_ui_small[DQS_NUMBER];

	U8 ucdq_reg_dqm_pi[DQS_NUMBER] = {0}, ucdq_reg_dqm_ui_large[DQS_NUMBER] = {0}, ucdq_reg_dqm_ui_small[DQS_NUMBER] = {0};
	U8 ucdq_reg_dqm_oen_ui_large[DQS_NUMBER] = {0}, ucdq_reg_dqm_oen_ui_small[DQS_NUMBER] = {0};

	#if 1//TX_DQM_CALC_MAX_MIN_CENTER
	U16 u2DQM_Delay;  // LP4 only
	U16 u2Center_min[DQS_NUMBER] = {0}, u2Center_max[DQS_NUMBER] = {0};
	#endif
	U8 u1EnableDelayCell = 0;
	U16 u2DelayCellOfst[DQ_DATA_WIDTH] = {0};
	U32 u4err_value, u4fail_bit;
	U16 u2FinalRange = 0, u2FinalVref;
	U16 u2VrefLevel, u2VrefBegin = 0, u2VrefEnd = 0, u2VrefStep;
	U16 u2TempWinSum, u2MaxWindowSum = 0;//, u2tx_window_sum[LP4_TX_VREF_DATA_NUM]={0};
	U8 u1min_bit, u1min_winsize = 0;
	U8 u1VrefIdx = 0;
	U8 u1PIDiff;
	PASS_WIN_DATA_BY_VREF_T VrefInfo[LP4_TX_VREF_DATA_NUM];

	if (!p)
	{
		mcSHOW_ERR_MSG(("context NULL\n"));
		return DRAM_FAIL;
	}

	#if TX_AUTO_K_ENABLE
	U8 u1PI_Len, u1dq_shift;
	U32 PwMaxInitReg[4] = {DRAMC_REG_TX_ATK_RESULT0, DRAMC_REG_TX_ATK_RESULT1, DRAMC_REG_TX_ATK_RESULT2, DRAMC_REG_TX_ATK_RESULT3};
	U32 PwMaxLenReg[4] = {DRAMC_REG_TX_ATK_RESULT4, DRAMC_REG_TX_ATK_RESULT5, DRAMC_REG_TX_ATK_RESULT6, DRAMC_REG_TX_ATK_RESULT7};
	U32 u4Length = 0;
	#if TX_AUTO_K_WORKAROUND
	U8 u1backup_Rank = 0;
	#endif
	#if TX_AUTO_K_WORKAROUND
	U32 u4RegBackupAddress[] =
	{
		(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ0)),
		(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ2)),
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_DQ0)),
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_DQ0)),
		(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ1)),
		(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ3)),
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_DQ0)),
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_DQ0)),
	};
	#endif
	#endif

#if VENDER_JV_LOG
	if (calType == TX_DQ_DQS_MOVE_DQ_ONLY)
		vPrintCalibrationBasicInfo_ForJV(p);
#else
		vPrintCalibrationBasicInfo(p);
#endif

	backup_rank = u1GetRank(p);

	TXPerbitCalibrationInit(p, calType);
	TXScanRange_PI(p, calType, &u2DQDelayBegin, &u2DQDelayEnd);
	TXScanRange_Vref(p, u1VrefScanEnable, &u2FinalRange, &u2VrefBegin, &u2VrefEnd, &u2VrefStep);

	//default set FAIL
	vSetCalibrationResult(p, DRAM_CALIBRATION_TX_PERBIT, DRAM_FAIL);

	if (isAutoK)
	{
	#if TX_AUTO_K_ENABLE
	//CKEFixOnOff(p, p->rank, CKE_FIXON, CKE_WRITE_TO_ONE_CHANNEL); //Let CLK always on

	//Set base address of TX MCK and UI
	u1UpdateRegUI = 1;
	uiDelay = u2DQDelayBegin;
	u1PI_Len = 3;
	TxWinTransferDelayToUIPI(p, uiDelay, 0, &ucdq_ui_large, &ucdq_ui_small, &ucdq_pi, &ucdq_oen_ui_large, &ucdq_oen_ui_small);

	for (u1ByteIdx = 0; u1ByteIdx < DQS_NUMBER; u1ByteIdx++)
	{
		if (u1UpdateRegUI)
		{
			ucdq_reg_ui_large[u1ByteIdx] = ucdq_ui_large;
			ucdq_reg_ui_small[u1ByteIdx] = ucdq_ui_small;
			ucdq_reg_oen_ui_large[u1ByteIdx] = ucdq_oen_ui_large;
			ucdq_reg_oen_ui_small[u1ByteIdx] = ucdq_oen_ui_small;

			ucdq_reg_dqm_ui_large[u1ByteIdx] = ucdq_ui_large;
			ucdq_reg_dqm_ui_small[u1ByteIdx] = ucdq_ui_small;
			ucdq_reg_dqm_oen_ui_large[u1ByteIdx] = ucdq_oen_ui_large;
			ucdq_reg_dqm_oen_ui_small[u1ByteIdx] = ucdq_oen_ui_small;
		}

			ucdq_reg_pi[u1ByteIdx] = ucdq_pi;
			ucdq_reg_dqm_pi[u1ByteIdx] = ucdq_pi;
	}

	#if TX_AUTO_K_WORKAROUND
	if (p->rank == 1)
	{
		u1backup_Rank = 1;
		p->rank = 0;
		DramcBackupRegisters(p, u4RegBackupAddress, sizeof(u4RegBackupAddress) / sizeof(U32));
	}
	#endif

	if (calType == TX_DQ_DQS_MOVE_DQ_ONLY || calType == TX_DQ_DQS_MOVE_DQ_DQM)
	{
		TXSetDelayReg_DQ(p, u1UpdateRegUI, ucdq_reg_ui_large, ucdq_reg_oen_ui_large, ucdq_reg_ui_small, ucdq_reg_oen_ui_small, ucdq_reg_pi);
		mcSHOW_DBG_MSG(("TX Auto-K set begin delay DQ MCK: %d, UI: %d, PI: %d\n", ucdq_reg_ui_large[0], ucdq_reg_ui_small[0], ucdq_reg_pi[0]));

		#if TX_AUTO_K_WORKAROUND
		if ((calType == TX_DQ_DQS_MOVE_DQ_ONLY) && (u1backup_Rank == 1))
			Tx_Auto_K_DQM_Workaround(p); //Set best DLY value of RK1 DQM to RK0 DQM
		#endif
	}
	if (calType == TX_DQ_DQS_MOVE_DQM_ONLY || calType == TX_DQ_DQS_MOVE_DQ_DQM)
	{
		TXSetDelayReg_DQM(p, u1UpdateRegUI, ucdq_reg_dqm_ui_large, ucdq_reg_dqm_oen_ui_large, ucdq_reg_dqm_ui_small, ucdq_reg_dqm_oen_ui_small, ucdq_reg_dqm_pi);
		mcSHOW_DBG_MSG(("TX Auto-K set begin delay DQM MCK: %d, UI: %d, PI: %d\n", ucdq_reg_dqm_ui_large[0], ucdq_reg_dqm_ui_small[0], ucdq_reg_dqm_pi[0]));

		#if TX_AUTO_K_WORKAROUND
		if ((calType == TX_DQ_DQS_MOVE_DQM_ONLY) && (u1backup_Rank == 1))
			Tx_Auto_K_DQ_Workaround(p); //Set best DLY value of RK1 DQ to RK0 DQ
		#endif
	}

	#if TX_AUTO_K_WORKAROUND
	if (u1backup_Rank == 1)
		p->rank = 1;
	#endif

	//Tx_Auto_K_Init(p, calType, ucdq_pi, u1PI_Len); //u1PI_Len = 1 means that PI len is 64 PI
	#endif
	}
	else
	{
	if (vGet_DDR_Loop_Mode(p) == SEMI_OPEN_LOOP_MODE)
		u2DQDelayStep = (1 << 3);
	else if (vGet_DDR_Loop_Mode(p) == OPEN_LOOP_MODE)
		u2DQDelayStep = (1 << 4);
	else if (calType == TX_DQ_DQS_MOVE_DQ_DQM)
		u2DQDelayStep = 2;
	else
		u2DQDelayStep = 1;
	if (is_lp5_family(p))
		u2DQDelayStep = 4; /* To speed up simulation */
	#if (FOR_DV_SIMULATION_USED == 1)
		u2DQDelayStep = (vGet_DDR_Loop_Mode(p) == OPEN_LOOP_MODE) ? 16 : 8;
	#endif
	}

#if 0
	mcSHOW_DBG_MSG(("[TxWindowPerbitCal] calType=%d, VrefScanEnable %d (Range %d,  VrefBegin %d, u2VrefEnd %d)\n"
					"\nBegin, DQ Scan Range %d~%d\n",
					calType, u1VrefScanEnable, u2FinalRange, u2VrefBegin, u2VrefEnd, u2DQDelayBegin, u2DQDelayEnd));
#endif

	#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
	if (p->femmc_Ready == 1 && (p->Bypass_TXWINDOW))
	{
		for (u1ByteIdx = 0; u1ByteIdx < (p->data_width / DQS_BIT_NUMBER); u1ByteIdx++)
		{
			u2Center_min[u1ByteIdx] = p->pSavetimeData->u1TxCenter_min_Save[p->channel][p->rank][u1ByteIdx];
			u2Center_max[u1ByteIdx] = p->pSavetimeData->u1TxCenter_max_Save[p->channel][p->rank][u1ByteIdx];

			for (u1BitIdx = 0; u1BitIdx < DQS_BIT_NUMBER; u1BitIdx++)
			{
				u1BitTemp = u1ByteIdx * DQS_BIT_NUMBER + u1BitIdx;
				FinalWinPerBit[u1BitTemp].win_center = p->pSavetimeData->u1Txwin_center_Save[p->channel][p->rank][u1BitTemp];
			}
		}
		vSetCalibrationResult(p, DRAM_CALIBRATION_TX_PERBIT, DRAM_FAST_K);
	}
	else
	#endif
	{
#if ENABLE_K_WITH_WORST_SI_UI_SHIFT
		DramcEngine2Init(p, p->test2_1, p->test2_2, p->test_pattern | 0x80, 0, TE_UI_SHIFT);//UI_SHIFT + LEN1
#else
		DramcEngine2Init(p, p->test2_1, p->test2_2, TEST_XTALK_PATTERN, 0, TE_NO_UI_SHIFT);
#endif

		for (u2VrefLevel = u2VrefBegin; u2VrefLevel <= u2VrefEnd; u2VrefLevel += u2VrefStep)
		{
			// SET tx Vref (DQ) here, LP3 no need to set this.
			if (u1VrefScanEnable)
			{
				#if (!REDUCE_LOG_FOR_PRELOADER)
				mcSHOW_DBG_MSG(("\n\n\tLP4 TX VrefRange %d, VrefLevel=%d\n", u2FinalRange, u2VrefLevel));
				#endif

				#if VENDER_JV_LOG
				if (calType == TX_DQ_DQS_MOVE_DQ_ONLY)
				{
					mcSHOW_DBG_MSG5(("\n\tLP4 TX VrefRange %d, VrefLevel=%d\n", u2FinalRange, u2VrefLevel));
				}
				#endif

				DramcTXSetVref(p, u2FinalRange, u2VrefLevel);
			}
			else
			{
				mcSHOW_DBG_MSG(("\n\n\tTX Vref Scan disable\n"));
			}

			// initialize parameters
			uiFinishCount = 0;
			u2TempWinSum = 0;
			ucdq_ui_small_reg_value = 0xff;

			for (u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
			{
				WinPerBit[u1BitIdx].first_pass = (S16)PASS_RANGE_NA;
				WinPerBit[u1BitIdx].last_pass = (S16)PASS_RANGE_NA;
				VrefWinPerBit[u1BitIdx].first_pass = (S16)PASS_RANGE_NA;
				VrefWinPerBit[u1BitIdx].last_pass = (S16)PASS_RANGE_NA;
			}

			if (isAutoK)
			{
			#if TX_AUTO_K_ENABLE
			Tx_Auto_K_Init(p, calType, ucdq_pi, u1PI_Len); //u1PI_Len = 1 means that PI len is 64 PI
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TX_ATK_SET1), 0x1, TX_ATK_SET1_TX_ATK_TRIG); //TX Auto K start
			#endif
			}
			else
			{
			//Move DQ delay ,  1 PI = tCK/64, total 128 PI, 1UI = 32 PI
			//For data rate 3200, max tDQS2DQ is 2.56UI (82 PI)
			//For data rate 4266, max tDQS2DQ is 3.41UI (109 PI)
			for (uiDelay = u2DQDelayBegin; uiDelay < u2DQDelayEnd; uiDelay += u2DQDelayStep)
			{
				TxWinTransferDelayToUIPI(p, uiDelay, 0, &ucdq_ui_large, &ucdq_ui_small, &ucdq_pi, &ucdq_oen_ui_large, &ucdq_oen_ui_small);

				// Check if TX UI changed, if not change , don't need to set reg again
				if (ucdq_ui_small_reg_value != ucdq_ui_small)
				{
					u1UpdateRegUI = 1;
					ucdq_ui_small_reg_value = ucdq_ui_small;
				}
				else
					u1UpdateRegUI = 0;

				for (u1ByteIdx = 0; u1ByteIdx < DQS_NUMBER; u1ByteIdx++)
				{
					if (u1UpdateRegUI)
					{
						ucdq_reg_ui_large[u1ByteIdx] = ucdq_ui_large;
						ucdq_reg_ui_small[u1ByteIdx] = ucdq_ui_small;
						ucdq_reg_oen_ui_large[u1ByteIdx] = ucdq_oen_ui_large;
						ucdq_reg_oen_ui_small[u1ByteIdx] = ucdq_oen_ui_small;

						ucdq_reg_dqm_ui_large[u1ByteIdx] = ucdq_ui_large;
						ucdq_reg_dqm_ui_small[u1ByteIdx] = ucdq_ui_small;
						ucdq_reg_dqm_oen_ui_large[u1ByteIdx] = ucdq_oen_ui_large;
						ucdq_reg_dqm_oen_ui_small[u1ByteIdx] = ucdq_oen_ui_small;
					}

					ucdq_reg_pi[u1ByteIdx] = ucdq_pi;
					ucdq_reg_dqm_pi[u1ByteIdx] = ucdq_pi;
				}

				if (calType == TX_DQ_DQS_MOVE_DQ_ONLY || calType == TX_DQ_DQS_MOVE_DQ_DQM)
				{
					TXSetDelayReg_DQ(p, u1UpdateRegUI, ucdq_reg_ui_large, ucdq_reg_oen_ui_large, ucdq_reg_ui_small, ucdq_reg_oen_ui_small, ucdq_reg_pi);
				}

				if (calType == TX_DQ_DQS_MOVE_DQM_ONLY || calType == TX_DQ_DQS_MOVE_DQ_DQM)
				{
					TXSetDelayReg_DQM(p, u1UpdateRegUI, ucdq_reg_dqm_ui_large, ucdq_reg_dqm_oen_ui_large, ucdq_reg_dqm_ui_small, ucdq_reg_dqm_oen_ui_small, ucdq_reg_dqm_pi);
				}

				u4err_value = 0;
#if ENABLE_K_WITH_WORST_SI_UI_SHIFT
				//DramcEngine2SetPat(p, p->test_pattern, 0, 0, TE_UI_SHIFT);
				u4err_value = DramcEngine2Run(p, TE_OP_WRITE_READ_CHECK, p->test_pattern);
#else
				//audio + xtalk pattern
				DramcEngine2SetPat(p, TEST_AUDIO_PATTERN, 0, 0, TE_NO_UI_SHIFT);
				u4err_value = DramcEngine2Run(p, TE_OP_WRITE_READ_CHECK, TEST_AUDIO_PATTERN);
				DramcEngine2SetPat(p, TEST_XTALK_PATTERN, 0, 1, TE_NO_UI_SHIFT);
				u4err_value |= DramcEngine2Run(p, TE_OP_WRITE_READ_CHECK, TEST_XTALK_PATTERN);
#endif
				//audio + xtalk pattern
				//u4err_value = 0;
				//DramcEngine2SetPat(p, TEST_AUDIO_PATTERN, 0, 0);
				//u4err_value = DramcEngine2Run(p, TE_OP_WRITE_READ_CHECK, TEST_AUDIO_PATTERN);
				//DramcEngine2SetPat(p, TEST_XTALK_PATTERN, 0, 1);
				//u4err_value |= DramcEngine2Run(p, TE_OP_WRITE_READ_CHECK, TEST_XTALK_PATTERN);

				if (u1VrefScanEnable == 0 && (calType != TX_DQ_DQS_MOVE_DQM_ONLY))
				{
					//mcSHOW_DBG_MSG(("Delay=%3d |%2d %2d %3d| %2d %2d| 0x%8x [0]",uiDelay, ucdq_ui_large,ucdq_ui_small, ucdq_pi, ucdq_oen_ui_large,ucdq_oen_ui_small, u4err_value));
					#ifdef ETT_PRINT_FORMAT
					if (u4err_value != 0)
					{
						mcSHOW_DBG_MSG2(("%d |%d %d %d|[0]", uiDelay, ucdq_ui_large, ucdq_ui_small, ucdq_pi));
					}
					#else
					mcSHOW_DBG_MSG2(("Delay=%3d |%2d %2d %3d| 0x%8x [0]", uiDelay, ucdq_ui_large, ucdq_ui_small, ucdq_pi, u4err_value));
					#endif
				}

				// check fail bit ,0 ok ,others fail
				for (u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
				{
					u4fail_bit = u4err_value & ((U32)1 << u1BitIdx);

					if (u1VrefScanEnable == 0 && (calType != TX_DQ_DQS_MOVE_DQM_ONLY))
					{
						if(u4err_value != 0)
						{
							if (u1BitIdx % DQS_BIT_NUMBER == 0)
							{
								mcSHOW_DBG_MSG2((" "));
							}

							if (u4fail_bit == 0)
							{
								 mcSHOW_DBG_MSG2(("o"));
							}
							else
							{
								mcSHOW_DBG_MSG2(("x"));
							}
						}
					}

					if (WinPerBit[u1BitIdx].first_pass == PASS_RANGE_NA)
					{
						if (u4fail_bit == 0) //compare correct: pass
						{
							WinPerBit[u1BitIdx].first_pass = uiDelay;

							#if TX_TDQS2DQ_PRE_CAL
							if ((u1IsLP4Div4DDR800(p) == FALSE) && (calType == TX_DQ_DQS_MOVE_DQ_ONLY) && (u1VrefScanEnable == FALSE))
							{
								if (u2DQS2DQ_Pre_Cal[p->channel][p->rank][vGet_Div_Mode(p)] == 0)
								{
									u2DQS2DQ_Pre_Cal[p->channel][p->rank][vGet_Div_Mode(p)] = ((uiDelay - u2DQDelayBegin)* 1000) / p->frequency;
								}

								if (uiDelay == u2DQDelayBegin)
								{
									mcSHOW_ERR_MSG(("TX_TDQS2DQ_PRE_CAL: Warning, possible miss TX window boundary\n"));
									#if __ETT__
									while (1);
									#endif
								}
							}
							#endif
						}
					}
					else if (WinPerBit[u1BitIdx].last_pass == PASS_RANGE_NA)
					{
						if (u4fail_bit != 0) //compare error : fail
						{
							WinPerBit[u1BitIdx].last_pass = uiDelay - u2DQDelayStep;
						}
						else if (uiDelay > (u2DQDelayEnd - u2DQDelayStep))
						{
							WinPerBit[u1BitIdx].last_pass = uiDelay;
						}

						if (WinPerBit[u1BitIdx].last_pass != PASS_RANGE_NA)
						{
							if ((WinPerBit[u1BitIdx].last_pass - WinPerBit[u1BitIdx].first_pass) >= (VrefWinPerBit[u1BitIdx].last_pass - VrefWinPerBit[u1BitIdx].first_pass))
							{
								if ((VrefWinPerBit[u1BitIdx].last_pass != PASS_RANGE_NA) && (VrefWinPerBit[u1BitIdx].last_pass - VrefWinPerBit[u1BitIdx].first_pass) > 0)
								{
									mcSHOW_DBG_MSG2(("Bit[%d] Bigger window update %d > %d, window broken?\n", u1BitIdx, \
										(WinPerBit[u1BitIdx].last_pass - WinPerBit[u1BitIdx].first_pass), (VrefWinPerBit[u1BitIdx].last_pass - VrefWinPerBit[u1BitIdx].first_pass)));
								}

								//if window size bigger than TX_PASS_WIN_CRITERIA, consider as real pass window. If not, don't update finish counte and won't do early break;
								if ((WinPerBit[u1BitIdx].last_pass - WinPerBit[u1BitIdx].first_pass) > TX_PASS_WIN_CRITERIA)
									uiFinishCount |= (1 << u1BitIdx);

								//update bigger window size
								VrefWinPerBit[u1BitIdx].first_pass = WinPerBit[u1BitIdx].first_pass;
								VrefWinPerBit[u1BitIdx].last_pass = WinPerBit[u1BitIdx].last_pass;
							}

							//reset tmp window
							WinPerBit[u1BitIdx].first_pass = PASS_RANGE_NA;
							WinPerBit[u1BitIdx].last_pass = PASS_RANGE_NA;
						}
					 }
				}

				if(u1VrefScanEnable==0 && (calType != TX_DQ_DQS_MOVE_DQM_ONLY))
				{
					if(u4err_value != 0)
					{
						mcSHOW_DBG_MSG2((" [MSB]\n"));
					}
				}

				//if all bits widnow found and all bits turns to fail again, early break;
				if (uiFinishCount == 0xffff)
				{
					vSetCalibrationResult(p, DRAM_CALIBRATION_TX_PERBIT, DRAM_OK);
					#if !REDUCE_LOG_FOR_PRELOADER
					#ifdef ETT_PRINT_FORMAT
					mcSHOW_DBG_MSG2(("TX calibration finding left boundary early break. PI DQ delay=0x%B\n", uiDelay));
					#else
					mcSHOW_DBG_MSG2(("TX calibration finding left boundary early break. PI DQ delay=0x%2x\n", uiDelay));
					#endif
					#endif
					break;	//early break
				}
			}
			}

			if (isAutoK)
			{
			#if TX_AUTO_K_ENABLE
				Tx_Auto_K_complete_check(p);
				#if TX_AUTO_K_DEBUG_ENABLE
				Tx_Auto_K_Debug_Message(p, u1PI_Len);
				#endif
			#endif
			}

			// (1) calculate per bit window size
			// (2) find out min win of all DQ bits
			// (3) calculate perbit window center
			u1min_winsize = 0xff;
			u1min_bit = 0xff;
			for (u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
			{
				if (isAutoK)
				{
				#if TX_AUTO_K_ENABLE
				u1dq_shift = ((u1BitIdx % 4) * 8);
				VrefWinPerBit[u1BitIdx].first_pass = u2DQDelayBegin - ucdq_pi + ((u4IO32Read4B(DRAMC_REG_ADDR(PwMaxInitReg[u1BitIdx / 4])) & (0xff << u1dq_shift)) >> u1dq_shift);
				VrefWinPerBit[u1BitIdx].last_pass = ((u4IO32Read4B(DRAMC_REG_ADDR(PwMaxLenReg[u1BitIdx / 4])) & (0xff << u1dq_shift)) >> u1dq_shift) + VrefWinPerBit[u1BitIdx].first_pass;
				VrefWinPerBit[u1BitIdx].win_size = ((u4IO32Read4B(DRAMC_REG_ADDR(PwMaxLenReg[u1BitIdx / 4])) & (0xff << u1dq_shift)) >> u1dq_shift);

				if (u1PI_Len == 0)
					u4Length = 48;
				else
					u4Length = 32 * (1 + u1PI_Len);

				if ((VrefWinPerBit[u1BitIdx].first_pass == (int)(u2DQDelayBegin - ucdq_pi)) || (VrefWinPerBit[u1BitIdx].last_pass == (int)(u2DQDelayBegin + u4Length)))
				{
					mcSHOW_ERR_MSG(("Error! Probably miss pass window!\n"));
				}

				mcSHOW_DBG_MSG(("TX DQ bit %d, first pass: %d, last pass: %d\n", u1BitIdx, VrefWinPerBit[u1BitIdx].first_pass, VrefWinPerBit[u1BitIdx].last_pass));
				#else
				//if(VrefWinPerBit[u1BitIdx].last_pass == VrefWinPerBit[u1BitIdx].first_pass)
				if (VrefWinPerBit[u1BitIdx].first_pass == PASS_RANGE_NA)
					VrefWinPerBit[u1BitIdx].win_size = 0;
				else
					VrefWinPerBit[u1BitIdx].win_size = VrefWinPerBit[u1BitIdx].last_pass - VrefWinPerBit[u1BitIdx].first_pass + u2DQDelayStep;
				#endif
				}
				else
				{
					if (VrefWinPerBit[u1BitIdx].first_pass == PASS_RANGE_NA)
						VrefWinPerBit[u1BitIdx].win_size = 0;
					else
						VrefWinPerBit[u1BitIdx].win_size = VrefWinPerBit[u1BitIdx].last_pass - VrefWinPerBit[u1BitIdx].first_pass + u2DQDelayStep;
				}

				if (VrefWinPerBit[u1BitIdx].win_size < u1min_winsize)
				{
					u1min_bit = u1BitIdx;
					u1min_winsize = VrefWinPerBit[u1BitIdx].win_size;
				}

				u2TempWinSum += VrefWinPerBit[u1BitIdx].win_size;  //Sum of CA Windows for vref selection

				#if VENDER_JV_LOG
				if (calType == TX_DQ_DQS_MOVE_DQ_ONLY)
				{
					mcSHOW_DBG_MSG5(("TX Bit%d, %d%%\n", u1BitIdx, (VrefWinPerBit[u1BitIdx].win_size * 100 + 31) / 32));
				}
				#endif


				// calculate per bit window position and print
				VrefWinPerBit[u1BitIdx].win_center = (VrefWinPerBit[u1BitIdx].first_pass + VrefWinPerBit[u1BitIdx].last_pass) >> 1;
				#if PINMUX_AUTO_TEST_PER_BIT_TX
				gFinalTXPerbitFirstPass[p->channel][u1BitIdx] = VrefWinPerBit[u1BitIdx].first_pass;
				#endif
			}


			#if __ETT__
			if (u1VrefScanEnable == 0)
			{
				//mcSHOW_DBG_MSG(("\n\tCH=%d, VrefRange= %d, VrefLevel = %d\n", p->channel, u2FinalRange, u2VrefLevel));
				TxPrintWidnowInfo(p, VrefWinPerBit);
			}
			#endif

			if (u1VrefScanEnable == 1)
			{
				if (u2TempWinSum > u2MaxWindowSum)
					u2MaxWindowSum = u2TempWinSum;

				VrefInfo[u1VrefIdx].u2VrefUsed = u2VrefLevel;
				VrefInfo[u1VrefIdx].u1WorseBitWinSize_byVref = u1min_winsize;
				VrefInfo[u1VrefIdx].u1WorseBitIdx_byVref = u1min_bit;
				VrefInfo[u1VrefIdx].u2WinSum_byVref = u2TempWinSum;
				u1VrefIdx ++;
			}

			#if TX_AUTO_K_ENABLE
			if (isAutoK)
				Tx_Auto_K_Clear(p);
			#endif

			#if LP4_TX_VREF_PASS_CONDITION
			if (u1VrefScanEnable && (u2TempWinSum < (u2MaxWindowSum * 95 / 100)) && (u1min_winsize < LP4_TX_VREF_PASS_CONDITION))
			#else
			if (u1VrefScanEnable && (u2TempWinSum < (u2MaxWindowSum * 95 / 100)) && (u1min_winsize > TX_PASS_WIN_CRITERIA))
			#endif
			{
				mcSHOW_DBG_MSG(("\nTX Vref early break, caculate TX vref\n"));
				break;
			}

			#if TX_AUTO_K_ENABLE
			Tx_Auto_K_Clear(p);
			#endif
		}

		DramcEngine2End(p);

		#if (TX_AUTO_K_ENABLE && TX_AUTO_K_WORKAROUND)
		if ((isAutoK) && (p->rank == RANK_1))
		{
			vSetRank(p, RANK_0);
			DramcRestoreRegisters(p, u4RegBackupAddress, sizeof(u4RegBackupAddress) / sizeof(U32));
			vSetRank(p, backup_rank);
		}
		#endif

		if (u1VrefScanEnable == 0)// ..if time domain (not vref scan) , calculate window center of all bits.
		{
			// Calculate the center of DQ pass window
			// Record center sum of each byte
			for (u1ByteIdx = 0; u1ByteIdx < (p->data_width / DQS_BIT_NUMBER); u1ByteIdx++)
			{
				#if 1//TX_DQM_CALC_MAX_MIN_CENTER
				u2Center_min[u1ByteIdx] = 0xffff;
				u2Center_max[u1ByteIdx] = 0;
				#endif

				for (u1BitIdx = 0; u1BitIdx < DQS_BIT_NUMBER; u1BitIdx++)
				{
					u1BitTemp = u1ByteIdx * DQS_BIT_NUMBER + u1BitIdx;
					memcpy(FinalWinPerBit, VrefWinPerBit, sizeof(PASS_WIN_DATA_T) * DQ_DATA_WIDTH);

					if (FinalWinPerBit[u1BitTemp].win_center < u2Center_min[u1ByteIdx])
						u2Center_min[u1ByteIdx] = FinalWinPerBit[u1BitTemp].win_center;

					if (FinalWinPerBit[u1BitTemp].win_center > u2Center_max[u1ByteIdx])
						u2Center_max[u1ByteIdx] = FinalWinPerBit[u1BitTemp].win_center;

					#ifdef FOR_HQA_TEST_USED
					if ((calType == TX_DQ_DQS_MOVE_DQ_ONLY) && (u1VrefScanEnable == 0))
					{
						gFinalTXPerbitWin[p->channel][p->rank][u1BitTemp] = FinalWinPerBit[u1BitTemp].win_size;
					}
					#endif
				}
			}
		}
	}

	// SET tx Vref (DQ) = u2FinalVref, LP3 no need to set this.
	if (u1VrefScanEnable)
	{
		#if SUPPORT_SAVE_TIME_FOR_CALIBRATION && BYPASS_VREF_CAL
		if (p->femmc_Ready == 1 && (p->Bypass_TXWINDOW))
		{
			u2FinalVref = p->pSavetimeData->u1TxWindowPerbitVref_Save[p->channel][p->rank];
		}
		else
		#endif
		{
			u2FinalVref = TxChooseVref(p, VrefInfo, u1VrefIdx);
		}

		TXSetFinalVref(p, u2FinalRange, u2FinalVref);
		return DRAM_OK;
	}

#ifdef FOR_HQA_TEST_USED
	// LP4 DQ time domain || LP3 DQ_DQM time domain
	if (calType == TX_DQ_DQS_MOVE_DQ_ONLY)
	{
		gFinalTXPerbitWin_min_max[p->channel][p->rank] = u1min_winsize;
		if(u1min_winsize<16)
		{
			mcSHOW_ERR_MSG(("[WARNING] Smaller TX win !!\n"));
			#if CHECK_HQA_CRITERIA
			while(1);
			#endif
		}
	}
#endif

	// LP3 only use "TX_DQ_DQS_MOVE_DQ_DQM" scan
	// first freq 800(LP4-1600) doesn't support jitter meter(data < 1T), therefore, don't use delay cell
	if ((calType == TX_DQ_DQS_MOVE_DQ_ONLY) && (p->frequency >= 1333) && (p->u2DelayCellTimex100 != 0))
	{
		u1EnableDelayCell = 1;
		mcSHOW_DBG_MSG(("[TX_PER_BIT_DELAY_CELL] DelayCellTimex100 =%d/100 ps\n", p->u2DelayCellTimex100));
	}

	//Calculate the center of DQ pass window
	//average the center delay
	for (u1ByteIdx = 0; u1ByteIdx < (p->data_width / DQS_BIT_NUMBER); u1ByteIdx++)
	{
		mcSHOW_DBG_MSG((" == TX Byte %d ==\n", u1ByteIdx));
		u2DQM_Delay = ((u2Center_min[u1ByteIdx] + u2Center_max[u1ByteIdx]) >> 1); //(max +min)/2

		if (u1EnableDelayCell == 0)
		{
			uiDelay = u2DQM_Delay;
		}
		else// if(calType == TX_DQ_DQS_MOVE_DQ_ONLY)
		{
			uiDelay = u2Center_min[u1ByteIdx];	// for DQ PI delay , will adjust with delay cell

			// calculate delay cell perbit
			for (u1BitIdx = 0; u1BitIdx < DQS_BIT_NUMBER; u1BitIdx++)
			{
				u1BitTemp = u1ByteIdx * DQS_BIT_NUMBER + u1BitIdx;
				u1PIDiff = FinalWinPerBit[u1BitTemp].win_center - u2Center_min[u1ByteIdx];
				if (p->u2DelayCellTimex100 != 0)
				{
					u2DelayCellOfst[u1BitTemp] = (u1PIDiff * 100000000 / (p->frequency << 6)) / p->u2DelayCellTimex100;

					mcSHOW_DBG_MSG(("u2DelayCellOfst[%d]=%d cells (%d PI)\n", u1BitTemp, u2DelayCellOfst[u1BitTemp], u1PIDiff));

					if(u2DelayCellOfst[u1BitTemp]>255)
					{
						mcSHOW_DBG_MSG(("[WARNING] TX DQ%d delay cell %d >255, adjust to 255 cell\n", u1BitIdx, u2DelayCellOfst[u1BitTemp]));
						u2DelayCellOfst[u1BitTemp] =255;
					}
				}
				else
				{
					mcSHOW_ERR_MSG(("Error: Cell time (p->u2DelayCellTimex100) is 0 \n"));
					break;
				}
			}

		}

		TxWinTransferDelayToUIPI(p, uiDelay, 1, &ucdq_reg_ui_large[u1ByteIdx], &ucdq_reg_ui_small[u1ByteIdx], &ucdq_reg_pi[u1ByteIdx], \
								&ucdq_reg_oen_ui_large[u1ByteIdx], &ucdq_reg_oen_ui_small[u1ByteIdx]);

		TxWinTransferDelayToUIPI(p, u2DQM_Delay, 1, &ucdq_reg_dqm_ui_large[u1ByteIdx], &ucdq_reg_dqm_ui_small[u1ByteIdx], &ucdq_reg_dqm_pi[u1ByteIdx], \
								&ucdq_reg_dqm_oen_ui_large[u1ByteIdx], &ucdq_reg_dqm_oen_ui_small[u1ByteIdx]);

		if (calType == TX_DQ_DQS_MOVE_DQ_ONLY || calType == TX_DQ_DQS_MOVE_DQ_DQM)
		{
			mcSHOW_DBG_MSG(("Update DQ	dly =%d (%d ,%d, %d)  DQ  OEN =(%d ,%d)\n",
							uiDelay, ucdq_reg_ui_large[u1ByteIdx], ucdq_reg_ui_small[u1ByteIdx], ucdq_reg_pi[u1ByteIdx], \
							ucdq_reg_oen_ui_large[u1ByteIdx], ucdq_reg_oen_ui_small[u1ByteIdx]));
		}

		//if(calType ==TX_DQ_DQS_MOVE_DQM_ONLY || calType== TX_DQ_DQS_MOVE_DQ_DQM)
		{
			mcSHOW_DBG_MSG(("Update DQM dly =%d (%d ,%d, %d)  DQM OEN =(%d ,%d)",
					u2DQM_Delay, ucdq_reg_dqm_ui_large[u1ByteIdx], ucdq_reg_dqm_ui_small[u1ByteIdx], ucdq_reg_dqm_pi[u1ByteIdx], \
					ucdq_reg_dqm_oen_ui_large[u1ByteIdx], ucdq_reg_dqm_oen_ui_small[u1ByteIdx]));
		}
		mcSHOW_DBG_MSG(("\n"));

#ifdef FOR_HQA_REPORT_USED
		if (calType == TX_DQ_DQS_MOVE_DQ_ONLY)
		{
			for (u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
			{
				HQA_Log_Message_for_Report(p, p->channel, p->rank, HQA_REPORT_FORMAT1, "TX_Window_Center_", "DQ", u1BitIdx, FinalWinPerBit[u1BitIdx].win_center, NULL);
			}
		}

		if (calType == TX_DQ_DQS_MOVE_DQM_ONLY)
		{
			HQA_Log_Message_for_Report(p, p->channel, p->rank, HQA_REPORT_FORMAT0, "TX_Window_Center_", "DQM", u1ByteIdx, u2DQM_Delay, NULL);
		}
#if 0
		HQA_Log_Message_for_Report(p, p->channel, p->rank, HQA_REPORT_FORMAT1, "TX_Window_Center_", "LargeUI", u1ByteIdx, ucdq_reg_ui_large[u1ByteIdx], NULL);
		HQA_Log_Message_for_Report(p, p->channel, p->rank, HQA_REPORT_FORMAT0, "TX_Window_Center_", "SmallUI", u1ByteIdx, ucdq_reg_ui_small[u1ByteIdx], NULL);
		HQA_Log_Message_for_Report(p, p->channel, p->rank, HQA_REPORT_FORMAT0, "TX_Window_Center_", "PI", u1ByteIdx, ucdq_reg_pi[u1ByteIdx], NULL);
#endif
#endif

	}


#if REG_ACCESS_PORTING_DGB
	RegLogEnable = 1;
#endif

		/* p->rank = RANK_0, save to Reg Rank0 and Rank1, p->rank = RANK_1, save to Reg Rank1 */
		for (u1RankIdx = p->rank; u1RankIdx < RANK_MAX; u1RankIdx++)
		{
			vSetRank(p, u1RankIdx);

			if (calType == TX_DQ_DQS_MOVE_DQ_ONLY || calType == TX_DQ_DQS_MOVE_DQ_DQM)
			{
				TXSetDelayReg_DQ(p, TRUE, ucdq_reg_ui_large, ucdq_reg_oen_ui_large, ucdq_reg_ui_small, ucdq_reg_oen_ui_small, ucdq_reg_pi);
			}

			TXSetDelayReg_DQM(p, TRUE, ucdq_reg_dqm_ui_large, ucdq_reg_dqm_oen_ui_large, ucdq_reg_dqm_ui_small, ucdq_reg_dqm_oen_ui_small, ucdq_reg_dqm_pi);

			 if (u1EnableDelayCell)
			 {
				vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_TXDLY0),
						   P_Fld(u2DelayCellOfst[3], SHU_R0_B0_TXDLY0_TX_ARDQ3_DLY_B0)
						 | P_Fld(u2DelayCellOfst[2], SHU_R0_B0_TXDLY0_TX_ARDQ2_DLY_B0)
						 | P_Fld(u2DelayCellOfst[1], SHU_R0_B0_TXDLY0_TX_ARDQ1_DLY_B0)
						 | P_Fld(u2DelayCellOfst[0], SHU_R0_B0_TXDLY0_TX_ARDQ0_DLY_B0));
				 vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_TXDLY1),
						   P_Fld(u2DelayCellOfst[7], SHU_R0_B0_TXDLY1_TX_ARDQ7_DLY_B0)
						 | P_Fld(u2DelayCellOfst[6], SHU_R0_B0_TXDLY1_TX_ARDQ6_DLY_B0)
						 | P_Fld(u2DelayCellOfst[5], SHU_R0_B0_TXDLY1_TX_ARDQ5_DLY_B0)
						 | P_Fld(u2DelayCellOfst[4], SHU_R0_B0_TXDLY1_TX_ARDQ4_DLY_B0));
				 vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_TXDLY0),
						   P_Fld(u2DelayCellOfst[11], SHU_R0_B1_TXDLY0_TX_ARDQ3_DLY_B1)
						 | P_Fld(u2DelayCellOfst[10], SHU_R0_B1_TXDLY0_TX_ARDQ2_DLY_B1)
						 | P_Fld(u2DelayCellOfst[9], SHU_R0_B1_TXDLY0_TX_ARDQ1_DLY_B1)
						 | P_Fld(u2DelayCellOfst[8], SHU_R0_B1_TXDLY0_TX_ARDQ0_DLY_B1));
				 vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_TXDLY1),
						   P_Fld(u2DelayCellOfst[15], SHU_R0_B1_TXDLY1_TX_ARDQ7_DLY_B1)
						 | P_Fld(u2DelayCellOfst[14], SHU_R0_B1_TXDLY1_TX_ARDQ6_DLY_B1)
						 | P_Fld(u2DelayCellOfst[13], SHU_R0_B1_TXDLY1_TX_ARDQ5_DLY_B1)
						 | P_Fld(u2DelayCellOfst[12], SHU_R0_B1_TXDLY1_TX_ARDQ4_DLY_B1));
			 }

			#if ENABLE_TX_TRACKING
			TXUpdateTXTracking(p, calType, ucdq_reg_pi, ucdq_reg_dqm_pi);
			#endif
		}

		vSetRank(p, backup_rank);

		if (isAutoK)
		{
		#if TX_AUTO_K_ENABLE
			#if TX_AUTO_K_WORKAROUND
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TX_ATK_SET0),
								 P_Fld(ucdq_reg_pi[0], TX_ATK_SET0_TX_ATK_DQ_B0_PI_INIT) |
								 P_Fld(ucdq_reg_pi[1], TX_ATK_SET0_TX_ATK_DQ_B1_PI_INIT) |
								 P_Fld(ucdq_reg_dqm_pi[0], TX_ATK_SET0_TX_ATK_DQM_B0_PI_INIT) |
								 P_Fld(ucdq_reg_dqm_pi[1], TX_ATK_SET0_TX_ATK_DQM_B1_PI_INIT)); //If TX auto-k is enable, TX_PI will be switch to PI_INIT
			#endif
		#endif
		}

#if REG_ACCESS_PORTING_DGB
	RegLogEnable = 0;
#endif

#if (TX_AUTO_K_ENABLE && TX_AUTO_K_WORKAROUND)
	if ((isAutoK) && (p->rank == RANK_1) && (calType == TX_DQ_DQS_MOVE_DQ_DQM))
	{
		u4DQM_MCK_RK1_backup = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ1));
		u4DQM_UI_RK1_backup = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ3));
		u4DQM_PI_RK1_backup[0] = u4IO32Read4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_DQ0));
		u4DQM_PI_RK1_backup[1] = u4IO32Read4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_DQ0));
		u4DQ_MCK_RK1_backup = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ0));
		u4DQ_UI_RK1_backup = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ2));
		u4DQ_PI_RK1_backup[0] = u4IO32Read4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_DQ0));
		u4DQ_PI_RK1_backup[1] = u4IO32Read4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_DQ0));
	}
#endif

	mcSHOW_DBG_MSG3(("[TxWindowPerbitCal] Done\n\n"));

	#if 0
	vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DRAMC_REG_PADCTL4), 1, PADCTL4_CKEFIXON);  // test only
	#endif

	return DRAM_OK;
}

#endif //SIMULATION_TX_PERBIT

#if ENABLE_EYESCAN_GRAPH
void Dramc_K_TX_EyeScan_Log(DRAMC_CTX_T *p)
{
	U8 ucindex, u1BitIdx, u1ByteIdx;
	U8 ii, backup_rank, u1PrintWinData, u1vrefidx;
	PASS_WIN_DATA_T WinPerBit[DQ_DATA_WIDTH], VrefWinPerBit[DQ_DATA_WIDTH], FinalWinPerBit[DQ_DATA_WIDTH];
	U16 tx_pi_delay[4], tx_dqm_pi_delay[4];
	U16 u2DQDelayBegin, uiDelay;
	U16 u2VrefLevel, u2VrefBegin, u2VrefEnd, u2VrefStep, u2VrefRange;
	U8 ucdq_pi, ucdq_ui_small, ucdq_ui_large,ucdq_oen_ui_small, ucdq_oen_ui_large;
	U32 uiFinishCount;
	U16 u2TempWinSum, u2tx_window_sum=0;
	U32 u4err_value, u4fail_bit;
	#if 1//TX_DQM_CALC_MAX_MIN_CENTER
	U16 u2Center_min[DQS_NUMBER],u2Center_max[DQS_NUMBER];
	#endif

	U16 TXPerbitWin_min_max = 0;
	U32 min_bit, min_winsize;

	U16 u2FinalVref=0xd;
	U16 u2FinalRange=0;

	U8 EyeScan_index[DQ_DATA_WIDTH];

	U16 backup_u1MR14Value;
	U8 u1pass_in_this_vref_flag[DQ_DATA_WIDTH];

	U8 u1MCK2UI, u1UI2PI;

	U32 u4RegBackupAddress[] =
	{
		(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ0)),
		(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ2)),
		(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ1)),
		(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ3)),
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_DQ0)),
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_DQ0)),
	};

	if (GetEyeScanEnable(p, 2)==DISABLE) return;

	//if (gTX_EYE_Scan_only_higheset_freq_flag==1 && p->frequency != u2DFSGetHighestFreq(p)) return;

	//backup register value
	DramcBackupRegisters(p, u4RegBackupAddress, sizeof(u4RegBackupAddress)/sizeof(U32));

	backup_u1MR14Value = u1MR14Value[p->channel][p->rank][p->dram_fsp];
	//Jimmy Temp
	DramcModeRegReadByRank(p, p->rank, 14, &backup_u1MR14Value);

	if (gFinalTXVrefDQ[p->channel][p->rank] ==0) //Set final TX Vref as default value
		gFinalTXVrefDQ[p->channel][p->rank] = u1MR14Value[p->channel][p->rank][p->dram_fsp];

	//set initial values
	for(u1vrefidx=0; u1vrefidx<=VREF_VOLTAGE_TABLE_NUM_LP5-1;u1vrefidx++)
	{
		for (u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
		{
			for(ii=0; ii<EYESCAN_BROKEN_NUM; ii++)
			{
				gEyeScan_Min[u1vrefidx][u1BitIdx][ii] = EYESCAN_DATA_INVALID;
				gEyeScan_Max[u1vrefidx][u1BitIdx][ii] = EYESCAN_DATA_INVALID;
			}
			gEyeScan_ContinueVrefHeight[u1BitIdx] = 0;
			gEyeScan_TotalPassCount[u1BitIdx] = 0;
		}
	}


	u1MCK2UI = u1MCK2UI_DivShift(p);

	//if (vGet_DDR800_Mode(p) == DDR800_CLOSE_LOOP)
	//	  u1UI2PI = 6;
	//else
		u1UI2PI = 5;


	for(u1ByteIdx=0; u1ByteIdx < p->data_width/DQS_BIT_NUMBER; u1ByteIdx++)
	{
		if (u1ByteIdx == 0)
		{
			tx_pi_delay[u1ByteIdx] = (u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ0), SHURK_SELPH_DQ0_TXDLY_DQ0)<<(u1MCK2UI+u1UI2PI)) +
						  (u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ2), SHURK_SELPH_DQ2_DLY_DQ0)<<u1UI2PI) +
						  u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_DQ0), SHU_R0_B0_DQ0_SW_ARPI_DQ_B0)*(u1IsPhaseMode(p)==TRUE ? 8 : 1);

			tx_dqm_pi_delay[u1ByteIdx] = (u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ1), SHURK_SELPH_DQ1_TXDLY_DQM0)<<(u1MCK2UI+u1UI2PI)) +
							  (u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ3), SHURK_SELPH_DQ3_DLY_DQM0)<<u1UI2PI) +
							  u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_DQ0), SHU_R0_B0_DQ0_SW_ARPI_DQM_B0)*(u1IsPhaseMode(p)==TRUE ? 8 : 1);
		}
		else
		{
			tx_pi_delay[u1ByteIdx] = (u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ0), SHURK_SELPH_DQ0_TXDLY_DQ1)<<(u1MCK2UI+u1UI2PI)) +
						  (u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ2), SHURK_SELPH_DQ2_DLY_DQ1)<<u1UI2PI) +
						  u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_DQ0), SHU_R0_B1_DQ0_SW_ARPI_DQ_B1)*(u1IsPhaseMode(p)==TRUE ? 8 : 1);

			tx_dqm_pi_delay[u1ByteIdx] = (u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ1), SHURK_SELPH_DQ1_TXDLY_DQM1)<<(u1MCK2UI+u1UI2PI)) +
							  (u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ3), SHURK_SELPH_DQ3_DLY_DQM1)<<u1UI2PI) +
							  u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_DQ0), SHU_R0_B1_DQ0_SW_ARPI_DQM_B1)*(u1IsPhaseMode(p)==TRUE ? 8 : 1);
		}
	}

	if (tx_pi_delay[0] < tx_pi_delay[1])
	{
		u2DQDelayBegin = tx_pi_delay[0]-32;
	}
	else
	{
		u2DQDelayBegin = tx_pi_delay[1]-32;
	}

	u2VrefRange = 0;
	u2VrefBegin = 0;
	u2VrefEnd = (p->dram_type==TYPE_LPDDR5?VREF_VOLTAGE_TABLE_NUM_LP5:VREF_VOLTAGE_TABLE_NUM_LP4)-1;
	u2VrefStep = EYESCAN_GRAPH_CATX_VREF_STEP;
	mcSHOW_DBG_MSG3(("\nTX Vref %d -> %d, step: %d\n", u2VrefBegin, u2VrefEnd, u2VrefStep));

#if ENABLE_K_WITH_WORST_SI_UI_SHIFT
	DramcEngine2Init(p, p->test2_1, p->test2_2, p->test_pattern | 0x80, 0, TE_UI_SHIFT);//UI_SHIFT + LEN1
#else
	DramcEngine2Init(p, p->test2_1, p->test2_2, TEST_XTALK_PATTERN, 0, TE_NO_UI_SHIFT);
#endif


	for(u2VrefLevel = u2VrefBegin; u2VrefLevel <= u2VrefEnd; u2VrefLevel += u2VrefStep)
	{
		//set vref
//fra		 u1MR14Value[p->channel][p->rank][p->dram_fsp] = (u2VrefLevel | (u2VrefRange<<6));
		DramcTXSetVref(p, u2VrefRange, u2VrefLevel);
		mcSHOW_DBG_MSG3(("\n\n Set TX VrefRange %d, VrefLevel=%d\n", u2VrefRange, u2VrefLevel));

		// initialize parameters
		uiFinishCount = 0;
		u2TempWinSum =0;

		for (u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
		{
			WinPerBit[u1BitIdx].first_pass = (S16)PASS_RANGE_NA;
			WinPerBit[u1BitIdx].last_pass = (S16)PASS_RANGE_NA;
			VrefWinPerBit[u1BitIdx].first_pass = (S16)PASS_RANGE_NA;
			VrefWinPerBit[u1BitIdx].last_pass = (S16)PASS_RANGE_NA;

			gEyeScan_DelayCellPI[u1BitIdx] = 0;

			EyeScan_index[u1BitIdx] = 0;
			u1pass_in_this_vref_flag[u1BitIdx] = 0;
		}

		for (uiDelay=0; uiDelay<64; uiDelay+=(u1IsPhaseMode(p)==TRUE ? 8 : 1))
		{
			TxWinTransferDelayToUIPI(p, tx_pi_delay[0]+uiDelay-32, 0, &ucdq_ui_large, &ucdq_ui_small, &ucdq_pi, &ucdq_oen_ui_large, &ucdq_oen_ui_small);
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ0), \
											 P_Fld(ucdq_ui_large, SHURK_SELPH_DQ0_TXDLY_DQ0) | \
											 P_Fld(ucdq_oen_ui_large, SHURK_SELPH_DQ0_TXDLY_OEN_DQ0));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ2), \
											 P_Fld(ucdq_ui_small, SHURK_SELPH_DQ2_DLY_DQ0) | \
											 P_Fld(ucdq_oen_ui_small, SHURK_SELPH_DQ2_DLY_OEN_DQ0));
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_DQ0), ucdq_pi, SHU_R0_B0_DQ0_SW_ARPI_DQ_B0);

			TxWinTransferDelayToUIPI(p, tx_pi_delay[1]+uiDelay-32, 0, &ucdq_ui_large, &ucdq_ui_small, &ucdq_pi, &ucdq_oen_ui_large, &ucdq_oen_ui_small);
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ0), \
											 P_Fld(ucdq_ui_large, SHURK_SELPH_DQ0_TXDLY_DQ1) | \
											 P_Fld(ucdq_oen_ui_large, SHURK_SELPH_DQ0_TXDLY_OEN_DQ1));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ2), \
											 P_Fld(ucdq_ui_small, SHURK_SELPH_DQ2_DLY_DQ1) | \
											 P_Fld(ucdq_oen_ui_small, SHURK_SELPH_DQ2_DLY_OEN_DQ1));
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_DQ0), ucdq_pi, SHU_R0_B1_DQ0_SW_ARPI_DQ_B1);

			TxWinTransferDelayToUIPI(p, tx_dqm_pi_delay[0]+uiDelay-32, 0, &ucdq_ui_large, &ucdq_ui_small, &ucdq_pi, &ucdq_oen_ui_large, &ucdq_oen_ui_small);
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ1), \
											P_Fld(ucdq_ui_large, SHURK_SELPH_DQ1_TXDLY_DQM0) | \
											P_Fld(ucdq_oen_ui_large, SHURK_SELPH_DQ1_TXDLY_OEN_DQM0));
		   vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ3), \
											P_Fld(ucdq_ui_small, SHURK_SELPH_DQ3_DLY_DQM0) | \
											P_Fld(ucdq_oen_ui_small, SHURK_SELPH_DQ3_DLY_OEN_DQM0));
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_DQ0), ucdq_pi,  SHU_R0_B0_DQ0_SW_ARPI_DQM_B0);

			TxWinTransferDelayToUIPI(p, tx_dqm_pi_delay[1]+uiDelay-32, 0, &ucdq_ui_large, &ucdq_ui_small, &ucdq_pi, &ucdq_oen_ui_large, &ucdq_oen_ui_small);
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ1), \
											 P_Fld(ucdq_ui_large, SHURK_SELPH_DQ1_TXDLY_DQM1) | \
											 P_Fld(ucdq_oen_ui_large, SHURK_SELPH_DQ1_TXDLY_OEN_DQM1));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ3), \
											 P_Fld(ucdq_ui_small, SHURK_SELPH_DQ3_DLY_DQM1) | \
											 P_Fld(ucdq_oen_ui_small, SHURK_SELPH_DQ3_DLY_OEN_DQM1));
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_DQ0), ucdq_pi, SHU_R0_B1_DQ0_SW_ARPI_DQM_B1);

			u4err_value=0;
#if ENABLE_K_WITH_WORST_SI_UI_SHIFT
			//DramcEngine2SetPat(p, p->test_pattern, 0, 0, TE_UI_SHIFT);
			u4err_value = DramcEngine2Run(p, TE_OP_WRITE_READ_CHECK, p->test_pattern);
#else
			//audio + xtalk pattern
			DramcEngine2SetPat(p, TEST_AUDIO_PATTERN, 0, 0, TE_NO_UI_SHIFT);
			u4err_value = DramcEngine2Run(p, TE_OP_WRITE_READ_CHECK, TEST_AUDIO_PATTERN);
			DramcEngine2SetPat(p, TEST_XTALK_PATTERN, 0, 1, TE_NO_UI_SHIFT);
			u4err_value |= DramcEngine2Run(p, TE_OP_WRITE_READ_CHECK, TEST_XTALK_PATTERN);
#endif
			// audio + xtalk pattern
			//u4err_value=0;
			//DramcEngine2SetPat(p,TEST_AUDIO_PATTERN, 0, 0, TE_NO_UI_SHIFT);
			//u4err_value = DramcEngine2Run(p, TE_OP_WRITE_READ_CHECK, TEST_AUDIO_PATTERN);
			//DramcEngine2SetPat(p,TEST_XTALK_PATTERN, 0, 1, TE_NO_UI_SHIFT);
			//u4err_value |= DramcEngine2Run(p, TE_OP_WRITE_READ_CHECK, TEST_XTALK_PATTERN);

			// check fail bit ,0 ok ,others fail
			for (u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
			{
				u4fail_bit = u4err_value&((U32)1<<u1BitIdx);

				if (u4fail_bit == 0)
				{
					gEyeScan_TotalPassCount[u1BitIdx]+=EYESCAN_GRAPH_CATX_VREF_STEP;
				}

				if(WinPerBit[u1BitIdx].first_pass== PASS_RANGE_NA)
				{
					if(u4fail_bit==0) //compare correct: pass
					{
						WinPerBit[u1BitIdx].first_pass = uiDelay;
						u1pass_in_this_vref_flag[u1BitIdx] = 1;
					}
				}
				else if(WinPerBit[u1BitIdx].last_pass == PASS_RANGE_NA)
				{
					if(u4fail_bit !=0) //compare error : fail
					{
						WinPerBit[u1BitIdx].last_pass  = (uiDelay-1);
					}
					else if (uiDelay>=63)
					{
						WinPerBit[u1BitIdx].last_pass  = 63;
					}

					if(WinPerBit[u1BitIdx].last_pass  !=PASS_RANGE_NA)
					{
						if((WinPerBit[u1BitIdx].last_pass -WinPerBit[u1BitIdx].first_pass) >= (VrefWinPerBit[u1BitIdx].last_pass -VrefWinPerBit[u1BitIdx].first_pass))
						{
							//if window size bigger than 7, consider as real pass window. If not, don't update finish counte and won't do early break;
							if((WinPerBit[u1BitIdx].last_pass -WinPerBit[u1BitIdx].first_pass) >7)
								uiFinishCount |= (1<<u1BitIdx);

							//update bigger window size
							VrefWinPerBit[u1BitIdx].first_pass = WinPerBit[u1BitIdx].first_pass;
							VrefWinPerBit[u1BitIdx].last_pass = WinPerBit[u1BitIdx].last_pass;
						}


							if (EyeScan_index[u1BitIdx] < EYESCAN_BROKEN_NUM)
							{
#if VENDER_JV_LOG || defined(RELEASE)
								gEyeScan_Min[(u2VrefLevel+u2VrefRange*30)/EYESCAN_GRAPH_CATX_VREF_STEP][u1BitIdx][EyeScan_index[u1BitIdx]] = WinPerBit[u1BitIdx].first_pass;
								gEyeScan_Max[(u2VrefLevel+u2VrefRange*30)/EYESCAN_GRAPH_CATX_VREF_STEP][u1BitIdx][EyeScan_index[u1BitIdx]] = WinPerBit[u1BitIdx].last_pass;
#else
//fra								 gEyeScan_Min[(u2VrefLevel+u2VrefRange*30)/EYESCAN_GRAPH_CATX_VREF_STEP][u1BitIdx][EyeScan_index[u1BitIdx]] = WinPerBit[u1BitIdx].first_pass + tx_pi_delay[u1BitIdx/8]-32;
//fra								 gEyeScan_Max[(u2VrefLevel+u2VrefRange*30)/EYESCAN_GRAPH_CATX_VREF_STEP][u1BitIdx][EyeScan_index[u1BitIdx]] = WinPerBit[u1BitIdx].last_pass + tx_pi_delay[u1BitIdx/8]-32;
								gEyeScan_Min[(u2VrefLevel+u2VrefRange*30)/EYESCAN_GRAPH_CATX_VREF_STEP][u1BitIdx][EyeScan_index[u1BitIdx]] = (S8) WinPerBit[u1BitIdx].first_pass;
								gEyeScan_Max[(u2VrefLevel+u2VrefRange*30)/EYESCAN_GRAPH_CATX_VREF_STEP][u1BitIdx][EyeScan_index[u1BitIdx]] = (S8) WinPerBit[u1BitIdx].last_pass;
								mcSHOW_DBG_MSG3(("VrefRange %d, VrefLevel=%d, u1BitIdx=%d, index=%d (%d, %d)==\n",u2VrefRange,u2VrefLevel, u1BitIdx, EyeScan_index[u1BitIdx], gEyeScan_Min[u2VrefLevel/EYESCAN_GRAPH_CATX_VREF_STEP][u1BitIdx][EyeScan_index[u1BitIdx]], gEyeScan_Max[u2VrefLevel/EYESCAN_GRAPH_CATX_VREF_STEP][u1BitIdx][EyeScan_index[u1BitIdx]]));
								gEyeScan_MinMax_store_delay[u1BitIdx/8] =  tx_pi_delay[u1BitIdx/8]-32; /* save this information for HQA pass/fail judgement used */
#endif
								EyeScan_index[u1BitIdx]=EyeScan_index[u1BitIdx]+1;
							}


						//reset tmp window
						WinPerBit[u1BitIdx].first_pass = PASS_RANGE_NA;
						WinPerBit[u1BitIdx].last_pass = PASS_RANGE_NA;
					}
				 }
			   }
		}

		min_winsize = 0xffff;
		min_bit = 0xff;
		for (u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
		{
			VrefWinPerBit[u1BitIdx].win_size = VrefWinPerBit[u1BitIdx].last_pass- VrefWinPerBit[u1BitIdx].first_pass +(VrefWinPerBit[u1BitIdx].last_pass==VrefWinPerBit[u1BitIdx].first_pass?0:1);

			if (VrefWinPerBit[u1BitIdx].win_size < min_winsize)
			{
				min_bit = u1BitIdx;
				min_winsize = VrefWinPerBit[u1BitIdx].win_size;
			}

			u2TempWinSum += VrefWinPerBit[u1BitIdx].win_size;  //Sum of CA Windows for vref selection

			gEyeScan_WinSize[(u2VrefLevel+u2VrefRange*30)/EYESCAN_GRAPH_CATX_VREF_STEP][u1BitIdx] = VrefWinPerBit[u1BitIdx].win_size;

#ifdef FOR_HQA_TEST_USED
			if((((backup_u1MR14Value>>6)&1) == u2VrefRange) && ((backup_u1MR14Value&0x3f)==u2VrefLevel))
			{
				gFinalTXPerbitWin[p->channel][p->rank][u1BitIdx] = VrefWinPerBit[u1BitIdx].win_size;
			}
#endif

		}

		if ((min_winsize > TXPerbitWin_min_max) || ((min_winsize == TXPerbitWin_min_max) && (u2TempWinSum >u2tx_window_sum)))
		{
			TXPerbitWin_min_max = min_winsize;
			u2tx_window_sum =u2TempWinSum;
			u2FinalRange = u2VrefRange;
			u2FinalVref = u2VrefLevel;

			//Calculate the center of DQ pass window
			// Record center sum of each byte
			for (u1ByteIdx=0; u1ByteIdx<(p->data_width/DQS_BIT_NUMBER); u1ByteIdx++)
			{
		#if 1//TX_DQM_CALC_MAX_MIN_CENTER
				u2Center_min[u1ByteIdx] = 0xffff;
				u2Center_max[u1ByteIdx] = 0;
		#endif

				for (u1BitIdx=0; u1BitIdx<DQS_BIT_NUMBER; u1BitIdx++)
				{
					ucindex = u1ByteIdx * DQS_BIT_NUMBER + u1BitIdx;
					FinalWinPerBit[ucindex].first_pass = VrefWinPerBit[ucindex].first_pass;
					FinalWinPerBit[ucindex].last_pass =  VrefWinPerBit[ucindex].last_pass;
					FinalWinPerBit[ucindex].win_size = VrefWinPerBit[ucindex].win_size;
					FinalWinPerBit[ucindex].win_center = (FinalWinPerBit[ucindex].first_pass + FinalWinPerBit[ucindex].last_pass) >> 1;

					if(FinalWinPerBit[ucindex].win_center < u2Center_min[u1ByteIdx])
						u2Center_min[u1ByteIdx] = FinalWinPerBit[ucindex].win_center;

					if(FinalWinPerBit[ucindex].win_center > u2Center_max[u1ByteIdx])
						u2Center_max[u1ByteIdx] = FinalWinPerBit[ucindex].win_center;
				}
			}
		}


		if(u2VrefRange==0 && u2VrefLevel ==50 && p->dram_type!=TYPE_LPDDR5)
		{
			u2VrefRange = 1;
			u2VrefLevel = 20;
		}

		for (u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
		{
			if (u1pass_in_this_vref_flag[u1BitIdx]) gEyeScan_ContinueVrefHeight[u1BitIdx]+=EYESCAN_GRAPH_CATX_VREF_STEP;  //count pass number of continue vref
		}
	}

	DramcEngine2End(p);

	//Calculate the center of DQ pass window
	//average the center delay
	for (u1ByteIdx=0; u1ByteIdx<(p->data_width/DQS_BIT_NUMBER); u1ByteIdx++)
	{
		uiDelay = ((u2Center_min[u1ByteIdx] + u2Center_max[u1ByteIdx])>>1); //(max +min)/2

#if VENDER_JV_LOG || defined(RELEASE)
		gEyeScan_CaliDelay[u1ByteIdx] = uiDelay;
#else
		gEyeScan_CaliDelay[u1ByteIdx] = uiDelay + tx_pi_delay[u1ByteIdx]-32;
#endif
	}


	//restore to orignal value
	DramcRestoreRegisters(p, u4RegBackupAddress, sizeof(u4RegBackupAddress)/sizeof(U32));

	//restore Vref
	#ifdef __LP5_COMBO__
	if (is_lp5_family(p))
	{
	   u2VrefRange = 0;
	   u2VrefLevel = backup_u1MR14Value;
	}
	else
	#endif
	{
	   u2VrefRange = backup_u1MR14Value>>6;
	   u2VrefLevel = backup_u1MR14Value & 0x3f;
	}
	DramcTXSetVref(p, u2VrefRange, u2VrefLevel);
	u1MR14Value[p->channel][p->rank][p->dram_fsp] = backup_u1MR14Value;

}
#endif

#if TX_OE_CALIBATION
#define TX_OE_PATTERN_USE_TA2 1
#define TX_OE_SCAN_FULL_RANGE 0

void DramcTxOECalibration(DRAMC_CTX_T *p)
{
	U8 u1ByteIdx;
	//U8 ucbegin=0xff, , ucfirst, ucsum, ucbest_step;
	U8 ucdq_oen_ui_large[2] = {0}, ucdq_oen_ui_small[2] = {0};
	//U8 ucdq_ui_large_reg_value=0xff, ucdq_ui_small_reg_value=0xff;
	U8 u1TxDQOEShift = 0;

	u1TxDQOEShift = TX_DQ_OE_SHIFT_LP4;

	#if TX_OE_PATTERN_USE_TA2
	mcSHOW_DBG_MSG(("\n[DramC_TX_OE_Calibration] TA2\n"));
	#else
	mcSHOW_DBG_MSG(("\n[DramC_TX_OE_Calibration] DMA\n"));
	#endif

	//default set FAIL
	vSetCalibrationResult(p, DRAM_CALIBRATION_TX_OE, DRAM_FAIL);

#if (SUPPORT_SAVE_TIME_FOR_CALIBRATION)
	if (p->femmc_Ready == 1)
	{
		for (u1ByteIdx = 0; u1ByteIdx < DQS_NUMBER_LP4; u1ByteIdx++)
		{
			ucdq_oen_ui_large[u1ByteIdx] = p->pSavetimeData->u1TX_OE_DQ_MCK[p->channel][p->rank][u1ByteIdx];
			ucdq_oen_ui_small[u1ByteIdx] = p->pSavetimeData->u1TX_OE_DQ_UI[p->channel][p->rank][u1ByteIdx];
		}
	 vSetCalibrationResult(p, DRAM_CALIBRATION_TX_OE, DRAM_FAST_K);
	}
#endif

	for (u1ByteIdx = 0; u1ByteIdx < DQS_NUMBER_LP4; u1ByteIdx++)
	{
		mcSHOW_DBG_MSG(("Byte%d TX OE(2T, 0.5T) = (%d, %d)\n", u1ByteIdx, ucdq_oen_ui_large[u1ByteIdx], ucdq_oen_ui_small[u1ByteIdx]));
	}
	mcSHOW_DBG_MSG(("\n\n"));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ0), \
									P_Fld(ucdq_oen_ui_large[0], SHURK_SELPH_DQ0_TXDLY_OEN_DQ0) | \
									P_Fld(ucdq_oen_ui_large[1], SHURK_SELPH_DQ0_TXDLY_OEN_DQ1));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ1), \
									P_Fld(ucdq_oen_ui_large[0], SHURK_SELPH_DQ1_TXDLY_OEN_DQM0) | \
									P_Fld(ucdq_oen_ui_large[1], SHURK_SELPH_DQ1_TXDLY_OEN_DQM1));
	// DLY_DQ[2:0]
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ2), \
									P_Fld(ucdq_oen_ui_small[0], SHURK_SELPH_DQ2_DLY_OEN_DQ0) | \
									P_Fld(ucdq_oen_ui_small[1], SHURK_SELPH_DQ2_DLY_OEN_DQ1) );
	 // DLY_DQM[2:0]
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ3), \
									 P_Fld(ucdq_oen_ui_small[0], SHURK_SELPH_DQ3_DLY_OEN_DQM0) | \
									 P_Fld(ucdq_oen_ui_small[1], SHURK_SELPH_DQ3_DLY_OEN_DQM1));
}
#endif

//-------------------------------------------------------------------------
/** DramcMiockJmeter
 *	start MIOCK jitter meter.
 *	@param p				Pointer of context created by DramcCtxCreate.
 *	@param block_no 		(U8): block 0 or 1.
 *	@retval status			(DRAM_STATUS_T): DRAM_OK or DRAM_FAIL
 */
//-------------------------------------------------------------------------

#ifdef ENABLE_MIOCK_JMETER
DRAM_STATUS_T DramcMiockJmeter(DRAMC_CTX_T *p)
{
	U16 ucsearch_state, fgcurrent_value, fginitial_value, ucstart_period = 0, ucmiddle_period = 0, ucend_period = 0;
	U32 u4sample_cnt, u4ones_cnt[DQS_NUMBER];
	U16 u2real_freq, u2real_period, ucdqs_dly;
	U16 u2Jm_dly_start = 0, u2Jm_dly_end = 512, u2Jm_dly_step = 4;
	U8 u1ShuLevel;
	U32 u4PLL3_ADDR, u4B0_DQ;
	U32 u4PLL5_ADDR;
	U32 u4PLL8_ADDR;
	U32 u4SDM_PCW;
	U32 u4PREDIV;
	U32 u4POSDIV;
	U32 u4CKDIV4;
	U32 u4VCOFreq;
	U32 u4DataRate;
	U8 u1RxGatingPI = 0, u1RxGatingPI_start = 0, u1RxGatingPI_end = 63;
	U8 backup_rank, u1RankIdx, u1FBKSEL;

	u1RxGatingPI = 0x0;

	u2gdelay_cell_ps = 0;

	// error handling
	if (!p)
	{
		mcSHOW_ERR_MSG(("context NULL\n"));
		return DRAM_FAIL;
	}

	U32 u4RegBackupAddress[] =
	{
		(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DUTYSCAN1)),
		(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ6)),
		(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ6)),
		(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ5)),
		(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ5)),
		(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ3)),
		(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ3)),
		(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CTRL1)),
		(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CTRL4)),
		(DRAMC_REG_ADDR(DDRPHY_REG_B0_PHY2)),
		(DRAMC_REG_ADDR(DDRPHY_REG_B1_PHY2)),
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DLL_ARPI2)),
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DLL_ARPI2)),
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ11)),
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ11)),
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD11)),
		(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL)),
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B0_DQSIEN_PI_DLY)), // need porting to Jmeter
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B0_DQSIEN_PI_DLY + DDRPHY_AO_RANK_OFFSET)), // need porting to Jmeter
		(DRAMC_REG_ADDR(DDRPHY_REG_MISC_JMETER)),
		//(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL2)), // for gating on/off
		//(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DVFSCTL2)), // for gating on/off
		//(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_STBCAL)), // for gating on/off
		((DDRPHY_REG_SHU_CA_DLL1)),
		((DDRPHY_REG_SHU_B0_DLL1)),
		((DDRPHY_REG_SHU_B1_DLL1)),
		((DDRPHY_REG_B0_DQ2)),
		((DDRPHY_REG_B1_DQ2)),
		((DDRPHY_REG_CA_CMD2)),
		((DDRPHY_REG_SHU_B0_DQ13)),
		((DDRPHY_REG_SHU_B1_DQ13)),
		((DDRPHY_REG_SHU_CA_CMD13)),

		((DDRPHY_REG_SHU_CA_DLL1) + SHIFT_TO_CHB_ADDR),
		((DDRPHY_REG_SHU_B0_DLL1) + SHIFT_TO_CHB_ADDR),
		((DDRPHY_REG_SHU_B1_DLL1) + SHIFT_TO_CHB_ADDR),
		((DDRPHY_REG_B0_DQ2) + SHIFT_TO_CHB_ADDR),
		((DDRPHY_REG_B1_DQ2) + SHIFT_TO_CHB_ADDR),
		((DDRPHY_REG_CA_CMD2) + SHIFT_TO_CHB_ADDR),
		((DDRPHY_REG_SHU_B0_DQ13) + SHIFT_TO_CHB_ADDR),
		((DDRPHY_REG_SHU_B1_DQ13) + SHIFT_TO_CHB_ADDR),
		((DDRPHY_REG_SHU_CA_CMD13) + SHIFT_TO_CHB_ADDR),
#if (CHANNEL_NUM > 2)
		((DDRPHY_REG_SHU_CA_DLL1) + SHIFT_TO_CHC_ADDR),
		((DDRPHY_REG_SHU_B0_DLL1) + SHIFT_TO_CHC_ADDR),
		((DDRPHY_REG_SHU_B1_DLL1) + SHIFT_TO_CHC_ADDR),
		((DDRPHY_REG_B0_DQ2) + SHIFT_TO_CHC_ADDR),
		((DDRPHY_REG_B1_DQ2) + SHIFT_TO_CHC_ADDR),
		((DDRPHY_REG_CA_CMD2) + SHIFT_TO_CHC_ADDR),
		((DDRPHY_REG_SHU_B0_DQ13) + SHIFT_TO_CHC_ADDR),
		((DDRPHY_REG_SHU_B1_DQ13) + SHIFT_TO_CHC_ADDR),
		((DDRPHY_REG_SHU_CA_CMD13) + SHIFT_TO_CHC_ADDR),

		((DDRPHY_REG_SHU_CA_DLL1) + SHIFT_TO_CHD_ADDR),
		((DDRPHY_REG_SHU_B0_DLL1) + SHIFT_TO_CHD_ADDR),
		((DDRPHY_REG_SHU_B1_DLL1) + SHIFT_TO_CHD_ADDR),
		((DDRPHY_REG_B0_DQ2) + SHIFT_TO_CHD_ADDR),
		((DDRPHY_REG_B1_DQ2) + SHIFT_TO_CHD_ADDR),
		((DDRPHY_REG_CA_CMD2) + SHIFT_TO_CHD_ADDR),
		((DDRPHY_REG_SHU_B0_DQ13) + SHIFT_TO_CHD_ADDR),
		((DDRPHY_REG_SHU_B1_DQ13) + SHIFT_TO_CHD_ADDR),
		((DDRPHY_REG_SHU_CA_CMD13) + SHIFT_TO_CHD_ADDR),
#endif
	};

	backup_rank = u1GetRank(p);

	//backup register value
	DramcBackupRegisters(p, u4RegBackupAddress, sizeof(u4RegBackupAddress) / sizeof(U32));

	//OE disable - start
	vIO32WriteFldMulti_All(DDRPHY_REG_B0_DQ2, P_Fld( 0 , B0_DQ2_RG_TX_ARDQS_OE_TIE_SEL_B0 ) \
														  | P_Fld( 1	   , B0_DQ2_RG_TX_ARDQS_OE_TIE_EN_B0  ) \
														  | P_Fld( 0 , B0_DQ2_RG_TX_ARWCK_OE_TIE_SEL_B0 ) \
														  | P_Fld( 1	   , B0_DQ2_RG_TX_ARWCK_OE_TIE_EN_B0  ) \
														  | P_Fld( 0 , B0_DQ2_RG_TX_ARWCKB_OE_TIE_SEL_B0 ) \
														  | P_Fld( 1	   , B0_DQ2_RG_TX_ARWCKB_OE_TIE_EN_B0  ) \
														  | P_Fld( 0 , B0_DQ2_RG_TX_ARDQM_OE_TIE_SEL_B0 ) \
														  | P_Fld( 1	   , B0_DQ2_RG_TX_ARDQM_OE_TIE_EN_B0  ) \
														  | P_Fld( 0 , B0_DQ2_RG_TX_ARDQ_OE_TIE_SEL_B0 ) \
														  | P_Fld( 0xff 	  , B0_DQ2_RG_TX_ARDQ_OE_TIE_EN_B0	) );

	vIO32WriteFldMulti_All(DDRPHY_REG_B1_DQ2, P_Fld( 0 , B1_DQ2_RG_TX_ARDQS_OE_TIE_SEL_B1 ) \
														  | P_Fld( 1	   , B1_DQ2_RG_TX_ARDQS_OE_TIE_EN_B1  ) \
														  | P_Fld( 0 , B1_DQ2_RG_TX_ARWCK_OE_TIE_SEL_B1 ) \
														  | P_Fld( 1	   , B1_DQ2_RG_TX_ARWCK_OE_TIE_EN_B1  ) \
														  | P_Fld( 0 , B1_DQ2_RG_TX_ARWCKB_OE_TIE_SEL_B1 ) \
														  | P_Fld( 1	   , B1_DQ2_RG_TX_ARWCKB_OE_TIE_EN_B1  ) \
														  | P_Fld( 0 , B1_DQ2_RG_TX_ARDQM_OE_TIE_SEL_B1 ) \
														  | P_Fld( 1	   , B1_DQ2_RG_TX_ARDQM_OE_TIE_EN_B1  ) \
														  | P_Fld( 0 , B1_DQ2_RG_TX_ARDQ_OE_TIE_SEL_B1 ) \
														  | P_Fld( 0xff 	  , B1_DQ2_RG_TX_ARDQ_OE_TIE_EN_B1	) );

	vIO32WriteFldMulti_All(DDRPHY_REG_CA_CMD2, P_Fld( 0 , CA_CMD2_RG_TX_ARCLK_OE_TIE_SEL_CA ) \
														  | P_Fld( 1	   , CA_CMD2_RG_TX_ARCLK_OE_TIE_EN_CA  ) \
														  | P_Fld( 0 , CA_CMD2_RG_TX_ARCS_OE_TIE_SEL_CA ) \
														  | P_Fld( 1	   , CA_CMD2_RG_TX_ARCS_OE_TIE_EN_CA  ) \
														  | P_Fld( 0 , CA_CMD2_RG_TX_ARCA_OE_TIE_SEL_CA ) \
														  | P_Fld( 0xff 	  , CA_CMD2_RG_TX_ARCA_OE_TIE_EN_CA  ) );

	vIO32WriteFldMulti_All(DDRPHY_REG_SHU_B0_DQ13 , P_Fld( 0 , SHU_B0_DQ13_RG_TX_ARDQSB_OE_TIE_SEL_B0  ) \
															  | P_Fld( 1	   , SHU_B0_DQ13_RG_TX_ARDQSB_OE_TIE_EN_B0	 ));
	vIO32WriteFldMulti_All(DDRPHY_REG_SHU_B1_DQ13 , P_Fld( 0 , SHU_B1_DQ13_RG_TX_ARDQSB_OE_TIE_SEL_B1  ) \
															  | P_Fld( 1	   , SHU_B1_DQ13_RG_TX_ARDQSB_OE_TIE_EN_B1	 ));
	vIO32WriteFldMulti_All(DDRPHY_REG_SHU_CA_CMD13, P_Fld( 0 , SHU_CA_CMD13_RG_TX_ARCLKB_OE_TIE_SEL_CA ) \
															  | P_Fld( 1	   , SHU_CA_CMD13_RG_TX_ARCLKB_OE_TIE_EN_CA  ));
	//OE disable - end

	//DramcHWGatingOnOff(p, 0); // disable Gating tracking for DQS PI, Remove to vApplyConfigBeforeCalibration

	// @A60868 for *RANK_SEL_SER_EN* = 0 to DA_RX_ARDQ_RANK_SEL_TXD_*[0]
	//				for *RANK_SEL_SER_EN* = 1 to DA_RX_ARDQ_RANK_SEL_TXD_*[7:0]
	// The *RANK_SEL_SER_EN* = 0 is old mode.
	// The *RANK_SEL_SER_EN* = 1 is new mode when background no any access.
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ11), 0, SHU_B0_DQ11_RG_RX_ARDQ_RANK_SEL_SER_EN_B0);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ11), 0, SHU_B1_DQ11_RG_RX_ARDQ_RANK_SEL_SER_EN_B1);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD11), 0, SHU_CA_CMD11_RG_RX_ARCA_RANK_SEL_SER_EN_CA);

	//@Darren, DLL off to stable fix middle transion from high to low or low to high at high vcore
	vIO32WriteFldMulti_All(DDRPHY_REG_SHU_CA_DLL1, P_Fld(0x0, SHU_CA_DLL1_RG_ARDLL_PHDET_EN_CA)
										| P_Fld(0x0, SHU_CA_DLL1_RG_ARDLL_PHDET_OUT_SEL_CA));
	vIO32WriteFldMulti_All(DDRPHY_REG_SHU_B0_DLL1, P_Fld(0x0, SHU_B0_DLL1_RG_ARDLL_PHDET_EN_B0)
										| P_Fld(0x0, SHU_B0_DLL1_RG_ARDLL_PHDET_OUT_SEL_B0));
	vIO32WriteFldMulti_All(DDRPHY_REG_SHU_B1_DLL1, P_Fld(0x0, SHU_B1_DLL1_RG_ARDLL_PHDET_EN_B1)
										| P_Fld(0x0, SHU_B1_DLL1_RG_ARDLL_PHDET_OUT_SEL_B1));

	//MCK4X CG
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CTRL1), 0, MISC_CTRL1_R_DMDQSIENCG_EN);
	//@A60868, DQS PI mode for JMTR
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DLL_ARPI2), 0, SHU_B0_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B0); // DQS PI mode
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DLL_ARPI2), 0, SHU_B1_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B1); // DQS PI mode
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DUTYSCAN1), 1, MISC_DUTYSCAN1_RX_EYE_SCAN_CG_EN); // enable toggle cnt
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CTRL4), 0, MISC_CTRL4_R_OPT2_CG_DQSIEN); // Remove to Golden settings for Jmeter clock
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL), 0, MISC_STBCAL_DQSIENCG_NORMAL_EN); // @Darren need confirm for DQS*_ERR_CNT, APHY PICG freerun
	//@A60868, End

	// Bypass DQS glitch-free mode
	// RG_RX_*RDQ_EYE_DLY_DQS_BYPASS_B**
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ6), 1, B0_DQ6_RG_RX_ARDQ_EYE_DLY_DQS_BYPASS_B0);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ6), 1, B1_DQ6_RG_RX_ARDQ_EYE_DLY_DQS_BYPASS_B1);

	//Enable DQ eye scan
	//RG_*_RX_EYE_SCAN_EN
	//RG_*_RX_VREF_EN
	//RG_*_RX_SMT_EN
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DUTYSCAN1), 1, MISC_DUTYSCAN1_RX_EYE_SCAN_EN);
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DUTYSCAN1), P_Fld(0x1, MISC_DUTYSCAN1_EYESCAN_DQS_SYNC_EN)
										| P_Fld(0x1, MISC_DUTYSCAN1_EYESCAN_NEW_DQ_SYNC_EN)
										| P_Fld(0x1, MISC_DUTYSCAN1_EYESCAN_DQ_SYNC_EN));
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ5), 1, B0_DQ5_RG_RX_ARDQ_EYE_EN_B0);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ5), 1, B1_DQ5_RG_RX_ARDQ_EYE_EN_B1);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ5), 1, B0_DQ5_RG_RX_ARDQ_VREF_EN_B0);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ5), 1, B1_DQ5_RG_RX_ARDQ_VREF_EN_B1);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ3), 1, B0_DQ3_RG_RX_ARDQ_SMT_EN_B0);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ3), 1, B1_DQ3_RG_RX_ARDQ_SMT_EN_B1);
	//@A60868, JMTR en
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B0_PHY2), 1, B0_PHY2_RG_RX_ARDQS_JM_EN_B0);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B1_PHY2), 1, B1_PHY2_RG_RX_ARDQS_JM_EN_B1);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_JMETER), 1, MISC_JMETER_JMTR_EN);
	//@A60868, End

	//@A60868, JM_SEL = 1, JM_SEL = 0 for LPBK
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B0_PHY2), 1, B0_PHY2_RG_RX_ARDQS_JM_SEL_B0);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B1_PHY2), 1, B1_PHY2_RG_RX_ARDQS_JM_SEL_B1);
	//@A60868, End

	//Enable MIOCK jitter meter mode ( RG_RX_MIOCK_JIT_EN=1)
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DUTYSCAN1), 1, MISC_DUTYSCAN1_RX_MIOCK_JIT_EN);

	//Disable DQ eye scan (b'1), for counter clear
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DUTYSCAN1), 0, MISC_DUTYSCAN1_RX_EYE_SCAN_EN);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DUTYSCAN1), 0, MISC_DUTYSCAN1_DQSERRCNT_DIS);

	for (u1RxGatingPI = u1RxGatingPI_start; u1RxGatingPI < u1RxGatingPI_end; u1RxGatingPI++)
	{
		mcSHOW_DBG_MSG(("\n[DramcMiockJmeter] u1RxGatingPI = %d\n", u1RxGatingPI));

		ucsearch_state = 0;
		for (u1RankIdx = RANK_0; u1RankIdx < p->support_rank_num; u1RankIdx++)
		{
			vSetRank(p, u1RankIdx);
			// SHU_RK_B0_DQSIEN_PI_DLY_DQSIEN_PI_B0[6] no use (ignore)
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B0_DQSIEN_PI_DLY), u1RxGatingPI, SHU_RK_B0_DQSIEN_PI_DLY_DQSIEN_PI_B0); // for rank*_B0
			//Darren---vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B1_DQSIEN_PI_DLY), u1RxGatingPI, SHU_RK_B1_DQSIEN_PI_DLY_DQSIEN_PI_B1); // for rank*_B0
		}
		vSetRank(p, backup_rank);

		for (ucdqs_dly = u2Jm_dly_start; ucdqs_dly < u2Jm_dly_end; ucdqs_dly += u2Jm_dly_step)
		{

			//@A60868, Set DQS delay (RG_*_RX_DQS_EYE_DLY)
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B0_PHY2), ucdqs_dly, B0_PHY2_RG_RX_ARDQS_JM_DLY_B0);
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B1_PHY2), ucdqs_dly, B1_PHY2_RG_RX_ARDQS_JM_DLY_B1);
			//@A60868, End
			DramPhyReset(p);

			//Reset eye scan counters (reg_sw_rst): 1 to 0
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DUTYSCAN1), 1, MISC_DUTYSCAN1_REG_SW_RST);
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DUTYSCAN1), 0, MISC_DUTYSCAN1_REG_SW_RST);

			//Enable DQ eye scan (b'1)
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DUTYSCAN1), 1, MISC_DUTYSCAN1_RX_EYE_SCAN_EN);

			//2ns/sample, here we delay 1ms about 500 samples
			mcDELAY_US(10);

			//Disable DQ eye scan (b'1), for counter latch
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DUTYSCAN1), 0, MISC_DUTYSCAN1_RX_EYE_SCAN_EN);

			//Read the counter values from registers (toggle_cnt*, dqs_err_cnt*);
			u4sample_cnt = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DUTY_TOGGLE_CNT), MISC_DUTY_TOGGLE_CNT_TOGGLE_CNT);
			u4ones_cnt[0] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DUTY_DQS0_ERR_CNT), MISC_DUTY_DQS0_ERR_CNT_DQS0_ERR_CNT);
			//u4ones_cnt[1] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DUTY_DQS1_ERR_CNT), MISC_DUTY_DQS1_ERR_CNT_DQS1_ERR_CNT);
			//u4ones_cnt[2] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DUTY_DQS2_ERR_CNT), MISC_DUTY_DQS2_ERR_CNT_DQS2_ERR_CNT);
			//u4ones_cnt[3] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DUTY_DQS3_ERR_CNT), MISC_DUTY_DQS3_ERR_CNT_DQS3_ERR_CNT);
			#ifdef ETT_PRINT_FORMAT
			mcSHOW_DBG_MSG(("%d : %d, %d\n", ucdqs_dly, u4sample_cnt, u4ones_cnt[0]));
			#else
			mcSHOW_DBG_MSG(("%3d : %8d, %8d\n", ucdqs_dly, u4sample_cnt, u4ones_cnt[0]));
			#endif

			//change to boolean value
			if (u4ones_cnt[0] < (u4sample_cnt / 2))
			{
				fgcurrent_value = 0;
			}
			else
			{
				fgcurrent_value = 1;
			}

			#if 1//more than 1T data
			{
				if (ucsearch_state == 0)
				{
					//record initial value at the beginning
					fginitial_value = fgcurrent_value;
					ucsearch_state = 1;
				}
				else if (ucsearch_state == 1)
				{
					// check if change value
					if (fgcurrent_value != fginitial_value)
					{
						// start of the period
						fginitial_value = fgcurrent_value;
						ucstart_period = ucdqs_dly;
						ucsearch_state = 2;
					}
				}
				else if (ucsearch_state == 2)
				{
					// check if change value
					if (fgcurrent_value != fginitial_value)
					{
						fginitial_value = fgcurrent_value;
						ucmiddle_period = ucdqs_dly;
						ucsearch_state = 3;
					}
				}
				else if (ucsearch_state == 3)
				{
					// check if change value
					if (fgcurrent_value != fginitial_value)
					{
						// end of the period, break the loop
						ucend_period = ucdqs_dly;
						ucsearch_state = 4;
						break;
					}
				}
				else
				{
					//nothing
				}
			}
			#else //only 0.5T data
			{
				if (ucsearch_state == 0)
				{
					//record initial value at the beginning
					fginitial_value = fgcurrent_value;
					ucsearch_state = 1;
				}
				else if (ucsearch_state == 1)
				{
					// check if change value
					if (fgcurrent_value != fginitial_value)
					{
						// start of the period
						fginitial_value = fgcurrent_value;
						ucstart_period = ucdqs_dly;
						ucsearch_state = 2;
					}
				}
				else if (ucsearch_state == 2)
				{
					// check if change value
					if (fgcurrent_value != fginitial_value)
					{
						// end of the period, break the loop
						ucend_period = ucdqs_dly;
						ucsearch_state = 4;
					   break;
					}
				}
			}
			#endif
		}

		if ((ucsearch_state == 4) || (ucsearch_state == 3))
			break;
	}

	//restore to orignal value
	DramcRestoreRegisters(p, u4RegBackupAddress, sizeof(u4RegBackupAddress) / sizeof(U32));

	if (ucsearch_state != 4)
	{
		if (ucsearch_state != 3)
		{
			mcSHOW_DBG_MSG(("\n\tMIOCK jitter meter - ch=%d\n", p->channel));
			mcSHOW_DBG_MSG(("\tLess than 0.5T data. Cannot calculate delay cell time\n\n"));

			u2g_num_dlycell_perT = 0;	//for LP3 and LP4 lookup table used

			return DRAM_FAIL;
		}
		else
		{
			//Calculate 1 delay cell = ? ps
			// 1T = ? delay cell
			u2g_num_dlycell_perT = (ucmiddle_period - ucstart_period) * 2;
			// 1T = ? ps
		}
	}
	else
	{
		//Calculate 1 delay cell = ? ps
		// 1T = ? delay cell
		u2g_num_dlycell_perT = (ucend_period - ucstart_period);
		// 1T = ? ps
	}

	u1ShuLevel = u4IO32ReadFldAlign(DDRPHY_REG_DVFS_STATUS, DVFS_STATUS_OTHER_SHU_GP);
	u4PLL5_ADDR = DDRPHY_REG_SHU_PHYPLL1 + DDRPHY_AO_SHU_OFFSET * u1ShuLevel;
	u4PLL8_ADDR = DDRPHY_REG_SHU_PHYPLL2 + DDRPHY_AO_SHU_OFFSET * u1ShuLevel;
	u4PLL3_ADDR = DDRPHY_REG_SHU_PHYPLL3 + DDRPHY_AO_SHU_OFFSET * u1ShuLevel;
	u4B0_DQ = DDRPHY_REG_SHU_B0_DQ1 + DDRPHY_AO_SHU_OFFSET * u1ShuLevel;
	u4SDM_PCW = u4IO32ReadFldAlign(u4PLL5_ADDR, SHU_PHYPLL1_RG_RPHYPLL_SDM_PCW);
	u4PREDIV = u4IO32ReadFldAlign(u4PLL8_ADDR, SHU_PHYPLL2_RG_RPHYPLL_PREDIV);
	u4POSDIV = u4IO32ReadFldAlign(u4PLL8_ADDR, SHU_PHYPLL2_RG_RPHYPLL_POSDIV);
	u4CKDIV4 = u4IO32ReadFldAlign(u4B0_DQ, SHU_B0_DQ1_RG_ARPI_MIDPI_CKDIV4_EN_B0);
	u1FBKSEL = u4IO32ReadFldAlign(u4PLL3_ADDR, SHU_PHYPLL3_RG_RPHYPLL_FBKSEL);
	u4VCOFreq = (((52 >> u4PREDIV) * (u4SDM_PCW >> 8)) >> u4POSDIV) << u1FBKSEL;
	u4DataRate = u4VCOFreq >> u4CKDIV4;
	u2real_freq = u4DataRate >> 1;
	u2real_period = (U16) (1000000 / u2real_freq);

	//calculate delay cell time
	u2gdelay_cell_ps = u2real_period * 100 / u2g_num_dlycell_perT;

	if (ucsearch_state == 4)
	{ // 1T
		mcSHOW_DBG_MSG(("\n\tMIOCK jitter meter\tch=%d\n\n"
						"1T = (%d-%d) = %d dly cells\n"
						"Clock freq = %d MHz, period = %d ps, 1 dly cell = %d/100 ps\n",
							p->channel,
							ucend_period, ucstart_period, u2g_num_dlycell_perT,
							u2real_freq, u2real_period, u2gdelay_cell_ps));
	}
	else
	{ // 0.5T
		mcSHOW_DBG_MSG(("\n\tMIOCK jitter meter\tch=%d\n\n"
						"1T = (%d-%d)*2 = %d dly cells\n"
						"Clock freq = %d MHz, period = %d ps, 1 dly cell = %d/100 ps\n",
							p->channel,
							ucmiddle_period, ucstart_period, u2g_num_dlycell_perT,
							u2real_freq, u2real_period, u2gdelay_cell_ps));
	}

	return DRAM_OK;

// log example
/* dly: sample_cnt	 DQS0_cnt  DQS1_cnt
	0 : 10962054,		 0, 	   0
	1 : 10958229,		 0, 	   0
	2 : 10961109,		 0, 	   0
	3 : 10946916,		 0, 	   0
	4 : 10955421,		 0, 	   0
	5 : 10967274,		 0, 	   0
	6 : 10893582,		 0, 	   0
	7 : 10974762,		 0, 	   0
	8 : 10990278,		 0, 	   0
	9 : 10972026,		 0, 	   0
   10 :  7421004,		 0, 	   0
   11 : 10943883,		 0, 	   0
   12 : 10984275,		 0, 	   0
   13 : 10955268,		 0, 	   0
   14 : 10960326,		 0, 	   0
   15 : 10952451,		 0, 	   0
   16 : 10956906,		 0, 	   0
   17 : 10960803,		 0, 	   0
   18 : 10944108,		 0, 	   0
   19 : 10959939,		 0, 	   0
   20 : 10959246,		 0, 	   0
   21 : 11002212,		 0, 	   0
   22 : 10919700,		 0, 	   0
   23 : 10977489,		 0, 	   0
   24 : 11009853,		 0, 	   0
   25 : 10991133,		 0, 	   0
   26 : 10990431,		 0, 	   0
   27 : 10970703,	 11161, 	   0
   28 : 10970775,	257118, 	   0
   29 : 10934442,  9450467, 	   0
   30 : 10970622, 10968475, 	   0
   31 : 10968831, 10968831, 	   0
   32 : 10956123, 10956123, 	   0
   33 : 10950273, 10950273, 	   0
   34 : 10975770, 10975770, 	   0
   35 : 10983024, 10983024, 	   0
   36 : 10981701, 10981701, 	   0
   37 : 10936782, 10936782, 	   0
   38 : 10889523, 10889523, 	   0
   39 : 10985913, 10985913,    55562
   40 : 10970235, 10970235,   272294
   41 : 10996056, 10996056,  9322868
   42 : 10972350, 10972350, 10969738
   43 : 10963917, 10963917, 10963917
   44 : 10967895, 10967895, 10967895
   45 : 10961739, 10961739, 10961739
   46 : 10937097, 10937097, 10937097
   47 : 10937952, 10937952, 10937952
   48 : 10926018, 10926018, 10926018
   49 : 10943793, 10943793, 10943793
   50 : 10954638, 10954638, 10954638
   51 : 10968048, 10968048, 10968048
   52 : 10944036, 10944036, 10944036
   53 : 11012112, 11012112, 11012112
   54 : 10969137, 10969137, 10969137
   55 : 10968516, 10968516, 10968516
   56 : 10952532, 10952532, 10952532
   57 : 10985832, 10985832, 10985832
   58 : 11002527, 11002527, 11002527
   59 : 10950660, 10873571, 10950660
   60 : 10949022, 10781797, 10949022
   61 : 10974366, 10700617, 10974366
   62 : 10972422,  1331974, 10972422
   63 : 10926567,		 0, 10926567
   64 : 10961658,		 0, 10961658
   65 : 10978893,		 0, 10978893
   66 : 10962828,		 0, 10962828
   67 : 10957599,		 0, 10957599
   68 : 10969227,		 0, 10969227
   69 : 10960722,		 0, 10960722
   70 : 10970937,		 0, 10963180
   71 : 10962054,		 0, 10711639
   72 : 10954719,		 0, 10612707
   73 : 10958778,		 0,   479589
   74 : 10973898,		 0, 	   0
   75 : 11004156,		 0, 	   0
   76 : 10944261,		 0, 	   0
   77 : 10955340,		 0, 	   0
   78 : 10998153,		 0, 	   0
   79 : 10998774,		 0, 	   0
   80 : 10953234,		 0, 	   0
   81 : 10960020,		 0, 	   0
   82 : 10923831,		 0, 	   0
   83 : 10951362,		 0, 	   0
   84 : 10965249,		 0, 	   0
   85 : 10949103,		 0, 	   0
   86 : 10948707,		 0, 	   0
   87 : 10941147,		 0, 	   0
   88 : 10966572,		 0, 	   0
   89 : 10971333,		 0, 	   0
   90 : 10943721,		 0, 	   0
   91 : 10949337,		 0, 	   0
   92 : 10965942,		 0, 	   0
   93 : 10970397,		 0, 	   0
   94 : 10956429,		 0, 	   0
   95 : 10939896,		 0, 	   0
   96 : 10967112,		 0, 	   0
   97 : 10951911,		 0, 	   0
   98 : 10953702,		 0, 	   0
   99 : 10971090,		 0, 	   0
  100 : 10939590,		 0, 	   0
  101 : 10993392,		 0, 	   0
  102 : 10975932,		 0, 	   0
  103 : 10949499,	 40748, 	   0
  104 : 10962522,	258638, 	   0
  105 : 10951524,	275292, 	   0
  106 : 10982475,	417642, 	   0
  107 : 10966887, 10564347, 	   0
  ===============================================================================
	  MIOCK jitter meter - channel=0
  ===============================================================================
  1T = (107-29) = 78 delay cells
  Clock frequency = 936 MHz, Clock period = 1068 ps, 1 delay cell = 13 ps
*/
}

/* "picoseconds per delay cell" depends on Vcore only (frequency doesn't matter)
 * 1. Retrieve current freq's vcore voltage using pmic API
 * 2. Perform delay cell time calculation (Bypass if shuffle vcore value is the same as before)
 */
static void GetVcoreDelayCellTime(DRAMC_CTX_T *p, U8 shuffleIdx)
{
	U32 channel_i;

#if __ETT__
#if (FOR_DV_SIMULATION_USED==0 && SW_CHANGE_FOR_SIMULATION==0)
	u4gVcore[shuffleIdx] = pmic_vcore_voltage_read();
#endif

	/* delay cell calculation is skipped if vcore is same as previous shuffle's */
	if (u4gVcore[shuffleIdx] != u4previousVcore)
	{
		u4previousVcore = u4gVcore[shuffleIdx];
		DramcMiockJmeter(p);
	}
#else
		DramcMiockJmeter(p);
#endif

	for(channel_i=CHANNEL_A; channel_i < p->support_channel_num; channel_i++)
	{
		u2g_num_dlycell_perT_all[shuffleIdx][channel_i] = u2g_num_dlycell_perT;
		u2gdelay_cell_ps_all[shuffleIdx][channel_i] = u2gdelay_cell_ps;
	}
#if __ETT__
	mcSHOW_DBG_MSG(("Freq=%d, CH_%d, VCORE=%d, cell=%d\n", p->frequency, p->channel, u4gVcore[shuffleIdx], u2gdelay_cell_ps_all[shuffleIdx][p->channel]));
#endif

	return;
}

void DramcMiockJmeterHQA(DRAMC_CTX_T *p)
{
	//do MiockJitterMeter@DDR2667
	U8 shuffleIdx;

	mcSHOW_DBG_MSG(("[MiockJmeterHQA]\n"));

	shuffleIdx = get_shuffleIndex_by_Freq(p);

	if(p->channel == CHANNEL_A)
	{
		if (p->frequency <= 600)
		{
			u2g_num_dlycell_perT_all[shuffleIdx][p->channel] = 0; // always lookup table
			u2gdelay_cell_ps_all[shuffleIdx][p->channel] = 270; // @Darren, Wait arnold for lookup table
		}
		else
			GetVcoreDelayCellTime(p, shuffleIdx);
	}

	u2gdelay_cell_ps_all[shuffleIdx][CHANNEL_B] = u2gdelay_cell_ps_all[shuffleIdx][CHANNEL_A];

#ifdef FOR_HQA_TEST_USED
	if (u2g_num_dlycell_perT_all[shuffleIdx][p->channel] == 0) GetVcoreDelayCellTimeFromTable(p); //lookup table
#endif

	/* Use highest freq's delay cell time measurement results as reference */
	p->u2num_dlycell_perT = u2g_num_dlycell_perT_all[shuffleIdx][p->channel];
	p->u2DelayCellTimex100 = u2gdelay_cell_ps_all[shuffleIdx][p->channel];
	mcSHOW_DBG_MSG3(("DelayCellTimex100 CH_%d, (VCORE=%d, cell=%d)\n",p->channel, u4gVcore[shuffleIdx], p->u2DelayCellTimex100));
}
#endif

//-------------------------------------------------------------------------
/** Dramc8PhaseCal
 *	start 8-Phase Calibration.
 *	@param p				Pointer of context created by DramcCtxCreate.
 *	@param block_no 		(U8): block 0 or 1.
 *	@retval status			(DRAM_STATUS_T): DRAM_OK or DRAM_FAIL
 */
//-------------------------------------------------------------------------

DRAM_STATUS_T Dramc8PhaseCal(DRAMC_CTX_T *p)
{
#if ENABLE_8PHASE_CALIBRATION
	U8 u1DqsLevel = 0xff, u18Ph_dly_loop_break = 0;
	U8 u1DqsienPI = 0;
	U8 u18Phase_SM = DQS_8PH_DEGREE_0, u18Ph_dly = 0, u18Ph_start = 0, u18Ph_end = 0, u18Ph_dly_final = 0xff;
	U16 u2R0 = 0xffff, u2R180 = 0xffff, u2R = 0xffff;
	U16 u2P = 0xffff, ucdqs_dly = 0;
	S16 s2Err_code = 0x7fff, s2Err_code_min = 0x7fff;
	U16 u2Jm_dly_start = 0, u2Jm_dly_end = 512, u2Jm_dly_step = 1;
	U32 u4sample_cnt, u4ones_cnt[DQS_NUMBER];
	U8 backup_rank, u1RankIdx, u18PhDlyBackup = 0;
	U8 u1loop_cnt = 0, u1early_break_cnt = 5;
	U32 u4backup_broadcast= GetDramcBroadcast();
	DRAM_STATUS_T eDRAMStatus = DRAM_OK;

#ifdef DUMP_INIT_RG_LOG_TO_DE //for FT dump 3733 dram_init.c
	return DRAM_OK;
#endif

	u1DqsienPI = 0x0;

	// error handling
	if (!p)
	{
		mcSHOW_ERR_MSG(("context NULL\n"));
		return DRAM_FAIL;
	}

	if (p->frequency < 1866)
	{
		//mcSHOW_ERR_MSG(("skip 8-Phase Calib Freq is %d < 1866 !!!\n", p->frequency));
		return DRAM_OK;
	}

	U32 u4RegBackupAddress[] =
	{
		(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DUTYSCAN1)),
		(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ6)),
		(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ6)),
		(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ5)),
		(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ5)),
		(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ3)),
		(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ3)),
		(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CTRL1)),
		(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CTRL4)),
		(DRAMC_REG_ADDR(DDRPHY_REG_B0_PHY2)),
		(DRAMC_REG_ADDR(DDRPHY_REG_B1_PHY2)),
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DLL_ARPI2)),
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DLL_ARPI2)),
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ11)),
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ11)),
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD11)),
		(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL)),
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B0_DQSIEN_PI_DLY)), // need porting to Jmeter
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B0_DQSIEN_PI_DLY + DDRPHY_AO_RANK_OFFSET)), // need porting to Jmeter
		(DRAMC_REG_ADDR(DDRPHY_REG_MISC_JMETER)),
		//(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL2)), // for gating on/off backup/restore
		//(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DVFSCTL2)), // for gating on/off backup/restore
		(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_STBCAL)), // for gating on/off backup/restore
#if 0
		(DRAMC_REG_ADDR(DDRPHY_REG_B0_DLL_ARPI0)),
		(DRAMC_REG_ADDR(DDRPHY_REG_B1_DLL_ARPI0)),
		(DRAMC_REG_ADDR(DDRPHY_REG_CA_DLL_ARPI0)),
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ6)),
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ6)),
		(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD6)),
#endif
		((DDRPHY_REG_SHU_CA_DLL1)),
		((DDRPHY_REG_SHU_B0_DLL1)),
		((DDRPHY_REG_SHU_B1_DLL1)),
		((DDRPHY_REG_B0_DQ2)),
		((DDRPHY_REG_B1_DQ2)),
		((DDRPHY_REG_CA_CMD2)),
		((DDRPHY_REG_SHU_B0_DQ13)),
		((DDRPHY_REG_SHU_B1_DQ13)),
		((DDRPHY_REG_SHU_CA_CMD13)),

		((DDRPHY_REG_SHU_CA_DLL1) + SHIFT_TO_CHB_ADDR),
		((DDRPHY_REG_SHU_B0_DLL1) + SHIFT_TO_CHB_ADDR),
		((DDRPHY_REG_SHU_B1_DLL1) + SHIFT_TO_CHB_ADDR),
		((DDRPHY_REG_B0_DQ2) + SHIFT_TO_CHB_ADDR),
		((DDRPHY_REG_B1_DQ2) + SHIFT_TO_CHB_ADDR),
		((DDRPHY_REG_CA_CMD2) + SHIFT_TO_CHB_ADDR),
		((DDRPHY_REG_SHU_B0_DQ13) + SHIFT_TO_CHB_ADDR),
		((DDRPHY_REG_SHU_B1_DQ13) + SHIFT_TO_CHB_ADDR),
		((DDRPHY_REG_SHU_CA_CMD13) + SHIFT_TO_CHB_ADDR),
#if (CHANNEL_NUM > 2)
		((DDRPHY_REG_SHU_CA_DLL1) + SHIFT_TO_CHC_ADDR),
		((DDRPHY_REG_SHU_B0_DLL1) + SHIFT_TO_CHC_ADDR),
		((DDRPHY_REG_SHU_B1_DLL1) + SHIFT_TO_CHC_ADDR),
		((DDRPHY_REG_B0_DQ2) + SHIFT_TO_CHC_ADDR),
		((DDRPHY_REG_B1_DQ2) + SHIFT_TO_CHC_ADDR),
		((DDRPHY_REG_CA_CMD2) + SHIFT_TO_CHC_ADDR),
		((DDRPHY_REG_SHU_B0_DQ13) + SHIFT_TO_CHC_ADDR),
		((DDRPHY_REG_SHU_B1_DQ13) + SHIFT_TO_CHC_ADDR),
		((DDRPHY_REG_SHU_CA_CMD13) + SHIFT_TO_CHC_ADDR),

		((DDRPHY_REG_SHU_CA_DLL1) + SHIFT_TO_CHD_ADDR),
		((DDRPHY_REG_SHU_B0_DLL1) + SHIFT_TO_CHD_ADDR),
		((DDRPHY_REG_SHU_B1_DLL1) + SHIFT_TO_CHD_ADDR),
		((DDRPHY_REG_B0_DQ2) + SHIFT_TO_CHD_ADDR),
		((DDRPHY_REG_B1_DQ2) + SHIFT_TO_CHD_ADDR),
		((DDRPHY_REG_CA_CMD2) + SHIFT_TO_CHD_ADDR),
		((DDRPHY_REG_SHU_B0_DQ13) + SHIFT_TO_CHD_ADDR),
		((DDRPHY_REG_SHU_B1_DQ13) + SHIFT_TO_CHD_ADDR),
		((DDRPHY_REG_SHU_CA_CMD13) + SHIFT_TO_CHD_ADDR),
#endif
	};

	backup_rank = u1GetRank(p);
	DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);

	//backup register value
	DramcBackupRegisters(p, u4RegBackupAddress, sizeof(u4RegBackupAddress) / sizeof(U32));

	//OE disable - start
	vIO32WriteFldMulti_All(DDRPHY_REG_B0_DQ2, P_Fld( 0 , B0_DQ2_RG_TX_ARDQS_OE_TIE_SEL_B0 ) \
														  | P_Fld( 1	   , B0_DQ2_RG_TX_ARDQS_OE_TIE_EN_B0  ) \
														  | P_Fld( 0 , B0_DQ2_RG_TX_ARWCK_OE_TIE_SEL_B0 ) \
														  | P_Fld( 1	   , B0_DQ2_RG_TX_ARWCK_OE_TIE_EN_B0  ) \
														  | P_Fld( 0 , B0_DQ2_RG_TX_ARWCKB_OE_TIE_SEL_B0 ) \
														  | P_Fld( 1	   , B0_DQ2_RG_TX_ARWCKB_OE_TIE_EN_B0  ) \
														  | P_Fld( 0 , B0_DQ2_RG_TX_ARDQM_OE_TIE_SEL_B0 ) \
														  | P_Fld( 1	   , B0_DQ2_RG_TX_ARDQM_OE_TIE_EN_B0  ) \
														  | P_Fld( 0 , B0_DQ2_RG_TX_ARDQ_OE_TIE_SEL_B0 ) \
														  | P_Fld( 0xff 	  , B0_DQ2_RG_TX_ARDQ_OE_TIE_EN_B0	) );

	vIO32WriteFldMulti_All(DDRPHY_REG_B1_DQ2, P_Fld( 0 , B1_DQ2_RG_TX_ARDQS_OE_TIE_SEL_B1 ) \
														  | P_Fld( 1	   , B1_DQ2_RG_TX_ARDQS_OE_TIE_EN_B1  ) \
														  | P_Fld( 0 , B1_DQ2_RG_TX_ARWCK_OE_TIE_SEL_B1 ) \
														  | P_Fld( 1	   , B1_DQ2_RG_TX_ARWCK_OE_TIE_EN_B1  ) \
														  | P_Fld( 0 , B1_DQ2_RG_TX_ARWCKB_OE_TIE_SEL_B1 ) \
														  | P_Fld( 1	   , B1_DQ2_RG_TX_ARWCKB_OE_TIE_EN_B1  ) \
														  | P_Fld( 0 , B1_DQ2_RG_TX_ARDQM_OE_TIE_SEL_B1 ) \
														  | P_Fld( 1	   , B1_DQ2_RG_TX_ARDQM_OE_TIE_EN_B1  ) \
														  | P_Fld( 0 , B1_DQ2_RG_TX_ARDQ_OE_TIE_SEL_B1 ) \
														  | P_Fld( 0xff 	  , B1_DQ2_RG_TX_ARDQ_OE_TIE_EN_B1	) );

	vIO32WriteFldMulti_All(DDRPHY_REG_CA_CMD2, P_Fld( 0 , CA_CMD2_RG_TX_ARCLK_OE_TIE_SEL_CA ) \
														  | P_Fld( 1	   , CA_CMD2_RG_TX_ARCLK_OE_TIE_EN_CA  ) \
														  | P_Fld( 0 , CA_CMD2_RG_TX_ARCS_OE_TIE_SEL_CA ) \
														  | P_Fld( 1	   , CA_CMD2_RG_TX_ARCS_OE_TIE_EN_CA  ) \
														  | P_Fld( 0 , CA_CMD2_RG_TX_ARCA_OE_TIE_SEL_CA ) \
														  | P_Fld( 0xff 	  , CA_CMD2_RG_TX_ARCA_OE_TIE_EN_CA  ) );

	vIO32WriteFldMulti_All(DDRPHY_REG_SHU_B0_DQ13 , P_Fld( 0 , SHU_B0_DQ13_RG_TX_ARDQSB_OE_TIE_SEL_B0  ) \
															  | P_Fld( 1	   , SHU_B0_DQ13_RG_TX_ARDQSB_OE_TIE_EN_B0	 ));
	vIO32WriteFldMulti_All(DDRPHY_REG_SHU_B1_DQ13 , P_Fld( 0 , SHU_B1_DQ13_RG_TX_ARDQSB_OE_TIE_SEL_B1  ) \
															  | P_Fld( 1	   , SHU_B1_DQ13_RG_TX_ARDQSB_OE_TIE_EN_B1	 ));
	vIO32WriteFldMulti_All(DDRPHY_REG_SHU_CA_CMD13, P_Fld( 0 , SHU_CA_CMD13_RG_TX_ARCLKB_OE_TIE_SEL_CA ) \
															  | P_Fld( 1	   , SHU_CA_CMD13_RG_TX_ARCLKB_OE_TIE_EN_CA  ));
	//OE disable - end

	u18PhDlyBackup = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ1), SHU_B0_DQ1_RG_ARPI_MIDPI_8PH_DLY_B0);

	//DramcHWGatingOnOff(p, 0); // disable Gating tracking for DQS PI, Remove to vApplyConfigBeforeCalibration
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_STBCAL), P_Fld(0x0, MISC_SHU_STBCAL_STBCALEN)
										| P_Fld(0x0, MISC_SHU_STBCAL_STB_SELPHCALEN));

#if 0 // 8-Phase calib must to do before DLL init for test only
	//@A60868, Reset PI code to avoid 8-phase offset
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B0_DLL_ARPI0), 0, B0_DLL_ARPI0_RG_ARPI_RESETB_B0);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B1_DLL_ARPI0), 0, B1_DLL_ARPI0_RG_ARPI_RESETB_B1);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_CA_DLL_ARPI0), 0, CA_DLL_ARPI0_RG_ARPI_RESETB_CA);
	mcDELAY_US(1);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B0_DLL_ARPI0), 1, B0_DLL_ARPI0_RG_ARPI_RESETB_B0);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B1_DLL_ARPI0), 1, B1_DLL_ARPI0_RG_ARPI_RESETB_B1);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_CA_DLL_ARPI0), 1, CA_DLL_ARPI0_RG_ARPI_RESETB_CA);
	//@A60868, End

	// @A60868, DQSIEN PI offset clear to 0
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ6), 0, SHU_B0_DQ6_RG_ARPI_OFFSET_DQSIEN_B0);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ6), 0, SHU_B1_DQ6_RG_ARPI_OFFSET_DQSIEN_B1);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD6), 0, SHU_CA_CMD6_RG_ARPI_OFFSET_DQSIEN_CA);
#endif

	// @A60868 for *RANK_SEL_SER_EN* = 0 to DA_RX_ARDQ_RANK_SEL_TXD_*[0]
	//				for *RANK_SEL_SER_EN* = 1 to DA_RX_ARDQ_RANK_SEL_TXD_*[7:0]
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ11), 0, SHU_B0_DQ11_RG_RX_ARDQ_RANK_SEL_SER_EN_B0);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ11), 0, SHU_B1_DQ11_RG_RX_ARDQ_RANK_SEL_SER_EN_B1);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD11), 0, SHU_CA_CMD11_RG_RX_ARCA_RANK_SEL_SER_EN_CA);

	//@Darren, DLL off to stable fix middle transion from high to low or low to high at high vcore
	vIO32WriteFldMulti_All(DDRPHY_REG_SHU_CA_DLL1, P_Fld(0x0, SHU_CA_DLL1_RG_ARDLL_PHDET_EN_CA)
										| P_Fld(0x0, SHU_CA_DLL1_RG_ARDLL_PHDET_OUT_SEL_CA));
	vIO32WriteFldMulti_All(DDRPHY_REG_SHU_B0_DLL1, P_Fld(0x0, SHU_B0_DLL1_RG_ARDLL_PHDET_EN_B0)
										| P_Fld(0x0, SHU_B0_DLL1_RG_ARDLL_PHDET_OUT_SEL_B0));
	vIO32WriteFldMulti_All(DDRPHY_REG_SHU_B1_DLL1, P_Fld(0x0, SHU_B1_DLL1_RG_ARDLL_PHDET_EN_B1)
										| P_Fld(0x0, SHU_B1_DLL1_RG_ARDLL_PHDET_OUT_SEL_B1));

	//MCK4X CG
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CTRL1), 0, MISC_CTRL1_R_DMDQSIENCG_EN);
	//@A60868, DQS PI mode for JMTR
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DLL_ARPI2), 0, SHU_B0_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B0); // DQS PI mode
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DLL_ARPI2), 0, SHU_B1_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B1); // DQS PI mode
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DUTYSCAN1), 1, MISC_DUTYSCAN1_RX_EYE_SCAN_CG_EN); // enable toggle cnt
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CTRL4), 0, MISC_CTRL4_R_OPT2_CG_DQSIEN); // Remove to Golden settings for Jmeter clock
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL), 0, MISC_STBCAL_DQSIENCG_NORMAL_EN); // for DQS*_ERR_CNT
	//@A60868, End

	// Bypass DQS glitch-free mode
	// RG_RX_*RDQ_EYE_DLY_DQS_BYPASS_B**
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ6), 1, B0_DQ6_RG_RX_ARDQ_EYE_DLY_DQS_BYPASS_B0);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ6), 1, B1_DQ6_RG_RX_ARDQ_EYE_DLY_DQS_BYPASS_B1);

	//Enable DQ eye scan
	//RG_*_RX_EYE_SCAN_EN
	//RG_*_RX_VREF_EN
	//RG_*_RX_SMT_EN
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DUTYSCAN1), 1, MISC_DUTYSCAN1_RX_EYE_SCAN_EN);
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DUTYSCAN1), P_Fld(0x1, MISC_DUTYSCAN1_EYESCAN_DQS_SYNC_EN)
										| P_Fld(0x1, MISC_DUTYSCAN1_EYESCAN_NEW_DQ_SYNC_EN)
										| P_Fld(0x1, MISC_DUTYSCAN1_EYESCAN_DQ_SYNC_EN));
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ5), 1, B0_DQ5_RG_RX_ARDQ_EYE_EN_B0);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ5), 1, B1_DQ5_RG_RX_ARDQ_EYE_EN_B1);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ5), 1, B0_DQ5_RG_RX_ARDQ_VREF_EN_B0);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ5), 1, B1_DQ5_RG_RX_ARDQ_VREF_EN_B1);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ3), 1, B0_DQ3_RG_RX_ARDQ_SMT_EN_B0);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ3), 1, B1_DQ3_RG_RX_ARDQ_SMT_EN_B1);
	//@A60868, JMTR en
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B0_PHY2), 1, B0_PHY2_RG_RX_ARDQS_JM_EN_B0);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B1_PHY2), 1, B1_PHY2_RG_RX_ARDQS_JM_EN_B1);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_JMETER), 1, MISC_JMETER_JMTR_EN);
	//@A60868, End

	//@A60868, JM_SEL = 1, JM_SEL = 0 for LPBK
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B0_PHY2), 1, B0_PHY2_RG_RX_ARDQS_JM_SEL_B0);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B1_PHY2), 1, B1_PHY2_RG_RX_ARDQS_JM_SEL_B1);

	//Enable MIOCK jitter meter mode ( RG_RX_MIOCK_JIT_EN=1)
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DUTYSCAN1), 1, MISC_DUTYSCAN1_RX_MIOCK_JIT_EN);

	//Disable DQ eye scan (b'1), for counter clear
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DUTYSCAN1), 0, MISC_DUTYSCAN1_RX_EYE_SCAN_EN);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DUTYSCAN1), 0, MISC_DUTYSCAN1_DQSERRCNT_DIS);

	for (u18Phase_SM = DQS_8PH_DEGREE_0; u18Phase_SM < DQS_8PH_DEGREE_MAX; u18Phase_SM++)
	{
		switch (u18Phase_SM)
		{
			case DQS_8PH_DEGREE_0:
				u1DqsienPI = 16;
				u18Ph_start = 0;
				u18Ph_end = 1;
				break;
			case DQS_8PH_DEGREE_180:
				u1DqsienPI = 48;
				u18Ph_start = 0;
				u18Ph_end = 1;
				break;
			case DQS_8PH_DEGREE_45:
				u1DqsienPI = 24;
				u18Ph_start = 0;
				u18Ph_end = 32;
				break;
			default:
				mcSHOW_ERR_MSG(("u18Phase_SM err!\n"));
				#if __ETT__
				while (1);
				#endif
		}

		mcSHOW_DBG_MSG(("\n[Dramc8PhaseCal] 8-Phase SM_%d, 8PH_dly (%d~%d), DQSIEN PI = %d, 8PH_Dly = %d\n", u18Phase_SM, u18Ph_start, u18Ph_end, u1DqsienPI, u18PhDlyBackup));

		//to see 1T(H,L) or 1T(L,H) from delaycell=0 to 127
		//NOTE: Must set dual ranks for Rx path
		for (u1RankIdx = RANK_0; u1RankIdx < p->support_rank_num; u1RankIdx++)
		{
			vSetRank(p, u1RankIdx);
			// SHU_RK_B0_DQSIEN_PI_DLY_DQSIEN_PI_B0[6] no use (ignore)
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B0_DQSIEN_PI_DLY), u1DqsienPI, SHU_RK_B0_DQSIEN_PI_DLY_DQSIEN_PI_B0); // for rank*_B0
		}
		vSetRank(p, backup_rank);

		for (u18Ph_dly = u18Ph_start; u18Ph_dly < u18Ph_end; u18Ph_dly++)
		{
			mcSHOW_DBG_MSG(("8PH dly = %d\n", u18Ph_dly));

			u1DqsLevel = 0xff;
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ1), u18Ph_dly, SHU_B0_DQ1_RG_ARPI_MIDPI_8PH_DLY_B0);
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ1), u18Ph_dly, SHU_B1_DQ1_RG_ARPI_MIDPI_8PH_DLY_B1);
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD1), u18Ph_dly, SHU_CA_CMD1_RG_ARPI_MIDPI_8PH_DLY_CA);

			for (ucdqs_dly = u2Jm_dly_start; ucdqs_dly < u2Jm_dly_end; ucdqs_dly += u2Jm_dly_step)
			{
				//Set DQS delay (RG_*_RX_DQS_EYE_DLY)
				vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B0_PHY2), ucdqs_dly, B0_PHY2_RG_RX_ARDQS_JM_DLY_B0);
				vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B1_PHY2), ucdqs_dly, B1_PHY2_RG_RX_ARDQS_JM_DLY_B1);
				DramPhyReset(p);

				//Reset eye scan counters (reg_sw_rst): 1 to 0
				vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DUTYSCAN1), 1, MISC_DUTYSCAN1_REG_SW_RST);
				vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DUTYSCAN1), 0, MISC_DUTYSCAN1_REG_SW_RST);

				//Enable DQ eye scan (b'1)
				vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DUTYSCAN1), 1, MISC_DUTYSCAN1_RX_EYE_SCAN_EN);

				//2ns/sample, here we delay 1ms about 500 samples
				mcDELAY_US(10);

				//Disable DQ eye scan (b'1), for counter latch
				vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DUTYSCAN1), 0, MISC_DUTYSCAN1_RX_EYE_SCAN_EN);

				//Read the counter values from registers (toggle_cnt*, dqs_err_cnt*);
				u4sample_cnt = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DUTY_TOGGLE_CNT), MISC_DUTY_TOGGLE_CNT_TOGGLE_CNT);
				u4ones_cnt[0] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DUTY_DQS0_ERR_CNT), MISC_DUTY_DQS0_ERR_CNT_DQS0_ERR_CNT);
				//u4ones_cnt[1] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DUTY_DQS1_ERR_CNT), MISC_DUTY_DQS1_ERR_CNT_DQS1_ERR_CNT);
				//u4ones_cnt[2] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DUTY_DQS2_ERR_CNT), MISC_DUTY_DQS2_ERR_CNT_DQS2_ERR_CNT);
				//u4ones_cnt[3] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DUTY_DQS3_ERR_CNT), MISC_DUTY_DQS3_ERR_CNT_DQS3_ERR_CNT);
				//Darren-mcSHOW_DBG_MSG(("%3d : %8d, %8d, %8d\n", ucdqs_dly, u4sample_cnt, u4ones_cnt[0], u4ones_cnt[1]));

				//change to boolean value
				if (u4ones_cnt[0] < (u4sample_cnt / 2))
				{
					if (u1DqsLevel == 0xff) // print once
					{
						mcSHOW_DBG_MSG(("[L] %d, %8d\n", ucdqs_dly, u4ones_cnt[0]));
						//mcSHOW_DBG_MSG(("[L] %d, %8d, %8d\n", ucdqs_dly, u4ones_cnt[0], u4ones_cnt[1]));
					}

					u1DqsLevel = 0;
				}
				else
				{
					if (u1DqsLevel == 0)  // from low to high
					{
						u1DqsLevel = 1;
						mcSHOW_DBG_MSG(("[H] %d, %8d\n", ucdqs_dly, u4ones_cnt[0]));
						//mcSHOW_DBG_MSG(("[H] %d, %8d, %8d\n", ucdqs_dly, u4ones_cnt[0], u4ones_cnt[1]));

						if (u18Phase_SM == DQS_8PH_DEGREE_0)
						{
							u2R0 = ucdqs_dly;
							mcSHOW_DBG_MSG(("R0 = %d\n", u2R0));
							break; // break ucdqs_dly for loop
						}
						else if (u18Phase_SM == DQS_8PH_DEGREE_180)
						{
							u2R180 = ucdqs_dly;
							if (u2R180 > u2R0)
							{
								u2R = u2R0 + ((u2R180 - u2R0) >> 2); // u2R180 >= u2R0 for (u1R180 - u1R0)/4 for 180 degree. /2 for 90 degree
								mcSHOW_DBG_MSG(("R = %d, R180 = %d\n", u2R, u2R180));
								break; // break ucdqs_dly for loop
							}
							else
							{
								u1DqsLevel = 0xff; //next u2Jm_dly to find edge (L->H)
							}
						}
						else if (u18Phase_SM == DQS_8PH_DEGREE_45)
						{
							u2P = ucdqs_dly;
							if (u2P > u2R0) // u2P ~= DQS_8PH_DEGREE_180
							{
								// Absolute to find min diff
								if (u2R > u2P)
									s2Err_code = u2R - u2P;
								else
									s2Err_code = u2P - u2R;

								if (s2Err_code == 0)
								{
									u18Ph_dly_final = u18Ph_dly;
									u18Ph_dly_loop_break = 1;
								}
								else if (s2Err_code < s2Err_code_min)
								{
									s2Err_code_min = s2Err_code;
									u18Ph_dly_final = u18Ph_dly;
									u1loop_cnt = 0;
								}
								else if (s2Err_code >= s2Err_code_min)
								{
									// check early break for u18Ph_dly for loop
									u1loop_cnt++;
									if (u1loop_cnt > u1early_break_cnt)
									{
										u18Ph_dly_loop_break = 1;
									}
								}

								mcSHOW_DBG_MSG(("diff (P-R) = %d, min = %d, break count = %d\n", s2Err_code, s2Err_code_min, u1loop_cnt));

								break; // if (s2Err_code == s2Err_code_min) for next u18Ph_dly
							}
							else
							{
								u1DqsLevel = 0xff; //next u2Jm_dly to find edge (L->H)
							}
						}
						else
						{
							mcSHOW_ERR_MSG(("u18Phase_SM err!\n"));
							#if __ETT__
							while (1);
							#endif
						}
					}
				}

			}

			// Error handing
			if ((u1DqsLevel == 0xff) || (u1DqsLevel == 0))
			{
				// (u1DqsLevel == 0) => skip from 1 to all's 0 or all's 0
				// (u1DqsLevel == 0xff) => skip all's 1
				// NOTE: 8-Phase calib must from 0 to 1
				u18Ph_dly_final = u18PhDlyBackup; //rollback to init settings
				eDRAMStatus = DRAM_FAIL;
				mcSHOW_ERR_MSG(("\n[Dramc8PhaseCal] 8-Phase SM_%d is fail (to Default)!!!\n", u18Phase_SM));
				goto exit;
			} else if (u18Ph_dly_loop_break == 1)
				break;

		}
	}

exit:
	mcSHOW_DBG_MSG(("\n[Dramc8PhaseCal] u18Ph_dly_final = %d\n\n", u18Ph_dly_final));
	vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B0_DQ1, u18Ph_dly_final, SHU_B0_DQ1_RG_ARPI_MIDPI_8PH_DLY_B0);
	vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B1_DQ1, u18Ph_dly_final, SHU_B1_DQ1_RG_ARPI_MIDPI_8PH_DLY_B1);
	vIO32WriteFldAlign_All(DDRPHY_REG_SHU_CA_CMD1, u18Ph_dly_final, SHU_CA_CMD1_RG_ARPI_MIDPI_8PH_DLY_CA);

	//restore to orignal value
	DramcRestoreRegisters(p, u4RegBackupAddress, sizeof(u4RegBackupAddress) / sizeof(U32));
	DramcBroadcastOnOff(u4backup_broadcast);

	return eDRAMStatus;
#endif
}

#if SIMULATION_SW_IMPED
void DramcSwImpedanceSaveRegister(DRAMC_CTX_T *p, U8 ca_freq_option, U8 dq_freq_option, U8 save_to_where)
{
	U32 backup_broadcast;

	backup_broadcast = GetDramcBroadcast();

	DramcBroadcastOnOff(DRAMC_BROADCAST_ON);
	//DQ
	vIO32WriteFldMulti((DDRPHY_REG_SHU_MISC_DRVING1 + save_to_where * SHU_GRP_DDRPHY_OFFSET), P_Fld(gDramcSwImpedanceResult[dq_freq_option][DRVP], SHU_MISC_DRVING1_DQDRVP2) | P_Fld(gDramcSwImpedanceResult[dq_freq_option][DRVN], SHU_MISC_DRVING1_DQDRVN2));
	vIO32WriteFldMulti((DDRPHY_REG_SHU_MISC_DRVING2 + save_to_where * SHU_GRP_DDRPHY_OFFSET), P_Fld(gDramcSwImpedanceResult[dq_freq_option][DRVP], SHU_MISC_DRVING2_DQDRVP1) | P_Fld(gDramcSwImpedanceResult[dq_freq_option][DRVN], SHU_MISC_DRVING2_DQDRVN1));
	vIO32WriteFldMulti((DDRPHY_REG_SHU_MISC_DRVING3 + save_to_where * SHU_GRP_DDRPHY_OFFSET), P_Fld(gDramcSwImpedanceResult[dq_freq_option][ODTP], SHU_MISC_DRVING3_DQODTP2) | P_Fld(gDramcSwImpedanceResult[dq_freq_option][ODTN], SHU_MISC_DRVING3_DQODTN2));
	vIO32WriteFldMulti((DDRPHY_REG_SHU_MISC_DRVING4 + save_to_where * SHU_GRP_DDRPHY_OFFSET), P_Fld(gDramcSwImpedanceResult[dq_freq_option][ODTP], SHU_MISC_DRVING4_DQODTP1) | P_Fld(gDramcSwImpedanceResult[dq_freq_option][ODTN], SHU_MISC_DRVING4_DQODTN1));

	//DQS
	#if SUPPORT_HYNIX_RX_DQS_WEAK_PULL
	if (p->vendor_id == VENDOR_HYNIX)
	{  U32 temp_value[4];
		int i;
		for(i=0; i<4; i++)
		{
			temp_value[i] = SwImpedanceAdjust(gDramcSwImpedanceResult[dq_freq_option][i], 2);
		}
		 vIO32WriteFldMulti((DDRPHY_REG_SHU_MISC_DRVING1 + save_to_where * SHU_GRP_DDRPHY_OFFSET), P_Fld(temp_value[0], SHU_MISC_DRVING1_DQSDRVP2) | P_Fld(temp_value[1], SHU_MISC_DRVING1_DQSDRVN2));
		 vIO32WriteFldMulti((DDRPHY_REG_SHU_MISC_DRVING1 + save_to_where * SHU_GRP_DDRPHY_OFFSET), P_Fld(temp_value[0], SHU_MISC_DRVING1_DQSDRVP1) | P_Fld(temp_value[1], SHU_MISC_DRVING1_DQSDRVN1));
		 vIO32WriteFldMulti((DDRPHY_REG_SHU_MISC_DRVING3 + save_to_where * SHU_GRP_DDRPHY_OFFSET), P_Fld(temp_value[2], SHU_MISC_DRVING3_DQSODTP2) | P_Fld(temp_value[3], SHU_MISC_DRVING3_DQSODTN2));
		 vIO32WriteFldMulti((DDRPHY_REG_SHU_MISC_DRVING3 + save_to_where * SHU_GRP_DDRPHY_OFFSET), P_Fld(temp_value[2], SHU_MISC_DRVING3_DQSODTP) | P_Fld(temp_value[3], SHU_MISC_DRVING3_DQSODTN));
	}
	else
	#endif
	{
		vIO32WriteFldMulti((DDRPHY_REG_SHU_MISC_DRVING1 + save_to_where * SHU_GRP_DDRPHY_OFFSET), P_Fld(gDramcSwImpedanceResult[dq_freq_option][DRVP], SHU_MISC_DRVING1_DQSDRVP2) | P_Fld(gDramcSwImpedanceResult[dq_freq_option][DRVN], SHU_MISC_DRVING1_DQSDRVN2));
		vIO32WriteFldMulti((DDRPHY_REG_SHU_MISC_DRVING1 + save_to_where * SHU_GRP_DDRPHY_OFFSET), P_Fld(gDramcSwImpedanceResult[dq_freq_option][DRVP], SHU_MISC_DRVING1_DQSDRVP1) | P_Fld(gDramcSwImpedanceResult[dq_freq_option][DRVN], SHU_MISC_DRVING1_DQSDRVN1));
		vIO32WriteFldMulti((DDRPHY_REG_SHU_MISC_DRVING3 + save_to_where * SHU_GRP_DDRPHY_OFFSET), P_Fld(gDramcSwImpedanceResult[dq_freq_option][ODTP], SHU_MISC_DRVING3_DQSODTP2) | P_Fld(gDramcSwImpedanceResult[dq_freq_option][ODTN], SHU_MISC_DRVING3_DQSODTN2));
		vIO32WriteFldMulti((DDRPHY_REG_SHU_MISC_DRVING3 + save_to_where * SHU_GRP_DDRPHY_OFFSET), P_Fld(gDramcSwImpedanceResult[dq_freq_option][ODTP], SHU_MISC_DRVING3_DQSODTP) | P_Fld(gDramcSwImpedanceResult[dq_freq_option][ODTN], SHU_MISC_DRVING3_DQSODTN));
	}

	//CMD & CLK
	vIO32WriteFldMulti((DDRPHY_REG_SHU_MISC_DRVING2 + save_to_where * SHU_GRP_DDRPHY_OFFSET), P_Fld(gDramcSwImpedanceResult[ca_freq_option][DRVP], SHU_MISC_DRVING2_CMDDRVP2) | P_Fld(gDramcSwImpedanceResult[ca_freq_option][DRVN], SHU_MISC_DRVING2_CMDDRVN2));
	vIO32WriteFldMulti((DDRPHY_REG_SHU_MISC_DRVING2 + save_to_where * SHU_GRP_DDRPHY_OFFSET), P_Fld(gDramcSwImpedanceResult[ca_freq_option][DRVP], SHU_MISC_DRVING2_CMDDRVP1) | P_Fld(gDramcSwImpedanceResult[ca_freq_option][DRVN], SHU_MISC_DRVING2_CMDDRVN1));
	vIO32WriteFldMulti((DDRPHY_REG_SHU_MISC_DRVING4 + save_to_where * SHU_GRP_DDRPHY_OFFSET), P_Fld(gDramcSwImpedanceResult[ca_freq_option][ODTP], SHU_MISC_DRVING4_CMDODTP2) | P_Fld(gDramcSwImpedanceResult[ca_freq_option][ODTN], SHU_MISC_DRVING4_CMDODTN2));
	vIO32WriteFldMulti((DDRPHY_REG_SHU_MISC_DRVING4 + save_to_where * SHU_GRP_DDRPHY_OFFSET), P_Fld(gDramcSwImpedanceResult[ca_freq_option][ODTP], SHU_MISC_DRVING4_CMDODTP1) | P_Fld(gDramcSwImpedanceResult[ca_freq_option][ODTN], SHU_MISC_DRVING4_CMDODTN1));

	//RG_TX_*RCKE_DRVP/RG_TX_*RCKE_DRVN doesn't set, so set 0xA first
	//@Maoauo confirm, RG no function
	//vIO32WriteFldAlign((DDRPHY_SHU_CA_CMD11 + save_to_where * SHU_GRP_DDRPHY_OFFSET), gDramcSwImpedanceResult[ca_freq_option][DRVP], SHU_CA_CMD11_RG_TX_ARCKE_DRVP);
	//vIO32WriteFldAlign((DDRPHY_SHU_CA_CMD11 + save_to_where * SHU_GRP_DDRPHY_OFFSET), gDramcSwImpedanceResult[ca_freq_option][DRVN], SHU_CA_CMD11_RG_TX_ARCKE_DRVN);

	//CKE
	// CKE is full swing.
	// LP4/LP4X set DRVP/DRVN as LP3's default value
	// DRVP=8 -> 0xA for 868 by Alucary Chen
	// DRVN=9 -> 0xA for 868 by Alucary Chen
	//DRVP[4:0] = RG_TX_ARCMD_PU_PRE<1:0>, RG_TX_ARCLK_DRVN_PRE<2:0> for La_fite only
	//@Darren-vIO32WriteFldAlign((DDRPHY_REG_SHU_CA_CMD3 + save_to_where * SHU_GRP_DDRPHY_OFFSET), (8>>3)&0x3, SHU_CA_CMD3_RG_TX_ARCMD_PU_PRE); //Darren need confirm
	//@Darren-vIO32WriteFldAlign((DDRPHY_REG_SHU_CA_CMD0 + save_to_where * SHU_GRP_DDRPHY_OFFSET), 8&0x7, SHU_CA_CMD0_RG_TX_ARCLK_DRVN_PRE); //Darren need confirm
	//DRVN[4:0] = RG_ARCMD_REV<12:8>
	//@Darren-vIO32WriteFldAlign_All((DDRPHY_SHU_CA_DLL2 + save_to_where * SHU_GRP_DDRPHY_OFFSET), 9, SHU_CA_DLL2_RG_TX_ARCKE_DRVN_B0);
	#if (fcFOR_CHIP_ID == fcA60868) // for 868 CS and CKE control together
	vIO32WriteFldAlign((DDRPHY_REG_MISC_SHU_DRVING8 + save_to_where * SHU_GRP_DDRPHY_OFFSET), 0xA, MISC_SHU_DRVING8_CS_DRVP);
	vIO32WriteFldAlign((DDRPHY_REG_MISC_SHU_DRVING8 + save_to_where * SHU_GRP_DDRPHY_OFFSET), 0xA, MISC_SHU_DRVING8_CS_DRVN);
	#elif (fcFOR_CHIP_ID == fcMargaux)
	// @Darren, confirm with ACD Alucary,
	// MISC_SHU_DRVING8_CS_DRVP & MISC_SHU_DRVING8_CS_DRVN -> DA_TX_ARCKE_DRVP_C0[4:0]	 & DA_TX_ARCKE_DRVN_C0[4:0]
	vIO32WriteFldAlign((DDRPHY_REG_MISC_SHU_DRVING8 + save_to_where * SHU_GRP_DDRPHY_OFFSET), 0xF, MISC_SHU_DRVING8_CS_DRVP);
	vIO32WriteFldAlign((DDRPHY_REG_MISC_SHU_DRVING8 + save_to_where * SHU_GRP_DDRPHY_OFFSET), 0x14, MISC_SHU_DRVING8_CS_DRVN);
	#endif

	DramcBroadcastOnOff(backup_broadcast);
}

//-------------------------------------------------------------------------
/** vImpCalVrefSel
 *	Set IMP_VREF_SEL for DRVP, DRVN, Run-time/Tracking
 *	(Refer to "IMPCAL Settings" document register "RG_RIMP_VREF_SEL" settings)
 *	@param p				Pointer of context created by DramcCtxCreate.
 *	@param	freq_region 	(enum): pass freq_region (IMP_LOW_FREQ/IMP_HIGH_FREQ) for LP4X
 *	@param	u1ImpCalStage	(U8): During DRVP, DRVN, run-time/tracking stages
 *								  some vref_sel values are different
 */
//-------------------------------------------------------------------------
/* Definitions to make IMPCAL_VREF_SEL function more readable */
#define IMPCAL_STAGE_DRVP	  0
#define IMPCAL_STAGE_DRVN	  1
#define IMPCAL_STAGE_ODTP	  2
#define IMPCAL_STAGE_ODTN	  3
#define IMPCAL_STAGE_TRACKING 4

/* LP4X IMP_VREF_SEL w/o term ==== */
#define IMP_TRACK_LP4X_LOWFREQ_VREF_SEL  0x37 // for <= DDR3733
#define IMP_TRACK_LP4X_HIGHFREQ_VREF_SEL  0x3a // for > 3733 and Samsung NT-ODTN
/* LPDDR5 IMP_VREF_SEL w/o term ==== */
#define IMP_TRACK_LP5_LOWFREQ_VREF_SEL	0x38 // for <= DDR3733
#define IMP_TRACK_LP5_HIGHFREQ_VREF_SEL  0x3a // for > 3733 and Samsung NT-ODTN

static const U8 ImpLP4VrefSel[IMP_VREF_MAX][IMP_DRV_MAX] = {
				  /* DRVP  DRVN  ODTP  ODTN */
/* IMP_LOW_FREQ */	{0x37, 0x33, 0x00, 0x37},
/* IMP_HIGH_FREQ */ {0x3a, 0x33, 0x00, 0x3a},
/* IMP_NT_ODTN */	{0x2a, 0x2a, 0x00, 0x3a}
};

static const U8 ImpLP5VrefSel[IMP_VREF_MAX][IMP_DRV_MAX] = {
				  /* DRVP  DRVN  ODTP  ODTN */
/* IMP_LOW_FREQ */	{0x38, 0x33, 0x00, 0x38},
/* IMP_HIGH_FREQ */ {0x3a, 0x33, 0x00, 0x3a},
/* IMP_NT_ODTN */	{0x2a, 0x2a, 0x00, 0x3a}
};

/* Refer to "IMPCAL Settings" document register "RG_RIMP_VREF_SEL" settings */
// @Maoauo: DRVP/ODTN for IMP tracking. But DRVN not support IMP tracking. (before La_fite)
// DRVP/DRVN/ODTN for IMP tracking after Pe_trus
static void vImpCalVrefSel(DRAMC_CTX_T *p, DRAMC_IMP_T efreq_region, U8 u1ImpCalStage)
{
	U8 u1RegTmpValue = 0;
	U32 u4DrvFld = 0;

	if (p->dram_type == TYPE_LPDDR4X)
	{
		if (u1ImpCalStage == IMPCAL_STAGE_TRACKING)
			u1RegTmpValue = (efreq_region == IMP_LOW_FREQ) ? IMP_TRACK_LP4X_LOWFREQ_VREF_SEL : IMP_TRACK_LP4X_HIGHFREQ_VREF_SEL;
		else
			u1RegTmpValue = ImpLP4VrefSel[efreq_region][u1ImpCalStage];
	}
	else if (p->dram_type == TYPE_LPDDR5)
	{
		if (u1ImpCalStage == IMPCAL_STAGE_TRACKING)
			u1RegTmpValue = (efreq_region == IMP_LOW_FREQ) ? IMP_TRACK_LP5_LOWFREQ_VREF_SEL : IMP_TRACK_LP5_HIGHFREQ_VREF_SEL;
		else
			u1RegTmpValue = ImpLP5VrefSel[efreq_region][u1ImpCalStage];
	}
	else
	{
		mcSHOW_ERR_MSG(("[vImpCalVrefSel] Warnning: Need confirm DRAM type for IMP_VREF_SEL !!!\n"));
		#if __ETT__
		while(1);
		#endif
	}

	switch (u1ImpCalStage)
	{
		case IMPCAL_STAGE_DRVP:
			u4DrvFld = SHU_CA_CMD12_RG_RIMP_VREF_SEL_DRVP;
			break;
		case IMPCAL_STAGE_DRVN:
			u4DrvFld = SHU_CA_CMD12_RG_RIMP_VREF_SEL_DRVN;
			break;
		case IMPCAL_STAGE_ODTN:
			u4DrvFld = SHU_CA_CMD12_RG_RIMP_VREF_SEL_ODTN;
			break;
		default:
			mcSHOW_ERR_MSG(("[vImpCalVrefSel] Warnning: Need confirm u1ImpCalStage for SW IMP Calibration !!!\n"));
			break;
	}

	// dbg msg after vref_sel selection
	mcSHOW_DBG_MSG3(("[vImpCalVrefSel] IMP_VREF_SEL 0x%x, IMPCAL stage:%u, freq_region:%u\n",
					  u1RegTmpValue, u1ImpCalStage, efreq_region));

	/* Set IMP_VREF_SEL register field's value */
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD12), u1RegTmpValue, u4DrvFld);

	return;
}

static U32 DramcSwImpCalResult(DRAMC_CTX_T *p, const char *drvType, U32 u4Fld)
{
	U32 u4ImpxDrv = 0, u4ImpCalResult = 0;
	U32 u4CheckImpChange = (u4Fld == SHU_MISC_IMPCAL1_IMPDRVP)? 1: 0;

	for (u4ImpxDrv = 0; u4ImpxDrv < 32; u4ImpxDrv++)
	{
#if 0 // for A60868 no need
		if (u4ImpxDrv == 16) //0~15, 29~31
			u4ImpxDrv = 29;
#endif

		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_MISC_IMPCAL1), u4ImpxDrv, u4Fld);
		mcDELAY_US(1);
		u4ImpCalResult = u4IO32ReadFldAlign((DDRPHY_REG_MISC_PHY_RGS_CMD), MISC_PHY_RGS_CMD_RGS_RIMPCALOUT);
		mcSHOW_DBG_MSG2(("OCD %s=%d ,CALOUT=%d\n", drvType, u4ImpxDrv, u4ImpCalResult));

		if (u4ImpCalResult == u4CheckImpChange)//first found
		{
			mcSHOW_DBG_MSG2(("\nOCD %s calibration OK! %s=%d\n\n", drvType, drvType, u4ImpxDrv));
			break;
		}
	}

	if (u4ImpxDrv == 32) // Can't find SwImp drv results
	{
		u4ImpxDrv = 31;
		mcSHOW_DBG_MSG2(("\nOCD %s calibration FAIL! %s=%d\n\n", drvType, drvType, u4ImpxDrv));
	}

	return u4ImpxDrv;
}

DRAM_STATUS_T DramcSwImpedanceCal(DRAMC_CTX_T *p, U8 u1Para, DRAMC_IMP_T freq_region)
{
	U32 u4DRVP_Result = 0xff, u4ODTN_Result = 0xff, u4DRVN_Result = 0xff;
	//U32 u4BaklReg_DDRPHY_MISC_IMP_CTRL0, u4BaklReg_DDRPHY_MISC_IMP_CTRL1;
	U32 u4BaklReg_DRAMC_REG_IMPCAL;
	U8 backup_channel;
	U32 backup_broadcast;
	U8 u1DrvType = 0, u1CALI_ENP = 0, u1CALI_ENN = 0, u1DDR4 = 0;
	U32 u4SwImpCalResult = 0, u4DrvFld = 0;
	const char *drvStr = "NULL";

	backup_broadcast = GetDramcBroadcast();
	DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);

	//default set FAIL
	vSetCalibrationResult(p, DRAM_CALIBRATION_SW_IMPEDANCE, DRAM_FAIL);

	//Suspend: DA_RIMP_DMSUS=1
	vIO32WriteFldMulti_All(DDRPHY_REG_MISC_LP_CTRL, P_Fld(0x0, MISC_LP_CTRL_RG_ARDMSUS_10) | \
						P_Fld(0x0, MISC_LP_CTRL_RG_ARDMSUS_10_LP_SEL) | \
						P_Fld(0x0, MISC_LP_CTRL_RG_RIMP_DMSUS_10) | \
						P_Fld(0x0, MISC_LP_CTRL_RG_RIMP_DMSUS_10_LP_SEL));

	//Disable IMP HW Tracking
	//Hw Imp tracking disable for all channels Because SwImpCal will be K again when resume from DDR reserved mode
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_IMPCAL, 0, MISC_IMPCAL_IMPCAL_HW);

	backup_channel = p->channel;
	vSetPHY2ChannelMapping(p, CHANNEL_A);

	//Register backup
	//u4BaklReg_DDRPHY_MISC_IMP_CTRL0 = u4IO32Read4B((DDRPHY_MISC_IMP_CTRL0));
	//u4BaklReg_DDRPHY_MISC_IMP_CTRL1 = u4IO32Read4B((DDRPHY_MISC_IMP_CTRL1));
	u4BaklReg_DRAMC_REG_IMPCAL = u4IO32Read4B(DRAMC_REG_ADDR(DDRPHY_REG_MISC_IMPCAL));

	//RG_IMPCAL_VREF_SEL (now set in vImpCalVrefSel())
	//RG_IMPCAL_LP3_EN=0, RG_IMPCAL_LP4_EN=1
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_IMP_CTRL1), 0, MISC_IMP_CTRL1_RG_RIMP_PRE_EN);
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_IMPCAL), P_Fld(0, MISC_IMPCAL_IMPCAL_CALI_ENN) | P_Fld(1, MISC_IMPCAL_IMPCAL_IMPPDP) | \
						P_Fld(1, MISC_IMPCAL_IMPCAL_IMPPDN));	 //RG_RIMP_BIAS_EN and RG_RIMP_VREF_EN move to IMPPDP and IMPPDN

	if (is_lp5_family(p))
		u1DDR4 = 0;
	else //LPDDR4
		u1DDR4 = 1;

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_IMP_CTRL1), P_Fld(1, MISC_IMP_CTRL1_RG_IMP_EN) | \
						P_Fld(0, MISC_IMP_CTRL1_RG_RIMP_DDR3_SEL) | \
						P_Fld(1, MISC_IMP_CTRL1_RG_RIMP_VREF_EN) | \
						P_Fld(u1DDR4, MISC_IMP_CTRL1_RG_RIMP_DDR4_SEL));
	mcDELAY_US(1);

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_IMPCAL), 1, MISC_IMPCAL_IMPCAL_CALI_EN);
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_MISC_IMPCAL1), P_Fld(0, SHU_MISC_IMPCAL1_IMPDRVN) | P_Fld(0, SHU_MISC_IMPCAL1_IMPDRVP));


	//LP4X: ODTN/DRVN/DRVP calibration start
	for (u1DrvType = DRVP; u1DrvType < IMP_DRV_MAX; u1DrvType++) // Calibration sequence for DRVP, DRVN and ODTN
	{
		if (u1DrvType == ODTP) // no use, skip ODTP
			continue;

		/* Set IMP_VREF_SEL value for DRVP/DRVN and ODTN */
		vImpCalVrefSel(p, freq_region, u1DrvType);

		switch (u1DrvType)
		{
			case DRVP:
				drvStr = "DRVP";
				u1CALI_ENP = 0x1;
				u1CALI_ENN = 0x0;
				u4DrvFld = SHU_MISC_IMPCAL1_IMPDRVP;
				u4DRVP_Result = 0;
				break;
			case DRVN:
			case ODTN:
				drvStr = (u1DrvType == DRVN)? "DRVN" : "ODTN";
				u1CALI_ENP = 0x0;
				u1CALI_ENN = (u1DrvType == DRVN)? 0x0: 0x1; // 0x1 change to ODTN path
				u4DrvFld = SHU_MISC_IMPCAL1_IMPDRVN;
				break;
			default:
				mcSHOW_ERR_MSG(("[DramcSwImpedanceCal] Warnning: Need confirm u1DrvType for SW IMP Calibration !!!\n"));
				break;
		}

		// @A60868 for DRVn/p and ODTn select
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_IMPCAL), u1CALI_ENP, MISC_IMPCAL_IMPCAL_CALI_ENP);  //MISC_IMP_CTRL1_RG_IMP_OCD_PUCMP_EN move to CALI_ENP
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_IMPCAL), u1CALI_ENN, MISC_IMPCAL_IMPCAL_CALI_ENN);  //MISC_IMP_CTRL1_RG_RIMP_ODT_EN move to CALI_ENN

		mcSHOW_DBG_MSG2(("\n\n\tK %s\n", drvStr));

		//DRVP=DRVP_FINAL
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_MISC_IMPCAL1), u4DRVP_Result, SHU_MISC_IMPCAL1_IMPDRVP);  //PUCMP_EN move to CALI_ENP
		//RIMP_DRV05 for LP4/5
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD12), 0, SHU_CA_CMD12_RG_RIMP_DRV05);


		//If RGS_TX_OCD_IMPCALOUTX=1
		//RG_IMPX_DRVN++;
		//Else save RG_IMPX_DRVN value and assign to DRVN
		u4SwImpCalResult = DramcSwImpCalResult(p, drvStr, u4DrvFld);

		switch (u1DrvType)
		{
			case DRVP:
				u4DRVP_Result = u4SwImpCalResult;
				break;
			case DRVN:
				u4DRVN_Result = u4SwImpCalResult;
				break;
			case ODTN:
				u4ODTN_Result = u4SwImpCalResult;
				break;
			default:
				mcSHOW_ERR_MSG(("[DramcSwImpedanceCal] Warnning: Need confirm u4SwImpCalResult for SW IMP Calibration !!!\n"));
				break;
		}
	}
	//Register Restore
	vIO32Write4B(DRAMC_REG_ADDR(DDRPHY_REG_MISC_IMPCAL), u4BaklReg_DRAMC_REG_IMPCAL);
	//vIO32Write4B((DDRPHY_MISC_IMP_CTRL0), u4BaklReg_DDRPHY_MISC_IMP_CTRL0);
	//vIO32Write4B((DDRPHY_MISC_IMP_CTRL1), u4BaklReg_DDRPHY_MISC_IMP_CTRL1);


/*** default value if K fail
	LP3:  DRVP=8, DRVN=9
	LP4:  DRVP=6, DRVN=9, ODTN=14
	LP4X(UT): DRVP=12, DRVN=9
	LP4X(T):  DRVP=5, DRVN=9, ODTN=14
	LP4P: DRVP=8, DRVN=10
***/
	mcSHOW_DBG_MSG(("[SwImpedanceCal] DRVP=%d, DRVN=%d, ODTN=%d\n", u4DRVP_Result, u4DRVN_Result, u4ODTN_Result));

	#if 0//HYNIX_IMPX_ADJUST
	if (u1Para)
	{
		u4ODTN_Result = ImpedanceAdjustment_Hynix(u4ODTN_Result, u1Para);
	}
	#endif

	gDramcSwImpedanceResult[freq_region][DRVP] = u4DRVP_Result;
	gDramcSwImpedanceResult[freq_region][DRVN] = u4DRVN_Result;
	gDramcSwImpedanceResult[freq_region][ODTP] = 0;
	gDramcSwImpedanceResult[freq_region][ODTN] = u4ODTN_Result;

#if RUNTIME_SHMOO_RELEATED_FUNCTION && SUPPORT_SAVE_TIME_FOR_CALIBRATION
	{
		U8 u1drv;
		{
			for (u1drv = 0; u1drv < 4; u1drv++)
			{
				if (p->femmc_Ready == 0)
					p->pSavetimeData->u1SwImpedanceResule[freq_region][u1drv] = gDramcSwImpedanceResult[freq_region][u1drv];
				else
				{
					gDramcSwImpedanceResult[freq_region][u1drv] = p->pSavetimeData->u1SwImpedanceResule[freq_region][u1drv];
					vSetCalibrationResult(p, DRAM_CALIBRATION_SW_IMPEDANCE, DRAM_FAST_K);
				}
			}
		}
	}
#endif

	mcSHOW_DBG_MSG(("freq_region=%d, Reg: DRVP=%d, DRVN=%d, ODTN=%d\n", freq_region, gDramcSwImpedanceResult[freq_region][DRVP],
									gDramcSwImpedanceResult[freq_region][DRVN], gDramcSwImpedanceResult[freq_region][ODTN]));

#if APPLY_SIGNAL_WAVEFORM_SETTINGS_ADJUST
	if ((p->dram_type == TYPE_LPDDR4) && (freq_region == 0))
	{
		gDramcSwImpedanceResult[freq_region][DRVP] = SwImpedanceAdjust(gDramcSwImpedanceResult[freq_region][DRVP], gDramcSwImpedanceAdjust[freq_region][DRVP]);
		gDramcSwImpedanceResult[freq_region][DRVN] = SwImpedanceAdjust(gDramcSwImpedanceResult[freq_region][DRVN], gDramcSwImpedanceAdjust[freq_region][ODTN]);
	}
	else
	{
		gDramcSwImpedanceResult[freq_region][DRVP] = SwImpedanceAdjust(gDramcSwImpedanceResult[freq_region][DRVP], gDramcSwImpedanceAdjust[freq_region][DRVP]);
		gDramcSwImpedanceResult[freq_region][ODTN] = SwImpedanceAdjust(gDramcSwImpedanceResult[freq_region][ODTN], gDramcSwImpedanceAdjust[freq_region][ODTN]);
	}

	mcSHOW_DBG_MSG(("freq_region=%d, Reg: DRVP=%d, DRVN=%d, ODTN=%d (After Adjust)\n", freq_region, gDramcSwImpedanceResult[freq_region][DRVP],
										gDramcSwImpedanceResult[freq_region][DRVN], gDramcSwImpedanceResult[freq_region][ODTN]));
#endif

#if __FLASH_TOOL_DA__
	if((gDramcSwImpedanceResult[freq_region][ODTN] ==0)||(gDramcSwImpedanceResult[freq_region][ODTN] >=31))
	{
		mcSHOW_DBG_MSG(("[WARNING] freq_region = %d, ODTN = %d ==> unexpect value\n", freq_region, gDramcSwImpedanceResult[freq_region][ODTN]));
		PINInfo_flashtool.IMP_ERR_FLAG |= (0x1<<(freq_region+ODTN));
	}
	else if((gDramcSwImpedanceResult[freq_region][DRVP] ==0)||(gDramcSwImpedanceResult[freq_region][DRVP] >=31))
	{
		mcSHOW_DBG_MSG(("[WARNING] freq_region = %d, DRVP = %d ==> unexpect value\n", freq_region, gDramcSwImpedanceResult[freq_region][DRVP]));
		PINInfo_flashtool.IMP_ERR_FLAG |= (0x1<<(freq_region+DRVP));
	}
	else if((gDramcSwImpedanceResult[freq_region][DRVN] ==0)||(gDramcSwImpedanceResult[freq_region][DRVN] >=31))
	{
		mcSHOW_DBG_MSG(("[WARNING] freq_region = %d, DRVN = %d ==> unexpect value\n", freq_region, gDramcSwImpedanceResult[freq_region][DRVN]));
		PINInfo_flashtool.IMP_ERR_FLAG |= (0x1<<(freq_region+DRVN));
	}
	else
#endif
	{
	vSetCalibrationResult(p, DRAM_CALIBRATION_SW_IMPEDANCE, DRAM_OK);
	}
	mcSHOW_DBG_MSG3(("[DramcSwImpedanceCal] Done\n\n"));

	vSetPHY2ChannelMapping(p, backup_channel);
	DramcBroadcastOnOff(backup_broadcast);

	return DRAM_OK;
}
#endif //SIMULATION_SW_IMPED

#if ENABLE_WRITE_DBI || TX_K_DQM_WITH_WDBI
void DramcWriteShiftMCKForWriteDBI(DRAMC_CTX_T *p, S8 iShiftMCK)
{
	U8 ucDataRateDivShift = 0;
	S8 s1UIMove = 0;

	ucDataRateDivShift = u1MCK2UI_DivShift(p);
	s1UIMove = iShiftMCK * (S8)(1 << ucDataRateDivShift);
	ShiftDQUI(p, s1UIMove, ALL_BYTES);
}
#endif

#if ENABLE_DUTY_SCAN_V2

#define DutyPrintAllLog 		0
#define DutyPrintCalibrationLog 0

#define DUTY_OFFSET_START -28
#define DUTY_OFFSET_END 28
#define DUTY_OFFSET_STEP 4

#define CLOCK_PI_START 0
#define CLOCK_PI_END 63

#if FOR_DV_SIMULATION_USED
#define CLOCK_PI_STEP 8
#else
#define CLOCK_PI_STEP 2
#endif

#define ClockDutyFailLowerBound 4500	// 45%
#define ClockDutyFailUpperBound 5500	// 55%
#define ClockDutyMiddleBound	5000	// 50%

/*
* duty form smallest to biggest
* 011111->011110->...->000001-->000000=100000->100001-->...->111111
*/
static U8 DramcDutyDelayRGSettingConvert(DRAMC_CTX_T *p, S8 scDutyDelay,
	U8 *tDly)
{
	U8 tDelay;

	if (scDutyDelay < 0)
	{
		tDelay = -scDutyDelay;
	}
	else if (scDutyDelay > 0)
	{
		tDelay = scDutyDelay + (1 << 5);
	}
	else
	{
		tDelay = 0;
	}

	*tDly = tDelay;
	return tDelay;
}

static void DramcClockDutySetClkDelayCell(DRAMC_CTX_T *p, S8 *scDutyDelay)
{
	U8 u1ShuffleIdx = 0;
	U32 save_offset;
	U8 tDelay;

	DramcDutyDelayRGSettingConvert(p, scDutyDelay[0], &tDelay);

#if DUTY_SCAN_V2_ONLY_K_HIGHEST_FREQ
	for(u1ShuffleIdx = 0; u1ShuffleIdx<DRAM_DFS_SHUFFLE_MAX; u1ShuffleIdx++)
#endif
	{
		save_offset = u1ShuffleIdx * SHU_GRP_DDRPHY_OFFSET;
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_TXDUTY + save_offset),
			P_Fld(tDelay, SHU_CA_TXDUTY_DA_TX_ARCLK_DUTY_DLY));
	}
}

static void DQSDutyScan_SetDqsDelayCell(DRAMC_CTX_T *p, S8 *scDutyDelay)
{
	U8 u1ShuffleIdx = 0, u1DQSIdx;
	U32 save_offset;
	U8 tDelay[2];

//	  mcSHOW_DBG_MSG(("CH%d, Final DQS0 duty delay cell = %d\n", p->channel, scDutyDelay[0]));
//	  mcSHOW_DBG_MSG(("CH%d, Final DQS1 duty delay cell = %d\n", p->channel, scDutyDelay[1]));

	for(u1DQSIdx=0; u1DQSIdx<2; u1DQSIdx++)
	{
		DramcDutyDelayRGSettingConvert(p, scDutyDelay[u1DQSIdx], &(tDelay[u1DQSIdx]));
	}

#if DUTY_SCAN_V2_ONLY_K_HIGHEST_FREQ
	for(u1ShuffleIdx = 0; u1ShuffleIdx<DRAM_DFS_SHUFFLE_MAX; u1ShuffleIdx++)
#endif
	{
		{
			for(u1DQSIdx = 0; u1DQSIdx<2; u1DQSIdx++)
			{
				save_offset = u1ShuffleIdx * SHU_GRP_DDRPHY_OFFSET + u1DQSIdx*DDRPHY_AO_B0_B1_OFFSET;
				vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_TXDUTY) + save_offset,
					P_Fld(tDelay[u1DQSIdx], SHU_B0_TXDUTY_DA_TX_ARDQS_DUTY_DLY_B0));
			}
		}
	}
}

static void WCKDutyScan_SetWCKDelayCell(DRAMC_CTX_T *p, S8 *scDutyDelay)
{
	U8 u1ShuffleIdx = 0, u1DQSIdx;
	U32 save_offset;
	U8 tDelay[2];

	for(u1DQSIdx=0; u1DQSIdx<2; u1DQSIdx++)
	{
		DramcDutyDelayRGSettingConvert(p, scDutyDelay[u1DQSIdx], &(tDelay[u1DQSIdx]));
	}

#if DUTY_SCAN_V2_ONLY_K_HIGHEST_FREQ
	for(u1ShuffleIdx = 0; u1ShuffleIdx<DRAM_DFS_SHUFFLE_MAX; u1ShuffleIdx++)
#endif
	{
		{
			for(u1DQSIdx = 0; u1DQSIdx<2; u1DQSIdx++)
			{
				save_offset = u1ShuffleIdx * SHU_GRP_DDRPHY_OFFSET + u1DQSIdx*DDRPHY_AO_B0_B1_OFFSET;
				vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_TXDUTY) + save_offset,
					P_Fld(tDelay[u1DQSIdx], SHU_B0_TXDUTY_DA_TX_ARWCK_DUTY_DLY_B0));
			}
		}
	}
}

#if APPLY_DQDQM_DUTY_CALIBRATION
static void DQDQMDutyScan_SetDQDQMDelayCell(DRAMC_CTX_T *p, U8 u1ChannelIdx, S8 *scDutyDelay, U8 k_type)
{
	U8 u1ShuffleIdx = 0, u1DQSIdx;
	U32 save_offset;
	U8 tDelay[2];

	for(u1DQSIdx=0; u1DQSIdx<2; u1DQSIdx++)
	{
		DramcDutyDelayRGSettingConvert(p, scDutyDelay[u1DQSIdx], &(tDelay[u1DQSIdx]));
	}

#if DUTY_SCAN_V2_ONLY_K_HIGHEST_FREQ
	for(u1ShuffleIdx = 0; u1ShuffleIdx<DRAM_DFS_SHUFFLE_MAX; u1ShuffleIdx++)
#endif
	{
		for(u1DQSIdx = 0; u1DQSIdx<2; u1DQSIdx++)
		{
			save_offset = u1ShuffleIdx * SHU_GRP_DDRPHY_OFFSET + u1DQSIdx*DDRPHY_AO_B0_B1_OFFSET;

			if (k_type == DutyScan_Calibration_K_DQ)
			{
				vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_TXDUTY) + save_offset,
					P_Fld(tDelay[u1DQSIdx], SHU_B0_TXDUTY_DA_TX_ARDQ_DUTY_DLY_B0));
			}

			if (k_type == DutyScan_Calibration_K_DQM)
			{
				vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_TXDUTY) + save_offset,
					P_Fld(tDelay[u1DQSIdx], SHU_B0_TXDUTY_DA_TX_ARDQM_DUTY_DLY_B0));
			}
		}
	}
}

#if 0
void DQDQMDutyScan_CopyDQRG2DQMRG(DRAMC_CTX_T *p)
{
	U8 u1ShuffleIdx = 0, u1DQSIdx, u1RankIdx = 0;
	U32 save_offset;
	U8 ucDQDQMDelay;
	U8 ucRev_DQDQM_Bit0, ucRev_DQDQM_Bit1;

#if DUTY_SCAN_V2_ONLY_K_HIGHEST_FREQ
	for(u1ShuffleIdx = 0; u1ShuffleIdx<DRAM_DFS_SHUFFLE_MAX; u1ShuffleIdx++)
#endif
	{
			for(u1DQSIdx = 0; u1DQSIdx<2; u1DQSIdx++)
			{
			save_offset = u1ShuffleIdx * SHU_GRP_DDRPHY_OFFSET + u1DQSIdx*DDRPHY_AO_B0_B1_OFFSET_0X80;

			ucDQDQMDelay = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_SHU_B0_DQ3) + save_offset, SHU_B0_DQ3_RG_ARDQ_DUTYREV_B0_DQ_DLY);
			ucRev_DQDQM_Bit0 = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_SHU_B0_DQ3) + save_offset, SHU_B0_DQ3_RG_TX_ARDQS0_PU_PRE_B0_BIT0);
			ucRev_DQDQM_Bit1 = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_SHU_B0_DQ3) + save_offset, SHU_B0_DQ3_RG_TX_ARDQS0_PU_PRE_B0_BIT0);

			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_SHU_B0_DQ3) + save_offset, P_Fld(ucDQDQMDelay, SHU_B0_DQ3_RG_ARDQ_DUTYREV_B0_DQM_DLY)
																	| P_Fld(ucRev_DQDQM_Bit0, SHU_B0_DQ3_RG_TX_ARDQS0_PU_B0_BIT0)
																	| P_Fld(ucRev_DQDQM_Bit1, SHU_B0_DQ3_RG_TX_ARDQS0_PU_B0_BIT1));
			}
		}
	}
#endif
#endif

S8 gcFinal_K_CLK_delay_cell[CHANNEL_NUM][DQS_NUMBER];
S8 gcFinal_K_DQS_delay_cell[CHANNEL_NUM][DQS_NUMBER];
S8 gcFinal_K_WCK_delay_cell[CHANNEL_NUM][DQS_NUMBER];
#if APPLY_DQDQM_DUTY_CALIBRATION
S8 gcFinal_K_DQ_delay_cell[CHANNEL_NUM][DQS_NUMBER];
S8 gcFinal_K_DQM_delay_cell[CHANNEL_NUM][DQS_NUMBER];
#endif

void DramcNewDutyCalibration(DRAMC_CTX_T *p)
{
	U8 u1backup_channel, u1backup_rank;

#if(DQS_DUTY_SLT_CONDITION_TEST)
		U16 u2TestCnt, u2FailCnt=0, u2TestCntTotal =20; //fra 400;
		U8 u1ByteIdx, u1PI_FB;
		U32 u4Variance;
#endif

	u1backup_rank = u1GetRank(p);
	vSetRank(p, RANK_0);

#if !FT_DSIM_USED
#if DUTY_SCAN_V2_ONLY_K_HIGHEST_FREQ
	if((p->frequency == u2DFSGetHighestFreq(p)) && (Get_PRE_MIOCK_JMETER_HQA_USED_flag()==0))
#else
	//TODO if(Get_PRE_MIOCK_JMETER_HQA_USED_flag()==0)
#endif
#endif
	{
		U8 u1ChannelIdx;
		u1backup_channel = vGetPHY2ChannelMapping(p);

		#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
		if(p->femmc_Ready==1)
		{
			for(u1ChannelIdx=CHANNEL_A; u1ChannelIdx<p->support_channel_num; u1ChannelIdx++)
			{
				vSetPHY2ChannelMapping(p, u1ChannelIdx);
				DramcClockDutySetClkDelayCell(p, p->pSavetimeData->s1ClockDuty_clk_delay_cell[p->channel]);
				DQSDutyScan_SetDqsDelayCell(p, p->pSavetimeData->s1DQSDuty_clk_delay_cell[p->channel]);
				WCKDutyScan_SetWCKDelayCell(p, p->pSavetimeData->s1WCKDuty_clk_delay_cell[p->channel]);
				#if APPLY_DQDQM_DUTY_CALIBRATION
				DQDQMDutyScan_SetDQDQMDelayCell(p, p->channel, p->pSavetimeData->s1DQMDuty_clk_delay_cell[p->channel], DutyScan_Calibration_K_DQM);
				DQDQMDutyScan_SetDQDQMDelayCell(p, p->channel, p->pSavetimeData->s1DQDuty_clk_delay_cell[p->channel], DutyScan_Calibration_K_DQ);
				#endif
			}
			vSetPHY2ChannelMapping(p, u1backup_channel);
			return;
		}
		#endif

		vSetPHY2ChannelMapping(p, u1backup_channel);
	}

	vSetRank(p, u1backup_rank);
}
#endif
