/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 Indrek Kruusa <indrek.kruusa@artecdesign.ee>
 * Copyright (C) 2006 Ronald G. Minnich <rminnich@gmail.com>
 * Copyright (C) 2006 Stefan Reinauer <stepan@coresystems.de>
 * Copyright (C) 2006 Andrei Birjukov <andrei.birjukov@artecdesign.ee>
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef CPU_AMD_LXDEF_H
#define CPU_AMD_LXDEF_H

/* Stepping ID 1.x CPUbug fix to change it to 5A0*/
#define CPU_ID_1_X	0x00000560
#define CPU_ID_2_0	0x000005A1
#define CPU_ID_3_0	0x000005A2

#define CPU_REV_1_0	0x010
#define CPU_REV_1_1	0x011
#define	CPU_REV_2_0		0x020
#define	CPU_REV_2_1		0x021
#define	CPU_REV_2_2	0x022
#define CPU_REV_C_0	0x030
#define CPU_REV_C_1	0x031
#define CPU_REV_C_2	0x032		/* 3.2 part was never produced ...*/
#define CPU_REV_C_3	0x033


/* MSR routing as follows*/
/* MSB = 1 means not for CPU*/
/* next 3 bits 1st port*/
/* next3 bits next port if through an GLIU*/
/* etc...*/

/* GLIU0 ports */
#define	GL0_GLIU0			0
#define	GL0_MC				1
#define	GL0_GLIU1			2
#define	GL0_CPU				3
#define	GL0_VG				4
#define	GL0_GP				5

/* GLIU1 ports */
#define	GL1_GLIU0			1
#define	GL1_DF				2
#define	GL1_GLCP			3
#define	GL1_PCI				4
#define GL1_VIP				5
#define GL1_AES				6


/* 1000xxxx, To get on GeodeLink one bit has to be set */
#define MSR_GLIU0		((GL0_GLIU0 << 29) + (1 << 28))
#define	MSR_MC				(GL0_MC		<< 29)	/* 2000xxxx */
#define	MSR_GLIU1			(GL0_GLIU1	<< 29)	/* 4000xxxx */
/* 0000xxxx this is not used for BIOS since code executing on CPU doesn't
 * need to be routed
 */
#define MSR_CPU			(GL0_CPU	<< 29)
#define	MSR_VG				(GL0_VG		<< 29)	/* 8000xxxx */
#define	MSR_GP				(GL0_GP		<< 29)	/* A000xxxx */

#define MSR_DF		((GL1_DF << 26) + MSR_GLIU1)		/* 4800xxxx */
#define MSR_GLCP	((GL1_GLCP << 26) + MSR_GLIU1)		/* 4C00xxxx */
#define MSR_PCI		((GL1_PCI << 26) + MSR_GLIU1)		/* 5000xxxx */
#define MSR_VIP			((GL1_VIP << 26) + MSR_GLIU1)	/* 5400xxxx */
#define MSR_AES			((GL1_AES << 26) + MSR_GLIU1)	/* 5800xxxx */
#define MSR_FG		MSR_GLCP

/*GeodeLink Interface Unit 0 (GLIU0) port0*/

#define	GLIU0_GLD_MSR_CAP			(MSR_GLIU0 + 0x2000)
#define GLIU0_GLD_MSR_ERROR		(MSR_GLIU0 + 0x2003)
#define	GLIU0_GLD_MSR_PM			(MSR_GLIU0 + 0x2004)

#define	GLIU0_DESC_BASE				(MSR_GLIU0 + 0x20)
#define	GLIU0_CAP					(MSR_GLIU0 + 0x86)
#define	GLIU0_GLD_MSR_COH			(MSR_GLIU0 + 0x80)
#define GLIU0_ARB				(MSR_GLIU0 + 0x82)
#define		ARB_UPPER_QUACK_EN_SET			(1 << 31)
#define		ARB_UPPER_DACK_EN_SET			(1 << 28)


/* Memory Controller GLIU0 port 1*/

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
#define		CF8F_LOWER_CAS_LAT_SHIFT			28
#define		CF8F_LOWER_ACT2ACTREF_SHIFT		24
#define		CF8F_LOWER_ACT2PRE_SHIFT			20
#define		CF8F_LOWER_PRE2ACT_SHIFT			16
#define		CF8F_LOWER_ACT2CMD_SHIFT			12
#define		CF8F_LOWER_ACT2ACT_SHIFT			8
#define		CF8F_UPPER_HOI_LOI_SET			(1 << 1)

#define	MC_CF1017_DATA			(MSR_MC + 0x1A)
#define		CF1017_LOWER_WR_TO_RD_SHIFT			28
#define		CF1017_LOWER_RD_TMG_CTL_SHIFT		24
#define		CF1017_LOWER_REF2ACT_SHIFT			16
#define		CF1017_LOWER_PM1_UP_DLY_SET			(1 << 8)
#define		CF1017_LOWER_WR2DAT_SHIFT			0

#define	MC_CFCLK_DBUG			(MSR_MC + 0x1D)

#define		CFCLK_UPPER_MTST_B2B_DIS_SET			(1 << 2)
#define		CFCLK_UPPER_MTST_RBEX_EN_SET		(1 << 1)
#define		CFCLK_UPPER_MTEST_EN_SET				(1 << 0)

#define		CFCLK_LOWER_FORCE_PRE_SET			(1 << 16)
#define		CFCLK_LOWER_TRISTATE_DIS_SET		(1 << 12)
#define		CFCLK_LOWER_MASK_CKE_SET1			(1 << 9)
#define		CFCLK_LOWER_MASK_CKE_SET0			(1 << 8)
#define		CFCLK_LOWER_SDCLK_SET				(0x0F << 0)

#define	MC_CF_RDSYNC			(MSR_MC	+ 0x1F)
#define MC_CF_PMCTR			(MSR_MC + 0x20)


/* GLIU1 GLIU0 port2*/

#define	GLIU1_GLD_MSR_CAP			(MSR_GLIU1 + 0x2000)
#define GLIU1_GLD_MSR_ERROR		(MSR_GLIU1 + 0x2003)
#define	GLIU1_GLD_MSR_PM			(MSR_GLIU1 + 0x2004)

#define	GLIU1_GLD_MSR_COH			(MSR_GLIU1 + 0x80)
#define GLIU1_PORT_ACTIVE		(MSR_GLIU1 + 0x81)
#define GLIU1_ARB				(MSR_GLIU1 + 0x82)



/* CPU	; does not need routing instructions since we are executing there.*/

#define	CPU_GLD_MSR_CAP						0x2000
#define	CPU_GLD_MSR_CONFIG					0x2001
#define	CPU_GLD_MSR_PM						0x2004

#define	CPU_GLD_MSR_DIAG					0x2005
#define		DIAG_SEL1_MODE_SHIFT				16
#define		DIAG_SEL1_SET					(1 << 31)
#define		DIAG_SEL0__MODE_SHIFT				0
#define		DIAG_SET0_SET					(1 << 15)

#define CPU_PF_CONF						0x1100
#define		RETURN_STACK_ENABLE_SET				(1 << 4)
#define		PF_CONF_CC_L1					(1 << 0)
#define CPU_PF_INVD						0x1102
#define		PF_RS_INVD_SET					(1 << 1)
#define		PF_CC_INVD_SET					(1 << 0)
#define CPU_PF_BIST						0x1140

#define	CPU_XC_CONFIG						0x1210
#define		XC_CONFIG_SUSP_ON_HLT				(1 << 0)
#define		XC_CONFIG_SUSP_ON_PAUSE			(1 << 1)

#define	CPU_ID_CONFIG						0x1250
#define		ID_CONFIG_SERIAL_SET				(1 << 0)

#define	CPU_AC_MSR							0x1301

/* SMM*/
#define CPU_AC_SMM_CTL					0x1301
#define		SMM_NMI_EN_SET					(1 << 0)
#define		SMM_SUSP_EN_SET					(1 << 1)
#define		NEST_SMI_EN_SET					(1 << 2)
#define		SMM_INST_EN_SET					(1 << 3)
#define		INTL_SMI_EN_SET					(1 << 4)
#define		EXTL_SMI_EN_SET					(1 << 5)

#define	CPU_EX_BIST							0x1428

/*IM*/
#define	CPU_IM_CONFIG							0x1700
#define		IM_CONFIG_LOWER_SERIAL_SET			(1 << 2)
#define		IM_CONFIG_LOWER_L0WE_SET			(1 << 6)
#define		IM_CONFIG_LOWER_ICD_SET					(1 << 8)
#define		IM_CONFIG_LOWER_EBE_SET				(1 << 10)
#define		IM_CONFIG_LOWER_ABSE_SET			(1 << 11)
#define		IM_CONFIG_LOWER_QWT_SET				(1 << 20)
#define	CPU_IC_INDEX							0x1710
#define	CPU_IC_DATA							0x1711
#define	CPU_IC_TAG							0x1712
#define	CPU_IC_TAG_I							0x1713
#define	CPU_ITB_INDEX							0x1720
#define	CPU_ITB_LRU							0x1721
#define	CPU_ITB_ENTRY							0x1722
#define	CPU_ITB_ENTRY_I							0x1723
#define	CPU_IM_BIST_TAG							0x1730
#define	CPU_IM_BIST_DATA						0x1731


/*DM MSR MAP*/
#define CPU_DM_CONFIG0 0x1800
#define	DM_CONFIG0_UPPER_WSREQ_SHIFT  12
#define		DM_CONFIG0_LOWER_EVCTONRPL_SET		(1 << 14)
#define	DM_CONFIG0_LOWER_WBINVD_SET	(1<<5)
#define		DM_CONFIG0_LOWER_DCDIS_SET			(1 << 8)
#define	DM_CONFIG0_LOWER_MISSER_SET		(1<<1)

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

#define	CPU_BC_CONF_0						0x1900
#define		TSC_SUSP_SET				 (1<<5)
#define		SUSP_EN_SET				 (1<<12)

#define CPU_BC_CONF_1						0x1901
#define CPU_BC_MSR_LOCK						0x1908
#define CPU_BC_L2_CONF						0x1920
#define		BC_L2_ENABLE_SET					(1 << 0)
#define		BC_L2_ALLOC_ENABLE_SET				(1 << 1)
#define		BC_L2_DM_ALLOC_ENABLE_SET			(1 << 2)
#define		BC_L2_IM_ALLOC_ENABLE_SET			(1 << 3)
#define		BC_L2_INVALIDATE_SET				(1 << 4)
#define CPU_BC_L2_STATUS					0x1921
#define CPU_BC_L2_INDEX						0x1922
#define CPU_BC_L2_DATA						0x1923
#define CPU_BC_L2_TAG						0x1924
#define CPU_BC_L2_TAG_AUTOINC				0x1925
#define CPU_BC_L2_BIST						0x1926
#define		BC_L2_BIST_TAG_ENABLE_SET			(1 << 0)
#define		BC_L2_BIST_TAG_DRT_EN_SET			(1 << 1)
#define		BC_L2_BIST_DATA_ENABLE_SET			(1 << 2)
#define		BC_L2_BIST_DATA_DRT_EN_SET			(1 << 3)
#define		BC_L2_BIST_MRU_ENABLE_SET			(1 << 4)
#define		BC_L2_BIST_MRU_DRT_EN_SET			(1 << 5)
#define CPU_BC_PMODE_MSR					0x1930
#define CPU_BC_MSS_ARRAY_CTL_ENA			0x1980
#define CPU_BC_MSS_ARRAY_CTL0				0x1981
#define CPU_BC_MSS_ARRAY_CTL1				0x1982
#define CPU_BC_MSS_ARRAY_CTL2				0x1983

#define CPU_FPU_MSR_MODE					0x1A00
#define		FPU_IE_SET					(1 << 0)

#define CPU_FP_UROM_BIST					0x1A03

#define CPU_CPUID0							0x3000
#define CPU_CPUID1							0x3001
#define CPU_CPUID2							0x3002
#define CPU_CPUID3							0x3003
#define CPU_CPUID4							0x3004
#define CPU_CPUID5							0x3005
#define CPU_CPUID6							0x3006
#define CPU_CPUID7							0x3007
#define CPU_CPUID8							0x3008
#define CPU_CPUID9							0x3009
#define CPU_CPUIDA							0x300A
#define CPU_CPUIDB							0x300B
#define CPU_CPUIDC							0x300C
#define CPU_CPUIDD							0x300D
#define CPU_CPUIDE							0x300E
#define CPU_CPUIDF							0x300F
#define CPU_CPUID10							0x3010
#define CPU_CPUID11							0x3011
#define CPU_CPUID12							0x3012
#define CPU_CPUID13							0x3013




	/*	VG GLIU0 port4*/


#define	VG_GLD_MSR_CAP				(MSR_VG + 0x2000)
#define	VG_GLD_MSR_CONFIG			(MSR_VG + 0x2001)
#define	VG_GLD_MSR_PM				(MSR_VG + 0x2004)
#define VG_BIST					(MSR_VG + 0x2010)



/*	GP GLIU0 port5*/


#define	GP_GLD_MSR_CAP				(MSR_GP + 0x2000)
#define	GP_GLD_MSR_CONFIG			(MSR_GP + 0x2001)
#define	GP_GLD_MSR_PM				(MSR_GP + 0x2004)



/*	DF GLIU0 port6*/


#define	DF_GLD_MSR_CAP					(MSR_DF + 0x2000)
#define	DF_GLD_MSR_MASTER_CONF			(MSR_DF + 0x2001)
#define		DF_LOWER_LCD_SHIFT				6
#define	DF_GLD_MSR_PM					(MSR_DF + 0x2004)
#define DF_BIST					(MSR_DF + 0x2005)



/* GeodeLink Control Processor GLIU1 port3*/

#define	GLCP_GLD_MSR_CAP			(MSR_GLCP + 0x2000)
#define	GLCP_GLD_MSR_CONF			(MSR_GLCP + 0x2001)
#define GLCP_GLD_MSR_SMI		(MSR_GLCP + 0x2002)
#define GLCP_GLD_MSR_ERROR		(MSR_GLCP + 0x2003)
#define	GLCP_GLD_MSR_PM				(MSR_GLCP + 0x2004)

#define	GLCP_DELAY_CONTROLS			(MSR_GLCP + 0x0F)
#define GLCP_SYS_RSTPLL			(MSR_GLCP + 0x14)	/* R/W */
#define		RSTPLL_UPPER_GLMULT_SHIFT			7
#define		RSTPLL_UPPER_GLDIV_SHIFT			6
#define		RSTPLL_UPPER_CPUMULT_SHIFT			1
#define		RSTPLL_UPPER_CPUDIV_SHIFT			0
#define		RSTPLL_LOWER_SWFLAGS_SHIFT			26
#define		RSTPLL_LOWER_SWFLAGS_MASK			\
				(0x03F << RSTPLL_LOWER_SWFLAGS_SHIFT)
#define		RSTPPL_LOWER_HOLD_COUNT_SHIFT			16
#define		RSTPPL_LOWER_COREBYPASS_SHIFT		12
#define		RSTPPL_LOWER_GLBYPASS_SHIFT			11
#define		RSTPPL_LOWER_PCISPEED_SHIFT			7
#define		RSTPPL_LOWER_BOOTSTRAP_SHIFT		1
#define		RSTPLL_LOWER_BOOTSTRAP_MASK			\
				(0x07F << RSTPLL_LOWER_BOOTSTRAP_SHIFT)

#define		RSTPPL_LOWER_GLLOCK_SET				(1 << 25)
#define		RSTPPL_LOWER_CORELOCK_SET			(1 << 24)
#define		RSTPPL_LOWER_LOCKWAIT_SET			(1 << 15)
#define		RSTPPL_LOWER_CLPD_SET				(1 << 14)
#define		RSTPPL_LOWER_COREPD_SET				(1 << 13)
#define		RSTPPL_LOWER_MBBYPASS_SET			(1 << 12)
#define		RSTPPL_LOWER_COREBYPASS_SET			(1 << 11)
#define		RSTPPL_LOWER_LPFEN_SET				(1 << 10)
#define		RSTPPL_LOWER_CPU_SEMI_SYNC_SET		 (1<<9)
#define		RSTPPL_LOWER_PCI_SEMI_SYNC_SET		 (1<<8)
#define		RSTPPL_LOWER_CHIP_RESET_SET		 (1<<0)

#define GLCP_DOWSER				(MSR_GLCP + 0x0E)
#define GLCP_DBGCLKCTL			(MSR_GLCP + 0x16)
#define GLCP_REVID				(MSR_GLCP + 0x17)
#define GLCP_TH_OD				(MSR_GLCP + 0x1E)
#define GLCP_FIFOCTL			(MSR_GLCP + 0x5E)
#define GLCP_BIST				GLCP_FIFOCTL

#define MSR_INIT				(MSR_GLCP + 0x33)


/*  GLIU1 port 4*/

#define	GLPCI_GLD_MSR_CAP			(MSR_PCI + 0x2000)
#define	GLPCI_GLD_MSR_CONFIG			(MSR_PCI + 0x2001)
#define	GLPCI_GLD_MSR_PM				(MSR_PCI + 0x2004)

#define	GLPCI_CTRL			(MSR_PCI + 0x2010)
#define	GLPCI_CTRL_UPPER_FTH_SHIFT				28
#define	GLPCI_CTRL_UPPER_RTH_SHIFT				24
#define	GLPCI_CTRL_UPPER_SBRTH_SHIFT				20
#define GLPCI_CTRL_UPPER_RTL_SHIFT				17
#define	GLPCI_CTRL_UPPER_DTL_SHIFT				14
#define	GLPCI_CTRL_UPPER_WTO_SHIFT				11
#define GLPCI_CTRL_UPPER_SLTO_SHIFT				10
#define	GLPCI_CTRL_UPPER_ILTO_SHIFT				8
#define GLPCI_CTRL_UPPER_LAT_SHIFT				3

#define	GLPCI_CTRL_LOWER_IRFT_SHIFT				18
#define	GLPCI_CTRL_LOWER_IRFC_SHIFT				16
#define	GLPCI_CTRL_LOWER_ER_SET					(1<<11)
#define	GLPCI_CTRL_LOWER_LDE_SET					(1<<9)
#define	GLPCI_CTRL_LOWER_OWC_SET					(1<<4)
#define	GLPCI_CTRL_LOWER_IWC_SET					(1<<3)
#define	GLPCI_CTRL_LOWER_PCD_SET					(1<<2)
#define	GLPCI_CTRL_LOWER_ME_SET					(1<<0)

#define	GLPCI_ARB			(MSR_PCI + 0x2011)
#define GLPCI_ARB_UPPER_CR_SHIFT			28
#define GLPCI_ARB_UPPER_R2_SHIFT			24
#define GLPCI_ARB_UPPER_R1_SHIFT			20
#define GLPCI_ARB_UPPER_R0_SHIFT			16
#define GLPCI_ARB_UPPER_CH_SHIFT			12
#define GLPCI_ARB_UPPER_H2_SHIFT			8
#define GLPCI_ARB_UPPER_H1_SHIFT			4
#define GLPCI_ARB_UPPER_H0_SHIFT			0

#define	GLPCI_ARB_LOWER_COV_SET						(1<<23)
#define GLPCI_ARB_LOWER_VO2_SET				(1 << 22)
#define GLPCI_ARB_LOWER_OV1_SET				(1 << 21)
#define GLPCI_ARB_LOWER_OV0_SET				(1 << 20)
#define	GLPCI_ARB_LOWER_MSK2_SET					(1<<18)
#define	GLPCI_ARB_LOWER_MSK1_SET					(1<<17)
#define	GLPCI_ARB_LOWER_MSK0_SET					(1<<16)
#define	GLPCI_ARB_LOWER_CPRE_SET					(1<<11)
#define	GLPCI_ARB_LOWER_PRE2_SET					(1<<10)
#define	GLPCI_ARB_LOWER_PRE1_SET					(1<<9)
#define	GLPCI_ARB_LOWER_PRE0_SET					(1<<8)
#define	GLPCI_ARB_LOWER_BM1_SET						(1<<7)
#define	GLPCI_ARB_LOWER_BM0_SET						(1<<6)
#define GLPCI_ARB_LOWER_EA_SET				(1 << 2)
#define GLPCI_ARB_LOWER_BMD_SET				(1 << 1)
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

#define GLPCI_ExtMSR	(MSR_PCI + 0x201E)

#define	GLPCI_SPARE			(MSR_PCI + 0x201F)
#define		GLPCI_SPARE_LOWER_AILTO_SET			(1<<6)
#define		GLPCI_SPARE_LOWER_PPD_SET			(1<<5)
#define		GLPCI_SPARE_LOWER_PPC_SET			(1<<4)
#define		GLPCI_SPARE_LOWER_MPC_SET			(1<<3)
#define		GLPCI_SPARE_LOWER_MME_SET			(1<<2)
#define		GLPCI_SPARE_LOWER_NSE_SET			(1<<1)
#define		GLPCI_SPARE_LOWER_SUPO_SET			(1<<0)



/*  VIP GLIU1 port 5*/

#define VIP_GLD_MSR_CAP		(MSR_VIP + 0x2000)
#define VIP_GLD_MSR_CONFIG	(MSR_VIP + 0x2001)
#define VIP_GLD_MSR_PM		(MSR_VIP + 0x2004)
#define VIP_BIST			(MSR_VIP + 0x2005)

/*  AES GLIU1 port 6*/

#define AES_GLD_MSR_CAP		(MSR_AES + 0x2000)
#define AES_GLD_MSR_CONFIG	(MSR_AES + 0x2001)
#define AES_GLD_MSR_PM		(MSR_AES + 0x2004)
#define AES_CONTROL			(MSR_AES + 0x2006)


/* from MC spec */
#define MIN_MOD_BANKS		1
#define MAX_MOD_BANKS		2
#define MIN_DEV_BANKS		2
#define MAX_DEV_BANKS		4
#define MAX_COL_ADDR		17

/* GLIU typedefs */
/* Base Mask - map power of 2 size aligned region*/
#define BM			1
#define BMO			2	/*  BM with an offset*/
#define R				3	/*  Range - 4k range minimum*/
#define RO			4	/*  R with offset*/
/* Swiss 0xCeese - maps a 256K region in to 16K 0xcunks. Set W/R*/
#define SC			5
#define BMIO			6	/*  Base Mask IO*/
#define SCIO			7	/*  Swiss 0xCeese IO*/
/* Special marker for Shadow SC descriptors so setShadow proc is independent
 * of CPU
 */
#define SC_SHADOW	8
/* Special marker for SYSMEM R descriptors so GLIUInit proc is independent
 * of CPU
 */
#define R_SYSMEM		9
#define BMO_SMM		10	/*  Special marker for SMM*/
#define BM_SMM		11	/*  Special marker for SMM*/
#define BMO_DMM		12	/*  Special marker for DMM*/
#define BM_DMM		13	/*  Special marker for DMM*/
#define RO_FB			14	/*  special for Frame buffer.*/
#define R_FB			15	/*  special for FB.*/
#define OTHER			0x0FE /*  Special marker for other*/
#define GL_END		 0x0FF	/*  end*/

#define MSR_GL0	(GL1_GLIU0 << 29)


/* Platform stuff but unlikely to change */
/*  Set up desc addresses from 20 - 3f*/
/*  This is chip specific!*/
#define MSR_GLIU0_BASE1			(MSR_GLIU0 + 0x20)		/*  BM*/
#define MSR_GLIU0_BASE2			(MSR_GLIU0 + 0x21)		/*  BM*/
#define MSR_GLIU0_BASE3			(MSR_GLIU0 + 0x22)		/*  BM*/
#define MSR_GLIU0_BASE4			(MSR_GLIU0 + 0x23)		/*  BM*/
#define MSR_GLIU0_BASE5			(MSR_GLIU0 + 0x24)		/*  BM*/
#define MSR_GLIU0_BASE6			(MSR_GLIU0 + 0x25)		/*  BM*/

#define GLIU0_P2D_BMO_0			(MSR_GLIU0 + 0x26)
#define GLIU0_P2D_BMO_1			(MSR_GLIU0 + 0x27)

#define MSR_GLIU0_SMM			(GLIU0_P2D_BMO_0)
#define MSR_GLIU0_DMM			(GLIU0_P2D_BMO_1)

#define GLIU0_P2D_R				(MSR_GLIU0 + 0x28)
#define MSR_GLIU0_SYSMEM		(GLIU0_P2D_R)

#define GLIU0_P2D_RO_0			(MSR_GLIU0 + 0x29)
#define GLIU0_P2D_RO_1			(MSR_GLIU0 + 0x2A)
#define GLIU0_P2D_RO_2			(MSR_GLIU0 + 0x2B)

/*  SCO should only be SC*/
#define MSR_GLIU0_SHADOW		(MSR_GLIU0 + 0x2C)

#define GLIU0_IOD_BM_0			(MSR_GLIU0 + 0xE0)
#define GLIU0_IOD_BM_1			(MSR_GLIU0 + 0xE1)
#define GLIU0_IOD_BM_2			(MSR_GLIU0 + 0xE2)

#define GLIU0_IOD_SC_0			(MSR_GLIU0 + 0xE3)
#define GLIU0_IOD_SC_1			(MSR_GLIU0 + 0xE4)
#define GLIU0_IOD_SC_2			(MSR_GLIU0 + 0xE5)
#define GLIU0_IOD_SC_3			(MSR_GLIU0 + 0xE6)
#define GLIU0_IOD_SC_4			(MSR_GLIU0 + 0xE7)
#define GLIU0_IOD_SC_5			(MSR_GLIU0 + 0xE8)


#define MSR_GLIU1_BASE1			(MSR_GLIU1 + 0x20)		/*  BM*/
#define MSR_GLIU1_BASE2			(MSR_GLIU1 + 0x21)		/*  BM*/
#define MSR_GLIU1_BASE3			(MSR_GLIU1 + 0x22)		/*  BM*/
#define MSR_GLIU1_BASE4			(MSR_GLIU1 + 0x23)		/*  BM*/
#define MSR_GLIU1_BASE5			(MSR_GLIU1 + 0x24)		/*  BM*/
#define MSR_GLIU1_BASE6			(MSR_GLIU1 + 0x25)		/*  BM*/
#define MSR_GLIU1_BASE7			(MSR_GLIU1 + 0x26)		/*  BM*/
#define MSR_GLIU1_BASE8			(MSR_GLIU1 + 0x27)		/*  BM*/
#define MSR_GLIU1_BASE9			(MSR_GLIU1 + 0x28)		/*  BM*/
#define MSR_GLIU1_BASE10		(MSR_GLIU1 + 0x29)		/*  BM*/

#define GLIU1_P2D_R_0			(MSR_GLIU1 + 0x2A)
#define GLIU1_P2D_R_1			(MSR_GLIU1 + 0x2B)
#define GLIU1_P2D_R_2			(MSR_GLIU1 + 0x2C)
#define GLIU1_P2D_R_3			(MSR_GLIU1 + 0x2D)


#define MSR_GLIU1_SHADOW		(MSR_GLIU1 + 0x2E)

#define MSR_GLIU1_SYSMEM		(GLIU1_P2D_R_0)

#define MSR_GLIU1_SMM			(MSR_GLIU1_BASE4)		/*  BM*/
#define MSR_GLIU1_DMM			(MSR_GLIU1_BASE5)		/*  BM*/

#define GLIU1_IOD_BM_0			(MSR_GLIU1 + 0xE0)
#define GLIU1_IOD_BM_1			(MSR_GLIU1 + 0xE1)
#define GLIU1_IOD_BM_2			(MSR_GLIU1 + 0xE2)

#define GLIU1_IOD_SC_0			(MSR_GLIU1 + 0xE3)
#define GLIU1_IOD_SC_1			(MSR_GLIU1 + 0xE4)
#define GLIU1_IOD_SC_2			(MSR_GLIU1 + 0xE5)
#define GLIU1_IOD_SC_3			(MSR_GLIU1 + 0xE6)
/*  FooGlue F0 for FPU*/
#define MSR_GLIU1_FPU_TRAP		(GLIU1_IOD_SC_0)

/* ------------------------  */

#define SMM_OFFSET	0x80400000	/* above 2GB */
#define SMM_SIZE	128	/* changed SMM_SIZE from 256 KB to 128 KB */

/* DRAM_TERMINATED affects how the DELAY register is set. */
#define DRAM_TERMINATED 'T'
#define DRAM_UNTERMINATED 't'
/* Bitfield definitions for the DELAY register */
#define DELAY_UPPER_DISABLE_CLK135	(1 << 23)
#define DELAY_LOWER_STATUS_MASK		0x7C0

#if !defined(__ASSEMBLER__)

#include <stdint.h>
#include <arch/cpu.h>

#if defined(__PRE_RAM__)
void cpuRegInit(int debug_clock_disable, u8 dimm0, u8 dimm1, int terminated);
void SystemPreInit(void);
void asmlinkage mainboard_romstage_entry(unsigned long bist);
void done_cache_as_ram_main(void);
#endif
void cpubug(void);
#endif

#endif
