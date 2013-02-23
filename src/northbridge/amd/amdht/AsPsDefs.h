/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */


#ifndef ASPSDEFS_H
#define ASPSDEFS_H

#define APIC_BAR 0x1b			/* APIC_BAR register */
#define APIC_BAR_BP 0x100		/* APIC_BAR BSP bit */

#define PS_LIM_REG 0xC0010061		/* P-state Current Limit Register */
#define PS_MAX_VAL_SHFT 4		/* P-state Maximum Value shift position */

#define PS_CTL_REG 0xC0010062		/* P-state Control Register */
#define PS_CMD_MASK_OFF 0xfffffff8	/* P-state Control Register CMD Mask OFF */

#define PS_STS_REG 0xC0010063		/* P-state Status Register */
#define PS_STS_MASK 0x7		/* P-state Status Mask */

#define PS_REG_BASE 0xC0010064		/* P-state Register base */
#define PS_MAX_REG 0xC0010068		/* Maximum P-State Register */
#define PS_MIN_REG 0xC0010064		/* Mimimum P-State Register */

/* P-state register offset */
#define PS_REG0 0			/* offset for P0 */
#define PS_REG1 1			/* offset for P1 */
#define PS_REG2 2			/* offset for P2 */
#define PS_REG3 3			/* offset for P3 */
#define PS_REG4 4			/* offset for P4 */

#define PS_IDD_VALUE_SHFT 0            /* IddValue: current value
					  field offset for msr.hi */
#define PS_IDD_VALUE_MASK 0xFF         /* IddValue: current value
					  field mask for msr.hi */
#define PS_PSDIS_MASK	0x7fffffff	/* disable P-state register */
#define PS_EN_MASK  0x80000000		/* P-state register enable mask */
#define PS_NB_DID_MASK 0x400000	/* P-state Reg[NbDid] Mask */
#define PS_NB_VID_M_OFF 0x01ffffff	/* P-state Reg[NbVid] Mask OFF */
#define PS_CPU_VID_M_ON 0x0fe00	/* P-state Reg[CpuVid] Mask On */
#define PS_NB_VID_M_ON 0x0fe000000	/* P-state Reg[NbVid] Mask On */
#define PS_CPU_VID_SHFT 9		/* P-state bit shift for CpuVid */
#define PS_NB_VID_SHFT 25		/* P-state bit shift for NbVid */
#define PS_BOTH_VID_OFF 0x01ff01ff	/* Mask NbVid & CpuVid */
#define PS_CPU_NB_VID_SHFT 16		/* P-state bit shift from CpuVid to NbVid */
#define PS_DIS 0x7fffffff		/* disable P-state reg */
#define PS_EN 0x80000000		/* enable P-state reg */
#define PS_CPU_FID_MASK 0x03f           /* MSRC001_00[68:64][CpuFid]
					   Core Frequency Id */
#define PS_CURDIV_SHFT 8		/* P-state Current Divisor shift position */
#define PS_CPUDID_SHIFT 6		/* P-state CPU DID shift position */

/* for unfused parts */
#define PS_NB_VID_110V 0x48000000
#define PS_NB_VID_1175V 0x3c000000
/* NB VID 1.100V =0x12[PVI]=0x24[SVI] = 0100100b 7-bit code */

#define PS_NB_DID0 0			/* NB DID 0 */
#define PS_NB_DID1 0x400000		/* NB DID 1 */
#define PS_CPU_VID_110V 0x4800		/* CPU VID 1.100V */
#define PS_CPU_VID_1175V 0x3c00	/* CPU VID 1.175V */
#define PS_CPU_DID 0x40		/* CPU DID 1 = divisor of 2 */
#define PS_CPU_DID0 0			/* CPU DID 0 = divisor of 1 */
#define PS_CPU_FID_16G 0x00		/* CPU FID of 00 = 1.6GHz */
#define PS_CPU_FID_16G1 0x10		/* CPU FId of 16 COF = 16+16/2 = 16 */
#define PS_CPU_FID_18G 20		/* CPU FId of 20 COF = 20+16/2 = 18 */
#define PS_CPU_FID_19G 22		/* CPU FId of 20 COF = 22+16/2 = 19 */
#define PS_CPU_FID_20G 24		/* CPU FId of 20 COF = 24+16/2 = 20 */
#define PS_CPU_FID_22G 28		/* CPU FId of 2C COF = 28+16/2 = 22 */
#define PS_CPU_FID_30G 44		/* CPU FId of 2C COF = 44+16/2 = 30 */



#define PCI_DEV_BASE 24		/* System PCI device ID base */
#define LOCAL_APIC_ID_SHIFT 24		/* Local APIC ID shift bit # */
#define APIC_CID_SIZE_SHIFT 12		/* ApicCoreIdSize shift bit # */
#define FN_0 0				/* Function 0 */
#define FN_1 1				/* Function 1 */
#define FN_2 2				/* Function 2 */
#define FN_3 3				/* Function 3 */
#define FN_4 4				/* Function 4 */
#define FN_5 5				/* Function 5 */
#define FN_80000000 0x80000000		/* Function 8000_0000 */
#define FN_80000001 0x80000001		/* Function 8000_0001 */
#define FN_80000008 0x80000008		/* Function 8000_0008 */

#define LNK_INIT_REG 0x6C		/* F0x6C link initialization control register */
#define WARM_RESET_BIT 0x10		/* bit 4 =1 : warm reset */

#define HTC_REG 0x64			/* hardware thermal control reg */
#define HTC_PS_LMT_MASK 0x8fffffff	/* HtcPstateLimit mask off */
#define PS_LIMIT_POS 28		/* PstateLimit position for HTC & STC */

#define STC_REG 0x68			/* software thermal control reg */
#define STC_PS_LMT_MASK 0x8fffffff	/* StcPstateLimit mask off */

#define CPTC0 0x0d4			/* Clock Power/Timing Control0 Register*/
#define CPTC0_MASK 0x000cffff		/* Reset mask for this register */
#define CPTC0_NBFID_MASK 0xffffffe0	/* NbFid mask off for this register */
#define CPTC0_NBFID_MON 0x1f		/* NbFid mask on for this register */
#define NB_FID_EN 0x20			/* NbFidEn bit ON */
#define NB_CLKDID_ALL 0x80000000	/* NbClkDidApplyAll bit ON */
#define NB_CLKDID     0x40000000	/* NbClkDid value set by BIOS */
#define NB_CLKDID_SHIFT   28	        /* NbClkDid bit shift */
#define PW_STP_UP50   0x08000000	/* PowerStepUp 50nS(1000b) */
#define PW_STP_DN50   0x00800000	/* PowerStepDown 50nS (1000b)*/
#define PW_STP_UP100  0x03000000	/* PowerStepUp 100nS(0011b) */
#define PW_STP_DN100  0x00300000	/* PowerStepDown 100nS (0011b)*/
#define PW_STP_UP200  0x02000000	/* PowerStepUp 200nS(0010b) */
#define PW_STP_DN200  0x00200000	/* PowerStepDown 200nS (0010b)*/
#define PW_STP_UP400  0x00000000	/* PowerStepUp 400nS(0000b) */
#define PW_STP_DN400  0x00000000	/* PowerStepDown 400nS (0000b)*/
#define CLK_RAMP_HYST_SEL_VAL 0x00000f00 /* value mask for clock ramp
					    hysteresis select. BIOS
					    should program
					    F3xC4[ClkRampHystSel] to
					    1111b */


#define LNK_PLL_LOCK  0x00010000	/* LnkPllLock value set (01b) by BIOS */



#define PSTATE_CTL 0xC0010070		/* P-state Control Register */
#define NB_VID_POS 25			/* NbVid bit shift for position */
#define NB_VID_MASK_OFF 0x01ffffff	/* NbVid bits mask off */
#define NB_VID_MASK_ON 0xfe000000	/* NbVid bits mask on */
#define CPU_VID_POS 0x9		/* CpuVid bit shift for position */
#define CPU_VID_MASK_OFF 0xffff01ff	/* CpuVid bits mask off */
#define CPU_VID_MASK_ON 0x0000fe00	/* CpuVid bits mask on */
#define CPU_FID_DID_M_ON 0x000001ff	/* CpuFid & CpuDid mask on */
#define CPU_FID_DID_M_OFF 0xfffffe00	/* CpuFid & CpuDid mask off */
#define NB_DID_VID_M_ON 0xfe400000	/* NbDid & NbVid mask on */
#define NB_DID_M_ON 0x00400000		/* NbDid mask on */
#define NB_DID_M_OFF 0xffbfffff	/* NbDid mask off */
#define NB_DID_POS 22			/* NbDid bit shift for position */
#define PS_M_OFF 0xfff8ffff		/* Cur Pstate mask off */
#define PS_1 0x00010000		/* P-state 1 */
#define PS_2 0x00020000		/* P-state 2 */
#define PS_CPU_DID_1 0x40		/* Cpu Did 1 */

#define NB_VID1_MASK  0x00003f80      /* F3x1F4[NbVid1]*/
#define NB_VID1_SHIFT  7      /* F3x1F4[NbVid1] */



#define PSTATE_STS 0xC0010071		/* P-state Status Register */
#define STARTUP_PS_MASK 0x7		/* StartupPstate Mask */

/* define for NB VID & CPU VID transition functions */
#define IS_NB 1
#define IS_CPU 0

/* F3xD8 Clock Power/Timing Control 1 Register */
#define CPTC1 0xd8			/* Clock Power/Timing Control1 Register*/
#define VSRAMP_SLAM_MASK 0xffffff88	/* MaskOff [VSRampTime]&[VSSlamTime] */
#define VSRAMP_SLAM_VALUE 0x16		/* [VSRampTime]=001b&[VSSlamTime]=110b */
#define VSRAMP_MASK 0xffffff8f		/* MaskOff [VSRampTime] */
#define VSRAMP_VALUE 0x10		/* [VSRampTime]=001b */
#define VS_RAMP_T 4			/* VSRampTime bit position */
#define VSSLAM_MASK 0xfffffff8		/* MaskOff [VSSlamTime] */
#define PWR_PLN_SHIFT 28		/* PwrPlanes bit shift */
#define PWR_PLN_ON 0x10000000		/* PwrPlanes bit ON */
#define PWR_PLN_OFF 0x0efffffff	/* PwrPlanes bit OFF */



/* Northbridge Capability Register */
#define NB_CAP 0xe8			/* Northbridge Cap Reg */
#define CMP_CAP_SHFT 12		/* CMP CAP - number of enabled cores */

/* F3xDC Clock Power/Timing Control 2 Register */
#define CPTC2 0xdc			/* Clock Power/Timing Control2 Register*/
#define PS_MAX_VAL_POS 8		/* PstateMaxValue bit shift */
#define PS_MAX_VAL_MASK 0xfffff8ff	/* PstateMaxValue Mask off */
#define NB_SYN_PTR_ADJ_POS 12            /* NbsynPtrAdj bit shift */
#define NB_SYN_PTR_ADJ_MASK (0x7 << NB_SYN_PTR_ADJ_POS)  /* NbsynPtrAdj bit mask */

#define PRCT_INFO 0x1fc		/* Product Info Register */
#define DUAL_PLANE_ONLY_MASK 0x80000000  /* F3x1FC[DualPlaneOnly] */
#define UNI_NB_FID_BIT 2		/* UniNbFid bit position */
#define UNI_NB_VID_BIT 7		/* UniNbVid bit position */
#define SPLT_NB_FID_OFFSET 14		/* SpltNbFidOffset value bit position */
#define SPLT_NB_VID_OFFSET 17		/* SpltNbVidOffset value bit position */
#define NB_CV_UPDATE 0x01		/* F3x1FC[NbCofVidUpdated] bit mask */
#define NB_VID_UPDATE_ALL 0x02		/* F3x1FC[NbVidUpdatedAll] bit mask */
#define C_FID_DID_M_OFF  0xfffffe00	/* mask off Core FID & DID */

#define CPB_MASK 0x00000020             /* core performance
					   boost. CPUID Fn8000 0007 edx */
#define NC_MASK 0x000000FF              /* number of cores - 1. CPUID
					   Fn8000 0008 ecx */
#define PW_CTL_MISC 0x0a0		/* Power Control Miscellaneous Register */
#define COF_VID_PROG_BIT 0x80000000	/* CofVidProg bit. 0= unfused part */
#define DUAL_VDD_BIT 0x40000000	/* DualVdd bit. */
#define NB_COFVID_UPDATE_BIT 0x01	/* NbCOFVIDUpdated bit */
#define PVI_MODE 0x100			/* PviMode bit mask */
#define VID_SLAM_OFF 0x0dfffffff	/* set VidSlamMode OFF */
#define VID_SLAM_ON 0x020000000	/* set VidSlamMode ON */
#define NB_PSTATE_FORCE_ON 0x010000000 /* set Northbridge P-state
					  force on next LDTSTOP
					  assertion on, in F3xA0 */
#define BP_INS_TRI_EN_ON 0x00004000   /* breakpoint pins tristate
					 enable in F3xA0 */
#define PLLLOCK_OFF 0x0ffffc7ff	/* PllLockTime Mask OFF */
#define PLLLOCK_DFT 0x00001800		/* PllLockTime default value = 011b */
#define PLLLOCK_DFT_L 0x00002800	/* PllLockTime long value = 101b */

#define SVI_HIGH_FREQ_ON 0x00000200     /* F3xA0[SviHighFreqSel] for
					   3.4 MHz SVI in rev. C3 */

/* P-state Specification register base in PCI space */
#define PS_SPEC_REG 0x1e0		/* PS Spec register base address */
#define PCI_REG_LEN 4			/* PCI register length */
#define NB_DID_MASK 0x10000		/* NbDid bit mask */
#define NB_DID_2 2			/* NbDid = 2 */
#define NB_DID_1 1			/* NbDid = 1 */
#define SPEC_PWRDIV_M_ON 0x06000000	/* PwrDiv mask on */
#define SPEC_PWRVAL_M_ON 0x01e00000	/* PwrValue mask on */
#define SPEC_PWRDIV_SHFT 25		/* PwrDiv shift */
#define SPEC_PWRVAL_SHFT 17		/* PwrValue shift */

/* F4x1F4 Northbridge P-state spec register */
#define NB_PS_SPEC_REG 0x1f4		/* Nb PS spec reg */

/* F3x1F0 Product Information Register */
#define NB_PSTATE_MASK 0x00070000 /* NbPstate for CPU rev C3 */

/* F3x1FC Product Information Register */
#define NB_COF_VID_UPDATE_MASK 1 /* for CPU rev <= C */
#define SINGLE_PLANE_NB_FID_MASK 0x007c/* for CPU rev <= C */
#define SINGLE_PLANE_NB_FID_SHIFT 2/* for CPU rev <= C */
#define SINGLE_PLANE_NB_VID_MASK 0x3f80/* for CPU rev <= C */
#define SINGLE_PLANE_NB_VID_SHIFT 7/* for CPU rev <= C */

#define DUAL_PLANE_NB_FID_OFF_MASK 0x001c000/* for CPU rev <= C */
#define DUAL_PLANE_NB_FID_SHIFT 14/* for CPU rev <= C */
#define DUAL_PLANE_NB_VID_OFF_MASK 0x3e0000/* for CPU rev <= C */
#define DUAL_PLANE_NB_VID_SHIFT 17/* for CPU rev <= C */


#define NM_PS_REG 5			/* number of P-state MSR registers */

/* sFidVidInit.outFlags defines */
#define PWR_CK_OK 0			/* System board check OK */
#define PWR_CK_NO_PS 1			/* All P-state registers are over
					   the limit */

/* bit mask */
#define BIT_MASK_1 0x1
#define BIT_MASK_2 0x3
#define BIT_MASK_3 0x7
#define BIT_MASK_4 0x0f
#define BIT_MASK_5 0x1f
#define BIT_MASK_6 0x3f
#define BIT_MASK_7 0x7f
#define BIT_MASK_8 0x0ff

/* VID Code */
#define VID_1_100V 0x12		/* 1.100V */
#define VID_1_175V 0x1E		/* 1.175V */


/* Nb Fid Code */
#define NB_FID_800M 0x00		/* 800MHz */

/* Nb DID Code */
#define NB_DID_0 0
#define NB_DID_1 1

/* GH Logical ID */

#define GH_REV_A2 0x4			/* GH Rev A2 logical ID, Upper half */


#define TSC_MSR 0x10
#define CUR_PSTATE_MSR 0xc0010063
#define TSC_FREQ_SEL_SHIFT 24

#define TSC_FREQ_SEL_MASK (1 << TSC_FREQ_SEL_SHIFT)

#define  WAIT_PSTATE_TIMEOUT 80000000  /* 0.1 s , unit : 1.25 ns */

#endif
