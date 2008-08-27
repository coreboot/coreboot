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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef CPU_AMD_LXDEF_H
#define CPU_AMD_LXDEF_H

#ifndef __ASSEMBLER__
#include <types.h>
#include <io.h>
#endif

#define CPU_ID_1_X				0x00000560	/* Stepping ID 1.x CPUbug fix to change it to 5A0 */
#define CPU_ID_2_0				0x000005A1
#define CPU_ID_3_0				0x000005A2

#define CPU_REV_1_0				0x010
#define CPU_REV_1_1				0x011
#define CPU_REV_2_0				0x020
#define CPU_REV_2_1				0x021
#define CPU_REV_2_2				0x022
#define CPU_REV_C_0				0x030
#define CPU_REV_C_1				0x031
#define CPU_REV_C_2				0x032	/* 3.2 part was never produced ... */
#define CPU_REV_C_3				0x033

/* MSR routing as follows*/
/* MSB = 1 means not for CPU*/
/* next 3 bits 1st port*/
/* next3 bits next port if through an GLIU*/
/* etc...*/

/* GLIU0 ports */
#define GL0_GLIU0				0
#define GL0_MC					1
#define GL0_GLIU1				2
#define GL0_CPU					3
#define GL0_VG					4
#define GL0_GP					5

/* GLIU1 ports */
#define GL1_GLIU0				1
#define GL1_DF					2
#define GL1_GLCP				3
#define GL1_PCI					4
#define GL1_VIP					5
#define GL1_AES					6

#define MSR_GLIU0				(GL0_GLIU0 << 29) + (1 << 28)	/* 1000xxxx, To get on GeodeLink one bit has to be set */
#define MSR_MC					(GL0_MC		<< 29)	/* 2000xxxx */
#define MSR_GLIU1				(GL0_GLIU1	<< 29)	/* 4000xxxx */
#define MSR_CPU					(GL0_CPU	<< 29)	/* 0000xxxx this is not used for BIOS since code executing on CPU doesn't need to be routed */
#define MSR_VG					(GL0_VG		<< 29)	/* 8000xxxx */
#define MSR_GP					(GL0_GP		<< 29)	/* A000xxxx */

#define MSR_DF					((GL1_DF << 26) + MSR_GLIU1)	/* 4800xxxx */
#define MSR_GLCP				((GL1_GLCP << 26) + MSR_GLIU1)	/* 4C00xxxx */
#define MSR_PCI					((GL1_PCI << 26) + MSR_GLIU1)	/* 5000xxxx */
#define MSR_VIP					((GL1_VIP << 26) + MSR_GLIU1)	/* 5400xxxx */
#define MSR_AES					((GL1_AES << 26) + MSR_GLIU1)	/* 5800xxxx */
#define MSR_FG					MSR_GLCP

/*GeodeLink Interface Unit 0 (GLIU0) port0*/

#define GLIU0_GLD_MSR_CAP			(MSR_GLIU0 + 0x2000)
#define GLIU0_GLD_MSR_ERROR			(MSR_GLIU0 + 0x2003)
#define GLIU0_GLD_MSR_PM			(MSR_GLIU0 + 0x2004)

#define GLIU0_DESC_BASE				(MSR_GLIU0 + 0x20)
#define GLIU0_CAP				(MSR_GLIU0 + 0x86)
#define GLIU0_GLD_MSR_COH			(MSR_GLIU0 + 0x80)

#define GLIU0_ARB				(MSR_GLIU0 + 0x82)
#define ARB_UPPER_QUACK_EN_SET			(1 << 31)
#define ARB_UPPER_DACK_EN_SET			(1 << 28)

/* Memory Controller GLIU0 port 1*/

#define MC_GLD_MSR_CAP				(MSR_MC + 0x2000)
#define MC_GLD_MSR_PM				(MSR_MC + 0x2004)

#define MC_CF07_DATA				(MSR_MC + 0x18)
#define CF07_UPPER_D1_SZ_SHIFT			28
#define CF07_UPPER_D1_MB_SHIFT			24
#define CF07_UPPER_D1_CB_SHIFT			20
#define CF07_UPPER_D1_PSZ_SHIFT			16
#define CF07_UPPER_D0_SZ_SHIFT			12
#define CF07_UPPER_D0_MB_SHIFT			8
#define CF07_UPPER_D0_CB_SHIFT			4
#define CF07_UPPER_D0_PSZ_SHIFT			0

#define CF07_LOWER_REF_INT_SHIFT		8
#define CF07_LOWER_LOAD_MODE_DDR_SET		(1 << 28)
#define CF07_LOWER_LOAD_MODE_DLL_RESET		(1 << 27)
#define CF07_LOWER_EMR_QFC_SET			(1 << 26)
#define CF07_LOWER_EMR_DRV_SET			(1 << 25)
#define CF07_LOWER_REF_TEST_SET			(1 << 3)
#define CF07_LOWER_PROG_DRAM_SET		(1 << 0)

#define MC_CF8F_DATA				(MSR_MC + 0x19)
#define CF8F_UPPER_XOR_BS_SHIFT			19
#define CF8F_UPPER_XOR_MB0_SHIFT		18
#define CF8F_UPPER_XOR_BA1_SHIFT		17
#define CF8F_UPPER_XOR_BA0_SHIFT		16
#define CF8F_UPPER_REORDER_DIS_SET		(1 << 8)
#define CF8F_LOWER_CAS_LAT_SHIFT		28
#define CF8F_LOWER_ACT2ACTREF_SHIFT		24
#define CF8F_LOWER_ACT2PRE_SHIFT		20
#define CF8F_LOWER_PRE2ACT_SHIFT		16
#define CF8F_LOWER_ACT2CMD_SHIFT		12
#define CF8F_LOWER_ACT2ACT_SHIFT		8
#define CF8F_UPPER_HOI_LOI_SET			(1 << 1)

#define MC_CF1017_DATA				(MSR_MC + 0x1A)
#define CF1017_LOWER_WR_TO_RD_SHIFT		28
#define CF1017_LOWER_RD_TMG_CTL_SHIFT		24
#define CF1017_LOWER_REF2ACT_SHIFT		16
#define CF1017_LOWER_PM1_UP_DLY_SET		(1 << 8)
#define CF1017_LOWER_WR2DAT_SHIFT		0

#define MC_CFCLK_DBUG				(MSR_MC + 0x1D)

#define CFCLK_UPPER_MTST_B2B_DIS_SET		(1 << 2)
#define CFCLK_UPPER_MTST_RBEX_EN_SET		(1 << 1)
#define CFCLK_UPPER_MTEST_EN_SET		(1 << 0)

#define CFCLK_LOWER_FORCE_PRE_SET		(1 << 16)
#define CFCLK_LOWER_TRISTATE_DIS_SET		(1 << 12)
#define CFCLK_LOWER_MASK_CKE_SET1		(1 << 9)
#define CFCLK_LOWER_MASK_CKE_SET0		(1 << 8)
#define CFCLK_LOWER_SDCLK_SET			(0x0F << 0)

#define MC_CF_RDSYNC				(MSR_MC	+ 0x1F)
#define MC_CF_PMCTR				(MSR_MC + 0x20)

/* GLIU1 GLIU0 port2*/

#define GLIU1_GLD_MSR_CAP			(MSR_GLIU1 + 0x2000)
#define GLIU1_GLD_MSR_ERROR			(MSR_GLIU1 + 0x2003)
#define GLIU1_GLD_MSR_PM			(MSR_GLIU1 + 0x2004)

#define GLIU1_GLD_MSR_COH			(MSR_GLIU1 + 0x80)
#define GLIU1_PORT_ACTIVE			(MSR_GLIU1 + 0x81)
#define GLIU1_ARB				(MSR_GLIU1 + 0x82)

/* CPU		; does not need routing instructions since we are executing there.*/

#define CPU_GLD_MSR_CAP				0x2000
#define CPU_GLD_MSR_CONFIG			0x2001
#define CPU_GLD_MSR_PM				0x2004

#define CPU_GLD_MSR_DIAG			0x2005
#define DIAG_SEL1_MODE_SHIFT			16
#define DIAG_SEL1_SET				(1 << 31)
#define DIAG_SEL0__MODE_SHIFT			0
#define DIAG_SET0_SET				(1 << 15)

#define CPU_PF_CONF				0x1100
#define RETURN_STACK_ENABLE_SET			(1 << 4)
#define PF_CONF_CC_L1				(1 << 0)
#define CPU_PF_INVD				0x1102
#define PF_RS_INVD_SET				(1 << 1)
#define PF_CC_INVD_SET				(1 << 0)
#define CPU_PF_BIST				0x1140

#define CPU_XC_CONFIG				0x1210
#define XC_CONFIG_SUSP_ON_HLT			(1 << 0)
#define XC_CONFIG_SUSP_ON_PAUSE			(1 << 1)

#define CPU_ID_CONFIG				0x1250
#define ID_CONFIG_SERIAL_SET			(1 << 0)

#define CPU_AC_MSR				0x1301

/* SMM*/
#define CPU_AC_SMM_CTL				0x1301
#define SMM_NMI_EN_SET				(1 << 0)
#define SMM_SUSP_EN_SET				(1 << 1)
#define NEST_SMI_EN_SET				(1 << 2)
#define SMM_INST_EN_SET				(1 << 3)
#define INTL_SMI_EN_SET				(1 << 4)
#define EXTL_SMI_EN_SET				(1 << 5)

#define CPU_EX_BIST				0x1428

 /*IM*/
#define CPU_IM_CONFIG				0x1700
#define IM_CONFIG_LOWER_SERIAL_SET		(1 << 2)
#define IM_CONFIG_LOWER_L0WE_SET		(1 << 6)
#define IM_CONFIG_LOWER_ICD_SET			(1 << 8)
#define IM_CONFIG_LOWER_EBE_SET			(1 << 10)
#define IM_CONFIG_LOWER_ABSE_SET		(1 << 11)
#define IM_CONFIG_LOWER_QWT_SET			(1 << 20)

#define CPU_IC_INDEX				0x1710
#define CPU_IC_DATA				0x1711
#define CPU_IC_TAG				0x1712
#define CPU_IC_TAG_I				0x1713
#define CPU_ITB_INDEX				0x1720
#define CPU_ITB_LRU				0x1721
#define CPU_ITB_ENTRY				0x1722
#define CPU_ITB_ENTRY_I				0x1723
#define CPU_IM_BIST_TAG				0x1730
#define CPU_IM_BIST_DATA			0x1731
/*DM MSR MAP*/
#define CPU_DM_CONFIG0				0x1800
#define DM_CONFIG0_UPPER_WSREQ_SHIFT		12
#define DM_CONFIG0_LOWER_EVCTONRPL_SET		(1 << 14)
#define DM_CONFIG0_LOWER_WBINVD_SET		(1<<5)
#define DM_CONFIG0_LOWER_DCDIS_SET		(1 << 8)
#define DM_CONFIG0_LOWER_MISSER_SET		(1<<1)

/* Region CONFiguration registers  (RCONF) */
/* There are control bits for memory configuration. They are at different 
 * offsets depending on the MSR. We define them here with values
 * independent of their position in a 64-bit MSR, with a "shift" value
 * to get them into the right place. To use them for, e.g., the 
 * CPU_RCONF_DEFAULT register, you would use 
 * RCONF_WT(RCONF_DEFAULT_LOWER_SYSRC_SHIFT)
 */
#define RCONF_WS(x)	(1<<(5+x))	/* Write-serialize */ 
#define RCONF_WC(x)	(1<<(4+x))	/* Write-combine */ 
#define RCONF_WT(x)	(1<<(3+x))	/* Write-through */ 
#define RCONF_WP(x)	(1<<(2+x))	/* Write-protect */ 
#define RCONF_WA(x)	(1<<(1+x))	/* Write-allocate */ 
#define RCONF_CD(x)	(1<<(0+x))	/* Cache Disable */

#define CPU_RCONF_DEFAULT			0x1808
#define RCONF_DEFAULT_UPPER_ROMRC_SHIFT		24
#define RCONF_DEFAULT_UPPER_ROMBASE_SHIFT	4
#define RCONF_DEFAULT_UPPER_DEVRC_HI_SHIFT	0
#define RCONF_DEFAULT_LOWER_DEVRC_LOW_SHIFT	28
#define RCONF_DEFAULT_LOWER_SYSTOP_SHIFT	8
#define RCONF_DEFAULT_LOWER_SYSRC_SHIFT		0

#define CPU_RCONF_BYPASS			0x180A
#define CPU_RCONF_A0_BF				0x180B
#define CPU_RCONF_C0_DF				0x180C
#define CPU_RCONF_E0_FF				0x180D
#define CPU_RCONF_SMM				0x180E
#define RCONF_SMM_UPPER_SMMTOP_SHIFT		12
#define RCONF_SMM_UPPER_RCSMM_SHIFT		0
#define RCONF_SMM_LOWER_SMMBASE_SHIFT		12
#define RCONF_SMM_LOWER_RCNORM_SHIFT		0
#define RCONF_SMM_LOWER_EN_SET			(1<<8)

#define CPU_RCONF_DMM				0x180F
#define RCONF_DMM_UPPER_DMMTOP_SHIFT		12
#define RCONF_DMM_UPPER_RCDMM_SHIFT		0
#define RCONF_DMM_LOWER_DMMBASE_SHIFT		12
#define RCONF_DMM_LOWER_RCNORM_SHIFT		0
#define RCONF_DMM_LOWER_EN_SET			(1<<8)

#define CPU_RCONF0				0x1810
#define CPU_RCONF1				0x1811
#define CPU_RCONF2				0x1812
#define CPU_RCONF3				0x1813
#define CPU_RCONF4				0x1814
#define CPU_RCONF5				0x1815
#define CPU_RCONF6				0x1816
#define CPU_RCONF7				0x1817
#define CPU_CR1_MSR				0x1881
#define CPU_CR2_MSR				0x1882
#define CPU_CR3_MSR				0x1883
#define CPU_CR4_MSR				0x1884
#define CPU_DC_INDEX				0x1890
#define CPU_DC_DATA				0x1891
#define CPU_DC_TAG				0x1892
#define CPU_DC_TAG_I				0x1893
#define CPU_SNOOP				0x1894
#define CPU_DTB_INDEX				0x1898
#define CPU_DTB_LRU				0x1899
#define CPU_DTB_ENTRY				0x189A
#define CPU_DTB_ENTRY_I				0x189B
#define CPU_L2TB_INDEX				0x189C
#define CPU_L2TB_LRU				0x189D
#define CPU_L2TB_ENTRY				0x189E
#define CPU_L2TB_ENTRY_I			0x189F
#define CPU_DM_BIST				0x18C0
#define CPU_BC_CONF_0				0x1900
#define TSC_SUSP_SET				(1<<5)
#define SUSP_EN_SET				(1<<12)

#define CPU_BC_CONF_1				0x1901
#define CPU_BC_MSR_LOCK				0x1908
#define CPU_BC_L2_CONF				0x1920
#define BC_L2_ENABLE_SET			(1 << 0)
#define BC_L2_ALLOC_ENABLE_SET			(1 << 1)
#define BC_L2_DM_ALLOC_ENABLE_SET		(1 << 2)
#define BC_L2_IM_ALLOC_ENABLE_SET		(1 << 3)
#define BC_L2_INVALIDATE_SET			(1 << 4)

#define CPU_BC_L2_STATUS			0x1921
#define CPU_BC_L2_INDEX				0x1922
#define CPU_BC_L2_DATA				0x1923
#define CPU_BC_L2_TAG				0x1924
#define CPU_BC_L2_TAG_AUTOINC			0x1925
#define CPU_BC_L2_BIST				0x1926
#define BC_L2_BIST_TAG_ENABLE_SET		(1 << 0)
#define BC_L2_BIST_TAG_DRT_EN_SET		(1 << 1)
#define BC_L2_BIST_DATA_ENABLE_SET		(1 << 2)
#define BC_L2_BIST_DATA_DRT_EN_SET		(1 << 3)
#define BC_L2_BIST_MRU_ENABLE_SET		(1 << 4)
#define BC_L2_BIST_MRU_DRT_EN_SET		(1 << 5)

#define CPU_BC_PMODE_MSR			0x1930
#define CPU_BC_MSS_ARRAY_CTL_ENA		0x1980
#define CPU_BC_MSS_ARRAY_CTL0			0x1981
#define CPU_BC_MSS_ARRAY_CTL1			0x1982
#define CPU_BC_MSS_ARRAY_CTL2			0x1983
#define CPU_FPU_MSR_MODE			0x1A00
#define FPU_IE_SET				(1 << 0)

#define CPU_FP_UROM_BIST			0x1A03
#define CPU_CPUID0				0x3000
#define CPU_CPUID1				0x3001
#define CPU_CPUID2				0x3002
#define CPU_CPUID3				0x3003
#define CPU_CPUID4				0x3004
#define CPU_CPUID5				0x3005
#define CPU_CPUID6				0x3006
#define CPU_CPUID7				0x3007
#define CPU_CPUID8				0x3008
#define CPU_CPUID9				0x3009
#define CPU_CPUIDA				0x300A
#define CPU_CPUIDB				0x300B
#define CPU_CPUIDC				0x300C
#define CPU_CPUIDD				0x300D
#define CPU_CPUIDE				0x300E
#define CPU_CPUIDF				0x300F
#define CPU_CPUID10				0x3010
#define CPU_CPUID11				0x3011
#define CPU_CPUID12				0x3012
#define CPU_CPUID13				0x3013
    /*      VG GLIU0 port4 */
#define VG_GLD_MSR_CAP				(MSR_VG + 0x2000)
#define VG_GLD_MSR_CONFIG			(MSR_VG + 0x2001)
#define VG_GLD_MSR_PM				(MSR_VG + 0x2004)
#define VG_BIST					(MSR_VG + 0x2010)
/*	GP GLIU0 port5*/
#define GP_GLD_MSR_CAP				(MSR_GP + 0x2000)
#define GP_GLD_MSR_CONFIG			(MSR_GP + 0x2001)
#define GP_GLD_MSR_PM				(MSR_GP + 0x2004)
/*	DF GLIU0 port6*/
#define DF_GLD_MSR_CAP				(MSR_DF + 0x2000)
#define DF_GLD_MSR_MASTER_CONF			(MSR_DF + 0x2001)
#define DF_LOWER_LCD_SHIFT			6
#define DF_GLD_MSR_PM				(MSR_DF + 0x2004)
#define DF_BIST					(MSR_DF + 0x2005)
/* GeodeLink Control Processor GLIU1 port3*/
#define GLCP_GLD_MSR_CAP			(MSR_GLCP + 0x2000)
#define GLCP_GLD_MSR_CONF			(MSR_GLCP + 0x2001)
#define GLCP_GLD_MSR_SMI			(MSR_GLCP + 0x2002)
#define GLCP_GLD_MSR_ERROR			(MSR_GLCP + 0x2003)
#define GLCP_GLD_MSR_PM				(MSR_GLCP + 0x2004)
#define GLCP_DELAY_CONTROLS			(MSR_GLCP + 0x0F)
#define DELAY_UPPER_DISABLE_CLK135	(1 << 23)
#define DELAY_LOWER_STATUS_MASK		0x7C0

/* DRAM_TERMINATED affects how the DELAY register is set. */
#define DRAM_TERMINATED 'T'
#define DRAM_UNTERMINATED 't'

#define GLCP_SYS_RSTPLL				(MSR_GLCP + 0x14)	/* R/W */
#define RSTPLL_UPPER_GLMULT_SHIFT		7
#define RSTPLL_UPPER_GLMULT_MASK		0x1F
#define RSTPLL_UPPER_GLDIV_SHIFT		6
#define RSTPLL_UPPER_CPUMULT_SHIFT		1
#define RSTPLL_UPPER_CPUMULT_MASK		0x1F
#define RSTPLL_UPPER_CPUDIV_SHIFT		0
#define RSTPLL_LOWER_SWFLAGS_SHIFT		26
#define RSTPLL_LOWER_SWFLAGS_MASK		(0x03F << RSTPLL_LOWER_SWFLAGS_SHIFT)
#define RSTPPL_LOWER_HOLD_COUNT_SHIFT		16
#define RSTPPL_LOWER_COREBYPASS_SHIFT		12
#define RSTPPL_LOWER_GLBYPASS_SHIFT		11
#define RSTPPL_LOWER_PCISPEED_SHIFT		7
#define RSTPPL_LOWER_BOOTSTRAP_SHIFT		1
#define RSTPLL_LOWER_BOOTSTRAP_MASK		(0x07F << RSTPLL_LOWER_BOOTSTRAP_SHIFT)
#define RSTPPL_LOWER_GLLOCK_SET			(1 << 25)
#define RSTPPL_LOWER_CORELOCK_SET		(1 << 24)
#define RSTPPL_LOWER_LOCKWAIT_SET		(1 << 15)
#define RSTPPL_LOWER_CLPD_SET			(1 << 14)
#define RSTPPL_LOWER_COREPD_SET			(1 << 13)
#define RSTPPL_LOWER_MBBYPASS_SET		(1 << 12)
#define RSTPPL_LOWER_COREBYPASS_SET		(1 << 11)
#define RSTPPL_LOWER_LPFEN_SET			(1 << 10)
#define RSTPPL_LOWER_CPU_SEMI_SYNC_SET		(1<<9)
#define RSTPPL_LOWER_PCI_SEMI_SYNC_SET		(1<<8)
#define RSTPPL_LOWER_CHIP_RESET_SET		(1<<0)

#define GLCP_DOWSER				(MSR_GLCP + 0x0E)
#define GLCP_DBGCLKCTL				(MSR_GLCP + 0x16)
#define GLCP_REVID				(MSR_GLCP + 0x17)
#define GLCP_TH_OD				(MSR_GLCP + 0x1E)
#define GLCP_FIFOCTL				(MSR_GLCP + 0x5E)
#define GLCP_BIST				GLCP_FIFOCTL
#define MSR_INIT				(MSR_GLCP + 0x33)
/*  GLIU1 port 4*/
#define GLPCI_GLD_MSR_CAP			(MSR_PCI + 0x2000)
#define GLPCI_GLD_MSR_CONFIG			(MSR_PCI + 0x2001)
#define GLPCI_GLD_MSR_PM			(MSR_PCI + 0x2004)
#define GLPCI_CTRL				(MSR_PCI + 0x2010)
#define GLPCI_CTRL_UPPER_FTH_SHIFT		28
#define GLPCI_CTRL_UPPER_RTH_SHIFT		24
#define GLPCI_CTRL_UPPER_SBRTH_SHIFT		20
#define GLPCI_CTRL_UPPER_RTL_SHIFT		17
#define GLPCI_CTRL_UPPER_DTL_SHIFT		14
#define GLPCI_CTRL_UPPER_WTO_SHIFT		11
#define GLPCI_CTRL_UPPER_SLTO_SHIFT		10
#define GLPCI_CTRL_UPPER_ILTO_SHIFT		8
#define GLPCI_CTRL_UPPER_LAT_SHIFT		3
#define GLPCI_CTRL_LOWER_IRFT_SHIFT		18
#define GLPCI_CTRL_LOWER_IRFC_SHIFT		16
#define GLPCI_CTRL_LOWER_ER_SET			(1<<11)
#define GLPCI_CTRL_LOWER_LDE_SET		(1<<9)
#define GLPCI_CTRL_LOWER_OWC_SET		(1<<4)
#define GLPCI_CTRL_LOWER_IWC_SET		(1<<3)
#define GLPCI_CTRL_LOWER_PCD_SET		(1<<2)
#define GLPCI_CTRL_LOWER_ME_SET			(1<<0)

#define GLPCI_ARB				(MSR_PCI + 0x2011)
#define GLPCI_ARB_UPPER_CR_SHIFT		28
#define GLPCI_ARB_UPPER_R2_SHIFT		24
#define GLPCI_ARB_UPPER_R1_SHIFT		20
#define GLPCI_ARB_UPPER_R0_SHIFT		16
#define GLPCI_ARB_UPPER_CH_SHIFT		12
#define GLPCI_ARB_UPPER_H2_SHIFT		8
#define GLPCI_ARB_UPPER_H1_SHIFT		4
#define GLPCI_ARB_UPPER_H0_SHIFT		0
#define GLPCI_ARB_LOWER_COV_SET			(1<<23)
#define GLPCI_ARB_LOWER_VO2_SET			(1 << 22)
#define GLPCI_ARB_LOWER_OV1_SET			(1 << 21)
#define GLPCI_ARB_LOWER_OV0_SET			(1 << 20)
#define GLPCI_ARB_LOWER_MSK2_SET		(1<<18)
#define GLPCI_ARB_LOWER_MSK1_SET		(1<<17)
#define GLPCI_ARB_LOWER_MSK0_SET		(1<<16)
#define GLPCI_ARB_LOWER_CPRE_SET		(1<<11)
#define GLPCI_ARB_LOWER_PRE2_SET		(1<<10)
#define GLPCI_ARB_LOWER_PRE1_SET		(1<<9)
#define GLPCI_ARB_LOWER_PRE0_SET		(1<<8)
#define GLPCI_ARB_LOWER_BM1_SET			(1<<7)
#define GLPCI_ARB_LOWER_BM0_SET			(1<<6)
#define GLPCI_ARB_LOWER_EA_SET			(1 << 2)
#define GLPCI_ARB_LOWER_BMD_SET			(1 << 1)
#define GLPCI_ARB_LOWER_PARK_SET		(1<<0)

#define GLPCI_REN				(MSR_PCI + 0x2014)
#define GLPCI_A0_BF				(MSR_PCI + 0x2015)
#define GLPCI_C0_DF				(MSR_PCI + 0x2016)
#define GLPCI_E0_FF				(MSR_PCI + 0x2017)
#define GLPCI_RC0				(MSR_PCI + 0x2018)
#define GLPCI_RC1				(MSR_PCI + 0x2019)
#define GLPCI_RC2				(MSR_PCI + 0x201A)
#define GLPCI_RC3				(MSR_PCI + 0x201B)
#define GLPCI_RC4				(MSR_PCI + 0x201C)
#define GLPCI_RC_UPPER_TOP_SHIFT				12
#define GLPCI_RC_LOWER_BASE_SHIFT		12
#define GLPCI_RC_LOWER_EN_SET			(1<<8)
#define GLPCI_RC_LOWER_PF_SET			(1<<5)
#define GLPCI_RC_LOWER_WC_SET			(1<<4)
#define GLPCI_RC_LOWER_WP_SET			(1<<2)
#define GLPCI_RC_LOWER_CD_SET			(1<<0)

#define GLPCI_ExtMSR				(MSR_PCI + 0x201E)
#define GLPCI_SPARE				(MSR_PCI + 0x201F)
#define GLPCI_SPARE_LOWER_AILTO_SET		(1<<6)
#define GLPCI_SPARE_LOWER_PPD_SET		(1<<5)
#define GLPCI_SPARE_LOWER_PPC_SET		(1<<4)
#define GLPCI_SPARE_LOWER_MPC_SET		(1<<3)
#define GLPCI_SPARE_LOWER_MME_SET		(1<<2)
#define GLPCI_SPARE_LOWER_NSE_SET		(1<<1)
#define GLPCI_SPARE_LOWER_SUPO_SET		(1<<0)

/*  VIP GLIU1 port 5*/
#define VIP_GLD_MSR_CAP				(MSR_VIP + 0x2000)
#define VIP_GLD_MSR_CONFIG			(MSR_VIP + 0x2001)
#define VIP_GLD_MSR_PM				(MSR_VIP + 0x2004)
#define VIP_BIST				(MSR_VIP + 0x2005)
/*  AES GLIU1 port 6*/
#define AES_GLD_MSR_CAP				(MSR_AES + 0x2000)
#define AES_GLD_MSR_CONFIG			(MSR_AES + 0x2001)
#define AES_GLD_MSR_PM				(MSR_AES + 0x2004)
#define AES_CONTROL				(MSR_AES + 0x2006)
/* from MC spec */
#define MIN_MOD_BANKS				1
#define MAX_MOD_BANKS				2
#define MIN_DEV_BANKS				2
#define MAX_DEV_BANKS				4
#define MAX_COL_ADDR				17
/* GLIU typedefs */
#define BM					1	/*  Base Mask - map power of 2 size aligned region */
#define BMO					2	/*  BM with an offset */
#define R					3	/*  Range - 4k range minimum */
#define RO					4	/*  R with offset */
#define SC					5	/*  Swiss 0xCeese - maps a 256K region in to 16K 0xcunks. Set W/R */
#define BMIO					6	/*  Base Mask IO */
#define SCIO					7	/*  Swiss 0xCeese IO */
#define SC_SHADOW				8	/*  Special marker for Shadow SC descriptors so setShadow proc is independant of CPU */
#define R_SYSMEM				9	/*  Special marker for SYSMEM R descriptors so GLIUInit proc is independant of CPU */
#define BMO_SMM					10	/*  Specail marker for SMM */
#define BM_SMM					11	/*  Specail marker for SMM */
#define BMO_DMM					12	/*  Specail marker for DMM */
#define BM_DMM					13	/*  Specail marker for DMM */
#define RO_FB					14	/*  special for Frame buffer. */
#define R_FB					15	/*  special for FB. */
#define OTHER					0x0FE	/*  Special marker for other */
#define GL_END					0x0FF	/*  end */
#define MSR_GL0					(GL1_GLIU0 << 29)
/* Platform stuff but unlikely to change */
/*  Set up desc addresses from 20 - 3f*/
/*  This is chip specific!*/
#define MSR_GLIU0_BASE1				(MSR_GLIU0 + 0x20)	/*  BM */
#define MSR_GLIU0_BASE2				(MSR_GLIU0 + 0x21)	/*  BM */
#define MSR_GLIU0_BASE3				(MSR_GLIU0 + 0x22)	/*  BM */
#define MSR_GLIU0_BASE4				(MSR_GLIU0 + 0x23)	/*  BM */
#define MSR_GLIU0_BASE5				(MSR_GLIU0 + 0x24)	/*  BM */
#define MSR_GLIU0_BASE6				(MSR_GLIU0 + 0x25)	/*  BM */
#define GLIU0_P2D_BMO_0				(MSR_GLIU0 + 0x26)
#define GLIU0_P2D_BMO_1				(MSR_GLIU0 + 0x27)
#define MSR_GLIU0_SMM				(GLIU0_P2D_BMO_0)
#define MSR_GLIU0_DMM				(GLIU0_P2D_BMO_1)
#define GLIU0_P2D_R				(MSR_GLIU0 + 0x28)
#define MSR_GLIU0_SYSMEM			(GLIU0_P2D_R)
#define GLIU0_P2D_RO_0				(MSR_GLIU0 + 0x29)
#define GLIU0_P2D_RO_1				(MSR_GLIU0 + 0x2A)
#define GLIU0_P2D_RO_2				(MSR_GLIU0 + 0x2B)
#define MSR_GLIU0_SHADOW			(MSR_GLIU0 + 0x2C)	/*  SCO should only be SC */
#define GLIU0_IOD_BM_0				(MSR_GLIU0 + 0xE0)
#define GLIU0_IOD_BM_1				(MSR_GLIU0 + 0xE1)
#define GLIU0_IOD_BM_2				(MSR_GLIU0 + 0xE2)
#define GLIU0_IOD_SC_0				(MSR_GLIU0 + 0xE3)
#define GLIU0_IOD_SC_1				(MSR_GLIU0 + 0xE4)
#define GLIU0_IOD_SC_2				(MSR_GLIU0 + 0xE5)
#define GLIU0_IOD_SC_3				(MSR_GLIU0 + 0xE6)
#define GLIU0_IOD_SC_4				(MSR_GLIU0 + 0xE7)
#define GLIU0_IOD_SC_5				(MSR_GLIU0 + 0xE8)
#define MSR_GLIU1_BASE1				(MSR_GLIU1 + 0x20)	/*  BM */
#define MSR_GLIU1_BASE2				(MSR_GLIU1 + 0x21)	/*  BM */
#define MSR_GLIU1_BASE3				(MSR_GLIU1 + 0x22)	/*  BM */
#define MSR_GLIU1_BASE4				(MSR_GLIU1 + 0x23)	/*  BM */
#define MSR_GLIU1_BASE5				(MSR_GLIU1 + 0x24)	/*  BM */
#define MSR_GLIU1_BASE6				(MSR_GLIU1 + 0x25)	/*  BM */
#define MSR_GLIU1_BASE7				(MSR_GLIU1 + 0x26)	/*  BM */
#define MSR_GLIU1_BASE8				(MSR_GLIU1 + 0x27)	/*  BM */
#define MSR_GLIU1_BASE9				(MSR_GLIU1 + 0x28)	/*  BM */
#define MSR_GLIU1_BASE10			(MSR_GLIU1 + 0x29)	/*  BM */
#define GLIU1_P2D_R_0				(MSR_GLIU1 + 0x2A)
#define GLIU1_P2D_R_1				(MSR_GLIU1 + 0x2B)
#define GLIU1_P2D_R_2				(MSR_GLIU1 + 0x2C)
#define GLIU1_P2D_R_3				(MSR_GLIU1 + 0x2D)
#define MSR_GLIU1_SHADOW			(MSR_GLIU1 + 0x2E)
#define MSR_GLIU1_SYSMEM			(GLIU1_P2D_R_0)
#define MSR_GLIU1_SMM				(MSR_GLIU1_BASE4)	/*  BM */
#define MSR_GLIU1_DMM				(MSR_GLIU1_BASE5)	/*  BM */
#define GLIU1_IOD_BM_0				(MSR_GLIU1 + 0xE0)
#define GLIU1_IOD_BM_1				(MSR_GLIU1 + 0xE1)
#define GLIU1_IOD_BM_2				(MSR_GLIU1 + 0xE2)
#define GLIU1_IOD_SC_0				(MSR_GLIU1 + 0xE3)
#define GLIU1_IOD_SC_1				(MSR_GLIU1 + 0xE4)
#define GLIU1_IOD_SC_2				(MSR_GLIU1 + 0xE5)
#define GLIU1_IOD_SC_3				(MSR_GLIU1 + 0xE6)
#define MSR_GLIU1_FPU_TRAP			(GLIU1_IOD_SC_0)	/*  FooGlue F0 for FPU */
/* ------------------------  */
#define SMM_OFFSET				0x80400000	/* above 2GB */
#define SMM_SIZE				128	/* changed SMM_SIZE from 256 KB to 128 KB */

/* ------------------------  */
#define DCACHE_RAM_SIZE CONFIG_CARSIZE
#define DCACHE_RAM_BASE CONFIG_CARBASE
/* This is where the DCache will be mapped and be used as stack. It would be
 * cool if it was the same base as coreboot normal stack.
 */
#define LX_STACK_BASE		DCACHE_RAM_BASE
#define LX_STACK_END		LX_STACK_BASE + (DCACHE_RAM_SIZE - 4)

#define LX_NUM_CACHELINES	0x080	/* There are 128 lines per way. */
#define LX_CACHELINE_SIZE	0x020	/* There are 32 bytes per line. */
#define LX_CACHEWAY_SIZE	(LX_NUM_CACHELINES * LX_CACHELINE_SIZE)
#define CR0_CD			0x40000000 /* Bit 30 = Cache Disable */
#define CR0_NW			0x20000000 /* Bit 29 = Not Write Through */

#define ROM_CODE_SEG		0x08
#define ROM_DATA_SEG		0x10

#define CACHE_RAM_CODE_SEG	0x18
#define CACHE_RAM_DATA_SEG	0x20

/* POST CODES */
/* standard AMD post definitions -- might as well use them. */

#define POST_Output_Port				(0x080)	/*	port to write post codes to*/

#define POST_preSioInit					(0x000)	/* geode.asm*/
#define POST_clockInit					(0x001)	/* geode.asm*/
#define POST_CPURegInit					(0x002)	/* geode.asm*/
#define POST_UNREAL						(0x003)	/* geode.asm*/
#define POST_CPUMemRegInit				(0x004)	/* geode.asm*/
#define POST_CPUTest					(0x005)	/* geode.asm*/
#define POST_memSetup					(0x006)	/* geode.asm*/
#define POST_memSetUpStack				(0x007)	/* geode.asm*/
#define POST_memTest					(0x008)	/* geode.asm*/
#define POST_shadowRom					(0x009)	/* geode.asm*/
#define POST_memRAMoptimize				(0x00A)	/* geode.asm*/
#define POST_cacheInit					(0x00B)	/* geode.asm*/
#define POST_northBridgeInit			(0x00C)	/* geode.asm*/
#define POST_chipsetInit				(0x00D)	/* geode.asm*/
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
#define POST_summary_screen				(0x01A)	/* geode.asm*/
#define POST_Boot						(0x01B)	/* geode.asm*/
#define POST_SystemPreInit				(0x01C)	/* geode.asm*/
#define POST_ClearRebootFlag			(0x01D)	/* geode.asm*/
#define POST_GLIUInit					(0x01E)	/* geode.asm*/
#define POST_BootFailed					(0x01F)	/* geode.asm*/


#define POST_CPU_ID						(0x020)	/* cpucpuid.asm*/
#define POST_COUNTERBROKEN				(0x021)	/* pllinit.asm*/
#define POST_DIFF_DIMMS					(0x022)	/* pllinit.asm*/
#define POST_WIGGLE_MEM_LINES			(0x023)	/* pllinit.asm*/
#define POST_NO_GLIU_DESC				(0x024)	/* pllinit.asm*/
#define POST_CPU_LCD_CHECK				(0x025)	/* pllinit.asm*/
#define POST_CPU_LCD_PASS				(0x026)	/* pllinit.asm*/
#define POST_CPU_LCD_FAIL				(0x027)	/* pllinit.asm*/
#define POST_CPU_STEPPING				(0x028)	/* cpucpuid.asm*/
#define POST_CPU_DM_BIST_FAILURE		(0x029)	/* gx2reg.asm*/
#define POST_CPU_FLAGS					(0x02A)	/* cpucpuid.asm*/
#define POST_CHIPSET_ID					(0x02b)	/* chipset.asm*/
#define POST_CHIPSET_ID_PASS			(0x02c)	/* chipset.asm*/
#define POST_CHIPSET_ID_FAIL			(0x02d)	/* chipset.asm*/
#define POST_CPU_ID_GOOD				(0x02E)	/* cpucpuid.asm*/
#define POST_CPU_ID_FAIL				(0x02F)	/* cpucpuid.asm*/



/*	PCI config*/
#define P80_PCICFG					(0x030)	/*	pcispace.asm*/


/*	PCI io*/
#define P80_PCIIO					(0x040)	/*	pcispace.asm*/


/*	PCI memory*/
#define P80_PCIMEM					(0x050)	/*	pcispace.asm*/


/*	SIO*/
#define P80_SIO						(0x060)		/*	*sio.asm*/

/*	Memory Setp*/
#define P80_MEM_SETUP				(0x070)	/* docboot meminit*/
#define POST_MEM_SETUP				(0x070)	/* memsize.asm*/
#define ERROR_32BIT_DIMMS			(0x071)	/* memsize.asm*/
#define POST_MEM_SETUP2				(0x072)	/* memsize.asm*/
#define POST_MEM_SETUP3				(0x073)	/* memsize.asm*/
#define POST_MEM_SETUP4				(0x074)	/* memsize.asm*/
#define POST_MEM_SETUP5				(0x075)	/* memsize.asm*/
#define POST_MEM_ENABLE				(0x076)	/* memsize.asm*/
#define ERROR_NO_DIMMS				(0x077)	/* memsize.asm*/
#define ERROR_DIFF_DIMMS			(0x078)	/* memsize.asm*/
#define ERROR_BAD_LATENCY			(0x079)	/* memsize.asm*/
#define ERROR_SET_PAGE				(0x07a)	/* memsize.asm*/
#define ERROR_DENSITY_DIMM			(0x07b)	/* memsize.asm*/
#define ERROR_UNSUPPORTED_DIMM		(0x07c)	/* memsize.asm*/
#define ERROR_BANK_SET				(0x07d)	/* memsize.asm*/
#define POST_MEM_SETUP_GOOD			(0x07E)	/* memsize.asm*/
#define POST_MEM_SETUP_FAIL			(0x07F)	/* memsize.asm*/


#define POST_UserPreInit				(0x080)	/* geode.asm*/
#define POST_UserPostInit				(0x081)	/* geode.asm*/
#define POST_Equipment_check			(0x082)	/* geode.asm*/
#define POST_InitNVRAMBX				(0x083)	/* geode.asm*/
#define POST_NoPIRTable					(0x084)	/* pci.asm*/
#define POST_ChipsetFingerPrintPass		(0x085)	/*	prechipsetinit*/
#define POST_ChipsetFingerPrintFail		(0x086)	/*	prechipsetinit*/
#define POST_CPU_IM_TAG_BIST_FAILURE	(0x087)	/*	gx2reg.asm*/
#define POST_CPU_IM_DATA_BIST_FAILURE	(0x088)	/*	gx2reg.asm*/
#define POST_CPU_FPU_BIST_FAILURE		(0x089)	/*	gx2reg.asm*/
#define POST_CPU_BTB_BIST_FAILURE		(0x08a)	/*	gx2reg.asm*/
#define POST_CPU_EX_BIST_FAILURE		(0x08b)	/*	gx2reg.asm*/
#define POST_Chipset_PI_Test_Fail			(0x08c)	/*	prechipsetinit*/
#define POST_Chipset_SMBus_SDA_Test_Fail	(0x08d)	/*	prechipsetinit*/
#define POST_BIT_CLK_Fail				(0x08e)	/*	Hawk geode.asm override*/


#define POST_STACK_SETUP				(0x090)	/* memstack.asm*/
#define POST_CPU_PF_BIST_FAILURE		(0x091)	/*	gx2reg.asm*/
#define POST_CPU_L2_BIST_FAILURE		(0x092)	/*	gx2reg.asm*/
#define POST_CPU_GLCP_BIST_FAILURE		(0x093)	/*	gx2reg.asm*/
#define POST_CPU_DF_BIST_FAILURE		(0x094)	/*	gx2reg.asm*/
#define POST_CPU_VG_BIST_FAILURE		(0x095)	/*	gx2reg.asm*/
#define POST_CPU_VIP_BIST_FAILURE		(0x096)	/*	gx2reg.asm*/
#define POST_STACK_SETUP_PASS			(0x09E)	/* memstack.asm*/
#define POST_STACK_SETUP_FAIL			(0x09F)	/* memstack.asm*/


#define POST_PLL_INIT					(0x0A0)	/* pllinit.asm*/
#define POST_PLL_MANUAL					(0x0A1)	/* pllinit.asm*/
#define POST_PLL_STRAP					(0x0A2)	/* pllinit.asm*/
#define POST_PLL_RESET_FAIL				(0x0A3)	/* pllinit.asm*/
#define POST_PLL_PCI_FAIL				(0x0A4)	/* pllinit.asm*/
#define POST_PLL_MEM_FAIL				(0x0A5)	/* pllinit.asm*/
#define POST_PLL_CPU_VER_FAIL			(0x0A6)	/* pllinit.asm*/


#define POST_MEM_TESTMEM			(0x0B0)	/* memtest.asm*/
#define POST_MEM_TESTMEM1			(0x0B1)	/* memtest.asm*/
#define POST_MEM_TESTMEM2			(0x0B2)	/* memtest.asm*/
#define POST_MEM_TESTMEM3			(0x0B3)	/* memtest.asm*/
#define POST_MEM_TESTMEM4			(0x0B4)	/* memtest.asm*/
#define POST_MEM_TESTMEM_PASS		(0x0BE)	/* memtest.asm*/
#define POST_MEM_TESTMEM_FAIL		(0x0BF)	/* memtest.asm*/


#define POST_SECUROM_SECBOOT_START		(0x0C0)	/* secstart.asm*/
#define POST_SECUROM_BOOTSRCSETUP		(0x0C1)	/* secstart.asm*/
#define POST_SECUROM_REMAP_FAIL			(0x0C2)	/* secstart.asm*/
#define POST_SECUROM_BOOTSRCSETUP_FAIL	(0x0C3)	/* secstart.asm*/
#define POST_SECUROM_DCACHESETUP		(0x0C4)	/* secstart.asm*/
#define POST_SECUROM_DCACHESETUP_FAIL	(0x0C5)	/* secstart.asm*/
#define POST_SECUROM_ICACHESETUP		(0x0C6)	/* secstart.asm*/
#define POST_SECUROM_DESCRIPTORSETUP	(0x0C7)	/* secstart.asm*/
#define POST_SECUROM_DCACHESETUPBIOS	(0x0C8)	/* secstart.asm*/
#define POST_SECUROM_PLATFORMSETUP		(0x0C9)	/* secstart.asm*/
#define POST_SECUROM_SIGCHECKBIOS		(0x0CA)	/* secstart.asm*/
#define POST_SECUROM_ICACHESETUPBIOS	(0x0CB)	/* secstart.asm*/
#define POST_SECUROM_PASS				(0x0CC)	/* secstart.asm*/
#define POST_SECUROM_FAIL				(0x0CD)	/* secstart.asm*/

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


/* VR values */

#define VRC_INDEX				0xAC1C	// Index register 
#define VRC_DATA				0xAC1E	// Data register
#define VR_UNLOCK				0xFC53	// Virtual register unlock code
#define NO_VR					-1		// No virtual registers

#define	VRC_MISCELLANEOUS		0x00	// Miscellaneous Class
    #define VSA_VERSION_NUM     0x00
	#define HIGH_MEM_ACCESS		0x01
    #define GET_VSM_INFO        0x02	// Used by INFO
       #define GET_BASICS       0x00
       #define GET_EVENT        0x01
       #define GET_STATISTICS   0x02
       #define GET_HISTORY      0x03
       #define GET_HARDWARE     0x04
       #define GET_ERROR        0x05
       #define SET_VSM_TYPE     0x06
	#define SIGNATURE			0x03
       #define VSA2_SIGNATURE	0x56534132	// 'VSA2' returned in EAX 

    #define GET_HW_INFO			0x04
    #define VSM_VERSION			0x05
	#define CTRL_ALT_DEL		0x06
    #define MSR_ACCESS          0x07
    #define GET_DESCR_INFO		0x08
    #define PCI_INT_AB			0x09	// GPIO pins for INTA# and INTB#
    #define PCI_INT_CD			0x0A	// GPIO pins for INTC# and INTD#	
    #define WATCHDOG			0x0B	// Watchdog timer

    #define MAX_MISC           	WATCHDOG


// NOTE:  Do not change the order of the following registers:
#define	VRC_AUDIO      			0x01	// XpressAudio Class
	#define AUDIO_VERSION      	0x00
	#define PM_STATE        	0x01
	#define SB_16_IO_BASE   	0x02
	#define MIDI_BASE       	0x03
	#define CPU_USAGE       	0x04
	#define CODEC_TYPE      	0x05
	#define STATE_INDEX     	0x06
	#define STATE_DATA      	0x07
	#define AUDIO_IRQ	      	0x08	// For use by native audio drivers 
	#define STATUS_PTR			0x09	// For use by native audio drivers
	#define MAX_AUDIO           STATUS_PTR

#define	VRC_VG					0x02	// SoftVG Class
#define	VRC_VGA					0x02	// SoftVGA Class
	#define	VG_MEM_SIZE			0x00	// bits 7:0 - 512K unit size, bit 8 controller priority
	#define	VG_CONFIG			0x00	// Main configuration register
		#define VG_CFG_BYPASS		0x0001	// DOTPLL bypass bit
		#define VG_MEM_MASK			0x00FE	// Memory size mask bits, 2MB increment
		#define VG_CFG_DSMASK		0x0700	// Active display mask bits
			#define VG_CFG_DSCRT		0x0000	// Active display is CRT
			#define VG_CFG_DSPAN		0x0100	// Active display is panel
			#define VG_CFG_DSTV			0x0200	// Active display is TV
			#define VG_CFG_DSSIM		0x0400	// Simultaneous CRT
		#define VG_CFG_PRIORITY		0x0800	// Controller priority bit
		#define VG_CFG_MONO			0x1000	// External monochrome card support bit
		#define VG_CFG_DRIVER		0x2000	// Driver active bit
		#define VG_CRTC_DIAG		0x8000	// Enable CRTC emulation

	// Defined for GX3/GX3VG
	#define	VG_REFRESH			0x01	// Mode refresh, a mode switch without changing modes
		#define VG_FRSH_REF_MASK	0xE000	// Refresh rate mask
		#define VG_FRSH_REF_GO		0x1000	// Refresh rate GO bit
			// Uses CFP_REF_xxx values from below
		#define VG_FRSH_BPP_MASK	0x0E00	// Color depth mask
		#define VG_FRSH_BPP_GO		0x0100	// Color depth GO bit
			#define FRSH_BPP_8RGB		0x0200	// 8 bits per pixel, RGB
			#define FRSH_BPP_16ARGB		0x0400	// 16BPP, ARGB (4:4:4:4)
			#define FRSH_BPP_15RGB		0x0600	// 15BPP, RGB (1:5:5:5)
			#define FRSH_BPP_16RGB		0x0800	// 16BPP, RGB (5:6:5)
			#define FRSH_BPP_24RGB		0x0A00	// 24BPP, RGB (0:8:8:8)
			#define FRSH_BPP_32ARGB		0x0C00	// 32BPP, ARGB (8:8:8:8)
		#define VG_CFG_DPMS			0x00C0	// DPMS mask bits
			#define VG_CFG_DPMS_H		0x0040	// HSYNC mask bit
			#define VG_CFG_DPMS_V		0x0080	// VSYNC mask bit
		#define VG_VESA_SV_RST		0x0020	// VESA Save/Restore state flag
			#define VG_VESA_RST			0x0000	// VESA Restore state
			#define VG_VESA_SV			0x0020	// VESA Save state 
		#define VG_FRSH_MODE		0x0002	// Mode refresh flag
		#define VG_FRSH_TIMINGS		0x0001	// Timings only refresh flag

	// Defined for GX2/SoftVG
	#define	VG_PLL_REF			0x01	// PLL reference frequency selection register
		#define	PLL_14MHZ			0x0000	// 14.31818MHz PLL reference frequency (Default)
		#define	PLL_48MHZ			0x0100	// 48MHz PLL reference frequency

	// Defined for GX1/SoftVGA
	#define	VGA_MEM_SIZE		0x01	// bits 7:1 - 128K unit size, bit 0 controller enable

	#define	VG_FP_TYPE			0x02	// Flat panel type data
	// VG_FP_TYPE definitions for GX2/SoftVG
		#define FP_TYPE_SSTN		0x0000	// SSTN panel type value
		#define FP_TYPE_DSTN		0x0001	// DSTN panel type value
		#define FP_TYPE_TFT			0x0002	// TFT panel type value
		#define FP_TYPE_LVDS		0x0003	// LVDS panel type value
		#define FP_RES_6X4			0x0000	// 640x480 resolution value
		#define FP_RES_8X6			0x0008	// 800x600 resolution value
		#define FP_RES_10X7			0x0010	// 1024x768 resolution value
		#define FP_RES_11X8			0x0018	// 1152x864 resolution value
		#define FP_RES_12X10		0x0020	// 1280x1024 resolution value
		#define FP_RES_16X12		0x0028	// 1600x1200 resolution value
		#define FP_WIDTH_8			0x0000	// 8 bit data bus width
		#define FP_WIDTH_9			0x0040	// 9 bit data bus width
		#define FP_WIDTH_12			0x0080	// 12 bit data bus width
		#define FP_WIDTH_18			0x00C0	// 18 bit data bus width
		#define FP_WIDTH_24			0x0100	// 24 bit data bus width
		#define FP_WIDTH_16			0x0140	// 16 bit data bus width - 16 bit Mono DSTN only
		#define FP_COLOR_COLOR		0x0000	// Color panel
		#define FP_COLOR_MONO		0x0200	// Mono Panel
		#define FP_PPC_1PPC			0x0000	// One pixel per clock
		#define FP_PPC_2PPC			0x0400	// Two pixels per clock
		#define	FP_H_POL_LGH		0x0000	// HSync at panel, normally low, active high
		#define FP_H_POL_HGL		0x0800	// HSync at panel, normally high, active low
		#define FP_V_POL_LGH		0x0000	// VSync at panel, normally low, active high
		#define FP_V_POL_HGL		0x1000	// VSync at panel, normally high, active low
		#define FP_REF_60			0x0000	// 60Hz refresh rate
		#define FP_REF_65			0x2000	// 65Hz refresh rate
		#define FP_REF_70			0x4000	// 70Hz refresh rate
		#define FP_REF_72			0x6000	// 72Hz refresh rate
		#define FP_REF_75			0x8000	// 75Hz refresh rate
		#define FP_REF_85			0xA000	// 85Hz refresh rate

	// VG_FP_TYPE definitions for GX3/GX3VG
		#define FP_TYPE_TYPE	0x0003		// Flat panel type bits mask
			#define CFP_TYPE_TFT	0x0000		// TFT panel type value
			#define CFP_TYPE_LVDS	0x0001		// LVDS panel type value
		#define FP_TYPE_RES		0x0038		// Panel resolution bits mask
			#define CFP_RES_3X2		0x0000		// 320x240 resolution value
			#define CFP_RES_6X4		0x0008		// 640x480 resolution value
			#define CFP_RES_8X6		0x0010		// 800x600 resolution value
			#define CFP_RES_10X7	0x0018		// 1024x768 resolution value
			#define CFP_RES_11X8	0x0020		// 1152x864 resolution value
			#define CFP_RES_12X10	0x0028		// 1280x1024 resolution value
			#define CFP_RES_16X12	0x0030		// 1600x1200 resolution value
		#define FP_TYPE_BUS		0x00C0		// Data bus width and pixels/clock mask
			#define CFP_BUS_1PPC	0x0040		// 9, 12, 18 or 24 bit data bus, 1 pixel per clock
			#define CFP_BUS_2PPC	0x0080		// 18 or 24 bit data bus, 2 pixels per clock
		#define FP_TYPE_HPOL	0x0800		// HSYNC polarity into the panel
			#define CFP_HPOL_HGL	0x0000	// HSync at panel, normally high, active low
			#define	CFP_HPOL_LGH	0x0800	// HSync at panel, normally low, active high
		#define FP_TYPE_VPOL	0x1000		// VSYNC polarity into the panel
			#define CFP_VPOL_HGL	0x0000	// VSync at panel, normally high, active low
			#define CFP_VPOL_LGH	0x1000	// VSync at panel, normally low, active high
		#define FP_TYPE_REF		0xE000		// Panel refresh rate
			#define CFP_REF_60		0x0000	// 60Hz refresh rate
			#define CFP_REF_70		0x2000	// 70Hz refresh rate
			#define CFP_REF_75		0x4000	// 75Hz refresh rate
			#define CFP_REF_85		0x6000	// 85Hz refresh rate
			#define CFP_REF_100		0x8000	// 100Hz refresh rate

	#define	VG_FP_OPTION		0x03	// Flat panel option data
		#define FP_OPT_SCLK_NORMAL		0x0000		// SHFTClk not inverted to panel
		#define FP_OPT_SCLK_INVERTED	0x0010		// SHFTClk inverted to panel
		#define FP_OPT_SCLK_ACT_ACTIVE	0x0000		// SHFTClk active during "active" only
		#define FP_OPT_SCLK_ACT_FREE	0x0020		// SHFTClk free-running
		#define FP_OPT_LP_ACT_FREE		0x0000		// LP free-running
		#define FP_OPT_LP_ACT_ACTIVE	0x0040		// LP active during "active" only
		#define FP_OPT_LDE_POL_LGH		0x0000		// LDE/MOD, normally low, active high
		#define FP_OPT_LDE_POL_HGL		0x0080		// LDE/MOD, normally high, active low
		#define FP_OPT_PWR_DLY_32MS		0x0000		// 32MS delay for each step of pwr seq.
		#define FP_OPT_PWR_DLY_128MS	0x0100		// 128MS delay for each step of pwr seq.

	#define	VG_TV_CONFIG		0x04	// TV configuration register
		#define VG_TV_ENC			0x000F	// TV encoder select mask
			#define VG_TV_ADV7171		0x0000	// ADV7171 Encoder
			#define VG_TV_SAA7127		0x0001	// ADV7171 Encoder
			#define VG_TV_ADV7300		0x0002	// ADV7300 Encoder
			#define VG_TV_FS454			0x0003	// FS454 Encoder
		#define VG_TV_FMT			0x0070	// TV encoder output format mask
			#define VG_TV_FMT_SHIFT		0x0004	// Right shift value
			#define VG_TV_NTSC			0x0000	// NTSC output format
			#define VG_TV_PAL			0x0010	// PAL output format
			#define VG_TV_HDTV			0x0020	// HDTV output format

		// The meaning of the VG_TV_RES field is dependent on the selected  
		// encoder and output format.  The translations are:
		//		ADV7171 - Not Used
		//		SAA7127 - Not Used
		//		ADV7300 - HDTV resolutions only
		//			LO  -> 720x480p
		//	   		MED -> 1280x720p
		//			HI  -> 1920x1080i
		// 		FS454   - Both SD and HD resolutions	
		// 			SD Resolutions - NTSC and PAL
		//				LO  -> 640x480
		//	   			MED -> 800x600
		//				HI  -> 1024x768
		// 			HD Resolutions
		//				LO  -> 720x480p
		//	   			MED -> 1280x720p
		//				HI  -> 1920x1080i
		#define VG_TV_RES			0x0780	// TV resolution select mask
			#define VG_TV_RES_SHIFT		0x0007	// Right shift value
			#define VG_TV_RES_LO		0x0000	// Low resolution
			#define VG_TV_RES_MED		0x0080	// Medium resolution
			#define VG_TV_RES_HI 		0x0100	// High resolution
		#define VG_TV_PASSTHRU		0x0800	// TV passthru mode

	#define	VG_TV_SCALE_ADJ		0x05	// Modifies scaling factors for TV resolutions
		#define VG_TV_HACT_ADJ		0x00FF		// Horizontal active scale adjust value mask
		#define VG_TV_VACT_ADJ		0xFF00		// Vertical active scale adjust value mask

	#define	VG_DEBUG			0x0F	// A debug register

	#define	VG_FT_HTOT			0x10	// Fixed timings, horizontal total
	#define	VG_FT_HACT			0x11	// Fixed timings, horizontal active
	#define	VG_FT_HBST			0x12	// Fixed timings, horizontal blank start
	#define	VG_FT_HBND			0x13	// Fixed timings, horizontal blank end
	#define	VG_FT_HSST			0x14	// Fixed timings, horizontal sync start
	#define	VG_FT_HSND			0x15	// Fixed timings, horizontal sync end

	#define	VG_FT_VTOT			0x16	// Fixed timings, vertical total
	#define	VG_FT_VACT			0x17	// Fixed timings, vertical active
	#define	VG_FT_VBST			0x18	// Fixed timings, vertical blank start
	#define	VG_FT_VBND			0x19	// Fixed timings, vertical blank end
	#define	VG_FT_VSST			0x1A	// Fixed timings, vertical sync start
	#define	VG_FT_VSND			0x1B	// Fixed timings, vertical sync end

	#define	VG_START_OFFS_LO	0x20	// Framebuffer start offset bits 15:0
	#define	VG_START_OFFS_HI	0x21	// Framebuffer start offset bits 27:16

	#define	VG_FT_VEACT			0x28	// Fixed timings, vertical active
	#define	VG_FT_VETOT			0x29	// Fixed timings, vertical total
	#define	VG_FT_VEBST			0x2A	// Fixed timings, vertical blank start
	#define	VG_FT_VEBND			0x2B	// Fixed timings, vertical blank end
	#define	VG_FT_VESST			0x2C	// Fixed timings, vertical sync start
	#define	VG_FT_VESND			0x2D	// Fixed timings, vertical sync end

	#define MAX_VGA           	VGA_MEM_SIZE
//	#define MAX_VG           	VG_FP_OPTION
//	#define MAX_VG           	VG_START_OFFS_HI
	#define MAX_VG           	VG_FT_VESND

#define	VRC_APM					0x03
    #define REPORT_EVENT		0x00
    #define CAPABILITIES    	0x01
	#define APM_PRESENT		0x02
	#define MAX_APM           	APM_PRESENT


#define	VRC_PM					0x04	// Legacy PM Class
	#define	POWER_MODE			0x00
	#define POWER_STATE			0x01
	#define	DOZE_TIMEOUT		0x02
	#define	STANDBY_TIMEOUT	   	0x03
	#define	SUSPEND_TIMEOUT	   	0x04
    #define PS2_TIMEOUT		    0x05
	#define RESUME_ON_RING		0x06
	#define VIDEO_TIMEOUT		0x07
	#define	DISK_TIMEOUT		0x08
	#define	FLOPPY_TIMEOUT	   	0x09
	#define	SERIAL_TIMEOUT	    0x0A
	#define	PARALLEL_TIMEOUT	0x0B
	#define IRQ_WAKEUP_MASK	   	0x0C
//	#define SUSPEND_MODULATION 	0x0D
	#define SLEEP_PIN			0x0E
	#define SLEEP_PIN_ATTR		0x0F
//	#define SMI_WAKEUP_MASK		0x10
	#define INACTIVITY_CONTROL	0x11
	#define PM_S1_CLOCKS		0x12
		#define S1_CLOCKS_ON		0x00
		#define S1_CLOCKS_OFF		0x01
//	#define PM_S2_CLOCKS		0x13
//	#define PM_S3_CLOCKS		0x14
//	#define PM_S4_CLOCKS		0x15
//	#define PM_S5_CLOCKS		0x16
    #define PM_S0_LED           0x17
    #define PM_S1_LED           0x18
    #define PM_S2_LED           0x19
    #define PM_S3_LED           0x1A
    #define PM_S4_LED           0x1B
    #define PM_S5_LED           0x1C
	#define PM_LED_GPIO			0x1D
    #define PM_IMM_LED			0x1E
    #define PM_PWR_LEDS			0x1F
	    #define MB_LED0				0x01
	    #define MB_LED1				0x02
	    #define MB_LED2				0x04
	    #define MB_LED3				0x08
	    #define SIO_LED0			0x10
	    #define SIO_LED1			0x20
	    #define SIO_LED2			0x40
	    #define SIO_LED3			0x80
	#define PM_PME_MASK			0x20
    #define MAX_PM              PM_PME_MASK


#define VRC_INFRARED			0x05
	#define MAX_INFRARED        NO_VR

#define	VRC_TV					0x06	// TV Encoder Class
	#define TV_ENCODER_TYPE		0x00
	#define TV_CALLBACK_MASK	0x01
	#define TV_MODE				0x02
	#define TV_POSITION			0x03
	#define TV_BRIGHTNESS		0x04
	#define TV_CONTRAST			0x05
	#define TV_OUTPUT			0x06
	#define TV_TIMING			0x10	// 0x10...0x1D are all timings
	#define MAX_TV           	TV_TIMING



#define VRC_EXTERNAL_AMP		0x07
    #define EAPD_VERSION        0x00
    #define AMP_POWER          	0x01
	   #define AMP_OFF          0x00
	   #define AMP_ON           0x01
	#define AMP_TYPE            0x02
	#define MAX_EXTERNAL_AMP    	AMP_TYPE


#define	VRC_ACPI				0x08
    #define	ENABLE_ACPI			0x00	// Enable ACPI Mode
	#define SCI_IRQ			   	0x01	// Set the IRQ the SCI is mapped to, sysbios use.
	#define ACPINVS_LO			0x02	// new calls to send 32bit physAddress of
	#define ACPINVS_HI		  	0x03	// ACPI NVS region to VSA
	#define GLOBAL_LOCK			0x04	// read requests semaphore, write clears
	#define ACPI_UNUSED1		0x05
	#define RW_PIRQ				0x06	// read/write PCI IRQ router regs in SB Func0 cfg space
	#define SLPB_CLEAR			0x07	// clear sleep button GPIO status's
	#define PIRQ_ROUTING		0x08	// read the PCI IRQ routing based on BIOS setup
	#define ACPI_UNUSED2		0x09	
	#define ACPI_UNUSED3		0x0A	
	#define PIC_INTERRUPT		0x0B
	#define ACPI_PRESENT		0x0C
	#define	ACPI_GEN_COMMAND	0x0D
	#define	ACPI_GEN_PARAM1		0x0E
	#define	ACPI_GEN_PARAM2		0x0F
	#define	ACPI_GEN_PARAM3		0x10
	#define	ACPI_GEN_RETVAL		0x11
	#define MAX_ACPI	        ACPI_GEN_RETVAL

#define	VRC_ACPI_OEM			0x09
	#define MAX_ACPI_OEM        NO_VR

#define VRC_POWER               0x0A
    #define BATTERY_UNITS       0x00	// No. battery units
    #define BATTERY_SELECT      0x01
    #define AC_STATUS           0x02
	#define BATTERY_STATUS		0x03
	#define BATTERY_FLAG		0x04
	#define BATTERY_PERCENTAGE	0x05
    #define BATTERY_TIME        0x06
	#define MAX_POWER           BATTERY_TIME



#define VRC_OHCI				0x0B	// OHCI Class
    #define SET_LED             0x00
    #define INIT_OHCI			0x01
    #define MAX_OHCI            INIT_OHCI

#define VRC_KEYBOARD            0x0C	// Kbd Controller Class
    #define KEYBOARD_PRESENT    0x00
    #define SCANCODE            0x01
    #define MOUSE_PRESENT		0x02
    #define MOUSE_BUTTONS       0x03
	#define MOUSE_XY            0x04
	#define MAX_KEYBOARD        MOUSE_XY


#define VRC_DDC     			0x0D	// Video DDC Class
	#define VRC_DDC_ENABLE 		0x00	// Enable/disable register
		#define DDC_DISABLE		0x00
		#define DDC_ENABLE 		0x01
	#define VRC_DDC_IO	  		0x01	// A non-zero value for safety
	#define MAX_DDC           	VRC_DDC_IO

#define VRC_DEBUGGER			0x0E
	#define MAX_DEBUGGER        NO_VR
						   

#define	VRC_STR					0x0F		// Virtual Register class
	#define RESTORE_ADDR		0x00		// Physical address of MSR restore table


#define	VRC_COP8				0x10		// Virtual Register class
	#define	VRC_HIB_ENABLE		0x00		// HIB enable/disable index
	#define		HIB_ENABLE		0x00		// HIB enable command
	#define		HIB_DISABLE		0x01		// HIB disable command
	#define	VRC_HIB_SEND		0x01		// Send packet to COP8
	#define	VRC_HIB_READUART	0x02		// Read byte from COP8 UART
	#define	VRC_HIB_VERSION		0x03		// Read COP8 version
	#define	VRC_HIB_SERIAL		0x04		// Read 8 byte serial number
	#define	VRC_HIB_USRBTN		0x05		// Read POST button pressed status
	#define MAX_COP8	        NO_VR

#define	VRC_OWL					0x11		// Virtual Register class
	#define	VRC_OWL_DAC			0x00		// DAC (Backlight) Control
	#define	VRC_OWL_GPIO 		0x01		// GPIO Control
	#define MAX_OWL				VRC_OWL_GPIO

#define	VRC_SYSINFO				0x12		// Virtual Register class
	#define	VRC_SI_VERSION				0x00		// Sysinfo VSM version
	#define	VRC_SI_CPU_MHZ				0x01	// CPU speed in MHZ 
	#define	VRC_SI_CHIPSET_BASE_LOW		0x02
	#define	VRC_SI_CHIPSET_BASE_HI		0x03
	#define	VRC_SI_CHIPSET_ID			0x04
	#define	VRC_SI_CHIPSET_REV			0x05
	#define	VRC_SI_CPU_ID				0x06
	#define	VRC_SI_CPU_REV				0x07
	#define	MAX_SYSINFO			VRC_SI_CPU_REV

#define VRC_SUPERIO				0x13
	#define VRC_SIO_CHIPID				0x00
	#define VRC_SIO_NUMLD				0x01
	#define VRC_SIO_FDCEN				0x02
	#define VRC_SIO_FDCIO				0x03
	#define VRC_SIO_FDCIRQ				0x04
	#define VRC_SIO_FDCDMA				0x05
	#define VRC_SIO_FDCCFG1				0x06
	#define VRC_SIO_FDCCFG2				0x07
	#define VRC_SIO_PP1EN				0x08
	#define VRC_SIO_PP1IO				0x09
	#define VRC_SIO_PP1IRQ				0x0A
	#define VRC_SIO_PP1DMA				0x0B
	#define VRC_SIO_PP1CFG1				0x0C
	#define VRC_SIO_SP1EN				0x0D
	#define VRC_SIO_SP1IO				0x0E
	#define VRC_SIO_SP1IRQ				0x0F
	#define VRC_SIO_SP1CFG1				0x10
	#define VRC_SIO_SP2EN				0x11
	#define VRC_SIO_SP2IO				0x12
	#define VRC_SIO_SP2IRQ				0x13
	#define VRC_SIO_SP2CFG1				0x14
	#define VRC_SIO_KBEN				0x15
	#define VRC_SIO_KBIO1				0x16
	#define VRC_SIO_KBIO2				0x17
	#define VRC_SIO_KBIRQ				0x18
	#define VRC_SIO_KBCFG1				0x19
	#define VRC_SIO_MSEN				0x1A
	#define VRC_SIO_MSIO				0x1B
	#define VRC_SIO_MSIRQ				0x1C
	#define VRC_SIO_RTCEN				0x1D
	#define VRC_SIO_RTCIO1				0x1E
	#define VRC_SIO_RTCIO2				0x1F
	#define VRC_SIO_RTCIRQ				0x20
	#define VRC_SIO_RTCCFG1				0x21
	#define VRC_SIO_RTCCFG2				0x22
	#define VRC_SIO_RTCCFG3				0x23
	#define VRC_SIO_RTCCFG4				0x24
	#define MAX_SUPERIO			VRC_SIO_RTCCFG4

#define VRC_CHIPSET				0x14
	#define VRC_CS_PWRBTN		0x00
	#define VRC_CS_UART1		0x01
	#define VRC_CS_UART2		0x02
	#define MAX_CHIPSET			VRC_CS_UART2

#define VRC_THERMAL     0x15
    #define VRC_THERMAL_CURR_RTEMP      0x00        // read only
    #define VRC_THERMAL_CURR_LTEMP      0x01        // read only
    #define VRC_THERMAL_FAN             0x02
    #define VRC_THERMAL_LOW_THRESHOLD   0x03
    #define VRC_THERMAL_HIGH_THRESHOLD  0x04
    #define VRC_THERMAL_INDEX           0x05
    #define VRC_THERMAL_DATA            0x06
    #define VRC_THERMAL_SMB_ADDRESS     0x07
    #define VRC_THERMAL_SMB_INDEX       0x08
    #define VRC_THERMAL_SMB_DATA        0x09
    #define MAX_THERMAL         VRC_THERMAL_SMB_DATA

#define MAX_VR_CLASS	  		VRC_THERMAL

#ifndef __ASSEMBLER__

/* This is new. 
 * We're not using it yet on Geode. 
 * K8 requires it and, for future ports, we are going to require it. 
 * it's a useful placeholder for platform info that usually ends up 
 * scattered everywhere. On K8, it is initially stored at the base of stack
 * in cache-as-ram and then copied out once ram is started. 
 */
struct sys_info {
	int empty;
};


/*
 * Write to a Virtual Register
 * @param class_index The register index
 * @param data The data to write
 */
static inline void vr_write(u16 class_index, u16 data)
{
	outl(((u32) VR_UNLOCK << 16) | class_index, VRC_INDEX);
	outw(data, VRC_DATA);
}

 /*
  * Read from a Virtual Register
  * @param class_index The register index
  * @return the 16-bit word of data
  */
static inline u16 vr_read(u16 class_index)
{
	u16 data;
	outl(((u32) VR_UNLOCK << 16) | class_index, VRC_INDEX);
	data = inw(VRC_DATA);
	return data;
}

/* support functions */
u32 geode_link_speed(void);
void geodelx_msr_init(void);
void pll_reset(int manualconf, u32 pll_hi, u32 pll_lo);
void cpu_reg_init(int debug_clock_disable, u8 dimm0, u8 dimm1, int terminated);
void system_preinit(void);
void msr_init(void);
void geode_pre_payload(void);
#endif

#endif
