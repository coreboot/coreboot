/* SPDX-License-Identifier: BSD-3-Clause */

#include "dramc_dv_init.h"

//DRAM LP4 initial configuration
void LP4_DRAM_config(U32 data_rate, LP4_DRAM_CONFIG_T *tr)
{
	tr->BYTE_MODE[0] = 0;//TODO
	tr->BYTE_MODE[1] = 0;//TODO
#if 0 // @Darren, remove it
	#if SA_CONFIG_EN
		tr->EX_ROW_EN[0] = 0;//TODO
		tr->EX_ROW_EN[1] = 0;//TODO
	#else
		tr->EX_ROW_EN[0] = 1;//TODO
		tr->EX_ROW_EN[1] = 0;//TODO
	#endif
#endif
	tr->MR_WL	  = LP4_DRAM_INIT_RLWL_MRfield_config(data_rate);
	tr->MR_RL	  = tr->MR_WL;
	tr->BL		  = 2;
	tr->RPST	  = 0;
	tr->RD_PRE	  = 0;
	tr->WR_PRE	  = 1;
	tr->WR_PST	  = (data_rate>=2667)?1:0; //TODO
	#if SA_CONFIG_EN
		tr->DBI_WR	  = 0;
		tr->DBI_RD	  = 0;
	#else
		tr->DBI_WR	  = (data_rate>=2667)?1:0;
		tr->DBI_RD	  = (data_rate>=2667)?1:0;
	#endif
	//	tr->DMI 	  = 1;
	tr->OTF 	  = 1;
	#if ENABLE_LP4Y_DFS
		tr->LP4Y_EN   = (data_rate>=1866)?0:1; //TODO, @Darren for LP4Y
	#else
		tr->LP4Y_EN   = 0;
	#endif
	tr->WORK_FSP  = (data_rate>=2667)?1:0;

	mcSHOW_DBG_MSG(("=================================== \n"));
	mcSHOW_DBG_MSG(("LPDDR4 DRAM CONFIGURATION\n"	  ));
	mcSHOW_DBG_MSG(("=================================== \n"));
//	  mcSHOW_DBG_MSG(("BYTE_MODE	= B%1b\n",tr->BYTE_MODE));
	mcSHOW_DBG_MSG(("EX_ROW_EN[0]	 = 0x%1x\n",tr->EX_ROW_EN[0]));
	mcSHOW_DBG_MSG(("EX_ROW_EN[1]	 = 0x%1x\n",tr->EX_ROW_EN[1]));
	mcSHOW_DBG_MSG(("LP4Y_EN	  = 0x%1x\n",tr->LP4Y_EN  ));
	mcSHOW_DBG_MSG(("WORK_FSP	  = 0x%1x\n",tr->WORK_FSP ));
	mcSHOW_DBG_MSG(("WL 		  = 0x%1x\n",tr->MR_WL	  ));
	mcSHOW_DBG_MSG(("RL 		  = 0x%1x\n",tr->MR_RL	  ));
	mcSHOW_DBG_MSG(("BL 		  = 0x%1x\n",tr->BL 	  ));
	mcSHOW_DBG_MSG(("RPST		  = 0x%1x\n",tr->RPST	  ));
	mcSHOW_DBG_MSG(("RD_PRE 	  = 0x%1x\n",tr->RD_PRE   ));
	mcSHOW_DBG_MSG(("WR_PRE 	  = 0x%1x\n",tr->WR_PRE   ));
	mcSHOW_DBG_MSG(("WR_PST 	  = 0x%1x\n",tr->WR_PST   ));
	mcSHOW_DBG_MSG(("DBI_WR 	  = 0x%1x\n",tr->DBI_WR   ));
	mcSHOW_DBG_MSG(("DBI_RD 	  = 0x%1x\n",tr->DBI_RD   ));
//	mcSHOW_DBG_MSG(("DMI		  = 0x%1x\n",tr->DMI	  ));
	mcSHOW_DBG_MSG(("OTF		  = 0x%1x\n",tr->OTF	  ));
	mcSHOW_DBG_MSG(("=================================== \n"));
}
//LP4 dram initial ModeRegister setting
U8 LP4_DRAM_INIT_RLWL_MRfield_config(U32 data_rate)
{
	U8 MR2_RLWL;

	if		((data_rate<=4266) && (data_rate > 3733)) {MR2_RLWL = 7 ;}
	else if ((data_rate<=3733) && (data_rate > 3200)) {MR2_RLWL = 6 ;}
	else if ((data_rate<=3200) && (data_rate > 2667)) {MR2_RLWL = 5 ;}
	else if ((data_rate<=2667) && (data_rate > 2400)) {MR2_RLWL = 4 ;}
	else if ((data_rate<=2400) && (data_rate > 1866)) {MR2_RLWL = 4 ;}
	else if ((data_rate<=1866) && (data_rate > 1600)) {MR2_RLWL = 3 ;}
	else if ((data_rate<=1600) && (data_rate > 1200)) {MR2_RLWL = 2 ;}
	else if ((data_rate<=1200) && (data_rate > 800 )) {MR2_RLWL = 2 ;}
	else if ((data_rate<=800 ) && (data_rate > 400 )) {MR2_RLWL = 2 ;}
	else if (data_rate<=400 )						  {MR2_RLWL = 0 ;}
	else {mcSHOW_DBG_MSG(("ERROR: Unexpected data_rate:%4d under LPDDR4 \n",data_rate));return -1;}

	mcSHOW_DBG_MSG(("[ModeRegister RLWL Config] data_rate:%4d-MR2_RLWL:%1x\n",data_rate,MR2_RLWL));

	return MR2_RLWL;
}

U32 Get_RL_by_MR_LP4(U8 BYTE_MODE_EN,U8 DBI_EN, U8 MR_RL_field_value)
{
	U32 RL=0;

	switch(MR_RL_field_value)
	{
		case 0: {RL = 6; break;}
		case 1: {RL = (BYTE_MODE_EN == 1) ? ( 12 ) : ((DBI_EN == 1) ? 12 : 10); break;}
		case 2: {RL = (BYTE_MODE_EN == 1) ? ((DBI_EN == 1) ? 18 : 16 ) : ((DBI_EN == 1) ? 16 : 14); break;}
		case 3: {RL = (BYTE_MODE_EN == 1) ? ((DBI_EN == 1) ? 24 : 22 ) : ((DBI_EN == 1) ? 22 : 20); break;}
		case 4: {RL = (BYTE_MODE_EN == 1) ? ((DBI_EN == 1) ? 30 : 28 ) : ((DBI_EN == 1) ? 28 : 24); break;}
		case 5: {RL = (BYTE_MODE_EN == 1) ? ((DBI_EN == 1) ? 36 : 32 ) : ((DBI_EN == 1) ? 32 : 28); break;}
		case 6: {RL = (BYTE_MODE_EN == 1) ? ((DBI_EN == 1) ? 40 : 36 ) : ((DBI_EN == 1) ? 36 : 32); break;}
		case 7: {RL = (BYTE_MODE_EN == 1) ? ((DBI_EN == 1) ? 44 : 40 ) : ((DBI_EN == 1) ? 40 : 36); break;}
		default:{mcSHOW_DBG_MSG(("ERROR: Unexpected MR_RL_field_value:%1x under LPDDR4 \n",MR_RL_field_value));}
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
		case 1: {WL = ((Version == 0) ? 6  : 8	); break;}
		case 2: {WL = ((Version == 0) ? 8  : 12 ); break;}
		case 3: {WL = ((Version == 0) ? 10 : 18 ); break;}
		case 4: {WL = ((Version == 0) ? 12 : 22 ); break;}
		case 5: {WL = ((Version == 0) ? 14 : 26 ); break;}
		case 6: {WL = ((Version == 0) ? 16 : 30 ); break;}
		case 7: {WL = ((Version == 0) ? 18 : 34 ); break;}
		default:{mcSHOW_DBG_MSG(("ERROR: Unexpected MR_WL_field_value:%1x under LPDDR4 \n",MR_WL_field_value));}
	}

	mcSHOW_DBG_MSG(("[WriteLatency GET] Version:%1d-MR_RL_field_value:%1x-WL:%2d\n",Version,MR_WL_field_value,WL));

	return WL;
}

//LP5 dram initial ModeRegister setting
static U8 LP5_DRAM_INIT_RLWL_MRfield_config(U32 data_rate)
{
	U8 MR2_RLWL=0;

	if		((data_rate<=6400) && (data_rate > 6000)) {MR2_RLWL = 11 ;}
	else if ((data_rate<=6400) && (data_rate > 5500)) {MR2_RLWL = 10 ;}
	else if ((data_rate<=5500) && (data_rate > 4800)) {MR2_RLWL = 9  ;}
	else if ((data_rate<=4800) && (data_rate > 4266)) {MR2_RLWL = 8  ;}
	else if ((data_rate<=4266) && (data_rate > 3733)) {MR2_RLWL = 7  ;}
	else if ((data_rate<=3700) && (data_rate > 3200)) {MR2_RLWL = 6  ;}
	else if ((data_rate<=3200) && (data_rate > 2400)) {MR2_RLWL = 5  ;}
	else if ((data_rate<=2400) && (data_rate > 1866)) {MR2_RLWL = 4  ;}
	else if ((data_rate<=1866) && (data_rate > 1600)) {MR2_RLWL = 3  ;}
	else if ((data_rate<=1600) && (data_rate >= 800)) {MR2_RLWL = 2  ;}
	else {mcSHOW_DBG_MSG(("ERROR: Unexpected data_rate:%4d under LPDDR5 \n",data_rate));return -1;}

	mcSHOW_DBG_MSG(("[ModeRegister RLWL Config] data_rate:%4d-MR2_RLWL:%1x\n",data_rate,MR2_RLWL));

	return MR2_RLWL;
}

void LP5_DRAM_config(DRAMC_DVFS_GROUP_CONFIG_T *dfs_tr, LP5_DRAM_CONFIG_T *tr)
{
	tr->BYTE_MODE[0] = 0 ;
	tr->BYTE_MODE[1] = 0 ;
	tr->EX_ROW_EN[0] = 0 ;
	tr->EX_ROW_EN[1] = 0 ;
	tr->MR_WL		 = LP5_DRAM_INIT_RLWL_MRfield_config(dfs_tr->data_rate);
	tr->MR_RL		 = tr->MR_WL;
	tr->BL			 = 2;
	tr->CK_Mode 	 = (dfs_tr->data_rate>=2133)?0:1;	//0:diff 1:SE
	tr->RPST		 = 0;
	tr->RD_PRE		 = 0;
	tr->WR_PRE		 = 1;
	tr->WR_PST		 = (dfs_tr->data_rate>=3200)?1:0 ;
	#if SA_CONFIG_EN
		tr->DBI_WR		 = 0;
		#if LP5_DDR4266_RDBI_WORKAROUND
		tr->DBI_RD		 = (dfs_tr->data_rate>=3733)?1:0 ;
		#else
		tr->DBI_RD		 = 0;
		#endif
	#else
	tr->DBI_WR		 = (dfs_tr->data_rate>=3733)?1:0 ;
	tr->DBI_RD		 = (dfs_tr->data_rate>=3733)?1:0 ;
	#endif
	tr->DMI 		 = 1;
	tr->OTF 		 = 1;
	tr->WCK_PST 	 = (dfs_tr->data_rate>=3733)?1:0 ;
	tr->RDQS_PST	 = 0;
	tr->CA_ODT		 = 0;
	tr->DQ_ODT		 = (dfs_tr->data_rate>=3733)?3:0 ;
	tr->CKR 		 = (dfs_tr->CKR==4)?0:1;
	tr->WCK_ON		 =	0; //TODO
	#if SA_CONFIG_EN
		#if WCK_LEVELING_FM_WORKAROUND
		tr->WCK_FM		 = 0;
		#else
		tr->WCK_FM		 = (dfs_tr->data_rate>=2133)?1:0;
		#endif
	#else
	tr->WCK_FM		 = (dfs_tr->data_rate>=2133)?1:0;
	#endif
	tr->WCK_ODT 	 = (dfs_tr->CKR==4)?3:0;
	tr->DVFSQ		 = (dfs_tr->data_rate>=3733)?0:1;
	tr->DVFSC		 = (dfs_tr->data_rate>=2133)?0:1;
	tr->RDQSmode[0]  = EN_both;//TODO  --RK0 have to EN_t if SE enable
	tr->RDQSmode[1]  = EN_both;//TODO  --RK1 have to EN_c if SE enable
	tr->WCKmode[0]	 = (dfs_tr->data_rate>=1600)?0:1;
	tr->WCKmode[1]	 = (dfs_tr->data_rate>=1600)?0:2;
	tr->RECC		 = 0;//TODO
	tr->WECC		 = 0;//TODO
	tr->BankMode	 = (dfs_tr->data_rate>=3733)?BG4BK4:BK16;
	tr->WORK_FSP	 = 0;//TODO

	switch (dfs_tr->DQSIEN_MODE)
	{
		case 1: {tr->RDQS_PRE = 0;break;}
		case 2: {tr->RDQS_PRE = 1;break;}
		case 3: {tr->RDQS_PRE = 3;break;}
		case 6: {tr->RDQS_PRE = 1;break;}
		case 7: {tr->RDQS_PRE = 3;break;}
		default : {mcSHOW_DBG_MSG(("ERROR: Unexpected DQSIEN_MODE :%d \n",dfs_tr->DQSIEN_MODE)); while(1);};
	}

	mcSHOW_DBG_MSG(("=================================== \n"));
	mcSHOW_DBG_MSG(("LPDDR5 DRAM CONFIGURATION\n"	  ));
	mcSHOW_DBG_MSG(("=================================== \n"));
	mcSHOW_DBG_MSG(("MR_WL		   = 0x%1x\n",tr->MR_WL 	  ));
	mcSHOW_DBG_MSG(("MR_RL		   = 0x%1x\n",tr->MR_RL 	  ));
	mcSHOW_DBG_MSG(("BL 		   = 0x%1x\n",tr->BL		  ));
	mcSHOW_DBG_MSG(("CK_Mode	   = 0x%1x\n",tr->CK_Mode	  ));
	mcSHOW_DBG_MSG(("RPST		   = 0x%1x\n",tr->RPST		  ));
	mcSHOW_DBG_MSG(("RD_PRE 	   = 0x%1x\n",tr->RD_PRE	  ));
	mcSHOW_DBG_MSG(("RDQS_PRE	   = 0x%1x\n",tr->RDQS_PRE	  ));
	mcSHOW_DBG_MSG(("WR_PRE 	   = 0x%1x\n",tr->WR_PRE	  ));
	mcSHOW_DBG_MSG(("WR_PST 	   = 0x%1x\n",tr->WR_PST	  ));
	mcSHOW_DBG_MSG(("DBI_WR 	   = 0x%1x\n",tr->DBI_WR	  ));
	mcSHOW_DBG_MSG(("DBI_RD 	   = 0x%1x\n",tr->DBI_RD	  ));
	mcSHOW_DBG_MSG(("DMI		   = 0x%1x\n",tr->DMI		  ));
	mcSHOW_DBG_MSG(("OTF		   = 0x%1x\n",tr->OTF		  ));
	mcSHOW_DBG_MSG(("WCK_PST	   = 0x%1x\n",tr->WCK_PST	  ));
	mcSHOW_DBG_MSG(("RDQS_PST	   = 0x%1x\n",tr->RDQS_PST	  ));
	mcSHOW_DBG_MSG(("CA_ODT 	   = 0x%1x\n",tr->CA_ODT	  ));
	mcSHOW_DBG_MSG(("DQ_ODT 	   = 0x%1x\n",tr->DQ_ODT	  ));
	mcSHOW_DBG_MSG(("CKR		   = 0x%1x\n",tr->CKR		  ));
	mcSHOW_DBG_MSG(("WCK_ON 	   = 0x%1x\n",tr->WCK_ON	  ));
	mcSHOW_DBG_MSG(("WCK_FM 	   = 0x%1x\n",tr->WCK_FM	  ));
	mcSHOW_DBG_MSG(("WCK_ODT	   = 0x%1x\n",tr->WCK_ODT	  ));
	mcSHOW_DBG_MSG(("DVFSQ		   = 0x%1x\n",tr->DVFSQ 	  ));
	mcSHOW_DBG_MSG(("DVFSC		   = 0x%1x\n",tr->DVFSC 	  ));
	mcSHOW_DBG_MSG(("RDQSmode[0]   = 0x%1x\n",tr->RDQSmode[0] ));
	mcSHOW_DBG_MSG(("RDQSmode[1]   = 0x%1x\n",tr->RDQSmode[1] ));
	mcSHOW_DBG_MSG(("WCKmode[0]    = 0x%1x\n",tr->WCKmode[0]  ));
	mcSHOW_DBG_MSG(("WCKmode[1]    = 0x%1x\n",tr->WCKmode[1]  ));
	mcSHOW_DBG_MSG(("RECC		   = 0x%1x\n",tr->RECC		  ));
	mcSHOW_DBG_MSG(("WECC		   = 0x%1x\n",tr->WECC		  ));
	mcSHOW_DBG_MSG(("BankMode	   = 0x%1x\n",tr->BankMode	  ));
	mcSHOW_DBG_MSG(("WORK_FSP	   = 0x%1x\n",tr->WORK_FSP	  ));
	mcSHOW_DBG_MSG(("=================================== \n"));
}
