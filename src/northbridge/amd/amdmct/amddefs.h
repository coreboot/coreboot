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

/* FIXME: this file should be moved to include/cpu/amd/amddefs.h */

/* Public Revisions - USE THESE VERSIONS TO MAKE COMPARE WITH CPULOGICALID RETURN VALUE*/
#define	AMD_SAFEMODE	0x80000000	/* Unknown future revision - SAFE MODE */
#define	AMD_NPT_F0	0x00000001	/* F0 stepping */
#define	AMD_NPT_F1	0x00000002	/* F1 stepping */
#define	AMD_NPT_F2C	0x00000004
#define	AMD_NPT_F2D	0x00000008
#define	AMD_NPT_F2E	0x00000010	/* F2 stepping E */
#define	AMD_NPT_F2G	0x00000020	/* F2 stepping G */
#define	AMD_NPT_F2J	0x00000040
#define	AMD_NPT_F2K	0x00000080
#define	AMD_NPT_F3L	0x00000100	/* F3 Stepping */
#define	AMD_NPT_G0A	0x00000200	/* G0 stepping */
#define	AMD_NPT_G1B	0x00000400	/* G1 stepping */
#define	AMD_DR_A0A	0x00010000	/* Barcelona A0 */
#define	AMD_DR_A1B	0x00020000	/* Barcelona A1 */
#define	AMD_DR_A2	0x00040000	/* Barcelona A2 */
#define	AMD_DR_B0	0x00080000	/* Barcelona B0 */
#define	AMD_DR_B1	0x00100000	/* Barcelona B1 */
#define	AMD_DR_B2	0x00200000	/* Barcelona B2 */
#define	AMD_DR_BA	0x00400000	/* Barcelona BA */
#define	AMD_DR_B3	0x00800000	/* Barcelona B3 */
#define	AMD_RB_C2	0x01000000	/* Shanghai C2 */
#define	AMD_DA_C2	0x02000000	/* XXXX C2 */
#define	AMD_HY_D0	0x04000000	/* Istanbul D0 */
#define	AMD_RB_C3	0x08000000	/* ??? C3 */
#define	AMD_DA_C3	0x10000000	/* XXXX C3 */
#define	AMD_HY_D1	0x20000000	/* Istanbul D1 */
#define	AMD_PH_E0	0x40000000	/* Phenom II X4 X6 */

/*
 * Groups - Create as many as you wish, from the above public values
 */
#define	AMD_NPT_F2	(AMD_NPT_F2C | AMD_NPT_F2D | AMD_NPT_F2E | AMD_NPT_F2G | AMD_NPT_F2J | AMD_NPT_F2K)
#define	AMD_NPT_F3	(AMD_NPT_F3L)
#define	AMD_NPT_Fx	(AMD_NPT_F0 | AMD_NPT_F1 | AMD_NPT_F2 | AMD_NPT_F3)
#define	AMD_NPT_Gx	(AMD_NPT_G0A | AMD_NPT_G1B)
#define	AMD_NPT_ALL	(AMD_NPT_Fx | AMD_NPT_Gx)
#define	AMD_FINEDELAY	(AMD_NPT_F0 | AMD_NPT_F1 | AMD_NPT_F2)
#define	AMD_GT_F0	(AMD_NPT_ALL AND NOT AMD_NPT_F0)
#define	AMD_DR_Ax	(AMD_DR_A0A + AMD_DR_A1B + AMD_DR_A2)
#define	AMD_DR_Bx	(AMD_DR_B0 | AMD_DR_B1 | AMD_DR_B2 | AMD_DR_B3 | AMD_DR_BA)
#define	AMD_DR_LT_B2	(AMD_DR_B0 | AMD_DR_B1 | AMD_DR_BA)
#define	AMD_DR_LT_B3	(AMD_DR_B0 | AMD_DR_B1 | AMD_DR_B2 | AMD_DR_BA)
#define	AMD_DR_GT_B0	(AMD_DR_ALL & ~(AMD_DR_B0))
#define	AMD_DR_GT_Bx	(AMD_DR_ALL & ~(AMD_DR_Ax | AMD_DR_Bx))
#define	AMD_DR_ALL	(AMD_DR_Bx)
#define	AMD_FAM10_ALL	(AMD_DR_ALL | AMD_RB_C2 | AMD_HY_D0 | AMD_DA_C3 | AMD_DA_C2 | AMD_RB_C3 | AMD_HY_D1 | AMD_PH_E0)
#define AMD_FAM10_LT_D  (AMD_FAM10_ALL & ~(AMD_HY_D0))
#define	AMD_FAM10_GT_B0	(AMD_FAM10_ALL & ~(AMD_DR_B0))
#define	AMD_DA_Cx       (AMD_DA_C2 | AMD_DA_C3)
#define	AMD_DR_Cx       (AMD_RB_C2 | AMD_RB_C3 | AMD_DA_Cx)
#define	AMD_FAM10_C3       (AMD_RB_C3 | AMD_DA_C3)
#define	AMD_DR_Dx	(AMD_HY_D0 | AMD_HY_D1)
#define	AMD_DRBH_Cx	(AMD_DR_Cx | AMD_HY_D0 )
#define	AMD_DRBA23_RBC2	(AMD_DR_BA | AMD_DR_B2 | AMD_DR_B3 | AMD_RB_C2 )
#define	AMD_DR_DAC2_OR_C3	(AMD_DA_C2 | AMD_DA_C3 | AMD_RB_C3)

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
#define HTPHY_LINKTYPE_COHERENT	0x00000004
#define HTPHY_LINKTYPE_NONCOHERENT	0x00000008
#define HTPHY_LINKTYPE_CONNECTED	(HTPHY_LINKTYPE_COHERENT | HTPHY_LINKTYPE_NONCOHERENT)
#define HTPHY_LINKTYPE_GANGED		0x00000010
#define HTPHY_LINKTYPE_UNGANGED	0x00000020
#define HTPHY_LINKTYPE_ALL		0x7FFFFFFF


/*
 * CPU HT PHY REGISTERS, FIELDS, AND MASKS
 */
#define HTPHY_OFFSET_MASK		0xE00001FF
#define HTPHY_WRITE_CMD		0x40000000
#define HTPHY_IS_COMPLETE_MASK		0x80000000
#define HTPHY_DIRECT_MAP		0x20000000
#define HTPHY_DIRECT_OFFSET_MASK	0xE000FFFF


/*
 * Various AMD MSRs
 */
#define CPUID_EXT_PM		0x80000007
#define CPUID_MODEL		1
#define MCG_CAP		0x00000179
	#define MCG_CTL_P	8
#define MC0_CTL		0x00000400
#define MC0_STA		MC0_CTL + 1
#define FS_Base		0xC0000100
#define SYSCFG			0xC0010010
#define HWCR			0xC0010015
#define NB_CFG			0xC001001F
#define FidVidStatus		0xC0010042
#define MC4_CTL_MASK		0xC0010048
#define OSVW_ID_Length		0xC0010140
#define OSVW_Status		0xC0010141
#define CPUIDFEATURES		0xC0011004
#define LS_CFG			0xC0011020
#define DC_CFG			0xC0011022
#define BU_CFG			0xC0011023
#define BU_CFG2		0xC001102A

/*
 * Processor package types
 */
#define AMD_PKGTYPE_FrX_1207 0
#define AMD_PKGTYPE_AM3_2r2 1
#define AMD_PKGTYPE_S1gX 2
#define AMD_PKGTYPE_G34 3
#define AMD_PKGTYPE_ASB2 4
#define AMD_PKGTYPE_C32 5
