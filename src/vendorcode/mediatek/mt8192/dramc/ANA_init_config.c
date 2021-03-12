/* SPDX-License-Identifier: BSD-3-Clause */

#include "dramc_dv_init.h"

//==========================
//PLL config
//==========================
static void ANA_PLL_shuffle_Config(DRAMC_CTX_T *p,U32 PLL_FREQ,U16 data_rate)
{
	U32 XTAL_FREQ = 26;
	U8	PREDIV	  = 1;	//0/1/2
	U8	POSDIV	  = 0;	//0/1/2
	U8	FBKSEL	  = 0;	//over 3800 1 otherwise 0
	U32 PCW;
	U8 DIV16_CK_SEL    = 0;

#if EMI_LPBK_USE_DDR_800 // For Pe_trus DDR1600, sedalpbk DDR800 thru io
	if(p->frequency==800)
	{
		POSDIV = 1;
	}
#endif

#if (fcFOR_CHIP_ID == fcMargaux)
	if(A_D->DQ_CA_OPEN == 1)
	{
		DIV16_CK_SEL = 1; // For open loop mode DDR250 = 4G/div16, confirm with WL Lee
	}
	else
#endif
	FBKSEL = (PLL_FREQ > 3800)?1:0;
	PCW    = (PLL_FREQ/XTAL_FREQ) << (8+1-FBKSEL-PREDIV-POSDIV);


	msg(">>>>>> [CONFIGURE PHASE][SHUFFLE]: PLL\n");
	msg("=================================== \n");
	msg("data_rate = %d,PCW = 0X%x\n",data_rate,PCW);
	msg("=================================== \n");

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_PHYPLL1), P_Fld(0, PHYPLL1_RG_RPHYPLL_TST_EN) | P_Fld(0, PHYPLL1_RG_RPHYPLL_TSTOP_EN));
	// @Darren, Mp settings sync @WL
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_PHYPLL0)	  , P_Fld(0 	 , SHU_PHYPLL0_RG_RPHYPLL_RESERVED			 ) \
																  | P_Fld(0 	 , SHU_PHYPLL0_RG_RPHYPLL_ICHP			 ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CLRPLL0)	  , P_Fld(0 	 , SHU_CLRPLL0_RG_RCLRPLL_RESERVED			 ) \
																  | P_Fld(0 	 , SHU_CLRPLL0_RG_RCLRPLL_ICHP			 ));


	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_PHYPLL2)	  , P_Fld(PREDIV	  , SHU_PHYPLL2_RG_RPHYPLL_PREDIV			) \
																  | P_Fld(POSDIV	  , SHU_PHYPLL2_RG_RPHYPLL_POSDIV			));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CLRPLL2)	  , P_Fld(PREDIV	  , SHU_CLRPLL2_RG_RCLRPLL_PREDIV			) \
																  | P_Fld(POSDIV	  , SHU_CLRPLL2_RG_RCLRPLL_POSDIV			));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_PHYPLL1)	  , P_Fld(PCW		  , SHU_PHYPLL1_RG_RPHYPLL_SDM_PCW			) \
																  | P_Fld(1 		  , SHU_PHYPLL1_RG_RPHYPLL_SDM_PCW_CHG		) \
																  | P_Fld(0 		  , SHU_PHYPLL1_RG_RPHYPLL_SDM_FRA_EN		)); //for DV could set 1 to solve clock jitter issue.
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CLRPLL1)	  , P_Fld(PCW		  , SHU_CLRPLL1_RG_RCLRPLL_SDM_PCW			) \
																  | P_Fld(1 		  , SHU_CLRPLL1_RG_RCLRPLL_SDM_PCW_CHG		) \
																  | P_Fld(0 		  , SHU_CLRPLL1_RG_RCLRPLL_SDM_FRA_EN		)); //for DV could set 1 to solve clock jitter issue.
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_PLL1)		  , P_Fld(1 		  , SHU_PLL1_RG_RPHYPLLGP_CK_SEL			) \
																  | P_Fld(1 		  , SHU_PLL1_R_SHU_AUTO_PLL_MUX 			));  //notice here. TODO. should create another function to manage the SPM related
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_PHYPLL3)	  , P_Fld(0 		  , SHU_PHYPLL3_RG_RPHYPLL_LVROD_EN 		) \
																  | P_Fld(1 		  , SHU_PHYPLL3_RG_RPHYPLL_RST_DLY			) \
																  | P_Fld(FBKSEL	  , SHU_PHYPLL3_RG_RPHYPLL_FBKSEL			));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CLRPLL3)	  , P_Fld(0 		  , SHU_CLRPLL3_RG_RCLRPLL_LVROD_EN 		) \
																  | P_Fld(1 		  , SHU_CLRPLL3_RG_RCLRPLL_RST_DLY			) \
																  | P_Fld(FBKSEL	  , SHU_CLRPLL3_RG_RCLRPLL_FBKSEL			));
	if(A_D->DQ_CA_OPEN == 1)
	{
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_MISC_CLK_CTRL0), P_Fld( A_D->DQ_CA_OPEN   , SHU_MISC_CLK_CTRL0_M_CK_OPENLOOP_MODE_SEL	 ));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_PHYPLL3) , P_Fld( 1		  , SHU_PHYPLL3_RG_RPHYPLL_MONCK_EN 		) \
																  | P_Fld( DIV16_CK_SEL 		 , SHU_PHYPLL3_RG_RPHYPLL_DIV_CK_SEL	   )); //@Darren, DDR250 = 4G/div16, confirm with WL Lee
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CLRPLL3) , P_Fld( 1		  , SHU_CLRPLL3_RG_RCLRPLL_MONCK_EN 		) \
																  | P_Fld( DIV16_CK_SEL 		 , SHU_CLRPLL3_RG_RCLRPLL_DIV_CK_SEL	   )); //@Darren, DDR250 = 4G/div16, confirm with WL Lee
	}
//	  vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_PLL2)		, P_Fld(1			, SHU_PLL2_RG_RPHYPLL_ADA_MCK8X_EN_SHU	  ));
	msg("<<<<<< [CONFIGURE PHASE][SHUFFLE]: PLL\n");
}


static void ANA_CLK_DIV_config_setting(DRAMC_CTX_T *p,ANA_DVFS_CORE_T *tr,ANA_top_config_T *a_cfg)
{
	U8 TX_ARDQ_SERMODE=0; //DQ_P2S_RATIO
	U8 TX_ARCA_SERMODE=0; //CA_P2S_RATIO
	U8 ARDLL_SERMODE_B=0;
	U8 ARDLL_SERMODE_C=0;

	msg(">>>>>> [CONFIGURE PHASE][SHUFFLE]: ANA CLOCK DIV configuration\n");
	switch (tr->DQ_P2S_RATIO)
	{
		case 4 : { TX_ARDQ_SERMODE = 1; break; }
		case 8 : { TX_ARDQ_SERMODE = 2; break; }
		case 16: { TX_ARDQ_SERMODE = 3; break; }
		default: msg("ERROR: tr->DQ_P2S_RATIO= %2d, Not support!!",tr->DQ_P2S_RATIO);
	}

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ14), P_Fld(  TX_ARDQ_SERMODE	  , SHU_B0_DQ14_RG_TX_ARDQ_SER_MODE_B0		  ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ14), P_Fld(  TX_ARDQ_SERMODE	  , SHU_B1_DQ14_RG_TX_ARDQ_SER_MODE_B1		  ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ6) , P_Fld(  TX_ARDQ_SERMODE	  , SHU_B0_DQ6_RG_RX_ARDQ_RANK_SEL_SER_MODE_B0));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ6) , P_Fld(  TX_ARDQ_SERMODE	  , SHU_B1_DQ6_RG_RX_ARDQ_RANK_SEL_SER_MODE_B1));

	//Justin confirm that DES_MODE -> Deserializer mode, while DQ_P2S_RATIO=16 setting 3 others 2. in fact ANA could support some other mode, Here is an propsal option
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD11), P_Fld( (tr->DQ_P2S_RATIO == 16 ) ? 3 : 2 , SHU_CA_CMD11_RG_RX_ARCA_DES_MODE_CA));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ11) , P_Fld( (tr->DQ_P2S_RATIO == 16 ) ? 3 : 2 , SHU_B0_DQ11_RG_RX_ARDQ_DES_MODE_B0));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ11) , P_Fld( (tr->DQ_P2S_RATIO == 16 ) ? 3 : 2 , SHU_B1_DQ11_RG_RX_ARDQ_DES_MODE_B1));

	switch (tr->CA_P2S_RATIO)
	{
		case 2 : { TX_ARCA_SERMODE = (0 + tr->CA_FULL_RATE); break; }
		case 4 : { TX_ARCA_SERMODE = (1 + tr->CA_FULL_RATE); break; }
		case 8:  { TX_ARCA_SERMODE = (2 + tr->CA_FULL_RATE); break; }
		default: msg("ERROR: tr->CA_P2S_RATIO= %2d, Not support!!",tr->CA_P2S_RATIO);
	}
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD14), P_Fld(TX_ARCA_SERMODE, SHU_CA_CMD14_RG_TX_ARCA_SER_MODE_CA));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD6),	P_Fld(TX_ARCA_SERMODE, SHU_CA_CMD6_RG_RX_ARCMD_RANK_SEL_SER_MODE));

	switch (tr->DQ_AAMCK_DIV)
	{
		case 2 : { ARDLL_SERMODE_B = 1; break; }
		case 4 : { ARDLL_SERMODE_B = 2; break; }
		case 8:  { ARDLL_SERMODE_B = 3; break; }
		default: msg("WARN: tr->DQ_AAMCK_DIV= %2d, Because of DQ_SEMI_OPEN, It's don't care.",tr->DQ_AAMCK_DIV);
	}
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DLL1), P_Fld(ARDLL_SERMODE_B			 , SHU_B0_DLL1_RG_ARDLL_SER_MODE_B0));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DLL1), P_Fld(ARDLL_SERMODE_B			 , SHU_B1_DLL1_RG_ARDLL_SER_MODE_B1));

	switch (tr->CA_ADMCK_DIV)
	{
		case 2 : { ARDLL_SERMODE_C = 1; break; }
		case 4 : { ARDLL_SERMODE_C = 2; break; }
		case 8:  { ARDLL_SERMODE_C = 3; break; }
		default: msg("ERROR: tr->CA_ADMCK_DIV= %2d, Not support!!",tr->CA_ADMCK_DIV);
	}
	DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
	vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_DLL1),	P_Fld(ARDLL_SERMODE_C			, SHU_CA_DLL1_RG_ARDLL_SER_MODE_CA));
	DramcBroadcastOnOff(DRAMC_BROADCAST_ON);

	//DQ  SEMI-OPEN register control
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ6)			 , P_Fld(  tr->DQ_SEMI_OPEN , SHU_B0_DQ6_RG_ARPI_SOPEN_EN_B0		 ) \
																		 | P_Fld(  tr->DQ_CA_OPEN	, SHU_B0_DQ6_RG_ARPI_OPEN_EN_B0 		 ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ6)			 , P_Fld(  tr->DQ_SEMI_OPEN , SHU_B1_DQ6_RG_ARPI_SOPEN_EN_B1		 ) \
																		 | P_Fld(  tr->DQ_CA_OPEN	, SHU_B1_DQ6_RG_ARPI_OPEN_EN_B1 		 ));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ1)			 , P_Fld((!(tr->DQ_SEMI_OPEN))&&(!(tr->DQ_CKDIV4_EN)), SHU_B0_DQ1_RG_ARPI_MIDPI_EN_B0		  ) \
																		 | P_Fld((!(tr->DQ_SEMI_OPEN))&&(tr->DQ_CKDIV4_EN), SHU_B0_DQ1_RG_ARPI_MIDPI_CKDIV4_EN_B0	  ) \
																		 | P_Fld(	tr->PH8_DLY 	, SHU_B0_DQ1_RG_ARPI_MIDPI_8PH_DLY_B0	 ));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ1)			 , P_Fld((!(tr->DQ_SEMI_OPEN))&&(!(tr->DQ_CKDIV4_EN)), SHU_B1_DQ1_RG_ARPI_MIDPI_EN_B1		  ) \
																		 | P_Fld((!(tr->DQ_SEMI_OPEN))&&(tr->DQ_CKDIV4_EN), SHU_B1_DQ1_RG_ARPI_MIDPI_CKDIV4_EN_B1	  ) \
																		 | P_Fld(	tr->PH8_DLY 	, SHU_B1_DQ1_RG_ARPI_MIDPI_8PH_DLY_B1	 ));


	//CA  SEMI-OPEN register control
	if(tr->CA_SEMI_OPEN == 0)
	{
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD6)		 , P_Fld( 0 		, SHU_CA_CMD6_RG_ARPI_SOPEN_EN_CA		 ) \
																		 | P_Fld( 0 		, SHU_CA_CMD6_RG_ARPI_SOPEN_CKGEN_EN_CA  ) \
																		 | P_Fld( 0 		, SHU_CA_CMD6_RG_ARPI_OFFSET_DQSIEN_CA	 ));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_DLL_ARPI3)	 , P_Fld( 1 		, SHU_CA_DLL_ARPI3_RG_ARPI_FB_EN_CA 	 ) \
																		 | P_Fld( 1 		, SHU_CA_DLL_ARPI3_RG_ARPI_CLK_EN		 ) \
																		 | P_Fld( 1 		, SHU_CA_DLL_ARPI3_RG_ARPI_MCTL_EN_CA	 ));
	} else {
		// @Darren, for DDR800semi
		DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
		vIO32WriteFldMulti_All(DDRPHY_REG_SHU_CA_CMD6		 , P_Fld( 1 		, SHU_CA_CMD6_RG_ARPI_SOPEN_EN_CA		 ) \
																		 | P_Fld( 1 		, SHU_CA_CMD6_RG_ARPI_SOPEN_CKGEN_EN_CA  ) \
																		 | P_Fld( 16		, SHU_CA_CMD6_RG_ARPI_OFFSET_DQSIEN_CA	 ));
		vIO32WriteFldMulti_All(DDRPHY_REG_SHU_CA_DLL_ARPI3	 , P_Fld( 1 		, SHU_CA_DLL_ARPI3_RG_ARPI_FB_EN_CA 	 ) \
																		 | P_Fld( 1 		, SHU_CA_DLL_ARPI3_RG_ARPI_CLK_EN		 ));
		//CHA CA as master
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_DLL_ARPI3)	 , P_Fld(1, SHU_CA_DLL_ARPI3_RG_ARPI_MCTL_EN_CA    ));
		//CHB CA as slave
		vSetPHY2ChannelMapping(p, CHANNEL_B);
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_DLL_ARPI3)	 , P_Fld(0, SHU_CA_DLL_ARPI3_RG_ARPI_MCTL_EN_CA    ));
		vSetPHY2ChannelMapping(p, CHANNEL_A);
		DramcBroadcastOnOff(DRAMC_BROADCAST_ON);

//--------TODO ---20190721 WAITING DPHY KaiHsin & Alucary confirm this RG setting.
//		  if(a_cfg->DLL_ASYNC_EN == 1)
//		  {
//			  //CA as all master
//			  vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_DLL_ARPI3)   , P_Fld(1, SHU_CA_DLL_ARPI3_RG_ARPI_MCTL_EN_CA	 ));
//		  } else {
//			  DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
//			  //CHA CA as master
//			  vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_DLL_ARPI3)   , P_Fld(1, SHU_CA_DLL_ARPI3_RG_ARPI_MCTL_EN_CA	 ));
//			  //CHB CA as slave
//			  vSetPHY2ChannelMapping(p, CHANNEL_B);
//			  vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_DLL_ARPI3)   , P_Fld(0, SHU_CA_DLL_ARPI3_RG_ARPI_MCTL_EN_CA	 ));
//			  vSetPHY2ChannelMapping(p, CHANNEL_A);
//			  DramcBroadcastOnOff(DRAMC_BROADCAST_ON);
//		  }
	}
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD6)		 , P_Fld( tr->DQ_CA_OPEN	, SHU_CA_CMD6_RG_ARPI_OPEN_EN_CA		));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD13)		 , P_Fld( tr->CA_FULL_RATE	, SHU_CA_CMD13_RG_TX_ARCA_FRATE_EN_CA	 ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD1)		 , P_Fld(	tr->PH8_DLY 	, SHU_CA_CMD1_RG_ARPI_MIDPI_8PH_DLY_CA	 ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD1)		 , P_Fld( tr->CA_PREDIV_EN	, SHU_CA_CMD1_RG_ARPI_MIDPI_CKDIV4_PREDIV_EN_CA));
	if(tr->SEMI_OPEN_CA_PICK_MCK_RATIO == 4)
	{
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD6)		 , P_Fld( 0, SHU_CA_CMD6_RG_ARPI_SOPEN_CKGEN_DIV_CA));
	}
	else if (tr->SEMI_OPEN_CA_PICK_MCK_RATIO == 8)
	{
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD6)		 , P_Fld( 1, SHU_CA_CMD6_RG_ARPI_SOPEN_CKGEN_DIV_CA));
	} else {}
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DLL1)		 , P_Fld( tr->DQ_TRACK_CA_EN, SHU_B0_DLL1_RG_ARDLL_TRACKING_CA_EN_B0 ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DLL1)		 , P_Fld( tr->DQ_TRACK_CA_EN, SHU_B1_DLL1_RG_ARDLL_TRACKING_CA_EN_B1 ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_DLL_ARPI2)	 , P_Fld( 1 				, SHU_CA_DLL_ARPI2_RG_ARPI_CG_CLKIEN	 ));
  msg("<<<<<< [CONFIGURE PHASE][SHUFFLE]: ANA CLOCK DIV configuration\n");
}

//==========================
//DLL config
//==========================
static void ANA_DLL_non_shuffle_config(DRAMC_CTX_T *p,ANA_top_config_T *a_cfg)
{
  U8 u1PDZone = (p->frequency >= 2133) ? 0x2 : 0x3;

  if(a_cfg->DLL_IDLE_MODE == 1)
  {
	if(a_cfg->DLL_ASYNC_EN == 1)
	{
	  DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
	  vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_CA_DLL_ARPI5)	 , P_Fld(0			  , CA_DLL_ARPI5_RG_ARDLL_IDLE_EN_CA	   ) \
																	 | P_Fld(u1PDZone			 , CA_DLL_ARPI5_RG_ARDLL_PD_ZONE_CA 	  ));
	  vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_B0_DLL_ARPI5)	 , P_Fld(1			  , B0_DLL_ARPI5_RG_ARDLL_IDLE_EN_B0	   ) \
																	 | P_Fld(u1PDZone			 , B0_DLL_ARPI5_RG_ARDLL_PD_ZONE_B0 	  ));
	  vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_B1_DLL_ARPI5)	 , P_Fld(1			  , B1_DLL_ARPI5_RG_ARDLL_IDLE_EN_B1	   ) \
																	 | P_Fld(u1PDZone			 , B1_DLL_ARPI5_RG_ARDLL_PD_ZONE_B1 	  ));
	  DramcBroadcastOnOff(DRAMC_BROADCAST_ON);
	} else {
	  DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);

	  vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_CA_DLL_ARPI5)	 , P_Fld(0			  , CA_DLL_ARPI5_RG_ARDLL_IDLE_EN_CA	   ) \
																	 | P_Fld(u1PDZone			 , CA_DLL_ARPI5_RG_ARDLL_PD_ZONE_CA 	  ));
	  vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DLL_ARPI5)	 , P_Fld(1			  , B0_DLL_ARPI5_RG_ARDLL_IDLE_EN_B0	   ) \
																	 | P_Fld(u1PDZone			 , B0_DLL_ARPI5_RG_ARDLL_PD_ZONE_B0 	  ));
	  vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DLL_ARPI5)	 , P_Fld(1			  , B1_DLL_ARPI5_RG_ARDLL_IDLE_EN_B1	   ) \
																	 | P_Fld(u1PDZone			 , B1_DLL_ARPI5_RG_ARDLL_PD_ZONE_B1 	  ));
	  vSetPHY2ChannelMapping(p, CHANNEL_B);

	  vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_CA_DLL_ARPI5)	 , P_Fld(1			  , CA_DLL_ARPI5_RG_ARDLL_IDLE_EN_CA	   ) \
																	 | P_Fld(u1PDZone			 , CA_DLL_ARPI5_RG_ARDLL_PD_ZONE_CA 	  ));
	  vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DLL_ARPI5)	 , P_Fld(1			  , B0_DLL_ARPI5_RG_ARDLL_IDLE_EN_B0	   ) \
																	 | P_Fld(u1PDZone			 , B0_DLL_ARPI5_RG_ARDLL_PD_ZONE_B0 	  ));
	  vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DLL_ARPI5)	 , P_Fld(1			  , B1_DLL_ARPI5_RG_ARDLL_IDLE_EN_B1	   ) \
																	 | P_Fld(u1PDZone			 , B1_DLL_ARPI5_RG_ARDLL_PD_ZONE_B1 	  ));

	  #if (CHANNEL_NUM>2)
	  vSetPHY2ChannelMapping(p, CHANNEL_C);
	  vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_CA_DLL_ARPI5)	 , P_Fld(0			  , CA_DLL_ARPI5_RG_ARDLL_IDLE_EN_CA	   )
																	 | P_Fld(u1PDZone			 , CA_DLL_ARPI5_RG_ARDLL_PD_ZONE_CA 	  ));
	  vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DLL_ARPI5)	 , P_Fld(1			  , B0_DLL_ARPI5_RG_ARDLL_IDLE_EN_B0	   )
																	 | P_Fld(u1PDZone			 , B0_DLL_ARPI5_RG_ARDLL_PD_ZONE_B0 	  ));
	  vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DLL_ARPI5)	 , P_Fld(1			  , B1_DLL_ARPI5_RG_ARDLL_IDLE_EN_B1	   )
																	 | P_Fld(u1PDZone			 , B1_DLL_ARPI5_RG_ARDLL_PD_ZONE_B1 	  ));
	  vSetPHY2ChannelMapping(p, CHANNEL_D);

	  vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_CA_DLL_ARPI5)	 , P_Fld(1			  , CA_DLL_ARPI5_RG_ARDLL_IDLE_EN_CA	   )
																	 | P_Fld(u1PDZone			 , CA_DLL_ARPI5_RG_ARDLL_PD_ZONE_CA 	  ));
	  vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DLL_ARPI5)	 , P_Fld(1			  , B0_DLL_ARPI5_RG_ARDLL_IDLE_EN_B0	   )
																	 | P_Fld(u1PDZone			 , B0_DLL_ARPI5_RG_ARDLL_PD_ZONE_B0 	  ));
	  vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DLL_ARPI5)	 , P_Fld(1			  , B1_DLL_ARPI5_RG_ARDLL_IDLE_EN_B1	   )
																	 | P_Fld(u1PDZone			 , B1_DLL_ARPI5_RG_ARDLL_PD_ZONE_B1 	  ));
	  #endif

	  vSetPHY2ChannelMapping(p, CHANNEL_A);

	  DramcBroadcastOnOff(DRAMC_BROADCAST_ON);
	}
  }

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_CA_DLL_ARPI1)		 , P_Fld(0			 , CA_DLL_ARPI1_RG_ARPI_CLKIEN_JUMP_EN		) \
																	 | P_Fld(0			 , CA_DLL_ARPI1_RG_ARPI_CMD_JUMP_EN 		) \
																	 | P_Fld(0			 , CA_DLL_ARPI1_RG_ARPI_CLK_JUMP_EN 		) \
																	 | P_Fld(0			 , CA_DLL_ARPI1_RG_ARPI_CS_JUMP_EN			) \
																	 | P_Fld(0			 , CA_DLL_ARPI1_RG_ARPI_FB_JUMP_EN_CA		) \
																	 | P_Fld(0			 , CA_DLL_ARPI1_RG_ARPI_MCTL_JUMP_EN_CA 	) \
																	 | P_Fld(1			 , CA_DLL_ARPI1_RG_ARPISM_MCK_SEL_CA		) \
																	 | P_Fld(0			 , CA_DLL_ARPI1_RG_ARPISM_MCK_SEL_CA_REG_OPT));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DLL_ARPI1)		 , P_Fld(0			 , B0_DLL_ARPI1_RG_ARPI_DQSIEN_JUMP_EN_B0	) \
																	 | P_Fld(0			 , B0_DLL_ARPI1_RG_ARPI_DQ_JUMP_EN_B0		) \
																	 | P_Fld(0			 , B0_DLL_ARPI1_RG_ARPI_DQM_JUMP_EN_B0		) \
																	 | P_Fld(0			 , B0_DLL_ARPI1_RG_ARPI_DQS_JUMP_EN_B0		) \
																	 | P_Fld(0			 , B0_DLL_ARPI1_RG_ARPI_FB_JUMP_EN_B0		) \
																	 | P_Fld(0			 , B0_DLL_ARPI1_RG_ARPI_MCTL_JUMP_EN_B0 	) \
																	 | P_Fld(0			 , B0_DLL_ARPI1_RG_ARPISM_MCK_SEL_B0		) \
																	 | P_Fld(0			 , B0_DLL_ARPI1_RG_ARPISM_MCK_SEL_B0_REG_OPT));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DLL_ARPI1)		 , P_Fld(0			 , B1_DLL_ARPI1_RG_ARPI_DQSIEN_JUMP_EN_B1	) \
																	 | P_Fld(0			 , B1_DLL_ARPI1_RG_ARPI_DQ_JUMP_EN_B1		) \
																	 | P_Fld(0			 , B1_DLL_ARPI1_RG_ARPI_DQM_JUMP_EN_B1		) \
																	 | P_Fld(0			 , B1_DLL_ARPI1_RG_ARPI_DQS_JUMP_EN_B1		) \
																	 | P_Fld(0			 , B1_DLL_ARPI1_RG_ARPI_FB_JUMP_EN_B1		) \
																	 | P_Fld(0			 , B1_DLL_ARPI1_RG_ARPI_MCTL_JUMP_EN_B1 	) \
																	 | P_Fld(0			 , B1_DLL_ARPI1_RG_ARPISM_MCK_SEL_B1		) \
																	 | P_Fld(0			 , B1_DLL_ARPI1_RG_ARPISM_MCK_SEL_B1_REG_OPT));

	DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
	vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_CA_DLL_ARPI5)		 , P_Fld(0			 , CA_DLL_ARPI5_RG_ARDLL_FJ_OUT_MODE_CA 	) \
																	 | P_Fld(0			 , CA_DLL_ARPI5_RG_ARDLL_FJ_OUT_MODE_SEL_CA ) \
																	 | P_Fld(0			 , CA_DLL_ARPI5_RG_ARDLL_DIV_DEC_CA 		) \
																	 | P_Fld(0			 , CA_DLL_ARPI5_RG_ARDLL_MON_SEL_CA 		));
	vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_B0_DLL_ARPI5)		 , P_Fld(0			 , B0_DLL_ARPI5_RG_ARDLL_FJ_OUT_MODE_B0 	) \
																	 | P_Fld(0			 , B0_DLL_ARPI5_RG_ARDLL_FJ_OUT_MODE_SEL_B0 ) \
																	 | P_Fld(0			 , B0_DLL_ARPI5_RG_ARDLL_DIV_DEC_B0 		) \
																	 | P_Fld(0			 , B0_DLL_ARPI5_RG_ARDLL_MON_SEL_B0 		));
	vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_B1_DLL_ARPI5)		 , P_Fld(0			 , B1_DLL_ARPI5_RG_ARDLL_FJ_OUT_MODE_B1 	) \
																	 | P_Fld(0			 , B1_DLL_ARPI5_RG_ARDLL_FJ_OUT_MODE_SEL_B1 ) \
																	 | P_Fld(0			 , B1_DLL_ARPI5_RG_ARDLL_DIV_DEC_B1 		) \
																	 | P_Fld(0			 , B1_DLL_ARPI5_RG_ARDLL_MON_SEL_B1 		));
	DramcBroadcastOnOff(DRAMC_BROADCAST_ON);
}


static void ANA_DLL_shuffle_Config(DRAMC_CTX_T *p, ANA_top_config_T *a_cfg)
{
	U8 u1Gain = 0;
	if(p->frequency<=1600)
	{
		u1Gain = 2;
		msg(">>>>>> [CONFIGURE PHASE][SHUFFLE]: Add DLL Gain = %d\n",u1Gain);
	}

	msg(">>>>>> [CONFIGURE PHASE][SHUFFLE]: DLL\n");
	if(a_cfg->DLL_ASYNC_EN == 1)
	{
	  DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
	  vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_DLL0) 	   , P_Fld( (a_cfg->ALL_SLAVE_EN == 0)?6:7, SHU_CA_DLL0_RG_ARDLL_GAIN_CA		 ) \
																	   | P_Fld( (a_cfg->ALL_SLAVE_EN == 0)?9:7, SHU_CA_DLL0_RG_ARDLL_IDLECNT_CA 	 ) \
																	   | P_Fld(!(a_cfg->ALL_SLAVE_EN)		  , SHU_CA_DLL0_RG_ARDLL_FAST_PSJP_CA	 ) \
																	   | P_Fld(  0							  , SHU_CA_DLL0_RG_ARDLL_GEAR2_PSJP_CA	 ) \
																	   | P_Fld(  a_cfg->ALL_SLAVE_EN		  , SHU_CA_DLL0_RG_ARDLL_FASTPJ_CK_SEL_CA));
	  vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_DLL1) 	   , P_Fld(  a_cfg->ALL_SLAVE_EN		  , SHU_CA_DLL1_RG_ARDLL_PHDET_OUT_SEL_CA) \
																	   | P_Fld(  a_cfg->ALL_SLAVE_EN		  , SHU_CA_DLL1_RG_ARDLL_PHDET_IN_SWAP_CA) \
																	   | P_Fld(  0							  , SHU_CA_DLL1_RG_ARDLL_PGAIN_CA		 ) \
																	   | P_Fld(  1							  , SHU_CA_DLL1_RG_ARDLL_PSJP_EN_CA 	 ) \
																	   | P_Fld(  1							  , SHU_CA_DLL1_RG_ARDLL_PHDIV_CA		 ) \
																	   | P_Fld(  1							  , SHU_CA_DLL1_RG_ARDLL_UDIV_EN_CA 	 ) \
																	   | P_Fld(  1							  , SHU_CA_DLL1_RG_ARDLL_PS_EN_CA		 ) \
																	   | P_Fld(  !(a_cfg->ALL_SLAVE_EN) 	  , SHU_CA_DLL1_RG_ARDLL_PD_CK_SEL_CA	 ));
	  DramcBroadcastOnOff(DRAMC_BROADCAST_ON);
	} else {
	  DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);

	  vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_DLL0) 	   , P_Fld( (a_cfg->ALL_SLAVE_EN == 0)?6+u1Gain:7+u1Gain, SHU_CA_DLL0_RG_ARDLL_GAIN_CA		   ) \
																	   | P_Fld( (a_cfg->ALL_SLAVE_EN == 0)?9:7, SHU_CA_DLL0_RG_ARDLL_IDLECNT_CA 	 ) \
																	   | P_Fld(!(a_cfg->ALL_SLAVE_EN)		  , SHU_CA_DLL0_RG_ARDLL_FAST_PSJP_CA	 ) \
																	   | P_Fld(  0							  , SHU_CA_DLL0_RG_ARDLL_GEAR2_PSJP_CA	 ) \
																	   | P_Fld(  a_cfg->ALL_SLAVE_EN		  , SHU_CA_DLL0_RG_ARDLL_FASTPJ_CK_SEL_CA));
	  vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_DLL1) 	   , P_Fld(  a_cfg->ALL_SLAVE_EN		  , SHU_CA_DLL1_RG_ARDLL_PHDET_OUT_SEL_CA) \
																	   | P_Fld(  a_cfg->ALL_SLAVE_EN		  , SHU_CA_DLL1_RG_ARDLL_PHDET_IN_SWAP_CA) \
																	   | P_Fld(  0							  , SHU_CA_DLL1_RG_ARDLL_PGAIN_CA		 ) \
																	   | P_Fld(  1							  , SHU_CA_DLL1_RG_ARDLL_PSJP_EN_CA 	 ) \
																	   | P_Fld(  1							  , SHU_CA_DLL1_RG_ARDLL_PHDIV_CA		 ) \
																	   | P_Fld(  1							  , SHU_CA_DLL1_RG_ARDLL_UDIV_EN_CA 	 ) \
																	   | P_Fld(  1							  , SHU_CA_DLL1_RG_ARDLL_PS_EN_CA		 ) \
																	   | P_Fld(!(a_cfg->ALL_SLAVE_EN)		  , SHU_CA_DLL1_RG_ARDLL_PD_CK_SEL_CA	 ));

	  vSetPHY2ChannelMapping(p, CHANNEL_B);

	  vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_DLL0) 	   , P_Fld(  7+u1Gain					  , SHU_CA_DLL0_RG_ARDLL_GAIN_CA		 ) \
																	   | P_Fld(  7					   , SHU_CA_DLL0_RG_ARDLL_IDLECNT_CA	  ) \
																	   | P_Fld(  0					   , SHU_CA_DLL0_RG_ARDLL_FAST_PSJP_CA	  ) \
																	   | P_Fld(  0					   , SHU_CA_DLL0_RG_ARDLL_GEAR2_PSJP_CA   ) \
																	   | P_Fld(  1					   , SHU_CA_DLL0_RG_ARDLL_FASTPJ_CK_SEL_CA));
	  vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_DLL1) 	   , P_Fld(  1					   , SHU_CA_DLL1_RG_ARDLL_PHDET_OUT_SEL_CA) \
																	   | P_Fld(  1					   , SHU_CA_DLL1_RG_ARDLL_PHDET_IN_SWAP_CA) \
																	   | P_Fld(  0					   , SHU_CA_DLL1_RG_ARDLL_PGAIN_CA		  ) \
																	   | P_Fld(  1					   , SHU_CA_DLL1_RG_ARDLL_PSJP_EN_CA	  ) \
																	   | P_Fld(  1					   , SHU_CA_DLL1_RG_ARDLL_PHDIV_CA		  ) \
																	   | P_Fld(  1					   , SHU_CA_DLL1_RG_ARDLL_UDIV_EN_CA	  ) \
																	   | P_Fld(  1					   , SHU_CA_DLL1_RG_ARDLL_PS_EN_CA		  ) \
																	   | P_Fld(  0					   , SHU_CA_DLL1_RG_ARDLL_PD_CK_SEL_CA	  ));

	  #if (CHANNEL_NUM>2)
	  vSetPHY2ChannelMapping(p, CHANNEL_C);

	  vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_DLL0) 	   , P_Fld( (a_cfg->ALL_SLAVE_EN == 0)?6+u1Gain:7+u1Gain, SHU_CA_DLL0_RG_ARDLL_GAIN_CA		   )
																	   | P_Fld( (a_cfg->ALL_SLAVE_EN == 0)?9:7, SHU_CA_DLL0_RG_ARDLL_IDLECNT_CA 	 )
																	   | P_Fld(!(a_cfg->ALL_SLAVE_EN)		  , SHU_CA_DLL0_RG_ARDLL_FAST_PSJP_CA	 )
																	   | P_Fld(  0							  , SHU_CA_DLL0_RG_ARDLL_GEAR2_PSJP_CA	 )
																	   | P_Fld(  a_cfg->ALL_SLAVE_EN		  , SHU_CA_DLL0_RG_ARDLL_FASTPJ_CK_SEL_CA));
	  vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_DLL1) 	   , P_Fld(  a_cfg->ALL_SLAVE_EN		  , SHU_CA_DLL1_RG_ARDLL_PHDET_OUT_SEL_CA)
																	   | P_Fld(  a_cfg->ALL_SLAVE_EN		  , SHU_CA_DLL1_RG_ARDLL_PHDET_IN_SWAP_CA)
																	   | P_Fld(  0							  , SHU_CA_DLL1_RG_ARDLL_PGAIN_CA		 )
																	   | P_Fld(  1							  , SHU_CA_DLL1_RG_ARDLL_PSJP_EN_CA 	 )
																	   | P_Fld(  1							  , SHU_CA_DLL1_RG_ARDLL_PHDIV_CA		 )
																	   | P_Fld(  1							  , SHU_CA_DLL1_RG_ARDLL_UDIV_EN_CA 	 )
																	   | P_Fld(  1							  , SHU_CA_DLL1_RG_ARDLL_PS_EN_CA		 )
																	   | P_Fld(!(a_cfg->ALL_SLAVE_EN)		  , SHU_CA_DLL1_RG_ARDLL_PD_CK_SEL_CA	 ));

	  vSetPHY2ChannelMapping(p, CHANNEL_D);

	  vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_DLL0) 	   , P_Fld(  7+u1Gain					  , SHU_CA_DLL0_RG_ARDLL_GAIN_CA		 )
																	   | P_Fld(  7					   , SHU_CA_DLL0_RG_ARDLL_IDLECNT_CA	  )
																	   | P_Fld(  0					   , SHU_CA_DLL0_RG_ARDLL_FAST_PSJP_CA	  )
																	   | P_Fld(  0					   , SHU_CA_DLL0_RG_ARDLL_GEAR2_PSJP_CA   )
																	   | P_Fld(  1					   , SHU_CA_DLL0_RG_ARDLL_FASTPJ_CK_SEL_CA));
	  vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_DLL1) 	   , P_Fld(  1					   , SHU_CA_DLL1_RG_ARDLL_PHDET_OUT_SEL_CA)
																	   | P_Fld(  1					   , SHU_CA_DLL1_RG_ARDLL_PHDET_IN_SWAP_CA)
																	   | P_Fld(  0					   , SHU_CA_DLL1_RG_ARDLL_PGAIN_CA		  )
																	   | P_Fld(  1					   , SHU_CA_DLL1_RG_ARDLL_PSJP_EN_CA	  )
																	   | P_Fld(  1					   , SHU_CA_DLL1_RG_ARDLL_PHDIV_CA		  )
																	   | P_Fld(  1					   , SHU_CA_DLL1_RG_ARDLL_UDIV_EN_CA	  )
																	   | P_Fld(  1					   , SHU_CA_DLL1_RG_ARDLL_PS_EN_CA		  )
																	   | P_Fld(  0					   , SHU_CA_DLL1_RG_ARDLL_PD_CK_SEL_CA	  ));
	  #endif

	  vSetPHY2ChannelMapping(p, CHANNEL_A);

	  DramcBroadcastOnOff(DRAMC_BROADCAST_ON);
	}

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DLL0)		 , P_Fld(  7+u1Gain 					  , SHU_B0_DLL0_RG_ARDLL_GAIN_B0		 ) \
																	 | P_Fld(  7					   , SHU_B0_DLL0_RG_ARDLL_IDLECNT_B0	  ) \
																	 | P_Fld(  0					   , SHU_B0_DLL0_RG_ARDLL_FAST_PSJP_B0	  ) \
																	 | P_Fld(  0					   , SHU_B0_DLL0_RG_ARDLL_GEAR2_PSJP_B0   ) \
																	 | P_Fld(  1					   , SHU_B0_DLL0_RG_ARDLL_FASTPJ_CK_SEL_B0));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DLL1)		 , P_Fld(  1					   , SHU_B0_DLL1_RG_ARDLL_PHDET_OUT_SEL_B0) \
																	 | P_Fld(  1					   , SHU_B0_DLL1_RG_ARDLL_PHDET_IN_SWAP_B0) \
																	 | P_Fld(  0					   , SHU_B0_DLL1_RG_ARDLL_PGAIN_B0		  ) \
																	 | P_Fld(  1					   , SHU_B0_DLL1_RG_ARDLL_PSJP_EN_B0	  ) \
																	 | P_Fld(  1					   , SHU_B0_DLL1_RG_ARDLL_PHDIV_B0		  ) \
																	 | P_Fld(  1					   , SHU_B0_DLL1_RG_ARDLL_UDIV_EN_B0	  ) \
																	 | P_Fld(  1					   , SHU_B0_DLL1_RG_ARDLL_PS_EN_B0		  ) \
																	 | P_Fld(  0					   , SHU_B0_DLL1_RG_ARDLL_PD_CK_SEL_B0	  ));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DLL0)		 , P_Fld(  7+u1Gain 					  , SHU_B1_DLL0_RG_ARDLL_GAIN_B1		 ) \
																	 | P_Fld(  7					   , SHU_B1_DLL0_RG_ARDLL_IDLECNT_B1	  ) \
																	 | P_Fld(  0					   , SHU_B1_DLL0_RG_ARDLL_FAST_PSJP_B1	  ) \
																	 | P_Fld(  0					   , SHU_B1_DLL0_RG_ARDLL_GEAR2_PSJP_B1   ) \
																	 | P_Fld(  1					   , SHU_B1_DLL0_RG_ARDLL_FASTPJ_CK_SEL_B1));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DLL1)		 , P_Fld(  1					   , SHU_B1_DLL1_RG_ARDLL_PHDET_OUT_SEL_B1) \
																	 | P_Fld(  1					   , SHU_B1_DLL1_RG_ARDLL_PHDET_IN_SWAP_B1) \
																	 | P_Fld(  0					   , SHU_B1_DLL1_RG_ARDLL_PGAIN_B1		  ) \
																	 | P_Fld(  1					   , SHU_B1_DLL1_RG_ARDLL_PSJP_EN_B1	  ) \
																	 | P_Fld(  1					   , SHU_B1_DLL1_RG_ARDLL_PHDIV_B1		  ) \
																	 | P_Fld(  1					   , SHU_B1_DLL1_RG_ARDLL_UDIV_EN_B1	  ) \
																	 | P_Fld(  1					   , SHU_B1_DLL1_RG_ARDLL_PS_EN_B1		  ) \
																	 | P_Fld(  0					   , SHU_B1_DLL1_RG_ARDLL_PD_CK_SEL_B1	  ));
	msg("<<<<<< [CONFIGURE PHASE][SHUFFLE]: DLL\n");
}

static void ANA_ARPI_shuffle_config(DRAMC_CTX_T *p,ANA_top_config_T *a_cfg,ANA_DVFS_CORE_T *tr)
{
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_DLL_ARPI3)	 , P_Fld(  0		  , SHU_CA_DLL_ARPI3_RG_ARPI_CLKIEN_EN	   ) \
																	 | P_Fld(  !(tr->DQ_SEMI_OPEN), SHU_CA_DLL_ARPI3_RG_ARPI_CMD_EN 	   ) \
																	 | P_Fld(  !(tr->DQ_SEMI_OPEN), SHU_CA_DLL_ARPI3_RG_ARPI_CS_EN		   ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DLL_ARPI3)	 , P_Fld(  !(tr->DQ_SEMI_OPEN), SHU_B0_DLL_ARPI3_RG_ARPI_DQSIEN_EN_B0  ) \
																	 | P_Fld(  !(tr->DQ_SEMI_OPEN), SHU_B0_DLL_ARPI3_RG_ARPI_DQ_EN_B0	   ) \
																	 | P_Fld(  !(tr->DQ_SEMI_OPEN), SHU_B0_DLL_ARPI3_RG_ARPI_DQM_EN_B0	   ) \
																	 | P_Fld(  !(tr->DQ_SEMI_OPEN), SHU_B0_DLL_ARPI3_RG_ARPI_DQS_EN_B0	   ) \
																	 | P_Fld(  !(tr->DQ_SEMI_OPEN), SHU_B0_DLL_ARPI3_RG_ARPI_FB_EN_B0	   ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DLL_ARPI3)	 , P_Fld((!(tr->DQ_SEMI_OPEN)) && (a_cfg->NEW_RANK_MODE), SHU_B0_DLL_ARPI3_RG_ARPI_MCTL_EN_B0	 ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DLL_ARPI3)	 , P_Fld(  !(tr->DQ_SEMI_OPEN), SHU_B1_DLL_ARPI3_RG_ARPI_DQSIEN_EN_B1  ) \
																	 | P_Fld(  !(tr->DQ_SEMI_OPEN), SHU_B1_DLL_ARPI3_RG_ARPI_DQ_EN_B1	   ) \
																	 | P_Fld(  !(tr->DQ_SEMI_OPEN), SHU_B1_DLL_ARPI3_RG_ARPI_DQM_EN_B1	   ) \
																	 | P_Fld(  !(tr->DQ_SEMI_OPEN), SHU_B1_DLL_ARPI3_RG_ARPI_DQS_EN_B1	   ) \
																	 | P_Fld(  !(tr->DQ_SEMI_OPEN), SHU_B1_DLL_ARPI3_RG_ARPI_FB_EN_B1	   ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DLL_ARPI3)	 , P_Fld((!(tr->DQ_SEMI_OPEN)) && (a_cfg->NEW_RANK_MODE), SHU_B1_DLL_ARPI3_RG_ARPI_MCTL_EN_B1	 ));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD2)		 , P_Fld(  1		  , SHU_CA_CMD2_RG_ARPISM_MCK_SEL_CA_SHU   ) \
																	 | P_Fld(  1		  , SHU_CA_CMD2_RG_ARPI_TX_CG_SYNC_DIS_CA  ) \
																	 | P_Fld(  1		  , SHU_CA_CMD2_RG_ARPI_TX_CG_CA_EN_CA	   ) \
																	 | P_Fld(  1		  , SHU_CA_CMD2_RG_ARPI_TX_CG_CLK_EN_CA    ) \
																	 | P_Fld(  1		  , SHU_CA_CMD2_RG_ARPI_TX_CG_CS_EN_CA	   ) \
																	 | P_Fld(  0		  , SHU_CA_CMD2_RG_ARPI_PD_MCTL_SEL_CA	   ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ2)		 , P_Fld(  1		  , SHU_B0_DQ2_RG_ARPISM_MCK_SEL_B0_SHU    ) \
																	 | P_Fld(  1		  , SHU_B0_DQ2_RG_ARPI_TX_CG_SYNC_DIS_B0   ) \
																	 | P_Fld(  1		  , SHU_B0_DQ2_RG_ARPI_TX_CG_DQ_EN_B0	   ) \
																	 | P_Fld(  1		  , SHU_B0_DQ2_RG_ARPI_TX_CG_DQS_EN_B0	   ) \
																	 | P_Fld(  1		  , SHU_B0_DQ2_RG_ARPI_TX_CG_DQM_EN_B0	   ) \
																	 | P_Fld(  1		  , SHU_B0_DQ2_RG_ARPI_PD_MCTL_SEL_B0	   ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ2)		 , P_Fld(  1		  , SHU_B1_DQ2_RG_ARPISM_MCK_SEL_B1_SHU    ) \
																	 | P_Fld(  1		  , SHU_B1_DQ2_RG_ARPI_TX_CG_SYNC_DIS_B1   ) \
																	 | P_Fld(  1		  , SHU_B1_DQ2_RG_ARPI_TX_CG_DQ_EN_B1	   ) \
																	 | P_Fld(  1		  , SHU_B1_DQ2_RG_ARPI_TX_CG_DQS_EN_B1	   ) \
																	 | P_Fld(  1		  , SHU_B1_DQ2_RG_ARPI_TX_CG_DQM_EN_B1	   ) \
																	 | P_Fld(  1		  , SHU_B1_DQ2_RG_ARPI_PD_MCTL_SEL_B1	   ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ7)		 , P_Fld(  0		  , SHU_B0_DQ7_R_DMTX_ARPI_CG_DQ_NEW_B0    ) \
																	 | P_Fld(  0		  , SHU_B0_DQ7_R_DMTX_ARPI_CG_DQS_NEW_B0   ) \
																	 | P_Fld(  0		  , SHU_B0_DQ7_R_DMTX_ARPI_CG_DQM_NEW_B0   ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ7)		 , P_Fld(  0		  , SHU_B1_DQ7_R_DMTX_ARPI_CG_DQ_NEW_B1    ) \
																	 | P_Fld(  0		  , SHU_B1_DQ7_R_DMTX_ARPI_CG_DQS_NEW_B1   ) \
																	 | P_Fld(  0		  , SHU_B1_DQ7_R_DMTX_ARPI_CG_DQM_NEW_B1   ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD7)		 , P_Fld(  0		  , SHU_CA_CMD7_R_DMTX_ARPI_CG_CS_NEW	   ) \
																	 | P_Fld(  0		  , SHU_CA_CMD7_R_DMTX_ARPI_CG_CMD_NEW	   ));
}
//==========================
//ANA_TX_CONFIG
//==========================
static void ANA_TX_nonshuffle_config(DRAMC_CTX_T *p,ANA_top_config_T *a_cfg)
{
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_CA_CMD6)		, P_Fld(0							 , CA_CMD6_RG_TX_ARCMD_DDR3_SEL 		) \
																| P_Fld(0							 , CA_CMD6_RG_TX_ARCMD_DDR4_SEL 		) \
																| P_Fld(1							 , CA_CMD6_RG_TX_ARCMD_LP4_SEL			));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ6)		, P_Fld(0							 , B0_DQ6_RG_TX_ARDQ_DDR3_SEL_B0		) \
																| P_Fld(!(a_cfg->LP45_APHY_COMB_EN)  , B0_DQ6_RG_TX_ARDQ_DDR4_SEL_B0		) \
																| P_Fld(a_cfg->LP45_APHY_COMB_EN	 , B0_DQ6_RG_TX_ARDQ_LP4_SEL_B0 		));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ6)		, P_Fld(0							 , B1_DQ6_RG_TX_ARDQ_DDR3_SEL_B1		) \
																| P_Fld(!(a_cfg->LP45_APHY_COMB_EN)  , B1_DQ6_RG_TX_ARDQ_DDR4_SEL_B1		) \
																| P_Fld(a_cfg->LP45_APHY_COMB_EN	 , B1_DQ6_RG_TX_ARDQ_LP4_SEL_B1 		));
	msg("<<<<<< [CONFIGURE PHASE]: ANA_TX\n");
	//enable TX OE
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_CA_CMD2)		, P_Fld(1					   , CA_CMD2_RG_TX_ARCMD_OE_DIS_CA		  ) \
																| P_Fld(0					   , CA_CMD2_RG_TX_ARCMD_ODTEN_DIS_CA	  ) \
																| P_Fld(0					   , CA_CMD2_RG_TX_ARCLK_OE_DIS_CA		  ) \
																| P_Fld(0					   , CA_CMD2_RG_TX_ARCLK_ODTEN_DIS_CA	  ) \
																| P_Fld(1					   , CA_CMD2_RG_TX_ARCS_OE_TIE_SEL_CA	  ) \
																| P_Fld(1					   , CA_CMD2_RG_TX_ARCS_OE_TIE_EN_CA	  ) \
																| P_Fld(0					   , CA_CMD2_RG_TX_ARCA_OE_TIE_SEL_CA	  ) \
																| P_Fld(0xff				   , CA_CMD2_RG_TX_ARCA_OE_TIE_EN_CA	  ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ2)		, P_Fld(0					   , B0_DQ2_RG_TX_ARDQ_OE_DIS_B0		  ) \
																| P_Fld(0					   , B0_DQ2_RG_TX_ARDQ_ODTEN_DIS_B0 	  ) \
																| P_Fld(0					   , B0_DQ2_RG_TX_ARDQM0_OE_DIS_B0		  ) \
																| P_Fld(0					   , B0_DQ2_RG_TX_ARDQM0_ODTEN_DIS_B0	  )
																| P_Fld(0					   , B0_DQ2_RG_TX_ARDQS0_OE_DIS_B0		  ) \
																| P_Fld(0					   , B0_DQ2_RG_TX_ARDQS0_ODTEN_DIS_B0	  ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ2)		, P_Fld(0					   , B1_DQ2_RG_TX_ARDQ_OE_DIS_B1		  ) \
																| P_Fld(0					   , B1_DQ2_RG_TX_ARDQ_ODTEN_DIS_B1 	  ) \
																| P_Fld(0					   , B1_DQ2_RG_TX_ARDQM0_OE_DIS_B1		  ) \
																| P_Fld(0					   , B1_DQ2_RG_TX_ARDQM0_ODTEN_DIS_B1	  ) \
																| P_Fld(0					   , B1_DQ2_RG_TX_ARDQS0_OE_DIS_B1		  ) \
																| P_Fld(0					   , B1_DQ2_RG_TX_ARDQS0_ODTEN_DIS_B1	  ));
	//enable TX & reset
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_CA_CMD3)		, P_Fld(1					   , CA_CMD3_RG_TX_ARCMD_EN 			  ) \
																| P_Fld(1					   , CA_CMD3_RG_ARCMD_RESETB			  ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ3)		, P_Fld(1					   , B0_DQ3_RG_ARDQ_RESETB_B0			  ) \
																| P_Fld(1					   , B0_DQ3_RG_TX_ARDQ_EN_B0			  ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ3)		, P_Fld(1					   , B1_DQ3_RG_ARDQ_RESETB_B1			  ) \
																| P_Fld(1					   , B1_DQ3_RG_TX_ARDQ_EN_B1			  ));
}

static void ANA_TX_shuffle_config(DRAMC_CTX_T *p,ANA_top_config_T *a_cfg,U8 group_id)
{
	//ODTEN & DQS control
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD14)	, P_Fld(0					   , SHU_CA_CMD14_RG_TX_ARCA_OE_ODTEN_CG_EN_CA		   ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD13)	, P_Fld(0					   , SHU_CA_CMD13_RG_TX_ARCLK_OE_ODTEN_CG_EN_CA 	   ) \
																| P_Fld(0					   , SHU_CA_CMD13_RG_TX_ARCS_OE_ODTEN_CG_EN_CA		   ));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ13)	, P_Fld(1					   , SHU_B0_DQ13_RG_TX_ARDQS_OE_ODTEN_CG_EN_B0		   )
																| P_Fld(1					   , SHU_B0_DQ13_RG_TX_ARDQM_OE_ODTEN_CG_EN_B0		   )
																| P_Fld(0			, SHU_B0_DQ13_RG_TX_ARDQS_READ_BASE_EN_B0			)
																| P_Fld(0			, SHU_B0_DQ13_RG_TX_ARDQSB_READ_BASE_EN_B0			)
																| P_Fld(0			, SHU_B0_DQ13_RG_TX_ARDQS_READ_BASE_DATA_TIE_EN_B0	)
																| P_Fld(0			, SHU_B0_DQ13_RG_TX_ARDQSB_READ_BASE_DATA_TIE_EN_B0 )
																| P_Fld(a_cfg->TX_ODT_DIS	   , SHU_B0_DQ13_RG_TX_ARDQ_IO_ODT_DIS_B0			   ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ13)	, P_Fld(1					   , SHU_B1_DQ13_RG_TX_ARDQS_OE_ODTEN_CG_EN_B1		   )
																| P_Fld(1					   , SHU_B1_DQ13_RG_TX_ARDQM_OE_ODTEN_CG_EN_B1		   )
																| P_Fld(0			, SHU_B1_DQ13_RG_TX_ARDQS_READ_BASE_EN_B1			)
																| P_Fld(0			, SHU_B1_DQ13_RG_TX_ARDQSB_READ_BASE_EN_B1			)
																| P_Fld(0			, SHU_B1_DQ13_RG_TX_ARDQS_READ_BASE_DATA_TIE_EN_B1	)
																| P_Fld(0			, SHU_B1_DQ13_RG_TX_ARDQSB_READ_BASE_DATA_TIE_EN_B1 )
																| P_Fld(a_cfg->TX_ODT_DIS	   , SHU_B1_DQ13_RG_TX_ARDQ_IO_ODT_DIS_B1			   ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ14)	, P_Fld(1					   , SHU_B0_DQ14_RG_TX_ARDQ_OE_ODTEN_CG_EN_B0		   ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ14)	, P_Fld(1					   , SHU_B1_DQ14_RG_TX_ARDQ_OE_ODTEN_CG_EN_B1		   ));


	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ13)	, P_Fld((LPDDR5_EN_S) ? 2 : 0  , SHU_B0_DQ13_RG_TX_ARDQS_MCKIO_SEL_B0			));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ13)	, P_Fld((LPDDR5_EN_S) ? 2 : 0  , SHU_B1_DQ13_RG_TX_ARDQS_MCKIO_SEL_B1			));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ14)	, P_Fld(  0 				   , SHU_B0_DQ14_RG_TX_ARDQ_MCKIO_SEL_B0			   ) \
																| P_Fld(  0 				   , SHU_B0_DQ14_RG_TX_ARWCK_MCKIO_SEL_B0			   ));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ14)	, P_Fld(  0 				   , SHU_B1_DQ14_RG_TX_ARDQ_MCKIO_SEL_B1			   ) \
																| P_Fld(  0 				   , SHU_B1_DQ14_RG_TX_ARWCK_MCKIO_SEL_B1			   ));

	#if SA_CONFIG_EN
	// enable after runtime configs
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ13)	, P_Fld( 0	, SHU_B0_DQ13_RG_TX_ARDQ_DLY_LAT_EN_B0				));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ13)	, P_Fld( 0	, SHU_B1_DQ13_RG_TX_ARDQ_DLY_LAT_EN_B1				));
	#else
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ13)	, P_Fld( a_cfg->NEW_RANK_MODE  , SHU_B0_DQ13_RG_TX_ARDQ_DLY_LAT_EN_B0			   ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ13)	, P_Fld( a_cfg->NEW_RANK_MODE  , SHU_B1_DQ13_RG_TX_ARDQ_DLY_LAT_EN_B1			   ));
	#endif
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD13)	, P_Fld( 0					   , SHU_CA_CMD13_RG_TX_ARCA_DLY_LAT_EN_CA			   ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD2)	, P_Fld( 0					   , SHU_CA_CMD2_RG_ARPI_OFFSET_LAT_EN_CA			   ) \
																| P_Fld( 0					   , SHU_CA_CMD2_RG_ARPI_OFFSET_ASYNC_EN_CA 		   ));
}

static void ANA_RX_shuffle_config(DRAMC_CTX_T *p,U8 group_id)
{

	U8 RDQS_SE_EN		 ;
	U8 DQSIEN_MODE		 ;
	U8 NEW_RANK_MODE	 ;

#if ENABLE_LP4Y_DFS
	RDQS_SE_EN		  = DFS(group_id)->data_rate<=1600 ? 1 : 0;
#else
	RDQS_SE_EN		  = 0; //TODO for LPDDR5
#endif
	DQSIEN_MODE 	  = DFS(group_id)->DQSIEN_MODE;
	NEW_RANK_MODE	  = A_T->NEW_RANK_MODE;

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ10)	, P_Fld(  RDQS_SE_EN		   , SHU_B0_DQ10_RG_RX_ARDQS_SE_EN_B0				  ) \
																| P_Fld(DQSIEN_MODE 		   , SHU_B0_DQ10_RG_RX_ARDQS_DQSIEN_MODE_B0 		  ) \
																| P_Fld(1					   , SHU_B0_DQ10_RG_RX_ARDQS_DLY_LAT_EN_B0			  ) \
																| P_Fld(NEW_RANK_MODE		   , SHU_B0_DQ10_RG_RX_ARDQS_RANK_SEL_LAT_EN_B0 	  ) \
																| P_Fld(NEW_RANK_MODE		   , SHU_B0_DQ10_RG_RX_ARDQS_DQSIEN_RANK_SEL_LAT_EN_B0));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ11)	, P_Fld(NEW_RANK_MODE		   , SHU_B0_DQ11_RG_RX_ARDQ_RANK_SEL_SER_EN_B0		  ) \
																| P_Fld(NEW_RANK_MODE		   , SHU_B0_DQ11_RG_RX_ARDQ_RANK_SEL_LAT_EN_B0		  ) );
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ2)	, P_Fld(NEW_RANK_MODE		   , SHU_B0_DQ2_RG_ARPI_OFFSET_LAT_EN_B0			  )\
																| P_Fld(NEW_RANK_MODE		   , SHU_B0_DQ2_RG_ARPI_OFFSET_ASYNC_EN_B0			  ));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ10)	, P_Fld(  RDQS_SE_EN		   , SHU_B1_DQ10_RG_RX_ARDQS_SE_EN_B1				  ) \
																| P_Fld(DQSIEN_MODE 		   , SHU_B1_DQ10_RG_RX_ARDQS_DQSIEN_MODE_B1 		  ) \
																| P_Fld(1					   , SHU_B1_DQ10_RG_RX_ARDQS_DLY_LAT_EN_B1			  ) \
																| P_Fld(NEW_RANK_MODE		   , SHU_B1_DQ10_RG_RX_ARDQS_RANK_SEL_LAT_EN_B1 	  ) \
																| P_Fld(NEW_RANK_MODE		   , SHU_B1_DQ10_RG_RX_ARDQS_DQSIEN_RANK_SEL_LAT_EN_B1));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ11)	, P_Fld(NEW_RANK_MODE		   , SHU_B1_DQ11_RG_RX_ARDQ_RANK_SEL_SER_EN_B1		  ) \
																| P_Fld(NEW_RANK_MODE		   , SHU_B1_DQ11_RG_RX_ARDQ_RANK_SEL_LAT_EN_B1		  ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ2)	, P_Fld(NEW_RANK_MODE		   , SHU_B1_DQ2_RG_ARPI_OFFSET_LAT_EN_B1			  )\
																| P_Fld(NEW_RANK_MODE		   , SHU_B1_DQ2_RG_ARPI_OFFSET_ASYNC_EN_B1			  ));

	#if SA_CONFIG_EN
	// enable after runtime configs
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ10)	, P_Fld( 0	, SHU_B0_DQ10_RG_RX_ARDQS_DLY_LAT_EN_B0 			 ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ2)   , P_Fld( 0  , SHU_B0_DQ2_RG_ARPI_OFFSET_LAT_EN_B0			  ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ10)	, P_Fld( 0	, SHU_B1_DQ10_RG_RX_ARDQS_DLY_LAT_EN_B1 			 ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ2)   , P_Fld( 0  , SHU_B1_DQ2_RG_ARPI_OFFSET_LAT_EN_B1			  ));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD10)	  , P_Fld(1, SHU_CA_CMD10_RG_RX_ARCLK_RANK_SEL_LAT_EN_CA			  )\
																| P_Fld(1, SHU_CA_CMD10_RG_RX_ARCLK_DQSIEN_RANK_SEL_LAT_EN_CA			 ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD11)	 , P_Fld(1, SHU_CA_CMD11_RG_RX_ARCA_RANK_SEL_LAT_EN_CA				));
	#endif
}


static void ANA_RX_nonshuffle_config(DRAMC_CTX_T *p)
{
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_CA_CMD6)		, P_Fld( 0					   , CA_CMD6_RG_RX_ARCMD_DDR3_SEL					  ) \
																| P_Fld( 0					   , CA_CMD6_RG_RX_ARCMD_DDR4_SEL					  ) \
																| P_Fld( 0					   , CA_CMD6_RG_RX_ARCMD_BIAS_VREF_SEL				  ) \
																| P_Fld( 0					   , CA_CMD6_RG_RX_ARCMD_RES_BIAS_EN				  ));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ5)		, P_Fld( 1					   , B0_DQ5_RG_RX_ARDQ_VREF_EN_B0					  ));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ6)		, P_Fld( 0					   , B0_DQ6_RG_RX_ARDQ_DDR3_SEL_B0					  ) \
																| P_Fld( 1					   , B0_DQ6_RG_RX_ARDQ_DDR4_SEL_B0					  ) \
																| P_Fld( 0					   , B0_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B0 			  ) \
																| P_Fld( 1					   , B0_DQ6_RG_RX_ARDQ_BIAS_EN_B0					  ) \
																| P_Fld( 0					   , B0_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B0 			  ) \
																| P_Fld( 1					   , B0_DQ6_RG_RX_ARDQ_RES_BIAS_EN_B0				  ));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ3)		, P_Fld( 1					   , B0_DQ3_RG_RX_ARDQ_STBENCMP_EN_B0				  ) \
																| P_Fld( 1					   , B0_DQ3_RG_RX_ARDQ_SMT_EN_B0					  ));

//	  vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ6)		  , P_Fld( 1					 , B0_DQ6_RG_RX_ARDQ_BIAS_EN_B0));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ5)		, P_Fld( 1					   , B1_DQ5_RG_RX_ARDQ_VREF_EN_B1					  ));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ6)		, P_Fld( 0					   , B1_DQ6_RG_RX_ARDQ_DDR3_SEL_B1					  ) \
																| P_Fld( 1					   , B1_DQ6_RG_RX_ARDQ_DDR4_SEL_B1					  ) \
																| P_Fld( 0					   , B1_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B1 			  ) \
																| P_Fld( 1					   , B1_DQ6_RG_RX_ARDQ_BIAS_EN_B1					  ) \
																| P_Fld( 0					   , B1_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B1 			  ) \
																| P_Fld( 1					   , B1_DQ6_RG_RX_ARDQ_RES_BIAS_EN_B1				  ));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ3)		, P_Fld( 1					   , B1_DQ3_RG_RX_ARDQ_STBENCMP_EN_B1				  ) \
																| P_Fld( 1					   , B1_DQ3_RG_RX_ARDQ_SMT_EN_B1					  ));

	//vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ6)		  , P_Fld( 1					 , B1_DQ6_RG_RX_ARDQ_BIAS_EN_B1));

	//RX reset
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_CA_CMD9)		, P_Fld( 1					   , CA_CMD9_RG_RX_ARCMD_STBEN_RESETB				  ) \
																| P_Fld( 1					   , CA_CMD9_RG_RX_ARCLK_STBEN_RESETB				  ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ9)		, P_Fld( 1					   , B0_DQ9_RG_RX_ARDQ_STBEN_RESETB_B0				  ) \
																| P_Fld( 1					   , B0_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B0			  ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ9)		, P_Fld( 1					   , B1_DQ9_RG_RX_ARDQ_STBEN_RESETB_B1				  ) \
																| P_Fld( 1					   , B1_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B1			  ));

	//Justin confirm that: All set 1  for improving internal timing option
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_CA_CMD8)		, P_Fld( 1					   , CA_CMD8_RG_RX_ARCLK_SER_RST_MODE				  ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ8)		, P_Fld( 1					   , B0_DQ8_RG_RX_ARDQS_SER_RST_MODE_B0 			  ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ8)		, P_Fld( 1					   , B1_DQ8_RG_RX_ARDQS_SER_RST_MODE_B1 			  ));

}

//============================================
// RESET
//============================================
void RESETB_PULL_DN(DRAMC_CTX_T *p)
{
	msg("============ PULL DRAM RESETB DOWN ============\n");
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_CA_CMD11)		, P_Fld( 1					   , CA_CMD11_RG_RRESETB_DRVP		  ) \
																| P_Fld( 1					   , CA_CMD11_RG_RRESETB_DRVN		  ) \
																| P_Fld( 1					   , CA_CMD11_RG_TX_RRESETB_DDR3_SEL  ) \
																| P_Fld( 1					   , CA_CMD11_RG_TX_RRESETB_PULL_DN   ));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CTRL1)	, P_Fld( 1					   , MISC_CTRL1_R_DMRRESETB_I_OPT	  ));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CTRL1)	, P_Fld( 1					   , MISC_CTRL1_R_DMDA_RRESETB_E	  ));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_CA_CMD11)		, P_Fld( 0					   , CA_CMD11_RG_TX_RRESETB_PULL_DN   ));
	msg("========== PULL DRAM RESETB DOWN end =========\n");
}
//============================================
// SUSPEND_OFF_control
//============================================
static void SUSPEND_ON(DRAMC_CTX_T *p)
{
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_LP_CTRL0)	, P_Fld( 0					   , B0_LP_CTRL0_RG_ARDMSUS_10_B0	  ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_LP_CTRL0)	, P_Fld( 0					   , B1_LP_CTRL0_RG_ARDMSUS_10_B1	  ));
	DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
	vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_CA_LP_CTRL0)	, P_Fld( 0					   , CA_LP_CTRL0_RG_ARDMSUS_10_CA	  ));
	DramcBroadcastOnOff(DRAMC_BROADCAST_ON);
}
//============================================
// SPM_control
//============================================
static void SPM_control(DRAMC_CTX_T *p,ANA_top_config_T *a_cfg)
{
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_LP_CTRL), P_Fld( 1				  , MISC_LP_CTRL_RG_ARDMSUS_10_LP_SEL			 ) \
															  | P_Fld( 1				  , MISC_LP_CTRL_RG_RIMP_DMSUS_10_LP_SEL		 ) \
															  | P_Fld( 1				  , MISC_LP_CTRL_RG_RRESETB_LP_SEL				 ) \
															  | P_Fld( 1				  , MISC_LP_CTRL_RG_RPHYPLL_RESETB_LP_SEL		 ) \
															  | P_Fld( 1				  , MISC_LP_CTRL_RG_RPHYPLL_EN_LP_SEL			 ) \
															  | P_Fld( 1				  , MISC_LP_CTRL_RG_RCLRPLL_EN_LP_SEL			 ) \
															  | P_Fld( 1				  , MISC_LP_CTRL_RG_RPHYPLL_ADA_MCK8X_EN_LP_SEL  ) \
															  | P_Fld( 1				  , MISC_LP_CTRL_RG_RPHYPLL_AD_MCK8X_EN_LP_SEL	 ) \
															  | P_Fld( 1				  , MISC_LP_CTRL_RG_RPHYPLL_TOP_REV_0_LP_SEL	 ) \
															  | P_Fld( a_cfg->NEW_8X_MODE , MISC_LP_CTRL_RG_SC_ARPI_RESETB_8X_SEQ_LP_SEL ) \
															  | P_Fld( a_cfg->NEW_8X_MODE , MISC_LP_CTRL_RG_ADA_MCK8X_8X_SEQ_LP_SEL 	 ) \
															  | P_Fld( a_cfg->NEW_8X_MODE , MISC_LP_CTRL_RG_AD_MCK8X_8X_SEQ_LP_SEL		 ) \
															  | P_Fld( a_cfg->NEW_8X_MODE , MISC_LP_CTRL_RG_MIDPI_EN_8X_SEQ_LP_SEL		 ) \
															  | P_Fld( a_cfg->NEW_8X_MODE , MISC_LP_CTRL_RG_MIDPI_CKDIV4_EN_8X_SEQ_LP_SEL) \
															  | P_Fld( a_cfg->NEW_8X_MODE , MISC_LP_CTRL_RG_MCK8X_CG_SRC_LP_SEL 		 ) \
															  | P_Fld( a_cfg->NEW_8X_MODE , MISC_LP_CTRL_RG_MCK8X_CG_SRC_AND_LP_SEL 	 ));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_LP_CTRL0) , P_Fld( 1				  , B0_LP_CTRL0_RG_ARDMSUS_10_B0_LP_SEL 		 ) \
															  | P_Fld( 1				  , B0_LP_CTRL0_RG_ARDQ_RESETB_B0_LP_SEL		 ) \
															  | P_Fld( 1				  , B0_LP_CTRL0_RG_ARPI_RESETB_B0_LP_SEL		 ) \
															  | P_Fld( 0				  , B0_LP_CTRL0_RG_B0_MS_SLV_LP_SEL 			 ) \
															  | P_Fld( 1				  , B0_LP_CTRL0_RG_ARDLL_PHDET_EN_B0_LP_SEL 	 ) \
															  | P_Fld( 0				  , B0_LP_CTRL0_RG_RX_ARDQ_BIAS_EN_B0_LP_SEL	 ) \
															  | P_Fld( 1				  , B0_LP_CTRL0_DA_ARPI_CG_MCK_B0_LP_SEL		 ) \
															  | P_Fld( 1				  , B0_LP_CTRL0_DA_ARPI_CG_MCK_FB2DLL_B0_LP_SEL  ) \
															  | P_Fld( 1				  , B0_LP_CTRL0_DA_ARPI_CG_MCTL_B0_LP_SEL		 ) \
															  | P_Fld( 1				  , B0_LP_CTRL0_DA_ARPI_CG_FB_B0_LP_SEL 		 ) \
															  | P_Fld( 1				  , B0_LP_CTRL0_DA_ARPI_CG_DQ_B0_LP_SEL 		 ) \
															  | P_Fld( 1				  , B0_LP_CTRL0_DA_ARPI_CG_DQM_B0_LP_SEL		 ) \
															  | P_Fld( 1				  , B0_LP_CTRL0_DA_ARPI_CG_DQS_B0_LP_SEL		 ) \
															  | P_Fld( 1				  , B0_LP_CTRL0_DA_ARPI_CG_DQSIEN_B0_LP_SEL 	 ) \
															  | P_Fld( 1				  , B0_LP_CTRL0_DA_ARPI_MPDIV_CG_B0_LP_SEL		 ) \
															  | P_Fld( 1				  , B0_LP_CTRL0_RG_RX_ARDQ_VREF_EN_B0_LP_SEL	 ) \
															  | P_Fld( 1				  , B0_LP_CTRL0_DA_ARPI_MIDPI_EN_B0_LP_SEL		 ) \
															  | P_Fld( 1				  , B0_LP_CTRL0_DA_ARPI_MIDPI_CKDIV4_EN_B0_LP_SEL));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_LP_CTRL0) , P_Fld( 1				  , B1_LP_CTRL0_RG_ARDMSUS_10_B1_LP_SEL 		 ) \
															  | P_Fld( 1				  , B1_LP_CTRL0_RG_ARDQ_RESETB_B1_LP_SEL		 ) \
															  | P_Fld( 1				  , B1_LP_CTRL0_RG_ARPI_RESETB_B1_LP_SEL		 ) \
															  | P_Fld( 0				  , B1_LP_CTRL0_RG_B1_MS_SLV_LP_SEL 			 ) \
															  | P_Fld( 1				  , B1_LP_CTRL0_RG_ARDLL_PHDET_EN_B1_LP_SEL 	 ) \
															  | P_Fld( 0				  , B1_LP_CTRL0_RG_RX_ARDQ_BIAS_EN_B1_LP_SEL	 ) \
															  | P_Fld( 1				  , B1_LP_CTRL0_DA_ARPI_CG_MCK_B1_LP_SEL		 ) \
															  | P_Fld( 1				  , B1_LP_CTRL0_DA_ARPI_CG_MCK_FB2DLL_B1_LP_SEL  ) \
															  | P_Fld( 1				  , B1_LP_CTRL0_DA_ARPI_CG_MCTL_B1_LP_SEL		 ) \
															  | P_Fld( 1				  , B1_LP_CTRL0_DA_ARPI_CG_FB_B1_LP_SEL 		 ) \
															  | P_Fld( 1				  , B1_LP_CTRL0_DA_ARPI_CG_DQ_B1_LP_SEL 		 ) \
															  | P_Fld( 1				  , B1_LP_CTRL0_DA_ARPI_CG_DQM_B1_LP_SEL		 ) \
															  | P_Fld( 1				  , B1_LP_CTRL0_DA_ARPI_CG_DQS_B1_LP_SEL		 ) \
															  | P_Fld( 1				  , B1_LP_CTRL0_DA_ARPI_CG_DQSIEN_B1_LP_SEL 	 ) \
															  | P_Fld( 1				  , B1_LP_CTRL0_DA_ARPI_MPDIV_CG_B1_LP_SEL		 ) \
															  | P_Fld( 1				  , B1_LP_CTRL0_RG_RX_ARDQ_VREF_EN_B1_LP_SEL	 ) \
															  | P_Fld( 1				  , B1_LP_CTRL0_DA_ARPI_MIDPI_EN_B1_LP_SEL		 ) \
															  | P_Fld( 1				  , B1_LP_CTRL0_DA_ARPI_MIDPI_CKDIV4_EN_B1_LP_SEL));
	DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
	vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_CA_LP_CTRL0) , P_Fld( 1				  , CA_LP_CTRL0_RG_ARDMSUS_10_CA_LP_SEL 		 ) \
															  | P_Fld( 1				  , CA_LP_CTRL0_RG_ARCMD_RESETB_LP_SEL			 ) \
															  | P_Fld( 1				  , CA_LP_CTRL0_RG_ARPI_RESETB_CA_LP_SEL		 ) \
							  //							  | P_Fld( 1				  , CA_LP_CTRL0_RG_CA_MS_SLV_LP_SEL)
															  | P_Fld( 1				  , CA_LP_CTRL0_RG_ARDLL_PHDET_EN_CA_LP_SEL 	 ) \
															  | P_Fld( 1				  , CA_LP_CTRL0_RG_TX_ARCS_PULL_UP_LP_SEL		 ) \
															  | P_Fld( 1				  , CA_LP_CTRL0_RG_TX_ARCS_PULL_DN_LP_SEL		 ) \
															  | P_Fld( 1				  , CA_LP_CTRL0_RG_TX_ARCA_PULL_UP_LP_SEL		 ) \
															  | P_Fld( 1				  , CA_LP_CTRL0_RG_TX_ARCA_PULL_DN_LP_SEL		 ) \
															  | P_Fld( 1				  , CA_LP_CTRL0_DA_ARPI_CG_MCK_CA_LP_SEL		 ) \
															  | P_Fld( 1				  , CA_LP_CTRL0_DA_ARPI_CG_MCK_FB2DLL_CA_LP_SEL  ) \
															  | P_Fld( 1				  , CA_LP_CTRL0_DA_ARPI_CG_MCTL_CA_LP_SEL		 ) \
															  | P_Fld( 1				  , CA_LP_CTRL0_DA_ARPI_CG_FB_CA_LP_SEL 		 ) \
															  | P_Fld( 1				  , CA_LP_CTRL0_DA_ARPI_CG_CS_LP_SEL			 ) \
															  | P_Fld( 1				  , CA_LP_CTRL0_DA_ARPI_CG_CLK_LP_SEL			 ) \
															  | P_Fld( 1				  , CA_LP_CTRL0_DA_ARPI_CG_CMD_LP_SEL			 ) \
															  | P_Fld( 1				  , CA_LP_CTRL0_DA_ARPI_CG_CLKIEN_LP_SEL		 ) \
															  | P_Fld( 1				  , CA_LP_CTRL0_DA_ARPI_MPDIV_CG_CA_LP_SEL		 ) \
															  | P_Fld( 1				  , CA_LP_CTRL0_RG_RX_ARCMD_VREF_EN_LP_SEL		 ) \
															  | P_Fld( 1				  , CA_LP_CTRL0_DA_ARPI_MIDPI_EN_CA_LP_SEL		 ) \
															  | P_Fld( 1				  , CA_LP_CTRL0_DA_ARPI_MIDPI_CKDIV4_EN_CA_LP_SEL) \
															  | P_Fld( 0 , CA_LP_CTRL0_RG_RX_ARCMD_BIAS_EN_LP_SEL		)); //use CA as DQ set 1
	DramcBroadcastOnOff(DRAMC_BROADCAST_ON);

	if(a_cfg->DLL_ASYNC_EN == 1)
	{
		DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
		vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_CA_LP_CTRL0), P_Fld((a_cfg->ALL_SLAVE_EN==0), CA_LP_CTRL0_RG_CA_MS_SLV_LP_SEL		 ));
		DramcBroadcastOnOff(DRAMC_BROADCAST_ON);
	}
	else
	{
		DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_CA_LP_CTRL0), P_Fld((a_cfg->ALL_SLAVE_EN==0), CA_LP_CTRL0_RG_CA_MS_SLV_LP_SEL		 ));
		vSetPHY2ChannelMapping(p, CHANNEL_B);
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_CA_LP_CTRL0), P_Fld(0				  , CA_LP_CTRL0_RG_CA_MS_SLV_LP_SEL 	  ));
		#if (CHANNEL_NUM>2)
		vSetPHY2ChannelMapping(p, CHANNEL_C);
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_CA_LP_CTRL0), P_Fld((a_cfg->ALL_SLAVE_EN==0), CA_LP_CTRL0_RG_CA_MS_SLV_LP_SEL		 ));
		vSetPHY2ChannelMapping(p, CHANNEL_D);
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_CA_LP_CTRL0), P_Fld(0				  , CA_LP_CTRL0_RG_CA_MS_SLV_LP_SEL 	  ));
		#endif
		vSetPHY2ChannelMapping(p, CHANNEL_A);
		DramcBroadcastOnOff(DRAMC_BROADCAST_ON);
	}

	//FOR DDR400 OPEN-LOOP MODE
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CG_CTRL9), P_Fld( 1			, MISC_CG_CTRL9_RG_M_CK_OPENLOOP_MODE_EN	  ) \
															   | P_Fld( 1			, MISC_CG_CTRL9_RG_MCK4X_I_OPENLOOP_MODE_EN   ) \
															   | P_Fld( 1			, MISC_CG_CTRL9_RG_CG_DDR400_MCK4X_I_OFF	  ) \
															   | P_Fld( 0			, MISC_CG_CTRL9_RG_DDR400_MCK4X_I_FORCE_ON	  ) \
															   | P_Fld( 1			, MISC_CG_CTRL9_RG_MCK4X_I_FB_CK_CG_OFF 	  ) \
															   | P_Fld( 1			, MISC_CG_CTRL9_RG_MCK4X_Q_OPENLOOP_MODE_EN   ) \
															   | P_Fld( 1			, MISC_CG_CTRL9_RG_CG_DDR400_MCK4X_Q_OFF	  ) \
															   | P_Fld( 0			, MISC_CG_CTRL9_RG_DDR400_MCK4X_Q_FORCE_ON	  ) \
															   | P_Fld( 1			, MISC_CG_CTRL9_RG_MCK4X_Q_FB_CK_CG_OFF 	  ));
#if 0 // @Darren-, new APHY remove 45/135 phases
															   | P_Fld( 1			, MISC_CG_CTRL9_RG_MCK4X_O_OPENLOOP_MODE_EN   ) \
															   | P_Fld( 1			, MISC_CG_CTRL9_RG_CG_DDR400_MCK4X_O_OFF	  ) \
															   | P_Fld( 0			, MISC_CG_CTRL9_RG_DDR400_MCK4X_O_FORCE_ON	  ) \
															   | P_Fld( 1			, MISC_CG_CTRL9_RG_MCK4X_O_FB_CK_CG_OFF 	  ));
#endif
}

static void DIG_DCM_nonshuffle_config(DRAMC_CTX_T *p)
{
	//RX DCM
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_RX_CG_CTRL), P_Fld(3		, MISC_SHU_RX_CG_CTRL_RX_DCM_EXT_DLY		  ));
}

static void DIG_PHY_SHU_MISC_CG_CTRL(DRAMC_CTX_T *p)
{
	 //bit 0	   : DPHY_NAO_GLUE_B0.mck_dq_cg_ctrl
	 //bit 1	   : DPHY_NAO_GLUE_B1.mck_dq_cg_ctrl
	 //bit 2	   : DPHY_NAO_GLUE_CA.mck_ca_cg_ctrl
	 //bit 4	   : DPHY_NAO_GLUE_B0.rx_mck_dq_cg_ctrl
	 //bit 5	   : DPHY_NAO_GLUE_B1.rx_mck_dq_cg_ctrl
	 //bit 6	   : DPHY_NAO_GLUE_CA.rx_mck_ca_cg_ctrl
	 //bit [9 : 8] : DPHY_TX_BRIDGE_GLUE.ddrphy_idle
	 //bit [11:10] : DPHY_TX_BRIDGE_GLUE.ddrphy_idle_dq
	 //bit [13:12] : DPHY_TX_BRIDGE_GLUE.ddrphy_idle_tx_cmd
	 //bit [17:16] : DPHY_TX_BRIDGE_GLUE.ddrphy_idle_tx_b0
	 //bit [19:18] : DPHY_TX_BRIDGE_GLUE.ddrphy_idle_tx_b1
	 //bit [22:20] : DPHY_TX_BRIDGE_GLUE.ddrphy_idle_rx_cmd
	 //bit [26:24] : DPHY_TX_BRIDGE_GLUE.ddrphy_idle_rx_b0
	 //bit [30:28] : DPHY_TX_BRIDGE_GLUE.ddrphy_idle_rx_b1

//	   vIO32Write4B 	 (DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_CG_CTRL0), 0x333f3f00);
//	   //1. ignore NAO_GLUE cg ctrl,
//	   2.00:ddrphy_idle/_ca/b0/b1 01: ddrphy_idle_shuopt 10: ddrphy_idle_shuopt_pinmux
	 vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_CG_CTRL0), 0x33400000);//rx_cmd_idle tie 1 others DCM control depend on CA B0 B1 independtly -- could save more power
}

static void ANA_IMP_configure(DRAMC_CTX_T *p)
{
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_IMP_CTRL1)  , P_Fld( 0, MISC_IMP_CTRL1_RG_RIMP_DDR3_SEL					  ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_IMP_CTRL1)  , P_Fld( !(LPDDR5_EN_S), MISC_IMP_CTRL1_RG_RIMP_DDR4_SEL	 ));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_IMP_CTRL1)  , P_Fld( 0, MISC_IMP_CTRL1_RG_RIMP_BIAS_EN					  ) \
																  | P_Fld( 0, MISC_IMP_CTRL1_RG_RIMP_ODT_EN 					  ) \
																  | P_Fld( 0, MISC_IMP_CTRL1_RG_RIMP_PRE_EN 					  ) \
																  | P_Fld( 0, MISC_IMP_CTRL1_RG_RIMP_VREF_EN					  ));
}


static void ANA_CLOCK_SWITCH(DRAMC_CTX_T *p)
{
	//OPENLOOP MODE. w_chg_mem_mck1x
	if(A_D->DQ_CA_OPEN)
	{
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CG_CTRL0) 	 , P_Fld(  1				, MISC_CG_CTRL0_RESERVED_MISC_CG_CTRL0_BIT3_1));
		mcDELAY_XNS(100);
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CG_CTRL0) 	 , P_Fld(  0				, MISC_CG_CTRL0_RESERVED_MISC_CG_CTRL0_BIT3_1));
	}
	//mem_sel	  _____|------------------------------------
	//w_chg_mem   ______________|------------|______________
	//BLCK		  __|---|___|---|____________|-|_|-|_|-|_|-
	//			  |<-	 26M  ->|<-  MUTE  ->|<-   MCK4X  ->|
	//before DLL enable switch feedback clock
	DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
	vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CKMUX_SEL)	 , P_Fld(  1						 , MISC_CKMUX_SEL_R_PHYCTRLDCM			) \
																	 | P_Fld(  1						 , MISC_CKMUX_SEL_R_PHYCTRLMUX			));
	DramcBroadcastOnOff(DRAMC_BROADCAST_ON);
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CG_CTRL0) 	 , P_Fld(  1						 , MISC_CG_CTRL0_CLK_MEM_SEL			) \
																	 | P_Fld(  1						 , MISC_CG_CTRL0_W_CHG_MEM				));

	mcDELAY_XNS(100);//reserve 100ns period for clock mute and latch the rising edge sync condition for BCLK
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CG_CTRL0) 	 , P_Fld(  0						 , MISC_CG_CTRL0_W_CHG_MEM				));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CG_CTRL0) 	 , P_Fld(  1						 , MISC_CG_CTRL0_RG_FREERUN_MCK_CG		));

	//after clock change, if OPEN LOOP MODE should change clock to 1x.	bit7 is RG_dvfs_clk_mem_mck1x_sel
	if(A_D->DQ_CA_OPEN)
	{
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CG_CTRL0) 	 , P_Fld(  1				, MISC_CG_CTRL0_RESERVED_MISC_CG_CTRL0_BIT7));
	}
}

static void ANA_Config_nonshuffle(DRAMC_CTX_T *p,ANA_top_config_T *a_cfg)
{
	#if !SA_CONFIG_EN
	RESETB_PULL_DN(p);
	#endif
	SUSPEND_ON(p);
	SPM_control(p,a_cfg);
	ANA_TX_nonshuffle_config(p,a_cfg);
	ANA_RX_nonshuffle_config(p);
	DIG_DCM_nonshuffle_config(p);
	ANA_IMP_configure(p);
	ANA_DLL_non_shuffle_config(p,a_cfg);
}

void ANA_Config_shuffle(DRAMC_CTX_T *p,ANA_top_config_T *a_cfg,U8 group_id)
{
	ANA_PLL_shuffle_Config(p,A_D->PLL_FREQ,DFS(group_id)->data_rate);
	ANA_ARPI_shuffle_config(p,a_cfg,A_D);
	ANA_TX_shuffle_config(p,a_cfg,group_id);
	ANA_RX_shuffle_config(p,group_id);
	DIG_PHY_SHU_MISC_CG_CTRL(p);
	ANA_CLK_DIV_config_setting(p,A_D,a_cfg);
	ANA_DLL_shuffle_Config(p,a_cfg);
//	  ANA_sequence_shuffle_colletion(p,&ana_core_p);
}

static void ANA_PHY_Config(DRAMC_CTX_T *p,ANA_top_config_T *a_cfg)
{
	//RESET MD32
	//vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_MD32_REG_SSPM_CFGREG_SW_RSTN), 32'h1000_0001 );

//	  SC_DPY_MODE_SW(PULL_UP);
	ANA_Config_nonshuffle(p,a_cfg);
	ANA_Config_shuffle(p,a_cfg,0);
}


static void ANA_PLL_sequence(DRAMC_CTX_T *p)
{
	msg("[ANA_INIT] PLL >>>>>>>> \n");
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_PHYPLL2)	 , P_Fld(1, PHYPLL2_RG_RPHYPLL_RESETB				 ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_PHYPLL0)	 , P_Fld(1, PHYPLL0_RG_RPHYPLL_EN					 ));
	mcDELAY_XUS(20);
	msg("[ANA_INIT] PLL <<<<<<<< \n");
}

static void ANA_MIDPI_sequence(DRAMC_CTX_T *p,ANA_DVFS_CORE_T *tr)
{
	msg("[ANA_INIT] MIDPI >>>>>>>> \n");
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ1)   , 1, SHU_B0_DQ1_RG_ARPI_MIDPI_LDO_VREF_SEL_B0   );
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ1)   , 1, SHU_B1_DQ1_RG_ARPI_MIDPI_LDO_VREF_SEL_B1   );
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD1)  , 1, SHU_CA_CMD1_RG_ARPI_MIDPI_LDO_VREF_SEL_CA	);

	//ASVA 2-6
	//step1: CG high. --disable 8 phase clk output
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DLL_ARPI2)	 , P_Fld(  1		 , SHU_B0_DLL_ARPI2_RG_ARPI_MPDIV_CG_B0   ) \
																	 | P_Fld(  1		 , SHU_B0_DLL_ARPI2_RG_ARPI_CG_FB_B0	  ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DLL_ARPI2)	 , P_Fld(  1		 , SHU_B1_DLL_ARPI2_RG_ARPI_MPDIV_CG_B1   ) \
																	 | P_Fld(  1		 , SHU_B1_DLL_ARPI2_RG_ARPI_CG_FB_B1	  ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_DLL_ARPI2)	 , P_Fld(  1		 , SHU_CA_DLL_ARPI2_RG_ARPI_MPDIV_CG_CA   ) \
																	 | P_Fld(  1		 , SHU_CA_DLL_ARPI2_RG_ARPI_CG_FB_CA	  ));

	//CG
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DLL_ARPI2), P_Fld(0x1, SHU_B0_DLL_ARPI2_RG_ARPI_CG_MCK_B0)
				| P_Fld(0x1, SHU_B0_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_B0)
				| P_Fld(0x1, SHU_B0_DLL_ARPI2_RG_ARPI_CG_MCTL_B0)
				| P_Fld(0x1, SHU_B0_DLL_ARPI2_RG_ARPI_CG_FB_B0)
				| P_Fld(0x1, SHU_B0_DLL_ARPI2_RG_ARPI_CG_DQS_B0)
				| P_Fld(0x1, SHU_B0_DLL_ARPI2_RG_ARPI_CG_DQM_B0)
				| P_Fld(0x1, SHU_B0_DLL_ARPI2_RG_ARPI_CG_DQ_B0)
				| P_Fld(0x1, SHU_B0_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B0)
				| P_Fld(0x1, SHU_B0_DLL_ARPI2_RG_ARPI_MPDIV_CG_B0));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DLL_ARPI2), P_Fld(0x1, SHU_B1_DLL_ARPI2_RG_ARPI_CG_MCK_B1)
				| P_Fld(0x1, SHU_B1_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_B1)
				| P_Fld(0x1, SHU_B1_DLL_ARPI2_RG_ARPI_CG_MCTL_B1)
				| P_Fld(0x1, SHU_B1_DLL_ARPI2_RG_ARPI_CG_FB_B1)
				| P_Fld(0x1, SHU_B1_DLL_ARPI2_RG_ARPI_CG_DQS_B1)
				| P_Fld(0x1, SHU_B1_DLL_ARPI2_RG_ARPI_CG_DQM_B1)
				| P_Fld(0x1, SHU_B1_DLL_ARPI2_RG_ARPI_CG_DQ_B1)
				| P_Fld(0x1, SHU_B1_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B1)
				| P_Fld(0x1, SHU_B1_DLL_ARPI2_RG_ARPI_MPDIV_CG_B1));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_DLL_ARPI2), P_Fld(0x1, SHU_CA_DLL_ARPI2_RG_ARPI_CG_MCK_CA)
				| P_Fld(0x1, SHU_CA_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_CA)
				| P_Fld(0x1, SHU_CA_DLL_ARPI2_RG_ARPI_CG_MCTL_CA)
				| P_Fld(0x1, SHU_CA_DLL_ARPI2_RG_ARPI_CG_FB_CA)
				| P_Fld(0x1, SHU_CA_DLL_ARPI2_RG_ARPI_CG_CS)
				| P_Fld(0x1, SHU_CA_DLL_ARPI2_RG_ARPI_CG_CLK)
				| P_Fld(0x1, SHU_CA_DLL_ARPI2_RG_ARPI_CG_CMD)
				| P_Fld(0x1, SHU_CA_DLL_ARPI2_RG_ARPI_CG_CLKIEN)
				| P_Fld(0x1, SHU_CA_DLL_ARPI2_RG_ARPI_MPDIV_CG_CA));

	//step2:PLLGP_CK_SEL -- Initial no need it
	//step3: PLLCK_EN disable
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_PLL2)			 , P_Fld(  0		 , SHU_PLL2_RG_RPHYPLL_ADA_MCK8X_EN_SHU 	));  //refer to MISC_DVFSCTRL2
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_PHYPLL2)			 , P_Fld(  0		 , PHYPLL2_RG_RPHYPLL_AD_MCK8X_EN			) \
																	 | P_Fld(  0		 , PHYPLL2_RG_RPHYPLL_ADA_MCK8X_EN			));
	//step4:MIDPI_EN
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD1)		 , P_Fld( !(tr->CA_CKDIV4_EN), SHU_CA_CMD1_RG_ARPI_MIDPI_EN_CA		  ) \
																	 | P_Fld(	tr->CA_CKDIV4_EN , SHU_CA_CMD1_RG_ARPI_MIDPI_CKDIV4_EN_CA ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD1)		 , P_Fld(	tr->CA_PREDIV_EN , SHU_CA_CMD1_RG_ARPI_MIDPI_CKDIV4_PREDIV_EN_CA));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_SHU_MIDPI_CTRL)  , P_Fld( (!(tr->DQ_SEMI_OPEN))&&(!(tr->DQ_CKDIV4_EN)), B0_SHU_MIDPI_CTRL_MIDPI_ENABLE_B0	   ) \
																	 | P_Fld( (!(tr->DQ_SEMI_OPEN))&&(tr->DQ_CKDIV4_EN), B0_SHU_MIDPI_CTRL_MIDPI_DIV4_ENABLE_B0 ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_SHU_MIDPI_CTRL)  , P_Fld( (!(tr->DQ_SEMI_OPEN))&&(!(tr->DQ_CKDIV4_EN)), B1_SHU_MIDPI_CTRL_MIDPI_ENABLE_B1	   ) \
																	 | P_Fld( (!(tr->DQ_SEMI_OPEN))&&(tr->DQ_CKDIV4_EN), B1_SHU_MIDPI_CTRL_MIDPI_DIV4_ENABLE_B1 ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_CA_SHU_MIDPI_CTRL)  , P_Fld( !(tr->CA_CKDIV4_EN), CA_SHU_MIDPI_CTRL_MIDPI_ENABLE_CA	  ) \
																	 | P_Fld(	tr->CA_CKDIV4_EN , CA_SHU_MIDPI_CTRL_MIDPI_DIV4_ENABLE_CA ));
	//step5:PI_RESETB
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_CA_DLL_ARPI0)		 , P_Fld(  0		 , CA_DLL_ARPI0_RG_ARPI_RESETB_CA			));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DLL_ARPI0)		 , P_Fld(  0		 , B0_DLL_ARPI0_RG_ARPI_RESETB_B0			));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DLL_ARPI0)		 , P_Fld(  0		 , B1_DLL_ARPI0_RG_ARPI_RESETB_B1			));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_CA_DLL_ARPI0)		 , P_Fld(  1		 , CA_DLL_ARPI0_RG_ARPI_RESETB_CA			));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DLL_ARPI0)		 , P_Fld(  1		 , B0_DLL_ARPI0_RG_ARPI_RESETB_B0			));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DLL_ARPI0)		 , P_Fld(  1		 , B1_DLL_ARPI0_RG_ARPI_RESETB_B1			));
	//step6: PLLCK_EN enable
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_PLL2)			 , P_Fld(  1		 , SHU_PLL2_RG_RPHYPLL_ADA_MCK8X_EN_SHU 	));  //refer to MISC_DVFSCTRL2
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_PHYPLL2)			 , P_Fld(  1		 , PHYPLL2_RG_RPHYPLL_AD_MCK8X_EN			) \
																	 | P_Fld(  1		 , PHYPLL2_RG_RPHYPLL_ADA_MCK8X_EN			));
	//step7: release CG 8 Phase clk enable
	//CG
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DLL_ARPI2), P_Fld(0x0, SHU_B0_DLL_ARPI2_RG_ARPI_CG_MCK_B0)
				| P_Fld(0x0, SHU_B0_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_B0)
				| P_Fld(0x0, SHU_B0_DLL_ARPI2_RG_ARPI_CG_MCTL_B0)
				| P_Fld(0x0, SHU_B0_DLL_ARPI2_RG_ARPI_CG_FB_B0)
				| P_Fld(0x0, SHU_B0_DLL_ARPI2_RG_ARPI_CG_DQS_B0)
				| P_Fld(0x0, SHU_B0_DLL_ARPI2_RG_ARPI_CG_DQM_B0)
				| P_Fld(0x0, SHU_B0_DLL_ARPI2_RG_ARPI_CG_DQ_B0)
				| P_Fld(0x0, SHU_B0_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B0)
				| P_Fld(tr->DQ_SEMI_OPEN, SHU_B0_DLL_ARPI2_RG_ARPI_MPDIV_CG_B0));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DLL_ARPI2), P_Fld(0x0, SHU_B1_DLL_ARPI2_RG_ARPI_CG_MCK_B1)
				| P_Fld(0x0, SHU_B1_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_B1)
				| P_Fld(0x0, SHU_B1_DLL_ARPI2_RG_ARPI_CG_MCTL_B1)
				| P_Fld(0x0, SHU_B1_DLL_ARPI2_RG_ARPI_CG_FB_B1)
				| P_Fld(0x0, SHU_B1_DLL_ARPI2_RG_ARPI_CG_DQS_B1)
				| P_Fld(0x0, SHU_B1_DLL_ARPI2_RG_ARPI_CG_DQM_B1)
				| P_Fld(0x0, SHU_B1_DLL_ARPI2_RG_ARPI_CG_DQ_B1)
				| P_Fld(0x0, SHU_B1_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B1)
				| P_Fld(tr->DQ_SEMI_OPEN, SHU_B1_DLL_ARPI2_RG_ARPI_MPDIV_CG_B1));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_DLL_ARPI2), P_Fld(0x0, SHU_CA_DLL_ARPI2_RG_ARPI_CG_MCK_CA)
				| P_Fld(0x0, SHU_CA_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_CA)
				| P_Fld(0x0, SHU_CA_DLL_ARPI2_RG_ARPI_CG_MCTL_CA)
				| P_Fld(0x0, SHU_CA_DLL_ARPI2_RG_ARPI_CG_FB_CA)
				| P_Fld(0x0, SHU_CA_DLL_ARPI2_RG_ARPI_CG_CS)
				| P_Fld(0x0, SHU_CA_DLL_ARPI2_RG_ARPI_CG_CLK)
				| P_Fld(0x0, SHU_CA_DLL_ARPI2_RG_ARPI_CG_CMD)
				| P_Fld(0x0, SHU_CA_DLL_ARPI2_RG_ARPI_CG_CLKIEN)
				| P_Fld(0x0, SHU_CA_DLL_ARPI2_RG_ARPI_MPDIV_CG_CA));

	msg("[ANA_INIT] MIDPI <<<<<<<< \n");
}

static void ANA_DLL_sequence(DRAMC_CTX_T *p,ANA_DVFS_CORE_T *tr,ANA_top_config_T *a_cfg)
{
	U8 DLL_ASYNC_EN;
	U8 ALL_SLAVE_EN;

	DLL_ASYNC_EN = a_cfg->DLL_ASYNC_EN;
	ALL_SLAVE_EN = a_cfg->ALL_SLAVE_EN;

	msg("[ANA_INIT] DLL >>>>>>>> \n");
	//step1: DLL_RESETB
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_CA_CMD8)			 , P_Fld(  1		 , CA_CMD8_RG_ARDLL_RESETB_CA ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ8)			 , P_Fld(  1		 , B0_DQ8_RG_ARDLL_RESETB_B0  ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ8)			 , P_Fld(  1		 , B1_DQ8_RG_ARDLL_RESETB_B1  ));
	//step2: master DLL_EN
	if(ALL_SLAVE_EN == 1)
	{
		DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
		vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_DLL1)		 , P_Fld(  1				, SHU_CA_DLL1_RG_ARDLL_PHDET_EN_CA));
		DramcBroadcastOnOff(DRAMC_BROADCAST_ON);
		mcDELAY_XNS(300);
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DLL1)		 , P_Fld(!(tr->DQ_SEMI_OPEN), SHU_B0_DLL1_RG_ARDLL_PHDET_EN_B0));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DLL1)		 , P_Fld(!(tr->DQ_SEMI_OPEN), SHU_B1_DLL1_RG_ARDLL_PHDET_EN_B1));
		mcDELAY_XNS(400); //2nd DLL > 77TMCK
	}
	else
	{
		if(DLL_ASYNC_EN == 1)
		{
			DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
			vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_DLL1)		 , P_Fld(  1	 , SHU_CA_DLL1_RG_ARDLL_PHDET_EN_CA));
			DramcBroadcastOnOff(DRAMC_BROADCAST_ON);
			mcDELAY_XNS(300); //1st DLL  > 55 TMCK
		}
		else
		{
			DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_DLL1)		 , P_Fld(  1	 , SHU_CA_DLL1_RG_ARDLL_PHDET_EN_CA));
			mcDELAY_XNS(300); //1st DLL >55T MCK
			vSetPHY2ChannelMapping(p, CHANNEL_B);
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_DLL1)		 , P_Fld(  1	 , SHU_CA_DLL1_RG_ARDLL_PHDET_EN_CA));
			#if (CHANNEL_NUM>2)
			vSetPHY2ChannelMapping(p, CHANNEL_C);
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_DLL1)		 , P_Fld(  1	 , SHU_CA_DLL1_RG_ARDLL_PHDET_EN_CA));
			mcDELAY_XNS(300); //1st DLL >55T MCK
			vSetPHY2ChannelMapping(p, CHANNEL_D);
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_DLL1)		 , P_Fld(  1	 , SHU_CA_DLL1_RG_ARDLL_PHDET_EN_CA));
			#endif
			mcDELAY_XNS(300); //1st DLL >55T MCK
			vSetPHY2ChannelMapping(p, CHANNEL_A);
			DramcBroadcastOnOff(DRAMC_BROADCAST_ON);
		}
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DLL1)		 , P_Fld(!(tr->DQ_SEMI_OPEN), SHU_B0_DLL1_RG_ARDLL_PHDET_EN_B0		 ));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DLL1)		 , P_Fld(!(tr->DQ_SEMI_OPEN), SHU_B1_DLL1_RG_ARDLL_PHDET_EN_B1		 ));
		mcDELAY_XNS(400); //2nd DLL > 77TMCK
	msg("[ANA_INIT] DLL <<<<<<<< \n");
	}
}


//shuffle register for ANA initial flow control
//It is not easy for initial sequence SA/DV coding --- same register for different group.  need two different method to manage it
//1. for seqeunce
//2. for another shuffle group need to DMA to SRAM
void ANA_sequence_shuffle_colletion(DRAMC_CTX_T *p,ANA_DVFS_CORE_T *tr)
{
	//PLL
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_PLL2)			 , P_Fld(  1				 , SHU_PLL2_RG_RPHYPLL_ADA_MCK8X_EN_SHU   ));  //refer to MISC_DVFSCTRL2

	//MIDPI
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DLL_ARPI2)	 , P_Fld(  tr->DQ_SEMI_OPEN  , SHU_B0_DLL_ARPI2_RG_ARPI_MPDIV_CG_B0   ) \
																	 | P_Fld(  0				 , SHU_B0_DLL_ARPI2_RG_ARPI_CG_FB_B0	  ) \
																	 | P_Fld( tr->DQ_SEMI_OPEN	 , SHU_B0_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_B0));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DLL_ARPI2)	 , P_Fld( tr->DQ_SEMI_OPEN	 , SHU_B1_DLL_ARPI2_RG_ARPI_MPDIV_CG_B1   ) \
																	 | P_Fld(  0				 , SHU_B1_DLL_ARPI2_RG_ARPI_CG_FB_B1	  ) \
																	 | P_Fld( tr->DQ_SEMI_OPEN	 , SHU_B1_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_B1));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_DLL_ARPI2)	 , P_Fld(  0				 , SHU_CA_DLL_ARPI2_RG_ARPI_MPDIV_CG_CA   ) \
																	 | P_Fld(  0				 , SHU_CA_DLL_ARPI2_RG_ARPI_CG_FB_CA	  ) \
																	 | P_Fld( tr->DQ_CA_OPEN	 , SHU_CA_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_CA	));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD1)		 , P_Fld( !(tr->CA_CKDIV4_EN), SHU_CA_CMD1_RG_ARPI_MIDPI_EN_CA		  ) \
																	 | P_Fld(	tr->CA_CKDIV4_EN , SHU_CA_CMD1_RG_ARPI_MIDPI_CKDIV4_EN_CA ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD1)		 , P_Fld(	tr->CA_PREDIV_EN , SHU_CA_CMD1_RG_ARPI_MIDPI_CKDIV4_PREDIV_EN_CA));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_SHU_MIDPI_CTRL)  , P_Fld( !(tr->DQ_CKDIV4_EN), B0_SHU_MIDPI_CTRL_MIDPI_ENABLE_B0	  ) \
																	 | P_Fld( (!(tr->DQ_SEMI_OPEN))&&(tr->DQ_CKDIV4_EN), B0_SHU_MIDPI_CTRL_MIDPI_DIV4_ENABLE_B0 ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_SHU_MIDPI_CTRL)  , P_Fld( !(tr->DQ_CKDIV4_EN), B1_SHU_MIDPI_CTRL_MIDPI_ENABLE_B1	  ) \
																	 | P_Fld( (!(tr->DQ_SEMI_OPEN))&&(tr->DQ_CKDIV4_EN), B1_SHU_MIDPI_CTRL_MIDPI_DIV4_ENABLE_B1 ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_CA_SHU_MIDPI_CTRL)  , P_Fld( !(tr->CA_CKDIV4_EN), CA_SHU_MIDPI_CTRL_MIDPI_ENABLE_CA	  ) \
																	 | P_Fld(	tr->CA_CKDIV4_EN , CA_SHU_MIDPI_CTRL_MIDPI_DIV4_ENABLE_CA ));
	//DLL
	DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
	vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_DLL1)		 , P_Fld(  1				, SHU_CA_DLL1_RG_ARDLL_PHDET_EN_CA		  ));
	DramcBroadcastOnOff(DRAMC_BROADCAST_ON);
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DLL1)		 , P_Fld(!(tr->DQ_SEMI_OPEN), SHU_B0_DLL1_RG_ARDLL_PHDET_EN_B0		  ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DLL1)		 , P_Fld(!(tr->DQ_SEMI_OPEN), SHU_B1_DLL1_RG_ARDLL_PHDET_EN_B1		  ));
}

static void ANA_ClockOff_Sequence(DRAMC_CTX_T *p)
{
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CG_CTRL0), P_Fld(0, MISC_CG_CTRL0_CLK_MEM_SEL)
																| P_Fld(1, MISC_CG_CTRL0_W_CHG_MEM));
	mcDELAY_XNS(100);//reserve 100ns period for clock mute and latch the rising edge sync condition for BCLK
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CG_CTRL0), 0, MISC_CG_CTRL0_W_CHG_MEM);

	//DLL Off
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DLL1), 0, SHU_B0_DLL1_RG_ARDLL_PHDET_EN_B0);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DLL1), 0, SHU_B1_DLL1_RG_ARDLL_PHDET_EN_B1);
	DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
	vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_DLL1), 0, SHU_CA_DLL1_RG_ARDLL_PHDET_EN_CA);
	DramcBroadcastOnOff(DRAMC_BROADCAST_ON);

	//CG
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DLL_ARPI2), P_Fld(0x1, SHU_B0_DLL_ARPI2_RG_ARPI_CG_MCK_B0)
				| P_Fld(0x1, SHU_B0_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_B0)
				| P_Fld(0x1, SHU_B0_DLL_ARPI2_RG_ARPI_CG_MCTL_B0)
				| P_Fld(0x1, SHU_B0_DLL_ARPI2_RG_ARPI_CG_FB_B0)
				| P_Fld(0x1, SHU_B0_DLL_ARPI2_RG_ARPI_CG_DQS_B0)
				| P_Fld(0x1, SHU_B0_DLL_ARPI2_RG_ARPI_CG_DQM_B0)
				| P_Fld(0x1, SHU_B0_DLL_ARPI2_RG_ARPI_CG_DQ_B0)
				| P_Fld(0x1, SHU_B0_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B0)
				| P_Fld(0x1, SHU_B0_DLL_ARPI2_RG_ARPI_MPDIV_CG_B0));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DLL_ARPI2), P_Fld(0x1, SHU_B1_DLL_ARPI2_RG_ARPI_CG_MCK_B1)
				| P_Fld(0x1, SHU_B1_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_B1)
				| P_Fld(0x1, SHU_B1_DLL_ARPI2_RG_ARPI_CG_MCTL_B1)
				| P_Fld(0x1, SHU_B1_DLL_ARPI2_RG_ARPI_CG_FB_B1)
				| P_Fld(0x1, SHU_B1_DLL_ARPI2_RG_ARPI_CG_DQS_B1)
				| P_Fld(0x1, SHU_B1_DLL_ARPI2_RG_ARPI_CG_DQM_B1)
				| P_Fld(0x1, SHU_B1_DLL_ARPI2_RG_ARPI_CG_DQ_B1)
				| P_Fld(0x1, SHU_B1_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B1)
				| P_Fld(0x1, SHU_B1_DLL_ARPI2_RG_ARPI_MPDIV_CG_B1));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_DLL_ARPI2), P_Fld(0x1, SHU_CA_DLL_ARPI2_RG_ARPI_CG_MCK_CA)
				| P_Fld(0x1, SHU_CA_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_CA)
				| P_Fld(0x1, SHU_CA_DLL_ARPI2_RG_ARPI_CG_MCTL_CA)
				| P_Fld(0x1, SHU_CA_DLL_ARPI2_RG_ARPI_CG_FB_CA)
				| P_Fld(0x1, SHU_CA_DLL_ARPI2_RG_ARPI_CG_CS)
				| P_Fld(0x1, SHU_CA_DLL_ARPI2_RG_ARPI_CG_CLK)
				| P_Fld(0x1, SHU_CA_DLL_ARPI2_RG_ARPI_CG_CMD)
				| P_Fld(0x1, SHU_CA_DLL_ARPI2_RG_ARPI_CG_CLKIEN)
				| P_Fld(0x1, SHU_CA_DLL_ARPI2_RG_ARPI_MPDIV_CG_CA));

	//PLLCK
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_SHU_PLL2), 0, SHU_PLL2_RG_RPHYPLL_ADA_MCK8X_EN_SHU);  //refer to MISC_DVFSCTRL2
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_PHYPLL2), P_Fld(0, PHYPLL2_RG_RPHYPLL_AD_MCK8X_EN)
														| P_Fld(0, PHYPLL2_RG_RPHYPLL_ADA_MCK8X_EN));

	//PLL
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_PHYPLL0), 0, PHYPLL0_RG_RPHYPLL_EN);

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_PHYPLL2), 0, PHYPLL2_RG_RPHYPLL_RESETB);
}


static void ANA_init_sequence(DRAMC_CTX_T *p,ANA_DVFS_CORE_T *tr,ANA_top_config_T *a_cfg)
{
	msg("[ANA_INIT] flow start \n");
	ANA_PLL_sequence(p);
	ANA_MIDPI_sequence(p,tr);
	ANA_CLOCK_SWITCH(p); //clock switch supply correct FB clk. have to do this before DLL
	ANA_DLL_sequence(p,tr,a_cfg);
	msg("[ANA_INIT] flow end \n");
}

void ANA_init(DRAMC_CTX_T *p)
{
	DRAMC_SUBSYS_PRE_CONFIG(p, &DV_p);
	msg("[ANA_INIT] >>>>>>>>>>>>>> \n");
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_CA_CMD2), P_Fld(1, CA_CMD2_RG_TX_ARCMD_OE_DIS_CA)
													| P_Fld(0, CA_CMD2_RG_TX_ARCA_OE_TIE_SEL_CA)
													| P_Fld(0xff, CA_CMD2_RG_TX_ARCA_OE_TIE_EN_CA));
	ANA_ClockOff_Sequence(p);
	ANA_PHY_Config(p,A_T);
	ANA_init_sequence(p,A_D,A_T);
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_CA_CMD2), P_Fld(0, CA_CMD2_RG_TX_ARCMD_OE_DIS_CA)
											| P_Fld(1, CA_CMD2_RG_TX_ARCA_OE_TIE_SEL_CA)
											| P_Fld(0xff, CA_CMD2_RG_TX_ARCA_OE_TIE_EN_CA));
	LP4_single_end_DRAMC_post_config(p, M_LP4->LP4Y_EN);
	msg("[ANA_INIT] <<<<<<<<<<<<< \n");
}

#if 0
void DPI_ANA_init(void)
{
	mysetscope();
	ANA_init(DramcConfig);
}
#endif
