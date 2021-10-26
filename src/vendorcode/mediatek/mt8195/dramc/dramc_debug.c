/* SPDX-License-Identifier: BSD-3-Clause */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "dramc_common.h"
#include "dramc_int_global.h"
#include "x_hal_io.h"

#if (FOR_DV_SIMULATION_USED == 0 && SW_CHANGE_FOR_SIMULATION == 0)
#include "dramc_top.h"
#ifndef MT6779_FPGA
#endif
#endif


//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

#if ENABLE_EYESCAN_GRAPH || defined(FOR_HQA_REPORT_USED)
EXTERN const U16 gRXVref_Voltage_Table_0P5V_T[EYESCAN_RX_VREF_RANGE_END];
EXTERN const U16 gRXVref_Voltage_Table_0P5V_UT[EYESCAN_RX_VREF_RANGE_END];
EXTERN const U16 gRXVref_Voltage_Table_0P3V_UT[EYESCAN_RX_VREF_RANGE_END];
EXTERN const U16 gRXVref_Voltage_Table_0P6V_T[EYESCAN_RX_VREF_RANGE_END];
EXTERN const U16 gRXVref_Voltage_Table_0P6V_UT[EYESCAN_RX_VREF_RANGE_END];
EXTERN const U16 gVref_Voltage_Table_LP4X[VREF_RANGE_MAX][VREF_VOLTAGE_TABLE_NUM_LP4];
EXTERN const U16 gVref_Voltage_Table_LP4[VREF_RANGE_MAX][VREF_VOLTAGE_TABLE_NUM_LP4];
EXTERN const U16 gVref_Voltage_Table_LP5[VREF_VOLTAGE_TABLE_NUM_LP5];
#endif

#if __ETT__
#define __NOBITS_SECTION__(x)
#endif

#if ENABLE_EYESCAN_GRAPH || defined(FOR_HQA_REPORT_USED)
const U16 gRXVref_Voltage_Table_0P5V_T[EYESCAN_RX_VREF_RANGE_END] = {
        3990,4140,4290,4440,4590,4740,4890,5030,5180,5330,
        5480,5630,5770,5920,6070,6220,6480,6630,6780,6920,
        7070,7220,7370,7520,7670,7820,7970,8110,8260,8410,
        8560,8710,8950,9110,9250,9400,9550,9700,9850,10000,
        10150,10300,10450,10590,10740,10890,11040,11190,11430,11590,
        11740,11890,12040,12190,12340,12500,12650,12800,12950,13100,
        13260,13410,13560,13710,13950,14110,14260,14410,14560,14710,
        14860,15010,15170,15320,15470,15620,15770,15920,16080,16230,
        16470,16620,16770,16920,17070,17220,17360,17510,17670,17820,
        17960,18110,18260,18410,18560,18710,18950,19100,19250,19400,
        19550,19700,19850,20000,20150,20300,20450,20600,20750,20900,
        21040,21190,21430,21580,21730,21880,22030,22180,22330,22480,
        22630,22780,22930,23080,23230,23380,23530,23680
};
const U16 gRXVref_Voltage_Table_0P5V_UT[EYESCAN_RX_VREF_RANGE_END] = {
        7890,8190,8490,8780,9080,9380,9670,9970,10270,10560,
        10860,11160,11450,11750,12050,12350,12830,13120,13420,13720,
        14010,14310,14610,14900,15200,15500,15800,16090,16390,16680,
        16980,17280,17760,18060,18350,18650,18940,19240,19540,19830,
        20130,20430,20720,21020,21320,21610,21910,22210,22690,22990,
        23290,23590,23900,24200,24500,24800,25100,25410,25710,26010,
        26310,26610,26920,27220,27700,28000,28300,28610,28910,29210,
        29510,29810,30110,30420,30720,31020,31320,31620,31920,32220,
        32700,32990,33290,33590,33880,34180,34480,34770,35070,35370,
        35670,35970,36260,36560,36860,37150,37630,37930,38220,38520,
        38820,39110,39410,39710,40010,40300,40600,40900,41190,41490,
        41790,42090,42560,42860,43150,43450,43750,44040,44340,44630,
        44930,45230,45530,45820,46120,46420,46710,47010
};
const U16 gRXVref_Voltage_Table_0P3V_UT[EYESCAN_RX_VREF_RANGE_END] = {
        4720,4900,5080,5250,5430,5600,5780,5950,6130,6310,
        6480,6660,6840,7010,7190,7370,7650,7830,8010,8180,
        8360,8540,8710,8890,9070,9240,9420,9590,9770,9950,
        10120,10300,10580,10760,10940,11110,11290,11470,11640,11820,
        12000,12170,12350,12530,12700,12880,13050,13230,13520,13700,
        13880,14060,14240,14420,14600,14780,14960,15140,15320,15500,
        15680,15860,16030,16220,16500,16680,16860,17040,17220,17400,
        17580,17760,17940,18120,18300,18480,18660,18840,19010,19190,
        19480,19660,19830,20010,20190,20360,20540,20720,20900,21070,
        21250,21430,21600,21780,21960,22130,22420,22600,22780,22950,
        23130,23310,23480,23660,23840,24010,24190,24370,24540,24720,
        24900,25080,25360,25540,25710,25890,26070,26240,26420,26600,
        26780,26950,27130,27310,27480,27660,27840,28020
};
const U16 gRXVref_Voltage_Table_0P6V_T[EYESCAN_RX_VREF_RANGE_END] = {
        4790,4970,5150,5330,5500,5680,5860,6040,6220,6390,
        6570,6750,6930,7110,7290,7470,7770,7950,8130,8310,
        8490,8660,8840,9020,9200,9380,9560,9740,9920,10100,
        10280,10460,10750,10930,11110,11280,11460,11640,11820,12000,
        12180,12360,12540,12720,12900,13080,13250,13430,13720,13910,
        14090,14270,14450,14640,14820,15000,15180,15370,15550,15730,
        15910,16100,16280,16460,16750,16940,17120,17300,17480,17660,
        17850,18030,18210,18390,18580,18760,18940,19120,19300,19490,
        19780,19960,20140,20310,20490,20670,20850,21030,21210,21390,
        21570,21750,21930,22110,22290,22470,22760,22940,23120,23300,
        23480,23650,23830,24010,24190,24370,24550,24730,24910,25090,
        25270,25450,25740,25920,26100,26280,26450,26630,26810,26990,
        27170,27350,27530,27710,27890,28070,28250,28430
};
const U16 gRXVref_Voltage_Table_0P6V_UT[EYESCAN_RX_VREF_RANGE_END] = {
        9470,9830,10190,10540,10900,11260,11610,11970,12330,12680,
        13040,13400,13750,14110,14460,14820,15400,15760,16120,16470,
        16830,17180,17540,17890,18250,18610,18970,19320,19680,20040,
        20390,20750,21330,21680,22040,22400,22750,23110,23460,23820,
        24180,24530,24890,25250,25600,25960,26310,26670,27250,27610,
        27970,28330,28700,29060,29420,29780,30150,30520,30880,31240,
        31600,31960,32330,32690,33270,33630,33990,34350,34710,35080,
        35440,35800,36160,36530,36890,37250,37610,37970,38330,38700,
        39270,39630,39980,40340,40690,41050,41410,41760,42120,42480,
        42840,43190,43550,43900,44260,44620,45190,45550,45910,46260,
        46620,46970,47330,47680,48040,48400,48760,49110,49470,49820,
        50180,50540,51120,51480,51830,52190,52540,52890,53250,53600,
        53960,54320,54670,55030,55380,55740,56090,56450
};
const U16 gRXVref_Voltage_Table_LP4[EYESCAN_RX_VREF_RANGE_END+1]={
#if __Petrus_TO_BE_PORTING__
        (364+821)/2,
        (1202+1621)/2,
        (2039+2420)/2,
        (2875+3219)/2,
        (3711+4018)/2,
        (4548+4817)/2,
        (5384+5615)/2,
        (6221+6414)/2,
        (7057+7213)/2,
        (7893+8012)/2,
        (8727+8811)/2,
        (9554+9618)/2,
        (10380+10430)/2,
        (11200+11250)/2,
        (12010+12080)/2,
        (12810+12910)/2,
        (13120+13540)/2,
        (13830+14210)/2,
        (14530+14870)/2,
        (15240+15540)/2,
        (15950+16210)/2,
        (16650+16880)/2,
        (17360+17550)/2,
        (18070+18220)/2,
        (18770+18890)/2,
        (19470+19560)/2,
        (20160+20240)/2,
        (20850+20930)/2,
        (21520+21610)/2,
        (22190+22310)/2,
        (22860+23010)/2,
        (23540+23720)/2,
        (23330+23770)/2,
        (24040+24430)/2,
        (24740+25090)/2,
        (25450+25740)/2,
        (26150+26400)/2,
        (26860+27060)/2,
        (27560+27730)/2,
        (28240+28400)/2,
        (28920+29070)/2,
        (29580+29730)/2,
        (30230+30410)/2,
        (30890+31100)/2,
        (31540+31790)/2,
        (32190+32490)/2,
        (32840+33200)/2,
        (33500+33900)/2,
        (33540+34030)/2,
        (34250+34660)/2,
        (34950+35300)/2,
        (35650+35950)/2,
        (36360+36600)/2,
        (37060+37250)/2,
        (37760+37900)/2,
        (38420+38560)/2,
        (39050+39230)/2,
        (39680+39900)/2,
        (40310+40590)/2,
        (40940+41280)/2,
        (41570+41990)/2,
        (42190+42690)/2,
        (42820+43400)/2,
        (43450+44100)/2,
#endif
};

const U16 gVref_Voltage_Table_LP4X[VREF_RANGE_MAX][VREF_VOLTAGE_TABLE_NUM_LP4] = {
    {1500, 1560, 1620, 1680, 1740, 1800, 1860, 1920, 1980, 2040, 2100, 2160, 2220, 2280, 2340, 2400, 2460, 2510, 2570, 2630, 2690, 2750, 2810, 2870, 2930, 2990, 3050, 3110, 3170, 3230, 3290, 3350, 3410, 3470, 3530, 3590, 3650, 3710, 3770, 3830, 3890, 3950, 4010, 4070, 4130, 4190, 4250, 4310, 4370, 4430, 4490},
    {3290, 3350, 3410, 3470, 3530, 3590, 3650, 3710, 3770, 3830, 3890, 3950, 4010, 4070, 4130, 4190, 4250, 4310, 4370, 4430, 4490, 4550, 4610, 4670, 4730, 4790, 4850, 4910, 4970, 5030, 5090, 5150, 5210, 5270, 5330, 5390, 5450, 5510, 5570, 5630, 5690, 5750, 5810, 5870, 5930, 5990, 6050, 6110, 6170, 6230, 6290}
};
const U16 gVref_Voltage_Table_LP4[VREF_RANGE_MAX][VREF_VOLTAGE_TABLE_NUM_LP4] = {
    {1000, 1040, 1080, 1120, 1160, 1200, 1240, 1280, 1320, 1360, 1400, 1440, 1480, 1520, 1560, 1600, 1640, 1680, 1720, 1760, 1800, 1840, 1880, 1920, 1960, 2000, 2040, 2080, 2120, 2160, 2200, 2240, 2280, 2320, 2360, 2400, 2440, 2480, 2520, 2560, 2600, 2640, 2680, 2720, 2760, 2800, 2840, 2880, 2920, 2960, 3000},
    {2200, 2240, 2280, 2320, 2360, 2400, 2440, 2480, 2520, 2560, 2600, 2640, 2680, 2720, 2760, 2800, 2840, 2880, 2920, 2960, 3000, 3040, 3080, 3120, 3160, 3200, 3240, 3280, 3320, 3360, 3400, 3440, 3480, 3520, 3560, 3600, 3640, 3680, 3720, 3760, 3880, 3840, 3880, 3920, 3960, 4000, 4040, 4080, 4120, 4160, 4200}
};
const U16 gVref_Voltage_Table_LP5[VREF_VOLTAGE_TABLE_NUM_LP5] = {
   1000,1050,1100,1150,1200,1250,1300,1350,1400,1450,
   1500,1550,1600,1650,1700,1750,1800,1850,1900,1950,
   2000,2050,2100,2150,2200,2250,2300,2350,2400,2450,
   2500,2550,2600,2650,2700,2750,2800,2850,2900,2950,
   3000,3050,3100,3150,3200,3250,3300,3350,3400,3450,
   3500,3550,3600,3650,3700,3750,3800,3850,3900,3950,
   4000,4050,4100,4150,4200,4250,4300,4350,4400,4450,
   4500,4550,4600,4650,4700,4750,4800,4850,4900,4950,
   5000,5050,5100,5150,5200,5250,5300,5350,5400,5450,
   5500,5550,5600,5650,5700,5750,5800,5850,5900,5950,
   6000,6050,6100,6150,6200,6250,6300,6350,6400,6450,
   6500,6550,6600,6650,6700,6750,6800,6850,6900,6950,
   7000,7050,7100,7150,7200,7250,7300,7350
};
#endif


#if ENABLE_EYESCAN_GRAPH
__NOBITS_SECTION__(.gEyeScan_Min) S16 gEyeScan_Min[VREF_VOLTAGE_TABLE_NUM_LP5][DQ_DATA_WIDTH][EYESCAN_BROKEN_NUM];
__NOBITS_SECTION__(.gEyeScan_Max) S16 gEyeScan_Max[VREF_VOLTAGE_TABLE_NUM_LP5][DQ_DATA_WIDTH][EYESCAN_BROKEN_NUM];
U16  gEyeScan_CaliDelay[DQS_BYTE_NUMBER];
S16 gEyeScan_MinMax_store_delay[DQS_BYTE_NUMBER];
__NOBITS_SECTION__(.gEyeScan_WinSize) U16 gEyeScan_WinSize[VREF_VOLTAGE_TABLE_NUM_LP5][DQ_DATA_WIDTH];
S16  gEyeScan_DelayCellPI[DQ_DATA_WIDTH];
U16  gEyeScan_ContinueVrefHeight[DQ_DATA_WIDTH];
U16  gEyeScan_TotalPassCount[DQ_DATA_WIDTH];
#endif

const char* HQA_LOG_Parsing_Freq(void)
{
    if (gHQA_Test_Freq_Vcore_Level != 0)
    {
        return (gHQA_Test_Freq_Vcore_Level == 1? "_L" : "_H");
    }

    return " ";
}
void HQA_LOG_Print_Prefix_String(DRAMC_CTX_T *p)
{
    mcSHOW_DBG_MSG(("[HQALOG] %d%s ", p->frequency * 2, HQA_LOG_Parsing_Freq() ));
}

void HQA_LOG_Print_Freq_String(DRAMC_CTX_T *p)
{
#if VENDER_JV_LOG
    mcSHOW_EYESCAN_MSG(("%d%s ", p->frequency * 2, HQA_LOG_Parsing_Freq() ));
#else
    mcSHOW_DBG_MSG(("%d%s ", p->frequency * 2, HQA_LOG_Parsing_Freq() ));
#endif
}

#ifdef FOR_HQA_TEST_USED
static void EYESCAN_LOG_Print_Prefix_String(void)
{
    mcSHOW_DBG_MSG(("[EYESCAN_LOG] "));
}

static const char* print_EYESCAN_LOG_type(U8 print_type)
{
    switch (print_type)
    {
        case EYESCAN_TYPE_CBT: return "CBT";
        case EYESCAN_TYPE_RX: return "RX";
        default:
        case EYESCAN_TYPE_TX: return "TX";
    }
}



void HQA_measure_message_reset_all_data(DRAMC_CTX_T *p)
{
    U32 uiCA, u1BitIdx, u1ByteIdx, u1RankIdx, u1ChannelIdx;

    for (u1ChannelIdx = CHANNEL_A; u1ChannelIdx < CHANNEL_NUM; u1ChannelIdx++)
    {
        for (u1RankIdx = RANK_0; u1RankIdx < RANK_MAX; u1RankIdx++)
        {
            for (u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
            {
                gFinalRXPerbitWin[u1ChannelIdx][u1RankIdx][u1BitIdx] = 0;
                gFinalTXPerbitWin[u1ChannelIdx][u1RankIdx][u1BitIdx] = 0;
            }
        }
    }
}

void print_HQA_SLT_BIN_message(DRAMC_CTX_T *p)
{
    S8 u1ShuIdx;
    S8 ucstatus;

    if (gHQALog_flag == 1)
    {
        ucstatus = 0;
        for (u1ShuIdx = DRAM_DFS_SRAM_MAX - 1; u1ShuIdx >= SRAM_SHU0; u1ShuIdx--)
        {
            if (gHQALog_SLT_BIN[u1ShuIdx] != 0)
            {
                mcSHOW_DBG_MSG(("SLT_BIN%d\n", gHQALog_SLT_BIN[u1ShuIdx]));
                ucstatus = 1;
            }
        }

        if (ucstatus==0)
        {
            mcSHOW_DBG_MSG(("SLT_BIN1\n"));
        }
    }
}

#if defined(FOR_HQA_REPORT_USED)
void print_HQA_message_before_CalibrationAllChannel(DRAMC_CTX_T *p)
{
    HQA_LOG_Print_Prefix_String(p); mcSHOW_DBG_MSG(("DBI_W_ONOFF %d\n", p->DBI_W_onoff[p->dram_fsp]));
}
#endif

#ifdef RELEASE
#undef mcSHOW_DBG_MSG
#define mcSHOW_DBG_MSG(_x_) opt_print _x_
#endif
void print_HQA_measure_message(DRAMC_CTX_T *p)
{
    U32 uiCA, u1BitIdx, u1ByteIdx, u1RankIdx, u1ChannelIdx;
    U32 min_ca_value[CHANNEL_NUM][RANK_MAX], min_ca_bit[CHANNEL_NUM][RANK_MAX];
    U32 min_rx_value[CHANNEL_NUM][RANK_MAX], min_tx_value[CHANNEL_NUM][RANK_MAX];
    U32 min_RX_DQ_bit[CHANNEL_NUM][RANK_MAX], min_TX_DQ_bit[CHANNEL_NUM][RANK_MAX];
    U8 SRAMIdx, local_channel_num = CHANNEL_NUM, shuffle_index;
    U8 print_imp_option[2] = {FALSE, FALSE};
    int i;
    U32 read_voltage_value;

    mcSHOW_DBG_MSG(("\n\n\n[HQA] information for measurement, "));
    mcSHOW_DBG_MSG(("\tDram Data rate = ")); HQA_LOG_Print_Freq_String(p); mcSHOW_DBG_MSG(("\n"));
    //mcDUMP_REG_MSG(("\n\n\n[HQA] information for measurement, "));
    //mcDUMP_REG_MSG(("\tDram Data rate = "));
    //mcDUMP_REG_MSG(("%d%s ", p->frequency * 2, HQA_LOG_Parsing_Freq() ));
    //mcDUMP_REG_MSG(("\n"));
    vPrintCalibrationBasicInfo(p);
    HQA_LOG_Print_Prefix_String(p);
    mcSHOW_DBG_MSG(("Frequency = %u\n", DDRPhyGetRealFreq(p)));


    if (gHQALog_flag == 1)
    {
        HQA_LOG_Print_Prefix_String(p); mcSHOW_DBG_MSG(("ETT_Commit_Version = %s\n", _COMMIT_VERSION_));
        HQA_LOG_Print_Prefix_String(p); mcSHOW_DBG_MSG(("ETT_Commit_Date = %s\n", _COMMIT_DATE_));
        HQA_LOG_Print_Prefix_String(p); mcSHOW_DBG_MSG(("Frequency = %d\n", DDRPhyGetRealFreq(p)));
        HQA_LOG_Print_Prefix_String(p); mcSHOW_DBG_MSG(("fsp = %d\n", p->dram_fsp));
        HQA_LOG_Print_Prefix_String(p); mcSHOW_DBG_MSG(("odt_onoff = %d\n", p->odt_onoff));
        HQA_LOG_Print_Prefix_String(p); mcSHOW_DBG_MSG(("byte_mode = %d\n", p->dram_cbt_mode[p->rank]));
        HQA_LOG_Print_Prefix_String(p); mcSHOW_DBG_MSG(("DivMode = %d\n", vGet_Div_Mode(p)));
    }
    SRAMIdx = vGet_Current_SRAMIdx(p);

    if (u1IsLP4Family(p->dram_type))
    {
        local_channel_num = p->support_channel_num;
    }

    for (u1ChannelIdx = CHANNEL_A; u1ChannelIdx < local_channel_num; u1ChannelIdx++)
    {
        for (u1RankIdx = RANK_0; u1RankIdx < p->support_rank_num; u1RankIdx++)
        {
            min_ca_value[u1ChannelIdx][u1RankIdx] = 0xffff;
            min_rx_value[u1ChannelIdx][u1RankIdx] = 0xffff;
            min_tx_value[u1ChannelIdx][u1RankIdx] = 0xffff;
            min_RX_DQ_bit[u1ChannelIdx][u1RankIdx] = 0xffff;
            min_TX_DQ_bit[u1ChannelIdx][u1RankIdx] = 0xffff;

            for (uiCA = 0; uiCA < ((u1IsLP4Family(p->dram_type) == 1)? CATRAINING_NUM_LP4: CATRAINING_NUM_LP5); uiCA++)
            {
                if (gFinalCBTCA[u1ChannelIdx][u1RankIdx][uiCA] < min_ca_value[u1ChannelIdx][u1RankIdx])
                {
                    min_ca_value[u1ChannelIdx][u1RankIdx] = gFinalCBTCA[u1ChannelIdx][u1RankIdx][uiCA];
                    min_ca_bit[u1ChannelIdx][u1RankIdx] = uiCA;
                }
            }

            for (u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
            {
                if (gFinalRXPerbitWin[u1ChannelIdx][u1RankIdx][u1BitIdx] < min_rx_value[u1ChannelIdx][u1RankIdx])
                {
                    min_rx_value[u1ChannelIdx][u1RankIdx] = gFinalRXPerbitWin[u1ChannelIdx][u1RankIdx][u1BitIdx];
                    min_RX_DQ_bit[u1ChannelIdx][u1RankIdx] = u1BitIdx;
                }
                if (gFinalTXPerbitWin[u1ChannelIdx][u1RankIdx][u1BitIdx] < min_tx_value[u1ChannelIdx][u1RankIdx])
                {
                    min_tx_value[u1ChannelIdx][u1RankIdx] = gFinalTXPerbitWin[u1ChannelIdx][u1RankIdx][u1BitIdx];
                    min_TX_DQ_bit[u1ChannelIdx][u1RankIdx] = u1BitIdx;
                }
            }
        }
    }


    if (p->support_rank_num == RANK_DUAL)
    {

        if (!u1IsLP4Family(p->dram_type))
        {
#ifndef LP3_DUAL_RANK_RX_K
            min_rx_value[0][1] = min_rx_value[0][0];
            min_RX_DQ_bit[0][1] = min_RX_DQ_bit[0][0];
#endif

#ifndef LP3_DUAL_RANK_TX_K
            min_tx_value[0][1] = min_tx_value[0][0];
            gFinalTXPerbitWin_min_max[0][1] = gFinalTXPerbitWin_min_max[0][0];
            min_TX_DQ_bit[0][1] = min_TX_DQ_bit[0][0];
#endif

            #if 0
            gFinalTXPerbitWin_min_margin[0][1] = gFinalTXPerbitWin_min_margin[0][0];
            gFinalTXPerbitWin_min_margin_bit[0][1] = gFinalTXPerbitWin_min_margin_bit[0][0];
            #endif
        }
    }


#if defined(DRAM_HQA)
        mcSHOW_DBG_MSG(("[Read Voltage]\n"));
        read_voltage_value = vGetVoltage(p, 0);
        HQA_LOG_Print_Prefix_String(p); mcSHOW_DBG_MSG(("Vcore_HQA = %d\n", read_voltage_value));

        if (u1IsLP4Family(p->dram_type)) {

            read_voltage_value = vGetVoltage(p, 1);
            HQA_LOG_Print_Prefix_String(p); mcSHOW_DBG_MSG(("Vdram_HQA = %d\n", read_voltage_value));
            read_voltage_value = vGetVoltage(p, 2);
            HQA_LOG_Print_Prefix_String(p); mcSHOW_DBG_MSG(("Vddq_HQA = %d\n", read_voltage_value));
            read_voltage_value = vGetVoltage(p, 3);
            HQA_LOG_Print_Prefix_String(p); mcSHOW_DBG_MSG(("Vdd1_HQA = %d\n", read_voltage_value));
            read_voltage_value = vGetVoltage(p, 4);
            HQA_LOG_Print_Prefix_String(p); mcSHOW_DBG_MSG(("Vmddr_HQA = %d\n", read_voltage_value));
        } else {

            read_voltage_value = vGetVoltage(p, 1);
            HQA_LOG_Print_Prefix_String(p); mcSHOW_DBG_MSG(("Vdram_HQA = %d\n", read_voltage_value));
        }
        mcSHOW_DBG_MSG(("\n"));
#endif

    if (p->dram_type == TYPE_LPDDR4)
    {
        print_imp_option[1] = TRUE;
    }
    else if (p->dram_type == TYPE_LPDDR4X)
    {
        print_imp_option[0] = TRUE;
        print_imp_option[1] = TRUE;
    }
    else
    {

      print_imp_option[0] = TRUE;
    }

#ifdef FOR_HQA_REPORT_USED
if (gHQALog_flag == 1)
{
    mcSHOW_DBG_MSG(("[Impedance Calibration]\n"));

    HQA_Log_Message_for_Report(p, p->channel, p->rank, HQA_REPORT_FORMAT6, "Impedance freq_region=0 ", "DRVP", 0, gDramcImpedanceResult[0][DRVP], NULL);
    HQA_Log_Message_for_Report(p, p->channel, p->rank, HQA_REPORT_FORMAT6, "Impedance freq_region=0 ", "DRVN", 0, gDramcImpedanceResult[0][DRVN], NULL);
    HQA_Log_Message_for_Report(p, p->channel, p->rank, HQA_REPORT_FORMAT6, "Impedance freq_region=0 ", "ODTN", 0, gDramcImpedanceResult[0][ODTN], NULL);

    HQA_Log_Message_for_Report(p, p->channel, p->rank, HQA_REPORT_FORMAT6, "Impedance freq_region=1 ", "DRVP", 0, gDramcImpedanceResult[1][DRVP], NULL);
    HQA_Log_Message_for_Report(p, p->channel, p->rank, HQA_REPORT_FORMAT6, "Impedance freq_region=1 ", "DRVN", 0, gDramcImpedanceResult[1][DRVN], NULL);
    HQA_Log_Message_for_Report(p, p->channel, p->rank, HQA_REPORT_FORMAT6, "Impedance freq_region=1 ", "ODTN", 0, gDramcImpedanceResult[1][ODTN], NULL);

    mcSHOW_DBG_MSG(("\n"));
}
#endif





    mcSHOW_DBG_MSG(("\n[Cmd Bus Training window]\n"));

    if (u1IsLP4Family(p->dram_type))
    {

        mcSHOW_DBG_MSG(("VrefCA\n"));
        for (u1ChannelIdx = CHANNEL_A; u1ChannelIdx < local_channel_num; u1ChannelIdx++)
        {
            for (u1RankIdx = RANK_0; u1RankIdx < p->support_rank_num; u1RankIdx++)
            {
                HQA_LOG_Print_Prefix_String(p); mcSHOW_DBG_MSG(("VrefCA Channel%d "
                                "Rank%d %d\n",
                            u1ChannelIdx,
                                u1RankIdx,
                                gFinalCBTVrefCA[u1ChannelIdx][u1RankIdx]));
            }
        }
    }

#if 0
    if (p->femmc_Ready == 1 )
    {
        mcSHOW_DBG_MSG(("\n[Cmd Bus Training window bypass calibration]\n"));
    }
    else
#endif
    {
        mcSHOW_DBG_MSG(("CA_Window\n"));
#ifdef FOR_HQA_REPORT_USED
if (gHQALog_flag == 1)
{
        for (u1ChannelIdx = CHANNEL_A; u1ChannelIdx < local_channel_num; u1ChannelIdx++)
        {
            for (u1RankIdx = RANK_0; u1RankIdx < p->support_rank_num; u1RankIdx++)
            {
                    HQA_Log_Message_for_Report(p, u1ChannelIdx, u1RankIdx, HQA_REPORT_FORMAT2, "CA", "_Window", 0, min_ca_value[u1ChannelIdx][u1RankIdx], NULL);
                    HQA_Log_Message_for_Report(p, u1ChannelIdx, u1RankIdx, HQA_REPORT_FORMAT2, "CA", "_Window_bit", 0, min_ca_bit[u1ChannelIdx][u1RankIdx], NULL);
            }
        }
        mcSHOW_DBG_MSG(("\n"));
}
else
#endif
{
        for (u1ChannelIdx = CHANNEL_A; u1ChannelIdx < local_channel_num; u1ChannelIdx++)
        {
            for (u1RankIdx = RANK_0; u1RankIdx < p->support_rank_num; u1RankIdx++)
            {
                HQA_LOG_Print_Prefix_String(p); mcSHOW_DBG_MSG(("CA_Window Channel%d "
                                "Rank%d %d (bit %d)\n",
                                u1ChannelIdx,
                                u1RankIdx,
                                min_ca_value[u1ChannelIdx][u1RankIdx], min_ca_bit[u1ChannelIdx][u1RankIdx]));
            }
        }
}

        mcSHOW_DBG_MSG(("CA Min Window(%%)\n"));
#ifdef FOR_HQA_REPORT_USED
if (gHQALog_flag == 1)
{
        for (u1ChannelIdx = CHANNEL_A; u1ChannelIdx < local_channel_num; u1ChannelIdx++)
        {
            for (u1RankIdx = RANK_0; u1RankIdx < p->support_rank_num; u1RankIdx++)
            {
                HQA_Log_Message_for_Report(p, u1ChannelIdx, u1RankIdx, HQA_REPORT_FORMAT2, "CA", "_Window(%)", 0, ((min_ca_value[u1ChannelIdx][u1RankIdx] * 100 + (u1IsLP4Family(p->dram_type) == 1? 63: 31)) / (u1IsLP4Family(p->dram_type) == 1? 64: 32)), NULL);
                HQA_Log_Message_for_Report(p, u1ChannelIdx, u1RankIdx, HQA_REPORT_FORMAT4, "CA", "_Window_PF", 0, 0, ((((min_ca_value[u1ChannelIdx][u1RankIdx] * 100 + (u1IsLP4Family(p->dram_type)==1?63:31)) / (u1IsLP4Family(p->dram_type)==1?64:32)) >= 30) ? "PASS" : "FAIL"));
            }
        }
        mcSHOW_DBG_MSG(("\n"));
}
else
#endif
{
        for (u1ChannelIdx = CHANNEL_A; u1ChannelIdx < local_channel_num; u1ChannelIdx++)
        {
            for (u1RankIdx = RANK_0; u1RankIdx < p->support_rank_num; u1RankIdx++)
            {
                HQA_LOG_Print_Prefix_String(p); mcSHOW_DBG_MSG(("CA_Window(%%) Channel%d "
                                "Rank%d %d%% (%s)\n",
                                u1ChannelIdx,
                                u1RankIdx,
                                        ((min_ca_value[u1ChannelIdx][u1RankIdx] * 100 + (u1IsLP4Family(p->dram_type) == 1? 63: 31)) / (u1IsLP4Family(p->dram_type) == 1? 64: 32)),
                                    ((((min_ca_value[u1ChannelIdx][u1RankIdx] * 100 + (u1IsLP4Family(p->dram_type) == 1? 63: 31)) / (u1IsLP4Family(p->dram_type) == 1? 64: 32)) >= 30)? "PASS" : "FAIL")));
            }
        }
}
    }
            mcSHOW_DBG_MSG(("\n"));




    mcSHOW_DBG_MSG(("\n[RX minimum per bit window]\n"));
    mcSHOW_DBG_MSG(("Delaycell measurement(/100ps)\n"));

#if defined(FOR_HQA_REPORT_USED) && !defined(RELEASE) && (VENDER_JV_LOG == 0)
    HQA_LOG_Print_Prefix_String(p); mcSHOW_DBG_MSG(("delaycell %d\n",
                    gHQALOG_RX_delay_cell_ps_075V));
#endif
#if defined(FOR_HQA_REPORT_USED)
    if (gHQALog_flag == 1)
    {
        if ((gHQALOG_RX_delay_cell_ps_075V < 245) || (gHQALOG_RX_delay_cell_ps_075V > 300))
        {
            gHQALog_SLT_BIN[vGet_Current_SRAMIdx(p)] = 2;
        }
    }
#endif


    if (u1IsLP4Family(p->dram_type))
    {
            mcSHOW_DBG_MSG(("VrefRX\n"));
            if (gRX_EYE_Scan_flag == 1)
            {
                for (u1ChannelIdx = CHANNEL_A; u1ChannelIdx < local_channel_num; u1ChannelIdx++)
                {
                    for (u1RankIdx = RANK_0; u1RankIdx < p->support_rank_num; u1RankIdx++)
                    {
                        for (u1ByteIdx = 0; u1ByteIdx < DQS_BYTE_NUMBER; u1ByteIdx++)
                        {
                            HQA_LOG_Print_Prefix_String(p); mcSHOW_DBG_MSG(("VrefRX Channel%d Rank%d Byte%d %d\n",
                                        u1ChannelIdx,
                                        u1RankIdx,
                                        u1ByteIdx,
                                        gFinalRXVrefDQ[u1ChannelIdx][u1RankIdx][u1ByteIdx]));
                        }
                    }
                }
            }
            else
            {
                for (u1ChannelIdx = CHANNEL_A; u1ChannelIdx < local_channel_num; u1ChannelIdx++)
                {
                    for (u1ByteIdx = 0; u1ByteIdx < DQS_BYTE_NUMBER; u1ByteIdx++)
                    {
                        HQA_LOG_Print_Prefix_String(p); mcSHOW_DBG_MSG(("VrefRX Channel%d Byte%d %d\n",
                                    u1ChannelIdx,
                                    u1ByteIdx,
                                    gFinalRXVrefDQ[u1ChannelIdx][RANK_0][u1ByteIdx]));
                    }
                }
            }
        }

#if 0//(SUPPORT_SAVE_TIME_FOR_CALIBRATION )
    if (p->femmc_Ready == 1 && ( p->Bypass_RXWINDOW))
    {
        mcSHOW_DBG_MSG(("\n[RX minimum per bit window bypass calibration]\n"));
    }
    else
#endif
    {

        mcSHOW_DBG_MSG(("RX_Window\n"));
#ifdef FOR_HQA_REPORT_USED
if (gHQALog_flag == 1)
{
        for (u1ChannelIdx = CHANNEL_A; u1ChannelIdx < local_channel_num; u1ChannelIdx++)
        {
            for (u1RankIdx = RANK_0; u1RankIdx < p->support_rank_num; u1RankIdx++)
            {
                    HQA_Log_Message_for_Report(p, u1ChannelIdx, u1RankIdx, HQA_REPORT_FORMAT2, "RX", "_Window", 0, min_rx_value[u1ChannelIdx][u1RankIdx], NULL);
                    HQA_Log_Message_for_Report(p, u1ChannelIdx, u1RankIdx, HQA_REPORT_FORMAT2, "RX", "_Window_bit", 0, min_RX_DQ_bit[u1ChannelIdx][u1RankIdx], NULL);
            }
        }
}
else
#endif
{
        for (u1ChannelIdx = CHANNEL_A; u1ChannelIdx < local_channel_num; u1ChannelIdx++)
        {
            for (u1RankIdx = RANK_0; u1RankIdx < p->support_rank_num; u1RankIdx++)
            {
                HQA_LOG_Print_Prefix_String(p); mcSHOW_DBG_MSG(("RX_Window Channel%d "
                                "Rank%d %d (bit %d)\n",
                            u1ChannelIdx,
                                u1RankIdx,
                                min_rx_value[u1ChannelIdx][u1RankIdx], min_RX_DQ_bit[u1ChannelIdx][u1RankIdx]));
            }
        }
}


        mcSHOW_DBG_MSG(("RX Window(%%)\n"));
#ifdef FOR_HQA_REPORT_USED
if (gHQALog_flag == 1)
{
        for (u1ChannelIdx = CHANNEL_A; u1ChannelIdx < local_channel_num; u1ChannelIdx++)
        {
            for (u1RankIdx = RANK_0; u1RankIdx < p->support_rank_num; u1RankIdx++)
            {
                HQA_Log_Message_for_Report(p, u1ChannelIdx, u1RankIdx, HQA_REPORT_FORMAT2, "RX", "_Window(%)", 0, ((min_rx_value[u1ChannelIdx][u1RankIdx] * gHQALOG_RX_delay_cell_ps_075V * DDRPhyGetRealFreq(p) * 2) + (1000000 - 1)) / 1000000, NULL);
                HQA_Log_Message_for_Report(p, u1ChannelIdx, u1RankIdx, HQA_REPORT_FORMAT4, "RX", "_Window_PF", 0, 0, (min_rx_value[u1ChannelIdx][u1RankIdx] * gHQALOG_RX_delay_cell_ps_075V * DDRPhyGetRealFreq(p) * 2) / 1000000 >= 40 ? "PASS" : "FAIL");

                if ((((min_rx_value[u1ChannelIdx][u1RankIdx] * gHQALOG_RX_delay_cell_ps_075V * DDRPhyGetRealFreq(p) * 2) + (1000000 - 1)) / 1000000) < 55)
                {
                    gHQALog_SLT_BIN[vGet_Current_SRAMIdx(p)] = 4;
                }
            }
        }
}
else
{
        for (u1ChannelIdx = CHANNEL_A; u1ChannelIdx < local_channel_num; u1ChannelIdx++)
        {
            for (u1RankIdx = RANK_0; u1RankIdx < p->support_rank_num; u1RankIdx++)
            {
                HQA_LOG_Print_Prefix_String(p); mcSHOW_DBG_MSG(("RX_Window(%%) Channel%d "
                                "Rank%d "
                                "%d/100ps (%d%%) (%s)\n",
                                u1ChannelIdx,
                                u1RankIdx,
                                min_rx_value[u1ChannelIdx][u1RankIdx] * gHQALOG_RX_delay_cell_ps_075V,
                                ((min_rx_value[u1ChannelIdx][u1RankIdx] * gHQALOG_RX_delay_cell_ps_075V * DDRPhyGetRealFreq(p) * 2) + (1000000 - 1)) / 1000000,
                                ((min_rx_value[u1ChannelIdx][u1RankIdx] * gHQALOG_RX_delay_cell_ps_075V * DDRPhyGetRealFreq(p) * 2) + (1000000 - 1)) / 1000000 >= 40? "PASS" : "FAIL"));
            }
        }
}
#endif
        mcSHOW_DBG_MSG(("\n"));
    }






    mcSHOW_DBG_MSG(("\n[TX minimum per bit window]\n"));
    if (u1IsLP4Family(p->dram_type))
    {
        if (Get_Vref_Calibration_OnOff(p) == VREF_CALI_ON)
        {
            mcSHOW_DBG_MSG(("VrefDQ Range : %d\n", (u1MR14Value[p->channel][p->rank][p->dram_fsp] >> 6) & 1));
            mcSHOW_DBG_MSG(("VrefDQ\n"));
            for (u1ChannelIdx = CHANNEL_A; u1ChannelIdx < local_channel_num; u1ChannelIdx++)
            {
                for (u1RankIdx = RANK_0; u1RankIdx < p->support_rank_num; u1RankIdx++)
                {
                    HQA_LOG_Print_Prefix_String(p); mcSHOW_DBG_MSG(("VrefDQ Channel%d "
                                    "Rank%d %d\n",
                                    u1ChannelIdx,
                                    u1RankIdx,
                                    gFinalTXVrefDQ[u1ChannelIdx][u1RankIdx]));
                }
            }
        }
        else
        {
            mcSHOW_DBG_MSG(("TX DQ Vref Scan : Disable\n"));
        }
    }
#if 0//(SUPPORT_SAVE_TIME_FOR_CALIBRATION )
    if (p->femmc_Ready == 1 && (p->Bypass_TXWINDOW))
    {
        mcSHOW_DBG_MSG(("\n[TX minimum per bit window bypass calibration]\n"));
    }
    else
#endif
    {

        mcSHOW_DBG_MSG(("TX_Window\n"));
#ifdef FOR_HQA_REPORT_USED
if (gHQALog_flag == 1)
{
        for (u1ChannelIdx = CHANNEL_A; u1ChannelIdx < local_channel_num; u1ChannelIdx++)
        {
            for (u1RankIdx = RANK_0; u1RankIdx < p->support_rank_num; u1RankIdx++)
            {
                    HQA_Log_Message_for_Report(p, u1ChannelIdx, u1RankIdx, HQA_REPORT_FORMAT2, "TX", "_Window", 0, gFinalTXPerbitWin_min_max[u1ChannelIdx][u1RankIdx], NULL);
                    HQA_Log_Message_for_Report(p, u1ChannelIdx, u1RankIdx, HQA_REPORT_FORMAT2, "TX", "_Window_bit", 0, min_TX_DQ_bit[u1ChannelIdx][u1RankIdx], NULL);
            }
        }
}
else
#endif
{
        for (u1ChannelIdx = CHANNEL_A; u1ChannelIdx < local_channel_num; u1ChannelIdx++)
        {
            for (u1RankIdx = RANK_0; u1RankIdx < p->support_rank_num; u1RankIdx++)
            {
                HQA_LOG_Print_Prefix_String(p); mcSHOW_DBG_MSG(("TX_Window Channel%d "
                                "Rank%d %d (bit %d)\n",
                                u1ChannelIdx,
                                u1RankIdx,
                                gFinalTXPerbitWin_min_max[u1ChannelIdx][u1RankIdx], min_TX_DQ_bit[u1ChannelIdx][u1RankIdx]));
            }
        }
}
#if 0//(TX_PER_BIT_DELAY_CELL==0)
        mcSHOW_DBG_MSG(("min DQ margin\n"));
        for (u1ChannelIdx = CHANNEL_A; u1ChannelIdx < local_channel_num; u1ChannelIdx++)
        {
            for (u1RankIdx = RANK_0; u1RankIdx < 2; u1RankIdx++)
            {
                HQA_LOG_Print_Prefix_String(p); mcSHOW_DBG_MSG(("min_DQ_margin Channel%d "
                                "Rank%d %d (bit %d)\n",
                                u1ChannelIdx,
                                u1RankIdx,
                                gFinalTXPerbitWin_min_margin[u1ChannelIdx][u1RankIdx], gFinalTXPerbitWin_min_margin_bit[u1ChannelIdx][u1RankIdx]));
            }
        }
#endif


        mcSHOW_DBG_MSG(("TX Min Window(%%)\n"));
#ifdef FOR_HQA_REPORT_USED
if (gHQALog_flag == 1)
{
        for (u1ChannelIdx = CHANNEL_A; u1ChannelIdx < local_channel_num; u1ChannelIdx++)
        {
            for (u1RankIdx = RANK_0; u1RankIdx < p->support_rank_num; u1RankIdx++)
            {

                HQA_Log_Message_for_Report(p, u1ChannelIdx, u1RankIdx, HQA_REPORT_FORMAT2, "TX", "_Window(%)", 0, (min_tx_value[u1ChannelIdx][u1RankIdx] * 100 + (vGet_DDR_Loop_Mode(p) == DDR800_CLOSE_LOOP? 63: 31)) / (vGet_DDR_Loop_Mode(p) == DDR800_CLOSE_LOOP? 64: 32), NULL);
                HQA_Log_Message_for_Report(p, u1ChannelIdx, u1RankIdx, HQA_REPORT_FORMAT4, "TX", "_Window_PF", 0, 0, (min_tx_value[u1ChannelIdx][u1RankIdx] * 100 + (vGet_DDR_Loop_Mode(p) == DDR800_CLOSE_LOOP ? 63 : 31)) / (vGet_DDR_Loop_Mode(p) == DDR800_CLOSE_LOOP ? 64 : 32) >= 45 ? "PASS" : "FAIL");

                if (((min_tx_value[u1ChannelIdx][u1RankIdx] * 100 + (vGet_DDR_Loop_Mode(p) == DDR800_CLOSE_LOOP? 63: 31)) / (vGet_DDR_Loop_Mode(p) == DDR800_CLOSE_LOOP? 64: 32)) < 55)
                {
                    gHQALog_SLT_BIN[vGet_Current_SRAMIdx(p)] = 4;
                }
            }
        }
}
else
#endif
{
        for (u1ChannelIdx = CHANNEL_A; u1ChannelIdx < local_channel_num; u1ChannelIdx++)
        {
            for (u1RankIdx = RANK_0; u1RankIdx < p->support_rank_num; u1RankIdx++)
            {


                HQA_LOG_Print_Prefix_String(p); mcSHOW_DBG_MSG(("TX_Window(%%) Channel%d "
                                "Rank%d %d%% (%s)\n",
                                u1ChannelIdx,
                                u1RankIdx,
                               (min_tx_value[u1ChannelIdx][u1RankIdx] * 100 + (vGet_DDR_Loop_Mode(p) == DDR800_CLOSE_LOOP? 63: 31)) / (vGet_DDR_Loop_Mode(p) == DDR800_CLOSE_LOOP? 64: 32),
                               (min_tx_value[u1ChannelIdx][u1RankIdx] * 100 + (vGet_DDR_Loop_Mode(p) == DDR800_CLOSE_LOOP? 63: 31)) / (vGet_DDR_Loop_Mode(p) == DDR800_CLOSE_LOOP? 64: 32) >= 45? "PASS" : "FAIL"));
            }
        }
}

        mcSHOW_DBG_MSG(("\n"));
    }




#if !defined(RELEASE) && (VENDER_JV_LOG == 0)
    if (u1IsLP4Family(p->dram_type) && (Get_Duty_Calibration_Mode(p) != DUTY_DEFAULT))
    {
        mcSHOW_DBG_MSG(("[duty Calibration]\n"));
        mcSHOW_DBG_MSG(("CLK Duty Final Delay Cell\n"));
        for (u1ChannelIdx = CHANNEL_A; u1ChannelIdx < local_channel_num; u1ChannelIdx++)
        {
                     HQA_LOG_Print_Prefix_String(p); mcSHOW_DBG_MSG(("DUTY CLK_Final_Delay Channel%d %d\n", u1ChannelIdx, gFinalClkDuty[u1ChannelIdx]));
        }


        mcSHOW_DBG_MSG(("CLK Duty MAX\n"));
        for (u1ChannelIdx = CHANNEL_A; u1ChannelIdx < local_channel_num; u1ChannelIdx++)
        {
#ifdef FOR_HQA_REPORT_USED
                if (gHQALog_flag == 1)
                {
                    HQA_Log_Message_for_Report(p, u1ChannelIdx, 0, HQA_REPORT_FORMAT3, "DUTY CLK_", "MAX", 0, gFinalClkDutyMinMax[u1ChannelIdx][1], NULL);
                }
                else
#endif
                {
                    HQA_LOG_Print_Prefix_String(p); mcSHOW_DBG_MSG(("DUTY CLK_MAX Channel%d %d%%(X100)\n", u1ChannelIdx, gFinalClkDutyMinMax[u1ChannelIdx][1]));
                }
        }


        mcSHOW_DBG_MSG(("CLK Duty MIN\n"));
        for (u1ChannelIdx = CHANNEL_A; u1ChannelIdx < local_channel_num; u1ChannelIdx++)
        {
#ifdef FOR_HQA_REPORT_USED
            if (gHQALog_flag == 1)
            {
                HQA_Log_Message_for_Report(p, u1ChannelIdx, 0, HQA_REPORT_FORMAT3, "DUTY CLK_", "MIN", 0, gFinalClkDutyMinMax[u1ChannelIdx][0], NULL);
                HQA_Log_Message_for_Report(p, u1ChannelIdx, 0, HQA_REPORT_FORMAT3, "DUTY CLK_", "MAX-MIN", 0, gFinalClkDutyMinMax[u1ChannelIdx][1] - gFinalClkDutyMinMax[u1ChannelIdx][0], NULL);

                if ((gFinalClkDutyMinMax[u1ChannelIdx][1] - gFinalClkDutyMinMax[u1ChannelIdx][0]) > 750)
                {
                    gHQALog_SLT_BIN[vGet_Current_SRAMIdx(p)] = 3;
                }
            }
            else
#endif
            {
                HQA_LOG_Print_Prefix_String(p); mcSHOW_DBG_MSG(("DUTY CLK_MIN Channel%d %d%%(X100)\n", u1ChannelIdx, gFinalClkDutyMinMax[u1ChannelIdx][0]));
            }
        }

        mcSHOW_DBG_MSG(("\n"));
    }





    if (u1IsLP4Family(p->dram_type) && (Get_Duty_Calibration_Mode(p) != DUTY_DEFAULT))
    {
        mcSHOW_DBG_MSG(("DQS Duty Final Delay Cell\n"));
        for (u1ChannelIdx = CHANNEL_A; u1ChannelIdx < local_channel_num; u1ChannelIdx++)
        {
            HQA_LOG_Print_Prefix_String(p); mcSHOW_DBG_MSG(("DUTY DQS_Final_Delay Channel%d DQS0 %d\n", u1ChannelIdx, gFinalDQSDuty[u1ChannelIdx][0]));
            HQA_LOG_Print_Prefix_String(p); mcSHOW_DBG_MSG(("DUTY DQS_Final_Delay Channel%d DQS1 %d\n", u1ChannelIdx, gFinalDQSDuty[u1ChannelIdx][1]));
        }

        mcSHOW_DBG_MSG(("DQS Duty MAX\n"));
        for (u1ChannelIdx = CHANNEL_A; u1ChannelIdx < local_channel_num; u1ChannelIdx++)
        {
#ifdef FOR_HQA_REPORT_USED
                if (gHQALog_flag == 1)
                {
                    HQA_Log_Message_for_Report(p, u1ChannelIdx, 0, HQA_REPORT_FORMAT0, "", "DUTY DQS_MAX", 0, gFinalDQSDutyMinMax[u1ChannelIdx][0][1], NULL);
                    HQA_Log_Message_for_Report(p, u1ChannelIdx, 0, HQA_REPORT_FORMAT0, "", "DUTY DQS_MAX", 1, gFinalDQSDutyMinMax[u1ChannelIdx][1][1], NULL);
                }
                else
#endif
                {
                    HQA_LOG_Print_Prefix_String(p); mcSHOW_DBG_MSG(("DUTY DQS_MAX Channel%d DQS0 %d%%(X100)\n", u1ChannelIdx, gFinalDQSDutyMinMax[u1ChannelIdx][0][1]));
                    HQA_LOG_Print_Prefix_String(p); mcSHOW_DBG_MSG(("DUTY DQS_MAX Channel%d DQS1 %d%%(X100)\n", u1ChannelIdx, gFinalDQSDutyMinMax[u1ChannelIdx][1][1]));
                }
        }


        mcSHOW_DBG_MSG(("DQS Duty MIN\n"));
        for (u1ChannelIdx = CHANNEL_A; u1ChannelIdx < local_channel_num; u1ChannelIdx++)
        {
#ifdef FOR_HQA_REPORT_USED
            if (gHQALog_flag == 1)
            {
                HQA_Log_Message_for_Report(p, u1ChannelIdx, 0, HQA_REPORT_FORMAT0, "DUTY DQS_", "MIN", 0, gFinalDQSDutyMinMax[u1ChannelIdx][0][0], NULL);
                HQA_Log_Message_for_Report(p, u1ChannelIdx, 0, HQA_REPORT_FORMAT0, "DUTY DQS_", "MIN", 1, gFinalDQSDutyMinMax[u1ChannelIdx][1][0], NULL);
                HQA_Log_Message_for_Report(p, u1ChannelIdx, 0, HQA_REPORT_FORMAT0, "DUTY DQS_", "MAX-MIN", 0, gFinalDQSDutyMinMax[u1ChannelIdx][0][1] - gFinalDQSDutyMinMax[u1ChannelIdx][0][0], NULL);
                HQA_Log_Message_for_Report(p, u1ChannelIdx, 0, HQA_REPORT_FORMAT0, "DUTY DQS_", "MAX-MIN", 1, gFinalDQSDutyMinMax[u1ChannelIdx][1][1] - gFinalDQSDutyMinMax[u1ChannelIdx][1][0], NULL);

                if (((gFinalDQSDutyMinMax[u1ChannelIdx][0][1] - gFinalDQSDutyMinMax[u1ChannelIdx][0][0]) > 750) || ((gFinalDQSDutyMinMax[u1ChannelIdx][1][1] - gFinalDQSDutyMinMax[u1ChannelIdx][1][0]) > 750))
                {
                    gHQALog_SLT_BIN[vGet_Current_SRAMIdx(p)] = 3;
                }
            }
            else
#endif
            {
                HQA_LOG_Print_Prefix_String(p); mcSHOW_DBG_MSG(("DUTY DQS_MIN Channel%d DQS0 %d%%(X100)\n", u1ChannelIdx, gFinalDQSDutyMinMax[u1ChannelIdx][0][0]));
                HQA_LOG_Print_Prefix_String(p); mcSHOW_DBG_MSG(("DUTY DQS_MIN Channel%d DQS1 %d%%(X100)\n", u1ChannelIdx, gFinalDQSDutyMinMax[u1ChannelIdx][1][0]));
            }
        }

        mcSHOW_DBG_MSG(("\n"));
    }
#endif


    #if defined(ENABLE_MIOCK_JMETER) && !defined(RELEASE)
            mcSHOW_DBG_MSG(("\n[DramcMiockJmeter]\n"
                    "\t\t1 delay cell\t\t%d/100 ps\n", p->u2DelayCellTimex100));
    #endif

    mcSHOW_DBG_MSG(("\n\n\n"));



#if VENDER_JV_LOG
    mcSHOW_JV_LOG_MSG(("\n\n\n[Summary] information for measurement\n"));
    //mcSHOW_JV_LOG_MSG(("\tDram Data rate = ")); HQA_LOG_Print_Freq_String(p); mcSHOW_DBG_MSG(("\n"));
    vPrintCalibrationBasicInfo_ForJV(p);

    if (u1IsLP4Family(p->dram_type))
     {
         mcSHOW_JV_LOG_MSG(("[Cmd Bus Training window]\n"));

         //mcSHOW_JV_LOG_MSG(("VrefCA Range : %d\n", gCBT_VREF_RANGE_SEL));
#if CHANNEL_NUM == 4
        mcSHOW_JV_LOG_MSG(("CHA_VrefCA_Rank0   CHB_VrefCA_Rank0   CHC_VrefCA_Rank0    CHD_VrefCA_Rank0\n"));
        mcSHOW_JV_LOG_MSG(("%d                 %d                 %d                  %d\n", gFinalCBTVrefCA[0][0], gFinalCBTVrefCA[1][0], gFinalCBTVrefCA[2][0], gFinalCBTVrefCA[3][0]));
#else
         mcSHOW_JV_LOG_MSG(("CHA_VrefCA_Rank0   CHB_VrefCA_Rank0\n"));
         mcSHOW_JV_LOG_MSG(("%d                 %d\n", gFinalCBTVrefCA[0][0], gFinalCBTVrefCA[1][0]));
#endif
         mcSHOW_JV_LOG_MSG(("CHA_CA_window_Rank0   CHB_CA_winow_Rank0\n"));
         mcSHOW_JV_LOG_MSG(("%d%%(bit %d)              %d%%(bit %d) \n\n", (min_ca_value[0][0] * 100 + 63) / 64, min_ca_bit[0][0],
                                                                     (min_ca_value[1][0] * 100 + 63) / 64, min_ca_bit[1][0]));
     }
     else
     {
         mcSHOW_JV_LOG_MSG(("[CA Training window]\n"));
         mcSHOW_JV_LOG_MSG(("CHA_CA_win_Rank0\n"));
         mcSHOW_JV_LOG_MSG(("%d%%(bit %d)\n\n", (min_ca_value[0][0] * 100 + 63) / 64, min_ca_bit[0][0]));
     }

     mcSHOW_JV_LOG_MSG(("[RX minimum per bit window]\n"));
     if (Get_Vref_Calibration_OnOff(p) == VREF_CALI_ON)
     {
#if CHANNEL_NUM == 4
        mcSHOW_JV_LOG_MSG(("CHA_VrefDQ    CHB_VrefDQ      CHC_VrefDQ      CHD_VrefDQ\n"));
        mcSHOW_JV_LOG_MSG(("B0 %d            %d              %d              %d \n", gFinalRXVrefDQ[CHANNEL_A][RANK_0][0], gFinalRXVrefDQ[CHANNEL_B][RANK_0][0], gFinalRXVrefDQ[CHANNEL_C][RANK_0][0], gFinalRXVrefDQ[CHANNEL_D][RANK_0][0]));
        mcSHOW_JV_LOG_MSG(("B1 %d            %d              %d              %d \n", gFinalRXVrefDQ[CHANNEL_A][RANK_0][1], gFinalRXVrefDQ[CHANNEL_B][RANK_0][1], gFinalRXVrefDQ[CHANNEL_C][RANK_0][1], gFinalRXVrefDQ[CHANNEL_D][RANK_0][1]));
#else
         mcSHOW_JV_LOG_MSG(("CHA_VrefDQ   CHB_VrefDQ\n"));
         mcSHOW_JV_LOG_MSG(("B0 %d           %d \n", gFinalRXVrefDQ[CHANNEL_A][RANK_0][0], gFinalRXVrefDQ[CHANNEL_B][RANK_0][0]));
         mcSHOW_JV_LOG_MSG(("B1 %d           %d \n", gFinalRXVrefDQ[CHANNEL_A][RANK_0][1], gFinalRXVrefDQ[CHANNEL_B][RANK_0][1]));
#endif
     }
     else
     {
         mcSHOW_JV_LOG_MSG(("RX DQ Vref Scan : Disable\n"));

     }

    if (u1IsLP4Family(p->dram_type))
    {
#if CHANNEL_NUM == 4
    mcSHOW_JV_LOG_MSG(("CHA_Rank0           CHA_Rank1           CHB_Rank0           CHB_Rank1         CHC_Rank0       CHC_Rank1       CHD_Rank0       CHD_Rank1\n"));
    mcSHOW_JV_LOG_MSG(("%d%%(bit %d)         %d%%(bit %d)         %d%%(bit %d)         %d%%(bit %d)         %d%%(bit %d)         %d%%(bit %d)         %d%%(bit %d)         %d%%(bit %d)\n\n",
                                    ((min_rx_value[0][0] * gHQALOG_RX_delay_cell_ps_075V * DDRPhyGetRealFreq(p) * 2) + (1000000 - 1)) / 1000000, min_RX_DQ_bit[0][0],
                                    ((min_rx_value[0][1] * gHQALOG_RX_delay_cell_ps_075V * DDRPhyGetRealFreq(p) * 2) + (1000000 - 1)) / 1000000, min_RX_DQ_bit[0][1],
                                    ((min_rx_value[1][0] * gHQALOG_RX_delay_cell_ps_075V * DDRPhyGetRealFreq(p) * 2) + (1000000 - 1)) / 1000000, min_RX_DQ_bit[1][0],
                                    ((min_rx_value[1][1] * gHQALOG_RX_delay_cell_ps_075V * DDRPhyGetRealFreq(p) * 2) + (1000000 - 1)) / 1000000, min_RX_DQ_bit[1][1],
                                    ((min_rx_value[2][0] * gHQALOG_RX_delay_cell_ps_075V * DDRPhyGetRealFreq(p) * 2) + (1000000 - 1)) / 1000000, min_RX_DQ_bit[2][0],
                                    ((min_rx_value[2][1] * gHQALOG_RX_delay_cell_ps_075V * DDRPhyGetRealFreq(p) * 2) + (1000000 - 1)) / 1000000, min_RX_DQ_bit[2][1],
                                    ((min_rx_value[3][0] * gHQALOG_RX_delay_cell_ps_075V * DDRPhyGetRealFreq(p) * 2) + (1000000 - 1)) / 1000000, min_RX_DQ_bit[3][0],
                                    ((min_rx_value[3][1] * gHQALOG_RX_delay_cell_ps_075V * DDRPhyGetRealFreq(p) * 2) + (1000000 - 1)) / 1000000, min_RX_DQ_bit[3][1]));
#else
     mcSHOW_JV_LOG_MSG(("CHA_Rank0           CHA_Rank1           CHB_Rank0           CHB_Rank1\n"));
     mcSHOW_JV_LOG_MSG(("%d%%(bit %d)         %d%%(bit %d)         %d%%(bit %d)         %d%%(bit %d)\n\n",
                                     ((min_rx_value[0][0] * gHQALOG_RX_delay_cell_ps_075V * DDRPhyGetRealFreq(p) * 2) + (1000000 - 1)) / 1000000, min_RX_DQ_bit[0][0],
                                     ((min_rx_value[0][1] * gHQALOG_RX_delay_cell_ps_075V * DDRPhyGetRealFreq(p) * 2) + (1000000 - 1)) / 1000000, min_RX_DQ_bit[0][1],
                                     ((min_rx_value[1][0] * gHQALOG_RX_delay_cell_ps_075V * DDRPhyGetRealFreq(p) * 2) + (1000000 - 1)) / 1000000, min_RX_DQ_bit[1][0],
                                     ((min_rx_value[1][1] * gHQALOG_RX_delay_cell_ps_075V * DDRPhyGetRealFreq(p) * 2) + (1000000 - 1)) / 1000000, min_RX_DQ_bit[1][1]));
#endif
    }
#if ENABLE_LP3_SW
    else
    {
        mcSHOW_JV_LOG_MSG(("CHA_Rank0           CHA_Rank1\n"));
        mcSHOW_JV_LOG_MSG(("%d%%(bit %d)         %d%%(bit %d)\n\n",
                                        ((min_rx_value[0][0] * u2gdelay_cell_ps_all[SRAMIdx] * DDRPhyGetRealFreq(p) * 2) + (1000000 - 1)) / 1000000, min_RX_DQ_bit[0][0],
                                        ((min_rx_value[0][1] * u2gdelay_cell_ps_all[SRAMIdx] * DDRPhyGetRealFreq(p) * 2) + (1000000 - 1)) / 1000000, min_RX_DQ_bit[0][1]));
    }
#endif

     mcSHOW_JV_LOG_MSG(("[TX minimum per bit window]\n"));
     if (Get_Vref_Calibration_OnOff(p) == VREF_CALI_ON)
     {
         mcSHOW_JV_LOG_MSG(("VrefDQ Range : %d\n", (u1MR14Value[p->channel][p->rank][p->dram_fsp] >> 6) & 1));
#if CHANNEL_NUM == 4
        mcSHOW_JV_LOG_MSG(("CHA_VrefDQ_Rank0   CHA_VrefDQ_Rank1    CHB_VrefDQ_Rank0    CHB_VrefDQ_Rank1   CHC_VrefDQ_Rank0    CHC_VrefDQ_Rank1    CHD_VrefDQ_Rank0    CHD_VrefDQ_Rank1\n"));
        mcSHOW_JV_LOG_MSG(("%d                 %d                  %d                  %d                 %d                  %d                  %d                  %d\n"
 , gFinalTXVrefDQ[0][0], gFinalTXVrefDQ[0][1], gFinalTXVrefDQ[1][0], gFinalTXVrefDQ[1][1]
 , gFinalTXVrefDQ[2][0], gFinalTXVrefDQ[2][1], gFinalTXVrefDQ[3][0], gFinalTXVrefDQ[3][1]
                    ));
#else
         mcSHOW_JV_LOG_MSG(("CHA_VrefDQ_Rank0   CHA_VrefDQ_Rank1    CHB_VrefDQ_Rank0    CHB_VrefDQ_Rank1\n"));
         mcSHOW_JV_LOG_MSG(("%d                  %d                   %d                   %d\n", gFinalTXVrefDQ[0][0], gFinalTXVrefDQ[0][1], gFinalTXVrefDQ[1][0], gFinalTXVrefDQ[1][1]));
#endif
     }
     else
     {
         mcSHOW_JV_LOG_MSG(("TX DQ Vref Scan : Disable\n"));
     }

    if (u1IsLP4Family(p->dram_type))
    {
#if CHANNEL_NUM == 4
    mcSHOW_JV_LOG_MSG(("CHA_Rank0         CHA_Rank1           CHB_Rank0           CHB_Rank1       CHC_Rank0       CHC_Rank1       CHD_Rank0   CHD_Rank1\n"));
    mcSHOW_JV_LOG_MSG(("%d%%               %d%%                 %d%%                 %d%%                 %d%%                 %d%%                 %d%%                 %d%%\n",
                                        (min_tx_value[0][0] * 100 + 31) / 32,
                                        (min_tx_value[0][1] * 100 + 31) / 32,
                                        (min_tx_value[1][0] * 100 + 31) / 32,
                                        (min_tx_value[1][1] * 100 + 31) / 32,
                                        (min_tx_value[2][0] * 100 + 31) / 32,
                                        (min_tx_value[2][1] * 100 + 31) / 32,
                                        (min_tx_value[3][0] * 100 + 31) / 32,
                                        (min_tx_value[3][1] * 100 + 31) / 32
                                        ));
#else
     mcSHOW_JV_LOG_MSG(("CHA_Rank0           CHA_Rank1           CHB_Rank0           CHB_Rank1\n"));
     mcSHOW_JV_LOG_MSG(("%d%%                %d%%                %d%%                %d%%\n",
                                         (min_tx_value[0][0] * 100 + 31) / 32,
                                         (min_tx_value[0][1] * 100 + 31) / 32,
                                         (min_tx_value[1][0] * 100 + 31) / 32,
                                         (min_tx_value[1][1] * 100 + 31) / 32));
#endif
    }
    else
    {
     mcSHOW_JV_LOG_MSG(("CHA_Rank0           CHA_Rank1\n"));
     mcSHOW_JV_LOG_MSG(("%d%%                %d%%\n",
                                         (min_tx_value[0][0] * 100 + 31) / 32,
                                         (min_tx_value[0][1] * 100 + 31) / 32));
    }
#endif



    HQA_measure_message_reset_all_data(p);
}
#ifdef RELEASE
#undef mcSHOW_DBG_MSG
#define mcSHOW_DBG_MSG(_x_)
#endif
#endif


#ifdef FOR_HQA_REPORT_USED
static void print_EyeScanVcent_for_HQA_report_used(DRAMC_CTX_T *p, U8 print_type, U8 u1ChannelIdx, U8 u1RankIdx, U8 u1ByteIdx, U8 *EyeScanVcent, U8 EyeScanVcentUpperBound, U8 EyeScanVcentUpperBound_bit, U8 EyeScanVcentLowerBound, U8 EyeScanVcentLowerBound_bit)
{
    U8 uiCA, u1BitIdx;
    U16 *pVref_Voltage_Table[VREF_VOLTAGE_TABLE_NUM_LP5] = {0};
    U8  VrefRange;
    U32 vddq = 0;
    U8 local_channel_num = 2;
    U8 SRAMIdx;
    U8 u1CBTEyeScanEnable, u1RXEyeScanEnable, u1TXEyeScanEnable;
    U16 Final_Vref_Vcent, VdlVWHigh_Upper_Vcent, VdlVWHigh_Lower_Vcent;
    U32 Vcent_UpperBound_Window_percent = 0, Vcent_LowerBound_Window_percent = 0;
    U32 Perbit_Window_percent, Perbit_BestWindow_percent, Perbit_Window_Upperbond_percent, Perbit_Window_Lowerbond_percent, Perbit_Eye_Height, Perbit_Eye_Area;
    U8 u1BitIndex_Begin = 0, u1BitIndex_End = p->data_width;

    if (gHQALog_flag == 0)
    {
    mcSHOW_DBG_MSG(("\n"));
        return;
    }

    if (u1IsLP4Family(p->dram_type))
    {
        local_channel_num = p->support_channel_num;
    }
    else
    {

        local_channel_num = 1;
    }

    u1CBTEyeScanEnable = GetEyeScanEnable(p, EYESCAN_TYPE_CBT);
    u1RXEyeScanEnable = GetEyeScanEnable(p, EYESCAN_TYPE_RX);
    u1TXEyeScanEnable = GetEyeScanEnable(p, EYESCAN_TYPE_TX);

    SRAMIdx = vGet_Current_SRAMIdx(p);

    if (print_type == EYESCAN_TYPE_RX)
    {
        if (p->odt_onoff==TRUE)
        {
            if (p->dram_type==TYPE_LPDDR5)
                pVref_Voltage_Table[VREF_RANGE_0] = (U16 *)gRXVref_Voltage_Table_0P5V_T;
            else
                pVref_Voltage_Table[VREF_RANGE_0] = (U16 *)gRXVref_Voltage_Table_0P6V_T;
        }
        else
        {
            if (p->dram_type==TYPE_LPDDR5)
                pVref_Voltage_Table[VREF_RANGE_0] = (U16 *)gRXVref_Voltage_Table_0P5V_UT;
            else
                pVref_Voltage_Table[VREF_RANGE_0] = (U16 *)gRXVref_Voltage_Table_0P6V_UT;
        }

        u1BitIndex_Begin = u1ByteIdx * DQS_BIT_NUMBER;
        u1BitIndex_End = u1ByteIdx * DQS_BIT_NUMBER + DQS_BIT_NUMBER;
    }
    else
    {
        if (u1CBTEyeScanEnable || u1TXEyeScanEnable)
        {
            vddq = vGetVoltage(p, 2) / 1000;

            if (p->dram_type == TYPE_LPDDR4)
            {
                pVref_Voltage_Table[VREF_RANGE_0] = (U16 *)gVref_Voltage_Table_LP4[VREF_RANGE_0];
                pVref_Voltage_Table[VREF_RANGE_1] = (U16 *)gVref_Voltage_Table_LP4[VREF_RANGE_1];
            }
            if (p->dram_type == TYPE_LPDDR4X)
            {
                pVref_Voltage_Table[VREF_RANGE_0] = (U16 *)gVref_Voltage_Table_LP4X[VREF_RANGE_0];
                pVref_Voltage_Table[VREF_RANGE_1] = (U16 *)gVref_Voltage_Table_LP4X[VREF_RANGE_1];
            }
        }
    }

    if (print_type == EYESCAN_TYPE_CBT)
    {
        if (u1CBTEyeScanEnable)
        {
            VrefRange = (u1MR12Value[p->channel][p->rank][p->dram_fsp] >> 6) & 1;

            Vcent_UpperBound_Window_percent = (EyeScanVcentUpperBound * 100 + (u1IsLP4Family(p->dram_type) == 1? 63: 31)) / (u1IsLP4Family(p->dram_type) == 1? 64: 32);
            Vcent_LowerBound_Window_percent = (EyeScanVcentLowerBound * 100 + (u1IsLP4Family(p->dram_type) == 1? 63: 31)) / (u1IsLP4Family(p->dram_type) == 1? 64: 32);
        }
    }

    if (print_type == EYESCAN_TYPE_RX)
    {
        if (u1RXEyeScanEnable)
        {
            Final_Vref_Vcent = pVref_Voltage_Table[0][EyeScanVcent[1]];
            VdlVWHigh_Upper_Vcent = pVref_Voltage_Table[0][EyeScanVcent[3]];
            VdlVWHigh_Lower_Vcent = pVref_Voltage_Table[0][EyeScanVcent[5]];

            Vcent_UpperBound_Window_percent = ((EyeScanVcentUpperBound * gHQALOG_RX_delay_cell_ps_075V * DDRPhyGetRealFreq(p) * 2) + (1000000 - 1)) / 1000000;
            Vcent_LowerBound_Window_percent = ((EyeScanVcentLowerBound * gHQALOG_RX_delay_cell_ps_075V * DDRPhyGetRealFreq(p) * 2) + (1000000 - 1)) / 1000000;
        }
    }

    if (print_type == EYESCAN_TYPE_TX)
    {
        if (u1TXEyeScanEnable)
        {
            VrefRange = (u1MR14Value[p->channel][p->rank][p->dram_fsp] >> 6) & 1;

            Final_Vref_Vcent = pVref_Voltage_Table[EyeScanVcent[0]][EyeScanVcent[1]] * vddq / 100;
            VdlVWHigh_Upper_Vcent = pVref_Voltage_Table[EyeScanVcent[2]][EyeScanVcent[3]] * vddq / 100;
            VdlVWHigh_Lower_Vcent = pVref_Voltage_Table[EyeScanVcent[4]][EyeScanVcent[5]] * vddq / 100;

            Vcent_UpperBound_Window_percent = (EyeScanVcentUpperBound * 100 + (u1IsLP4Div4DDR800(p) == TRUE? 63: 31)) / (u1IsLP4Div4DDR800(p) == TRUE? 64: 32);
            Vcent_LowerBound_Window_percent = (EyeScanVcentLowerBound * 100 + (u1IsLP4Div4DDR800(p) == TRUE? 63: 31)) / (u1IsLP4Div4DDR800(p) == TRUE? 64: 32);
        }
   }

    mcSHOW_DBG_MSG(("\n\n\n[HQA] information for measurement, "));
    mcSHOW_DBG_MSG(("\tDram Data rate = ")); HQA_LOG_Print_Freq_String(p); mcSHOW_DBG_MSG(("\n"));

#if ENABLE_EYESCAN_GRAPH
    mcSHOW_DBG_MSG(("%s Eye Scan Vcent Voltage\n", print_EYESCAN_LOG_type(print_type)));
    if (print_type == EYESCAN_TYPE_RX)
    {
            HQA_Log_Message_for_Report(p, p->channel, p->rank, HQA_REPORT_FORMAT2_1, print_EYESCAN_LOG_type(print_type), "_Final_Vref Vcent", u1ByteIdx, pVref_Voltage_Table[0][EyeScanVcent[1]], NULL);
            HQA_Log_Message_for_Report(p, p->channel, p->rank, HQA_REPORT_FORMAT2_1, print_EYESCAN_LOG_type(print_type), "_VdlVWHigh_Upper Vcent", u1ByteIdx, pVref_Voltage_Table[0][EyeScanVcent[3]], NULL);
            HQA_Log_Message_for_Report(p, p->channel, p->rank, HQA_REPORT_FORMAT2_1, print_EYESCAN_LOG_type(print_type), "_VdlVWHigh_Lower Vcent", u1ByteIdx, pVref_Voltage_Table[0][EyeScanVcent[5]], NULL);
    }
    else
    {
            HQA_Log_Message_for_Report(p, p->channel, p->rank, HQA_REPORT_FORMAT2, print_EYESCAN_LOG_type(print_type), "_Final_Vref Vcent", 0, pVref_Voltage_Table[EyeScanVcent[0]][EyeScanVcent[1]] * vddq / 100, NULL);
            HQA_Log_Message_for_Report(p, p->channel, p->rank, HQA_REPORT_FORMAT2, print_EYESCAN_LOG_type(print_type), "_VdlVWHigh_Upper Vcent", 0, pVref_Voltage_Table[EyeScanVcent[2]][EyeScanVcent[3]] * vddq / 100, NULL);
            HQA_Log_Message_for_Report(p, p->channel, p->rank, HQA_REPORT_FORMAT2, print_EYESCAN_LOG_type(print_type), "_VdlVWHigh_Lower Vcent", 0, pVref_Voltage_Table[EyeScanVcent[4]][EyeScanVcent[5]] * vddq / 100, NULL);
    }

    mcSHOW_DBG_MSG(("\n"));
#endif

    mcSHOW_DBG_MSG(("%s Eye Scan Vcent_UpperBound window\n", print_EYESCAN_LOG_type(print_type)));
            HQA_Log_Message_for_Report(p, p->channel, p->rank, HQA_REPORT_FORMAT2, print_EYESCAN_LOG_type(print_type), "_Vcent_UpperBound_Window", 0, EyeScanVcentUpperBound, NULL);
    mcSHOW_DBG_MSG(("%s Eye Scan Vcent_UpperBound_Window worse bit\n", print_EYESCAN_LOG_type(print_type)));
            HQA_Log_Message_for_Report(p, p->channel, p->rank, HQA_REPORT_FORMAT2, print_EYESCAN_LOG_type(print_type), "_Vcent_UpperBound_Window_bit", 0, EyeScanVcentUpperBound_bit, NULL);
    mcSHOW_DBG_MSG(("%s Eye Scan Vcent_UpperBound Min Window(%%)\n", print_EYESCAN_LOG_type(print_type)));
            HQA_Log_Message_for_Report(p, p->channel, p->rank, HQA_REPORT_FORMAT2, print_EYESCAN_LOG_type(print_type), "_Vcent_UpperBound_Window(%)", 0, Vcent_UpperBound_Window_percent, NULL);

    mcSHOW_DBG_MSG(("\n"));

    mcSHOW_DBG_MSG(("%s Eye Scan Vcent_LowerBound window\n", print_EYESCAN_LOG_type(print_type)));
            HQA_Log_Message_for_Report(p, p->channel, p->rank, HQA_REPORT_FORMAT2, print_EYESCAN_LOG_type(print_type), "_Vcent_LowerBound_Window", 0, EyeScanVcentLowerBound, NULL);
    mcSHOW_DBG_MSG(("%s Eye Scan Vcent_LowerBound_Window worse bit\n", print_EYESCAN_LOG_type(print_type)));
            HQA_Log_Message_for_Report(p, p->channel, p->rank, HQA_REPORT_FORMAT2, print_EYESCAN_LOG_type(print_type), "_Vcent_LowerBound_Window_bit", 0, EyeScanVcentLowerBound_bit, NULL);
    mcSHOW_DBG_MSG(("%s Eye Scan Vcent_UpperBound Min Window(%%)\n", print_EYESCAN_LOG_type(print_type)));
            HQA_Log_Message_for_Report(p, p->channel, p->rank, HQA_REPORT_FORMAT2, print_EYESCAN_LOG_type(print_type), "_Vcent_LowerBound_Window(%)", 0, Vcent_LowerBound_Window_percent, NULL);

#if defined(FOR_HQA_REPORT_USED)
    if (gHQALog_flag == 1)
    {
        if ((Vcent_UpperBound_Window_percent < 30) || (Vcent_LowerBound_Window_percent < 30))
        {
            gHQALog_SLT_BIN[vGet_Current_SRAMIdx(p)] = 4;
        }
    }
#endif

    if (print_type == EYESCAN_TYPE_RX || print_type == EYESCAN_TYPE_TX)
    {
        mcSHOW_DBG_MSG(("\n"));
        mcSHOW_DBG_MSG(("%s Eye Scan per_bit window(%%)\n", print_EYESCAN_LOG_type(print_type)));
        for (u1BitIdx = u1BitIndex_Begin; u1BitIdx < u1BitIndex_End; u1BitIdx++)
        {
#if ENABLE_EYESCAN_GRAPH
            if (print_type == EYESCAN_TYPE_RX)
            {
                Perbit_Window_percent = ((gEyeScan_WinSize[EyeScanVcent[1] / EYESCAN_GRAPH_RX_VREF_STEP][u1BitIdx] * gHQALOG_RX_delay_cell_ps_075V * DDRPhyGetRealFreq(p) * 2) + (1000000 - 1)) / 1000000;
                Perbit_BestWindow_percent = ((gEyeScan_WinSize[EyeScanVcent[10 + u1BitIdx] / EYESCAN_GRAPH_RX_VREF_STEP][u1BitIdx] * gHQALOG_RX_delay_cell_ps_075V * DDRPhyGetRealFreq(p) * 2) + (1000000 - 1)) / 1000000;
                Perbit_Window_Upperbond_percent = ((gEyeScan_WinSize[EyeScanVcent[3] / EYESCAN_GRAPH_RX_VREF_STEP][u1BitIdx] * gHQALOG_RX_delay_cell_ps_075V * DDRPhyGetRealFreq(p) * 2) + (1000000 - 1)) / 1000000;
                Perbit_Window_Lowerbond_percent = ((gEyeScan_WinSize[EyeScanVcent[5] / EYESCAN_GRAPH_RX_VREF_STEP][u1BitIdx] * gHQALOG_RX_delay_cell_ps_075V * DDRPhyGetRealFreq(p) * 2) + (1000000 - 1)) / 1000000;

                Perbit_Eye_Height = (pVref_Voltage_Table[VREF_RANGE_0][((gEyeScan_ContinueVrefHeight[u1BitIdx] >> 8) & 0xff)] - pVref_Voltage_Table[VREF_RANGE_0][(gEyeScan_ContinueVrefHeight[u1BitIdx] & 0xff)]) / 100;
                Perbit_Eye_Area = gEyeScan_TotalPassCount[u1BitIdx] * gHQALOG_RX_delay_cell_ps_075V * (((pVref_Voltage_Table[VREF_RANGE_0][((gEyeScan_ContinueVrefHeight[u1BitIdx] >> 8) & 0xff)] - pVref_Voltage_Table[VREF_RANGE_0][(gEyeScan_ContinueVrefHeight[u1BitIdx] & 0xff)])) / ((((gEyeScan_ContinueVrefHeight[u1BitIdx] >> 8) & 0xff) - (gEyeScan_ContinueVrefHeight[u1BitIdx] & 0xff)) * 10)) / 1000;
            }
            else //if (print_type==2)
            {
                Perbit_Window_percent = ((gEyeScan_WinSize[(EyeScanVcent[0] * 30 + EyeScanVcent[1]) / EYESCAN_GRAPH_CATX_VREF_STEP][u1BitIdx]) * 100 + 31) / 32;
                Perbit_BestWindow_percent = ((gEyeScan_WinSize[(EyeScanVcent[10 + u1BitIdx * 2] * 30 + EyeScanVcent[10 + u1BitIdx * 2 + 1]) / EYESCAN_GRAPH_CATX_VREF_STEP][u1BitIdx]) * 100 + 31) / 32;
                Perbit_Window_Upperbond_percent = ((gEyeScan_WinSize[(EyeScanVcent[2] * 30 + EyeScanVcent[3]) / EYESCAN_GRAPH_CATX_VREF_STEP][u1BitIdx]) * 100 + 31) / 32;
                Perbit_Window_Lowerbond_percent = ((gEyeScan_WinSize[(EyeScanVcent[4] * 30 + EyeScanVcent[5]) / EYESCAN_GRAPH_CATX_VREF_STEP][u1BitIdx]) * 100 + 31) / 32;
                Perbit_Eye_Height = (gEyeScan_ContinueVrefHeight[u1BitIdx] - 1) * 6 * vddq / 1000;
                Perbit_Eye_Area = (gEyeScan_TotalPassCount[u1BitIdx] * 10 * 3 * vddq / (32 * DDRPhyGetRealFreq(p)))*100;
            }

            HQA_Log_Message_for_Report(p, p->channel, p->rank, HQA_REPORT_FORMAT0_1, print_EYESCAN_LOG_type(print_type), "_Perbit_Window(%)", u1BitIdx, Perbit_Window_percent, NULL);
            HQA_Log_Message_for_Report(p, p->channel, p->rank, HQA_REPORT_FORMAT0_1, print_EYESCAN_LOG_type(print_type), "_Perbit_BestWindow(%)", u1BitIdx, Perbit_BestWindow_percent, NULL);
            HQA_Log_Message_for_Report(p, p->channel, p->rank, HQA_REPORT_FORMAT0_1, print_EYESCAN_LOG_type(print_type), "_Perbit_Window_Upperbond(%)", u1BitIdx, Perbit_Window_Upperbond_percent, NULL);
            HQA_Log_Message_for_Report(p, p->channel, p->rank, HQA_REPORT_FORMAT0_1, print_EYESCAN_LOG_type(print_type), "_Perbit_Window_Lowerbond(%) ", u1BitIdx, Perbit_Window_Lowerbond_percent, NULL);
            HQA_Log_Message_for_Report(p, p->channel, p->rank, HQA_REPORT_FORMAT0_1, print_EYESCAN_LOG_type(print_type), "_Perbit_Eye_Height", u1BitIdx, Perbit_Eye_Height, NULL);
            HQA_Log_Message_for_Report(p, p->channel, p->rank, HQA_REPORT_FORMAT0_1, print_EYESCAN_LOG_type(print_type), "_Perbit_Eye_Area", u1BitIdx, Perbit_Eye_Area, NULL);
#endif
        }
    }
}

void HQA_Log_Message_for_Report(DRAMC_CTX_T *p, U8 u1ChannelIdx, U8 u1RankIdx, U32 who_am_I, U8 *main_str, U8 *main_str2, U8 byte_bit_idx, S32 value1, U8 *ans_str)
{

    if (gHQALog_flag == 1)
    {
        HQA_LOG_Print_Prefix_String(p); mcSHOW_DBG_MSG(("%s%s", main_str, main_str2));
        switch (who_am_I)
        {
            case HQA_REPORT_FORMAT1:
                 mcSHOW_DBG_MSG(("%d", byte_bit_idx));
                break;
        }

        if (who_am_I == HQA_REPORT_FORMAT3)
        {
            mcSHOW_DBG_MSG((" Channel%d ", u1ChannelIdx));
        }
        else if (who_am_I != HQA_REPORT_FORMAT5 && who_am_I != HQA_REPORT_FORMAT6)
        {
            mcSHOW_DBG_MSG((" Channel%d Rank%d ", u1ChannelIdx, u1RankIdx));
        }

        switch (who_am_I)
        {
            case HQA_REPORT_FORMAT2_1:
            case HQA_REPORT_FORMAT0:
                mcSHOW_DBG_MSG(("Byte%d %d\n", byte_bit_idx, value1));
                break;
            case HQA_REPORT_FORMAT0_1:
                mcSHOW_DBG_MSG(("Bit%x %d\n", byte_bit_idx, value1));
                break;
            case HQA_REPORT_FORMAT0_2:
                mcSHOW_DBG_MSG(("CA%x %d\n", byte_bit_idx, value1));
                break;
            case HQA_REPORT_FORMAT1:
            case HQA_REPORT_FORMAT2:
            case HQA_REPORT_FORMAT3:
            case HQA_REPORT_FORMAT6:
                mcSHOW_DBG_MSG(("%d\n", value1));
                break;
            case HQA_REPORT_FORMAT4:
            case HQA_REPORT_FORMAT5:
                mcSHOW_DBG_MSG((" %s\n", ans_str));
                break;
        }
    }
}

#endif


#ifdef RELEASE
#undef mcSHOW_DBG_MSG
#define mcSHOW_DBG_MSG(_x_) opt_print _x_
#endif


#if ENABLE_EYESCAN_GRAPH
#define EyeScan_Pic_draw_line_Mirror 1
#define EysScan_Pic_draw_1UI_line 1
static void EyeScan_Pic_draw_line(DRAMC_CTX_T *p, U8 draw_type, U8 u1VrefRange, U8 u1VrefIdx, U8 u1BitIdx, S16 u2DQDelayBegin, S16 u2DQDelayEnd, U8 u1FinalVrefRange, U16 Final_Vref_val, U8 VdlVWHigh_Upper_Vcent_Range, U32 VdlVWHigh_Upper_Vcent, U8 VdlVWHigh_Lower_Vcent_Range, U32 VdlVWHigh_Lower_Vcent, U16 FinalDQCaliDelay, S16 EyeScan_DelayCellPI_value, U16 delay_cell_ps, U16 Max_EyeScan_Min_val)
{
    int i;
    int local_VrefIdx, local_Upper_Vcent, local_Lower_Vcent, local_Final_VrefIdx;
    S8 EyeScan_Index = 0;
    S16 EyeScan_Min_val, EyeScan_Max_val, Final_EyeScan_Min_val = EYESCAN_DATA_INVALID, Final_EyeScan_Max_val = EYESCAN_DATA_INVALID, Final_EyeScan_winsize = 1;
    U16 *pVref_Voltage_Table[VREF_VOLTAGE_TABLE_NUM_LP5];
    U16 PI_of_1_UI;
    U16 u2DQDelayStep=1, u2VrefStep=1;
    U16 pass_region_h_value=3;

    if (draw_type == EYESCAN_TYPE_RX)
    {
        u2DQDelayStep=4;
        u2VrefStep=EYESCAN_GRAPH_RX_VREF_STEP;
        if (p->odt_onoff==TRUE)
        {
            if (p->dram_type==TYPE_LPDDR5)
                pVref_Voltage_Table[VREF_RANGE_0] = (U16 *)gRXVref_Voltage_Table_0P5V_T;
            else
                pVref_Voltage_Table[VREF_RANGE_0] = (U16 *)gRXVref_Voltage_Table_0P6V_T;
        }
        else
        {
            if (p->dram_type==TYPE_LPDDR5)
                pVref_Voltage_Table[VREF_RANGE_0] = (U16 *)gRXVref_Voltage_Table_0P5V_UT;
            else
                pVref_Voltage_Table[VREF_RANGE_0] = (U16 *)gRXVref_Voltage_Table_0P6V_UT;
        }

        if (p->u2DelayCellTimex100 != 0)
        {
            PI_of_1_UI = (50000000 / (DDRPhyGetRealFreq(p)* p->u2DelayCellTimex100));

            FinalDQCaliDelay = (U16)EyeScan_DelayCellPI_value;
            EyeScan_DelayCellPI_value = 0;


            pass_region_h_value = PI_of_1_UI/10;

            delay_cell_ps = p->u2DelayCellTimex100;
        }
        else
        {
            PI_of_1_UI = 0;
            //mcSHOW_ERR_MSG(("DelayCell is 0\n"));
        }
    }
    else
    {
        u2VrefStep = EYESCAN_GRAPH_CATX_VREF_STEP;


        pass_region_h_value = 3;

        if (p->dram_type == TYPE_LPDDR4)
        {
            pVref_Voltage_Table[VREF_RANGE_0] = (U16 *)gVref_Voltage_Table_LP4[VREF_RANGE_0];
            pVref_Voltage_Table[VREF_RANGE_1] = (U16 *)gVref_Voltage_Table_LP4[VREF_RANGE_1];
        }
        else if (p->dram_type == TYPE_LPDDR4X)
        {
            pVref_Voltage_Table[VREF_RANGE_0] = (U16 *)gVref_Voltage_Table_LP4X[VREF_RANGE_0];
            pVref_Voltage_Table[VREF_RANGE_1] = (U16 *)gVref_Voltage_Table_LP4X[VREF_RANGE_1];
        }
        else if (p->dram_type == TYPE_LPDDR5)
        {
            pVref_Voltage_Table[VREF_RANGE_0] = (U16 *)gVref_Voltage_Table_LP5;
        }

        PI_of_1_UI = 32;
    }

    if (u1VrefRange == 1 && u1VrefIdx <= 20)
    {
        u1VrefRange = 0;
        u1VrefIdx += 30;
    }
    if (u1FinalVrefRange == 1 && Final_Vref_val <= 20)
    {
        u1FinalVrefRange = 0;
        Final_Vref_val += 30;
    }
    if (u1VrefRange != u1FinalVrefRange)
    {
        Final_Vref_val = 0xff;
    }

    local_Upper_Vcent = VdlVWHigh_Upper_Vcent_Range * VREF_VOLTAGE_TABLE_NUM_LP4+ VdlVWHigh_Upper_Vcent;
    local_Lower_Vcent = VdlVWHigh_Lower_Vcent_Range * VREF_VOLTAGE_TABLE_NUM_LP4+ VdlVWHigh_Lower_Vcent;
    local_VrefIdx = u1VrefRange * VREF_VOLTAGE_TABLE_NUM_LP4+ u1VrefIdx;
    local_Final_VrefIdx = u1FinalVrefRange * VREF_VOLTAGE_TABLE_NUM_LP4+ Final_Vref_val;

    if (VdlVWHigh_Upper_Vcent_Range == VREF_RANGE_1 && VdlVWHigh_Upper_Vcent <= 20) local_Upper_Vcent = VdlVWHigh_Upper_Vcent_Range * VREF_VOLTAGE_TABLE_NUM_LP4+ VdlVWHigh_Upper_Vcent - 20;
    if (VdlVWHigh_Lower_Vcent_Range == VREF_RANGE_1 && VdlVWHigh_Lower_Vcent <= 20) local_Lower_Vcent = VdlVWHigh_Lower_Vcent_Range * VREF_VOLTAGE_TABLE_NUM_LP4+ VdlVWHigh_Lower_Vcent - 20;

    mcSHOW_EYESCAN_MSG(("Vref-"));

    if (draw_type == EYESCAN_TYPE_RX && pVref_Voltage_Table[u1VrefRange][u1VrefIdx] <10000)
    {
        mcSHOW_EYESCAN_MSG((" "));
    }

    mcSHOW_EYESCAN_MSG(("%d.%d%d", pVref_Voltage_Table[u1VrefRange][u1VrefIdx] / 100, ((pVref_Voltage_Table[u1VrefRange][u1VrefIdx] % 100) / 10), pVref_Voltage_Table[u1VrefRange][u1VrefIdx] % 10));
    if (draw_type == EYESCAN_TYPE_RX)
    {
        mcSHOW_EYESCAN_MSG(("m|"));
    }
    else
    {
        mcSHOW_EYESCAN_MSG(("%%|"));
    }





#if VENDER_JV_LOG || defined(RELEASE)
#if EyeScan_Pic_draw_line_Mirror
    EyeScan_DelayCellPI_value = 0 - EyeScan_DelayCellPI_value;
#endif
#endif

#if EyeScan_Pic_draw_line_Mirror
    EyeScan_Index = EYESCAN_BROKEN_NUM - 1;
    EyeScan_Min_val = gEyeScan_Min[(u1VrefIdx + u1VrefRange * 30) / u2VrefStep][u1BitIdx][EyeScan_Index];
    EyeScan_Max_val = gEyeScan_Max[(u1VrefIdx + u1VrefRange * 30) / u2VrefStep][u1BitIdx][EyeScan_Index];

    while (EyeScan_Min_val == EYESCAN_DATA_INVALID && EyeScan_Index > 0)
    {
        EyeScan_Index--;
        EyeScan_Min_val = gEyeScan_Min[(u1VrefIdx + u1VrefRange * 30) / u2VrefStep][u1BitIdx][EyeScan_Index];
        EyeScan_Max_val = gEyeScan_Max[(u1VrefIdx + u1VrefRange * 30) / u2VrefStep][u1BitIdx][EyeScan_Index];
    }
#else
    EyeScan_Index = 0;
    EyeScan_Min_val = gEyeScan_Min[(u1VrefIdx + u1VrefRange * 30) / u2VrefStep][u1BitIdx][EyeScan_Index];
    EyeScan_Max_val = gEyeScan_Max[(u1VrefIdx + u1VrefRange * 30) / u2VrefStep][u1BitIdx][EyeScan_Index];
#endif

    if (draw_type == EYESCAN_TYPE_TX)
    {
        EyeScan_Min_val += gEyeScan_MinMax_store_delay[u1BitIdx / 8];
        EyeScan_Max_val += gEyeScan_MinMax_store_delay[u1BitIdx / 8];
    }

    if ((EyeScan_Max_val - EyeScan_Min_val + 1) > Final_EyeScan_winsize)
    {
#if EyeScan_Pic_draw_line_Mirror
        Final_EyeScan_Max_val = EyeScan_Max_val;
        Final_EyeScan_Min_val = EyeScan_Min_val;
#else
        Final_EyeScan_Max_val = EyeScan_Max_val;
        Final_EyeScan_Min_val = EyeScan_Min_val;
#endif
        Final_EyeScan_winsize = (EyeScan_Max_val - EyeScan_Min_val + 1);
    }

#if VENDER_JV_LOG || defined(RELEASE)
#if EyeScan_Pic_draw_line_Mirror
    for (i = (Max_EyeScan_Min_val + PI_of_1_UI + EyeScan_DelayCellPI_value) * delay_cell_ps / 100; i > (Max_EyeScan_Min_val - PI_of_1_UI + EyeScan_DelayCellPI_value) * delay_cell_ps / 100; i -= 10)
#else
    for (i = (Max_EyeScan_Min_val - PI_of_1_UI + EyeScan_DelayCellPI_value) * delay_cell_ps / 100; i < (Max_EyeScan_Min_val + PI_of_1_UI + EyeScan_DelayCellPI_value) * delay_cell_ps / 100; i += 10)
#endif
#else
#if EyeScan_Pic_draw_line_Mirror
    for (i = u2DQDelayEnd; i >= u2DQDelayBegin; i-=u2DQDelayStep)
#else
    for (i = u2DQDelayBegin; i <= u2DQDelayEnd; i+=u2DQDelayStep)
#endif
#endif
    {

#if VENDER_JV_LOG || defined(RELEASE)
#if EyeScan_Pic_draw_line_Mirror
        if (i <= ((EyeScan_Min_val + EyeScan_DelayCellPI_value) * delay_cell_ps / 100) && EyeScan_Index != 0)
        {
            EyeScan_Index--;
            EyeScan_Min_val = gEyeScan_Min[(u1VrefIdx + u1VrefRange * 30) / u2VrefStep][u1BitIdx][EyeScan_Index];
            EyeScan_Max_val = gEyeScan_Max[(u1VrefIdx + u1VrefRange * 30) / u2VrefStep][u1BitIdx][EyeScan_Index];

            if (draw_type == EYESCAN_TYPE_TX)
            {
                EyeScan_Min_val += gEyeScan_MinMax_store_delay[u1BitIdx / 8];
                EyeScan_Max_val += gEyeScan_MinMax_store_delay[u1BitIdx / 8];
            }

            if ((EyeScan_Max_val - EyeScan_Min_val + 1) > Final_EyeScan_winsize)
            {
                Final_EyeScan_Max_val = EyeScan_Max_val;
                Final_EyeScan_Min_val = EyeScan_Min_val;
                Final_EyeScan_winsize = (EyeScan_Max_val - EyeScan_Min_val + 1);
            }

        }
#endif
#else
#if EyeScan_Pic_draw_line_Mirror
        if (i == (EyeScan_Min_val) && EyeScan_Index != 0)
        {
            EyeScan_Index--;
            EyeScan_Min_val = gEyeScan_Min[(u1VrefIdx + u1VrefRange * 30) / u2VrefStep][u1BitIdx][EyeScan_Index];
            EyeScan_Max_val = gEyeScan_Max[(u1VrefIdx + u1VrefRange * 30) / u2VrefStep][u1BitIdx][EyeScan_Index];

            if (draw_type == EYESCAN_TYPE_TX)
            {
                EyeScan_Min_val += gEyeScan_MinMax_store_delay[u1BitIdx / 8];
                EyeScan_Max_val += gEyeScan_MinMax_store_delay[u1BitIdx / 8];
            }

            if ((EyeScan_Max_val - EyeScan_Min_val + 1) > Final_EyeScan_winsize)
            {
                Final_EyeScan_Max_val = EyeScan_Max_val;
                Final_EyeScan_Min_val = EyeScan_Min_val;
                Final_EyeScan_winsize = (EyeScan_Max_val - EyeScan_Min_val + 1);
            }

        }
#endif
#endif

#if VENDER_JV_LOG || defined(RELEASE)
        if (i >= ((EyeScan_Min_val + EyeScan_DelayCellPI_value) * delay_cell_ps / 100) && i <= ((EyeScan_Max_val + EyeScan_DelayCellPI_value) * delay_cell_ps / 100))
#else
        if (i >= (EyeScan_Min_val) && i <= (EyeScan_Max_val))
#endif
        {
#if !VENDER_JV_LOG && !defined(RELEASE)
            if ((i>=(FinalDQCaliDelay+EyeScan_DelayCellPI_value-(u2DQDelayStep/2)))&&(i<=(FinalDQCaliDelay+EyeScan_DelayCellPI_value+(u2DQDelayStep/2))))
            {
                if (gEye_Scan_color_flag)
                {
                    mcSHOW_EYESCAN_MSG(("\033[0;105mH\033[m"));
                }
                else
                {
                    mcSHOW_EYESCAN_MSG(("H"));
                }
            }
            else
            if ((local_VrefIdx >= local_Final_VrefIdx-(u2VrefStep/2))&&(local_VrefIdx <= local_Final_VrefIdx+(u2VrefStep/2)))
            {
                if (gEye_Scan_color_flag)
                {
                    mcSHOW_EYESCAN_MSG(("\033[0;105mV\033[m"));
                }
                else
                {
                    mcSHOW_EYESCAN_MSG(("V"));
                }
            }
            else
            if (local_VrefIdx <= local_Upper_Vcent && local_VrefIdx >= local_Lower_Vcent && i >= (FinalDQCaliDelay + EyeScan_DelayCellPI_value - pass_region_h_value) && i <= (FinalDQCaliDelay + EyeScan_DelayCellPI_value + pass_region_h_value))
            {
                if (gEye_Scan_color_flag)
                {
                    mcSHOW_EYESCAN_MSG(("\033[0;103mQ\033[m"));
                }
                else
                {
                    mcSHOW_EYESCAN_MSG(("Q"));
                }
            }
            else
#endif
            {
#if VENDER_JV_LOG || defined(RELEASE)
                if (gEye_Scan_color_flag)
                {
                    mcSHOW_EYESCAN_MSG(("\033[0;102mO\033[m"));
                }
                else
                {
                    mcSHOW_EYESCAN_MSG(("O"));
                }
#else
                if (gEye_Scan_color_flag)
                {
                    mcSHOW_EYESCAN_MSG(("\033[0;102mO\033[m"));
                }
                else
                {
                    mcSHOW_EYESCAN_MSG(("O"));
                }
#endif
            }
        }
        else
        {
#if !VENDER_JV_LOG && !defined(RELEASE)
#if EysScan_Pic_draw_1UI_line
            if (i == (int)(Max_EyeScan_Min_val) || i == (int)(Max_EyeScan_Min_val + PI_of_1_UI))
            {
                if (gEye_Scan_color_flag)
                {
                    if (p->dram_cbt_mode[p->rank] == CBT_BYTE_MODE1 && p->dram_fsp == FSP_0 && draw_type==0 && u1VrefIdx < EYESCAN_SKIP_UNTERM_CBT_EYESCAN_VREF)
                    {
                        mcSHOW_EYESCAN_MSG(("\033[0;107m \033[m"));
                    }
                    else
                    {
                        mcSHOW_EYESCAN_MSG(("\033[0;107m.\033[m"));
                    }
                }
                else
                {
                    if (p->dram_cbt_mode[p->rank] == CBT_BYTE_MODE1 && p->dram_fsp == FSP_0 && draw_type==0 && u1VrefIdx < EYESCAN_SKIP_UNTERM_CBT_EYESCAN_VREF)
                    {
                        mcSHOW_EYESCAN_MSG((" "));
                    }
                    else
                    {
                        mcSHOW_EYESCAN_MSG(("."));
                    }
                }
            }
            else
#endif
#endif
            {

#if VENDER_JV_LOG || defined(RELEASE)
                if (gEye_Scan_color_flag)
                {
                    if (p->dram_cbt_mode[p->rank] == CBT_BYTE_MODE1 && p->dram_fsp == FSP_0 && draw_type==0 && u1VrefIdx < EYESCAN_SKIP_UNTERM_CBT_EYESCAN_VREF)
                    {
                        mcSHOW_EYESCAN_MSG(("\033[0;100m \033[m"));
                    }
                    else
                    {
                        mcSHOW_EYESCAN_MSG(("\033[0;100m.\033[m"));
                    }
                }
                else
                {
                    if (p->dram_cbt_mode[p->rank] == CBT_BYTE_MODE1 && p->dram_fsp == FSP_0 && draw_type==0 && u1VrefIdx < EYESCAN_SKIP_UNTERM_CBT_EYESCAN_VREF)
                    {
                        mcSHOW_EYESCAN_MSG((" "));
                    }
                    else
                    {
                        mcSHOW_EYESCAN_MSG(("."));
                    }
                }
#else
                if (gEye_Scan_color_flag)
                {
                    if (p->dram_cbt_mode[p->rank] == CBT_BYTE_MODE1 && p->dram_fsp == FSP_0 && draw_type==0 && u1VrefIdx < EYESCAN_SKIP_UNTERM_CBT_EYESCAN_VREF)
                    {
                        mcSHOW_EYESCAN_MSG(("\033[0;100m \033[m"));
                    }
                    else
                    {
                        mcSHOW_EYESCAN_MSG(("\033[0;100m.\033[m"));
                    }
                }
                else
                {
                    if (p->dram_cbt_mode[p->rank] == CBT_BYTE_MODE1 && p->dram_fsp == FSP_0 && draw_type==0 && u1VrefIdx < EYESCAN_SKIP_UNTERM_CBT_EYESCAN_VREF)
                    {
                        mcSHOW_EYESCAN_MSG((" "));
                    }
                    else
                    {
                        mcSHOW_EYESCAN_MSG(("."));
                    }
                }
#endif

            }
        }
    }


#if EyeScan_Pic_draw_line_Mirror
    if (Final_EyeScan_Min_val != EYESCAN_DATA_INVALID && Final_EyeScan_Max_val != EYESCAN_DATA_INVALID)
    {
#if !VENDER_JV_LOG && !defined(RELEASE)
        if (Final_EyeScan_Max_val > (FinalDQCaliDelay + EyeScan_DelayCellPI_value) && (FinalDQCaliDelay + EyeScan_DelayCellPI_value) > Final_EyeScan_Min_val)
        {
            mcSHOW_EYESCAN_MSG((" -%d ", (Final_EyeScan_Max_val - (FinalDQCaliDelay + EyeScan_DelayCellPI_value))));
            mcSHOW_EYESCAN_MSG(("%d ", ((FinalDQCaliDelay + EyeScan_DelayCellPI_value) - Final_EyeScan_Min_val)));
        }
        else if (Final_EyeScan_Max_val > (FinalDQCaliDelay + EyeScan_DelayCellPI_value) && Final_EyeScan_Min_val > (FinalDQCaliDelay + EyeScan_DelayCellPI_value))
        {
            mcSHOW_EYESCAN_MSG((" -%d ", (Final_EyeScan_Max_val - Final_EyeScan_Min_val)));
            mcSHOW_EYESCAN_MSG((" --- "));
        }
        else if ((FinalDQCaliDelay + EyeScan_DelayCellPI_value) > Final_EyeScan_Max_val && (FinalDQCaliDelay + EyeScan_DelayCellPI_value) > Final_EyeScan_Min_val)
        {
            mcSHOW_EYESCAN_MSG((" --- "));
            mcSHOW_EYESCAN_MSG(("%d ", (Final_EyeScan_Max_val - Final_EyeScan_Min_val)));
        }
        else
        {
            mcSHOW_EYESCAN_MSG((" --- "));
            mcSHOW_EYESCAN_MSG((" --- "));
        }
#endif

#if VENDER_JV_LOG || defined(RELEASE)
        mcSHOW_EYESCAN_MSG(("%dps", Final_EyeScan_winsize * delay_cell_ps / 100));
#else
        mcSHOW_EYESCAN_MSG(("%d", Final_EyeScan_winsize));
#endif
    }
#else
    if (Final_EyeScan_Max_val != Final_EyeScan_Min_val && Final_EyeScan_Max_val != EYESCAN_DATA_INVALID)
    {
#if !VENDER_JV_LOG && !defined(RELEASE)
        if (Final_EyeScan_Max_val > (FinalDQCaliDelay + EyeScan_DelayCellPI_value) && (FinalDQCaliDelay + EyeScan_DelayCellPI_value) > Final_EyeScan_Min_val)
        {
            mcSHOW_EYESCAN_MSG((" -%d ", ((FinalDQCaliDelay + EyeScan_DelayCellPI_value) - Final_EyeScan_Min_val)));
            mcSHOW_EYESCAN_MSG(("%d ", (Final_EyeScan_Max_val - (FinalDQCaliDelay + EyeScan_DelayCellPI_value))));
        }
        else if (Final_EyeScan_Max_val > (FinalDQCaliDelay + EyeScan_DelayCellPI_value) && Final_EyeScan_Min_val > (FinalDQCaliDelay + EyeScan_DelayCellPI_value))
        {
            mcSHOW_EYESCAN_MSG((" --- "));
            mcSHOW_EYESCAN_MSG(("%d ", (Final_EyeScan_Max_val - Final_EyeScan_Min_val)));
        }
        else if ((FinalDQCaliDelay + EyeScan_DelayCellPI_value) > Final_EyeScan_Max_val && (FinalDQCaliDelay + EyeScan_DelayCellPI_value) > Final_EyeScan_Min_val)
        {
            mcSHOW_EYESCAN_MSG((" -%d ", (Final_EyeScan_Max_val - Final_EyeScan_Min_val)));
            mcSHOW_EYESCAN_MSG((" --- "));
        }
        else
        {
            mcSHOW_EYESCAN_MSG((" --- "));
            mcSHOW_EYESCAN_MSG((" --- "));
        }
#endif

#if VENDER_JV_LOG || defined(RELEASE)
        mcSHOW_EYESCAN_MSG(("%dps", Final_EyeScan_winsize * delay_cell_ps / 100));
#else
        mcSHOW_EYESCAN_MSG(("%d", Final_EyeScan_winsize));
#endif
    }
#endif

    mcSHOW_EYESCAN_MSG(("\n"));


}

void print_EYESCAN_LOG_message(DRAMC_CTX_T *p, U8 print_type)
{
    U32 u1ChannelIdx = p->channel, u1RankIdx = p->rank;
    S8 u1VrefIdx;
    U8 u1VrefRange;
    U8 u1BitIdx, u1CA;
    U32 VdlVWTotal, VdlVWLow, VdlVWHigh, Vcent_DQ;
    U32 VdlVWHigh_Upper_Vcent = VREF_VOLTAGE_TABLE_NUM_LP4- 1, VdlVWHigh_Lower_Vcent = 0, VdlVWBest_Vcent = 0;
    U32 VdlVWHigh_Upper_Vcent_Range = 1, VdlVWHigh_Lower_Vcent_Range = 0, VdlVWBest_Vcent_Range = 1; ;
    U8 Upper_Vcent_pass_flag = 0, Lower_Vcent_pass_flag = 0;
    S32 i, vrefrange_i;
    U8 local_channel_num = 2;
    U8 SRAMIdx;
    U8 TXVrefRange = 0, CBTVrefRange = 0;
    U16 finalVref = 0, finalVrefRange = 0;
    S32 scan_vref_number = 0;
    U32 vddq;
    U8 Min_Value_1UI_Line;
    S8 EyeScan_Index;
    U8 cal_length = 0;
    U16 *pVref_Voltage_Table[VREF_VOLTAGE_TABLE_NUM_LP5];
    S16 EyeScan_DelayCellPI_value;
    U8 EyeScanVcent[10 + DQ_DATA_WIDTH * 2], max_winsize;
    U8 minEyeScanVcentUpperBound = 0xff, minEyeScanVcentUpperBound_bit = 0;
    U8 minEyeScanVcentLowerBound = 0xff, minEyeScanVcentLowerBound_bit = 0;
    U16 one_pi_ps = 100000000 / (DDRPhyGetRealFreq(p)* 2 * 32);
    U8 u1CBTEyeScanEnable, u1RXEyeScanEnable, u1TXEyeScanEnable;

    U32 u2DQDelayEnd=0, u2TX_DQ_PreCal_LP4_Samll;
    S32 u2DQDelayBegin=0;

    U8 u1ByteIdx, u1ByteIdx_Begin = 0, u1ByteIdx_End = 0;

    U16 u2VrefStep=1;

    local_channel_num = p->support_channel_num;

     if (print_type == EYESCAN_TYPE_RX)
    {
        u2VrefStep = EYESCAN_GRAPH_RX_VREF_STEP;

        if (p->odt_onoff==TRUE)
        {
            if (p->dram_type==TYPE_LPDDR5)
                pVref_Voltage_Table[VREF_RANGE_0] = (U16 *)gRXVref_Voltage_Table_0P5V_T;
            else
                pVref_Voltage_Table[VREF_RANGE_0] = (U16 *)gRXVref_Voltage_Table_0P6V_T;
        }
    else
    {
         if (p->dram_type==TYPE_LPDDR5)
                pVref_Voltage_Table[VREF_RANGE_0] = (U16 *)gRXVref_Voltage_Table_0P5V_UT;
            else
                pVref_Voltage_Table[VREF_RANGE_0] = (U16 *)gRXVref_Voltage_Table_0P6V_UT;
    }
    }
    else
    {
        u2VrefStep = EYESCAN_GRAPH_CATX_VREF_STEP;

        if (p->dram_type == TYPE_LPDDR4)
        {
            pVref_Voltage_Table[VREF_RANGE_0] = (U16 *)gVref_Voltage_Table_LP4[VREF_RANGE_0];
            pVref_Voltage_Table[VREF_RANGE_1] = (U16 *)gVref_Voltage_Table_LP4[VREF_RANGE_1];
        }
        if (p->dram_type == TYPE_LPDDR4X)
        {
            pVref_Voltage_Table[VREF_RANGE_0] = (U16 *)gVref_Voltage_Table_LP4X[VREF_RANGE_0];
            pVref_Voltage_Table[VREF_RANGE_1] = (U16 *)gVref_Voltage_Table_LP4X[VREF_RANGE_1];
        }
        if (p->dram_type == TYPE_LPDDR5)
        {
            pVref_Voltage_Table[VREF_RANGE_0] = (U16 *)gVref_Voltage_Table_LP5;
        }
    }

    u1CBTEyeScanEnable = GetEyeScanEnable(p, EYESCAN_TYPE_CBT);
    u1RXEyeScanEnable =  GetEyeScanEnable(p, EYESCAN_TYPE_RX);
    u1TXEyeScanEnable =  GetEyeScanEnable(p, EYESCAN_TYPE_TX);

#if 0
    u1CBTEyeScanEnable = u1CBTEyeScanEnable & (p->channel == 0 && p->rank == 0);
#endif

    if (print_type == EYESCAN_TYPE_CBT)
    {
        if (p->frequency <= 934) VdlVWTotal = 17500;
        else if (p->frequency <= 1600) VdlVWTotal = 15500;
        else VdlVWTotal = 14500;
    }
    else
    {
#if 0
        if (p->frequency <= 1600) VdlVWTotal = 10000;
        else VdlVWTotal = 10000;
#else
        VdlVWTotal = 10000;
#endif
    }

    if (p->dram_type!=TYPE_LPDDR5)
    {
        CBTVrefRange = (u1MR12Value[p->channel][p->rank][p->dram_fsp] >> 6) & 1;
        TXVrefRange = (u1MR14Value[p->channel][p->rank][p->dram_fsp] >> 6) & 1;
    }

    if (print_type == EYESCAN_TYPE_RX)
    {
        u1ByteIdx_Begin = 0;
        u1ByteIdx_End = 1;
    }

for (u1ByteIdx = u1ByteIdx_Begin; u1ByteIdx <= u1ByteIdx_End; u1ByteIdx++)
{

#if 1 //#if !VENDER_JV_LOG && !defined(RELEASE)
    if ((print_type == EYESCAN_TYPE_CBT && u1CBTEyeScanEnable) || (print_type == EYESCAN_TYPE_RX && u1RXEyeScanEnable) || (print_type == EYESCAN_TYPE_TX && u1TXEyeScanEnable))
    {
        vddq = vGetVoltage(p, 2) / 1000;
        EYESCAN_LOG_Print_Prefix_String(); mcSHOW_DBG_MSG(("VDDQ=%dmV\n", vddq));

        if (print_type == EYESCAN_TYPE_CBT)
        {
            if (p->dram_type==TYPE_LPDDR5)
            {
                finalVref = u1MR12Value[u1ChannelIdx][u1RankIdx][p->dram_fsp];
            }
            else
            {
                finalVrefRange = u1MR12Value[u1ChannelIdx][u1RankIdx][p->dram_fsp]>>6;
                finalVref = u1MR12Value[u1ChannelIdx][u1RankIdx][p->dram_fsp]&0x3f;
            }
            cal_length = CATRAINING_NUM_LP4;
        }
        else if (print_type == EYESCAN_TYPE_RX)
        {
            finalVrefRange = 0;
            finalVref = gFinalRXVrefDQ[u1ChannelIdx][u1RankIdx][u1ByteIdx];

            cal_length = DQS_BIT_NUMBER;
        }
        else//if (print_type==2)
        {
            if (p->dram_type==TYPE_LPDDR5)
            {
                finalVref = u1MR14Value[u1ChannelIdx][u1RankIdx][p->dram_fsp];
            }
            else
            {
                finalVrefRange = u1MR14Value[u1ChannelIdx][u1RankIdx][p->dram_fsp]>>6;
                finalVref = u1MR14Value[u1ChannelIdx][u1RankIdx][p->dram_fsp]&0x3f;
            }

            cal_length = p->data_width;
        }


        EYESCAN_LOG_Print_Prefix_String(); mcSHOW_DBG_MSG(("%s Window\n", print_EYESCAN_LOG_type(print_type)));

        EYESCAN_LOG_Print_Prefix_String(); mcSHOW_DBG_MSG(("%s Channel%d ", print_EYESCAN_LOG_type(print_type), u1ChannelIdx));
        if (print_type == EYESCAN_TYPE_RX)
        {
            mcSHOW_DBG_MSG(("Final_Vref Byte%d Vcent %d(%dmV(X100))\n",
                                                    u1ByteIdx,
                                                    finalVref,
                                                    pVref_Voltage_Table[finalVrefRange][finalVref]));

            Vcent_DQ = pVref_Voltage_Table[finalVrefRange][finalVref];
        }
        else
        {
            mcSHOW_DBG_MSG(("Range %d Final_Vref Vcent=%d(%dmV(X100))\n",
                                                finalVrefRange,
                                                finalVref,
                                                pVref_Voltage_Table[finalVrefRange][finalVref] * vddq / 100));

            Vcent_DQ = pVref_Voltage_Table[finalVrefRange][finalVref] * vddq / 100;
        }



        if (print_type == EYESCAN_TYPE_RX)
        {
            VdlVWHigh_Upper_Vcent_Range = 0;
            VdlVWHigh_Upper_Vcent = RX_VREF_RANGE_END-1;
            vrefrange_i = 0;
            scan_vref_number = RX_VREF_RANGE_END;
        }
        else
        {
            if (p->dram_type==TYPE_LPDDR5)
            {
                VdlVWHigh_Upper_Vcent_Range = 0;
                VdlVWHigh_Upper_Vcent = VREF_VOLTAGE_TABLE_NUM_LP5-1;
                vrefrange_i = 0;
                scan_vref_number = VREF_VOLTAGE_TABLE_NUM_LP5;
            }
            else
            {
                VdlVWHigh_Upper_Vcent_Range = 1;
                VdlVWHigh_Upper_Vcent = VREF_VOLTAGE_TABLE_NUM_LP4-1;
                vrefrange_i = finalVrefRange;
                scan_vref_number = VREF_VOLTAGE_TABLE_NUM_LP4;
            }
        }
        for (i = finalVref; i < scan_vref_number; i += u2VrefStep)
        {
            if (print_type == EYESCAN_TYPE_RX||(print_type != EYESCAN_TYPE_RX&&p->dram_type==TYPE_LPDDR5))
            {
                if (pVref_Voltage_Table[vrefrange_i][i] - Vcent_DQ >= VdlVWTotal / 2)
                {

                    VdlVWHigh_Upper_Vcent = i;
                    break;
                }
            }
            else
            {
                if (((pVref_Voltage_Table[vrefrange_i][i] * vddq / 100 - Vcent_DQ)) >= VdlVWTotal / 2)
                {

                    VdlVWHigh_Upper_Vcent = i;
                    VdlVWHigh_Upper_Vcent_Range = vrefrange_i;
                    break;
                }
                if (i == (VREF_VOLTAGE_TABLE_NUM_LP4- 1) && vrefrange_i == 0)
                {
                    vrefrange_i = 1;
                    i = 20;
                }
            }
        }

        EYESCAN_LOG_Print_Prefix_String(); mcSHOW_DBG_MSG(("%s VdlVWHigh_Upper ", print_EYESCAN_LOG_type(print_type)));
        if (print_type != EYESCAN_TYPE_RX)
        {
            mcSHOW_DBG_MSG(("Range=%d ", VdlVWHigh_Upper_Vcent_Range));
            mcSHOW_DBG_MSG(("Vcent=%d(%dmV(X100))\n",
                VdlVWHigh_Upper_Vcent,
                pVref_Voltage_Table[VdlVWHigh_Upper_Vcent_Range][VdlVWHigh_Upper_Vcent] * vddq / 100));
        }
        else
        {
            mcSHOW_DBG_MSG(("Byte%d ", u1ByteIdx));
            mcSHOW_DBG_MSG(("Vcent=%d(%dmV(X100))\n",
                VdlVWHigh_Upper_Vcent,
                pVref_Voltage_Table[VdlVWHigh_Upper_Vcent_Range][VdlVWHigh_Upper_Vcent]));
        }



        VdlVWHigh_Lower_Vcent_Range = 0;
        VdlVWHigh_Lower_Vcent = 0;
        vrefrange_i = finalVrefRange;
        for (i = (finalVref); i >= 0; i -= u2VrefStep)
        {
            if (print_type == EYESCAN_TYPE_RX||(print_type != EYESCAN_TYPE_RX&&p->dram_type==TYPE_LPDDR5))
            {
                if (Vcent_DQ - pVref_Voltage_Table[vrefrange_i][i] >= VdlVWTotal / 2)
                {

                    VdlVWHigh_Lower_Vcent = i;
                    break;
                }
            }
            else
            {
                if (((Vcent_DQ - pVref_Voltage_Table[vrefrange_i][i] * vddq / 100)) >= VdlVWTotal / 2)
                {

                    VdlVWHigh_Lower_Vcent = i;
                    VdlVWHigh_Lower_Vcent_Range = vrefrange_i;
                    break;
                }
                if (i <= 21 && vrefrange_i == 1)
                {
                    vrefrange_i = 0;
                    i = VREF_VOLTAGE_TABLE_NUM_LP4- (21 - i);
                }
            }
        }

        EYESCAN_LOG_Print_Prefix_String(); mcSHOW_DBG_MSG(("%s VdlVWHigh_Lower ", print_EYESCAN_LOG_type(print_type)));
        if (print_type != EYESCAN_TYPE_RX)
        {
            mcSHOW_DBG_MSG(("Range=%d ", VdlVWHigh_Lower_Vcent_Range));
            mcSHOW_DBG_MSG(("Vcent=%d(%dmV(X100))\n",
                VdlVWHigh_Lower_Vcent,
                pVref_Voltage_Table[VdlVWHigh_Lower_Vcent_Range][VdlVWHigh_Lower_Vcent] * vddq / 100));
        }
        else
        {
            mcSHOW_DBG_MSG(("Byte%d ", u1ByteIdx));
            mcSHOW_DBG_MSG(("Vcent=%d(%dmV(X100))\n",
                VdlVWHigh_Lower_Vcent,
                pVref_Voltage_Table[VdlVWHigh_Lower_Vcent_Range][VdlVWHigh_Lower_Vcent]));
        }


#ifdef FOR_HQA_TEST_USED
            EyeScanVcent[0] = finalVrefRange;
            EyeScanVcent[1] = finalVref;
            EyeScanVcent[2] = VdlVWHigh_Upper_Vcent_Range;
            EyeScanVcent[3] = VdlVWHigh_Upper_Vcent;
            EyeScanVcent[4] = VdlVWHigh_Lower_Vcent_Range;
            EyeScanVcent[5] = VdlVWHigh_Lower_Vcent;
#endif

        SRAMIdx = vGet_Current_SRAMIdx(p);

#ifdef FOR_HQA_REPORT_USED
        if (print_type == EYESCAN_TYPE_RX)
        {
            EYESCAN_LOG_Print_Prefix_String(); mcSHOW_DBG_MSG(("delay cell %d/100ps\n", gHQALOG_RX_delay_cell_ps_075V));
        }
#endif

//            EYESCAN_LOG_Print_Prefix_String(); mcSHOW_DBG_MSG(("delay cell %d/100ps\n", u2gdelay_cell_ps_all[SRAMIdx][u1ChannelIdx]));

        for (u1BitIdx = u1ByteIdx * DQS_BIT_NUMBER; u1BitIdx < (u1ByteIdx * DQS_BIT_NUMBER + cal_length); u1BitIdx++)
        {
            if (print_type != EYESCAN_TYPE_RX)
            {

                for (u1VrefIdx = finalVref + finalVrefRange * 30; u1VrefIdx <= (S8)(VdlVWHigh_Upper_Vcent + VdlVWHigh_Upper_Vcent_Range * 30); u1VrefIdx += u2VrefStep)
                {
                    Upper_Vcent_pass_flag = 0;
                    for (EyeScan_Index = 0; EyeScan_Index < EYESCAN_BROKEN_NUM; EyeScan_Index++)
                    {
                        if (gEyeScan_Min[u1VrefIdx / u2VrefStep][u1BitIdx][EyeScan_Index] != EYESCAN_DATA_INVALID && gEyeScan_Max[u1VrefIdx / u2VrefStep][u1BitIdx][EyeScan_Index] != EYESCAN_DATA_INVALID)
                        if ((print_type == EYESCAN_TYPE_CBT && ((((gEyeScan_CaliDelay[0] + gEyeScan_DelayCellPI[u1BitIdx]) - gEyeScan_Min[u1VrefIdx / u2VrefStep][u1BitIdx][EyeScan_Index]) >= 3) && ((gEyeScan_Max[u1VrefIdx / u2VrefStep][u1BitIdx][EyeScan_Index] - (gEyeScan_CaliDelay[0] + gEyeScan_DelayCellPI[u1BitIdx])) >= 3))) ||
                            (print_type == EYESCAN_TYPE_TX && ((((gEyeScan_CaliDelay[u1BitIdx / 8] + gEyeScan_DelayCellPI[u1BitIdx]) - (gEyeScan_Min[u1VrefIdx / u2VrefStep][u1BitIdx][EyeScan_Index] + gEyeScan_MinMax_store_delay[u1BitIdx / 8])) >= 3) && (((gEyeScan_Max[u1VrefIdx / u2VrefStep][u1BitIdx][EyeScan_Index] + gEyeScan_MinMax_store_delay[u1BitIdx / 8]) - (gEyeScan_CaliDelay[u1BitIdx / 8] + gEyeScan_DelayCellPI[u1BitIdx])) >= 3 ))))
                        {
                            Upper_Vcent_pass_flag = 1;
                        }
                    }
                    if (Upper_Vcent_pass_flag == 0) break;
                }
                for (u1VrefIdx = VdlVWHigh_Lower_Vcent + VdlVWHigh_Lower_Vcent_Range * 30; u1VrefIdx <= (S8)(finalVref + finalVrefRange * 30); u1VrefIdx += u2VrefStep)
                {
                    Lower_Vcent_pass_flag = 0;
                    for (EyeScan_Index = 0; EyeScan_Index < EYESCAN_BROKEN_NUM; EyeScan_Index++)
                    {
                        if (gEyeScan_Min[u1VrefIdx / u2VrefStep][u1BitIdx][EyeScan_Index] != EYESCAN_DATA_INVALID && gEyeScan_Max[u1VrefIdx / u2VrefStep][u1BitIdx][EyeScan_Index] != EYESCAN_DATA_INVALID)
                        if ((print_type == EYESCAN_TYPE_CBT && ((((gEyeScan_CaliDelay[0] + gEyeScan_DelayCellPI[u1BitIdx]) - gEyeScan_Min[u1VrefIdx / u2VrefStep][u1BitIdx][EyeScan_Index]) >= 3) && ((gEyeScan_Max[u1VrefIdx / u2VrefStep][u1BitIdx][EyeScan_Index] - (gEyeScan_CaliDelay[0] + gEyeScan_DelayCellPI[u1BitIdx])) >= 3))) ||
                             (print_type == EYESCAN_TYPE_TX && ((((gEyeScan_CaliDelay[u1BitIdx / 8] + gEyeScan_DelayCellPI[u1BitIdx]) - (gEyeScan_Min[u1VrefIdx / u2VrefStep][u1BitIdx][EyeScan_Index] + gEyeScan_MinMax_store_delay[u1BitIdx / 8])) >= 3) && (((gEyeScan_Max[u1VrefIdx / u2VrefStep][u1BitIdx][EyeScan_Index] + gEyeScan_MinMax_store_delay[u1BitIdx / 8]) - (gEyeScan_CaliDelay[u1BitIdx / 8] + gEyeScan_DelayCellPI[u1BitIdx])) >= 3 ))))
                        {
                            Lower_Vcent_pass_flag = 1;
                        }
                    }
                    if (Lower_Vcent_pass_flag == 0) break;
                }
            }

#ifdef FOR_HQA_TEST_USED

            VdlVWBest_Vcent_Range = 1;
            VdlVWBest_Vcent = VREF_VOLTAGE_TABLE_NUM_LP4- 1;
            if (print_type == EYESCAN_TYPE_RX||(print_type != EYESCAN_TYPE_RX&&p->dram_type==TYPE_LPDDR5)) vrefrange_i = 0;
            else vrefrange_i = 1;
            max_winsize = 0;
            for (i = ((scan_vref_number-1) - ((scan_vref_number-1)%u2VrefStep)); i >= 0; i -= u2VrefStep)
            {
                if (gEyeScan_WinSize[(i + vrefrange_i * 30) / u2VrefStep][u1BitIdx] > max_winsize)
                {
                    max_winsize = gEyeScan_WinSize[(i + vrefrange_i * 30) / u2VrefStep][u1BitIdx];
                    VdlVWBest_Vcent_Range = vrefrange_i;
                    VdlVWBest_Vcent = i;
                }
                if (print_type != EYESCAN_TYPE_RX && i == 21 && vrefrange_i == 1)
                {
                    vrefrange_i = 0;
                    i = VREF_VOLTAGE_TABLE_NUM_LP4;
                }
            }

            if (print_type == EYESCAN_TYPE_RX)
            {
                EyeScanVcent[10 + u1BitIdx] = VdlVWBest_Vcent;
            }
            else
            {
                EyeScanVcent[10 + u1BitIdx * 2] = VdlVWBest_Vcent_Range;
                EyeScanVcent[10 + u1BitIdx * 2 + 1] = VdlVWBest_Vcent;
            }
#endif

            if (print_type == EYESCAN_TYPE_RX)
            {
#ifdef FOR_HQA_REPORT_USED
                EYESCAN_LOG_Print_Prefix_String(); HQA_LOG_Print_Freq_String(p); mcSHOW_DBG_MSG(("Channel%d Rank%d Bit%d(DRAM DQ%d)\tHigher VdlTW=%d/100ps\tLower VdlTW=%d/100ps\n",
                    u1ChannelIdx,
                    u1RankIdx,
                    u1BitIdx,
                    uiLPDDR4_O1_Mapping_POP[p->channel][u1BitIdx],
                    gEyeScan_WinSize[VdlVWHigh_Upper_Vcent / u2VrefStep][u1BitIdx] * gHQALOG_RX_delay_cell_ps_075V,
                    gEyeScan_WinSize[VdlVWHigh_Lower_Vcent / u2VrefStep][u1BitIdx] * gHQALOG_RX_delay_cell_ps_075V
                    ));
#endif
            }
            else
            {
                EYESCAN_LOG_Print_Prefix_String(); HQA_LOG_Print_Freq_String(p); mcSHOW_DBG_MSG(("Channel%d Rank%d %s%d",
                    u1ChannelIdx,
                    u1RankIdx,
                    print_type == EYESCAN_TYPE_CBT? "CA" : "Bit",
                    u1BitIdx
                    ));
                    if (print_type == EYESCAN_TYPE_TX)
                    {
                        mcSHOW_DBG_MSG(("(DRAM DQ%d)", uiLPDDR4_O1_Mapping_POP[p->channel][u1BitIdx]));
                    }
                mcSHOW_DBG_MSG(("\tHigher VdlTW=%dPI(%d/100ps)(%s)\tLower VdlTW=%dpi(%d/100ps)(%s)\n",
                    gEyeScan_WinSize[(VdlVWHigh_Upper_Vcent + VdlVWHigh_Upper_Vcent_Range * 30) / u2VrefStep][u1BitIdx],
                    gEyeScan_WinSize[(VdlVWHigh_Upper_Vcent + VdlVWHigh_Upper_Vcent_Range * 30) / u2VrefStep][u1BitIdx] * one_pi_ps,
                    Upper_Vcent_pass_flag == 1? "PASS" : "WARNING",
                    gEyeScan_WinSize[(VdlVWHigh_Lower_Vcent + VdlVWHigh_Lower_Vcent_Range * 30) / u2VrefStep][u1BitIdx],
                    gEyeScan_WinSize[(VdlVWHigh_Lower_Vcent + VdlVWHigh_Lower_Vcent_Range * 30) / u2VrefStep][u1BitIdx] * one_pi_ps,
                    Lower_Vcent_pass_flag == 1? "PASS" : "WARNING"
                    ));
            }

            if (gEyeScan_WinSize[(VdlVWHigh_Upper_Vcent + VdlVWHigh_Upper_Vcent_Range * 30) / u2VrefStep][u1BitIdx] < minEyeScanVcentUpperBound)
            {
                minEyeScanVcentUpperBound = gEyeScan_WinSize[(VdlVWHigh_Upper_Vcent + VdlVWHigh_Upper_Vcent_Range * 30) / u2VrefStep][u1BitIdx];
                minEyeScanVcentUpperBound_bit = u1BitIdx;
            }
            if (gEyeScan_WinSize[(VdlVWHigh_Lower_Vcent + VdlVWHigh_Lower_Vcent_Range * 30) / u2VrefStep][u1BitIdx] < minEyeScanVcentLowerBound)
            {
                minEyeScanVcentLowerBound = gEyeScan_WinSize[(VdlVWHigh_Lower_Vcent + VdlVWHigh_Lower_Vcent_Range * 30) / u2VrefStep][u1BitIdx];
                minEyeScanVcentLowerBound_bit = u1BitIdx;
            }
        }
#ifdef FOR_HQA_TEST_USED
#ifdef FOR_HQA_REPORT_USED
        print_EyeScanVcent_for_HQA_report_used(p, print_type, u1ChannelIdx, u1RankIdx, u1ByteIdx, EyeScanVcent, minEyeScanVcentUpperBound, minEyeScanVcentUpperBound_bit, minEyeScanVcentLowerBound, minEyeScanVcentLowerBound_bit);
#endif
#endif
    }


#endif


    if ((print_type == EYESCAN_TYPE_CBT && u1CBTEyeScanEnable) || (print_type == EYESCAN_TYPE_RX && u1RXEyeScanEnable) || (print_type == EYESCAN_TYPE_TX && u1TXEyeScanEnable))
    {   U8 space_num;

        mcSHOW_DBG_MSG(("\n\n"));

        if (print_type == EYESCAN_TYPE_CBT )
        {
            u2DQDelayBegin = 0;
            u2DQDelayEnd = 64+20;

            space_num = 8 + one_pi_ps * 32 / 1000;
        }
        else if (print_type == EYESCAN_TYPE_RX)
        {
            u2DQDelayBegin = gEyeScan_Min[(finalVref) / EYESCAN_GRAPH_RX_VREF_STEP][0][0]-32;
            u2DQDelayEnd = gEyeScan_Max[(finalVref) / EYESCAN_GRAPH_RX_VREF_STEP][0][0]+32;

            space_num = 8 + one_pi_ps * 32 / 1000;
        }
        else// if (print_type==2)
        {
            if (gEyeScan_CaliDelay[0] < gEyeScan_CaliDelay[1])
                u2DQDelayBegin = gEyeScan_CaliDelay[0] - 24;
            else
                u2DQDelayBegin = gEyeScan_CaliDelay[1] - 24;

            u2DQDelayEnd = u2DQDelayBegin + 64;

            space_num = 15 + u2DQDelayEnd - u2DQDelayBegin + 2;
        }

        for (u1BitIdx = u1ByteIdx * DQS_BIT_NUMBER; u1BitIdx < (u1ByteIdx * DQS_BIT_NUMBER + cal_length); u1BitIdx++)
        {
            EyeScan_Index = 0;

            if (print_type == EYESCAN_TYPE_RX)
            {
                Min_Value_1UI_Line = gEyeScan_DelayCellPI[u1BitIdx] - 16;

                EYESCAN_LOG_Print_Prefix_String(); mcSHOW_EYESCAN_MSG(("RX EYESCAN Channel%d, Rank%d, DQ%d ===\n", p->channel, p->rank, u1BitIdx));
            }
            else
            {
#if EyeScan_Pic_draw_line_Mirror
                EyeScan_DelayCellPI_value = 0 - gEyeScan_DelayCellPI[u1BitIdx];
#else
                EyeScan_DelayCellPI_value = gEyeScan_DelayCellPI[u1BitIdx];
#endif
                Min_Value_1UI_Line = gEyeScan_CaliDelay[u1BitIdx / 8] - 16 - EyeScan_DelayCellPI_value;

                EYESCAN_LOG_Print_Prefix_String(); mcSHOW_EYESCAN_MSG(("%s EYESCAN Channel%d, Rank%d, Bit%d", print_type==EYESCAN_TYPE_CBT ? "CBT" : "TX DQ", p->channel, p->rank, u1BitIdx));
                if (print_type == EYESCAN_TYPE_TX)
                {
                    mcSHOW_EYESCAN_MSG(("(DRAM DQ%d)", uiLPDDR4_O1_Mapping_POP[p->channel][u1BitIdx]));
                }
                mcSHOW_EYESCAN_MSG((" ===\n"));
            }


#if VENDER_JV_LOG
            for (i = 0; i < 8 + one_pi_ps * 32 / 1000; i++) mcSHOW_EYESCAN_MSG((" "));
            mcSHOW_EYESCAN_MSG(("window\n"));
#else
            for (i = 0; i < space_num; i++) mcSHOW_EYESCAN_MSG((" "));
            mcSHOW_EYESCAN_MSG(("first last window\n"));
            mcSHOW_EYESCAN_MSG(("Vref Step: %d, Delay Step: %d\n",u2VrefStep, (print_type==EYESCAN_TYPE_RX)?4:1));

#endif

                if (print_type == EYESCAN_TYPE_RX||(print_type != EYESCAN_TYPE_RX&&p->dram_type==TYPE_LPDDR5)) u1VrefRange = VREF_RANGE_0;
                else u1VrefRange = VREF_RANGE_1;

                for (u1VrefIdx = ((scan_vref_number-1) - ((scan_vref_number-1)%u2VrefStep)); u1VrefIdx >= 0; u1VrefIdx -= u2VrefStep)
                {
                        EyeScan_Pic_draw_line(p, print_type, u1VrefRange, u1VrefIdx, u1BitIdx, u2DQDelayBegin, u2DQDelayEnd, finalVrefRange, finalVref, VdlVWHigh_Upper_Vcent_Range, VdlVWHigh_Upper_Vcent, VdlVWHigh_Lower_Vcent_Range, VdlVWHigh_Lower_Vcent, gEyeScan_CaliDelay[u1BitIdx / 8], gEyeScan_DelayCellPI[u1BitIdx], one_pi_ps, Min_Value_1UI_Line);

                        if (print_type != EYESCAN_TYPE_RX && u1VrefRange == VREF_RANGE_1 && u1VrefIdx == 20)
                        {
                            u1VrefRange = VREF_RANGE_0;
                            u1VrefIdx = VREF_VOLTAGE_TABLE_NUM_LP4-1;
                        }
                }

            mcSHOW_EYESCAN_MSG(("\n\n"));

        }
    }
}
}


#ifdef RELEASE
#undef mcSHOW_DBG_MSG
#define mcSHOW_DBG_MSG(_x_)
#endif
#endif

#if PIN_CHECK_TOOL
#define CA_THRESHOLD 20
#define RX_THRESHOLD 150
#define TX_THRESHOLD 20
#define PERCENTAGE_THRESHOLD 50
#define PRINT_WIN_SIZE 0
DEBUG_PIN_INF_FOR_FLASHTOOL_T PINInfo_flashtool;
static U8* print_Impedence_LOG_type(U8 print_type)
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

    mcSHOW_DBG_MSG3(("\n\n[Pin Info Summary] Freqency %d\n", p->frequency));

    for (u1FreqRegionIdx=0;u1FreqRegionIdx<2;u1FreqRegionIdx++)
    {
        for (u1ImpIdx=0;u1ImpIdx<IMP_DRV_MAX;u1ImpIdx++)
        {
            mcSHOW_DBG_MSG3(("IMP %s type:%s %s\n", u1FreqRegionIdx?"Region1":"Region0", print_Impedence_LOG_type(u1ImpIdx), ((PINInfo_flashtool.IMP_ERR_FLAG>>(u1FreqRegionIdx*4+u1ImpIdx)&0x1)?"ERROR":"PASS")));
        }
    }

    //for(ucFreqIdx=SRAM_SHU0; ucFreqIdx<DRAM_DFS_SRAM_MAX; ucFreqIdx++)
    {
        //mcSHOW_DBG_MSG(("==Freqency = %d==\n", get_FreqTbl_by_SRAMIndex(p,ucFreqIdx)->frequency));
        for(u1CHIdx=0; u1CHIdx<p->support_channel_num; u1CHIdx++)
        {
            for(u1RankIdx=0; u1RankIdx<p->support_rank_num; u1RankIdx++)
            {
                mcSHOW_DBG_MSG3(("CH %d, Rank %d\n", u1CHIdx, u1RankIdx));


                for (u1CAIdx =0; u1CAIdx <CATRAINING_NUM_LP4; u1CAIdx++)
                {
                    #if 1
                    PINInfo_flashtool.CA_WIN_SIZE[u1CHIdx][u1RankIdx][u1CAIdx]= (PINInfo_flashtool.CA_WIN_SIZE[u1CHIdx][u1RankIdx][u1CAIdx]* 100 + 63) /64;
                    if ((PINInfo_flashtool.CA_WIN_SIZE[u1CHIdx][u1RankIdx][u1CAIdx]==0)||(PINInfo_flashtool.CA_WIN_SIZE[u1CHIdx][u1RankIdx][u1CAIdx]<=PERCENTAGE_THRESHOLD))
                    #else
                    if ((PINInfo_flashtool.CA_WIN_SIZE[u1CHIdx][u1RankIdx][u1CAIdx]==0)||(PINInfo_flashtool.CA_WIN_SIZE[u1CHIdx][u1RankIdx][u1CAIdx]<=CA_THRESHOLD))
                    #endif
                    {
                        PINInfo_flashtool.CA_ERR_FLAG[u1CHIdx][u1RankIdx] |= (1<<u1CAIdx);
                        PINInfo_flashtool.TOTAL_ERR |= (0x1<<(u1CHIdx*4+u1RankIdx*2));
                    }
                    mcSHOW_DBG_MSG3(("CA %d: %s ", u1CAIdx, ((PINInfo_flashtool.CA_ERR_FLAG[u1CHIdx][u1RankIdx]>>u1CAIdx)&0x1)?"ERROR":"PASS"));
                    #if PRINT_WIN_SIZE
                    mcSHOW_DBG_MSG3(("(WIN_SIZE: %d %% )", (PINInfo_flashtool.CA_WIN_SIZE[u1CHIdx][u1RankIdx][u1CAIdx])));
                    #endif
                    mcSHOW_DBG_MSG3(("\n"));

                }


                for (u1BitIdx =0; u1BitIdx <DQ_DATA_WIDTH; u1BitIdx++)
                {
                    u1ByteIdx = (u1BitIdx>=8?1:0);
                    u1BitIdx_DQ = uiLPDDR4_O1_Mapping_POP[p->channel][u1BitIdx];
                    u1ByteIdx_DQ = (u1BitIdx_DQ>=8?1:0);


                #if 1
                    PINInfo_flashtool.DQ_RX_WIN_SIZE[u1CHIdx][u1RankIdx][u1BitIdx] = ((PINInfo_flashtool.DQ_RX_WIN_SIZE[u1CHIdx][u1RankIdx][u1BitIdx]* gHQALOG_RX_delay_cell_ps_075V * DDRPhyGetRealFreq(p)* 2)+ (1000000 - 1)) / 1000000;
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


                #if 1
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

                    //mcSHOW_DBG_MSG(("bit %d(DRAM DQ %d): %s ", u1BitIdx, uiLPDDR4_O1_Mapping_POP[p->channel][u1BitIdx], ((PINInfo_flashtool.DQ_ERR_FLAG[u1CHIdx][u1RankIdx][u1ByteIdx]>>u1BitIdx)&0x1)?"ERROR":"PASS"));
                    //mcSHOW_DBG_MSG(("(RX WIN SIZE: %d, TX WIN SIZE: %d)", PINInfo_flashtool.DQ_RX_WIN_SIZE[u1CHIdx][u1RankIdx][u1BitIdx], PINInfo_flashtool.DQ_TX_WIN_SIZE[u1CHIdx][u1RankIdx][u1BitIdx]));
                    //mcSHOW_DBG_MSG(("\n"));
                }
                for (u1BitIdx_DQ=0; u1BitIdx_DQ<DQ_DATA_WIDTH; u1BitIdx_DQ++)
                {
                    u1ByteIdx_DQ = (u1BitIdx_DQ>=8?1:0);
                    mcSHOW_DBG_MSG3(("DRAM DQ %d: RX %s, TX %s ", u1BitIdx_DQ, (((PINInfo_flashtool.DRAM_PIN_RX_ERR_FLAG[u1CHIdx][u1RankIdx][u1ByteIdx_DQ]>>(u1BitIdx_DQ-(u1ByteIdx_DQ==1?8:0)))&0x1)?"ERROR":"PASS"),\
                                                                                                                        (((PINInfo_flashtool.DRAM_PIN_TX_ERR_FLAG[u1CHIdx][u1RankIdx][u1ByteIdx_DQ]>>(u1BitIdx_DQ-(u1ByteIdx_DQ==1?8:0)))&0x1)?"ERROR":"PASS")));
                    #if PRINT_WIN_SIZE
                    mcSHOW_DBG_MSG3(("(RX WIN SIZE: %d %%, TX WIN SIZE: %d %% )", PINInfo_flashtool.DQ_RX_WIN_SIZE[u1CHIdx][u1RankIdx][uiLPDDR4_O1_Mapping_POP[u1CHIdx][u1BitIdx_DQ]], PINInfo_flashtool.DQ_TX_WIN_SIZE[u1CHIdx][u1RankIdx][uiLPDDR4_O1_Mapping_POP[u1CHIdx][u1BitIdx_DQ]]));
                    #endif
                    mcSHOW_DBG_MSG3(("\n"));
                }
            }
        }
    }
}

void vGetErrorTypeResult(DRAMC_CTX_T *p)
{
    U8 u1CHIdx, u1CHIdx_EMI, u1RankIdx, u1CAIdx, u1ByteIdx, u1BitIdx, u1FreqRegionIdx, u1ImpIdx;


    mcSHOW_DBG_MSG3(("\n[Get Pin Error Type Result]\n"));


    if (PINInfo_flashtool.TOTAL_ERR==0 && PINInfo_flashtool.IMP_ERR_FLAG==0)
    {
        mcSHOW_DBG_MSG3(("ALL PASS\n"));
    }


    if (PINInfo_flashtool.IMP_ERR_FLAG)
    {
        mcSHOW_DBG_MSG3(("[CHECK RESULT] FAIL: Impedance calibration fail\n"));
        mcSHOW_DBG_MSG3(("Suspect EXTR contact issue\n"));
        mcSHOW_DBG_MSG3(("Suspect EXTR related resistor contact issue\n"));
    }


    if ((PINInfo_flashtool.TOTAL_ERR == 0xffff) && (PINInfo_flashtool.WL_ERR_FLAG== 0xff))
    {
        mcSHOW_DBG_MSG3(("[CHECK RESULT] FAIL: ALL calibration fail\n"));
        mcSHOW_DBG_MSG3(("Suspect RESET_N contact issue\n"));
        mcSHOW_DBG_MSG3(("Suspect DRAM Power (VDD1/VDD2/VDDQ) contact issue\n"));
    }
    else
    {
        for (u1CHIdx = 0; u1CHIdx < p->support_channel_num; u1CHIdx++)
        {
            #if (CHANNEL_NUM > 2)
		    if (channel_num_auxadc > 2) {
            if(u1CHIdx == CHANNEL_B)
                u1CHIdx_EMI = CHANNEL_C;
            else if(u1CHIdx == CHANNEL_C)
                u1CHIdx_EMI = CHANNEL_B;
            else
		    	}
            #endif
                u1CHIdx_EMI = u1CHIdx;


            if ((PINInfo_flashtool.TOTAL_ERR>>(u1CHIdx*4) & 0xf) == 0xf)
            {
                mcSHOW_DBG_MSG3(("[CHECK RESULT] FAIL: CH%d all calibration fail\n",u1CHIdx));
                mcSHOW_DBG_MSG3(("Suspect EMI%d_CK_T contact issue\n",u1CHIdx_EMI));
                mcSHOW_DBG_MSG3(("Suspect EMI%d_CK_C contact issue\n",u1CHIdx_EMI));
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
                        mcSHOW_DBG_MSG3(("[CHECK RESULT] FAIL: CH%d RK%d all calibration fail\n",u1CHIdx,u1RankIdx));
                        mcSHOW_DBG_MSG3(("Suspect EMI%d_CKE_%d contact issue\n",u1CHIdx_EMI,u1RankIdx));
                        mcSHOW_DBG_MSG3(("Suspect EMI%d_CS_%d contact issue\n",u1CHIdx_EMI,u1RankIdx));
                    }
                    else
                    {
                        for (u1ByteIdx = 0; u1ByteIdx < DQS_BYTE_NUMBER; u1ByteIdx++)
                        {

                            if((PINInfo_flashtool.DRAM_PIN_RX_ERR_FLAG[u1CHIdx][u1RankIdx][u1ByteIdx] == 0xff) &&\
                                      (PINInfo_flashtool.DRAM_PIN_TX_ERR_FLAG[u1CHIdx][u1RankIdx][u1ByteIdx] == 0xff))
                            {
                                mcSHOW_DBG_MSG3(("[CHECK RESULT] FAIL: CH%d RK%d Byte%d WL/Read/Write calibration fail\n",u1CHIdx,u1RankIdx,u1ByteIdx));
                                mcSHOW_DBG_MSG3(("Suspect EMI%d_DQS%d_T contact issue\n",u1CHIdx_EMI,u1ByteIdx));
                                mcSHOW_DBG_MSG3(("Suspect EMI%d_DQS%d_C contact issue\n",u1CHIdx_EMI,u1ByteIdx));
                            }

                            else if (PINInfo_flashtool.DRAM_PIN_RX_ERR_FLAG[u1CHIdx][u1RankIdx][u1ByteIdx]&&\
                                       PINInfo_flashtool.DRAM_PIN_TX_ERR_FLAG[u1CHIdx][u1RankIdx][u1ByteIdx])
                            {
                                for (u1BitIdx = 0; u1BitIdx < DQS_BIT_NUMBER; u1BitIdx++)
                                {
                                    if (((PINInfo_flashtool.DRAM_PIN_RX_ERR_FLAG[u1CHIdx][u1RankIdx][u1ByteIdx]>>u1BitIdx)&0x1)&&\
                                         ((PINInfo_flashtool.DRAM_PIN_TX_ERR_FLAG[u1CHIdx][u1RankIdx][u1ByteIdx]>>u1BitIdx)&0x1))
                                    {
                                            mcSHOW_DBG_MSG3(("[CHECK RESULT] FAIL: CH%d RK%d DRAM DQ%d Read/Write fail\n",u1CHIdx,u1RankIdx,u1ByteIdx*8+u1BitIdx));
                                            mcSHOW_DBG_MSG3(("Suspect EMI%d_DQ%d contact issue\n",u1CHIdx_EMI,u1ByteIdx*8+u1BitIdx));
                                    }
                                }
                            }

                            else if((PINInfo_flashtool.DRAM_PIN_RX_ERR_FLAG[u1CHIdx][u1RankIdx][u1ByteIdx] == 0xff) ||\
                                      (PINInfo_flashtool.DRAM_PIN_TX_ERR_FLAG[u1CHIdx][u1RankIdx][u1ByteIdx] == 0xff))
                            {
                                mcSHOW_DBG_MSG3(("[CHECK RESULT] FAIL: CH%d RK%d Byte%d Suspect other special contact or calibration issue\n",u1CHIdx_EMI,u1RankIdx,u1ByteIdx));
                            }
                        }
                    }
                }
            }
        }
    }
    mcSHOW_DBG_MSG3(("\n"));
    return;
}
#endif


#if GATING_ONLY_FOR_DEBUG
void DramcGatingDebugRankSel(DRAMC_CTX_T *p, U8 u1Rank)
{
    if (p->support_rank_num == RANK_SINGLE)
    {
        u1Rank = 0;
    }

    vIO32WriteFldAlign_All(DDRPHY_MISC_CTRL1, u1Rank, MISC_CTRL1_R_DMSTBENCMP_RK_OPT);
}

void DramcGatingDebugInit(DRAMC_CTX_T *p)
{
    U32 backupReg0x64[CHANNEL_NUM], backupReg0xC8[CHANNEL_NUM], backupReg0xD0[CHANNEL_NUM];
    U32 backupRegShu_SCINTV[DRAM_DFS_SRAM_MAX][CHANNEL_NUM];
    U32 channel_idx, backup_channel, shu_index;

    backup_channel = vGetPHY2ChannelMapping(p);

    for (channel_idx = CHANNEL_A; channel_idx < p->support_channel_num; channel_idx++)
    {
        vSetPHY2ChannelMapping(p, channel_idx);
        backupReg0x64[channel_idx] = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_SPCMDCTRL));
        backupReg0xC8[channel_idx] = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_DQSOSCR));
        backupReg0xD0[channel_idx] = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_DUMMY_RD));
        for (shu_index = SRAM_SHU0; shu_index < DRAM_DFS_SRAM_MAX; shu_index++)
            backupRegShu_SCINTV[shu_index][channel_idx] = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_SHU_SCINTV) + SHU_GRP_DRAMC_OFFSET * shu_index);
    }
    vSetPHY2ChannelMapping(p, backup_channel);


    vIO32WriteFldAlign_All(DRAMC_REG_SPCMDCTRL, 0x1, SPCMDCTRL_REFRDIS);
    vIO32WriteFldAlign_All(DRAMC_REG_DQSOSCR, 0x1, DQSOSCR_DQSOSCRDIS);
    for (shu_index = SRAM_SHU0; shu_index < DRAM_DFS_SRAM_MAX; shu_index++)
        vIO32WriteFldAlign_All(DRAMC_REG_SHU_SCINTV + SHU_GRP_DRAMC_OFFSET * shu_index, 0x1, SHU_SCINTV_DQSOSCENDIS);
    vIO32WriteFldMulti_All(DRAMC_REG_DUMMY_RD, P_Fld(0x0, DUMMY_RD_DUMMY_RD_EN)
                                            | P_Fld(0x0, DUMMY_RD_SREF_DMYRD_EN)
                                            | P_Fld(0x0, DUMMY_RD_DQSG_DMYRD_EN)
                                            | P_Fld(0x0, DUMMY_RD_DMY_RD_DBG));
    mcDELAY_US(4);

    DramPhyReset(p);


    for (channel_idx = CHANNEL_A; channel_idx < p->support_channel_num; channel_idx++)
    {
        vSetPHY2ChannelMapping(p, channel_idx);
        vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_SPCMDCTRL), backupReg0x64[channel_idx]);
        vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_DQSOSCR), backupReg0xC8[channel_idx]);
        vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_DUMMY_RD), backupReg0xD0[channel_idx]);
        for (shu_index = SRAM_SHU0; shu_index < DRAM_DFS_SRAM_MAX; shu_index++)
            vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_SHU_SCINTV) + SHU_GRP_DRAMC_OFFSET * shu_index, backupRegShu_SCINTV[shu_index][channel_idx]);
    }
    vSetPHY2ChannelMapping(p, backup_channel);



    vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DRAMC_REG_SPCMD), 1, SPCMD_DQSGCNTEN);
    mcDELAY_US(4);

    vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DRAMC_REG_SPCMD), 1, SPCMD_DQSGCNTRST);
    mcDELAY_US(1);
    vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DRAMC_REG_SPCMD), 0, SPCMD_DQSGCNTRST);
    //mcSHOW_DBG_MSG(("DramcGatingDebugInit done\n" ));
}

void DramcGatingDebugExit(DRAMC_CTX_T *p)
{

    vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DRAMC_REG_SPCMD), 0, SPCMD_DQSGCNTEN);
    vIO32WriteFldAlign_All(DDRPHY_MISC_CTRL1, 0, MISC_CTRL1_R_DMSTBENCMP_RK_OPT);
}

static void DramcGatingDebug(DRAMC_CTX_T *p, U8 u1Channel)
{
    U32 LP3_DataPerByte[DQS_BYTE_NUMBER];
    U32 u4DebugCnt[DQS_BYTE_NUMBER];
    U16 u2DebugCntPerByte;

    U32 u4value, u4all_result_R, u4all_result_F, u4err_value;
    U8 backup_channel;

    backup_channel = p->channel;
    vSetPHY2ChannelMapping(p, u1Channel);

    mcDELAY_MS(10);

    LP3_DataPerByte[0] = (u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_MISC_STBERR_RK0_R), MISC_STBERR_RK0_R_STBERR_RK0_R));
    LP3_DataPerByte[2] = (LP3_DataPerByte[0] >> 8) & 0xff;
    LP3_DataPerByte[0] &= 0xff;

    u4all_result_R = LP3_DataPerByte[0] | (LP3_DataPerByte[2] << 8);


    LP3_DataPerByte[0] = (u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_MISC_STBERR_RK0_F), MISC_STBERR_RK0_F_STBERR_RK0_F));
    LP3_DataPerByte[2] = (LP3_DataPerByte[0] >> 8) & 0xff;
    LP3_DataPerByte[0] &= 0xff;

    u4all_result_F = LP3_DataPerByte[0] | (LP3_DataPerByte[2] << 8);


    u4DebugCnt[0] = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_DQSGNWCNT0));
    u4DebugCnt[1] = (u4DebugCnt[0] >> 16) & 0xffff;
    u4DebugCnt[0] &= 0xffff;

    u4DebugCnt[2] = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_DQSGNWCNT1));
    u4DebugCnt[3] = (u4DebugCnt[2] >> 16) & 0xffff;
    u4DebugCnt[2] &= 0xffff;

    mcSHOW_DBG_MSG2(("\n[GatingDebug] CH %d, DQS count (B3->B0) 0x%H, 0x%H, 0x%H, 0x%H \nError flag Rank0 (B3->B0) %B %B  %B %B  %B %B  %B %B\n", \
                                   u1Channel, u4DebugCnt[3], u4DebugCnt[2], u4DebugCnt[1], u4DebugCnt[0], \
                                   (u4all_result_F >> 24) & 0xff, (u4all_result_R >> 24) & 0xff, \
                                   (u4all_result_F >> 16) & 0xff, (u4all_result_R >> 16) & 0xff, \
                                   (u4all_result_F >> 8) & 0xff, (u4all_result_R >> 8) & 0xff, \
                                   (u4all_result_F) & 0xff, (u4all_result_R) & 0xff));

    if (p->support_rank_num == RANK_DUAL)
    {
        LP3_DataPerByte[0] = (u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_MISC_STBERR_RK1_R), MISC_STBERR_RK1_R_STBERR_RK1_R));
        LP3_DataPerByte[2] = (LP3_DataPerByte[0] >> 8) & 0xff;
        LP3_DataPerByte[0] &= 0xff;

        u4all_result_R = LP3_DataPerByte[0] | (LP3_DataPerByte[2] << 8);


        LP3_DataPerByte[0] = (u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_MISC_STBERR_RK1_F), MISC_STBERR_RK1_F_STBERR_RK1_F));
        LP3_DataPerByte[2] = (LP3_DataPerByte[0] >> 8) & 0xff;
        LP3_DataPerByte[0] &= 0xff;

        u4all_result_F = LP3_DataPerByte[0];

        mcSHOW_DBG_MSG2(("Error flag Rank1 (B3->B0) %B %B  %B %B  %B %B  %B %B\n", \
                                       (u4all_result_F >> 24) & 0xff, (u4all_result_R >> 24) & 0xff, \
                                       (u4all_result_F >> 16) & 0xff, (u4all_result_R >> 16) & 0xff, \
                                       (u4all_result_F >> 8) & 0xff, (u4all_result_R >> 8) & 0xff, \
                                       (u4all_result_F) & 0xff, (u4all_result_R) & 0xff));
    }
    else
    {
        mcSHOW_DBG_MSG2(("\n"));
    }

    //vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SPCMD), 1, SPCMD_DQSGCNTRST);
    //mcDELAY_US(1);//delay 2T
    //vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SPCMD), 0, SPCMD_DQSGCNTRST);

    vSetPHY2ChannelMapping(p, backup_channel);
}
#endif

#if (FOR_DV_SIMULATION_USED == 0 && SW_CHANGE_FOR_SIMULATION == 0)
#if (__ETT__ || CPU_RW_TEST_AFTER_K)
void DramcDumpDebugInfo(DRAMC_CTX_T *p)
{

U8 u1RefreshRate;
DRAM_CHANNEL_T channelIdx;


#ifdef TEMP_SENSOR_ENABLE
    for (channelIdx = CHANNEL_A; channelIdx < (int)p->support_channel_num; channelIdx++)
    {
        u1RefreshRate = u1GetMR4RefreshRate(p, channelIdx);
        mcSHOW_DBG_MSG2(("[CH%d] MRR(MR4) [10:8]=%x\n", channelIdx, u1RefreshRate));
    }
#endif

#if ENABLE_REFRESH_RATE_DEBOUNCE
    {
        U8 pre_refresh_rate[4];
        for (channelIdx = CHANNEL_A; channelIdx < (int)p->support_channel_num; channelIdx++)
        {
            vSetPHY2ChannelMapping(p, channelIdx);

            pre_refresh_rate[0] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_IRQ_INFO1A), DRAMC_IRQ_INFO1A_PRE_REFRESH_RATE_RK0_FOR_INT_X0);
            pre_refresh_rate[1] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_IRQ_INFO1A), DRAMC_IRQ_INFO1A_PRE_REFRESH_RATE_RK1_FOR_INT_X0);
            pre_refresh_rate[2] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_IRQ_INFO1A), DRAMC_IRQ_INFO1A_PRE_REFRESH_RATE_RK0_B1_FOR_INT_X0);
            pre_refresh_rate[3] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_IRQ_INFO1A), DRAMC_IRQ_INFO1A_PRE_REFRESH_RATE_RK1_B1_FOR_INT_X0);

            mcSHOW_DBG_MSG2(("\t[CH%d] RK0 B0 Int Status=0x%x\n", channelIdx, pre_refresh_rate[0]));
            mcSHOW_DBG_MSG2(("\t[CH%d] RK1 B0 Int Status=0x%x\n", channelIdx, pre_refresh_rate[1]));
            mcSHOW_DBG_MSG2(("\t[CH%d] RK0 B1 Int Status=0x%x\n", channelIdx, pre_refresh_rate[2]));
            mcSHOW_DBG_MSG2(("\t[CH%d] RK1 B1 Int Status=0x%x\n", channelIdx, pre_refresh_rate[3]));

            mcSHOW_DBG_MSG2(("\t[CH%d] MISC_STATUSA_REFRESH_RATE=%d\n", channelIdx, u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_MISC_STATUSA), MISC_STATUSA_REFRESH_RATE)));
        }
        vSetPHY2ChannelMapping(p, CHANNEL_A);
    }
#endif



#if __A60868_TO_BE_PORTING__
    U8 mpdivInSel, cali_shu_sel, mpdiv_shu_sel, u1RefreshRate;
    DRAM_CHANNEL_T channelIdx;

    mpdivInSel = u4IO32ReadFldAlign(DDRPHY_MISC_CTRL2, MISC_CTRL2_CLRPLL_SHU_GP);
    cali_shu_sel = u4IO32ReadFldAlign(DRAMC_REG_SHUCTRL, SHUCTRL_R_OTHER_SHU_GP);
    mpdiv_shu_sel = u4IO32ReadFldAlign(DRAMC_REG_SHUCTRL, SHUCTRL_R_MPDIV_SHU_GP);


    mcSHOW_DBG_MSG2(("\n\n[DumpDebugInfo]\n"
                    "\tmpdivInSel %d, cali_shu_sel %d, mpdiv_shu_sel %d\n",
                         mpdivInSel, cali_shu_sel, mpdiv_shu_sel));

    #if GATING_ONLY_FOR_DEBUG

    //DramcGatingDebugInit(p);
    for (channelIdx = CHANNEL_A; channelIdx < p->support_channel_num; channelIdx++)
    {
        DramcGatingDebug(p, channelIdx);
    }
    #endif

#if (FOR_DV_SIMULATION_USED == 0 && SW_CHANGE_FOR_SIMULATION == 0)

#ifdef HW_GATING
    for (channelIdx = CHANNEL_A; channelIdx < p->support_channel_num; channelIdx++)
        {
        DramcPrintHWGatingStatus(p, channelIdx);
        }
#endif

    DramcPrintRXFIFODebugStatus(p);

#if (ENABLE_RX_TRACKING && RX_DLY_TRACK_ONLY_FOR_DEBUG && defined(DUMMY_READ_FOR_TRACKING))
    DramcPrintRxDlyTrackDebugStatus(p);
#endif

#if ENABLE_RX_TRACKING
    for (channelIdx = CHANNEL_A; channelIdx < p->support_channel_num; channelIdx++)
        {
        DramcPrintRXDQDQSStatus(p, channelIdx);
        }
#endif

#ifdef IMPEDANCE_TRACKING_ENABLE
        if (u1IsLP4Family(p->dram_type))
        {
        for (channelIdx = CHANNEL_A; channelIdx < p->support_channel_num; channelIdx++)
        {
            DramcPrintIMPTrackingStatus(p, channelIdx);
        }

        }
#endif

#ifdef TEMP_SENSOR_ENABLE
    for (channelIdx = CHANNEL_A; channelIdx < p->support_channel_num; channelIdx++)
    {
        u1RefreshRate = u1GetMR4RefreshRate(p, channelIdx);
        mcSHOW_DBG_MSG2(("[CH%d] MRR(MR4) [10:8]=%x\n", channelIdx, u1RefreshRate));
    }
#endif
#endif//(FOR_DV_SIMULATION_USED==0 && SW_CHANGE_FOR_SIMULATION==0)
#endif
}
#endif
#endif

#if ((fcFOR_CHIP_ID == fcA60868) || (FOR_DV_SIMULATION_USED == 0 && SW_CHANGE_FOR_SIMULATION == 0))
#if __ETT__
#if DUMP_ALLSUH_RG
static void DramcRegDumpRange(DRAMC_CTX_T *p, U32 u4StartAddr, U32 u4EndAddr)
{
    U32 ii;
    for (ii = u4StartAddr; ii <= u4EndAddr; ii += 4)
    {
        mcSHOW_DBG_MSG(("Reg(0x%xh) Address 0x%X = 0x%X\n", (ii & 0xfff) >> 2, ii, u4Dram_Register_Read(p, DRAMC_REG_ADDR(ii))));
        mcDELAY_US(20000);
    }
}
 #if 0
#define DRAMC_NAO_DUMP_RANGE (DRAMC_REG_RK2_B23_STB_DBG_INFO_15 - DRAMC_NAO_BASE_ADDRESS)
#define DDRPHY_NAO_DUMP_RANGE (DDRPHY_MISC_MBIST_STATUS2 - DDRPHY_NAO_BASE_ADDR)
#define DRAMC_AO_NONSHU_DUMP_RANGE (DRAMC_REG_RK1_PRE_TDQSCK27 - DRAMC_AO_BASE_ADDRESS)
#define DRAMC_AO_SHU_BASE (DRAMC_REG_SHU_ACTIM0 - DRAMC_AO_BASE_ADDRESS)
#define DRAMC_AO_SHU_DUMP_RANGE (DRAMC_REG_SHURK1_DQS2DQ_CAL5 - DRAMC_REG_SHU_ACTIM0)
#define DDRPHY_AO_NONSHU_DUMP_RANGE (DDRPHY_RFU_0XB2C - DDRPHY_AO_BASE_ADDR)
#define DDRPHY_AO_SHU_BASE (DDRPHY_SHU_B0_DQ0 - DDRPHY_AO_BASE_ADDR)
#define DDRPHY_AO_SHU_DUMP_RANGE (DDRPHY_SHU_R2_CA_CMD9 - DDRPHY_SHU_B0_DQ0)
#endif
void DumpAoNonShuNoRkReg(DRAMC_CTX_T *p)
{
    mcSHOW_DBG_MSG(("\n\tCHA_DRAMC_AO_NONSHU_NORK_BASE\n"));
    DramcRegDumpRange(p, DRAMC_REG_DDRCOMMON0, DRAMC_REG_SA_RESERVE);
    DramcRegDumpRange(p, DRAMC_REG_NONSHU_RSV, DRAMC_REG_SCRAMBLE_CFGF);
    DramcRegDumpRange(p, DRAMC_REG_WDT_DBG_SIGNAL, DRAMC_REG_SELFREF_HWSAVE_FLAG);
    DramcRegDumpRange(p, DRAMC_REG_DRAMC_IRQ_STATUS1, DRAMC_REG_DRAMC_IRQ_INFO5);
    mcSHOW_DBG_MSG(("\n\tCHA_PHY_AO_NONSHU_NORK_BASE\n"));
    DramcRegDumpRange(p, DDRPHY_REG_PHYPLL0, DDRPHY_REG_CLRPLL0);
    DramcRegDumpRange(p, DDRPHY_REG_B0_LP_CTRL0, DDRPHY_REG_B0_PHY4);
    DramcRegDumpRange(p, DDRPHY_REG_B1_LP_CTRL0, DDRPHY_REG_B1_TX_CKE_CTRL);
    DramcRegDumpRange(p, DDRPHY_REG_CA_LP_CTRL0, DDRPHY_REG_CA_TX_ARDQ_CTRL);
    DramcRegDumpRange(p, DDRPHY_REG_MISC_STBCAL, DDRPHY_REG_MISC_BIST_LPBK_CTRL0);
    mcSHOW_DBG_MSG(("\n\tDDRPHY_MD32_BASE_ADDRESS\n"));
    DramcRegDumpRange(p, DDRPHY_MD32_REG_SSPM_CFGREG_SW_RSTN, DDRPHY_MD32_REG_SSPM_CFGREG_TBUFH);
    DramcRegDumpRange(p, DDRPHY_MD32_REG_SSPM_TIMER0_CON, DDRPHY_MD32_REG_SSPM_OS_TIMER_IRQ_ACK);
    DramcRegDumpRange(p, DDRPHY_MD32_REG_SSPM_INTC_IRQ_RAW_STA0, DDRPHY_MD32_REG_SSPM_INTC_UART_RX_IRQ);
    DramcRegDumpRange(p, DDRPHY_MD32_REG_SSPM_MCLK_DIV, DDRPHY_MD32_REG_SSPM_UART_CTRL);
    DramcRegDumpRange(p, DDRPHY_MD32_REG_SSPM_DMA_GLBSTA, DDRPHY_MD32_REG_SSPM_DMA4_LIMITER);
    DramcRegDumpRange(p, DDRPHY_MD32_REG_SSPM_UART_RBR_THR_DLL_ADDR, DDRPHY_MD32_REG_SSPM_UART_SPM_SEL);
    DramcRegDumpRange(p, DDRPHY_MD32_REG_SSPM_TWAM_CTRL, DDRPHY_MD32_REG_SSPM_LAST_IDLE_CNT15);
    DramcRegDumpRange(p, DDRPHY_MD32_REG_LPIF_FSM_CFG, DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_15_3);
    return;
}
void DumpAoNonShuRkReg(DRAMC_CTX_T *p)
{
    mcSHOW_DBG_MSG(("\n\tCHA_DRAMC_AO_NONSHU_RK_BASE\n"));
    DramcRegDumpRange(p, DRAMC_REG_RK_TEST2_A1, DRAMC_REG_RK_DQSOSC);
    mcSHOW_DBG_MSG(("\n\tCHA_PHY_AO_NONSHU_RK_BASE\n"));
    DramcRegDumpRange(p, DDRPHY_REG_RK_B0_RXDVS0, DDRPHY_REG_RK_B0_RXDVS4);
    DramcRegDumpRange(p, DDRPHY_REG_RK_B1_RXDVS0, DDRPHY_REG_RK_B1_RXDVS4);
    DramcRegDumpRange(p, DDRPHY_REG_RK_CA_RXDVS0, DDRPHY_REG_RK_CA_RXDVS4);
    return;
}
void DumpAoShuNoRkReg(DRAMC_CTX_T *p)
{
    mcSHOW_DBG_MSG(("\n\tCHA_DRAMC_AO_SHU_NORK_BASE\n"));
    DramcRegDumpRange(p, DRAMC_REG_SHU_MATYPE, DRAMC_REG_SHU_ACTIM7);
    mcSHOW_DBG_MSG(("\n\tCHA_PHY_AO_SHU_NORK_BASE\n"));
    DramcRegDumpRange(p, DDRPHY_REG_SHU_PHYPLL0, DDRPHY_REG_SHU_PLL2);
    DramcRegDumpRange(p, DDRPHY_REG_SHU_B0_DQ0, DDRPHY_REG_SHU_B0_DQSIEN_CFG);
    DramcRegDumpRange(p, DDRPHY_REG_SHU_B1_DQ0, DDRPHY_REG_SHU_B1_CKE_DLY_CTRL);
    DramcRegDumpRange(p, DDRPHY_REG_SHU_CA_CMD0, DDRPHY_REG_SHU_CA_DQ_DLY);
    DramcRegDumpRange(p, DDRPHY_REG_MISC_SHU_DRVING7, DDRPHY_REG_MISC_SHU_CG_CTRL0);
    DramcRegDumpRange(p, DDRPHY_REG_MISC_STBERR_ALL, DDRPHY_REG_MISC_DBG_DB_MESSAGE7);
    return;
}
void DumpAoShuRkReg(DRAMC_CTX_T *p)
{
    mcSHOW_DBG_MSG(("\n\tCHA_DRAMC_AO_SHU_RK_BASE\n"));
    DramcRegDumpRange(p, DRAMC_REG_SHURK_SELPH_DQ0, DRAMC_REG_SHURK_CKE_CTRL);
    mcSHOW_DBG_MSG(("\n\tCHA_PHY_AO_SHU_RK_BASE\n"));
    DramcRegDumpRange(p, DDRPHY_REG_SHU_R0_B0_TXDLY0, DDRPHY_REG_SHU_RK_B0_BIST_CTRL);
    DramcRegDumpRange(p, DDRPHY_REG_SHU_R0_B1_TXDLY0, DDRPHY_REG_SHU_RK_B1_CKE_DLY);
    DramcRegDumpRange(p, DDRPHY_REG_SHU_R0_CA_TXDLY0, DDRPHY_REG_SHU_RK_CA_DQ_DLY);
    DramcRegDumpRange(p, DDRPHY_REG_MISC_SHU_RK_DQSCTL, DDRPHY_REG_MISC_SHU_RK_DQSCAL);
    return;
}
void DumpNaoReg(DRAMC_CTX_T *p)
{
    mcSHOW_DBG_MSG(("\n\n\tCHA_DRAMC_NAO_BASE\n"));
    DramcRegDumpRange(p, DRAMC_REG_TESTMODE, DRAMC_REG_CBT_WLEV_ATK_RESULT13);
    DramcRegDumpRange(p, DRAMC_REG_HWMRR_PUSH2POP_CNT, DRAMC_REG_SREF_DLY_CNT);
    DramcRegDumpRange(p, DRAMC_REG_TX_ATK_SET0, DRAMC_REG_TX_ATK_DBG_BIT_STATUS4);
    DramcRegDumpRange(p, DRAMC_REG_LP5_PDX_PDE_MON, DRAMC_REG_LAT_COUNTER_BLOCK_ALE);
    DramcRegDumpRange(p, DRAMC_REG_DRAMC_LOOP_BAK_ADR, DRAMC_REG_DRAMC_LOOP_BAK_WDAT7);
    DramcRegDumpRange(p, DRAMC_REG_RK0_DQSOSC_STATUS, DRAMC_REG_RK0_DUMMY_RD_DATA7);
    DramcRegDumpRange(p, DRAMC_REG_RK0_PI_DQ_CAL, DRAMC_REG_RK0_PI_DQM_CAL);
    DramcRegDumpRange(p, DRAMC_REG_RK1_DQSOSC_STATUS, DRAMC_REG_RK1_DUMMY_RD_DATA7);
    DramcRegDumpRange(p, DRAMC_REG_RK1_PI_DQ_CAL, DRAMC_REG_RK1_PI_DQM_CAL);
    DramcRegDumpRange(p, DRAMC_REG_MR_BACKUP_00_RK0_FSP0, DRAMC_REG_MR_BACKUP_03_RK0_FSP2);
    DramcRegDumpRange(p, DRAMC_REG_MR_BACKUP_00_RK1_FSP0, DRAMC_REG_MR_BACKUP_03_RK1_FSP2);
    mcSHOW_DBG_MSG(("\n\tCHA_PHY_NAO_BASE\n"));
    DramcRegDumpRange(p, DDRPHY_REG_MISC_STA_EXTLB0, DDRPHY_REG_MISC_RDSEL_TRACK_DBG);
    DramcRegDumpRange(p, DDRPHY_REG_MISC_DQ_RXDLY_TRRO0, DDRPHY_REG_MISC_DQ_RXDLY_TRRO31);
    DramcRegDumpRange(p, DDRPHY_REG_MISC_CA_RXDLY_TRRO20, DDRPHY_REG_MISC_STA_TOGLB1);
    DramcRegDumpRange(p, DDRPHY_REG_MISC_STA_EXTLB_DBG0, DDRPHY_REG_MISC_STA_EXTLB5);
    DramcRegDumpRange(p, DDRPHY_REG_DEBUG_APHY_RX_CTL, DDRPHY_REG_DEBUG_RODT_CTL);
    DramcRegDumpRange(p, DDRPHY_REG_CAL_DQSG_CNT_B0, DDRPHY_REG_RX_AUTOK_STATUS20);
    DramcRegDumpRange(p, DDRPHY_REG_DQSIEN_AUTOK_B0_RK0_STATUS0, DDRPHY_REG_DQSIEN_AUTOK_CA_RK0_DBG_STATUS5);
    DramcRegDumpRange(p, DDRPHY_REG_DQSIEN_AUTOK_CA_RK1_STATUS0, DDRPHY_REG_MISC_DBG_DB_IMP_MESSAGE11);
    DramcRegDumpRange(p, DDRPHY_REG_MISC_DMA_SRAM_MBIST, DDRPHY_REG_MISC_STA_EXTLB9);
    return;
}
void DumpAllRkRG(DRAMC_CTX_T *p, U8 u1ShuType)
{
    U8 u1RankIdx, u1rk_backup=u1GetRank(p);
    if (u1ShuType == NONSHUFFLE_RG||u1ShuType == BOTH_SHU_NONSHU_RG)
    {
        mcSHOW_DBG_MSG(("\n[DUMPLOG] Channel=%d, Rank=NON, SHU=NON, FREQ=NON, AORG, start \n", vGetPHY2ChannelMapping(p)));
        DumpAoNonShuNoRkReg(p);
        mcSHOW_DBG_MSG(("\n[DUMPLOG] Channel=%d, Rank=NON, SHU=NON, FREQ=NON, AORG, end \n", vGetPHY2ChannelMapping(p)));
    }
    if (u1ShuType == SHUFFLE_RG||u1ShuType == BOTH_SHU_NONSHU_RG)
    {
        mcSHOW_DBG_MSG(("\n[DUMPLOG] Channel=%d, Rank=NON, SHU=SRAM%d, FREQ=%d, AORG, start \n", vGetPHY2ChannelMapping(p), vGet_Current_SRAMIdx(p), GetFreqBySel(p, vGet_PLL_FreqSel(p))));
        DumpAoShuNoRkReg(p);
        mcSHOW_DBG_MSG(("\n[DUMPLOG] Channel=%d, Rank=NON, SHU=SRAM%d, FREQ=%d, AORG, end \n", vGetPHY2ChannelMapping(p), vGet_Current_SRAMIdx(p), GetFreqBySel(p, vGet_PLL_FreqSel(p))));
    }
    for (u1RankIdx=RANK_0; u1RankIdx<p->support_rank_num; u1RankIdx++)
    {
        vSetRank(p, u1RankIdx);
        if (u1ShuType == NONSHUFFLE_RG||u1ShuType == BOTH_SHU_NONSHU_RG)
        {
            mcSHOW_DBG_MSG(("\n[DUMPLOG] Channel=%d, Rank=%d, SHU=NON, FREQ=NON, AORG, start \n", vGetPHY2ChannelMapping(p), u1GetRank(p)));
            DumpAoNonShuRkReg(p);
            mcSHOW_DBG_MSG(("\n[DUMPLOG] Channel=%d, Rank=%d, SHU=NON, FREQ=NON, AORG, end \n", vGetPHY2ChannelMapping(p), u1GetRank(p)));
        }
        if (u1ShuType == SHUFFLE_RG||u1ShuType == BOTH_SHU_NONSHU_RG)
        {
            mcSHOW_DBG_MSG(("\n[DUMPLOG] Channel=%d, Rank=%d, SHU=SRAM%d, FREQ=%d, AORG, start \n", vGetPHY2ChannelMapping(p), u1GetRank(p), vGet_Current_SRAMIdx(p), GetFreqBySel(p, vGet_PLL_FreqSel(p))));
            DumpAoShuRkReg(p);
            mcSHOW_DBG_MSG(("\n[DUMPLOG] Channel=%d, Rank=%d, SHU=SRAM%d, FREQ=%d, AORG, end \n", vGetPHY2ChannelMapping(p), u1GetRank(p), vGet_Current_SRAMIdx(p), GetFreqBySel(p, vGet_PLL_FreqSel(p))));
        }
    }
    vSetRank(p, u1rk_backup);
}
void DumpAllChAllShuAllRkRG(DRAMC_CTX_T *p)
{
    U8 u1ChannelIdx, u1Ch_backup=vGetPHY2ChannelMapping(p);
    U8 u1ShuffleIdx, u1Shu_backup=p->ShuRGAccessIdx;
    for(u1ChannelIdx=CHANNEL_A; u1ChannelIdx<p->support_channel_num; u1ChannelIdx++)
    {
        vSetPHY2ChannelMapping(p, u1ChannelIdx);
        DumpAllRkRG(p,NONSHUFFLE_RG);
        for(u1ShuffleIdx=DRAM_DFS_REG_SHU0; u1ShuffleIdx<DRAM_DFS_REG_MAX; u1ShuffleIdx++)
        {
            p->ShuRGAccessIdx = u1ShuffleIdx;
            DumpAllRkRG(p,SHUFFLE_RG);
        }
    }
    p->ShuRGAccessIdx=u1Shu_backup;
    vSetPHY2ChannelMapping(p, u1Ch_backup);
}
static void DumpShuRG(DRAMC_CTX_T *p)
{
    DRAM_DFS_FREQUENCY_TABLE_T *pFreqTable = p->pDFSTable;
    U8 u1ShuffleIdx;
    U32 u4DramcShuOffset = 0;
    U32 u4DDRPhyShuOffset = 0;
    U32 u4RegBackupAddress[] =
    {
        (DDRPHY_REG_MISC_SRAM_DMA0),
        (DDRPHY_REG_MISC_SRAM_DMA1),
        #if (CHANNEL_NUM>1)
        (DDRPHY_REG_MISC_SRAM_DMA0+SHIFT_TO_CHB_ADDR),
        (DDRPHY_REG_MISC_SRAM_DMA1+SHIFT_TO_CHB_ADDR),
        #endif
        #if (CHANNEL_NUM>2)
        (DDRPHY_REG_MISC_SRAM_DMA0+SHIFT_TO_CHC_ADDR),
        (DDRPHY_REG_MISC_SRAM_DMA1+SHIFT_TO_CHC_ADDR),
        (DDRPHY_REG_MISC_SRAM_DMA0+SHIFT_TO_CHD_ADDR),
        (DDRPHY_REG_MISC_SRAM_DMA1+SHIFT_TO_CHD_ADDR),
        #endif
    };
    DramcBackupRegisters(p, u4RegBackupAddress, sizeof(u4RegBackupAddress) / sizeof(U32));
    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SRAM_DMA0, 0x0, MISC_SRAM_DMA0_APB_SLV_SEL);
    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SRAM_DMA1, 0x1, MISC_SRAM_DMA1_R_APB_DMA_DBG_ACCESS);
    DumpAllRkRG(p,NONSHUFFLE_RG);
    for (u1ShuffleIdx = 0; u1ShuffleIdx <= DRAM_DFS_SRAM_MAX; u1ShuffleIdx++)
    {
        if (u1ShuffleIdx == DRAM_DFS_SRAM_MAX)
        {
            vSetDFSTable(p, pFreqTable);
            u4DramcShuOffset = 0;
            u4DDRPhyShuOffset = 0;
            DramcRestoreRegisters(p, u4RegBackupAddress, sizeof(u4RegBackupAddress) / sizeof(U32));
        }
        else
        {
            vSetDFSTable(p, get_FreqTbl_by_SRAMIndex(p, u1ShuffleIdx));
            vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SRAM_DMA0, 0x0, MISC_SRAM_DMA0_APB_SLV_SEL);
            vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SRAM_DMA1, u1ShuffleIdx, MISC_SRAM_DMA1_R_APB_DMA_DBG_LEVEL);
            vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SRAM_DMA0, 0x1, MISC_SRAM_DMA0_APB_SLV_SEL);
            p->ShuRGAccessIdx = DRAM_DFS_REG_SHU1;
            DumpAllRkRG(p,SHUFFLE_RG);
            p->ShuRGAccessIdx = DRAM_DFS_REG_SHU0;
        }
     }
}
void DumpAllChAllShuRG(DRAMC_CTX_T *p)
{
    U8 u1ChannelIdx, u1Channel_backup;
    u1Channel_backup = p->channel;
    for(u1ChannelIdx=CHANNEL_A; u1ChannelIdx<p->support_channel_num; u1ChannelIdx++)
    {
        vSetPHY2ChannelMapping(p, u1ChannelIdx);
        mcSHOW_DBG_MSG(("\n[DUMPLOG]AllChannelAllShu-start Channel=%d\n", p->channel));
        DumpShuRG(p);
    }
    vSetPHY2ChannelMapping(p, u1Channel_backup);
}
#endif
#if ETT_NO_DRAM
static void NoDramDramcRegDumpRange(U32 u4StartAddr, U32 u4EndAddr)
{
    U32 ii;
    for (ii = u4StartAddr; ii < u4EndAddr; ii += 4)
    {
        mcSHOW_DBG_MSG(("*(volatile unsigned int *)0x%X = 0x%X;\n", ii, (*(volatile unsigned int *)(ii))));
    }
}
DRAM_STATUS_T NoDramDramcRegDump(DRAMC_CTX_T *p)
{
    mcSHOW_DBG_MSG(("\n\n\tCHA_DRAMC_AO_BASE\n"));
    NoDramDramcRegDumpRange(Channel_A_DRAMC_AO_BASE_ADDRESS, Channel_A_DRAMC_AO_BASE_ADDRESS + 0x1E58);
    mcSHOW_DBG_MSG(("\n\tCHB_DRAMC_AO_BASE\n"));
    NoDramDramcRegDumpRange(Channel_B_DRAMC_AO_BASE_ADDRESS, Channel_B_DRAMC_AO_BASE_ADDRESS + 0x1E58);
    mcSHOW_DBG_MSG(("\n\tPHY_A_BASE\n"));
    NoDramDramcRegDumpRange(Channel_A_DDRPHY_BASE_ADDRESS, Channel_A_DDRPHY_BASE_ADDRESS + 0x1FC8);
    mcSHOW_DBG_MSG(("\n\tPHY_B_BASE\n"));
    NoDramDramcRegDumpRange(Channel_B_PHY_BASE_ADDRESS, Channel_B_PHY_BASE_ADDRESS + 0x1FC8);
    return DRAM_OK;
}
DRAM_STATUS_T NoDramRegFill(void)
{
}
#endif
#endif
#endif
#if DRAMC_MODEREG_CHECK
#if defined(RELEASE) && defined(DEVIATION)
#undef mcSHOW_JV_LOG_MSG
#define mcSHOW_JV_LOG_MSG(_x_)    opt_print _x_
#endif
void DramcModeReg_Check(DRAMC_CTX_T *p)
{
    U8 backup_channel, backup_rank;
    U8 u1ChannelIdx, u1RankIdx;
    U8 u1MRFsp;
    U8 ii, u1MR[] = {5, 12, 14, 4, 18, 19};
    U16 u2MRValue = 0, u2Value = 0;
    U8 u1match = 0;
    U8 backup_u1MR13Value[RANK_MAX] = {0};
#if MRW_CHECK_ONLY || MRW_BACKUP
    U8 FSPWR_FlagBak[RANK_MAX];
#endif

    mcSHOW_DBG_MSG2(("\n\n[DramcModeReg_Check] Freq_%d, FSP_%d\n", p->frequency, p->dram_fsp));

    backup_channel = vGetPHY2ChannelMapping(p);
    backup_rank = u1GetRank(p);

    #if MRW_CHECK_ONLY || MRW_BACKUP
    for (u1RankIdx = 0; u1RankIdx < (U32)(p->support_rank_num); u1RankIdx++)
    {
        FSPWR_FlagBak[u1RankIdx] = gFSPWR_Flag[u1RankIdx];
    }
    #endif

    if (u1IsLP4Family(p->dram_type))
    {
        #if VENDER_JV_LOG && defined(DEVIATION)
        mcSHOW_JV_LOG_MSG(("\n\n[DramcModeReg_Check] Freq_%d, FSP_%d\n",p->frequency,p->dram_fsp));
        #else
        mcSHOW_DBG_MSG2(("\n\n[DramcModeReg_Check] Freq_%d, FSP_%d\n",p->frequency,p->dram_fsp));
        #endif

        #if MRW_CHECK_ONLY
        mcSHOW_MRW_MSG(("\n==[MR Dump] %s==\n", __func__));
        #endif

        for (u1MRFsp = FSP_0; u1MRFsp < p->support_fsp_num; u1MRFsp++)
        {
            for (u1ChannelIdx = 0; u1ChannelIdx < (p->support_channel_num); u1ChannelIdx++)
            {
                vSetPHY2ChannelMapping(p, u1ChannelIdx);

                for (u1RankIdx = 0; u1RankIdx < (U32)(p->support_rank_num); u1RankIdx++)
                {
                #if MRW_CHECK_ONLY || MRW_BACKUP
                    if (u1MRFsp == FSP_0)
                    {
                        gFSPWR_Flag[u1RankIdx] = FSPWR_FlagBak[u1RankIdx];
                    }
                    else
                    {
                        gFSPWR_Flag[u1RankIdx] = u1MRFsp - 1;
                    }
                #endif

                    backup_u1MR13Value[u1RankIdx] = u1MR13Value[u1RankIdx];
                #if VENDER_JV_LOG && defined(DEVIATION)
                    mcSHOW_JV_LOG_MSG(("FSP_%d, CH_%d, RK%d\n",u1MRFsp,u1ChannelIdx,u1RankIdx));
                #else
                    mcSHOW_DBG_MSG2(("FSP_%d, CH_%d, RK%d\n",u1MRFsp,u1ChannelIdx,u1RankIdx));
                #endif

                    if (u1MRFsp == FSP_1)
                        u1MR13Value[u1RankIdx] |= 0x40;
                    else
                        u1MR13Value[u1RankIdx] &= (~0x40);

                    DramcModeRegWriteByRank(p, u1RankIdx, 13, u1MR13Value[u1RankIdx]);

                    for (ii = 0; ii < sizeof(u1MR); ii++)
                    {
                        DramcModeRegReadByRank(p, u1RankIdx, u1MR[ii], &u2Value);
                        u2Value &= 0xFF;

                        if ((u1MR[ii] == 12) || (u1MR[ii] == 14))
                        {
                            if (u1MR[ii] == 12)
                            {
                                u2MRValue = u1MR12Value[u1ChannelIdx][u1RankIdx][u1MRFsp];
                            }
                            else if (u1MR[ii] == 14)
                            {
                                u2MRValue = u1MR14Value[u1ChannelIdx][u1RankIdx][u1MRFsp];
                            }
                            u1match = (u2Value == u2MRValue)? 1: 0;

    #ifdef FOR_HQA_REPORT_USED
                            if (gHQALog_flag == 1)
                            {
                                HQA_Log_Message_for_Report(p, u1ChannelIdx, u1RankIdx, HQA_REPORT_FORMAT1, "DramcModeReg_Read_FSP_", u1MRFsp==0?"0_MR":"1_MR", u1MR[ii], u2Value, NULL);
                            }
                            else
    #endif
                            {
                                mcSHOW_DBG_MSG2(("\t\tMR%d = 0x%x (global = 0x%x)\t%smatch\n",u1MR[ii],u2Value,u2MRValue, (u1match==1)?"":"mis"));
                                #if VENDER_JV_LOG && defined(DEVIATION)
                                mcSHOW_JV_LOG_MSG(("[Vref Range = %d, Vref Value = %d]\n",(u2Value>>6)&1, u2Value&0x3f));
                                #else
                                mcSHOW_DBG_MSG2(("[Vref Range = %d, Vref Value = %d]\n",(u2Value>>6)&1, u2Value&0x3f));
                                #endif

                            }
                        }
                        else
                        {
    #ifdef FOR_HQA_REPORT_USED
                            if (gHQALog_flag == 1)
                            {
                                HQA_Log_Message_for_Report(p, u1ChannelIdx, u1RankIdx, HQA_REPORT_FORMAT1, "DramcModeReg_Read_FSP_", u1MRFsp==0?"0_MR":"1_MR", u1MR[ii], u2Value, NULL);
                            }
                            else
    #endif
                            {
                                const char *str_vender = "";
                                if (u1MR[ii] == 5)
                                {

                                    str_vender = (u2Value == 1)? "Samsung":(u2Value==0xff)?"Micron":(u2Value==0x5)?"Nanya":(u2Value==0x6)?"Hynix":"mismatch";
                                }
                                mcSHOW_DBG_MSG2(("\t\tMR%d = 0x%x %s\n", u1MR[ii], u2Value, str_vender));
                            }
                        }

                    }


                    u1MR13Value[u1RankIdx] = backup_u1MR13Value[u1RankIdx];
                }
            }
        }

    #if MRW_CHECK_ONLY || MRW_BACKUP
        for (u1RankIdx = 0; u1RankIdx < (U32)(p->support_rank_num); u1RankIdx++)
        {
            FSPWR_FlagBak[u1RankIdx] = gFSPWR_Flag[u1RankIdx];
        }
    #endif

        for (u1ChannelIdx = 0; u1ChannelIdx < (p->support_channel_num); u1ChannelIdx++)
        {
            vSetPHY2ChannelMapping(p, u1ChannelIdx);
            for (u1RankIdx = 0; u1RankIdx < (U32)(p->support_rank_num); u1RankIdx++)
            {
            #if MRW_CHECK_ONLY || MRW_BACKUP
                gFSPWR_Flag[u1RankIdx] = FSPWR_FlagBak[u1RankIdx];
            #endif

                DramcModeRegWriteByRank(p, u1RankIdx, 13, backup_u1MR13Value[u1RankIdx]);
            }
        }
    }

    vSetPHY2ChannelMapping(p, backup_channel);
    vSetRank(p, backup_rank);
    return;
}
#if defined(RELEASE) && defined(DEVIATION)
#undef mcSHOW_JV_LOG_MSG
#define mcSHOW_JV_LOG_MSG(_x_)
#endif
#endif

#if MRW_CHECK_ONLY
void vPrintFinalModeRegisterSetting(DRAMC_CTX_T * p)
{
    U8 u1CHIdx, u1RankIdx, u1FSPIdx, u1MRIdx;
    U16 u2MRValue;
    U8 u1Backup_Channel, u1Backup_Rank, u1Backup_Fsp;

    mcSHOW_MRW_MSG(("\n\n\n==vPrintFinalModeRegisterSetting====\n"));

    u1Backup_Channel = vGetPHY2ChannelMapping(p);
    u1Backup_Rank = u1GetRank (p);

    for (u1CHIdx = 0; u1CHIdx < p->support_channel_num; u1CHIdx++)
    {
        vSetPHY2ChannelMapping(p, u1CHIdx);
        for (u1RankIdx = 0; u1RankIdx < p->support_rank_num; u1RankIdx++)
        {
            vSetRank(p, u1RankIdx);
            u1Backup_Fsp = gFSPWR_Flag[p->rank];
            for (u1FSPIdx = 0; u1FSPIdx < p->support_fsp_num; u1FSPIdx++)
            {
                gFSPWR_Flag[p->rank]=u1FSPIdx;
                mcSHOW_MRW_MSG(("==[MR Dump] CH%d Rank%d Fsp%d ==\n", p->channel, p->rank, gFSPWR_Flag[p->rank]));
                for (u1MRIdx = 0; u1MRIdx < MR_NUM; u1MRIdx++)
                {
                    u2MRValue = u2MRRecord[u1CHIdx][u1RankIdx][u1FSPIdx][u1MRIdx];
                    if (u2MRValue != 0xffff)
                    {
                        mcSHOW_MRW_MSG(("[MR Dump] CH%d Rank%d Fsp%d MR%d =0x%x\n", p->channel, p->rank, gFSPWR_Flag[p->rank], u1MRIdx, u2MRValue));
                        #if MRW_BACKUP

                        {
                             if (u1MRIdx==13)
                                gFSPWR_Flag[p->rank]=u1Backup_Fsp;
                        }

                        DramcMRWriteBackup(p, u1MRIdx, u1RankIdx);

                        {
                             if (u1MRIdx==13)
                                gFSPWR_Flag[p->rank]=u1FSPIdx;
                        }
                        #endif
                    }
                }
                mcSHOW_MRW_MSG(("\n"));
            }
            gFSPWR_Flag[p->rank] = u1Backup_Fsp;
        }
    }
    vSetRank(p, u1Backup_Rank);
    vSetPHY2ChannelMapping(p, u1Backup_Channel);
}
#endif

#if defined(TEST_MODE_MRS) || SAMSUNG_TEST_MODE_MRS_FOR_PRELOADER
const U8 PME_Key[4][6] =
{
    { 2, 5, 5, 2, 4, 8},
    { 2, 5, 5, 2, 4, 8},
    { 2, 5, 5, 2, 4, 8},
    { 10, 13, 13, 10, 12, 13}
};


static U32 u4TestModeV0[2] = {};
static U32 u4TestModeV1[2] = {};
static U32 u4TestModeV2[2] = {};
static U32 u4TestModeV3[2] = {};
static U32 u4TestModeV4[2] = {};
static U32 u4TestModeV5[2] = {};
static U32 u4TestModeV6[2] = {};

static void BackupRGBeforeTestMode(DRAMC_CTX_T *p)
{
    DRAM_CHANNEL_T eOriChannel = p->channel;
    int i = 0;
     p->channel = CHANNEL_A;
    for (i = 0; i < 2; i++)
    {
        if (i == 0)
        {
            mcSHOW_DBG_MSG(("***CHA\n"));
        }
        else
        {
            mcSHOW_DBG_MSG(("***CHB\n"));
        }

        u4TestModeV0[i] = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL0));
        mcSHOW_DBG_MSG5(("DRAMC_REG_MRS[0x%x]\n", u4TestModeV0[i]));
        u4TestModeV1[i] = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL));
        mcSHOW_DBG_MSG5(("DRAMC_REG_DRAMC_PD_CTRL[0x%x]\n", u4TestModeV1[i]));
        u4TestModeV2[i] = u4IO32Read4B(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL));
        mcSHOW_DBG_MSG5(("DRAMC_REG_STBCAL[0x%x]\n", u4TestModeV2[i]));
        u4TestModeV3[i] = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_REFCTRL0));
        mcSHOW_DBG_MSG5(("DRAMC_REG_REFCTRL0[0x%x]\n", u4TestModeV3[i]));
        u4TestModeV4[i] = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_HMR4));
        mcSHOW_DBG_MSG5(("DRAMC_REG_SPCMDCTRL[0x%x]\n", u4TestModeV4[i]));
        u4TestModeV5[i] = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL));
        mcSHOW_DBG_MSG5(("DRAMC_REG_CKECTRL[0x%x]\n", u4TestModeV5[i]));
        u4TestModeV6[i] = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_SLP4_TESTMODE));
        mcSHOW_DBG_MSG5(("DRAMC_REG_SLP4_TESTMODE[0x%x]\n", u4TestModeV6[i]));
        p->channel = CHANNEL_B;
    }
    p->channel = eOriChannel;
    return;

}

static void RestoreRGBeforeTestMode(DRAMC_CTX_T *p)
{
    DRAM_CHANNEL_T eOriChannel = p->channel;
    int i = 0;
    p->channel = CHANNEL_A;
    for (i = 0; i < 2; i++)
    {
        vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL0), u4TestModeV0[i]);
        vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), u4TestModeV1[i]);
        vIO32Write4B(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL), u4TestModeV2[i]);
        vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_REFCTRL0), u4TestModeV3[i]);
        vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_HMR4), u4TestModeV4[i]);
        vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL), u4TestModeV5[i]);
        vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_SLP4_TESTMODE), u4TestModeV6[i]);

        p->channel = CHANNEL_B;
    }
    p->channel = eOriChannel;
    return;
}

void ProgramModeEnter(DRAMC_CTX_T *p)
{
    DRAM_CHANNEL_T eOriChannel = p->channel;
    U8 i = 0;
    U8 j = 0;
    U8 k = 0;
    U8 u1Rank = 0;

    BackupRGBeforeTestMode(p);
    p->channel = CHANNEL_A;
    mcSHOW_DBG_MSG3(("ProgramModeEnter [Begin]\n"));

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 6; j++)
        {
             mcSHOW_DBG_MSG3(("Key[%d] CA[%d] => [%d]\n", i, j, PME_Key[i][j]));
        }
        mcSHOW_DBG_MSG3(("\n"));
    }

    for (i = 0; i < p->support_channel_num; i++)
    {
        if (i == 0)
        {
            mcSHOW_DBG_MSG3(("***CHA\n"));
        }
        else
        {
            mcSHOW_DBG_MSG3(("***CHB\n"));
        }
        for (j = 0; j < p->support_rank_num; j++)
        {
            if (j == 0)
            {
                u1Rank = 0;
                mcSHOW_DBG_MSG3(("***R0\n"));
            }
            else
            {
                u1Rank = 1;
                mcSHOW_DBG_MSG3(("***R1\n"));
            }
            vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL0), u1Rank, SWCMD_CTRL0_MRSRK);


            vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), 0, DRAMC_PD_CTRL_DCMEN);
            vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), 0, DRAMC_PD_CTRL_PHYCLKDYNGEN);
            vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL), 0, MISC_STBCAL_DQSIENCG_NORMAL_EN);


            vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_REFCTRL0), 1, REFCTRL0_REFDIS);
            vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_HMR4), 1, HMR4_REFRDIS);
            vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL), 1, CKECTRL_CKEFIXON);

            for (k = 0; k < 4; k++)
            {
                vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SLP4_TESTMODE), PME_Key[k][0], SLP4_TESTMODE_CA0_TEST);
                vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SLP4_TESTMODE), PME_Key[k][1], SLP4_TESTMODE_CA1_TEST);
                vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SLP4_TESTMODE), PME_Key[k][2], SLP4_TESTMODE_CA2_TEST);
                vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SLP4_TESTMODE), PME_Key[k][3], SLP4_TESTMODE_CA3_TEST);
                vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SLP4_TESTMODE), PME_Key[k][4], SLP4_TESTMODE_CA4_TEST);
                vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SLP4_TESTMODE), PME_Key[k][5], SLP4_TESTMODE_CA5_TEST);

                vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), 1, SWCMD_EN_STESTEN);
                while (u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SPCMDRESP), SPCMDRESP_STEST_RESPONSE))
                {
                    mcSHOW_DBG_MSG3(("status[%d]: %d\n", k, u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SPCMDRESP), SPCMDRESP_STEST_RESPONSE)));
                    break;
                }
                vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), 0, SWCMD_EN_STESTEN);
            }
        }
        p->channel = CHANNEL_B;
    }
    mcSHOW_DBG_MSG3(("ProgramModeEnter [Stop]\n"));
    p->channel = eOriChannel;
    RestoreRGBeforeTestMode(p);
    return;
}

void ProgramCodeInput(DRAMC_CTX_T *p, U16 u2A_value, U16 u2B_value, U16 u2C_value)
{
    DRAM_CHANNEL_T eOriChannel = p->channel;
    U8 i = 0;
    U8 j = 0;
    U8 k = 0;
    int keyNumber = 0;
    U16 u2Value = 0;
    U8 u1Rank = 0;

    BackupRGBeforeTestMode(p);
    p->channel = CHANNEL_A;

    U8 PCI_Key[5][6] =
    {
        { 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0},
        { 2, 5, 1, 2, 0, 0},
        { 2, 5, 1, 2, 0, 0}
    };

    mcSHOW_DBG_MSG3(("ProgramCodeInput [Begin]\n"));

    for (keyNumber = 0; keyNumber < 3; keyNumber++)
    {
        if (keyNumber == 0)
            u2Value = u2A_value;
        else if (keyNumber == 1)
            u2Value = u2B_value;
        else
            u2Value = u2C_value;

        PCI_Key[keyNumber][0] = (u2Value == 0)? (10): (2);
        PCI_Key[keyNumber][1] = (u2Value == 1)? (13): (5);
        PCI_Key[keyNumber][2] = (u2Value == 10)? (5): ((u2Value == 2)? (9): (1));
        PCI_Key[keyNumber][3] = (u2Value == 9)? (6): ((u2Value == 3)? (10): (2));
        PCI_Key[keyNumber][4] = (u2Value == 8)? (4): ((u2Value == 4)? (8): (0));
        PCI_Key[keyNumber][5] = (u2Value == 7)? (1): ((u2Value == 6)? (4): ((u2Value == 5)? (8): (0)));
    }

    for (i = 0; i < 5; i++)
    {
        for (j = 0; j < 6; j++)
        {
             mcSHOW_DBG_MSG3(("Key[%d] CA[%d] => [%d]\n", i, j, PCI_Key[i][j]));
        }
        mcSHOW_DBG_MSG3(("\n"));
    }

    for (i = 0; i < p->support_channel_num; i++)
    {
        if (i == 0)
        {
            mcSHOW_DBG_MSG3(("***CHA\n"));
        }
        else
        {
            mcSHOW_DBG_MSG3(("***CHB\n"));
        }
        for (j = 0; j < p->support_rank_num; j++)
        {
            if (j == 0)
            {
                u1Rank = 0;
                mcSHOW_DBG_MSG3(("***R0\n"));
            }
            else
            {
                u1Rank = 1;
                mcSHOW_DBG_MSG3(("***R1\n"));
            }
            vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL0), u1Rank, SWCMD_CTRL0_MRSRK);


            vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), 0, DRAMC_PD_CTRL_DCMEN);
            vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), 0, DRAMC_PD_CTRL_PHYCLKDYNGEN);
            vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL), 0, MISC_STBCAL_DQSIENCG_NORMAL_EN);


            vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_REFCTRL0), 1, REFCTRL0_REFDIS);
            vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_HMR4), 1, HMR4_REFRDIS);
            vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL), 1, CKECTRL_CKEFIXON);

            for (k = 0; k < 5; k++)
            {
                vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SLP4_TESTMODE), PCI_Key[k][0], SLP4_TESTMODE_CA0_TEST);
                vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SLP4_TESTMODE), PCI_Key[k][1], SLP4_TESTMODE_CA1_TEST);
                vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SLP4_TESTMODE), PCI_Key[k][2], SLP4_TESTMODE_CA2_TEST);
                vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SLP4_TESTMODE), PCI_Key[k][3], SLP4_TESTMODE_CA3_TEST);
                vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SLP4_TESTMODE), PCI_Key[k][4], SLP4_TESTMODE_CA4_TEST);
                vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SLP4_TESTMODE), PCI_Key[k][5], SLP4_TESTMODE_CA5_TEST);

                vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), 1, SWCMD_EN_STESTEN);
                while (u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SPCMDRESP), SPCMDRESP_STEST_RESPONSE))
                {
                    mcSHOW_DBG_MSG3(("status[%d]: %d\n", k, u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SPCMDRESP), SPCMDRESP_STEST_RESPONSE)));
                    break;
                }
                vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), 0, SWCMD_EN_STESTEN);
            }
        }
        p->channel = CHANNEL_B;
    }
    mcSHOW_DBG_MSG3(("ProgramCodeInput [Stop]\n"));
    p->channel = eOriChannel;
    RestoreRGBeforeTestMode(p);
    return;
}

void vApplyProgramSequence(DRAMC_CTX_T *p)
{
    #if SAMSUNG_TEST_MODE_MRS_FOR_PRELOADER == 1

    ProgramModeEnter(p);
    ProgramCodeInput(p, 0, 0, 0);
    ProgramCodeInput(p, 3, 9, 0);
    ProgramCodeInput(p, 1, 2, 0);
    ProgramCodeInput(p, 8, 0xa, 7);
    ProgramCodeInput(p, 2, 5, 8);
    mcSHOW_DBG_MSG3(("Buffer sensitivity decrease1: TMRS enter -> 000 -> 390 -> 120 -> 8A7 -> 258\n"));

    #elif SAMSUNG_TEST_MODE_MRS_FOR_PRELOADER == 2

    ProgramModeEnter(p);
    ProgramCodeInput(p, 0, 0, 0);
    ProgramCodeInput(p, 3, 9, 0);
    ProgramCodeInput(p, 1, 2, 0);
    ProgramCodeInput(p, 8, 0, 3);
    ProgramCodeInput(p, 2, 5, 8);
    mcSHOW_DBG_MSG3(("Buffer sensitivity decrease1: TMRS enter -> 000 -> 390 -> 120 -> 803 -> 258\n"));

    #elif SAMSUNG_TEST_MODE_MRS_FOR_PRELOADER == 3

    ProgramModeEnter(p);
    ProgramCodeInput(p, 0, 0, 0);
    ProgramCodeInput(p, 3, 9, 0);
    ProgramCodeInput(p, 1, 2, 0);
    ProgramCodeInput(p, 0, 1, 4);
    ProgramCodeInput(p, 8, 6, 3);
    ProgramCodeInput(p, 2, 5, 8);
    mcSHOW_DBG_MSG3(("2014 + 2863: test 4.TMRS enter -> 000 -> 390 -> 120 -> 014 -> 863 -> 258\n"));

    #elif SAMSUNG_TEST_MODE_MRS_FOR_PRELOADER == 4

    ProgramModeEnter(p);
    ProgramCodeInput(p, 0, 0, 0);
    ProgramCodeInput(p, 3, 9, 0);
    ProgramCodeInput(p, 1, 2, 0);
    ProgramCodeInput(p, 5, 2, 0xa);
    ProgramCodeInput(p, 2, 5, 8);
    mcSHOW_DBG_MSG3(("252A: test 5. TMRS enter -> 000 -> 390 -> 120 -> 52A -> 258\n"));

    #elif SAMSUNG_TEST_MODE_MRS_FOR_PRELOADER == 5
    mcSHOW_DBG_MSG3(("Trigger Samsung Test mode after 5 sec\n"));
    mcDELAY_MS(5000);
    while(1)
    {
        ProgramModeEnter(p);
    }

    #else

    ProgramModeEnter(p);
    ProgramCodeInput(p, 0, 0, 0);
    ProgramCodeInput(p, 3, 9, 0);
    ProgramCodeInput(p, 0, 2, 1);
    ProgramCodeInput(p, 2, 5, 8);
    mcSHOW_DBG_MSG3(("Buffer sensitivity decrease1: TMRS enter -> 000 -> 390 -> 021\n"));
    #endif
}
#endif

#if 0
void Hynix_Test_Mode(DRAMC_CTX_T *p)
{
#if MRW_CHECK_ONLY
    mcSHOW_MRW_MSG(("\n==[MR Dump] %s==\n", __func__));
#endif

    DramcModeRegWriteByRank(p, u1GetRank(p), 9, 0xb8);
    DramcModeRegWriteByRank(p, u1GetRank(p), 9, 0xe8);
    DramcModeRegWriteByRank(p, u1GetRank(p), 9, 0x98);
    DramcModeRegWriteByRank(p, u1GetRank(p), 9, 0xbf);
    DramcModeRegWriteByRank(p, u1GetRank(p), 9, 0xef);
    DramcModeRegWriteByRank(p, u1GetRank(p), 9, 0x9f);
    DramcModeRegWriteByRank(p, u1GetRank(p), 9, 0xb9);
    DramcModeRegWriteByRank(p, u1GetRank(p), 9, 0xe9);
    DramcModeRegWriteByRank(p, u1GetRank(p), 9, 0x99);
    DramcModeRegWriteByRank(p, u1GetRank(p), 9, 0xd8);
    DramcModeRegWriteByRank(p, u1GetRank(p), 9, 0x88);
    DramcModeRegWriteByRank(p, u1GetRank(p), 9, 0xa3);
    DramcModeRegWriteByRank(p, u1GetRank(p), 9, 0xe0);
}
#endif

#if RUNTIME_SHMOO_RELEATED_FUNCTION && SUPPORT_SAVE_TIME_FOR_CALIBRATION
void DramcRunTimeShmooRG_BackupRestore(DRAMC_CTX_T *p)
{
    U8 channel_idx, channel_backup;
    U32 ii, u4RgBackupNum, *u4RG_Backup = 0;

    channel_backup = vGetPHY2ChannelMapping(p);

    for (channel_idx = CHANNEL_A; channel_idx < p->support_channel_num; channel_idx++)
    {
        vSetPHY2ChannelMapping(p, channel_idx);

        U32 u4RegBackupAddress[] =
        {

            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ5)),
            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ5)),

            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_RXDLY0)),
            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_RXDLY1)),
            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_RXDLY2)),
            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_RXDLY3)),
            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_RXDLY4)),
            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_RXDLY5)),
            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_RXDLY0)),
            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_RXDLY1)),
            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_RXDLY2)),
            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_RXDLY3)),
            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_RXDLY4)),
            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_RXDLY5)),

            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_RXDLY0+DDRPHY_AO_RANK_OFFSET)),
            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_RXDLY1+DDRPHY_AO_RANK_OFFSET)),
            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_RXDLY2+DDRPHY_AO_RANK_OFFSET)),
            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_RXDLY3+DDRPHY_AO_RANK_OFFSET)),
            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_RXDLY4+DDRPHY_AO_RANK_OFFSET)),
            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_RXDLY5+DDRPHY_AO_RANK_OFFSET)),
            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_RXDLY0+DDRPHY_AO_RANK_OFFSET)),
            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_RXDLY1+DDRPHY_AO_RANK_OFFSET)),
            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_RXDLY2+DDRPHY_AO_RANK_OFFSET)),
            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_RXDLY3+DDRPHY_AO_RANK_OFFSET)),
            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_RXDLY4+DDRPHY_AO_RANK_OFFSET)),
            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_RXDLY5+DDRPHY_AO_RANK_OFFSET)),




            (DRAMC_REG_ADDR(DRAMC_REG_SHURK_PI)),
            (DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ0)),
            (DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ1)),
            (DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ2)),
            (DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ3)),
            (DRAMC_REG_ADDR(DRAMC_REG_SHURK_DQS2DQ_CAL1)),
            (DRAMC_REG_ADDR(DRAMC_REG_SHURK_DQS2DQ_CAL2)),
            (DRAMC_REG_ADDR(DRAMC_REG_SHURK_DQS2DQ_CAL5)),

            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_TXDLY0)),
            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_TXDLY1)),
            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_TXDLY0)),
            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_TXDLY1)),
            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_TXDLY3)),
            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_TXDLY3)),
            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_DQ0)),
            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_DQ0)),



            (DRAMC_REG_ADDR(DRAMC_REG_SHURK_PI+DRAMC_REG_AO_RANK_OFFSET )),
            (DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ0+DRAMC_REG_AO_RANK_OFFSET)),
            (DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ1+DRAMC_REG_AO_RANK_OFFSET)),
            (DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ2+DRAMC_REG_AO_RANK_OFFSET)),
            (DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ3+DRAMC_REG_AO_RANK_OFFSET)),
            (DRAMC_REG_ADDR(DRAMC_REG_SHURK_DQS2DQ_CAL1+DRAMC_REG_AO_RANK_OFFSET)),
            (DRAMC_REG_ADDR(DRAMC_REG_SHURK_DQS2DQ_CAL2+DRAMC_REG_AO_RANK_OFFSET)),
            (DRAMC_REG_ADDR(DRAMC_REG_SHURK_DQS2DQ_CAL5+DRAMC_REG_AO_RANK_OFFSET)),

            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_TXDLY0+DDRPHY_AO_RANK_OFFSET)),
            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_TXDLY1+DDRPHY_AO_RANK_OFFSET)),
            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_TXDLY0+DDRPHY_AO_RANK_OFFSET)),
            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_TXDLY1+DDRPHY_AO_RANK_OFFSET)),
            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_TXDLY3+DDRPHY_AO_RANK_OFFSET)),
            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_TXDLY3+DDRPHY_AO_RANK_OFFSET)),
            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_DQ0+DDRPHY_AO_RANK_OFFSET)),
            (DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_DQ0+DDRPHY_AO_RANK_OFFSET)),
            //(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_CA_CMD0+DDRPHY_AO_RANK_OFFSET)),
        };
        u4RgBackupNum = sizeof(u4RegBackupAddress) >> 2;
        u4RG_Backup = (U32*)(p->pSavetimeData->u4RG_Backup[channel_idx]);

        for (ii = 0; ii < u4RgBackupNum; ii++)
        {
            if (ii >= RUNTIME_SHMOO_RG_BACKUP_NUM)
            {
                mcSHOW_DBG_MSG(("\nRuntime Shmoo Backup/Restore buffer(%d) empty!!\n", RUNTIME_SHMOO_RG_BACKUP_NUM));
                while (1);
            }
            if (u1IsLP4Family(p->dram_type) && (p->femmc_Ready == 0))
            {
                *(U32*)(u4RG_Backup + ii) = u4IO32Read4B(u4RegBackupAddress[ii]);
                mcSHOW_DBG_MSG(("=== run time shmoo backup : *(U32*)(0x%x) = 0x%x\n", u4RegBackupAddress[ii], *(U32*)(u4RG_Backup + ii)));
            }
            else
            {
                vIO32Write4B(u4RegBackupAddress[ii], *(U32*)(u4RG_Backup + ii));
                mcSHOW_DBG_MSG(("=== run time shmoo restore: *(U32*)(0x%x) = 0x%x\n", u4RegBackupAddress[ii], *(U32*)(u4RG_Backup + ii)));
            }
        }
    }
    vSetPHY2ChannelMapping(p, channel_backup);

    //DramcRegDump(p);
}
#endif

#if VERIFY_CKE_PWR_DOWN_FLOW
void CKEFixOnOff_dbg(DRAMC_CTX_T *p, U8 u1RankIdx, CKE_FIX_OPTION option, CHANNEL_RANK_SEL_T WriteChannelNUM)
{
    U8 u1CKEOn, u1CKEOff, u1setChannel, u1BackupChannel;

    if (option == CKE_DYNAMIC)
    {
        u1CKEOn = u1CKEOff = 0;
    }
    else
    {
        u1CKEOn = option;
        u1CKEOff = (1 - option);
    }

    if (WriteChannelNUM == TO_ALL_CHANNEL)
    {
        if((u1RankIdx == RANK_0)||(u1RankIdx == TO_ALL_RANK))
        {
            vIO32WriteFldMulti_All(DRAMC_REG_DCM_CTRL0, P_Fld(u1CKEOff, DCM_CTRL0_DBG_CKEFIXOFF)
                                                    | P_Fld(u1CKEOn, DCM_CTRL0_DBG_CKEFIXON));
        }

        if(u1RankIdx == RANK_1||((u1RankIdx == TO_ALL_RANK) && (p->support_rank_num == RANK_DUAL)))
        {
            vIO32WriteFldMulti_All(DRAMC_REG_DCM_CTRL0, P_Fld(u1CKEOff, DCM_CTRL0_DBG_CKE1FIXOFF)
                                                    | P_Fld(u1CKEOn, DCM_CTRL0_DBG_CKE1FIXON));
        }
    }
    else
    {
        if((u1RankIdx == RANK_0) || (u1RankIdx == TO_ALL_RANK))
        {
            vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_DCM_CTRL0), P_Fld(u1CKEOff, DCM_CTRL0_DBG_CKEFIXOFF)
                                                                | P_Fld(u1CKEOn, DCM_CTRL0_DBG_CKEFIXON));
        }

        if((u1RankIdx == RANK_1) ||((u1RankIdx == TO_ALL_RANK) && (p->support_rank_num == RANK_DUAL)))
        {
            vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_DCM_CTRL0), P_Fld(u1CKEOff, DCM_CTRL0_DBG_CKE1FIXOFF)
                                                                | P_Fld(u1CKEOn, DCM_CTRL0_DBG_CKE1FIXON));
        }
    }
}

void DramcModeRegWrite_DcmOff(DRAMC_CTX_T *p, U8 u1MRIdx, U8 u1Value)
{
    U32 u4register_dcm;
    mcSHOW_DBG_MSG3(("### Write_lewis ###\n"));

    u4register_dcm = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL));

    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), 1, DRAMC_PD_CTRL_MIOCKCTRLOFF);
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), 0, DRAMC_PD_CTRL_DCMEN2);
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), 0, DRAMC_PD_CTRL_PHYCLKDYNGEN);

    DramcModeRegWrite_111(p, u1MRIdx, u1Value);

    vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), u4register_dcm);
}
#endif


#if 0
void Get_TA2_ST(DRAMC_CTX_T *p)
{
    U32 u4Value = 0;

    u4Value = u4IO32Read4B(DRAMC_REG_TESTRPT);
    mcSHOW_DBG_MSG(("DRAMC_REG_TESTRPT: 0x%x \n", u4Value));
}

void Get_TA2_ErrCnt(DRAMC_CTX_T *p)
{
    U32 u4Value = 0xffffffff;
    DRAM_CHANNEL_T channelIdx;
    DRAM_CHANNEL_T bkchannel = p->channel;

    for(channelIdx=CHANNEL_A; channelIdx<(p->support_channel_num); channelIdx++)
    {
        vSetPHY2ChannelMapping(p, channelIdx);
        mcSHOW_DBG_MSG(("CH[%d]\n", channelIdx));

        u4Value = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_TEST_RF_ERROR_FLAG0));
        mcSHOW_DBG_MSG(("flag 148:0x%x ", u4Value));

        u4Value = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_TEST_RF_ERROR_FLAG1));
        mcSHOW_DBG_MSG(("flag 14C:0x%x\n", u4Value));

        {

            //vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A3), iByte, TEST2_A3_ERRFLAG_BYTE_SEL);


            u4Value = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_TEST_RF_ERROR_CNT1));
            mcSHOW_DBG_MSG(("150:0x%x ", u4Value));

            u4Value = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_TEST_RF_ERROR_CNT2));
            mcSHOW_DBG_MSG(("154:0x%x ", u4Value));

            u4Value = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_TEST_RF_ERROR_CNT3));
            mcSHOW_DBG_MSG(("158:0x%x ", u4Value));

            u4Value = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_TEST_RF_ERROR_CNT4));
            mcSHOW_DBG_MSG(("15C:0x%x ", u4Value));

            u4Value = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_TEST_RF_ERROR_CNT5));
            mcSHOW_DBG_MSG(("160:0x%x ", u4Value));

            u4Value = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_TEST_RF_ERROR_CNT6));
            mcSHOW_DBG_MSG(("164:0x%x ", u4Value));

            u4Value = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_TEST_RF_ERROR_CNT7));
            mcSHOW_DBG_MSG(("168:0x%x ", u4Value));

            u4Value = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_TEST_RF_ERROR_CNT8));
            mcSHOW_DBG_MSG(("16C:0x%x\n", u4Value));
        }
    }
    vSetPHY2ChannelMapping(p, bkchannel);
}

void Modify_TX_Delay_Cell(DRAMC_CTX_T *p, int i)
{
    U16 u2DelayCellOfst[16] = {0};
    U32 u4value = (i<8) ? 0 : 1;

    if(i<16)
    {
        u2DelayCellOfst[i] = 0xffff;
    }
    else
    {
        mcSHOW_DBG_MSG(("!Error input, hang....\n"));
        while(1);
    }


    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A3), u4value, TEST2_A3_ERRFLAG_BYTE_SEL);
    mcSHOW_DBG_MSG(("********** Modify_TX_Delay_Cell %d **********\n", i));

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

    //Read_TX_Delay_Cell(p);

    return;
}
#endif

#if 0
void Ett_Mini_Strss_Test(DRAMC_CTX_T *p)
{
    int i=0;
    U32 u4BackupDQSOSCENDIS = 0;
    U8 u1ShuLevel = 0;
    U8 channelIdx, channelBak;
    U8 rankIdx, rankBak;
    U32 u4RegBackupAddress[] =
    {
        (DRAMC_REG_HMR4),
        (DRAMC_REG_DQSOSCR),
        (DRAMC_REG_DUMMY_RD),
#if CHANNEL_NUM > 1
        (DRAMC_REG_HMR4+SHIFT_TO_CHB_ADDR),
        (DRAMC_REG_DQSOSCR+SHIFT_TO_CHB_ADDR),
        (DRAMC_REG_DUMMY_RD+SHIFT_TO_CHB_ADDR),
#endif
#if CHANNEL_NUM > 2
        (DRAMC_REG_HMR4+SHIFT_TO_CHC_ADDR),
        (DRAMC_REG_DQSOSCR+SHIFT_TO_CHC_ADDR),
        (DRAMC_REG_DUMMY_RD+SHIFT_TO_CHC_ADDR),
        (DRAMC_REG_HMR4+SHIFT_TO_CHD_ADDR),
        (DRAMC_REG_DQSOSCR+SHIFT_TO_CHD_ADDR),
        (DRAMC_REG_DUMMY_RD+SHIFT_TO_CHD_ADDR),
#endif
    };

#ifdef FAKE_ENGINE_STRESS
    if(u1IsLP4Family(p->dram_type))
    {
        Fake_Engine_Presetting(p, 0);
    }
#endif

    //DramcEngine2SetPat(p, TEST_XTALK_PATTERN, p->support_rank_num - 1, 0);//bug only one channel
    TA2_Test_Run_Time_Pat_Setting(p, TA2_PAT_SWITCH_OFF);
    do {
        while(u1StopMiniStress){mcDELAY_MS(1000);}

#ifdef TA2_STRESS


        TA2_Test_Run_Time_HW_Presetting(p, TA2_TEST_SIZE, TA2_RKSEL_HW);
        TA2_Test_Run_Time_HW_Write(p, ENABLE);
#endif

#ifdef FAKE_ENGINE_STRESS
        if(u1IsLP4Family(p->dram_type))
        {
            static U8 trans_type = W;
            Do_Memory_Test_Fake_Engine_Presetting(p, (trans_type++) % 3);
        }
#endif
        Get_TA2_ErrCnt(p);

#ifdef TA2_STRESS
        TA2_Test_Run_Time_HW_Status(p);
#endif


        vSetPHY2ChannelMapping(p, (i++) % p->support_channel_num);
        vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ0), \
                                       P_Fld(7, SHURK_SELPH_DQ0_TXDLY_DQ0))
        //vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ2),
        //                         P_Fld(6, SHURK_SELPH_DQ2_DLY_DQ0));

        //vSetRank(p, RANK_1);
        //Modify_TX_Delay_Cell(p, (i++)%16); //Delay cell
        //vSetRank(p, RANK_0);
    } while(1);
    return;
}
#endif

#if 0
void Ett_Mini_Strss_Test(DRAMC_CTX_T *p)
{
    U32 u4BackupDQSOSCENDIS = 0;
    U8 u1ShuLevel = 0;
    U8 channelIdx, channelBak;
    U8 rankIdx, rankBak;
    U32 u4RegBackupAddress[] =
    {
        (DRAMC_REG_HMR4),
        (DRAMC_REG_DQSOSCR),
        (DRAMC_REG_DUMMY_RD),
#if CHANNEL_NUM > 1
        (DRAMC_REG_HMR4+SHIFT_TO_CHB_ADDR),
        (DRAMC_REG_DQSOSCR+SHIFT_TO_CHB_ADDR),
        (DRAMC_REG_DUMMY_RD+SHIFT_TO_CHB_ADDR),
#endif
#if CHANNEL_NUM > 2
        (DRAMC_REG_HMR4+SHIFT_TO_CHC_ADDR),
        (DRAMC_REG_DQSOSCR+SHIFT_TO_CHC_ADDR),
        (DRAMC_REG_DUMMY_RD+SHIFT_TO_CHC_ADDR),
        (DRAMC_REG_HMR4+SHIFT_TO_CHD_ADDR),
        (DRAMC_REG_DQSOSCR+SHIFT_TO_CHD_ADDR),
        (DRAMC_REG_DUMMY_RD+SHIFT_TO_CHD_ADDR),
#endif
    };

#ifdef FAKE_ENGINE_STRESS
    if(u1IsLP4Family(p->dram_type))
    {
        Fake_Engine_Presetting(p, 0);
    }
#endif

    TA2_Test_Run_Time_Pat_Setting(p, TA2_PAT_SWITCH_OFF);

    do {
        while(u1StopMiniStress){mcDELAY_MS(1000);}

#ifdef TA2_STRESS
        if (p->support_rank_num==RANK_DUAL)
        {
            DramcBackupRegisters(p, u4RegBackupAddress, sizeof(u4RegBackupAddress)/sizeof(U32));


            vIO32WriteFldAlign_All(DRAMC_REG_HMR4, 0x1, HMR4_REFRDIS);
            u1ShuLevel = u4IO32ReadFldAlign(DDRPHY_REG_MISC_DVFSCTL, MISC_DVFSCTL_R_OTHER_SHU_GP);
            u4BackupDQSOSCENDIS = u4IO32Read4B(DRAMC_REG_SHU_DQSOSC_SET0 + (SHU_GRP_DRAMC_OFFSET * u1ShuLevel));
            vIO32WriteFldAlign_All(DRAMC_REG_DQSOSCR, 0x1, DQSOSCR_DQSOSCRDIS);
            vIO32WriteFldAlign_All(DRAMC_REG_SHU_DQSOSC_SET0+(SHU_GRP_DRAMC_OFFSET*u1ShuLevel), 0x1, SHU_DQSOSC_SET0_DQSOSCENDIS);
            vIO32WriteFldMulti_All(DRAMC_REG_DUMMY_RD, P_Fld(0x0, DUMMY_RD_DUMMY_RD_EN)
                                                     | P_Fld(0x0, DUMMY_RD_SREF_DMYRD_EN)
                                                     | P_Fld(0x0, DUMMY_RD_DQSG_DMYRD_EN)
                                                     | P_Fld(0x0, DUMMY_RD_DMY_RD_DBG));

            //TA2_Test_Run_Time_Pat_Setting(p, TA2_PAT_SWITCH_ON);
            TA2_Test_Run_Time_HW_Presetting(p, TA2_TEST_SIZE, TA2_RKSEL_XRT);
            TA2_Test_Run_Time_HW_Write(p, ENABLE);
            TA2_Test_Run_Time_HW_Status(p);
        }
#endif

#ifdef TA2_STRESS

        //TA2_Test_Run_Time_Pat_Setting(p, TA2_PAT_SWITCH_ON);
        TA2_Test_Run_Time_HW_Presetting(p, TA2_TEST_SIZE, TA2_RKSEL_HW);
        //vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A2), 0x10, TEST2_A2_TEST2_OFF);


        if (p->support_rank_num==RANK_DUAL)
        {

            vIO32Write4B_All(DRAMC_REG_SHU_DQSOSC_SET0+(SHU_GRP_DRAMC_OFFSET*u1ShuLevel), u4BackupDQSOSCENDIS);
            DramcRestoreRegisters(p, u4RegBackupAddress, sizeof(u4RegBackupAddress)/sizeof(U32));
        }
        TA2_Test_Run_Time_HW_Write(p, ENABLE);
#endif

#ifdef FAKE_ENGINE_STRESS
        if(u1IsLP4Family(p->dram_type))
        {

            //Do_Memory_Test_Fake_Engine_Presetting(p, (trans_type++) % 2);
            static U8 trans_type = W;
            Do_Memory_Test_Fake_Engine_Presetting(p, (trans_type++) % 3);
            //Do_Memory_Test_Fake_Engine_Presetting(p, W);
        }
#endif

#if (DRAMC_DFS_MODE != 3)
        DFSTestProgram(p, 0);
#else
        GetPhyPllFrequency(p);
#endif

#ifdef TA2_STRESS
        TA2_Test_Run_Time_HW_Status(p);
#endif

#ifdef FAKE_ENGINE_STRESS
        if(u1IsLP4Family(p->dram_type))
        {
            Disable_Fake_Engine();
        }
#endif

#ifdef ENABLE_SW_RUN_TIME_ZQ_WA
        DramcRunTimeSWZQ(p);
#endif

        static int cnt=0;
        //if ((++cnt&0x7F)==0)
        if (1)
        {
            MR_periodic_check(p);
#ifdef COMPLEX_STRESS
            int s4value, num;
            U32 ii, addr[] = {
                0x40024000,
                0x56000000,
                0x80000000,
            };
            num = sizeof(addr)>>2;
            for(ii=0;ii<(U32)num;ii++)
            {
                s4value = dramc_complex_mem_test (addr[ii], 0x20000);
                mcSHOW_DBG_MSG(("complex R/W mem 0x%x test %s: %d\n",addr[ii],s4value?"fail":"pass",-s4value));
            }
#endif
        }


#if ENABLE_SW_TX_TRACKING
        channelBak = vGetPHY2ChannelMapping(p);
        rankBak = u1GetRank(p);
        for(channelIdx=CHANNEL_A; channelIdx<(p->support_channel_num); channelIdx++)
        {
            vSetPHY2ChannelMapping(p, channelIdx);
            DramcSWTxTracking(p);
        }
        vSetRank(p, rankBak);
        vSetPHY2ChannelMapping(p, channelBak);
#endif
        //DramcRestoreRegisters(p, u4RegBackupAddress, sizeof(u4RegBackupAddress)/sizeof(U32));
    } while(1);
    return;

}
#endif

#if 0
void Ett_Mini_Strss_Test(DRAMC_CTX_T *p)
{
    int i=0;

#ifdef FAKE_ENGINE_STRESS
    if(u1IsLP4Family(p->dram_type))
    {
        Fake_Engine_Presetting(p, 0);
    }
#endif

    TA2_Test_Run_Time_Pat_Setting(p, TA2_PAT_SWITCH_OFF);
    TA2_Test_Run_Time_HW_Presetting(p, TA2_TEST_SIZE, TA2_RKSEL_HW);

    vIO32WriteFldAlign_All(DRAMC_REG_TEST2_A0, 1, TEST2_A0_TA2_LOOP_EN);
    vIO32WriteFldAlign_All(DRAMC_REG_TEST2_A0, 1, TEST2_A0_LOOP_NV_END);
    vIO32WriteFldAlign_All(DRAMC_REG_TEST2_A0, 1, TEST2_A0_ERR_BREAK_EN);
    TA2_Test_Run_Time_HW_Write(p, ENABLE);
    do {
        while(u1StopMiniStress){mcDELAY_MS(1000);}

#ifdef FAKE_ENGINE_STRESS
        if(u1IsLP4Family(p->dram_type))
        {
            static U8 trans_type = W;
            Do_Memory_Test_Fake_Engine_Presetting(p, (trans_type++) % 3);
        }
#endif
        //Get_TA2_ErrCnt(p);
        Get_TA2_ST(p);


        if(i==20)
        {
            mcSHOW_DBG_MSG(("!!! Error Injection in CHA\n"));

            vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ0), \
                                       P_Fld(7, SHURK_SELPH_DQ0_TXDLY_DQ0))
        }
        i++;
    } while(1);
    return;
}
#endif

#if 0
typedef enum
{
    SIDLE_SR_S1_S0 = 0,
    SIDLE_SR_S1,
} ETT_STRESS_LPS;
void Ett_Mini_Strss_Test(DRAMC_CTX_T *p)
{
    U32 u4BackupDQSOSCENDIS = 0;
    U8 u1ShuLevel = 0;
    bool bTa2_stress_enable = FALSE;
    ETT_STRESS_LPS bLowPwrState = SIDLE_SR_S1_S0;
    int iTestCnt = 0;

    U32 u4RegBackupAddress[] =
    {
        (DRAMC_REG_HMR4),
        (DRAMC_REG_DQSOSCR),
        (DRAMC_REG_DUMMY_RD),
#if CHANNEL_NUM > 1
        (DRAMC_REG_HMR4+SHIFT_TO_CHB_ADDR),
        (DRAMC_REG_DQSOSCR+SHIFT_TO_CHB_ADDR),
        (DRAMC_REG_DUMMY_RD+SHIFT_TO_CHB_ADDR),
#endif
#if CHANNEL_NUM > 2
        (DRAMC_REG_HMR4+SHIFT_TO_CHC_ADDR),
        (DRAMC_REG_DQSOSCR+SHIFT_TO_CHC_ADDR),
        (DRAMC_REG_DUMMY_RD+SHIFT_TO_CHC_ADDR),
        (DRAMC_REG_HMR4+SHIFT_TO_CHD_ADDR),
        (DRAMC_REG_DQSOSCR+SHIFT_TO_CHD_ADDR),
        (DRAMC_REG_DUMMY_RD+SHIFT_TO_CHD_ADDR),
#endif
    };

#ifdef FAKE_ENGINE_STRESS
    if(u1IsLP4Family(p->dram_type))
    {
        Fake_Engine_Presetting(p, 0);
    }
#endif

    do {
        while(u1StopMiniStress){mcDELAY_MS(1000);}
#if 1
        if(iTestCnt % 10 == 0)
        {
            bLowPwrState = (bLowPwrState == SIDLE_SR_S1_S0) ? (SIDLE_SR_S1) : (SIDLE_SR_S1_S0);

            mcSHOW_DBG_MSG(("*** Stop delay then start\n"));
            Reg_Sync_Writel(0x10940020, 0x00000000);
            Reg_Sync_Writel(0x10A40020, 0x00000000);

            Reg_Sync_Writel(0x10940028, 0x00000003);
            Reg_Sync_Writel(0x10A40028, 0x00000003);

            Reg_Sync_Writel(0x109400A0, 1);
            Reg_Sync_Writel(0x10A400A0, 1);

            mcDELAY_MS(100);

            if(bLowPwrState == SIDLE_SR_S1_S0)
            {
                mcSHOW_DBG_MSG(("*** SIDLE_SR_S1_S0\n"));

                Reg_Sync_Writel(0x10940020, 0x00010203);
                Reg_Sync_Writel(0x10A40020, 0x00010203);

                Reg_Sync_Writel(0x10940028, 0x80000004);
                Reg_Sync_Writel(0x10A40028, 0x80000004);
                bTa2_stress_enable = 0;
            }
            else
            {
                mcSHOW_DBG_MSG(("*** SIDLE_SR_S1 stop then start\n"));

                Reg_Sync_Writel(0x10940020, 0x00010200);
                Reg_Sync_Writel(0x10A40020, 0x00010200);

                Reg_Sync_Writel(0x10940028, 0x80000003);
                Reg_Sync_Writel(0x10A40028, 0x80000003);
                bTa2_stress_enable = 1;
            }
            Reg_Sync_Writel(0x109400A0, 1);
            Reg_Sync_Writel(0x10A400A0, 1);
        }
        iTestCnt++;
#endif
#if 0
        if(iTestCnt % 10 == 0)
        {
            bLowPwrState = (bLowPwrState == SIDLE_SR_S1_S0) ? (SIDLE_SR_S1) : (SIDLE_SR_S1_S0);

            mcSHOW_DBG_MSG(("*** Stop delay then start\n"));
            Reg_Sync_Writel(0x10940020, 0x00000000);
            Reg_Sync_Writel(0x10A40020, 0x00000000);

            Reg_Sync_Writel(0x10940028, 0x00000003);
            Reg_Sync_Writel(0x10A40028, 0x00000003);

            Reg_Sync_Writel(0x109400A0, 1);
            Reg_Sync_Writel(0x10A400A0, 1);

            if(bLowPwrState == SIDLE_SR_S1_S0)
            {
                mcSHOW_DBG_MSG(("*** SIDLE_SR_S1_S0\n"));

                Reg_Sync_Writel(0x10940020, 0x00010300);
                Reg_Sync_Writel(0x10A40020, 0x00010300);

                Reg_Sync_Writel(0x10940024, 0x13121110);
                Reg_Sync_Writel(0x10A40024, 0x13121110);

                Reg_Sync_Writel(0x10940028, 0x80000008);
                Reg_Sync_Writel(0x10A40028, 0x80000008);
                bTa2_stress_enable = 0;
            }
            else
            {
                mcSHOW_DBG_MSG(("*** SIDLE_SR_S1\n"));

                Reg_Sync_Writel(0x10940020, 0x00010200);
                Reg_Sync_Writel(0x10A40020, 0x00010200);

                Reg_Sync_Writel(0x10940024, 0x13121110);
                Reg_Sync_Writel(0x10A40024, 0x13121110);

                Reg_Sync_Writel(0x10940028, 0x80000008);
                Reg_Sync_Writel(0x10A40028, 0x80000008);
                bTa2_stress_enable = 1;
            }
            Reg_Sync_Writel(0x109400A0, 1);
            Reg_Sync_Writel(0x10A400A0, 1);
        }
        iTestCnt++;
#endif


#if 0
        if(iTestCnt % 10 == 0)
        {
            bLowPwrState = (bLowPwrState == SIDLE_SR_S1_S0) ? (SIDLE_SR_S1) : (SIDLE_SR_S1_S0);
            if(bLowPwrState == SIDLE_SR_S1_S0)
            {
                mcSHOW_DBG_MSG(("*** SIDLE_SR_S1_S0\n"));

                Reg_Sync_Writel(0x10940020, 0x12111000);
                Reg_Sync_Writel(0x10A40020, 0x12111000);

                Reg_Sync_Writel(0x10940024, 0x16151413);
                Reg_Sync_Writel(0x10A40024, 0x16151413);

                Reg_Sync_Writel(0x10940028, 0x80000008);
                Reg_Sync_Writel(0x10A40028, 0x80000008);
                bTa2_stress_enable = 0;
            }
            else
            {
                mcSHOW_DBG_MSG(("*** SIDLE_SR_S1\n"));

                Reg_Sync_Writel(0x10940020, 0x12111000);
                Reg_Sync_Writel(0x10A40020, 0x12111000);

                Reg_Sync_Writel(0x10940024, 0x16151413);
                Reg_Sync_Writel(0x10A40024, 0x16151413);

                Reg_Sync_Writel(0x10940028, 0x80000008);
                Reg_Sync_Writel(0x10A40028, 0x80000008);
                bTa2_stress_enable = 1;
            }
            Reg_Sync_Writel(0x109400A0, 1);
            Reg_Sync_Writel(0x10A400A0, 1);
        }
        iTestCnt++;
#endif



#ifdef TA2_STRESS
if(bTa2_stress_enable)
{
        if (p->support_rank_num==RANK_DUAL)
        {
            DramcBackupRegisters(p, u4RegBackupAddress, sizeof(u4RegBackupAddress)/sizeof(U32));


            vIO32WriteFldAlign_All(DRAMC_REG_HMR4, 0x1, HMR4_REFRDIS);
            u1ShuLevel = u4IO32ReadFldAlign(DDRPHY_REG_MISC_DVFSCTL, MISC_DVFSCTL_R_OTHER_SHU_GP);
            u4BackupDQSOSCENDIS = u4IO32Read4B(DRAMC_REG_SHU_DQSOSC_SET0 + (SHU_GRP_DRAMC_OFFSET * u1ShuLevel));
            vIO32WriteFldAlign_All(DRAMC_REG_DQSOSCR, 0x1, DQSOSCR_DQSOSCRDIS);
            vIO32WriteFldAlign_All(DRAMC_REG_SHU_DQSOSC_SET0+(SHU_GRP_DRAMC_OFFSET*u1ShuLevel), 0x1, SHU_DQSOSC_SET0_DQSOSCENDIS);
            vIO32WriteFldMulti_All(DRAMC_REG_DUMMY_RD, P_Fld(0x0, DUMMY_RD_DUMMY_RD_EN)
                                                     | P_Fld(0x0, DUMMY_RD_SREF_DMYRD_EN)
                                                     | P_Fld(0x0, DUMMY_RD_DQSG_DMYRD_EN)
                                                     | P_Fld(0x0, DUMMY_RD_DMY_RD_DBG));

            TA2_Test_Run_Time_Pat_Setting(p, TA2_PAT_SWITCH_ON);
            TA2_Test_Run_Time_HW_Presetting(p, TA2_TEST_SIZE, TA2_RKSEL_XRT);
            TA2_Test_Run_Time_HW_Write(p, ENABLE);
            TA2_Test_Run_Time_HW_Status(p);
        }



        TA2_Test_Run_Time_Pat_Setting(p, TA2_PAT_SWITCH_ON);
        TA2_Test_Run_Time_HW_Presetting(p, TA2_TEST_SIZE, TA2_RKSEL_HW);
        //vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A2), 0x10, TEST2_A2_TEST2_OFF);//TODO: Need to find out the reason


        if (p->support_rank_num==RANK_DUAL)
        {

            vIO32Write4B_All(DRAMC_REG_SHU_DQSOSC_SET0+(SHU_GRP_DRAMC_OFFSET*u1ShuLevel), u4BackupDQSOSCENDIS);
            DramcRestoreRegisters(p, u4RegBackupAddress, sizeof(u4RegBackupAddress)/sizeof(U32));
        }
        TA2_Test_Run_Time_HW_Write(p, ENABLE);
}
#endif

#ifdef FAKE_ENGINE_STRESS
        if(u1IsLP4Family(p->dram_type))
        {
            //static U8 trans_type = W;
            //Do_Memory_Test_Fake_Engine_Presetting(p, (trans_type++) % 2);
            static U8 trans_type = W;
            Do_Memory_Test_Fake_Engine_Presetting(p, (trans_type++) % 3);
            //Do_Memory_Test_Fake_Engine_Presetting(p, W);
        }
#endif


#ifdef TA2_STRESS
if(bTa2_stress_enable)
{
        TA2_Test_Run_Time_HW_Status(p);
}
#endif

#ifdef FAKE_ENGINE_STRESS
        if(u1IsLP4Family(p->dram_type))
        {
            Disable_Fake_Engine();
        }
#endif

        static int cnt=0;
        //if ((++cnt&0x7F)==0)
        if (1)
        {
            MR_periodic_check(p);
#ifdef COMPLEX_STRESS
            int s4value, num;
            U32 ii, addr[] = {
                0x40024000,
                0x56000000,
                0x80000000,
            };
            num = sizeof(addr)>>2;
            for(ii=0;ii<(U32)num;ii++)
            {
                s4value = dramc_complex_mem_test (addr[ii], 0x20000);
                mcSHOW_DBG_MSG(("[%d]complex R/W mem 0x%x test %s: %d\n",iTestCnt, addr[ii],s4value?"fail":"pass",-s4value));
            }
#endif
        }
        //DramcRestoreRegisters(p, u4RegBackupAddress, sizeof(u4RegBackupAddress)/sizeof(U32));
    } while(1);
    return;
}
#endif


//#ifdef ETT_MINI_STRESS_TEST
#if 0
//err |= aTA2_Test_Run_Time_HW_Status(p);
unsigned int DPMIsAlive(DRAMC_CTX_T *p);
void Ett_Mini_Strss_Test_DPM(DRAMC_CTX_T *p, int iTestCnt, bool *bTa2_stress_enable);

void Ett_Mini_Strss_Test(DRAMC_CTX_T *p)
{
    static int sTestCnt = 0;
    U8 channelIdx, channelBak;
    U8 rankIdx, rankBak;
    bool bTa2_stress_enable = TRUE;
    int err = 0;

#ifdef FAKE_ENGINE_STRESS
    if(u1IsLP4Family(p->dram_type))
    {
        Fake_Engine_Presetting(p, 0);
    }
#endif

    TA2_Test_Run_Time_Pat_Setting(p, TA2_PAT_SWITCH_OFF);
    TA2_Test_Run_Time_HW_Presetting(p, TA2_TEST_SIZE, TA2_RKSEL_HW);
    TA2_Test_Run_Time_HW_Write(p, ENABLE);

    do {
        while(u1StopMiniStress){mcDELAY_MS(1000);}
        mcSHOW_DBG_MSG(("Stress test.................\n"));

#ifdef TA2_STRESS
        if(bTa2_stress_enable)
        {
            if(sTestCnt)
            {
                TA2_Test_Run_Time_HW_Read(p, ENABLE);
            }
        }
#endif

#ifdef FAKE_ENGINE_STRESS
        if(u1IsLP4Family(p->dram_type))
        {
            static U8 trans_type = W;
            Do_Memory_Test_Fake_Engine_Presetting(p, (trans_type++) % 3);
        }
#endif

        DFSTestProgram(p, 0);

#ifdef TA2_STRESS
        if(bTa2_stress_enable)
        {
            err |= TA2_Test_Run_Time_HW_Status(p);
        }
#endif
        sTestCnt ++;

        if (1)
        {
            MR_periodic_check(p);
#ifdef COMPLEX_STRESS
            int s4value, num;
            U32 ii, addr[] = {
                0x40024000,
                0x56000000,
                0x80000000,
            };
            num = sizeof(addr)>>2;
            for(ii=0;ii<(U32)num;ii++)
            {
                s4value = dramc_complex_mem_test (addr[ii], 0x20000);
                mcSHOW_DBG_MSG(("complex R/W mem 0x%x test %s: %d\n",addr[ii],s4value?"fail":"pass",-s4value));

                err |= s4value;
            }
#endif
        }
    } while(1);
    return;
}
#endif

