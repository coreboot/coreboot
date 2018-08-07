/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 Advanced Micro Devices, Inc.
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#ifndef AMDDEFS_H
#define AMDDEFS_H

/* FIXME: this file should be moved to include/cpu/amd/amddefs.h */

/* Public Revisions - USE THESE VERSIONS TO MAKE COMPARE WITH CPULOGICALID RETURN VALUE*/
#define	AMD_SAFEMODE	0x8000000000000000	/* Unknown future revision - SAFE MODE */
#define	AMD_NPT_F0	0x0000000000000001	/* F0 stepping */
#define	AMD_NPT_F1	0x0000000000000002	/* F1 stepping */
#define	AMD_NPT_F2C	0x0000000000000004
#define	AMD_NPT_F2D	0x0000000000000008
#define	AMD_NPT_F2E	0x0000000000000010	/* F2 stepping E */
#define	AMD_NPT_F2G	0x0000000000000020	/* F2 stepping G */
#define	AMD_NPT_F2J	0x0000000000000040
#define	AMD_NPT_F2K	0x0000000000000080
#define	AMD_NPT_F3L	0x0000000000000100	/* F3 Stepping */
#define	AMD_NPT_G0A	0x0000000000000200	/* G0 stepping */
#define	AMD_NPT_G1B	0x0000000000000400	/* G1 stepping */
#define	AMD_DR_A0A	0x0000000000010000	/* Barcelona A0 */
#define	AMD_DR_A1B	0x0000000000020000	/* Barcelona A1 */
#define	AMD_DR_A2	0x0000000000040000	/* Barcelona A2 */
#define	AMD_DR_B0	0x0000000000080000	/* Barcelona B0 */
#define	AMD_DR_B1	0x0000000000100000	/* Barcelona B1 */
#define	AMD_DR_B2	0x0000000000200000	/* Barcelona B2 */
#define	AMD_DR_BA	0x0000000000400000	/* Barcelona BA */
#define	AMD_DR_B3	0x0000000000800000	/* Barcelona B3 */
#define	AMD_RB_C2	0x0000000001000000	/* Shanghai C2 */
#define	AMD_DA_C2	0x0000000002000000	/* XXXX C2 */
#define	AMD_HY_D0	0x0000000004000000	/* Istanbul D0 */
#define	AMD_RB_C3	0x0000000008000000	/* ??? C3 */
#define	AMD_DA_C3	0x0000000010000000	/* XXXX C3 */
#define	AMD_HY_D1	0x0000000020000000	/* Istanbul D1 */
#define	AMD_PH_E0	0x0000000040000000	/* Phenom II X4 X6 */
#define	AMD_OR_B2	0x0000000080000000	/* Interlagos */
#define	AMD_OR_C0	0x0000000100000000	/* Abu Dhabi */

/*
 * Groups - Create as many as you wish, from the above public values
 */
#define	AMD_NPT_F2		(AMD_NPT_F2C | AMD_NPT_F2D | AMD_NPT_F2E | AMD_NPT_F2G | AMD_NPT_F2J | AMD_NPT_F2K)
#define	AMD_NPT_F3		(AMD_NPT_F3L)
#define	AMD_NPT_Fx		(AMD_NPT_F0 | AMD_NPT_F1 | AMD_NPT_F2 | AMD_NPT_F3)
#define	AMD_NPT_Gx		(AMD_NPT_G0A | AMD_NPT_G1B)
#define	AMD_NPT_ALL		(AMD_NPT_Fx | AMD_NPT_Gx)
#define	AMD_FINEDELAY		(AMD_NPT_F0 | AMD_NPT_F1 | AMD_NPT_F2)
#define	AMD_GT_F0		(AMD_NPT_ALL AND NOT AMD_NPT_F0)
#define	AMD_DR_Ax		(AMD_DR_A0A + AMD_DR_A1B + AMD_DR_A2)
#define	AMD_DR_Bx		(AMD_DR_B0 | AMD_DR_B1 | AMD_DR_B2 | AMD_DR_B3 | AMD_DR_BA)
#define	AMD_DR_Cx		(AMD_RB_C2 | AMD_RB_C3 | AMD_DA_Cx)
#define	AMD_DR_Dx		(AMD_HY_D0 | AMD_HY_D1)
#define	AMD_DR_Ex		(AMD_PH_E0)
#define	AMD_DR_LT_B2		(AMD_DR_B0 | AMD_DR_B1 | AMD_DR_BA)
#define	AMD_DR_LT_B3		(AMD_DR_B0 | AMD_DR_B1 | AMD_DR_B2 | AMD_DR_BA)
#define	AMD_DR_GT_B0		(AMD_DR_ALL & ~(AMD_DR_B0))
#define	AMD_DR_GT_Bx		(AMD_DR_ALL & ~(AMD_DR_Ax | AMD_DR_Bx))
#define	AMD_DR_GT_D0		((AMD_DR_Dx & ~(AMD_HY_D0)) | AMD_DR_Ex)
#define	AMD_DR_ALL		(AMD_DR_Ax | AMD_DR_Bx | AMD_DR_Cx | AMD_DR_Dx | AMD_DR_Ex)
#define	AMD_FAM10_ALL		(AMD_DR_ALL | AMD_RB_C2 | AMD_HY_D0 | AMD_DA_C3 | AMD_DA_C2 | AMD_RB_C3 | AMD_HY_D1 | AMD_PH_E0)
#define AMD_FAM10_LT_D		(AMD_FAM10_ALL & ~(AMD_HY_D0))
#define	AMD_FAM10_GT_B0		(AMD_FAM10_ALL & ~(AMD_DR_B0))
#define AMD_FAM10_REV_D		(AMD_HY_D0 | AMD_HY_D1)
#define	AMD_DA_Cx		(AMD_DA_C2 | AMD_DA_C3)
#define	AMD_FAM10_C3		(AMD_RB_C3 | AMD_DA_C3)
#define	AMD_DRBH_Cx		(AMD_DR_Cx | AMD_HY_D0)
#define	AMD_DRBA23_RBC2		(AMD_DR_BA | AMD_DR_B2 | AMD_DR_B3 | AMD_RB_C2)
#define	AMD_DR_DAC2_OR_C3	(AMD_DA_C2 | AMD_DA_C3 | AMD_RB_C3)
#define	AMD_FAM15_ALL		(AMD_OR_B2 | AMD_OR_C0)

/*
 *  Public Platforms - USE THESE VERSIONS TO MAKE COMPARE WITH CPUPLATFORMTYPE RETURN VALUE
 */
#define	AMD_PTYPE_DSK	0x001	/* Desktop/DTR/UP */
#define	AMD_PTYPE_MOB	0x002	/* Mobile/Cool-n-quiet */
#define	AMD_PTYPE_SVR	0x004	/* Workstation/Server/Multicore DT */
#define	AMD_PTYPE_UC	0x008	/* Single Core */
#define	AMD_PTYPE_DC	0x010	/* Dual Core */
#define	AMD_PTYPE_MC	0x020	/* Multi Core (>2) */
#define	AMD_PTYPE_UMA	0x040	/* UMA required */

/*
 * Groups - Create as many as you wish, from the above public values
 */
#define	AMD_PTYPE_ALL	0xFFFFFFFF	/* A mask for all */


/*
 * CPU PCI HT PHY REGISTER, LINK TYPES - PRIVATE
 */
#define HTPHY_LINKTYPE_HT3		0x00000001
#define HTPHY_LINKTYPE_HT1		0x00000002
#define HTPHY_LINKTYPE_COHERENT		0x00000004
#define HTPHY_LINKTYPE_NONCOHERENT	0x00000008
#define HTPHY_LINKTYPE_CONNECTED	(HTPHY_LINKTYPE_COHERENT | HTPHY_LINKTYPE_NONCOHERENT)
#define HTPHY_LINKTYPE_GANGED		0x00000010
#define HTPHY_LINKTYPE_UNGANGED		0x00000020
#define HTPHY_LINKTYPE_ALL		0x7FFFFFFF


/*
 * CPU HT PHY REGISTERS, FIELDS, AND MASKS
 */
#define HTPHY_OFFSET_MASK		0xE000FFFF
#define HTPHY_WRITE_CMD			0x40000000
#define HTPHY_IS_COMPLETE_MASK		0x80000000
#define HTPHY_DIRECT_MAP		0x20000000
#define HTPHY_DIRECT_OFFSET_MASK	0xE000FFFF


/*
 * Various AMD MSRs
 */
#define CPUID_EXT_PM		0x80000007
#define CPUID_MODEL		1
#define MCG_CAP			0x00000179
	#define MCG_CTL_P	8
	#define MCA_BANKS_MASK	0xff
#define MC0_CTL			0x00000400
#define MC0_STA			(MC0_CTL + 1)
#define MC4_MISC0		0x00000413
#define MC4_MISC1		0xC0000408
#define MC4_MISC2		0xC0000409
#define FS_Base			0xC0000100
#define SYSCFG			0xC0010010
#define HWCR			0xC0010015
#define NB_CFG			0xC001001F
#define FidVidStatus		0xC0010042
#define MC1_CTL_MASK		0xC0010045
#define MC4_CTL_MASK		0xC0010048
#define OSVW_ID_Length		0xC0010140
#define OSVW_Status		0xC0010141
#define CPUIDFEATURES		0xC0011004
#define LS_CFG			0xC0011020
#define IC_CFG			0xC0011021
#define DC_CFG			0xC0011022
#define BU_CFG			0xC0011023
#define FP_CFG			0xC0011028
#define DE_CFG			0xC0011029
#define BU_CFG2			0xC001102A
#define BU_CFG3			0xC001102B
#define EX_CFG			0xC001102C
#define LS_CFG2			0xC001102D
#define IBS_OP_DATA3		0xC0011037

/*
 * Processor package types
 */
#define AMD_PKGTYPE_FrX_1207 0
#define AMD_PKGTYPE_AM3_2r2 1
#define AMD_PKGTYPE_S1gX 2
#define AMD_PKGTYPE_G34 3
#define AMD_PKGTYPE_ASB2 4
#define AMD_PKGTYPE_C32 5
#define AMD_PKGTYPE_FM2 6

//DDR2 REG and unbuffered : Socket F 1027 and AM3
/* every channel have 4 DDR2 DIMM for socket F
 *		       2 for socket M2/M3
 *		       1 for socket s1g1
 */
#define DIMM_SOCKETS 4
struct mem_controller {
	u32 node_id;
	pci_devfn_t f0, f1, f2, f3, f4, f5;
	/* channel0 is DCT0 --- channelA
	 * channel1 is DCT1 --- channelB
	 * can be ganged, a single dual-channel DCT ---> 128 bit
	 *	 or unganged a two single-channel DCTs ---> 64bit
	 * When the DCTs are ganged, the writes to DCT1 set of registers
	 * (F2x1XX) are ignored and reads return all 0's
	 * The exception is the DCT phy registers, F2x[1,0]98, F2x[1,0]9C,
	 * and all the associated indexed registers, are still
	 * independently accessiable
	 */
	/* FIXME: I will only support ganged mode for easy support */
	u8 spd_switch_addr;
	u8 spd_addr[DIMM_SOCKETS*2];
};

#endif
