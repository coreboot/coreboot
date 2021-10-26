/* SPDX-License-Identifier: BSD-3-Clause */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "dramc_common.h"
#include "x_hal_io.h"
#include "dramc_actiming.h"
#include "dramc_int_global.h"
#include "dramc_dv_init.h"

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
U32 u4Datlat = 0;

static const ACTime_T_LP4 ACTimingTbl_LP4[AC_TIMING_NUMBER_LP4];

static U8 u1GetACTimingIdx(DRAMC_CTX_T *p)
{
    U8 u1TimingIdx = 0xff, u1TmpIdx;
    U8 u1TmpDramType = p->dram_type;

    {
        if (u1TmpDramType == TYPE_LPDDR4X || u1TmpDramType == TYPE_LPDDR4P)
            u1TmpDramType = TYPE_LPDDR4;
    }

    {
        for (u1TmpIdx = 0; u1TmpIdx < AC_TIMING_NUMBER_LP4; u1TmpIdx++)
        {
            if ((ACTimingTbl_LP4[u1TmpIdx].dramType == u1TmpDramType) &&

                (ACTimingTbl_LP4[u1TmpIdx].freq == p->freqGroup) &&
                (ACTimingTbl_LP4[u1TmpIdx].readDBI == p->DBI_R_onoff[p->dram_fsp]) &&
                (ACTimingTbl_LP4[u1TmpIdx].DivMode == vGet_Div_Mode(p)) &&
                (ACTimingTbl_LP4[u1TmpIdx].cbtMode == vGet_Dram_CBT_Mode(p))
               )
            {
                u1TimingIdx = u1TmpIdx;
                //mcDUMP_REG_MSG(("match AC timing %d\n", u1TimingIdx));

                mcSHOW_DBG_MSG2(("match AC timing %d\n", u1TimingIdx));
                mcSHOW_DBG_MSG2(("dramType %d, freq %d, readDBI %d, DivMode %d, cbtMode %d\n", u1TmpDramType, p->freqGroup, p->DBI_R_onoff[p->dram_fsp], vGet_Div_Mode(p), vGet_Dram_CBT_Mode(p)));
                break;
            }
        }
    }

    return u1TimingIdx;
}

static U8 u1GetDQSIEN_p2s_latency(U8 p2s_ratio)
{
    U8 ser_latency = 0;

    switch(p2s_ratio)
    {
        case 16:
            ser_latency = 18;
            break;
        case 8:
            ser_latency = 8;
            break;
        case 4:
            ser_latency = 4;
            break;
        case 2:
            ser_latency = 2;
            break;
        default:
            mcSHOW_ERR_MSG(("Error: %d is unexpected p2s ratio \n", p2s_ratio));
    }

    return ser_latency;
}

static U8 u1GetDQ_CA_p2s_latency(U8 p2s_ratio, U8 frate)
{
    U8 ser_latency = 0;

    if(((p2s_ratio == 2) && (frate == 0)) || (frate > 1))
    {
        mcSHOW_ERR_MSG(("Error: when p2s ratio is 2, frate should be 1. But p2s ratio is %d, frate is %d\n", p2s_ratio, frate));
    }

    switch(p2s_ratio)
    {
        case 16:
            ser_latency = 9;
            break;
        case 8:
            ser_latency = 5;
            break;
        case 4:
            ser_latency = 3;
            break;
        case 2:
            ser_latency = 1;
            break;
        default:
            mcSHOW_ERR_MSG(("Error: %d is unexpected p2s ratio \n", p2s_ratio));
    }

    return ser_latency;
}

static U32 A_div_B_Round(U32 A, U32 B)
{
    U32 result;

    if (B == 0)
    {
        return 0xffff;
    }

    result = A/B;

    if ((A - result*B) >= ((result + 1)*B - A))
    {
        return (result + 1);
    }
    else
    {
        return result;
    }
}

static void DramcCalculate_Datlat_val(DRAMC_CTX_T *p)
{
    U32 u4TxPipeline = 0, u4RxPipeline = 0;
    U32 u4Datlat_dsel = 0, u4Datlat_margin = 1, u4RDSEL_Offset = 2;
    U32 u4DQ_P2S_Ratio = A_D->DQ_P2S_RATIO, u4CA_p2s_ratio = 0, u4CKR = A_D->CKR;
    U32 u4CAdefault_delay = 1, u4CS2RL_start = 0, u4tRPRE_toggle = 0;
    U32 u4DQSIEN_ser_latency = 0, u4CA_ser_latency = 0;
    U32 u4DQ_ui_unit = 0, u4CA_ui_unit = 0, u4Dram_ui_ratio = 2, u4MCK_unit = 0;
    U32 u4RL[2] = {0}, u4RLMax = 0, u4DQ_2_1stDVI4CK = 0, u4CA_MCKIO_ui_unit = 0;
    U32 u4tDQSCK_Max = 0;
    U32 u4RX_rdcmdout2rdcmdbus_by_ps = 0;

    u4DQ_ui_unit =  (1000000 / (DDRPhyGetRealFreq(p) * 2));
    u4CA_ui_unit = u4CKR * u4DQ_ui_unit;

    if (u1IsLP4Family(p->dram_type))
    {
        u4CS2RL_start = 7;
        u4tRPRE_toggle = 0;
        u4tDQSCK_Max = 3500;
        u4RL[0] = Get_RL_by_MR_LP4(p->dram_cbt_mode[RANK_0], 0, LP4_DRAM_INIT_RLWL_MRfield_config(p->frequency * 2));
        u4RL[1] = Get_RL_by_MR_LP4(p->dram_cbt_mode[RANK_1], 0, LP4_DRAM_INIT_RLWL_MRfield_config(p->frequency * 2));
        u4RLMax = (u4RL[0] > u4RL[1]) ? u4RL[0] : u4RL[1];
    }
    else
    {
        mcSHOW_ERR_MSG(("Error DramType[%d] !!!\n", p->dram_type));
        #if __ETT__
        while (1);
        #endif
    }

    if (u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_MISC_TX_PIPE_CTRL), SHU_MISC_TX_PIPE_CTRL_TX_PIPE_BYPASS_EN))
    {
        u4TxPipeline = 1;
    }
    else
    {
        u4TxPipeline = 0;
    }

    if (u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_MISC_RX_PIPE_CTRL), SHU_MISC_RX_PIPE_CTRL_RX_PIPE_BYPASS_EN))
    {
        u4RxPipeline = 0;
    }
    else
    {
        u4RxPipeline = 1;
    }

    u4DQ_2_1stDVI4CK = 5 * u4DQ_ui_unit * u4Dram_ui_ratio;
    u4RDSEL_Offset = 2 + 2 *(u4DQ_P2S_Ratio == 4);
    u4MCK_unit = u4DQ_ui_unit * u4DQ_P2S_Ratio;
    u4CA_p2s_ratio = u4DQ_P2S_Ratio / u4CKR;
    u4DQSIEN_ser_latency = u1GetDQSIEN_p2s_latency(u4DQ_P2S_Ratio);
    u4CA_ser_latency = u1GetDQ_CA_p2s_latency(u4CA_p2s_ratio, A_D->CA_FULL_RATE);
    u4CA_MCKIO_ui_unit = u4DQ_ui_unit * u4CKR / (A_D->CA_FULL_RATE + 1);
    u4RX_rdcmdout2rdcmdbus_by_ps = 3 * u4MCK_unit + u4CAdefault_delay * u4CA_ui_unit + u4CA_ser_latency * u4CA_MCKIO_ui_unit /*+ RX_C->ca_default_PI * RX_C->ca_MCKIO_ps / RX_C->ca_ui_pi_ratio*/ ;
    u4Datlat_dsel = A_div_B_Round((u4RX_rdcmdout2rdcmdbus_by_ps + (u4CS2RL_start + u4RLMax * 2 )* u4CKR * u4DQ_ui_unit + u4tDQSCK_Max + u4DQ_2_1stDVI4CK), u4MCK_unit) - u4RDSEL_Offset - u4TxPipeline + u4Datlat_margin;
    u4Datlat = u4Datlat_dsel + u4TxPipeline + u4RxPipeline - 1;

    mcSHOW_DBG_MSG(("Calculate Datlat value is %d on freq %d\n", u4Datlat, p->frequency));
}


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
            mcSHOW_ERR_MSG(("[WDQSMode2AcTimingEnlarge] frequency err!\n"));
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
    DRAM_ODT_MODE_T r2w_odt_onoff = p->odt_onoff;


#ifdef XRTR2R_PERFORM_ENHANCE_DQSG_RX_DLY
    U8 u1RANKINCTL = 0;
#endif
    U8 RODT_TRACKING_SAVEING_MCK = 0, u1ROOT = 0, u1TXRANKINCTL = 0, u1TXDLY = 0, u1DATLAT_DSEL = 0;

#if SAMSUNG_LP4_NWR_WORKAROUND
    U8 u1TWTR = 0, u1TWTR_05T = 0, u1TWTR_TMP = 0;
    U8 u1MCKtoTck = 0, u105TtoTck = 0;
#endif

    U8 u1TREFBW = 0;
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

#if ENABLE_DATLAT_BY_FORMULA
    DramcCalculate_Datlat_val(p);
#else
    u4Datlat = ACTblFinal.datlat;
#endif


#if ENABLE_TX_WDQS
    r2w_odt_onoff = ODT_ON;
#else
    r2w_odt_onoff = p->odt_onoff;
#endif

    if (r2w_odt_onoff == ODT_ON)
    {
        u2TRTW = ACTblFinal.trtw_odt_on;
        u2TRTW_05T = ACTblFinal.trtw_odt_on_05T;
        u2XRTW2R = ACTblFinal.xrtw2r_odt_on;
        u2XRTR2W = ACTblFinal.xrtr2w_odt_on;
    }
    else
    {
        u2TRTW = ACTblFinal.trtw_odt_off;
        u2TRTW_05T = ACTblFinal.trtw_odt_off_05T;
        u2XRTW2R = ACTblFinal.xrtw2r_odt_off;
        u2XRTR2W = ACTblFinal.xrtr2w_odt_off;
    }


    if ((p->dram_type == TYPE_LPDDR4P) && (p->vendor_id == VENDOR_HYNIX))
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

    u1ODT_ON = p->odt_onoff;
    u1RODT_TRACK = ENABLE_RODT_TRACKING;
    u1ROEN = u1WDQS_ON | u1ODT_ON;
    u1ModeSel = u1RODT_TRACK & u1ROEN;


    //if (u1RODT_TRACK && (u1ROEN==1))
    //    RODT_TRACKING_SAVEING_MCK = 1;
#endif

#if (ENABLE_RODT_TRACKING || defined(XRTR2W_PERFORM_ENHANCE_RODTEN))
    RODT_TRACKING_SAVEING_MCK = 0;
#endif

    u2TRTW = u2TRTW - RODT_TRACKING_SAVEING_MCK;
    u2XRTR2W = u2XRTR2W - RODT_TRACKING_SAVEING_MCK;

#if SAMSUNG_LP4_NWR_WORKAROUND

    if ((p->vendor_id == VENDOR_SAMSUNG) && (p->frequency <= 800))
    {
        if (vGet_Div_Mode(p) == DIV8_MODE)
        {
            u1MCKtoTck = 4;
            u105TtoTck = 2;
        }
        else
        {
            u1MCKtoTck = 2;
            u105TtoTck= 1;
        }

        u1TWTR_TMP = (ACTblFinal.twtr * u1MCKtoTck - ACTblFinal.twtr_05T * u105TtoTck) + 2;
        if ((u1TWTR_TMP % u1MCKtoTck) == 0)
        {
            u1TWTR = u1TWTR_TMP/u1MCKtoTck;
            u1TWTR_05T = 0;
        }
        else
        {
            u1TWTR = (u1TWTR_TMP + u105TtoTck)/u1MCKtoTck;
            u1TWTR_05T = 1;
        }

        ACTblFinal.twtr = u1TWTR;
        ACTblFinal.twtr_05T = u1TWTR_05T;
    }
#endif


    if (u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_MISC_RX_PIPE_CTRL), SHU_MISC_RX_PIPE_CTRL_RX_PIPE_BYPASS_EN))
        u1DATLAT_DSEL = u4Datlat;
    else
        u1DATLAT_DSEL = u4Datlat - 1;

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
                                                | P_Fld(u2TRTW, SHU_ACTIM2_TR2W)
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
//    vIO32WriteFldMulti_All(DRAMC_REG_SHU_HWSET_VRCG, P_Fld(ACTblFinal.vrcgdis_prdcnt, SHU_HWSET_VRCG_VRCGDIS_PRDCNT)
//                                                | P_Fld(ACTblFinal.hwset_vrcg_op, SHU_HWSET_VRCG_HWSET_VRCG_OP));

    //vIO32WriteFldAlign_All(DRAMC_REG_SHU_HWSET_MR2, ACTblFinal.hwset_mr2_op, SHU_HWSET_MR2_HWSET_MR2_OP);
    //vIO32WriteFldAlign_All(DRAMC_REG_SHU_HWSET_MR13, ACTblFinal.hwset_mr13_op, SHU_HWSET_MR13_HWSET_MR13_OP);


    vIO32WriteFldMulti_All(DRAMC_REG_SHU_AC_TIME_05T, P_Fld(ACTblFinal.twtr_05T, SHU_AC_TIME_05T_TWTR_M05T)
                                                    | P_Fld(u2TRTW_05T, SHU_AC_TIME_05T_TR2W_05T)
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
        vIO32WriteFldAlign_All(DRAMC_REG_SHU_AC_DERATING0, 0x1, SHU_AC_DERATING0_ACDERATEEN);
    }
#endif


    vSetRank(p, RANK_0);
    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SHU_RK_DQSCTL, ACTblFinal.dqsinctl, MISC_SHU_RK_DQSCTL_DQSINCTL);
    vSetRank(p, RANK_1);
    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SHU_RK_DQSCTL, ACTblFinal.dqsinctl, MISC_SHU_RK_DQSCTL_DQSINCTL);
    vSetRank(p, backup_rank);
    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SHU_ODTCTRL, ACTblFinal.dqsinctl, MISC_SHU_ODTCTRL_RODT_LAT);

    if (ACTblFinal.dqsinctl >= 2)
    {
        u4RankINCTL_ROOT = ACTblFinal.dqsinctl - 2;
    }
    else
    {
        mcSHOW_ERR_MSG(("u4RankINCTL_ROOT <2, Please check\n"));
        u4RankINCTL_ROOT = 0;
    }

    vIO32WriteFldMulti_All(DDRPHY_REG_MISC_SHU_RANKCTL, P_Fld(ACTblFinal.dqsinctl, MISC_SHU_RANKCTL_RANKINCTL_PHY)
                                                       | P_Fld(u4RankINCTL_ROOT, MISC_SHU_RANKCTL_RANKINCTL_ROOT1)
                                                       | P_Fld(u4RankINCTL_ROOT, MISC_SHU_RANKCTL_RANKINCTL));

#if XRTRTR_NEW_CROSS_RK_MODE
    u2PHSINCTL = (ACTblFinal.dqsinctl == 0)? 0: (ACTblFinal.dqsinctl - 1);
    vIO32WriteFldAlign_All(DDRPHY_REG_SHU_MISC_RANK_SEL_STB, u2PHSINCTL, SHU_MISC_RANK_SEL_STB_RANK_SEL_PHSINCTL);
#endif


    vIO32WriteFldMulti_All(DDRPHY_REG_MISC_SHU_RDAT, P_Fld(u4Datlat, MISC_SHU_RDAT_DATLAT)
                                            | P_Fld(u1DATLAT_DSEL, MISC_SHU_RDAT_DATLAT_DSEL)
                                            | P_Fld(u1DATLAT_DSEL, MISC_SHU_RDAT_DATLAT_DSEL_PHY));

    vIO32WriteFldAlign_All(DRAMC_REG_SHU_ACTIMING_CONF, u1TREFBW, SHU_ACTIMING_CONF_REFBW_FR);


#ifdef XRTR2R_PERFORM_ENHANCE_DQSG_RX_DLY
    u1RANKINCTL = u4IO32ReadFldAlign(DDRPHY_REG_MISC_SHU_RANKCTL, MISC_SHU_RANKCTL_RANKINCTL);
    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SHU_RANKCTL, u1RANKINCTL, MISC_SHU_RANKCTL_RANKINCTL_RXDLY);
#endif


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

    mcSHOW_DBG_MSG4(("[UpdateACTiming]\n"));


    u1TimingIdx = u1GetACTimingIdx(p);


    if (u1TimingIdx == 0xff)
    {
        #if 0
        if (u1TmpDramType = TYPE_LPDDR4)
            u1TimingIdx = 0;
        else
            u1TimingIdx = 6;
        mcSHOW_ERR_MSG(("Error, no match AC timing, use default timing %d\n", u1TimingIdx));
        #else
        mcSHOW_ERR_MSG(("Error, no match AC timing, not apply table\n"));
        return DRAM_FAIL;
        #endif
    }


    {
        DdrUpdateACTimingReg_LP4(p, &ACTimingTbl_LP4[u1TimingIdx]);
    }

    return DRAM_OK;
}


void vDramcACTimingOptimize(DRAMC_CTX_T *p)
{

    U8 u1RFCabGrpIdx = 0, u1FreqGrpIdx = 0, u1ExecuteOptimize = ENABLE;
    U8 u1TRFC=101, u1TRFC_05T=0, u1TRFCpb=44, u1TRFCpb_05T=0,u1TXREFCNT=118;
    typedef struct
    {
        U8 u1TRFC      : 8;
        U8 u1TRFRC_05T : 1;
        U8 u1TRFCpb      : 8;
        U8 u1TRFRCpb_05T : 1;
        U16 u2TXREFCNT : 10;
    } optimizeACTime;

    enum tRFCABIdx{tRFCAB_130 = 0, tRFCAB_180, tRFCAB_280, tRFCAB_380, tRFCAB_NUM};
    enum ACTimeIdx{GRP_DDR1200_ACTIM, GRP_DDR1600_ACTIM, GRP_DDR1866_ACTIM, GRP_DDR2400_ACTIM, GRP_DDR2667_ACTIM, GRP_DDR3200_ACTIM, GRP_DDR3733_ACTIM, GRP_DDR4266_ACTIM, GRP_ACTIM_NUM};
    enum ACTimeIdxDiv4{
#if ENABLE_DDR400_OPEN_LOOP_MODE_OPTION
        GRP_DDR400_DIV4_ACTIM = 0,
#endif
        GRP_DDR800_DIV4_ACTIM, GRP_DDR1200_DIV4_ACTIM, GRP_DDR1600_DIV4_ACTIM, GRP_ACTIM_NUM_DIV4};


    optimizeACTime *ptRFCab_Opt;

    optimizeACTime tRFCab_Opt [GRP_ACTIM_NUM][tRFCAB_NUM] =
    {

        {{.u1TRFC = 8, .u1TRFRC_05T = 0, .u1TRFCpb = 0, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 21},
         {.u1TRFC = 15, .u1TRFRC_05T = 1, .u1TRFCpb = 2, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 29},
         {.u1TRFC = 30, .u1TRFRC_05T = 1, .u1TRFCpb = 9, .u1TRFRCpb_05T = 1, .u2TXREFCNT = 44},
         {.u1TRFC = 45, .u1TRFRC_05T = 1, .u1TRFCpb = 17, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 59}},

        {{.u1TRFC = 14, .u1TRFRC_05T = 0, .u1TRFCpb = 0, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 28},
         {.u1TRFC = 24, .u1TRFRC_05T = 0, .u1TRFCpb = 6, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 38},
         {.u1TRFC = 44, .u1TRFRC_05T = 0, .u1TRFCpb = 16, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 58},
         {.u1TRFC = 64, .u1TRFRC_05T = 0, .u1TRFCpb = 26, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 78}},

        {{.u1TRFC = 18, .u1TRFRC_05T = 1, .u1TRFCpb = 2, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 33},
         {.u1TRFC = 30, .u1TRFRC_05T = 0, .u1TRFCpb = 9, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 44},
         {.u1TRFC = 53, .u1TRFRC_05T = 1, .u1TRFCpb = 21, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 68},
         {.u1TRFC = 77, .u1TRFRC_05T = 0, .u1TRFCpb = 32, .u1TRFRCpb_05T = 1, .u2TXREFCNT = 91}},

        {{.u1TRFC = 27, .u1TRFRC_05T = 1,  .u1TRFCpb = 6, .u1TRFRCpb_05T = 1, .u2TXREFCNT = 42},
         {.u1TRFC = 42, .u1TRFRC_05T = 1,  .u1TRFCpb = 15, .u1TRFRCpb_05T = 1, .u2TXREFCNT = 57},
         {.u1TRFC = 72, .u1TRFRC_05T = 1,  .u1TRFCpb = 30, .u1TRFRCpb_05T = 1, .u2TXREFCNT = 87},
         {.u1TRFC = 102, .u1TRFRC_05T = 1,  .u1TRFCpb = 45, .u1TRFRCpb_05T = 1, .u2TXREFCNT = 117}},

        {{.u1TRFC = 31, .u1TRFRC_05T = 1,  .u1TRFCpb = 8, .u1TRFRCpb_05T = 1, .u2TXREFCNT = 46},
         {.u1TRFC = 48, .u1TRFRC_05T = 1,  .u1TRFCpb = 18, .u1TRFRCpb_05T = 1, .u2TXREFCNT = 63},
         {.u1TRFC = 81, .u1TRFRC_05T = 1,  .u1TRFCpb = 35, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 96},
         {.u1TRFC = 115, .u1TRFRC_05T = 0,  .u1TRFCpb = 51, .u1TRFRCpb_05T = 1, .u2TXREFCNT = 130}},

       {{.u1TRFC = 40, .u1TRFRC_05T = 0,  .u1TRFCpb = 12, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 55},
         {.u1TRFC = 60, .u1TRFRC_05T = 0,  .u1TRFCpb = 24, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 75},
         {.u1TRFC = 100, .u1TRFRC_05T = 0, .u1TRFCpb = 44, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 115},
         {.u1TRFC = 140, .u1TRFRC_05T = 0, .u1TRFCpb = 64, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 155}},

        {{.u1TRFC = 49, .u1TRFRC_05T = 0, .u1TRFCpb = 16, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 65},
         {.u1TRFC = 72, .u1TRFRC_05T = 0, .u1TRFCpb = 30, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 88},
         {.u1TRFC = 119, .u1TRFRC_05T = 0, .u1TRFCpb = 53, .u1TRFRCpb_05T = 1, .u2TXREFCNT = 135},
         {.u1TRFC = 165, .u1TRFRC_05T = 1, .u1TRFCpb = 77, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 181}},

        {{.u1TRFC = 57, .u1TRFRC_05T = 1, .u1TRFCpb = 20, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 74},
         {.u1TRFC = 84, .u1TRFRC_05T = 0, .u1TRFCpb = 36, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 100},
         {.u1TRFC = 137, .u1TRFRC_05T = 1, .u1TRFCpb = 63, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 154},
         {.u1TRFC = 191, .u1TRFRC_05T = 0, .u1TRFCpb = 89, .u1TRFRCpb_05T = 1, .u2TXREFCNT = 207}}
    };

    optimizeACTime tRFCab_Opt_Div4 [GRP_ACTIM_NUM_DIV4][tRFCAB_NUM] =
    {
#if ENABLE_DDR400_OPEN_LOOP_MODE_OPTION

        {{.u1TRFC = 1,  .u1TRFRC_05T = 0, .u1TRFCpb = 0, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 14},
         {.u1TRFC = 6,  .u1TRFRC_05T = 0, .u1TRFCpb = 0, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 19},
         {.u1TRFC = 16, .u1TRFRC_05T = 0, .u1TRFCpb = 2, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 29},
         {.u1TRFC = 26, .u1TRFRC_05T = 0, .u1TRFCpb = 7, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 39}},
#endif

        {{.u1TRFC = 14, .u1TRFRC_05T = 0, .u1TRFCpb = 0, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 28},
         {.u1TRFC = 24, .u1TRFRC_05T = 0, .u1TRFCpb = 6, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 38},
         {.u1TRFC = 44, .u1TRFRC_05T = 0, .u1TRFCpb = 16, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 58},
         {.u1TRFC = 64, .u1TRFRC_05T = 0, .u1TRFCpb = 26, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 78}},

        {{.u1TRFC = 28, .u1TRFRC_05T = 0,  .u1TRFCpb = 7, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 42},
         {.u1TRFC = 43, .u1TRFRC_05T = 0,  .u1TRFCpb = 16, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 57},
         {.u1TRFC = 73, .u1TRFRC_05T = 0,  .u1TRFCpb = 31, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 87},
         {.u1TRFC = 103, .u1TRFRC_05T = 0,  .u1TRFCpb = 46, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 117}},

       {{.u1TRFC = 40, .u1TRFRC_05T = 0,  .u1TRFCpb = 12, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 55},
         {.u1TRFC = 60, .u1TRFRC_05T = 0,  .u1TRFCpb = 24, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 75},
         {.u1TRFC = 100, .u1TRFRC_05T = 0, .u1TRFCpb = 44, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 115},
         {.u1TRFC = 140, .u1TRFRC_05T = 0, .u1TRFCpb = 64, .u1TRFRCpb_05T = 0, .u2TXREFCNT = 155}},
    };


    mcSHOW_DBG_MSG2(("[ACTimingOptimize]"));

#if __ETT__
    if (p->density==0xff)
    {
        mcSHOW_ERR_MSG(("Error : No call MR8 to get density!!\n"));
        while(1);
    }
#endif

    switch (p->density)
    {
        case 0x0:
            u1RFCabGrpIdx = tRFCAB_130;
            break;
        case 0x1:
        case 0x2:
            u1RFCabGrpIdx = tRFCAB_180;
            break;
        case 0x3:
        case 0x4:
            u1RFCabGrpIdx = tRFCAB_280;
            break;
        case 0x5:
        case 0x6:
            u1RFCabGrpIdx = tRFCAB_380;
            break;
        default:
            u1ExecuteOptimize = DISABLE;
            mcSHOW_ERR_MSG(("MR8 density err!\n"));
    }

    switch (p->freqGroup)
    {
#if ENABLE_DDR400_OPEN_LOOP_MODE_OPTION
        case 200:
            if (vGet_Div_Mode(p) == DIV4_MODE)
            {
                u1FreqGrpIdx = GRP_DDR400_DIV4_ACTIM;
            }
            else
            {
                u1ExecuteOptimize = DISABLE;
                mcSHOW_ERR_MSG(("freqGroup err!\n"));
                #if __ETT__
                while(1);
                #endif
            }
            break;
#endif
        case 400:
            if (vGet_Div_Mode(p) == DIV4_MODE)
                u1FreqGrpIdx = GRP_DDR800_DIV4_ACTIM;
            else
            {
                u1ExecuteOptimize = DISABLE;
                mcSHOW_ERR_MSG(("freqGroup err!\n"));
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
            mcSHOW_ERR_MSG(("freqGroup err!\n"));
            #if __ETT__
            while(1);
            #endif
    }

    if (vGet_Div_Mode(p) == DIV4_MODE && u1FreqGrpIdx >= GRP_ACTIM_NUM_DIV4)
    {
            u1ExecuteOptimize = DISABLE;
            mcSHOW_ERR_MSG(("freqGroup err!\n"));
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


    if (u1ExecuteOptimize == ENABLE)
    {
        vIO32WriteFldAlign_All(DRAMC_REG_SHU_ACTIM3, u1TRFC, SHU_ACTIM3_TRFC);
        vIO32WriteFldAlign_All(DRAMC_REG_SHU_AC_TIME_05T, u1TRFC_05T, SHU_AC_TIME_05T_TRFC_05T);
        vIO32WriteFldAlign_All(DRAMC_REG_SHU_ACTIM4, u1TXREFCNT, SHU_ACTIM4_TXREFCNT);
        vIO32WriteFldAlign_All(DRAMC_REG_SHU_ACTIM3, u1TRFCpb, SHU_ACTIM3_TRFCPB);
        vIO32WriteFldAlign_All(DRAMC_REG_SHU_AC_TIME_05T, u1TRFCpb_05T, SHU_AC_TIME_05T_TRFCPB_05T);

        mcSHOW_DBG_MSG2(("Density (MR8 OP[5:2]) %u, TRFC %u, TRFC_05T %u, TXREFCNT %u, TRFCpb %u, TRFCpb_05T %u\n", p->density, u1TRFC, u1TRFC_05T, u1TXREFCNT, u1TRFCpb, u1TRFCpb_05T));
    }

    return;
}

static const ACTime_T_LP4 ACTimingTbl_LP4[AC_TIMING_NUMBER_LP4] = {

#if SUPPORT_LP4_DDR4266_ACTIM

#if (ENABLE_READ_DBI == 1)

{
    .dramType = TYPE_LPDDR4, .freq = 2133, .cbtMode = CBT_NORMAL_MODE, .readDBI = 1,
    .readLat = 40,  .writeLat =  18,    .DivMode =  DIV8_MODE,

    .tras = 14, .tras_05T = 0,
    .trp = 8,   .trp_05T = 1,
    .trpab = 10,    .trpab_05T = 0,
    .trc = 23,  .trc_05T = 0,
    .trfc = 137,    .trfc_05T = 1,
    .trfcpb = 63,   .trfcpb_05T = 0,
    .txp = 1,   .txp_05T = 0,
    .trtp = 2,  .trtp_05T = 1,
    .trcd = 10, .trcd_05T = 0,
    .twr = 15,  .twr_05T = 0,
    .twtr = 10, .twtr_05T = 1,
    .tpbr2pbr = 41, .tpbr2pbr_05T = 0,
    .tpbr2act = 0,  .tpbr2act_05T = 0,
    .tr2mrw = 18,   .tr2mrw_05T = 0,
    .tw2mrw = 11,   .tw2mrw_05T = 0,
    .tmrr2mrw = 15, .tmrr2mrw_05T = 0,
    .tmrw = 6,  .tmrw_05T = 0,
    .tmrd = 8,  .tmrd_05T = 0,
    .tmrwckel = 9,  .tmrwckel_05T = 0,
    .tpde = 1,  .tpde_05T = 1,
    .tpdx = 1,  .tpdx_05T = 0,
    .tmrri = 14,    .tmrri_05T = 0,
    .trrd = 4,  .trrd_05T = 1,
    .trrd_4266 = 3, .trrd_4266_05T = 0,
    .tfaw = 13, .tfaw_05T = 1,
    .tfaw_4266 = 8, .tfaw_4266_05T = 0,
    .trtw_odt_off = 7,  .trtw_odt_off_05T = 0,
    .trtw_odt_on = 9,   .trtw_odt_on_05T = 0,
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
    .trcd_derate = 11,  .trcd_derate_05T = 0,
    .trc_derate = 26,   .trc_derate_05T = 0,
    .tras_derate = 15,  .tras_derate_05T = 0,
    .trpab_derate = 11, .trpab_derate_05T = 0,
    .trp_derate = 9,    .trp_derate_05T = 1,
    .trrd_derate = 5,   .trrd_derate_05T = 1,
    .trtpd = 15,    .trtpd_05T = 1,
    .twtpd = 18,    .twtpd_05T = 0,
    .tmrr2w_odt_off = 11,
    .tmrr2w_odt_on = 13,
    .ckeprd = 3,
    .ckelckcnt = 3,
    .zqlat2 = 16,


    .dqsinctl = 7,   .datlat = 18
},

{
    .dramType = TYPE_LPDDR4, .freq = 2133, .cbtMode = CBT_BYTE_MODE1, .readDBI = 1,
    .readLat = 44,  .writeLat =  18,    .DivMode =  DIV8_MODE,

    .tras = 14, .tras_05T = 0,
    .trp = 8,   .trp_05T = 1,
    .trpab = 10,    .trpab_05T = 0,
    .trc = 23,  .trc_05T = 0,
    .trfc = 137,    .trfc_05T = 1,
    .trfcpb = 63,   .trfcpb_05T = 0,
    .txp = 1,   .txp_05T = 0,
    .trtp = 2,  .trtp_05T = 1,
    .trcd = 10, .trcd_05T = 0,
    .twr = 16,  .twr_05T = 0,
    .twtr = 11, .twtr_05T = 1,
    .tpbr2pbr = 41, .tpbr2pbr_05T = 0,
    .tpbr2act = 0,  .tpbr2act_05T = 0,
    .tr2mrw = 19,   .tr2mrw_05T = 0,
    .tw2mrw = 11,   .tw2mrw_05T = 0,
    .tmrr2mrw = 16, .tmrr2mrw_05T = 0,
    .tmrw = 6,  .tmrw_05T = 0,
    .tmrd = 8,  .tmrd_05T = 0,
    .tmrwckel = 9,  .tmrwckel_05T = 0,
    .tpde = 1,  .tpde_05T = 1,
    .tpdx = 1,  .tpdx_05T = 0,
    .tmrri = 14,    .tmrri_05T = 0,
    .trrd = 4,  .trrd_05T = 1,
    .trrd_4266 = 3, .trrd_4266_05T = 0,
    .tfaw = 13, .tfaw_05T = 1,
    .tfaw_4266 = 8, .tfaw_4266_05T = 0,
    .trtw_odt_off = 8,  .trtw_odt_off_05T = 0,
    .trtw_odt_on = 10,  .trtw_odt_on_05T = 0,
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
    .trcd_derate = 11,  .trcd_derate_05T = 0,
    .trc_derate = 26,   .trc_derate_05T = 0,
    .tras_derate = 15,  .tras_derate_05T = 0,
    .trpab_derate = 11, .trpab_derate_05T = 0,
    .trp_derate = 9,    .trp_derate_05T = 1,
    .trrd_derate = 5,   .trrd_derate_05T = 1,
    .trtpd = 16,    .trtpd_05T = 1,
    .twtpd = 19,    .twtpd_05T = 0,
    .tmrr2w_odt_off = 12,
    .tmrr2w_odt_on = 14,
    .ckeprd = 3,
    .ckelckcnt = 3,
    .zqlat2 = 16,


    .dqsinctl = 7,   .datlat = 18
},
#else

{
    .dramType = TYPE_LPDDR4, .freq = 2133, .cbtMode = CBT_NORMAL_MODE, .readDBI = 0,
    .readLat = 36,  .writeLat =  18,    .DivMode =  DIV8_MODE,

    .tras = 14, .tras_05T = 0,
    .trp = 8,   .trp_05T = 1,
    .trpab = 10,    .trpab_05T = 0,
    .trc = 23,  .trc_05T = 0,
    .trfc = 137,    .trfc_05T = 1,
    .trfcpb = 63,   .trfcpb_05T = 0,
    .txp = 1,   .txp_05T = 0,
    .trtp = 2,  .trtp_05T = 1,
    .trcd = 10, .trcd_05T = 0,
    .twr = 15,  .twr_05T = 0,
    .twtr = 10, .twtr_05T = 1,
    .tpbr2pbr = 41, .tpbr2pbr_05T = 0,
    .tpbr2act = 0,  .tpbr2act_05T = 0,
    .tr2mrw = 17,   .tr2mrw_05T = 0,
    .tw2mrw = 11,   .tw2mrw_05T = 0,
    .tmrr2mrw = 14, .tmrr2mrw_05T = 0,
    .tmrw = 6,  .tmrw_05T = 0,
    .tmrd = 8,  .tmrd_05T = 0,
    .tmrwckel = 9,  .tmrwckel_05T = 0,
    .tpde = 1,  .tpde_05T = 1,
    .tpdx = 1,  .tpdx_05T = 0,
    .tmrri = 14,    .tmrri_05T = 0,
    .trrd = 4,  .trrd_05T = 1,
    .trrd_4266 = 3, .trrd_4266_05T = 0,
    .tfaw = 13, .tfaw_05T = 1,
    .tfaw_4266 = 8, .tfaw_4266_05T = 0,
    .trtw_odt_off = 6,  .trtw_odt_off_05T = 0,
    .trtw_odt_on = 8,   .trtw_odt_on_05T = 0,
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
    .trcd_derate = 11,  .trcd_derate_05T = 0,
    .trc_derate = 26,   .trc_derate_05T = 0,
    .tras_derate = 15,  .tras_derate_05T = 0,
    .trpab_derate = 11, .trpab_derate_05T = 0,
    .trp_derate = 9,    .trp_derate_05T = 1,
    .trrd_derate = 5,   .trrd_derate_05T = 1,
    .trtpd = 14,    .trtpd_05T = 1,
    .twtpd = 18,    .twtpd_05T = 0,
    .tmrr2w_odt_off = 10,
    .tmrr2w_odt_on = 12,
    .ckeprd = 3,
    .ckelckcnt = 3,
    .zqlat2 = 16,


    .dqsinctl = 7,   .datlat = 18
},

{
    .dramType = TYPE_LPDDR4, .freq = 2133, .cbtMode = CBT_BYTE_MODE1, .readDBI = 0,
    .readLat = 40,  .writeLat =  18,    .DivMode =  DIV8_MODE,

    .tras = 14, .tras_05T = 0,
    .trp = 8,   .trp_05T = 1,
    .trpab = 10,    .trpab_05T = 0,
    .trc = 23,  .trc_05T = 0,
    .trfc = 137,    .trfc_05T = 1,
    .trfcpb = 63,   .trfcpb_05T = 0,
    .txp = 1,   .txp_05T = 0,
    .trtp = 2,  .trtp_05T = 1,
    .trcd = 10, .trcd_05T = 0,
    .twr = 16,  .twr_05T = 0,
    .twtr = 11, .twtr_05T = 1,
    .tpbr2pbr = 41, .tpbr2pbr_05T = 0,
    .tpbr2act = 0,  .tpbr2act_05T = 0,
    .tr2mrw = 18,   .tr2mrw_05T = 0,
    .tw2mrw = 11,   .tw2mrw_05T = 0,
    .tmrr2mrw = 15, .tmrr2mrw_05T = 0,
    .tmrw = 6,  .tmrw_05T = 0,
    .tmrd = 8,  .tmrd_05T = 0,
    .tmrwckel = 9,  .tmrwckel_05T = 0,
    .tpde = 1,  .tpde_05T = 1,
    .tpdx = 1,  .tpdx_05T = 0,
    .tmrri = 14,    .tmrri_05T = 0,
    .trrd = 4,  .trrd_05T = 1,
    .trrd_4266 = 3, .trrd_4266_05T = 0,
    .tfaw = 13, .tfaw_05T = 1,
    .tfaw_4266 = 8, .tfaw_4266_05T = 0,
    .trtw_odt_off = 7,  .trtw_odt_off_05T = 0,
    .trtw_odt_on = 9,   .trtw_odt_on_05T = 0,
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
    .trcd_derate = 11,  .trcd_derate_05T = 0,
    .trc_derate = 26,   .trc_derate_05T = 0,
    .tras_derate = 15,  .tras_derate_05T = 0,
    .trpab_derate = 11, .trpab_derate_05T = 0,
    .trp_derate = 9,    .trp_derate_05T = 1,
    .trrd_derate = 5,   .trrd_derate_05T = 1,
    .trtpd = 15,    .trtpd_05T = 1,
    .twtpd = 19,    .twtpd_05T = 0,
    .tmrr2w_odt_off = 11,
    .tmrr2w_odt_on = 13,
    .ckeprd = 3,
    .ckelckcnt = 3,
    .zqlat2 = 16,


    .dqsinctl = 7,   .datlat = 18
},
#endif
#endif
#if SUPPORT_LP4_DDR3733_ACTIM

#if (ENABLE_READ_DBI == 1)

{
    .dramType = TYPE_LPDDR4, .freq = 1866, .cbtMode = CBT_NORMAL_MODE, .readDBI = 1,
    .readLat = 36,  .writeLat =  16,    .DivMode =  DIV8_MODE,

    .tras = 11, .tras_05T = 1,
    .trp = 7,   .trp_05T = 0,
    .trpab = 8, .trpab_05T = 1,
    .trc = 19,  .trc_05T = 0,
    .trfc = 119,    .trfc_05T = 0,
    .trfcpb = 53,   .trfcpb_05T = 1,
    .txp = 0,   .txp_05T = 1,
    .trtp = 2,  .trtp_05T = 0,
    .trcd = 8,  .trcd_05T = 1,
    .twr = 13,  .twr_05T = 1,
    .twtr = 8,  .twtr_05T = 0,
    .tpbr2pbr = 35, .tpbr2pbr_05T = 0,
    .tpbr2act = 0,  .tpbr2act_05T = 0,
    .tr2mrw = 16,   .tr2mrw_05T = 1,
    .tw2mrw = 10,   .tw2mrw_05T = 0,
    .tmrr2mrw = 13, .tmrr2mrw_05T = 1,
    .tmrw = 5,  .tmrw_05T = 1,
    .tmrd = 7,  .tmrd_05T = 1,
    .tmrwckel = 8,  .tmrwckel_05T = 1,
    .tpde = 1,  .tpde_05T = 1,
    .tpdx = 1,  .tpdx_05T = 0,
    .tmrri = 12,    .tmrri_05T = 0,
    .trrd = 4,  .trrd_05T = 0,
    .trrd_4266 = 2, .trrd_4266_05T = 1,
    .tfaw = 11, .tfaw_05T = 0,
    .tfaw_4266 = 6, .tfaw_4266_05T = 0,
    .trtw_odt_off = 6,  .trtw_odt_off_05T = 0,
    .trtw_odt_on = 9,   .trtw_odt_on_05T = 0,
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
    .trcd_derate = 9,   .trcd_derate_05T = 1,
    .trc_derate = 21,   .trc_derate_05T = 1,
    .tras_derate = 12,  .tras_derate_05T = 0,
    .trpab_derate = 9,  .trpab_derate_05T = 1,
    .trp_derate = 8,    .trp_derate_05T = 0,
    .trrd_derate = 5,   .trrd_derate_05T = 0,
    .trtpd = 14,    .trtpd_05T = 0,
    .twtpd = 16,    .twtpd_05T = 1,
    .tmrr2w_odt_off = 10,
    .tmrr2w_odt_on = 12,
    .ckeprd = 3,
    .ckelckcnt = 3,
    .zqlat2 = 14,


    .dqsinctl = 6,   .datlat = 16
},

{
    .dramType = TYPE_LPDDR4, .freq = 1866, .cbtMode = CBT_BYTE_MODE1, .readDBI = 1,
    .readLat = 40,  .writeLat =  16,    .DivMode =  DIV8_MODE,

    .tras = 11, .tras_05T = 1,
    .trp = 7,   .trp_05T = 0,
    .trpab = 8, .trpab_05T = 1,
    .trc = 19,  .trc_05T = 0,
    .trfc = 119,    .trfc_05T = 0,
    .trfcpb = 53,   .trfcpb_05T = 1,
    .txp = 0,   .txp_05T = 1,
    .trtp = 2,  .trtp_05T = 0,
    .trcd = 8,  .trcd_05T = 1,
    .twr = 14,  .twr_05T = 1,
    .twtr = 9,  .twtr_05T = 0,
    .tpbr2pbr = 35, .tpbr2pbr_05T = 0,
    .tpbr2act = 0,  .tpbr2act_05T = 0,
    .tr2mrw = 17,   .tr2mrw_05T = 1,
    .tw2mrw = 10,   .tw2mrw_05T = 0,
    .tmrr2mrw = 14, .tmrr2mrw_05T = 1,
    .tmrw = 5,  .tmrw_05T = 1,
    .tmrd = 7,  .tmrd_05T = 1,
    .tmrwckel = 8,  .tmrwckel_05T = 1,
    .tpde = 1,  .tpde_05T = 1,
    .tpdx = 1,  .tpdx_05T = 0,
    .tmrri = 12,    .tmrri_05T = 0,
    .trrd = 4,  .trrd_05T = 0,
    .trrd_4266 = 2, .trrd_4266_05T = 1,
    .tfaw = 11, .tfaw_05T = 0,
    .tfaw_4266 = 6, .tfaw_4266_05T = 0,
    .trtw_odt_off = 7,  .trtw_odt_off_05T = 0,
    .trtw_odt_on = 10,  .trtw_odt_on_05T = 0,
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
    .trcd_derate = 9,   .trcd_derate_05T = 1,
    .trc_derate = 21,   .trc_derate_05T = 1,
    .tras_derate = 12,  .tras_derate_05T = 0,
    .trpab_derate = 9,  .trpab_derate_05T = 1,
    .trp_derate = 8,    .trp_derate_05T = 0,
    .trrd_derate = 5,   .trrd_derate_05T = 0,
    .trtpd = 15,    .trtpd_05T = 0,
    .twtpd = 17,    .twtpd_05T = 1,
    .tmrr2w_odt_off = 11,
    .tmrr2w_odt_on = 13,
    .ckeprd = 3,
    .ckelckcnt = 3,
    .zqlat2 = 14,


    .dqsinctl = 6,   .datlat = 16
},
#else

{
    .dramType = TYPE_LPDDR4, .freq = 1866, .cbtMode = CBT_NORMAL_MODE, .readDBI = 0,
    .readLat = 32,  .writeLat =  16,    .DivMode =  DIV8_MODE,

    .tras = 11, .tras_05T = 1,
    .trp = 7,   .trp_05T = 0,
    .trpab = 8, .trpab_05T = 1,
    .trc = 19,  .trc_05T = 0,
    .trfc = 119,    .trfc_05T = 0,
    .trfcpb = 53,   .trfcpb_05T = 1,
    .txp = 0,   .txp_05T = 1,
    .trtp = 2,  .trtp_05T = 0,
    .trcd = 8,  .trcd_05T = 1,
    .twr = 13,  .twr_05T = 1,
    .twtr = 8,  .twtr_05T = 0,
    .tpbr2pbr = 35, .tpbr2pbr_05T = 0,
    .tpbr2act = 0,  .tpbr2act_05T = 0,
    .tr2mrw = 15,   .tr2mrw_05T = 1,
    .tw2mrw = 10,   .tw2mrw_05T = 0,
    .tmrr2mrw = 12, .tmrr2mrw_05T = 1,
    .tmrw = 5,  .tmrw_05T = 1,
    .tmrd = 7,  .tmrd_05T = 1,
    .tmrwckel = 8,  .tmrwckel_05T = 1,
    .tpde = 1,  .tpde_05T = 1,
    .tpdx = 1,  .tpdx_05T = 0,
    .tmrri = 12,    .tmrri_05T = 0,
    .trrd = 4,  .trrd_05T = 0,
    .trrd_4266 = 2, .trrd_4266_05T = 1,
    .tfaw = 11, .tfaw_05T = 0,
    .tfaw_4266 = 6, .tfaw_4266_05T = 0,
    .trtw_odt_off = 5,  .trtw_odt_off_05T = 0,
    .trtw_odt_on = 8,   .trtw_odt_on_05T = 0,
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
    .trcd_derate = 9,   .trcd_derate_05T = 1,
    .trc_derate = 21,   .trc_derate_05T = 1,
    .tras_derate = 12,  .tras_derate_05T = 0,
    .trpab_derate = 9,  .trpab_derate_05T = 1,
    .trp_derate = 8,    .trp_derate_05T = 0,
    .trrd_derate = 5,   .trrd_derate_05T = 0,
    .trtpd = 13,    .trtpd_05T = 0,
    .twtpd = 16,    .twtpd_05T = 1,
    .tmrr2w_odt_off = 9,
    .tmrr2w_odt_on = 11,
    .ckeprd = 3,
    .ckelckcnt = 3,
    .zqlat2 = 14,


    .dqsinctl = 6,   .datlat = 16
},

{
    .dramType = TYPE_LPDDR4, .freq = 1866, .cbtMode = CBT_BYTE_MODE1, .readDBI = 0,
    .readLat = 36,  .writeLat =  16,    .DivMode =  DIV8_MODE,

    .tras = 11, .tras_05T = 1,
    .trp = 7,   .trp_05T = 0,
    .trpab = 8, .trpab_05T = 1,
    .trc = 19,  .trc_05T = 0,
    .trfc = 119,    .trfc_05T = 0,
    .trfcpb = 53,   .trfcpb_05T = 1,
    .txp = 0,   .txp_05T = 1,
    .trtp = 2,  .trtp_05T = 0,
    .trcd = 8,  .trcd_05T = 1,
    .twr = 14,  .twr_05T = 1,
    .twtr = 9,  .twtr_05T = 0,
    .tpbr2pbr = 35, .tpbr2pbr_05T = 0,
    .tpbr2act = 0,  .tpbr2act_05T = 0,
    .tr2mrw = 16,   .tr2mrw_05T = 1,
    .tw2mrw = 10,   .tw2mrw_05T = 0,
    .tmrr2mrw = 13, .tmrr2mrw_05T = 1,
    .tmrw = 5,  .tmrw_05T = 1,
    .tmrd = 7,  .tmrd_05T = 1,
    .tmrwckel = 8,  .tmrwckel_05T = 1,
    .tpde = 1,  .tpde_05T = 1,
    .tpdx = 1,  .tpdx_05T = 0,
    .tmrri = 12,    .tmrri_05T = 0,
    .trrd = 4,  .trrd_05T = 0,
    .trrd_4266 = 2, .trrd_4266_05T = 1,
    .tfaw = 11, .tfaw_05T = 0,
    .tfaw_4266 = 6, .tfaw_4266_05T = 0,
    .trtw_odt_off = 6,  .trtw_odt_off_05T = 0,
    .trtw_odt_on = 9,   .trtw_odt_on_05T = 0,
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
    .trcd_derate = 9,   .trcd_derate_05T = 1,
    .trc_derate = 21,   .trc_derate_05T = 1,
    .tras_derate = 12,  .tras_derate_05T = 0,
    .trpab_derate = 9,  .trpab_derate_05T = 1,
    .trp_derate = 8,    .trp_derate_05T = 0,
    .trrd_derate = 5,   .trrd_derate_05T = 0,
    .trtpd = 14,    .trtpd_05T = 0,
    .twtpd = 17,    .twtpd_05T = 1,
    .tmrr2w_odt_off = 10,
    .tmrr2w_odt_on = 12,
    .ckeprd = 3,
    .ckelckcnt = 3,
    .zqlat2 = 14,


    .dqsinctl = 6,   .datlat = 16
},
#endif
#endif
#if SUPPORT_LP4_DDR3200_ACTIM

#if (ENABLE_READ_DBI == 1)

{
    .dramType = TYPE_LPDDR4, .freq = 1600, .cbtMode = CBT_NORMAL_MODE, .readDBI = 1,
    .readLat = 32,  .writeLat =  14,    .DivMode =  DIV8_MODE,

    .tras = 8,  .tras_05T = 1,
    .trp = 6,   .trp_05T = 0,
    .trpab = 7, .trpab_05T = 0,
    .trc = 15,  .trc_05T = 0,
    .trfc = 100,    .trfc_05T = 0,
    .trfcpb = 44,   .trfcpb_05T = 0,
    .txp = 0,   .txp_05T = 0,
    .trtp = 1,  .trtp_05T = 1,
    .trcd = 7,  .trcd_05T = 1,
    .twr = 12,  .twr_05T = 1,
    .twtr = 7,  .twtr_05T = 0,
    .tpbr2pbr = 29, .tpbr2pbr_05T = 0,
    .tpbr2act = 0,  .tpbr2act_05T = 0,
    .tr2mrw = 14,   .tr2mrw_05T = 1,
    .tw2mrw = 9,    .tw2mrw_05T = 0,
    .tmrr2mrw = 12, .tmrr2mrw_05T = 1,
    .tmrw = 4,  .tmrw_05T = 1,
    .tmrd = 6,  .tmrd_05T = 1,
    .tmrwckel = 7,  .tmrwckel_05T = 1,
    .tpde = 1,  .tpde_05T = 1,
    .tpdx = 1,  .tpdx_05T = 0,
    .tmrri = 10,    .tmrri_05T = 1,
    .trrd = 3,  .trrd_05T = 0,
    .trrd_4266 = 2, .trrd_4266_05T = 0,
    .tfaw = 8,  .tfaw_05T = 0,
    .tfaw_4266 = 4, .tfaw_4266_05T = 0,
    .trtw_odt_off = 5,  .trtw_odt_off_05T = 0,
    .trtw_odt_on = 7,   .trtw_odt_on_05T = 0,
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
    .trcd_derate = 8,   .trcd_derate_05T = 0,
    .trc_derate = 17,   .trc_derate_05T = 0,
    .tras_derate = 9,   .tras_derate_05T = 1,
    .trpab_derate = 8,  .trpab_derate_05T = 0,
    .trp_derate = 6,    .trp_derate_05T = 1,
    .trrd_derate = 4,   .trrd_derate_05T = 0,
    .trtpd = 13,    .trtpd_05T = 0,
    .twtpd = 14,    .twtpd_05T = 1,
    .tmrr2w_odt_off = 9,
    .tmrr2w_odt_on = 11,
    .ckeprd = 2,
    .ckelckcnt = 2,
    .zqlat2 = 12,


    .dqsinctl = 5,   .datlat = 15
},

{
    .dramType = TYPE_LPDDR4, .freq = 1600, .cbtMode = CBT_BYTE_MODE1, .readDBI = 1,
    .readLat = 36,  .writeLat =  14,    .DivMode =  DIV8_MODE,

    .tras = 8,  .tras_05T = 1,
    .trp = 6,   .trp_05T = 0,
    .trpab = 7, .trpab_05T = 0,
    .trc = 15,  .trc_05T = 0,
    .trfc = 100,    .trfc_05T = 0,
    .trfcpb = 44,   .trfcpb_05T = 0,
    .txp = 0,   .txp_05T = 0,
    .trtp = 1,  .trtp_05T = 1,
    .trcd = 7,  .trcd_05T = 1,
    .twr = 12,  .twr_05T = 1,
    .twtr = 8,  .twtr_05T = 0,
    .tpbr2pbr = 29, .tpbr2pbr_05T = 0,
    .tpbr2act = 0,  .tpbr2act_05T = 0,
    .tr2mrw = 15,   .tr2mrw_05T = 1,
    .tw2mrw = 9,    .tw2mrw_05T = 0,
    .tmrr2mrw = 13, .tmrr2mrw_05T = 1,
    .tmrw = 4,  .tmrw_05T = 1,
    .tmrd = 6,  .tmrd_05T = 1,
    .tmrwckel = 7,  .tmrwckel_05T = 1,
    .tpde = 1,  .tpde_05T = 1,
    .tpdx = 1,  .tpdx_05T = 0,
    .tmrri = 10,    .tmrri_05T = 1,
    .trrd = 3,  .trrd_05T = 0,
    .trrd_4266 = 2, .trrd_4266_05T = 0,
    .tfaw = 8,  .tfaw_05T = 0,
    .tfaw_4266 = 4, .tfaw_4266_05T = 0,
    .trtw_odt_off = 6,  .trtw_odt_off_05T = 0,
    .trtw_odt_on = 8,   .trtw_odt_on_05T = 0,
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
    .trcd_derate = 8,   .trcd_derate_05T = 0,
    .trc_derate = 17,   .trc_derate_05T = 0,
    .tras_derate = 9,   .tras_derate_05T = 1,
    .trpab_derate = 8,  .trpab_derate_05T = 0,
    .trp_derate = 6,    .trp_derate_05T = 1,
    .trrd_derate = 4,   .trrd_derate_05T = 0,
    .trtpd = 14,    .trtpd_05T = 0,
    .twtpd = 15,    .twtpd_05T = 1,
    .tmrr2w_odt_off = 10,
    .tmrr2w_odt_on = 12,
    .ckeprd = 2,
    .ckelckcnt = 2,
    .zqlat2 = 12,


    .dqsinctl = 5,   .datlat = 15
},
#else

{
    .dramType = TYPE_LPDDR4, .freq = 1600, .cbtMode = CBT_NORMAL_MODE, .readDBI = 0,
    .readLat = 28,  .writeLat =  14,    .DivMode =  DIV8_MODE,

    .tras = 8,  .tras_05T = 1,
    .trp = 6,   .trp_05T = 0,
    .trpab = 7, .trpab_05T = 0,
    .trc = 15,  .trc_05T = 0,
    .trfc = 100,    .trfc_05T = 0,
    .trfcpb = 44,   .trfcpb_05T = 0,
    .txp = 0,   .txp_05T = 0,
    .trtp = 1,  .trtp_05T = 1,
    .trcd = 7,  .trcd_05T = 1,
    .twr = 12,  .twr_05T = 1,
    .twtr = 7,  .twtr_05T = 0,
    .tpbr2pbr = 29, .tpbr2pbr_05T = 0,
    .tpbr2act = 0,  .tpbr2act_05T = 0,
    .tr2mrw = 13,   .tr2mrw_05T = 1,
    .tw2mrw = 9,    .tw2mrw_05T = 0,
    .tmrr2mrw = 11, .tmrr2mrw_05T = 1,
    .tmrw = 4,  .tmrw_05T = 1,
    .tmrd = 6,  .tmrd_05T = 1,
    .tmrwckel = 7,  .tmrwckel_05T = 1,
    .tpde = 1,  .tpde_05T = 1,
    .tpdx = 1,  .tpdx_05T = 0,
    .tmrri = 10,    .tmrri_05T = 1,
    .trrd = 3,  .trrd_05T = 0,
    .trrd_4266 = 2, .trrd_4266_05T = 0,
    .tfaw = 8,  .tfaw_05T = 0,
    .tfaw_4266 = 4, .tfaw_4266_05T = 0,
    .trtw_odt_off = 4,  .trtw_odt_off_05T = 0,
    .trtw_odt_on = 6,   .trtw_odt_on_05T = 0,
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
    .trcd_derate = 8,   .trcd_derate_05T = 0,
    .trc_derate = 17,   .trc_derate_05T = 0,
    .tras_derate = 9,   .tras_derate_05T = 1,
    .trpab_derate = 8,  .trpab_derate_05T = 0,
    .trp_derate = 6,    .trp_derate_05T = 1,
    .trrd_derate = 4,   .trrd_derate_05T = 0,
    .trtpd = 12,    .trtpd_05T = 0,
    .twtpd = 14,    .twtpd_05T = 1,
    .tmrr2w_odt_off = 8,
    .tmrr2w_odt_on = 10,
    .ckeprd = 2,
    .ckelckcnt = 2,
    .zqlat2 = 12,


    .dqsinctl = 5,   .datlat = 15
},

{
    .dramType = TYPE_LPDDR4, .freq = 1600, .cbtMode = CBT_BYTE_MODE1, .readDBI = 0,
    .readLat = 32,  .writeLat =  14,    .DivMode =  DIV8_MODE,

    .tras = 8,  .tras_05T = 1,
    .trp = 6,   .trp_05T = 0,
    .trpab = 7, .trpab_05T = 0,
    .trc = 15,  .trc_05T = 0,
    .trfc = 100,    .trfc_05T = 0,
    .trfcpb = 44,   .trfcpb_05T = 0,
    .txp = 0,   .txp_05T = 0,
    .trtp = 1,  .trtp_05T = 1,
    .trcd = 7,  .trcd_05T = 1,
    .twr = 12,  .twr_05T = 1,
    .twtr = 8,  .twtr_05T = 0,
    .tpbr2pbr = 29, .tpbr2pbr_05T = 0,
    .tpbr2act = 0,  .tpbr2act_05T = 0,
    .tr2mrw = 14,   .tr2mrw_05T = 1,
    .tw2mrw = 9,    .tw2mrw_05T = 0,
    .tmrr2mrw = 12, .tmrr2mrw_05T = 1,
    .tmrw = 4,  .tmrw_05T = 1,
    .tmrd = 6,  .tmrd_05T = 1,
    .tmrwckel = 7,  .tmrwckel_05T = 1,
    .tpde = 1,  .tpde_05T = 1,
    .tpdx = 1,  .tpdx_05T = 0,
    .tmrri = 10,    .tmrri_05T = 1,
    .trrd = 3,  .trrd_05T = 0,
    .trrd_4266 = 2, .trrd_4266_05T = 0,
    .tfaw = 8,  .tfaw_05T = 0,
    .tfaw_4266 = 4, .tfaw_4266_05T = 0,
    .trtw_odt_off = 5,  .trtw_odt_off_05T = 0,
    .trtw_odt_on = 7,   .trtw_odt_on_05T = 0,
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
    .trcd_derate = 8,   .trcd_derate_05T = 0,
    .trc_derate = 17,   .trc_derate_05T = 0,
    .tras_derate = 9,   .tras_derate_05T = 1,
    .trpab_derate = 8,  .trpab_derate_05T = 0,
    .trp_derate = 6,    .trp_derate_05T = 1,
    .trrd_derate = 4,   .trrd_derate_05T = 0,
    .trtpd = 13,    .trtpd_05T = 0,
    .twtpd = 15,    .twtpd_05T = 1,
    .tmrr2w_odt_off = 9,
    .tmrr2w_odt_on = 11,
    .ckeprd = 2,
    .ckelckcnt = 2,
    .zqlat2 = 12,


    .dqsinctl = 5,   .datlat = 15
},
#endif
#endif
#if SUPPORT_LP4_DDR2667_ACTIM

{
    .dramType = TYPE_LPDDR4, .freq = 1333, .cbtMode = CBT_NORMAL_MODE, .readDBI = 0,
    .readLat = 24,  .writeLat =  12,    .DivMode =  DIV8_MODE,

    .tras = 6,  .tras_05T = 0,
    .trp = 5,   .trp_05T = 0,
    .trpab = 6, .trpab_05T = 0,
    .trc = 11,  .trc_05T = 1,
    .trfc = 81, .trfc_05T = 1,
    .trfcpb = 35,   .trfcpb_05T = 0,
    .txp = 0,   .txp_05T = 0,
    .trtp = 1,  .trtp_05T = 1,
    .trcd = 6,  .trcd_05T = 1,
    .twr = 10,  .twr_05T = 0,
    .twtr = 6,  .twtr_05T = 0,
    .tpbr2pbr = 23, .tpbr2pbr_05T = 1,
    .tpbr2act = 0,  .tpbr2act_05T = 0,
    .tr2mrw = 12,   .tr2mrw_05T = 0,
    .tw2mrw = 8,    .tw2mrw_05T = 0,
    .tmrr2mrw = 10, .tmrr2mrw_05T = 0,
    .tmrw = 4,  .tmrw_05T = 0,
    .tmrd = 5,  .tmrd_05T = 1,
    .tmrwckel = 6,  .tmrwckel_05T = 1,
    .tpde = 1,  .tpde_05T = 1,
    .tpdx = 1,  .tpdx_05T = 0,
    .tmrri = 9, .tmrri_05T = 0,
    .trrd = 2,  .trrd_05T = 1,
    .trrd_4266 = 2, .trrd_4266_05T = 0,
    .tfaw = 5,  .tfaw_05T = 1,
    .tfaw_4266 = 2, .tfaw_4266_05T = 1,
    .trtw_odt_off = 3,  .trtw_odt_off_05T = 0,
    .trtw_odt_on = 6,   .trtw_odt_on_05T = 0,
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
    .trcd_derate = 7,   .trcd_derate_05T = 0,
    .trc_derate = 13,   .trc_derate_05T = 0,
    .tras_derate = 6,   .tras_derate_05T = 1,
    .trpab_derate = 6,  .trpab_derate_05T = 1,
    .trp_derate = 5,    .trp_derate_05T = 1,
    .trrd_derate = 3,   .trrd_derate_05T = 0,
    .trtpd = 10,    .trtpd_05T = 1,
    .twtpd = 13,    .twtpd_05T = 0,
    .tmrr2w_odt_off = 6,
    .tmrr2w_odt_on = 8,
    .ckeprd = 2,
    .ckelckcnt = 2,
    .zqlat2 = 11,


    .dqsinctl = TBD,     .datlat = TBD
},

{
    .dramType = TYPE_LPDDR4, .freq = 1333, .cbtMode = CBT_BYTE_MODE1, .readDBI = 0,
    .readLat = 26,  .writeLat =  12,    .DivMode =  DIV8_MODE,

    .tras = 6,  .tras_05T = 0,
    .trp = 5,   .trp_05T = 0,
    .trpab = 6, .trpab_05T = 0,
    .trc = 11,  .trc_05T = 1,
    .trfc = 81, .trfc_05T = 1,
    .trfcpb = 35,   .trfcpb_05T = 0,
    .txp = 0,   .txp_05T = 0,
    .trtp = 1,  .trtp_05T = 1,
    .trcd = 6,  .trcd_05T = 1,
    .twr = 11,  .twr_05T = 1,
    .twtr = 7,  .twtr_05T = 1,
    .tpbr2pbr = 23, .tpbr2pbr_05T = 1,
    .tpbr2act = 0,  .tpbr2act_05T = 0,
    .tr2mrw = 12,   .tr2mrw_05T = 1,
    .tw2mrw = 8,    .tw2mrw_05T = 0,
    .tmrr2mrw = 10, .tmrr2mrw_05T = 1,
    .tmrw = 4,  .tmrw_05T = 0,
    .tmrd = 5,  .tmrd_05T = 1,
    .tmrwckel = 6,  .tmrwckel_05T = 1,
    .tpde = 1,  .tpde_05T = 1,
    .tpdx = 1,  .tpdx_05T = 0,
    .tmrri = 9, .tmrri_05T = 0,
    .trrd = 2,  .trrd_05T = 1,
    .trrd_4266 = 2, .trrd_4266_05T = 0,
    .tfaw = 5,  .tfaw_05T = 1,
    .tfaw_4266 = 2, .tfaw_4266_05T = 1,
    .trtw_odt_off = 4,  .trtw_odt_off_05T = 0,
    .trtw_odt_on = 6,   .trtw_odt_on_05T = 0,
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
    .trcd_derate = 7,   .trcd_derate_05T = 0,
    .trc_derate = 13,   .trc_derate_05T = 0,
    .tras_derate = 6,   .tras_derate_05T = 1,
    .trpab_derate = 6,  .trpab_derate_05T = 1,
    .trp_derate = 5,    .trp_derate_05T = 1,
    .trrd_derate = 3,   .trrd_derate_05T = 0,
    .trtpd = 11,    .trtpd_05T = 0,
    .twtpd = 13,    .twtpd_05T = 1,
    .tmrr2w_odt_off = 7,
    .tmrr2w_odt_on = 9,
    .ckeprd = 2,
    .ckelckcnt = 2,
    .zqlat2 = 11,


    .dqsinctl = TBD,     .datlat = TBD
},
#endif
#if SUPPORT_LP4_DDR2400_ACTIM

{
    .dramType = TYPE_LPDDR4, .freq = 1200, .cbtMode = CBT_NORMAL_MODE, .readDBI = 0,
    .readLat = 24,  .writeLat =  12,    .DivMode =  DIV8_MODE,

    .tras = 4,  .tras_05T = 1,
    .trp = 4,   .trp_05T = 0,
    .trpab = 5, .trpab_05T = 0,
    .trc = 9,   .trc_05T = 1,
    .trfc = 72, .trfc_05T = 1,
    .trfcpb = 30,   .trfcpb_05T = 1,
    .txp = 0,   .txp_05T = 1,
    .trtp = 1,  .trtp_05T = 0,
    .trcd = 5,  .trcd_05T = 1,
    .twr = 9,   .twr_05T = 1,
    .twtr = 6,  .twtr_05T = 1,
    .tpbr2pbr = 20, .tpbr2pbr_05T = 1,
    .tpbr2act = 0,  .tpbr2act_05T = 0,
    .tr2mrw = 12,   .tr2mrw_05T = 0,
    .tw2mrw = 8,    .tw2mrw_05T = 0,
    .tmrr2mrw = 10, .tmrr2mrw_05T = 0,
    .tmrw = 4,  .tmrw_05T = 0,
    .tmrd = 5,  .tmrd_05T = 0,
    .tmrwckel = 6,  .tmrwckel_05T = 0,
    .tpde = 1,  .tpde_05T = 1,
    .tpdx = 1,  .tpdx_05T = 0,
    .tmrri = 8, .tmrri_05T = 0,
    .trrd = 2,  .trrd_05T = 1,
    .trrd_4266 = 1, .trrd_4266_05T = 1,
    .tfaw = 4,  .tfaw_05T = 1,
    .tfaw_4266 = 1, .tfaw_4266_05T = 1,
    .trtw_odt_off = 3,  .trtw_odt_off_05T = 0,
    .trtw_odt_on = 6,   .trtw_odt_on_05T = 0,
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
    .trcd_derate = 6,   .trcd_derate_05T = 0,
    .trc_derate = 10,   .trc_derate_05T = 1,
    .tras_derate = 5,   .tras_derate_05T = 0,
    .trpab_derate = 5,  .trpab_derate_05T = 1,
    .trp_derate = 4,    .trp_derate_05T = 1,
    .trrd_derate = 3,   .trrd_derate_05T = 0,
    .trtpd = 10,    .trtpd_05T = 1,
    .twtpd = 12,    .twtpd_05T = 0,
    .tmrr2w_odt_off = 6,
    .tmrr2w_odt_on = 8,
    .ckeprd = 2,
    .ckelckcnt = 2,
    .zqlat2 = 10,


    .dqsinctl = 4,   .datlat = 13
},

{
    .dramType = TYPE_LPDDR4, .freq = 1200, .cbtMode = CBT_BYTE_MODE1, .readDBI = 0,
    .readLat = 26,  .writeLat =  12,    .DivMode =  DIV8_MODE,

    .tras = 4,  .tras_05T = 1,
    .trp = 4,   .trp_05T = 0,
    .trpab = 5, .trpab_05T = 0,
    .trc = 9,   .trc_05T = 1,
    .trfc = 72, .trfc_05T = 1,
    .trfcpb = 30,   .trfcpb_05T = 1,
    .txp = 0,   .txp_05T = 1,
    .trtp = 1,  .trtp_05T = 0,
    .trcd = 5,  .trcd_05T = 1,
    .twr = 10,  .twr_05T = 0,
    .twtr = 6,  .twtr_05T = 0,
    .tpbr2pbr = 20, .tpbr2pbr_05T = 1,
    .tpbr2act = 0,  .tpbr2act_05T = 0,
    .tr2mrw = 12,   .tr2mrw_05T = 1,
    .tw2mrw = 8,    .tw2mrw_05T = 0,
    .tmrr2mrw = 10, .tmrr2mrw_05T = 1,
    .tmrw = 4,  .tmrw_05T = 0,
    .tmrd = 5,  .tmrd_05T = 0,
    .tmrwckel = 6,  .tmrwckel_05T = 0,
    .tpde = 1,  .tpde_05T = 1,
    .tpdx = 1,  .tpdx_05T = 0,
    .tmrri = 8, .tmrri_05T = 0,
    .trrd = 2,  .trrd_05T = 1,
    .trrd_4266 = 1, .trrd_4266_05T = 1,
    .tfaw = 4,  .tfaw_05T = 1,
    .tfaw_4266 = 1, .tfaw_4266_05T = 1,
    .trtw_odt_off = 4,  .trtw_odt_off_05T = 0,
    .trtw_odt_on = 6,   .trtw_odt_on_05T = 0,
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
    .trcd_derate = 6,   .trcd_derate_05T = 0,
    .trc_derate = 10,   .trc_derate_05T = 1,
    .tras_derate = 5,   .tras_derate_05T = 0,
    .trpab_derate = 5,  .trpab_derate_05T = 1,
    .trp_derate = 4,    .trp_derate_05T = 1,
    .trrd_derate = 3,   .trrd_derate_05T = 0,
    .trtpd = 11,    .trtpd_05T = 0,
    .twtpd = 13,    .twtpd_05T = 0,
    .tmrr2w_odt_off = 7,
    .tmrr2w_odt_on = 9,
    .ckeprd = 2,
    .ckelckcnt = 2,
    .zqlat2 = 10,


    .dqsinctl = 4,   .datlat = 13
},
#endif
#if SUPPORT_LP4_DDR1866_ACTIM

{
    .dramType = TYPE_LPDDR4, .freq = 933, .cbtMode = CBT_NORMAL_MODE, .readDBI = 0,
    .readLat = 20,  .writeLat =  10,    .DivMode =  DIV8_MODE,

    .tras = 1,  .tras_05T = 1,
    .trp = 3,   .trp_05T = 0,
    .trpab = 3, .trpab_05T = 1,
    .trc = 5,   .trc_05T = 0,
    .trfc = 53, .trfc_05T = 1,
    .trfcpb = 21,   .trfcpb_05T = 0,
    .txp = 0,   .txp_05T = 0,
    .trtp = 0,  .trtp_05T = 1,
    .trcd = 4,  .trcd_05T = 1,
    .twr = 8,   .twr_05T = 1,
    .twtr = 5,  .twtr_05T = 1,
    .tpbr2pbr = 14, .tpbr2pbr_05T = 0,
    .tpbr2act = 0,  .tpbr2act_05T = 0,
    .tr2mrw = 10,   .tr2mrw_05T = 0,
    .tw2mrw = 7,    .tw2mrw_05T = 0,
    .tmrr2mrw = 9,  .tmrr2mrw_05T = 0,
    .tmrw = 3,  .tmrw_05T = 0,
    .tmrd = 4,  .tmrd_05T = 0,
    .tmrwckel = 5,  .tmrwckel_05T = 0,
    .tpde = 1,  .tpde_05T = 1,
    .tpdx = 1,  .tpdx_05T = 0,
    .tmrri = 6, .tmrri_05T = 0,
    .trrd = 1,  .trrd_05T = 1,
    .trrd_4266 = 1, .trrd_4266_05T = 0,
    .tfaw = 1,  .tfaw_05T = 1,
    .tfaw_4266 = 0, .tfaw_4266_05T = 0,
    .trtw_odt_off = 3,  .trtw_odt_off_05T = 0,
    .trtw_odt_on = 5,   .trtw_odt_on_05T = 0,
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
    .trcd_derate = 5,   .trcd_derate_05T = 0,
    .trc_derate = 6,    .trc_derate_05T = 1,
    .tras_derate = 2,   .tras_derate_05T = 0,
    .trpab_derate = 4,  .trpab_derate_05T = 0,
    .trp_derate = 3,    .trp_derate_05T = 1,
    .trrd_derate = 2,   .trrd_derate_05T = 0,
    .trtpd = 9, .trtpd_05T = 1,
    .twtpd = 10,    .twtpd_05T = 1,
    .tmrr2w_odt_off = 5,
    .tmrr2w_odt_on = 7,
    .ckeprd = 1,
    .ckelckcnt = 2,
    .zqlat2 = 7,


    .dqsinctl = 3,   .datlat = 13
},

{
    .dramType = TYPE_LPDDR4, .freq = 933, .cbtMode = CBT_BYTE_MODE1, .readDBI = 0,
    .readLat = 22,  .writeLat =  10,    .DivMode =  DIV8_MODE,

    .tras = 1,  .tras_05T = 1,
    .trp = 3,   .trp_05T = 0,
    .trpab = 3, .trpab_05T = 1,
    .trc = 5,   .trc_05T = 0,
    .trfc = 53, .trfc_05T = 1,
    .trfcpb = 21,   .trfcpb_05T = 0,
    .txp = 0,   .txp_05T = 0,
    .trtp = 0,  .trtp_05T = 1,
    .trcd = 4,  .trcd_05T = 1,
    .twr = 8,   .twr_05T = 0,
    .twtr = 5,  .twtr_05T = 0,
    .tpbr2pbr = 14, .tpbr2pbr_05T = 0,
    .tpbr2act = 0,  .tpbr2act_05T = 0,
    .tr2mrw = 10,   .tr2mrw_05T = 1,
    .tw2mrw = 7,    .tw2mrw_05T = 0,
    .tmrr2mrw = 9,  .tmrr2mrw_05T = 1,
    .tmrw = 3,  .tmrw_05T = 0,
    .tmrd = 4,  .tmrd_05T = 0,
    .tmrwckel = 5,  .tmrwckel_05T = 0,
    .tpde = 1,  .tpde_05T = 1,
    .tpdx = 1,  .tpdx_05T = 0,
    .tmrri = 6, .tmrri_05T = 0,
    .trrd = 1,  .trrd_05T = 1,
    .trrd_4266 = 1, .trrd_4266_05T = 0,
    .tfaw = 1,  .tfaw_05T = 1,
    .tfaw_4266 = 0, .tfaw_4266_05T = 0,
    .trtw_odt_off = 3,  .trtw_odt_off_05T = 0,
    .trtw_odt_on = 5,   .trtw_odt_on_05T = 0,
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
    .trcd_derate = 5,   .trcd_derate_05T = 0,
    .trc_derate = 6,    .trc_derate_05T = 1,
    .tras_derate = 2,   .tras_derate_05T = 0,
    .trpab_derate = 4,  .trpab_derate_05T = 0,
    .trp_derate = 3,    .trp_derate_05T = 1,
    .trrd_derate = 2,   .trrd_derate_05T = 0,
    .trtpd = 10,    .trtpd_05T = 0,
    .twtpd = 11,    .twtpd_05T = 0,
    .tmrr2w_odt_off = 6,
    .tmrr2w_odt_on = 8,
    .ckeprd = 1,
    .ckelckcnt = 2,
    .zqlat2 = 7,


    .dqsinctl = 3,   .datlat = 13
},
#endif
#if SUPPORT_LP4_DDR1600_ACTIM

{
    .dramType = TYPE_LPDDR4, .freq = 800, .cbtMode = CBT_NORMAL_MODE, .readDBI = 0,
    .readLat = 14,  .writeLat =  8, .DivMode =  DIV4_MODE,

    .tras = 9,  .tras_05T = 0,
    .trp = 6,   .trp_05T = 0,
    .trpab = 7, .trpab_05T = 0,
    .trc = 15,  .trc_05T = 0,
    .trfc = 100,    .trfc_05T = 0,
    .trfcpb = 44,   .trfcpb_05T = 0,
    .txp = 0,   .txp_05T = 0,
    .trtp = 3,  .trtp_05T = 0,
    .trcd = 8,  .trcd_05T = 0,
    .twr = 15,  .twr_05T = 0,
    .twtr = 10, .twtr_05T = 0,
    .tpbr2pbr = 29, .tpbr2pbr_05T = 0,
    .tpbr2act = 0,  .tpbr2act_05T = 0,
    .tr2mrw = 17,   .tr2mrw_05T = 0,
    .tw2mrw = 13,   .tw2mrw_05T = 0,
    .tmrr2mrw = 14, .tmrr2mrw_05T = 0,
    .tmrw = 6,  .tmrw_05T = 0,
    .tmrd = 7,  .tmrd_05T = 0,
    .tmrwckel = 9,  .tmrwckel_05T = 0,
    .tpde = 3,  .tpde_05T = 0,
    .tpdx = 3,  .tpdx_05T = 0,
    .tmrri = 11,    .tmrri_05T = 0,
    .trrd = 3,  .trrd_05T = 0,
    .trrd_4266 = 2, .trrd_4266_05T = 0,
    .tfaw = 8,  .tfaw_05T = 0,
    .tfaw_4266 = 4, .tfaw_4266_05T = 0,
    .trtw_odt_off = 7,  .trtw_odt_off_05T = 0,
    .trtw_odt_on = 11,  .trtw_odt_on_05T = 0,
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
    .trcd_derate = 8,   .trcd_derate_05T = 0,
    .trc_derate = 17,   .trc_derate_05T = 0,
    .tras_derate = 10,  .tras_derate_05T = 0,
    .trpab_derate = 8,  .trpab_derate_05T = 0,
    .trp_derate = 6,    .trp_derate_05T = 0,
    .trrd_derate = 4,   .trrd_derate_05T = 0,
    .trtpd = 15,    .trtpd_05T = 0,
    .twtpd = 19,    .twtpd_05T = 0,
    .tmrr2w_odt_off = 10,
    .tmrr2w_odt_on = 12,
    .ckeprd = 2,
    .ckelckcnt = 3,
    .zqlat2 = 12,


    .dqsinctl = 2,   .datlat = 10
},

{
    .dramType = TYPE_LPDDR4, .freq = 800, .cbtMode = CBT_BYTE_MODE1, .readDBI = 0,
    .readLat = 16,  .writeLat =  8, .DivMode =  DIV4_MODE,

    .tras = 9,  .tras_05T = 0,
    .trp = 6,   .trp_05T = 0,
    .trpab = 7, .trpab_05T = 0,
    .trc = 15,  .trc_05T = 0,
    .trfc = 100,    .trfc_05T = 0,
    .trfcpb = 44,   .trfcpb_05T = 0,
    .txp = 0,   .txp_05T = 0,
    .trtp = 3,  .trtp_05T = 0,
    .trcd = 8,  .trcd_05T = 0,
    .twr = 16,  .twr_05T = 0,
    .twtr = 11, .twtr_05T = 0,
    .tpbr2pbr = 29, .tpbr2pbr_05T = 0,
    .tpbr2act = 0,  .tpbr2act_05T = 0,
    .tr2mrw = 18,   .tr2mrw_05T = 0,
    .tw2mrw = 13,   .tw2mrw_05T = 0,
    .tmrr2mrw = 15, .tmrr2mrw_05T = 0,
    .tmrw = 6,  .tmrw_05T = 0,
    .tmrd = 7,  .tmrd_05T = 0,
    .tmrwckel = 9,  .tmrwckel_05T = 0,
    .tpde = 3,  .tpde_05T = 0,
    .tpdx = 3,  .tpdx_05T = 0,
    .tmrri = 11,    .tmrri_05T = 0,
    .trrd = 3,  .trrd_05T = 0,
    .trrd_4266 = 2, .trrd_4266_05T = 0,
    .tfaw = 8,  .tfaw_05T = 0,
    .tfaw_4266 = 4, .tfaw_4266_05T = 0,
    .trtw_odt_off = 8,  .trtw_odt_off_05T = 0,
    .trtw_odt_on = 12,  .trtw_odt_on_05T = 0,
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
    .trcd_derate = 8,   .trcd_derate_05T = 0,
    .trc_derate = 17,   .trc_derate_05T = 0,
    .tras_derate = 10,  .tras_derate_05T = 0,
    .trpab_derate = 8,  .trpab_derate_05T = 0,
    .trp_derate = 6,    .trp_derate_05T = 0,
    .trrd_derate = 4,   .trrd_derate_05T = 0,
    .trtpd = 16,    .trtpd_05T = 0,
    .twtpd = 19,    .twtpd_05T = 0,
    .tmrr2w_odt_off = 11,
    .tmrr2w_odt_on = 13,
    .ckeprd = 2,
    .ckelckcnt = 3,
    .zqlat2 = 12,


    .dqsinctl = 2,   .datlat = 10
},

{
    .dramType = TYPE_LPDDR4, .freq = 800, .cbtMode = CBT_NORMAL_MODE, .readDBI = 0,
    .readLat = 14,  .writeLat =  8, .DivMode =  DIV8_MODE,

    .tras = 0,  .tras_05T = 0,
    .trp = 2,   .trp_05T = 1,
    .trpab = 3, .trpab_05T = 0,
    .trc = 3,   .trc_05T = 0,
    .trfc = 44, .trfc_05T = 0,
    .trfcpb = 16,   .trfcpb_05T = 0,
    .txp = 0,   .txp_05T = 0,
    .trtp = 0,  .trtp_05T = 1,
    .trcd = 4,  .trcd_05T = 0,
    .twr = 7,   .twr_05T = 1,
    .twtr = 4,  .twtr_05T = 1,
    .tpbr2pbr = 11, .tpbr2pbr_05T = 0,
    .tpbr2act = 0,  .tpbr2act_05T = 0,
    .tr2mrw = 8,    .tr2mrw_05T = 1,
    .tw2mrw = 6,    .tw2mrw_05T = 1,
    .tmrr2mrw = 7,  .tmrr2mrw_05T = 0,
    .tmrw = 3,  .tmrw_05T = 0,
    .tmrd = 3,  .tmrd_05T = 1,
    .tmrwckel = 4,  .tmrwckel_05T = 1,
    .tpde = 1,  .tpde_05T = 1,
    .tpdx = 1,  .tpdx_05T = 0,
    .tmrri = 5, .tmrri_05T = 1,
    .trrd = 1,  .trrd_05T = 0,
    .trrd_4266 = 0, .trrd_4266_05T = 1,
    .tfaw = 0,  .tfaw_05T = 0,
    .tfaw_4266 = 0, .tfaw_4266_05T = 0,
    .trtw_odt_off = 1,  .trtw_odt_off_05T = 0,
    .trtw_odt_on = 4,   .trtw_odt_on_05T = 0,
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
    .trcd_derate = 4,   .trcd_derate_05T = 0,
    .trc_derate = 4,    .trc_derate_05T = 0,
    .tras_derate = 0,   .tras_derate_05T = 1,
    .trpab_derate = 3,  .trpab_derate_05T = 1,
    .trp_derate = 2,    .trp_derate_05T = 1,
    .trrd_derate = 1,   .trrd_derate_05T = 1,
    .trtpd = 7, .trtpd_05T = 1,
    .twtpd = 9, .twtpd_05T = 1,
    .tmrr2w_odt_off = 3,
    .tmrr2w_odt_on = 5,
    .ckeprd = 1,
    .ckelckcnt = 2,
    .zqlat2 = 6,


    .dqsinctl = 2,   .datlat = 10
},

{
    .dramType = TYPE_LPDDR4, .freq = 800, .cbtMode = CBT_BYTE_MODE1, .readDBI = 0,
    .readLat = 16,  .writeLat =  8, .DivMode =  DIV8_MODE,

    .tras = 0,  .tras_05T = 0,
    .trp = 2,   .trp_05T = 1,
    .trpab = 3, .trpab_05T = 0,
    .trc = 3,   .trc_05T = 0,
    .trfc = 44, .trfc_05T = 0,
    .trfcpb = 16,   .trfcpb_05T = 0,
    .txp = 0,   .txp_05T = 0,
    .trtp = 0,  .trtp_05T = 1,
    .trcd = 4,  .trcd_05T = 0,
    .twr = 7,   .twr_05T = 0,
    .twtr = 4,  .twtr_05T = 0,
    .tpbr2pbr = 11, .tpbr2pbr_05T = 0,
    .tpbr2act = 0,  .tpbr2act_05T = 0,
    .tr2mrw = 9,    .tr2mrw_05T = 0,
    .tw2mrw = 6,    .tw2mrw_05T = 1,
    .tmrr2mrw = 7,  .tmrr2mrw_05T = 1,
    .tmrw = 3,  .tmrw_05T = 0,
    .tmrd = 3,  .tmrd_05T = 1,
    .tmrwckel = 4,  .tmrwckel_05T = 1,
    .tpde = 1,  .tpde_05T = 1,
    .tpdx = 1,  .tpdx_05T = 0,
    .tmrri = 5, .tmrri_05T = 1,
    .trrd = 1,  .trrd_05T = 0,
    .trrd_4266 = 0, .trrd_4266_05T = 1,
    .tfaw = 0,  .tfaw_05T = 0,
    .tfaw_4266 = 0, .tfaw_4266_05T = 0,
    .trtw_odt_off = 2,  .trtw_odt_off_05T = 0,
    .trtw_odt_on = 4,   .trtw_odt_on_05T = 0,
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
    .trcd_derate = 4,   .trcd_derate_05T = 0,
    .trc_derate = 4,    .trc_derate_05T = 0,
    .tras_derate = 0,   .tras_derate_05T = 1,
    .trpab_derate = 3,  .trpab_derate_05T = 1,
    .trp_derate = 2,    .trp_derate_05T = 1,
    .trrd_derate = 1,   .trrd_derate_05T = 1,
    .trtpd = 8, .trtpd_05T = 0,
    .twtpd = 9, .twtpd_05T = 1,
    .tmrr2w_odt_off = 4,
    .tmrr2w_odt_on = 6,
    .ckeprd = 1,
    .ckelckcnt = 2,
    .zqlat2 = 6,


    .dqsinctl = 2,   .datlat = 10
},
#endif
#if SUPPORT_LP4_DDR1333_ACTIM

{
    .dramType = TYPE_LPDDR4, .freq = 666, .cbtMode = CBT_NORMAL_MODE, .readDBI = 0,
    .readLat = 14,  .writeLat =  8, .DivMode =  DIV4_MODE,

    .tras = 6,  .tras_05T = 0,
    .trp = 4,   .trp_05T = 0,
    .trpab = 5, .trpab_05T = 0,
    .trc = 11,  .trc_05T = 0,
    .trfc = 82, .trfc_05T = 0,
    .trfcpb = 35,   .trfcpb_05T = 0,
    .txp = 0,   .txp_05T = 0,
    .trtp = 3,  .trtp_05T = 0,
    .trcd = 6,  .trcd_05T = 0,
    .twr = 14,  .twr_05T = 0,
    .twtr = 10, .twtr_05T = 0,
    .tpbr2pbr = 23, .tpbr2pbr_05T = 0,
    .tpbr2act = 0,  .tpbr2act_05T = 0,
    .tr2mrw = 17,   .tr2mrw_05T = 0,
    .tw2mrw = 13,   .tw2mrw_05T = 0,
    .tmrr2mrw = 14, .tmrr2mrw_05T = 0,
    .tmrw = 6,  .tmrw_05T = 0,
    .tmrd = 6,  .tmrd_05T = 0,
    .tmrwckel = 8,  .tmrwckel_05T = 0,
    .tpde = 3,  .tpde_05T = 0,
    .tpdx = 3,  .tpdx_05T = 0,
    .tmrri = 9, .tmrri_05T = 0,
    .trrd = 3,  .trrd_05T = 0,
    .trrd_4266 = 2, .trrd_4266_05T = 0,
    .tfaw = 6,  .tfaw_05T = 0,
    .tfaw_4266 = 2, .tfaw_4266_05T = 0,
    .trtw_odt_off = 7,  .trtw_odt_off_05T = 0,
    .trtw_odt_on = 11,  .trtw_odt_on_05T = 0,
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
    .trcd_derate = 7,   .trcd_derate_05T = 0,
    .trc_derate = 13,   .trc_derate_05T = 0,
    .tras_derate = 7,   .tras_derate_05T = 0,
    .trpab_derate = 6,  .trpab_derate_05T = 0,
    .trp_derate = 5,    .trp_derate_05T = 0,
    .trrd_derate = 3,   .trrd_derate_05T = 0,
    .trtpd = 15,    .trtpd_05T = 0,
    .twtpd = 17,    .twtpd_05T = 0,
    .tmrr2w_odt_off = 10,
    .tmrr2w_odt_on = 12,
    .ckeprd = 2,
    .ckelckcnt = 3,
    .zqlat2 = 10,


    .dqsinctl = TBD,     .datlat = TBD
},

{
    .dramType = TYPE_LPDDR4, .freq = 666, .cbtMode = CBT_BYTE_MODE1, .readDBI = 0,
    .readLat = 16,  .writeLat =  8, .DivMode =  DIV4_MODE,

    .tras = 6,  .tras_05T = 0,
    .trp = 4,   .trp_05T = 0,
    .trpab = 5, .trpab_05T = 0,
    .trc = 11,  .trc_05T = 0,
    .trfc = 82, .trfc_05T = 0,
    .trfcpb = 35,   .trfcpb_05T = 0,
    .txp = 0,   .txp_05T = 0,
    .trtp = 3,  .trtp_05T = 0,
    .trcd = 6,  .trcd_05T = 0,
    .twr = 15,  .twr_05T = 0,
    .twtr = 10, .twtr_05T = 0,
    .tpbr2pbr = 23, .tpbr2pbr_05T = 0,
    .tpbr2act = 0,  .tpbr2act_05T = 0,
    .tr2mrw = 18,   .tr2mrw_05T = 0,
    .tw2mrw = 13,   .tw2mrw_05T = 0,
    .tmrr2mrw = 15, .tmrr2mrw_05T = 0,
    .tmrw = 6,  .tmrw_05T = 0,
    .tmrd = 6,  .tmrd_05T = 0,
    .tmrwckel = 8,  .tmrwckel_05T = 0,
    .tpde = 3,  .tpde_05T = 0,
    .tpdx = 3,  .tpdx_05T = 0,
    .tmrri = 9, .tmrri_05T = 0,
    .trrd = 3,  .trrd_05T = 0,
    .trrd_4266 = 2, .trrd_4266_05T = 0,
    .tfaw = 6,  .tfaw_05T = 0,
    .tfaw_4266 = 2, .tfaw_4266_05T = 0,
    .trtw_odt_off = 8,  .trtw_odt_off_05T = 0,
    .trtw_odt_on = 12,  .trtw_odt_on_05T = 0,
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
    .trcd_derate = 7,   .trcd_derate_05T = 0,
    .trc_derate = 13,   .trc_derate_05T = 0,
    .tras_derate = 7,   .tras_derate_05T = 0,
    .trpab_derate = 6,  .trpab_derate_05T = 0,
    .trp_derate = 5,    .trp_derate_05T = 0,
    .trrd_derate = 3,   .trrd_derate_05T = 0,
    .trtpd = 16,    .trtpd_05T = 0,
    .twtpd = 18,    .twtpd_05T = 0,
    .tmrr2w_odt_off = 11,
    .tmrr2w_odt_on = 13,
    .ckeprd = 2,
    .ckelckcnt = 3,
    .zqlat2 = 10,


    .dqsinctl = TBD,     .datlat = TBD
},

{
    .dramType = TYPE_LPDDR4, .freq = 666, .cbtMode = CBT_NORMAL_MODE, .readDBI = 0,
    .readLat = 14,  .writeLat =  8, .DivMode =  DIV8_MODE,

    .tras = 0,  .tras_05T = 0,
    .trp = 1,   .trp_05T = 1,
    .trpab = 2, .trpab_05T = 0,
    .trc = 1,   .trc_05T = 0,
    .trfc = 35, .trfc_05T = 0,
    .trfcpb = 11,   .trfcpb_05T = 1,
    .txp = 0,   .txp_05T = 0,
    .trtp = 0,  .trtp_05T = 1,
    .trcd = 3,  .trcd_05T = 0,
    .twr = 6,   .twr_05T = 0,
    .twtr = 4,  .twtr_05T = 1,
    .tpbr2pbr = 8,  .tpbr2pbr_05T = 0,
    .tpbr2act = 0,  .tpbr2act_05T = 0,
    .tr2mrw = 8,    .tr2mrw_05T = 1,
    .tw2mrw = 6,    .tw2mrw_05T = 1,
    .tmrr2mrw = 7,  .tmrr2mrw_05T = 0,
    .tmrw = 3,  .tmrw_05T = 0,
    .tmrd = 3,  .tmrd_05T = 0,
    .tmrwckel = 4,  .tmrwckel_05T = 0,
    .tpde = 1,  .tpde_05T = 1,
    .tpdx = 1,  .tpdx_05T = 0,
    .tmrri = 4, .tmrri_05T = 1,
    .trrd = 1,  .trrd_05T = 0,
    .trrd_4266 = 0, .trrd_4266_05T = 1,
    .tfaw = 0,  .tfaw_05T = 0,
    .tfaw_4266 = 0, .tfaw_4266_05T = 0,
    .trtw_odt_off = 1,  .trtw_odt_off_05T = 0,
    .trtw_odt_on = 4,   .trtw_odt_on_05T = 0,
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
    .trcd_derate = 3,   .trcd_derate_05T = 1,
    .trc_derate = 2,    .trc_derate_05T = 0,
    .tras_derate = 0,   .tras_derate_05T = 0,
    .trpab_derate = 2,  .trpab_derate_05T = 1,
    .trp_derate = 2,    .trp_derate_05T = 0,
    .trrd_derate = 1,   .trrd_derate_05T = 0,
    .trtpd = 7, .trtpd_05T = 1,
    .twtpd = 8, .twtpd_05T = 1,
    .tmrr2w_odt_off = 3,
    .tmrr2w_odt_on = 5,
    .ckeprd = 1,
    .ckelckcnt = 2,
    .zqlat2 = 5,


    .dqsinctl = TBD,     .datlat = TBD
},

{
    .dramType = TYPE_LPDDR4, .freq = 666, .cbtMode = CBT_BYTE_MODE1, .readDBI = 0,
    .readLat = 16,  .writeLat =  8, .DivMode =  DIV8_MODE,

    .tras = 0,  .tras_05T = 0,
    .trp = 1,   .trp_05T = 1,
    .trpab = 2, .trpab_05T = 0,
    .trc = 1,   .trc_05T = 0,
    .trfc = 35, .trfc_05T = 0,
    .trfcpb = 11,   .trfcpb_05T = 1,
    .txp = 0,   .txp_05T = 0,
    .trtp = 0,  .trtp_05T = 1,
    .trcd = 3,  .trcd_05T = 0,
    .twr = 6,   .twr_05T = 1,
    .twtr = 4,  .twtr_05T = 1,
    .tpbr2pbr = 8,  .tpbr2pbr_05T = 0,
    .tpbr2act = 0,  .tpbr2act_05T = 0,
    .tr2mrw = 9,    .tr2mrw_05T = 0,
    .tw2mrw = 6,    .tw2mrw_05T = 1,
    .tmrr2mrw = 7,  .tmrr2mrw_05T = 1,
    .tmrw = 3,  .tmrw_05T = 0,
    .tmrd = 3,  .tmrd_05T = 0,
    .tmrwckel = 4,  .tmrwckel_05T = 0,
    .tpde = 1,  .tpde_05T = 1,
    .tpdx = 1,  .tpdx_05T = 0,
    .tmrri = 4, .tmrri_05T = 1,
    .trrd = 1,  .trrd_05T = 0,
    .trrd_4266 = 0, .trrd_4266_05T = 1,
    .tfaw = 0,  .tfaw_05T = 0,
    .tfaw_4266 = 0, .tfaw_4266_05T = 0,
    .trtw_odt_off = 2,  .trtw_odt_off_05T = 0,
    .trtw_odt_on = 5,   .trtw_odt_on_05T = 0,
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
    .trcd_derate = 3,   .trcd_derate_05T = 1,
    .trc_derate = 2,    .trc_derate_05T = 0,
    .tras_derate = 0,   .tras_derate_05T = 0,
    .trpab_derate = 2,  .trpab_derate_05T = 1,
    .trp_derate = 2,    .trp_derate_05T = 0,
    .trrd_derate = 1,   .trrd_derate_05T = 0,
    .trtpd = 8, .trtpd_05T = 0,
    .twtpd = 9, .twtpd_05T = 0,
    .tmrr2w_odt_off = 4,
    .tmrr2w_odt_on = 6,
    .ckeprd = 1,
    .ckelckcnt = 2,
    .zqlat2 = 5,


    .dqsinctl = TBD,     .datlat = TBD
},
#endif
#if SUPPORT_LP4_DDR1200_ACTIM

{
    .dramType = TYPE_LPDDR4, .freq = 600, .cbtMode = CBT_NORMAL_MODE, .readDBI = 0,
    .readLat = 14,  .writeLat =  8, .DivMode =  DIV4_MODE,

    .tras = 5,  .tras_05T = 0,
    .trp = 4,   .trp_05T = 0,
    .trpab = 5, .trpab_05T = 0,
    .trc = 10,  .trc_05T = 0,
    .trfc = 73, .trfc_05T = 0,
    .trfcpb = 31,   .trfcpb_05T = 0,
    .txp = 0,   .txp_05T = 0,
    .trtp = 3,  .trtp_05T = 0,
    .trcd = 6,  .trcd_05T = 0,
    .twr = 13,  .twr_05T = 0,
    .twtr = 10, .twtr_05T = 0,
    .tpbr2pbr = 21, .tpbr2pbr_05T = 0,
    .tpbr2act = 0,  .tpbr2act_05T = 0,
    .tr2mrw = 17,   .tr2mrw_05T = 0,
    .tw2mrw = 13,   .tw2mrw_05T = 0,
    .tmrr2mrw = 14, .tmrr2mrw_05T = 0,
    .tmrw = 6,  .tmrw_05T = 0,
    .tmrd = 6,  .tmrd_05T = 0,
    .tmrwckel = 8,  .tmrwckel_05T = 0,
    .tpde = 3,  .tpde_05T = 0,
    .tpdx = 3,  .tpdx_05T = 0,
    .tmrri = 8, .tmrri_05T = 0,
    .trrd = 3,  .trrd_05T = 0,
    .trrd_4266 = 2, .trrd_4266_05T = 0,
    .tfaw = 5,  .tfaw_05T = 0,
    .tfaw_4266 = 2, .tfaw_4266_05T = 0,
    .trtw_odt_off = 7,  .trtw_odt_off_05T = 0,
    .trtw_odt_on = 11,  .trtw_odt_on_05T = 0,
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
    .trcd_derate = 6,   .trcd_derate_05T = 0,
    .trc_derate = 11,   .trc_derate_05T = 0,
    .tras_derate = 6,   .tras_derate_05T = 0,
    .trpab_derate = 5,  .trpab_derate_05T = 0,
    .trp_derate = 4,    .trp_derate_05T = 0,
    .trrd_derate = 3,   .trrd_derate_05T = 0,
    .trtpd = 15,    .trtpd_05T = 0,
    .twtpd = 17,    .twtpd_05T = 0,
    .tmrr2w_odt_off = 10,
    .tmrr2w_odt_on = 12,
    .ckeprd = 2,
    .ckelckcnt = 3,
    .zqlat2 = 10,


    .dqsinctl = 2,   .datlat = 9
},

{
    .dramType = TYPE_LPDDR4, .freq = 600, .cbtMode = CBT_BYTE_MODE1, .readDBI = 0,
    .readLat = 16,  .writeLat =  8, .DivMode =  DIV4_MODE,

    .tras = 5,  .tras_05T = 0,
    .trp = 4,   .trp_05T = 0,
    .trpab = 5, .trpab_05T = 0,
    .trc = 10,  .trc_05T = 0,
    .trfc = 73, .trfc_05T = 0,
    .trfcpb = 31,   .trfcpb_05T = 0,
    .txp = 0,   .txp_05T = 0,
    .trtp = 3,  .trtp_05T = 0,
    .trcd = 6,  .trcd_05T = 0,
    .twr = 14,  .twr_05T = 0,
    .twtr = 10, .twtr_05T = 0,
    .tpbr2pbr = 21, .tpbr2pbr_05T = 0,
    .tpbr2act = 0,  .tpbr2act_05T = 0,
    .tr2mrw = 18,   .tr2mrw_05T = 0,
    .tw2mrw = 13,   .tw2mrw_05T = 0,
    .tmrr2mrw = 15, .tmrr2mrw_05T = 0,
    .tmrw = 6,  .tmrw_05T = 0,
    .tmrd = 6,  .tmrd_05T = 0,
    .tmrwckel = 8,  .tmrwckel_05T = 0,
    .tpde = 3,  .tpde_05T = 0,
    .tpdx = 3,  .tpdx_05T = 0,
    .tmrri = 8, .tmrri_05T = 0,
    .trrd = 3,  .trrd_05T = 0,
    .trrd_4266 = 2, .trrd_4266_05T = 0,
    .tfaw = 5,  .tfaw_05T = 0,
    .tfaw_4266 = 2, .tfaw_4266_05T = 0,
    .trtw_odt_off = 8,  .trtw_odt_off_05T = 0,
    .trtw_odt_on = 12,  .trtw_odt_on_05T = 0,
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
    .trcd_derate = 6,   .trcd_derate_05T = 0,
    .trc_derate = 11,   .trc_derate_05T = 0,
    .tras_derate = 6,   .tras_derate_05T = 0,
    .trpab_derate = 5,  .trpab_derate_05T = 0,
    .trp_derate = 4,    .trp_derate_05T = 0,
    .trrd_derate = 3,   .trrd_derate_05T = 0,
    .trtpd = 16,    .trtpd_05T = 0,
    .twtpd = 18,    .twtpd_05T = 0,
    .tmrr2w_odt_off = 11,
    .tmrr2w_odt_on = 13,
    .ckeprd = 2,
    .ckelckcnt = 3,
    .zqlat2 = 10,


    .dqsinctl = 2,   .datlat = 9
},

{
    .dramType = TYPE_LPDDR4, .freq = 600, .cbtMode = CBT_NORMAL_MODE, .readDBI = 0,
    .readLat = 14,  .writeLat =  8, .DivMode =  DIV8_MODE,

    .tras = 0,  .tras_05T = 0,
    .trp = 1,   .trp_05T = 1,
    .trpab = 2, .trpab_05T = 0,
    .trc = 0,   .trc_05T = 1,
    .trfc = 30, .trfc_05T = 1,
    .trfcpb = 9,    .trfcpb_05T = 1,
    .txp = 0,   .txp_05T = 0,
    .trtp = 0,  .trtp_05T = 1,
    .trcd = 3,  .trcd_05T = 0,
    .twr = 6,   .twr_05T = 1,
    .twtr = 4,  .twtr_05T = 1,
    .tpbr2pbr = 7,  .tpbr2pbr_05T = 0,
    .tpbr2act = 0,  .tpbr2act_05T = 0,
    .tr2mrw = 8,    .tr2mrw_05T = 1,
    .tw2mrw = 6,    .tw2mrw_05T = 1,
    .tmrr2mrw = 7,  .tmrr2mrw_05T = 0,
    .tmrw = 3,  .tmrw_05T = 0,
    .tmrd = 3,  .tmrd_05T = 0,
    .tmrwckel = 4,  .tmrwckel_05T = 0,
    .tpde = 1,  .tpde_05T = 1,
    .tpdx = 1,  .tpdx_05T = 0,
    .tmrri = 4, .tmrri_05T = 0,
    .trrd = 1,  .trrd_05T = 0,
    .trrd_4266 = 0, .trrd_4266_05T = 1,
    .tfaw = 0,  .tfaw_05T = 0,
    .tfaw_4266 = 0, .tfaw_4266_05T = 0,
    .trtw_odt_off = 1,  .trtw_odt_off_05T = 0,
    .trtw_odt_on = 4,   .trtw_odt_on_05T = 0,
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
    .trcd_derate = 3,   .trcd_derate_05T = 0,
    .trc_derate = 1,    .trc_derate_05T = 0,
    .tras_derate = 0,   .tras_derate_05T = 0,
    .trpab_derate = 2,  .trpab_derate_05T = 0,
    .trp_derate = 1,    .trp_derate_05T = 1,
    .trrd_derate = 1,   .trrd_derate_05T = 0,
    .trtpd = 7, .trtpd_05T = 1,
    .twtpd = 8, .twtpd_05T = 1,
    .tmrr2w_odt_off = 3,
    .tmrr2w_odt_on = 5,
    .ckeprd = 1,
    .ckelckcnt = 2,
    .zqlat2 = 5,


    .dqsinctl = 2,   .datlat = 9
},

{
    .dramType = TYPE_LPDDR4, .freq = 600, .cbtMode = CBT_BYTE_MODE1, .readDBI = 0,
    .readLat = 16,  .writeLat =  8, .DivMode =  DIV8_MODE,

    .tras = 0,  .tras_05T = 0,
    .trp = 1,   .trp_05T = 1,
    .trpab = 2, .trpab_05T = 0,
    .trc = 0,   .trc_05T = 1,
    .trfc = 30, .trfc_05T = 1,
    .trfcpb = 9,    .trfcpb_05T = 1,
    .txp = 0,   .txp_05T = 0,
    .trtp = 0,  .trtp_05T = 1,
    .trcd = 3,  .trcd_05T = 0,
    .twr = 6,   .twr_05T = 0,
    .twtr = 4,  .twtr_05T = 1,
    .tpbr2pbr = 7,  .tpbr2pbr_05T = 0,
    .tpbr2act = 0,  .tpbr2act_05T = 0,
    .tr2mrw = 9,    .tr2mrw_05T = 0,
    .tw2mrw = 6,    .tw2mrw_05T = 1,
    .tmrr2mrw = 7,  .tmrr2mrw_05T = 1,
    .tmrw = 3,  .tmrw_05T = 0,
    .tmrd = 3,  .tmrd_05T = 0,
    .tmrwckel = 4,  .tmrwckel_05T = 0,
    .tpde = 1,  .tpde_05T = 1,
    .tpdx = 1,  .tpdx_05T = 0,
    .tmrri = 4, .tmrri_05T = 0,
    .trrd = 1,  .trrd_05T = 0,
    .trrd_4266 = 0, .trrd_4266_05T = 1,
    .tfaw = 0,  .tfaw_05T = 0,
    .tfaw_4266 = 0, .tfaw_4266_05T = 0,
    .trtw_odt_off = 2,  .trtw_odt_off_05T = 0,
    .trtw_odt_on = 5,   .trtw_odt_on_05T = 0,
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
    .trcd_derate = 3,   .trcd_derate_05T = 0,
    .trc_derate = 1,    .trc_derate_05T = 0,
    .tras_derate = 0,   .tras_derate_05T = 0,
    .trpab_derate = 2,  .trpab_derate_05T = 0,
    .trp_derate = 1,    .trp_derate_05T = 1,
    .trrd_derate = 1,   .trrd_derate_05T = 0,
    .trtpd = 8, .trtpd_05T = 0,
    .twtpd = 9, .twtpd_05T = 0,
    .tmrr2w_odt_off = 4,
    .tmrr2w_odt_on = 6,
    .ckeprd = 1,
    .ckelckcnt = 2,
    .zqlat2 = 5,


    .dqsinctl = 2,   .datlat = 9
},
#endif
#if SUPPORT_LP4_DDR800_ACTIM

{
    .dramType = TYPE_LPDDR4, .freq = 400, .cbtMode = CBT_NORMAL_MODE, .readDBI = 0,
    .readLat = 14,  .writeLat =  8, .DivMode =  DIV4_MODE,

    .tras = 1,  .tras_05T = 0,
    .trp = 2,   .trp_05T = 0,
    .trpab = 3, .trpab_05T = 0,
    .trc = 3,   .trc_05T = 0,
    .trfc = 44, .trfc_05T = 0,
    .trfcpb = 16,   .trfcpb_05T = 0,
    .txp = 0,   .txp_05T = 0,
    .trtp = 3,  .trtp_05T = 0,
    .trcd = 4,  .trcd_05T = 0,
    .twr = 12,  .twr_05T = 0,
    .twtr = 10, .twtr_05T = 0,
    .tpbr2pbr = 11, .tpbr2pbr_05T = 0,
    .tpbr2act = 0,  .tpbr2act_05T = 0,
    .tr2mrw = 16,   .tr2mrw_05T = 0,
    .tw2mrw = 13,   .tw2mrw_05T = 0,
    .tmrr2mrw = 14, .tmrr2mrw_05T = 0,
    .tmrw = 6,  .tmrw_05T = 0,
    .tmrd = 6,  .tmrd_05T = 0,
    .tmrwckel = 8,  .tmrwckel_05T = 0,
    .tpde = 3,  .tpde_05T = 0,
    .tpdx = 3,  .tpdx_05T = 0,
    .tmrri = 7, .tmrri_05T = 0,
    .trrd = 1,  .trrd_05T = 0,
    .trrd_4266 = 1, .trrd_4266_05T = 0,
    .tfaw = 0,  .tfaw_05T = 0,
    .tfaw_4266 = 0, .tfaw_4266_05T = 0,
    .trtw_odt_off =5,  .trtw_odt_off_05T = 0,
    .trtw_odt_on = 9,  .trtw_odt_on_05T = 0,
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
    .hwset_mr2_op = 9,
    .hwset_mr13_op = 24,
    .hwset_vrcg_op = 16,
    .trcd_derate = 4,   .trcd_derate_05T = 0,
    .trc_derate = 4,    .trc_derate_05T = 0,
    .tras_derate = 1,   .tras_derate_05T = 0,
    .trpab_derate = 3,  .trpab_derate_05T = 0,
    .trp_derate = 2,    .trp_derate_05T = 0,
    .trrd_derate = 2,   .trrd_derate_05T = 0,
    .trtpd = 15,    .trtpd_05T = 0,
    .twtpd = 15,    .twtpd_05T = 0,
    .tmrr2w_odt_off = 10,
    .tmrr2w_odt_on = 12,
    .ckeprd = 2,
    .ckelckcnt = 3,
    .zqlat2 = 6,


    .dqsinctl = 4,   .datlat = 15
},

{
    .dramType = TYPE_LPDDR4, .freq = 400, .cbtMode = CBT_BYTE_MODE1, .readDBI = 0,
    .readLat = 16,  .writeLat =  8, .DivMode =  DIV4_MODE,

    .tras = 1,  .tras_05T = 0,
    .trp = 2,   .trp_05T = 0,
    .trpab = 3, .trpab_05T = 0,
    .trc = 3,   .trc_05T = 0,
    .trfc = 44, .trfc_05T = 0,
    .trfcpb = 16,   .trfcpb_05T = 0,
    .txp = 0,   .txp_05T = 0,
    .trtp = 3,  .trtp_05T = 0,
    .trcd = 4,  .trcd_05T = 0,
    .twr = 12,  .twr_05T = 0,
    .twtr = 10, .twtr_05T = 0,
    .tpbr2pbr = 11, .tpbr2pbr_05T = 0,
    .tpbr2act = 0,  .tpbr2act_05T = 0,
    .tr2mrw = 17,   .tr2mrw_05T = 0,
    .tw2mrw = 13,   .tw2mrw_05T = 0,
    .tmrr2mrw = 15, .tmrr2mrw_05T = 0,
    .tmrw = 6,  .tmrw_05T = 0,
    .tmrd = 6,  .tmrd_05T = 0,
    .tmrwckel = 8,  .tmrwckel_05T = 0,
    .tpde = 3,  .tpde_05T = 0,
    .tpdx = 3,  .tpdx_05T = 0,
    .tmrri = 7, .tmrri_05T = 0,
    .trrd = 1,  .trrd_05T = 0,
    .trrd_4266 = 1, .trrd_4266_05T = 0,
    .tfaw = 0,  .tfaw_05T = 0,
    .tfaw_4266 = 0, .tfaw_4266_05T = 0,
    .trtw_odt_off = 7,  .trtw_odt_off_05T = 0,
    .trtw_odt_on = 12,  .trtw_odt_on_05T = 0,
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
    .hwset_mr2_op = 9,
    .hwset_mr13_op = 24,
    .hwset_vrcg_op = 16,
    .trcd_derate = 4,   .trcd_derate_05T = 0,
    .trc_derate = 4,    .trc_derate_05T = 0,
    .tras_derate = 1,   .tras_derate_05T = 0,
    .trpab_derate = 3,  .trpab_derate_05T = 0,
    .trp_derate = 2,    .trp_derate_05T = 0,
    .trrd_derate = 2,   .trrd_derate_05T = 0,
    .trtpd = 16,    .trtpd_05T = 0,
    .twtpd = 15,    .twtpd_05T = 0,
    .tmrr2w_odt_off = 11,
    .tmrr2w_odt_on = 13,
    .ckeprd = 2,
    .ckelckcnt = 3,
    .zqlat2 = 6,


    .dqsinctl = 4,   .datlat = 15
},

{
    .dramType = TYPE_LPDDR4, .freq = 400, .cbtMode = CBT_NORMAL_MODE, .readDBI = 0,
    .readLat = 14,  .writeLat =  8, .DivMode =  DIV8_MODE,

    .tras = 0,  .tras_05T = 0,
    .trp = 0,   .trp_05T = 1,
    .trpab = 1, .trpab_05T = 0,
    .trc = 0,   .trc_05T = 0,
    .trfc = 16, .trfc_05T = 0,
    .trfcpb = 2,    .trfcpb_05T = 0,
    .txp = 0,   .txp_05T = 0,
    .trtp = 0,  .trtp_05T = 1,
    .trcd = 2,  .trcd_05T = 0,
    .twr = 5,   .twr_05T = 0,
    .twtr = 4,  .twtr_05T = 1,
    .tpbr2pbr = 2,  .tpbr2pbr_05T = 0,
    .tpbr2act = 0,  .tpbr2act_05T = 0,
    .tr2mrw = 8,    .tr2mrw_05T = 0,
    .tw2mrw = 6,    .tw2mrw_05T = 1,
    .tmrr2mrw = 7,  .tmrr2mrw_05T = 0,
    .tmrw = 3,  .tmrw_05T = 0,
    .tmrd = 3,  .tmrd_05T = 0,
    .tmrwckel = 4,  .tmrwckel_05T = 0,
    .tpde = 1,  .tpde_05T = 1,
    .tpdx = 1,  .tpdx_05T = 0,
    .tmrri = 3, .tmrri_05T = 1,
    .trrd = 0,  .trrd_05T = 0,
    .trrd_4266 = 0, .trrd_4266_05T = 0,
    .tfaw = 0,  .tfaw_05T = 0,
    .tfaw_4266 = 0, .tfaw_4266_05T = 0,
    .trtw_odt_off = 1,  .trtw_odt_off_05T = 0,
    .trtw_odt_on = 4,   .trtw_odt_on_05T = 0,
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
    .trcd_derate = 2,   .trcd_derate_05T = 0,
    .trc_derate = 0,    .trc_derate_05T = 0,
    .tras_derate = 0,   .tras_derate_05T = 0,
    .trpab_derate = 1,  .trpab_derate_05T = 0,
    .trp_derate = 0,    .trp_derate_05T = 1,
    .trrd_derate = 0,   .trrd_derate_05T = 1,
    .trtpd = 7, .trtpd_05T = 1,
    .twtpd = 7, .twtpd_05T = 1,
    .tmrr2w_odt_off = 3,
    .tmrr2w_odt_on = 5,
    .ckeprd = 1,
    .ckelckcnt = 2,
    .zqlat2 = 3,


    .dqsinctl = 5,   .datlat = 15
},

{
    .dramType = TYPE_LPDDR4, .freq = 400, .cbtMode = CBT_BYTE_MODE1, .readDBI = 0,
    .readLat = 16,  .writeLat =  8, .DivMode =  DIV8_MODE,

    .tras = 0,  .tras_05T = 0,
    .trp = 0,   .trp_05T = 1,
    .trpab = 1, .trpab_05T = 0,
    .trc = 0,   .trc_05T = 0,
    .trfc = 16, .trfc_05T = 0,
    .trfcpb = 2,    .trfcpb_05T = 0,
    .txp = 0,   .txp_05T = 0,
    .trtp = 0,  .trtp_05T = 1,
    .trcd = 2,  .trcd_05T = 0,
    .twr = 5,   .twr_05T = 0,
    .twtr = 4,  .twtr_05T = 1,
    .tpbr2pbr = 2,  .tpbr2pbr_05T = 0,
    .tpbr2act = 0,  .tpbr2act_05T = 0,
    .tr2mrw = 8,    .tr2mrw_05T = 1,
    .tw2mrw = 6,    .tw2mrw_05T = 1,
    .tmrr2mrw = 7,  .tmrr2mrw_05T = 1,
    .tmrw = 3,  .tmrw_05T = 0,
    .tmrd = 3,  .tmrd_05T = 0,
    .tmrwckel = 4,  .tmrwckel_05T = 0,
    .tpde = 1,  .tpde_05T = 1,
    .tpdx = 1,  .tpdx_05T = 0,
    .tmrri = 3, .tmrri_05T = 1,
    .trrd = 0,  .trrd_05T = 0,
    .trrd_4266 = 0, .trrd_4266_05T = 0,
    .tfaw = 0,  .tfaw_05T = 0,
    .tfaw_4266 = 0, .tfaw_4266_05T = 0,
    .trtw_odt_off = 2,  .trtw_odt_off_05T = 0,
    .trtw_odt_on = 4,   .trtw_odt_on_05T = 0,
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
    .trcd_derate = 2,   .trcd_derate_05T = 0,
    .trc_derate = 0,    .trc_derate_05T = 0,
    .tras_derate = 0,   .tras_derate_05T = 0,
    .trpab_derate = 1,  .trpab_derate_05T = 0,
    .trp_derate = 0,    .trp_derate_05T = 1,
    .trrd_derate = 0,   .trrd_derate_05T = 1,
    .trtpd = 8, .trtpd_05T = 0,
    .twtpd = 7, .twtpd_05T = 1,
    .tmrr2w_odt_off = 4,
    .tmrr2w_odt_on = 6,
    .ckeprd = 1,
    .ckelckcnt = 2,
    .zqlat2 = 3,


    .dqsinctl = 5,   .datlat = 15
},
#endif
#if SUPPORT_LP4_DDR400_ACTIM

{
    .dramType = TYPE_LPDDR4, .freq = 200, .cbtMode = CBT_NORMAL_MODE, .readDBI = 0,
    .readLat = 14,  .writeLat =  8, .DivMode =  DIV4_MODE,

    .tras = 0,  .tras_05T = 0,
    .trp = 0,   .trp_05T = 0,
    .trpab = 1, .trpab_05T = 0,
    .trc = 0,   .trc_05T = 0,
    .trfc = 16, .trfc_05T = 0,
    .trfcpb = 2,    .trfcpb_05T = 0,
    .txp = 0,   .txp_05T = 0,
    .trtp = 3,  .trtp_05T = 0,
    .trcd = 2,  .trcd_05T = 0,
    .twr = 11,  .twr_05T = 0,
    .twtr = 10, .twtr_05T = 0,
    .tpbr2pbr = 2,  .tpbr2pbr_05T = 0,
    .tpbr2act = 0,  .tpbr2act_05T = 0,
    .tr2mrw = 16,   .tr2mrw_05T = 0,
    .tw2mrw = 13,   .tw2mrw_05T = 0,
    .tmrr2mrw = 13, .tmrr2mrw_05T = 0,
    .tmrw = 6,  .tmrw_05T = 0,
    .tmrd = 6,  .tmrd_05T = 0,
    .tmrwckel = 8,  .tmrwckel_05T = 0,
    .tpde = 3,  .tpde_05T = 0,
    .tpdx = 3,  .tpdx_05T = 0,
    .tmrri = 5, .tmrri_05T = 0,
    .trrd = 1,  .trrd_05T = 0,
    .trrd_4266 = 1, .trrd_4266_05T = 0,
    .tfaw = 0,  .tfaw_05T = 0,
    .tfaw_4266 = 0, .tfaw_4266_05T = 0,
    .trtw_odt_off = 6,  .trtw_odt_off_05T = 0,
    .trtw_odt_on = 10,  .trtw_odt_on_05T = 0,
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
    .hwset_mr2_op = 0,
    .hwset_mr13_op = 24,
    .hwset_vrcg_op = 16,
    .trcd_derate = 2,   .trcd_derate_05T = 0,
    .trc_derate = 0,    .trc_derate_05T = 0,
    .tras_derate = 0,   .tras_derate_05T = 0,
    .trpab_derate = 1,  .trpab_derate_05T = 0,
    .trp_derate = 0,    .trp_derate_05T = 0,
    .trrd_derate = 1,   .trrd_derate_05T = 0,
    .trtpd = 14,    .trtpd_05T = 0,
    .twtpd = 14,    .twtpd_05T = 0,
    .tmrr2w_odt_off = 9,
    .tmrr2w_odt_on = 11,
    .ckeprd = 2,
    .ckelckcnt = 3,
    .zqlat2 = 4,


    .dqsinctl = 2,   .datlat = 15
},

{
    .dramType = TYPE_LPDDR4, .freq = 200, .cbtMode = CBT_BYTE_MODE1, .readDBI = 0,
    .readLat = 16,  .writeLat =  8, .DivMode =  DIV4_MODE,

    .tras = 0,  .tras_05T = 0,
    .trp = 0,   .trp_05T = 0,
    .trpab = 1, .trpab_05T = 0,
    .trc = 0,   .trc_05T = 0,
    .trfc = 16, .trfc_05T = 0,
    .trfcpb = 2,    .trfcpb_05T = 0,
    .txp = 0,   .txp_05T = 0,
    .trtp = 3,  .trtp_05T = 0,
    .trcd = 2,  .trcd_05T = 0,
    .twr = 11,  .twr_05T = 0,
    .twtr = 10, .twtr_05T = 0,
    .tpbr2pbr = 2,  .tpbr2pbr_05T = 0,
    .tpbr2act = 0,  .tpbr2act_05T = 0,
    .tr2mrw = 17,   .tr2mrw_05T = 0,
    .tw2mrw = 13,   .tw2mrw_05T = 0,
    .tmrr2mrw = 14, .tmrr2mrw_05T = 0,
    .tmrw = 6,  .tmrw_05T = 0,
    .tmrd = 6,  .tmrd_05T = 0,
    .tmrwckel = 8,  .tmrwckel_05T = 0,
    .tpde = 3,  .tpde_05T = 0,
    .tpdx = 3,  .tpdx_05T = 0,
    .tmrri = 5, .tmrri_05T = 0,
    .trrd = 1,  .trrd_05T = 0,
    .trrd_4266 = 1, .trrd_4266_05T = 0,
    .tfaw = 0,  .tfaw_05T = 0,
    .tfaw_4266 = 0, .tfaw_4266_05T = 0,
    .trtw_odt_off = 7,  .trtw_odt_off_05T = 0,
    .trtw_odt_on = 11,  .trtw_odt_on_05T = 0,
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
    .hwset_mr2_op = 0,
    .hwset_mr13_op = 24,
    .hwset_vrcg_op = 16,
    .trcd_derate = 2,   .trcd_derate_05T = 0,
    .trc_derate = 0,    .trc_derate_05T = 0,
    .tras_derate = 0,   .tras_derate_05T = 0,
    .trpab_derate = 1,  .trpab_derate_05T = 0,
    .trp_derate = 0,    .trp_derate_05T = 0,
    .trrd_derate = 1,   .trrd_derate_05T = 0,
    .trtpd = 15,    .trtpd_05T = 0,
    .twtpd = 14,    .twtpd_05T = 0,
    .tmrr2w_odt_off = 10,
    .tmrr2w_odt_on = 12,
    .ckeprd = 2,
    .ckelckcnt = 3,
    .zqlat2 = 4,


    .dqsinctl = 2,   .datlat = 15
},
#endif
};

