/* SPDX-License-Identifier: BSD-3-Clause */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "dramc_common.h"
#include "dramc_int_global.h"
#include "x_hal_io.h"
#if (FOR_DV_SIMULATION_USED == 0)
#include "dramc_top.h"
#endif

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

//U8 gu1MR23Done = FALSE;
U8 gu1MR23[CHANNEL_NUM][RANK_MAX];

U16 gu2DQSOSCTHRD_INC[CHANNEL_NUM][RANK_MAX];
U16 gu2DQSOSCTHRD_DEC[CHANNEL_NUM][RANK_MAX];
U16 gu2MR18[CHANNEL_NUM][RANK_MAX];
U16 gu2MR19[CHANNEL_NUM][RANK_MAX];
U16 gu2DQSOSC[CHANNEL_NUM][RANK_MAX];
U16 gu2DQSOscCnt[CHANNEL_NUM][RANK_MAX][2];


void DramcDQSOSCInit(void)
{
    memset(gu1MR23, 0x3F, sizeof(gu1MR23));
    memset(gu2DQSOSCTHRD_INC, 0x6, sizeof(gu2DQSOSCTHRD_INC));
    memset(gu2DQSOSCTHRD_DEC, 0x4, sizeof(gu2DQSOSCTHRD_DEC));
}

#if DQSOSC_SCSM
static DRAM_STATUS_T DramcStartDQSOSC_SCSM(DRAMC_CTX_T *p)
{
    U32 u4Response;
    U32 u4TimeCnt;
    U32 u4MRSRKBak;

    u4TimeCnt = TIME_OUT_CNT;
    mcSHOW_DBG_MSG2(("[DQSOSC]\n"));

    u4MRSRKBak = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL0), SWCMD_CTRL0_MRSRK);


    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DQSOSCR), 0, DQSOSCR_DQSOSC2RK);
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL0), u1GetRank(p), SWCMD_CTRL0_MRSRK);
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_MPC_OPTION), 1, MPC_OPTION_MPCRKEN);


    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), 1, SWCMD_EN_DQSOSCENEN);

    do
    {
        u4Response = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SPCMDRESP), SPCMDRESP_DQSOSCEN_RESPONSE);
        u4TimeCnt --;
        mcDELAY_US(1);
    } while ((u4Response == 0) && (u4TimeCnt > 0));

    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), 0, SWCMD_EN_DQSOSCENEN);

    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL0), u4MRSRKBak, SWCMD_CTRL0_MRSRK);

    if (u4TimeCnt == 0)
    {
        mcSHOW_ERR_MSG(("Start fail (time out)\n"));
        return DRAM_FAIL;
    }

    return DRAM_OK;
}
#endif

#if DQSOSC_RTSWCMD
static DRAM_STATUS_T DramcStartDQSOSC_RTSWCMD(DRAMC_CTX_T *p)
{
    U32 u4Response, u4TimeCnt;

    vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL2),
            P_Fld(0, SWCMD_CTRL2_RTSWCMD_AGE) |
            P_Fld(u1GetRank(p), SWCMD_CTRL2_RTSWCMD_RK));
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_MPC_CTRL),
            1, MPC_CTRL_RTSWCMD_HPRI_EN);
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_RTSWCMD_CNT),
            0x2a, RTSWCMD_CNT_RTSWCMD_CNT);
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN),
            2, SWCMD_EN_RTSWCMD_SEL);
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN),
            1, SWCMD_EN_RTSWCMDEN);

    u4TimeCnt = TIME_OUT_CNT;

    do {
        u4Response = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SPCMDRESP3),
                SPCMDRESP3_RTSWCMD_RESPONSE);
        u4TimeCnt--;
        mcDELAY_US(5);
    } while ((u4Response == 0) && (u4TimeCnt > 0));

    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN),
            0, SWCMD_EN_RTSWCMDEN);
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_MPC_CTRL),
            0, MPC_CTRL_RTSWCMD_HPRI_EN);

    if (u4TimeCnt == 0)
    {
        mcSHOW_ERR_MSG(("[LP5 RT SW Cmd MRW ] Resp fail (time out)\n"));
        return DRAM_FAIL;
    }

    return DRAM_OK;
}
#endif

#if DQSOSC_SWCMD
static DRAM_STATUS_T DramcStartDQSOSC_SWCMD(DRAMC_CTX_T *p)
{
    U32 u4Response;
    U32 u4TimeCnt = TIME_OUT_CNT;
    U32 u4RegBackupAddress[] = {DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL0), DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), DRAMC_REG_ADDR(DRAMC_REG_CKECTRL)};


    DramcBackupRegisters(p, u4RegBackupAddress, sizeof(u4RegBackupAddress)/sizeof(U32));

    mcSHOW_DBG_MSG4(("[ZQCalibration]\n"));
    //mcFPRINTF((fp_A60501, "[ZQCalibration]\n"));


    DramCLKAlwaysOnOff(p, ON, TO_ONE_CHANNEL);
    mcDELAY_US(1);


    CKEFixOnOff(p, TO_ALL_RANK, CKE_FIXON, TO_ONE_CHANNEL);


    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), 1, SWCMD_EN_WCK2DQI_START_SWTRIG);

    do
    {
        u4Response = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SPCMDRESP3), SPCMDRESP3_WCK2DQI_START_SWTRIG_RESPONSE);
        u4TimeCnt --;
        mcDELAY_US(1);

        mcSHOW_DBG_MSG4(("%d- ", u4TimeCnt));
        //mcFPRINTF((fp_A60501, "%d- ", u4TimeCnt));
    }while((u4Response==0) &&(u4TimeCnt>0));

    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), 0, SWCMD_EN_WCK2DQI_START_SWTRIG);

    if(u4TimeCnt==0)//time out
    {
        mcSHOW_ERR_MSG(("ZQCAL Start fail (time out)\n"));
        //mcFPRINTF((fp_A60501, "ZQCAL Start fail (time out)\n"));
        return DRAM_FAIL;
    }


    DramcRestoreRegisters(p, u4RegBackupAddress, sizeof(u4RegBackupAddress)/sizeof(U32));

    mcSHOW_DBG_MSG4(("\n[DramcZQCalibration] Done\n\n"));
    //mcFPRINTF((fp_A60501, "\n[DramcZQCalibration] Done\n\n"));

    return DRAM_OK;
}
#endif

static DRAM_STATUS_T DramcStartDQSOSC(DRAMC_CTX_T *p)
{
#if DQSOSC_SCSM
    return DramcStartDQSOSC_SCSM(p);
#elif DQSOSC_RTSWCMD
    return DramcStartDQSOSC_RTSWCMD(p);
#else
    return DramcStartDQSOSC_SWCMD(p);
#endif
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


    //vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_MRS), u1GetRank(p), MRS_MRSRK);
    DramcModeRegWriteByRank(p, p->rank, 23, u1MR23);


    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_DQSOSC_SET0), 1, SHU_DQSOSC_SET0_DQSOSCENDIS);


    DramCLKAlwaysOnOff(p, ON, TO_ONE_CHANNEL);

    CKEFixOnOff(p, p->rank, CKE_FIXON, TO_ONE_CHANNEL);

    DramcStartDQSOSC(p);
    mcDELAY_US(1);

    DramcModeRegReadByRank(p, p->rank, 18, &u2MR18);
    DramcModeRegReadByRank(p, p->rank, 19, &u2MR19);

#if (SW_CHANGE_FOR_SIMULATION == 0)

    u2DQSCnt = (u2MR18 & 0x00FF) | ((u2MR19 & 0x00FF) << 8);
    if (u2DQSCnt != 0)
        u2DQSOsc[0] = u1MR23 * 16 * 1000000 / (2 * u2DQSCnt * DDRPhyGetRealFreq(p));
    else
        u2DQSOsc[0] = 0;


    u2DQSCnt = (u2MR18 >> 8) | ((u2MR19 & 0xFF00));
    if (u2DQSCnt != 0)
        u2DQSOsc[1] = u1MR23 * 16 * 1000000 / (2 * u2DQSCnt * DDRPhyGetRealFreq(p));
    else
        u2DQSOsc[1] = 0;
    mcSHOW_DBG_MSG2(("[DQSOSCAuto] RK%d, (LSB)MR18= 0x%x, (MSB)MR19= 0x%x, tDQSOscB0 = %d ps tDQSOscB1 = %d ps\n", u1GetRank(p), u2MR18, u2MR19, u2DQSOsc[0], u2DQSOsc[1]));
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

DRAM_STATUS_T DramcDQSOSCMR23(DRAMC_CTX_T *p)
{
#if (SW_CHANGE_FOR_SIMULATION == 0)

    U8 u1MR23 = gu1MR23[p->channel][p->rank];
    U16 u2DQSOSC = gu2DQSOSC[p->channel][p->rank];
    U32 u4tCK = 1000000 / DDRPhyGetRealFreq(p);

    if (u2DQSOSC != 0)
    {
        gu2DQSOSCTHRD_INC[p->channel][p->rank] = (3 * u1MR23 * u4tCK * u4tCK) / (u2DQSOSC * u2DQSOSC * 20);
        gu2DQSOSCTHRD_DEC[p->channel][p->rank] = (u1MR23 * u4tCK * u4tCK) / (u2DQSOSC * u2DQSOSC * 10);
    }

    mcSHOW_DBG_MSG2(("CH%d_RK%d: MR19=0x%X, MR18=0x%X, DQSOSC=%d, MR23=%u, INC=%u, DEC=%u\n", p->channel, p->rank,
                    gu2MR19[p->channel][p->rank], gu2MR18[p->channel][p->rank], gu2DQSOSC[p->channel][p->rank],
                    u1MR23, gu2DQSOSCTHRD_INC[p->channel][p->rank], gu2DQSOSCTHRD_DEC[p->channel][p->rank]));
#endif
    return DRAM_OK;
}



DRAM_STATUS_T DramcDQSOSCSetMR18MR19(DRAMC_CTX_T *p)
{
    U16 u2DQSOscCnt[2];

    DramcDQSOSCAuto(p);


    gu2DQSOscCnt[p->channel][p->rank][0] = u2DQSOscCnt[0] = (gu2MR18[p->channel][p->rank] & 0x00FF) | ((gu2MR19[p->channel][p->rank] & 0x00FF) << 8);

    gu2DQSOscCnt[p->channel][p->rank][1] = u2DQSOscCnt[1] = (gu2MR18[p->channel][p->rank] >> 8) | ((gu2MR19[p->channel][p->rank] & 0xFF00));

    if ((p->dram_cbt_mode[p->rank] == CBT_NORMAL_MODE) && (gu2DQSOscCnt[p->channel][p->rank][1] == 0))
    {
        gu2DQSOscCnt[p->channel][p->rank][1] = u2DQSOscCnt[1] = u2DQSOscCnt[0];
    }

    vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHURK_DQSOSC), P_Fld(u2DQSOscCnt[0], SHURK_DQSOSC_DQSOSC_BASE_RK0) | P_Fld(u2DQSOscCnt[1], SHURK_DQSOSC_DQSOSC_BASE_RK0_B1));

    mcSHOW_DBG_MSG2(("CH%d RK%d: MR19=%X, MR18=%X\n", p->channel, p->rank, gu2MR19[p->channel][p->rank], gu2MR18[p->channel][p->rank]));
    //mcDUMP_REG_MSG(("CH%d RK%d: MR19=%X, MR18=%X\n", p->channel, p->rank, gu2MR19[p->channel][p->rank], gu2MR18[p->channel][p->rank]));
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
    else
    {
        u1FILT_PITHRD = 0x17;
        u1W2R_SEL = 0x2;
    }

    if (vGet_Div_Mode(p) == DIV4_MODE)
    {
        u1PRDCNT_DIV = 2;
        u1IsDiv4 = 1;
    }

    u1DQSOSCRCNT = ((p->frequency<< u1IsDiv4))/100;
    if ((p->frequency%100) != 0)
        u1DQSOSCRCNT++;

    if (gu1MR23[p->channel][RANK_1] > gu1MR23[p->channel][RANK_0])
        u2PRDCNTtmp = ((gu1MR23[p->channel][RANK_1]*100)/u1PRDCNT_DIV);
    else
        u2PRDCNTtmp = ((gu1MR23[p->channel][RANK_0]*100)/u1PRDCNT_DIV);

     u2PRDCNT = (u2PRDCNTtmp + ((u1DQSOSCRCNT*100)/16))/100;
     u1RoundUp = (u2PRDCNTtmp + ((u1DQSOSCRCNT*100)/16))%100;
     if (u1RoundUp != 0)
        u2PRDCNT++;


    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_DQSOSC_SET0), u2PRDCNT, SHU_DQSOSC_SET0_DQSOSC_PRDCNT);


    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_DQSOSCR), u1DQSOSCRCNT, SHU_DQSOSCR_DQSOSCRCNT);
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_TX_SET0), (u1FILT_PITHRD>>1), SHU_TX_SET0_DQS2DQ_FILT_PITHRD);
    vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_TX_SET0), P_Fld(u1W2R_SEL, SHU_TX_SET0_TXUPD_W2R_SEL) | P_Fld(0x0, SHU_TX_SET0_TXUPD_SEL));


    for (u1RankIdx = RANK_0; u1RankIdx < p->support_rank_num; u1RankIdx++)
    {
        vSetRank(p, u1RankIdx);
        vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_DQSOSC_THRD), gu2DQSOSCTHRD_INC[p->channel][u1RankIdx], SHURK_DQSOSC_THRD_DQSOSCTHRD_INC);
        vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_DQSOSC_THRD), gu2DQSOSCTHRD_DEC[p->channel][u1RankIdx], SHURK_DQSOSC_THRD_DQSOSCTHRD_DEC);
    }
    vSetRank(p, u1RankIdxBak);


    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_DQSOSC_SET0), u2DQSOSCENCNT, SHU_DQSOSC_SET0_DQSOSCENCNT);

    return DRAM_OK;
}

void DramcHwDQSOSC(DRAMC_CTX_T *p)
{
    DRAM_RANK_T rank_bak = u1GetRank(p);
    DRAM_CHANNEL_T ch_bak = p->channel;


    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TX_FREQ_RATIO_OLD_MODE0), 1, TX_FREQ_RATIO_OLD_MODE0_SHUFFLE_LEVEL_MODE_SELECT);


    vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TX_TRACKING_SET0), P_Fld(1, TX_TRACKING_SET0_SHU_PRELOAD_TX_HW)
                                            | P_Fld(0, TX_TRACKING_SET0_SHU_PRELOAD_TX_START)
                                            | P_Fld(0, TX_TRACKING_SET0_SW_UP_TX_NOW_CASE));


#if ENABLE_TMRRI_NEW_MODE
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_MPC_CTRL), 1, MPC_CTRL_MPC_BLOCKALE_OPT);
#else
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_MPC_CTRL), 0, MPC_CTRL_MPC_BLOCKALE_OPT);
#endif


    #if ENABLE_SW_TX_TRACKING
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CTRL1), 1, MISC_CTRL1_R_DMARPIDQ_SW);
    #else
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CTRL1), 0, MISC_CTRL1_R_DMARPIDQ_SW);
    #if ENABLE_PA_IMPRO_FOR_TX_TRACKING
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DCM_SUB_CTRL), 1, DCM_SUB_CTRL_SUBCLK_CTRL_TX_TRACKING);
    #endif
    #endif
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DQSOSCR), 1, DQSOSCR_ARUIDQ_SW);


    #if 0
    vSetRank(p, RANK_0);
    vSetPHY2ChannelMapping(p, CHANNEL_A);
    DramcModeRegWrite(p, 23, u1MR23);
    vSetPHY2ChannelMapping(p, CHANNEL_B);
    DramcModeRegWrite(p, 23, u1MR23);

    vSetRank(p, RANK_1);
    vSetPHY2ChannelMapping(p, CHANNEL_A);
    DramcModeRegWrite(p, 23, u1MR23);
    vSetPHY2ChannelMapping(p, CHANNEL_B);
    DramcModeRegWrite(p, 23, u1MR23);
    #endif


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


    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TX_SET0), 1, TX_SET0_DRSCLR_RK0_EN);

    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DQSOSCR), 1, DQSOSCR_DQSOSC_CALEN);

    vSetRank(p, rank_bak);
    vSetPHY2ChannelMapping(p, ch_bak);
}

void Enable_TX_Tracking(DRAMC_CTX_T *p)
{

    if (u1IsPhaseMode(p) == TRUE)
    {
        vIO32WriteFldAlign_All(DRAMC_REG_SHU_DQSOSC_SET0, 1, SHU_DQSOSC_SET0_DQSOSCENDIS);
    }
    else
    {
        vIO32WriteFldAlign_All(DRAMC_REG_SHU_DQSOSC_SET0, 0, SHU_DQSOSC_SET0_DQSOSCENDIS);
    }
}
#endif

#if RDSEL_TRACKING_EN
void Enable_RDSEL_Tracking(DRAMC_CTX_T *p, U16 u2Freq)
{

    if (u2Freq >= RDSEL_TRACKING_TH)
    {
        vIO32WriteFldAlign_All(DDRPHY_REG_SHU_MISC_RDSEL_TRACK, 0x0, SHU_MISC_RDSEL_TRACK_RDSEL_TRACK_EN);
        vIO32WriteFldMulti_All(DDRPHY_REG_SHU_MISC_RDSEL_TRACK, P_Fld(0x1, SHU_MISC_RDSEL_TRACK_RDSEL_TRACK_EN)
                                                                         | P_Fld(0x0, SHU_MISC_RDSEL_TRACK_RDSEL_HWSAVE_MSK));
    }
    else
    {
        vIO32WriteFldAlign_All(DDRPHY_REG_SHU_MISC_RDSEL_TRACK, 0x0, SHU_MISC_RDSEL_TRACK_RDSEL_TRACK_EN);
    }
}
#endif

#ifdef HW_GATING
void Enable_Gating_Tracking(DRAMC_CTX_T *p)
{

    if (u1IsPhaseMode(p) == TRUE)
    {
        vIO32WriteFldMulti_All(DDRPHY_REG_MISC_SHU_STBCAL,
            P_Fld(0x0, MISC_SHU_STBCAL_STBCALEN) |
            P_Fld(0x0, MISC_SHU_STBCAL_STB_SELPHCALEN));
    } else {
        vIO32WriteFldMulti_All(DDRPHY_REG_MISC_SHU_STBCAL,
            P_Fld(0x1, MISC_SHU_STBCAL_STBCALEN) |
            P_Fld(0x1, MISC_SHU_STBCAL_STB_SELPHCALEN));
    }
}
#endif

#if ENABLE_PER_BANK_REFRESH
void Enable_PerBank_Refresh(DRAMC_CTX_T *p)
{
    vIO32WriteFldAlign_All(DRAMC_REG_SHU_CONF0, 1, SHU_CONF0_PBREFEN);
}
#endif

void Enable_ClkTxRxLatchEn(DRAMC_CTX_T *p)
{

    BOOL isLP4_DSC = (p->DRAMPinmux == PINMUX_DSC)?1:0;
    vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B0_DQ13, 1, SHU_B0_DQ13_RG_TX_ARDQ_DLY_LAT_EN_B0);
    if (!isLP4_DSC)
        vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B1_DQ13, 1, SHU_B1_DQ13_RG_TX_ARDQ_DLY_LAT_EN_B1);
    else
        vIO32WriteFldAlign_All(DDRPHY_REG_SHU_CA_CMD13, 1, SHU_CA_CMD13_RG_TX_ARCA_DLY_LAT_EN_CA);

    vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B0_DQ10, 1, SHU_B0_DQ10_RG_RX_ARDQS_DLY_LAT_EN_B0);
    if (!isLP4_DSC)
        vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B1_DQ10, 1, SHU_B1_DQ10_RG_RX_ARDQS_DLY_LAT_EN_B1);
    else
        vIO32WriteFldAlign_All(DDRPHY_REG_SHU_CA_CMD10, 1, SHU_CA_CMD10_RG_RX_ARCLK_DLY_LAT_EN_CA);


    vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B0_DQ2, 1, SHU_B0_DQ2_RG_ARPI_OFFSET_LAT_EN_B0);
    if (!isLP4_DSC)
        vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B1_DQ2, 1, SHU_B1_DQ2_RG_ARPI_OFFSET_LAT_EN_B1);
    else
        vIO32WriteFldAlign_All(DDRPHY_REG_SHU_CA_CMD2, 1, SHU_CA_CMD2_RG_ARPI_OFFSET_LAT_EN_CA);


    vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B0_DQ11, 1, SHU_B0_DQ11_RG_RX_ARDQ_OFFSETC_LAT_EN_B0);
    vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B1_DQ11, 1, SHU_B1_DQ11_RG_RX_ARDQ_OFFSETC_LAT_EN_B1);
    vIO32WriteFldAlign_All(DDRPHY_REG_SHU_CA_CMD11, 1, SHU_CA_CMD11_RG_RX_ARCA_OFFSETC_LAT_EN_CA);

    if(!isLP4_DSC)
    {
        vIO32WriteFldMulti_All(DDRPHY_REG_SHU_CA_CMD10, P_Fld(0, SHU_CA_CMD10_RG_RX_ARCLK_RANK_SEL_LAT_EN_CA)\
                                                                    | P_Fld(0, SHU_CA_CMD10_RG_RX_ARCLK_DQSIEN_RANK_SEL_LAT_EN_CA));
        vIO32WriteFldAlign_All(DDRPHY_REG_SHU_CA_CMD11, 0, SHU_CA_CMD11_RG_RX_ARCA_RANK_SEL_LAT_EN_CA);
    }
    else
    {
        vIO32WriteFldMulti_All(DDRPHY_REG_SHU_B1_DQ10, P_Fld(0, SHU_B1_DQ10_RG_RX_ARDQS_RANK_SEL_LAT_EN_B1)\
                                                                    | P_Fld(0, SHU_B1_DQ10_RG_RX_ARDQS_DQSIEN_RANK_SEL_LAT_EN_B1));
        vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B1_DQ11, 0, SHU_B1_DQ11_RG_RX_ARDQ_RANK_SEL_LAT_EN_B1);
    }
}

#if ENABLE_TX_WDQS
void Enable_TxWDQS(DRAMC_CTX_T *p)
{
    BOOL isLP4_DSC = (p->DRAMPinmux == PINMUX_DSC)?1:0;

    vIO32WriteFldMulti_All(DDRPHY_REG_SHU_B0_DQ13, P_Fld(1, SHU_B0_DQ13_RG_TX_ARDQS_READ_BASE_EN_B0)
                                                                | P_Fld(1, SHU_B0_DQ13_RG_TX_ARDQSB_READ_BASE_EN_B0)
                                                                | P_Fld(1, SHU_B0_DQ13_RG_TX_ARDQS_READ_BASE_DATA_TIE_EN_B0)
                                                                | P_Fld(1, SHU_B0_DQ13_RG_TX_ARDQSB_READ_BASE_DATA_TIE_EN_B0));
    vIO32WriteFldMulti_All(DDRPHY_REG_SHU_B1_DQ13, P_Fld(!isLP4_DSC, SHU_B1_DQ13_RG_TX_ARDQS_READ_BASE_EN_B1)
                                                                | P_Fld(!isLP4_DSC, SHU_B1_DQ13_RG_TX_ARDQSB_READ_BASE_EN_B1)
                                                                | P_Fld(!isLP4_DSC, SHU_B1_DQ13_RG_TX_ARDQS_READ_BASE_DATA_TIE_EN_B1)
                                                                | P_Fld(!isLP4_DSC, SHU_B1_DQ13_RG_TX_ARDQSB_READ_BASE_DATA_TIE_EN_B1));
    vIO32WriteFldMulti_All(DDRPHY_REG_SHU_CA_CMD13, P_Fld(isLP4_DSC, SHU_CA_CMD13_RG_TX_ARCLK_READ_BASE_EN_CA)
                                                                | P_Fld(isLP4_DSC, SHU_CA_CMD13_RG_TX_ARCLKB_READ_BASE_EN_CA)
                                                                | P_Fld(isLP4_DSC, SHU_CA_CMD13_RG_TX_ARCLK_READ_BASE_DATA_TIE_EN_CA)
                                                                | P_Fld(isLP4_DSC, SHU_CA_CMD13_RG_TX_ARCLKB_READ_BASE_DATA_TIE_EN_CA));
}
#endif
#if 0
static void Enable_and_Trigger_TX_Retry(DRAMC_CTX_T *p)
{
#if TX_RETRY_CONTROL_BY_SPM
    vIO32WriteFldAlign_All(DRAMC_REG_TX_RETRY_SET0, 1, TX_RETRY_SET0_XSR_TX_RETRY_SPM_MODE);
#else
    vIO32WriteFldAlign_All(DRAMC_REG_TX_RETRY_SET0, 0, TX_RETRY_SET0_XSR_TX_RETRY_SPM_MODE);
#endif
    vIO32WriteFldMulti_All(DRAMC_REG_TX_RETRY_SET0, P_Fld(1, TX_RETRY_SET0_TX_RETRY_SHU_RESP_OPT)
                                            | P_Fld(0, TX_RETRY_SET0_TX_RETRY_UPDPI_CG_OPT)
                                            | P_Fld(1, TX_RETRY_SET0_XSR_TX_RETRY_OPT)
                                            | P_Fld(0, TX_RETRY_SET0_XSR_TX_RETRY_EN)
                                            | P_Fld(0, TX_RETRY_SET0_XSR_TX_RETRY_SW_EN));

    if (u1IsPhaseMode(p) == TRUE)
    {
        vIO32WriteFldAlign_All(DRAMC_REG_SHU_DQSOSC_SET0, 1, SHU_DQSOSC_SET0_DQSOSCENDIS);
    }
    else
    {
        vIO32WriteFldAlign_All(DRAMC_REG_SHU_DQSOSC_SET0, 0, SHU_DQSOSC_SET0_DQSOSCENDIS);
    }
}
#endif
#if ENABLE_SW_TX_TRACKING & __ETT__
void DramcSWTxTracking(DRAMC_CTX_T *p)
{
    U8 u1MR4OnOff;
    U8 rankIdx, rankBak;
    U8 u1SRAMShuLevel = vGet_Current_SRAMIdx(p);
    U8 u1CurrentShuLevel;
    U16 u2MR1819_Base[RANK_MAX][2], u2MR1819_Runtime[RANK_MAX][2];
    U16 u2DQSOSC_INC[RANK_MAX] = {6}, u2DQSOSC_DEC[RANK_MAX] = {4};
    U8 u1AdjPI[RANK_MAX][2];
    U8 u1OriginalPI_DQ[DRAM_DFS_SRAM_MAX][RANK_MAX][2];
    U8 u1UpdatedPI_DQ[DRAM_DFS_SRAM_MAX][RANK_MAX][2];
    U8 u1OriginalPI_DQM[DRAM_DFS_SRAM_MAX][RANK_MAX][2];
    U8 u1UpdatedPI_DQM[DRAM_DFS_SRAM_MAX][RANK_MAX][2];
    U8 u1FreqRatioTX[DRAM_DFS_SRAM_MAX];
    U8 shuIdx, shuBak, byteIdx;

    if (gAndroid_DVFS_en)
    {
    for (shuIdx = 0; shuIdx < DRAM_DFS_SRAM_MAX; shuIdx++)
    {
        DRAM_DFS_FREQUENCY_TABLE_T *pDstFreqTbl = get_FreqTbl_by_SRAMIndex(p, shuIdx);
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
            mcSHOW_DBG_MSG2(("[SWTxTracking] ShuLevel=%d, Ratio[%d]=%d (%d, %d)\n", u1SRAMShuLevel, shuIdx, u1FreqRatioTX[shuIdx], GetFreqBySel(p, pDstFreqTbl->freq_sel), p->frequency));
        }
    }
    }
    else
    {
        u1FreqRatioTX[0] = 8;
    }

    mcSHOW_DBG_MSG2(("[SWTxTracking] channel=%d\n", p->channel));
    rankBak = u1GetRank(p);
    shuBak = p->ShuRGAccessIdx;

    u1CurrentShuLevel = u4IO32ReadFldAlign(DDRPHY_REG_DVFS_STATUS, DVFS_STATUS_OTHER_SHU_GP);

    for (shuIdx = 0; shuIdx < DRAM_DFS_SRAM_MAX; shuIdx++)
    {
        if (shuIdx == u1SRAMShuLevel || !gAndroid_DVFS_en) {
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

        if (!gAndroid_DVFS_en)
            break;
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


        for (byteIdx = 0; byteIdx < 2; byteIdx++)
        {
            U16 deltaMR1819 = 0;

            if (u2MR1819_Runtime[p->rank][byteIdx] >= u2MR1819_Base[p->rank][byteIdx])
            {
                deltaMR1819 = u2MR1819_Runtime[p->rank][byteIdx] - u2MR1819_Base[p->rank][byteIdx];
                u1AdjPI[rankIdx][byteIdx] = deltaMR1819 / u2DQSOSC_INC[rankIdx];
                for (shuIdx = 0; shuIdx < DRAM_DFS_SRAM_MAX; shuIdx++)
                {
                    u1UpdatedPI_DQ[shuIdx][rankIdx][byteIdx] = u1OriginalPI_DQ[shuIdx][rankIdx][byteIdx] - (u1AdjPI[rankIdx][byteIdx] * u1FreqRatioTX[shuIdx] / u1FreqRatioTX[u1SRAMShuLevel]);
                    u1UpdatedPI_DQM[shuIdx][rankIdx][byteIdx] = u1OriginalPI_DQM[shuIdx][rankIdx][byteIdx] - (u1AdjPI[rankIdx][byteIdx] * u1FreqRatioTX[shuIdx] / u1FreqRatioTX[u1SRAMShuLevel]);
                    mcSHOW_DBG_MSG2(("SHU%u CH%d RK%d B%d, Base=%X Runtime=%X delta=%d INC=%d PI=0x%B Adj=%d newPI=0x%B\n", shuIdx, p->channel, u1GetRank(p), byteIdx
                                , u2MR1819_Base[p->rank][byteIdx], u2MR1819_Runtime[p->rank][byteIdx], deltaMR1819, u2DQSOSC_INC[rankIdx]
                                , u1OriginalPI_DQ[shuIdx][rankIdx][byteIdx], (u1AdjPI[rankIdx][byteIdx] * u1FreqRatioTX[shuIdx] / u1FreqRatioTX[u1SRAMShuLevel]), u1UpdatedPI_DQ[shuIdx][rankIdx][byteIdx]));

                    if (!gAndroid_DVFS_en)
                        break;
                }
            }
            else
            {
                deltaMR1819 = u2MR1819_Base[p->rank][byteIdx] - u2MR1819_Runtime[p->rank][byteIdx];
                u1AdjPI[rankIdx][byteIdx] = deltaMR1819 / u2DQSOSC_DEC[rankIdx];
                for (shuIdx = 0; shuIdx < DRAM_DFS_SRAM_MAX; shuIdx++)
                {
                    u1UpdatedPI_DQ[shuIdx][rankIdx][byteIdx] = u1OriginalPI_DQ[shuIdx][rankIdx][byteIdx] + (u1AdjPI[rankIdx][byteIdx] * u1FreqRatioTX[shuIdx] / u1FreqRatioTX[u1SRAMShuLevel]);
                    u1UpdatedPI_DQM[shuIdx][rankIdx][byteIdx] = u1OriginalPI_DQM[shuIdx][rankIdx][byteIdx] + (u1AdjPI[rankIdx][byteIdx] * u1FreqRatioTX[shuIdx] / u1FreqRatioTX[u1SRAMShuLevel]);
                    mcSHOW_DBG_MSG2(("SHU%u CH%d RK%d B%d, Base=%X Runtime=%X delta=%d DEC=%d PI=0x%B Adj=%d newPI=0x%B\n", shuIdx, p->channel, u1GetRank(p), byteIdx
                                , u2MR1819_Base[p->rank][byteIdx], u2MR1819_Runtime[p->rank][byteIdx], deltaMR1819, u2DQSOSC_DEC[rankIdx]
                                , u1OriginalPI_DQ[shuIdx][rankIdx][byteIdx], (u1AdjPI[rankIdx][byteIdx] * u1FreqRatioTX[shuIdx] / u1FreqRatioTX[u1SRAMShuLevel]), u1UpdatedPI_DQ[shuIdx][rankIdx][byteIdx]));

                    if (!gAndroid_DVFS_en)
                        break;
                }
            }
        }
    }

    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DQSOSCR), 1, DQSOSCR_TXUPDMODE);

    if (gAndroid_DVFS_en)
    {
    p->ShuRGAccessIdx = !u1CurrentShuLevel;
    for (shuIdx = 0; shuIdx < DRAM_DFS_SRAM_MAX; shuIdx++)
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
    }

    p->ShuRGAccessIdx = u1CurrentShuLevel;
    shuIdx = (gAndroid_DVFS_en) ? u1SRAMShuLevel : 0;
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

#if SIMULATION_RX_DVS || ENABLE_RX_TRACKING
void DramcRxInputDelayTrackingInit_byFreq(DRAMC_CTX_T *p)
{
    BOOL isLP4_DSC = (p->DRAMPinmux == PINMUX_DSC)?1:0;
    U8 u1DVS_Delay;
    U8 u1DVS_En=1;
    U32 u4WbrBackup = GetDramcBroadcast();
    DramcBroadcastOnOff(DRAMC_BROADCAST_ON);


#if (fcFOR_CHIP_ID == fc8195)

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

#if RX_DVS_NOT_SHU_WA
    if (isLP4_DSC)
    {

        u1DVS_En =1;
    }
#endif


    vIO32WriteFldAlign((DDRPHY_REG_SHU_B0_DQ5), u1DVS_Delay, SHU_B0_DQ5_RG_RX_ARDQS0_DVS_DLY_B0);
    if (!isLP4_DSC)
    {
    vIO32WriteFldAlign((DDRPHY_REG_SHU_B1_DQ5), u1DVS_Delay, SHU_B1_DQ5_RG_RX_ARDQS0_DVS_DLY_B1);
    }
    else
    {
        vIO32WriteFldAlign((DDRPHY_REG_SHU_CA_CMD5), u1DVS_Delay, SHU_CA_CMD5_RG_RX_ARCLK_DVS_DLY);
    }


    vIO32WriteFldMulti((DDRPHY_REG_SHU_B0_DQ7), P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B0)
                                                        | P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B0));
    vIO32WriteFldMulti((DDRPHY_REG_SHU_B1_DQ7), P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B1)
                                                        | P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B1));

    vIO32WriteFldAlign((DDRPHY_REG_SHU_B0_DQ11), u1DVS_En, SHU_B0_DQ11_RG_RX_ARDQ_DVS_EN_B0);
    if (!isLP4_DSC)
    {
    vIO32WriteFldAlign((DDRPHY_REG_SHU_B1_DQ11), u1DVS_En, SHU_B1_DQ11_RG_RX_ARDQ_DVS_EN_B1);
    }
    else
    {
        vIO32WriteFldAlign((DDRPHY_REG_CA_CMD5), u1DVS_En, CA_CMD5_RG_RX_ARCLK_DVS_EN);
    }

    DramcBroadcastOnOff(u4WbrBackup);
}
#endif


#if __A60868_TO_BE_PORTING__
#if RX_DLY_TRACK_ONLY_FOR_DEBUG
void DramcRxDlyTrackDebug(DRAMC_CTX_T *p)
{


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


    vIO32WriteFldMulti_All((DDRPHY_MISC_RXDVS2), P_Fld(0, MISC_RXDVS2_R_DMRXDVS_DBG_MON_EN)
                        | P_Fld(1, MISC_RXDVS2_R_DMRXDVS_DBG_MON_CLR)
                        | P_Fld(0, MISC_RXDVS2_R_DMRXDVS_DBG_PAUSE_EN));


    vIO32WriteFldAlign_All(DRAMC_REG_DUMMY_RD, 0x1, DUMMY_RD_DMY_WR_DBG);
    vIO32WriteFldAlign_All(DRAMC_REG_DUMMY_RD, 0x0, DUMMY_RD_DMY_WR_DBG);


    vIO32WriteFldMulti_All((DDRPHY_MISC_RXDVS2), P_Fld(1, MISC_RXDVS2_R_DMRXDVS_DBG_MON_EN)
                        | P_Fld(0, MISC_RXDVS2_R_DMRXDVS_DBG_MON_CLR)
                        | P_Fld(1, MISC_RXDVS2_R_DMRXDVS_DBG_PAUSE_EN));
}

void DramcPrintRxDlyTrackDebugStatus(DRAMC_CTX_T *p)
{
    U32 backup_rank, u1ChannelBak, u4value;
    U8 u1ChannelIdx, u1ChannelMax = p->support_channel_num;

    u1ChannelBak = p->channel;
    backup_rank = u1GetRank(p);

    for (u1ChannelIdx = CHANNEL_A; u1ChannelIdx < p->support_channel_num; u1ChannelIdx++)
    {
        p->channel = u1ChannelIdx;

        u4value = u4IO32Read4B(DRAMC_REG_ADDR(DDRPHY_MISC_DQ_RXDLY_TRRO18));
        //mcSHOW_DBG_MSG(("\nCH_%d DQ_RXDLY_TRRO18 = 0x\033[1;36m%x\033[m\n",u1ChannelIdx,u4value));
        if (u4value & 1)
        {
            mcSHOW_DBG_MSG3(("=== CH_%d DQ_RXDLY_TRRO18 = 0x\033[1;36m%x\033[m, %s %s shu: %d\n", u1ChannelIdx, u4value,
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
    U16 u2TmpValue, u2TmpUI[DQS_BYTE_NUMBER], u2TmpPI[DQS_BYTE_NUMBER];
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
        mcSHOW_DBG_MSG3(("[RXDQDQSStatus] CH%d, RK%d\n", p->channel, u1RankIdx));
        if (u1RankIdx == 0)
        u4ResultDQS_PI = u4IO32Read4B(DRAMC_REG_ADDR(DDRPHY_MISC_DQ_RXDLY_TRRO22));
        if (u1RankIdx == 1)
        u4ResultDQS_PI = u4IO32Read4B(DRAMC_REG_ADDR(DDRPHY_MISC_DQ_RXDLY_TRRO23));

        u1DQX_B0 = u4ResultDQS_PI & 0xff;
        u1DQS0 = (u4ResultDQS_PI >> 8) & 0xff;
        u1DQX_B1 = (u4ResultDQS_PI >> 16) & 0xff;
        u1DQS1 = (u4ResultDQS_PI >> 24) & 0xff;

        mcSHOW_DBG_MSG3(("DQX_B0, DQS0, DQX_B1, DQS1 =(%d, %d, %d, %d)\n\n", u1DQX_B0, u1DQS0, u1DQX_B1, u1DQS1));

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
        vIO32WriteFldMulti_All(DDRPHY_REG_MISC_SHU_DQSG_RETRY1, P_Fld(0, MISC_SHU_DQSG_RETRY1_RETRY_ROUND_NUM)
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
        vIO32WriteFldAlign_All(DRAMC_REG_TEST2_A4, 4, TEST2_A4_TESTAGENTRKSEL);
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
    #if 0
    vIO32WriteFldMulti_All(DRAMC_REG_IMPEDAMCE_CTRL1, P_Fld(2, IMPEDAMCE_CTRL1_DQS1_OFF) | P_Fld(2, IMPEDAMCE_CTRL1_DOS2_OFF));
    vIO32WriteFldMulti_All(DRAMC_REG_IMPEDAMCE_CTRL2, P_Fld(2, IMPEDAMCE_CTRL2_DQ1_OFF) | P_Fld(2, IMPEDAMCE_CTRL2_DQ2_OFF));
    #endif

    #ifdef IMP_DEBUG_ENABLE
    vIO32WriteFldMulti_All(DDRPHY_REG_MISC_DBG_IRQ_CTRL9, P_Fld(1, MISC_DBG_IRQ_CTRL9_IMP_CLK_ERR_CLEAN)
                                                        | P_Fld(1, MISC_DBG_IRQ_CTRL9_IMP_CMD_ERR_CLEAN)
                                                        | P_Fld(1, MISC_DBG_IRQ_CTRL9_IMP_DQ1_ERR_CLEAN)
                                                        | P_Fld(1, MISC_DBG_IRQ_CTRL9_IMP_DQ0_ERR_CLEAN)
                                                        | P_Fld(1, MISC_DBG_IRQ_CTRL9_IMP_DQS_ERR_CLEAN)
                                                        | P_Fld(1, MISC_DBG_IRQ_CTRL9_IMP_ODTN_ERR_CLEAN)
                                                        | P_Fld(1, MISC_DBG_IRQ_CTRL9_IMP_DRVN_ERR_CLEAN)
                                                        | P_Fld(1, MISC_DBG_IRQ_CTRL9_IMP_DRVP_ERR_CLEAN));
    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_IMP_CTRL1, 1, MISC_IMP_CTRL1_IMP_ABN_LAT_CLR);
    vIO32WriteFldMulti_All(DDRPHY_REG_MISC_IMP_CTRL1, P_Fld(3, MISC_IMP_CTRL1_IMP_DIFF_THD) | P_Fld(0, MISC_IMP_CTRL1_IMP_ABN_LAT_CLR));
    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_IMP_CTRL1, 1, MISC_IMP_CTRL1_IMP_ABN_LAT_EN);
    #endif


    //u1CHAB_en = (p->support_channel_num == CHANNEL_DUAL) ? ENABLE : DISABLE;
    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_CTRL0, u1CHAB_en, MISC_CTRL0_IMPCAL_CHAB_EN);


    vIO32WriteFldMulti_All(DDRPHY_REG_MISC_IMPCAL, P_Fld(1, MISC_IMPCAL_IMPCAL_HW) | P_Fld(0, MISC_IMPCAL_IMPCAL_EN) |
                                             P_Fld(1, MISC_IMPCAL_IMPCAL_SWVALUE_EN) | P_Fld(1, MISC_IMPCAL_IMPCAL_NEW_OLD_SL) |
                                             P_Fld(1, MISC_IMPCAL_IMPCAL_DRVUPDOPT) | P_Fld(1, MISC_IMPCAL_IMPCAL_CHGDRV_ECO_OPT) |
                                             P_Fld(1, MISC_IMPCAL_IMPCAL_SM_ECO_OPT) | P_Fld(1, MISC_IMPCAL_IMPBINARY) |
                                             P_Fld(1, MISC_IMPCAL_DRV_ECO_OPT));


    vIO32WriteFldMulti(DDRPHY_REG_MISC_CTRL0, P_Fld(0x1, MISC_CTRL0_IMPCAL_LP_ECO_OPT) |
                                         P_Fld(0x0, MISC_CTRL0_IMPCAL_TRACK_DISABLE));
    vIO32WriteFldMulti(DDRPHY_REG_MISC_CTRL0 + SHIFT_TO_CHB_ADDR, P_Fld(0x1, MISC_CTRL0_IMPCAL_LP_ECO_OPT) |
                                         P_Fld(0x1, MISC_CTRL0_IMPCAL_TRACK_DISABLE));


    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_IMPCAL, 1, MISC_IMPCAL_IMPCAL_BYPASS_UP_CA_DRV);


    vIO32WriteFldMulti(DDRPHY_REG_MISC_IMPCAL, P_Fld(0, MISC_IMPCAL_DIS_SUS_CH0_DRV) |
                                         P_Fld(1, MISC_IMPCAL_DIS_SUS_CH1_DRV) |
                                         P_Fld(0, MISC_IMPCAL_IMPSRCEXT) |
                                         P_Fld(1, MISC_IMPCAL_IMPCAL_ECO_OPT));
    vIO32WriteFldMulti(DDRPHY_REG_MISC_IMPCAL + SHIFT_TO_CHB_ADDR, P_Fld(1, MISC_IMPCAL_DIS_SUS_CH0_DRV) |
                                         P_Fld(0, MISC_IMPCAL_DIS_SUS_CH1_DRV) |
                                         P_Fld(1, MISC_IMPCAL_IMPSRCEXT) |
                                         P_Fld(0, MISC_IMPCAL_IMPCAL_ECO_OPT));
#if (CHANNEL_NUM > 2)
	if (channel_num_auxadc > 2) {
    	vIO32WriteFldMulti(DDRPHY_REG_MISC_IMPCAL + SHIFT_TO_CHC_ADDR, P_Fld(0, MISC_IMPCAL_DIS_SUS_CH0_DRV) | P_Fld(1, MISC_IMPCAL_DIS_SUS_CH1_DRV));
	    vIO32WriteFldMulti(DDRPHY_REG_MISC_IMPCAL + SHIFT_TO_CHD_ADDR, P_Fld(1, MISC_IMPCAL_DIS_SUS_CH0_DRV) | P_Fld(0, MISC_IMPCAL_DIS_SUS_CH1_DRV));
	    vIO32WriteFldMulti(DDRPHY_REG_MISC_CTRL0 + SHIFT_TO_CHC_ADDR, P_Fld(0x1, MISC_CTRL0_IMPCAL_LP_ECO_OPT) | P_Fld(0x0, MISC_CTRL0_IMPCAL_TRACK_DISABLE));
	    vIO32WriteFldMulti(DDRPHY_REG_MISC_CTRL0 + SHIFT_TO_CHD_ADDR, P_Fld(0x1, MISC_CTRL0_IMPCAL_LP_ECO_OPT) | P_Fld(0x1, MISC_CTRL0_IMPCAL_TRACK_DISABLE));
	    vIO32WriteFldMulti(DDRPHY_REG_MISC_IMPCAL + SHIFT_TO_CHC_ADDR, P_Fld(0, MISC_IMPCAL_IMPSRCEXT) | P_Fld(1, MISC_IMPCAL_IMPCAL_ECO_OPT));
	    vIO32WriteFldMulti(DDRPHY_REG_MISC_IMPCAL + SHIFT_TO_CHD_ADDR, P_Fld(1, MISC_IMPCAL_IMPSRCEXT) | P_Fld(0, MISC_IMPCAL_IMPCAL_ECO_OPT));
	}
#endif


    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_IMPCAL, 1, MISC_IMPCAL_DRVCGWREF);
    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_IMPCAL, 1, MISC_IMPCAL_DQDRVSWUPD);
}
#endif

#if __ETT__
void DramcPrintIMPTrackingStatus(DRAMC_CTX_T *p, U8 u1Channel)
{
#ifdef IMPEDANCE_TRACKING_ENABLE

    U8 u1RankIdx, u1RankMax, u1ChannelBak, u1ByteIdx, ii;
    U8 DQS_DRVN_2, DQS_DRVP_2, DQS_ODTN_2, DQS_DRVN, DQS_DRVP, DQS_ODTN;
    U8 DQ_DRVN_2, DQ_DRVP_2, DQ_ODTN_2, DQ_DRVN, DQ_DRVP, DQ_ODTN;
    U8 CMD_DRVN_2, CMD_DRVP_2, CMD_ODTN_2, CMD_DRVN, CMD_DRVP, CMD_ODTN;

    u1ChannelBak = p->channel;
    vSetPHY2ChannelMapping(p, u1Channel);

    mcSHOW_DBG_MSG3(("[IMPTrackingStatus] CH=%d\n", p->channel));

//    if (u1Channel == CHANNEL_A)
    {


        DQS_DRVN_2 = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_IMPCAL_STATUS2), MISC_IMPCAL_STATUS2_DRVNDQS_SAVE_2);
        DQS_DRVP_2 = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_IMPCAL_STATUS2), MISC_IMPCAL_STATUS2_DRVPDQS_SAVE_2);
        DQS_ODTN_2 = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_IMPCAL_STATUS2), MISC_IMPCAL_STATUS2_ODTNDQS_SAVE_2);
        DQS_DRVN = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_IMPCAL_STATUS1), MISC_IMPCAL_STATUS1_DRVNDQS_SAVE_1);
        DQS_DRVP = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_IMPCAL_STATUS1), MISC_IMPCAL_STATUS1_DRVPDQS_SAVE_1);
        DQS_ODTN = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_IMPCAL_STATUS1), MISC_IMPCAL_STATUS1_ODTNDQS_SAVE_1);


        DQ_DRVN_2 = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_IMPCAL_STATUS4), MISC_IMPCAL_STATUS4_DRVNDQ_SAVE_2);
        DQ_DRVP_2 = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_IMPCAL_STATUS4), MISC_IMPCAL_STATUS4_DRVPDQ_SAVE_2);
        DQ_ODTN_2 = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_IMPCAL_STATUS4), MISC_IMPCAL_STATUS4_ODTNDQ_SAVE_2);
        DQ_DRVN = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_IMPCAL_STATUS3), MISC_IMPCAL_STATUS3_DRVNDQ_SAVE_1);
        DQ_DRVP = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_IMPCAL_STATUS3), MISC_IMPCAL_STATUS3_DRVPDQ_SAVE_1);
        DQ_ODTN = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_IMPCAL_STATUS3), MISC_IMPCAL_STATUS3_ODTNDQ_SAVE_1);


        CMD_DRVN_2 = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_IMPCAL_STATUS8), MISC_IMPCAL_STATUS8_DRVNCMD_SAVE_2);
        CMD_DRVP_2 = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_IMPCAL_STATUS8), MISC_IMPCAL_STATUS8_DRVPCMD_SAVE_2);
        CMD_ODTN_2 = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_IMPCAL_STATUS8), MISC_IMPCAL_STATUS8_ODTNCMD_SAVE_2);
        CMD_DRVN = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_IMPCAL_STATUS7), MISC_IMPCAL_STATUS7_DRVNCMD_SAVE_1);
        CMD_DRVP = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_IMPCAL_STATUS7), MISC_IMPCAL_STATUS7_DRVPCMD_SAVE_1);
        CMD_ODTN = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_IMPCAL_STATUS7), MISC_IMPCAL_STATUS7_ODTNCMD_SAVE_1);
    }
#if 0
    else
    {
        U8 shu_level;


        shu_level = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHUSTATUS), SHUSTATUS_SHUFFLE_LEVEL);
        mcSHOW_DBG_MSG(("shu_level=%d\n", shu_level));


        DQ_DRVP_2 = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_DRVING1 + shu_level * SHU_GRP_DRAMC_OFFSET), SHU_DRVING1_DQDRV2_DRVP);
        DQ_DRVP = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_DRVING2 + shu_level * SHU_GRP_DRAMC_OFFSET), SHU_DRVING2_DQDRV1_DRVP);
        DQ_ODTN_2 = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_DRVING3 + shu_level * SHU_GRP_DRAMC_OFFSET), SHU_DRVING3_DQODT2_ODTN);
        DQ_ODTN = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_DRVING4 + shu_level * SHU_GRP_DRAMC_OFFSET), SHU_DRVING4_DQODT1_ODTN);


        DQS_DRVP_2 = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_DRVING1 + shu_level * SHU_GRP_DRAMC_OFFSET), SHU_DRVING1_DQSDRV2_DRVP);
        DQS_DRVP = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_DRVING1 + shu_level * SHU_GRP_DRAMC_OFFSET), SHU_DRVING1_DQSDRV1_DRVP);
        DQS_ODTN_2 = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_DRVING3 + shu_level * SHU_GRP_DRAMC_OFFSET), SHU_DRVING3_DQSODT2_ODTN);
        DQS_ODTN = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_DRVING3 + shu_level * SHU_GRP_DRAMC_OFFSET), SHU_DRVING3_DQSODT1_ODTN);


        CMD_DRVP_2 = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_DRVING2 + shu_level * SHU_GRP_DRAMC_OFFSET), SHU_DRVING2_CMDDRV2_DRVP);
        CMD_DRVP = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_DRVING2 + shu_level * SHU_GRP_DRAMC_OFFSET), SHU_DRVING2_CMDDRV1_DRVP);
        CMD_ODTN_2 = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_DRVING4 + shu_level * SHU_GRP_DRAMC_OFFSET), SHU_DRVING4_CMDODT2_ODTN);
        CMD_ODTN = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_DRVING4 + shu_level * SHU_GRP_DRAMC_OFFSET), SHU_DRVING4_CMDODT1_ODTN);
    }
#endif

    mcSHOW_DBG_MSG3(("\tDRVN_2\tDRVP_2\tODTN_2\tDRVN\tDRVP\tODTN\n"
                    "DQS\t%d\t%d\t%d\t%d\t%d\t%d\n"
                    "DQ\t%d\t%d\t%d\t%d\t%d\t%d\n"
                    "CMD\t%d\t%d\t%d\t%d\t%d\t%d\n",
                    DQS_DRVN_2, DQS_DRVP_2, DQS_ODTN_2, DQS_DRVN, DQS_DRVP, DQS_ODTN,
                    DQ_DRVN_2, DQ_DRVP_2, DQ_ODTN_2, DQ_DRVN, DQ_DRVP, DQ_ODTN,
                    CMD_DRVN_2, CMD_DRVP_2, CMD_ODTN_2, CMD_DRVN, CMD_DRVP, CMD_ODTN));

    p->channel = u1ChannelBak;
    vSetPHY2ChannelMapping(p, u1ChannelBak);
#endif
}
#endif

int divRoundClosest(const int n, const int d)
{
  return ((n < 0) ^ (d < 0))? ((n - d / 2) / d): ((n + d / 2) / d);
}


#if (ENABLE_TX_TRACKING || TDQSCK_PRECALCULATION_FOR_DVFS)
void FreqJumpRatioCalculation(DRAMC_CTX_T *p)
{
    U32 shuffle_src_freq, shuffle_dst_index, jump_ratio_index;
    U16 u2JumpRatio[12] = {0};
    U16 u2Freq = 0;


    jump_ratio_index = 0;

    if (vGet_DDR_Loop_Mode(p) == CLOSE_LOOP_MODE)
    {
        shuffle_src_freq = p->frequency;
        for (shuffle_dst_index = SRAM_SHU0; shuffle_dst_index < DRAM_DFS_SRAM_MAX; shuffle_dst_index++)
        {
            DRAM_DFS_FREQUENCY_TABLE_T *pDstFreqTbl = get_FreqTbl_by_SRAMIndex(p, shuffle_dst_index);
            if (pDstFreqTbl == NULL)
            {
                mcSHOW_ERR_MSG(("NULL pFreqTbl\n"));
                #if __ETT__
                while (1);
                #endif
            }
            #if 0
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
                //                                                            shuffle_src_index, get_FreqTbl_by_SRAMIndex(p,shuffle_src_index)->frequency<<1));
                //mcSHOW_DBG_MSG3(("Jump_RATIO_%d : 0x%x\n", jump_ratio_index, u2JumpRatio[jump_ratio_index],
                //                                            get_FreqTbl_by_SRAMIndex(p,shuffle_src_index)->frequency));
            }
            #if DUMP_ALLSUH_RG
            mcSHOW_DBG_MSG(("[DUMPLOG] %d Jump_RATIO[%d]=%d, Jump_DDR%d=%d, Jump_DDR%d=%d\n", p->frequency*2, jump_ratio_index, u2JumpRatio[jump_ratio_index], vGet_Current_SRAMIdx(p), shuffle_src_freq << 1, shuffle_dst_index, u2Freq << 1));
            #endif
            jump_ratio_index++;
        }
    }


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
    U8 set_value;

    backup_rank = u1GetRank(p);

#if 1//(fcFOR_CHIP_ID == fc8195)
    mck2ui = 4;
#else

    if (vGet_Div_Mode(p) == DIV16_MODE)
        mck2ui = 4;
    else if (vGet_Div_Mode(p) == DIV8_MODE)
        mck2ui = 3;
    else
        mck2ui = 2;
#endif

    mcSHOW_DBG_MSG2(("Pre-setting of DQS Precalculation\n"));

    for (byte_idx = 0; byte_idx < DQS_BYTE_NUMBER; byte_idx++) {
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


    if (vGet_DDR_Loop_Mode(p) != CLOSE_LOOP_MODE)
    {
        set_value = 1;
    }
    else
    {
        set_value = 0;
    }

    vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_MISC_PRE_TDQSCK),
        set_value, SHU_MISC_PRE_TDQSCK_PRECAL_DISABLE);
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_RX_CG_CTRL),
        set_value, MISC_SHU_RX_CG_CTRL_RX_PRECAL_CG_EN);
    #if DUMP_ALLSUH_RG
    mcSHOW_DBG_MSG(("[DUMPLOG] %d RX_DQS_PREK_DISABLE Channel%d %d\n", p->frequency * 2, p->channel, set_value));
    #endif
}

#if 0
void DramcDQSPrecalculation_preset(DRAMC_CTX_T *p)
{
    U8 u1ByteIdx, u1RankNum, u1RankBackup = p->rank;
    U8 u1ShuLevel = vGet_Current_SRAMIdx(p);
    U8 u1UI_value, u1PI_value, u1MCK_value;
    U16 u2Byte_offset;
    U32 u1Delay_Addr[2] = {0}, u1Delay_Fld[2];
    REG_FLD_DQS_PRE_K TransferReg;

    mcSHOW_DBG_MSG(("Pre-setting of DQS Precalculation\n"));
    mcDUMP_REG_MSG(("Pre-setting of DQS Precalculation\n"));

    if ((u1ShuLevel >= SRAM_SHU4) && (u1ShuLevel <= SRAM_SHU7))
    {
        u1Delay_Addr[0] = ((u1ShuLevel / 6) * 0x4) + 0x30;
        u1Delay_Addr[1] = 0x38;
        u2Byte_offset = 0xc;
    }
    else if (u1ShuLevel >= SRAM_SHU8)
    {
        u1Delay_Addr[0] = 0x260;
        u1Delay_Addr[1] = 0x268;
        u2Byte_offset = 0x4;
    }
    else
    {
        u1Delay_Addr[0] = ((u1ShuLevel / 2) * 0x4);
        u1Delay_Addr[1] = 0x8;
        u2Byte_offset = 0xc;
    }

    u1Delay_Fld[0] = u1ShuLevel % 2;
    u1Delay_Fld[1] = u1ShuLevel % 4;

    switch (u1Delay_Fld[0])
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
        TransferReg.u4UI_Fld_P1[0] = RK0_PRE_TDQSCK27_TDQSCK_UIFREQ9_P1_B0R0;
        TransferReg.u4UI_Fld_P1[1] = RK0_PRE_TDQSCK27_TDQSCK_UIFREQ9_P1_B1R0;
    }
    else if (u1ShuLevel == SRAM_SHU9)
    {
        TransferReg.u4UI_Fld_P1[0] = RK0_PRE_TDQSCK27_TDQSCK_UIFREQ10_P1_B0R0;
        TransferReg.u4UI_Fld_P1[1] = RK0_PRE_TDQSCK27_TDQSCK_UIFREQ10_P1_B1R0;
    }
    else
    {
        switch (u1Delay_Fld[1])
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
            else
            {
                u1MCK_value = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG0), SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED);
                u1UI_value = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG1), SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED);
                u1PI_value = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_DQSIEN), SHURK0_DQSIEN_R0DQS1IEN);
            }

            vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK1 + u1Delay_Addr[0] + (u1ByteIdx * u2Byte_offset)), (u1MCK_value << 3) | u1UI_value, TransferReg.u4UI_Fld);
            vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK1 + u1Delay_Addr[0] + (u1ByteIdx * u2Byte_offset)), u1PI_value, TransferReg.u4PI_Fld);

            if (u1ByteIdx == 0)
            {
                u1MCK_value = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG0), SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED_P1);
                u1UI_value = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG1), SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED_P1);
            }
            else
            {
                u1MCK_value = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG0), SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED_P1);
                u1UI_value = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG1), SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED_P1);
            }

            if ((u1ShuLevel == SRAM_SHU8) || (u1ShuLevel == SRAM_SHU9))
                vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK1 + u1Delay_Addr[1]), (u1MCK_value << 3) | u1UI_value, TransferReg.u4UI_Fld_P1[u1ByteIdx]);
            else
                vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK1 + u1Delay_Addr[1] + (u1ByteIdx * u2Byte_offset)), (u1MCK_value << 3) | u1UI_value, TransferReg.u4UI_Fld_P1[0]);
        }
    }
    vSetRank(p, u1RankBackup);

    return;
}
#endif

void DramcDQSPrecalculation_enable(DRAMC_CTX_T *p)
{

    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_PRE_TDQSCK1, 0x1, MISC_PRE_TDQSCK1_TDQSCK_PRECAL_HW);

    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_PRE_TDQSCK1, 0x1, MISC_PRE_TDQSCK1_TDQSCK_REG_DVFS);

    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_PRE_TDQSCK1, 0x1, MISC_PRE_TDQSCK1_TDQSCK_HW_SW_UP_SEL);
}
#endif

#if 0
void DramcHWGatingInit(DRAMC_CTX_T *p)
{
#ifdef HW_GATING
    vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_STBCAL),
        0, MISC_SHU_STBCAL_STBCALEN);
    vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL),
        P_Fld(0, MISC_STBCAL_STBCAL2R) |
        //P_Fld(0,STBCAL_STB_SELPHYCALEN) |
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
    vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL2),
        u1OnOff, MISC_STBCAL2_STB_GERRSTOP);
    vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_STBCAL),
        u1OnOff, MISC_SHU_STBCAL_STBCALEN);
    vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_STBCAL),
        u1OnOff, MISC_SHU_STBCAL_STB_SELPHCALEN);
#else
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
#if 0
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
        mcSHOW_DBG_MSG3(("[DramcHWGatingStatus] Channel=%d, Rank=%d\n", p->channel, u1RankIdx));

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

        mcSHOW_DBG_MSG3(("Byte0(ui, pi) =(%x, %x)  Byte1(ui, pi) =(%x, %x)\n",
            u1Dqs_ui[0], u1Dqs_pi[0], u1Dqs_ui[1], u1Dqs_pi[1]));
        mcSHOW_DBG_MSG3(("UI_Phase1 (DQS0~1) =(%x, %x)\n\n",
            u1Dqs_ui_P1[0], u1Dqs_ui_P1[1]));
    }

    vSetRank(p, backup_rank);
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL), STBSTATE_OPT_bak, MISC_STBCAL_STBSTATE_OPT);
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DLLFRZ_CTRL), MANUDLLFRZ_bak, DLLFRZ_CTRL_MANUDLLFRZ);

    p->channel = u1ChannelBak;
    vSetPHY2ChannelMapping(p, u1ChannelBak);
#endif
}
#if 0
static void DramcHWGatingTrackingRecord(DRAMC_CTX_T *p, U8 u1Channel)
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


    //for(u1RankIdx=0; u1RankIdx<u1RankMax; u1RankIdx++)
    for (u1RankIdx = 0; u1RankIdx < u1RankMax; u1RankIdx++)
    {
        vSetRank(p, u1RankIdx);

        u1ShuffleLevel = u4IO32ReadFldAlign(DRAMC_REG_SHUSTATUS, SHUSTATUS_SHUFFLE_LEVEL);

        mcSHOW_DBG_MSG3(("\n[HWGatingTrackingRecord] Channel=%d, Rank=%d, SHU_LEVEL=%d\n", p->channel, u1RankIdx, u1ShuffleLevel));

        mcSHOW_DBG_MSG3(("Run Time HW Gating Debug Information :\n"));
        mcSHOW_DBG_MSG3(("        B0=(DFS,Lead,Lag,4T, UI, PI), B1=(DFS,Lead,Lag,4T, UI, PI)\n"));

        for (u1Info_NUM = 0; u1Info_NUM < u1Info_Max_MUM; u1Info_NUM++)
        {

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

          mcSHOW_DBG_MSG3(("B0=(  %d,  %d,  %d,  %d,  %d, %d), B1=(  %d,  %d,  %d,  %d,  %d,  %d)\n",
          u1DBG_Dqs0_DFS, u1DBG_Dqs0_Lead, u1DBG_Dqs0_Lag, u1DBG_Dqs0_UI / 8, u1DBG_Dqs0_UI % 8, u1DBG_Dqs0_PI,
          u1DBG_Dqs1_DFS, u1DBG_Dqs1_Lead, u1DBG_Dqs1_Lag, u1DBG_Dqs1_UI / 8, u1DBG_Dqs1_UI % 8, u1DBG_Dqs1_PI));
        }


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
        mcSHOW_DBG_MSG3(("B0 = MAX( %d,  %d, %d) MIN( %d,  %d, %d),  B1 = MAX( %d,  %d, %d) MIN( %d,  %d, %d)\n",
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
#endif
///TODO: wait for porting +++
#if __A60868_TO_BE_PORTING__

void DramcPrintRXFIFODebugStatus(DRAMC_CTX_T *p)
{
#if RX_PICG_NEW_MODE

    U32 u1ChannelBak, u4value;
    U8 u1ChannelIdx;

    u1ChannelBak = p->channel;

    for (u1ChannelIdx = CHANNEL_A; u1ChannelIdx < p->support_channel_num; u1ChannelIdx++)
    {
        p->channel = u1ChannelIdx;

        u4value = u4IO32Read4B(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBERR_RK0_R)) & (0xf << 24);
        if (u4value)
        {
            mcSHOW_DBG_MSG2(("\n[RXFIFODebugStatus] CH_%d MISC_STBERR_RK0_R_RX_ARDQ = 0x\033[1;36m%x\033[m for Gating error information\n", u1ChannelIdx, u4value));
        }
    }
    p->channel = u1ChannelBak;
    vSetPHY2ChannelMapping(p, u1ChannelBak);
#endif
}
#endif  //#if __ETT__
#endif

#endif /// __A60868_TO_BE_PORTING__
///TODO: wait for porting +++
