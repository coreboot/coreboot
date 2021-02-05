/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef _PI_API_H
#define _PI_API_H

#ifndef __ETT__
#define __ETT__ 0
#endif

#define __FLASH_TOOL_DA__	0
#define CFG_DRAM_LOG_TO_STORAGE	0

#define FORCE_FASTK

/***********************************************************************/
/*              Includes                                               */
/***********************************************************************/

/***********************************************************************/
/*              Constant Define                                        */
/***********************************************************************/

#define SW_CHANGE_FOR_SIMULATION 0  //calibration funciton for whole chip simulation. Code changed due to different compiler
#ifndef FOR_DV_SIMULATION_USED
#define FOR_DV_SIMULATION_USED  (FALSE) ////calibration funciton for DV simulation. Code changed due to different compiler#define FT_DSIM_USED 0
#endif
#define DV_SIMULATION_LP4 1
#define BYPASS_CALIBRATION 0
//Bring Up Selection : Do Not open it when normal operation
//#define SLT
//#define FIRST_BRING_UP
//#define DUMP_INIT_RG_LOG_TO_DE  //dump init RG settings to DE

#include "dramc_typedefs.h"
#include "dramc_reg_base_addr.h"
#include <soc/addressmap.h>
#include <soc/dramc_soc.h>
#include <soc/dramc_param.h>

#define CPU_RW_TEST_AFTER_K  0
#define TA2_RW_TEST_AFTER_K  0

#define ENABLE      (1)
#define DISABLE     (0)
#define ON          (1)
#define OFF         (0)
#define AUTOK_ON    (1)
#define AUTOK_OFF   (0)
#define DCM_ON      (1)
#define DCM_OFF     (0)


//Read Chip QT Tool
#ifndef QT_GUI_Tool
#define QT_GUI_Tool     0    //Setting 1 when using QT GUI Tool Compiler.
#define HAPS_FPFG_A60868    0  //Setting 1 when testing HAPS FPGA
#endif

//DRAMC Chip
#define fcA60868        1
#define fcPetrus        2
#define fcIPM           3
#define fcMargaux        4
#define fcFOR_CHIP_ID   fcMargaux

#define __A60868_TO_BE_PORTING__ 0
#define __Petrus_TO_BE_PORTING__ 0

#define VENDOR_SAMSUNG 1
#define VENDOR_HYNIX 6
#define REVISION_ID_MAGIC 0x9501


#define __LP5_COMBO__   (FALSE)
#define FEATURE_RDDQC_K_DMI   (FALSE)   // This feature is not supported at A60868 test chip


#if __ETT__
#define __FLASH_TOOL_DA__ 0
#endif

#if (FEATURE_RDDQC_K_DMI == TRUE)
    #define RDDQC_ADD_DMI_NUM   2
#else
    #define RDDQC_ADD_DMI_NUM   0
#endif

#define CHANNEL_NUM      2   // single chhanel for A60868.  1 single channel, 2 dual channel, 4 channel
#define DPM_CH_NUM      2 // CH0/1 is Master, CH2/3 is Slave

//ZQ calibration
#define ENABLE_LP4_ZQ_CAL  1
#if ENABLE_LP4_ZQ_CAL //choose one mode to do ZQ calibration
#define ZQ_SWCMD_MODE         1 //suggested SW CMD mode
#define ZQ_RTSWCMD_MODE       0 //run time SW mode
#define ZQ_SCSM_MODE          0 //old mode
#endif

#define CALIBRATION_SPEED_UP_DEBUG 0
#define VENDER_JV_LOG 0

//SW option
#define DUAL_FREQ_K 0 //If enable, need to define DDR_xxxx the same as DUAL_FREQ_HIGH
#define ENABLE_EYESCAN_GRAPH 0 //__ETT__ //draw eye diagram after calibration, if enable, need to fix code size problem.
#define EYESCAN_GRAPH_CATX_VREF_STEP 1  // 1 (origin), 2 (div 2)(save 9K size), 5 for A60868
#define EYESCAN_GRAPH_RX_VREF_STEP 2
#define EYESCAN_RX_VREF_RANGE_END 128   //field is 6 bit, but can only use 0~63,7bit ->127
#if (fcFOR_CHIP_ID == fcA60868)
#define ENABLE_EYESCAN_CBT 0      //TO DO:Forece to draw CBT eye diagram after calibration
#define ENABLE_EYESCAN_RX 0       //TO DO:Forece to draw RX eye diagram after calibration
#define ENABLE_EYESCAN_TX 0       //TO DO:Forece to draw TX eye diagram after calibration
#define ENABLE_VREFSCAN 0           //TO DO:Forece to Vref Scan for calibration
#endif

#define CHECK_HQA_CRITERIA  0
#define REDUCE_LOG_FOR_PRELOADER 1
#define APPLY_LP4_POWER_INIT_SEQUENCE 1
#define ENABLE_READ_DBI 0
#define ENABLE_WRITE_DBI 1
#define ENABLE_WRITE_DBI_Protect 0
#define ENABLE_TX_WDQS 1
#define ENABLE_WDQS_MODE_2 0
#define ENABLE_DRS 0
#define ENABLE_TX_TRACKING 1
#define ENABLE_K_WITH_WORST_SI_UI_SHIFT 1
#define ETT_MINI_STRESS_USE_TA2_LOOP_MODE 1
#define DUMP_TA2_WINDOW_SIZE_RX_TX 0
#if ENABLE_TX_TRACKING
    #define ENABLE_SW_TX_TRACKING 0 //if SW_TX_TRACKING is 0, using HW_TX_TRACKING
#endif
#define ENABLE_PA_IMPRO_FOR_TX_TRACKING 1
#define ENABLE_WRITE_POST_AMBLE_1_POINT_5_TCK 1
#define ENABLE_RX_TRACKING 0
#define ENABLE_RX_DCM_DPHY 1 //Set 0 will lead DCM on/off error
#define ENABLE_OPEN_LOOP_MODE_OPTION 1
#define ENABLE_TMRRI_NEW_MODE 1
#define ENABLE_8PHASE_CALIBRATION 1
#define ENABLE_DUTY_SCAN_V2 1
#define DUTY_SCAN_V2_ONLY_K_HIGHEST_FREQ 0 //0: K all Freq 1: K highest Freq
#define APPLY_DQDQM_DUTY_CALIBRATION 1
#define IMPEDANCE_TRACKING_ENABLE //Impendence tracking
#define IMPEDANCE_HW_SAVING   //mask because function fail, it lets clk swing change larger before DVFS occurs
#define ENABLE_MIOCK_JMETER
#define ENABLE_RUNTIME_MRW_FOR_LP5	1
#define ENABLE_RODT_TRACKING 1
#define GATING_ADJUST_TXDLY_FOR_TRACKING 1
#define TDQSCK_PRECALCULATION_FOR_DVFS 1
#define HW_GATING
#define ENABLE_RX_FIFO_MISMATCH_DEBUG 1
#define VERIFY_CKE_PWR_DOWN_FLOW 0 //Lewis add for DVT
#define CBT_MOVE_CA_INSTEAD_OF_CLK 1	// need to check on LP5
#define MRW_CHECK_ONLY 0
#define MRW_BACKUP 0
#define ENABLE_SAMSUNG_NT_ODT 0
#define DRAMC_MODEREG_CHECK 0
#define DVT_READ_LATENCY_MONITOR 0

#define PINMUX_AUTO_TEST_PER_BIT_CA 0
#define PINMUX_AUTO_TEST_PER_BIT_RX 0
#define PINMUX_AUTO_TEST_PER_BIT_TX 0

#define CA_PER_BIT_DELAY_CELL 1//LP4
#if PINMUX_AUTO_TEST_PER_BIT_CA
#undef CA_PER_BIT_DELAY_CELL
#define CA_PER_BIT_DELAY_CELL 0
#endif

//Gating calibration
#define GATING_LEADLAG_LOW_LEVEL_CHECK 0

//#define ENABLE_POST_PACKAGE_REPAIR

#define DPM_CONTROL_AFTERK
#if __ETT__
#define ENABLE_DBG_2_0_IRQ
#endif

//////////////////////////////////// FIXME start /////////////////////////
#define CMD_CKE_WORKAROUND_FIX 0
#define DQS_DUTY_SLT_CONDITION_TEST 0
#define DV_SIMULATION_BEFORE_K 0
#define DV_SIMULATION_DATLAT 0
#define DV_SIMULATION_DBI_ON 0
#define DV_SIMULATION_DFS 0
#define DV_SIMULATION_GATING 0
#define ENABLE_APB_MASK_WRITE 0
#define ENABLE_DVFS_BYPASS_MR13_FSP 0
#define ENABLE_RODT_TRACKING_SAVE_MCK 0
#define ETT_NO_DRAM 0
#define EYESCAN_LOG 0
#define FSP1_CLKCA_TERM 1
#define MR_CBT_SWITCH_FREQ !FOR_DV_SIMULATION_USED //@Darren, Wait DFS ready
#define FT_DSIM_USED 0
#define GATING_ONLY_FOR_DEBUG 0
#define MEASURE_DRAM_POWER_INDEX 0
#define PA_IMPROVEMENT_FOR_DRAMC_ACTIVE_POWER 0
#define REG_ACCESS_PORTING_DGB 0
#define RX_PIPE_BYPASS_ENABLE 0
#define SAMSUNG_TEST_MODE_MRS_FOR_PRELOADER 0
#define SUPPORT_PICG_MONITOR 0
#define SUPPORT_REQ_QUEUE_BLOCK_ALE 0
#define SUPPORT_REQ_QUEUE_READ_LATERNCY_MONITOR 0
#define REFRESH_OVERHEAD_REDUCTION 1
#define TEST_LOW_POWER_WITH_1_SEC_DELAY 1 //Add 1 second dealy between suspend and resume to avoid APHY control PATH is not switched to SPM {SPM_CONTROL_AFTERK}
#define TEST_LOW_POWER_WITH_STRESS 0
#if TEST_LOW_POWER_WITH_STRESS
    #undef TEST_LOW_POWER_WITH_1_SEC_DELAY
    #define TEST_LOW_POWER_WITH_1_SEC_DELAY 0
#endif
#define XRTRTR_NEW_CROSS_RK_MODE 1
#define XRTWTW_NEW_CROSS_RK_MODE 1
#define APPLY_SIGNAL_WAVEFORM_SETTINGS_ADJUST 0
#define SUPPORT_HYNIX_RX_DQS_WEAK_PULL 0
#define RX_DLY_TRACK_ONLY_FOR_DEBUG 0
//Run time config
#define TEMP_SENSOR_ENABLE // Enable rumtime HMR4
#define ENABLE_REFRESH_RATE_DEBOUNCE 1
#define ENABLE_PER_BANK_REFRESH 1
#define PER_BANK_REFRESH_USE_MODE 1 // 0: original mode, 1: hybrid mode, 2: always pb mode
#define IMP_TRACKING_PB_TO_AB_REFRESH_WA 1
#define DRAMC_MODIFIED_REFRESH_MODE 1
#define DRAMC_CKE_DEBOUNCE 1
#define XRTR2R_PERFORM_ENHANCE_DQSG_RX_DLY

#define SAMSUNG_LP4_NWR_WORKAROUND 0
#define AC_TIMING_DERATE_ENABLE 1
#define ENABLE_EARLY_BG_CMD 0   // 0: Disable 1: Enable, Reduce the CMD latency by CTO_EBG request

//////////////////////////////////// DVFS //////////////////////////////
#define ENABLE_DVS 1 //DVS need tracking enable
#define DRAMC_DFS_MODE 1 // 0:Legacy, 1:MD32 RG, 2: PHY RG
#define ENABLE_SRAM_DMA_WA 0 // for Pexxxs/Maxxxx/xxx868
#define ENABLE_ECO_SRAM_DMA_MISS_REG 1 // for Maxxxx
#define ENABLE_TX_REBASE_ODT_WA 0 // for Pexxxs/xxx868
#if ENABLE_TX_WDQS
#define ENABLE_TX_REBASE_WDQS_DQS_PI_WA 0
#endif
#define ENABLE_DFS_DEBUG_MODE 0
#define DFS_NOQUEUE_FLUSH_WA 1
#define DFS_NOQUEUE_FLUSH_LATENCY_CNT 0
#define ENABLE_DFS_NOQUEUE_FLUSH_DBG 0
#define ENABLE_CONFIG_MCK_4TO1_MUX 0
#define ENABLE_TPBR2PBR_REFRESH_TIMING 1
#define ENABLE_DFS_TIMING_ENLARGE 0
#define ENABLE_DFS_208M_CLOCK 0
#define ENABLE_DFS_HW_SAVE_MASK 0
#define ENABLE_LP4Y_DFS 0
#if ENABLE_LP4Y_DFS
#define ENABLE_LP4Y_WA 1
#define ENABLE_DFS_RUNTIME_MRW 1
#else
#define ENABLE_LP4Y_WA 0
#define ENABLE_DFS_RUNTIME_MRW 0 // for LP4x
#endif
#define ENABLE_TIMING_TXSR_DFS_WA REFRESH_OVERHEAD_REDUCTION // Wait overhead refresh enable, @Darren, Entry SREF -> EXIT SREF -> PDE Command violates tXSR time
#define ENABLE_RANK_NUMBER_AUTO_DETECTION 0
#define DDR_HW_AUTOK_POLLING_CNT 100000

//////////////////////////////////// FIXME end/////////////////////////

#if (fcFOR_CHIP_ID == fcA60868)
#define WORKAROUND_LP5_HEFF 1 //High efficiency mode
#undef ENABLE_RUNTIME_MRW_FOR_LP5
#define ENABLE_RUNTIME_MRW_FOR_LP5 0  // DV fail in 868, use RTSWCMD_MRW
#endif

#if ENABLE_RODT_TRACKING
#define GATING_RODT_LATANCY_EN      0
#else
#define GATING_RODT_LATANCY_EN		1
#endif

#define CHECK_GOLDEN_SETTING (FALSE)
#define APPLY_LOWPOWER_GOLDEN_SETTINGS 1 // 0: DCM Off, 1: DCM On
#define LP5_GOLDEN_SETTING_CHECKER (FALSE)  // FALSE: enable LP4 checker

#if APPLY_LOWPOWER_GOLDEN_SETTINGS
#define TX_PICG_NEW_MODE 1
#define RX_PICG_NEW_MODE 1
#else
#define TX_PICG_NEW_MODE 0
#define RX_PICG_NEW_MODE 0
#endif
#define CMD_PICG_NEW_MODE 0

#define DDR_RESERVE_NEW_MODE 1 //0: old mode 1: new mode
//=============================================================================
// for D Sim sumulation used
//=============================================================================
#if QT_GUI_Tool || !FOR_DV_SIMULATION_USED
#define DV_SIMULATION_INIT_C        1
#define SIMULATION_LP4_ZQ           1
#define SIMULATION_SW_IMPED         1
#define SIMULATION_MIOCK_JMETER     0
#define SIMULATION_8PHASE           0
#define SIMULATION_RX_INPUT_BUF     0
#define SIMUILATION_CBT             1
#define SIMULATION_WRITE_LEVELING   1
#define SIMULATION_DUTY_CYC_MONITOR 0
#define SIMULATION_GATING           1
#define SIMULATION_DATLAT           1
#define SIMULATION_RX_RDDQC         1
#define SIMULATION_RX_PERBIT        1
#define SIMULATION_TX_PERBIT        1  // Please enable with write leveling
#define SIMULATION_RX_DVS           0
#define SIMULATION_RUNTIME_CONFIG   0
#else
#define DV_SIMULATION_INIT_C        1
#define SIMULATION_LP4_ZQ           1
#define SIMULATION_SW_IMPED         1
#define SIMULATION_MIOCK_JMETER     0
#define SIMULATION_8PHASE           0
#define SIMULATION_RX_INPUT_BUF     0
#define SIMUILATION_CBT             1
#define SIMULATION_WRITE_LEVELING   1
#define SIMULATION_DUTY_CYC_MONITOR 0
#define SIMULATION_GATING           1
#define SIMULATION_DATLAT           1
#define SIMULATION_RX_RDDQC         1
#define SIMULATION_RX_PERBIT        1
#define SIMULATION_TX_PERBIT        1  // Please enable with write leveling
#define SIMULATION_RX_DVS           0
#define SIMULATION_RUNTIME_CONFIG   1 // @Darren for DV sim
#endif
//Used to keep original VREF when doing Rx calibration for RX DVS
#define DVS_CAL_KEEP_VREF 0xf

//#define DDR_INIT_TIME_PROFILING
#define DDR_INIT_TIME_PROFILING_TEST_CNT 1
#ifdef DDR_INIT_TIME_PROFILING
extern U16 u2TimeProfileCnt;
#endif

//=============================================================================
// common
#define DQS_NUMBER                  4
#define DQ_DATA_WIDTH               32   // define max support bus width in the system (to allocate array size)
#define TIME_OUT_CNT                100  //100us
#define HW_REG_SHUFFLE_MAX          4

typedef enum
{
    BYTE_0 = 0,
    BYTE_1 = 1,
    ALL_BYTES
} BYTES_T;

//Should be removed after A60868
#define LP5_DDR4266_RDBI_WORKAROUND 0
#define CBT_O1_PINMUX_WORKAROUND  0
#define WLEV_O1_PINMUX_WORKAROUND 0
#define WCK_LEVELING_FM_WORKAROUND  0


/* Gating window */
#define DQS_GW_COARSE_STEP      1
#define DQS_GW_FINE_START       0
#define DQS_GW_FINE_END         32
#define DQS_GW_FINE_STEP        4

#define DQS_GW_UI_PER_MCK	16
#define DQS_GW_PI_PER_UI	32

// DATLAT
#define DATLAT_TAP_NUMBER 32

// RX DQ/DQS
#define MAX_RX_DQSDLY_TAPS          508     // 0x018, May set back to 64 if no need.
#define MAX_RX_DQDLY_TAPS           252
#define RX_VREF_NOT_SPECIFY         0xff
#define RX_VREF_DUAL_RANK_K_FREQ    1866    // if freq >=RX_VREF_DUAL_RANK_K_FREQ, Rank1 rx vref K will be enable.
#define RX_VREF_RANGE_BEGIN         0
#define RX_VREF_RANGE_BEGIN_ODT_OFF 32
#define RX_VREF_RANGE_BEGIN_ODT_ON  24
#define RX_VREF_RANGE_END           128      //field is 6 bit, but can only use 0~63
#define RX_VREF_RANGE_STEP          1
#define RX_PASS_WIN_CRITERIA        30
#define RDDQC_PINMUX_WORKAROUND     1

// TX DQ/DQS
#define TX_AUTO_K_ENABLE 1
#if TX_AUTO_K_ENABLE
#define TX_AUTO_K_DEBUG_ENABLE 0
#define TX_AUTO_K_WORKAROUND 1
#define ENABLE_PA_IMPRO_FOR_TX_AUTOK 1
#endif
#define MAX_TX_DQDLY_TAPS           31   // max DQ TAP number
#define MAX_TX_DQSDLY_TAPS          31   // max DQS TAP number
#define TX_OE_EXTEND 0
#define TX_DQ_OE_SHIFT_LP5 5
#if TX_OE_EXTEND
#define TX_DQ_OE_SHIFT_LP4 4
#else
#define TX_DQ_OE_SHIFT_LP4 3
#endif
#define TX_DQ_OE_SHIFT_LP3 2
#define TX_K_DQM_WITH_WDBI  1
#define TX_OE_CALIBATION (!TX_OE_EXTEND)

#define TX_RETRY_ENABLE 0
#if TX_RETRY_ENABLE
#define TX_RETRY_CONTROL_BY_SPM 1
#define SW_TX_RETRY_ENABLE 0
#else
#define TX_RETRY_CONTROL_BY_SPM 0
#endif

// Sw work around options.
#define CA_TRAIN_RESULT_DO_NOT_MOVE_CLK                 1 // work around for clock multi phase problem(cannot move clk or the clk will be bad)
#define DramcHWDQSGatingTracking_JADE_TRACKING_MODE 1
#define DramcHWDQSGatingTracking_FIFO_MODE          1
#define DONT_MOVE_CLK_DELAY // don't move clk delay
/* If defined for gFreqTbl and fastK
 */
#define LP4_SHU0_FREQ      (1866)
#define LP4_SHU8_FREQ      (1600)
#define LP4_SHU9_FREQ      (1600)
#define LP4_SHU6_FREQ      (1200)
#define LP4_SHU5_FREQ      (1200)
#define LP4_SHU4_FREQ      (800)
#define LP4_SHU3_FREQ      (800)
#define LP4_SHU2_FREQ      (600)
#define LP4_SHU1_FREQ      (600)
#define LP4_SHU7_FREQ      (400)
#define LP4_HIGHEST_FREQ LP4_SHU0_FREQ

#define LP4_SHU0_FREQSEL   (LP4_DDR3733)
#define LP4_SHU8_FREQSEL   (LP4_DDR3200)
#define LP4_SHU9_FREQSEL   (LP4_DDR3200)
#define LP4_SHU6_FREQSEL   (LP4_DDR2400)
#define LP4_SHU5_FREQSEL   (LP4_DDR2400)
#define LP4_SHU4_FREQSEL   (LP4_DDR1600)
#define LP4_SHU3_FREQSEL   (LP4_DDR1600)
#define LP4_SHU2_FREQSEL   (LP4_DDR1200)
#define LP4_SHU1_FREQSEL   (LP4_DDR1200)
#define LP4_SHU7_FREQSEL   (LP4_DDR800)

#if FOR_DV_SIMULATION_USED
#define DEFAULT_TEST2_1_CAL 0x55000000   // pattern0 and base address for test engine when we do calibration
#define DEFAULT_TEST2_2_CAL 0xaa000020   // pattern1 and offset address for test engine when we do calibraion
#else
#define DEFAULT_TEST2_1_CAL 0x55000000   // pattern0 and base address for test engine when we do calibration
#define DEFAULT_TEST2_2_CAL 0xaa000100   // pattern1 and offset address for test engine when we do calibraion
#endif

//CBT/CA training
#define CATRAINING_NUM_LP4      6
#define CATRAINING_NUM_LP5      7
#define CATRAINING_NUM    CATRAINING_NUM_LP5
#define LP4_MRFSP_TERM_FREQ 1333
#define LP5_MRFSP_TERM_FREQ 1866

//Calibration Summary
#define PRINT_CALIBRATION_SUMMARY (!SW_CHANGE_FOR_SIMULATION)
#define PRINT_CALIBRATION_SUMMARY_DETAIL 1
#define PRINT_CALIBRATION_SUMMARY_FASTK_CHECK 0

#if 1 //(FOR_DV_SIMULATION_USED==0)
#define ETT_PRINT_FORMAT  // Apply for both preloader and ETT
#endif

//#define FOR_HQA_TEST_USED   // HQA test used, to print result for easy report
//#define FOR_HQA_REPORT_USED
//Run Time Config
//#define DUMMY_READ_FOR_TRACKING
#define ZQCS_ENABLE_LP4
#ifndef ZQCS_ENABLE_LP4
#define ENABLE_SW_RUN_TIME_ZQ_WA
#endif

//============================ For Future DVT Definition =================================

#define ENABLE_DVFS_CDC_SYNCHRONIZER_OPTION 1
#define ENABLE_BLOCK_APHY_CLOCK_DFS_OPTION 1
#define ENABLE_REMOVE_MCK8X_UNCERT_LOWPOWER_OPTION 1
#define ENABLE_REMOVE_MCK8X_UNCERT_DFS_OPTION 1
#define RDSEL_TRACKING_EN 0 // @Darren, for SHU0 only (DDR3733 or DDR4266)
#define ENABLE_DFS_SSC_WA 0
#define ENABLE_DDR800_OPEN_LOOP_MODE_OPTION 1

//=============================================================================
//#define DDR_BASE 0x40000000ULL //for DV sim and ett_test.c
/***********************************************************************/
/*              Defines                                                */
/***********************************************************************/
#define CBT_LOW_FREQ   0
#define CBT_HIGH_FREQ   1
#define CBT_UNKNOWN_FREQ   0xFF


#if !__ETT__
// Preloader: using config CFG_DRAM_CALIB_OPTIMIZATION to identify
#if (FOR_DV_SIMULATION_USED==0) && !defined(SLT)
// Preloader: using config CFG_DRAM_CALIB_OPTIMIZATION to identify
#define SUPPORT_SAVE_TIME_FOR_CALIBRATION		1

#if defined(FORCE_FASTK) && !SUPPORT_SAVE_TIME_FOR_CALIBRATION
#error "FORCE_FASTK needs enable SUPPORT_SAVE_TIME_FOR_CALIBRATION!"
#endif

#else
// DV simulation, use full calibration flow
#define SUPPORT_SAVE_TIME_FOR_CALIBRATION		0
#endif
#define EMMC_READY 1
#define BYPASS_VREF_CAL		(SUPPORT_SAVE_TIME_FOR_CALIBRATION & EMMC_READY)
#define BYPASS_CBT		(SUPPORT_SAVE_TIME_FOR_CALIBRATION & EMMC_READY)
#define BYPASS_DATLAT		(SUPPORT_SAVE_TIME_FOR_CALIBRATION & EMMC_READY)
#define BYPASS_WRITELEVELING	(SUPPORT_SAVE_TIME_FOR_CALIBRATION & EMMC_READY)
#define BYPASS_RDDQC		(SUPPORT_SAVE_TIME_FOR_CALIBRATION & EMMC_READY)
#define BYPASS_RXWINDOW		(SUPPORT_SAVE_TIME_FOR_CALIBRATION & EMMC_READY)
#define BYPASS_TXWINDOW		(SUPPORT_SAVE_TIME_FOR_CALIBRATION & EMMC_READY)
#define BYPASS_TXOE         (SUPPORT_SAVE_TIME_FOR_CALIBRATION & EMMC_READY)
#define BYPASS_GatingCal	(SUPPORT_SAVE_TIME_FOR_CALIBRATION & EMMC_READY)
#define BYPASS_CA_PER_BIT_DELAY_CELL (SUPPORT_SAVE_TIME_FOR_CALIBRATION & EMMC_READY)
//#define BYPASS_TX_PER_BIT_DELAY_CELL (SUPPORT_SAVE_TIME_FOR_CALIBRATION & EMMC_READY)
#else
// ETT
#define SUPPORT_SAVE_TIME_FOR_CALIBRATION 0
#define EMMC_READY 0
#define BYPASS_VREF_CAL 1
#define BYPASS_CBT 1
#define BYPASS_DATLAT 1
#define BYPASS_WRITELEVELING 1
#define BYPASS_RDDQC 1
#define BYPASS_RXWINDOW 1
#define BYPASS_TXWINDOW 1
#define BYPASS_TXOE 1
#define BYPASS_GatingCal 1
#define BYPASS_CA_PER_BIT_DELAY_CELL CA_PER_BIT_DELAY_CELL
//#define BYPASS_TX_PER_BIT_DELAY_CELL 0
#endif

#define ENABLE_PINMUX_FOR_RANK_SWAP    0

//======================== FIRST_BRING_UP Init Definition =====================
#ifdef FIRST_BRING_UP

//#define USE_CLK26M

#undef TDQSCK_PRECALCULATION_FOR_DVFS
#define TDQSCK_PRECALCULATION_FOR_DVFS  0//DQS pre-calculation

#undef CHANNEL_NUM
#define CHANNEL_NUM 2

#undef ENABLE_DUTY_SCAN_V2
#define ENABLE_DUTY_SCAN_V2 0

#undef ENABLE_DRS
#define ENABLE_DRS 0

#undef ENABLE_CA_TRAINING
#define ENABLE_CA_TRAINING  1
#undef ENABLE_WRITE_LEVELING
#define ENABLE_WRITE_LEVELING 1
#undef ENABLE_PHY_RX_INPUT_OFFSET
#define ENABLE_PHY_RX_INPUT_OFFSET  0

//#undef REDUCE_LOG_FOR_PRELOADER
//#define REDUCE_LOG_FOR_PRELOADER 0

#undef REDUCE_CALIBRATION_OLYMPUS_ONLY
#define REDUCE_CALIBRATION_OLYMPUS_ONLY 0

#undef APPLY_LOWPOWER_GOLDEN_SETTINGS
#define APPLY_LOWPOWER_GOLDEN_SETTINGS 0 //Should open APPLY_LOWPOWER_GOLDEN_SETTINGS before SB + 3

//#undef SPM_CONTROL_AFTERK //Should open SPM_CONTROL_AFTERK before SB + 3

#undef TX_K_DQM_WITH_WDBI
#define TX_K_DQM_WITH_WDBI 0

#undef ENABLE_EYESCAN_GRAPH
#define ENABLE_EYESCAN_GRAPH 0

#undef PA_IMPROVEMENT_FOR_DRAMC_ACTIVE_POWER
#define PA_IMPROVEMENT_FOR_DRAMC_ACTIVE_POWER 0

#undef ENABLE_TX_TRACKING
#undef ENABLE_SW_TX_TRACKING
#define ENABLE_TX_TRACKING  0
#define ENABLE_SW_TX_TRACKING 0

#undef ENABLE_RX_TRACKING
#define ENABLE_RX_TRACKING  0

#undef ENABLE_PER_BANK_REFRESH
#define ENABLE_PER_BANK_REFRESH 1

#undef CMD_PICG_NEW_MODE
#define CMD_PICG_NEW_MODE 0

#undef XRTWTW_NEW_CROSS_RK_MODE
#define XRTWTW_NEW_CROSS_RK_MODE 1
#undef XRTRTR_NEW_CROSS_RK_MODE
#define XRTRTR_NEW_CROSS_RK_MODE 1

#undef ENABLE_DVFS_BYPASS_MR13_FSP
#define ENABLE_DVFS_BYPASS_MR13_FSP 0

#undef HW_GATING
#undef DUMMY_READ_FOR_TRACKING
#undef ZQCS_ENABLE_LP4
//#define ZQCS_ENABLE_LP4

#undef TEMP_SENSOR_ENABLE
//#define TEMP_SENSOR_ENABLE
#undef IMPEDANCE_TRACKING_ENABLE
//#define IMPEDANCE_TRACKING_ENABLE
#undef ENABLE_SW_RUN_TIME_ZQ_WA

//#undef XRTR2R_PERFORM_ENHANCE_DQSG_RX_DLY

#undef APPLY_SIGNAL_WAVEFORM_SETTINGS_ADJUST
#define APPLY_SIGNAL_WAVEFORM_SETTINGS_ADJUST 0

#undef DFS_NOQUEUE_FLUSH_WA
#define DFS_NOQUEUE_FLUSH_WA 0


#undef TX_PICG_NEW_MODE
#undef RX_PICG_NEW_MODE
#undef ENABLE_RX_DCM_DPHY
#define ENABLE_RX_DCM_DPHY 1 //Set 0 will lead DCM on/off error
#if APPLY_LOWPOWER_GOLDEN_SETTINGS
#define TX_PICG_NEW_MODE 1
#define RX_PICG_NEW_MODE 1
#else
#define TX_PICG_NEW_MODE 0
#define RX_PICG_NEW_MODE 0
#endif


#if 0
#undef XRTW2W_PERFORM_ENHANCE_TX
#undef XRTR2R_PERFORM_ENHANCE_DQSG_RX_DLY
#ifdef XRTR2W_PERFORM_ENHANCE_RODTEN
#undef XRTR2W_PERFORM_ENHANCE_RODTEN     //conflict with ENABLE_RODT_TRACKING, LP4 support only
#endif
#endif
#endif //FIRST_BRING_UP

#define CFG_DRAM_SAVE_FOR_RUNTIME_SHMOO    0
#define RUNTIME_SHMOO_RELEATED_FUNCTION    CFG_DRAM_SAVE_FOR_RUNTIME_SHMOO
#define RUNTIME_SHMOO_RG_BACKUP_NUM         (100)
#define RUNTIME_SHMOO_TX    0    //TX RX can't be opened simultaneously
#define RUNTIME_SHMOO_RX    0
#if RUNTIME_SHMOO_RELEATED_FUNCTION    //if enable rshmoo, close TX OE calibration
#undef TX_OE_EXTEND
#define TX_OE_EXTEND 1
#undef TX_DQ_OE_SHIFT_LP4
#define TX_DQ_OE_SHIFT_LP4 4
#undef TX_OE_CALIBATION
#define TX_OE_CALIBATION (!TX_OE_EXTEND)
#undef ENABLE_RX_TRACKING_LP4
#define ENABLE_RX_TRACKING_LP4 0
#undef ENABLE_TX_TRACKING
#undef ENABLE_SW_TX_TRACKING
#define ENABLE_TX_TRACKING  0
#define ENABLE_SW_TX_TRACKING 0
#define RUNTIME_SHMOO_FAST_K 1
#define RUNTIME_SHMOO_TEST_CHANNEL    0   // 0: CHA, 1: CHB
#define RUNTIME_SHMOO_TEST_RANK       0   // 0: RK0, 1: RK1
#define RUNTIME_SHMOO_TEST_BYTE       0   // 0: Byte0, 1: Byte1
#define RUNTIME_SHMOO_TEST_PI_DELAY_START 0   // 0~63
#define RUNTIME_SHMOO_TEST_PI_DELAY_END   63  // 0~63
#define RUNTIME_SHMOO_TEST_PI_DELAY_STEP  1
#define RUNTIME_SHMOO_RX_VREF_RANGE_END          127 //La_fite: 63
#define RUNTIME_SHMOO_RX_TEST_MARGIN        2 //for RX Delay (start:first_pass-margin, end:last_pass +margin)
#define RUNTIME_SHMOO_TEST_VREF_START     0   // 0~81  : 0~50 is range 0, 51~81 is range 1
#define RUNTIME_SHMOO_TEST_VREF_END       81  // 0~81  : 0~50 is range 0, 51~81 is range 1
#define RUNTIME_SHMOO_TEST_VREF_STEP      1
#endif
typedef enum
{
    CLK_MUX_208M = 0,
    CLK_MUX_104M,
    CLK_MUX_52M,
} CLK_MUX_T;

typedef enum
{
    BEF_DFS_MODE = 0,
    AFT_DFS_MODE,
    CHG_CLK_MODE,
} DFS_DBG_T;

typedef enum
{
    SHUFFLE_HW_MODE = 0,
    SPM_DEBUG_MODE,
    RG_DEBUG_MODE,
} DFS_IP_CLOCK_T;

typedef enum
{
    DutyScan_Calibration_K_CLK= 0,
    DutyScan_Calibration_K_DQS,
    DutyScan_Calibration_K_DQ,
    DutyScan_Calibration_K_DQM,
    DutyScan_Calibration_K_WCK
} DUTYSCAN_CALIBRATION_FLOW_K_T;

typedef enum
{
    DQS_8PH_DEGREE_0 = 0,
    DQS_8PH_DEGREE_180,
    DQS_8PH_DEGREE_45,
    DQS_8PH_DEGREE_MAX,
} DQS_8_PHASE_T;

typedef enum
{
    DRVP = 0,
    DRVN,
    ODTP,
    ODTN,
    IMP_DRV_MAX
} DRAM_IMP_DRV_T;

typedef enum
{
    IMP_LOW_FREQ = 0,
    IMP_HIGH_FREQ,
    IMP_NT_ODTN, // Samsung support only for LP4X
    IMP_VREF_MAX
} DRAMC_IMP_T;

typedef enum
{
    GET_MDL_USED = 0,
    NORMAL_USED,
    SLT_USED
} DRAM_INIT_USED_T;

typedef enum
{
    PATTERN_RDDQC,
    PATTERN_TEST_ENGINE,
} RX_PATTERN_OPTION_T;

typedef enum
{
    DRAM_OK = 0, // OK
    DRAM_FAIL,    // FAIL
    DRAM_FAST_K,
    DRAM_NO_K,
} DRAM_STATUS_T; // DRAM status type

typedef enum
{
    VREF_RANGE_0= 0,
    VREF_RANGE_1,
    VREF_RANGE_MAX
}DRAM_VREF_RANGE_T;
#define VREF_VOLTAGE_TABLE_NUM_LP4 51
#define VREF_VOLTAGE_TABLE_NUM_LP5 128

typedef enum
{
    CKE_FIXOFF = 0,
    CKE_FIXON,
    CKE_DYNAMIC //After CKE FIX on/off, CKE should be returned to dynamic (control by HW)
} CKE_FIX_OPTION;

typedef enum
{
    CKE_WRITE_TO_ONE_CHANNEL = 0, //just need to write CKE FIX register to current channel
    CKE_WRITE_TO_ALL_CHANNEL, //need to write CKE FIX register to all channel
    CKE_WRITE_TO_ALL_RANK
} CKE_FIX_CHANNEL;

typedef enum
{
    LP5_DDR6400 = 0,
    LP5_DDR6000,
    LP5_DDR5500,
    LP5_DDR4800,
    LP5_DDR4266,
    LP5_DDR3733,
    LP5_DDR3200,
    LP5_DDR2400,
    LP5_DDR1600,
    LP5_DDR1200,
    LP5_DDR800,

    LP4_DDR4266,
    LP4_DDR3733,
    LP4_DDR3200,
    LP4_DDR2667,
    LP4_DDR2400,
    LP4_DDR2280,
    LP4_DDR1866,
    LP4_DDR1600,
    LP4_DDR1200,
    LP4_DDR800,
    LP4_DDR400,

    PLL_FREQ_SEL_MAX
} DRAM_PLL_FREQ_SEL_T; // DRAM DFS type

typedef enum
{
    MCK_TO_4UI_SHIFT = 2,
    MCK_TO_8UI_SHIFT = 3,
    MCK_TO_16UI_SHIFT = 4
} MCK_TO_UI_SHIFT_T;

typedef enum
{
    AUTOK_CA,
    AUTOK_CS,
    AUTOK_DQS
} ATUOK_MODE_T;

typedef enum
{
    AUTOK_RESPI_1 = 0,
    AUTOK_RESPI_2 = 1,
    AUTOK_RESPI_4 = 2,
    AUTOK_RESPI_8 = 3
} AUTOK_PI_RESOLUTION;

typedef enum
{
    DRAM_DFS_REG_SHU0 = 0,
    DRAM_DFS_REG_SHU1,
    DRAM_DFS_REG_MAX
} DRAM_DFS_REG_SHU_T;

typedef enum
{
    SRAM_SHU0 = 0,
    SRAM_SHU1,
    SRAM_SHU2,
    SRAM_SHU3,
    SRAM_SHU4,
    SRAM_SHU5,
    SRAM_SHU6,
    DRAM_DFS_SRAM_MAX
} DRAM_DFS_SRAM_SHU_T; // DRAM SRAM RG type

typedef enum
{
    SHUFFLE_RG = 0,
    NONSHUFFLE_RG,
    BOTH_SHU_NONSHU_RG,
} RG_SHU_TYPE_T; // RG SHUFFLE type
typedef enum
{
    DIV16_MODE = 0,
    DIV8_MODE,
    DIV4_MODE,
    UNKNOWN_MODE,
} DIV_MODE_T;

typedef enum
{
    DUTY_DEFAULT = 0,
    DUTY_NEED_K,
    DUTY_LAST_K
} DUTY_CALIBRATION_T;


typedef enum
{
    VREF_CALI_OFF = 0,
    VREF_CALI_ON,
} VREF_CALIBRATION_ENABLE_T;

typedef enum
{
    DDR800_CLOSE_LOOP = 0,
    OPEN_LOOP_MODE,
    SEMI_OPEN_LOOP_MODE,
    CLOSE_LOOP_MODE,
} DDR800_MODE_T;

typedef enum
{
    DRAM_CALIBRATION_SW_IMPEDANCE= 0,
    DRAM_CALIBRATION_DUTY_SCAN,
    DRAM_CALIBRATION_ZQ,
    DRAM_CALIBRATION_JITTER_METER,
    DRAM_CALIBRATION_CA_TRAIN ,
    DRAM_CALIBRATION_WRITE_LEVEL,
    DRAM_CALIBRATION_GATING,
    DRAM_CALIBRATION_RX_RDDQC,
    DRAM_CALIBRATION_TX_PERBIT,
    DRAM_CALIBRATION_DATLAT,
    DRAM_CALIBRATION_RX_PERBIT,
    DRAM_CALIBRATION_TX_OE,
    DRAM_CALIBRATION_MAX
} DRAM_CALIBRATION_STATUS_T;

typedef struct _DRAM_DFS_FREQUENCY_TABLE_T
{
    DRAM_PLL_FREQ_SEL_T freq_sel;
    DIV_MODE_T divmode;
    DRAM_DFS_SRAM_SHU_T shuffleIdx;
    DUTY_CALIBRATION_T duty_calibration_mode;
    VREF_CALIBRATION_ENABLE_T vref_calibartion_enable; // CBT/RX/TX vref calibration enable or not
    DDR800_MODE_T ddr_loop_mode;
} DRAM_DFS_FREQUENCY_TABLE_T;

typedef enum
{
    CHANNEL_SINGLE = 1,
    CHANNEL_DUAL,
#if (CHANNEL_NUM > 2)
    CHANNEL_THIRD,
    CHANNEL_FOURTH
#endif
} DRAM_CHANNEL_NUMBER_T;


typedef enum
{
    RANK_SINGLE = 1,
    RANK_DUAL
} DRAM_RANK_NUMBER_T;


typedef enum
{
    TYPE_DDR1 = 1,
    TYPE_LPDDR2,
    TYPE_LPDDR3,
    TYPE_PCDDR3,
    TYPE_LPDDR4,
    TYPE_LPDDR4X,
    TYPE_LPDDR4P,
    TYPE_LPDDR5
} DRAM_DRAM_TYPE_T;

typedef enum
{
    PINMUX_DSC = 0,
    PINMUX_LPBK,
    PINMUX_EMCP,
    PINMUX_MAX
} DRAM_PINMUX;

/* For faster switching between term and un-term operation
 * FSP_0: For un-terminated freq.
 * FSP_1: For terminated freq.
 */
typedef enum
{
    FSP_0 = 0,
    FSP_1,
    FSP_2,
    FSP_MAX
} DRAM_FAST_SWITH_POINT_T;

typedef struct
{
	u8 pat_v[8];
	u8 pat_a[8];
	u8 pat_dmv;
	u8 pat_dma;
	u8 pat_cs0;
	u8 pat_cs1;
	u8 ca_golden_sel;
	u8 invert_num;
} new_cbt_pat_cfg_t;

typedef enum
{
	TRAINING_MODE1 = 0,
	TRAINING_MODE2
} lp5_training_mode_t;

typedef enum
{
	CBT_PHASE_RISING = 0,
	CBT_PHASE_FALLING
} lp5_cbt_phase_t;

/*
 * External CBT mode enum
 * Due to MDL structure compatibility (single field for dram CBT mode),
 * the below enum is used in preloader to differentiate between dram cbt modes
 */
typedef enum
{
    CBT_R0_R1_NORMAL = 0,   // Normal mode
    CBT_R0_R1_BYTE,         // Byte mode
    CBT_R0_NORMAL_R1_BYTE,  // Mixed mode R0: Normal R1: Byte
    CBT_R0_BYTE_R1_NORMAL   // Mixed mode R0: Byte R1: Normal
} DRAM_CBT_MODE_EXTERN_T;

typedef enum
{
    ODT_OFF = 0,
    ODT_ON
} DRAM_ODT_MODE_T;

typedef enum
{
    DBI_OFF = 0,
    DBI_ON
} DRAM_DBI_MODE_T;

typedef enum
{
    DATA_WIDTH_16BIT = 16,
    DATA_WIDTH_32BIT = 32
} DRAM_DATA_WIDTH_T;

typedef enum
{
    TE_OP_WRITE_READ_CHECK = 0,
    TE_OP_READ_CHECK
} DRAM_TE_OP_T;

typedef enum
{
    TEST_ISI_PATTERN = 0, //don't change
    TEST_AUDIO_PATTERN = 1, //don't change
    TEST_XTALK_PATTERN = 2, //don't change
    TEST_WORST_SI_PATTERN,
    TEST_TA1_SIMPLE,
    TEST_TESTPAT4,
    TEST_TESTPAT4_3,
    TEST_MIX_PATTERN,
    TEST_DMA,
    TEST_SSOXTALK_PATTERN,
} DRAM_TEST_PATTERN_T;

typedef enum
{
    TE_NO_UI_SHIFT = 0,
    TE_UI_SHIFT
} DRAM_TE_UI_SHIFT_T;

typedef enum
{
    TX_DQ_DQS_MOVE_DQ_ONLY = 0,
    TX_DQ_DQS_MOVE_DQM_ONLY,
    TX_DQ_DQS_MOVE_DQ_DQM
} DRAM_TX_PER_BIT_CALIBRATION_TYTE_T;

typedef enum
{
    TX_DQM_WINDOW_SPEC_IN = 0xfe,
    TX_DQM_WINDOW_SPEC_OUT = 0xff
} DRAM_TX_PER_BIT_DQM_WINDOW_RESULT_TYPE_T;

// enum for CKE toggle mode (toggle both ranks 1. at the same time (CKE_RANK_DEPENDENT) 2. individually (CKE_RANK_INDEPENDENT))
typedef enum
{
    CKE_RANK_INDEPENDENT = 0,
    CKE_RANK_DEPENDENT
} CKE_CTRL_MODE_T;

typedef enum
{
    TA2_RKSEL_XRT = 3,
    TA2_RKSEL_HW = 4,
} TA2_RKSEL_TYPE_T;

typedef enum
{
    TA2_PAT_SWITCH_OFF = 0,
    TA2_PAT_SWITCH_ON,
} TA2_PAT_SWITCH_TYPE_T;

typedef enum
{
    PHYPLL_MODE = 0,
    CLRPLL_MODE,
} PLL_MODE_T;

typedef enum
{
    RUNTIME_SWCMD_CAS_FS = 0,
    RUNTIME_SWCMD_CAS_OFF,
    RUNTIME_SWCMD_WCK2DQI_START,
    RUNTIME_SWCMD_WCK2DQO_START,
    RUNTIME_SWCMD_MRW,
    RUNTIME_SWCMD_ZQCAL_START,
    RUNTIME_SWCMD_ZQCAL_LATCH
} RUNTIME_SWCMD_SEL_T;

typedef enum
{
    PI_BASED,
    DLY_BASED
} WLEV_DELAY_BASED_T;

enum lpddr5_rpre_mode {
	LPDDR5_RPRE_4S_0T = 0,
	LPDDR5_RPRE_2S_2T,
	LPDDR5_RPRE_0S_4T,
	LPDDR5_RPRE_XS_4T, /* X = 2~4tWCK */
};

enum rxdqs_autok_burst_len {
	RXDQS_BURST_LEN_8 = 0,
	RXDQS_BURST_LEN_16,
	RXDQS_BURST_LEN_32,
};

typedef enum
{
    EYESCAN_FLAG_DISABLE= 0,
    EYESCAN_FLAG_ENABLE,
    EYESCAN_FLAG_ENABLE_BUT_NORMAL_K,
} EYESCAN_FLAG_TYPE_T;

#ifdef FOR_HQA_REPORT_USED
typedef enum
{
    HQA_REPORT_FORMAT0 = 0,
    HQA_REPORT_FORMAT0_1,
    HQA_REPORT_FORMAT0_2,
    HQA_REPORT_FORMAT1,
    HQA_REPORT_FORMAT2,
    HQA_REPORT_FORMAT2_1,
    HQA_REPORT_FORMAT3,
    HQA_REPORT_FORMAT4,
    HQA_REPORT_FORMAT5,
    HQA_REPORT_FORMAT6
} HQA_REPORT_FORMAT_T;
#endif

#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
#if RUNTIME_SHMOO_RELEATED_FUNCTION
typedef struct _RUNTIME_SHMOO_SAVE_PARAMETER_T
{
    U8 flag;
    U16 TX_PI_delay;
    U16 TX_Original_PI_delay;
    U16 TX_DQM_PI_delay;
    U16 TX_Original_DQM_PI_delay;
    S16 RX_delay[8];
    S16 RX_Original_delay;
    U8 TX_Vref_Range;
    U8 TX_Vref_Value;
    U8 TX_Channel;
    U8 TX_Rank;
    U8 TX_Byte;
    U8 Scan_Direction;
} RUNTIME_SHMOO_SAVE_PARAMETER_T;
#endif

typedef struct _SAVE_TIME_FOR_CALIBRATION_T
{
    //U8 femmc_Ready;

    DRAM_RANK_NUMBER_T support_rank_num;

    U16 u2num_dlycell_perT;
    U16 u2DelayCellTimex100;

    // CLK & DQS duty
    S8 s1ClockDuty_clk_delay_cell[CHANNEL_NUM][RANK_MAX];
    S8 s1DQSDuty_clk_delay_cell[CHANNEL_NUM][DQS_NUMBER_LP4];
    S8 s1WCKDuty_clk_delay_cell[CHANNEL_NUM][DQS_NUMBER_LP4];
#if APPLY_DQDQM_DUTY_CALIBRATION
    S8 s1DQDuty_clk_delay_cell[CHANNEL_NUM][DQS_NUMBER_LP4];
    S8 s1DQMDuty_clk_delay_cell[CHANNEL_NUM][DQS_NUMBER_LP4];
#endif
    // CBT
    U8 u1CBTVref_Save[CHANNEL_NUM][RANK_MAX];
    S8 s1CBTCmdDelay_Save[CHANNEL_NUM][RANK_MAX];
    U8 u1CBTCsDelay_Save[CHANNEL_NUM][RANK_MAX];
    #if CA_PER_BIT_DELAY_CELL
    U8 u1CBTCA_PerBit_DelayLine_Save[CHANNEL_NUM][RANK_MAX][DQS_BIT_NUMBER];
    #endif

    // Write leveling
    U8 u1WriteLeveling_bypass_Save[CHANNEL_NUM][RANK_MAX][DQS_NUMBER_LP4];  //for bypass writeleveling

    // Gating
    U8 u1Gating_MCK_Save[CHANNEL_NUM][RANK_MAX][DQS_NUMBER_LP4];
    U8 u1Gating_UI_Save[CHANNEL_NUM][RANK_MAX][DQS_NUMBER_LP4];
    U8 u1Gating_PI_Save[CHANNEL_NUM][RANK_MAX][DQS_NUMBER_LP4];
    U8 u1Gating_pass_count_Save[CHANNEL_NUM][RANK_MAX][DQS_NUMBER_LP4];

    // TX perbit
    U8 u1TxWindowPerbitVref_Save[CHANNEL_NUM][RANK_MAX];
    U16 u1TxCenter_min_Save[CHANNEL_NUM][RANK_MAX][DQS_NUMBER_LP4];
    U16 u1TxCenter_max_Save[CHANNEL_NUM][RANK_MAX][DQS_NUMBER_LP4];
    U16 u1Txwin_center_Save[CHANNEL_NUM][RANK_MAX][DQ_DATA_WIDTH_LP4];

    // Datlat
    U8 u1RxDatlat_Save[CHANNEL_NUM][RANK_MAX];

    // RX perbit
    U8 u1RxWinPerbitVref_Save[CHANNEL_NUM][RANK_MAX][DQS_NUMBER_LP4];
    U16 u1RxWinPerbit_DQS[CHANNEL_NUM][RANK_MAX][DQS_NUMBER_LP4];
    U16 u1RxWinPerbit_DQM[CHANNEL_NUM][RANK_MAX][DQS_NUMBER_LP4];
    U16 u1RxWinPerbit_DQ[CHANNEL_NUM][RANK_MAX][DQ_DATA_WIDTH_LP4];

    //TX OE
    U8 u1TX_OE_DQ_MCK[CHANNEL_NUM][RANK_MAX][DQS_NUMBER_LP4];
    U8 u1TX_OE_DQ_UI[CHANNEL_NUM][RANK_MAX][DQS_NUMBER_LP4];


#if RUNTIME_SHMOO_RELEATED_FUNCTION
    S16 u1RxWinPerbitDQ_firsbypass_Save[CHANNEL_NUM][RANK_MAX][DQ_DATA_WIDTH_LP4];  //for bypass rxwindow
    U8 u1RxWinPerbitDQ_lastbypass_Save[CHANNEL_NUM][RANK_MAX][DQ_DATA_WIDTH_LP4];  //for bypass rxwindow
    U8 u1SwImpedanceResule[2][4];
    U32 u4RG_Backup[CHANNEL_NUM][RUNTIME_SHMOO_RG_BACKUP_NUM];

    RUNTIME_SHMOO_SAVE_PARAMETER_T Runtime_Shmoo_para;
#endif
}SAVE_TIME_FOR_CALIBRATION_T;
#endif // SUPPORT_SAVE_TIME_FOR_CALIBRATION

#if MRW_CHECK_ONLY
#define MR_NUM 64
extern U16 u2MRRecord[CHANNEL_NUM][RANK_MAX][FSP_MAX][MR_NUM];
#endif

////////////////////////////
typedef struct _DRAMC_CTX_T
{
    DRAM_CHANNEL_NUMBER_T support_channel_num;
    DRAM_CHANNEL_T channel;
    DRAM_RANK_NUMBER_T support_rank_num;
    DRAM_RANK_T rank;
    DRAM_PLL_FREQ_SEL_T freq_sel;
    DRAM_DFS_SHUFFLE_TYPE_T shu_type;
    DRAM_DRAM_TYPE_T dram_type;
    DRAM_FAST_SWITH_POINT_T dram_fsp;  // only for LP4, uesless in LP3
    DRAM_ODT_MODE_T odt_onoff;/// only for LP4, uesless in LP3
    DRAM_CBT_MODE_T dram_cbt_mode[RANK_MAX]; //only for LP4, useless in LP3
    DRAM_DBI_MODE_T DBI_R_onoff[FSP_MAX];   // only for LP4, uesless in LP3
    DRAM_DBI_MODE_T DBI_W_onoff[FSP_MAX];   // only for LP4, uesless in LP3
    DRAM_DATA_WIDTH_T data_width;
    U32 test2_1;
    U32 test2_2;
    DRAM_TEST_PATTERN_T test_pattern;
    U16 frequency;
    U16 freqGroup; /* Used to support freq's that are not in ACTimingTable */
    U16 vendor_id;
    U16 revision_id;
    U16 density;
    U64 ranksize[RANK_MAX];
    U16 u2num_dlycell_perT;
    U16 u2DelayCellTimex100;
    //U8 enable_cbt_scan_vref;
    //U8 enable_rx_scan_vref;
    //U8 enable_tx_scan_vref;

    #if PRINT_CALIBRATION_SUMMARY
    U32 aru4CalResultFlag[CHANNEL_NUM][RANK_MAX];// record the calibration is fail or success,  0:success, 1: fail
    U32 aru4CalExecuteFlag[CHANNEL_NUM][RANK_MAX]; // record the calibration is execute or not,  0:no operate, 1: done
    U32 SWImpCalResult;
    U32 SWImpCalExecute;
    #if PRINT_CALIBRATION_SUMMARY_FASTK_CHECK
    U32 FastKResultFlag[2][RANK_MAX];// record the calibration is fail or success,  0:success, 1: fail
    U32 FastKExecuteFlag[2][RANK_MAX]; // record the calibration is execute or not,  0:no operate, 1: done
    #endif
    #endif

    bool isWLevInitShift[CHANNEL_NUM];

    #if SUPPORT_SAVE_TIME_FOR_CALIBRATION
    U8 femmc_Ready;
    // Calibration or not
    U8 Bypass_TXWINDOW;
    U8 Bypass_RXWINDOW;
    U8 Bypass_RDDQC;
    SAVE_TIME_FOR_CALIBRATION_T *pSavetimeData;
    #endif
    DRAM_DFS_FREQUENCY_TABLE_T *pDFSTable;
    DRAM_DFS_REG_SHU_T ShuRGAccessIdx;
    lp5_training_mode_t lp5_training_mode; //only for LP5
    lp5_cbt_phase_t lp5_cbt_phase; //only for LP5
    u8 new_cbt_mode;
    U8 u1PLLMode;
    DRAM_DBI_MODE_T curDBIState;
    DRAM_FAST_SWITH_POINT_T support_fsp_num;
    DRAM_PINMUX DRAMPinmux;
    U8 u110GBEn[RANK_MAX];
    bool isMaxFreq4266;
} DRAMC_CTX_T;

typedef struct _DRAM_DVFS_TABLE_T
{
    DRAM_PLL_FREQ_SEL_T freq_sel;
    DIV_MODE_T divmode;
    DRAM_DFS_SRAM_SHU_T shuffleIdx;
    U32 u4Vcore;
} DRAM_DVFS_TABLE_T;

typedef struct _PASS_WIN_DATA_T
{
    S16 first_pass;
    S16 last_pass;
    S16 win_center;
    U16 win_size;
    U16 best_dqdly;
} PASS_WIN_DATA_T;

typedef struct _FINAL_WIN_DATA_T {
	unsigned char final_vref;
	signed int final_ca_clk;
	unsigned char final_range;
} FINAL_WIN_DATA_T;

typedef struct _REG_TRANSFER
{
    U32 u4Addr;
    U32 u4Fld;
} REG_TRANSFER_T;

typedef struct _DRAM_INFO_BY_MRR_T
{
    U16 u2MR5VendorID;
    U16 u2MR6RevisionID;
    U64 u8MR8RankSize[RANK_MAX];
} DRAM_INFO_BY_MRR_T;

#if __FLASH_TOOL_DA__
typedef struct _DEBUG_PIN_INF_FOR_FLASHTOOL_T
{
    U16 TOTAL_ERR;//DQ,CA
    U16 IMP_ERR_FLAG;
    U8 WL_ERR_FLAG;//DQS
    U8 CA_ERR_FLAG[CHANNEL_MAX][RANK_MAX];
    U8 CA_WIN_SIZE[CHANNEL_MAX][RANK_MAX][CATRAINING_NUM_LP4];
    U8 DRAM_PIN_RX_ERR_FLAG[CHANNEL_MAX][RANK_MAX][DQS_NUMBER_LP4];
    U8 DRAM_PIN_TX_ERR_FLAG[CHANNEL_MAX][RANK_MAX][DQS_NUMBER_LP4];
    U8 DQ_RX_ERR_FLAG[CHANNEL_MAX][RANK_MAX][DQS_NUMBER_LP4];
    U8 DQ_TX_ERR_FLAG[CHANNEL_MAX][RANK_MAX][DQS_NUMBER_LP4];
    U16 DQ_RX_WIN_SIZE[CHANNEL_MAX][RANK_MAX][DQ_DATA_WIDTH_LP4];
    U8 DQ_TX_WIN_SIZE[CHANNEL_MAX][RANK_MAX][DQ_DATA_WIDTH_LP4];
} DEBUG_PIN_INF_FOR_FLASHTOOL_T;
#endif
typedef struct _VCORE_DELAYCELL_T
{
    U32 u2Vcore;
    U16 u2DelayCell;
} VCORE_DELAYCELL_T;

typedef enum
{
    DMA_PREPARE_DATA_ONLY,
    DMA_CHECK_DATA_ACCESS_ONLY_AND_NO_WAIT,
    DMA_CHECK_COMAPRE_RESULT_ONLY,
    DMA_CHECK_DATA_ACCESS_AND_COMPARE,
} DRAM_DMA_CHECK_RESULT_T;


//For new register access
#define SHIFT_TO_CHB_ADDR       ((U32)CHANNEL_B << POS_BANK_NUM)
#if (CHANNEL_NUM > 2)
#define SHIFT_TO_CHC_ADDR       ((U32)CHANNEL_C << POS_BANK_NUM)
#define SHIFT_TO_CHD_ADDR       ((U32)CHANNEL_D << POS_BANK_NUM)
#endif
#define DRAMC_REG_ADDR(offset)  ((p->channel << POS_BANK_NUM) + (offset))
#define SYS_REG_ADDR(offset)    (offset)

// Different from Pi_calibration.c due to Base address
//#define mcSET_DRAMC_REG_ADDR(offset)    (DRAMC_BASE_ADDRESS | (p->channel << POS_BANK_NUM) | (offset))
#define mcSET_SYS_REG_ADDR(offset)    (DRAMC_BASE_ADDRESS | (offset))
#define mcSET_DRAMC_NAO_REG_ADDR(offset)   (DRAMC_NAO_BASE_ADDRESS | (offset))
#define mcSET_DRAMC_AO_REG_ADDR(offset)   (DRAMC_AO_BASE_ADDRESS | (offset))
//#define mcSET_DRAMC_AO_REG_ADDR_CHC(offset)   ((DRAMC_AO_BASE_ADDRESS + ((U32)CHANNEL_C << POS_BANK_NUM)) | (offset))
#define mcSET_DDRPHY_REG_ADDR(offset) (DDRPHY_BASE_ADDR | (offset))
#define mcSET_DDRPHY_REG_ADDR_CHA(offset) ((DDRPHY_BASE_ADDR + ((U32) CHANNEL_A << POS_BANK_NUM)) | (offset))
#define mcSET_DDRPHY_REG_ADDR_CHB(offset) ((DDRPHY_BASE_ADDR + SHIFT_TO_CHB_ADDR) | (offset))
//#define mcSET_DDRPHY_REG_ADDR_CHC(offset) ((DDRPHY_BASE_ADDR + ((U32) CHANNEL_C << POS_BANK_NUM)) | (offset))
//#define mcSET_DDRPHY_REG_ADDR_CHD(offset) ((DDRPHY_BASE_ADDR + ((U32) CHANNEL_D << POS_BANK_NUM)) | (offset))

//--------------------------------------------------------------------------
//              Dram Mode Registers Operation
//--------------------------------------------------------------------------
#define MRWriteFldMulti(p, mr_idx, list, UpdateMode)                        \
{                                                                           \
	UINT16 upk = 1; \
	U8 msk = (U8)(list); \
	{                                                                       \
        upk = 0; \
    	DramcMRWriteFldMsk(p, mr_idx, (U8)(list), msk, UpdateMode); \
	}                                                                       \
}

#define JUST_TO_GLOBAL_VALUE (0)
#define TO_MR     (1)

// LP5 MR30
#define MR30_DCAU            (Fld(4, 4)) // DCA for upper byte
#define MR30_DCAL            (Fld(4, 0)) // DCA for lower byte

// LP5 MR26
#define MR26_DCMU1           (Fld(1, 5))
#define MR26_DCMU0           (Fld(1, 4))
#define MR26_DCML1           (Fld(1, 3))
#define MR26_DCML0           (Fld(1, 2))
#define MR26_DCM_FLIP        (Fld(1, 1))
#define MR26_DCM_START_STOP  (Fld(1, 0))

// LP4 MR13
#define MR13_FSP_OP          (Fld(1, 7))
#define MR13_FSP_WR          (Fld(1, 6))
#define MR13_DMD             (Fld(1, 5))
#define MR13_PRO             (Fld(1, 4))
#define MR13_VRCG            (Fld(1, 3))
#define MR13_CBT             (Fld(1, 0))

#define MR16_FSP_WR_SHIFT    (0)
#define MR16_FSP_OP_SHIFT    (2)
#define MR16_FSP_CBT         (4)
#define MR16_VRCG            (6)
#define MR16_CBT_PHASE       (7)

/***********************************************************************/
/*              External declarations                                  */
/***********************************************************************/
EXTERN DRAMC_CTX_T *psCurrDramCtx;
#if QT_GUI_Tool
EXTERN FILE *fp_A60868;
EXTERN FILE *fp_A60868_RGDump;
#endif
/***********************************************************************/
/*              Public Functions                                       */
/***********************************************************************/
// basic function
EXTERN U8 u1IsLP4Family(DRAM_DRAM_TYPE_T dram_type);
EXTERN int Init_DRAM(DRAM_DRAM_TYPE_T dram_type, DRAM_CBT_MODE_EXTERN_T dram_cbt_mode_extern, DRAM_INFO_BY_MRR_T *DramInfo, U8 get_mdl_used);
EXTERN void Dramc_DDR_Reserved_Mode_setting(void);
EXTERN void Dramc_DDR_Reserved_Mode_AfterSR(void);
EXTERN void Before_Init_DRAM_While_Reserve_Mode_fail(DRAM_DRAM_TYPE_T dram_type);
EXTERN void ShuffleDfsToFSP1(DRAMC_CTX_T *p);

void vSetVcoreByFreq(DRAMC_CTX_T *p);
U32 Get_WL_by_MR_LP4(U8 Version, U8 MR_WL_field_value);
U8 u1MCK2UI_DivShift(DRAMC_CTX_T *p);
void DramcDFSDirectJump_SRAMShuRGMode(DRAMC_CTX_T *p, U8 shu_level);
void UpdateDFSTbltoDDR3200(DRAMC_CTX_T *p);
void DFSInitForCalibration(DRAMC_CTX_T *p);
void mdl_setting(DRAMC_CTX_T *p);
void MPLLInit(void);
void DramcCKEDebounce(DRAMC_CTX_T *p);
void DramcModifiedRefreshMode(DRAMC_CTX_T *p);
void XRTRTR_SHU_Setting(DRAMC_CTX_T * p);
void TXPICGSetting(DRAMC_CTX_T * p);
void XRTWTW_SHU_Setting(DRAMC_CTX_T * p);
DRAM_STATUS_T DramcDualRankRxdatlatCal(DRAMC_CTX_T *p);
void vSwitchWriteDBISettings(DRAMC_CTX_T *p, U8 u1OnOff);
void Get_RX_DelayCell(DRAMC_CTX_T *p);
void DramcRxdqsGatingPostProcess(DRAMC_CTX_T *p);
void EnableDFSNoQueueFlush(DRAMC_CTX_T *p);
void EnableDramcPhyDCMNonShuffle(DRAMC_CTX_T *p, bool bEn);
void Enable_TxWDQS(DRAMC_CTX_T *p, U32 u4DDRPhyShuOffset, U16 u2Freq);
void Enable_ClkTxRxLatchEn(DRAMC_CTX_T *p, U32 u4DDRPhyShuOffset);
void EnableRxDcmDPhy(DRAMC_CTX_T *p, U32 u4DDRPhyShuOffset, U16 u2Freq);
void DramcRxdqsGatingPreProcess(DRAMC_CTX_T *p);
void LP4_single_end_DRAMC_post_config(DRAMC_CTX_T *p, U8 LP4Y_EN);
void vResetDelayChainBeforeCalibration(DRAMC_CTX_T *p);

unsigned int dramc_set_vcore_voltage(unsigned int vcore);
unsigned int dramc_get_vcore_voltage(void);
unsigned int dramc_set_vdram_voltage(unsigned int ddr_type, unsigned int vdram);
unsigned int dramc_get_vdram_voltage(unsigned int ddr_type);
unsigned int dramc_set_vddq_voltage(unsigned int ddr_type, unsigned int vddq);
unsigned int dramc_get_vddq_voltage(unsigned int ddr_type);
unsigned int dramc_set_vmddr_voltage(unsigned int vmddr);
unsigned int dramc_get_vmddr_voltage(void);
unsigned int dramc_set_vio18_voltage(unsigned int vio18);
unsigned int dramc_get_vio18_voltage(void);

void sv_algorithm_assistance_LP4_4266(DRAMC_CTX_T *p);

void DramcNewDutyCalibration(DRAMC_CTX_T *p);
unsigned int mt_get_dram_type_from_hw_trap(void);
U8 Get_MDL_Used_Flag(void);
void Set_MDL_Used_Flag(U8 value);
void SetMr13VrcgToNormalOperation(DRAMC_CTX_T *p);
void cbt_dfs_mr13_global(DRAMC_CTX_T *p, U8 freq);
void TX_Path_Algorithm(DRAMC_CTX_T *p);
DRAM_PLL_FREQ_SEL_T GetSelByFreq(DRAMC_CTX_T *p, U16 u2freq);
U32 Get_RL_by_MR_LP4(U8 BYTE_MODE_EN,U8 DBI_EN, U8 MR_RL_field_value);

#endif // _PI_API_H
