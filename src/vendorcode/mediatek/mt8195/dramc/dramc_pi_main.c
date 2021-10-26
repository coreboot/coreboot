/* SPDX-License-Identifier: BSD-3-Clause */

#include "dramc_common.h"
#include "dramc_dv_init.h"
#include "dramc_int_global.h"
#include "x_hal_io.h"
#include "sv_c_data_traffic.h"
#include "dramc_pi_api.h"
#if (FOR_DV_SIMULATION_USED == 0)
#include "dramc_top.h"
#endif
#include <emi.h>

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

U8 gHQA_Test_Freq_Vcore_Level = 0;

u8 ett_fix_freq = 0xff;

DRAM_DFS_FREQUENCY_TABLE_T gFreqTbl[DRAM_DFS_SRAM_MAX] = {
    {LP4_DDR3200 /*0*/, DIV8_MODE, SRAM_SHU1, DUTY_LAST_K,  VREF_CALI_ON,  CLOSE_LOOP_MODE},
    {LP4_DDR4266 /*1*/, DIV8_MODE, SRAM_SHU0, DUTY_NEED_K,  VREF_CALI_ON,   CLOSE_LOOP_MODE},
#if ENABLE_DDR400_OPEN_LOOP_MODE_OPTION
    {LP4_DDR400  /*2*/, DIV4_MODE, SRAM_SHU7, DUTY_DEFAULT, VREF_CALI_OFF,  OPEN_LOOP_MODE},
#endif
    {LP4_DDR800  /*2*/, DIV4_MODE, SRAM_SHU6, DUTY_DEFAULT, VREF_CALI_OFF,  SEMI_OPEN_LOOP_MODE},
    {LP4_DDR1866 /*3*/, DIV8_MODE, SRAM_SHU3, DUTY_LAST_K,  VREF_CALI_OFF,  CLOSE_LOOP_MODE},
    {LP4_DDR1200 /*4*/, DIV8_MODE, SRAM_SHU5, DUTY_LAST_K,  VREF_CALI_OFF,  CLOSE_LOOP_MODE},
    {LP4_DDR2400 /*5*/, DIV8_MODE, SRAM_SHU2, DUTY_NEED_K,  VREF_CALI_ON,   CLOSE_LOOP_MODE},
    {LP4_DDR1600 /*6*/, DIV8_MODE, SRAM_SHU4, DUTY_DEFAULT, VREF_CALI_ON,   CLOSE_LOOP_MODE},
};

DRAMC_CTX_T DramCtx_LPDDR4 =
{
    CHANNEL_SINGLE,
    CHANNEL_A,
    RANK_DUAL,
    RANK_0,

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
#if DV_SIMULATION_LP4
    TYPE_LPDDR4X,
#else
    TYPE_LPDDR5,
#endif
    FSP_0 ,
    FSP_0 ,
    ODT_OFF,
    {CBT_NORMAL_MODE, CBT_NORMAL_MODE},
#if ENABLE_READ_DBI
    {DBI_OFF,DBI_ON},
#else
    {DBI_OFF,DBI_OFF},
#endif
#if ENABLE_WRITE_DBI
    {DBI_OFF,DBI_ON},
#else
    {DBI_OFF,DBI_OFF},
#endif
    DATA_WIDTH_16BIT,
    DEFAULT_TEST2_1_CAL,
    DEFAULT_TEST2_2_CAL,
#if ENABLE_K_WITH_WORST_SI_UI_SHIFT
    TEST_WORST_SI_PATTERN,
#else
    TEST_XTALK_PATTERN,
#endif
#if (DV_SIMULATION_LP4 == 1)
    800,
    800,
#else
    1600,
    1600,
#endif
    0x88,
    REVISION_ID_MAGIC,
    0xff,
    {0,0},
    270,

#if PRINT_CALIBRATION_SUMMARY

    {{0,0,}},

    {{0,0,}},
    1,
    0,
#endif
    {0},
#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
    FALSE,
    0,
    0,
    0,
    &SavetimeData,
#endif
    &gFreqTbl[DRAM_DFS_SRAM_MAX-1],
    DRAM_DFS_REG_SHU0,
    TRAINING_MODE2,
    CBT_PHASE_RISING,
    0,
    PHYPLL_MODE,
    DBI_OFF,
    FSP_MAX,
    PINMUX_EMCP,
    {DISABLE,DISABLE},
    0,
};



#if defined(DDR_INIT_TIME_PROFILING) || (__ETT__ && SUPPORT_SAVE_TIME_FOR_CALIBRATION)
DRAMC_CTX_T gTimeProfilingDramCtx;
U8 gtime_profiling_flag = 0;
#endif

void vSetVcoreByFreq(DRAMC_CTX_T *p)
{
#if (FOR_DV_SIMULATION_USED==0 && SW_CHANGE_FOR_SIMULATION==0)
#if __FLASH_TOOL_DA__
    dramc_set_vcore_voltage(750000);
#else
    unsigned int vio18, vcore, vdram, vddq, vmddr;
    //int ret;

    vio18 = vcore = vdram = vddq = vmddr = 0;

#if __ETT__
    hqa_set_voltage_by_freq(p, &vio18, &vcore, &vdram, &vddq, &vmddr);
#elif defined(VCORE_BIN)
    switch (vGet_Current_SRAMIdx(p)) {
    case SRAM_SHU0:
    #ifdef VOLTAGE_SEL
        vcore = vcore_voltage_select(KSHU0);
        if (!vcore)
    #endif
        vcore = get_vcore_uv_table(0);
        break;
    case SRAM_SHU1:
    #ifdef VOLTAGE_SEL
        vcore = vcore_voltage_select(KSHU1);
        if (!vcore)
    #endif
        vcore = (get_vcore_uv_table(0) + get_vcore_uv_table(1)) >> 1;
        break;
    case SRAM_SHU2:
    case SRAM_SHU3:
    #ifdef VOLTAGE_SEL
        vcore = vcore_voltage_select(KSHU2);
        if (!vcore)
    #endif
        vcore = (get_vcore_uv_table(0) + get_vcore_uv_table(2)) >> 1;
        break;
    case SRAM_SHU4:
    case SRAM_SHU5:
    case SRAM_SHU6:
    #ifdef VOLTAGE_SEL
        vcore = vcore_voltage_select(KSHU4);
        if (!vcore)
    #endif
        vcore = (get_vcore_uv_table(0) + get_vcore_uv_table(3)) >> 1;
        break;
    }
#else
    switch (vGet_Current_SRAMIdx(p)) {
    case SRAM_SHU0:
    #ifdef VOLTAGE_SEL
        vcore = vcore_voltage_select(KSHU0);
    #else
        vcore = SEL_PREFIX_VCORE(LP4, KSHU0);
    #endif
        break;
    case SRAM_SHU1:
    #ifdef VOLTAGE_SEL
        vcore = vcore_voltage_select(KSHU1);
    #else
        vcore = SEL_PREFIX_VCORE(LP4, KSHU1);
    #endif
        break;
    case SRAM_SHU2:
    case SRAM_SHU3:
    #ifdef VOLTAGE_SEL
        vcore = vcore_voltage_select(KSHU2);
    #else
        vcore = SEL_PREFIX_VCORE(LP4, KSHU2);
    #endif
        break;
    case SRAM_SHU4:
    case SRAM_SHU5:
    case SRAM_SHU6:
#if ENABLE_DDR400_OPEN_LOOP_MODE_OPTION
    case SRAM_SHU7:
#endif
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

    if (CONFIG(MEDIATEK_DRAM_DVFS))
        if (vcore)
            dramc_set_vcore_voltage(vcore);

#if defined(DRAM_HQA)
    if (vio18)
        dramc_set_vio18_voltage(vio18);

    if (vdram)
        dramc_set_vmdd_voltage(p->dram_type, vdram);

    if (vddq)
        dramc_set_vmddq_voltage(p->dram_type, vddq);

    if (vmddr)
        dramc_set_vmddr_voltage(vmddr);
#endif

#ifdef FOR_HQA_REPORT_USED
    switch (vGet_Current_SRAMIdx(p)) {
        case SRAM_SHU0:
        case SRAM_SHU1:
    case SRAM_SHU2:
    case SRAM_SHU3:
    case SRAM_SHU4:
    case SRAM_SHU5:
    case SRAM_SHU6:
#if ENABLE_DDR400_OPEN_LOOP_MODE_OPTION
    case SRAM_SHU7:
#endif
        gHQA_Test_Freq_Vcore_Level = 0;
            break;
        default:
            print("[HQA] undefined shuffle level for Vcore (SHU%d)\r\n", vGet_Current_SRAMIdx(p));
#if __ETT__
            while(1);
#endif
            break;
    }
#endif


#ifndef DDR_INIT_TIME_PROFILING
    print("Read voltage for %d, %d\n", p->frequency, vGet_Current_SRAMIdx(p));
    print("Vcore = %d\n", dramc_get_vcore_voltage());
#ifdef FOR_HQA_REPORT_USED
    print("Vdram = %d\n", dramc_get_vmdd_voltage(p->dram_type));
    print("Vddq = %d\n", dramc_get_vmddq_voltage(p->dram_type));
    print("Vmddr = %d\n", dramc_get_vmddr_voltage());
#endif
#endif

#endif
#endif
}

U32 vGetVoltage(DRAMC_CTX_T *p, U32 get_voltage_type)
{
#if (defined(DRAM_HQA) || defined(__ETT__)) && (FOR_DV_SIMULATION_USED == 0)
    if (get_voltage_type==0)
        return dramc_get_vcore_voltage();

    if (get_voltage_type==1)
        return dramc_get_vmdd_voltage(p->dram_type);

    if (get_voltage_type==2)
        return dramc_get_vmddq_voltage(p->dram_type);

    if (get_voltage_type==3)
           return dramc_get_vio18_voltage();
    if (get_voltage_type==4)
             return dramc_get_vmddr_voltage();

#endif

    return 0;
}


#if RUNTIME_SHMOO_RELEATED_FUNCTION && SUPPORT_SAVE_TIME_FOR_CALIBRATION
#define RUNTIME_SHMOO_ON_GOING  0x00
#define RUNTIME_SHMOO_END       0xff
void RunTime_Shmoo_update_parameters(DRAMC_CTX_T *p)
{
    U8 backup_channel, backup_rank;
    U16 tx_pi_delay, tx_dqm_pi_delay;
    S16 rx_first_delay=0x1fff, rx_first_delay_bit=0xff, rx_last_delay=-0x1fff, rx_last_delay_bit=0xff;
    U8 u1BitIdx;
    U32 u4value;
    PASS_WIN_DATA_T FinalWinPerBit[DQ_DATA_WIDTH];
    U8 ui_large_value, ui_small_value, pi_value;
    U8 ui_dqm_large_value, ui_dqm_small_value, pi_dqm_value;
#if 0
    U8 ui_oen_large_value, ui_oen_small_value, pi_oen_value;
    U8 ui_dqm_oen_large_value, ui_dqm_oen_small_value, pi_dqm_oen_value;
#endif
    U8 scan_invert_flag=0;

    backup_channel = p->channel;
    backup_rank = p->rank;

    p->channel = RUNTIME_SHMOO_TEST_CHANNEL;
    p->rank = RUNTIME_SHMOO_TEST_RANK;

#if RUNTIME_SHMOO_TX
    if (RUNTIME_SHMOO_TEST_BYTE == 0)
    {
        tx_pi_delay = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ0), SHURK_SELPH_DQ0_TXDLY_DQ0) * 256 +
                      u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ2), SHURK_SELPH_DQ2_DLY_DQ0) * 32 +
                      u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_DQ0), SHU_R0_B0_DQ0_SW_ARPI_DQ_B0);

        tx_dqm_pi_delay = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ1), SHURK_SELPH_DQ1_TXDLY_DQM0) * 256 +
                          u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ3), SHURK_SELPH_DQ3_DLY_DQM0) * 32 +
                          u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_DQ0), SHU_R0_B0_DQ0_SW_ARPI_DQM_B0);
    }
    else
    {
        tx_pi_delay = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ0), SHURK_SELPH_DQ0_TXDLY_DQ1) * 256 +
                      u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ2), SHURK_SELPH_DQ2_DLY_DQ1) * 32 +
                      u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_DQ0), SHU_R0_B1_DQ0_SW_ARPI_DQ_B1);

        tx_dqm_pi_delay = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ1), SHURK_SELPH_DQ1_TXDLY_DQM1) * 256 +
                          u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ3), SHURK_SELPH_DQ3_DLY_DQM1) * 32 +
                          u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_DQ0), SHU_R0_B1_DQ0_SW_ARPI_DQM_B1);
    }
#endif

#if RUNTIME_SHMOO_RX
//    vAutoRefreshSwitch(p, ENABLE);

    for (u1BitIdx=0; u1BitIdx<8; u1BitIdx++)
    {
        FinalWinPerBit[u1BitIdx].first_pass=p->pSavetimeData->u1RxWinPerbitDQ_firsbypass_Save[p->channel][p->rank][u1BitIdx+RUNTIME_SHMOO_TEST_BYTE*8];
        FinalWinPerBit[u1BitIdx].last_pass=p->pSavetimeData->u1RxWinPerbitDQ_lastbypass_Save[p->channel][p->rank][u1BitIdx+RUNTIME_SHMOO_TEST_BYTE*8];
    }


    for (u1BitIdx=0; u1BitIdx<8; u1BitIdx++)
    {
        if (FinalWinPerBit[u1BitIdx].first_pass < rx_first_delay)
        {
            rx_first_delay = FinalWinPerBit[u1BitIdx].first_pass;
            rx_first_delay_bit = u1BitIdx;
        }
        if (FinalWinPerBit[u1BitIdx].last_pass > rx_last_delay)
        {
            rx_last_delay = FinalWinPerBit[u1BitIdx].last_pass;
            rx_last_delay_bit = u1BitIdx;
        }
    }
#endif

#if __ETT__
    mcSHOW_ERR_MSG(("fra dramc_get_rshmoo_pass_cnt = %x\n",dramc_get_rshmoo_pass_cnt()));
    mcSHOW_ERR_MSG(("fra femmc_Ready = %d ==\n",p->femmc_Ready));
#else
    print("fra dramc_get_rshmoo_pass_cnt = %x\n",dramc_get_rshmoo_pass_cnt());
    print("fra femmc_Ready = %d ==\n",p->femmc_Ready);
#endif

    if (p->femmc_Ready==0 ||
 ((p->pSavetimeData->Runtime_Shmoo_para.TX_Channel!=RUNTIME_SHMOO_TEST_CHANNEL) || (p->pSavetimeData->Runtime_Shmoo_para.TX_Rank!=RUNTIME_SHMOO_TEST_RANK) || (p->pSavetimeData->Runtime_Shmoo_para.TX_Byte!=RUNTIME_SHMOO_TEST_BYTE)))
    {
#if RUNTIME_SHMOO_TX
        p->pSavetimeData->Runtime_Shmoo_para.flag= RUNTIME_SHMOO_ON_GOING;
        p->pSavetimeData->Runtime_Shmoo_para.Scan_Direction=0;
        p->pSavetimeData->Runtime_Shmoo_para.TX_PI_delay = tx_pi_delay-32+RUNTIME_SHMOO_TEST_PI_DELAY_START;
        p->pSavetimeData->Runtime_Shmoo_para.TX_Original_PI_delay = p->pSavetimeData->Runtime_Shmoo_para.TX_PI_delay;
        p->pSavetimeData->Runtime_Shmoo_para.TX_DQM_PI_delay = tx_dqm_pi_delay-32+RUNTIME_SHMOO_TEST_PI_DELAY_START;
        p->pSavetimeData->Runtime_Shmoo_para.TX_Original_DQM_PI_delay = p->pSavetimeData->Runtime_Shmoo_para.TX_DQM_PI_delay;
        if (RUNTIME_SHMOO_TEST_VREF_START<51)
        {
            p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Range = 0;
            p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Value = RUNTIME_SHMOO_TEST_VREF_START;
        }
        else
        {
            p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Range = 1;
            p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Value = RUNTIME_SHMOO_TEST_VREF_START-51+21;
        }
        p->pSavetimeData->Runtime_Shmoo_para.TX_Channel = RUNTIME_SHMOO_TEST_CHANNEL;
        p->pSavetimeData->Runtime_Shmoo_para.TX_Rank = RUNTIME_SHMOO_TEST_RANK;
        p->pSavetimeData->Runtime_Shmoo_para.TX_Byte = RUNTIME_SHMOO_TEST_BYTE;
#endif

#if RUNTIME_SHMOO_RX
        p->pSavetimeData->Runtime_Shmoo_para.flag= RUNTIME_SHMOO_ON_GOING;
        p->pSavetimeData->Runtime_Shmoo_para.Scan_Direction=0;
        p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Range = 0;
        p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Value = 0;
        for (u1BitIdx=0; u1BitIdx<8; u1BitIdx++)
        {
            p->pSavetimeData->Runtime_Shmoo_para.RX_delay[u1BitIdx] = FinalWinPerBit[u1BitIdx].first_pass- RUNTIME_SHMOO_RX_TEST_MARGIN;
        }
        p->pSavetimeData->Runtime_Shmoo_para.RX_Original_delay = p->pSavetimeData->Runtime_Shmoo_para.RX_delay[0];
        p->pSavetimeData->Runtime_Shmoo_para.TX_Channel = RUNTIME_SHMOO_TEST_CHANNEL;
        p->pSavetimeData->Runtime_Shmoo_para.TX_Rank = RUNTIME_SHMOO_TEST_RANK;
        p->pSavetimeData->Runtime_Shmoo_para.TX_Byte = RUNTIME_SHMOO_TEST_BYTE;
#endif
    }

    else if ((dramc_get_rshmoo_step()) && (p->pSavetimeData->Runtime_Shmoo_para.flag != RUNTIME_SHMOO_END))
    {

        if (RUNTIME_SHMOO_FAST_K == 0)
        {
            p->pSavetimeData->Runtime_Shmoo_para.Scan_Direction=0;
        }
        else
        {
            if (dramc_get_rshmoo_pass_cnt() > (RUNTIME_SHMOO_FAST_K-1))
            {
                if (p->pSavetimeData->Runtime_Shmoo_para.Scan_Direction==0)
                {
                    p->pSavetimeData->Runtime_Shmoo_para.Scan_Direction=1;
                }
                else
                {
                    p->pSavetimeData->Runtime_Shmoo_para.Scan_Direction=0;
                }

                scan_invert_flag = 1;
            }
        }

        if (scan_invert_flag == 0)
        {
            if (p->pSavetimeData->Runtime_Shmoo_para.Scan_Direction==0)
            {
#if RUNTIME_SHMOO_TX
        p->pSavetimeData->Runtime_Shmoo_para.TX_PI_delay += RUNTIME_SHMOO_TEST_PI_DELAY_STEP;
        if (p->pSavetimeData->Runtime_Shmoo_para.TX_PI_delay > p->pSavetimeData->Runtime_Shmoo_para.TX_Original_PI_delay+RUNTIME_SHMOO_TEST_PI_DELAY_END)
        {
            p->pSavetimeData->Runtime_Shmoo_para.TX_PI_delay = p->pSavetimeData->Runtime_Shmoo_para.TX_Original_PI_delay;
            p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Value += RUNTIME_SHMOO_TEST_VREF_STEP;

            if ((p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Value+p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Range*30) > RUNTIME_SHMOO_TEST_VREF_END)
            {
                p->pSavetimeData->Runtime_Shmoo_para.flag= RUNTIME_SHMOO_END;
            }
            else
            {
                if (p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Range==0)
                {
                    if (p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Value > 50)
                    {
                        p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Range = 1;
                        p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Value -= 30;
                    }
                }
            }
        }

        p->pSavetimeData->Runtime_Shmoo_para.TX_DQM_PI_delay += RUNTIME_SHMOO_TEST_PI_DELAY_STEP;
        if (p->pSavetimeData->Runtime_Shmoo_para.TX_DQM_PI_delay > p->pSavetimeData->Runtime_Shmoo_para.TX_Original_DQM_PI_delay+RUNTIME_SHMOO_TEST_PI_DELAY_END)
        {
            p->pSavetimeData->Runtime_Shmoo_para.TX_DQM_PI_delay = p->pSavetimeData->Runtime_Shmoo_para.TX_Original_DQM_PI_delay;
        }
#endif

#if RUNTIME_SHMOO_RX
                for (u1BitIdx=0; u1BitIdx<8; u1BitIdx++)
                {
                    p->pSavetimeData->Runtime_Shmoo_para.RX_delay[u1BitIdx] += RUNTIME_SHMOO_TEST_PI_DELAY_STEP;
                }
                if (p->pSavetimeData->Runtime_Shmoo_para.RX_delay[rx_last_delay_bit] > rx_last_delay+RUNTIME_SHMOO_RX_TEST_MARGIN)
                {
                    for (u1BitIdx=0; u1BitIdx<8; u1BitIdx++)
                    {
                        p->pSavetimeData->Runtime_Shmoo_para.RX_delay[u1BitIdx] = FinalWinPerBit[u1BitIdx].first_pass - RUNTIME_SHMOO_RX_TEST_MARGIN;
                    }
                    p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Value += RUNTIME_SHMOO_TEST_VREF_STEP;
                    if ((p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Value) > RUNTIME_SHMOO_RX_VREF_RANGE_END)
                    {
                        p->pSavetimeData->Runtime_Shmoo_para.flag= RUNTIME_SHMOO_END;
                    }
                }
#endif
    }
            else
            {
#if RUNTIME_SHMOO_TX
                p->pSavetimeData->Runtime_Shmoo_para.TX_PI_delay -= RUNTIME_SHMOO_TEST_PI_DELAY_STEP;
                if (p->pSavetimeData->Runtime_Shmoo_para.TX_PI_delay < p->pSavetimeData->Runtime_Shmoo_para.TX_Original_PI_delay)
                {
                    p->pSavetimeData->Runtime_Shmoo_para.TX_PI_delay = p->pSavetimeData->Runtime_Shmoo_para.TX_Original_PI_delay + (RUNTIME_SHMOO_TEST_PI_DELAY_END+1);
                    p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Value += RUNTIME_SHMOO_TEST_VREF_STEP;

                    if ((p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Value+p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Range*30) > RUNTIME_SHMOO_TEST_VREF_END)
                    {
                        p->pSavetimeData->Runtime_Shmoo_para.flag= RUNTIME_SHMOO_END;
                    }
                    else
                    {
                        if (p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Range==0)
                        {
                            if (p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Value > 50)
                            {
                                p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Range = 1;
                                p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Value -= 30;
                            }
                        }
                    }
                }

                p->pSavetimeData->Runtime_Shmoo_para.TX_DQM_PI_delay -= RUNTIME_SHMOO_TEST_PI_DELAY_STEP;
                if (p->pSavetimeData->Runtime_Shmoo_para.TX_DQM_PI_delay < p->pSavetimeData->Runtime_Shmoo_para.TX_Original_DQM_PI_delay)
                {
                    p->pSavetimeData->Runtime_Shmoo_para.TX_DQM_PI_delay = p->pSavetimeData->Runtime_Shmoo_para.TX_Original_DQM_PI_delay + (RUNTIME_SHMOO_TEST_PI_DELAY_END+1);
                }
#endif

#if RUNTIME_SHMOO_RX
                for (u1BitIdx=0; u1BitIdx<8; u1BitIdx++)
                {
                    p->pSavetimeData->Runtime_Shmoo_para.RX_delay[u1BitIdx] -= RUNTIME_SHMOO_TEST_PI_DELAY_STEP;
                }
                if (p->pSavetimeData->Runtime_Shmoo_para.RX_delay[rx_first_delay_bit] < rx_first_delay-RUNTIME_SHMOO_RX_TEST_MARGIN)
                {
                    for (u1BitIdx=0; u1BitIdx<8; u1BitIdx++)
                    {
                        p->pSavetimeData->Runtime_Shmoo_para.RX_delay[u1BitIdx] = FinalWinPerBit[u1BitIdx].last_pass + RUNTIME_SHMOO_RX_TEST_MARGIN;
                    }
                    p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Value += RUNTIME_SHMOO_TEST_VREF_STEP;
                    if ((p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Value) > RUNTIME_SHMOO_RX_VREF_RANGE_END)
                    {
                        p->pSavetimeData->Runtime_Shmoo_para.flag= RUNTIME_SHMOO_END;
                    }
                }
#endif

            }
        }
        else
        {
            if (p->pSavetimeData->Runtime_Shmoo_para.Scan_Direction==0)
            {
#if RUNTIME_SHMOO_TX
                p->pSavetimeData->Runtime_Shmoo_para.TX_PI_delay = p->pSavetimeData->Runtime_Shmoo_para.TX_Original_PI_delay;
                p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Value += RUNTIME_SHMOO_TEST_VREF_STEP;

                if ((p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Value+p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Range*30) > RUNTIME_SHMOO_TEST_VREF_END)
                {
                    p->pSavetimeData->Runtime_Shmoo_para.flag= RUNTIME_SHMOO_END;
                }
                else
                {
                    if (p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Range==0)
                    {
                        if (p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Value > 50)
                        {
                            p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Range = 1;
                            p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Value -= 30;
                        }
                    }
                }

                p->pSavetimeData->Runtime_Shmoo_para.TX_DQM_PI_delay = p->pSavetimeData->Runtime_Shmoo_para.TX_Original_DQM_PI_delay;
#endif

#if RUNTIME_SHMOO_RX
                for (u1BitIdx=0; u1BitIdx<8; u1BitIdx++)
                {
                    p->pSavetimeData->Runtime_Shmoo_para.RX_delay[u1BitIdx] = FinalWinPerBit[u1BitIdx].first_pass - RUNTIME_SHMOO_RX_TEST_MARGIN;
                }
                p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Value += RUNTIME_SHMOO_TEST_VREF_STEP;

                if ((p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Value) > RUNTIME_SHMOO_RX_VREF_RANGE_END)
                {
                    p->pSavetimeData->Runtime_Shmoo_para.flag= RUNTIME_SHMOO_END;
                }
#endif
            }
            else
            {
#if RUNTIME_SHMOO_TX
                p->pSavetimeData->Runtime_Shmoo_para.TX_PI_delay = p->pSavetimeData->Runtime_Shmoo_para.TX_Original_PI_delay + (RUNTIME_SHMOO_TEST_PI_DELAY_END+1);
                //p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Value += RUNTIME_SHMOO_TEST_VREF_STEP;

                p->pSavetimeData->Runtime_Shmoo_para.TX_DQM_PI_delay = p->pSavetimeData->Runtime_Shmoo_para.TX_Original_DQM_PI_delay + (RUNTIME_SHMOO_TEST_PI_DELAY_END+1);
#endif

#if RUNTIME_SHMOO_RX
                for (u1BitIdx=0; u1BitIdx<8; u1BitIdx++)
                {
                    p->pSavetimeData->Runtime_Shmoo_para.RX_delay[u1BitIdx] = FinalWinPerBit[u1BitIdx].last_pass + RUNTIME_SHMOO_RX_TEST_MARGIN;
                }
#endif
            }
        }
    }

#if __ETT__
    mcSHOW_ERR_MSG(("Fra RunTime Shmoo CH%d, Rank%d, Byte%d\n",RUNTIME_SHMOO_TEST_CHANNEL, RUNTIME_SHMOO_TEST_RANK, RUNTIME_SHMOO_TEST_BYTE ));
#else
    print("Fra RunTime Shmoo CH%d, Rank%d, Byte%d\n",RUNTIME_SHMOO_TEST_CHANNEL, RUNTIME_SHMOO_TEST_RANK, RUNTIME_SHMOO_TEST_BYTE );
#endif


    {
#if RUNTIME_SHMOO_TX
#if __ETT__
        mcSHOW_ERR_MSG(("Fra RunTime Shmoo original K TX Vref = (%d, %d)\n", (u1MR14Value[RUNTIME_SHMOO_TEST_CHANNEL][RUNTIME_SHMOO_TEST_RANK][p->dram_fsp]>>6) & 1, u1MR14Value[RUNTIME_SHMOO_TEST_CHANNEL][RUNTIME_SHMOO_TEST_RANK][p->dram_fsp] & 0x3f));
        mcSHOW_ERR_MSG(("Fra RunTime Shmoo original K TX Byte%d PI Delay = %d\n", RUNTIME_SHMOO_TEST_BYTE, p->pSavetimeData->Runtime_Shmoo_para.TX_Original_PI_delay+32-RUNTIME_SHMOO_TEST_PI_DELAY_START));

        mcSHOW_ERR_MSG(("Fra RunTime Shmoo TX Vref = (%d, %d)\n", p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Range, p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Value));
        mcSHOW_ERR_MSG(("Fra RunTime Shmoo TX Byte%d PI Delay = %d\n", RUNTIME_SHMOO_TEST_BYTE, p->pSavetimeData->Runtime_Shmoo_para.TX_PI_delay-p->pSavetimeData->Runtime_Shmoo_para.TX_Original_PI_delay));
#else
        print("Fra RunTime Shmoo original K TX Vref = (%d, %d)\n", (u1MR14Value[RUNTIME_SHMOO_TEST_CHANNEL][RUNTIME_SHMOO_TEST_RANK][p->dram_fsp]>>6) & 1, u1MR14Value[RUNTIME_SHMOO_TEST_CHANNEL][RUNTIME_SHMOO_TEST_RANK][p->dram_fsp] & 0x3f);
        print("Fra RunTime Shmoo original K TX Byte%d PI Delay = %d\n", RUNTIME_SHMOO_TEST_BYTE, p->pSavetimeData->Runtime_Shmoo_para.TX_Original_PI_delay+32-RUNTIME_SHMOO_TEST_PI_DELAY_START);

        print("Fra RunTime Shmoo TX Vref = (%d, %d)\n", p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Range, p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Value);
        print("Fra RunTime Shmoo TX Byte%d PI Delay = %d\n", RUNTIME_SHMOO_TEST_BYTE, p->pSavetimeData->Runtime_Shmoo_para.TX_PI_delay-p->pSavetimeData->Runtime_Shmoo_para.TX_Original_PI_delay);
#endif
#endif

#if RUNTIME_SHMOO_RX
        mcSHOW_ERR_MSG(("Fra RunTime Shmoo K RX Vref = %d\n", p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Value));
        mcSHOW_ERR_MSG(("Fra RunTime Shmoo RX Byte%d Delay = %d\n", RUNTIME_SHMOO_TEST_BYTE, p->pSavetimeData->Runtime_Shmoo_para.RX_delay[0]));
#endif

#if RUNTIME_SHMOO_TX
        TxWinTransferDelayToUIPI(p, TX_DQ_DQS_MOVE_DQ_DQM, p->pSavetimeData->Runtime_Shmoo_para.TX_PI_delay, 1, &ui_large_value, &ui_small_value, &pi_value);
        TxWinTransferDelayToUIPI(p, TX_DQ_DQS_MOVE_DQ_DQM, p->pSavetimeData->Runtime_Shmoo_para.TX_DQM_PI_delay, 1, &ui_dqm_large_value, &ui_dqm_small_value, &pi_dqm_value);

        if (RUNTIME_SHMOO_TEST_BYTE == 0)
        {
            vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ0), ui_large_value, SHURK_SELPH_DQ0_TXDLY_DQ0);
            vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ2), ui_small_value, SHURK_SELPH_DQ2_DLY_DQ0);
            vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_DQ0), pi_value, SHU_R0_B0_DQ0_SW_ARPI_DQ_B0);

            vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ1), ui_dqm_large_value, SHURK_SELPH_DQ1_TXDLY_DQM0);
            vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ3), ui_dqm_small_value, SHURK_SELPH_DQ3_DLY_DQM0);
            vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_DQ0), pi_dqm_value, SHU_R0_B0_DQ0_SW_ARPI_DQM_B0);
        }
        else
        {
            vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ0), ui_large_value, SHURK_SELPH_DQ0_TXDLY_DQ1);
            vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ2), ui_small_value, SHURK_SELPH_DQ2_DLY_DQ1);
            vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_DQ0), pi_value, SHU_R0_B1_DQ0_SW_ARPI_DQ_B1);

            vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ1), ui_dqm_large_value, SHURK_SELPH_DQ1_TXDLY_DQM1);
            vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ3), ui_dqm_small_value, SHURK_SELPH_DQ3_DLY_DQM1);
            vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_DQ0), pi_dqm_value, SHU_R0_B1_DQ0_SW_ARPI_DQM_B1);
        }
        DramcTXSetVref(p, p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Range, p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Value);


    #if 0
    #if ENABLE_MCK8X_MODE
        vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_SHU_CA_DLL0), 0x0, SHU_CA_DLL0_RG_ARDLL_PHDET_EN_CA_SHU);
        vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_SHU_B0_DLL0), 0x0, SHU_B0_DLL0_RG_ARDLL_PHDET_EN_B0_SHU);
        vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_SHU_B1_DLL0), 0x0, SHU_B1_DLL0_RG_ARDLL_PHDET_EN_B1_SHU);
    #else
        vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_CA_DLL_ARPI2), 0x0, CA_DLL_ARPI2_RG_ARDLL_PHDET_EN_CA);
        vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_B0_DLL_ARPI2), 0x0, B0_DLL_ARPI2_RG_ARDLL_PHDET_EN_B0);
        vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_B1_DLL_ARPI2), 0x0, B1_DLL_ARPI2_RG_ARDLL_PHDET_EN_B1);
    #endif
    #endif
        vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B0_DLL1, 0x0, SHU_B0_DLL1_RG_ARDLL_PHDET_EN_B0);
        vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B1_DLL1, 0x0, SHU_B1_DLL1_RG_ARDLL_PHDET_EN_B1);
        vIO32WriteFldAlign_All(DDRPHY_REG_SHU_CA_DLL1, 0x0, SHU_CA_DLL1_RG_ARDLL_PHDET_EN_CA);

#endif



#if RUNTIME_SHMOO_RX

    if (p->pSavetimeData->Runtime_Shmoo_para.RX_delay[0] <=0)
    {
#if 0

        vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_SHU1_R0_B0_DQ6 + 0x50*RUNTIME_SHMOO_TEST_BYTE), \
                                P_Fld((-p->pSavetimeData->Runtime_Shmoo_para.RX_delay[0]),SHU1_R0_B0_DQ6_RK0_RX_ARDQS0_R_DLY_B0) |P_Fld((-p->pSavetimeData->Runtime_Shmoo_para.RX_delay[0]),SHU1_R0_B0_DQ6_RK0_RX_ARDQS0_F_DLY_B0));


        vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_SHU1_R0_B0_DQ6 + 0x50*RUNTIME_SHMOO_TEST_BYTE), \
                                P_Fld(0,SHU1_R0_B0_DQ6_RK0_RX_ARDQM0_R_DLY_B0) |P_Fld(0,SHU1_R0_B0_DQ6_RK0_RX_ARDQM0_F_DLY_B0));

        DramPhyReset(p);
#endif

        for (u1BitIdx=0; u1BitIdx<4; u1BitIdx++)
        {
                vIO32Write4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_RXDLY0 + DDRPHY_AO_B0_B1_OFFSET*RUNTIME_SHMOO_TEST_BYTE + u1BitIdx*4), 0);
        }
    }
    else
    {
#if 0

        vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_SHU1_R0_B0_DQ6 + 0x50*RUNTIME_SHMOO_TEST_BYTE), \
                                P_Fld(0,SHU1_R0_B0_DQ6_RK0_RX_ARDQS0_R_DLY_B0) |P_Fld(0,SHU1_R0_B0_DQ6_RK0_RX_ARDQS0_F_DLY_B0));



        vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_SHU1_R0_B0_DQ6 + 0x50*RUNTIME_SHMOO_TEST_BYTE), \
                                P_Fld(p->pSavetimeData->Runtime_Shmoo_para.RX_delay,SHU1_R0_B0_DQ6_RK0_RX_ARDQM0_R_DLY_B0) |P_Fld(p->pSavetimeData->Runtime_Shmoo_para.RX_delay,SHU1_R0_B0_DQ6_RK0_RX_ARDQM0_F_DLY_B0));

        DramPhyReset(p);
#endif

        for (u1BitIdx=0; u1BitIdx<8; u1BitIdx+=2)
        {
             vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_RXDLY0+ (DDRPHY_AO_B0_B1_OFFSET*RUNTIME_SHMOO_TEST_BYTE) +u1BitIdx*2), \
                                            P_Fld(((U32)p->pSavetimeData->Runtime_Shmoo_para.RX_delay[u1BitIdx]),SHU_R0_B0_RXDLY0_RX_ARDQ0_R_DLY_B0) |\
                                            P_Fld(((U32)p->pSavetimeData->Runtime_Shmoo_para.RX_delay[u1BitIdx+1]),SHU_R0_B0_RXDLY0_RX_ARDQ1_R_DLY_B0));

        }
    }


    vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ5 + DDRPHY_AO_B0_B1_OFFSET*RUNTIME_SHMOO_TEST_BYTE), p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Value, SHU_B0_DQ5_RG_RX_ARDQ_VREF_SEL_B0);
#endif
    }


#if EMMC_READY
    write_offline_dram_calibration_data(vGet_Current_SRAMIdx(p), p->pSavetimeData);
#endif
    mcSHOW_ERR_MSG(("Fra Save calibration result to emmc\n"));


#if RUNTIME_SHMOO_TX
    dramc_set_rshmoo_info(p->pSavetimeData->Runtime_Shmoo_para.TX_Rank, p->pSavetimeData->Runtime_Shmoo_para.TX_Channel,
        p->pSavetimeData->Runtime_Shmoo_para.TX_Byte, p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Range, p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Value,
        p->pSavetimeData->Runtime_Shmoo_para.TX_PI_delay-p->pSavetimeData->Runtime_Shmoo_para.TX_Original_PI_delay, 1, (p->pSavetimeData->Runtime_Shmoo_para.flag == RUNTIME_SHMOO_END) ? 1 : 0);
#endif

#if RUNTIME_SHMOO_RX
    dramc_set_rshmoo_info(p->pSavetimeData->Runtime_Shmoo_para.TX_Rank, p->pSavetimeData->Runtime_Shmoo_para.TX_Channel,
        p->pSavetimeData->Runtime_Shmoo_para.TX_Byte, p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Range, p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Value,
        p->pSavetimeData->Runtime_Shmoo_para.RX_delay[0], 1, (p->pSavetimeData->Runtime_Shmoo_para.flag == RUNTIME_SHMOO_END) ? 1 : 0);

//    vAutoRefreshSwitch(p, DISABLE);

#endif

    p->channel = backup_channel;
    p->rank = backup_rank;
}
#endif



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

#if __ETT__
void ett_set_emi_rank1_address(void)
{
    static unsigned int remap_rank1_done = 0;

    if (!remap_rank1_done)
    {
    #ifndef MT6880_FPGA
        set_emi_before_rank1_mem_test();
    #endif
        remap_rank1_done = 1;
    }
}
#endif

static void mem_test_address_calculation(DRAMC_CTX_T * p, U32 uiSrcAddr, U32*pu4Dest)
{
#if __ETT__
    ett_set_emi_rank1_address();

    *pu4Dest = uiSrcAddr - RANK0_START_VA + RANK1_START_VA;
#else
    *pu4Dest = uiSrcAddr + p->ranksize[RANK_0];
#endif
}

#if CPU_RW_TEST_AFTER_K
static void vDramCPUReadWriteTestAfterCalibration(DRAMC_CTX_T *p)
{
    U8 u1DumpInfo=0, u1RankIdx;
    U32 uiLen, uiRankdAddr[RANK_MAX];
    U32 pass_count, err_count;
    U64 count, uiFixedAddr; //[FOR_CHROMEOS] Change type from U32 to U64
    uiLen = 0xffff;

#if GATING_ONLY_FOR_DEBUG
    DramcGatingDebugInit(p);
#endif

    uiRankdAddr[0] = DDR_BASE;
    mem_test_address_calculation(p, DDR_BASE, &uiRankdAddr[1]);

    for(u1RankIdx =0; u1RankIdx< p->support_rank_num; u1RankIdx++)
    {
        u1DumpInfo=0;
        err_count=0;
        pass_count=0;

        #if !__ETT__

        if (u1RankIdx >= 1)
            continue;
        #endif

        #if GATING_ONLY_FOR_DEBUG
        DramcGatingDebugRankSel(p, u1RankIdx);
        #endif

        uiFixedAddr = uiRankdAddr[u1RankIdx];

        for (count= 0; count<uiLen; count+=4)
        {
            *(volatile unsigned int   *)(count +uiFixedAddr) = count + (0x5a5a <<16);
        }

        for (count=0; count<uiLen; count+=4)
        {
            if (*(volatile unsigned int   *)(count +uiFixedAddr) != count + (0x5a5a <<16))
            {
                //mcSHOW_DBG_MSG(("[Fail] Addr %xh = %xh\n",count, *(volatile unsigned int   *)(count)));
                err_count++;
            }
            else
                pass_count ++;
        }

#if RUNTIME_SHMOO_RELEATED_FUNCTION && SUPPORT_SAVE_TIME_FOR_CALIBRATION
        if (err_count==0)
        {
#if __ETT__
            mcSHOW_ERR_MSG(("CH %c,RANK %d,BYTE %d,VRANGE %d,VREF %d,PI %d,MEM_RESULT PASS\n",
                p->pSavetimeData->Runtime_Shmoo_para.TX_Channel == 0 ? 'A' : 'B',
                p->pSavetimeData->Runtime_Shmoo_para.TX_Rank,
                p->pSavetimeData->Runtime_Shmoo_para.TX_Byte,
                p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Range,
                p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Value,
                p->pSavetimeData->Runtime_Shmoo_para.TX_PI_delay-p->pSavetimeData->Runtime_Shmoo_para.TX_Original_PI_delay));
#else
            print("CH %c,RANK %d,BYTE %d,VRANGE %d,VREF %d,PI %d,MEM_RESULT PASS\n",
                p->pSavetimeData->Runtime_Shmoo_para.TX_Channel == 0 ? 'A' : 'B',
                p->pSavetimeData->Runtime_Shmoo_para.TX_Rank,
                p->pSavetimeData->Runtime_Shmoo_para.TX_Byte,
                p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Range,
                p->pSavetimeData->Runtime_Shmoo_para.TX_Vref_Value,
                p->pSavetimeData->Runtime_Shmoo_para.TX_PI_delay-p->pSavetimeData->Runtime_Shmoo_para.TX_Original_PI_delay);
#endif
        }
#else
        if(err_count)
        {
            mcSHOW_DBG_MSG2(("[MEM_TEST] Rank %d Fail.", u1RankIdx));
            u1DumpInfo =1;
#if defined(SLT)
            mcSHOW_ERR_MSG(("\n[EMI] EMI_FATAL_ERR_FLAG = 0x00000001, line: %d\n",__LINE__));
            while(1);
#endif
        }
        else
        {
            mcSHOW_DBG_MSG2(("[MEM_TEST] Rank %d OK.", u1RankIdx));
        }
        mcSHOW_DBG_MSG2(("(uiFixedAddr 0x%llX, Pass count =%d, Fail count =%d)\n", uiFixedAddr, pass_count, err_count)); //[FOR_CHROMEOS]
#endif
    }

    if(u1DumpInfo)
    {

        #if (FOR_DV_SIMULATION_USED==0 && SW_CHANGE_FOR_SIMULATION==0)
        DramcDumpDebugInfo(p);
        #endif
    }

    #if GATING_ONLY_FOR_DEBUG
    DramcGatingDebugExit(p);
    #endif
}
#endif


#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
#if !EMMC_READY
u32 g_dram_save_time_init_done[DRAM_DFS_SRAM_MAX] = {0};
SAVE_TIME_FOR_CALIBRATION_T SaveTimeDataByShuffle[DRAM_DFS_SRAM_MAX];
#endif
static DRAM_STATUS_T DramcSave_Time_For_Cal_End(DRAMC_CTX_T *p)
{
    if (!u1IsLP4Family(p->dram_type))
        return DRAM_FAIL;

    if (p->femmc_Ready == 0)
    {
        #if EMMC_READY
        write_offline_dram_calibration_data(vGet_Current_SRAMIdx(p), p->pSavetimeData);
        mcSHOW_DBG_MSG(("[FAST_K] Save calibration result to emmc\n"));
        #else
        g_dram_save_time_init_done[vGet_Current_SRAMIdx(p)] = 1;
        memcpy(&(SaveTimeDataByShuffle[vGet_Current_SRAMIdx(p)]), p->pSavetimeData, sizeof(SAVE_TIME_FOR_CALIBRATION_T));
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


    #if EMMC_READY

    if (read_offline_dram_calibration_data(vGet_Current_SRAMIdx(p), p->pSavetimeData) < 0)
        {
        p->femmc_Ready = 0;
        memset(p->pSavetimeData, 0, sizeof(SAVE_TIME_FOR_CALIBRATION_T));
        }
    else
        {
        p->femmc_Ready = 1;
        }

    #else

    if (g_dram_save_time_init_done[vGet_Current_SRAMIdx(p)] == 0)
    {
        p->femmc_Ready = 0;
        memset(p->pSavetimeData, 0, sizeof(SAVE_TIME_FOR_CALIBRATION_T));
    }
    else
    {
        memcpy(p->pSavetimeData, &(SaveTimeDataByShuffle[vGet_Current_SRAMIdx(p)]), sizeof(SAVE_TIME_FOR_CALIBRATION_T));
        p->femmc_Ready = 1;
    }
    #endif

    if (p->femmc_Ready == 1)
    {
        if (p->frequency < 1600)
        {
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
    mcSHOW_DBG_MSG(("[FAST_K] DramcSave_Time_For_Cal_Init SRAM_SHU%d, femmc_Ready=%d\n", vGet_Current_SRAMIdx(p), p->femmc_Ready));
#else
    mcSHOW_DBG_MSG(("[FAST_K] DramcSave_Time_For_Cal_Init SRAM_SHU%d, Init_done=%d, femmc_Ready=%d\n", vGet_Current_SRAMIdx(p), g_dram_save_time_init_done[vGet_Current_SRAMIdx(p)], p->femmc_Ready));
#endif
    mcSHOW_DBG_MSG(("[FAST_K] Bypass_RDDQC %d, Bypass_RXWINDOW=%d, Bypass_TXWINDOW=%d\n", p->Bypass_RDDQC, p->Bypass_RXWINDOW, p->Bypass_TXWINDOW));

    return DRAM_OK;
}
#endif

#if ENABLE_RANK_NUMBER_AUTO_DETECTION
static void DramRankNumberDetection(DRAMC_CTX_T *p)
{
    U8 u1RankBak;

    u1RankBak = u1GetRank(p);

    vSetPHY2ChannelMapping(p, CHANNEL_A);
    vSetRank(p, RANK_1);

    if (DramcWriteLeveling(p, AUTOK_ON, PI_BASED) == DRAM_OK)
    {
        p->support_rank_num = RANK_DUAL;
        vIO32WriteFldAlign(DRAMC_REG_SA_RESERVE, 0, SA_RESERVE_SINGLE_RANK);
    }
    else
    {
        p->support_rank_num = RANK_SINGLE;
        vIO32WriteFldAlign(DRAMC_REG_SA_RESERVE, 1, SA_RESERVE_SINGLE_RANK);
    }
    mcSHOW_DBG_MSG2(("[RankNumberDetection] %d\n", p->support_rank_num));

    vSetRank(p, u1RankBak);
}
#endif

#if (FOR_DV_SIMULATION_USED == 0)
static void UpdateGlobal10GBEnVariables(DRAMC_CTX_T *p)
{
    p->u110GBEn[RANK_0] = (get_row_width_by_emi(RANK_0) >= 18) ? ENABLE : DISABLE;
    p->u110GBEn[RANK_1] = (get_row_width_by_emi(RANK_1) >= 18) ? ENABLE : DISABLE;
    //mcSHOW_DBG_MSG(("[10GBEn] RANK0=%d, RANK1=%d\n", p->u110GBEn[RANK_0], p->u110GBEn[RANK_1]));
}
#endif

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

        vAutoRefreshSwitch(p, ENABLE);
        dramc_rx_dqs_gating_cal(p, AUTOK_OFF, 0);
        DramcRxWindowPerbitCal(p, PATTERN_RDDQC, NULL, AUTOK_OFF, NORMAL_K);

#if MRW_CHECK_ONLY
        mcSHOW_MRW_MSG(("\n==[MR Dump] %s==\n", __func__));
#endif
        vAutoRefreshSwitch(p, DISABLE);
    }

    vSetRank(p, RANK_0);

#if GATING_ADJUST_TXDLY_FOR_TRACKING
    DramcRxdqsGatingPostProcess(p);
#endif
}

static int GetDramInforAfterCalByMRR(DRAMC_CTX_T *p, DRAM_INFO_BY_MRR_T *DramInfo)
{
    U8 u1RankIdx, u1DieNumber = 0; //u1ChannelIdx, u1RankMax,
    U16 u2Density;
    U16 u2MR7;
    U16 u2MR8 = 0;
    U64 u8Size = 0, u8Size_backup = 0;
    //U64 u8ChannelSize;
    //U32 u4ChannelNumber = 1, u4RankNumber = 1;

    if (p->revision_id != REVISION_ID_MAGIC)
        return 0;

    vSetPHY2ChannelMapping(p, CHANNEL_A);


    DramcModeRegReadByRank(p, RANK_0, 5, &(p->vendor_id));
    p->vendor_id &= 0xFF;
    mcSHOW_DBG_MSG2(("[GetDramInforAfterCalByMRR] Vendor %x.\n", p->vendor_id));

    DramcModeRegReadByRank(p, RANK_0, 6, &(p->revision_id));
    mcSHOW_DBG_MSG2(("[GetDramInforAfterCalByMRR] Revision %x.\n", p->revision_id));

    DramcModeRegReadByRank(p, RANK_0, 7, &u2MR7);
    mcSHOW_DBG_MSG2(("[GetDramInforAfterCalByMRR] Revision 2 %x.\n", u2MR7));
#if (!__ETT__) && (FOR_DV_SIMULATION_USED==0)
    set_dram_mr(5, p->vendor_id);
    set_dram_mr(6, p->revision_id);
    set_dram_mr(7, u2MR7);
#endif
    if (DramInfo != NULL)
    {
        DramInfo->u2MR5VendorID = p->vendor_id;
        DramInfo->u2MR6RevisionID = p->revision_id;

            for(u1RankIdx =0; u1RankIdx<RANK_MAX; u1RankIdx++)
                DramInfo->u8MR8Density[u1RankIdx] =0;
    }


    for (u1RankIdx = 0; u1RankIdx < (p->support_rank_num); u1RankIdx++)
    {
        #if 0
        if ((p->aru4CalExecuteFlag[u1ChannelIdx][u1RankIdx] != 0) && \
            (p->aru4CalResultFlag[u1ChannelIdx][u1RankIdx] == 0))
        #endif
        {
            DramcModeRegReadByRank(p, u1RankIdx, 0, &(gu2MR0_Value[u1RankIdx]));
            mcSHOW_DBG_MSG2(("MR0 0x%x\n", gu2MR0_Value[u1RankIdx]));

            DramcModeRegReadByRank(p, u1RankIdx, 8, &u2Density);
            mcSHOW_DBG_MSG2(("MR8 0x%x\n", u2Density));
            u2MR8 |= (u2Density & 0xFF) << (u1RankIdx * 8);

            u1DieNumber = 1;
            if (((u2Density >> 6) & 0x3) == 1)
                u1DieNumber = 2;

            if (DramInfo != NULL)
                DramInfo->u1DieNum[u1RankIdx] = u1DieNumber;

            u2Density = (u2Density >> 2) & 0xf;

            switch (u2Density)
            {

                case 0x0:
                    u8Size = 0x20000000;
                    //mcSHOW_DBG_MSG(("[EMI]DRAM density = 4Gb\n"));
                    break;
                case 0x1:
                    u8Size = 0x30000000;
                    //mcSHOW_DBG_MSG(("[EMI]DRAM density = 6Gb\n"));
                    break;
                case 0x2:
                    u8Size = 0x40000000;
                    //mcSHOW_DBG_MSG(("[EMI]DRAM density = 8Gb\n"));
                    break;
                case 0x3:
                    u8Size = 0x60000000;
                    //mcSHOW_DBG_MSG(("[EMI]DRAM density = 12Gb\n"));
                    break;
                case 0x4:
                    u8Size = 0x80000000;
                    //mcSHOW_DBG_MSG(("[EMI]DRAM density = 16Gb\n"));
                    break;
                case 0x5:
                    u8Size = 0xc0000000;
                    //mcSHOW_DBG_MSG(("[EMI]DRAM density = 24Gb\n"));
                    break;
                case 0x6:
                    u8Size = 0x100000000L;
                    //mcSHOW_DBG_MSG(("[EMI]DRAM density = 32Gb\n"));
                    break;
                default:
                    u8Size = 0; //reserved
            }

    #if (fcFOR_CHIP_ID == fc8195)
            if (p->support_channel_num == CHANNEL_SINGLE)
                u8Size >>= 1;
    #endif

            if (u8Size_backup < u8Size)
            {
                u8Size_backup = u8Size;
                p->density = u2Density;
            }

            p->ranksize[u1RankIdx] = u8Size * u1DieNumber;

            if (DramInfo != NULL)
            {
                DramInfo->u8MR8Density[u1RankIdx] = p->ranksize[u1RankIdx];
            }
        }
    DramInfo->u4RankNum = p->support_rank_num;

        mcSHOW_DBG_MSG2(("RK%d, DieNum %d, Density %dGb, RKsize %dGb.\n\n", u1RankIdx, u1DieNumber, (U32)(u8Size >> 27), (U32)(p->ranksize[u1RankIdx] >> 27)));
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

#if 0
        if((p->frequency >= 2133) && (p->rank == RANK_0))
        //if (p->rank == RANK_0)
            DramcRXInputBufferOffsetCal(p);

#ifdef DDR_INIT_TIME_PROFILING
        CPU_Cycle=TimeProfileEnd();
        mcSHOW_TIME_MSG(("\tRX input cal takes %d us\n", CPU_Cycle));
        TimeProfileBegin();
#endif
#endif

#if GATING_ADJUST_TXDLY_FOR_TRACKING
    DramcRxdqsGatingPreProcess(p);
#endif

    if (p->support_rank_num==RANK_DUAL)
        u1RankMax = RANK_MAX;
    else
        u1RankMax = RANK_1;

    //vAutoRefreshSwitch(p, DISABLE);
    vAutoRefreshSwitch(p, DISABLE);

#if 1
    for(s1RankIdx=RANK_0; s1RankIdx<u1RankMax; s1RankIdx++)
    {
        vSetRank(p, s1RankIdx);
        #if PINMUX_AUTO_TEST_PER_BIT_CA
        CheckCAPinMux(p);
        #endif

        CmdBusTrainingLP45(p, AUTOK_OFF, NORMAL_K);

        #if ENABLE_EYESCAN_GRAPH
            if (GetEyeScanEnable(p, EYESCAN_TYPE_CBT) == ENABLE)
            {
                CmdBusTrainingLP45(p, AUTOK_OFF, EYESCAN_K);
                print_EYESCAN_LOG_message(p, EYESCAN_TYPE_CBT);
            }
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


    ShuffleDfsToOriginalFSP(p);
#endif

#if 0
    for(s1RankIdx=RANK_0; s1RankIdx<u1RankMax; s1RankIdx++)
    {
        vSetRank(p, s1RankIdx);

        vAutoRefreshSwitch(p, DISABLE);

#if (!WCK_LEVELING_FM_WORKAROUND)
        if (u1IsLP4Family(p->dram_type))
#endif
        {
            if (!(u1IsLP4Div4DDR800(p) && (p->rank == RANK_1)))
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

    #if ENABLE_WDQS_MODE_2
    if (!(u1IsLP4Div4DDR800(p)) && (p->frequency <= 800) && (p->support_rank_num == RANK_DUAL))
        WriteLevelingPosCal(p, PI_BASED);
    #elif ENABLE_TX_WDQS
    if (!(u1IsLP4Div4DDR800(p)) && (p->support_rank_num == RANK_DUAL))
        WriteLevelingPosCal(p, PI_BASED);
    #endif
#endif

    for(s1RankIdx=RANK_0; s1RankIdx<u1RankMax; s1RankIdx++)
    {
        vSetRank(p, s1RankIdx);

#if 1
        vAutoRefreshSwitch(p, DISABLE);

#if (!WCK_LEVELING_FM_WORKAROUND)
        if (u1IsLP4Family(p->dram_type))
#endif
        {
            if ((!((vGet_DDR_Loop_Mode(p) == SEMI_OPEN_LOOP_MODE) && (p->rank == RANK_1)))
                 && (!(vGet_DDR_Loop_Mode(p) == OPEN_LOOP_MODE))) // skip for DDR400 
            {
                //mcSHOW_DBG_MSG(("\n----->DramcWriteLeveling(PI) begin...\n"));

                DramcWriteLeveling(p, AUTOK_ON, PI_BASED);

               //mcSHOW_DBG_MSG(("DramcWriteLeveling(PI) end<-----\n\n"));
            }

            #ifdef DDR_INIT_TIME_PROFILING
            CPU_Cycle=TimeProfileEnd();
            mcSHOW_TIME_MSG(("\tRank %d Write leveling takes %d us\n", s1RankIdx, CPU_Cycle));
            TimeProfileBegin();
            #endif
        }
#endif

        #if LJPLL_FREQ_DEBUG_LOG
        DDRPhyFreqMeter(p);
        #endif

        vAutoRefreshSwitch(p, ENABLE);

        dramc_rx_dqs_gating_cal(p, AUTOK_OFF, 0);

        #ifdef DDR_INIT_TIME_PROFILING
        CPU_Cycle=TimeProfileEnd();
        mcSHOW_TIME_MSG(("\tRank %d Gating takes %d us\n", s1RankIdx, CPU_Cycle));
        TimeProfileBegin();
        #endif

        #if LJPLL_FREQ_DEBUG_LOG
            DDRPhyFreqMeter(p);
        #endif

#if ENABLE_RX_INPUT_BUFF_OFF_K
        if((p->frequency >= 2133) && (p->rank == RANK_0))
            DramcRXInputBufferOffsetCal(p);

#ifdef DDR_INIT_TIME_PROFILING
        CPU_Cycle=TimeProfileEnd();
        mcSHOW_TIME_MSG(("\tRX input cal takes %d us\n", CPU_Cycle));
        TimeProfileBegin();
#endif
#endif

        DramcRxWindowPerbitCal(p, PATTERN_RDDQC, NULL, AUTOK_OFF, NORMAL_K);

        #ifdef DDR_INIT_TIME_PROFILING
        CPU_Cycle=TimeProfileEnd();
        mcSHOW_TIME_MSG(("\tRank %d RX RDDQC takes %d us\n", s1RankIdx, CPU_Cycle));
        TimeProfileBegin();
        #endif

        #if LJPLL_FREQ_DEBUG_LOG
            DDRPhyFreqMeter(p);
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

            //mcSHOW_DBG_MSG(("[TX_K_DQM_WITH_WDBI] Step1: K DQM with DBI_ON, and check DQM window spec.\n\n"));
            vSwitchWriteDBISettings(p, DBI_ON);
            DramcTxWindowPerbitCal((DRAMC_CTX_T *) p, TX_DQ_DQS_MOVE_DQM_ONLY, FALSE, AUTOK_OFF);
            vSwitchWriteDBISettings(p, DBI_OFF);
        }
#endif

    #if ENABLE_EYESCAN_GRAPH
        if (GetEyeScanEnable(p, EYESCAN_TYPE_TX) == ENABLE)
        {
            Dramc_K_TX_EyeScan_Log(p);
            print_EYESCAN_LOG_message(p, EYESCAN_TYPE_TX);
        }
    #endif

        #ifdef DDR_INIT_TIME_PROFILING
        CPU_Cycle=TimeProfileEnd();
        mcSHOW_TIME_MSG(("\tRank %d TX calibration takes %d us\n", s1RankIdx, CPU_Cycle));
        TimeProfileBegin();
        #endif

        #if LJPLL_FREQ_DEBUG_LOG
            DDRPhyFreqMeter(p);
        #endif

        DramcRxdatlatCal(p);

        #ifdef DDR_INIT_TIME_PROFILING
        CPU_Cycle=TimeProfileEnd();
        mcSHOW_TIME_MSG(("\tRank %d Datlat takes %d us\n", s1RankIdx, CPU_Cycle));
        TimeProfileBegin();
        #endif

        #if LJPLL_FREQ_DEBUG_LOG
            DDRPhyFreqMeter(p);
        #endif

#if PINMUX_AUTO_TEST_PER_BIT_RX
        CheckRxPinMux(p);
#endif

        DramcRxWindowPerbitCal(p, PATTERN_TEST_ENGINE, NULL , AUTOK_ON, NORMAL_K);

        #ifdef DDR_INIT_TIME_PROFILING
        CPU_Cycle=TimeProfileEnd();
        mcSHOW_TIME_MSG(("\tRank %d RX calibration takes %d us\n", s1RankIdx, CPU_Cycle));
        TimeProfileBegin();
        #endif
       // DramcRxdqsGatingCal(p);

#if ENABLE_EYESCAN_GRAPH
        if (GetEyeScanEnable(p, EYESCAN_TYPE_RX) == ENABLE)
        {
            DramcRxWindowPerbitCal(p, PATTERN_TEST_ENGINE, NULL , AUTOK_ON, EYESCAN_K);
            print_EYESCAN_LOG_message(p, EYESCAN_TYPE_RX);
        }
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
        if(p->frequency >= RX_VREF_DUAL_RANK_K_FREQ)
        {
            U8 u1ByteIdx, u1HighFreqRXVref[2];
            for(u1ByteIdx =0 ; u1ByteIdx<DQS_BYTE_NUMBER; u1ByteIdx++)
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

    vSetRank(p, RANK_0);

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
    if (p->frequency >= RDSEL_TRACKING_TH)
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
    DDRPhyFreqMeter(p);
#endif
#endif

    #ifdef DDR_INIT_TIME_PROFILING
    CPU_Cycle=TimeProfileEnd();
    mcSHOW_TIME_MSG(("\tMisc takes %d us\n\n", s1RankIdx, CPU_Cycle));
    #endif
}

static void vDramCalibrationSingleChannel(DRAMC_CTX_T *p)
{
    vCalibration_Flow_LP4(p);
}

void vDramCalibrationAllChannel(DRAMC_CTX_T *p)
{
    U8 channel_idx, rank_idx;

#ifdef FOR_HQA_REPORT_USED
    print_HQA_message_before_CalibrationAllChannel(p);
#endif

#ifdef DDR_INIT_TIME_PROFILING
    PROFILING_TIME_T ptime;

    TimeProfileGetTick(&ptime);
#endif

    CmdOEOnOff(p, DISABLE, CMDOE_DIS_TO_ALL_CHANNEL);
    for (channel_idx = CHANNEL_A; channel_idx < p->support_channel_num; channel_idx++)
    {
        vSetPHY2ChannelMapping(p, channel_idx);
        CmdOEOnOff(p, ENABLE, CMDOE_DIS_TO_ONE_CHANNEL);
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

#if defined(DEVIATION)
    if(p->frequency == u2DFSGetHighestFreq(p))
    {
        SetDeviationVref(p);
        #if  !__ETT__
        vSetDeviationVariable();
        #endif
    }
#endif


#if ENABLE_READ_DBI

    vSetRank(p, RANK_0);
    vSetPHY2ChannelMapping(p, CHANNEL_A);

    DramcReadDBIOnOff(p, p->DBI_R_onoff[p->dram_fsp]);
#endif

#if ENABLE_WRITE_DBI

    if (p->DBI_W_onoff[p->dram_fsp])
    {
        for (channel_idx = CHANNEL_A; channel_idx < p->support_channel_num; channel_idx++)
        {
            vSetPHY2ChannelMapping(p, channel_idx);

            for (rank_idx = RANK_0; rank_idx < RANK_MAX; rank_idx++)
            {
                vSetRank(p, rank_idx);
                DramcWriteShiftMCKForWriteDBI(p, -1);
            }
            vSetRank(p, RANK_0);
        }
        vSetPHY2ChannelMapping(p, CHANNEL_A);


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
    //mcSHOW_DBG_MSG(("TX_TRACKING: ON\n"));
//#else
    //mcSHOW_DBG_MSG(("TX_TRACKING: OFF\n"));
#endif

#if ENABLE_DFS_RUNTIME_MRW
    DFSRuntimeMRW_preset_AfterK(p, vGet_Current_SRAMIdx(p));
#endif

#ifdef DDR_INIT_TIME_PROFILING
    mcSHOW_TIME_MSG(("  (4) vDramCalibrationAllChannel() take %d ms\n\r", TimeProfileDiffUS(&ptime) / 1000));
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
//extern int MustStop;
int Init_DRAM(DRAM_DRAM_TYPE_T dram_type, DRAM_CBT_MODE_EXTERN_T dram_cbt_mode_extern, DRAM_INFO_BY_MRR_T *DramInfo, U8 get_mdl_used)
{
    #if !SW_CHANGE_FOR_SIMULATION

    //int mem_start, len, s4value;
    DRAMC_CTX_T * p;
    //U8 ucstatus = 0;
    //U32 u4value;
    //U8 chIdx;
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

    //DramcConfInfraReset(p);
#else
    p = psCurrDramCtx;
#endif

    p->new_cbt_mode = 1;

    Set_MDL_Used_Flag(get_mdl_used);

    p->dram_type = dram_type;


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

    DramcBroadcastOnOff(DRAMC_BROADCAST_ON);

    if (gfirst_init_flag == 0)
    {
        MPLLInit();
        Global_Option_Init(p);
        gfirst_init_flag = 1;
    }

#ifdef FIRST_BRING_UP
    if (p->support_channel_num != CHANNEL_SINGLE)
        Test_Broadcast_Feature(p);
#endif

#if (FOR_DV_SIMULATION_USED == 0 && SW_CHANGE_FOR_SIMULATION == 0)
    {
        U32 backup_broadcast;
        backup_broadcast = GetDramcBroadcast();
        DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
        mdl_setting(p);
        UpdateGlobal10GBEnVariables(p);
        TA2_Test_Run_Time_HW_Set_Column_Num(p);
        DramcBroadcastOnOff(backup_broadcast);
    }
#endif

    mcSHOW_DBG_MSG(("\n\n[Bian_co] %s\n dram_type %d, R0 cbt_mode %d, R1 cbt_mode %d VENDOR=%d\n\n", _VERSION_, p->dram_type, p->dram_cbt_mode[RANK_0], p->dram_cbt_mode[RANK_1], p->vendor_id));

#if __Petrus_TO_BE_PORTING__
    vDramcInit_PreSettings(p);
#endif



#if defined(DUMP_INIT_RG_LOG_TO_DE)
    vSetDFSFreqSelByTable(p, &gFreqTbl[1]);
#else
    vSetDFSFreqSelByTable(p, &gFreqTbl[DRAM_DFS_SRAM_MAX-1]);
    //vSetDFSFreqSelByTable(p, &gFreqTbl[1]);
#endif

#if (DUAL_FREQ_K==0) || (__FLASH_TOOL_DA__)
    gAndroid_DVFS_en = FALSE;
#endif

#if RUNTIME_SHMOO_RELEATED_FUNCTION
    ett_fix_freq = 1;
#endif

    if (!CONFIG(MEDIATEK_DRAM_DVFS))
        ett_fix_freq = 0x1;

    if (ett_fix_freq != 0xff)
        gAndroid_DVFS_en = FALSE;

#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
    DramcSave_Time_For_Cal_Init(p);
#endif
#ifndef LOOPBACK_TEST
    if (p->dram_type == TYPE_LPDDR4X)
    {

        DramcImpedanceCal(p, 1, IMP_LOW_FREQ);
        DramcImpedanceCal(p, 1, IMP_HIGH_FREQ);
        #if ENABLE_SAMSUNG_NT_ODT
        DramcImpedanceCal(p, 1, IMP_NT_ODTN);
        #endif
    }
    else
    {
        mcSHOW_ERR_MSG(("[DramcImpedanceCal] Warnning: Need confirm DRAM type for SW IMP Calibration !!!\n"));
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
    else
#endif
    {
#if ENABLE_RANK_NUMBER_AUTO_DETECTION
        if (Get_MDL_Used_Flag()==GET_MDL_USED)
        {
            DramRankNumberDetection(p);
            DFSInitForCalibration(p);
        }
#endif

#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
        p->pSavetimeData->support_rank_num = p->support_rank_num;
#endif
    }

    #if (FOR_DV_SIMULATION_USED == 0 && SW_CHANGE_FOR_SIMULATION == 0)
    U32 backup_broadcast;
    backup_broadcast = GetDramcBroadcast();
    DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
    emi_init2();
    DramcBroadcastOnOff(backup_broadcast);
    #endif

    if (Get_MDL_Used_Flag()==GET_MDL_USED)
    {

        vSetPHY2ChannelMapping(p, CHANNEL_A);
        vCalibration_Flow_For_MDL(p);
        GetDramInforAfterCalByMRR(p, DramInfo);
        return 0;
    }
    else
    {
        vDramCalibrationAllChannel(p);
        GetDramInforAfterCalByMRR(p, DramInfo);
        vDramcACTimingOptimize(p);
    }

    #if SUPPORT_SAVE_TIME_FOR_CALIBRATION
    DramcSave_Time_For_Cal_End(p);
    #endif


#if (DUAL_FREQ_K) && (!__FLASH_TOOL_DA__)
    DramcSaveToShuffleSRAM(p, DRAM_DFS_REG_SHU0, vGet_Current_SRAMIdx(p));

    #if SUPPORT_SAVE_TIME_FOR_CALIBRATION
    DramcSave_Time_For_Cal_End(p);
    #endif
    LoadShuffleSRAMtoDramc(p, vGet_Current_SRAMIdx(p), DRAM_DFS_REG_SHU1);

    S8 u1ShuIdx;
    S8 s1ShuStart, s1ShuEnd;

    if (ett_fix_freq != 0xff) {
        s1ShuStart = ett_fix_freq;
        s1ShuEnd = ett_fix_freq;
        gFreqTbl[ett_fix_freq].vref_calibartion_enable = VREF_CALI_ON;
    } else {
        s1ShuStart = DRAM_DFS_SRAM_MAX - 2;
        s1ShuEnd = SRAM_SHU0;
    }

    for (u1ShuIdx = s1ShuStart; u1ShuIdx >= s1ShuEnd; u1ShuIdx--)
    {
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

        DramcSaveToShuffleSRAM(p, DRAM_DFS_REG_SHU0, gFreqTbl[u1ShuIdx].SRAMIdx);

        #if SUPPORT_SAVE_TIME_FOR_CALIBRATION
        DramcSave_Time_For_Cal_End(p);
        #endif
    }
#endif


#ifdef DDR_INIT_TIME_PROFILING
    TimeProfileBegin();
#endif

    vAfterCalibration(p);

#ifdef ENABLE_POST_PACKAGE_REPAIR
    PostPackageRepair();
#endif

#if __Petrus_TO_BE_PORTING__

#if 0
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



#if ETT_NO_DRAM
    //NoDramDramcRegDump(p);
    NoDramRegFill();
#endif
#endif

    #if DRAMC_MODEREG_CHECK
    DramcModeReg_Check(p);
    #endif

    #if PIN_CHECK_TOOL
    vPrintPinInfoResult(p);
    vGetErrorTypeResult(p);
    #endif

    #if CPU_RW_TEST_AFTER_K
    mcSHOW_DBG_MSG2(("\n[MEM_TEST] 02: After DFS, before run time config\n"));
    vDramCPUReadWriteTestAfterCalibration(p);
    #endif

    #if TA2_RW_TEST_AFTER_K
    mcSHOW_DBG_MSG2(("\n[TA2_TEST]\n"));
    TA2_Test_Run_Time_HW(p);
    #endif

#if __ETT__
#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
    if (!(p->femmc_Ready == 0))
#elif defined(DDR_INIT_TIME_PROFILING)
if (u2TimeProfileCnt == (DDR_INIT_TIME_PROFILING_TEST_CNT - 1))
#endif
#endif
    {
        EnableDFSHwModeClk(p);
        mcSHOW_DBG_MSG2(("DFS_SHUFFLE_HW_MODE: ON\n"));
        if (gAndroid_DVFS_en == TRUE)
        {
#if defined(SLT)
#ifdef SLT_2400_EXIT_PRELOADER
            final_shu = SRAM_SHU0;
#else
            final_shu = SRAM_SHU0;
#endif
#else
            final_shu = SRAM_SHU0;
#endif

            vSetDFSFreqSelByTable(p, get_FreqTbl_by_SRAMIndex(p, final_shu));
#if REPLACE_DFS_RG_MODE
            DramcDFSDirectJump_SPMMode(p, SRAM_SHU1);
            DramcDFSDirectJump_SPMMode(p, final_shu);
#else
            DramcDFSDirectJump_SRAMShuRGMode(p, SRAM_SHU1);
            DramcDFSDirectJump_SRAMShuRGMode(p, final_shu);
#endif
        }


#if __Petrus_TO_BE_PORTING__
        #if (DVT_TEST_DUMMY_RD_SIDEBAND_FROM_SPM && defined(DUMMY_READ_FOR_TRACKING))
        DramcDummyReadForSPMSideBand(p);
        #endif

        EnableDramcTrackingBySPMControl(p);

        mcSHOW_DBG_MSG2(("\n\nSettings after calibration\n\n"));
        //mcDUMP_REG_MSG(("\n\nSettings after calibration\n\n"));
#endif


        DramcRunTimeConfig(p);

        #if DUMP_ALLSUH_RG
        DumpAllChAllShuRG(p);
        #endif
    }

    #if CPU_RW_TEST_AFTER_K
    mcSHOW_DBG_MSG2(("\n[MEM_TEST] 03: After run time config\n"));
    vDramCPUReadWriteTestAfterCalibration(p);
    #endif

    #if TA2_RW_TEST_AFTER_K
    mcSHOW_DBG_MSG2(("\n[TA2_TEST]\n"));
    TA2_Test_Run_Time_HW(p);
    #endif


#if (__ETT__ && CPU_RW_TEST_AFTER_K)

    //while(1)
    {
        //if ((s4value = dramc_complex_mem_test (0x46000000, 0x2000)) == 0)
        if ((s4value = dramc_complex_mem_test (0x40024000, 0x20000)) == 0)
        {
            mcSHOW_DBG_MSG3(("1st complex R/W mem test pass\n"));
        }
        else
        {
            mcSHOW_DBG_MSG3(("1st complex R/W mem test fail :-%d\n", -s4value));
#if defined(SLT)
            mcSHOW_ERR_MSG(("[EMI] EMI_FATAL_ERR_FLAG = 0x00000001, line: %d\n",__LINE__));
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
    mcSHOW_TIME_MSG(("  (5) After calibration takes %d ms\n\r", CPU_Cycle / 1000));
#endif

#endif

#if defined(FOR_HQA_REPORT_USED)
    print_HQA_SLT_BIN_message(p);
#endif

#if DVT_TEST_RX_FIFO_MISMATCH_IRQ_CLEAN
    DVTRxFifoMismatchIrqClean(p);
#endif

    //final_shu = SRAM_SHU7;
    //vSetDFSFreqSelByTable(p, get_FreqTbl_by_SRAMIndex(p, final_shu));
    //DramcDFSDirectJump(p, SRAM_SHU1);//Switch to CLRPLL
    //DramcDFSDirectJump(p, final_shu);//Switch to CLRPLL
    //if(MustStop)
    //    while(1);
    //ETT_DRM(p);

    return 0;
}



#if FOR_DV_SIMULATION_USED


void DPI_vDramCalibrationSingleChannel(DRAMC_CTX_T *DramConfig, cal_sv_rand_args_t *psra)
{
    U8 ii;


#if GATING_ADJUST_TXDLY_FOR_TRACKING
    DramcRxdqsGatingPreProcess(DramConfig);
#endif


    vAutoRefreshSwitch(DramConfig, DISABLE);

#if (SIMUILATION_CBT == 1)
    for (ii = RANK_0; ii < DramConfig->support_rank_num; ii++)
    {
        vSetRank(DramConfig, ii);

        if (!psra || psra->cbt) {
            mcSHOW_DBG_MSG6(("\n----->DramcCBT begin...\n"));
            timestamp_show();
        #if CBT_O1_PINMUX_WORKAROUND
            CmdBusTrainingLP45(DramConfig, AUTOK_OFF);
        #else
            if (psra)
                CmdBusTrainingLP45(DramConfig, psra->cbt_autok, NORMAL_K);
            else
                CmdBusTrainingLP45(DramConfig, AUTOK_OFF, NORMAL_K);
        #endif
            timestamp_show();
            mcSHOW_DBG_MSG6(("DramcCBT end<-----\n\n"));
        }
    #if ENABLE_EYESCAN_GRAPH
        if (GetEyeScanEnable(DramConfig, EYESCAN_TYPE_CBT) == ENABLE)
        {
            mcSHOW_DBG_MSG6(("CBT EYESCAN start<-----\n\n"));
            CmdBusTrainingLP45(DramConfig, AUTOK_OFF, EYESCAN_K);
            print_EYESCAN_LOG_message(DramConfig, EYESCAN_TYPE_CBT);
            mcSHOW_DBG_MSG6(("CBT EYESCAN end<-----\n\n"));
        }
       #endif
    }

    vSetRank(DramConfig, RANK_0);

    ShuffleDfsToOriginalFSP(DramConfig);


#if __A60868_TO_BE_PORTING__
    No_Parking_On_CLRPLL(DramConfig);
#endif

#endif

    for (ii = RANK_0; ii < DramConfig->support_rank_num; ii++)
    {
        vSetRank(DramConfig, ii);

        vAutoRefreshSwitch(DramConfig, DISABLE);

#if (SIMULATION_WRITE_LEVELING == 1)
#if (!WCK_LEVELING_FM_WORKAROUND)
    if (u1IsLP4Family(DramConfig->dram_type))
#endif
    {
        if (!(u1IsPhaseMode(DramConfig) && (DramConfig->rank == RANK_1)))
        {
            if (!psra || psra->wl) {
                mcSHOW_DBG_MSG6(("\n----->DramcWriteLeveling(PI) begin...\n"));
                timestamp_show();
                if (psra)
                {
                    DramcWriteLeveling(DramConfig, psra->wl_autok, PI_BASED);
                }
                else
                    DramcWriteLeveling(DramConfig, AUTOK_OFF, PI_BASED);

                timestamp_show();
                mcSHOW_DBG_MSG6(("DramcWriteLeveling(PI) end<-----\n\n"));
            }
        }
    }
#endif

        vAutoRefreshSwitch(DramConfig, ENABLE);

#if (SIMULATION_GATING == 1)
        if (!psra || psra->gating) {
            mcSHOW_DBG_MSG6(("\n----->DramcGating begin...\n"));
            timestamp_show();
            if (psra)
                dramc_rx_dqs_gating_cal(DramConfig, psra->gating_autok, 0);
            else
                dramc_rx_dqs_gating_cal(DramConfig, AUTOK_OFF, 0);
            timestamp_show();
            mcSHOW_DBG_MSG6(("DramcGating end < -----\n\n"));
        }
#endif

#if (SIMULATION_RX_RDDQC == 1)
        if (!psra || psra->rddqc) {
            mcSHOW_DBG_MSG6(("\n----->DramcRxWindowPerbitCal RDDQC begin...\n"));
            timestamp_show();

            #if 0
            p->rank = 1;

            vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_WCKCTRL), 0, SHU_WCKCTRL_WCKDUAL);
            vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_COMMON0),
                    P_Fld(1, SHU_COMMON0_LP5WCKON) |
                    P_Fld(1, SHU_COMMON0_LP5HEFF_MODE));
            vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL), 0, CKECTRL_CKE2RANK_OPT8);
            #endif
            DramcRxWindowPerbitCal(DramConfig, PATTERN_RDDQC, NULL, AUTOK_OFF, NORMAL_K);
            timestamp_show();
            mcSHOW_DBG_MSG6(("DramcRxWindowPerbitCal end<-----\n\n"));
        }

#endif

#if (SIMULATION_TX_PERBIT == 1)
        if (!psra || psra->tx_perbit) {
            mcSHOW_DBG_MSG6(("\n----->DramcTxWindowPerbitCal begin...\n"));
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
            mcSHOW_DBG_MSG6(("DramcTxWindowPerbitCal end<-----\n\n"));

        #if ENABLE_EYESCAN_GRAPH
                if (GetEyeScanEnable(DramConfig, EYESCAN_TYPE_TX) == ENABLE)
                {
                    mcSHOW_DBG_MSG6(("\n----->DramcTxEYESCAN begin...\n"));
                    Dramc_K_TX_EyeScan_Log(DramConfig);
                    print_EYESCAN_LOG_message(DramConfig, EYESCAN_TYPE_TX);
                    mcSHOW_DBG_MSG6(("\n----->DramcTxEYESCAN end...\n"));
                }
               #endif
        }
#endif

#if (SIMULATION_DATLAT == 1)
        if (1) {
            mcSHOW_DBG_MSG6(("\n----->DramcRxdatlatCal begin...\n"));
            timestamp_show();

            DramcRxdatlatCal(DramConfig);

            timestamp_show();
            mcSHOW_DBG_MSG6(("DramcRxdatlatCal end<-----\n\n"));
        }
#endif

#if (SIMULATION_RX_PERBIT == 1)
        if (!psra || psra->rx_perbit) {
            mcSHOW_DBG_MSG6(("\n----->DramcRxWindowPerbitCal begin...\n"));
            timestamp_show();
            if (psra)
                DramcRxWindowPerbitCal(DramConfig, PATTERN_TEST_ENGINE,
                        NULL , psra->rx_auto_cal, NORMAL_K);
            else
                DramcRxWindowPerbitCal(DramConfig, PATTERN_TEST_ENGINE,
                        NULL , AUTOK_OFF, NORMAL_K);
            timestamp_show();
            mcSHOW_DBG_MSG6(("DramcRxWindowPerbitCal end<-----\n\n"));

            #if ENABLE_EYESCAN_GRAPH
                if (GetEyeScanEnable(DramConfig, EYESCAN_TYPE_RX) == ENABLE)
                {
                    mcSHOW_DBG_MSG6(("DramcRxWindowPerbitCal EYESCAN start<-----\n\n"));
                    DramcRxWindowPerbitCal(DramConfig, PATTERN_TEST_ENGINE, NULL , AUTOK_ON, EYESCAN_K);
                    print_EYESCAN_LOG_message(DramConfig, EYESCAN_TYPE_RX);
                    mcSHOW_DBG_MSG6(("DramcRxWindowPerbitCal EYESCAN end<-----\n\n"));
                }
            #endif
        }
#endif

#if (SIMULATION_RX_DVS == 1)
    if (DramConfig->frequency >=2133)
        DramcRxDVSWindowCal(DramConfig);
#endif

#if TX_OE_CALIBATION
        if (DramConfig->frequency >= 1600)
        {
            DramcTxOECalibration(DramConfig);
        }
#endif

        #if ENABLE_TX_TRACKING
        #if 0
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



}

void DPI_vDramCalibrationAllChannel(DRAMC_CTX_T *DramConfig, cal_sv_rand_args_t *psra)
{
    U8 channel_idx, rank_idx;

    CKEFixOnOff(DramConfig, TO_ALL_RANK, CKE_FIXOFF, TO_ALL_CHANNEL);
    for (channel_idx = CHANNEL_A; channel_idx < DramConfig->support_channel_num; channel_idx++)
    {
        vSetPHY2ChannelMapping(DramConfig, channel_idx);
        CKEFixOnOff(DramConfig, TO_ALL_RANK, CKE_FIXON, TO_ONE_CHANNEL);
        DPI_vDramCalibrationSingleChannel(DramConfig, psra);
    }

    vSetPHY2ChannelMapping(DramConfig, CHANNEL_A);


#if ENABLE_READ_DBI
    DramcReadDBIOnOff(DramConfig, DramConfig->DBI_R_onoff[DramConfig->dram_fsp]);
#endif

#if ENABLE_WRITE_DBI

    if (DramConfig->DBI_W_onoff[DramConfig->dram_fsp])
    {
        for (channel_idx = CHANNEL_A; channel_idx < DramConfig->support_channel_num; channel_idx++)
        {
            vSetPHY2ChannelMapping(DramConfig, channel_idx);

            for (rank_idx = RANK_0; rank_idx < DramConfig->support_rank_num; rank_idx++)
            {
                vSetRank(DramConfig, rank_idx);
                DramcWriteShiftMCKForWriteDBI(DramConfig, -1);
            }
            vSetRank(DramConfig, RANK_0);
        }
        vSetPHY2ChannelMapping(DramConfig, CHANNEL_A);


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

#if ENABLE_TX_TRACKING
    U8 backup_channel = DramConfig->channel;
    U8 channelIdx;

    for (channelIdx = CHANNEL_A; channelIdx < DramConfig->support_channel_num; channelIdx++)
    {
        vSetPHY2ChannelMapping(DramConfig, channelIdx);
        DramcHwDQSOSC(DramConfig);
    }

    vSetPHY2ChannelMapping(DramConfig, backup_channel);
    mcSHOW_DBG_MSG6(("TX_TRACKING: ON\n"));
#else
    mcSHOW_DBG_MSG6(("TX_TRACKING: OFF\n"));
#endif



}


#if __A60868_TO_BE_PORTING__
void RG_dummy_write(DRAMC_CTX_T *p, U32 pattern)
{
    unsigned int ii;
    for (ii = 0; ii < 20; ii++)
        vIO32WriteFldAlign(DDRPHY_RFU_0X1D4, pattern, RFU_0X1D4_RESERVED_0X1D4);
}

void EnablePLLtoSPMControl(DRAMC_CTX_T *p)
{
    vIO32WriteFldAlign_All(DDRPHY_MISC_SPM_CTRL1, 0, MISC_SPM_CTRL1_SPM_DVFS_CONTROL_SEL);
}
#endif


void dump_dramc_ctx(DRAMC_CTX_T *p)
{
    mcSHOW_DBG_MSG6(("== DRAMC_CTX_T ==\n"));
    mcSHOW_DBG_MSG6(("support_channel_num:    %d\n", p->support_channel_num));
    mcSHOW_DBG_MSG6(("channel:                %d\n", p->channel));
    mcSHOW_DBG_MSG6(("support_rank_num:       %d\n", p->support_rank_num));
    mcSHOW_DBG_MSG6(("rank:                   %d\n", p->rank));
    mcSHOW_DBG_MSG6(("freq_sel:               %d\n", p->freq_sel));
    mcSHOW_DBG_MSG6(("SRAMIdx:               %d\n", vGet_Current_SRAMIdx(p)));
    mcSHOW_DBG_MSG6(("dram_type:              %d\n", p->dram_type));
    mcSHOW_DBG_MSG6(("dram_fsp:               %d\n", p->dram_fsp));
    mcSHOW_DBG_MSG6(("odt_onoff:              %d\n", p->odt_onoff));
    mcSHOW_DBG_MSG6(("dram_cbt_mode:          %d, %d\n", (int)p->dram_cbt_mode[0], (int)p->dram_cbt_mode[1]));
    mcSHOW_DBG_MSG6(("DBI_R_onoff:            %d, %d\n", (int)p->DBI_R_onoff[0], (int)p->DBI_R_onoff[1]));
    mcSHOW_DBG_MSG6(("DBI_W_onoff:            %d, %d\n", (int)p->DBI_W_onoff[0], (int)p->DBI_W_onoff[1]));
    mcSHOW_DBG_MSG6(("data_width:             %d\n", p->data_width));
    mcSHOW_DBG_MSG6(("test2_1:             0x%x\n", p->test2_1));
    mcSHOW_DBG_MSG6(("test2_2:             0x%x\n", p->test2_2));
    mcSHOW_DBG_MSG6(("frequency:              %d\n", p->frequency));
    mcSHOW_DBG_MSG6(("freqGroup:              %d\n", p->freqGroup));
    mcSHOW_DBG_MSG6(("lp5_training_mode:              %d\n", p->lp5_training_mode));
    mcSHOW_DBG_MSG6(("lp5_cbt_phase:              %d\n", p->lp5_cbt_phase));
    mcSHOW_DBG_MSG6(("new_cbt_mode:              %d\n", p->new_cbt_mode));
    mcSHOW_DBG_MSG6(("u1PLLMode:              %d\n", p->u1PLLMode));
    mcSHOW_DBG_MSG6(("curDBIState:              %d\n", p->curDBIState));
}


void DPI_SW_main_LP4(DRAMC_CTX_T *ExtConfig, cal_sv_rand_args_t *psra)
{
    u32 value;
#if DV_SIMULATION_DFS
    S8 s1ShuIdx;
#endif

    DRAMC_CTX_T *p = &DramCtx_LPDDR4;

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

#if 0
DRAM_DFS_FREQUENCY_TABLE_T gFreqTbl[DRAM_DFS_SRAM_MAX] = {
    {LP4_DDR3200 /*0*/, DIV8_MODE, SRAM_SHU1, DUTY_LAST_K,  VREF_CALI_OFF,  CLOSE_LOOP_MODE},
    {LP4_DDR4266 /*1*/, DIV8_MODE, SRAM_SHU0, DUTY_NEED_K,  VREF_CALI_ON,   CLOSE_LOOP_MODE},
    {LP4_DDR800  /*2*/, DIV4_MODE, SRAM_SHU6, DUTY_DEFAULT, VREF_CALI_OFF,  SEMI_OPEN_LOOP_MODE},
    {LP4_DDR1866 /*3*/, DIV8_MODE, SRAM_SHU3, DUTY_LAST_K,  VREF_CALI_OFF,  CLOSE_LOOP_MODE},
    {LP4_DDR1200 /*4*/, DIV8_MODE, SRAM_SHU5, DUTY_LAST_K,  VREF_CALI_OFF,  CLOSE_LOOP_MODE},
    {LP4_DDR2400 /*5*/, DIV8_MODE, SRAM_SHU2, DUTY_NEED_K,  VREF_CALI_ON,   CLOSE_LOOP_MODE},
    {LP4_DDR1600 /*6*/, DIV8_MODE, SRAM_SHU4, DUTY_DEFAULT, VREF_CALI_ON,  CLOSE_LOOP_MODE},
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
            p->pDFSTable = &gFreqTbl[DRAM_DFS_SRAM_MAX - 1];
        }
        else if(ExtConfig->freq_sel==LP4_DDR800)
        {
            p->pDFSTable = &gFreqTbl[2];
        }
#if ENABLE_DDR400_OPEN_LOOP_MODE_OPTION
        else if(ExtConfig->freq_sel==LP4_DDR400)
        {
            p->pDFSTable = &gFreqTbl[2];
        }
#endif
    }

    enter_function();

    if (!psra) {

        mcSHOW_DBG_MSG6(("enter SA's simulation flow.\n"));
        p->support_channel_num = CHANNEL_SINGLE;
        p->channel = CHANNEL_A;
        p->support_rank_num = RANK_DUAL;

        p->rank = RANK_0;

        #if (fcFOR_CHIP_ID == fcA60868)
        #if DV_SIMULATION_DFS
        p->pDFSTable = &gFreqTbl[DRAM_DFS_SRAM_MAX-1];
        #endif
        #endif
#if 0

        #if DV_SIMULATION_LP4
        p->dram_type = TYPE_LPDDR4X;
        //p->freq_sel = LP4_DDR3200;
        //p->frequency = 1600;
        p->freq_sel = LP4_DDR1600;
        p->frequency = 800;
        #else
        p->dram_type = TYPE_LPDDR5;
        p->freq_sel = LP5_DDR3200;
        p->frequency = 1600;
        #endif
#endif

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

        p->DBI_R_onoff[FSP_0] = DBI_OFF;
        p->DBI_R_onoff[FSP_1] = DBI_OFF;
        #if ENABLE_READ_DBI
        p->DBI_R_onoff[FSP_1] = DBI_ON;
        #else
        p->DBI_R_onoff[FSP_1] = DBI_OFF;
        #endif

        p->DBI_W_onoff[FSP_0] = DBI_OFF;
        p->DBI_W_onoff[FSP_1] = DBI_OFF;
        #if ENABLE_WRITE_DBI
        p->DBI_W_onoff[FSP_1] = DBI_ON;
        #else
        p->DBI_W_onoff[FSP_1] = DBI_OFF;
        #endif

        p->data_width = DATA_WIDTH_16BIT;

        p->test2_1 = DEFAULT_TEST2_1_CAL;
        p->test2_2 = DEFAULT_TEST2_2_CAL;

        p->test_pattern = TEST_XTALK_PATTERN;

        p->u2DelayCellTimex100 = 250;
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

        mcSHOW_DBG_MSG6(("enter DV's regression flow.\n"));
        p->support_channel_num = CHANNEL_SINGLE;
        p->channel = psra->calibration_channel;
        p->support_rank_num = RANK_DUAL;

        p->rank = psra->calibration_rank;

        #if (fcFOR_CHIP_ID == fcA60868)
        #if DV_SIMULATION_DFS
        p->pDFSTable = &gFreqTbl[DRAM_DFS_SRAM_MAX-1];
        #endif
        #endif


        //p->dram_type = psra->dram_type;
        //p->freq_sel = LP5_DDR4266;
        //p->frequency = 2133;
        set_type_freq_by_svargs(p, psra);


        p->dram_fsp = FSP_0;

        p->dram_cbt_mode[RANK_0] = psra->rk0_cbt_mode;
        p->dram_cbt_mode[RANK_1] = psra->rk1_cbt_mode;


        p->DBI_R_onoff[FSP_0] = (psra->mr3_value >> 6) & 0x1;
        p->DBI_R_onoff[FSP_1] = (psra->mr3_value >> 6) & 0x1;
        p->DBI_R_onoff[FSP_2] = (psra->mr3_value >> 6) & 0x1;

        p->DBI_W_onoff[FSP_0] = (psra->mr3_value >> 7) & 0x1;
        p->DBI_W_onoff[FSP_1] = (psra->mr3_value >> 7) & 0x1;
        p->DBI_W_onoff[FSP_2] = (psra->mr3_value >> 7) & 0x1;

        p->data_width = DATA_WIDTH_16BIT;

        p->test2_1 = DEFAULT_TEST2_1_CAL;
        p->test2_2 = DEFAULT_TEST2_2_CAL;

        p->test_pattern = TEST_XTALK_PATTERN;

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


    #define __FW_VER__ "All struct move done, new RX range -- 444"

    if (u1IsLP4Family(p->dram_type)) {
        mcSHOW_DBG_MSG6(("%s enter == LP4 == ...%s\n", __FUNCTION__, __FW_VER__));
    } else {
        mcSHOW_DBG_MSG6(("%s enter == LP5 == ...%s\n", __FUNCTION__, __FW_VER__));
    }
    mcSHOW_DBG_MSG6((CHK_INCLUDE_LOCAL_HEADER));

    mcSHOW_DBG_MSG6(("SIMULATION_LP4_ZQ           ... %d\n", SIMULATION_LP4_ZQ));
    mcSHOW_DBG_MSG6(("SIMULATION_SW_IMPED         ... %d\n", SIMULATION_SW_IMPED));
    mcSHOW_DBG_MSG6(("SIMULATION_MIOCK_JMETER     ... %d\n", SIMULATION_MIOCK_JMETER));
    mcSHOW_DBG_MSG6(("SIMULATION_8PHASE           ... %d\n", SIMULATION_8PHASE));
    mcSHOW_DBG_MSG6(("SIMULATION_RX_INPUT_BUF     ... %d\n", SIMULATION_RX_INPUT_BUF));
    mcSHOW_DBG_MSG6(("SIMUILATION_CBT             ... %d\n", SIMUILATION_CBT));
    mcSHOW_DBG_MSG6(("SIMULATION_WRITE_LEVELING   ... %d\n", SIMULATION_WRITE_LEVELING));
    mcSHOW_DBG_MSG6(("SIMULATION_DUTY_CYC_MONITOR ... %d\n", SIMULATION_DUTY_CYC_MONITOR));
    mcSHOW_DBG_MSG6(("SIMULATION_GATING           ... %d\n", SIMULATION_GATING));
    mcSHOW_DBG_MSG6(("SIMULATION_DATLAT           ... %d\n", SIMULATION_DATLAT));
    mcSHOW_DBG_MSG6(("SIMULATION_RX_RDDQC         ... %d\n", SIMULATION_RX_RDDQC));
    mcSHOW_DBG_MSG6(("SIMULATION_RX_PERBIT        ... %d\n", SIMULATION_RX_PERBIT));
    mcSHOW_DBG_MSG6(("SIMULATION_TX_PERBIT        ... %d\n", SIMULATION_TX_PERBIT));
    mcSHOW_DBG_MSG6(("\n\n"));

    mcSHOW_DBG_MSG6(("============== CTX before calibration ================\n"));
    dump_dramc_ctx(p);

    DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);

    //vIO32Write4B_All2(p, DDRPHY_SHU_RK_CA_CMD1, 0x0FFF);
    value = u4Dram_Register_Read(p, DRAMC_REG_DDRCOMMON0);
    mcSHOW_DBG_MSG6(("Get Addr:0x%x, Value:0x%x\n", DRAMC_REG_DDRCOMMON0, value));

    value = u4Dram_Register_Read(p, DDRPHY_REG_SHU_RK_CA_CMD1);
    mcSHOW_DBG_MSG6(("Get Addr:0x%x, Value:0x%x\n", DDRPHY_REG_SHU_RK_CA_CMD1, value));

    value = u4Dram_Register_Read(p, DDRPHY_REG_MISC_DQO1);
    mcSHOW_DBG_MSG6(("Get Addr:0x%x, Value:0x%x\n", DDRPHY_REG_MISC_DQO1, value));

    value = u4Dram_Register_Read(p, DDRPHY_MD32_REG_SSPM_TIMER0_RESET_VAL );
    mcSHOW_DBG_MSG6(("Get Addr:0x%x, Value:0x%x\n", DDRPHY_MD32_REG_SSPM_TIMER0_RESET_VAL, value));

    DramcBroadcastOnOff(DRAMC_BROADCAST_ON);

    Global_Option_Init(p);

#if __A60868_TO_BE_PORTING__
    vDramcInit_PreSettings(p);

    DDRPhyFreqSel(p, p->pDFSTable->freq_sel);

    vSetPHY2ChannelMapping(p, p->channel);
#endif


    if (u1IsLP4Family(p->dram_type))
    {
        vSetDFSFreqSelByTable(p, p->pDFSTable);
    }
    else
    {
        DDRPhyFreqSel(p, p->freq_sel);
    }

#if (SIMULATION_SW_IMPED == 1)
    mcSHOW_DBG_MSG6(("\n----->DramcImpedanceCal begin...\n"));
    timestamp_show();

    DramcImpedanceCal(p, 1, IMP_LOW_FREQ);
    DramcImpedanceCal(p, 1, IMP_HIGH_FREQ);
    timestamp_show();
    mcSHOW_DBG_MSG6(("DramcImpedanceCal end<-----\n\n"));
#endif

#if DV_SIMULATION_INIT_C

    DramcInit(p);


    vBeforeCalibration(p);
#if __A60868_TO_BE_PORTING__
    #if DV_SIMULATION_BEFORE_K
    vApplyConfigBeforeCalibration(p);
    //vMR2InitForSimulationTest(p);
    #endif

#ifdef DUMP_INIT_RG_LOG_TO_DE
    #if 0
        mcSHOW_DUMP_INIT_RG_MSG(("\n\n\n\n\n\n===== Save to Shuffle RG ======\n"));
        DramcSaveToShuffleReg(p, DRAM_DFS_SHUFFLE_1, DRAM_DFS_SHUFFLE_3);
    #endif
        while (1);
#endif
#endif
#endif


#if (SIMULATION_MIOCK_JMETER == 1)
    mcSHOW_DBG_MSG6(("\n----->DramcMiockJmeter begin...\n"));
    timestamp_show();
    p->u2DelayCellTimex100 = GetVcoreDelayCellTime(p);
    timestamp_show();
    mcSHOW_DBG_MSG6(("DramcMiockJmeter end<-----\n\n"));
#endif

#if (SIMULATION_8PHASE == 1)
    if(is_lp5_family(p) && (p->frequency >= 2133)) {
        mcSHOW_DBG_MSG6(("\n----->Dramc8PhaseCal begin...\n"));
        timestamp_show();
        Dramc8PhaseCal(p);
        timestamp_show();
        mcSHOW_DBG_MSG6(("Dramc8PhaseCal end<-----\n\n"));
    }
#endif
    #if !DV_SIMULATION_DFS
    DPI_vDramCalibrationAllChannel(p, psra);
    #endif

#if DV_SIMULATION_DFS
    DramcSaveToShuffleSRAM(p, DRAM_DFS_REG_SHU0, vGet_Current_SRAMIdx(p));
    LoadShuffleSRAMtoDramc(p, vGet_Current_SRAMIdx(p), DRAM_DFS_REG_SHU1);

    #if (fcFOR_CHIP_ID == fcA60868)
    for (s1ShuIdx = DRAM_DFS_SRAM_MAX - 10; s1ShuIdx >= 0; s1ShuIdx--)
    #else
    for (s1ShuIdx = DRAM_DFS_SRAM_MAX - 2; s1ShuIdx >= 0; s1ShuIdx--)
    #endif
    {
        vSetDFSFreqSelByTable(p, &gFreqTbl[s1ShuIdx]);
        DramcInit(p);

        vBeforeCalibration(p);

        #if DV_SIMULATION_BEFORE_K
        vApplyConfigBeforeCalibration(p);
        #endif

        #if (SIMULATION_8PHASE == 1)
        if(is_lp5_family(p) && (p->frequency >= 2133)) {
            mcSHOW_DBG_MSG6(("\n----->Dramc8PhaseCal begin...\n"));
            timestamp_show();
            Dramc8PhaseCal(p);
            timestamp_show();
            mcSHOW_DBG_MSG6(("Dramc8PhaseCal end<-----\n\n"));
        }
        #endif

        #if !DV_SIMULATION_DFS
        DPI_vDramCalibrationAllChannel(p, psra);
        #endif
        DramcSaveToShuffleSRAM(p, DRAM_DFS_REG_SHU0, gFreqTbl[s1ShuIdx].SRAMIdx);
    }
#endif






    vAfterCalibration(p);

#if SIMULATION_RUNTIME_CONFIG
    DramcRunTimeConfig(p);
#endif

#if 0
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

#if 0
    for (s1ShuIdx = 0; s1ShuIdx < DV_SIMULATION_DFS_SHU_MAX; s1ShuIdx++)
            DramcDFSDirectJump_SRAMShuRGMode(p, gDVDFSTbl[s1ShuIdx].SRAMIdx);

    for (s1ShuIdx = DV_SIMULATION_DFS_SHU_MAX - 1; s1ShuIdx >= DRAM_DFS_SHUFFLE_1; s1ShuIdx--)
            DramcDFSDirectJump_SRAMShuRGMode(p, gDVDFSTbl[s1ShuIdx].SRAMIdx);
#endif

#if DV_SIMULATION_SPM_CONTROL
    EnablePLLtoSPMControl(p);
#endif

    RG_dummy_write(p, 0xAAAAAAAA);
#endif

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


void start_dramk(void)
{
    DRAMC_CTX_T *p;

    enter_function();
    broadcast_off();
    p = &DramCtx_LPDDR4;
    DPI_SW_main_LP4(p, NULL);
    exit_function();
}


void sa_calibration(cal_sv_rand_args_t *psra)
{
    DRAMC_CTX_T *p;

    enter_function();
    if (psra == NULL) {
        mcSHOW_DBG_MSG6(("psv_args is NULL.\n"));
        goto out;
    }

    print_sv_args(psra);
    if (!valid_magic(psra)) {
        mcSHOW_DBG_MSG6(("sv arguments is invalid.\n"));
        goto out;
    }

    set_psra(psra);
    broadcast_off();
    if (psra->calibration_channel == SV_CHN_A)
        p = &DramCtx_LPDDR4;
    else
        p = &dram_ctx_chb;

    DPI_SW_main_LP4(p, psra);

out:
    exit_function();
    return;
}



#if __A60868_TO_BE_PORTING__
#if SW_CHANGE_FOR_SIMULATION
void main(void)
{

    DRAMC_CTX_T DramConfig;
    DramConfig.channel = CHANNEL_A;
    DramConfig.support_rank_num = RANK_DUAL;

    DramConfig.rank = RANK_0;

    DramConfig.dram_type = TYPE_LPDDR4X;

    DramConfig.dram_fsp = FSP_0;

    DramConfig.dram_cbt_mode[RANK_0] = CBT_NORMAL_MODE;
    DramConfig.dram_cbt_mode[RANK_1] = CBT_NORMAL_MODE;

    DramConfig.DBI_R_onoff[FSP_0] = DBI_OFF;
    #if ENABLE_READ_DBI
    DramConfig.DBI_R_onoff[FSP_1] = DBI_ON;
    #else
    DramConfig.DBI_R_onoff[FSP_1] = DBI_OFF;
    #endif

    DramConfig.DBI_W_onoff[FSP_0] = DBI_OFF;
    #if ENABLE_WRITE_DBI
    DramConfig.DBI_W_onoff[FSP_1] = DBI_ON;
    #else
    DramConfig.DBI_W_onoff[FSP_1] = DBI_OFF;
    #endif

    DramConfig.data_width = DATA_WIDTH_32BIT;

    DramConfig.test2_1 = DEFAULT_TEST2_1_CAL;
    DramConfig.test2_2 = DEFAULT_TEST2_2_CAL;

    DramConfig.test_pattern = TEST_XTALK_PATTERN;

    DramConfig.frequency = 800;

    //DramConfig.enable_rx_scan_vref =DISABLE;
    //DramConfig.enable_tx_scan_vref =DISABLE;
    //DramConfig.dynamicODT = DISABLE;

    MPLLInit();

    Global_Option_Init(&DramConfig);


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
    DramcImpedanceCal(&DramConfig, 1, LOW_FREQ);
    //DramcImpedanceCal(&DramConfig, 1, HIGH_FREQ);
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

    DramcRxdqsGatingCal(&DramConfig);


    //DualRankDramcRxdqsGatingCal(&DramConfig);
    #endif

#if SIMUILATION_LP4_RDDQC
    DramcRxWindowPerbitCal(&DramConfig, 0, NULL);
#endif

    #if SIMULATION_DATLAT

    DramcRxdatlatCal(&DramConfig);


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

    SetDramModeRegForReadDBIOnOff(&DramConfig, DramConfig.dram_fsp, DramConfig.DBI_R_onoff[DramConfig.dram_fsp]);
    #endif

    #if ENABLE_WRITE_DBI

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
#endif
#endif
#endif

