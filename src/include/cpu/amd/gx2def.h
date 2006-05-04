#ifndef CPU_AMD_GX2DEF_H
#define CPU_AMD_GX2DEF_H
#define	CPU_ID_1_X							0x540		/* Stepping ID 1.x*/
#define	CPU_ID_2_0							0x551		/* Stepping ID 2.0*/
#define	CPU_ID_2_1							0x552		/* Stepping ID 2.1*/
#define	CPU_ID_2_2							0x553		/* Stepping ID 2.2*/

#define	CPU_REV_1_0							0x011
#define	CPU_REV_1_1							0x012
#define	CPU_REV_1_2							0x013
#define	CPU_REV_1_3							0x014
#define	CPU_REV_2_0							0x020
#define	CPU_REV_2_1							0x021
#define	CPU_REV_2_2							0x022
#define	CPU_REV_3_0							0x030
/* GeodeLink Control Processor Registers, GLIU1, Port 3 */
#define	GLCP_CLK_DIS_DELAY	0x4c000008
#define	GLCP_PMCLKDISABLE	0x4c000009
#define	GLCP_CHIP_REVID		0x4c000017

/* GLCP_SYS_RSTPLL, Upper 32 bits */
#define	GLCP_SYS_RSTPLL_MDIV_SHIFT   9
#define	GLCP_SYS_RSTPLL_VDIV_SHIFT   6
#define	GLCP_SYS_RSTPLL_FBDIV_SHIFT  0

/* GLCP_SYS_RSTPLL, Lower 32 bits */
#define	GLCP_SYS_RSTPLL_SWFLAGS_SHIFT	26
#define	GLCP_SYS_RSTPLL_SWFLAGS_MASK 	(0x3f << 26)

#define	GLCP_SYS_RSTPLL_LOCKWAIT	24
#define	GLCP_SYS_RSTPLL_HOLDCOUNT	16
#define	GLCP_SYS_RSTPLL_BYPASS		15
#define	GLCP_SYS_RSTPLL_PD		14
#define	GLCP_SYS_RSTPLL_RESETPLL	13
#define	GLCP_SYS_RSTPLL_DDRMODE		10
#define	GLCP_SYS_RSTPLL_VA_SEMI_SYNC_MODE   9
#define	GLCP_SYS_RSTPLL_PCI_SEMI_SYNC_MODE  8
#define	GLCP_SYS_RSTPLL_CHIP_RESET	    0

/* MSR routing as follows*/
/* MSB = 1 means not for CPU*/
/* next 3 bits 1st port*/
/* next3 bits next port if through an GLIU*/
/* etc...*/

/*Redcloud as follows.*/
/* GLIU0*/
/*	port0 - GLIU0*/
/*	port1 - MC*/
/*	port2 - GLIU1*/
/*	port3 - CPU*/
/*	port4 - VG*/
/*	port5 - GP*/
/*	port6 - DF*/

/* GLIU1*/
/*	port1 - GLIU0*/
/*	port3 - GLCP*/
/*	port4 - PCI*/
/*	port5 - FG*/


#define	GL0_GLIU0			0
#define	GL0_MC				1
#define	GL0_GLIU1			2
#define	GL0_CPU				3
#define	GL0_VG				4
#define	GL0_GP				5
#define	GL0_DF				6

#define	GL1_GLIU0			1
#define	GL1_GLCP			3
#define	GL1_PCI				4
#define	GL1_FG				5
#define GL1_VIP				5
#define GL1_AES				6

#define	MSR_GLIU0			(GL0_GLIU0 << 29) + (1 << 28) /* To get on GeodeLink one bit has to be set */
#define	MSR_MC				(GL0_MC		<< 29)
#define	MSR_GLIU1			(GL0_GLIU1	<< 29)
#define	MSR_CPU				(GL0_CPU	<< 29) /* this is not used for BIOS since code executing on CPU doesn't need to be routed*/
#define	MSR_VG				(GL0_VG		<< 29)
#define	MSR_GP				(GL0_GP		<< 29)
#define	MSR_DF				(GL0_DF		<< 29)

#define	MSR_GLCP			(GL1_GLCP << 26) + MSR_GLIU1
#define	MSR_PCI				(GL1_PCI << 26) + MSR_GLIU1
#define	MSR_FG				(GL1_FG << 26) + MSR_GLIU1
#define 	MSR_VIP				((GL1_VIP << 26) + MSR_GLIU1)
#define 	MSR_AES				((GL1_AES << 26) + MSR_GLIU1)
/* South Bridge*/
#define SB_PORT	2			/* port of the SouthBridge */
#define	MSR_SB				((SB_PORT << 23) + MSR_PCI)	/* address to the SouthBridge*/
#define	SB_SHIFT			20							/* 29 -> 26 -> 23 -> 20...... When making a SB address uses this shift.*/


/**/
/*GeodeLink Interface Unit 0 (GLIU0) port0*/
/**/

#define	GLIU0_GLD_MSR_CAP			(MSR_GLIU0 + 0x2000)
#define	GLIU0_GLD_MSR_PM			(MSR_GLIU0 + 0x2004)

#define	GLIU0_DESC_BASE				(MSR_GLIU0 + 0x20)
#define	GLIU0_CAP					(MSR_GLIU0 + 0x86)
#define	GLIU0_GLD_MSR_COH			(MSR_GLIU0 + 0x80)


/**/
/* Memory Controller GLIU0 port 1*/
/**/
#define	MC_GLD_MSR_CAP			(MSR_MC + 0x2000)
#define	MC_GLD_MSR_PM			(MSR_MC + 0x2004)

#define	MC_CF07_DATA			(MSR_MC + 0x18)

#define		CF07_UPPER_D1_SZ_SHIFT				28
#define		CF07_UPPER_D1_MB_SHIFT				24
#define		CF07_UPPER_D1_CB_SHIFT				20
#define		CF07_UPPER_D1_PSZ_SHIFT				16
#define		CF07_UPPER_D0_SZ_SHIFT				12
#define		CF07_UPPER_D0_MB_SHIFT				8
#define		CF07_UPPER_D0_CB_SHIFT				4
#define		CF07_UPPER_D0_PSZ_SHIFT				0

#define		CF07_LOWER_REF_INT_SHIFT				8
#define		CF07_LOWER_LOAD_MODE_DDR_SET		(1 << 28)
#define		CF07_LOWER_LOAD_MODE_DLL_RESET		(1 << 27)
#define		CF07_LOWER_EMR_QFC_SET				(1 << 26)
#define		CF07_LOWER_EMR_DRV_SET				(1 << 25)
#define		CF07_LOWER_REF_TEST_SET				(1 << 3)
#define		CF07_LOWER_PROG_DRAM_SET			(1 << 0)


#define	MC_CF8F_DATA			(MSR_MC + 0x19)

#define		CF8F_UPPER_XOR_BS_SHIFT				19
#define		CF8F_UPPER_XOR_MB0_SHIFT			18
#define		CF8F_UPPER_XOR_BA1_SHIFT			17
#define		CF8F_UPPER_XOR_BA0_SHIFT			16
#define		CF8F_UPPER_REORDER_DIS_SET		(1 << 8)
#define		CF8F_UPPER_REG_DIMM_SHIFT			4
#define		CF8F_LOWER_CAS_LAT_SHIFT			28
#define		CF8F_LOWER_REF2ACT_SHIFT			24
#define		CF8F_LOWER_ACT2PRE_SHIFT			20
#define		CF8F_LOWER_PRE2ACT_SHIFT			16
#define		CF8F_LOWER_ACT2CMD_SHIFT			12
#define		CF8F_LOWER_ACT2ACT_SHIFT			8
#define		CF8F_UPPER_32BIT_SET				(1 << 5)
#define		CF8F_UPPER_HOI_LOI_SET			(1 << 1)

#define	MC_CF1017_DATA			(MSR_MC + 0x1A)

#define		CF1017_LOWER_PM1_UP_DLY_SET			(1 << 8)
#define		CF1017_LOWER_WR2DAT_SHIFT			0

#define	MC_CFCLK_DBUG			(MSR_MC + 0x1D)

#define		CFCLK_UPPER_MTST_B2B_DIS_SET			(1 << 2)
#define		CFCLK_UPPER_MTST_DQS_EN_SET			(1 << 1)
#define		CFCLK_UPPER_MTEST_EN_SET				(1 << 0)

#define		CFCLK_LOWER_MASK_CKE_SET1			(1 << 9)
#define		CFCLK_LOWER_MASK_CKE_SET0			(1 << 8)
#define		CFCLK_LOWER_SDCLK_SET				(0x0F << 0)

#define	MC_CF_RDSYNC			(MSR_MC	+ 0x1F)


/**/
/* GLIU1 GLIU0 port2*/
/**/
#define	GLIU1_GLD_MSR_CAP			(MSR_GLIU1 + 0x2000)
#define	GLIU1_GLD_MSR_PM			(MSR_GLIU1 + 0x2004)

#define	GLIU1_GLD_MSR_COH			(MSR_GLIU1 + 0x80)


/**/
/* CPU	; does not need routing instructions since we are executing there.*/
/**/
#define	CPU_GLD_MSR_CAP						0x2000
#define	CPU_GLD_MSR_CONFIG					0x2001
#define	CPU_GLD_MSR_PM						0x2004

#define	CPU_GLD_MSR_DIAG					0x2005
#define		DIAG_SEL1_MODE_SHIFT				16
#define		DIAG_SEL1_SET						(1 << 31)
#define		DIAG_SEL0__MODE_SHIFT				0
#define		DIAG_SET0_SET						(1 << 15)

#define	CPU_PF_BTB_CONF						0x1100
#define		BTB_ENABLE_SET						(1 << 0)
#define		RETURN_STACK_ENABLE_SET				(1 << 4)
#define	CPU_PF_BTBRMA_BIST					0x110C

#define	CPU_XC_CONFIG						0x1210
#define		XC_CONFIG_SUSP_ON_HLT				(1 << 0)
#define	CPU_ID_CONFIG						0x1250
#define		ID_CONFIG_SERIAL_SET				(1 << 0)

#define	CPU_AC_MSR							0x1301
#define	CPU_EX_BIST							0x1428

/*IM*/
#define	CPU_IM_CONFIG							0x1700
#define		IM_CONFIG_LOWER_ICD_SET					(1 << 8)
#define		IM_CONFIG_LOWER_QWT_SET					(1 << 20)
#define	CPU_IC_INDEX							0x1710
#define	CPU_IC_DATA								0x1711
#define	CPU_IC_TAG								0x1712
#define	CPU_IC_TAG_I							0x1713
#define	CPU_ITB_INDEX							0x1720
#define	CPU_ITB_LRU								0x1721
#define	CPU_ITB_ENTRY							0x1722
#define	CPU_ITB_ENTRY_I							0x1723
#define	CPU_IM_BIST_TAG							0x1730
#define	CPU_IM_BIST_DATA						0x1731


/* various CPU MSRs */
#define CPU_DM_CONFIG0 0x1800
#define	DM_CONFIG0_UPPER_WSREQ_SHIFT  12
#define	DM_CONFIG0_LOWER_DCDIS_SET		(1<<8)
#define	DM_CONFIG0_LOWER_WBINVD_SET	(1<<5)
#define	DM_CONFIG0_LOWER_MISSER_SET		(1<<1)
/* configuration MSRs */
#define	CPU_RCONF_DEFAULT						0x1808
#define		RCONF_DEFAULT_UPPER_ROMRC_SHIFT				24
#define		RCONF_DEFAULT_UPPER_ROMBASE_SHIFT			4
#define		RCONF_DEFAULT_UPPER_DEVRC_HI_SHIFT			0
#define		RCONF_DEFAULT_LOWER_DEVRC_LOW_SHIFT			28
#define		RCONF_DEFAULT_LOWER_SYSTOP_SHIFT			8
#define		RCONF_DEFAULT_LOWER_SYSRC_SHIFT				0

#define	CPU_RCONF_BYPASS						0x180A
#define	CPU_RCONF_A0_BF							0x180B
#define	CPU_RCONF_C0_DF							0x180C
#define	CPU_RCONF_E0_FF							0x180D

#define	CPU_RCONF_SMM							0x180E
#define		RCONF_SMM_UPPER_SMMTOP_SHIFT			12
#define		RCONF_SMM_UPPER_RCSMM_SHIFT				0
#define		RCONF_SMM_LOWER_SMMBASE_SHIFT			12
#define		RCONF_SMM_LOWER_RCNORM_SHIFT			0
#define		RCONF_SMM_LOWER_EN_SET					(1<<8)

#define	CPU_RCONF_DMM							0x180F
#define		RCONF_DMM_UPPER_DMMTOP_SHIFT			12
#define		RCONF_DMM_UPPER_RCDMM_SHIFT				0
#define		RCONF_DMM_LOWER_DMMBASE_SHIFT			12
#define		RCONF_DMM_LOWER_RCNORM_SHIFT			0
#define		RCONF_DMM_LOWER_EN_SET					(1<<8)

#define	CPU_RCONF0							0x1810
#define	CPU_RCONF1							0x1811
#define	CPU_RCONF2							0x1812
#define	CPU_RCONF3							0x1813
#define	CPU_RCONF4							0x1814
#define	CPU_RCONF5							0x1815
#define	CPU_RCONF6							0x1816
#define	CPU_RCONF7							0x1817
#define	CPU_CR1_MSR							0x1881
#define	CPU_CR2_MSR							0x1882
#define	CPU_CR3_MSR							0x1883
#define	CPU_CR4_MSR							0x1884
#define	CPU_DC_INDEX						0x1890
#define	CPU_DC_DATA							0x1891
#define	CPU_DC_TAG							0x1892
#define	CPU_DC_TAG_I						0x1893
#define	CPU_SNOOP							0x1894
#define	CPU_DTB_INDEX						0x1898
#define	CPU_DTB_LRU							0x1899
#define	CPU_DTB_ENTRY						0x189A
#define	CPU_DTB_ENTRY_I						0x189B
#define	CPU_L2TB_INDEX						0x189C
#define	CPU_L2TB_LRU						0x189D
#define	CPU_L2TB_ENTRY						0x189E
#define	CPU_L2TB_ENTRY_I					0x189F
#define	CPU_DM_BIST							0x18C0
		/* SMM*/
#define	CPU_AC_SMM_CTL						0x1301
#define		SMM_NMI_EN_SET					(1<<0)
#define		SMM_SUSP_EN_SET					(1<<1)
#define		NEST_SMI_EN_SET					(1<<2)
#define		SMM_INST_EN_SET					(1<<3)
#define		INTL_SMI_EN_SET					(1<<4)
#define		EXTL_SMI_EN_SET					(1<<5)
	
#define	CPU_FPU_MSR_MODE					0x1A00
#define		FPU_IE_SET						(1<<0)
	
#define	CPU_FP_UROM_BIST					0x1A03
	
#define	CPU_BC_CONF_0						0x1900
#define		TSC_SUSP_SET				 (1<<5)
#define		SUSP_EN_SET				 (1<<12)
	
	/**/
	/*	VG GLIU0 port4*/
	/**/
	
#define	VG_GLD_MSR_CAP				(MSR_VG + 0x2000)
#define	VG_GLD_MSR_CONFIG			(MSR_VG + 0x2001)
#define	VG_GLD_MSR_PM				(MSR_VG + 0x2004)

#define	GP_GLD_MSR_CAP				(MSR_GP + 0x2000)
#define	GP_GLD_MSR_CONFIG			(MSR_GP + 0x2001)
#define	GP_GLD_MSR_PM				(MSR_GP + 0x2004)



/**/
/*	DF GLIU0 port6*/
/**/

#define	DF_GLD_MSR_CAP					(MSR_DF + 0x2000)
#define	DF_GLD_MSR_MASTER_CONF			(MSR_DF + 0x2001)
#define		DF_LOWER_LCD_SHIFT				6
#define	DF_GLD_MSR_PM					(MSR_DF + 0x2004)



/**/
/* GeodeLink Control Processor GLIU1 port3*/
/**/
#define	GLCP_GLD_MSR_CAP			(MSR_GLCP + 0x2000)
#define	GLCP_GLD_MSR_CONF			(MSR_GLCP + 0x2001)
#define	GLCP_GLD_MSR_PM				(MSR_GLCP + 0x2004)

#define	GLCP_DELAY_CONTROLS			(MSR_GLCP + 0x0F)

#define	GLCP_SYS_RSTPLL				(MSR_GLCP +0x14	/* R/W*/)
#define		RSTPLL_UPPER_MDIV_SHIFT				9
#define		RSTPLL_UPPER_VDIV_SHIFT				6
#define		RSTPLL_UPPER_FBDIV_SHIFT			0
	
#define		RSTPLL_LOWER_SWFLAGS_SHIFT			26
#define		RSTPLL_LOWER_SWFLAGS_MASK			(0x3F<<RSTPLL_LOWER_SWFLAGS_SHIFT)

#define		RSTPPL_LOWER_HOLD_COUNT_SHIFT			16
#define		RSTPPL_LOWER_BYPASS_SHIFT			15
#define		RSTPPL_LOWER_TST_SHIFT				11
#define		RSTPPL_LOWER_SDRMODE_SHIFT		 10
#define		RSTPPL_LOWER_BOOTSTRAP_SHIFT		 4

#define		RSTPPL_LOWER_LOCK_SET				(1<<25)
#define		RSTPPL_LOWER_LOCKWAIT_SET			(1<<24)
#define		RSTPPL_LOWER_BYPASS_SET				(1<<15)
#define		RSTPPL_LOWER_PD_SET					(1<<14)
#define		RSTPPL_LOWER_PLL_RESET_SET			(1<<13)
#define		RSTPPL_LOWER_SDRMODE_SET		 (1<<10)
#define		RSTPPL_LOWER_CPU_SEMI_SYNC_SET		 (1<<9)
#define		RSTPPL_LOWER_PCI_SEMI_SYNC_SET		 (1<<8)
#define		RSTPPL_LOWER_CHIP_RESET_SET		 (1<<0)

#define	GLCP_DOTPLL				(MSR_GLCP + 0x15	/* R/W*/)
#define		DOTPPL_LOWER_PD_SET				 (1<<14)


/**/
/*  GLIU1 port 4*/
/**/
#define	GLPCI_GLD_MSR_CAP			(MSR_PCI + 0x2000)
#define	GLPCI_GLD_MSR_CONFIG			(MSR_PCI + 0x2001)
#define	GLPCI_GLD_MSR_PM				(MSR_PCI + 0x2004)

#define	GLPCI_CTRL			(MSR_PCI + 0x2010)
#define	GLPCI_CTRL_UPPER_FTH_SHIFT				28
#define	GLPCI_CTRL_UPPER_RTH_SHIFT				24
#define	GLPCI_CTRL_UPPER_SBRTH_SHIFT				20
#define	GLPCI_CTRL_UPPER_DTL_SHIFT				14
#define	GLPCI_CTRL_UPPER_WTO_SHIFT				11
#define	GLPCI_CTRL_UPPER_LAT_SHIFT				3
#define	GLPCI_CTRL_UPPER_ILTO_SHIFT				8
#define	GLPCI_CTRL_LOWER_IRFT_SHIFT				18
#define	GLPCI_CTRL_LOWER_IRFC_SHIFT				16
#define	GLPCI_CTRL_LOWER_ER_SET					(1<<11)
#define	GLPCI_CTRL_LOWER_LDE_SET					(1<<9)
#define	GLPCI_CTRL_LOWER_OWC_SET					(1<<4)
#define	GLPCI_CTRL_LOWER_IWC_SET					(1<<3)
#define	GLPCI_CTRL_LOWER_PCD_SET					(1<<2)
#define	GLPCI_CTRL_LOWER_ME_SET					(1<<0)

#define	GLPCI_ARB			(MSR_PCI + 0x2011)
#define	GLPCI_ARB_UPPER_BM1_SET					(1<<17)
#define	GLPCI_ARB_UPPER_BM0_SET					(1<<16)
#define	GLPCI_ARB_UPPER_CPRE_SET					(1<<15)
#define	GLPCI_ARB_UPPER_PRE2_SET					(1<<10)
#define	GLPCI_ARB_UPPER_PRE1_SET					(1<<9)
#define	GLPCI_ARB_UPPER_PRE0_SET					(1<<8)
#define	GLPCI_ARB_UPPER_CRME_SET					(1<<7)
#define	GLPCI_ARB_UPPER_RME2_SET					(1<<2)
#define	GLPCI_ARB_UPPER_RME1_SET					(1<<1)
#define	GLPCI_ARB_UPPER_RME0_SET					(1<<0)
#define	GLPCI_ARB_LOWER_PRCM_SHIFT				24
#define	GLPCI_ARB_LOWER_FPVEC_SHIFT				16
#define	GLPCI_ARB_LOWER_RMT_SHIFT				6
#define	GLPCI_ARB_LOWER_IIE_SET					(1<<8)
#define	GLPCI_ARB_LOWER_PARK_SET					(1<<0)

#define	GLPCI_REN			(MSR_PCI + 0x2014)
#define	GLPCI_A0_BF			(MSR_PCI + 0x2015)
#define	GLPCI_C0_DF			(MSR_PCI + 0x2016)
#define	GLPCI_E0_FF			(MSR_PCI + 0x2017)
#define	GLPCI_RC0			(MSR_PCI + 0x2018)
#define	GLPCI_RC1			(MSR_PCI + 0x2019)
#define	GLPCI_RC2			(MSR_PCI + 0x201A)
#define	GLPCI_RC3			(MSR_PCI + 0x201B)
#define	GLPCI_RC4			(MSR_PCI + 0x201C)
#define		GLPCI_RC_UPPER_TOP_SHIFT				12
#define		GLPCI_RC_LOWER_BASE_SHIFT			12
#define		GLPCI_RC_LOWER_EN_SET				(1<<8)
#define		GLPCI_RC_LOWER_PF_SET				(1<<5)
#define		GLPCI_RC_LOWER_WC_SET				(1<<4)
#define		GLPCI_RC_LOWER_WP_SET				(1<<2)
#define		GLPCI_RC_LOWER_CD_SET				(1<<0)
#define	GLPCI_ExtMSR			(MSR_PCI + 0x201E)
#define	GLPCI_SPARE			(MSR_PCI + 0x201F)
#define		GLPCI_SPARE_LOWER_AILTO_SET					(1<<6)
#define		GLPCI_SPARE_LOWER_PPD_SET					(1<<5)
#define		GLPCI_SPARE_LOWER_PPC_SET					(1<<4)
#define		GLPCI_SPARE_LOWER_MPC_SET					(1<<3)
#define		GLPCI_SPARE_LOWER_MME_SET					(1<<2)
#define		GLPCI_SPARE_LOWER_NSE_SET					(1<<1)
#define		GLPCI_SPARE_LOWER_SUPO_SET					(1<<0)


/**/
/* FooGlue GLIU1 port 5*/
/**/
#define	FG_GLD_MSR_CAP			(MSR_FG + 0x2000)
#define	FG_GLD_MSR_PM			(MSR_FG + 0x2004)

/*  VIP GLIU1 port 5*/
/* */
#define VIP_GLD_MSR_CAP		(MSR_VIP + 0x2000)
#define VIP_GLD_MSR_CONFIG	(MSR_VIP + 0x2001)
#define VIP_GLD_MSR_PM		(MSR_VIP + 0x2004)
#define VIP_BIST			(MSR_VIP + 0x2005)
/* */
/*  AES GLIU1 port 6*/
/* */
#define AES_GLD_MSR_CAP		(MSR_AES + 0x2000)
#define AES_GLD_MSR_CONFIG	(MSR_AES + 0x2001)
#define AES_GLD_MSR_PM		(MSR_AES + 0x2004)
#define AES_CONTROL			(MSR_AES + 0x2006)
/* more fun stuff */
#define BM			1	/*  Base Mask - map power of 2 size aligned region*/
#define BMO			2	/*  BM with an offset*/
#define R				3	/*  Range - 4k range minimum*/
#define RO			4	/*  R with offset*/
#define SC			5	/*  Swiss 0xCeese - maps a 256K region in to 16K 0xcunks. Set W/R*/
#define BMIO			6	/*  Base Mask IO*/
#define SCIO			7	/*  Swiss 0xCeese IO*/
#define SC_SHADOW	8	/*  Special marker for Shadow SC descriptors so setShadow proc is independant of CPU*/
#define R_SYSMEM		9	/*  Special marker for SYSMEM R descriptors so GLIUInit proc is independant of CPU*/
#define BMO_SMM		10	/*  Specail marker for SMM*/
#define BM_SMM		11	/*  Specail marker for SMM*/
#define BMO_DMM		12	/*  Specail marker for DMM*/
#define BM_DMM		13	/*  Specail marker for DMM*/
#define RO_FB			14	/*  special for Frame buffer.*/
#define R_FB			15	/*  special for FB.*/
#define OTHER			0x0FE /*  Special marker for other*/
#define GL_END		 0x0FF	/*  end*/

#define MSR_GL0	(GL1_GLIU0 << 29)

/*  Set up desc addresses from 20 - 3f*/
/*  This is chip specific!*/
#define MSR_GLIU0_BASE1			(MSR_GLIU0 + 0x20)		/*  BM*/
#define MSR_GLIU0_BASE2			(MSR_GLIU0 + 0x21)		/*  BM*/
#define MSR_GLIU0_SHADOW			(MSR_GLIU0 + 0x2C)		/*  SCO should only be SC*/
#define MSR_GLIU0_SYSMEM			(MSR_GLIU0 + 0x28)		/*  RO should only be R*/
#define MSR_GLIU0_SMM			(MSR_GLIU0 + 0x26)		/*  BMO*/
#define MSR_GLIU0_DMM			(MSR_GLIU0 + 0x27)		/*  BMO*/

#define MSR_GLIU1_BASE1			(MSR_GLIU1 + 0x20)		/*  BM*/
#define MSR_GLIU1_BASE2			(MSR_GLIU1 + 0x21)		/*  BM*/
#define MSR_GLIU1_SHADOW			(MSR_GLIU1 + 0x2D)		/*  SCO should only be SC*/
#define MSR_GLIU1_SYSMEM			(MSR_GLIU1 + 0x29)		/*  RO should only be R*/
#define MSR_GLIU1_SMM			(MSR_GLIU1 + 0x23)		/*  BM*/
#define MSR_GLIU1_DMM			(MSR_GLIU1 + 0x24)		/*  BM*/
#define MSR_GLIU1_FPU_TRAP		(MSR_GLIU1 + 0x0E3)	/*  FooGlue F0 for FPU*/

/* definitions that are "once you are mostly up, start VSA" type things */
#define SMM_OFFSET	0x40400000
#define SMM_SIZE		256
#define DMM_OFFSET	0x0C0000000
#define DMM_SIZE		128
#define FB_OFFSET		0x41000000
#define PCI_MEM_TOP	0x0EFFFFFFF	 // Top of PCI mem allocation region
#define PCI_IO_TOP		0x0EFFF		 // Top of PCI I/O allocation region
#define END_OPTIONROM_SPACE	0x0DFFF		 // E0000 is reserved for SystemROMs.

#define MDD_SMBUS	0x06000		 // SMBUS IO location
#define MDD_GPIO		0x06100		 // GPIO & ICF IO location
#define MDD_MFGPT	0x06200		 // General Purpose Timers IO location
#define MDD_IRQ_MAPPER	0x06300		 // IRQ Mapper
#define ACPI_BASE		0x09C00		 // ACPI Base
#define MDD_PM		0x09D00		 // Power Management Logic - placed at the end of ACPI

#define CS5535_IDSEL	0x02000000	 // IDSEL = AD25, device #15
#define CHIPSET_DEV_NUM	15
#define IDSEL_BASE	11			 // bit 11 = device 1


/* standard AMD post definitions -- might as well use them. */
#define POST_Output_Port				(0x080)	/*  port to write post codes to*/

#define POST_preSioInit					(0x000)	/* geode.asm*/
#define POST_clockInit					(0x001)	/* geode.asm*/
#define POST_CPURegInit					(0x002)	/* geode.asm*/
#define POST_UNREAL					(0x003)	/* geode.asm*/
#define POST_CPUMemRegInit				(0x004)	/* geode.asm*/
#define POST_CPUTest					(0x005)	/* geode.asm*/
#define POST_memSetup					(0x006)	/* geode.asm*/
#define POST_memSetUpStack				(0x007)	/* geode.asm*/
#define POST_memTest					(0x008)	/* geode.asm*/
#define POST_shadowRom				(0x009)	/* geode.asm*/
#define POST_memRAMoptimize			(0x00A)	/* geode.asm*/
#define POST_cacheInit					(0x00B)	/* geode.asm*/
#define POST_northBridgeInit				(0x00C)	/* geode.asm*/
#define POST_chipsetInit					(0x00D)	/* geode.asm*/
#define POST_sioTest					(0x00E)	/* geode.asm*/
#define POST_pcATjunk					(0x00F)	/* geode.asm*/


#define POST_intTable					(0x010)	/* geode.asm*/
#define POST_memInfo					(0x011)	/* geode.asm*/
#define POST_romCopy					(0x012)	/* geode.asm*/
#define POST_PLLCheck					(0x013)	/* geode.asm*/
#define POST_keyboardInit				(0x014)	/* geode.asm*/
#define POST_cpuCacheOff				(0x015)	/* geode.asm*/
#define POST_BDAInit					(0x016)	/* geode.asm*/
#define POST_pciScan					(0x017)	/* geode.asm*/
#define POST_optionRomInit				(0x018)	/* geode.asm*/
#define POST_ResetLimits				(0x019)	/* geode.asm*/
#define POST_summary_screen			(0x01A)	/* geode.asm*/
#define POST_Boot						(0x01B)	/* geode.asm*/
#define POST_SystemPreInit				(0x01C)	/* geode.asm*/
#define POST_ClearRebootFlag				(0x01D)	/* geode.asm*/
#define POST_GLIUInit					(0x01E)	/* geode.asm*/
#define POST_BootFailed					(0x01F)	/* geode.asm*/


#define POST_CPU_ID					(0x020)	/* cpucpuid.asm*/
#define POST_COUNTERBROKEN			(0x021)	/* pllinit.asm*/
#define POST_DIFF_DIMMS				(0x022)	/* pllinit.asm*/
#define POST_WIGGLE_MEM_LINES			(0x023)	/* pllinit.asm*/
#define POST_NO_GLIU_DESC				(0x024)	/* pllinit.asm*/
#define POST_CPU_LCD_CHECK			(0x025)	/* pllinit.asm*/
#define POST_CPU_LCD_PASS				(0x026)	/* pllinit.asm*/
#define POST_CPU_LCD_FAIL				(0x027)	/* pllinit.asm*/
#define POST_CPU_STEPPING				(0x028)	/* cpucpuid.asm*/
#define POST_CPU_DM_BIST_FAILURE		(0x029)	/* gx2reg.asm*/
#define POST_CPU_FLAGS				(0x02A)	/* cpucpuid.asm*/
#define POST_CHIPSET_ID				(0x02b)	/* chipset.asm*/
#define POST_CHIPSET_ID_PASS			(0x02c)	/* chipset.asm*/
#define POST_CHIPSET_ID_FAIL			(0x02d)	/* chipset.asm*/
#define POST_CPU_ID_GOOD				(0x02E)	/* cpucpuid.asm*/
#define POST_CPU_ID_FAIL				(0x02F)	/* cpucpuid.asm*/



/*  PCI config*/
#define P80_PCICFG						(0x030)	/*  pcispace.asm*/


/*  PCI io*/
#define P80_PCIIO						(0x040)	/*  pcispace.asm*/


/*  PCI memory*/
#define P80_PCIMEM					(0x050)	/*  pcispace.asm*/


/*  SIO*/
#define P80_SIO						(0x060)		/*  *sio.asm*/

/*  Memory Setp*/
#define P80_MEM_SETUP					(0x070)	/* docboot meminit*/
#define POST_MEM_SETUP				(0x070)	/* memsize.asm*/
#define ERROR_32BIT_DIMMS				(0x071)	/* memsize.asm*/
#define POST_MEM_SETUP2				(0x072)	/* memsize.asm*/
#define POST_MEM_SETUP3				(0x073)	/* memsize.asm*/
#define POST_MEM_SETUP4				(0x074)	/* memsize.asm*/
#define POST_MEM_SETUP5				(0x075)	/* memsize.asm*/
#define POST_MEM_ENABLE				(0x076)	/* memsize.asm*/
#define ERROR_NO_DIMMS				(0x077)	/* memsize.asm*/
#define ERROR_DIFF_DIMMS				(0x078)	/* memsize.asm*/
#define ERROR_BAD_LATENCY				(0x079)	/* memsize.asm*/
#define ERROR_SET_PAGE				(0x07a)	/* memsize.asm*/
#define ERROR_DENSITY_DIMM			(0x07b)	/* memsize.asm*/
#define ERROR_UNSUPPORTED_DIMM		(0x07c)	/* memsize.asm*/
#define ERROR_BANK_SET				(0x07d)	/* memsize.asm*/
#define POST_MEM_SETUP_GOOD			(0x07E)	/* memsize.asm*/
#define POST_MEM_SETUP_FAIL			(0x07F)	/* memsize.asm*/


#define POST_UserPreInit					(0x080)	/* geode.asm*/
#define POST_UserPostInit				(0x081)	/* geode.asm*/
#define POST_Equipment_check			(0x082)	/* geode.asm*/
#define POST_InitNVRAMBX				(0x083)	/* geode.asm*/
#define POST_NoPIRTable				(0x084)	/* pci.asm*/
#define POST_ChipsetFingerPrintPass		(0x085)	/*  prechipsetinit*/
#define POST_ChipsetFingerPrintFail		(0x086)	/*  prechipsetinit*/
#define POST_CPU_IM_TAG_BIST_FAILURE	(0x087)	/*  gx2reg.asm*/
#define POST_CPU_IM_DATA_BIST_FAILURE	(0x088)	/*  gx2reg.asm*/
#define POST_CPU_FPU_BIST_FAILURE		(0x089)	/*  gx2reg.asm*/
#define POST_CPU_BTB_BIST_FAILURE		(0x08a)	/*  gx2reg.asm*/
#define POST_CPU_EX_BIST_FAILURE		(0x08b)	/*  gx2reg.asm*/
#define POST_Chipset_PI_Test_Fail			(0x08c)	/*  prechipsetinit*/
#define POST_Chipset_SMBus_SDA_Test_Fail	(0x08d)	/*  prechipsetinit*/
#define POST_BIT_CLK_Fail				(0x08e)	/*  Hawk geode.asm override*/


#define POST_STACK_SETUP				(0x090)	/* memstack.asm*/
#define POST_CPU_PF_BIST_FAILURE		(0x091)	/*  gx2reg.asm*/
#define POST_CPU_L2_BIST_FAILURE		(0x092)	/*  gx2reg.asm*/
#define POST_CPU_GLCP_BIST_FAILURE		(0x093)	/*  gx2reg.asm*/
#define POST_CPU_DF_BIST_FAILURE		(0x094)	/*  gx2reg.asm*/
#define POST_CPU_VG_BIST_FAILURE		(0x095)	/*  gx2reg.asm*/
#define POST_CPU_VIP_BIST_FAILURE		(0x096)	/*  gx2reg.asm*/
#define POST_STACK_SETUP_PASS			(0x09E)	/* memstack.asm*/
#define POST_STACK_SETUP_FAIL			(0x09F)	/* memstack.asm*/


#define POST_PLL_INIT					(0x0A0)	/* pllinit.asm*/
#define POST_PLL_MANUAL				(0x0A1)	/* pllinit.asm*/
#define POST_PLL_STRAP					(0x0A2)	/* pllinit.asm*/
#define POST_PLL_RESET_FAIL			(0x0A3)	/* pllinit.asm*/
#define POST_PLL_PCI_FAIL				(0x0A4)	/* pllinit.asm*/
#define POST_PLL_MEM_FAIL				(0x0A5)	/* pllinit.asm*/
#define POST_PLL_CPU_VER_FAIL			(0x0A6)	/* pllinit.asm*/


#define POST_MEM_TESTMEM				(0x0B0)	/* memtest.asm*/
#define POST_MEM_TESTMEM1			(0x0B1)	/* memtest.asm*/
#define POST_MEM_TESTMEM2			(0x0B2)	/* memtest.asm*/
#define POST_MEM_TESTMEM3			(0x0B3)	/* memtest.asm*/
#define POST_MEM_TESTMEM4			(0x0B4)	/* memtest.asm*/
#define POST_MEM_TESTMEM_PASS		(0x0BE)	/* memtest.asm*/
#define POST_MEM_TESTMEM_FAIL		(0x0BF)	/* memtest.asm*/


#define POST_SECUROM_SECBOOT_START        (0x0C0)	/* secstart.asm*/
#define POST_SECUROM_BOOTSRCSETUP         (0x0C1)	/* secstart.asm*/
#define POST_SECUROM_REMAP_FAIL          	 (0x0C2)	/* secstart.asm*/
#define POST_SECUROM_BOOTSRCSETUP_FAIL    (0x0C3)	/* secstart.asm*/
#define POST_SECUROM_DCACHESETUP          (0x0C4)	/* secstart.asm*/
#define POST_SECUROM_DCACHESETUP_FAIL     (0x0C5)	/* secstart.asm*/
#define POST_SECUROM_ICACHESETUP          (0x0C6)	/* secstart.asm*/
#define POST_SECUROM_DESCRIPTORSETUP      (0x0C7)	/* secstart.asm*/
#define POST_SECUROM_DCACHESETUPBIOS      (0x0C8)	/* secstart.asm*/
#define POST_SECUROM_PLATFORMSETUP        (0x0C9)	/* secstart.asm*/
#define POST_SECUROM_SIGCHECKBIOS         (0x0CA)	/* secstart.asm*/
#define POST_SECUROM_ICACHESETUPBIOS      (0x0CB)	/* secstart.asm*/
#define POST_SECUROM_PASS				  (0x0CC)	/* secstart.asm*/
#define POST_SECUROM_FAIL				  (0x0CD)	/* secstart.asm*/

#define POST_RCONFInitError				(0x0CE)	/* cache.asm*/
#define POST_CacheInitError				(0x0CF)	/* cache.asm*/


#define POST_ROM_PREUNCOMPRESS			(0x0D0)	/* rominit.asm*/
#define POST_ROM_UNCOMPRESS				(0x0D1)	/* rominit.asm*/
#define POST_ROM_SMM_INIT				(0x0D2)	/* rominit.asm*/
#define POST_ROM_VID_BIOS				(0x0D3)	/* rominit.asm*/
#define POST_ROM_LCDINIT				(0x0D4)	/* rominit.asm*/
#define POST_ROM_SPLASH					(0x0D5)	/* rominit.asm*/
#define POST_ROM_HDDINIT				(0x0D6)	/* rominit.asm*/
#define POST_ROM_SYS_INIT				(0x0D7)	/* rominit.asm*/
#define POST_ROM_DMM_INIT				(0x0D8)	/* rominit.asm*/
#define POST_ROM_TVINIT					(0x0D9)	/* rominit.asm*/
#define POST_ROM_POSTUNCOMPRESS			(0x0DE)


#define P80_CHIPSET_INIT				(0x0E0)	/* chipset.asm*/
#define POST_PreChipsetInit				(0x0E1)	/* geode.asm*/
#define POST_LateChipsetInit			(0x0E2)	/* geode.asm*/
#define POST_NORTHB_INIT				(0x0E8)	/* northb.asm*/


#define POST_INTR_SEG_JUMP				(0x0F0)	/* vector.asm*/


/* I don't mind if somebody decides this needs to be in a seperate file. I don't see much point
  * in it, either. 
  * RGM 
  */
#define Cx5535_ID	 (	0x002A100B)
#define Cx5536_ID	 (	0x208F1022)

/* Cs5535 as follows. */
/*  SB_GLIU*/
/* 	port0 - GLIU*/
/* 	port1 - GLPCI*/
/* 	port2 - USB Controller #2*/
/* 	port3 - ATA-5 Controller*/
/* 	port4 - MDD*/
/* 	port5 - AC97*/
/* 	port6 - USB Controller #1*/
/* 	port7 - GLCP*/


/*  SouthBridge Equates*/
/*  MSR_SB and SB_SHIFT are located in CPU.inc*/
#define MSR_SB_GLIU		 (		(9 << 14) + MSR_SB			/*  fake out just like GL0 on CPU.*/)
#define MSR_SB_GLPCI	 (		MSR_SB							/*  don't go to the GLIU*/)
#define MSR_SB_USB2		 (		(2 << SB_SHIFT) + MSR_SB)
#define MSR_SB_ATA		 (		(3 << SB_SHIFT) + MSR_SB)
#define MSR_SB_MDD		 (		(4 << SB_SHIFT) + MSR_SB)
#define MSR_SB_AC97		 (		(5 << SB_SHIFT) + MSR_SB)
#define MSR_SB_USB1		 (		(6 << SB_SHIFT) + MSR_SB)
#define MSR_SB_GLCP		 (		(7 << SB_SHIFT) + MSR_SB)

/* */
/* GLIU*/
/* */
#define GLIU_SB_GLD_MSR_CAP		 (	MSR_SB_GLIU + 0x00)
#define GLIU_SB_GLD_MSR_CONF	 (	MSR_SB_GLIU + 0x01)
#define GLIU_SB_GLD_MSR_PM		 (	MSR_SB_GLIU + 0x04)

/* */
/*  USB1*/
/* */
#define USB1_SB_GLD_MSR_CAP		 (	MSR_SB_USB1 + 0x00)
#define USB1_SB_GLD_MSR_CONF	 (	MSR_SB_USB1 + 0x01)
#define USB1_SB_GLD_MSR_PM		 (	MSR_SB_USB1 + 0x04)
/* */
/*  USB2*/
/* */
#define USB2_SB_GLD_MSR_CAP		 (	MSR_SB_USB2 + 0x00)
#define USB2_SB_GLD_MSR_CONF	 (	MSR_SB_USB2 + 0x01)
#define USB2_SB_GLD_MSR_PM		 (	MSR_SB_USB2 + 0x04)


/* */
/*  ATA*/
/* */
#define ATA_SB_GLD_MSR_CAP		 (	MSR_SB_ATA + 0x00)
#define ATA_SB_GLD_MSR_CONF		 (	MSR_SB_ATA + 0x01)
#define ATA_SB_GLD_MSR_ERR		 (	MSR_SB_ATA + 0x03)
#define ATA_SB_GLD_MSR_PM		 (	MSR_SB_ATA + 0x04)

/* */
/*  AC97*/
/* */
#define AC97_SB_GLD_MSR_CAP		 (	MSR_SB_AC97 + 0x00)
#define AC97_SB_GLD_MSR_CONF	 (	MSR_SB_AC97 + 0x01)
#define AC97_SB_GLD_MSR_PM		 (	MSR_SB_AC97 + 0x04)

/* */
/*  GLPCI*/
/* */
#define GLPCI_SB_GLD_MSR_CAP	 (	MSR_SB_GLPCI + 0x00)
#define GLPCI_SB_GLD_MSR_CONF	 (	MSR_SB_GLPCI + 0x01)
#define GLPCI_SB_GLD_MSR_PM		 (	MSR_SB_GLPCI + 0x04)
#define GLPCI_SB_CTRL			 (	MSR_SB_GLPCI + 0x10)
#define 	GLPCI_CRTL_PPIDE_SET	 (	1 << 17)
/* */
/*  GLCP*/
/* */
#define GLCP_SB_GLD_MSR_CAP		 (	MSR_SB_GLCP + 0x00)
#define GLCP_SB_GLD_MSR_CONF	 (	MSR_SB_GLCP + 0x01)
#define GLCP_SB_GLD_MSR_PM		 (	MSR_SB_GLCP + 0x04)

/* 	*/
/*  MDD*/
/* */
#define MDD_SB_GLD_MSR_CAP		 (	MSR_SB_MDD + 0x00)
#define MDD_SB_GLD_MSR_CONF		 (	MSR_SB_MDD + 0x01)
#define MDD_SB_GLD_MSR_PM		 (	MSR_SB_MDD + 0x04)
#define LBAR_EN			 (		0x01)
#define IO_MASK			 (		0x1f)
#define MEM_MASK		 (		0x0FFFFF)
#define MDD_LBAR_IRQ	 (		MSR_SB_MDD + 0x08)
#define MDD_LBAR_KEL1	 (		MSR_SB_MDD + 0x09)
#define MDD_LBAR_KEL2	 (		MSR_SB_MDD + 0x0A)
#define MDD_LBAR_SMB	 (		MSR_SB_MDD + 0x0B)
#define MDD_LBAR_GPIO	 (		MSR_SB_MDD + 0x0C)
#define MDD_LBAR_MFGPT	 (		MSR_SB_MDD + 0x0D)
#define MDD_LBAR_ACPI	 (		MSR_SB_MDD + 0x0E)
#define MDD_LBAR_PMS	 (		MSR_SB_MDD + 0x0F)

#define MDD_LBAR_FLSH0	 (		MSR_SB_MDD + 0x010)
#define MDD_LBAR_FLSH1	 (		MSR_SB_MDD + 0x011)
#define MDD_LBAR_FLSH2	 (		MSR_SB_MDD + 0x012)
#define MDD_LBAR_FLSH3	 (		MSR_SB_MDD + 0x013)
#define MDD_LEG_IO		 (		MSR_SB_MDD + 0x014)
#define MDD_PIN_OPT		 (		MSR_SB_MDD + 0x015)
#define MDD_SOFT_IRQ	 (		MSR_SB_MDD + 0x016)
#define MDD_SOFT_RESET	 (		MSR_SB_MDD + 0x017)
#define MDD_NORF_CNTRL	 (		MSR_SB_MDD + 0x018)
#define MDD_NORF_T01	 (		MSR_SB_MDD + 0x019)
#define MDD_NORF_T23	 (		MSR_SB_MDD + 0x01A)
#define MDD_NANDF_DATA	 (		MSR_SB_MDD + 0x01B)
#define MDD_NADF_CNTL	 (		MSR_SB_MDD + 0x01C)
#define MDD_AC_DMA		 (		MSR_SB_MDD + 0x01E)
#define MDD_KEL_CNTRL	 (		MSR_SB_MDD + 0x01F)

#define MDD_IRQM_YLOW	 (		MSR_SB_MDD + 0x020)
#define MDD_IRQM_YHIGH	 (		MSR_SB_MDD + 0x021)
#define MDD_IRQM_ZLOW	 (		MSR_SB_MDD + 0x022)
#define MDD_IRQM_ZHIGH	 (		MSR_SB_MDD + 0x023)
#define MDD_IRQM_PRIM	 (		MSR_SB_MDD + 0x024)
#define MDD_IRQM_LPC	 (		MSR_SB_MDD + 0x025)
#define MDD_IRQM_LXIRR	 (		MSR_SB_MDD + 0x026)
#define MDD_IRQM_HXIRR	 (		MSR_SB_MDD + 0x027)

#define MDD_MFGPT_IRQ	 (		MSR_SB_MDD + 0x028)
#define MDD_MFGPT_NR	 (		MSR_SB_MDD + 0x029)
#define MDD_MFGPT_RES0	 (		MSR_SB_MDD + 0x02A)
#define MDD_MFGPT_RES1	 (		MSR_SB_MDD + 0x02B)

#define MDD_FLOP_S3F2	 (		MSR_SB_MDD + 0x030)
#define MDD_FLOP_S3F7	 (		MSR_SB_MDD + 0x031)
#define MDD_FLOP_S372	 (		MSR_SB_MDD + 0x032)
#define MDD_FLOP_S377	 (		MSR_SB_MDD + 0x033)

#define MDD_PIC_S		 (		MSR_SB_MDD + 0x034)
#define MDD_PIT_S		 (		MSR_SB_MDD + 0x036)
#define MDD_PIT_CNTRL	 (		MSR_SB_MDD + 0x037)

#define MDD_UART1_MOD	 (		MSR_SB_MDD + 0x038)
#define MDD_UART1_DON	 (		MSR_SB_MDD + 0x039)
#define MDD_UART1_CONF	 (		MSR_SB_MDD + 0x03A)
#define MDD_UART2_MOD	 (		MSR_SB_MDD + 0x03C)
#define MDD_UART2_DON	 (		MSR_SB_MDD + 0x03D)
#define MDD_UART2_CONF	 (		MSR_SB_MDD + 0x03E)

#define MDD_DMA_MAP		 (		MSR_SB_MDD + 0x040)
#define MDD_DMA_SHAD1		 (		MSR_SB_MDD + 0x041)
#define MDD_DMA_SHAD2		 (		MSR_SB_MDD + 0x042)
#define MDD_DMA_SHAD3		 (		MSR_SB_MDD + 0x043)
#define MDD_DMA_SHAD4		 (		MSR_SB_MDD + 0x044)
#define MDD_DMA_SHAD5		 (		MSR_SB_MDD + 0x045)
#define MDD_DMA_SHAD6		 (		MSR_SB_MDD + 0x046)
#define MDD_DMA_SHAD7		 (		MSR_SB_MDD + 0x047)
#define MDD_DMA_SHAD8		 (		MSR_SB_MDD + 0x048)
#define MDD_DMA_SHAD9		 (		MSR_SB_MDD + 0x049)

#define MDD_LPC_EADDR	 (		MSR_SB_MDD + 0x04C)
#define MDD_LPC_ESTAT	 (		MSR_SB_MDD + 0x04D)
#define MDD_LPC_SIRQ	 (		MSR_SB_MDD + 0x04E)
#define MDD_LPC_RES		 (		MSR_SB_MDD + 0x04F)

#define MDD_PML_TMR		 (		MSR_SB_MDD + 0x050)
#define MDD_RTC_RAM_LO_CK	 (		MSR_SB_MDD + 0x054)
#define MDD_RTC_DOMA_IND	 (		MSR_SB_MDD + 0x055)
#define MDD_RTC_MONA_IND	 (		MSR_SB_MDD + 0x056)
#define MDD_RTC_CENTURY_OFFSET	 (		MSR_SB_MDD + 0x057)

/* */
/* LBAR IO + MEMORY MAP*/
/* */
#define SMBUS_BASE		 (		0x6000)
#define GPIO_BASE		 (		0x6100)
#define MFGPT_BASE		 (		0x6200)
#define IRQMAP_BASE		 (		0x6300)
#define PMLogic_BASE	 (		0x9D00)



/* ***********************************************************/
/*  LBUS Device Equates - */
/* ***********************************************************/

/* */
/*  SMBus*/
/* */

#define SMBUS_SMBSDA		 (		SMBUS_BASE + 0x00)
#define SMBUS_SMBST			 (		SMBUS_BASE + 0x01)
#define 	SMBST_SLVSTP_SET	 (		1 << 7)
#define 	SMBST_SDAST_SET		 (		1 << 6)
#define 	SMBST_BER_SET		 (		1 << 5)
#define 	SMBST_NEGACK_SET	 (		1 << 4)
#define 	SMBST_STASTR_SET	 (		1 << 3)
#define 	SMBST_NMATCH_SET	 (		1 << 2)
#define 	SMBST_MASTER_SET	 (		1 << 1)
#define 	SMBST_XMIT_SET		 (		1 << 0)
#define SMBUS_SMBCST		 (		SMBUS_BASE + 0x02)
#define 	SMBCST_TGSCL_SET	 (		1 << 5)
#define 	SMBCST_TSDA_SET		 (		1 << 4)
#define 	SMBCST_GCMTCH_SET	 (		1 << 3)
#define 	SMBCST_MATCH_SET	 (		1 << 2)
#define 	SMBCST_BB_SET		 (		1 << 1)
#define 	SMBCST_BUSY_SET		 (		1 << 0)
#define SMBUS_SMBCTL1		 (		SMBUS_BASE + 0x03)
#define 	SMBCTL1_STASTRE_SET	 (		1 << 7)
#define 	SMBCTL1_NMINTE_SET	 (		1 << 6)
#define 	SMBCTL1_GCMEN_SET	 (		1 << 5)
#define 	SMBCTL1_RECACK_SET	 (		1 << 4)
#define 	SMBCTL1_DMAEN_SET	 (		1 << 3)
#define 	SMBCTL1_INTEN_SET	 (		1 << 2)
#define 	SMBCTL1_STOP_SET	 (		1 << 1)
#define 	SMBCTL1_START_SET	 (		1 << 0)
#define SMBUS_SMBADDR		 (		SMBUS_BASE + 0x04)
#define 	SMBADDR_SAEN_SET	 (		1 << 7)
#define SMBUS_SMBCTL2		 (		SMBUS_BASE + 0x05)
#define 	SMBCTL2_SCLFRQ_SHIFT	 (		1 << 1)
#define 	SMBCTL2_ENABLE_SET		 (		1 << 0)

/* */
/*  GPIO*/
/* */

#define GPIOL_0_SET		 (		1 << 0)
#define GPIOL_1_SET		 (		1 << 1)
#define GPIOL_2_SET		 (		1 << 2)
#define GPIOL_3_SET		 (		1 << 3)
#define GPIOL_4_SET		 (		1 << 4)
#define GPIOL_5_SET		 (		1 << 5)
#define GPIOL_6_SET		 (		1 << 6)
#define GPIOL_7_SET		 (		1 << 7)
#define GPIOL_8_SET		 (		1 << 8)
#define GPIOL_9_SET		 (		1 << 9)
#define GPIOL_10_SET	 (		1 << 10)
#define GPIOL_11_SET	 (		1 << 11)
#define GPIOL_12_SET	 (		1 << 12)
#define GPIOL_13_SET	 (		1 << 13)
#define GPIOL_14_SET	 (		1 << 14)
#define GPIOL_15_SET	 (		1 << 15)

#define GPIOL_0_CLEAR	 (		1 << 16)
#define GPIOL_1_CLEAR	 (		1 << 17)
#define GPIOL_2_CLEAR	 (		1 << 18)
#define GPIOL_3_CLEAR	 (		1 << 19)
#define GPIOL_4_CLEAR	 (		1 << 20)
#define GPIOL_5_CLEAR	 (		1 << 21)
#define GPIOL_6_CLEAR	 (		1 << 22)
#define GPIOL_7_CLEAR	 (		1 << 23)
#define GPIOL_8_CLEAR	 (		1 << 24)
#define GPIOL_9_CLEAR	 (		1 << 25)
#define GPIOL_10_CLEAR	 (		1 << 26)
#define GPIOL_11_CLEAR	 (		1 << 27)
#define GPIOL_12_CLEAR	 (		1 << 28)
#define GPIOL_13_CLEAR	 (		1 << 29)
#define GPIOL_14_CLEAR	 (		1 << 30)
#define GPIOL_15_CLEAR	 (		1 << 31)

#define GPIOH_16_SET	 (		1 << 0)
#define GPIOH_17_SET	 (		1 << 1)
#define GPIOH_18_SET	 (		1 << 2)
#define GPIOH_19_SET	 (		1 << 3)
#define GPIOH_20_SET	 (		1 << 4)
#define GPIOH_21_SET	 (		1 << 5)
#define GPIOH_22_SET	 (		1 << 6)
#define GPIOH_23_SET	 (		1 << 7)
#define GPIOH_24_SET	 (		1 << 8)
#define GPIOH_25_SET	 (		1 << 9)
#define GPIOH_26_SET	 (		1 << 10)
#define GPIOH_27_SET	 (		1 << 11)
#define GPIOH_28_SET	 (		1 << 12)
#define GPIOH_29_SET	 (		1 << 13)
#define GPIOH_30_SET	 (		1 << 14)
#define GPIOH_31_SET	 (		1 << 15)

#define GPIOH_16_CLEAR	 (		1 << 16)
#define GPIOH_17_CLEAR	 (		1 << 17)
#define GPIOH_18_CLEAR	 (		1 << 18)
#define GPIOH_19_CLEAR	 (		1 << 19)
#define GPIOH_20_CLEAR	 (		1 << 20)
#define GPIOH_21_CLEAR	 (		1 << 21)
#define GPIOH_22_CLEAR	 (		1 << 22)
#define GPIOH_23_CLEAR	 (		1 << 23)
#define GPIOH_24_CLEAR	 (		1 << 24)
#define GPIOH_25_CLEAR	 (		1 << 25)
#define GPIOH_26_CLEAR	 (		1 << 26)
#define GPIOH_27_CLEAR	 (		1 << 27)
#define GPIOH_28_CLEAR	 (		1 << 28)
#define GPIOH_29_CLEAR	 (		1 << 29)
#define GPIOH_30_CLEAR	 (		1 << 30)
#define GPIOH_31_CLEAR	 (		1 << 31)


/*  GPIO LOW Bank Bit Registers*/
#define GPIOL_OUTPUT_VALUE		 (		GPIO_BASE + 0x00)
#define GPIOL_OUTPUT_ENABLE		 (		GPIO_BASE + 0x04)
#define GPIOL_OUT_OPENDRAIN		 (		GPIO_BASE + 0x08)
#define GPIOL_OUTPUT_INVERT_ENABLE	 (	GPIO_BASE + 0x0C)
#define GPIOL_OUT_AUX1_SELECT	 (		GPIO_BASE + 0x10)
#define GPIOL_OUT_AUX2_SELECT	 (		GPIO_BASE + 0x14)
#define GPIOL_PULLUP_ENABLE		 (		GPIO_BASE + 0x18)
#define GPIOL_PULLDOWN_ENABLE	 (		GPIO_BASE + 0x1C)
#define GPIOL_INPUT_ENABLE		 (		GPIO_BASE + 0x20)
#define GPIOL_INPUT_INVERT_ENABLE	 (	GPIO_BASE + 0x24)
#define GPIOL_IN_FILTER_ENABLE	 (		GPIO_BASE + 0x28)
#define GPIOL_IN_EVENTCOUNT_ENABLE	 (	GPIO_BASE + 0x2C)
#define GPIOL_READ_BACK			 (		GPIO_BASE + 0x30)
#define GPIOL_IN_AUX1_SELECT	 (		GPIO_BASE + 0x34)
#define GPIOL_EVENTS_ENABLE		 (		GPIO_BASE + 0x38)
#define GPIOL_LOCK_ENABLE		 (		GPIO_BASE + 0x3C)
#define GPIOL_IN_POSEDGE_ENABLE	 (		GPIO_BASE + 0x40)
#define GPIOL_IN_NEGEDGE_ENABLE	 (		GPIO_BASE + 0x44)
#define GPIOL_IN_POSEDGE_STATUS	 (		GPIO_BASE + 0x48)
#define GPIOL_IN_NEGEDGE_STATUS	 (		GPIO_BASE + 0x4C)

/*  GPIO  High Bank Bit Registers*/
#define GPIOH_OUTPUT_VALUE		 (		GPIO_BASE + 0x80)
#define GPIOH_OUTPUT_ENABLE		 (		GPIO_BASE + 0x84)
#define GPIOH_OUT_OPENDRAIN		 (		GPIO_BASE + 0x88)
#define GPIOH_OUTPUT_INVERT_ENABLE	 (	GPIO_BASE + 0x8C)
#define GPIOH_OUT_AUX1_SELECT	 (		GPIO_BASE + 0x90)
#define GPIOH_OUT_AUX2_SELECT	 (		GPIO_BASE + 0x94)
#define GPIOH_PULLUP_ENABLE		 (		GPIO_BASE + 0x98)
#define GPIOH_PULLDOWN_ENABLE	 (		GPIO_BASE + 0x9C)
#define GPIOH_INPUT_ENABLE		 (		GPIO_BASE + 0x0A0)
#define GPIOH_INPUT_INVERT_ENABLE	 (	GPIO_BASE + 0x0A4)
#define GPIOH_IN_FILTER_ENABLE	 (		GPIO_BASE + 0x0A8)
#define GPIOH_IN_EVENTCOUNT_ENABLE	 (	GPIO_BASE + 0x0AC)
#define GPIOH_READ_BACK			 (		GPIO_BASE + 0x0B0)
#define GPIOH_IN_AUX1_SELECT	 (		GPIO_BASE + 0x0B4)
#define GPIOH_EVENTS_ENABLE		 (		GPIO_BASE + 0x0B8)
#define GPIOH_LOCK_ENABLE		 (		GPIO_BASE + 0x0BC)
#define GPIOH_IN_POSEDGE_ENABLE	 (		GPIO_BASE + 0x0C0)
#define GPIOH_IN_NEGEDGE_ENABLE	 (		GPIO_BASE + 0x0C4)
#define GPIOH_IN_POSEDGE_STATUS	 (		GPIO_BASE + 0x0C8)
#define GPIOH_IN_NEGEDGE_STATUS	 (		GPIO_BASE + 0x0CC)

/*  Input Conditioning Function Registers*/
#define GPIO_00_FILTER_AMOUNT	 (		GPIO_BASE + 0x50)
#define GPIO_00_FILTER_COUNT	 (		GPIO_BASE + 0x52)
#define GPIO_00_EVENT_COUNT		 (		GPIO_BASE + 0x54)
#define GPIO_00_EVENTCOMPARE_VALUE	 (	GPIO_BASE + 0x56)
#define GPIO_01_FILTER_AMOUNT	 (		GPIO_BASE + 0x58)
#define GPIO_01_FILTER_COUNT	 (		GPIO_BASE + 0x5A)
#define GPIO_01_EVENT_COUNT		 (		GPIO_BASE + 0x5C)
#define GPIO_01_EVENTCOMPARE_VALUE	 (	GPIO_BASE + 0x5E)
#define GPIO_02_FILTER_AMOUNT	 (		GPIO_BASE + 0x60)
#define GPIO_02_FILTER_COUNT	 (		GPIO_BASE + 0x62)
#define GPIO_02_EVENT_COUNT		 (		GPIO_BASE + 0x64)
#define GPIO_02_EVENTCOMPARE_VALUE	 (	GPIO_BASE + 0x66)
#define GPIO_03_FILTER_AMOUNT	 (		GPIO_BASE + 0x68)
#define GPIO_03_FILTER_COUNT	 (		GPIO_BASE + 0x6A)
#define GPIO_03_EVENT_COUNT		 (		GPIO_BASE + 0x6C)
#define GPIO_03_EVENTCOMPARE_VALUE	 (	GPIO_BASE + 0x6E)
#define GPIO_04_FILTER_AMOUNT	 (		GPIO_BASE + 0x70)
#define GPIO_04_FILTER_COUNT	 (		GPIO_BASE + 0x72)
#define GPIO_04_EVENT_COUNT		 (		GPIO_BASE + 0x74)
#define GPIO_04_EVENTCOMPARE_VALUE	 (	GPIO_BASE + 0x76)
#define GPIO_05_FILTER_AMOUNT	 (		GPIO_BASE + 0x78)
#define GPIO_05_FILTER_COUNT	 (		GPIO_BASE + 0x7A)
#define GPIO_05_EVENT_COUNT		 (		GPIO_BASE + 0x7C)
#define GPIO_05_EVENTCOMPARE_VALUE	 (	GPIO_BASE + 0x7E)
#define GPIO_06_FILTER_AMOUNT	 (		GPIO_BASE + 0x0D0)
#define GPIO_06_FILTER_COUNT	 (		GPIO_BASE + 0x0D2)
#define GPIO_06_EVENT_COUNT		 (		GPIO_BASE + 0x0D4)
#define GPIO_06_EVENTCOMPARE_VALUE	 (	GPIO_BASE + 0x0D6)
#define GPIO_07_FILTER_AMOUNT	 (		GPIO_BASE + 0x0D8)
#define GPIO_07_FILTER_COUNT	 (		GPIO_BASE + 0x0DA)
#define GPIO_07_EVENT_COUNT		 (		GPIO_BASE + 0x0DC)
#define GPIO_07_EVENTCOMPARE_VALUE	 (	GPIO_BASE + 0x0DE)

/*  R/W GPIO Interrupt &PME Mapper Registers*/
#define GPIO_MAPPER_X		 (		GPIO_BASE + 0x0E0)
#define GPIO_MAPPER_Y		 (		GPIO_BASE + 0x0E4)
#define GPIO_MAPPER_Z		 (		GPIO_BASE + 0x0E8)
#define GPIO_MAPPER_W		 (		GPIO_BASE + 0x0EC)
#define GPIO_FE_SELECT_0	 (		GPIO_BASE + 0x0F0)
#define GPIO_FE_SELECT_1	 (		GPIO_BASE + 0x0F1)
#define GPIO_FE_SELECT_2	 (		GPIO_BASE + 0x0F2)
#define GPIO_FE_SELECT_3	 (		GPIO_BASE + 0x0F3)
#define GPIO_FE_SELECT_4	 (		GPIO_BASE + 0x0F4)
#define GPIO_FE_SELECT_5	 (		GPIO_BASE + 0x0F5)
#define GPIO_FE_SELECT_6	 (		GPIO_BASE + 0x0F6)
#define GPIO_FE_SELECT_7	 (		GPIO_BASE + 0x0F7)

/*  Event Counter Decrement Registers*/
#define GPIOL_IN_EVENT_DECREMENT	 (	GPIO_BASE + 0x0F8)
#define GPIOH_IN_EVENT_DECREMENT	 (	GPIO_BASE + 0x0FC)

/*  This is for 286reset compatibility. 0xCange to mat0xc 5535 virtualized stuff.*/
#define FUNC0		 (	0x90)


/*  sworley, PMC register*/
#define PM_SSD		 (	PMLogic_BASE + 0x00)
#define PM_SCXA		 (	PMLogic_BASE + 0x04)
#define PM_SCYA		 (	PMLogic_BASE + 0x08)
#define PM_SODA		 (	PMLogic_BASE + 0x0C)
#define PM_SCLK		 (	PMLogic_BASE + 0x10)
#define PM_SED		 (	PMLogic_BASE + 0x14)
#define PM_SCXD		 (	PMLogic_BASE + 0x18)
#define PM_SCYD		 (	PMLogic_BASE + 0x1C)
#define PM_SIDD		 (	PMLogic_BASE + 0x20)
#define PM_WKD		 (	PMLogic_BASE + 0x30)
#define PM_WKXD		 (	PMLogic_BASE + 0x34)
#define PM_RD		 (	PMLogic_BASE + 0x38)
#define PM_WKXA		 ( PMLogic_BASE + 0x3C)
#define PM_FSD		 (	PMLogic_BASE + 0x40)
#define PM_TSD		 (	PMLogic_BASE + 0x44)
#define PM_PSD		 ( PMLogic_BASE + 0x48)
#define PM_NWKD		 (	PMLogic_BASE + 0x4C)
#define PM_AWKD		 (	PMLogic_BASE + 0x50)
#define PM_SSC		 (	PMLogic_BASE + 0x54)

#endif /* CPU_AMD_GX2DEF_H */
