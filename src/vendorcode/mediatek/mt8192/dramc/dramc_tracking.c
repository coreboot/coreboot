/* SPDX-License-Identifier: BSD-3-Clause */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "dramc_common.h"
#include "dramc_int_global.h"
#include "x_hal_io.h"
#include "dramc_top.h"


//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

//U8 gu1MR23Done = FALSE; /* Not used starting from Vinson (all freqs use MR23=0x3F) */
U8 gu1MR23[CHANNEL_NUM][RANK_MAX];
/* DQSOSCTHRD_INC & _DEC are 12 bits (Starting from Vinson) */
U16 gu2DQSOSCTHRD_INC[CHANNEL_NUM][RANK_MAX];
U16 gu2DQSOSCTHRD_DEC[CHANNEL_NUM][RANK_MAX];
U16 gu2MR18[CHANNEL_NUM][RANK_MAX]; /* Stores MRR MR18 (DQS ocillator count - MSB) */
U16 gu2MR19[CHANNEL_NUM][RANK_MAX]; /* Stores MRR MR19 (DQS ocillator count - LSB) */
U16 gu2DQSOSC[CHANNEL_NUM][RANK_MAX]; /* Stores tDQSOSC results */
U16 gu2DQSOscCnt[CHANNEL_NUM][RANK_MAX][2];


void DramcDQSOSCInit(void)
{
	memset(gu1MR23, 0x3F, sizeof(gu1MR23)); /* MR23 should be 0x3F for all freqs (Starting from Vinson) */
	memset(gu2DQSOSCTHRD_INC, 0x6, sizeof(gu2DQSOSCTHRD_INC));
	memset(gu2DQSOSCTHRD_DEC, 0x4, sizeof(gu2DQSOSCTHRD_DEC));
}

static DRAM_STATUS_T DramcStartDQSOSC_SWCMD(DRAMC_CTX_T *p)
{
	U32 u4Response;
	U32 u4TimeCnt = TIME_OUT_CNT;
	U32 u4RegBackupAddress[] = {DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL0), DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), DRAMC_REG_ADDR(DRAMC_REG_CKECTRL)};

	// Backup rank, CKE fix on/off, HW MIOCK control settings
	DramcBackupRegisters(p, u4RegBackupAddress, sizeof(u4RegBackupAddress)/sizeof(U32));

	mcSHOW_DBG_MSG3(("[ZQCalibration]\n"));
	//mcFPRINTF((fp_A60501, "[ZQCalibration]\n"));

	// Disable HW MIOCK control to make CLK always on
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), 1, DRAMC_PD_CTRL_APHYCKCG_FIXOFF);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), 1, DRAMC_PD_CTRL_TCKFIXON);
	mcDELAY_US(1);

	//if CKE2RANK=1, only need to set CKEFIXON, it will apply to both rank.
	CKEFixOnOff(p, CKE_WRITE_TO_ALL_RANK, CKE_FIXON, CKE_WRITE_TO_ONE_CHANNEL);

	//ZQCAL Start
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), 1, SWCMD_EN_WCK2DQI_START_SWTRIG);

	do
	{
		u4Response = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SPCMDRESP3), SPCMDRESP3_WCK2DQI_START_SWTRIG_RESPONSE);
		u4TimeCnt --;
		mcDELAY_US(1);	// Wait tZQCAL(min) 1us or wait next polling

		mcSHOW_DBG_MSG3(("%d- ", u4TimeCnt));
		//mcFPRINTF((fp_A60501, "%d- ", u4TimeCnt));
	}while((u4Response==0) &&(u4TimeCnt>0));

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), 0, SWCMD_EN_WCK2DQI_START_SWTRIG);

	if(u4TimeCnt==0)//time out
	{
		mcSHOW_DBG_MSG(("ZQCAL Start fail (time out)\n"));
		//mcFPRINTF((fp_A60501, "ZQCAL Start fail (time out)\n"));
		return DRAM_FAIL;
	}

	// Restore rank, CKE fix on, HW MIOCK control settings
	DramcRestoreRegisters(p, u4RegBackupAddress, sizeof(u4RegBackupAddress)/sizeof(U32));

	mcSHOW_DBG_MSG3(("\n[DramcZQCalibration] Done\n\n"));
	//mcFPRINTF((fp_A60501, "\n[DramcZQCalibration] Done\n\n"));

	return DRAM_OK;
}

static DRAM_STATUS_T DramcStartDQSOSC(DRAMC_CTX_T *p)
{
//	return DramcStartDQSOSC_SCSM(p);
//	return DramcStartDQSOSC_RTSWCMD(p);
	return DramcStartDQSOSC_SWCMD(p);
}

DRAM_STATUS_T DramcDQSOSCAuto(DRAMC_CTX_T *p)
{
	U8 u1MR23 = gu1MR23[p->channel][p->rank];
	U16 u2MR18, u2MR19;
	U16 u2DQSCnt;
	U16 u2DQSOsc[2];
	U32 u4RegBak[2];

#if MRW_CHECK_ONLY
	mcSHOW_MRW_MSG(("\n==[MR Dump] %s==\n", __func__));
#endif

	u4RegBak[0] = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL));
	u4RegBak[1] = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL));

	//LPDDR4-3200,	   PI resolution = tCK/64 =9.76ps
	//Only if MR23>=16, then error < PI resolution.
	//Set MR23 == 0x3f, stop after 63*16 clock
	//vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_MRS), u1GetRank(p), MRS_MRSRK);
	DramcModeRegWriteByRank(p, p->rank, 23, u1MR23);

	//SW mode
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_DQSOSC_SET0), 1, SHU_DQSOSC_SET0_DQSOSCENDIS);

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), 1, DRAMC_PD_CTRL_APHYCKCG_FIXOFF);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), 1, DRAMC_PD_CTRL_TCKFIXON);

	CKEFixOnOff(p, p->rank, CKE_FIXON, CKE_WRITE_TO_ONE_CHANNEL);

	DramcStartDQSOSC(p);
	mcDELAY_US(1);

	DramcModeRegReadByRank(p, p->rank, 18, &u2MR18);
	DramcModeRegReadByRank(p, p->rank, 19, &u2MR19);

#if (SW_CHANGE_FOR_SIMULATION == 0)
	//B0
	u2DQSCnt = (u2MR18 & 0x00FF) | ((u2MR19 & 0x00FF) << 8);
	if (u2DQSCnt != 0)
		u2DQSOsc[0] = u1MR23 * 16 * 1000000 / (2 * u2DQSCnt * p->frequency); //tDQSOSC = 16*MR23*tCK/2*count
	else
		u2DQSOsc[0] = 0;

	//B1
	u2DQSCnt = (u2MR18 >> 8) | ((u2MR19 & 0xFF00));
	if (u2DQSCnt != 0)
		u2DQSOsc[1] = u1MR23 * 16 * 1000000 / (2 * u2DQSCnt * p->frequency); //tDQSOSC = 16*MR23*tCK/2*count
	else
		u2DQSOsc[1] = 0;
	mcSHOW_DBG_MSG(("[DQSOSCAuto] RK%d, (LSB)MR18= 0x%x, (MSB)MR19= 0x%x, tDQSOscB0 = %d ps tDQSOscB1 = %d ps\n", u1GetRank(p), u2MR18, u2MR19, u2DQSOsc[0], u2DQSOsc[1]));
#endif

	gu2MR18[p->channel][p->rank] = u2MR18;
	gu2MR19[p->channel][p->rank] = u2MR19;
	gu2DQSOSC[p->channel][p->rank] = u2DQSOsc[0];

	if (u2DQSOsc[1] != 0 && u2DQSOsc[1] < u2DQSOsc[0])
		gu2DQSOSC[p->channel][p->rank] = u2DQSOsc[1];

	vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), u4RegBak[0]);
	vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL), u4RegBak[1]);

	return DRAM_OK;
}


#if ENABLE_TX_TRACKING
/* Using gu2DQSOSC results calculated from DramcDQSOSCAuto
 * -> calculate DQSOSCTHRD_INC, DQSOSCTHRD_DEC
 * _INC, _DEC formulas are extracted from "Verification plan of Vinson LPDDR4 HW TX Tracking" doc
 */
DRAM_STATUS_T DramcDQSOSCMR23(DRAMC_CTX_T *p)
{
#if (SW_CHANGE_FOR_SIMULATION == 0)
	/* Preloader doesn't support floating point numbers -> Manually expand/simpify _INC, _DEC formula */
	U8 u1MR23 = gu1MR23[p->channel][p->rank];
	U16 u2DQSOSC = gu2DQSOSC[p->channel][p->rank];
	U32 u4tCK = 1000000 / p->frequency;

	if (u2DQSOSC != 0)
	{
		gu2DQSOSCTHRD_INC[p->channel][p->rank] = (3 * u1MR23 * u4tCK * u4tCK) / (u2DQSOSC * u2DQSOSC * 20);
		gu2DQSOSCTHRD_DEC[p->channel][p->rank] = (u1MR23 * u4tCK * u4tCK) / (u2DQSOSC * u2DQSOSC * 10);
	}

	mcSHOW_DBG_MSG(("CH%d_RK%d: MR19=0x%X, MR18=0x%X, DQSOSC=%d, MR23=%u, INC=%u, DEC=%u\n", p->channel, p->rank,
					gu2MR19[p->channel][p->rank], gu2MR18[p->channel][p->rank], gu2DQSOSC[p->channel][p->rank],
					u1MR23, gu2DQSOSCTHRD_INC[p->channel][p->rank], gu2DQSOSCTHRD_DEC[p->channel][p->rank]));
#endif
	return DRAM_OK;
}


/* Sets DQSOSC_BASE for specified rank/byte */
DRAM_STATUS_T DramcDQSOSCSetMR18MR19(DRAMC_CTX_T *p)
{
	U16 u2DQSOscCnt[2];

	DramcDQSOSCAuto(p);

	//B0
	gu2DQSOscCnt[p->channel][p->rank][0] = u2DQSOscCnt[0] = (gu2MR18[p->channel][p->rank] & 0x00FF) | ((gu2MR19[p->channel][p->rank] & 0x00FF) << 8);
	//B1
	gu2DQSOscCnt[p->channel][p->rank][1] = u2DQSOscCnt[1] = (gu2MR18[p->channel][p->rank] >> 8) | ((gu2MR19[p->channel][p->rank] & 0xFF00));

	if ((p->dram_cbt_mode[p->rank] == CBT_NORMAL_MODE) && (gu2DQSOscCnt[p->channel][p->rank][1] == 0))
	{
		gu2DQSOscCnt[p->channel][p->rank][1] = u2DQSOscCnt[1] = u2DQSOscCnt[0];
	}

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHURK_DQSOSC), P_Fld(u2DQSOscCnt[0], SHURK_DQSOSC_DQSOSC_BASE_RK0) | P_Fld(u2DQSOscCnt[1], SHURK_DQSOSC_DQSOSC_BASE_RK0_B1));

	mcSHOW_DBG_MSG(("CH%d RK%d: MR19=%X, MR18=%X\n", p->channel, p->rank, gu2MR19[p->channel][p->rank], gu2MR18[p->channel][p->rank]));
	mcDUMP_REG_MSG(("CH%d RK%d: MR19=%X, MR18=%X\n", p->channel, p->rank, gu2MR19[p->channel][p->rank], gu2MR18[p->channel][p->rank]));
	return DRAM_OK;
}

DRAM_STATUS_T DramcDQSOSCShuSettings(DRAMC_CTX_T *p)
{
	U16 u2PRDCNT = 0x3FF, u2PRDCNTtmp = 0x3FF;
	U8 u1PRDCNT_DIV = 4;
	U16 u2DQSOSCENCNT = 0xFFF;
	U8 u1FILT_PITHRD = 0;
	U8 u1W2R_SEL = 0;
	U8 u1RankIdx, u1RankIdxBak;
	U8 u1DQSOSCRCNT = 0, u1IsDiv4 = 0, u1RoundUp= 0;
	u1RankIdxBak = u1GetRank(p);

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_TX_SET0), 0x0, SHU_TX_SET0_DQS2DQ_FILT_PITHRD);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DQSOSCR), 0x0, DQSOSCR_R_DMDQS2DQ_FILT_OPT);
	if (p->frequency <= 400)
	{
		u1FILT_PITHRD = 0x5;
		if (vGet_Div_Mode(p) == DIV4_MODE)
			u1W2R_SEL = 0x2;
		else
		u1W2R_SEL = 0x5;
	}
	else if (p->frequency <= 600)
	{
		u1FILT_PITHRD = 0x6;
		if (vGet_Div_Mode(p) == DIV4_MODE)
			u1W2R_SEL = 0x2;
		else
		u1W2R_SEL = 0x5;
	}
	else if (p->frequency <= 800)
	{
		u1FILT_PITHRD = 0x6;
		if (vGet_Div_Mode(p) == DIV4_MODE)
			u1W2R_SEL = 0x2;
		else
		u1W2R_SEL = 0x5;
	}
	else if (p->frequency <= 933)
	{
		u1FILT_PITHRD = 0x9;
		u1W2R_SEL = 0x2;
	}
	else if (p->frequency <= 1200)
	{
		u1FILT_PITHRD = 0xb;
		u1W2R_SEL = 0x2;
	}
	else if (p->frequency <= 1333)
	{
		u1FILT_PITHRD = 0xc;
		u1W2R_SEL = 0x2;
	}
	else if (p->frequency <= 1600)
	{
		u1FILT_PITHRD = 0xE;
		u1W2R_SEL = 0x2;
	}
	else if (p->frequency <= 1866)
	{
		u1FILT_PITHRD = 0x12;
		u1W2R_SEL = 0x2;
	}
	else	//4266
	{
		u1FILT_PITHRD = 0x17;
		u1W2R_SEL = 0x2;
	}

	if (vGet_Div_Mode(p) == DIV4_MODE)
	{
		u1PRDCNT_DIV = 2;
		u1IsDiv4 = 1;
	}

	u1DQSOSCRCNT = ((p->frequency << u1IsDiv4))/100;
	if ((p->frequency%100) != 0) // @Darren,  Round up for tOSCO timing (40ns)
		u1DQSOSCRCNT++;
	if (gu1MR23[p->channel][RANK_1] > gu1MR23[p->channel][RANK_0])
		u2PRDCNTtmp = ((gu1MR23[p->channel][RANK_1]*100)/u1PRDCNT_DIV);
	else
		u2PRDCNTtmp = ((gu1MR23[p->channel][RANK_0]*100)/u1PRDCNT_DIV);

	 u2PRDCNT = (u2PRDCNTtmp + ((u1DQSOSCRCNT*100)/16))/100;
	 u1RoundUp = (u2PRDCNTtmp + ((u1DQSOSCRCNT*100)/16))%100;
	 if (u1RoundUp != 0)
		u2PRDCNT++;

	//Don't power down dram during DQS interval timer run time, (MR23[7:0] /4) + (tOSCO/MCK unit/16)
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_DQSOSC_SET0), u2PRDCNT, SHU_DQSOSC_SET0_DQSOSC_PRDCNT);

	//set tOSCO constraint to read MR18/MR19, should be > 40ns/MCK
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_DQSOSCR), u1DQSOSCRCNT, SHU_DQSOSCR_DQSOSCRCNT);//@Darren, unit: MCK to meet spec. tOSCO=40ns/MCK
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_TX_SET0), (u1FILT_PITHRD>>1), SHU_TX_SET0_DQS2DQ_FILT_PITHRD);
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_TX_SET0), P_Fld(u1W2R_SEL, SHU_TX_SET0_TXUPD_W2R_SEL) | P_Fld(0x0, SHU_TX_SET0_TXUPD_SEL));

	/* Starting from Vinson, DQSOSCTHRD_INC & _DEC is split into RK0 and RK1 */
	//Rank 0


	for (u1RankIdx = RANK_0; u1RankIdx < p->support_rank_num; u1RankIdx++)
	{
		vSetRank(p, u1RankIdx);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_DQSOSC_THRD), gu2DQSOSCTHRD_INC[p->channel][u1RankIdx], SHURK_DQSOSC_THRD_DQSOSCTHRD_INC);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_DQSOSC_THRD), gu2DQSOSCTHRD_DEC[p->channel][u1RankIdx], SHURK_DQSOSC_THRD_DQSOSCTHRD_DEC);
	}
	vSetRank(p, u1RankIdxBak);

	//set interval to do MPC(start DQSOSC) command, and dramc send DQSOSC start to rank0/1/2 at the same time
	//TX tracking period unit: 3.9us
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_DQSOSC_SET0), u2DQSOSCENCNT, SHU_DQSOSC_SET0_DQSOSCENCNT);

	return DRAM_OK;
}

void DramcHwDQSOSC(DRAMC_CTX_T *p)
{
	DRAM_RANK_T rank_bak = u1GetRank(p);
	DRAM_CHANNEL_T ch_bak = p->channel;

	//Enable TX tracking new mode
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TX_FREQ_RATIO_OLD_MODE0), 1, TX_FREQ_RATIO_OLD_MODE0_SHUFFLE_LEVEL_MODE_SELECT);

	//Enable Freq_RATIO update
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TX_TRACKING_SET0), P_Fld(1, TX_TRACKING_SET0_SHU_PRELOAD_TX_HW)
											| P_Fld(0, TX_TRACKING_SET0_SHU_PRELOAD_TX_START)
											| P_Fld(0, TX_TRACKING_SET0_SW_UP_TX_NOW_CASE));

	//DQSOSC MPC command violation
#if ENABLE_TMRRI_NEW_MODE
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_MPC_CTRL), 1, MPC_CTRL_MPC_BLOCKALE_OPT);
#else
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_MPC_CTRL), 0, MPC_CTRL_MPC_BLOCKALE_OPT);
#endif

	//DQS2DQ UI/PI setting controlled by HW
	#if ENABLE_SW_TX_TRACKING
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CTRL1), 1, MISC_CTRL1_R_DMARPIDQ_SW);
	#else
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CTRL1), 0, MISC_CTRL1_R_DMARPIDQ_SW);
	#if ENABLE_PA_IMPRO_FOR_TX_TRACKING
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DCM_SUB_CTRL), 1, DCM_SUB_CTRL_SUBCLK_CTRL_TX_TRACKING);
	#endif
	#endif
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DQSOSCR), 1, DQSOSCR_ARUIDQ_SW);

	//Set dqsosc oscillator run time by MRW
	//write RK0 MR23
	#if 0
	vSetRank(p, RANK_0);
	vSetPHY2ChannelMapping(p, CHANNEL_A);
	DramcModeRegWrite(p, 23, u1MR23);
	vSetPHY2ChannelMapping(p, CHANNEL_B);
	DramcModeRegWrite(p, 23, u1MR23);
	//write RK1 MR23
	vSetRank(p, RANK_1);
	vSetPHY2ChannelMapping(p, CHANNEL_A);
	DramcModeRegWrite(p, 23, u1MR23);
	vSetPHY2ChannelMapping(p, CHANNEL_B);
	DramcModeRegWrite(p, 23, u1MR23);
	#endif

	//Enable HW read MR18/MR19 for each rank
	#if ENABLE_SW_TX_TRACKING
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DQSOSCR), 1, DQSOSCR_DQSOSCRDIS);
	#else
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DQSOSCR), 0, DQSOSCR_DQSOSCRDIS);
	#endif

	vSetRank(p, RANK_0);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_RK_DQSOSC), 1, RK_DQSOSC_DQSOSCR_RK0EN);
	if (p->support_rank_num == RANK_DUAL)
	{
		vSetRank(p, RANK_1);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_RK_DQSOSC), 1, RK_DQSOSC_DQSOSCR_RK0EN);
	}

	//@Jouling, Update MP setting
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TX_SET0), 1, TX_SET0_DRSCLR_RK0_EN); //Set as 1 to fix issue of RANK_SINGLE, dual rank can also be enable

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DQSOSCR), 1, DQSOSCR_DQSOSC_CALEN);

	vSetRank(p, rank_bak);
	vSetPHY2ChannelMapping(p, ch_bak);
}

void Enable_TX_Tracking(DRAMC_CTX_T *p, U32 u4DramcShuOffset)
{
	//DDR800 do not enable tracking
	if (p->pDFSTable->shuffleIdx == SRAM_SHU6) //add if(u1ShuffleIdx==DRAM_DFS_SRAM_MAX) to avoid enable tx-tracking when running DDR800 as RG-SHU0
	{
		vIO32WriteFldAlign_All(DRAMC_REG_SHU_DQSOSC_SET0 + u4DramcShuOffset, 1, SHU_DQSOSC_SET0_DQSOSCENDIS);
	}
	else
	{
		vIO32WriteFldAlign_All(DRAMC_REG_SHU_DQSOSC_SET0 + u4DramcShuOffset, 0, SHU_DQSOSC_SET0_DQSOSCENDIS);
	}
}
#endif

#if RDSEL_TRACKING_EN
void Enable_RDSEL_Tracking(DRAMC_CTX_T *p, U32 u4DramcShuOffset)
{
	//Only enable at DDR3733
	if (p->pDFSTable->shuffleIdx == SRAM_SHU0) //add if(u1ShuffleIdx==DRAM_DFS_SRAM_MAX) to avoid enable tx-tracking when running DDR800 as RG-SHU0
	{
		vIO32WriteFldAlign_All(DDRPHY_REG_SHU_MISC_RDSEL_TRACK + u4DramcShuOffset, 0x0, SHU_MISC_RDSEL_TRACK_RDSEL_TRACK_EN);
		vIO32WriteFldMulti_All(DDRPHY_REG_SHU_MISC_RDSEL_TRACK + u4DramcShuOffset, P_Fld(0x1, SHU_MISC_RDSEL_TRACK_RDSEL_TRACK_EN)
																		 | P_Fld(0x0, SHU_MISC_RDSEL_TRACK_RDSEL_HWSAVE_MSK));
	}
	else
	{
		vIO32WriteFldAlign_All(DDRPHY_REG_SHU_MISC_RDSEL_TRACK + u4DramcShuOffset, 0x0, SHU_MISC_RDSEL_TRACK_RDSEL_TRACK_EN);
	}
}
#endif

#ifdef HW_GATING
void Enable_Gating_Tracking(DRAMC_CTX_T *p, U32 u4DDRPhyShuOffset)
{
	if (p->pDFSTable->shuffleIdx == SRAM_SHU6) {
		vIO32WriteFldMulti_All(DDRPHY_REG_MISC_SHU_STBCAL + u4DDRPhyShuOffset,
			P_Fld(0x0, MISC_SHU_STBCAL_STBCALEN) |
			P_Fld(0x0, MISC_SHU_STBCAL_STB_SELPHCALEN));
	} else {
		vIO32WriteFldMulti_All(DDRPHY_REG_MISC_SHU_STBCAL + u4DDRPhyShuOffset,
			P_Fld(0x1, MISC_SHU_STBCAL_STBCALEN) |
			P_Fld(0x1, MISC_SHU_STBCAL_STB_SELPHCALEN));
	}
}
#endif

void Enable_ClkTxRxLatchEn(DRAMC_CTX_T *p, U32 u4DDRPhyShuOffset)
{
	vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B0_DQ13 + u4DDRPhyShuOffset, 1, SHU_B0_DQ13_RG_TX_ARDQ_DLY_LAT_EN_B0);
	vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B1_DQ13 + u4DDRPhyShuOffset, 1, SHU_B1_DQ13_RG_TX_ARDQ_DLY_LAT_EN_B1);

	vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B0_DQ10 + u4DDRPhyShuOffset, 1, SHU_B0_DQ10_RG_RX_ARDQS_DLY_LAT_EN_B0);
	vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B1_DQ10 + u4DDRPhyShuOffset, 1, SHU_B1_DQ10_RG_RX_ARDQS_DLY_LAT_EN_B1);

	// Set 1 to be make TX DQS/DQ/DQM PI take effect when TX OE low, for new cross rank mode.
	vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B0_DQ2 + u4DDRPhyShuOffset, 1, SHU_B0_DQ2_RG_ARPI_OFFSET_LAT_EN_B0);
	vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B1_DQ2 + u4DDRPhyShuOffset, 1, SHU_B1_DQ2_RG_ARPI_OFFSET_LAT_EN_B1);

	// Default settings before init
	vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B0_DQ11 + u4DDRPhyShuOffset, 1, SHU_B0_DQ11_RG_RX_ARDQ_OFFSETC_LAT_EN_B0);
	vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B1_DQ11 + u4DDRPhyShuOffset, 1, SHU_B1_DQ11_RG_RX_ARDQ_OFFSETC_LAT_EN_B1);
	vIO32WriteFldAlign_All(DDRPHY_REG_SHU_CA_CMD11 + u4DDRPhyShuOffset, 1, SHU_CA_CMD11_RG_RX_ARCA_OFFSETC_LAT_EN_CA);

	// vReplaceDVInit
	vIO32WriteFldAlign_All(DDRPHY_REG_SHU_CA_CMD10 + u4DDRPhyShuOffset, 1, SHU_CA_CMD10_RG_RX_ARCLK_DLY_LAT_EN_CA);
}

#if ENABLE_TX_WDQS // @Darren, To avoid unexpected DQS toggle during calibration
void Enable_TxWDQS(DRAMC_CTX_T *p, U32 u4DDRPhyShuOffset, U16 u2Freq)
{
#if ENABLE_LP4Y_DFS
	U8 DQSB_READ_BASE		 = u2Freq<=800 ? 0 : 1; // for LP4Y
#else
	U8 DQSB_READ_BASE		 = 1;
#endif

	vIO32WriteFldMulti_All(DDRPHY_REG_SHU_B0_DQ13 + u4DDRPhyShuOffset	, P_Fld(1, SHU_B0_DQ13_RG_TX_ARDQS_READ_BASE_EN_B0			 )
																| P_Fld(DQSB_READ_BASE, SHU_B0_DQ13_RG_TX_ARDQSB_READ_BASE_EN_B0		  )
																| P_Fld(1, SHU_B0_DQ13_RG_TX_ARDQS_READ_BASE_DATA_TIE_EN_B0  )
																| P_Fld(DQSB_READ_BASE, SHU_B0_DQ13_RG_TX_ARDQSB_READ_BASE_DATA_TIE_EN_B0 ));
	vIO32WriteFldMulti_All(DDRPHY_REG_SHU_B1_DQ13 + u4DDRPhyShuOffset	, P_Fld(1, SHU_B1_DQ13_RG_TX_ARDQS_READ_BASE_EN_B1			 )
																| P_Fld(DQSB_READ_BASE, SHU_B1_DQ13_RG_TX_ARDQSB_READ_BASE_EN_B1		  )
																| P_Fld(1, SHU_B1_DQ13_RG_TX_ARDQS_READ_BASE_DATA_TIE_EN_B1  )
																| P_Fld(DQSB_READ_BASE, SHU_B1_DQ13_RG_TX_ARDQSB_READ_BASE_DATA_TIE_EN_B1 ));
}
#endif

#if ENABLE_SW_TX_TRACKING & __ETT__
void DramcSWTxTracking(DRAMC_CTX_T *p)
{
	U8 u1MR4OnOff;
	U8 rankIdx, rankBak;
	U8 u1SRAMShuLevel = get_shuffleIndex_by_Freq(p);
	U8 u1CurrentShuLevel;
	U16 u2MR1819_Base[RANK_MAX][2], u2MR1819_Runtime[RANK_MAX][2];
	U16 u2DQSOSC_INC[RANK_MAX] = {6}, u2DQSOSC_DEC[RANK_MAX] = {4};
	U8 u1AdjPI[RANK_MAX][2];
	U8 u1OriginalPI_DQ[DRAM_DFS_SHUFFLE_MAX][RANK_MAX][2];
	U8 u1UpdatedPI_DQ[DRAM_DFS_SHUFFLE_MAX][RANK_MAX][2];
	U8 u1OriginalPI_DQM[DRAM_DFS_SHUFFLE_MAX][RANK_MAX][2];
	U8 u1UpdatedPI_DQM[DRAM_DFS_SHUFFLE_MAX][RANK_MAX][2];
	U8 u1FreqRatioTX[DRAM_DFS_SHUFFLE_MAX];
	U8 shuIdx, shuBak, byteIdx;

	for (shuIdx = 0; shuIdx < DRAM_DFS_SHUFFLE_MAX; shuIdx++)
	{
		DRAM_DFS_FREQUENCY_TABLE_T *pDstFreqTbl = get_FreqTbl_by_shuffleIndex(p, shuIdx);
		if (pDstFreqTbl == NULL)
		{
			mcSHOW_ERR_MSG(("NULL pFreqTbl %d\n", shuIdx));
			while (1);
		}

		if (pDstFreqTbl->freq_sel == LP4_DDR800)
		{
			u1FreqRatioTX[shuIdx] = 0;
		}
		else
		{
			u1FreqRatioTX[shuIdx] = ((GetFreqBySel(p, pDstFreqTbl->freq_sel)) * 8) / p->frequency;
			mcSHOW_DBG_MSG(("[SWTxTracking] ShuLevel=%d, Ratio[%d]=%d (%d, %d)\n", u1SRAMShuLevel, shuIdx, u1FreqRatioTX[shuIdx], GetFreqBySel(p, pDstFreqTbl->freq_sel), p->frequency));
		}
	}

	mcSHOW_DBG_MSG(("[SWTxTracking] channel=%d\n", p->channel));
	rankBak = u1GetRank(p);
	shuBak = p->ShuRGAccessIdx;

	u1CurrentShuLevel = u4IO32ReadFldAlign(DDRPHY_REG_DVFS_STATUS, DVFS_STATUS_OTHER_SHU_GP);

	for (shuIdx = 0; shuIdx < DRAM_DFS_SHUFFLE_MAX; shuIdx++)
	{
		if (shuIdx == p->pDFSTable->shuffleIdx) {
			p->ShuRGAccessIdx = u1CurrentShuLevel;
		} else {
			LoadShuffleSRAMtoDramc(p, shuIdx, !u1CurrentShuLevel);
			p->ShuRGAccessIdx = !u1CurrentShuLevel;
		}

		for (rankIdx = RANK_0;rankIdx < p->support_rank_num;rankIdx++)
		{
			vSetRank(p, rankIdx);

			u1OriginalPI_DQ[shuIdx][p->rank][0] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_PI), SHURK_PI_RK0_ARPI_DQ_B0);
			u1OriginalPI_DQ[shuIdx][p->rank][1] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_PI), SHURK_PI_RK0_ARPI_DQ_B1);

			u1OriginalPI_DQM[shuIdx][p->rank][0] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_PI), SHURK_PI_RK0_ARPI_DQM_B0);
			u1OriginalPI_DQM[shuIdx][p->rank][1] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_PI), SHURK_PI_RK0_ARPI_DQM_B1);
		}
	}

	u1MR4OnOff = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_HMR4), HMR4_REFRDIS);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_HMR4), 1, HMR4_REFRDIS);

	p->ShuRGAccessIdx = u1CurrentShuLevel;

	for (rankIdx = RANK_0;rankIdx < p->support_rank_num;rankIdx++)
	{
		vSetRank(p, rankIdx);

		u2DQSOSC_INC[p->rank] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_DQSOSC_THRD), SHURK_DQSOSC_THRD_DQSOSCTHRD_INC);
		u2DQSOSC_DEC[p->rank] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_DQSOSC_THRD), SHURK_DQSOSC_THRD_DQSOSCTHRD_DEC);

		u2MR1819_Base[p->rank][0] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_DQSOSC), SHURK_DQSOSC_DQSOSC_BASE_RK0);
		u2MR1819_Base[p->rank][1] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_DQSOSC), SHURK_DQSOSC_DQSOSC_BASE_RK0_B1);

		DramcDQSOSCAuto(p);

		u2MR1819_Runtime[p->rank][0] = (gu2MR18[p->channel][p->rank] & 0x00FF) | ((gu2MR19[p->channel][p->rank] & 0x00FF) << 8);
		if (p->dram_cbt_mode[p->rank] == CBT_BYTE_MODE1)
		{
			u2MR1819_Runtime[p->rank][1] = (gu2MR18[p->channel][p->rank] >> 8) | ((gu2MR19[p->channel][p->rank] & 0xFF00));
		}
		else
		{
			u2MR1819_Runtime[p->rank][1] = u2MR1819_Runtime[p->rank][0];
		}

		//INC : MR1819>base. PI-
		//DEC : MR1819<base. PI+
		for (byteIdx = 0; byteIdx < 2; byteIdx++)
		{
			U16 deltaMR1819 = 0;

			if (u2MR1819_Runtime[p->rank][byteIdx] >= u2MR1819_Base[p->rank][byteIdx])
			{
				deltaMR1819 = u2MR1819_Runtime[p->rank][byteIdx] - u2MR1819_Base[p->rank][byteIdx];
				u1AdjPI[rankIdx][byteIdx] = deltaMR1819 / u2DQSOSC_INC[rankIdx];
				for (shuIdx = 0; shuIdx < DRAM_DFS_SHUFFLE_MAX; shuIdx++)
				{
					u1UpdatedPI_DQ[shuIdx][rankIdx][byteIdx] = u1OriginalPI_DQ[shuIdx][rankIdx][byteIdx] - (u1AdjPI[rankIdx][byteIdx] * u1FreqRatioTX[shuIdx] / u1FreqRatioTX[u1SRAMShuLevel]);
					u1UpdatedPI_DQM[shuIdx][rankIdx][byteIdx] = u1OriginalPI_DQM[shuIdx][rankIdx][byteIdx] - (u1AdjPI[rankIdx][byteIdx] * u1FreqRatioTX[shuIdx] / u1FreqRatioTX[u1SRAMShuLevel]);
					mcSHOW_DBG_MSG(("SHU%u CH%d RK%d B%d, Base=%X Runtime=%X delta=%d INC=%d PI=0x%B Adj=%d newPI=0x%B\n", shuIdx, p->channel, u1GetRank(p), byteIdx
								, u2MR1819_Base[p->rank][byteIdx], u2MR1819_Runtime[p->rank][byteIdx], deltaMR1819, u2DQSOSC_INC[rankIdx]
								, u1OriginalPI_DQ[shuIdx][rankIdx][byteIdx], (u1AdjPI[rankIdx][byteIdx] * u1FreqRatioTX[shuIdx] / u1FreqRatioTX[u1SRAMShuLevel]), u1UpdatedPI_DQ[shuIdx][rankIdx][byteIdx]));
				}
			}
			else
			{
				deltaMR1819 = u2MR1819_Base[p->rank][byteIdx] - u2MR1819_Runtime[p->rank][byteIdx];
				u1AdjPI[rankIdx][byteIdx] = deltaMR1819 / u2DQSOSC_DEC[rankIdx];
				for (shuIdx = 0; shuIdx < DRAM_DFS_SHUFFLE_MAX; shuIdx++)
				{
					u1UpdatedPI_DQ[shuIdx][rankIdx][byteIdx] = u1OriginalPI_DQ[shuIdx][rankIdx][byteIdx] + (u1AdjPI[rankIdx][byteIdx] * u1FreqRatioTX[shuIdx] / u1FreqRatioTX[u1SRAMShuLevel]);
					u1UpdatedPI_DQM[shuIdx][rankIdx][byteIdx] = u1OriginalPI_DQM[shuIdx][rankIdx][byteIdx] + (u1AdjPI[rankIdx][byteIdx] * u1FreqRatioTX[shuIdx] / u1FreqRatioTX[u1SRAMShuLevel]);
					mcSHOW_DBG_MSG(("SHU%u CH%d RK%d B%d, Base=%X Runtime=%X delta=%d DEC=%d PI=0x%B Adj=%d newPI=0x%B\n", shuIdx, p->channel, u1GetRank(p), byteIdx
								, u2MR1819_Base[p->rank][byteIdx], u2MR1819_Runtime[p->rank][byteIdx], deltaMR1819, u2DQSOSC_DEC[rankIdx]
								, u1OriginalPI_DQ[shuIdx][rankIdx][byteIdx], (u1AdjPI[rankIdx][byteIdx] * u1FreqRatioTX[shuIdx] / u1FreqRatioTX[u1SRAMShuLevel]), u1UpdatedPI_DQ[shuIdx][rankIdx][byteIdx]));
				}
			}
		}
	}

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DQSOSCR), 1, DQSOSCR_TXUPDMODE);

	p->ShuRGAccessIdx = !u1CurrentShuLevel;
	for (shuIdx = 0; shuIdx < DRAM_DFS_SHUFFLE_MAX; shuIdx++)
	{
		LoadShuffleSRAMtoDramc(p, shuIdx, !u1CurrentShuLevel);

		for (rankIdx = RANK_0;rankIdx < p->support_rank_num;rankIdx++)
		{
			vSetRank(p, rankIdx);

			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_DQ0),
					P_Fld(u1UpdatedPI_DQ[shuIdx][p->rank][0], SHU_R0_B0_DQ0_SW_ARPI_DQ_B0) |
					P_Fld(u1UpdatedPI_DQM[shuIdx][p->rank][0], SHU_R0_B0_DQ0_SW_ARPI_DQM_B0));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_DQ0),
					P_Fld(u1UpdatedPI_DQ[shuIdx][p->rank][1], SHU_R0_B1_DQ0_SW_ARPI_DQ_B1) |
					P_Fld(u1UpdatedPI_DQM[shuIdx][p->rank][1], SHU_R0_B1_DQ0_SW_ARPI_DQM_B1));
		}

		DramcSaveToShuffleSRAM(p, !u1CurrentShuLevel, shuIdx);
	}

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DQSOSCR), 1, DQSOSCR_MANUTXUPD);

	while ((u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TCMDO1LAT), TCMDO1LAT_MANUTXUPD_B0_DONE) != 1) && (u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TCMDO1LAT), TCMDO1LAT_MANUTXUPD_B1_DONE) != 1))
	{
		mcDELAY_US(1);
	}

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DQSOSCR), 0, DQSOSCR_TXUPDMODE);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DQSOSCR), 0, DQSOSCR_MANUTXUPD);

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_HMR4), u1MR4OnOff, HMR4_REFRDIS);

	vSetRank(p, rankBak);
	p->ShuRGAccessIdx = shuBak;
}
#endif


#if ENABLE_RX_TRACKING
void DramcRxInputDelayTrackingInit_Common(DRAMC_CTX_T *p)
{
	U8 ii, backup_rank;
	U32 u4WbrBackup = GetDramcBroadcast();

	backup_rank = u1GetRank(p);
	DramcBroadcastOnOff(DRAMC_BROADCAST_ON);

	//Enable RX_FIFO macro DIV4 clock CG
	vIO32WriteFldAlign((DDRPHY_REG_MISC_CG_CTRL1), 0xffffffff, MISC_CG_CTRL1_R_DVS_DIV4_CG_CTRL);

	for (ii = RANK_0; ii < p->support_rank_num; ii++)
	{
		vSetRank(p, ii);

		//DVS mode to RG mode
		vIO32WriteFldAlign((DDRPHY_REG_RK_B0_RXDVS2), 0x0, RK_B0_RXDVS2_R_RK0_DVS_MODE_B0);
		vIO32WriteFldAlign((DDRPHY_REG_RK_B1_RXDVS2), 0x0, RK_B1_RXDVS2_R_RK0_DVS_MODE_B1);

		//Turn off F_DLY individual calibration option (CTO_AGENT_RDAT cannot separate DR/DF error)
		//tracking rising and update rising/falling together
		vIO32WriteFldAlign((DDRPHY_REG_RK_B0_RXDVS2), 0x1, RK_B0_RXDVS2_R_RK0_DVS_FDLY_MODE_B0);
		vIO32WriteFldAlign((DDRPHY_REG_RK_B1_RXDVS2), 0x1, RK_B1_RXDVS2_R_RK0_DVS_FDLY_MODE_B1);

		//DQ/DQM/DQS DLY MAX/MIN value under Tracking mode
		/* DQS, DQ, DQM (DQ, DQM are tied together now) -> controlled using DQM MAX_MIN */

		/* Byte 0 */
		vIO32WriteFldMulti((DDRPHY_REG_RK_B0_RXDVS3), P_Fld(0x0, RK_B0_RXDVS3_RG_RK0_ARDQ_MIN_DLY_B0) | P_Fld(0xff, RK_B0_RXDVS3_RG_RK0_ARDQ_MAX_DLY_B0));
		vIO32WriteFldMulti((DDRPHY_REG_RK_B0_RXDVS4), P_Fld(0x0, RK_B0_RXDVS4_RG_RK0_ARDQS0_MIN_DLY_B0) | P_Fld(0x1ff, RK_B0_RXDVS4_RG_RK0_ARDQS0_MAX_DLY_B0));

		/* Byte 1 */
		vIO32WriteFldMulti((DDRPHY_REG_RK_B1_RXDVS3), P_Fld(0x0, RK_B1_RXDVS3_RG_RK0_ARDQ_MIN_DLY_B1) | P_Fld(0xff, RK_B1_RXDVS3_RG_RK0_ARDQ_MAX_DLY_B1));
		vIO32WriteFldMulti((DDRPHY_REG_RK_B1_RXDVS4), P_Fld(0x0, RK_B1_RXDVS4_RG_RK0_ARDQS0_MIN_DLY_B1) | P_Fld(0x1ff, RK_B1_RXDVS4_RG_RK0_ARDQS0_MAX_DLY_B1));

		//Threshold for LEAD/LAG filter
		vIO32WriteFldMulti((DDRPHY_REG_RK_B0_RXDVS1), P_Fld(0x0, RK_B0_RXDVS1_R_RK0_B0_DVS_TH_LEAD) | P_Fld(0x0, RK_B0_RXDVS1_R_RK0_B0_DVS_TH_LAG));
		vIO32WriteFldMulti((DDRPHY_REG_RK_B1_RXDVS1), P_Fld(0x0, RK_B1_RXDVS1_R_RK0_B1_DVS_TH_LEAD) | P_Fld(0x0, RK_B1_RXDVS1_R_RK0_B1_DVS_TH_LAG));

		//DQ/DQS Rx DLY adjustment for tracking mode
		vIO32WriteFldMulti((DDRPHY_REG_RK_B0_RXDVS2), P_Fld(0x1, RK_B0_RXDVS2_R_RK0_RX_DLY_RIS_DQ_SCALE_B0) | P_Fld(0x1, RK_B0_RXDVS2_R_RK0_RX_DLY_RIS_DQS_SCALE_B0));
		vIO32WriteFldMulti((DDRPHY_REG_RK_B1_RXDVS2), P_Fld(0x1, RK_B1_RXDVS2_R_RK0_RX_DLY_RIS_DQ_SCALE_B1) | P_Fld(0x1, RK_B1_RXDVS2_R_RK0_RX_DLY_RIS_DQS_SCALE_B1));

		vIO32WriteFldMulti((DDRPHY_REG_RK_B0_RXDVS2), P_Fld(0x3, RK_B0_RXDVS2_R_RK0_RX_DLY_FAL_DQ_SCALE_B0) | P_Fld(0x3, RK_B0_RXDVS2_R_RK0_RX_DLY_FAL_DQS_SCALE_B0));
		vIO32WriteFldMulti((DDRPHY_REG_RK_B1_RXDVS2), P_Fld(0x3, RK_B1_RXDVS2_R_RK0_RX_DLY_FAL_DQ_SCALE_B1) | P_Fld(0x3, RK_B1_RXDVS2_R_RK0_RX_DLY_FAL_DQS_SCALE_B1));

	}
	vSetRank(p, backup_rank);

	//Tracking lead/lag counter >> Rx DLY adjustment fixed to 1
	vIO32WriteFldAlign((DDRPHY_REG_B0_RXDVS0), 0x0, B0_RXDVS0_R_DMRXDVS_CNTCMP_OPT_B0);
	vIO32WriteFldAlign((DDRPHY_REG_B1_RXDVS0), 0x0, B1_RXDVS0_R_DMRXDVS_CNTCMP_OPT_B1);

	//DQIEN pre-state option to block update for RX ASVA  1-2
	vIO32WriteFldAlign((DDRPHY_REG_B0_RXDVS0), 0x1, B0_RXDVS0_R_DMRXDVS_DQIENPRE_OPT_B0);
	vIO32WriteFldAlign((DDRPHY_REG_B1_RXDVS0), 0x1, B1_RXDVS0_R_DMRXDVS_DQIENPRE_OPT_B1);

	DramcBroadcastOnOff(u4WbrBackup);
}
#endif

void DramcRxInputDelayTrackingInit_byFreq(DRAMC_CTX_T *p)
{
	U8 u1DVS_Delay;
	U8 u1DVS_En=1;
	U32 u4WbrBackup = GetDramcBroadcast();
	DramcBroadcastOnOff(DRAMC_BROADCAST_ON);

	//Monitor window size setting
	//DDRPHY.SHU*_B*_DQ5.RG_RX_ARDQS0_DVS_DLY_B* (suggested value from A-PHY owner)
//WHITNEY_TO_BE_PORTING
#if (fcFOR_CHIP_ID == fcMargaux)
	//				6400	5500	4266	3733	3200	2400	1600	1200	800
	//UI			156p	181p	234p	268p	312p	417p	625p	833p	1250p
	//DVS_EN		O		O		O		O		O		O		X		X		X
	//INI			1		2		3		N=5 	N=5 	N=7 	N=12	N=15	N=15
	//DVS delay 	O		O		O		X		X		X		X		X		X
	//calibration

	if(p->frequency >= 3200)
	{
		u1DVS_Delay =1;
	}
	else if(p->frequency >= 2250)
	{
		u1DVS_Delay = 2;
	}
	else if(p->frequency >= 2133)
	{
		u1DVS_Delay = 3;
	}
	else if(p->frequency >= 1600)
	{
		u1DVS_Delay =5;
	}
	else if(p->frequency >= 1200)
	{
		u1DVS_Delay =7;
	}
	else if(p->frequency >= 800)
	{
		u1DVS_Delay =12;
		u1DVS_En =0;
	}
	else
	{
		u1DVS_Delay =15;
		u1DVS_En =0;
	}
#endif

	vIO32WriteFldAlign((DDRPHY_REG_SHU_B0_DQ5), u1DVS_Delay, SHU_B0_DQ5_RG_RX_ARDQS0_DVS_DLY_B0);
	vIO32WriteFldAlign((DDRPHY_REG_SHU_B1_DQ5), u1DVS_Delay, SHU_B1_DQ5_RG_RX_ARDQS0_DVS_DLY_B1);

	/* Bian_co HW design issue: run-time PBYTE flag will lose it's function and become per-bit -> set to 0 */
	vIO32WriteFldMulti((DDRPHY_REG_SHU_B0_DQ7), P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B0)
														| P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B0));
	vIO32WriteFldMulti((DDRPHY_REG_SHU_B1_DQ7), P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B1)
														| P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B1));

	//Enable A-PHY DVS LEAD/LAG
	vIO32WriteFldAlign((DDRPHY_REG_SHU_B0_DQ11), u1DVS_En, SHU_B0_DQ11_RG_RX_ARDQ_DVS_EN_B0);
	vIO32WriteFldAlign((DDRPHY_REG_SHU_B1_DQ11), u1DVS_En, SHU_B1_DQ11_RG_RX_ARDQ_DVS_EN_B1);

	DramcBroadcastOnOff(u4WbrBackup);
}

#if ENABLE_RX_TRACKING
void DramcRxInputDelayTrackingHW(DRAMC_CTX_T *p)
{
	DRAM_CHANNEL_T channel_bak = p->channel;
	U8 ii, backup_rank;
	U32 u4WbrBackup = GetDramcBroadcast();
	DramcBroadcastOnOff(DRAMC_BROADCAST_ON);

	vSetPHY2ChannelMapping(p, CHANNEL_A);
	backup_rank = u1GetRank(p);

	//Rx DLY tracking setting (Static)
	vIO32WriteFldMulti((DDRPHY_REG_B0_RXDVS0),
										   P_Fld(1, B0_RXDVS0_R_RX_DLY_TRACK_SPM_CTRL_B0) |
										   P_Fld(0, B0_RXDVS0_R_RX_RANKINCTL_B0) |
										   P_Fld(1, B0_RXDVS0_R_RX_RANKINSEL_B0));

	vIO32WriteFldMulti((DDRPHY_REG_B1_RXDVS0),
										   P_Fld(1, B1_RXDVS0_R_RX_DLY_TRACK_SPM_CTRL_B1) |
										   P_Fld(0, B1_RXDVS0_R_RX_RANKINCTL_B1) |
										   P_Fld(1, B1_RXDVS0_R_RX_RANKINSEL_B1));

	vIO32WriteFldMulti((DDRPHY_REG_B0_DQ9), P_Fld(0x1, B0_DQ9_R_DMRXDVS_RDSEL_LAT_B0 | P_Fld(0, B0_DQ9_R_DMRXDVS_VALID_LAT_B0)));
	vIO32WriteFldMulti((DDRPHY_REG_B1_DQ9), P_Fld(0x1, B1_DQ9_R_DMRXDVS_RDSEL_LAT_B1) | P_Fld(0, B1_DQ9_R_DMRXDVS_VALID_LAT_B1));
	vIO32WriteFldMulti((DDRPHY_REG_CA_CMD9), P_Fld(0, CA_CMD9_R_DMRXDVS_RDSEL_LAT_CA) | P_Fld(0, CA_CMD9_R_DMRXDVS_VALID_LAT_CA));

	//Rx DLY tracking function CG enable
	vIO32WriteFldAlign((DDRPHY_REG_B0_RXDVS0), 0x1, B0_RXDVS0_R_RX_DLY_TRACK_CG_EN_B0);
	vIO32WriteFldAlign((DDRPHY_REG_B1_RXDVS0), 0x1, B1_RXDVS0_R_RX_DLY_TRACK_CG_EN_B1);

	//Rx DLY tracking lead/lag counter enable
	vIO32WriteFldAlign((DDRPHY_REG_B0_RXDVS0), 0x1, B0_RXDVS0_R_RX_DLY_TRACK_ENA_B0);
	vIO32WriteFldAlign((DDRPHY_REG_B1_RXDVS0), 0x1, B1_RXDVS0_R_RX_DLY_TRACK_ENA_B1);

	//fra 0: origin mode (use LEAD/LAG rising tracking) 	1: new mode (use LEAD/LAG rising/fall tracking, more faster)
	vIO32WriteFldAlign((DDRPHY_REG_B0_RXDVS1), 1, B0_RXDVS1_F_LEADLAG_TRACK_B0);
	vIO32WriteFldAlign((DDRPHY_REG_B1_RXDVS1), 1, B1_RXDVS1_F_LEADLAG_TRACK_B1);

	for (ii = RANK_0; ii < RANK_MAX; ii++)
	{
		vSetRank(p, ii);

		//Rx DLY tracking update enable (HW mode)
		vIO32WriteFldMulti((DDRPHY_REG_RK_B0_RXDVS2),
											   P_Fld(2, RK_B0_RXDVS2_R_RK0_DVS_MODE_B0) |
											   P_Fld(1, RK_B0_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B0) |
											   P_Fld(1, RK_B0_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B0));

		vIO32WriteFldMulti((DDRPHY_REG_RK_B1_RXDVS2),
											   P_Fld(2, RK_B1_RXDVS2_R_RK0_DVS_MODE_B1) |
											   P_Fld(1, RK_B1_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B1) |
											   P_Fld(1, RK_B1_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B1));
	}
	vSetRank(p, backup_rank);

	//Rx DLY tracking: "TRACK_CLR" let rx_dly reload the correct dly setting
	vIO32WriteFldAlign((DDRPHY_REG_B0_RXDVS0), 1, B0_RXDVS0_R_RX_DLY_TRACK_CLR_B0);
	vIO32WriteFldAlign((DDRPHY_REG_B1_RXDVS0), 1, B1_RXDVS0_R_RX_DLY_TRACK_CLR_B1);
	vIO32WriteFldAlign((DDRPHY_REG_B0_RXDVS0), 0, B0_RXDVS0_R_RX_DLY_TRACK_CLR_B0);
	vIO32WriteFldAlign((DDRPHY_REG_B1_RXDVS0), 0, B1_RXDVS0_R_RX_DLY_TRACK_CLR_B1);

	DramcBroadcastOnOff(u4WbrBackup);
}
#endif

///TODO: wait for porting +++
#if __A60868_TO_BE_PORTING__
#if RX_DLY_TRACK_ONLY_FOR_DEBUG
void DramcRxDlyTrackDebug(DRAMC_CTX_T *p)
{
	/* indicate ROW_ADR = 2 for dummy write & read for Rx dly track debug feature, avoid pattern overwrite by MEM_TEST
	 * pattern(0xAAAA5555) locates: 0x40010000, 0x40010100, 0x80010000, 0x80010100 */

	vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DRAMC_REG_RK0_DUMMY_RD_ADR), P_Fld(2, RK0_DUMMY_RD_ADR_DMY_RD_RK0_ROW_ADR)
						| P_Fld(0, RK0_DUMMY_RD_ADR_DMY_RD_RK0_COL_ADR)
						| P_Fld(0, RK0_DUMMY_RD_ADR_DMY_RD_RK0_LEN));
	vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DRAMC_REG_RK1_DUMMY_RD_ADR), P_Fld(2, RK1_DUMMY_RD_ADR_DMY_RD_RK1_ROW_ADR)
						| P_Fld(0, RK1_DUMMY_RD_ADR_DMY_RD_RK1_COL_ADR)
						| P_Fld(0, RK1_DUMMY_RD_ADR_DMY_RD_RK1_LEN));
	vIO32WriteFldAlign_All(DRAMC_REG_RK0_DUMMY_RD_BK, 0, RK0_DUMMY_RD_BK_DMY_RD_RK0_BK);
	vIO32WriteFldAlign_All(DRAMC_REG_RK1_DUMMY_RD_BK, 0, RK1_DUMMY_RD_BK_DMY_RD_RK1_BK);

	vIO32Write4B_All(DRAMC_REG_RK0_DUMMY_RD_WDATA0, 0xAAAA5555);
	vIO32Write4B_All(DRAMC_REG_RK0_DUMMY_RD_WDATA1, 0xAAAA5555);
	vIO32Write4B_All(DRAMC_REG_RK0_DUMMY_RD_WDATA2, 0xAAAA5555);
	vIO32Write4B_All(DRAMC_REG_RK0_DUMMY_RD_WDATA3, 0xAAAA5555);
	vIO32Write4B_All(DRAMC_REG_RK1_DUMMY_RD_WDATA0, 0xAAAA5555);
	vIO32Write4B_All(DRAMC_REG_RK1_DUMMY_RD_WDATA1, 0xAAAA5555);
	vIO32Write4B_All(DRAMC_REG_RK1_DUMMY_RD_WDATA2, 0xAAAA5555);
	vIO32Write4B_All(DRAMC_REG_RK1_DUMMY_RD_WDATA3, 0xAAAA5555);

	//disable Rx dly track debug and clear status lock
	vIO32WriteFldMulti_All((DDRPHY_MISC_RXDVS2), P_Fld(0, MISC_RXDVS2_R_DMRXDVS_DBG_MON_EN)
						| P_Fld(1, MISC_RXDVS2_R_DMRXDVS_DBG_MON_CLR)
						| P_Fld(0, MISC_RXDVS2_R_DMRXDVS_DBG_PAUSE_EN));

	//trigger dummy write pattern 0xAAAA5555
	vIO32WriteFldAlign_All(DRAMC_REG_DUMMY_RD, 0x1, DUMMY_RD_DMY_WR_DBG);
	vIO32WriteFldAlign_All(DRAMC_REG_DUMMY_RD, 0x0, DUMMY_RD_DMY_WR_DBG);

	// enable Rx dly track debug feature
	vIO32WriteFldMulti_All((DDRPHY_MISC_RXDVS2), P_Fld(1, MISC_RXDVS2_R_DMRXDVS_DBG_MON_EN)
						| P_Fld(0, MISC_RXDVS2_R_DMRXDVS_DBG_MON_CLR)
						| P_Fld(1, MISC_RXDVS2_R_DMRXDVS_DBG_PAUSE_EN));
}

void DramcPrintRxDlyTrackDebugStatus(DRAMC_CTX_T *p)
{
	U32 backup_rank, u1ChannelBak, u4value;
	U8 u1ChannelIdx, u1ChannelMax = p->support_channel_num;//channel A/B ...

	u1ChannelBak = p->channel;
	backup_rank = u1GetRank(p);

	for (u1ChannelIdx = CHANNEL_A; u1ChannelIdx < p->support_channel_num; u1ChannelIdx++)
	{
		p->channel = u1ChannelIdx;

		u4value = u4IO32Read4B(DRAMC_REG_ADDR(DDRPHY_MISC_DQ_RXDLY_TRRO18));
		//mcSHOW_DBG_MSG(("\nCH_%d DQ_RXDLY_TRRO18 = 0x\033[1;36m%x\033[m\n",u1ChannelIdx,u4value));
		if (u4value & 1)
		{
			mcSHOW_DBG_MSG(("=== CH_%d DQ_RXDLY_TRRO18 = 0x\033[1;36m%x\033[m, %s %s shu: %d\n", u1ChannelIdx, u4value,
						u4value & 0x2? "RK0: fail":"",u4value&0x4?"RK1: fail":"", (u4value >> 4) & 0x3));
		}
	}
}
#endif

#if (FOR_DV_SIMULATION_USED == 0 && SW_CHANGE_FOR_SIMULATION == 0)
#if (__ETT__ || CPU_RW_TEST_AFTER_K)
void DramcPrintRXDQDQSStatus(DRAMC_CTX_T *p, U8 u1Channel)
{
	U8 u1RankIdx, u1RankMax, u1ChannelBak, u1ByteIdx, ii;
	U32 u4ResultDQS_PI, u4ResultDQS_UI, u4ResultDQS_UI_P1;
	U8 u1Dqs_pi[DQS_BIT_NUMBER], u1Dqs_ui[DQS_BIT_NUMBER], u1Dqs_ui_P1[DQS_BIT_NUMBER];
	U16 u2TmpValue, u2TmpUI[DQS_NUMBER], u2TmpPI[DQS_NUMBER];
	U32 MANUDLLFRZ_bak, STBSTATE_OPT_bak;
	U32 backup_rank;
	U8 u1DQX_B0, u1DQS0, u1DQX_B1, u1DQS1;

	u1ChannelBak = p->channel;
	vSetPHY2ChannelMapping(p, u1Channel);
	backup_rank = u1GetRank(p);

	if (p->support_rank_num == RANK_DUAL)
		u1RankMax = RANK_MAX;
	else
		 u1RankMax = RANK_1;

	for (u1RankIdx = 0; u1RankIdx < u1RankMax; u1RankIdx++)
	{
		vSetRank(p, u1RankIdx);
		mcSHOW_DBG_MSG(("[RXDQDQSStatus] CH%d, RK%d\n", p->channel, u1RankIdx));
		if (u1RankIdx == 0)
		u4ResultDQS_PI = u4IO32Read4B(DRAMC_REG_ADDR(DDRPHY_MISC_DQ_RXDLY_TRRO22));
		if (u1RankIdx == 1)
		u4ResultDQS_PI = u4IO32Read4B(DRAMC_REG_ADDR(DDRPHY_MISC_DQ_RXDLY_TRRO23));

		u1DQX_B0 = u4ResultDQS_PI & 0xff;
		u1DQS0 = (u4ResultDQS_PI >> 8) & 0xff;
		u1DQX_B1 = (u4ResultDQS_PI >> 16) & 0xff;
		u1DQS1 = (u4ResultDQS_PI >> 24) & 0xff;

		mcSHOW_DBG_MSG(("DQX_B0, DQS0, DQX_B1, DQS1 =(%d, %d, %d, %d)\n\n", u1DQX_B0, u1DQS0, u1DQX_B1, u1DQS1));

		}
	vSetRank(p, backup_rank);

	p->channel = u1ChannelBak;
	vSetPHY2ChannelMapping(p, u1ChannelBak);
}
#endif
#endif

void DummyReadForDqsGatingRetryShuffle(DRAMC_CTX_T *p, bool bEn)
{
	if (bEn == 1)
	{
		vIO32WriteFldMulti_All(DDRPHY_REG_MISC_SHU_DQSG_RETRY1, P_Fld(0, MISC_SHU_DQSG_RETRY1_RETRY_ROUND_NUM)//Retry once
									 | P_Fld(1, MISC_SHU_DQSG_RETRY1_XSR_RETRY_SPM_MODE)
									 | P_Fld(0, MISC_SHU_DQSG_RETRY1_XSR_DQSG_RETRY_EN)
									 | P_Fld(0, MISC_SHU_DQSG_RETRY1_RETRY_SW_EN)
									 | P_Fld(1, MISC_SHU_DQSG_RETRY1_RETRY_USE_BURST_MODE)
									 | P_Fld(1, MISC_SHU_DQSG_RETRY1_RETRY_RDY_SEL_DLE)
									 | P_Fld(1, MISC_SHU_DQSG_RETRY1_RETRY_DDR1866_PLUS));
	}
	else
	{
		vIO32WriteFldMulti_All(DDRPHY_REG_MISC_SHU_DQSG_RETRY1, P_Fld(0, MISC_SHU_DQSG_RETRY1_XSR_RETRY_SPM_MODE)
									 | P_Fld(0, MISC_SHU_DQSG_RETRY1_XSR_DQSG_RETRY_EN)
									 | P_Fld(0, MISC_SHU_DQSG_RETRY1_RETRY_SW_EN));
	}
	return;
}

void DummyReadForDqsGatingRetryNonShuffle(DRAMC_CTX_T *p, bool bEn)
{
	U8 backup_rank = p->rank;
	U8 rankIdx;

	if (bEn == 1)
	{
		vIO32WriteFldAlign_All(DRAMC_REG_TEST2_A4, 4, TEST2_A4_TESTAGENTRKSEL);//Dummy Read rank selection is controlled by Test Agent
		vIO32WriteFldMulti_All(DRAMC_REG_DUMMY_RD, P_Fld(1, DUMMY_RD_DQSG_DMYRD_EN)
									 | P_Fld(p->support_rank_num, DUMMY_RD_RANK_NUM)
									 | P_Fld(1, DUMMY_RD_DUMMY_RD_SW));
		for (rankIdx = RANK_0; rankIdx < p->support_rank_num; rankIdx++)
		{
			vSetRank(p, rankIdx);
			vIO32WriteFldAlign_All(DRAMC_REG_RK_DUMMY_RD_ADR, 0, RK_DUMMY_RD_ADR_DMY_RD_LEN);
		}
		vSetRank(p, backup_rank);
	}
	else
	{
	}
	return;
}

#endif // __A60868_TO_BE_PORTING__

#ifdef DUMMY_READ_FOR_TRACKING
void DramcDummyReadAddressSetting(DRAMC_CTX_T *p)
{
	U8 backup_channel = p->channel, backup_rank = p->rank;
	U8 channelIdx, rankIdx;
	dram_addr_t dram_addr;

	for (channelIdx = CHANNEL_A; channelIdx < CHANNEL_NUM; channelIdx++)
	{
		vSetPHY2ChannelMapping(p, channelIdx);
		for (rankIdx = RANK_0; rankIdx < RANK_MAX; rankIdx++)
		{
			vSetRank(p, rankIdx);

			dram_addr.ch = channelIdx;
			dram_addr.rk = rankIdx;

			get_dummy_read_addr(&dram_addr);
			mcSHOW_DBG_MSG3(("=== dummy read address: CH_%d, RK%d, row: 0x%x, bk: %d, col: 0x%x\n\n",
					channelIdx, rankIdx, dram_addr.row, dram_addr.bk, dram_addr.col));

			vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_RK_DUMMY_RD_ADR2), P_Fld(dram_addr.row, RK_DUMMY_RD_ADR2_DMY_RD_ROW_ADR)
																		| P_Fld(dram_addr.bk, RK_DUMMY_RD_ADR2_DMY_RD_BK));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_RK_DUMMY_RD_ADR), P_Fld(dram_addr.col, RK_DUMMY_RD_ADR_DMY_RD_COL_ADR)
																		| P_Fld(0, RK_DUMMY_RD_ADR_DMY_RD_LEN));
		}
	}

	vSetPHY2ChannelMapping(p, backup_channel);
	vSetRank(p, backup_rank);

}

void DramcDummyReadForTrackingEnable(DRAMC_CTX_T *p)
{
	U8 backup_rank = p->rank;
	U8 rankIdx;

	/* Dummy read pattern (Better efficiency during rx dly tracking) DE: YH Tsai, Wei-jen */
	for (rankIdx = RANK_0; rankIdx < p->support_rank_num; rankIdx++)
	{
		vSetRank(p, rankIdx);

		vIO32Write4B_All(DRAMC_REG_RK_DUMMY_RD_WDATA0, 0xAAAA5555); // Field RK0_DUMMY_RD_WDATA0_DMY_RD_RK0_WDATA0
		vIO32Write4B_All(DRAMC_REG_RK_DUMMY_RD_WDATA1, 0xAAAA5555); // Field RK0_DUMMY_RD_WDATA1_DMY_RD_RK0_WDATA1
		vIO32Write4B_All(DRAMC_REG_RK_DUMMY_RD_WDATA2, 0xAAAA5555); // Field RK0_DUMMY_RD_WDATA2_DMY_RD_RK0_WDATA2
		vIO32Write4B_All(DRAMC_REG_RK_DUMMY_RD_WDATA3, 0xAAAA5555); // Field RK0_DUMMY_RD_WDATA3_DMY_RD_RK0_WDATA3
	}
	vSetRank(p, backup_rank);

	vIO32WriteFldAlign_All(DRAMC_REG_TEST2_A4, 4, TEST2_A4_TESTAGENTRKSEL);//Dummy Read rank selection is controlled by Test Agent

#if 0//__ETT__
	/* indicate ROW_ADR = 2 for Dummy Write pattern address, in order to avoid pattern will be overwrited by MEM_TEST(test range 0xffff)
	 * Pattern locates: 0x40010000, 0x40010100, 0x80010000, 0x80010100 */
	dram_addr_t dram_addr;

	dram_addr.ch = 0;
	dram_addr.rk = 0;
	get_dummy_read_addr(&dram_addr);

	vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DRAMC_REG_RK0_DUMMY_RD_ADR), P_Fld(dram_addr.row, RK0_DUMMY_RD_ADR_DMY_RD_RK0_ROW_ADR)
						| P_Fld(dram_addr.col, RK0_DUMMY_RD_ADR_DMY_RD_RK0_COL_ADR)
						| P_Fld(0, RK0_DUMMY_RD_ADR_DMY_RD_RK0_LEN));
	vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DRAMC_REG_RK0_DUMMY_RD_BK), dram_addr.bk, RK0_DUMMY_RD_BK_DMY_RD_RK0_BK);

	dram_addr.rk = 1;
	get_dummy_read_addr(&dram_addr);

	vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DRAMC_REG_RK1_DUMMY_RD_ADR), P_Fld(dram_addr.row, RK1_DUMMY_RD_ADR_DMY_RD_RK1_ROW_ADR)
						| P_Fld(dram_addr.col, RK1_DUMMY_RD_ADR_DMY_RD_RK1_COL_ADR)
						| P_Fld(0, RK1_DUMMY_RD_ADR_DMY_RD_RK1_LEN));
	vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DRAMC_REG_RK1_DUMMY_RD_BK), dram_addr.bk, RK1_DUMMY_RD_BK_DMY_RD_RK1_BK);

	/* trigger dummy write pattern 0xAAAA5555 */
	vIO32WriteFldAlign_All(DRAMC_REG_DUMMY_RD, 0x1, DUMMY_RD_DMY_WR_DBG);
	vIO32WriteFldAlign_All(DRAMC_REG_DUMMY_RD, 0x0, DUMMY_RD_DMY_WR_DBG);
#else
	DramcDummyReadAddressSetting(p);
#endif

	/* DUMMY_RD_RX_TRACK = 1:
	 * During "RX input delay tracking enable" and "DUMMY_RD_EN=1" Dummy read will force a read command to a certain rank,
	 * ignoring whether or not EMI has executed a read command to that certain rank in the past 4us.
	 */

	if (p->frequency >= 1600)
	{
		vIO32WriteFldMulti_All(DRAMC_REG_DUMMY_RD, P_Fld(1, DUMMY_RD_DMY_RD_RX_TRACK) | P_Fld(1, DUMMY_RD_DUMMY_RD_EN));
		mcSHOW_DBG_MSG(("High Freq DUMMY_READ_FOR_TRACKING: ON\n"));
	}
	else
	{
		mcSHOW_DBG_MSG(("Low Freq DUMMY_READ_FOR_TRACKING: OFF\n"));
	}

	return;
}
#endif

#ifdef IMPEDANCE_HW_SAVING
void DramcImpedanceHWSaving(DRAMC_CTX_T *p)
{
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_IMPCAL, 1, MISC_IMPCAL_IMPCAL_HWSAVE_EN);
}
#endif

#ifdef IMPEDANCE_TRACKING_ENABLE
void DramcImpedanceTrackingEnable(DRAMC_CTX_T *p)
{
	U8 u1CHAB_en = DISABLE;
	#if 0  //Impedance tracking offset for DRVP+2
	vIO32WriteFldMulti_All(DRAMC_REG_IMPEDAMCE_CTRL1, P_Fld(2, IMPEDAMCE_CTRL1_DQS1_OFF) | P_Fld(2, IMPEDAMCE_CTRL1_DOS2_OFF));
	vIO32WriteFldMulti_All(DRAMC_REG_IMPEDAMCE_CTRL2, P_Fld(2, IMPEDAMCE_CTRL2_DQ1_OFF) | P_Fld(2, IMPEDAMCE_CTRL2_DQ2_OFF));
	#endif

	//Write (DRAMC _BASE+ 0x8B) [31:0] = 32'he4000000//enable impedance tracking
	//u1CHAB_en = (p->support_channel_num == CHANNEL_DUAL) ? ENABLE : DISABLE;// @tg Only CHA do Impcal tracking for Margaux
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_CTRL0, u1CHAB_en, MISC_CTRL0_IMPCAL_CHAB_EN);//Set CHA this bit to enable dual channel tracking

	//During shuffle, after CH_A IMP update done, CH_B has no enough time to update (IMPCAL_IMPCAL_DRVUPDOPT=1)
	//enable ECO function for impedance load last tracking result of previous shuffle level (IMPCAL_IMPCAL_CHGDRV_ECO_OPT=1)
	//enable ECO function for impcal_sm hange when DRVP>=0x1D (IMPCAL_IMPCAL_SM_ECO_OPT=1)
	vIO32WriteFldMulti_All(DDRPHY_REG_MISC_IMPCAL, P_Fld(1, MISC_IMPCAL_IMPCAL_HW) | P_Fld(0, MISC_IMPCAL_IMPCAL_EN) |
											 P_Fld(1, MISC_IMPCAL_IMPCAL_SWVALUE_EN) | P_Fld(1, MISC_IMPCAL_IMPCAL_NEW_OLD_SL) |
											 P_Fld(1, MISC_IMPCAL_IMPCAL_DRVUPDOPT) | P_Fld(1, MISC_IMPCAL_IMPCAL_CHGDRV_ECO_OPT) |
											 P_Fld(1, MISC_IMPCAL_IMPCAL_SM_ECO_OPT) | P_Fld(1, MISC_IMPCAL_IMPBINARY) |
											 P_Fld(1, MISC_IMPCAL_DRV_ECO_OPT));

	//dual channel continuously tracking @ system busy, self-refresh, Hhbrid-S1
	//vIO32WriteFldAlign_All(DDRPHY_REG_MISC_CTRL0, 0x1, MISC_CTRL0_IMPCAL_LP_ECO_OPT);
	//@tg.Only CHA do Impcal tracking, CHB sync CHA result value
	vIO32WriteFldMulti(DDRPHY_REG_MISC_CTRL0, P_Fld(0x1, MISC_CTRL0_IMPCAL_LP_ECO_OPT) |
										 P_Fld(0x0, MISC_CTRL0_IMPCAL_TRACK_DISABLE));
	vIO32WriteFldMulti(DDRPHY_REG_MISC_CTRL0 + SHIFT_TO_CHB_ADDR, P_Fld(0x1, MISC_CTRL0_IMPCAL_LP_ECO_OPT) |
										 P_Fld(0x1, MISC_CTRL0_IMPCAL_TRACK_DISABLE));

	// no update imp CA, because CA is unterm now
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_IMPCAL, 1, MISC_IMPCAL_IMPCAL_BYPASS_UP_CA_DRV);

	// CH_A set 1, CH_B set 0 (mp setting)
	vIO32WriteFldMulti(DDRPHY_REG_MISC_IMPCAL, P_Fld(0, MISC_IMPCAL_DIS_SUS_CH0_DRV) |
										 P_Fld(1, MISC_IMPCAL_DIS_SUS_CH1_DRV) |
										 P_Fld(0, MISC_IMPCAL_IMPSRCEXT) |		//Update mp setting
										 P_Fld(1, MISC_IMPCAL_IMPCAL_ECO_OPT)); //Update mp setting
	vIO32WriteFldMulti(DDRPHY_REG_MISC_IMPCAL + SHIFT_TO_CHB_ADDR, P_Fld(1, MISC_IMPCAL_DIS_SUS_CH0_DRV) |
										 P_Fld(0, MISC_IMPCAL_DIS_SUS_CH1_DRV) |
										 P_Fld(1, MISC_IMPCAL_IMPSRCEXT) |		//Update mp setting
										 P_Fld(0, MISC_IMPCAL_IMPCAL_ECO_OPT)); //Update mp setting
#if (CHANNEL_NUM > 2)
	vIO32WriteFldMulti(DDRPHY_REG_MISC_IMPCAL + SHIFT_TO_CHC_ADDR, P_Fld(0, MISC_IMPCAL_DIS_SUS_CH0_DRV) | P_Fld(1, MISC_IMPCAL_DIS_SUS_CH1_DRV));
	vIO32WriteFldMulti(DDRPHY_REG_MISC_IMPCAL + SHIFT_TO_CHD_ADDR, P_Fld(1, MISC_IMPCAL_DIS_SUS_CH0_DRV) | P_Fld(0, MISC_IMPCAL_DIS_SUS_CH1_DRV));
#endif

	//Maoauo: keep following setting for SPMFW enable REFCTRL0_DRVCGWREF = 1 (Imp SW Save mode)
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_IMPCAL, 1, MISC_IMPCAL_DRVCGWREF); //@Maoauo, Wait AB refresh to avoid IO drive via logic design
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_IMPCAL, 1, MISC_IMPCAL_DQDRVSWUPD);
}
#endif

/* divRoundClosest() - to round up to the nearest integer
 * discard four, but treat five as whole (of decimal points)
 */
static int divRoundClosest(const int n, const int d)
{
  return ((n < 0) ^ (d < 0))? ((n - d / 2) / d): ((n + d / 2) / d);
}


#if (ENABLE_TX_TRACKING || TDQSCK_PRECALCULATION_FOR_DVFS)
void FreqJumpRatioCalculation(DRAMC_CTX_T *p)
{
	U32 shuffle_src_freq, shuffle_dst_index, jump_ratio_index;
	U16 u2JumpRatio[12] = {0}; /* Used to record __DBQUOTE_ANCHOR__ calulation results */
	U16 u2Freq = 0;

	/* Calculate jump ratios and save to u2JumpRatio array */
	jump_ratio_index = 0;

	if (p->frequency != 400)
	{
		shuffle_src_freq = p->frequency;
		for (shuffle_dst_index = DRAM_DFS_SHUFFLE_1; shuffle_dst_index < DRAM_DFS_SHUFFLE_MAX; shuffle_dst_index++)
		{
			DRAM_DFS_FREQUENCY_TABLE_T *pDstFreqTbl = get_FreqTbl_by_shuffleIndex(p, shuffle_dst_index);
			if (pDstFreqTbl == NULL)
			{
				mcSHOW_ERR_MSG(("NULL pFreqTbl\n"));
				#if __ETT__
				while (1);
				#endif
			}
			#if 0 //cc mark since been removed in new flow
			if (pDstFreqTbl->freq_sel == LP4_DDR800)
			{
				u2JumpRatio[jump_ratio_index] = 0;
			}
			else
			#endif
			{
				u2Freq = GetFreqBySel(p, pDstFreqTbl->freq_sel);
				u2JumpRatio[jump_ratio_index] = divRoundClosest(u2Freq * 32, shuffle_src_freq);
				//u2JumpRatio[jump_ratio_index] = (pDstFreqTbl->frequency/shuffle_src_freq)*32;
				//mcSHOW_DBG_MSG3(("shuffle_%d=DDR%d / shuffle_%d=DDR%d \n", shuffle_dst_index, pFreqTbl->frequency<<1,
				//															  shuffle_src_index, get_FreqTbl_by_shuffleIndex(p,shuffle_src_index)->frequency<<1));
				//mcSHOW_DBG_MSG3(("Jump_RATIO_%d : 0x%x\n", jump_ratio_index, u2JumpRatio[jump_ratio_index],
				//											  get_FreqTbl_by_shuffleIndex(p,shuffle_src_index)->frequency));
			}
			mcSHOW_DBG_MSG3(("Jump_RATIO [%d]: %x\tFreq %d -> %d\tDDR%d -> DDR%d\n", jump_ratio_index, u2JumpRatio[jump_ratio_index], get_shuffleIndex_by_Freq(p), shuffle_dst_index, shuffle_src_freq << 1, u2Freq << 1));
			jump_ratio_index++;
		}
	}

	/* Save jumpRatios into corresponding register fields */
	vIO32WriteFldMulti_All(DRAMC_REG_SHU_FREQ_RATIO_SET0, P_Fld(u2JumpRatio[0], SHU_FREQ_RATIO_SET0_TDQSCK_JUMP_RATIO0)
															| P_Fld(u2JumpRatio[1], SHU_FREQ_RATIO_SET0_TDQSCK_JUMP_RATIO1)
															| P_Fld(u2JumpRatio[2], SHU_FREQ_RATIO_SET0_TDQSCK_JUMP_RATIO2)
															| P_Fld(u2JumpRatio[3], SHU_FREQ_RATIO_SET0_TDQSCK_JUMP_RATIO3));
	vIO32WriteFldMulti_All(DRAMC_REG_SHU_FREQ_RATIO_SET1, P_Fld(u2JumpRatio[4], SHU_FREQ_RATIO_SET1_TDQSCK_JUMP_RATIO4)
															| P_Fld(u2JumpRatio[5], SHU_FREQ_RATIO_SET1_TDQSCK_JUMP_RATIO5)
															| P_Fld(u2JumpRatio[6], SHU_FREQ_RATIO_SET1_TDQSCK_JUMP_RATIO6)
															| P_Fld(u2JumpRatio[7], SHU_FREQ_RATIO_SET1_TDQSCK_JUMP_RATIO7));
	vIO32WriteFldMulti_All(DRAMC_REG_SHU_FREQ_RATIO_SET2, P_Fld(u2JumpRatio[8], SHU_FREQ_RATIO_SET2_TDQSCK_JUMP_RATIO8)
															| P_Fld(u2JumpRatio[9], SHU_FREQ_RATIO_SET2_TDQSCK_JUMP_RATIO9));
	return;
}
#endif


#if TDQSCK_PRECALCULATION_FOR_DVFS
void DramcDQSPrecalculation_preset(DRAMC_CTX_T *p)
{
	U8 mck, ui, pi;
	U8 mck_p1, ui_p1;
	U8 byte_idx, rank;
	U8 backup_rank;
	U8 mck2ui;

	backup_rank = u1GetRank(p);

#if 1//(fcFOR_CHIP_ID == fcMargaux)
	mck2ui = 4;
#else
	///TODO: use vGet_Div_Mode() instead later
	if (vGet_Div_Mode(p) == DIV16_MODE)
		mck2ui = 4; /* 1:16 mode */
	else if (vGet_Div_Mode(p) == DIV8_MODE)
		mck2ui = 3; /* 1: 8 mode */
	else
		mck2ui = 2; /* 1: 4 mode */
#endif

	mcSHOW_DBG_MSG(("Pre-setting of DQS Precalculation\n"));

	for (byte_idx = 0; byte_idx < (p->data_width / DQS_BIT_NUMBER); byte_idx++) {
		for (rank = RANK_0; rank < p->support_rank_num; rank++) {
			vSetRank(p, rank);

			if (byte_idx == 0) {
				mck = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B0_DQSIEN_MCK_UI_DLY),
					SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B0);
				ui = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B0_DQSIEN_MCK_UI_DLY),
					SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B0);

				mck_p1= u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B0_DQSIEN_MCK_UI_DLY),
					SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B0);
				ui_p1 = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B0_DQSIEN_MCK_UI_DLY),
					SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B0);

				pi = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B0_DQSIEN_PI_DLY),
					SHU_RK_B0_DQSIEN_PI_DLY_DQSIEN_PI_B0);

				vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_INI_UIPI),
					P_Fld((mck << mck2ui) + ui, SHU_R0_B0_INI_UIPI_CURR_INI_UI_B0) |
					P_Fld(pi, SHU_R0_B0_INI_UIPI_CURR_INI_PI_B0));
				vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_NEXT_INI_UIPI),
					P_Fld((mck << mck2ui) +ui, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_UI_B0) |
					P_Fld((mck_p1 << mck2ui) + ui_p1, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_UI_P1_B0) |
					P_Fld(pi, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_PI_B0));
			} else {
				mck = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B1_DQSIEN_MCK_UI_DLY),
					SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B1);
				ui = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B1_DQSIEN_MCK_UI_DLY),
					SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B1);

				mck_p1= u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B1_DQSIEN_MCK_UI_DLY),
					SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B1);
				ui_p1 = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B1_DQSIEN_MCK_UI_DLY),
					SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B1);

				pi = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B1_DQSIEN_PI_DLY),
					SHU_RK_B1_DQSIEN_PI_DLY_DQSIEN_PI_B1);

				vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_INI_UIPI),
					P_Fld((mck << mck2ui) + ui, SHU_R0_B1_INI_UIPI_CURR_INI_UI_B1) |
					P_Fld(pi, SHU_R0_B1_INI_UIPI_CURR_INI_PI_B1));
				vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_NEXT_INI_UIPI),
					P_Fld((mck << mck2ui) +ui, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_UI_B1) |
					P_Fld((mck_p1 << mck2ui) + ui_p1, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_UI_P1_B1) |
					P_Fld(pi, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_PI_B0));
			}
		}
	}

	vSetRank(p, backup_rank);

	/* Disable DDR800semi precal */
	if (vGet_Current_ShuLevel(p) == SRAM_SHU6) {///TODO: Confirm DDR800's shuffle
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_MISC_PRE_TDQSCK),
			0x1, SHU_MISC_PRE_TDQSCK_PRECAL_DISABLE);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_RX_CG_CTRL),
			0x1, MISC_SHU_RX_CG_CTRL_RX_PRECAL_CG_EN);
	}
	else
	{
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_MISC_PRE_TDQSCK),
			0x0, SHU_MISC_PRE_TDQSCK_PRECAL_DISABLE);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_RX_CG_CTRL),
			0x0, MISC_SHU_RX_CG_CTRL_RX_PRECAL_CG_EN);
	}
}

#if 0
void DramcDQSPrecalculation_preset(DRAMC_CTX_T *p)//Test tDQSCK_temp Pre-calculation
{
	U8 u1ByteIdx, u1RankNum, u1RankBackup = p->rank;
	U8 u1ShuLevel = vGet_Current_ShuLevel(p);
	U8 u1UI_value, u1PI_value, u1MCK_value;
	U16 u2Byte_offset;
	U32 u1Delay_Addr[2] = {0}, u1Delay_Fld[2];
	REG_FLD_DQS_PRE_K TransferReg;

	mcSHOW_DBG_MSG(("Pre-setting of DQS Precalculation\n"));
	mcDUMP_REG_MSG(("Pre-setting of DQS Precalculation\n"));

	if ((u1ShuLevel >= SRAM_SHU4) && (u1ShuLevel <= SRAM_SHU7))
	{ //SHU4, 5, 6, 7
		u1Delay_Addr[0] = ((u1ShuLevel / 6) * 0x4) + 0x30; //Offset of phase0 UI register
		u1Delay_Addr[1] = 0x38; //Offset of phase1 UI register
		u2Byte_offset = 0xc;
	}
	else if (u1ShuLevel >= SRAM_SHU8)
	{  //SHU8, 9
		u1Delay_Addr[0] = 0x260; //Offset of phase0 UI register
		u1Delay_Addr[1] = 0x268; //Offset of phase1 UI register
		u2Byte_offset = 0x4;
	}
	else //SHU0, 1, 2, 3
	{
		u1Delay_Addr[0] = ((u1ShuLevel / 2) * 0x4); //Offset of phase0 UI register
		u1Delay_Addr[1] = 0x8; //Offset of phase1 UI register
		u2Byte_offset = 0xc;
	}

	u1Delay_Fld[0] = u1ShuLevel % 2; //Field of phase0 PI and UI
	u1Delay_Fld[1] = u1ShuLevel % 4; //Field of phase1 UI

	switch (u1Delay_Fld[0]) //Phase0 UI and PI
	{
		case 0:
			TransferReg.u4UI_Fld = RK0_PRE_TDQSCK1_TDQSCK_UIFREQ1_B0R0;
			TransferReg.u4PI_Fld = RK0_PRE_TDQSCK1_TDQSCK_PIFREQ1_B0R0;
			break;
		case 1:
			TransferReg.u4UI_Fld = RK0_PRE_TDQSCK1_TDQSCK_UIFREQ2_B0R0;
			TransferReg.u4PI_Fld = RK0_PRE_TDQSCK1_TDQSCK_PIFREQ2_B0R0;
			break;
		default:
			break;
	}

	if (u1ShuLevel == SRAM_SHU8)
	{
		TransferReg.u4UI_Fld_P1[0] = RK0_PRE_TDQSCK27_TDQSCK_UIFREQ9_P1_B0R0; //Byte0
		TransferReg.u4UI_Fld_P1[1] = RK0_PRE_TDQSCK27_TDQSCK_UIFREQ9_P1_B1R0; //Byte1
	}
	else if (u1ShuLevel == SRAM_SHU9)
	{
		TransferReg.u4UI_Fld_P1[0] = RK0_PRE_TDQSCK27_TDQSCK_UIFREQ10_P1_B0R0; //Byte0
		TransferReg.u4UI_Fld_P1[1] = RK0_PRE_TDQSCK27_TDQSCK_UIFREQ10_P1_B1R0; //Byte1
	}
	else //(u1ShuLevel < SRAM_SHU8)
	{
		switch (u1Delay_Fld[1]) //Phase1 UI
		{
			case 0:
				TransferReg.u4UI_Fld_P1[0] = RK0_PRE_TDQSCK3_TDQSCK_UIFREQ1_P1_B0R0;
				break;
			case 1:
				TransferReg.u4UI_Fld_P1[0] = RK0_PRE_TDQSCK3_TDQSCK_UIFREQ2_P1_B0R0;
				break;
			case 2:
				TransferReg.u4UI_Fld_P1[0] = RK0_PRE_TDQSCK3_TDQSCK_UIFREQ3_P1_B0R0;
				break;
			case 3:
				TransferReg.u4UI_Fld_P1[0] = RK0_PRE_TDQSCK3_TDQSCK_UIFREQ4_P1_B0R0;
				break;
			default:
				break;
		}
	}

	for (u1ByteIdx = 0; u1ByteIdx < (p->data_width / DQS_BIT_NUMBER); u1ByteIdx++)
	{
		for (u1RankNum = 0; u1RankNum < p->support_rank_num; u1RankNum++)
		{
			vSetRank(p, u1RankNum);

			if (u1ByteIdx == 0)
			{
				u1MCK_value = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG0), SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED);
				u1UI_value = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG1), SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED);
				u1PI_value = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_DQSIEN), SHURK0_DQSIEN_R0DQS0IEN);
			}
			else //Byte1
			{
				u1MCK_value = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG0), SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED);
				u1UI_value = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG1), SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED);
				u1PI_value = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_DQSIEN), SHURK0_DQSIEN_R0DQS1IEN);
			}

			vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK1 + u1Delay_Addr[0] + (u1ByteIdx * u2Byte_offset)), (u1MCK_value << 3) | u1UI_value, TransferReg.u4UI_Fld);//UI
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK1 + u1Delay_Addr[0] + (u1ByteIdx * u2Byte_offset)), u1PI_value, TransferReg.u4PI_Fld); //PI

			if (u1ByteIdx == 0)
			{
				u1MCK_value = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG0), SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED_P1);
				u1UI_value = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG1), SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED_P1);
			}
			else //Byte1
			{
				u1MCK_value = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG0), SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED_P1);
				u1UI_value = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG1), SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED_P1);
			}

			if ((u1ShuLevel == SRAM_SHU8) || (u1ShuLevel == SRAM_SHU9))
				vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK1 + u1Delay_Addr[1]), (u1MCK_value << 3) | u1UI_value, TransferReg.u4UI_Fld_P1[u1ByteIdx]); //phase1 UI
			else
				vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK1 + u1Delay_Addr[1] + (u1ByteIdx * u2Byte_offset)), (u1MCK_value << 3) | u1UI_value, TransferReg.u4UI_Fld_P1[0]); //phase1 UI
		}
	}
	vSetRank(p, u1RankBackup);

	return;
}
#endif

void DramcDQSPrecalculation_enable(DRAMC_CTX_T *p)
{
	//DQS pre-K new mode
	//cc mark removed vIO32WriteFldAlign_All(DRAMC_REG_RK0_PRE_TDQSCK15, 0x1, RK0_PRE_TDQSCK15_SHUFFLE_LEVEL_MODE_SELECT);
	//Enable pre-K HW
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_PRE_TDQSCK1, 0x1, MISC_PRE_TDQSCK1_TDQSCK_PRECAL_HW);
	//Select HW flow
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_PRE_TDQSCK1, 0x1, MISC_PRE_TDQSCK1_TDQSCK_REG_DVFS);
	//Set Auto save to RG
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_PRE_TDQSCK1, 0x1, MISC_PRE_TDQSCK1_TDQSCK_HW_SW_UP_SEL);
}
#endif

#if 0 /* CC mark to use DV initial setting */
void DramcHWGatingInit(DRAMC_CTX_T *p)
{
#ifdef HW_GATING
	vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_STBCAL),
		0, MISC_SHU_STBCAL_STBCALEN);
	vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL),
		P_Fld(0, MISC_STBCAL_STBCAL2R) |
		//cc mark P_Fld(0,STBCAL_STB_SELPHYCALEN) |
		P_Fld(0, MISC_STBCAL_STBSTATE_OPT) |
		P_Fld(0, MISC_STBCAL_RKCHGMASKDIS) |
		P_Fld(0, MISC_STBCAL_REFUICHG) |
		P_Fld(1, MISC_STBCAL_PICGEN));

	vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL), P_Fld(1, MISC_STBCAL_DQSIENCG_CHG_EN));
	vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL), 0, MISC_STBCAL_CG_RKEN);
	vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL), 1, MISC_STBCAL_DQSIENCG_NORMAL_EN);
	vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CTRL1), 1, MISC_CTRL1_R_DMDQSIENCG_EN);

	DramcHWDQSGatingTracking_ModeSetting(p);
#endif
}
#endif

void DramcHWGatingOnOff(DRAMC_CTX_T *p, U8 u1OnOff)
{
#ifdef HW_GATING
	vIO32WriteFldMulti_All(DDRPHY_REG_MISC_DVFSCTL2,
		P_Fld(u1OnOff, MISC_DVFSCTL2_R_DVFS_OPTION) |
		P_Fld(u1OnOff, MISC_DVFSCTL2_R_DVFS_PARK_N));
	vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL2),
		u1OnOff, MISC_STBCAL2_STB_GERRSTOP);
	vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_STBCAL),
		u1OnOff, MISC_SHU_STBCAL_STBCALEN);
	vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_STBCAL),
		u1OnOff, MISC_SHU_STBCAL_STB_SELPHCALEN);
#else
	vIO32WriteFldMulti_All(DDRPHY_REG_MISC_DVFSCTL2,
		P_Fld(0x0, MISC_DVFSCTL2_R_DVFS_OPTION) |
		P_Fld(0x0, MISC_DVFSCTL2_R_DVFS_PARK_N));
	vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL2), 0,
		MISC_STBCAL2_STB_GERRSTOP);
	vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_STBCAL),
		0, MISC_SHU_STBCAL_STBCALEN);
	vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_STBCAL),
		0, MISC_SHU_STBCAL_STB_SELPHCALEN);
#endif
}


void DramcHWGatingDebugOnOff(DRAMC_CTX_T *p, U8 u1OnOff)
{
#ifdef HW_GATING
	// STBCAL2_STB_DBG_EN = 0x3, byte0/1 enable
	U8 u1EnB0B1 = (u1OnOff == ENABLE)? 0x3: 0x0;

	vIO32WriteFldMulti_All(DDRPHY_REG_MISC_STBCAL2,
		P_Fld(u1EnB0B1, MISC_STBCAL2_STB_DBG_EN) |
		P_Fld(u1OnOff, MISC_STBCAL2_STB_PIDLYCG_IG) |
		P_Fld(u1OnOff, MISC_STBCAL2_STB_UIDLYCG_IG) |
		P_Fld(u1OnOff, MISC_STBCAL2_STB_GERRSTOP) |
		P_Fld(0, MISC_STBCAL2_STB_DBG_CG_AO) |
		P_Fld(0, MISC_STBCAL2_STB_DBG_UIPI_UPD_OPT));
#endif

#if ENABLE_RX_FIFO_MISMATCH_DEBUG
	vIO32WriteFldAlign_All(DDRPHY_REG_B0_DQ9, 1, B0_DQ9_R_DMRXFIFO_STBENCMP_EN_B0);
	vIO32WriteFldAlign_All(DDRPHY_REG_B1_DQ9, 1, B1_DQ9_R_DMRXFIFO_STBENCMP_EN_B1);
#endif
}

#if (FOR_DV_SIMULATION_USED == 0 && SW_CHANGE_FOR_SIMULATION == 0)
#if (__ETT__ || CPU_RW_TEST_AFTER_K)
#if 0 // Please use memeset to initail value, due to different CHANNEL_NUM
U16 u2MaxGatingPos[CHANNEL_NUM][RANK_MAX][DQS_NUMBER] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
U16 u2MinGatingPos[CHANNEL_NUM][RANK_MAX][DQS_NUMBER] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
#endif
void DramcPrintHWGatingStatus(DRAMC_CTX_T *p, U8 u1Channel)
{
#ifdef HW_GATING
	U8 u1RankIdx, u1RankMax, u1ChannelBak;
	U8 u1Dqs_pi[DQS_BIT_NUMBER]={0}, u1Dqs_ui[DQS_BIT_NUMBER]={0}, u1Dqs_ui_P1[DQS_BIT_NUMBER]={0};
	U32 MANUDLLFRZ_bak, STBSTATE_OPT_bak;
	U32 backup_rank;

	u1ChannelBak = p->channel;
	vSetPHY2ChannelMapping(p, u1Channel);
	backup_rank = u1GetRank(p);

	if (p->support_rank_num == RANK_DUAL)
		u1RankMax = RANK_MAX;
	else
		u1RankMax = RANK_1;

	MANUDLLFRZ_bak = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DLLFRZ_CTRL), DLLFRZ_CTRL_MANUDLLFRZ);
	STBSTATE_OPT_bak = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL), MISC_STBCAL_STBSTATE_OPT);

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DLLFRZ_CTRL), 1, DLLFRZ_CTRL_MANUDLLFRZ);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL), 1, MISC_STBCAL_STBSTATE_OPT);

	for (u1RankIdx = 0; u1RankIdx < u1RankMax; u1RankIdx++)
	{
		vSetRank(p, u1RankIdx);
		mcSHOW_DBG_MSG(("[DramcHWGatingStatus] Channel=%d, Rank=%d\n", p->channel, u1RankIdx));

		u1Dqs_pi[0] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_GATING_ERR_LATCH_DLY_B0_RK0),
			GATING_ERR_LATCH_DLY_B0_RK0_DQSIEN0_PI_DLY_RK0);
		u1Dqs_ui[0] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_GATING_ERR_LATCH_DLY_B0_RK0),
			GATING_ERR_LATCH_DLY_B0_RK0_DQSIEN0_UI_P0_DLY_RK0);
		u1Dqs_ui_P1[0] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_GATING_ERR_LATCH_DLY_B0_RK0),
			GATING_ERR_LATCH_DLY_B0_RK0_DQSIEN0_UI_P1_DLY_RK0);;

		u1Dqs_pi[1] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_GATING_ERR_LATCH_DLY_B1_RK0),
			GATING_ERR_LATCH_DLY_B1_RK0_DQSIEN1_PI_DLY_RK0);
		u1Dqs_ui[1] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_GATING_ERR_LATCH_DLY_B1_RK0),
			GATING_ERR_LATCH_DLY_B1_RK0_DQSIEN1_UI_P0_DLY_RK0);
		u1Dqs_ui_P1[1] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_GATING_ERR_LATCH_DLY_B1_RK0),
			GATING_ERR_LATCH_DLY_B1_RK0_DQSIEN1_UI_P1_DLY_RK0);;

		mcSHOW_DBG_MSG(("Byte0(ui, pi) =(%d, %d)\n Byte1(ui, pi) =(%d, %d)\n",
			u1Dqs_ui[0], u1Dqs_pi[0], u1Dqs_ui[1], u1Dqs_pi[1]));
		mcSHOW_DBG_MSG(("UI_Phase1 (DQS0~3) =(%d, %d, %d, %d)\n\n",
			u1Dqs_ui_P1[0], u1Dqs_ui_P1[1], u1Dqs_ui_P1[2], u1Dqs_ui_P1[3]));
	}

	vSetRank(p, backup_rank);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL), STBSTATE_OPT_bak, MISC_STBCAL_STBSTATE_OPT);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DLLFRZ_CTRL), MANUDLLFRZ_bak, DLLFRZ_CTRL_MANUDLLFRZ);

	p->channel = u1ChannelBak;
	vSetPHY2ChannelMapping(p, u1ChannelBak);
#endif
}

void DramcHWGatingTrackingRecord(DRAMC_CTX_T *p, U8 u1Channel)
{
#ifdef HW_GATING
#if __A60868_TO_BE_PORTING__
	U8 u1RankIdx, u1RankMax, u1Info_NUM, u1Info_Max_MUM = 16;
	U8 u1ChannelBak, u1RankBak;

	U8 u1DBG_Dqs0_DFS, u1DBG_Dqs0_Lead, u1DBG_Dqs0_Lag, u1DBG_Dqs0_UI, u1DBG_Dqs0_PI;
	U8 u1DBG_Dqs1_DFS, u1DBG_Dqs1_Lead, u1DBG_Dqs1_Lag, u1DBG_Dqs1_UI, u1DBG_Dqs1_PI;
	U16 u4DBG_Dqs0_Info, u4DBG_Dqs1_Info;
	U32 u4DBG_Dqs01_Info;

	U32 u4Dqs0_MAX_MIN_DLY, u4Dqs1_MAX_MIN_DLY;
	U16 u2Dqs0_UI_MAX_DLY, u2Dqs0_PI_MAX_DLY, u2Dqs0_UI_MIN_DLY, u2Dqs0_PI_MIN_DLY;
	U16 u2Dqs1_UI_MAX_DLY, u2Dqs1_PI_MAX_DLY, u2Dqs1_UI_MIN_DLY, u2Dqs1_PI_MIN_DLY;
	U8 u1ShuffleLevel;

	u1ChannelBak = p->channel;
	vSetPHY2ChannelMapping(p, u1Channel);
	u1RankBak = u1GetRank(p);

	if (p->support_rank_num == RANK_DUAL)
		u1RankMax = RANK_MAX;
	else
		 u1RankMax = RANK_1;

	//Run Time HW Gating Debug Information
	//for(u1RankIdx=0; u1RankIdx<u1RankMax; u1RankIdx++)
	for (u1RankIdx = 0; u1RankIdx < u1RankMax; u1RankIdx++)
	{
		vSetRank(p, u1RankIdx);

		u1ShuffleLevel = u4IO32ReadFldAlign(DRAMC_REG_SHUSTATUS, SHUSTATUS_SHUFFLE_LEVEL);

		mcSHOW_DBG_MSG3(("\n[HWGatingTrackingRecord] Channel=%d, Rank=%d, SHU_LEVEL=%d\n", p->channel, u1RankIdx, u1ShuffleLevel));

		mcSHOW_DBG_MSG3(("Run Time HW Gating Debug Information :\n"));
		mcSHOW_DBG_MSG3(("		  B0=(DFS,Lead,Lag,4T, UI, PI), B1=(DFS,Lead,Lag,4T, UI, PI)\n"));

		for (u1Info_NUM = 0; u1Info_NUM < u1Info_Max_MUM; u1Info_NUM++)
		{
		  //DFS_ST(Shuffle Level): bit[15:14]
		  //Shift_R(Lead): bit[13]
		  //Shift_L(Lag) : bit[12]
		  //UI_DLY : bit[11:06]
		  //PI_DLY : bit[05:00]
		  u4DBG_Dqs01_Info = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_RK0_B01_STB_DBG_INFO_00 + 0x4 * u1Info_NUM));
		  u4DBG_Dqs0_Info = (u4DBG_Dqs01_Info >> 0) & 0xffff;
		  u4DBG_Dqs1_Info = (u4DBG_Dqs01_Info >> 16) & 0xffff;

		  u1DBG_Dqs0_DFS = (u4DBG_Dqs0_Info >> 14) & 0x03;
		  u1DBG_Dqs0_Lead = (u4DBG_Dqs0_Info >> 13) & 0x01;
		  u1DBG_Dqs0_Lag = (u4DBG_Dqs0_Info >> 12) & 0x01;
		  u1DBG_Dqs0_UI = (u4DBG_Dqs0_Info >> 6) & 0x3f;
		  u1DBG_Dqs0_PI = (u4DBG_Dqs0_Info >> 0) & 0x3f;

		  u1DBG_Dqs1_DFS = (u4DBG_Dqs1_Info >> 14) & 0x03;
		  u1DBG_Dqs1_Lead = (u4DBG_Dqs1_Info >> 13) & 0x01;
		  u1DBG_Dqs1_Lag = (u4DBG_Dqs1_Info >> 12) & 0x01;
		  u1DBG_Dqs1_UI = (u4DBG_Dqs1_Info >> 6) & 0x3f;
		  u1DBG_Dqs1_PI = (u4DBG_Dqs1_Info >> 0) & 0x3f;

		  if (u1Info_NUM < 10)
		  {
			mcSHOW_DBG_MSG3(("Info= %d ", u1Info_NUM));
		  }
		  else
		  {
			mcSHOW_DBG_MSG3(("Info=%d ", u1Info_NUM));
		  }

		  mcSHOW_DBG_MSG3(("B0=(  %d,  %d,	%d,  %d,  %d, %d), B1=(  %d,  %d,  %d,	%d,  %d,  %d)\n",
		  u1DBG_Dqs0_DFS, u1DBG_Dqs0_Lead, u1DBG_Dqs0_Lag, u1DBG_Dqs0_UI / 8, u1DBG_Dqs0_UI % 8, u1DBG_Dqs0_PI,
		  u1DBG_Dqs1_DFS, u1DBG_Dqs1_Lead, u1DBG_Dqs1_Lag, u1DBG_Dqs1_UI / 8, u1DBG_Dqs1_UI % 8, u1DBG_Dqs1_PI));
		}

		//Run Time HW Gating Max and Min Value Record
		//Run Time HW Gating MAX_DLY UI : bit[27:22]
		//Run Time HW Gating MAX_DLY PI : bit[21:16]
		//Run Time HW Gating MIN_DLY UI : bit[11:06]
		//Run Time HW Gating MIN_DLY PI : bit[05:00]
		u4Dqs0_MAX_MIN_DLY = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_RK0_B0_STB_MAX_MIN_DLY));
		u2Dqs0_UI_MAX_DLY = (u4Dqs0_MAX_MIN_DLY >> 22) & 0x3f;
		u2Dqs0_PI_MAX_DLY = (u4Dqs0_MAX_MIN_DLY >> 16) & 0x3f;
		u2Dqs0_UI_MIN_DLY = (u4Dqs0_MAX_MIN_DLY >> 6) & 0x3f;
		u2Dqs0_PI_MIN_DLY = (u4Dqs0_MAX_MIN_DLY >> 0) & 0x3f;

		u4Dqs1_MAX_MIN_DLY = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_RK0_B1_STB_MAX_MIN_DLY));
		u2Dqs1_UI_MAX_DLY = (u4Dqs1_MAX_MIN_DLY >> 22) & 0x3f;
		u2Dqs1_PI_MAX_DLY = (u4Dqs1_MAX_MIN_DLY >> 16) & 0x3f;
		u2Dqs1_UI_MIN_DLY = (u4Dqs1_MAX_MIN_DLY >> 6) & 0x3f;
		u2Dqs1_PI_MIN_DLY = (u4Dqs1_MAX_MIN_DLY >> 0) & 0x3f;

		mcSHOW_DBG_MSG3(("B0 = MAX(4T, UI, PI) MIN(4T, UI, PI), B1 = MAX(4T, UI, PI) MIN(4T, UI, PI)\n"));
		mcSHOW_DBG_MSG3(("B0 = MAX( %d,  %d, %d) MIN( %d,  %d, %d),  B1 = MAX( %d,	%d, %d) MIN( %d,  %d, %d)\n",
						u2Dqs0_UI_MAX_DLY / 8, u2Dqs0_UI_MAX_DLY % 8, u2Dqs0_PI_MAX_DLY,
						u2Dqs0_UI_MIN_DLY / 8, u2Dqs0_UI_MIN_DLY % 8, u2Dqs0_PI_MIN_DLY,
						u2Dqs1_UI_MAX_DLY / 8, u2Dqs1_UI_MAX_DLY % 8, u2Dqs1_PI_MAX_DLY,
						u2Dqs1_UI_MIN_DLY / 8, u2Dqs1_UI_MIN_DLY % 8, u2Dqs1_PI_MIN_DLY));
	}
	vSetRank(p, u1RankBak);
	p->channel = u1ChannelBak;
	vSetPHY2ChannelMapping(p, u1ChannelBak);
#endif
#endif
}

///TODO: wait for porting +++
#if __A60868_TO_BE_PORTING__

void DramcPrintRXFIFODebugStatus(DRAMC_CTX_T *p)
{
#if RX_PICG_NEW_MODE
	//RX FIFO debug feature, MP setting should enable debug function for Gating error information
	//APHY control new mode
	U32 u1ChannelBak, u4value;
	U8 u1ChannelIdx;

	u1ChannelBak = p->channel;

	for (u1ChannelIdx = CHANNEL_A; u1ChannelIdx < p->support_channel_num; u1ChannelIdx++)
	{
		p->channel = u1ChannelIdx;

		u4value = u4IO32Read4B(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBERR_RK0_R)) & (0xf << 24); //DDRPHY NAO bit24~27
		if (u4value)
		{
			mcSHOW_DBG_MSG(("\n[RXFIFODebugStatus] CH_%d MISC_STBERR_RK0_R_RX_ARDQ = 0x\033[1;36m%x\033[m for Gating error information\n", u1ChannelIdx, u4value));
		}
	}
	p->channel = u1ChannelBak;
	vSetPHY2ChannelMapping(p, u1ChannelBak);
#endif
}
#endif	//#if __ETT__
#endif

#endif /// __A60868_TO_BE_PORTING__
///TODO: wait for porting +++
