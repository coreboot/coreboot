/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef _DRAMC_DV_INIT_H_
#define _DRAMC_DV_INIT_H_

#include "dramc_common.h"
#include "dramc_int_global.h"
#include "x_hal_io.h"
#include "sv_c_data_traffic.h"

//=========================================================
//DRAM CONFIG ELEMENT COLLECTION
//=========================================================
typedef enum { DDR3, DDR4, LPDDR3, LPDDR4, LPDDR5, PSRAM } DRAM_TYPE_T;
typedef enum {BG4BK4, BK8, BK16, BKORG_RFU} e_BKORG;
typedef enum {DIS_both, EN_t, EN_both, EN_c} e_RDQSWCK;//MR20

#define SA_CONFIG_EN 1
#define DV_CONFIG_EN 1
//=========================================================
//Build Top configuration
//=========================================================
#define DFS_GROUP_NUM 10
#define CH_NUM        2
#define RK_NUM_PER_CH 2
#define DONT_CARE_VALUE 0
#define PULL_UP   1
#define PULL_DOWN 1



typedef struct Gating_config
{
    U8 GAT_TRACK_EN         ;
    U8 RX_GATING_MODE       ;
    U8 RX_GATING_TRACK_MODE ;
    U8 SELPH_MODE           ;
    U8 PICG_EARLY_EN        ;
    U8 VALID_LAT_VALUE      ;
}Gating_confg_T;


//=========================================================
//DV configuration connection
//=========================================================
#if DV_CONFIG_EN==1
typedef struct DRAMC_DVFS_GROUP_transfer
{
    U8                CKR         ; //LPDDR5 CKR could be 4 and 2  other memory type should be 1
    U8                DQSIEN_MODE ; //ANA DQSG mode config  LPDDR4 = 1, LPDDR5 with other modes
    U8                DQ_P2S_RATIO; //16-1 8-1 4-1   LPDDR5 could support 16-1 mode
    U8                RESERVED_8BIT;
    U32               data_rate   ;
}DRAMC_DVFS_GROUP_transfer_T;

typedef struct DV_configuration
{
    U8  EX_ROW_EN_1      ;
    U8  EX_ROW_EN_0      ;
    U8  BYTE_MODE_1      ;
    U8  BYTE_MODE_0      ;
    U8  LP4Y_EN          ;
    U8  LP4_WR_PST       ;
    U8  LP4_OTF          ;
    U8  NEW_8X_MODE      ;
    U8  LP45_APHY_COMB_EN;
    U8  DLL_IDLE_MODE    ;
    U8  NEW_RANK_MODE    ;
    U8  DLL_ASYNC_EN     ;
    U8  MD32_EN          ;
    U8  SRAM_EN          ;
    U8  GP_NUM           ;
} DV_new_config_T;
#endif


//=========================================================
//LPDDR4 DRAM config
//=========================================================
typedef struct LP4_DRAM_CONFIG
{
    U8  BYTE_MODE[2]; //diff rank
    U8  EX_ROW_EN[2]; //diff rank --density over 10G should 1
    U8  MR_WL    ;
	U8  MR_RL    ;
	U8  BL       ;
	U8  RPST     ;
	U8  RD_PRE   ;
	U8  WR_PRE   ;
	U8  WR_PST   ;
	U8  DBI_WR   ;
	U8  DBI_RD   ;
//	U8  DMI      ;    //No use  default enable
	U8  OTF      ;
    U8  LP4Y_EN  ;
    U8  WORK_FSP ;
} LP4_DRAM_CONFIG_T;



//=========================================================
//LPDDR5 DRAM config
//=========================================================
typedef struct LP5_DRAM_CONFIG
{
    U8          BYTE_MODE[2] ;
    U8          EX_ROW_EN[2] ;
    U8          MR_WL        ;
	U8          MR_RL        ;
	U8          BL           ;
	U8          CK_Mode      ;
	U8          RPST         ;
	U8          RD_PRE       ;
	U8          WR_PRE       ;
	U8          WR_PST       ;
	U8          DBI_WR       ;
	U8          DBI_RD       ;
	U8          DMI          ;
	U8          OTF          ;
    U8          WCK_PST      ;
    U8          RDQS_PRE     ;
    U8          RDQS_PST     ;
    U8          CA_ODT       ;
    U8          DQ_ODT       ;
    U8          CKR          ;
    U8          WCK_ON       ;
    U8          WCK_FM       ;
    U8          WCK_ODT      ;
    U8          DVFSQ        ;
    U8          DVFSC        ;
    e_RDQSWCK   RDQSmode[2]  ;
    U8          WCKmode[2]   ;
    U8          RECC         ;
    U8          WECC         ;
    e_BKORG     BankMode     ;
    U8          WORK_FSP     ;
} LP5_DRAM_CONFIG_T;

//=========================================================
//Analog PHY config
//=========================================================
typedef struct ANA_top_function_config
{
    U8   DLL_ASYNC_EN     ;
    U8   ALL_SLAVE_EN     ;
    U8   NEW_RANK_MODE    ;
    U8   DLL_IDLE_MODE    ;
    U8   LP45_APHY_COMB_EN;
    U8   TX_ODT_DIS       ;
    U8   NEW_8X_MODE      ;
}ANA_top_config_T;


typedef struct ANA_DVFS_core_config
{
    U8   CKR;
    U8   DQ_P2S_RATIO;
    U8   LP5_1600_DQ_P2S_MODE;
    U8   CA_P2S_RATIO;
    U8   DQ_CA_OPEN;
    U8   DQ_SEMI_OPEN;
    U8   CA_SEMI_OPEN;
    U8   CA_FULL_RATE;
    U8   DQ_CKDIV4_EN;
    U8   CA_CKDIV4_EN;
    U8   CA_PREDIV_EN;
    U8   PH8_DLY;
    U8   SEMI_OPEN_CA_PICK_MCK_RATIO;
    U8   DQ_AAMCK_DIV;
    U8   CA_AAMCK_DIV;
    U8   CA_ADMCK_DIV;
    U8   DQ_TRACK_CA_EN;
    U32  PLL_FREQ;
    U8   DQ_UI_PI_RATIO;
    U8   CA_UI_PI_RATIO;
} ANA_DVFS_CORE_T;


//=========================================================
//DVFS group configuration
//=========================================================
typedef struct DRAMC_DVFS_GROUP_CONFIG
{
    U32               data_rate   ;
    U8                DQSIEN_MODE ; //ANA DQSG mode config  LPDDR4 = 1, LPDDR5 with other modes
    U8                DQ_P2S_RATIO; //16-1 8-1 4-1   LPDDR5 could support 16-1 mode
    U8                CKR         ; //LPDDR5 CKR could be 4 and 2  other memory type should be 1
}DRAMC_DVFS_GROUP_CONFIG_T;

//=========================================================
//DRAMC Subsystem config
//=========================================================
typedef struct DRAMC_SUBSYS_CONFIG
{
    U8                        GP_NUM               ;
    U8                        SRAM_EN              ;
    U8                        MD32_EN              ;
    ANA_top_config_T          *a_cfg               ;
    ANA_DVFS_CORE_T           *a_opt               ;
    LP4_DRAM_CONFIG_T         *lp4_init            ;
    LP5_DRAM_CONFIG_T         *lp5_init            ;
    DRAMC_DVFS_GROUP_CONFIG_T *DFS_GP[DFS_GROUP_NUM];
}DRAMC_SUBSYS_CONFIG_T;


typedef struct DUT_shuf_config_T {
  U8         CKE_DBE_CNT                   ;
  U8         FASTWAKE2                     ;
  U8         DMPGTIM                       ;
  U8         ADVPREEN                      ;
  U8         DLE_256EN                     ;
  U8         LECC                          ;
  U8         WPST1P5T_OPT                  ;
  U8         LP4YEN                        ;
  U8         LP5_CAS_MODE                  ;
  U8         LP5_SEP_ACT                   ;
  U8         LP5_BGOTF                     ;
  U8         LP5_BGEN                      ;
  U8         LP5_RDQS_SE_EN                ;
  U8         CKR                           ;
  U8         DQSIEN_MODE                   ;
  U8         DQ_P2S_RATIO                  ;
  U32        data_rate                     ;
}__attribute__((packed))  DUT_shuf_config_T;


typedef struct DUT_top_set_T {
  U8         DVFSRTMRWEN                   ;
  U8         NO_QUEUEFLUSH_EN              ;
  U8         RG_SPM_MODE                   ;
  U8         MD32_EN                       ;
  U8         SRAM_EN                       ;
  U8         RX_PIPE_BYPASS_EN             ;
  U8         TX_PIPE_BYPASS_EN             ;
  U32        WAIT_DLE_EXT_DLY              ;
  U32        RX_DCM_EXT_DLY                ;
  U8         old_dcm_mode                  ;
  U8         DPHY_DCM_MODE                 ;
  U8         TX_OE_EXT_OPT                 ;
  U8         TXP_WORKAROUND_OPT            ;
  U32        VALID_LAT_VALUE               ;
  U8         RXTRACK_PBYTE_OPT             ;
  U8         TRACK_UP_MODE                 ;
  U8         TREFBWIG_IGNORE               ;
  U8         SELPH_MODE                    ;
  U8         RANK_SWAP                     ;
  U8         BGPIPE_EN                     ;
  U8         PICG_MODE                     ;
  U8         RTMRR_MODE                    ;
  U8         TMRRI_MODE                    ;
  U8         DQS_OSC_AT_TIMER              ;
  U8         WPST1P5T_OPT                  ;
  U8         LP5_ZQ_OPT                    ;
  U8         LP5WRAPEN                     ;
  U8         LP4_SE_MODE                   ;
  U8         LP4Y_EN                       ;
  U8         LP4_WR_PST                    ;
  U8         LP4_OTF                       ;
  U8         PLL_MODE_OPTION               ;
  U8         NEW_8X_MODE                   ;
  U8         LP45_APHY_COMB_EN             ;
  U8         DLL_IDLE_MODE                 ;
  U8         NEW_RANK_MODE                 ;
  U8         DLL_ASYNC_EN                  ;
  U32        memory_type                   ;
  U32        GP_NUM                        ;
}__attribute__((packed))  DUT_top_set_T;



extern Gating_confg_T Gat_p;
extern DRAM_TYPE_T MEM_TYPE;
extern LP4_DRAM_CONFIG_T LP4_INIT;
extern LP5_DRAM_CONFIG_T LP5_INIT;
extern ANA_top_config_T ana_top_p;
extern ANA_DVFS_CORE_T ANA_option;
extern DRAMC_DVFS_GROUP_CONFIG_T DFS_TOP[DFS_GROUP_NUM];
extern DRAMC_SUBSYS_CONFIG_T DV_p;
extern DRAMC_CTX_T *DramcConfig;
extern DUT_top_set_T  DUTTopSetGlobal;
extern DUT_shuf_config_T  DUTShufConfigGlobal[10];

#define A_T DV_p.a_cfg
#define A_D DV_p.a_opt
#define M_LP4 DV_p.lp4_init
#define DFS(i) DV_p.DFS_GP[i]
#define LPDDR5_EN_S ((MEM_TYPE==LPDDR5) ? 1 : 0)
#define LPDDR4_EN_S ((MEM_TYPE==LPDDR4) ? 1 : 0)

#define  DUT_p DUTTopSetGlobal
#define  DUT_shu_p DUTShufConfigGlobal


#if FOR_DV_SIMULATION_USED==1
EXTERN void register_write(int address, int data);
EXTERN void register_read(int address, int * data);
EXTERN void delay_us(u32 delta);
EXTERN void delay_ns(u32 delta);
EXTERN void timestamp_show();
EXTERN void build_api_initial();
EXTERN void register_write_c(u32 address, u32 data);
EXTERN u32 register_read_c(u32 address);
EXTERN void conf_to_sram_sudo(int ch_id , int group_id, int conf_id);
//================ added by Lingyun Wu 11.14 =====================
EXTERN void broadcast_on(void);
EXTERN void broadcast_off(void);
//================ added by Lingyun Wu 11.14 =====================
EXTERN void mygetscope();
EXTERN void mysetscope();
#endif


#if DV_CONFIG_EN
extern void get_dfs_configuration_from_DV_random(DRAMC_DVFS_GROUP_transfer_T * tr, int group_id);
extern void get_top_configuration_from_DV_random(DV_new_config_T * tr);
#endif
//DRAM LP4 initial configuration
extern U8 LP4_DRAM_INIT_RLWL_MRfield_config(U32 data_rate);



extern void DPI_SW_main_LP4(DRAMC_CTX_T *p, cal_sv_rand_args_t *psra);
extern void DRAMC_SUBSYS_PRE_CONFIG(DRAMC_CTX_T *p, DRAMC_SUBSYS_CONFIG_T *tr);
extern void LP4_DRAM_config(U32 data_rate, LP4_DRAM_CONFIG_T *tr);
extern void LP5_DRAM_config(DRAMC_DVFS_GROUP_CONFIG_T *dfs_tr, LP5_DRAM_CONFIG_T *tr);
extern void ANA_TOP_FUNCTION_CFG(ANA_top_config_T *tr,U16 data_rate);
extern void ANA_CLK_DIV_config( ANA_DVFS_CORE_T *tr,DRAMC_DVFS_GROUP_CONFIG_T *dfs);
extern void ANA_sequence_shuffle_colletion(DRAMC_CTX_T *p,ANA_DVFS_CORE_T *tr);
extern void ANA_Config_shuffle(DRAMC_CTX_T *p,ANA_top_config_T *a_cfg,U8 group_id);

#endif   // _DRAMC_DV_INIT_H_
