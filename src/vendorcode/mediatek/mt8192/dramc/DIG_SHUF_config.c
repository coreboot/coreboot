/* SPDX-License-Identifier: BSD-3-Clause */

#include "dramc_dv_init.h"

//====================================
//TX CA delay configuration
//------------------------------------
//Notice:
//TX config with shuffle register with all data_rate the same
//if under real IC , need to banlance the PI/Dline calibrated result
//====================================
static void DIG_CONFIG_SHUF_ALG_TXCA(DRAMC_CTX_T *p, int ch_id, int group_id)
{
	msg("[DIG_FREQ_CONFIG][TX_CA][Delay] ch_id:%2d, group_id:%2d >>>>>\n", ch_id, group_id);

	U8 backup_ch_id = p->channel;
	u8 backup_ShuRGAccessIdx = p->ShuRGAccessIdx;
	u8 TX_UI;

	TX_UI = (DFS(group_id)->data_rate<=800) ? 1: 0 ; //TODO -- LPDDR5 need	confirm

	vSetPHY2ChannelMapping(p, ch_id);

	p->ShuRGAccessIdx = (group_id == 0) ? DRAM_DFS_REG_SHU0 : DRAM_DFS_REG_SHU1;

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_CA1), P_Fld(	0  , SHU_SELPH_CA1_TXDLY_CS   )\
															  | P_Fld(	0  , SHU_SELPH_CA1_TXDLY_CKE  )\
															  | P_Fld(	0  , SHU_SELPH_CA1_TXDLY_ODT  )\
															  | P_Fld(	0  , SHU_SELPH_CA1_TXDLY_RESET)\
															  | P_Fld(	0  , SHU_SELPH_CA1_TXDLY_WE   )\
															  | P_Fld(	0  , SHU_SELPH_CA1_TXDLY_CAS  )\
															  | P_Fld(	0  , SHU_SELPH_CA1_TXDLY_RAS  )\
															  | P_Fld(	0  , SHU_SELPH_CA1_TXDLY_CS1  ));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_CA2), P_Fld(	0  , SHU_SELPH_CA2_TXDLY_BA0  )\
															  | P_Fld(	0  , SHU_SELPH_CA2_TXDLY_BA1  )\
															  | P_Fld(	0  , SHU_SELPH_CA2_TXDLY_BA2  )\
															  | P_Fld(	0  , SHU_SELPH_CA2_TXDLY_CKE1 ));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_CA3), P_Fld(	0  , SHU_SELPH_CA3_TXDLY_RA0  )\
															  | P_Fld(	0  , SHU_SELPH_CA3_TXDLY_RA1  )\
															  | P_Fld(	0  , SHU_SELPH_CA3_TXDLY_RA2  )\
															  | P_Fld(	0  , SHU_SELPH_CA3_TXDLY_RA3  )\
															  | P_Fld(	0  , SHU_SELPH_CA3_TXDLY_RA4  )\
															  | P_Fld(	0  , SHU_SELPH_CA3_TXDLY_RA5  )\
															  | P_Fld(	0  , SHU_SELPH_CA3_TXDLY_RA6  )\
															  | P_Fld(	0  , SHU_SELPH_CA3_TXDLY_RA7  ));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_CA4), P_Fld(	0  , SHU_SELPH_CA4_TXDLY_RA8  )\
															  | P_Fld(	0  , SHU_SELPH_CA4_TXDLY_RA9  )\
															  | P_Fld(	0  , SHU_SELPH_CA4_TXDLY_RA10 )\
															  | P_Fld(	0  , SHU_SELPH_CA4_TXDLY_RA11 )\
															  | P_Fld(	0  , SHU_SELPH_CA4_TXDLY_RA12 )\
															  | P_Fld(	0  , SHU_SELPH_CA4_TXDLY_RA13 )\
															  | P_Fld(	0  , SHU_SELPH_CA4_TXDLY_RA14 )\
															  | P_Fld(	0  , SHU_SELPH_CA4_TXDLY_RA15 ));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_CA5), P_Fld(	TX_UI  , SHU_SELPH_CA5_DLY_CS	  )\
															  | P_Fld(	1  , SHU_SELPH_CA5_DLY_CKE	  )\
															  | P_Fld(	0  , SHU_SELPH_CA5_DLY_ODT	  )\
															  | P_Fld(	1  , SHU_SELPH_CA5_DLY_RESET  )\
															  | P_Fld(	1  , SHU_SELPH_CA5_DLY_WE	  )\
															  | P_Fld(	1  , SHU_SELPH_CA5_DLY_CAS	  )\
															  | P_Fld(	1  , SHU_SELPH_CA5_DLY_RAS	  )\
															  | P_Fld(	TX_UI  , SHU_SELPH_CA5_DLY_CS1	  ));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_CA6), P_Fld(	1  , SHU_SELPH_CA6_DLY_BA0	  )\
															  | P_Fld(	1  , SHU_SELPH_CA6_DLY_BA1	  )\
															  | P_Fld(	1  , SHU_SELPH_CA6_DLY_BA2	  )\
															  | P_Fld(	1  , SHU_SELPH_CA6_DLY_CKE1   ));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_CA7), P_Fld(	TX_UI  , SHU_SELPH_CA7_DLY_RA0	  )\
															  | P_Fld(	TX_UI  , SHU_SELPH_CA7_DLY_RA1	  )\
															  | P_Fld(	TX_UI  , SHU_SELPH_CA7_DLY_RA2	  )\
															  | P_Fld(	TX_UI  , SHU_SELPH_CA7_DLY_RA3	  )\
															  | P_Fld(	TX_UI  , SHU_SELPH_CA7_DLY_RA4	  )\
															  | P_Fld(	TX_UI  , SHU_SELPH_CA7_DLY_RA5	  )\
															  | P_Fld(	TX_UI  , SHU_SELPH_CA7_DLY_RA6	  )\
															  | P_Fld(	TX_UI  , SHU_SELPH_CA7_DLY_RA7	  ));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_CA8), P_Fld(	TX_UI  , SHU_SELPH_CA8_DLY_RA8	  )\
															  | P_Fld(	TX_UI  , SHU_SELPH_CA8_DLY_RA9	  )\
															  | P_Fld(	TX_UI  , SHU_SELPH_CA8_DLY_RA10   )\
															  | P_Fld(	TX_UI  , SHU_SELPH_CA8_DLY_RA11   )\
															  | P_Fld(	TX_UI  , SHU_SELPH_CA8_DLY_RA12   )\
															  | P_Fld(	TX_UI  , SHU_SELPH_CA8_DLY_RA13   )\
															  | P_Fld(	TX_UI  , SHU_SELPH_CA8_DLY_RA14   )\
															  | P_Fld(	TX_UI  , SHU_SELPH_CA8_DLY_RA15   ));

	vSetPHY2ChannelMapping(p, backup_ch_id);
	p->ShuRGAccessIdx = backup_ShuRGAccessIdx;

	msg("[DIG_FREQ_CONFIG][TX_CA][Delay] ch_id:%2d, group_id:%2d <<<<<\n", ch_id, group_id);
}

//====================================
//Impdance configuration
//------------------------------------
//Notice:
//ANA result depend on calibration
//====================================
static void DIG_CONFIG_SHUF_IMP(DRAMC_CTX_T *p, int ch_id, int group_id)
{
	msg("[DIG_FREQ_CONFIG][IMPDANCE][Configuration] ch_id:%2d, group_id:%2d >>>>>\n", ch_id, group_id);
	U8	IPM_ODT_EN;
	U8	CHKCYCLE	  = 7; //200ns algrith --TODO, @Darren, fix hw imp tracking
	U8	TXDLY_CMD	  = 8; //Need algrithm support .. RL . TODO
	U8	backup_ch_id  = p->channel;
	u8	backup_ShuRGAccessIdx = p->ShuRGAccessIdx;
	vSetPHY2ChannelMapping(p, ch_id);
	p->ShuRGAccessIdx = (group_id == 0) ? DRAM_DFS_REG_SHU0 : DRAM_DFS_REG_SHU1;

	IPM_ODT_EN = (DFS(group_id)->data_rate>=2667) ? 1 : 0;
	if (DFS(group_id)->data_rate>=4266)
		TXDLY_CMD = 0xc;
	else if (DFS(group_id)->data_rate>=3733)
		TXDLY_CMD = 0xb;
	else if (DFS(group_id)->data_rate>=3200)
		TXDLY_CMD = 0xa;

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_MISC_DRVING2)			 , P_Fld(!IPM_ODT_EN		  , SHU_MISC_DRVING2_DIS_IMPCAL_ODT_EN			  ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_MISC_IMPCAL1)			 , P_Fld(CHKCYCLE			  , SHU_MISC_IMPCAL1_IMPCAL_CHKCYCLE			  )\
																			 | P_Fld(8					  , SHU_MISC_IMPCAL1_IMPCAL_CALICNT 			  )\
																			 | P_Fld(4					  , SHU_MISC_IMPCAL1_IMPCAL_CALEN_CYCLE 		  )\
																			 | P_Fld(0x40					 , SHU_MISC_IMPCAL1_IMPCALCNT					 ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD12)				 , P_Fld(IPM_ODT_EN?0x1b:0x0f , SHU_CA_CMD12_RG_RIMP_REV					  ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_IMPEDAMCE_UPD_DIS1), P_Fld(1					  , MISC_SHU_IMPEDAMCE_UPD_DIS1_CMD2_DRVP_UPD_DIS )\
																			 | P_Fld(1					  , MISC_SHU_IMPEDAMCE_UPD_DIS1_CMD2_DRVN_UPD_DIS )\
																			 | P_Fld(1					  , MISC_SHU_IMPEDAMCE_UPD_DIS1_CMD2_ODTN_UPD_DIS ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_MISC_DRVING6)			 , P_Fld(TXDLY_CMD			  , SHU_MISC_DRVING6_IMP_TXDLY_CMD				  ));

	vSetPHY2ChannelMapping(p, backup_ch_id);
	p->ShuRGAccessIdx = backup_ShuRGAccessIdx;
	msg("[DIG_FREQ_CONFIG][IMPDANCE][Configuration] ch_id:%2d, group_id:%2d <<<<<\n", ch_id, group_id);
}

//====================================
//RX input delay configuration by mode choose
//------------------------------------
//Notice:
//
//====================================
static void DIG_CONFIG_SHUF_RXINPUT(DRAMC_CTX_T *p, int ch_id, int group_id)
{
	U8 PERBYTE_TRACK_EN = 1;//TODO
	U8 DQM_TRACK_EN 	= 1;//TODO --following RD DBI
	U8 DQM_FLOW_DQ_SEL	= 3;//TODO
	U8 RX_force_upd 	= 0;//TODO

	U8 backup_ch_id = p->channel;
	u8	backup_ShuRGAccessIdx = p->ShuRGAccessIdx;
	vSetPHY2ChannelMapping(p, ch_id);
	p->ShuRGAccessIdx = (group_id == 0) ? DRAM_DFS_REG_SHU0 : DRAM_DFS_REG_SHU1;

	msg("[DIG_FREQ_CONFIG][RX_INPUT][Configuration] ch_id:%2d, group_id:%2d >>>>>\n", ch_id, group_id);
	if(RX_force_upd == 1)
	{
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ8), P_Fld(1, SHU_B0_DQ8_R_DMRXDVS_UPD_FORCE_EN_B0));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ8), P_Fld(1, SHU_B1_DQ8_R_DMRXDVS_UPD_FORCE_EN_B1));
	}
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ7)  , P_Fld(PERBYTE_TRACK_EN	, SHU_B0_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B0) \
															  | P_Fld(DQM_FLOW_DQ_SEL	, SHU_B0_DQ7_R_DMRXDVS_DQM_FLAGSEL_B0	) \
															  | P_Fld(DQM_TRACK_EN		, SHU_B0_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B0	) \
															  | P_Fld(DQM_TRACK_EN		, SHU_B0_DQ7_R_DMRXTRACK_DQM_EN_B0		));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ7)  , P_Fld(PERBYTE_TRACK_EN	, SHU_B1_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B1) \
															  | P_Fld(DQM_FLOW_DQ_SEL	, SHU_B1_DQ7_R_DMRXDVS_DQM_FLAGSEL_B1	) \
															  | P_Fld(DQM_TRACK_EN		, SHU_B1_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B1	) \
															  | P_Fld(DQM_TRACK_EN		, SHU_B1_DQ7_R_DMRXTRACK_DQM_EN_B1		));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ11) , P_Fld(1, SHU_B0_DQ11_RG_RX_ARDQ_DVS_EN_B0));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ11) , P_Fld(1, SHU_B1_DQ11_RG_RX_ARDQ_DVS_EN_B1));

	vSetPHY2ChannelMapping(p, backup_ch_id);
	p->ShuRGAccessIdx = backup_ShuRGAccessIdx;

	msg("[DIG_FREQ_CONFIG][RX_INPUT][Configuration] ch_id:%2d, group_id:%2d <<<<<\n", ch_id, group_id);
}

#if ENABLE_WDQS_MODE_2
static void WDQSMode2TxDQOE_CNT(DRAMC_CTX_T *p, U8 *u1DQOE_CNT)
{
	switch (p->frequency)
	{
		case 1866:
			*u1DQOE_CNT = 6;
			break;
		case 1600:
		case 1200:
		case 800:
		case 600:
			*u1DQOE_CNT = 5;
			break;
		case 933:
			*u1DQOE_CNT = 4;
			break;
		case 400:
			*u1DQOE_CNT = 9;
			break;
		default:
			err("[WDQSMode2TxDQOE_CNT] frequency err!\n");
			#if __ETT__
			while (1);
			#endif
	}
}
#endif

//====================================
// MISC shuffle register fit
//------------------------------------
//Notice:
//	 MISC shuffle reigster should be fixed
//====================================
static void DIG_CONFIG_SHUF_MISC_FIX(DRAMC_CTX_T *p,U32 ch_id, U32 group_id)
{
	U8 PICG_MODE	 = 1;
	U8 LP5_HEFF 	 = 0;//TODO
	U8 LP5WRAPEN	 = 1;//Could random 1bit
	U8 DQSIEN_DQSSTB_MODE=0;
	U8 irank		 = 0;
	U8 LP5_CASMODE	 = 1; //TODO  Impact AC timing	1,2,3 three mode support  1:Low Power; 2:Low Freq; 3:High Eff;
	U8 WCKDUAL		 = 0;
	U8 NEW_RANK_MODE = 1;
	U8 DUALSCHEN	 = 1;
	U8 backup_rank	 = 0;
	U8 DQOE_OPT = 0, DQOE_CNT = 0;

	#if ENABLE_WDQS_MODE_2
	DQOE_OPT = 1;
	WDQSMode2TxDQOE_CNT(p, &DQOE_CNT);
	#endif

	backup_rank = p->rank;

	msg("[DIG_SHUF_CONFIG] MISC >>>>>, group_id=%2d \n", group_id);
	if(LPDDR4_EN_S)
	{
		DUALSCHEN = (A_D->DQ_P2S_RATIO==4) ? 0 : 1;
	}
	else if (LPDDR5_EN_S)
	{
		DUALSCHEN = (A_D->CA_P2S_RATIO==2) ? 0 : 1;
	}


	switch(DFS(group_id)->DQSIEN_MODE)
	{
		case 1: {DQSIEN_DQSSTB_MODE = 1;break;}
		case 2: {DQSIEN_DQSSTB_MODE = 2;break;}
		case 3: {DQSIEN_DQSSTB_MODE = 3;break;}
		case 6: {DQSIEN_DQSSTB_MODE = 2;break;}
		case 7: {DQSIEN_DQSSTB_MODE = 3;break;}
		default: msg("[DIG_SHUF_CONFIG] Unexpected DFS(group_id)->DQSIEN_MODE=%1d input, group_id=%2d, \n",DFS(group_id)->DQSIEN_MODE, group_id);
	}

	switch(LP5_CASMODE)
	{
		case 1: {WCKDUAL=0;LP5_HEFF=0;break;}
		case 2: {WCKDUAL=1;LP5_HEFF=0;break;}
		case 3: {WCKDUAL=0;LP5_HEFF=1;break;}
		default: msg("[DIG_SHUF_CONFIG] Unexpected LP5_CASMODE(%d) input\n",LP5_CASMODE);
	}


	U8 backup_ch_id = p->channel;
	u8 backup_ShuRGAccessIdx = p->ShuRGAccessIdx;

	vSetPHY2ChannelMapping(p, ch_id);

	p->ShuRGAccessIdx = (group_id == 0) ? DRAM_DFS_REG_SHU0 : DRAM_DFS_REG_SHU1;

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_COMMON0)	   , P_Fld(  1									  , SHU_COMMON0_BL4 			  ) \
																   | P_Fld( (A_D->DQ_P2S_RATIO==8)				  , SHU_COMMON0_FREQDIV4		  ) \
																   | P_Fld( (A_D->DQ_P2S_RATIO==4)				  , SHU_COMMON0_FDIV2			  ) \
																   | P_Fld( LPDDR4_EN_S 						  , SHU_COMMON0_BC4OTF			  ) \
																   | P_Fld( !(A_D->DQ_P2S_RATIO==4) 			  , SHU_COMMON0_DM64BITEN		  ));//TODO
	if(LPDDR5_EN_S == 1)
	{
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_COMMON0)	 , P_Fld( (A_D->DQ_P2S_RATIO==16)			  , SHU_COMMON0_FREQDIV8		  ) \
																	 | P_Fld( (DFS(group_id)->data_rate>=3733)	  , SHU_COMMON0_LP5BGEN 		  ) \
																	 | P_Fld( (DFS(group_id)->data_rate<=3200)	  , SHU_COMMON0_LP5BGOTF		  ) \
																	 | P_Fld( LP5_HEFF							  , SHU_COMMON0_LP5WCKON		  ) \
																	 | P_Fld( (DFS(group_id)->data_rate>=4800)	  , SHU_COMMON0_DLE256EN		  ) \
																	 | P_Fld( LP5WRAPEN 						  , SHU_COMMON0_LP5WRAPEN		  ) \
																	 | P_Fld( LP5_HEFF							  , SHU_COMMON0_LP5HEFF_MODE	  ));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_WCKCTRL)	 , P_Fld( WCKDUAL						   , SHU_WCKCTRL_WCKDUAL		   ));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_WCKCTRL_1)	 , P_Fld( (A_D->CKR==2) 					   , SHU_WCKCTRL_1_WCKSYNC_PRE_MODE));//TODO
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_LP5_CMD)	 , P_Fld( (A_D->CA_P2S_RATIO==2)			   , SHU_LP5_CMD_LP5_CMD1TO2EN	   ));//TODO
	}

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_ACTIMING_CONF), P_Fld(	1									  , SHU_ACTIMING_CONF_TREFBWIG		) \
																  | P_Fld(	54									  , SHU_ACTIMING_CONF_SCINTV		));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_DCM_CTRL0)	  , P_Fld(	1									  , SHU_DCM_CTRL0_FASTWAKE2 		) \
																  | P_Fld(	1									  , SHU_DCM_CTRL0_FASTWAKE			));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_CONF0)		  , P_Fld(	1									  , SHU_CONF0_ADVPREEN				) \
																  | P_Fld(	63									  , SHU_CONF0_DMPGTIM				) \
																  | P_Fld(	0									  , SHU_CONF0_REFTHD				) \
																  | P_Fld(	1									  , SHU_CONF0_PBREFEN				));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_MATYPE) 	  , P_Fld(	2									  , SHU_MATYPE_MATYPE				));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_SCHEDULER)	  , P_Fld(	DUALSCHEN							  , SHU_SCHEDULER_DUALSCHEN 		));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TX_SET0)		  , P_Fld(	 1									  , TX_SET0_WPRE2T					));
	//TODO SHU_TX_SET0_WPST1P5T OVER3200 DRAM spec need 1 but in TBA should random
	//OE_EXT2UI strange rule.--TODO
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_TX_SET0)	  , P_Fld(	(A_D->DQ_P2S_RATIO==4)				  , SHU_TX_SET0_WDATRGO 			) \
																  | P_Fld(	(DFS(group_id)->data_rate>=3200)	  , SHU_TX_SET0_WPST1P5T			) \
																  | P_Fld(	 DQOE_OPT									 , SHU_TX_SET0_DQOE_OPT 		   ) \
																  | P_Fld(	 DQOE_CNT									 , SHU_TX_SET0_DQOE_CNT 		   ) \
																  | P_Fld(	LPDDR5_EN_S 						  , SHU_TX_SET0_OE_EXT2UI			) \
																  | P_Fld(	((DFS(group_id)->data_rate==1600) && (A_D->DQ_P2S_RATIO==8))?5:2, SHU_TX_SET0_TXUPD_W2R_SEL 	   )); //TODO
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_STBCAL1), P_Fld(	0x30								  , MISC_SHU_STBCAL1_STB_PI_TRACKING_RATIO) \
																  | P_Fld(	1									  , MISC_SHU_STBCAL1_STB_UPDMASK_EN ) \
																  | P_Fld(	9									  , MISC_SHU_STBCAL1_STB_UPDMASKCYC ) \
																  | P_Fld(	(DFS(group_id)->data_rate > 1600)	  , MISC_SHU_STBCAL1_DQSINCTL_PRE_SEL)); //TODO

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_STBCAL) , P_Fld(	Gat_p.GAT_TRACK_EN					  , MISC_SHU_STBCAL_STBCALEN		  ) \
																  | P_Fld(	Gat_p.GAT_TRACK_EN					  , MISC_SHU_STBCAL_STB_SELPHCALEN	  ) \
																  | P_Fld(	DQSIEN_DQSSTB_MODE					  , MISC_SHU_STBCAL_DQSIEN_DQSSTB_MODE)); //TODO

	//@Darren, NOTE: Fix gating error or fifo mismatch => DMSTBLAT date_rate=1866 >= 3 : 1
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_STBCAL) , P_Fld( (((Gat_p.GAT_TRACK_EN)&&(DFS(group_id)->data_rate>=1866))?(2+Gat_p.VALID_LAT_VALUE):(0+Gat_p.VALID_LAT_VALUE)) , MISC_SHU_STBCAL_DMSTBLAT	) \
																  | P_Fld(	 1									  , MISC_SHU_STBCAL_PICGLAT 		) \
																  | P_Fld(	 1									  , MISC_SHU_STBCAL_DQSG_MODE		) \
																  | P_Fld(	 PICG_MODE							  , MISC_SHU_STBCAL_DQSIEN_PICG_MODE));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_RANKCTL), P_Fld(	 PICG_MODE							  , MISC_SHU_RANKCTL_RANK_RXDLY_OPT ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_MISC		 ), P_Fld(	 1									  , SHU_MISC_REQQUE_MAXCNT			));
	for(irank = RANK_0; irank < p->support_rank_num; irank++)
	{
		vSetRank(p, irank);
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_RK_DQSIEN_PICG_CTRL), P_Fld(	0					  , MISC_SHU_RK_DQSIEN_PICG_CTRL_DQSIEN_PICG_HEAD_EXT_LAT) \
																				  | P_Fld(	(A_D->DQ_P2S_RATIO == 4)  , MISC_SHU_RK_DQSIEN_PICG_CTRL_DQSIEN_PICG_TAIL_EXT_LAT));
	}
	vSetRank(p, backup_rank);

	//RODT offset TODO
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_RODTENSTB), P_Fld(  1 								  , MISC_SHU_RODTENSTB_RODTENSTB_TRACK_EN		) \
																	| P_Fld(  0 								  , MISC_SHU_RODTENSTB_RODTEN_P1_ENABLE 		) \
																	| P_Fld(  (NEW_RANK_MODE)?1:PICG_MODE		  , MISC_SHU_RODTENSTB_RODTENSTB_SELPH_MODE 	) \
																	| P_Fld(  1 								  , MISC_SHU_RODTENSTB_RODTENSTB_TRACK_UDFLWCTRL) \
																	| P_Fld(  ((A_D->DQ_P2S_RATIO == 4)?2:0)	  , MISC_SHU_RODTENSTB_RODTENSTB_MCK_OFFSET 	) \
																	| P_Fld(  ((A_D->DQ_P2S_RATIO == 4)?1:4)	  , MISC_SHU_RODTENSTB_RODTENSTB__UI_OFFSET 	) \
																	| P_Fld(  ((A_D->DQ_P2S_RATIO == 16)?19:((A_D->DQ_P2S_RATIO == 8)?13:10))	 , MISC_SHU_RODTENSTB_RODTENSTB_EXT 		   ));

	//[SV] //SHL, fix RODT rd_period low 1T issue
	// @Darren, confirm MP settings w/ Chau-Wei Wang (Jason)
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_RODTENSTB1), P_Fld( ((DFS(group_id)->data_rate >=3200)?1:0)	, MISC_SHU_RODTENSTB1_RODTENCGEN_TAIL		  ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_RODTENSTB1), P_Fld( ((DFS(group_id)->data_rate >=3200)?2:1)	, MISC_SHU_RODTENSTB1_RODTENCGEN_HEAD		  ));

	switch (A_D->DQ_P2S_RATIO)
	{
		case  4:
		{
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_RX_SELPH_MODE), P_Fld( 1			, MISC_SHU_RX_SELPH_MODE_DQSIEN_SELPH_SERMODE));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_RX_SELPH_MODE), P_Fld( 0			, MISC_SHU_RX_SELPH_MODE_RODT_SELPH_SERMODE  ) \
																				| P_Fld( 0			, MISC_SHU_RX_SELPH_MODE_RANK_SELPH_SERMODE  ));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_RDAT1 	   ), P_Fld( 1			, MISC_SHU_RDAT1_R_DMRDSEL_DIV2_OPT 		 ) \
																				| P_Fld( 1			, MISC_SHU_RDAT1_R_DMRDSEL_LOBYTE_OPT		 ) \
																				| P_Fld( 0			, MISC_SHU_RDAT1_R_DMRDSEL_HIBYTE_OPT		 ));
			break;
		}
		case  8:
		{
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_RX_SELPH_MODE), P_Fld( 2			, MISC_SHU_RX_SELPH_MODE_DQSIEN_SELPH_SERMODE));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_RX_SELPH_MODE), P_Fld( 1			, MISC_SHU_RX_SELPH_MODE_RODT_SELPH_SERMODE  ) \
																				| P_Fld( 1			, MISC_SHU_RX_SELPH_MODE_RANK_SELPH_SERMODE  ));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_RDAT1 	   ), P_Fld( 0			, MISC_SHU_RDAT1_R_DMRDSEL_DIV2_OPT 		 ) \
																				| P_Fld( 0			, MISC_SHU_RDAT1_R_DMRDSEL_LOBYTE_OPT		 ) \
																				| P_Fld( 0			, MISC_SHU_RDAT1_R_DMRDSEL_HIBYTE_OPT		 ));
			break;
		}
		case  16:
		{
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_RX_SELPH_MODE), P_Fld( 3			, MISC_SHU_RX_SELPH_MODE_DQSIEN_SELPH_SERMODE));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_RX_SELPH_MODE), P_Fld( 2			, MISC_SHU_RX_SELPH_MODE_RODT_SELPH_SERMODE  ) \
																				| P_Fld( 2			, MISC_SHU_RX_SELPH_MODE_RANK_SELPH_SERMODE  ));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_RDAT1 	   ), P_Fld( 0			, MISC_SHU_RDAT1_R_DMRDSEL_DIV2_OPT 		 ) \
																				| P_Fld( 0			, MISC_SHU_RDAT1_R_DMRDSEL_LOBYTE_OPT		 ) \
																				| P_Fld( 0			, MISC_SHU_RDAT1_R_DMRDSEL_HIBYTE_OPT		 ));
			break;
		}
		default:msg("ERROR:Unexcepted A_D.DQ_P2S_RATIO = %2d \n", A_D->DQ_P2S_RATIO);
	}
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHURK_CKE_CTRL) 				, P_Fld(  0 		, SHURK_CKE_CTRL_CKE_DBE_CNT				 ));

	vSetPHY2ChannelMapping(p, backup_ch_id);
	p->ShuRGAccessIdx = backup_ShuRGAccessIdx;

	msg("[DIG_SHUF_CONFIG] MISC <<<<<<, group_id=%2d \n", group_id);
}

static void DIG_CONFIG_SHUF_DQSGRETRY(DRAMC_CTX_T *p, int ch_id, int group_id)
{
	U8 backup_ch_id = p->channel;
	u8 backup_ShuRGAccessIdx = p->ShuRGAccessIdx;

	vSetPHY2ChannelMapping(p, ch_id);
	msg("[DIG_SHUF_CONFIG] DQSG_RETRY >>>>>>, group_id=%2d \n", group_id);
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_DQSG_RETRY1), P_Fld( 0								, MISC_SHU_DQSG_RETRY1_RETRY_SW_RESET		 ) \
																	  | P_Fld( 0								, MISC_SHU_DQSG_RETRY1_RETRY_SW_EN			 ) \
																	  | P_Fld( (DFS(group_id)->data_rate>=3733) , MISC_SHU_DQSG_RETRY1_RETRY_DDR1866_PLUS	 ) \
																	  | P_Fld( 0								, MISC_SHU_DQSG_RETRY1_RETRY_ONCE			 ) \
																	  | P_Fld( (DFS(group_id)->data_rate>=3733) , MISC_SHU_DQSG_RETRY1_RETRY_3TIMES 		 ) \
																	  | P_Fld( 0								, MISC_SHU_DQSG_RETRY1_RETRY_1RANK			 ) \
																	  | P_Fld( (DFS(group_id)->data_rate>=3733) , MISC_SHU_DQSG_RETRY1_RETRY_BY_RANK		 ) \
																	  | P_Fld( 0								, MISC_SHU_DQSG_RETRY1_RETRY_DM4BYTE		 ) \
																	  | P_Fld( 0								, MISC_SHU_DQSG_RETRY1_RETRY_DQSIENLAT		 ) \
																	  | P_Fld( 0								, MISC_SHU_DQSG_RETRY1_RETRY_STBENCMP_ALLBYTE) \
																	  | P_Fld( 0								, MISC_SHU_DQSG_RETRY1_XSR_DQSG_RETRY_EN	 ) \
																	  | P_Fld( 0 /*@Darren, sync MP settings by YT (DFS(group_id)->data_rate>=3733)*/ , MISC_SHU_DQSG_RETRY1_XSR_RETRY_SPM_MODE    ) \
																	  | P_Fld( 0								, MISC_SHU_DQSG_RETRY1_RETRY_CMP_DATA		 ) \
																	  | P_Fld( 0								, MISC_SHU_DQSG_RETRY1_RETRY_ALE_BLOCK_MASK  ) \
																	  | P_Fld( (DFS(group_id)->data_rate>=3733) , MISC_SHU_DQSG_RETRY1_RETRY_RDY_SEL_DLE	 ) \
																	  | P_Fld( (DFS(group_id)->data_rate>=3733) , MISC_SHU_DQSG_RETRY1_RETRY_USE_NON_EXTEND  ) \
																	  | P_Fld( (DFS(group_id)->data_rate>=3733) , MISC_SHU_DQSG_RETRY1_RETRY_USE_CG_GATING	 ) \
																	  | P_Fld( 1								, MISC_SHU_DQSG_RETRY1_RETRY_ROUND_NUM		 ) \
																	  | P_Fld( 0								, MISC_SHU_DQSG_RETRY1_RETRY_RANKSEL_FROM_PHY) \
																	  | P_Fld( 0								, MISC_SHU_DQSG_RETRY1_RETRY_PA_DISABLE 	 ) \
																	  | P_Fld( 0								, MISC_SHU_DQSG_RETRY1_RETRY_STBEN_RESET_MSK ) \
																	  | P_Fld( (DFS(group_id)->data_rate>=3733) , MISC_SHU_DQSG_RETRY1_RETRY_USE_BURST_MODE  ));

	vSetPHY2ChannelMapping(p, backup_ch_id);
	p->ShuRGAccessIdx = backup_ShuRGAccessIdx;
	msg("[DIG_SHUF_CONFIG] DQSG_RETRY <<<<<<, group_id=%2d \n", group_id);
}

static void DIG_CONFIG_SHUF_DBI(DRAMC_CTX_T *p, int ch_id, int group_id)
{
	U8 RD_DBI_EN = 1;//TODO
	U8 WR_DBI_EN = 1;//TODO

	LP4_DRAM_CONFIG_T LP4_temp;
	LP5_DRAM_CONFIG_T LP5_temp;

	U8 backup_ch_id = p->channel;
	u8 backup_ShuRGAccessIdx = p->ShuRGAccessIdx;

	msg("[DIG_SHUF_CONFIG] DBI >>>>>>, group_id=%2d \n",  group_id);
	if(LPDDR4_EN_S)
	{
		LP4_DRAM_config(DFS(group_id)->data_rate,&LP4_temp);
		RD_DBI_EN = LP4_temp.DBI_RD;
		WR_DBI_EN = LP4_temp.DBI_WR;
	}
	else
	{//TODO LPDDR5 and other dram type not ready
		LP5_DRAM_config(DFS(group_id),&LP5_temp);
		RD_DBI_EN = LP5_temp.DBI_RD;
		WR_DBI_EN = LP5_temp.DBI_WR;
	}


	vSetPHY2ChannelMapping(p, ch_id);
	p->ShuRGAccessIdx = (group_id == 0) ? DRAM_DFS_REG_SHU0 : DRAM_DFS_REG_SHU1;

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ7), P_Fld(RD_DBI_EN, SHU_B0_DQ7_R_DMDQMDBI_SHU_B0    ) \
															| P_Fld(RD_DBI_EN, SHU_B0_DQ7_R_DMDQMDBI_EYE_SHU_B0));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ7), P_Fld(RD_DBI_EN, SHU_B1_DQ7_R_DMDQMDBI_SHU_B1    ) \
															| P_Fld(RD_DBI_EN, SHU_B1_DQ7_R_DMDQMDBI_EYE_SHU_B1));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_TX_SET0), P_Fld(WR_DBI_EN, SHU_TX_SET0_DBIWR			   ));

	vSetPHY2ChannelMapping(p, backup_ch_id);
	p->ShuRGAccessIdx = backup_ShuRGAccessIdx;
	msg("[DIG_SHUF_CONFIG] DBI <<<<<<, group_id=%2d \n",  group_id);
}

//TODO LPDDR5
static void DIG_CONFIG_SHUF_DVFSWLRL(DRAMC_CTX_T *p, int ch_id, int group_id)
{
	U8 backup_ch_id = p->channel;
	u8 backup_ShuRGAccessIdx = p->ShuRGAccessIdx;

	LP4_DRAM_CONFIG_T LP4_temp;

	U8	HWSET_MR13_OP_Value   =0;
	U8	HWSET_VRCG_OP_Value   =0;
	U8	HWSET_MR2_OP_Value	  =0;

	msg("[DIG_SHUF_CONFIG] DVFSRLWL >>>>>>, group_id=%2d \n",  group_id);
	p->ShuRGAccessIdx = (group_id == 0) ? DRAM_DFS_REG_SHU0 : DRAM_DFS_REG_SHU1;

	if(LPDDR4_EN_S)
	{
		LP4_DRAM_config (DFS(group_id)->data_rate,&LP4_temp);

		HWSET_MR13_OP_Value = ((LP4_temp.WORK_FSP & 1) << 7) | ((LP4_temp.WORK_FSP & 1) << 6) | (( 0 << 5) | 8); //DMI default enable
		HWSET_VRCG_OP_Value = ((LP4_temp.WORK_FSP & 1) << 7) | ((LP4_temp.WORK_FSP & 1) << 6);
		HWSET_MR2_OP_Value	= ((LP4_temp.MR_WL & 7) << 3) | (LP4_temp.MR_WL & 7);
	} else {
		msg("[DIG_SHUF_CONFIG] LPDDR5 have to use Run-time MRW to support DVFS! Do not Use HWSET_MR serial Registers.");
	}

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_HWSET_MR13), P_Fld(HWSET_MR13_OP_Value, SHU_HWSET_MR13_HWSET_MR13_OP ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_HWSET_VRCG), P_Fld(HWSET_VRCG_OP_Value, SHU_HWSET_VRCG_HWSET_VRCG_OP ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_HWSET_VRCG), P_Fld(0xb				  , SHU_HWSET_VRCG_VRCGDIS_PRDCNT));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_HWSET_MR2 ), P_Fld(HWSET_MR2_OP_Value , SHU_HWSET_MR2_HWSET_MR2_OP	 ));
	vSetPHY2ChannelMapping(p, backup_ch_id);
	p->ShuRGAccessIdx = backup_ShuRGAccessIdx;

	msg("[test_sa.c]====>ch_id:%2d, group_id:%2d, DPI_TBA_DVFS_WLRL_setting Exit\n", ch_id, group_id);
}

//=================================================
//Jump ratio calculate and setting
//------------------------------------------------
//notice: 400 800 not support tracking TODO
//	should confirm it with DQ_SEMI_OPEN =1 or not but not data_rate as condition
//
//================================================
#if 0
void TX_RX_jumpratio_calculate(DRAMC_CTX_T *p,int ch_id,int group_id)
{
	int tar;
	int ratio = 32;
	int result[DFS_GROUP_NUM];

	U8 backup_ch_id = p->channel;
	u8 backup_ShuRGAccessIdx = p->ShuRGAccessIdx;

	vSetPHY2ChannelMapping(p, ch_id);
	msg("[TX_RX_jumpratio_calculate]>>>>>>>> group_id = %1d",group_id);
	for(tar = 0; tar<DFS_GROUP_NUM;tar++)
	{
		if(((DFS(group_id)->data_rate == 800) || (DFS(group_id)->data_rate == 400)) || ((DFS(tar)->data_rate == 800) || (DFS(tar)->data_rate == 400))) //TODO wihtout tracking
		{
			result[tar] = 0;
		}
		else
		{
			result[tar] = (int)(((float)(DFS(tar)->data_rate) * (float)ratio) / (float)(DFS(group_id)->data_rate) + 0.5); //+0.5 for roundup
		}
		msg("\n[TXRX_jumpratio]current_group data_rate=%1d,tar_data_rate=%1d,jumpratio=%1d;\n",DFS(group_id)->data_rate,DFS(tar)->data_rate,result[tar]);
	}
	//=============================
	//setting
	//=============================
	p->ShuRGAccessIdx = (group_id == 0) ? DRAM_DFS_REG_SHU0 : DRAM_DFS_REG_SHU1;

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_FREQ_RATIO_SET0), P_Fld(   result[0]  , SHU_FREQ_RATIO_SET0_TDQSCK_JUMP_RATIO0) \
																	| P_Fld(   result[1]  , SHU_FREQ_RATIO_SET0_TDQSCK_JUMP_RATIO1) \
																	| P_Fld(   result[2]  , SHU_FREQ_RATIO_SET0_TDQSCK_JUMP_RATIO2) \
																	| P_Fld(   result[3]  , SHU_FREQ_RATIO_SET0_TDQSCK_JUMP_RATIO3));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_FREQ_RATIO_SET1), P_Fld(   result[4]  , SHU_FREQ_RATIO_SET1_TDQSCK_JUMP_RATIO4) \
																	| P_Fld(   result[5]  , SHU_FREQ_RATIO_SET1_TDQSCK_JUMP_RATIO5) \
																	| P_Fld(   result[6]  , SHU_FREQ_RATIO_SET1_TDQSCK_JUMP_RATIO6) \
																	| P_Fld(   result[7]  , SHU_FREQ_RATIO_SET1_TDQSCK_JUMP_RATIO7));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_FREQ_RATIO_SET2), P_Fld(   result[8]  , SHU_FREQ_RATIO_SET2_TDQSCK_JUMP_RATIO8) \
																	| P_Fld(   result[9]  , SHU_FREQ_RATIO_SET2_TDQSCK_JUMP_RATIO9));
	vSetPHY2ChannelMapping(p, backup_ch_id);
	p->ShuRGAccessIdx = DRAM_DFS_REG_SHU0;
	msg("[TX_RX_jumpratio_calculate]<<<<<<< group_id = %1d",group_id);
}
#endif

static void DIG_CONFIG_DVFS_DEPENDENCE(DRAMC_CTX_T *p,U32 ch_id, U32 group_id)
{
	DIG_CONFIG_SHUF_DVFSWLRL(p,ch_id,group_id);
	//TX_RX_jumpratio_calculate(p,ch_id,group_id);

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_DVFSDLL ) , P_Fld((LPDDR4_EN_S==1)?0x37:0x37	 , MISC_SHU_DVFSDLL_R_DLL_IDLE		  )\
																	| P_Fld((LPDDR4_EN_S==1)?0x4d:0x37	 , MISC_SHU_DVFSDLL_R_2ND_DLL_IDLE	  )\
																	| P_Fld(   ana_top_p.ALL_SLAVE_EN	 , MISC_SHU_DVFSDLL_R_BYPASS_1ST_DLL  )\
																	| P_Fld(			 0				 , MISC_SHU_DVFSDLL_R_BYPASS_2ND_DLL  ));
}

//====================================
// Digital shuffle configuration entry
//------------------------------------
//Notice:
//
//====================================
void DIG_CONFIG_SHUF(DRAMC_CTX_T *p,U32 ch_id, U32 group_id)
{
	DIG_CONFIG_SHUF_ALG_TXCA(p,ch_id,group_id);
	DIG_CONFIG_SHUF_IMP(p,ch_id,group_id);
	DIG_CONFIG_SHUF_RXINPUT(p,ch_id,group_id);
	DIG_CONFIG_SHUF_MISC_FIX(p,ch_id,group_id);
	DIG_CONFIG_SHUF_DQSGRETRY(p,ch_id,group_id);
	DIG_CONFIG_SHUF_DBI(p,ch_id,group_id);
	DIG_CONFIG_DVFS_DEPENDENCE(p,ch_id,group_id);
}

#if 0
void DIG_CONFIG_SHUF_init(void)
{
	mysetscope();
	DIG_CONFIG_SHUF(DramcConfig,0,0); //temp ch0 group 0
}

void OTHER_GP_INIT(DRAMC_CTX_T *p,U32 ch_id, U32 group_id)
{
	U8 backup_ch_id = p->channel;
	U8 backup_ShuRGAccessIdx = p->ShuRGAccessIdx;

	//notice here.	Replace the A_D A_T with new frequency auto-generation
	ANA_TOP_FUNCTION_CFG(A_T,DFS(group_id)->data_rate);
	ANA_CLK_DIV_config(A_D,DFS(group_id));

	p->ShuRGAccessIdx = (group_id == 0) ? DRAM_DFS_REG_SHU0 : DRAM_DFS_REG_SHU1;
	ANA_sequence_shuffle_colletion(p,A_D);//these RG will be set during flow,but for DV another GP should be set directly
	ANA_Config_shuffle(p,A_T,group_id);
	DIG_CONFIG_SHUF(p,ch_id,group_id);
	vSetPHY2ChannelMapping(p, backup_ch_id);
	p->ShuRGAccessIdx = DRAM_DFS_REG_SHU0;
}

void DPI_OTHER_GP_INIT(U32 ch_id, U32 group_id)
{
	mysetscope();
	DramcBroadcastOnOff(DRAMC_BROADCAST_ON);
	OTHER_GP_INIT(DramcConfig,ch_id,group_id);
	DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
	conf_to_sram_sudo(0,group_id,1);
	conf_to_sram_sudo(1,group_id,1);
	//DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
	//vSetPHY2ChannelMapping(DramcConfig, CHANNEL_A);
	//DRAMC_DMA_CONF_to_SRAM(DramcConfig,group_id,1);
	//vSetPHY2ChannelMapping(DramcConfig, CHANNEL_B);
	//DRAMC_DMA_CONF_to_SRAM(DramcConfig,group_id,1);
	//vSetPHY2ChannelMapping(DramcConfig, CHANNEL_A);
	//DramcBroadcastOnOff(DRAMC_BROADCAST_ON);
}
#endif
