/* SPDX-License-Identifier: BSD-3-Clause */

#include "dramc_dv_init.h"

DRAM_TYPE_T MEM_TYPE = LPDDR4;
LP4_DRAM_CONFIG_T LP4_INIT;
LP5_DRAM_CONFIG_T LP5_INIT;
ANA_top_config_T ana_top_p;
ANA_DVFS_CORE_T ANA_option;
DRAMC_DVFS_GROUP_CONFIG_T DFS_TOP[DFS_GROUP_NUM];
DRAMC_SUBSYS_CONFIG_T DV_p;


void ANA_TOP_FUNCTION_CFG(ANA_top_config_T *tr,U16 data_rate)
{


    tr->ALL_SLAVE_EN     = (data_rate <= 1866)?1:0;

    if(LPDDR5_EN_S == 1)
    {
        tr->TX_ODT_DIS = (data_rate <=3200) ? 1 : 0 ;
    } else {
        tr->TX_ODT_DIS = (data_rate <=2400) ? 1 : 0 ;
    }

    mcSHOW_DBG_MSG6(("=================================== \n"));
    mcSHOW_DBG_MSG6(("ANA top config\n"              ));
    mcSHOW_DBG_MSG6(("=================================== \n"));
    mcSHOW_DBG_MSG6(("DLL_ASYNC_EN            = %2d\n",tr->DLL_ASYNC_EN     ));
    mcSHOW_DBG_MSG6(("ALL_SLAVE_EN            = %2d\n",tr->ALL_SLAVE_EN     ));
    mcSHOW_DBG_MSG6(("NEW_RANK_MODE           = %2d\n",tr->NEW_RANK_MODE    ));
    mcSHOW_DBG_MSG6(("DLL_IDLE_MODE           = %2d\n",tr->DLL_IDLE_MODE    ));
    mcSHOW_DBG_MSG6(("LP45_APHY_COMB_EN       = %2d\n",tr->LP45_APHY_COMB_EN));
    mcSHOW_DBG_MSG6(("TX_ODT_DIS              = %2d\n",tr->TX_ODT_DIS       ));
    mcSHOW_DBG_MSG6(("NEW_8X_MODE             = %2d\n",tr->NEW_8X_MODE      ));
    mcSHOW_DBG_MSG6(("=================================== \n"));
}


void ANA_CLK_DIV_config( ANA_DVFS_CORE_T *tr,DRAMC_DVFS_GROUP_CONFIG_T *dfs)
{
    U32  SEMI_OPEN_FMIN = 300;
    U32  SEMI_OPEN_FMAX = 500;
    U32  PI_FMIN        = 600;
    U32  DQ_PICK;
    U32  CA_PICK;
    U32  CA_MCKIO;
    U32  MCKIO_SEMI;
    U16  data_rate;

    data_rate = dfs->data_rate;
    tr->DQ_P2S_RATIO = dfs->DQ_P2S_RATIO;
    tr->CKR = dfs->CKR;


    tr->CA_P2S_RATIO = tr->DQ_P2S_RATIO/tr->CKR;


    tr->DQ_CA_OPEN   = ( data_rate < (SEMI_OPEN_FMIN * 2) ) ? 1 : 0;
    tr->DQ_SEMI_OPEN = ( data_rate/2 < PI_FMIN ) ? (1-tr->DQ_CA_OPEN) : ((data_rate <= SEMI_OPEN_FMAX*2) ? (1-tr->DQ_CA_OPEN) : 0);
    tr->CA_SEMI_OPEN = (( data_rate/(tr->CKR*2) < PI_FMIN ) ? ((data_rate/(tr->CKR*2) > SEMI_OPEN_FMAX) ? 0 : (((tr->CA_P2S_RATIO>2)||(tr->DQ_SEMI_OPEN))*(1-tr->DQ_CA_OPEN))) : tr->DQ_SEMI_OPEN);
    tr->CA_FULL_RATE = (tr->DQ_CA_OPEN == 1) ? ((tr->CKR>1)?1:0) : ((tr->DQ_SEMI_OPEN*tr->CA_SEMI_OPEN*(tr->CKR>>1)) + (( data_rate/(tr->CKR*2) < PI_FMIN) ? (1-tr->CA_SEMI_OPEN) : 0 ));
    tr->DQ_CKDIV4_EN = ( tr->DQ_SEMI_OPEN == 1) ? DONT_CARE_VALUE : ((( (data_rate/2) < 1200 ) ? 1 : 0 ) * (1-tr->DQ_CA_OPEN)) ;

    CA_MCKIO     = (data_rate/(tr->CKR*2))*(1+tr->CA_FULL_RATE);
    DQ_PICK      = (tr->DQ_SEMI_OPEN == 1) ? 0 : (data_rate/2) ;
    CA_PICK      = (tr->CA_SEMI_OPEN == 1) ? CA_MCKIO*2 : ((CA_MCKIO>=PI_FMIN) ? CA_MCKIO : (( CA_MCKIO >= (PI_FMIN/2) ) ? CA_MCKIO*2 : CA_MCKIO *4 ));

    tr->CA_CKDIV4_EN = ((CA_PICK < 1200 ) ? 1 : 0 ) * ( 1- tr->DQ_CA_OPEN) ;

    tr->CA_PREDIV_EN = (data_rate >= 4800) ? 1 : 0 ;

#if SA_CONFIG_EN
    if(LPDDR4_EN_S)
    {

        if (data_rate <= 1866)
            tr->PH8_DLY = 0;
        else if (data_rate <= 2400)
            tr->PH8_DLY = 0x12;
        else if (data_rate <= 3200)
            tr->PH8_DLY = 0xd;
        else if (data_rate <= 4266)
            tr->PH8_DLY = 0x8;
        else
            tr->PH8_DLY = 0x7;
    }
    else
#endif
    {
        tr->PH8_DLY = ((tr->DQ_CA_OPEN == 0) && (tr->DQ_SEMI_OPEN == 0) && (tr->DQ_CKDIV4_EN  == 0)) ? ( (1000000>>4)/data_rate -4) : DONT_CARE_VALUE;
    }

    MCKIO_SEMI = (tr->DQ_SEMI_OPEN * tr->CA_SEMI_OPEN * (data_rate/2)) + (1-tr->DQ_SEMI_OPEN) * tr->CA_SEMI_OPEN * CA_MCKIO;

    tr->SEMI_OPEN_CA_PICK_MCK_RATIO = ( MCKIO_SEMI == 0) ? DONT_CARE_VALUE : (CA_PICK*tr->DQ_P2S_RATIO)/data_rate ;

    tr->DQ_AAMCK_DIV = (tr->DQ_SEMI_OPEN == 0) ? ((tr->DQ_P2S_RATIO/2)*(1-tr->DQ_SEMI_OPEN)) : DONT_CARE_VALUE;
    tr->CA_AAMCK_DIV = (tr->CA_SEMI_OPEN == 0) ? ((tr->DQ_P2S_RATIO/(2*tr->CKR))*(1+tr->CA_FULL_RATE)) : DONT_CARE_VALUE;
    tr->CA_ADMCK_DIV = CA_PICK/(data_rate/tr->DQ_P2S_RATIO);
    //tr->DQ_TRACK_CA_EN = ((data_rate/2) >= 2133) ? 1 : 0 ;
    tr->DQ_TRACK_CA_EN = 0 ;
    tr->PLL_FREQ = ((DQ_PICK*2*(tr->DQ_CKDIV4_EN+1)) > (CA_PICK*2*(tr->CA_CKDIV4_EN+1))) ? (DQ_PICK*2*(tr->DQ_CKDIV4_EN+1)) : (CA_PICK*2*(tr->CA_CKDIV4_EN+1));
#if SA_CONFIG_EN
    //de-sense
    if(data_rate==2400)
        tr->PLL_FREQ = 2366;
    else if(data_rate==1200)
        tr->PLL_FREQ = 2288;
    else if(data_rate==3200 || data_rate==1600)
        tr->PLL_FREQ = 3068;
    else if(data_rate==800)
        tr->PLL_FREQ = 3016;
    else if(data_rate==400)
        tr->PLL_FREQ = 1600;
#endif
    tr->DQ_UI_PI_RATIO = 32;
    tr->CA_UI_PI_RATIO = (tr->CA_SEMI_OPEN == 0) ? ((tr->CA_FULL_RATE == 1)? 64 : DONT_CARE_VALUE) : 32;

    mcSHOW_DBG_MSG6(("=================================== \n"));
    mcSHOW_DBG_MSG6(("data_rate                  = %4d\n"    ,data_rate                      ));
    mcSHOW_DBG_MSG6(("CKR                        = %1d\n"    ,tr->CKR                        ));
    mcSHOW_DBG_MSG6(("DQ_P2S_RATIO               = %1d\n"    ,tr->DQ_P2S_RATIO               ));
    mcSHOW_DBG_MSG6(("=================================== \n"));
    mcSHOW_DBG_MSG6(("CA_P2S_RATIO               = %1d\n"    ,tr->CA_P2S_RATIO               ));
    mcSHOW_DBG_MSG6(("DQ_CA_OPEN                 = %1d\n"    ,tr->DQ_CA_OPEN                 ));
    mcSHOW_DBG_MSG6(("DQ_SEMI_OPEN               = %1d\n"    ,tr->DQ_SEMI_OPEN               ));
    mcSHOW_DBG_MSG6(("CA_SEMI_OPEN               = %1d\n"    ,tr->CA_SEMI_OPEN               ));
    mcSHOW_DBG_MSG6(("CA_FULL_RATE               = %1d\n"    ,tr->CA_FULL_RATE               ));
    mcSHOW_DBG_MSG6(("DQ_CKDIV4_EN               = %1d\n"    ,tr->DQ_CKDIV4_EN               ));
    mcSHOW_DBG_MSG6(("CA_CKDIV4_EN               = %1d\n"    ,tr->CA_CKDIV4_EN               ));
    mcSHOW_DBG_MSG6(("CA_PREDIV_EN               = %1d\n"    ,tr->CA_PREDIV_EN               ));
    mcSHOW_DBG_MSG6(("PH8_DLY                    = %1d\n"    ,tr->PH8_DLY                    ));
    mcSHOW_DBG_MSG6(("SEMI_OPEN_CA_PICK_MCK_RATIO= %1d\n"    ,tr->SEMI_OPEN_CA_PICK_MCK_RATIO));
    mcSHOW_DBG_MSG6(("DQ_AAMCK_DIV               = %1d\n"    ,tr->DQ_AAMCK_DIV               ));
    mcSHOW_DBG_MSG6(("CA_AAMCK_DIV               = %1d\n"    ,tr->CA_AAMCK_DIV               ));
    mcSHOW_DBG_MSG6(("CA_ADMCK_DIV               = %1d\n"    ,tr->CA_ADMCK_DIV               ));
    mcSHOW_DBG_MSG6(("DQ_TRACK_CA_EN             = %1d\n"    ,tr->DQ_TRACK_CA_EN             ));
    mcSHOW_DBG_MSG6(("CA_PICK                    = %2d\n"    ,CA_PICK                        ));
    mcSHOW_DBG_MSG6(("CA_MCKIO                   = %1d\n"    ,CA_MCKIO                       ));
    mcSHOW_DBG_MSG6(("MCKIO_SEMI                 = %1d\n"    ,MCKIO_SEMI                     ));
    mcSHOW_DBG_MSG6(("PLL_FREQ                   = %1d\n"    ,tr->PLL_FREQ                   ));
    mcSHOW_DBG_MSG6(("DQ_UI_PI_RATIO             = %1d\n"    ,tr->DQ_UI_PI_RATIO             ));
    mcSHOW_DBG_MSG6(("CA_UI_PI_RATIO             = %1d\n"    ,tr->CA_UI_PI_RATIO             ));
    mcSHOW_DBG_MSG6(("=================================== \n"));
}

void DRAMC_SUBSYS_PRE_CONFIG(DRAMC_CTX_T *p, DRAMC_SUBSYS_CONFIG_T *tr)
{
   U8 gp_id;
   tr->SRAM_EN               = 1;
   tr->MD32_EN               = 1;
   tr->a_cfg                 = &ana_top_p;
   tr->a_opt                 = &ANA_option;
   tr->lp4_init              = &LP4_INIT;
   tr->lp5_init              = &LP5_INIT;

   for(gp_id = 0; gp_id < DFS_GROUP_NUM; gp_id++)
   {
       tr->DFS_GP[gp_id]  = &DFS_TOP[gp_id];
   }

   if(LPDDR4_EN_S)
   {
        (tr->DFS_GP[0])->data_rate = 4266; (tr->DFS_GP[0])->DQ_P2S_RATIO = 8;
        (tr->DFS_GP[1])->data_rate = 3200; (tr->DFS_GP[1])->DQ_P2S_RATIO = 8;
        (tr->DFS_GP[2])->data_rate = 2400; (tr->DFS_GP[2])->DQ_P2S_RATIO = 8;
        (tr->DFS_GP[3])->data_rate = 1866; (tr->DFS_GP[3])->DQ_P2S_RATIO = 8;
        (tr->DFS_GP[4])->data_rate = 1600; (tr->DFS_GP[4])->DQ_P2S_RATIO = 4;
        (tr->DFS_GP[5])->data_rate = 1200; (tr->DFS_GP[5])->DQ_P2S_RATIO = 4;
        (tr->DFS_GP[6])->data_rate = 800 ; (tr->DFS_GP[6])->DQ_P2S_RATIO = 4;
        (tr->DFS_GP[7])->data_rate = 400 ; (tr->DFS_GP[7])->DQ_P2S_RATIO = 4;
        (tr->DFS_GP[8])->data_rate = 4266; (tr->DFS_GP[8])->DQ_P2S_RATIO = 4;
        (tr->DFS_GP[9])->data_rate = 1600; (tr->DFS_GP[9])->DQ_P2S_RATIO = 4;

       for(gp_id = 0; gp_id < DFS_GROUP_NUM; gp_id++)
       {
           (tr->DFS_GP[gp_id])->CKR = 1;
           (tr->DFS_GP[gp_id])->DQSIEN_MODE = 1;
       }
#if 0//DV_CONFIG_EN==1
       tr->lp4_init->LP4Y_EN         = DUT_p.LP4Y_EN       ;
       tr->lp4_init->WR_PST          = DUT_p.LP4_WR_PST    ;
       tr->lp4_init->OTF             = DUT_p.LP4_OTF       ;
       tr->a_cfg->NEW_8X_MODE        = DUT_p.NEW_8X_MODE   ;
       tr->a_cfg->LP45_APHY_COMB_EN  = 1                   ;
       tr->a_cfg->DLL_IDLE_MODE      = DUT_p.DLL_IDLE_MODE ;
       tr->a_cfg->NEW_RANK_MODE      = DUT_p.NEW_RANK_MODE ;
       tr->a_cfg->DLL_ASYNC_EN       = DUT_p.DLL_ASYNC_EN  ;
       tr->MD32_EN                   = DUT_p.MD32_EN       ;
       tr->SRAM_EN                   = DUT_p.SRAM_EN       ;
       tr->GP_NUM                    = DUT_p.GP_NUM        ;


       for(gp_id = 0; gp_id < DV_p.GP_NUM; gp_id++)
       {
           tr->DFS_GP[gp_id]->data_rate    = DUT_shu_p[gp_id].data_rate   ;
           tr->DFS_GP[gp_id]->DQSIEN_MODE  = DUT_shu_p[gp_id].DQSIEN_MODE ;
           tr->DFS_GP[gp_id]->DQ_P2S_RATIO = DUT_shu_p[gp_id].DQ_P2S_RATIO;
           tr->DFS_GP[gp_id]->CKR          = DUT_shu_p[gp_id].CKR         ;
       }
#endif
        #if SA_CONFIG_EN
        tr->lp4_init->EX_ROW_EN[0]    = p->u110GBEn[RANK_0] ;
        tr->lp4_init->EX_ROW_EN[1]    = p->u110GBEn[RANK_1] ;
        tr->lp4_init->BYTE_MODE[0]    = 0                   ;
        tr->lp4_init->BYTE_MODE[1]    = 0                   ;
        tr->lp4_init->LP4Y_EN         = 0;
        tr->lp4_init->WR_PST          = 1;
        tr->lp4_init->OTF             = 1;
        tr->a_cfg->NEW_8X_MODE        = 1;
        tr->a_cfg->LP45_APHY_COMB_EN  = 1                   ;
        tr->a_cfg->DLL_IDLE_MODE      = 1;
        tr->a_cfg->NEW_RANK_MODE      = 1;
        tr->a_cfg->DLL_ASYNC_EN       = 0;
        tr->MD32_EN                   = 0;
        tr->SRAM_EN                   = 1;
        tr->GP_NUM                    = 10;

        if(p->freq_sel==LP4_DDR4266)
        {
            (tr->DFS_GP[0])->data_rate = 4266; (tr->DFS_GP[0])->DQ_P2S_RATIO = 8;
        }
        else if(p->freq_sel==LP4_DDR3733)
        {
            (tr->DFS_GP[0])->data_rate = 3733; (tr->DFS_GP[0])->DQ_P2S_RATIO = 8;
        }
        else if(p->freq_sel==LP4_DDR3200)
        {
            (tr->DFS_GP[0])->data_rate = 3200; (tr->DFS_GP[0])->DQ_P2S_RATIO = 8;
        }
        else if(p->freq_sel==LP4_DDR2400)
        {
            (tr->DFS_GP[0])->data_rate = 2400; (tr->DFS_GP[0])->DQ_P2S_RATIO = 8;
        }
        else if(p->freq_sel==LP4_DDR1866)
        {
            (tr->DFS_GP[0])->data_rate = 1866; (tr->DFS_GP[0])->DQ_P2S_RATIO = 8;
        }
        else if(p->freq_sel==LP4_DDR1600)
        {
            (tr->DFS_GP[0])->data_rate = 1600; (tr->DFS_GP[0])->DQ_P2S_RATIO = 8;
        }
        else if(p->freq_sel==LP4_DDR1200)
        {
            (tr->DFS_GP[0])->data_rate = 1200; (tr->DFS_GP[0])->DQ_P2S_RATIO = 8;
        }
        else if(p->freq_sel==LP4_DDR800)
        {
            (tr->DFS_GP[0])->data_rate = 800; (tr->DFS_GP[0])->DQ_P2S_RATIO = 4;
        }
        else if(p->freq_sel==LP4_DDR400)
        {
            (tr->DFS_GP[0])->data_rate = 400; (tr->DFS_GP[0])->DQ_P2S_RATIO = 4;
        }
        #endif



       LP4_DRAM_config(tr->DFS_GP[0]->data_rate,tr->lp4_init);
   }



    ANA_TOP_FUNCTION_CFG(tr->a_cfg,tr->DFS_GP[0]->data_rate);
    ANA_CLK_DIV_config(tr->a_opt,tr->DFS_GP[0]);
    mcSHOW_DBG_MSG6(("=================================== \n"));
    mcSHOW_DBG_MSG6(("memory_type:%s         \n",LPDDR5_EN_S?"LPDDR5":"LPDDR4"     ));
    mcSHOW_DBG_MSG6(("GP_NUM     : %1d       \n",tr->GP_NUM      ));
    mcSHOW_DBG_MSG6(("SRAM_EN    : %1d       \n",tr->SRAM_EN     ));
    mcSHOW_DBG_MSG6(("MD32_EN    : %1d       \n",tr->MD32_EN     ));
    mcSHOW_DBG_MSG6(("=================================== \n"));
    #if DUMP_ALLSUH_RG
    mcSHOW_DBG_MSG(("[DUMPLOG] %d DQ_MCK_UI_RATIO=%d, DQ_UI_PI_RATIO=%d, CA_UI_PI_RATIO=%d\n", p->frequency * 2, vGet_Div_Mode(p) == DIV8_MODE ? 8 : 4, tr->a_opt->DQ_UI_PI_RATIO, tr->a_opt->CA_UI_PI_RATIO));
    #endif
}
