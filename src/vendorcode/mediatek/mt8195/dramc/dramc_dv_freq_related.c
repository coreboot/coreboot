/* SPDX-License-Identifier: BSD-3-Clause */

//#include "svdpi.h"
//#include "vc_hdrs.h"
#include "dramc_common.h"
#include "dramc_int_global.h"
#include "x_hal_io.h"
//#include "./dramc_actiming.h"
#include "dramc_dv_init.h"


#if 0
    #include "./drivers/DRAMC_SUBSYS_config.c"
    #include "./drivers/ANA_init_config.c"
    #include "./drivers/DIG_NONSHUF_config.c"
    #include "./drivers/DRAM_config_collctioin.c"
    #include "./drivers/LP4_dram_init.c"
    #include "./drivers/LP5_dram_init.c"
    #include "./drivers/DIG_SHUF_config.c"
#endif

#if 0
void CInit_ConfigFromTBA(void)
{
    TbaConfig.dramc_dram_ratio  = 4;
    TbaConfig.channel           = 0;
    TbaConfig.dram_type         = 4;
    TbaConfig.shu_type          = 0;
    TbaConfig.dram_cbt_mode_RK0 = 1;
    TbaConfig.dram_cbt_mode_RK1 = 1;
    TbaConfig.DBI_R_onoff_FSP0  = 0;
    TbaConfig.DBI_R_onoff_FSP1  = 0;
    TbaConfig.frequency         = 1600;
    TbaConfig.voltage_state     = 3;
    mcSHOW_DBG_MSG(("[TBA_CTX_T] Global: dramc_dram_ratio  = %1d\n", TbaConfig.dramc_dram_ratio));
    mcSHOW_DBG_MSG(("[TBA_CTX_T] Global: channel           = %1d\n", TbaConfig.channel));
    mcSHOW_DBG_MSG(("[TBA_CTX_T] Global: dram_type         = %1d\n", TbaConfig.dram_type));
    mcSHOW_DBG_MSG(("[TBA_CTX_T] Global: shu_type          = %1d\n", TbaConfig.shu_type));
    mcSHOW_DBG_MSG(("[TBA_CTX_T] Global: dram_cbt_mode_RK0 = %1d\n", TbaConfig.dram_cbt_mode_RK0));
    mcSHOW_DBG_MSG(("[TBA_CTX_T] Global: dram_cbt_mode_RK1 = %1d\n", TbaConfig.dram_cbt_mode_RK1));
    mcSHOW_DBG_MSG(("[TBA_CTX_T] Global: DBI_R_onoff_FSP0  = %1d\n", TbaConfig.DBI_R_onoff_FSP0));
    mcSHOW_DBG_MSG(("[TBA_CTX_T] Global: DBI_R_onoff_FSP1  = %1d\n", TbaConfig.DBI_R_onoff_FSP1));
    mcSHOW_DBG_MSG(("[TBA_CTX_T] Global: frequency         = %1d\n", TbaConfig.frequency));
    mcSHOW_DBG_MSG(("[TBA_CTX_T] Global: voltage_state     = %1d\n", TbaConfig.voltage_state));


    TbaEnvConfig.TMRRI_way             = 1;
    TbaEnvConfig.RUNTIMEMRR_way        = 1;
    TbaEnvConfig.PICG_MODE             = 1;
    TbaEnvConfig.LP_NEW8X_SEQ_MODE     = 0;
    TbaEnvConfig.DDR400_OPEN_LOOP_MODE = 0;
    TbaEnvConfig.RXTRACK_PBYTE_OPT     = 0;
    TbaEnvConfig.RG_SPM_MODE           = 0;
    TbaEnvConfig.TX_OE_EXT_OPT         = 0;
    TbaEnvConfig.PLL_MODE_OPTION       = 1;
    TbaEnvConfig.TREFBWIG_IGNORE       = 1;
    TbaEnvConfig.SHUFFLE_LVL_MODE      = 1;
    TbaEnvConfig.SELPH_MODE            = 1;
    TbaEnvConfig.TRACK_UP_MODE         = 1;
    TbaEnvConfig.VALID_LAT_VALUE       = 0;
    TbaEnvConfig.NEW_RANK_MODE         = 1;
    TbaEnvConfig.WPST1P5T_OPT          = 1;
    TbaEnvConfig.TXP_WORKAROUND_OPT    = 0;
    TbaEnvConfig.DLL_IDLE_MODE         = 1;
    mcSHOW_DBG_MSG(("[TBA_dramc_env_config_T] Global: TMRRI_way             = %1d\n", TbaEnvConfig.TMRRI_way));
    mcSHOW_DBG_MSG(("[TBA_dramc_env_config_T] Global: RUNTIMEMRR_way        = %1d\n", TbaEnvConfig.RUNTIMEMRR_way));
    mcSHOW_DBG_MSG(("[TBA_dramc_env_config_T] Global: PICG_MODE             = %1d\n", TbaEnvConfig.PICG_MODE));
    mcSHOW_DBG_MSG(("[TBA_dramc_env_config_T] Global: LP_NEW8X_SEQ_MODE     = %1d\n", TbaEnvConfig.LP_NEW8X_SEQ_MODE));
    mcSHOW_DBG_MSG(("[TBA_dramc_env_config_T] Global: DDR400_OPEN_LOOP_MODE = %1d\n", TbaEnvConfig.DDR400_OPEN_LOOP_MODE));
    mcSHOW_DBG_MSG(("[TBA_dramc_env_config_T] Global: RXTRACK_PBYTE_OPT     = %1d\n", TbaEnvConfig.RXTRACK_PBYTE_OPT));
    mcSHOW_DBG_MSG(("[TBA_dramc_env_config_T] Global: RG_SPM_MODE           = %1d\n", TbaEnvConfig.RG_SPM_MODE));
    mcSHOW_DBG_MSG(("[TBA_dramc_env_config_T] Global: TX_OE_EXT_OPT         = %1d\n", TbaEnvConfig.TX_OE_EXT_OPT));
    mcSHOW_DBG_MSG(("[TBA_dramc_env_config_T] Global: PLL_MODE_OPTION       = %1d\n", TbaEnvConfig.PLL_MODE_OPTION));
    mcSHOW_DBG_MSG(("[TBA_dramc_env_config_T] Global: TREFBWIG_IGNORE       = %1d\n", TbaEnvConfig.TREFBWIG_IGNORE));
    mcSHOW_DBG_MSG(("[TBA_dramc_env_config_T] Global: SHUFFLE_LVL_MODE      = %1d\n", TbaEnvConfig.SHUFFLE_LVL_MODE));
    mcSHOW_DBG_MSG(("[TBA_dramc_env_config_T] Global: SELPH_MODE            = %1d\n", TbaEnvConfig.SELPH_MODE));
    mcSHOW_DBG_MSG(("[TBA_dramc_env_config_T] Global: TRACK_UP_MODE         = %1d\n", TbaEnvConfig.TRACK_UP_MODE));
    mcSHOW_DBG_MSG(("[TBA_dramc_env_config_T] Global: VALID_LAT_VALUE       = %1d\n", TbaEnvConfig.VALID_LAT_VALUE));
    mcSHOW_DBG_MSG(("[TBA_dramc_env_config_T] Global: NEW_RANK_MODE         = %1d\n", TbaEnvConfig.NEW_RANK_MODE));
    mcSHOW_DBG_MSG(("[TBA_dramc_env_config_T] Global: WPST1P5T_OPT          = %1d\n", TbaEnvConfig.WPST1P5T_OPT));
    mcSHOW_DBG_MSG(("[TBA_dramc_env_config_T] Global: TXP_WORKAROUND_OPT    = %1d\n", TbaEnvConfig.TXP_WORKAROUND_OPT));
    mcSHOW_DBG_MSG(("[TBA_dramc_env_config_T] Global: DLL_IDLE_MODE         = %1d\n", TbaEnvConfig.DLL_IDLE_MODE));

    TBA_Build_Def.DQSG_TRACK_on = 1;
    mcSHOW_DBG_MSG(("[TBA_build_def_T] Global: DQSG_TRACK_on = %1d\n", TBA_Build_Def.DQSG_TRACK_on));


    TbaEnvDVFSCfg.group_num             = 2;
    TbaEnvDVFSCfg.LP5BGOTF              = 0;
    TbaEnvDVFSCfg.LP5BGEN               = 0;
    TbaEnvDVFSCfg.CAS_MODE              = 3;
    TbaEnvDVFSCfg.LP5_RDQS_SE_EN        = 0;
    TbaEnvDVFSCfg.DQSIEN_MODE           = 1;
    TbaEnvDVFSCfg.freq_group_map_0      = 2;
    TbaEnvDVFSCfg.freq_group_map_1      = 6;
    mcSHOW_DBG_MSG(("[TBA_dramc_env_dvfs_config_T] Global: group_num        = %1d\n", TbaEnvDVFSCfg.group_num));
    mcSHOW_DBG_MSG(("[TBA_dramc_env_dvfs_config_T] Global: LP5BGOTF         = %1d\n", TbaEnvDVFSCfg.LP5BGOTF));
    mcSHOW_DBG_MSG(("[TBA_dramc_env_dvfs_config_T] Global: LP5BGEN          = %1d\n", TbaEnvDVFSCfg.LP5BGEN));
    mcSHOW_DBG_MSG(("[TBA_dramc_env_dvfs_config_T] Global: CAS_MODE         = %1d\n", TbaEnvDVFSCfg.CAS_MODE));
    mcSHOW_DBG_MSG(("[TBA_dramc_env_dvfs_config_T] Global: LP5_RDQS_SE_EN   = %1d\n", TbaEnvDVFSCfg.LP5_RDQS_SE_EN));
    mcSHOW_DBG_MSG(("[TBA_dramc_env_dvfs_config_T] Global: DQSIEN_MODE      = %1d\n", TbaEnvDVFSCfg.DQSIEN_MODE));
    mcSHOW_DBG_MSG(("[TBA_dramc_env_dvfs_config_T] Global: freq_group_map_0 = %1d\n", TbaEnvDVFSCfg.freq_group_map_0));
    mcSHOW_DBG_MSG(("[TBA_dramc_env_dvfs_config_T] Global: freq_group_map_1 = %1d\n", TbaEnvDVFSCfg.freq_group_map_1));


    TbaEnvHwFuncCfg.HW_MR4_OPTION_RANDOM             = 1;
    TbaEnvHwFuncCfg.HW_DMYRD_OPTION_RANDOM           = 1;
    TbaEnvHwFuncCfg.HW_DQSOSC_OPTION_RANDOM          = 1;
    TbaEnvHwFuncCfg.HW_ZQ_OPTION_RANDOM              = 1;
    TbaEnvHwFuncCfg.RX_INPUT_TRACK_OPTION_RANDOM     = 1;
    TbaEnvHwFuncCfg.DQSG_RUNTIME_DEBUG_OPTION_RANDOM = 1;
    TbaEnvHwFuncCfg.HW_MR4_EN                        = 1;
    TbaEnvHwFuncCfg.HW_DMYRD_EN                      = 1;
    TbaEnvHwFuncCfg.HW_DQSOSC_EN                     = 1;
    TbaEnvHwFuncCfg.HW_ZQ_EN                         = 1;
    TbaEnvHwFuncCfg.DQSG_RETRY_EN                    = 0;
    TbaEnvHwFuncCfg.RX_INPUT_TRACK_EN                = 1;
    TbaEnvHwFuncCfg.DQSG_RUNTIME_DEBUG_EN            = 1;
    TbaEnvHwFuncCfg.RDSEL_TRACK_EN                   = 0;
    TbaEnvHwFuncCfg.DQSG_TRACK_EN                    = 0;
    mcSHOW_DBG_MSG(("[dramc_env_hw_func_config_T] Global: HW_MR4_OPTION_RANDOM             = %1d\n", TbaEnvHwFuncCfg.HW_MR4_OPTION_RANDOM));
    mcSHOW_DBG_MSG(("[dramc_env_hw_func_config_T] Global: HW_DMYRD_OPTION_RANDOM           = %1d\n", TbaEnvHwFuncCfg.HW_DMYRD_OPTION_RANDOM));
    mcSHOW_DBG_MSG(("[dramc_env_hw_func_config_T] Global: HW_DQSOSC_OPTION_RANDOM          = %1d\n", TbaEnvHwFuncCfg.HW_DQSOSC_OPTION_RANDOM));
    mcSHOW_DBG_MSG(("[dramc_env_hw_func_config_T] Global: HW_ZQ_OPTION_RANDOM              = %1d\n", TbaEnvHwFuncCfg.HW_ZQ_OPTION_RANDOM));
    mcSHOW_DBG_MSG(("[dramc_env_hw_func_config_T] Global: RX_INPUT_TRACK_OPTION_RANDOM     = %1d\n", TbaEnvHwFuncCfg.RX_INPUT_TRACK_OPTION_RANDOM));
    mcSHOW_DBG_MSG(("[dramc_env_hw_func_config_T] Global: DQSG_RUNTIME_DEBUG_OPTION_RANDOM = %1d\n", TbaEnvHwFuncCfg.DQSG_RUNTIME_DEBUG_OPTION_RANDOM));
    mcSHOW_DBG_MSG(("[dramc_env_hw_func_config_T] Global: HW_MR4_EN                        = %1d\n", TbaEnvHwFuncCfg.HW_MR4_EN));
    mcSHOW_DBG_MSG(("[dramc_env_hw_func_config_T] Global: HW_DMYRD_EN                      = %1d\n", TbaEnvHwFuncCfg.HW_DMYRD_EN));
    mcSHOW_DBG_MSG(("[dramc_env_hw_func_config_T] Global: HW_DQSOSC_EN                     = %1d\n", TbaEnvHwFuncCfg.HW_DQSOSC_EN));
    mcSHOW_DBG_MSG(("[dramc_env_hw_func_config_T] Global: HW_ZQ_EN                         = %1d\n", TbaEnvHwFuncCfg.HW_ZQ_EN));
    mcSHOW_DBG_MSG(("[dramc_env_hw_func_config_T] Global: DQSG_RETRY_EN                    = %1d\n", TbaEnvHwFuncCfg.DQSG_RETRY_EN));
    mcSHOW_DBG_MSG(("[dramc_env_hw_func_config_T] Global: RX_INPUT_TRACK_EN                = %1d\n", TbaEnvHwFuncCfg.RX_INPUT_TRACK_EN));
    mcSHOW_DBG_MSG(("[dramc_env_hw_func_config_T] Global: DQSG_RUNTIME_DEBUG_EN            = %1d\n", TbaEnvHwFuncCfg.DQSG_RUNTIME_DEBUG_EN));
    mcSHOW_DBG_MSG(("[dramc_env_hw_func_config_T] Global: RDSEL_TRACK_EN                   = %1d\n", TbaEnvHwFuncCfg.RDSEL_TRACK_EN));
    mcSHOW_DBG_MSG(("[dramc_env_hw_func_config_T] Global: DQSG_TRACK_EN                    = %1d\n", TbaEnvHwFuncCfg.DQSG_TRACK_EN));


    TbaTestListDef.RandDefaultData_on      = 1;
    TbaTestListDef.DQS2DQ_TRACK_on         = 1;
    TbaTestListDef.LPDDR4_EN               = 1;
    TbaTestListDef.ESL_LOG_GEN             = 1;
    TbaTestListDef.LP4_X8_on               = 0;
    TbaTestListDef.LP4_X8_mix_on           = 0;
    TbaTestListDef.INCLUDE_LP45_COMBO_APHY = 1;
    TbaTestListDef.LP45_COMBO_APHY_EN      = 1;
    TbaTestListDef.LPDDR5_EN               = 0;
    TbaTestListDef.LP3_4_PINMUX_EN         = 0;
    mcSHOW_DBG_MSG(("[TBA_TestList_Def_T] Global: RandDefaultData_on      = %1d\n", TbaTestListDef.RandDefaultData_on));
    mcSHOW_DBG_MSG(("[TBA_TestList_Def_T] Global: DQS2DQ_TRACK_on         = %1d\n", TbaTestListDef.DQS2DQ_TRACK_on));
    mcSHOW_DBG_MSG(("[TBA_TestList_Def_T] Global: LPDDR4_EN               = %1d\n", TbaTestListDef.LPDDR4_EN));
    mcSHOW_DBG_MSG(("[TBA_TestList_Def_T] Global: ESL_LOG_GEN             = %1d\n", TbaTestListDef.ESL_LOG_GEN));
    mcSHOW_DBG_MSG(("[TBA_TestList_Def_T] Global: LP4_X8_on               = %1d\n", TbaTestListDef.LP4_X8_on));
    mcSHOW_DBG_MSG(("[TBA_TestList_Def_T] Global: INCLUDE_LP45_COMBO_APHY = %1d\n", TbaTestListDef.INCLUDE_LP45_COMBO_APHY));
    mcSHOW_DBG_MSG(("[TBA_TestList_Def_T] Global: LP45_COMBO_APHY_EN      = %1d\n", TbaTestListDef.LP45_COMBO_APHY_EN));
    mcSHOW_DBG_MSG(("[TBA_TestList_Def_T] Global: LPDDR5_EN               = %1d\n", TbaTestListDef.LPDDR5_EN));
    mcSHOW_DBG_MSG(("[TBA_TestList_Def_T] Global: LP3_4_PINMUX_EN         = %1d\n", TbaTestListDef.LP3_4_PINMUX_EN));



    TbaDramcBenchConfig.rank_swap                = 0;
    TbaDramcBenchConfig.new_uP_spm_mode          = 0;
    TbaDramcBenchConfig.LP_MTCMOS_CONTROL_SEL    = 0;

    TbaDramcBenchConfig.cha_pinmux_anti_order_0  = 0;
    TbaDramcBenchConfig.cha_pinmux_anti_order_1  = 1;
    TbaDramcBenchConfig.cha_pinmux_anti_order_2  = 2;
    TbaDramcBenchConfig.cha_pinmux_anti_order_3  = 3;
    TbaDramcBenchConfig.cha_pinmux_anti_order_4  = 4;
    TbaDramcBenchConfig.cha_pinmux_anti_order_5  = 5;
    TbaDramcBenchConfig.cha_pinmux_anti_order_6  = 6;
    TbaDramcBenchConfig.cha_pinmux_anti_order_7  = 7;

    TbaDramcBenchConfig.cha_pinmux_anti_order_8  = 8;
    TbaDramcBenchConfig.cha_pinmux_anti_order_9  = 9;
    TbaDramcBenchConfig.cha_pinmux_anti_order_10 = 10;
    TbaDramcBenchConfig.cha_pinmux_anti_order_11 = 11;
    TbaDramcBenchConfig.cha_pinmux_anti_order_12 = 12;
    TbaDramcBenchConfig.cha_pinmux_anti_order_13 = 13;
    TbaDramcBenchConfig.cha_pinmux_anti_order_14 = 14;
    TbaDramcBenchConfig.cha_pinmux_anti_order_15 = 15;

    TbaDramcBenchConfig.chb_pinmux_anti_order_0  = 0;
    TbaDramcBenchConfig.chb_pinmux_anti_order_1  = 1;
    TbaDramcBenchConfig.chb_pinmux_anti_order_2  = 2;
    TbaDramcBenchConfig.chb_pinmux_anti_order_3  = 3;
    TbaDramcBenchConfig.chb_pinmux_anti_order_4  = 4;
    TbaDramcBenchConfig.chb_pinmux_anti_order_5  = 5;
    TbaDramcBenchConfig.chb_pinmux_anti_order_6  = 6;
    TbaDramcBenchConfig.chb_pinmux_anti_order_7  = 7;

    TbaDramcBenchConfig.chb_pinmux_anti_order_8  = 8;
    TbaDramcBenchConfig.chb_pinmux_anti_order_9  = 9;
    TbaDramcBenchConfig.chb_pinmux_anti_order_10 = 10;
    TbaDramcBenchConfig.chb_pinmux_anti_order_11 = 11;
    TbaDramcBenchConfig.chb_pinmux_anti_order_12 = 12;
    TbaDramcBenchConfig.chb_pinmux_anti_order_13 = 13;
    TbaDramcBenchConfig.chb_pinmux_anti_order_14 = 14;
    TbaDramcBenchConfig.chb_pinmux_anti_order_15 = 15;

    mcSHOW_DBG_MSG(("[TBA_dramc_bench_T] Global: rank_swap                = %1d\n", TbaDramcBenchConfig.rank_swap));
    mcSHOW_DBG_MSG(("[TBA_dramc_bench_T] Global: new_uP_spm_mode          = %1d\n", TbaDramcBenchConfig.new_uP_spm_mode));
    mcSHOW_DBG_MSG(("[TBA_dramc_bench_T] Global: LP_MTCMOS_CONTROL_SEL    = %1d\n", TbaDramcBenchConfig.LP_MTCMOS_CONTROL_SEL));

    mcSHOW_DBG_MSG(("[TBA_dramc_bench_T] Global: cha_pinmux_anti_order_0  = %1d\n", TbaDramcBenchConfig.cha_pinmux_anti_order_0));
    mcSHOW_DBG_MSG(("[TBA_dramc_bench_T] Global: cha_pinmux_anti_order_1  = %1d\n", TbaDramcBenchConfig.cha_pinmux_anti_order_1));
    mcSHOW_DBG_MSG(("[TBA_dramc_bench_T] Global: cha_pinmux_anti_order_2  = %1d\n", TbaDramcBenchConfig.cha_pinmux_anti_order_2));
    mcSHOW_DBG_MSG(("[TBA_dramc_bench_T] Global: cha_pinmux_anti_order_3  = %1d\n", TbaDramcBenchConfig.cha_pinmux_anti_order_3));
    mcSHOW_DBG_MSG(("[TBA_dramc_bench_T] Global: cha_pinmux_anti_order_4  = %1d\n", TbaDramcBenchConfig.cha_pinmux_anti_order_4));
    mcSHOW_DBG_MSG(("[TBA_dramc_bench_T] Global: cha_pinmux_anti_order_5  = %1d\n", TbaDramcBenchConfig.cha_pinmux_anti_order_5));
    mcSHOW_DBG_MSG(("[TBA_dramc_bench_T] Global: cha_pinmux_anti_order_6  = %1d\n", TbaDramcBenchConfig.cha_pinmux_anti_order_6));
    mcSHOW_DBG_MSG(("[TBA_dramc_bench_T] Global: cha_pinmux_anti_order_7  = %1d\n", TbaDramcBenchConfig.cha_pinmux_anti_order_7));

    mcSHOW_DBG_MSG(("[TBA_dramc_bench_T] Global: cha_pinmux_anti_order_8  = %1d\n", TbaDramcBenchConfig.cha_pinmux_anti_order_8));
    mcSHOW_DBG_MSG(("[TBA_dramc_bench_T] Global: cha_pinmux_anti_order_9  = %1d\n", TbaDramcBenchConfig.cha_pinmux_anti_order_9));
    mcSHOW_DBG_MSG(("[TBA_dramc_bench_T] Global: cha_pinmux_anti_order_10 = %1d\n", TbaDramcBenchConfig.cha_pinmux_anti_order_10));
    mcSHOW_DBG_MSG(("[TBA_dramc_bench_T] Global: cha_pinmux_anti_order_11 = %1d\n", TbaDramcBenchConfig.cha_pinmux_anti_order_11));
    mcSHOW_DBG_MSG(("[TBA_dramc_bench_T] Global: cha_pinmux_anti_order_12 = %1d\n", TbaDramcBenchConfig.cha_pinmux_anti_order_12));
    mcSHOW_DBG_MSG(("[TBA_dramc_bench_T] Global: cha_pinmux_anti_order_13 = %1d\n", TbaDramcBenchConfig.cha_pinmux_anti_order_13));
    mcSHOW_DBG_MSG(("[TBA_dramc_bench_T] Global: cha_pinmux_anti_order_14 = %1d\n", TbaDramcBenchConfig.cha_pinmux_anti_order_14));
    mcSHOW_DBG_MSG(("[TBA_dramc_bench_T] Global: cha_pinmux_anti_order_15 = %1d\n", TbaDramcBenchConfig.cha_pinmux_anti_order_15));

    mcSHOW_DBG_MSG(("[TBA_dramc_bench_T] Global: chb_pinmux_anti_order_0  = %1d\n", TbaDramcBenchConfig.chb_pinmux_anti_order_0));
    mcSHOW_DBG_MSG(("[TBA_dramc_bench_T] Global: chb_pinmux_anti_order_1  = %1d\n", TbaDramcBenchConfig.chb_pinmux_anti_order_1));
    mcSHOW_DBG_MSG(("[TBA_dramc_bench_T] Global: chb_pinmux_anti_order_2  = %1d\n", TbaDramcBenchConfig.chb_pinmux_anti_order_2));
    mcSHOW_DBG_MSG(("[TBA_dramc_bench_T] Global: chb_pinmux_anti_order_3  = %1d\n", TbaDramcBenchConfig.chb_pinmux_anti_order_3));
    mcSHOW_DBG_MSG(("[TBA_dramc_bench_T] Global: chb_pinmux_anti_order_4  = %1d\n", TbaDramcBenchConfig.chb_pinmux_anti_order_4));
    mcSHOW_DBG_MSG(("[TBA_dramc_bench_T] Global: chb_pinmux_anti_order_5  = %1d\n", TbaDramcBenchConfig.chb_pinmux_anti_order_5));
    mcSHOW_DBG_MSG(("[TBA_dramc_bench_T] Global: chb_pinmux_anti_order_6  = %1d\n", TbaDramcBenchConfig.chb_pinmux_anti_order_6));
    mcSHOW_DBG_MSG(("[TBA_dramc_bench_T] Global: chb_pinmux_anti_order_7  = %1d\n", TbaDramcBenchConfig.chb_pinmux_anti_order_7));

    mcSHOW_DBG_MSG(("[TBA_dramc_bench_T] Global: chb_pinmux_anti_order_8  = %1d\n", TbaDramcBenchConfig.chb_pinmux_anti_order_8));
    mcSHOW_DBG_MSG(("[TBA_dramc_bench_T] Global: chb_pinmux_anti_order_9  = %1d\n", TbaDramcBenchConfig.chb_pinmux_anti_order_9));
    mcSHOW_DBG_MSG(("[TBA_dramc_bench_T] Global: chb_pinmux_anti_order_10 = %1d\n", TbaDramcBenchConfig.chb_pinmux_anti_order_10));
    mcSHOW_DBG_MSG(("[TBA_dramc_bench_T] Global: chb_pinmux_anti_order_11 = %1d\n", TbaDramcBenchConfig.chb_pinmux_anti_order_11));
    mcSHOW_DBG_MSG(("[TBA_dramc_bench_T] Global: chb_pinmux_anti_order_12 = %1d\n", TbaDramcBenchConfig.chb_pinmux_anti_order_12));
    mcSHOW_DBG_MSG(("[TBA_dramc_bench_T] Global: chb_pinmux_anti_order_13 = %1d\n", TbaDramcBenchConfig.chb_pinmux_anti_order_13));
    mcSHOW_DBG_MSG(("[TBA_dramc_bench_T] Global: chb_pinmux_anti_order_14 = %1d\n", TbaDramcBenchConfig.chb_pinmux_anti_order_14));
    mcSHOW_DBG_MSG(("[TBA_dramc_bench_T] Global: chb_pinmux_anti_order_15 = %1d\n", TbaDramcBenchConfig.chb_pinmux_anti_order_15));

}
#endif

#if BYPASS_CALIBRATION
#if (FOR_DV_SIMULATION_USED==0)
void Apply_LP4_1600_Calibraton_Result(DRAMC_CTX_T *p)
{
    U8 u1RankIdx=0;
    U8 backup_rank=0;

    backup_rank = p->rank;

    ShiftDQUI_AllRK(p, -1, ALL_BYTES);
    ShiftDQ_OENUI_AllRK(p, -1, ALL_BYTES);
    ShiftDQSWCK_UI(p, -1, ALL_BYTES);

    for(u1RankIdx=0; u1RankIdx<p->support_rank_num; u1RankIdx++)
    {
        vSetRank(p, u1RankIdx);

		DramcCmdUIDelaySetting(p, 0);

        vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_CA_CMD0),
    		P_Fld(0x20, SHU_R0_CA_CMD0_RG_ARPI_CMD) |
    		P_Fld(0, SHU_R0_CA_CMD0_RG_ARPI_CLK));
        vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_CA_CMD0), 0, SHU_R0_CA_CMD0_RG_ARPI_CS);


        vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_DQ0), 0x20, SHU_R0_B0_DQ0_ARPI_PBYTE_B0);
        vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_DQ0), 0x20, SHU_R0_B1_DQ0_ARPI_PBYTE_B1);


        if((p->dram_cbt_mode[RANK_0] == CBT_NORMAL_MODE) && (p->dram_cbt_mode[RANK_1] == CBT_NORMAL_MODE))
        {

        	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B0_DQSIEN_MCK_UI_DLY),
        		P_Fld(0,
        		SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B0) |
        		P_Fld(11,
        		SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B0) |
        		P_Fld(0,
        		SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B0) |
        		P_Fld(15,
        		SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B0));

        	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B1_DQSIEN_MCK_UI_DLY),
        		P_Fld(0,
        		SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B1) |
        		P_Fld(11,
        		SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B1) |
        		P_Fld(0,
        		SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B1) |
        		P_Fld(15,
        		SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B1));

            #if GATING_RODT_LATANCY_EN
        	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B0_RODTEN_MCK_UI_DLY),
        		P_Fld(0,
        		SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B0) |
        		P_Fld(0,
        		SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B0) |
        		P_Fld(0,
        		SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B0) |
        		P_Fld(0,
        		SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B0));

        	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B1_RODTEN_MCK_UI_DLY),
        		P_Fld(0,
        		SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B1) |
        		P_Fld(0,
        		SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B1) |
        		P_Fld(0,
        		SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B1) |
        		P_Fld(0,
        		SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B1));
            #endif
        }
        else
        {

        	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B0_DQSIEN_MCK_UI_DLY),
        		P_Fld(0,
        		SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B0) |
        		P_Fld(15,
        		SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B0) |
        		P_Fld(1,
        		SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B0) |
        		P_Fld(3,
        		SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B0));

        	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B1_DQSIEN_MCK_UI_DLY),
        		P_Fld(0,
        		SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B1) |
        		P_Fld(15,
        		SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B1) |
        		P_Fld(1,
        		SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B1) |
        		P_Fld(3,
        		SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B1));
            
            #if GATING_RODT_LATANCY_EN
        	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B0_RODTEN_MCK_UI_DLY),
        		P_Fld(0,
        		SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B0) |
        		P_Fld(4,
        		SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B0) |
        		P_Fld(0,
        		SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B0) |
        		P_Fld(4,
        		SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B0));

        	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B1_RODTEN_MCK_UI_DLY),
        		P_Fld(0,
        		SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B1) |
        		P_Fld(4,
        		SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B1) |
        		P_Fld(0,
        		SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B1) |
        		P_Fld(4,
        		SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B1));
            #endif
        }
        
        vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B0_DQSIEN_PI_DLY),
        	8,
        	SHU_RK_B0_DQSIEN_PI_DLY_DQSIEN_PI_B0);
        vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B1_DQSIEN_PI_DLY),
        	8,
        	SHU_RK_B1_DQSIEN_PI_DLY_DQSIEN_PI_B1);

    	DramPhyReset(p);


        vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_RXDLY5),
                P_Fld((U32)0, SHU_R0_B0_RXDLY5_RX_ARDQS0_R_DLY_B0));
        vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_RXDLY4),
                P_Fld((U32)0x46, SHU_R0_B0_RXDLY4_RX_ARDQM0_R_DLY_B0));
        vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_RXDLY5),
                P_Fld((U32)0, SHU_R0_B1_RXDLY5_RX_ARDQS0_R_DLY_B1));
        vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_RXDLY4),
                P_Fld((U32)0x46, SHU_R0_B1_RXDLY4_RX_ARDQM0_R_DLY_B1));


        U8 u1BitIdx;
        for (u1BitIdx = 0; u1BitIdx < DQS_BIT_NUMBER; u1BitIdx += 2)
        {
             vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_RXDLY0 + u1BitIdx * 2),
                                            P_Fld(((U32)0x46), SHU_R0_B0_RXDLY0_RX_ARDQ0_R_DLY_B0) |
                                            P_Fld(((U32)0x46), SHU_R0_B0_RXDLY0_RX_ARDQ1_R_DLY_B0));

             vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_RXDLY0 + u1BitIdx * 2),
                                            P_Fld((U32)0x46, SHU_R0_B1_RXDLY0_RX_ARDQ0_R_DLY_B1) |
                                            P_Fld((U32)0x46, SHU_R0_B1_RXDLY0_RX_ARDQ1_R_DLY_B1));

            //mcSHOW_DBG_MSG(("u1BitId %d  Addr 0x%2x = %2d %2d %2d %2d \n", u1BitIdx, DDRPHY_RXDQ1+u1BitIdx*2,
            //                FinalWinPerBit[u1BitIdx].best_dqdly, FinalWinPerBit[u1BitIdx+1].best_dqdly,  FinalWinPerBit[u1BitIdx+8].best_dqdly, FinalWinPerBit[u1BitIdx+9].best_dqdly));
        }

        {
            U8 u1TXMCK[4] = {2,2,2,2};
            U8 u1TXOENMCK[4] = {1,1,1,1};
            U8 u1TXUI[4] = {1,1,1,1};
            U8 u1TXOENUI[4] = {5,5,5,5};
            U8 u1TXPI[4] = {30,30,30,30};

            if((p->dram_cbt_mode[RANK_0] == CBT_NORMAL_MODE) && (p->dram_cbt_mode[RANK_1] == CBT_NORMAL_MODE))
            {
                u1TXPI[0] = u1TXPI[1]= u1TXPI[2] = u1TXPI[3] =41;
            }
            else
            {
                u1TXPI[0] = u1TXPI[1]= u1TXPI[2] = u1TXPI[3] =30;
            }
            
            TXSetDelayReg_DQ(p, 1, u1TXMCK, u1TXOENMCK, u1TXUI, u1TXOENUI, u1TXPI);
            TXSetDelayReg_DQM(p, 1, u1TXMCK, u1TXOENMCK, u1TXUI, u1TXOENUI, u1TXPI);


            vIO32Write4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_TXDLY0), 0);
            vIO32Write4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_TXDLY1), 0);
            vIO32Write4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_TXDLY0), 0);
            vIO32Write4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_TXDLY1), 0);
            vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_TXDLY3), 0x0, SHU_R0_B0_TXDLY3_TX_ARDQM0_DLY_B0);
            vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_TXDLY3), 0x0, SHU_R0_B1_TXDLY3_TX_ARDQM0_DLY_B1);


            #if ENABLE_TX_TRACKING
            TXUpdateTXTracking(p, TX_DQ_DQS_MOVE_DQ_ONLY, u1TXPI, u1TXPI);
            #endif
        }

        dle_factor_handler(p, 9);
    }

    vSetRank(p, backup_rank);
}

void Apply_LP4_4266_Calibraton_Result(DRAMC_CTX_T *p)
{
    U8 u1RankIdx=0;

    U8 u1WLB0_Dly=0, u1WLB1_Dly=0;

    U8 u1GatingMCKB0_Dly=0, u1GatingMCKB1_Dly=0;
    U8 u1GatingUIB0_Dly=0, u1GatingUIB1_Dly=0;
    U8 u1GatingPIB0_Dly=0, u1GatingPIB1_Dly=0;
    U8 u1B0RodtMCK=0, u1B1RodtMCK=0;
    U8 u1B0RodtUI=0, u1B1RodtUI=0;

    U8 u1RxDQS0=0, u1RxDQS1=0;
    U8 u1RxDQM0=0, u1RxDQM1=0;
    U8 u1RxRK0B0DQ[8] = {153,147,155,133,149,147,147,143};
    U8 u1RxRK0B1DQ[8] = {163,157,149,143,147,159,151,155};
    U8 u1RxRK1B0DQ[8] = {151,147,149,131,151,147,143,139};
    U8 u1RxRK1B1DQ[8] = {167,159,149,143,151,157,149,157};
    U8 *pRxB0DQ, *pRxB1DQ;
    U8 *pTxDQPi;
    U8 backup_rank=0;

    backup_rank = p->rank;

    ShiftDQUI_AllRK(p, -1, ALL_BYTES);
    ShiftDQ_OENUI_AllRK(p, -1, ALL_BYTES);
    ShiftDQSWCK_UI(p, -1, ALL_BYTES);

    for(u1RankIdx=0; u1RankIdx<p->support_rank_num; u1RankIdx++)
    {
        vSetRank(p, u1RankIdx);

#if 0

        DramcCmdUIDelaySetting(p, 0);
        vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_CA_CMD0),
                                P_Fld(32, SHU_R0_CA_CMD0_RG_ARPI_CMD) |
                                P_Fld(0, SHU_R0_CA_CMD0_RG_ARPI_CLK));
        vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_CA_CMD0), 0, SHU_R0_CA_CMD0_RG_ARPI_CS);
#endif

    #if 1

        if (p->rank == RANK_0)
        {
            u1WLB0_Dly = 29;
            u1WLB1_Dly = 28;
        }
        else
        {
            u1WLB0_Dly = 31;
            u1WLB1_Dly = 24;
        }
        vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_DQ0), u1WLB0_Dly, SHU_R0_B0_DQ0_ARPI_PBYTE_B0);
        vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_DQ0), u1WLB1_Dly, SHU_R0_B1_DQ0_ARPI_PBYTE_B1);


        if((p->dram_cbt_mode[RANK_0] == CBT_NORMAL_MODE) && (p->dram_cbt_mode[RANK_1] == CBT_NORMAL_MODE))
        {
            //if (p->rank == RANK_0)
            {
                u1GatingMCKB0_Dly=0; u1GatingMCKB1_Dly=0;
                u1GatingUIB0_Dly=14; u1GatingUIB1_Dly=14;
                u1GatingPIB0_Dly=8; u1GatingPIB1_Dly=10;
                u1B0RodtMCK=0; u1B1RodtMCK=0;
                u1B0RodtUI=3; u1B1RodtUI=3;
            }
            //else
            {
            //    u1GatingMCKB0_Dly=0; u1GatingMCKB1_Dly=0;
            //    u1GatingUIB0_Dly=14; u1GatingUIB1_Dly=14;
            //    u1GatingPIB0_Dly=4; u1GatingPIB1_Dly=10;
            //    u1B0RodtMCK=0; u1B1RodtMCK=0;
            //    u1B0RodtUI=3; u1B1RodtUI=3;
            }
            
            vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B0_DQSIEN_MCK_UI_DLY),
                P_Fld(u1GatingMCKB0_Dly,
                SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B0) |
                P_Fld(u1GatingUIB0_Dly,
                SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B0) |
                P_Fld(u1GatingMCKB0_Dly+1,
                SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B0) |
                P_Fld(u1GatingUIB0_Dly+4-16,
                SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B0));
            
            vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B1_DQSIEN_MCK_UI_DLY),
                P_Fld(u1GatingMCKB1_Dly,
                SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B1) |
                P_Fld(u1GatingUIB1_Dly,
                SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B1) |
                P_Fld(u1GatingMCKB1_Dly+1,
                SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B1) |
                P_Fld(u1GatingUIB1_Dly+4-16,
                SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B1));
        }
        else
        {
            if (p->rank == RANK_0)
            {
                u1GatingMCKB0_Dly=1; u1GatingMCKB1_Dly=1;
                u1GatingUIB0_Dly=6; u1GatingUIB1_Dly=6;
                u1GatingPIB0_Dly=0; u1GatingPIB1_Dly=4;
                u1B0RodtMCK=1; u1B1RodtMCK=1;
                u1B0RodtUI=3; u1B1RodtUI=3;
            }
            else
            {
                u1GatingMCKB0_Dly=1; u1GatingMCKB1_Dly=1;
                u1GatingUIB0_Dly=5; u1GatingUIB1_Dly=6;
                u1GatingPIB0_Dly=28; u1GatingPIB1_Dly=0;
                u1B0RodtMCK=1; u1B1RodtMCK=1;
                u1B0RodtUI=2; u1B1RodtUI=3;
            }
            
            vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B0_DQSIEN_MCK_UI_DLY),
                P_Fld(u1GatingMCKB0_Dly,
                SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B0) |
                P_Fld(u1GatingUIB0_Dly,
                SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B0) |
                P_Fld(u1GatingMCKB0_Dly,
                SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B0) |
                P_Fld(u1GatingUIB0_Dly+4,
                SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B0));
            
            vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B1_DQSIEN_MCK_UI_DLY),
                P_Fld(u1GatingMCKB1_Dly,
                SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B1) |
                P_Fld(u1GatingUIB1_Dly,
                SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B1) |
                P_Fld(u1GatingMCKB1_Dly,
                SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B1) |
                P_Fld(u1GatingUIB1_Dly+4,
                SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B1));
        }

#if GATING_RODT_LATANCY_EN
            vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B0_RODTEN_MCK_UI_DLY),
                            P_Fld(u1B0RodtMCK,
                            SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B0) |
                            P_Fld(u1B0RodtUI,
                            SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B0) |
                            P_Fld(u1B0RodtMCK,
                            SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B0) |
                            P_Fld(u1B0RodtUI,
                            SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B0));

            vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B1_RODTEN_MCK_UI_DLY),
                            P_Fld(u1B1RodtMCK,
                            SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B1) |
                            P_Fld(u1B1RodtUI,
                            SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B1) |
                            P_Fld(u1B1RodtMCK,
                            SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B1) |
                            P_Fld(u1B1RodtUI,
                            SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B1));
#endif


        vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B0_DQSIEN_PI_DLY),
                u1GatingPIB0_Dly,
                SHU_RK_B0_DQSIEN_PI_DLY_DQSIEN_PI_B0);
        vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B1_DQSIEN_PI_DLY),
                u1GatingPIB1_Dly,
                SHU_RK_B1_DQSIEN_PI_DLY_DQSIEN_PI_B1); 

        DramPhyReset(p);

    #if RDSEL_TRACKING_EN

        vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_INI_UIPI),
        (u1GatingMCKB0_Dly << 3) | (u1GatingUIB0_Dly),
        SHU_R0_B0_INI_UIPI_CURR_INI_UI_B0);
        vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_INI_UIPI), u1GatingPIB0_Dly,
        SHU_R0_B0_INI_UIPI_CURR_INI_PI_B0);

        vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_INI_UIPI),
        (u1GatingMCKB1_Dly << 3) | (u1GatingUIB1_Dly),
        DDRPHY_REG_SHU_R0_B1_INI_UIPI);
        vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_INI_UIPI),
        u1GatingPIB1_Dly, SHU_R0_B1_INI_UIPI_CURR_INI_PI_B1);
    #endif

        if (p->rank == RANK_0)
        {
            u1RxDQS0=0; u1RxDQS1=0;
            u1RxDQM0=146; u1RxDQM1=153;
            pRxB0DQ = u1RxRK0B0DQ;
            pRxB1DQ = u1RxRK0B1DQ;
        }
        else
        {
            u1RxDQS0=0; u1RxDQS1=0;
            u1RxDQM0=144; u1RxDQM1=154;
            pRxB0DQ = u1RxRK1B0DQ;
            pRxB1DQ = u1RxRK1B1DQ;
        }


        vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_RXDLY5),
                P_Fld((U32)u1RxDQS0, SHU_R0_B0_RXDLY5_RX_ARDQS0_R_DLY_B0));
        vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_RXDLY4),
                P_Fld((U32)u1RxDQM0, SHU_R0_B0_RXDLY4_RX_ARDQM0_R_DLY_B0) |
                P_Fld((U32)0, SHU_R0_B0_RXDLY4_RX_ARDQM0_F_DLY_B0));
        vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_RXDLY5),
                P_Fld((U32)u1RxDQS1, SHU_R0_B1_RXDLY5_RX_ARDQS0_R_DLY_B1));
        vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_RXDLY4),
                P_Fld((U32)u1RxDQM1, SHU_R0_B1_RXDLY4_RX_ARDQM0_R_DLY_B1) |
                P_Fld((U32)0, SHU_R0_B1_RXDLY4_RX_ARDQM0_F_DLY_B1));


        U8 u1BitIdx;
        for (u1BitIdx = 0; u1BitIdx < DQS_BIT_NUMBER; u1BitIdx += 2)
        {
              vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_RXDLY0 + u1BitIdx * 2),
                                            P_Fld(((U32)pRxB0DQ[u1BitIdx]), SHU_R0_B0_RXDLY0_RX_ARDQ0_R_DLY_B0) |
                                            P_Fld(((U32)pRxB0DQ[u1BitIdx+1]), SHU_R0_B0_RXDLY0_RX_ARDQ1_R_DLY_B0));

             vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_RXDLY0 + u1BitIdx * 2),
                                            P_Fld((U32)pRxB1DQ[u1BitIdx], SHU_R0_B1_RXDLY0_RX_ARDQ0_R_DLY_B1) |
                                            P_Fld((U32)pRxB1DQ[u1BitIdx+1], SHU_R0_B1_RXDLY0_RX_ARDQ1_R_DLY_B1));

            //mcSHOW_DBG_MSG(("u1BitId %d  Addr 0x%2x = %2d %2d %2d %2d \n", u1BitIdx, DDRPHY_RXDQ1+u1BitIdx*2,
            //                FinalWinPerBit[u1BitIdx].best_dqdly, FinalWinPerBit[u1BitIdx+1].best_dqdly,  FinalWinPerBit[u1BitIdx+8].best_dqdly, FinalWinPerBit[u1BitIdx+9].best_dqdly));
        }

        {
            U8 u1TXMCK[4] = {4,4,4,4};
            U8 u1TXOENMCK[4] = {4,4,4,4};
            U8 u1TXUI[4] = {6,6,6,6};
            U8 u1TXOENUI[4] = {2,2,2,2};
            U8 u1TXRK0PI[4] = {31,34,31,34};
            U8 u1TXRK1PI[4] = {33,28,33,28};

            if((p->dram_cbt_mode[RANK_0] == CBT_NORMAL_MODE) && (p->dram_cbt_mode[RANK_1] == CBT_NORMAL_MODE))
            {
                u1TXRK0PI[0] = u1TXRK0PI[1]= u1TXRK0PI[2] = u1TXRK0PI[3] =53;
                u1TXRK1PI[0] = 45;
                u1TXRK1PI[1]= u1TXRK1PI[2] = u1TXRK1PI[3] =53;
            }

            if (p->rank == RANK_0)
                pTxDQPi = u1TXRK0PI;
            else
                pTxDQPi = u1TXRK1PI;
            
            TXSetDelayReg_DQ(p, 1, u1TXMCK, u1TXOENMCK, u1TXUI, u1TXOENUI, pTxDQPi);
            TXSetDelayReg_DQM(p, 1, u1TXMCK, u1TXOENMCK, u1TXUI, u1TXOENUI, pTxDQPi);


            vIO32Write4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_TXDLY0), 0);
            vIO32Write4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_TXDLY1), 0);
            vIO32Write4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_TXDLY0), 0);
            vIO32Write4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_TXDLY1), 0);
            vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_TXDLY3), 0x0, SHU_R0_B0_TXDLY3_TX_ARDQM0_DLY_B0);
            vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_TXDLY3), 0x0, SHU_R0_B1_TXDLY3_TX_ARDQM0_DLY_B1);

            #if ENABLE_TX_TRACKING
            TXUpdateTXTracking(p, TX_DQ_DQS_MOVE_DQ_ONLY, pTxDQPi, pTxDQPi);
            #endif
        }

    if((p->dram_cbt_mode[RANK_0] == CBT_NORMAL_MODE) && (p->dram_cbt_mode[RANK_1] == CBT_NORMAL_MODE))
        dle_factor_handler(p, 16);
    else
        dle_factor_handler(p, 17);
    #endif
    }

    vSetRank(p, backup_rank);
}
#else
void Apply_LP4_1600_Calibraton_Result(DRAMC_CTX_T *p)
{
    U8 u1RankIdx=0;
    U8 backup_rank=0;

    backup_rank = p->rank;

    ShiftDQUI_AllRK(p, -1, ALL_BYTES);
    ShiftDQ_OENUI_AllRK(p, -1, ALL_BYTES);
    ShiftDQSWCK_UI(p, -1, ALL_BYTES);

    for(u1RankIdx=0; u1RankIdx<p->support_rank_num; u1RankIdx++)
    {
        vSetRank(p, u1RankIdx);

		DramcCmdUIDelaySetting(p, 0);

        vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_CA_CMD0),
    		P_Fld(0x20, SHU_R0_CA_CMD0_RG_ARPI_CMD) |
    		P_Fld(0, SHU_R0_CA_CMD0_RG_ARPI_CLK));
        vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_CA_CMD0), 0, SHU_R0_CA_CMD0_RG_ARPI_CS);


        vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_DQ0), 0x20, SHU_R0_B0_DQ0_ARPI_PBYTE_B0);
        vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_DQ0), 0x20, SHU_R0_B1_DQ0_ARPI_PBYTE_B1);


        if((p->dram_cbt_mode[RANK_0] == CBT_NORMAL_MODE) && (p->dram_cbt_mode[RANK_1] == CBT_NORMAL_MODE))
        {
	    if(p->rank==RANK_0)
	    {

        	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B0_DQSIEN_MCK_UI_DLY),
        		P_Fld(0,
        		SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B0) |
        		P_Fld(9,
        		SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B0) |
        		P_Fld(0,
        		SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B0) |
        		P_Fld(13,
        		SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B0));

        	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B1_DQSIEN_MCK_UI_DLY),
        		P_Fld(0,
        		SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B1) |
        		P_Fld(9,
        		SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B1) |
        		P_Fld(0,
        		SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B1) |
        		P_Fld(13,
        		SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B1));

            #if GATING_RODT_LATANCY_EN
        	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B0_RODTEN_MCK_UI_DLY),
        		P_Fld(0,
        		SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B0) |
        		P_Fld(0,
        		SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B0) |
        		P_Fld(0,
        		SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B0) |
        		P_Fld(0,
        		SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B0));

        	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B1_RODTEN_MCK_UI_DLY),
        		P_Fld(0,
        		SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B1) |
        		P_Fld(0,
        		SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B1) |
        		P_Fld(0,
        		SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B1) |
        		P_Fld(0,
        		SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B1));
            #endif
	    }
	    else
	    {

        	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B0_DQSIEN_MCK_UI_DLY),
        		P_Fld(0,
        		SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B0) |
        		P_Fld(12,
        		SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B0) |
        		P_Fld(1,
        		SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B0) |
        		P_Fld(0,
        		SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B0));

        	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B1_DQSIEN_MCK_UI_DLY),
        		P_Fld(0,
        		SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B1) |
        		P_Fld(12,
        		SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B1) |
        		P_Fld(1,
        		SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B1) |
        		P_Fld(0,
        		SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B1));

            #if GATING_RODT_LATANCY_EN
        	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B0_RODTEN_MCK_UI_DLY),
        		P_Fld(0,
        		SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B0) |
        		P_Fld(1,
        		SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B0) |
        		P_Fld(0,
        		SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B0) |
        		P_Fld(1,
        		SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B0));

        	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B1_RODTEN_MCK_UI_DLY),
        		P_Fld(0,
        		SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B1) |
        		P_Fld(1,
        		SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B1) |
        		P_Fld(0,
        		SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B1) |
        		P_Fld(1,
        		SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B1));
            #endif
	    }
        }
        else
        {

        	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B0_DQSIEN_MCK_UI_DLY),
        		P_Fld(0,
        		SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B0) |
        		P_Fld(15,
        		SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B0) |
        		P_Fld(1,
        		SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B0) |
        		P_Fld(3,
        		SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B0));

        	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B1_DQSIEN_MCK_UI_DLY),
        		P_Fld(0,
        		SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B1) |
        		P_Fld(15,
        		SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B1) |
        		P_Fld(1,
        		SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B1) |
        		P_Fld(3,
        		SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B1));
            
            #if GATING_RODT_LATANCY_EN
        	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B0_RODTEN_MCK_UI_DLY),
        		P_Fld(0,
        		SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B0) |
        		P_Fld(4,
        		SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B0) |
        		P_Fld(0,
        		SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B0) |
        		P_Fld(4,
        		SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B0));

        	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B1_RODTEN_MCK_UI_DLY),
        		P_Fld(0,
        		SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B1) |
        		P_Fld(4,
        		SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B1) |
        		P_Fld(0,
        		SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B1) |
        		P_Fld(4,
        		SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B1));
            #endif
        }
        
        vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B0_DQSIEN_PI_DLY),
        	16,
        	SHU_RK_B0_DQSIEN_PI_DLY_DQSIEN_PI_B0);
        vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B1_DQSIEN_PI_DLY),
        	16,
        	SHU_RK_B1_DQSIEN_PI_DLY_DQSIEN_PI_B1);

    	DramPhyReset(p);


        vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_RXDLY5),
                P_Fld((U32)0x45, SHU_R0_B0_RXDLY5_RX_ARDQS0_R_DLY_B0));
        vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_RXDLY4),
                P_Fld((U32)0x0, SHU_R0_B0_RXDLY4_RX_ARDQM0_R_DLY_B0));
        vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_RXDLY5),
                P_Fld((U32)0x45, SHU_R0_B1_RXDLY5_RX_ARDQS0_R_DLY_B1));
        vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_RXDLY4),
                P_Fld((U32)0x0, SHU_R0_B1_RXDLY4_RX_ARDQM0_R_DLY_B1));


        U8 u1BitIdx;
        for (u1BitIdx = 0; u1BitIdx < DQS_BIT_NUMBER; u1BitIdx += 2)
        {
             vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_RXDLY0 + u1BitIdx * 2),
                                            P_Fld(((U32)0x0), SHU_R0_B0_RXDLY0_RX_ARDQ0_R_DLY_B0) |
                                            P_Fld(((U32)0x0), SHU_R0_B0_RXDLY0_RX_ARDQ1_R_DLY_B0));

             vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_RXDLY0 + u1BitIdx * 2),
                                            P_Fld((U32)0x0, SHU_R0_B1_RXDLY0_RX_ARDQ0_R_DLY_B1) |
                                            P_Fld((U32)0x0, SHU_R0_B1_RXDLY0_RX_ARDQ1_R_DLY_B1));

            //mcSHOW_DBG_MSG(("u1BitId %d  Addr 0x%2x = %2d %2d %2d %2d \n", u1BitIdx, DDRPHY_RXDQ1+u1BitIdx*2,
            //                FinalWinPerBit[u1BitIdx].best_dqdly, FinalWinPerBit[u1BitIdx+1].best_dqdly,  FinalWinPerBit[u1BitIdx+8].best_dqdly, FinalWinPerBit[u1BitIdx+9].best_dqdly));
        }

        {
            U8 u1TXMCK[4] = {2,2,2,2};
            U8 u1TXOENMCK[4] = {1,1,1,1};
            U8 u1TXUI[4] = {1,1,1,1};
            U8 u1TXOENUI[4] = {5,5,5,5};
            U8 u1TXPI[4] = {30,30,30,30};

            if((p->dram_cbt_mode[RANK_0] == CBT_NORMAL_MODE) && (p->dram_cbt_mode[RANK_1] == CBT_NORMAL_MODE))
            {
                u1TXPI[0] = u1TXPI[1]= u1TXPI[2] = u1TXPI[3] =30;
            }
            else
            {
                u1TXPI[0] = u1TXPI[1]= u1TXPI[2] = u1TXPI[3] =30;
            }

            if(p->rank==RANK_1)
            {
                u1TXPI[0] = u1TXPI[1]= u1TXPI[2] = u1TXPI[3] =50;
            }
            
            TXSetDelayReg_DQ(p, 1, u1TXMCK, u1TXOENMCK, u1TXUI, u1TXOENUI, u1TXPI);
            TXSetDelayReg_DQM(p, 1, u1TXMCK, u1TXOENMCK, u1TXUI, u1TXOENUI, u1TXPI);


            vIO32Write4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_TXDLY0), 0);
            vIO32Write4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_TXDLY1), 0);
            vIO32Write4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_TXDLY0), 0);
            vIO32Write4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_TXDLY1), 0);
            vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_TXDLY3), 0x0, SHU_R0_B0_TXDLY3_TX_ARDQM0_DLY_B0);
            vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_TXDLY3), 0x0, SHU_R0_B1_TXDLY3_TX_ARDQM0_DLY_B1);


            #if ENABLE_TX_TRACKING
            TXUpdateTXTracking(p, TX_DQ_DQS_MOVE_DQ_ONLY, u1TXPI, u1TXPI);
            #endif
        }

        dle_factor_handler(p, 8);
    }

    vSetRank(p, backup_rank);
}

void Apply_LP4_4266_Calibraton_Result(DRAMC_CTX_T *p)
{
    U8 u1RankIdx=0;

    U8 u1WLB0_Dly=0, u1WLB1_Dly=0;

    U8 u1GatingMCKB0_Dly=0, u1GatingMCKB1_Dly=0;
    U8 u1GatingUIB0_Dly=0, u1GatingUIB1_Dly=0;
    U8 u1GatingPIB0_Dly=0, u1GatingPIB1_Dly=0;
    U8 u1B0RodtMCK=0, u1B1RodtMCK=0;
    U8 u1B0RodtUI=0, u1B1RodtUI=0;

    U8 u1RxDQS0=0, u1RxDQS1=0;
    U8 u1RxDQM0=0, u1RxDQM1=0;
    U8 u1RxRK0B0DQ[8] = {70,70,70,70,70,70,70,70};
    U8 u1RxRK0B1DQ[8] = {70,70,70,70,70,70,70,70};
    U8 u1RxRK1B0DQ[8] = {70,70,70,70,70,70,70,70};
    U8 u1RxRK1B1DQ[8] = {70,70,70,70,70,70,70,70};
    U8 *pRxB0DQ, *pRxB1DQ;
    U8 *pTxDQPi;
    U8 backup_rank=0;

    backup_rank = p->rank;

    ShiftDQUI_AllRK(p, -1, ALL_BYTES);
    ShiftDQ_OENUI_AllRK(p, -1, ALL_BYTES);
    ShiftDQSWCK_UI(p, -1, ALL_BYTES);

    for(u1RankIdx=0; u1RankIdx<p->support_rank_num; u1RankIdx++)
    {
        vSetRank(p, u1RankIdx);

#if 1

        //DramcCmdUIDelaySetting(p, 0);
        vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_CA_CMD0),
                                P_Fld(40, SHU_R0_CA_CMD0_RG_ARPI_CMD) |
                                P_Fld(0, SHU_R0_CA_CMD0_RG_ARPI_CLK));
        vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_CA_CMD0), 0, SHU_R0_CA_CMD0_RG_ARPI_CS);
#endif

    #if 1

        if (p->rank == RANK_0)
        {
            u1WLB0_Dly = 34;
            u1WLB1_Dly = 38;
        }
        else
        {
            u1WLB0_Dly = 37;
            u1WLB1_Dly = 32;
        }
        vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_DQ0), u1WLB0_Dly, SHU_R0_B0_DQ0_ARPI_PBYTE_B0);
        vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_DQ0), u1WLB1_Dly, SHU_R0_B1_DQ0_ARPI_PBYTE_B1);


        if (p->rank == RANK_0)
        {
            u1GatingMCKB0_Dly=0; u1GatingMCKB1_Dly=0;
            u1GatingUIB0_Dly=9; u1GatingUIB1_Dly=9;
            u1GatingPIB0_Dly=16; u1GatingPIB1_Dly=16;
            u1B0RodtMCK=0; u1B1RodtMCK=0;
            u1B0RodtUI=0; u1B1RodtUI=0;
        }
        else
        {
            u1GatingMCKB0_Dly=1; u1GatingMCKB1_Dly=1;
            u1GatingUIB0_Dly=2; u1GatingUIB1_Dly=2;
            u1GatingPIB0_Dly=0; u1GatingPIB1_Dly=0;
            u1B0RodtMCK=1; u1B1RodtMCK=1;
            u1B0RodtUI=3; u1B1RodtUI=3;
        }
        vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B0_DQSIEN_MCK_UI_DLY),
            P_Fld(u1GatingMCKB0_Dly,
            SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B0) |
            P_Fld(u1GatingUIB0_Dly,
            SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B0) |
            P_Fld(((u1GatingMCKB0_Dly<<4)+u1GatingUIB0_Dly+4)>>4,
            SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B0) |
            P_Fld(((u1GatingMCKB0_Dly<<4)+u1GatingUIB0_Dly+4)%16,
            SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B0));
        
        vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B1_DQSIEN_MCK_UI_DLY),
            P_Fld(u1GatingMCKB1_Dly,
            SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B1) |
            P_Fld(u1GatingUIB1_Dly,
            SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B1) |
            P_Fld(((u1GatingMCKB1_Dly<<4)+u1GatingUIB1_Dly+4)>>4,
            SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B1) |
            P_Fld(((u1GatingMCKB1_Dly<<4)+u1GatingUIB1_Dly+4)%16,
            SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B1));

#if GATING_RODT_LATANCY_EN
            vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B0_RODTEN_MCK_UI_DLY),
                            P_Fld(u1B0RodtMCK,
                            SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B0) |
                            P_Fld(u1B0RodtUI,
                            SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B0) |
                            P_Fld(u1B0RodtMCK,
                            SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B0) |
                            P_Fld(u1B0RodtUI,
                            SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B0));

            vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B1_RODTEN_MCK_UI_DLY),
                            P_Fld(u1B1RodtMCK,
                            SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B1) |
                            P_Fld(u1B1RodtUI,
                            SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B1) |
                            P_Fld(u1B1RodtMCK,
                            SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B1) |
                            P_Fld(u1B1RodtUI,
                            SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B1));
#endif


        vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B0_DQSIEN_PI_DLY),
                u1GatingPIB0_Dly,
                SHU_RK_B0_DQSIEN_PI_DLY_DQSIEN_PI_B0);
        vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_RK_B1_DQSIEN_PI_DLY),
                u1GatingPIB1_Dly,
                SHU_RK_B1_DQSIEN_PI_DLY_DQSIEN_PI_B1); 

        DramPhyReset(p);


        if (p->rank == RANK_0)
        {
            u1RxDQS0=0; u1RxDQS1=0;
            u1RxDQM0=70; u1RxDQM1=70;
            pRxB0DQ = u1RxRK0B0DQ;
            pRxB1DQ = u1RxRK0B1DQ;
        }
        else
        {
            u1RxDQS0=0; u1RxDQS1=0;
            u1RxDQM0=70; u1RxDQM1=70;
            pRxB0DQ = u1RxRK1B0DQ;
            pRxB1DQ = u1RxRK1B1DQ;
        }


        vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_RXDLY5),
                P_Fld((U32)u1RxDQS0, SHU_R0_B0_RXDLY5_RX_ARDQS0_R_DLY_B0));
        vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_RXDLY4),
                P_Fld((U32)u1RxDQM0, SHU_R0_B0_RXDLY4_RX_ARDQM0_R_DLY_B0) |
                P_Fld((U32)0, SHU_R0_B0_RXDLY4_RX_ARDQM0_F_DLY_B0));
        vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_RXDLY5),
                P_Fld((U32)u1RxDQS1, SHU_R0_B1_RXDLY5_RX_ARDQS0_R_DLY_B1));
        vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_RXDLY4),
                P_Fld((U32)u1RxDQM1, SHU_R0_B1_RXDLY4_RX_ARDQM0_R_DLY_B1) |
                P_Fld((U32)0, SHU_R0_B1_RXDLY4_RX_ARDQM0_F_DLY_B1));


        U8 u1BitIdx;
        for (u1BitIdx = 0; u1BitIdx < DQS_BIT_NUMBER; u1BitIdx += 2)
        {
              vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_RXDLY0 + u1BitIdx * 2),
                                            P_Fld(((U32)pRxB0DQ[u1BitIdx]), SHU_R0_B0_RXDLY0_RX_ARDQ0_R_DLY_B0) |
                                            P_Fld(((U32)pRxB0DQ[u1BitIdx+1]), SHU_R0_B0_RXDLY0_RX_ARDQ1_R_DLY_B0));

             vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_RXDLY0 + u1BitIdx * 2),
                                            P_Fld((U32)pRxB1DQ[u1BitIdx], SHU_R0_B1_RXDLY0_RX_ARDQ0_R_DLY_B1) |
                                            P_Fld((U32)pRxB1DQ[u1BitIdx+1], SHU_R0_B1_RXDLY0_RX_ARDQ1_R_DLY_B1));

            //mcSHOW_DBG_MSG(("u1BitId %d  Addr 0x%2x = %2d %2d %2d %2d \n", u1BitIdx, DDRPHY_RXDQ1+u1BitIdx*2,
            //                FinalWinPerBit[u1BitIdx].best_dqdly, FinalWinPerBit[u1BitIdx+1].best_dqdly,  FinalWinPerBit[u1BitIdx+8].best_dqdly, FinalWinPerBit[u1BitIdx+9].best_dqdly));
        }

        {
            U8 u1TXMCK[4] = {4,4,4,4};
            U8 u1TXOENMCK[4] = {4,4,4,4};
            U8 u1TXUI[4] = {6,6,6,6 };
            U8 u1TXOENUI[4] = {2,2,2,2};
            U8 u1TXRK0PI[4] = {28,28,28,28};
                        

            U8 u1TXMCK_RK1[4] = {4,4,4,4};
            U8 u1TXOENMCK_RK1[4] = {4,4,4,4};
            U8 u1TXUI_RK1[4] = {7,7,7,7};
            U8 u1TXOENUI_RK1[4] = {3,3,3,3};
            U8 u1TXRK1PI[4] = {46,46,46,46};

            if (p->rank == RANK_0)
            {
                pTxDQPi = u1TXRK0PI;
                
                TXSetDelayReg_DQ(p, 1, u1TXMCK, u1TXOENMCK, u1TXUI, u1TXOENUI, pTxDQPi);
                TXSetDelayReg_DQM(p, 1, u1TXMCK, u1TXOENMCK, u1TXUI, u1TXOENUI, pTxDQPi);
            }
            else
            {
                pTxDQPi = u1TXRK1PI;
                TXSetDelayReg_DQ(p, 1, u1TXMCK_RK1, u1TXOENMCK_RK1, u1TXUI_RK1, u1TXOENUI_RK1, pTxDQPi);
                TXSetDelayReg_DQM(p, 1, u1TXMCK_RK1, u1TXOENMCK_RK1, u1TXUI_RK1, u1TXOENUI_RK1, pTxDQPi);
            }
            

            vIO32Write4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_TXDLY0), 0);
            vIO32Write4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_TXDLY1), 0);
            vIO32Write4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_TXDLY0), 0);
            vIO32Write4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_TXDLY1), 0);
            vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B0_TXDLY3), 0x0, SHU_R0_B0_TXDLY3_TX_ARDQM0_DLY_B0);
            vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_R0_B1_TXDLY3), 0x0, SHU_R0_B1_TXDLY3_TX_ARDQM0_DLY_B1);
        }

        dle_factor_handler(p, 14);
    #endif
    }

    vSetRank(p, backup_rank);
}

#endif
#endif

void sv_algorithm_assistance_LP4_1600(DRAMC_CTX_T *p){

vIO32WriteFldMulti(DDRPHY_REG_SHU_MISC_RDSEL_TRACK, P_Fld(0x09, SHU_MISC_RDSEL_TRACK_DMDATLAT_I) |
        P_Fld(0x1, SHU_MISC_RDSEL_TRACK_RDSEL_HWSAVE_MSK) | P_Fld(0x0, SHU_MISC_RDSEL_TRACK_RDSEL_TRACK_EN) |
        P_Fld(0xfeb, SHU_MISC_RDSEL_TRACK_SHU_GW_THRD_NEG) | P_Fld(0x015, SHU_MISC_RDSEL_TRACK_SHU_GW_THRD_POS));

#if !CODE_SIZE_REDUCE
vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RDAT, P_Fld(0x09, MISC_SHU_RDAT_DATLAT) |
        P_Fld(0x09, MISC_SHU_RDAT_DATLAT_DSEL) | P_Fld(0x09, MISC_SHU_RDAT_DATLAT_DSEL_PHY));
#endif

vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_PHY_RX_CTRL, P_Fld(0x1, MISC_SHU_PHY_RX_CTRL_RANK_RXDLY_UPDLAT_EN) |
        P_Fld(0x2, MISC_SHU_PHY_RX_CTRL_RANK_RXDLY_UPD_OFFSET) | P_Fld(0x2, MISC_SHU_PHY_RX_CTRL_RX_IN_GATE_EN_PRE_OFFSET) |
        P_Fld(0x0, MISC_SHU_PHY_RX_CTRL_RX_IN_GATE_EN_HEAD) | P_Fld(0x1, MISC_SHU_PHY_RX_CTRL_RX_IN_GATE_EN_TAIL) |
        P_Fld(0x0, MISC_SHU_PHY_RX_CTRL_RX_IN_BUFF_EN_HEAD) | P_Fld(0x0, MISC_SHU_PHY_RX_CTRL_RX_IN_BUFF_EN_TAIL));

vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RANKCTL, P_Fld(0x0, MISC_SHU_RANKCTL_RANKINCTL_RXDLY) |
        P_Fld(0x1, MISC_SHU_RANKCTL_RANK_RXDLY_OPT) | P_Fld(0x1, MISC_SHU_RANKCTL_RANKSEL_SELPH_FRUN) |
        P_Fld(0x1, MISC_SHU_RANKCTL_RANKINCTL_STB));

#if !CODE_SIZE_REDUCE
vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RANKCTL, P_Fld(0x0, MISC_SHU_RANKCTL_RANKINCTL) |
        P_Fld(0x0, MISC_SHU_RANKCTL_RANKINCTL_ROOT1) | P_Fld(0x3, MISC_SHU_RANKCTL_RANKINCTL_PHY));

#endif

vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RANK_SEL_LAT, P_Fld(0x2, MISC_SHU_RANK_SEL_LAT_RANK_SEL_LAT_B0) |
        P_Fld(0x2, MISC_SHU_RANK_SEL_LAT_RANK_SEL_LAT_B1) | P_Fld(0x2, MISC_SHU_RANK_SEL_LAT_RANK_SEL_LAT_CA));

#if !CODE_SIZE_REDUCE

vIO32WriteFldAlign(DDRPHY_REG_MISC_SHU_RK_DQSCTL, 0x2, MISC_SHU_RK_DQSCTL_DQSINCTL);

vIO32WriteFldAlign(DDRPHY_REG_MISC_SHU_RK_DQSCTL+(1*DDRPHY_AO_RANK_OFFSET), 0x2, MISC_SHU_RK_DQSCTL_DQSINCTL);

vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B0_DQSIEN_MCK_UI_DLY, P_Fld(0x9, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B0) |
        P_Fld(0xd, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B0) | P_Fld(0x0, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B0) |
        P_Fld(0x0, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B0));

vIO32WriteFldAlign(DDRPHY_REG_SHU_RK_B0_DQSIEN_PI_DLY, 0x0b, SHU_RK_B0_DQSIEN_PI_DLY_DQSIEN_PI_B0);

vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B0_DQSIEN_MCK_UI_DLY+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0xc, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B0) |
        P_Fld(0x0, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B0) | P_Fld(0x0, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B0) |
        P_Fld(0x1, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B0));

vIO32WriteFldAlign(DDRPHY_REG_SHU_RK_B0_DQSIEN_PI_DLY+(1*DDRPHY_AO_RANK_OFFSET), 0x11, SHU_RK_B0_DQSIEN_PI_DLY_DQSIEN_PI_B0);

vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B1_DQSIEN_MCK_UI_DLY, P_Fld(0x9, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B1) |
        P_Fld(0xd, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B1) | P_Fld(0x0, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B1) |
        P_Fld(0x0, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B1));

vIO32WriteFldAlign(DDRPHY_REG_SHU_RK_B1_DQSIEN_PI_DLY, 0x0b, SHU_RK_B1_DQSIEN_PI_DLY_DQSIEN_PI_B1);

vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B1_DQSIEN_MCK_UI_DLY+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0xc, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B1) |
        P_Fld(0x0, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B1) | P_Fld(0x0, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B1) |
        P_Fld(0x1, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B1));

vIO32WriteFldAlign(DDRPHY_REG_SHU_RK_B1_DQSIEN_PI_DLY+(1*DDRPHY_AO_RANK_OFFSET), 0x11, SHU_RK_B1_DQSIEN_PI_DLY_DQSIEN_PI_B1);
#endif

vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_ODTCTRL, P_Fld(0x1, MISC_SHU_ODTCTRL_RODTEN) |
        P_Fld(0x0, MISC_SHU_ODTCTRL_RODTENSTB_SELPH_CG_IG) |
        P_Fld(0x0, MISC_SHU_ODTCTRL_RODTEN_SELPH_FRUN) | P_Fld(0x0, MISC_SHU_ODTCTRL_RODTDLY_LAT_OPT) |
        P_Fld(0x0, MISC_SHU_ODTCTRL_FIXRODT) | P_Fld(0x1, MISC_SHU_ODTCTRL_RODTEN_OPT) |
        P_Fld(0x1, MISC_SHU_ODTCTRL_RODTE2) | P_Fld(0x1, MISC_SHU_ODTCTRL_RODTE));

#if !CODE_SIZE_REDUCE
vIO32WriteFldAlign(DDRPHY_REG_MISC_SHU_ODTCTRL, 0x1, MISC_SHU_ODTCTRL_RODT_LAT);
#endif

vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ7, P_Fld(0x0, SHU_B0_DQ7_R_DMRANKRXDVS_B0) |
        P_Fld(0x0, SHU_B0_DQ7_R_DMDQMDBI_EYE_SHU_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMDQMDBI_SHU_B0) |
        P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_DQM_FLAGSEL_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B0) |
        P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMRXTRACK_DQM_EN_B0) |
        P_Fld(0x1, SHU_B0_DQ7_R_DMRODTEN_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMARPI_CG_FB2DLL_DCM_EN_B0) |
        P_Fld(0x0, SHU_B0_DQ7_R_DMTX_ARPI_CG_DQ_NEW_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMTX_ARPI_CG_DQS_NEW_B0) |
        P_Fld(0x0, SHU_B0_DQ7_R_DMTX_ARPI_CG_DQM_NEW_B0) | P_Fld(0x0, SHU_B0_DQ7_R_LP4Y_SDN_MODE_DQS0) |
        P_Fld(0x1, SHU_B0_DQ7_R_DMRXRANK_DQ_EN_B0) | P_Fld(0x1, SHU_B0_DQ7_R_DMRXRANK_DQ_LAT_B0) |
        P_Fld(0x1, SHU_B0_DQ7_R_DMRXRANK_DQS_EN_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMRXRANK_DQS_LAT_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ7, P_Fld(0x0, SHU_B1_DQ7_R_DMRANKRXDVS_B1) |
        P_Fld(0x0, SHU_B1_DQ7_R_DMDQMDBI_EYE_SHU_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMDQMDBI_SHU_B1) |
        P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_DQM_FLAGSEL_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B1) |
        P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMRXTRACK_DQM_EN_B1) |
        P_Fld(0x1, SHU_B1_DQ7_R_DMRODTEN_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMARPI_CG_FB2DLL_DCM_EN_B1) |
        P_Fld(0x0, SHU_B1_DQ7_R_DMTX_ARPI_CG_DQ_NEW_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMTX_ARPI_CG_DQS_NEW_B1) |
        P_Fld(0x0, SHU_B1_DQ7_R_DMTX_ARPI_CG_DQM_NEW_B1) | P_Fld(0x0, SHU_B1_DQ7_R_LP4Y_SDN_MODE_DQS1) |
        P_Fld(0x1, SHU_B1_DQ7_R_DMRXRANK_DQ_EN_B1) | P_Fld(0x1, SHU_B1_DQ7_R_DMRXRANK_DQ_LAT_B1) |
        P_Fld(0x1, SHU_B1_DQ7_R_DMRXRANK_DQS_EN_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMRXRANK_DQS_LAT_B1));

vIO32WriteFldAlign(DDRPHY_REG_SHU_MISC_RX_PIPE_CTRL, 0x1, SHU_MISC_RX_PIPE_CTRL_RX_PIPE_BYPASS_EN);

vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B0_RODTEN_MCK_UI_DLY, P_Fld(0x4, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B0) |
        P_Fld(0x4, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B0) | P_Fld(0x0, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B0) |
        P_Fld(0x0, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B0_RODTEN_MCK_UI_DLY+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x7, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B0) |
        P_Fld(0x7, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B0) | P_Fld(0x0, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B0) |
        P_Fld(0x0, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B1_RODTEN_MCK_UI_DLY, P_Fld(0x4, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B1) |
        P_Fld(0x4, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B1) | P_Fld(0x0, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B1) |
        P_Fld(0x0, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B1_RODTEN_MCK_UI_DLY+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x7, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B1) |
        P_Fld(0x7, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B1) | P_Fld(0x0, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B1) |
        P_Fld(0x0, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B1));

#if !CODE_SIZE_REDUCE
vIO32WriteFldMulti(DRAMC_REG_SHU_RX_CG_SET0, P_Fld(0x0, SHU_RX_CG_SET0_DLE_LAST_EXTEND3) |
        P_Fld(0x0, SHU_RX_CG_SET0_READ_START_EXTEND3) | P_Fld(0x0, SHU_RX_CG_SET0_DLE_LAST_EXTEND2) |
        P_Fld(0x0, SHU_RX_CG_SET0_READ_START_EXTEND2) | P_Fld(0x1, SHU_RX_CG_SET0_DLE_LAST_EXTEND1) |
        P_Fld(0x1, SHU_RX_CG_SET0_READ_START_EXTEND1));
#endif

vIO32WriteFldMulti(DDRPHY_REG_SHU_MISC_RANK_SEL_STB, P_Fld(0x1, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_EN) |
        P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_EN_B23) | P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_SERMODE) |
        P_Fld(0x1, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_TRACK) | P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_RXDLY_TRACK) |
        P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_PHASE_EN) | P_Fld(0x1, SHU_MISC_RANK_SEL_STB_RANK_SEL_PHSINCTL) |
        P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_UI_PLUS) | P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_MCK_PLUS) |
        P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_UI_MINUS) | P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_MCK_MINUS));

vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RK_DQSCAL, P_Fld(0x60, MISC_SHU_RK_DQSCAL_DQSIENLLMT) |
        P_Fld(0x1, MISC_SHU_RK_DQSCAL_DQSIENLLMTEN) | P_Fld(0x3f, MISC_SHU_RK_DQSCAL_DQSIENHLMT) |
        P_Fld(0x1, MISC_SHU_RK_DQSCAL_DQSIENHLMTEN));

vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RK_DQSCAL+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x60, MISC_SHU_RK_DQSCAL_DQSIENLLMT) |
        P_Fld(0x1, MISC_SHU_RK_DQSCAL_DQSIENLLMTEN) | P_Fld(0x3f, MISC_SHU_RK_DQSCAL_DQSIENHLMT) |
        P_Fld(0x1, MISC_SHU_RK_DQSCAL_DQSIENHLMTEN));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_INI_UIPI, P_Fld(0x0b, SHU_R0_B0_INI_UIPI_CURR_INI_PI_B0) |
        P_Fld(0x09, SHU_R0_B0_INI_UIPI_CURR_INI_UI_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_INI_UIPI, P_Fld(0x0b, SHU_R0_B1_INI_UIPI_CURR_INI_PI_B1) |
        P_Fld(0x09, SHU_R0_B1_INI_UIPI_CURR_INI_UI_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_INI_UIPI+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x11, SHU_R0_B0_INI_UIPI_CURR_INI_PI_B0) |
        P_Fld(0x0c, SHU_R0_B0_INI_UIPI_CURR_INI_UI_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_INI_UIPI+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x11, SHU_R0_B1_INI_UIPI_CURR_INI_PI_B1) |
        P_Fld(0x0c, SHU_R0_B1_INI_UIPI_CURR_INI_UI_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_NEXT_INI_UIPI, P_Fld(0x0b, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_PI_B0) |
        P_Fld(0x09, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_UI_B0) | P_Fld(0x0d, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_UI_P1_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_NEXT_INI_UIPI, P_Fld(0x0b, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_PI_B1) |
        P_Fld(0x09, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_UI_B1) | P_Fld(0x0d, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_UI_P1_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_NEXT_INI_UIPI+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x11, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_PI_B0) |
        P_Fld(0x0c, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_UI_B0) | P_Fld(0x10, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_UI_P1_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_NEXT_INI_UIPI+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x11, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_PI_B1) |
        P_Fld(0x0c, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_UI_B1) | P_Fld(0x10, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_UI_P1_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_CA_CMD0, P_Fld(0x0, SHU_R0_CA_CMD0_RG_RX_ARCLK_R_DLY_DUTY) |
        P_Fld(0x0, SHU_R0_CA_CMD0_RG_RX_ARCLK_F_DLY_DUTY) |
        P_Fld(0x0, SHU_R0_CA_CMD0_DA_ARPI_DDR400_0D5UI_RK0_CA) | P_Fld(0x0, SHU_R0_CA_CMD0_DA_RX_ARDQSIEN_0D5UI_RK0_CA));
#if !CODE_SIZE_REDUCE
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_CA_CMD0, P_Fld(0x00, SHU_R0_CA_CMD0_RG_ARPI_CS) |
        P_Fld(0x20, SHU_R0_CA_CMD0_RG_ARPI_CMD) | P_Fld(0x00, SHU_R0_CA_CMD0_RG_ARPI_CLK));
#endif

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_DQ0, P_Fld(0x0, SHU_R0_B0_DQ0_RG_RX_ARDQS0_R_DLY_DUTY) |
        P_Fld(0x0, SHU_R0_B0_DQ0_RG_RX_ARDQS0_F_DLY_DUTY) |
        P_Fld(0x0, SHU_R0_B0_DQ0_DA_ARPI_DDR400_0D5UI_RK0_B0) | P_Fld(0x0, SHU_R0_B0_DQ0_DA_RX_ARDQSIEN_0D5UI_RK0_B0));

#if !CODE_SIZE_REDUCE
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_DQ0, P_Fld(0x19, SHU_R0_B0_DQ0_SW_ARPI_DQ_B0) |
        P_Fld(0x19, SHU_R0_B0_DQ0_SW_ARPI_DQM_B0) | P_Fld(0x00, SHU_R0_B0_DQ0_ARPI_PBYTE_B0));
#endif

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_DQ0, P_Fld(0x0, SHU_R0_B1_DQ0_RG_RX_ARDQS1_R_DLY_DUTY) |
        P_Fld(0x0, SHU_R0_B1_DQ0_RG_RX_ARDQS1_F_DLY_DUTY) |
        P_Fld(0x0, SHU_R0_B1_DQ0_DA_ARPI_DDR400_0D5UI_RK0_B1) | P_Fld(0x0, SHU_R0_B1_DQ0_DA_RX_ARDQSIEN_0D5UI_RK0_B1));

#if !CODE_SIZE_REDUCE
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_DQ0, P_Fld(0x1f, SHU_R0_B1_DQ0_SW_ARPI_DQ_B1) |
        P_Fld(0x1f, SHU_R0_B1_DQ0_SW_ARPI_DQM_B1) | P_Fld(0x00, SHU_R0_B1_DQ0_ARPI_PBYTE_B1));
#endif

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_CA_CMD0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x0, SHU_R0_CA_CMD0_RG_RX_ARCLK_R_DLY_DUTY) |
        P_Fld(0x0, SHU_R0_CA_CMD0_RG_RX_ARCLK_F_DLY_DUTY) |
        P_Fld(0x0, SHU_R0_CA_CMD0_DA_ARPI_DDR400_0D5UI_RK0_CA) | P_Fld(0x0, SHU_R0_CA_CMD0_DA_RX_ARDQSIEN_0D5UI_RK0_CA));

#if !CODE_SIZE_REDUCE
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_CA_CMD0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x00, SHU_R0_CA_CMD0_RG_ARPI_CS) |
        P_Fld(0x20, SHU_R0_CA_CMD0_RG_ARPI_CMD) | P_Fld(0x00, SHU_R0_CA_CMD0_RG_ARPI_CLK));
#endif

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_DQ0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x0, SHU_R0_B0_DQ0_RG_RX_ARDQS0_R_DLY_DUTY) |
        P_Fld(0x0, SHU_R0_B0_DQ0_RG_RX_ARDQS0_F_DLY_DUTY) |
        P_Fld(0x0, SHU_R0_B0_DQ0_DA_ARPI_DDR400_0D5UI_RK0_B0) | P_Fld(0x0, SHU_R0_B0_DQ0_DA_RX_ARDQSIEN_0D5UI_RK0_B0));

#if !CODE_SIZE_REDUCE
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_DQ0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x13, SHU_R0_B0_DQ0_SW_ARPI_DQ_B0) |
        P_Fld(0x13, SHU_R0_B0_DQ0_SW_ARPI_DQM_B0) | P_Fld(0x00, SHU_R0_B0_DQ0_ARPI_PBYTE_B0));
#endif

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_DQ0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x0, SHU_R0_B1_DQ0_RG_RX_ARDQS1_R_DLY_DUTY) |
        P_Fld(0x0, SHU_R0_B1_DQ0_RG_RX_ARDQS1_F_DLY_DUTY) |
        P_Fld(0x0, SHU_R0_B1_DQ0_DA_ARPI_DDR400_0D5UI_RK0_B1) | P_Fld(0x0, SHU_R0_B1_DQ0_DA_RX_ARDQSIEN_0D5UI_RK0_B1));

#if !CODE_SIZE_REDUCE
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_DQ0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x12, SHU_R0_B1_DQ0_SW_ARPI_DQ_B1) |
        P_Fld(0x12, SHU_R0_B1_DQ0_SW_ARPI_DQM_B1) | P_Fld(0x00, SHU_R0_B1_DQ0_ARPI_PBYTE_B1));
#endif

vIO32WriteFldMulti(DRAMC_REG_SHU_DCM_CTRL0, P_Fld(0x1, SHU_DCM_CTRL0_DDRPHY_CLK_EN_OPT) |
        P_Fld(0x3, SHU_DCM_CTRL0_DPHY_CMD_CLKEN_EXTCNT) | P_Fld(0x5, SHU_DCM_CTRL0_DDRPHY_CLK_DYN_GATING_SEL) |
        P_Fld(0x2, SHU_DCM_CTRL0_APHYPI_CKCGL_CNT) | P_Fld(0x4, SHU_DCM_CTRL0_APHYPI_CKCGH_CNT) |
        P_Fld(0x0, SHU_DCM_CTRL0_FASTWAKE2) | P_Fld(0x1, SHU_DCM_CTRL0_FASTWAKE));

vIO32WriteFldMulti(DRAMC_REG_SHU_APHY_TX_PICG_CTRL, P_Fld(0x3, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_PICG_CNT) |
        P_Fld(0x1, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQS_SEL_P1) | P_Fld(0x0, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQS_SEL_P0) |
        P_Fld(0x2, SHU_APHY_TX_PICG_CTRL_DPHY_TX_DCM_EXTCNT) | P_Fld(0x1, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_OPT));

vIO32WriteFldMulti(DRAMC_REG_SHURK_APHY_TX_PICG_CTRL, P_Fld(0x1, SHURK_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQ_RK_SEL_P1) |
        P_Fld(0x0, SHURK_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQ_RK_SEL_P0));

vIO32WriteFldMulti(DRAMC_REG_SHURK_APHY_TX_PICG_CTRL+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x1, SHURK_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQ_RK_SEL_P1) |
        P_Fld(0x1, SHURK_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQ_RK_SEL_P0));

vIO32WriteFldMulti(DRAMC_REG_SHU_NEW_XRW2W_CTRL, P_Fld(0x0, SHU_NEW_XRW2W_CTRL_TX_PI_UPDCTL_B0) |
        P_Fld(0x0, SHU_NEW_XRW2W_CTRL_TX_PI_UPDCTL_B1) | P_Fld(0x0, SHU_NEW_XRW2W_CTRL_TXPI_UPD_MODE));

vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_DQS0, P_Fld(0x1, SHU_SELPH_DQS0_TXDLY_DQS2) |
        P_Fld(0x1, SHU_SELPH_DQS0_TXDLY_DQS3) | P_Fld(0x1, SHU_SELPH_DQS0_TXDLY_OEN_DQS2) |
        P_Fld(0x1, SHU_SELPH_DQS0_TXDLY_OEN_DQS3));

#if !CODE_SIZE_REDUCE
vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_DQS0, P_Fld(0x2, SHU_SELPH_DQS0_TXDLY_DQS0) |
        P_Fld(0x2, SHU_SELPH_DQS0_TXDLY_DQS1) | P_Fld(0x1, SHU_SELPH_DQS0_TXDLY_OEN_DQS0) |
        P_Fld(0x1, SHU_SELPH_DQS0_TXDLY_OEN_DQS1));
#endif

vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_DQS1, P_Fld(0x1, SHU_SELPH_DQS1_DLY_DQS2) |
        P_Fld(0x1, SHU_SELPH_DQS1_DLY_DQS3) | P_Fld(0x1, SHU_SELPH_DQS1_DLY_OEN_DQS2) |
        P_Fld(0x1, SHU_SELPH_DQS1_DLY_OEN_DQS3));

#if !CODE_SIZE_REDUCE
vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_DQS1, P_Fld(0x1, SHU_SELPH_DQS1_DLY_DQS0) |
        P_Fld(0x1, SHU_SELPH_DQS1_DLY_DQS1) | P_Fld(0x6, SHU_SELPH_DQS1_DLY_OEN_DQS0) |
        P_Fld(0x6, SHU_SELPH_DQS1_DLY_OEN_DQS1));

vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ0, P_Fld(0x2, SHURK_SELPH_DQ0_TXDLY_DQ0) |
        P_Fld(0x2, SHURK_SELPH_DQ0_TXDLY_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_DQ2) |
        P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_DQ3) | P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_OEN_DQ0) |
        P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_OEN_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_OEN_DQ2) |
        P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_OEN_DQ3));

vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ1, P_Fld(0x2, SHURK_SELPH_DQ1_TXDLY_DQM0) |
        P_Fld(0x2, SHURK_SELPH_DQ1_TXDLY_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_DQM2) |
        P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_DQM3) | P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_OEN_DQM0) |
        P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_OEN_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_OEN_DQM2) |
        P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_OEN_DQM3));

vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ2, P_Fld(0x1, SHURK_SELPH_DQ2_DLY_DQ0) |
        P_Fld(0x1, SHURK_SELPH_DQ2_DLY_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ2_DLY_DQ2) |
        P_Fld(0x1, SHURK_SELPH_DQ2_DLY_DQ3) | P_Fld(0x6, SHURK_SELPH_DQ2_DLY_OEN_DQ0) |
        P_Fld(0x6, SHURK_SELPH_DQ2_DLY_OEN_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ2_DLY_OEN_DQ2) |
        P_Fld(0x1, SHURK_SELPH_DQ2_DLY_OEN_DQ3));

vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ3, P_Fld(0x1, SHURK_SELPH_DQ3_DLY_DQM0) |
        P_Fld(0x1, SHURK_SELPH_DQ3_DLY_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ3_DLY_DQM2) |
        P_Fld(0x1, SHURK_SELPH_DQ3_DLY_DQM3) | P_Fld(0x6, SHURK_SELPH_DQ3_DLY_OEN_DQM0) |
        P_Fld(0x6, SHURK_SELPH_DQ3_DLY_OEN_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ3_DLY_OEN_DQM2) |
        P_Fld(0x1, SHURK_SELPH_DQ3_DLY_OEN_DQM3));

vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ0+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x2, SHURK_SELPH_DQ0_TXDLY_DQ0) |
        P_Fld(0x2, SHURK_SELPH_DQ0_TXDLY_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_DQ2) |
        P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_DQ3) | P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_OEN_DQ0) |
        P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_OEN_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_OEN_DQ2) |
        P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_OEN_DQ3));

vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ1+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x2, SHURK_SELPH_DQ1_TXDLY_DQM0) |
        P_Fld(0x2, SHURK_SELPH_DQ1_TXDLY_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_DQM2) |
        P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_DQM3) | P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_OEN_DQM0) |
        P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_OEN_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_OEN_DQM2) |
        P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_OEN_DQM3));

vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ2+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x2, SHURK_SELPH_DQ2_DLY_DQ0) |
        P_Fld(0x2, SHURK_SELPH_DQ2_DLY_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ2_DLY_DQ2) |
        P_Fld(0x1, SHURK_SELPH_DQ2_DLY_DQ3) | P_Fld(0x7, SHURK_SELPH_DQ2_DLY_OEN_DQ0) |
        P_Fld(0x7, SHURK_SELPH_DQ2_DLY_OEN_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ2_DLY_OEN_DQ2) |
        P_Fld(0x1, SHURK_SELPH_DQ2_DLY_OEN_DQ3));

vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ3+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x2, SHURK_SELPH_DQ3_DLY_DQM0) |
        P_Fld(0x2, SHURK_SELPH_DQ3_DLY_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ3_DLY_DQM2) |
        P_Fld(0x1, SHURK_SELPH_DQ3_DLY_DQM3) | P_Fld(0x7, SHURK_SELPH_DQ3_DLY_OEN_DQM0) |
        P_Fld(0x7, SHURK_SELPH_DQ3_DLY_OEN_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ3_DLY_OEN_DQM2) |
        P_Fld(0x1, SHURK_SELPH_DQ3_DLY_OEN_DQM3));
#endif

vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL1, P_Fld(0x019, SHURK_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ0) |
        P_Fld(0x01f, SHURK_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ1));

vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL2, P_Fld(0x019, SHURK_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ0) |
        P_Fld(0x01f, SHURK_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ1));

vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL5, P_Fld(0x019, SHURK_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM0) |
        P_Fld(0x01f, SHURK_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM1));

vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL1+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x013, SHURK_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ0) |
        P_Fld(0x012, SHURK_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ1));

vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL2+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x013, SHURK_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ0) |
        P_Fld(0x012, SHURK_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ1));

vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL5+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x013, SHURK_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM0) |
        P_Fld(0x012, SHURK_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM1));

vIO32WriteFldMulti(DRAMC_REG_SHURK_PI, P_Fld(0x1f, SHURK_PI_RK0_ARPI_DQ_B1) |
        P_Fld(0x19, SHURK_PI_RK0_ARPI_DQ_B0) | P_Fld(0x1f, SHURK_PI_RK0_ARPI_DQM_B1) |
        P_Fld(0x19, SHURK_PI_RK0_ARPI_DQM_B0));

vIO32WriteFldMulti(DRAMC_REG_SHURK_PI+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x12, SHURK_PI_RK0_ARPI_DQ_B1) |
        P_Fld(0x13, SHURK_PI_RK0_ARPI_DQ_B0) | P_Fld(0x12, SHURK_PI_RK0_ARPI_DQM_B1) |
        P_Fld(0x13, SHURK_PI_RK0_ARPI_DQM_B0));

#if !CODE_SIZE_REDUCE

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_TXDLY0, P_Fld(0x3c, SHU_R0_B0_TXDLY0_TX_ARDQ0_DLY_B0) |
        P_Fld(0x3c, SHU_R0_B0_TXDLY0_TX_ARDQ1_DLY_B0) | P_Fld(0x3c, SHU_R0_B0_TXDLY0_TX_ARDQ2_DLY_B0) |
        P_Fld(0x3c, SHU_R0_B0_TXDLY0_TX_ARDQ3_DLY_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_TXDLY1, P_Fld(0x3c, SHU_R0_B0_TXDLY1_TX_ARDQ4_DLY_B0) |
        P_Fld(0x3c, SHU_R0_B0_TXDLY1_TX_ARDQ5_DLY_B0) | P_Fld(0x3c, SHU_R0_B0_TXDLY1_TX_ARDQ6_DLY_B0) |
        P_Fld(0x3c, SHU_R0_B0_TXDLY1_TX_ARDQ7_DLY_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_TXDLY3, P_Fld(0x3c, SHU_R0_B0_TXDLY3_TX_ARDQM0_DLY_B0) |
        P_Fld(0x00, SHU_R0_B0_TXDLY3_TX_ARWCK_DLY_B0) | P_Fld(0x00, SHU_R0_B0_TXDLY3_TX_ARWCKB_DLY_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_TXDLY0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x08, SHU_R0_B1_TXDLY0_TX_ARDQ0_DLY_B1) |
        P_Fld(0x08, SHU_R0_B1_TXDLY0_TX_ARDQ1_DLY_B1) | P_Fld(0x08, SHU_R0_B1_TXDLY0_TX_ARDQ2_DLY_B1) |
        P_Fld(0x08, SHU_R0_B1_TXDLY0_TX_ARDQ3_DLY_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_TXDLY1+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x08, SHU_R0_B1_TXDLY1_TX_ARDQ4_DLY_B1) |
        P_Fld(0x08, SHU_R0_B1_TXDLY1_TX_ARDQ5_DLY_B1) | P_Fld(0x08, SHU_R0_B1_TXDLY1_TX_ARDQ6_DLY_B1) |
        P_Fld(0x08, SHU_R0_B1_TXDLY1_TX_ARDQ7_DLY_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_TXDLY3+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x08, SHU_R0_B1_TXDLY3_TX_ARDQM0_DLY_B1) |
        P_Fld(0x00, SHU_R0_B1_TXDLY3_TX_ARWCK_DLY_B1) | P_Fld(0x00, SHU_R0_B1_TXDLY3_TX_ARWCKB_DLY_B1));

#endif

# if !(CODE_SIZE_REDUCE && AC_TIMING_DERATE_ENABLE)

vIO32WriteFldMulti(DRAMC_REG_SHU_AC_DERATING0, P_Fld(0x0, SHU_AC_DERATING0_ACDERATEEN) |
        P_Fld(0x1, SHU_AC_DERATING0_TRRD_DERATE) | P_Fld(0x4, SHU_AC_DERATING0_TRCD_DERATE));

vIO32WriteFldMulti(DRAMC_REG_SHU_AC_DERATING1, P_Fld(0x3, SHU_AC_DERATING1_TRPAB_DERATE) |
        P_Fld(0x2, SHU_AC_DERATING1_TRP_DERATE) | P_Fld(0x00, SHU_AC_DERATING1_TRAS_DERATE) |
        P_Fld(0x00, SHU_AC_DERATING1_TRC_DERATE));

vIO32WriteFldMulti(DRAMC_REG_SHU_AC_DERATING_05T, P_Fld(0x0, SHU_AC_DERATING_05T_TRC_05T_DERATE) |
        P_Fld(0x0, SHU_AC_DERATING_05T_TRCD_05T_DERATE) | P_Fld(0x1, SHU_AC_DERATING_05T_TRP_05T_DERATE) |
        P_Fld(0x1, SHU_AC_DERATING_05T_TRPAB_05T_DERATE) | P_Fld(0x1, SHU_AC_DERATING_05T_TRAS_05T_DERATE) |
        P_Fld(0x0, SHU_AC_DERATING_05T_TRRD_05T_DERATE));
#endif

vIO32WriteFldMulti(DRAMC_REG_SHU_SREF_CTRL, P_Fld(0x3, SHU_SREF_CTRL_CKEHCMD) |
        P_Fld(0x3, SHU_SREF_CTRL_SREF_CK_DLY));

vIO32WriteFldMulti(DRAMC_REG_SHU_HMR4_DVFS_CTRL0, P_Fld(0x32, SHU_HMR4_DVFS_CTRL0_FSPCHG_PRDCNT) |
        P_Fld(0x000, SHU_HMR4_DVFS_CTRL0_REFRCNT));

vIO32WriteFldMulti(DRAMC_REG_SHU_AC_TIME_05T,
        P_Fld(0x0, SHU_AC_TIME_05T_TCKEPRD_05T) | P_Fld(0x0, SHU_AC_TIME_05T_BGTRRD_05T) |
        P_Fld(0x0, SHU_AC_TIME_05T_BGTCCD_05T) | P_Fld(0x0, SHU_AC_TIME_05T_BGTWTR_M05T) |
        P_Fld(0x0, SHU_AC_TIME_05T_XRTR2W_05T) | P_Fld(0x0, SHU_AC_TIME_05T_XRTW2R_M05T));

#if !CODE_SIZE_REDUCE
vIO32WriteFldMulti(DRAMC_REG_SHU_AC_TIME_05T, P_Fld(0x0, SHU_AC_TIME_05T_TRC_05T) |
        P_Fld(0x0, SHU_AC_TIME_05T_TRFCPB_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TRFC_05T) |
        P_Fld(0x0, SHU_AC_TIME_05T_TPBR2PBR_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TXP_05T) |
        P_Fld(0x1, SHU_AC_TIME_05T_TRTP_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TRCD_05T) |
        P_Fld(0x1, SHU_AC_TIME_05T_TRP_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TRPAB_05T) |
        P_Fld(0x0, SHU_AC_TIME_05T_TRAS_05T) | P_Fld(0x1, SHU_AC_TIME_05T_TWR_M05T) |
        P_Fld(0x0, SHU_AC_TIME_05T_TRRD_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TFAW_05T) |
        P_Fld(0x0, SHU_AC_TIME_05T_TR2PD_05T) | P_Fld(0x1, SHU_AC_TIME_05T_TWTPD_M05T) |
        P_Fld(0x1, SHU_AC_TIME_05T_TMRRI_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TMRWCKEL_05T) |
        P_Fld(0x0, SHU_AC_TIME_05T_TR2W_05T) | P_Fld(0x1, SHU_AC_TIME_05T_TWTR_M05T) |
        P_Fld(0x1, SHU_AC_TIME_05T_TMRD_05T) | P_Fld(0x1, SHU_AC_TIME_05T_TMRW_05T) |
        P_Fld(0x1, SHU_AC_TIME_05T_TMRR2MRW_05T) | P_Fld(0x1, SHU_AC_TIME_05T_TW2MRW_05T) |
        P_Fld(0x0, SHU_AC_TIME_05T_TR2MRW_05T) | P_Fld(0x1, SHU_AC_TIME_05T_TPBR2ACT_05T));

vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM_XRT, P_Fld(0x03, SHU_ACTIM_XRT_XRTR2R) |
        P_Fld(0x03, SHU_ACTIM_XRT_XRTR2W) | P_Fld(0x3, SHU_ACTIM_XRT_XRTW2R) |
        P_Fld(0x04, SHU_ACTIM_XRT_XRTW2W));

vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM0, P_Fld(0x04, SHU_ACTIM0_TWTR) |
        P_Fld(0x07, SHU_ACTIM0_TWR) | P_Fld(0x1, SHU_ACTIM0_TRRD) |
        P_Fld(0x4, SHU_ACTIM0_TRCD) | P_Fld(0x2, SHU_ACTIM0_CKELCKCNT));

vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM1, P_Fld(0x3, SHU_ACTIM1_TRPAB) |
        P_Fld(0x4, SHU_ACTIM1_TMRWCKEL) | P_Fld(0x2, SHU_ACTIM1_TRP) |
        P_Fld(0x00, SHU_ACTIM1_TRAS) | P_Fld(0x00, SHU_ACTIM1_TRC));

vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM2, P_Fld(0x0, SHU_ACTIM2_TXP) |
        P_Fld(0x05, SHU_ACTIM2_TMRRI) | P_Fld(0x0, SHU_ACTIM2_TRTP) |
        P_Fld(0x03, SHU_ACTIM2_TR2W) | P_Fld(0x00, SHU_ACTIM2_TFAW));

vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM3, P_Fld(0x1a, SHU_ACTIM3_TRFCPB) | 
        P_Fld(0x4, SHU_ACTIM3_TR2MRR) | P_Fld(0x40, SHU_ACTIM3_TRFC));
#endif
vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM3, P_Fld(0x4, SHU_ACTIM3_MANTMRR) |
        P_Fld(0x00, SHU_ACTIM3_TWTR_L));

#if !CODE_SIZE_REDUCE
vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM4, P_Fld(0x04e, SHU_ACTIM4_TXREFCNT) |
        P_Fld(0x07, SHU_ACTIM4_TMRR2MRW) | P_Fld(0x05, SHU_ACTIM4_TMRR2W) |
        P_Fld(0x10, SHU_ACTIM4_TZQCS));

vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM5, P_Fld(0x08, SHU_ACTIM5_TR2PD) |
        P_Fld(0x09, SHU_ACTIM5_TWTPD) | P_Fld(0x0b, SHU_ACTIM5_TPBR2PBR) |
        P_Fld(0x0, SHU_ACTIM5_TPBR2ACT));

vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM6, P_Fld(0x06, SHU_ACTIM6_TZQLAT2) |
        P_Fld(0x3, SHU_ACTIM6_TMRD) | P_Fld(0x2, SHU_ACTIM6_TMRW) |
        P_Fld(0x06, SHU_ACTIM6_TW2MRW) | P_Fld(0x09, SHU_ACTIM6_TR2MRW));

vIO32WriteFldMulti(DRAMC_REG_SHU_CKECTRL, P_Fld(0x1, SHU_CKECTRL_TPDE_05T) |
        P_Fld(0x0, SHU_CKECTRL_TPDX_05T) | P_Fld(0x1, SHU_CKECTRL_TPDE) |
        P_Fld(0x1, SHU_CKECTRL_TPDX) | P_Fld(0x1, SHU_CKECTRL_TCKEPRD));
#endif

vIO32WriteFldAlign(DRAMC_REG_SHU_CKECTRL, 0x3, SHU_CKECTRL_TCKESRX);

vIO32WriteFldMulti(DRAMC_REG_SHU_MISC, P_Fld(0x7, SHU_MISC_DCMDLYREF) |
        P_Fld(0x0, SHU_MISC_DAREFEN));

#if !CODE_SIZE_REDUCE
vIO32WriteFldAlign(DRAMC_REG_SHU_MISC, 0x2, SHU_MISC_REQQUE_MAXCNT);


vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ8, P_Fld(0x0, SHU_B0_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B0) |
        P_Fld(0x0, SHU_B0_DQ8_R_RMRODTEN_CG_IG_B0) | P_Fld(0x1, SHU_B0_DQ8_R_RMRX_TOPHY_CG_IG_B0) |
        P_Fld(0x0, SHU_B0_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B0) |
        P_Fld(0x1, SHU_B0_DQ8_R_DMRXDLY_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B0) |
        P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B0) |
        P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMRANK_PIPE_CG_IG_B0) |
        P_Fld(0x0, SHU_B0_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B0));
#endif
vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ8, P_Fld(0x0063, SHU_B0_DQ8_R_DMRXDVS_UPD_FORCE_CYC_B0) |
        P_Fld(0x0, SHU_B0_DQ8_R_DMRXDVS_UPD_FORCE_EN_B0));


#if !CODE_SIZE_REDUCE
vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ8, P_Fld(0x0, SHU_B1_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B1) |
        P_Fld(0x0, SHU_B1_DQ8_R_RMRODTEN_CG_IG_B1) | P_Fld(0x1, SHU_B1_DQ8_R_RMRX_TOPHY_CG_IG_B1) |
        P_Fld(0x0, SHU_B1_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B1) |
        P_Fld(0x1, SHU_B1_DQ8_R_DMRXDLY_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B1) |
        P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B1) |
        P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMRANK_PIPE_CG_IG_B1) |
        P_Fld(0x0, SHU_B1_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B1));
#endif

vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ8, P_Fld(0x0063, SHU_B1_DQ8_R_DMRXDVS_UPD_FORCE_CYC_B1) |
        P_Fld(0x0, SHU_B1_DQ8_R_DMRXDVS_UPD_FORCE_EN_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ5, P_Fld(0x0e, SHU_B0_DQ5_RG_RX_ARDQ_VREF_SEL_B0) |
        P_Fld(0x0, SHU_B0_DQ5_RG_RX_ARDQ_VREF_BYPASS_B0) | P_Fld(0x00, SHU_B0_DQ5_RG_ARPI_FB_B0) |
        P_Fld(0x0, SHU_B0_DQ5_RG_RX_ARDQS0_DQSIEN_DLY_B0) | P_Fld(0x0, SHU_B0_DQ5_RG_RX_ARDQS_DQSIEN_RB_DLY_B0) |
        P_Fld(0x5, SHU_B0_DQ5_RG_RX_ARDQS0_DVS_DLY_B0) | P_Fld(0x0, SHU_B0_DQ5_RG_RX_ARDQ_FIFO_DQSI_DLY_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ5, P_Fld(0x0e, SHU_B1_DQ5_RG_RX_ARDQ_VREF_SEL_B1) |
        P_Fld(0x0, SHU_B1_DQ5_RG_RX_ARDQ_VREF_BYPASS_B1) | P_Fld(0x00, SHU_B1_DQ5_RG_ARPI_FB_B1) |
        P_Fld(0x0, SHU_B1_DQ5_RG_RX_ARDQS0_DQSIEN_DLY_B1) | P_Fld(0x0, SHU_B1_DQ5_RG_RX_ARDQS_DQSIEN_RB_DLY_B1) |
        P_Fld(0x5, SHU_B1_DQ5_RG_RX_ARDQS0_DVS_DLY_B1) | P_Fld(0x0, SHU_B1_DQ5_RG_RX_ARDQ_FIFO_DQSI_DLY_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY0, P_Fld(0x64, SHU_R0_B0_RXDLY0_RX_ARDQ0_F_DLY_B0) | 
        P_Fld(0x64, SHU_R0_B0_RXDLY0_RX_ARDQ1_F_DLY_B0));

#if !CODE_SIZE_REDUCE
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY0, P_Fld(0x64, SHU_R0_B0_RXDLY0_RX_ARDQ0_R_DLY_B0) |
        P_Fld(0x64, SHU_R0_B0_RXDLY0_RX_ARDQ1_R_DLY_B0));
#endif


vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY1, P_Fld(0x64, SHU_R0_B0_RXDLY1_RX_ARDQ2_R_DLY_B0) |
        P_Fld(0x64, SHU_R0_B0_RXDLY1_RX_ARDQ2_F_DLY_B0) | P_Fld(0x64, SHU_R0_B0_RXDLY1_RX_ARDQ3_R_DLY_B0) |
        P_Fld(0x64, SHU_R0_B0_RXDLY1_RX_ARDQ3_F_DLY_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY2, P_Fld(0x64, SHU_R0_B0_RXDLY2_RX_ARDQ4_R_DLY_B0) |
        P_Fld(0x64, SHU_R0_B0_RXDLY2_RX_ARDQ4_F_DLY_B0) | P_Fld(0x64, SHU_R0_B0_RXDLY2_RX_ARDQ5_R_DLY_B0) |
        P_Fld(0x64, SHU_R0_B0_RXDLY2_RX_ARDQ5_F_DLY_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY3, P_Fld(0x64, SHU_R0_B0_RXDLY3_RX_ARDQ6_R_DLY_B0) |
        P_Fld(0x64, SHU_R0_B0_RXDLY3_RX_ARDQ6_F_DLY_B0) | P_Fld(0x64, SHU_R0_B0_RXDLY3_RX_ARDQ7_R_DLY_B0) |
        P_Fld(0x64, SHU_R0_B0_RXDLY3_RX_ARDQ7_F_DLY_B0));

#if !CODE_SIZE_REDUCE
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY4, P_Fld(0x64, SHU_R0_B0_RXDLY4_RX_ARDQM0_R_DLY_B0) |
        P_Fld(0x64, SHU_R0_B0_RXDLY4_RX_ARDQM0_F_DLY_B0));
#endif

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY5, P_Fld(0x0da, SHU_R0_B0_RXDLY5_RX_ARDQS0_R_DLY_B0) |
        P_Fld(0x0da, SHU_R0_B0_RXDLY5_RX_ARDQS0_F_DLY_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x63, SHU_R0_B0_RXDLY0_RX_ARDQ0_F_DLY_B0) | 
        P_Fld(0x63, SHU_R0_B0_RXDLY0_RX_ARDQ1_F_DLY_B0));

#if !CODE_SIZE_REDUCE
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x63, SHU_R0_B0_RXDLY0_RX_ARDQ0_R_DLY_B0) |
        P_Fld(0x63, SHU_R0_B0_RXDLY0_RX_ARDQ1_R_DLY_B0));
#endif


vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY1+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x63, SHU_R0_B0_RXDLY1_RX_ARDQ2_R_DLY_B0) |
        P_Fld(0x63, SHU_R0_B0_RXDLY1_RX_ARDQ2_F_DLY_B0) | P_Fld(0x63, SHU_R0_B0_RXDLY1_RX_ARDQ3_R_DLY_B0) |
        P_Fld(0x63, SHU_R0_B0_RXDLY1_RX_ARDQ3_F_DLY_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY2+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x63, SHU_R0_B0_RXDLY2_RX_ARDQ4_R_DLY_B0) |
        P_Fld(0x63, SHU_R0_B0_RXDLY2_RX_ARDQ4_F_DLY_B0) | P_Fld(0x63, SHU_R0_B0_RXDLY2_RX_ARDQ5_R_DLY_B0) |
        P_Fld(0x63, SHU_R0_B0_RXDLY2_RX_ARDQ5_F_DLY_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY3+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x63, SHU_R0_B0_RXDLY3_RX_ARDQ6_R_DLY_B0) |
        P_Fld(0x63, SHU_R0_B0_RXDLY3_RX_ARDQ6_F_DLY_B0) | P_Fld(0x63, SHU_R0_B0_RXDLY3_RX_ARDQ7_R_DLY_B0) |
        P_Fld(0x63, SHU_R0_B0_RXDLY3_RX_ARDQ7_F_DLY_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY4+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x63, SHU_R0_B0_RXDLY4_RX_ARDQM0_R_DLY_B0) |
        P_Fld(0x63, SHU_R0_B0_RXDLY4_RX_ARDQM0_F_DLY_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY5+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x0d9, SHU_R0_B0_RXDLY5_RX_ARDQS0_R_DLY_B0) |
        P_Fld(0x0d9, SHU_R0_B0_RXDLY5_RX_ARDQS0_F_DLY_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY0, P_Fld(0x64, SHU_R0_B1_RXDLY0_RX_ARDQ0_R_DLY_B1) |
        P_Fld(0x64, SHU_R0_B1_RXDLY0_RX_ARDQ0_F_DLY_B1) | P_Fld(0x64, SHU_R0_B1_RXDLY0_RX_ARDQ1_R_DLY_B1) |
        P_Fld(0x64, SHU_R0_B1_RXDLY0_RX_ARDQ1_F_DLY_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY1, P_Fld(0x64, SHU_R0_B1_RXDLY1_RX_ARDQ2_R_DLY_B1) |
        P_Fld(0x64, SHU_R0_B1_RXDLY1_RX_ARDQ2_F_DLY_B1) | P_Fld(0x64, SHU_R0_B1_RXDLY1_RX_ARDQ3_R_DLY_B1) |
        P_Fld(0x64, SHU_R0_B1_RXDLY1_RX_ARDQ3_F_DLY_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY2, P_Fld(0x64, SHU_R0_B1_RXDLY2_RX_ARDQ4_R_DLY_B1) |
        P_Fld(0x64, SHU_R0_B1_RXDLY2_RX_ARDQ4_F_DLY_B1) | P_Fld(0x64, SHU_R0_B1_RXDLY2_RX_ARDQ5_R_DLY_B1) |
        P_Fld(0x64, SHU_R0_B1_RXDLY2_RX_ARDQ5_F_DLY_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY3, P_Fld(0x64, SHU_R0_B1_RXDLY3_RX_ARDQ6_R_DLY_B1) |
        P_Fld(0x64, SHU_R0_B1_RXDLY3_RX_ARDQ6_F_DLY_B1) | P_Fld(0x64, SHU_R0_B1_RXDLY3_RX_ARDQ7_R_DLY_B1) |
        P_Fld(0x64, SHU_R0_B1_RXDLY3_RX_ARDQ7_F_DLY_B1));

#if !CODE_SIZE_REDUCE
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY4, P_Fld(0x64, SHU_R0_B1_RXDLY4_RX_ARDQM0_R_DLY_B1) |
        P_Fld(0x64, SHU_R0_B1_RXDLY4_RX_ARDQM0_F_DLY_B1));
#endif

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY5, P_Fld(0x0da, SHU_R0_B1_RXDLY5_RX_ARDQS0_R_DLY_B1) |
        P_Fld(0x0da, SHU_R0_B1_RXDLY5_RX_ARDQS0_F_DLY_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x63, SHU_R0_B1_RXDLY0_RX_ARDQ0_R_DLY_B1) |
        P_Fld(0x63, SHU_R0_B1_RXDLY0_RX_ARDQ0_F_DLY_B1) | P_Fld(0x63, SHU_R0_B1_RXDLY0_RX_ARDQ1_R_DLY_B1) |
        P_Fld(0x63, SHU_R0_B1_RXDLY0_RX_ARDQ1_F_DLY_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY1+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x63, SHU_R0_B1_RXDLY1_RX_ARDQ2_R_DLY_B1) |
        P_Fld(0x63, SHU_R0_B1_RXDLY1_RX_ARDQ2_F_DLY_B1) | P_Fld(0x63, SHU_R0_B1_RXDLY1_RX_ARDQ3_R_DLY_B1) |
        P_Fld(0x63, SHU_R0_B1_RXDLY1_RX_ARDQ3_F_DLY_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY2+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x63, SHU_R0_B1_RXDLY2_RX_ARDQ4_R_DLY_B1) |
        P_Fld(0x63, SHU_R0_B1_RXDLY2_RX_ARDQ4_F_DLY_B1) | P_Fld(0x63, SHU_R0_B1_RXDLY2_RX_ARDQ5_R_DLY_B1) |
        P_Fld(0x63, SHU_R0_B1_RXDLY2_RX_ARDQ5_F_DLY_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY3+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x63, SHU_R0_B1_RXDLY3_RX_ARDQ6_R_DLY_B1) |
        P_Fld(0x63, SHU_R0_B1_RXDLY3_RX_ARDQ6_F_DLY_B1) | P_Fld(0x63, SHU_R0_B1_RXDLY3_RX_ARDQ7_R_DLY_B1) |
        P_Fld(0x63, SHU_R0_B1_RXDLY3_RX_ARDQ7_F_DLY_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY4+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x63, SHU_R0_B1_RXDLY4_RX_ARDQM0_R_DLY_B1) |
        P_Fld(0x63, SHU_R0_B1_RXDLY4_RX_ARDQM0_F_DLY_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY5+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x0d9, SHU_R0_B1_RXDLY5_RX_ARDQS0_R_DLY_B1) |
        P_Fld(0x0d9, SHU_R0_B1_RXDLY5_RX_ARDQS0_F_DLY_B1));

vIO32WriteFldMulti(DDRPHY_REG_B0_DQ9, P_Fld(0x1, B0_DQ9_RG_RX_ARDQ_STBEN_RESETB_B0) |
        P_Fld(0x0, B0_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B0) | P_Fld(0x0, B0_DQ9_RG_RX_ARDQS0_DQSIENMODE_B0) |
        P_Fld(0x1, B0_DQ9_R_DMRXDVS_R_F_DLY_RK_OPT_B0) | P_Fld(0x0, B0_DQ9_R_DMRXFIFO_STBENCMP_EN_B0) |
        P_Fld(0x00, B0_DQ9_R_IN_GATE_EN_LOW_OPT_B0) | P_Fld(0x0, B0_DQ9_R_DMDQSIEN_VALID_LAT_B0) |
        P_Fld(0x0, B0_DQ9_R_DMDQSIEN_RDSEL_LAT_B0) | P_Fld(0x0, B0_DQ9_R_DMRXDVS_VALID_LAT_B0) |
        P_Fld(0x0, B0_DQ9_R_DMRXDVS_RDSEL_LAT_B0));

vIO32WriteFldMulti(DDRPHY_REG_B1_DQ9, P_Fld(0x1, B1_DQ9_RG_RX_ARDQ_STBEN_RESETB_B1) |
        P_Fld(0x0, B1_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B1) | P_Fld(0x0, B1_DQ9_RG_RX_ARDQS0_DQSIENMODE_B1) |
        P_Fld(0x1, B1_DQ9_R_DMRXDVS_R_F_DLY_RK_OPT_B1) | P_Fld(0x0, B1_DQ9_R_DMRXFIFO_STBENCMP_EN_B1) |
        P_Fld(0x00, B1_DQ9_R_IN_GATE_EN_LOW_OPT_B1) | P_Fld(0x0, B1_DQ9_R_DMDQSIEN_VALID_LAT_B1) |
        P_Fld(0x0, B1_DQ9_R_DMDQSIEN_RDSEL_LAT_B1) | P_Fld(0x0, B1_DQ9_R_DMRXDVS_VALID_LAT_B1) |
        P_Fld(0x0, B1_DQ9_R_DMRXDVS_RDSEL_LAT_B1));

vIO32WriteFldMulti(DDRPHY_REG_B0_DQ4, P_Fld(0x6e, B0_DQ4_RG_RX_ARDQS_EYE_R_DLY_B0) |
        P_Fld(0x6e, B0_DQ4_RG_RX_ARDQS_EYE_F_DLY_B0) | P_Fld(0x24, B0_DQ4_RG_RX_ARDQ_EYE_R_DLY_B0) |
        P_Fld(0x24, B0_DQ4_RG_RX_ARDQ_EYE_F_DLY_B0));

vIO32WriteFldMulti(DDRPHY_REG_B1_DQ4, P_Fld(0x6e, B1_DQ4_RG_RX_ARDQS_EYE_R_DLY_B1) |
        P_Fld(0x6e, B1_DQ4_RG_RX_ARDQS_EYE_F_DLY_B1) | P_Fld(0x24, B1_DQ4_RG_RX_ARDQ_EYE_R_DLY_B1) |
        P_Fld(0x24, B1_DQ4_RG_RX_ARDQ_EYE_F_DLY_B1));

vIO32WriteFldMulti(DDRPHY_REG_B0_DQ5, P_Fld(0x0e, B0_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B0) |
        P_Fld(0x0, B0_DQ5_RG_RX_ARDQ_VREF_EN_B0) | P_Fld(0x0, B0_DQ5_RG_RX_ARDQ_EYE_VREF_EN_B0) |
        P_Fld(0x0, B0_DQ5_RG_RX_ARDQ_EYE_SEL_B0) | P_Fld(0x0, B0_DQ5_RG_RX_ARDQ_EYE_EN_B0) |
        P_Fld(0x1, B0_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B0) | P_Fld(0x0, B0_DQ5_RG_RX_ARDQS0_DVS_EN_B0));

vIO32WriteFldMulti(DDRPHY_REG_B1_DQ5, P_Fld(0x0e, B1_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B1) |
        P_Fld(0x0, B1_DQ5_RG_RX_ARDQ_VREF_EN_B1) | P_Fld(0x0, B1_DQ5_RG_RX_ARDQ_EYE_VREF_EN_B1) |
        P_Fld(0x0, B1_DQ5_RG_RX_ARDQ_EYE_SEL_B1) | P_Fld(0x0, B1_DQ5_RG_RX_ARDQ_EYE_EN_B1) |
        P_Fld(0x1, B1_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B1) | P_Fld(0x0, B1_DQ5_RG_RX_ARDQS0_DVS_EN_B1));

}

void sv_algorithm_assistance_LP4_400(DRAMC_CTX_T *p)
{

    #if !(CODE_SIZE_REDUCE && AC_TIMING_DERATE_ENABLE)
    vIO32WriteFldMulti(DRAMC_REG_SHU_AC_DERATING0, P_Fld(0x0, SHU_AC_DERATING0_ACDERATEEN) |
            P_Fld(0x1, SHU_AC_DERATING0_TRRD_DERATE) | P_Fld(0x2, SHU_AC_DERATING0_TRCD_DERATE));

    vIO32WriteFldMulti(DRAMC_REG_SHU_AC_DERATING1, P_Fld(0x1, SHU_AC_DERATING1_TRPAB_DERATE) |
            P_Fld(0x0, SHU_AC_DERATING1_TRP_DERATE) | P_Fld(0x00, SHU_AC_DERATING1_TRAS_DERATE) |
            P_Fld(0x00, SHU_AC_DERATING1_TRC_DERATE));
	#endif

    vIO32WriteFldMulti(DRAMC_REG_SHU_SREF_CTRL, P_Fld(0x3, SHU_SREF_CTRL_CKEHCMD) |
            P_Fld(0x3, SHU_SREF_CTRL_SREF_CK_DLY));

    vIO32WriteFldMulti(DRAMC_REG_SHU_HMR4_DVFS_CTRL0, P_Fld(0x1e, SHU_HMR4_DVFS_CTRL0_FSPCHG_PRDCNT) |
            P_Fld(0x000, SHU_HMR4_DVFS_CTRL0_REFRCNT));

    vIO32WriteFldMulti(DRAMC_REG_SHU_AC_TIME_05T,
            P_Fld(0x0, SHU_AC_TIME_05T_TCKEPRD_05T) | P_Fld(0x0, SHU_AC_TIME_05T_BGTRRD_05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_BGTCCD_05T) | P_Fld(0x1, SHU_AC_TIME_05T_BGTWTR_M05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_XRTR2W_05T) | P_Fld(0x0, SHU_AC_TIME_05T_XRTW2R_M05T));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DRAMC_REG_SHU_AC_TIME_05T, P_Fld(0x0, SHU_AC_TIME_05T_TRC_05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_TRFCPB_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TRFC_05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_TPBR2PBR_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TXP_05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_TRTP_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TRCD_05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_TRP_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TRPAB_05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_TRAS_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TWR_M05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_TRRD_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TFAW_05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_TR2PD_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TWTPD_M05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_TMRRI_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TMRWCKEL_05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_TR2W_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TWTR_M05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_TMRD_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TMRW_05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_TMRR2MRW_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TW2MRW_05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_TR2MRW_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TPBR2ACT_05T));

    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM_XRT, P_Fld(0x05, SHU_ACTIM_XRT_XRTR2R) |
            P_Fld(0x0a, SHU_ACTIM_XRT_XRTR2W) | P_Fld(0x7, SHU_ACTIM_XRT_XRTW2R) |
            P_Fld(0x0a, SHU_ACTIM_XRT_XRTW2W));

    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM0, P_Fld(0x0a, SHU_ACTIM0_TWTR) |
            P_Fld(0x0b, SHU_ACTIM0_TWR) | P_Fld(0x1, SHU_ACTIM0_TRRD) |
            P_Fld(0x2, SHU_ACTIM0_TRCD));
    #endif
    vIO32WriteFldAlign(DRAMC_REG_SHU_ACTIM0, 0x3, SHU_ACTIM0_CKELCKCNT);

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM1, P_Fld(0x1, SHU_ACTIM1_TRPAB) |
            P_Fld(0x7, SHU_ACTIM1_TMRWCKEL) | P_Fld(0x0, SHU_ACTIM1_TRP) |
            P_Fld(0x00, SHU_ACTIM1_TRAS) | P_Fld(0x00, SHU_ACTIM1_TRC));

    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM2, P_Fld(0x0, SHU_ACTIM2_TXP) |
            P_Fld(0x05, SHU_ACTIM2_TMRRI) | P_Fld(0x3, SHU_ACTIM2_TRTP) |
            P_Fld(0x09, SHU_ACTIM2_TR2W) | P_Fld(0x00, SHU_ACTIM2_TFAW));

    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM3, P_Fld(0x07, SHU_ACTIM3_TRFCPB) |
            P_Fld(0x8, SHU_ACTIM3_TR2MRR) | P_Fld(0x1a, SHU_ACTIM3_TRFC));
    #endif

    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM3, P_Fld(0x8, SHU_ACTIM3_MANTMRR) |
            P_Fld(0x05, SHU_ACTIM3_TWTR_L));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM4, P_Fld(0x027, SHU_ACTIM4_TXREFCNT) |
            P_Fld(0x0e, SHU_ACTIM4_TMRR2MRW) | P_Fld(0x0c, SHU_ACTIM4_TMRR2W) |
            P_Fld(0x07, SHU_ACTIM4_TZQCS));

    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM5, P_Fld(0x10, SHU_ACTIM5_TR2PD) |
            P_Fld(0x0f, SHU_ACTIM5_TWTPD) | P_Fld(0x0b, SHU_ACTIM5_TPBR2PBR) |
            P_Fld(0x0, SHU_ACTIM5_TPBR2ACT));

    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM6, P_Fld(0x04, SHU_ACTIM6_TZQLAT2) |
            P_Fld(0x6, SHU_ACTIM6_TMRD) | P_Fld(0x5, SHU_ACTIM6_TMRW) |
            P_Fld(0x0d, SHU_ACTIM6_TW2MRW) | P_Fld(0x11, SHU_ACTIM6_TR2MRW));

    vIO32WriteFldMulti(DRAMC_REG_SHU_CKECTRL, P_Fld(0x0, SHU_CKECTRL_TPDE_05T) |
            P_Fld(0x0, SHU_CKECTRL_TPDX_05T) | P_Fld(0x3, SHU_CKECTRL_TPDE) |
            P_Fld(0x3, SHU_CKECTRL_TPDX) | P_Fld(0x2, SHU_CKECTRL_TCKEPRD));
    #endif
    vIO32WriteFldAlign(DRAMC_REG_SHU_CKECTRL, 0x3, SHU_CKECTRL_TCKESRX);

    vIO32WriteFldMulti(DRAMC_REG_SHU_MISC,
            P_Fld(0x7, SHU_MISC_DCMDLYREF) | P_Fld(0x0, SHU_MISC_DAREFEN));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldAlign(DRAMC_REG_SHU_MISC, 0x2, SHU_MISC_REQQUE_MAXCNT);

    vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ8, P_Fld(0x0, SHU_B0_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B0) |
            P_Fld(0x0, SHU_B0_DQ8_R_RMRODTEN_CG_IG_B0) | P_Fld(0x1, SHU_B0_DQ8_R_RMRX_TOPHY_CG_IG_B0) |
            P_Fld(0x0, SHU_B0_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B0) |
            P_Fld(0x1, SHU_B0_DQ8_R_DMRXDLY_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B0) |
            P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B0) |
            P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMRANK_PIPE_CG_IG_B0) |
            P_Fld(0x0, SHU_B0_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B0));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ8, P_Fld(0x0018, SHU_B0_DQ8_R_DMRXDVS_UPD_FORCE_CYC_B0) |
            P_Fld(0x0, SHU_B0_DQ8_R_DMRXDVS_UPD_FORCE_EN_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ8, P_Fld(0x0018, SHU_B1_DQ8_R_DMRXDVS_UPD_FORCE_CYC_B1) |
            P_Fld(0x0, SHU_B1_DQ8_R_DMRXDVS_UPD_FORCE_EN_B1));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ8, P_Fld(0x0, SHU_B1_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B1) |
            P_Fld(0x0, SHU_B1_DQ8_R_RMRODTEN_CG_IG_B1) | P_Fld(0x1, SHU_B1_DQ8_R_RMRX_TOPHY_CG_IG_B1) |
            P_Fld(0x0, SHU_B1_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B1) |
            P_Fld(0x1, SHU_B1_DQ8_R_DMRXDLY_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B1) |
            P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B1) |
            P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMRANK_PIPE_CG_IG_B1) |
            P_Fld(0x0, SHU_B1_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B1));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ5, P_Fld(0x0e, SHU_B0_DQ5_RG_RX_ARDQ_VREF_SEL_B0) |
            P_Fld(0x0, SHU_B0_DQ5_RG_RX_ARDQ_VREF_BYPASS_B0) | P_Fld(0x00, SHU_B0_DQ5_RG_ARPI_FB_B0) |
            P_Fld(0x0, SHU_B0_DQ5_RG_RX_ARDQS0_DQSIEN_DLY_B0) | P_Fld(0x0, SHU_B0_DQ5_RG_RX_ARDQS_DQSIEN_RB_DLY_B0) |
            P_Fld(0x7, SHU_B0_DQ5_RG_RX_ARDQS0_DVS_DLY_B0) | P_Fld(0x0, SHU_B0_DQ5_RG_RX_ARDQ_FIFO_DQSI_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ5, P_Fld(0x0e, SHU_B1_DQ5_RG_RX_ARDQ_VREF_SEL_B1) |
            P_Fld(0x0, SHU_B1_DQ5_RG_RX_ARDQ_VREF_BYPASS_B1) | P_Fld(0x00, SHU_B1_DQ5_RG_ARPI_FB_B1) |
            P_Fld(0x0, SHU_B1_DQ5_RG_RX_ARDQS0_DQSIEN_DLY_B1) | P_Fld(0x0, SHU_B1_DQ5_RG_RX_ARDQS_DQSIEN_RB_DLY_B1) |
            P_Fld(0x7, SHU_B1_DQ5_RG_RX_ARDQS0_DVS_DLY_B1) | P_Fld(0x0, SHU_B1_DQ5_RG_RX_ARDQ_FIFO_DQSI_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY0, P_Fld(0x68, SHU_R0_B0_RXDLY0_RX_ARDQ0_F_DLY_B0) |
            P_Fld(0x68, SHU_R0_B0_RXDLY0_RX_ARDQ1_F_DLY_B0));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY0, P_Fld(0x68, SHU_R0_B0_RXDLY0_RX_ARDQ0_R_DLY_B0) |
            P_Fld(0x68, SHU_R0_B0_RXDLY0_RX_ARDQ1_R_DLY_B0));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY1, P_Fld(0x68, SHU_R0_B0_RXDLY1_RX_ARDQ2_R_DLY_B0) |
            P_Fld(0x68, SHU_R0_B0_RXDLY1_RX_ARDQ2_F_DLY_B0) | P_Fld(0x68, SHU_R0_B0_RXDLY1_RX_ARDQ3_R_DLY_B0) |
            P_Fld(0x68, SHU_R0_B0_RXDLY1_RX_ARDQ3_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY2, P_Fld(0x68, SHU_R0_B0_RXDLY2_RX_ARDQ4_R_DLY_B0) |
            P_Fld(0x68, SHU_R0_B0_RXDLY2_RX_ARDQ4_F_DLY_B0) | P_Fld(0x68, SHU_R0_B0_RXDLY2_RX_ARDQ5_R_DLY_B0) |
            P_Fld(0x68, SHU_R0_B0_RXDLY2_RX_ARDQ5_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY3, P_Fld(0x68, SHU_R0_B0_RXDLY3_RX_ARDQ6_R_DLY_B0) |
            P_Fld(0x68, SHU_R0_B0_RXDLY3_RX_ARDQ6_F_DLY_B0) | P_Fld(0x68, SHU_R0_B0_RXDLY3_RX_ARDQ7_R_DLY_B0) |
            P_Fld(0x68, SHU_R0_B0_RXDLY3_RX_ARDQ7_F_DLY_B0));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY4, P_Fld(0x68, SHU_R0_B0_RXDLY4_RX_ARDQM0_R_DLY_B0) |
            P_Fld(0x68, SHU_R0_B0_RXDLY4_RX_ARDQM0_F_DLY_B0));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY5, P_Fld(0x14a, SHU_R0_B0_RXDLY5_RX_ARDQS0_R_DLY_B0) |
            P_Fld(0x14a, SHU_R0_B0_RXDLY5_RX_ARDQS0_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0xc6, SHU_R0_B0_RXDLY0_RX_ARDQ0_F_DLY_B0) |
            P_Fld(0xc6, SHU_R0_B0_RXDLY0_RX_ARDQ1_F_DLY_B0));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0xc6, SHU_R0_B0_RXDLY0_RX_ARDQ0_R_DLY_B0) |
            P_Fld(0xc6, SHU_R0_B0_RXDLY0_RX_ARDQ1_R_DLY_B0));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY1+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0xc6, SHU_R0_B0_RXDLY1_RX_ARDQ2_R_DLY_B0) |
            P_Fld(0xc6, SHU_R0_B0_RXDLY1_RX_ARDQ2_F_DLY_B0) | P_Fld(0xc6, SHU_R0_B0_RXDLY1_RX_ARDQ3_R_DLY_B0) |
            P_Fld(0xc6, SHU_R0_B0_RXDLY1_RX_ARDQ3_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY2+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0xc6, SHU_R0_B0_RXDLY2_RX_ARDQ4_R_DLY_B0) |
            P_Fld(0xc6, SHU_R0_B0_RXDLY2_RX_ARDQ4_F_DLY_B0) | P_Fld(0xc6, SHU_R0_B0_RXDLY2_RX_ARDQ5_R_DLY_B0) |
            P_Fld(0xc6, SHU_R0_B0_RXDLY2_RX_ARDQ5_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY3+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0xc6, SHU_R0_B0_RXDLY3_RX_ARDQ6_R_DLY_B0) |
            P_Fld(0xc6, SHU_R0_B0_RXDLY3_RX_ARDQ6_F_DLY_B0) | P_Fld(0xc6, SHU_R0_B0_RXDLY3_RX_ARDQ7_R_DLY_B0) |
            P_Fld(0xc6, SHU_R0_B0_RXDLY3_RX_ARDQ7_F_DLY_B0));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY4+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0xc6, SHU_R0_B0_RXDLY4_RX_ARDQM0_R_DLY_B0) |
            P_Fld(0xc6, SHU_R0_B0_RXDLY4_RX_ARDQM0_F_DLY_B0));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY5+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x1a1, SHU_R0_B0_RXDLY5_RX_ARDQS0_R_DLY_B0) |
            P_Fld(0x1a1, SHU_R0_B0_RXDLY5_RX_ARDQS0_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY0, P_Fld(0x06, SHU_R0_B1_RXDLY0_RX_ARDQ0_F_DLY_B1) |
            P_Fld(0x06, SHU_R0_B1_RXDLY0_RX_ARDQ1_F_DLY_B1));
    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY0, P_Fld(0x06, SHU_R0_B1_RXDLY0_RX_ARDQ0_R_DLY_B1) |
            P_Fld(0x06, SHU_R0_B1_RXDLY0_RX_ARDQ1_R_DLY_B1));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY1, P_Fld(0x06, SHU_R0_B1_RXDLY1_RX_ARDQ2_R_DLY_B1) |
            P_Fld(0x06, SHU_R0_B1_RXDLY1_RX_ARDQ2_F_DLY_B1) | P_Fld(0x06, SHU_R0_B1_RXDLY1_RX_ARDQ3_R_DLY_B1) |
            P_Fld(0x06, SHU_R0_B1_RXDLY1_RX_ARDQ3_F_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY2, P_Fld(0x06, SHU_R0_B1_RXDLY2_RX_ARDQ4_R_DLY_B1) |
            P_Fld(0x06, SHU_R0_B1_RXDLY2_RX_ARDQ4_F_DLY_B1) | P_Fld(0x06, SHU_R0_B1_RXDLY2_RX_ARDQ5_R_DLY_B1) |
            P_Fld(0x06, SHU_R0_B1_RXDLY2_RX_ARDQ5_F_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY3, P_Fld(0x06, SHU_R0_B1_RXDLY3_RX_ARDQ6_R_DLY_B1) |
            P_Fld(0x06, SHU_R0_B1_RXDLY3_RX_ARDQ6_F_DLY_B1) | P_Fld(0x06, SHU_R0_B1_RXDLY3_RX_ARDQ7_R_DLY_B1) |
            P_Fld(0x06, SHU_R0_B1_RXDLY3_RX_ARDQ7_F_DLY_B1));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY4, P_Fld(0x06, SHU_R0_B1_RXDLY4_RX_ARDQM0_R_DLY_B1) |
            P_Fld(0x06, SHU_R0_B1_RXDLY4_RX_ARDQM0_F_DLY_B1));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY5, P_Fld(0x0e5, SHU_R0_B1_RXDLY5_RX_ARDQS0_R_DLY_B1) |
            P_Fld(0x0e5, SHU_R0_B1_RXDLY5_RX_ARDQS0_F_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x65, SHU_R0_B1_RXDLY0_RX_ARDQ0_F_DLY_B1) |
            P_Fld(0x65, SHU_R0_B1_RXDLY0_RX_ARDQ1_F_DLY_B1));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x65, SHU_R0_B1_RXDLY0_RX_ARDQ0_R_DLY_B1) |
            P_Fld(0x65, SHU_R0_B1_RXDLY0_RX_ARDQ1_R_DLY_B1));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY1+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x65, SHU_R0_B1_RXDLY1_RX_ARDQ2_R_DLY_B1) |
            P_Fld(0x65, SHU_R0_B1_RXDLY1_RX_ARDQ2_F_DLY_B1) | P_Fld(0x65, SHU_R0_B1_RXDLY1_RX_ARDQ3_R_DLY_B1) |
            P_Fld(0x65, SHU_R0_B1_RXDLY1_RX_ARDQ3_F_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY2+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x65, SHU_R0_B1_RXDLY2_RX_ARDQ4_R_DLY_B1) |
            P_Fld(0x65, SHU_R0_B1_RXDLY2_RX_ARDQ4_F_DLY_B1) | P_Fld(0x65, SHU_R0_B1_RXDLY2_RX_ARDQ5_R_DLY_B1) |
            P_Fld(0x65, SHU_R0_B1_RXDLY2_RX_ARDQ5_F_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY3+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x65, SHU_R0_B1_RXDLY3_RX_ARDQ6_R_DLY_B1) |
            P_Fld(0x65, SHU_R0_B1_RXDLY3_RX_ARDQ6_F_DLY_B1) | P_Fld(0x65, SHU_R0_B1_RXDLY3_RX_ARDQ7_R_DLY_B1) |
            P_Fld(0x65, SHU_R0_B1_RXDLY3_RX_ARDQ7_F_DLY_B1));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY4+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x65, SHU_R0_B1_RXDLY4_RX_ARDQM0_R_DLY_B1) |
            P_Fld(0x65, SHU_R0_B1_RXDLY4_RX_ARDQM0_F_DLY_B1));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY5+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x141, SHU_R0_B1_RXDLY5_RX_ARDQS0_R_DLY_B1) |
            P_Fld(0x141, SHU_R0_B1_RXDLY5_RX_ARDQS0_F_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_B0_DQ9, P_Fld(0x1, B0_DQ9_RG_RX_ARDQ_STBEN_RESETB_B0) |
            P_Fld(0x0, B0_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B0) | P_Fld(0x0, B0_DQ9_RG_RX_ARDQS0_DQSIENMODE_B0) |
            P_Fld(0x1, B0_DQ9_R_DMRXDVS_R_F_DLY_RK_OPT_B0) | P_Fld(0x0, B0_DQ9_R_DMRXFIFO_STBENCMP_EN_B0) |
            P_Fld(0x00, B0_DQ9_R_IN_GATE_EN_LOW_OPT_B0) | P_Fld(0x0, B0_DQ9_R_DMDQSIEN_VALID_LAT_B0) |
            P_Fld(0x0, B0_DQ9_R_DMDQSIEN_RDSEL_LAT_B0) | P_Fld(0x0, B0_DQ9_R_DMRXDVS_VALID_LAT_B0) |
            P_Fld(0x0, B0_DQ9_R_DMRXDVS_RDSEL_LAT_B0));

    vIO32WriteFldMulti(DDRPHY_REG_B1_DQ9, P_Fld(0x1, B1_DQ9_RG_RX_ARDQ_STBEN_RESETB_B1) |
            P_Fld(0x0, B1_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B1) | P_Fld(0x0, B1_DQ9_RG_RX_ARDQS0_DQSIENMODE_B1) |
            P_Fld(0x1, B1_DQ9_R_DMRXDVS_R_F_DLY_RK_OPT_B1) | P_Fld(0x0, B1_DQ9_R_DMRXFIFO_STBENCMP_EN_B1) |
            P_Fld(0x00, B1_DQ9_R_IN_GATE_EN_LOW_OPT_B1) | P_Fld(0x0, B1_DQ9_R_DMDQSIEN_VALID_LAT_B1) |
            P_Fld(0x0, B1_DQ9_R_DMDQSIEN_RDSEL_LAT_B1) | P_Fld(0x0, B1_DQ9_R_DMRXDVS_VALID_LAT_B1) |
            P_Fld(0x0, B1_DQ9_R_DMRXDVS_RDSEL_LAT_B1));

    vIO32WriteFldMulti(DDRPHY_REG_B0_DQ4, P_Fld(0x76, B0_DQ4_RG_RX_ARDQS_EYE_R_DLY_B0) |
            P_Fld(0x76, B0_DQ4_RG_RX_ARDQS_EYE_F_DLY_B0) | P_Fld(0x28, B0_DQ4_RG_RX_ARDQ_EYE_R_DLY_B0) |
            P_Fld(0x28, B0_DQ4_RG_RX_ARDQ_EYE_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_B1_DQ4, P_Fld(0x14, B1_DQ4_RG_RX_ARDQS_EYE_R_DLY_B1) |
            P_Fld(0x14, B1_DQ4_RG_RX_ARDQS_EYE_F_DLY_B1) | P_Fld(0x06, B1_DQ4_RG_RX_ARDQ_EYE_R_DLY_B1) |
            P_Fld(0x06, B1_DQ4_RG_RX_ARDQ_EYE_F_DLY_B1));

vIO32WriteFldMulti(DDRPHY_REG_B0_DQ5, P_Fld(0x0e, B0_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B0) |
        P_Fld(0x0, B0_DQ5_RG_RX_ARDQ_VREF_EN_B0) | P_Fld(0x0, B0_DQ5_RG_RX_ARDQ_EYE_VREF_EN_B0) |
        P_Fld(0x0, B0_DQ5_RG_RX_ARDQ_EYE_SEL_B0) | P_Fld(0x0, B0_DQ5_RG_RX_ARDQ_EYE_EN_B0) |
        P_Fld(0x1, B0_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B0) | P_Fld(0x0, B0_DQ5_RG_RX_ARDQS0_DVS_EN_B0));

vIO32WriteFldMulti(DDRPHY_REG_B1_DQ5, P_Fld(0x0e, B1_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B1) |
        P_Fld(0x0, B1_DQ5_RG_RX_ARDQ_VREF_EN_B1) | P_Fld(0x0, B1_DQ5_RG_RX_ARDQ_EYE_VREF_EN_B1) |
        P_Fld(0x0, B1_DQ5_RG_RX_ARDQ_EYE_SEL_B1) | P_Fld(0x0, B1_DQ5_RG_RX_ARDQ_EYE_EN_B1) |
        P_Fld(0x1, B1_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B1) | P_Fld(0x0, B1_DQ5_RG_RX_ARDQS0_DVS_EN_B1));

}

void sv_algorithm_assistance_LP4_800(DRAMC_CTX_T *p)
{

    vIO32WriteFldMulti(DDRPHY_REG_SHU_MISC_RDSEL_TRACK, P_Fld(0x0e, SHU_MISC_RDSEL_TRACK_DMDATLAT_I) |
            P_Fld(0x1, SHU_MISC_RDSEL_TRACK_RDSEL_HWSAVE_MSK) | P_Fld(0x0, SHU_MISC_RDSEL_TRACK_RDSEL_TRACK_EN) |
            P_Fld(0xff5, SHU_MISC_RDSEL_TRACK_SHU_GW_THRD_NEG) | P_Fld(0x00b, SHU_MISC_RDSEL_TRACK_SHU_GW_THRD_POS));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RDAT, P_Fld(0x0e, MISC_SHU_RDAT_DATLAT) |
            P_Fld(0x0e, MISC_SHU_RDAT_DATLAT_DSEL) | P_Fld(0x0e, MISC_SHU_RDAT_DATLAT_DSEL_PHY));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_PHY_RX_CTRL, P_Fld(0x1, MISC_SHU_PHY_RX_CTRL_RANK_RXDLY_UPDLAT_EN) |
            P_Fld(0x2, MISC_SHU_PHY_RX_CTRL_RANK_RXDLY_UPD_OFFSET) | P_Fld(0x1, MISC_SHU_PHY_RX_CTRL_RX_IN_GATE_EN_PRE_OFFSET) |
            P_Fld(0x0, MISC_SHU_PHY_RX_CTRL_RX_IN_GATE_EN_HEAD) | P_Fld(0x1, MISC_SHU_PHY_RX_CTRL_RX_IN_GATE_EN_TAIL) |
            P_Fld(0x0, MISC_SHU_PHY_RX_CTRL_RX_IN_BUFF_EN_HEAD) | P_Fld(0x0, MISC_SHU_PHY_RX_CTRL_RX_IN_BUFF_EN_TAIL));

    vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RANKCTL, P_Fld(0x3, MISC_SHU_RANKCTL_RANKINCTL_RXDLY) |
            P_Fld(0x1, MISC_SHU_RANKCTL_RANK_RXDLY_OPT) | P_Fld(0x1, MISC_SHU_RANKCTL_RANKSEL_SELPH_FRUN) |
            P_Fld(0x4, MISC_SHU_RANKCTL_RANKINCTL_STB));
    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RANKCTL, P_Fld(0x4, MISC_SHU_RANKCTL_RANKINCTL) |
            P_Fld(0x4, MISC_SHU_RANKCTL_RANKINCTL_ROOT1) | P_Fld(0x6, MISC_SHU_RANKCTL_RANKINCTL_PHY));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RANK_SEL_LAT, P_Fld(0x4, MISC_SHU_RANK_SEL_LAT_RANK_SEL_LAT_B0) |
            P_Fld(0x4, MISC_SHU_RANK_SEL_LAT_RANK_SEL_LAT_B1) | P_Fld(0x4, MISC_SHU_RANK_SEL_LAT_RANK_SEL_LAT_CA));

    #if !CODE_SIZE_REDUCE

    vIO32WriteFldAlign(DDRPHY_REG_MISC_SHU_RK_DQSCTL, 0x6, MISC_SHU_RK_DQSCTL_DQSINCTL);

    vIO32WriteFldAlign(DDRPHY_REG_MISC_SHU_RK_DQSCTL+(1*DDRPHY_AO_RANK_OFFSET), 0x6, MISC_SHU_RK_DQSCTL_DQSINCTL);

    vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B0_DQSIEN_MCK_UI_DLY, P_Fld(0x6, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B0) |
            P_Fld(0x8, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B0) | P_Fld(0x0, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B0) |
            P_Fld(0x0, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B0));

    vIO32WriteFldAlign(DDRPHY_REG_SHU_RK_B0_DQSIEN_PI_DLY, 0x0b, SHU_RK_B0_DQSIEN_PI_DLY_DQSIEN_PI_B0);

    vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B0_DQSIEN_MCK_UI_DLY+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x7, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B0) |
            P_Fld(0x9, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B0) | P_Fld(0x0, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B0) |
            P_Fld(0x0, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B0));

    vIO32WriteFldAlign(DDRPHY_REG_SHU_RK_B0_DQSIEN_PI_DLY+(1*DDRPHY_AO_RANK_OFFSET), 0x1f, SHU_RK_B0_DQSIEN_PI_DLY_DQSIEN_PI_B0);

    vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B1_DQSIEN_MCK_UI_DLY, P_Fld(0x6, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B1) |
            P_Fld(0x8, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B1) | P_Fld(0x0, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B1) |
            P_Fld(0x0, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B1));

    vIO32WriteFldAlign(DDRPHY_REG_SHU_RK_B1_DQSIEN_PI_DLY, 0x0b, SHU_RK_B1_DQSIEN_PI_DLY_DQSIEN_PI_B1);

    vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B1_DQSIEN_MCK_UI_DLY+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x7, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B1) |
            P_Fld(0x9, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B1) | P_Fld(0x0, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B1) |
            P_Fld(0x0, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B1));

    vIO32WriteFldAlign(DDRPHY_REG_SHU_RK_B1_DQSIEN_PI_DLY+(1*DDRPHY_AO_RANK_OFFSET), 0x1f, SHU_RK_B1_DQSIEN_PI_DLY_DQSIEN_PI_B1);
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_ODTCTRL, P_Fld(0x1, MISC_SHU_ODTCTRL_RODTEN) |
            P_Fld(0x0, MISC_SHU_ODTCTRL_RODTENSTB_SELPH_CG_IG) |
            P_Fld(0x0, MISC_SHU_ODTCTRL_RODTEN_SELPH_FRUN) | P_Fld(0x0, MISC_SHU_ODTCTRL_RODTDLY_LAT_OPT) |
            P_Fld(0x0, MISC_SHU_ODTCTRL_FIXRODT) | P_Fld(0x1, MISC_SHU_ODTCTRL_RODTEN_OPT) |
            P_Fld(0x1, MISC_SHU_ODTCTRL_RODTE2) | P_Fld(0x1, MISC_SHU_ODTCTRL_RODTE));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldAlign(DDRPHY_REG_MISC_SHU_ODTCTRL, 0x4, MISC_SHU_ODTCTRL_RODT_LAT);
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ7, P_Fld(0x0, SHU_B0_DQ7_R_DMRANKRXDVS_B0) |
            P_Fld(0x0, SHU_B0_DQ7_R_DMDQMDBI_EYE_SHU_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMDQMDBI_SHU_B0) |
            P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_DQM_FLAGSEL_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B0) |
            P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMRXTRACK_DQM_EN_B0) |
            P_Fld(0x1, SHU_B0_DQ7_R_DMRODTEN_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMARPI_CG_FB2DLL_DCM_EN_B0) |
            P_Fld(0x0, SHU_B0_DQ7_R_DMTX_ARPI_CG_DQ_NEW_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMTX_ARPI_CG_DQS_NEW_B0) |
            P_Fld(0x0, SHU_B0_DQ7_R_DMTX_ARPI_CG_DQM_NEW_B0) | P_Fld(0x0, SHU_B0_DQ7_R_LP4Y_SDN_MODE_DQS0) |
            P_Fld(0x1, SHU_B0_DQ7_R_DMRXRANK_DQ_EN_B0) | P_Fld(0x1, SHU_B0_DQ7_R_DMRXRANK_DQ_LAT_B0) |
            P_Fld(0x1, SHU_B0_DQ7_R_DMRXRANK_DQS_EN_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMRXRANK_DQS_LAT_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ7, P_Fld(0x0, SHU_B1_DQ7_R_DMRANKRXDVS_B1) |
            P_Fld(0x0, SHU_B1_DQ7_R_DMDQMDBI_EYE_SHU_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMDQMDBI_SHU_B1) |
            P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_DQM_FLAGSEL_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B1) |
            P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMRXTRACK_DQM_EN_B1) |
            P_Fld(0x1, SHU_B1_DQ7_R_DMRODTEN_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMARPI_CG_FB2DLL_DCM_EN_B1) |
            P_Fld(0x0, SHU_B1_DQ7_R_DMTX_ARPI_CG_DQ_NEW_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMTX_ARPI_CG_DQS_NEW_B1) |
            P_Fld(0x0, SHU_B1_DQ7_R_DMTX_ARPI_CG_DQM_NEW_B1) | P_Fld(0x0, SHU_B1_DQ7_R_LP4Y_SDN_MODE_DQS1) |
            P_Fld(0x1, SHU_B1_DQ7_R_DMRXRANK_DQ_EN_B1) | P_Fld(0x1, SHU_B1_DQ7_R_DMRXRANK_DQ_LAT_B1) |
            P_Fld(0x1, SHU_B1_DQ7_R_DMRXRANK_DQS_EN_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMRXRANK_DQS_LAT_B1));

    vIO32WriteFldAlign(DDRPHY_REG_SHU_MISC_RX_PIPE_CTRL, 0x1, SHU_MISC_RX_PIPE_CTRL_RX_PIPE_BYPASS_EN);

    vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B0_RODTEN_MCK_UI_DLY, P_Fld(0x1, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B0) |
            P_Fld(0x1, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B0) | P_Fld(0x0, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B0) |
            P_Fld(0x0, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B0_RODTEN_MCK_UI_DLY+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x2, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B0) |
            P_Fld(0x2, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B0) | P_Fld(0x0, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B0) |
            P_Fld(0x0, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B1_RODTEN_MCK_UI_DLY, P_Fld(0x1, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B1) |
            P_Fld(0x1, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B1) | P_Fld(0x0, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B1) |
            P_Fld(0x0, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B1_RODTEN_MCK_UI_DLY+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x2, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B1) |
            P_Fld(0x2, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B1) | P_Fld(0x0, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B1) |
            P_Fld(0x0, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B1));

    #if CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DRAMC_REG_SHU_RX_CG_SET0, P_Fld(0x0, SHU_RX_CG_SET0_DLE_LAST_EXTEND3) |
            P_Fld(0x0, SHU_RX_CG_SET0_READ_START_EXTEND3) | P_Fld(0x1, SHU_RX_CG_SET0_DLE_LAST_EXTEND2) |
            P_Fld(0x1, SHU_RX_CG_SET0_READ_START_EXTEND2) | P_Fld(0x1, SHU_RX_CG_SET0_DLE_LAST_EXTEND1) |
            P_Fld(0x1, SHU_RX_CG_SET0_READ_START_EXTEND1));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_MISC_RANK_SEL_STB, P_Fld(0x1, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_EN) |
            P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_EN_B23) | P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_SERMODE) |
            P_Fld(0x1, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_TRACK) | P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_RXDLY_TRACK) |
            P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_PHASE_EN) | P_Fld(0x5, SHU_MISC_RANK_SEL_STB_RANK_SEL_PHSINCTL) |
            P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_UI_PLUS) | P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_MCK_PLUS) |
            P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_UI_MINUS) | P_Fld(0x1, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_MCK_MINUS));

    vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RK_DQSCAL, P_Fld(0x60, MISC_SHU_RK_DQSCAL_DQSIENLLMT) |
            P_Fld(0x1, MISC_SHU_RK_DQSCAL_DQSIENLLMTEN) | P_Fld(0x3f, MISC_SHU_RK_DQSCAL_DQSIENHLMT) |
            P_Fld(0x1, MISC_SHU_RK_DQSCAL_DQSIENHLMTEN));

    vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RK_DQSCAL+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x60, MISC_SHU_RK_DQSCAL_DQSIENLLMT) |
            P_Fld(0x1, MISC_SHU_RK_DQSCAL_DQSIENLLMTEN) | P_Fld(0x3f, MISC_SHU_RK_DQSCAL_DQSIENHLMT) |
            P_Fld(0x1, MISC_SHU_RK_DQSCAL_DQSIENHLMTEN));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_INI_UIPI, P_Fld(0x0b, SHU_R0_B0_INI_UIPI_CURR_INI_PI_B0) |
            P_Fld(0x06, SHU_R0_B0_INI_UIPI_CURR_INI_UI_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_INI_UIPI, P_Fld(0x0b, SHU_R0_B1_INI_UIPI_CURR_INI_PI_B1) |
            P_Fld(0x06, SHU_R0_B1_INI_UIPI_CURR_INI_UI_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_INI_UIPI+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x1f, SHU_R0_B0_INI_UIPI_CURR_INI_PI_B0) |
            P_Fld(0x07, SHU_R0_B0_INI_UIPI_CURR_INI_UI_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_INI_UIPI+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x1f, SHU_R0_B1_INI_UIPI_CURR_INI_PI_B1) |
            P_Fld(0x07, SHU_R0_B1_INI_UIPI_CURR_INI_UI_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_NEXT_INI_UIPI, P_Fld(0x0b, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_PI_B0) |
            P_Fld(0x06, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_UI_B0) | P_Fld(0x08, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_UI_P1_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_NEXT_INI_UIPI, P_Fld(0x0b, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_PI_B1) |
            P_Fld(0x06, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_UI_B1) | P_Fld(0x08, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_UI_P1_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_NEXT_INI_UIPI+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x1f, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_PI_B0) |
            P_Fld(0x07, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_UI_B0) | P_Fld(0x09, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_UI_P1_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_NEXT_INI_UIPI+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x1f, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_PI_B1) |
            P_Fld(0x07, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_UI_B1) | P_Fld(0x09, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_UI_P1_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_DQ0, P_Fld(0x0, SHU_R0_B0_DQ0_RG_RX_ARDQS0_R_DLY_DUTY) |
            P_Fld(0x0, SHU_R0_B0_DQ0_RG_RX_ARDQS0_F_DLY_DUTY) | P_Fld(0x0, SHU_R0_B0_DQ0_DA_ARPI_DDR400_0D5UI_RK0_B0) |
            P_Fld(0x0, SHU_R0_B0_DQ0_DA_RX_ARDQSIEN_0D5UI_RK0_B0));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_DQ0, P_Fld(0x18, SHU_R0_B0_DQ0_SW_ARPI_DQ_B0) |
            P_Fld(0x18, SHU_R0_B0_DQ0_SW_ARPI_DQM_B0) | P_Fld(0x00, SHU_R0_B0_DQ0_ARPI_PBYTE_B0));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_DQ0, P_Fld(0x0, SHU_R0_B1_DQ0_RG_RX_ARDQS1_R_DLY_DUTY) |
            P_Fld(0x0, SHU_R0_B1_DQ0_RG_RX_ARDQS1_F_DLY_DUTY) | P_Fld(0x0, SHU_R0_B1_DQ0_DA_ARPI_DDR400_0D5UI_RK0_B1) |
            P_Fld(0x0, SHU_R0_B1_DQ0_DA_RX_ARDQSIEN_0D5UI_RK0_B1));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_DQ0, P_Fld(0x18, SHU_R0_B1_DQ0_SW_ARPI_DQ_B1) |
            P_Fld(0x18, SHU_R0_B1_DQ0_SW_ARPI_DQM_B1) | P_Fld(0x00, SHU_R0_B1_DQ0_ARPI_PBYTE_B1));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_DQ0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x0, SHU_R0_B0_DQ0_RG_RX_ARDQS0_R_DLY_DUTY) |
            P_Fld(0x0, SHU_R0_B0_DQ0_RG_RX_ARDQS0_F_DLY_DUTY) | P_Fld(0x0, SHU_R0_B0_DQ0_DA_ARPI_DDR400_0D5UI_RK0_B0) |
            P_Fld(0x0, SHU_R0_B0_DQ0_DA_RX_ARDQSIEN_0D5UI_RK0_B0));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_DQ0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x18, SHU_R0_B0_DQ0_SW_ARPI_DQ_B0) |
            P_Fld(0x18, SHU_R0_B0_DQ0_SW_ARPI_DQM_B0) | P_Fld(0x00, SHU_R0_B0_DQ0_ARPI_PBYTE_B0));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_DQ0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x0, SHU_R0_B1_DQ0_RG_RX_ARDQS1_R_DLY_DUTY) |
            P_Fld(0x0, SHU_R0_B1_DQ0_RG_RX_ARDQS1_F_DLY_DUTY) | P_Fld(0x0, SHU_R0_B1_DQ0_DA_ARPI_DDR400_0D5UI_RK0_B1) |
            P_Fld(0x0, SHU_R0_B1_DQ0_DA_RX_ARDQSIEN_0D5UI_RK0_B1));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_DQ0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x18, SHU_R0_B1_DQ0_SW_ARPI_DQ_B1) |
            P_Fld(0x18, SHU_R0_B1_DQ0_SW_ARPI_DQM_B1) | P_Fld(0x00, SHU_R0_B1_DQ0_ARPI_PBYTE_B1));
    #endif

    vIO32WriteFldMulti(DRAMC_REG_SHU_DCM_CTRL0, P_Fld(0x1, SHU_DCM_CTRL0_DDRPHY_CLK_EN_OPT) |
            P_Fld(0x3, SHU_DCM_CTRL0_DPHY_CMD_CLKEN_EXTCNT) | P_Fld(0x6, SHU_DCM_CTRL0_DDRPHY_CLK_DYN_GATING_SEL) |
            P_Fld(0x2, SHU_DCM_CTRL0_APHYPI_CKCGL_CNT) | P_Fld(0x5, SHU_DCM_CTRL0_APHYPI_CKCGH_CNT) |
            P_Fld(0x0, SHU_DCM_CTRL0_FASTWAKE2) | P_Fld(0x1, SHU_DCM_CTRL0_FASTWAKE));

    vIO32WriteFldMulti(DRAMC_REG_SHU_APHY_TX_PICG_CTRL, P_Fld(0x3, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_PICG_CNT) |
            P_Fld(0x0, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQS_SEL_P1) | P_Fld(0x3, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQS_SEL_P0) |
            P_Fld(0x2, SHU_APHY_TX_PICG_CTRL_DPHY_TX_DCM_EXTCNT) | P_Fld(0x1, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_OPT));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_APHY_TX_PICG_CTRL, P_Fld(0x0, SHURK_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQ_RK_SEL_P1) |
            P_Fld(0x3, SHURK_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQ_RK_SEL_P0));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_APHY_TX_PICG_CTRL+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x0, SHURK_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQ_RK_SEL_P1) |
            P_Fld(0x3, SHURK_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQ_RK_SEL_P0));

    vIO32WriteFldMulti(DRAMC_REG_SHU_NEW_XRW2W_CTRL, P_Fld(0x2, SHU_NEW_XRW2W_CTRL_TX_PI_UPDCTL_B0) |
            P_Fld(0x2, SHU_NEW_XRW2W_CTRL_TX_PI_UPDCTL_B1) | P_Fld(0x0, SHU_NEW_XRW2W_CTRL_TXPI_UPD_MODE));

    vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_DQS0, P_Fld(0x1, SHU_SELPH_DQS0_TXDLY_DQS2) |
            P_Fld(0x1, SHU_SELPH_DQS0_TXDLY_DQS3) | P_Fld(0x1, SHU_SELPH_DQS0_TXDLY_OEN_DQS2) |
            P_Fld(0x1, SHU_SELPH_DQS0_TXDLY_OEN_DQS3));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_DQS0, P_Fld(0x4, SHU_SELPH_DQS0_TXDLY_DQS0) |
            P_Fld(0x4, SHU_SELPH_DQS0_TXDLY_DQS1) | P_Fld(0x3, SHU_SELPH_DQS0_TXDLY_OEN_DQS0) |
            P_Fld(0x3, SHU_SELPH_DQS0_TXDLY_OEN_DQS1));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ0, P_Fld(0x4, SHURK_SELPH_DQ0_TXDLY_DQ0) |
            P_Fld(0x4, SHURK_SELPH_DQ0_TXDLY_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_DQ2) |
            P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_DQ3) | P_Fld(0x3, SHURK_SELPH_DQ0_TXDLY_OEN_DQ0) |
            P_Fld(0x3, SHURK_SELPH_DQ0_TXDLY_OEN_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_OEN_DQ2) |
            P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_OEN_DQ3));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ1, P_Fld(0x4, SHURK_SELPH_DQ1_TXDLY_DQM0) |
            P_Fld(0x4, SHURK_SELPH_DQ1_TXDLY_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_DQM2) |
            P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_DQM3) | P_Fld(0x3, SHURK_SELPH_DQ1_TXDLY_OEN_DQM0) |
            P_Fld(0x3, SHURK_SELPH_DQ1_TXDLY_OEN_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_OEN_DQM2) |
            P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_OEN_DQM3));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ2, P_Fld(0x1, SHURK_SELPH_DQ2_DLY_DQ0) |
            P_Fld(0x1, SHURK_SELPH_DQ2_DLY_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ2_DLY_DQ2) |
            P_Fld(0x1, SHURK_SELPH_DQ2_DLY_DQ3) | P_Fld(0x2, SHURK_SELPH_DQ2_DLY_OEN_DQ0) |
            P_Fld(0x2, SHURK_SELPH_DQ2_DLY_OEN_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ2_DLY_OEN_DQ2) |
            P_Fld(0x1, SHURK_SELPH_DQ2_DLY_OEN_DQ3));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ3, P_Fld(0x1, SHURK_SELPH_DQ3_DLY_DQM0) |
            P_Fld(0x1, SHURK_SELPH_DQ3_DLY_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ3_DLY_DQM2) |
            P_Fld(0x1, SHURK_SELPH_DQ3_DLY_DQM3) | P_Fld(0x2, SHURK_SELPH_DQ3_DLY_OEN_DQM0) |
            P_Fld(0x2, SHURK_SELPH_DQ3_DLY_OEN_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ3_DLY_OEN_DQM2) |
            P_Fld(0x1, SHURK_SELPH_DQ3_DLY_OEN_DQM3));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ0+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x4, SHURK_SELPH_DQ0_TXDLY_DQ0) |
            P_Fld(0x4, SHURK_SELPH_DQ0_TXDLY_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_DQ2) |
            P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_DQ3) | P_Fld(0x3, SHURK_SELPH_DQ0_TXDLY_OEN_DQ0) |
            P_Fld(0x3, SHURK_SELPH_DQ0_TXDLY_OEN_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_OEN_DQ2) |
            P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_OEN_DQ3));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ1+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x4, SHURK_SELPH_DQ1_TXDLY_DQM0) |
            P_Fld(0x4, SHURK_SELPH_DQ1_TXDLY_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_DQM2) |
            P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_DQM3) | P_Fld(0x3, SHURK_SELPH_DQ1_TXDLY_OEN_DQM0) |
            P_Fld(0x3, SHURK_SELPH_DQ1_TXDLY_OEN_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_OEN_DQM2) |
            P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_OEN_DQM3));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ2+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x1, SHURK_SELPH_DQ2_DLY_DQ0) |
            P_Fld(0x1, SHURK_SELPH_DQ2_DLY_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ2_DLY_DQ2) |
            P_Fld(0x1, SHURK_SELPH_DQ2_DLY_DQ3) | P_Fld(0x2, SHURK_SELPH_DQ2_DLY_OEN_DQ0) |
            P_Fld(0x2, SHURK_SELPH_DQ2_DLY_OEN_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ2_DLY_OEN_DQ2) |
            P_Fld(0x1, SHURK_SELPH_DQ2_DLY_OEN_DQ3));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ3+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x1, SHURK_SELPH_DQ3_DLY_DQM0) |
            P_Fld(0x1, SHURK_SELPH_DQ3_DLY_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ3_DLY_DQM2) |
            P_Fld(0x1, SHURK_SELPH_DQ3_DLY_DQM3) | P_Fld(0x2, SHURK_SELPH_DQ3_DLY_OEN_DQM0) |
            P_Fld(0x2, SHURK_SELPH_DQ3_DLY_OEN_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ3_DLY_OEN_DQM2) |
            P_Fld(0x1, SHURK_SELPH_DQ3_DLY_OEN_DQM3));
    #endif

    vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL1, P_Fld(0x018, SHURK_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ0) |
            P_Fld(0x018, SHURK_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ1));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL2, P_Fld(0x018, SHURK_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ0) |
            P_Fld(0x018, SHURK_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ1));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL5, P_Fld(0x018, SHURK_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM0) |
            P_Fld(0x018, SHURK_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM1));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL1+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x018, SHURK_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ0) |
            P_Fld(0x018, SHURK_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ1));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL2+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x018, SHURK_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ0) |
            P_Fld(0x018, SHURK_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ1));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL5+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x018, SHURK_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM0) |
            P_Fld(0x018, SHURK_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM1));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_PI, P_Fld(0x18, SHURK_PI_RK0_ARPI_DQ_B1) |
            P_Fld(0x18, SHURK_PI_RK0_ARPI_DQ_B0) | P_Fld(0x18, SHURK_PI_RK0_ARPI_DQM_B1) |
            P_Fld(0x18, SHURK_PI_RK0_ARPI_DQM_B0));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_PI+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x18, SHURK_PI_RK0_ARPI_DQ_B1) |
            P_Fld(0x18, SHURK_PI_RK0_ARPI_DQ_B0) | P_Fld(0x18, SHURK_PI_RK0_ARPI_DQM_B1) |
            P_Fld(0x18, SHURK_PI_RK0_ARPI_DQM_B0));

    #if !CODE_SIZE_REDUCE

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_TXDLY0, P_Fld(0x30, SHU_R0_B0_TXDLY0_TX_ARDQ0_DLY_B0) |
            P_Fld(0x30, SHU_R0_B0_TXDLY0_TX_ARDQ1_DLY_B0) | P_Fld(0x30, SHU_R0_B0_TXDLY0_TX_ARDQ2_DLY_B0) |
            P_Fld(0x30, SHU_R0_B0_TXDLY0_TX_ARDQ3_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_TXDLY1, P_Fld(0x30, SHU_R0_B0_TXDLY1_TX_ARDQ4_DLY_B0) |
            P_Fld(0x30, SHU_R0_B0_TXDLY1_TX_ARDQ5_DLY_B0) | P_Fld(0x30, SHU_R0_B0_TXDLY1_TX_ARDQ6_DLY_B0) |
            P_Fld(0x30, SHU_R0_B0_TXDLY1_TX_ARDQ7_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_TXDLY3, P_Fld(0x30, SHU_R0_B0_TXDLY3_TX_ARDQM0_DLY_B0) |
            P_Fld(0x00, SHU_R0_B0_TXDLY3_TX_ARWCK_DLY_B0) | P_Fld(0x00, SHU_R0_B0_TXDLY3_TX_ARWCKB_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_TXDLY0, P_Fld(0x0c, SHU_R0_B1_TXDLY0_TX_ARDQ0_DLY_B1) |
            P_Fld(0x0c, SHU_R0_B1_TXDLY0_TX_ARDQ1_DLY_B1) | P_Fld(0x0c, SHU_R0_B1_TXDLY0_TX_ARDQ2_DLY_B1) |
            P_Fld(0x0c, SHU_R0_B1_TXDLY0_TX_ARDQ3_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_TXDLY1, P_Fld(0x0c, SHU_R0_B1_TXDLY1_TX_ARDQ4_DLY_B1) |
            P_Fld(0x0c, SHU_R0_B1_TXDLY1_TX_ARDQ5_DLY_B1) | P_Fld(0x0c, SHU_R0_B1_TXDLY1_TX_ARDQ6_DLY_B1) |
            P_Fld(0x0c, SHU_R0_B1_TXDLY1_TX_ARDQ7_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_TXDLY3, P_Fld(0x0c, SHU_R0_B1_TXDLY3_TX_ARDQM0_DLY_B1) |
            P_Fld(0x00, SHU_R0_B1_TXDLY3_TX_ARWCK_DLY_B1) | P_Fld(0x00, SHU_R0_B1_TXDLY3_TX_ARWCKB_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_TXDLY0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x10, SHU_R0_B0_TXDLY0_TX_ARDQ0_DLY_B0) |
            P_Fld(0x10, SHU_R0_B0_TXDLY0_TX_ARDQ1_DLY_B0) | P_Fld(0x10, SHU_R0_B0_TXDLY0_TX_ARDQ2_DLY_B0) |
            P_Fld(0x10, SHU_R0_B0_TXDLY0_TX_ARDQ3_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_TXDLY1+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x10, SHU_R0_B0_TXDLY1_TX_ARDQ4_DLY_B0) |
            P_Fld(0x10, SHU_R0_B0_TXDLY1_TX_ARDQ5_DLY_B0) | P_Fld(0x10, SHU_R0_B0_TXDLY1_TX_ARDQ6_DLY_B0) |
            P_Fld(0x10, SHU_R0_B0_TXDLY1_TX_ARDQ7_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_TXDLY3+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x10, SHU_R0_B0_TXDLY3_TX_ARDQM0_DLY_B0) |
            P_Fld(0x00, SHU_R0_B0_TXDLY3_TX_ARWCK_DLY_B0) | P_Fld(0x00, SHU_R0_B0_TXDLY3_TX_ARWCKB_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_TXDLY0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x20, SHU_R0_B1_TXDLY0_TX_ARDQ0_DLY_B1) |
            P_Fld(0x20, SHU_R0_B1_TXDLY0_TX_ARDQ1_DLY_B1) | P_Fld(0x20, SHU_R0_B1_TXDLY0_TX_ARDQ2_DLY_B1) |
            P_Fld(0x20, SHU_R0_B1_TXDLY0_TX_ARDQ3_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_TXDLY1+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x20, SHU_R0_B1_TXDLY1_TX_ARDQ4_DLY_B1) |
            P_Fld(0x20, SHU_R0_B1_TXDLY1_TX_ARDQ5_DLY_B1) | P_Fld(0x20, SHU_R0_B1_TXDLY1_TX_ARDQ6_DLY_B1) |
            P_Fld(0x20, SHU_R0_B1_TXDLY1_TX_ARDQ7_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_TXDLY3+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x20, SHU_R0_B1_TXDLY3_TX_ARDQM0_DLY_B1) |
            P_Fld(0x00, SHU_R0_B1_TXDLY3_TX_ARWCK_DLY_B1) | P_Fld(0x00, SHU_R0_B1_TXDLY3_TX_ARWCKB_DLY_B1));

    vIO32WriteFldMulti(DRAMC_REG_SHU_TX_RANKCTL, P_Fld(0x1, SHU_TX_RANKCTL_TXRANKINCTL_TXDLY) |
            P_Fld(0x1, SHU_TX_RANKCTL_TXRANKINCTL) | P_Fld(0x0, SHU_TX_RANKCTL_TXRANKINCTL_ROOT));
    #endif

    #if !(CODE_SIZE_REDUCE && AC_TIMING_DERATE_ENABLE)
    vIO32WriteFldMulti(DRAMC_REG_SHU_AC_DERATING0, P_Fld(0x1, SHU_AC_DERATING0_ACDERATEEN) |
            P_Fld(0x2, SHU_AC_DERATING0_TRRD_DERATE) | P_Fld(0x4, SHU_AC_DERATING0_TRCD_DERATE));

    vIO32WriteFldMulti(DRAMC_REG_SHU_AC_DERATING1, P_Fld(0x3, SHU_AC_DERATING1_TRPAB_DERATE) |
            P_Fld(0x2, SHU_AC_DERATING1_TRP_DERATE) | P_Fld(0x01, SHU_AC_DERATING1_TRAS_DERATE) |
            P_Fld(0x00, SHU_AC_DERATING1_TRC_DERATE));
    #endif

    vIO32WriteFldMulti(DRAMC_REG_SHU_SREF_CTRL, P_Fld(0x3, SHU_SREF_CTRL_CKEHCMD) |
            P_Fld(0x3, SHU_SREF_CTRL_SREF_CK_DLY));

    vIO32WriteFldMulti(DRAMC_REG_SHU_HMR4_DVFS_CTRL0, P_Fld(0x32, SHU_HMR4_DVFS_CTRL0_FSPCHG_PRDCNT) |
            P_Fld(0x000, SHU_HMR4_DVFS_CTRL0_REFRCNT));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM_XRT, P_Fld(0x05, SHU_ACTIM_XRT_XRTR2R) |
            P_Fld(0x0a, SHU_ACTIM_XRT_XRTR2W) | P_Fld(0x6, SHU_ACTIM_XRT_XRTW2R) |
            P_Fld(0x09, SHU_ACTIM_XRT_XRTW2W));

    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM0, P_Fld(0x0a, SHU_ACTIM0_TWTR) |
            P_Fld(0x0c, SHU_ACTIM0_TWR) | P_Fld(0x1, SHU_ACTIM0_TRRD) |
            P_Fld(0x4, SHU_ACTIM0_TRCD) | P_Fld(0x3, SHU_ACTIM0_CKELCKCNT));

    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM1, P_Fld(0x3, SHU_ACTIM1_TRPAB) |
            P_Fld(0x7, SHU_ACTIM1_TMRWCKEL) | P_Fld(0x2, SHU_ACTIM1_TRP) |
            P_Fld(0x01, SHU_ACTIM1_TRAS) | P_Fld(0x00, SHU_ACTIM1_TRC));

    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM2, P_Fld(0x0, SHU_ACTIM2_TXP) |
            P_Fld(0x07, SHU_ACTIM2_TMRRI) | P_Fld(0x3, SHU_ACTIM2_TRTP) |
            P_Fld(0x0a, SHU_ACTIM2_TR2W) | P_Fld(0x00, SHU_ACTIM2_TFAW));

    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM3, P_Fld(0x1a, SHU_ACTIM3_TRFCPB) |
            P_Fld(0x8, SHU_ACTIM3_TR2MRR) | P_Fld(0x40, SHU_ACTIM3_TRFC));
    #endif
    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM3, P_Fld(0x8, SHU_ACTIM3_MANTMRR) |
            P_Fld(0x25, SHU_ACTIM3_TWTR_L));

    #if !CODE_SIZE_REDUCE

    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM4, P_Fld(0x04e, SHU_ACTIM4_TXREFCNT) |
            P_Fld(0x0f, SHU_ACTIM4_TMRR2MRW) | P_Fld(0x0c, SHU_ACTIM4_TMRR2W) |
            P_Fld(0x10, SHU_ACTIM4_TZQCS));

    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM5, P_Fld(0x10, SHU_ACTIM5_TR2PD) |
            P_Fld(0x0f, SHU_ACTIM5_TWTPD) | P_Fld(0x15, SHU_ACTIM5_TPBR2PBR) |
            P_Fld(0x0, SHU_ACTIM5_TPBR2ACT));

    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM6, P_Fld(0x06, SHU_ACTIM6_TZQLAT2) |
            P_Fld(0x6, SHU_ACTIM6_TMRD) | P_Fld(0x5, SHU_ACTIM6_TMRW) |
            P_Fld(0x0d, SHU_ACTIM6_TW2MRW) | P_Fld(0x11, SHU_ACTIM6_TR2MRW));

    vIO32WriteFldMulti(DRAMC_REG_SHU_CKECTRL, P_Fld(0x0, SHU_CKECTRL_TPDE_05T) |
            P_Fld(0x0, SHU_CKECTRL_TPDX_05T) | P_Fld(0x3, SHU_CKECTRL_TPDE) |
            P_Fld(0x3, SHU_CKECTRL_TPDX) | P_Fld(0x2, SHU_CKECTRL_TCKEPRD));
    #endif

    vIO32WriteFldAlign(DRAMC_REG_SHU_CKECTRL, 0x3, SHU_CKECTRL_TCKESRX);

    vIO32WriteFldMulti(DRAMC_REG_SHU_MISC, P_Fld(0x7, SHU_MISC_DCMDLYREF) |
    P_Fld(0x0, SHU_MISC_DAREFEN));

	#if !CODE_SIZE_REDUCE
    vIO32WriteFldAlign(DRAMC_REG_SHU_MISC, 0x2, SHU_MISC_REQQUE_MAXCNT);

    vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ8, P_Fld(0x0, SHU_B0_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B0) |
            P_Fld(0x0, SHU_B0_DQ8_R_RMRODTEN_CG_IG_B0) | P_Fld(0x1, SHU_B0_DQ8_R_RMRX_TOPHY_CG_IG_B0) |
            P_Fld(0x0, SHU_B0_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B0) |
            P_Fld(0x1, SHU_B0_DQ8_R_DMRXDLY_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B0) |
            P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B0) |
            P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMRANK_PIPE_CG_IG_B0) |
            P_Fld(0x0, SHU_B0_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B0));
    #endif
    vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ8, P_Fld(0x0031, SHU_B0_DQ8_R_DMRXDVS_UPD_FORCE_CYC_B0) |
            P_Fld(0x0, SHU_B0_DQ8_R_DMRXDVS_UPD_FORCE_EN_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ8, P_Fld(0x0031, SHU_B1_DQ8_R_DMRXDVS_UPD_FORCE_CYC_B1) |
            P_Fld(0x0, SHU_B1_DQ8_R_DMRXDVS_UPD_FORCE_EN_B1));

	#if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ8, P_Fld(0x0, SHU_B1_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B1) |
            P_Fld(0x0, SHU_B1_DQ8_R_RMRODTEN_CG_IG_B1) | P_Fld(0x1, SHU_B1_DQ8_R_RMRX_TOPHY_CG_IG_B1) |
            P_Fld(0x0, SHU_B1_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B1) |
            P_Fld(0x1, SHU_B1_DQ8_R_DMRXDLY_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B1) |
            P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B1) |
            P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMRANK_PIPE_CG_IG_B1) |
            P_Fld(0x0, SHU_B1_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B1));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ5, P_Fld(0x0e, SHU_B0_DQ5_RG_RX_ARDQ_VREF_SEL_B0) |
            P_Fld(0x0, SHU_B0_DQ5_RG_RX_ARDQ_VREF_BYPASS_B0) | P_Fld(0x00, SHU_B0_DQ5_RG_ARPI_FB_B0) |
            P_Fld(0x0, SHU_B0_DQ5_RG_RX_ARDQS0_DQSIEN_DLY_B0) | P_Fld(0x0, SHU_B0_DQ5_RG_RX_ARDQS_DQSIEN_RB_DLY_B0) |
            P_Fld(0x7, SHU_B0_DQ5_RG_RX_ARDQS0_DVS_DLY_B0) | P_Fld(0x0, SHU_B0_DQ5_RG_RX_ARDQ_FIFO_DQSI_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ5, P_Fld(0x0e, SHU_B1_DQ5_RG_RX_ARDQ_VREF_SEL_B1) |
            P_Fld(0x0, SHU_B1_DQ5_RG_RX_ARDQ_VREF_BYPASS_B1) | P_Fld(0x00, SHU_B1_DQ5_RG_ARPI_FB_B1) |
            P_Fld(0x0, SHU_B1_DQ5_RG_RX_ARDQS0_DQSIEN_DLY_B1) | P_Fld(0x0, SHU_B1_DQ5_RG_RX_ARDQS_DQSIEN_RB_DLY_B1) |
            P_Fld(0x7, SHU_B1_DQ5_RG_RX_ARDQS0_DVS_DLY_B1) | P_Fld(0x0, SHU_B1_DQ5_RG_RX_ARDQ_FIFO_DQSI_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY0, P_Fld(0x75, SHU_R0_B0_RXDLY0_RX_ARDQ0_F_DLY_B0) |
            P_Fld(0x75, SHU_R0_B0_RXDLY0_RX_ARDQ1_F_DLY_B0));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY0, P_Fld(0x75, SHU_R0_B0_RXDLY0_RX_ARDQ0_R_DLY_B0) |
            P_Fld(0x75, SHU_R0_B0_RXDLY0_RX_ARDQ1_R_DLY_B0));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY1, P_Fld(0x75, SHU_R0_B0_RXDLY1_RX_ARDQ2_R_DLY_B0) |
            P_Fld(0x75, SHU_R0_B0_RXDLY1_RX_ARDQ2_F_DLY_B0) | P_Fld(0x75, SHU_R0_B0_RXDLY1_RX_ARDQ3_R_DLY_B0) |
            P_Fld(0x75, SHU_R0_B0_RXDLY1_RX_ARDQ3_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY2, P_Fld(0x75, SHU_R0_B0_RXDLY2_RX_ARDQ4_R_DLY_B0) |
            P_Fld(0x75, SHU_R0_B0_RXDLY2_RX_ARDQ4_F_DLY_B0) | P_Fld(0x75, SHU_R0_B0_RXDLY2_RX_ARDQ5_R_DLY_B0) |
            P_Fld(0x75, SHU_R0_B0_RXDLY2_RX_ARDQ5_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY3, P_Fld(0x75, SHU_R0_B0_RXDLY3_RX_ARDQ6_R_DLY_B0) |
            P_Fld(0x75, SHU_R0_B0_RXDLY3_RX_ARDQ6_F_DLY_B0) | P_Fld(0x75, SHU_R0_B0_RXDLY3_RX_ARDQ7_R_DLY_B0) |
            P_Fld(0x75, SHU_R0_B0_RXDLY3_RX_ARDQ7_F_DLY_B0));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY4, P_Fld(0x75, SHU_R0_B0_RXDLY4_RX_ARDQM0_R_DLY_B0) |
            P_Fld(0x75, SHU_R0_B0_RXDLY4_RX_ARDQM0_F_DLY_B0));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY5, P_Fld(0x17e, SHU_R0_B0_RXDLY5_RX_ARDQS0_R_DLY_B0) |
            P_Fld(0x17e, SHU_R0_B0_RXDLY5_RX_ARDQS0_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x74, SHU_R0_B0_RXDLY0_RX_ARDQ0_F_DLY_B0) |
            P_Fld(0x74, SHU_R0_B0_RXDLY0_RX_ARDQ1_F_DLY_B0));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x74, SHU_R0_B0_RXDLY0_RX_ARDQ0_R_DLY_B0) |
            P_Fld(0x74, SHU_R0_B0_RXDLY0_RX_ARDQ1_R_DLY_B0));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY1+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x74, SHU_R0_B0_RXDLY1_RX_ARDQ2_R_DLY_B0) |
            P_Fld(0x74, SHU_R0_B0_RXDLY1_RX_ARDQ2_F_DLY_B0) | P_Fld(0x74, SHU_R0_B0_RXDLY1_RX_ARDQ3_R_DLY_B0) |
            P_Fld(0x74, SHU_R0_B0_RXDLY1_RX_ARDQ3_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY2+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x74, SHU_R0_B0_RXDLY2_RX_ARDQ4_R_DLY_B0) |
            P_Fld(0x74, SHU_R0_B0_RXDLY2_RX_ARDQ4_F_DLY_B0) | P_Fld(0x74, SHU_R0_B0_RXDLY2_RX_ARDQ5_R_DLY_B0) |
            P_Fld(0x74, SHU_R0_B0_RXDLY2_RX_ARDQ5_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY3+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x74, SHU_R0_B0_RXDLY3_RX_ARDQ6_R_DLY_B0) |
            P_Fld(0x74, SHU_R0_B0_RXDLY3_RX_ARDQ6_F_DLY_B0) | P_Fld(0x74, SHU_R0_B0_RXDLY3_RX_ARDQ7_R_DLY_B0) |
            P_Fld(0x74, SHU_R0_B0_RXDLY3_RX_ARDQ7_F_DLY_B0));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY4+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x74, SHU_R0_B0_RXDLY4_RX_ARDQM0_R_DLY_B0) |
            P_Fld(0x74, SHU_R0_B0_RXDLY4_RX_ARDQM0_F_DLY_B0));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY5+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x17d, SHU_R0_B0_RXDLY5_RX_ARDQS0_R_DLY_B0) |
            P_Fld(0x17d, SHU_R0_B0_RXDLY5_RX_ARDQS0_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY0, P_Fld(0x75, SHU_R0_B1_RXDLY0_RX_ARDQ0_F_DLY_B1) |
            P_Fld(0x75, SHU_R0_B1_RXDLY0_RX_ARDQ1_F_DLY_B1));
    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY0, P_Fld(0x75, SHU_R0_B1_RXDLY0_RX_ARDQ0_R_DLY_B1) |
            P_Fld(0x75, SHU_R0_B1_RXDLY0_RX_ARDQ1_R_DLY_B1));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY1, P_Fld(0x75, SHU_R0_B1_RXDLY1_RX_ARDQ2_R_DLY_B1) |
            P_Fld(0x75, SHU_R0_B1_RXDLY1_RX_ARDQ2_F_DLY_B1) | P_Fld(0x75, SHU_R0_B1_RXDLY1_RX_ARDQ3_R_DLY_B1) |
            P_Fld(0x75, SHU_R0_B1_RXDLY1_RX_ARDQ3_F_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY2, P_Fld(0x75, SHU_R0_B1_RXDLY2_RX_ARDQ4_R_DLY_B1) |
            P_Fld(0x75, SHU_R0_B1_RXDLY2_RX_ARDQ4_F_DLY_B1) | P_Fld(0x75, SHU_R0_B1_RXDLY2_RX_ARDQ5_R_DLY_B1) |
            P_Fld(0x75, SHU_R0_B1_RXDLY2_RX_ARDQ5_F_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY3, P_Fld(0x75, SHU_R0_B1_RXDLY3_RX_ARDQ6_R_DLY_B1) |
            P_Fld(0x75, SHU_R0_B1_RXDLY3_RX_ARDQ6_F_DLY_B1) | P_Fld(0x75, SHU_R0_B1_RXDLY3_RX_ARDQ7_R_DLY_B1) |
            P_Fld(0x75, SHU_R0_B1_RXDLY3_RX_ARDQ7_F_DLY_B1));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY4, P_Fld(0x75, SHU_R0_B1_RXDLY4_RX_ARDQM0_R_DLY_B1) |
            P_Fld(0x75, SHU_R0_B1_RXDLY4_RX_ARDQM0_F_DLY_B1));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY5, P_Fld(0x17e, SHU_R0_B1_RXDLY5_RX_ARDQS0_R_DLY_B1) |
            P_Fld(0x17e, SHU_R0_B1_RXDLY5_RX_ARDQS0_F_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x74, SHU_R0_B1_RXDLY0_RX_ARDQ0_F_DLY_B1) |
            P_Fld(0x74, SHU_R0_B1_RXDLY0_RX_ARDQ1_F_DLY_B1));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x74, SHU_R0_B1_RXDLY0_RX_ARDQ0_R_DLY_B1) |
            P_Fld(0x74, SHU_R0_B1_RXDLY0_RX_ARDQ1_R_DLY_B1));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY1+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x74, SHU_R0_B1_RXDLY1_RX_ARDQ2_R_DLY_B1) |
            P_Fld(0x74, SHU_R0_B1_RXDLY1_RX_ARDQ2_F_DLY_B1) | P_Fld(0x74, SHU_R0_B1_RXDLY1_RX_ARDQ3_R_DLY_B1) |
            P_Fld(0x74, SHU_R0_B1_RXDLY1_RX_ARDQ3_F_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY2+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x74, SHU_R0_B1_RXDLY2_RX_ARDQ4_R_DLY_B1) |
            P_Fld(0x74, SHU_R0_B1_RXDLY2_RX_ARDQ4_F_DLY_B1) | P_Fld(0x74, SHU_R0_B1_RXDLY2_RX_ARDQ5_R_DLY_B1) |
            P_Fld(0x74, SHU_R0_B1_RXDLY2_RX_ARDQ5_F_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY3+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x74, SHU_R0_B1_RXDLY3_RX_ARDQ6_R_DLY_B1) |
            P_Fld(0x74, SHU_R0_B1_RXDLY3_RX_ARDQ6_F_DLY_B1) | P_Fld(0x74, SHU_R0_B1_RXDLY3_RX_ARDQ7_R_DLY_B1) |
            P_Fld(0x74, SHU_R0_B1_RXDLY3_RX_ARDQ7_F_DLY_B1));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY4+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x74, SHU_R0_B1_RXDLY4_RX_ARDQM0_R_DLY_B1) |
            P_Fld(0x74, SHU_R0_B1_RXDLY4_RX_ARDQM0_F_DLY_B1));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY5+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x17d, SHU_R0_B1_RXDLY5_RX_ARDQS0_R_DLY_B1) |
            P_Fld(0x17d, SHU_R0_B1_RXDLY5_RX_ARDQS0_F_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_B0_DQ9, P_Fld(0x1, B0_DQ9_RG_RX_ARDQ_STBEN_RESETB_B0) |
            P_Fld(0x0, B0_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B0) | P_Fld(0x0, B0_DQ9_RG_RX_ARDQS0_DQSIENMODE_B0) |
            P_Fld(0x1, B0_DQ9_R_DMRXDVS_R_F_DLY_RK_OPT_B0) | P_Fld(0x0, B0_DQ9_R_DMRXFIFO_STBENCMP_EN_B0) |
            P_Fld(0x00, B0_DQ9_R_IN_GATE_EN_LOW_OPT_B0) | P_Fld(0x0, B0_DQ9_R_DMDQSIEN_VALID_LAT_B0) |
            P_Fld(0x0, B0_DQ9_R_DMDQSIEN_RDSEL_LAT_B0) | P_Fld(0x0, B0_DQ9_R_DMRXDVS_VALID_LAT_B0) |
            P_Fld(0x0, B0_DQ9_R_DMRXDVS_RDSEL_LAT_B0));

    vIO32WriteFldMulti(DDRPHY_REG_B1_DQ9, P_Fld(0x1, B1_DQ9_RG_RX_ARDQ_STBEN_RESETB_B1) |
            P_Fld(0x0, B1_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B1) | P_Fld(0x0, B1_DQ9_RG_RX_ARDQS0_DQSIENMODE_B1) |
            P_Fld(0x1, B1_DQ9_R_DMRXDVS_R_F_DLY_RK_OPT_B1) | P_Fld(0x0, B1_DQ9_R_DMRXFIFO_STBENCMP_EN_B1) |
            P_Fld(0x00, B1_DQ9_R_IN_GATE_EN_LOW_OPT_B1) | P_Fld(0x0, B1_DQ9_R_DMDQSIEN_VALID_LAT_B1) |
            P_Fld(0x0, B1_DQ9_R_DMDQSIEN_RDSEL_LAT_B1) | P_Fld(0x0, B1_DQ9_R_DMRXDVS_VALID_LAT_B1) |
            P_Fld(0x0, B1_DQ9_R_DMRXDVS_RDSEL_LAT_B1));

    vIO32WriteFldMulti(DDRPHY_REG_B0_DQ4, P_Fld(0x03, B0_DQ4_RG_RX_ARDQS_EYE_R_DLY_B0) |
            P_Fld(0x03, B0_DQ4_RG_RX_ARDQS_EYE_F_DLY_B0) | P_Fld(0x35, B0_DQ4_RG_RX_ARDQ_EYE_R_DLY_B0) |
            P_Fld(0x35, B0_DQ4_RG_RX_ARDQ_EYE_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_B1_DQ4, P_Fld(0x03, B1_DQ4_RG_RX_ARDQS_EYE_R_DLY_B1) |
            P_Fld(0x03, B1_DQ4_RG_RX_ARDQS_EYE_F_DLY_B1) | P_Fld(0x35, B1_DQ4_RG_RX_ARDQ_EYE_R_DLY_B1) |
            P_Fld(0x35, B1_DQ4_RG_RX_ARDQ_EYE_F_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_B0_DQ5, P_Fld(0x0e, B0_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B0) |
            P_Fld(0x0, B0_DQ5_RG_RX_ARDQ_VREF_EN_B0) | P_Fld(0x0, B0_DQ5_RG_RX_ARDQ_EYE_VREF_EN_B0) |
            P_Fld(0x0, B0_DQ5_RG_RX_ARDQ_EYE_SEL_B0) | P_Fld(0x0, B0_DQ5_RG_RX_ARDQ_EYE_EN_B0) |
            P_Fld(0x1, B0_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B0) | P_Fld(0x0, B0_DQ5_RG_RX_ARDQS0_DVS_EN_B0));

    vIO32WriteFldMulti(DDRPHY_REG_B1_DQ5, P_Fld(0x0e, B1_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B1) |
            P_Fld(0x0, B1_DQ5_RG_RX_ARDQ_VREF_EN_B1) | P_Fld(0x0, B1_DQ5_RG_RX_ARDQ_EYE_VREF_EN_B1) |
            P_Fld(0x0, B1_DQ5_RG_RX_ARDQ_EYE_SEL_B1) | P_Fld(0x0, B1_DQ5_RG_RX_ARDQ_EYE_EN_B1) |
            P_Fld(0x1, B1_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B1) | P_Fld(0x0, B1_DQ5_RG_RX_ARDQS0_DVS_EN_B1));

}


void sv_algorithm_assistance_LP4_3733(DRAMC_CTX_T *p)
{

    vIO32WriteFldMulti(DDRPHY_REG_SHU_MISC_RDSEL_TRACK, P_Fld(0x0f, SHU_MISC_RDSEL_TRACK_DMDATLAT_I) |
            P_Fld(0x1, SHU_MISC_RDSEL_TRACK_RDSEL_HWSAVE_MSK) | P_Fld(0x0, SHU_MISC_RDSEL_TRACK_RDSEL_TRACK_EN) |
            P_Fld(0xfd0, SHU_MISC_RDSEL_TRACK_SHU_GW_THRD_NEG) | P_Fld(0x030, SHU_MISC_RDSEL_TRACK_SHU_GW_THRD_POS));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RDAT, P_Fld(0x0f, MISC_SHU_RDAT_DATLAT) |
            P_Fld(0x0f, MISC_SHU_RDAT_DATLAT_DSEL) | P_Fld(0x0f, MISC_SHU_RDAT_DATLAT_DSEL_PHY));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_PHY_RX_CTRL, P_Fld(0x1, MISC_SHU_PHY_RX_CTRL_RANK_RXDLY_UPDLAT_EN) |
            P_Fld(0x2, MISC_SHU_PHY_RX_CTRL_RANK_RXDLY_UPD_OFFSET) | P_Fld(0x2, MISC_SHU_PHY_RX_CTRL_RX_IN_GATE_EN_PRE_OFFSET) |
            P_Fld(0x1, MISC_SHU_PHY_RX_CTRL_RX_IN_GATE_EN_HEAD) | P_Fld(0x1, MISC_SHU_PHY_RX_CTRL_RX_IN_GATE_EN_TAIL) |
            P_Fld(0x2, MISC_SHU_PHY_RX_CTRL_RX_IN_BUFF_EN_HEAD) | P_Fld(0x0, MISC_SHU_PHY_RX_CTRL_RX_IN_BUFF_EN_TAIL));

    vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RANKCTL, P_Fld(0x4, MISC_SHU_RANKCTL_RANKINCTL_RXDLY) |
            P_Fld(0x1, MISC_SHU_RANKCTL_RANK_RXDLY_OPT) | P_Fld(0x1, MISC_SHU_RANKCTL_RANKSEL_SELPH_FRUN) |
            P_Fld(0x6, MISC_SHU_RANKCTL_RANKINCTL_STB));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RANKCTL, P_Fld(0x5, MISC_SHU_RANKCTL_RANKINCTL) |
            P_Fld(0x5, MISC_SHU_RANKCTL_RANKINCTL_ROOT1) | P_Fld(0x8, MISC_SHU_RANKCTL_RANKINCTL_PHY));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RANK_SEL_LAT, P_Fld(0x2, MISC_SHU_RANK_SEL_LAT_RANK_SEL_LAT_B0) |
            P_Fld(0x2, MISC_SHU_RANK_SEL_LAT_RANK_SEL_LAT_B1) | P_Fld(0x2, MISC_SHU_RANK_SEL_LAT_RANK_SEL_LAT_CA));

    #if !CODE_SIZE_REDUCE

    vIO32WriteFldAlign(DDRPHY_REG_MISC_SHU_RK_DQSCTL, 0x7, MISC_SHU_RK_DQSCTL_DQSINCTL);

    vIO32WriteFldAlign(DDRPHY_REG_MISC_SHU_RK_DQSCTL+(1*DDRPHY_AO_RANK_OFFSET), 0x7, MISC_SHU_RK_DQSCTL_DQSINCTL);

    vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B0_DQSIEN_MCK_UI_DLY, P_Fld(0x0, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B0) |
            P_Fld(0x4, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B0) | P_Fld(0x1, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B0) |
            P_Fld(0x1, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B0));

    vIO32WriteFldAlign(DDRPHY_REG_SHU_RK_B0_DQSIEN_PI_DLY, 0x0f, SHU_RK_B0_DQSIEN_PI_DLY_DQSIEN_PI_B0);

    vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B0_DQSIEN_MCK_UI_DLY+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x7, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B0) |
            P_Fld(0xb, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B0) | P_Fld(0x1, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B0) |
            P_Fld(0x1, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B0));

    vIO32WriteFldAlign(DDRPHY_REG_SHU_RK_B0_DQSIEN_PI_DLY+(1*DDRPHY_AO_RANK_OFFSET), 0x1c, SHU_RK_B0_DQSIEN_PI_DLY_DQSIEN_PI_B0);

    vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B1_DQSIEN_MCK_UI_DLY, P_Fld(0x0, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B1) |
            P_Fld(0x4, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B1) | P_Fld(0x1, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B1) |
            P_Fld(0x1, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B1));

    vIO32WriteFldAlign(DDRPHY_REG_SHU_RK_B1_DQSIEN_PI_DLY, 0x0f, SHU_RK_B1_DQSIEN_PI_DLY_DQSIEN_PI_B1);

    vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B1_DQSIEN_MCK_UI_DLY+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x7, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B1) |
            P_Fld(0xb, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B1) | P_Fld(0x1, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B1) |
            P_Fld(0x1, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B1));

    vIO32WriteFldAlign(DDRPHY_REG_SHU_RK_B1_DQSIEN_PI_DLY+(1*DDRPHY_AO_RANK_OFFSET), 0x1c, SHU_RK_B1_DQSIEN_PI_DLY_DQSIEN_PI_B1);
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_ODTCTRL, P_Fld(0x1, MISC_SHU_ODTCTRL_RODTEN) |
            P_Fld(0x0, MISC_SHU_ODTCTRL_RODTENSTB_SELPH_CG_IG) |
            P_Fld(0x0, MISC_SHU_ODTCTRL_RODTEN_SELPH_FRUN) | P_Fld(0x0, MISC_SHU_ODTCTRL_RODTDLY_LAT_OPT) |
            P_Fld(0x0, MISC_SHU_ODTCTRL_FIXRODT) | P_Fld(0x1, MISC_SHU_ODTCTRL_RODTEN_OPT) |
            P_Fld(0x1, MISC_SHU_ODTCTRL_RODTE2) | P_Fld(0x1, MISC_SHU_ODTCTRL_RODTE));
    #if !CODE_SIZE_REDUCE
    vIO32WriteFldAlign(DDRPHY_REG_MISC_SHU_ODTCTRL, 0x7, MISC_SHU_ODTCTRL_RODT_LAT);
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ7, P_Fld(0x0, SHU_B0_DQ7_R_DMRANKRXDVS_B0) |
            P_Fld(0x0, SHU_B0_DQ7_R_DMDQMDBI_EYE_SHU_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMDQMDBI_SHU_B0) |
            P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_DQM_FLAGSEL_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B0) |
            P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMRXTRACK_DQM_EN_B0) |
            P_Fld(0x1, SHU_B0_DQ7_R_DMRODTEN_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMARPI_CG_FB2DLL_DCM_EN_B0) |
            P_Fld(0x0, SHU_B0_DQ7_R_DMTX_ARPI_CG_DQ_NEW_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMTX_ARPI_CG_DQS_NEW_B0) |
            P_Fld(0x0, SHU_B0_DQ7_R_DMTX_ARPI_CG_DQM_NEW_B0) | P_Fld(0x0, SHU_B0_DQ7_R_LP4Y_SDN_MODE_DQS0) |
            P_Fld(0x1, SHU_B0_DQ7_R_DMRXRANK_DQ_EN_B0) | P_Fld(0x1, SHU_B0_DQ7_R_DMRXRANK_DQ_LAT_B0) |
            P_Fld(0x1, SHU_B0_DQ7_R_DMRXRANK_DQS_EN_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMRXRANK_DQS_LAT_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ7, P_Fld(0x0, SHU_B1_DQ7_R_DMRANKRXDVS_B1) |
            P_Fld(0x0, SHU_B1_DQ7_R_DMDQMDBI_EYE_SHU_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMDQMDBI_SHU_B1) |
            P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_DQM_FLAGSEL_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B1) |
            P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMRXTRACK_DQM_EN_B1) |
            P_Fld(0x1, SHU_B1_DQ7_R_DMRODTEN_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMARPI_CG_FB2DLL_DCM_EN_B1) |
            P_Fld(0x0, SHU_B1_DQ7_R_DMTX_ARPI_CG_DQ_NEW_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMTX_ARPI_CG_DQS_NEW_B1) |
            P_Fld(0x0, SHU_B1_DQ7_R_DMTX_ARPI_CG_DQM_NEW_B1) | P_Fld(0x0, SHU_B1_DQ7_R_LP4Y_SDN_MODE_DQS1) |
            P_Fld(0x1, SHU_B1_DQ7_R_DMRXRANK_DQ_EN_B1) | P_Fld(0x1, SHU_B1_DQ7_R_DMRXRANK_DQ_LAT_B1) |
            P_Fld(0x1, SHU_B1_DQ7_R_DMRXRANK_DQS_EN_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMRXRANK_DQS_LAT_B1));

    vIO32WriteFldAlign(DDRPHY_REG_SHU_MISC_RX_PIPE_CTRL, 0x1, SHU_MISC_RX_PIPE_CTRL_RX_PIPE_BYPASS_EN);

    vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B0_RODTEN_MCK_UI_DLY, P_Fld(0x3, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B0) |
            P_Fld(0x3, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B0) | P_Fld(0x0, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B0) |
            P_Fld(0x0, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B0_RODTEN_MCK_UI_DLY+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x2, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B0) |
            P_Fld(0x2, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B0) | P_Fld(0x1, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B0) |
            P_Fld(0x0, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B1_RODTEN_MCK_UI_DLY, P_Fld(0x3, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B1) |
            P_Fld(0x3, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B1) | P_Fld(0x0, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B1) |
            P_Fld(0x0, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B1_RODTEN_MCK_UI_DLY+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x2, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B1) |
            P_Fld(0x2, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B1) | P_Fld(0x1, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B1) |
            P_Fld(0x0, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B1));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DRAMC_REG_SHU_RX_CG_SET0, P_Fld(0x0, SHU_RX_CG_SET0_DLE_LAST_EXTEND3) |
            P_Fld(0x0, SHU_RX_CG_SET0_READ_START_EXTEND3) | P_Fld(0x1, SHU_RX_CG_SET0_DLE_LAST_EXTEND2) |
            P_Fld(0x1, SHU_RX_CG_SET0_READ_START_EXTEND2) | P_Fld(0x1, SHU_RX_CG_SET0_DLE_LAST_EXTEND1) |
            P_Fld(0x1, SHU_RX_CG_SET0_READ_START_EXTEND1));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_MISC_RANK_SEL_STB, P_Fld(0x1, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_EN) |
            P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_EN_B23) | P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_SERMODE) |
            P_Fld(0x1, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_TRACK) | P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_RXDLY_TRACK) |
            P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_PHASE_EN) | P_Fld(0x6, SHU_MISC_RANK_SEL_STB_RANK_SEL_PHSINCTL) |
            P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_UI_PLUS) | P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_MCK_PLUS) |
            P_Fld(0x2, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_UI_MINUS) | P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_MCK_MINUS));

    vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RK_DQSCAL, P_Fld(0x60, MISC_SHU_RK_DQSCAL_DQSIENLLMT) |
            P_Fld(0x1, MISC_SHU_RK_DQSCAL_DQSIENLLMTEN) | P_Fld(0x3f, MISC_SHU_RK_DQSCAL_DQSIENHLMT) |
            P_Fld(0x1, MISC_SHU_RK_DQSCAL_DQSIENHLMTEN));

    vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RK_DQSCAL+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x60, MISC_SHU_RK_DQSCAL_DQSIENLLMT) |
            P_Fld(0x1, MISC_SHU_RK_DQSCAL_DQSIENLLMTEN) | P_Fld(0x3f, MISC_SHU_RK_DQSCAL_DQSIENHLMT) |
            P_Fld(0x1, MISC_SHU_RK_DQSCAL_DQSIENHLMTEN));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_INI_UIPI, P_Fld(0x0f, SHU_R0_B0_INI_UIPI_CURR_INI_PI_B0) |
            P_Fld(0x10, SHU_R0_B0_INI_UIPI_CURR_INI_UI_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_INI_UIPI, P_Fld(0x0f, SHU_R0_B1_INI_UIPI_CURR_INI_PI_B1) |
            P_Fld(0x10, SHU_R0_B1_INI_UIPI_CURR_INI_UI_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_INI_UIPI+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x1c, SHU_R0_B0_INI_UIPI_CURR_INI_PI_B0) |
            P_Fld(0x17, SHU_R0_B0_INI_UIPI_CURR_INI_UI_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_INI_UIPI+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x1c, SHU_R0_B1_INI_UIPI_CURR_INI_PI_B1) |
            P_Fld(0x17, SHU_R0_B1_INI_UIPI_CURR_INI_UI_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_NEXT_INI_UIPI, P_Fld(0x0f, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_PI_B0) |
            P_Fld(0x10, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_UI_B0) | P_Fld(0x14, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_UI_P1_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_NEXT_INI_UIPI, P_Fld(0x0f, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_PI_B1) |
            P_Fld(0x10, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_UI_B1) | P_Fld(0x14, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_UI_P1_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_NEXT_INI_UIPI+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x1c, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_PI_B0) |
            P_Fld(0x17, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_UI_B0) | P_Fld(0x1b, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_UI_P1_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_NEXT_INI_UIPI+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x1c, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_PI_B1) |
            P_Fld(0x17, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_UI_B1) | P_Fld(0x1b, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_UI_P1_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_CA_CMD0, P_Fld(0x0, SHU_R0_CA_CMD0_RG_RX_ARCLK_R_DLY_DUTY) |
            P_Fld(0x0, SHU_R0_CA_CMD0_RG_RX_ARCLK_F_DLY_DUTY) |
            P_Fld(0x0, SHU_R0_CA_CMD0_DA_ARPI_DDR400_0D5UI_RK0_CA) | P_Fld(0x0, SHU_R0_CA_CMD0_DA_RX_ARDQSIEN_0D5UI_RK0_CA));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_CA_CMD0, P_Fld(0x00, SHU_R0_CA_CMD0_RG_ARPI_CS) |
            P_Fld(0x20, SHU_R0_CA_CMD0_RG_ARPI_CMD) | P_Fld(0x00, SHU_R0_CA_CMD0_RG_ARPI_CLK));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_DQ0, P_Fld(0x0, SHU_R0_B0_DQ0_RG_RX_ARDQS0_R_DLY_DUTY) |
            P_Fld(0x0, SHU_R0_B0_DQ0_RG_RX_ARDQS0_F_DLY_DUTY) |
            P_Fld(0x0, SHU_R0_B0_DQ0_DA_ARPI_DDR400_0D5UI_RK0_B0) | P_Fld(0x0, SHU_R0_B0_DQ0_DA_RX_ARDQSIEN_0D5UI_RK0_B0));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_DQ0, P_Fld(0x11, SHU_R0_B0_DQ0_SW_ARPI_DQ_B0) |
            P_Fld(0x11, SHU_R0_B0_DQ0_SW_ARPI_DQM_B0) | P_Fld(0x00, SHU_R0_B0_DQ0_ARPI_PBYTE_B0));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_DQ0, P_Fld(0x0, SHU_R0_B1_DQ0_RG_RX_ARDQS1_R_DLY_DUTY) |
            P_Fld(0x0, SHU_R0_B1_DQ0_RG_RX_ARDQS1_F_DLY_DUTY) |
            P_Fld(0x0, SHU_R0_B1_DQ0_DA_ARPI_DDR400_0D5UI_RK0_B1) | P_Fld(0x0, SHU_R0_B1_DQ0_DA_RX_ARDQSIEN_0D5UI_RK0_B1));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_DQ0, P_Fld(0x12, SHU_R0_B1_DQ0_SW_ARPI_DQ_B1) |
            P_Fld(0x12, SHU_R0_B1_DQ0_SW_ARPI_DQM_B1) | P_Fld(0x00, SHU_R0_B1_DQ0_ARPI_PBYTE_B1));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_CA_CMD0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x0, SHU_R0_CA_CMD0_RG_RX_ARCLK_R_DLY_DUTY) |
            P_Fld(0x0, SHU_R0_CA_CMD0_RG_RX_ARCLK_F_DLY_DUTY) |
            P_Fld(0x0, SHU_R0_CA_CMD0_DA_ARPI_DDR400_0D5UI_RK0_CA) | P_Fld(0x0, SHU_R0_CA_CMD0_DA_RX_ARDQSIEN_0D5UI_RK0_CA));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_CA_CMD0+(1*DDRPHY_AO_RANK_OFFSET),P_Fld(0x00, SHU_R0_CA_CMD0_RG_ARPI_CS) |
            P_Fld(0x20, SHU_R0_CA_CMD0_RG_ARPI_CMD) | P_Fld(0x00, SHU_R0_CA_CMD0_RG_ARPI_CLK));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_DQ0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x0, SHU_R0_B0_DQ0_RG_RX_ARDQS0_R_DLY_DUTY) |
            P_Fld(0x0, SHU_R0_B0_DQ0_RG_RX_ARDQS0_F_DLY_DUTY) |
            P_Fld(0x0, SHU_R0_B0_DQ0_DA_ARPI_DDR400_0D5UI_RK0_B0) | P_Fld(0x0, SHU_R0_B0_DQ0_DA_RX_ARDQSIEN_0D5UI_RK0_B0));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_DQ0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x16, SHU_R0_B0_DQ0_SW_ARPI_DQ_B0) |
            P_Fld(0x16, SHU_R0_B0_DQ0_SW_ARPI_DQM_B0) | P_Fld(0x00, SHU_R0_B0_DQ0_ARPI_PBYTE_B0));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_DQ0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x0, SHU_R0_B1_DQ0_RG_RX_ARDQS1_R_DLY_DUTY) |
            P_Fld(0x0, SHU_R0_B1_DQ0_RG_RX_ARDQS1_F_DLY_DUTY) |
            P_Fld(0x0, SHU_R0_B1_DQ0_DA_ARPI_DDR400_0D5UI_RK0_B1) | P_Fld(0x0, SHU_R0_B1_DQ0_DA_RX_ARDQSIEN_0D5UI_RK0_B1));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_DQ0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x21, SHU_R0_B1_DQ0_SW_ARPI_DQ_B1) |
            P_Fld(0x21, SHU_R0_B1_DQ0_SW_ARPI_DQM_B1) | P_Fld(0x00, SHU_R0_B1_DQ0_ARPI_PBYTE_B1));
    #endif

    vIO32WriteFldMulti(DRAMC_REG_SHU_DCM_CTRL0, P_Fld(0x1, SHU_DCM_CTRL0_DDRPHY_CLK_EN_OPT) |
            P_Fld(0x3, SHU_DCM_CTRL0_DPHY_CMD_CLKEN_EXTCNT) | P_Fld(0x5, SHU_DCM_CTRL0_DDRPHY_CLK_DYN_GATING_SEL) |
            P_Fld(0x2, SHU_DCM_CTRL0_APHYPI_CKCGL_CNT) | P_Fld(0x4, SHU_DCM_CTRL0_APHYPI_CKCGH_CNT) |
            P_Fld(0x0, SHU_DCM_CTRL0_FASTWAKE2) | P_Fld(0x1, SHU_DCM_CTRL0_FASTWAKE));

    vIO32WriteFldMulti(DRAMC_REG_SHU_APHY_TX_PICG_CTRL, P_Fld(0x3, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_PICG_CNT) |
            P_Fld(0x3, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQS_SEL_P1) | P_Fld(0x2, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQS_SEL_P0) |
            P_Fld(0x2, SHU_APHY_TX_PICG_CTRL_DPHY_TX_DCM_EXTCNT) | P_Fld(0x1, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_OPT));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_APHY_TX_PICG_CTRL, P_Fld(0x3, SHURK_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQ_RK_SEL_P1) |
            P_Fld(0x3, SHURK_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQ_RK_SEL_P0));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_APHY_TX_PICG_CTRL+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x3, SHURK_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQ_RK_SEL_P1) |
            P_Fld(0x3, SHURK_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQ_RK_SEL_P0));

    vIO32WriteFldMulti(DRAMC_REG_SHU_NEW_XRW2W_CTRL, P_Fld(0x3, SHU_NEW_XRW2W_CTRL_TX_PI_UPDCTL_B0) |
            P_Fld(0x3, SHU_NEW_XRW2W_CTRL_TX_PI_UPDCTL_B1) | P_Fld(0x0, SHU_NEW_XRW2W_CTRL_TXPI_UPD_MODE));

    vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_DQS0, P_Fld(0x1, SHU_SELPH_DQS0_TXDLY_DQS2) |
            P_Fld(0x1, SHU_SELPH_DQS0_TXDLY_DQS3) | P_Fld(0x1, SHU_SELPH_DQS0_TXDLY_OEN_DQS2) |
            P_Fld(0x1, SHU_SELPH_DQS0_TXDLY_OEN_DQS3));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_DQS0, P_Fld(0x4, SHU_SELPH_DQS0_TXDLY_DQS0) |
            P_Fld(0x4, SHU_SELPH_DQS0_TXDLY_DQS1) | P_Fld(0x3, SHU_SELPH_DQS0_TXDLY_OEN_DQS0) |
            P_Fld(0x3, SHU_SELPH_DQS0_TXDLY_OEN_DQS1));
    #endif

    vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_DQS1, P_Fld(0x1, SHU_SELPH_DQS1_DLY_DQS2) |
            P_Fld(0x1, SHU_SELPH_DQS1_DLY_DQS3) | P_Fld(0x1, SHU_SELPH_DQS1_DLY_OEN_DQS2) |
            P_Fld(0x1, SHU_SELPH_DQS1_DLY_OEN_DQS3));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_DQS1, P_Fld(0x1, SHU_SELPH_DQS1_DLY_DQS0) |
            P_Fld(0x1, SHU_SELPH_DQS1_DLY_DQS1) | P_Fld(0x6, SHU_SELPH_DQS1_DLY_OEN_DQS0) |
            P_Fld(0x6, SHU_SELPH_DQS1_DLY_OEN_DQS1));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ0, P_Fld(0x4, SHURK_SELPH_DQ0_TXDLY_DQ0) |
            P_Fld(0x4, SHURK_SELPH_DQ0_TXDLY_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_DQ2) |
            P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_DQ3) | P_Fld(0x3, SHURK_SELPH_DQ0_TXDLY_OEN_DQ0) |
            P_Fld(0x3, SHURK_SELPH_DQ0_TXDLY_OEN_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_OEN_DQ2) |
            P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_OEN_DQ3));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ1, P_Fld(0x4, SHURK_SELPH_DQ1_TXDLY_DQM0) |
            P_Fld(0x4, SHURK_SELPH_DQ1_TXDLY_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_DQM2) |
            P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_DQM3) | P_Fld(0x3, SHURK_SELPH_DQ1_TXDLY_OEN_DQM0) |
            P_Fld(0x3, SHURK_SELPH_DQ1_TXDLY_OEN_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_OEN_DQM2) |
            P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_OEN_DQM3));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ2, P_Fld(0x2, SHURK_SELPH_DQ2_DLY_DQ0) |
            P_Fld(0x2, SHURK_SELPH_DQ2_DLY_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ2_DLY_DQ2) |
            P_Fld(0x1, SHURK_SELPH_DQ2_DLY_DQ3) | P_Fld(0x7, SHURK_SELPH_DQ2_DLY_OEN_DQ0) |
            P_Fld(0x7, SHURK_SELPH_DQ2_DLY_OEN_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ2_DLY_OEN_DQ2) |
            P_Fld(0x1, SHURK_SELPH_DQ2_DLY_OEN_DQ3));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ3, P_Fld(0x2, SHURK_SELPH_DQ3_DLY_DQM0) |
            P_Fld(0x2, SHURK_SELPH_DQ3_DLY_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ3_DLY_DQM2) |
            P_Fld(0x1, SHURK_SELPH_DQ3_DLY_DQM3) | P_Fld(0x7, SHURK_SELPH_DQ3_DLY_OEN_DQM0) |
            P_Fld(0x7, SHURK_SELPH_DQ3_DLY_OEN_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ3_DLY_OEN_DQM2) |
            P_Fld(0x1, SHURK_SELPH_DQ3_DLY_OEN_DQM3));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ0+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x4, SHURK_SELPH_DQ0_TXDLY_DQ0) |
            P_Fld(0x4, SHURK_SELPH_DQ0_TXDLY_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_DQ2) |
            P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_DQ3) | P_Fld(0x4, SHURK_SELPH_DQ0_TXDLY_OEN_DQ0) |
            P_Fld(0x4, SHURK_SELPH_DQ0_TXDLY_OEN_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_OEN_DQ2) |
            P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_OEN_DQ3));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ1+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x4, SHURK_SELPH_DQ1_TXDLY_DQM0) |
            P_Fld(0x4, SHURK_SELPH_DQ1_TXDLY_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_DQM2) |
            P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_DQM3) | P_Fld(0x4, SHURK_SELPH_DQ1_TXDLY_OEN_DQM0) |
            P_Fld(0x4, SHURK_SELPH_DQ1_TXDLY_OEN_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_OEN_DQM2) |
            P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_OEN_DQM3));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ2+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x3, SHURK_SELPH_DQ2_DLY_DQ0) |
            P_Fld(0x3, SHURK_SELPH_DQ2_DLY_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ2_DLY_DQ2) |
            P_Fld(0x1, SHURK_SELPH_DQ2_DLY_DQ3) | P_Fld(0x0, SHURK_SELPH_DQ2_DLY_OEN_DQ0) |
            P_Fld(0x0, SHURK_SELPH_DQ2_DLY_OEN_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ2_DLY_OEN_DQ2) |
            P_Fld(0x1, SHURK_SELPH_DQ2_DLY_OEN_DQ3));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ3+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x3, SHURK_SELPH_DQ3_DLY_DQM0) |
            P_Fld(0x3, SHURK_SELPH_DQ3_DLY_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ3_DLY_DQM2) |
            P_Fld(0x1, SHURK_SELPH_DQ3_DLY_DQM3) | P_Fld(0x0, SHURK_SELPH_DQ3_DLY_OEN_DQM0) |
            P_Fld(0x0, SHURK_SELPH_DQ3_DLY_OEN_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ3_DLY_OEN_DQM2) |
            P_Fld(0x1, SHURK_SELPH_DQ3_DLY_OEN_DQM3));
    #endif

    vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL1, P_Fld(0x011, SHURK_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ0) |
            P_Fld(0x012, SHURK_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ1));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL2, P_Fld(0x011, SHURK_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ0) |
            P_Fld(0x012, SHURK_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ1));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL5, P_Fld(0x011, SHURK_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM0) |
            P_Fld(0x012, SHURK_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM1));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL1+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x016, SHURK_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ0) |
            P_Fld(0x021, SHURK_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ1));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL2+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x016, SHURK_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ0) |
            P_Fld(0x021, SHURK_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ1));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL5+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x016, SHURK_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM0) |
            P_Fld(0x021, SHURK_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM1));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_PI, P_Fld(0x12, SHURK_PI_RK0_ARPI_DQ_B1) |
            P_Fld(0x11, SHURK_PI_RK0_ARPI_DQ_B0) | P_Fld(0x12, SHURK_PI_RK0_ARPI_DQM_B1) |
            P_Fld(0x11, SHURK_PI_RK0_ARPI_DQM_B0));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_PI+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x21, SHURK_PI_RK0_ARPI_DQ_B1) |
            P_Fld(0x16, SHURK_PI_RK0_ARPI_DQ_B0) | P_Fld(0x21, SHURK_PI_RK0_ARPI_DQM_B1) |
            P_Fld(0x16, SHURK_PI_RK0_ARPI_DQM_B0));

    #if !CODE_SIZE_REDUCE

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_TXDLY0, P_Fld(0x08, SHU_R0_B0_TXDLY0_TX_ARDQ0_DLY_B0) |
            P_Fld(0x08, SHU_R0_B0_TXDLY0_TX_ARDQ1_DLY_B0) | P_Fld(0x08, SHU_R0_B0_TXDLY0_TX_ARDQ2_DLY_B0) |
            P_Fld(0x08, SHU_R0_B0_TXDLY0_TX_ARDQ3_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_TXDLY1, P_Fld(0x08, SHU_R0_B0_TXDLY1_TX_ARDQ4_DLY_B0) |
            P_Fld(0x08, SHU_R0_B0_TXDLY1_TX_ARDQ5_DLY_B0) | P_Fld(0x08, SHU_R0_B0_TXDLY1_TX_ARDQ6_DLY_B0) |
            P_Fld(0x08, SHU_R0_B0_TXDLY1_TX_ARDQ7_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_TXDLY3, P_Fld(0x08, SHU_R0_B0_TXDLY3_TX_ARDQM0_DLY_B0) |
            P_Fld(0x00, SHU_R0_B0_TXDLY3_TX_ARWCK_DLY_B0) | P_Fld(0x00, SHU_R0_B0_TXDLY3_TX_ARWCKB_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_TXDLY0, P_Fld(0x04, SHU_R0_B1_TXDLY0_TX_ARDQ0_DLY_B1) |
            P_Fld(0x04, SHU_R0_B1_TXDLY0_TX_ARDQ1_DLY_B1) | P_Fld(0x04, SHU_R0_B1_TXDLY0_TX_ARDQ2_DLY_B1) |
            P_Fld(0x04, SHU_R0_B1_TXDLY0_TX_ARDQ3_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_TXDLY1, P_Fld(0x04, SHU_R0_B1_TXDLY1_TX_ARDQ4_DLY_B1) |
            P_Fld(0x04, SHU_R0_B1_TXDLY1_TX_ARDQ5_DLY_B1) | P_Fld(0x04, SHU_R0_B1_TXDLY1_TX_ARDQ6_DLY_B1) |
            P_Fld(0x04, SHU_R0_B1_TXDLY1_TX_ARDQ7_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_TXDLY3, P_Fld(0x04, SHU_R0_B1_TXDLY3_TX_ARDQM0_DLY_B1) |
            P_Fld(0x00, SHU_R0_B1_TXDLY3_TX_ARWCK_DLY_B1) | P_Fld(0x00, SHU_R0_B1_TXDLY3_TX_ARWCKB_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_TXDLY0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x34, SHU_R0_B0_TXDLY0_TX_ARDQ0_DLY_B0) |
            P_Fld(0x34, SHU_R0_B0_TXDLY0_TX_ARDQ1_DLY_B0) | P_Fld(0x34, SHU_R0_B0_TXDLY0_TX_ARDQ2_DLY_B0) |
            P_Fld(0x34, SHU_R0_B0_TXDLY0_TX_ARDQ3_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_TXDLY1+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x34, SHU_R0_B0_TXDLY1_TX_ARDQ4_DLY_B0) |
            P_Fld(0x34, SHU_R0_B0_TXDLY1_TX_ARDQ5_DLY_B0) | P_Fld(0x34, SHU_R0_B0_TXDLY1_TX_ARDQ6_DLY_B0) |
            P_Fld(0x34, SHU_R0_B0_TXDLY1_TX_ARDQ7_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_TXDLY3+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x34, SHU_R0_B0_TXDLY3_TX_ARDQM0_DLY_B0) |
            P_Fld(0x00, SHU_R0_B0_TXDLY3_TX_ARWCK_DLY_B0) | P_Fld(0x00, SHU_R0_B0_TXDLY3_TX_ARWCKB_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_TXDLY0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x08, SHU_R0_B1_TXDLY0_TX_ARDQ0_DLY_B1) |
            P_Fld(0x08, SHU_R0_B1_TXDLY0_TX_ARDQ1_DLY_B1) | P_Fld(0x08, SHU_R0_B1_TXDLY0_TX_ARDQ2_DLY_B1) |
            P_Fld(0x08, SHU_R0_B1_TXDLY0_TX_ARDQ3_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_TXDLY1+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x08, SHU_R0_B1_TXDLY1_TX_ARDQ4_DLY_B1) |
            P_Fld(0x08, SHU_R0_B1_TXDLY1_TX_ARDQ5_DLY_B1) | P_Fld(0x08, SHU_R0_B1_TXDLY1_TX_ARDQ6_DLY_B1) |
            P_Fld(0x08, SHU_R0_B1_TXDLY1_TX_ARDQ7_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_TXDLY3+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x08, SHU_R0_B1_TXDLY3_TX_ARDQM0_DLY_B1) |
            P_Fld(0x00, SHU_R0_B1_TXDLY3_TX_ARWCK_DLY_B1) | P_Fld(0x00, SHU_R0_B1_TXDLY3_TX_ARWCKB_DLY_B1));

    vIO32WriteFldMulti(DRAMC_REG_SHU_TX_RANKCTL, P_Fld(0x2, SHU_TX_RANKCTL_TXRANKINCTL_TXDLY) |
            P_Fld(0x2, SHU_TX_RANKCTL_TXRANKINCTL) | P_Fld(0x0, SHU_TX_RANKCTL_TXRANKINCTL_ROOT));
    #endif

    #if !(CODE_SIZE_REDUCE && AC_TIMING_DERATE_ENABLE)
    vIO32WriteFldMulti(DRAMC_REG_SHU_AC_DERATING0, P_Fld(0x0, SHU_AC_DERATING0_ACDERATEEN) |
            P_Fld(0x5, SHU_AC_DERATING0_TRRD_DERATE) | P_Fld(0x9, SHU_AC_DERATING0_TRCD_DERATE));

    vIO32WriteFldMulti(DRAMC_REG_SHU_AC_DERATING1, P_Fld(0x9, SHU_AC_DERATING1_TRPAB_DERATE) |
            P_Fld(0x8, SHU_AC_DERATING1_TRP_DERATE) | P_Fld(0x0c, SHU_AC_DERATING1_TRAS_DERATE) |
            P_Fld(0x00, SHU_AC_DERATING1_TRC_DERATE));

    vIO32WriteFldMulti(DRAMC_REG_SHU_AC_DERATING_05T, P_Fld(0x0, SHU_AC_DERATING_05T_TRC_05T_DERATE) |
            P_Fld(0x1, SHU_AC_DERATING_05T_TRCD_05T_DERATE) | P_Fld(0x0, SHU_AC_DERATING_05T_TRP_05T_DERATE) |
            P_Fld(0x1, SHU_AC_DERATING_05T_TRPAB_05T_DERATE) | P_Fld(0x0, SHU_AC_DERATING_05T_TRAS_05T_DERATE) |
            P_Fld(0x0, SHU_AC_DERATING_05T_TRRD_05T_DERATE));
    #endif

    vIO32WriteFldMulti(DRAMC_REG_SHU_SREF_CTRL, P_Fld(0x3, SHU_SREF_CTRL_CKEHCMD) |
            P_Fld(0x3, SHU_SREF_CTRL_SREF_CK_DLY));

    vIO32WriteFldMulti(DRAMC_REG_SHU_HMR4_DVFS_CTRL0, P_Fld(0x75, SHU_HMR4_DVFS_CTRL0_FSPCHG_PRDCNT) |
            P_Fld(0x000, SHU_HMR4_DVFS_CTRL0_REFRCNT));

    vIO32WriteFldMulti(DRAMC_REG_SHU_AC_TIME_05T,
            P_Fld(0x0, SHU_AC_TIME_05T_TCKEPRD_05T) | P_Fld(0x0, SHU_AC_TIME_05T_BGTRRD_05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_BGTCCD_05T) | P_Fld(0x0, SHU_AC_TIME_05T_BGTWTR_M05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_XRTR2W_05T) | P_Fld(0x0, SHU_AC_TIME_05T_XRTW2R_M05T));

     #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DRAMC_REG_SHU_AC_TIME_05T, P_Fld(0x0, SHU_AC_TIME_05T_TRC_05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_TRFCPB_05T) | P_Fld(0x1, SHU_AC_TIME_05T_TRFC_05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_TPBR2PBR_05T) | P_Fld(0x1, SHU_AC_TIME_05T_TXP_05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_TRTP_05T) | P_Fld(0x1, SHU_AC_TIME_05T_TRCD_05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_TRP_05T) | P_Fld(0x1, SHU_AC_TIME_05T_TRPAB_05T) |
            P_Fld(0x1, SHU_AC_TIME_05T_TRAS_05T) | P_Fld(0x1, SHU_AC_TIME_05T_TWR_M05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_TRRD_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TFAW_05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_TR2PD_05T) | P_Fld(0x1, SHU_AC_TIME_05T_TWTPD_M05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_TMRRI_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TMRWCKEL_05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_TR2PD_05T) | P_Fld(0x1, SHU_AC_TIME_05T_TWTPD_M05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_TMRRI_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TMRWCKEL_05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_TR2W_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TWTR_M05T) |
            P_Fld(0x1, SHU_AC_TIME_05T_TMRD_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TMRW_05T) |
            P_Fld(0x1, SHU_AC_TIME_05T_TMRR2MRW_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TW2MRW_05T) |
            P_Fld(0x1, SHU_AC_TIME_05T_TR2MRW_05T) | P_Fld(0x1, SHU_AC_TIME_05T_TPBR2ACT_05T));

    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM_XRT, P_Fld(0x03, SHU_ACTIM_XRT_XRTR2R) |
            P_Fld(0x08, SHU_ACTIM_XRT_XRTR2W) | P_Fld(0x1, SHU_ACTIM_XRT_XRTW2R) |
            P_Fld(0x05, SHU_ACTIM_XRT_XRTW2W));

    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM0, P_Fld(0x08, SHU_ACTIM0_TWTR) |
            P_Fld(0x0d, SHU_ACTIM0_TWR) | P_Fld(0x4, SHU_ACTIM0_TRRD) |
            P_Fld(0x8, SHU_ACTIM0_TRCD) | P_Fld(0x3, SHU_ACTIM0_CKELCKCNT));

    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM1, P_Fld(0x8, SHU_ACTIM1_TRPAB) |
            P_Fld(0x8, SHU_ACTIM1_TMRWCKEL) | P_Fld(0x7, SHU_ACTIM1_TRP) |
            P_Fld(0x0b, SHU_ACTIM1_TRAS) | P_Fld(0x00, SHU_ACTIM1_TRC));

    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM2, P_Fld(0x0, SHU_ACTIM2_TXP) |
            P_Fld(0x0c, SHU_ACTIM2_TMRRI) | P_Fld(0x2, SHU_ACTIM2_TRTP) |
            P_Fld(0x09, SHU_ACTIM2_TR2W) | P_Fld(0x0b, SHU_ACTIM2_TFAW));

    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM3, P_Fld(0x4d, SHU_ACTIM3_TRFCPB) |
            P_Fld(0x4, SHU_ACTIM3_TR2MRR) | P_Fld(0xa5, SHU_ACTIM3_TRFC));
    #endif

    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM3, P_Fld(0x4, SHU_ACTIM3_MANTMRR) |
            P_Fld(0x00, SHU_ACTIM3_TWTR_L));
    #if !CODE_SIZE_REDUCE

    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM4, P_Fld(0x0b5, SHU_ACTIM4_TXREFCNT) |
            P_Fld(0x0d, SHU_ACTIM4_TMRR2MRW) | P_Fld(0x0c, SHU_ACTIM4_TMRR2W) |
            P_Fld(0x28, SHU_ACTIM4_TZQCS));

    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM5, P_Fld(0x0e, SHU_ACTIM5_TR2PD) |
            P_Fld(0x10, SHU_ACTIM5_TWTPD) | P_Fld(0x23, SHU_ACTIM5_TPBR2PBR) |
            P_Fld(0x0, SHU_ACTIM5_TPBR2ACT));

    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM6, P_Fld(0x0e, SHU_ACTIM6_TZQLAT2) |
            P_Fld(0x7, SHU_ACTIM6_TMRD) | P_Fld(0x5, SHU_ACTIM6_TMRW) |
            P_Fld(0x0a, SHU_ACTIM6_TW2MRW) | P_Fld(0x10, SHU_ACTIM6_TR2MRW));

    vIO32WriteFldMulti(DRAMC_REG_SHU_CKECTRL, P_Fld(0x1, SHU_CKECTRL_TPDE_05T) |
            P_Fld(0x0, SHU_CKECTRL_TPDX_05T) | P_Fld(0x1, SHU_CKECTRL_TPDE) |
            P_Fld(0x1, SHU_CKECTRL_TPDX) | P_Fld(0x3, SHU_CKECTRL_TCKEPRD));
    #endif

    vIO32WriteFldAlign(DRAMC_REG_SHU_CKECTRL, 0x3, SHU_CKECTRL_TCKESRX);

    vIO32WriteFldMulti(DRAMC_REG_SHU_MISC, P_Fld(0x7, SHU_MISC_DCMDLYREF) |
            P_Fld(0x0, SHU_MISC_DAREFEN));

    #if !CODE_SIZE_REDUCE
	vIO32WriteFldAlign(DRAMC_REG_SHU_MISC, 0x2, SHU_MISC_REQQUE_MAXCNT);

    vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ8, P_Fld(0x0, SHU_B0_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B0) |
            P_Fld(0x0, SHU_B0_DQ8_R_RMRODTEN_CG_IG_B0) | P_Fld(0x1, SHU_B0_DQ8_R_RMRX_TOPHY_CG_IG_B0) |
            P_Fld(0x0, SHU_B0_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B0) |
            P_Fld(0x1, SHU_B0_DQ8_R_DMRXDLY_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B0) |
            P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B0) |
            P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMRANK_PIPE_CG_IG_B0) |
            P_Fld(0x0, SHU_B0_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B0));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ8, P_Fld(0x00e7, SHU_B0_DQ8_R_DMRXDVS_UPD_FORCE_CYC_B0) |
            P_Fld(0x0, SHU_B0_DQ8_R_DMRXDVS_UPD_FORCE_EN_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ8, P_Fld(0x00e7, SHU_B1_DQ8_R_DMRXDVS_UPD_FORCE_CYC_B1) |
            P_Fld(0x0, SHU_B1_DQ8_R_DMRXDVS_UPD_FORCE_EN_B1));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ8, P_Fld(0x0, SHU_B1_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B1) |
            P_Fld(0x0, SHU_B1_DQ8_R_RMRODTEN_CG_IG_B1) | P_Fld(0x1, SHU_B1_DQ8_R_RMRX_TOPHY_CG_IG_B1) |
            P_Fld(0x0, SHU_B1_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B1) |
            P_Fld(0x1, SHU_B1_DQ8_R_DMRXDLY_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B1) |
            P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B1) |
            P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMRANK_PIPE_CG_IG_B1) |
            P_Fld(0x0, SHU_B1_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B1));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ5, P_Fld(0x0e, SHU_B0_DQ5_RG_RX_ARDQ_VREF_SEL_B0) |
            P_Fld(0x0, SHU_B0_DQ5_RG_RX_ARDQ_VREF_BYPASS_B0) | P_Fld(0x00, SHU_B0_DQ5_RG_ARPI_FB_B0) |
            P_Fld(0x0, SHU_B0_DQ5_RG_RX_ARDQS0_DQSIEN_DLY_B0) | P_Fld(0x0, SHU_B0_DQ5_RG_RX_ARDQS_DQSIEN_RB_DLY_B0) |
            P_Fld(0x4, SHU_B0_DQ5_RG_RX_ARDQS0_DVS_DLY_B0) | P_Fld(0x0, SHU_B0_DQ5_RG_RX_ARDQ_FIFO_DQSI_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ5, P_Fld(0x0e, SHU_B1_DQ5_RG_RX_ARDQ_VREF_SEL_B1) |
            P_Fld(0x0, SHU_B1_DQ5_RG_RX_ARDQ_VREF_BYPASS_B1) | P_Fld(0x00, SHU_B1_DQ5_RG_ARPI_FB_B1) |
            P_Fld(0x0, SHU_B1_DQ5_RG_RX_ARDQS0_DQSIEN_DLY_B1) | P_Fld(0x0, SHU_B1_DQ5_RG_RX_ARDQS_DQSIEN_RB_DLY_B1) |
            P_Fld(0x4, SHU_B1_DQ5_RG_RX_ARDQS0_DVS_DLY_B1) | P_Fld(0x0, SHU_B1_DQ5_RG_RX_ARDQ_FIFO_DQSI_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY0, P_Fld(0x6d, SHU_R0_B0_RXDLY0_RX_ARDQ0_F_DLY_B0) |
            P_Fld(0x6d, SHU_R0_B0_RXDLY0_RX_ARDQ1_F_DLY_B0));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY0, P_Fld(0x6d, SHU_R0_B0_RXDLY0_RX_ARDQ0_R_DLY_B0) |
            P_Fld(0x6d, SHU_R0_B0_RXDLY0_RX_ARDQ1_R_DLY_B0));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY1, P_Fld(0x6d, SHU_R0_B0_RXDLY1_RX_ARDQ2_R_DLY_B0) |
            P_Fld(0x6d, SHU_R0_B0_RXDLY1_RX_ARDQ2_F_DLY_B0) | P_Fld(0x6d, SHU_R0_B0_RXDLY1_RX_ARDQ3_R_DLY_B0) |
            P_Fld(0x6d, SHU_R0_B0_RXDLY1_RX_ARDQ3_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY2, P_Fld(0x6d, SHU_R0_B0_RXDLY2_RX_ARDQ4_R_DLY_B0) |
            P_Fld(0x6d, SHU_R0_B0_RXDLY2_RX_ARDQ4_F_DLY_B0) | P_Fld(0x6d, SHU_R0_B0_RXDLY2_RX_ARDQ5_R_DLY_B0) |
            P_Fld(0x6d, SHU_R0_B0_RXDLY2_RX_ARDQ5_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY3, P_Fld(0x6d, SHU_R0_B0_RXDLY3_RX_ARDQ6_R_DLY_B0) |
            P_Fld(0x6d, SHU_R0_B0_RXDLY3_RX_ARDQ6_F_DLY_B0) | P_Fld(0x6d, SHU_R0_B0_RXDLY3_RX_ARDQ7_R_DLY_B0) |
            P_Fld(0x6d, SHU_R0_B0_RXDLY3_RX_ARDQ7_F_DLY_B0));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY4, P_Fld(0x6d, SHU_R0_B0_RXDLY4_RX_ARDQM0_R_DLY_B0) |
            P_Fld(0x6d, SHU_R0_B0_RXDLY4_RX_ARDQM0_F_DLY_B0));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY5, P_Fld(0x061, SHU_R0_B0_RXDLY5_RX_ARDQS0_R_DLY_B0) |
            P_Fld(0x061, SHU_R0_B0_RXDLY5_RX_ARDQS0_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY0+(1*DDRPHY_AO_RANK_OFFSET),
            P_Fld(0x6c, SHU_R0_B0_RXDLY0_RX_ARDQ0_F_DLY_B0) |
            P_Fld(0x6c, SHU_R0_B0_RXDLY0_RX_ARDQ1_F_DLY_B0));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x6c, SHU_R0_B0_RXDLY0_RX_ARDQ0_R_DLY_B0) |
    P_Fld(0x6c, SHU_R0_B0_RXDLY0_RX_ARDQ1_R_DLY_B0));
	#endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY1+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x6c, SHU_R0_B0_RXDLY1_RX_ARDQ2_R_DLY_B0) |
            P_Fld(0x6c, SHU_R0_B0_RXDLY1_RX_ARDQ2_F_DLY_B0) | P_Fld(0x6c, SHU_R0_B0_RXDLY1_RX_ARDQ3_R_DLY_B0) |
            P_Fld(0x6c, SHU_R0_B0_RXDLY1_RX_ARDQ3_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY2+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x6c, SHU_R0_B0_RXDLY2_RX_ARDQ4_R_DLY_B0) |
            P_Fld(0x6c, SHU_R0_B0_RXDLY2_RX_ARDQ4_F_DLY_B0) | P_Fld(0x6c, SHU_R0_B0_RXDLY2_RX_ARDQ5_R_DLY_B0) |
            P_Fld(0x6c, SHU_R0_B0_RXDLY2_RX_ARDQ5_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY3+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x6c, SHU_R0_B0_RXDLY3_RX_ARDQ6_R_DLY_B0) |
            P_Fld(0x6c, SHU_R0_B0_RXDLY3_RX_ARDQ6_F_DLY_B0) | P_Fld(0x6c, SHU_R0_B0_RXDLY3_RX_ARDQ7_R_DLY_B0) |
            P_Fld(0x6c, SHU_R0_B0_RXDLY3_RX_ARDQ7_F_DLY_B0));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY4+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x6c, SHU_R0_B0_RXDLY4_RX_ARDQM0_R_DLY_B0) |
            P_Fld(0x6c, SHU_R0_B0_RXDLY4_RX_ARDQM0_F_DLY_B0));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY5+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x060, SHU_R0_B0_RXDLY5_RX_ARDQS0_R_DLY_B0) |
            P_Fld(0x060, SHU_R0_B0_RXDLY5_RX_ARDQS0_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY0, P_Fld(0x6d, SHU_R0_B1_RXDLY0_RX_ARDQ0_F_DLY_B1) |
            P_Fld(0x6d, SHU_R0_B1_RXDLY0_RX_ARDQ1_F_DLY_B1));
	
    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY0, P_Fld(0x6d, SHU_R0_B1_RXDLY0_RX_ARDQ0_R_DLY_B1) |
            P_Fld(0x6d, SHU_R0_B1_RXDLY0_RX_ARDQ0_R_DLY_B1));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY1, P_Fld(0x6d, SHU_R0_B1_RXDLY1_RX_ARDQ2_R_DLY_B1) |
            P_Fld(0x6d, SHU_R0_B1_RXDLY1_RX_ARDQ2_F_DLY_B1) | P_Fld(0x6d, SHU_R0_B1_RXDLY1_RX_ARDQ3_R_DLY_B1) |
            P_Fld(0x6d, SHU_R0_B1_RXDLY1_RX_ARDQ3_F_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY2, P_Fld(0x6d, SHU_R0_B1_RXDLY2_RX_ARDQ4_R_DLY_B1) |
            P_Fld(0x6d, SHU_R0_B1_RXDLY2_RX_ARDQ4_F_DLY_B1) | P_Fld(0x6d, SHU_R0_B1_RXDLY2_RX_ARDQ5_R_DLY_B1) |
            P_Fld(0x6d, SHU_R0_B1_RXDLY2_RX_ARDQ5_F_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY3, P_Fld(0x6d, SHU_R0_B1_RXDLY3_RX_ARDQ6_R_DLY_B1) |
            P_Fld(0x6d, SHU_R0_B1_RXDLY3_RX_ARDQ6_F_DLY_B1) | P_Fld(0x6d, SHU_R0_B1_RXDLY3_RX_ARDQ7_R_DLY_B1) |
            P_Fld(0x6d, SHU_R0_B1_RXDLY3_RX_ARDQ7_F_DLY_B1));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY4, P_Fld(0x6d, SHU_R0_B1_RXDLY4_RX_ARDQM0_R_DLY_B1) |
            P_Fld(0x6d, SHU_R0_B1_RXDLY4_RX_ARDQM0_F_DLY_B1));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY5, P_Fld(0x061, SHU_R0_B1_RXDLY5_RX_ARDQS0_R_DLY_B1) |
            P_Fld(0x061, SHU_R0_B1_RXDLY5_RX_ARDQS0_F_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY0+(1*DDRPHY_AO_RANK_OFFSET),
            P_Fld(0x6c, SHU_R0_B1_RXDLY0_RX_ARDQ0_F_DLY_B1) |
            P_Fld(0x6c, SHU_R0_B1_RXDLY0_RX_ARDQ1_F_DLY_B1));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x6c, SHU_R0_B1_RXDLY0_RX_ARDQ0_R_DLY_B1) |
            P_Fld(0x6c, SHU_R0_B1_RXDLY0_RX_ARDQ1_R_DLY_B1));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY1+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x6c, SHU_R0_B1_RXDLY1_RX_ARDQ2_R_DLY_B1) |
            P_Fld(0x6c, SHU_R0_B1_RXDLY1_RX_ARDQ2_F_DLY_B1) | P_Fld(0x6c, SHU_R0_B1_RXDLY1_RX_ARDQ3_R_DLY_B1) |
            P_Fld(0x6c, SHU_R0_B1_RXDLY1_RX_ARDQ3_F_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY2+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x6c, SHU_R0_B1_RXDLY2_RX_ARDQ4_R_DLY_B1) |
            P_Fld(0x6c, SHU_R0_B1_RXDLY2_RX_ARDQ4_F_DLY_B1) | P_Fld(0x6c, SHU_R0_B1_RXDLY2_RX_ARDQ5_R_DLY_B1) |
            P_Fld(0x6c, SHU_R0_B1_RXDLY2_RX_ARDQ5_F_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY3+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x6c, SHU_R0_B1_RXDLY3_RX_ARDQ6_R_DLY_B1) |
            P_Fld(0x6c, SHU_R0_B1_RXDLY3_RX_ARDQ6_F_DLY_B1) | P_Fld(0x6c, SHU_R0_B1_RXDLY3_RX_ARDQ7_R_DLY_B1) |
            P_Fld(0x6c, SHU_R0_B1_RXDLY3_RX_ARDQ7_F_DLY_B1));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY4+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x6c, SHU_R0_B1_RXDLY4_RX_ARDQM0_R_DLY_B1) |
            P_Fld(0x6c, SHU_R0_B1_RXDLY4_RX_ARDQM0_F_DLY_B1));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY5+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x060, SHU_R0_B1_RXDLY5_RX_ARDQS0_R_DLY_B1) |
            P_Fld(0x060, SHU_R0_B1_RXDLY5_RX_ARDQS0_F_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_B0_DQ9, P_Fld(0x1, B0_DQ9_RG_RX_ARDQ_STBEN_RESETB_B0) |
            P_Fld(0x0, B0_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B0) | P_Fld(0x0, B0_DQ9_RG_RX_ARDQS0_DQSIENMODE_B0) |
            P_Fld(0x1, B0_DQ9_R_DMRXDVS_R_F_DLY_RK_OPT_B0) | P_Fld(0x0, B0_DQ9_R_DMRXFIFO_STBENCMP_EN_B0) |
            P_Fld(0x00, B0_DQ9_R_IN_GATE_EN_LOW_OPT_B0) | P_Fld(0x0, B0_DQ9_R_DMDQSIEN_VALID_LAT_B0) |
            P_Fld(0x0, B0_DQ9_R_DMDQSIEN_RDSEL_LAT_B0) | P_Fld(0x0, B0_DQ9_R_DMRXDVS_VALID_LAT_B0) |
            P_Fld(0x0, B0_DQ9_R_DMRXDVS_RDSEL_LAT_B0));

    vIO32WriteFldMulti(DDRPHY_REG_B1_DQ9, P_Fld(0x1, B1_DQ9_RG_RX_ARDQ_STBEN_RESETB_B1) |
            P_Fld(0x0, B1_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B1) | P_Fld(0x0, B1_DQ9_RG_RX_ARDQS0_DQSIENMODE_B1) |
            P_Fld(0x1, B1_DQ9_R_DMRXDVS_R_F_DLY_RK_OPT_B1) | P_Fld(0x0, B1_DQ9_R_DMRXFIFO_STBENCMP_EN_B1) |
            P_Fld(0x00, B1_DQ9_R_IN_GATE_EN_LOW_OPT_B1) | P_Fld(0x0, B1_DQ9_R_DMDQSIEN_VALID_LAT_B1) |
            P_Fld(0x0, B1_DQ9_R_DMDQSIEN_RDSEL_LAT_B1) | P_Fld(0x0, B1_DQ9_R_DMRXDVS_VALID_LAT_B1) |
            P_Fld(0x0, B1_DQ9_R_DMRXDVS_RDSEL_LAT_B1));

    vIO32WriteFldMulti(DDRPHY_REG_B0_DQ4, P_Fld(0x75, B0_DQ4_RG_RX_ARDQS_EYE_R_DLY_B0) |
            P_Fld(0x75, B0_DQ4_RG_RX_ARDQS_EYE_F_DLY_B0) | P_Fld(0x2d, B0_DQ4_RG_RX_ARDQ_EYE_R_DLY_B0) |
            P_Fld(0x2d, B0_DQ4_RG_RX_ARDQ_EYE_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_B1_DQ4, P_Fld(0x75, B1_DQ4_RG_RX_ARDQS_EYE_R_DLY_B1) |
            P_Fld(0x75, B1_DQ4_RG_RX_ARDQS_EYE_F_DLY_B1) | P_Fld(0x2d, B1_DQ4_RG_RX_ARDQ_EYE_R_DLY_B1) |
            P_Fld(0x2d, B1_DQ4_RG_RX_ARDQ_EYE_F_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_B0_DQ5, P_Fld(0x0e, B0_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B0) |
            P_Fld(0x0, B0_DQ5_RG_RX_ARDQ_VREF_EN_B0) | P_Fld(0x0, B0_DQ5_RG_RX_ARDQ_EYE_VREF_EN_B0) |
            P_Fld(0x0, B0_DQ5_RG_RX_ARDQ_EYE_SEL_B0) | P_Fld(0x0, B0_DQ5_RG_RX_ARDQ_EYE_EN_B0) |
            P_Fld(0x1, B0_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B0) | P_Fld(0x0, B0_DQ5_RG_RX_ARDQS0_DVS_EN_B0));

    vIO32WriteFldMulti(DDRPHY_REG_B1_DQ5, P_Fld(0x0e, B1_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B1) |
            P_Fld(0x0, B1_DQ5_RG_RX_ARDQ_VREF_EN_B1) | P_Fld(0x0, B1_DQ5_RG_RX_ARDQ_EYE_VREF_EN_B1) |
            P_Fld(0x0, B1_DQ5_RG_RX_ARDQ_EYE_SEL_B1) | P_Fld(0x0, B1_DQ5_RG_RX_ARDQ_EYE_EN_B1) |
            P_Fld(0x1, B1_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B1) | P_Fld(0x0, B1_DQ5_RG_RX_ARDQS0_DVS_EN_B1));

}


void sv_algorithm_assistance_LP4_4266(DRAMC_CTX_T *p)
{

    vIO32WriteFldMulti(DDRPHY_REG_SHU_MISC_RDSEL_TRACK, P_Fld(0x10, SHU_MISC_RDSEL_TRACK_DMDATLAT_I) |
            P_Fld(0x1, SHU_MISC_RDSEL_TRACK_RDSEL_HWSAVE_MSK) | P_Fld(0x0, SHU_MISC_RDSEL_TRACK_RDSEL_TRACK_EN) |
            P_Fld(0xfcb, SHU_MISC_RDSEL_TRACK_SHU_GW_THRD_NEG) | P_Fld(0x035, SHU_MISC_RDSEL_TRACK_SHU_GW_THRD_POS));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RDAT, P_Fld(0x10, MISC_SHU_RDAT_DATLAT) |
            P_Fld(0x0f, MISC_SHU_RDAT_DATLAT_DSEL) | P_Fld(0x0f, MISC_SHU_RDAT_DATLAT_DSEL_PHY));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_PHY_RX_CTRL, P_Fld(0x1, MISC_SHU_PHY_RX_CTRL_RANK_RXDLY_UPDLAT_EN) |
            P_Fld(0x2, MISC_SHU_PHY_RX_CTRL_RANK_RXDLY_UPD_OFFSET) | P_Fld(0x2, MISC_SHU_PHY_RX_CTRL_RX_IN_GATE_EN_PRE_OFFSET) |
            P_Fld(0x1, MISC_SHU_PHY_RX_CTRL_RX_IN_GATE_EN_HEAD) | P_Fld(0x1, MISC_SHU_PHY_RX_CTRL_RX_IN_GATE_EN_TAIL) |
            P_Fld(0x2, MISC_SHU_PHY_RX_CTRL_RX_IN_BUFF_EN_HEAD) | P_Fld(0x0, MISC_SHU_PHY_RX_CTRL_RX_IN_BUFF_EN_TAIL));

    vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RANKCTL, P_Fld(0x4, MISC_SHU_RANKCTL_RANKINCTL_RXDLY) |
            P_Fld(0x1, MISC_SHU_RANKCTL_RANK_RXDLY_OPT) | P_Fld(0x1, MISC_SHU_RANKCTL_RANKSEL_SELPH_FRUN) |
            P_Fld(0x6, MISC_SHU_RANKCTL_RANKINCTL_STB));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RANKCTL, P_Fld(0x5, MISC_SHU_RANKCTL_RANKINCTL) |
            P_Fld(0x5, MISC_SHU_RANKCTL_RANKINCTL_ROOT1) | P_Fld(0x8, MISC_SHU_RANKCTL_RANKINCTL_PHY));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RANK_SEL_LAT, P_Fld(0x2, MISC_SHU_RANK_SEL_LAT_RANK_SEL_LAT_B0) |
            P_Fld(0x2, MISC_SHU_RANK_SEL_LAT_RANK_SEL_LAT_B1) | P_Fld(0x2, MISC_SHU_RANK_SEL_LAT_RANK_SEL_LAT_CA));

    #if !CODE_SIZE_REDUCE

    vIO32WriteFldAlign(DDRPHY_REG_MISC_SHU_RK_DQSCTL, 0x7, MISC_SHU_RK_DQSCTL_DQSINCTL);

    vIO32WriteFldAlign(DDRPHY_REG_MISC_SHU_RK_DQSCTL+(1*DDRPHY_AO_RANK_OFFSET), 0x7, MISC_SHU_RK_DQSCTL_DQSINCTL);

    vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B0_DQSIEN_MCK_UI_DLY, P_Fld(0x1, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B0) |
            P_Fld(0x5, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B0) | P_Fld(0x1, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B0) |
            P_Fld(0x1, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B0));

    vIO32WriteFldAlign(DDRPHY_REG_SHU_RK_B0_DQSIEN_PI_DLY, 0x01, SHU_RK_B0_DQSIEN_PI_DLY_DQSIEN_PI_B0);

    vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B0_DQSIEN_MCK_UI_DLY+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x9, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B0) |
            P_Fld(0xd, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B0) | P_Fld(0x1, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B0) |
            P_Fld(0x1, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B0));

    vIO32WriteFldAlign(DDRPHY_REG_SHU_RK_B0_DQSIEN_PI_DLY+(1*DDRPHY_AO_RANK_OFFSET), 0x08, SHU_RK_B0_DQSIEN_PI_DLY_DQSIEN_PI_B0);

    vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B1_DQSIEN_MCK_UI_DLY, P_Fld(0x1, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B1) |
            P_Fld(0x5, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B1) | P_Fld(0x1, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B1) |
            P_Fld(0x1, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B1));

    vIO32WriteFldAlign(DDRPHY_REG_SHU_RK_B1_DQSIEN_PI_DLY, 0x01, SHU_RK_B1_DQSIEN_PI_DLY_DQSIEN_PI_B1);

    vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B1_DQSIEN_MCK_UI_DLY+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x9, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B1) |
            P_Fld(0xd, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B1) | P_Fld(0x1, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B1) |
            P_Fld(0x1, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B1));

    vIO32WriteFldAlign(DDRPHY_REG_SHU_RK_B1_DQSIEN_PI_DLY+(1*DDRPHY_AO_RANK_OFFSET), 0x08, SHU_RK_B1_DQSIEN_PI_DLY_DQSIEN_PI_B1);
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_ODTCTRL, P_Fld(0x1, MISC_SHU_ODTCTRL_RODTEN) |
            P_Fld(0x0, MISC_SHU_ODTCTRL_RODTENSTB_SELPH_CG_IG) | P_Fld(0x0, MISC_SHU_ODTCTRL_RODTEN_SELPH_FRUN) |
            P_Fld(0x0, MISC_SHU_ODTCTRL_RODTDLY_LAT_OPT) | P_Fld(0x0, MISC_SHU_ODTCTRL_FIXRODT) |
            P_Fld(0x1, MISC_SHU_ODTCTRL_RODTEN_OPT) | P_Fld(0x1, MISC_SHU_ODTCTRL_RODTE2) |
            P_Fld(0x1, MISC_SHU_ODTCTRL_RODTE));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldAlign(DDRPHY_REG_MISC_SHU_ODTCTRL, 0x7, MISC_SHU_ODTCTRL_RODT_LAT);
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ7, P_Fld(0x0, SHU_B0_DQ7_R_DMRANKRXDVS_B0) |
            P_Fld(0x0, SHU_B0_DQ7_R_DMDQMDBI_EYE_SHU_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMDQMDBI_SHU_B0) |
            P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_DQM_FLAGSEL_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B0) |
            P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMRXTRACK_DQM_EN_B0) |
            P_Fld(0x1, SHU_B0_DQ7_R_DMRODTEN_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMARPI_CG_FB2DLL_DCM_EN_B0) |
            P_Fld(0x0, SHU_B0_DQ7_R_DMTX_ARPI_CG_DQ_NEW_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMTX_ARPI_CG_DQS_NEW_B0) |
            P_Fld(0x0, SHU_B0_DQ7_R_DMTX_ARPI_CG_DQM_NEW_B0) | P_Fld(0x0, SHU_B0_DQ7_R_LP4Y_SDN_MODE_DQS0) |
            P_Fld(0x1, SHU_B0_DQ7_R_DMRXRANK_DQ_EN_B0) | P_Fld(0x2, SHU_B0_DQ7_R_DMRXRANK_DQ_LAT_B0) |
            P_Fld(0x1, SHU_B0_DQ7_R_DMRXRANK_DQS_EN_B0) | P_Fld(0x1, SHU_B0_DQ7_R_DMRXRANK_DQS_LAT_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ7, P_Fld(0x0, SHU_B1_DQ7_R_DMRANKRXDVS_B1) |
            P_Fld(0x0, SHU_B1_DQ7_R_DMDQMDBI_EYE_SHU_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMDQMDBI_SHU_B1) |
            P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_DQM_FLAGSEL_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B1) |
            P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMRXTRACK_DQM_EN_B1) |
            P_Fld(0x1, SHU_B1_DQ7_R_DMRODTEN_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMARPI_CG_FB2DLL_DCM_EN_B1) |
            P_Fld(0x0, SHU_B1_DQ7_R_DMTX_ARPI_CG_DQ_NEW_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMTX_ARPI_CG_DQS_NEW_B1) |
            P_Fld(0x0, SHU_B1_DQ7_R_DMTX_ARPI_CG_DQM_NEW_B1) | P_Fld(0x0, SHU_B1_DQ7_R_LP4Y_SDN_MODE_DQS1) |
            P_Fld(0x1, SHU_B1_DQ7_R_DMRXRANK_DQ_EN_B1) | P_Fld(0x2, SHU_B1_DQ7_R_DMRXRANK_DQ_LAT_B1) |
            P_Fld(0x1, SHU_B1_DQ7_R_DMRXRANK_DQS_EN_B1) | P_Fld(0x1, SHU_B1_DQ7_R_DMRXRANK_DQS_LAT_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B0_RODTEN_MCK_UI_DLY, P_Fld(0x4, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B0) |
            P_Fld(0x4, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B0) | P_Fld(0x0, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B0) |
            P_Fld(0x0, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B0_RODTEN_MCK_UI_DLY+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x4, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B0) |
            P_Fld(0x4, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B0) | P_Fld(0x1, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B0) |
            P_Fld(0x0, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B1_RODTEN_MCK_UI_DLY, P_Fld(0x4, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B1) |
            P_Fld(0x4, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B1) | P_Fld(0x0, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B1) |
            P_Fld(0x0, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B1_RODTEN_MCK_UI_DLY+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x4, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B1) |
            P_Fld(0x4, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B1) | P_Fld(0x1, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B1) |
            P_Fld(0x0, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B1));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DRAMC_REG_SHU_RX_CG_SET0, P_Fld(0x0, SHU_RX_CG_SET0_DLE_LAST_EXTEND3) |
            P_Fld(0x0, SHU_RX_CG_SET0_READ_START_EXTEND3) | P_Fld(0x1, SHU_RX_CG_SET0_DLE_LAST_EXTEND2) |
            P_Fld(0x1, SHU_RX_CG_SET0_READ_START_EXTEND2) | P_Fld(0x1, SHU_RX_CG_SET0_DLE_LAST_EXTEND1) |
            P_Fld(0x1, SHU_RX_CG_SET0_READ_START_EXTEND1));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_MISC_RANK_SEL_STB, P_Fld(0x1, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_EN) |
            P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_EN_B23) | P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_SERMODE) |
            P_Fld(0x1, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_TRACK) | P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_RXDLY_TRACK) |
            P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_PHASE_EN) | P_Fld(0x6, SHU_MISC_RANK_SEL_STB_RANK_SEL_PHSINCTL) |
            P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_UI_PLUS) | P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_MCK_PLUS) |
            P_Fld(0x2, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_UI_MINUS) | P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_MCK_MINUS));

    vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RK_DQSCAL, P_Fld(0x60, MISC_SHU_RK_DQSCAL_DQSIENLLMT) |
            P_Fld(0x1, MISC_SHU_RK_DQSCAL_DQSIENLLMTEN) | P_Fld(0x3f, MISC_SHU_RK_DQSCAL_DQSIENHLMT) |
            P_Fld(0x1, MISC_SHU_RK_DQSCAL_DQSIENHLMTEN));

    vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RK_DQSCAL+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x60, MISC_SHU_RK_DQSCAL_DQSIENLLMT) |
            P_Fld(0x1, MISC_SHU_RK_DQSCAL_DQSIENLLMTEN) | P_Fld(0x3f, MISC_SHU_RK_DQSCAL_DQSIENHLMT) |
            P_Fld(0x1, MISC_SHU_RK_DQSCAL_DQSIENHLMTEN));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_INI_UIPI, P_Fld(0x01, SHU_R0_B0_INI_UIPI_CURR_INI_PI_B0) |
            P_Fld(0x11, SHU_R0_B0_INI_UIPI_CURR_INI_UI_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_INI_UIPI, P_Fld(0x01, SHU_R0_B1_INI_UIPI_CURR_INI_PI_B1) |
            P_Fld(0x11, SHU_R0_B1_INI_UIPI_CURR_INI_UI_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_INI_UIPI+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x08, SHU_R0_B0_INI_UIPI_CURR_INI_PI_B0) |
            P_Fld(0x19, SHU_R0_B0_INI_UIPI_CURR_INI_UI_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_INI_UIPI+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x08, SHU_R0_B1_INI_UIPI_CURR_INI_PI_B1) |
            P_Fld(0x19, SHU_R0_B1_INI_UIPI_CURR_INI_UI_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_NEXT_INI_UIPI, P_Fld(0x01, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_PI_B0) |
            P_Fld(0x11, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_UI_B0) | P_Fld(0x15, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_UI_P1_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_NEXT_INI_UIPI, P_Fld(0x01, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_PI_B1) |
            P_Fld(0x11, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_UI_B1) | P_Fld(0x15, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_UI_P1_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_NEXT_INI_UIPI+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x08, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_PI_B0) |
            P_Fld(0x19, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_UI_B0) | P_Fld(0x1d, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_UI_P1_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_NEXT_INI_UIPI+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x08, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_PI_B1) |
            P_Fld(0x19, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_UI_B1) | P_Fld(0x1d, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_UI_P1_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_DQ0, P_Fld(0x0, SHU_R0_B0_DQ0_RG_RX_ARDQS0_R_DLY_DUTY) |
            P_Fld(0x0, SHU_R0_B0_DQ0_RG_RX_ARDQS0_F_DLY_DUTY) |
            P_Fld(0x0, SHU_R0_B0_DQ0_DA_ARPI_DDR400_0D5UI_RK0_B0) | P_Fld(0x0, SHU_R0_B0_DQ0_DA_RX_ARDQSIEN_0D5UI_RK0_B0));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_DQ0, P_Fld(0x13, SHU_R0_B0_DQ0_SW_ARPI_DQ_B0) |
            P_Fld(0x13, SHU_R0_B0_DQ0_SW_ARPI_DQM_B0) | P_Fld(0x00, SHU_R0_B0_DQ0_ARPI_PBYTE_B0));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_DQ0, P_Fld(0x0, SHU_R0_B1_DQ0_RG_RX_ARDQS1_R_DLY_DUTY) |
            P_Fld(0x0, SHU_R0_B1_DQ0_RG_RX_ARDQS1_F_DLY_DUTY) |
            P_Fld(0x0, SHU_R0_B1_DQ0_DA_ARPI_DDR400_0D5UI_RK0_B1) | P_Fld(0x0, SHU_R0_B1_DQ0_DA_RX_ARDQSIEN_0D5UI_RK0_B1));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_DQ0, P_Fld(0x16, SHU_R0_B1_DQ0_SW_ARPI_DQ_B1) |
            P_Fld(0x16, SHU_R0_B1_DQ0_SW_ARPI_DQM_B1) | P_Fld(0x00, SHU_R0_B1_DQ0_ARPI_PBYTE_B1));
    #endif


    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_DQ0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x0, SHU_R0_B0_DQ0_RG_RX_ARDQS0_R_DLY_DUTY) |
            P_Fld(0x0, SHU_R0_B0_DQ0_RG_RX_ARDQS0_F_DLY_DUTY) |
            P_Fld(0x0, SHU_R0_B0_DQ0_DA_ARPI_DDR400_0D5UI_RK0_B0) | P_Fld(0x0, SHU_R0_B0_DQ0_DA_RX_ARDQSIEN_0D5UI_RK0_B0));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_DQ0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x2b, SHU_R0_B0_DQ0_SW_ARPI_DQ_B0) |
            P_Fld(0x2b, SHU_R0_B0_DQ0_SW_ARPI_DQM_B0) | P_Fld(0x01, SHU_R0_B0_DQ0_ARPI_PBYTE_B0));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_DQ0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x0, SHU_R0_B1_DQ0_RG_RX_ARDQS1_R_DLY_DUTY) |
            P_Fld(0x0, SHU_R0_B1_DQ0_RG_RX_ARDQS1_F_DLY_DUTY) |
            P_Fld(0x0, SHU_R0_B1_DQ0_DA_ARPI_DDR400_0D5UI_RK0_B1) | P_Fld(0x0, SHU_R0_B1_DQ0_DA_RX_ARDQSIEN_0D5UI_RK0_B1));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_DQ0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x2b, SHU_R0_B1_DQ0_SW_ARPI_DQ_B1) |
            P_Fld(0x2b, SHU_R0_B1_DQ0_SW_ARPI_DQM_B1) | P_Fld(0x01, SHU_R0_B1_DQ0_ARPI_PBYTE_B1));
    #endif

    vIO32WriteFldMulti(DRAMC_REG_SHU_DCM_CTRL0, P_Fld(0x1, SHU_DCM_CTRL0_DDRPHY_CLK_EN_OPT) |
            P_Fld(0x3, SHU_DCM_CTRL0_DPHY_CMD_CLKEN_EXTCNT) | P_Fld(0x5, SHU_DCM_CTRL0_DDRPHY_CLK_DYN_GATING_SEL) |
            P_Fld(0x2, SHU_DCM_CTRL0_APHYPI_CKCGL_CNT) | P_Fld(0x4, SHU_DCM_CTRL0_APHYPI_CKCGH_CNT) |
            P_Fld(0x0, SHU_DCM_CTRL0_FASTWAKE2) | P_Fld(0x1, SHU_DCM_CTRL0_FASTWAKE));

    vIO32WriteFldMulti(DRAMC_REG_SHU_APHY_TX_PICG_CTRL, P_Fld(0x3, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_PICG_CNT) |
            P_Fld(0x3, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQS_SEL_P1) | P_Fld(0x3, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQS_SEL_P0) |
            P_Fld(0x2, SHU_APHY_TX_PICG_CTRL_DPHY_TX_DCM_EXTCNT) | P_Fld(0x1, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_OPT));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_APHY_TX_PICG_CTRL, P_Fld(0x4, SHURK_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQ_RK_SEL_P1) |
            P_Fld(0x3, SHURK_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQ_RK_SEL_P0));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_APHY_TX_PICG_CTRL+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x4, SHURK_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQ_RK_SEL_P1) |
            P_Fld(0x3, SHURK_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQ_RK_SEL_P0));

    vIO32WriteFldMulti(DRAMC_REG_SHU_NEW_XRW2W_CTRL, P_Fld(0x2, SHU_NEW_XRW2W_CTRL_TX_PI_UPDCTL_B0) |
            P_Fld(0x2, SHU_NEW_XRW2W_CTRL_TX_PI_UPDCTL_B1) | P_Fld(0x0, SHU_NEW_XRW2W_CTRL_TXPI_UPD_MODE));

    vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_DQS0, P_Fld(0x1, SHU_SELPH_DQS0_TXDLY_DQS2) |
            P_Fld(0x1, SHU_SELPH_DQS0_TXDLY_DQS3) | P_Fld(0x1, SHU_SELPH_DQS0_TXDLY_OEN_DQS2) |
            P_Fld(0x1, SHU_SELPH_DQS0_TXDLY_OEN_DQS3));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_DQS0, P_Fld(0x4, SHU_SELPH_DQS0_TXDLY_DQS0) |
            P_Fld(0x4, SHU_SELPH_DQS0_TXDLY_DQS1) | P_Fld(0x4, SHU_SELPH_DQS0_TXDLY_OEN_DQS0) |
            P_Fld(0x4, SHU_SELPH_DQS0_TXDLY_OEN_DQS1));
    #endif

    vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_DQS1, P_Fld(0x1, SHU_SELPH_DQS1_DLY_DQS2) |
            P_Fld(0x1, SHU_SELPH_DQS1_DLY_DQS3) | P_Fld(0x1, SHU_SELPH_DQS1_DLY_OEN_DQS2) |
            P_Fld(0x1, SHU_SELPH_DQS1_DLY_OEN_DQS3));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_DQS1, P_Fld(0x5, SHU_SELPH_DQS1_DLY_DQS0) |
            P_Fld(0x5, SHU_SELPH_DQS1_DLY_DQS1) | P_Fld(0x2, SHU_SELPH_DQS1_DLY_OEN_DQS0) |
            P_Fld(0x2, SHU_SELPH_DQS1_DLY_OEN_DQS1));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ0, P_Fld(0x4, SHURK_SELPH_DQ0_TXDLY_DQ0) |
            P_Fld(0x4, SHURK_SELPH_DQ0_TXDLY_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_DQ2) |
            P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_DQ3) | P_Fld(0x4, SHURK_SELPH_DQ0_TXDLY_OEN_DQ0) |
            P_Fld(0x4, SHURK_SELPH_DQ0_TXDLY_OEN_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_OEN_DQ2) |
            P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_OEN_DQ3));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ1, P_Fld(0x4, SHURK_SELPH_DQ1_TXDLY_DQM0) |
            P_Fld(0x4, SHURK_SELPH_DQ1_TXDLY_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_DQM2) |
            P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_DQM3) | P_Fld(0x4, SHURK_SELPH_DQ1_TXDLY_OEN_DQM0) |
            P_Fld(0x4, SHURK_SELPH_DQ1_TXDLY_OEN_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_OEN_DQM2) |
            P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_OEN_DQM3));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ2, P_Fld(0x6, SHURK_SELPH_DQ2_DLY_DQ0) |
            P_Fld(0x6, SHURK_SELPH_DQ2_DLY_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ2_DLY_DQ2) |
            P_Fld(0x1, SHURK_SELPH_DQ2_DLY_DQ3) | P_Fld(0x3, SHURK_SELPH_DQ2_DLY_OEN_DQ0) |
            P_Fld(0x3, SHURK_SELPH_DQ2_DLY_OEN_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ2_DLY_OEN_DQ2) |
            P_Fld(0x1, SHURK_SELPH_DQ2_DLY_OEN_DQ3));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ3, P_Fld(0x6, SHURK_SELPH_DQ3_DLY_DQM0) |
            P_Fld(0x6, SHURK_SELPH_DQ3_DLY_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ3_DLY_DQM2) |
            P_Fld(0x1, SHURK_SELPH_DQ3_DLY_DQM3) | P_Fld(0x3, SHURK_SELPH_DQ3_DLY_OEN_DQM0) |
            P_Fld(0x3, SHURK_SELPH_DQ3_DLY_OEN_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ3_DLY_OEN_DQM2) |
            P_Fld(0x1, SHURK_SELPH_DQ3_DLY_OEN_DQM3));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ0+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x4, SHURK_SELPH_DQ0_TXDLY_DQ0) |
            P_Fld(0x4, SHURK_SELPH_DQ0_TXDLY_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_DQ2) |
            P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_DQ3) | P_Fld(0x4, SHURK_SELPH_DQ0_TXDLY_OEN_DQ0) |
            P_Fld(0x4, SHURK_SELPH_DQ0_TXDLY_OEN_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_OEN_DQ2) |
            P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_OEN_DQ3));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ1+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x4, SHURK_SELPH_DQ1_TXDLY_DQM0) |
            P_Fld(0x4, SHURK_SELPH_DQ1_TXDLY_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_DQM2) |
            P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_DQM3) | P_Fld(0x4, SHURK_SELPH_DQ1_TXDLY_OEN_DQM0) |
            P_Fld(0x4, SHURK_SELPH_DQ1_TXDLY_OEN_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_OEN_DQM2) |
            P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_OEN_DQM3));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ2+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x7, SHURK_SELPH_DQ2_DLY_DQ0) |
            P_Fld(0x7, SHURK_SELPH_DQ2_DLY_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ2_DLY_DQ2) |
            P_Fld(0x1, SHURK_SELPH_DQ2_DLY_DQ3) | P_Fld(0x4, SHURK_SELPH_DQ2_DLY_OEN_DQ0) |
            P_Fld(0x4, SHURK_SELPH_DQ2_DLY_OEN_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ2_DLY_OEN_DQ2) |
            P_Fld(0x1, SHURK_SELPH_DQ2_DLY_OEN_DQ3));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ3+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x7, SHURK_SELPH_DQ3_DLY_DQM0) |
            P_Fld(0x7, SHURK_SELPH_DQ3_DLY_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ3_DLY_DQM2) |
            P_Fld(0x1, SHURK_SELPH_DQ3_DLY_DQM3) | P_Fld(0x4, SHURK_SELPH_DQ3_DLY_OEN_DQM0) |
            P_Fld(0x4, SHURK_SELPH_DQ3_DLY_OEN_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ3_DLY_OEN_DQM2) |
            P_Fld(0x1, SHURK_SELPH_DQ3_DLY_OEN_DQM3));
    #endif

    vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL1, P_Fld(0x013, SHURK_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ0) |
            P_Fld(0x016, SHURK_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ1));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL2, P_Fld(0x013, SHURK_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ0) |
            P_Fld(0x016, SHURK_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ1));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL5, P_Fld(0x013, SHURK_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM0) |
            P_Fld(0x016, SHURK_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM1));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL1+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x02b, SHURK_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ0) |
            P_Fld(0x02b, SHURK_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ1));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL2+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x02b, SHURK_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ0) |
            P_Fld(0x02b, SHURK_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ1));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL5+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x02b, SHURK_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM0) |
            P_Fld(0x02b, SHURK_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM1));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_PI, P_Fld(0x16, SHURK_PI_RK0_ARPI_DQ_B1) |
            P_Fld(0x13, SHURK_PI_RK0_ARPI_DQ_B0) | P_Fld(0x16, SHURK_PI_RK0_ARPI_DQM_B1) |
            P_Fld(0x13, SHURK_PI_RK0_ARPI_DQM_B0));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_PI+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x2b, SHURK_PI_RK0_ARPI_DQ_B1) |
            P_Fld(0x2b, SHURK_PI_RK0_ARPI_DQ_B0) | P_Fld(0x2b, SHURK_PI_RK0_ARPI_DQM_B1) |
            P_Fld(0x2b, SHURK_PI_RK0_ARPI_DQM_B0));

    #if !CODE_SIZE_REDUCE

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_TXDLY0, P_Fld(0x10, SHU_R0_B0_TXDLY0_TX_ARDQ0_DLY_B0) |
            P_Fld(0x10, SHU_R0_B0_TXDLY0_TX_ARDQ1_DLY_B0) | P_Fld(0x10, SHU_R0_B0_TXDLY0_TX_ARDQ2_DLY_B0) |
            P_Fld(0x10, SHU_R0_B0_TXDLY0_TX_ARDQ3_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_TXDLY1, P_Fld(0x10, SHU_R0_B0_TXDLY1_TX_ARDQ4_DLY_B0) |
            P_Fld(0x10, SHU_R0_B0_TXDLY1_TX_ARDQ5_DLY_B0) | P_Fld(0x10, SHU_R0_B0_TXDLY1_TX_ARDQ6_DLY_B0) |
            P_Fld(0x10, SHU_R0_B0_TXDLY1_TX_ARDQ7_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_TXDLY3, P_Fld(0x10, SHU_R0_B0_TXDLY3_TX_ARDQM0_DLY_B0) |
            P_Fld(0x00, SHU_R0_B0_TXDLY3_TX_ARWCK_DLY_B0) | P_Fld(0x00, SHU_R0_B0_TXDLY3_TX_ARWCKB_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_TXDLY0, P_Fld(0x04, SHU_R0_B1_TXDLY0_TX_ARDQ0_DLY_B1) |
            P_Fld(0x04, SHU_R0_B1_TXDLY0_TX_ARDQ1_DLY_B1) | P_Fld(0x04, SHU_R0_B1_TXDLY0_TX_ARDQ2_DLY_B1) |
            P_Fld(0x04, SHU_R0_B1_TXDLY0_TX_ARDQ3_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_TXDLY1, P_Fld(0x04, SHU_R0_B1_TXDLY1_TX_ARDQ4_DLY_B1) |
            P_Fld(0x04, SHU_R0_B1_TXDLY1_TX_ARDQ5_DLY_B1) | P_Fld(0x04, SHU_R0_B1_TXDLY1_TX_ARDQ6_DLY_B1) |
            P_Fld(0x04, SHU_R0_B1_TXDLY1_TX_ARDQ7_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_TXDLY3, P_Fld(0x04, SHU_R0_B1_TXDLY3_TX_ARDQM0_DLY_B1) |
            P_Fld(0x00, SHU_R0_B1_TXDLY3_TX_ARWCK_DLY_B1) | P_Fld(0x00, SHU_R0_B1_TXDLY3_TX_ARWCKB_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_TXDLY0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x08, SHU_R0_B0_TXDLY0_TX_ARDQ0_DLY_B0) |
            P_Fld(0x08, SHU_R0_B0_TXDLY0_TX_ARDQ1_DLY_B0) | P_Fld(0x08, SHU_R0_B0_TXDLY0_TX_ARDQ2_DLY_B0) |
            P_Fld(0x08, SHU_R0_B0_TXDLY0_TX_ARDQ3_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_TXDLY1+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x08, SHU_R0_B0_TXDLY1_TX_ARDQ4_DLY_B0) |
            P_Fld(0x08, SHU_R0_B0_TXDLY1_TX_ARDQ5_DLY_B0) | P_Fld(0x08, SHU_R0_B0_TXDLY1_TX_ARDQ6_DLY_B0) |
            P_Fld(0x08, SHU_R0_B0_TXDLY1_TX_ARDQ7_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_TXDLY3+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x08, SHU_R0_B0_TXDLY3_TX_ARDQM0_DLY_B0) |
            P_Fld(0x00, SHU_R0_B0_TXDLY3_TX_ARWCK_DLY_B0) | P_Fld(0x00, SHU_R0_B0_TXDLY3_TX_ARWCKB_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_TXDLY0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x08, SHU_R0_B1_TXDLY0_TX_ARDQ0_DLY_B1) |
            P_Fld(0x08, SHU_R0_B1_TXDLY0_TX_ARDQ1_DLY_B1) | P_Fld(0x08, SHU_R0_B1_TXDLY0_TX_ARDQ2_DLY_B1) |
            P_Fld(0x08, SHU_R0_B1_TXDLY0_TX_ARDQ3_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_TXDLY1+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x08, SHU_R0_B1_TXDLY1_TX_ARDQ4_DLY_B1) |
            P_Fld(0x08, SHU_R0_B1_TXDLY1_TX_ARDQ5_DLY_B1) | P_Fld(0x08, SHU_R0_B1_TXDLY1_TX_ARDQ6_DLY_B1) |
            P_Fld(0x08, SHU_R0_B1_TXDLY1_TX_ARDQ7_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_TXDLY3+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x08, SHU_R0_B1_TXDLY3_TX_ARDQM0_DLY_B1) |
            P_Fld(0x00, SHU_R0_B1_TXDLY3_TX_ARWCK_DLY_B1) | P_Fld(0x00, SHU_R0_B1_TXDLY3_TX_ARWCKB_DLY_B1));

    vIO32WriteFldMulti(DRAMC_REG_SHU_TX_RANKCTL, P_Fld(0x1, SHU_TX_RANKCTL_TXRANKINCTL_TXDLY) |
            P_Fld(0x1, SHU_TX_RANKCTL_TXRANKINCTL) | P_Fld(0x0, SHU_TX_RANKCTL_TXRANKINCTL_ROOT));
    #endif

    #if !(CODE_SIZE_REDUCE && AC_TIMING_DERATE_ENABLE)
    vIO32WriteFldMulti(DRAMC_REG_SHU_AC_DERATING0, P_Fld(0x0, SHU_AC_DERATING0_ACDERATEEN) |
            P_Fld(0x5, SHU_AC_DERATING0_TRRD_DERATE) | P_Fld(0xb, SHU_AC_DERATING0_TRCD_DERATE));

    vIO32WriteFldMulti(DRAMC_REG_SHU_AC_DERATING1, P_Fld(0xb, SHU_AC_DERATING1_TRPAB_DERATE) |
            P_Fld(0x9, SHU_AC_DERATING1_TRP_DERATE) | P_Fld(0x0f, SHU_AC_DERATING1_TRAS_DERATE) |
            P_Fld(0x00, SHU_AC_DERATING1_TRC_DERATE));

    vIO32WriteFldMulti(DRAMC_REG_SHU_AC_DERATING_05T, P_Fld(0x0, SHU_AC_DERATING_05T_TRC_05T_DERATE) |
            P_Fld(0x0, SHU_AC_DERATING_05T_TRCD_05T_DERATE) | P_Fld(0x1, SHU_AC_DERATING_05T_TRP_05T_DERATE) |
            P_Fld(0x0, SHU_AC_DERATING_05T_TRPAB_05T_DERATE) | P_Fld(0x0, SHU_AC_DERATING_05T_TRAS_05T_DERATE) |
            P_Fld(0x1, SHU_AC_DERATING_05T_TRRD_05T_DERATE));
    #endif

    vIO32WriteFldMulti(DRAMC_REG_SHU_SREF_CTRL, P_Fld(0x3, SHU_SREF_CTRL_CKEHCMD) |
            P_Fld(0x3, SHU_SREF_CTRL_SREF_CK_DLY));

    vIO32WriteFldMulti(DRAMC_REG_SHU_HMR4_DVFS_CTRL0, P_Fld(0x86, SHU_HMR4_DVFS_CTRL0_FSPCHG_PRDCNT) |
            P_Fld(0x000, SHU_HMR4_DVFS_CTRL0_REFRCNT));

    vIO32WriteFldMulti(DRAMC_REG_SHU_AC_TIME_05T,
            P_Fld(0x0, SHU_AC_TIME_05T_TCKEPRD_05T) | P_Fld(0x0, SHU_AC_TIME_05T_BGTRRD_05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_BGTCCD_05T) | P_Fld(0x0, SHU_AC_TIME_05T_BGTWTR_M05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_XRTR2W_05T) | P_Fld(0x0, SHU_AC_TIME_05T_XRTW2R_M05T));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DRAMC_REG_SHU_AC_TIME_05T, P_Fld(0x0, SHU_AC_TIME_05T_TRC_05T) |
            P_Fld(0x1, SHU_AC_TIME_05T_TRFCPB_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TRFC_05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_TPBR2PBR_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TXP_05T) |
            P_Fld(0x1, SHU_AC_TIME_05T_TRTP_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TRCD_05T) |
            P_Fld(0x1, SHU_AC_TIME_05T_TRP_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TRPAB_05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_TRAS_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TWR_M05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_TRRD_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TFAW_05T) |
            P_Fld(0x1, SHU_AC_TIME_05T_TR2PD_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TWTPD_M05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_TMRRI_05T) | P_Fld(0x1, SHU_AC_TIME_05T_TMRWCKEL_05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_TR2W_05T) | P_Fld(0x1, SHU_AC_TIME_05T_TWTR_M05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_TMRD_05T) | P_Fld(0x1, SHU_AC_TIME_05T_TMRW_05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_TMRR2MRW_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TW2MRW_05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_TR2MRW_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TPBR2ACT_05T));

    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM_XRT, P_Fld(0x03, SHU_ACTIM_XRT_XRTR2R) |
            P_Fld(0x08, SHU_ACTIM_XRT_XRTR2W) | P_Fld(0x1, SHU_ACTIM_XRT_XRTW2R) |
            P_Fld(0x05, SHU_ACTIM_XRT_XRTW2W));

    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM0, P_Fld(0x0a, SHU_ACTIM0_TWTR) |
            P_Fld(0x0f, SHU_ACTIM0_TWR) | P_Fld(0x3, SHU_ACTIM0_TRRD) |
            P_Fld(0xa, SHU_ACTIM0_TRCD) | P_Fld(0x3, SHU_ACTIM0_CKELCKCNT));

    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM1, P_Fld(0xa, SHU_ACTIM1_TRPAB) |
            P_Fld(0x8, SHU_ACTIM1_TMRWCKEL) | P_Fld(0x8, SHU_ACTIM1_TRP) |
            P_Fld(0x0e, SHU_ACTIM1_TRAS) | P_Fld(0x00, SHU_ACTIM1_TRC));

    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM2, P_Fld(0x1, SHU_ACTIM2_TXP) |
            P_Fld(0x0e, SHU_ACTIM2_TMRRI) | P_Fld(0x2, SHU_ACTIM2_TRTP) |
            P_Fld(0x09, SHU_ACTIM2_TR2W) | P_Fld(0x08, SHU_ACTIM2_TFAW));

    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM3, P_Fld(0x59, SHU_ACTIM3_TRFCPB) |
            P_Fld(0x4, SHU_ACTIM3_TR2MRR) | P_Fld(0xbf, SHU_ACTIM3_TRFC));
    #endif
		vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM3, P_Fld(0x4, SHU_ACTIM3_MANTMRR) |
            P_Fld(0x00, SHU_ACTIM3_TWTR_L));

    #if !CODE_SIZE_REDUCE

    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM4, P_Fld(0x0cf, SHU_ACTIM4_TXREFCNT) |
            P_Fld(0x0f, SHU_ACTIM4_TMRR2MRW) | P_Fld(0x0b, SHU_ACTIM4_TMRR2W) |
            P_Fld(0x2e, SHU_ACTIM4_TZQCS));

    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM5, P_Fld(0x0f, SHU_ACTIM5_TR2PD) |
            P_Fld(0x12, SHU_ACTIM5_TWTPD) | P_Fld(0x29, SHU_ACTIM5_TPBR2PBR) |
            P_Fld(0x0, SHU_ACTIM5_TPBR2ACT));

    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM6, P_Fld(0x10, SHU_ACTIM6_TZQLAT2) |
            P_Fld(0x8, SHU_ACTIM6_TMRD) | P_Fld(0x5, SHU_ACTIM6_TMRW) |
            P_Fld(0x0b, SHU_ACTIM6_TW2MRW) | P_Fld(0x12, SHU_ACTIM6_TR2MRW));

    vIO32WriteFldMulti(DRAMC_REG_SHU_CKECTRL, P_Fld(0x1, SHU_CKECTRL_TPDE_05T) |
            P_Fld(0x0, SHU_CKECTRL_TPDX_05T) | P_Fld(0x1, SHU_CKECTRL_TPDE) |
            P_Fld(0x1, SHU_CKECTRL_TPDX) | P_Fld(0x3, SHU_CKECTRL_TCKEPRD));
    #endif

    vIO32WriteFldAlign(DRAMC_REG_SHU_CKECTRL, 0x3, SHU_CKECTRL_TCKESRX);

    vIO32WriteFldMulti(DRAMC_REG_SHU_MISC, P_Fld(0x7, SHU_MISC_DCMDLYREF) |
    P_Fld(0x0, SHU_MISC_DAREFEN));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldAlign(DRAMC_REG_SHU_MISC, 0x2, SHU_MISC_REQQUE_MAXCNT);

    vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ8, P_Fld(0x0, SHU_B0_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B0) |
            P_Fld(0x0, SHU_B0_DQ8_R_RMRODTEN_CG_IG_B0) | P_Fld(0x1, SHU_B0_DQ8_R_RMRX_TOPHY_CG_IG_B0) |
            P_Fld(0x0, SHU_B0_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B0) |
            P_Fld(0x1, SHU_B0_DQ8_R_DMRXDLY_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B0) |
            P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B0) |
            P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMRANK_PIPE_CG_IG_B0) |
            P_Fld(0x0, SHU_B0_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B0));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ8, P_Fld(0x0100, SHU_B0_DQ8_R_DMRXDVS_UPD_FORCE_CYC_B0) |
            P_Fld(0x0, SHU_B0_DQ8_R_DMRXDVS_UPD_FORCE_EN_B0));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ8, P_Fld(0x0, SHU_B1_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B1) |
            P_Fld(0x0, SHU_B1_DQ8_R_RMRODTEN_CG_IG_B1) | P_Fld(0x1, SHU_B1_DQ8_R_RMRX_TOPHY_CG_IG_B1) |
            P_Fld(0x0, SHU_B1_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B1) |
            P_Fld(0x1, SHU_B1_DQ8_R_DMRXDLY_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B1) |
            P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B1) |
            P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMRANK_PIPE_CG_IG_B1) |
            P_Fld(0x0, SHU_B1_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B1));
    #endif
    vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ8, P_Fld(0x0100, SHU_B1_DQ8_R_DMRXDVS_UPD_FORCE_CYC_B1) |
            P_Fld(0x0, SHU_B1_DQ8_R_DMRXDVS_UPD_FORCE_EN_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ5, P_Fld(0x0e, SHU_B0_DQ5_RG_RX_ARDQ_VREF_SEL_B0) |
            P_Fld(0x0, SHU_B0_DQ5_RG_RX_ARDQ_VREF_BYPASS_B0) | P_Fld(0x00, SHU_B0_DQ5_RG_ARPI_FB_B0) |
            P_Fld(0x0, SHU_B0_DQ5_RG_RX_ARDQS0_DQSIEN_DLY_B0) | P_Fld(0x0, SHU_B0_DQ5_RG_RX_ARDQS_DQSIEN_RB_DLY_B0) |
            P_Fld(0x3, SHU_B0_DQ5_RG_RX_ARDQS0_DVS_DLY_B0) | P_Fld(0x0, SHU_B0_DQ5_RG_RX_ARDQ_FIFO_DQSI_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ5, P_Fld(0x0e, SHU_B1_DQ5_RG_RX_ARDQ_VREF_SEL_B1) |
            P_Fld(0x0, SHU_B1_DQ5_RG_RX_ARDQ_VREF_BYPASS_B1) | P_Fld(0x00, SHU_B1_DQ5_RG_ARPI_FB_B1) |
            P_Fld(0x0, SHU_B1_DQ5_RG_RX_ARDQS0_DQSIEN_DLY_B1) | P_Fld(0x0, SHU_B1_DQ5_RG_RX_ARDQS_DQSIEN_RB_DLY_B1) |
            P_Fld(0x3, SHU_B1_DQ5_RG_RX_ARDQS0_DVS_DLY_B1) | P_Fld(0x0, SHU_B1_DQ5_RG_RX_ARDQ_FIFO_DQSI_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY0, P_Fld(0x54, SHU_R0_B0_RXDLY0_RX_ARDQ0_F_DLY_B0) |
            P_Fld(0x54, SHU_R0_B0_RXDLY0_RX_ARDQ1_F_DLY_B0));
	
    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY0, P_Fld(0x54, SHU_R0_B0_RXDLY0_RX_ARDQ0_R_DLY_B0) |
            P_Fld(0x54, SHU_R0_B0_RXDLY0_RX_ARDQ1_R_DLY_B0));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY1, P_Fld(0x54, SHU_R0_B0_RXDLY1_RX_ARDQ2_R_DLY_B0) |
            P_Fld(0x54, SHU_R0_B0_RXDLY1_RX_ARDQ2_F_DLY_B0) | P_Fld(0x54, SHU_R0_B0_RXDLY1_RX_ARDQ3_R_DLY_B0) |
            P_Fld(0x54, SHU_R0_B0_RXDLY1_RX_ARDQ3_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY2, P_Fld(0x54, SHU_R0_B0_RXDLY2_RX_ARDQ4_R_DLY_B0) |
            P_Fld(0x54, SHU_R0_B0_RXDLY2_RX_ARDQ4_F_DLY_B0) | P_Fld(0x54, SHU_R0_B0_RXDLY2_RX_ARDQ5_R_DLY_B0) |
            P_Fld(0x54, SHU_R0_B0_RXDLY2_RX_ARDQ5_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY3, P_Fld(0x54, SHU_R0_B0_RXDLY3_RX_ARDQ6_R_DLY_B0) |
            P_Fld(0x54, SHU_R0_B0_RXDLY3_RX_ARDQ6_F_DLY_B0) | P_Fld(0x54, SHU_R0_B0_RXDLY3_RX_ARDQ7_R_DLY_B0) |
            P_Fld(0x54, SHU_R0_B0_RXDLY3_RX_ARDQ7_F_DLY_B0));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY4, P_Fld(0x54, SHU_R0_B0_RXDLY4_RX_ARDQM0_R_DLY_B0) |
            P_Fld(0x54, SHU_R0_B0_RXDLY4_RX_ARDQM0_F_DLY_B0));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY5, P_Fld(0x04a, SHU_R0_B0_RXDLY5_RX_ARDQS0_R_DLY_B0) |
            P_Fld(0x04a, SHU_R0_B0_RXDLY5_RX_ARDQS0_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x46, SHU_R0_B0_RXDLY0_RX_ARDQ0_F_DLY_B0) |
            P_Fld(0x46, SHU_R0_B0_RXDLY0_RX_ARDQ1_F_DLY_B0));

	#if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x46, SHU_R0_B0_RXDLY0_RX_ARDQ0_R_DLY_B0) |
            P_Fld(0x46, SHU_R0_B0_RXDLY0_RX_ARDQ1_R_DLY_B0));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY1+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x46, SHU_R0_B0_RXDLY1_RX_ARDQ2_R_DLY_B0) |
            P_Fld(0x46, SHU_R0_B0_RXDLY1_RX_ARDQ2_F_DLY_B0) | P_Fld(0x46, SHU_R0_B0_RXDLY1_RX_ARDQ3_R_DLY_B0) |
            P_Fld(0x46, SHU_R0_B0_RXDLY1_RX_ARDQ3_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY2+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x46, SHU_R0_B0_RXDLY2_RX_ARDQ4_R_DLY_B0) |
            P_Fld(0x46, SHU_R0_B0_RXDLY2_RX_ARDQ4_F_DLY_B0) | P_Fld(0x46, SHU_R0_B0_RXDLY2_RX_ARDQ5_R_DLY_B0) |
            P_Fld(0x46, SHU_R0_B0_RXDLY2_RX_ARDQ5_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY3+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x46, SHU_R0_B0_RXDLY3_RX_ARDQ6_R_DLY_B0) |
            P_Fld(0x46, SHU_R0_B0_RXDLY3_RX_ARDQ6_F_DLY_B0) | P_Fld(0x46, SHU_R0_B0_RXDLY3_RX_ARDQ7_R_DLY_B0) |
            P_Fld(0x46, SHU_R0_B0_RXDLY3_RX_ARDQ7_F_DLY_B0));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY4+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x46, SHU_R0_B0_RXDLY4_RX_ARDQM0_R_DLY_B0) |
            P_Fld(0x46, SHU_R0_B0_RXDLY4_RX_ARDQM0_F_DLY_B0));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY5+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x038, SHU_R0_B0_RXDLY5_RX_ARDQS0_R_DLY_B0) |
            P_Fld(0x038, SHU_R0_B0_RXDLY5_RX_ARDQS0_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY0, P_Fld(0xcd, SHU_R0_B1_RXDLY0_RX_ARDQ0_F_DLY_B1) |
            P_Fld(0xcd, SHU_R0_B1_RXDLY0_RX_ARDQ1_F_DLY_B1));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY0, P_Fld(0xcd, SHU_R0_B1_RXDLY0_RX_ARDQ0_R_DLY_B1) |
            P_Fld(0xcd, SHU_R0_B1_RXDLY0_RX_ARDQ1_R_DLY_B1));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY1, P_Fld(0xcd, SHU_R0_B1_RXDLY1_RX_ARDQ2_R_DLY_B1) |
            P_Fld(0xcd, SHU_R0_B1_RXDLY1_RX_ARDQ2_F_DLY_B1) | P_Fld(0xcd, SHU_R0_B1_RXDLY1_RX_ARDQ3_R_DLY_B1) |
            P_Fld(0xcd, SHU_R0_B1_RXDLY1_RX_ARDQ3_F_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY2, P_Fld(0xcd, SHU_R0_B1_RXDLY2_RX_ARDQ4_R_DLY_B1) |
            P_Fld(0xcd, SHU_R0_B1_RXDLY2_RX_ARDQ4_F_DLY_B1) | P_Fld(0xcd, SHU_R0_B1_RXDLY2_RX_ARDQ5_R_DLY_B1) |
            P_Fld(0xcd, SHU_R0_B1_RXDLY2_RX_ARDQ5_F_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY3, P_Fld(0xcd, SHU_R0_B1_RXDLY3_RX_ARDQ6_R_DLY_B1) |
            P_Fld(0xcd, SHU_R0_B1_RXDLY3_RX_ARDQ6_F_DLY_B1) | P_Fld(0xcd, SHU_R0_B1_RXDLY3_RX_ARDQ7_R_DLY_B1) |
            P_Fld(0xcd, SHU_R0_B1_RXDLY3_RX_ARDQ7_F_DLY_B1));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY4, P_Fld(0xcd, SHU_R0_B1_RXDLY4_RX_ARDQM0_R_DLY_B1) |
            P_Fld(0xcd, SHU_R0_B1_RXDLY4_RX_ARDQM0_F_DLY_B1));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY5, P_Fld(0x0bd, SHU_R0_B1_RXDLY5_RX_ARDQS0_R_DLY_B1) |
            P_Fld(0x0bd, SHU_R0_B1_RXDLY5_RX_ARDQS0_F_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0xfe, SHU_R0_B1_RXDLY0_RX_ARDQ0_F_DLY_B1) |
            P_Fld(0xfe, SHU_R0_B1_RXDLY0_RX_ARDQ1_F_DLY_B1));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0xfe, SHU_R0_B1_RXDLY0_RX_ARDQ0_R_DLY_B1) |
            P_Fld(0xfe, SHU_R0_B1_RXDLY0_RX_ARDQ1_R_DLY_B1));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY1+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0xfe, SHU_R0_B1_RXDLY1_RX_ARDQ2_R_DLY_B1) |
            P_Fld(0xfe, SHU_R0_B1_RXDLY1_RX_ARDQ2_F_DLY_B1) | P_Fld(0xfe, SHU_R0_B1_RXDLY1_RX_ARDQ3_R_DLY_B1) |
            P_Fld(0xfe, SHU_R0_B1_RXDLY1_RX_ARDQ3_F_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY2+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0xfe, SHU_R0_B1_RXDLY2_RX_ARDQ4_R_DLY_B1) |
            P_Fld(0xfe, SHU_R0_B1_RXDLY2_RX_ARDQ4_F_DLY_B1) | P_Fld(0xfe, SHU_R0_B1_RXDLY2_RX_ARDQ5_R_DLY_B1) |
            P_Fld(0xfe, SHU_R0_B1_RXDLY2_RX_ARDQ5_F_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY3+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0xfe, SHU_R0_B1_RXDLY3_RX_ARDQ6_R_DLY_B1) |
            P_Fld(0xfe, SHU_R0_B1_RXDLY3_RX_ARDQ6_F_DLY_B1) | P_Fld(0xfe, SHU_R0_B1_RXDLY3_RX_ARDQ7_R_DLY_B1) |
            P_Fld(0xfe, SHU_R0_B1_RXDLY3_RX_ARDQ7_F_DLY_B1));

    #if !CODE_SIZE_REDUCE
    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY4+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0xfe, SHU_R0_B1_RXDLY4_RX_ARDQM0_R_DLY_B1) |
            P_Fld(0xfe, SHU_R0_B1_RXDLY4_RX_ARDQM0_F_DLY_B1));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY5+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x0f4, SHU_R0_B1_RXDLY5_RX_ARDQS0_R_DLY_B1) |
            P_Fld(0x0f4, SHU_R0_B1_RXDLY5_RX_ARDQS0_F_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_B0_DQ9, P_Fld(0x1, B0_DQ9_RG_RX_ARDQ_STBEN_RESETB_B0) |
            P_Fld(0x0, B0_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B0) | P_Fld(0x0, B0_DQ9_RG_RX_ARDQS0_DQSIENMODE_B0) |
            P_Fld(0x1, B0_DQ9_R_DMRXDVS_R_F_DLY_RK_OPT_B0) | P_Fld(0x0, B0_DQ9_R_DMRXFIFO_STBENCMP_EN_B0) |
            P_Fld(0x00, B0_DQ9_R_IN_GATE_EN_LOW_OPT_B0) | P_Fld(0x0, B0_DQ9_R_DMDQSIEN_VALID_LAT_B0) |
            P_Fld(0x0, B0_DQ9_R_DMDQSIEN_RDSEL_LAT_B0) | P_Fld(0x0, B0_DQ9_R_DMRXDVS_VALID_LAT_B0) |
            P_Fld(0x0, B0_DQ9_R_DMRXDVS_RDSEL_LAT_B0));

    vIO32WriteFldMulti(DDRPHY_REG_B1_DQ9, P_Fld(0x1, B1_DQ9_RG_RX_ARDQ_STBEN_RESETB_B1) |
            P_Fld(0x0, B1_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B1) | P_Fld(0x0, B1_DQ9_RG_RX_ARDQS0_DQSIENMODE_B1) |
            P_Fld(0x1, B1_DQ9_R_DMRXDVS_R_F_DLY_RK_OPT_B1) | P_Fld(0x0, B1_DQ9_R_DMRXFIFO_STBENCMP_EN_B1) |
            P_Fld(0x00, B1_DQ9_R_IN_GATE_EN_LOW_OPT_B1) | P_Fld(0x0, B1_DQ9_R_DMDQSIEN_VALID_LAT_B1) |
            P_Fld(0x0, B1_DQ9_R_DMDQSIEN_RDSEL_LAT_B1) | P_Fld(0x0, B1_DQ9_R_DMRXDVS_VALID_LAT_B1) |
            P_Fld(0x0, B1_DQ9_R_DMRXDVS_RDSEL_LAT_B1));

    vIO32WriteFldMulti(DDRPHY_REG_B0_DQ4, P_Fld(0x5a, B0_DQ4_RG_RX_ARDQS_EYE_R_DLY_B0) |
            P_Fld(0x5a, B0_DQ4_RG_RX_ARDQS_EYE_F_DLY_B0) | P_Fld(0x14, B0_DQ4_RG_RX_ARDQ_EYE_R_DLY_B0) |
            P_Fld(0x14, B0_DQ4_RG_RX_ARDQ_EYE_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_B1_DQ4, P_Fld(0x53, B1_DQ4_RG_RX_ARDQS_EYE_R_DLY_B1) |
            P_Fld(0x53, B1_DQ4_RG_RX_ARDQS_EYE_F_DLY_B1) | P_Fld(0x0d, B1_DQ4_RG_RX_ARDQ_EYE_R_DLY_B1) |
            P_Fld(0x0d, B1_DQ4_RG_RX_ARDQ_EYE_F_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_B0_DQ5, P_Fld(0x0e, B0_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B0) |
            P_Fld(0x0, B0_DQ5_RG_RX_ARDQ_VREF_EN_B0) | P_Fld(0x0, B0_DQ5_RG_RX_ARDQ_EYE_VREF_EN_B0) |
            P_Fld(0x0, B0_DQ5_RG_RX_ARDQ_EYE_SEL_B0) | P_Fld(0x0, B0_DQ5_RG_RX_ARDQ_EYE_EN_B0) |
            P_Fld(0x1, B0_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B0) | P_Fld(0x0, B0_DQ5_RG_RX_ARDQS0_DVS_EN_B0));

    vIO32WriteFldMulti(DDRPHY_REG_B1_DQ5, P_Fld(0x0e, B1_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B1) |
            P_Fld(0x0, B1_DQ5_RG_RX_ARDQ_VREF_EN_B1) | P_Fld(0x0, B1_DQ5_RG_RX_ARDQ_EYE_VREF_EN_B1) |
            P_Fld(0x0, B1_DQ5_RG_RX_ARDQ_EYE_SEL_B1) | P_Fld(0x0, B1_DQ5_RG_RX_ARDQ_EYE_EN_B1) |
            P_Fld(0x1, B1_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B1) | P_Fld(0x0, B1_DQ5_RG_RX_ARDQS0_DVS_EN_B1));

}


void TX_Path_Algorithm(DRAMC_CTX_T *p)
{

    U8 u1RandIdx = 0;
    U8 WL = 0;
    U8 u1DQS_TotalUI = 0;
    U8 u1DQS_OE_TotalUI = 0;
    U8 u1DQS_MCK, u1DQS_UI;
    U8 u1DQS_OE_MCK, u1DQS_OE_UI;
    U8 u1Small_ui_to_large;
    U8 u1TxDQOEShift = 0;

    u1Small_ui_to_large = u1MCK2UI_DivShift(p);
    WL = Get_WL_by_MR_LP4(0, (u1MR02Value[p->dram_fsp]&0x3F)>>3);
    u1DQS_TotalUI = WL*DFS_TOP[0].CKR*2 + 1;
    #if ENABLE_WDQS_MODE_2
    u1TxDQOEShift = WDQSMode2AcTxOEShift(p);
    #else
    u1TxDQOEShift = TX_DQ_OE_SHIFT_LP4;
    #endif
    u1DQS_OE_TotalUI = u1DQS_TotalUI - u1TxDQOEShift;

    u1DQS_UI = u1DQS_TotalUI - ((u1DQS_TotalUI >> u1Small_ui_to_large) << u1Small_ui_to_large);
    u1DQS_MCK = (u1DQS_TotalUI >> u1Small_ui_to_large);

    u1DQS_OE_UI = u1DQS_OE_TotalUI - ((u1DQS_OE_TotalUI >> u1Small_ui_to_large) << u1Small_ui_to_large);
    u1DQS_OE_MCK = (u1DQS_OE_TotalUI >> u1Small_ui_to_large);



    mcSHOW_DBG_MSG3(("[TX_path_calculate] data rate=%d, WL=%d, DQS_TotalUI=%d\n", DFS_TOP[0].data_rate, WL, u1DQS_TotalUI));
    mcSHOW_DBG_MSG3(("[TX_path_calculate] DQS = (%d,%d) DQS_OE = (%d,%d)\n", u1DQS_MCK, u1DQS_UI, u1DQS_OE_MCK, u1DQS_OE_UI));

    for(u1RandIdx = 0; u1RandIdx < p->support_rank_num; u1RandIdx ++)
    {
        vIO32WriteFldMulti_All(DRAMC_REG_SHU_SELPH_DQS0, P_Fld(u1DQS_MCK, SHU_SELPH_DQS0_TXDLY_DQS0)
                                                | P_Fld(u1DQS_MCK, SHU_SELPH_DQS0_TXDLY_DQS1)
                                                | P_Fld(u1DQS_OE_MCK, SHU_SELPH_DQS0_TXDLY_OEN_DQS0)
                                                | P_Fld(u1DQS_OE_MCK, SHU_SELPH_DQS0_TXDLY_OEN_DQS1));

        vIO32WriteFldMulti_All(DRAMC_REG_SHU_SELPH_DQS1, P_Fld(u1DQS_UI, SHU_SELPH_DQS1_DLY_DQS0)
                                                | P_Fld(u1DQS_UI, SHU_SELPH_DQS1_DLY_DQS1)
                                                | P_Fld(u1DQS_OE_UI, SHU_SELPH_DQS1_DLY_OEN_DQS0)
                                                | P_Fld(u1DQS_OE_UI, SHU_SELPH_DQS1_DLY_OEN_DQS1));    
    }
}


void CInit_golden_mini_freq_related_vseq_LP4_1600(DRAMC_CTX_T *p)
{

    vIO32WriteFldMulti(DDRPHY_REG_SHU_MISC_DRVING1, P_Fld(0x09, SHU_MISC_DRVING1_DQDRVN2) |
            P_Fld(0x07, SHU_MISC_DRVING1_DQDRVP2) | P_Fld(0x09, SHU_MISC_DRVING1_DQSDRVN1) |
            P_Fld(0x07, SHU_MISC_DRVING1_DQSDRVP1) | P_Fld(0x09, SHU_MISC_DRVING1_DQSDRVN2) |
            P_Fld(0x07, SHU_MISC_DRVING1_DQSDRVP2) | P_Fld(0x1, SHU_MISC_DRVING1_DIS_IMP_ODTN_TRACK) |
            P_Fld(0x1, SHU_MISC_DRVING1_DIS_IMPCAL_HW));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_MISC_DRVING2, P_Fld(0xF, SHU_MISC_DRVING2_CMDDRVN1) |
            P_Fld(0x07, SHU_MISC_DRVING2_CMDDRVP1) | P_Fld(0xF, SHU_MISC_DRVING2_CMDDRVN2) |
            P_Fld(0x07, SHU_MISC_DRVING2_CMDDRVP2) | P_Fld(0x09, SHU_MISC_DRVING2_DQDRVN1) |
            P_Fld(0x07, SHU_MISC_DRVING2_DQDRVP1) | P_Fld(0x1, SHU_MISC_DRVING2_DIS_IMPCAL_ODT_EN));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_MISC_DRVING3, P_Fld(0x0a, SHU_MISC_DRVING3_DQODTN2) |
            P_Fld(0x0a, SHU_MISC_DRVING3_DQODTP2) | P_Fld(0x0a, SHU_MISC_DRVING3_DQSODTN) |
            P_Fld(0x0a, SHU_MISC_DRVING3_DQSODTP) | P_Fld(0x0a, SHU_MISC_DRVING3_DQSODTN2) |
            P_Fld(0x0a, SHU_MISC_DRVING3_DQSODTP2));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_MISC_DRVING4, P_Fld(0x0a, SHU_MISC_DRVING4_CMDODTN1) |
            P_Fld(0x0a, SHU_MISC_DRVING4_CMDODTP1) | P_Fld(0x0a, SHU_MISC_DRVING4_CMDODTN2) |
            P_Fld(0x0a, SHU_MISC_DRVING4_CMDODTP2) | P_Fld(0x0a, SHU_MISC_DRVING4_DQODTN1) |
            P_Fld(0x0a, SHU_MISC_DRVING4_DQODTP1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_MISC_DRVING6, P_Fld(0x07, SHU_MISC_DRVING6_IMP_TXDLY_CMD) |
            P_Fld(0x00, SHU_MISC_DRVING6_DQCODTN1) | P_Fld(0x00, SHU_MISC_DRVING6_DQCODTP1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_MISC_IMPCAL1, P_Fld(0x3, SHU_MISC_IMPCAL1_IMPCAL_CHKCYCLE) |
            P_Fld(0x00, SHU_MISC_IMPCAL1_IMPDRVP) | P_Fld(0x00, SHU_MISC_IMPCAL1_IMPDRVN) |
            P_Fld(0x4, SHU_MISC_IMPCAL1_IMPCAL_CALEN_CYCLE) | P_Fld(0x03, SHU_MISC_IMPCAL1_IMPCALCNT) |
            P_Fld(0x8, SHU_MISC_IMPCAL1_IMPCAL_CALICNT));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_MISC_RDSEL_TRACK, P_Fld(0x0a, SHU_MISC_RDSEL_TRACK_DMDATLAT_I) |
            P_Fld(0x1, SHU_MISC_RDSEL_TRACK_RDSEL_HWSAVE_MSK) | P_Fld(0x0, SHU_MISC_RDSEL_TRACK_RDSEL_TRACK_EN) |
            P_Fld(0xfeb, SHU_MISC_RDSEL_TRACK_SHU_GW_THRD_NEG) | P_Fld(0x015, SHU_MISC_RDSEL_TRACK_SHU_GW_THRD_POS));

    vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RDAT, P_Fld(0x0a, MISC_SHU_RDAT_DATLAT) |
            P_Fld(0x09, MISC_SHU_RDAT_DATLAT_DSEL) | P_Fld(0x09, MISC_SHU_RDAT_DATLAT_DSEL_PHY));

    vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_PHY_RX_CTRL, P_Fld(0x1, MISC_SHU_PHY_RX_CTRL_RANK_RXDLY_UPDLAT_EN) |
            P_Fld(0x2, MISC_SHU_PHY_RX_CTRL_RANK_RXDLY_UPD_OFFSET) | P_Fld(0x2, MISC_SHU_PHY_RX_CTRL_RX_IN_GATE_EN_PRE_OFFSET) |
            P_Fld(0x0, MISC_SHU_PHY_RX_CTRL_RX_IN_GATE_EN_HEAD) | P_Fld(0x1, MISC_SHU_PHY_RX_CTRL_RX_IN_GATE_EN_TAIL) |
            P_Fld(0x0, MISC_SHU_PHY_RX_CTRL_RX_IN_BUFF_EN_HEAD) | P_Fld(0x0, MISC_SHU_PHY_RX_CTRL_RX_IN_BUFF_EN_TAIL));

    vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RANKCTL, P_Fld(0x0, MISC_SHU_RANKCTL_RANKINCTL_RXDLY) |
            P_Fld(0x1, MISC_SHU_RANKCTL_RANK_RXDLY_OPT) | P_Fld(0x0, MISC_SHU_RANKCTL_RANKSEL_SELPH_FRUN) |
            P_Fld(0x2, MISC_SHU_RANKCTL_RANKINCTL_STB) | P_Fld(0x0, MISC_SHU_RANKCTL_RANKINCTL) |
            P_Fld(0x0, MISC_SHU_RANKCTL_RANKINCTL_ROOT1) | P_Fld(0x3, MISC_SHU_RANKCTL_RANKINCTL_PHY));

    vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RANK_SEL_LAT, P_Fld(0x2, MISC_SHU_RANK_SEL_LAT_RANK_SEL_LAT_B0) |
            P_Fld(0x2, MISC_SHU_RANK_SEL_LAT_RANK_SEL_LAT_B1) | P_Fld(0x2, MISC_SHU_RANK_SEL_LAT_RANK_SEL_LAT_CA));

    vIO32WriteFldAlign(DDRPHY_REG_MISC_SHU_RK_DQSCTL, 0x2, MISC_SHU_RK_DQSCTL_DQSINCTL);

    vIO32WriteFldAlign(DDRPHY_REG_MISC_SHU_RK_DQSCTL+(1*0x80), 0x2, MISC_SHU_RK_DQSCTL_DQSINCTL);

    vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B0_DQSIEN_MCK_UI_DLY, P_Fld(0x9, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B0) |
            P_Fld(0xd, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B0) | P_Fld(0x0, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B0) |
            P_Fld(0x0, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B0));

    vIO32WriteFldAlign(DDRPHY_REG_SHU_RK_B0_DQSIEN_PI_DLY, 0x0b, SHU_RK_B0_DQSIEN_PI_DLY_DQSIEN_PI_B0);

    vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B0_DQSIEN_MCK_UI_DLY+(1*0x80), P_Fld(0xc, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B0) |
            P_Fld(0x0, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B0) | P_Fld(0x0, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B0) |
            P_Fld(0x1, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B0));

    vIO32WriteFldAlign(DDRPHY_REG_SHU_RK_B0_DQSIEN_PI_DLY+(1*0x80), 0x11, SHU_RK_B0_DQSIEN_PI_DLY_DQSIEN_PI_B0);

    vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B1_DQSIEN_MCK_UI_DLY, P_Fld(0x9, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B1) |
            P_Fld(0xd, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B1) | P_Fld(0x0, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B1) |
            P_Fld(0x0, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B1));

    vIO32WriteFldAlign(DDRPHY_REG_SHU_RK_B1_DQSIEN_PI_DLY, 0x0b, SHU_RK_B1_DQSIEN_PI_DLY_DQSIEN_PI_B1);

    vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B1_DQSIEN_MCK_UI_DLY+(1*0x80), P_Fld(0xc, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B1) |
            P_Fld(0x0, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B1) | P_Fld(0x0, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B1) |
            P_Fld(0x1, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B1));

    vIO32WriteFldAlign(DDRPHY_REG_SHU_RK_B1_DQSIEN_PI_DLY+(1*0x80), 0x11, SHU_RK_B1_DQSIEN_PI_DLY_DQSIEN_PI_B1);

    vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_ODTCTRL, P_Fld(0x1, MISC_SHU_ODTCTRL_RODTEN) |
            P_Fld(0x0, MISC_SHU_ODTCTRL_RODTENSTB_SELPH_CG_IG) | P_Fld(0x1, MISC_SHU_ODTCTRL_RODT_LAT) |
            P_Fld(0x0, MISC_SHU_ODTCTRL_RODTEN_SELPH_FRUN) | P_Fld(0x0, MISC_SHU_ODTCTRL_RODTDLY_LAT_OPT) |
            P_Fld(0x0, MISC_SHU_ODTCTRL_FIXRODT) | P_Fld(0x1, MISC_SHU_ODTCTRL_RODTEN_OPT) |
            P_Fld(0x1, MISC_SHU_ODTCTRL_RODTE2) | P_Fld(0x1, MISC_SHU_ODTCTRL_RODTE));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ7, P_Fld(0x0, SHU_B0_DQ7_R_DMRANKRXDVS_B0) |
            P_Fld(0x0, SHU_B0_DQ7_R_DMDQMDBI_EYE_SHU_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMDQMDBI_SHU_B0) |
            P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_DQM_FLAGSEL_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B0) |
            P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMRXTRACK_DQM_EN_B0) |
            P_Fld(0x1, SHU_B0_DQ7_R_DMRODTEN_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMARPI_CG_FB2DLL_DCM_EN_B0) |
            P_Fld(0x0, SHU_B0_DQ7_R_DMTX_ARPI_CG_DQ_NEW_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMTX_ARPI_CG_DQS_NEW_B0) |
            P_Fld(0x0, SHU_B0_DQ7_R_DMTX_ARPI_CG_DQM_NEW_B0) | P_Fld(0x0, SHU_B0_DQ7_R_LP4Y_SDN_MODE_DQS0) |
            P_Fld(0x1, SHU_B0_DQ7_R_DMRXRANK_DQ_EN_B0) | P_Fld(0x1, SHU_B0_DQ7_R_DMRXRANK_DQ_LAT_B0) |
            P_Fld(0x1, SHU_B0_DQ7_R_DMRXRANK_DQS_EN_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMRXRANK_DQS_LAT_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ7, P_Fld(0x0, SHU_B1_DQ7_R_DMRANKRXDVS_B1) |
            P_Fld(0x0, SHU_B1_DQ7_R_DMDQMDBI_EYE_SHU_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMDQMDBI_SHU_B1) |
            P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_DQM_FLAGSEL_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B1) |
            P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMRXTRACK_DQM_EN_B1) |
            P_Fld(0x1, SHU_B1_DQ7_R_DMRODTEN_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMARPI_CG_FB2DLL_DCM_EN_B1) |
            P_Fld(0x0, SHU_B1_DQ7_R_DMTX_ARPI_CG_DQ_NEW_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMTX_ARPI_CG_DQS_NEW_B1) |
            P_Fld(0x0, SHU_B1_DQ7_R_DMTX_ARPI_CG_DQM_NEW_B1) | P_Fld(0x0, SHU_B1_DQ7_R_LP4Y_SDN_MODE_DQS1) |
            P_Fld(0x1, SHU_B1_DQ7_R_DMRXRANK_DQ_EN_B1) | P_Fld(0x1, SHU_B1_DQ7_R_DMRXRANK_DQ_LAT_B1) |
            P_Fld(0x1, SHU_B1_DQ7_R_DMRXRANK_DQS_EN_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMRXRANK_DQS_LAT_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B0_RODTEN_MCK_UI_DLY, P_Fld(0x4, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B0) |
            P_Fld(0x4, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B0) | P_Fld(0x0, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B0) |
            P_Fld(0x0, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B0_RODTEN_MCK_UI_DLY+(1*0x80), P_Fld(0x7, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B0) |
            P_Fld(0x7, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B0) | P_Fld(0x0, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B0) |
            P_Fld(0x0, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B1_RODTEN_MCK_UI_DLY, P_Fld(0x4, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B1) |
            P_Fld(0x4, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B1) | P_Fld(0x0, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B1) |
            P_Fld(0x0, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B1_RODTEN_MCK_UI_DLY+(1*0x80), P_Fld(0x7, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B1) |
            P_Fld(0x7, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B1) | P_Fld(0x0, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B1) |
            P_Fld(0x0, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B1));

    vIO32WriteFldMulti(DRAMC_REG_SHU_RX_CG_SET0, P_Fld(0x0, SHU_RX_CG_SET0_DLE_LAST_EXTEND3) |
            P_Fld(0x0, SHU_RX_CG_SET0_READ_START_EXTEND3) | P_Fld(0x0, SHU_RX_CG_SET0_DLE_LAST_EXTEND2) |
            P_Fld(0x0, SHU_RX_CG_SET0_READ_START_EXTEND2) | P_Fld(0x1, SHU_RX_CG_SET0_DLE_LAST_EXTEND1) |
            P_Fld(0x1, SHU_RX_CG_SET0_READ_START_EXTEND1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_MISC_RANK_SEL_STB, P_Fld(0x1, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_EN) |
            P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_EN_B23) | P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_SERMODE) |
            P_Fld(0x1, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_TRACK) | P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_RXDLY_TRACK) |
            P_Fld(0x1, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_PHASE_EN) | P_Fld(0x3, SHU_MISC_RANK_SEL_STB_RANK_SEL_PHSINCTL) |
            P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_UI_PLUS) | P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_MCK_PLUS) |
            P_Fld(0x1, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_UI_MINUS) | P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_MCK_MINUS));

    vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RK_DQSCAL, P_Fld(0x60, MISC_SHU_RK_DQSCAL_DQSIENLLMT) |
            P_Fld(0x1, MISC_SHU_RK_DQSCAL_DQSIENLLMTEN) | P_Fld(0x3f, MISC_SHU_RK_DQSCAL_DQSIENHLMT) |
            P_Fld(0x1, MISC_SHU_RK_DQSCAL_DQSIENHLMTEN));

    vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RK_DQSCAL+(1*0x80), P_Fld(0x60, MISC_SHU_RK_DQSCAL_DQSIENLLMT) |
            P_Fld(0x1, MISC_SHU_RK_DQSCAL_DQSIENLLMTEN) | P_Fld(0x3f, MISC_SHU_RK_DQSCAL_DQSIENHLMT) |
            P_Fld(0x1, MISC_SHU_RK_DQSCAL_DQSIENHLMTEN));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_INI_UIPI, P_Fld(0x0b, SHU_R0_B0_INI_UIPI_CURR_INI_PI_B0) |
            P_Fld(0x09, SHU_R0_B0_INI_UIPI_CURR_INI_UI_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_INI_UIPI, P_Fld(0x0b, SHU_R0_B1_INI_UIPI_CURR_INI_PI_B1) |
            P_Fld(0x09, SHU_R0_B1_INI_UIPI_CURR_INI_UI_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_INI_UIPI+(1*0x80), P_Fld(0x11, SHU_R0_B0_INI_UIPI_CURR_INI_PI_B0) |
            P_Fld(0x0c, SHU_R0_B0_INI_UIPI_CURR_INI_UI_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_INI_UIPI+(1*0x80), P_Fld(0x11, SHU_R0_B1_INI_UIPI_CURR_INI_PI_B1) |
            P_Fld(0x0c, SHU_R0_B1_INI_UIPI_CURR_INI_UI_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_NEXT_INI_UIPI, P_Fld(0x0b, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_PI_B0) |
            P_Fld(0x09, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_UI_B0) | P_Fld(0x0d, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_UI_P1_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_NEXT_INI_UIPI, P_Fld(0x0b, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_PI_B1) |
            P_Fld(0x09, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_UI_B1) | P_Fld(0x0d, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_UI_P1_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_NEXT_INI_UIPI+(1*0x80), P_Fld(0x11, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_PI_B0) |
            P_Fld(0x0c, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_UI_B0) | P_Fld(0x10, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_UI_P1_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_NEXT_INI_UIPI+(1*0x80), P_Fld(0x11, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_PI_B1) |
            P_Fld(0x0c, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_UI_B1) | P_Fld(0x10, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_UI_P1_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_DQ0, P_Fld(0x0, SHU_R0_B0_DQ0_RG_RX_ARDQS0_R_DLY_DUTY) |
            P_Fld(0x0, SHU_R0_B0_DQ0_RG_RX_ARDQS0_F_DLY_DUTY) | P_Fld(0x1d, SHU_R0_B0_DQ0_SW_ARPI_DQ_B0) |
            P_Fld(0x1d, SHU_R0_B0_DQ0_SW_ARPI_DQM_B0) | P_Fld(0x00, SHU_R0_B0_DQ0_ARPI_PBYTE_B0) |
            P_Fld(0x0, SHU_R0_B0_DQ0_DA_ARPI_DDR400_0D5UI_RK0_B0) | P_Fld(0x0, SHU_R0_B0_DQ0_DA_RX_ARDQSIEN_0D5UI_RK0_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_DQ0, P_Fld(0x0, SHU_R0_B1_DQ0_RG_RX_ARDQS1_R_DLY_DUTY) |
            P_Fld(0x0, SHU_R0_B1_DQ0_RG_RX_ARDQS1_F_DLY_DUTY) | P_Fld(0x1d, SHU_R0_B1_DQ0_SW_ARPI_DQ_B1) |
            P_Fld(0x1d, SHU_R0_B1_DQ0_SW_ARPI_DQM_B1) | P_Fld(0x00, SHU_R0_B1_DQ0_ARPI_PBYTE_B1) |
            P_Fld(0x0, SHU_R0_B1_DQ0_DA_ARPI_DDR400_0D5UI_RK0_B1) | P_Fld(0x0, SHU_R0_B1_DQ0_DA_RX_ARDQSIEN_0D5UI_RK0_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_DQ0+(1*0x80), P_Fld(0x0, SHU_R0_B0_DQ0_RG_RX_ARDQS0_R_DLY_DUTY) |
            P_Fld(0x0, SHU_R0_B0_DQ0_RG_RX_ARDQS0_F_DLY_DUTY) | P_Fld(0x0d, SHU_R0_B0_DQ0_SW_ARPI_DQ_B0) |
            P_Fld(0x0d, SHU_R0_B0_DQ0_SW_ARPI_DQM_B0) | P_Fld(0x00, SHU_R0_B0_DQ0_ARPI_PBYTE_B0) |
            P_Fld(0x0, SHU_R0_B0_DQ0_DA_ARPI_DDR400_0D5UI_RK0_B0) | P_Fld(0x0, SHU_R0_B0_DQ0_DA_RX_ARDQSIEN_0D5UI_RK0_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_DQ0+(1*0x80), P_Fld(0x0, SHU_R0_B1_DQ0_RG_RX_ARDQS1_R_DLY_DUTY) |
            P_Fld(0x0, SHU_R0_B1_DQ0_RG_RX_ARDQS1_F_DLY_DUTY) | P_Fld(0x13, SHU_R0_B1_DQ0_SW_ARPI_DQ_B1) |
            P_Fld(0x13, SHU_R0_B1_DQ0_SW_ARPI_DQM_B1) | P_Fld(0x00, SHU_R0_B1_DQ0_ARPI_PBYTE_B1) |
            P_Fld(0x0, SHU_R0_B1_DQ0_DA_ARPI_DDR400_0D5UI_RK0_B1) | P_Fld(0x0, SHU_R0_B1_DQ0_DA_RX_ARDQSIEN_0D5UI_RK0_B1));

    #if (fcFOR_CHIP_ID == fcA60868)
    vIO32WriteFldMulti(DRAMC_REG_SHU_DCM_CTRL0, P_Fld(0x1, SHU_DCM_CTRL0_DDRPHY_CLK_EN_OPT) |
            P_Fld(0x4, SHU_DCM_CTRL0_DPHY_CMDDCM_EXTCNT) | P_Fld(0x5, SHU_DCM_CTRL0_DDRPHY_CLK_DYN_GATING_SEL) |
            P_Fld(0x0, SHU_DCM_CTRL0_CKE_EXTNONPD_CNT) | P_Fld(0x0, SHU_DCM_CTRL0_FASTWAKE2) |
            P_Fld(0x1, SHU_DCM_CTRL0_FASTWAKE));
    #elif(fcFOR_CHIP_ID == fcPetrus)
    vIO32WriteFldMulti(DRAMC_REG_SHU_DCM_CTRL0, P_Fld(0x1, SHU_DCM_CTRL0_DDRPHY_CLK_EN_OPT) |
            P_Fld(0x5, SHU_DCM_CTRL0_DDRPHY_CLK_DYN_GATING_SEL) |
            P_Fld(0x0, SHU_DCM_CTRL0_FASTWAKE2) |
            P_Fld(0x1, SHU_DCM_CTRL0_FASTWAKE));
    #endif

    vIO32WriteFldMulti(DRAMC_REG_SHU_APHY_TX_PICG_CTRL, P_Fld(0x7, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_PICG_CNT) |
            P_Fld(0x1, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQS_SEL_P1) | P_Fld(0x0, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQS_SEL_P0) |
            P_Fld(0x1, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_OPT));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_APHY_TX_PICG_CTRL, P_Fld(0x1, SHURK_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQ_RK_SEL_P1) |
            P_Fld(0x0, SHURK_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQ_RK_SEL_P0));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_APHY_TX_PICG_CTRL+(1*0x200), P_Fld(0x1, SHURK_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQ_RK_SEL_P1) |
            P_Fld(0x0, SHURK_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQ_RK_SEL_P0));

    vIO32WriteFldMulti(DRAMC_REG_SHU_NEW_XRW2W_CTRL, P_Fld(0x0, SHU_NEW_XRW2W_CTRL_TX_PI_UPDCTL_B0) |
            P_Fld(0x0, SHU_NEW_XRW2W_CTRL_TX_PI_UPDCTL_B1) | P_Fld(0x0, SHU_NEW_XRW2W_CTRL_TXPI_UPD_MODE));

    vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_DQS0, P_Fld(0x2, SHU_SELPH_DQS0_TXDLY_DQS0) |
            P_Fld(0x2, SHU_SELPH_DQS0_TXDLY_DQS1) | P_Fld(0x1, SHU_SELPH_DQS0_TXDLY_DQS2) |
            P_Fld(0x1, SHU_SELPH_DQS0_TXDLY_DQS3) | P_Fld(0x1, SHU_SELPH_DQS0_TXDLY_OEN_DQS0) |
            P_Fld(0x1, SHU_SELPH_DQS0_TXDLY_OEN_DQS1) | P_Fld(0x1, SHU_SELPH_DQS0_TXDLY_OEN_DQS2) |
            P_Fld(0x1, SHU_SELPH_DQS0_TXDLY_OEN_DQS3));

    vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_DQS1, P_Fld(0x1, SHU_SELPH_DQS1_DLY_DQS0) |
            P_Fld(0x1, SHU_SELPH_DQS1_DLY_DQS1) | P_Fld(0x1, SHU_SELPH_DQS1_DLY_DQS2) |
            P_Fld(0x1, SHU_SELPH_DQS1_DLY_DQS3) | P_Fld(0x6, SHU_SELPH_DQS1_DLY_OEN_DQS0) |
            P_Fld(0x6, SHU_SELPH_DQS1_DLY_OEN_DQS1) | P_Fld(0x1, SHU_SELPH_DQS1_DLY_OEN_DQS2) |
            P_Fld(0x1, SHU_SELPH_DQS1_DLY_OEN_DQS3));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ0, P_Fld(0x2, SHURK_SELPH_DQ0_TXDLY_DQ0) |
            P_Fld(0x2, SHURK_SELPH_DQ0_TXDLY_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_DQ2) |
            P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_DQ3) | P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_OEN_DQ0) |
            P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_OEN_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_OEN_DQ2) |
            P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_OEN_DQ3));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ1, P_Fld(0x2, SHURK_SELPH_DQ1_TXDLY_DQM0) |
            P_Fld(0x2, SHURK_SELPH_DQ1_TXDLY_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_DQM2) |
            P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_DQM3) | P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_OEN_DQM0) |
            P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_OEN_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_OEN_DQM2) |
            P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_OEN_DQM3));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ2, P_Fld(0x1, SHURK_SELPH_DQ2_DLY_DQ0) |
            P_Fld(0x1, SHURK_SELPH_DQ2_DLY_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ2_DLY_DQ2) |
            P_Fld(0x1, SHURK_SELPH_DQ2_DLY_DQ3) | P_Fld(0x6, SHURK_SELPH_DQ2_DLY_OEN_DQ0) |
            P_Fld(0x6, SHURK_SELPH_DQ2_DLY_OEN_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ2_DLY_OEN_DQ2) |
            P_Fld(0x1, SHURK_SELPH_DQ2_DLY_OEN_DQ3));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ3, P_Fld(0x1, SHURK_SELPH_DQ3_DLY_DQM0) |
            P_Fld(0x1, SHURK_SELPH_DQ3_DLY_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ3_DLY_DQM2) |
            P_Fld(0x1, SHURK_SELPH_DQ3_DLY_DQM3) | P_Fld(0x6, SHURK_SELPH_DQ3_DLY_OEN_DQM0) |
            P_Fld(0x6, SHURK_SELPH_DQ3_DLY_OEN_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ3_DLY_OEN_DQM2) |
            P_Fld(0x1, SHURK_SELPH_DQ3_DLY_OEN_DQM3));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ0+(1*0x200), P_Fld(0x2, SHURK_SELPH_DQ0_TXDLY_DQ0) |
            P_Fld(0x2, SHURK_SELPH_DQ0_TXDLY_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_DQ2) |
            P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_DQ3) | P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_OEN_DQ0) |
            P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_OEN_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_OEN_DQ2) |
            P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_OEN_DQ3));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ1+(1*0x200), P_Fld(0x2, SHURK_SELPH_DQ1_TXDLY_DQM0) |
            P_Fld(0x2, SHURK_SELPH_DQ1_TXDLY_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_DQM2) |
            P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_DQM3) | P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_OEN_DQM0) |
            P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_OEN_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_OEN_DQM2) |
            P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_OEN_DQM3));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ2+(1*0x200), P_Fld(0x2, SHURK_SELPH_DQ2_DLY_DQ0) |
            P_Fld(0x2, SHURK_SELPH_DQ2_DLY_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ2_DLY_DQ2) |
            P_Fld(0x1, SHURK_SELPH_DQ2_DLY_DQ3) | P_Fld(0x7, SHURK_SELPH_DQ2_DLY_OEN_DQ0) |
            P_Fld(0x7, SHURK_SELPH_DQ2_DLY_OEN_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ2_DLY_OEN_DQ2) |
            P_Fld(0x1, SHURK_SELPH_DQ2_DLY_OEN_DQ3));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ3+(1*0x200), P_Fld(0x2, SHURK_SELPH_DQ3_DLY_DQM0) |
            P_Fld(0x2, SHURK_SELPH_DQ3_DLY_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ3_DLY_DQM2) |
            P_Fld(0x1, SHURK_SELPH_DQ3_DLY_DQM3) | P_Fld(0x7, SHURK_SELPH_DQ3_DLY_OEN_DQM0) |
            P_Fld(0x7, SHURK_SELPH_DQ3_DLY_OEN_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ3_DLY_OEN_DQM2) |
            P_Fld(0x1, SHURK_SELPH_DQ3_DLY_OEN_DQM3));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL1, P_Fld(0x01d, SHURK_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ0) |
            P_Fld(0x01d, SHURK_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ1));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL2, P_Fld(0x01d, SHURK_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ0) |
            P_Fld(0x01d, SHURK_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ1));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL5, P_Fld(0x01d, SHURK_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM0) |
            P_Fld(0x01d, SHURK_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM1));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL1+(1*0x200), P_Fld(0x00d, SHURK_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ0) |
            P_Fld(0x013, SHURK_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ1));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL2+(1*0x200), P_Fld(0x00d, SHURK_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ0) |
            P_Fld(0x013, SHURK_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ1));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL5+(1*0x200), P_Fld(0x00d, SHURK_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM0) |
            P_Fld(0x013, SHURK_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM1));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_PI, P_Fld(0x1d, SHURK_PI_RK0_ARPI_DQ_B1) |
            P_Fld(0x1d, SHURK_PI_RK0_ARPI_DQ_B0) | P_Fld(0x1d, SHURK_PI_RK0_ARPI_DQM_B1) |
            P_Fld(0x1d, SHURK_PI_RK0_ARPI_DQM_B0));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_PI+(1*0x200), P_Fld(0x13, SHURK_PI_RK0_ARPI_DQ_B1) |
            P_Fld(0x0d, SHURK_PI_RK0_ARPI_DQ_B0) | P_Fld(0x13, SHURK_PI_RK0_ARPI_DQM_B1) |
            P_Fld(0x0d, SHURK_PI_RK0_ARPI_DQM_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_TXDLY0, P_Fld(0x10, SHU_R0_B0_TXDLY0_TX_ARDQ0_DLY_B0) |
            P_Fld(0x10, SHU_R0_B0_TXDLY0_TX_ARDQ1_DLY_B0) | P_Fld(0x10, SHU_R0_B0_TXDLY0_TX_ARDQ2_DLY_B0) |
            P_Fld(0x10, SHU_R0_B0_TXDLY0_TX_ARDQ3_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_TXDLY1, P_Fld(0x10, SHU_R0_B0_TXDLY1_TX_ARDQ4_DLY_B0) |
            P_Fld(0x10, SHU_R0_B0_TXDLY1_TX_ARDQ5_DLY_B0) | P_Fld(0x10, SHU_R0_B0_TXDLY1_TX_ARDQ6_DLY_B0) |
            P_Fld(0x10, SHU_R0_B0_TXDLY1_TX_ARDQ7_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_TXDLY3, P_Fld(0x10, SHU_R0_B0_TXDLY3_TX_ARDQM0_DLY_B0) |
            P_Fld(0x00, SHU_R0_B0_TXDLY3_TX_ARWCK_DLY_B0) | P_Fld(0x00, SHU_R0_B0_TXDLY3_TX_ARWCKB_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_TXDLY0, P_Fld(0x14, SHU_R0_B1_TXDLY0_TX_ARDQ0_DLY_B1) |
            P_Fld(0x14, SHU_R0_B1_TXDLY0_TX_ARDQ1_DLY_B1) | P_Fld(0x14, SHU_R0_B1_TXDLY0_TX_ARDQ2_DLY_B1) |
            P_Fld(0x14, SHU_R0_B1_TXDLY0_TX_ARDQ3_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_TXDLY1, P_Fld(0x14, SHU_R0_B1_TXDLY1_TX_ARDQ4_DLY_B1) |
            P_Fld(0x14, SHU_R0_B1_TXDLY1_TX_ARDQ5_DLY_B1) | P_Fld(0x14, SHU_R0_B1_TXDLY1_TX_ARDQ6_DLY_B1) |
            P_Fld(0x14, SHU_R0_B1_TXDLY1_TX_ARDQ7_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_TXDLY3, P_Fld(0x14, SHU_R0_B1_TXDLY3_TX_ARDQM0_DLY_B1) |
            P_Fld(0x00, SHU_R0_B1_TXDLY3_TX_ARWCK_DLY_B1) | P_Fld(0x00, SHU_R0_B1_TXDLY3_TX_ARWCKB_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_TXDLY0+(1*0x80), P_Fld(0x3c, SHU_R0_B0_TXDLY0_TX_ARDQ0_DLY_B0) |
            P_Fld(0x3c, SHU_R0_B0_TXDLY0_TX_ARDQ1_DLY_B0) | P_Fld(0x3c, SHU_R0_B0_TXDLY0_TX_ARDQ2_DLY_B0) |
            P_Fld(0x3c, SHU_R0_B0_TXDLY0_TX_ARDQ3_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_TXDLY1+(1*0x80), P_Fld(0x3c, SHU_R0_B0_TXDLY1_TX_ARDQ4_DLY_B0) |
            P_Fld(0x3c, SHU_R0_B0_TXDLY1_TX_ARDQ5_DLY_B0) | P_Fld(0x3c, SHU_R0_B0_TXDLY1_TX_ARDQ6_DLY_B0) |
            P_Fld(0x3c, SHU_R0_B0_TXDLY1_TX_ARDQ7_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_TXDLY3+(1*0x80), P_Fld(0x3c, SHU_R0_B0_TXDLY3_TX_ARDQM0_DLY_B0) |
            P_Fld(0x00, SHU_R0_B0_TXDLY3_TX_ARWCK_DLY_B0) | P_Fld(0x00, SHU_R0_B0_TXDLY3_TX_ARWCKB_DLY_B0));

    vIO32WriteFldAlign(DDRPHY_REG_SHU_B0_DQ9, 0xbf31f45b, SHU_B0_DQ9_RG_ARPI_RESERVE_B0);

    vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_CA1, P_Fld(0x0, SHU_SELPH_CA1_TXDLY_CS) |
            P_Fld(0x0, SHU_SELPH_CA1_TXDLY_CKE) | P_Fld(0x0, SHU_SELPH_CA1_TXDLY_ODT) |
            P_Fld(0x0, SHU_SELPH_CA1_TXDLY_RESET) | P_Fld(0x0, SHU_SELPH_CA1_TXDLY_WE) |
            P_Fld(0x0, SHU_SELPH_CA1_TXDLY_CAS) | P_Fld(0x0, SHU_SELPH_CA1_TXDLY_RAS) |
            P_Fld(0x0, SHU_SELPH_CA1_TXDLY_CS1));

    vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_CA2, P_Fld(0x0, SHU_SELPH_CA2_TXDLY_BA0) |
            P_Fld(0x0, SHU_SELPH_CA2_TXDLY_BA1) | P_Fld(0x0, SHU_SELPH_CA2_TXDLY_BA2) |
            P_Fld(0x01, SHU_SELPH_CA2_TXDLY_CMD) | P_Fld(0x0, SHU_SELPH_CA2_TXDLY_CKE1));

    vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_CA3, P_Fld(0x0, SHU_SELPH_CA3_TXDLY_RA0) |
            P_Fld(0x0, SHU_SELPH_CA3_TXDLY_RA1) | P_Fld(0x0, SHU_SELPH_CA3_TXDLY_RA2) |
            P_Fld(0x0, SHU_SELPH_CA3_TXDLY_RA3) | P_Fld(0x0, SHU_SELPH_CA3_TXDLY_RA4) |
            P_Fld(0x0, SHU_SELPH_CA3_TXDLY_RA5) | P_Fld(0x0, SHU_SELPH_CA3_TXDLY_RA6) |
            P_Fld(0x0, SHU_SELPH_CA3_TXDLY_RA7));

    vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_CA4, P_Fld(0x0, SHU_SELPH_CA4_TXDLY_RA8) |
            P_Fld(0x0, SHU_SELPH_CA4_TXDLY_RA9) | P_Fld(0x0, SHU_SELPH_CA4_TXDLY_RA10) |
            P_Fld(0x0, SHU_SELPH_CA4_TXDLY_RA11) | P_Fld(0x0, SHU_SELPH_CA4_TXDLY_RA12) |
            P_Fld(0x0, SHU_SELPH_CA4_TXDLY_RA13) | P_Fld(0x0, SHU_SELPH_CA4_TXDLY_RA14) |
            P_Fld(0x0, SHU_SELPH_CA4_TXDLY_RA15));

    vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_CA5, P_Fld(0x1, SHU_SELPH_CA5_DLY_CS) |
            P_Fld(0x1, SHU_SELPH_CA5_DLY_CKE) | P_Fld(0x0, SHU_SELPH_CA5_DLY_ODT) |
            P_Fld(0x1, SHU_SELPH_CA5_DLY_RESET) | P_Fld(0x1, SHU_SELPH_CA5_DLY_WE) |
            P_Fld(0x1, SHU_SELPH_CA5_DLY_CAS) | P_Fld(0x1, SHU_SELPH_CA5_DLY_RAS) |
            P_Fld(0x1, SHU_SELPH_CA5_DLY_CS1));

    vIO32WriteFldMulti(DRAMC_REG_SHU_SREF_CTRL, P_Fld(0x3, SHU_SREF_CTRL_CKEHCMD) |
            P_Fld(0x3, SHU_SREF_CTRL_SREF_CK_DLY));

    vIO32WriteFldMulti(DRAMC_REG_SHU_HMR4_DVFS_CTRL0, P_Fld(0x32, SHU_HMR4_DVFS_CTRL0_FSPCHG_PRDCNT) |
            P_Fld(0x000, SHU_HMR4_DVFS_CTRL0_REFRCNT));

    vIO32WriteFldMulti(DRAMC_REG_SHU_AC_TIME_05T, P_Fld(0x0, SHU_AC_TIME_05T_TRC_05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_TRFCPB_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TRFC_05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_TPBR2PBR_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TXP_05T) |
            P_Fld(0x1, SHU_AC_TIME_05T_TRTP_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TRCD_05T) |
            P_Fld(0x1, SHU_AC_TIME_05T_TRP_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TRPAB_05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_TRAS_05T) | P_Fld(0x1, SHU_AC_TIME_05T_TWR_M05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_TRRD_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TFAW_05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_TCKEPRD_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TR2PD_05T) |
            P_Fld(0x1, SHU_AC_TIME_05T_TWTPD_M05T) | P_Fld(0x1, SHU_AC_TIME_05T_TMRRI_05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_TMRWCKEL_05T) | P_Fld(0x0, SHU_AC_TIME_05T_BGTRRD_05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_BGTCCD_05T) | P_Fld(0x1, SHU_AC_TIME_05T_BGTWTR_M05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_TR2W_05T) | P_Fld(0x1, SHU_AC_TIME_05T_TWTR_M05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_XRTR2W_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TMRD_05T) |
            P_Fld(0x1, SHU_AC_TIME_05T_TMRW_05T) | P_Fld(0x1, SHU_AC_TIME_05T_TMRR2MRW_05T) |
            P_Fld(0x1, SHU_AC_TIME_05T_TW2MRW_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TR2MRW_05T) |
            P_Fld(0x0, SHU_AC_TIME_05T_TPBR2ACT_05T) | P_Fld(0x0, SHU_AC_TIME_05T_XRTW2R_M05T));

    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM_XRT, P_Fld(0x03, SHU_ACTIM_XRT_XRTR2R) |
            P_Fld(0x03, SHU_ACTIM_XRT_XRTR2W) | P_Fld(0x3, SHU_ACTIM_XRT_XRTW2R) |
            P_Fld(0x4, SHU_ACTIM_XRT_XRTW2W));

    #if (fcFOR_CHIP_ID == fcA60868)
    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM0, P_Fld(0x4, SHU_ACTIM0_TWTR) |
            P_Fld(0x2, SHU_ACTIM0_CKELCKCNT) | P_Fld(0x08, SHU_ACTIM0_TWR) |
            P_Fld(0x1, SHU_ACTIM0_TRRD) | P_Fld(0x4, SHU_ACTIM0_TRCD) |
            P_Fld(0x7, SHU_ACTIM0_TWTR_L));
    #elif (fcFOR_CHIP_ID == fcPetrus)
    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM0, P_Fld(0x4, SHU_ACTIM0_TWTR) |
            P_Fld(0x2, SHU_ACTIM0_CKELCKCNT) | P_Fld(0x08, SHU_ACTIM0_TWR) |
            P_Fld(0x1, SHU_ACTIM0_TRRD) | P_Fld(0x4, SHU_ACTIM0_TRCD));
    #endif

    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM1, P_Fld(0x3, SHU_ACTIM1_TRPAB) |
            P_Fld(0x4, SHU_ACTIM1_TMRWCKEL) | P_Fld(0x2, SHU_ACTIM1_TRP) |
            P_Fld(0x00, SHU_ACTIM1_TRAS) | P_Fld(0x04, SHU_ACTIM1_TRC));

    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM2, P_Fld(0x0, SHU_ACTIM2_TXP) |
            P_Fld(0x05, SHU_ACTIM2_TMRRI) | P_Fld(0x1, SHU_ACTIM2_TRTP) |
            P_Fld(0x03, SHU_ACTIM2_TR2W) | P_Fld(0x00, SHU_ACTIM2_TFAW));

    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM3, P_Fld(0x10, SHU_ACTIM3_TRFCPB) |
            P_Fld(0x4, SHU_ACTIM3_MANTMRR) | P_Fld(0x4, SHU_ACTIM3_TR2MRR) |
            P_Fld(0x2c, SHU_ACTIM3_TRFC));

    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM4, P_Fld(0x03a, SHU_ACTIM4_TXREFCNT) |
            P_Fld(0x07, SHU_ACTIM4_TMRR2MRW) | P_Fld(0x05, SHU_ACTIM4_TMRR2W) |
            P_Fld(0x10, SHU_ACTIM4_TZQCS));

    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM5, P_Fld(0x08, SHU_ACTIM5_TR2PD) |
            P_Fld(0x09, SHU_ACTIM5_TWTPD) | P_Fld(0x12, SHU_ACTIM5_TPBR2PBR) |
            P_Fld(0x0, SHU_ACTIM5_TPBR2ACT));

    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM6, P_Fld(0x06, SHU_ACTIM6_TZQLAT2) |
            P_Fld(0x3, SHU_ACTIM6_TMRD) | P_Fld(0x2, SHU_ACTIM6_TMRW) |
            P_Fld(0x06, SHU_ACTIM6_TW2MRW) | P_Fld(0x07, SHU_ACTIM6_TR2MRW));

    vIO32WriteFldMulti(DRAMC_REG_SHU_CKECTRL, P_Fld(0x1, SHU_CKECTRL_TPDE_05T) |
            P_Fld(0x0, SHU_CKECTRL_TPDX_05T) | P_Fld(0x1, SHU_CKECTRL_TPDE) |
            P_Fld(0x1, SHU_CKECTRL_TPDX) | P_Fld(0x1, SHU_CKECTRL_TCKEPRD) |
            P_Fld(0x3, SHU_CKECTRL_TCKESRX));

    vIO32WriteFldMulti(DRAMC_REG_SHU_MISC, P_Fld(0x2, SHU_MISC_REQQUE_MAXCNT) |
            P_Fld(0x7, SHU_MISC_DCMDLYREF) | P_Fld(0x0, SHU_MISC_DAREFEN));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ8, P_Fld(0x0063, SHU_B0_DQ8_R_DMRXDVS_UPD_FORCE_CYC_B0) |
            P_Fld(0x0, SHU_B0_DQ8_R_DMRXDVS_UPD_FORCE_EN_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B0) |
            P_Fld(0x0, SHU_B0_DQ8_R_RMRODTEN_CG_IG_B0) | P_Fld(0x1, SHU_B0_DQ8_R_RMRX_TOPHY_CG_IG_B0) |
            P_Fld(0x0, SHU_B0_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B0) |
            P_Fld(0x1, SHU_B0_DQ8_R_DMRXDLY_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B0) |
            P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B0) |
            P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMRANK_PIPE_CG_IG_B0) |
            P_Fld(0x0, SHU_B0_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ8, P_Fld(0x0063, SHU_B1_DQ8_R_DMRXDVS_UPD_FORCE_CYC_B1) |
            P_Fld(0x0, SHU_B1_DQ8_R_DMRXDVS_UPD_FORCE_EN_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B1) |
            P_Fld(0x0, SHU_B1_DQ8_R_RMRODTEN_CG_IG_B1) | P_Fld(0x1, SHU_B1_DQ8_R_RMRX_TOPHY_CG_IG_B1) |
            P_Fld(0x0, SHU_B1_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B1) |
            P_Fld(0x1, SHU_B1_DQ8_R_DMRXDLY_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B1) |
            P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B1) |
            P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMRANK_PIPE_CG_IG_B1) |
            P_Fld(0x0, SHU_B1_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ5, P_Fld(0x20, SHU_B0_DQ5_RG_RX_ARDQ_VREF_SEL_B0) |
            P_Fld(0x0, SHU_B0_DQ5_RG_RX_ARDQ_VREF_BYPASS_B0) | P_Fld(0x00, SHU_B0_DQ5_RG_ARPI_FB_B0) |
            P_Fld(0x0, SHU_B0_DQ5_RG_RX_ARDQS0_DQSIEN_DLY_B0) | P_Fld(0x0, SHU_B0_DQ5_RG_RX_ARDQS_DQSIEN_RB_DLY_B0) |
            P_Fld(0x5, SHU_B0_DQ5_RG_RX_ARDQS0_DVS_DLY_B0) | P_Fld(0x0, SHU_B0_DQ5_RG_RX_ARDQ_FIFO_DQSI_DLY_B0));  //RX_ARDQ_VREF_SEL_B0 is useless


    vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ5, P_Fld(0x20, SHU_B1_DQ5_RG_RX_ARDQ_VREF_SEL_B1) |
            P_Fld(0x0, SHU_B1_DQ5_RG_RX_ARDQ_VREF_BYPASS_B1) | P_Fld(0x00, SHU_B1_DQ5_RG_ARPI_FB_B1) |
            P_Fld(0x0, SHU_B1_DQ5_RG_RX_ARDQS0_DQSIEN_DLY_B1) | P_Fld(0x0, SHU_B1_DQ5_RG_RX_ARDQS_DQSIEN_RB_DLY_B1) |
            P_Fld(0x5, SHU_B1_DQ5_RG_RX_ARDQS0_DVS_DLY_B1) | P_Fld(0x0, SHU_B1_DQ5_RG_RX_ARDQ_FIFO_DQSI_DLY_B1)); //RX_ARDQ_VREF_SEL_B0 is useless


    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY0, P_Fld(0x9f, SHU_R0_B0_RXDLY0_RX_ARDQ0_R_DLY_B0) |
            P_Fld(0x9f, SHU_R0_B0_RXDLY0_RX_ARDQ0_F_DLY_B0) | P_Fld(0x9f, SHU_R0_B0_RXDLY0_RX_ARDQ1_R_DLY_B0) |
            P_Fld(0x9f, SHU_R0_B0_RXDLY0_RX_ARDQ1_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY1, P_Fld(0x9f, SHU_R0_B0_RXDLY1_RX_ARDQ2_R_DLY_B0) |
            P_Fld(0x9f, SHU_R0_B0_RXDLY1_RX_ARDQ2_F_DLY_B0) | P_Fld(0x9f, SHU_R0_B0_RXDLY1_RX_ARDQ3_R_DLY_B0) |
            P_Fld(0x9f, SHU_R0_B0_RXDLY1_RX_ARDQ3_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY2, P_Fld(0x9f, SHU_R0_B0_RXDLY2_RX_ARDQ4_R_DLY_B0) |
            P_Fld(0x9f, SHU_R0_B0_RXDLY2_RX_ARDQ4_F_DLY_B0) | P_Fld(0x9f, SHU_R0_B0_RXDLY2_RX_ARDQ5_R_DLY_B0) |
            P_Fld(0x9f, SHU_R0_B0_RXDLY2_RX_ARDQ5_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY3, P_Fld(0x9f, SHU_R0_B0_RXDLY3_RX_ARDQ6_R_DLY_B0) |
            P_Fld(0x9f, SHU_R0_B0_RXDLY3_RX_ARDQ6_F_DLY_B0) | P_Fld(0x9f, SHU_R0_B0_RXDLY3_RX_ARDQ7_R_DLY_B0) |
            P_Fld(0x9f, SHU_R0_B0_RXDLY3_RX_ARDQ7_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY4, P_Fld(0x9f, SHU_R0_B0_RXDLY4_RX_ARDQM0_R_DLY_B0) |
            P_Fld(0x9f, SHU_R0_B0_RXDLY4_RX_ARDQM0_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY5, P_Fld(0x0e5, SHU_R0_B0_RXDLY5_RX_ARDQS0_R_DLY_B0) |
            P_Fld(0x0e5, SHU_R0_B0_RXDLY5_RX_ARDQS0_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY0+(1*0x80), P_Fld(0x9e, SHU_R0_B0_RXDLY0_RX_ARDQ0_R_DLY_B0) |
            P_Fld(0x9e, SHU_R0_B0_RXDLY0_RX_ARDQ0_F_DLY_B0) | P_Fld(0x9e, SHU_R0_B0_RXDLY0_RX_ARDQ1_R_DLY_B0) |
            P_Fld(0x9e, SHU_R0_B0_RXDLY0_RX_ARDQ1_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY1+(1*0x80), P_Fld(0x9e, SHU_R0_B0_RXDLY1_RX_ARDQ2_R_DLY_B0) |
            P_Fld(0x9e, SHU_R0_B0_RXDLY1_RX_ARDQ2_F_DLY_B0) | P_Fld(0x9e, SHU_R0_B0_RXDLY1_RX_ARDQ3_R_DLY_B0) |
            P_Fld(0x9e, SHU_R0_B0_RXDLY1_RX_ARDQ3_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY2+(1*0x80), P_Fld(0x9e, SHU_R0_B0_RXDLY2_RX_ARDQ4_R_DLY_B0) |
            P_Fld(0x9e, SHU_R0_B0_RXDLY2_RX_ARDQ4_F_DLY_B0) | P_Fld(0x9e, SHU_R0_B0_RXDLY2_RX_ARDQ5_R_DLY_B0) |
            P_Fld(0x9e, SHU_R0_B0_RXDLY2_RX_ARDQ5_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY3+(1*0x80), P_Fld(0x9e, SHU_R0_B0_RXDLY3_RX_ARDQ6_R_DLY_B0) |
            P_Fld(0x9e, SHU_R0_B0_RXDLY3_RX_ARDQ6_F_DLY_B0) | P_Fld(0x9e, SHU_R0_B0_RXDLY3_RX_ARDQ7_R_DLY_B0) |
            P_Fld(0x9e, SHU_R0_B0_RXDLY3_RX_ARDQ7_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY4+(1*0x80), P_Fld(0x9e, SHU_R0_B0_RXDLY4_RX_ARDQM0_R_DLY_B0) |
            P_Fld(0x9e, SHU_R0_B0_RXDLY4_RX_ARDQM0_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY5+(1*0x80), P_Fld(0x0e4, SHU_R0_B0_RXDLY5_RX_ARDQS0_R_DLY_B0) |
            P_Fld(0x0e4, SHU_R0_B0_RXDLY5_RX_ARDQS0_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY0, P_Fld(0x9f, SHU_R0_B1_RXDLY0_RX_ARDQ0_R_DLY_B1) |
            P_Fld(0x9f, SHU_R0_B1_RXDLY0_RX_ARDQ0_F_DLY_B1) | P_Fld(0x9f, SHU_R0_B1_RXDLY0_RX_ARDQ1_R_DLY_B1) |
            P_Fld(0x9f, SHU_R0_B1_RXDLY0_RX_ARDQ1_F_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY1, P_Fld(0x9f, SHU_R0_B1_RXDLY1_RX_ARDQ2_R_DLY_B1) |
            P_Fld(0x9f, SHU_R0_B1_RXDLY1_RX_ARDQ2_F_DLY_B1) | P_Fld(0x9f, SHU_R0_B1_RXDLY1_RX_ARDQ3_R_DLY_B1) |
            P_Fld(0x9f, SHU_R0_B1_RXDLY1_RX_ARDQ3_F_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY2, P_Fld(0x9f, SHU_R0_B1_RXDLY2_RX_ARDQ4_R_DLY_B1) |
            P_Fld(0x9f, SHU_R0_B1_RXDLY2_RX_ARDQ4_F_DLY_B1) | P_Fld(0x9f, SHU_R0_B1_RXDLY2_RX_ARDQ5_R_DLY_B1) |
            P_Fld(0x9f, SHU_R0_B1_RXDLY2_RX_ARDQ5_F_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY3, P_Fld(0x9f, SHU_R0_B1_RXDLY3_RX_ARDQ6_R_DLY_B1) |
            P_Fld(0x9f, SHU_R0_B1_RXDLY3_RX_ARDQ6_F_DLY_B1) | P_Fld(0x9f, SHU_R0_B1_RXDLY3_RX_ARDQ7_R_DLY_B1) |
            P_Fld(0x9f, SHU_R0_B1_RXDLY3_RX_ARDQ7_F_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY4, P_Fld(0x9f, SHU_R0_B1_RXDLY4_RX_ARDQM0_R_DLY_B1) |
            P_Fld(0x9f, SHU_R0_B1_RXDLY4_RX_ARDQM0_F_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY5, P_Fld(0x0e5, SHU_R0_B1_RXDLY5_RX_ARDQS0_R_DLY_B1) |
            P_Fld(0x0e5, SHU_R0_B1_RXDLY5_RX_ARDQS0_F_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY0+(1*0x80), P_Fld(0x9e, SHU_R0_B1_RXDLY0_RX_ARDQ0_R_DLY_B1) |
            P_Fld(0x9e, SHU_R0_B1_RXDLY0_RX_ARDQ0_F_DLY_B1) | P_Fld(0x9e, SHU_R0_B1_RXDLY0_RX_ARDQ1_R_DLY_B1) |
            P_Fld(0x9e, SHU_R0_B1_RXDLY0_RX_ARDQ1_F_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY1+(1*0x80), P_Fld(0x9e, SHU_R0_B1_RXDLY1_RX_ARDQ2_R_DLY_B1) |
            P_Fld(0x9e, SHU_R0_B1_RXDLY1_RX_ARDQ2_F_DLY_B1) | P_Fld(0x9e, SHU_R0_B1_RXDLY1_RX_ARDQ3_R_DLY_B1) |
            P_Fld(0x9e, SHU_R0_B1_RXDLY1_RX_ARDQ3_F_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY2+(1*0x80), P_Fld(0x9e, SHU_R0_B1_RXDLY2_RX_ARDQ4_R_DLY_B1) |
            P_Fld(0x9e, SHU_R0_B1_RXDLY2_RX_ARDQ4_F_DLY_B1) | P_Fld(0x9e, SHU_R0_B1_RXDLY2_RX_ARDQ5_R_DLY_B1) |
            P_Fld(0x9e, SHU_R0_B1_RXDLY2_RX_ARDQ5_F_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY3+(1*0x80), P_Fld(0x9e, SHU_R0_B1_RXDLY3_RX_ARDQ6_R_DLY_B1) |
            P_Fld(0x9e, SHU_R0_B1_RXDLY3_RX_ARDQ6_F_DLY_B1) | P_Fld(0x9e, SHU_R0_B1_RXDLY3_RX_ARDQ7_R_DLY_B1) |
            P_Fld(0x9e, SHU_R0_B1_RXDLY3_RX_ARDQ7_F_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY4+(1*0x80), P_Fld(0x9e, SHU_R0_B1_RXDLY4_RX_ARDQM0_R_DLY_B1) |
            P_Fld(0x9e, SHU_R0_B1_RXDLY4_RX_ARDQM0_F_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY5+(1*0x80), P_Fld(0x0e4, SHU_R0_B1_RXDLY5_RX_ARDQS0_R_DLY_B1) |
            P_Fld(0x0e4, SHU_R0_B1_RXDLY5_RX_ARDQS0_F_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_B0_DQ9, P_Fld(0x0, B0_DQ9_RG_RX_ARDQ_STBEN_RESETB_B0) |
            P_Fld(0x1, B0_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B0) | P_Fld(0x0, B0_DQ9_RG_RX_ARDQS0_DQSIENMODE_B0) |
            P_Fld(0x1, B0_DQ9_R_DMRXDVS_R_F_DLY_RK_OPT_B0) | P_Fld(0x0, B0_DQ9_R_DMRXFIFO_STBENCMP_EN_B0) |
            P_Fld(0x00, B0_DQ9_R_IN_GATE_EN_LOW_OPT_B0) | P_Fld(0x0, B0_DQ9_R_DMDQSIEN_VALID_LAT_B0) |
            P_Fld(0x0, B0_DQ9_R_DMDQSIEN_RDSEL_LAT_B0) | P_Fld(0x0, B0_DQ9_R_DMRXDVS_VALID_LAT_B0) |
            P_Fld(0x0, B0_DQ9_R_DMRXDVS_RDSEL_LAT_B0));

    vIO32WriteFldMulti(DDRPHY_REG_B1_DQ9, P_Fld(0x0, B1_DQ9_RG_RX_ARDQ_STBEN_RESETB_B1) |
            P_Fld(0x1, B1_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B1) | P_Fld(0x0, B1_DQ9_RG_RX_ARDQS0_DQSIENMODE_B1) |
            P_Fld(0x1, B1_DQ9_R_DMRXDVS_R_F_DLY_RK_OPT_B1) | P_Fld(0x0, B1_DQ9_R_DMRXFIFO_STBENCMP_EN_B1) |
            P_Fld(0x00, B1_DQ9_R_IN_GATE_EN_LOW_OPT_B1) | P_Fld(0x0, B1_DQ9_R_DMDQSIEN_VALID_LAT_B1) |
            P_Fld(0x0, B1_DQ9_R_DMDQSIEN_RDSEL_LAT_B1) | P_Fld(0x0, B1_DQ9_R_DMRXDVS_VALID_LAT_B1) |
            P_Fld(0x0, B1_DQ9_R_DMRXDVS_RDSEL_LAT_B1));

    vIO32WriteFldMulti(DDRPHY_REG_B0_DQ9, P_Fld(0x1, B0_DQ9_RG_RX_ARDQ_STBEN_RESETB_B0) |
            P_Fld(0x1, B0_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B0) | P_Fld(0x0, B0_DQ9_RG_RX_ARDQS0_DQSIENMODE_B0) |
            P_Fld(0x1, B0_DQ9_R_DMRXDVS_R_F_DLY_RK_OPT_B0) | P_Fld(0x0, B0_DQ9_R_DMRXFIFO_STBENCMP_EN_B0) |
            P_Fld(0x00, B0_DQ9_R_IN_GATE_EN_LOW_OPT_B0) | P_Fld(0x0, B0_DQ9_R_DMDQSIEN_VALID_LAT_B0) |
            P_Fld(0x0, B0_DQ9_R_DMDQSIEN_RDSEL_LAT_B0) | P_Fld(0x0, B0_DQ9_R_DMRXDVS_VALID_LAT_B0) |
            P_Fld(0x0, B0_DQ9_R_DMRXDVS_RDSEL_LAT_B0));

    vIO32WriteFldMulti(DDRPHY_REG_B1_DQ9, P_Fld(0x1, B1_DQ9_RG_RX_ARDQ_STBEN_RESETB_B1) |
            P_Fld(0x1, B1_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B1) | P_Fld(0x0, B1_DQ9_RG_RX_ARDQS0_DQSIENMODE_B1) |
            P_Fld(0x1, B1_DQ9_R_DMRXDVS_R_F_DLY_RK_OPT_B1) | P_Fld(0x0, B1_DQ9_R_DMRXFIFO_STBENCMP_EN_B1) |
            P_Fld(0x00, B1_DQ9_R_IN_GATE_EN_LOW_OPT_B1) | P_Fld(0x0, B1_DQ9_R_DMDQSIEN_VALID_LAT_B1) |
            P_Fld(0x0, B1_DQ9_R_DMDQSIEN_RDSEL_LAT_B1) | P_Fld(0x0, B1_DQ9_R_DMRXDVS_VALID_LAT_B1) |
            P_Fld(0x0, B1_DQ9_R_DMRXDVS_RDSEL_LAT_B1));

    vIO32WriteFldMulti(DDRPHY_REG_B0_DQ4, P_Fld(0x29, B0_DQ4_RG_RX_ARDQS_EYE_R_DLY_B0) |
            P_Fld(0x29, B0_DQ4_RG_RX_ARDQS_EYE_F_DLY_B0) | P_Fld(0x1f, B0_DQ4_RG_RX_ARDQ_EYE_R_DLY_B0) |
            P_Fld(0x1f, B0_DQ4_RG_RX_ARDQ_EYE_F_DLY_B0));

    vIO32WriteFldMulti(DDRPHY_REG_B1_DQ4, P_Fld(0x29, B1_DQ4_RG_RX_ARDQS_EYE_R_DLY_B1) |
            P_Fld(0x29, B1_DQ4_RG_RX_ARDQS_EYE_F_DLY_B1) | P_Fld(0x1f, B1_DQ4_RG_RX_ARDQ_EYE_R_DLY_B1) |
            P_Fld(0x1f, B1_DQ4_RG_RX_ARDQ_EYE_F_DLY_B1));

    vIO32WriteFldMulti(DDRPHY_REG_B0_DQ5, P_Fld(0x10, B0_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B0) |
            P_Fld(0x1, B0_DQ5_RG_RX_ARDQ_VREF_EN_B0) | P_Fld(0x1, B0_DQ5_RG_RX_ARDQ_EYE_VREF_EN_B0) |
            P_Fld(0x0, B0_DQ5_RG_RX_ARDQ_EYE_SEL_B0) | P_Fld(0x1, B0_DQ5_RG_RX_ARDQ_EYE_EN_B0) |
            P_Fld(0x1, B0_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B0) | P_Fld(0x0, B0_DQ5_RG_RX_ARDQS0_DVS_EN_B0));

    vIO32WriteFldMulti(DDRPHY_REG_B1_DQ5, P_Fld(0x10, B1_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B1) |
            P_Fld(0x1, B1_DQ5_RG_RX_ARDQ_VREF_EN_B1) | P_Fld(0x1, B1_DQ5_RG_RX_ARDQ_EYE_VREF_EN_B1) |
            P_Fld(0x0, B1_DQ5_RG_RX_ARDQ_EYE_SEL_B1) | P_Fld(0x1, B1_DQ5_RG_RX_ARDQ_EYE_EN_B1) |
            P_Fld(0x1, B1_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B1) | P_Fld(0x0, B1_DQ5_RG_RX_ARDQS0_DVS_EN_B1));

    vIO32WriteFldMulti(DRAMC_REG_SHU_COMMON0, P_Fld(0x1, SHU_COMMON0_FREQDIV4) |
            P_Fld(0x0, SHU_COMMON0_FDIV2) | P_Fld(0x0, SHU_COMMON0_FREQDIV8) |
            P_Fld(0x1, SHU_COMMON0_DM64BITEN) | P_Fld(0x0, SHU_COMMON0_DLE256EN) |
            P_Fld(0x0, SHU_COMMON0_LP5BGEN) | P_Fld(0x0, SHU_COMMON0_LP5WCKON) |
            P_Fld(0x0, SHU_COMMON0_CL2) | P_Fld(0x0, SHU_COMMON0_BL2) |
            P_Fld(0x1, SHU_COMMON0_BL4) | P_Fld(0x0, SHU_COMMON0_LP5BGOTF) |
            P_Fld(0x1, SHU_COMMON0_BC4OTF) | P_Fld(0x0, SHU_COMMON0_LP5HEFF_MODE) |
            P_Fld(0x00000, SHU_COMMON0_SHU_COMMON0_RSV));

    vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIMING_CONF, P_Fld(0x26, SHU_ACTIMING_CONF_SCINTV) |
            P_Fld(0x0, SHU_ACTIMING_CONF_TRFCPBIG) | P_Fld(0x000, SHU_ACTIMING_CONF_REFBW_FR) |
            P_Fld(0x1, SHU_ACTIMING_CONF_TREFBWIG));

    #if (fcFOR_CHIP_ID == fcA60868)
    vIO32WriteFldMulti(DRAMC_REG_SHU_DCM_CTRL0, P_Fld(0x1, SHU_DCM_CTRL0_DDRPHY_CLK_EN_OPT) |
            P_Fld(0x4, SHU_DCM_CTRL0_DPHY_CMDDCM_EXTCNT) | P_Fld(0x5, SHU_DCM_CTRL0_DDRPHY_CLK_DYN_GATING_SEL) |
            P_Fld(0x0, SHU_DCM_CTRL0_CKE_EXTNONPD_CNT) | P_Fld(0x1, SHU_DCM_CTRL0_FASTWAKE2) |
            P_Fld(0x1, SHU_DCM_CTRL0_FASTWAKE));
    #elif (fcFOR_CHIP_ID == fcPetrus)
    vIO32WriteFldMulti(DRAMC_REG_SHU_DCM_CTRL0, P_Fld(0x1, SHU_DCM_CTRL0_DDRPHY_CLK_EN_OPT) |
            P_Fld(0x5, SHU_DCM_CTRL0_DDRPHY_CLK_DYN_GATING_SEL) |
            P_Fld(0x1, SHU_DCM_CTRL0_FASTWAKE2) |
            P_Fld(0x1, SHU_DCM_CTRL0_FASTWAKE));
    #endif

    vIO32WriteFldMulti(DRAMC_REG_SHU_CONF0, P_Fld(0x3f, SHU_CONF0_DMPGTIM) |
            P_Fld(0x0, SHU_CONF0_ADVREFEN) | P_Fld(0x1, SHU_CONF0_ADVPREEN) |
            P_Fld(0x1, SHU_CONF0_PBREFEN) | P_Fld(0x1, SHU_CONF0_REFTHD) |
            P_Fld(0x8, SHU_CONF0_REQQUE_DEPTH));

    vIO32WriteFldMulti(DRAMC_REG_SHU_MATYPE, P_Fld(0x2, SHU_MATYPE_MATYPE) |
            P_Fld(0x1, SHU_MATYPE_NORMPOP_LEN));

    vIO32WriteFldAlign(DRAMC_REG_SHU_SCHEDULER, 0x1, SHU_SCHEDULER_DUALSCHEN);

    vIO32WriteFldMulti(DRAMC_REG_TX_SET0, P_Fld(0x0, TX_SET0_TXRANK) |
            P_Fld(0x0, TX_SET0_TXRANKFIX) | P_Fld(0x0, TX_SET0_DDRPHY_COMB_CG_SEL) |
            P_Fld(0x1, TX_SET0_TX_DQM_DEFAULT) | P_Fld(0x0, TX_SET0_DQBUS_X32) |
            P_Fld(0x0, TX_SET0_OE_DOWNGRADE) | P_Fld(0x0, TX_SET0_DQ16COM1) |
            P_Fld(0x1, TX_SET0_WPRE2T) | P_Fld(0x0, TX_SET0_DRSCLR_EN) |
            P_Fld(0x0, TX_SET0_DRSCLR_RK0_EN) | P_Fld(0x0, TX_SET0_ARPI_CAL_E2OPT) |
            P_Fld(0x0, TX_SET0_TX_DLY_CAL_E2OPT) | P_Fld(0x0, TX_SET0_DQS_OE_OP1_DIS) |
            P_Fld(0x0, TX_SET0_DQS_OE_OP2_EN) | P_Fld(0x0, TX_SET0_RK_SCINPUT_OPT) |
            P_Fld(0x0, TX_SET0_DRAMOEN));

    #if (fcFOR_CHIP_ID == fcA60868)
    vIO32WriteFldMulti(DRAMC_REG_SHU_TX_SET0, P_Fld(0x0, SHU_TX_SET0_DQOE_CNT) |
            P_Fld(0x0, SHU_TX_SET0_DQOE_OPT) | P_Fld(0x0, SHU_TX_SET0_TXUPD_SEL) |
            P_Fld(0x5, SHU_TX_SET0_TXUPD_W2R_SEL) | P_Fld(0x0, SHU_TX_SET0_WECC_EN) |
            P_Fld(0x0, SHU_TX_SET0_DBIWR) | P_Fld(0x0, SHU_TX_SET0_WDATRGO) |
            P_Fld(0x0, SHU_TX_SET0_TWPSTEXT) | P_Fld(0x0, SHU_TX_SET0_WPST1P5T) |
            P_Fld(0x3, SHU_TX_SET0_TXOEN_AUTOSET_OFFSET) | P_Fld(0x1, SHU_TX_SET0_TWCKPST) |
            P_Fld(0x1, SHU_TX_SET0_OE_EXT2UI) | P_Fld(0x0e, SHU_TX_SET0_DQS2DQ_FILT_PITHRD) |
            P_Fld(0x0, SHU_TX_SET0_TXOEN_AUTOSET_EN));
    #elif (fcFOR_CHIP_ID == fcPetrus)
    vIO32WriteFldMulti(DRAMC_REG_SHU_TX_SET0, P_Fld(0x0, SHU_TX_SET0_DQOE_CNT) |
            P_Fld(0x0, SHU_TX_SET0_DQOE_OPT) | P_Fld(0x0, SHU_TX_SET0_TXUPD_SEL) |
            P_Fld(0x5, SHU_TX_SET0_TXUPD_W2R_SEL) | P_Fld(0x0, SHU_TX_SET0_WECC_EN) |
            P_Fld(0x0, SHU_TX_SET0_DBIWR) | P_Fld(0x0, SHU_TX_SET0_WDATRGO) |
            P_Fld(0x0, SHU_TX_SET0_WPST1P5T) |
            P_Fld(0x3, SHU_TX_SET0_TXOEN_AUTOSET_OFFSET) | P_Fld(0x1, SHU_TX_SET0_TWCKPST) |
            P_Fld(0x1, SHU_TX_SET0_OE_EXT2UI) | P_Fld(0x0e, SHU_TX_SET0_DQS2DQ_FILT_PITHRD) |
            P_Fld(0x0, SHU_TX_SET0_TXOEN_AUTOSET_EN));
    #endif

    vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_STBCAL1, P_Fld(0x0, MISC_SHU_STBCAL1_DLLFRZRFCOPT) |
            P_Fld(0x0, MISC_SHU_STBCAL1_DLLFRZWROPT) | P_Fld(0x0, MISC_SHU_STBCAL1_R_RSTBCNT_LATCH_OPT) |
            P_Fld(0x1, MISC_SHU_STBCAL1_STB_UPDMASK_EN) | P_Fld(0x9, MISC_SHU_STBCAL1_STB_UPDMASKCYC) |
            P_Fld(0x0, MISC_SHU_STBCAL1_DQSINCTL_PRE_SEL));

    vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_STBCAL, P_Fld(0x0, MISC_SHU_STBCAL_DMSTBLAT) |
            P_Fld(0x1, MISC_SHU_STBCAL_PICGLAT) | P_Fld(0x1, MISC_SHU_STBCAL_DQSG_MODE) |
            P_Fld(0x1, MISC_SHU_STBCAL_DQSIEN_PICG_MODE) | P_Fld(0x1, MISC_SHU_STBCAL_DQSIEN_DQSSTB_MODE) |
            P_Fld(0x1, MISC_SHU_STBCAL_DQSIEN_BURST_MODE) | P_Fld(0x0, MISC_SHU_STBCAL_DQSIEN_SELPH_FRUN) |
            P_Fld(0x1, MISC_SHU_STBCAL_STBCALEN) | P_Fld(0x1, MISC_SHU_STBCAL_STB_SELPHCALEN) |
            P_Fld(0x0, MISC_SHU_STBCAL_DQSIEN_4TO1_EN) | P_Fld(0x0, MISC_SHU_STBCAL_DQSIEN_8TO1_EN) |
            P_Fld(0x0, MISC_SHU_STBCAL_DQSIEN_16TO1_EN));

    vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RODTENSTB, P_Fld(0x1, MISC_SHU_RODTENSTB_RODTENSTB_TRACK_EN) |
            P_Fld(0x0, MISC_SHU_RODTENSTB_RODTEN_P1_ENABLE) | P_Fld(0x0, MISC_SHU_RODTENSTB_RODTENSTB_4BYTE_EN) |
            P_Fld(0x1, MISC_SHU_RODTENSTB_RODTENSTB_TRACK_UDFLWCTRL) | P_Fld(0x1, MISC_SHU_RODTENSTB_RODTENSTB_SELPH_MODE) |
            P_Fld(0x0, MISC_SHU_RODTENSTB_RODTENSTB_SELPH_BY_BITTIME) | P_Fld(0x4, MISC_SHU_RODTENSTB_RODTENSTB__UI_OFFSET) |
            P_Fld(0x0, MISC_SHU_RODTENSTB_RODTENSTB_MCK_OFFSET) | P_Fld(0x0008, MISC_SHU_RODTENSTB_RODTENSTB_EXT));

    vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RX_SELPH_MODE, P_Fld(0x2, MISC_SHU_RX_SELPH_MODE_DQSIEN_SELPH_SERMODE) |
            P_Fld(0x1, MISC_SHU_RX_SELPH_MODE_RODT_SELPH_SERMODE) | P_Fld(0x1, MISC_SHU_RX_SELPH_MODE_RANK_SELPH_SERMODE));

    vIO32WriteFldMulti(DRAMC_REG_SHU_HWSET_MR13, P_Fld(0x000d, SHU_HWSET_MR13_HWSET_MR13_MRSMA) |
            P_Fld(0x08, SHU_HWSET_MR13_HWSET_MR13_OP));

    vIO32WriteFldMulti(DRAMC_REG_SHU_HWSET_VRCG, P_Fld(0x000d, SHU_HWSET_VRCG_HWSET_VRCG_MRSMA) |
            P_Fld(0x00, SHU_HWSET_VRCG_HWSET_VRCG_OP) | P_Fld(0x00, SHU_HWSET_VRCG_VRCGDIS_PRDCNT));

    vIO32WriteFldMulti(DRAMC_REG_SHU_FREQ_RATIO_SET0, P_Fld(0x20, SHU_FREQ_RATIO_SET0_TDQSCK_JUMP_RATIO3) |
            P_Fld(0x2b, SHU_FREQ_RATIO_SET0_TDQSCK_JUMP_RATIO2) | P_Fld(0x18, SHU_FREQ_RATIO_SET0_TDQSCK_JUMP_RATIO1) |
            P_Fld(0x20, SHU_FREQ_RATIO_SET0_TDQSCK_JUMP_RATIO0));

    vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_DVFSDLL, P_Fld(0x1, MISC_SHU_DVFSDLL_R_BYPASS_1ST_DLL) |
            P_Fld(0x0, MISC_SHU_DVFSDLL_R_BYPASS_2ND_DLL) | P_Fld(0x5a, MISC_SHU_DVFSDLL_R_DLL_IDLE) |
            P_Fld(0x5a, MISC_SHU_DVFSDLL_R_2ND_DLL_IDLE));

        mcDELAY_US(1);

        mcDELAY_US(1);


         DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);

        mcDELAY_US(1);

        mcDELAY_US(1);


         DramcBroadcastOnOff(DRAMC_BROADCAST_ON);

    vIO32WriteFldMulti(DRAMC_REG_SHU_DQSOSCR, P_Fld(0x08, SHU_DQSOSCR_DQSOSCRCNT) |
            P_Fld(0x0, SHU_DQSOSCR_DQSOSC_ADV_SEL) | P_Fld(0x0, SHU_DQSOSCR_DQSOSC_DRS_ADV_SEL) |
            P_Fld(0xffff, SHU_DQSOSCR_DQSOSC_DELTA));

    vIO32WriteFldMulti(DRAMC_REG_SHU_DQSOSC_SET0, P_Fld(0x1, SHU_DQSOSC_SET0_DQSOSCENDIS) |
            P_Fld(0x011, SHU_DQSOSC_SET0_DQSOSC_PRDCNT) | P_Fld(0x0002, SHU_DQSOSC_SET0_DQSOSCENCNT));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_DQSOSC, P_Fld(0x0866, SHURK_DQSOSC_DQSOSC_BASE_RK0) |
            P_Fld(0x0866, SHURK_DQSOSC_DQSOSC_BASE_RK0_B1));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_DQSOSC+(1*0x200), P_Fld(0x0399, SHURK_DQSOSC_DQSOSC_BASE_RK0) |
            P_Fld(0x0399, SHURK_DQSOSC_DQSOSC_BASE_RK0_B1));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_DQSOSC_THRD, P_Fld(0x0ac, SHURK_DQSOSC_THRD_DQSOSCTHRD_INC) |
            P_Fld(0x072, SHURK_DQSOSC_THRD_DQSOSCTHRD_DEC));

    vIO32WriteFldMulti(DRAMC_REG_SHURK_DQSOSC_THRD+(1*0x200), P_Fld(0x01f, SHURK_DQSOSC_THRD_DQSOSCTHRD_INC) |
            P_Fld(0x015, SHURK_DQSOSC_THRD_DQSOSCTHRD_DEC));

    #if (fcFOR_CHIP_ID == fcA60868)
    vIO32WriteFldMulti(DRAMC_REG_SHU_TX_SET0, P_Fld(0x0, SHU_TX_SET0_DQOE_CNT) |
            P_Fld(0x0, SHU_TX_SET0_DQOE_OPT) | P_Fld(0x0, SHU_TX_SET0_TXUPD_SEL) |
            P_Fld(0x5, SHU_TX_SET0_TXUPD_W2R_SEL) | P_Fld(0x0, SHU_TX_SET0_WECC_EN) |
            P_Fld(0x0, SHU_TX_SET0_DBIWR) | P_Fld(0x0, SHU_TX_SET0_WDATRGO) |
            P_Fld(0x0, SHU_TX_SET0_TWPSTEXT) | P_Fld(0x0, SHU_TX_SET0_WPST1P5T) |
            P_Fld(0x3, SHU_TX_SET0_TXOEN_AUTOSET_OFFSET) | P_Fld(0x1, SHU_TX_SET0_TWCKPST) |
            P_Fld(0x1, SHU_TX_SET0_OE_EXT2UI) | P_Fld(0x08, SHU_TX_SET0_DQS2DQ_FILT_PITHRD) |
            P_Fld(0x0, SHU_TX_SET0_TXOEN_AUTOSET_EN));
    #elif (fcFOR_CHIP_ID == fcPetrus)
    vIO32WriteFldMulti(DRAMC_REG_SHU_TX_SET0, P_Fld(0x0, SHU_TX_SET0_DQOE_CNT) |
            P_Fld(0x0, SHU_TX_SET0_DQOE_OPT) | P_Fld(0x0, SHU_TX_SET0_TXUPD_SEL) |
            P_Fld(0x5, SHU_TX_SET0_TXUPD_W2R_SEL) | P_Fld(0x0, SHU_TX_SET0_WECC_EN) |
            P_Fld(0x0, SHU_TX_SET0_DBIWR) | P_Fld(0x0, SHU_TX_SET0_WDATRGO) |
            P_Fld(0x0, SHU_TX_SET0_WPST1P5T) |
            P_Fld(0x3, SHU_TX_SET0_TXOEN_AUTOSET_OFFSET) | P_Fld(0x1, SHU_TX_SET0_TWCKPST) |
            P_Fld(0x1, SHU_TX_SET0_OE_EXT2UI) | P_Fld(0x08, SHU_TX_SET0_DQS2DQ_FILT_PITHRD) |
            P_Fld(0x0, SHU_TX_SET0_TXOEN_AUTOSET_EN));
    #endif

    vIO32WriteFldMulti(DRAMC_REG_SHU_ZQ_SET0, P_Fld(0x0005, SHU_ZQ_SET0_ZQCSCNT) |
            P_Fld(0x1b, SHU_ZQ_SET0_TZQLAT));

    vIO32WriteFldMulti(DRAMC_REG_SHU_HMR4_DVFS_CTRL0, P_Fld(0x32, SHU_HMR4_DVFS_CTRL0_FSPCHG_PRDCNT) |
            P_Fld(0x005, SHU_HMR4_DVFS_CTRL0_REFRCNT));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ8, P_Fld(0x0063, SHU_B0_DQ8_R_DMRXDVS_UPD_FORCE_CYC_B0) |
            P_Fld(0x1, SHU_B0_DQ8_R_DMRXDVS_UPD_FORCE_EN_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B0) |
            P_Fld(0x0, SHU_B0_DQ8_R_RMRODTEN_CG_IG_B0) | P_Fld(0x1, SHU_B0_DQ8_R_RMRX_TOPHY_CG_IG_B0) |
            P_Fld(0x0, SHU_B0_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B0) |
            P_Fld(0x1, SHU_B0_DQ8_R_DMRXDLY_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B0) |
            P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B0) |
            P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMRANK_PIPE_CG_IG_B0) |
            P_Fld(0x0, SHU_B0_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ8, P_Fld(0x0063, SHU_B1_DQ8_R_DMRXDVS_UPD_FORCE_CYC_B1) |
            P_Fld(0x1, SHU_B1_DQ8_R_DMRXDVS_UPD_FORCE_EN_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B1) |
            P_Fld(0x0, SHU_B1_DQ8_R_RMRODTEN_CG_IG_B1) | P_Fld(0x1, SHU_B1_DQ8_R_RMRX_TOPHY_CG_IG_B1) |
            P_Fld(0x0, SHU_B1_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B1) |
            P_Fld(0x1, SHU_B1_DQ8_R_DMRXDLY_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B1) |
            P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B1) |
            P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMRANK_PIPE_CG_IG_B1) |
            P_Fld(0x0, SHU_B1_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ7, P_Fld(0x0, SHU_B0_DQ7_R_DMRANKRXDVS_B0) |
            P_Fld(0x0, SHU_B0_DQ7_R_DMDQMDBI_EYE_SHU_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMDQMDBI_SHU_B0) |
            P_Fld(0xd, SHU_B0_DQ7_R_DMRXDVS_DQM_FLAGSEL_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B0) |
            P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMRXTRACK_DQM_EN_B0) |
            P_Fld(0x1, SHU_B0_DQ7_R_DMRODTEN_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMARPI_CG_FB2DLL_DCM_EN_B0) |
            P_Fld(0x0, SHU_B0_DQ7_R_DMTX_ARPI_CG_DQ_NEW_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMTX_ARPI_CG_DQS_NEW_B0) |
            P_Fld(0x0, SHU_B0_DQ7_R_DMTX_ARPI_CG_DQM_NEW_B0) | P_Fld(0x0, SHU_B0_DQ7_R_LP4Y_SDN_MODE_DQS0) |
            P_Fld(0x1, SHU_B0_DQ7_R_DMRXRANK_DQ_EN_B0) | P_Fld(0x1, SHU_B0_DQ7_R_DMRXRANK_DQ_LAT_B0) |
            P_Fld(0x1, SHU_B0_DQ7_R_DMRXRANK_DQS_EN_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMRXRANK_DQS_LAT_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ7, P_Fld(0x0, SHU_B1_DQ7_R_DMRANKRXDVS_B1) |
            P_Fld(0x0, SHU_B1_DQ7_R_DMDQMDBI_EYE_SHU_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMDQMDBI_SHU_B1) |
            P_Fld(0xd, SHU_B1_DQ7_R_DMRXDVS_DQM_FLAGSEL_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B1) |
            P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMRXTRACK_DQM_EN_B1) |
            P_Fld(0x1, SHU_B1_DQ7_R_DMRODTEN_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMARPI_CG_FB2DLL_DCM_EN_B1) |
            P_Fld(0x0, SHU_B1_DQ7_R_DMTX_ARPI_CG_DQ_NEW_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMTX_ARPI_CG_DQS_NEW_B1) |
            P_Fld(0x0, SHU_B1_DQ7_R_DMTX_ARPI_CG_DQM_NEW_B1) | P_Fld(0x0, SHU_B1_DQ7_R_LP4Y_SDN_MODE_DQS1) |
            P_Fld(0x1, SHU_B1_DQ7_R_DMRXRANK_DQ_EN_B1) | P_Fld(0x1, SHU_B1_DQ7_R_DMRXRANK_DQ_LAT_B1) |
            P_Fld(0x1, SHU_B1_DQ7_R_DMRXRANK_DQS_EN_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMRXRANK_DQS_LAT_B1));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ11, P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_RANK_SEL_SER_EN_B0) |
            P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_RANK_SEL_LAT_EN_B0) | P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_OFFSETC_LAT_EN_B0) |
            P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_OFFSETC_EN_B0) | P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_OFFSETC_BIAS_EN_B0) |
            P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_FRATE_EN_B0) | P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_CDR_EN_B0) |
            P_Fld(0x1, SHU_B0_DQ11_RG_RX_ARDQ_DVS_EN_B0) | P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_DVS_DLY_B0) |
            P_Fld(0x2, SHU_B0_DQ11_RG_RX_ARDQ_DES_MODE_B0) | P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_BW_SEL_B0));

    vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ11, P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_RANK_SEL_SER_EN_B1) |
            P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_RANK_SEL_LAT_EN_B1) | P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_OFFSETC_LAT_EN_B1) |
            P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_OFFSETC_EN_B1) | P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_OFFSETC_BIAS_EN_B1) |
            P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_FRATE_EN_B1) | P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_CDR_EN_B1) |
            P_Fld(0x1, SHU_B1_DQ11_RG_RX_ARDQ_DVS_EN_B1) | P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_DVS_DLY_B1) |
            P_Fld(0x2, SHU_B1_DQ11_RG_RX_ARDQ_DES_MODE_B1) | P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_BW_SEL_B1));

}

#if 0
void CInit_golden_mini_freq_related_vseq_LP4_1600_SHU1(DRAMC_CTX_T *p)
{

vIO32WriteFldMulti(DDRPHY_REG_SHU_MISC_DRVING1+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x09, SHU_MISC_DRVING1_DQDRVN2) |
        P_Fld(0x07, SHU_MISC_DRVING1_DQDRVP2) | P_Fld(0x09, SHU_MISC_DRVING1_DQSDRVN1) |
        P_Fld(0x07, SHU_MISC_DRVING1_DQSDRVP1) | P_Fld(0x09, SHU_MISC_DRVING1_DQSDRVN2) |
        P_Fld(0x07, SHU_MISC_DRVING1_DQSDRVP2) | P_Fld(0x0, SHU_MISC_DRVING1_DIS_IMP_ODTN_TRACK) |
        P_Fld(0x0, SHU_MISC_DRVING1_DIS_IMPCAL_HW));
vIO32WriteFldMulti(DDRPHY_REG_SHU_MISC_DRVING2+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x09, SHU_MISC_DRVING2_CMDDRVN1) |
        P_Fld(0x07, SHU_MISC_DRVING2_CMDDRVP1) | P_Fld(0x09, SHU_MISC_DRVING2_CMDDRVN2) |
        P_Fld(0x07, SHU_MISC_DRVING2_CMDDRVP2) | P_Fld(0x09, SHU_MISC_DRVING2_DQDRVN1) |
        P_Fld(0x07, SHU_MISC_DRVING2_DQDRVP1) | P_Fld(0x1, SHU_MISC_DRVING2_DIS_IMPCAL_ODT_EN));
vIO32WriteFldMulti(DDRPHY_REG_SHU_MISC_DRVING3+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x0a, SHU_MISC_DRVING3_DQODTN2) |
        P_Fld(0x0a, SHU_MISC_DRVING3_DQODTP2) | P_Fld(0x0a, SHU_MISC_DRVING3_DQSODTN) |
        P_Fld(0x0a, SHU_MISC_DRVING3_DQSODTP) | P_Fld(0x0a, SHU_MISC_DRVING3_DQSODTN2) |
        P_Fld(0x0a, SHU_MISC_DRVING3_DQSODTP2));
vIO32WriteFldMulti(DDRPHY_REG_SHU_MISC_DRVING4+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x0a, SHU_MISC_DRVING4_CMDODTN1) |
        P_Fld(0x0a, SHU_MISC_DRVING4_CMDODTP1) | P_Fld(0x0a, SHU_MISC_DRVING4_CMDODTN2) |
        P_Fld(0x0a, SHU_MISC_DRVING4_CMDODTP2) | P_Fld(0x0a, SHU_MISC_DRVING4_DQODTN1) |
        P_Fld(0x0a, SHU_MISC_DRVING4_DQODTP1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_MISC_DRVING6+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x07, SHU_MISC_DRVING6_IMP_TXDLY_CMD) |
        P_Fld(0x00, SHU_MISC_DRVING6_DQCODTN1) | P_Fld(0x00, SHU_MISC_DRVING6_DQCODTP1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_MISC_IMPCAL1+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x4, SHU_MISC_IMPCAL1_IMPCAL_CHKCYCLE) |
        P_Fld(0x00, SHU_MISC_IMPCAL1_IMPDRVP) | P_Fld(0x00, SHU_MISC_IMPCAL1_IMPDRVN) |
        P_Fld(0x4, SHU_MISC_IMPCAL1_IMPCAL_CALEN_CYCLE) | P_Fld(0x03, SHU_MISC_IMPCAL1_IMPCALCNT) |
        P_Fld(0x8, SHU_MISC_IMPCAL1_IMPCAL_CALICNT));

    mcDELAY_US(1);

    mcDELAY_US(1);

     DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);

vIO32WriteFldMulti(DDRPHY_REG_SHU_CA_CMD6+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x00, SHU_CA_CMD6_RG_ARPI_OFFSET_DQSIEN_CA) |
        P_Fld(0x00, SHU_CA_CMD6_RG_ARPI_OFFSET_MCTL_CA) | P_Fld(0x00, SHU_CA_CMD6_RG_ARPI_CAP_SEL_CA) |
        P_Fld(0x0, SHU_CA_CMD6_RG_ARPI_SOPEN_EN_CA) | P_Fld(0x0, SHU_CA_CMD6_RG_ARPI_OPEN_EN_CA) |
        P_Fld(0x0, SHU_CA_CMD6_RG_ARPI_HYST_SEL_CA) | P_Fld(0x0, SHU_CA_CMD6_RG_ARPI_BUFGP_XLATCH_FORCE_CA_CA) |
        P_Fld(0x0, SHU_CA_CMD6_RG_ARPI_BUFGP_XLATCH_FORCE_CLK_CA) | P_Fld(0x0, SHU_CA_CMD6_RG_ARPI_SOPEN_CKGEN_EN_CA) |
        P_Fld(0x0, SHU_CA_CMD6_RG_ARPI_SOPEN_CKGEN_DIV_CA) | P_Fld(0x0, SHU_CA_CMD6_RG_ARPI_DDR400_EN_CA) |
        P_Fld(0x1, SHU_CA_CMD6_RG_RX_ARCMD_RANK_SEL_SER_MODE));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ6+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x00, SHU_B0_DQ6_RG_ARPI_OFFSET_DQSIEN_B0) |
        P_Fld(0x00, SHU_B0_DQ6_RG_ARPI_OFFSET_MCTL_B0) | P_Fld(0x00, SHU_B0_DQ6_RG_ARPI_CAP_SEL_B0) |
        P_Fld(0x0, SHU_B0_DQ6_RG_ARPI_SOPEN_EN_B0) | P_Fld(0x0, SHU_B0_DQ6_RG_ARPI_OPEN_EN_B0) |
        P_Fld(0x0, SHU_B0_DQ6_RG_ARPI_HYST_SEL_B0) | P_Fld(0x0, SHU_B0_DQ6_RG_ARPI_BUFGP_XLATCH_FORCE_DQ_B0) |
        P_Fld(0x0, SHU_B0_DQ6_RG_ARPI_BUFGP_XLATCH_FORCE_DQS_B0) | P_Fld(0x0, SHU_B0_DQ6_RG_ARPI_SOPEN_CKGEN_EN_B0) |
        P_Fld(0x0, SHU_B0_DQ6_RG_ARPI_SOPEN_CKGEN_DIV_B0) | P_Fld(0x0, SHU_B0_DQ6_RG_ARPI_DDR400_EN_B0) |
        P_Fld(0x1, SHU_B0_DQ6_RG_RX_ARDQ_RANK_SEL_SER_MODE_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ6+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x00, SHU_B1_DQ6_RG_ARPI_OFFSET_DQSIEN_B1) |
        P_Fld(0x00, SHU_B1_DQ6_RG_ARPI_OFFSET_MCTL_B1) | P_Fld(0x00, SHU_B1_DQ6_RG_ARPI_CAP_SEL_B1) |
        P_Fld(0x0, SHU_B1_DQ6_RG_ARPI_SOPEN_EN_B1) | P_Fld(0x0, SHU_B1_DQ6_RG_ARPI_OPEN_EN_B1) |
        P_Fld(0x0, SHU_B1_DQ6_RG_ARPI_HYST_SEL_B1) | P_Fld(0x0, SHU_B1_DQ6_RG_ARPI_BUFGP_XLATCH_FORCE_DQ_B1) |
        P_Fld(0x0, SHU_B1_DQ6_RG_ARPI_BUFGP_XLATCH_FORCE_DQS_B1) | P_Fld(0x0, SHU_B1_DQ6_RG_ARPI_SOPEN_CKGEN_EN_B1) |
        P_Fld(0x0, SHU_B1_DQ6_RG_ARPI_SOPEN_CKGEN_DIV_B1) | P_Fld(0x0, SHU_B1_DQ6_RG_ARPI_DDR400_EN_B1) |
        P_Fld(0x1, SHU_B1_DQ6_RG_RX_ARDQ_RANK_SEL_SER_MODE_B1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_CA_CMD1+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x0, SHU_CA_CMD1_RG_ARPI_MIDPI_EN_CA) |
        P_Fld(0x0, SHU_CA_CMD1_RG_ARPI_MIDPI_CKDIV4_PREDIV_EN_CA) | P_Fld(0x1, SHU_CA_CMD1_RG_ARPI_MIDPI_CKDIV4_EN_CA) |
        P_Fld(0x15, SHU_CA_CMD1_RG_ARPI_MIDPI_8PH_DLY_CA) | P_Fld(0x0, SHU_CA_CMD1_RG_ARPI_MIDPI_LDO_VREF_SEL_CA) |
        P_Fld(0x3, SHU_CA_CMD1_RG_ARPI_MIDPI_CAP_SEL_CA) | P_Fld(0x0, SHU_CA_CMD1_RG_ARPI_MIDPI_VTH_SEL_CA) |
        P_Fld(0x0, SHU_CA_CMD1_RG_ARPI_8PHASE_XLATCH_FORCE_CA) | P_Fld(0x0, SHU_CA_CMD1_RG_ARPI_MIDPI_DUMMY_EN_CA) |
        P_Fld(0x0, SHU_CA_CMD1_RG_ARPI_MIDPI_BYPASS_EN_CA));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ1+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x0, SHU_B0_DQ1_RG_ARPI_MIDPI_EN_B0) |
        P_Fld(0x0, SHU_B0_DQ1_RG_ARPI_MIDPI_CKDIV4_PREDIV_EN_B0) | P_Fld(0x1, SHU_B0_DQ1_RG_ARPI_MIDPI_CKDIV4_EN_B0) |
        P_Fld(0x0e, SHU_B0_DQ1_RG_ARPI_MIDPI_8PH_DLY_B0) | P_Fld(0x0, SHU_B0_DQ1_RG_ARPI_MIDPI_LDO_VREF_SEL_B0) |
        P_Fld(0x3, SHU_B0_DQ1_RG_ARPI_MIDPI_CAP_SEL_B0) | P_Fld(0x0, SHU_B0_DQ1_RG_ARPI_MIDPI_VTH_SEL_B0) |
        P_Fld(0x0, SHU_B0_DQ1_RG_ARPI_8PHASE_XLATCH_FORCE_B0) | P_Fld(0x0, SHU_B0_DQ1_RG_ARPI_MIDPI_DUMMY_EN_B0) |
        P_Fld(0x0, SHU_B0_DQ1_RG_ARPI_MIDPI_BYPASS_EN_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ1+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x0, SHU_B1_DQ1_RG_ARPI_MIDPI_EN_B1) |
        P_Fld(0x0, SHU_B1_DQ1_RG_ARPI_MIDPI_CKDIV4_PREDIV_EN_B1) | P_Fld(0x1, SHU_B1_DQ1_RG_ARPI_MIDPI_CKDIV4_EN_B1) |
        P_Fld(0x0a, SHU_B1_DQ1_RG_ARPI_MIDPI_8PH_DLY_B1) | P_Fld(0x0, SHU_B1_DQ1_RG_ARPI_MIDPI_LDO_VREF_SEL_B1) |
        P_Fld(0x3, SHU_B1_DQ1_RG_ARPI_MIDPI_CAP_SEL_B1) | P_Fld(0x0, SHU_B1_DQ1_RG_ARPI_MIDPI_VTH_SEL_B1) |
        P_Fld(0x0, SHU_B1_DQ1_RG_ARPI_8PHASE_XLATCH_FORCE_B1) | P_Fld(0x0, SHU_B1_DQ1_RG_ARPI_MIDPI_DUMMY_EN_B1) |
        P_Fld(0x0, SHU_B1_DQ1_RG_ARPI_MIDPI_BYPASS_EN_B1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_CA_CMD14+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x1, SHU_CA_CMD14_RG_TX_ARCA_SER_MODE_CA) |
        P_Fld(0x0, SHU_CA_CMD14_RG_TX_ARCA_AUX_SER_MODE_CA) | P_Fld(0x0, SHU_CA_CMD14_RG_TX_ARCA_PRE_DATA_SEL_CA) |
        P_Fld(0x0, SHU_CA_CMD14_RG_TX_ARCA_OE_ODTEN_SWAP_CA) | P_Fld(0x0, SHU_CA_CMD14_RG_TX_ARCA_OE_ODTEN_CG_EN_CA) |
        P_Fld(0x50, SHU_CA_CMD14_RG_TX_ARCA_MCKIO_SEL_CA));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ10+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x0, SHU_B0_DQ10_RG_RX_ARDQS_SE_EN_B0) |
        P_Fld(0x0, SHU_B0_DQ10_RG_RX_ARDQS_DQSSTB_CG_EN_B0) | P_Fld(0x1, SHU_B0_DQ10_RG_RX_ARDQS_DQSIEN_RANK_SEL_LAT_EN_B0) |
        P_Fld(0x1, SHU_B0_DQ10_RG_RX_ARDQS_RANK_SEL_LAT_EN_B0) | P_Fld(0x0, SHU_B0_DQ10_RG_RX_ARDQS_DQSSTB_RPST_HS_EN_B0) |
        P_Fld(0x1, SHU_B0_DQ10_RG_RX_ARDQS_DQSIEN_MODE_B0) | P_Fld(0x1, SHU_B0_DQ10_RG_RX_ARDQS_DLY_LAT_EN_B0) |
        P_Fld(0x0, SHU_B0_DQ10_RG_RX_ARDQS_DIFF_SWAP_EN_B0) | P_Fld(0x0, SHU_B0_DQ10_RG_RX_ARDQS_BW_SEL_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ14+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x0, SHU_B0_DQ14_RG_TX_ARWCK_PRE_EN_B0) |
        P_Fld(0x0, SHU_B0_DQ14_RG_TX_ARWCK_PRE_DATA_SEL_B0) | P_Fld(0x0, SHU_B0_DQ14_RG_TX_ARWCK_MCKIO_SEL_B0) |
        P_Fld(0x1, SHU_B0_DQ14_RG_TX_ARDQ_SER_MODE_B0) | P_Fld(0x0, SHU_B0_DQ14_RG_TX_ARDQ_AUX_SER_MODE_B0) |
        P_Fld(0x0, SHU_B0_DQ14_RG_TX_ARDQ_PRE_DATA_SEL_B0) | P_Fld(0x0, SHU_B0_DQ14_RG_TX_ARDQ_OE_ODTEN_SWAP_B0) |
        P_Fld(0x1, SHU_B0_DQ14_RG_TX_ARDQ_OE_ODTEN_CG_EN_B0) | P_Fld(0x00, SHU_B0_DQ14_RG_TX_ARDQ_MCKIO_SEL_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ10+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x0, SHU_B1_DQ10_RG_RX_ARDQS_SE_EN_B1) |
        P_Fld(0x0, SHU_B1_DQ10_RG_RX_ARDQS_DQSSTB_CG_EN_B1) | P_Fld(0x1, SHU_B1_DQ10_RG_RX_ARDQS_DQSIEN_RANK_SEL_LAT_EN_B1) |
        P_Fld(0x1, SHU_B1_DQ10_RG_RX_ARDQS_RANK_SEL_LAT_EN_B1) | P_Fld(0x0, SHU_B1_DQ10_RG_RX_ARDQS_DQSSTB_RPST_HS_EN_B1) |
        P_Fld(0x1, SHU_B1_DQ10_RG_RX_ARDQS_DQSIEN_MODE_B1) | P_Fld(0x1, SHU_B1_DQ10_RG_RX_ARDQS_DLY_LAT_EN_B1) |
        P_Fld(0x0, SHU_B1_DQ10_RG_RX_ARDQS_DIFF_SWAP_EN_B1) | P_Fld(0x0, SHU_B1_DQ10_RG_RX_ARDQS_BW_SEL_B1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ14+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x0, SHU_B1_DQ14_RG_TX_ARWCK_PRE_EN_B1) |
        P_Fld(0x0, SHU_B1_DQ14_RG_TX_ARWCK_PRE_DATA_SEL_B1) | P_Fld(0x0, SHU_B1_DQ14_RG_TX_ARWCK_MCKIO_SEL_B1) |
        P_Fld(0x1, SHU_B1_DQ14_RG_TX_ARDQ_SER_MODE_B1) | P_Fld(0x0, SHU_B1_DQ14_RG_TX_ARDQ_AUX_SER_MODE_B1) |
        P_Fld(0x0, SHU_B1_DQ14_RG_TX_ARDQ_PRE_DATA_SEL_B1) | P_Fld(0x0, SHU_B1_DQ14_RG_TX_ARDQ_OE_ODTEN_SWAP_B1) |
        P_Fld(0x1, SHU_B1_DQ14_RG_TX_ARDQ_OE_ODTEN_CG_EN_B1) | P_Fld(0x00, SHU_B1_DQ14_RG_TX_ARDQ_MCKIO_SEL_B1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_CA_DLL0+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x0, SHU_CA_DLL0_RG_ARDLL_LCK_DET_LV2_DLY_CA) |
        P_Fld(0x0, SHU_CA_DLL0_RG_ARDLL_LCK_DET_LV1_DLY_CA) | P_Fld(0x0, SHU_CA_DLL0_RG_ARDLL_LCK_DET_EN_CA) |
        P_Fld(0x7, SHU_CA_DLL0_RG_ARDLL_IDLECNT_CA) | P_Fld(0x0, SHU_CA_DLL0_RG_ARDLL_HOLD_ON_LOCK_EN_CA) |
        P_Fld(0x0, SHU_CA_DLL0_RG_ARDLL_GAIN_BOOST_CA) | P_Fld(0x7, SHU_CA_DLL0_RG_ARDLL_GAIN_CA) |
        P_Fld(0x0, SHU_CA_DLL0_RG_ARDLL_FAST_DIV_EN_CA) | P_Fld(0x0, SHU_CA_DLL0_RG_ARDLL_FAST_PSJP_CA) |
        P_Fld(0x1, SHU_CA_DLL0_RG_ARDLL_FASTPJ_CK_SEL_CA) | P_Fld(0x0, SHU_CA_DLL0_RG_ARDLL_GEAR2_PSJP_CA));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DLL0+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x0, SHU_B0_DLL0_RG_ARDLL_LCK_DET_LV2_DLY_B0) |
        P_Fld(0x0, SHU_B0_DLL0_RG_ARDLL_LCK_DET_LV1_DLY_B0) | P_Fld(0x0, SHU_B0_DLL0_RG_ARDLL_LCK_DET_EN_B0) |
        P_Fld(0x7, SHU_B0_DLL0_RG_ARDLL_IDLECNT_B0) | P_Fld(0x0, SHU_B0_DLL0_RG_ARDLL_HOLD_ON_LOCK_EN_B0) |
        P_Fld(0x0, SHU_B0_DLL0_RG_ARDLL_GAIN_BOOST_B0) | P_Fld(0x7, SHU_B0_DLL0_RG_ARDLL_GAIN_B0) |
        P_Fld(0x0, SHU_B0_DLL0_RG_ARDLL_FAST_DIV_EN_B0) | P_Fld(0x0, SHU_B0_DLL0_RG_ARDLL_FAST_PSJP_B0) |
        P_Fld(0x1, SHU_B0_DLL0_RG_ARDLL_FASTPJ_CK_SEL_B0) | P_Fld(0x0, SHU_B0_DLL0_RG_ARDLL_GEAR2_PSJP_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DLL0+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x0, SHU_B1_DLL0_RG_ARDLL_LCK_DET_LV2_DLY_B1) |
        P_Fld(0x0, SHU_B1_DLL0_RG_ARDLL_LCK_DET_LV1_DLY_B1) | P_Fld(0x0, SHU_B1_DLL0_RG_ARDLL_LCK_DET_EN_B1) |
        P_Fld(0x7, SHU_B1_DLL0_RG_ARDLL_IDLECNT_B1) | P_Fld(0x0, SHU_B1_DLL0_RG_ARDLL_HOLD_ON_LOCK_EN_B1) |
        P_Fld(0x0, SHU_B1_DLL0_RG_ARDLL_GAIN_BOOST_B1) | P_Fld(0x7, SHU_B1_DLL0_RG_ARDLL_GAIN_B1) |
        P_Fld(0x0, SHU_B1_DLL0_RG_ARDLL_FAST_DIV_EN_B1) | P_Fld(0x0, SHU_B1_DLL0_RG_ARDLL_FAST_PSJP_B1) |
        P_Fld(0x1, SHU_B1_DLL0_RG_ARDLL_FASTPJ_CK_SEL_B1) | P_Fld(0x0, SHU_B1_DLL0_RG_ARDLL_GEAR2_PSJP_B1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_PLL0+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x894a, SHU_PLL0_RG_RPHYPLL_TOP_REV) |
        P_Fld(0x0, SHU_PLL0_RG_RPLLGP_SOPEN_SER_MODE) | P_Fld(0x0, SHU_PLL0_RG_RPLLGP_SOPEN_PREDIV_EN) |
        P_Fld(0x0, SHU_PLL0_RG_RPLLGP_SOPEN_EN) | P_Fld(0x0, SHU_PLL0_RG_RPLLGP_DLINE_MON_TSHIFT) |
        P_Fld(0x0, SHU_PLL0_RG_RPLLGP_DLINE_MON_DIV) | P_Fld(0x00, SHU_PLL0_RG_RPLLGP_DLINE_MON_DLY) |
        P_Fld(0x0, SHU_PLL0_RG_RPLLGP_DLINE_MON_EN));
vIO32WriteFldMulti(DDRPHY_REG_SHU_PLL1+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x1, SHU_PLL1_RG_RPHYPLLGP_CK_SEL) |
        P_Fld(0x0, SHU_PLL1_RG_RPLLGP_PLLCK_VSEL) | P_Fld(0x1, SHU_PLL1_R_SHU_AUTO_PLL_MUX) |
        P_Fld(0x0, SHU_PLL1_RG_RPHYPLL_DDR400_EN));
vIO32WriteFldAlign(DDRPHY_REG_SHU_PLL2+(1*SHU_GRP_DDRPHY_OFFSET), 0x1, SHU_PLL2_RG_RPHYPLL_ADA_MCK8X_EN_SHU);
vIO32WriteFldMulti(DDRPHY_REG_SHU_PHYPLL0+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x00fe, SHU_PHYPLL0_RG_RPHYPLL_RESERVED) |
        P_Fld(0x2, SHU_PHYPLL0_RG_RPHYPLL_FS) | P_Fld(0x7, SHU_PHYPLL0_RG_RPHYPLL_BW) |
        P_Fld(0x1, SHU_PHYPLL0_RG_RPHYPLL_ICHP) | P_Fld(0x1, SHU_PHYPLL0_RG_RPHYPLL_IBIAS) |
        P_Fld(0x1, SHU_PHYPLL0_RG_RPHYPLL_BLP) | P_Fld(0x1, SHU_PHYPLL0_RG_RPHYPLL_BR) |
        P_Fld(0x1, SHU_PHYPLL0_RG_RPHYPLL_BP));
vIO32WriteFldMulti(DDRPHY_REG_SHU_PHYPLL1+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x0, SHU_PHYPLL1_RG_RPHYPLL_SDM_FRA_EN) |
        P_Fld(0x1, SHU_PHYPLL1_RG_RPHYPLL_SDM_PCW_CHG) | P_Fld(0x5b00, SHU_PHYPLL1_RG_RPHYPLL_SDM_PCW));
vIO32WriteFldMulti(DDRPHY_REG_SHU_PHYPLL2+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x0, SHU_PHYPLL2_RG_RPHYPLL_POSDIV) |
        P_Fld(0x1, SHU_PHYPLL2_RG_RPHYPLL_PREDIV));
vIO32WriteFldMulti(DDRPHY_REG_SHU_PHYPLL3+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x1, SHU_PHYPLL3_RG_RPHYPLL_DIV_CK_SEL) |
        P_Fld(0x1, SHU_PHYPLL3_RG_RPHYPLL_GLITCH_FREE_EN) | P_Fld(0x0, SHU_PHYPLL3_RG_RPHYPLL_LVR_REFSEL) |
        P_Fld(0x0, SHU_PHYPLL3_RG_RPHYPLL_DIV3_EN) | P_Fld(0x1, SHU_PHYPLL3_RG_RPHYPLL_FS_EN) |
        P_Fld(0x0, SHU_PHYPLL3_RG_RPHYPLL_FBKSEL) | P_Fld(0x2, SHU_PHYPLL3_RG_RPHYPLL_RST_DLY) |
        P_Fld(0x1, SHU_PHYPLL3_RG_RPHYPLL_LVROD_EN) | P_Fld(0x0, SHU_PHYPLL3_RG_RPHYPLL_MONREF_EN) |
        P_Fld(0x0, SHU_PHYPLL3_RG_RPHYPLL_MONVC_EN) | P_Fld(0x0, SHU_PHYPLL3_RG_RPHYPLL_MONCK_EN));
vIO32WriteFldMulti(DDRPHY_REG_SHU_CLRPLL0+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x00fe, SHU_CLRPLL0_RG_RCLRPLL_RESERVED) |
        P_Fld(0x2, SHU_CLRPLL0_RG_RCLRPLL_FS) | P_Fld(0x7, SHU_CLRPLL0_RG_RCLRPLL_BW) |
        P_Fld(0x1, SHU_CLRPLL0_RG_RCLRPLL_ICHP) | P_Fld(0x1, SHU_CLRPLL0_RG_RCLRPLL_IBIAS) |
        P_Fld(0x1, SHU_CLRPLL0_RG_RCLRPLL_BLP) | P_Fld(0x1, SHU_CLRPLL0_RG_RCLRPLL_BR) |
        P_Fld(0x1, SHU_CLRPLL0_RG_RCLRPLL_BP));
vIO32WriteFldMulti(DDRPHY_REG_SHU_CLRPLL1+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x0, SHU_CLRPLL1_RG_RCLRPLL_SDM_FRA_EN) |
        P_Fld(0x1, SHU_CLRPLL1_RG_RCLRPLL_SDM_PCW_CHG) | P_Fld(0x5b00, SHU_CLRPLL1_RG_RCLRPLL_SDM_PCW));
vIO32WriteFldMulti(DDRPHY_REG_SHU_CLRPLL2+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x0, SHU_CLRPLL2_RG_RCLRPLL_POSDIV) |
        P_Fld(0x1, SHU_CLRPLL2_RG_RCLRPLL_PREDIV));
vIO32WriteFldMulti(DDRPHY_REG_SHU_CA_CMD5+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x00, SHU_CA_CMD5_RG_RX_ARCMD_VREF_SEL) |
        P_Fld(0x0, SHU_CA_CMD5_RG_RX_ARCMD_VREF_BYPASS) | P_Fld(0x00, SHU_CA_CMD5_RG_ARPI_FB_CA) |
        P_Fld(0x0, SHU_CA_CMD5_RG_RX_ARCLK_DQSIEN_DLY) | P_Fld(0x0, SHU_CA_CMD5_RG_RX_ARCLK_DQSIEN_RB_DLY) |
        P_Fld(0x0, SHU_CA_CMD5_RG_RX_ARCLK_DVS_DLY) | P_Fld(0x0, SHU_CA_CMD5_RG_RX_ARCMD_FIFO_DQSI_DLY));
vIO32WriteFldAlign(DDRPHY_REG_SHU_CA_CMD9+(1*SHU_GRP_DDRPHY_OFFSET), 0x716c0638, SHU_CA_CMD9_RG_ARPI_RESERVE_CA);
vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_MIDPI_CTRL+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x0, MISC_SHU_MIDPI_CTRL_MIDPI_ENABLE) |
        P_Fld(0x1, MISC_SHU_MIDPI_CTRL_MIDPI_DIV4_ENABLE));
vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RDAT1+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x1, MISC_SHU_RDAT1_R_DMRDSEL_DIV2_OPT) |
        P_Fld(0x1, MISC_SHU_RDAT1_R_DMRDSEL_LOBYTE_OPT) | P_Fld(0x0, MISC_SHU_RDAT1_R_DMRDSEL_HIBYTE_OPT) |
        P_Fld(0x0, MISC_SHU_RDAT1_RDATDIV2) | P_Fld(0x1, MISC_SHU_RDAT1_RDATDIV4));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ13+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x1, SHU_B0_DQ13_RG_TX_ARDQ_IO_ODT_DIS_B0) |
        P_Fld(0x0, SHU_B0_DQ13_RG_TX_ARDQ_FRATE_EN_B0) | P_Fld(0x0, SHU_B0_DQ13_RG_TX_ARDQ_DLY_LAT_EN_B0) |
        P_Fld(0x1, SHU_B0_DQ13_RG_TX_ARDQS_READ_BASE_EN_B0) | P_Fld(0x0, SHU_B0_DQ13_RG_TX_ARDQS_PRE_DATA_SEL_B0) |
        P_Fld(0x0, SHU_B0_DQ13_RG_TX_ARDQS_OE_ODTEN_SWAP_B0) | P_Fld(0x1, SHU_B0_DQ13_RG_TX_ARDQS_OE_ODTEN_CG_EN_B0) |
        P_Fld(0x0, SHU_B0_DQ13_RG_TX_ARDQS_MCKIO_CG_B0) | P_Fld(0x0, SHU_B0_DQ13_RG_TX_ARDQS_MCKIO_SEL_B0) |
        P_Fld(0x0, SHU_B0_DQ13_RG_TX_ARDQM_MCKIO_SEL_B0) | P_Fld(0x1, SHU_B0_DQ13_RG_TX_ARDQSB_READ_BASE_EN_B0) |
        P_Fld(0x1, SHU_B0_DQ13_RG_TX_ARDQM_OE_ODTEN_CG_EN_B0) | P_Fld(0x1, SHU_B0_DQ13_RG_TX_ARDQSB_READ_BASE_DATA_TIE_EN_B0) |
        P_Fld(0x1, SHU_B0_DQ13_RG_TX_ARDQS_READ_BASE_DATA_TIE_EN_B0) | P_Fld(0x0, SHU_B0_DQ13_RG_TX_ARDQ_READ_BASE_EN_B0) |
        P_Fld(0x0, SHU_B0_DQ13_RG_TX_ARDQ_READ_BASE_DATA_TIE_EN_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ13+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x1, SHU_B1_DQ13_RG_TX_ARDQ_IO_ODT_DIS_B1) |
        P_Fld(0x0, SHU_B1_DQ13_RG_TX_ARDQ_FRATE_EN_B1) | P_Fld(0x0, SHU_B1_DQ13_RG_TX_ARDQ_DLY_LAT_EN_B1) |
        P_Fld(0x1, SHU_B1_DQ13_RG_TX_ARDQS_READ_BASE_EN_B1) | P_Fld(0x0, SHU_B1_DQ13_RG_TX_ARDQS_PRE_DATA_SEL_B1) |
        P_Fld(0x0, SHU_B1_DQ13_RG_TX_ARDQS_OE_ODTEN_SWAP_B1) | P_Fld(0x1, SHU_B1_DQ13_RG_TX_ARDQS_OE_ODTEN_CG_EN_B1) |
        P_Fld(0x0, SHU_B1_DQ13_RG_TX_ARDQS_MCKIO_CG_B1) | P_Fld(0x0, SHU_B1_DQ13_RG_TX_ARDQS_MCKIO_SEL_B1) |
        P_Fld(0x0, SHU_B1_DQ13_RG_TX_ARDQM_MCKIO_SEL_B1) | P_Fld(0x1, SHU_B1_DQ13_RG_TX_ARDQSB_READ_BASE_EN_B1) |
        P_Fld(0x1, SHU_B1_DQ13_RG_TX_ARDQM_OE_ODTEN_CG_EN_B1) | P_Fld(0x1, SHU_B1_DQ13_RG_TX_ARDQSB_READ_BASE_DATA_TIE_EN_B1) |
        P_Fld(0x1, SHU_B1_DQ13_RG_TX_ARDQS_READ_BASE_DATA_TIE_EN_B1) | P_Fld(0x0, SHU_B1_DQ13_RG_TX_ARDQ_READ_BASE_EN_B1) |
        P_Fld(0x0, SHU_B1_DQ13_RG_TX_ARDQ_READ_BASE_DATA_TIE_EN_B1));
vIO32WriteFldAlign(DDRPHY_REG_SHU_B0_DQ9+(1*SHU_GRP_DDRPHY_OFFSET), 0xd4f91bb7, SHU_B0_DQ9_RG_ARPI_RESERVE_B0);
vIO32WriteFldAlign(DDRPHY_REG_SHU_B1_DQ9+(1*SHU_GRP_DDRPHY_OFFSET), 0x53b8f566, SHU_B1_DQ9_RG_ARPI_RESERVE_B1);
vIO32WriteFldMulti(DDRPHY_REG_SHU_CA_CMD7+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x0, SHU_CA_CMD7_R_DMRANKRXDVS_CA) |
        P_Fld(0x0, SHU_CA_CMD7_R_DMRXDVS_PBYTE_FLAG_OPT_CA) | P_Fld(0x0, SHU_CA_CMD7_R_DMRODTEN_CA) |
        P_Fld(0x0, SHU_CA_CMD7_R_DMARPI_CG_FB2DLL_DCM_EN_CA) | P_Fld(0x1, SHU_CA_CMD7_R_DMTX_ARPI_CG_CMD_NEW) |
        P_Fld(0x1, SHU_CA_CMD7_R_DMTX_ARPI_CG_CS_NEW) | P_Fld(0x0, SHU_CA_CMD7_R_LP4Y_SDN_MODE_CLK) |
        P_Fld(0x0, SHU_CA_CMD7_R_DMRXRANK_CMD_EN) | P_Fld(0x0, SHU_CA_CMD7_R_DMRXRANK_CMD_LAT) |
        P_Fld(0x0, SHU_CA_CMD7_R_DMRXRANK_CLK_EN) | P_Fld(0x0, SHU_CA_CMD7_R_DMRXRANK_CLK_LAT));
vIO32WriteFldMulti(DDRPHY_REG_SHU_CA_DLL1+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x0, SHU_CA_DLL1_RG_ARDLL_AD_ARFB_CK_EN_CA) |
        P_Fld(0x3, SHU_CA_DLL1_RG_ARDLL_DIV_MODE_CA) | P_Fld(0x1, SHU_CA_DLL1_RG_ARDLL_UDIV_EN_CA) |
        P_Fld(0x0, SHU_CA_DLL1_RG_ARDLL_UPDATE_ON_IDLE_EN_CA) | P_Fld(0x0, SHU_CA_DLL1_RG_ARDLL_TRACKING_CA_EN_CA) |
        P_Fld(0x0, SHU_CA_DLL1_RG_ARDLL_UPDATE_ON_IDLE_MODE_CA) | P_Fld(0x1, SHU_CA_DLL1_RG_ARDLL_SER_MODE_CA) |
        P_Fld(0x1, SHU_CA_DLL1_RG_ARDLL_PS_EN_CA) | P_Fld(0x1, SHU_CA_DLL1_RG_ARDLL_PSJP_EN_CA) |
        P_Fld(0x1, SHU_CA_DLL1_RG_ARDLL_PHDIV_CA) | P_Fld(0x1, SHU_CA_DLL1_RG_ARDLL_PHDET_OUT_SEL_CA) |
        P_Fld(0x1, SHU_CA_DLL1_RG_ARDLL_PHDET_IN_SWAP_CA) | P_Fld(0x1, SHU_CA_DLL1_RG_ARDLL_PHDET_EN_CA) |
        P_Fld(0x0, SHU_CA_DLL1_RG_ARDLL_DIV_MCTL_CA) | P_Fld(0x0, SHU_CA_DLL1_RG_ARDLL_PGAIN_CA) |
        P_Fld(0x0, SHU_CA_DLL1_RG_ARDLL_PD_CK_SEL_CA));
vIO32WriteFldAlign(DDRPHY_REG_SHU_CA_DLL2+(1*SHU_GRP_DDRPHY_OFFSET), 0x15ba2788, SHU_CA_DLL2_RG_ARCMD_REV);
vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DLL1+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x0, SHU_B0_DLL1_RG_ARDLL_AD_ARFB_CK_EN_B0) |
        P_Fld(0x3, SHU_B0_DLL1_RG_ARDLL_DIV_MODE_B0) | P_Fld(0x1, SHU_B0_DLL1_RG_ARDLL_UDIV_EN_B0) |
        P_Fld(0x0, SHU_B0_DLL1_RG_ARDLL_UPDATE_ON_IDLE_EN_B0) | P_Fld(0x0, SHU_B0_DLL1_RG_ARDLL_TRACKING_CA_EN_B0) |
        P_Fld(0x0, SHU_B0_DLL1_RG_ARDLL_UPDATE_ON_IDLE_MODE_B0) | P_Fld(0x1, SHU_B0_DLL1_RG_ARDLL_SER_MODE_B0) |
        P_Fld(0x1, SHU_B0_DLL1_RG_ARDLL_PS_EN_B0) | P_Fld(0x1, SHU_B0_DLL1_RG_ARDLL_PSJP_EN_B0) |
        P_Fld(0x1, SHU_B0_DLL1_RG_ARDLL_PHDIV_B0) | P_Fld(0x1, SHU_B0_DLL1_RG_ARDLL_PHDET_OUT_SEL_B0) |
        P_Fld(0x1, SHU_B0_DLL1_RG_ARDLL_PHDET_IN_SWAP_B0) | P_Fld(0x1, SHU_B0_DLL1_RG_ARDLL_PHDET_EN_B0) |
        P_Fld(0x0, SHU_B0_DLL1_RG_ARDLL_DIV_MCTL_B0) | P_Fld(0x0, SHU_B0_DLL1_RG_ARDLL_PGAIN_B0) |
        P_Fld(0x0, SHU_B0_DLL1_RG_ARDLL_PD_CK_SEL_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DLL1+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x0, SHU_B1_DLL1_RG_ARDLL_AD_ARFB_CK_EN_B1) |
        P_Fld(0x3, SHU_B1_DLL1_RG_ARDLL_DIV_MODE_B1) | P_Fld(0x1, SHU_B1_DLL1_RG_ARDLL_UDIV_EN_B1) |
        P_Fld(0x0, SHU_B1_DLL1_RG_ARDLL_UPDATE_ON_IDLE_EN_B1) | P_Fld(0x0, SHU_B1_DLL1_RG_ARDLL_TRACKING_CA_EN_B1) |
        P_Fld(0x0, SHU_B1_DLL1_RG_ARDLL_UPDATE_ON_IDLE_MODE_B1) | P_Fld(0x1, SHU_B1_DLL1_RG_ARDLL_SER_MODE_B1) |
        P_Fld(0x1, SHU_B1_DLL1_RG_ARDLL_PS_EN_B1) | P_Fld(0x1, SHU_B1_DLL1_RG_ARDLL_PSJP_EN_B1) |
        P_Fld(0x1, SHU_B1_DLL1_RG_ARDLL_PHDIV_B1) | P_Fld(0x1, SHU_B1_DLL1_RG_ARDLL_PHDET_OUT_SEL_B1) |
        P_Fld(0x1, SHU_B1_DLL1_RG_ARDLL_PHDET_IN_SWAP_B1) | P_Fld(0x1, SHU_B1_DLL1_RG_ARDLL_PHDET_EN_B1) |
        P_Fld(0x0, SHU_B1_DLL1_RG_ARDLL_DIV_MCTL_B1) | P_Fld(0x0, SHU_B1_DLL1_RG_ARDLL_PGAIN_B1) |
        P_Fld(0x0, SHU_B1_DLL1_RG_ARDLL_PD_CK_SEL_B1));
vIO32WriteFldAlign(DDRPHY_REG_SHU_B0_DLL2+(1*SHU_GRP_DDRPHY_OFFSET), 0x0cebb192, SHU_B0_DLL2_RG_ARDQ_REV_B0);
vIO32WriteFldAlign(DDRPHY_REG_SHU_B1_DLL2+(1*SHU_GRP_DDRPHY_OFFSET), 0x4b99094e, SHU_B1_DLL2_RG_ARDQ_REV_B1);
vIO32WriteFldMulti(DDRPHY_REG_SHU_CA_CMD11+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x0, SHU_CA_CMD11_RG_RX_ARCA_RANK_SEL_SER_EN_CA) |
        P_Fld(0x0, SHU_CA_CMD11_RG_RX_ARCA_RANK_SEL_LAT_EN_CA) | P_Fld(0x0, SHU_CA_CMD11_RG_RX_ARCA_OFFSETC_LAT_EN_CA) |
        P_Fld(0x0, SHU_CA_CMD11_RG_RX_ARCA_OFFSETC_EN_CA) | P_Fld(0x0, SHU_CA_CMD11_RG_RX_ARCA_OFFSETC_BIAS_EN_CA) |
        P_Fld(0x0, SHU_CA_CMD11_RG_RX_ARCA_FRATE_EN_CA) | P_Fld(0x0, SHU_CA_CMD11_RG_RX_ARCA_CDR_EN_CA) |
        P_Fld(0x0, SHU_CA_CMD11_RG_RX_ARCA_DVS_EN_CA) | P_Fld(0x0, SHU_CA_CMD11_RG_RX_ARCA_DVS_DLY_CA) |
        P_Fld(0x2, SHU_CA_CMD11_RG_RX_ARCA_DES_MODE_CA) | P_Fld(0x0, SHU_CA_CMD11_RG_RX_ARCA_BW_SEL_CA));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ11+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x1, SHU_B0_DQ11_RG_RX_ARDQ_RANK_SEL_SER_EN_B0) |
        P_Fld(0x1, SHU_B0_DQ11_RG_RX_ARDQ_RANK_SEL_LAT_EN_B0) | P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_OFFSETC_LAT_EN_B0) |
        P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_OFFSETC_EN_B0) | P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_OFFSETC_BIAS_EN_B0) |
        P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_FRATE_EN_B0) | P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_CDR_EN_B0) |
        P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_DVS_EN_B0) | P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_DVS_DLY_B0) |
        P_Fld(0x2, SHU_B0_DQ11_RG_RX_ARDQ_DES_MODE_B0) | P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_BW_SEL_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ11+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x1, SHU_B1_DQ11_RG_RX_ARDQ_RANK_SEL_SER_EN_B1) |
        P_Fld(0x1, SHU_B1_DQ11_RG_RX_ARDQ_RANK_SEL_LAT_EN_B1) | P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_OFFSETC_LAT_EN_B1) |
        P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_OFFSETC_EN_B1) | P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_OFFSETC_BIAS_EN_B1) |
        P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_FRATE_EN_B1) | P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_CDR_EN_B1) |
        P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_DVS_EN_B1) | P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_DVS_DLY_B1) |
        P_Fld(0x2, SHU_B1_DQ11_RG_RX_ARDQ_DES_MODE_B1) | P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_BW_SEL_B1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_CA_CMD2+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x1, SHU_CA_CMD2_RG_ARPI_TX_CG_SYNC_DIS_CA) |
        P_Fld(0x1, SHU_CA_CMD2_RG_ARPI_TX_CG_CA_EN_CA) | P_Fld(0x1, SHU_CA_CMD2_RG_ARPI_TX_CG_CLK_EN_CA) |
        P_Fld(0x1, SHU_CA_CMD2_RG_ARPI_TX_CG_CS_EN_CA) | P_Fld(0x0, SHU_CA_CMD2_RG_ARPI_SMT_XLATCH_FORCE_CLK_CA) |
        P_Fld(0x0, SHU_CA_CMD2_RG_ARPI_SMT_XLATCH_CA_FORCE_CA) | P_Fld(0x0, SHU_CA_CMD2_RG_ARPI_PSMUX_XLATCH_FORCE_CA_CA) |
        P_Fld(0x0, SHU_CA_CMD2_RG_ARPI_PSMUX_XLATCH_FORCE_CLK_CA) | P_Fld(0x1, SHU_CA_CMD2_RG_ARPISM_MCK_SEL_CA_SHU) |
        P_Fld(0x0, SHU_CA_CMD2_RG_ARPI_PD_MCTL_SEL_CA) | P_Fld(0x0, SHU_CA_CMD2_RG_ARPI_OFFSET_LAT_EN_CA) |
        P_Fld(0x0, SHU_CA_CMD2_RG_ARPI_OFFSET_ASYNC_EN_CA));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ2+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x1, SHU_B0_DQ2_RG_ARPI_TX_CG_SYNC_DIS_B0) |
        P_Fld(0x1, SHU_B0_DQ2_RG_ARPI_TX_CG_DQ_EN_B0) | P_Fld(0x1, SHU_B0_DQ2_RG_ARPI_TX_CG_DQS_EN_B0) |
        P_Fld(0x1, SHU_B0_DQ2_RG_ARPI_TX_CG_DQM_EN_B0) | P_Fld(0x0, SHU_B0_DQ2_RG_ARPI_SMT_XLATCH_FORCE_DQS_B0) |
        P_Fld(0x0, SHU_B0_DQ2_RG_ARPI_SMT_XLATCH_DQ_FORCE_B0) | P_Fld(0x0, SHU_B0_DQ2_RG_ARPI_PSMUX_XLATCH_FORCE_DQ_B0) |
        P_Fld(0x0, SHU_B0_DQ2_RG_ARPI_PSMUX_XLATCH_FORCE_DQS_B0) | P_Fld(0x1, SHU_B0_DQ2_RG_ARPISM_MCK_SEL_B0_SHU) |
        P_Fld(0x0, SHU_B0_DQ2_RG_ARPI_PD_MCTL_SEL_B0) | P_Fld(0x1, SHU_B0_DQ2_RG_ARPI_OFFSET_LAT_EN_B0) |
        P_Fld(0x1, SHU_B0_DQ2_RG_ARPI_OFFSET_ASYNC_EN_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ2+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x1, SHU_B1_DQ2_RG_ARPI_TX_CG_SYNC_DIS_B1) |
        P_Fld(0x1, SHU_B1_DQ2_RG_ARPI_TX_CG_DQ_EN_B1) | P_Fld(0x1, SHU_B1_DQ2_RG_ARPI_TX_CG_DQS_EN_B1) |
        P_Fld(0x1, SHU_B1_DQ2_RG_ARPI_TX_CG_DQM_EN_B1) | P_Fld(0x0, SHU_B1_DQ2_RG_ARPI_SMT_XLATCH_FORCE_DQS_B1) |
        P_Fld(0x0, SHU_B1_DQ2_RG_ARPI_SMT_XLATCH_DQ_FORCE_B1) | P_Fld(0x0, SHU_B1_DQ2_RG_ARPI_PSMUX_XLATCH_FORCE_DQ_B1) |
        P_Fld(0x0, SHU_B1_DQ2_RG_ARPI_PSMUX_XLATCH_FORCE_DQS_B1) | P_Fld(0x1, SHU_B1_DQ2_RG_ARPISM_MCK_SEL_B1_SHU) |
        P_Fld(0x0, SHU_B1_DQ2_RG_ARPI_PD_MCTL_SEL_B1) | P_Fld(0x1, SHU_B1_DQ2_RG_ARPI_OFFSET_LAT_EN_B1) |
        P_Fld(0x1, SHU_B1_DQ2_RG_ARPI_OFFSET_ASYNC_EN_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_CA_CMD6+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), P_Fld(0x00, SHU_CA_CMD6_RG_ARPI_OFFSET_DQSIEN_CA) |
        P_Fld(0x00, SHU_CA_CMD6_RG_ARPI_OFFSET_MCTL_CA) | P_Fld(0x00, SHU_CA_CMD6_RG_ARPI_CAP_SEL_CA) |
        P_Fld(0x0, SHU_CA_CMD6_RG_ARPI_SOPEN_EN_CA) | P_Fld(0x0, SHU_CA_CMD6_RG_ARPI_OPEN_EN_CA) |
        P_Fld(0x0, SHU_CA_CMD6_RG_ARPI_HYST_SEL_CA) | P_Fld(0x0, SHU_CA_CMD6_RG_ARPI_BUFGP_XLATCH_FORCE_CA_CA) |
        P_Fld(0x0, SHU_CA_CMD6_RG_ARPI_BUFGP_XLATCH_FORCE_CLK_CA) | P_Fld(0x0, SHU_CA_CMD6_RG_ARPI_SOPEN_CKGEN_EN_CA) |
        P_Fld(0x0, SHU_CA_CMD6_RG_ARPI_SOPEN_CKGEN_DIV_CA) | P_Fld(0x0, SHU_CA_CMD6_RG_ARPI_DDR400_EN_CA) |
        P_Fld(0x1, SHU_CA_CMD6_RG_RX_ARCMD_RANK_SEL_SER_MODE));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ6+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), P_Fld(0x00, SHU_B0_DQ6_RG_ARPI_OFFSET_DQSIEN_B0) |
        P_Fld(0x00, SHU_B0_DQ6_RG_ARPI_OFFSET_MCTL_B0) | P_Fld(0x00, SHU_B0_DQ6_RG_ARPI_CAP_SEL_B0) |
        P_Fld(0x0, SHU_B0_DQ6_RG_ARPI_SOPEN_EN_B0) | P_Fld(0x0, SHU_B0_DQ6_RG_ARPI_OPEN_EN_B0) |
        P_Fld(0x0, SHU_B0_DQ6_RG_ARPI_HYST_SEL_B0) | P_Fld(0x0, SHU_B0_DQ6_RG_ARPI_BUFGP_XLATCH_FORCE_DQ_B0) |
        P_Fld(0x0, SHU_B0_DQ6_RG_ARPI_BUFGP_XLATCH_FORCE_DQS_B0) | P_Fld(0x0, SHU_B0_DQ6_RG_ARPI_SOPEN_CKGEN_EN_B0) |
        P_Fld(0x0, SHU_B0_DQ6_RG_ARPI_SOPEN_CKGEN_DIV_B0) | P_Fld(0x0, SHU_B0_DQ6_RG_ARPI_DDR400_EN_B0) |
        P_Fld(0x1, SHU_B0_DQ6_RG_RX_ARDQ_RANK_SEL_SER_MODE_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ6+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), P_Fld(0x00, SHU_B1_DQ6_RG_ARPI_OFFSET_DQSIEN_B1) |
        P_Fld(0x00, SHU_B1_DQ6_RG_ARPI_OFFSET_MCTL_B1) | P_Fld(0x00, SHU_B1_DQ6_RG_ARPI_CAP_SEL_B1) |
        P_Fld(0x0, SHU_B1_DQ6_RG_ARPI_SOPEN_EN_B1) | P_Fld(0x0, SHU_B1_DQ6_RG_ARPI_OPEN_EN_B1) |
        P_Fld(0x0, SHU_B1_DQ6_RG_ARPI_HYST_SEL_B1) | P_Fld(0x0, SHU_B1_DQ6_RG_ARPI_BUFGP_XLATCH_FORCE_DQ_B1) |
        P_Fld(0x0, SHU_B1_DQ6_RG_ARPI_BUFGP_XLATCH_FORCE_DQS_B1) | P_Fld(0x0, SHU_B1_DQ6_RG_ARPI_SOPEN_CKGEN_EN_B1) |
        P_Fld(0x0, SHU_B1_DQ6_RG_ARPI_SOPEN_CKGEN_DIV_B1) | P_Fld(0x0, SHU_B1_DQ6_RG_ARPI_DDR400_EN_B1) |
        P_Fld(0x1, SHU_B1_DQ6_RG_RX_ARDQ_RANK_SEL_SER_MODE_B1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_CA_CMD1+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), P_Fld(0x0, SHU_CA_CMD1_RG_ARPI_MIDPI_EN_CA) |
        P_Fld(0x0, SHU_CA_CMD1_RG_ARPI_MIDPI_CKDIV4_PREDIV_EN_CA) | P_Fld(0x1, SHU_CA_CMD1_RG_ARPI_MIDPI_CKDIV4_EN_CA) |
        P_Fld(0x03, SHU_CA_CMD1_RG_ARPI_MIDPI_8PH_DLY_CA) | P_Fld(0x0, SHU_CA_CMD1_RG_ARPI_MIDPI_LDO_VREF_SEL_CA) |
        P_Fld(0x3, SHU_CA_CMD1_RG_ARPI_MIDPI_CAP_SEL_CA) | P_Fld(0x0, SHU_CA_CMD1_RG_ARPI_MIDPI_VTH_SEL_CA) |
        P_Fld(0x0, SHU_CA_CMD1_RG_ARPI_8PHASE_XLATCH_FORCE_CA) | P_Fld(0x0, SHU_CA_CMD1_RG_ARPI_MIDPI_DUMMY_EN_CA) |
        P_Fld(0x0, SHU_CA_CMD1_RG_ARPI_MIDPI_BYPASS_EN_CA));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ1+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), P_Fld(0x0, SHU_B0_DQ1_RG_ARPI_MIDPI_EN_B0) |
        P_Fld(0x0, SHU_B0_DQ1_RG_ARPI_MIDPI_CKDIV4_PREDIV_EN_B0) | P_Fld(0x1, SHU_B0_DQ1_RG_ARPI_MIDPI_CKDIV4_EN_B0) |
        P_Fld(0x05, SHU_B0_DQ1_RG_ARPI_MIDPI_8PH_DLY_B0) | P_Fld(0x0, SHU_B0_DQ1_RG_ARPI_MIDPI_LDO_VREF_SEL_B0) |
        P_Fld(0x3, SHU_B0_DQ1_RG_ARPI_MIDPI_CAP_SEL_B0) | P_Fld(0x0, SHU_B0_DQ1_RG_ARPI_MIDPI_VTH_SEL_B0) |
        P_Fld(0x0, SHU_B0_DQ1_RG_ARPI_8PHASE_XLATCH_FORCE_B0) | P_Fld(0x0, SHU_B0_DQ1_RG_ARPI_MIDPI_DUMMY_EN_B0) |
        P_Fld(0x0, SHU_B0_DQ1_RG_ARPI_MIDPI_BYPASS_EN_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ1+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), P_Fld(0x0, SHU_B1_DQ1_RG_ARPI_MIDPI_EN_B1) |
        P_Fld(0x0, SHU_B1_DQ1_RG_ARPI_MIDPI_CKDIV4_PREDIV_EN_B1) | P_Fld(0x1, SHU_B1_DQ1_RG_ARPI_MIDPI_CKDIV4_EN_B1) |
        P_Fld(0x07, SHU_B1_DQ1_RG_ARPI_MIDPI_8PH_DLY_B1) | P_Fld(0x0, SHU_B1_DQ1_RG_ARPI_MIDPI_LDO_VREF_SEL_B1) |
        P_Fld(0x3, SHU_B1_DQ1_RG_ARPI_MIDPI_CAP_SEL_B1) | P_Fld(0x0, SHU_B1_DQ1_RG_ARPI_MIDPI_VTH_SEL_B1) |
        P_Fld(0x0, SHU_B1_DQ1_RG_ARPI_8PHASE_XLATCH_FORCE_B1) | P_Fld(0x0, SHU_B1_DQ1_RG_ARPI_MIDPI_DUMMY_EN_B1) |
        P_Fld(0x0, SHU_B1_DQ1_RG_ARPI_MIDPI_BYPASS_EN_B1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_CA_CMD14+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), P_Fld(0x1, SHU_CA_CMD14_RG_TX_ARCA_SER_MODE_CA) |
        P_Fld(0x0, SHU_CA_CMD14_RG_TX_ARCA_AUX_SER_MODE_CA) | P_Fld(0x0, SHU_CA_CMD14_RG_TX_ARCA_PRE_DATA_SEL_CA) |
        P_Fld(0x0, SHU_CA_CMD14_RG_TX_ARCA_OE_ODTEN_SWAP_CA) | P_Fld(0x0, SHU_CA_CMD14_RG_TX_ARCA_OE_ODTEN_CG_EN_CA) |
        P_Fld(0x50, SHU_CA_CMD14_RG_TX_ARCA_MCKIO_SEL_CA));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ10+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), P_Fld(0x0, SHU_B0_DQ10_RG_RX_ARDQS_SE_EN_B0) |
        P_Fld(0x0, SHU_B0_DQ10_RG_RX_ARDQS_DQSSTB_CG_EN_B0) | P_Fld(0x1, SHU_B0_DQ10_RG_RX_ARDQS_DQSIEN_RANK_SEL_LAT_EN_B0) |
        P_Fld(0x1, SHU_B0_DQ10_RG_RX_ARDQS_RANK_SEL_LAT_EN_B0) | P_Fld(0x0, SHU_B0_DQ10_RG_RX_ARDQS_DQSSTB_RPST_HS_EN_B0) |
        P_Fld(0x1, SHU_B0_DQ10_RG_RX_ARDQS_DQSIEN_MODE_B0) | P_Fld(0x1, SHU_B0_DQ10_RG_RX_ARDQS_DLY_LAT_EN_B0) |
        P_Fld(0x0, SHU_B0_DQ10_RG_RX_ARDQS_DIFF_SWAP_EN_B0) | P_Fld(0x0, SHU_B0_DQ10_RG_RX_ARDQS_BW_SEL_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ14+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), P_Fld(0x0, SHU_B0_DQ14_RG_TX_ARWCK_PRE_EN_B0) |
        P_Fld(0x0, SHU_B0_DQ14_RG_TX_ARWCK_PRE_DATA_SEL_B0) | P_Fld(0x0, SHU_B0_DQ14_RG_TX_ARWCK_MCKIO_SEL_B0) |
        P_Fld(0x1, SHU_B0_DQ14_RG_TX_ARDQ_SER_MODE_B0) | P_Fld(0x0, SHU_B0_DQ14_RG_TX_ARDQ_AUX_SER_MODE_B0) |
        P_Fld(0x0, SHU_B0_DQ14_RG_TX_ARDQ_PRE_DATA_SEL_B0) | P_Fld(0x0, SHU_B0_DQ14_RG_TX_ARDQ_OE_ODTEN_SWAP_B0) |
        P_Fld(0x1, SHU_B0_DQ14_RG_TX_ARDQ_OE_ODTEN_CG_EN_B0) | P_Fld(0x00, SHU_B0_DQ14_RG_TX_ARDQ_MCKIO_SEL_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ10+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), P_Fld(0x0, SHU_B1_DQ10_RG_RX_ARDQS_SE_EN_B1) |
        P_Fld(0x0, SHU_B1_DQ10_RG_RX_ARDQS_DQSSTB_CG_EN_B1) | P_Fld(0x1, SHU_B1_DQ10_RG_RX_ARDQS_DQSIEN_RANK_SEL_LAT_EN_B1) |
        P_Fld(0x1, SHU_B1_DQ10_RG_RX_ARDQS_RANK_SEL_LAT_EN_B1) | P_Fld(0x0, SHU_B1_DQ10_RG_RX_ARDQS_DQSSTB_RPST_HS_EN_B1) |
        P_Fld(0x1, SHU_B1_DQ10_RG_RX_ARDQS_DQSIEN_MODE_B1) | P_Fld(0x1, SHU_B1_DQ10_RG_RX_ARDQS_DLY_LAT_EN_B1) |
        P_Fld(0x0, SHU_B1_DQ10_RG_RX_ARDQS_DIFF_SWAP_EN_B1) | P_Fld(0x0, SHU_B1_DQ10_RG_RX_ARDQS_BW_SEL_B1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ14+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), P_Fld(0x0, SHU_B1_DQ14_RG_TX_ARWCK_PRE_EN_B1) |
        P_Fld(0x0, SHU_B1_DQ14_RG_TX_ARWCK_PRE_DATA_SEL_B1) | P_Fld(0x0, SHU_B1_DQ14_RG_TX_ARWCK_MCKIO_SEL_B1) |
        P_Fld(0x1, SHU_B1_DQ14_RG_TX_ARDQ_SER_MODE_B1) | P_Fld(0x0, SHU_B1_DQ14_RG_TX_ARDQ_AUX_SER_MODE_B1) |
        P_Fld(0x0, SHU_B1_DQ14_RG_TX_ARDQ_PRE_DATA_SEL_B1) | P_Fld(0x0, SHU_B1_DQ14_RG_TX_ARDQ_OE_ODTEN_SWAP_B1) |
        P_Fld(0x1, SHU_B1_DQ14_RG_TX_ARDQ_OE_ODTEN_CG_EN_B1) | P_Fld(0x00, SHU_B1_DQ14_RG_TX_ARDQ_MCKIO_SEL_B1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_CA_DLL0+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), P_Fld(0x0, SHU_CA_DLL0_RG_ARDLL_LCK_DET_LV2_DLY_CA) |
        P_Fld(0x0, SHU_CA_DLL0_RG_ARDLL_LCK_DET_LV1_DLY_CA) | P_Fld(0x0, SHU_CA_DLL0_RG_ARDLL_LCK_DET_EN_CA) |
        P_Fld(0x7, SHU_CA_DLL0_RG_ARDLL_IDLECNT_CA) | P_Fld(0x0, SHU_CA_DLL0_RG_ARDLL_HOLD_ON_LOCK_EN_CA) |
        P_Fld(0x0, SHU_CA_DLL0_RG_ARDLL_GAIN_BOOST_CA) | P_Fld(0x7, SHU_CA_DLL0_RG_ARDLL_GAIN_CA) |
        P_Fld(0x0, SHU_CA_DLL0_RG_ARDLL_FAST_DIV_EN_CA) | P_Fld(0x0, SHU_CA_DLL0_RG_ARDLL_FAST_PSJP_CA) |
        P_Fld(0x1, SHU_CA_DLL0_RG_ARDLL_FASTPJ_CK_SEL_CA) | P_Fld(0x0, SHU_CA_DLL0_RG_ARDLL_GEAR2_PSJP_CA));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DLL0+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), P_Fld(0x0, SHU_B0_DLL0_RG_ARDLL_LCK_DET_LV2_DLY_B0) |
        P_Fld(0x0, SHU_B0_DLL0_RG_ARDLL_LCK_DET_LV1_DLY_B0) | P_Fld(0x0, SHU_B0_DLL0_RG_ARDLL_LCK_DET_EN_B0) |
        P_Fld(0x7, SHU_B0_DLL0_RG_ARDLL_IDLECNT_B0) | P_Fld(0x0, SHU_B0_DLL0_RG_ARDLL_HOLD_ON_LOCK_EN_B0) |
        P_Fld(0x0, SHU_B0_DLL0_RG_ARDLL_GAIN_BOOST_B0) | P_Fld(0x7, SHU_B0_DLL0_RG_ARDLL_GAIN_B0) |
        P_Fld(0x0, SHU_B0_DLL0_RG_ARDLL_FAST_DIV_EN_B0) | P_Fld(0x0, SHU_B0_DLL0_RG_ARDLL_FAST_PSJP_B0) |
        P_Fld(0x1, SHU_B0_DLL0_RG_ARDLL_FASTPJ_CK_SEL_B0) | P_Fld(0x0, SHU_B0_DLL0_RG_ARDLL_GEAR2_PSJP_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DLL0+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), P_Fld(0x0, SHU_B1_DLL0_RG_ARDLL_LCK_DET_LV2_DLY_B1) |
        P_Fld(0x0, SHU_B1_DLL0_RG_ARDLL_LCK_DET_LV1_DLY_B1) | P_Fld(0x0, SHU_B1_DLL0_RG_ARDLL_LCK_DET_EN_B1) |
        P_Fld(0x7, SHU_B1_DLL0_RG_ARDLL_IDLECNT_B1) | P_Fld(0x0, SHU_B1_DLL0_RG_ARDLL_HOLD_ON_LOCK_EN_B1) |
        P_Fld(0x0, SHU_B1_DLL0_RG_ARDLL_GAIN_BOOST_B1) | P_Fld(0x7, SHU_B1_DLL0_RG_ARDLL_GAIN_B1) |
        P_Fld(0x0, SHU_B1_DLL0_RG_ARDLL_FAST_DIV_EN_B1) | P_Fld(0x0, SHU_B1_DLL0_RG_ARDLL_FAST_PSJP_B1) |
        P_Fld(0x1, SHU_B1_DLL0_RG_ARDLL_FASTPJ_CK_SEL_B1) | P_Fld(0x0, SHU_B1_DLL0_RG_ARDLL_GEAR2_PSJP_B1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_PLL0+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), P_Fld(0x0825, SHU_PLL0_RG_RPHYPLL_TOP_REV) |
        P_Fld(0x0, SHU_PLL0_RG_RPLLGP_SOPEN_SER_MODE) | P_Fld(0x0, SHU_PLL0_RG_RPLLGP_SOPEN_PREDIV_EN) |
        P_Fld(0x0, SHU_PLL0_RG_RPLLGP_SOPEN_EN) | P_Fld(0x0, SHU_PLL0_RG_RPLLGP_DLINE_MON_TSHIFT) |
        P_Fld(0x0, SHU_PLL0_RG_RPLLGP_DLINE_MON_DIV) | P_Fld(0x00, SHU_PLL0_RG_RPLLGP_DLINE_MON_DLY) |
        P_Fld(0x0, SHU_PLL0_RG_RPLLGP_DLINE_MON_EN));
vIO32WriteFldMulti(DDRPHY_REG_SHU_PLL1+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), P_Fld(0x1, SHU_PLL1_RG_RPHYPLLGP_CK_SEL) |
        P_Fld(0x0, SHU_PLL1_RG_RPLLGP_PLLCK_VSEL) | P_Fld(0x1, SHU_PLL1_R_SHU_AUTO_PLL_MUX) |
        P_Fld(0x0, SHU_PLL1_RG_RPHYPLL_DDR400_EN));
vIO32WriteFldAlign(DDRPHY_REG_SHU_PLL2+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), 0x1, SHU_PLL2_RG_RPHYPLL_ADA_MCK8X_EN_SHU);
vIO32WriteFldMulti(DDRPHY_REG_SHU_PHYPLL0+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), P_Fld(0x00fe, SHU_PHYPLL0_RG_RPHYPLL_RESERVED) |
        P_Fld(0x2, SHU_PHYPLL0_RG_RPHYPLL_FS) | P_Fld(0x7, SHU_PHYPLL0_RG_RPHYPLL_BW) |
        P_Fld(0x1, SHU_PHYPLL0_RG_RPHYPLL_ICHP) | P_Fld(0x1, SHU_PHYPLL0_RG_RPHYPLL_IBIAS) |
        P_Fld(0x1, SHU_PHYPLL0_RG_RPHYPLL_BLP) | P_Fld(0x1, SHU_PHYPLL0_RG_RPHYPLL_BR) |
        P_Fld(0x1, SHU_PHYPLL0_RG_RPHYPLL_BP));
vIO32WriteFldMulti(DDRPHY_REG_SHU_PHYPLL1+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), P_Fld(0x0, SHU_PHYPLL1_RG_RPHYPLL_SDM_FRA_EN) |
        P_Fld(0x1, SHU_PHYPLL1_RG_RPHYPLL_SDM_PCW_CHG) | P_Fld(0x5b00, SHU_PHYPLL1_RG_RPHYPLL_SDM_PCW));
vIO32WriteFldMulti(DDRPHY_REG_SHU_PHYPLL2+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), P_Fld(0x0, SHU_PHYPLL2_RG_RPHYPLL_POSDIV) |
        P_Fld(0x1, SHU_PHYPLL2_RG_RPHYPLL_PREDIV));
vIO32WriteFldMulti(DDRPHY_REG_SHU_PHYPLL3+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), P_Fld(0x1, SHU_PHYPLL3_RG_RPHYPLL_DIV_CK_SEL) |
        P_Fld(0x1, SHU_PHYPLL3_RG_RPHYPLL_GLITCH_FREE_EN) | P_Fld(0x0, SHU_PHYPLL3_RG_RPHYPLL_LVR_REFSEL) |
        P_Fld(0x0, SHU_PHYPLL3_RG_RPHYPLL_DIV3_EN) | P_Fld(0x1, SHU_PHYPLL3_RG_RPHYPLL_FS_EN) |
        P_Fld(0x0, SHU_PHYPLL3_RG_RPHYPLL_FBKSEL) | P_Fld(0x2, SHU_PHYPLL3_RG_RPHYPLL_RST_DLY) |
        P_Fld(0x1, SHU_PHYPLL3_RG_RPHYPLL_LVROD_EN) | P_Fld(0x0, SHU_PHYPLL3_RG_RPHYPLL_MONREF_EN) |
        P_Fld(0x0, SHU_PHYPLL3_RG_RPHYPLL_MONVC_EN) | P_Fld(0x0, SHU_PHYPLL3_RG_RPHYPLL_MONCK_EN));
vIO32WriteFldMulti(DDRPHY_REG_SHU_CLRPLL0+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), P_Fld(0x00fe, SHU_CLRPLL0_RG_RCLRPLL_RESERVED) |
        P_Fld(0x2, SHU_CLRPLL0_RG_RCLRPLL_FS) | P_Fld(0x7, SHU_CLRPLL0_RG_RCLRPLL_BW) |
        P_Fld(0x1, SHU_CLRPLL0_RG_RCLRPLL_ICHP) | P_Fld(0x1, SHU_CLRPLL0_RG_RCLRPLL_IBIAS) |
        P_Fld(0x1, SHU_CLRPLL0_RG_RCLRPLL_BLP) | P_Fld(0x1, SHU_CLRPLL0_RG_RCLRPLL_BR) |
        P_Fld(0x1, SHU_CLRPLL0_RG_RCLRPLL_BP));
vIO32WriteFldMulti(DDRPHY_REG_SHU_CLRPLL1+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), P_Fld(0x0, SHU_CLRPLL1_RG_RCLRPLL_SDM_FRA_EN) |
        P_Fld(0x1, SHU_CLRPLL1_RG_RCLRPLL_SDM_PCW_CHG) | P_Fld(0x5b00, SHU_CLRPLL1_RG_RCLRPLL_SDM_PCW));
vIO32WriteFldMulti(DDRPHY_REG_SHU_CLRPLL2+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), P_Fld(0x0, SHU_CLRPLL2_RG_RCLRPLL_POSDIV) |
        P_Fld(0x1, SHU_CLRPLL2_RG_RCLRPLL_PREDIV));
vIO32WriteFldMulti(DDRPHY_REG_SHU_CA_CMD5+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), P_Fld(0x00, SHU_CA_CMD5_RG_RX_ARCMD_VREF_SEL) |
        P_Fld(0x0, SHU_CA_CMD5_RG_RX_ARCMD_VREF_BYPASS) | P_Fld(0x00, SHU_CA_CMD5_RG_ARPI_FB_CA) |
        P_Fld(0x0, SHU_CA_CMD5_RG_RX_ARCLK_DQSIEN_DLY) | P_Fld(0x0, SHU_CA_CMD5_RG_RX_ARCLK_DQSIEN_RB_DLY) |
        P_Fld(0x0, SHU_CA_CMD5_RG_RX_ARCLK_DVS_DLY) | P_Fld(0x0, SHU_CA_CMD5_RG_RX_ARCMD_FIFO_DQSI_DLY));
vIO32WriteFldAlign(DDRPHY_REG_SHU_CA_CMD9+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), 0x4103911e, SHU_CA_CMD9_RG_ARPI_RESERVE_CA);
vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_MIDPI_CTRL+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), P_Fld(0x0, MISC_SHU_MIDPI_CTRL_MIDPI_ENABLE) |
        P_Fld(0x1, MISC_SHU_MIDPI_CTRL_MIDPI_DIV4_ENABLE));
vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RDAT1+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), P_Fld(0x1, MISC_SHU_RDAT1_R_DMRDSEL_DIV2_OPT) |
        P_Fld(0x1, MISC_SHU_RDAT1_R_DMRDSEL_LOBYTE_OPT) | P_Fld(0x0, MISC_SHU_RDAT1_R_DMRDSEL_HIBYTE_OPT) |
        P_Fld(0x0, MISC_SHU_RDAT1_RDATDIV2) | P_Fld(0x1, MISC_SHU_RDAT1_RDATDIV4));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ13+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), P_Fld(0x1, SHU_B0_DQ13_RG_TX_ARDQ_IO_ODT_DIS_B0) |
        P_Fld(0x0, SHU_B0_DQ13_RG_TX_ARDQ_FRATE_EN_B0) | P_Fld(0x0, SHU_B0_DQ13_RG_TX_ARDQ_DLY_LAT_EN_B0) |
        P_Fld(0x1, SHU_B0_DQ13_RG_TX_ARDQS_READ_BASE_EN_B0) | P_Fld(0x0, SHU_B0_DQ13_RG_TX_ARDQS_PRE_DATA_SEL_B0) |
        P_Fld(0x0, SHU_B0_DQ13_RG_TX_ARDQS_OE_ODTEN_SWAP_B0) | P_Fld(0x1, SHU_B0_DQ13_RG_TX_ARDQS_OE_ODTEN_CG_EN_B0) |
        P_Fld(0x0, SHU_B0_DQ13_RG_TX_ARDQS_MCKIO_CG_B0) | P_Fld(0x0, SHU_B0_DQ13_RG_TX_ARDQS_MCKIO_SEL_B0) |
        P_Fld(0x0, SHU_B0_DQ13_RG_TX_ARDQM_MCKIO_SEL_B0) | P_Fld(0x1, SHU_B0_DQ13_RG_TX_ARDQSB_READ_BASE_EN_B0) |
        P_Fld(0x1, SHU_B0_DQ13_RG_TX_ARDQM_OE_ODTEN_CG_EN_B0) | P_Fld(0x1, SHU_B0_DQ13_RG_TX_ARDQSB_READ_BASE_DATA_TIE_EN_B0) |
        P_Fld(0x1, SHU_B0_DQ13_RG_TX_ARDQS_READ_BASE_DATA_TIE_EN_B0) | P_Fld(0x0, SHU_B0_DQ13_RG_TX_ARDQ_READ_BASE_EN_B0) |
        P_Fld(0x0, SHU_B0_DQ13_RG_TX_ARDQ_READ_BASE_DATA_TIE_EN_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ13+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), P_Fld(0x1, SHU_B1_DQ13_RG_TX_ARDQ_IO_ODT_DIS_B1) |
        P_Fld(0x0, SHU_B1_DQ13_RG_TX_ARDQ_FRATE_EN_B1) | P_Fld(0x0, SHU_B1_DQ13_RG_TX_ARDQ_DLY_LAT_EN_B1) |
        P_Fld(0x1, SHU_B1_DQ13_RG_TX_ARDQS_READ_BASE_EN_B1) | P_Fld(0x0, SHU_B1_DQ13_RG_TX_ARDQS_PRE_DATA_SEL_B1) |
        P_Fld(0x0, SHU_B1_DQ13_RG_TX_ARDQS_OE_ODTEN_SWAP_B1) | P_Fld(0x1, SHU_B1_DQ13_RG_TX_ARDQS_OE_ODTEN_CG_EN_B1) |
        P_Fld(0x0, SHU_B1_DQ13_RG_TX_ARDQS_MCKIO_CG_B1) | P_Fld(0x0, SHU_B1_DQ13_RG_TX_ARDQS_MCKIO_SEL_B1) |
        P_Fld(0x0, SHU_B1_DQ13_RG_TX_ARDQM_MCKIO_SEL_B1) | P_Fld(0x1, SHU_B1_DQ13_RG_TX_ARDQSB_READ_BASE_EN_B1) |
        P_Fld(0x1, SHU_B1_DQ13_RG_TX_ARDQM_OE_ODTEN_CG_EN_B1) | P_Fld(0x1, SHU_B1_DQ13_RG_TX_ARDQSB_READ_BASE_DATA_TIE_EN_B1) |
        P_Fld(0x1, SHU_B1_DQ13_RG_TX_ARDQS_READ_BASE_DATA_TIE_EN_B1) | P_Fld(0x0, SHU_B1_DQ13_RG_TX_ARDQ_READ_BASE_EN_B1) |
        P_Fld(0x0, SHU_B1_DQ13_RG_TX_ARDQ_READ_BASE_DATA_TIE_EN_B1));
vIO32WriteFldAlign(DDRPHY_REG_SHU_B0_DQ9+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), 0xfa9fd5c2, SHU_B0_DQ9_RG_ARPI_RESERVE_B0);
vIO32WriteFldAlign(DDRPHY_REG_SHU_B1_DQ9+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), 0x6b9a7ebf, SHU_B1_DQ9_RG_ARPI_RESERVE_B1);
vIO32WriteFldMulti(DDRPHY_REG_SHU_CA_CMD7+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), P_Fld(0x0, SHU_CA_CMD7_R_DMRANKRXDVS_CA) |
        P_Fld(0x0, SHU_CA_CMD7_R_DMRXDVS_PBYTE_FLAG_OPT_CA) | P_Fld(0x0, SHU_CA_CMD7_R_DMRODTEN_CA) |
        P_Fld(0x0, SHU_CA_CMD7_R_DMARPI_CG_FB2DLL_DCM_EN_CA) | P_Fld(0x1, SHU_CA_CMD7_R_DMTX_ARPI_CG_CMD_NEW) |
        P_Fld(0x1, SHU_CA_CMD7_R_DMTX_ARPI_CG_CS_NEW) | P_Fld(0x0, SHU_CA_CMD7_R_LP4Y_SDN_MODE_CLK) |
        P_Fld(0x0, SHU_CA_CMD7_R_DMRXRANK_CMD_EN) | P_Fld(0x0, SHU_CA_CMD7_R_DMRXRANK_CMD_LAT) |
        P_Fld(0x0, SHU_CA_CMD7_R_DMRXRANK_CLK_EN) | P_Fld(0x0, SHU_CA_CMD7_R_DMRXRANK_CLK_LAT));
vIO32WriteFldMulti(DDRPHY_REG_SHU_CA_DLL1+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), P_Fld(0x0, SHU_CA_DLL1_RG_ARDLL_AD_ARFB_CK_EN_CA) |
        P_Fld(0x3, SHU_CA_DLL1_RG_ARDLL_DIV_MODE_CA) | P_Fld(0x1, SHU_CA_DLL1_RG_ARDLL_UDIV_EN_CA) |
        P_Fld(0x0, SHU_CA_DLL1_RG_ARDLL_UPDATE_ON_IDLE_EN_CA) | P_Fld(0x0, SHU_CA_DLL1_RG_ARDLL_TRACKING_CA_EN_CA) |
        P_Fld(0x0, SHU_CA_DLL1_RG_ARDLL_UPDATE_ON_IDLE_MODE_CA) | P_Fld(0x1, SHU_CA_DLL1_RG_ARDLL_SER_MODE_CA) |
        P_Fld(0x1, SHU_CA_DLL1_RG_ARDLL_PS_EN_CA) | P_Fld(0x1, SHU_CA_DLL1_RG_ARDLL_PSJP_EN_CA) |
        P_Fld(0x1, SHU_CA_DLL1_RG_ARDLL_PHDIV_CA) | P_Fld(0x1, SHU_CA_DLL1_RG_ARDLL_PHDET_OUT_SEL_CA) |
        P_Fld(0x1, SHU_CA_DLL1_RG_ARDLL_PHDET_IN_SWAP_CA) | P_Fld(0x1, SHU_CA_DLL1_RG_ARDLL_PHDET_EN_CA) |
        P_Fld(0x0, SHU_CA_DLL1_RG_ARDLL_DIV_MCTL_CA) | P_Fld(0x0, SHU_CA_DLL1_RG_ARDLL_PGAIN_CA) |
        P_Fld(0x0, SHU_CA_DLL1_RG_ARDLL_PD_CK_SEL_CA));
vIO32WriteFldAlign(DDRPHY_REG_SHU_CA_DLL2+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), 0x1793f0fe, SHU_CA_DLL2_RG_ARCMD_REV);
vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DLL1+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), P_Fld(0x0, SHU_B0_DLL1_RG_ARDLL_AD_ARFB_CK_EN_B0) |
        P_Fld(0x3, SHU_B0_DLL1_RG_ARDLL_DIV_MODE_B0) | P_Fld(0x1, SHU_B0_DLL1_RG_ARDLL_UDIV_EN_B0) |
        P_Fld(0x0, SHU_B0_DLL1_RG_ARDLL_UPDATE_ON_IDLE_EN_B0) | P_Fld(0x0, SHU_B0_DLL1_RG_ARDLL_TRACKING_CA_EN_B0) |
        P_Fld(0x0, SHU_B0_DLL1_RG_ARDLL_UPDATE_ON_IDLE_MODE_B0) | P_Fld(0x1, SHU_B0_DLL1_RG_ARDLL_SER_MODE_B0) |
        P_Fld(0x1, SHU_B0_DLL1_RG_ARDLL_PS_EN_B0) | P_Fld(0x1, SHU_B0_DLL1_RG_ARDLL_PSJP_EN_B0) |
        P_Fld(0x1, SHU_B0_DLL1_RG_ARDLL_PHDIV_B0) | P_Fld(0x1, SHU_B0_DLL1_RG_ARDLL_PHDET_OUT_SEL_B0) |
        P_Fld(0x1, SHU_B0_DLL1_RG_ARDLL_PHDET_IN_SWAP_B0) | P_Fld(0x1, SHU_B0_DLL1_RG_ARDLL_PHDET_EN_B0) |
        P_Fld(0x0, SHU_B0_DLL1_RG_ARDLL_DIV_MCTL_B0) | P_Fld(0x0, SHU_B0_DLL1_RG_ARDLL_PGAIN_B0) |
        P_Fld(0x0, SHU_B0_DLL1_RG_ARDLL_PD_CK_SEL_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DLL1+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), P_Fld(0x0, SHU_B1_DLL1_RG_ARDLL_AD_ARFB_CK_EN_B1) |
        P_Fld(0x3, SHU_B1_DLL1_RG_ARDLL_DIV_MODE_B1) | P_Fld(0x1, SHU_B1_DLL1_RG_ARDLL_UDIV_EN_B1) |
        P_Fld(0x0, SHU_B1_DLL1_RG_ARDLL_UPDATE_ON_IDLE_EN_B1) | P_Fld(0x0, SHU_B1_DLL1_RG_ARDLL_TRACKING_CA_EN_B1) |
        P_Fld(0x0, SHU_B1_DLL1_RG_ARDLL_UPDATE_ON_IDLE_MODE_B1) | P_Fld(0x1, SHU_B1_DLL1_RG_ARDLL_SER_MODE_B1) |
        P_Fld(0x1, SHU_B1_DLL1_RG_ARDLL_PS_EN_B1) | P_Fld(0x1, SHU_B1_DLL1_RG_ARDLL_PSJP_EN_B1) |
        P_Fld(0x1, SHU_B1_DLL1_RG_ARDLL_PHDIV_B1) | P_Fld(0x1, SHU_B1_DLL1_RG_ARDLL_PHDET_OUT_SEL_B1) |
        P_Fld(0x1, SHU_B1_DLL1_RG_ARDLL_PHDET_IN_SWAP_B1) | P_Fld(0x1, SHU_B1_DLL1_RG_ARDLL_PHDET_EN_B1) |
        P_Fld(0x0, SHU_B1_DLL1_RG_ARDLL_DIV_MCTL_B1) | P_Fld(0x0, SHU_B1_DLL1_RG_ARDLL_PGAIN_B1) |
        P_Fld(0x0, SHU_B1_DLL1_RG_ARDLL_PD_CK_SEL_B1));
vIO32WriteFldAlign(DDRPHY_REG_SHU_B0_DLL2+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), 0xb357f9bf, SHU_B0_DLL2_RG_ARDQ_REV_B0);
vIO32WriteFldAlign(DDRPHY_REG_SHU_B1_DLL2+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), 0x7e18d6d9, SHU_B1_DLL2_RG_ARDQ_REV_B1);
vIO32WriteFldMulti(DDRPHY_REG_SHU_CA_CMD11+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), P_Fld(0x0, SHU_CA_CMD11_RG_RX_ARCA_RANK_SEL_SER_EN_CA) |
        P_Fld(0x0, SHU_CA_CMD11_RG_RX_ARCA_RANK_SEL_LAT_EN_CA) | P_Fld(0x0, SHU_CA_CMD11_RG_RX_ARCA_OFFSETC_LAT_EN_CA) |
        P_Fld(0x0, SHU_CA_CMD11_RG_RX_ARCA_OFFSETC_EN_CA) | P_Fld(0x0, SHU_CA_CMD11_RG_RX_ARCA_OFFSETC_BIAS_EN_CA) |
        P_Fld(0x0, SHU_CA_CMD11_RG_RX_ARCA_FRATE_EN_CA) | P_Fld(0x0, SHU_CA_CMD11_RG_RX_ARCA_CDR_EN_CA) |
        P_Fld(0x0, SHU_CA_CMD11_RG_RX_ARCA_DVS_EN_CA) | P_Fld(0x0, SHU_CA_CMD11_RG_RX_ARCA_DVS_DLY_CA) |
        P_Fld(0x2, SHU_CA_CMD11_RG_RX_ARCA_DES_MODE_CA) | P_Fld(0x0, SHU_CA_CMD11_RG_RX_ARCA_BW_SEL_CA));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ11+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), P_Fld(0x1, SHU_B0_DQ11_RG_RX_ARDQ_RANK_SEL_SER_EN_B0) |
        P_Fld(0x1, SHU_B0_DQ11_RG_RX_ARDQ_RANK_SEL_LAT_EN_B0) | P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_OFFSETC_LAT_EN_B0) |
        P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_OFFSETC_EN_B0) | P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_OFFSETC_BIAS_EN_B0) |
        P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_FRATE_EN_B0) | P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_CDR_EN_B0) |
        P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_DVS_EN_B0) | P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_DVS_DLY_B0) |
        P_Fld(0x2, SHU_B0_DQ11_RG_RX_ARDQ_DES_MODE_B0) | P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_BW_SEL_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ11+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), P_Fld(0x1, SHU_B1_DQ11_RG_RX_ARDQ_RANK_SEL_SER_EN_B1) |
        P_Fld(0x1, SHU_B1_DQ11_RG_RX_ARDQ_RANK_SEL_LAT_EN_B1) | P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_OFFSETC_LAT_EN_B1) |
        P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_OFFSETC_EN_B1) | P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_OFFSETC_BIAS_EN_B1) |
        P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_FRATE_EN_B1) | P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_CDR_EN_B1) |
        P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_DVS_EN_B1) | P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_DVS_DLY_B1) |
        P_Fld(0x2, SHU_B1_DQ11_RG_RX_ARDQ_DES_MODE_B1) | P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_BW_SEL_B1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_CA_CMD2+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), P_Fld(0x1, SHU_CA_CMD2_RG_ARPI_TX_CG_SYNC_DIS_CA) |
        P_Fld(0x1, SHU_CA_CMD2_RG_ARPI_TX_CG_CA_EN_CA) | P_Fld(0x1, SHU_CA_CMD2_RG_ARPI_TX_CG_CLK_EN_CA) |
        P_Fld(0x1, SHU_CA_CMD2_RG_ARPI_TX_CG_CS_EN_CA) | P_Fld(0x0, SHU_CA_CMD2_RG_ARPI_SMT_XLATCH_FORCE_CLK_CA) |
        P_Fld(0x0, SHU_CA_CMD2_RG_ARPI_SMT_XLATCH_CA_FORCE_CA) | P_Fld(0x0, SHU_CA_CMD2_RG_ARPI_PSMUX_XLATCH_FORCE_CA_CA) |
        P_Fld(0x0, SHU_CA_CMD2_RG_ARPI_PSMUX_XLATCH_FORCE_CLK_CA) | P_Fld(0x1, SHU_CA_CMD2_RG_ARPISM_MCK_SEL_CA_SHU) |
        P_Fld(0x0, SHU_CA_CMD2_RG_ARPI_PD_MCTL_SEL_CA) | P_Fld(0x0, SHU_CA_CMD2_RG_ARPI_OFFSET_LAT_EN_CA) |
        P_Fld(0x0, SHU_CA_CMD2_RG_ARPI_OFFSET_ASYNC_EN_CA));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ2+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), P_Fld(0x1, SHU_B0_DQ2_RG_ARPI_TX_CG_SYNC_DIS_B0) |
        P_Fld(0x1, SHU_B0_DQ2_RG_ARPI_TX_CG_DQ_EN_B0) | P_Fld(0x1, SHU_B0_DQ2_RG_ARPI_TX_CG_DQS_EN_B0) |
        P_Fld(0x1, SHU_B0_DQ2_RG_ARPI_TX_CG_DQM_EN_B0) | P_Fld(0x0, SHU_B0_DQ2_RG_ARPI_SMT_XLATCH_FORCE_DQS_B0) |
        P_Fld(0x0, SHU_B0_DQ2_RG_ARPI_SMT_XLATCH_DQ_FORCE_B0) | P_Fld(0x0, SHU_B0_DQ2_RG_ARPI_PSMUX_XLATCH_FORCE_DQ_B0) |
        P_Fld(0x0, SHU_B0_DQ2_RG_ARPI_PSMUX_XLATCH_FORCE_DQS_B0) | P_Fld(0x1, SHU_B0_DQ2_RG_ARPISM_MCK_SEL_B0_SHU) |
        P_Fld(0x0, SHU_B0_DQ2_RG_ARPI_PD_MCTL_SEL_B0) | P_Fld(0x1, SHU_B0_DQ2_RG_ARPI_OFFSET_LAT_EN_B0) |
        P_Fld(0x1, SHU_B0_DQ2_RG_ARPI_OFFSET_ASYNC_EN_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ2+(1*SHU_GRP_DDRPHY_OFFSET)+((U32)CHANNEL_B<<POS_BANK_NUM), P_Fld(0x1, SHU_B1_DQ2_RG_ARPI_TX_CG_SYNC_DIS_B1) |
        P_Fld(0x1, SHU_B1_DQ2_RG_ARPI_TX_CG_DQ_EN_B1) | P_Fld(0x1, SHU_B1_DQ2_RG_ARPI_TX_CG_DQS_EN_B1) |
        P_Fld(0x1, SHU_B1_DQ2_RG_ARPI_TX_CG_DQM_EN_B1) | P_Fld(0x0, SHU_B1_DQ2_RG_ARPI_SMT_XLATCH_FORCE_DQS_B1) |
        P_Fld(0x0, SHU_B1_DQ2_RG_ARPI_SMT_XLATCH_DQ_FORCE_B1) | P_Fld(0x0, SHU_B1_DQ2_RG_ARPI_PSMUX_XLATCH_FORCE_DQ_B1) |
        P_Fld(0x0, SHU_B1_DQ2_RG_ARPI_PSMUX_XLATCH_FORCE_DQS_B1) | P_Fld(0x1, SHU_B1_DQ2_RG_ARPISM_MCK_SEL_B1_SHU) |
        P_Fld(0x0, SHU_B1_DQ2_RG_ARPI_PD_MCTL_SEL_B1) | P_Fld(0x1, SHU_B1_DQ2_RG_ARPI_OFFSET_LAT_EN_B1) |
        P_Fld(0x1, SHU_B1_DQ2_RG_ARPI_OFFSET_ASYNC_EN_B1));

    mcDELAY_US(1);

    mcDELAY_US(1);

     DramcBroadcastOnOff(DRAMC_BROADCAST_ON);

vIO32WriteFldMulti(DDRPHY_REG_SHU_CA_DLL_ARPI3+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x0, SHU_CA_DLL_ARPI3_RG_ARPI_CLKIEN_EN) |
        P_Fld(0x1, SHU_CA_DLL_ARPI3_RG_ARPI_CMD_EN) | P_Fld(0x1, SHU_CA_DLL_ARPI3_RG_ARPI_CLK_EN) |
        P_Fld(0x1, SHU_CA_DLL_ARPI3_RG_ARPI_CS_EN) | P_Fld(0x1, SHU_CA_DLL_ARPI3_RG_ARPI_FB_EN_CA) |
        P_Fld(0x1, SHU_CA_DLL_ARPI3_RG_ARPI_MCTL_EN_CA));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DLL_ARPI3+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x1, SHU_B0_DLL_ARPI3_RG_ARPI_DQSIEN_EN_B0) |
        P_Fld(0x1, SHU_B0_DLL_ARPI3_RG_ARPI_DQ_EN_B0) | P_Fld(0x1, SHU_B0_DLL_ARPI3_RG_ARPI_DQM_EN_B0) |
        P_Fld(0x1, SHU_B0_DLL_ARPI3_RG_ARPI_DQS_EN_B0) | P_Fld(0x1, SHU_B0_DLL_ARPI3_RG_ARPI_FB_EN_B0) |
        P_Fld(0x1, SHU_B0_DLL_ARPI3_RG_ARPI_MCTL_EN_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DLL_ARPI3+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x1, SHU_B1_DLL_ARPI3_RG_ARPI_DQSIEN_EN_B1) |
        P_Fld(0x1, SHU_B1_DLL_ARPI3_RG_ARPI_DQ_EN_B1) | P_Fld(0x1, SHU_B1_DLL_ARPI3_RG_ARPI_DQM_EN_B1) |
        P_Fld(0x1, SHU_B1_DLL_ARPI3_RG_ARPI_DQS_EN_B1) | P_Fld(0x1, SHU_B1_DLL_ARPI3_RG_ARPI_FB_EN_B1) |
        P_Fld(0x1, SHU_B1_DLL_ARPI3_RG_ARPI_MCTL_EN_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ13+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x1, SHU_B0_DQ13_RG_TX_ARDQ_IO_ODT_DIS_B0) |
        P_Fld(0x0, SHU_B0_DQ13_RG_TX_ARDQ_FRATE_EN_B0) | P_Fld(0x1, SHU_B0_DQ13_RG_TX_ARDQ_DLY_LAT_EN_B0) |
        P_Fld(0x1, SHU_B0_DQ13_RG_TX_ARDQS_READ_BASE_EN_B0) | P_Fld(0x0, SHU_B0_DQ13_RG_TX_ARDQS_PRE_DATA_SEL_B0) |
        P_Fld(0x0, SHU_B0_DQ13_RG_TX_ARDQS_OE_ODTEN_SWAP_B0) | P_Fld(0x1, SHU_B0_DQ13_RG_TX_ARDQS_OE_ODTEN_CG_EN_B0) |
        P_Fld(0x0, SHU_B0_DQ13_RG_TX_ARDQS_MCKIO_CG_B0) | P_Fld(0x0, SHU_B0_DQ13_RG_TX_ARDQS_MCKIO_SEL_B0) |
        P_Fld(0x0, SHU_B0_DQ13_RG_TX_ARDQM_MCKIO_SEL_B0) | P_Fld(0x1, SHU_B0_DQ13_RG_TX_ARDQSB_READ_BASE_EN_B0) |
        P_Fld(0x1, SHU_B0_DQ13_RG_TX_ARDQM_OE_ODTEN_CG_EN_B0) | P_Fld(0x1, SHU_B0_DQ13_RG_TX_ARDQSB_READ_BASE_DATA_TIE_EN_B0) |
        P_Fld(0x1, SHU_B0_DQ13_RG_TX_ARDQS_READ_BASE_DATA_TIE_EN_B0) | P_Fld(0x0, SHU_B0_DQ13_RG_TX_ARDQ_READ_BASE_EN_B0) |
        P_Fld(0x0, SHU_B0_DQ13_RG_TX_ARDQ_READ_BASE_DATA_TIE_EN_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ13+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x1, SHU_B1_DQ13_RG_TX_ARDQ_IO_ODT_DIS_B1) |
        P_Fld(0x0, SHU_B1_DQ13_RG_TX_ARDQ_FRATE_EN_B1) | P_Fld(0x1, SHU_B1_DQ13_RG_TX_ARDQ_DLY_LAT_EN_B1) |
        P_Fld(0x1, SHU_B1_DQ13_RG_TX_ARDQS_READ_BASE_EN_B1) | P_Fld(0x0, SHU_B1_DQ13_RG_TX_ARDQS_PRE_DATA_SEL_B1) |
        P_Fld(0x0, SHU_B1_DQ13_RG_TX_ARDQS_OE_ODTEN_SWAP_B1) | P_Fld(0x1, SHU_B1_DQ13_RG_TX_ARDQS_OE_ODTEN_CG_EN_B1) |
        P_Fld(0x0, SHU_B1_DQ13_RG_TX_ARDQS_MCKIO_CG_B1) | P_Fld(0x0, SHU_B1_DQ13_RG_TX_ARDQS_MCKIO_SEL_B1) |
        P_Fld(0x0, SHU_B1_DQ13_RG_TX_ARDQM_MCKIO_SEL_B1) | P_Fld(0x1, SHU_B1_DQ13_RG_TX_ARDQSB_READ_BASE_EN_B1) |
        P_Fld(0x1, SHU_B1_DQ13_RG_TX_ARDQM_OE_ODTEN_CG_EN_B1) | P_Fld(0x1, SHU_B1_DQ13_RG_TX_ARDQSB_READ_BASE_DATA_TIE_EN_B1) |
        P_Fld(0x1, SHU_B1_DQ13_RG_TX_ARDQS_READ_BASE_DATA_TIE_EN_B1) | P_Fld(0x0, SHU_B1_DQ13_RG_TX_ARDQ_READ_BASE_EN_B1) |
        P_Fld(0x0, SHU_B1_DQ13_RG_TX_ARDQ_READ_BASE_DATA_TIE_EN_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_MISC_RDSEL_TRACK+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x10, SHU_MISC_RDSEL_TRACK_DMDATLAT_I) |
        P_Fld(0x1, SHU_MISC_RDSEL_TRACK_RDSEL_HWSAVE_MSK) | P_Fld(0x0, SHU_MISC_RDSEL_TRACK_RDSEL_TRACK_EN) |
        P_Fld(0xff0, SHU_MISC_RDSEL_TRACK_SHU_GW_THRD_NEG) | P_Fld(0x010, SHU_MISC_RDSEL_TRACK_SHU_GW_THRD_POS));
vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RDAT+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x10, MISC_SHU_RDAT_DATLAT) |
        P_Fld(0x0f, MISC_SHU_RDAT_DATLAT_DSEL) | P_Fld(0x0f, MISC_SHU_RDAT_DATLAT_DSEL_PHY));
vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_PHY_RX_CTRL+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x1, MISC_SHU_PHY_RX_CTRL_RANK_RXDLY_UPDLAT_EN) |
        P_Fld(0x2, MISC_SHU_PHY_RX_CTRL_RANK_RXDLY_UPD_OFFSET) | P_Fld(0x1, MISC_SHU_PHY_RX_CTRL_RX_IN_GATE_EN_PRE_OFFSET) |
        P_Fld(0x1, MISC_SHU_PHY_RX_CTRL_RX_IN_GATE_EN_HEAD) | P_Fld(0x1, MISC_SHU_PHY_RX_CTRL_RX_IN_GATE_EN_TAIL) |
        P_Fld(0x1, MISC_SHU_PHY_RX_CTRL_RX_IN_BUFF_EN_HEAD) | P_Fld(0x0, MISC_SHU_PHY_RX_CTRL_RX_IN_BUFF_EN_TAIL));
vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RANKCTL+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x2, MISC_SHU_RANKCTL_RANKINCTL_RXDLY) |
        P_Fld(0x1, MISC_SHU_RANKCTL_RANK_RXDLY_OPT) | P_Fld(0x0, MISC_SHU_RANKCTL_RANKSEL_SELPH_FRUN) |
        P_Fld(0x5, MISC_SHU_RANKCTL_RANKINCTL_STB) | P_Fld(0x3, MISC_SHU_RANKCTL_RANKINCTL) |
        P_Fld(0x3, MISC_SHU_RANKCTL_RANKINCTL_ROOT1) | P_Fld(0x5, MISC_SHU_RANKCTL_RANKINCTL_PHY));
vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RANK_SEL_LAT+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x4, MISC_SHU_RANK_SEL_LAT_RANK_SEL_LAT_B0) |
        P_Fld(0x4, MISC_SHU_RANK_SEL_LAT_RANK_SEL_LAT_B1) | P_Fld(0x4, MISC_SHU_RANK_SEL_LAT_RANK_SEL_LAT_CA));
vIO32WriteFldAlign(DDRPHY_REG_MISC_SHU_RK_DQSCTL+(1*SHU_GRP_DDRPHY_OFFSET), 0x5, MISC_SHU_RK_DQSCTL_DQSINCTL);
vIO32WriteFldAlign(DDRPHY_REG_MISC_SHU_RK_DQSCTL+(1*SHU_GRP_DDRPHY_OFFSET)+(1*DDRPHY_AO_RANK_OFFSET), 0x5, MISC_SHU_RK_DQSCTL_DQSINCTL);
vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B0_DQSIEN_MCK_UI_DLY+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0xa, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B0) |
        P_Fld(0xc, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B0) | P_Fld(0x0, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B0) |
        P_Fld(0x0, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B0));
vIO32WriteFldAlign(DDRPHY_REG_SHU_RK_B0_DQSIEN_PI_DLY+(1*SHU_GRP_DDRPHY_OFFSET), 0x19, SHU_RK_B0_DQSIEN_PI_DLY_DQSIEN_PI_B0);
vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B0_DQSIEN_MCK_UI_DLY+(1*SHU_GRP_DDRPHY_OFFSET)+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0xd, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B0) |
        P_Fld(0xf, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B0) | P_Fld(0x0, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B0) |
        P_Fld(0x0, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B0));
vIO32WriteFldAlign(DDRPHY_REG_SHU_RK_B0_DQSIEN_PI_DLY+(1*SHU_GRP_DDRPHY_OFFSET)+(1*DDRPHY_AO_RANK_OFFSET), 0x05, SHU_RK_B0_DQSIEN_PI_DLY_DQSIEN_PI_B0);
vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B1_DQSIEN_MCK_UI_DLY+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0xa, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B1) |
        P_Fld(0xc, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B1) | P_Fld(0x0, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B1) |
        P_Fld(0x0, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B1));
vIO32WriteFldAlign(DDRPHY_REG_SHU_RK_B1_DQSIEN_PI_DLY+(1*SHU_GRP_DDRPHY_OFFSET), 0x19, SHU_RK_B1_DQSIEN_PI_DLY_DQSIEN_PI_B1);
vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B1_DQSIEN_MCK_UI_DLY+(1*SHU_GRP_DDRPHY_OFFSET)+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0xd, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B1) |
        P_Fld(0xf, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B1) | P_Fld(0x0, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B1) |
        P_Fld(0x0, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B1));
vIO32WriteFldAlign(DDRPHY_REG_SHU_RK_B1_DQSIEN_PI_DLY+(1*SHU_GRP_DDRPHY_OFFSET)+(1*DDRPHY_AO_RANK_OFFSET), 0x05, SHU_RK_B1_DQSIEN_PI_DLY_DQSIEN_PI_B1);
vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_ODTCTRL+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x1, MISC_SHU_ODTCTRL_RODTEN) |
        P_Fld(0x0, MISC_SHU_ODTCTRL_RODTENSTB_SELPH_CG_IG) | P_Fld(0x4, MISC_SHU_ODTCTRL_RODT_LAT) |
        P_Fld(0x0, MISC_SHU_ODTCTRL_RODTEN_SELPH_FRUN) | P_Fld(0x0, MISC_SHU_ODTCTRL_RODTDLY_LAT_OPT) |
        P_Fld(0x0, MISC_SHU_ODTCTRL_FIXRODT) | P_Fld(0x1, MISC_SHU_ODTCTRL_RODTEN_OPT) |
        P_Fld(0x1, MISC_SHU_ODTCTRL_RODTE2) | P_Fld(0x1, MISC_SHU_ODTCTRL_RODTE));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ7+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x0, SHU_B0_DQ7_R_DMRANKRXDVS_B0) |
        P_Fld(0x0, SHU_B0_DQ7_R_DMDQMDBI_EYE_SHU_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMDQMDBI_SHU_B0) |
        P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_DQM_FLAGSEL_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B0) |
        P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMRXTRACK_DQM_EN_B0) |
        P_Fld(0x1, SHU_B0_DQ7_R_DMRODTEN_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMARPI_CG_FB2DLL_DCM_EN_B0) |
        P_Fld(0x0, SHU_B0_DQ7_R_DMTX_ARPI_CG_DQ_NEW_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMTX_ARPI_CG_DQS_NEW_B0) |
        P_Fld(0x0, SHU_B0_DQ7_R_DMTX_ARPI_CG_DQM_NEW_B0) | P_Fld(0x0, SHU_B0_DQ7_R_LP4Y_SDN_MODE_DQS0) |
        P_Fld(0x1, SHU_B0_DQ7_R_DMRXRANK_DQ_EN_B0) | P_Fld(0x2, SHU_B0_DQ7_R_DMRXRANK_DQ_LAT_B0) |
        P_Fld(0x1, SHU_B0_DQ7_R_DMRXRANK_DQS_EN_B0) | P_Fld(0x1, SHU_B0_DQ7_R_DMRXRANK_DQS_LAT_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ7+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x0, SHU_B1_DQ7_R_DMRANKRXDVS_B1) |
        P_Fld(0x0, SHU_B1_DQ7_R_DMDQMDBI_EYE_SHU_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMDQMDBI_SHU_B1) |
        P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_DQM_FLAGSEL_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B1) |
        P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMRXTRACK_DQM_EN_B1) |
        P_Fld(0x1, SHU_B1_DQ7_R_DMRODTEN_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMARPI_CG_FB2DLL_DCM_EN_B1) |
        P_Fld(0x0, SHU_B1_DQ7_R_DMTX_ARPI_CG_DQ_NEW_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMTX_ARPI_CG_DQS_NEW_B1) |
        P_Fld(0x0, SHU_B1_DQ7_R_DMTX_ARPI_CG_DQM_NEW_B1) | P_Fld(0x0, SHU_B1_DQ7_R_LP4Y_SDN_MODE_DQS1) |
        P_Fld(0x1, SHU_B1_DQ7_R_DMRXRANK_DQ_EN_B1) | P_Fld(0x2, SHU_B1_DQ7_R_DMRXRANK_DQ_LAT_B1) |
        P_Fld(0x1, SHU_B1_DQ7_R_DMRXRANK_DQS_EN_B1) | P_Fld(0x1, SHU_B1_DQ7_R_DMRXRANK_DQS_LAT_B1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B0_RODTEN_MCK_UI_DLY+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x1, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B0) |
        P_Fld(0x1, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B0) | P_Fld(0x0, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B0) |
        P_Fld(0x0, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B0_RODTEN_MCK_UI_DLY+(1*SHU_GRP_DDRPHY_OFFSET)+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x0, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B0) |
        P_Fld(0x0, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B0) | P_Fld(0x1, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B0) |
        P_Fld(0x0, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B1_RODTEN_MCK_UI_DLY+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x1, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B1) |
        P_Fld(0x1, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B1) | P_Fld(0x0, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B1) |
        P_Fld(0x0, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B1_RODTEN_MCK_UI_DLY+(1*SHU_GRP_DDRPHY_OFFSET)+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x0, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B1) |
        P_Fld(0x0, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B1) | P_Fld(0x1, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B1) |
        P_Fld(0x0, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B1));
vIO32WriteFldMulti(DRAMC_REG_SHU_RX_CG_SET0+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x0, SHU_RX_CG_SET0_DLE_LAST_EXTEND3) |
        P_Fld(0x0, SHU_RX_CG_SET0_READ_START_EXTEND3) | P_Fld(0x1, SHU_RX_CG_SET0_DLE_LAST_EXTEND2) |
        P_Fld(0x1, SHU_RX_CG_SET0_READ_START_EXTEND2) | P_Fld(0x1, SHU_RX_CG_SET0_DLE_LAST_EXTEND1) |
        P_Fld(0x1, SHU_RX_CG_SET0_READ_START_EXTEND1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_MISC_RANK_SEL_STB+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x1, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_EN) |
        P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_EN_B23) | P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_SERMODE) |
        P_Fld(0x1, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_TRACK) | P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_RXDLY_TRACK) |
        P_Fld(0x1, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_PHASE_EN) | P_Fld(0x6, SHU_MISC_RANK_SEL_STB_RANK_SEL_PHSINCTL) |
        P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_UI_PLUS) | P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_MCK_PLUS) |
        P_Fld(0x2, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_UI_MINUS) | P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_MCK_MINUS));
vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RK_DQSCAL+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x60, MISC_SHU_RK_DQSCAL_DQSIENLLMT) |
        P_Fld(0x1, MISC_SHU_RK_DQSCAL_DQSIENLLMTEN) | P_Fld(0x3f, MISC_SHU_RK_DQSCAL_DQSIENHLMT) |
        P_Fld(0x1, MISC_SHU_RK_DQSCAL_DQSIENHLMTEN));
vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RK_DQSCAL+(1*SHU_GRP_DDRPHY_OFFSET)+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x60, MISC_SHU_RK_DQSCAL_DQSIENLLMT) |
        P_Fld(0x1, MISC_SHU_RK_DQSCAL_DQSIENLLMTEN) | P_Fld(0x3f, MISC_SHU_RK_DQSCAL_DQSIENHLMT) |
        P_Fld(0x1, MISC_SHU_RK_DQSCAL_DQSIENHLMTEN));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_INI_UIPI+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x19, SHU_R0_B0_INI_UIPI_CURR_INI_PI_B0) |
        P_Fld(0x0a, SHU_R0_B0_INI_UIPI_CURR_INI_UI_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_INI_UIPI+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x19, SHU_R0_B1_INI_UIPI_CURR_INI_PI_B1) |
        P_Fld(0x0a, SHU_R0_B1_INI_UIPI_CURR_INI_UI_B1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_INI_UIPI+(1*SHU_GRP_DDRPHY_OFFSET)+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x05, SHU_R0_B0_INI_UIPI_CURR_INI_PI_B0) |
        P_Fld(0x0d, SHU_R0_B0_INI_UIPI_CURR_INI_UI_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_INI_UIPI+(1*SHU_GRP_DDRPHY_OFFSET)+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x05, SHU_R0_B1_INI_UIPI_CURR_INI_PI_B1) |
        P_Fld(0x0d, SHU_R0_B1_INI_UIPI_CURR_INI_UI_B1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_NEXT_INI_UIPI+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x19, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_PI_B0) |
        P_Fld(0x0a, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_UI_B0) | P_Fld(0x0c, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_UI_P1_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_NEXT_INI_UIPI+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x19, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_PI_B1) |
        P_Fld(0x0a, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_UI_B1) | P_Fld(0x0c, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_UI_P1_B1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_NEXT_INI_UIPI+(1*SHU_GRP_DDRPHY_OFFSET)+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x05, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_PI_B0) |
        P_Fld(0x0d, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_UI_B0) | P_Fld(0x0f, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_UI_P1_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_NEXT_INI_UIPI+(1*SHU_GRP_DDRPHY_OFFSET)+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x05, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_PI_B1) |
        P_Fld(0x0d, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_UI_B1) | P_Fld(0x0f, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_UI_P1_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_DQ0+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x0, SHU_R0_B0_DQ0_RG_RX_ARDQS0_R_DLY_DUTY) |
        P_Fld(0x0, SHU_R0_B0_DQ0_RG_RX_ARDQS0_F_DLY_DUTY) | P_Fld(0x17, SHU_R0_B0_DQ0_SW_ARPI_DQ_B0) |
        P_Fld(0x17, SHU_R0_B0_DQ0_SW_ARPI_DQM_B0) | P_Fld(0x00, SHU_R0_B0_DQ0_ARPI_PBYTE_B0) |
        P_Fld(0x0, SHU_R0_B0_DQ0_DA_ARPI_DDR400_0D5UI_RK0_B0) | P_Fld(0x0, SHU_R0_B0_DQ0_DA_RX_ARDQSIEN_0D5UI_RK0_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_DQ0+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x0, SHU_R0_B1_DQ0_RG_RX_ARDQS1_R_DLY_DUTY) |
        P_Fld(0x0, SHU_R0_B1_DQ0_RG_RX_ARDQS1_F_DLY_DUTY) | P_Fld(0x18, SHU_R0_B1_DQ0_SW_ARPI_DQ_B1) |
        P_Fld(0x18, SHU_R0_B1_DQ0_SW_ARPI_DQM_B1) | P_Fld(0x00, SHU_R0_B1_DQ0_ARPI_PBYTE_B1) |
        P_Fld(0x0, SHU_R0_B1_DQ0_DA_ARPI_DDR400_0D5UI_RK0_B1) | P_Fld(0x0, SHU_R0_B1_DQ0_DA_RX_ARDQSIEN_0D5UI_RK0_B1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_DQ0+(1*SHU_GRP_DDRPHY_OFFSET)+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x0, SHU_R0_B0_DQ0_RG_RX_ARDQS0_R_DLY_DUTY) |
        P_Fld(0x0, SHU_R0_B0_DQ0_RG_RX_ARDQS0_F_DLY_DUTY) | P_Fld(0x0a, SHU_R0_B0_DQ0_SW_ARPI_DQ_B0) |
        P_Fld(0x0a, SHU_R0_B0_DQ0_SW_ARPI_DQM_B0) | P_Fld(0x00, SHU_R0_B0_DQ0_ARPI_PBYTE_B0) |
        P_Fld(0x0, SHU_R0_B0_DQ0_DA_ARPI_DDR400_0D5UI_RK0_B0) | P_Fld(0x0, SHU_R0_B0_DQ0_DA_RX_ARDQSIEN_0D5UI_RK0_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_DQ0+(1*SHU_GRP_DDRPHY_OFFSET)+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x0, SHU_R0_B1_DQ0_RG_RX_ARDQS1_R_DLY_DUTY) |
        P_Fld(0x0, SHU_R0_B1_DQ0_RG_RX_ARDQS1_F_DLY_DUTY) | P_Fld(0x06, SHU_R0_B1_DQ0_SW_ARPI_DQ_B1) |
        P_Fld(0x06, SHU_R0_B1_DQ0_SW_ARPI_DQM_B1) | P_Fld(0x00, SHU_R0_B1_DQ0_ARPI_PBYTE_B1) |
        P_Fld(0x0, SHU_R0_B1_DQ0_DA_ARPI_DDR400_0D5UI_RK0_B1) | P_Fld(0x0, SHU_R0_B1_DQ0_DA_RX_ARDQSIEN_0D5UI_RK0_B1));
vIO32WriteFldMulti(DRAMC_REG_SHU_DCM_CTRL0+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x1, SHU_DCM_CTRL0_DDRPHY_CLK_EN_OPT) |
        P_Fld(0x4, SHU_DCM_CTRL0_DPHY_CMDDCM_EXTCNT) | P_Fld(0x6, SHU_DCM_CTRL0_DDRPHY_CLK_DYN_GATING_SEL) |
        P_Fld(0x0, SHU_DCM_CTRL0_CKE_EXTNONPD_CNT) | P_Fld(0x0, SHU_DCM_CTRL0_FASTWAKE2) |
        P_Fld(0x1, SHU_DCM_CTRL0_FASTWAKE));
vIO32WriteFldMulti(DRAMC_REG_SHU_APHY_TX_PICG_CTRL+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0xa, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_PICG_CNT) |
        P_Fld(0x0, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQS_SEL_P1) | P_Fld(0x2, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQS_SEL_P0) |
        P_Fld(0x1, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_OPT));
vIO32WriteFldMulti(DRAMC_REG_SHURK_APHY_TX_PICG_CTRL+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x0, SHURK_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQ_RK_SEL_P1) |
        P_Fld(0x3, SHURK_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQ_RK_SEL_P0));
vIO32WriteFldMulti(DRAMC_REG_SHURK_APHY_TX_PICG_CTRL+(1*SHU_GRP_DRAMC_OFFSET)+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x0, SHURK_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQ_RK_SEL_P1) |
        P_Fld(0x3, SHURK_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQ_RK_SEL_P0));
vIO32WriteFldMulti(DRAMC_REG_SHU_NEW_XRW2W_CTRL+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x2, SHU_NEW_XRW2W_CTRL_TX_PI_UPDCTL_B0) |
        P_Fld(0x2, SHU_NEW_XRW2W_CTRL_TX_PI_UPDCTL_B1) | P_Fld(0x0, SHU_NEW_XRW2W_CTRL_TXPI_UPD_MODE));
vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_DQS0+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x4, SHU_SELPH_DQS0_TXDLY_DQS0) |
        P_Fld(0x4, SHU_SELPH_DQS0_TXDLY_DQS1) | P_Fld(0x1, SHU_SELPH_DQS0_TXDLY_DQS2) |
        P_Fld(0x1, SHU_SELPH_DQS0_TXDLY_DQS3) | P_Fld(0x3, SHU_SELPH_DQS0_TXDLY_OEN_DQS0) |
        P_Fld(0x3, SHU_SELPH_DQS0_TXDLY_OEN_DQS1) | P_Fld(0x1, SHU_SELPH_DQS0_TXDLY_OEN_DQS2) |
        P_Fld(0x1, SHU_SELPH_DQS0_TXDLY_OEN_DQS3));
vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ0+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x4, SHURK_SELPH_DQ0_TXDLY_DQ0) |
        P_Fld(0x4, SHURK_SELPH_DQ0_TXDLY_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_DQ2) |
        P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_DQ3) | P_Fld(0x3, SHURK_SELPH_DQ0_TXDLY_OEN_DQ0) |
        P_Fld(0x3, SHURK_SELPH_DQ0_TXDLY_OEN_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_OEN_DQ2) |
        P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_OEN_DQ3));
vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ1+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x4, SHURK_SELPH_DQ1_TXDLY_DQM0) |
        P_Fld(0x4, SHURK_SELPH_DQ1_TXDLY_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_DQM2) |
        P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_DQM3) | P_Fld(0x3, SHURK_SELPH_DQ1_TXDLY_OEN_DQM0) |
        P_Fld(0x3, SHURK_SELPH_DQ1_TXDLY_OEN_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_OEN_DQM2) |
        P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_OEN_DQM3));
vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ2+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x1, SHURK_SELPH_DQ2_DLY_DQ0) |
        P_Fld(0x1, SHURK_SELPH_DQ2_DLY_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ2_DLY_DQ2) |
        P_Fld(0x1, SHURK_SELPH_DQ2_DLY_DQ3) | P_Fld(0x2, SHURK_SELPH_DQ2_DLY_OEN_DQ0) |
        P_Fld(0x2, SHURK_SELPH_DQ2_DLY_OEN_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ2_DLY_OEN_DQ2) |
        P_Fld(0x1, SHURK_SELPH_DQ2_DLY_OEN_DQ3));
vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ3+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x1, SHURK_SELPH_DQ3_DLY_DQM0) |
        P_Fld(0x1, SHURK_SELPH_DQ3_DLY_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ3_DLY_DQM2) |
        P_Fld(0x1, SHURK_SELPH_DQ3_DLY_DQM3) | P_Fld(0x2, SHURK_SELPH_DQ3_DLY_OEN_DQM0) |
        P_Fld(0x2, SHURK_SELPH_DQ3_DLY_OEN_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ3_DLY_OEN_DQM2) |
        P_Fld(0x1, SHURK_SELPH_DQ3_DLY_OEN_DQM3));
vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ0+(1*SHU_GRP_DRAMC_OFFSET)+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x4, SHURK_SELPH_DQ0_TXDLY_DQ0) |
        P_Fld(0x4, SHURK_SELPH_DQ0_TXDLY_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_DQ2) |
        P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_DQ3) | P_Fld(0x3, SHURK_SELPH_DQ0_TXDLY_OEN_DQ0) |
        P_Fld(0x3, SHURK_SELPH_DQ0_TXDLY_OEN_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_OEN_DQ2) |
        P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_OEN_DQ3));
vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ1+(1*SHU_GRP_DRAMC_OFFSET)+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x4, SHURK_SELPH_DQ1_TXDLY_DQM0) |
        P_Fld(0x4, SHURK_SELPH_DQ1_TXDLY_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_DQM2) |
        P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_DQM3) | P_Fld(0x3, SHURK_SELPH_DQ1_TXDLY_OEN_DQM0) |
        P_Fld(0x3, SHURK_SELPH_DQ1_TXDLY_OEN_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_OEN_DQM2) |
        P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_OEN_DQM3));
vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ2+(1*SHU_GRP_DRAMC_OFFSET)+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x2, SHURK_SELPH_DQ2_DLY_DQ0) |
        P_Fld(0x2, SHURK_SELPH_DQ2_DLY_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ2_DLY_DQ2) |
        P_Fld(0x1, SHURK_SELPH_DQ2_DLY_DQ3) | P_Fld(0x3, SHURK_SELPH_DQ2_DLY_OEN_DQ0) |
        P_Fld(0x3, SHURK_SELPH_DQ2_DLY_OEN_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ2_DLY_OEN_DQ2) |
        P_Fld(0x1, SHURK_SELPH_DQ2_DLY_OEN_DQ3));
vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ3+(1*SHU_GRP_DRAMC_OFFSET)+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x2, SHURK_SELPH_DQ3_DLY_DQM0) |
        P_Fld(0x2, SHURK_SELPH_DQ3_DLY_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ3_DLY_DQM2) |
        P_Fld(0x1, SHURK_SELPH_DQ3_DLY_DQM3) | P_Fld(0x3, SHURK_SELPH_DQ3_DLY_OEN_DQM0) |
        P_Fld(0x3, SHURK_SELPH_DQ3_DLY_OEN_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ3_DLY_OEN_DQM2) |
        P_Fld(0x1, SHURK_SELPH_DQ3_DLY_OEN_DQM3));
vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL1+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x017, SHURK_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ0) |
        P_Fld(0x018, SHURK_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ1));
vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL2+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x017, SHURK_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ0) |
        P_Fld(0x018, SHURK_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ1));
vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL5+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x017, SHURK_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM0) |
        P_Fld(0x018, SHURK_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM1));
vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL1+(1*SHU_GRP_DRAMC_OFFSET)+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x00a, SHURK_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ0) |
        P_Fld(0x006, SHURK_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ1));
vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL2+(1*SHU_GRP_DRAMC_OFFSET)+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x00a, SHURK_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ0) |
        P_Fld(0x006, SHURK_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ1));
vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL5+(1*SHU_GRP_DRAMC_OFFSET)+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x00a, SHURK_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM0) |
        P_Fld(0x006, SHURK_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM1));
vIO32WriteFldMulti(DRAMC_REG_SHURK_PI+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x18, SHURK_PI_RK0_ARPI_DQ_B1) |
        P_Fld(0x17, SHURK_PI_RK0_ARPI_DQ_B0) | P_Fld(0x18, SHURK_PI_RK0_ARPI_DQM_B1) |
        P_Fld(0x17, SHURK_PI_RK0_ARPI_DQM_B0));
vIO32WriteFldMulti(DRAMC_REG_SHURK_PI+(1*SHU_GRP_DRAMC_OFFSET)+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x06, SHURK_PI_RK0_ARPI_DQ_B1) |
        P_Fld(0x0a, SHURK_PI_RK0_ARPI_DQ_B0) | P_Fld(0x06, SHURK_PI_RK0_ARPI_DQM_B1) |
        P_Fld(0x0a, SHURK_PI_RK0_ARPI_DQM_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_TXDLY0+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x38, SHU_R0_B0_TXDLY0_TX_ARDQ0_DLY_B0) |
        P_Fld(0x38, SHU_R0_B0_TXDLY0_TX_ARDQ1_DLY_B0) | P_Fld(0x38, SHU_R0_B0_TXDLY0_TX_ARDQ2_DLY_B0) |
        P_Fld(0x38, SHU_R0_B0_TXDLY0_TX_ARDQ3_DLY_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_TXDLY1+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x38, SHU_R0_B0_TXDLY1_TX_ARDQ4_DLY_B0) |
        P_Fld(0x38, SHU_R0_B0_TXDLY1_TX_ARDQ5_DLY_B0) | P_Fld(0x38, SHU_R0_B0_TXDLY1_TX_ARDQ6_DLY_B0) |
        P_Fld(0x38, SHU_R0_B0_TXDLY1_TX_ARDQ7_DLY_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_TXDLY3+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x38, SHU_R0_B0_TXDLY3_TX_ARDQM0_DLY_B0) |
        P_Fld(0x00, SHU_R0_B0_TXDLY3_TX_ARWCK_DLY_B0) | P_Fld(0x00, SHU_R0_B0_TXDLY3_TX_ARWCKB_DLY_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_TXDLY0+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x2c, SHU_R0_B1_TXDLY0_TX_ARDQ0_DLY_B1) |
        P_Fld(0x2c, SHU_R0_B1_TXDLY0_TX_ARDQ1_DLY_B1) | P_Fld(0x2c, SHU_R0_B1_TXDLY0_TX_ARDQ2_DLY_B1) |
        P_Fld(0x2c, SHU_R0_B1_TXDLY0_TX_ARDQ3_DLY_B1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_TXDLY1+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x2c, SHU_R0_B1_TXDLY1_TX_ARDQ4_DLY_B1) |
        P_Fld(0x2c, SHU_R0_B1_TXDLY1_TX_ARDQ5_DLY_B1) | P_Fld(0x2c, SHU_R0_B1_TXDLY1_TX_ARDQ6_DLY_B1) |
        P_Fld(0x2c, SHU_R0_B1_TXDLY1_TX_ARDQ7_DLY_B1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_TXDLY3+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x2c, SHU_R0_B1_TXDLY3_TX_ARDQM0_DLY_B1) |
        P_Fld(0x00, SHU_R0_B1_TXDLY3_TX_ARWCK_DLY_B1) | P_Fld(0x00, SHU_R0_B1_TXDLY3_TX_ARWCKB_DLY_B1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_TXDLY0+(1*SHU_GRP_DDRPHY_OFFSET)+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x08, SHU_R0_B0_TXDLY0_TX_ARDQ0_DLY_B0) |
        P_Fld(0x08, SHU_R0_B0_TXDLY0_TX_ARDQ1_DLY_B0) | P_Fld(0x08, SHU_R0_B0_TXDLY0_TX_ARDQ2_DLY_B0) |
        P_Fld(0x08, SHU_R0_B0_TXDLY0_TX_ARDQ3_DLY_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_TXDLY1+(1*SHU_GRP_DDRPHY_OFFSET)+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x08, SHU_R0_B0_TXDLY1_TX_ARDQ4_DLY_B0) |
        P_Fld(0x08, SHU_R0_B0_TXDLY1_TX_ARDQ5_DLY_B0) | P_Fld(0x08, SHU_R0_B0_TXDLY1_TX_ARDQ6_DLY_B0) |
        P_Fld(0x08, SHU_R0_B0_TXDLY1_TX_ARDQ7_DLY_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_TXDLY3+(1*SHU_GRP_DDRPHY_OFFSET)+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x08, SHU_R0_B0_TXDLY3_TX_ARDQM0_DLY_B0) |
        P_Fld(0x00, SHU_R0_B0_TXDLY3_TX_ARWCK_DLY_B0) | P_Fld(0x00, SHU_R0_B0_TXDLY3_TX_ARWCKB_DLY_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_TXDLY0+(1*SHU_GRP_DDRPHY_OFFSET)+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x3c, SHU_R0_B1_TXDLY0_TX_ARDQ0_DLY_B1) |
        P_Fld(0x3c, SHU_R0_B1_TXDLY0_TX_ARDQ1_DLY_B1) | P_Fld(0x3c, SHU_R0_B1_TXDLY0_TX_ARDQ2_DLY_B1) |
        P_Fld(0x3c, SHU_R0_B1_TXDLY0_TX_ARDQ3_DLY_B1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_TXDLY1+(1*SHU_GRP_DDRPHY_OFFSET)+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x3c, SHU_R0_B1_TXDLY1_TX_ARDQ4_DLY_B1) |
        P_Fld(0x3c, SHU_R0_B1_TXDLY1_TX_ARDQ5_DLY_B1) | P_Fld(0x3c, SHU_R0_B1_TXDLY1_TX_ARDQ6_DLY_B1) |
        P_Fld(0x3c, SHU_R0_B1_TXDLY1_TX_ARDQ7_DLY_B1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_TXDLY3+(1*SHU_GRP_DDRPHY_OFFSET)+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x3c, SHU_R0_B1_TXDLY3_TX_ARDQM0_DLY_B1) |
        P_Fld(0x00, SHU_R0_B1_TXDLY3_TX_ARWCK_DLY_B1) | P_Fld(0x00, SHU_R0_B1_TXDLY3_TX_ARWCKB_DLY_B1));
vIO32WriteFldMulti(DRAMC_REG_SHU_TX_RANKCTL+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x1, SHU_TX_RANKCTL_TXRANKINCTL_TXDLY) |
        P_Fld(0x1, SHU_TX_RANKCTL_TXRANKINCTL) | P_Fld(0x0, SHU_TX_RANKCTL_TXRANKINCTL_ROOT));

vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_CA1+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x0, SHU_SELPH_CA1_TXDLY_CS) |
        P_Fld(0x0, SHU_SELPH_CA1_TXDLY_CKE) | P_Fld(0x0, SHU_SELPH_CA1_TXDLY_ODT) |
        P_Fld(0x0, SHU_SELPH_CA1_TXDLY_RESET) | P_Fld(0x0, SHU_SELPH_CA1_TXDLY_WE) |
        P_Fld(0x0, SHU_SELPH_CA1_TXDLY_CAS) | P_Fld(0x0, SHU_SELPH_CA1_TXDLY_RAS) |
        P_Fld(0x0, SHU_SELPH_CA1_TXDLY_CS1));
vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_CA2+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x0, SHU_SELPH_CA2_TXDLY_BA0) |
        P_Fld(0x0, SHU_SELPH_CA2_TXDLY_BA1) | P_Fld(0x0, SHU_SELPH_CA2_TXDLY_BA2) |
        P_Fld(0x01, SHU_SELPH_CA2_TXDLY_CMD) | P_Fld(0x0, SHU_SELPH_CA2_TXDLY_CKE1));
vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_CA3+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x0, SHU_SELPH_CA3_TXDLY_RA0) |
        P_Fld(0x0, SHU_SELPH_CA3_TXDLY_RA1) | P_Fld(0x0, SHU_SELPH_CA3_TXDLY_RA2) |
        P_Fld(0x0, SHU_SELPH_CA3_TXDLY_RA3) | P_Fld(0x0, SHU_SELPH_CA3_TXDLY_RA4) |
        P_Fld(0x0, SHU_SELPH_CA3_TXDLY_RA5) | P_Fld(0x0, SHU_SELPH_CA3_TXDLY_RA6) |
        P_Fld(0x0, SHU_SELPH_CA3_TXDLY_RA7));
vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_CA4+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x0, SHU_SELPH_CA4_TXDLY_RA8) |
        P_Fld(0x0, SHU_SELPH_CA4_TXDLY_RA9) | P_Fld(0x0, SHU_SELPH_CA4_TXDLY_RA10) |
        P_Fld(0x0, SHU_SELPH_CA4_TXDLY_RA11) | P_Fld(0x0, SHU_SELPH_CA4_TXDLY_RA12) |
        P_Fld(0x0, SHU_SELPH_CA4_TXDLY_RA13) | P_Fld(0x0, SHU_SELPH_CA4_TXDLY_RA14) |
        P_Fld(0x0, SHU_SELPH_CA4_TXDLY_RA15));
vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_CA5+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x1, SHU_SELPH_CA5_DLY_CS) |
        P_Fld(0x1, SHU_SELPH_CA5_DLY_CKE) | P_Fld(0x0, SHU_SELPH_CA5_DLY_ODT) |
        P_Fld(0x1, SHU_SELPH_CA5_DLY_RESET) | P_Fld(0x1, SHU_SELPH_CA5_DLY_WE) |
        P_Fld(0x1, SHU_SELPH_CA5_DLY_CAS) | P_Fld(0x1, SHU_SELPH_CA5_DLY_RAS) |
        P_Fld(0x1, SHU_SELPH_CA5_DLY_CS1));

vIO32WriteFldMulti(DRAMC_REG_SHU_SREF_CTRL+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x3, SHU_SREF_CTRL_CKEHCMD) |
        P_Fld(0x3, SHU_SREF_CTRL_SREF_CK_DLY));
vIO32WriteFldMulti(DRAMC_REG_SHU_HMR4_DVFS_CTRL0+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x4b, SHU_HMR4_DVFS_CTRL0_FSPCHG_PRDCNT) |
        P_Fld(0x000, SHU_HMR4_DVFS_CTRL0_REFRCNT));
vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM_XRT+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x06, SHU_ACTIM_XRT_XRTR2R) |
        P_Fld(0x0a, SHU_ACTIM_XRT_XRTR2W) | P_Fld(0x6, SHU_ACTIM_XRT_XRTW2R) |
        P_Fld(0x07, SHU_ACTIM_XRT_XRTW2W));
vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM0+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0xa, SHU_ACTIM0_TWTR) |
        P_Fld(0x3, SHU_ACTIM0_CKELCKCNT) | P_Fld(0x0e, SHU_ACTIM0_TWR) |
        P_Fld(0x3, SHU_ACTIM0_TRRD) | P_Fld(0x6, SHU_ACTIM0_TRCD) |
        P_Fld(0x3, SHU_ACTIM0_TWTR_L));
vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM1+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x5, SHU_ACTIM1_TRPAB) |
        P_Fld(0x7, SHU_ACTIM1_TMRWCKEL) | P_Fld(0x4, SHU_ACTIM1_TRP) |
        P_Fld(0x05, SHU_ACTIM1_TRAS) | P_Fld(0x0a, SHU_ACTIM1_TRC));
vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM2+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x1, SHU_ACTIM2_TXP) |
        P_Fld(0x08, SHU_ACTIM2_TMRRI) | P_Fld(0x1, SHU_ACTIM2_TRTP) |
        P_Fld(0x0a, SHU_ACTIM2_TR2W) | P_Fld(0x05, SHU_ACTIM2_TFAW));
vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM3+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x1f, SHU_ACTIM3_TRFCPB) |
        P_Fld(0x8, SHU_ACTIM3_MANTMRR) | P_Fld(0x8, SHU_ACTIM3_TR2MRR) |
        P_Fld(0x49, SHU_ACTIM3_TRFC));
vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM4+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x057, SHU_ACTIM4_TXREFCNT) |
        P_Fld(0x0f, SHU_ACTIM4_TMRR2MRW) | P_Fld(0x0c, SHU_ACTIM4_TMRR2W) |
        P_Fld(0x1a, SHU_ACTIM4_TZQCS));
vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM5+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x10, SHU_ACTIM5_TR2PD) |
        P_Fld(0x11, SHU_ACTIM5_TWTPD) | P_Fld(0x1c, SHU_ACTIM5_TPBR2PBR) |
        P_Fld(0x0, SHU_ACTIM5_TPBR2ACT));
vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM6+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x0a, SHU_ACTIM6_TZQLAT2) |
        P_Fld(0x5, SHU_ACTIM6_TMRD) | P_Fld(0x5, SHU_ACTIM6_TMRW) |
        P_Fld(0x0d, SHU_ACTIM6_TW2MRW) | P_Fld(0x12, SHU_ACTIM6_TR2MRW));
vIO32WriteFldMulti(DRAMC_REG_SHU_CKECTRL+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x0, SHU_CKECTRL_TPDE_05T) |
        P_Fld(0x0, SHU_CKECTRL_TPDX_05T) | P_Fld(0x3, SHU_CKECTRL_TPDE) |
        P_Fld(0x2, SHU_CKECTRL_TPDX) | P_Fld(0x2, SHU_CKECTRL_TCKEPRD) |
        P_Fld(0x3, SHU_CKECTRL_TCKESRX));
vIO32WriteFldMulti(DRAMC_REG_SHU_MISC+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x2, SHU_MISC_REQQUE_MAXCNT) |
        P_Fld(0x7, SHU_MISC_DCMDLYREF) | P_Fld(0x0, SHU_MISC_DAREFEN));
vIO32WriteFldMulti(DDRPHY_REG_SHU_MISC_TX_PIPE_CTRL+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x0, SHU_MISC_TX_PIPE_CTRL_CMD_TXPIPE_BYPASS_EN) |
        P_Fld(0x1, SHU_MISC_TX_PIPE_CTRL_CK_TXPIPE_BYPASS_EN) | P_Fld(0x0, SHU_MISC_TX_PIPE_CTRL_TX_PIPE_BYPASS_EN) |
        P_Fld(0x0, SHU_MISC_TX_PIPE_CTRL_CS_TXPIPE_BYPASS_EN) | P_Fld(0x0, SHU_MISC_TX_PIPE_CTRL_SKIP_TXPIPE_BYPASS));

vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ8+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x004a, SHU_B0_DQ8_R_DMRXDVS_UPD_FORCE_CYC_B0) |
        P_Fld(0x0, SHU_B0_DQ8_R_DMRXDVS_UPD_FORCE_EN_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B0) |
        P_Fld(0x0, SHU_B0_DQ8_R_RMRODTEN_CG_IG_B0) | P_Fld(0x1, SHU_B0_DQ8_R_RMRX_TOPHY_CG_IG_B0) |
        P_Fld(0x0, SHU_B0_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B0) |
        P_Fld(0x1, SHU_B0_DQ8_R_DMRXDLY_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B0) |
        P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B0) |
        P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMRANK_PIPE_CG_IG_B0) |
        P_Fld(0x0, SHU_B0_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ8+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x004a, SHU_B1_DQ8_R_DMRXDVS_UPD_FORCE_CYC_B1) |
        P_Fld(0x0, SHU_B1_DQ8_R_DMRXDVS_UPD_FORCE_EN_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B1) |
        P_Fld(0x0, SHU_B1_DQ8_R_RMRODTEN_CG_IG_B1) | P_Fld(0x1, SHU_B1_DQ8_R_RMRX_TOPHY_CG_IG_B1) |
        P_Fld(0x0, SHU_B1_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B1) |
        P_Fld(0x1, SHU_B1_DQ8_R_DMRXDLY_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B1) |
        P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B1) |
        P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMRANK_PIPE_CG_IG_B1) |
        P_Fld(0x0, SHU_B1_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ5+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x0e, SHU_B0_DQ5_RG_RX_ARDQ_VREF_SEL_B0) |
        P_Fld(0x0, SHU_B0_DQ5_RG_RX_ARDQ_VREF_BYPASS_B0) | P_Fld(0x00, SHU_B0_DQ5_RG_ARPI_FB_B0) |
        P_Fld(0x0, SHU_B0_DQ5_RG_RX_ARDQS0_DQSIEN_DLY_B0) | P_Fld(0x0, SHU_B0_DQ5_RG_RX_ARDQS_DQSIEN_RB_DLY_B0) |
        P_Fld(0x6, SHU_B0_DQ5_RG_RX_ARDQS0_DVS_DLY_B0) | P_Fld(0x0, SHU_B0_DQ5_RG_RX_ARDQ_FIFO_DQSI_DLY_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ5+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x0e, SHU_B1_DQ5_RG_RX_ARDQ_VREF_SEL_B1) |
        P_Fld(0x0, SHU_B1_DQ5_RG_RX_ARDQ_VREF_BYPASS_B1) | P_Fld(0x00, SHU_B1_DQ5_RG_ARPI_FB_B1) |
        P_Fld(0x0, SHU_B1_DQ5_RG_RX_ARDQS0_DQSIEN_DLY_B1) | P_Fld(0x0, SHU_B1_DQ5_RG_RX_ARDQS_DQSIEN_RB_DLY_B1) |
        P_Fld(0x6, SHU_B1_DQ5_RG_RX_ARDQS0_DVS_DLY_B1) | P_Fld(0x0, SHU_B1_DQ5_RG_RX_ARDQ_FIFO_DQSI_DLY_B1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY0+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0xe5, SHU_R0_B0_RXDLY0_RX_ARDQ0_R_DLY_B0) |
        P_Fld(0xe5, SHU_R0_B0_RXDLY0_RX_ARDQ0_F_DLY_B0) | P_Fld(0xe5, SHU_R0_B0_RXDLY0_RX_ARDQ1_R_DLY_B0) |
        P_Fld(0xe5, SHU_R0_B0_RXDLY0_RX_ARDQ1_F_DLY_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY1+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0xe5, SHU_R0_B0_RXDLY1_RX_ARDQ2_R_DLY_B0) |
        P_Fld(0xe5, SHU_R0_B0_RXDLY1_RX_ARDQ2_F_DLY_B0) | P_Fld(0xe5, SHU_R0_B0_RXDLY1_RX_ARDQ3_R_DLY_B0) |
        P_Fld(0xe5, SHU_R0_B0_RXDLY1_RX_ARDQ3_F_DLY_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY2+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0xe5, SHU_R0_B0_RXDLY2_RX_ARDQ4_R_DLY_B0) |
        P_Fld(0xe5, SHU_R0_B0_RXDLY2_RX_ARDQ4_F_DLY_B0) | P_Fld(0xe5, SHU_R0_B0_RXDLY2_RX_ARDQ5_R_DLY_B0) |
        P_Fld(0xe5, SHU_R0_B0_RXDLY2_RX_ARDQ5_F_DLY_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY3+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0xe5, SHU_R0_B0_RXDLY3_RX_ARDQ6_R_DLY_B0) |
        P_Fld(0xe5, SHU_R0_B0_RXDLY3_RX_ARDQ6_F_DLY_B0) | P_Fld(0xe5, SHU_R0_B0_RXDLY3_RX_ARDQ7_R_DLY_B0) |
        P_Fld(0xe5, SHU_R0_B0_RXDLY3_RX_ARDQ7_F_DLY_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY4+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0xe5, SHU_R0_B0_RXDLY4_RX_ARDQM0_R_DLY_B0) |
        P_Fld(0xe5, SHU_R0_B0_RXDLY4_RX_ARDQM0_F_DLY_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY5+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x18a, SHU_R0_B0_RXDLY5_RX_ARDQS0_R_DLY_B0) |
        P_Fld(0x18a, SHU_R0_B0_RXDLY5_RX_ARDQS0_F_DLY_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY0+(1*SHU_GRP_DDRPHY_OFFSET)+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0xe4, SHU_R0_B0_RXDLY0_RX_ARDQ0_R_DLY_B0) |
        P_Fld(0xe4, SHU_R0_B0_RXDLY0_RX_ARDQ0_F_DLY_B0) | P_Fld(0xe4, SHU_R0_B0_RXDLY0_RX_ARDQ1_R_DLY_B0) |
        P_Fld(0xe4, SHU_R0_B0_RXDLY0_RX_ARDQ1_F_DLY_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY1+(1*SHU_GRP_DDRPHY_OFFSET)+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0xe4, SHU_R0_B0_RXDLY1_RX_ARDQ2_R_DLY_B0) |
        P_Fld(0xe4, SHU_R0_B0_RXDLY1_RX_ARDQ2_F_DLY_B0) | P_Fld(0xe4, SHU_R0_B0_RXDLY1_RX_ARDQ3_R_DLY_B0) |
        P_Fld(0xe4, SHU_R0_B0_RXDLY1_RX_ARDQ3_F_DLY_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY2+(1*SHU_GRP_DDRPHY_OFFSET)+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0xe4, SHU_R0_B0_RXDLY2_RX_ARDQ4_R_DLY_B0) |
        P_Fld(0xe4, SHU_R0_B0_RXDLY2_RX_ARDQ4_F_DLY_B0) | P_Fld(0xe4, SHU_R0_B0_RXDLY2_RX_ARDQ5_R_DLY_B0) |
        P_Fld(0xe4, SHU_R0_B0_RXDLY2_RX_ARDQ5_F_DLY_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY3+(1*SHU_GRP_DDRPHY_OFFSET)+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0xe4, SHU_R0_B0_RXDLY3_RX_ARDQ6_R_DLY_B0) |
        P_Fld(0xe4, SHU_R0_B0_RXDLY3_RX_ARDQ6_F_DLY_B0) | P_Fld(0xe4, SHU_R0_B0_RXDLY3_RX_ARDQ7_R_DLY_B0) |
        P_Fld(0xe4, SHU_R0_B0_RXDLY3_RX_ARDQ7_F_DLY_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY4+(1*SHU_GRP_DDRPHY_OFFSET)+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0xe4, SHU_R0_B0_RXDLY4_RX_ARDQM0_R_DLY_B0) |
        P_Fld(0xe4, SHU_R0_B0_RXDLY4_RX_ARDQM0_F_DLY_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY5+(1*SHU_GRP_DDRPHY_OFFSET)+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x189, SHU_R0_B0_RXDLY5_RX_ARDQS0_R_DLY_B0) |
        P_Fld(0x189, SHU_R0_B0_RXDLY5_RX_ARDQS0_F_DLY_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY0+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0xe5, SHU_R0_B1_RXDLY0_RX_ARDQ0_R_DLY_B1) |
        P_Fld(0xe5, SHU_R0_B1_RXDLY0_RX_ARDQ0_F_DLY_B1) | P_Fld(0xe5, SHU_R0_B1_RXDLY0_RX_ARDQ1_R_DLY_B1) |
        P_Fld(0xe5, SHU_R0_B1_RXDLY0_RX_ARDQ1_F_DLY_B1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY1+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0xe5, SHU_R0_B1_RXDLY1_RX_ARDQ2_R_DLY_B1) |
        P_Fld(0xe5, SHU_R0_B1_RXDLY1_RX_ARDQ2_F_DLY_B1) | P_Fld(0xe5, SHU_R0_B1_RXDLY1_RX_ARDQ3_R_DLY_B1) |
        P_Fld(0xe5, SHU_R0_B1_RXDLY1_RX_ARDQ3_F_DLY_B1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY2+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0xe5, SHU_R0_B1_RXDLY2_RX_ARDQ4_R_DLY_B1) |
        P_Fld(0xe5, SHU_R0_B1_RXDLY2_RX_ARDQ4_F_DLY_B1) | P_Fld(0xe5, SHU_R0_B1_RXDLY2_RX_ARDQ5_R_DLY_B1) |
        P_Fld(0xe5, SHU_R0_B1_RXDLY2_RX_ARDQ5_F_DLY_B1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY3+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0xe5, SHU_R0_B1_RXDLY3_RX_ARDQ6_R_DLY_B1) |
        P_Fld(0xe5, SHU_R0_B1_RXDLY3_RX_ARDQ6_F_DLY_B1) | P_Fld(0xe5, SHU_R0_B1_RXDLY3_RX_ARDQ7_R_DLY_B1) |
        P_Fld(0xe5, SHU_R0_B1_RXDLY3_RX_ARDQ7_F_DLY_B1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY4+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0xe5, SHU_R0_B1_RXDLY4_RX_ARDQM0_R_DLY_B1) |
        P_Fld(0xe5, SHU_R0_B1_RXDLY4_RX_ARDQM0_F_DLY_B1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY5+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x18a, SHU_R0_B1_RXDLY5_RX_ARDQS0_R_DLY_B1) |
        P_Fld(0x18a, SHU_R0_B1_RXDLY5_RX_ARDQS0_F_DLY_B1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY0+(1*SHU_GRP_DDRPHY_OFFSET)+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0xe4, SHU_R0_B1_RXDLY0_RX_ARDQ0_R_DLY_B1) |
        P_Fld(0xe4, SHU_R0_B1_RXDLY0_RX_ARDQ0_F_DLY_B1) | P_Fld(0xe4, SHU_R0_B1_RXDLY0_RX_ARDQ1_R_DLY_B1) |
        P_Fld(0xe4, SHU_R0_B1_RXDLY0_RX_ARDQ1_F_DLY_B1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY1+(1*SHU_GRP_DDRPHY_OFFSET)+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0xe4, SHU_R0_B1_RXDLY1_RX_ARDQ2_R_DLY_B1) |
        P_Fld(0xe4, SHU_R0_B1_RXDLY1_RX_ARDQ2_F_DLY_B1) | P_Fld(0xe4, SHU_R0_B1_RXDLY1_RX_ARDQ3_R_DLY_B1) |
        P_Fld(0xe4, SHU_R0_B1_RXDLY1_RX_ARDQ3_F_DLY_B1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY2+(1*SHU_GRP_DDRPHY_OFFSET)+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0xe4, SHU_R0_B1_RXDLY2_RX_ARDQ4_R_DLY_B1) |
        P_Fld(0xe4, SHU_R0_B1_RXDLY2_RX_ARDQ4_F_DLY_B1) | P_Fld(0xe4, SHU_R0_B1_RXDLY2_RX_ARDQ5_R_DLY_B1) |
        P_Fld(0xe4, SHU_R0_B1_RXDLY2_RX_ARDQ5_F_DLY_B1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY3+(1*SHU_GRP_DDRPHY_OFFSET)+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0xe4, SHU_R0_B1_RXDLY3_RX_ARDQ6_R_DLY_B1) |
        P_Fld(0xe4, SHU_R0_B1_RXDLY3_RX_ARDQ6_F_DLY_B1) | P_Fld(0xe4, SHU_R0_B1_RXDLY3_RX_ARDQ7_R_DLY_B1) |
        P_Fld(0xe4, SHU_R0_B1_RXDLY3_RX_ARDQ7_F_DLY_B1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY4+(1*SHU_GRP_DDRPHY_OFFSET)+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0xe4, SHU_R0_B1_RXDLY4_RX_ARDQM0_R_DLY_B1) |
        P_Fld(0xe4, SHU_R0_B1_RXDLY4_RX_ARDQM0_F_DLY_B1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY5+(1*SHU_GRP_DDRPHY_OFFSET)+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x189, SHU_R0_B1_RXDLY5_RX_ARDQS0_R_DLY_B1) |
        P_Fld(0x189, SHU_R0_B1_RXDLY5_RX_ARDQS0_F_DLY_B1));

vIO32WriteFldMulti(DRAMC_REG_SHU_COMMON0+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x0, SHU_COMMON0_FREQDIV4) |
        P_Fld(0x1, SHU_COMMON0_FDIV2) | P_Fld(0x0, SHU_COMMON0_FREQDIV8) |
        P_Fld(0x0, SHU_COMMON0_DM64BITEN) | P_Fld(0x0, SHU_COMMON0_DLE256EN) |
        P_Fld(0x0, SHU_COMMON0_LP5BGEN) | P_Fld(0x0, SHU_COMMON0_LP5WCKON) |
        P_Fld(0x0, SHU_COMMON0_CL2) | P_Fld(0x0, SHU_COMMON0_BL2) |
        P_Fld(0x1, SHU_COMMON0_BL4) | P_Fld(0x0, SHU_COMMON0_LP5BGOTF) |
        P_Fld(0x1, SHU_COMMON0_BC4OTF) | P_Fld(0x0, SHU_COMMON0_LP5HEFF_MODE) |
        P_Fld(0x00000, SHU_COMMON0_SHU_COMMON0_RSV));
vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIMING_CONF+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x26, SHU_ACTIMING_CONF_SCINTV) |
        P_Fld(0x0, SHU_ACTIMING_CONF_TRFCPBIG) | P_Fld(0x000, SHU_ACTIMING_CONF_REFBW_FR) |
        P_Fld(0x1, SHU_ACTIMING_CONF_TREFBWIG));
vIO32WriteFldMulti(DRAMC_REG_SHU_DCM_CTRL0+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x1, SHU_DCM_CTRL0_DDRPHY_CLK_EN_OPT) |
        P_Fld(0x4, SHU_DCM_CTRL0_DPHY_CMDDCM_EXTCNT) | P_Fld(0x6, SHU_DCM_CTRL0_DDRPHY_CLK_DYN_GATING_SEL) |
        P_Fld(0x0, SHU_DCM_CTRL0_CKE_EXTNONPD_CNT) | P_Fld(0x1, SHU_DCM_CTRL0_FASTWAKE2) |
        P_Fld(0x1, SHU_DCM_CTRL0_FASTWAKE));
vIO32WriteFldMulti(DRAMC_REG_SHU_CONF0+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x3f, SHU_CONF0_DMPGTIM) |
        P_Fld(0x0, SHU_CONF0_ADVREFEN) | P_Fld(0x1, SHU_CONF0_ADVPREEN) |
        P_Fld(0x1, SHU_CONF0_PBREFEN) | P_Fld(0x1, SHU_CONF0_REFTHD) |
        P_Fld(0x8, SHU_CONF0_REQQUE_DEPTH));
vIO32WriteFldMulti(DRAMC_REG_SHU_MATYPE+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x2, SHU_MATYPE_MATYPE) |
        P_Fld(0x1, SHU_MATYPE_NORMPOP_LEN));
vIO32WriteFldMulti(DRAMC_REG_SHU_TX_SET0+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x0, SHU_TX_SET0_DQOE_CNT) |
        P_Fld(0x0, SHU_TX_SET0_DQOE_OPT) | P_Fld(0x0, SHU_TX_SET0_TXUPD_SEL) |
        P_Fld(0x2, SHU_TX_SET0_TXUPD_W2R_SEL) | P_Fld(0x0, SHU_TX_SET0_WECC_EN) |
        P_Fld(0x0, SHU_TX_SET0_DBIWR) | P_Fld(0x1, SHU_TX_SET0_WDATRGO) |
        P_Fld(0x0, SHU_TX_SET0_TWPSTEXT) | P_Fld(0x0, SHU_TX_SET0_WPST1P5T) |
        P_Fld(0x3, SHU_TX_SET0_TXOEN_AUTOSET_OFFSET) | P_Fld(0x1, SHU_TX_SET0_TWCKPST) |
        P_Fld(0x0, SHU_TX_SET0_OE_EXT2UI) | P_Fld(0x0e, SHU_TX_SET0_DQS2DQ_FILT_PITHRD) |
        P_Fld(0x0, SHU_TX_SET0_TXOEN_AUTOSET_EN));
vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_STBCAL1+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x0, MISC_SHU_STBCAL1_DLLFRZRFCOPT) |
        P_Fld(0x0, MISC_SHU_STBCAL1_DLLFRZWROPT) | P_Fld(0x0, MISC_SHU_STBCAL1_R_RSTBCNT_LATCH_OPT) |
        P_Fld(0x1, MISC_SHU_STBCAL1_STB_UPDMASK_EN) | P_Fld(0x9, MISC_SHU_STBCAL1_STB_UPDMASKCYC) |
        P_Fld(0x0, MISC_SHU_STBCAL1_DQSINCTL_PRE_SEL));
vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_STBCAL+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x0, MISC_SHU_STBCAL_DMSTBLAT) |
        P_Fld(0x1, MISC_SHU_STBCAL_PICGLAT) | P_Fld(0x1, MISC_SHU_STBCAL_DQSG_MODE) |
        P_Fld(0x1, MISC_SHU_STBCAL_DQSIEN_PICG_MODE) | P_Fld(0x1, MISC_SHU_STBCAL_DQSIEN_DQSSTB_MODE) |
        P_Fld(0x1, MISC_SHU_STBCAL_DQSIEN_BURST_MODE) | P_Fld(0x0, MISC_SHU_STBCAL_DQSIEN_SELPH_FRUN) |
        P_Fld(0x1, MISC_SHU_STBCAL_STBCALEN) | P_Fld(0x1, MISC_SHU_STBCAL_STB_SELPHCALEN) |
        P_Fld(0x0, MISC_SHU_STBCAL_DQSIEN_4TO1_EN) | P_Fld(0x0, MISC_SHU_STBCAL_DQSIEN_8TO1_EN) |
        P_Fld(0x0, MISC_SHU_STBCAL_DQSIEN_16TO1_EN));
vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RK_DQSIEN_PICG_CTRL+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x0, MISC_SHU_RK_DQSIEN_PICG_CTRL_DQSIEN_PICG_HEAD_EXT_LAT) |
        P_Fld(0x1, MISC_SHU_RK_DQSIEN_PICG_CTRL_DQSIEN_PICG_TAIL_EXT_LAT));
vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RK_DQSIEN_PICG_CTRL+(1*SHU_GRP_DDRPHY_OFFSET)+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x0, MISC_SHU_RK_DQSIEN_PICG_CTRL_DQSIEN_PICG_HEAD_EXT_LAT) |
        P_Fld(0x1, MISC_SHU_RK_DQSIEN_PICG_CTRL_DQSIEN_PICG_TAIL_EXT_LAT));
vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RODTENSTB+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x1, MISC_SHU_RODTENSTB_RODTENSTB_TRACK_EN) |
        P_Fld(0x0, MISC_SHU_RODTENSTB_RODTEN_P1_ENABLE) | P_Fld(0x0, MISC_SHU_RODTENSTB_RODTENSTB_4BYTE_EN) |
        P_Fld(0x1, MISC_SHU_RODTENSTB_RODTENSTB_TRACK_UDFLWCTRL) | P_Fld(0x1, MISC_SHU_RODTENSTB_RODTENSTB_SELPH_MODE) |
        P_Fld(0x0, MISC_SHU_RODTENSTB_RODTENSTB_SELPH_BY_BITTIME) | P_Fld(0x1, MISC_SHU_RODTENSTB_RODTENSTB__UI_OFFSET) |
        P_Fld(0x2, MISC_SHU_RODTENSTB_RODTENSTB_MCK_OFFSET) | P_Fld(0x0008, MISC_SHU_RODTENSTB_RODTENSTB_EXT));
vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RX_SELPH_MODE+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x1, MISC_SHU_RX_SELPH_MODE_DQSIEN_SELPH_SERMODE) |
        P_Fld(0x0, MISC_SHU_RX_SELPH_MODE_RODT_SELPH_SERMODE) | P_Fld(0x0, MISC_SHU_RX_SELPH_MODE_RANK_SELPH_SERMODE));

vIO32WriteFldMulti(DRAMC_REG_SHU_HWSET_MR13+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x000d, SHU_HWSET_MR13_HWSET_MR13_MRSMA) |
        P_Fld(0x08, SHU_HWSET_MR13_HWSET_MR13_OP));
vIO32WriteFldMulti(DRAMC_REG_SHU_HWSET_VRCG+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x000d, SHU_HWSET_VRCG_HWSET_VRCG_MRSMA) |
        P_Fld(0x00, SHU_HWSET_VRCG_HWSET_VRCG_OP) | P_Fld(0x00, SHU_HWSET_VRCG_VRCGDIS_PRDCNT));

vIO32WriteFldMulti(DRAMC_REG_SHU_FREQ_RATIO_SET0+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x00, SHU_FREQ_RATIO_SET0_TDQSCK_JUMP_RATIO3) |
        P_Fld(0x00, SHU_FREQ_RATIO_SET0_TDQSCK_JUMP_RATIO2) | P_Fld(0x20, SHU_FREQ_RATIO_SET0_TDQSCK_JUMP_RATIO1) |
        P_Fld(0x2b, SHU_FREQ_RATIO_SET0_TDQSCK_JUMP_RATIO0));

vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_DVFSDLL+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x1, MISC_SHU_DVFSDLL_R_BYPASS_1ST_DLL) |
        P_Fld(0x0, MISC_SHU_DVFSDLL_R_BYPASS_2ND_DLL) | P_Fld(0x5a, MISC_SHU_DVFSDLL_R_DLL_IDLE) |
        P_Fld(0x5a, MISC_SHU_DVFSDLL_R_2ND_DLL_IDLE));

    mcDELAY_US(1);

    mcDELAY_US(1);

     DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);

    mcDELAY_US(1);

    mcDELAY_US(1);

     DramcBroadcastOnOff(DRAMC_BROADCAST_ON);

vIO32WriteFldMulti(DRAMC_REG_SHU_DQSOSCR+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x0c, SHU_DQSOSCR_DQSOSCRCNT) |
        P_Fld(0x0, SHU_DQSOSCR_DQSOSC_ADV_SEL) | P_Fld(0x0, SHU_DQSOSCR_DQSOSC_DRS_ADV_SEL) |
        P_Fld(0xffff, SHU_DQSOSCR_DQSOSC_DELTA));
vIO32WriteFldMulti(DRAMC_REG_SHU_DQSOSC_SET0+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x1, SHU_DQSOSC_SET0_DQSOSCENDIS) |
        P_Fld(0x021, SHU_DQSOSC_SET0_DQSOSC_PRDCNT) | P_Fld(0x0002, SHU_DQSOSC_SET0_DQSOSCENCNT));
vIO32WriteFldMulti(DRAMC_REG_SHURK_DQSOSC+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x0b06, SHURK_DQSOSC_DQSOSC_BASE_RK0) |
        P_Fld(0x0b06, SHURK_DQSOSC_DQSOSC_BASE_RK0_B1));
vIO32WriteFldMulti(DRAMC_REG_SHURK_DQSOSC+(1*SHU_GRP_DRAMC_OFFSET)+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x04b9, SHURK_DQSOSC_DQSOSC_BASE_RK0) |
        P_Fld(0x04b9, SHURK_DQSOSC_DQSOSC_BASE_RK0_B1));
vIO32WriteFldMulti(DRAMC_REG_SHURK_DQSOSC_THRD+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x128, SHURK_DQSOSC_THRD_DQSOSCTHRD_INC) |
        P_Fld(0x0c5, SHURK_DQSOSC_THRD_DQSOSCTHRD_DEC));
vIO32WriteFldMulti(DRAMC_REG_SHURK_DQSOSC_THRD+(1*SHU_GRP_DRAMC_OFFSET)+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x036, SHURK_DQSOSC_THRD_DQSOSCTHRD_INC) |
        P_Fld(0x024, SHURK_DQSOSC_THRD_DQSOSCTHRD_DEC));
vIO32WriteFldMulti(DRAMC_REG_SHU_TX_SET0+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x0, SHU_TX_SET0_DQOE_CNT) |
        P_Fld(0x0, SHU_TX_SET0_DQOE_OPT) | P_Fld(0x0, SHU_TX_SET0_TXUPD_SEL) |
        P_Fld(0x2, SHU_TX_SET0_TXUPD_W2R_SEL) | P_Fld(0x0, SHU_TX_SET0_WECC_EN) |
        P_Fld(0x0, SHU_TX_SET0_DBIWR) | P_Fld(0x1, SHU_TX_SET0_WDATRGO) |
        P_Fld(0x0, SHU_TX_SET0_TWPSTEXT) | P_Fld(0x0, SHU_TX_SET0_WPST1P5T) |
        P_Fld(0x3, SHU_TX_SET0_TXOEN_AUTOSET_OFFSET) | P_Fld(0x1, SHU_TX_SET0_TWCKPST) |
        P_Fld(0x0, SHU_TX_SET0_OE_EXT2UI) | P_Fld(0x06, SHU_TX_SET0_DQS2DQ_FILT_PITHRD) |
        P_Fld(0x0, SHU_TX_SET0_TXOEN_AUTOSET_EN));
vIO32WriteFldMulti(DRAMC_REG_SHU_DQSOSC_SET0+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x0, SHU_DQSOSC_SET0_DQSOSCENDIS) |
        P_Fld(0x021, SHU_DQSOSC_SET0_DQSOSC_PRDCNT) | P_Fld(0x0002, SHU_DQSOSC_SET0_DQSOSCENCNT));
vIO32WriteFldMulti(DRAMC_REG_SHU_ZQ_SET0+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x0000, SHU_ZQ_SET0_ZQCSCNT) |
        P_Fld(0x1d, SHU_ZQ_SET0_TZQLAT));
vIO32WriteFldMulti(DRAMC_REG_SHU_ZQ_SET0+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x0005, SHU_ZQ_SET0_ZQCSCNT) |
        P_Fld(0x1d, SHU_ZQ_SET0_TZQLAT));
vIO32WriteFldMulti(DRAMC_REG_SHU_HMR4_DVFS_CTRL0+(1*SHU_GRP_DRAMC_OFFSET), P_Fld(0x4b, SHU_HMR4_DVFS_CTRL0_FSPCHG_PRDCNT) |
        P_Fld(0x005, SHU_HMR4_DVFS_CTRL0_REFRCNT));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ8+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x004a, SHU_B0_DQ8_R_DMRXDVS_UPD_FORCE_CYC_B0) |
        P_Fld(0x1, SHU_B0_DQ8_R_DMRXDVS_UPD_FORCE_EN_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B0) |
        P_Fld(0x0, SHU_B0_DQ8_R_RMRODTEN_CG_IG_B0) | P_Fld(0x1, SHU_B0_DQ8_R_RMRX_TOPHY_CG_IG_B0) |
        P_Fld(0x0, SHU_B0_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B0) |
        P_Fld(0x1, SHU_B0_DQ8_R_DMRXDLY_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B0) |
        P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B0) |
        P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMRANK_PIPE_CG_IG_B0) |
        P_Fld(0x0, SHU_B0_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ8+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x004a, SHU_B1_DQ8_R_DMRXDVS_UPD_FORCE_CYC_B1) |
        P_Fld(0x1, SHU_B1_DQ8_R_DMRXDVS_UPD_FORCE_EN_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B1) |
        P_Fld(0x0, SHU_B1_DQ8_R_RMRODTEN_CG_IG_B1) | P_Fld(0x1, SHU_B1_DQ8_R_RMRX_TOPHY_CG_IG_B1) |
        P_Fld(0x0, SHU_B1_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B1) |
        P_Fld(0x1, SHU_B1_DQ8_R_DMRXDLY_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B1) |
        P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B1) |
        P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMRANK_PIPE_CG_IG_B1) |
        P_Fld(0x0, SHU_B1_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ7+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x0, SHU_B0_DQ7_R_DMRANKRXDVS_B0) |
        P_Fld(0x0, SHU_B0_DQ7_R_DMDQMDBI_EYE_SHU_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMDQMDBI_SHU_B0) |
        P_Fld(0xb, SHU_B0_DQ7_R_DMRXDVS_DQM_FLAGSEL_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B0) |
        P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMRXTRACK_DQM_EN_B0) |
        P_Fld(0x1, SHU_B0_DQ7_R_DMRODTEN_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMARPI_CG_FB2DLL_DCM_EN_B0) |
        P_Fld(0x0, SHU_B0_DQ7_R_DMTX_ARPI_CG_DQ_NEW_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMTX_ARPI_CG_DQS_NEW_B0) |
        P_Fld(0x0, SHU_B0_DQ7_R_DMTX_ARPI_CG_DQM_NEW_B0) | P_Fld(0x0, SHU_B0_DQ7_R_LP4Y_SDN_MODE_DQS0) |
        P_Fld(0x1, SHU_B0_DQ7_R_DMRXRANK_DQ_EN_B0) | P_Fld(0x2, SHU_B0_DQ7_R_DMRXRANK_DQ_LAT_B0) |
        P_Fld(0x1, SHU_B0_DQ7_R_DMRXRANK_DQS_EN_B0) | P_Fld(0x1, SHU_B0_DQ7_R_DMRXRANK_DQS_LAT_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ7+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x0, SHU_B1_DQ7_R_DMRANKRXDVS_B1) |
        P_Fld(0x0, SHU_B1_DQ7_R_DMDQMDBI_EYE_SHU_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMDQMDBI_SHU_B1) |
        P_Fld(0xb, SHU_B1_DQ7_R_DMRXDVS_DQM_FLAGSEL_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B1) |
        P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMRXTRACK_DQM_EN_B1) |
        P_Fld(0x1, SHU_B1_DQ7_R_DMRODTEN_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMARPI_CG_FB2DLL_DCM_EN_B1) |
        P_Fld(0x0, SHU_B1_DQ7_R_DMTX_ARPI_CG_DQ_NEW_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMTX_ARPI_CG_DQS_NEW_B1) |
        P_Fld(0x0, SHU_B1_DQ7_R_DMTX_ARPI_CG_DQM_NEW_B1) | P_Fld(0x0, SHU_B1_DQ7_R_LP4Y_SDN_MODE_DQS1) |
        P_Fld(0x1, SHU_B1_DQ7_R_DMRXRANK_DQ_EN_B1) | P_Fld(0x2, SHU_B1_DQ7_R_DMRXRANK_DQ_LAT_B1) |
        P_Fld(0x1, SHU_B1_DQ7_R_DMRXRANK_DQS_EN_B1) | P_Fld(0x1, SHU_B1_DQ7_R_DMRXRANK_DQS_LAT_B1));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ11+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x1, SHU_B0_DQ11_RG_RX_ARDQ_RANK_SEL_SER_EN_B0) |
        P_Fld(0x1, SHU_B0_DQ11_RG_RX_ARDQ_RANK_SEL_LAT_EN_B0) | P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_OFFSETC_LAT_EN_B0) |
        P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_OFFSETC_EN_B0) | P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_OFFSETC_BIAS_EN_B0) |
        P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_FRATE_EN_B0) | P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_CDR_EN_B0) |
        P_Fld(0x1, SHU_B0_DQ11_RG_RX_ARDQ_DVS_EN_B0) | P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_DVS_DLY_B0) |
        P_Fld(0x2, SHU_B0_DQ11_RG_RX_ARDQ_DES_MODE_B0) | P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_BW_SEL_B0));
vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ11+(1*SHU_GRP_DDRPHY_OFFSET), P_Fld(0x1, SHU_B1_DQ11_RG_RX_ARDQ_RANK_SEL_SER_EN_B1) |
        P_Fld(0x1, SHU_B1_DQ11_RG_RX_ARDQ_RANK_SEL_LAT_EN_B1) | P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_OFFSETC_LAT_EN_B1) |
        P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_OFFSETC_EN_B1) | P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_OFFSETC_BIAS_EN_B1) |
        P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_FRATE_EN_B1) | P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_CDR_EN_B1) |
        P_Fld(0x1, SHU_B1_DQ11_RG_RX_ARDQ_DVS_EN_B1) | P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_DVS_DLY_B1) |
        P_Fld(0x2, SHU_B1_DQ11_RG_RX_ARDQ_DES_MODE_B1) | P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_BW_SEL_B1));

}
#endif

void CInit_golden_mini_freq_related_vseq_LP4_4266(DRAMC_CTX_T *p)
{

vIO32WriteFldMulti(DDRPHY_REG_SHU_MISC_DRVING1, P_Fld(0x08, SHU_MISC_DRVING1_DQDRVN2) |
        P_Fld(0x06, SHU_MISC_DRVING1_DQDRVP2) | P_Fld(0x08, SHU_MISC_DRVING1_DQSDRVN1) |
        P_Fld(0x06, SHU_MISC_DRVING1_DQSDRVP1) | P_Fld(0x08, SHU_MISC_DRVING1_DQSDRVN2) |
        P_Fld(0x06, SHU_MISC_DRVING1_DQSDRVP2) | P_Fld(0x1, SHU_MISC_DRVING1_DIS_IMP_ODTN_TRACK) |
        P_Fld(0x1, SHU_MISC_DRVING1_DIS_IMPCAL_HW));

vIO32WriteFldMulti(DDRPHY_REG_SHU_MISC_DRVING2, P_Fld(0x08, SHU_MISC_DRVING2_CMDDRVN1) |
        P_Fld(0x06, SHU_MISC_DRVING2_CMDDRVP1) | P_Fld(0x08, SHU_MISC_DRVING2_CMDDRVN2) |
        P_Fld(0x06, SHU_MISC_DRVING2_CMDDRVP2) | P_Fld(0x08, SHU_MISC_DRVING2_DQDRVN1) |
        P_Fld(0x06, SHU_MISC_DRVING2_DQDRVP1) | P_Fld(0x0, SHU_MISC_DRVING2_DIS_IMPCAL_ODT_EN));

vIO32WriteFldMulti(DDRPHY_REG_SHU_MISC_DRVING3, P_Fld(0x0a, SHU_MISC_DRVING3_DQODTN2) |
        P_Fld(0x0a, SHU_MISC_DRVING3_DQODTP2) | P_Fld(0x0a, SHU_MISC_DRVING3_DQSODTN) |
        P_Fld(0x0a, SHU_MISC_DRVING3_DQSODTP) | P_Fld(0x0a, SHU_MISC_DRVING3_DQSODTN2) |
        P_Fld(0x0a, SHU_MISC_DRVING3_DQSODTP2));

vIO32WriteFldMulti(DDRPHY_REG_SHU_MISC_DRVING4, P_Fld(0x0a, SHU_MISC_DRVING4_CMDODTN1) |
        P_Fld(0x0a, SHU_MISC_DRVING4_CMDODTP1) | P_Fld(0x0a, SHU_MISC_DRVING4_CMDODTN2) |
        P_Fld(0x0a, SHU_MISC_DRVING4_CMDODTP2) | P_Fld(0x0a, SHU_MISC_DRVING4_DQODTN1) |
        P_Fld(0x0a, SHU_MISC_DRVING4_DQODTP1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_MISC_DRVING6, P_Fld(0x0a, SHU_MISC_DRVING6_IMP_TXDLY_CMD) |
        P_Fld(0x00, SHU_MISC_DRVING6_DQCODTN1) | P_Fld(0x00, SHU_MISC_DRVING6_DQCODTP1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_MISC_IMPCAL1, P_Fld(0x7, SHU_MISC_IMPCAL1_IMPCAL_CHKCYCLE) |
        P_Fld(0x00, SHU_MISC_IMPCAL1_IMPDRVP) | P_Fld(0x00, SHU_MISC_IMPCAL1_IMPDRVN) |
        P_Fld(0x4, SHU_MISC_IMPCAL1_IMPCAL_CALEN_CYCLE) | P_Fld(0x03, SHU_MISC_IMPCAL1_IMPCALCNT) |
        P_Fld(0x8, SHU_MISC_IMPCAL1_IMPCAL_CALICNT));

vIO32WriteFldMulti(DDRPHY_REG_SHU_MISC_RDSEL_TRACK, P_Fld(0x10, SHU_MISC_RDSEL_TRACK_DMDATLAT_I) |
        P_Fld(0x1, SHU_MISC_RDSEL_TRACK_RDSEL_HWSAVE_MSK) | P_Fld(0x0, SHU_MISC_RDSEL_TRACK_RDSEL_TRACK_EN) |
        P_Fld(0xfcb, SHU_MISC_RDSEL_TRACK_SHU_GW_THRD_NEG) | P_Fld(0x035, SHU_MISC_RDSEL_TRACK_SHU_GW_THRD_POS));

vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RDAT, P_Fld(0x10, MISC_SHU_RDAT_DATLAT) |
        P_Fld(0x10, MISC_SHU_RDAT_DATLAT_DSEL) | P_Fld(0x10, MISC_SHU_RDAT_DATLAT_DSEL_PHY));

vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_PHY_RX_CTRL, P_Fld(0x1, MISC_SHU_PHY_RX_CTRL_RANK_RXDLY_UPDLAT_EN) |
        P_Fld(0x2, MISC_SHU_PHY_RX_CTRL_RANK_RXDLY_UPD_OFFSET) | P_Fld(0x2, MISC_SHU_PHY_RX_CTRL_RX_IN_GATE_EN_PRE_OFFSET) |
        P_Fld(0x1, MISC_SHU_PHY_RX_CTRL_RX_IN_GATE_EN_HEAD) | P_Fld(0x1, MISC_SHU_PHY_RX_CTRL_RX_IN_GATE_EN_TAIL) |
        P_Fld(0x3, MISC_SHU_PHY_RX_CTRL_RX_IN_BUFF_EN_HEAD) | P_Fld(0x0, MISC_SHU_PHY_RX_CTRL_RX_IN_BUFF_EN_TAIL));

vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RANKCTL, P_Fld(0x5, MISC_SHU_RANKCTL_RANKINCTL_RXDLY) |
        P_Fld(0x1, MISC_SHU_RANKCTL_RANK_RXDLY_OPT) | P_Fld(0x0, MISC_SHU_RANKCTL_RANKSEL_SELPH_FRUN) |
        P_Fld(0x8, MISC_SHU_RANKCTL_RANKINCTL_STB) | P_Fld(0x6, MISC_SHU_RANKCTL_RANKINCTL) |
        P_Fld(0x6, MISC_SHU_RANKCTL_RANKINCTL_ROOT1) | P_Fld(0x9, MISC_SHU_RANKCTL_RANKINCTL_PHY));

vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RANK_SEL_LAT, P_Fld(0x2, MISC_SHU_RANK_SEL_LAT_RANK_SEL_LAT_B0) |
        P_Fld(0x2, MISC_SHU_RANK_SEL_LAT_RANK_SEL_LAT_B1) | P_Fld(0x2, MISC_SHU_RANK_SEL_LAT_RANK_SEL_LAT_CA));

vIO32WriteFldAlign(DDRPHY_REG_MISC_SHU_RK_DQSCTL, 0x8, MISC_SHU_RK_DQSCTL_DQSINCTL);

vIO32WriteFldAlign(DDRPHY_REG_MISC_SHU_RK_DQSCTL+(1*DDRPHY_AO_RANK_OFFSET), 0x8, MISC_SHU_RK_DQSCTL_DQSINCTL);

vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B0_DQSIEN_MCK_UI_DLY, P_Fld(0x1, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B0) |
        P_Fld(0x5, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B0) | P_Fld(0x1, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B0) |
        P_Fld(0x1, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B0));

vIO32WriteFldAlign(DDRPHY_REG_SHU_RK_B0_DQSIEN_PI_DLY, 0x01, SHU_RK_B0_DQSIEN_PI_DLY_DQSIEN_PI_B0);

vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B0_DQSIEN_MCK_UI_DLY+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x9, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B0) |
        P_Fld(0xd, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B0) | P_Fld(0x1, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B0) |
        P_Fld(0x1, SHU_RK_B0_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B0));

vIO32WriteFldAlign(DDRPHY_REG_SHU_RK_B0_DQSIEN_PI_DLY+(1*DDRPHY_AO_RANK_OFFSET), 0x08, SHU_RK_B0_DQSIEN_PI_DLY_DQSIEN_PI_B0);

vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B1_DQSIEN_MCK_UI_DLY, P_Fld(0x1, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B1) |
        P_Fld(0x5, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B1) | P_Fld(0x1, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B1) |
        P_Fld(0x1, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B1));

vIO32WriteFldAlign(DDRPHY_REG_SHU_RK_B1_DQSIEN_PI_DLY, 0x01, SHU_RK_B1_DQSIEN_PI_DLY_DQSIEN_PI_B1);

vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B1_DQSIEN_MCK_UI_DLY+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x9, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P0_B1) |
        P_Fld(0xd, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_UI_P1_B1) | P_Fld(0x1, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P0_B1) |
        P_Fld(0x1, SHU_RK_B1_DQSIEN_MCK_UI_DLY_DQSIEN_MCK_P1_B1));

vIO32WriteFldAlign(DDRPHY_REG_SHU_RK_B1_DQSIEN_PI_DLY+(1*DDRPHY_AO_RANK_OFFSET), 0x08, SHU_RK_B1_DQSIEN_PI_DLY_DQSIEN_PI_B1);

vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_ODTCTRL, P_Fld(0x1, MISC_SHU_ODTCTRL_RODTEN) |
        P_Fld(0x0, MISC_SHU_ODTCTRL_RODTENSTB_SELPH_CG_IG) | P_Fld(0x8, MISC_SHU_ODTCTRL_RODT_LAT) |
        P_Fld(0x0, MISC_SHU_ODTCTRL_RODTEN_SELPH_FRUN) | P_Fld(0x0, MISC_SHU_ODTCTRL_RODTDLY_LAT_OPT) |
        P_Fld(0x0, MISC_SHU_ODTCTRL_FIXRODT) | P_Fld(0x1, MISC_SHU_ODTCTRL_RODTEN_OPT) |
        P_Fld(0x1, MISC_SHU_ODTCTRL_RODTE2) | P_Fld(0x1, MISC_SHU_ODTCTRL_RODTE));

vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ7, P_Fld(0x0, SHU_B0_DQ7_R_DMRANKRXDVS_B0) |
        P_Fld(0x0, SHU_B0_DQ7_R_DMDQMDBI_EYE_SHU_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMDQMDBI_SHU_B0) |
        P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_DQM_FLAGSEL_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B0) |
        P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMRXTRACK_DQM_EN_B0) |
        P_Fld(0x1, SHU_B0_DQ7_R_DMRODTEN_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMARPI_CG_FB2DLL_DCM_EN_B0) |
        P_Fld(0x0, SHU_B0_DQ7_R_DMTX_ARPI_CG_DQ_NEW_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMTX_ARPI_CG_DQS_NEW_B0) |
        P_Fld(0x0, SHU_B0_DQ7_R_DMTX_ARPI_CG_DQM_NEW_B0) | P_Fld(0x0, SHU_B0_DQ7_R_LP4Y_SDN_MODE_DQS0) |
        P_Fld(0x1, SHU_B0_DQ7_R_DMRXRANK_DQ_EN_B0) | P_Fld(0x2, SHU_B0_DQ7_R_DMRXRANK_DQ_LAT_B0) |
        P_Fld(0x1, SHU_B0_DQ7_R_DMRXRANK_DQS_EN_B0) | P_Fld(0x1, SHU_B0_DQ7_R_DMRXRANK_DQS_LAT_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ7, P_Fld(0x0, SHU_B1_DQ7_R_DMRANKRXDVS_B1) |
        P_Fld(0x0, SHU_B1_DQ7_R_DMDQMDBI_EYE_SHU_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMDQMDBI_SHU_B1) |
        P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_DQM_FLAGSEL_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B1) |
        P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMRXTRACK_DQM_EN_B1) |
        P_Fld(0x1, SHU_B1_DQ7_R_DMRODTEN_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMARPI_CG_FB2DLL_DCM_EN_B1) |
        P_Fld(0x0, SHU_B1_DQ7_R_DMTX_ARPI_CG_DQ_NEW_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMTX_ARPI_CG_DQS_NEW_B1) |
        P_Fld(0x0, SHU_B1_DQ7_R_DMTX_ARPI_CG_DQM_NEW_B1) | P_Fld(0x0, SHU_B1_DQ7_R_LP4Y_SDN_MODE_DQS1) |
        P_Fld(0x1, SHU_B1_DQ7_R_DMRXRANK_DQ_EN_B1) | P_Fld(0x2, SHU_B1_DQ7_R_DMRXRANK_DQ_LAT_B1) |
        P_Fld(0x1, SHU_B1_DQ7_R_DMRXRANK_DQS_EN_B1) | P_Fld(0x1, SHU_B1_DQ7_R_DMRXRANK_DQS_LAT_B1));

vIO32WriteFldAlign(DDRPHY_REG_SHU_MISC_RX_PIPE_CTRL, 0x1, SHU_MISC_RX_PIPE_CTRL_RX_PIPE_BYPASS_EN);

vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B0_RODTEN_MCK_UI_DLY, P_Fld(0x4, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B0) |
        P_Fld(0x4, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B0) | P_Fld(0x0, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B0) |
        P_Fld(0x0, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B0_RODTEN_MCK_UI_DLY+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x4, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B0) |
        P_Fld(0x4, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B0) | P_Fld(0x1, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B0) |
        P_Fld(0x0, SHU_RK_B0_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B1_RODTEN_MCK_UI_DLY, P_Fld(0x4, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B1) |
        P_Fld(0x4, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B1) | P_Fld(0x0, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B1) |
        P_Fld(0x0, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_RK_B1_RODTEN_MCK_UI_DLY+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x4, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P0_B1) |
        P_Fld(0x4, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_UI_P1_B1) | P_Fld(0x1, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P0_B1) |
        P_Fld(0x0, SHU_RK_B1_RODTEN_MCK_UI_DLY_RODTEN_MCK_P1_B1));

vIO32WriteFldMulti(DRAMC_REG_SHU_RX_CG_SET0, P_Fld(0x0, SHU_RX_CG_SET0_DLE_LAST_EXTEND3) |
        P_Fld(0x0, SHU_RX_CG_SET0_READ_START_EXTEND3) | P_Fld(0x1, SHU_RX_CG_SET0_DLE_LAST_EXTEND2) |
        P_Fld(0x1, SHU_RX_CG_SET0_READ_START_EXTEND2) | P_Fld(0x1, SHU_RX_CG_SET0_DLE_LAST_EXTEND1) |
        P_Fld(0x1, SHU_RX_CG_SET0_READ_START_EXTEND1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_MISC_RANK_SEL_STB, P_Fld(0x1, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_EN) |
        P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_EN_B23) | P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_SERMODE) |
        P_Fld(0x1, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_TRACK) | P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_RXDLY_TRACK) |
        P_Fld(0x1, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_PHASE_EN) | P_Fld(0x9, SHU_MISC_RANK_SEL_STB_RANK_SEL_PHSINCTL) |
        P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_UI_PLUS) | P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_MCK_PLUS) |
        P_Fld(0x1, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_UI_MINUS) | P_Fld(0x0, SHU_MISC_RANK_SEL_STB_RANK_SEL_STB_MCK_MINUS));

vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RK_DQSCAL, P_Fld(0x60, MISC_SHU_RK_DQSCAL_DQSIENLLMT) |
        P_Fld(0x1, MISC_SHU_RK_DQSCAL_DQSIENLLMTEN) | P_Fld(0x3f, MISC_SHU_RK_DQSCAL_DQSIENHLMT) |
        P_Fld(0x1, MISC_SHU_RK_DQSCAL_DQSIENHLMTEN));

vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RK_DQSCAL+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x60, MISC_SHU_RK_DQSCAL_DQSIENLLMT) |
        P_Fld(0x1, MISC_SHU_RK_DQSCAL_DQSIENLLMTEN) | P_Fld(0x3f, MISC_SHU_RK_DQSCAL_DQSIENHLMT) |
        P_Fld(0x1, MISC_SHU_RK_DQSCAL_DQSIENHLMTEN));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_INI_UIPI, P_Fld(0x01, SHU_R0_B0_INI_UIPI_CURR_INI_PI_B0) |
        P_Fld(0x11, SHU_R0_B0_INI_UIPI_CURR_INI_UI_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_INI_UIPI, P_Fld(0x01, SHU_R0_B1_INI_UIPI_CURR_INI_PI_B1) |
        P_Fld(0x11, SHU_R0_B1_INI_UIPI_CURR_INI_UI_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_INI_UIPI+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x08, SHU_R0_B0_INI_UIPI_CURR_INI_PI_B0) |
        P_Fld(0x19, SHU_R0_B0_INI_UIPI_CURR_INI_UI_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_INI_UIPI+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x08, SHU_R0_B1_INI_UIPI_CURR_INI_PI_B1) |
        P_Fld(0x19, SHU_R0_B1_INI_UIPI_CURR_INI_UI_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_NEXT_INI_UIPI, P_Fld(0x01, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_PI_B0) |
        P_Fld(0x11, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_UI_B0) | P_Fld(0x15, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_UI_P1_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_NEXT_INI_UIPI, P_Fld(0x01, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_PI_B1) |
        P_Fld(0x11, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_UI_B1) | P_Fld(0x15, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_UI_P1_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_NEXT_INI_UIPI+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x08, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_PI_B0) |
        P_Fld(0x19, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_UI_B0) | P_Fld(0x1d, SHU_R0_B0_NEXT_INI_UIPI_NEXT_INI_UI_P1_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_NEXT_INI_UIPI+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x08, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_PI_B1) |
        P_Fld(0x19, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_UI_B1) | P_Fld(0x1d, SHU_R0_B1_NEXT_INI_UIPI_NEXT_INI_UI_P1_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_DQ0, P_Fld(0x0, SHU_R0_B0_DQ0_RG_RX_ARDQS0_R_DLY_DUTY) |
        P_Fld(0x0, SHU_R0_B0_DQ0_RG_RX_ARDQS0_F_DLY_DUTY) | P_Fld(0x15, SHU_R0_B0_DQ0_SW_ARPI_DQ_B0) |
        P_Fld(0x15, SHU_R0_B0_DQ0_SW_ARPI_DQM_B0) | P_Fld(0x00, SHU_R0_B0_DQ0_ARPI_PBYTE_B0) |
        P_Fld(0x0, SHU_R0_B0_DQ0_DA_ARPI_DDR400_0D5UI_RK0_B0) | P_Fld(0x0, SHU_R0_B0_DQ0_DA_RX_ARDQSIEN_0D5UI_RK0_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_DQ0, P_Fld(0x0, SHU_R0_B1_DQ0_RG_RX_ARDQS1_R_DLY_DUTY) |
        P_Fld(0x0, SHU_R0_B1_DQ0_RG_RX_ARDQS1_F_DLY_DUTY) | P_Fld(0x15, SHU_R0_B1_DQ0_SW_ARPI_DQ_B1) |
        P_Fld(0x15, SHU_R0_B1_DQ0_SW_ARPI_DQM_B1) | P_Fld(0x00, SHU_R0_B1_DQ0_ARPI_PBYTE_B1) |
        P_Fld(0x0, SHU_R0_B1_DQ0_DA_ARPI_DDR400_0D5UI_RK0_B1) | P_Fld(0x0, SHU_R0_B1_DQ0_DA_RX_ARDQSIEN_0D5UI_RK0_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_DQ0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x0, SHU_R0_B0_DQ0_RG_RX_ARDQS0_R_DLY_DUTY) |
        P_Fld(0x0, SHU_R0_B0_DQ0_RG_RX_ARDQS0_F_DLY_DUTY) | P_Fld(0x24, SHU_R0_B0_DQ0_SW_ARPI_DQ_B0) |
        P_Fld(0x24, SHU_R0_B0_DQ0_SW_ARPI_DQM_B0) | P_Fld(0x00, SHU_R0_B0_DQ0_ARPI_PBYTE_B0) |
        P_Fld(0x0, SHU_R0_B0_DQ0_DA_ARPI_DDR400_0D5UI_RK0_B0) | P_Fld(0x0, SHU_R0_B0_DQ0_DA_RX_ARDQSIEN_0D5UI_RK0_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_DQ0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x0, SHU_R0_B1_DQ0_RG_RX_ARDQS1_R_DLY_DUTY) |
        P_Fld(0x0, SHU_R0_B1_DQ0_RG_RX_ARDQS1_F_DLY_DUTY) | P_Fld(0x22, SHU_R0_B1_DQ0_SW_ARPI_DQ_B1) |
        P_Fld(0x22, SHU_R0_B1_DQ0_SW_ARPI_DQM_B1) | P_Fld(0x00, SHU_R0_B1_DQ0_ARPI_PBYTE_B1) |
        P_Fld(0x0, SHU_R0_B1_DQ0_DA_ARPI_DDR400_0D5UI_RK0_B1) | P_Fld(0x0, SHU_R0_B1_DQ0_DA_RX_ARDQSIEN_0D5UI_RK0_B1));

#if (fcFOR_CHIP_ID == fcA60868)
vIO32WriteFldMulti(DRAMC_REG_SHU_DCM_CTRL0, P_Fld(0x1, SHU_DCM_CTRL0_DDRPHY_CLK_EN_OPT) |
        P_Fld(0x4, SHU_DCM_CTRL0_DPHY_CMDDCM_EXTCNT) | P_Fld(0x5, SHU_DCM_CTRL0_DDRPHY_CLK_DYN_GATING_SEL) |
        P_Fld(0x0, SHU_DCM_CTRL0_CKE_EXTNONPD_CNT) | P_Fld(0x0, SHU_DCM_CTRL0_FASTWAKE2) |
        P_Fld(0x1, SHU_DCM_CTRL0_FASTWAKE));
#elif (fcFOR_CHIP_ID == fcPetrus)
vIO32WriteFldMulti(DRAMC_REG_SHU_DCM_CTRL0, P_Fld(0x1, SHU_DCM_CTRL0_DDRPHY_CLK_EN_OPT) |
        P_Fld(0x5, SHU_DCM_CTRL0_DDRPHY_CLK_DYN_GATING_SEL) |
        P_Fld(0x0, SHU_DCM_CTRL0_FASTWAKE2) |
        P_Fld(0x1, SHU_DCM_CTRL0_FASTWAKE));
#endif

vIO32WriteFldMulti(DRAMC_REG_SHU_APHY_TX_PICG_CTRL, P_Fld(0x7, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_PICG_CNT) |
        P_Fld(0x3, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQS_SEL_P1) | P_Fld(0x3, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQS_SEL_P0) |
        P_Fld(0x1, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_OPT));

vIO32WriteFldMulti(DRAMC_REG_SHURK_APHY_TX_PICG_CTRL, P_Fld(0x3, SHURK_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQ_RK_SEL_P1) |
        P_Fld(0x3, SHURK_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQ_RK_SEL_P0));

vIO32WriteFldMulti(DRAMC_REG_SHURK_APHY_TX_PICG_CTRL+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x4, SHURK_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQ_RK_SEL_P1) |
        P_Fld(0x3, SHURK_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQ_RK_SEL_P0));

vIO32WriteFldMulti(DRAMC_REG_SHU_NEW_XRW2W_CTRL, P_Fld(0x3, SHU_NEW_XRW2W_CTRL_TX_PI_UPDCTL_B0) |
        P_Fld(0x3, SHU_NEW_XRW2W_CTRL_TX_PI_UPDCTL_B1) | P_Fld(0x0, SHU_NEW_XRW2W_CTRL_TXPI_UPD_MODE));

vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_DQS0, P_Fld(0x4, SHU_SELPH_DQS0_TXDLY_DQS0) |
        P_Fld(0x4, SHU_SELPH_DQS0_TXDLY_DQS1) | P_Fld(0x1, SHU_SELPH_DQS0_TXDLY_DQS2) |
        P_Fld(0x1, SHU_SELPH_DQS0_TXDLY_DQS3) | P_Fld(0x4, SHU_SELPH_DQS0_TXDLY_OEN_DQS0) |
        P_Fld(0x4, SHU_SELPH_DQS0_TXDLY_OEN_DQS1) | P_Fld(0x1, SHU_SELPH_DQS0_TXDLY_OEN_DQS2) |
        P_Fld(0x1, SHU_SELPH_DQS0_TXDLY_OEN_DQS3));

vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_DQS1, P_Fld(0x5, SHU_SELPH_DQS1_DLY_DQS0) |
        P_Fld(0x5, SHU_SELPH_DQS1_DLY_DQS1) | P_Fld(0x1, SHU_SELPH_DQS1_DLY_DQS2) |
        P_Fld(0x1, SHU_SELPH_DQS1_DLY_DQS3) | P_Fld(0x2, SHU_SELPH_DQS1_DLY_OEN_DQS0) |
        P_Fld(0x2, SHU_SELPH_DQS1_DLY_OEN_DQS1) | P_Fld(0x1, SHU_SELPH_DQS1_DLY_OEN_DQS2) |
        P_Fld(0x1, SHU_SELPH_DQS1_DLY_OEN_DQS3));

vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ0, P_Fld(0x3, SHURK_SELPH_DQ0_TXDLY_DQ0) |
        P_Fld(0x3, SHURK_SELPH_DQ0_TXDLY_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_DQ2) |
        P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_DQ3) | P_Fld(0x4, SHURK_SELPH_DQ0_TXDLY_OEN_DQ0) |
        P_Fld(0x4, SHURK_SELPH_DQ0_TXDLY_OEN_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_OEN_DQ2) |
        P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_OEN_DQ3));

vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ1, P_Fld(0x3, SHURK_SELPH_DQ1_TXDLY_DQM0) |
        P_Fld(0x3, SHURK_SELPH_DQ1_TXDLY_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_DQM2) |
        P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_DQM3) | P_Fld(0x4, SHURK_SELPH_DQ1_TXDLY_OEN_DQM0) |
        P_Fld(0x4, SHURK_SELPH_DQ1_TXDLY_OEN_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_OEN_DQM2) |
        P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_OEN_DQM3));

vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ2, P_Fld(0x6, SHURK_SELPH_DQ2_DLY_DQ0) |
        P_Fld(0x6, SHURK_SELPH_DQ2_DLY_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ2_DLY_DQ2) |
        P_Fld(0x1, SHURK_SELPH_DQ2_DLY_DQ3) | P_Fld(0x3, SHURK_SELPH_DQ2_DLY_OEN_DQ0) |
        P_Fld(0x3, SHURK_SELPH_DQ2_DLY_OEN_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ2_DLY_OEN_DQ2) |
        P_Fld(0x1, SHURK_SELPH_DQ2_DLY_OEN_DQ3));

vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ3, P_Fld(0x6, SHURK_SELPH_DQ3_DLY_DQM0) |
        P_Fld(0x6, SHURK_SELPH_DQ3_DLY_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ3_DLY_DQM2) |
        P_Fld(0x1, SHURK_SELPH_DQ3_DLY_DQM3) | P_Fld(0x3, SHURK_SELPH_DQ3_DLY_OEN_DQM0) |
        P_Fld(0x3, SHURK_SELPH_DQ3_DLY_OEN_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ3_DLY_OEN_DQM2) |
        P_Fld(0x1, SHURK_SELPH_DQ3_DLY_OEN_DQM3));

vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ0+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x3, SHURK_SELPH_DQ0_TXDLY_DQ0) |
        P_Fld(0x3, SHURK_SELPH_DQ0_TXDLY_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_DQ2) |
        P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_DQ3) | P_Fld(0x4, SHURK_SELPH_DQ0_TXDLY_OEN_DQ0) |
        P_Fld(0x4, SHURK_SELPH_DQ0_TXDLY_OEN_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_OEN_DQ2) |
        P_Fld(0x1, SHURK_SELPH_DQ0_TXDLY_OEN_DQ3));

vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ1+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x3, SHURK_SELPH_DQ1_TXDLY_DQM0) |
        P_Fld(0x3, SHURK_SELPH_DQ1_TXDLY_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_DQM2) |
        P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_DQM3) | P_Fld(0x4, SHURK_SELPH_DQ1_TXDLY_OEN_DQM0) |
        P_Fld(0x4, SHURK_SELPH_DQ1_TXDLY_OEN_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_OEN_DQM2) |
        P_Fld(0x1, SHURK_SELPH_DQ1_TXDLY_OEN_DQM3));

vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ2+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x7, SHURK_SELPH_DQ2_DLY_DQ0) |
        P_Fld(0x7, SHURK_SELPH_DQ2_DLY_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ2_DLY_DQ2) |
        P_Fld(0x1, SHURK_SELPH_DQ2_DLY_DQ3) | P_Fld(0x4, SHURK_SELPH_DQ2_DLY_OEN_DQ0) |
        P_Fld(0x4, SHURK_SELPH_DQ2_DLY_OEN_DQ1) | P_Fld(0x1, SHURK_SELPH_DQ2_DLY_OEN_DQ2) |
        P_Fld(0x1, SHURK_SELPH_DQ2_DLY_OEN_DQ3));

vIO32WriteFldMulti(DRAMC_REG_SHURK_SELPH_DQ3+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x7, SHURK_SELPH_DQ3_DLY_DQM0) |
        P_Fld(0x7, SHURK_SELPH_DQ3_DLY_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ3_DLY_DQM2) |
        P_Fld(0x1, SHURK_SELPH_DQ3_DLY_DQM3) | P_Fld(0x4, SHURK_SELPH_DQ3_DLY_OEN_DQM0) |
        P_Fld(0x4, SHURK_SELPH_DQ3_DLY_OEN_DQM1) | P_Fld(0x1, SHURK_SELPH_DQ3_DLY_OEN_DQM2) |
        P_Fld(0x1, SHURK_SELPH_DQ3_DLY_OEN_DQM3));

vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL1, P_Fld(0x015, SHURK_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ0) |
        P_Fld(0x015, SHURK_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ1));

vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL2, P_Fld(0x015, SHURK_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ0) |
        P_Fld(0x015, SHURK_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ1));

vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL5, P_Fld(0x015, SHURK_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM0) |
        P_Fld(0x015, SHURK_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM1));

vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL1+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x024, SHURK_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ0) |
        P_Fld(0x022, SHURK_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ1));

vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL2+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x024, SHURK_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ0) |
        P_Fld(0x022, SHURK_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ1));

vIO32WriteFldMulti(DRAMC_REG_SHURK_DQS2DQ_CAL5+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x024, SHURK_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM0) |
        P_Fld(0x022, SHURK_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM1));

vIO32WriteFldMulti(DRAMC_REG_SHURK_PI, P_Fld(0x15, SHURK_PI_RK0_ARPI_DQ_B1) |
        P_Fld(0x15, SHURK_PI_RK0_ARPI_DQ_B0) | P_Fld(0x15, SHURK_PI_RK0_ARPI_DQM_B1) |
        P_Fld(0x15, SHURK_PI_RK0_ARPI_DQM_B0));

vIO32WriteFldMulti(DRAMC_REG_SHURK_PI+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x22, SHURK_PI_RK0_ARPI_DQ_B1) |
        P_Fld(0x24, SHURK_PI_RK0_ARPI_DQ_B0) | P_Fld(0x22, SHURK_PI_RK0_ARPI_DQM_B1) |
        P_Fld(0x24, SHURK_PI_RK0_ARPI_DQM_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_TXDLY0, P_Fld(0x08, SHU_R0_B0_TXDLY0_TX_ARDQ0_DLY_B0) |
        P_Fld(0x08, SHU_R0_B0_TXDLY0_TX_ARDQ1_DLY_B0) | P_Fld(0x08, SHU_R0_B0_TXDLY0_TX_ARDQ2_DLY_B0) |
        P_Fld(0x08, SHU_R0_B0_TXDLY0_TX_ARDQ3_DLY_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_TXDLY1, P_Fld(0x08, SHU_R0_B0_TXDLY1_TX_ARDQ4_DLY_B0) |
        P_Fld(0x08, SHU_R0_B0_TXDLY1_TX_ARDQ5_DLY_B0) | P_Fld(0x08, SHU_R0_B0_TXDLY1_TX_ARDQ6_DLY_B0) |
        P_Fld(0x08, SHU_R0_B0_TXDLY1_TX_ARDQ7_DLY_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_TXDLY3, P_Fld(0x08, SHU_R0_B0_TXDLY3_TX_ARDQM0_DLY_B0) |
        P_Fld(0x00, SHU_R0_B0_TXDLY3_TX_ARWCK_DLY_B0) | P_Fld(0x00, SHU_R0_B0_TXDLY3_TX_ARWCKB_DLY_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_TXDLY0, P_Fld(0x08, SHU_R0_B1_TXDLY0_TX_ARDQ0_DLY_B1) |
        P_Fld(0x08, SHU_R0_B1_TXDLY0_TX_ARDQ1_DLY_B1) | P_Fld(0x08, SHU_R0_B1_TXDLY0_TX_ARDQ2_DLY_B1) |
        P_Fld(0x08, SHU_R0_B1_TXDLY0_TX_ARDQ3_DLY_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_TXDLY1, P_Fld(0x08, SHU_R0_B1_TXDLY1_TX_ARDQ4_DLY_B1) |
        P_Fld(0x08, SHU_R0_B1_TXDLY1_TX_ARDQ5_DLY_B1) | P_Fld(0x08, SHU_R0_B1_TXDLY1_TX_ARDQ6_DLY_B1) |
        P_Fld(0x08, SHU_R0_B1_TXDLY1_TX_ARDQ7_DLY_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_TXDLY3, P_Fld(0x08, SHU_R0_B1_TXDLY3_TX_ARDQM0_DLY_B1) |
        P_Fld(0x00, SHU_R0_B1_TXDLY3_TX_ARWCK_DLY_B1) | P_Fld(0x00, SHU_R0_B1_TXDLY3_TX_ARWCKB_DLY_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_TXDLY0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x20, SHU_R0_B0_TXDLY0_TX_ARDQ0_DLY_B0) |
        P_Fld(0x20, SHU_R0_B0_TXDLY0_TX_ARDQ1_DLY_B0) | P_Fld(0x20, SHU_R0_B0_TXDLY0_TX_ARDQ2_DLY_B0) |
        P_Fld(0x20, SHU_R0_B0_TXDLY0_TX_ARDQ3_DLY_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_TXDLY1+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x20, SHU_R0_B0_TXDLY1_TX_ARDQ4_DLY_B0) |
        P_Fld(0x20, SHU_R0_B0_TXDLY1_TX_ARDQ5_DLY_B0) | P_Fld(0x20, SHU_R0_B0_TXDLY1_TX_ARDQ6_DLY_B0) |
        P_Fld(0x20, SHU_R0_B0_TXDLY1_TX_ARDQ7_DLY_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_TXDLY3+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x20, SHU_R0_B0_TXDLY3_TX_ARDQM0_DLY_B0) |
        P_Fld(0x00, SHU_R0_B0_TXDLY3_TX_ARWCK_DLY_B0) | P_Fld(0x00, SHU_R0_B0_TXDLY3_TX_ARWCKB_DLY_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_TXDLY0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x28, SHU_R0_B1_TXDLY0_TX_ARDQ0_DLY_B1) |
        P_Fld(0x28, SHU_R0_B1_TXDLY0_TX_ARDQ1_DLY_B1) | P_Fld(0x28, SHU_R0_B1_TXDLY0_TX_ARDQ2_DLY_B1) |
        P_Fld(0x28, SHU_R0_B1_TXDLY0_TX_ARDQ3_DLY_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_TXDLY1+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x28, SHU_R0_B1_TXDLY1_TX_ARDQ4_DLY_B1) |
        P_Fld(0x28, SHU_R0_B1_TXDLY1_TX_ARDQ5_DLY_B1) | P_Fld(0x28, SHU_R0_B1_TXDLY1_TX_ARDQ6_DLY_B1) |
        P_Fld(0x28, SHU_R0_B1_TXDLY1_TX_ARDQ7_DLY_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_TXDLY3+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x28, SHU_R0_B1_TXDLY3_TX_ARDQM0_DLY_B1) |
        P_Fld(0x00, SHU_R0_B1_TXDLY3_TX_ARWCK_DLY_B1) | P_Fld(0x00, SHU_R0_B1_TXDLY3_TX_ARWCKB_DLY_B1));

vIO32WriteFldMulti(DRAMC_REG_SHU_TX_RANKCTL, P_Fld(0x2, SHU_TX_RANKCTL_TXRANKINCTL_TXDLY) |
        P_Fld(0x2, SHU_TX_RANKCTL_TXRANKINCTL) | P_Fld(0x0, SHU_TX_RANKCTL_TXRANKINCTL_ROOT));

vIO32WriteFldAlign(DDRPHY_REG_SHU_B0_DQ9, 0x31105ab1, SHU_B0_DQ9_RG_ARPI_RESERVE_B0);

vIO32WriteFldAlign(DDRPHY_REG_SHU_B1_DQ9, 0xd5713d50, SHU_B1_DQ9_RG_ARPI_RESERVE_B1);

vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_CA1, P_Fld(0x0, SHU_SELPH_CA1_TXDLY_CS) |
        P_Fld(0x0, SHU_SELPH_CA1_TXDLY_CKE) | P_Fld(0x0, SHU_SELPH_CA1_TXDLY_ODT) |
        P_Fld(0x0, SHU_SELPH_CA1_TXDLY_RESET) | P_Fld(0x0, SHU_SELPH_CA1_TXDLY_WE) |
        P_Fld(0x0, SHU_SELPH_CA1_TXDLY_CAS) | P_Fld(0x0, SHU_SELPH_CA1_TXDLY_RAS) |
        P_Fld(0x0, SHU_SELPH_CA1_TXDLY_CS1));

vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_CA2, P_Fld(0x0, SHU_SELPH_CA2_TXDLY_BA0) |
        P_Fld(0x0, SHU_SELPH_CA2_TXDLY_BA1) | P_Fld(0x0, SHU_SELPH_CA2_TXDLY_BA2) |
        P_Fld(0x01, SHU_SELPH_CA2_TXDLY_CMD) | P_Fld(0x0, SHU_SELPH_CA2_TXDLY_CKE1));

vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_CA3, P_Fld(0x0, SHU_SELPH_CA3_TXDLY_RA0) |
        P_Fld(0x0, SHU_SELPH_CA3_TXDLY_RA1) | P_Fld(0x0, SHU_SELPH_CA3_TXDLY_RA2) |
        P_Fld(0x0, SHU_SELPH_CA3_TXDLY_RA3) | P_Fld(0x0, SHU_SELPH_CA3_TXDLY_RA4) |
        P_Fld(0x0, SHU_SELPH_CA3_TXDLY_RA5) | P_Fld(0x0, SHU_SELPH_CA3_TXDLY_RA6) |
        P_Fld(0x0, SHU_SELPH_CA3_TXDLY_RA7));

vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_CA4, P_Fld(0x0, SHU_SELPH_CA4_TXDLY_RA8) |
        P_Fld(0x0, SHU_SELPH_CA4_TXDLY_RA9) | P_Fld(0x0, SHU_SELPH_CA4_TXDLY_RA10) |
        P_Fld(0x0, SHU_SELPH_CA4_TXDLY_RA11) | P_Fld(0x0, SHU_SELPH_CA4_TXDLY_RA12) |
        P_Fld(0x0, SHU_SELPH_CA4_TXDLY_RA13) | P_Fld(0x0, SHU_SELPH_CA4_TXDLY_RA14) |
        P_Fld(0x0, SHU_SELPH_CA4_TXDLY_RA15));

vIO32WriteFldMulti(DRAMC_REG_SHU_SELPH_CA5, P_Fld(0x1, SHU_SELPH_CA5_DLY_CS) |
        P_Fld(0x1, SHU_SELPH_CA5_DLY_CKE) | P_Fld(0x0, SHU_SELPH_CA5_DLY_ODT) |
        P_Fld(0x1, SHU_SELPH_CA5_DLY_RESET) | P_Fld(0x1, SHU_SELPH_CA5_DLY_WE) |
        P_Fld(0x1, SHU_SELPH_CA5_DLY_CAS) | P_Fld(0x1, SHU_SELPH_CA5_DLY_RAS) |
        P_Fld(0x1, SHU_SELPH_CA5_DLY_CS1));

vIO32WriteFldMulti(DRAMC_REG_SHU_SREF_CTRL, P_Fld(0x3, SHU_SREF_CTRL_CKEHCMD) |
        P_Fld(0x3, SHU_SREF_CTRL_SREF_CK_DLY));

vIO32WriteFldMulti(DRAMC_REG_SHU_HMR4_DVFS_CTRL0, P_Fld(0x86, SHU_HMR4_DVFS_CTRL0_FSPCHG_PRDCNT) |
        P_Fld(0x000, SHU_HMR4_DVFS_CTRL0_REFRCNT));

vIO32WriteFldMulti(DRAMC_REG_SHU_AC_TIME_05T, P_Fld(0x0, SHU_AC_TIME_05T_TRC_05T) |
        P_Fld(0x0, SHU_AC_TIME_05T_TRFCPB_05T) | P_Fld(0x1, SHU_AC_TIME_05T_TRFC_05T) |
        P_Fld(0x0, SHU_AC_TIME_05T_TPBR2PBR_05T) | P_Fld(0x1, SHU_AC_TIME_05T_TXP_05T) |
        P_Fld(0x1, SHU_AC_TIME_05T_TRTP_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TRCD_05T) |
        P_Fld(0x1, SHU_AC_TIME_05T_TRP_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TRPAB_05T) |
        P_Fld(0x0, SHU_AC_TIME_05T_TRAS_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TWR_M05T) |
        P_Fld(0x1, SHU_AC_TIME_05T_TRRD_05T) | P_Fld(0x1, SHU_AC_TIME_05T_TFAW_05T) |
        P_Fld(0x0, SHU_AC_TIME_05T_TCKEPRD_05T) | P_Fld(0x1, SHU_AC_TIME_05T_TR2PD_05T) |
        P_Fld(0x0, SHU_AC_TIME_05T_TWTPD_M05T) | P_Fld(0x0, SHU_AC_TIME_05T_TMRRI_05T) |
        P_Fld(0x1, SHU_AC_TIME_05T_TMRWCKEL_05T) | P_Fld(0x0, SHU_AC_TIME_05T_BGTRRD_05T) |
        P_Fld(0x0, SHU_AC_TIME_05T_BGTCCD_05T) | P_Fld(0x1, SHU_AC_TIME_05T_BGTWTR_M05T) |
        P_Fld(0x0, SHU_AC_TIME_05T_TR2W_05T) | P_Fld(0x1, SHU_AC_TIME_05T_TWTR_M05T) |
        P_Fld(0x0, SHU_AC_TIME_05T_XRTR2W_05T) | P_Fld(0x1, SHU_AC_TIME_05T_TMRD_05T) |
        P_Fld(0x1, SHU_AC_TIME_05T_TMRW_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TMRR2MRW_05T) |
        P_Fld(0x0, SHU_AC_TIME_05T_TW2MRW_05T) | P_Fld(0x0, SHU_AC_TIME_05T_TR2MRW_05T) |
        P_Fld(0x0, SHU_AC_TIME_05T_TPBR2ACT_05T) | P_Fld(0x0, SHU_AC_TIME_05T_XRTW2R_M05T));

vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM_XRT, P_Fld(0x03, SHU_ACTIM_XRT_XRTR2R) |
        P_Fld(0x08, SHU_ACTIM_XRT_XRTR2W) | P_Fld(0x1, SHU_ACTIM_XRT_XRTW2R) |
        P_Fld(0x05, SHU_ACTIM_XRT_XRTW2W));

#if (fcFOR_CHIP_ID == fcA60868)
vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM0, P_Fld(0xa, SHU_ACTIM0_TWTR) |
        P_Fld(0x3, SHU_ACTIM0_CKELCKCNT) | P_Fld(0x10, SHU_ACTIM0_TWR) |
        P_Fld(0x4, SHU_ACTIM0_TRRD) | P_Fld(0xa, SHU_ACTIM0_TRCD) |
        P_Fld(0xc, SHU_ACTIM0_TWTR_L));
#elif (fcFOR_CHIP_ID == fcPetrus)
vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM0, P_Fld(0xa, SHU_ACTIM0_TWTR) |
        P_Fld(0x3, SHU_ACTIM0_CKELCKCNT) | P_Fld(0x10, SHU_ACTIM0_TWR) |
        P_Fld(0x4, SHU_ACTIM0_TRRD) | P_Fld(0xa, SHU_ACTIM0_TRCD));
#endif

vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM1, P_Fld(0xa, SHU_ACTIM1_TRPAB) |
        P_Fld(0x8, SHU_ACTIM1_TMRWCKEL) | P_Fld(0x8, SHU_ACTIM1_TRP) |
        P_Fld(0x0e, SHU_ACTIM1_TRAS) | P_Fld(0x19, SHU_ACTIM1_TRC));

vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM2, P_Fld(0x2, SHU_ACTIM2_TXP) |
        P_Fld(0x0e, SHU_ACTIM2_TMRRI) | P_Fld(0x3, SHU_ACTIM2_TRTP) |
        P_Fld(0x09, SHU_ACTIM2_TR2W) | P_Fld(0x0d, SHU_ACTIM2_TFAW));

vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM3, P_Fld(0x3f, SHU_ACTIM3_TRFCPB) |
        P_Fld(0x4, SHU_ACTIM3_MANTMRR) | P_Fld(0x4, SHU_ACTIM3_TR2MRR) |
        P_Fld(0x89, SHU_ACTIM3_TRFC));

vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM4, P_Fld(0x09a, SHU_ACTIM4_TXREFCNT) |
        P_Fld(0x0f, SHU_ACTIM4_TMRR2MRW) | P_Fld(0x0b, SHU_ACTIM4_TMRR2W) |
        P_Fld(0x2e, SHU_ACTIM4_TZQCS));

vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM5, P_Fld(0x0f, SHU_ACTIM5_TR2PD) |
        P_Fld(0x12, SHU_ACTIM5_TWTPD) | P_Fld(0x30, SHU_ACTIM5_TPBR2PBR) |
        P_Fld(0x0, SHU_ACTIM5_TPBR2ACT));

vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIM6, P_Fld(0x10, SHU_ACTIM6_TZQLAT2) |
        P_Fld(0x7, SHU_ACTIM6_TMRD) | P_Fld(0x5, SHU_ACTIM6_TMRW) |
        P_Fld(0x0b, SHU_ACTIM6_TW2MRW) | P_Fld(0x12, SHU_ACTIM6_TR2MRW));

vIO32WriteFldMulti(DRAMC_REG_SHU_CKECTRL, P_Fld(0x1, SHU_CKECTRL_TPDE_05T) |
        P_Fld(0x0, SHU_CKECTRL_TPDX_05T) | P_Fld(0x1, SHU_CKECTRL_TPDE) |
        P_Fld(0x1, SHU_CKECTRL_TPDX) | P_Fld(0x4, SHU_CKECTRL_TCKEPRD) |
        P_Fld(0x3, SHU_CKECTRL_TCKESRX));

vIO32WriteFldMulti(DRAMC_REG_SHU_MISC, P_Fld(0x2, SHU_MISC_REQQUE_MAXCNT) |
        P_Fld(0x7, SHU_MISC_DCMDLYREF) | P_Fld(0x0, SHU_MISC_DAREFEN));

vIO32WriteFldMulti(DDRPHY_REG_SHU_MISC_TX_PIPE_CTRL, P_Fld(0x1, SHU_MISC_TX_PIPE_CTRL_CMD_TXPIPE_BYPASS_EN) |
        P_Fld(0x1, SHU_MISC_TX_PIPE_CTRL_CK_TXPIPE_BYPASS_EN) | P_Fld(0x0, SHU_MISC_TX_PIPE_CTRL_TX_PIPE_BYPASS_EN) |
        P_Fld(0x1, SHU_MISC_TX_PIPE_CTRL_CS_TXPIPE_BYPASS_EN) | P_Fld(0x0, SHU_MISC_TX_PIPE_CTRL_SKIP_TXPIPE_BYPASS));

vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ8, P_Fld(0x0100, SHU_B0_DQ8_R_DMRXDVS_UPD_FORCE_CYC_B0) |
        P_Fld(0x0, SHU_B0_DQ8_R_DMRXDVS_UPD_FORCE_EN_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B0) |
        P_Fld(0x0, SHU_B0_DQ8_R_RMRODTEN_CG_IG_B0) | P_Fld(0x1, SHU_B0_DQ8_R_RMRX_TOPHY_CG_IG_B0) |
        P_Fld(0x0, SHU_B0_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B0) |
        P_Fld(0x1, SHU_B0_DQ8_R_DMRXDLY_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B0) |
        P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B0) |
        P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMRANK_PIPE_CG_IG_B0) |
        P_Fld(0x0, SHU_B0_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ8, P_Fld(0x0100, SHU_B1_DQ8_R_DMRXDVS_UPD_FORCE_CYC_B1) |
        P_Fld(0x0, SHU_B1_DQ8_R_DMRXDVS_UPD_FORCE_EN_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B1) |
        P_Fld(0x0, SHU_B1_DQ8_R_RMRODTEN_CG_IG_B1) | P_Fld(0x1, SHU_B1_DQ8_R_RMRX_TOPHY_CG_IG_B1) |
        P_Fld(0x0, SHU_B1_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B1) |
        P_Fld(0x1, SHU_B1_DQ8_R_DMRXDLY_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B1) |
        P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B1) |
        P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMRANK_PIPE_CG_IG_B1) |
        P_Fld(0x0, SHU_B1_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ5, P_Fld(0x0e, SHU_B0_DQ5_RG_RX_ARDQ_VREF_SEL_B0) |
        P_Fld(0x0, SHU_B0_DQ5_RG_RX_ARDQ_VREF_BYPASS_B0) | P_Fld(0x00, SHU_B0_DQ5_RG_ARPI_FB_B0) |
        P_Fld(0x0, SHU_B0_DQ5_RG_RX_ARDQS0_DQSIEN_DLY_B0) | P_Fld(0x0, SHU_B0_DQ5_RG_RX_ARDQS_DQSIEN_RB_DLY_B0) |
        P_Fld(0x3, SHU_B0_DQ5_RG_RX_ARDQS0_DVS_DLY_B0) | P_Fld(0x0, SHU_B0_DQ5_RG_RX_ARDQ_FIFO_DQSI_DLY_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ5, P_Fld(0x0e, SHU_B1_DQ5_RG_RX_ARDQ_VREF_SEL_B1) |
        P_Fld(0x0, SHU_B1_DQ5_RG_RX_ARDQ_VREF_BYPASS_B1) | P_Fld(0x00, SHU_B1_DQ5_RG_ARPI_FB_B1) |
        P_Fld(0x0, SHU_B1_DQ5_RG_RX_ARDQS0_DQSIEN_DLY_B1) | P_Fld(0x0, SHU_B1_DQ5_RG_RX_ARDQS_DQSIEN_RB_DLY_B1) |
        P_Fld(0x3, SHU_B1_DQ5_RG_RX_ARDQS0_DVS_DLY_B1) | P_Fld(0x0, SHU_B1_DQ5_RG_RX_ARDQ_FIFO_DQSI_DLY_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY0, P_Fld(0x6f, SHU_R0_B0_RXDLY0_RX_ARDQ0_R_DLY_B0) |
        P_Fld(0x6f, SHU_R0_B0_RXDLY0_RX_ARDQ0_F_DLY_B0) | P_Fld(0x6f, SHU_R0_B0_RXDLY0_RX_ARDQ1_R_DLY_B0) |
        P_Fld(0x6f, SHU_R0_B0_RXDLY0_RX_ARDQ1_F_DLY_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY1, P_Fld(0x6f, SHU_R0_B0_RXDLY1_RX_ARDQ2_R_DLY_B0) |
        P_Fld(0x6f, SHU_R0_B0_RXDLY1_RX_ARDQ2_F_DLY_B0) | P_Fld(0x6f, SHU_R0_B0_RXDLY1_RX_ARDQ3_R_DLY_B0) |
        P_Fld(0x6f, SHU_R0_B0_RXDLY1_RX_ARDQ3_F_DLY_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY2, P_Fld(0x6f, SHU_R0_B0_RXDLY2_RX_ARDQ4_R_DLY_B0) |
        P_Fld(0x6f, SHU_R0_B0_RXDLY2_RX_ARDQ4_F_DLY_B0) | P_Fld(0x6f, SHU_R0_B0_RXDLY2_RX_ARDQ5_R_DLY_B0) |
        P_Fld(0x6f, SHU_R0_B0_RXDLY2_RX_ARDQ5_F_DLY_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY3, P_Fld(0x6f, SHU_R0_B0_RXDLY3_RX_ARDQ6_R_DLY_B0) |
        P_Fld(0x6f, SHU_R0_B0_RXDLY3_RX_ARDQ6_F_DLY_B0) | P_Fld(0x6f, SHU_R0_B0_RXDLY3_RX_ARDQ7_R_DLY_B0) |
        P_Fld(0x6f, SHU_R0_B0_RXDLY3_RX_ARDQ7_F_DLY_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY4, P_Fld(0x6f, SHU_R0_B0_RXDLY4_RX_ARDQM0_R_DLY_B0) |
        P_Fld(0x6f, SHU_R0_B0_RXDLY4_RX_ARDQM0_F_DLY_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY5, P_Fld(0x02f, SHU_R0_B0_RXDLY5_RX_ARDQS0_R_DLY_B0) |
        P_Fld(0x02f, SHU_R0_B0_RXDLY5_RX_ARDQS0_F_DLY_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x6e, SHU_R0_B0_RXDLY0_RX_ARDQ0_R_DLY_B0) |
        P_Fld(0x6e, SHU_R0_B0_RXDLY0_RX_ARDQ0_F_DLY_B0) | P_Fld(0x6e, SHU_R0_B0_RXDLY0_RX_ARDQ1_R_DLY_B0) |
        P_Fld(0x6e, SHU_R0_B0_RXDLY0_RX_ARDQ1_F_DLY_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY1+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x6e, SHU_R0_B0_RXDLY1_RX_ARDQ2_R_DLY_B0) |
        P_Fld(0x6e, SHU_R0_B0_RXDLY1_RX_ARDQ2_F_DLY_B0) | P_Fld(0x6e, SHU_R0_B0_RXDLY1_RX_ARDQ3_R_DLY_B0) |
        P_Fld(0x6e, SHU_R0_B0_RXDLY1_RX_ARDQ3_F_DLY_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY2+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x6e, SHU_R0_B0_RXDLY2_RX_ARDQ4_R_DLY_B0) |
        P_Fld(0x6e, SHU_R0_B0_RXDLY2_RX_ARDQ4_F_DLY_B0) | P_Fld(0x6e, SHU_R0_B0_RXDLY2_RX_ARDQ5_R_DLY_B0) |
        P_Fld(0x6e, SHU_R0_B0_RXDLY2_RX_ARDQ5_F_DLY_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY3+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x6e, SHU_R0_B0_RXDLY3_RX_ARDQ6_R_DLY_B0) |
        P_Fld(0x6e, SHU_R0_B0_RXDLY3_RX_ARDQ6_F_DLY_B0) | P_Fld(0x6e, SHU_R0_B0_RXDLY3_RX_ARDQ7_R_DLY_B0) |
        P_Fld(0x6e, SHU_R0_B0_RXDLY3_RX_ARDQ7_F_DLY_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY4+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x6e, SHU_R0_B0_RXDLY4_RX_ARDQM0_R_DLY_B0) |
        P_Fld(0x6e, SHU_R0_B0_RXDLY4_RX_ARDQM0_F_DLY_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B0_RXDLY5+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x02e, SHU_R0_B0_RXDLY5_RX_ARDQS0_R_DLY_B0) |
        P_Fld(0x02e, SHU_R0_B0_RXDLY5_RX_ARDQS0_F_DLY_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY0, P_Fld(0x6f, SHU_R0_B1_RXDLY0_RX_ARDQ0_R_DLY_B1) |
        P_Fld(0x6f, SHU_R0_B1_RXDLY0_RX_ARDQ0_F_DLY_B1) | P_Fld(0x6f, SHU_R0_B1_RXDLY0_RX_ARDQ1_R_DLY_B1) |
        P_Fld(0x6f, SHU_R0_B1_RXDLY0_RX_ARDQ1_F_DLY_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY1, P_Fld(0x6f, SHU_R0_B1_RXDLY1_RX_ARDQ2_R_DLY_B1) |
        P_Fld(0x6f, SHU_R0_B1_RXDLY1_RX_ARDQ2_F_DLY_B1) | P_Fld(0x6f, SHU_R0_B1_RXDLY1_RX_ARDQ3_R_DLY_B1) |
        P_Fld(0x6f, SHU_R0_B1_RXDLY1_RX_ARDQ3_F_DLY_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY2, P_Fld(0x6f, SHU_R0_B1_RXDLY2_RX_ARDQ4_R_DLY_B1) |
        P_Fld(0x6f, SHU_R0_B1_RXDLY2_RX_ARDQ4_F_DLY_B1) | P_Fld(0x6f, SHU_R0_B1_RXDLY2_RX_ARDQ5_R_DLY_B1) |
        P_Fld(0x6f, SHU_R0_B1_RXDLY2_RX_ARDQ5_F_DLY_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY3, P_Fld(0x6f, SHU_R0_B1_RXDLY3_RX_ARDQ6_R_DLY_B1) |
        P_Fld(0x6f, SHU_R0_B1_RXDLY3_RX_ARDQ6_F_DLY_B1) | P_Fld(0x6f, SHU_R0_B1_RXDLY3_RX_ARDQ7_R_DLY_B1) |
        P_Fld(0x6f, SHU_R0_B1_RXDLY3_RX_ARDQ7_F_DLY_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY4, P_Fld(0x6f, SHU_R0_B1_RXDLY4_RX_ARDQM0_R_DLY_B1) |
        P_Fld(0x6f, SHU_R0_B1_RXDLY4_RX_ARDQM0_F_DLY_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY5, P_Fld(0x02f, SHU_R0_B1_RXDLY5_RX_ARDQS0_R_DLY_B1) |
        P_Fld(0x02f, SHU_R0_B1_RXDLY5_RX_ARDQS0_F_DLY_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY0+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x6e, SHU_R0_B1_RXDLY0_RX_ARDQ0_R_DLY_B1) |
        P_Fld(0x6e, SHU_R0_B1_RXDLY0_RX_ARDQ0_F_DLY_B1) | P_Fld(0x6e, SHU_R0_B1_RXDLY0_RX_ARDQ1_R_DLY_B1) |
        P_Fld(0x6e, SHU_R0_B1_RXDLY0_RX_ARDQ1_F_DLY_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY1+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x6e, SHU_R0_B1_RXDLY1_RX_ARDQ2_R_DLY_B1) |
        P_Fld(0x6e, SHU_R0_B1_RXDLY1_RX_ARDQ2_F_DLY_B1) | P_Fld(0x6e, SHU_R0_B1_RXDLY1_RX_ARDQ3_R_DLY_B1) |
        P_Fld(0x6e, SHU_R0_B1_RXDLY1_RX_ARDQ3_F_DLY_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY2+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x6e, SHU_R0_B1_RXDLY2_RX_ARDQ4_R_DLY_B1) |
        P_Fld(0x6e, SHU_R0_B1_RXDLY2_RX_ARDQ4_F_DLY_B1) | P_Fld(0x6e, SHU_R0_B1_RXDLY2_RX_ARDQ5_R_DLY_B1) |
        P_Fld(0x6e, SHU_R0_B1_RXDLY2_RX_ARDQ5_F_DLY_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY3+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x6e, SHU_R0_B1_RXDLY3_RX_ARDQ6_R_DLY_B1) |
        P_Fld(0x6e, SHU_R0_B1_RXDLY3_RX_ARDQ6_F_DLY_B1) | P_Fld(0x6e, SHU_R0_B1_RXDLY3_RX_ARDQ7_R_DLY_B1) |
        P_Fld(0x6e, SHU_R0_B1_RXDLY3_RX_ARDQ7_F_DLY_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY4+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x6e, SHU_R0_B1_RXDLY4_RX_ARDQM0_R_DLY_B1) |
        P_Fld(0x6e, SHU_R0_B1_RXDLY4_RX_ARDQM0_F_DLY_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_R0_B1_RXDLY5+(1*DDRPHY_AO_RANK_OFFSET), P_Fld(0x02e, SHU_R0_B1_RXDLY5_RX_ARDQS0_R_DLY_B1) |
        P_Fld(0x02e, SHU_R0_B1_RXDLY5_RX_ARDQS0_F_DLY_B1));

vIO32WriteFldMulti(DDRPHY_REG_B0_DQ9, P_Fld(0x0, B0_DQ9_RG_RX_ARDQ_STBEN_RESETB_B0) |
        P_Fld(0x1, B0_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B0) | P_Fld(0x0, B0_DQ9_RG_RX_ARDQS0_DQSIENMODE_B0) |
        P_Fld(0x1, B0_DQ9_R_DMRXDVS_R_F_DLY_RK_OPT_B0) | P_Fld(0x0, B0_DQ9_R_DMRXFIFO_STBENCMP_EN_B0) |
        P_Fld(0x00, B0_DQ9_R_IN_GATE_EN_LOW_OPT_B0) | P_Fld(0x0, B0_DQ9_R_DMDQSIEN_VALID_LAT_B0) |
        P_Fld(0x0, B0_DQ9_R_DMDQSIEN_RDSEL_LAT_B0) | P_Fld(0x0, B0_DQ9_R_DMRXDVS_VALID_LAT_B0) |
        P_Fld(0x0, B0_DQ9_R_DMRXDVS_RDSEL_LAT_B0));

vIO32WriteFldMulti(DDRPHY_REG_B1_DQ9, P_Fld(0x0, B1_DQ9_RG_RX_ARDQ_STBEN_RESETB_B1) |
        P_Fld(0x1, B1_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B1) | P_Fld(0x0, B1_DQ9_RG_RX_ARDQS0_DQSIENMODE_B1) |
        P_Fld(0x1, B1_DQ9_R_DMRXDVS_R_F_DLY_RK_OPT_B1) | P_Fld(0x0, B1_DQ9_R_DMRXFIFO_STBENCMP_EN_B1) |
        P_Fld(0x00, B1_DQ9_R_IN_GATE_EN_LOW_OPT_B1) | P_Fld(0x0, B1_DQ9_R_DMDQSIEN_VALID_LAT_B1) |
        P_Fld(0x0, B1_DQ9_R_DMDQSIEN_RDSEL_LAT_B1) | P_Fld(0x0, B1_DQ9_R_DMRXDVS_VALID_LAT_B1) |
        P_Fld(0x0, B1_DQ9_R_DMRXDVS_RDSEL_LAT_B1));

vIO32WriteFldMulti(DDRPHY_REG_B0_DQ9, P_Fld(0x1, B0_DQ9_RG_RX_ARDQ_STBEN_RESETB_B0) |
        P_Fld(0x1, B0_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B0) | P_Fld(0x0, B0_DQ9_RG_RX_ARDQS0_DQSIENMODE_B0) |
        P_Fld(0x1, B0_DQ9_R_DMRXDVS_R_F_DLY_RK_OPT_B0) | P_Fld(0x0, B0_DQ9_R_DMRXFIFO_STBENCMP_EN_B0) |
        P_Fld(0x00, B0_DQ9_R_IN_GATE_EN_LOW_OPT_B0) | P_Fld(0x0, B0_DQ9_R_DMDQSIEN_VALID_LAT_B0) |
        P_Fld(0x0, B0_DQ9_R_DMDQSIEN_RDSEL_LAT_B0) | P_Fld(0x0, B0_DQ9_R_DMRXDVS_VALID_LAT_B0) |
        P_Fld(0x0, B0_DQ9_R_DMRXDVS_RDSEL_LAT_B0));

vIO32WriteFldMulti(DDRPHY_REG_B1_DQ9, P_Fld(0x1, B1_DQ9_RG_RX_ARDQ_STBEN_RESETB_B1) |
        P_Fld(0x1, B1_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B1) | P_Fld(0x0, B1_DQ9_RG_RX_ARDQS0_DQSIENMODE_B1) |
        P_Fld(0x1, B1_DQ9_R_DMRXDVS_R_F_DLY_RK_OPT_B1) | P_Fld(0x0, B1_DQ9_R_DMRXFIFO_STBENCMP_EN_B1) |
        P_Fld(0x00, B1_DQ9_R_IN_GATE_EN_LOW_OPT_B1) | P_Fld(0x0, B1_DQ9_R_DMDQSIEN_VALID_LAT_B1) |
        P_Fld(0x0, B1_DQ9_R_DMDQSIEN_RDSEL_LAT_B1) | P_Fld(0x0, B1_DQ9_R_DMRXDVS_VALID_LAT_B1) |
        P_Fld(0x0, B1_DQ9_R_DMRXDVS_RDSEL_LAT_B1));

vIO32WriteFldMulti(DDRPHY_REG_B0_DQ4, P_Fld(0x75, B0_DQ4_RG_RX_ARDQS_EYE_R_DLY_B0) |
        P_Fld(0x75, B0_DQ4_RG_RX_ARDQS_EYE_F_DLY_B0) | P_Fld(0x2f, B0_DQ4_RG_RX_ARDQ_EYE_R_DLY_B0) |
        P_Fld(0x2f, B0_DQ4_RG_RX_ARDQ_EYE_F_DLY_B0));

vIO32WriteFldMulti(DDRPHY_REG_B1_DQ4, P_Fld(0x75, B1_DQ4_RG_RX_ARDQS_EYE_R_DLY_B1) |
        P_Fld(0x75, B1_DQ4_RG_RX_ARDQS_EYE_F_DLY_B1) | P_Fld(0x2f, B1_DQ4_RG_RX_ARDQ_EYE_R_DLY_B1) |
        P_Fld(0x2f, B1_DQ4_RG_RX_ARDQ_EYE_F_DLY_B1));

vIO32WriteFldMulti(DDRPHY_REG_B0_DQ5, P_Fld(0x10, B0_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B0) |
        P_Fld(0x1, B0_DQ5_RG_RX_ARDQ_VREF_EN_B0) | P_Fld(0x1, B0_DQ5_RG_RX_ARDQ_EYE_VREF_EN_B0) |
        P_Fld(0x0, B0_DQ5_RG_RX_ARDQ_EYE_SEL_B0) | P_Fld(0x1, B0_DQ5_RG_RX_ARDQ_EYE_EN_B0) |
        P_Fld(0x1, B0_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B0) | P_Fld(0x0, B0_DQ5_RG_RX_ARDQS0_DVS_EN_B0));

vIO32WriteFldMulti(DDRPHY_REG_B1_DQ5, P_Fld(0x10, B1_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B1) |
        P_Fld(0x1, B1_DQ5_RG_RX_ARDQ_VREF_EN_B1) | P_Fld(0x1, B1_DQ5_RG_RX_ARDQ_EYE_VREF_EN_B1) |
        P_Fld(0x0, B1_DQ5_RG_RX_ARDQ_EYE_SEL_B1) | P_Fld(0x1, B1_DQ5_RG_RX_ARDQ_EYE_EN_B1) |
        P_Fld(0x1, B1_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B1) | P_Fld(0x0, B1_DQ5_RG_RX_ARDQS0_DVS_EN_B1));

vIO32WriteFldMulti(DRAMC_REG_SHU_COMMON0, P_Fld(0x1, SHU_COMMON0_FREQDIV4) |
        P_Fld(0x0, SHU_COMMON0_FDIV2) | P_Fld(0x0, SHU_COMMON0_FREQDIV8) |
        P_Fld(0x1, SHU_COMMON0_DM64BITEN) | P_Fld(0x0, SHU_COMMON0_DLE256EN) |
        P_Fld(0x0, SHU_COMMON0_LP5BGEN) | P_Fld(0x0, SHU_COMMON0_LP5WCKON) |
        P_Fld(0x0, SHU_COMMON0_CL2) | P_Fld(0x0, SHU_COMMON0_BL2) |
        P_Fld(0x1, SHU_COMMON0_BL4) | P_Fld(0x0, SHU_COMMON0_LP5BGOTF) |
        P_Fld(0x1, SHU_COMMON0_BC4OTF) | P_Fld(0x0, SHU_COMMON0_LP5HEFF_MODE) |
        P_Fld(0x00000, SHU_COMMON0_SHU_COMMON0_RSV));

vIO32WriteFldMulti(DRAMC_REG_SHU_ACTIMING_CONF, P_Fld(0x26, SHU_ACTIMING_CONF_SCINTV) |
        P_Fld(0x0, SHU_ACTIMING_CONF_TRFCPBIG) | P_Fld(0x000, SHU_ACTIMING_CONF_REFBW_FR) |
        P_Fld(0x1, SHU_ACTIMING_CONF_TREFBWIG));

#if (fcFOR_CHIP_ID == fcA60868)
vIO32WriteFldMulti(DRAMC_REG_SHU_DCM_CTRL0, P_Fld(0x1, SHU_DCM_CTRL0_DDRPHY_CLK_EN_OPT) |
        P_Fld(0x4, SHU_DCM_CTRL0_DPHY_CMDDCM_EXTCNT) | P_Fld(0x5, SHU_DCM_CTRL0_DDRPHY_CLK_DYN_GATING_SEL) |
        P_Fld(0x0, SHU_DCM_CTRL0_CKE_EXTNONPD_CNT) | P_Fld(0x1, SHU_DCM_CTRL0_FASTWAKE2) |
        P_Fld(0x1, SHU_DCM_CTRL0_FASTWAKE));
#elif (fcFOR_CHIP_ID == fcPetrus)
vIO32WriteFldMulti(DRAMC_REG_SHU_DCM_CTRL0, P_Fld(0x1, SHU_DCM_CTRL0_DDRPHY_CLK_EN_OPT) |
        P_Fld(0x5, SHU_DCM_CTRL0_DDRPHY_CLK_DYN_GATING_SEL) |
        P_Fld(0x1, SHU_DCM_CTRL0_FASTWAKE2) |
        P_Fld(0x1, SHU_DCM_CTRL0_FASTWAKE));
#endif

vIO32WriteFldMulti(DRAMC_REG_SHU_CONF0, P_Fld(0x3f, SHU_CONF0_DMPGTIM) |
        P_Fld(0x0, SHU_CONF0_ADVREFEN) | P_Fld(0x1, SHU_CONF0_ADVPREEN) |
        P_Fld(0x1, SHU_CONF0_PBREFEN) | P_Fld(0x1, SHU_CONF0_REFTHD) |
        P_Fld(0x8, SHU_CONF0_REQQUE_DEPTH));

vIO32WriteFldMulti(DRAMC_REG_SHU_MATYPE, P_Fld(0x2, SHU_MATYPE_MATYPE) |
        P_Fld(0x1, SHU_MATYPE_NORMPOP_LEN));

vIO32WriteFldAlign(DRAMC_REG_SHU_SCHEDULER, 0x1, SHU_SCHEDULER_DUALSCHEN);

vIO32WriteFldMulti(DRAMC_REG_TX_SET0, P_Fld(0x0, TX_SET0_TXRANK) |
        P_Fld(0x0, TX_SET0_TXRANKFIX) | P_Fld(0x0, TX_SET0_DDRPHY_COMB_CG_SEL) |
        P_Fld(0x1, TX_SET0_TX_DQM_DEFAULT) | P_Fld(0x0, TX_SET0_DQBUS_X32) |
        P_Fld(0x0, TX_SET0_OE_DOWNGRADE) | P_Fld(0x0, TX_SET0_DQ16COM1) |
        P_Fld(0x1, TX_SET0_WPRE2T) | P_Fld(0x0, TX_SET0_DRSCLR_EN) |
        P_Fld(0x0, TX_SET0_DRSCLR_RK0_EN) | P_Fld(0x0, TX_SET0_ARPI_CAL_E2OPT) |
        P_Fld(0x0, TX_SET0_TX_DLY_CAL_E2OPT) | P_Fld(0x0, TX_SET0_DQS_OE_OP1_DIS) |
        P_Fld(0x0, TX_SET0_DQS_OE_OP2_EN) | P_Fld(0x0, TX_SET0_RK_SCINPUT_OPT) |
        P_Fld(0x0, TX_SET0_DRAMOEN));

#if (fcFOR_CHIP_ID == fcA60868)
vIO32WriteFldMulti(DRAMC_REG_SHU_TX_SET0, P_Fld(0x0, SHU_TX_SET0_DQOE_CNT) |
        P_Fld(0x0, SHU_TX_SET0_DQOE_OPT) | P_Fld(0x0, SHU_TX_SET0_TXUPD_SEL) |
        P_Fld(0x2, SHU_TX_SET0_TXUPD_W2R_SEL) | P_Fld(0x0, SHU_TX_SET0_WECC_EN) |
        P_Fld(0x0, SHU_TX_SET0_DBIWR) | P_Fld(0x0, SHU_TX_SET0_WDATRGO) |
        P_Fld(0x0, SHU_TX_SET0_TWPSTEXT) | P_Fld(0x1, SHU_TX_SET0_WPST1P5T) |
        P_Fld(0x3, SHU_TX_SET0_TXOEN_AUTOSET_OFFSET) | P_Fld(0x1, SHU_TX_SET0_TWCKPST) |
        P_Fld(0x1, SHU_TX_SET0_OE_EXT2UI) | P_Fld(0x0e, SHU_TX_SET0_DQS2DQ_FILT_PITHRD) |
        P_Fld(0x0, SHU_TX_SET0_TXOEN_AUTOSET_EN));
#elif (fcFOR_CHIP_ID == fcPetrus)
vIO32WriteFldMulti(DRAMC_REG_SHU_TX_SET0, P_Fld(0x0, SHU_TX_SET0_DQOE_CNT) |
        P_Fld(0x0, SHU_TX_SET0_DQOE_OPT) | P_Fld(0x0, SHU_TX_SET0_TXUPD_SEL) |
        P_Fld(0x2, SHU_TX_SET0_TXUPD_W2R_SEL) | P_Fld(0x0, SHU_TX_SET0_WECC_EN) |
        P_Fld(0x0, SHU_TX_SET0_DBIWR) | P_Fld(0x0, SHU_TX_SET0_WDATRGO) |
        P_Fld(0x1, SHU_TX_SET0_WPST1P5T) |
        P_Fld(0x3, SHU_TX_SET0_TXOEN_AUTOSET_OFFSET) | P_Fld(0x1, SHU_TX_SET0_TWCKPST) |
        P_Fld(0x1, SHU_TX_SET0_OE_EXT2UI) | P_Fld(0x0e, SHU_TX_SET0_DQS2DQ_FILT_PITHRD) |
        P_Fld(0x0, SHU_TX_SET0_TXOEN_AUTOSET_EN));
#endif

vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_STBCAL1, P_Fld(0x0, MISC_SHU_STBCAL1_DLLFRZRFCOPT) |
        P_Fld(0x0, MISC_SHU_STBCAL1_DLLFRZWROPT) | P_Fld(0x0, MISC_SHU_STBCAL1_R_RSTBCNT_LATCH_OPT) |
        P_Fld(0x1, MISC_SHU_STBCAL1_STB_UPDMASK_EN) | P_Fld(0x9, MISC_SHU_STBCAL1_STB_UPDMASKCYC) |
        P_Fld(0x1, MISC_SHU_STBCAL1_DQSINCTL_PRE_SEL));

vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_STBCAL, P_Fld(0x2, MISC_SHU_STBCAL_DMSTBLAT) |
        P_Fld(0x1, MISC_SHU_STBCAL_PICGLAT) | P_Fld(0x1, MISC_SHU_STBCAL_DQSG_MODE) |
        P_Fld(0x1, MISC_SHU_STBCAL_DQSIEN_PICG_MODE) | P_Fld(0x1, MISC_SHU_STBCAL_DQSIEN_DQSSTB_MODE) |
        P_Fld(0x1, MISC_SHU_STBCAL_DQSIEN_BURST_MODE) | P_Fld(0x0, MISC_SHU_STBCAL_DQSIEN_SELPH_FRUN) |
        P_Fld(0x1, MISC_SHU_STBCAL_STBCALEN) | P_Fld(0x1, MISC_SHU_STBCAL_STB_SELPHCALEN) |
        P_Fld(0x0, MISC_SHU_STBCAL_DQSIEN_4TO1_EN) | P_Fld(0x0, MISC_SHU_STBCAL_DQSIEN_8TO1_EN) |
        P_Fld(0x0, MISC_SHU_STBCAL_DQSIEN_16TO1_EN));

vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RODTENSTB, P_Fld(0x1, MISC_SHU_RODTENSTB_RODTENSTB_TRACK_EN) |
        P_Fld(0x0, MISC_SHU_RODTENSTB_RODTEN_P1_ENABLE) | P_Fld(0x0, MISC_SHU_RODTENSTB_RODTENSTB_4BYTE_EN) |
        P_Fld(0x1, MISC_SHU_RODTENSTB_RODTENSTB_TRACK_UDFLWCTRL) | P_Fld(0x1, MISC_SHU_RODTENSTB_RODTENSTB_SELPH_MODE) |
        P_Fld(0x0, MISC_SHU_RODTENSTB_RODTENSTB_SELPH_BY_BITTIME) | P_Fld(0x4, MISC_SHU_RODTENSTB_RODTENSTB__UI_OFFSET) |
        P_Fld(0x0, MISC_SHU_RODTENSTB_RODTENSTB_MCK_OFFSET) | P_Fld(0x0008, MISC_SHU_RODTENSTB_RODTENSTB_EXT));

vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_RX_SELPH_MODE, P_Fld(0x2, MISC_SHU_RX_SELPH_MODE_DQSIEN_SELPH_SERMODE) |
        P_Fld(0x1, MISC_SHU_RX_SELPH_MODE_RODT_SELPH_SERMODE) | P_Fld(0x1, MISC_SHU_RX_SELPH_MODE_RANK_SELPH_SERMODE));

vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ7, P_Fld(0x0, SHU_B0_DQ7_R_DMRANKRXDVS_B0) |
        P_Fld(0x1, SHU_B0_DQ7_R_DMDQMDBI_EYE_SHU_B0) | P_Fld(0x1, SHU_B0_DQ7_R_DMDQMDBI_SHU_B0) |
        P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_DQM_FLAGSEL_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B0) |
        P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMRXTRACK_DQM_EN_B0) |
        P_Fld(0x1, SHU_B0_DQ7_R_DMRODTEN_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMARPI_CG_FB2DLL_DCM_EN_B0) |
        P_Fld(0x0, SHU_B0_DQ7_R_DMTX_ARPI_CG_DQ_NEW_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMTX_ARPI_CG_DQS_NEW_B0) |
        P_Fld(0x0, SHU_B0_DQ7_R_DMTX_ARPI_CG_DQM_NEW_B0) | P_Fld(0x0, SHU_B0_DQ7_R_LP4Y_SDN_MODE_DQS0) |
        P_Fld(0x1, SHU_B0_DQ7_R_DMRXRANK_DQ_EN_B0) | P_Fld(0x2, SHU_B0_DQ7_R_DMRXRANK_DQ_LAT_B0) |
        P_Fld(0x1, SHU_B0_DQ7_R_DMRXRANK_DQS_EN_B0) | P_Fld(0x1, SHU_B0_DQ7_R_DMRXRANK_DQS_LAT_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ7, P_Fld(0x0, SHU_B1_DQ7_R_DMRANKRXDVS_B1) |
        P_Fld(0x1, SHU_B1_DQ7_R_DMDQMDBI_EYE_SHU_B1) | P_Fld(0x1, SHU_B1_DQ7_R_DMDQMDBI_SHU_B1) |
        P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_DQM_FLAGSEL_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B1) |
        P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMRXTRACK_DQM_EN_B1) |
        P_Fld(0x1, SHU_B1_DQ7_R_DMRODTEN_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMARPI_CG_FB2DLL_DCM_EN_B1) |
        P_Fld(0x0, SHU_B1_DQ7_R_DMTX_ARPI_CG_DQ_NEW_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMTX_ARPI_CG_DQS_NEW_B1) |
        P_Fld(0x0, SHU_B1_DQ7_R_DMTX_ARPI_CG_DQM_NEW_B1) | P_Fld(0x0, SHU_B1_DQ7_R_LP4Y_SDN_MODE_DQS1) |
        P_Fld(0x1, SHU_B1_DQ7_R_DMRXRANK_DQ_EN_B1) | P_Fld(0x2, SHU_B1_DQ7_R_DMRXRANK_DQ_LAT_B1) |
        P_Fld(0x1, SHU_B1_DQ7_R_DMRXRANK_DQS_EN_B1) | P_Fld(0x1, SHU_B1_DQ7_R_DMRXRANK_DQS_LAT_B1));

#if (fcFOR_CHIP_ID == fcA60868)
vIO32WriteFldMulti(DRAMC_REG_SHU_TX_SET0, P_Fld(0x0, SHU_TX_SET0_DQOE_CNT) |
        P_Fld(0x0, SHU_TX_SET0_DQOE_OPT) | P_Fld(0x0, SHU_TX_SET0_TXUPD_SEL) |
        P_Fld(0x2, SHU_TX_SET0_TXUPD_W2R_SEL) | P_Fld(0x0, SHU_TX_SET0_WECC_EN) |
        P_Fld(0x1, SHU_TX_SET0_DBIWR) | P_Fld(0x0, SHU_TX_SET0_WDATRGO) |
        P_Fld(0x0, SHU_TX_SET0_TWPSTEXT) | P_Fld(0x1, SHU_TX_SET0_WPST1P5T) |
        P_Fld(0x3, SHU_TX_SET0_TXOEN_AUTOSET_OFFSET) | P_Fld(0x1, SHU_TX_SET0_TWCKPST) |
        P_Fld(0x1, SHU_TX_SET0_OE_EXT2UI) | P_Fld(0x0e, SHU_TX_SET0_DQS2DQ_FILT_PITHRD) |
        P_Fld(0x0, SHU_TX_SET0_TXOEN_AUTOSET_EN));
#elif (fcFOR_CHIP_ID == fcPetrus)
vIO32WriteFldMulti(DRAMC_REG_SHU_TX_SET0, P_Fld(0x0, SHU_TX_SET0_DQOE_CNT) |
        P_Fld(0x0, SHU_TX_SET0_DQOE_OPT) | P_Fld(0x0, SHU_TX_SET0_TXUPD_SEL) |
        P_Fld(0x2, SHU_TX_SET0_TXUPD_W2R_SEL) | P_Fld(0x0, SHU_TX_SET0_WECC_EN) |
        P_Fld(0x1, SHU_TX_SET0_DBIWR) | P_Fld(0x0, SHU_TX_SET0_WDATRGO) |
        P_Fld(0x1, SHU_TX_SET0_WPST1P5T) |
        P_Fld(0x3, SHU_TX_SET0_TXOEN_AUTOSET_OFFSET) | P_Fld(0x1, SHU_TX_SET0_TWCKPST) |
        P_Fld(0x1, SHU_TX_SET0_OE_EXT2UI) | P_Fld(0x0e, SHU_TX_SET0_DQS2DQ_FILT_PITHRD) |
        P_Fld(0x0, SHU_TX_SET0_TXOEN_AUTOSET_EN));
#endif

vIO32WriteFldMulti(DRAMC_REG_SHU_HWSET_MR2, P_Fld(0x0002, SHU_HWSET_MR2_HWSET_MR2_MRSMA) |
        P_Fld(0x3f, SHU_HWSET_MR2_HWSET_MR2_OP));
vIO32WriteFldMulti(DRAMC_REG_SHU_FREQ_RATIO_SET0, P_Fld(0x20, SHU_FREQ_RATIO_SET0_TDQSCK_JUMP_RATIO3) |
        P_Fld(0x72, SHU_FREQ_RATIO_SET0_TDQSCK_JUMP_RATIO2) | P_Fld(0x09, SHU_FREQ_RATIO_SET0_TDQSCK_JUMP_RATIO1) |
        P_Fld(0x20, SHU_FREQ_RATIO_SET0_TDQSCK_JUMP_RATIO0));

vIO32WriteFldMulti(DDRPHY_REG_MISC_SHU_DVFSDLL, P_Fld(0x0, MISC_SHU_DVFSDLL_R_BYPASS_1ST_DLL) |
        P_Fld(0x0, MISC_SHU_DVFSDLL_R_BYPASS_2ND_DLL) | P_Fld(0x5a, MISC_SHU_DVFSDLL_R_DLL_IDLE) |
        P_Fld(0x5a, MISC_SHU_DVFSDLL_R_2ND_DLL_IDLE));

    mcDELAY_US(1);

    mcDELAY_US(1);


     DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);

    mcDELAY_US(1);

    mcDELAY_US(1);


     DramcBroadcastOnOff(DRAMC_BROADCAST_ON);

vIO32WriteFldMulti(DRAMC_REG_SHU_DQSOSCR, P_Fld(0x15, SHU_DQSOSCR_DQSOSCRCNT) |
        P_Fld(0x0, SHU_DQSOSCR_DQSOSC_ADV_SEL) | P_Fld(0x0, SHU_DQSOSCR_DQSOSC_DRS_ADV_SEL) |
        P_Fld(0xffff, SHU_DQSOSCR_DQSOSC_DELTA));

vIO32WriteFldMulti(DRAMC_REG_SHU_DQSOSC_SET0, P_Fld(0x1, SHU_DQSOSC_SET0_DQSOSCENDIS) |
        P_Fld(0x012, SHU_DQSOSC_SET0_DQSOSC_PRDCNT) | P_Fld(0x0002, SHU_DQSOSC_SET0_DQSOSCENCNT));

vIO32WriteFldMulti(DRAMC_REG_SHURK_DQSOSC, P_Fld(0x0326, SHURK_DQSOSC_DQSOSC_BASE_RK0) |
        P_Fld(0x0326, SHURK_DQSOSC_DQSOSC_BASE_RK0_B1));

vIO32WriteFldMulti(DRAMC_REG_SHURK_DQSOSC+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x0159, SHURK_DQSOSC_DQSOSC_BASE_RK0) |
        P_Fld(0x0159, SHURK_DQSOSC_DQSOSC_BASE_RK0_B1));

vIO32WriteFldMulti(DRAMC_REG_SHURK_DQSOSC_THRD, P_Fld(0x018, SHURK_DQSOSC_THRD_DQSOSCTHRD_INC) |
        P_Fld(0x010, SHURK_DQSOSC_THRD_DQSOSCTHRD_DEC));

vIO32WriteFldMulti(DRAMC_REG_SHURK_DQSOSC_THRD+(1*DRAMC_REG_AO_RANK_OFFSET), P_Fld(0x004, SHURK_DQSOSC_THRD_DQSOSCTHRD_INC) |
        P_Fld(0x002, SHURK_DQSOSC_THRD_DQSOSCTHRD_DEC));

#if (fcFOR_CHIP_ID == fcA60868)
vIO32WriteFldMulti(DRAMC_REG_SHU_TX_SET0, P_Fld(0x0, SHU_TX_SET0_DQOE_CNT) |
        P_Fld(0x0, SHU_TX_SET0_DQOE_OPT) | P_Fld(0x0, SHU_TX_SET0_TXUPD_SEL) |
        P_Fld(0x2, SHU_TX_SET0_TXUPD_W2R_SEL) | P_Fld(0x0, SHU_TX_SET0_WECC_EN) |
        P_Fld(0x1, SHU_TX_SET0_DBIWR) | P_Fld(0x0, SHU_TX_SET0_WDATRGO) |
        P_Fld(0x0, SHU_TX_SET0_TWPSTEXT) | P_Fld(0x1, SHU_TX_SET0_WPST1P5T) |
        P_Fld(0x3, SHU_TX_SET0_TXOEN_AUTOSET_OFFSET) | P_Fld(0x1, SHU_TX_SET0_TWCKPST) |
        P_Fld(0x1, SHU_TX_SET0_OE_EXT2UI) | P_Fld(0x17, SHU_TX_SET0_DQS2DQ_FILT_PITHRD) |
        P_Fld(0x0, SHU_TX_SET0_TXOEN_AUTOSET_EN));
#elif (fcFOR_CHIP_ID == fcPetrus)
vIO32WriteFldMulti(DRAMC_REG_SHU_TX_SET0, P_Fld(0x0, SHU_TX_SET0_DQOE_CNT) |
        P_Fld(0x0, SHU_TX_SET0_DQOE_OPT) | P_Fld(0x0, SHU_TX_SET0_TXUPD_SEL) |
        P_Fld(0x2, SHU_TX_SET0_TXUPD_W2R_SEL) | P_Fld(0x0, SHU_TX_SET0_WECC_EN) |
        P_Fld(0x1, SHU_TX_SET0_DBIWR) | P_Fld(0x0, SHU_TX_SET0_WDATRGO) |
        P_Fld(0x1, SHU_TX_SET0_WPST1P5T) |
        P_Fld(0x3, SHU_TX_SET0_TXOEN_AUTOSET_OFFSET) | P_Fld(0x1, SHU_TX_SET0_TWCKPST) |
        P_Fld(0x1, SHU_TX_SET0_OE_EXT2UI) | P_Fld(0x17, SHU_TX_SET0_DQS2DQ_FILT_PITHRD) |
        P_Fld(0x0, SHU_TX_SET0_TXOEN_AUTOSET_EN));
#endif

vIO32WriteFldMulti(DRAMC_REG_SHU_ZQ_SET0, P_Fld(0x0000, SHU_ZQ_SET0_ZQCSCNT) |
        P_Fld(0x1d, SHU_ZQ_SET0_TZQLAT));

vIO32WriteFldMulti(DRAMC_REG_SHU_ZQ_SET0, P_Fld(0x0005, SHU_ZQ_SET0_ZQCSCNT) |
        P_Fld(0x1d, SHU_ZQ_SET0_TZQLAT));

vIO32WriteFldMulti(DRAMC_REG_SHU_HMR4_DVFS_CTRL0, P_Fld(0x86, SHU_HMR4_DVFS_CTRL0_FSPCHG_PRDCNT) |
        P_Fld(0x005, SHU_HMR4_DVFS_CTRL0_REFRCNT));

vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ8, P_Fld(0x0100, SHU_B0_DQ8_R_DMRXDVS_UPD_FORCE_CYC_B0) |
        P_Fld(0x1, SHU_B0_DQ8_R_DMRXDVS_UPD_FORCE_EN_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B0) |
        P_Fld(0x0, SHU_B0_DQ8_R_RMRODTEN_CG_IG_B0) | P_Fld(0x1, SHU_B0_DQ8_R_RMRX_TOPHY_CG_IG_B0) |
        P_Fld(0x0, SHU_B0_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B0) |
        P_Fld(0x1, SHU_B0_DQ8_R_DMRXDLY_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B0) |
        P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B0) |
        P_Fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B0) | P_Fld(0x0, SHU_B0_DQ8_R_DMRANK_PIPE_CG_IG_B0) |
        P_Fld(0x0, SHU_B0_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ8, P_Fld(0x0100, SHU_B1_DQ8_R_DMRXDVS_UPD_FORCE_CYC_B1) |
        P_Fld(0x1, SHU_B1_DQ8_R_DMRXDVS_UPD_FORCE_EN_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B1) |
        P_Fld(0x0, SHU_B1_DQ8_R_RMRODTEN_CG_IG_B1) | P_Fld(0x1, SHU_B1_DQ8_R_RMRX_TOPHY_CG_IG_B1) |
        P_Fld(0x0, SHU_B1_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B1) |
        P_Fld(0x1, SHU_B1_DQ8_R_DMRXDLY_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B1) |
        P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B1) |
        P_Fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B1) | P_Fld(0x0, SHU_B1_DQ8_R_DMRANK_PIPE_CG_IG_B1) |
        P_Fld(0x0, SHU_B1_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ7, P_Fld(0x0, SHU_B0_DQ7_R_DMRANKRXDVS_B0) |
        P_Fld(0x1, SHU_B0_DQ7_R_DMDQMDBI_EYE_SHU_B0) | P_Fld(0x1, SHU_B0_DQ7_R_DMDQMDBI_SHU_B0) |
        P_Fld(0xb, SHU_B0_DQ7_R_DMRXDVS_DQM_FLAGSEL_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B0) |
        P_Fld(0x1, SHU_B0_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B0) | P_Fld(0x1, SHU_B0_DQ7_R_DMRXTRACK_DQM_EN_B0) |
        P_Fld(0x1, SHU_B0_DQ7_R_DMRODTEN_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMARPI_CG_FB2DLL_DCM_EN_B0) |
        P_Fld(0x0, SHU_B0_DQ7_R_DMTX_ARPI_CG_DQ_NEW_B0) | P_Fld(0x0, SHU_B0_DQ7_R_DMTX_ARPI_CG_DQS_NEW_B0) |
        P_Fld(0x0, SHU_B0_DQ7_R_DMTX_ARPI_CG_DQM_NEW_B0) | P_Fld(0x0, SHU_B0_DQ7_R_LP4Y_SDN_MODE_DQS0) |
        P_Fld(0x1, SHU_B0_DQ7_R_DMRXRANK_DQ_EN_B0) | P_Fld(0x2, SHU_B0_DQ7_R_DMRXRANK_DQ_LAT_B0) |
        P_Fld(0x1, SHU_B0_DQ7_R_DMRXRANK_DQS_EN_B0) | P_Fld(0x1, SHU_B0_DQ7_R_DMRXRANK_DQS_LAT_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ7, P_Fld(0x0, SHU_B1_DQ7_R_DMRANKRXDVS_B1) |
        P_Fld(0x1, SHU_B1_DQ7_R_DMDQMDBI_EYE_SHU_B1) | P_Fld(0x1, SHU_B1_DQ7_R_DMDQMDBI_SHU_B1) |
        P_Fld(0xb, SHU_B1_DQ7_R_DMRXDVS_DQM_FLAGSEL_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B1) |
        P_Fld(0x1, SHU_B1_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B1) | P_Fld(0x1, SHU_B1_DQ7_R_DMRXTRACK_DQM_EN_B1) |
        P_Fld(0x1, SHU_B1_DQ7_R_DMRODTEN_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMARPI_CG_FB2DLL_DCM_EN_B1) |
        P_Fld(0x0, SHU_B1_DQ7_R_DMTX_ARPI_CG_DQ_NEW_B1) | P_Fld(0x0, SHU_B1_DQ7_R_DMTX_ARPI_CG_DQS_NEW_B1) |
        P_Fld(0x0, SHU_B1_DQ7_R_DMTX_ARPI_CG_DQM_NEW_B1) | P_Fld(0x0, SHU_B1_DQ7_R_LP4Y_SDN_MODE_DQS1) |
        P_Fld(0x1, SHU_B1_DQ7_R_DMRXRANK_DQ_EN_B1) | P_Fld(0x2, SHU_B1_DQ7_R_DMRXRANK_DQ_LAT_B1) |
        P_Fld(0x1, SHU_B1_DQ7_R_DMRXRANK_DQS_EN_B1) | P_Fld(0x1, SHU_B1_DQ7_R_DMRXRANK_DQS_LAT_B1));

vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ11, P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_RANK_SEL_SER_EN_B0) |
        P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_RANK_SEL_LAT_EN_B0) | P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_OFFSETC_LAT_EN_B0) |
        P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_OFFSETC_EN_B0) | P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_OFFSETC_BIAS_EN_B0) |
        P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_FRATE_EN_B0) | P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_CDR_EN_B0) |
        P_Fld(0x1, SHU_B0_DQ11_RG_RX_ARDQ_DVS_EN_B0) | P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_DVS_DLY_B0) |
        P_Fld(0x2, SHU_B0_DQ11_RG_RX_ARDQ_DES_MODE_B0) | P_Fld(0x0, SHU_B0_DQ11_RG_RX_ARDQ_BW_SEL_B0));

vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ11, P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_RANK_SEL_SER_EN_B1) |
        P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_RANK_SEL_LAT_EN_B1) | P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_OFFSETC_LAT_EN_B1) |
        P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_OFFSETC_EN_B1) | P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_OFFSETC_BIAS_EN_B1) |
        P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_FRATE_EN_B1) | P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_CDR_EN_B1) |
        P_Fld(0x1, SHU_B1_DQ11_RG_RX_ARDQ_DVS_EN_B1) | P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_DVS_DLY_B1) |
        P_Fld(0x2, SHU_B1_DQ11_RG_RX_ARDQ_DES_MODE_B1) | P_Fld(0x0, SHU_B1_DQ11_RG_RX_ARDQ_BW_SEL_B1));

}




