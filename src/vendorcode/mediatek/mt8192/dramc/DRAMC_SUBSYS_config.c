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
   // tr-> DLL_ASYNC_EN 	= 0   ; //from DV random
   // tr-> NEW_RANK_MODE	= 1   ; //from DV random
   // tr-> DLL_IDLE_MODE	= 1   ; //from DV random
   // tr-> LP45_APHY_COMB_EN= 1   ; //from DV define
   // tr-> NEW_8X_MODE		= 1   ;


	tr->ALL_SLAVE_EN	 = (data_rate <= 1866)?1:0;

	if(LPDDR5_EN_S == 1)
	{
		tr->TX_ODT_DIS = (data_rate <=3200) ? 1 : 0 ;
	} else {
		tr->TX_ODT_DIS = (data_rate <=2400) ? 1 : 0 ;
	}

	msg("=================================== \n");
	msg("ANA top config\n"				);
	msg("=================================== \n");
	msg("DLL_ASYNC_EN			 = %2d\n",tr->DLL_ASYNC_EN	   );
	msg("ALL_SLAVE_EN			 = %2d\n",tr->ALL_SLAVE_EN	   );
	msg("NEW_RANK_MODE			 = %2d\n",tr->NEW_RANK_MODE    );
	msg("DLL_IDLE_MODE			 = %2d\n",tr->DLL_IDLE_MODE    );
	msg("LP45_APHY_COMB_EN		 = %2d\n",tr->LP45_APHY_COMB_EN);
	msg("TX_ODT_DIS 			 = %2d\n",tr->TX_ODT_DIS	   );
	msg("NEW_8X_MODE			 = %2d\n",tr->NEW_8X_MODE	   );
	msg("=================================== \n");
}


void ANA_CLK_DIV_config( ANA_DVFS_CORE_T *tr,DRAMC_DVFS_GROUP_CONFIG_T *dfs)
{
	U32  SEMI_OPEN_FMIN = 300;
	U32  SEMI_OPEN_FMAX = 500;//lp4 600
	U32  PI_FMIN		= 600;
	U32  DQ_PICK;
	U32  CA_PICK;	 //U
	U32  CA_MCKIO;	 //S
	U32  MCKIO_SEMI; //Q
	U16  data_rate;

	data_rate = dfs->data_rate;
	tr->DQ_P2S_RATIO = dfs->DQ_P2S_RATIO;
	tr->CKR = dfs->CKR;

	//tr->CA_P2S_RATIO
	tr->CA_P2S_RATIO = tr->DQ_P2S_RATIO/tr->CKR;

	//tr->DQ_CA_OPEN
	tr->DQ_CA_OPEN	 = ( data_rate < (SEMI_OPEN_FMIN * 2) ) ? 1 : 0;
	tr->DQ_SEMI_OPEN = ( data_rate/2 < PI_FMIN ) ? (1-tr->DQ_CA_OPEN) : ((data_rate <= SEMI_OPEN_FMAX*2) ? (1-tr->DQ_CA_OPEN) : 0);
	tr->CA_SEMI_OPEN = (( data_rate/(tr->CKR*2) < PI_FMIN ) ? ((data_rate/(tr->CKR*2) > SEMI_OPEN_FMAX) ? 0 : (((tr->CA_P2S_RATIO>2)||(tr->DQ_SEMI_OPEN))*(1-tr->DQ_CA_OPEN))) : tr->DQ_SEMI_OPEN);
	tr->CA_FULL_RATE = (tr->DQ_CA_OPEN == 1) ? ((tr->CKR>1)?1:0) : ((tr->DQ_SEMI_OPEN*tr->CA_SEMI_OPEN*(tr->CKR>>1)) + (( data_rate/(tr->CKR*2) < PI_FMIN) ? (1-tr->CA_SEMI_OPEN) : 0 ));
	tr->DQ_CKDIV4_EN = ( tr->DQ_SEMI_OPEN == 1) ? DONT_CARE_VALUE : ((( (data_rate/2) < 1200 ) ? 1 : 0 ) * (1-tr->DQ_CA_OPEN)) ;

	CA_MCKIO	 = (data_rate/(tr->CKR*2))*(1+tr->CA_FULL_RATE);
	DQ_PICK 	 = (tr->DQ_SEMI_OPEN == 1) ? 0 : (data_rate/2) ;
	CA_PICK 	 = (tr->CA_SEMI_OPEN == 1) ? CA_MCKIO*2 : ((CA_MCKIO>=PI_FMIN) ? CA_MCKIO : (( CA_MCKIO >= (PI_FMIN/2) ) ? CA_MCKIO*2 : CA_MCKIO *4 ));

	tr->CA_CKDIV4_EN = ((CA_PICK < 1200 ) ? 1 : 0 ) * ( 1- tr->DQ_CA_OPEN) ;

	tr->CA_PREDIV_EN = (data_rate >= 4800) ? 1 : 0 ;

	#if SA_CONFIG_EN
	if(LPDDR4_EN_S)
	{
		// @Darren, for LP4 8PH Delay
		if (data_rate <= 1866)
			tr->PH8_DLY = 0;
		else if (data_rate <= 2400)
			tr->PH8_DLY = 0x11;
		else if (data_rate <= 3200)
			tr->PH8_DLY = 0xc;
		else if (data_rate <= 3733)
			tr->PH8_DLY = 0x9;
		else
			tr->PH8_DLY = 0x7;
	}
	else
	#endif
	{
		tr->PH8_DLY = ((tr->DQ_CA_OPEN == 0) && (tr->DQ_SEMI_OPEN == 0) && (tr->DQ_CKDIV4_EN  == 0)) ? ( (1000000>>4)/data_rate -4) : DONT_CARE_VALUE;
	}

	MCKIO_SEMI = (tr->DQ_SEMI_OPEN * tr->CA_SEMI_OPEN * (data_rate/2)) + (1-tr->DQ_SEMI_OPEN) * tr->CA_SEMI_OPEN * CA_MCKIO;

	tr->SEMI_OPEN_CA_PICK_MCK_RATIO = ( MCKIO_SEMI == 0) ? DONT_CARE_VALUE : (CA_PICK*tr->DQ_P2S_RATIO)/data_rate ; //need to be improved

	tr->DQ_AAMCK_DIV = (tr->DQ_SEMI_OPEN == 0) ? ((tr->DQ_P2S_RATIO/2)*(1-tr->DQ_SEMI_OPEN)) : DONT_CARE_VALUE;
	tr->CA_AAMCK_DIV = (tr->CA_SEMI_OPEN == 0) ? ((tr->DQ_P2S_RATIO/(2*tr->CKR))*(1+tr->CA_FULL_RATE)) : DONT_CARE_VALUE;
	tr->CA_ADMCK_DIV = CA_PICK/(data_rate/tr->DQ_P2S_RATIO); //need to be improved
	//tr->DQ_TRACK_CA_EN = ((data_rate/2) >= 2133) ? 1 : 0 ; //for Alucary confirm that 'interface timing' sign NOT OK.
	tr->DQ_TRACK_CA_EN = 0 ;
	tr->PLL_FREQ = ((DQ_PICK*2*(tr->DQ_CKDIV4_EN+1)) > (CA_PICK*2*(tr->CA_CKDIV4_EN+1))) ? (DQ_PICK*2*(tr->DQ_CKDIV4_EN+1)) : (CA_PICK*2*(tr->CA_CKDIV4_EN+1));
	#if SA_CONFIG_EN
	//de-sense
	if(data_rate==2400)
		tr->PLL_FREQ =	2366;	//DDR2366
	else if(data_rate==1200)
		tr->PLL_FREQ =	2288;	//DDR1144
	else if(data_rate==3200 || data_rate==1600)
		tr->PLL_FREQ =	3068;	//DDR3068 DDR1534
	else if(data_rate==800)
		tr->PLL_FREQ =	3016;	//DDR754
	else if(data_rate==400)
		tr->PLL_FREQ =	4000;	//DDR250 Op
	#endif
	tr->DQ_UI_PI_RATIO = 32; //TODO:notice here. infact if DQ_SEMI_OPEM == 1 UI_PI_RATIO will only 4 lower 2bit wihtout use
	tr->CA_UI_PI_RATIO = (tr->CA_SEMI_OPEN == 0) ? ((tr->CA_FULL_RATE == 1)? 64 : DONT_CARE_VALUE) : 32;

	msg("=================================== \n");
	msg("data_rate					= %4d\n"	,data_rate						);
	msg("CKR						= %1d\n"	,tr->CKR						);
	msg("DQ_P2S_RATIO				= %1d\n"	,tr->DQ_P2S_RATIO				);
	msg("=================================== \n");
	msg("CA_P2S_RATIO				= %1d\n"	,tr->CA_P2S_RATIO				);
	msg("DQ_CA_OPEN 				= %1d\n"	,tr->DQ_CA_OPEN 				);
	msg("DQ_SEMI_OPEN				= %1d\n"	,tr->DQ_SEMI_OPEN				);
	msg("CA_SEMI_OPEN				= %1d\n"	,tr->CA_SEMI_OPEN				);
	msg("CA_FULL_RATE				= %1d\n"	,tr->CA_FULL_RATE				);
	msg("DQ_CKDIV4_EN				= %1d\n"	,tr->DQ_CKDIV4_EN				);
	msg("CA_CKDIV4_EN				= %1d\n"	,tr->CA_CKDIV4_EN				);
	msg("CA_PREDIV_EN				= %1d\n"	,tr->CA_PREDIV_EN				);
	msg("PH8_DLY					= %1d\n"	,tr->PH8_DLY					);
	msg("SEMI_OPEN_CA_PICK_MCK_RATIO= %1d\n"	,tr->SEMI_OPEN_CA_PICK_MCK_RATIO);
	msg("DQ_AAMCK_DIV				= %1d\n"	,tr->DQ_AAMCK_DIV				);
	msg("CA_AAMCK_DIV				= %1d\n"	,tr->CA_AAMCK_DIV				);
	msg("CA_ADMCK_DIV				= %1d\n"	,tr->CA_ADMCK_DIV				);
	msg("DQ_TRACK_CA_EN 			= %1d\n"	,tr->DQ_TRACK_CA_EN 			);
	msg("CA_PICK					= %2d\n"	,CA_PICK						);
	msg("CA_MCKIO					= %1d\n"	,CA_MCKIO						);
	msg("MCKIO_SEMI 				= %1d\n"	,MCKIO_SEMI 					);
	msg("PLL_FREQ					= %1d\n"	,tr->PLL_FREQ					);
	msg("DQ_UI_PI_RATIO 			= %1d\n"	,tr->DQ_UI_PI_RATIO 			);
	msg("CA_UI_PI_RATIO 			= %1d\n"	,tr->CA_UI_PI_RATIO 			);
	msg("=================================== \n");
}

void DRAMC_SUBSYS_PRE_CONFIG(DRAMC_CTX_T *p, DRAMC_SUBSYS_CONFIG_T *tr)
{
   U8 gp_id;
   tr->SRAM_EN				 = 1;
   tr->MD32_EN				 = 1;
   tr->a_cfg				 = &ana_top_p;
   tr->a_opt				 = &ANA_option;
   tr->lp4_init 			 = &LP4_INIT;
   tr->lp5_init 			 = &LP5_INIT;

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
	   tr->lp4_init->LP4Y_EN		 = DUT_p.LP4Y_EN	   ;
	   tr->lp4_init->WR_PST 		 = DUT_p.LP4_WR_PST    ;
	   tr->lp4_init->OTF			 = DUT_p.LP4_OTF	   ;
	   tr->a_cfg->NEW_8X_MODE		 = DUT_p.NEW_8X_MODE   ;
	   tr->a_cfg->LP45_APHY_COMB_EN  = 1				   ;
	   tr->a_cfg->DLL_IDLE_MODE 	 = DUT_p.DLL_IDLE_MODE ;
	   tr->a_cfg->NEW_RANK_MODE 	 = DUT_p.NEW_RANK_MODE ;
	   tr->a_cfg->DLL_ASYNC_EN		 = DUT_p.DLL_ASYNC_EN  ;
	   tr->MD32_EN					 = DUT_p.MD32_EN	   ;
	   tr->SRAM_EN					 = DUT_p.SRAM_EN	   ;
	   tr->GP_NUM					 = DUT_p.GP_NUM 	   ;


	   for(gp_id = 0; gp_id < DV_p.GP_NUM; gp_id++)
	   {
		   tr->DFS_GP[gp_id]->data_rate    = DUT_shu_p[gp_id].data_rate   ;
		   tr->DFS_GP[gp_id]->DQSIEN_MODE  = DUT_shu_p[gp_id].DQSIEN_MODE ;
		   tr->DFS_GP[gp_id]->DQ_P2S_RATIO = DUT_shu_p[gp_id].DQ_P2S_RATIO;
		   tr->DFS_GP[gp_id]->CKR		   = DUT_shu_p[gp_id].CKR		  ;
	   }
#endif
		#if SA_CONFIG_EN
		tr->lp4_init->EX_ROW_EN[0]	  = p->u110GBEn[RANK_0]	;
		tr->lp4_init->EX_ROW_EN[1]	  = p->u110GBEn[RANK_1]	;
		tr->lp4_init->BYTE_MODE[0]	  = 0					;
		tr->lp4_init->BYTE_MODE[1]	  = 0					;
		tr->lp4_init->LP4Y_EN		  = 0;//DUT_p.LP4Y_EN		;
		tr->lp4_init->WR_PST		  = 1;//DUT_p.LP4_WR_PST	;
		tr->lp4_init->OTF			  = 1;//DUT_p.LP4_OTF		;
		tr->a_cfg->NEW_8X_MODE		  = 1;//DUT_p.NEW_8X_MODE	;
		tr->a_cfg->LP45_APHY_COMB_EN  = 1					;
		tr->a_cfg->DLL_IDLE_MODE	  = 1;//DUT_p.DLL_IDLE_MODE ;
		tr->a_cfg->NEW_RANK_MODE	  = 1;//DUT_p.NEW_RANK_MODE ;
		tr->a_cfg->DLL_ASYNC_EN 	  = 0;//DUT_p.DLL_ASYNC_EN	;
		tr->MD32_EN 				  = 0;//DUT_p.MD32_EN		;
		tr->SRAM_EN 				  = 1;//DUT_p.SRAM_EN		;
		tr->GP_NUM					  = 10;//DUT_p.GP_NUM		 ;

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

//==============================================
//Oterwise, SA should rebuild Top configuration.
//==============================================

	   LP4_DRAM_config(tr->DFS_GP[0]->data_rate,tr->lp4_init);
   }

   //TODO for LPDDR5
   //data_rate		  DQ_P2S_RATIO
   //[4800:6400]	  16
   //[1600:4800)	  8
   //[400 :1600]	  4
   //=========================
   //data_rate		  CKR
   //[3733:6400]	  4
   //[400 :3733)	  2
	else if (MEM_TYPE == LPDDR5)
	{
		#if SA_CONFIG_EN
			if(p->freq_sel==LP5_DDR4266)
			{
				(tr->DFS_GP[0])->data_rate = 4266; (tr->DFS_GP[0])->DQ_P2S_RATIO = 8 ; tr->DFS_GP[0]->CKR = 4;tr->DFS_GP[0]->DQSIEN_MODE = 2;
			}
			else if(p->freq_sel==LP5_DDR5500)
			{
				(tr->DFS_GP[0])->data_rate = 5500; (tr->DFS_GP[0])->DQ_P2S_RATIO = 16 ; tr->DFS_GP[0]->CKR = 4;tr->DFS_GP[0]->DQSIEN_MODE = 2;
			}
			else
			{
				(tr->DFS_GP[0])->data_rate = 3200; (tr->DFS_GP[0])->DQ_P2S_RATIO = 8 ; tr->DFS_GP[0]->CKR = 2;tr->DFS_GP[0]->DQSIEN_MODE = 1;
			}
		#else
		(tr->DFS_GP[0])->data_rate = 6400; (tr->DFS_GP[0])->DQ_P2S_RATIO = 16; tr->DFS_GP[0]->CKR = 4;tr->DFS_GP[0]->DQSIEN_MODE = 1;
		#endif
		(tr->DFS_GP[1])->data_rate = 3200; (tr->DFS_GP[1])->DQ_P2S_RATIO = 8 ; tr->DFS_GP[1]->CKR = 2;tr->DFS_GP[1]->DQSIEN_MODE = 1;
		(tr->DFS_GP[2])->data_rate = 1600; (tr->DFS_GP[2])->DQ_P2S_RATIO = 4 ; tr->DFS_GP[2]->CKR = 2;tr->DFS_GP[2]->DQSIEN_MODE = 1;
		(tr->DFS_GP[3])->data_rate = 4266; (tr->DFS_GP[3])->DQ_P2S_RATIO = 8 ; tr->DFS_GP[3]->CKR = 4;tr->DFS_GP[3]->DQSIEN_MODE = 1;
		(tr->DFS_GP[4])->data_rate = 3733; (tr->DFS_GP[4])->DQ_P2S_RATIO = 8 ; tr->DFS_GP[4]->CKR = 4;tr->DFS_GP[4]->DQSIEN_MODE = 1;
		(tr->DFS_GP[5])->data_rate = 1600; (tr->DFS_GP[5])->DQ_P2S_RATIO = 8 ; tr->DFS_GP[5]->CKR = 2;tr->DFS_GP[5]->DQSIEN_MODE = 1;
		(tr->DFS_GP[6])->data_rate = 1200; (tr->DFS_GP[6])->DQ_P2S_RATIO = 4 ; tr->DFS_GP[6]->CKR = 2;tr->DFS_GP[6]->DQSIEN_MODE = 1;
		(tr->DFS_GP[7])->data_rate = 800 ; (tr->DFS_GP[7])->DQ_P2S_RATIO = 4 ; tr->DFS_GP[7]->CKR = 2;tr->DFS_GP[7]->DQSIEN_MODE = 1;
		(tr->DFS_GP[8])->data_rate = 400 ; (tr->DFS_GP[8])->DQ_P2S_RATIO = 4 ; tr->DFS_GP[8]->CKR = 2;tr->DFS_GP[8]->DQSIEN_MODE = 1;
		(tr->DFS_GP[9])->data_rate = 5500; (tr->DFS_GP[9])->DQ_P2S_RATIO = 16; tr->DFS_GP[9]->CKR = 4;tr->DFS_GP[9]->DQSIEN_MODE = 1;
		LP5_DRAM_config(tr->DFS_GP[0],tr->lp5_init);
	}


	ANA_TOP_FUNCTION_CFG(tr->a_cfg,tr->DFS_GP[0]->data_rate);
	ANA_CLK_DIV_config(tr->a_opt,tr->DFS_GP[0]);

	msg("=================================== \n");
	msg("memory_type:%s 		\n",LPDDR5_EN_S?"LPDDR5":"LPDDR4"	  );
	msg("GP_NUM 	: %1d		\n",tr->GP_NUM		);
	msg("SRAM_EN	: %1d		\n",tr->SRAM_EN 	);
	msg("MD32_EN	: %1d		\n",tr->MD32_EN 	);
	msg("=================================== \n");
}
