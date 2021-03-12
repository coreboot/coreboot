/* SPDX-License-Identifier: BSD-3-Clause */

#include "dramc_dv_init.h"

void CKE_FIX_ON(DRAMC_CTX_T *p, U8 EN, U8 rank)
{
	switch(rank)
	{
		case 0 : vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL), P_Fld(EN, CKECTRL_CKEFIXON));  break;
		case 1 : vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL), P_Fld(EN, CKECTRL_CKE1FIXON)); break;
		default: msg("ERROR: CKE FIX ON error. Unexpected Rank	\n");
	}
}
//[SV] task LP4_MRS(bit [7:0] reg_addr, bit[7:0] reg_op, bit[1:0] rank);
static void LP4_MRS(DRAMC_CTX_T *p, U16 reg_addr, U8 reg_op, U8 rank)
{
	U8 temp_MRS_RESPONSE  ;

	msg("[LP4_DRAM_INIT_MRS] RK:%1d-MA:%2d-OP:0x%2x @Channle:%1d\n",rank,reg_addr,reg_op,vGetPHY2ChannelMapping(p));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL0), P_Fld(rank , SWCMD_CTRL0_MRSRK	   ) \
															| P_Fld(reg_addr, SWCMD_CTRL0_MRSMA    ) \
															| P_Fld(reg_op	, SWCMD_CTRL0_MRSOP    ));

	vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), 0);
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), P_Fld(1, SWCMD_EN_MRWEN));

	temp_MRS_RESPONSE = 0 ;
	do
	{
		temp_MRS_RESPONSE = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SPCMDRESP), SPCMDRESP_MRW_RESPONSE) ;
	} while ( temp_MRS_RESPONSE != 1 );

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), P_Fld(0, SWCMD_EN_MRWEN));
}


static void LP4_FSP_WR_or_OP (DRAMC_CTX_T *p, U8 FSP_WR, U8 FSP_OP, U8 rank)
{
	U8 MR13 = 0;
	MR13 = ((FSP_OP & 1) << 7) | ((FSP_WR & 1) << 6) /*| ((LP4_DMI & 1) << 5)*/ | (1 << 4)/*[RRO] for MR4 refresh rate*/;
	LP4_MRS(p, 13, MR13, rank);
}

//==================================
//uesage(constraint): DBI = 1 for FSPOP=1	if DBI=0 then FSP_OP =0
//==================================
static void lp4_dram_init_single_rank(DRAMC_CTX_T *p,LP4_DRAM_CONFIG_T *tr,U8 rank)
{
	U8 MR1;
	U8 MR2;
	U8 MR3;
	U8 MR51;
	U8 MR11;
	U8 MR12;
	U8 MR14;

	//default value for LP4 DRAM CONFIG
	U8 nWR	  =5;
	U8 WR_LEV =0;
	U8 PDDS   =5;
	U8 PPRP   =0;
	U8 PU_CAL =0;
	U8 WLS	  =0;

	//Notice: DBI default = 0

	//field & configuration adaption
	MR1 = ((tr->RPST	& 1)<<7) | ((nWR		 & 7)<<4) | ((tr->RD_PRE  & 1)<<3) | ((tr->WR_PRE & 1)<<2) | ((tr->BL & 3)<<0);
	MR2 = ((WR_LEV		& 1)<<7) | ((WLS		 & 1)<<6) | ((tr->MR_WL   & 7)<<3) | ((tr->MR_RL  & 7)<<0);
	MR3 = ((tr->DBI_WR	& 1)<<7) | ((tr->DBI_RD  & 1)<<6) | ((	 PDDS	  & 7)<<3) | ((PPRP 	  & 1)<<2) | ((tr->WR_PST & 1)<<1) | ((PU_CAL & 1)<<0);
	MR51= ((tr->LP4Y_EN & 1)<<3) | ((tr->LP4Y_EN & 1)<<2) | ((tr->LP4Y_EN & 1)<<1);
	if(tr->WORK_FSP == 0)
	{
		MR11 = 0x0;
		MR14 = 0x5d;
	}
	else
	{
		MR11 = 0x04;
		MR14 = 0x18;
	}
	MR12= 0x5d;
	#if FSP1_CLKCA_TERM
	if(p->dram_fsp == FSP_1)
		MR12 = 0x20;
	#endif
	//temp workaround for global variable of MR
	u1MR02Value[tr->WORK_FSP] = MR2;
	u1MR03Value[tr->WORK_FSP] = MR3;
	#if ENABLE_LP4_ZQ_CAL
	DramcZQCalibration(p, rank); //ZQ calobration should be done before CBT calibration by switching to low frequency
	#endif

	msg("[LP4_DRAM_INIT] Channle:%1d-Rank:%1d >>>>>>\n",vGetPHY2ChannelMapping(p),rank);

	//first FSP
	if(tr->WORK_FSP == 0) {LP4_FSP_WR_or_OP(p, 0, 1, rank);}
	else				  {LP4_FSP_WR_or_OP(p, 1, 0, rank);}

	mcDELAY_XNS(15); //TCKFSPE

	LP4_MRS(p,	1, MR1 , rank);
	LP4_MRS(p,	2, MR2 , rank);
	LP4_MRS(p,	3, MR3 , rank);
	LP4_MRS(p,	11, MR11 , rank);
	LP4_MRS(p,	12, MR12 , rank);
	LP4_MRS(p,	14, MR14 , rank);
	if(tr->LP4Y_EN == 1)  { LP4_MRS(p, 51, MR51, rank); }

	mcDELAY_XNS(15); //TCKFSPX

	//2nd FSP
	if(tr->WORK_FSP == 0) {LP4_FSP_WR_or_OP(p, 1, 0, rank);}
	else				  {LP4_FSP_WR_or_OP(p, 0, 1, rank);}

	mcDELAY_XNS(15); //TCKFSPE

	LP4_MRS(p,	1, MR1 , rank);
	LP4_MRS(p,	2, MR2 , rank);
	//reverse the DBI
	MR3 = ((!tr->DBI_WR  & 1)<<7) | ((!tr->DBI_RD  & 1)<<6) | ((   PDDS 	& 7)<<3) | ((PPRP		& 1)<<2) | ((tr->WR_PST & 1)<<1) | ((PU_CAL & 1)<<0);
	LP4_MRS(p,	3, MR3 , rank);
	LP4_MRS(p,	11, MR11 , rank);
	LP4_MRS(p,	12, MR12 , rank);
	LP4_MRS(p,	14, MR14 , rank);

	LP4_FSP_WR_or_OP(p, tr->WORK_FSP, tr->WORK_FSP, rank);

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_CA_CMD7)	  , P_Fld( 1	   , CA_CMD7_RG_TX_ARCLKB_PULL_DN_LP4Y		 ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ7)	  , P_Fld( 1	   , B0_DQ7_RG_TX_ARDQS0B_PULL_DN_B0_LP4Y	 ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ7)	  , P_Fld( 1	   , B1_DQ7_RG_TX_ARDQS0B_PULL_DN_B1_LP4Y	 ));

	msg("[LP4_DRAM_INIT] Channle:%1d-Rank:%1d <<<<<<\n",vGetPHY2ChannelMapping(p),rank);
}

void LP4_single_end_DRAMC_post_config(DRAMC_CTX_T *p, U8 LP4Y_EN)
{
	msg("============ LP4 DIFF to SE enter ============\n");
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD13), P_Fld( LP4Y_EN , SHU_CA_CMD13_RG_TX_ARCLKB_OE_TIE_SEL_CA ) \
															  | P_Fld( LP4Y_EN		 , SHU_CA_CMD13_RG_TX_ARCLKB_OE_TIE_EN_CA  ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ13) , P_Fld( LP4Y_EN , SHU_B0_DQ13_RG_TX_ARDQSB_OE_TIE_SEL_B0  ) \
															  | P_Fld( LP4Y_EN		 , SHU_B0_DQ13_RG_TX_ARDQSB_OE_TIE_EN_B0   ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ13) , P_Fld( LP4Y_EN , SHU_B1_DQ13_RG_TX_ARDQSB_OE_TIE_SEL_B1  ) \
															  | P_Fld( LP4Y_EN		 , SHU_B1_DQ13_RG_TX_ARDQSB_OE_TIE_EN_B1   ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD0) , P_Fld( 0 , SHU_CA_CMD0_R_LP4Y_WDN_MODE_CLK		   ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ0)  , P_Fld( 0 , SHU_B0_DQ0_R_LP4Y_WDN_MODE_DQS0		   ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ0)  , P_Fld( 0 , SHU_B1_DQ0_R_LP4Y_WDN_MODE_DQS1		   ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD7) , P_Fld( 0 , SHU_CA_CMD7_R_LP4Y_SDN_MODE_CLK		   )); //@Darren, debugging for DFS stress fail
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_DQ7)  , P_Fld( LP4Y_EN , SHU_B0_DQ7_R_LP4Y_SDN_MODE_DQS0		 ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B1_DQ7)  , P_Fld( LP4Y_EN , SHU_B1_DQ7_R_LP4Y_SDN_MODE_DQS1		 ));

#if 1//ENABLE_LP4Y_DFS // @Darren, need confirm
	// for strong pull low and normal mode
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_CA_CMD7)	  , P_Fld( 0 , CA_CMD7_RG_TX_ARCLKB_PULL_DN_LP4Y	   ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ7)	  , P_Fld( 0 , B0_DQ7_RG_TX_ARDQS0B_PULL_DN_B0_LP4Y    ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ7)	  , P_Fld( 0 , B1_DQ7_RG_TX_ARDQS0B_PULL_DN_B1_LP4Y    ));
#else
	// for weak pull low mode only
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_CA_CMD7)	  , P_Fld( 1 , CA_CMD7_RG_TX_ARCLKB_PULL_DN_LP4Y	   ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B0_DQ7)	  , P_Fld( 1 , B0_DQ7_RG_TX_ARDQS0B_PULL_DN_B0_LP4Y    ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_B1_DQ7)	  , P_Fld( 1 , B1_DQ7_RG_TX_ARDQS0B_PULL_DN_B1_LP4Y    ));
#endif
	msg("============ LP4 DIFF to SE exit  ============\n");
}

void LP4_DRAM_INIT(DRAMC_CTX_T *p)
{
	U8 RANK;


#if SA_CONFIG_EN && DV_SIMULATION_DFS// @Darren, temp workaround
	DramcPowerOnSequence(p);
#endif


	mcDELAY_XNS(200); //tINIT3 = 2ms  for DV fastup to 200ns

	for(RANK=0;RANK<2;RANK++)
	{
		CKE_FIX_ON(p,1,RANK);
		mcDELAY_XNS(400); //tINIT5	fastup to 400ns

		//step4 moderegister setting
		lp4_dram_init_single_rank(p,DV_p.lp4_init,RANK);
		CKE_FIX_ON(p,0,RANK);
	}

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_REFCTRL0), P_Fld(0, REFCTRL0_REFDIS)); //TODO enable refresh
}
