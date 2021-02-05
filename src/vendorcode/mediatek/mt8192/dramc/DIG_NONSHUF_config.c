/* SPDX-License-Identifier: BSD-3-Clause */

#include "dramc_dv_init.h"

Gating_confg_T Gat_p;

//============================================
// digital PHY config
//============================================
static void DIG_PHY_config(DRAMC_CTX_T *p)
{
#if ENABLE_PINMUX_FOR_RANK_SWAP
	U8 RK_SWAP_EN = 1;
#else
	U8 RK_SWAP_EN = 0;
#endif

	mcSHOW_DBG_MSG(("[Flow] Enable top DCM control >>>>> \n"));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CG_CTRL2) 		 , P_Fld(	3	  , MISC_CG_CTRL2_RG_MEM_DCM_IDLE_FSEL		));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CG_CTRL2) 		 , P_Fld(	0	  , MISC_CG_CTRL2_RG_MEM_DCM_APB_TOG		) \
																		 | P_Fld(	0x1f  , MISC_CG_CTRL2_RG_MEM_DCM_APB_SEL		));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CG_CTRL2) 		 , P_Fld(	1	  , MISC_CG_CTRL2_RG_MEM_DCM_APB_TOG		) \
																		 | P_Fld(	0x1f  , MISC_CG_CTRL2_RG_MEM_DCM_APB_SEL		));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CG_CTRL2) 		 , P_Fld(	0	  , MISC_CG_CTRL2_RG_MEM_DCM_APB_TOG		) \
																		 | P_Fld(	0x1f  , MISC_CG_CTRL2_RG_MEM_DCM_APB_SEL		));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CG_CTRL2) 		 , P_Fld(	0x17  , MISC_CG_CTRL2_RG_MEM_DCM_APB_SEL		) \
																		 | P_Fld(	1	  , MISC_CG_CTRL2_RG_MEM_DCM_APB_TOG		));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CTRL0)			 , P_Fld(	0	  , MISC_CTRL0_R_STBENCMP_DIV4CK_EN 		) \
																		 | P_Fld(	1	  , MISC_CTRL0_R_DQS0IEN_DIV4_CK_CG_CTRL	) \
																		 | P_Fld(	1	  , MISC_CTRL0_R_DQS1IEN_DIV4_CK_CG_CTRL	) \
																		 | P_Fld(	0	  , MISC_CTRL0_R_CLKIEN_DIV4_CK_CG_CTRL 	) \
																		 | P_Fld(	1	  , MISC_CTRL0_R_DMSHU_PHYDCM_FORCEOFF		));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_RXDVS2)			 , P_Fld(	1	  , MISC_RXDVS2_R_DMRXDVS_SHUFFLE_CTRL_CG_IG));
	mcSHOW_DBG_MSG(("[Flow] Enable top DCM control <<<<< \n"));

	mcSHOW_DBG_MSG(("Enable DLL master slave shuffle \n"));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DVFS_EMI_CLK) 	 , P_Fld(	1	  , MISC_DVFS_EMI_CLK_RG_DLL_SHUFFLE_DDRPHY ));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ9)				 , P_Fld(	1	  , B0_DQ9_R_DMRXFIFO_STBENCMP_EN_B0		));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ9)				 , P_Fld(	1	  , B1_DQ9_R_DMRXFIFO_STBENCMP_EN_B1		));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CTRL1)			 , P_Fld(RK_SWAP_EN, MISC_CTRL1_R_RK_PINMUXSWAP_EN			));

	mcDELAY_US(1);

	if(A_T->NEW_RANK_MODE==0)
	{
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ9)			 , P_Fld(	4	  , B0_DQ9_R_IN_GATE_EN_LOW_OPT_B0			) \
																		 | P_Fld(	0	  , B0_DQ9_R_DMRXDVS_R_F_DLY_RK_OPT_B0		));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ9)			 , P_Fld(	4	  , B1_DQ9_R_IN_GATE_EN_LOW_OPT_B1			) \
																		 | P_Fld(	0	  , B1_DQ9_R_DMRXDVS_R_F_DLY_RK_OPT_B1		));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ10)			 , P_Fld(	0	  , B0_DQ10_ARPI_CG_RK1_SRC_SEL_B0			));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ10)			 , P_Fld(	0	  , B1_DQ10_ARPI_CG_RK1_SRC_SEL_B1			));
	}
	else
	{
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ9)			 , P_Fld(	0	  , B0_DQ9_R_IN_GATE_EN_LOW_OPT_B0			) \
																		 | P_Fld(	1	  , B0_DQ9_R_DMRXDVS_R_F_DLY_RK_OPT_B0		));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ9)			 , P_Fld(	0	  , B1_DQ9_R_IN_GATE_EN_LOW_OPT_B1			) \
																		 | P_Fld(	1	  , B1_DQ9_R_DMRXDVS_R_F_DLY_RK_OPT_B1		));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ10)			 , P_Fld(	1	  , B0_DQ10_ARPI_CG_RK1_SRC_SEL_B0			));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ10)			 , P_Fld(	1	  , B1_DQ10_ARPI_CG_RK1_SRC_SEL_B1			));
	}

	if(A_T->NEW_8X_MODE==1)
	{
		DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
		vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DVFSCTL)		 , P_Fld(	1	  , MISC_DVFSCTL_R_SHUFFLE_PI_RESET_ENABLE	) \
																		 | P_Fld(	3	  , MISC_DVFSCTL_R_DVFS_MCK8X_MARGIN		));
		DramcBroadcastOnOff(DRAMC_BROADCAST_ON);
	}
}



static void GATING_MODE_CFG(Gating_confg_T *tr)
{
	tr->GAT_TRACK_EN		 = ((A_D->DQ_SEMI_OPEN == 1)||(A_D->DQ_CA_OPEN==1))?0:1;
	tr->RX_GATING_MODE		 = 2; //fix 7UI mode under LPDDR4
	tr->RX_GATING_TRACK_MODE = 2; //fix FIFO mode under LPDDR4
	tr->PICG_EARLY_EN		 = 1; //fix under LPDDR4, if LPDDR5 have to set 1
	tr->SELPH_MODE			 = 1; //random inside {0,1} //for improve APHY XRTR2R. NEW_APHY MODE with 1.
	tr->VALID_LAT_VALUE 	 = 1; //random inside {0,1}

	mcSHOW_DBG_MSG(("============================================================== \n"));
	mcSHOW_DBG_MSG(("Gating Mode config\n"				));
	mcSHOW_DBG_MSG(("============================================================== \n"));
	mcSHOW_DBG_MSG(("Config description: \n"));
	mcSHOW_DBG_MSG(("RX_GATING_MODE 	   0: Pulse Mode	  1: Burst Mode(8UI)		2: Burst Mode(7UI)	3: Original Burst Mode\n"));
	mcSHOW_DBG_MSG(("RX_GATING_TRACK_MODE  0: Valid DLY Mode  1: Valid Mode (-like) 2: FIFO mode\n"));
	mcSHOW_DBG_MSG(("SELPH_MODE 		   0: By rank		  1: By Phase \n"));
	mcSHOW_DBG_MSG(("============================================================== \n"));
	mcSHOW_DBG_MSG(("GAT_TRACK_EN				  = %2d\n",tr->GAT_TRACK_EN 	   ));
	mcSHOW_DBG_MSG(("RX_GATING_MODE 			  = %2d\n",tr->RX_GATING_MODE	   ));
	mcSHOW_DBG_MSG(("RX_GATING_TRACK_MODE		  = %2d\n",tr->RX_GATING_TRACK_MODE));
	mcSHOW_DBG_MSG(("SELPH_MODE 				  = %2d\n",tr->SELPH_MODE		   ));
	mcSHOW_DBG_MSG(("PICG_EARLY_EN				  = %2d\n",tr->PICG_EARLY_EN	   ));
	mcSHOW_DBG_MSG(("VALID_LAT_VALUE			  = %2d\n",tr->VALID_LAT_VALUE	   ));
	mcSHOW_DBG_MSG(("============================================================== \n"));
}

//======================================
//gating widnow mode
//======================================
static void DPHY_GAT_TRACK_Config(DRAMC_CTX_T *p,Gating_confg_T *gat_c)
{
	mcSHOW_DBG_MSG(("Enter into Gating configuration >>>> \n"));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL1)		, P_Fld(!gat_c->GAT_TRACK_EN, MISC_STBCAL1_STBCNT_SW_RST		  ));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL2)		, P_Fld(gat_c->SELPH_MODE, MISC_STBCAL2_DQSIEN_SELPH_BY_RANK_EN));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL1)		, P_Fld(   1	  , MISC_STBCAL1_STBCNT_SHU_RST_EN		));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL1)		, P_Fld(   1	  , MISC_STBCAL1_DIS_PI_TRACK_AS_NOT_RD ));

	if(gat_c->PICG_EARLY_EN == 1)
	{
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ6)		  , P_Fld(	 1		  , B0_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B0));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ6)		  , P_Fld(	 1		  , B1_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B1));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL2)	  , P_Fld(	 1		  , MISC_STBCAL2_STB_PICG_EARLY_1T_EN ));
	}

	//================================
	//gating Mode config
	//================================
	switch (gat_c->RX_GATING_MODE)
	{
		//Pulse Mode
		case 0:
		{
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ9)		  , P_Fld(	0	 , B0_DQ9_RG_RX_ARDQS0_DQSIENMODE_B0 ));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ9)		  , P_Fld(	0	 , B1_DQ9_RG_RX_ARDQS0_DQSIENMODE_B1 ));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ6)		  , P_Fld(	0	 , B0_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B0));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ6)		  , P_Fld(	0	 , B1_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B1));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_STBCAL) , P_Fld(	0	 , MISC_SHU_STBCAL_DQSIEN_BURST_MODE ));
			break;
		}
		// Burst Mode (8UI)
		case 1:
		{
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ9)		  , P_Fld(	1	 , B0_DQ9_RG_RX_ARDQS0_DQSIENMODE_B0 ));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ9)		  , P_Fld(	1	 , B1_DQ9_RG_RX_ARDQS0_DQSIENMODE_B1 ));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ6)		  , P_Fld(	1	 , B0_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B0));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ6)		  , P_Fld(	1	 , B1_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B1));
			break;
		}
		// Burst Mode (7UI)
		case 2:
		{
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ9)		  , P_Fld(	1	 , B0_DQ9_RG_RX_ARDQS0_DQSIENMODE_B0 ));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ9)		  , P_Fld(	1	 , B1_DQ9_RG_RX_ARDQS0_DQSIENMODE_B1 ));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ6)		  , P_Fld(	2	 , B0_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B0));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ6)		  , P_Fld(	2	 , B1_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B1));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL1)	  , P_Fld(	1	 , MISC_STBCAL1_DQSIEN_7UI_EN		 ));
			break;
		}
		// Oringinal Burst
		case 3:
		{
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ9)		  , P_Fld(	1	 , B0_DQ9_RG_RX_ARDQS0_DQSIENMODE_B0 ));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ9)		  , P_Fld(	1	 , B1_DQ9_RG_RX_ARDQS0_DQSIENMODE_B1 ));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ6)		  , P_Fld(	0	 , B0_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B0));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ6)		  , P_Fld(	0	 , B1_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B1));
			break;
		}
		default:
		{
			mcSHOW_DBG_MSG(("ERROR: Gating Mode choose unexpected Mode!!!!\n"));
			break;
		}
	}

	//================================
	//Gating tracking Mode config
	//================================
	switch (gat_c->RX_GATING_TRACK_MODE)
	{
		//Valid DLY Mode
		case 0:
		{
			//TODO SHU1_DQSG if -like mode should set STB_UPDMASKCYC = 0 STB_UPDMASK_EN=0 others STB_UPDMASKCYC=9 STB_UPDMASK_EN=1
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL)  , P_Fld(   1 	, MISC_STBCAL_STB_DQIEN_IG			) \
																	   | P_Fld(   1 	, MISC_STBCAL_PICHGBLOCK_NORD		) \
																	   | P_Fld(   0 	, MISC_STBCAL_REFUICHG				) \
																	   | P_Fld(   0 	, MISC_STBCAL_PHYVALID_IG			) \
																	   | P_Fld(   0 	, MISC_STBCAL_STBSTATE_OPT			) \
																	   | P_Fld(   0 	, MISC_STBCAL_STBDLELAST_FILTER 	) \
																	   | P_Fld(   0 	, MISC_STBCAL_STBDLELAST_PULSE		) \
																	   | P_Fld(   0 	, MISC_STBCAL_STBDLELAST_OPT		) \
																	   | P_Fld(   1 	, MISC_STBCAL_PIMASK_RKCHG_OPT		));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL1) , P_Fld(   1 	, MISC_STBCAL1_STBCAL_FILTER		) \
																	   | P_Fld(   1 	, MISC_STBCAL1_STB_FLAGCLR_OPT		) \
																	   | P_Fld(   1 	, MISC_STBCAL1_STB_SHIFT_DTCOUT_IG	) \
																	   | P_Fld(   1 	, MISC_STBCAL1_STBCNT_MODESEL		));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CTRL0)   , P_Fld(   0 	, MISC_CTRL0_R_DMDQSIEN_FIFO_EN 	) \
																	   | P_Fld(   2 	, MISC_CTRL0_R_DMVALID_DLY			) \
																	   | P_Fld(   1 	, MISC_CTRL0_R_DMVALID_DLY_OPT		) \
																	   | P_Fld(   0 	, MISC_CTRL0_R_DMSTBEN_SYNCOPT		) \
																	   | P_Fld(   0 	, MISC_CTRL0_R_DMVALID_NARROW_IG	)); //TODO
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ6)	   , P_Fld(   1 	, B0_DQ6_RG_RX_ARDQ_DMRANK_OUTSEL_B0));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ9)	   , P_Fld(   0 	, B0_DQ9_R_DMDQSIEN_RDSEL_LAT_B0	) \
																	   | P_Fld(   0 	, B0_DQ9_R_DMDQSIEN_VALID_LAT_B0	));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ6)	   , P_Fld(   1 	, B1_DQ6_RG_RX_ARDQ_DMRANK_OUTSEL_B1));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ9)	   , P_Fld(   0 	, B1_DQ9_R_DMDQSIEN_RDSEL_LAT_B1	) \
																	   | P_Fld(   0 	, B1_DQ9_R_DMDQSIEN_VALID_LAT_B1	));
			break;
		}
		//-like Mode
		case 1:
		{
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL)  , P_Fld(   0 	, MISC_STBCAL_STB_DQIEN_IG			) \
																	   | P_Fld(   0 	, MISC_STBCAL_PICHGBLOCK_NORD		) \
																	   | P_Fld(   1 	, MISC_STBCAL_REFUICHG				) \
																	   | P_Fld(   0 	, MISC_STBCAL_PHYVALID_IG			) \
																	   | P_Fld(   1 	, MISC_STBCAL_STBSTATE_OPT			) \
																	   | P_Fld(   0 	, MISC_STBCAL_STBDLELAST_FILTER 	) \
																	   | P_Fld(   0 	, MISC_STBCAL_STBDLELAST_PULSE		) \
																	   | P_Fld(   0 	, MISC_STBCAL_STBDLELAST_OPT		) \
																	   | P_Fld(   0 	, MISC_STBCAL_PIMASK_RKCHG_OPT		));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL1) , P_Fld(   0 	, MISC_STBCAL1_STBCAL_FILTER		) \
																	   | P_Fld(   1 	, MISC_STBCAL1_STB_FLAGCLR_OPT		) \
																	   | P_Fld(   0 	, MISC_STBCAL1_STB_SHIFT_DTCOUT_IG	) \
																	   | P_Fld(   1 	, MISC_STBCAL1_STBCNT_MODESEL		));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CTRL0)   , P_Fld(   0 	, MISC_CTRL0_R_DMDQSIEN_FIFO_EN 	) \
																	   | P_Fld(   0 	, MISC_CTRL0_R_DMVALID_DLY			) \
																	   | P_Fld(   0 	, MISC_CTRL0_R_DMVALID_DLY_OPT		) \
																	   | P_Fld(   0 	, MISC_CTRL0_R_DMSTBEN_SYNCOPT		) \
																	   | P_Fld(   1 	, MISC_CTRL0_R_DMVALID_NARROW_IG	));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ6)	   , P_Fld(   1 	, B0_DQ6_RG_RX_ARDQ_DMRANK_OUTSEL_B0));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ9)	   , P_Fld(   0 	, B0_DQ9_R_DMDQSIEN_RDSEL_LAT_B0	) \
																	   | P_Fld(   0 	, B0_DQ9_R_DMDQSIEN_VALID_LAT_B0	));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ6)	   , P_Fld(   1 	, B1_DQ6_RG_RX_ARDQ_DMRANK_OUTSEL_B1));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ9)	   , P_Fld(   0 	, B1_DQ9_R_DMDQSIEN_RDSEL_LAT_B1	) \
																	   | P_Fld(   0 	, B1_DQ9_R_DMDQSIEN_VALID_LAT_B1	));
			break;
		}
		//FIFO Mode
		case 2:
		{
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL)  , P_Fld(   1 	, MISC_STBCAL_STB_DQIEN_IG			) \
																	   | P_Fld(   1 	, MISC_STBCAL_PICHGBLOCK_NORD		) \
																	   | P_Fld(   0 	, MISC_STBCAL_REFUICHG				) \
																	   | P_Fld(   0 	, MISC_STBCAL_PHYVALID_IG			) \
																	   | P_Fld(   0 	, MISC_STBCAL_STBSTATE_OPT			) \
																	   | P_Fld(   0 	, MISC_STBCAL_STBDLELAST_FILTER 	) \
																	   | P_Fld(   0 	, MISC_STBCAL_STBDLELAST_PULSE		) \
																	   | P_Fld(   0 	, MISC_STBCAL_STBDLELAST_OPT		) \
																	   | P_Fld(   1 	, MISC_STBCAL_PIMASK_RKCHG_OPT		));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL1) , P_Fld(   1 	, MISC_STBCAL1_STBCAL_FILTER		) \
																	   | P_Fld(   1 	, MISC_STBCAL1_STB_FLAGCLR_OPT		) \
																	   | P_Fld(   1 	, MISC_STBCAL1_STB_SHIFT_DTCOUT_IG	) \
																	   | P_Fld(   1 	, MISC_STBCAL1_STBCNT_MODESEL		));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CTRL0)   , P_Fld(   1 	, MISC_CTRL0_R_DMDQSIEN_FIFO_EN 	) \
																	   | P_Fld(   0 	, MISC_CTRL0_R_DMVALID_DLY			) \
																	   | P_Fld(   0 	, MISC_CTRL0_R_DMVALID_DLY_OPT		) \
																	   | P_Fld(   0 	, MISC_CTRL0_R_DMSTBEN_SYNCOPT		) \
																	   | P_Fld(   0 	, MISC_CTRL0_R_DMVALID_NARROW_IG	)); // @Darren, func no use sync MP settings from HJ
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ6)	   , P_Fld(   0 	, B0_DQ6_RG_RX_ARDQ_DMRANK_OUTSEL_B0));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ9)	   , P_Fld(   1+gat_c->VALID_LAT_VALUE	   , B0_DQ9_R_DMDQSIEN_RDSEL_LAT_B0    ) \
																	   | P_Fld(   0+gat_c->VALID_LAT_VALUE	   , B0_DQ9_R_DMDQSIEN_VALID_LAT_B0    ));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ6)	   , P_Fld(   0 	, B1_DQ6_RG_RX_ARDQ_DMRANK_OUTSEL_B1));
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ9)	   , P_Fld(   1+gat_c->VALID_LAT_VALUE	   , B1_DQ9_R_DMDQSIEN_RDSEL_LAT_B1    ) \
																	   | P_Fld(   0+gat_c->VALID_LAT_VALUE	   , B1_DQ9_R_DMDQSIEN_VALID_LAT_B1    ));
			break;
		}
		default:
		{
			mcSHOW_DBG_MSG(("ERROR: Gating tracking Mode choose unexpected Mode!!!!"));
			break;
		}
	}

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_PHY2)			, P_Fld(   1  , B0_PHY2_RG_RX_ARDQS_DQSIEN_UI_LEAD_LAG_EN_B0));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_PHY2)			, P_Fld(   1  , B1_PHY2_RG_RX_ARDQS_DQSIEN_UI_LEAD_LAG_EN_B1));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL)		, P_Fld(   1  , MISC_STBCAL_DQSIENMODE						) \
																	| P_Fld(   1  , MISC_STBCAL_SREF_DQSGUPD					) \
																	| P_Fld(   1  , MISC_STBCAL_DQSIENCG_CHG_EN 				) \
																	| P_Fld(   1  , MISC_STBCAL_PICGEN							) \
																	| P_Fld(   0  , MISC_STBCAL_RKCHGMASKDIS					) \
																	| P_Fld(   0  , MISC_STBCAL_STBCAL2R						));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CTRL1)		, P_Fld(   1  , MISC_CTRL1_R_DMDQSIENCG_EN					));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL2)		, P_Fld(   0  , MISC_STBCAL2_STB_GERRSTOP					) \
																	| P_Fld(   0  , MISC_STBCAL2_STB_GERR_RST					) \
																	| P_Fld(   1  , MISC_STBCAL2_STB_GERR_B01					) \
																	| P_Fld(   0  , MISC_STBCAL2_STB_GERR_B23					));
	//PICG_MODE only support new mode  so here fix 1
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_RX_IN_GATE_EN_CTRL), P_Fld(1, MISC_RX_IN_GATE_EN_CTRL_RX_IN_GATE_EN_OPT	));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_RX_IN_BUFF_EN_CTRL), P_Fld(1, MISC_RX_IN_BUFF_EN_CTRL_RX_IN_BUFF_EN_OPT	));

	if(A_T->NEW_RANK_MODE == 1)
	{
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_STBCAL2)	, P_Fld(   1  , MISC_STBCAL2_STB_IG_XRANK_CG_RST			) \
																	| P_Fld(   1  , MISC_STBCAL2_STB_RST_BY_RANK				) \
																	| P_Fld(   1  , MISC_STBCAL2_DQSIEN_SELPH_BY_RANK_EN		));
	}
	mcSHOW_DBG_MSG(("Exit from Gating configuration <<<< \n"));
}

static void RX_INTPUT_Config(DRAMC_CTX_T *p)
{
	U8	  VALID_LAT    = 1;// TODO inside {0,1}
	U8	  RDSEL_LAT    = 2;// TODO alywas VALID_LAT+1;
	U8	  dq_min	   = 0;
	U8	  dq_max	   = 0xff;
	U8	  scale 	   = 3;
	U8	  threadhold   = 0;
	U32   dqs_min	   = 0;
	U32   dqs_max	   = 0x1ff;
	U8	  RX_force_upd = 0; //TODO
	U8	  F_LEADLAG    = 0; //TODO
	U8	  RG_MODE_EN   = 0; //TODO
	U8	  irank 	   = 0;
	U8	  backup_rank  = 0;

	backup_rank = p->rank;

	mcSHOW_DBG_MSG(("[RX_INPUT] configuration >>>>> \n"));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_RXDVS0) 	 , P_Fld(  1   , B0_RXDVS0_R_HWSAVE_MODE_ENA_B0 						   ) \
																 | P_Fld(  0   , B0_RXDVS0_R_DMRXDVS_CNTCMP_OPT_B0						   ) \
																 | P_Fld(  1   , B0_RXDVS0_R_DMRXDVS_DQIENPRE_OPT_B0					   ) \
																 | P_Fld(  1   , B0_RXDVS0_R_HWRESTORE_ENA_B0							   ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_RXDVS0) 	 , P_Fld(  1   , B1_RXDVS0_R_HWSAVE_MODE_ENA_B1 						   ) \
																 | P_Fld(  0   , B1_RXDVS0_R_DMRXDVS_CNTCMP_OPT_B1						   ) \
																 | P_Fld(  1   , B1_RXDVS0_R_DMRXDVS_DQIENPRE_OPT_B1					   ) \
																 | P_Fld(  1   , B1_RXDVS0_R_HWRESTORE_ENA_B1							   ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ9   ) 	 , P_Fld(  VALID_LAT   , B0_DQ9_R_DMRXDVS_VALID_LAT_B0					   ) \
																 | P_Fld(  RDSEL_LAT   , B0_DQ9_R_DMRXDVS_RDSEL_LAT_B0					   ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ9   ) 	 , P_Fld(  VALID_LAT   , B1_DQ9_R_DMRXDVS_VALID_LAT_B1					   ) \
																 | P_Fld(  RDSEL_LAT   , B1_DQ9_R_DMRXDVS_RDSEL_LAT_B1					   ));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_RXDVS2 )	 , P_Fld(  1   , MISC_RXDVS2_R_DMRXDVS_DBG_MON_EN						   ) \
																 | P_Fld(  0   , MISC_RXDVS2_R_DMRXDVS_DBG_MON_CLR						   ) \
																 | P_Fld(  0   , MISC_RXDVS2_R_DMRXDVS_DBG_PAUSE_EN 					   ) \
																 | P_Fld(  1   , MISC_RXDVS2_R_DMRXDVS_DEPTH_HALF						   ));

	for(irank = RANK_0; irank < p->support_rank_num; irank++)
	{
		vSetRank(p, irank);
		//RK0--B0
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_RK_B0_RXDVS3 )  , P_Fld(  dq_min	, RK_B0_RXDVS3_RG_RK0_ARDQ_MIN_DLY_B0			   ) \
																	 | P_Fld(  dq_max	, RK_B0_RXDVS3_RG_RK0_ARDQ_MAX_DLY_B0			   ));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_RK_B0_RXDVS4 )  , P_Fld(  dqs_min	, RK_B0_RXDVS4_RG_RK0_ARDQS0_MIN_DLY_B0 		   ) \
																	 | P_Fld(  dqs_max	, RK_B0_RXDVS4_RG_RK0_ARDQS0_MAX_DLY_B0 		   ));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_RK_B0_RXDVS2 )  , P_Fld(  scale	, RK_B0_RXDVS2_R_RK0_RX_DLY_FAL_DQS_SCALE_B0	   ) \
																	 | P_Fld(  scale	, RK_B0_RXDVS2_R_RK0_RX_DLY_FAL_DQ_SCALE_B0 	   ) \
																	 | P_Fld(  0		, RK_B0_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B0  ) \
																	 | P_Fld(  scale	, RK_B0_RXDVS2_R_RK0_RX_DLY_RIS_DQS_SCALE_B0	   ) \
																	 | P_Fld(  scale	, RK_B0_RXDVS2_R_RK0_RX_DLY_RIS_DQ_SCALE_B0 	   ) \
																	 | P_Fld(  0		, RK_B0_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B0  ) \
																	 | P_Fld(  1		, RK_B0_RXDVS2_R_RK0_DVS_FDLY_MODE_B0			   ) \
																	 | P_Fld(  0		, RK_B0_RXDVS2_R_RK0_DVS_MODE_B0				   ));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_RK_B0_RXDVS1 )  , P_Fld(  threadhold  , RK_B0_RXDVS1_R_RK0_B0_DVS_TH_LAG			   ) \
																	 | P_Fld(  threadhold  , RK_B0_RXDVS1_R_RK0_B0_DVS_TH_LEAD			   ));


		//RK0--B1
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_RK_B1_RXDVS3 )  , P_Fld(  dq_min	, RK_B1_RXDVS3_RG_RK0_ARDQ_MIN_DLY_B1			   ) \
																	 | P_Fld(  dq_max	, RK_B1_RXDVS3_RG_RK0_ARDQ_MAX_DLY_B1			   ));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_RK_B1_RXDVS4 )  , P_Fld(  dqs_min	, RK_B1_RXDVS4_RG_RK0_ARDQS0_MIN_DLY_B1 		   ) \
																	 | P_Fld(  dqs_max	, RK_B1_RXDVS4_RG_RK0_ARDQS0_MAX_DLY_B1 		   ));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_RK_B1_RXDVS2 )  , P_Fld(  scale	, RK_B1_RXDVS2_R_RK0_RX_DLY_FAL_DQS_SCALE_B1	   ) \
																	 | P_Fld(  scale	, RK_B1_RXDVS2_R_RK0_RX_DLY_FAL_DQ_SCALE_B1 	   ) \
																	 | P_Fld(  0		, RK_B1_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B1  ) \
																	 | P_Fld(  scale	, RK_B1_RXDVS2_R_RK0_RX_DLY_RIS_DQS_SCALE_B1	   ) \
																	 | P_Fld(  scale	, RK_B1_RXDVS2_R_RK0_RX_DLY_RIS_DQ_SCALE_B1 	   ) \
																	 | P_Fld(  0		, RK_B1_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B1  ) \
																	 | P_Fld(  1		, RK_B1_RXDVS2_R_RK0_DVS_FDLY_MODE_B1			   ) \
																	 | P_Fld(  0		, RK_B1_RXDVS2_R_RK0_DVS_MODE_B1				   ));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_RK_B1_RXDVS1 )  , P_Fld(  threadhold  , RK_B1_RXDVS1_R_RK0_B1_DVS_TH_LAG			   ) \
																	 | P_Fld(  threadhold  , RK_B1_RXDVS1_R_RK0_B1_DVS_TH_LEAD			   ));
	}
	vSetRank(p, backup_rank);


	vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CG_CTRL1 )	 ,	 0xffffffff  , MISC_CG_CTRL1_R_DVS_DIV4_CG_CTRL 			 );  //TODO


	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_RXDVS1	)  , P_Fld(  F_LEADLAG	, B0_RXDVS1_F_LEADLAG_TRACK_B0			 ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_RXDVS1	)  , P_Fld(  F_LEADLAG	, B1_RXDVS1_F_LEADLAG_TRACK_B1			 ));

	if(RX_force_upd == 1)
	{
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_DLLFRZ_CTRL )  , P_Fld(  1	, DLLFRZ_CTRL_DLLFRZ_MON_PBREF_OPT				) \
																   | P_Fld(  1	, DLLFRZ_CTRL_DLLFRZ_BLOCKLONG					) \
																   | P_Fld(  1	, DLLFRZ_CTRL_INPUTRXTRACK_BLOCK				));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_RXDVS1	)  , P_Fld(  1	, B0_RXDVS1_R_DMRXDVS_UPD_CLR_NORD_B0			));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_RXDVS1	)  , P_Fld(  1	, B1_RXDVS1_R_DMRXDVS_UPD_CLR_NORD_B1			));
	}

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ5  )	  , P_Fld(	1  , B1_DQ5_RG_RX_ARDQS0_DVS_EN_B1			));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ5  )	  , P_Fld(	1  , B0_DQ5_RG_RX_ARDQS0_DVS_EN_B0			));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_RXDVS0 )  , P_Fld(	1  , B0_RXDVS0_R_RX_DLY_TRACK_ENA_B0		)\
															  | P_Fld(	1  , B0_RXDVS0_R_RX_DLY_TRACK_CG_EN_B0		)\
															  | P_Fld(	1  , B0_RXDVS0_R_RX_DLY_TRACK_SPM_CTRL_B0	)\
															  | P_Fld(	0  , B0_RXDVS0_R_RX_RANKINCTL_B0			)\
															  | P_Fld(	1  , B0_RXDVS0_R_RX_RANKINSEL_B0			));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_RXDVS0 )  , P_Fld(	1  , B1_RXDVS0_R_RX_DLY_TRACK_ENA_B1		)\
															  | P_Fld(	1  , B1_RXDVS0_R_RX_DLY_TRACK_CG_EN_B1		)\
															  | P_Fld(	1  , B1_RXDVS0_R_RX_DLY_TRACK_SPM_CTRL_B1	)\
															  | P_Fld(	0  , B1_RXDVS0_R_RX_RANKINCTL_B1			)\
															  | P_Fld(	1  , B1_RXDVS0_R_RX_RANKINSEL_B1			));

	for(irank = RANK_0; irank < RANK_MAX; irank++)
	{
		vSetRank(p, irank);
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_RK_B0_RXDVS2 )  , P_Fld(  1  , RK_B0_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B0  ) \
																	 | P_Fld(  1  , RK_B0_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B0  ) \
																	 | P_Fld(  2  , RK_B0_RXDVS2_R_RK0_DVS_MODE_B0					 ));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_RK_B1_RXDVS2 )  , P_Fld(  1  , RK_B1_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B1  ) \
																	 | P_Fld(  1  , RK_B1_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B1  ) \
																	 | P_Fld(  2  , RK_B1_RXDVS2_R_RK0_DVS_MODE_B1					 ));
	}
	vSetRank(p, backup_rank);

	//Enable RX input delay tracking..
	//TODO notice here if SA should not enbale it before RX perbit calibration
	if (RG_MODE_EN == 1)
	{
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_RG_DFS_CTRL)	  , P_Fld(	1  , MISC_RG_DFS_CTRL_RG_DPY_RXDLY_TRACK_EN 	 ));
	} else {
//		  `TBA_TOP.dvfs_spm_vif.sc_dphy_reserved[1:0] = 2'b11; //TODO
	}

	mcSHOW_DBG_MSG(("[RX_INPUT] configuration <<<<< \n"));
}

static void DDRPHY_PICG_Config(DRAMC_CTX_T *p)
{
	U8 PICG_MODE  = 1; // only support new Mode under
	U8 MISC_CG_EN = 1;
	U8 MISC_CG_REVERSE_DEFAULT_ON = 0;	//for default CG enable.

	mcSHOW_DBG_MSG(("Enter into PICG configuration >>>> \n"));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CTRL4)		, P_Fld(   PICG_MODE  , MISC_CTRL4_R_OPT2_MPDIV_CG					) \
																	| P_Fld(   PICG_MODE  , MISC_CTRL4_R_OPT2_CG_MCK					) \
																	| P_Fld(   PICG_MODE  , MISC_CTRL4_R_OPT2_CG_DQM					) \
																	| P_Fld(   PICG_MODE  , MISC_CTRL4_R_OPT2_CG_DQS					) \
																	| P_Fld(   PICG_MODE  , MISC_CTRL4_R_OPT2_CG_DQ 					) \
																	| P_Fld(   PICG_MODE  , MISC_CTRL4_R_OPT2_CG_DQSIEN 				));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CTRL3)		, P_Fld(  !PICG_MODE  , MISC_CTRL3_ARPI_MPDIV_CG_DQ_OPT 			) \
																	| P_Fld(  !PICG_MODE  , MISC_CTRL3_ARPI_CG_MCK_DQ_OPT				) \
																	| P_Fld(  !PICG_MODE  , MISC_CTRL3_ARPI_CG_DQS_OPT					) \
																	| P_Fld(  !PICG_MODE  , MISC_CTRL3_ARPI_CG_DQ_OPT					));

	//Notice here: MISC_CG_CTRL0_RG_CG_COMB0_OFF_DISABLE = 1 will leading other_shuffle_group before register settle down latch ->error. can not set to 1
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CG_CTRL0) 	, P_Fld(  !MISC_CG_EN , MISC_CG_CTRL0_RG_CG_DRAMC_OFF_DISABLE		) \
																	| P_Fld(  !MISC_CG_EN , MISC_CG_CTRL0_RG_CG_PHY_OFF_DIABLE			) \
																	| P_Fld(  !MISC_CG_EN , MISC_CG_CTRL0_RG_CG_COMB_OFF_DISABLE		) \
																	| P_Fld(  !MISC_CG_EN , MISC_CG_CTRL0_RG_CG_CMD_OFF_DISABLE 		) \
																	| P_Fld(  !MISC_CG_EN , MISC_CG_CTRL0_RG_CG_COMB0_OFF_DISABLE		) \
																	| P_Fld(  !MISC_CG_EN , MISC_CG_CTRL0_RG_CG_COMB1_OFF_DISABLE		) \
																	| P_Fld(  !MISC_CG_EN , MISC_CG_CTRL0_RG_CG_RX_CMD_OFF_DISABLE		) \
																	| P_Fld(  !MISC_CG_EN , MISC_CG_CTRL0_RG_CG_RX_COMB0_OFF_DISABLE	) \
																	| P_Fld(  !MISC_CG_EN , MISC_CG_CTRL0_RG_CG_RX_COMB1_OFF_DISABLE	) \
																	| P_Fld(  !MISC_CG_EN , MISC_CG_CTRL0_RG_CG_INFRA_OFF_DISABLE		));


	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CG_CTRL2) 	, P_Fld(  !MISC_CG_EN , MISC_CG_CTRL2_RG_MEM_DCM_CG_OFF_DISABLE 	) \
																	| P_Fld(  !MISC_CG_EN , MISC_CG_CTRL2_RG_PIPE0_CG_OFF_DISABLE		) \
																	| P_Fld(  !MISC_CG_EN , MISC_CG_CTRL2_RG_PHY_CG_OFF_DISABLE 		));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CG_CTRL5) 	, P_Fld(   MISC_CG_EN , MISC_CG_CTRL5_R_DQ1_DLY_DCM_EN				) \
																	| P_Fld(   MISC_CG_EN , MISC_CG_CTRL5_R_DQ0_DLY_DCM_EN				) \
																	| P_Fld(   MISC_CG_EN , MISC_CG_CTRL5_R_CA_DLY_DCM_EN				) \
																	| P_Fld(   MISC_CG_EN , MISC_CG_CTRL5_R_DQ1_PI_DCM_EN				) \
																	| P_Fld(   MISC_CG_EN , MISC_CG_CTRL5_R_DQ0_PI_DCM_EN				) \
																	| P_Fld(   MISC_CG_EN , MISC_CG_CTRL5_R_CA_PI_DCM_EN				));

	//defualt DCM enable, if we wanner to test CG enable, modified default CG condition.
	//disable DCM.--- I think just for debug
	if(MISC_CG_REVERSE_DEFAULT_ON == 1)
	{
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_RX_CG_SET0) 		, P_Fld(   1	  , RX_CG_SET0_RDATCKAR 						 ) \
																		| P_Fld(   1	  , RX_CG_SET0_RDYCKAR							 ));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SREF_DPD_CTRL)		, P_Fld(   1	  , SREF_DPD_CTRL_CMDCKAR						 ));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_DCM_CTRL0)			, P_Fld(   1	  , DCM_CTRL0_BCLKAR							 ));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TX_CG_SET0) 		, P_Fld(   1	  , TX_CG_SET0_PSELAR							 ) \
																		| P_Fld(   1	  , TX_CG_SET0_DWCLKRUN 						 ));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SCSMCTRL_CG)		, P_Fld(   1	  , SCSMCTRL_CG_SCSM_CGAR						 ) \
																		| P_Fld(   1	  , SCSMCTRL_CG_SCARB_SM_CGAR					 ));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TX_TRACKING_SET0)	, P_Fld(   1	  , TX_TRACKING_SET0_RDDQSOSC_CGAR				 ) \
																		| P_Fld(   1	  , TX_TRACKING_SET0_HMRRSEL_CGAR				 ) \
																		| P_Fld(   1	  , TX_TRACKING_SET0_TXUIPI_CAL_CGAR			 ));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_ZQ_SET0)			, P_Fld(   1	  , ZQ_SET0_ZQCS_MASK_SEL_CGAR					 ));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_ACTIMING_CTRL)		, P_Fld(   1	  , ACTIMING_CTRL_CLKWITRFC 					 ) \
																		| P_Fld(   1	  , ACTIMING_CTRL_SEQCLKRUN3					 ) \
																		| P_Fld(   1	  , ACTIMING_CTRL_SEQCLKRUN2					 ) \
																		| P_Fld(   1	  , ACTIMING_CTRL_SEQCLKRUN 					 ));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_CLKAR)				, P_Fld(   1	  , CLKAR_REQQUECLKRUN							 ) \
																		| P_Fld(   1	  , CLKAR_REQQUE_PACG_DIS						 ));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL)		, P_Fld(   1	  , DRAMC_PD_CTRL_PHYGLUECLKRUN 				 ));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A3)			, P_Fld(   1	  , TEST2_A3_TESTCLKRUN 						 ));
		DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
		vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DRAMC_REG_DVFS_CTRL0) 		, P_Fld(   1	  , DVFS_CTRL0_DVFS_CG_OPT						 ));
		DramcBroadcastOnOff(DRAMC_BROADCAST_ON);
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DUTYSCAN1)	, P_Fld(   1	  , MISC_DUTYSCAN1_EYESCAN_DQS_OPT				 ));

		//TODO -- for DPHY shuffle RG have to set to different Group into SRAM or not.--here just conf0 but not all frequency group
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ8)		, P_Fld(   1	  , SHU_B0_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B0 	 ) \
																		| P_Fld(   1	  , SHU_B0_DQ8_R_RMRODTEN_CG_IG_B0				 ) \
																		| P_Fld(   1	  , SHU_B0_DQ8_R_RMRX_TOPHY_CG_IG_B0			 ) \
																		| P_Fld(   1	  , SHU_B0_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B0	 ) \
																		| P_Fld(   1	  , SHU_B0_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B0 ) \
																		| P_Fld(   1	  , SHU_B0_DQ8_R_DMRXDLY_CG_IG_B0				 ) \
																		| P_Fld(   1	  , SHU_B0_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B0	 ) \
																		| P_Fld(   1	  , SHU_B0_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B0	 ) \
																		| P_Fld(   1	  , SHU_B0_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B0	 ) \
																		| P_Fld(   1	  , SHU_B0_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B0) \
																		| P_Fld(   1	  , SHU_B0_DQ8_R_DMRANK_PIPE_CG_IG_B0			 ) \
																		| P_Fld(   1	  , SHU_B0_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B0		 ));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ8)		, P_Fld(   1	  , SHU_B1_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B1 	 ) \
																		| P_Fld(   1	  , SHU_B1_DQ8_R_RMRODTEN_CG_IG_B1				 ) \
																		| P_Fld(   1	  , SHU_B1_DQ8_R_RMRX_TOPHY_CG_IG_B1			 ) \
																		| P_Fld(   1	  , SHU_B1_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B1	 ) \
																		| P_Fld(   1	  , SHU_B1_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B1 ) \
																		| P_Fld(   1	  , SHU_B1_DQ8_R_DMRXDLY_CG_IG_B1				 ) \
																		| P_Fld(   1	  , SHU_B1_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B1	 ) \
																		| P_Fld(   1	  , SHU_B1_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B1	 ) \
																		| P_Fld(   1	  , SHU_B1_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B1	 ) \
																		| P_Fld(   1	  , SHU_B1_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B1) \
																		| P_Fld(   1	  , SHU_B1_DQ8_R_DMRANK_PIPE_CG_IG_B1			 ) \
																		| P_Fld(   1	  , SHU_B1_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B1		 ));
	}

	mcSHOW_DBG_MSG(("Exit from PICG configuration <<<< \n"));
}

static void DRAMC_COMMON_Config(DRAMC_CTX_T *p)
{
	U8 RD2MRR_EXTEND_EN = 1; // for fix Samsung RD2MRR seamless error, If the samsung fix that bug, this could set 0
	U8 EBG_EN			   ;
	U8 TMRRI_MODE		= 1; // !!!Notice here: 0: Old Mode, 1: New Mode   --- FIX NEW MODE. Pertrus not support old mode anymore
	U8 NOBLOCKALE_EN	= 1;
	U8 RUNTIME_MRR		= 1;
#if (fcFOR_CHIP_ID == fcMargaux)
	U8 dram_mux = p->DRAMPinmux; // 0: DSC, 1: LPBK, 2: MCP
#endif

	//pre configuration calculate
	if(TMRRI_MODE == 1)
	{
		NOBLOCKALE_EN = 1;
		RUNTIME_MRR   = 1;
	} else {
		//TODO
		mcSHOW_DBG_MSG(("NONBLOCKALE RUNTIMEMRR could be random.--for MP should setting 1. just record it."));
	}


#if ENABLE_EARLY_BG_CMD==1
	if((LPDDR5_EN_S == 1) && (DFS(0)->data_rate < 3200)) EBG_EN = 0; else EBG_EN = 1;
#else
	EBG_EN = 0;
#endif

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_DDRCOMMON0) 		, P_Fld(   1			 , DDRCOMMON0_BK8EN 				) \
																	| P_Fld(   LPDDR5_EN_S	 , DDRCOMMON0_LPDDR5EN				) \
																	| P_Fld(   LPDDR4_EN_S	 , DDRCOMMON0_LPDDR4EN				) \
																	| P_Fld(   0			 , DDRCOMMON0_TRCDEARLY 			)); //if LPDDR5 set1 HEFF mode ACT -> R/W delay-1

#if (fcFOR_CHIP_ID == fcMargaux)
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CTRL1)	, P_Fld(   dram_mux 			, MISC_CTRL1_R_DMPINMUX 		   ));
#else
	if(LPDDR5_EN_S == 1)
	{
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CTRL1)	, P_Fld(   2			 , MISC_CTRL1_R_DMPINMUX			));
	}
#endif

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_RX_SET0)			, P_Fld(   0			 , RX_SET0_DM4TO1MODE				));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_REFCTRL0)			, P_Fld(   5			 , REFCTRL0_REF_PREGATE_CNT 			) \
																	| P_Fld(   0			 , REFCTRL0_DMPGVLD_IG					) \
																	| P_Fld(   4			 , REFCTRL0_DISBYREFNUM 				) \
																	| P_Fld(   0			 , REFCTRL0_PBREF_DISBYRATE 			) \
																	| P_Fld(   1			 , REFCTRL0_PBREF_DISBYREFNUM			) \
																	| P_Fld(   1			 , REFCTRL0_PBREF_BK_REFA_ENA			) \
																	| P_Fld(   1			 , REFCTRL0_PBREF_BK_REFA_NUM			));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_REFCTRL1)			, P_Fld(   1			 , REFCTRL1_PB2AB_OPT					) \
																	| P_Fld(   1			 , REFCTRL1_REF_QUE_AUTOSAVE_EN 		) \
																	| P_Fld(   0			 , REFCTRL1_REF_OVERHEAD_ALL_REFPB_ENA	) \
																	| P_Fld(   1			 , REFCTRL1_REF_OVERHEAD_SLOW_REFPB_ENA ) \
																	| P_Fld(   0			 , REFCTRL1_REF_OVERHEAD_ALL_REFAL_ENA	) \
																	| P_Fld(   0			 , REFCTRL1_REF_OVERHEAD_SLOW_REFAL_ENA ) \
																	| P_Fld(   0			 , REFCTRL1_REF_OVERHEAD_RATE_REFPB_ENA ) \
																	| P_Fld(   0			 , REFCTRL1_REF_OVERHEAD_RATE_REFAL_ENA ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_REFCTRL2)			, P_Fld(   0			 , REFCTRL2_REF_OVERHEAD_RATE			));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_DLLFRZ_CTRL)		, P_Fld(   0			 , DLLFRZ_CTRL_UPDBYWR				) \
																	| P_Fld(   1			 , DLLFRZ_CTRL_DLLFRZ				));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_DRAMCTRL)			, P_Fld(   0			 , DRAMCTRL_ADRDECEN				) \
																	| P_Fld(   1			 , DRAMCTRL_PREALL_OPTION			) \
																	| P_Fld(   1			 , DRAMCTRL_REQQUE_THD_EN			) \
																	| P_Fld(   1			 , DRAMCTRL_DYNMWREN				) \
																	| P_Fld(   0			 , DRAMCTRL_AG0MWR					) \
																	| P_Fld(   0			 , DRAMCTRL_ADRBIT3DEC				) \
																	| P_Fld(   0			 , DRAMCTRL_CTOREQ_HPRI_OPT 		));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_ACTIMING_CTRL)		, P_Fld(   0			 , ACTIMING_CTRL_CLKWITRFC			) \
																	| P_Fld(   1			 , ACTIMING_CTRL_SEQCLKRUN3 		) \
																	| P_Fld(   0			 , ACTIMING_CTRL_FASTW2R			) \
																	| P_Fld(   0			 , ACTIMING_CTRL_REFBW_FREN 		) \
																	| P_Fld(   1			 , ACTIMING_CTRL_TMRRICHKDIS		) \
																	| P_Fld(   0			 , ACTIMING_CTRL_REFNA_OPT			) \
																	| P_Fld(!TMRRI_MODE 	 , ACTIMING_CTRL_MRRIOPT			) \
																	| P_Fld(!TMRRI_MODE 	 , ACTIMING_CTRL_TMRRIBYRK_DIS		) \
																	| P_Fld( TMRRI_MODE 	 , ACTIMING_CTRL_TMRRICHKDIS		));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_ARBCTL) 			, P_Fld(   0x80 		 , ARBCTL_MAXPENDCNT				) \
																	| P_Fld(   0			 , ARBCTL_WDATACNTDIS				) );

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_DRAM_CLK_CTRL)		, P_Fld(   1			 , DRAM_CLK_CTRL_CLK_EN 			) );

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_CLKAR)				, P_Fld(   1			 , CLKAR_DCMREF_OPT 				) );

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL)		, P_Fld(   1			 , DRAMC_PD_CTRL_COMBCLKCTRL		) \
																	| P_Fld(   0			 , DRAMC_PD_CTRL_MIOCKCTRLOFF		) \
																	| P_Fld(   1			 , DRAMC_PD_CTRL_PHYCLKDYNGEN		) \
																	| P_Fld(   1			 , DRAMC_PD_CTRL_DCMEN				) \
																	| P_Fld(   1			 , DRAMC_PD_CTRL_DCMEN2 			) \
//																	  | P_Fld(	 0x3		   , DRAMC_PD_CTRL_APHYPI_CKCGH_CNT   )
																	| P_Fld(   0			 , DRAMC_PD_CTRL_PG_DCM_OPT 		));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_RKCFG)				, P_Fld(   0			 , RKCFG_CKE2RANK					) \
																	| P_Fld(   0			 , RKCFG_MRS2RK 					));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL)			, P_Fld(   1			 , CKECTRL_CKE2RANK_OPT2			) \
																	| P_Fld(   1			 , CKECTRL_CKEON					) \
																	| P_Fld(   0			 , CKECTRL_CKETIMER_SEL 			) \
																	| P_Fld(   1			 , CKECTRL_CKE2RANK_OPT8			) \
																	| P_Fld(!RUNTIME_MRR	 , CKECTRL_RUNTIMEMRRMIODIS 		) \
																	| P_Fld(   1			 , CKECTRL_FASTWAKE_SEL 			) \
																	| P_Fld(   1			 , CKECTRL_CKEPBDIS 				) \
																	| P_Fld( !TMRRI_MODE	 , CKECTRL_RUNTIMEMRRCKEFIX 		) \
																	| P_Fld(   0			 , CKECTRL_CKELCKFIX				));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SREF_DPD_CTRL)		, P_Fld(   1			 , SREF_DPD_CTRL_SELFREF_AUTOSAVE_EN) \
																	| P_Fld(   0			 , SREF_DPD_CTRL_GT_SYNC_MASK		) \
																	| P_Fld(   0			 , SREF_DPD_CTRL_DAT_SYNC_MASK		) \
																	| P_Fld(   0			 , SREF_DPD_CTRL_PHY_SYNC_MASK		) \
																	| P_Fld(   1			 , SREF_DPD_CTRL_LPSM_BYPASS_B		) \
																	| P_Fld(   0			 , SREF_DPD_CTRL_SREF_PRD_OPT		) \
																	| P_Fld(   1			 , SREF_DPD_CTRL_CLR_EN 			) \
																	| P_Fld(   0			 , SREF_DPD_CTRL_SRFPD_DIS			) \
																	| P_Fld(   8			 , SREF_DPD_CTRL_SREFDLY			) \
																	| P_Fld(   1			 , SREF_DPD_CTRL_SREF_HW_EN 		));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SCHEDULER_COM)		, P_Fld(   1			 , SCHEDULER_COM_DISRDPHASE1		) \
																	| P_Fld(   1			 , SCHEDULER_COM_MWHPRIEN			) \
																	| P_Fld(   0			 , SCHEDULER_COM_RWHPRICTL			) \
																	| P_Fld(   1			 , SCHEDULER_COM_RWOFOEN			) \
																	| P_Fld(   1			 , SCHEDULER_COM_RWSPLIT			));
//																	  | P_Fld(	 1			   , SCHEDULER_COM_BGPIPEEN 		  )); //diff with IPMV2 compile error
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_PERFCTL0)			, P_Fld(   1			 , PERFCTL0_EMILLATEN				) \
																	| P_Fld(   1			 , PERFCTL0_RWHPRIEN				) \
																	| P_Fld( EBG_EN 		 , PERFCTL0_EBG_EN					) \
																	| P_Fld(   1			 , PERFCTL0_RWLLATEN				) \
																	| P_Fld(   1			 , PERFCTL0_RWAGEEN 				) \
																	| P_Fld(   1			 , PERFCTL0_WFLUSHEN				) \
																	| P_Fld(   0			 , PERFCTL0_REORDEREN				) \
																	| P_Fld(   0			 , PERFCTL0_REORDER_MODE			));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_HW_MRR_FUN) 		, P_Fld(RUNTIME_MRR 	 , HW_MRR_FUN_TMRR_ENA				) \
																	| P_Fld(   0			 , HW_MRR_FUN_TRPMRR_EN 			) \
																	| P_Fld(   0			 , HW_MRR_FUN_TRCDMRR_EN			) \
																	| P_Fld(   1			 , HW_MRR_FUN_MRR_HW_HIPRI			) \
																	| P_Fld(RD2MRR_EXTEND_EN , HW_MRR_FUN_TR2MRR_ENA			) \
																	| P_Fld(RD2MRR_EXTEND_EN , HW_MRR_FUN_R2MRRHPRICTL			) \
																	| P_Fld(RD2MRR_EXTEND_EN , HW_MRR_FUN_MANTMRR_EN			));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_ZQ_SET0)			, P_Fld(   0x0A 		 , ZQ_SET0_ZQCSAD					) \
																	| P_Fld(   0x56 		 , ZQ_SET0_ZQCSOP					));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_MPC_OPTION) 		, P_Fld(   1			 , MPC_OPTION_MPCRKEN				));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_MPC_CTRL)			, P_Fld(!RUNTIME_MRR	 , MPC_CTRL_REFR_BLOCKEN			) \
																	| P_Fld( NOBLOCKALE_EN	 , MPC_CTRL_ZQ_BLOCKALE_OPT 		) \
																	| P_Fld( NOBLOCKALE_EN	 , MPC_CTRL_MPC_BLOCKALE_OPT		) \
																	| P_Fld( NOBLOCKALE_EN	 , MPC_CTRL_MPC_BLOCKALE_OPT1		) \
																	| P_Fld( NOBLOCKALE_EN	 , MPC_CTRL_MPC_BLOCKALE_OPT2		));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_HMR4)				, P_Fld(   1			 , HMR4_SPDR_MR4_OPT				));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_RK_TEST2_A1)		, P_Fld(   0x010000 	 , RK_TEST2_A1_TEST2_BASE			));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A2)			, P_Fld(   0x000020 	 , TEST2_A2_TEST2_OFF				));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A3)			, P_Fld(   1			 , TEST2_A3_TESTAUDPAT				) \
																	| P_Fld(   1			 , TEST2_A3_TEST2WREN2_HW_EN		));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A4)			, P_Fld(   0x11 		 , TEST2_A4_TESTAUDINIT 			) \
																	| P_Fld(   0x0d 		 , TEST2_A4_TESTAUDINC				) \
																	| P_Fld(   0x04 		 , TEST2_A4_TESTAGENTRKSEL			));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_CMD_DEC_CTRL0)		, P_Fld(   1			 , CMD_DEC_CTRL0_RKMODE 			));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_MISCTL0)			, P_Fld(   0			 , MISCTL0_PAGDIS					) \
																	| P_Fld(   0			 , MISCTL0_PBC_ARB_E1T				) \
																	| P_Fld(   1			 , MISCTL0_REFA_ARB_EN2 			) \
																	| P_Fld(   1			 , MISCTL0_PBC_ARB_EN				) \
																	| P_Fld(   1			 , MISCTL0_REFP_ARB_EN2 			) \
																	| P_Fld(   0			 , MISCTL0_EMIPREEN 				) \
																	| P_Fld(   1			 , MISCTL0_PG_WAKEUP_OPT			));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SCSMCTRL)			, P_Fld(   1			 , SCSMCTRL_SC_PG_MAN_DIS			) \
																	| P_Fld( TMRRI_MODE 	 , SCSMCTRL_SC_PG_UPD_OPT			));


	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHUCTRL1)			, P_Fld(   0x1a 		 , SHUCTRL1_FC_PRDCNT				)); //TODO
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_DVFS_TIMING_CTRL1)	, P_Fld(   1			 , DVFS_TIMING_CTRL1_DMSHU_CNT		));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_REFPEND1)			, P_Fld(   0x5			 , REFPEND1_MPENDREFCNT_TH0 		) \
																	| P_Fld(   0x5			 , REFPEND1_MPENDREFCNT_TH1 		) \
																	| P_Fld(   0x5			 , REFPEND1_MPENDREFCNT_TH2 		) \
																	| P_Fld(   0x5			 , REFPEND1_MPENDREFCNT_TH3 		) \
																	| P_Fld(   0x5			 , REFPEND1_MPENDREFCNT_TH4 		) \
																	| P_Fld(   0x3			 , REFPEND1_MPENDREFCNT_TH5 		) \
																	| P_Fld(   0x3			 , REFPEND1_MPENDREFCNT_TH6 		) \
																	| P_Fld(   0x3			 , REFPEND1_MPENDREFCNT_TH7 		));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_CBT_WLEV_CTRL1) 	, P_Fld(   0x10 		 , CBT_WLEV_CTRL1_CATRAIN_INTV		) \
																	| P_Fld(   0x3			 , CBT_WLEV_CTRL1_CATRAINLAT		));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TX_SET0)			, P_Fld(   1			 , TX_SET0_DRSCLR_EN				) \
																	| P_Fld( !TMRRI_MODE	 , TX_SET0_RK_SCINPUT_OPT			));

	if(A_T->LP45_APHY_COMB_EN == 1)
	{
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TX_SET0)		, P_Fld(   1			 , TX_SET0_OE_DOWNGRADE 			));
	}
	//@Jouling, UI reloade path is updated. (DQSOSCR_SREF_TXUI_RELOAD_OPT)
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_DQSOSCR)			, P_Fld(   0			 , DQSOSCR_SREF_TXUI_RELOAD_OPT 	) \
																	| P_Fld(   1			 , DQSOSCR_SREF_TXPI_RELOAD_OPT 	));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_DUMMY_RD)			, P_Fld(   1			 , DUMMY_RD_DRS_SELFWAKE_DMYRD_DIS	) \
																	| P_Fld(   2			 , DUMMY_RD_RANK_NUM				) \
																	| P_Fld(   1			 , DUMMY_RD_DUMMY_RD_SW 			) \
																	| P_Fld(   1			 , DUMMY_RD_DQSG_DMYRD_EN			));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_DUMMY_RD_INTV)		, P_Fld(   0			 , DUMMY_RD_INTV_DUMMY_RD_CNT7		) \
																	| P_Fld(   1			 , DUMMY_RD_INTV_DUMMY_RD_CNT6		) \
																	| P_Fld(   1			 , DUMMY_RD_INTV_DUMMY_RD_CNT5		) \
																	| P_Fld(   0			 , DUMMY_RD_INTV_DUMMY_RD_CNT4		) \
																	| P_Fld(   1			 , DUMMY_RD_INTV_DUMMY_RD_CNT3		) \
																	| P_Fld(   0			 , DUMMY_RD_INTV_DUMMY_RD_CNT2		) \
																	| P_Fld(   0			 , DUMMY_RD_INTV_DUMMY_RD_CNT1		) \
																	| P_Fld(   0			 , DUMMY_RD_INTV_DUMMY_RD_CNT0		));
	//Byte Mode choose
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_RK_DQSOSC)			, P_Fld( p->dram_cbt_mode[RANK_0] , RK_DQSOSC_RK0_BYTE_MODE 		 ));
	vSetRank(p, RANK_1);
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_RK_DQSOSC)			, P_Fld( p->dram_cbt_mode[RANK_1] , RK_DQSOSC_RK0_BYTE_MODE 		 ));
	vSetRank(p, RANK_0);

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TX_TRACKING_SET0)	, P_Fld(   0			 , TX_TRACKING_SET0_TX_TRACKING_OPT ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TX_CG_SET0) 		, P_Fld(   1			 , TX_CG_SET0_SELPH_4LCG_DIS		));

	//DVFS support SRAM_EN only
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TX_FREQ_RATIO_OLD_MODE0), P_Fld(DV_p.SRAM_EN, TX_FREQ_RATIO_OLD_MODE0_SHUFFLE_LEVEL_MODE_SELECT));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL1)		, P_Fld(   1			 , SWCMD_CTRL1_WRFIFO_MODE2 		));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_DBG_CMDDEC_CMDSEL0) , P_Fld( M_LP4->EX_ROW_EN[0], DBG_CMDDEC_CMDSEL0_RANK0_10GBEN ) \
																	| P_Fld( M_LP4->EX_ROW_EN[1], DBG_CMDDEC_CMDSEL0_RANK1_10GBEN ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_DBIWR_PROTECT)		, P_Fld(   1			 , DBIWR_PROTECT_DBIWR_IMP_EN		 ) \
																	| P_Fld(   0			 , DBIWR_PROTECT_DBIWR_PINMUX_EN	 ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_RX_SET0)			, P_Fld(   1			 , RX_SET0_PRE_DLE_VLD_OPT			 ) \
																	| P_Fld(   7			 , RX_SET0_DATLAT_PDLE_TH			 ));

	//TODO SRAM MD32 control
	// @Darren, sync MP settings from Joe (APB will be available when SRAM DMA access)
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SRAM_DMA0)	, P_Fld(   1			 , MISC_SRAM_DMA0_PENABLE_LAT_WR	 ) \
																	| P_Fld(   1			 , MISC_SRAM_DMA0_KEEP_APB_ARB_ENA	 ) \
																	| P_Fld(   1			 , MISC_SRAM_DMA0_KEEP_SRAM_ARB_ENA  ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_MD32_REG_SSPM_MCLK_DIV), P_Fld(   1			 , SSPM_MCLK_DIV_MCLK_DCM_EN		 ));

	//Indivial random sync
	vIO32Write4B(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DBG_IRQ_CTRL1),   0xFFFFFFFF);
	vIO32Write4B(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DBG_IRQ_CTRL4),   0xFFFFFFFF);
	vIO32Write4B(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DBG_IRQ_CTRL7),   0xFFFFFFFF);
}

static void IO_Release(DRAMC_CTX_T *p)
{

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CTRL1)		, P_Fld(   1			 , MISC_CTRL1_R_DM_TX_ARCLK_OE		) \
																	| P_Fld(   1			 , MISC_CTRL1_R_DM_TX_ARCMD_OE		));
	if(LPDDR5_EN_S == 1)
	{
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ2)		, P_Fld(   1			 , B0_DQ2_RG_TX_ARWCK_OE_TIE_EN_B0	) \
																	| P_Fld(   1			 , B0_DQ2_RG_TX_ARWCK_OE_TIE_SEL_B0 ) \
																	| P_Fld(   1			 , B0_DQ2_RG_TX_ARWCKB_OE_TIE_EN_B0 ) \
																	| P_Fld(   1			 , B0_DQ2_RG_TX_ARWCKB_OE_TIE_SEL_B0) );
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ2)		, P_Fld(   1			 , B1_DQ2_RG_TX_ARWCK_OE_TIE_EN_B1	) \
																	| P_Fld(   1			 , B1_DQ2_RG_TX_ARWCK_OE_TIE_SEL_B1 ) \
																	| P_Fld(   1			 , B1_DQ2_RG_TX_ARWCKB_OE_TIE_EN_B1 ) \
																	| P_Fld(   1			 , B1_DQ2_RG_TX_ARWCKB_OE_TIE_SEL_B1) );
	}
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_CA_CMD7)			, P_Fld(   0			 , CA_CMD7_RG_TX_ARCLKB_PULL_DN 	) \
																	| P_Fld(   0			 , CA_CMD7_RG_TX_ARCLKB_PULL_UP 	) \
																	| P_Fld(   0			 , CA_CMD7_RG_TX_ARCLK_PULL_DN		) \
																	| P_Fld(   0			 , CA_CMD7_RG_TX_ARCLK_PULL_UP		) \
																	| P_Fld(   0			 , CA_CMD7_RG_TX_ARCS0_PULL_DN		) \
																	| P_Fld(   0			 , CA_CMD7_RG_TX_ARCS0_PULL_UP		) \
																	| P_Fld(   0			 , CA_CMD7_RG_TX_ARCMD_PULL_DN		) \
																	| P_Fld(   0			 , CA_CMD7_RG_TX_ARCMD_PULL_UP		));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ7)			, P_Fld(   0			 , B0_DQ7_RG_TX_ARDQS0B_PULL_DN_B0	) \
																	| P_Fld(   0			 , B0_DQ7_RG_TX_ARDQS0B_PULL_UP_B0	) \
																	| P_Fld(   0			 , B0_DQ7_RG_TX_ARDQS0_PULL_DN_B0	) \
																	| P_Fld(   0			 , B0_DQ7_RG_TX_ARDQS0_PULL_UP_B0	) \
																	| P_Fld(   0			 , B0_DQ7_RG_TX_ARDQM0_PULL_DN_B0	) \
																	| P_Fld(   0			 , B0_DQ7_RG_TX_ARDQM0_PULL_UP_B0	) \
																	| P_Fld(   0			 , B0_DQ7_RG_TX_ARDQ_PULL_DN_B0 	) \
																	| P_Fld(   0			 , B0_DQ7_RG_TX_ARDQ_PULL_UP_B0 	));

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ7)			, P_Fld(   0			 , B1_DQ7_RG_TX_ARDQS0B_PULL_DN_B1	) \
																	| P_Fld(   0			 , B1_DQ7_RG_TX_ARDQS0B_PULL_UP_B1	) \
																	| P_Fld(   0			 , B1_DQ7_RG_TX_ARDQS0_PULL_DN_B1	) \
																	| P_Fld(   0			 , B1_DQ7_RG_TX_ARDQS0_PULL_UP_B1	) \
																	| P_Fld(   0			 , B1_DQ7_RG_TX_ARDQM0_PULL_DN_B1	) \
																	| P_Fld(   0			 , B1_DQ7_RG_TX_ARDQM0_PULL_UP_B1	) \
																	| P_Fld(   0			 , B1_DQ7_RG_TX_ARDQ_PULL_DN_B1 	) \
																	| P_Fld(   0			 , B1_DQ7_RG_TX_ARDQ_PULL_UP_B1 	));
	//for dram spec CA_CMD2_RG_TX_ARCS_OE_TIE_EN_CA will help to fix CKE=0 before  for meet 10ns tINIT2
	//Assert DRAM reset PIN
	#if !SA_CONFIG_EN
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CTRL1)		, P_Fld(   1			 , MISC_CTRL1_R_DMDA_RRESETB_I		));
	#endif
}

static void DVFS_PRE_config(DRAMC_CTX_T *p)
{
#if (fcFOR_CHIP_ID == fcMargaux)
	U32 MCP_EN = 0; //remove for MCP timing issue
#else
	U32 MCP_EN = 1; //for MCP should adjust some setting between CHs (A-B/C-D)
#endif
	U32 REF_104M_EN = 1;
	DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
	mcSHOW_DBG_MSG(("Enter into  DVFS_PRE_config >>>>> \n"));

#if ENABLE_ECO_SRAM_DMA_MISS_REG
	vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ11  )  , P_Fld( 1 	, B0_DQ11_DMY_DQ11_B0					));
	vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ11  )  , P_Fld( 1 	, B1_DQ11_DMY_DQ11_B1					));
#endif

	vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DRAMC_REG_DVFS_CTRL0) 		, P_Fld(   1			 , DVFS_CTRL0_VRCG_EN								) \
																	| P_Fld(   0			 , DVFS_CTRL0_DVFS_SYNC_MASK						) \
																	| P_Fld(   1			 , DVFS_CTRL0_MR13_SHU_EN							) \
																	| P_Fld(   1			 , DVFS_CTRL0_HWSET_WLRL							) \
																	| P_Fld(   0			 , DVFS_CTRL0_MRWWOPRA								)); //Have to fix 0, 1 with bug (some bank without precharge)
	//for DVFS sync
	vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_RG_DFS_CTRL)	, P_Fld(   0			 , MISC_RG_DFS_CTRL_SPM_DVFS_CONTROL_SEL			));//SPM mode TODO should random  0 for SPM mode default
	vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SRAM_DMA0  )	, P_Fld(   0			 , MISC_SRAM_DMA0_DMA_TIMER_EN						));
	vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SRAM_DMA1  )	, P_Fld(   0x1ffff		   , MISC_SRAM_DMA1_SPM_RESTORE_STEP_EN 			  ));
	vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CG_CTRL7	 )	, P_Fld(   1			 , MISC_CG_CTRL7_ARMCTL_CK_OUT_CG_SEL				));
	vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DVFSCTL	 )	, P_Fld(   1			 , MISC_DVFSCTL_R_DVFS_PICG_POSTPONE				) \
																	| P_Fld(   1			 , MISC_DVFSCTL_R_DMSHUFFLE_CHANGE_FREQ_OPT 		));
	//for channel balance
	vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DVFSCTL2) 	, P_Fld(   0			 , MISC_DVFSCTL2_R_CDC_MUX_SEL_OPTION				) \
																	| P_Fld(   0			 , MISC_DVFSCTL2_R_DVFS_SYNC_MODULE_RST_SEL 		));
	vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DVFSCTL2) 	, P_Fld(   1			 , MISC_DVFSCTL2_R_DVFS_CDC_OPTION					));
	//Could be randomed
	vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DRAMC_REG_DVFS_CTRL0	  ) 	, P_Fld(   0			 , DVFS_CTRL0_DVFS_CKE_OPT							) \
																	| P_Fld(   1			 , DVFS_CTRL0_SCARB_PRI_OPT 						));
	//here is a flow??--TODO
	vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DVFSCTL3	)	, P_Fld(   1			 , MISC_DVFSCTL3_RG_PHY_ST_DELAY_AFT_CHG_TO_MCLK	) \
																	| P_Fld(   0			 , MISC_DVFSCTL3_RG_PHY_ST_DELAY_AFT_CHG_TO_BCLK	) \
																	| P_Fld(   0			 , MISC_DVFSCTL3_RG_PHY_ST_DELAY_BEF_CHG_TO_MCLK	) \
																	| P_Fld(   1			 , MISC_DVFSCTL3_RG_PHY_ST_DELAY_BEF_CHG_TO_BCLK	) \
																	| P_Fld(   3			 , MISC_DVFSCTL3_RG_DVFS_MEM_CK_SEL_DESTI			) \
																	| P_Fld(   1			 , MISC_DVFSCTL3_RG_DVFS_MEM_CK_SEL_SOURCE			) \
																	| P_Fld(   7			 , MISC_DVFSCTL3_RG_CNT_PHY_ST_DELAY_BEF_CHG_TO_BCLK) \
																	| P_Fld(   1			 , MISC_DVFSCTL3_RG_CNT_PHY_ST_DELAY_AFT_CHG_TO_MCLK) \
																	| P_Fld(   0x3f 		 , MISC_DVFSCTL3_RG_CNT_PHY_ST_DELAY_AFT_CHG_TO_BCLK) );

	vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CLK_CTRL	)	, P_Fld(   1			 , MISC_CLK_CTRL_DVFS_CLK_MEM_SEL					) \
																	| P_Fld(   1			 , MISC_CLK_CTRL_DVFS_MEM_CK_MUX_UPDATE_EN			));

	vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DVFSCTL3	)	, P_Fld(   0x10 		 , MISC_DVFSCTL3_RG_CNT_PHY_ST_DELAY_AFT_CHG_TO_BCLK));
	//flow end??

	vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DRAMC_REG_DVFS_TIMING_CTRL1)	, P_Fld(   1			 , DVFS_TIMING_CTRL1_DMSHU_CNT						)\
																	| P_Fld(   1			 , DVFS_TIMING_CTRL1_SHU_PERIOD_GO_ZERO_CNT 		));

	vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DVFSCTL2	 )	, P_Fld(   1			 , MISC_DVFSCTL2_R_DVFS_CDC_OPTION					)\
																	| P_Fld(   0			 , MISC_DVFSCTL2_R_DVFS_DLL_CHA 					)\
																	| P_Fld(   1			 , MISC_DVFSCTL2_RG_TOPCK_FMEM_CK_BLOCK_DURING_DFS					   )\
																	| P_Fld(   1			 , MISC_DVFSCTL2_R_DVFS_PARK_N						)\
																	| P_Fld(   1			 , MISC_DVFSCTL2_R_DVFS_OPTION						));

	vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CKMUX_SEL  )	, P_Fld( REF_104M_EN	 , MISC_CKMUX_SEL_RG_52M_104M_SEL					));

	//notice here:
	//*SHU_PHDET_SPM_EN = 1 means during DFS period as master. 2 means slave.
	vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_OPT	 )	, P_Fld(   1			 , MISC_SHU_OPT_R_DQB0_SHU_PHY_GATING_RESETB_SPM_EN )\
																	| P_Fld(   2			 , MISC_SHU_OPT_R_DQB0_SHU_PHDET_SPM_EN 			)\
																	| P_Fld(   1			 , MISC_SHU_OPT_R_DQB1_SHU_PHY_GATING_RESETB_SPM_EN )\
																	| P_Fld(   2			 , MISC_SHU_OPT_R_DQB1_SHU_PHDET_SPM_EN 			)\
																	| P_Fld(   1			 , MISC_SHU_OPT_R_CA_SHU_PHY_GATING_RESETB_SPM_EN	)\
																	| P_Fld(   1			 , MISC_SHU_OPT_R_CA_SHU_PHDET_SPM_EN				));

	vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DVFSCTL	 )	, P_Fld((REF_104M_EN==1)?3:1, MISC_DVFSCTL_R_DVFS_PICG_MARGIN_NEW			) \
																	| P_Fld((REF_104M_EN==1)?3:1, MISC_DVFSCTL_R_DVFS_PICG_MARGIN2_NEW			) \
																	| P_Fld((REF_104M_EN==1)?3:1, MISC_DVFSCTL_R_DVFS_PICG_MARGIN3_NEW			) );
	if(A_T->DLL_ASYNC_EN == 0)
	{
		mcSHOW_DBG_MSG(("Because of DLL_ASYNC_EN for indenpendent DLL NOT enable, salve channel's DVFS_DLL_CHA should set 0 to follow master CH's DLL. \n"));
		//DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
		vSetPHY2ChannelMapping(p, CHANNEL_B);
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DVFSCTL2	 )	, P_Fld(   0			 , MISC_DVFSCTL2_R_DVFS_DLL_CHA 				 ));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_OPT	 )	, P_Fld(   2			 , MISC_SHU_OPT_R_CA_SHU_PHDET_SPM_EN			 ));
		#if (CHANNEL_NUM>2)
		vSetPHY2ChannelMapping(p, CHANNEL_D);
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DVFSCTL2	 )	, P_Fld(   0			 , MISC_DVFSCTL2_R_DVFS_DLL_CHA 				 ));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SHU_OPT	 )	, P_Fld(   2			 , MISC_SHU_OPT_R_CA_SHU_PHDET_SPM_EN			 ));
		#endif
		vSetPHY2ChannelMapping(p, CHANNEL_A);
		//DramcBroadcastOnOff(DRAMC_BROADCAST_ON);
	}


	if(MCP_EN == 1)
	{
		//DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
		vSetPHY2ChannelMapping(p, CHANNEL_B);
		mcSHOW_DBG_MSG(("MCP Enable leading 2ch's sync singles should adjust delay margin."));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DVFSCTL	 )	, P_Fld((REF_104M_EN==1)?6:4, MISC_DVFSCTL_R_DVFS_PICG_MARGIN_NEW		 ));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DVFSCTL3	)	, P_Fld(   9				, MISC_DVFSCTL3_RG_CNT_PHY_ST_DELAY_BEF_CHG_TO_BCLK    ));
		#if (CHANNEL_NUM>2)
		vSetPHY2ChannelMapping(p, CHANNEL_D);
		mcSHOW_DBG_MSG(("MCP Enable leading 2ch's sync singles should adjust delay margin."));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DVFSCTL	 )	, P_Fld((REF_104M_EN==1)?6:4, MISC_DVFSCTL_R_DVFS_PICG_MARGIN_NEW		 ));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DVFSCTL3	)	, P_Fld(   9				, MISC_DVFSCTL3_RG_CNT_PHY_ST_DELAY_BEF_CHG_TO_BCLK    ));
		#endif
		vSetPHY2ChannelMapping(p, CHANNEL_A);
		//DramcBroadcastOnOff(DRAMC_BROADCAST_ON);
	}


	//DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CKMUX_SEL  )	, P_Fld(   1			 , MISC_CKMUX_SEL_FMEM_CK_MUX ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_DVFS_CTRL0) 		, P_Fld(   0			 , DVFS_CTRL0_R_DRAMC_CHA	  )\
																	| P_Fld(   0			 , DVFS_CTRL0_SHU_PHYRST_SEL  ));
	vSetPHY2ChannelMapping(p, CHANNEL_B);
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CKMUX_SEL  )	, P_Fld(   3			 , MISC_CKMUX_SEL_FMEM_CK_MUX ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_DVFS_CTRL0) 		, P_Fld(   0			 , DVFS_CTRL0_R_DRAMC_CHA	  )\
																	| P_Fld(   1			 , DVFS_CTRL0_SHU_PHYRST_SEL  ));
	#if (CHANNEL_NUM>2)
	vSetPHY2ChannelMapping(p, CHANNEL_C);
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CKMUX_SEL  )	, P_Fld(   1			 , MISC_CKMUX_SEL_FMEM_CK_MUX ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_DVFS_CTRL0) 		, P_Fld(   0			 , DVFS_CTRL0_R_DRAMC_CHA	  )\
																	| P_Fld(   0			 , DVFS_CTRL0_SHU_PHYRST_SEL  ));
	vSetPHY2ChannelMapping(p, CHANNEL_D);
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CKMUX_SEL  )	, P_Fld(   3			 , MISC_CKMUX_SEL_FMEM_CK_MUX ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_DVFS_CTRL0) 		, P_Fld(   0			 , DVFS_CTRL0_R_DRAMC_CHA	  )\
																	| P_Fld(   1			 , DVFS_CTRL0_SHU_PHYRST_SEL  ));
	#endif
	vSetPHY2ChannelMapping(p, CHANNEL_A);
	DramcBroadcastOnOff(DRAMC_BROADCAST_ON);
	mcSHOW_DBG_MSG(("Exit from	DVFS_PRE_config <<<<< \n"));
}

void DIG_STATIC_SETTING(DRAMC_CTX_T *p)
{
	DIG_PHY_config(p);
	GATING_MODE_CFG(&Gat_p);
	DPHY_GAT_TRACK_Config(p,&Gat_p);
	DRAMC_COMMON_Config(p);
	#if 1//!SA_CONFIG_EN
	DVFS_PRE_config(p);//for DVFS initial config.-- bring-up no need this code. but DVFS will need this
	#endif
	DDRPHY_PICG_Config(p);
	IO_Release(p);
	RX_INTPUT_Config(p);//TODO dummy write trigger
}

#if 0
void DPI_DIG_init(void)
{
	mysetscope();
	DIG_STATIC_SETTING(DramcConfig);
}
#endif
