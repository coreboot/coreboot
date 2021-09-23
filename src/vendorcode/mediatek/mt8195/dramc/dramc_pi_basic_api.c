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
#if (FOR_DV_SIMULATION_USED==0)
#include "dramc_top.h"
#include "pll.h"
//extern EMI_SETTINGS g_default_emi_setting;
#endif

//#include "DramC_reg.h"
//#include "System_reg.h"
//#include "string.h"

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
U8 u1PrintModeRegWrite = 0;

#if ENABLE_RODT_TRACKING_SAVE_MCK
// global variables for RODT tracking & ROEN
U8 u1ODT_ON;        // infor of p->odt_onoff
U8 u1WDQS_ON = 0;   // infor of WDQS on(ROEN=1)
U8 u1RODT_TRACK = 0;  // infor of rodt tracking enable
U8 u1ROEN, u1ModeSel;//status of ROEN, MODESEL setting
#endif

//MRR DRAM->DRAMC
const U8 uiLPDDR4_MRR_DRAM_Pinmux[PINMUX_MAX][CHANNEL_NUM][16] =
{
    {
        // for EMCP
        //CH-A
        {
            0, 1, 2, 3, 5, 7, 6, 4,
            9, 8, 13, 15, 10, 14, 11, 12
        },
#if (CHANNEL_NUM>1)
        //CH-B
        {
            0, 1, 5, 4, 3, 7, 6, 2,
            9, 8, 13, 14, 10, 15, 11, 12
        },
#endif
#if (CHANNEL_NUM>2)
        //CH-C
        {
            0, 1, 2, 3, 5, 7, 6, 4,
            9, 8, 13, 15, 10, 14, 11, 12
        },
        //CH-D
        {
            0, 1, 5, 4, 3, 7, 6, 2,
            9, 8, 13, 14, 10, 15, 11, 12
        },
#endif
    },
    {
    // for DSC_2CH, HFID RESERVED
        //CH-A
        {
            0, 1, 4, 3, 2, 5, 7, 6,
            9, 8, 10, 11, 14, 13, 15, 12
        },
#if (CHANNEL_NUM>1)
        //CH-B
        {
            0, 1, 2, 4, 5, 3, 7, 6,
            8, 9, 10, 11, 15, 14, 13, 12
        },
#endif
#if (CHANNEL_NUM>2)
        //CH-C
        {
            0, 1, 2, 3, 4, 5, 6, 7,
            8, 9, 10, 11, 12, 13, 14, 15
        },
        //CH-D
        {
            0, 1, 2, 3, 4, 5, 6, 7,
            8, 9, 10, 11, 12, 13, 14, 15
        },
#endif
    },
    {
        // for MCP
        //CH-A
        {
            0, 1, 3, 6, 4, 7, 2, 5,
            8, 9, 10, 13, 11, 12, 15, 14
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
    {
    // for DSC_180
        //CH-A
        {
            9, 8, 11, 10, 14, 15, 13, 12,
            0, 1, 7, 6, 4, 5, 2, 3
        },
#if (CHANNEL_NUM>1)
        //CH-B
        {
            8, 9, 11, 10, 12, 14, 13, 15,
            1, 0, 5, 6, 3, 2, 7, 4
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
};


//MRR DRAM->DRAMC
U8 uiLPDDR4_MRR_Mapping_POP[CHANNEL_NUM][16] =
{
    //CH-A
    {
		0, 1, 2, 3, 7, 4, 6, 5,
		9, 8, 12, 14, 15, 10, 13, 11
    },
#if (CHANNEL_NUM>1)
    //CH-B
    {
		0, 1, 7, 4, 3, 2, 6, 5,
		9, 8, 12, 14, 15, 10, 11, 13
    },
#endif
#if (CHANNEL_NUM>2)
    //CH-C
    {
		0, 1, 2, 3, 7, 4, 6, 5,
		9, 8, 12, 14, 15, 10, 13, 11
    },
    //CH-D
    {
		0, 1, 7, 4, 3, 2, 6, 5,
		9, 8, 12, 14, 15, 10, 11, 13
    },
#endif
};

#if (fcFOR_CHIP_ID == fc8195)
static void Set_DRAM_Pinmux_Sel(DRAMC_CTX_T *p)
{

#if !FOR_DV_SIMULATION_USED
    if (is_discrete_lpddr4())
        p->DRAMPinmux = PINMUX_DSC;
    else
#endif
        p->DRAMPinmux = PINMUX_EMCP;

    mcSHOW_DBG_MSG2(("[Set_DRAM_Pinmux_Sel] DRAMPinmux = %d\n", p->DRAMPinmux));

    vIO32WriteFldAlign(DRAMC_REG_SA_RESERVE, p->DRAMPinmux, SA_RESERVE_DRM_DSC_DRAM);
    memcpy(&uiLPDDR4_MRR_Mapping_POP, uiLPDDR4_MRR_DRAM_Pinmux[p->DRAMPinmux], sizeof(uiLPDDR4_MRR_Mapping_POP));
    memcpy(&uiLPDDR4_O1_Mapping_POP, uiLPDDR4_O1_DRAM_Pinmux[p->DRAMPinmux], sizeof(uiLPDDR4_O1_Mapping_POP));
    memcpy(&uiLPDDR4_CA_Mapping_POP, uiLPDDR4_CA_DRAM_Pinmux[p->DRAMPinmux], sizeof(uiLPDDR4_CA_Mapping_POP));
}
#endif

static void Set_MRR_Pinmux_Mapping(DRAMC_CTX_T *p)
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


static void Set_DQO1_Pinmux_Mapping(DRAMC_CTX_T *p)
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

    emi_set = &g_default_emi_setting;

    u4value = ((emi_set->EMI_CONA_VAL >> 17) & 0x1)? 0: 1;//CONA 17th bit 0: Disable dual rank mode 1: Enable dual rank mode

    vIO32WriteFldAlign(DRAMC_REG_SA_RESERVE, u4value, SA_RESERVE_SINGLE_RANK);

    mcSHOW_JV_LOG_MSG(("Rank info: %d, CONA[0x%x]\n", u4value, emi_set->EMI_CONA_VAL));
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
#if 0
static void UpdateHighestFreqInDFSTbl(DRAMC_CTX_T *p, DRAM_PLL_FREQ_SEL_T new_freq_sel)
{
#if(FOR_DV_SIMULATION_USED==0 && SW_CHANGE_FOR_SIMULATION==0)
    U16 u2HighestFreq = u2DFSGetHighestFreq(p);
    DRAM_PLL_FREQ_SEL_T cur_freq_sel = 0;
    U8 u1ShuffleIdx = 0;

    // lookup table to find highest freq
    cur_freq_sel = GetSelByFreq(p, u2HighestFreq);
    if (cur_freq_sel == new_freq_sel)
        return;

    for (u1ShuffleIdx = 0; u1ShuffleIdx < DRAM_DFS_SRAM_MAX; u1ShuffleIdx++)
        if (gFreqTbl[u1ShuffleIdx].freq_sel == cur_freq_sel)
            break;

    gFreqTbl[u1ShuffleIdx].freq_sel = new_freq_sel; // update

    gUpdateHighestFreq = TRUE;
    u2HighestFreq = u2DFSGetHighestFreq(p); // @Darren, Update u2FreqMax variables

    #if __ETT__
    UpdateEttDFVSTblHighest(p, cur_freq_sel, new_freq_sel); //@Darren, Update for ETT DVFS stress
    #endif
    mcSHOW_DBG_MSG2(("[UpdateHighestFreqInDFSTbl] Get Highest Freq is %d\n", u2HighestFreq));
#endif
}

static void UpdateHighestFreqToDDR3733(DRAMC_CTX_T *p)
{
#if (FOR_DV_SIMULATION_USED == 0 && SW_CHANGE_FOR_SIMULATION == 0)
    EMI_SETTINGS *emi_set;

    emi_set = &g_default_emi_setting;

    if (emi_set->highest_freq == 3733) {
        UpdateHighestFreqInDFSTbl(p, LP4_DDR3733);
    }
#endif
}
#endif
#ifdef SCRAMBLE_EN
#endif
#if 0
static void TzCfgScramble(DRAMC_CTX_T *p, U32 u4RandmKey, U32 u4EncryptType)
{
    U32 u4ScrambleCFGAddr = DRAMC_REG_SCRAMBLE_CFG0;
    U32 u4ScrambleCFGRGIdx = 0;

    for(u4ScrambleCFGRGIdx = 0; u4ScrambleCFGRGIdx < 8; u4ScrambleCFGRGIdx++)
    {
        u4ScrambleCFGAddr += 4;
        vIO32Write4B(DRAMC_REG_ADDR(u4ScrambleCFGAddr), u4RandmKey);
    }

    vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SCRAMBLE_CFG8), P_Fld(1, SCRAMBLE_CFG8_SC_CMP_EN0) \
                                                              | P_Fld(0, SCRAMBLE_CFG8_SC_CMP_TYPE0));
    vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_SCRAMBLE_CFG9), 0xFFFF0000);
    vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SCRAMBLE_CFG8), P_Fld(u4EncryptType, SCRAMBLE_CFG8_SC_RETRUN_HUSKY) \
                                                              | P_Fld(1, SCRAMBLE_CFG8_SC_DDR_TYPE)
                                                              | P_Fld(1, SCRAMBLE_CFG8_SC_EN));
}
#endif


#if defined(DEVIATION) && !__ETT__
#include "deviation.h"
#define KEY_DEVIATION 0x4073
static void vInitDeviationVariable(void)
{
    DEVIATION_INFO_T* deviation_info_ptr;

    deviation_info_ptr = (DEVIATION_INFO_T *) get_dbg_info_base(KEY_DEVIATION);

    if (deviation_info_ptr->magic == DEVIATION_MAGIC_PATTERN)
    {
        if (deviation_info_ptr->ca_enable == ENABLE)
        {
            gSetSpecificedVref_Enable[0] = ENABLE;

            /*
                CH_BA : (0: ch A + rank 0)
                CH_BA : (1: ch A + rank 1)
                CH_BA : (2: ch B + rank 0)
                CH_BA : (3: ch B + rank 1)
                CH_BA : (4: All Channel and All Rank)
            */
            if (deviation_info_ptr->ca_channel_bank == 4)
            {
                gSetSpecificedVref_All_ChRk[0] = ENABLE;
            }
            else
            {
                gSetSpecificedVref_Channel[0] = (deviation_info_ptr->ca_channel_bank >> 1) & 1;
                gSetSpecificedVref_Rank[0] = deviation_info_ptr->ca_channel_bank & 1;
            }
            gSetSpecificedVref_Vref_Offset[0] = deviation_info_ptr->ca_offset;
        }

        if (deviation_info_ptr->rx_enable == ENABLE)
        {
            gSetSpecificedVref_Enable[1] = ENABLE;

            if (deviation_info_ptr->rx_channel_bank == 4)
            {
                gSetSpecificedVref_All_ChRk[1] = ENABLE;
            }
            else
            {
                gSetSpecificedVref_Channel[1] = (deviation_info_ptr->rx_channel_bank >> 1) & 1;
                gSetSpecificedVref_Rank[1] = deviation_info_ptr->rx_channel_bank & 1;
            }
            gSetSpecificedVref_Vref_Offset[1] = deviation_info_ptr->rx_offset;
        }

        if (deviation_info_ptr->tx_enable == ENABLE)
        {
            gSetSpecificedVref_Enable[2] = ENABLE;

            if (deviation_info_ptr->tx_channel_bank == 4)
            {
                gSetSpecificedVref_All_ChRk[2] = ENABLE;
            }
            else
            {
                gSetSpecificedVref_Channel[2] = (deviation_info_ptr->tx_channel_bank >> 1) & 1;
                gSetSpecificedVref_Rank[2] = deviation_info_ptr->tx_channel_bank & 1;
            }
            gSetSpecificedVref_Vref_Offset[2] = deviation_info_ptr->tx_offset;
        }
    }
}

void vSetDeviationVariable(void)
{
    DEVIATION_INFO_T* deviation_info_ptr;
    U8 u1ChannelIdx, u1RankIdx, u1ByteIdx;

    deviation_info_ptr = (DEVIATION_INFO_T *) get_dbg_info_base(KEY_DEVIATION);

    for(u1ChannelIdx=0; u1ChannelIdx<2; u1ChannelIdx++)
    {
        for(u1RankIdx=0; u1RankIdx<2; u1RankIdx++)
        {
            deviation_info_ptr->dram_k_ca_vref_range[u1ChannelIdx][u1RankIdx] = (u1MR12Value[u1ChannelIdx][u1RankIdx][FSP_1]>>6) & 1;
            deviation_info_ptr->dram_k_ca_vref_value[u1ChannelIdx][u1RankIdx] = u1MR12Value[u1ChannelIdx][u1RankIdx][FSP_1] & 0x3f;

            deviation_info_ptr->dram_k_tx_vref_range[u1ChannelIdx][u1RankIdx] = (u1MR14Value[u1ChannelIdx][u1RankIdx][FSP_1]>>6)&1;
            deviation_info_ptr->dram_k_tx_vref_value[u1ChannelIdx][u1RankIdx] = u1MR14Value[u1ChannelIdx][u1RankIdx][FSP_1] & 0x3f;

            for(u1ByteIdx=0; u1ByteIdx<2; u1ByteIdx++)
            {
                deviation_info_ptr->dram_k_rx_vref_value[u1ChannelIdx][u1RankIdx][u1ByteIdx] = gFinalRXVrefDQ[u1ChannelIdx][u1RankIdx][u1ByteIdx];
            }
        }
    }
}
#endif

void Global_Option_Init(DRAMC_CTX_T *p)
{
    //SaveCurrDramCtx(p);
    vSetChannelNumber(p);
    SetRankInfoToConf(p);
    vSetRankNumber(p);
    vSetFSPNumber(p);
#if (fcFOR_CHIP_ID == fc8195)
    Set_DRAM_Pinmux_Sel(p);
#endif
    Set_MRR_Pinmux_Mapping(p);
    Set_DQO1_Pinmux_Mapping(p);

    vInitGlobalVariablesByCondition(p);

#if ENABLE_TX_TRACKING
    DramcDQSOSCInit();
#endif

#ifdef FOR_HQA_TEST_USED
    HQA_measure_message_reset_all_data(p);
#endif

#if defined(DEVIATION) && !__ETT__
    vInitDeviationVariable();
#endif
}

/* RxDQSIsiPulseCG() - API for "RX DQS ISI pulse CG function" 0: disable, 1: enable
 * 1. RG_*_RPRE_TOG_EN (16nm APHY): B0_DQ6_RG_RX_ARDQ_RPRE_TOG_EN_B0, B1_DQ6_RG_RX_ARDQ_RPRE_TOG_EN_B1
 * 2. RG_RX_*RDQS*_DQSSTB_CG_EN*(10nm APHY): B0_DQ8_RG_RX_ARDQS_DQSSTB_CG_EN_B0, B1_DQ8_RG_RX_ARDQS_DQSSTB_CG_EN_B1
 * Supports setting current channel only, add function to set "all channels" in the future
 */
#if 0
static void RxDQSIsiPulseCG(DRAMC_CTX_T *p, U8 u1OnOff)
{
#if __A60868_TO_BE_PORTING__

    mcSHOW_DBG_MSG4(("CH%u RX DQS ISI pulse CG: %u (0:disable, 1:enable)\n", u1OnOff));

    /* LP4: Disable(set to 0) "RX DQS ISI pulse CG function" during the below senarios (must enable(set to 1) when done)
     *      1. Gating window calibration
     *      2. Duty related calibration (Justin: prevents DQSI from being kept high after READ burst)
     * LP3: should always be set to 1
     */
#if (fcFOR_CHIP_ID == fcA60868)
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ6), u1OnOff, B0_DQ6_RG_RX_ARDQ_RPRE_TOG_EN_B0);
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ6), u1OnOff, B1_DQ6_RG_RX_ARDQ_RPRE_TOG_EN_B1);
#else
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ10), u1OnOff, SHU_B0_DQ10_RG_RX_ARDQS_DQSSTB_CG_EN_B0);
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ10), u1OnOff, SHU_B1_DQ10_RG_RX_ARDQS_DQSSTB_CG_EN_B1);
#endif

    return;
#endif
}
#endif
#if 0
static void OpenLoopModeSetting(DRAMC_CTX_T * p, DDR800_MODE_T eDDR800Mode)
{
#if __A60868_TO_BE_PORTING__

    //Enable SHU option for MCK8X_EN and PHDET_EN
    if (eDDR800Mode == DDR800_OPEN_LOOP)
    {
        vIO32WriteFldMulti_All(DDRPHY_MISC_CG_CTRL6, P_Fld(0x1, MISC_CG_CTRL6_RG_M_CK_OPENLOOP_MODE_EN)
                    | P_Fld(0x1, MISC_CG_CTRL6_RG_MCK4X_I_OPENLOOP_MODE_EN)
                    | P_Fld(0x1, MISC_CG_CTRL6_RG_CG_DDR400_MCK4X_I_OFF)
                    | P_Fld(0x0, MISC_CG_CTRL6_RG_DDR400_MCK4X_I_FORCE_ON)
                    | P_Fld(0x1, MISC_CG_CTRL6_RG_MCK4X_I_FB_CK_CG_OFF)
                    | P_Fld(0x1, MISC_CG_CTRL6_RG_MCK4X_Q_OPENLOOP_MODE_EN)
                    | P_Fld(0x1, MISC_CG_CTRL6_RG_CG_DDR400_MCK4X_Q_OFF)
                    | P_Fld(0x0, MISC_CG_CTRL6_RG_DDR400_MCK4X_Q_FORCE_ON)
                    | P_Fld(0x1, MISC_CG_CTRL6_RG_MCK4X_Q_FB_CK_CG_OFF)
                    | P_Fld(0x1, MISC_CG_CTRL6_RG_MCK4X_O_OPENLOOP_MODE_EN)
                    | P_Fld(0x1, MISC_CG_CTRL6_RG_CG_DDR400_MCK4X_O_OFF)
                    | P_Fld(0x0, MISC_CG_CTRL6_RG_DDR400_MCK4X_O_FORCE_ON)
                    | P_Fld(0x1, MISC_CG_CTRL6_RG_MCK4X_O_FB_CK_CG_OFF));

        vIO32WriteFldAlign_All(DDRPHY_SHU_MISC2, 1, SHU_MISC2_M_CK_OPENLOOP_MODE_SEL);
    }
#endif
}
#endif
#if __A60868_TO_BE_PORTING__
#endif
#if 0
static void APhyModeSetting(DRAMC_CTX_T * p, DDR800_MODE_T eMode, U8 *uDLL1, U8 *uMode, U8 *uDLL0)
{
#if __A60868_TO_BE_PORTING__

    DDR800Mode_T DDR800Mode;

    OpenLoopModeSetting(p, eMode);

    if ((eMode == DDR800_CLOSE_LOOP) || (eMode == NORMAL_CLOSE_LOOP))
    {
        DDR800Mode.phypll_ddr400_en = 0;
        DDR800Mode.ddr400_en_b0 = 0;
        DDR800Mode.ddr400_en_b1 = 0;
        DDR800Mode.ddr400_en_ca = 0;
        DDR800Mode.dll_phdet_en_b0 = 1;
        DDR800Mode.dll_phdet_en_b1 = 1;
        DDR800Mode.dll_phdet_en_ca_cha = 1;
        DDR800Mode.dll_phdet_en_ca_chb = 1;
        DDR800Mode.phypll_ada_mck8x_en = 1;
        DDR800Mode.ddr400_dqs_ps_b0 = 0;
        DDR800Mode.ddr400_dqs_ps_b1 = 0;
        DDR800Mode.ddr400_dq_ps_b0 = 0;
        DDR800Mode.ddr400_dq_ps_b1 = 0;
        DDR800Mode.ddr400_dqs_ps_ca = 0;
        DDR800Mode.ddr400_dq_ps_ca = 0;
        DDR800Mode.ddr400_semi_en_b0 = 0;
        DDR800Mode.ddr400_semi_en_b1 = 0;
        DDR800Mode.ddr400_semi_en_ca = 0;
        DDR800Mode.ddr400_semi_open_en = 0;
        DDR800Mode.pll0_ada_mck8x_chb_en = 1;
        DDR800Mode.pll0_ada_mck8x_cha_en = 1;

        if (p->frequency <= 400) //DDR800 1:4 mode
        {
            *uDLL1 = 1;
            *uMode = 0;
            *uDLL0 = 2;
        }
        else if (p->frequency <= 800)
        {
            if (vGet_Div_Mode(p) == DIV8_MODE) // DDR1600 1:8 mode
            {
                *uDLL1 = 0;
                *uMode = 0;
                *uDLL0 = 2;
            }
            else // DDR1600 1:4 mode
            {
                *uDLL1 = 0;
                *uMode = 1;
                *uDLL0 = 0;
            }
        }
        else // 1:8 mode
        {
            *uDLL1 = 0;
            *uMode = 0;
            *uDLL0 = 2;
        }
    }
    else if (eMode == DDR800_OPEN_LOOP) // For DDR800
    {
        DDR800Mode.phypll_ddr400_en = 1;
        DDR800Mode.ddr400_en_b0 = 1;
        DDR800Mode.ddr400_en_b1 = 1;
        DDR800Mode.ddr400_en_ca = 1;
        DDR800Mode.dll_phdet_en_b0 = 0;
        DDR800Mode.dll_phdet_en_b1 = 0;
        DDR800Mode.dll_phdet_en_ca_cha = 0;
        DDR800Mode.dll_phdet_en_ca_chb = 0;
        DDR800Mode.phypll_ada_mck8x_en = 0;
#if (FOR_DV_SIMULATION_USED == 0 && SW_CHANGE_FOR_SIMULATION == 0)
        DDR800Mode.ddr400_dqs_ps_b0 = 0;  // 2'b00-0 degree, 2'b01-45 degree, 2'b10-90 degree
        DDR800Mode.ddr400_dqs_ps_b1 = 0;
        DDR800Mode.ddr400_dq_ps_b0 = 0;
        DDR800Mode.ddr400_dq_ps_b1 = 0;
        DDR800Mode.ddr400_dqs_ps_ca = 0; // clk
        DDR800Mode.ddr400_dq_ps_ca = 0;  // ca
#else
        DDR800Mode.ddr400_dqs_ps_b0 = 1;
        DDR800Mode.ddr400_dqs_ps_b1 = 1;
        DDR800Mode.ddr400_dq_ps_b0 = 0;
        DDR800Mode.ddr400_dq_ps_b1 = 0;
        DDR800Mode.ddr400_dqs_ps_ca = 1;
        DDR800Mode.ddr400_dq_ps_ca = 2;
#endif
        DDR800Mode.ddr400_semi_en_b0 = 0;
        DDR800Mode.ddr400_semi_en_b1 = 0;
        DDR800Mode.ddr400_semi_en_ca = 0;
        DDR800Mode.ddr400_semi_open_en = 0;
        DDR800Mode.pll0_ada_mck8x_chb_en = 0;
        DDR800Mode.pll0_ada_mck8x_cha_en = 0;
        *uDLL1 = 0;
        *uMode = 1;
        *uDLL0 = 0;
    }
    else if (eMode == DDR800_SEMI_LOOP) // For DDR800
    {
        DDR800Mode.phypll_ddr400_en = 1;
        DDR800Mode.ddr400_en_b0 = 1;
        DDR800Mode.ddr400_en_b1 = 1;
        DDR800Mode.ddr400_en_ca = 1;
        DDR800Mode.dll_phdet_en_b0 = 0;
        DDR800Mode.dll_phdet_en_b1 = 0;
        DDR800Mode.dll_phdet_en_ca_cha = 1;
        DDR800Mode.dll_phdet_en_ca_chb = 0;
        DDR800Mode.phypll_ada_mck8x_en = 1;
#if (FOR_DV_SIMULATION_USED == 0 && SW_CHANGE_FOR_SIMULATION == 0)
        DDR800Mode.ddr400_dqs_ps_b0 = 0;  // 2'b00-0 degree, 2'b01-45 degree, 2'b10-90 degree
        DDR800Mode.ddr400_dqs_ps_b1 = 0;
        DDR800Mode.ddr400_dq_ps_b0 = 0;
        DDR800Mode.ddr400_dq_ps_b1 = 0;
        DDR800Mode.ddr400_dqs_ps_ca = 0; // clk
        DDR800Mode.ddr400_dq_ps_ca = 0;  // ca
#else
        DDR800Mode.ddr400_dqs_ps_b0 = 1;
        DDR800Mode.ddr400_dqs_ps_b1 = 1;
        DDR800Mode.ddr400_dq_ps_b0 = 0;
        DDR800Mode.ddr400_dq_ps_b1 = 0;
        DDR800Mode.ddr400_dqs_ps_ca = 1;
        DDR800Mode.ddr400_dq_ps_ca = 2;
#endif
        DDR800Mode.ddr400_semi_en_b0 = 1;
        DDR800Mode.ddr400_semi_en_b1 = 1;
        DDR800Mode.ddr400_semi_en_ca = 1;
        DDR800Mode.ddr400_semi_open_en = 1;
        DDR800Mode.pll0_ada_mck8x_chb_en = 0;
        DDR800Mode.pll0_ada_mck8x_cha_en = 1;
        *uDLL1 = 0;
        *uMode = 1;
        *uDLL0 = 2;
    }
    else // Others
    {
        mcSHOW_ERR_MSG(("[FAIL] APhy mode incorrect !!!\n"));
        #if __ETT__
        while (1);
        #endif
    }

    // Enable DDR800 RG
    vIO32WriteFldAlign(DDRPHY_SHU_PLL1, DDR800Mode.phypll_ddr400_en, SHU_PLL1_RG_RPHYPLL_DDR400_EN); // CHA only
    //vIO32WriteFldAlign_All(DDRPHY_PLL4, DDR800Mode.phypll_ada_mck8x_en, PLL4_RG_RPHYPLL_ADA_MCK8X_EN);
    vIO32WriteFldAlign_All(DDRPHY_SHU_B0_DQ6, DDR800Mode.ddr400_en_b0, SHU_B0_DQ6_RG_ARPI_DDR400_EN_B0);
    vIO32WriteFldAlign_All(DDRPHY_SHU_B1_DQ6, DDR800Mode.ddr400_en_b1, SHU_B1_DQ6_RG_ARPI_DDR400_EN_B1);
    vIO32WriteFldAlign_All(DDRPHY_SHU_CA_CMD6, DDR800Mode.ddr400_en_ca, SHU_CA_CMD6_RG_ARPI_DDR400_EN_CA);

    // DLL & Clock
    vIO32WriteFldAlign_All(DDRPHY_SHU_B0_DLL0, DDR800Mode.dll_phdet_en_b0, SHU_B0_DLL0_RG_ARDLL_PHDET_EN_B0_SHU);
    vIO32WriteFldAlign_All(DDRPHY_SHU_B1_DLL0, DDR800Mode.dll_phdet_en_b1, SHU_B1_DLL0_RG_ARDLL_PHDET_EN_B1_SHU);
    vIO32WriteFldAlign(DDRPHY_SHU_CA_DLL0, DDR800Mode.dll_phdet_en_ca_cha, SHU_CA_DLL0_RG_ARDLL_PHDET_EN_CA_SHU);
    vIO32WriteFldAlign(DDRPHY_SHU_CA_DLL0 + SHIFT_TO_CHB_ADDR, DDR800Mode.dll_phdet_en_ca_chb, SHU_CA_DLL0_RG_ARDLL_PHDET_EN_CA_SHU);
    vIO32WriteFldAlign(DDRPHY_SHU_PLL22, DDR800Mode.phypll_ada_mck8x_en, SHU_PLL22_RG_RPHYPLL_ADA_MCK8X_EN_SHU);
    vIO32WriteFldAlign_All(DDRPHY_SHU_PLL0, DDR800Mode.pll0_ada_mck8x_chb_en, SHU_PLL0_ADA_MCK8X_CHB_EN);
    vIO32WriteFldAlign_All(DDRPHY_SHU_PLL0, DDR800Mode.pll0_ada_mck8x_cha_en, SHU_PLL0_ADA_MCK8X_CHA_EN);

    // CA/DQS/DQ position
    vIO32WriteFldAlign_All(DDRPHY_SHU_B0_DQ9, DDR800Mode.ddr400_dqs_ps_b0, SHU_B0_DQ9_RG_DDR400_DQS_PS_B0);
    vIO32WriteFldAlign_All(DDRPHY_SHU_B1_DQ9, DDR800Mode.ddr400_dqs_ps_b1, SHU_B1_DQ9_RG_DDR400_DQS_PS_B1);
    vIO32WriteFldAlign_All(DDRPHY_SHU_B0_DQ9, DDR800Mode.ddr400_dq_ps_b0, SHU_B0_DQ9_RG_DDR400_DQ_PS_B0);
    vIO32WriteFldAlign_All(DDRPHY_SHU_B1_DQ9, DDR800Mode.ddr400_dq_ps_b1, SHU_B1_DQ9_RG_DDR400_DQ_PS_B1);
    vIO32WriteFldAlign_All(DDRPHY_SHU_CA_CMD9, DDR800Mode.ddr400_dqs_ps_ca, SHU_CA_CMD9_RG_DDR400_DQS_PS_CA);
    vIO32WriteFldAlign_All(DDRPHY_SHU_CA_CMD9, DDR800Mode.ddr400_dq_ps_ca, SHU_CA_CMD9_RG_DDR400_DQ_PS_CA);

    // Semi Open Enable
    vIO32WriteFldAlign_All(DDRPHY_SHU_B0_DQ9, DDR800Mode.ddr400_semi_en_b0, SHU_B0_DQ9_RG_DDR400_SEMI_EN_B0);
    vIO32WriteFldAlign_All(DDRPHY_SHU_B1_DQ9, DDR800Mode.ddr400_semi_en_b1, SHU_B1_DQ9_RG_DDR400_SEMI_EN_B1);
    vIO32WriteFldAlign_All(DDRPHY_SHU_CA_CMD9, DDR800Mode.ddr400_semi_en_ca, SHU_CA_CMD9_RG_DDR400_SEMI_EN_CA);
    vIO32WriteFldAlign_All(DDRPHY_SHU_PLL0, DDR800Mode.ddr400_semi_open_en, SHU_PLL0_RG_DDR400_SEMI_OPEN_EN);
#endif
}

static void DDRDllModeSetting(DRAMC_CTX_T * p)
{
#if __A60868_TO_BE_PORTING__
    U8 uDLL1 = 0, uMode = 0, uDLL0 = 2; // 1:8 mode for bring-up

    APhyModeSetting(p, vGet_DDR800_Mode(p), &uDLL1, &uMode, &uDLL0);

    vIO32WriteFldAlign_All(DDRPHY_SHU_B0_DLL1, uDLL1, SHU_B0_DLL1_FRATE_EN_B0);
    vIO32WriteFldAlign_All(DDRPHY_SHU_B1_DLL1, uDLL1, SHU_B1_DLL1_FRATE_EN_B1);
    vIO32WriteFldAlign_All(DDRPHY_SHU_CA_DLL2, uDLL1, SHU_CA_DLL2_FRATE_EN);

    vIO32WriteFldAlign_All(DDRPHY_SHU_B0_DQ6, uMode, SHU_B0_DQ6_RG_TX_ARDQ_SER_MODE_B0);
    vIO32WriteFldAlign_All(DDRPHY_SHU_B1_DQ6, uMode, SHU_B1_DQ6_RG_TX_ARDQ_SER_MODE_B1);
    vIO32WriteFldAlign_All(DDRPHY_SHU_CA_CMD6, uMode, SHU_CA_CMD6_RG_TX_ARCMD_SER_MODE);
    vIO32WriteFldAlign_All(DDRPHY_SHU_B0_DQ6, uMode, SHU_B0_DQ6_RG_RX_ARDQ_RANK_SEL_SER_MODE_B0);
    vIO32WriteFldAlign_All(DDRPHY_SHU_B1_DQ6, uMode, SHU_B1_DQ6_RG_RX_ARDQ_RANK_SEL_SER_MODE_B1);
    vIO32WriteFldAlign_All(DDRPHY_SHU_CA_CMD6, uMode, SHU_CA_CMD6_RG_RX_ARCMD_RANK_SEL_SER_MODE);

    vIO32WriteFldAlign_All(DDRPHY_SHU_B0_DLL0, uDLL0, SHU_B0_DLL0_RG_ARDLL_DIV_MCTL_B0);
    vIO32WriteFldAlign_All(DDRPHY_SHU_B1_DLL0, uDLL0, SHU_B1_DLL0_RG_ARDLL_DIV_MCTL_B1);
    vIO32WriteFldAlign_All(DDRPHY_SHU_CA_DLL0, uDLL0, SHU_CA_DLL0_RG_ARDLL_DIV_MCTL_CA);
#endif
}

static void DDRPhyPLLSetting(DRAMC_CTX_T *p)
{
#if __A60868_TO_BE_PORTING__

    U8 u1CAP_SEL;
    U8 u1MIDPICAP_SEL;
    U8 u1VTH_SEL;
    U16 u2SDM_PCW = 0;
    U8 u1CA_DLL_Mode[2];
    U8 iChannel = CHANNEL_A;
    U8 u1BRPI_MCTL_EN_CA = 0;
    U8 u1PCW_CHG = 0;
#if ENABLE_TMRRI_NEW_MODE
    U8 u1RankIdx;
#endif
    U8 u1Gain_Ca_ChA = 0, u1Gain_Ca_ChB = 0;
    U8 u1CurrShuLevel = 0;

    u1VTH_SEL = 0x2; /* RG_*RPI_MIDPI_VTH_SEL[1:0] is 2 for all freqs */

#if (fcFOR_CHIP_ID == fcLafite)
    if (p->frequency <= 400)
    {
        u1CAP_SEL = 0xb;
    }
    else if (p->frequency <= 600)
    {
        u1CAP_SEL = 0xf;
    }
    else if (p->frequency <= 800)
    {
        u1CAP_SEL = 0xb;
    }
    else if (p->frequency <= 1200)
    {
        u1CAP_SEL = 0x8;
    }
    else if (p->frequency <= 1333)
    {
        u1CAP_SEL = 0x8;
    }
    else if (p->frequency <= 1600)
    {
        u1CAP_SEL = 0x4;
    }
    else if (p->frequency <= 1866)
    {
        u1CAP_SEL = 0x2;
    }
    else//4266
    {
        u1CAP_SEL = 0x1;
    }

    if (p->frequency <= 933) //Lewis@20161129: Fix DDR1600 S-idle Vcore power is too big problem.
    {
        u1MIDPICAP_SEL = 0x2;
    }
    else if (p->frequency <= 1200)
    {
        u1MIDPICAP_SEL = 0x3;
    }
    else if (p->frequency <= 1333)
    {
        u1MIDPICAP_SEL = 0x3;
    }
    else if (p->frequency <= 1600)
    {
        u1MIDPICAP_SEL = 0x2;
    }
    else if (p->frequency <= 1866)
    {
        u1MIDPICAP_SEL = 0x1;
    }
    else//4266
    {
        u1MIDPICAP_SEL = 0x0;
    }
#if EMI_LPBK_USE_DDR_800
    if (p->frequency == 800)
    {
        u1CAP_SEL = 0xf;    //Ying-Yu suggest setting for FT pattern
    }
#endif
#endif

    vIO32WriteFldAlign_All(DDRPHY_SHU_PLL4, 0x0, SHU_PLL4_RG_RPHYPLL_RESERVED);
    vIO32WriteFldAlign_All(DDRPHY_SHU_PLL6, 0x0, SHU_PLL6_RG_RCLRPLL_RESERVED);

#if (fcFOR_CHIP_ID == fcLafite)
    #if DLL_ASYNC_MODE
    u1BRPI_MCTL_EN_CA = 1;
    u1CA_DLL_Mode[CHANNEL_A] = u1CA_DLL_Mode[CHANNEL_B] = DLL_MASTER;
    vIO32WriteFldAlign(DDRPHY_MISC_SHU_OPT + ((U32)CHANNEL_A << POS_BANK_NUM), 1, MISC_SHU_OPT_R_CA_SHU_PHDET_SPM_EN);
    vIO32WriteFldAlign(DDRPHY_MISC_SHU_OPT + SHIFT_TO_CHB_ADDR, 1, MISC_SHU_OPT_R_CA_SHU_PHDET_SPM_EN);
    vIO32WriteFldMulti(DDRPHY_CKMUX_SEL + ((U32)CHANNEL_A << POS_BANK_NUM), P_Fld(0, CKMUX_SEL_FMEM_CK_MUX) | P_Fld(0, CKMUX_SEL_FB_CK_MUX));
    vIO32WriteFldMulti(DDRPHY_CKMUX_SEL + SHIFT_TO_CHB_ADDR, P_Fld(2, CKMUX_SEL_FMEM_CK_MUX) | P_Fld(2, CKMUX_SEL_FB_CK_MUX));
    #else
    u1CA_DLL_Mode[CHANNEL_A] = DLL_MASTER;
    u1CA_DLL_Mode[CHANNEL_B] = DLL_SLAVE;
    vIO32WriteFldAlign(DDRPHY_MISC_SHU_OPT + ((U32)CHANNEL_A << POS_BANK_NUM), 1, MISC_SHU_OPT_R_CA_SHU_PHDET_SPM_EN);
    vIO32WriteFldAlign(DDRPHY_MISC_SHU_OPT + SHIFT_TO_CHB_ADDR, 2, MISC_SHU_OPT_R_CA_SHU_PHDET_SPM_EN);
    vIO32WriteFldMulti(DDRPHY_CKMUX_SEL + ((U32)CHANNEL_A << POS_BANK_NUM), P_Fld(1, CKMUX_SEL_FMEM_CK_MUX) | P_Fld(1, CKMUX_SEL_FB_CK_MUX));
    vIO32WriteFldMulti(DDRPHY_CKMUX_SEL + SHIFT_TO_CHB_ADDR, P_Fld(1, CKMUX_SEL_FMEM_CK_MUX) | P_Fld(1, CKMUX_SEL_FB_CK_MUX));
    #endif

    #if ENABLE_DLL_ALL_SLAVE_MODE
    if ((p->frequency <= 933) && (vGet_DDR800_Mode(p) != DDR800_SEMI_LOOP))
    {
        u1CA_DLL_Mode[CHANNEL_A] = u1CA_DLL_Mode[CHANNEL_B] = DLL_SLAVE;
    }
    #endif

    if (u1CA_DLL_Mode[CHANNEL_A] == DLL_SLAVE)//All slave mode
    {
        vIO32WriteFldAlign_All(DRAMC_REG_SHU_DVFSCTL, 1, SHU_DVFSCTL_R_BYPASS_1ST_DLL);
    }
    else
    {
        vIO32WriteFldAlign_All(DRAMC_REG_SHU_DVFSCTL, 0, SHU_DVFSCTL_R_BYPASS_1ST_DLL);
    }

    for (iChannel = CHANNEL_A; iChannel <= CHANNEL_B; iChannel++)
    {
        if (u1CA_DLL_Mode[iChannel] == DLL_MASTER)
        {
            vIO32WriteFldMulti(DDRPHY_SHU_CA_DLL0 + ((U32)iChannel << POS_BANK_NUM), P_Fld(0x0, SHU_CA_DLL0_RG_ARDLL_PHDET_OUT_SEL_CA)
                        | P_Fld(0x0, SHU_CA_DLL0_RG_ARDLL_PHDET_IN_SWAP_CA)
                        | P_Fld(0x6, SHU_CA_DLL0_RG_ARDLL_GAIN_CA)
                        | P_Fld(0x9, SHU_CA_DLL0_RG_ARDLL_IDLECNT_CA)
                        | P_Fld(0x8, SHU_CA_DLL0_RG_ARDLL_P_GAIN_CA)
                        | P_Fld(0x1, SHU_CA_DLL0_RG_ARDLL_PHJUMP_EN_CA)
                        | P_Fld(0x1, SHU_CA_DLL0_RG_ARDLL_PHDIV_CA)
                        | P_Fld(0x1, SHU_CA_DLL0_RG_ARDLL_FAST_PSJP_CA));
            vIO32WriteFldMulti(DDRPHY_SHU_CA_DLL1 + ((U32)iChannel << POS_BANK_NUM), P_Fld(0x1, SHU_CA_DLL1_RG_ARDLL_PD_CK_SEL_CA) | P_Fld(0x0, SHU_CA_DLL1_RG_ARDLL_FASTPJ_CK_SEL_CA));
            vIO32WriteFldAlign(DDRPHY_SHU_CA_CMD9 + ((U32)iChannel << POS_BANK_NUM), 1, SHU_CA_CMD9_RG_DLL_FAST_PSJP_CA); // RG_*RPI_RESERVE_CA[1] 1'b1 tracking leaf(slave)
        }
        else
        {
            vIO32WriteFldMulti(DDRPHY_SHU_CA_DLL0 + ((U32)iChannel << POS_BANK_NUM), P_Fld(0x1, SHU_CA_DLL0_RG_ARDLL_PHDET_OUT_SEL_CA)
                        | P_Fld(0x1, SHU_CA_DLL0_RG_ARDLL_PHDET_IN_SWAP_CA)
                        | P_Fld(0x7, SHU_CA_DLL0_RG_ARDLL_GAIN_CA)
                        | P_Fld(0x7, SHU_CA_DLL0_RG_ARDLL_IDLECNT_CA)
                        | P_Fld(0x8, SHU_CA_DLL0_RG_ARDLL_P_GAIN_CA)
                        | P_Fld(0x1, SHU_CA_DLL0_RG_ARDLL_PHJUMP_EN_CA)
                        | P_Fld(0x1, SHU_CA_DLL0_RG_ARDLL_PHDIV_CA)
                        | P_Fld(0x0, SHU_CA_DLL0_RG_ARDLL_FAST_PSJP_CA));
            vIO32WriteFldMulti(DDRPHY_SHU_CA_DLL1 + ((U32)iChannel << POS_BANK_NUM), P_Fld(0x0, SHU_CA_DLL1_RG_ARDLL_PD_CK_SEL_CA) | P_Fld(0x1, SHU_CA_DLL1_RG_ARDLL_FASTPJ_CK_SEL_CA));
            vIO32WriteFldAlign(DDRPHY_SHU_CA_CMD9 + ((U32)iChannel << POS_BANK_NUM), 0, SHU_CA_CMD9_RG_DLL_FAST_PSJP_CA); // RG_*RPI_RESERVE_CA[1] 1'b1 tracking leaf(slave)
        }
    }
#endif

#if (fcFOR_CHIP_ID == fcLafite) // @Darren, enhance DLL Gain for 4S corner DDR2400/DDR1600 0.6125V DVS fail
    u1CurrShuLevel = vGet_Current_SRAMIdx(p);
    if ((u1CurrShuLevel == SRAM_SHU4) || (u1CurrShuLevel == SRAM_SHU6))
    {
        vIO32WriteFldAlign_All(DDRPHY_SHU_B0_DLL0, 0x8, SHU_B0_DLL0_RG_ARDLL_GAIN_B0);
        vIO32WriteFldAlign_All(DDRPHY_SHU_B1_DLL0, 0x8, SHU_B1_DLL0_RG_ARDLL_GAIN_B1);
        u1Gain_Ca_ChA = u4IO32ReadFldAlign(DDRPHY_SHU_CA_DLL0, SHU_CA_DLL0_RG_ARDLL_GAIN_CA);
        u1Gain_Ca_ChB = u4IO32ReadFldAlign(DDRPHY_SHU_CA_DLL0 + SHIFT_TO_CHB_ADDR, SHU_CA_DLL0_RG_ARDLL_GAIN_CA);
        u1Gain_Ca_ChA += 1;
        u1Gain_Ca_ChB += 1;
        vIO32WriteFldAlign(DDRPHY_SHU_CA_DLL0, u1Gain_Ca_ChA, SHU_CA_DLL0_RG_ARDLL_GAIN_CA);
        vIO32WriteFldAlign(DDRPHY_SHU_CA_DLL0 + SHIFT_TO_CHB_ADDR, u1Gain_Ca_ChB, SHU_CA_DLL0_RG_ARDLL_GAIN_CA);
    }
#endif

    U32 u4RegBackupAddress[] =
    {
        (DDRPHY_B0_DQ7),
        (DDRPHY_B1_DQ7),
        (DDRPHY_CA_CMD7),
        (DDRPHY_B0_DQ7 + SHIFT_TO_CHB_ADDR),
        (DDRPHY_B1_DQ7 + SHIFT_TO_CHB_ADDR),
        (DDRPHY_CA_CMD7 + SHIFT_TO_CHB_ADDR),
    };

    //if(p->vendor_id==VENDOR_SAMSUNG && p->dram_type==TYPE_LPDDR3)
    {
#if 0
        mcSHOW_DBG_MSG(("DDRPhyPLLSetting-DMSUS\n"));
        vIO32WriteFldAlign_All(DDRPHY_MISC_SPM_CTRL0, 0x0, MISC_SPM_CTRL0_PHY_SPM_CTL0);
        vIO32WriteFldAlign_All(DDRPHY_MISC_SPM_CTRL2, 0x0, MISC_SPM_CTRL2_PHY_SPM_CTL2);
        vIO32WriteFldMulti_All(DDRPHY_MISC_SPM_CTRL1, P_Fld(0x1, MISC_SPM_CTRL1_RG_ARDMSUS_10) | P_Fld(0x1, MISC_SPM_CTRL1_RG_ARDMSUS_10_B0)
                                                   | P_Fld(0x1, MISC_SPM_CTRL1_RG_ARDMSUS_10_B1) | P_Fld(0x1, MISC_SPM_CTRL1_RG_ARDMSUS_10_CA));
#else
        DramcBackupRegisters(p, u4RegBackupAddress, sizeof(u4RegBackupAddress) / sizeof(U32));
        vIO32WriteFldMulti_All(DDRPHY_B0_DQ7, P_Fld(0x1, B0_DQ7_RG_TX_ARDQ_PULL_DN_B0) | P_Fld(0x1, B0_DQ7_RG_TX_ARDQM0_PULL_DN_B0)
                                                   | P_Fld(0x1, B0_DQ7_RG_TX_ARDQS0_PULL_DN_B0) | P_Fld(0x1, B0_DQ7_RG_TX_ARDQS0B_PULL_DN_B0));
        vIO32WriteFldMulti_All(DDRPHY_B1_DQ7, P_Fld(0x1, B1_DQ7_RG_TX_ARDQ_PULL_DN_B1) | P_Fld(0x1, B1_DQ7_RG_TX_ARDQM0_PULL_DN_B1)
                                                   | P_Fld(0x1, B1_DQ7_RG_TX_ARDQS0_PULL_DN_B1) | P_Fld(0x1, B1_DQ7_RG_TX_ARDQS0B_PULL_DN_B1));
        vIO32WriteFldMulti_All(DDRPHY_CA_CMD7, P_Fld(0x1, CA_CMD7_RG_TX_ARCMD_PULL_DN) | P_Fld(0x1, CA_CMD7_RG_TX_ARCS_PULL_DN)
                                                   | P_Fld(0x1, CA_CMD7_RG_TX_ARCLK_PULL_DN) | P_Fld(0x1, CA_CMD7_RG_TX_ARCLKB_PULL_DN));

        // DMSUS replaced by CA_CMD2_RG_TX_ARCMD_OE_DIS, CMD_OE_DIS(1) will prevent illegal command ouput
        // And DRAM 1st reset_n pulse will disappear if use CA_CMD2_RG_TX_ARCMD_OE_DIS
        vIO32WriteFldAlign_All(DDRPHY_CA_CMD2, 1, CA_CMD2_RG_TX_ARCMD_OE_DIS);
#endif
    }

    //26M
    vIO32WriteFldAlign_All(DDRPHY_MISC_CG_CTRL0, 0x0, MISC_CG_CTRL0_CLK_MEM_SEL);

    #ifdef USE_CLK26M
    vIO32WriteFldAlign_All(DDRPHY_MISC_CG_CTRL0, 0x1, MISC_CG_CTRL0_RG_DA_RREF_CK_SEL);
    #endif

    //PLL close sequence:
    //DLL_PHDET_EN_* = 0
    //==> CG = 1
    //==> MIDPI_EN = 0(async)
    //==> RG_ARPI_RESETB_* = 0
    //==> MCK8X_EN(source of clk gating) = 0
    //==> PLL_EN = 0 PIC: Ying-Yu

    //DLL
    vIO32WriteFldAlign_All(DDRPHY_CA_DLL_ARPI2, 0x0, CA_DLL_ARPI2_RG_ARDLL_PHDET_EN_CA);
    vIO32WriteFldAlign_All(DDRPHY_B0_DLL_ARPI2, 0x0, B0_DLL_ARPI2_RG_ARDLL_PHDET_EN_B0);
    vIO32WriteFldAlign_All(DDRPHY_B1_DLL_ARPI2, 0x0, B1_DLL_ARPI2_RG_ARDLL_PHDET_EN_B1);

    //CG
    vIO32WriteFldMulti_All(DDRPHY_B0_DLL_ARPI2, P_Fld(0x1, B0_DLL_ARPI2_RG_ARPI_CG_MCK_B0)
                | P_Fld(0x1, B0_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_B0)
                | P_Fld(0x1, B0_DLL_ARPI2_RG_ARPI_CG_MCTL_B0)
                | P_Fld(0x1, B0_DLL_ARPI2_RG_ARPI_CG_FB_B0)
                | P_Fld(0x1, B0_DLL_ARPI2_RG_ARPI_CG_DQS_B0)
                | P_Fld(0x1, B0_DLL_ARPI2_RG_ARPI_CG_DQM_B0)
                | P_Fld(0x1, B0_DLL_ARPI2_RG_ARPI_CG_DQ_B0)
                | P_Fld(0x1, B0_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B0)
                | P_Fld(0x1, B0_DLL_ARPI2_RG_ARPI_MPDIV_CG_B0));
    vIO32WriteFldMulti_All(DDRPHY_B1_DLL_ARPI2, P_Fld(0x1, B1_DLL_ARPI2_RG_ARPI_CG_MCK_B1)
                | P_Fld(0x1, B1_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_B1)
                | P_Fld(0x1, B1_DLL_ARPI2_RG_ARPI_CG_MCTL_B1)
                | P_Fld(0x1, B1_DLL_ARPI2_RG_ARPI_CG_FB_B1)
                | P_Fld(0x1, B1_DLL_ARPI2_RG_ARPI_CG_DQS_B1)
                | P_Fld(0x1, B1_DLL_ARPI2_RG_ARPI_CG_DQM_B1)
                | P_Fld(0x1, B1_DLL_ARPI2_RG_ARPI_CG_DQ_B1)
                | P_Fld(0x1, B1_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B1)
                | P_Fld(0x1, B1_DLL_ARPI2_RG_ARPI_MPDIV_CG_B1));
    vIO32WriteFldMulti_All(DDRPHY_CA_DLL_ARPI2, P_Fld(0x1, CA_DLL_ARPI2_RG_ARPI_CG_MCK_CA)
                | P_Fld(0x1, CA_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_CA)
                | P_Fld(0x1, CA_DLL_ARPI2_RG_ARPI_CG_MCTL_CA)
                | P_Fld(0x1, CA_DLL_ARPI2_RG_ARPI_CG_FB_CA)
                | P_Fld(0x1, CA_DLL_ARPI2_RG_ARPI_CG_CS)
                | P_Fld(0x1, CA_DLL_ARPI2_RG_ARPI_CG_CLK)
                | P_Fld(0x1, CA_DLL_ARPI2_RG_ARPI_CG_CMD)
                | P_Fld(0x1, CA_DLL_ARPI2_RG_ARPI_CG_CLKIEN)
                | P_Fld(0x1, CA_DLL_ARPI2_RG_ARPI_MPDIV_CG_CA));

    //MIDPI_EN
    vIO32WriteFldMulti_All(DDRPHY_SHU_B0_DQ6, P_Fld(0x0, SHU_B0_DQ6_RG_ARPI_MIDPI_EN_B0)
                | P_Fld(0x0, SHU_B0_DQ6_RG_ARPI_MIDPI_CKDIV4_EN_B0));
    vIO32WriteFldMulti_All(DDRPHY_SHU_B1_DQ6, P_Fld(0x0, SHU_B1_DQ6_RG_ARPI_MIDPI_EN_B1)
                | P_Fld(0x0, SHU_B1_DQ6_RG_ARPI_MIDPI_CKDIV4_EN_B1));
    vIO32WriteFldMulti_All(DDRPHY_SHU_CA_CMD6, P_Fld(0x0, SHU_CA_CMD6_RG_ARPI_MIDPI_EN_CA)
                | P_Fld(0x0, SHU_CA_CMD6_RG_ARPI_MIDPI_CKDIV4_EN_CA));

    //RESETB
    vIO32WriteFldAlign_All(DDRPHY_CA_DLL_ARPI0, 0x0, CA_DLL_ARPI0_RG_ARPI_RESETB_CA);
    vIO32WriteFldAlign_All(DDRPHY_B0_DLL_ARPI0, 0x0, B0_DLL_ARPI0_RG_ARPI_RESETB_B0);
    vIO32WriteFldAlign_All(DDRPHY_B1_DLL_ARPI0, 0x0, B1_DLL_ARPI0_RG_ARPI_RESETB_B1);
    mcDELAY_US(1);

    //MCK8X_EN
    vIO32WriteFldMulti_All(DDRPHY_PLL4, P_Fld(0x0, PLL4_RG_RPHYPLL_ADA_MCK8X_EN)
                | P_Fld(0x0, PLL4_RG_RPHYPLL_RESETB));

    //PLL
    vIO32WriteFldAlign_All(DDRPHY_PLL1, 0x0, PLL1_RG_RPHYPLL_EN);
    vIO32WriteFldAlign_All(DDRPHY_PLL2, 0x0, PLL2_RG_RCLRPLL_EN);

    ///TODO: PLL/MIDPI Settings
    //Ref clock should be 20M~30M, if MPLL=52M, Pre-divider should be set to 1
    #ifdef USE_CLK26M
    vIO32WriteFldMulti_All(DDRPHY_SHU_PLL8, P_Fld(0x0, SHU_PLL8_RG_RPHYPLL_POSDIV) | P_Fld(0x0, SHU_PLL8_RG_RPHYPLL_PREDIV));
    vIO32WriteFldMulti_All(DDRPHY_SHU_PLL10, P_Fld(0x0, SHU_PLL10_RG_RCLRPLL_POSDIV) | P_Fld(0x0, SHU_PLL10_RG_RCLRPLL_PREDIV));
    #else //MPLL 52M
    vIO32WriteFldMulti_All(DDRPHY_SHU_PLL8, P_Fld(0x0, SHU_PLL8_RG_RPHYPLL_POSDIV) | P_Fld(0x1, SHU_PLL8_RG_RPHYPLL_PREDIV));
    vIO32WriteFldMulti_All(DDRPHY_SHU_PLL10, P_Fld(0x0, SHU_PLL10_RG_RCLRPLL_POSDIV) | P_Fld(0x1, SHU_PLL10_RG_RCLRPLL_PREDIV));
    #endif

    if (p->frequency == 2133)
    {
        u2SDM_PCW = 0xa400;
    }
    else if (p->frequency == 1866)
    {   // 2722/26=143(0x8f)
        #if ENABLE_FIX_SHORT_PLUSE
        u2SDM_PCW = 0x7b00; // DDR3200
        #else
        u2SDM_PCW = 0x8f00;
        #endif
    }
    else if (p->frequency == 1600 || p->frequency == 800 || p->frequency == 400)
    {
        #if ENABLE_FIX_SHORT_PLUSE
        if (p->frequency == 1600)
            u2SDM_PCW = 0x6c00; //DDR2800
        #else
        if (p->frequency == 1600)
            u2SDM_PCW = 0x7700; //DDR3200(3094)
        else if (p->frequency == 800)
            u2SDM_PCW = 0x7600; //DDR1600(1534)
        #endif
        else if ((p->frequency == 400) && (vGet_DDR800_Mode(p) == DDR800_OPEN_LOOP))
            u2SDM_PCW = 0x3c00; //DDR800 Open Loop Mode
        else if (p->frequency == 400)
            u2SDM_PCW = 0x7e00; //DDR826 for avoid GPS de-sense
        else
            u2SDM_PCW = 0x7b00; //DDR1600

#if EMI_LPBK_USE_DDR_800 // For Ei_ger DDR800 no need
        if (p->frequency == 800)
        {
            vIO32WriteFldAlign_All(DDRPHY_SHU_PLL8, 0x1, SHU_PLL8_RG_RPHYPLL_POSDIV);
            vIO32WriteFldAlign_All(DDRPHY_SHU_PLL10, 0x1, SHU_PLL10_RG_RCLRPLL_POSDIV);
        }
#endif
    }
    else if (p->frequency == 1333 || p->frequency == 667)
    {
        u2SDM_PCW = 0x6600;
    }
    else if (p->frequency == 1200)
    {
        #if ENABLE_FIX_SHORT_PLUSE
        u2SDM_PCW = 0x5100; //DDR2100
        #else
        u2SDM_PCW = 0x5c00;
        #endif
    }
    else if (p->frequency == 1140)
    {
        u2SDM_PCW = 0x5700;
    }
    //LP3
    else if (p->frequency == 933)
    {
        u2SDM_PCW = 0x8f00;
    }
    else if (p->frequency == 600)
    {
        u2SDM_PCW = 0x5c00;
    }
    else if (p->frequency == 467)
    {
        u2SDM_PCW = 0x4700;
    }

    /* SDM_PCW: Feedback divide ratio (8-bit integer + 8-bit fraction)
     * PLL_SDM_FRA_EN: SDMPLL fractional mode enable (0:Integer mode, 1:Fractional mode)
     */
    vIO32WriteFldMulti_All(DDRPHY_SHU_PLL5, P_Fld(u2SDM_PCW, SHU_PLL5_RG_RPHYPLL_SDM_PCW)
                                            | P_Fld(0x0, SHU_PLL5_RG_RPHYPLL_SDM_FRA_EN)); // Disable fractional mode
    vIO32WriteFldMulti_All(DDRPHY_SHU_PLL7, P_Fld(u2SDM_PCW, SHU_PLL7_RG_RCLRPLL_SDM_PCW)
                                            | P_Fld(0x0, SHU_PLL7_RG_RCLRPLL_SDM_FRA_EN)); // Disable fractional mode

#if (fcFOR_CHIP_ID == fcLafite) // YH for LPDDR4 1:4 mode and 1:8 mode, Darren confirm with YH Cho
    DDRDllModeSetting(p);
#endif

    vIO32WriteFldAlign_All(DDRPHY_CA_DLL_ARPI0, 0, CA_DLL_ARPI0_RG_ARMCTLPLL_CK_SEL_CA);
    vIO32WriteFldAlign_All(DDRPHY_B0_DLL_ARPI0, 0, B0_DLL_ARPI0_RG_ARMCTLPLL_CK_SEL_B0);
    vIO32WriteFldAlign_All(DDRPHY_B1_DLL_ARPI0, 0, B1_DLL_ARPI0_RG_ARMCTLPLL_CK_SEL_B1);
    vIO32WriteFldAlign_All(DDRPHY_CA_DLL_ARPI1, 0, CA_DLL_ARPI1_RG_ARPI_CLKIEN_JUMP_EN);
    vIO32WriteFldAlign_All(DDRPHY_B0_DLL_ARPI1, 0, B0_DLL_ARPI1_RG_ARPI_MCTL_JUMP_EN_B0);
    vIO32WriteFldAlign_All(DDRPHY_B1_DLL_ARPI1, 0, B1_DLL_ARPI1_RG_ARPI_MCTL_JUMP_EN_B1);


    vIO32WriteFldMulti_All(DDRPHY_SHU_B0_DQ6, P_Fld(u1VTH_SEL, SHU_B0_DQ6_RG_ARPI_MIDPI_VTH_SEL_B0)
                | P_Fld(u1CAP_SEL, SHU_B0_DQ6_RG_ARPI_CAP_SEL_B0)
                | P_Fld(u1MIDPICAP_SEL, SHU_B0_DQ6_RG_ARPI_MIDPI_CAP_SEL_B0));
    vIO32WriteFldMulti_All(DDRPHY_SHU_B1_DQ6, P_Fld(u1VTH_SEL, SHU_B1_DQ6_RG_ARPI_MIDPI_VTH_SEL_B1)
                | P_Fld(u1CAP_SEL, SHU_B1_DQ6_RG_ARPI_CAP_SEL_B1)
                | P_Fld(u1MIDPICAP_SEL, SHU_B1_DQ6_RG_ARPI_MIDPI_CAP_SEL_B1));
    vIO32WriteFldMulti_All(DDRPHY_SHU_CA_CMD6, P_Fld(u1VTH_SEL, SHU_CA_CMD6_RG_ARPI_MIDPI_VTH_SEL_CA)
                | P_Fld(u1CAP_SEL, SHU_CA_CMD6_RG_ARPI_CAP_SEL_CA)
                | P_Fld(u1MIDPICAP_SEL, SHU_CA_CMD6_RG_ARPI_MIDPI_CAP_SEL_CA));

    //PLL open sequence
    //PLL_EN = 1
    //==> some of MIDPI*_EN = 1(async)
    //==> RG_ARPI_RESETB_* = 1(async, open first)
    //==> MCK8X_EN(source of clk gating) = 1
    //==> CG = 0
    //==> DLL_PHDET_EN_* = 1 PIC: Ying-Yu

    //PLL
    vIO32WriteFldAlign_All(DDRPHY_PLL1, 0x1, PLL1_RG_RPHYPLL_EN);
    vIO32WriteFldAlign_All(DDRPHY_PLL2, 0x1, PLL2_RG_RCLRPLL_EN);
    mcDELAY_US(100);

    ///TODO: MIDPI Init 2
    /* MIDPI Settings (Olymp_us): DA_*RPI_MIDPI_EN, DA_*RPI_MIDPI_CKDIV4_EN
     * Justin suggests use frequency > 933 as boundary
     */
    if (p->frequency > 933)
    {
        vIO32WriteFldMulti_All(DDRPHY_SHU_B0_DQ6, P_Fld(0x1, SHU_B0_DQ6_RG_ARPI_MIDPI_EN_B0)
                    | P_Fld(0x0, SHU_B0_DQ6_RG_ARPI_MIDPI_CKDIV4_EN_B0));
        vIO32WriteFldMulti_All(DDRPHY_SHU_B1_DQ6, P_Fld(0x1, SHU_B1_DQ6_RG_ARPI_MIDPI_EN_B1)
                    | P_Fld(0x0, SHU_B1_DQ6_RG_ARPI_MIDPI_CKDIV4_EN_B1));
        vIO32WriteFldMulti_All(DDRPHY_SHU_CA_CMD6, P_Fld(0x1, SHU_CA_CMD6_RG_ARPI_MIDPI_EN_CA)
                    | P_Fld(0x0, SHU_CA_CMD6_RG_ARPI_MIDPI_CKDIV4_EN_CA));
    }
    else
    {
        if ((p->frequency == 400) && (vGet_DDR800_Mode(p) != DDR800_CLOSE_LOOP)) // For *DDR800_OPEN_LOOP
        {
            vIO32WriteFldMulti_All(DDRPHY_SHU_B0_DQ6, P_Fld(0x0, SHU_B0_DQ6_RG_ARPI_MIDPI_EN_B0)
                        | P_Fld(0x0, SHU_B0_DQ6_RG_ARPI_MIDPI_CKDIV4_EN_B0));
            vIO32WriteFldMulti_All(DDRPHY_SHU_B1_DQ6, P_Fld(0x0, SHU_B1_DQ6_RG_ARPI_MIDPI_EN_B1)
                        | P_Fld(0x0, SHU_B1_DQ6_RG_ARPI_MIDPI_CKDIV4_EN_B1));
            if (vGet_DDR800_Mode(p) == DDR800_SEMI_LOOP)
            {
                // DDR800_SEMI_LOOP from YY comment to DVT (1/0, CHA CA/other)
                vIO32WriteFldAlign_All(DDRPHY_SHU_CA_CMD6, 0x0, SHU_CA_CMD6_RG_ARPI_MIDPI_EN_CA);
                vIO32WriteFldAlign(DDRPHY_SHU_CA_CMD6, 0x1, SHU_CA_CMD6_RG_ARPI_MIDPI_CKDIV4_EN_CA);
                vIO32WriteFldAlign(DDRPHY_SHU_CA_CMD6 + SHIFT_TO_CHB_ADDR, 0x0, SHU_CA_CMD6_RG_ARPI_MIDPI_CKDIV4_EN_CA);
            }
            else
            {   //DDR800_OPEN_LOOP
                vIO32WriteFldMulti_All(DDRPHY_SHU_CA_CMD6, P_Fld(0x0, SHU_CA_CMD6_RG_ARPI_MIDPI_EN_CA)
                            | P_Fld(0x0, SHU_CA_CMD6_RG_ARPI_MIDPI_CKDIV4_EN_CA));
            }
        }
        else
        {
            //MIDPI_EN
            vIO32WriteFldMulti_All(DDRPHY_SHU_B0_DQ6, P_Fld(0x0, SHU_B0_DQ6_RG_ARPI_MIDPI_EN_B0)
                        | P_Fld(0x1, SHU_B0_DQ6_RG_ARPI_MIDPI_CKDIV4_EN_B0));
            vIO32WriteFldMulti_All(DDRPHY_SHU_B1_DQ6, P_Fld(0x0, SHU_B1_DQ6_RG_ARPI_MIDPI_EN_B1)
                        | P_Fld(0x1, SHU_B1_DQ6_RG_ARPI_MIDPI_CKDIV4_EN_B1));
            vIO32WriteFldMulti_All(DDRPHY_SHU_CA_CMD6, P_Fld(0x0, SHU_CA_CMD6_RG_ARPI_MIDPI_EN_CA)
                        | P_Fld(0x1, SHU_CA_CMD6_RG_ARPI_MIDPI_CKDIV4_EN_CA));
        }
    }
    mcDELAY_US(1);

    //RESETB
    vIO32WriteFldAlign_All(DDRPHY_CA_DLL_ARPI0, 0x1, CA_DLL_ARPI0_RG_ARPI_RESETB_CA);
    vIO32WriteFldAlign_All(DDRPHY_B0_DLL_ARPI0, 0x1, B0_DLL_ARPI0_RG_ARPI_RESETB_B0);
    vIO32WriteFldAlign_All(DDRPHY_B1_DLL_ARPI0, 0x1, B1_DLL_ARPI0_RG_ARPI_RESETB_B1);
    mcDELAY_US(1);

    ///TODO: MIDPI Init 1
    //MCK8X_EN
    vIO32WriteFldMulti_All(DDRPHY_PLL4, P_Fld(0x1, PLL4_RG_RPHYPLL_ADA_MCK8X_EN)
                | P_Fld(0x1, PLL4_RG_RPHYPLL_RESETB));
    mcDELAY_US(1);

    #if (fcFOR_CHIP_ID == fcLafite)
    vIO32WriteFldMulti(DDRPHY_CA_DLL_ARPI3, P_Fld(0x1, CA_DLL_ARPI3_RG_ARPI_MCTL_EN_CA)
                | P_Fld(0x1, CA_DLL_ARPI3_RG_ARPI_FB_EN_CA)
                | P_Fld(0x1, CA_DLL_ARPI3_RG_ARPI_CS_EN)
                | P_Fld(0x1, CA_DLL_ARPI3_RG_ARPI_CLK_EN)
                | P_Fld(0x1, CA_DLL_ARPI3_RG_ARPI_CMD_EN));
    vIO32WriteFldMulti(DDRPHY_CA_DLL_ARPI3 + SHIFT_TO_CHB_ADDR, P_Fld(u1BRPI_MCTL_EN_CA, CA_DLL_ARPI3_RG_ARPI_MCTL_EN_CA) //CH_B CA slave
                | P_Fld(0x1, CA_DLL_ARPI3_RG_ARPI_FB_EN_CA)
                | P_Fld(0x1, CA_DLL_ARPI3_RG_ARPI_CS_EN)
                | P_Fld(0x1, CA_DLL_ARPI3_RG_ARPI_CLK_EN)
                | P_Fld(0x1, CA_DLL_ARPI3_RG_ARPI_CMD_EN));
    #endif
    vIO32WriteFldMulti_All(DDRPHY_B0_DLL_ARPI3, P_Fld(0x1, B0_DLL_ARPI3_RG_ARPI_FB_EN_B0)
                | P_Fld(0x1, B0_DLL_ARPI3_RG_ARPI_DQS_EN_B0)
                | P_Fld(0x1, B0_DLL_ARPI3_RG_ARPI_DQM_EN_B0)
                | P_Fld(0x1, B0_DLL_ARPI3_RG_ARPI_DQ_EN_B0)
                | P_Fld(0x1, B0_DLL_ARPI3_RG_ARPI_DQSIEN_EN_B0));
    vIO32WriteFldMulti_All(DDRPHY_B1_DLL_ARPI3, P_Fld(0x1, B1_DLL_ARPI3_RG_ARPI_FB_EN_B1)
                | P_Fld(0x1, B1_DLL_ARPI3_RG_ARPI_DQS_EN_B1)
                | P_Fld(0x1, B1_DLL_ARPI3_RG_ARPI_DQM_EN_B1)
                | P_Fld(0x1, B1_DLL_ARPI3_RG_ARPI_DQ_EN_B1)
                | P_Fld(0x1, B1_DLL_ARPI3_RG_ARPI_DQSIEN_EN_B1));
   //CG
    vIO32WriteFldMulti_All(DDRPHY_CA_DLL_ARPI2, P_Fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_MCK_CA)
                | P_Fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_CA)
                | P_Fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_MCTL_CA)
                | P_Fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_FB_CA)
                | P_Fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_CS)
                | P_Fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_CLK)
                | P_Fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_CMD)
                | P_Fld(0x0, CA_DLL_ARPI2_RG_ARPI_MPDIV_CG_CA));
    vIO32WriteFldMulti_All(DDRPHY_B0_DLL_ARPI2, P_Fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_MCK_B0)
                | P_Fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_B0)
                | P_Fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_MCTL_B0)
                | P_Fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_FB_B0)
                | P_Fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_DQS_B0)
                | P_Fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_DQM_B0)
                | P_Fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_DQ_B0)
                | P_Fld(0x0, B0_DLL_ARPI2_RG_ARPI_MPDIV_CG_B0));
    vIO32WriteFldMulti_All(DDRPHY_B1_DLL_ARPI2, P_Fld(0x0, B1_DLL_ARPI2_RG_ARPI_CG_MCK_B1)
                | P_Fld(0x0, B1_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_B1)
                | P_Fld(0x0, B1_DLL_ARPI2_RG_ARPI_CG_MCTL_B1)
                | P_Fld(0x0, B1_DLL_ARPI2_RG_ARPI_CG_FB_B1)
                | P_Fld(0x0, B1_DLL_ARPI2_RG_ARPI_CG_DQS_B1)
                | P_Fld(0x0, B1_DLL_ARPI2_RG_ARPI_CG_DQM_B1)
                | P_Fld(0x0, B1_DLL_ARPI2_RG_ARPI_CG_DQ_B1)
                | P_Fld(0x0, B1_DLL_ARPI2_RG_ARPI_MPDIV_CG_B1));
    #if (fcFOR_CHIP_ID == fcLafite)
    vIO32WriteFldAlign_All(DDRPHY_CA_DLL_ARPI2, 1, CA_DLL_ARPI2_RG_ARPI_CG_CLKIEN);
    vIO32WriteFldAlign_All(DDRPHY_B0_DLL_ARPI2, 0, B0_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B0);
    vIO32WriteFldAlign_All(DDRPHY_B1_DLL_ARPI2, 0, B1_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B1);
    #endif

    mcDELAY_US(2);

    vIO32WriteFldAlign_All(DDRPHY_MISC_CG_CTRL0, 0x1, MISC_CG_CTRL0_CLK_MEM_SEL);
    mcDELAY_US(1);

#if ENABLE_APHY_DLL_IDLE_MODE_OPTION //Should before the PHDET_EN = 1
    vIO32WriteFldAlign(DDRPHY_CA_DLL_ARPI5, 0x0, CA_DLL_ARPI5_RG_ARDLL_IDLE_EN_CA);
    vIO32WriteFldAlign(DDRPHY_CA_DLL_ARPI5 + SHIFT_TO_CHB_ADDR, 0x1, CA_DLL_ARPI5_RG_ARDLL_IDLE_EN_CA);
    vIO32WriteFldAlign_All(DDRPHY_B0_DLL_ARPI5, 0x1, B0_DLL_ARPI5_RG_ARDLL_IDLE_EN_B0);
    vIO32WriteFldAlign_All(DDRPHY_B1_DLL_ARPI5, 0x1, B1_DLL_ARPI5_RG_ARDLL_IDLE_EN_B1);

    vIO32WriteFldAlign_All(DDRPHY_B0_DLL_ARPI5, 0x3, B0_DLL_ARPI5_RG_ARDLL_PD_ZONE_B0);//Zone1: 48ps Zone2: 96ps if(8ps/delay cell)
    vIO32WriteFldAlign_All(DDRPHY_B1_DLL_ARPI5, 0x3, B1_DLL_ARPI5_RG_ARDLL_PD_ZONE_B1);//Intrinsic 2 delay cell, setting: x
    vIO32WriteFldAlign_All(DDRPHY_CA_DLL_ARPI5, 0x3, CA_DLL_ARPI5_RG_ARDLL_PD_ZONE_CA);//Zone1 = 2 + 2x; Zone2 = 2 * Zone1

    vIO32WriteFldAlign_All(DDRPHY_B0_DLL_ARPI5, 0xC, B0_DLL_ARPI5_RG_ARDLL_MON_SEL_B0);
    vIO32WriteFldAlign_All(DDRPHY_B1_DLL_ARPI5, 0xC, B1_DLL_ARPI5_RG_ARDLL_MON_SEL_B1);
    vIO32WriteFldAlign_All(DDRPHY_CA_DLL_ARPI5, 0xC, CA_DLL_ARPI5_RG_ARDLL_MON_SEL_CA);
#endif

    //DLL
    vIO32WriteFldAlign(DDRPHY_CA_DLL_ARPI2, 0x1, CA_DLL_ARPI2_RG_ARDLL_PHDET_EN_CA);
    mcDELAY_US(1);
    vIO32WriteFldAlign_All(DDRPHY_CA_DLL_ARPI2, 0x1, CA_DLL_ARPI2_RG_ARDLL_PHDET_EN_CA);
    mcDELAY_US(1);
    vIO32WriteFldAlign_All(DDRPHY_B0_DLL_ARPI2, 0x1, B0_DLL_ARPI2_RG_ARDLL_PHDET_EN_B0);
    mcDELAY_US(1);
    vIO32WriteFldAlign_All(DDRPHY_B1_DLL_ARPI2, 0x1, B1_DLL_ARPI2_RG_ARDLL_PHDET_EN_B1);
    mcDELAY_US(1);

    //if(p->vendor_id==VENDOR_SAMSUNG && p->dram_type==TYPE_LPDDR3)
    {
#if 0
        mcSHOW_DBG_MSG(("DDRPhyPLLSetting-DMSUS\n\n"));
        vIO32WriteFldMulti_All(DDRPHY_MISC_SPM_CTRL1, P_Fld(0x0, MISC_SPM_CTRL1_RG_ARDMSUS_10) | P_Fld(0x0, MISC_SPM_CTRL1_RG_ARDMSUS_10_B0)
                                                   | P_Fld(0x0, MISC_SPM_CTRL1_RG_ARDMSUS_10_B1) | P_Fld(0x0, MISC_SPM_CTRL1_RG_ARDMSUS_10_CA));
        vIO32WriteFldAlign_All(DDRPHY_MISC_SPM_CTRL0, 0xffffffff, MISC_SPM_CTRL0_PHY_SPM_CTL0);
        vIO32WriteFldAlign_All(DDRPHY_MISC_SPM_CTRL2, 0xffffffff, MISC_SPM_CTRL2_PHY_SPM_CTL2);
#else
        // DMSUS replaced by CA_CMD2_RG_TX_ARCMD_OE_DIS, CMD_OE_DIS(1) will prevent illegal command ouput
        // And DRAM 1st reset_n pulse will disappear if use CA_CMD2_RG_TX_ARCMD_OE_DIS
        vIO32WriteFldAlign_All(DDRPHY_CA_CMD2, 0, CA_CMD2_RG_TX_ARCMD_OE_DIS);
        DramcRestoreRegisters(p, u4RegBackupAddress, sizeof(u4RegBackupAddress) / sizeof(U32));
#endif

        mcSHOW_DBG_MSG2(("DDRPhyPLLSetting-CKEON\n\n"));

        CKEFixOnOff(p, TO_ALL_RANK, CKE_DYNAMIC, TO_ALL_CHANNEL);
    }

#if ENABLE_DFS_SSC_WA
    DDRSSCSetting(p);
#endif

    DDRPhyFreqMeter(p);
#endif
}
#endif
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
//  vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_CA5), P_Fld(0x1, SHU_SELPH_CA5_DLY_CS) | P_Fld(0x1, SHU_SELPH_CA5_DLY_CS1));
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

    U8 u1backup_TCKFIXON[CHANNEL_NUM], ch, ch_bak;

#if MR_CBT_SWITCH_FREQ
#if (fcFOR_CHIP_ID == fc8195)
    static U8 _CurFreq = CBT_UNKNOWN_FREQ;
    if (_CurFreq == freq)
    {
        return; // Do nothing no meter the frequency is.
    }
    _CurFreq = freq;

    /* @chengchun + Dynamic MIOCK to avoid CK stop state violation during DFS */
    ch_bak = p->channel;
    for (ch = CHANNEL_A; ch < p->support_channel_num; ch++) {
        vSetPHY2ChannelMapping(p, ch);
        u1backup_TCKFIXON[ch] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL),
            DRAMC_PD_CTRL_TCKFIXON);
    }
    vSetPHY2ChannelMapping(p, ch_bak);
    vIO32WriteFldAlign_All(DRAMC_REG_DRAMC_PD_CTRL, 0x0, DRAMC_PD_CTRL_TCKFIXON);

    EnableDFSHwModeClk(p);

    if (freq == CBT_LOW_FREQ)
    {
        #if REPLACE_DFS_RG_MODE
        DramcDFSDirectJump_SPMMode_forK(p, DRAM_DFS_REG_SHU1); //only use in Calibration (SCSM mode)
        //DramcDFSDirectJump_SPMMode(p, SRAM_SHU3);// Darren NOTE: Dramc shu1 for MRW (DramcModeRegInit_LP4 and CBT)
        //We use SRAM3(DDR1866-DIFF) instead of SRAM4(DDR1600-SE) because MRW is blocked and will cause SOC(SE) and DRAM(DIFF) mismatch.
        #else
        DramcDFSDirectJump_RGMode(p, DRAM_DFS_REG_SHU1); // Darren NOTE: Dramc shu1 for MRW (DramcModeRegInit_LP4 and CBT)
        #endif
    }
    else
    {
        #if REPLACE_DFS_RG_MODE
        DramcDFSDirectJump_SPMMode_forK(p, DRAM_DFS_REG_SHU0);
        //DramcDFSDirectJump_SPMMode(p, vGet_Current_SRAMIdx(p));
        #else
        DramcDFSDirectJump_RGMode(p, DRAM_DFS_REG_SHU0);
        #endif
    }

    vIO32WriteFldMulti_All(DDRPHY_REG_MISC_CLK_CTRL, P_Fld(0, MISC_CLK_CTRL_DVFS_CLK_MEM_SEL)
                                                    | P_Fld(0, MISC_CLK_CTRL_DVFS_MEM_CK_MUX_UPDATE_EN));

    for (ch = CHANNEL_A; ch < p->support_channel_num; ch++) {
        vSetPHY2ChannelMapping(p, ch);
        vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), u1backup_TCKFIXON[ch],
            DRAMC_PD_CTRL_TCKFIXON);
    }
    vSetPHY2ChannelMapping(p, ch_bak);

    DDRPhyFreqMeter(p);
#else
    #error Need check of the DRAM_DFS_SHUFFLE_X for your chip !!!
#endif
#endif
}


void DramcPowerOnSequence(DRAMC_CTX_T *p)
{
#if ENABLE_TMRRI_NEW_MODE
    //U8 u1RankIdx;
#endif

#ifdef DUMP_INIT_RG_LOG_TO_DE
    //CKE high
    CKEFixOnOff(p, TO_ALL_RANK, CKE_FIXON, TO_ALL_CHANNEL);
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
    CKEFixOnOff(p, TO_ALL_RANK, CKE_FIXOFF, TO_ALL_CHANNEL);

    // delay tINIT1=200us(min) & tINIT2=10ns(min)
    mcDELAY_US(200);

    //reset dram = low
    vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CTRL1), 0x1, MISC_CTRL1_R_DMDA_RRESETB_I);

    // Disable HW MIOCK control to make CLK always on
    DramCLKAlwaysOnOff(p, ON, TO_ALL_CHANNEL);

    //tINIT3=2ms(min)
    mcDELAY_MS(2);

    //CKE high
    CKEFixOnOff(p, TO_ALL_RANK, CKE_FIXON, TO_ALL_CHANNEL);

    // tINIT5=2us(min)
    mcDELAY_US(2);
    //u1PowerOn=1;

    //// Enable  HW MIOCK control to make CLK dynamic
    DramCLKAlwaysOnOff(p, OFF, TO_ALL_CHANNEL);
    mcSHOW_DBG_MSG5(("APPLY_LP4_POWER_INIT_SEQUENCE\n"));
    }
    #endif
}

DRAM_STATUS_T DramcModeRegInit_CATerm(DRAMC_CTX_T *p, U8 bWorkAround)
{
    static U8 CATermWA[CHANNEL_NUM] = {0};
    U8 u1ChannelIdx, u1RankIdx, u1RankIdxBak;
    U32 backup_broadcast;
    //U8 operating_fsp = p->dram_fsp;
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

        mcSHOW_DBG_MSG2(("[DramcModeRegInit_CATerm] CH%u RK%u bWorkAround=%d\n", u1ChannelIdx, u1RankIdx, bWorkAround));
        /* FSP_1: 1. For term freqs   2. Assumes "data rate >= DDR2667" are terminated */
        #if MRW_CHECK_ONLY
        mcSHOW_MRW_MSG(("\n==[MR Dump] %s==\n", __func__));
        #endif
        DramcMRWriteFldAlign(p, 13, 0, MR13_FSP_OP, TO_MR); //@Darren, Fix high freq keep FSP0 for CA term workaround (PPR abnormal)
        DramcMRWriteFldAlign(p, 13, 1, MR13_FSP_WR, TO_MR);

        //MR12 use previous value

        if (p->dram_type == TYPE_LPDDR4P)
        {
            u1MR11_Value = 0x0;                  //ODT disable
        }
        else
        {
#if ENABLE_SAMSUNG_NT_ODT
            if ((p->vendor_id == VENDOR_SAMSUNG) && (p->revision_id == 0x7)) // 1ynm process for NT-ODT
            {
                u1MR11_Value = 0x2;              //@Darren, DQ ODT:120ohm -> parallel to 60ohm
                u1MR11_Value |= (0x1 << 3);   //@Darren, MR11[3]=1 to enable NT-ODT for B707
            }
            else
#endif
                u1MR11_Value = 0x3;              //DQ ODT:80ohm

        #if FSP1_CLKCA_TERM
            if (p->dram_cbt_mode[u1RankIdx] == CBT_NORMAL_MODE)
            {
                u1MR11_Value |= 0x40;              //CA ODT:60ohm for byte mode
            }
            else
            {
                u1MR11_Value |= 0x20;              //CA ODT:120ohm for byte mode
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
            u1MR22_Value = 0x24;     //SOC-ODT, ODTE-CK, ODTE-CS, Disable ODTD-CA
        }
        else  //TYPE_LPDDR4x, LP4P
        {
            u1MR22_Value = 0x3c;     //Disable CA-CS-CLK ODT, SOC ODT=RZQ/4
        #if FSP1_CLKCA_TERM
            if (bWorkAround)
            {
                u1MR22_Value = 0x4;
            }
            else
            {
                if (u1RankIdx == RANK_0)
                {
                    u1MR22_Value = 0x4;     //Enable CA-CS-CLK ODT, SOC ODT=RZQ/4
                }
                else
                {
                    u1MR22_Value = 0x2c;     //Enable CS ODT, SOC ODT=RZQ/4
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
//  vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_MRS), RANK_0, MRS_MRSRK);

    DramcBroadcastOnOff(backup_broadcast);

    return DRAM_OK;
}

DRAM_STATUS_T DramcModeRegInit_LP4(DRAMC_CTX_T *p)
{
    U32 u4RankIdx;//, u4CKE0Bak, u4CKE1Bak, u4MIOCKBak, u4AutoRefreshBak;
    U8  u1MR2_RLWL;
    //U16 u2MR3Value;
    U8 u1MRFsp= FSP_0;
    U8 u1ChannelIdx; //support_channel_num
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

        mcSHOW_DBG_MSG2(("nWR fixed to %d\n", u1nWR));
        //mcDUMP_REG_MSG(("nWR fixed to %d\n", u1nWR));
    }

#ifndef DUMP_INIT_RG_LOG_TO_DE
    if(p->dram_fsp == FSP_1)
    {
        // @Darren, VDDQ = 600mv + CaVref default is 301mV (no impact)
        // Fix high freq keep FSP0 for CA term workaround (PPR abnormal)
        CmdOEOnOff(p, DISABLE, CMDOE_DIS_TO_ALL_CHANNEL);
        cbt_switch_freq(p, CBT_LOW_FREQ);
        CmdOEOnOff(p, ENABLE, CMDOE_DIS_TO_ALL_CHANNEL);
    }
#endif

    for(u1ChannelIdx=0; u1ChannelIdx<(p->support_channel_num); u1ChannelIdx++)
    {
        vSetPHY2ChannelMapping(p, u1ChannelIdx);

        for(u4RankIdx =0; u4RankIdx < (U32)(p->support_rank_num); u4RankIdx++)
        {
            vSetRank(p, u4RankIdx);

            mcSHOW_DBG_MSG2(("[ModeRegInit_LP4] CH%u RK%u\n", u1ChannelIdx, u4RankIdx));
            //mcDUMP_REG_MSG(("[ModeRegInit_LP4] CH%u RK%u\n", u1ChannelIdx, u4RankIdx));
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
                /* FSP_0: 1. For un-term freqs   2. Assumes "data rate < DDR2667" are un-term */
                u1MRFsp = FSP_0;
                mcSHOW_DBG_MSG2(("\tFsp%d\n", u1MRFsp));
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
                u1MR2_RLWL = LP4_DRAM_INIT_RLWL_MRfield_config(p->frequency << 1);
                u1MR02Value[p->dram_fsp] = u1MR2_RLWL | (u1MR2_RLWL << 3); //@tg update 1:4 mode DDR400/800 WL/RL according to spec.

                #if 0
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
                #endif
                DramcModeRegWriteByRank(p, u4RankIdx, 2, u1MR02Value[u1MRFsp]);
                //if(p->odt_onoff)
                u1MR11Value[u1MRFsp] = 0x0;
                DramcModeRegWriteByRank(p, u4RankIdx, 11, u1MR11Value[u1MRFsp]); //ODT disable

#if ENABLE_LP4Y_DFS
                // For LPDDR4Y <= DDR1600 un-term
                if (p->freqGroup <= 800)
                {
                    u1MR21Value[u1MRFsp] |= (0x1 << 5); // Low Speed CA buffer for LP4Y
                    #if LP4Y_BACKUP_SOLUTION
                    u1MR51Value[u1MRFsp] |= (0x3 << 1); // CLK[3]=0, WDQS[2]=1 and RDQS[1]=1 Single-End mode for LP4Y
                    #endif
                }
                else if ((p->freqGroup == 1200) || (p->freqGroup == 933))
#endif
                {
                    u1MR21Value[u1MRFsp] = 0;
                    u1MR51Value[u1MRFsp] = 0;
                }
                DramcModeRegWriteByRank(p, u4RankIdx, 21, u1MR21Value[u1MRFsp]);
                DramcModeRegWriteByRank(p, u4RankIdx, 51, u1MR51Value[u1MRFsp]);

                if(p->dram_type == TYPE_LPDDR4)
                {
                    u1MR22_Value = 0x20;     //SOC-ODT, ODTE-CK, ODTE-CS, Disable ODTD-CA
                }
                else  //TYPE_LPDDR4x, LP4P
                {
                    u1MR22_Value = 0x38;     //SOC-ODT, ODTE-CK, ODTE-CS, Disable ODTD-CA
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
                mcSHOW_DBG_MSG2(("CBT Vref Init: CH%d Rank%d FSP%d, Range %d Vref %d\n\n",p->channel, p->rank, u1MRFsp, u1MR12Value[u1ChannelIdx][u4RankIdx][u1MRFsp]>>6, (u1MR12Value[u1ChannelIdx][u4RankIdx][u1MRFsp] & 0x3f)));
                mcSHOW_DBG_MSG2(("TX Vref Init: CH%d Rank%d FSP%d, TX Range %d Vref %d\n\n",p->channel, p->rank, u1MRFsp,u1MR14Value[u1ChannelIdx][u4RankIdx][u1MRFsp]>>6, (u1MR14Value[u1ChannelIdx][u4RankIdx][u1MRFsp] & 0x3f)));
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
                // @Darren, Follow samsung PPR recommend flow
                DramcModeRegWriteByRank(p, u4RankIdx, 3, u1MR03Value[u1MRFsp]);
                DramcModeRegWriteByRank(p, u4RankIdx, 4, u1MR04Value[u4RankIdx]);
                DramcModeRegWriteByRank(p, u4RankIdx, 3, u1MR03Value[u1MRFsp]);
            }
            //else
            {
                /* FSP_1: 1. For term freqs   2. Assumes "data rate >= DDR2667" are terminated */
                u1MRFsp = FSP_1;
                mcSHOW_DBG_MSG2(("\tFsp%d\n", u1MRFsp));
            #if VENDER_JV_LOG
                mcSHOW_JV_LOG_MSG(("\tFsp%d\n", u1MRFsp));
            #endif

                DramcMRWriteFldAlign(p, 13, 1, MR13_FSP_WR, TO_MR);

                //MR12 use previous value
                #if CBT_FSP1_MATCH_FSP0_UNTERM_WA
                if (p->dram_fsp == FSP_0)
                    DramcModeRegWriteByRank(p, u4RankIdx, 12, u1MR12Value[u1ChannelIdx][u4RankIdx][FSP_0]); //MR12 VREF-CA
                else
                #endif
                DramcModeRegWriteByRank(p, u4RankIdx, 12, u1MR12Value[u1ChannelIdx][u4RankIdx][u1MRFsp]); //MR12 VREF-CA
                DramcModeRegWriteByRank(p, u4RankIdx, 1, u1MR01Value[u1MRFsp]);
                #if 0
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
                #endif
                DramcModeRegWriteByRank(p, u4RankIdx, 2, u1MR02Value[u1MRFsp]);

                if(p->dram_type == TYPE_LPDDR4P)
                    u1MR11_Value = 0x0;                  //ODT disable
                else
                {
#if ENABLE_SAMSUNG_NT_ODT
                    if ((p->vendor_id == VENDOR_SAMSUNG) && (p->revision_id == 0x7)) // 1ynm process for NT-ODT
                    {
                        u1MR11_Value = 0x2;              //@Darren, DQ ODT:120ohm -> parallel to 60ohm
                        u1MR11_Value |= (0x1 << 3);   //@Darren, MR11[3]=1 to enable NT-ODT for B707
                    }
                    else
#endif
                        u1MR11_Value = 0x3;              //DQ ODT:80ohm

                #if FSP1_CLKCA_TERM
                #if CBT_FSP1_MATCH_FSP0_UNTERM_WA
                if (p->dram_fsp == FSP_1)
                #endif
                {
                    if(p->dram_cbt_mode[u4RankIdx]==CBT_NORMAL_MODE)
                    {
                        u1MR11_Value |= 0x40;              //CA ODT:60ohm for normal mode die
                    }
                    else
                    {
                        u1MR11_Value |= 0x20;              //CA ODT:120ohm for byte mode die
                    }
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
                    u1MR22_Value = 0x24;     //SOC-ODT, ODTE-CK, ODTE-CS, Disable ODTD-CA
                }
                else  //TYPE_LPDDR4x, LP4P
                {
                    u1MR22_Value = 0x3c;     //Disable CA-CS-CLK ODT, SOC ODT=RZQ/4
                #if FSP1_CLKCA_TERM
                #if CBT_FSP1_MATCH_FSP0_UNTERM_WA
                if (p->dram_fsp == FSP_1)
                #endif
                {
                    if(u4RankIdx==RANK_0)
                    {
                        u1MR22_Value = 0x4;     //Enable CA-CS-CLK ODT, SOC ODT=RZQ/4
                    }
                    else
                    {
                        u1MR22_Value = 0x2c;     //Enable CS ODT, SOC ODT=RZQ/4
                    }
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
                mcSHOW_DBG_MSG2(("CBT Vref Init: CH%d Rank%d FSP%d, Range %d Vref %d\n\n",p->channel, p->rank, u1MRFsp, u1MR12Value[u1ChannelIdx][u4RankIdx][u1MRFsp]>>6, (u1MR12Value[u1ChannelIdx][u4RankIdx][u1MRFsp] & 0x3f)));
                mcSHOW_DBG_MSG2(("TX Vref Init: CH%d Rank%d FSP%d, TX Range %d Vref %d\n\n",p->channel, p->rank, u1MRFsp, u1MR14Value[u1ChannelIdx][u4RankIdx][u1MRFsp]>>6, (u1MR14Value[u1ChannelIdx][u4RankIdx][u1MRFsp] & 0x3f)));
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
                // @Darren, Follow samsung PPR recommend flow
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
        // Fix high freq keep FSP0 for CA term workaround (PPR abnormal)
        CmdOEOnOff(p, DISABLE, CMDOE_DIS_TO_ALL_CHANNEL);
        cbt_switch_freq(p, CBT_HIGH_FREQ);
        CmdOEOnOff(p, ENABLE, CMDOE_DIS_TO_ALL_CHANNEL);
    }
#endif
    vSetPHY2ChannelMapping(p, backup_channel);

#if ENABLE_DFS_RUNTIME_MRW
    DFSRuntimeMRW_preset_BeforeK(p, vGet_Current_SRAMIdx(p));
#endif

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
    DRV_WriteReg32(MPLL_PWR_CON0, 0x1); // turn off ISO of  MPLL
    mcDELAY_US(1);
    tmp = DRV_Reg32(MPLL_CON1);
    DRV_WriteReg32(MPLL_CON1, tmp | 0x80000000); // Config MPLL freq
    DRV_WriteReg32(MPLL_CON0, 0x181); // enable MPLL
    mcDELAY_US(20);
#endif
#else
    unsigned int tmp;

	tmp = DRV_Reg32(MPLL_CON4);
	DRV_WriteReg32(MPLL_CON4, tmp | 0x1); // turn off ISO of  MPLL


	tmp = DRV_Reg32(MPLL_CON4);
	DRV_WriteReg32(MPLL_CON4, tmp & 0xfffffffd); // turn off ISO of  MPLL


	tmp = DRV_Reg32(MPLL_CON0);
	DRV_WriteReg32(MPLL_CON0, tmp & 0xffffefff); // turn off ISO of  MPLL


	tmp = DRV_Reg32(MPLL_CON2);
 	DRV_WriteReg32(MPLL_CON2, 0x84200000); // turn off ISO of  MPLL


	tmp = DRV_Reg32(MPLL_CON0);
	DRV_WriteReg32(MPLL_CON0, tmp | 0x200); // turn off ISO of  MPLL

#endif
#endif
}


#if ENABLE_RODT_TRACKING_SAVE_MCK
static void SetTxWDQSStatusOnOff(U8 u1OnOff)
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

#if 0
static void ENABLE_XRTRTR_Setting(DRAMC_CTX_T * p)
{
#if 0 // @Darren, DV codes is included
    U8 u1ByteIdx = 0;
    U32 u4ByteOffset = 0;

    // DV codes is included
    for(u1ByteIdx=0; u1ByteIdx<DQS_NUMBER_LP4; u1ByteIdx++)
    {
        u4ByteOffset = u1ByteIdx*DDRPHY_AO_B0_B1_OFFSET;
        // PHY setting for B0/B1
        vIO32WriteFldAlign_All(DDRPHY_REG_B0_DLL_ARPI1 + u4ByteOffset, 0x1, B0_DLL_ARPI1_RG_ARPI_MCTL_JUMP_EN_B0);
        vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B0_DLL_ARPI3 + u4ByteOffset, 0x1, SHU_B0_DLL_ARPI3_RG_ARPI_MCTL_EN_B0);

        vIO32WriteFldMulti_All(DDRPHY_REG_SHU_B0_DQ2 + u4ByteOffset, P_Fld(0x1, SHU_B0_DQ2_RG_ARPI_PD_MCTL_SEL_B0)
                                           | P_Fld(0x1, SHU_B0_DQ2_RG_ARPI_OFFSET_LAT_EN_B0)
                                           | P_Fld(0x1, SHU_B0_DQ2_RG_ARPI_OFFSET_ASYNC_EN_B0));
        vIO32WriteFldMulti_All(DDRPHY_REG_SHU_B0_DQ10 + u4ByteOffset, P_Fld(0x1, SHU_B0_DQ10_RG_RX_ARDQS_DQSIEN_RANK_SEL_LAT_EN_B0)
                                           | P_Fld(0x1, SHU_B0_DQ10_RG_RX_ARDQS_RANK_SEL_LAT_EN_B0));
        vIO32WriteFldMulti_All(DDRPHY_REG_SHU_B0_DQ11 + u4ByteOffset, P_Fld(0x1, SHU_B0_DQ11_RG_RX_ARDQ_RANK_SEL_SER_EN_B0)
                                           | P_Fld(0x1, SHU_B0_DQ11_RG_RX_ARDQ_RANK_SEL_LAT_EN_B0));
        vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B0_DQ13 + u4ByteOffset, 0x1, SHU_B0_DQ13_RG_TX_ARDQ_DLY_LAT_EN_B0);
    }

    // DV codes is included
    vIO32WriteFldMulti_All(DDRPHY_REG_B0_DQ9, P_Fld(0x0, B0_DQ9_R_IN_GATE_EN_LOW_OPT_B0) | P_Fld(0x1, B0_DQ9_R_DMRXDVS_R_F_DLY_RK_OPT_B0));
    vIO32WriteFldMulti_All(DDRPHY_REG_B1_DQ9, P_Fld(0x0, B1_DQ9_R_IN_GATE_EN_LOW_OPT_B1) | P_Fld(0x1, B1_DQ9_R_DMRXDVS_R_F_DLY_RK_OPT_B1));
    //Darren-vIO32WriteFldMulti_All(DDRPHY_REG_CA_CMD9, P_Fld(0, CA_CMD9_R_IN_GATE_EN_LOW_OPT_CA) | P_Fld(0, CA_CMD9_R_DMRXDVS_R_F_DLY_RK_OPT));

    // DV codes is included
    vIO32WriteFldAlign_All(DDRPHY_REG_B0_DQ10, 0x1, B0_DQ10_ARPI_CG_RK1_SRC_SEL_B0);
    vIO32WriteFldAlign_All(DDRPHY_REG_B1_DQ10, 0x1, B1_DQ10_ARPI_CG_RK1_SRC_SEL_B1);

    // DV codes is included
    vIO32WriteFldMulti_All(DDRPHY_REG_MISC_STBCAL2, P_Fld(0x1, MISC_STBCAL2_DQSIEN_SELPH_BY_RANK_EN)
                                       | P_Fld(0x1, MISC_STBCAL2_STB_RST_BY_RANK)
                                       | P_Fld(0x1, MISC_STBCAL2_STB_IG_XRANK_CG_RST));

    //Extend 1T timing of FIFO mode rank switch
    // DV codes is included
    vIO32WriteFldAlign_All(DDRPHY_REG_B0_DQ9, 0x2, B0_DQ9_R_DMDQSIEN_RDSEL_LAT_B0);
    vIO32WriteFldAlign_All(DDRPHY_REG_B1_DQ9, 0x2, B1_DQ9_R_DMDQSIEN_RDSEL_LAT_B1);
    vIO32WriteFldAlign_All(DDRPHY_REG_B0_DQ9, 0x1, B0_DQ9_R_DMDQSIEN_VALID_LAT_B0);
    vIO32WriteFldAlign_All(DDRPHY_REG_B1_DQ9, 0x1, B1_DQ9_R_DMDQSIEN_VALID_LAT_B1);
#endif
}
#endif
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

    for (u1ByteIdx = 0; u1ByteIdx < DQS_BYTE_NUMBER; u1ByteIdx++)
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
#if 0
static void ENABLE_XRTWTW_Setting(DRAMC_CTX_T * p)
{
#if 0 // @Darren, DV codes is included
    // DV codes is included
    vIO32WriteFldMulti_All(DDRPHY_REG_SHU_B0_DQ2, P_Fld(0x1, SHU_B0_DQ2_RG_ARPI_OFFSET_ASYNC_EN_B0)
                                       | P_Fld(0x1, SHU_B0_DQ2_RG_ARPI_OFFSET_LAT_EN_B0));
    vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B0_DQ13, 0x1, SHU_B0_DQ13_RG_TX_ARDQ_DLY_LAT_EN_B0);

    // DV codes is included
    vIO32WriteFldMulti_All(DDRPHY_REG_SHU_B1_DQ2, P_Fld(0x1, SHU_B1_DQ2_RG_ARPI_OFFSET_ASYNC_EN_B1)
                                       | P_Fld(0x1, SHU_B1_DQ2_RG_ARPI_OFFSET_LAT_EN_B1));
    vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B1_DQ13, 0x1, SHU_B1_DQ13_RG_TX_ARDQ_DLY_LAT_EN_B1);

    // @Darren, CA don't care for xrank w2w
    vIO32WriteFldMulti_All(DDRPHY_REG_SHU_CA_CMD2, P_Fld(0x0, SHU_CA_CMD2_RG_ARPI_OFFSET_ASYNC_EN_CA)
                                       | P_Fld(0x0, SHU_CA_CMD2_RG_ARPI_OFFSET_LAT_EN_CA));
    vIO32WriteFldAlign_All(DDRPHY_REG_SHU_CA_CMD13, 0x1, SHU_CA_CMD13_RG_TX_ARCA_DLY_LAT_EN_CA);
#endif
}
#endif
#endif

#if CMD_CKE_WORKAROUND_FIX
static void CMD_CKE_Modified_txp_Setting(DRAMC_CTX_T * p)
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
    //    set 1'b1: adjust DQSOE/DQOE length with R_DMDQOE_CNT
    // 2. R_DMDQOE_CNT (dramc_conf 0x8C0[10:8])
    //    set 3'h3
    // 3. Initial TX setting OE/DATA
    //    OE = DATA - 4 UI

    // For LP3
    // 1. R_DMDQOE_OPT (dramc_conf 0x8C0[11])
    //    set 1'b1: adjust DQSOE/DQOE length with R_DMDQOE_CNT
    // 2. R_DMDQOE_CNT (dramc_conf 0x8C0[10:8])
    //    set 3'h2
    // 3. Initial TX setting OE/DATA
    //    OE = DATA - 2 UI

    u1DQ_OE_CNT = 3;

    vIO32WriteFldMulti(DRAMC_REG_SHU_TX_SET0, P_Fld(1, SHU_TX_SET0_DQOE_OPT) | P_Fld(u1DQ_OE_CNT, SHU_TX_SET0_DQOE_CNT));

    backup_rank = u1GetRank(p);

    LP4_ShiftDQS_OENUI(p, -1, ALL_BYTES);
    ShiftDQ_OENUI_AllRK(p, -1, ALL_BYTES);

}
#endif
#if 0
static DRAM_STATUS_T UpdateInitialSettings_LP4(DRAMC_CTX_T *p)
{
#if __A60868_TO_BE_PORTING__

    U16 u2RXVrefDefault = 0x8;
    U16 u1ChannelIdx, u1RankIdx, u1ByteIdx;
    U16 u2Clk_Dyn_Gating_Sel = 0x4, u2CG_CK_SEL = 0xb;
    #if CBT_MOVE_CA_INSTEAD_OF_CLK
    U8 u1CaPI = 0, u1CaUI = 0;
    U8 u1RankIdxBak;
    #endif

    if (p->odt_onoff == ODT_ON)
    {
        vIO32WriteFldAlign(DRAMC_REG_SHU_ODTCTRL, 1, SHU_ODTCTRL_ROEN);
        vIO32WriteFldAlign(DDRPHY_SHU_B0_DQ7, 0x1, SHU_B0_DQ7_R_DMRODTEN_B0);
        vIO32WriteFldAlign(DDRPHY_SHU_B1_DQ7, 0x1, SHU_B1_DQ7_R_DMRODTEN_B1);
        vIO32WriteFldMulti(DDRPHY_SHU_CA_CMD0, P_Fld(0x0, SHU_CA_CMD0_RG_TX_ARCMD_PRE_EN) // OE Suspend EN
                | P_Fld(0x1, SHU_CA_CMD0_RG_TX_ARCLK_PRE_EN)); //ODT Suspend EN
    }
    else
    {
        vIO32WriteFldAlign(DRAMC_REG_SHU_ODTCTRL, 0, SHU_ODTCTRL_ROEN);
        vIO32WriteFldAlign(DDRPHY_SHU_B0_DQ7, 0x0, SHU_B0_DQ7_R_DMRODTEN_B0);
        vIO32WriteFldAlign(DDRPHY_SHU_B1_DQ7, 0x0, SHU_B1_DQ7_R_DMRODTEN_B1);
        vIO32WriteFldMulti(DDRPHY_SHU_CA_CMD0, P_Fld(0x0, SHU_CA_CMD0_RG_TX_ARCMD_PRE_EN) // OE Suspend EN
                | P_Fld(0x0, SHU_CA_CMD0_RG_TX_ARCLK_PRE_EN)); //ODT Suspend EN
    }

    //close RX DQ/DQS tracking to save power
    vIO32WriteFldMulti(DDRPHY_R0_B0_RXDVS2, P_Fld(0x0, R0_B0_RXDVS2_R_RK0_DVS_MODE_B0)
                | P_Fld(0x0, R0_B0_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B0)
                | P_Fld(0x0, R0_B0_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B0));
    vIO32WriteFldMulti(DDRPHY_R1_B0_RXDVS2, P_Fld(0x0, R1_B0_RXDVS2_R_RK1_DVS_MODE_B0)
                | P_Fld(0x0, R1_B0_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_B0)
                | P_Fld(0x0, R1_B0_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_B0));
    vIO32WriteFldMulti(DDRPHY_R0_B1_RXDVS2, P_Fld(0x0, R0_B1_RXDVS2_R_RK0_DVS_MODE_B1)
                | P_Fld(0x0, R0_B1_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B1)
                | P_Fld(0x0, R0_B1_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B1));
    vIO32WriteFldMulti(DDRPHY_R1_B1_RXDVS2, P_Fld(0x0, R1_B1_RXDVS2_R_RK1_DVS_MODE_B1)
                | P_Fld(0x0, R1_B1_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_B1)
                | P_Fld(0x0, R1_B1_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_B1));
    //wei-jen: RX rank_sel for CA is not used(), set it's dly to 0 to save power
    vIO32WriteFldAlign(DDRPHY_SHU_CA_CMD7, 0, SHU_CA_CMD7_R_DMRANKRXDVS_CA); //Move to DCM off setting


    //DDRPhyTxRxInitialSettings_LP4
    vIO32WriteFldAlign(DDRPHY_CA_CMD3, 0x1, CA_CMD3_RG_RX_ARCMD_STBENCMP_EN);

    vIO32WriteFldAlign(DDRPHY_CA_CMD10, 0x1, CA_CMD10_RG_RX_ARCLK_DQSIENMODE);

    vIO32WriteFldAlign(DDRPHY_CA_CMD6, 0x2, CA_CMD6_RG_RX_ARCMD_BIAS_VREF_SEL);

    vIO32WriteFldMulti(DDRPHY_B0_DQ3, P_Fld(0x1, B0_DQ3_RG_RX_ARDQ_IN_BUFF_EN_B0)
                        | P_Fld(0x1, B0_DQ3_RG_RX_ARDQM0_IN_BUFF_EN_B0)
                        | P_Fld(0x1, B0_DQ3_RG_RX_ARDQS0_IN_BUFF_EN_B0));
    vIO32WriteFldMulti(DDRPHY_B1_DQ3, P_Fld(0x1, B1_DQ3_RG_RX_ARDQ_IN_BUFF_EN_B1)
                        | P_Fld(0x1, B1_DQ3_RG_RX_ARDQM0_IN_BUFF_EN_B1)
                        | P_Fld(0x1, B1_DQ3_RG_RX_ARDQS0_IN_BUFF_EN_B1));
    vIO32WriteFldMulti(DDRPHY_CA_CMD3, P_Fld(0x1, CA_CMD3_RG_RX_ARCMD_IN_BUFF_EN)
                            | P_Fld(0x1, CA_CMD3_RG_RX_ARCLK_IN_BUFF_EN));

    vIO32WriteFldAlign(DDRPHY_B0_DQ3, 0x0, B0_DQ3_RG_RX_ARDQ_SMT_EN_B0);
    vIO32WriteFldAlign(DDRPHY_B1_DQ3, 0x0, B1_DQ3_RG_RX_ARDQ_SMT_EN_B1);

    vIO32WriteFldAlign(DDRPHY_B0_DQ5, 0x1, B0_DQ5_RG_RX_ARDQS0_DVS_EN_B0);
    vIO32WriteFldAlign(DDRPHY_B1_DQ5, 0x1, B1_DQ5_RG_RX_ARDQS0_DVS_EN_B1);
    vIO32WriteFldAlign(DDRPHY_CA_CMD5, 0x1, CA_CMD5_RG_RX_ARCLK_DVS_EN);

    //LP4 no need, follow LP3 first.
    //vIO32WriteFldAlign(DDRPHY_MISC_VREF_CTRL, P_Fld(0x1, MISC_VREF_CTRL_RG_RVREF_DDR3_SEL)
    //                                        | P_Fld(0x0, MISC_VREF_CTRL_RG_RVREF_DDR4_SEL));


    vIO32WriteFldMulti(DDRPHY_CA_CMD6, P_Fld(0x0, CA_CMD6_RG_TX_ARCMD_DDR3_SEL)
                                    | P_Fld(0x1, CA_CMD6_RG_TX_ARCMD_DDR4_SEL)
                                    | P_Fld(0x0, CA_CMD6_RG_RX_ARCMD_DDR3_SEL)
                                    | P_Fld(0x1, CA_CMD6_RG_RX_ARCMD_DDR4_SEL));
    vIO32WriteFldMulti(DDRPHY_CA_CMD6, P_Fld(0x0, CA_CMD6_RG_TX_ARCMD_DDR3_SEL)
                                    | P_Fld(0x1, CA_CMD6_RG_TX_ARCMD_DDR4_SEL)
                                    | P_Fld(0x0, CA_CMD6_RG_RX_ARCMD_DDR3_SEL)
                                    | P_Fld(0x1, CA_CMD6_RG_RX_ARCMD_DDR4_SEL));
    vIO32WriteFldMulti(DDRPHY_MISC_IMP_CTRL0, P_Fld(0x0, MISC_IMP_CTRL0_RG_RIMP_DDR3_SEL)
                                    | P_Fld(0x1, MISC_IMP_CTRL0_RG_RIMP_DDR4_SEL));


    vIO32WriteFldAlign(DDRPHY_B0_DQ6, 0x0, B0_DQ6_RG_RX_ARDQ_O1_SEL_B0);
    vIO32WriteFldAlign(DDRPHY_B1_DQ6, 0x0, B1_DQ6_RG_RX_ARDQ_O1_SEL_B1);
    vIO32WriteFldAlign(DDRPHY_CA_CMD6, 0x0, CA_CMD6_RG_RX_ARCMD_O1_SEL);

    vIO32WriteFldAlign(DDRPHY_B0_DQ6, 0x1, B0_DQ6_RG_RX_ARDQ_BIAS_PS_B0);
    vIO32WriteFldAlign(DDRPHY_B1_DQ6, 0x1, B1_DQ6_RG_RX_ARDQ_BIAS_PS_B1);
    vIO32WriteFldAlign(DDRPHY_CA_CMD6, 0x1, CA_CMD6_RG_RX_ARCMD_BIAS_PS);
    vIO32WriteFldAlign(DDRPHY_CA_CMD6, 0x1, CA_CMD6_RG_RX_ARCMD_RES_BIAS_EN);

    vIO32WriteFldAlign(DDRPHY_B0_DQ6, 0x0, B0_DQ6_RG_TX_ARDQ_ODTEN_EXT_DIS_B0);
    vIO32WriteFldAlign(DDRPHY_B1_DQ6, 0x0, B1_DQ6_RG_TX_ARDQ_ODTEN_EXT_DIS_B1);
    vIO32WriteFldAlign(DDRPHY_CA_CMD6, 0x0, CA_CMD6_RG_TX_ARCMD_ODTEN_EXT_DIS);

    vIO32WriteFldAlign(DDRPHY_B0_DQ6, 0x1, B0_DQ6_RG_RX_ARDQ_RPRE_TOG_EN_B0);
    vIO32WriteFldAlign(DDRPHY_B1_DQ6, 0x1, B1_DQ6_RG_RX_ARDQ_RPRE_TOG_EN_B1);
    vIO32WriteFldAlign(DDRPHY_CA_CMD6, 0x1, CA_CMD6_RG_RX_ARCMD_RPRE_TOG_EN);

    if (p->dram_type == TYPE_LPDDR4)
    {
        if (p->odt_onoff == ODT_ON)
        {
            u2RXVrefDefault = 0x17; // 0.6*VDDQ, 0x12=0.5*VDDQ
        }
        else
        {
            u2RXVrefDefault = 0x29;
        }
    }
    else if (p->dram_type == TYPE_LPDDR4X)
    {
        if (p->odt_onoff == ODT_ON)
        {
            u2RXVrefDefault = 0x17; // 0.6*VDDQ, 0x12=0.5*VDDQ
        }
        else
        {
            u2RXVrefDefault = 0x29;
        }
    }
    else //if(p->dram_type == TYPE_LPDDR4P)
    {
        u2RXVrefDefault = 0x10;
    }

    #if CALIBRATION_SPEED_UP_DEBUG
    mcSHOW_DBG_MSG2(("\nInit Yulia RX Vref %d, apply to both rank0 and 1\n", u2RXVrefDefault));
    #endif

    vIO32WriteFldAlign(DDRPHY_SHU_B0_DQ5, u2RXVrefDefault, SHU_B0_DQ5_RG_RX_ARDQ_VREF_SEL_B0);
    vIO32WriteFldAlign(DDRPHY_SHU_B1_DQ5, u2RXVrefDefault, SHU_B1_DQ5_RG_RX_ARDQ_VREF_SEL_B1);
    vIO32WriteFldAlign(DDRPHY_B0_DQ5, u2RXVrefDefault, B0_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B0);
    vIO32WriteFldAlign(DDRPHY_B1_DQ5, u2RXVrefDefault, B1_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B1);

    for (u1ChannelIdx = CHANNEL_A; u1ChannelIdx < CHANNEL_NUM; u1ChannelIdx++)
    {
        for (u1RankIdx = RANK_0; u1RankIdx < RANK_MAX; u1RankIdx++)
        {
            for (u1ByteIdx = 0; u1ByteIdx < DQS_BYTE_NUMBER; u1ByteIdx++)
            {
                gFinalRXVrefDQ[u1ChannelIdx][u1RankIdx][u1ByteIdx] = u2RXVrefDefault;
            }
        }
    }

    if ((p->dram_type == TYPE_LPDDR4X) || (p->dram_type == TYPE_LPDDR4P))
    {
        // LP4x eye fine-tune
        // APHY Review by YY Hsu
        vIO32WriteFldAlign(DDRPHY_B0_DQ8, 0x1, B0_DQ8_RG_TX_ARDQ_EN_LP4P_B0);
        vIO32WriteFldAlign(DDRPHY_B1_DQ8, 0x1, B1_DQ8_RG_TX_ARDQ_EN_LP4P_B1);
        vIO32WriteFldAlign(DDRPHY_CA_CMD9, 0x1, CA_CMD9_RG_TX_ARCMD_EN_LP4P);
    }


    /* Set initial default mode to "new burst mode (7UI or new 8UI)" */
    DramcGatingMode(p, 1);

    vIO32WriteFldAlign(DDRPHY_CA_CMD8, 0x1, CA_CMD8_RG_TX_RRESETB_DDR3_SEL);
    vIO32WriteFldAlign(DDRPHY_CA_CMD8, 0x0, CA_CMD8_RG_TX_RRESETB_DDR4_SEL); //TODO: Remove if register default value is 0
    //End of DDRPhyTxRxInitialSettings_LP4

    //DFS workaround
    vIO32WriteFldAlign(DRAMC_REG_SHU_MISC, 0x2, SHU_MISC_REQQUE_MAXCNT);

    //should set 0x2a, otherwise AC-timing violation from Berson
    vIO32WriteFldMulti(DRAMC_REG_SHU_DQSG, P_Fld(0x2a, SHU_DQSG_SCINTV) | P_Fld(0x1, SHU_DQSG_DQSINCTL_PRE_SEL));


    //Update setting for
    vIO32WriteFldAlign(DDRPHY_SHU_B0_DQ5, 0x0, SHU_B0_DQ5_RG_ARPI_FB_B0);
    vIO32WriteFldAlign(DDRPHY_SHU_B1_DQ5, 0x0, SHU_B1_DQ5_RG_ARPI_FB_B1);
    vIO32WriteFldAlign(DDRPHY_SHU_CA_CMD5, 0x0, SHU_CA_CMD5_RG_ARPI_FB_CA);


    //Reserved bits usage, check with PHY owners
    DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
    vIO32WriteFldAlign_All(DDRPHY_SHU_B0_DQ6, 0x0, SHU_B0_DQ6_RG_ARPI_OFFSET_DQSIEN_B0);
    vIO32WriteFldAlign_All(DDRPHY_SHU_B1_DQ6, 0x0, SHU_B1_DQ6_RG_ARPI_OFFSET_DQSIEN_B1);
    vIO32WriteFldMulti_All(DDRPHY_SHU_CA_CMD6, P_Fld(0x0, SHU_CA_CMD6_RG_ARPI_OFFSET_CLKIEN)
                                        | P_Fld(0x0, SHU_CA_CMD6_RG_ARPI_OFFSET_MCTL_CA));
    DramcBroadcastOnOff(DRAMC_BROADCAST_ON);

    //IMP Tracking Init Settings
    //Write (DRAMC _BASE+ 0x219) [31:0] = 32'h80080020//DDR3200 default
    //SHU_IMPCAL1_IMPCAL_CHKCYCLE should > 12.5/MCK, 1:4 mode will disable imp tracking -> don't care
    vIO32WriteFldMulti(DRAMC_REG_SHU_IMPCAL1, P_Fld(8, SHU_IMPCAL1_IMPCAL_CALICNT) | P_Fld(0x10, SHU_IMPCAL1_IMPCALCNT)
                                            | P_Fld(4, SHU_IMPCAL1_IMPCAL_CALEN_CYCLE) | P_Fld(1, SHU_IMPCAL1_IMPCALCNT_OPT)
                                            | P_Fld((p->frequency * 25 / 8000) + 1, SHU_IMPCAL1_IMPCAL_CHKCYCLE));

    //for _K_
    vIO32WriteFldMulti(DRAMC_REG_SREFCTRL, P_Fld(0x1, SREFCTRL_SCSM_CGAR)
                                            | P_Fld(0x1, SREFCTRL_SCARB_SM_CGAR)
                                            | P_Fld(0x1, SREFCTRL_RDDQSOSC_CGAR)
                                            | P_Fld(0x1, SREFCTRL_HMRRSEL_CGAR));
    vIO32WriteFldAlign(DRAMC_REG_PRE_TDQSCK1, 0x1, PRE_TDQSCK1_TXUIPI_CAL_CGAR);
    /* DVFS related, PREA interval counter (After DVFS DVT, set to 0xf (originally was 0x1f)) */
    vIO32WriteFldAlign(DRAMC_REG_SHU_MISC, 0xf, SHU_MISC_PREA_INTV);
    vIO32WriteFldMulti(DDRPHY_SHU_B0_DQ8, P_Fld(0x1, SHU_B0_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B0)
                                        | P_Fld(0x1, SHU_B0_DQ8_R_DMRANK_PIPE_CG_IG_B0)
                                        | P_Fld(0x1, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B0)
                                        | P_Fld(0x1, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B0)
                                        | P_Fld(0x1, SHU_B0_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B0)
                                        | P_Fld(0x1, SHU_B0_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B0)
                                        | P_Fld(0x1, SHU_B0_DQ8_R_DMSTBEN_SYNC_CG_IG_B0)
                                        | P_Fld(0x1, SHU_B0_DQ8_R_DMRXDLY_CG_IG_B0)
                                        | P_Fld(0x1, SHU_B0_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B0)
                                        | P_Fld(0x1, SHU_B0_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B0)
                                        | P_Fld(0x0, SHU_B0_DQ8_R_DMRXDVS_UPD_FORCE_EN_B0)
                                        | P_Fld(0x7fff, SHU_B0_DQ8_R_DMRXDVS_UPD_FORCE_CYC_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_B1_DQ8, P_Fld(0x1, SHU_B1_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B1)
                                        | P_Fld(0x1, SHU_B1_DQ8_R_DMRANK_PIPE_CG_IG_B1)
                                        | P_Fld(0x1, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B1)
                                        | P_Fld(0x1, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B1)
                                        | P_Fld(0x1, SHU_B1_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B1)
                                        | P_Fld(0x1, SHU_B1_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B1)
                                        | P_Fld(0x1, SHU_B1_DQ8_R_DMSTBEN_SYNC_CG_IG_B1)
                                        | P_Fld(0x1, SHU_B1_DQ8_R_DMRXDLY_CG_IG_B1)
                                        | P_Fld(0x1, SHU_B1_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B1)
                                        | P_Fld(0x1, SHU_B1_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B1)
                                        | P_Fld(0x0, SHU_B1_DQ8_R_DMRXDVS_UPD_FORCE_EN_B1)
                                        | P_Fld(0x7fff, SHU_B1_DQ8_R_DMRXDVS_UPD_FORCE_CYC_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_CA_CMD8, P_Fld(0x1, SHU_CA_CMD8_R_DMRANK_CHG_PIPE_CG_IG_CA)
                                        | P_Fld(0x1, SHU_CA_CMD8_R_DMRANK_PIPE_CG_IG_CA)
                                        | P_Fld(0x1, SHU_CA_CMD8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_CA)
                                        | P_Fld(0x1, SHU_CA_CMD8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_CA)
                                        | P_Fld(0x1, SHU_CA_CMD8_R_DMDQSIEN_FLAG_PIPE_CG_IG_CA)
                                        | P_Fld(0x1, SHU_CA_CMD8_R_DMDQSIEN_FLAG_SYNC_CG_IG_CA)
                                        | P_Fld(0x1, SHU_CA_CMD8_R_DMSTBEN_SYNC_CG_IG_CA)
                                        | P_Fld(0x1, SHU_CA_CMD8_R_DMRXDLY_CG_IG_CA)
                                        | P_Fld(0x1, SHU_CA_CMD8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_CA)
                                        | P_Fld(0x1, SHU_CA_CMD8_R_DMRXDVS_RDSEL_PIPE_CG_IG_CA)
                                        | P_Fld(0x0, SHU_CA_CMD8_R_DMRXDVS_UPD_FORCE_EN_CA)
                                        | P_Fld(0x7fff, SHU_CA_CMD8_R_DMRXDVS_UPD_FORCE_CYC_CA));
    vIO32WriteFldAlign(DDRPHY_MISC_CTRL3, 0x1, MISC_CTRL3_R_DDRPHY_COMB_CG_IG);
    /* HW design issue: run-time PBYTE (B0, B1) flags will lose it's function and become per-bit -> set to 0 */
    vIO32WriteFldMulti(DDRPHY_SHU_B0_DQ7, P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B0)
                                        | P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B0)
                                        | P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_DQM_FLAGSEL_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_B1_DQ7, P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B1)
                                        | P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B1)
                                        | P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_DQM_FLAGSEL_B1));

    vIO32WriteFldMulti(DRAMC_REG_CLKAR, P_Fld(0x1, CLKAR_SELPH_CMD_CG_DIS) | P_Fld(0x7FFF, CLKAR_REQQUE_PACG_DIS));

    vIO32WriteFldAlign(DRAMC_REG_SHU_DQSG_RETRY, 0x0, SHU_DQSG_RETRY_R_RETRY_PA_DSIABLE); //SH: Set to 0 -> save power
    vIO32WriteFldAlign(DRAMC_REG_WRITE_LEV, 0x0, WRITE_LEV_DDRPHY_COMB_CG_SEL);
    vIO32WriteFldAlign(DRAMC_REG_DUMMY_RD, 0x1, DUMMY_RD_DUMMY_RD_PA_OPT);
    vIO32WriteFldMulti(DRAMC_REG_STBCAL2, P_Fld(0x0, STBCAL2_STB_UIDLYCG_IG)
                                            | P_Fld(0x0, STBCAL2_STB_PIDLYCG_IG));
    vIO32WriteFldMulti(DRAMC_REG_EYESCAN, P_Fld(0x1, EYESCAN_EYESCAN_DQS_SYNC_EN)
                                            | P_Fld(0x1, EYESCAN_EYESCAN_NEW_DQ_SYNC_EN)
                                            | P_Fld(0x1, EYESCAN_EYESCAN_DQ_SYNC_EN));
    //vIO32WriteFldMulti(DRAMC_REG_SHU_ODTCTRL, P_Fld(0x1, SHU_ODTCTRL_RODTENSTB_SELPH_CG_IG)
    //                                        | P_Fld(0x1, SHU_ODTCTRL_RODTEN_SELPH_CG_IG));

    vIO32WriteFldAlign(DDRPHY_SHU_B0_DLL0, 0x1, SHU_B0_DLL0_RG_ARPISM_MCK_SEL_B0_SHU);//Move to DCM off setting
    vIO32WriteFldAlign(DDRPHY_SHU_B1_DLL0, 0x1, SHU_B1_DLL0_RG_ARPISM_MCK_SEL_B1_SHU);//Move to DCM off setting
    //vIO32WriteFldAlign(DDRPHY_SHU_CA_DLL0, 0x1, SHU_CA_DLL0_RG_ARPISM_MCK_SEL_CA_SHU); move to DramcSetting_Olympus_LP4_ByteMode()

    vIO32WriteFldAlign(DDRPHY_CA_DLL_ARPI1, 0x1, CA_DLL_ARPI1_RG_ARPISM_MCK_SEL_CA);

    //end _K_

    //DE_UPDATE
    #if (fcFOR_CHIP_ID == fcLafite)
    // Must check with EMI owners -> Asynchronous EMI: Can't turn on RWSPLIT, Synchronous EMI: Can enable RWSPLIT (DE: JL Wu)
    vIO32WriteFldMulti(DRAMC_REG_PERFCTL0, P_Fld(0x1, PERFCTL0_WRFIFO_OPT)
                                        | P_Fld(0x0, PERFCTL0_REORDEREN) // from Unique  review (it will remove)
                                        | P_Fld(0x1, PERFCTL0_RWSPLIT)); //synchronous EMI -> can turn on RWSPLIT
    #endif
    vIO32WriteFldAlign(DRAMC_REG_SREFCTRL, 0x1, SREFCTRL_SREF2_OPTION);
    vIO32WriteFldAlign(DRAMC_REG_SHUCTRL1, 0x1a, SHUCTRL1_FC_PRDCNT);

#ifdef XRTR2R_PERFORM_ENHANCE_DQSG_RX_DLY
    vIO32WriteFldMulti(DDRPHY_B0_DQ6, P_Fld(0x1, B0_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B0)
                                | P_Fld(0x1, B0_DQ6_RG_RX_ARDQ_BIAS_EN_B0));
    vIO32WriteFldMulti(DDRPHY_B1_DQ6, P_Fld(0x1, B1_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B1)
                                | P_Fld(0x1, B1_DQ6_RG_RX_ARDQ_BIAS_EN_B1));
    vIO32WriteFldMulti(DDRPHY_CA_CMD6, P_Fld(0x1, CA_CMD6_RG_RX_ARCMD_OP_BIAS_SW_EN)
                                    | P_Fld(0x1, CA_CMD6_RG_RX_ARCMD_BIAS_EN));
    vIO32WriteFldAlign(DRAMC_REG_STBCAL2, 0x1, STBCAL2_STB_PICG_EARLY_1T_EN);
    vIO32WriteFldMulti(DDRPHY_SHU_B0_DQ7, P_Fld(0x0, SHU_B0_DQ7_R_DMRXRANK_DQS_LAT_B0)
                                    | P_Fld(0x1, SHU_B0_DQ7_R_DMRXRANK_DQS_EN_B0)
                                    | P_Fld(0x1, SHU_B0_DQ7_R_DMRXRANK_DQ_LAT_B0)
                                    | P_Fld(0x1, SHU_B0_DQ7_R_DMRXRANK_DQ_EN_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_B1_DQ7, P_Fld(0x0, SHU_B1_DQ7_R_DMRXRANK_DQS_LAT_B1)
                                    | P_Fld(0x1, SHU_B1_DQ7_R_DMRXRANK_DQS_EN_B1)
                                    | P_Fld(0x1, SHU_B1_DQ7_R_DMRXRANK_DQ_LAT_B1)
                                    | P_Fld(0x1, SHU_B1_DQ7_R_DMRXRANK_DQ_EN_B1));
#else
    vIO32WriteFldMulti(DDRPHY_B0_DQ6, P_Fld(0x0, B0_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B0)
                                | P_Fld(0x0, B0_DQ6_RG_RX_ARDQ_BIAS_EN_B0));
    vIO32WriteFldMulti(DDRPHY_B1_DQ6, P_Fld(0x0, B1_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B1)
                                | P_Fld(0x0, B1_DQ6_RG_RX_ARDQ_BIAS_EN_B1));
    vIO32WriteFldMulti(DDRPHY_CA_CMD6, P_Fld(0x0, CA_CMD6_RG_RX_ARCMD_OP_BIAS_SW_EN)
                                    | P_Fld(0x0, CA_CMD6_RG_RX_ARCMD_BIAS_EN));
#endif

    //end DE UPDATE

    //Disable RODT tracking
    //vIO32WriteFldAlign(DRAMC_REG_SHU_RODTENSTB, 0, SHU_RODTENSTB_RODTENSTB_TRACK_EN);

    //Rx Gating tracking settings
    vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_DQSG), \
                                                P_Fld(9, SHU_DQSG_STB_UPDMASKCYC) | \
                                                P_Fld(1, SHU_DQSG_STB_UPDMASK_EN));
    vIO32WriteFldMulti(DRAMC_REG_SHURK0_DQSCAL, P_Fld(0, SHURK0_DQSCAL_R0DQSIENLLMTEN) | P_Fld(0, SHURK0_DQSCAL_R0DQSIENHLMTEN));
    vIO32WriteFldMulti(DRAMC_REG_SHURK1_DQSCAL, P_Fld(0, SHURK1_DQSCAL_R1DQSIENLLMTEN) | P_Fld(0, SHURK1_DQSCAL_R1DQSIENHLMTEN));
    vIO32WriteFldMulti(DRAMC_REG_SHU_STBCAL, P_Fld(1, SHU_STBCAL_DQSG_MODE)
                                        | P_Fld(1, SHU_STBCAL_PICGLAT));

#ifdef XRTR2R_PERFORM_ENHANCE_DQSG_RX_DLY
    vIO32WriteFldAlign(DDRPHY_B0_DQ9, 0x4, B0_DQ9_R_IN_GATE_EN_LOW_OPT_B0);
    vIO32WriteFldAlign(DDRPHY_B1_DQ9, 0x4, B1_DQ9_R_IN_GATE_EN_LOW_OPT_B1);
#else
    //Modify for corner IC failed at HQA test XTLV
    vIO32WriteFldAlign(DDRPHY_B0_DQ9, 0x7, B0_DQ9_R_IN_GATE_EN_LOW_OPT_B0);
    vIO32WriteFldAlign(DDRPHY_B1_DQ9, 0x7, B1_DQ9_R_IN_GATE_EN_LOW_OPT_B1);
#endif
    vIO32WriteFldAlign(DDRPHY_CA_CMD10, 0x0, CA_CMD10_R_IN_GATE_EN_LOW_OPT_CA);

    vIO32WriteFldAlign(DDRPHY_SHU_B0_DQ8, 0x1, SHU_B0_DQ8_R_DMRXDLY_CG_IG_B0);
    vIO32WriteFldAlign(DDRPHY_SHU_B1_DQ8, 0x1, SHU_B1_DQ8_R_DMRXDLY_CG_IG_B1);

#ifdef DUMMY_READ_FOR_DQS_GATING_RETRY
    if (p->support_rank_num == RANK_SINGLE)
    {
        vIO32WriteFldAlign(DRAMC_REG_SHU_DQSG_RETRY, 1, SHU_DQSG_RETRY_R_RETRY_1RANK);
    }
#endif


    #if ENABLE_TX_WDQS
    mcSHOW_DBG_MSG2(("Enable WDQS\n"));
    //Check reserved bits with PHY integrator
    vIO32WriteFldMulti(DDRPHY_SHU_B0_DLL1, P_Fld(1, SHU_B0_DLL1_RG_READ_BASE_DQS_EN_B0) | P_Fld(1, SHU_B0_DLL1_RG_READ_BASE_DQSB_EN_B0)
                                        | P_Fld(!p->odt_onoff, SHU_B0_DLL1_RG_ODT_DISABLE_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_B1_DLL1, P_Fld(1, SHU_B1_DLL1_RG_READ_BASE_DQS_EN_B1) | P_Fld(1, SHU_B1_DLL1_RG_READ_BASE_DQSB_EN_B1)
                                        | P_Fld(!p->odt_onoff, SHU_B1_DLL1_RG_ODT_DISABLE_B1));
    vIO32WriteFldMulti(DRAMC_REG_SHU_ODTCTRL, P_Fld(0x1, SHU_ODTCTRL_RODTE)
            | P_Fld(0x1, SHU_ODTCTRL_RODTE2)
            | P_Fld(0x1, SHU_ODTCTRL_ROEN));

    vIO32WriteFldAlign(DDRPHY_SHU_B0_DQ7, 0x1, SHU_B0_DQ7_R_DMRODTEN_B0);
    vIO32WriteFldAlign(DDRPHY_SHU_B1_DQ7, 0x1, SHU_B1_DQ7_R_DMRODTEN_B1);
    #if ENABLE_RODT_TRACKING_SAVE_MCK
    SetTxWDQSStatusOnOff(1);
    #endif

    #else //WDQS and reak pull are disable
    //Check reserved bits with PHY integrator
    vIO32WriteFldMulti(DDRPHY_SHU_B0_DLL1, P_Fld(0, SHU_B0_DLL1_RG_READ_BASE_DQS_EN_B0) | P_Fld(0, SHU_B0_DLL1_RG_READ_BASE_DQSB_EN_B0)
                                        | P_Fld(0, SHU_B0_DLL1_RG_ODT_DISABLE_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_B1_DLL1, P_Fld(0, SHU_B1_DLL1_RG_READ_BASE_DQS_EN_B1) | P_Fld(0, SHU_B1_DLL1_RG_READ_BASE_DQSB_EN_B1)
                                        | P_Fld(0, SHU_B1_DLL1_RG_ODT_DISABLE_B1));
    #endif


    //DE review WhitneyE2
    vIO32WriteFldAlign(DRAMC_REG_DRSCTRL, 0x1, DRSCTRL_DRS_SELFWAKE_DMYRD_DIS);
    vIO32WriteFldAlign(DRAMC_REG_REFCTRL0, 0x1, REFCTRL0_REFNA_OPT);
    vIO32WriteFldAlign(DRAMC_REG_ZQCS, 0x1, ZQCS_ZQCS_MASK_SEL_CGAR);//Move to DCM off setting
    vIO32WriteFldMulti(DRAMC_REG_DUMMY_RD, P_Fld(0x1, DUMMY_RD_DMYRD_REORDER_DIS) | P_Fld(0x0, DUMMY_RD_DMYRD_HPRI_DIS)); //La_fite MP setting = 0
    vIO32WriteFldAlign(DRAMC_REG_SHUCTRL2, 0x1, SHUCTRL2_R_DVFS_SREF_OPT);
    vIO32WriteFldAlign(DRAMC_REG_SHUCTRL3, 0xb, SHUCTRL3_VRCGDIS_PRDCNT);
    vIO32WriteFldAlign(DDRPHY_MISC_CTRL3, 0x1, MISC_CTRL3_R_DDRPHY_RX_PIPE_CG_IG);
    //End

    //DE review
    /* ARPISM_MCK_SEL_B0, B1 set to 1 (Joe): "Due to TX_PICG modify register is set to 1,
     * ARPISM_MCK_SEL_Bx should be 1 to fulfill APHY TX OE spec for low freq (Ex: DDR1600)"
     */
    vIO32WriteFldMulti(DDRPHY_B0_DLL_ARPI1, P_Fld(0x1, B0_DLL_ARPI1_RG_ARPISM_MCK_SEL_B0_REG_OPT)
                                        | P_Fld(0x1, B0_DLL_ARPI1_RG_ARPISM_MCK_SEL_B0));
    vIO32WriteFldMulti(DDRPHY_B1_DLL_ARPI1, P_Fld(0x1, B1_DLL_ARPI1_RG_ARPISM_MCK_SEL_B1_REG_OPT)
                                        | P_Fld(0x1, B1_DLL_ARPI1_RG_ARPISM_MCK_SEL_B1));
    vIO32WriteFldAlign(DDRPHY_CA_DLL_ARPI1, 0x1, CA_DLL_ARPI1_RG_ARPISM_MCK_SEL_CA_REG_OPT);//Move to DCM off setting
    vIO32WriteFldAlign(DDRPHY_MISC_CTRL0, 0, MISC_CTRL0_R_DMSHU_PHYDCM_FORCEOFF);//Move to DCM off setting

    vIO32WriteFldAlign(DDRPHY_MISC_RXDVS2, 1, MISC_RXDVS2_R_DMRXDVS_SHUFFLE_CTRL_CG_IG);
    vIO32WriteFldAlign(DRAMC_REG_CLKCTRL, 0x1, CLKCTRL_SEQCLKRUN3);
    vIO32WriteFldAlign(DRAMC_REG_REFCTRL1, 1, REFCTRL1_SREF_CG_OPT);
    vIO32WriteFldMulti(DRAMC_REG_SHUCTRL, P_Fld(0x0, SHUCTRL_DVFS_CG_OPT) | P_Fld(0x3, SHUCTRL_R_DVFS_PICG_MARGIN2) | P_Fld(0x3, SHUCTRL_R_DVFS_PICG_MARGIN3));//Move to DCM off setting(SHUCTRL_DVFS_CG_OPT)
    //vIO32WriteFldMulti(DRAMC_REG_SHUCTRL, P_Fld(0x3, SHUCTRL_R_DVFS_PICG_MARGIN2) | P_Fld(0x3, SHUCTRL_R_DVFS_PICG_MARGIN3));
    vIO32WriteFldMulti(DRAMC_REG_SHUCTRL2, P_Fld(0x1, SHUCTRL2_SHORTQ_OPT) | P_Fld(0x3, SHUCTRL2_R_DVFS_PICG_MARGIN));
    vIO32WriteFldAlign(DRAMC_REG_STBCAL2, 0x0, STBCAL2_STB_DBG_EN);
    vIO32WriteFldMulti(DRAMC_REG_PRE_TDQSCK1, P_Fld(0x0, PRE_TDQSCK1_APHY_CG_OPT1) | P_Fld(0x0, PRE_TDQSCK1_SHU_PRELOAD_TX_HW));

    #ifndef FIRST_BRING_UP
    if (u2DFSGetHighestFreq(p) >= 1866)
    #endif
    {
        //if product supports 3733, CLKAR_SELPH_4LCG_DIS always 1 else o, but if 1, comsume more power
        vIO32WriteFldAlign(DRAMC_REG_CLKAR, 1, CLKAR_SELPH_4LCG_DIS);
    }

    #if TX_OE_EXTEND
    UpdateTxOEN(p);
    #endif

    vIO32WriteFldAlign(DRAMC_REG_CKECTRL, 0x1, CKECTRL_CKEPBDIS);

    vIO32WriteFldMulti(DDRPHY_CA_TX_MCK, P_Fld(0x1, CA_TX_MCK_R_DMRESET_FRPHY_OPT) | P_Fld(0xa, CA_TX_MCK_R_DMRESETB_DRVP_FRPHY) | P_Fld(0xa, CA_TX_MCK_R_DMRESETB_DRVN_FRPHY));

    //Syl_via MP setting should set CKECTRL_CKELCKFIX as 0 to rollback to M17
    //Ei_ger review by CF Chang
    vIO32WriteFldAlign(DRAMC_REG_CKECTRL, 0x0, CKECTRL_CKELCKFIX);

    //Gating error problem happened in M17 has been solved by setting this RG as 0 (when RODT tracking on (), TX DLY of byte2,3 must not be zero)
    vIO32WriteFldAlign(DRAMC_REG_SHU_RODTENSTB, 0x0, SHU_RODTENSTB_RODTENSTB_4BYTE_EN);

    #if (fcFOR_CHIP_ID == fcLafite) //RODT old mode
    // RODT old mode (it must set SHU_RODTENSTB_RODTENSTB_TRACK_EN=1) from HJ review
    vIO32WriteFldAlign(DRAMC_REG_SHU_ODTCTRL, 0x1, SHU_ODTCTRL_RODTEN_OPT);
    vIO32WriteFldMulti(DRAMC_REG_SHU_RODTENSTB, P_Fld(0, SHU_RODTENSTB_RODTEN_P1_ENABLE)
                                        | P_Fld(1, SHU_RODTENSTB_RODTENSTB_TRACK_UDFLWCTRL)
                                        | P_Fld(1, SHU_RODTENSTB_RODTENSTB_SELPH_MODE));
    vIO32WriteFldAlign(DRAMC_REG_SHU_STBCAL, 0x0, SHU_STBCAL_DQSIEN_PICG_MODE);
    #if ENABLE_RODT_TRACKING // The SHU_RODTENSTB_RODTENSTB_TRACK_EN will depend on SHU_ODTCTRL_ROEN setting
    if (vGet_Div_Mode(p) == DIV4_MODE)
        vIO32WriteFldAlign(DRAMC_REG_SHU_RODTENSTB, 0x21, SHU_RODTENSTB_RODTENSTB_OFFSET);
    else
        vIO32WriteFldAlign(DRAMC_REG_SHU_RODTENSTB, 0x11, SHU_RODTENSTB_RODTENSTB_OFFSET);
    #endif

    //Enable RODT tracking at Init, because RODT position should be changed with Gating position simultaneously
    U8 u1ReadROEN;
    u1ReadROEN = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_ODTCTRL), SHU_ODTCTRL_ROEN);
    vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_RODTENSTB), P_Fld(0xff, SHU_RODTENSTB_RODTENSTB_EXT) | \
                                                                P_Fld(u1ReadROEN, SHU_RODTENSTB_RODTENSTB_TRACK_EN));
    #endif

    #if (fcFOR_CHIP_ID == fcLafite) //DDRPHY settings review from YH Cho
    vIO32WriteFldAlign(DDRPHY_SHU_B0_DQ3, 0x0, SHU_B0_DQ3_RG_TX_ARDQS0_PU_PRE_B0);
    vIO32WriteFldAlign(DDRPHY_SHU_B1_DQ3, 0x0, SHU_B1_DQ3_RG_TX_ARDQS0_PU_PRE_B1);
    #endif

    #if ENABLE_FIX_PRE8_DVFS_AT_CKEPRD
    vIO32WriteFldAlign(DRAMC_REG_MISCTL0, 0x1, MISCTL0_PG_WAKEUP_OPT);
    #endif

#if ENABLE_TMRRI_NEW_MODE
    //[DVT](1)dram auto refersh rate by hardware mr4 for rank0 and rank1; (2)After suspend resume, HW MR4 will be fire autoly (Berson)
    vIO32WriteFldAlign(DRAMC_REG_SPCMDCTRL, (p->support_rank_num == RANK_DUAL)? (1): (0), SPCMDCTRL_HMR4_TOG_OPT);
#else
    vIO32WriteFldAlign(DRAMC_REG_SPCMDCTRL, 0x0, SPCMDCTRL_HMR4_TOG_OPT);
#endif

#if RX_PICG_NEW_MODE
    RXPICGSetting(p);
#endif

    //[DVT]RX FIFO debug feature, MP setting should enable debug function
    vIO32WriteFldAlign(DDRPHY_B0_DQ9, 0x1, B0_DQ9_R_DMRXFIFO_STBENCMP_EN_B0);
    vIO32WriteFldAlign(DDRPHY_B1_DQ9, 0x1, B1_DQ9_R_DMRXFIFO_STBENCMP_EN_B1);

    // E2 - new -start =========================================================================
    //Design Review Meeting dramc_stbcal_cross rank read to read for new APHY spec - HJ Huang
    vIO32WriteFldMulti(DDRPHY_B0_DQ9, P_Fld(4, B0_DQ9_R_IN_GATE_EN_LOW_OPT_B0) | P_Fld(0, B0_DQ9_R_DMRXDVS_R_F_DLY_RK_OPT_B0));
    vIO32WriteFldMulti(DDRPHY_B1_DQ9, P_Fld(4, B1_DQ9_R_IN_GATE_EN_LOW_OPT_B1) | P_Fld(0, B1_DQ9_R_DMRXDVS_R_F_DLY_RK_OPT_B1));
    vIO32WriteFldMulti(DDRPHY_CA_CMD10, P_Fld(0, CA_CMD10_R_IN_GATE_EN_LOW_OPT_CA) | P_Fld(0, CA_CMD10_R_DMRXDVS_R_F_DLY_RK_OPT));
    vIO32WriteFldAlign(DDRPHY_MISC_CTRL3, 0x0, MISC_CTRL3_ARPI_CG_RK1_SRC_SEL);
    vIO32WriteFldAlign(DRAMC_REG_SHU_RANK_SEL_STB, 0x1, SHU_RANK_SEL_STB_RANK_SEL_RXDLY_TRACK);
    vIO32WriteFldAlign(DRAMC_REG_SHU_RANK_SEL_STB, 0x0, SHU_RANK_SEL_STB_RANK_SEL_STB_SERMODE); // for old rank mode settings - HJ Huang

    // Update VREF1/VREF2 per bit select - Ying-Yu Hsu
    // Note: The RG (SHU_B1_DQ3_RG_TX_ARDQS0_PDB_B1) need update when using VREF2 per bit select
    vIO32WriteFldMulti(DDRPHY_SHU_B0_DQ3, P_Fld(0, SHU_B0_DQ3_RG_TX_ARDQS0_PDB_B0) | P_Fld(0, SHU_B0_DQ3_RG_TX_ARDQ_PDB_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_B1_DQ3, P_Fld(0, SHU_B1_DQ3_RG_TX_ARDQS0_PDB_B1) | P_Fld(0, SHU_B1_DQ3_RG_TX_ARDQ_PDB_B1));

    // DQSI DLY 3B in FIFO for adjustmen by YingMin
    vIO32WriteFldAlign(DDRPHY_SHU_B0_DQ5, 0x4, SHU_B0_DQ5_RG_RX_ARDQ_FIFO_DQSI_DLY_B0);
    vIO32WriteFldAlign(DDRPHY_SHU_B1_DQ5, 0x4, SHU_B1_DQ5_RG_RX_ARDQ_FIFO_DQSI_DLY_B1);

#if ENABLE_REMOVE_MCK8X_UNCERT_LOWPOWER_OPTION
    vIO32WriteFldAlign(DDRPHY_MISC_VREF_CTRL, 0x7F, MISC_VREF_CTRL_MISC_LP_8X_MUX); // @Mazar: MCK8X uncertainty remove
    vIO32WriteFldAlign(DDRPHY_MISC_VREF_CTRL, 0x7F, MISC_VREF_CTRL_MISC_LP_DDR400_MUX); // @Mazar: open loop mode setting(found connection problem in CHB in DSIM)
#endif

#if ENABLE_REMOVE_MCK8X_UNCERT_DFS_OPTION
    vIO32WriteFldAlign(DDRPHY_DVFS_EMI_CLK, 1, DVFS_EMI_CLK_R_DDRPHY_SHUFFLE_MUX_ENABLE); // @Lynx
    vIO32WriteFldAlign(DRAMC_REG_DVFSDLL2, 1, DVFSDLL2_R_SHUFFLE_PI_RESET_ENABLE); // @Lynx
    vIO32WriteFldAlign(DRAMC_REG_DVFSDLL2, 3, DVFSDLL2_R_DVFS_MCK8X_MARGIN); // @Lynx

    vIO32WriteFldAlign(DDRPHY_B0_DLL_ARPI0, 1, B0_DLL_ARPI0_RG_ARPI_MCK8X_SEL_B0); // @YY
    vIO32WriteFldAlign(DDRPHY_B1_DLL_ARPI0, 1, B1_DLL_ARPI0_RG_ARPI_MCK8X_SEL_B1); // @YY
    vIO32WriteFldAlign(DDRPHY_CA_DLL_ARPI0, 1, CA_DLL_ARPI0_RG_ARPI_MCK8X_SEL_CA); // @YY

    vIO32WriteFldAlign(DRAMC_REG_DVFSDLL2, 3, DVFSDLL2_R_DVFS_PICG_MARGIN4_NEW); // @Lynx
#endif

    vIO32WriteFldAlign(DDRPHY_SHU_MISC1, 0x00000020, SHU_MISC1_DR_EMI_RESERVE); // @TK, For EMI slice timing

    //@Ton, DQS retry off
    vIO32WriteFldMulti(DRAMC_REG_SHU_DQSG_RETRY, P_Fld(0x0, SHU_DQSG_RETRY_R_RETRY_USE_BURST_MDOE)
                | P_Fld(0x0, SHU_DQSG_RETRY_R_RDY_SEL_DLE)
                | P_Fld(0x0, SHU_DQSG_RETRY_R_DQSIENLAT)
                | P_Fld(0x0, SHU_DQSG_RETRY_R_RETRY_ROUND_NUM)
                | P_Fld(0x0, SHU_DQSG_RETRY_R_RETRY_ONCE));

    vIO32WriteFldAlign(DRAMC_REG_PRE_TDQSCK1, 0x1, PRE_TDQSCK1_TX_TRACKING_OPT); //@Jouling

    //@Berson, NOTE: Please set to 1 when DVT is verified. SPMFW must updated by DVT owner.
    vIO32WriteFldAlign(DRAMC_REG_SPCMDCTRL, 0x1, SPCMDCTRL_SPDR_MR4_OPT); //S0 from suspend to resume trigger HW MR4

    //@YY, APHY init settings review
    vIO32WriteFldAlign(DDRPHY_SHU_B0_DQ6, 0x0, SHU_B0_DQ6_RG_ARPI_OFFSET_MCTL_B0);
    vIO32WriteFldAlign(DDRPHY_SHU_B1_DQ6, 0x0, SHU_B1_DQ6_RG_ARPI_OFFSET_MCTL_B1);
    vIO32WriteFldAlign(DDRPHY_CA_CMD5, 0x29, CA_CMD5_RG_RX_ARCMD_EYE_VREF_SEL);
    vIO32WriteFldAlign(DDRPHY_SHU_CA_CMD5, 0x4, SHU_CA_CMD5_RG_RX_ARCMD_FIFO_DQSI_DLY);
    // E2 - new -end =========================================================================

    #if CMD_CKE_WORKAROUND_FIX
    CMD_CKE_Modified_txp_Setting(p);
    #endif

    #if RX_PIPE_BYPASS_ENABLE
    vIO32WriteFldAlign(DDRPHY_SHU_MISC0, 0x1, SHU_MISC0_R_RX_PIPE_BYPASS_EN);
    #endif

    #if CBT_MOVE_CA_INSTEAD_OF_CLK
    if (u1IsPhaseMode(p) == FALSE)
    {
        u1CaUI = 0;
        u1CaPI = 32;
    }
    else
    {
        u1CaUI = 0;
        u1CaPI = 24;
    }
    // CA delay shift u1CaUI*UI
    DramcCmdUIDelaySetting(p, u1CaUI);

    // Rank0/1 u1CaPI*PI CA delay

    u1RankIdxBak = u1GetRank(p);

    for (u1RankIdx = 0; u1RankIdx < (U32)(p->support_rank_num); u1RankIdx++)
    {
        vSetRank(p, u1RankIdx);

        CBTDelayCACLK(p, u1CaPI);
    }

    vSetRank(p, u1RankIdxBak);
    #endif

    //Reset XRTRTR related setting
    #if XRTRTR_NEW_CROSS_RK_MODE
    vIO32WriteFldAlign(DDRPHY_SHU_B0_DQ6, 0, SHU_B0_DQ6_RG_RX_ARDQ_RANK_SEL_SER_MODE_B0);
    vIO32WriteFldAlign(DDRPHY_SHU_B1_DQ6, 0, SHU_B1_DQ6_RG_RX_ARDQ_RANK_SEL_SER_MODE_B1);
    vIO32WriteFldAlign(DRAMC_REG_SHU_RANK_SEL_STB, 0, SHU_RANK_SEL_STB_RANK_SEL_STB_EN);
    #endif


    //Update MP setting
    vIO32WriteFldAlign(DRAMC_REG_DRSCTRL, 0x1, DRSCTRL_DRSCLR_RK0_EN); //Jouling
    vIO32WriteFldMulti(DRAMC_REG_STBCAL2, P_Fld(0x7, STBCAL2_STBCAL_UI_UPD_MASK_OPT)
                                         |P_Fld(0x1, STBCAL2_STBCAL_UI_UPD_MASK_EN)); //HJ, Gating tracking
    #if XRTRTR_NEW_CROSS_RK_MODE
    vIO32WriteFldMulti(DRAMC_REG_SHU_PHY_RX_CTRL, P_Fld(0x2, SHU_PHY_RX_CTRL_RX_IN_GATE_EN_PRE_OFFSET)
                                         |P_Fld(0x2, SHU_PHY_RX_CTRL_RANK_RXDLY_UPD_OFFSET)
                                         |P_Fld(0x1, SHU_PHY_RX_CTRL_RANK_RXDLY_UPDLAT_EN)); //HJ, R2R
    #endif

#endif// #if __A60868_TO_BE_PORTING__
    return DRAM_OK;
}
#endif
#if __A60868_TO_BE_PORTING__
#if LEGACY_DELAY_CELL
/* Legacy CA, TX DQ, TX DQM delay cell initial settings
 * RK#_TX_ARCA#_DLY (RK0~2, CA0~5), RK#_TX_ARDQ#_DLY_B# (RK0~2, DQ0~8, B0~1), RK#_TX_ARDQM0_DLY_B# (RK0~2, B0~1)
 */
static void LegacyDlyCellInitLP4_DDR2667(DRAMC_CTX_T *p)
{
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B0_DQ0, P_Fld(0xa, SHU_R1_B0_DQ0_RK1_TX_ARDQ7_DLY_B0)
                | P_Fld(0xa, SHU_R1_B0_DQ0_RK1_TX_ARDQ6_DLY_B0)
                | P_Fld(0xa, SHU_R1_B0_DQ0_RK1_TX_ARDQ5_DLY_B0)
                | P_Fld(0xa, SHU_R1_B0_DQ0_RK1_TX_ARDQ4_DLY_B0)
                | P_Fld(0xa, SHU_R1_B0_DQ0_RK1_TX_ARDQ3_DLY_B0)
                | P_Fld(0xa, SHU_R1_B0_DQ0_RK1_TX_ARDQ2_DLY_B0)
                | P_Fld(0xa, SHU_R1_B0_DQ0_RK1_TX_ARDQ1_DLY_B0)
                | P_Fld(0xa, SHU_R1_B0_DQ0_RK1_TX_ARDQ0_DLY_B0));
    vIO32WriteFldAlign(DDRPHY_SHU_R1_B0_DQ1, 0xa, SHU_R1_B0_DQ1_RK1_TX_ARDQM0_DLY_B0);
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B1_DQ0, P_Fld(0xa, SHU_R1_B1_DQ0_RK1_TX_ARDQ7_DLY_B1)
                | P_Fld(0xa, SHU_R1_B1_DQ0_RK1_TX_ARDQ6_DLY_B1)
                | P_Fld(0xa, SHU_R1_B1_DQ0_RK1_TX_ARDQ5_DLY_B1)
                | P_Fld(0xa, SHU_R1_B1_DQ0_RK1_TX_ARDQ4_DLY_B1)
                | P_Fld(0xa, SHU_R1_B1_DQ0_RK1_TX_ARDQ3_DLY_B1)
                | P_Fld(0xa, SHU_R1_B1_DQ0_RK1_TX_ARDQ2_DLY_B1)
                | P_Fld(0xa, SHU_R1_B1_DQ0_RK1_TX_ARDQ1_DLY_B1)
                | P_Fld(0xa, SHU_R1_B1_DQ0_RK1_TX_ARDQ0_DLY_B1));
    vIO32WriteFldAlign(DDRPHY_SHU_R1_B1_DQ1, 0xa, SHU_R1_B1_DQ1_RK1_TX_ARDQM0_DLY_B1);
}

static void LegacyDlyCellInitLP4_DDR1600(DRAMC_CTX_T *p)
{
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B0_DQ0, P_Fld(0xd, SHU_R1_B0_DQ0_RK1_TX_ARDQ7_DLY_B0)
                | P_Fld(0xd, SHU_R1_B0_DQ0_RK1_TX_ARDQ6_DLY_B0)
                | P_Fld(0xd, SHU_R1_B0_DQ0_RK1_TX_ARDQ5_DLY_B0)
                | P_Fld(0xd, SHU_R1_B0_DQ0_RK1_TX_ARDQ4_DLY_B0)
                | P_Fld(0xd, SHU_R1_B0_DQ0_RK1_TX_ARDQ3_DLY_B0)
                | P_Fld(0xd, SHU_R1_B0_DQ0_RK1_TX_ARDQ2_DLY_B0)
                | P_Fld(0xd, SHU_R1_B0_DQ0_RK1_TX_ARDQ1_DLY_B0)
                | P_Fld(0xd, SHU_R1_B0_DQ0_RK1_TX_ARDQ0_DLY_B0));
    vIO32WriteFldAlign(DDRPHY_SHU_R1_B0_DQ1, 0xd, SHU_R1_B0_DQ1_RK1_TX_ARDQM0_DLY_B0);
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B1_DQ0, P_Fld(0xd, SHU_R1_B1_DQ0_RK1_TX_ARDQ7_DLY_B1)
                | P_Fld(0xd, SHU_R1_B1_DQ0_RK1_TX_ARDQ6_DLY_B1)
                | P_Fld(0xd, SHU_R1_B1_DQ0_RK1_TX_ARDQ5_DLY_B1)
                | P_Fld(0xd, SHU_R1_B1_DQ0_RK1_TX_ARDQ4_DLY_B1)
                | P_Fld(0xd, SHU_R1_B1_DQ0_RK1_TX_ARDQ3_DLY_B1)
                | P_Fld(0xd, SHU_R1_B1_DQ0_RK1_TX_ARDQ2_DLY_B1)
                | P_Fld(0xd, SHU_R1_B1_DQ0_RK1_TX_ARDQ1_DLY_B1)
                | P_Fld(0xd, SHU_R1_B1_DQ0_RK1_TX_ARDQ0_DLY_B1));
    vIO32WriteFldAlign(DDRPHY_SHU_R1_B1_DQ1, 0xd, SHU_R1_B1_DQ1_RK1_TX_ARDQM0_DLY_B1);
}

static void LegacyDlyCellInitLP4_DDR3200(DRAMC_CTX_T *p)
{
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B0_DQ0, P_Fld(0xa, SHU_R1_B0_DQ0_RK1_TX_ARDQ7_DLY_B0)
                | P_Fld(0xa, SHU_R1_B0_DQ0_RK1_TX_ARDQ6_DLY_B0)
                | P_Fld(0xa, SHU_R1_B0_DQ0_RK1_TX_ARDQ5_DLY_B0)
                | P_Fld(0xa, SHU_R1_B0_DQ0_RK1_TX_ARDQ4_DLY_B0)
                | P_Fld(0xa, SHU_R1_B0_DQ0_RK1_TX_ARDQ3_DLY_B0)
                | P_Fld(0xa, SHU_R1_B0_DQ0_RK1_TX_ARDQ2_DLY_B0)
                | P_Fld(0xa, SHU_R1_B0_DQ0_RK1_TX_ARDQ1_DLY_B0)
                | P_Fld(0xa, SHU_R1_B0_DQ0_RK1_TX_ARDQ0_DLY_B0));
    vIO32WriteFldAlign(DDRPHY_SHU_R1_B0_DQ1, 0xa, SHU_R1_B0_DQ1_RK1_TX_ARDQM0_DLY_B0);
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B1_DQ0, P_Fld(0xa, SHU_R1_B1_DQ0_RK1_TX_ARDQ7_DLY_B1)
                | P_Fld(0xa, SHU_R1_B1_DQ0_RK1_TX_ARDQ6_DLY_B1)
                | P_Fld(0xa, SHU_R1_B1_DQ0_RK1_TX_ARDQ5_DLY_B1)
                | P_Fld(0xa, SHU_R1_B1_DQ0_RK1_TX_ARDQ4_DLY_B1)
                | P_Fld(0xa, SHU_R1_B1_DQ0_RK1_TX_ARDQ3_DLY_B1)
                | P_Fld(0xa, SHU_R1_B1_DQ0_RK1_TX_ARDQ2_DLY_B1)
                | P_Fld(0xa, SHU_R1_B1_DQ0_RK1_TX_ARDQ1_DLY_B1)
                | P_Fld(0xa, SHU_R1_B1_DQ0_RK1_TX_ARDQ0_DLY_B1));
    vIO32WriteFldAlign(DDRPHY_SHU_R1_B1_DQ1, 0xa, SHU_R1_B1_DQ1_RK1_TX_ARDQM0_DLY_B1);
}
#endif

#if LEGACY_TX_TRACK
/* Legacy tx tracking related initial settings (actual correct values are set during calibration steps)
 * BOOT_ORIG_UI_RK#_DQ#, BOOT_TARG_UI_RK#_DQ#, BOOT_TARG_UI_RK#_DQM#, BOOT_TARG_UI_RK#_OEN_DQ#, BOOT_TARG_UI_RK#_OEN_DQM#
 * DQSOSCTHRD_DEC, DQSOSCTHRD_INC, DQSOSC_PRDCNT
 * DQSOSC_BASE_RK#, DQSOSCR_RK#_BYTE_MODE, DQSOSCR_DQSOSCRCNT
 */
static void LegacyTxTrackLP4_DDR2667(DRAMC_CTX_T *p)
{
    vIO32WriteFldMulti(DRAMC_REG_SHU_DQSOSCTHRD, P_Fld(0xc, SHU_DQSOSCTHRD_DQSOSCTHRD_DEC_RK0)
                | P_Fld(0xc, SHU_DQSOSCTHRD_DQSOSCTHRD_INC_RK0)
                | P_Fld(0xc, SHU_DQSOSCTHRD_DQSOSCTHRD_INC_RK1_7TO0));
    vIO32WriteFldMulti(DRAMC_REG_SHU_DQSOSC_PRD, P_Fld(0xc, SHU_DQSOSC_PRD_DQSOSCTHRD_DEC_RK1)
                | P_Fld(0x0, SHU_DQSOSC_PRD_DQSOSCTHRD_INC_RK1_11TO8)
                | P_Fld(0x10, SHU_DQSOSC_PRD_DQSOSC_PRDCNT));
    vIO32WriteFldMulti(DRAMC_REG_SHURK0_DQSOSC, P_Fld(0x1ae, SHURK0_DQSOSC_DQSOSC_BASE_RK0_B1)
                | P_Fld(0x1ae, SHURK0_DQSOSC_DQSOSC_BASE_RK0));
    if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
    {
        vIO32WriteFldMulti(DRAMC_REG_SHURK0_DQS2DQ_CAL1, P_Fld(0x354, SHURK0_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ1)
                    | P_Fld(0x354, SHURK0_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ0));
        vIO32WriteFldMulti(DRAMC_REG_SHURK0_DQS2DQ_CAL2, P_Fld(0x354, SHURK0_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ1)
                    | P_Fld(0x354, SHURK0_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ0));
    }
    else
    {
        vIO32WriteFldMulti(DRAMC_REG_SHURK0_DQS2DQ_CAL1, P_Fld(0x254, SHURK0_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ1)
                    | P_Fld(0x254, SHURK0_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ0));
        vIO32WriteFldMulti(DRAMC_REG_SHURK0_DQS2DQ_CAL2, P_Fld(0x254, SHURK0_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ1)
                    | P_Fld(0x254, SHURK0_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ0));
    }
    vIO32WriteFldMulti(DRAMC_REG_SHURK0_DQS2DQ_CAL3, P_Fld(0x14, SHURK0_DQS2DQ_CAL3_BOOT_TARG_UI_RK0_OEN_DQ1_B4TO0)
                | P_Fld(0x14, SHURK0_DQS2DQ_CAL3_BOOT_TARG_UI_RK0_OEN_DQ0_B4TO0)
                | P_Fld(0x18, SHURK0_DQS2DQ_CAL3_BOOT_TARG_UI_RK0_OEN_DQ1)
                | P_Fld(0x18, SHURK0_DQS2DQ_CAL3_BOOT_TARG_UI_RK0_OEN_DQ0));
    vIO32WriteFldMulti(DRAMC_REG_SHURK0_DQS2DQ_CAL4, P_Fld(0x14, SHURK0_DQS2DQ_CAL4_BOOT_TARG_UI_RK0_OEN_DQM1_B4TO0)
                | P_Fld(0x14, SHURK0_DQS2DQ_CAL4_BOOT_TARG_UI_RK0_OEN_DQM0_B4TO0)
                | P_Fld(0x18, SHURK0_DQS2DQ_CAL4_BOOT_TARG_UI_RK0_OEN_DQM1)
                | P_Fld(0x18, SHURK0_DQS2DQ_CAL4_BOOT_TARG_UI_RK0_OEN_DQM0));
    if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
    {
        vIO32WriteFldMulti(DRAMC_REG_SHURK0_DQS2DQ_CAL5, P_Fld(0x354, SHURK0_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM1)
                    | P_Fld(0x354, SHURK0_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM0));
    }
    else
    {
        vIO32WriteFldMulti(DRAMC_REG_SHURK0_DQS2DQ_CAL5, P_Fld(0x254, SHURK0_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM1)
                    | P_Fld(0x254, SHURK0_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM0));
    }
    vIO32WriteFldMulti(DRAMC_REG_SHURK1_DQSOSC, P_Fld(0x160, SHURK1_DQSOSC_DQSOSC_BASE_RK1_B1)
                | P_Fld(0x160, SHURK1_DQSOSC_DQSOSC_BASE_RK1));
    if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
    {
        vIO32WriteFldMulti(DRAMC_REG_SHURK1_DQS2DQ_CAL1, P_Fld(0x354, SHURK1_DQS2DQ_CAL1_BOOT_ORIG_UI_RK1_DQ1)
                    | P_Fld(0x354, SHURK1_DQS2DQ_CAL1_BOOT_ORIG_UI_RK1_DQ0));
        vIO32WriteFldMulti(DRAMC_REG_SHURK1_DQS2DQ_CAL2, P_Fld(0x354, SHURK1_DQS2DQ_CAL2_BOOT_TARG_UI_RK1_DQ1)
                    | P_Fld(0x354, SHURK1_DQS2DQ_CAL2_BOOT_TARG_UI_RK1_DQ0));
    }
    else
    {
        vIO32WriteFldMulti(DRAMC_REG_SHURK1_DQS2DQ_CAL1, P_Fld(0x254, SHURK1_DQS2DQ_CAL1_BOOT_ORIG_UI_RK1_DQ1)
                    | P_Fld(0x254, SHURK1_DQS2DQ_CAL1_BOOT_ORIG_UI_RK1_DQ0));
        vIO32WriteFldMulti(DRAMC_REG_SHURK1_DQS2DQ_CAL2, P_Fld(0x254, SHURK1_DQS2DQ_CAL2_BOOT_TARG_UI_RK1_DQ1)
                    | P_Fld(0x254, SHURK1_DQS2DQ_CAL2_BOOT_TARG_UI_RK1_DQ0));
    }
    vIO32WriteFldMulti(DRAMC_REG_SHURK1_DQS2DQ_CAL3, P_Fld(0x14, SHURK1_DQS2DQ_CAL3_BOOT_TARG_UI_RK1_OEN_DQ1_B4TO0)
                | P_Fld(0x14, SHURK1_DQS2DQ_CAL3_BOOT_TARG_UI_RK1_OEN_DQ0_B4TO0)
                | P_Fld(0x18, SHURK1_DQS2DQ_CAL3_BOOT_TARG_UI_RK1_OEN_DQ1)
                | P_Fld(0x18, SHURK1_DQS2DQ_CAL3_BOOT_TARG_UI_RK1_OEN_DQ0));
    vIO32WriteFldMulti(DRAMC_REG_SHURK1_DQS2DQ_CAL4, P_Fld(0x14, SHURK1_DQS2DQ_CAL4_BOOT_TARG_UI_RK1_OEN_DQM1_B4TO0)
                | P_Fld(0x14, SHURK1_DQS2DQ_CAL4_BOOT_TARG_UI_RK1_OEN_DQM0_B4TO0)
                | P_Fld(0x18, SHURK1_DQS2DQ_CAL4_BOOT_TARG_UI_RK1_OEN_DQM1)
                | P_Fld(0x18, SHURK1_DQS2DQ_CAL4_BOOT_TARG_UI_RK1_OEN_DQM0));
    if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
    {
        vIO32WriteFldMulti(DRAMC_REG_SHURK1_DQS2DQ_CAL5, P_Fld(0x354, SHURK1_DQS2DQ_CAL5_BOOT_TARG_UI_RK1_DQM1)
                    | P_Fld(0x354, SHURK1_DQS2DQ_CAL5_BOOT_TARG_UI_RK1_DQM0));
    }
    else
    {
        vIO32WriteFldMulti(DRAMC_REG_SHURK1_DQS2DQ_CAL5, P_Fld(0x254, SHURK1_DQS2DQ_CAL5_BOOT_TARG_UI_RK1_DQM1)
                    | P_Fld(0x254, SHURK1_DQS2DQ_CAL5_BOOT_TARG_UI_RK1_DQM0));
    }
}

static void LegacyTxTrackLP4_DDR1600(DRAMC_CTX_T *p)
{
    vIO32WriteFldMulti(DRAMC_REG_SHU_DQSOSCTHRD, P_Fld(0x14, SHU_DQSOSCTHRD_DQSOSCTHRD_DEC_RK0)
                | P_Fld(0x14, SHU_DQSOSCTHRD_DQSOSCTHRD_INC_RK0)
                | P_Fld(0x14, SHU_DQSOSCTHRD_DQSOSCTHRD_INC_RK1_7TO0));
    vIO32WriteFldMulti(DRAMC_REG_SHU_DQSOSC_PRD, P_Fld(0x14, SHU_DQSOSC_PRD_DQSOSCTHRD_DEC_RK1)
                | P_Fld(0x0, SHU_DQSOSC_PRD_DQSOSCTHRD_INC_RK1_11TO8)
                | P_Fld(0xf, SHU_DQSOSC_PRD_DQSOSC_PRDCNT));
    vIO32WriteFldMulti(DRAMC_REG_SHURK0_DQSOSC, P_Fld(0x2d0, SHURK0_DQSOSC_DQSOSC_BASE_RK0_B1)
                | P_Fld(0x2d0, SHURK0_DQSOSC_DQSOSC_BASE_RK0));
    vIO32WriteFldMulti(DRAMC_REG_SHURK0_DQS2DQ_CAL1, P_Fld(0x23a, SHURK0_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ1)
                | P_Fld(0x23a, SHURK0_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ0));
    vIO32WriteFldMulti(DRAMC_REG_SHURK0_DQS2DQ_CAL2, P_Fld(0x23a, SHURK0_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ1)
                | P_Fld(0x23a, SHURK0_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ0));
    vIO32WriteFldMulti(DRAMC_REG_SHURK0_DQS2DQ_CAL3, P_Fld(0x1a, SHURK0_DQS2DQ_CAL3_BOOT_TARG_UI_RK0_OEN_DQ1_B4TO0)
                | P_Fld(0x1a, SHURK0_DQS2DQ_CAL3_BOOT_TARG_UI_RK0_OEN_DQ0_B4TO0)
                | P_Fld(0xf, SHURK0_DQS2DQ_CAL3_BOOT_TARG_UI_RK0_OEN_DQ1)
                | P_Fld(0xf, SHURK0_DQS2DQ_CAL3_BOOT_TARG_UI_RK0_OEN_DQ0));
    vIO32WriteFldMulti(DRAMC_REG_SHURK0_DQS2DQ_CAL4, P_Fld(0x1a, SHURK0_DQS2DQ_CAL4_BOOT_TARG_UI_RK0_OEN_DQM1_B4TO0)
                | P_Fld(0x1a, SHURK0_DQS2DQ_CAL4_BOOT_TARG_UI_RK0_OEN_DQM0_B4TO0)
                | P_Fld(0xf, SHURK0_DQS2DQ_CAL4_BOOT_TARG_UI_RK0_OEN_DQM1)
                | P_Fld(0xf, SHURK0_DQS2DQ_CAL4_BOOT_TARG_UI_RK0_OEN_DQM0));
    vIO32WriteFldMulti(DRAMC_REG_SHURK0_DQS2DQ_CAL5, P_Fld(0x23e, SHURK0_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM1)
                | P_Fld(0x23e, SHURK0_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM0));
    vIO32WriteFldMulti(DRAMC_REG_SHURK1_DQSOSC, P_Fld(0x24e, SHURK1_DQSOSC_DQSOSC_BASE_RK1_B1)
                | P_Fld(0x24e, SHURK1_DQSOSC_DQSOSC_BASE_RK1));
    vIO32WriteFldMulti(DRAMC_REG_SHURK1_DQS2DQ_CAL1, P_Fld(0x23e, SHURK1_DQS2DQ_CAL1_BOOT_ORIG_UI_RK1_DQ1)
                | P_Fld(0x23e, SHURK1_DQS2DQ_CAL1_BOOT_ORIG_UI_RK1_DQ0));
    vIO32WriteFldMulti(DRAMC_REG_SHURK1_DQS2DQ_CAL2, P_Fld(0x23e, SHURK1_DQS2DQ_CAL2_BOOT_TARG_UI_RK1_DQ1)
                | P_Fld(0x23e, SHURK1_DQS2DQ_CAL2_BOOT_TARG_UI_RK1_DQ0));
    vIO32WriteFldMulti(DRAMC_REG_SHURK1_DQS2DQ_CAL3, P_Fld(0x1e, SHURK1_DQS2DQ_CAL3_BOOT_TARG_UI_RK1_OEN_DQ1_B4TO0)
                | P_Fld(0x1e, SHURK1_DQS2DQ_CAL3_BOOT_TARG_UI_RK1_OEN_DQ0_B4TO0)
                | P_Fld(0xf, SHURK1_DQS2DQ_CAL3_BOOT_TARG_UI_RK1_OEN_DQ1)
                | P_Fld(0xf, SHURK1_DQS2DQ_CAL3_BOOT_TARG_UI_RK1_OEN_DQ0));
    vIO32WriteFldMulti(DRAMC_REG_SHURK1_DQS2DQ_CAL4, P_Fld(0x1e, SHURK1_DQS2DQ_CAL4_BOOT_TARG_UI_RK1_OEN_DQM1_B4TO0)
                | P_Fld(0x1e, SHURK1_DQS2DQ_CAL4_BOOT_TARG_UI_RK1_OEN_DQM0_B4TO0)
                | P_Fld(0xf, SHURK1_DQS2DQ_CAL4_BOOT_TARG_UI_RK1_OEN_DQM1)
                | P_Fld(0xf, SHURK1_DQS2DQ_CAL4_BOOT_TARG_UI_RK1_OEN_DQM0));
    vIO32WriteFldMulti(DRAMC_REG_SHURK1_DQS2DQ_CAL5, P_Fld(0x23e, SHURK1_DQS2DQ_CAL5_BOOT_TARG_UI_RK1_DQM1)
                | P_Fld(0x23e, SHURK1_DQS2DQ_CAL5_BOOT_TARG_UI_RK1_DQM0));
}

static void LegacyTxTrackLP4_DDR3200(DRAMC_CTX_T *p)
{
    vIO32WriteFldMulti(DRAMC_REG_SHU_DQSOSCTHRD, P_Fld(0xa, SHU_DQSOSCTHRD_DQSOSCTHRD_DEC_RK0)
                | P_Fld(0xa, SHU_DQSOSCTHRD_DQSOSCTHRD_INC_RK0)
                | P_Fld(0xa, SHU_DQSOSCTHRD_DQSOSCTHRD_INC_RK1_7TO0));
    vIO32WriteFldMulti(DRAMC_REG_SHU_DQSOSC_PRD, P_Fld(0xa, SHU_DQSOSC_PRD_DQSOSCTHRD_DEC_RK1)
                | P_Fld(0x0, SHU_DQSOSC_PRD_DQSOSCTHRD_INC_RK1_11TO8)
                | P_Fld(0x10, SHU_DQSOSC_PRD_DQSOSC_PRDCNT));
    vIO32WriteFldAlign(DRAMC_REG_SHU_DQSOSCR, 0x10, SHU_DQSOSCR_DQSOSCRCNT);
    vIO32WriteFldMulti(DRAMC_REG_SHURK0_DQSOSC, P_Fld(0x168, SHURK0_DQSOSC_DQSOSC_BASE_RK0_B1)
                | P_Fld(0x168, SHURK0_DQSOSC_DQSOSC_BASE_RK0));
    if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
    {
            vIO32WriteFldMulti(DRAMC_REG_SHURK0_DQS2DQ_CAL1, P_Fld(0x3da, SHURK0_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ1)
                        | P_Fld(0x3da, SHURK0_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ0));
            vIO32WriteFldMulti(DRAMC_REG_SHURK0_DQS2DQ_CAL2, P_Fld(0x3da, SHURK0_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ1)
                        | P_Fld(0x3da, SHURK0_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ0));
    }
    else
    {
            vIO32WriteFldMulti(DRAMC_REG_SHURK0_DQS2DQ_CAL1, P_Fld(0x2da, SHURK0_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ1)
                        | P_Fld(0x2da, SHURK0_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ0));
            vIO32WriteFldMulti(DRAMC_REG_SHURK0_DQS2DQ_CAL2, P_Fld(0x2da, SHURK0_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ1)
                        | P_Fld(0x2da, SHURK0_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ0));
    }
    vIO32WriteFldMulti(DRAMC_REG_SHURK0_DQS2DQ_CAL3, P_Fld(0x1a, SHURK0_DQS2DQ_CAL3_BOOT_TARG_UI_RK0_OEN_DQ1_B4TO0)
                | P_Fld(0x1a, SHURK0_DQS2DQ_CAL3_BOOT_TARG_UI_RK0_OEN_DQ0_B4TO0)
                | P_Fld(0x1c, SHURK0_DQS2DQ_CAL3_BOOT_TARG_UI_RK0_OEN_DQ1)
                | P_Fld(0x1c, SHURK0_DQS2DQ_CAL3_BOOT_TARG_UI_RK0_OEN_DQ0));
    vIO32WriteFldMulti(DRAMC_REG_SHURK0_DQS2DQ_CAL4, P_Fld(0x1a, SHURK0_DQS2DQ_CAL4_BOOT_TARG_UI_RK0_OEN_DQM1_B4TO0)
                | P_Fld(0x1a, SHURK0_DQS2DQ_CAL4_BOOT_TARG_UI_RK0_OEN_DQM0_B4TO0)
                | P_Fld(0x1c, SHURK0_DQS2DQ_CAL4_BOOT_TARG_UI_RK0_OEN_DQM1)
                | P_Fld(0x1c, SHURK0_DQS2DQ_CAL4_BOOT_TARG_UI_RK0_OEN_DQM0));
    if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
    {
            vIO32WriteFldMulti(DRAMC_REG_SHURK0_DQS2DQ_CAL5, P_Fld(0x3da, SHURK0_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM1)
                        | P_Fld(0x3da, SHURK0_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM0));
    }
    else
    {
            vIO32WriteFldMulti(DRAMC_REG_SHURK0_DQS2DQ_CAL5, P_Fld(0x2da, SHURK0_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM1)
                        | P_Fld(0x2da, SHURK0_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM0));
    }
    vIO32WriteFldMulti(DRAMC_REG_SHURK1_DQSOSC, P_Fld(0x127, SHURK1_DQSOSC_DQSOSC_BASE_RK1_B1)
                | P_Fld(0x127, SHURK1_DQSOSC_DQSOSC_BASE_RK1));
    if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
    {
            vIO32WriteFldMulti(DRAMC_REG_SHURK1_DQS2DQ_CAL1, P_Fld(0x3d4, SHURK1_DQS2DQ_CAL1_BOOT_ORIG_UI_RK1_DQ1)
                        | P_Fld(0x3d4, SHURK1_DQS2DQ_CAL1_BOOT_ORIG_UI_RK1_DQ0));
            vIO32WriteFldMulti(DRAMC_REG_SHURK1_DQS2DQ_CAL2, P_Fld(0x3d4, SHURK1_DQS2DQ_CAL2_BOOT_TARG_UI_RK1_DQ1)
                        | P_Fld(0x3d4, SHURK1_DQS2DQ_CAL2_BOOT_TARG_UI_RK1_DQ0));
    }
    else
    {
            vIO32WriteFldMulti(DRAMC_REG_SHURK1_DQS2DQ_CAL1, P_Fld(0x2d4, SHURK1_DQS2DQ_CAL1_BOOT_ORIG_UI_RK1_DQ1)
                        | P_Fld(0x2d4, SHURK1_DQS2DQ_CAL1_BOOT_ORIG_UI_RK1_DQ0));
            vIO32WriteFldMulti(DRAMC_REG_SHURK1_DQS2DQ_CAL2, P_Fld(0x2d4, SHURK1_DQS2DQ_CAL2_BOOT_TARG_UI_RK1_DQ1)
                        | P_Fld(0x2d4, SHURK1_DQS2DQ_CAL2_BOOT_TARG_UI_RK1_DQ0));
    }
    vIO32WriteFldMulti(DRAMC_REG_SHURK1_DQS2DQ_CAL3, P_Fld(0x14, SHURK1_DQS2DQ_CAL3_BOOT_TARG_UI_RK1_OEN_DQ1_B4TO0)
                | P_Fld(0x14, SHURK1_DQS2DQ_CAL3_BOOT_TARG_UI_RK1_OEN_DQ0_B4TO0)
                | P_Fld(0x1c, SHURK1_DQS2DQ_CAL3_BOOT_TARG_UI_RK1_OEN_DQ1)
                | P_Fld(0x1c, SHURK1_DQS2DQ_CAL3_BOOT_TARG_UI_RK1_OEN_DQ0));
    vIO32WriteFldMulti(DRAMC_REG_SHURK1_DQS2DQ_CAL4, P_Fld(0x14, SHURK1_DQS2DQ_CAL4_BOOT_TARG_UI_RK1_OEN_DQM1_B4TO0)
                | P_Fld(0x14, SHURK1_DQS2DQ_CAL4_BOOT_TARG_UI_RK1_OEN_DQM0_B4TO0)
                | P_Fld(0x1c, SHURK1_DQS2DQ_CAL4_BOOT_TARG_UI_RK1_OEN_DQM1)
                | P_Fld(0x1c, SHURK1_DQS2DQ_CAL4_BOOT_TARG_UI_RK1_OEN_DQM0));
    if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
    {
            vIO32WriteFldMulti(DRAMC_REG_SHURK1_DQS2DQ_CAL5, P_Fld(0x3d4, SHURK1_DQS2DQ_CAL5_BOOT_TARG_UI_RK1_DQM1)
                        | P_Fld(0x3d4, SHURK1_DQS2DQ_CAL5_BOOT_TARG_UI_RK1_DQM0));
    }
    else
    {
            vIO32WriteFldMulti(DRAMC_REG_SHURK1_DQS2DQ_CAL5, P_Fld(0x2d4, SHURK1_DQS2DQ_CAL5_BOOT_TARG_UI_RK1_DQM1)
                        | P_Fld(0x2d4, SHURK1_DQS2DQ_CAL5_BOOT_TARG_UI_RK1_DQM0));
    }
    if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
    {
        vIO32WriteFldAlign(DRAMC_REG_DQSOSCR, 0x1, DQSOSCR_RK0_BYTE_MODE);
        vIO32WriteFldAlign(DRAMC_REG_DQSOSCR, 0x1, DQSOSCR_RK1_BYTE_MODE);
    }
}
#endif

#if LEGACY_TDQSCK_PRECAL
/* Legacy tDQSCK precal related initial settings (actual correct values are set during calibration)
 * Ex: TDQSCK_JUMP_RATIO, TDQSCK_UIFREQ#, TDQSCK_PIFREQ#
 */
static void LegacyPreCalLP4_DDR2667(DRAMC_CTX_T *p)
{
    vIO32WriteFldMulti(DRAMC_REG_PRE_TDQSCK2, P_Fld(0x1a, PRE_TDQSCK2_TDDQSCK_JUMP_RATIO0)
                | P_Fld(0x10, PRE_TDQSCK2_TDDQSCK_JUMP_RATIO1)
                | P_Fld(0x0, PRE_TDQSCK2_TDDQSCK_JUMP_RATIO2)
                | P_Fld(0x26, PRE_TDQSCK2_TDDQSCK_JUMP_RATIO3));
    vIO32WriteFldMulti(DRAMC_REG_PRE_TDQSCK3, P_Fld(0x13, PRE_TDQSCK3_TDDQSCK_JUMP_RATIO4)
                | P_Fld(0x0, PRE_TDQSCK3_TDDQSCK_JUMP_RATIO5)
                | P_Fld(0x40, PRE_TDQSCK3_TDDQSCK_JUMP_RATIO6)
                | P_Fld(0x35, PRE_TDQSCK3_TDDQSCK_JUMP_RATIO7));
    vIO32WriteFldMulti(DRAMC_REG_PRE_TDQSCK4, P_Fld(0x0, PRE_TDQSCK4_TDDQSCK_JUMP_RATIO8)
                | P_Fld(0x0, PRE_TDQSCK4_TDDQSCK_JUMP_RATIO9)
                | P_Fld(0x0, PRE_TDQSCK4_TDDQSCK_JUMP_RATIO10)
                | P_Fld(0x0, PRE_TDQSCK4_TDDQSCK_JUMP_RATIO11));
    vIO32WriteFldAlign(DRAMC_REG_RK0_PRE_TDQSCK1, 0xa, RK0_PRE_TDQSCK1_TDQSCK_UIFREQ2_B0R0);
    vIO32WriteFldMulti(DRAMC_REG_RK0_PRE_TDQSCK2, P_Fld(0x0, RK0_PRE_TDQSCK2_TDQSCK_PIFREQ4_B0R0)
                | P_Fld(0x0, RK0_PRE_TDQSCK2_TDQSCK_UIFREQ4_B0R0)
                | P_Fld(0xb, RK0_PRE_TDQSCK2_TDQSCK_PIFREQ3_B0R0)
                | P_Fld(0xd, RK0_PRE_TDQSCK2_TDQSCK_UIFREQ3_B0R0));
    vIO32WriteFldMulti(DRAMC_REG_RK0_PRE_TDQSCK3, P_Fld(0x0, RK0_PRE_TDQSCK3_TDQSCK_UIFREQ4_P1_B0R0)
                | P_Fld(0x11, RK0_PRE_TDQSCK3_TDQSCK_UIFREQ3_P1_B0R0)
                | P_Fld(0xe, RK0_PRE_TDQSCK3_TDQSCK_UIFREQ2_P1_B0R0));
    vIO32WriteFldAlign(DRAMC_REG_RK0_PRE_TDQSCK4, 0xa, RK0_PRE_TDQSCK4_TDQSCK_UIFREQ2_B1R0);
    vIO32WriteFldMulti(DRAMC_REG_RK0_PRE_TDQSCK5, P_Fld(0x0, RK0_PRE_TDQSCK5_TDQSCK_PIFREQ4_B1R0)
                | P_Fld(0x0, RK0_PRE_TDQSCK5_TDQSCK_UIFREQ4_B1R0)
                | P_Fld(0xb, RK0_PRE_TDQSCK5_TDQSCK_PIFREQ3_B1R0)
                | P_Fld(0xd, RK0_PRE_TDQSCK5_TDQSCK_UIFREQ3_B1R0));
    vIO32WriteFldMulti(DRAMC_REG_RK0_PRE_TDQSCK6, P_Fld(0x0, RK0_PRE_TDQSCK6_TDQSCK_UIFREQ4_P1_B1R0)
                | P_Fld(0x11, RK0_PRE_TDQSCK6_TDQSCK_UIFREQ3_P1_B1R0)
                | P_Fld(0xe, RK0_PRE_TDQSCK6_TDQSCK_UIFREQ2_P1_B1R0));
    vIO32WriteFldMulti(DRAMC_REG_RK1_PRE_TDQSCK1, P_Fld(0x8, RK1_PRE_TDQSCK1_TDQSCK_PIFREQ2_B0R1)
                | P_Fld(0x10, RK1_PRE_TDQSCK1_TDQSCK_UIFREQ2_B0R1));
    vIO32WriteFldMulti(DRAMC_REG_RK1_PRE_TDQSCK2, P_Fld(0x0, RK1_PRE_TDQSCK2_TDQSCK_PIFREQ4_B0R1)
                | P_Fld(0x0, RK1_PRE_TDQSCK2_TDQSCK_UIFREQ4_B0R1)
                | P_Fld(0xe, RK1_PRE_TDQSCK2_TDQSCK_PIFREQ3_B0R1)
                | P_Fld(0x10, RK1_PRE_TDQSCK2_TDQSCK_UIFREQ3_B0R1));
    vIO32WriteFldMulti(DRAMC_REG_RK1_PRE_TDQSCK3, P_Fld(0x0, RK1_PRE_TDQSCK3_TDQSCK_UIFREQ4_P1_B0R1)
                | P_Fld(0x14, RK1_PRE_TDQSCK3_TDQSCK_UIFREQ3_P1_B0R1)
                | P_Fld(0x14, RK1_PRE_TDQSCK3_TDQSCK_UIFREQ2_P1_B0R1));
    vIO32WriteFldMulti(DRAMC_REG_RK1_PRE_TDQSCK4, P_Fld(0x8, RK1_PRE_TDQSCK4_TDQSCK_PIFREQ2_B1R1)
                | P_Fld(0x10, RK1_PRE_TDQSCK4_TDQSCK_UIFREQ2_B1R1));
    vIO32WriteFldMulti(DRAMC_REG_RK1_PRE_TDQSCK5, P_Fld(0x0, RK1_PRE_TDQSCK5_TDQSCK_PIFREQ4_B1R1)
                | P_Fld(0x0, RK1_PRE_TDQSCK5_TDQSCK_UIFREQ4_B1R1)
                | P_Fld(0xe, RK1_PRE_TDQSCK5_TDQSCK_PIFREQ3_B1R1)
                | P_Fld(0x10, RK1_PRE_TDQSCK5_TDQSCK_UIFREQ3_B1R1));
    vIO32WriteFldMulti(DRAMC_REG_RK1_PRE_TDQSCK6, P_Fld(0x0, RK1_PRE_TDQSCK6_TDQSCK_UIFREQ4_P1_B1R1)
                | P_Fld(0x14, RK1_PRE_TDQSCK6_TDQSCK_UIFREQ3_P1_B1R1)
                | P_Fld(0x14, RK1_PRE_TDQSCK6_TDQSCK_UIFREQ2_P1_B1R1));
}

static void LegacyPreCalLP4_DDR1600(DRAMC_CTX_T *p)
{
    vIO32WriteFldMulti(DRAMC_REG_PRE_TDQSCK2, P_Fld(0x1a, PRE_TDQSCK2_TDDQSCK_JUMP_RATIO0)
                | P_Fld(0x10, PRE_TDQSCK2_TDDQSCK_JUMP_RATIO1)
                | P_Fld(0x0, PRE_TDQSCK2_TDDQSCK_JUMP_RATIO2)
                | P_Fld(0x26, PRE_TDQSCK2_TDDQSCK_JUMP_RATIO3));
    vIO32WriteFldMulti(DRAMC_REG_PRE_TDQSCK3, P_Fld(0x13, PRE_TDQSCK3_TDDQSCK_JUMP_RATIO4)
                | P_Fld(0x0, PRE_TDQSCK3_TDDQSCK_JUMP_RATIO5)
                | P_Fld(0x40, PRE_TDQSCK3_TDDQSCK_JUMP_RATIO6)
                | P_Fld(0x35, PRE_TDQSCK3_TDDQSCK_JUMP_RATIO7));
    vIO32WriteFldMulti(DRAMC_REG_PRE_TDQSCK4, P_Fld(0x0, PRE_TDQSCK4_TDDQSCK_JUMP_RATIO8)
                | P_Fld(0x0, PRE_TDQSCK4_TDDQSCK_JUMP_RATIO9)
                | P_Fld(0x0, PRE_TDQSCK4_TDDQSCK_JUMP_RATIO10)
                | P_Fld(0x0, PRE_TDQSCK4_TDDQSCK_JUMP_RATIO11));
    vIO32WriteFldAlign(DRAMC_REG_RK0_PRE_TDQSCK1, 0xa, RK0_PRE_TDQSCK1_TDQSCK_UIFREQ2_B0R0);
    vIO32WriteFldMulti(DRAMC_REG_RK0_PRE_TDQSCK2, P_Fld(0x0, RK0_PRE_TDQSCK2_TDQSCK_PIFREQ4_B0R0)
                | P_Fld(0x0, RK0_PRE_TDQSCK2_TDQSCK_UIFREQ4_B0R0)
                | P_Fld(0xb, RK0_PRE_TDQSCK2_TDQSCK_PIFREQ3_B0R0)
                | P_Fld(0xd, RK0_PRE_TDQSCK2_TDQSCK_UIFREQ3_B0R0));
    vIO32WriteFldMulti(DRAMC_REG_RK0_PRE_TDQSCK3, P_Fld(0x0, RK0_PRE_TDQSCK3_TDQSCK_UIFREQ4_P1_B0R0)
                | P_Fld(0x11, RK0_PRE_TDQSCK3_TDQSCK_UIFREQ3_P1_B0R0)
                | P_Fld(0xe, RK0_PRE_TDQSCK3_TDQSCK_UIFREQ2_P1_B0R0));
    vIO32WriteFldAlign(DRAMC_REG_RK0_PRE_TDQSCK4, 0xa, RK0_PRE_TDQSCK4_TDQSCK_UIFREQ2_B1R0);
    vIO32WriteFldMulti(DRAMC_REG_RK0_PRE_TDQSCK5, P_Fld(0x0, RK0_PRE_TDQSCK5_TDQSCK_PIFREQ4_B1R0)
                | P_Fld(0x0, RK0_PRE_TDQSCK5_TDQSCK_UIFREQ4_B1R0)
                | P_Fld(0xb, RK0_PRE_TDQSCK5_TDQSCK_PIFREQ3_B1R0)
                | P_Fld(0xd, RK0_PRE_TDQSCK5_TDQSCK_UIFREQ3_B1R0));
    vIO32WriteFldMulti(DRAMC_REG_RK0_PRE_TDQSCK6, P_Fld(0x0, RK0_PRE_TDQSCK6_TDQSCK_UIFREQ4_P1_B1R0)
                | P_Fld(0x11, RK0_PRE_TDQSCK6_TDQSCK_UIFREQ3_P1_B1R0)
                | P_Fld(0xe, RK0_PRE_TDQSCK6_TDQSCK_UIFREQ2_P1_B1R0));
    vIO32WriteFldMulti(DRAMC_REG_RK1_PRE_TDQSCK1, P_Fld(0x8, RK1_PRE_TDQSCK1_TDQSCK_PIFREQ2_B0R1)
                | P_Fld(0x10, RK1_PRE_TDQSCK1_TDQSCK_UIFREQ2_B0R1));
    vIO32WriteFldMulti(DRAMC_REG_RK1_PRE_TDQSCK2, P_Fld(0x0, RK1_PRE_TDQSCK2_TDQSCK_PIFREQ4_B0R1)
                | P_Fld(0x0, RK1_PRE_TDQSCK2_TDQSCK_UIFREQ4_B0R1)
                | P_Fld(0xe, RK1_PRE_TDQSCK2_TDQSCK_PIFREQ3_B0R1)
                | P_Fld(0x10, RK1_PRE_TDQSCK2_TDQSCK_UIFREQ3_B0R1));
    vIO32WriteFldMulti(DRAMC_REG_RK1_PRE_TDQSCK3, P_Fld(0x0, RK1_PRE_TDQSCK3_TDQSCK_UIFREQ4_P1_B0R1)
                | P_Fld(0x14, RK1_PRE_TDQSCK3_TDQSCK_UIFREQ3_P1_B0R1)
                | P_Fld(0x14, RK1_PRE_TDQSCK3_TDQSCK_UIFREQ2_P1_B0R1));
    vIO32WriteFldMulti(DRAMC_REG_RK1_PRE_TDQSCK4, P_Fld(0x8, RK1_PRE_TDQSCK4_TDQSCK_PIFREQ2_B1R1)
                | P_Fld(0x10, RK1_PRE_TDQSCK4_TDQSCK_UIFREQ2_B1R1));
    vIO32WriteFldMulti(DRAMC_REG_RK1_PRE_TDQSCK5, P_Fld(0x0, RK1_PRE_TDQSCK5_TDQSCK_PIFREQ4_B1R1)
                | P_Fld(0x0, RK1_PRE_TDQSCK5_TDQSCK_UIFREQ4_B1R1)
                | P_Fld(0xe, RK1_PRE_TDQSCK5_TDQSCK_PIFREQ3_B1R1)
                | P_Fld(0x10, RK1_PRE_TDQSCK5_TDQSCK_UIFREQ3_B1R1));
    vIO32WriteFldMulti(DRAMC_REG_RK1_PRE_TDQSCK6, P_Fld(0x0, RK1_PRE_TDQSCK6_TDQSCK_UIFREQ4_P1_B1R1)
                | P_Fld(0x14, RK1_PRE_TDQSCK6_TDQSCK_UIFREQ3_P1_B1R1)
                | P_Fld(0x14, RK1_PRE_TDQSCK6_TDQSCK_UIFREQ2_P1_B1R1));
}

static void LegacyPreCalLP4_DDR3200(DRAMC_CTX_T *p)
{
    vIO32WriteFldMulti(DRAMC_REG_PRE_TDQSCK2, P_Fld(0x25, PRE_TDQSCK2_TDDQSCK_JUMP_RATIO0)
                | P_Fld(0x18, PRE_TDQSCK2_TDDQSCK_JUMP_RATIO1)
                | P_Fld(0x10, PRE_TDQSCK2_TDDQSCK_JUMP_RATIO2)
                | P_Fld(0x1b, PRE_TDQSCK2_TDDQSCK_JUMP_RATIO3));
    vIO32WriteFldMulti(DRAMC_REG_PRE_TDQSCK3, P_Fld(0x14, PRE_TDQSCK3_TDDQSCK_JUMP_RATIO4)
                | P_Fld(0xd, PRE_TDQSCK3_TDDQSCK_JUMP_RATIO5)
                | P_Fld(0x2a, PRE_TDQSCK3_TDDQSCK_JUMP_RATIO6)
                | P_Fld(0x31, PRE_TDQSCK3_TDDQSCK_JUMP_RATIO7));
    vIO32WriteFldMulti(DRAMC_REG_PRE_TDQSCK4, P_Fld(0x15, PRE_TDQSCK4_TDDQSCK_JUMP_RATIO8)
                | P_Fld(0x40, PRE_TDQSCK4_TDDQSCK_JUMP_RATIO9)
                | P_Fld(0x4a, PRE_TDQSCK4_TDDQSCK_JUMP_RATIO10)
                | P_Fld(0x30, PRE_TDQSCK4_TDDQSCK_JUMP_RATIO11));
    vIO32WriteFldMulti(DRAMC_REG_RK0_PRE_TDQSCK1, P_Fld(0x1a, RK0_PRE_TDQSCK1_TDQSCK_PIFREQ2_B0R0)
                | P_Fld(0x1c, RK0_PRE_TDQSCK1_TDQSCK_UIFREQ2_B0R0)
                | P_Fld(0x14, RK0_PRE_TDQSCK1_TDQSCK_UIFREQ1_B0R0));
    vIO32WriteFldMulti(DRAMC_REG_RK0_PRE_TDQSCK2, P_Fld(0xb, RK0_PRE_TDQSCK2_TDQSCK_PIFREQ4_B0R0)
                | P_Fld(0xd, RK0_PRE_TDQSCK2_TDQSCK_UIFREQ4_B0R0)
                | P_Fld(0x10, RK0_PRE_TDQSCK2_TDQSCK_PIFREQ3_B0R0)
                | P_Fld(0xa, RK0_PRE_TDQSCK2_TDQSCK_UIFREQ3_B0R0));
    vIO32WriteFldMulti(DRAMC_REG_RK0_PRE_TDQSCK3, P_Fld(0x11, RK0_PRE_TDQSCK3_TDQSCK_UIFREQ4_P1_B0R0)
                | P_Fld(0xe, RK0_PRE_TDQSCK3_TDQSCK_UIFREQ3_P1_B0R0)
                | P_Fld(0x20, RK0_PRE_TDQSCK3_TDQSCK_UIFREQ2_P1_B0R0)
                | P_Fld(0x18, RK0_PRE_TDQSCK3_TDQSCK_UIFREQ1_P1_B0R0));
    vIO32WriteFldMulti(DRAMC_REG_RK0_PRE_TDQSCK4, P_Fld(0x1a, RK0_PRE_TDQSCK4_TDQSCK_PIFREQ2_B1R0)
                | P_Fld(0x1c, RK0_PRE_TDQSCK4_TDQSCK_UIFREQ2_B1R0)
                | P_Fld(0x14, RK0_PRE_TDQSCK4_TDQSCK_UIFREQ1_B1R0));
    vIO32WriteFldMulti(DRAMC_REG_RK0_PRE_TDQSCK5, P_Fld(0xb, RK0_PRE_TDQSCK5_TDQSCK_PIFREQ4_B1R0)
                | P_Fld(0xd, RK0_PRE_TDQSCK5_TDQSCK_UIFREQ4_B1R0)
                | P_Fld(0x10, RK0_PRE_TDQSCK5_TDQSCK_PIFREQ3_B1R0)
                | P_Fld(0xa, RK0_PRE_TDQSCK5_TDQSCK_UIFREQ3_B1R0));
    vIO32WriteFldMulti(DRAMC_REG_RK0_PRE_TDQSCK6, P_Fld(0x11, RK0_PRE_TDQSCK6_TDQSCK_UIFREQ4_P1_B1R0)
                | P_Fld(0xe, RK0_PRE_TDQSCK6_TDQSCK_UIFREQ3_P1_B1R0)
                | P_Fld(0x20, RK0_PRE_TDQSCK6_TDQSCK_UIFREQ2_P1_B1R0)
                | P_Fld(0x18, RK0_PRE_TDQSCK6_TDQSCK_UIFREQ1_P1_B1R0));
    vIO32WriteFldMulti(DRAMC_REG_RK1_PRE_TDQSCK1, P_Fld(0xb, RK1_PRE_TDQSCK1_TDQSCK_PIFREQ2_B0R1)
                | P_Fld(0x23, RK1_PRE_TDQSCK1_TDQSCK_UIFREQ2_B0R1)
                | P_Fld(0xf, RK1_PRE_TDQSCK1_TDQSCK_PIFREQ1_B0R1)
                | P_Fld(0x19, RK1_PRE_TDQSCK1_TDQSCK_UIFREQ1_B0R1));
    vIO32WriteFldMulti(DRAMC_REG_RK1_PRE_TDQSCK2, P_Fld(0xe, RK1_PRE_TDQSCK2_TDQSCK_PIFREQ4_B0R1)
                | P_Fld(0x10, RK1_PRE_TDQSCK2_TDQSCK_UIFREQ4_B0R1)
                | P_Fld(0x1f, RK1_PRE_TDQSCK2_TDQSCK_PIFREQ3_B0R1)
                | P_Fld(0xe, RK1_PRE_TDQSCK2_TDQSCK_UIFREQ3_B0R1));
    vIO32WriteFldMulti(DRAMC_REG_RK1_PRE_TDQSCK3, P_Fld(0x14, RK1_PRE_TDQSCK3_TDQSCK_UIFREQ4_P1_B0R1)
                | P_Fld(0x12, RK1_PRE_TDQSCK3_TDQSCK_UIFREQ3_P1_B0R1)
                | P_Fld(0x27, RK1_PRE_TDQSCK3_TDQSCK_UIFREQ2_P1_B0R1)
                | P_Fld(0x1d, RK1_PRE_TDQSCK3_TDQSCK_UIFREQ1_P1_B0R1));
    vIO32WriteFldMulti(DRAMC_REG_RK1_PRE_TDQSCK4, P_Fld(0xb, RK1_PRE_TDQSCK4_TDQSCK_PIFREQ2_B1R1)
                | P_Fld(0x23, RK1_PRE_TDQSCK4_TDQSCK_UIFREQ2_B1R1)
                | P_Fld(0xf, RK1_PRE_TDQSCK4_TDQSCK_PIFREQ1_B1R1)
                | P_Fld(0x19, RK1_PRE_TDQSCK4_TDQSCK_UIFREQ1_B1R1));
    vIO32WriteFldMulti(DRAMC_REG_RK1_PRE_TDQSCK5, P_Fld(0xe, RK1_PRE_TDQSCK5_TDQSCK_PIFREQ4_B1R1)
                | P_Fld(0x10, RK1_PRE_TDQSCK5_TDQSCK_UIFREQ4_B1R1)
                | P_Fld(0x1f, RK1_PRE_TDQSCK5_TDQSCK_PIFREQ3_B1R1)
                | P_Fld(0xe, RK1_PRE_TDQSCK5_TDQSCK_UIFREQ3_B1R1));
    vIO32WriteFldMulti(DRAMC_REG_RK1_PRE_TDQSCK6, P_Fld(0x14, RK1_PRE_TDQSCK6_TDQSCK_UIFREQ4_P1_B1R1)
                | P_Fld(0x12, RK1_PRE_TDQSCK6_TDQSCK_UIFREQ3_P1_B1R1)
                | P_Fld(0x27, RK1_PRE_TDQSCK6_TDQSCK_UIFREQ2_P1_B1R1)
                | P_Fld(0x1d, RK1_PRE_TDQSCK6_TDQSCK_UIFREQ1_P1_B1R1));
}
#endif

#if LEGACY_GATING_DLY
/* Legacy initial settings (actual correct values are set during gating calibration)
 * Ex: TX_DLY_DQS#_GATED, TX_DLY_DQS#_GATED_P1, REG_DLY_DQS#_GATED, REG_DLY_DQS#_GATED_P1
 *     TXDLY_B#_RODTEN, TXDLY_B#_RODTEN_P1, DLY_B#_RODTEN, DLY_B#_RODTEN_P1
 */
static void LegacyGatingDlyLP3(DRAMC_CTX_T *p)
{
    vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_ODTEN0, P_Fld(0x0, SHURK0_SELPH_ODTEN0_TXDLY_B3_RODTEN_P1)
                | P_Fld(0x0, SHURK0_SELPH_ODTEN0_TXDLY_B3_RODTEN)
                | P_Fld(0x0, SHURK0_SELPH_ODTEN0_TXDLY_B2_RODTEN_P1)
                | P_Fld(0x0, SHURK0_SELPH_ODTEN0_TXDLY_B2_RODTEN)
                | P_Fld(0x0, SHURK0_SELPH_ODTEN0_TXDLY_B1_RODTEN_P1)
                | P_Fld(0x0, SHURK0_SELPH_ODTEN0_TXDLY_B1_RODTEN)
                | P_Fld(0x0, SHURK0_SELPH_ODTEN0_TXDLY_B0_RODTEN_P1)
                | P_Fld(0x0, SHURK0_SELPH_ODTEN0_TXDLY_B0_RODTEN));
    vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_ODTEN1, P_Fld(0x4, SHURK0_SELPH_ODTEN1_DLY_B3_RODTEN_P1)
                | P_Fld(0x4, SHURK0_SELPH_ODTEN1_DLY_B3_RODTEN)
                | P_Fld(0x4, SHURK0_SELPH_ODTEN1_DLY_B2_RODTEN_P1)
                | P_Fld(0x4, SHURK0_SELPH_ODTEN1_DLY_B2_RODTEN)
                | P_Fld(0x4, SHURK0_SELPH_ODTEN1_DLY_B1_RODTEN_P1)
                | P_Fld(0x4, SHURK0_SELPH_ODTEN1_DLY_B1_RODTEN)
                | P_Fld(0x4, SHURK0_SELPH_ODTEN1_DLY_B0_RODTEN_P1)
                | P_Fld(0x4, SHURK0_SELPH_ODTEN1_DLY_B0_RODTEN));
    vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_ODTEN0, P_Fld(0x0, SHURK1_SELPH_ODTEN0_TXDLY_B3_R1RODTEN_P1)
                | P_Fld(0x0, SHURK1_SELPH_ODTEN0_TXDLY_B3_R1RODTEN)
                | P_Fld(0x0, SHURK1_SELPH_ODTEN0_TXDLY_B2_R1RODTEN_P1)
                | P_Fld(0x0, SHURK1_SELPH_ODTEN0_TXDLY_B2_R1RODTEN)
                | P_Fld(0x0, SHURK1_SELPH_ODTEN0_TXDLY_B1_R1RODTEN_P1)
                | P_Fld(0x0, SHURK1_SELPH_ODTEN0_TXDLY_B1_R1RODTEN)
                | P_Fld(0x0, SHURK1_SELPH_ODTEN0_TXDLY_B0_R1RODTEN_P1)
                | P_Fld(0x0, SHURK1_SELPH_ODTEN0_TXDLY_B0_R1RODTEN));
    vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_ODTEN1, P_Fld(0x0, SHURK1_SELPH_ODTEN1_DLY_B3_R1RODTEN_P1)
                | P_Fld(0x0, SHURK1_SELPH_ODTEN1_DLY_B3_R1RODTEN)
                | P_Fld(0x4, SHURK1_SELPH_ODTEN1_DLY_B2_R1RODTEN_P1)
                | P_Fld(0x4, SHURK1_SELPH_ODTEN1_DLY_B2_R1RODTEN)
                | P_Fld(0x4, SHURK1_SELPH_ODTEN1_DLY_B1_R1RODTEN_P1)
                | P_Fld(0x4, SHURK1_SELPH_ODTEN1_DLY_B1_R1RODTEN)
                | P_Fld(0x4, SHURK1_SELPH_ODTEN1_DLY_B0_R1RODTEN_P1)
                | P_Fld(0x4, SHURK1_SELPH_ODTEN1_DLY_B0_R1RODTEN));
    vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQSG0, P_Fld(0x1, SHURK0_SELPH_DQSG0_TX_DLY_DQS3_GATED_P1)
                | P_Fld(0x1, SHURK0_SELPH_DQSG0_TX_DLY_DQS3_GATED)
                | P_Fld(0x1, SHURK0_SELPH_DQSG0_TX_DLY_DQS2_GATED_P1)
                | P_Fld(0x1, SHURK0_SELPH_DQSG0_TX_DLY_DQS2_GATED)
                | P_Fld(0x1, SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED_P1)
                | P_Fld(0x1, SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED)
                | P_Fld(0x1, SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED_P1)
                | P_Fld(0x1, SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED));
    vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQSG1, P_Fld(0x4, SHURK0_SELPH_DQSG1_REG_DLY_DQS3_GATED_P1)
                | P_Fld(0x2, SHURK0_SELPH_DQSG1_REG_DLY_DQS3_GATED)
                | P_Fld(0x4, SHURK0_SELPH_DQSG1_REG_DLY_DQS2_GATED_P1)
                | P_Fld(0x2, SHURK0_SELPH_DQSG1_REG_DLY_DQS2_GATED)
                | P_Fld(0x4, SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED_P1)
                | P_Fld(0x2, SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED)
                | P_Fld(0x4, SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED_P1)
                | P_Fld(0x2, SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED));
    vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQSG0, P_Fld(0x2, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS3_GATED_P1)
                | P_Fld(0x1, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS3_GATED)
                | P_Fld(0x2, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS2_GATED_P1)
                | P_Fld(0x1, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS2_GATED)
                | P_Fld(0x2, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS1_GATED_P1)
                | P_Fld(0x1, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS1_GATED)
                | P_Fld(0x2, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS0_GATED_P1)
                | P_Fld(0x1, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS0_GATED));
    vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQSG1, P_Fld(0x1, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS3_GATED_P1)
                | P_Fld(0x7, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS3_GATED)
                | P_Fld(0x1, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS2_GATED_P1)
                | P_Fld(0x7, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS2_GATED)
                | P_Fld(0x1, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS1_GATED_P1)
                | P_Fld(0x7, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS1_GATED)
                | P_Fld(0x1, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS0_GATED_P1)
                | P_Fld(0x7, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS0_GATED));
    vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQSG0 + SHIFT_TO_CHB_ADDR, P_Fld(0x1, SHURK0_SELPH_DQSG0_TX_DLY_DQS3_GATED_P1)
                | P_Fld(0x1, SHURK0_SELPH_DQSG0_TX_DLY_DQS3_GATED)
                | P_Fld(0x1, SHURK0_SELPH_DQSG0_TX_DLY_DQS2_GATED_P1)
                | P_Fld(0x1, SHURK0_SELPH_DQSG0_TX_DLY_DQS2_GATED)
                | P_Fld(0x1, SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED_P1)
                | P_Fld(0x1, SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED)
                | P_Fld(0x1, SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED_P1)
                | P_Fld(0x1, SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED));
    vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQSG1 + SHIFT_TO_CHB_ADDR, P_Fld(0x3, SHURK0_SELPH_DQSG1_REG_DLY_DQS3_GATED_P1)
                | P_Fld(0x1, SHURK0_SELPH_DQSG1_REG_DLY_DQS3_GATED)
                | P_Fld(0x3, SHURK0_SELPH_DQSG1_REG_DLY_DQS2_GATED_P1)
                | P_Fld(0x1, SHURK0_SELPH_DQSG1_REG_DLY_DQS2_GATED)
                | P_Fld(0x3, SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED_P1)
                | P_Fld(0x1, SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED)
                | P_Fld(0x3, SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED_P1)
                | P_Fld(0x1, SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED));
    vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQSG0 + SHIFT_TO_CHB_ADDR, P_Fld(0x2, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS3_GATED_P1)
                | P_Fld(0x1, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS3_GATED)
                | P_Fld(0x2, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS2_GATED_P1)
                | P_Fld(0x1, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS2_GATED)
                | P_Fld(0x2, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS1_GATED_P1)
                | P_Fld(0x1, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS1_GATED)
                | P_Fld(0x2, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS0_GATED_P1)
                | P_Fld(0x1, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS0_GATED));
    vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQSG1 + SHIFT_TO_CHB_ADDR, P_Fld(0x1, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS3_GATED_P1)
                | P_Fld(0x7, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS3_GATED)
                | P_Fld(0x1, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS2_GATED_P1)
                | P_Fld(0x7, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS2_GATED)
                | P_Fld(0x1, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS1_GATED_P1)
                | P_Fld(0x7, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS1_GATED)
                | P_Fld(0x1, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS0_GATED_P1)
                | P_Fld(0x7, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS0_GATED));
}

static void LegacyGatingDlyLP4_DDR800(DRAMC_CTX_T *p)
{

    U8 ucR0GatingMCK = 0, ucR0GatingB0UI = 0, ucR0GatingB1UI = 0;
    U8 ucR0GatingP1MCK = 0, ucR0GatingB0P1UI = 0, ucR0GatingB1P1UI = 0;

    U8 ucR1GatingMCK = 0, ucR1GatingB0UI = 0, ucR1GatingB1UI = 0;
    U8 ucR1GatingP1MCK = 0, ucR1GatingB0P1UI = 0, ucR1GatingB1P1UI = 0;

    U8 ucR0GatingB0PI = 0, ucR0GatingB1PI = 0;
    U8 ucR1GatingB0PI = 0, ucR1GatingB1PI = 0;

    if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
    {
        // Byte mode don't use. need fine-tune for DV sim for DDR800
        //RK0
        ucR0GatingMCK = 0x2;
        ucR0GatingP1MCK = 0x2;
        ucR0GatingB0UI = 0x2;
        ucR0GatingB1UI = 0x2;
        ucR0GatingB0P1UI = 0x6;
        ucR0GatingB1P1UI = 0x6;
        //RK1
        ucR1GatingMCK = 0x2;
        ucR1GatingP1MCK = 0x3;
        ucR1GatingB0UI = 0x5;
        ucR1GatingB1UI = 0x5;
        ucR1GatingB0P1UI = 0x1;
        ucR1GatingB1P1UI = 0x1;
    }
    else
    {
        //RK0
        ucR0GatingMCK = 0x1;
        ucR0GatingP1MCK = 0x1;
        ucR0GatingB0UI = 0x4;
        ucR0GatingB1UI = 0x4;
        ucR0GatingB0P1UI = 0x6;
        ucR0GatingB1P1UI = 0x6;
        //RK1
        ucR1GatingMCK = 0x1;
        ucR1GatingP1MCK = 0x1;
        ucR1GatingB0UI = 0x5;
        ucR1GatingB1UI = 0x5;
        ucR1GatingB0P1UI = 0x7;
        ucR1GatingB1P1UI = 0x7;
    }

    //Gating PI
    if (vGet_DDR800_Mode(p) == DDR800_CLOSE_LOOP)
    {
        ucR0GatingB0PI = 0xc;
        ucR0GatingB1PI = 0xe;
        ucR1GatingB0PI = 0x1e;
        ucR1GatingB1PI = 0x1e;
    }
    else
    {
        // DDR800_SEMI_LOOP and DDR800_OPEN_LOOP
        ucR0GatingB0PI = 0x0;
        ucR0GatingB1PI = 0x0;
        ucR1GatingB0PI = 0x0;
        ucR1GatingB1PI = 0x0;
    }

    //Gating RK0 MCK
    vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQSG0, P_Fld(ucR0GatingP1MCK, SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED_P1)
                | P_Fld(ucR0GatingMCK, SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED)
                | P_Fld(ucR0GatingP1MCK, SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED_P1)
                | P_Fld(ucR0GatingMCK, SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED));
    //Gating RK0 UI
    vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQSG1, P_Fld(ucR0GatingB1P1UI, SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED_P1)
                | P_Fld(ucR0GatingB1UI, SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED)
                | P_Fld(ucR0GatingB0P1UI, SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED_P1)
                | P_Fld(ucR0GatingB0UI, SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED));

    //Gating RK1 MCK
    vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQSG0, P_Fld(ucR1GatingP1MCK, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS1_GATED_P1)
                | P_Fld(ucR1GatingMCK, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS1_GATED)
                | P_Fld(ucR1GatingP1MCK, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS0_GATED_P1)
                | P_Fld(ucR1GatingMCK, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS0_GATED));
    //Gating RK1 UI
    vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQSG1, P_Fld(ucR1GatingB1P1UI, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS1_GATED_P1)
                | P_Fld(ucR1GatingB1UI, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS1_GATED)
                | P_Fld(ucR1GatingB0P1UI, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS0_GATED_P1)
                | P_Fld(ucR1GatingB0UI, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS0_GATED));

    //Gating RK0 PI
    vIO32WriteFldMulti(DRAMC_REG_SHURK0_DQSIEN, P_Fld(ucR0GatingB1PI, SHURK0_DQSIEN_R0DQS1IEN)
                | P_Fld(ucR0GatingB0PI, SHURK0_DQSIEN_R0DQS0IEN));
    //Gating RK1 PI
    vIO32WriteFldMulti(DRAMC_REG_SHURK1_DQSIEN, P_Fld(ucR1GatingB1PI, SHURK1_DQSIEN_R1DQS1IEN)
                | P_Fld(ucR1GatingB0PI, SHURK1_DQSIEN_R1DQS0IEN));
}


static void LegacyGatingDlyLP4_DDR1600(DRAMC_CTX_T *p)
{
    U8 ucR0GatingMCK = 0, ucR0GatingB0UI = 0, ucR0GatingB1UI = 0;
    U8 ucR0GatingP1MCK = 0, ucR0GatingB0P1UI = 0, ucR0GatingB1P1UI = 0;

    U8 ucR1GatingMCK = 0, ucR1GatingB0UI = 0, ucR1GatingB1UI = 0;
    U8 ucR1GatingP1MCK = 0, ucR1GatingB0P1UI = 0, ucR1GatingB1P1UI = 0;

    U8 ucR0GatingB0PI = 0, ucR0GatingB1PI = 0;
    U8 ucR1GatingB0PI = 0, ucR1GatingB1PI = 0;

    if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
    {
        // DV sim mix mode = RK0_BYTE and RK1_NORMAL
        if ((p->dram_cbt_mode[RANK_0] == CBT_BYTE_MODE1) && (p->dram_cbt_mode[RANK_1] == CBT_NORMAL_MODE)) // DV sim mixed mode
        {
            //RK0
            ucR0GatingMCK = 0x2;
            ucR0GatingP1MCK = 0x2;
            ucR0GatingB0UI = 0x0;
            ucR0GatingB1UI = 0x0;
            ucR0GatingB0P1UI = 0x4;
            ucR0GatingB1P1UI = 0x4;
            //RK1
            ucR1GatingMCK = 0x2;
            ucR1GatingP1MCK = 0x2;
            ucR1GatingB0UI = 0x3;
            ucR1GatingB1UI = 0x2;
            ucR1GatingB0P1UI = 0x7;
            ucR1GatingB1P1UI = 0x6;
        }
        else
        {
            //RK0
            ucR0GatingMCK = 0x2;
            ucR0GatingP1MCK = 0x2;
            ucR0GatingB0UI = 0x0;
            ucR0GatingB1UI = 0x1;
            ucR0GatingB0P1UI = 0x4;
            ucR0GatingB1P1UI = 0x5;
            //RK1
            ucR1GatingMCK = 0x1;
            ucR1GatingP1MCK = 0x2;
            ucR1GatingB0UI = 0x7;
            ucR1GatingB1UI = 0x7;
            ucR1GatingB0P1UI = 0x3;
            ucR1GatingB1P1UI = 0x3;
        }
    }
    else
    {
        // Normal mode DRAM
        //Normal mode dram, B0/1 tDQSCK = 1.5ns
        //Byte/Mix mode dram, B0 tDQSCK = 1.5ns, B1 tDQSCK = 1.95ns
        //RK1, tDQSCK=3.5ns
        if (vGet_Div_Mode(p) == DIV4_MODE)
        {
            //RK0
            ucR0GatingMCK = 0x1;
            ucR0GatingP1MCK = 0x1;
            ucR0GatingB0UI = 0x4;
            ucR0GatingB1UI = 0x4;
            ucR0GatingB0P1UI = 0x6;
            ucR0GatingB1P1UI = 0x6;
            //RK1
            ucR1GatingMCK = 0x1;
            ucR1GatingP1MCK = 0x2;
            ucR1GatingB0UI = 0x7;
            ucR1GatingB1UI = 0x7;
            ucR1GatingB0P1UI = 0x1;
            ucR1GatingB1P1UI = 0x1;
        }
        else
        {
            //RK0
            ucR0GatingMCK = 0x2;
            ucR0GatingP1MCK = 0x3;
            ucR0GatingB0UI = 0x4;
            ucR0GatingB1UI = 0x4;
            ucR0GatingB0P1UI = 0x0;
            ucR0GatingB1P1UI = 0x0;
            //RK1
            ucR1GatingMCK = 0x2;
            ucR1GatingP1MCK = 0x3;
            ucR1GatingB0UI = 0x7;
            ucR1GatingB1UI = 0x7;
            ucR1GatingB0P1UI = 0x3;
            ucR1GatingB1P1UI = 0x3;
        }
    }

    //Gating PI
    if (vGet_Div_Mode(p) == DIV4_MODE)
    {
        ucR0GatingB0PI = 0xb;
        ucR0GatingB1PI = 0x0;
        ucR1GatingB0PI = 0x1;
        ucR1GatingB1PI = 0x1;
    }
    else
    {
        // for DDR1600 1:8 mode
        if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
        {
            if ((p->dram_cbt_mode[RANK_0] == CBT_BYTE_MODE1) && (p->dram_cbt_mode[RANK_1] == CBT_BYTE_MODE1)) // RK1 gating for byte mode
            {
                ucR0GatingB0PI = 0xa;
                ucR0GatingB1PI = 0x0;
                ucR1GatingB0PI = 0xc;
                ucR1GatingB1PI = 0x16;
            }
            else if ((p->dram_cbt_mode[RANK_0] == CBT_BYTE_MODE1) && (p->dram_cbt_mode[RANK_1] == CBT_NORMAL_MODE)) // DV sim mixed mode
            {
                ucR0GatingB0PI = 0xa;
                ucR0GatingB1PI = 0x1e;
                ucR1GatingB0PI = 0xc;
                ucR1GatingB1PI = 0xc;
            }
            else //RK0: normal and RK1: byte
            {
                ucR0GatingB0PI = 0xa;
                ucR0GatingB1PI = 0x0;
                ucR1GatingB0PI = 0xc;
                ucR1GatingB1PI = 0xc;

            }
        }
        else
        {
            ucR0GatingB0PI = 0x0;
            ucR0GatingB1PI = 0x0;
            ucR1GatingB0PI = 0x0;
            ucR1GatingB1PI = 0x0;
        }
    }

    //Gating RK0 MCK
    vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQSG0, P_Fld(ucR0GatingP1MCK, SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED_P1)
                | P_Fld(ucR0GatingMCK, SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED)
                | P_Fld(ucR0GatingP1MCK, SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED_P1)
                | P_Fld(ucR0GatingMCK, SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED));
    //Gating RK0 UI
    vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQSG1, P_Fld(ucR0GatingB1P1UI, SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED_P1)
                | P_Fld(ucR0GatingB1UI, SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED)
                | P_Fld(ucR0GatingB0P1UI, SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED_P1)
                | P_Fld(ucR0GatingB0UI, SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED));

    //Gating RK1 MCK
    vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQSG0, P_Fld(ucR1GatingP1MCK, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS1_GATED_P1)
                | P_Fld(ucR1GatingMCK, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS1_GATED)
                | P_Fld(ucR1GatingP1MCK, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS0_GATED_P1)
                | P_Fld(ucR1GatingMCK, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS0_GATED));
    //Gating RK1 UI
    vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQSG1, P_Fld(ucR1GatingB1P1UI, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS1_GATED_P1)
                | P_Fld(ucR1GatingB1UI, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS1_GATED)
                | P_Fld(ucR1GatingB0P1UI, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS0_GATED_P1)
                | P_Fld(ucR1GatingB0UI, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS0_GATED));

    //Gating RK0 PI
    vIO32WriteFldMulti(DRAMC_REG_SHURK0_DQSIEN, P_Fld(ucR0GatingB1PI, SHURK0_DQSIEN_R0DQS1IEN)
                | P_Fld(ucR0GatingB0PI, SHURK0_DQSIEN_R0DQS0IEN));
    //Gating RK1 PI
    vIO32WriteFldMulti(DRAMC_REG_SHURK1_DQSIEN, P_Fld(ucR1GatingB1PI, SHURK1_DQSIEN_R1DQS1IEN)
                | P_Fld(ucR1GatingB0PI, SHURK1_DQSIEN_R1DQS0IEN));
}

static void LegacyGatingDlyLP4_DDR2667(DRAMC_CTX_T *p)
{
    if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
    {
       vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_ODTEN0, P_Fld(0x1, SHURK0_SELPH_ODTEN0_TXDLY_B1_RODTEN_P1)
                    | P_Fld(0x1, SHURK0_SELPH_ODTEN0_TXDLY_B1_RODTEN)
                    | P_Fld(0x1, SHURK0_SELPH_ODTEN0_TXDLY_B0_RODTEN_P1)
                    | P_Fld(0x1, SHURK0_SELPH_ODTEN0_TXDLY_B0_RODTEN));
        vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_ODTEN1, P_Fld(0x1, SHURK0_SELPH_ODTEN1_DLY_B1_RODTEN_P1)
                    | P_Fld(0x1, SHURK0_SELPH_ODTEN1_DLY_B1_RODTEN)
                    | P_Fld(0x1, SHURK0_SELPH_ODTEN1_DLY_B0_RODTEN_P1)
                    | P_Fld(0x1, SHURK0_SELPH_ODTEN1_DLY_B0_RODTEN));
        vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQSG0, P_Fld(0x2, SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED_P1)
                    | P_Fld(0x2, SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED)
                    | P_Fld(0x2, SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED_P1)
                    | P_Fld(0x2, SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED));
        vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQSG1, P_Fld(0x6, SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED_P1)
                    | P_Fld(0x2, SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED)
                    | P_Fld(0x6, SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED_P1)
                    | P_Fld(0x2, SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED));
    }
    else
    {
        vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_ODTEN0, P_Fld(0x2, SHURK0_SELPH_ODTEN0_TXDLY_B1_RODTEN_P1)
                    | P_Fld(0x2, SHURK0_SELPH_ODTEN0_TXDLY_B1_RODTEN)
                    | P_Fld(0x2, SHURK0_SELPH_ODTEN0_TXDLY_B0_RODTEN_P1)
                    | P_Fld(0x2, SHURK0_SELPH_ODTEN0_TXDLY_B0_RODTEN));
        vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_ODTEN1, P_Fld(0x1, SHURK0_SELPH_ODTEN1_DLY_B1_RODTEN_P1)
                    | P_Fld(0x1, SHURK0_SELPH_ODTEN1_DLY_B1_RODTEN)
                    | P_Fld(0x1, SHURK0_SELPH_ODTEN1_DLY_B0_RODTEN_P1)
                    | P_Fld(0x1, SHURK0_SELPH_ODTEN1_DLY_B0_RODTEN));

        vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQSG0, P_Fld(0x3, SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED_P1)
                    | P_Fld(0x3, SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED)
                    | P_Fld(0x3, SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED_P1)
                    | P_Fld(0x3, SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED));
        vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQSG1, P_Fld(0x4, SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED_P1)
                    | P_Fld(0x0, SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED)
                    | P_Fld(0x4, SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED_P1)
                    | P_Fld(0x0, SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED));
    }
    if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
    {
        vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_ODTEN0, P_Fld(0x1, SHURK1_SELPH_ODTEN0_TXDLY_B1_R1RODTEN_P1)
                    | P_Fld(0x1, SHURK1_SELPH_ODTEN0_TXDLY_B1_R1RODTEN)
                    | P_Fld(0x1, SHURK1_SELPH_ODTEN0_TXDLY_B0_R1RODTEN_P1)
                    | P_Fld(0x1, SHURK1_SELPH_ODTEN0_TXDLY_B0_R1RODTEN));
        vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_ODTEN1, P_Fld(0x6, SHURK1_SELPH_ODTEN1_DLY_B1_R1RODTEN_P1)
                    | P_Fld(0x6, SHURK1_SELPH_ODTEN1_DLY_B1_R1RODTEN)
                    | P_Fld(0x6, SHURK1_SELPH_ODTEN1_DLY_B0_R1RODTEN_P1)
                    | P_Fld(0x6, SHURK1_SELPH_ODTEN1_DLY_B0_R1RODTEN));
        vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQSG0, P_Fld(0x3, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS1_GATED_P1)
                    | P_Fld(0x3, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS1_GATED)
                    | P_Fld(0x3, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS0_GATED_P1)
                    | P_Fld(0x3, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS0_GATED));
    }
    else
    {
        vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_ODTEN0, P_Fld(0x2, SHURK1_SELPH_ODTEN0_TXDLY_B1_R1RODTEN_P1)
                    | P_Fld(0x2, SHURK1_SELPH_ODTEN0_TXDLY_B1_R1RODTEN)
                    | P_Fld(0x2, SHURK1_SELPH_ODTEN0_TXDLY_B0_R1RODTEN_P1)
                    | P_Fld(0x2, SHURK1_SELPH_ODTEN0_TXDLY_B0_R1RODTEN));
        vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_ODTEN1, P_Fld(0x6, SHURK1_SELPH_ODTEN1_DLY_B1_R1RODTEN_P1)
                    | P_Fld(0x6, SHURK1_SELPH_ODTEN1_DLY_B1_R1RODTEN)
                    | P_Fld(0x6, SHURK1_SELPH_ODTEN1_DLY_B0_R1RODTEN_P1)
                    | P_Fld(0x6, SHURK1_SELPH_ODTEN1_DLY_B0_R1RODTEN));
        vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQSG0, P_Fld(0x4, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS1_GATED_P1)
                    | P_Fld(0x3, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS1_GATED)
                    | P_Fld(0x4, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS0_GATED_P1)
                    | P_Fld(0x3, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS0_GATED));
    }
    vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQSG1, P_Fld(0x2, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS1_GATED_P1)
                | P_Fld(0x6, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS1_GATED)
                | P_Fld(0x2, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS0_GATED_P1)
                | P_Fld(0x6, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS0_GATED));
}

static void LegacyGatingDlyLP4_DDR3200(DRAMC_CTX_T *p)
{
    if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
    {
        vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_ODTEN1, P_Fld(0x2, SHURK0_SELPH_ODTEN1_DLY_B3_RODTEN_P1)
                    | P_Fld(0x2, SHURK0_SELPH_ODTEN1_DLY_B3_RODTEN)
                    | P_Fld(0x2, SHURK0_SELPH_ODTEN1_DLY_B2_RODTEN_P1)
                    | P_Fld(0x2, SHURK0_SELPH_ODTEN1_DLY_B2_RODTEN)
                    | P_Fld(0x6, SHURK0_SELPH_ODTEN1_DLY_B1_RODTEN_P1)
                    | P_Fld(0x6, SHURK0_SELPH_ODTEN1_DLY_B1_RODTEN)
                    | P_Fld(0x6, SHURK0_SELPH_ODTEN1_DLY_B0_RODTEN_P1)
                    | P_Fld(0x6, SHURK0_SELPH_ODTEN1_DLY_B0_RODTEN));
        vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_ODTEN0, P_Fld(0x2, SHURK1_SELPH_ODTEN0_TXDLY_B3_R1RODTEN_P1)
                    | P_Fld(0x2, SHURK1_SELPH_ODTEN0_TXDLY_B3_R1RODTEN)
                    | P_Fld(0x2, SHURK1_SELPH_ODTEN0_TXDLY_B2_R1RODTEN_P1)
                    | P_Fld(0x2, SHURK1_SELPH_ODTEN0_TXDLY_B2_R1RODTEN)
                    | P_Fld(0x2, SHURK1_SELPH_ODTEN0_TXDLY_B1_R1RODTEN_P1)
                    | P_Fld(0x2, SHURK1_SELPH_ODTEN0_TXDLY_B1_R1RODTEN)
                    | P_Fld(0x2, SHURK1_SELPH_ODTEN0_TXDLY_B0_R1RODTEN_P1)
                    | P_Fld(0x2, SHURK1_SELPH_ODTEN0_TXDLY_B0_R1RODTEN));
        vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_ODTEN1, P_Fld(0x4, SHURK1_SELPH_ODTEN1_DLY_B3_R1RODTEN_P1)
                    | P_Fld(0x4, SHURK1_SELPH_ODTEN1_DLY_B3_R1RODTEN)
                    | P_Fld(0x4, SHURK1_SELPH_ODTEN1_DLY_B2_R1RODTEN_P1)
                    | P_Fld(0x4, SHURK1_SELPH_ODTEN1_DLY_B2_R1RODTEN)
                    | P_Fld(0x2, SHURK1_SELPH_ODTEN1_DLY_B1_R1RODTEN_P1)
                    | P_Fld(0x2, SHURK1_SELPH_ODTEN1_DLY_B1_R1RODTEN)
                    | P_Fld(0x2, SHURK1_SELPH_ODTEN1_DLY_B0_R1RODTEN_P1)
                    | P_Fld(0x2, SHURK1_SELPH_ODTEN1_DLY_B0_R1RODTEN));
    }
    else
    {
        vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_ODTEN1, P_Fld(0x2, SHURK0_SELPH_ODTEN1_DLY_B3_RODTEN_P1)
                    | P_Fld(0x2, SHURK0_SELPH_ODTEN1_DLY_B3_RODTEN)
                    | P_Fld(0x2, SHURK0_SELPH_ODTEN1_DLY_B2_RODTEN_P1)
                    | P_Fld(0x2, SHURK0_SELPH_ODTEN1_DLY_B2_RODTEN));
        vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_ODTEN0, P_Fld(0x2, SHURK1_SELPH_ODTEN0_TXDLY_B3_R1RODTEN_P1)
                    | P_Fld(0x2, SHURK1_SELPH_ODTEN0_TXDLY_B3_R1RODTEN)
                    | P_Fld(0x2, SHURK1_SELPH_ODTEN0_TXDLY_B2_R1RODTEN_P1)
                    | P_Fld(0x2, SHURK1_SELPH_ODTEN0_TXDLY_B2_R1RODTEN));
        vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_ODTEN1, P_Fld(0x4, SHURK1_SELPH_ODTEN1_DLY_B3_R1RODTEN_P1)
                    | P_Fld(0x4, SHURK1_SELPH_ODTEN1_DLY_B3_R1RODTEN)
                    | P_Fld(0x4, SHURK1_SELPH_ODTEN1_DLY_B2_R1RODTEN_P1)
                    | P_Fld(0x4, SHURK1_SELPH_ODTEN1_DLY_B2_R1RODTEN)
                    | P_Fld(0x7, SHURK1_SELPH_ODTEN1_DLY_B1_R1RODTEN_P1)
                    | P_Fld(0x7, SHURK1_SELPH_ODTEN1_DLY_B1_R1RODTEN)
                    | P_Fld(0x7, SHURK1_SELPH_ODTEN1_DLY_B0_R1RODTEN_P1)
                    | P_Fld(0x7, SHURK1_SELPH_ODTEN1_DLY_B0_R1RODTEN));
    }
    if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
    {
        vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQSG0, P_Fld(0x4, SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED_P1)
                    | P_Fld(0x3, SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED)
                    | P_Fld(0x3, SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED_P1)
                    | P_Fld(0x3, SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED));
        vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQSG1, P_Fld(0x4, SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED)
                    | P_Fld(0x7, SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED_P1)
                    | P_Fld(0x3, SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED));
        vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQSG0, P_Fld(0x4, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS1_GATED_P1)
                    | P_Fld(0x3, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS1_GATED)
                    | P_Fld(0x4, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS0_GATED_P1)
                    | P_Fld(0x4, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS0_GATED));
        vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQSG1, P_Fld(0x3, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS1_GATED_P1)
                    | P_Fld(0x7, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS1_GATED)
                    | P_Fld(0x5, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS0_GATED_P1)
                    | P_Fld(0x1, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS0_GATED));
    }
    else
    {
        vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQSG0, P_Fld(0x2, SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED_P1)
                    | P_Fld(0x2, SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED)
                    | P_Fld(0x2, SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED_P1)
                    | P_Fld(0x2, SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED));
        vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQSG1, P_Fld(0x6, SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED_P1)
                    | P_Fld(0x2, SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED)
                    | P_Fld(0x6, SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED_P1)
                    | P_Fld(0x2, SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED));
        vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQSG0, P_Fld(0x3, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS1_GATED_P1)
                    | P_Fld(0x3, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS1_GATED)
                    | P_Fld(0x3, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS0_GATED_P1)
                    | P_Fld(0x3, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS0_GATED));
        vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQSG1, P_Fld(0x4, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS1_GATED_P1)
                    | P_Fld(0x4, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS0_GATED_P1));
    }
    if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
    {
    }
    else
    {
        vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQSG0, P_Fld(0x3, SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED_P1)
                    | P_Fld(0x3, SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED)
                    | P_Fld(0x3, SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED_P1)
                    | P_Fld(0x3, SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED));
        vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQSG0, P_Fld(0x4, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS1_GATED_P1)
                    | P_Fld(0x4, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS1_GATED)
                    | P_Fld(0x4, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS0_GATED_P1)
                    | P_Fld(0x4, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS0_GATED));
        vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_ODTEN0, P_Fld(0x2, SHURK0_SELPH_ODTEN0_TXDLY_B1_RODTEN)
                    | P_Fld(0x2, SHURK0_SELPH_ODTEN0_TXDLY_B0_RODTEN));
        vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_ODTEN0, P_Fld(0x2, SHURK1_SELPH_ODTEN0_TXDLY_B1_R1RODTEN)
                    | P_Fld(0x2, SHURK1_SELPH_ODTEN0_TXDLY_B0_R1RODTEN));
    }
    if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
    {
            vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_ODTEN0, P_Fld(0x0, SHURK0_SELPH_ODTEN0_TXDLY_B3_RODTEN_P1)
                        | P_Fld(0x0, SHURK0_SELPH_ODTEN0_TXDLY_B3_RODTEN)
                        | P_Fld(0x0, SHURK0_SELPH_ODTEN0_TXDLY_B2_RODTEN_P1)
                        | P_Fld(0x0, SHURK0_SELPH_ODTEN0_TXDLY_B2_RODTEN)
                        | P_Fld(0x2, SHURK0_SELPH_ODTEN0_TXDLY_B1_RODTEN_P1)
                        | P_Fld(0x2, SHURK0_SELPH_ODTEN0_TXDLY_B1_RODTEN)
                        | P_Fld(0x2, SHURK0_SELPH_ODTEN0_TXDLY_B0_RODTEN_P1)
                        | P_Fld(0x2, SHURK0_SELPH_ODTEN0_TXDLY_B0_RODTEN));
            vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_ODTEN1, P_Fld(0x0, SHURK0_SELPH_ODTEN1_DLY_B3_RODTEN_P1)
                        | P_Fld(0x0, SHURK0_SELPH_ODTEN1_DLY_B3_RODTEN)
                        | P_Fld(0x0, SHURK0_SELPH_ODTEN1_DLY_B2_RODTEN_P1)
                        | P_Fld(0x0, SHURK0_SELPH_ODTEN1_DLY_B2_RODTEN)
                        | P_Fld(0x0, SHURK0_SELPH_ODTEN1_DLY_B1_RODTEN_P1)
                        | P_Fld(0x0, SHURK0_SELPH_ODTEN1_DLY_B1_RODTEN)
                        | P_Fld(0x0, SHURK0_SELPH_ODTEN1_DLY_B0_RODTEN_P1)
                        | P_Fld(0x0, SHURK0_SELPH_ODTEN1_DLY_B0_RODTEN));
            vIO32WriteFldAlign(DRAMC_REG_SHURK0_SELPH_DQSG0, 0x4, SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED_P1);
            vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQSG1, P_Fld(0x1, SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED_P1)
                        | P_Fld(0x5, SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED)
                        | P_Fld(0x0, SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED_P1)
                        | P_Fld(0x4, SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED));
    }
    else
    {
            vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_ODTEN0, P_Fld(0x0, SHURK0_SELPH_ODTEN0_TXDLY_B3_RODTEN_P1)
                        | P_Fld(0x0, SHURK0_SELPH_ODTEN0_TXDLY_B3_RODTEN)
                        | P_Fld(0x0, SHURK0_SELPH_ODTEN0_TXDLY_B2_RODTEN_P1)
                        | P_Fld(0x0, SHURK0_SELPH_ODTEN0_TXDLY_B2_RODTEN)
                        | P_Fld(0x2, SHURK0_SELPH_ODTEN0_TXDLY_B1_RODTEN_P1)
                        | P_Fld(0x2, SHURK0_SELPH_ODTEN0_TXDLY_B0_RODTEN_P1));
            vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_ODTEN1, P_Fld(0x0, SHURK0_SELPH_ODTEN1_DLY_B3_RODTEN_P1)
                        | P_Fld(0x0, SHURK0_SELPH_ODTEN1_DLY_B3_RODTEN)
                        | P_Fld(0x0, SHURK0_SELPH_ODTEN1_DLY_B2_RODTEN_P1)
                        | P_Fld(0x0, SHURK0_SELPH_ODTEN1_DLY_B2_RODTEN)
                        | P_Fld(0x0, SHURK0_SELPH_ODTEN1_DLY_B1_RODTEN_P1)
                        | P_Fld(0x0, SHURK0_SELPH_ODTEN1_DLY_B1_RODTEN)
                        | P_Fld(0x0, SHURK0_SELPH_ODTEN1_DLY_B0_RODTEN_P1)
                        | P_Fld(0x0, SHURK0_SELPH_ODTEN1_DLY_B0_RODTEN));
            vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQSG1, P_Fld(0x7, SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED_P1)
                        | P_Fld(0x3, SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED)
                        | P_Fld(0x7, SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED_P1)
                        | P_Fld(0x3, SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED));
    }
    if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
    {
            vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_ODTEN0, P_Fld(0x0, SHURK1_SELPH_ODTEN0_TXDLY_B3_R1RODTEN_P1)
                        | P_Fld(0x0, SHURK1_SELPH_ODTEN0_TXDLY_B3_R1RODTEN)
                        | P_Fld(0x0, SHURK1_SELPH_ODTEN0_TXDLY_B2_R1RODTEN_P1)
                        | P_Fld(0x0, SHURK1_SELPH_ODTEN0_TXDLY_B2_R1RODTEN));
    }
    else
    {
            vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_ODTEN0, P_Fld(0x0, SHURK1_SELPH_ODTEN0_TXDLY_B3_R1RODTEN_P1)
                        | P_Fld(0x0, SHURK1_SELPH_ODTEN0_TXDLY_B3_R1RODTEN)
                        | P_Fld(0x0, SHURK1_SELPH_ODTEN0_TXDLY_B2_R1RODTEN_P1)
                        | P_Fld(0x0, SHURK1_SELPH_ODTEN0_TXDLY_B2_R1RODTEN)
                        | P_Fld(0x2, SHURK1_SELPH_ODTEN0_TXDLY_B1_R1RODTEN_P1)
                        | P_Fld(0x2, SHURK1_SELPH_ODTEN0_TXDLY_B0_R1RODTEN_P1));
    }
    vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_ODTEN1, P_Fld(0x0, SHURK1_SELPH_ODTEN1_DLY_B3_R1RODTEN_P1)
                | P_Fld(0x0, SHURK1_SELPH_ODTEN1_DLY_B3_R1RODTEN)
                | P_Fld(0x0, SHURK1_SELPH_ODTEN1_DLY_B2_R1RODTEN_P1)
                | P_Fld(0x0, SHURK1_SELPH_ODTEN1_DLY_B2_R1RODTEN)
                | P_Fld(0x0, SHURK1_SELPH_ODTEN1_DLY_B1_R1RODTEN_P1)
                | P_Fld(0x0, SHURK1_SELPH_ODTEN1_DLY_B1_R1RODTEN)
                | P_Fld(0x0, SHURK1_SELPH_ODTEN1_DLY_B0_R1RODTEN_P1)
                | P_Fld(0x0, SHURK1_SELPH_ODTEN1_DLY_B0_R1RODTEN));
    if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
    {
            vIO32WriteFldAlign(DRAMC_REG_SHURK1_SELPH_DQSG0, 0x4, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS1_GATED);
            vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQSG1, P_Fld(0x4, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS1_GATED_P1)
                        | P_Fld(0x0, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS1_GATED)
                        | P_Fld(0x6, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS0_GATED_P1)
                        | P_Fld(0x2, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS0_GATED));
    }
    else
    {
            vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQSG1, P_Fld(0x5, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS1_GATED_P1)
                        | P_Fld(0x1, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS1_GATED)
                        | P_Fld(0x5, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS0_GATED_P1)
                        | P_Fld(0x1, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS0_GATED));
    }
}

static void LegacyGatingDlyLP4_DDR3733(DRAMC_CTX_T *p)
{
    U8 ucR0GatingMCK = 0, ucR0GatingB0UI = 0, ucR0GatingB1UI = 0;
    U8 ucR1GatingMCK = 0, ucR1GatingB0UI = 0, ucR1GatingB1UI = 0;

    U8 ucR0GatingP1MCK = 0, ucR0GatingB0P1UI = 0, ucR0GatingB1P1UI = 0;
    U8 ucR1GatingP1MCK = 0, ucR1GatingB0P1UI = 0, ucR1GatingB1P1UI = 0;

    U8 ucR0GatingB0PI = 0, ucR0GatingB1PI = 0;
    U8 ucR1GatingB0PI = 0, ucR1GatingB1PI = 0;

    if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
    {
        // Byte mode don't use. need fine-tune for DV sim
        //RK0
        ucR0GatingMCK = 0x3;
        ucR0GatingP1MCK = 0x3;
        ucR0GatingB0UI = 0x3;
        ucR0GatingB1UI = 0x3;
        ucR0GatingB0P1UI = 0x7;
        ucR0GatingB1P1UI = 0x7;
        ucR0GatingB0PI = 0x14;
        ucR0GatingB1PI = 0x14;
        //RK1
        ucR1GatingMCK = 0x4;
        ucR1GatingP1MCK = 0x4;
        ucR1GatingB0UI = 0x4;
        ucR1GatingB1UI = 0x4;
        ucR1GatingB0P1UI = 0x7;
        ucR1GatingB1P1UI = 0x7;
        ucR1GatingB0PI = 0x4;
        ucR1GatingB1PI = 0x4;
    }
    else
    {
        //RK0
        //Normal mode dram, B0/1 tDQSCK = 1.5ns
        //Byte/Mix mode dram, B0 tDQSCK = 1.5ns, B1 tDQSCK = 1.95ns
        ucR0GatingMCK = 0x2;
        ucR0GatingP1MCK = 0x2;
        ucR0GatingB0UI = 0x3;
        ucR0GatingB1UI = 0x3;
        ucR0GatingB0P1UI = 0x7;
        ucR0GatingB1P1UI = 0x7;
        ucR0GatingB0PI = 0x11;
        ucR0GatingB1PI = 0x11;
        //RK1, tDQSCK=3.5ns
        ucR1GatingMCK = 0x3;
        ucR1GatingP1MCK = 0x3;
        ucR1GatingB0UI = 0x2;
        ucR1GatingB1UI = 0x2;
        ucR1GatingB0P1UI = 0x6;
        ucR1GatingB1P1UI = 0x6;
        ucR1GatingB0PI = 0x1f;
        ucR1GatingB1PI = 0x1f;
    }

    #if ENABLE_READ_DBI
    ucR0GatingMCK++;
    ucR0GatingP1MCK++;
    ucR1GatingMCK++;
    ucR1GatingP1MCK++;
    #endif

    //Gating RK0 MCK
    vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQSG0, P_Fld(ucR0GatingP1MCK, SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED_P1)
                | P_Fld(ucR0GatingMCK, SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED)
                | P_Fld(ucR0GatingP1MCK, SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED_P1)
                | P_Fld(ucR0GatingMCK, SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED));
    //Gating RK0 UI
    vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQSG1, P_Fld(ucR0GatingB1P1UI, SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED_P1)
                | P_Fld(ucR0GatingB1UI, SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED)
                | P_Fld(ucR0GatingB0P1UI, SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED_P1)
                | P_Fld(ucR0GatingB0UI, SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED));

    //Gating RK1 MCK
    vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQSG0, P_Fld(ucR1GatingP1MCK, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS1_GATED_P1)
                | P_Fld(ucR1GatingMCK, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS1_GATED)
                | P_Fld(ucR1GatingP1MCK, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS0_GATED_P1)
                | P_Fld(ucR1GatingMCK, SHURK1_SELPH_DQSG0_TX_DLY_R1DQS0_GATED));
    //Gating RK1 UI
    vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQSG1, P_Fld(ucR1GatingB1P1UI, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS1_GATED_P1)
                | P_Fld(ucR1GatingB1UI, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS1_GATED)
                | P_Fld(ucR1GatingB0P1UI, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS0_GATED_P1)
                | P_Fld(ucR1GatingB0UI, SHURK1_SELPH_DQSG1_REG_DLY_R1DQS0_GATED));

    //Gating RK0 PI
    vIO32WriteFldMulti(DRAMC_REG_SHURK0_DQSIEN, P_Fld(0x0, SHURK0_DQSIEN_R0DQS3IEN)
                | P_Fld(0x0, SHURK0_DQSIEN_R0DQS2IEN)
                | P_Fld(ucR0GatingB1PI, SHURK0_DQSIEN_R0DQS1IEN)
                | P_Fld(ucR0GatingB0PI, SHURK0_DQSIEN_R0DQS0IEN));

    //Gating RK1 PI
    vIO32WriteFldMulti(DRAMC_REG_SHURK1_DQSIEN, P_Fld(0x0, SHURK1_DQSIEN_R1DQS3IEN)
                | P_Fld(0x0, SHURK1_DQSIEN_R1DQS2IEN)
                | P_Fld(ucR1GatingB1PI, SHURK1_DQSIEN_R1DQS1IEN)
                | P_Fld(ucR1GatingB0PI, SHURK1_DQSIEN_R1DQS0IEN));
}

#endif //LEGACY_GATING_DLY

#if LEGACY_RX_DLY
/* Legacy RX related delay initial settings:
 * RK#_RX_ARDQ#_F_DLY_B#, RK#_RX_ARDQ#_R_DLY_B# (DQ Rx per bit falling/rising edge delay line control)
 * RK#_RX_ARDQS#_F_DLY_B#, RK#_RX_ARDQS#_R_DLY_B# (DQS RX per bit falling/rising edge delay line control)
 * RK#_RX_ARDQM#_F_DLY_B#, RK#_RX_ARDQM#_R_DLY_B# (DQM RX per bit falling/rising edge delay line control)
 * TODO: Channel B's PHY, seems to be all used for LP3's DQ, DQS, DQM -> LP3's Channel B RX CA, CKE etc.. delay should be regarded as RX_DLY?
 */
static void LegacyRxDly_LP4_DDR2667(DRAMC_CTX_T *p)
{
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B0_DQ2, P_Fld(0xa, SHU_R0_B0_DQ2_RK0_RX_ARDQ1_R_DLY_B0)
                | P_Fld(0xa, SHU_R0_B0_DQ2_RK0_RX_ARDQ0_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B0_DQ3, P_Fld(0xa, SHU_R0_B0_DQ3_RK0_RX_ARDQ3_R_DLY_B0)
                | P_Fld(0xa, SHU_R0_B0_DQ3_RK0_RX_ARDQ2_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B0_DQ4, P_Fld(0xa, SHU_R0_B0_DQ4_RK0_RX_ARDQ5_R_DLY_B0)
                | P_Fld(0xa, SHU_R0_B0_DQ4_RK0_RX_ARDQ4_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B0_DQ5, P_Fld(0xa, SHU_R0_B0_DQ5_RK0_RX_ARDQ7_R_DLY_B0)
                | P_Fld(0xa, SHU_R0_B0_DQ5_RK0_RX_ARDQ6_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B0_DQ6, P_Fld(0x15, SHU_R0_B0_DQ6_RK0_RX_ARDQS0_R_DLY_B0)
                | P_Fld(0xa, SHU_R0_B0_DQ6_RK0_RX_ARDQM0_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B1_DQ2, P_Fld(0xa, SHU_R0_B1_DQ2_RK0_RX_ARDQ1_R_DLY_B1)
                | P_Fld(0xa, SHU_R0_B1_DQ2_RK0_RX_ARDQ0_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B1_DQ3, P_Fld(0xa, SHU_R0_B1_DQ3_RK0_RX_ARDQ3_R_DLY_B1)
                | P_Fld(0xa, SHU_R0_B1_DQ3_RK0_RX_ARDQ2_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B1_DQ4, P_Fld(0xa, SHU_R0_B1_DQ4_RK0_RX_ARDQ5_R_DLY_B1)
                | P_Fld(0xa, SHU_R0_B1_DQ4_RK0_RX_ARDQ4_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B1_DQ5, P_Fld(0xa, SHU_R0_B1_DQ5_RK0_RX_ARDQ7_R_DLY_B1)
                | P_Fld(0xa, SHU_R0_B1_DQ5_RK0_RX_ARDQ6_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B1_DQ6, P_Fld(0x15, SHU_R0_B1_DQ6_RK0_RX_ARDQS0_R_DLY_B1)
                | P_Fld(0xa, SHU_R0_B1_DQ6_RK0_RX_ARDQM0_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B0_DQ2, P_Fld(0xb, SHU_R1_B0_DQ2_RK1_RX_ARDQ1_R_DLY_B0)
                | P_Fld(0xb, SHU_R1_B0_DQ2_RK1_RX_ARDQ0_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B0_DQ3, P_Fld(0xb, SHU_R1_B0_DQ3_RK1_RX_ARDQ3_R_DLY_B0)
                | P_Fld(0xb, SHU_R1_B0_DQ3_RK1_RX_ARDQ2_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B0_DQ4, P_Fld(0xb, SHU_R1_B0_DQ4_RK1_RX_ARDQ5_R_DLY_B0)
                | P_Fld(0xb, SHU_R1_B0_DQ4_RK1_RX_ARDQ4_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B0_DQ5, P_Fld(0xb, SHU_R1_B0_DQ5_RK1_RX_ARDQ7_R_DLY_B0)
                | P_Fld(0xb, SHU_R1_B0_DQ5_RK1_RX_ARDQ6_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B0_DQ6, P_Fld(0x16, SHU_R1_B0_DQ6_RK1_RX_ARDQS0_R_DLY_B0)
                | P_Fld(0xb, SHU_R1_B0_DQ6_RK1_RX_ARDQM0_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B1_DQ2, P_Fld(0xb, SHU_R1_B1_DQ2_RK1_RX_ARDQ1_R_DLY_B1)
                | P_Fld(0xb, SHU_R1_B1_DQ2_RK1_RX_ARDQ0_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B1_DQ3, P_Fld(0xb, SHU_R1_B1_DQ3_RK1_RX_ARDQ3_R_DLY_B1)
                | P_Fld(0xb, SHU_R1_B1_DQ3_RK1_RX_ARDQ2_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B1_DQ4, P_Fld(0xb, SHU_R1_B1_DQ4_RK1_RX_ARDQ5_R_DLY_B1)
                | P_Fld(0xb, SHU_R1_B1_DQ4_RK1_RX_ARDQ4_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B1_DQ5, P_Fld(0xb, SHU_R1_B1_DQ5_RK1_RX_ARDQ7_R_DLY_B1)
                | P_Fld(0xb, SHU_R1_B1_DQ5_RK1_RX_ARDQ6_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B1_DQ6, P_Fld(0x16, SHU_R1_B1_DQ6_RK1_RX_ARDQS0_R_DLY_B1)
                | P_Fld(0xb, SHU_R1_B1_DQ6_RK1_RX_ARDQM0_R_DLY_B1));
}

static void LegacyRxDly_LP4_DDR800(DRAMC_CTX_T *p)
{
    U8 u1Dq = 0x0;
    U8 u1Dqm = 0x0;
    U8 u1Dqs = 0x26;

    //RK0_B0
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B0_DQ2, P_Fld(u1Dq, SHU_R0_B0_DQ2_RK0_RX_ARDQ1_R_DLY_B0)
                | P_Fld(u1Dq, SHU_R0_B0_DQ2_RK0_RX_ARDQ0_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B0_DQ3, P_Fld(u1Dq, SHU_R0_B0_DQ3_RK0_RX_ARDQ3_R_DLY_B0)
                | P_Fld(u1Dq, SHU_R0_B0_DQ3_RK0_RX_ARDQ2_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B0_DQ4, P_Fld(u1Dq, SHU_R0_B0_DQ4_RK0_RX_ARDQ5_R_DLY_B0)
                | P_Fld(u1Dq, SHU_R0_B0_DQ4_RK0_RX_ARDQ4_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B0_DQ5, P_Fld(u1Dq, SHU_R0_B0_DQ5_RK0_RX_ARDQ7_R_DLY_B0)
                | P_Fld(u1Dq, SHU_R0_B0_DQ5_RK0_RX_ARDQ6_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B0_DQ6, P_Fld(u1Dqs, SHU_R0_B0_DQ6_RK0_RX_ARDQS0_R_DLY_B0)
                | P_Fld(u1Dqm, SHU_R0_B0_DQ6_RK0_RX_ARDQM0_R_DLY_B0));
    //RK0_B1
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B1_DQ2, P_Fld(u1Dq, SHU_R0_B1_DQ2_RK0_RX_ARDQ1_R_DLY_B1)
                | P_Fld(u1Dq, SHU_R0_B1_DQ2_RK0_RX_ARDQ0_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B1_DQ3, P_Fld(u1Dq, SHU_R0_B1_DQ3_RK0_RX_ARDQ3_R_DLY_B1)
                | P_Fld(u1Dq, SHU_R0_B1_DQ3_RK0_RX_ARDQ2_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B1_DQ4, P_Fld(u1Dq, SHU_R0_B1_DQ4_RK0_RX_ARDQ5_R_DLY_B1)
                | P_Fld(u1Dq, SHU_R0_B1_DQ4_RK0_RX_ARDQ4_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B1_DQ5, P_Fld(u1Dq, SHU_R0_B1_DQ5_RK0_RX_ARDQ7_R_DLY_B1)
                | P_Fld(u1Dq, SHU_R0_B1_DQ5_RK0_RX_ARDQ6_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B1_DQ6, P_Fld(u1Dqs, SHU_R0_B1_DQ6_RK0_RX_ARDQS0_R_DLY_B1)
                | P_Fld(u1Dqm, SHU_R0_B1_DQ6_RK0_RX_ARDQM0_R_DLY_B1));
    //RK1_B0
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B0_DQ2, P_Fld(u1Dq, SHU_R1_B0_DQ2_RK1_RX_ARDQ1_R_DLY_B0)
                | P_Fld(u1Dq, SHU_R1_B0_DQ2_RK1_RX_ARDQ0_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B0_DQ3, P_Fld(u1Dq, SHU_R1_B0_DQ3_RK1_RX_ARDQ3_R_DLY_B0)
                | P_Fld(u1Dq, SHU_R1_B0_DQ3_RK1_RX_ARDQ2_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B0_DQ4, P_Fld(u1Dq, SHU_R1_B0_DQ4_RK1_RX_ARDQ5_R_DLY_B0)
                | P_Fld(u1Dq, SHU_R1_B0_DQ4_RK1_RX_ARDQ4_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B0_DQ5, P_Fld(u1Dq, SHU_R1_B0_DQ5_RK1_RX_ARDQ7_R_DLY_B0)
                | P_Fld(u1Dq, SHU_R1_B0_DQ5_RK1_RX_ARDQ6_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B0_DQ6, P_Fld(u1Dqs, SHU_R1_B0_DQ6_RK1_RX_ARDQS0_R_DLY_B0)
                | P_Fld(u1Dqm, SHU_R1_B0_DQ6_RK1_RX_ARDQM0_R_DLY_B0));
    //RK1_B1
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B1_DQ2, P_Fld(u1Dq, SHU_R1_B1_DQ2_RK1_RX_ARDQ1_R_DLY_B1)
                | P_Fld(u1Dq, SHU_R1_B1_DQ2_RK1_RX_ARDQ0_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B1_DQ3, P_Fld(u1Dq, SHU_R1_B1_DQ3_RK1_RX_ARDQ3_R_DLY_B1)
                | P_Fld(u1Dq, SHU_R1_B1_DQ3_RK1_RX_ARDQ2_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B1_DQ4, P_Fld(u1Dq, SHU_R1_B1_DQ4_RK1_RX_ARDQ5_R_DLY_B1)
                | P_Fld(u1Dq, SHU_R1_B1_DQ4_RK1_RX_ARDQ4_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B1_DQ5, P_Fld(u1Dq, SHU_R1_B1_DQ5_RK1_RX_ARDQ7_R_DLY_B1)
                | P_Fld(u1Dq, SHU_R1_B1_DQ5_RK1_RX_ARDQ6_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B1_DQ6, P_Fld(u1Dqs, SHU_R1_B1_DQ6_RK1_RX_ARDQS0_R_DLY_B1)
                | P_Fld(u1Dqm, SHU_R1_B1_DQ6_RK1_RX_ARDQM0_R_DLY_B1));
}

static void LegacyRxDly_LP4_DDR1600(DRAMC_CTX_T *p)
{
    U8 u1Rk0_Dq, u1Rk0_Dq5_6;
    U8 u1Rk0_Dqm;
    U8 u1Rk0_Dqs;
    U8 u1Rk1_Dq;
    U8 u1Rk1_Dqm;
    U8 u1Rk1_Dqs;

    if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
    {
        u1Rk0_Dq = 0x0;
        u1Rk0_Dq5_6 = 0x2;
        u1Rk0_Dqm = 0x0;
        u1Rk0_Dqs = 0x7;
        u1Rk1_Dq = 0x0;
        u1Rk1_Dqm = 0x0;
        u1Rk1_Dqs = 0x5;
    }
    else
    {
        u1Rk0_Dq = 0x4;
        u1Rk0_Dq5_6 = 0x4;
        u1Rk0_Dqm = 0x4;
        u1Rk0_Dqs = 0x15;
        u1Rk1_Dq = 0x5;
        u1Rk1_Dqm = 0x5;
        u1Rk1_Dqs = 0x16;
    }

    //RK0_B0
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B0_DQ2, P_Fld(u1Rk0_Dq, SHU_R0_B0_DQ2_RK0_RX_ARDQ1_R_DLY_B0)
                | P_Fld(u1Rk0_Dq, SHU_R0_B0_DQ2_RK0_RX_ARDQ0_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B0_DQ3, P_Fld(u1Rk0_Dq, SHU_R0_B0_DQ3_RK0_RX_ARDQ3_R_DLY_B0)
                | P_Fld(u1Rk0_Dq, SHU_R0_B0_DQ3_RK0_RX_ARDQ2_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B0_DQ4, P_Fld(u1Rk0_Dq5_6, SHU_R0_B0_DQ4_RK0_RX_ARDQ5_R_DLY_B0)
                | P_Fld(u1Rk0_Dq, SHU_R0_B0_DQ4_RK0_RX_ARDQ4_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B0_DQ5, P_Fld(u1Rk0_Dq, SHU_R0_B0_DQ5_RK0_RX_ARDQ7_R_DLY_B0)
                | P_Fld(u1Rk0_Dq5_6, SHU_R0_B0_DQ5_RK0_RX_ARDQ6_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B0_DQ6, P_Fld(u1Rk0_Dqs, SHU_R0_B0_DQ6_RK0_RX_ARDQS0_R_DLY_B0)
                | P_Fld(u1Rk0_Dqm, SHU_R0_B0_DQ6_RK0_RX_ARDQM0_R_DLY_B0));
    //RK0_B1
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B1_DQ2, P_Fld(u1Rk0_Dq, SHU_R0_B1_DQ2_RK0_RX_ARDQ1_R_DLY_B1)
                | P_Fld(u1Rk0_Dq, SHU_R0_B1_DQ2_RK0_RX_ARDQ0_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B1_DQ3, P_Fld(u1Rk0_Dq, SHU_R0_B1_DQ3_RK0_RX_ARDQ3_R_DLY_B1)
                | P_Fld(u1Rk0_Dq, SHU_R0_B1_DQ3_RK0_RX_ARDQ2_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B1_DQ4, P_Fld(u1Rk0_Dq5_6, SHU_R0_B1_DQ4_RK0_RX_ARDQ5_R_DLY_B1)
                | P_Fld(u1Rk0_Dq, SHU_R0_B1_DQ4_RK0_RX_ARDQ4_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B1_DQ5, P_Fld(u1Rk0_Dq, SHU_R0_B1_DQ5_RK0_RX_ARDQ7_R_DLY_B1)
                | P_Fld(u1Rk0_Dq5_6, SHU_R0_B1_DQ5_RK0_RX_ARDQ6_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B1_DQ6, P_Fld(u1Rk0_Dqs, SHU_R0_B1_DQ6_RK0_RX_ARDQS0_R_DLY_B1)
                | P_Fld(u1Rk0_Dqm, SHU_R0_B1_DQ6_RK0_RX_ARDQM0_R_DLY_B1));
    //RK1_B0
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B0_DQ2, P_Fld(u1Rk1_Dq, SHU_R1_B0_DQ2_RK1_RX_ARDQ1_R_DLY_B0)
                | P_Fld(u1Rk1_Dq, SHU_R1_B0_DQ2_RK1_RX_ARDQ0_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B0_DQ3, P_Fld(u1Rk1_Dq, SHU_R1_B0_DQ3_RK1_RX_ARDQ3_R_DLY_B0)
                | P_Fld(u1Rk1_Dq, SHU_R1_B0_DQ3_RK1_RX_ARDQ2_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B0_DQ4, P_Fld(u1Rk1_Dq, SHU_R1_B0_DQ4_RK1_RX_ARDQ5_R_DLY_B0)
                | P_Fld(u1Rk1_Dq, SHU_R1_B0_DQ4_RK1_RX_ARDQ4_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B0_DQ5, P_Fld(u1Rk1_Dq, SHU_R1_B0_DQ5_RK1_RX_ARDQ7_R_DLY_B0)
                | P_Fld(u1Rk1_Dq, SHU_R1_B0_DQ5_RK1_RX_ARDQ6_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B0_DQ6, P_Fld(u1Rk1_Dqs, SHU_R1_B0_DQ6_RK1_RX_ARDQS0_R_DLY_B0)
                | P_Fld(u1Rk1_Dqm, SHU_R1_B0_DQ6_RK1_RX_ARDQM0_R_DLY_B0));
    //RK1_B1
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B1_DQ2, P_Fld(u1Rk1_Dq, SHU_R1_B1_DQ2_RK1_RX_ARDQ1_R_DLY_B1)
                | P_Fld(u1Rk1_Dq, SHU_R1_B1_DQ2_RK1_RX_ARDQ0_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B1_DQ3, P_Fld(u1Rk1_Dq, SHU_R1_B1_DQ3_RK1_RX_ARDQ3_R_DLY_B1)
                | P_Fld(u1Rk1_Dq, SHU_R1_B1_DQ3_RK1_RX_ARDQ2_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B1_DQ4, P_Fld(u1Rk1_Dq, SHU_R1_B1_DQ4_RK1_RX_ARDQ5_R_DLY_B1)
                | P_Fld(u1Rk1_Dq, SHU_R1_B1_DQ4_RK1_RX_ARDQ4_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B1_DQ5, P_Fld(u1Rk1_Dq, SHU_R1_B1_DQ5_RK1_RX_ARDQ7_R_DLY_B1)
                | P_Fld(u1Rk1_Dq, SHU_R1_B1_DQ5_RK1_RX_ARDQ6_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B1_DQ6, P_Fld(u1Rk1_Dqs, SHU_R1_B1_DQ6_RK1_RX_ARDQS0_R_DLY_B1)
                | P_Fld(u1Rk1_Dqm, SHU_R1_B1_DQ6_RK1_RX_ARDQM0_R_DLY_B1));
}

static void LegacyRxDly_LP4_DDR3200(DRAMC_CTX_T *p)
{
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B0_DQ2, P_Fld(0xa, SHU_R0_B0_DQ2_RK0_RX_ARDQ1_R_DLY_B0)
                | P_Fld(0xa, SHU_R0_B0_DQ2_RK0_RX_ARDQ0_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B0_DQ3, P_Fld(0xa, SHU_R0_B0_DQ3_RK0_RX_ARDQ3_R_DLY_B0)
                | P_Fld(0xa, SHU_R0_B0_DQ3_RK0_RX_ARDQ2_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B0_DQ4, P_Fld(0xa, SHU_R0_B0_DQ4_RK0_RX_ARDQ5_R_DLY_B0)
                | P_Fld(0xa, SHU_R0_B0_DQ4_RK0_RX_ARDQ4_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B0_DQ5, P_Fld(0xa, SHU_R0_B0_DQ5_RK0_RX_ARDQ7_R_DLY_B0)
                | P_Fld(0xa, SHU_R0_B0_DQ5_RK0_RX_ARDQ6_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B0_DQ6, P_Fld(0xc, SHU_R0_B0_DQ6_RK0_RX_ARDQS0_R_DLY_B0)
                | P_Fld(0xa, SHU_R0_B0_DQ6_RK0_RX_ARDQM0_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B0_DQ2, P_Fld(0xb, SHU_R1_B0_DQ2_RK1_RX_ARDQ1_R_DLY_B0)
                | P_Fld(0xb, SHU_R1_B0_DQ2_RK1_RX_ARDQ0_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B0_DQ3, P_Fld(0xb, SHU_R1_B0_DQ3_RK1_RX_ARDQ3_R_DLY_B0)
                | P_Fld(0xb, SHU_R1_B0_DQ3_RK1_RX_ARDQ2_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B0_DQ4, P_Fld(0xb, SHU_R1_B0_DQ4_RK1_RX_ARDQ5_R_DLY_B0)
                | P_Fld(0xb, SHU_R1_B0_DQ4_RK1_RX_ARDQ4_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B0_DQ5, P_Fld(0xb, SHU_R1_B0_DQ5_RK1_RX_ARDQ7_R_DLY_B0)
                | P_Fld(0xb, SHU_R1_B0_DQ5_RK1_RX_ARDQ6_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B0_DQ6, P_Fld(0xd, SHU_R1_B0_DQ6_RK1_RX_ARDQS0_R_DLY_B0)
                | P_Fld(0xb, SHU_R1_B0_DQ6_RK1_RX_ARDQM0_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B1_DQ2, P_Fld(0xa, SHU_R0_B1_DQ2_RK0_RX_ARDQ1_R_DLY_B1)
                | P_Fld(0xa, SHU_R0_B1_DQ2_RK0_RX_ARDQ0_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B1_DQ3, P_Fld(0xa, SHU_R0_B1_DQ3_RK0_RX_ARDQ3_R_DLY_B1)
                | P_Fld(0xa, SHU_R0_B1_DQ3_RK0_RX_ARDQ2_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B1_DQ4, P_Fld(0xa, SHU_R0_B1_DQ4_RK0_RX_ARDQ5_R_DLY_B1)
                | P_Fld(0xa, SHU_R0_B1_DQ4_RK0_RX_ARDQ4_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B1_DQ5, P_Fld(0xa, SHU_R0_B1_DQ5_RK0_RX_ARDQ7_R_DLY_B1)
                | P_Fld(0xa, SHU_R0_B1_DQ5_RK0_RX_ARDQ6_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B1_DQ6, P_Fld(0xc, SHU_R0_B1_DQ6_RK0_RX_ARDQS0_R_DLY_B1)
                | P_Fld(0xa, SHU_R0_B1_DQ6_RK0_RX_ARDQM0_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B1_DQ2, P_Fld(0xb, SHU_R1_B1_DQ2_RK1_RX_ARDQ1_R_DLY_B1)
                | P_Fld(0xb, SHU_R1_B1_DQ2_RK1_RX_ARDQ0_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B1_DQ3, P_Fld(0xb, SHU_R1_B1_DQ3_RK1_RX_ARDQ3_R_DLY_B1)
                | P_Fld(0xb, SHU_R1_B1_DQ3_RK1_RX_ARDQ2_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B1_DQ4, P_Fld(0xb, SHU_R1_B1_DQ4_RK1_RX_ARDQ5_R_DLY_B1)
                | P_Fld(0xb, SHU_R1_B1_DQ4_RK1_RX_ARDQ4_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B1_DQ5, P_Fld(0xb, SHU_R1_B1_DQ5_RK1_RX_ARDQ7_R_DLY_B1)
                | P_Fld(0xb, SHU_R1_B1_DQ5_RK1_RX_ARDQ6_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B1_DQ6, P_Fld(0xd, SHU_R1_B1_DQ6_RK1_RX_ARDQS0_R_DLY_B1)
                | P_Fld(0xb, SHU_R1_B1_DQ6_RK1_RX_ARDQM0_R_DLY_B1));
}

static void LegacyRxDly_LP4_DDR3733(DRAMC_CTX_T *p)
{
    U8 u1Dq = 0xf;
    U8 u1Dqm = 0xf;
    U8 u1Dqs = 0x0;

    //RK0_B0
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B0_DQ2, P_Fld(u1Dq, SHU_R0_B0_DQ2_RK0_RX_ARDQ1_R_DLY_B0)
                | P_Fld(u1Dq, SHU_R0_B0_DQ2_RK0_RX_ARDQ0_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B0_DQ3, P_Fld(u1Dq, SHU_R0_B0_DQ3_RK0_RX_ARDQ3_R_DLY_B0)
                | P_Fld(u1Dq, SHU_R0_B0_DQ3_RK0_RX_ARDQ2_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B0_DQ4, P_Fld(u1Dq, SHU_R0_B0_DQ4_RK0_RX_ARDQ5_R_DLY_B0)
                | P_Fld(u1Dq, SHU_R0_B0_DQ4_RK0_RX_ARDQ4_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B0_DQ5, P_Fld(u1Dq, SHU_R0_B0_DQ5_RK0_RX_ARDQ7_R_DLY_B0)
                | P_Fld(u1Dq, SHU_R0_B0_DQ5_RK0_RX_ARDQ6_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B0_DQ6, P_Fld(u1Dqs, SHU_R0_B0_DQ6_RK0_RX_ARDQS0_R_DLY_B0)
                | P_Fld(u1Dqm, SHU_R0_B0_DQ6_RK0_RX_ARDQM0_R_DLY_B0));
    //RK1_B0
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B0_DQ2, P_Fld(u1Dq, SHU_R1_B0_DQ2_RK1_RX_ARDQ1_R_DLY_B0)
                | P_Fld(u1Dq, SHU_R1_B0_DQ2_RK1_RX_ARDQ0_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B0_DQ3, P_Fld(u1Dq, SHU_R1_B0_DQ3_RK1_RX_ARDQ3_R_DLY_B0)
                | P_Fld(u1Dq, SHU_R1_B0_DQ3_RK1_RX_ARDQ2_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B0_DQ4, P_Fld(u1Dq, SHU_R1_B0_DQ4_RK1_RX_ARDQ5_R_DLY_B0)
                | P_Fld(u1Dq, SHU_R1_B0_DQ4_RK1_RX_ARDQ4_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B0_DQ5, P_Fld(u1Dq, SHU_R1_B0_DQ5_RK1_RX_ARDQ7_R_DLY_B0)
                | P_Fld(u1Dq, SHU_R1_B0_DQ5_RK1_RX_ARDQ6_R_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B0_DQ6, P_Fld(u1Dqs, SHU_R1_B0_DQ6_RK1_RX_ARDQS0_R_DLY_B0)
                | P_Fld(u1Dqm, SHU_R1_B0_DQ6_RK1_RX_ARDQM0_R_DLY_B0));
    //RK0_B1
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B1_DQ2, P_Fld(u1Dq, SHU_R0_B1_DQ2_RK0_RX_ARDQ1_R_DLY_B1)
                | P_Fld(u1Dq, SHU_R0_B1_DQ2_RK0_RX_ARDQ0_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B1_DQ3, P_Fld(u1Dq, SHU_R0_B1_DQ3_RK0_RX_ARDQ3_R_DLY_B1)
                | P_Fld(u1Dq, SHU_R0_B1_DQ3_RK0_RX_ARDQ2_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B1_DQ4, P_Fld(u1Dq, SHU_R0_B1_DQ4_RK0_RX_ARDQ5_R_DLY_B1)
                | P_Fld(u1Dq, SHU_R0_B1_DQ4_RK0_RX_ARDQ4_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B1_DQ5, P_Fld(u1Dq, SHU_R0_B1_DQ5_RK0_RX_ARDQ7_R_DLY_B1)
                | P_Fld(u1Dq, SHU_R0_B1_DQ5_RK0_RX_ARDQ6_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B1_DQ6, P_Fld(u1Dqs, SHU_R0_B1_DQ6_RK0_RX_ARDQS0_R_DLY_B1)
                | P_Fld(u1Dqm, SHU_R0_B1_DQ6_RK0_RX_ARDQM0_R_DLY_B1));
    //RK1_B1
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B1_DQ2, P_Fld(u1Dq, SHU_R1_B1_DQ2_RK1_RX_ARDQ1_R_DLY_B1)
                | P_Fld(u1Dq, SHU_R1_B1_DQ2_RK1_RX_ARDQ0_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B1_DQ3, P_Fld(u1Dq, SHU_R1_B1_DQ3_RK1_RX_ARDQ3_R_DLY_B1)
                | P_Fld(u1Dq, SHU_R1_B1_DQ3_RK1_RX_ARDQ2_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B1_DQ4, P_Fld(u1Dq, SHU_R1_B1_DQ4_RK1_RX_ARDQ5_R_DLY_B1)
                | P_Fld(u1Dq, SHU_R1_B1_DQ4_RK1_RX_ARDQ4_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B1_DQ5, P_Fld(u1Dq, SHU_R1_B1_DQ5_RK1_RX_ARDQ7_R_DLY_B1)
                | P_Fld(u1Dq, SHU_R1_B1_DQ5_RK1_RX_ARDQ6_R_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B1_DQ6, P_Fld(u1Dqs, SHU_R1_B1_DQ6_RK1_RX_ARDQS0_R_DLY_B1)
                | P_Fld(u1Dqm, SHU_R1_B1_DQ6_RK1_RX_ARDQM0_R_DLY_B1));
}

#endif //LEGACY_RX_DLY

#if LEGACY_DAT_LAT
static void LegacyDatlatLP4_DDR3733(DRAMC_CTX_T *p)
{
    // set by DramcRxdqsGatingPostProcess
#if 0
    vIO32WriteFldMulti(DRAMC_REG_SHU_RANKCTL, P_Fld(0x7, SHU_RANKCTL_RANKINCTL_PHY)
                | P_Fld(0x5, SHU_RANKCTL_RANKINCTL_ROOT1)
                | P_Fld(0x5, SHU_RANKCTL_RANKINCTL));
#endif

    // DATLAT init by AcTiming Table
    /*vIO32WriteFldMulti(DRAMC_REG_SHU_CONF1, P_Fld(0x11, SHU_CONF1_DATLAT)
                 | P_Fld(0xf, SHU_CONF1_DATLAT_DSEL)
                 | P_Fld(0xf, SHU_CONF1_DATLAT_DSEL_PHY));*/

    //DATLAT 1:8 mode DDR3733
    vIO32WriteFldMulti(DDRPHY_SHU_B0_DQ7, P_Fld(0x0, SHU_B0_DQ7_R_DMRDSEL_DIV2_OPT)
                | P_Fld(0x0, SHU_B0_DQ7_R_DMRDSEL_LOBYTE_OPT)
                | P_Fld(0x0, SHU_B0_DQ7_R_DMRDSEL_HIBYTE_OPT));

    //RDATA PIPE from Ton
    vIO32WriteFldMulti(DRAMC_REG_SHU_PIPE, P_Fld(0x1, SHU_PIPE_READ_START_EXTEND1)
                | P_Fld(0x1, SHU_PIPE_DLE_LAST_EXTEND1)
                | P_Fld(0x1, SHU_PIPE_READ_START_EXTEND2)
                | P_Fld(0x1, SHU_PIPE_DLE_LAST_EXTEND2)
                | P_Fld(0x0, SHU_PIPE_READ_START_EXTEND3)
                | P_Fld(0x0, SHU_PIPE_DLE_LAST_EXTEND3));
}
#endif
#endif  //__A60868_TO_BE_PORTING__

#if __A60868_TO_BE_PORTING__
static void DramcSetting_LP4_TX_Delay_DDR3733(DRAMC_CTX_T *p, DRAM_RANK_T eRank)
{
    U8 ucR0TxdlyOendq = 0, ucR0Txdlydq = 0;
    U8 ucR1TxdlyOendq = 0, ucR1Txdlydq = 0;

    if (eRank == RANK_0)
    {
        if (vGet_Div_Mode(p) == DIV4_MODE)
        {
            ucR0TxdlyOendq = 0x3; // don't use
            ucR0Txdlydq = 0x4; // don't use
        }
        else
        {
            ucR0TxdlyOendq = 0x3;
            ucR0Txdlydq = 0x4;
        }

        if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
        {
            vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ0, P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ3)
                        | P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ2)
                        | P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ1)
                        | P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ0)
                        | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ0_TXDLY_DQ3)
                        | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ0_TXDLY_DQ2)
                        | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ0_TXDLY_DQ1)
                        | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ0_TXDLY_DQ0));
            vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ1, P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM3)
                        | P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM2)
                        | P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM1)
                        | P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM0)
                        | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ1_TXDLY_DQM3)
                        | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ1_TXDLY_DQM2)
                        | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ1_TXDLY_DQM1)
                        | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ1_TXDLY_DQM0));
        }
        else
        {
            vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ0, P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ3)
                        | P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ2)
                        | P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ1)
                        | P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ0)
                        | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ0_TXDLY_DQ3)
                        | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ0_TXDLY_DQ2)
                        | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ0_TXDLY_DQ1)
                        | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ0_TXDLY_DQ0));
            vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ1, P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM3)
                        | P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM2)
                        | P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM1)
                        | P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM0)
                        | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ1_TXDLY_DQM3)
                        | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ1_TXDLY_DQM2)
                        | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ1_TXDLY_DQM1)
                        | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ1_TXDLY_DQM0));
        }
        if (vGet_Div_Mode(p) == DIV4_MODE)
        {
            ucR0TxdlyOendq = 0x2; //don't use
            ucR0Txdlydq = 0x1; //don't use
        }
        else
        {
            ucR0TxdlyOendq = 0x7; // TX OE -3UI
            ucR0Txdlydq = 0x2;
        }
        vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ2, P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ2_DLY_OEN_DQ3)
                    | P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ2_DLY_OEN_DQ2)
                    | P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ2_DLY_OEN_DQ1)
                    | P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ2_DLY_OEN_DQ0)
                    | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ2_DLY_DQ3)
                    | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ2_DLY_DQ2)
                    | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ2_DLY_DQ1)
                    | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ2_DLY_DQ0));
        vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ3, P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ3_DLY_OEN_DQM3)
                    | P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ3_DLY_OEN_DQM2)
                    | P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ3_DLY_OEN_DQM1)
                    | P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ3_DLY_OEN_DQM0)
                    | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ3_DLY_DQM3)
                    | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ3_DLY_DQM2)
                    | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ3_DLY_DQM1)
                    | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ3_DLY_DQM0));
    }
    else //RANK_1
    {
        if (vGet_Div_Mode(p) == DIV4_MODE)
        {
            ucR1TxdlyOendq = 0x3; //don't use
            ucR1Txdlydq = 0x4; //don't use
        }
        else
        {
            ucR1TxdlyOendq = 0x4;
            ucR1Txdlydq = 0x4;
        }
        if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
        {
            vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQ0, P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ3)
                        | P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ2)
                        | P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ1)
                        | P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ0)
                        | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ0_TX_DLY_R1DQ3)
                        | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ0_TX_DLY_R1DQ2)
                        | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ0_TX_DLY_R1DQ1)
                        | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ0_TX_DLY_R1DQ0));
            vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQ1, P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM3)
                        | P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM2)
                        | P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM1)
                        | P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM0)
                        | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ1_TX_DLY_R1DQM3)
                        | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ1_TX_DLY_R1DQM2)
                        | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ1_TX_DLY_R1DQM1)
                        | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ1_TX_DLY_R1DQM0));
        }
        else
        {
            vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQ0, P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ3)
                        | P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ2)
                        | P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ1)
                        | P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ0)
                        | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ0_TX_DLY_R1DQ3)
                        | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ0_TX_DLY_R1DQ2)
                        | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ0_TX_DLY_R1DQ1)
                        | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ0_TX_DLY_R1DQ0));
            vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQ1, P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM3)
                        | P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM2)
                        | P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM1)
                        | P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM0)
                        | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ1_TX_DLY_R1DQM3)
                        | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ1_TX_DLY_R1DQM2)
                        | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ1_TX_DLY_R1DQM1)
                        | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ1_TX_DLY_R1DQM0));
        }
        if (vGet_Div_Mode(p) == DIV4_MODE)
        {
            ucR1TxdlyOendq = 0x3; //don't use
            ucR1Txdlydq = 0x2; //don't use
        }
        else
        {
            ucR1TxdlyOendq = 0x0;
            ucR1Txdlydq = 0x3;
        }
        vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQ2, P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ3)
                    | P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ2)
                    | P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ1)
                    | P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ0)
                    | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ2_DLY_R1DQ3)
                    | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ2_DLY_R1DQ2)
                    | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ2_DLY_R1DQ1)
                    | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ2_DLY_R1DQ0));
        vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQ3, P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM3)
                    | P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM2)
                    | P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM1)
                    | P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM0)
                    | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ3_DLY_R1DQM3)
                    | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ3_DLY_R1DQM2)
                    | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ3_DLY_R1DQM1)
                    | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ3_DLY_R1DQM0));
    }
}

static void DramcSetting_Olympus_LP4_ByteMode_DDR4266(DRAMC_CTX_T *p)
{
    vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_DQS0, P_Fld(0x4, SHU_SELPH_DQS0_TXDLY_OEN_DQS3)
                | P_Fld(0x4, SHU_SELPH_DQS0_TXDLY_OEN_DQS2)
                | P_Fld(0x4, SHU_SELPH_DQS0_TXDLY_OEN_DQS1)
                | P_Fld(0x4, SHU_SELPH_DQS0_TXDLY_OEN_DQS0)
                | P_Fld(0x4, SHU_SELPH_DQS0_TXDLY_DQS3)
                | P_Fld(0x4, SHU_SELPH_DQS0_TXDLY_DQS2)
                | P_Fld(0x4, SHU_SELPH_DQS0_TXDLY_DQS1)
                | P_Fld(0x4, SHU_SELPH_DQS0_TXDLY_DQS0));
    vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_DQS1, P_Fld(0x2, SHU_SELPH_DQS1_DLY_OEN_DQS3)
                | P_Fld(0x2, SHU_SELPH_DQS1_DLY_OEN_DQS2)
                | P_Fld(0x2, SHU_SELPH_DQS1_DLY_OEN_DQS1)
                | P_Fld(0x2, SHU_SELPH_DQS1_DLY_OEN_DQS0)
                | P_Fld(0x5, SHU_SELPH_DQS1_DLY_DQS3)
                | P_Fld(0x5, SHU_SELPH_DQS1_DLY_DQS2)
                | P_Fld(0x5, SHU_SELPH_DQS1_DLY_DQS1)
                | P_Fld(0x5, SHU_SELPH_DQS1_DLY_DQS0));
}

static void DramcSetting_Olympus_LP4_ByteMode_DDR3733(DRAMC_CTX_T *p)
{
    DramcSetting_LP4_TX_Delay_DDR3733(p, RANK_0); // TX DQ/DQM MCK/UI
    DramcSetting_LP4_TX_Delay_DDR3733(p, RANK_1); // TX DQ/DQM MCK/UI
    // TX DQ/DQM PI
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B0_DQ7, P_Fld(0xe, SHU_R0_B0_DQ7_RK0_ARPI_DQM_B0)
                | P_Fld(0x13, SHU_R0_B0_DQ7_RK0_ARPI_DQ_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B1_DQ7, P_Fld(0xe, SHU_R0_B1_DQ7_RK0_ARPI_DQM_B1)
                | P_Fld(0x13, SHU_R0_B1_DQ7_RK0_ARPI_DQ_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B0_DQ7, P_Fld(0x19, SHU_R1_B0_DQ7_RK1_ARPI_DQM_B0)
                | P_Fld(0x22, SHU_R1_B0_DQ7_RK1_ARPI_DQ_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B1_DQ7, P_Fld(0x19, SHU_R1_B1_DQ7_RK1_ARPI_DQM_B1)
                | P_Fld(0x22, SHU_R1_B1_DQ7_RK1_ARPI_DQ_B1));

    //TX DQS MCK by WriteLeveling
    vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_DQS0, P_Fld(0x3, SHU_SELPH_DQS0_TXDLY_OEN_DQS3)
                | P_Fld(0x3, SHU_SELPH_DQS0_TXDLY_OEN_DQS2)
                | P_Fld(0x3, SHU_SELPH_DQS0_TXDLY_OEN_DQS1)
                | P_Fld(0x3, SHU_SELPH_DQS0_TXDLY_OEN_DQS0)
                | P_Fld(0x4, SHU_SELPH_DQS0_TXDLY_DQS3)
                | P_Fld(0x4, SHU_SELPH_DQS0_TXDLY_DQS2)
                | P_Fld(0x4, SHU_SELPH_DQS0_TXDLY_DQS1)
                | P_Fld(0x4, SHU_SELPH_DQS0_TXDLY_DQS0));
    //TX DQS UI by WriteLeveling
    vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_DQS1, P_Fld(0x6, SHU_SELPH_DQS1_DLY_OEN_DQS3)
                | P_Fld(0x6, SHU_SELPH_DQS1_DLY_OEN_DQS2)
                | P_Fld(0x6, SHU_SELPH_DQS1_DLY_OEN_DQS1)
                | P_Fld(0x6, SHU_SELPH_DQS1_DLY_OEN_DQS0)
                | P_Fld(0x1, SHU_SELPH_DQS1_DLY_DQS3)
                | P_Fld(0x1, SHU_SELPH_DQS1_DLY_DQS2)
                | P_Fld(0x1, SHU_SELPH_DQS1_DLY_DQS1)
                | P_Fld(0x1, SHU_SELPH_DQS1_DLY_DQS0));

#if LEGACY_GATING_DLY
    /*vIO32WriteFldMulti(DRAMC_REG_SHU_PHY_RX_CTRL, P_Fld(0x1, SHU_PHY_RX_CTRL_RANK_RXDLY_UPDLAT_EN)
                | P_Fld(0x2, SHU_PHY_RX_CTRL_RANK_RXDLY_UPD_OFFSET)
                | P_Fld(0x2, SHU_PHY_RX_CTRL_RX_IN_GATE_EN_PRE_OFFSET)
                | P_Fld(0x1, SHU_PHY_RX_CTRL_RX_IN_GATE_EN_HEAD)
                | P_Fld(0x0, SHU_PHY_RX_CTRL_RX_IN_GATE_EN_TAIL)
                | P_Fld(0x2, SHU_PHY_RX_CTRL_RX_IN_BUFF_EN_HEAD)
                | P_Fld(0x0, SHU_PHY_RX_CTRL_RX_IN_BUFF_EN_TAIL));*/

    LegacyGatingDlyLP4_DDR3733(p);
#endif

#if LEGACY_RX_DLY
    LegacyRxDly_LP4_DDR3733(p);
#endif


#if LEGACY_DAT_LAT // TODO: Darren
    LegacyDatlatLP4_DDR3733(p);
#endif
}


static void DramcSetting_Olympus_LP4_ByteMode_DDR2667(DRAMC_CTX_T *p)
{
    ////DRAMC0-SHU2
#if LEGACY_TDQSCK_PRECAL
    LegacyPreCalLP4_DDR2667(p);
#endif
    //vIO32WriteFldAlign(DRAMC_REG_SHU_CONF2, 0x54, SHU_CONF2_FSPCHG_PRDCNT);//ACTiming related -> set in UpdateACTiming_Reg()
#if 0
    vIO32WriteFldMulti(DRAMC_REG_SHU_RANKCTL, P_Fld(0x4, SHU_RANKCTL_RANKINCTL_PHY)
                | P_Fld(0x2, SHU_RANKCTL_RANKINCTL_ROOT1)
                | P_Fld(0x2, SHU_RANKCTL_RANKINCTL));
#endif
    vIO32WriteFldMulti(DRAMC_REG_SHU_CKECTRL, P_Fld(0x3, SHU_CKECTRL_TCKESRX));
                //| P_Fld(0x3, SHU_CKECTRL_CKEPRD));
    vIO32WriteFldMulti(DRAMC_REG_SHU_ODTCTRL, P_Fld(0x1, SHU_ODTCTRL_RODTE)
                | P_Fld(0x1, SHU_ODTCTRL_RODTE2)
                //| P_Fld(0x4, SHU_ODTCTRL_RODT) //Set in UpdateACTimingReg()
                | P_Fld(0x1, SHU_ODTCTRL_ROEN));
    vIO32WriteFldAlign(DDRPHY_SHU_B0_DQ7, 0x1, SHU_B0_DQ7_R_DMRODTEN_B0);
    vIO32WriteFldAlign(DDRPHY_SHU_B1_DQ7, 0x1, SHU_B1_DQ7_R_DMRODTEN_B1);
#if LEGACY_TX_TRACK
    LegacyTxTrackLP4_DDR2667(p);
#endif
    vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_DQS0, P_Fld(0x2, SHU_SELPH_DQS0_TXDLY_OEN_DQS3)
                | P_Fld(0x2, SHU_SELPH_DQS0_TXDLY_OEN_DQS2)
                | P_Fld(0x2, SHU_SELPH_DQS0_TXDLY_OEN_DQS1)
                | P_Fld(0x2, SHU_SELPH_DQS0_TXDLY_OEN_DQS0)
                | P_Fld(0x3, SHU_SELPH_DQS0_TXDLY_DQS3)
                | P_Fld(0x3, SHU_SELPH_DQS0_TXDLY_DQS2)
                | P_Fld(0x3, SHU_SELPH_DQS0_TXDLY_DQS1)
                | P_Fld(0x3, SHU_SELPH_DQS0_TXDLY_DQS0));
    vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_DQS1, P_Fld(0x6, SHU_SELPH_DQS1_DLY_OEN_DQS3)
                | P_Fld(0x6, SHU_SELPH_DQS1_DLY_OEN_DQS2)
                | P_Fld(0x6, SHU_SELPH_DQS1_DLY_OEN_DQS1)
                | P_Fld(0x6, SHU_SELPH_DQS1_DLY_OEN_DQS0)
                | P_Fld(0x1, SHU_SELPH_DQS1_DLY_DQS3)
                | P_Fld(0x1, SHU_SELPH_DQS1_DLY_DQS2)
                | P_Fld(0x1, SHU_SELPH_DQS1_DLY_DQS1)
                | P_Fld(0x1, SHU_SELPH_DQS1_DLY_DQS0));

    vIO32WriteFldAlign(DRAMC_REG_SHU_DQS2DQ_TX, 0x7, SHU_DQS2DQ_TX_OE2DQ_OFFSET);
    //vIO32WriteFldAlign(DRAMC_REG_SHU_HWSET_MR2, 0x24, SHU_HWSET_MR2_HWSET_MR2_OP);
    //vIO32WriteFldAlign(DRAMC_REG_SHU_HWSET_MR13, 0xc8, SHU_HWSET_MR13_HWSET_MR13_OP);
    //vIO32WriteFldAlign(DRAMC_REG_SHU_HWSET_VRCG, 0xc0, SHU_HWSET_VRCG_HWSET_VRCG_OP);
    vIO32WriteFldMulti(DRAMC_REG_SHURK0_DQSIEN, P_Fld(0x19, SHURK0_DQSIEN_R0DQS1IEN)
                | P_Fld(0x19, SHURK0_DQSIEN_R0DQS0IEN));
    vIO32WriteFldMulti(DRAMC_REG_SHURK0_PI, P_Fld(0x14, SHURK0_PI_RK0_ARPI_DQM_B1)
                | P_Fld(0x14, SHURK0_PI_RK0_ARPI_DQM_B0)
                | P_Fld(0x14, SHURK0_PI_RK0_ARPI_DQ_B1)
                | P_Fld(0x14, SHURK0_PI_RK0_ARPI_DQ_B0));

    #if LEGACY_GATING_DLY
    LegacyGatingDlyLP4_DDR2667(p);
    #endif

    if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
    {
        vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ0, P_Fld(0x2, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ3)
                    | P_Fld(0x2, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ2)
                    | P_Fld(0x2, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ1)
                    | P_Fld(0x2, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ0)
                    | P_Fld(0x3, SHURK0_SELPH_DQ0_TXDLY_DQ3)
                    | P_Fld(0x3, SHURK0_SELPH_DQ0_TXDLY_DQ2)
                    | P_Fld(0x3, SHURK0_SELPH_DQ0_TXDLY_DQ1)
                    | P_Fld(0x3, SHURK0_SELPH_DQ0_TXDLY_DQ0));
        vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ1, P_Fld(0x2, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM3)
                    | P_Fld(0x2, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM2)
                    | P_Fld(0x2, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM1)
                    | P_Fld(0x2, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM0)
                    | P_Fld(0x3, SHURK0_SELPH_DQ1_TXDLY_DQM3)
                    | P_Fld(0x3, SHURK0_SELPH_DQ1_TXDLY_DQM2)
                    | P_Fld(0x3, SHURK0_SELPH_DQ1_TXDLY_DQM1)
                    | P_Fld(0x3, SHURK0_SELPH_DQ1_TXDLY_DQM0));
    }
    else
    {
        vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ0, P_Fld(0x2, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ3)
                    | P_Fld(0x2, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ2)
                    | P_Fld(0x2, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ1)
                    | P_Fld(0x2, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ0)
                    | P_Fld(0x3, SHURK0_SELPH_DQ0_TXDLY_DQ3)
                    | P_Fld(0x3, SHURK0_SELPH_DQ0_TXDLY_DQ2)
                    | P_Fld(0x3, SHURK0_SELPH_DQ0_TXDLY_DQ1)
                    | P_Fld(0x3, SHURK0_SELPH_DQ0_TXDLY_DQ0));
        vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ1, P_Fld(0x2, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM3)
                    | P_Fld(0x2, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM2)
                    | P_Fld(0x2, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM1)
                    | P_Fld(0x2, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM0)
                    | P_Fld(0x3, SHURK0_SELPH_DQ1_TXDLY_DQM3)
                    | P_Fld(0x3, SHURK0_SELPH_DQ1_TXDLY_DQM2)
                    | P_Fld(0x3, SHURK0_SELPH_DQ1_TXDLY_DQM1)
                    | P_Fld(0x3, SHURK0_SELPH_DQ1_TXDLY_DQM0));
    }
    vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ2, P_Fld(0x6, SHURK0_SELPH_DQ2_DLY_OEN_DQ3)
                | P_Fld(0x6, SHURK0_SELPH_DQ2_DLY_OEN_DQ2)
                | P_Fld(0x6, SHURK0_SELPH_DQ2_DLY_OEN_DQ1)
                | P_Fld(0x6, SHURK0_SELPH_DQ2_DLY_OEN_DQ0)
                | P_Fld(0x1, SHURK0_SELPH_DQ2_DLY_DQ3)
                | P_Fld(0x1, SHURK0_SELPH_DQ2_DLY_DQ2)
                | P_Fld(0x1, SHURK0_SELPH_DQ2_DLY_DQ1)
                | P_Fld(0x1, SHURK0_SELPH_DQ2_DLY_DQ0));
    vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ3, P_Fld(0x6, SHURK0_SELPH_DQ3_DLY_OEN_DQM3)
                | P_Fld(0x6, SHURK0_SELPH_DQ3_DLY_OEN_DQM2)
                | P_Fld(0x6, SHURK0_SELPH_DQ3_DLY_OEN_DQM1)
                | P_Fld(0x6, SHURK0_SELPH_DQ3_DLY_OEN_DQM0)
                | P_Fld(0x1, SHURK0_SELPH_DQ3_DLY_DQM3)
                | P_Fld(0x1, SHURK0_SELPH_DQ3_DLY_DQM2)
                | P_Fld(0x1, SHURK0_SELPH_DQ3_DLY_DQM1)
                | P_Fld(0x1, SHURK0_SELPH_DQ3_DLY_DQM0));
    //vIO32WriteFldAlign(DRAMC_REG_SHURK1_DQSCTL, 0x4, SHURK1_DQSCTL_R1DQSINCTL); //Set in UpdateACTimingReg()
    vIO32WriteFldMulti(DRAMC_REG_SHURK1_DQSIEN, P_Fld(0x1f, SHURK1_DQSIEN_R1DQS1IEN)
                | P_Fld(0x1f, SHURK1_DQSIEN_R1DQS0IEN));
    vIO32WriteFldMulti(DRAMC_REG_SHURK1_PI, P_Fld(0x14, SHURK1_PI_RK1_ARPI_DQM_B1)
                | P_Fld(0x14, SHURK1_PI_RK1_ARPI_DQM_B0)
                | P_Fld(0x14, SHURK1_PI_RK1_ARPI_DQ_B1)
                | P_Fld(0x14, SHURK1_PI_RK1_ARPI_DQ_B0));
    if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
    {
        vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQ0, P_Fld(0x2, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ3)
                    | P_Fld(0x2, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ2)
                    | P_Fld(0x2, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ1)
                    | P_Fld(0x2, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ0)
                    | P_Fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1DQ3)
                    | P_Fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1DQ2)
                    | P_Fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1DQ1)
                    | P_Fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1DQ0));
        vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQ1, P_Fld(0x2, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM3)
                    | P_Fld(0x2, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM2)
                    | P_Fld(0x2, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM1)
                    | P_Fld(0x2, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM0)
                    | P_Fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1DQM3)
                    | P_Fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1DQM2)
                    | P_Fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1DQM1)
                    | P_Fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1DQM0));
    }
    else
    {
        vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQ0, P_Fld(0x2, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ3)
                    | P_Fld(0x2, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ2)
                    | P_Fld(0x2, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ1)
                    | P_Fld(0x2, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ0)
                    | P_Fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1DQ3)
                    | P_Fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1DQ2)
                    | P_Fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1DQ1)
                    | P_Fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1DQ0));
        vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQ1, P_Fld(0x2, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM3)
                    | P_Fld(0x2, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM2)
                    | P_Fld(0x2, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM1)
                    | P_Fld(0x2, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM0)
                    | P_Fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1DQM3)
                    | P_Fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1DQM2)
                    | P_Fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1DQM1)
                    | P_Fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1DQM0));
    }
    vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQ2, P_Fld(0x7, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ3)
                | P_Fld(0x7, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ2)
                | P_Fld(0x7, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ1)
                | P_Fld(0x7, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ0)
                | P_Fld(0x2, SHURK1_SELPH_DQ2_DLY_R1DQ3)
                | P_Fld(0x2, SHURK1_SELPH_DQ2_DLY_R1DQ2)
                | P_Fld(0x2, SHURK1_SELPH_DQ2_DLY_R1DQ1)
                | P_Fld(0x2, SHURK1_SELPH_DQ2_DLY_R1DQ0));
    vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQ3, P_Fld(0x7, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM3)
                | P_Fld(0x7, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM2)
                | P_Fld(0x7, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM1)
                | P_Fld(0x7, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM0)
                | P_Fld(0x2, SHURK1_SELPH_DQ3_DLY_R1DQM3)
                | P_Fld(0x2, SHURK1_SELPH_DQ3_DLY_R1DQM2)
                | P_Fld(0x2, SHURK1_SELPH_DQ3_DLY_R1DQM1)
                | P_Fld(0x2, SHURK1_SELPH_DQ3_DLY_R1DQM0));
    vIO32WriteFldMulti(DRAMC_REG_SHU_DQSG_RETRY, P_Fld(0x4, SHU_DQSG_RETRY_R_DQSIENLAT)
                | P_Fld(0x1, SHU_DQSG_RETRY_R_DDR1866_PLUS));

    ////DDRPHY0-SHU2
    vIO32WriteFldAlign(DDRPHY_SHU_B0_DQ5, 0x3, SHU_B0_DQ5_RG_RX_ARDQS0_DVS_DLY_B0);
    //vIO32WriteFldMulti(DDRPHY_SHU_B0_DQ6, P_Fld(0x0, SHU_B0_DQ6_RG_ARPI_MIDPI_CKDIV4_EN_B0)
    //            | P_Fld(0x1, SHU_B0_DQ6_RG_ARPI_MIDPI_EN_B0));
    if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
    {
        vIO32WriteFldMulti(DDRPHY_SHU_B0_DQ7, P_Fld(0x0, SHU_B0_DQ7_MIDPI_DIV4_ENABLE)
                    | P_Fld(0x1, SHU_B0_DQ7_MIDPI_ENABLE));
    }
    else
    {
        vIO32WriteFldMulti(DDRPHY_SHU_B0_DQ7, P_Fld(0x1, SHU_B0_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B0)
                    | P_Fld(0x1, SHU_B0_DQ7_R_DMDQMDBI_SHU_B0)
                    | P_Fld(0x0, SHU_B0_DQ7_MIDPI_DIV4_ENABLE)
                    | P_Fld(0x1, SHU_B0_DQ7_MIDPI_ENABLE));
    }
    vIO32WriteFldAlign(DDRPHY_SHU_B1_DQ5, 0x3, SHU_B1_DQ5_RG_RX_ARDQS0_DVS_DLY_B1);
    //vIO32WriteFldMulti(DDRPHY_SHU_B1_DQ6, P_Fld(0x0, SHU_B1_DQ6_RG_ARPI_MIDPI_CKDIV4_EN_B1)
    //            | P_Fld(0x1, SHU_B1_DQ6_RG_ARPI_MIDPI_EN_B1));
    if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
    {
    }
    else
    {
        vIO32WriteFldMulti(DDRPHY_SHU_B1_DQ7, P_Fld(0x1, SHU_B1_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B1)
                    | P_Fld(0x1, SHU_B1_DQ7_R_DMDQMDBI_SHU_B1));
    }
    //DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
    //vIO32WriteFldMulti(DDRPHY_SHU_CA_CMD6, P_Fld(0x0, SHU_CA_CMD6_RG_ARPI_MIDPI_CKDIV4_EN_CA)
    //            | P_Fld(0x1, SHU_CA_CMD6_RG_ARPI_MIDPI_EN_CA));
    //vIO32WriteFldMulti(DDRPHY_SHU_CA_CMD6+SHIFT_TO_CHB_ADDR, P_Fld(0x0, SHU_CA_CMD6_RG_ARPI_MIDPI_CKDIV4_EN_CA)
    //            | P_Fld(0x1, SHU_CA_CMD6_RG_ARPI_MIDPI_EN_CA));
    //DramcBroadcastOnOff(DRAMC_BROADCAST_ON);
    //vIO32WriteFldAlign(DDRPHY_SHU_PLL5, 0x3300, SHU_PLL5_RG_RPHYPLL_SDM_PCW);
    //vIO32WriteFldAlign(DDRPHY_SHU_PLL7, 0x3300, SHU_PLL7_RG_RCLRPLL_SDM_PCW);
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B0_DQ7, P_Fld(0x1e, SHU_R0_B0_DQ7_RK0_ARPI_DQM_B0)
                | P_Fld(0x1e, SHU_R0_B0_DQ7_RK0_ARPI_DQ_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B1_DQ7, P_Fld(0x1e, SHU_R0_B1_DQ7_RK0_ARPI_DQM_B1)
                | P_Fld(0x1e, SHU_R0_B1_DQ7_RK0_ARPI_DQ_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B0_DQ7, P_Fld(0x1e, SHU_R1_B0_DQ7_RK1_ARPI_DQM_B0)
                | P_Fld(0x1e, SHU_R1_B0_DQ7_RK1_ARPI_DQ_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B1_DQ7, P_Fld(0x1e, SHU_R1_B1_DQ7_RK1_ARPI_DQM_B1)
                | P_Fld(0x1e, SHU_R1_B1_DQ7_RK1_ARPI_DQ_B1));
#if LEGACY_RX_DLY
    LegacyRxDly_LP4_DDR2667(p);
#endif
#if LEGACY_DELAY_CELL
    LegacyDlyCellInitLP4_DDR2667(p);
#endif

}

static void DramcSetting_LP4_TX_Delay_DDR1600(DRAMC_CTX_T *p, DRAM_RANK_T eRank)
{
    U8 ucR0TxdlyOendq = 0, ucR0Txdlydq = 0;
    U8 ucR1TxdlyOendq = 0, ucR1Txdlydq = 0;
    U8 u1R0B0Pi = 0, u1R0B1Pi = 0;
    U8 u1R1B0Pi = 0, u1R1B1Pi = 0;

    if (eRank == RANK_0)
    {
        if (vGet_Div_Mode(p) == DIV4_MODE)
        {
            ucR0TxdlyOendq = 0x3;
            ucR0Txdlydq = 0x4;
        }
        else
        {
            if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
            {
                ucR0TxdlyOendq = 0x1;
                ucR0Txdlydq = 0x2;
            }
            else
            {
                ucR0TxdlyOendq = 0x1;
                ucR0Txdlydq = 0x2;
            }
        }

        if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
        {
            vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ0, P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ3)
                        | P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ2)
                        | P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ1)
                        | P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ0)
                        | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ0_TXDLY_DQ3)
                        | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ0_TXDLY_DQ2)
                        | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ0_TXDLY_DQ1)
                        | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ0_TXDLY_DQ0));
            vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ1, P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM3)
                        | P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM2)
                        | P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM1)
                        | P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM0)
                        | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ1_TXDLY_DQM3)
                        | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ1_TXDLY_DQM2)
                        | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ1_TXDLY_DQM1)
                        | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ1_TXDLY_DQM0));
        }
        else
        {
            vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ0, P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ3)
                        | P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ2)
                        | P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ1)
                        | P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ0)
                        | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ0_TXDLY_DQ3)
                        | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ0_TXDLY_DQ2)
                        | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ0_TXDLY_DQ1)
                        | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ0_TXDLY_DQ0));
            vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ1, P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM3)
                        | P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM2)
                        | P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM1)
                        | P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM0)
                        | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ1_TXDLY_DQM3)
                        | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ1_TXDLY_DQM2)
                        | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ1_TXDLY_DQM1)
                        | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ1_TXDLY_DQM0));
        }
        if (vGet_Div_Mode(p) == DIV4_MODE)
        {
            ucR0TxdlyOendq = 0x2;
            ucR0Txdlydq = 0x1;
        }
        else
        {
            if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
            {
                ucR0TxdlyOendq = 0x6;
                ucR0Txdlydq = 0x1;
            }
            else
            {
                ucR0TxdlyOendq = 0x7;
                ucR0Txdlydq = 0x1;
            }
        }
        vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ2, P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ2_DLY_OEN_DQ3)
                    | P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ2_DLY_OEN_DQ2)
                    | P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ2_DLY_OEN_DQ1)
                    | P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ2_DLY_OEN_DQ0)
                    | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ2_DLY_DQ3)
                    | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ2_DLY_DQ2)
                    | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ2_DLY_DQ1)
                    | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ2_DLY_DQ0));
        vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ3, P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ3_DLY_OEN_DQM3)
                    | P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ3_DLY_OEN_DQM2)
                    | P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ3_DLY_OEN_DQM1)
                    | P_Fld(ucR0TxdlyOendq, SHURK0_SELPH_DQ3_DLY_OEN_DQM0)
                    | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ3_DLY_DQM3)
                    | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ3_DLY_DQM2)
                    | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ3_DLY_DQM1)
                    | P_Fld(ucR0Txdlydq, SHURK0_SELPH_DQ3_DLY_DQM0));
    }
    else //RANK_1
    {
        if (vGet_Div_Mode(p) == DIV4_MODE)
        {
            ucR1TxdlyOendq = 0x3;
            ucR1Txdlydq = 0x4;
        }
        else
        {
            if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
            {
                ucR1TxdlyOendq = 0x1;
                ucR1Txdlydq = 0x2;
            }
            else
            {
                ucR1TxdlyOendq = 0x1;
                ucR1Txdlydq = 0x2;
            }
        }
        if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
        {
            vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQ0, P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ3)
                        | P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ2)
                        | P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ1)
                        | P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ0)
                        | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ0_TX_DLY_R1DQ3)
                        | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ0_TX_DLY_R1DQ2)
                        | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ0_TX_DLY_R1DQ1)
                        | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ0_TX_DLY_R1DQ0));
            vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQ1, P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM3)
                        | P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM2)
                        | P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM1)
                        | P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM0)
                        | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ1_TX_DLY_R1DQM3)
                        | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ1_TX_DLY_R1DQM2)
                        | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ1_TX_DLY_R1DQM1)
                        | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ1_TX_DLY_R1DQM0));
        }
        else
        {
            vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQ0, P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ3)
                        | P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ2)
                        | P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ1)
                        | P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ0)
                        | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ0_TX_DLY_R1DQ3)
                        | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ0_TX_DLY_R1DQ2)
                        | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ0_TX_DLY_R1DQ1)
                        | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ0_TX_DLY_R1DQ0));
            vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQ1, P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM3)
                        | P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM2)
                        | P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM1)
                        | P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM0)
                        | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ1_TX_DLY_R1DQM3)
                        | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ1_TX_DLY_R1DQM2)
                        | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ1_TX_DLY_R1DQM1)
                        | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ1_TX_DLY_R1DQM0));
        }
        if (vGet_Div_Mode(p) == DIV4_MODE)
        {
            ucR1TxdlyOendq = 0x3;
            ucR1Txdlydq = 0x2;
        }
        else
        {
            if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
            {
                ucR1TxdlyOendq = 0x7;
                ucR1Txdlydq = 0x2;
            }
            else
            {
                ucR1TxdlyOendq = 0x7;
                ucR1Txdlydq = 0x1;
            }
        }
        vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQ2, P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ3)
                    | P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ2)
                    | P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ1)
                    | P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ0)
                    | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ2_DLY_R1DQ3)
                    | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ2_DLY_R1DQ2)
                    | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ2_DLY_R1DQ1)
                    | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ2_DLY_R1DQ0));
        vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQ3, P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM3)
                    | P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM2)
                    | P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM1)
                    | P_Fld(ucR1TxdlyOendq, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM0)
                    | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ3_DLY_R1DQM3)
                    | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ3_DLY_R1DQM2)
                    | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ3_DLY_R1DQM1)
                    | P_Fld(ucR1Txdlydq, SHURK1_SELPH_DQ3_DLY_R1DQM0));
    }

    if (eRank == RANK_0)
    {
        if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
        {
            // Byte mode For RK0 Tx PI
            u1R0B0Pi = 0x1e;
            u1R0B1Pi = 0x1e;
        }
        else
        {
            // For RK0 Tx PI
            u1R0B0Pi = 0x1a;
            u1R0B1Pi = 0x1a;
        }
        vIO32WriteFldMulti(DDRPHY_SHU_R0_B0_DQ7, P_Fld(u1R0B0Pi, SHU_R0_B0_DQ7_RK0_ARPI_DQM_B0)
                    | P_Fld(u1R0B0Pi, SHU_R0_B0_DQ7_RK0_ARPI_DQ_B0));
        vIO32WriteFldMulti(DDRPHY_SHU_R0_B1_DQ7, P_Fld(u1R0B1Pi, SHU_R0_B1_DQ7_RK0_ARPI_DQM_B1)
                    | P_Fld(u1R0B1Pi, SHU_R0_B1_DQ7_RK0_ARPI_DQ_B1));
    }
    else
    {
        // For RK1 Tx PI
        if (vGet_Div_Mode(p) == DIV4_MODE)
        {
            // for DDR1600 1:4 mode
            u1R1B0Pi = 0x1b;
            u1R1B1Pi = 0x1b;
        }
        else
        {
            if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
            {
                // Byte mode for DDR1600 1:8 mode
                u1R1B0Pi = 0x13;
                u1R1B1Pi = 0x12;
            }
            else
            {
                // for DDR1600 1:8 mode
                u1R1B0Pi = 0x26;
                u1R1B1Pi = 0x26;
            }
        }
        vIO32WriteFldMulti(DDRPHY_SHU_R1_B0_DQ7, P_Fld(u1R1B0Pi, SHU_R1_B0_DQ7_RK1_ARPI_DQM_B0)
                    | P_Fld(u1R1B0Pi, SHU_R1_B0_DQ7_RK1_ARPI_DQ_B0));
        vIO32WriteFldMulti(DDRPHY_SHU_R1_B1_DQ7, P_Fld(u1R1B1Pi, SHU_R1_B1_DQ7_RK1_ARPI_DQM_B1)
                    | P_Fld(u1R1B1Pi, SHU_R1_B1_DQ7_RK1_ARPI_DQ_B1));

    }
}

static void DramcSetting_LP4_TX_Delay_DDR800(DRAMC_CTX_T *p, DRAM_RANK_T eRank)
{
    if (eRank == RANK_0)
    {
        if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
        {
            vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ0, P_Fld(0x3, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ3)
                        | P_Fld(0x3, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ2)
                        | P_Fld(0x3, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ1)
                        | P_Fld(0x3, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ0)
                        | P_Fld(0x4, SHURK0_SELPH_DQ0_TXDLY_DQ3)
                        | P_Fld(0x4, SHURK0_SELPH_DQ0_TXDLY_DQ2)
                        | P_Fld(0x4, SHURK0_SELPH_DQ0_TXDLY_DQ1)
                        | P_Fld(0x4, SHURK0_SELPH_DQ0_TXDLY_DQ0));
            vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ1, P_Fld(0x3, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM3)
                        | P_Fld(0x3, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM2)
                        | P_Fld(0x3, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM1)
                        | P_Fld(0x3, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM0)
                        | P_Fld(0x4, SHURK0_SELPH_DQ1_TXDLY_DQM3)
                        | P_Fld(0x4, SHURK0_SELPH_DQ1_TXDLY_DQM2)
                        | P_Fld(0x4, SHURK0_SELPH_DQ1_TXDLY_DQM1)
                        | P_Fld(0x4, SHURK0_SELPH_DQ1_TXDLY_DQM0));
        }
        else
        {
            vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ0, P_Fld(0x3, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ3)
                        | P_Fld(0x3, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ2)
                        | P_Fld(0x3, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ1)
                        | P_Fld(0x3, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ0)
                        | P_Fld(0x4, SHURK0_SELPH_DQ0_TXDLY_DQ3)
                        | P_Fld(0x4, SHURK0_SELPH_DQ0_TXDLY_DQ2)
                        | P_Fld(0x4, SHURK0_SELPH_DQ0_TXDLY_DQ1)
                        | P_Fld(0x4, SHURK0_SELPH_DQ0_TXDLY_DQ0));
            vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ1, P_Fld(0x3, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM3)
                        | P_Fld(0x3, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM2)
                        | P_Fld(0x3, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM1)
                        | P_Fld(0x3, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM0)
                        | P_Fld(0x4, SHURK0_SELPH_DQ1_TXDLY_DQM3)
                        | P_Fld(0x4, SHURK0_SELPH_DQ1_TXDLY_DQM2)
                        | P_Fld(0x4, SHURK0_SELPH_DQ1_TXDLY_DQM1)
                        | P_Fld(0x4, SHURK0_SELPH_DQ1_TXDLY_DQM0));
        }

        if (vGet_DDR800_Mode(p) == DDR800_CLOSE_LOOP)
        {
            // TX OE -3*UI
            vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ2, P_Fld(0x2, SHURK0_SELPH_DQ2_DLY_OEN_DQ3)
                        | P_Fld(0x2, SHURK0_SELPH_DQ2_DLY_OEN_DQ2)
                        | P_Fld(0x2, SHURK0_SELPH_DQ2_DLY_OEN_DQ1)
                        | P_Fld(0x2, SHURK0_SELPH_DQ2_DLY_OEN_DQ0)
                        | P_Fld(0x1, SHURK0_SELPH_DQ2_DLY_DQ3)
                        | P_Fld(0x1, SHURK0_SELPH_DQ2_DLY_DQ2)
                        | P_Fld(0x1, SHURK0_SELPH_DQ2_DLY_DQ1)
                        | P_Fld(0x1, SHURK0_SELPH_DQ2_DLY_DQ0));
            vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ3, P_Fld(0x2, SHURK0_SELPH_DQ3_DLY_OEN_DQM3)
                        | P_Fld(0x2, SHURK0_SELPH_DQ3_DLY_OEN_DQM2)
                        | P_Fld(0x2, SHURK0_SELPH_DQ3_DLY_OEN_DQM1)
                        | P_Fld(0x2, SHURK0_SELPH_DQ3_DLY_OEN_DQM0)
                        | P_Fld(0x1, SHURK0_SELPH_DQ3_DLY_DQM3)
                        | P_Fld(0x1, SHURK0_SELPH_DQ3_DLY_DQM2)
                        | P_Fld(0x1, SHURK0_SELPH_DQ3_DLY_DQM1)
                        | P_Fld(0x1, SHURK0_SELPH_DQ3_DLY_DQM0));
        }
        else //if (vGet_DDR800_Mode(p) == DDR800_OPEN_LOOP)
        {
            // DDR800_OPEN_LOOP and DDR800_SEMI_LOOP
            vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ2, P_Fld(0x3, SHURK0_SELPH_DQ2_DLY_OEN_DQ3)
                        | P_Fld(0x3, SHURK0_SELPH_DQ2_DLY_OEN_DQ2)
                        | P_Fld(0x3, SHURK0_SELPH_DQ2_DLY_OEN_DQ1)
                        | P_Fld(0x3, SHURK0_SELPH_DQ2_DLY_OEN_DQ0)
                        | P_Fld(0x2, SHURK0_SELPH_DQ2_DLY_DQ3)
                        | P_Fld(0x2, SHURK0_SELPH_DQ2_DLY_DQ2)
                        | P_Fld(0x2, SHURK0_SELPH_DQ2_DLY_DQ1)
                        | P_Fld(0x2, SHURK0_SELPH_DQ2_DLY_DQ0));
            vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ3, P_Fld(0x3, SHURK0_SELPH_DQ3_DLY_OEN_DQM3)
                        | P_Fld(0x3, SHURK0_SELPH_DQ3_DLY_OEN_DQM2)
                        | P_Fld(0x3, SHURK0_SELPH_DQ3_DLY_OEN_DQM1)
                        | P_Fld(0x3, SHURK0_SELPH_DQ3_DLY_OEN_DQM0)
                        | P_Fld(0x2, SHURK0_SELPH_DQ3_DLY_DQM3)
                        | P_Fld(0x2, SHURK0_SELPH_DQ3_DLY_DQM2)
                        | P_Fld(0x2, SHURK0_SELPH_DQ3_DLY_DQM1)
                        | P_Fld(0x2, SHURK0_SELPH_DQ3_DLY_DQM0));
        }
    }
    else
    {
        if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
        {
            vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQ0, P_Fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ3)
                        | P_Fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ2)
                        | P_Fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ1)
                        | P_Fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ0)
                        | P_Fld(0x4, SHURK1_SELPH_DQ0_TX_DLY_R1DQ3)
                        | P_Fld(0x4, SHURK1_SELPH_DQ0_TX_DLY_R1DQ2)
                        | P_Fld(0x4, SHURK1_SELPH_DQ0_TX_DLY_R1DQ1)
                        | P_Fld(0x4, SHURK1_SELPH_DQ0_TX_DLY_R1DQ0));
            vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQ1, P_Fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM3)
                        | P_Fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM2)
                        | P_Fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM1)
                        | P_Fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM0)
                        | P_Fld(0x4, SHURK1_SELPH_DQ1_TX_DLY_R1DQM3)
                        | P_Fld(0x4, SHURK1_SELPH_DQ1_TX_DLY_R1DQM2)
                        | P_Fld(0x4, SHURK1_SELPH_DQ1_TX_DLY_R1DQM1)
                        | P_Fld(0x4, SHURK1_SELPH_DQ1_TX_DLY_R1DQM0));
        }
        else
        {
            vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQ0, P_Fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ3)
                        | P_Fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ2)
                        | P_Fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ1)
                        | P_Fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ0)
                        | P_Fld(0x4, SHURK1_SELPH_DQ0_TX_DLY_R1DQ3)
                        | P_Fld(0x4, SHURK1_SELPH_DQ0_TX_DLY_R1DQ2)
                        | P_Fld(0x4, SHURK1_SELPH_DQ0_TX_DLY_R1DQ1)
                        | P_Fld(0x4, SHURK1_SELPH_DQ0_TX_DLY_R1DQ0));
            vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQ1, P_Fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM3)
                        | P_Fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM2)
                        | P_Fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM1)
                        | P_Fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM0)
                        | P_Fld(0x4, SHURK1_SELPH_DQ1_TX_DLY_R1DQM3)
                        | P_Fld(0x4, SHURK1_SELPH_DQ1_TX_DLY_R1DQM2)
                        | P_Fld(0x4, SHURK1_SELPH_DQ1_TX_DLY_R1DQM1)
                        | P_Fld(0x4, SHURK1_SELPH_DQ1_TX_DLY_R1DQM0));
        }

        if (vGet_DDR800_Mode(p) == DDR800_CLOSE_LOOP)
        {
            // TX OE -3*UI
            vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQ2, P_Fld(0x2, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ3)
                        | P_Fld(0x2, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ2)
                        | P_Fld(0x2, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ1)
                        | P_Fld(0x2, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ0)
                        | P_Fld(0x1, SHURK1_SELPH_DQ2_DLY_R1DQ3)
                        | P_Fld(0x1, SHURK1_SELPH_DQ2_DLY_R1DQ2)
                        | P_Fld(0x1, SHURK1_SELPH_DQ2_DLY_R1DQ1)
                        | P_Fld(0x1, SHURK1_SELPH_DQ2_DLY_R1DQ0));
            vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQ3, P_Fld(0x2, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM3)
                        | P_Fld(0x2, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM2)
                        | P_Fld(0x2, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM1)
                        | P_Fld(0x2, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM0)
                        | P_Fld(0x1, SHURK1_SELPH_DQ3_DLY_R1DQM3)
                        | P_Fld(0x1, SHURK1_SELPH_DQ3_DLY_R1DQM2)
                        | P_Fld(0x1, SHURK1_SELPH_DQ3_DLY_R1DQM1)
                        | P_Fld(0x1, SHURK1_SELPH_DQ3_DLY_R1DQM0));
        }
        else //if (vGet_DDR800_Mode(p) == DDR800_OPEN_LOOP)
        {
            // DDR800_OPEN_LOOP and DDR800_SEMI_LOOP
            vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQ2, P_Fld(0x3, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ3)
                        | P_Fld(0x3, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ2)
                        | P_Fld(0x3, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ1)
                        | P_Fld(0x3, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ0)
                        | P_Fld(0x2, SHURK1_SELPH_DQ2_DLY_R1DQ3)
                        | P_Fld(0x2, SHURK1_SELPH_DQ2_DLY_R1DQ2)
                        | P_Fld(0x2, SHURK1_SELPH_DQ2_DLY_R1DQ1)
                        | P_Fld(0x2, SHURK1_SELPH_DQ2_DLY_R1DQ0));
            vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQ3, P_Fld(0x3, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM3)
                        | P_Fld(0x3, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM2)
                        | P_Fld(0x3, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM1)
                        | P_Fld(0x3, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM0)
                        | P_Fld(0x2, SHURK1_SELPH_DQ3_DLY_R1DQM3)
                        | P_Fld(0x2, SHURK1_SELPH_DQ3_DLY_R1DQM2)
                        | P_Fld(0x2, SHURK1_SELPH_DQ3_DLY_R1DQM1)
                        | P_Fld(0x2, SHURK1_SELPH_DQ3_DLY_R1DQM0));
        }
    }

    if (vGet_DDR800_Mode(p) == DDR800_CLOSE_LOOP)
    {
        // DDR800 1:4 mode TX PI
        vIO32WriteFldMulti(DDRPHY_SHU_R0_B0_DQ7, P_Fld(0x1a, SHU_R0_B0_DQ7_RK0_ARPI_DQM_B0)
                    | P_Fld(0x1a, SHU_R0_B0_DQ7_RK0_ARPI_DQ_B0));
        vIO32WriteFldMulti(DDRPHY_SHU_R0_B1_DQ7, P_Fld(0x1a, SHU_R0_B1_DQ7_RK0_ARPI_DQM_B1)
                    | P_Fld(0x1a, SHU_R0_B1_DQ7_RK0_ARPI_DQ_B1));
        vIO32WriteFldMulti(DDRPHY_SHU_R1_B0_DQ7, P_Fld(0x25, SHU_R1_B0_DQ7_RK1_ARPI_DQM_B0)
                    | P_Fld(0x25, SHU_R1_B0_DQ7_RK1_ARPI_DQ_B0));
        vIO32WriteFldMulti(DDRPHY_SHU_R1_B1_DQ7, P_Fld(0x25, SHU_R1_B1_DQ7_RK1_ARPI_DQM_B1)
                    | P_Fld(0x25, SHU_R1_B1_DQ7_RK1_ARPI_DQ_B1));
    }
    else
    {
        // DDR800_SEMI_LOOP and DDR800_OPEN_LOOP
        // DDR800 1:4 mode TX PI
        vIO32WriteFldMulti(DDRPHY_SHU_R0_B0_DQ7, P_Fld(0x0, SHU_R0_B0_DQ7_RK0_ARPI_DQM_B0)
                    | P_Fld(0x0, SHU_R0_B0_DQ7_RK0_ARPI_DQ_B0));
        vIO32WriteFldMulti(DDRPHY_SHU_R0_B1_DQ7, P_Fld(0x0, SHU_R0_B1_DQ7_RK0_ARPI_DQM_B1)
                    | P_Fld(0x0, SHU_R0_B1_DQ7_RK0_ARPI_DQ_B1));
        vIO32WriteFldMulti(DDRPHY_SHU_R1_B0_DQ7, P_Fld(0x0, SHU_R1_B0_DQ7_RK1_ARPI_DQM_B0)
                    | P_Fld(0x0, SHU_R1_B0_DQ7_RK1_ARPI_DQ_B0));
        vIO32WriteFldMulti(DDRPHY_SHU_R1_B1_DQ7, P_Fld(0x0, SHU_R1_B1_DQ7_RK1_ARPI_DQM_B1)
                    | P_Fld(0x0, SHU_R1_B1_DQ7_RK1_ARPI_DQ_B1));
    }
}

static void DramcSetting_Olympus_LP4_ByteMode_DDR1600(DRAMC_CTX_T *p)
{
    U8 ucMCKTxdlyOendqs = 0, ucMCKTxdlydqs = 0;
    U8 ucUITxdlyOendqs = 0, ucUITxdlydqs = 0;

    ////DRAMC0-SHU3
#if LEGACY_TDQSCK_PRECAL
    LegacyPreCalLP4_DDR1600(p);
#endif
#if LEGACY_TX_TRACK
     LegacyTxTrackLP4_DDR1600(p);
#endif

    //vIO32WriteFldAlign(DRAMC_REG_SHU_CONF2, 0x32, SHU_CONF2_FSPCHG_PRDCNT); //ACTiming related -> set in UpdateACTiming_Reg()
    //vIO32WriteFldMulti(DRAMC_REG_SHU_CKECTRL, P_Fld(0x0, SHU_CKECTRL_TCKESRX)); //from CF review, only work for DDR3/4 (Darren)
                //| P_Fld(0x2, SHU_CKECTRL_CKEPRD));
    vIO32WriteFldMulti(DRAMC_REG_SHU_ODTCTRL, P_Fld(0x0, SHU_ODTCTRL_RODTE)
                | P_Fld(0x0, SHU_ODTCTRL_RODTE2)
                //| P_Fld(0x2, SHU_ODTCTRL_RODT) //Set in UpdateACTimingReg()
#ifdef LOOPBACK_TEST
                | P_Fld(0x1, SHU_ODTCTRL_ROEN));
#else
                | P_Fld(0x0, SHU_ODTCTRL_ROEN));
#endif
    vIO32WriteFldAlign(DDRPHY_SHU_B0_DQ7, 0x0, SHU_B0_DQ7_R_DMRODTEN_B0);
    vIO32WriteFldAlign(DDRPHY_SHU_B1_DQ7, 0x0, SHU_B1_DQ7_R_DMRODTEN_B1);

    if (vGet_Div_Mode(p) == DIV4_MODE)
    {
        ucMCKTxdlyOendqs = 0x3;
        ucMCKTxdlydqs = 0x4;
        ucUITxdlyOendqs = 0x2;
        ucUITxdlydqs = 0x1;
    }
    else
    {
        ucMCKTxdlyOendqs = 0x1;
        ucMCKTxdlydqs = 0x2;
        ucUITxdlyOendqs = 0x6;
        ucUITxdlydqs = 0x1;
    }

    //MCK by Wleveling
    vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_DQS0, P_Fld(ucMCKTxdlyOendqs, SHU_SELPH_DQS0_TXDLY_OEN_DQS3)
                | P_Fld(ucMCKTxdlyOendqs, SHU_SELPH_DQS0_TXDLY_OEN_DQS2)
                | P_Fld(ucMCKTxdlyOendqs, SHU_SELPH_DQS0_TXDLY_OEN_DQS1)
                | P_Fld(ucMCKTxdlyOendqs, SHU_SELPH_DQS0_TXDLY_OEN_DQS0)
                | P_Fld(ucMCKTxdlydqs, SHU_SELPH_DQS0_TXDLY_DQS3)
                | P_Fld(ucMCKTxdlydqs, SHU_SELPH_DQS0_TXDLY_DQS2)
                | P_Fld(ucMCKTxdlydqs, SHU_SELPH_DQS0_TXDLY_DQS1)
                | P_Fld(ucMCKTxdlydqs, SHU_SELPH_DQS0_TXDLY_DQS0));
    //UI by Wleveling
    vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_DQS1, P_Fld(ucUITxdlyOendqs, SHU_SELPH_DQS1_DLY_OEN_DQS3)
                | P_Fld(ucUITxdlyOendqs, SHU_SELPH_DQS1_DLY_OEN_DQS2)
                | P_Fld(ucUITxdlyOendqs, SHU_SELPH_DQS1_DLY_OEN_DQS1)
                | P_Fld(ucUITxdlyOendqs, SHU_SELPH_DQS1_DLY_OEN_DQS0)
                | P_Fld(ucUITxdlydqs, SHU_SELPH_DQS1_DLY_DQS3)
                | P_Fld(ucUITxdlydqs, SHU_SELPH_DQS1_DLY_DQS2)
                | P_Fld(ucUITxdlydqs, SHU_SELPH_DQS1_DLY_DQS1)
                | P_Fld(ucUITxdlydqs, SHU_SELPH_DQS1_DLY_DQS0));

    vIO32WriteFldAlign(DRAMC_REG_SHU_DQS2DQ_TX, 0x4, SHU_DQS2DQ_TX_OE2DQ_OFFSET);

    vIO32WriteFldMulti(DRAMC_REG_SHURK0_PI, P_Fld(0x1a, SHURK0_PI_RK0_ARPI_DQM_B1)
                | P_Fld(0x1a, SHURK0_PI_RK0_ARPI_DQM_B0)
                | P_Fld(0x1a, SHURK0_PI_RK0_ARPI_DQ_B1)
                | P_Fld(0x1a, SHURK0_PI_RK0_ARPI_DQ_B0));
    //Rank0/1 TX Delay
    if (p->freqGroup == 400)
    {
        DramcSetting_LP4_TX_Delay_DDR800(p, RANK_0);
        DramcSetting_LP4_TX_Delay_DDR800(p, RANK_1);
    }
    else // for DDR1200 and DDR1600
    {
        DramcSetting_LP4_TX_Delay_DDR1600(p, RANK_0);
        DramcSetting_LP4_TX_Delay_DDR1600(p, RANK_1);
    }
    // Tx tracking
    vIO32WriteFldMulti(DRAMC_REG_SHURK1_PI, P_Fld(0x1e, SHURK1_PI_RK1_ARPI_DQM_B1)
                | P_Fld(0x1e, SHURK1_PI_RK1_ARPI_DQM_B0)
                | P_Fld(0x1e, SHURK1_PI_RK1_ARPI_DQ_B1)
                | P_Fld(0x1e, SHURK1_PI_RK1_ARPI_DQ_B0));

#if LEGACY_GATING_DLY
    if (p->freqGroup == 400)
        LegacyGatingDlyLP4_DDR800(p); // for 1:4 mode settings
    else
        LegacyGatingDlyLP4_DDR1600(p); // for 1:8 and 1:4 mode settings for DDR1200 and DDR1600
#endif

    vIO32WriteFldMulti(DRAMC_REG_SHU_DQSG_RETRY, P_Fld(0x3, SHU_DQSG_RETRY_R_DQSIENLAT)
                | P_Fld(0x0, SHU_DQSG_RETRY_R_DDR1866_PLUS));

    ////DDRPHY0-SHU3
    vIO32WriteFldAlign(DDRPHY_SHU_B0_DQ5, 0x5, SHU_B0_DQ5_RG_RX_ARDQS0_DVS_DLY_B0); // for Rx guard
    //vIO32WriteFldMulti(DDRPHY_SHU_B0_DQ6, P_Fld(0x1, SHU_B0_DQ6_RG_ARPI_MIDPI_CKDIV4_EN_B0)
    //            | P_Fld(0x0, SHU_B0_DQ6_RG_ARPI_MIDPI_EN_B0));
    if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
    {
        vIO32WriteFldMulti(DDRPHY_SHU_B0_DQ7, P_Fld(0x1, SHU_B0_DQ7_MIDPI_DIV4_ENABLE)
                    | P_Fld(0x0, SHU_B0_DQ7_MIDPI_ENABLE));
    }
    else
    {
        vIO32WriteFldMulti(DDRPHY_SHU_B0_DQ7, P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B0)
                    | P_Fld(0x0, SHU_B0_DQ7_R_DMDQMDBI_SHU_B0)
                    | P_Fld(0x1, SHU_B0_DQ7_MIDPI_DIV4_ENABLE)
                    | P_Fld(0x0, SHU_B0_DQ7_MIDPI_ENABLE));
    }
    vIO32WriteFldAlign(DDRPHY_SHU_B1_DQ5, 0x5, SHU_B1_DQ5_RG_RX_ARDQS0_DVS_DLY_B1); // for Rx guard
    //vIO32WriteFldMulti(DDRPHY_SHU_B1_DQ6, P_Fld(0x1, SHU_B1_DQ6_RG_ARPI_MIDPI_CKDIV4_EN_B1)
    //            | P_Fld(0x0, SHU_B1_DQ6_RG_ARPI_MIDPI_EN_B1));
    if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
    {}
    else
    {
        vIO32WriteFldMulti(DDRPHY_SHU_B1_DQ7, P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B1)
                    | P_Fld(0x0, SHU_B1_DQ7_R_DMDQMDBI_SHU_B1));
    }

#if LEGACY_RX_DLY
    if (p->freqGroup == 400) // for DDR800 1:4 mode
        LegacyRxDly_LP4_DDR800(p);
    else
        LegacyRxDly_LP4_DDR1600(p);
#endif
#if LEGACY_DELAY_CELL
    LegacyDlyCellInitLP4_DDR1600(p);
#endif

    if (vGet_Div_Mode(p) == DIV4_MODE)
    {
#if 0
        if (p->freqGroup == 400) // for DDR800 1:4 mode
        {
            vIO32WriteFldMulti(DRAMC_REG_SHU_RANKCTL, P_Fld(0x6, SHU_RANKCTL_RANKINCTL_PHY)
                        | P_Fld(0x4, SHU_RANKCTL_RANKINCTL_ROOT1)
                        | P_Fld(0x4, SHU_RANKCTL_RANKINCTL));
        }
        else // for DDR1600 1:4 mode
        {
            vIO32WriteFldMulti(DRAMC_REG_SHU_RANKCTL, P_Fld(0x6, SHU_RANKCTL_RANKINCTL_PHY)
                        | P_Fld(0x4, SHU_RANKCTL_RANKINCTL_ROOT1)
                        | P_Fld(0x4, SHU_RANKCTL_RANKINCTL));
        }
#endif
        // 1:4 mode
        vIO32WriteFldMulti(DRAMC_REG_SHU_CONF0, P_Fld(0x0, SHU_CONF0_DM64BITEN)
                    | P_Fld(0x1, SHU_CONF0_FDIV2)
                    | P_Fld(0x0, SHU_CONF0_FREQDIV4)
                    | P_Fld(0x0, SHU_CONF0_DUALSCHEN)
                    | P_Fld(0x1, SHU_CONF0_WDATRGO));
#if 0 // 1:8 mode
        vIO32WriteFldMulti(DRAMC_REG_SHU_CONF0, P_Fld(0x1, SHU_CONF0_DM64BITEN)
                    | P_Fld(0x0, SHU_CONF0_FDIV2)
                    | P_Fld(0x1, SHU_CONF0_FREQDIV4)
                    | P_Fld(0x1, SHU_CONF0_DUALSCHEN)
                    | P_Fld(0x0, SHU_CONF0_WDATRGO));
#endif

    #if (!CMD_CKE_WORKAROUND_FIX)
        vIO32WriteFldAlign(DRAMC_REG_SHU_APHY_TX_PICG_CTRL, 0x7, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_DYN_GATING_SEL);
        vIO32WriteFldAlign(DRAMC_REG_SHU_SELPH_CA1, 0x1, SHU_SELPH_CA1_TXDLY_CKE);
        vIO32WriteFldAlign(DRAMC_REG_SHU_SELPH_CA2, 0x1, SHU_SELPH_CA2_TXDLY_CKE1);
    #endif
        //DATLAT 1:4 mode from Ton
        vIO32WriteFldMulti(DDRPHY_SHU_B0_DQ7, P_Fld(0x1, SHU_B0_DQ7_R_DMRDSEL_DIV2_OPT)
                    | P_Fld(0x1, SHU_B0_DQ7_R_DMRDSEL_LOBYTE_OPT)
                    | P_Fld(0x0, SHU_B0_DQ7_R_DMRDSEL_HIBYTE_OPT));
        //RDATA PIPE from Ton
        vIO32WriteFldMulti(DRAMC_REG_SHU_PIPE, P_Fld(0x1, SHU_PIPE_READ_START_EXTEND1)
                    | P_Fld(0x1, SHU_PIPE_DLE_LAST_EXTEND1)
                    | P_Fld(0x1, SHU_PIPE_READ_START_EXTEND2)
                    | P_Fld(0x1, SHU_PIPE_DLE_LAST_EXTEND2)
                    | P_Fld(0x0, SHU_PIPE_READ_START_EXTEND3)
                    | P_Fld(0x0, SHU_PIPE_DLE_LAST_EXTEND3));
    }
#if 0
    else
    {
        // for DDR1600 1:8 mode
        vIO32WriteFldMulti(DRAMC_REG_SHU_RANKCTL, P_Fld(0x2, SHU_RANKCTL_RANKINCTL_PHY)
                    | P_Fld(0x0, SHU_RANKCTL_RANKINCTL_ROOT1)
                    | P_Fld(0x0, SHU_RANKCTL_RANKINCTL));
    }
#endif
}

/*
CANNOT use DRAMC_WBR :
DDRPHY_CA_DLL_ARPI0 -> DDRPHY_SHU_CA_DLL1
DDRPHY_CA_DLL_ARPI3
DDRPHY_CA_DLL_ARPI5 -> DDRPHY_SHU_CA_DLL0
DDRPHY_SHU_CA_CMD6
*/

void DramcSetting_Olympus_LP4_ByteMode(DRAMC_CTX_T *p)
{

    U8 u1CAP_SEL;
    U8 u1MIDPICAP_SEL;
    //U16 u2SDM_PCW = 0; // SDM_PCW are set in DDRPhyPLLSetting()
    U8 u1TXDLY_CMD;
    U8 u1TAIL_LAT;

    AutoRefreshCKEOff(p);

    DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
                                                                             //before switch clock from 26M to PHY, need to init PHY clock first
    vIO32WriteFldMulti_All(DDRPHY_CKMUX_SEL, P_Fld(0x1, CKMUX_SEL_R_PHYCTRLMUX)  //move CKMUX_SEL_R_PHYCTRLMUX to here (it was originally between MISC_CG_CTRL0_CLK_MEM_SEL and MISC_CTRL0_R_DMRDSEL_DIV2_OPT)
                                        | P_Fld(0x1, CKMUX_SEL_R_PHYCTRLDCM)); // PHYCTRLDCM 1: follow DDRPHY_conf DCM settings, 0: follow infra DCM settings
    DramcBroadcastOnOff(DRAMC_BROADCAST_ON);

    //chg_mem_en = 1
    vIO32WriteFldAlign(DDRPHY_MISC_CG_CTRL0, 0x1, MISC_CG_CTRL0_W_CHG_MEM);
    //26M
    vIO32WriteFldAlign(DDRPHY_MISC_CG_CTRL0, 0x0, MISC_CG_CTRL0_CLK_MEM_SEL);

    vIO32WriteFldMulti(DDRPHY_SHU_B0_DQ7, P_Fld(0x0, SHU_B0_DQ7_R_DMRDSEL_DIV2_OPT)
                | P_Fld(0x0, SHU_B0_DQ7_R_DMRDSEL_LOBYTE_OPT)
                | P_Fld(0x0, SHU_B0_DQ7_R_DMRDSEL_HIBYTE_OPT));

    //                   0 ===LP4_3200_intial_setting_shu1 begin===
    #if APOLLO_SPECIFIC
    vIO32WriteFldAlign(DRAMC_REG_RKCFG, 1, RKCFG_CS1FORCE0);
    #endif
    //Francis : pin mux issue, need to set CHD
    // TODO: ARDMSUS_10 already set to 0 in SwimpedanceCal(), may be removed here?
    vIO32WriteFldMulti(DDRPHY_MISC_SPM_CTRL1, P_Fld(0x0, MISC_SPM_CTRL1_RG_ARDMSUS_10) | P_Fld(0x0, MISC_SPM_CTRL1_RG_ARDMSUS_10_B0)
                                           | P_Fld(0x0, MISC_SPM_CTRL1_RG_ARDMSUS_10_B1) | P_Fld(0x0, MISC_SPM_CTRL1_RG_ARDMSUS_10_CA));

    vIO32WriteFldAlign(DDRPHY_MISC_SPM_CTRL2, 0x0, MISC_SPM_CTRL2_PHY_SPM_CTL2);
    vIO32WriteFldAlign(DDRPHY_MISC_SPM_CTRL0, 0x0, MISC_SPM_CTRL0_PHY_SPM_CTL0);
    vIO32WriteFldAlign(DDRPHY_MISC_CG_CTRL2, 0x6003bf, MISC_CG_CTRL2_RG_MEM_DCM_CTL);//Move to DCM off setting
    vIO32WriteFldAlign(DDRPHY_MISC_CG_CTRL4, 0x333f3f00, MISC_CG_CTRL4_R_PHY_MCK_CG_CTRL);//Move to DCM off setting
    vIO32WriteFldMulti(DDRPHY_SHU_PLL1, P_Fld(0x1, SHU_PLL1_R_SHU_AUTO_PLL_MUX)
                | P_Fld(0x7, SHU_PLL1_SHU1_PLL1_RFU));
    vIO32WriteFldMulti(DDRPHY_SHU_B0_DQ7, P_Fld(0x1, SHU_B0_DQ7_MIDPI_ENABLE)
                | P_Fld(0x0, SHU_B0_DQ7_MIDPI_DIV4_ENABLE)
                | P_Fld(0, SHU_B0_DQ7_R_DMRANKRXDVS_B0));
    vIO32WriteFldAlign(DDRPHY_SHU_B1_DQ7, 0, SHU_B1_DQ7_R_DMRANKRXDVS_B1);
    //vIO32WriteFldAlign(DDRPHY_SHU_PLL4, 0xfe, SHU_PLL4_RG_RPHYPLL_RESERVED);
    vIO32WriteFldMulti(DDRPHY_SHU_PLL4, P_Fld(0x1, SHU_PLL4_RG_RPHYPLL_IBIAS) | P_Fld(0x1, SHU_PLL4_RG_RPHYPLL_ICHP) | P_Fld(0x1, SHU_PLL4_RG_RPHYPLL_FS));
    //vIO32WriteFldAlign(DDRPHY_SHU_PLL6, 0xfe, SHU_PLL6_RG_RCLRPLL_RESERVED);
    vIO32WriteFldMulti(DDRPHY_SHU_PLL6, P_Fld(0x1, SHU_PLL6_RG_RCLRPLL_IBIAS) | P_Fld(0x1, SHU_PLL6_RG_RCLRPLL_ICHP) | P_Fld(0x1, SHU_PLL6_RG_RCLRPLL_FS));
    vIO32WriteFldAlign(DDRPHY_SHU_PLL14, 0x0, SHU_PLL14_RG_RPHYPLL_SDM_SSC_PH_INIT);
    vIO32WriteFldAlign(DDRPHY_SHU_PLL20, 0x0, SHU_PLL20_RG_RCLRPLL_SDM_SSC_PH_INIT);
    vIO32WriteFldMulti(DDRPHY_CA_CMD2, P_Fld(0x0, CA_CMD2_RG_TX_ARCMD_OE_DIS)
                | P_Fld(0x0, CA_CMD2_RG_TX_ARCMD_ODTEN_DIS)
                | P_Fld(0x0, CA_CMD2_RG_TX_ARCLK_OE_DIS)
                | P_Fld(0x0, CA_CMD2_RG_TX_ARCLK_ODTEN_DIS));
    vIO32WriteFldMulti(DDRPHY_B0_DQ2, P_Fld(0x0, B0_DQ2_RG_TX_ARDQ_OE_DIS_B0)
                | P_Fld(0x0, B0_DQ2_RG_TX_ARDQ_ODTEN_DIS_B0)
                | P_Fld(0x0, B0_DQ2_RG_TX_ARDQS0_OE_DIS_B0)
                | P_Fld(0x0, B0_DQ2_RG_TX_ARDQS0_ODTEN_DIS_B0));
    vIO32WriteFldMulti(DDRPHY_B1_DQ2, P_Fld(0x0, B1_DQ2_RG_TX_ARDQ_OE_DIS_B1)
                | P_Fld(0x0, B1_DQ2_RG_TX_ARDQ_ODTEN_DIS_B1)
                | P_Fld(0x0, B1_DQ2_RG_TX_ARDQS0_OE_DIS_B1)
                | P_Fld(0x0, B1_DQ2_RG_TX_ARDQS0_ODTEN_DIS_B1));
    #if 0 //Correct settings are set in UpdateInitialSettings_LP4()
    vIO32WriteFldAlign(DDRPHY_B0_DQ9, 0x0, B0_DQ9_R_IN_GATE_EN_LOW_OPT_B0);
    vIO32WriteFldAlign(DDRPHY_B1_DQ9, 0x7, B1_DQ9_R_IN_GATE_EN_LOW_OPT_B1);
    vIO32WriteFldAlign(DDRPHY_CA_CMD10, 0x0, CA_CMD10_R_IN_GATE_EN_LOW_OPT_CA);
    #endif
    vIO32WriteFldAlign(DDRPHY_B0_DQ9, 0x1, B0_DQ9_R_DMRXDVS_RDSEL_LAT_B0);
    vIO32WriteFldAlign(DDRPHY_B1_DQ9, 0x1, B1_DQ9_R_DMRXDVS_RDSEL_LAT_B1);
    vIO32WriteFldAlign(DDRPHY_CA_CMD10, 0x0, CA_CMD10_R_DMRXDVS_RDSEL_LAT_CA);

    vIO32WriteFldAlign(DDRPHY_B0_RXDVS0, 0x1, B0_RXDVS0_R_RX_DLY_TRACK_CG_EN_B0);
    vIO32WriteFldAlign(DDRPHY_B1_RXDVS0, 0x1, B1_RXDVS0_R_RX_DLY_TRACK_CG_EN_B1);
    vIO32WriteFldAlign(DDRPHY_B0_RXDVS0, 0x1, B0_RXDVS0_R_DMRXDVS_DQIENPRE_OPT_B0);
    vIO32WriteFldAlign(DDRPHY_B1_RXDVS0, 0x1, B1_RXDVS0_R_DMRXDVS_DQIENPRE_OPT_B1);
    vIO32WriteFldAlign(DDRPHY_R0_B0_RXDVS2, 0x1, R0_B0_RXDVS2_R_RK0_DVS_FDLY_MODE_B0);
    vIO32WriteFldAlign(DDRPHY_R1_B0_RXDVS2, 0x1, R1_B0_RXDVS2_R_RK1_DVS_FDLY_MODE_B0);
    vIO32WriteFldAlign(DDRPHY_R0_B1_RXDVS2, 0x1, R0_B1_RXDVS2_R_RK0_DVS_FDLY_MODE_B1);
    vIO32WriteFldAlign(DDRPHY_R1_B1_RXDVS2, 0x1, R1_B1_RXDVS2_R_RK1_DVS_FDLY_MODE_B1);
    vIO32WriteFldAlign(DDRPHY_SHU_B0_DQ5, 0x3, SHU_B0_DQ5_RG_RX_ARDQS0_DVS_DLY_B0);
    vIO32WriteFldAlign(DDRPHY_SHU_B1_DQ5, 0x3, SHU_B1_DQ5_RG_RX_ARDQS0_DVS_DLY_B1);
#if LEGACY_RX_DLY
    LegacyRxDly_LP4_DDR3200(p);
#endif
    vIO32WriteFldMulti(DDRPHY_R0_B0_RXDVS1, P_Fld(0x2, R0_B0_RXDVS1_R_RK0_B0_DVS_TH_LEAD)
                | P_Fld(0x2, R0_B0_RXDVS1_R_RK0_B0_DVS_TH_LAG));
    vIO32WriteFldMulti(DDRPHY_R1_B0_RXDVS1, P_Fld(0x2, R1_B0_RXDVS1_R_RK1_B0_DVS_TH_LEAD)
                | P_Fld(0x2, R1_B0_RXDVS1_R_RK1_B0_DVS_TH_LAG));
    vIO32WriteFldMulti(DDRPHY_R0_B1_RXDVS1, P_Fld(0x2, R0_B1_RXDVS1_R_RK0_B1_DVS_TH_LEAD)
                | P_Fld(0x2, R0_B1_RXDVS1_R_RK0_B1_DVS_TH_LAG));
    vIO32WriteFldMulti(DDRPHY_R1_B1_RXDVS1, P_Fld(0x2, R1_B1_RXDVS1_R_RK1_B1_DVS_TH_LEAD)
                | P_Fld(0x2, R1_B1_RXDVS1_R_RK1_B1_DVS_TH_LAG));

    vIO32WriteFldMulti(DDRPHY_R0_B0_RXDVS2, P_Fld(0x2, R0_B0_RXDVS2_R_RK0_DVS_MODE_B0)
                | P_Fld(0x1, R0_B0_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B0)
                | P_Fld(0x1, R0_B0_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B0));
    vIO32WriteFldMulti(DDRPHY_R1_B0_RXDVS2, P_Fld(0x2, R1_B0_RXDVS2_R_RK1_DVS_MODE_B0)
                | P_Fld(0x1, R1_B0_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_B0)
                | P_Fld(0x1, R1_B0_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_B0));
    vIO32WriteFldMulti(DDRPHY_R0_B1_RXDVS2, P_Fld(0x2, R0_B1_RXDVS2_R_RK0_DVS_MODE_B1)
                | P_Fld(0x1, R0_B1_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B1)
                | P_Fld(0x1, R0_B1_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B1));
    vIO32WriteFldMulti(DDRPHY_R1_B1_RXDVS2, P_Fld(0x2, R1_B1_RXDVS2_R_RK1_DVS_MODE_B1)
                | P_Fld(0x1, R1_B1_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_B1)
                | P_Fld(0x1, R1_B1_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_B1));

    vIO32WriteFldAlign(DDRPHY_B0_RXDVS0, 0x0, B0_RXDVS0_R_RX_DLY_TRACK_CG_EN_B0);
    vIO32WriteFldAlign(DDRPHY_B1_RXDVS0, 0x0, B1_RXDVS0_R_RX_DLY_TRACK_CG_EN_B1);
    vIO32WriteFldAlign(DDRPHY_B0_DQ9, 0x1, B0_DQ9_RG_RX_ARDQ_STBEN_RESETB_B0);
    vIO32WriteFldAlign(DDRPHY_B1_DQ9, 0x1, B1_DQ9_RG_RX_ARDQ_STBEN_RESETB_B1);
#if LEGACY_DELAY_CELL
    LegacyDlyCellInitLP4_DDR3200(p);
#endif
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B0_DQ7, P_Fld(0x1f, SHU_R0_B0_DQ7_RK0_ARPI_DQM_B0)
                | P_Fld(0x1f, SHU_R0_B0_DQ7_RK0_ARPI_DQ_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B1_DQ7, P_Fld(0x1f, SHU_R0_B1_DQ7_RK0_ARPI_DQM_B1)
                | P_Fld(0x1f, SHU_R0_B1_DQ7_RK0_ARPI_DQ_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B0_DQ7, P_Fld(0x1f, SHU_R1_B0_DQ7_RK1_ARPI_DQM_B0)
                | P_Fld(0x1f, SHU_R1_B0_DQ7_RK1_ARPI_DQ_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B1_DQ7, P_Fld(0x1f, SHU_R1_B1_DQ7_RK1_ARPI_DQM_B1)
                | P_Fld(0x1f, SHU_R1_B1_DQ7_RK1_ARPI_DQ_B1));
    vIO32WriteFldMulti(DDRPHY_B0_DQ4, P_Fld(0x10, B0_DQ4_RG_RX_ARDQS_EYE_R_DLY_B0)
                | P_Fld(0x10, B0_DQ4_RG_RX_ARDQS_EYE_F_DLY_B0));
    vIO32WriteFldMulti(DDRPHY_B0_DQ5, P_Fld(0x0, B0_DQ5_RG_RX_ARDQ_EYE_EN_B0)
                | P_Fld(0x1, B0_DQ5_RG_RX_ARDQ_EYE_SEL_B0)
                | P_Fld(0x1, B0_DQ5_RG_RX_ARDQ_VREF_EN_B0)
                | P_Fld(0xe, B0_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B0)
                | P_Fld(0x10, B0_DQ5_B0_DQ5_RFU));
    vIO32WriteFldMulti(DDRPHY_B0_DQ6, P_Fld(0x1, B0_DQ6_RG_RX_ARDQ_EYE_DLY_DQS_BYPASS_B0)
                | P_Fld(0x0, B0_DQ6_RG_TX_ARDQ_DDR3_SEL_B0)
                | P_Fld(0x0, B0_DQ6_RG_RX_ARDQ_DDR3_SEL_B0)
                | P_Fld(0x1, B0_DQ6_RG_TX_ARDQ_DDR4_SEL_B0)
                | P_Fld(0x1, B0_DQ6_RG_RX_ARDQ_DDR4_SEL_B0)
                | P_Fld(0x0, B0_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B0)
                | P_Fld(0x1, B0_DQ6_RG_RX_ARDQ_BIAS_EN_B0)
                | P_Fld(0x1, B0_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B0));
    vIO32WriteFldMulti(DDRPHY_B0_DQ5, P_Fld(0x1, B0_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B0)
                | P_Fld(0x0, B0_DQ5_B0_DQ5_RFU));
    vIO32WriteFldMulti(DDRPHY_B1_DQ4, P_Fld(0x10, B1_DQ4_RG_RX_ARDQS_EYE_R_DLY_B1)
                | P_Fld(0x10, B1_DQ4_RG_RX_ARDQS_EYE_F_DLY_B1));
    vIO32WriteFldMulti(DDRPHY_B1_DQ5, P_Fld(0x0, B1_DQ5_RG_RX_ARDQ_EYE_EN_B1)
                | P_Fld(0x1, B1_DQ5_RG_RX_ARDQ_EYE_SEL_B1)
                | P_Fld(0x1, B1_DQ5_RG_RX_ARDQ_VREF_EN_B1)
                | P_Fld(0xe, B1_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B1)
                | P_Fld(0x10, B1_DQ5_B1_DQ5_RFU));
    vIO32WriteFldMulti(DDRPHY_B1_DQ6, P_Fld(0x1, B1_DQ6_RG_RX_ARDQ_EYE_DLY_DQS_BYPASS_B1)
                | P_Fld(0x0, B1_DQ6_RG_TX_ARDQ_DDR3_SEL_B1)
                | P_Fld(0x0, B1_DQ6_RG_RX_ARDQ_DDR3_SEL_B1)
                | P_Fld(0x1, B1_DQ6_RG_TX_ARDQ_DDR4_SEL_B1)
                | P_Fld(0x1, B1_DQ6_RG_RX_ARDQ_DDR4_SEL_B1)
                | P_Fld(0x0, B1_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B1)
                | P_Fld(0x1, B1_DQ6_RG_RX_ARDQ_BIAS_EN_B1)
                | P_Fld(0x1, B1_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B1));
    vIO32WriteFldMulti(DDRPHY_B1_DQ5, P_Fld(0x1, B1_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B1)
                | P_Fld(0x0, B1_DQ5_B1_DQ5_RFU));
    vIO32WriteFldMulti(DDRPHY_CA_CMD3, P_Fld(0x1, CA_CMD3_RG_RX_ARCMD_IN_BUFF_EN)
                | P_Fld(0x1, CA_CMD3_RG_ARCMD_RESETB)
                | P_Fld(0x1, CA_CMD3_RG_TX_ARCMD_EN));
    vIO32WriteFldMulti(DDRPHY_CA_CMD6, P_Fld(0x0, CA_CMD6_RG_RX_ARCMD_DDR4_SEL)
                | P_Fld(0x0, CA_CMD6_RG_RX_ARCMD_BIAS_VREF_SEL)
                | P_Fld(0x0, CA_CMD6_RG_RX_ARCMD_RES_BIAS_EN));
    /* ARCMD_DRVP, DRVN , ARCLK_DRVP, DRVN not used anymore
    vIO32WriteFldMulti(DDRPHY_SHU_CA_CMD1, P_Fld(0x1, SHU_CA_CMD1_RG_TX_ARCMD_DRVN)
                | P_Fld(0x1, SHU_CA_CMD1_RG_TX_ARCMD_DRVP));
    vIO32WriteFldMulti(DDRPHY_SHU_CA_CMD2, P_Fld(0x1, SHU_CA_CMD2_RG_TX_ARCLK_DRVN)
                | P_Fld(0x1, SHU_CA_CMD2_RG_TX_ARCLK_DRVP));
     */
    //vIO32WriteFldMulti(DDRPHY_SHU2_CA_CMD1, P_Fld(0x1, SHU2_CA_CMD1_RG_TX_ARCMD_DRVN)
    //            | P_Fld(0x1, SHU2_CA_CMD1_RG_TX_ARCMD_DRVP));
    //vIO32WriteFldMulti(DDRPHY_SHU2_CA_CMD2, P_Fld(0x1, SHU2_CA_CMD2_RG_TX_ARCLK_DRVN)
    //            | P_Fld(0x1, SHU2_CA_CMD2_RG_TX_ARCLK_DRVP));
    vIO32WriteFldMulti(DDRPHY_PLL3, P_Fld(0x0, PLL3_RG_RPHYPLL_TSTOP_EN) | P_Fld(0x0, PLL3_RG_RPHYPLL_TST_EN));
    vIO32WriteFldAlign(DDRPHY_MISC_VREF_CTRL, 0x0, MISC_VREF_CTRL_RG_RVREF_VREF_EN); //LP3 VREF

    vIO32WriteFldAlign(DDRPHY_B0_DQ3, 0x1, B0_DQ3_RG_ARDQ_RESETB_B0);
    vIO32WriteFldAlign(DDRPHY_B1_DQ3, 0x1, B1_DQ3_RG_ARDQ_RESETB_B1);

    mcDELAY_US(1);

    //Ref clock should be 20M~30M, if MPLL=52M, Pre-divider should be set to 1
    vIO32WriteFldMulti(DDRPHY_SHU_PLL8, P_Fld(0x0, SHU_PLL8_RG_RPHYPLL_POSDIV) | P_Fld(0x1, SHU_PLL8_RG_RPHYPLL_PREDIV));
    //vIO32WriteFldAlign(DDRPHY_SHU2_PLL8, 0x0, SHU2_PLL8_RG_RPHYPLL_POSDIV);
    //vIO32WriteFldAlign(DDRPHY_SHU3_PLL8, 0x0, SHU3_PLL8_RG_RPHYPLL_POSDIV);
    //vIO32WriteFldAlign(DDRPHY_SHU4_PLL8, 0x0, SHU4_PLL8_RG_RPHYPLL_POSDIV);

    mcDELAY_US(1);

    vIO32WriteFldMulti(DDRPHY_SHU_PLL9, P_Fld(0x0, SHU_PLL9_RG_RPHYPLL_MONCK_EN)
                | P_Fld(0x0, SHU_PLL9_RG_RPHYPLL_MONVC_EN)
                | P_Fld(0x0, SHU_PLL9_RG_RPHYPLL_LVROD_EN)
                | P_Fld(0x1, SHU_PLL9_RG_RPHYPLL_RST_DLY));
    vIO32WriteFldMulti(DDRPHY_SHU_PLL11, P_Fld(0x0, SHU_PLL11_RG_RCLRPLL_MONCK_EN)
                | P_Fld(0x0, SHU_PLL11_RG_RCLRPLL_MONVC_EN)
                | P_Fld(0x0, SHU_PLL11_RG_RCLRPLL_LVROD_EN)
                | P_Fld(0x1, SHU_PLL11_RG_RCLRPLL_RST_DLY));

    mcDELAY_US(1);

    //Ref clock should be 20M~30M, if MPLL=52M, Pre-divider should be set to 1
    vIO32WriteFldMulti(DDRPHY_SHU_PLL10, P_Fld(0x0, SHU_PLL10_RG_RCLRPLL_POSDIV) | P_Fld(0x1, SHU_PLL10_RG_RCLRPLL_PREDIV));
    //vIO32WriteFldAlign(DDRPHY_SHU2_PLL10, 0x0, SHU2_PLL10_RG_RCLRPLL_POSDIV);
    //vIO32WriteFldAlign(DDRPHY_SHU3_PLL10, 0x0, SHU3_PLL10_RG_RCLRPLL_POSDIV);
    //vIO32WriteFldAlign(DDRPHY_SHU4_PLL10, 0x0, SHU4_PLL10_RG_RCLRPLL_POSDIV);

    mcDELAY_US(1);


    ///TODO: MIDPI Init 1
    vIO32WriteFldMulti(DDRPHY_PLL4, P_Fld(0x0, PLL4_RG_RPHYPLL_AD_MCK8X_EN)
                | P_Fld(0x1, PLL4_PLL4_RFU)
                | P_Fld(0x1, PLL4_RG_RPHYPLL_MCK8X_SEL));


    mcDELAY_US(1);

    //@Darren-vIO32WriteFldAlign(DDRPHY_SHU_PLL0, 0x3, SHU_PLL0_RG_RPHYPLL_TOP_REV); // debug1111, org:3 -> mdf:0
    vIO32WriteFldMulti(DDRPHY_SHU_PLL0, P_Fld(0x1, SHU_PLL0_ADA_MCK8X_CHB_EN)
                | P_Fld(0x1, SHU_PLL0_ADA_MCK8X_CHA_EN)); //@Darren+
    //vIO32WriteFldAlign(DDRPHY_SHU2_PLL0, 0x3, SHU2_PLL0_RG_RPHYPLL_TOP_REV);
    //vIO32WriteFldAlign(DDRPHY_SHU3_PLL0, 0x3, SHU3_PLL0_RG_RPHYPLL_TOP_REV);
    //vIO32WriteFldAlign(DDRPHY_SHU4_PLL0, 0x3, SHU4_PLL0_RG_RPHYPLL_TOP_REV);

    mcDELAY_US(1);


    vIO32WriteFldAlign(DDRPHY_CA_DLL_ARPI1, 0x1, CA_DLL_ARPI1_RG_ARPISM_MCK_SEL_CA);
    vIO32WriteFldMulti(DDRPHY_B0_DQ3, P_Fld(0x1, B0_DQ3_RG_RX_ARDQ_STBENCMP_EN_B0)
                | P_Fld(0x1, B0_DQ3_RG_TX_ARDQ_EN_B0)
                | P_Fld(0x1, B0_DQ3_RG_RX_ARDQ_SMT_EN_B0));
    vIO32WriteFldMulti(DDRPHY_B1_DQ3, P_Fld(0x1, B1_DQ3_RG_RX_ARDQ_STBENCMP_EN_B1)
                | P_Fld(0x1, B1_DQ3_RG_TX_ARDQ_EN_B1)
                | P_Fld(0x1, B1_DQ3_RG_RX_ARDQ_SMT_EN_B1));



#if (fcFOR_CHIP_ID == fcLafite)
    DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
    vIO32WriteFldAlign(DDRPHY_SHU_CA_DLL0, 0x1, SHU_CA_DLL0_RG_ARPISM_MCK_SEL_CA_SHU);//Move to DCM off setting
    vIO32WriteFldAlign(DDRPHY_SHU_CA_DLL0 + SHIFT_TO_CHB_ADDR, 0x1, SHU_CA_DLL0_RG_ARPISM_MCK_SEL_CA_SHU);//Move to DCM off setting
    DramcBroadcastOnOff(DRAMC_BROADCAST_ON);
#endif

    vIO32WriteFldMulti(DDRPHY_SHU_B0_DLL0, P_Fld(0x1, SHU_B0_DLL0_RG_ARDLL_PHDET_IN_SWAP_B0)
                | P_Fld(0x7, SHU_B0_DLL0_RG_ARDLL_GAIN_B0)
                | P_Fld(0x7, SHU_B0_DLL0_RG_ARDLL_IDLECNT_B0)
                | P_Fld(0x8, SHU_B0_DLL0_RG_ARDLL_P_GAIN_B0)
                | P_Fld(0x1, SHU_B0_DLL0_RG_ARDLL_PHJUMP_EN_B0)
                | P_Fld(0x1, SHU_B0_DLL0_RG_ARDLL_PHDIV_B0)
                | P_Fld(0x0, SHU_B0_DLL0_RG_ARDLL_FAST_PSJP_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_B1_DLL0, P_Fld(0x1, SHU_B1_DLL0_RG_ARDLL_PHDET_IN_SWAP_B1)
                | P_Fld(0x7, SHU_B1_DLL0_RG_ARDLL_GAIN_B1)
                | P_Fld(0x7, SHU_B1_DLL0_RG_ARDLL_IDLECNT_B1)
                | P_Fld(0x8, SHU_B1_DLL0_RG_ARDLL_P_GAIN_B1)
                | P_Fld(0x1, SHU_B1_DLL0_RG_ARDLL_PHJUMP_EN_B1)
                | P_Fld(0x1, SHU_B1_DLL0_RG_ARDLL_PHDIV_B1)
                | P_Fld(0x0, SHU_B1_DLL0_RG_ARDLL_FAST_PSJP_B1));

    vIO32WriteFldAlign(DDRPHY_SHU_CA_CMD5, 0x29, SHU_CA_CMD5_RG_RX_ARCMD_VREF_SEL);
    //vIO32WriteFldAlign(DDRPHY_SHU2_CA_CMD5, 0x0, SHU2_CA_CMD5_RG_RX_ARCMD_VREF_SEL);
    //vIO32WriteFldAlign(DDRPHY_SHU3_CA_CMD5, 0x0, SHU3_CA_CMD5_RG_RX_ARCMD_VREF_SEL);
    //vIO32WriteFldAlign(DDRPHY_SHU4_CA_CMD5, 0x0, SHU4_CA_CMD5_RG_RX_ARCMD_VREF_SEL);
    vIO32WriteFldMulti(DDRPHY_SHU_CA_CMD0, P_Fld(0x1, SHU_CA_CMD0_RG_TX_ARCMD_PRE_EN)
                | P_Fld(0x4, SHU_CA_CMD0_RG_TX_ARCLK_DRVN_PRE)
                | P_Fld(0x1, SHU_CA_CMD0_RG_TX_ARCLK_PRE_EN));
    //vIO32WriteFldMulti(DDRPHY_SHU2_CA_CMD0, P_Fld(0x1, SHU2_CA_CMD0_RG_TX_ARCMD_PRE_EN)
    //            | P_Fld(0x4, SHU2_CA_CMD0_RG_TX_ARCLK_DRVN_PRE)
    //            | P_Fld(0x1, SHU2_CA_CMD0_RG_TX_ARCLK_PRE_EN));
    //vIO32WriteFldMulti(DDRPHY_SHU3_CA_CMD0, P_Fld(0x1, SHU3_CA_CMD0_RG_TX_ARCMD_PRE_EN)
    //            | P_Fld(0x4, SHU3_CA_CMD0_RG_TX_ARCLK_DRVN_PRE)
    //            | P_Fld(0x1, SHU3_CA_CMD0_RG_TX_ARCLK_PRE_EN));
    //vIO32WriteFldMulti(DDRPHY_SHU4_CA_CMD0, P_Fld(0x1, SHU4_CA_CMD0_RG_TX_ARCMD_PRE_EN)
    //            | P_Fld(0x4, SHU4_CA_CMD0_RG_TX_ARCLK_DRVN_PRE)
    //            | P_Fld(0x1, SHU4_CA_CMD0_RG_TX_ARCLK_PRE_EN));
#if (fcFOR_CHIP_ID == fcLafite)
    DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
    vIO32WriteFldAlign_All(DDRPHY_SHU_CA_CMD6, 0x0, SHU_CA_CMD6_RG_TX_ARCMD_SER_MODE);
    DramcBroadcastOnOff(DRAMC_BROADCAST_ON);
#endif
    //vIO32WriteFldAlign(DDRPHY_SHU2_CA_CMD6, 0x3, SHU2_CA_CMD6_RG_ARPI_RESERVE_CA);
    //vIO32WriteFldAlign(DDRPHY_SHU3_CA_CMD6, 0x3, SHU3_CA_CMD6_RG_ARPI_RESERVE_CA);
    //vIO32WriteFldAlign(DDRPHY_SHU4_CA_CMD6, 0x3, SHU4_CA_CMD6_RG_ARPI_RESERVE_CA);
    //vIO32WriteFldAlign(DDRPHY_SHU_CA_CMD3, 0x4e1, SHU_CA_CMD3_RG_ARCMD_REV);
    //vIO32WriteFldAlign(DDRPHY_SHU2_CA_CMD7, 0x4e1, SHU2_CA_CMD7_RG_ARCMD_REV);
    //vIO32WriteFldAlign(DDRPHY_SHU3_CA_CMD7, 0x4e1, SHU3_CA_CMD7_RG_ARCMD_REV);
    //vIO32WriteFldAlign(DDRPHY_SHU4_CA_CMD7, 0x4e1, SHU4_CA_CMD7_RG_ARCMD_REV);
    //vIO32WriteFldMulti(DDRPHY_SHU_B0_DQ7, P_Fld(0x00, SHU_B0_DQ7_RG_ARDQ_REV_B0)
    //            | P_Fld(0x0, SHU_B0_DQ7_DQ_REV_B0_BIT_05));
    //vIO32WriteFldAlign(DDRPHY_SHU2_B0_DQ7, 0x20, SHU2_B0_DQ7_RG_ARDQ_REV_B0);
    //vIO32WriteFldAlign(DDRPHY_SHU3_B0_DQ7, 0x20, SHU3_B0_DQ7_RG_ARDQ_REV_B0);
    //vIO32WriteFldAlign(DDRPHY_SHU4_B0_DQ7, 0x20, SHU4_B0_DQ7_RG_ARDQ_REV_B0);
    //vIO32WriteFldMulti(DDRPHY_SHU_B1_DQ7, P_Fld(0x00, SHU_B1_DQ7_RG_ARDQ_REV_B1)
    //            | P_Fld(0x0, SHU_B1_DQ7_DQ_REV_B1_BIT_05));

 //lynx added
    //vIO32WriteFldAlign(DDRPHY_SHU_B0_DQ7, 0x0, SHU_B0_DQ7_RG_ARDQ_REV_B0);
    //vIO32WriteFldAlign(DDRPHY_SHU_B1_DQ7, 0x0, SHU_B1_DQ7_RG_ARDQ_REV_B1);
 //
    //vIO32WriteFldAlign(DDRPHY_SHU2_B1_DQ7, 0x20, SHU2_B1_DQ7_RG_ARDQ_REV_B1);
    //vIO32WriteFldAlign(DDRPHY_SHU3_B1_DQ7, 0x20, SHU3_B1_DQ7_RG_ARDQ_REV_B1);
    //vIO32WriteFldAlign(DDRPHY_SHU4_B1_DQ7, 0x20, SHU4_B1_DQ7_RG_ARDQ_REV_B1);
    vIO32WriteFldAlign(DDRPHY_SHU_B0_DQ6, 0x0, SHU_B0_DQ6_RG_TX_ARDQ_SER_MODE_B0);
    //vIO32WriteFldAlign(DDRPHY_SHU2_B0_DQ6, 0x1, SHU2_B0_DQ6_RG_ARPI_RESERVE_B0);
    //vIO32WriteFldAlign(DDRPHY_SHU3_B0_DQ6, 0x1, SHU3_B0_DQ6_RG_ARPI_RESERVE_B0);
    //vIO32WriteFldAlign(DDRPHY_SHU4_B0_DQ6, 0x1, SHU4_B0_DQ6_RG_ARPI_RESERVE_B0);
    vIO32WriteFldAlign(DDRPHY_SHU_B1_DQ6, 0x0, SHU_B1_DQ6_RG_TX_ARDQ_SER_MODE_B1);
    //vIO32WriteFldAlign(DDRPHY_SHU2_B1_DQ6, 0x1, SHU2_B1_DQ6_RG_ARPI_RESERVE_B1);
    //vIO32WriteFldAlign(DDRPHY_SHU3_B1_DQ6, 0x1, SHU3_B1_DQ6_RG_ARPI_RESERVE_B1);
    //vIO32WriteFldAlign(DDRPHY_SHU4_B1_DQ6, 0x1, SHU4_B1_DQ6_RG_ARPI_RESERVE_B1);
    DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
    vIO32WriteFldMulti(DDRPHY_MISC_SHU_OPT, P_Fld(0x1, MISC_SHU_OPT_R_CA_SHU_PHDET_SPM_EN)
                | P_Fld(0x1, MISC_SHU_OPT_R_CA_SHU_PHY_GATING_RESETB_SPM_EN)
                | P_Fld(0x2, MISC_SHU_OPT_R_DQB1_SHU_PHDET_SPM_EN)
                | P_Fld(0x1, MISC_SHU_OPT_R_DQB1_SHU_PHY_GATING_RESETB_SPM_EN)
                | P_Fld(0x2, MISC_SHU_OPT_R_DQB0_SHU_PHDET_SPM_EN)
                | P_Fld(0x1, MISC_SHU_OPT_R_DQB0_SHU_PHY_GATING_RESETB_SPM_EN));
    vIO32WriteFldMulti(DDRPHY_MISC_SHU_OPT + SHIFT_TO_CHB_ADDR, P_Fld(0x2, MISC_SHU_OPT_R_CA_SHU_PHDET_SPM_EN)
                | P_Fld(0x1, MISC_SHU_OPT_R_CA_SHU_PHY_GATING_RESETB_SPM_EN)
                | P_Fld(0x2, MISC_SHU_OPT_R_DQB1_SHU_PHDET_SPM_EN)
                | P_Fld(0x1, MISC_SHU_OPT_R_DQB1_SHU_PHY_GATING_RESETB_SPM_EN)
                | P_Fld(0x2, MISC_SHU_OPT_R_DQB0_SHU_PHDET_SPM_EN)
                | P_Fld(0x1, MISC_SHU_OPT_R_DQB0_SHU_PHY_GATING_RESETB_SPM_EN));
    DramcBroadcastOnOff(DRAMC_BROADCAST_ON);

    mcDELAY_US(9);

#if (fcFOR_CHIP_ID == fcLafite)
    DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
    vIO32WriteFldMulti(DDRPHY_SHU_CA_DLL1, P_Fld(0x1, SHU_CA_DLL1_RG_ARDLL_PD_CK_SEL_CA) | P_Fld(0x0, SHU_CA_DLL1_RG_ARDLL_FASTPJ_CK_SEL_CA));
    vIO32WriteFldMulti(DDRPHY_SHU_CA_DLL1 + SHIFT_TO_CHB_ADDR, P_Fld(0x0, SHU_CA_DLL1_RG_ARDLL_PD_CK_SEL_CA)
                | P_Fld(0x1, SHU_CA_DLL1_RG_ARDLL_FASTPJ_CK_SEL_CA));//CH_B CA slave mode
    DramcBroadcastOnOff(DRAMC_BROADCAST_ON);
#endif
    vIO32WriteFldMulti(DDRPHY_SHU_B0_DLL1, P_Fld(0x0, SHU_B0_DLL1_RG_ARDLL_PD_CK_SEL_B0) | P_Fld(0x1, SHU_B0_DLL1_RG_ARDLL_FASTPJ_CK_SEL_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_B1_DLL1, P_Fld(0x0, SHU_B1_DLL1_RG_ARDLL_PD_CK_SEL_B1) | P_Fld(0x1, SHU_B1_DLL1_RG_ARDLL_FASTPJ_CK_SEL_B1));

    mcDELAY_US(1);

    vIO32WriteFldAlign(DDRPHY_PLL2, 0x0, PLL2_RG_RCLRPLL_EN);
    //vIO32WriteFldAlign(DDRPHY_SHU_PLL4, 0xff, SHU_PLL4_RG_RPHYPLL_RESERVED);
    //vIO32WriteFldAlign(DDRPHY_SHU_PLL6, 0xff, SHU_PLL6_RG_RCLRPLL_RESERVED);
    vIO32WriteFldAlign(DDRPHY_MISC_CG_CTRL0, 0xf, MISC_CG_CTRL0_CLK_MEM_DFS_CFG);

    mcDELAY_US(1);

    DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
    DDRPhyPLLSetting(p);
    DramcBroadcastOnOff(DRAMC_BROADCAST_ON);

    //rollback tMRRI design change
#if ENABLE_TMRRI_NEW_MODE
    //fix rank at 0 to trigger new TMRRI setting
    vIO32WriteFldAlign(DRAMC_REG_RKCFG, 0, RKCFG_TXRANK);
    vIO32WriteFldAlign(DRAMC_REG_RKCFG, 1, RKCFG_TXRANKFIX);
    vIO32WriteFldAlign(DRAMC_REG_DRSCTRL, 0x0, DRSCTRL_RK_SCINPUT_OPT);// new mode, HW_MRR: R_DMMRRRK, SW_MRR: R_DMMRSRK
    vIO32WriteFldMulti(DRAMC_REG_DRAMCTRL, P_Fld(0x0, DRAMCTRL_MRRIOPT) | P_Fld(0x0, DRAMCTRL_TMRRIBYRK_DIS) | P_Fld(0x1, DRAMCTRL_TMRRICHKDIS));
    vIO32WriteFldAlign(DRAMC_REG_SPCMDCTRL, 0x1, SPCMDCTRL_SC_PG_UPD_OPT);
    vIO32WriteFldMulti(DRAMC_REG_SPCMDCTRL, P_Fld(0x0, SPCMDCTRL_SC_PG_MPRW_DIS) | P_Fld(0x0, SPCMDCTRL_SC_PG_STCMD_AREF_DIS)
                                             | P_Fld(0x0, SPCMDCTRL_SC_PG_OPT2_DIS) | P_Fld(0x0, SPCMDCTRL_SC_PG_MAN_DIS));
    vIO32WriteFldMulti(DRAMC_REG_MPC_OPTION, P_Fld(0x1, MPC_OPTION_ZQ_BLOCKALE_OPT) | P_Fld(0x1, MPC_OPTION_MPC_BLOCKALE_OPT2) |
                                               P_Fld(0x1, MPC_OPTION_MPC_BLOCKALE_OPT1) | P_Fld(0x1, MPC_OPTION_MPC_BLOCKALE_OPT));
    //fix rank at 0 to trigger new TMRRI setting
    vIO32WriteFldAlign(DRAMC_REG_RKCFG, 0, RKCFG_TXRANK);
    vIO32WriteFldAlign(DRAMC_REG_RKCFG, 0, RKCFG_TXRANKFIX);
#else
    vIO32WriteFldAlign(DRAMC_REG_DRSCTRL, 0x1, DRSCTRL_RK_SCINPUT_OPT);// old mode, HW/SW MRR: R_DMMRRRK
    vIO32WriteFldMulti(DRAMC_REG_DRAMCTRL, P_Fld(0x1, DRAMCTRL_MRRIOPT) | P_Fld(0x1, DRAMCTRL_TMRRIBYRK_DIS) | P_Fld(0x0, DRAMCTRL_TMRRICHKDIS));
    vIO32WriteFldAlign(DRAMC_REG_SPCMDCTRL, 0x0, SPCMDCTRL_SC_PG_UPD_OPT);
    vIO32WriteFldMulti(DRAMC_REG_SPCMDCTRL, P_Fld(0x1, SPCMDCTRL_SC_PG_MPRW_DIS) | P_Fld(0x1, SPCMDCTRL_SC_PG_STCMD_AREF_DIS)
                                             | P_Fld(0x1, SPCMDCTRL_SC_PG_OPT2_DIS) | P_Fld(0x1, SPCMDCTRL_SC_PG_MAN_DIS));
#endif
    vIO32WriteFldAlign(DRAMC_REG_CKECTRL, 0x1, CKECTRL_RUNTIMEMRRCKEFIX);//Set Run time MRR CKE fix to 1 in tMRRI old mode to avoid no ACK from precharge all
    vIO32WriteFldAlign(DRAMC_REG_CKECTRL, 0x0, CKECTRL_RUNTIMEMRRMIODIS);

    vIO32WriteFldAlign(DDRPHY_B0_DQ9, 0x1, B0_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B0);
    vIO32WriteFldAlign(DDRPHY_B1_DQ9, 0x1, B1_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B1);
    vIO32WriteFldMulti(DRAMC_REG_SHURK1_DQSIEN, P_Fld(0xf, SHURK1_DQSIEN_R1DQS3IEN)
                | P_Fld(0xf, SHURK1_DQSIEN_R1DQS2IEN)
                | P_Fld(0xf, SHURK1_DQSIEN_R1DQS1IEN)
                | P_Fld(0xf, SHURK1_DQSIEN_R1DQS0IEN));
    vIO32WriteFldMulti(DRAMC_REG_STBCAL1, P_Fld(0x0, STBCAL1_DLLFRZ_MON_PBREF_OPT)
                | P_Fld(0x1, STBCAL1_STB_FLAGCLR)
                | P_Fld(0x1, STBCAL1_STBCNT_SHU_RST_EN) // from HJ review bring-up setting
                | P_Fld(0x1, STBCAL1_STBCNT_MODESEL));
    /* Darren-
    vIO32WriteFldMulti(DRAMC_REG_SHU_DQSG_RETRY, P_Fld(0x1, SHU_DQSG_RETRY_R_RETRY_USE_BURST_MDOE)
                | P_Fld(0x1, SHU_DQSG_RETRY_R_RDY_SEL_DLE)
                | P_Fld(0x6, SHU_DQSG_RETRY_R_DQSIENLAT)
                | P_Fld(0x1, SHU_DQSG_RETRY_R_RETRY_ONCE));
    */
    vIO32WriteFldMulti(DRAMC_REG_SHU_DRVING1, P_Fld(0xa, SHU_DRVING1_DQSDRVP2) | P_Fld(0xa, SHU_DRVING1_DQSDRVN2)
                | P_Fld(0xa, SHU_DRVING1_DQSDRVP1) | P_Fld(0xa, SHU_DRVING1_DQSDRVN1)
                | P_Fld(0xa, SHU_DRVING1_DQDRVP2) | P_Fld(0xa, SHU_DRVING1_DQDRVN2));
    vIO32WriteFldMulti(DRAMC_REG_SHU_DRVING2, P_Fld(0xa, SHU_DRVING2_DQDRVP1) | P_Fld(0xa, SHU_DRVING2_DQDRVN1)
                | P_Fld(0xa, SHU_DRVING2_CMDDRVP2) | P_Fld(0xa, SHU_DRVING2_CMDDRVN2)
                | P_Fld(0xa, SHU_DRVING2_CMDDRVP1) | P_Fld(0xa, SHU_DRVING2_CMDDRVN1));
    vIO32WriteFldMulti(DRAMC_REG_SHU_DRVING3, P_Fld(0xa, SHU_DRVING3_DQSODTP2) | P_Fld(0xa, SHU_DRVING3_DQSODTN2)
                | P_Fld(0xa, SHU_DRVING3_DQSODTP) | P_Fld(0xa, SHU_DRVING3_DQSODTN)
                | P_Fld(0xa, SHU_DRVING3_DQODTP2) | P_Fld(0xa, SHU_DRVING3_DQODTN2));
    vIO32WriteFldMulti(DRAMC_REG_SHU_DRVING4, P_Fld(0xa, SHU_DRVING4_DQODTP1) | P_Fld(0xa, SHU_DRVING4_DQODTN1)
                | P_Fld(0xa, SHU_DRVING4_CMDODTP2) | P_Fld(0xa, SHU_DRVING4_CMDODTN2)
                | P_Fld(0xa, SHU_DRVING4_CMDODTP1) | P_Fld(0xa, SHU_DRVING4_CMDODTN1));
    /*
    vIO32WriteFldMulti(DRAMC_REG_SHU2_DRVING1, P_Fld(0x14a, SHU2_DRVING1_DQSDRV2)
                | P_Fld(0x14a, SHU2_DRVING1_DQSDRV1)
                | P_Fld(0x14a, SHU2_DRVING1_DQDRV2));
    vIO32WriteFldMulti(DRAMC_REG_SHU2_DRVING2, P_Fld(0x14a, SHU2_DRVING2_DQDRV1)
                | P_Fld(0x14a, SHU2_DRVING2_CMDDRV2)
                | P_Fld(0x14a, SHU2_DRVING2_CMDDRV1));
    vIO32WriteFldMulti(DRAMC_REG_SHU2_DRVING3, P_Fld(0x14a, SHU2_DRVING3_DQSODT2)
                | P_Fld(0x14a, SHU2_DRVING3_DQSODT1)
                | P_Fld(0x14a, SHU2_DRVING3_DQODT2));
    vIO32WriteFldMulti(DRAMC_REG_SHU2_DRVING4, P_Fld(0x14a, SHU2_DRVING4_DQODT1)
                | P_Fld(0x14a, SHU2_DRVING4_CMDODT2)
                | P_Fld(0x14a, SHU2_DRVING4_CMDODT1));
    vIO32WriteFldMulti(DRAMC_REG_SHU3_DRVING1, P_Fld(0x14a, SHU3_DRVING1_DQSDRV2)
                | P_Fld(0x14a, SHU3_DRVING1_DQSDRV1)
                | P_Fld(0x14a, SHU3_DRVING1_DQDRV2));
    vIO32WriteFldMulti(DRAMC_REG_SHU3_DRVING2, P_Fld(0x14a, SHU3_DRVING2_DQDRV1)
                | P_Fld(0x14a, SHU3_DRVING2_CMDDRV2)
                | P_Fld(0x14a, SHU3_DRVING2_CMDDRV1));
    vIO32WriteFldMulti(DRAMC_REG_SHU3_DRVING3, P_Fld(0x14a, SHU3_DRVING3_DQSODT2)
                | P_Fld(0x14a, SHU3_DRVING3_DQSODT1)
                | P_Fld(0x14a, SHU3_DRVING3_DQODT2));
    vIO32WriteFldMulti(DRAMC_REG_SHU3_DRVING4, P_Fld(0x14a, SHU3_DRVING4_DQODT1)
                | P_Fld(0x14a, SHU3_DRVING4_CMDODT2)
                | P_Fld(0x14a, SHU3_DRVING4_CMDODT1));
    vIO32WriteFldMulti(DRAMC_REG_SHU4_DRVING1, P_Fld(0x14a, SHU4_DRVING1_DQSDRV2)
                | P_Fld(0x14a, SHU4_DRVING1_DQSDRV1)
                | P_Fld(0x14a, SHU4_DRVING1_DQDRV2));
    vIO32WriteFldMulti(DRAMC_REG_SHU4_DRVING2, P_Fld(0x14a, SHU4_DRVING2_DQDRV1)
                | P_Fld(0x14a, SHU4_DRVING2_CMDDRV2)
                | P_Fld(0x14a, SHU4_DRVING2_CMDDRV1));
    */
    //  *((UINT32P)(DRAMC1_AO_BASE + 0x08a8)) = 0x14a5294a;
    //  *((UINT32P)(DRAMC1_AO_BASE + 0x08ac)) = 0x14a5294a;
    //  *((UINT32P)(DRAMC1_AO_BASE + 0x08b0)) = 0x14a5294a;
    //  *((UINT32P)(DRAMC1_AO_BASE + 0x08b4)) = 0x14a5294a;
    //  *((UINT32P)(DRAMC1_AO_BASE + 0x0ea8)) = 0x14a5294a;
    //  *((UINT32P)(DRAMC1_AO_BASE + 0x0eac)) = 0x14a5294a;
    //  *((UINT32P)(DRAMC1_AO_BASE + 0x0eb0)) = 0x14a5294a;
    //  *((UINT32P)(DRAMC1_AO_BASE + 0x0eb4)) = 0x14a5294a;
    //  *((UINT32P)(DRAMC1_AO_BASE + 0x14a8)) = 0x14a5294a;
    //  *((UINT32P)(DRAMC1_AO_BASE + 0x14ac)) = 0x14a5294a;
    //  *((UINT32P)(DRAMC1_AO_BASE + 0x14b0)) = 0x14a5294a;
    //  *((UINT32P)(DRAMC1_AO_BASE + 0x14b4)) = 0x14a5294a;
    //  *((UINT32P)(DRAMC1_AO_BASE + 0x1aa8)) = 0x14a5294a;
    //  *((UINT32P)(DRAMC1_AO_BASE + 0x1aac)) = 0x14a5294a;
    vIO32WriteFldMulti(DRAMC_REG_SHUCTRL2, P_Fld(0x0, SHUCTRL2_HWSET_WLRL)
                | P_Fld(0x1, SHUCTRL2_SHU_PERIOD_GO_ZERO_CNT)
                | P_Fld(0x1, SHUCTRL2_R_DVFS_OPTION)
                | P_Fld(0x1, SHUCTRL2_R_DVFS_PARK_N)
                | P_Fld(0x1, SHUCTRL2_R_DVFS_DLL_CHA)
                | P_Fld(0xa, SHUCTRL2_R_DLL_IDLE));
    vIO32WriteFldAlign(DRAMC_REG_DVFSDLL, 0x1, DVFSDLL_DLL_LOCK_SHU_EN);
    vIO32WriteFldMulti(DRAMC_REG_DDRCONF0, P_Fld(0x1, DDRCONF0_LPDDR4EN)
                | P_Fld(0x1, DDRCONF0_BC4OTF)
                | P_Fld(0x1, DDRCONF0_BK8EN));
    vIO32WriteFldMulti(DRAMC_REG_STBCAL2, P_Fld(0x1, STBCAL2_STB_GERR_B01)
                | P_Fld(0x1, STBCAL2_STB_GERRSTOP)
                | P_Fld(0x1, EYESCAN_EYESCAN_RD_SEL_OPT));
    vIO32WriteFldAlign(DRAMC_REG_STBCAL2, 0x1, STBCAL2_STB_GERR_RST);
    vIO32WriteFldAlign(DRAMC_REG_STBCAL2, 0x0, STBCAL2_STB_GERR_RST);
    vIO32WriteFldAlign(DRAMC_REG_CLKAR, 0x1, CLKAR_PSELAR);
    vIO32WriteFldAlign(DDRPHY_B0_DQ9, 0x1, B0_DQ9_R_DMDQSIEN_RDSEL_LAT_B0);
    vIO32WriteFldAlign(DDRPHY_B1_DQ9, 0x1, B1_DQ9_R_DMDQSIEN_RDSEL_LAT_B1);
    vIO32WriteFldAlign(DDRPHY_CA_CMD10, 0x0, CA_CMD10_R_DMDQSIEN_RDSEL_LAT_CA);
    if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
    {
        vIO32WriteFldMulti(DDRPHY_MISC_CTRL0, P_Fld(0x1, MISC_CTRL0_R_STBENCMP_DIV4CK_EN)
                    | P_Fld(0x1, MISC_CTRL0_R_DMDQSIEN_FIFO_EN)
                    | P_Fld(0x1, MISC_CTRL0_R_DMSTBEN_OUTSEL)
                    | P_Fld(0xf, MISC_CTRL0_R_DMDQSIEN_SYNCOPT));
    }
    else
    {
        vIO32WriteFldMulti(DDRPHY_MISC_CTRL0, P_Fld(0x1, MISC_CTRL0_R_STBENCMP_DIV4CK_EN)
                    | P_Fld(0x1, MISC_CTRL0_R_DMDQSIEN_FIFO_EN)
                    | P_Fld(0x1, MISC_CTRL0_R_DMSTBEN_OUTSEL)
                    | P_Fld(0xf, MISC_CTRL0_R_DMDQSIEN_SYNCOPT));
    }
    //vIO32WriteFldMulti(DDRPHY_MISC_CTRL1, P_Fld(0x1, MISC_CTRL1_R_DMDA_RRESETB_E) //Already set in vDramcInit_PreSettings()
    vIO32WriteFldMulti(DDRPHY_MISC_CTRL1, P_Fld(0x1, MISC_CTRL1_R_DMDQSIENCG_EN)
                | P_Fld(0x1, MISC_CTRL1_R_DM_TX_ARCMD_OE)
#if ENABLE_PINMUX_FOR_RANK_SWAP
                | P_Fld(0x1, MISC_CTRL1_R_RK_PINMUXSWAP_EN)
#endif
                | P_Fld(0x1, MISC_CTRL1_R_DM_TX_ARCLK_OE));
    vIO32WriteFldAlign(DDRPHY_B0_RXDVS0, 1, B0_RXDVS0_R_HWSAVE_MODE_ENA_B0);
    vIO32WriteFldAlign(DDRPHY_B1_RXDVS0, 1, B1_RXDVS0_R_HWSAVE_MODE_ENA_B1);
    vIO32WriteFldAlign(DDRPHY_CA_RXDVS0, 0, CA_RXDVS0_R_HWSAVE_MODE_ENA_CA);

    vIO32WriteFldAlign(DDRPHY_CA_CMD7, 0x0, CA_CMD7_RG_TX_ARCMD_PULL_DN);
    vIO32WriteFldAlign(DDRPHY_CA_CMD7, 0x0, CA_CMD7_RG_TX_ARCS_PULL_DN); // Added by Lingyun.Wu, 11-15
    vIO32WriteFldAlign(DDRPHY_B0_DQ7, 0x0, B0_DQ7_RG_TX_ARDQ_PULL_DN_B0);
    vIO32WriteFldAlign(DDRPHY_B1_DQ7, 0x0, B1_DQ7_RG_TX_ARDQ_PULL_DN_B1);
    //vIO32WriteFldAlign(DDRPHY_CA_CMD8, 0x0, CA_CMD8_RG_TX_RRESETB_PULL_DN); //Already set in vDramcInit_PreSettings()

    //For 1:8 mode start=================
    // 1:8 mode
    vIO32WriteFldMulti(DRAMC_REG_SHU_CONF0, P_Fld(0x1, SHU_CONF0_DM64BITEN)
                | P_Fld(0x0, SHU_CONF0_FDIV2)
                | P_Fld(0x1, SHU_CONF0_FREQDIV4)
                | P_Fld(0x1, SHU_CONF0_DUALSCHEN)
                | P_Fld(0x0, SHU_CONF0_WDATRGO)
                | P_Fld(u1MaType, SHU_CONF0_MATYPE)
                | P_Fld(0x1, SHU_CONF0_BL4)
                | P_Fld(0x1, SHU_CONF0_REFTHD)
                | P_Fld(0x1, SHU_CONF0_ADVPREEN)
                | P_Fld(0x3f, SHU_CONF0_DMPGTIM));

    vIO32WriteFldAlign(DRAMC_REG_SHU_APHY_TX_PICG_CTRL, 0x5, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_DYN_GATING_SEL);
    vIO32WriteFldAlign(DRAMC_REG_SHU_SELPH_CA1, 0x0, SHU_SELPH_CA1_TXDLY_CKE);
    vIO32WriteFldAlign(DRAMC_REG_SHU_SELPH_CA2, 0x0, SHU_SELPH_CA2_TXDLY_CKE1);

#if (fcFOR_CHIP_ID == fcLafite)
    if (vGet_Div_Mode(p) == DIV4_MODE)
    {
        u1TAIL_LAT = 1;
    }
    else
 //DIV8_MODE
    {
        u1TAIL_LAT = 0;
    }

    //PICG old mode
    vIO32WriteFldMulti(DRAMC_REG_SHU_APHY_TX_PICG_CTRL, P_Fld(0x0, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQ_RK0_SEL_P0)
                | P_Fld(0x0, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQ_RK0_SEL_P1)
                | P_Fld(0x0, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_PICG_CNT));

    //PICG_DQSIEN old/new project have same settings
    vIO32WriteFldMulti(DRAMC_REG_SHU_STBCAL, P_Fld(u1TAIL_LAT, SHU_STBCAL_R1_DQSIEN_PICG_TAIL_EXT_LAT) //TAIL should be set as same value by old/new mode, no matter it is old or new mode
                | P_Fld(u1TAIL_LAT, SHU_STBCAL_R0_DQSIEN_PICG_TAIL_EXT_LAT) //TAIL should be set as same value by old/new mode, no matter it is old or new mode
                | P_Fld(u1TAIL_LAT, SHU_STBCAL_DQSIEN_RX_SELPH_OPT));

    //RX_IN_GATE_EN old mode
    vIO32WriteFldMulti(DRAMC_REG_SHU_PHY_RX_CTRL, P_Fld(0x0, SHU_PHY_RX_CTRL_RX_IN_GATE_EN_TAIL)
                | P_Fld(0x0, SHU_PHY_RX_CTRL_RX_IN_GATE_EN_HEAD));
#endif
    //For 1:8 mode end=================

    vIO32WriteFldMulti(DRAMC_REG_SHU_ODTCTRL, P_Fld(0x1, SHU_ODTCTRL_RODTE)
                | P_Fld(0x1, SHU_ODTCTRL_RODTE2)
                | P_Fld(0x1, SHU_ODTCTRL_TWODT)
                //| P_Fld(0x5, SHU_ODTCTRL_RODT) //Set in UpdateACTimingReg()
                | P_Fld(0x1, SHU_ODTCTRL_WOEN)
                | P_Fld(0x1, SHU_ODTCTRL_ROEN));
    vIO32WriteFldAlign(DDRPHY_SHU_B0_DQ7, 0x1, SHU_B0_DQ7_R_DMRODTEN_B0);
    vIO32WriteFldAlign(DDRPHY_SHU_B1_DQ7, 0x1, SHU_B1_DQ7_R_DMRODTEN_B1);
    vIO32WriteFldAlign(DRAMC_REG_REFCTRL0, 0x5, REFCTRL0_REF_PREGATE_CNT);
    vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_CA1, P_Fld(0x0, SHU_SELPH_CA1_TXDLY_CS1)
                | P_Fld(0x0, SHU_SELPH_CA1_TXDLY_RAS)
                | P_Fld(0x0, SHU_SELPH_CA1_TXDLY_CAS)
                | P_Fld(0x0, SHU_SELPH_CA1_TXDLY_WE)
                | P_Fld(0x0, SHU_SELPH_CA1_TXDLY_RESET)
                | P_Fld(0x0, SHU_SELPH_CA1_TXDLY_ODT)
                | P_Fld(0x0, SHU_SELPH_CA1_TXDLY_CS));

    if (p->frequency < 1333)
        u1TXDLY_CMD = 0x7;
    else if (p->frequency < 1600)
        u1TXDLY_CMD = 0x8;
    else if (p->frequency < 1866)
        u1TXDLY_CMD = 0x9;
    else if (p->frequency < 2133)
        u1TXDLY_CMD = 0xA;
    else
        u1TXDLY_CMD = 0xB;

    vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_CA2, P_Fld(u1TXDLY_CMD, SHU_SELPH_CA2_TXDLY_CMD)
                | P_Fld(0x0, SHU_SELPH_CA2_TXDLY_BA2)
                | P_Fld(0x0, SHU_SELPH_CA2_TXDLY_BA1)
                | P_Fld(0x0, SHU_SELPH_CA2_TXDLY_BA0));
    vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_CA3, P_Fld(0x0, SHU_SELPH_CA3_TXDLY_RA7)
                | P_Fld(0x0, SHU_SELPH_CA3_TXDLY_RA6)
                | P_Fld(0x0, SHU_SELPH_CA3_TXDLY_RA5)
                | P_Fld(0x0, SHU_SELPH_CA3_TXDLY_RA4)
                | P_Fld(0x0, SHU_SELPH_CA3_TXDLY_RA3)
                | P_Fld(0x0, SHU_SELPH_CA3_TXDLY_RA2)
                | P_Fld(0x0, SHU_SELPH_CA3_TXDLY_RA1)
                | P_Fld(0x0, SHU_SELPH_CA3_TXDLY_RA0));
    vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_CA4, P_Fld(0x0, SHU_SELPH_CA4_TXDLY_RA15)
                | P_Fld(0x0, SHU_SELPH_CA4_TXDLY_RA14)
                | P_Fld(0x0, SHU_SELPH_CA4_TXDLY_RA13)
                | P_Fld(0x0, SHU_SELPH_CA4_TXDLY_RA12)
                | P_Fld(0x0, SHU_SELPH_CA4_TXDLY_RA11)
                | P_Fld(0x0, SHU_SELPH_CA4_TXDLY_RA10)
                | P_Fld(0x0, SHU_SELPH_CA4_TXDLY_RA9)
                | P_Fld(0x0, SHU_SELPH_CA4_TXDLY_RA8));
    vIO32WriteFldAlign(DRAMC_REG_SHU_SELPH_CA5, 0x0, SHU_SELPH_CA5_DLY_ODT);
    vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_DQS0, P_Fld(0x3, SHU_SELPH_DQS0_TXDLY_OEN_DQS3)
                | P_Fld(0x3, SHU_SELPH_DQS0_TXDLY_OEN_DQS2)
                | P_Fld(0x3, SHU_SELPH_DQS0_TXDLY_OEN_DQS1)
                | P_Fld(0x3, SHU_SELPH_DQS0_TXDLY_OEN_DQS0)
                | P_Fld(0x3, SHU_SELPH_DQS0_TXDLY_DQS3)
                | P_Fld(0x3, SHU_SELPH_DQS0_TXDLY_DQS2)
                | P_Fld(0x3, SHU_SELPH_DQS0_TXDLY_DQS1)
                | P_Fld(0x3, SHU_SELPH_DQS0_TXDLY_DQS0));
    vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_DQS1, P_Fld(0x2, SHU_SELPH_DQS1_DLY_OEN_DQS3)
                | P_Fld(0x2, SHU_SELPH_DQS1_DLY_OEN_DQS2)
                | P_Fld(0x2, SHU_SELPH_DQS1_DLY_OEN_DQS1)
                | P_Fld(0x2, SHU_SELPH_DQS1_DLY_OEN_DQS0)
                | P_Fld(0x5, SHU_SELPH_DQS1_DLY_DQS3)
                | P_Fld(0x5, SHU_SELPH_DQS1_DLY_DQS2)
                | P_Fld(0x5, SHU_SELPH_DQS1_DLY_DQS1)
                | P_Fld(0x5, SHU_SELPH_DQS1_DLY_DQS0));
    vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ0, P_Fld(0x3, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ3)
                | P_Fld(0x3, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ2)
                | P_Fld(0x3, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ1)
                | P_Fld(0x3, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ0)
                | P_Fld(0x3, SHURK0_SELPH_DQ0_TXDLY_DQ3)
                | P_Fld(0x3, SHURK0_SELPH_DQ0_TXDLY_DQ2)
                | P_Fld(0x3, SHURK0_SELPH_DQ0_TXDLY_DQ1)
                | P_Fld(0x3, SHURK0_SELPH_DQ0_TXDLY_DQ0));
    vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ1, P_Fld(0x3, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM3)
                | P_Fld(0x3, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM2)
                | P_Fld(0x3, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM1)
                | P_Fld(0x3, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM0)
                | P_Fld(0x3, SHURK0_SELPH_DQ1_TXDLY_DQM3)
                | P_Fld(0x3, SHURK0_SELPH_DQ1_TXDLY_DQM2)
                | P_Fld(0x3, SHURK0_SELPH_DQ1_TXDLY_DQM1)
                | P_Fld(0x3, SHURK0_SELPH_DQ1_TXDLY_DQM0));
    vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ2, P_Fld(0x2, SHURK0_SELPH_DQ2_DLY_OEN_DQ3)
                | P_Fld(0x2, SHURK0_SELPH_DQ2_DLY_OEN_DQ2)
                | P_Fld(0x2, SHURK0_SELPH_DQ2_DLY_OEN_DQ1)
                | P_Fld(0x2, SHURK0_SELPH_DQ2_DLY_OEN_DQ0)
                | P_Fld(0x6, SHURK0_SELPH_DQ2_DLY_DQ3)
                | P_Fld(0x6, SHURK0_SELPH_DQ2_DLY_DQ2)
                | P_Fld(0x6, SHURK0_SELPH_DQ2_DLY_DQ1)
                | P_Fld(0x6, SHURK0_SELPH_DQ2_DLY_DQ0));
    vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ3, P_Fld(0x2, SHURK0_SELPH_DQ3_DLY_OEN_DQM3)
                | P_Fld(0x2, SHURK0_SELPH_DQ3_DLY_OEN_DQM2)
                | P_Fld(0x2, SHURK0_SELPH_DQ3_DLY_OEN_DQM1)
                | P_Fld(0x2, SHURK0_SELPH_DQ3_DLY_OEN_DQM0)
                | P_Fld(0x6, SHURK0_SELPH_DQ3_DLY_DQM3)
                | P_Fld(0x6, SHURK0_SELPH_DQ3_DLY_DQM2)
                | P_Fld(0x6, SHURK0_SELPH_DQ3_DLY_DQM1)
                | P_Fld(0x6, SHURK0_SELPH_DQ3_DLY_DQM0));
    vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQ0, P_Fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ3)
                | P_Fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ2)
                | P_Fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ1)
                | P_Fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ0)
                | P_Fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1DQ3)
                | P_Fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1DQ2)
                | P_Fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1DQ1)
                | P_Fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1DQ0));
    vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQ1, P_Fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM3)
                | P_Fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM2)
                | P_Fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM1)
                | P_Fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM0)
                | P_Fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1DQM3)
                | P_Fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1DQM2)
                | P_Fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1DQM1)
                | P_Fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1DQM0));
    vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQ2, P_Fld(0x2, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ3)
                | P_Fld(0x2, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ2)
                | P_Fld(0x2, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ1)
                | P_Fld(0x2, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ0)
                | P_Fld(0x6, SHURK1_SELPH_DQ2_DLY_R1DQ3)
                | P_Fld(0x6, SHURK1_SELPH_DQ2_DLY_R1DQ2)
                | P_Fld(0x6, SHURK1_SELPH_DQ2_DLY_R1DQ1)
                | P_Fld(0x6, SHURK1_SELPH_DQ2_DLY_R1DQ0));
    vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQ3, P_Fld(0x2, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM3)
                | P_Fld(0x2, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM2)
                | P_Fld(0x2, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM1)
                | P_Fld(0x2, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM0)
                | P_Fld(0x6, SHURK1_SELPH_DQ3_DLY_R1DQM3)
                | P_Fld(0x6, SHURK1_SELPH_DQ3_DLY_R1DQM2)
                | P_Fld(0x6, SHURK1_SELPH_DQ3_DLY_R1DQM1)
                | P_Fld(0x6, SHURK1_SELPH_DQ3_DLY_R1DQM0));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B1_DQ7, P_Fld(0x1a, SHU_R0_B1_DQ7_RK0_ARPI_DQM_B1)
                | P_Fld(0x1a, SHU_R0_B1_DQ7_RK0_ARPI_DQ_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R0_B0_DQ7, P_Fld(0x1a, SHU_R0_B0_DQ7_RK0_ARPI_DQM_B0)
                | P_Fld(0x1a, SHU_R0_B0_DQ7_RK0_ARPI_DQ_B0));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B1_DQ7, P_Fld(0x14, SHU_R1_B1_DQ7_RK1_ARPI_DQM_B1)
                | P_Fld(0x14, SHU_R1_B1_DQ7_RK1_ARPI_DQ_B1));
    vIO32WriteFldMulti(DDRPHY_SHU_R1_B0_DQ7, P_Fld(0x14, SHU_R1_B0_DQ7_RK1_ARPI_DQM_B0)
                | P_Fld(0x14, SHU_R1_B0_DQ7_RK1_ARPI_DQ_B0));

    mcDELAY_US(1);

    vIO32WriteFldAlign(DDRPHY_B1_DQ9, 0x1, B1_DQ9_RG_RX_ARDQS0_DQSIENMODE_B1);
    vIO32WriteFldAlign(DDRPHY_B0_DQ9, 0x1, B0_DQ9_RG_RX_ARDQS0_DQSIENMODE_B0);
    vIO32WriteFldAlign(DDRPHY_B0_DQ6, 0x2, B0_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B0);
    vIO32WriteFldAlign(DDRPHY_B1_DQ6, 0x2, B1_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B1);
    vIO32WriteFldAlign(DRAMC_REG_STBCAL, 0x1, STBCAL_DQSIENMODE);
    vIO32WriteFldMulti(DRAMC_REG_SREFCTRL, P_Fld(0x0, SREFCTRL_SREF_HW_EN)
                | P_Fld(0x8, SREFCTRL_SREFDLY));
    vIO32WriteFldMulti(DRAMC_REG_SHU_CKECTRL, P_Fld(0x3, SHU_CKECTRL_SREF_CK_DLY)
                | P_Fld(0x3, SHU_CKECTRL_TCKESRX));
                //| P_Fld(0x3, SHU_CKECTRL_CKEPRD));
    vIO32WriteFldMulti(DRAMC_REG_SHU_PIPE, P_Fld(0x1, SHU_PIPE_READ_START_EXTEND1)
                | P_Fld(0x1, SHU_PIPE_DLE_LAST_EXTEND1));
    vIO32WriteFldMulti(DRAMC_REG_CKECTRL, P_Fld(0x1, CKECTRL_CKEON)
                | P_Fld(0x1, CKECTRL_CKETIMER_SEL));
    vIO32WriteFldMulti(DRAMC_REG_RKCFG, P_Fld(0x1, RKCFG_CKE2RANK)
                | P_Fld(0x1, RKCFG_CKE2RANK_OPT2)); // Darren for CKE dependent
    if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
    {
        vIO32WriteFldMulti(DRAMC_REG_SHU_CONF2, P_Fld(0x1, SHU_CONF2_WPRE2T)
                    | P_Fld(0x7, SHU_CONF2_DCMDLYREF));
                    //| P_Fld(0x64, SHU_CONF2_FSPCHG_PRDCNT)); //ACTiming related -> set in UpdateACTiming_Reg()
        vIO32WriteFldAlign(DRAMC_REG_SPCMDCTRL, 0x1, SPCMDCTRL_CLR_EN);
        //vIO32WriteFldAlign(DRAMC_REG_SHU_SCINTV, 0xf, SHU_SCINTV_MRW_INTV); (Set in UpdateACTimingReg())
        vIO32WriteFldAlign(DRAMC_REG_SHUCTRL1, 0x40, SHUCTRL1_FC_PRDCNT);
    }
    else
    {
        vIO32WriteFldMulti(DRAMC_REG_SHU_CONF2, P_Fld(0x1, SHU_CONF2_WPRE2T)
                    | P_Fld(0x7, SHU_CONF2_DCMDLYREF));
                    //| P_Fld(0x64, SHU_CONF2_FSPCHG_PRDCNT)); //ACTiming related -> set in UpdateACTiming_Reg()
        vIO32WriteFldAlign(DRAMC_REG_SPCMDCTRL, 0x1, SPCMDCTRL_CLR_EN);
        //vIO32WriteFldAlign(DRAMC_REG_SHU_SCINTV, 0xf, SHU_SCINTV_MRW_INTV); (Set in UpdateACTimingReg())
        vIO32WriteFldAlign(DRAMC_REG_SHUCTRL1, 0x40, SHUCTRL1_FC_PRDCNT);
    }
    vIO32WriteFldAlign(DRAMC_REG_SHUCTRL, 0x1, SHUCTRL_LPSM_BYPASS_B);
    vIO32WriteFldMulti(DRAMC_REG_REFCTRL1, P_Fld(0x0, REFCTRL1_SREF_PRD_OPT) | P_Fld(0x0, REFCTRL1_PSEL_OPT1) | P_Fld(0x0, REFCTRL1_PSEL_OPT2) | P_Fld(0x0, REFCTRL1_PSEL_OPT3));
    //vIO32WriteFldAlign(DDRPHY_SHU_PLL4, 0xfe, SHU_PLL4_RG_RPHYPLL_RESERVED);
    //vIO32WriteFldAlign(DDRPHY_SHU_PLL6, 0xfe, SHU_PLL6_RG_RCLRPLL_RESERVED);
    vIO32WriteFldMulti(DRAMC_REG_REFRATRE_FILTER, P_Fld(0x1, REFRATRE_FILTER_PB2AB_OPT) | P_Fld(0x0, REFRATRE_FILTER_PB2AB_OPT1));

#if !APPLY_LP4_POWER_INIT_SEQUENCE
    vIO32WriteFldAlign(DDRPHY_MISC_CTRL1, 0x1, MISC_CTRL1_R_DMDA_RRESETB_I);
#endif
    vIO32WriteFldAlign(DRAMC_REG_DRAMCTRL, 0x0, DRAMCTRL_CLKWITRFC);
    vIO32WriteFldMulti(DRAMC_REG_MISCTL0, P_Fld(0x1, MISCTL0_REFP_ARB_EN2)
                | P_Fld(0x1, MISCTL0_PRE_DLE_VLD_OPT) // MISCTL0_PRE_DLE_VLD_OPT from Chris review
                | P_Fld(0x1, MISCTL0_PBC_ARB_EN)
                | P_Fld(0x1, MISCTL0_REFA_ARB_EN2));
    vIO32WriteFldMulti(DRAMC_REG_PERFCTL0, P_Fld(0x1, PERFCTL0_MWHPRIEN)
                //| P_Fld(0x1, PERFCTL0_RWSPLIT) //Set in UpdateInitialSettings_LP4()
                | P_Fld(0x1, PERFCTL0_WFLUSHEN)
                | P_Fld(0x1, PERFCTL0_EMILLATEN)
                | P_Fld(0x1, PERFCTL0_RWAGEEN)
                | P_Fld(0x1, PERFCTL0_RWLLATEN)
                | P_Fld(0x1, PERFCTL0_RWHPRIEN)
                | P_Fld(0x1, PERFCTL0_RWOFOEN)
                | P_Fld(0x1, PERFCTL0_DISRDPHASE1));
    vIO32WriteFldAlign(DRAMC_REG_ARBCTL, 0x80, ARBCTL_MAXPENDCNT);
    vIO32WriteFldMulti(DRAMC_REG_PADCTRL, P_Fld(0x1, PADCTRL_DQIENLATEBEGIN)
                | P_Fld(0x1, PADCTRL_DQIENQKEND));
    vIO32WriteFldAlign(DRAMC_REG_DRAMC_PD_CTRL, 0x1, DRAMC_PD_CTRL_DCMREF_OPT);
    vIO32WriteFldMulti(DRAMC_REG_CLKCTRL, P_Fld(0x1, CLKCTRL_CLK_EN_1)
                | P_Fld(0x1, CLKCTRL_CLK_EN_0)); // CLK_EN_0 from Jouling review
    vIO32WriteFldMulti(DRAMC_REG_REFCTRL0, P_Fld(0x4, REFCTRL0_DISBYREFNUM)
                | P_Fld(0x1, REFCTRL0_DLLFRZ));
#if 0 //CATRAIN_INTV, CATRAINLAT: Correct values are set in UpdateACTimingReg( )
    vIO32WriteFldMulti(DRAMC_REG_CATRAINING1, P_Fld(0xff, CATRAINING1_CATRAIN_INTV)
                | P_Fld(0x0, CATRAINING1_CATRAINLAT));
#endif
#if 0
    vIO32WriteFldMulti(DRAMC_REG_SHU_RANKCTL, P_Fld(0x6, SHU_RANKCTL_RANKINCTL_PHY)
                | P_Fld(0x4, SHU_RANKCTL_RANKINCTL_ROOT1)
                | P_Fld(0x4, SHU_RANKCTL_RANKINCTL));
#endif

    //vIO32WriteFldAlign(DRAMC_REG_SHU_STBCAL, 0x1, SHU_STBCAL_DMSTBLAT);
    //vIO32WriteFldAlign(DRAMC_REG_SHURK0_DQSCTL, 0x5, SHURK0_DQSCTL_DQSINCTL); //DQSINCTL: set in UpdateACTimingReg()
    //vIO32WriteFldAlign(DRAMC_REG_SHURK1_DQSCTL, 0x5, SHURK1_DQSCTL_R1DQSINCTL); //Set in UpdateACTimingReg()

    mcDELAY_US(2);
    #if LEGACY_GATING_DLY
    LegacyGatingDlyLP4_DDR3200(p);
    #endif
    if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
    {
    }
    else
    {
        vIO32WriteFldMulti(DRAMC_REG_SHURK0_DQSIEN, P_Fld(0x19, SHURK0_DQSIEN_R0DQS1IEN)
                    | P_Fld(0x19, SHURK0_DQSIEN_R0DQS0IEN));
        vIO32WriteFldMulti(DRAMC_REG_SHURK1_DQSIEN, P_Fld(0x0, SHURK1_DQSIEN_R1DQS3IEN)
                    | P_Fld(0x0, SHURK1_DQSIEN_R1DQS2IEN)
                    | P_Fld(0x1b, SHURK1_DQSIEN_R1DQS1IEN)
                    | P_Fld(0x1b, SHURK1_DQSIEN_R1DQS0IEN));
    }
    //               41536 === over_write_setting_begin ===
    //               41536 === over_write_setting_end ===
    vIO32WriteFldAlign(DRAMC_REG_DRAMCTRL, 0x1, DRAMCTRL_PREALL_OPTION);
    vIO32WriteFldAlign(DRAMC_REG_ZQCS, 0x56, ZQCS_ZQCSOP);

    mcDELAY_US(1);

    vIO32WriteFldAlign(DRAMC_REG_SHU_CONF1, 0x1, SHU_CONF1_TREFBWIG);
    vIO32WriteFldAlign(DRAMC_REG_SHU_CONF3, 0xff, SHU_CONF3_REFRCNT);
    vIO32WriteFldAlign(DRAMC_REG_REFCTRL0, 0x1, REFCTRL0_REFFRERUN);
    vIO32WriteFldAlign(DRAMC_REG_SREFCTRL, 0x1, SREFCTRL_SREF_HW_EN);
    vIO32WriteFldAlign(DRAMC_REG_MPC_OPTION, 0x1, MPC_OPTION_MPCRKEN);
    vIO32WriteFldAlign(DRAMC_REG_DRAMC_PD_CTRL, 0x1, DRAMC_PD_CTRL_PHYCLKDYNGEN);//Move to DCM off setting
    vIO32WriteFldAlign(DRAMC_REG_DRAMC_PD_CTRL, 0x1, DRAMC_PD_CTRL_DCMEN);//Move to DCM off setting
    vIO32WriteFldMulti(DRAMC_REG_EYESCAN, P_Fld(0x0, EYESCAN_RX_DQ_EYE_SEL)
                | P_Fld(0x0, EYESCAN_RG_RX_EYE_SCAN_EN));
    vIO32WriteFldMulti(DRAMC_REG_STBCAL1, P_Fld(0x1, STBCAL1_STBCNT_LATCH_EN)
                | P_Fld(0x1, STBCAL1_STBENCMPEN));
    vIO32WriteFldAlign(DRAMC_REG_TEST2_1, 0x10000, TEST2_1_TEST2_BASE);
#if (FOR_DV_SIMULATION_USED == 1 || SW_CHANGE_FOR_SIMULATION == 1)
    //because cmd_len=1 has bug with byte mode, so need to set cmd_len=0, then it will cost more time to do a pattern test
    //workaround: reduce TEST2_OFF to make less test agent cmd. make lpddr4-1600 can finish in 60us (Mengru)
    vIO32WriteFldAlign(DRAMC_REG_TEST2_2, 0x20, TEST2_2_TEST2_OFF);
#else
    vIO32WriteFldAlign(DRAMC_REG_TEST2_2, 0x400, TEST2_2_TEST2_OFF);
#endif
    vIO32WriteFldMulti(DRAMC_REG_TEST2_3, P_Fld(0x1, TEST2_3_TEST2WREN2_HW_EN)
                | P_Fld(0x1, TEST2_3_TESTAUDPAT));
    vIO32WriteFldAlign(DRAMC_REG_RSTMASK, 0x0, RSTMASK_DAT_SYNC_MASK);
    vIO32WriteFldAlign(DRAMC_REG_RSTMASK, 0x0, RSTMASK_PHY_SYNC_MASK);

    mcDELAY_US(1);

    vIO32WriteFldMulti(DRAMC_REG_HW_MRR_FUN, P_Fld(0x0, HW_MRR_FUN_TRPMRR_EN)
                    | P_Fld(0x0, HW_MRR_FUN_TRCDMRR_EN) | P_Fld(0x0, HW_MRR_FUN_TMRR_ENA)
                    | P_Fld(0x0, HW_MRR_FUN_MANTMRR_EN)); // from YH Tsai review for samsung MRR/Read cmd issue
    if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
    {
        vIO32WriteFldAlign(DRAMC_REG_PERFCTL0, 0x1, PERFCTL0_WRFIO_MODE2);
                    //| P_Fld(0x0, PERFCTL0_RWSPLIT)); //Set in UpdateInitialSettings_LP4()
        vIO32WriteFldMulti(DRAMC_REG_PERFCTL0, P_Fld(0x1, PERFCTL0_REORDEREN)
                    | P_Fld(0x0, PERFCTL0_REORDER_MODE));
        vIO32WriteFldAlign(DRAMC_REG_RSTMASK, 0x0, RSTMASK_GT_SYNC_MASK);
        vIO32WriteFldAlign(DRAMC_REG_RKCFG, 0x1, RKCFG_DQSOSC2RK);
        vIO32WriteFldAlign(DRAMC_REG_SPCMDCTRL, 0x1, SPCMDCTRL_REFR_BLOCKEN);
        vIO32WriteFldAlign(DRAMC_REG_EYESCAN, 0x0, EYESCAN_RG_RX_MIOCK_JIT_EN);
    }
    else
    {
        vIO32WriteFldAlign(DRAMC_REG_DRAMCTRL, 0x0, DRAMCTRL_CTOREQ_HPRI_OPT);
        vIO32WriteFldMulti(DRAMC_REG_PERFCTL0, P_Fld(0x1, PERFCTL0_REORDEREN)
                    | P_Fld(0x0, PERFCTL0_REORDER_MODE));
        vIO32WriteFldAlign(DRAMC_REG_SPCMDCTRL, 0x1, SPCMDCTRL_REFR_BLOCKEN);
        vIO32WriteFldAlign(DRAMC_REG_RSTMASK, 0x0, RSTMASK_GT_SYNC_MASK);
        vIO32WriteFldAlign(DRAMC_REG_RKCFG, 0x1, RKCFG_DQSOSC2RK);
        vIO32WriteFldAlign(DRAMC_REG_MPC_OPTION, 1, MPC_OPTION_MPCRKEN);
        vIO32WriteFldAlign(DRAMC_REG_EYESCAN, 0x0, EYESCAN_RG_RX_MIOCK_JIT_EN);
        vIO32WriteFldAlign(DDRPHY_SHU_B0_DQ7, 0x1, SHU_B0_DQ7_R_DMDQMDBI_SHU_B0);
        vIO32WriteFldAlign(DDRPHY_SHU_B1_DQ7, 0x1, SHU_B1_DQ7_R_DMDQMDBI_SHU_B1);
        vIO32WriteFldAlign(DRAMC_REG_SHU_RANKCTL, 0x4, SHU_RANKCTL_RANKINCTL);
        vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ0, P_Fld(0x2, SHURK0_SELPH_DQ0_TXDLY_DQ1)
                    | P_Fld(0x2, SHURK0_SELPH_DQ0_TXDLY_DQ0));
        vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ1, P_Fld(0x2, SHURK0_SELPH_DQ1_TXDLY_DQM1)
                    | P_Fld(0x2, SHURK0_SELPH_DQ1_TXDLY_DQM0));
        vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQ0, P_Fld(0x2, SHURK1_SELPH_DQ0_TX_DLY_R1DQ1)
                    | P_Fld(0x2, SHURK1_SELPH_DQ0_TX_DLY_R1DQ0));
        vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQ1, P_Fld(0x2, SHURK1_SELPH_DQ1_TX_DLY_R1DQM1)
                    | P_Fld(0x2, SHURK1_SELPH_DQ1_TX_DLY_R1DQM0));
    }
#if ENABLE_TMRRI_NEW_MODE
    vIO32WriteFldAlign(DRAMC_REG_SPCMDCTRL, 0x0, SPCMDCTRL_REFR_BLOCKEN);
    vIO32WriteFldAlign(DRAMC_REG_HW_MRR_FUN, 0x1, HW_MRR_FUN_TMRR_ENA);
#endif
    mcDELAY_US(5);

    vIO32WriteFldAlign(DRAMC_REG_STBCAL1, 0x3, STBCAL1_STBCAL_FILTER);
    vIO32WriteFldAlign(DRAMC_REG_STBCAL1, 0x1, STBCAL1_STBCAL_FILTER);
    vIO32WriteFldMulti(DRAMC_REG_STBCAL, P_Fld(0x1, STBCAL_STB_DQIEN_IG)
                | P_Fld(0x1, STBCAL_PICHGBLOCK_NORD)
                | P_Fld(0x0, STBCAL_STBCALEN)
                | P_Fld(0x0, STBCAL_STB_SELPHYCALEN)
                | P_Fld(0x1, STBCAL_PIMASK_RKCHG_OPT));
    vIO32WriteFldAlign(DRAMC_REG_STBCAL1, 0x1, STBCAL1_STB_SHIFT_DTCOUT_IG);
    vIO32WriteFldMulti(DRAMC_REG_SHU_DQSG, P_Fld(0x9, SHU_DQSG_STB_UPDMASKCYC)
                | P_Fld(0x1, SHU_DQSG_STB_UPDMASK_EN));
    vIO32WriteFldAlign(DDRPHY_MISC_CTRL0, 0x0, MISC_CTRL0_R_DMDQSIEN_SYNCOPT);
    vIO32WriteFldAlign(DRAMC_REG_SHU_STBCAL, 0x1, SHU_STBCAL_DQSG_MODE);
    vIO32WriteFldAlign(DRAMC_REG_STBCAL, 0x1, STBCAL_SREF_DQSGUPD);
    //M17_Remap:vIO32WriteFldAlign(DDRPHY_MISC_CTRL1, 0x0, MISC_CTRL1_R_DMDQMDBI);
    /* RX Tracking DQM SM enable (actual values are set in DramcRxInputDelayTrackingHW()) */
#if ENABLE_RX_TRACKING
    vIO32WriteFldAlign(DDRPHY_SHU_B0_DQ7, p->DBI_R_onoff[p->dram_fsp], SHU_B0_DQ7_R_DMRXTRACK_DQM_EN_B0);
    vIO32WriteFldAlign(DDRPHY_SHU_B1_DQ7, p->DBI_R_onoff[p->dram_fsp], SHU_B1_DQ7_R_DMRXTRACK_DQM_EN_B1);
#else
    vIO32WriteFldAlign(DDRPHY_SHU_B0_DQ7, 0, SHU_B0_DQ7_R_DMRXTRACK_DQM_EN_B0);
    vIO32WriteFldAlign(DDRPHY_SHU_B1_DQ7, 0, SHU_B1_DQ7_R_DMRXTRACK_DQM_EN_B1);
#endif
//Sylv_ia MP setting should set SHU_STBCAL_DMSTBLAT as 0x2 (review by HJ Huang)
    vIO32WriteFldMulti(DRAMC_REG_SHU_STBCAL, P_Fld(0x1, SHU_STBCAL_PICGLAT)
                | P_Fld(0x2, SHU_STBCAL_DMSTBLAT));
    vIO32WriteFldMulti(DRAMC_REG_REFCTRL1, P_Fld(0x1, REFCTRL1_REF_QUE_AUTOSAVE_EN)
                | P_Fld(0x1, REFCTRL1_SLEFREF_AUTOSAVE_EN));
    vIO32WriteFldMulti(DRAMC_REG_DQSOSCR, P_Fld(0x1, DQSOSCR_SREF_TXPI_RELOAD_OPT)
                | P_Fld(0x1, DQSOSCR_SREF_TXUI_RELOAD_OPT));
    vIO32WriteFldMulti(DRAMC_REG_RSTMASK, P_Fld(0x0, RSTMASK_DVFS_SYNC_MASK)
                | P_Fld(0x0, RSTMASK_GT_SYNC_MASK_FOR_PHY)
                | P_Fld(0x0, RSTMASK_DVFS_SYNC_MASK_FOR_PHY));
    vIO32WriteFldAlign(DRAMC_REG_RKCFG, 0x1, RKCFG_RKMODE);

#if !APPLY_LP4_POWER_INIT_SEQUENCE
    vIO32WriteFldMulti(DRAMC_REG_CKECTRL, P_Fld(0x1, CKECTRL_CKEFIXON)
                | P_Fld(0x1, CKECTRL_CKE1FIXON));
#endif

    mcDELAY_US(12);

    ///TODO: DDR3200
    //if(p->frequency==1600)
    {
#if 0
        //               60826 ===dramc_shu1_lp4_3200 begin===
        if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
        {
                vIO32WriteFldMulti(DRAMC_REG_SHU_RANKCTL, P_Fld(0x5, SHU_RANKCTL_RANKINCTL_PHY)
                            | P_Fld(0x3, SHU_RANKCTL_RANKINCTL_ROOT1)
                            | P_Fld(0x3, SHU_RANKCTL_RANKINCTL));
        }
        else
        {
                vIO32WriteFldMulti(DRAMC_REG_SHU_RANKCTL, P_Fld(0x6, SHU_RANKCTL_RANKINCTL_PHY)
                            | P_Fld(0x4, SHU_RANKCTL_RANKINCTL_ROOT1));
        }
#endif
#if LEGACY_TX_TRACK
        LegacyTxTrackLP4_DDR3200(p);
#endif

#if ENABLE_WRITE_POST_AMBLE_1_POINT_5_TCK
        vIO32WriteFldAlign(DRAMC_REG_SHU_WODT, p->dram_fsp, SHU_WODT_WPST1P5T); //Set write post-amble by FSP with MR3
#else
        vIO32WriteFldAlign(DRAMC_REG_SHU_WODT, 0x0, SHU_WODT_WPST1P5T); //Set write post-amble by FSP with MR3
#endif

        //vIO32WriteFldAlign(DRAMC_REG_SHU_HWSET_MR2, 0x2d, SHU_HWSET_MR2_HWSET_MR2_OP);
        if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
        {
                vIO32WriteFldMulti(DRAMC_REG_SHURK0_DQSIEN, P_Fld(0x19, SHURK0_DQSIEN_R0DQS1IEN)
                            | P_Fld(0x19, SHURK0_DQSIEN_R0DQS0IEN));
        }
        vIO32WriteFldMulti(DRAMC_REG_SHURK0_PI, P_Fld(0x1a, SHURK0_PI_RK0_ARPI_DQM_B1)
                    | P_Fld(0x1a, SHURK0_PI_RK0_ARPI_DQM_B0)
                    | P_Fld(0x1a, SHURK0_PI_RK0_ARPI_DQ_B1)
                    | P_Fld(0x1a, SHURK0_PI_RK0_ARPI_DQ_B0));
        vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ2, P_Fld(0x4, SHURK0_SELPH_DQ2_DLY_OEN_DQ3)
                    | P_Fld(0x4, SHURK0_SELPH_DQ2_DLY_OEN_DQ2)
                    | P_Fld(0x4, SHURK0_SELPH_DQ2_DLY_OEN_DQ1)
                    | P_Fld(0x4, SHURK0_SELPH_DQ2_DLY_OEN_DQ0));
        vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ3, P_Fld(0x4, SHURK0_SELPH_DQ3_DLY_OEN_DQM3)
                    | P_Fld(0x4, SHURK0_SELPH_DQ3_DLY_OEN_DQM2)
                    | P_Fld(0x4, SHURK0_SELPH_DQ3_DLY_OEN_DQM1)
                    | P_Fld(0x4, SHURK0_SELPH_DQ3_DLY_OEN_DQM0));
        if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
        {
                vIO32WriteFldMulti(DRAMC_REG_SHURK1_DQSIEN, P_Fld(0x0, SHURK1_DQSIEN_R1DQS3IEN)
                            | P_Fld(0x0, SHURK1_DQSIEN_R1DQS2IEN)
                            | P_Fld(0x1b, SHURK1_DQSIEN_R1DQS1IEN)
                            | P_Fld(0x1b, SHURK1_DQSIEN_R1DQS0IEN));
        }
        vIO32WriteFldMulti(DRAMC_REG_SHURK1_PI, P_Fld(0x14, SHURK1_PI_RK1_ARPI_DQM_B1)
                    | P_Fld(0x14, SHURK1_PI_RK1_ARPI_DQM_B0)
                    | P_Fld(0x14, SHURK1_PI_RK1_ARPI_DQ_B1)
                    | P_Fld(0x14, SHURK1_PI_RK1_ARPI_DQ_B0));
        vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQ2, P_Fld(0x4, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ3)
                    | P_Fld(0x4, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ2)
                    | P_Fld(0x4, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ1)
                    | P_Fld(0x4, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ0));
        vIO32WriteFldMulti(DRAMC_REG_SHURK1_SELPH_DQ3, P_Fld(0x4, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM3)
                    | P_Fld(0x4, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM2)
                    | P_Fld(0x4, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM1)
                    | P_Fld(0x4, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM0));
        vIO32WriteFldMulti(DRAMC_REG_SHU_DQSG_RETRY, P_Fld(0x0, SHU_DQSG_RETRY_R_XSR_RETRY_SPM_MODE)
                    | P_Fld(0x0, SHU_DQSG_RETRY_R_DDR1866_PLUS)); //La_fite MP setting = 0
#if LEGACY_TDQSCK_PRECAL
        LegacyPreCalLP4_DDR3200(p);
#endif
        //               61832 ===dramc_shu1_lp4_3200 end===


        //               66870 ===ddrphy_shu1_lp4_3200_CHA begin===
        if (vGet_Dram_CBT_Mode(p) == CBT_BYTE_MODE1)
        {
                vIO32WriteFldAlign(DDRPHY_SHU_B0_DQ7, 0x1, SHU_B0_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B0);
                vIO32WriteFldAlign(DDRPHY_SHU_B1_DQ7, 0x1, SHU_B1_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B1);
        }
        else
        {
                vIO32WriteFldMulti(DDRPHY_SHU_B0_DQ7, P_Fld(0x1, SHU_B0_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B0)
                            | P_Fld(0x1, SHU_B0_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B0));
                vIO32WriteFldMulti(DDRPHY_SHU_B1_DQ7, P_Fld(0x1, SHU_B1_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B1)
                            | P_Fld(0x1, SHU_B1_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B1));
//francis remove : it will make CLRPLL frequency wrong!
//francis remove                vIO32WriteFldMulti(DDRPHY_SHU_PLL7, P_Fld(0x3d00, SHU_PLL7_RG_RCLRPLL_SDM_PCW)
//francis remove                            | P_Fld(0x1, SHU_PLL7_RG_RCLRPLL_SDM_PCW_CHG));
        }
        //               67761 ===ddrphy_shu1_lp4_3200_CHA end===

        //NOT included in parsing tool
        vIO32WriteFldAlign(DRAMC_REG_SHU_DQS2DQ_TX, 0x0, SHU_DQS2DQ_TX_OE2DQ_OFFSET);
    }
    ///TODO: DDR3733
    if (p->freqGroup == 2133)
    {
        DramcSetting_Olympus_LP4_ByteMode_DDR4266(p);
    }
    else if (p->freqGroup == 1866)
    {
        DramcSetting_Olympus_LP4_ByteMode_DDR3733(p);
    }
    ///TODO: DDR2667
    else if (p->freqGroup == 1333 || p->freqGroup == 1200) // TODO: Initial settings for DDR2400?
    {
        DramcSetting_Olympus_LP4_ByteMode_DDR2667(p);
    }
    else if ((p->freqGroup == 800) || (p->freqGroup == 600) || (p->freqGroup == 400))
    {
        DramcSetting_Olympus_LP4_ByteMode_DDR1600(p);
    }

    UpdateInitialSettings_LP4(p);

#if SIMULATION_SW_IMPED // Darren: Need porting by E2 IMP Calib DVT owner
    #if FSP1_CLKCA_TERM
    U8 u1CASwImpFreqRegion = (p->dram_fsp == FSP_0)? IMP_LOW_FREQ: IMP_HIGH_FREQ;
    #else
    U8 u1CASwImpFreqRegion = (p->frequency <= 1866)? IMP_LOW_FREQ: IMP_HIGH_FREQ;
    #endif
    U8 u1DQSwImpFreqRegion = (p->frequency <= 1866)? IMP_LOW_FREQ: IMP_HIGH_FREQ;

    if (p->dram_type == TYPE_LPDDR4X)
        DramcSwImpedanceSaveRegister(p, u1CASwImpFreqRegion, u1DQSwImpFreqRegion, DRAM_DFS_REG_SHU0);
#endif

#ifndef LOOPBACK_TEST
    DDRPhyFreqMeter(p);
#endif

#if 0
    vIO32WriteFldMulti(DRAMC_REG_MRS, P_Fld(0x0, MRS_MRSRK)
                | P_Fld(0x4, MRS_MRSMA)
                | P_Fld(0x0, MRS_MRSOP));
    mcDELAY_US(1);
    vIO32WriteFldAlign(DRAMC_REG_SPCMD, 0x1, SPCMD_MRREN);
    vIO32WriteFldAlign(DRAMC_REG_SPCMD, 0x0, SPCMD_MRREN);
#endif

    vIO32WriteFldAlign(DRAMC_REG_TEST2_4, 0x0, TEST2_4_TEST_REQ_LEN1);

    vIO32WriteFldAlign(DRAMC_REG_SHU_CONF3, 0x5, SHU_CONF3_ZQCSCNT);

    mcDELAY_US(1);

#if !APPLY_LP4_POWER_INIT_SEQUENCE
    vIO32WriteFldMulti(DRAMC_REG_CKECTRL, P_Fld(0x0, CKECTRL_CKEFIXON)
                | P_Fld(0x0, CKECTRL_CKE1FIXON));
#endif
    vIO32WriteFldMulti(DRAMC_REG_REFCTRL0, P_Fld(0x1, REFCTRL0_PBREFEN)
                | P_Fld(0x1, REFCTRL0_PBREF_DISBYRATE));
    vIO32WriteFldMulti(DRAMC_REG_SHUCTRL2, P_Fld(0x1, SHUCTRL2_MR13_SHU_EN)
                | P_Fld(0x1, SHUCTRL2_HWSET_WLRL));
    vIO32WriteFldAlign(DRAMC_REG_REFCTRL0, 0x1, REFCTRL0_REFDIS);
    //vIO32WriteFldAlign(DRAMC_REG_SPCMDCTRL, 0x0, SPCMDCTRL_REFRDIS);//Lewis@20160613: Fix refresh rate is wrong while diable MR4
    vIO32WriteFldAlign(DRAMC_REG_DRAMCTRL, 0x1, DRAMCTRL_REQQUE_THD_EN);
                //| P_Fld(0x1, DRAMCTRL_DPDRK_OPT)); //DPDRK_OPT doesn't exit for Sylv_ia
    vIO32WriteFldMulti(DRAMC_REG_DUMMY_RD, P_Fld(0x1, DUMMY_RD_DQSG_DMYRD_EN)
                | P_Fld(p->support_rank_num, DUMMY_RD_RANK_NUM)
                | P_Fld(0x1, DUMMY_RD_DUMMY_RD_CNT6)
                | P_Fld(0x1, DUMMY_RD_DUMMY_RD_CNT5)
                | P_Fld(0x1, DUMMY_RD_DUMMY_RD_CNT3)
                | P_Fld(0x1, DUMMY_RD_DUMMY_RD_SW));
    vIO32WriteFldAlign(DRAMC_REG_TEST2_4, 0x4, TEST2_4_TESTAGENTRKSEL);
    vIO32WriteFldAlign(DRAMC_REG_DRAMCTRL, 0x0, DRAMCTRL_CTOREQ_HPRI_OPT);
    //               60759 === DE initial sequence done ===

///TODO: DVFS_Enable

    mcDELAY_US(1);

      //*((UINT32P)(DDRPHY0AO_BASE + 0x0000)) = 0x00000000;

    mcDELAY_US(1);

    DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
    //CH-A
    vIO32WriteFldMulti(DRAMC_REG_SHUCTRL, P_Fld(0x1, SHUCTRL_R_DRAMC_CHA) | P_Fld(0x0, SHUCTRL_SHU_PHYRST_SEL));
    vIO32WriteFldAlign(DRAMC_REG_SHUCTRL2, 0x1, SHUCTRL2_R_DVFS_DLL_CHA);
    //CH-B
    vIO32WriteFldMulti(DRAMC_REG_SHUCTRL + SHIFT_TO_CHB_ADDR, P_Fld(0x0, SHUCTRL_R_DRAMC_CHA) | P_Fld(0x1, SHUCTRL_SHU_PHYRST_SEL));
    vIO32WriteFldAlign(DRAMC_REG_SHUCTRL2 + SHIFT_TO_CHB_ADDR, 0x0, SHUCTRL2_R_DVFS_DLL_CHA);
    //               60826 ===LP4_3200_intial_setting_shu1 end===

#ifndef LOOPBACK_TEST
    DDRPhyFMeter_Init(p);
#endif

    DVFSSettings(p);

}
#endif // __A60868_TO_BE_PORTING__

#if 0
void vApplyConfigAfterCalibration(DRAMC_CTX_T *p)
{
#if __A60868_TO_BE_PORTING__

    U8 shu_index;
#if ENABLE_TMRRI_NEW_MODE
    U8 u1RankIdx;
#endif

/*================================
    PHY RX Settings
==================================*/
    vIO32WriteFldAlign_All(DDRPHY_MISC_CG_CTRL4, 0x11400000, MISC_CG_CTRL4_R_PHY_MCK_CG_CTRL);
    vIO32WriteFldAlign_All(DRAMC_REG_REFCTRL1, 0x0, REFCTRL1_SREF_CG_OPT);
    vIO32WriteFldAlign_All(DRAMC_REG_SHUCTRL, 0x0, SHUCTRL_DVFS_CG_OPT);//Move to DCM off setting

    /* Burst mode settings are removed from here due to
     *  1. Set in UpdateInitialSettings_LP4
     *  2. DQS Gating ensures new burst mode is switched when to done
     *     (or doesn't switch gatingMode at all, depending on "LP4_GATING_OLD_BURST_MODE")
     */

    vIO32WriteFldAlign_All(DDRPHY_CA_CMD6, 0x0, CA_CMD6_RG_RX_ARCMD_RES_BIAS_EN);
#if 0
    vIO32WriteFldAlign_All(DDRPHY_B0_DQ6, 0x0, B0_DQ6_RG_TX_ARDQ_OE_EXT_DIS_B0);
    vIO32WriteFldAlign_All(DDRPHY_B1_DQ6, 0x0, B1_DQ6_RG_TX_ARDQ_OE_EXT_DIS_B1);
    vIO32WriteFldAlign_All(DDRPHY_CA_CMD6, 0x0, CA_CMD6_RG_TX_ARCMD_OE_EXT_DIS);
#endif

#if ENABLE_WRITE_DBI
    EnableDRAMModeRegWriteDBIAfterCalibration(p);
#endif

#if ENABLE_READ_DBI
    EnableDRAMModeRegReadDBIAfterCalibration(p);
#endif

    // Set VRCG{MR13[3]} to 0 both to DRAM and DVFS
    SetMr13VrcgToNormalOperation(p);

    //DA mode
    vIO32WriteFldAlign_All(DDRPHY_B0_DQ6, 0x0, B0_DQ6_RG_RX_ARDQ_BIAS_PS_B0);
    vIO32WriteFldAlign_All(DDRPHY_B1_DQ6, 0x0, B1_DQ6_RG_RX_ARDQ_BIAS_PS_B1);
    vIO32WriteFldAlign_All(DDRPHY_CA_CMD6, 0x0, CA_CMD6_RG_RX_ARCMD_BIAS_PS);

    vIO32WriteFldAlign_All(DDRPHY_B0_DQ6, 0x1, B0_DQ6_RG_RX_ARDQ_RPRE_TOG_EN_B0);
    vIO32WriteFldAlign_All(DDRPHY_B1_DQ6, 0x1, B1_DQ6_RG_RX_ARDQ_RPRE_TOG_EN_B1);
    vIO32WriteFldAlign_All(DDRPHY_CA_CMD6, 0x1, CA_CMD6_RG_RX_ARCMD_RPRE_TOG_EN);

/*================================
    IMPCAL Settings
==================================*/
    vIO32WriteFldMulti_All(DRAMC_REG_IMPCAL, P_Fld(0, IMPCAL_IMPCAL_IMPPDP) | P_Fld(0, IMPCAL_IMPCAL_IMPPDN));    //RG_RIMP_BIAS_EN and RG_RIMP_VREF_EN move to IMPPDP and IMPPDN
    vIO32WriteFldAlign_All(DDRPHY_MISC_IMP_CTRL0, 0, MISC_IMP_CTRL0_RG_IMP_EN);

/*================================
    MR1
==================================*/

    //MR1 op[7]=0 already be setted at end of gating calibration, no need to set here again
/*
    u1MR01Value[p->dram_fsp] &= 0x7f;
    DramcModeRegWrite(p, 1, u1MR01Value[p->dram_fsp]);
*/
    //Prevent M_CK OFF because of hardware auto-sync
    vIO32WriteFldAlign_All(DDRPHY_MISC_CG_CTRL0, 0, Fld(4, 0, AC_MSKB0));

    //DFS- fix Gating Tracking settings
    vIO32WriteFldAlign_All(DDRPHY_MISC_CTRL0, 0, MISC_CTRL0_R_STBENCMP_DIV4CK_EN);
    vIO32WriteFldAlign_All(DDRPHY_MISC_CTRL1, 0, MISC_CTRL1_R_DMSTBENCMP_RK_OPT);

    ///TODO: Disable MR4 MR18/MR19, TxHWTracking, Dummy RD - for DFS workaround
    vIO32WriteFldAlign_All(DRAMC_REG_SPCMDCTRL, 0x1, SPCMDCTRL_REFRDIS);    //MR4 Disable
    //vIO32WriteFldAlign_All(DRAMC_REG_DQSOSCR, 0x1, DQSOSCR_DQSOSCRDIS);  //MR18, MR19 Disable
    //for(shu_index = SRAM_SHU0; shu_index < DRAM_DFS_SRAM_MAX; shu_index++)
      //vIO32WriteFldAlign_All(DRAMC_REG_SHU_SCINTV + SHU_GRP_DRAMC_OFFSET*shu_index, 0x1, SHU_SCINTV_DQSOSCENDIS);
    //vIO32WriteFldAlign_All(DRAMC_REG_SHU_SCINTV, 0x1, SHU_SCINTV_DQSOSCENDIS);
    //vIO32WriteFldAlign_All(DRAMC_REG_SHU2_SCINTV, 0x1, SHU2_SCINTV_DQSOSCENDIS);
    //vIO32WriteFldAlign_All(DRAMC_REG_SHU3_SCINTV, 0x1, SHU3_SCINTV_DQSOSCENDIS);
    vIO32WriteFldMulti_All(DRAMC_REG_DUMMY_RD, P_Fld(0x0, DUMMY_RD_DUMMY_RD_EN)
                                            | P_Fld(0x0, DUMMY_RD_SREF_DMYRD_EN)
                                            | P_Fld(0x0, DUMMY_RD_DQSG_DMYRD_EN)
                                            | P_Fld(0x0, DUMMY_RD_DMY_RD_DBG));

#if APPLY_LP4_POWER_INIT_SEQUENCE
    //CKE dynamic
#if ENABLE_TMRRI_NEW_MODE
    CKEFixOnOff(p, TO_ALL_RANK, CKE_DYNAMIC, TO_ALL_CHANNEL);
#else
    CKEFixOnOff(p, RANK_0, CKE_DYNAMIC, TO_ALL_CHANNEL);
#endif

    //// Enable  HW MIOCK control to make CLK dynamic
    DramCLKAlwaysOnOff(p, OFF);
#endif

    //close eyescan to save power
    vIO32WriteFldMulti_All(DRAMC_REG_EYESCAN, P_Fld(0x0, EYESCAN_EYESCAN_DQS_SYNC_EN)
                                        | P_Fld(0x0, EYESCAN_EYESCAN_NEW_DQ_SYNC_EN)
                                        | P_Fld(0x0, EYESCAN_EYESCAN_DQ_SYNC_EN));

    /* TESTAGENT2 */
    vIO32WriteFldAlign_All(DRAMC_REG_TEST2_4, 4, TEST2_4_TESTAGENTRKSEL); // Rank selection is controlled by Test Agent
#endif
}
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
        U8 u1DQ_CAP_SEL=0x1b, u1CA_CAP_SEL=0x1b;
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

        if(p->frequency<=400)
        {
            u1DQ_CAP_SEL= 0xf;
            u1CA_CAP_SEL= 0xf;
        }
        else if(p->frequency<=600)
        {
            u1DQ_CAP_SEL= 0x1b;
            u1CA_CAP_SEL= 0x1b;
        }
        else if(p->frequency<=1200)
        {
            u1DQ_CAP_SEL= 0xf;
            u1CA_CAP_SEL= 0xf;
        }
        else if(p->frequency<=1600)
        {
            u1DQ_CAP_SEL= 0xd;
            u1CA_CAP_SEL= 0xd;
        }
        else if(p->frequency<=2133)
        {
            u1DQ_CAP_SEL= 0xb;
            u1CA_CAP_SEL= 0xb;
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
                                                    | P_Fld((p->frequency==1200), SHU_B0_DQ2_RG_ARPI_SMT_XLATCH_FORCE_DQS_B0) //Sync MP setting WL
                                                    | P_Fld((p->frequency==1200), SHU_B0_DQ2_RG_ARPI_SMT_XLATCH_DQ_FORCE_B0)); //Sync MP setting WL
        vIO32WriteFldMulti_All(DDRPHY_REG_SHU_B1_DQ2,P_Fld((p->frequency>=2133), SHU_B1_DQ2_RG_ARPI_PSMUX_XLATCH_FORCE_DQS_B1)
                                                    | P_Fld((p->frequency>=2133), SHU_B1_DQ2_RG_ARPI_PSMUX_XLATCH_FORCE_DQ_B1)
                                                    | P_Fld((p->frequency==1200), SHU_B1_DQ2_RG_ARPI_SMT_XLATCH_FORCE_DQS_B1) //Sync MP setting WL
                                                    | P_Fld((p->frequency==1200), SHU_B1_DQ2_RG_ARPI_SMT_XLATCH_DQ_FORCE_B1)); //Sync MP setting WL
        vIO32WriteFldMulti_All(DDRPHY_REG_SHU_CA_CMD2,P_Fld((p->frequency>=2133), SHU_CA_CMD2_RG_ARPI_PSMUX_XLATCH_FORCE_CLK_CA)
                                                    | P_Fld((p->frequency>=2133), SHU_CA_CMD2_RG_ARPI_PSMUX_XLATCH_FORCE_CA_CA)
                                                    | P_Fld((p->frequency==1200), SHU_CA_CMD2_RG_ARPI_SMT_XLATCH_FORCE_CLK_CA) //Sync MP setting WL
                                                    | P_Fld((p->frequency==1200), SHU_CA_CMD2_RG_ARPI_SMT_XLATCH_CA_FORCE_CA)); //Sync MP setting WL

        //disable RX PIPE for RX timing pass
        vIO32WriteFldAlign_All(DDRPHY_REG_SHU_MISC_RX_PIPE_CTRL, 0x0, SHU_MISC_RX_PIPE_CTRL_RX_PIPE_BYPASS_EN);

        //Disable DPM IRQ
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
        vIO32WriteFldMulti_All(DDRPHY_REG_SHU_CA_CMD10, P_Fld(0x1, SHU_CA_CMD10_RG_RX_ARCLK_DQSSTB_RPST_HS_EN_CA)
                                                    | P_Fld(0x1, SHU_CA_CMD10_RG_RX_ARCLK_DQSSTB_CG_EN_CA));

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
        vIO32WriteFldAlign_All(DDRPHY_REG_SHU_CA_DLL2, 0x1, SHU_CA_DLL2_RG_ARCMD_REV);    //Jeremy

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
        vIO32WriteFldAlign_All(DRAMC_REG_SREF_DPD_CTRL, 0, SREF_DPD_CTRL_DSM_HW_EN);//DSM only for LP5


        //Follow DE - DDRPHY
        vIO32WriteFldMulti_All(DDRPHY_REG_B0_DLL_ARPI4, P_Fld(1, B0_DLL_ARPI4_RG_ARPI_BYPASS_SR_DQS_B0) | P_Fld(1, B0_DLL_ARPI4_RG_ARPI_BYPASS_SR_DQ_B0));
        vIO32WriteFldMulti_All(DDRPHY_REG_B1_DLL_ARPI4, P_Fld(1, B1_DLL_ARPI4_RG_ARPI_BYPASS_SR_DQS_B1) | P_Fld(1, B1_DLL_ARPI4_RG_ARPI_BYPASS_SR_DQ_B1));
        vIO32WriteFldMulti_All(DDRPHY_REG_CA_DLL_ARPI4, P_Fld(1, CA_DLL_ARPI4_RG_ARPI_BYPASS_SR_CLK_CA) | P_Fld(1, CA_DLL_ARPI4_RG_ARPI_BYPASS_SR_CA_CA));
        vIO32WriteFldMulti_All(DDRPHY_REG_CA_CMD11, P_Fld(0xa, CA_CMD11_RG_RRESETB_DRVN) | P_Fld(0xa, CA_CMD11_RG_RRESETB_DRVP));
        vIO32WriteFldAlign_All(DDRPHY_REG_MISC_CG_CTRL2, 0x1f, MISC_CG_CTRL2_RG_MEM_DCM_IDLE_FSEL);

        vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B0_DQSIEN_CFG, 1, SHU_B0_DQSIEN_CFG_RG_RX_ARDQS_DQSIEN_GLITCH_FREE_EN_B0);//checked with WL
        vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B1_DQSIEN_CFG, 1, SHU_B1_DQSIEN_CFG_RG_RX_ARDQS_DQSIEN_GLITCH_FREE_EN_B1);//checked with WL
        vIO32WriteFldAlign_All(DDRPHY_REG_SHU_CA_DQSIEN_CFG, 1, SHU_CA_DQSIEN_CFG_RG_RX_ARCLK_DQSIEN_GLITCH_FREE_EN_C0);//checked with WL


#if (ENABLE_DDR400_OPEN_LOOP_MODE_OPTION == 0) // Darren- for DDR400 open loop mode disable
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
        // @Darren, MISC_CTRL0_IDLE_DCM_CHB_CDC_ECO_OPT is empty after IPM from Mao
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
        vIO32WriteFldMulti_All(DDRPHY_REG_SHU_CA_CMD1, P_Fld(1, SHU_CA_CMD1_RG_ARPI_MIDPI_BYPASS_EN_CA)
                                                    | P_Fld(1, SHU_CA_CMD1_RG_ARPI_MIDPI_DUMMY_EN_CA)
                                                    | P_Fld(1, SHU_CA_CMD1_RG_ARPI_8PHASE_XLATCH_FORCE_CA));

        vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B0_DQ10, 1, SHU_B0_DQ10_RG_RX_ARDQS_BW_SEL_B0);
        vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B1_DQ10, 1, SHU_B1_DQ10_RG_RX_ARDQS_BW_SEL_B1);
        vIO32WriteFldAlign_All(DDRPHY_REG_SHU_CA_CMD10, 1, SHU_CA_CMD10_RG_RX_ARCLK_BW_SEL_CA); //sync MP settings by @WL review

        // @Darren, sync MP settings by @WL review
        {
            U8 u1DQ_BW_SEL_B0=0, u1DQ_BW_SEL_B1=0, u1CA_BW_SEL_CA=0;

            if (p->frequency >= 2133)
            {
                u1DQ_BW_SEL_B0 = 3;
                u1DQ_BW_SEL_B1 = 3;
                u1CA_BW_SEL_CA = 3;
            }

            vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B0_DQ11, u1DQ_BW_SEL_B0, SHU_B0_DQ11_RG_RX_ARDQ_BW_SEL_B0);
            vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B1_DQ11, u1DQ_BW_SEL_B1, SHU_B1_DQ11_RG_RX_ARDQ_BW_SEL_B1);
            vIO32WriteFldAlign_All(DDRPHY_REG_SHU_CA_CMD11, u1CA_BW_SEL_CA, SHU_CA_CMD11_RG_RX_ARCA_BW_SEL_CA);
        }

        //vIO32WriteFldMulti_All(DDRPHY_REG_SHU_CA_CMD1, P_Fld(1, SHU_CA_CMD1_RG_ARPI_MIDPI_BYPASS_EN_CA) | P_Fld(1, SHU_CA_CMD1_RG_ARPI_MIDPI_DUMMY_EN_CA));
        //Darren-vIO32WriteFldAlign_All(DDRPHY_REG_SHU_CA_CMD10, 1, SHU_CA_CMD10_RG_RX_ARCLK_DLY_LAT_EN_CA);

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
    //for(shu_index = SRAM_SHU0; shu_index < DRAM_DFS_SRAM_MAX; shu_index++)
        //vIO32WriteFldAlign_All(DRAMC_REG_SHU_CONF3 + SHU_GRP_DRAMC_OFFSET*shu_index, 0x1ff, SHU_CONF3_ZQCSCNT); //Every refresh number to issue ZQCS commands, only for DDR3/LPDDR2/LPDDR3/LPDDR4
    vIO32WriteFldAlign_All(DRAMC_REG_SHU_CONF3, 0x1ff, SHU_CONF3_ZQCSCNT); //Every refresh number to issue ZQCS commands, only for DDR3/LPDDR2/LPDDR3/LPDDR4
    //vIO32WriteFldAlign_All(DRAMC_REG_SHU2_CONF3, 0x1ff, SHU_CONF3_ZQCSCNT); //Every refresh number to issue ZQCS commands, only for DDR3/LPDDR2/LPDDR3/LPDDR4
    //vIO32WriteFldAlign_All(DRAMC_REG_SHU3_CONF3, 0x1ff, SHU_CONF3_ZQCSCNT); //Every refresh number to issue ZQCS commands, only for DDR3/LPDDR2/LPDDR3/LPDDR4
    vIO32WriteFldAlign_All(DRAMC_REG_DRAMCTRL, 0, DRAMCTRL_ZQCALL);  // HW send ZQ command for both rank, disable it due to some dram only have 1 ZQ pin for two rank.

    //Dual channel ZQCS interlace,  0: disable, 1: enable
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
        //Move to DCM off setting
        //vIO32WriteFldMulti_All(DRAMC_REG_ZQCS, P_Fld(1, ZQCS_ZQCSDUAL) |
        //                                       P_Fld(0, ZQCS_ZQCSMASK_OPT) |
        //                                       P_Fld(0, ZQCS_ZQMASK_CGAR));

        // DRAMC CHA(CHN0):ZQCSMASK=1, DRAMC CHB(CHN1):ZQCSMASK=0.
        // ZQCSMASK setting: (Ch A, Ch B) = (1,0) or (0,1)
        // if CHA.ZQCSMASK=1, and then set CHA.ZQCALDISB=1 first, else set CHB.ZQCALDISB=1 first
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
    //for(shu_index = SRAM_SHU0; shu_index < DRAM_DFS_SRAM_MAX; shu_index++)
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
    DramcLoopbackTest_settings(p, 0);   //0: internal loopback test 1: external loopback test
#else
    DramcLoopbackTest_settings(p, 1);   //0: internal loopback test 1: external loopback test
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



/* vDramcInit_PreSettings(): Initial register settings(which are required to be set before all calibration flow) */
#if 0
static void vDramcInit_PreSettings(DRAMC_CTX_T *p)
{
#if __A60868_TO_BE_PORTING__

    /* PAD_RRESETB control sequence */
    //remove twice dram reset pin pulse before dram power on sequence flow
    vIO32WriteFldMulti(DDRPHY_CA_CMD8, P_Fld(0x0, CA_CMD8_RG_TX_RRESETB_PULL_UP) | P_Fld(0x0, CA_CMD8_RG_TX_RRESETB_PULL_DN)
                                     | P_Fld(0x1, CA_CMD8_RG_TX_RRESETB_DDR3_SEL) | P_Fld(0x0, CA_CMD8_RG_TX_RRESETB_DDR4_SEL)
                                     | P_Fld(0xa, CA_CMD8_RG_RRESETB_DRVP) | P_Fld(0xa, CA_CMD8_RG_RRESETB_DRVN));
    vIO32WriteFldAlign(DDRPHY_MISC_CTRL1, 0x1, MISC_CTRL1_R_DMRRESETB_I_OPT); //Change to glitch-free path
    //replace DDRCONF0_GDDR3RST with MISC_CTRL1_R_DMDA_RRESETB_I
    vIO32WriteFldAlign(DDRPHY_MISC_CTRL1, 0x0, MISC_CTRL1_R_DMDA_RRESETB_I);
    vIO32WriteFldAlign(DDRPHY_MISC_CTRL1, 0x1, MISC_CTRL1_R_DMDA_RRESETB_E);
#if __ETT__
    if (GetDramcBroadcast() == DRAMC_BROADCAST_OFF)
    {   // In this function, broadcast is assumed to be ON(LP4) -> Show error if broadcast is OFF
        mcSHOW_ERR_MSG(("Err! Broadcast is OFF!\n"));
    }
#endif
    return;
#endif
}
#endif
static void DramcInit_DutyCalibration(DRAMC_CTX_T *p)
{
#if ENABLE_DUTY_SCAN_V2
    U8 channel_idx, channel_backup = vGetPHY2ChannelMapping(p);
    U32 u4backup_broadcast= GetDramcBroadcast();

    DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);

#ifndef DUMP_INIT_RG_LOG_TO_DE
    if (Get_MDL_Used_Flag()==NORMAL_USED)
    {
        for (channel_idx = CHANNEL_A; channel_idx < p->support_channel_num; channel_idx++)
        {
            vSetPHY2ChannelMapping(p, channel_idx);
            DramcNewDutyCalibration(p);
        }
        vSetPHY2ChannelMapping(p, channel_backup);
    }
#endif

    DramcBroadcastOnOff(u4backup_broadcast);

#endif
}

static void SV_BroadcastOn_DramcInit(DRAMC_CTX_T *p)
{
    DramcBroadcastOnOff(DRAMC_BROADCAST_ON);

    if(!is_lp5_family(p))
    {
        if(p->frequency>=2133)  //Term
        {
            mcSHOW_DBG_MSG2(("sv_algorithm_assistance_LP4_4266 \n"));
            sv_algorithm_assistance_LP4_4266(p);
        }
        else if(p->frequency>=1333)  //Term
        {
            mcSHOW_DBG_MSG2(("sv_algorithm_assistance_LP4_3733 \n"));
            sv_algorithm_assistance_LP4_3733(p);
        }
        else if(p->frequency>400) //Unterm
        {
            mcSHOW_DBG_MSG2(("sv_algorithm_assistance_LP4_1600 \n"));
            sv_algorithm_assistance_LP4_1600(p);
        }
        else if(p->frequency==400)  //DDR800 Semi-Open
        {
            //mcSHOW_DBG_MSG(("CInit_golden_mini_freq_related_vseq_LP4_1600 \n"));
            //CInit_golden_mini_freq_related_vseq_LP4_1600(p);
            //CInit_golden_mini_freq_related_vseq_LP4_1600_SHU1(DramcConfig);
            mcSHOW_DBG_MSG2(("sv_algorithm_assistance_LP4_800 \n"));
            sv_algorithm_assistance_LP4_800(p);
        }
        else //DDR400 Open Loop
        {
            mcSHOW_DBG_MSG(("sv_algorithm_assistance_LP4_400 \n"));
            sv_algorithm_assistance_LP4_400(p);
        }
    }

    RESETB_PULL_DN(p);
    ANA_init(p);
    DIG_STATIC_SETTING(p);
    DIG_CONFIG_SHUF(p,0,0); //temp ch0 group 0

    {
        LP4_UpdateInitialSettings(p);
    }

    DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
}

static void GetRealFreq_at_Init(DRAMC_CTX_T *p){
    gddrphyfmeter_value[vGet_Current_SRAMIdx(p)] = DDRPhyFreqMeter(p);
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

    //CInit_ConfigFromTBA();
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

        DVFSSettings(p);

#if REPLACE_DFS_RG_MODE
    DPMInit(p);
    #if ENABLE_DFS_RUNTIME_MRW
    DFSRuntimeFspMRW(p);
    #endif
    mcSHOW_DBG_MSG2(("DPMInit(p);done\n"));
#endif

    GetRealFreq_at_Init(p);

    vSetRank(p, RANK_0);

    // 8PhaseCal need executed before DutyCal
#ifdef DDR_INIT_TIME_PROFILING
    U32 CPU_Cycle;
    TimeProfileBegin();
#endif
    Dramc8PhaseCal(p);
#ifdef DDR_INIT_TIME_PROFILING
    CPU_Cycle=TimeProfileEnd();
    mcSHOW_TIME_MSG(("\t8PHaseCal takes %d us\n", CPU_Cycle));
    TimeProfileBegin();
#endif
    DramcInit_DutyCalibration(p);
#ifdef DDR_INIT_TIME_PROFILING
    CPU_Cycle=TimeProfileEnd();
    mcSHOW_TIME_MSG(("\tDutyCalibration takes %d us\n", CPU_Cycle));
#endif

    {
        //LP4_DRAM_INIT(p);
        DramcModeRegInit_LP4(p);
    }

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

#if 0
static void Switch26MHzDisableDummyReadRefreshAllBank(DRAMC_CTX_T *p)
{
#if __A60868_TO_BE_PORTING__

    vIO32WriteFldAlign_All(DDRPHY_MISC_CG_CTRL0, 0, MISC_CG_CTRL0_CLK_MEM_SEL);//Switch clk to 26MHz
    vIO32WriteFldAlign_All(DDRPHY_MISC_CG_CTRL0, 1, MISC_CG_CTRL0_W_CHG_MEM);
    vIO32WriteFldAlign_All(DDRPHY_MISC_CG_CTRL0, 0, MISC_CG_CTRL0_W_CHG_MEM);

    vIO32WriteFldAlign_All(DRAMC_REG_REFCTRL0, 0, REFCTRL0_PBREFEN);//Switch to all bank refresh

    vIO32WriteFldMulti_All(DRAMC_REG_DUMMY_RD, P_Fld(0x0, DUMMY_RD_DQSG_DMYWR_EN)//Disable Dummy Read
                | P_Fld(0x0, DUMMY_RD_DQSG_DMYRD_EN) | P_Fld(0x0, DUMMY_RD_SREF_DMYRD_EN)
                | P_Fld(0x0, DUMMY_RD_DUMMY_RD_EN) | P_Fld(0x0, DUMMY_RD_DMY_RD_DBG)
                | P_Fld(0x0, DUMMY_RD_DMY_WR_DBG));
#endif
    return;
}
#endif


#if ENABLE_TMRRI_NEW_MODE
void SetCKE2RankIndependent(DRAMC_CTX_T *p)
{
    #if ENABLE_TMRRI_NEW_MODE//Newly added CKE control mode API
    mcSHOW_DBG_MSG2(("SET_CKE_2_RANK_INDEPENDENT_RUN_TIME: ON\n"));
    vCKERankCtrl(p, CKE_RANK_INDEPENDENT);
    #else //Legacy individual CKE control register settings
    mcSHOW_DBG_MSG2(("SET_CKE_2_RANK_INDEPENDENT_RUN_TIME: OFF\n"));
    vCKERankCtrl(p, CKE_RANK_DEPENDENT);
    #endif
}
#endif


#if ENABLE_WRITE_DBI
void EnableDRAMModeRegWriteDBIAfterCalibration(DRAMC_CTX_T *p)
{
    U8 channel_idx, rank_idx;
    U8 ch_backup, rank_backup, u1FSPIdx = 0;
    //S8 u1ShuffleIdx;

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

    u4Value = u4IO32ReadFldAlign(DRAMC_REG_SHU_HWSET_VRCG, SHU_HWSET_VRCG_HWSET_VRCG_OP);
    vIO32WriteFldAlign_All(DRAMC_REG_SHU_HWSET_VRCG, u4Value & ~(0x1 << 3), SHU_HWSET_VRCG_HWSET_VRCG_OP);
    return;
}

#if ENABLE_LP4Y_DFS
void ENABLE_ClkSingleEndRG (DRAMC_CTX_T *p, U8 u1OnOff)
{
    vIO32WriteFldMulti_All(DDRPHY_REG_SHU_CA_CMD13, P_Fld( u1OnOff, SHU_CA_CMD13_RG_TX_ARCLKB_OE_TIE_SEL_CA) \
                                                      | P_Fld( u1OnOff, SHU_CA_CMD13_RG_TX_ARCLKB_OE_TIE_EN_CA));
    vIO32WriteFldAlign_All(DDRPHY_REG_SHU_CA_CMD7, u1OnOff, SHU_CA_CMD7_R_LP4Y_SDN_MODE_CLK);
}

void ENABLE_WDQSSingleEndRG (DRAMC_CTX_T *p, U8 u1OnOff)
{
    BOOL isLP4_DSC = (p->DRAMPinmux == PINMUX_DSC)?1:0;
    vIO32WriteFldMulti_All(DDRPHY_REG_SHU_B0_DQ13, P_Fld( !u1OnOff, SHU_B0_DQ13_RG_TX_ARDQSB_READ_BASE_EN_B0) \
                                                                | P_Fld(!u1OnOff, SHU_B0_DQ13_RG_TX_ARDQSB_READ_BASE_DATA_TIE_EN_B0) \
                                                                | P_Fld( u1OnOff, SHU_B0_DQ13_RG_TX_ARDQSB_OE_TIE_SEL_B0) \
                                                                | P_Fld( u1OnOff, SHU_B0_DQ13_RG_TX_ARDQSB_OE_TIE_EN_B0));
    vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B0_DQ7, u1OnOff , SHU_B0_DQ7_R_LP4Y_SDN_MODE_DQS0);

    if (isLP4_DSC)
    {
        vIO32WriteFldMulti_All(DDRPHY_REG_SHU_CA_CMD13, P_Fld(!u1OnOff, SHU_CA_CMD13_RG_TX_ARCLKB_READ_BASE_EN_CA) \
                                                                    | P_Fld(!u1OnOff, SHU_CA_CMD13_RG_TX_ARCLKB_READ_BASE_DATA_TIE_EN_CA) \
                                                                    | P_Fld( u1OnOff, SHU_CA_CMD13_RG_TX_ARCLKB_OE_TIE_SEL_CA) \
                                                                    | P_Fld( u1OnOff, SHU_CA_CMD13_RG_TX_ARCLKB_OE_TIE_EN_CA));
        vIO32WriteFldAlign_All(DDRPHY_REG_SHU_CA_CMD7, u1OnOff , SHU_CA_CMD7_R_LP4Y_SDN_MODE_CLK);
    }
    else
    {
        vIO32WriteFldMulti_All(DDRPHY_REG_SHU_B1_DQ13, P_Fld(!u1OnOff, SHU_B1_DQ13_RG_TX_ARDQSB_READ_BASE_EN_B1) \
                                                                    | P_Fld(!u1OnOff, SHU_B1_DQ13_RG_TX_ARDQSB_READ_BASE_DATA_TIE_EN_B1) \
                                                                    | P_Fld( u1OnOff, SHU_B1_DQ13_RG_TX_ARDQSB_OE_TIE_SEL_B1) \
                                                                    | P_Fld( u1OnOff, SHU_B1_DQ13_RG_TX_ARDQSB_OE_TIE_EN_B1));
        vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B1_DQ7, u1OnOff , SHU_B1_DQ7_R_LP4Y_SDN_MODE_DQS1);
    }
}

void ENABLE_RDQSSingleEndRG (DRAMC_CTX_T *p, U8 u1OnOff)
{
    vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B0_DQ10, u1OnOff, SHU_B0_DQ10_RG_RX_ARDQS_SE_EN_B0);
    if (p->DRAMPinmux == PINMUX_DSC)
        vIO32WriteFldAlign_All(DDRPHY_REG_SHU_CA_CMD10, u1OnOff, SHU_CA_CMD10_RG_RX_ARCLK_SE_EN_CA);
    else
        vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B1_DQ10, u1OnOff, SHU_B1_DQ10_RG_RX_ARDQS_SE_EN_B1);
}

void ENABLE_SingleEndRGEnable (DRAMC_CTX_T *p, U16 u2Freq)
{
    if (u2Freq <= 800)
    {
        ENABLE_ClkSingleEndRG (p, ENABLE);
        ENABLE_WDQSSingleEndRG (p, ENABLE);
        ENABLE_RDQSSingleEndRG (p, ENABLE);
    }
}
#endif

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
            DramcModeRegWriteByRank(p, u1RankIdx, 13, u1MR13Value[u1RankIdx]);
            DramcMRWriteFldAlign(p, 13, 0, MR13_VRCG, JUST_TO_GLOBAL_VALUE);
        }
    }
    vSetPHY2ChannelMapping(p, (U8)eOriChannel);
    vSetRank(p, (U8)eOriRank);
    return;
}


static void DramcShuTrackingDcmSeEnBySRAM(DRAMC_CTX_T *p)
{
#if (fcFOR_CHIP_ID == fcA60868)
    U8 u1ShuffleIdx, ShuRGAccessIdxBak;

    ShuRGAccessIdxBak = p->ShuRGAccessIdx;
    mcSHOW_DBG_MSG2(("\n==[DramcShuTrackingDcmEnBySRAM]==\n"));
    for (u1ShuffleIdx = 0; u1ShuffleIdx <= 1; u1ShuffleIdx++) //fill SHU1 of conf while (u1ShuffleIdx==DRAM_DFS_SRAM_MAX)
    {
        //Aceess DMA SRAM by APB bus use debug mode by conf SHU3
        p->ShuRGAccessIdx = u1ShuffleIdx;
        #ifdef HW_GATING
        //DramcHWGatingOnOff(p, 1, u4DramcShuOffset); // Enable HW gating tracking
        #endif

        #if ENABLE_TX_TRACKING
        Enable_TX_Tracking(p);
        #endif

        #if RDSEL_TRACKING_EN
        Enable_RDSEL_Tracking(p, u2Freq);
        #endif

        #ifdef HW_GATING
        Enable_Gating_Tracking(p); // Enable HW gating tracking
        #endif
    }
    p->ShuRGAccessIdx = ShuRGAccessIdxBak;
#else
    DRAM_DFS_FREQUENCY_TABLE_T *pFreqTable = p->pDFSTable; // from dramc conf shu0
    U8 u1ShuffleIdx;
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
            u2Freq = GetFreqBySel(p, p->pDFSTable->freq_sel);
            //Restore regs, or SHU0 RG cannot be set
            DramcRestoreRegisters(p, u4RegBackupAddress, sizeof(u4RegBackupAddress) / sizeof(U32));
            p->ShuRGAccessIdx = DRAM_DFS_REG_SHU0;//Since access conf SHU0
        }
        else
        {
            //Aceess DMA SRAM by APB bus use debug mode by conf SHU1
            vSetDFSTable(p, get_FreqTbl_by_SRAMIndex(p, u1ShuffleIdx));//Update DFS table
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
        Enable_TX_Tracking(p);
#endif
#if RDSEL_TRACKING_EN
        Enable_RDSEL_Tracking(p, u2Freq);
#endif
#ifdef HW_GATING
        Enable_Gating_Tracking(p); // Enable HW gating tracking
#endif

#if ENABLE_PER_BANK_REFRESH && (!IMP_TRACKING_PB_TO_AB_REFRESH_WA)
        Enable_PerBank_Refresh(p);
#endif

        EnableRxDcmDPhy(p, u2Freq);
        EnableCmdPicgEffImprove(p);
        Enable_ClkTxRxLatchEn(p); // for new xrank mode
#if ENABLE_TX_WDQS // @Darren, To avoid unexpected DQS toggle during calibration
        Enable_TxWDQS(p);
#endif

#if ENABLE_LP4Y_DFS && (LP4Y_BACKUP_SOLUTION == 0)
        ENABLE_SingleEndRGEnable (p, u2Freq);
#endif

#if (SW_CHANGE_FOR_SIMULATION == 0)
#if APPLY_LOWPOWER_GOLDEN_SETTINGS
        int enable_dcm = (doe_get_config("dramc_dcm")) ? 0 : 1;
        EnableDramcPhyDCMShuffle(p, enable_dcm);
#else
        EnableDramcPhyDCMShuffle(p, 0);
#endif
#endif
        SetMr13VrcgToNormalOperationShuffle(p);

        p->ShuRGAccessIdx = DRAM_DFS_REG_SHU0;
    }
#endif
}

#if ENABLE_PER_BANK_REFRESH
void DramcSetPerBankRefreshMode(DRAMC_CTX_T *p)
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
#endif

#ifdef TEMP_SENSOR_ENABLE
void DramcHMR4_Presetting(DRAMC_CTX_T *p)
{
    U8 backup_channel = p->channel;
    U8 channelIdx;

    for (channelIdx = CHANNEL_A; channelIdx < p->support_channel_num; channelIdx++)
    {
        vSetPHY2ChannelMapping(p, channelIdx);
    //    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_REFCTRL1), Refr_rate_manual_trigger, REFCTRL1_REFRATE_MANUAL_RATE_TRIG);
    //    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_REFCTRL1), Refr_rate_manual, REFCTRL1_REFRATE_MANUAL);

        vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_HMR4), 1, HMR4_REFR_PERIOD_OPT);
        vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_HMR4), 0, HMR4_REFRCNT_OPT);    // 0: 3.9us * cnt, 1: 15.6us * cnt
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

    {
        vIO32WriteFldAlign_All(DRAMC_REG_REF_BOUNCE2, 5, REF_BOUNCE2_PRE_MR4INT_TH);

        vIO32WriteFldAlign_All(DRAMC_REG_REFCTRL2, 5, REFCTRL2_MR4INT_TH);
    }

    // TOG_OPT, 0: Read rank0 only, 1: read both rank0 and rank1
    if (en && p->support_rank_num == RANK_DUAL)
        vIO32WriteFldAlign_All(DRAMC_REG_HMR4, 1, HMR4_HMR4_TOG_OPT);   // Read both rank0 and rank1
    else
        vIO32WriteFldAlign_All(DRAMC_REG_HMR4, 0, HMR4_HMR4_TOG_OPT);   // Read rank0 only (need for manual/SW MRR)

    vIO32WriteFldAlign_All(DRAMC_REG_HMR4, !en, HMR4_REFRDIS);

#if 0   // Reading HMR4 repeatedly for debugging
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
            mcSHOW_DBG_MSG(("@@         MIN    MAX\n"));
            mcSHOW_DBG_MSG(("@@ RK0_B0:  %d     %d\n",
                u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_HW_REFRATE_MON3), HW_REFRATE_MON3_REFRESH_RATE_MIN_MON_RK0_B0),
                u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_HW_REFRATE_MON3), HW_REFRATE_MON3_REFRESH_RATE_MAX_MON_RK0_B0)));
            mcSHOW_DBG_MSG(("@@ RK1_B0:  %d     %d\n",
                u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_HW_REFRATE_MON3), HW_REFRATE_MON3_REFRESH_RATE_MIN_MON_RK1_B0),
                u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_HW_REFRATE_MON3), HW_REFRATE_MON3_REFRESH_RATE_MAX_MON_RK1_B0)));
            mcSHOW_DBG_MSG(("@@ RK0_B1:  %d     %d\n",
                u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_HW_REFRATE_MON2), HW_REFRATE_MON2_REFRESH_RATE_MIN_MON_RK0_B1),
                u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_HW_REFRATE_MON2), HW_REFRATE_MON2_REFRESH_RATE_MAX_MON_RK0_B1)));
            mcSHOW_DBG_MSG(("@@ RK1_B1:  %d     %d\n",
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
                                                  P_Fld(5 , REF_BOUNCE1_REFRATE_DEBOUNCE_TH) |  // MR4 value >= 0.5X refresh rate, then de-bounce count active
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
            mcSHOW_DBG_MSG2(("CKE Debounce cnt = %d\n", u1CKE_DBECnt));
        }
        vSetRank(p, rank_backup);
    }
}
#endif

#if OPEN_LOOP_MODE_CLK_TOGGLE_WA
static void ToggleOpenLoopModeClk(DRAMC_CTX_T *p)
{
        U8 u1ChannelMode = (p->support_channel_num == CHANNEL_DUAL)?3:1;

    #if defined(DPM_CONTROL_AFTERK) && ((DRAMC_DFS_MODE%2) != 0)
        mcSHOW_DBG_MSG(("[WARNING] DDR400 out (DPM RG/PST MODE) Toggle CLK WA\n"));
        vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, u1ChannelMode, LPIF_LOW_POWER_CFG_0_DDRPHY_FB_CK_EN);
        mcDELAY_US(1);
        vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_1, u1ChannelMode, LPIF_LOW_POWER_CFG_1_DPY_BCLK_ENABLE);
        mcDELAY_US(1);
        vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_1, u1ChannelMode, LPIF_LOW_POWER_CFG_1_SHU_RESTORE);
        mcDELAY_US(1);
        vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_1, 0, LPIF_LOW_POWER_CFG_1_SHU_RESTORE);
        mcDELAY_US(1);
        vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_1, 0, LPIF_LOW_POWER_CFG_1_DPY_BCLK_ENABLE);
        mcDELAY_US(1);
        vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, 0, LPIF_LOW_POWER_CFG_0_DDRPHY_FB_CK_EN);
        mcDELAY_US(1);
    #else //PHY_RG_MODE
        mcSHOW_DBG_MSG(("[WARNING] DDR400 out (PHY RG MODE) Toggle CLK WA\n"));

        //set SPM project code and enable clock enable
        vIO32WriteFldMulti(SPM_POWERON_CONFIG_EN, P_Fld(0xB16, POWERON_CONFIG_EN_PROJECT_CODE) | P_Fld(1, POWERON_CONFIG_EN_BCLK_CG_EN));

        vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 1, MISC_RG_DFS_CTRL_RG_DDRPHY_FB_CK_EN);
        mcDELAY_US(1);
        vIO32WriteFldAlign(DRAMC_DPY_CLK_SW_CON_0, u1ChannelMode, DRAMC_DPY_CLK_SW_CON_0_SW_DPY_BCLK_ENABLE);
        mcDELAY_US(1);
        vIO32WriteFldAlign(DRAMC_DPY_CLK_SW_CON_1, u1ChannelMode, DRAMC_DPY_CLK_SW_CON_1_SW_SHU_RESTORE);
        mcDELAY_US(1);
        vIO32WriteFldAlign(DRAMC_DPY_CLK_SW_CON_1, 0, DRAMC_DPY_CLK_SW_CON_1_SW_SHU_RESTORE);
        mcDELAY_US(1);
        vIO32WriteFldAlign(DRAMC_DPY_CLK_SW_CON_0, 0, DRAMC_DPY_CLK_SW_CON_0_SW_DPY_BCLK_ENABLE);
        mcDELAY_US(1);
        vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 0, MISC_RG_DFS_CTRL_RG_DDRPHY_FB_CK_EN);
        mcDELAY_US(1);

        //Other WA
        //DramcDFSDirectJump_SRAMShuRGMode(p, SRAM_SHU7);
        //DramcDFSDirectJump_SRAMShuRGMode(p, SRAM_SHU7);
    #endif
}
#endif

//1.Some RG setting will need to be DCM on, since not consider S0 2.ENABLE_RX_DCM_DPHY should be 1
#if 0
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
#endif
void DramcRunTimeConfig(DRAMC_CTX_T *p)
{
#if (fcFOR_CHIP_ID == fcA60868)
    u1EnterRuntime = 1;
#endif

    mcSHOW_DBG_MSG(("[DramcRunTimeConfig]\n"));

    SetDramInfoToConf(p);

#if defined(DPM_CONTROL_AFTERK) && ((DRAMC_DFS_MODE%2) != 0) && (REPLACE_DFS_RG_MODE==0)// for DPM RG/PST mode
    DPMInit(p);
    mcSHOW_DBG_MSG(("DPM_CONTROL_AFTERK: ON\n"));
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
#elif (fcFOR_CHIP_ID == fc8195)
    vIO32WriteFldAlign(DRAMC_REG_ZQ_SET1 + (CHANNEL_A << POS_BANK_NUM), 1, ZQ_SET1_ZQCALDISB);
    vIO32WriteFldAlign(DRAMC_REG_ZQ_SET1 + (CHANNEL_C << POS_BANK_NUM), 1, ZQ_SET1_ZQCALDISB);

    mcDELAY_US(1);

    vIO32WriteFldAlign(DRAMC_REG_ZQ_SET1 + (CHANNEL_B << POS_BANK_NUM), 1, ZQ_SET1_ZQCALDISB);
    vIO32WriteFldAlign(DRAMC_REG_ZQ_SET1 + (CHANNEL_D << POS_BANK_NUM), 1, ZQ_SET1_ZQCALDISB);
#endif
    mcSHOW_DBG_MSG(("ZQCS_ENABLE_LP4: ON\n"));
#else
    vIO32WriteFldAlign_All(DRAMC_REG_ZQ_SET1, 0, ZQ_SET1_ZQCALDISB);// LP3 and LP4 are different, be careful.
    mcSHOW_DBG_MSG(("ZQCS_ENABLE_LP4: OFF\n"));
    #if (!__ETT__) && ENABLE_LP4Y_DFS // for preloader
    #error RTMRW DFS must support the SWZQ at DPM!!!
    #endif
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

#if ENABLE_LP4Y_DFS
    mcSHOW_DBG_MSG(("ENABLE_LP4Y_DFS: ON\n"));
#else
    mcSHOW_DBG_MSG(("ENABLE_LP4Y_DFS: OFF\n"));
#endif

#if (SW_CHANGE_FOR_SIMULATION == 0)
#if APPLY_LOWPOWER_GOLDEN_SETTINGS
    int enable_dcm = (doe_get_config("dramc_dcm"))? 0: 1;
    //const char *str = (enable_dcm == 1)? ("ON") : ("OFF");
//    EnableDramcPhyDCM(p, enable_dcm);
    EnableDramcPhyDCMNonShuffle(p, enable_dcm);
    //mcSHOW_DBG_MSG(("LOWPOWER_GOLDEN_SETTINGS(DCM): %s\n", str));

    if(enable_dcm == 0)
    {
        //S0_DCMOffWA(p);//For S0 + DCM off
    }

#else
//    EnableDramcPhyDCM(p, DCM_OFF);
    EnableDramcPhyDCMNonShuffle(p, 0);
    mcSHOW_DBG_MSG(("LOWPOWER_GOLDEN_SETTINGS(DCM): OFF\n"));

    //S0_DCMOffWA(p);//For S0 + DCM off
#endif
#endif

//DumpShuRG(p);



#if 1
    DramcShuTrackingDcmSeEnBySRAM(p);
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

    HwSaveForSR(p);

    ClkFreeRunForDramcPsel(p);

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

#if DFS_NOQUEUE_FLUSH_ENABLE
    EnableDFSNoQueueFlush(p);
    mcSHOW_DBG_MSG(("DFS_NO_QUEUE_FLUSH: ON\n"));
#else
    mcSHOW_DBG_MSG(("DFS_NO_QUEUE_FLUSH: OFF\n"));
#endif

#if DFS_NOQUEUE_FLUSH_LATENCY_CNT
    vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_FSM_CFG, 1, LPIF_FSM_CFG_DBG_LATENCY_CNT_EN);
    // DPM clock is 208M
    vIO32WriteFldMulti_All(DDRPHY_MD32_REG_SSPM_MCLK_DIV, P_Fld(0, SSPM_MCLK_DIV_MCLK_SRC)
        | P_Fld(0, SSPM_MCLK_DIV_MCLK_DIV));
    mcSHOW_DBG_MSG(("DFS_NO_QUEUE_FLUSH_LATENCY_CNT: ON\n"));
#else
    mcSHOW_DBG_MSG(("DFS_NO_QUEUE_FLUSH_LATENCY_CNT: OFF\n"));
#endif

#if ENABLE_DFS_RUNTIME_MRW
    #if (REPLACE_DFS_RG_MODE==0)
    DFSRuntimeFspMRW(p);
    #endif
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

#if OPEN_LOOP_MODE_CLK_TOGGLE_WA
    if(vGet_DDR_Loop_Mode(p) == OPEN_LOOP_MODE)
    {
        mcSHOW_DBG_MSG(("OPEN_LOOP_MODE_CLK_TOGGLE_WA: ON\n"));
        ToggleOpenLoopModeClk(p);
    }
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

    //Need to do tx retry when DDR800 -> DDr1200
    if ((gIsddr800TxRetry == 1) && (shu_level != SRAM_SHU6)
#if ENABLE_DDR400_OPEN_LOOP_MODE_OPTION
        && (shu_level != SRAM_SHU7))
#else
        )
#endif
    {
        if (onoff == ENABLE)
        {
            mcSHOW_DBG_MSG2(("TX track retry: ENABLE! (DDR800 to DDR1200)\n"));
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
            mcSHOW_DBG_MSG2(("TX track retry: DISABLE! (DDR800 to DDR1200)\n"));
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
#define SW_TX_RETRY_ENABLE_WA 1 //To recieve response by disable CG
void SWTx_Track_Retry_OnOff(DRAMC_CTX_T *p)
{
    U8 u4Response;
    U8 u1RegBackup_DQSOSCENDIS;
    U8 u1Shu_backup=p->ShuRGAccessIdx;
    #if SW_TX_RETRY_ENABLE_WA
    U8 u1RegBackup;

    u1RegBackup=u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DCM_SUB_CTRL), DCM_SUB_CTRL_SUBCLK_CTRL_TX_TRACKING);
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DCM_SUB_CTRL), 0, DCM_SUB_CTRL_SUBCLK_CTRL_TX_TRACKING);
    #endif
    p->ShuRGAccessIdx = u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_STATUS_10,LPIF_STATUS_10_DRAMC_DR_SHU_LEVEL)& 0x1;
    u1RegBackup_DQSOSCENDIS = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_DQSOSC_SET0),SHU_DQSOSC_SET0_DQSOSCENDIS);
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_DQSOSC_SET0), 1, SHU_DQSOSC_SET0_DQSOSCENDIS);
    mcSHOW_DBG_MSG2(("SW TX track retry!\n"));
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TX_RETRY_SET0), 0, TX_RETRY_SET0_XSR_TX_RETRY_SW_EN);
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TX_RETRY_SET0), 1, TX_RETRY_SET0_XSR_TX_RETRY_BLOCK_ALE_MASK);
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TX_RETRY_SET0), 1, TX_RETRY_SET0_XSR_TX_RETRY_SW_EN);
    do
    {
        u4Response = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SPCMDRESP), SPCMDRESP_TX_RETRY_DONE_RESPONSE);
        mcDELAY_US(1);  // Wait tZQCAL(min) 1us or wait next polling
        mcSHOW_DBG_MSG4(("still wait tx retry be done\n", u4Response));
    }while (u4Response == 0);
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TX_RETRY_SET0), 0, TX_RETRY_SET0_XSR_TX_RETRY_SW_EN);
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TX_RETRY_SET0), 0, TX_RETRY_SET0_XSR_TX_RETRY_BLOCK_ALE_MASK);
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_DQSOSC_SET0), u1RegBackup_DQSOSCENDIS, SHU_DQSOSC_SET0_DQSOSCENDIS);
    #if SW_TX_RETRY_ENABLE_WA
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DCM_SUB_CTRL), u1RegBackup, DCM_SUB_CTRL_SUBCLK_CTRL_TX_TRACKING);
    #endif
    p->ShuRGAccessIdx=u1Shu_backup;
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

#ifdef DDR_INIT_TIME_PROFILING
    CPU_Cycle = TimeProfileEnd();
    mcSHOW_TIME_MSG(("  DFSInitForCalibration() take %d ms\n\n", CPU_Cycle / 1000));
#endif

#ifndef DUMP_INIT_RG_LOG_TO_DE
    #ifdef ENABLE_MIOCK_JMETER
    if ((Get_MDL_Used_Flag()==NORMAL_USED) && (p->frequency >= 800))
    {
        Get_RX_DelayCell(p);
    }
    #endif
#endif

#ifdef DDR_INIT_TIME_PROFILING
    TimeProfileBegin();
#endif

#ifndef DUMP_INIT_RG_LOG_TO_DE
    #ifdef ENABLE_MIOCK_JMETER
    if (Get_MDL_Used_Flag()==NORMAL_USED)
    {
        p->u2DelayCellTimex100 = GetVcoreDelayCellTime(p);
    }
    #endif
#endif

#ifdef DDR_INIT_TIME_PROFILING
    CPU_Cycle=TimeProfileEnd();
    mcSHOW_TIME_MSG(("  (3) JMeter takes %d ms\n\r", CPU_Cycle / 1000));
#endif
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
static void RXPICGSetting(DRAMC_CTX_T * p)
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


static void RODTSettings(DRAMC_CTX_T *p)
{
    U8 u1VrefSel;
    U8 u1RankIdx, u1RankIdxBak;
    BOOL isLP4_DSC = (p->DRAMPinmux == PINMUX_DSC)?1:0;

    //VREF_EN
    vIO32WriteFldAlign(DDRPHY_REG_B0_DQ5, 1, B0_DQ5_RG_RX_ARDQ_VREF_EN_B0);
    vIO32WriteFldAlign(DDRPHY_REG_B1_DQ5, !isLP4_DSC, B1_DQ5_RG_RX_ARDQ_VREF_EN_B1);
    vIO32WriteFldAlign(DDRPHY_REG_CA_CMD5, isLP4_DSC, CA_CMD5_RG_RX_ARCMD_VREF_EN);

    //Update related setting of APHY RX and ODT
    vIO32WriteFldAlign(DDRPHY_REG_SHU_B0_VREF, !(p->odt_onoff), SHU_B0_VREF_RG_RX_ARDQ_VREF_UNTERM_EN_B0);
    if (!isLP4_DSC)
        vIO32WriteFldAlign(DDRPHY_REG_SHU_B1_VREF, !(p->odt_onoff), SHU_B1_VREF_RG_RX_ARDQ_VREF_UNTERM_EN_B1);
    else
        vIO32WriteFldAlign(DDRPHY_REG_SHU_CA_VREF, !(p->odt_onoff), SHU_CA_VREF_RG_RX_ARCA_VREF_UNTERM_EN_CA);

    if(p->odt_onoff==ODT_ON)
    {
            u1VrefSel = 0x2c;//term LP4
    }
    else
    {
            u1VrefSel = 0x37;//unterm LP4
    }

    u1RankIdxBak = u1GetRank(p);
    for (u1RankIdx = 0; u1RankIdx < (U32)(p->support_rank_num); u1RankIdx++)
    {
        vSetRank(p, u1RankIdx);
        vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_PHY_VREF_SEL,
                    P_Fld(u1VrefSel, SHU_B0_PHY_VREF_SEL_RG_RX_ARDQ_VREF_SEL_LB_B0) |
                    P_Fld(u1VrefSel, SHU_B0_PHY_VREF_SEL_RG_RX_ARDQ_VREF_SEL_UB_B0));
        if (!isLP4_DSC){
            vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_PHY_VREF_SEL,
                        P_Fld(u1VrefSel, SHU_B1_PHY_VREF_SEL_RG_RX_ARDQ_VREF_SEL_LB_B1) |
                        P_Fld(u1VrefSel, SHU_B1_PHY_VREF_SEL_RG_RX_ARDQ_VREF_SEL_UB_B1));
        }
        else
        {
            vIO32WriteFldMulti(DDRPHY_REG_SHU_CA_PHY_VREF_SEL,
                        P_Fld(u1VrefSel, SHU_CA_PHY_VREF_SEL_RG_RX_ARCA_VREF_SEL_LB) |
                        P_Fld(u1VrefSel, SHU_CA_PHY_VREF_SEL_RG_RX_ARCA_VREF_SEL_UB));
        }
    }
    vSetRank(p, u1RankIdxBak);

    vIO32WriteFldAlign(DDRPHY_REG_SHU_B0_VREF, 1, SHU_B0_VREF_RG_RX_ARDQ_VREF_RANK_SEL_EN_B0);
    if (!isLP4_DSC)
        vIO32WriteFldAlign(DDRPHY_REG_SHU_B1_VREF, 1, SHU_B1_VREF_RG_RX_ARDQ_VREF_RANK_SEL_EN_B1);
    else
        vIO32WriteFldAlign(DDRPHY_REG_SHU_CA_VREF, 1, SHU_CA_VREF_RG_RX_ARCA_VREF_RANK_SEL_EN_CA);

#if ENABLE_TX_WDQS
    vIO32WriteFldAlign(DDRPHY_REG_MISC_SHU_ODTCTRL, 1, MISC_SHU_ODTCTRL_RODTEN);
    vIO32WriteFldAlign(DDRPHY_REG_SHU_B0_DQ7, 1, SHU_B0_DQ7_R_DMRODTEN_B0);
    vIO32WriteFldAlign(DDRPHY_REG_SHU_B1_DQ7, !isLP4_DSC, SHU_B1_DQ7_R_DMRODTEN_B1);
    vIO32WriteFldAlign(DDRPHY_REG_SHU_CA_CMD7, isLP4_DSC, SHU_CA_CMD7_R_DMRODTEN_CA);
#else
    vIO32WriteFldAlign(DDRPHY_REG_MISC_SHU_ODTCTRL, p->odt_onoff, MISC_SHU_ODTCTRL_RODTEN);
    vIO32WriteFldAlign(DDRPHY_REG_SHU_B0_DQ7, p->odt_onoff, SHU_B0_DQ7_R_DMRODTEN_B0);
    vIO32WriteFldAlign(DDRPHY_REG_SHU_B1_DQ7, (p->odt_onoff & !isLP4_DSC), SHU_B1_DQ7_R_DMRODTEN_B1);
    vIO32WriteFldAlign(DDRPHY_REG_SHU_CA_CMD7, (p->odt_onoff & isLP4_DSC), SHU_CA_CMD7_R_DMRODTEN_CA);
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

    if (!isLP4_DSC)
    {
        vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ13, P_Fld(0, SHU_B1_DQ13_RG_TX_ARDQS_OE_ODTEN_CG_EN_B1)
                                            | P_Fld(0, SHU_B1_DQ13_RG_TX_ARDQM_OE_ODTEN_CG_EN_B1));
        vIO32WriteFldAlign(DDRPHY_REG_SHU_B1_DQ14, 0, SHU_B1_DQ14_RG_TX_ARDQ_OE_ODTEN_CG_EN_B1);
    }
    else
    {
        vIO32WriteFldMulti(DDRPHY_REG_SHU_CA_CMD13, P_Fld(0, SHU_CA_CMD13_RG_TX_ARCLK_OE_ODTEN_CG_EN_CA)
                                            | P_Fld(0, SHU_CA_CMD13_RG_TX_ARCS_OE_ODTEN_CG_EN_CA));
        vIO32WriteFldAlign(DDRPHY_REG_SHU_CA_CMD14, 0, SHU_CA_CMD14_RG_TX_ARCA_OE_ODTEN_CG_EN_CA);
    }
}

/* LP4 use 7UI mode (1)
 * LP5 lower than 4266 use 7UI mode (1)
 * LP5 higher than 4266 use 11UI mode (2)
 * LP5 higher than 4266 with better SI use 11/24UI mode (3)
 */
static void DQSSTBSettings(DRAMC_CTX_T *p)
{
    unsigned int dqsien_mode = 1;
    BOOL isLP4_DSC = (p->DRAMPinmux == PINMUX_DSC)?1:0;

    vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_STBCAL),
        dqsien_mode, MISC_SHU_STBCAL_DQSIEN_DQSSTB_MODE);
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ10),
        dqsien_mode, SHU_B0_DQ10_RG_RX_ARDQS_DQSIEN_MODE_B0);
    if(!isLP4_DSC)
        vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ10),
            dqsien_mode, SHU_B1_DQ10_RG_RX_ARDQS_DQSIEN_MODE_B1);
    else
        vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD10),
            dqsien_mode, SHU_CA_CMD10_RG_RX_ARCLK_DQSIEN_MODE_CA);
}

static void SetMck8xLowPwrOption(DRAMC_CTX_T *p)
{
#if ENABLE_REMOVE_MCK8X_UNCERT_LOWPOWER_OPTION
    U32 u4Mck8xMode = 1;
#else
    U32 u4Mck8xMode = 0;
#endif

    vIO32WriteFldMulti(DDRPHY_REG_MISC_LP_CTRL,  P_Fld( u4Mck8xMode , MISC_LP_CTRL_RG_SC_ARPI_RESETB_8X_SEQ_LP_SEL ) \
                                                              | P_Fld( u4Mck8xMode , MISC_LP_CTRL_RG_ADA_MCK8X_8X_SEQ_LP_SEL      ) \
                                                              | P_Fld( u4Mck8xMode, MISC_LP_CTRL_RG_AD_MCK8X_8X_SEQ_LP_SEL       ) \
                                                              | P_Fld( u4Mck8xMode , MISC_LP_CTRL_RG_MIDPI_EN_8X_SEQ_LP_SEL       ) \
                                                              | P_Fld( u4Mck8xMode , MISC_LP_CTRL_RG_MIDPI_CKDIV4_EN_8X_SEQ_LP_SEL) \
                                                              | P_Fld( u4Mck8xMode, MISC_LP_CTRL_RG_MCK8X_CG_SRC_LP_SEL          ) \
                                                              | P_Fld( u4Mck8xMode , MISC_LP_CTRL_RG_MCK8X_CG_SRC_AND_LP_SEL      ));

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
    vIO32WriteFldAlign(DDRPHY_REG_SHU_CA_CMD14, 0x0, SHU_CA_CMD14_RG_TX_ARCA_MCKIO_SEL_CA); //@Jimmy, confirm with WL set EMCP/DSC = 0, from mt6833

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
        DramcSwImpedanceSaveRegister(p, u1CASwImpFreqRegion, u1DQSwImpFreqRegion, DRAM_DFS_REG_SHU0);
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

#if (fcFOR_CHIP_ID == fc8195)
    if(p->support_rank_num== RANK_SINGLE)
        vIO32WriteFldAlign(DRAMC_REG_CMD_DEC_CTRL0, 1, CMD_DEC_CTRL0_CS1FIXOFF);

    //vIO32WriteFldAlign(DDRPHY_REG_SHU_MISC_EMI_CTRL, 0x26, SHU_MISC_EMI_CTRL_DR_EMI_RESERVE);//Alex-CH checked
#endif

    //MP Setting
    vIO32WriteFldMulti(DRAMC_REG_DUMMY_RD, P_Fld(0x1, DUMMY_RD_DMYRD_REORDER_DIS) | P_Fld(0x1, DUMMY_RD_SREF_DMYRD_EN));
    // @Unique, sync MP settings
    vIO32WriteFldMulti(DRAMC_REG_DRAMCTRL, P_Fld(0x0, DRAMCTRL_ALL_BLOCK_CTO_ALE_DBG_EN)
                                     | P_Fld(0x1, DRAMCTRL_DVFS_BLOCK_CTO_ALE_DBG_EN)
                                     | P_Fld(0x1, DRAMCTRL_SELFREF_BLOCK_CTO_ALE_DBG_EN));
    vIO32WriteFldAlign(DDRPHY_REG_MISC_STBCAL2, 1, MISC_STBCAL2_DQSGCNT_BYP_REF);
    //@Darren- enable bit11 via FMeter, vIO32WriteFldAlign(DDRPHY_REG_MISC_CG_CTRL7, 0, MISC_CG_CTRL7_CK_BFE_DCM_EN);

    //1:8 --> data rate<=1600 set 0,  data rate<=3200 set 1, else 2
    //1:4 --> data rate<= 800 set 0,  data rate<=1600 set 1, else 2
    // @Darren, confirm w/ Chau-Wei Wang (Jason) sync MP settings
    if(p->frequency<=800)
        vIO32WriteFldAlign(DDRPHY_REG_MISC_SHU_PHY_RX_CTRL, 0, MISC_SHU_PHY_RX_CTRL_RX_IN_BUFF_EN_HEAD);
    else if(p->frequency<=1200)
        vIO32WriteFldAlign(DDRPHY_REG_MISC_SHU_PHY_RX_CTRL, 1, MISC_SHU_PHY_RX_CTRL_RX_IN_BUFF_EN_HEAD);
    else // for DDR4266/DDR3200
        vIO32WriteFldAlign(DDRPHY_REG_MISC_SHU_PHY_RX_CTRL, 2, MISC_SHU_PHY_RX_CTRL_RX_IN_BUFF_EN_HEAD);

    // @Darren, Fix DQ/DQM duty PI not work.
    vIO32WriteFldAlign(DDRPHY_REG_MISC_CTRL1, 1, MISC_CTRL1_R_DMARPIDQ_SW);

    // @Darren, sync MP from Mao
    vIO32WriteFldMulti(DDRPHY_REG_CA_TX_MCK, P_Fld(0xa, CA_TX_MCK_R_DMRESETB_DRVP_FRPHY) | P_Fld(0xa, CA_TX_MCK_R_DMRESETB_DRVN_FRPHY));

    // @Darren, Fix GE for new RANK_SEL design from HJ/WL Lee (B16/8 + APHY design skew = 3*MCK)
    /*
    DQSI_DIV latency:
    100ps +    200ps     +    234ps*3    +    200ps      = 1202ps
    (INB)      (gate dly)     (3UI@4266)     (LVSH+wire)

    LEAD/LAG latency: (after retime with DQSI_DIV_INV) 200ps (LVSH+wire)
    */
    vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RANK_SEL_LAT, P_Fld(0x3, MISC_SHU_RANK_SEL_LAT_RANK_SEL_LAT_B0) |
            P_Fld(0x3, MISC_SHU_RANK_SEL_LAT_RANK_SEL_LAT_B1) | P_Fld(0x3, MISC_SHU_RANK_SEL_LAT_RANK_SEL_LAT_CA));

    SetMck8xLowPwrOption(p);
}

#define CKGEN_FMETER 0x0
#define ABIST_FMETER 0x1
/*
1. Select meter clock input: CLK_DBG_CFG[1:0] = 0x0
2. Select clock source from below table: CLK_DBG_CFG[21:16] = 0x39
3. Setup meter div: CLK_MISC_CFG_0[31:24] = 0x0
4. Enable frequency meter: CLK26CALI_0[12] = 0x1
5. Trigger frequency meter: CLK26CALI_0[4] = 0x1
6. Wait until CLK26CALI_0[4] = 0x0
7. Read meter count: CLK26CALI_1[15:0]
8. Calculate measured frequency: freq. = (26 * cal_cnt) / 1024

DE: Mas Lin
*/
static unsigned int FMeter(unsigned char u1CLKMeterSel, unsigned char u1CLKMuxSel)
{
#if (FOR_DV_SIMULATION_USED==0)
    unsigned int tmp, u4CalCnt;

    // enable fmeter
    DRV_WriteReg32(CLK26CALI_0, (0x1 << 7));

    // select meter
    tmp = DRV_Reg32(CLK_DBG_CFG);
    tmp &= ~0x3;

    if (u1CLKMeterSel == CKGEN_FMETER)
        tmp |= 0x1;

    DRV_WriteReg32(CLK_DBG_CFG, tmp);

    // select fmeter's input clock
    tmp = DRV_Reg32(CLK_DBG_CFG);

    if (u1CLKMeterSel == CKGEN_FMETER) {
        tmp &= ~(0xFF << 16);
        tmp |=  u1CLKMuxSel << 16;
    } else {
        tmp &= ~(0x7F << 8);
        tmp |=  u1CLKMuxSel << 8;
    }

    DRV_WriteReg32(CLK_DBG_CFG, tmp);

    // setup fmeter div
    tmp = DRV_Reg32(CLK_MISC_CFG_0);
    tmp &= ~(0xFF << 24);
    DRV_WriteReg32(CLK_MISC_CFG_0, tmp);

    // set load_cnt to 0x3FF (1024 - 1)
    tmp = DRV_Reg32(CLK26CALI_1);
    tmp &= ~(0x3FF << 16);
    tmp |= 0x3FF << 16;
    DRV_WriteReg32(CLK26CALI_1, tmp);

    // trigger fmeter
    tmp = DRV_Reg32(CLK26CALI_0);
    tmp |= (0x1 << 4);
    DRV_WriteReg32(CLK26CALI_0, tmp);

    // wait
    while (DRV_Reg32(CLK26CALI_0) & (0x1 << 4)) {
        mcDELAY_US(1);
    }

    // read
    u4CalCnt = DRV_Reg32(CLK26CALI_1) & 0xFFFF;

    // disable fmeter
    tmp = DRV_Reg32(CLK26CALI_0);
    tmp &= ~(0x1 << 7);
    DRV_WriteReg32(CLK26CALI_0, tmp);

    return ((u4CalCnt * 26) / 1024);
#endif
}

unsigned int DDRPhyFreqMeter(DRAMC_CTX_T *p)
{

#if (FOR_DV_SIMULATION_USED == 0)

    unsigned int reg0=0;//reg1=0
    unsigned int backup_phypll = 0, backup_clrpll = 0;
    unsigned int before_value=0, after_value=0;
    unsigned int frq_result=0;
    unsigned int meter_value=0;
    U16 frqValue = 0;

#if 1//def HJ_SIM
    /*TINFO="\n[PhyFreqMeter]"*/

    reg0 = DRV_Reg32(Channel_A_DDRPHY_AO_BASE_ADDRESS + 0x70c) ;
    backup_phypll = reg0;
    DRV_WriteReg32  (Channel_A_DDRPHY_AO_BASE_ADDRESS + 0x70c  , reg0 | (1 << 16));
    reg0 = DRV_Reg32(Channel_A_DDRPHY_AO_BASE_ADDRESS + 0x72c) ;
    backup_clrpll = reg0;
    DRV_WriteReg32  (Channel_A_DDRPHY_AO_BASE_ADDRESS + 0x72c  , reg0 | (1 << 16));

    mcDELAY_US(1);
    // abist_clk22: AD_CLKSQ_FS26M_CK
    frq_result = FMeter(ABIST_FMETER, 22);
    mcSHOW_DBG_MSG4(("AD_CLKSQ_FS26M_CK=%d MHz\n", frq_result));

    // abist_clk30: AD_MPLL_CK
	frq_result = FMeter(ABIST_FMETER, 30);
	mcSHOW_DBG_MSG4(("AD_MPLL_CK=%d MHz\n", frq_result));
    /*TINFO="AD_MPLL_CK FREQ=%d\n", frq_result*/

    #if 1
    if((DRV_Reg32(Channel_A_DDRPHY_NAO_BASE_ADDRESS + 0x50c) & (1<<8))==0)
    {
        // abist_clk119: AD_RCLRPLL_DIV4_CK_ch01
        //frq_result = FMeter(ABIST_FMETER, 119);
        //mcSHOW_DBG_MSG4(("AD_RCLRPLL_DIV4_CK_ch01 FREQ=%d MHz\n", frq_result));
        /*TINFO="AD_RCLRPLL_DIV4_CK_ch01 FREQ=%d\n", frq_result*/
    }
    else
    {
        // abist_clk120: AD_RPHYRPLL_DIV4_CK_ch01
        //frq_result = FMeter(ABIST_FMETER, 120);
        //mcSHOW_DBG_MSG4(("AD_RPHYRPLL_DIV4_CK_ch01 FREQ=%d\n", frq_result));
        /*TINFO="AD_RPHYRPLL_DIV4_CK_ch01 FREQ=%d\n", frq_result*/
    }
    #endif
    //! ch0
    reg0 = DRV_Reg32(Channel_A_DDRPHY_AO_BASE_ADDRESS + 0x504) ;
    DRV_WriteReg32  (Channel_A_DDRPHY_AO_BASE_ADDRESS + 0x504  , reg0 | (1 << 11));

    // abistgen_clk118: fmem_ck_bfe_dcm_ch0 (DRAMC CHA's clock before idle mask)
	before_value = FMeter(ABIST_FMETER, 118);
	mcSHOW_DBG_MSG4(("fmem_ck_bfe_dcm_ch0 FREQ=%d MHz\n", before_value));
    /*TINFO="fmem_ck_bfe_dcm_ch0 FREQ=%d\n", before_value*/

    // abistgen_clk117: fmem_ck_aft_dcm_ch0 (DRAMC CHA's clock after idle mask)
	after_value = FMeter(ABIST_FMETER, 117);
    mcSHOW_DBG_MSG4(("fmem_ck_aft_dcm_ch0 FREQ=%d MHz\n", after_value));
    /*TINFO="fmem_ck_aft_dcm_ch0 FREQ=%d\n", after_value*/

    //gddrphyfmeter_value = after_value << 2;

    #if 0 // @ tg change for Open loop mode(0x70C need fix 1)
    reg0 = DRV_Reg32(Channel_A_DDRPHY_AO_BASE_ADDRESS + 0x70c) ;
    DRV_WriteReg32  (Channel_A_DDRPHY_AO_BASE_ADDRESS + 0x70c  , reg0 & ~(1 << 16));
    reg0 = DRV_Reg32(Channel_A_DDRPHY_AO_BASE_ADDRESS + 0x72c) ;
    DRV_WriteReg32  (Channel_A_DDRPHY_AO_BASE_ADDRESS + 0x72c  , reg0 & ~(1 << 16));
    #else
    DRV_WriteReg32  (Channel_A_DDRPHY_AO_BASE_ADDRESS + 0x70c  , backup_phypll);
    DRV_WriteReg32  (Channel_A_DDRPHY_AO_BASE_ADDRESS + 0x72c  , backup_clrpll);
    #endif

    #if (CHANNEL_NUM>2)
	    if (channel_num_auxadc > 2) {
		    reg0 = DRV_Reg32(Channel_C_DDRPHY_AO_BASE_ADDRESS + 0x70c) ;
		    DRV_WriteReg32  (Channel_C_DDRPHY_AO_BASE_ADDRESS + 0x70c  , reg0 | (1 << 16));
		    reg0 = DRV_Reg32(Channel_C_DDRPHY_AO_BASE_ADDRESS + 0x72c) ;
		    DRV_WriteReg32  (Channel_C_DDRPHY_AO_BASE_ADDRESS + 0x72c  , reg0 | (1 << 16));

    #if 1
    if((DRV_Reg32(Channel_C_DDRPHY_NAO_BASE_ADDRESS + 0x50c) & (1<<8))==0)
    {
        // abist_clk116: AD_RCLRPLL_DIV4_CK_ch23
        //frq_result = FMeter(ABIST_FMETER, 116);
        //mcSHOW_DBG_MSG4(("AD_RCLRPLL_DIV4_CK_ch23 FREQ=%d MHz\n", frq_result));
        /*TINFO="AD_RCLRPLL_DIV4_CK_ch23 FREQ=%d\n", frq_result*/
    }
    else
    {
        // abist_clk115: AD_RPHYRPLL_DIV4_CK_ch23
        //frq_result = FMeter(ABIST_FMETER, 115);
        //mcSHOW_DBG_MSG4(("AD_RPHYRPLL_DIV4_CK_ch23 FREQ=%d\n", frq_result));
        /*TINFO="AD_RPHYRPLL_DIV4_CK_ch23 FREQ=%d\n", frq_result*/
    }
    #endif

    reg0 = DRV_Reg32(Channel_C_DDRPHY_AO_BASE_ADDRESS + 0x504) ;
    DRV_WriteReg32  (Channel_C_DDRPHY_AO_BASE_ADDRESS + 0x504  , reg0 | (1 << 11));
    reg0 = DRV_Reg32(Channel_D_DDRPHY_AO_BASE_ADDRESS + 0x504) ;
    DRV_WriteReg32  (Channel_D_DDRPHY_AO_BASE_ADDRESS + 0x504  , reg0 | (1 << 11));

    // abistgen_clk114: fmem_ck_bfe_dcm_ch2 (DRAMC CHC's clock after idle mask)
	before_value = FMeter(ABIST_FMETER, 114);
    mcSHOW_DBG_MSG4(("fmem_ck_bfe_dcm_ch2 FREQ=%d MHz\n", before_value));
    /*TINFO="fmem_ck_bfe_dcm_ch2 FREQ=%d\n", after_value*/

    // abistgen_clk113: fmem_ck_aft_dcm_ch2 (DRAMC CHC's clock after idle mask)
	after_value = FMeter(ABIST_FMETER, 113);
    mcSHOW_DBG_MSG4(("fmem_ck_aft_dcm_ch2 FREQ=%d MHz\n", after_value));
    /*TINFO="fmem_ck_aft_dcm_ch2 FREQ=%d\n", after_value*/

    reg0 = DRV_Reg32(Channel_C_DDRPHY_AO_BASE_ADDRESS + 0x70c) ;
    DRV_WriteReg32  (Channel_C_DDRPHY_AO_BASE_ADDRESS + 0x70c  , reg0 & ~(1 << 16));
    reg0 = DRV_Reg32(Channel_C_DDRPHY_AO_BASE_ADDRESS + 0x72c) ;
    DRV_WriteReg32  (Channel_C_DDRPHY_AO_BASE_ADDRESS + 0x72c  , reg0 & ~(1 << 16));
	}
    #endif

    meter_value = (before_value<<16 | after_value);
#else
    mcSHOW_DBG_MSG3(("\n[PhyFreqMeter]\n"));
    // abist_clk29: AD_MPLL_CK
    mcSHOW_DBG_MSG(("AD_MPLL_CK FREQ=%d\n", FMeter(ABIST_FMETER, 29)));

    // abist_clk31: AD_RCLRPLL_DIV4_CK_ch02
    mcSHOW_DBG_MSG(("AD_RCLRPLL_DIV4_CK_ch02 FREQ=%d\n", FMeter(ABIST_FMETER, 31)));

    // abist_clk32: AD_RCLRPLL_DIV4_CK_ch13
    mcSHOW_DBG_MSG(("AD_RCLRPLL_DIV4_CK_ch13 FREQ=%d\n", FMeter(ABIST_FMETER, 32)));

    // abist_clk33: AD_RPHYRPLL_DIV4_CK_ch02
    mcSHOW_DBG_MSG(("AD_RPHYRPLL_DIV4_CK_ch02 FREQ=%d\n", FMeter(ABIST_FMETER, 33)));

    // abist_clk34: AD_RPHYRPLL_DIV4_CK_ch13
    mcSHOW_DBG_MSG(("AD_RPHYRPLL_DIV4_CK_ch13 FREQ=%d\n", FMeter(ABIST_FMETER, 34)));

    // enable ck_bfe_dcm_en for freqmeter measure ddrphy clock, not needed for normal use
    reg0 = DRV_Reg32(Channel_A_DDRPHY_AO_BASE_ADDRESS + 0x504) ;
    DRV_WriteReg32  (Channel_A_DDRPHY_AO_BASE_ADDRESS + 0x504  , reg0 | (1 << 11));

    reg0 = DRV_Reg32(Channel_B_DDRPHY_AO_BASE_ADDRESS + 0x504) ;
    DRV_WriteReg32  (Channel_B_DDRPHY_AO_BASE_ADDRESS + 0x504  , reg0 | (1 << 11));

    reg0 = DRV_Reg32(Channel_C_DDRPHY_AO_BASE_ADDRESS + 0x504) ;
    DRV_WriteReg32  (Channel_C_DDRPHY_AO_BASE_ADDRESS + 0x504  , reg0 | (1 << 11));

    reg0 = DRV_Reg32(Channel_D_DDRPHY_AO_BASE_ADDRESS + 0x504) ;
    DRV_WriteReg32  (Channel_D_DDRPHY_AO_BASE_ADDRESS + 0x504  , reg0 | (1 << 11));

    // abistgen_clk44: fmem_ck_bfe_dcm_ch0 (DRAMC CHA's clock before idle mask)
    before_value = FMeter(ABIST_FMETER, 44);
    mcSHOW_DBG_MSG(("fmem_ck_bfe_dcm_ch0 FREQ=%d\n", FMeter(ABIST_FMETER, 44)));

    // abistgen_clk45: fmem_ck_bfe_dcm_ch1 (DRAMC CHB's clock before idle mask)
    before_value = FMeter(ABIST_FMETER, 45);
    mcSHOW_DBG_MSG(("fmem_ck_bfe_dcm_ch1 FREQ=%d\n", FMeter(ABIST_FMETER, 45)));

    // abistgen_clk46: fmem_ck_bfe_dcm_ch2 (DRAMC CHC's clock before idle mask)
    before_value = FMeter(ABIST_FMETER, 46);
    mcSHOW_DBG_MSG(("fmem_ck_bfe_dcm_ch2 FREQ=%d\n", FMeter(ABIST_FMETER, 46)));

    // abistgen_clk47: fmem_ck_bfe_dcm_ch3 (DRAMC CHC's clock before idle mask)
    before_value = FMeter(ABIST_FMETER, 47);
    mcSHOW_DBG_MSG(("fmem_ck_bfe_dcm_ch3 FREQ=%d\n", FMeter(ABIST_FMETER, 47)));

    return 1;
#if 0
    DRV_WriteReg32(DRAMC_CH0_TOP0_BASE + 0x2a0, reg0);
    DRV_WriteReg32(DRAMC_CH1_TOP0_BASE + 0x2a0, reg1);

    return ((before_value<<16) | after_value);
#endif

#endif //! end DSIM

        if(vGet_Div_Mode(p) == DIV8_MODE)
        {
        frqValue = (meter_value & 0xFFFF) << 2;
        }
        else if(vGet_Div_Mode(p) == DIV4_MODE)
        {
        frqValue = (meter_value & 0xFFFF) << 1;
        }
        else if(vGet_Div_Mode(p) == DIV16_MODE)
        {
        frqValue = (meter_value & 0xFFFF) << 4;
        }
        else
        {
            mcSHOW_ERR_MSG(("[DDRPhyFreqMeter] Get DIV mode error!\n"));
        #if __ETT__
            while (1);
        #endif
        }

    return frqValue;

#endif
}
