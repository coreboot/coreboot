#ifndef CPU_AMD_GX2DEF_H
#define CPU_AMD_GX2DEF_H

/* GeodeLink Control Processor Registers, GLIU1, Port 3 */
#define	GLCP_CLK_DIS_DELAY	0x4c000008
#define	GLCP_PMCLKDISABLE	0x4c000009
#define	GLCP_DELAY_CONTROLS	0x4c00000f
#define	GLCP_SYS_RSTPLL 	0x4c000014
#define	GLCP_DOTPLL		0x4c000015
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
#define	GL1_GLCP				3
#define	GL1_PCI				4
#define	GL1_FG				5


#define	MSR_GLIU0			(GL0_GLIU0 << 29) + 1 << 28 /* To get on GeodeLink one bit has to be set */
#define	MSR_MC				GL0_MC		<< 29
#define	MSR_GLIU1			GL0_GLIU1	<< 29
#define	MSR_CPU				GL0_CPU	<< 29 /* this is not used for BIOS since code executing on CPU doesn't need to be routed*/
#define	MSR_VG				GL0_VG		<< 29
#define	MSR_GP				GL0_GP		<< 29
#define	MSR_DF				GL0_DF		<< 29

#define	MSR_GLCP				(GL1_GLCP << 26) + MSR_GLIU1
#define	MSR_PCI				(GL1_PCI << 26) + MSR_GLIU1
#define	MSR_FG				(GL1_FG << 26) + MSR_GLIU1

/* South Bridge*/
#define	MSR_SB				(SB_PORT << 23) + MSR_PCI	/* address to the SouthBridge*/
#define	SB_SHIFT			20							/* 29 -> 26 -> 23 -> 20...... When making a SB address uses this shift.*/


/**/
/*GeodeLink Interface Unit 0 (GLIU0) port0*/
/**/

#define	GLIU0_GLD_MSR_CAP			MSR_GLIU0 + 2000h
#define	GLIU0_GLD_MSR_PM			MSR_GLIU0 + 2004h

#define	GLIU0_DESC_BASE				MSR_GLIU0 + 20h
#define	GLIU0_CAP					MSR_GLIU0 + 86h
#define	GLIU0_GLD_MSR_COH			MSR_GLIU0 + 80h


/**/
/* Memory Controller GLIU0 port 1*/
/**/
#define	MC_GLD_MSR_CAP			MSR_MC + 2000h
#define	MC_GLD_MSR_PM			MSR_MC + 2004h

#define	MC_CF07_DATA			MSR_MC + 18h

#define		CF07_UPPER_D1_SZ_SHIFT				28
#define		CF07_UPPER_D1_MB_SHIFT				24
#define		CF07_UPPER_D1_CB_SHIFT				20
#define		CF07_UPPER_D1_PSZ_SHIFT				16
#define		CF07_UPPER_D0_SZ_SHIFT				12
#define		CF07_UPPER_D0_MB_SHIFT				8
#define		CF07_UPPER_D0_CB_SHIFT				4
#define		CF07_UPPER_D0_PSZ_SHIFT				0

#define		CF07_LOWER_REF_INT_SHIFT			8
#define		CF07_LOWER_LOAD_MODE_DDR_SET			01 << 28
#define		CF07_LOWER_LOAD_MODE_DLL_RESET			01 << 27
#define		CF07_LOWER_EMR_QFC_SET				01 << 26
#define		CF07_LOWER_EMR_DRV_SET				01 << 25
#define		CF07_LOWER_REF_TEST_SET				1 << 3
#define		CF07_LOWER_PROG_DRAM_SET			1 << 0


#define	MC_CF8F_DATA			MSR_MC + 19h

#define		CF8F_UPPER_XOR_BS_SHIFT				19
#define		CF8F_UPPER_XOR_MB0_SHIFT			18
#define		CF8F_UPPER_XOR_BA1_SHIFT			17
#define		CF8F_UPPER_XOR_BA0_SHIFT			16
#define		CF8F_UPPER_REORDER_DIS_SET			1 << 8
#define		CF8F_UPPER_REG_DIMM_SHIFT			4
#define		CF8F_LOWER_CAS_LAT_SHIFT			28
#define		CF8F_LOWER_REF2ACT_SHIFT			24
#define		CF8F_LOWER_ACT2PRE_SHIFT			20
#define		CF8F_LOWER_PRE2ACT_SHIFT			16
#define		CF8F_LOWER_ACT2CMD_SHIFT			12
#define		CF8F_LOWER_ACT2ACT_SHIFT			8
#define		CF8F_UPPER_32BIT_SET				1 << 5
#define		CF8F_UPPER_HOI_LOI_SET				1 << 1

#define	MC_CF1017_DATA			MSR_MC + 1Ah

#define		CF1017_LOWER_PM1_UP_DLY_SET					1 << 8
#define		CF1017_LOWER_WR2DAT_SHIFT					0

#define	MC_CFCLK_DBUG			MSR_MC + 1Dh

#define		CFCLK_UPPER_MTST_B2B_DIS_SET			1 << 2
#define		CFCLK_UPPER_MTST_DQS_EN_SET			1 << 1
#define		CFCLK_UPPER_MTEST_EN_SET			1 << 0

#define		CFCLK_LOWER_MASK_CKE_SET1			1 << 9
#define		CFCLK_LOWER_MASK_CKE_SET0			1 << 8
#define		CFCLK_LOWER_SDCLK_SET				0Fh << 0

#define	MC_CF_RDSYNC		 MSR_MC	+ 1Fh


/**/
/* GLIU1 GLIU0 port2*/
/**/
#define	GLIU1_GLD_MSR_CAP			MSR_GLIU1 + 2000h
#define	GLIU1_GLD_MSR_PM			MSR_GLIU1 + 2004h

#define	GLIU1_GLD_MSR_COH			MSR_GLIU1 + 80h


/**/
/* CPU	; does not need routing instructions since we are executing there.*/
/**/
#define	CPU_GLD_MSR_CAP						2000h
#define	CPU_GLD_MSR_CONFIG					2001h
#define	CPU_GLD_MSR_PM						2004h

#define	CPU_GLD_MSR_DIAG					2005h
#define		DIAG_SEL1_MODE_SHIFT				16
#define		DIAG_SEL1_SET						1 << 31
#define		DIAG_SEL0__MODE_SHIFT				0
#define		DIAG_SET0_SET						1 << 15

#define	CPU_PF_BTB_CONF						1100h
#define		BTB_ENABLE_SET						1 << 0
#define		RETURN_STACK_ENABLE_SET				1 << 4
#define	CPU_PF_BTBRMA_BIST					110Ch

#define	CPU_XC_CONFIG						1210h
#define		XC_CONFIG_SUSP_ON_HLT				1 << 0
#define	CPU_ID_CONFIG						1250h
#define		ID_CONFIG_SERIAL_SET				1 << 0

#define	CPU_AC_MSR							1301h
#define	CPU_EX_BIST							1428h

/*IM*/
#define	CPU_IM_CONFIG							1700h
#define		IM_CONFIG_LOWER_ICD_SET					1 << 8
#define		IM_CONFIG_LOWER_QWT_SET					1 << 20
#define	CPU_IC_INDEX							1710h
#define	CPU_IC_DATA								1711h
#define	CPU_IC_TAG								1712h
#define	CPU_IC_TAG_I							1713h
#define	CPU_ITB_INDEX							1720h
#define	CPU_ITB_LRU								1721h
#define	CPU_ITB_ENTRY							1722h
#define	CPU_ITB_ENTRY_I							1723h
#define	CPU_IM_BIST_TAG							1730h
#define	CPU_IM_BIST_DATA						1731h


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
#define		SUSP_EN_SET				 (1<<1)2
	
	/**/
	/*	VG GLIU0 port4*/
	/**/
	
#define	VG_GLD_MSR_CAP			MSR_VG + 0x2000
#define	VG_GLD_MSR_CONFIG		MSR_VG + 0x2001
#define	VG_GLD_MSR_PM			MSR_VG + 0x2004

#endif /* CPU_AMD_GX2DEF_H */
