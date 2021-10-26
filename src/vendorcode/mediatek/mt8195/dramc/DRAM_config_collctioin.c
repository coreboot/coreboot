/* SPDX-License-Identifier: BSD-3-Clause */

#include "dramc_dv_init.h"


void LP4_DRAM_config(U32 data_rate, LP4_DRAM_CONFIG_T *tr)
{
    tr->BYTE_MODE[0] = 0;
    tr->BYTE_MODE[1] = 0;
#if 0
    #if SA_CONFIG_EN
        tr->EX_ROW_EN[0] = 0;
        tr->EX_ROW_EN[1] = 0;
    #else
        tr->EX_ROW_EN[0] = 1;
        tr->EX_ROW_EN[1] = 0;
    #endif
#endif
    tr->MR_WL     = LP4_DRAM_INIT_RLWL_MRfield_config(data_rate);
    tr->MR_RL     = tr->MR_WL;
    tr->BL        = 2;
    tr->RPST      = 0;
    tr->RD_PRE    = 0;
    tr->WR_PRE    = 1;
    tr->WR_PST    = (data_rate>=2667)?1:0;
#if SA_CONFIG_EN
        tr->DBI_WR    = 0;
        tr->DBI_RD    = 0;
#else
        tr->DBI_WR    = (data_rate>=2667)?1:0;
        tr->DBI_RD    = (data_rate>=2667)?1:0;
#endif
    //	tr->DMI       = 1;
    tr->OTF       = 1;
#if (ENABLE_LP4Y_DFS && LP4Y_BACKUP_SOLUTION)
        tr->LP4Y_EN   = (data_rate>=1866)?0:1;
#else
        tr->LP4Y_EN   = 0;
#endif
    tr->WORK_FSP  = (data_rate>=2667)?1:0;

    mcSHOW_DBG_MSG2(("=================================== \n"));
    mcSHOW_DBG_MSG2(("LPDDR4 DRAM CONFIGURATION\n"     ));
    mcSHOW_DBG_MSG2(("=================================== \n"));
//    mcSHOW_DBG_MSG(("BYTE_MODE    = B%1b\n",tr->BYTE_MODE));
    mcSHOW_DBG_MSG2(("EX_ROW_EN[0]    = 0x%1x\n",tr->EX_ROW_EN[0]));
    mcSHOW_DBG_MSG2(("EX_ROW_EN[1]    = 0x%1x\n",tr->EX_ROW_EN[1]));
    mcSHOW_DBG_MSG2(("LP4Y_EN      = 0x%1x\n",tr->LP4Y_EN  ));
    mcSHOW_DBG_MSG2(("WORK_FSP     = 0x%1x\n",tr->WORK_FSP ));
    mcSHOW_DBG_MSG2(("WL           = 0x%1x\n",tr->MR_WL    ));
    mcSHOW_DBG_MSG2(("RL           = 0x%1x\n",tr->MR_RL    ));
    mcSHOW_DBG_MSG2(("BL           = 0x%1x\n",tr->BL       ));
    mcSHOW_DBG_MSG2(("RPST         = 0x%1x\n",tr->RPST     ));
    mcSHOW_DBG_MSG2(("RD_PRE       = 0x%1x\n",tr->RD_PRE   ));
    mcSHOW_DBG_MSG2(("WR_PRE       = 0x%1x\n",tr->WR_PRE   ));
    mcSHOW_DBG_MSG2(("WR_PST       = 0x%1x\n",tr->WR_PST   ));
    mcSHOW_DBG_MSG2(("DBI_WR       = 0x%1x\n",tr->DBI_WR   ));
    mcSHOW_DBG_MSG2(("DBI_RD       = 0x%1x\n",tr->DBI_RD   ));
//  mcSHOW_DBG_MSG(("DMI          = 0x%1x\n",tr->DMI      ));
    mcSHOW_DBG_MSG2(("OTF          = 0x%1x\n",tr->OTF      ));
    mcSHOW_DBG_MSG2(("=================================== \n"));
}

U8 LP4_DRAM_INIT_RLWL_MRfield_config(U32 data_rate)
{
    U8 MR2_RLWL;

    if      ((data_rate<=4266) && (data_rate > 3733)) {MR2_RLWL = 7 ;}
    else if ((data_rate<=3733) && (data_rate > 3200)) {MR2_RLWL = 6 ;}
    else if ((data_rate<=3200) && (data_rate > 2667)) {MR2_RLWL = 5 ;}
    else if ((data_rate<=2667) && (data_rate > 2400)) {MR2_RLWL = 4 ;}
    else if ((data_rate<=2400) && (data_rate > 1866)) {MR2_RLWL = 4 ;}
    else if ((data_rate<=1866) && (data_rate > 1600)) {MR2_RLWL = 3 ;}
    else if ((data_rate<=1600) && (data_rate > 1200)) {MR2_RLWL = 2 ;}
    else if ((data_rate<=1200) && (data_rate > 800 )) {MR2_RLWL = 2 ;}
    else if ((data_rate<=800 ) && (data_rate > 400 )) {MR2_RLWL = 1 ;}
    else if (data_rate<=400 )                         {MR2_RLWL = 0 ;}
    else {mcSHOW_ERR_MSG(("ERROR: Unexpected data_rate:%4d under LPDDR4 \n",data_rate));return -1;}

    mcSHOW_DBG_MSG(("[ModeRegister RLWL Config] data_rate:%4d-MR2_RLWL:%1x\n",data_rate,MR2_RLWL));

    return MR2_RLWL;
}

U32 Get_RL_by_MR_LP4(U8 BYTE_MODE_EN,U8 DBI_EN, U8 MR_RL_field_value)
{
    U32 RL=0;

    switch(MR_RL_field_value)
    {
        case 0: {RL = 6; break;}
        case 1: {RL = ((DBI_EN == 1) ? 12 : 10); break;}
        case 2: {RL = (BYTE_MODE_EN == 1) ? ((DBI_EN == 1) ? 18 : 16 ) : ((DBI_EN == 1) ? 16 : 14); break;}
        case 3: {RL = (BYTE_MODE_EN == 1) ? ((DBI_EN == 1) ? 24 : 22 ) : ((DBI_EN == 1) ? 22 : 20); break;}
        case 4: {RL = (BYTE_MODE_EN == 1) ? ((DBI_EN == 1) ? 30 : 28 ) : ((DBI_EN == 1) ? 28 : 24); break;}
        case 5: {RL = (BYTE_MODE_EN == 1) ? ((DBI_EN == 1) ? 36 : 32 ) : ((DBI_EN == 1) ? 32 : 28); break;}
        case 6: {RL = (BYTE_MODE_EN == 1) ? ((DBI_EN == 1) ? 40 : 36 ) : ((DBI_EN == 1) ? 36 : 32); break;}
        case 7: {RL = (BYTE_MODE_EN == 1) ? ((DBI_EN == 1) ? 44 : 40 ) : ((DBI_EN == 1) ? 40 : 36); break;}
        default:{mcSHOW_ERR_MSG(("ERROR: Unexpected MR_RL_field_value:%1x under LPDDR4 \n",MR_RL_field_value));}
    }

    mcSHOW_DBG_MSG(("[ReadLatency GET] BYTE_MODE_EN:%1d-DBI_EN:%1d-MR_RL_field_value:%1x-RL:%2d\n",BYTE_MODE_EN,DBI_EN,MR_RL_field_value,RL));

    return RL;
}

U32 Get_WL_by_MR_LP4(U8 Version, U8 MR_WL_field_value)
{
    U32 WL=0;

    switch(MR_WL_field_value)
    {
        case 0: {WL = 4; break;}
        case 1: {WL = ((Version == 0) ? 6  : 8  ); break;}
        case 2: {WL = ((Version == 0) ? 8  : 12 ); break;}
        case 3: {WL = ((Version == 0) ? 10 : 18 ); break;}
        case 4: {WL = ((Version == 0) ? 12 : 22 ); break;}
        case 5: {WL = ((Version == 0) ? 14 : 26 ); break;}
        case 6: {WL = ((Version == 0) ? 16 : 30 ); break;}
        case 7: {WL = ((Version == 0) ? 18 : 34 ); break;}
        default:{mcSHOW_ERR_MSG(("ERROR: Unexpected MR_WL_field_value:%1x under LPDDR4 \n",MR_WL_field_value));}
    }

    mcSHOW_DBG_MSG(("[WriteLatency GET] Version:%1d-MR_RL_field_value:%1x-WL:%2d\n",Version,MR_WL_field_value,WL));

    return WL;
}


