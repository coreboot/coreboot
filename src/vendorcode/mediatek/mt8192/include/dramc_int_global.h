/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef _INT_GLOBAL_H
#define _INT_GLOBAL_H

#include "dramc_pi_api.h"
#include "dramc_int_slt.h"

/*
 ****************************************************************************************
 ** macro
 ****************************************************************************************
 */
#define DVT_TEST_DUMMY_RD_SIDEBAND_FROM_SPM 0
//#define DVT_TEST_DUMMY_READ_FOR_DQS_GATING_TRACKING
//#define DVT_TEST_RX_DLY_HW_TRACKING


/*
 ****************************************************************************************
 ** ANA_init_config.c
 ****************************************************************************************
 */
EXTERN void ANA_init(DRAMC_CTX_T *p);
EXTERN void RESETB_PULL_DN(DRAMC_CTX_T *p);



/*
 ****************************************************************************************
 ** DIG_NONSHUF_config.c
 ****************************************************************************************
 */
EXTERN void DIG_STATIC_SETTING(DRAMC_CTX_T *p);


/*
 ****************************************************************************************
 *
 ** DIG_SHUF_config.c
 ****************************************************************************************
 */
EXTERN void DIG_CONFIG_SHUF(DRAMC_CTX_T *p,U32 ch_id, U32 group_id);


/*
 ****************************************************************************************
 *
 ** dramc_debug.c
 ****************************************************************************************
 */
EXTERN U8 gFinalCBTVrefDQ[CHANNEL_NUM][RANK_MAX];
EXTERN U8 gFinalRXVrefDQ[CHANNEL_NUM][RANK_MAX][2];
EXTERN U8 gFinalTXVrefDQ[CHANNEL_NUM][RANK_MAX];

#ifdef FOR_HQA_REPORT_USED
EXTERN U8 gHQALog_flag;
EXTERN U16 gHQALOG_RX_delay_cell_ps_075V;
EXTERN int hqa_vmddr_class;
EXTERN void HQA_Log_Message_for_Report(DRAMC_CTX_T *p, U8 u1ChannelIdx, U8 u1RankIdx, U32 who_am_I, U8 *main_str, U8 *main_str2, U8 byte_bit_idx, S32 value1, U8 *ans_str);
#endif


// --- Eye scan variables -----

EXTERN U8 gCBT_EYE_Scan_flag;
EXTERN U8 gRX_EYE_Scan_flag;
EXTERN U8 gTX_EYE_Scan_flag;
EXTERN U8 gEye_Scan_color_flag;
EXTERN U8 gCBT_EYE_Scan_only_higheset_freq_flag;
EXTERN U8 gRX_EYE_Scan_only_higheset_freq_flag;
EXTERN U8 gTX_EYE_Scan_only_higheset_freq_flag;
EXTERN U8 gEye_Scan_unterm_highest_flag;

#if ENABLE_EYESCAN_GRAPH
#define VREF_TOTAL_NUM_WITH_RANGE (((51 + 30) + 1) / (EYESCAN_GRAPH_CATX_VREF_STEP < EYESCAN_GRAPH_RX_VREF_STEP ? EYESCAN_GRAPH_CATX_VREF_STEP : EYESCAN_GRAPH_RX_VREF_STEP))	//range0 0~50 + range1 21~50
#define EYESCAN_BROKEN_NUM 3
#define EYESCAN_DATA_INVALID 0x7f
EXTERN S16  gEyeScan_Min[VREF_VOLTAGE_TABLE_NUM_LP5][DQ_DATA_WIDTH_LP4][EYESCAN_BROKEN_NUM];
EXTERN S16  gEyeScan_Max[VREF_VOLTAGE_TABLE_NUM_LP5][DQ_DATA_WIDTH_LP4][EYESCAN_BROKEN_NUM];
EXTERN S16 gEyeScan_MinMax_store_delay[DQS_NUMBER];
EXTERN U16 gEyeScan_CaliDelay[DQS_NUMBER];
EXTERN U16  gEyeScan_WinSize[VREF_VOLTAGE_TABLE_NUM_LP5][DQ_DATA_WIDTH_LP4];
EXTERN S16  gEyeScan_DelayCellPI[DQ_DATA_WIDTH_LP4];
EXTERN U16 gEyeScan_ContinueVrefHeight[DQ_DATA_WIDTH_LP4];
EXTERN U16 gEyeScan_TotalPassCount[DQ_DATA_WIDTH_LP4];
EXTERN void Dramc_K_TX_EyeScan_Log(DRAMC_CTX_T *p);
EXTERN void print_EYESCAN_LOG_message(DRAMC_CTX_T *p, U8 print_type);
#endif
#if MRW_CHECK_ONLY || MRW_BACKUP
EXTERN U8 gFSPWR_Flag[RANK_MAX];
#endif
#ifdef FOR_HQA_TEST_USED
EXTERN void HQA_measure_message_reset_all_data(DRAMC_CTX_T *p);
#endif
#if RUNTIME_SHMOO_RELEATED_FUNCTION && SUPPORT_SAVE_TIME_FOR_CALIBRATION
void DramcRunTimeShmooRG_BackupRestore(DRAMC_CTX_T *p);
#endif



/*
 ****************************************************************************************
 ** dramc_dvfs.c
 ****************************************************************************************
 */
EXTERN U8 get_shuffleIndex_by_Freq(DRAMC_CTX_T *p);
EXTERN void vInitMappingFreqArray(DRAMC_CTX_T *p);
EXTERN void vSetDFSTable(DRAMC_CTX_T *p, DRAM_DFS_FREQUENCY_TABLE_T *pFreqTable);
EXTERN DRAM_DFS_FREQUENCY_TABLE_T* get_FreqTbl_by_shuffleIndex(DRAMC_CTX_T *p, U8 index);
EXTERN void vSetDFSFreqSelByTable(DRAMC_CTX_T *p, DRAM_DFS_FREQUENCY_TABLE_T *pFreqTable);
EXTERN void DramcDFSDirectJump(DRAMC_CTX_T *p, U8 shu_level);
EXTERN void DramcSaveToShuffleSRAM(DRAMC_CTX_T *p, DRAM_DFS_SHUFFLE_TYPE_T srcRG, DRAM_DFS_SHUFFLE_TYPE_T dstRG);
EXTERN void LoadShuffleSRAMtoDramc(DRAMC_CTX_T *p, DRAM_DFS_SHUFFLE_TYPE_T srcRG, DRAM_DFS_SHUFFLE_TYPE_T dstRG);
EXTERN void DramcDFSDirectJump_RGMode(DRAMC_CTX_T *p, U8 shu_level);
EXTERN void DVFSSettings(DRAMC_CTX_T *p);
EXTERN void DPMEnableTracking(DRAMC_CTX_T *p, U32 u4Reg, U32 u4Field, U8 u1ShuIdx, U8 u1Enable);
EXTERN void DPMInit(DRAMC_CTX_T *p);
EXTERN void TransferPLLToSPMControl(DRAMC_CTX_T *p, U32 MD32Offset);
EXTERN void DramcCopyShu0toShu1(DRAMC_CTX_T *p, U32 u4StartAddr, U32 u4EndAddr);
EXTERN void DdrphyCopyShu0toShu1(DRAMC_CTX_T *p, U32 u4StartAddr, U32 u4EndAddr);
EXTERN void EnableDFSHwModeClk(DRAMC_CTX_T *p);
EXTERN void DPHYSaveToSRAMShuWA(DRAMC_CTX_T *p, U8 shu_level);
EXTERN void DPHYSRAMShuWAToSHU1(DRAMC_CTX_T *p);
EXTERN void SRAMShuRestoreToDPHYWA(DRAMC_CTX_T *p, U8 sram_shu_level, U8 pingpong_shu_level);


/*
 ****************************************************************************************
 ** dramc_dv_freq_related.c
 ****************************************************************************************
 */
EXTERN void sv_algorithm_assistance_LP4_1600(DRAMC_CTX_T *p);
EXTERN void sv_algorithm_assistance_LP4_3733(DRAMC_CTX_T *p);
EXTERN void sv_algorithm_assistance_LP4_800(DRAMC_CTX_T *p);
EXTERN void CInit_golden_mini_freq_related_vseq_LP4_1600(DRAMC_CTX_T *p);
EXTERN void CInit_golden_mini_freq_related_vseq_LP4_4266(DRAMC_CTX_T *p);
EXTERN void CInit_golden_mini_freq_related_vseq_LP5_3200(DRAMC_CTX_T *p);
EXTERN void CInit_golden_mini_freq_related_vseq_LP5_3200_SHU1(DRAMC_CTX_T *p);
EXTERN void CInit_golden_mini_freq_related_vseq_LP5_4266(DRAMC_CTX_T *p);
EXTERN void CInit_golden_mini_freq_related_vseq_LP5_5500(DRAMC_CTX_T *p);


/*
 ****************************************************************************************
 ** dramc_dv_main.c
 ****************************************************************************************
 */
#if (FOR_DV_SIMULATION_USED == 1)
EXTERN void DPI_DRAMC_init_entry();
EXTERN void DPI_DRAM_INIT();
#endif


/*
 ****************************************************************************************
 ** dramc_pi_basic.c
 ****************************************************************************************
 */
EXTERN U8 u1PrintModeRegWrite;
EXTERN void vApplyConfigBeforeCalibration(DRAMC_CTX_T *p);
EXTERN DRAM_STATUS_T DramcInit(DRAMC_CTX_T *p);
EXTERN void SetCKE2RankIndependent(DRAMC_CTX_T *p);
EXTERN void DramcDQSPrecalculation_TrackingOff(DRAMC_CTX_T *p, U8 shu_level);
EXTERN void DramcDQSPrecalculation_TrackingOn(DRAMC_CTX_T *p, U8 shu_level);
EXTERN void DramcHWDQSGatingTracking_ModeSetting(DRAMC_CTX_T *p);
EXTERN void Set_MRR_Pinmux_Mapping(DRAMC_CTX_T *p);
EXTERN void Set_DQO1_Pinmux_Mapping(DRAMC_CTX_T *p);
#if CBT_MOVE_CA_INSTEAD_OF_CLK
EXTERN void DramcCmdUIDelaySetting(DRAMC_CTX_T *p, U8 value);
#endif
EXTERN void cbt_switch_freq(DRAMC_CTX_T *p, U8 freq);
EXTERN DRAM_STATUS_T DramcModeRegInit_LP4(DRAMC_CTX_T *p);
EXTERN DRAM_STATUS_T DramcModeRegInit_CATerm(DRAMC_CTX_T *p, U8 bWorkAround);
EXTERN void DramcPowerOnSequence(DRAMC_CTX_T *p);
EXTERN void Global_Option_Init(DRAMC_CTX_T *p);
EXTERN U16 u2DFSGetHighestFreq(DRAMC_CTX_T * p);
EXTERN void EnableDRAMModeRegWriteDBIAfterCalibration(DRAMC_CTX_T *p);
EXTERN void EnableDRAMModeRegReadDBIAfterCalibration(DRAMC_CTX_T *p);
EXTERN void ApplyWriteDBIPowerImprove(DRAMC_CTX_T *p, U8 onoff);
EXTERN void DramcHMR4_Presetting(DRAMC_CTX_T *p);
EXTERN void DramcEnablePerBankRefresh(DRAMC_CTX_T *p, bool en);
EXTERN void RXPICGSetting(DRAMC_CTX_T * p);
EXTERN void TXPICGNewModeEnable(DRAMC_CTX_T * p);
EXTERN unsigned int DDRPhyFreqMeter(void);
#ifndef DPM_CONTROL_AFTERK
EXTERN void dramc_exit_with_DFS_legacy_mode(DRAMC_CTX_T * p);
#endif



/*
 ****************************************************************************************
 ** dramc_pi_calibration_api.c
 ****************************************************************************************
 */
EXTERN U16 gu2MR0_Value[RANK_MAX];	//read only mode register
EXTERN U32 gDramcSwImpedanceResult[IMP_VREF_MAX][IMP_DRV_MAX];	//ODT_ON/OFF x DRVP/DRVN/ODTP/ODTN
EXTERN U16 u2g_num_dlycell_perT_all[DRAM_DFS_SHUFFLE_MAX][CHANNEL_NUM];	//TODO: to be removed by Francis
EXTERN U16 u2gdelay_cell_ps_all[DRAM_DFS_SHUFFLE_MAX][CHANNEL_NUM];		//TODO: to be removed by Francis
EXTERN U16 u2gdelay_cell_ps;
EXTERN U8 gCBT_VREF_RANGE_SEL;
EXTERN U32 u4gVcore[DRAM_DFS_SHUFFLE_MAX];
EXTERN U8 uiLPDDR4_O1_Mapping_POP[CHANNEL_NUM][16];
EXTERN const U8 uiLPDDR4_O1_DRAM_Pinmux[PINMUX_MAX][CHANNEL_NUM][16];
EXTERN U8 uiLPDDR4_CA_Mapping_POP[CHANNEL_NUM][6];
EXTERN const U8 uiLPDDR4_CA_DRAM_Pinmux[PINMUX_MAX][CHANNEL_NUM][6];

#if __ETT__
EXTERN U8 gETT_WHILE_1_flag;
#endif

#ifdef FOR_HQA_REPORT_USED
extern U8 gHQALog_flag;
extern U16 gHQALOG_RX_delay_cell_ps_075V;
#endif

#ifdef FOR_HQA_TEST_USED
EXTERN U16 gFinalCBTVrefCA[CHANNEL_NUM][RANK_MAX];
EXTERN U16 gFinalCBTCA[CHANNEL_NUM][RANK_MAX][10];
EXTERN U16 gFinalRXPerbitWin[CHANNEL_NUM][RANK_MAX][DQ_DATA_WIDTH];
EXTERN U16 gFinalTXPerbitWin[CHANNEL_NUM][RANK_MAX][DQ_DATA_WIDTH];
EXTERN U16 gFinalTXPerbitWin_min_max[CHANNEL_NUM][RANK_MAX];
EXTERN U16 gFinalTXPerbitWin_min_margin[CHANNEL_NUM][RANK_MAX];
EXTERN U16 gFinalTXPerbitWin_min_margin_bit[CHANNEL_NUM][RANK_MAX];
EXTERN S8 gFinalClkDuty[CHANNEL_NUM];
EXTERN U32 gFinalClkDutyMinMax[CHANNEL_NUM][2];
EXTERN S8 gFinalDQSDuty[CHANNEL_NUM][DQS_NUMBER];
EXTERN U32 gFinalDQSDutyMinMax[CHANNEL_NUM][DQS_NUMBER][2];
#endif
EXTERN U8 u1MR01Value[FSP_MAX];
EXTERN U8 u1MR02Value[FSP_MAX];
EXTERN U8 u1MR03Value[FSP_MAX];
EXTERN U8 u1MR11Value[FSP_MAX];
EXTERN U8 u1MR18Value[FSP_MAX];
EXTERN U8 u1MR19Value[FSP_MAX];
EXTERN U8 u1MR20Value[FSP_MAX];
EXTERN U8 u1MR21Value[FSP_MAX];
EXTERN U8 u1MR22Value[FSP_MAX];
EXTERN U8 u1MR51Value[FSP_MAX];
EXTERN U8 u1MR04Value[RANK_MAX];
EXTERN U8 u1MR13Value[RANK_MAX];
EXTERN U8 u1MR26Value[RANK_MAX];
EXTERN U8 u1MR30Value[RANK_MAX];
EXTERN U8 u1MR12Value[CHANNEL_NUM][RANK_MAX][FSP_MAX];
EXTERN U8 u1MR14Value[CHANNEL_NUM][RANK_MAX][FSP_MAX];
#if PINMUX_AUTO_TEST_PER_BIT_RX
EXTERN U8 gRX_check_per_bit_flag;
EXTERN S16 gFinalRXPerbitFirstPass[CHANNEL_NUM][DQ_DATA_WIDTH];
#endif
#if PINMUX_AUTO_TEST_PER_BIT_TX
EXTERN U8 gTX_check_per_bit_flag;
EXTERN S16 gFinalTXPerbitFirstPass[CHANNEL_NUM][DQ_DATA_WIDTH];
#endif
EXTERN U8 u1IsLP4Div4DDR800(DRAMC_CTX_T *p);
EXTERN DRAM_STATUS_T DramcTxWindowPerbitCal(DRAMC_CTX_T *p, DRAM_TX_PER_BIT_CALIBRATION_TYTE_T calType, U8 u1VrefScanEnable, u8 isAutoK);
EXTERN DRAM_STATUS_T DramcZQCalibration(DRAMC_CTX_T *p, U8 rank);
EXTERN DRAM_STATUS_T CmdBusTrainingLP45(DRAMC_CTX_T *p, int autok);
EXTERN DRAM_STATUS_T DramcWriteLeveling(DRAMC_CTX_T *p, u8 isAutoK, WLEV_DELAY_BASED_T stDelayBase);
EXTERN DRAM_STATUS_T dramc_rx_dqs_gating_cal(DRAMC_CTX_T *p, u8 autok, U8 use_enhanced_rdqs);
EXTERN DRAM_STATUS_T DramcRxWindowPerbitCal(DRAMC_CTX_T *p, RX_PATTERN_OPTION_T eRxPattern,
		U8 *u1AssignedVref, u8 isAutoK);
EXTERN DRAM_STATUS_T DramcRxDVSWindowCal(DRAMC_CTX_T *p);
EXTERN DRAM_STATUS_T Dramc8PhaseCal(DRAMC_CTX_T *p);
EXTERN DRAM_STATUS_T DramcSwImpedanceCal(DRAMC_CTX_T *p, U8 u1Para, DRAMC_IMP_T freq_region);
EXTERN void DramcSwImpedanceSaveRegister(DRAMC_CTX_T *p, U8 ca_freq_option, U8 dq_freq_option, U8 save_to_where);
EXTERN void vBeforeCalibration(DRAMC_CTX_T *p);
EXTERN void vAfterCalibration(DRAMC_CTX_T *p);
EXTERN void DramcRunTimeConfig(DRAMC_CTX_T *p);
EXTERN DRAM_STATUS_T DramcMiockJmeter(DRAMC_CTX_T *p);
EXTERN DRAM_STATUS_T DramcDutyCycleMonitor(DRAMC_CTX_T *p);
EXTERN void DramcTxOECalibration(DRAMC_CTX_T *p);
EXTERN DRAM_STATUS_T DramcRxdatlatCal(DRAMC_CTX_T *p);
EXTERN void LP4_ShiftDQS_OENUI(DRAMC_CTX_T *p, S8 iShiftUI, BYTES_T eByteIdx);
EXTERN void ShiftDQ_OENUI_AllRK(DRAMC_CTX_T *p, S8 iShiftUI, BYTES_T eByteIdx);
EXTERN void DramcMiockJmeterHQA(DRAMC_CTX_T *p);
EXTERN U8 u1IsPhaseMode(DRAMC_CTX_T *p);
EXTERN void RODTSettings(DRAMC_CTX_T *p);
EXTERN void DQSSTBSettings(DRAMC_CTX_T *p);
EXTERN void DramcWriteShiftMCKForWriteDBI(DRAMC_CTX_T *p, S8 iShiftMCK);
EXTERN void DramPhyReset(DRAMC_CTX_T *p);
EXTERN U32 DramcRxWinRDDQCInit(DRAMC_CTX_T *p);
EXTERN U32 DramcRxWinRDDQCRun(DRAMC_CTX_T *p);
EXTERN U32 DramcRxWinRDDQCEnd(DRAMC_CTX_T *p);
#if BYPASS_CALIBRATION
EXTERN void dle_factor_handler(DRAMC_CTX_T *p, U8 curr_val);
EXTERN void ShiftDQSWCK_UI(DRAMC_CTX_T *p, S8 iShiftUI, BYTES_T eByteIdx);
EXTERN void ShiftDQUI_AllRK(DRAMC_CTX_T *p, S8 iShiftUI, BYTES_T eByteIdx);
EXTERN void TXSetDelayReg_DQ(DRAMC_CTX_T *p, U8 u1UpdateRegUI, U8 ucdqm_ui_large[], U8 ucdqm_oen_ui_large[], U8 ucdqm_ui_small[], U8 ucdqm_oen_ui_small[], U8 ucdqm_pi[]);
EXTERN void TXSetDelayReg_DQM(DRAMC_CTX_T *p, U8 u1UpdateRegUI, U8 ucdqm_ui_large[], U8 ucdqm_oen_ui_large[], U8 ucdqm_ui_small[], U8 ucdqm_oen_ui_small[], U8 ucdqm_pi[]);
EXTERN void TXUpdateTXTracking(DRAMC_CTX_T *p, DRAM_TX_PER_BIT_CALIBRATION_TYTE_T calType, U8 ucdq_pi[], U8 ucdqm_pi[]);
EXTERN void Apply_LP4_1600_Calibraton_Result(DRAMC_CTX_T *p);
EXTERN void Apply_LP4_4266_Calibraton_Result(DRAMC_CTX_T *p);
#endif
EXTERN void vInitGlobalVariablesByCondition(DRAMC_CTX_T *p);
EXTERN void DramcDramcRxDVSCalPostProcess(DRAMC_CTX_T *p);
EXTERN void CBTDelayCACLK(DRAMC_CTX_T *p, S32 iDelay);
#if __FLASH_TOOL_DA__
EXTERN void vPrintPinInfoResult(DRAMC_CTX_T *p);
EXTERN DEBUG_PIN_INF_FOR_FLASHTOOL_T PINInfo_flashtool;
#endif


/*
 ****************************************************************************************
 ** dramc_pi_main.c
 ****************************************************************************************
 */
EXTERN DRAMC_CTX_T gTimeProfilingDramCtx;
EXTERN U8 gHQA_Test_Freq_Vcore_Level;
#if (FOR_DV_SIMULATION_USED == 1)
EXTERN U8 gu1BroadcastIsLP4;
#endif
EXTERN bool gAndroid_DVFS_en;
EXTERN bool gUpdateHighestFreq;
EXTERN DRAM_DFS_FREQUENCY_TABLE_T gFreqTbl[DRAM_DFS_SHUFFLE_MAX];
EXTERN void dump_dramc_ctx(DRAMC_CTX_T *p);
#ifdef ENABLE_MIOCK_JMETER
EXTERN void PRE_MIOCK_JMETER_HQA_USED(DRAMC_CTX_T *p);
#endif
EXTERN void vCalibration_Flow_For_MDL(DRAMC_CTX_T *p);
EXTERN void vDramCalibrationAllChannel(DRAMC_CTX_T *p);
EXTERN U32 vGetVoltage(DRAMC_CTX_T *p, U32 get_voltage_type);


/*
 ****************************************************************************************
 ** dramc_slt.c
 ****************************************************************************************
 */
#if ENABLE_EMI_LPBK_TEST
EXTERN U8 gEmiLpbkTest;
#endif
EXTERN void SLT_DramcDFS(DRAMC_CTX_T *p, int iDoDMA);
EXTERN void SLT_DFSTestProgram(DRAMC_CTX_T *p, int iDoDMA);
EXTERN void SLT_Test_DFS_and_Memory_Test(DRAMC_CTX_T*p);



/*
 ****************************************************************************************
 ** dramc_temp_function.c
 ****************************************************************************************
 */
EXTERN DRAMC_CTX_T DramCtx_LPDDR4;


/*
 ****************************************************************************************
 ** dramc_tracking.c
 ****************************************************************************************
 */
EXTERN U8 gu1MR23[CHANNEL_NUM][RANK_MAX];
EXTERN void DramcHWGatingInit(DRAMC_CTX_T *p);
EXTERN void DramcHWGatingOnOff(DRAMC_CTX_T *p, U8 u1OnOff);
EXTERN void DramcHWGatingDebugOnOff(DRAMC_CTX_T *p, U8 u1OnOff);
EXTERN void DramcPrintHWGatingStatus(DRAMC_CTX_T *p, U8 u1Channel);
#if (ENABLE_TX_TRACKING || TDQSCK_PRECALCULATION_FOR_DVFS)
EXTERN void FreqJumpRatioCalculation(DRAMC_CTX_T *p);
#endif
#if TDQSCK_PRECALCULATION_FOR_DVFS
EXTERN void DramcDQSPrecalculation_preset(DRAMC_CTX_T *p);
EXTERN void DramcDQSPrecalculation_enable(DRAMC_CTX_T *p);
#endif
EXTERN void DramcDQSOSCInit(void);
EXTERN DRAM_STATUS_T DramcDQSOSCAuto(DRAMC_CTX_T *p);
#if ENABLE_TX_TRACKING
EXTERN DRAM_STATUS_T DramcDQSOSCMR23(DRAMC_CTX_T *p);
EXTERN DRAM_STATUS_T DramcDQSOSCSetMR18MR19(DRAMC_CTX_T *p);
EXTERN DRAM_STATUS_T DramcDQSOSCShuSettings(DRAMC_CTX_T *p);
EXTERN void DramcHwDQSOSC(DRAMC_CTX_T *p);
EXTERN void Enable_TX_Tracking(DRAMC_CTX_T *p, U32 u4DramcShuOffset);
#endif

#if RDSEL_TRACKING_EN
EXTERN void Enable_RDSEL_Tracking(DRAMC_CTX_T *p, U32 u4DramcShuOffset);
EXTERN void RDSELRunTimeTracking_preset(DRAMC_CTX_T *p);
#endif
#ifdef HW_GATING
EXTERN void Enable_Gating_Tracking(DRAMC_CTX_T *p, U32 u4DramcShuOffset);
#endif
EXTERN void DramcImpedanceHWSaving(DRAMC_CTX_T *p);
EXTERN void DramcImpedanceTrackingEnable(DRAMC_CTX_T *p);
EXTERN void DramcRxInputDelayTrackingInit_Common(DRAMC_CTX_T *p);
EXTERN void DramcRxInputDelayTrackingHW(DRAMC_CTX_T *p);
EXTERN void DramcRxInputDelayTrackingInit_byFreq(DRAMC_CTX_T *p);


/*
 ****************************************************************************************
 ** dramc_utility.c
 ****************************************************************************************
 */
EXTERN U16 gddrphyfmeter_value;
#if FOR_DV_SIMULATION_USED
EXTERN U8 u1BroadcastOnOff;
#endif
#if (fcFOR_CHIP_ID == fcA60868)
EXTERN U8 u1EnterRuntime;
#endif
EXTERN U8 u1MaType;
EXTERN void TA2_Test_Run_Time_HW_Set_Column_Num(DRAMC_CTX_T * p);
EXTERN void TA2_Test_Run_Time_HW_Presetting(DRAMC_CTX_T * p, U32 len, TA2_RKSEL_TYPE_T rksel_mode);
EXTERN void TA2_Test_Run_Time_Pat_Setting(DRAMC_CTX_T *p, U8 PatSwitch);
EXTERN void TA2_Test_Run_Time_HW_Write(DRAMC_CTX_T * p, U8 u1Enable);
EXTERN U32 TA2_Test_Run_Time_HW_Status(DRAMC_CTX_T * p);
EXTERN void TA2_Test_Run_Time_HW(DRAMC_CTX_T * p);
EXTERN void vAutoRefreshSwitch(DRAMC_CTX_T *p, U8 option);
EXTERN void vSetRank(DRAMC_CTX_T *p, U8 ucRank);
EXTERN void vSetPHY2ChannelMapping(DRAMC_CTX_T *p, U8 u1Channel);
EXTERN VREF_CALIBRATION_ENABLE_T Get_Vref_Calibration_OnOff(DRAMC_CTX_T *p);
EXTERN u8 lp5heff_save_disable(DRAMC_CTX_T *p);
EXTERN void lp5heff_restore(DRAMC_CTX_T *p);
EXTERN u8 is_lp5_family(DRAMC_CTX_T *p);
EXTERN U32 GetDramcBroadcast(void);
EXTERN void CKEFixOnOff(DRAMC_CTX_T *p, U8 u1RankIdx, CKE_FIX_OPTION option,
		CKE_FIX_CHANNEL WriteChannelNUM);
EXTERN void DramcBackupRegisters(DRAMC_CTX_T *p, U32 *backup_addr, U32 backup_num);
EXTERN U8 u1GetRank(DRAMC_CTX_T *p);
EXTERN void vPrintCalibrationBasicInfo(DRAMC_CTX_T *p);
EXTERN void vPrintCalibrationBasicInfo_ForJV(DRAMC_CTX_T *p);
EXTERN U32 DramcEngine2Run(DRAMC_CTX_T *p, DRAM_TE_OP_T wr, U8 testaudpat);
EXTERN void DramcEngine2End(DRAMC_CTX_T *p);
EXTERN DRAM_STATUS_T DramcEngine2Init(DRAMC_CTX_T *p, U32 test2_1, U32 test2_2, U8 u1TestPat, U8 u1LoopCnt, U8 u1EnableUiShift);
EXTERN void DramcRestoreRegisters(DRAMC_CTX_T *p, U32 *restore_addr, U32 restore_num);
EXTERN DDR800_MODE_T vGet_DDR_Loop_Mode(DRAMC_CTX_T *p);
EXTERN u8 is_heff_mode(DRAMC_CTX_T *p);
EXTERN void DramcEngine2SetPat(DRAMC_CTX_T *p, U8 u1TestPat, U8 u1LoopCnt, U8 u1Len1Flag, U8 u1EnableUiShift);
EXTERN void DramcSetRankEngine2(DRAMC_CTX_T *p, U8 u1RankSel);
EXTERN U16 GetFreqBySel(DRAMC_CTX_T *p, DRAM_PLL_FREQ_SEL_T sel);
EXTERN U8 GetEyeScanEnable(DRAMC_CTX_T * p, U8 get_type);
EXTERN U8 vGetPHY2ChannelMapping(DRAMC_CTX_T *p);
EXTERN DUTY_CALIBRATION_T Get_Duty_Calibration_Mode(DRAMC_CTX_T *p);
EXTERN void DDRPhyFreqSel(DRAMC_CTX_T *p, DRAM_PLL_FREQ_SEL_T sel);
EXTERN DRAM_DFS_SRAM_SHU_T vGet_Current_ShuLevel(DRAMC_CTX_T *p);
EXTERN void vSetChannelNumber(DRAMC_CTX_T *p);
EXTERN void vSetRankNumber(DRAMC_CTX_T *p);
EXTERN void vSetFSPNumber(DRAMC_CTX_T *p);
EXTERN void vCKERankCtrl(DRAMC_CTX_T *p, CKE_CTRL_MODE_T CKECtrlMode);
EXTERN DRAM_PLL_FREQ_SEL_T vGet_PLL_FreqSel(DRAMC_CTX_T *p);
EXTERN void vSet_PLL_FreqSel(DRAMC_CTX_T *p, DRAM_PLL_FREQ_SEL_T sel);
EXTERN void Temp_TA2_Test_After_K(DRAMC_CTX_T * p);
EXTERN void DramcBroadcastOnOff(U32 bOnOff);
EXTERN DIV_MODE_T vGet_Div_Mode(DRAMC_CTX_T *p);
EXTERN void DramcMRWriteFldMsk(DRAMC_CTX_T *p, U8 mr_idx, U8 listValue, U8 msk, U8 UpdateMode);
EXTERN void DramcMRWriteFldAlign(DRAMC_CTX_T *p, U8 mr_idx, U8 value, U32 mr_fld, U8 UpdateMode);
EXTERN void DramcModeRegReadByRank(DRAMC_CTX_T *p, U8 u1Rank, U8 u1MRIdx, U16 *u2pValue);
EXTERN void DramcModeRegRead(DRAMC_CTX_T *p, U8 u1MRIdx, U16 *u1pValue);
EXTERN void DramcModeRegWriteByRank(DRAMC_CTX_T *p, U8 u1Rank, U8 u1MRIdx, U8 u1Value);
EXTERN void SetDramModeRegForWriteDBIOnOff(DRAMC_CTX_T *p, U8 u1fsp, U8 onoff);
EXTERN void SetDramModeRegForReadDBIOnOff(DRAMC_CTX_T *p, U8 u1fsp, U8 onoff);
#if MRW_CHECK_ONLY
EXTERN void vPrintFinalModeRegisterSetting(DRAMC_CTX_T *p);
#endif
#if MRW_BACKUP
EXTERN U8 DramcMRWriteBackup(DRAMC_CTX_T *p, U8 u1MRIdx, U8 u1Rank);
#endif
#if QT_GUI_Tool
EXTERN void TA2_Test_Run_Time_SW_Presetting(DRAMC_CTX_T *p, U32 test2_1, U32 test2_2, U8 testaudpat, U8 log2loopcount);
EXTERN U32 TestEngineCompare(DRAMC_CTX_T *p);
#endif
EXTERN void vSet_Div_Mode(DRAMC_CTX_T *p, DIV_MODE_T eMode);
EXTERN void vSet_Current_ShuLevel(DRAMC_CTX_T *p, DRAM_DFS_SRAM_SHU_T u1ShuIndex);
EXTERN void GetPhyPllFrequency(DRAMC_CTX_T *p);
EXTERN void DramcWriteDBIOnOff(DRAMC_CTX_T *p, U8 onoff);
EXTERN void DramcReadDBIOnOff(DRAMC_CTX_T *p, U8 onoff);
EXTERN void CheckDramcWBR(U32 u4address);
EXTERN void DramcModeRegWriteByRank_RTMRW(DRAMC_CTX_T *p, U8 *u1Rank, U8 *u1MRIdx, U8 *u1Value, U8 u1Len);
#if PRINT_CALIBRATION_SUMMARY
EXTERN void vPrintCalibrationResult(DRAMC_CTX_T *p);
#endif
EXTERN int dramc_complex_mem_test (unsigned int start, unsigned int len);
EXTERN U16 DDRPhyFMeter(void);
#ifdef DDR_INIT_TIME_PROFILING
void TimeProfileBegin(void);
UINT32 TimeProfileEnd(void);
#endif



/*
 ****************************************************************************************
 ** Hal_IO.cpp
 ****************************************************************************************
 */
#ifdef DUMP_INIT_RG_LOG_TO_DE
EXTERN U8  gDUMP_INIT_RG_LOG_TO_DE_RG_log_flag;
#endif



/*
 ****************************************************************************************
 ** dramc_utility.cpp
 ****************************************************************************************
 */
#if (QT_GUI_Tool == 1)
EXTERN MCK_TO_UI_SHIFT_T u1Lp5MCK2WCKUI_DivShift(DRAMC_CTX_T *p);
#endif


/*
 ****************************************************************************************
 ** dramc_debug.cpp
 ****************************************************************************************
 */
extern void HQA_Log_Message_for_Report(DRAMC_CTX_T *p, U8 u1ChannelIdx, U8 u1RankIdx, U32 who_am_I, U8 *main_str, U8 *main_str2, U8 byte_bit_idx, S32 value1, U8 *ans_str);


/*
 ****************************************************************************************
 ** dramc_utility_QT.cpp
 ****************************************************************************************
 */
#if (QT_GUI_Tool == 1)
EXTERN void QT_DRAMCTX_INIT(DRAMC_CTX_T *p);
EXTERN DRAM_STATUS_T DramcDDRPHYInit_FPGA_A60868(DRAMC_CTX_T *p);
EXTERN DRAM_STATUS_T DramcDDRPHYInit_LP5_FPGA_A60868(DRAMC_CTX_T *p);
EXTERN void TA2_Stress_Test(DRAMC_CTX_T *p);
EXTERN void TA2_Stress_Test_2(DRAMC_CTX_T *p);
EXTERN U32 QT_TestEngineCompare(DRAMC_CTX_T *p);
EXTERN void Write_Byte_Counter_Begin(DRAMC_CTX_T *p);
EXTERN U32 Write_Byte_Counter_End(DRAMC_CTX_T *p);
EXTERN void DDRPhyFMeter_Init(DRAMC_CTX_T *p);
EXTERN U32 DDRPhyFreqMeter(void);
#endif


/*
 ****************************************************************************************
 ** fake_engine.c
 ****************************************************************************************
 */


/*
 ****************************************************************************************
 ** low_power_test.c
 ****************************************************************************************
 */
EXTERN int global_which_test;
EXTERN void EnableDramcPhyDCMShuffle(DRAMC_CTX_T *p, bool bEn, U32 u4DramcShuOffset, U32 u4DDRPhyShuOffset);
EXTERN void Enter_Precharge_All(DRAMC_CTX_T *p);
EXTERN void EnableDramcPhyDCM(DRAMC_CTX_T *p, bool bEn);
EXTERN DRAM_STATUS_T CheckGoldenSetting(DRAMC_CTX_T *p);
EXTERN void Low_Power_Scenarios_Test(DRAMC_CTX_T *p);
#if ENABLE_DDR800_OPEN_LOOP_MODE_OPTION
void DDR800semiPowerSavingOn(DRAMC_CTX_T *p, U8 next_shu_level, U8 u1OnOff);
#endif

#define LOW_POWER_SCENARIO_PRECHARGE_ALL 3 //idle(all bank refresh)
#define LOW_POWER_SCENARIO_S1 5
#define LOW_POWER_SCENARIO_S0 6
#define LOW_POWER_SCENARIO_PASR 7
#define LOW_POWER_SCENARIO_ALL 8
#define LOW_POWER_SCENARIO_FAKE_ENGINE_READ 9
#define LOW_POWER_SCENARIO_FAKE_ENGINE_WRITE 10
#define LOW_POWER_SCENARIO_ONLY_SELF_REFRESH 12
#define LOW_POWER_SCENARIO_HW_AUTO_SAVE_S0 13
#define LOW_POWER_SCENARIO_HW_AUTO_SAVE_S0_METHOD_2 14
#define LOW_POWER_SCENARIO_PASR_1BANK 15
#define LOW_POWER_SCENARIO_PASR_2BANK 16
#define LOW_POWER_SCENARIO_PASR_4BANK 17
#define LOW_POWER_SCENARIO_PASR_8BANK 18
#define LOW_POWER_SCENARIO_FAKE_ENGINE_BW 19
#define LOW_POWER_SCENARIO_FAKE_ENGINE_READ_WRITE 21
#define AUTO_REFRESH_RESERVE_TEST 22
/*
 ****************************************************************************************
 ** low_power_test.c
 ****************************************************************************************
 */
EXTERN U8 u1StopMiniStress;
EXTERN void Ett_Mini_Strss_Test(DRAMC_CTX_T *p);


/*
 ****************************************************************************************
 ** LP4_dram_init.c
 ****************************************************************************************
 */
EXTERN void CKE_FIX_ON(DRAMC_CTX_T *p, U8 EN, U8 rank);
EXTERN void LP4_UpdateInitialSettings(DRAMC_CTX_T *p);
EXTERN void LP4_DRAM_INIT(DRAMC_CTX_T *p);


/*
 ****************************************************************************************
 ** LP5_dram_init.c
 ****************************************************************************************
 */
EXTERN void LP5_UpdateInitialSettings(DRAMC_CTX_T *p);
EXTERN void LP5_DRAM_INIT(DRAMC_CTX_T *p);


/*
 ****************************************************************************************
 ** system_init.c
 ****************************************************************************************
 */
#if (fcFOR_CHIP_ID == fcA60868)
EXTERN void syspll_init(DRAMC_CTX_T *p);
#endif


/*
 ****************************************************************************************
 ** dramc_utility_QT.cpp
 ****************************************************************************************
 */
#if (QT_GUI_Tool == 1)
EXTERN U8 ucDramRegRead_1(U32 reg_addr, U32 *reg_data);
EXTERN U8 ucDramRegWrite_1(U32 reg_addr, U32 reg_data);
#endif


/*
 ****************************************************************************************
 ** svsim_dummy.c
 ****************************************************************************************
 */
#if (FOR_DV_SIMULATION_USED == 0)
#define delay_us(x)
#define delay_ns(x)
#define mysetscope()
#define broadcast_on()
#define broadcast_off()
#define timestamp_show()
#define build_api_initial()
#define conf_to_sram_sudo(...)
#endif

/*
 ****************************************************************************************
 ** RS232.cpp
 ****************************************************************************************
 */
#if (QT_GUI_Tool == 1)
EXTERN U8 ucDramSetReg_1(U32 address, U32 *data, U16 count);
EXTERN U8 ucDramGetReg_1(U32 address, U32 *data, U16 count);
#endif


/*
 ****************************************************************************************
 ** ett_test.c
 ****************************************************************************************
 */
extern int hqa_vmddr_voltage, hqa_vmddr_class;


#endif //_INT_GLOBAL_H
