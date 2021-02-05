/* SPDX-License-Identifier: BSD-3-Clause */

#include "dramc_top.h"

#include "dramc_actiming.h"
#include "dramc_common.h"
#include "dramc_dv_init.h"
#include "dramc_int_global.h"
#include <emi.h>
#include "x_hal_io.h"
#include "sv_c_data_traffic.h"
#include "dramc_pi_api.h"
#include <soc/dramc_param.h>
#include <soc/emi.h>

DRAMC_CTX_T dram_ctx_chb;

#if (FOR_DV_SIMULATION_USED == 1)
U8 gu1BroadcastIsLP4 = TRUE;
#endif

bool gAndroid_DVFS_en = TRUE;
bool gUpdateHighestFreq = FALSE;

#define DV_SIMULATION_BYTEMODE 0
#define DV_SIMULATION_LP5_TRAINING_MODE1 1
#define DV_SIMULATION_LP5_CBT_PHASH_R 1

#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
   SAVE_TIME_FOR_CALIBRATION_T SavetimeData;
#endif

U8 gHQA_Test_Freq_Vcore_Level = 0;	// 0: only 1 freq  , others are multi freq	1: low vcore 2: high vcore

#define ENABLE_DRAM_SINGLE_FREQ_SELECT 0xFF  // 0xFF=all freq by gFreqTbl. The 0x"X" != 0xFF for single freq by gFreqTbl index, ex: 0x3 for DDR3733

DRAM_DFS_FREQUENCY_TABLE_T gFreqTbl[DRAM_DFS_SHUFFLE_MAX] = {
	{LP4_DDR3200 /*0*/, DIV8_MODE, SRAM_SHU1, DUTY_LAST_K,	VREF_CALI_OFF,	CLOSE_LOOP_MODE},  // highest freq of term group (3733) must k first.
	{LP4_DDR4266 /*1*/, DIV8_MODE, SRAM_SHU0, DUTY_NEED_K,	VREF_CALI_ON,	CLOSE_LOOP_MODE},  // highest freq of term group (3733) must k first.
	{LP4_DDR800  /*2*/, DIV4_MODE, SRAM_SHU6, DUTY_DEFAULT, VREF_CALI_OFF,	SEMI_OPEN_LOOP_MODE},  //Darren: DDR1600 for MRW (DramcModeRegInit_LP4 and CBT)
	{LP4_DDR1866 /*3*/, DIV8_MODE, SRAM_SHU3, DUTY_LAST_K,	VREF_CALI_OFF,	CLOSE_LOOP_MODE},  // highest freq of unterm group (2400) must k first.
	{LP4_DDR1200 /*4*/, DIV8_MODE, SRAM_SHU5, DUTY_LAST_K,	VREF_CALI_OFF,	CLOSE_LOOP_MODE},  // highest freq of unterm group (2400) must k first.
	{LP4_DDR2400 /*5*/, DIV8_MODE, SRAM_SHU2, DUTY_NEED_K,	VREF_CALI_ON,	CLOSE_LOOP_MODE},  // highest freq of unterm group (2400) must k first.
	{LP4_DDR1600 /*6*/, DIV8_MODE, SRAM_SHU4, DUTY_DEFAULT, VREF_CALI_ON,  CLOSE_LOOP_MODE},  //Darren: DDR1600 for MRW (DramcModeRegInit_LP4 and CBT)
};


DRAMC_CTX_T DramCtx_LPDDR4 =
{
	CHANNEL_SINGLE, // Channel number
	CHANNEL_A,			// DRAM_CHANNEL
	RANK_DUAL,		  //DRAM_RANK_NUMBER_T
	RANK_0, 			  //DRAM_RANK_T

#ifdef MTK_FIXDDR1600_SUPPORT
	LP4_DDR1600,
#else
#if __FLASH_TOOL_DA__
	LP4_DDR1600,
#else
#if (DV_SIMULATION_LP4 == 1)
	LP4_DDR1600,
#else
	LP5_DDR3200,
#endif
#endif
#endif
	DRAM_DFS_SHUFFLE_1,
#if DV_SIMULATION_LP4
	TYPE_LPDDR4X,		 // DRAM_DRAM_TYPE_T
#else
	TYPE_LPDDR5,
#endif
	FSP_0 , //// DRAM Fast switch point type, only for LP4, useless in LP3
	ODT_OFF,
	{CBT_NORMAL_MODE, CBT_NORMAL_MODE},  // bring up LP4X rank0 & rank1 use normal mode
#if ENABLE_READ_DBI
	{DBI_OFF,DBI_ON},  //read DBI
#else
	{DBI_OFF,DBI_OFF}, //read DBI
#endif
#if ENABLE_WRITE_DBI
	{DBI_OFF,DBI_ON},  // write DBI
#else
	{DBI_OFF,DBI_OFF},	// write DBI
#endif
	DATA_WIDTH_16BIT,	  // DRAM_DATA_WIDTH_T
	DEFAULT_TEST2_1_CAL,	// test2_1;
	DEFAULT_TEST2_2_CAL,	// test2_2;
#if ENABLE_K_WITH_WORST_SI_UI_SHIFT
	TEST_WORST_SI_PATTERN,	  // test_pattern;
#else
	TEST_XTALK_PATTERN,
#endif
#if (DV_SIMULATION_LP4 == 1)
	800,				  // frequency
	800,				  // freqGroup
#else
	1600,
	1600,
#endif
	0x88, //vendor_id initial value
	REVISION_ID_MAGIC,
	0xff, //density
	{0,0},
	0,	// u2num_dlycell_perT;
	270,  // u2DelayCellTimex100;

#if PRINT_CALIBRATION_SUMMARY
	//aru4CalResultFlag[CHANNEL_NUM][RANK_MAX]
	{{0,}},
	//aru4CalExecuteFlag[CHANNEL_NUM][RANK_MAX]
	{{0,}},
	1,
	0,
#endif
	{0}, //BOOL arfgWriteLevelingInitShif;
#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
	FALSE, //femmc_Ready
	0,
	0,
	0,
	&SavetimeData,
#endif
	&gFreqTbl[DRAM_DFS_SHUFFLE_MAX-1], // default is DDR1600 1:8 mode
	DRAM_DFS_REG_SHU0,
	TRAINING_MODE2,
	CBT_PHASE_RISING,
	0,	//new CBT pattern
	PHYPLL_MODE,
	DBI_OFF,
	FSP_MAX,
	PINMUX_EMCP,
	{DISABLE,DISABLE},	// disable 10GB
	0,
};



#if defined(DDR_INIT_TIME_PROFILING) || (__ETT__ && SUPPORT_SAVE_TIME_FOR_CALIBRATION)
DRAMC_CTX_T gTimeProfilingDramCtx;
U8 gtime_profiling_flag = 0;
#endif

void vSetVcoreByFreq(DRAMC_CTX_T *p)
{
#if 1//def MTK_PMIC_MT6359
#if (FOR_DV_SIMULATION_USED==0 && SW_CHANGE_FOR_SIMULATION==0)
#if __FLASH_TOOL_DA__
	dramc_set_vcore_voltage(725000);
#else
	unsigned int vio18, vcore, vdram, vddq, vmddr;

	vio18 = vcore = vdram = vddq = vmddr = 0;

#if __ETT__
	hqa_set_voltage_by_freq(p, &vio18, &vcore, &vdram, &vddq, &vmddr);
#elif 0//defined(VCORE_BIN)
	switch (vGet_Current_ShuLevel(p)) {
	case SRAM_SHU0:  //4266
	#ifdef VOLTAGE_SEL
		vcore = vcore_voltage_select(KSHU0);
		if (!vcore)
	#endif
		vcore = get_vcore_uv_table(0);
		break;
	case SRAM_SHU1:   //3200
	#ifdef VOLTAGE_SEL
		vcore = vcore_voltage_select(KSHU1);
		if (!vcore)
	#endif
		vcore = (get_vcore_uv_table(0) + get_vcore_uv_table(1)) >> 1;
		break;
	case SRAM_SHU2:  //2400
	case SRAM_SHU3:  //1866
	#ifdef VOLTAGE_SEL
		vcore = vcore_voltage_select(KSHU2);
		if (!vcore)
	#endif
		vcore = (get_vcore_uv_table(0) + get_vcore_uv_table(2)) >> 1;
		break;
	case SRAM_SHU4:  //1600
	case SRAM_SHU5:  //1200
	case SRAM_SHU6:  //800
	#ifdef VOLTAGE_SEL
		vcore = vcore_voltage_select(KSHU4);
		if (!vcore)
	#endif
		vcore = (get_vcore_uv_table(0) + get_vcore_uv_table(3)) >> 1;
		break;
	}
#else
	switch (vGet_Current_ShuLevel(p)) {
	case SRAM_SHU0: // 4266
	#ifdef VOLTAGE_SEL
		vcore = vcore_voltage_select(KSHU0);
	#else
		vcore = SEL_PREFIX_VCORE(LP4, KSHU0);
	#endif
		break;
	case SRAM_SHU1: // 3200
	#ifdef VOLTAGE_SEL
		vcore = vcore_voltage_select(KSHU1);
	#else
		vcore = SEL_PREFIX_VCORE(LP4, KSHU1);
	#endif
		break;
	case SRAM_SHU2: // 2400
	case SRAM_SHU3: //1866
	#ifdef VOLTAGE_SEL
		vcore = vcore_voltage_select(KSHU2);
	#else
		vcore = SEL_PREFIX_VCORE(LP4, KSHU2);
	#endif
		break;
	case SRAM_SHU4: //1600
	case SRAM_SHU5: //1200
	case SRAM_SHU6: //800
	#ifdef VOLTAGE_SEL
		vcore = vcore_voltage_select(KSHU4);
	#else
		vcore = SEL_PREFIX_VCORE(LP4, KSHU4);
	#endif
		break;
	default:
		return;
	}
#endif

	if (vcore)
		dramc_set_vcore_voltage(vcore);

#if defined(DRAM_HQA)
	vio18 = SEL_VIO18;
	if (vio18)
		dramc_set_vio18_voltage(vio18);

	vdram = SEL_PREFIX_VDRAM(LP4);
	if (vdram)
		dramc_set_vdram_voltage(p->dram_type, vdram);

	vddq = SEL_PREFIX_VDDQ;
	if (vddq)
		dramc_set_vddq_voltage(p->dram_type, vddq);

	vmddr = SEL_PREFIX_VMDDR;
	if (vmddr)
		dramc_set_vmddr_voltage(vmddr);
#endif

#ifdef FOR_HQA_REPORT_USED
	switch (vGet_Current_ShuLevel(p)) {
		case SRAM_SHU0: //3733
		case SRAM_SHU1: //3200
	case SRAM_SHU2: //2400
	case SRAM_SHU3: //1866
	case SRAM_SHU4: //1600
	case SRAM_SHU5: //1200
	case SRAM_SHU6: //800
		gHQA_Test_Freq_Vcore_Level = 0; //only 1 freq
			break;
		default:
			print("[HQA] undefined shuffle level for Vcore (SHU%d)\r\n", vGet_Current_ShuLevel(p));
#if __ETT__
			while(1);
#endif
			break;
	}
#endif

#ifndef DDR_INIT_TIME_PROFILING
	print("Read voltage for %d, %d\n", p->frequency, vGet_Current_ShuLevel(p));
	print("Vio18 = %d\n", dramc_get_vio18_voltage());
	print("Vcore = %d\n", dramc_get_vcore_voltage());
	print("Vdram = %d\n", dramc_get_vdram_voltage(p->dram_type));
	print("Vddq = %d\n", dramc_get_vddq_voltage(p->dram_type));
	print("Vmddr = %d\n", dramc_get_vmddr_voltage());
#endif
#endif
#endif
#endif
}

U32 vGetVoltage(DRAMC_CTX_T *p, U32 get_voltage_type)
{
#if (defined(DRAM_HQA) || __ETT__) && (FOR_DV_SIMULATION_USED == 0)
	if (get_voltage_type==0)
		return dramc_get_vcore_voltage();

	if (get_voltage_type==1)
		return dramc_get_vdram_voltage(p->dram_type);

	if (get_voltage_type==2)
		return dramc_get_vddq_voltage(p->dram_type);

	if (get_voltage_type==3)
		   return dramc_get_vio18_voltage();
	if (get_voltage_type==4)
			 return dramc_get_vmddr_voltage();

#endif

	return 0;
}

#ifdef FOR_HQA_TEST_USED
VCORE_DELAYCELL_T gVcoreDelayCellTable[49]={	{500000, 512},
												{506250, 496},
												{512500, 482},
												{518750, 469},
												{525000, 457},
												{531250, 445},
												{537500, 434},
												{543750, 423},
												{550000, 412},
												{556250, 402},
												{562500, 393},
												{568750, 384},
												{575000, 377},
												{581250, 369},
												{587500, 362},
												{593750, 355},
												{600000, 348},
												{606250, 341},
												{612500, 335},
												{618750, 328},
												{625000, 322},
												{631250, 317},
												{637500, 312},
												{643750, 307},
												{650000, 302},
												{656250, 297},
												{662500, 292},
												{668750, 288},
												{675000, 284},
												{681250, 280},
												{687500, 276},
												{693750, 272},
												{700000, 269},
												{706250, 265},
												{712500, 262},
												{718750, 258},
												{725000, 255},
												{731250, 252},
												{737500, 249},
												{743750, 246},
												{750000, 243},
												{756250, 241},
												{762500, 238},
												{768750, 236},
												{775000, 233},
												{781250, 231},
												{787500, 229},
												{793750, 227},
												{800000, 225},
												//{825000, 718},
												//{831250, 717},
												//{837500, 715},
												//{843750, 713},
												//{850000, 708},
												//{856250, 705},
												//{862500, 702},
												//{868750, 700},
												//{875000, 698}
												};

void GetVcoreDelayCellTimeFromTable(DRAMC_CTX_T *p)
{
	U32 channel_i, i;
	U32 get_vcore = 0;
	U16 u2gdelay_cell_ps = 0;
	U8 u1delay_cell_cnt = 0;
	VCORE_DELAYCELL_T *pVcoreDelayCellTable;

#if (defined(DRAM_HQA) || __ETT__) && (FOR_DV_SIMULATION_USED == 0)
	#if 1//__Petrus_TO_BE_PORTING__
	get_vcore = dramc_get_vcore_voltage();
	#endif
#endif

	pVcoreDelayCellTable = (VCORE_DELAYCELL_T *)gVcoreDelayCellTable;
	u1delay_cell_cnt = sizeof(gVcoreDelayCellTable)/sizeof(gVcoreDelayCellTable[0]);

	for(i=0; i<u1delay_cell_cnt; i++)
	{
		if (get_vcore >= pVcoreDelayCellTable[i].u2Vcore)
		{
			u2gdelay_cell_ps = pVcoreDelayCellTable[i].u2DelayCell;
		}
	}

	mcSHOW_DBG_MSG(("[GetVcoreDelayCellTimeFromTable(%d)] VCore=%d(x100), DelayCell=%d(x100)\n", u1delay_cell_cnt, get_vcore, u2gdelay_cell_ps));

	for(channel_i=CHANNEL_A; channel_i < p->support_channel_num; channel_i++)
	{
		u2gdelay_cell_ps_all[get_shuffleIndex_by_Freq(p)][channel_i] = u2gdelay_cell_ps;
		u4gVcore[get_shuffleIndex_by_Freq(p)] = get_vcore/1000;
	}
}
#endif

///TODO: wait for porting +++
#ifdef FIRST_BRING_UP
void Test_Broadcast_Feature(DRAMC_CTX_T *p)
{
	U32 u4RegBackupAddress[] =
	{
		(DRAMC_REG_SHURK_SELPH_DQ2),
		(DRAMC_REG_SHURK_SELPH_DQ2 + SHIFT_TO_CHB_ADDR),

		(DDRPHY_REG_SHU_RK_B0_DQ0),
		(DDRPHY_REG_SHU_RK_B0_DQ0 + SHIFT_TO_CHB_ADDR),
	};
	U32 read_value;
	U32 backup_broadcast;

	backup_broadcast = GetDramcBroadcast();

	DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);

	DramcBackupRegisters(p, u4RegBackupAddress, sizeof(u4RegBackupAddress) / sizeof(U32));

	DramcBroadcastOnOff(DRAMC_BROADCAST_ON);

	vIO32Write4B(DRAMC_REG_SHURK_SELPH_DQ2, 0xA55A00FF);
	vIO32Write4B(DDRPHY_REG_SHU_RK_B0_DQ0, 0xA55A00FF);

	read_value = u4IO32Read4B(DRAMC_REG_SHURK_SELPH_DQ2 + SHIFT_TO_CHB_ADDR);
	if (read_value != 0xA55A00FF)
	{
		mcSHOW_ERR_MSG(("Check Erro! Broad Cast CHA RG to CHB Fail!!\n"));
		while (1);
	}

	read_value = u4IO32Read4B(DDRPHY_REG_SHU_RK_B0_DQ0 + SHIFT_TO_CHB_ADDR);
	if (read_value != 0xA55A00FF)
	{
		mcSHOW_ERR_MSG(("Check Erro! Broad Cast CHA RG to CHB Fail!!\n"));
		while (1);
	}

	DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);

	DramcRestoreRegisters(p, u4RegBackupAddress, sizeof(u4RegBackupAddress) / sizeof(U32));

	DramcBroadcastOnOff(backup_broadcast);
}
#endif

#ifdef ENABLE_MIOCK_JMETER
U8 gPRE_MIOCK_JMETER_HQA_USED_flag=0;

static void Set_PRE_MIOCK_JMETER_HQA_USED_flag(U8 value)
{
	gPRE_MIOCK_JMETER_HQA_USED_flag = value;
}

void Get_RX_DelayCell(DRAMC_CTX_T *p)
{
#if defined(FOR_HQA_REPORT_USED) && (FOR_DV_SIMULATION_USED==0) && (SW_CHANGE_FOR_SIMULATION==0)
		if (gHQALOG_RX_delay_cell_ps_075V == 0)
		{
#if __ETT__
			mcSHOW_DBG_MSG(("RX delay cell calibration (%d):\n", hqa_vmddr_class));

			switch (hqa_vmddr_class)
			{
				case 1:
					dramc_set_vcore_voltage(_SEL_PREFIX(VMDDR, HV, LP4));
					break;
				case 2:
					dramc_set_vcore_voltage(_SEL_PREFIX(VMDDR, NV, LP4));
					break;
				case 3:
					dramc_set_vcore_voltage(_SEL_PREFIX(VMDDR, LV, LP4));
					break;
			}
#else
			// set vcore to RX used 0.75V
			dramc_set_vcore_voltage(SEL_PREFIX_VMDDR);	//set vmddr voltage to vcore to K RX delay cell
#endif

			DramcMiockJmeter(p);

			gHQALOG_RX_delay_cell_ps_075V = u2gdelay_cell_ps;

			// set vocre back
			vSetVcoreByFreq(p);
		}
#endif
}

void PRE_MIOCK_JMETER_HQA_USED(DRAMC_CTX_T *p)
{
	U32 backup_freq_sel, backup_channel;
	U32 channel_idx;

#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
	if(p->femmc_Ready==1)
	{
		for(channel_idx=CHANNEL_A; channel_idx<p->support_channel_num; channel_idx++)
		{
			//for (shuffleIdx = DRAM_DFS_SHUFFLE_1; shuffleIdx < DRAM_DFS_SHUFFLE_MAX; shuffleIdx++)
			{
				u2g_num_dlycell_perT_all[p->shu_type][channel_idx] = p->pSavetimeData->u2num_dlycell_perT;
				u2gdelay_cell_ps_all[p->shu_type][channel_idx] = p->pSavetimeData->u2DelayCellTimex100;
			}
		}

		p->u2num_dlycell_perT = p->pSavetimeData->u2num_dlycell_perT;
		p->u2DelayCellTimex100 = p->pSavetimeData->u2DelayCellTimex100;
		return;
	}
#endif


	backup_freq_sel = vGet_PLL_FreqSel(p);
	backup_channel = p->channel;

	mcSHOW_DBG_MSG3(("[JMETER_HQA]\n"));
	Set_PRE_MIOCK_JMETER_HQA_USED_flag(1);

	vSetPHY2ChannelMapping(p, CHANNEL_A);

	DramcMiockJmeterHQA(p);

	vSetPHY2ChannelMapping(p, backup_channel);

	Set_PRE_MIOCK_JMETER_HQA_USED_flag(0);

	vSet_PLL_FreqSel(p, backup_freq_sel);
}
#endif


#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
#if !EMMC_READY
u32 g_dram_save_time_init_done[DRAM_DFS_SHUFFLE_MAX] = {0};
SAVE_TIME_FOR_CALIBRATION_T SaveTimeDataByShuffle[DRAM_DFS_SHUFFLE_MAX];
#endif
static DRAM_STATUS_T DramcSave_Time_For_Cal_End(DRAMC_CTX_T *p)
{
	if (!u1IsLP4Family(p->dram_type))
		return DRAM_FAIL;

	if (p->femmc_Ready == 0)
	{
		#if EMMC_READY
		write_offline_dram_calibration_data(p->shu_type, p->pSavetimeData);
		mcSHOW_DBG_MSG(("[FAST_K] Save calibration result to emmc\n"));
		#else
		g_dram_save_time_init_done[p->shu_type] = 1;
		memcpy(&(SaveTimeDataByShuffle[p->shu_type]), p->pSavetimeData, sizeof(SAVE_TIME_FOR_CALIBRATION_T));
		mcSHOW_DBG_MSG(("[FAST_K] Save calibration result to SW memory\n"));
		#endif
	}
	else
	{
		mcSHOW_DBG_MSG(("[FAST_K] Bypass saving calibration result to emmc\n"));
	}

	return DRAM_OK;
}

static DRAM_STATUS_T DramcSave_Time_For_Cal_Init(DRAMC_CTX_T *p)
{
	if (!u1IsLP4Family(p->dram_type))
		return DRAM_FAIL;

	if (doe_get_config("fullk"))
	return DRAM_FAIL;

	// Parepare fask k data
	#if EMMC_READY
	// scy: only need to read emmc one time for each boot-up
	//if (g_dram_save_time_init_done == 1)
	//	  return DRAM_OK;
	//else
	//	  g_dram_save_time_init_done = 1;
	if (read_offline_dram_calibration_data(p->shu_type, p->pSavetimeData) < 0)
		{
		p->femmc_Ready = 0;
		memset(p->pSavetimeData, 0, sizeof(SAVE_TIME_FOR_CALIBRATION_T));
		}
		else
		{
		p->femmc_Ready = 1;
		}

	#else //EMMC is not avaliable, load off-line data

	if (g_dram_save_time_init_done[p->shu_type] == 0)
	{
		p->femmc_Ready = 0;
		memset(p->pSavetimeData, 0, sizeof(SAVE_TIME_FOR_CALIBRATION_T));
	}
	else
	{
		memcpy(p->pSavetimeData, &(SaveTimeDataByShuffle[p->shu_type]), sizeof(SAVE_TIME_FOR_CALIBRATION_T));
		p->femmc_Ready = 1;
	}
	#endif

	if (p->femmc_Ready == 1)
	{
		if (p->frequency < 1600)
		{	// freq < 1600, TX and RX tracking are disable. Therefore, bypass calibration.
			p->Bypass_RDDQC = 1;
			p->Bypass_RXWINDOW = 1;
			p->Bypass_TXWINDOW = 1;
	}
	else
	{
			p->Bypass_RDDQC = 1;
			p->Bypass_RXWINDOW = !ENABLE_RX_TRACKING;
			p->Bypass_TXWINDOW = 0;
	}

#if RUNTIME_SHMOO_RELEATED_FUNCTION
		p->Bypass_RDDQC = 1;
		p->Bypass_RXWINDOW = 1;
		p->Bypass_TXWINDOW = 1;
#endif
	}

#if EMMC_READY
	mcSHOW_DBG_MSG(("[FAST_K] DramcSave_Time_For_Cal_Init SHU%d, femmc_Ready=%d\n", p->shu_type, p->femmc_Ready));
#else
	mcSHOW_DBG_MSG(("[FAST_K] DramcSave_Time_For_Cal_Init SHU%d, Init_done=%d, femmc_Ready=%d\n", p->shu_type, g_dram_save_time_init_done[p->shu_type], p->femmc_Ready));
#endif
	mcSHOW_DBG_MSG(("[FAST_K] Bypass_RDDQC %d, Bypass_RXWINDOW=%d, Bypass_TXWINDOW=%d\n", p->Bypass_RDDQC, p->Bypass_RXWINDOW, p->Bypass_TXWINDOW));

	return DRAM_OK;
}
#endif

#if ENABLE_RANK_NUMBER_AUTO_DETECTION
static void DramRankNumberDetection(DRAMC_CTX_T *p)
{
	U8 u1RankBak;

	u1RankBak = u1GetRank(p);  // backup current rank setting

	vSetPHY2ChannelMapping(p, CHANNEL_A); // when switching channel, must update PHY to Channel Mapping
	vSetRank(p, RANK_1);

	if (DramcWriteLeveling(p, AUTOK_ON, PI_BASED) == DRAM_OK)
	{
		p->support_rank_num = RANK_DUAL;
		vIO32WriteFldAlign(DRAMC_REG_SA_RESERVE, 0, SA_RESERVE_SINGLE_RANK);  //keep support_rank_num to reserved rg
	}
	else
	{
		p->support_rank_num = RANK_SINGLE;
		vIO32WriteFldAlign(DRAMC_REG_SA_RESERVE, 1, SA_RESERVE_SINGLE_RANK);  //keep support_rank_num to reserved rg
	}
	mcSHOW_DBG_MSG(("[RankNumberDetection] %d\n", p->support_rank_num));

	vSetRank(p, u1RankBak);  // restore rank setting
}
#endif

static void UpdateGlobal10GBEnVariables(DRAMC_CTX_T *p)
{
	p->u110GBEn[RANK_0] = (get_row_width_by_emi(RANK_0) >= 18) ? ENABLE : DISABLE;
	p->u110GBEn[RANK_1] = (get_row_width_by_emi(RANK_1) >= 18) ? ENABLE : DISABLE;
	//mcSHOW_DBG_MSG(("[10GBEn] RANK0=%d, RANK1=%d\n", p->u110GBEn[RANK_0], p->u110GBEn[RANK_1]));
}

void vCalibration_Flow_For_MDL(DRAMC_CTX_T *p)
{
	U8 u1RankMax;
	S8 s1RankIdx;

#if GATING_ADJUST_TXDLY_FOR_TRACKING
	DramcRxdqsGatingPreProcess(p);
#endif

	if (p->support_rank_num == RANK_DUAL)
		u1RankMax = RANK_MAX;
	else
		u1RankMax = RANK_1;

	for (s1RankIdx = RANK_0; s1RankIdx < u1RankMax; s1RankIdx++)
	{
		vSetRank(p, s1RankIdx);

		vAutoRefreshSwitch(p, ENABLE); //when doing gating, RX and TX calibration, auto refresh should be enable
		dramc_rx_dqs_gating_cal(p, AUTOK_OFF, 0);
		DramcRxWindowPerbitCal(p, PATTERN_RDDQC, NULL, AUTOK_OFF);

#if MRW_CHECK_ONLY
		mcSHOW_MRW_MSG(("\n==[MR Dump] %s==\n", __func__));
#endif
		vAutoRefreshSwitch(p, DISABLE); //After gating, Rx and Tx calibration, auto refresh should be disable
	}

	vSetRank(p, RANK_0); // Set p's rank back to 0 (Avoids unexpected auto-rank offset calculation in u4RegBaseAddrTraslate())

#if GATING_ADJUST_TXDLY_FOR_TRACKING
	DramcRxdqsGatingPostProcess(p);
#endif
}

static int GetDramInforAfterCalByMRR(DRAMC_CTX_T *p, DRAM_INFO_BY_MRR_T *DramInfo)
{
	U8 u1RankIdx, u1DieNumber = 0;
	U16 u2Density;
	U16 u2MR7;
	U16 u2MR8 = 0;
	U64 u8Size = 0, u8Size_backup = 0;

	if (p->revision_id != REVISION_ID_MAGIC)
		return 0;

	vSetPHY2ChannelMapping(p, CHANNEL_A);

	// Read MR5 for Vendor ID
	DramcModeRegReadByRank(p, RANK_0, 5, &(p->vendor_id));// for byte mode, don't show value of another die.
	p->vendor_id &= 0xFF;
	mcSHOW_DBG_MSG(("[GetDramInforAfterCalByMRR] Vendor %x.\n", p->vendor_id));
	// Read MR6 for Revision ID
	DramcModeRegReadByRank(p, RANK_0, 6, &(p->revision_id));// for byte mode, don't show value of another die.
	mcSHOW_DBG_MSG(("[GetDramInforAfterCalByMRR] Revision %x.\n", p->revision_id));
	// Read MR6 for Revision ID2
	DramcModeRegReadByRank(p, RANK_0, 7, &u2MR7);// for byte mode, don't show value of another die.
	mcSHOW_DBG_MSG(("[GetDramInforAfterCalByMRR] Revision 2 %x.\n", u2MR7));
#if (!__ETT__) && (FOR_DV_SIMULATION_USED==0)
	set_dram_mr(5, p->vendor_id);
	set_dram_mr(6, p->revision_id);
	set_dram_mr(7, u2MR7);
#endif
	if (DramInfo != NULL)
	{
		DramInfo->u2MR5VendorID = p->vendor_id;
		DramInfo->u2MR6RevisionID = p->revision_id;

		for (u1RankIdx = 0; u1RankIdx < RANK_MAX; u1RankIdx++)
			DramInfo->u8MR8RankSize[u1RankIdx] = 0;
	}

	// Read MR8 for dram density
	for (u1RankIdx = 0; u1RankIdx < (p->support_rank_num); u1RankIdx++)
	{
		#if 0//PRINT_CALIBRATION_SUMMARY
		if ((p->aru4CalExecuteFlag[u1ChannelIdx][u1RankIdx] != 0) && \
			(p->aru4CalResultFlag[u1ChannelIdx][u1RankIdx] == 0))
		#endif
		{
			DramcModeRegReadByRank(p, u1RankIdx, 0, &(gu2MR0_Value[u1RankIdx]));
			mcSHOW_DBG_MSG(("MR0 0x%x\n", gu2MR0_Value[u1RankIdx]));

			DramcModeRegReadByRank(p, u1RankIdx, 8, &u2Density);
			mcSHOW_DBG_MSG(("MR8 0x%x\n", u2Density));
			u2MR8 |= (u2Density & 0xFF) << (u1RankIdx * 8);

			u1DieNumber = 1;
			if (((u2Density >> 6) & 0x3) == 1) //OP[7:6] =0, x16 (normal mode)
				u1DieNumber = 2;

			u2Density = (u2Density >> 2) & 0xf;

			switch (u2Density)
			{
				///TODO: Darren, please check the value of u8Size.
				case 0x0:
					u8Size = 0x20000000;  //4Gb = 512MB
					//mcSHOW_DBG_MSG(("[EMI]DRAM density = 4Gb\n"));
					break;
				case 0x1:
					u8Size = 0x30000000;  //6Gb = 768MB
					//mcSHOW_DBG_MSG(("[EMI]DRAM density = 6Gb\n"));
					break;
				case 0x2:
					u8Size = 0x40000000;  //8Gb = 1GB = 2^30 bytes = 0x40000000 bytes
					//mcSHOW_DBG_MSG(("[EMI]DRAM density = 8Gb\n"));
					break;
				case 0x3:
					u8Size = 0x60000000;  //12Gb = 1.5GB = 3^30 bytes = 0x60000000 bytes
					//mcSHOW_DBG_MSG(("[EMI]DRAM density = 12Gb\n"));
					break;
				case 0x4:
					u8Size = 0x80000000;  //16Gb = 2GB = 4^30 bytes = 0x80000000 bytes
					//mcSHOW_DBG_MSG(("[EMI]DRAM density = 16Gb\n"));
					break;
				case 0x5:
					u8Size = 0xc0000000; //24Gb = 3GB = 6^30 bytes = 0xc0000000 bytes
					//mcSHOW_DBG_MSG(("[EMI]DRAM density = 24Gb\n"));
					break;
				case 0x6:
					u8Size = 0x100000000L; //32Gb = 4GB = 8^30 bytes = 0x10000000 bytes
					//mcSHOW_DBG_MSG(("[EMI]DRAM density = 32Gb\n"));
					break;
				default:
					u8Size = 0; //reserved
			}

			if (u8Size_backup < u8Size) // find max dram size for vDramcACTimingOptimize
			{
				u8Size_backup = u8Size;
				p->density = u2Density;
			}

			p->ranksize[u1RankIdx] = u8Size * u1DieNumber;	//dram rank size = density * DieNumber

			if (DramInfo != NULL)
			{
				DramInfo->u8MR8RankSize[u1RankIdx] = p->ranksize[u1RankIdx];
			}
		}
		// 1GB = 2^30 bytes
		// u8Size * (2^3) / (2^30)	==>Gb
		mcSHOW_DBG_MSG(("RK%d, DieNum %d, Density %dGb, RKsize %dGb.\n\n", u1RankIdx, u1DieNumber, (U32)(u8Size >> 27), (U32)(p->ranksize[u1RankIdx] >> 27)));
	}
#if (!__ETT__) && (FOR_DV_SIMULATION_USED==0)
	set_dram_mr(8, u2MR8);
#endif
	return 0;
}

static void vCalibration_Flow_LP4(DRAMC_CTX_T *p)
{
	U8 u1RankMax;
	S8 s1RankIdx;
	//DRAM_STATUS_T VrefStatus;

#ifdef DDR_INIT_TIME_PROFILING
	U32 CPU_Cycle;
	TimeProfileBegin();
#endif

#if __Petrus_TO_BE_PORTING__
#if ENABLE_PHY_RX_INPUT_OFFSET	// skip when bring up
	///TODO: no shuffle, only need to do once under highest freq.
	if(p->frequency == u2DFSGetHighestFreq(p))
	DramcRXInputBufferOffsetCal(p);

#ifdef DDR_INIT_TIME_PROFILING
	CPU_Cycle=TimeProfileEnd();
	mcSHOW_TIME_MSG(("\tRX input cal takes %d us\n", CPU_Cycle));
	TimeProfileBegin();
#endif
#endif
#endif


#if GATING_ADJUST_TXDLY_FOR_TRACKING
	DramcRxdqsGatingPreProcess(p);
#endif

	if (p->support_rank_num==RANK_DUAL)
		u1RankMax = RANK_MAX;
	else
		u1RankMax = RANK_1;

	//vAutoRefreshSwitch(p, DISABLE); //auto refresh is set as disable in LP4_DramcSetting, so don't need to disable again
	vAutoRefreshSwitch(p, DISABLE);

#if 1//(SIMUILATION_CBT == 1)
	for(s1RankIdx=RANK_0; s1RankIdx<u1RankMax; s1RankIdx++)
	{
		vSetRank(p, s1RankIdx);
		#if PINMUX_AUTO_TEST_PER_BIT_CA
		CheckCAPinMux(p);
		#endif

		CmdBusTrainingLP45(p, AUTOK_OFF);

			#if ENABLE_EYESCAN_GRAPH
			print_EYESCAN_LOG_message(p, 0); //draw CBT eyescan
			#endif

		#ifdef DDR_INIT_TIME_PROFILING
		CPU_Cycle=TimeProfileEnd();
		mcSHOW_TIME_MSG(("\tRank %d CBT takes %d us\n", s1RankIdx, CPU_Cycle));
		TimeProfileBegin();
		#endif
	}
	vSetRank(p, RANK_0);

#if __Petrus_TO_BE_PORTING__
	No_Parking_On_CLRPLL(p);
#endif

	// The patch must to do after cbt training
	ShuffleDfsToFSP1(p);
#endif

#if 0//(SIMULATION_WRITE_LEVELING == 1)
	for(s1RankIdx=RANK_0; s1RankIdx<u1RankMax; s1RankIdx++)
	{
		vSetRank(p, s1RankIdx);

		vAutoRefreshSwitch(p, DISABLE); //When doing WriteLeveling, should make sure that auto refresh is disable

#if (!WCK_LEVELING_FM_WORKAROUND)
		if (u1IsLP4Family(p->dram_type))
#endif
		{
			if (!(u1IsLP4Div4DDR800(p) && (p->rank == RANK_1))) // skip for DDR800 rank1
			{
				mcSHOW_DBG_MSG(("\n----->DramcWriteLeveling(PI) begin...\n"));

				DramcWriteLeveling(p, AUTOK_OFF, PI_BASED);

				mcSHOW_DBG_MSG(("DramcWriteLeveling(PI) end<-----\n\n"));
			}
		}

		#ifdef DDR_INIT_TIME_PROFILING
		CPU_Cycle=TimeProfileEnd();
		mcSHOW_TIME_MSG(("\tRank %d Write leveling takes %d us\n", s1RankIdx, CPU_Cycle));
		TimeProfileBegin();
		#endif
	}
	vSetRank(p, RANK_0);

	#if ENABLE_WDQS_MODE_2 // <=DDR1600 reduce PI change code time
	if (!(u1IsLP4Div4DDR800(p)) && (p->frequency <= 800) && (p->support_rank_num == RANK_DUAL)) // skip DDR800semi, for DDR1200/DDR1600 only
		WriteLevelingPosCal(p, PI_BASED);
	#elif ENABLE_TX_WDQS // for WDQS mode 1 to avoid dual rank PI code incorrect
	if (!(u1IsLP4Div4DDR800(p)) && (p->support_rank_num == RANK_DUAL))
		WriteLevelingPosCal(p, PI_BASED);
	#endif
#endif /* (SIMULATION_WRITE_LEVELING == 1) */

	for(s1RankIdx=RANK_0; s1RankIdx<u1RankMax; s1RankIdx++)
	{
		vSetRank(p, s1RankIdx);

#if 1//(SIMULATION_WRITE_LEVELING == 1)
		vAutoRefreshSwitch(p, DISABLE); //When doing WriteLeveling, should make sure that auto refresh is disable

#if (!WCK_LEVELING_FM_WORKAROUND)
		if (u1IsLP4Family(p->dram_type))
#endif
		{
			if (!(u1IsLP4Div4DDR800(p) && (p->rank == RANK_1))) // skip for DDR800 rank1
			{
				mcSHOW_DBG_MSG(("\n----->DramcWriteLeveling(PI) begin...\n"));

				DramcWriteLeveling(p, AUTOK_ON, PI_BASED);

				mcSHOW_DBG_MSG(("DramcWriteLeveling(PI) end<-----\n\n"));
			}
			#ifdef DDR_INIT_TIME_PROFILING
			CPU_Cycle=TimeProfileEnd();
			mcSHOW_TIME_MSG(("\tRank %d Write leveling takes %d us\n", s1RankIdx, CPU_Cycle));
			TimeProfileBegin();
			#endif
		}
#endif /* (SIMULATION_WRITE_LEVELING == 1) */

		vAutoRefreshSwitch(p, ENABLE); //when doing gating, RX and TX calibration, auto refresh should be enable

		dramc_rx_dqs_gating_cal(p, AUTOK_OFF, 0);

		#ifdef DDR_INIT_TIME_PROFILING
		CPU_Cycle=TimeProfileEnd();
		mcSHOW_TIME_MSG(("\tRank %d Gating takes %d us\n", s1RankIdx, CPU_Cycle));
		TimeProfileBegin();
		#endif

		DramcRxWindowPerbitCal(p, PATTERN_RDDQC, NULL, AUTOK_OFF);

		#ifdef DDR_INIT_TIME_PROFILING
		CPU_Cycle=TimeProfileEnd();
		mcSHOW_TIME_MSG(("\tRank %d RX RDDQC takes %d us\n", s1RankIdx, CPU_Cycle));
		TimeProfileBegin();
		#endif

#if MRW_CHECK_ONLY
		mcSHOW_MRW_MSG(("\n==[MR Dump] %s==\n", __func__));
#endif

		DramcTxWindowPerbitCal(p, TX_DQ_DQS_MOVE_DQ_DQM, FALSE, AUTOK_OFF);

		if (Get_Vref_Calibration_OnOff(p)==VREF_CALI_ON)
		{
			DramcTxWindowPerbitCal(p, TX_DQ_DQS_MOVE_DQ_ONLY, TRUE, AUTOK_OFF);
		}

#if PINMUX_AUTO_TEST_PER_BIT_TX
		CheckTxPinMux(p);
#endif
		DramcTxWindowPerbitCal(p, TX_DQ_DQS_MOVE_DQ_ONLY, FALSE, AUTOK_OFF);

#if TX_K_DQM_WITH_WDBI
		if ((p->DBI_W_onoff[p->dram_fsp]==DBI_ON))
		{
			// K DQM with DBI_ON, and check DQM window spec.
			//mcSHOW_DBG_MSG(("[TX_K_DQM_WITH_WDBI] Step1: K DQM with DBI_ON, and check DQM window spec.\n\n"));
			vSwitchWriteDBISettings(p, DBI_ON);
			DramcTxWindowPerbitCal((DRAMC_CTX_T *) p, TX_DQ_DQS_MOVE_DQM_ONLY, FALSE, AUTOK_OFF);
			vSwitchWriteDBISettings(p, DBI_OFF);
		}
#endif

	#if ENABLE_EYESCAN_GRAPH
		Dramc_K_TX_EyeScan_Log(p);
		print_EYESCAN_LOG_message(p, 2); //draw TX eyescan
	#endif

		#ifdef DDR_INIT_TIME_PROFILING
		CPU_Cycle=TimeProfileEnd();
		mcSHOW_TIME_MSG(("\tRank %d TX calibration takes %d us\n", s1RankIdx, CPU_Cycle));
		TimeProfileBegin();
		#endif

		DramcRxdatlatCal(p);

		#ifdef DDR_INIT_TIME_PROFILING
		CPU_Cycle=TimeProfileEnd();
		mcSHOW_TIME_MSG(("\tRank %d Datlat takes %d us\n", s1RankIdx, CPU_Cycle));
		TimeProfileBegin();
		#endif

#if PINMUX_AUTO_TEST_PER_BIT_RX
		CheckRxPinMux(p);
#endif
		DramcRxWindowPerbitCal(p, PATTERN_TEST_ENGINE, NULL /*Set Vref = 0 to test*/, AUTOK_OFF);

		#ifdef DDR_INIT_TIME_PROFILING
		CPU_Cycle=TimeProfileEnd();
		mcSHOW_TIME_MSG(("\tRank %d RX calibration takes %d us\n", s1RankIdx, CPU_Cycle));
		TimeProfileBegin();
		#endif
	   // DramcRxdqsGatingCal(p);

#if ENABLE_EYESCAN_GRAPH
		print_EYESCAN_LOG_message(p, 1); //draw RX eyescan
#endif

#if (SIMULATION_RX_DVS == 1)
	if (p->frequency >=2133)
		DramcRxDVSWindowCal(p);
#endif

#if TX_OE_CALIBATION && !ENABLE_WDQS_MODE_2
		if(p->frequency >= 1600)
		{
			DramcTxOECalibration(p);
		}
#endif

		vAutoRefreshSwitch(p, DISABLE);

		#if ENABLE_TX_TRACKING
		DramcDQSOSCSetMR18MR19(p);
		DramcDQSOSCMR23(p);
		#endif

	}

#if __Petrus_TO_BE_PORTING__
	#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
	if(p->femmc_Ready==0)
	#endif
	{
		if(p->frequency >= RX_VREF_DUAL_RANK_K_FREQ)  // for 3733/4266
		{
			U8 u1ByteIdx, u1HighFreqRXVref[2];
			for(u1ByteIdx =0 ; u1ByteIdx<(p->data_width/DQS_BIT_NUMBER); u1ByteIdx++)
			{
				u1HighFreqRXVref[u1ByteIdx] = (gFinalRXVrefDQ[p->channel][RANK_0][u1ByteIdx] + gFinalRXVrefDQ[p->channel][RANK_1][u1ByteIdx]) >> 1;
				mcSHOW_DBG_MSG(("RX Vref Byte%d (u1HighFreqRXVref) = %d = (%d+ %d)>>1\n", u1ByteIdx, u1HighFreqRXVref[u1ByteIdx], gFinalRXVrefDQ[p->channel][RANK_0][u1ByteIdx], gFinalRXVrefDQ[p->channel][RANK_1][u1ByteIdx]));
			}

			for(s1RankIdx=RANK_0; s1RankIdx < u1RankMax; s1RankIdx++)
			{
				vSetRank(p, s1RankIdx);
				DramcRxWindowPerbitCal((DRAMC_CTX_T *) p, 1, u1HighFreqRXVref);
			}
		}
	}
#endif

	vSetRank(p, RANK_0); // Set p's rank back to 0 (Avoids unexpected auto-rank offset calculation in u4RegBaseAddrTraslate())

	#if ENABLE_TX_TRACKING
	DramcDQSOSCShuSettings(p);
	#endif

#if (SIMULATION_GATING && GATING_ADJUST_TXDLY_FOR_TRACKING)
	DramcRxdqsGatingPostProcess(p);
#endif

#if TDQSCK_PRECALCULATION_FOR_DVFS
	DramcDQSPrecalculation_preset(p);
#endif

#if SIMULATION_RX_DVS
	if (p->frequency >=2133)
		DramcDramcRxDVSCalPostProcess(p);
#endif

	DramcDualRankRxdatlatCal(p);

#if RDSEL_TRACKING_EN
	if (p->frequency >= 1866)
		RDSELRunTimeTracking_preset(p);
#endif

#if XRTWTW_NEW_CROSS_RK_MODE
	if(p->support_rank_num == RANK_DUAL)
	{
		XRTWTW_SHU_Setting(p);
	}
#endif

#if __Petrus_TO_BE_PORTING__
#if LJPLL_FREQ_DEBUG_LOG
	DDRPhyFreqMeter();
#endif
#endif

	#ifdef DDR_INIT_TIME_PROFILING
	CPU_Cycle=TimeProfileEnd();
	mcSHOW_TIME_MSG(("\tMisc takes %d us\n\n", s1RankIdx, CPU_Cycle));
	#endif
}

static void vDramCalibrationSingleChannel(DRAMC_CTX_T *p)
{
#if 0//!__ETT__
	/*
	 * Since DRAM calibration will cost much time,
	 * kick wdt here to prevent watchdog timeout.
	 */
#if (FOR_DV_SIMULATION_USED == 0 && SW_CHANGE_FOR_SIMULATION == 0)
	mtk_wdt_restart();
#endif
#endif

	vCalibration_Flow_LP4(p);
}

void vDramCalibrationAllChannel(DRAMC_CTX_T *p)
{
	U8 channel_idx, rank_idx;

#ifdef DDR_INIT_TIME_PROFILING
	U32 u4low_tick0, u4high_tick0, u4low_tick1, u4high_tick1;
#if __ETT__
	u4low_tick0 = GPT_GetTickCount(&u4high_tick0);
#else
	u4low_tick0 = get_timer(0);
#endif
#endif

	vIO32WriteFldMulti_All(DDRPHY_REG_CA_CMD2, P_Fld(1, CA_CMD2_RG_TX_ARCMD_OE_DIS_CA)
											| P_Fld(0, CA_CMD2_RG_TX_ARCA_OE_TIE_SEL_CA)
											| P_Fld(0xff, CA_CMD2_RG_TX_ARCA_OE_TIE_EN_CA));
	for (channel_idx = CHANNEL_A; channel_idx < p->support_channel_num; channel_idx++)
	{
		vSetPHY2ChannelMapping(p, channel_idx);// when switching channel, must update PHY to Channel Mapping
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_CA_CMD2), P_Fld(0, CA_CMD2_RG_TX_ARCMD_OE_DIS_CA)
											| P_Fld(1, CA_CMD2_RG_TX_ARCA_OE_TIE_SEL_CA)
											| P_Fld(0xff, CA_CMD2_RG_TX_ARCA_OE_TIE_EN_CA));
		vDramCalibrationSingleChannel(p);
	}

	vSetPHY2ChannelMapping(p, CHANNEL_A);

#if PRINT_CALIBRATION_SUMMARY
	vPrintCalibrationResult(p);
#endif

#ifdef FOR_HQA_TEST_USED
	#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
	if (p->femmc_Ready == 1)
	{
		mcSHOW_DBG_MSG(("\nCalibration fast K is enable, cannot show HQA measurement information\n"));
	}
	else
	#endif
	print_HQA_measure_message(p);
#endif

	/* Enable/Disable calibrated rank's DBI function accordingly */
#if ENABLE_READ_DBI
	//Read DBI ON
	vSetRank(p, RANK_0);
	vSetPHY2ChannelMapping(p, CHANNEL_A);

	DramcReadDBIOnOff(p, p->DBI_R_onoff[p->dram_fsp]);
#endif

#if ENABLE_WRITE_DBI
	// Just settle the DBI parameters which would be stored into shuffle space.
	if (p->DBI_W_onoff[p->dram_fsp])
	{
		for (channel_idx = CHANNEL_A; channel_idx < p->support_channel_num; channel_idx++)
		{
			vSetPHY2ChannelMapping(p, channel_idx);

			for (rank_idx = RANK_0; rank_idx < RANK_MAX; rank_idx++)
			{
				vSetRank(p, rank_idx);
				DramcWriteShiftMCKForWriteDBI(p, -1); //Tx DQ/DQM -1 MCK for write DBI ON
			}
			vSetRank(p, RANK_0);
		}
		vSetPHY2ChannelMapping(p, CHANNEL_A);

		// Improve Write DBI Power
		ApplyWriteDBIPowerImprove(p, ENABLE);

		#if ENABLE_WRITE_DBI_Protect
		ApplyWriteDBIProtect(p, ENABLE);
		#endif
	}

	DramcWriteDBIOnOff(p, p->DBI_W_onoff[p->dram_fsp]);
#endif

#if TX_PICG_NEW_MODE
	TXPICGSetting(p);
#endif

#if XRTRTR_NEW_CROSS_RK_MODE
	if (p->support_rank_num == RANK_DUAL)
	{
		XRTRTR_SHU_Setting(p);
	}
#endif

#if (ENABLE_TX_TRACKING || TDQSCK_PRECALCULATION_FOR_DVFS)
	FreqJumpRatioCalculation(p);
#endif

#ifdef TEMP_SENSOR_ENABLE
	DramcHMR4_Presetting(p);
#endif

#if (ENABLE_PER_BANK_REFRESH == 1)
	DramcEnablePerBankRefresh(p, ON);
#else
	DramcEnablePerBankRefresh(p, OFF);
#endif

#if DRAMC_MODIFIED_REFRESH_MODE
	DramcModifiedRefreshMode(p);
#endif

#if DRAMC_CKE_DEBOUNCE
	DramcCKEDebounce(p);
#endif

#if ENABLE_TX_TRACKING
	U8 backup_channel = p->channel;
	U8 channelIdx;

	for (channelIdx = CHANNEL_A; channelIdx < p->support_channel_num; channelIdx++)
	{
		vSetPHY2ChannelMapping(p, channelIdx);
		DramcHwDQSOSC(p);
	}

	vSetPHY2ChannelMapping(p, backup_channel);
	mcSHOW_DBG_MSG(("TX_TRACKING: ON\n"));
#else
	mcSHOW_DBG_MSG(("TX_TRACKING: OFF\n"));
#endif


#if ENABLE_DFS_RUNTIME_MRW
	DFSRuntimeMRW_preset(p, vGet_Current_ShuLevel(p));
#endif

#ifdef DDR_INIT_TIME_PROFILING
#if __ETT__
	u4low_tick1 = GPT_GetTickCount(&u4high_tick1);
	mcSHOW_TIME_MSG(("	(4) vDramCalibrationAllChannel() take %d ms\n\r", ((u4low_tick1 - u4low_tick0) * 76) / 1000000));
#else
	u4low_tick1 = get_timer(u4low_tick0);
	mcSHOW_TIME_MSG(("	(4) vDramCalibrationAllChannel() take %d ms\n\r", u4low_tick1));
#endif
#endif
}

U8 gGet_MDL_Used_Flag = 0;
void Set_MDL_Used_Flag(U8 value)
{
	gGet_MDL_Used_Flag = value;
}

U8 Get_MDL_Used_Flag(void)
{
	return gGet_MDL_Used_Flag;
}

DRAMC_CTX_T *psCurrDramCtx;
U8 gfirst_init_flag = 0;
int Init_DRAM(DRAM_DRAM_TYPE_T dram_type, DRAM_CBT_MODE_EXTERN_T dram_cbt_mode_extern,
		  DRAM_INFO_BY_MRR_T *DramInfo, U8 get_mdl_used)
{
	#if !SW_CHANGE_FOR_SIMULATION

	DRAMC_CTX_T * p;
	U8 final_shu;

#ifdef DDR_INIT_TIME_PROFILING
	U32 CPU_Cycle;
	TimeProfileBegin();
#endif

	psCurrDramCtx = &DramCtx_LPDDR4;

#if defined(DDR_INIT_TIME_PROFILING) || (__ETT__ && SUPPORT_SAVE_TIME_FOR_CALIBRATION)
	if (gtime_profiling_flag == 0)
	{
		memcpy(&gTimeProfilingDramCtx, psCurrDramCtx, sizeof(DRAMC_CTX_T));
		gtime_profiling_flag = 1;
	}

	p = &gTimeProfilingDramCtx;
	gfirst_init_flag = 0;

	//DramcConfInfraReset(p);  //No need when DDR_INIT_TIME_PROFILING_TEST_CNT=1
#else
	p = psCurrDramCtx;
#endif

	p->new_cbt_mode = 1;

	Set_MDL_Used_Flag(get_mdl_used);

	p->dram_type = dram_type;

	/* Convert DRAM_CBT_MODE_EXTERN_T to DRAM_CBT_MODE_T */
	switch ((int)dram_cbt_mode_extern)
	{
		case CBT_R0_R1_NORMAL:
			p->dram_cbt_mode[RANK_0] = CBT_NORMAL_MODE;
			p->dram_cbt_mode[RANK_1] = CBT_NORMAL_MODE;
			break;
		case CBT_R0_R1_BYTE:
			p->dram_cbt_mode[RANK_0] = CBT_BYTE_MODE1;
			p->dram_cbt_mode[RANK_1] = CBT_BYTE_MODE1;
			break;
		case CBT_R0_NORMAL_R1_BYTE:
			p->dram_cbt_mode[RANK_0] = CBT_NORMAL_MODE;
			p->dram_cbt_mode[RANK_1] = CBT_BYTE_MODE1;
			break;
		case CBT_R0_BYTE_R1_NORMAL:
			p->dram_cbt_mode[RANK_0] = CBT_BYTE_MODE1;
			p->dram_cbt_mode[RANK_1] = CBT_NORMAL_MODE;
			break;
		default:
			mcSHOW_ERR_MSG(("Error!"));
			break;
	}
	mcSHOW_DBG_MSG2(("dram_cbt_mode_extern: %d\n"
					  "dram_cbt_mode [RK0]: %d, [RK1]: %d\n",
					  (int)dram_cbt_mode_extern, p->dram_cbt_mode[RANK_0], p->dram_cbt_mode[RANK_1]));

#if ENABLE_APB_MASK_WRITE
	U32 u4GPTTickCnt;
	TimeProfileBegin();

	EnableDramcAPBMaskWrite(p);
	DramcRegAPBWriteMask(p);

	u4GPTTickCnt = TimeProfileEnd();
	mcSHOW_TIME_MSG(("[DramcRegAPBWriteMask] take %d ms\n", u4GPTTickCnt / 1000));

	TestAPBMaskWriteFunc(p);

	while (1);
#endif

	DramcBroadcastOnOff(DRAMC_BROADCAST_ON);   //LP4 broadcast on

	if (gfirst_init_flag == 0)
	{
		MPLLInit();
		Global_Option_Init(p);
		gfirst_init_flag = 1;
	}

#ifdef FIRST_BRING_UP
	Test_Broadcast_Feature(p);
#endif

#if (FOR_DV_SIMULATION_USED == 0 && SW_CHANGE_FOR_SIMULATION == 0)
	{
		U32 backup_broadcast;
		backup_broadcast = GetDramcBroadcast();
		DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
		mdl_setting(p);
		UpdateGlobal10GBEnVariables(p); // @Darren, for 10GB
		TA2_Test_Run_Time_HW_Set_Column_Num(p);
		DramcBroadcastOnOff(backup_broadcast);
	}
#endif

	mcSHOW_DBG_MSG(("\n\n[Bian_co] ETT version 0.0.0.1\n dram_type %d, R0 cbt_mode %d, R1 cbt_mode %d VENDOR=%d\n\n", p->dram_type, p->dram_cbt_mode[RANK_0], p->dram_cbt_mode[RANK_1], p->vendor_id));

#if __Petrus_TO_BE_PORTING__
	vDramcInit_PreSettings(p);
#endif

	// DramC & PHY init for all channels
	//===  First frequency ======

#if defined(DUMP_INIT_RG_LOG_TO_DE)
	vSetDFSFreqSelByTable(p, &gFreqTbl[1]); //0:3200 1:4266, 2:800, 3:1866, 4:1200, 5:2400, 6:1600
#else
	vSetDFSFreqSelByTable(p, &gFreqTbl[DRAM_DFS_SHUFFLE_MAX-1]);
	//vSetDFSFreqSelByTable(p, &gFreqTbl[1]);
#endif
	//#if (ENABLE_DRAM_SINGLE_FREQ_SELECT != 0xFF) || defined(FIRST_BRING_UP) || (__FLASH_TOOL_DA__)
	if (is_dvfs_enabled())
		gAndroid_DVFS_en = TRUE;
	else
		gAndroid_DVFS_en = FALSE;

#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
	DramcSave_Time_For_Cal_Init(p);
#endif
#ifndef LOOPBACK_TEST
	if (p->dram_type == TYPE_LPDDR4X) // LP4/LP4P need confirm
	{
		// LP4 IMP_LOW_FREQ <= DDR3733, IMP_HIGH_FREQ >= DDR4266
		// LP5 IMP_LOW_FREQ <= DDR3733, IMP_HIGH_FREQ >= DDR4266
		DramcSwImpedanceCal(p, 1, IMP_LOW_FREQ);
		DramcSwImpedanceCal(p, 1, IMP_HIGH_FREQ);
		#if ENABLE_SAMSUNG_NT_ODT
		DramcSwImpedanceCal(p, 1, IMP_NT_ODTN); // for Samsung NT ODTN
		#endif
	}
	else
	{
		mcSHOW_ERR_MSG(("[DramcSwImpedanceCal] Warnning: Need confirm DRAM type for SW IMP Calibration !!!\n"));
		#if __ETT__
		while (1);
		#endif
	}
#endif

#ifdef DDR_INIT_TIME_PROFILING
	CPU_Cycle = TimeProfileEnd();
	mcSHOW_TIME_MSG(("(0)Pre_Init + SwImdepance takes %d ms\n\r", CPU_Cycle / 1000));
#endif

#ifdef DUMP_INIT_RG_LOG_TO_DE
	gDUMP_INIT_RG_LOG_TO_DE_RG_log_flag = 1;
	mcSHOW_DUMP_INIT_RG_MSG(("\n\n//=== DDR\033[1;32m%d\033[m\n",p->frequency<<1));
#endif

	//Clk free run
	//EnableDramcPhyDCM(p, 0);

	DFSInitForCalibration(p);

#ifdef TEST_MODE_MRS
	if (global_which_test == 0)
		TestModeTestMenu();
#endif


#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
	if (p->femmc_Ready==1)
	{
		p->support_rank_num = p->pSavetimeData->support_rank_num;
	}
#endif

	#if (FOR_DV_SIMULATION_USED == 0 && SW_CHANGE_FOR_SIMULATION == 0)
	U32 backup_broadcast;
	backup_broadcast = GetDramcBroadcast();
	DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
	emi_init2();
	DramcBroadcastOnOff(backup_broadcast);
	#endif

	if (Get_MDL_Used_Flag()==GET_MDL_USED)
	{
		// only K CHA to save time
		vSetPHY2ChannelMapping(p, CHANNEL_A);
		vCalibration_Flow_For_MDL(p); // currently for LP4
		GetDramInforAfterCalByMRR(p, DramInfo);
		return 0;
	}
	else //NORMAL_USED
	{
		#if (fcFOR_CHIP_ID == fcMargaux) // @Darren, new chip need double confirm
		if (p->DRAMPinmux == PINMUX_DSC)
			UpdateDFSTbltoDDR3200(p);
		#endif
		vDramCalibrationAllChannel(p);
		GetDramInforAfterCalByMRR(p, DramInfo);
		vDramcACTimingOptimize(p);
	}

	#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
	DramcSave_Time_For_Cal_End(p);
	#endif

#if ((!defined(FIRST_BRING_UP)) || (ENABLE_DRAM_SINGLE_FREQ_SELECT != 0xFF)) && (!__FLASH_TOOL_DA__)
	DramcSaveToShuffleSRAM(p, DRAM_DFS_SHUFFLE_1, p->pDFSTable->shuffleIdx);
	#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
	DramcSave_Time_For_Cal_End(p);
	#endif
	LoadShuffleSRAMtoDramc(p, p->pDFSTable->shuffleIdx, DRAM_DFS_SHUFFLE_2); //Darren: DDR1600 for MRW (DramcModeRegInit_LP4 and CBT)
	#if ENABLE_SRAM_DMA_WA
	DPHYSRAMShuWAToSHU1(p); //Darren: DDR1600 for MRW (DramcModeRegInit_LP4 and CBT)
	#endif

	S8 u1ShuIdx;
//#if (ENABLE_DRAM_SINGLE_FREQ_SELECT == 0xFF)
	if (is_dvfs_enabled()) {
		for (u1ShuIdx = DRAM_DFS_SHUFFLE_MAX - 2; u1ShuIdx >= DRAM_DFS_SHUFFLE_1; u1ShuIdx--)
		{
			#if (fcFOR_CHIP_ID == fcMargaux) && (ENABLE_DRAM_SINGLE_FREQ_SELECT == 0xFF) // @Darren, new chip need double confirm
			if ((p->DRAMPinmux == PINMUX_DSC) && (gFreqTbl[u1ShuIdx].shuffleIdx == SRAM_SHU0))
				continue;
			#endif

			vSetDFSFreqSelByTable(p, &gFreqTbl[u1ShuIdx]);
			#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
			DramcSave_Time_For_Cal_Init(p);
			#endif
			DFSInitForCalibration(p);
			vDramCalibrationAllChannel(p);
			vDramcACTimingOptimize(p);

			#if RUNTIME_SHMOO_RELEATED_FUNCTION && SUPPORT_SAVE_TIME_FOR_CALIBRATION
			if (p->frequency == u2DFSGetHighestFreq(p))
			{
				DramcRunTimeShmooRG_BackupRestore(p);

				RunTime_Shmoo_update_parameters(p);
			}
#endif
			DramcSaveToShuffleSRAM(p, DRAM_DFS_SHUFFLE_1, gFreqTbl[u1ShuIdx].shuffleIdx);
			#if (fcFOR_CHIP_ID == fcMargaux) && (ENABLE_DRAM_SINGLE_FREQ_SELECT == 0xFF) // @Darren, new chip need double confirm
			if ((p->DRAMPinmux == PINMUX_DSC) && (gFreqTbl[u1ShuIdx].shuffleIdx == SRAM_SHU1))
				DramcSaveToShuffleSRAM(p, DRAM_DFS_SHUFFLE_1, gFreqTbl[u1ShuIdx + 1].shuffleIdx); // Copy SRAM_SHU1 to SRAM_SHU0
			#endif

			#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
			DramcSave_Time_For_Cal_End(p);
			#endif
		}
	}
#endif //((!defined(FIRST_BRING_UP)) || (ENABLE_DRAM_SINGLE_FREQ_SELECT != 0xFF)) && (!__FLASH_TOOL_DA__)

#ifdef DDR_INIT_TIME_PROFILING
	TimeProfileBegin();
#endif

	vAfterCalibration(p);

#ifdef ENABLE_POST_PACKAGE_REPAIR
	PostPackageRepair();
#endif

#if __Petrus_TO_BE_PORTING__

#if 0//TX_OE_CALIBATION, for DMA test
	U8 u1ChannelIdx, u1RankIdx;
	for (u1ChannelIdx = 0; u1ChannelIdx < (p->support_channel_num); u1ChannelIdx++)
		for (u1RankIdx = 0; u1RankIdx < (p->support_rank_num); u1RankIdx++)
		{
			vSetPHY2ChannelMapping(p, u1ChannelIdx);
			vSetRank(p, u1RankIdx);
			DramcTxOECalibration(p);
		}

	vSetPHY2ChannelMapping(p, CHANNEL_A);
	vSetRank(p, RANK_0);

	U32 u4err_value;
	DramcDmaEngine((DRAMC_CTX_T *)p, 0x50000000, 0x60000000, 0xff00, 8, DMA_PREPARE_DATA_ONLY, p->support_channel_num);
	u4err_value = DramcDmaEngine((DRAMC_CTX_T *)p, 0x50000000, 0x60000000, 0xff00, 8, DMA_CHECK_DATA_ACCESS_AND_COMPARE, p->support_channel_num);
	mcSHOW_DBG_MSG(("DramC_TX_OE_Calibration  0x%X\n", u4err_value));
#endif

#if !LCPLL_IC_SCAN
#if (FOR_DV_SIMULATION_USED == 0 && SW_CHANGE_FOR_SIMULATION == 0)
	print_DBG_info(p);
	Dump_EMIRegisters(p);
#endif
#endif

#if 0
	DramcRegDump(p, SRAM_SHU0);
#endif

// ETT_NO_DRAM #endif

#if ETT_NO_DRAM
	//NoDramDramcRegDump(p);
	NoDramRegFill();
#endif
#endif //#if __Petrus_TO_BE_PORTING__

	#if DRAMC_MODEREG_CHECK
	DramcModeReg_Check(p);
	#endif

	#if __FLASH_TOOL_DA__
	vPrintPinInfoResult(p);
	vGetErrorTypeResult(p);
	#endif
	#if CPU_RW_TEST_AFTER_K
	mcSHOW_DBG_MSG(("\n[MEM_TEST] 02: After DFS, before run time config\n"));
	vDramCPUReadWriteTestAfterCalibration(p);
#endif

	#if TA2_RW_TEST_AFTER_K
	mcSHOW_DBG_MSG(("\n[TA2_TEST]\n"));
	TA2_Test_Run_Time_HW(p);
	#endif

#if __ETT__
#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
	if (!(p->femmc_Ready == 0))
#elif defined(DDR_INIT_TIME_PROFILING)
if (u2TimeProfileCnt == (DDR_INIT_TIME_PROFILING_TEST_CNT - 1)) //last time of loop
#endif
#endif
	{
		EnableDFSHwModeClk(p);
		mcSHOW_DBG_MSG(("DFS_SHUFFLE_HW_MODE: ON\n"));
		if (gAndroid_DVFS_en == TRUE) // shuffle to DDR3733 boot
		{
#if defined(SLT)
			final_shu = SRAM_SHU1;	//DDR3200
#else
			final_shu = SRAM_SHU0;	//DDR4266
#endif

			vSetDFSFreqSelByTable(p, get_FreqTbl_by_shuffleIndex(p, final_shu));
			DramcDFSDirectJump_SRAMShuRGMode(p, SRAM_SHU1);
			DramcDFSDirectJump_SRAMShuRGMode(p, final_shu);
			print("switch to %d Mbps bootup\n", p->frequency * 2);
		}


#if __Petrus_TO_BE_PORTING__
		#if (DVT_TEST_DUMMY_RD_SIDEBAND_FROM_SPM && defined(DUMMY_READ_FOR_TRACKING))
		DramcDummyReadForSPMSideBand(p); // SPM dummy read 1us <-> 4us for DVT only (it must call after TransferPLLToSPMControl)
		#endif

		EnableDramcTrackingBySPMControl(p);

		mcSHOW_DBG_MSG(("\n\nSettings after calibration\n\n"));
		mcDUMP_REG_MSG(("\n\nSettings after calibration\n\n"));
#endif

		DramcRunTimeConfig(p);
	}

	#if CPU_RW_TEST_AFTER_K
	mcSHOW_DBG_MSG(("\n[MEM_TEST] 03: After run time config\n"));
	vDramCPUReadWriteTestAfterCalibration(p);
	#endif

	#if TA2_RW_TEST_AFTER_K
	mcSHOW_DBG_MSG(("\n[TA2_TEST]\n"));
	TA2_Test_Run_Time_HW(p);
	#endif


#if (__ETT__ && CPU_RW_TEST_AFTER_K)
	/* 0x46000000 is LK base addr */
	//while(1)
	{
		//if ((s4value = dramc_complex_mem_test (0x46000000, 0x2000)) == 0)
		if ((s4value = dramc_complex_mem_test (0x40024000, 0x20000)) == 0)
		{
			mcSHOW_DBG_MSG(("1st complex R/W mem test pass\n"));
		}
		else
		{
			mcSHOW_DBG_MSG(("1st complex R/W mem test fail :-%d\n", -s4value));
#if defined(SLT)
			mcSHOW_ERR_MSG(("[dramc] DRAM_FATAL_ERR_FLAG = 0x80000000\n"));
			while (1);
#endif
		}
	}
#endif

#if MRW_CHECK_ONLY
	vPrintFinalModeRegisterSetting(p);
#endif

#ifdef DDR_INIT_TIME_PROFILING
	CPU_Cycle = TimeProfileEnd();
	mcSHOW_TIME_MSG(("	(5) After calibration takes %d ms\n\r", CPU_Cycle / 1000));
#endif	// end of DDR_INIT_TIME_PROFILING

#endif//SW_CHANGE_FOR_SIMULATION
	//Low_Power_Scenarios_Test(p);

	//vSetDFSFreqSelByTable(p, get_FreqTbl_by_shuffleIndex(p, SRAM_SHU1));
	//DramcDFSDirectJump(p, SRAM_SHU1);//Switch to CLRPLL

	//ETT_DRM(p);
	return 0;
}
///TODO: wait for porting ---


///TODO: wait for porting +++
static void DPI_vDramCalibrationSingleChannel(DRAMC_CTX_T *DramConfig, cal_sv_rand_args_t *psra)
{
	U8 ii;

	///TODO: wait for porting +++
#if GATING_ADJUST_TXDLY_FOR_TRACKING
	DramcRxdqsGatingPreProcess(DramConfig);
#endif
	///TODO: wait for porting ---

	vAutoRefreshSwitch(DramConfig, DISABLE);

#if 1//(SIMUILATION_CBT == 1)
	for (ii = RANK_0; ii < DramConfig->support_rank_num; ii++)
	{
		vSetRank(DramConfig, ii);

		if (!psra || psra->cbt) {
			mcSHOW_DBG_MSG(("\n----->DramcCBT begin...\n"));
			timestamp_show();
		#if CBT_O1_PINMUX_WORKAROUND
			CmdBusTrainingLP45(DramConfig, AUTOK_OFF); //Cannot use aito-k in A60868
		#else
			if (psra)
				CmdBusTrainingLP45(DramConfig, psra->cbt_autok);
			else
				CmdBusTrainingLP45(DramConfig, AUTOK_OFF);
		#endif
			timestamp_show();
			mcSHOW_DBG_MSG(("DramcCBT end<-----\n\n"));
		}
	#if ENABLE_EYESCAN_GRAPH
	   mcSHOW_DBG_MSG(("CBT EYESCAN start<-----\n\n"));
	   print_EYESCAN_LOG_message(DramConfig, 0); //draw CBT eyescan
	   mcSHOW_DBG_MSG(("CBT EYESCAN end<-----\n\n"));
	   #endif
	}

	vSetRank(DramConfig, RANK_0);

	///TODO: wait for porting +++
#if __A60868_TO_BE_PORTING__
	No_Parking_On_CLRPLL(DramConfig);
#endif // __A60868_TO_BE_PORTING__
	///TODO: wait for porting ---
#endif /* (SIMUILATION_CBT == 1) */

	for (ii = RANK_0; ii < DramConfig->support_rank_num; ii++)
	{
		vSetRank(DramConfig, ii);

		vAutoRefreshSwitch(DramConfig, DISABLE); //When doing WriteLeveling, should make sure that auto refresh is disable

#if 1//(SIMULATION_WRITE_LEVELING == 1)
#if (!WCK_LEVELING_FM_WORKAROUND)
	if (u1IsLP4Family(DramConfig->dram_type))
#endif
	{
		if (!(u1IsLP4Div4DDR800(DramConfig) && (DramConfig->rank == RANK_1))) // skip for DDR800 rank1
		{
			if (!psra || psra->wl) {
				mcSHOW_DBG_MSG(("\n----->DramcWriteLeveling(PI) begin...\n"));
				timestamp_show();
				if (psra)
				{
					DramcWriteLeveling(DramConfig, psra->wl_autok, PI_BASED);
				}
				else
					DramcWriteLeveling(DramConfig, AUTOK_OFF, PI_BASED);

				timestamp_show();
				mcSHOW_DBG_MSG(("DramcWriteLeveling(PI) end<-----\n\n"));
			}
		}
	}
#endif /* (SIMULATION_WRITE_LEVELING == 1) */

		vAutoRefreshSwitch(DramConfig, ENABLE);

#if 1//(SIMULATION_GATING == 1)
		if (!psra || psra->gating) {
			mcSHOW_DBG_MSG(("\n----->DramcGating begin...\n"));
			timestamp_show();
			if (psra)
				dramc_rx_dqs_gating_cal(DramConfig, psra->gating_autok, 0);
			else
				dramc_rx_dqs_gating_cal(DramConfig, AUTOK_OFF, 0);
			timestamp_show();
			mcSHOW_DBG_MSG(("DramcGating end < -----\n\n"));
		}
#endif

#if 1//(SIMULATION_RX_RDDQC == 1)
		if (!psra || psra->rddqc) {
			mcSHOW_DBG_MSG(("\n----->DramcRxWindowPerbitCal RDDQC begin...\n"));
			timestamp_show();

			#if 0 // Used when testing LP5 RK1 WCK2CK in high efficiency mode and differential mode.
			p->rank = 1;
			// For test HEFF = 1 / WCKDUAL = 0
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_WCKCTRL), 0, SHU_WCKCTRL_WCKDUAL);
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_COMMON0),
					P_Fld(1, SHU_COMMON0_LP5WCKON) |
					P_Fld(1, SHU_COMMON0_LP5HEFF_MODE));
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL), 0, CKECTRL_CKE2RANK_OPT8);
			#endif
			DramcRxWindowPerbitCal(DramConfig, PATTERN_RDDQC, NULL, AUTOK_OFF);
			timestamp_show();
			mcSHOW_DBG_MSG(("DramcRxWindowPerbitCal end<-----\n\n"));
		}
#endif // (SIMULATION_RX_RDDQC == 1)

#if (__LP5_COMBO__ == TRUE)
#if (SIMULATION_DUTY_CYC_MONITOR == 1)
		if (is_lp5_family(DramConfig) && DramConfig->frequency >= GetFreqBySel(DramConfig,LP5_DDR4266))
		{
			if (!psra) {
				mcSHOW_DBG_MSG(("\n----->DramcDutyCycleMonitor begin...\n"));
				timestamp_show();
				DramcDutyCycleMonitor(DramConfig);
				timestamp_show();
				mcSHOW_DBG_MSG(("DramcDutyCycleMonitor end<-----\n\n"));

				mcSHOW_DBG_MSG(("\n----->DramcWriteLeveling(DLY) begin...\n"));
				timestamp_show();
				DramcWriteLeveling(DramConfig, psra->wl_autok, DLY_BASED);
				timestamp_show();
				mcSHOW_DBG_MSG(("DramcWriteLeveling(DLY)end<-----\n\n"));
			}
		}
#endif /* (SIMULATION_DUTY_CYC_MONITOR == 1) */
#endif // (__LP5_COMBO__ == TRUE)

#if 1//(SIMULATION_TX_PERBIT == 1)
		if (!psra || psra->tx_perbit) {
			mcSHOW_DBG_MSG(("\n----->DramcTxWindowPerbitCal begin...\n"));
			timestamp_show();
			if (psra)
				DramcTxWindowPerbitCal(DramConfig, TX_DQ_DQS_MOVE_DQ_DQM,
						FALSE, psra->tx_auto_cal);
			else
				DramcTxWindowPerbitCal(DramConfig, TX_DQ_DQS_MOVE_DQ_DQM,
						FALSE, AUTOK_OFF);
			if (Get_Vref_Calibration_OnOff(DramConfig) == VREF_CALI_ON) {
				if (psra)
					DramcTxWindowPerbitCal(DramConfig, TX_DQ_DQS_MOVE_DQ_ONLY,
							TRUE, psra->tx_auto_cal);
				else
					DramcTxWindowPerbitCal(DramConfig, TX_DQ_DQS_MOVE_DQ_ONLY,
							TRUE, AUTOK_OFF);
			}
			if (psra)
				DramcTxWindowPerbitCal(DramConfig, TX_DQ_DQS_MOVE_DQ_ONLY,
						FALSE, psra->tx_auto_cal);
			else
				DramcTxWindowPerbitCal(DramConfig, TX_DQ_DQS_MOVE_DQ_ONLY,
						FALSE, AUTOK_OFF);
			timestamp_show();
			mcSHOW_DBG_MSG(("DramcTxWindowPerbitCal end<-----\n\n"));

		#if ENABLE_EYESCAN_GRAPH
			   mcSHOW_DBG_MSG(("\n----->DramcTxEYESCAN begin...\n"));
			   Dramc_K_TX_EyeScan_Log(DramConfig);
			   print_EYESCAN_LOG_message(DramConfig, 2); //draw TX eyescan
			   mcSHOW_DBG_MSG(("\n----->DramcTxEYESCAN end...\n"));
			   #endif
		}
#endif // (SIMULATION_TX_PERBIT == 1)

#if 1//(SIMULATION_DATLAT == 1)
		if (1) { // No parameter in correspondence with by now
			mcSHOW_DBG_MSG(("\n----->DramcRxdatlatCal begin...\n"));
			timestamp_show();

			DramcRxdatlatCal(DramConfig);

			timestamp_show();
			mcSHOW_DBG_MSG(("DramcRxdatlatCal end<-----\n\n"));
		}
#endif // (SIMULATION_DATLAT == 1)

#if 1//(SIMULATION_RX_PERBIT == 1)
		if (!psra || psra->rx_perbit) {
			mcSHOW_DBG_MSG(("\n----->DramcRxWindowPerbitCal begin...\n"));
			timestamp_show();
			if (psra)
				DramcRxWindowPerbitCal(DramConfig, PATTERN_TEST_ENGINE,
						NULL /*Set Vref = 0 to test*/, psra->rx_auto_cal);
			else
				DramcRxWindowPerbitCal(DramConfig, PATTERN_TEST_ENGINE,
						NULL /*Set Vref = 0 to test*/, AUTOK_OFF);
			timestamp_show();
			mcSHOW_DBG_MSG(("DramcRxWindowPerbitCal end<-----\n\n"));

			#if ENABLE_EYESCAN_GRAPH
			   mcSHOW_DBG_MSG(("DramcRxWindowPerbitCal EYESCAN start<-----\n\n"));
			   print_EYESCAN_LOG_message(DramConfig, 1); //draw RX eyescan
			   mcSHOW_DBG_MSG(("DramcRxWindowPerbitCal EYESCAN end<-----\n\n"));
			#endif
		}
#endif // (SIMULATION_RX_PERBIT == 1)

#if (SIMULATION_RX_DVS == 1)
	if (DramConfig->frequency >=2133)
		DramcRxDVSWindowCal(DramConfig);
#endif

#if TX_OE_CALIBATION
		if (DramConfig->frequency >= 1600)
		{
			DramcTxOECalibration(DramConfig);
		}
#endif // TX_OE_CALIBATION

		#if ENABLE_TX_TRACKING
		#if 0 /* Starting from Vinson, no need to pre-calculate MR23 for different freqs */
		if (gu1MR23Done == FALSE)
		{
			DramcDQSOSCAuto(p);
		}
		#endif
		DramcDQSOSCAuto(DramConfig);
		DramcDQSOSCMR23(DramConfig);
		DramcDQSOSCSetMR18MR19(DramConfig);
		#endif
	}

	vSetRank(DramConfig, RANK_0);

	#if ENABLE_TX_TRACKING
	DramcDQSOSCShuSettings(DramConfig);
	#endif

///TODO: wait for porting +++
#if GATING_ADJUST_TXDLY_FOR_TRACKING
	DramcRxdqsGatingPostProcess(DramConfig);
#endif

#if TDQSCK_PRECALCULATION_FOR_DVFS
	DramcDQSPrecalculation_preset(DramConfig);
#endif

#if SIMULATION_RX_DVS
	if (DramConfig->frequency >=2133)
		DramcDramcRxDVSCalPostProcess(DramConfig);
#endif

#if XRTWTW_NEW_CROSS_RK_MODE
	if (DramConfig->support_rank_num == RANK_DUAL)
	{
		XRTWTW_SHU_Setting(DramConfig);
	}
#endif

#if DV_SIMULATION_DATLAT
	DramcDualRankRxdatlatCal(DramConfig);
#endif

#if RDSEL_TRACKING_EN
	if (DramConfig->frequency != 400)
		RDSELRunTimeTracking_preset(DramConfig);
#endif

///TODO: wait for porting ---

}

static void DPI_vDramCalibrationAllChannel(DRAMC_CTX_T *DramConfig, cal_sv_rand_args_t *psra)
{
	U8 channel_idx, rank_idx;

	CKEFixOnOff(DramConfig, CKE_WRITE_TO_ALL_RANK, CKE_FIXOFF, CKE_WRITE_TO_ALL_CHANNEL);
	for (channel_idx = CHANNEL_A; channel_idx < DramConfig->support_channel_num; channel_idx++)
	{
		vSetPHY2ChannelMapping(DramConfig, channel_idx);// when switching channel, must update PHY to Channel Mapping
		CKEFixOnOff(DramConfig, CKE_WRITE_TO_ALL_RANK, CKE_FIXON, CKE_WRITE_TO_ONE_CHANNEL);
		DPI_vDramCalibrationSingleChannel(DramConfig, psra);
	}

	vSetPHY2ChannelMapping(DramConfig, CHANNEL_A);

///TODO: wait for porting +++
#if ENABLE_READ_DBI
	DramcReadDBIOnOff(DramConfig, DramConfig->DBI_R_onoff[DramConfig->dram_fsp]);
#endif

#if ENABLE_WRITE_DBI
	// Just settle the DBI parameters which would be stored into shuffle space.
	if (DramConfig->DBI_W_onoff[DramConfig->dram_fsp])
	{
		for (channel_idx = CHANNEL_A; channel_idx < DramConfig->support_channel_num; channel_idx++)
		{
			vSetPHY2ChannelMapping(DramConfig, channel_idx);

			for (rank_idx = RANK_0; rank_idx < DramConfig->support_rank_num; rank_idx++)
			{
				vSetRank(DramConfig, rank_idx);
				DramcWriteShiftMCKForWriteDBI(DramConfig, -1); //Tx DQ/DQM -1 MCK for write DBI ON
			}
			vSetRank(DramConfig, RANK_0);
		}
		vSetPHY2ChannelMapping(DramConfig, CHANNEL_A);

		// Improve Write DBI Power
		ApplyWriteDBIPowerImprove(DramConfig, ENABLE);

		#if ENABLE_WRITE_DBI_Protect
		ApplyWriteDBIProtect(DramConfig, ENABLE);
		#endif
	}
	DramcWriteDBIOnOff(DramConfig, DramConfig->DBI_W_onoff[DramConfig->dram_fsp]);


#endif

#if XRTRTR_NEW_CROSS_RK_MODE
	if (DramConfig->support_rank_num == RANK_DUAL)
	{
		XRTRTR_SHU_Setting(DramConfig);
	}
#endif

#if DV_SIMULATION_DFS
#if (ENABLE_TX_TRACKING || TDQSCK_PRECALCULATION_FOR_DVFS)
	FreqJumpRatioCalculation(DramConfig);
#endif
#endif

#ifdef TEMP_SENSOR_ENABLE
	DramcHMR4_Presetting(DramConfig);
#endif

#if (ENABLE_PER_BANK_REFRESH == 1)
	DramcEnablePerBankRefresh(DramConfig, ON);
#else
	DramcEnablePerBankRefresh(DramConfig, OFF);
#endif

#if ENABLE_TX_TRACKING
	U8 backup_channel = DramConfig->channel;
	U8 channelIdx;

	for (channelIdx = CHANNEL_A; channelIdx < DramConfig->support_channel_num; channelIdx++)
	{
		vSetPHY2ChannelMapping(DramConfig, channelIdx);
		DramcHwDQSOSC(DramConfig);
	}

	vSetPHY2ChannelMapping(DramConfig, backup_channel);
	mcSHOW_DBG_MSG(("TX_TRACKING: ON\n"));
#else
	mcSHOW_DBG_MSG(("TX_TRACKING: OFF\n"));
#endif

///TODO: wait for porting ---

}

///TODO: wait for porting +++
#if __A60868_TO_BE_PORTING__
void RG_dummy_write(DRAMC_CTX_T *p, U32 pattern)
{
	unsigned int ii;
	for (ii = 0; ii < 20; ii++)
		vIO32WriteFldAlign(DDRPHY_RFU_0X1D4, pattern, RFU_0X1D4_RESERVED_0X1D4);
}

void EnablePLLtoSPMControl(DRAMC_CTX_T *p)
{
	vIO32WriteFldAlign_All(DDRPHY_MISC_SPM_CTRL1, 0, MISC_SPM_CTRL1_SPM_DVFS_CONTROL_SEL); // DFS SPM mode for calibration
}
#endif // __A60868_TO_BE_PORTING__
///TODO: wait for porting ---

void dump_dramc_ctx(DRAMC_CTX_T *p)
{
	mcSHOW_DBG_MSG(("== DRAMC_CTX_T ==\n"));
	mcSHOW_DBG_MSG(("support_channel_num:	 %d\n", p->support_channel_num));
	mcSHOW_DBG_MSG(("channel:				 %d\n", p->channel));
	mcSHOW_DBG_MSG(("support_rank_num:		 %d\n", p->support_rank_num));
	mcSHOW_DBG_MSG(("rank:					 %d\n", p->rank));
	mcSHOW_DBG_MSG(("freq_sel:				 %d\n", p->freq_sel));
	mcSHOW_DBG_MSG(("shu_type:				 %d\n", p->shu_type));
	mcSHOW_DBG_MSG(("dram_type: 			 %d\n", p->dram_type));
	mcSHOW_DBG_MSG(("dram_fsp:				 %d\n", p->dram_fsp));
	mcSHOW_DBG_MSG(("odt_onoff: 			 %d\n", p->odt_onoff));
	mcSHOW_DBG_MSG(("dram_cbt_mode: 		 %d, %d\n", (int)p->dram_cbt_mode[0], (int)p->dram_cbt_mode[1]));
	mcSHOW_DBG_MSG(("DBI_R_onoff:			 %d, %d\n", (int)p->DBI_R_onoff[0], (int)p->DBI_R_onoff[1]));
	mcSHOW_DBG_MSG(("DBI_W_onoff:			 %d, %d\n", (int)p->DBI_W_onoff[0], (int)p->DBI_W_onoff[1]));
	mcSHOW_DBG_MSG(("data_width:			 %d\n", p->data_width));
	mcSHOW_DBG_MSG(("test2_1:			  0x%x\n", p->test2_1));
	mcSHOW_DBG_MSG(("test2_2:			  0x%x\n", p->test2_2));
	mcSHOW_DBG_MSG(("frequency: 			 %d\n", p->frequency));
	mcSHOW_DBG_MSG(("freqGroup: 			 %d\n", p->freqGroup));
	mcSHOW_DBG_MSG(("lp5_training_mode: 			 %d\n", p->lp5_training_mode));
	mcSHOW_DBG_MSG(("lp5_cbt_phase: 			 %d\n", p->lp5_cbt_phase));
	mcSHOW_DBG_MSG(("new_cbt_mode:				%d\n", p->new_cbt_mode));
	mcSHOW_DBG_MSG(("u1PLLMode: 			 %d\n", p->u1PLLMode));
	mcSHOW_DBG_MSG(("curDBIState:			   %d\n", p->curDBIState));
}


void DPI_SW_main_LP4(DRAMC_CTX_T *ExtConfig, cal_sv_rand_args_t *psra)
{
	u32 value;
#if DV_SIMULATION_DFS
	S8 s1ShuIdx;
#endif

	DRAMC_CTX_T *p = &DramCtx_LPDDR4; //default;

	p->dram_type = ExtConfig->dram_type;
	if(p->dram_type==TYPE_LPDDR5)
	{
		MEM_TYPE = LPDDR5;
	}
	else
	{
		MEM_TYPE = LPDDR4;
	}

	p->dram_cbt_mode[0] = ExtConfig->dram_cbt_mode[0];
	p->dram_cbt_mode[1] = ExtConfig->dram_cbt_mode[1];
	p->freq_sel = ExtConfig->freq_sel;
	p->frequency = ExtConfig->frequency;
	p->freqGroup = ExtConfig->freqGroup;
	p->new_cbt_mode = ExtConfig->new_cbt_mode;

#if 0 // for Refs
DRAM_DFS_FREQUENCY_TABLE_T gFreqTbl[DRAM_DFS_SHUFFLE_MAX] = {
	{LP4_DDR3200 /*0*/, DIV8_MODE, SRAM_SHU1, DUTY_LAST_K,	VREF_CALI_OFF,	CLOSE_LOOP_MODE},  // highest freq of term group (3733) must k first.
	{LP4_DDR4266 /*1*/, DIV8_MODE, SRAM_SHU0, DUTY_NEED_K,	VREF_CALI_ON,	CLOSE_LOOP_MODE},  // highest freq of term group (3733) must k first.
	{LP4_DDR800  /*2*/, DIV4_MODE, SRAM_SHU6, DUTY_DEFAULT, VREF_CALI_OFF,	SEMI_OPEN_LOOP_MODE},  //Darren: DDR1600 for MRW (DramcModeRegInit_LP4 and CBT)
	{LP4_DDR1866 /*3*/, DIV8_MODE, SRAM_SHU3, DUTY_LAST_K,	VREF_CALI_OFF,	CLOSE_LOOP_MODE},  // highest freq of unterm group (2400) must k first.
	{LP4_DDR1200 /*4*/, DIV8_MODE, SRAM_SHU5, DUTY_LAST_K,	VREF_CALI_OFF,	CLOSE_LOOP_MODE},  // highest freq of unterm group (2400) must k first.
	{LP4_DDR2400 /*5*/, DIV8_MODE, SRAM_SHU2, DUTY_NEED_K,	VREF_CALI_ON,	CLOSE_LOOP_MODE},  // highest freq of unterm group (2400) must k first.
	{LP4_DDR1600 /*6*/, DIV8_MODE, SRAM_SHU4, DUTY_DEFAULT, VREF_CALI_ON,  CLOSE_LOOP_MODE},  //Darren: DDR1600 for MRW (DramcModeRegInit_LP4 and CBT)
};
#endif
	if (u1IsLP4Family(p->dram_type))
	{
		if((ExtConfig->freq_sel==LP4_DDR3733) || (ExtConfig->freq_sel==LP4_DDR4266))
		{
			p->pDFSTable = &gFreqTbl[1];
		}
		else if(ExtConfig->freq_sel==LP4_DDR1600)
		{
			p->pDFSTable = &gFreqTbl[6];
		}
		else if(ExtConfig->freq_sel==LP4_DDR800)
		{
			p->pDFSTable = &gFreqTbl[2];
		}
		/*else if(ExtConfig->freq_sel==LP4_DDR400)
		{
			p->pDFSTable = &gFreqTbl[2];
		}*/
	}

	enter_function();

	if (!psra) {
		/*
		 * for SA's simulation
		 */
		mcSHOW_DBG_MSG(("enter SA's simulation flow.\n"));
		p->support_channel_num = CHANNEL_SINGLE;
		p->channel = CHANNEL_A;
		p->support_rank_num = RANK_DUAL;
		/* DramRank */
		p->rank = RANK_0;
		/* DRAMC operation clock frequency in MHz */
		#if (fcFOR_CHIP_ID == fcA60868)
		#if DV_SIMULATION_DFS
		p->pDFSTable = &gFreqTbl[DRAM_DFS_SHUFFLE_2];
		p->shu_type = DRAM_DFS_SHUFFLE_2;
		#endif
		#endif
#if 0
		/* DRAM type */
		#if DV_SIMULATION_LP4
		p->dram_type = TYPE_LPDDR4X;
		//p->freq_sel = LP4_DDR3200;//DV_SIMULATION_RUN_FREQ_SEL;
		//p->frequency = 1600;//DV_SIMULATION_RUN_FREQ;
		p->freq_sel = LP4_DDR1600;//DV_SIMULATION_RUN_FREQ_SEL;
		p->frequency = 800;//DV_SIMULATION_RUN_FREQ;
		#else
		p->dram_type = TYPE_LPDDR5;
		p->freq_sel = LP5_DDR3200;//DV_SIMULATION_RUN_FREQ_SEL;
		p->frequency = 1600;//DV_SIMULATION_RUN_FREQ;
		#endif
#endif
		/* DRAM Fast switch point type, only for LP4, useless in LP3 */
		p->dram_fsp = FSP_0;

#if 0
		#if DV_SIMULATION_BYTEMODE
		p->dram_cbt_mode[RANK_0] = CBT_BYTE_MODE1;
		p->dram_cbt_mode[RANK_1] = CBT_BYTE_MODE1;
		#else
		p->dram_cbt_mode[RANK_0] = CBT_NORMAL_MODE;
		p->dram_cbt_mode[RANK_1] = CBT_NORMAL_MODE;
		#endif
#endif
		/* IC and DRAM read DBI */
		p->DBI_R_onoff[FSP_0] = DBI_OFF; /* only for LP4, uesless in LP3 */
		p->DBI_R_onoff[FSP_1] = DBI_OFF; /* only for LP4, uesless in LP3 */
		#if ENABLE_READ_DBI
		p->DBI_R_onoff[FSP_1] = DBI_ON; /* only for LP4, uesless in LP3 */
		#else
		p->DBI_R_onoff[FSP_1] = DBI_OFF; /* only for LP4, uesless in LP3 */
		#endif
		/* IC and DRAM write DBI */
		p->DBI_W_onoff[FSP_0] = DBI_OFF; /* only for LP4, uesless in LP3 */
		p->DBI_W_onoff[FSP_1] = DBI_OFF; /* only for LP4, uesless in LP3 */
		#if ENABLE_WRITE_DBI
		p->DBI_W_onoff[FSP_1] = DBI_ON; /* only for LP4, uesless in LP3 */
		#else
		p->DBI_W_onoff[FSP_1] = DBI_OFF; /* only for LP4, uesless in LP3 */
		#endif
		/* bus width */
		p->data_width = DATA_WIDTH_16BIT;
		/* DRAMC internal test engine-2 parameters in calibration */
		p->test2_1 = DEFAULT_TEST2_1_CAL;
		p->test2_2 = DEFAULT_TEST2_2_CAL;
		/* DRAMC test pattern in calibration */
		p->test_pattern = TEST_XTALK_PATTERN;
		/* u2DelayCellTimex100 */
		p->u2DelayCellTimex100 = 250; // @Darren, 2.5ps
		p->vendor_id = 0x1;
		p->density = 0;
		/* p->ranksize = {0,0}; */
		p->ShuRGAccessIdx = DRAM_DFS_REG_SHU0;
		#if DV_SIMULATION_LP5_TRAINING_MODE1
		p->lp5_training_mode = TRAINING_MODE1;
		#else
		p->lp5_training_mode = TRAINING_MODE2;
		#endif

		#if DV_SIMULATION_LP5_CBT_PHASH_R
		p->lp5_cbt_phase = CBT_PHASE_RISING;
		#else
		p->lp5_cbt_phase = CBT_PHASE_FALLING;
		#endif
	} else {
		/*
		 * for DV's regression
		 */
		mcSHOW_DBG_MSG(("enter DV's regression flow.\n"));
		p->support_channel_num = CHANNEL_SINGLE;
		p->channel = psra->calibration_channel;
		p->support_rank_num = RANK_DUAL;
		/* DramRank */
		p->rank = psra->calibration_rank;
		/* DRAMC operation clock frequency in MHz */
		#if (fcFOR_CHIP_ID == fcA60868)
		#if DV_SIMULATION_DFS
		p->pDFSTable = &gFreqTbl[DRAM_DFS_SHUFFLE_2];
		p->shu_type = DRAM_DFS_SHUFFLE_2;
		#endif
		#endif

		/* DRAM type */
		//p->dram_type = psra->dram_type;
		//p->freq_sel = LP5_DDR4266;//DV_SIMULATION_RUN_FREQ_SEL;
		//p->frequency = 2133;//DV_SIMULATION_RUN_FREQ;
		//set_type_freq_by_svargs(p, psra);

		/* DRAM Fast switch point type, only for LP4, useless in LP3 */
		p->dram_fsp = FSP_0;

		p->dram_cbt_mode[RANK_0] = psra->rk0_cbt_mode;
		p->dram_cbt_mode[RANK_1] = psra->rk1_cbt_mode;

		/* IC and DRAM read DBI */
		p->DBI_R_onoff[FSP_0] = (psra->mr3_value >> 6) & 0x1; /* only for LP4, uesless in LP3 */
		p->DBI_R_onoff[FSP_1] = (psra->mr3_value >> 6) & 0x1; /* only for LP4, uesless in LP3 */
		p->DBI_R_onoff[FSP_2] = (psra->mr3_value >> 6) & 0x1;
		/* IC and DRAM write DBI */
		p->DBI_W_onoff[FSP_0] = (psra->mr3_value >> 7) & 0x1; /* only for LP4, uesless in LP3 */
		p->DBI_W_onoff[FSP_1] = (psra->mr3_value >> 7) & 0x1; /* only for LP4, uesless in LP3 */
		p->DBI_W_onoff[FSP_2] = (psra->mr3_value >> 7) & 0x1;
		/* bus width */
		p->data_width = DATA_WIDTH_16BIT;
		/* DRAMC internal test engine-2 parameters in calibration */
		p->test2_1 = DEFAULT_TEST2_1_CAL;
		p->test2_2 = DEFAULT_TEST2_2_CAL;
		/* DRAMC test pattern in calibration */
		p->test_pattern = TEST_XTALK_PATTERN;
		/* u2DelayCellTimex100 */
		p->u2DelayCellTimex100 = 0;
		p->vendor_id = 0x1;
		p->density = 0;
		/* p->ranksize = {0,0}; */
		p->ShuRGAccessIdx = DRAM_DFS_REG_SHU0;
		p->lp5_training_mode = psra->cbt_training_mode;
		p->lp5_cbt_phase = psra->cbt_phase;
		p->new_cbt_mode = psra->new_cbt_mode;
	}

#if QT_GUI_Tool
	p->lp5_training_mode = ExtConfig->lp5_training_mode;
#endif

	if (psra && is_lp5_family(p)) {
		p->dram_fsp = (psra->mr16_value >> 2) & 0x3;
	} else if (psra && u1IsLP4Family(p->dram_type)) {
		p->dram_fsp = (psra->mr13_value >> 7) & 0x1;
	}

//	  p->dram_type = TYPE_LPDDR5;
//	  #define __FW_VER__ "WCK leveling with DLY +16! and MRinit for FSP1 -- 777"
	#define __FW_VER__ "All struct move done, new RX range -- 444"

	if (u1IsLP4Family(p->dram_type)) {
		mcSHOW_DBG_MSG(("%s enter == LP4 == ...%s\n", __FUNCTION__, __FW_VER__));
	} else {
		mcSHOW_DBG_MSG(("%s enter == LP5 == ...%s\n", __FUNCTION__, __FW_VER__));
	}
	mcSHOW_DBG_MSG((CHK_INCLUDE_LOCAL_HEADER));

	mcSHOW_DBG_MSG(("SIMULATION_LP4_ZQ			 ... %d\n", SIMULATION_LP4_ZQ));
	mcSHOW_DBG_MSG(("SIMULATION_SW_IMPED		 ... %d\n", SIMULATION_SW_IMPED));
	mcSHOW_DBG_MSG(("SIMULATION_MIOCK_JMETER	 ... %d\n", SIMULATION_MIOCK_JMETER));
	mcSHOW_DBG_MSG(("SIMULATION_8PHASE			 ... %d\n", SIMULATION_8PHASE));
	mcSHOW_DBG_MSG(("SIMULATION_RX_INPUT_BUF	 ... %d\n", SIMULATION_RX_INPUT_BUF));
	mcSHOW_DBG_MSG(("SIMUILATION_CBT			 ... %d\n", SIMUILATION_CBT));
	mcSHOW_DBG_MSG(("SIMULATION_WRITE_LEVELING	 ... %d\n", SIMULATION_WRITE_LEVELING));
	mcSHOW_DBG_MSG(("SIMULATION_DUTY_CYC_MONITOR ... %d\n", SIMULATION_DUTY_CYC_MONITOR));
	mcSHOW_DBG_MSG(("SIMULATION_GATING			 ... %d\n", SIMULATION_GATING));
	mcSHOW_DBG_MSG(("SIMULATION_DATLAT			 ... %d\n", SIMULATION_DATLAT));
	mcSHOW_DBG_MSG(("SIMULATION_RX_RDDQC		 ... %d\n", SIMULATION_RX_RDDQC));
	mcSHOW_DBG_MSG(("SIMULATION_RX_PERBIT		 ... %d\n", SIMULATION_RX_PERBIT));
	mcSHOW_DBG_MSG(("SIMULATION_TX_PERBIT		 ... %d\n", SIMULATION_TX_PERBIT));
	mcSHOW_DBG_MSG(("\n\n"));

	mcSHOW_DBG_MSG(("============== CTX before calibration ================\n"));
	dump_dramc_ctx(p);

	DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);

	//vIO32Write4B_All2(p, DDRPHY_SHU_RK_CA_CMD1, 0x0FFF);
	value = u4Dram_Register_Read(p, DRAMC_REG_DDRCOMMON0);
	mcSHOW_DBG_MSG(("Get Addr:0x%x, Value:0x%x\n", DRAMC_REG_DDRCOMMON0, value));

	value = u4Dram_Register_Read(p, DDRPHY_REG_SHU_RK_CA_CMD1);
	mcSHOW_DBG_MSG(("Get Addr:0x%x, Value:0x%x\n", DDRPHY_REG_SHU_RK_CA_CMD1, value));

	value = u4Dram_Register_Read(p, DDRPHY_REG_MISC_DQO1);
	mcSHOW_DBG_MSG(("Get Addr:0x%x, Value:0x%x\n", DDRPHY_REG_MISC_DQO1, value));

	value = u4Dram_Register_Read(p, DDRPHY_MD32_REG_SSPM_TIMER0_RESET_VAL );
	mcSHOW_DBG_MSG(("Get Addr:0x%x, Value:0x%x\n", DDRPHY_MD32_REG_SSPM_TIMER0_RESET_VAL, value));

	DramcBroadcastOnOff(DRAMC_BROADCAST_ON); //LP4 broadcast on

	Global_Option_Init(p);

#if __A60868_TO_BE_PORTING__
	vDramcInit_PreSettings(p);

	DDRPhyFreqSel(p, p->pDFSTable->freq_sel);

	vSetPHY2ChannelMapping(p, p->channel);
#endif // __A60868_TO_BE_PORTING__
	///TODO: wait for porting ---


	if (u1IsLP4Family(p->dram_type))
	{
		vSetDFSFreqSelByTable(p, p->pDFSTable); // for LP4x
	}
	else ///TODO: Jeremy, modify this when LP5 gFreqtbl ready
	{
		DDRPhyFreqSel(p, p->freq_sel);
	}

#if (SIMULATION_SW_IMPED == 1)
	mcSHOW_DBG_MSG(("\n----->DramcSwImpedanceCal begin...\n"));
	timestamp_show();
	// LP4 IMP_LOW_FREQ <= DDR3733, IMP_HIGH_FREQ >= DDR4266
	// LP5 IMP_LOW_FREQ <= DDR3733, IMP_HIGH_FREQ >= DDR4266
	DramcSwImpedanceCal(p, 1, IMP_LOW_FREQ);
	DramcSwImpedanceCal(p, 1, IMP_HIGH_FREQ);
	timestamp_show();
	mcSHOW_DBG_MSG(("DramcSwImpedanceCal end<-----\n\n"));
#endif /* (SIMULATION_SW_IMPED == 1) */

#if DV_SIMULATION_INIT_C
	///TODO: wait for porting +++
	DramcInit(p);

	// Before calibration setting
	vBeforeCalibration(p);
#if __A60868_TO_BE_PORTING__
	#if DV_SIMULATION_BEFORE_K
	vApplyConfigBeforeCalibration(p);
	//vMR2InitForSimulationTest(p);
	#endif

#ifdef DUMP_INIT_RG_LOG_TO_DE
	#if 0 //Dump RG to other shuffle for FT used, don't delete
		mcSHOW_DUMP_INIT_RG_MSG(("\n\n\n\n\n\n===== Save to Shuffle RG ======\n"));
		DramcSaveToShuffleReg(p, DRAM_DFS_SHUFFLE_1, DRAM_DFS_SHUFFLE_3);
	#endif
		while (1);
#endif
#endif
#endif // __A60868_TO_BE_PORTING__
	///TODO: wait for porting ---


#if (SIMULATION_MIOCK_JMETER == 1)
	mcSHOW_DBG_MSG(("\n----->DramcMiockJmeter begin...\n"));
	timestamp_show();
	PRE_MIOCK_JMETER_HQA_USED(p);
	timestamp_show();
	mcSHOW_DBG_MSG(("DramcMiockJmeter end<-----\n\n"));
#endif /* (SIMULATION_MIOCK_JMETER == 1) */

#if (SIMULATION_8PHASE == 1)
	if(is_lp5_family(p) && (p->frequency >= 2133)) {
		mcSHOW_DBG_MSG(("\n----->Dramc8PhaseCal begin...\n"));
		timestamp_show();
		Dramc8PhaseCal(p); // it must set before duty calib
		timestamp_show();
		mcSHOW_DBG_MSG(("Dramc8PhaseCal end<-----\n\n"));
	}
#endif /* (SIMULATION_8PHASE == 1) */

	///TODO: wait for porting +++
	#if !DV_SIMULATION_DFS // No calib will use legacy mode init settings
	DPI_vDramCalibrationAllChannel(p, psra); // for DDR1600 1:8 mode
	#endif

#if DV_SIMULATION_DFS
	DramcSaveToShuffleSRAM(p, DRAM_DFS_SHUFFLE_1, p->pDFSTable->shuffleIdx);
	LoadShuffleSRAMtoDramc(p, p->pDFSTable->shuffleIdx, DRAM_DFS_SHUFFLE_2); //Darren: DDR1600 for MRW (DramcModeRegInit_LP4 and CBT)
	#if ENABLE_SRAM_DMA_WA
	DPHYSaveToSRAMShuWA(p, p->pDFSTable->shuffleIdx);
	#endif

	#if (fcFOR_CHIP_ID == fcA60868)
	for (s1ShuIdx = DRAM_DFS_SHUFFLE_MAX - 10; s1ShuIdx >= DRAM_DFS_SHUFFLE_1; s1ShuIdx--)
	#else
	for (s1ShuIdx = DRAM_DFS_SHUFFLE_MAX - 2; s1ShuIdx >= DRAM_DFS_SHUFFLE_1; s1ShuIdx--)
	#endif
	{
		vSetDFSFreqSelByTable(p, &gFreqTbl[s1ShuIdx]);
		DramcInit(p);
		// Before calibration setting
		vBeforeCalibration(p);

		#if DV_SIMULATION_BEFORE_K
		vApplyConfigBeforeCalibration(p);
		#endif

		#if (SIMULATION_8PHASE == 1)
		if(is_lp5_family(p) && (p->frequency >= 2133)) {
			mcSHOW_DBG_MSG(("\n----->Dramc8PhaseCal begin...\n"));
			timestamp_show();
			Dramc8PhaseCal(p); // it must set before duty calib
			timestamp_show();
			mcSHOW_DBG_MSG(("Dramc8PhaseCal end<-----\n\n"));
		}
		#endif /* (SIMULATION_8PHASE == 1) */

		#if !DV_SIMULATION_DFS // No calib will use legacy mode init settings
		DPI_vDramCalibrationAllChannel(p, psra); // for gDVDFSTbl
		#endif
		DramcSaveToShuffleSRAM(p, DRAM_DFS_SHUFFLE_1, gFreqTbl[s1ShuIdx].shuffleIdx);
		#if ENABLE_SRAM_DMA_WA
		DPHYSaveToSRAMShuWA(p, gFreqTbl[s1ShuIdx].shuffleIdx);
		#endif
	}
#endif
	///TODO: wait for porting ---




	///TODO: wait for porting +++
	vAfterCalibration(p);

#if SIMULATION_RUNTIME_CONFIG
	DramcRunTimeConfig(p);
#endif

#if __A60868_TO_BE_PORTING__
#if DV_SIMULATION_AFTER_K
	vApplyConfigAfterCalibration(p);
#endif

#if DV_SIMULATION_RUN_TIME_MRW
	enter_pasr_dpd_config(0, 0xFF);
#endif

#if DV_SIMULATION_RUN_TIME_MRR
	DramcModeRegReadByRank(p, RANK_0, 4, &u2val1);
	DramcModeRegReadByRank(p, RANK_0, 5, &u2val2);
	DramcModeRegReadByRank(p, RANK_0, 8, &u2val3);
	mcSHOW_DBG_MSG(("[Runtime time MRR] MR4 = 0x%x, MR5 = 0x%x, MR8 = 0x%x\n", u2val1, u2val2, u2val3));
#endif

#if 0//DV_SIMULATION_DFS // NOTE: Don't use DramcDFSDirectJump_SPMMode. it will cause NULL object access.
	// high freq -> low freq
	for (s1ShuIdx = 0; s1ShuIdx < DV_SIMULATION_DFS_SHU_MAX; s1ShuIdx++)
			DramcDFSDirectJump_SRAMShuRGMode(p, gDVDFSTbl[s1ShuIdx].shuffleIdx);
	// low freq -> high freq
	for (s1ShuIdx = DV_SIMULATION_DFS_SHU_MAX - 1; s1ShuIdx >= DRAM_DFS_SHUFFLE_1; s1ShuIdx--)
			DramcDFSDirectJump_SRAMShuRGMode(p, gDVDFSTbl[s1ShuIdx].shuffleIdx);
#endif

#if DV_SIMULATION_SPM_CONTROL
	EnablePLLtoSPMControl(p);
#endif

	RG_dummy_write(p, 0xAAAAAAAA);
#endif // __A60868_TO_BE_PORTING__
	///TODO: wait for porting ---

	//Temp_TA2_Test_After_K(p);

	//Ett_Mini_Strss_Test(p);
#if MRW_CHECK_ONLY
	vPrintFinalModeRegisterSetting(p);
#endif
#if PRINT_CALIBRATION_SUMMARY
	vPrintCalibrationResult(p);
#endif

	exit_function();
}

///TODO: wait for porting +++
#if __A60868_TO_BE_PORTING__
#if SW_CHANGE_FOR_SIMULATION
void main(void)
{

	DRAMC_CTX_T DramConfig;
	DramConfig.channel = CHANNEL_A;
	DramConfig.support_rank_num = RANK_DUAL;
	// DramRank
	DramConfig.rank = RANK_0;
	// DRAM type
	DramConfig.dram_type = TYPE_LPDDR4X;
	// DRAM Fast switch point type, only for LP4, useless in LP3
	DramConfig.dram_fsp = FSP_0;
	// DRAM CBT mode, only for LP4, useless in LP3
	DramConfig.dram_cbt_mode[RANK_0] = CBT_NORMAL_MODE;
	DramConfig.dram_cbt_mode[RANK_1] = CBT_NORMAL_MODE;
	// IC and DRAM read DBI
	DramConfig.DBI_R_onoff[FSP_0] = DBI_OFF;			 // only for LP4, uesless in LP3
	#if ENABLE_READ_DBI
	DramConfig.DBI_R_onoff[FSP_1] = DBI_ON; 			 // only for LP4, uesless in LP3
	#else
	DramConfig.DBI_R_onoff[FSP_1] = DBI_OFF;		// only for LP4, uesless in LP3
	#endif
	// IC and DRAM write DBI
	DramConfig.DBI_W_onoff[FSP_0] = DBI_OFF;			 // only for LP4, uesless in LP3
	#if ENABLE_WRITE_DBI
	DramConfig.DBI_W_onoff[FSP_1] = DBI_ON; 			 // only for LP4, uesless in LP3
	#else
	DramConfig.DBI_W_onoff[FSP_1] = DBI_OFF;		 // only for LP4, uesless in LP3
	#endif
	// bus width
	DramConfig.data_width = DATA_WIDTH_32BIT;
	// DRAMC internal test engine-2 parameters in calibration
	DramConfig.test2_1 = DEFAULT_TEST2_1_CAL;
	DramConfig.test2_2 = DEFAULT_TEST2_2_CAL;
	// DRAMC test pattern in calibration
	DramConfig.test_pattern = TEST_XTALK_PATTERN;
	// DRAMC operation clock frequency in MHz
	DramConfig.frequency = 800;

	//DramConfig.enable_rx_scan_vref =DISABLE;
	//DramConfig.enable_tx_scan_vref =DISABLE;
	//DramConfig.dynamicODT = DISABLE;

	MPLLInit();

	Global_Option_Init(&DramConfig);

	// DramC & PHY init for all channels
	DDRPhyFreqSel(&DramConfig, LP4_DDR1600);


#if WRITE_LEVELING_MOVE_DQS_INSTEAD_OF_CLK
	memset(DramConfig.arfgWriteLevelingInitShif, FALSE, sizeof(DramConfig.arfgWriteLevelingInitShif));
	//>fgWriteLevelingInitShif= FALSE;
#endif

	DramcInit(&DramConfig);

	vApplyConfigBeforeCalibration(&DramConfig);
	vMR2InitForSimulationTest(&DramConfig);

	vSetPHY2ChannelMapping(&DramConfig, DramConfig.channel);

	#if SIMULATION_SW_IMPED
	DramcSwImpedanceCal(&DramConfig, 1, LOW_FREQ);	//for DRVN/P and ODTN
	//DramcSwImpedanceCal(&DramConfig, 1, HIGH_FREQ);  //for DRVN/P and ODTN
	#endif


#if SIMULATION_LP4_ZQ
	 if (DramConfig.dram_type == TYPE_LPDDR4 || DramConfig.dram_type == TYPE_LPDDR4X || DramConfig.dram_type == TYPE_LPDDR4P)
	 {
		 DramcZQCalibration(&DramConfig);
	 }
#endif

	#if SIMUILATION_LP4_CBT
	CmdBusTrainingLP4(&DramConfig);
	#endif

#if SIMULATION_WRITE_LEVELING
	DramcWriteLeveling(&DramConfig);
#endif

	#if SIMULATION_GATING
	// Gating calibration of single rank
	DramcRxdqsGatingCal(&DramConfig);

	// Gating calibration of both rank
	//DualRankDramcRxdqsGatingCal(&DramConfig);
	#endif

#if SIMUILATION_LP4_RDDQC
	DramcRxWindowPerbitCal(&DramConfig, 0, NULL);
#endif

	#if SIMULATION_DATLAT
	// RX Datlat calibration of single rank
	DramcRxdatlatCal(&DramConfig);

	// RX Datlat calibration of two rank
	//DramcDualRankRxdatlatCal(&DramConfig);
	#endif

	#if SIMULATION_RX_PERBIT
	DramcRxWindowPerbitCal(&DramConfig, 1, NULL);
	#endif

	#if SIMULATION_TX_PERBIT
	DramcTxWindowPerbitCal(&DramConfig, TX_DQ_DQS_MOVE_DQ_DQM);
	DramcTxWindowPerbitCal(&DramConfig, TX_DQ_DQS_MOVE_DQ_ONLY);
	#endif

	#if ENABLE_READ_DBI
	//Read DBI ON
	SetDramModeRegForReadDBIOnOff(&DramConfig, DramConfig.dram_fsp, DramConfig.DBI_R_onoff[DramConfig.dram_fsp]);
	#endif

	#if ENABLE_WRITE_DBI
	//Write DBI ON
	DramcWriteShiftMCKForWriteDBI(&DramConfig, -1);
	SetDramModeRegForWriteDBIOnOff(&DramConfig, DramConfig.dram_fsp, DramConfig.DBI_W_onoff[DramConfig.dram_fsp]);
	#endif

	#if ENABLE_READ_DBI
	DramcReadDBIOnOff(&DramConfig, DramConfig.DBI_R_onoff[DramConfig.dram_fsp]);
	#endif

	#if ENABLE_WRITE_DBI
	DramcWriteDBIOnOff(&DramConfig, DramConfig.DBI_W_onoff[DramConfig.dram_fsp]);
	#endif
}
#endif //SW_CHANGE_FOR_SIMULATION
#endif // __A60868_TO_BE_PORTING__
///TODO: wait for porting ---

