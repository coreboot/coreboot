/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */


/*
 * Default MSR and errata settings.
 */
static const struct {
	u32 msr;
	u32 revision;
	u32 platform;
	u32 data_lo;
	u32 data_hi;
	u32 mask_lo;
	u32 mask_hi;
} fam10_msr_default[] = {
	{ TOP_MEM2, AMD_DR_ALL, AMD_PTYPE_ALL,
	  0x00000000, 0x00000000,
	  0xFFFFFFFF, 0xFFFFFFFF },

	{ SYSCFG, AMD_DR_ALL, AMD_PTYPE_ALL,
	  3 << 21, 0x00000000,
	  3 << 21, 0x00000000 },	/* [MtrrTom2En]=1,[TOM2EnWB] = 1*/

	{ HWCR, AMD_DR_ALL, AMD_PTYPE_ALL,
	  1 << 4, 0x00000000,
	  1 << 4, 0x00000000 },		/* [INVD_WBINVD]=1 */

	{ MC4_CTL_MASK, AMD_DR_ALL, AMD_PTYPE_ALL,
	  0xF << 19, 0x00000000,
	  0xF << 19, 0x00000000 },	/* [RtryHt[0..3]]=1 */

	{ DC_CFG, AMD_DR_ALL, AMD_PTYPE_SVR,
	  0x00000000, 0x00000004,
	  0x00000000, 0x0000000C },	/* [REQ_CTR] = 1 for Server */

	{ DC_CFG, AMD_DR_Bx, AMD_PTYPE_SVR,
	  0x00000000, 0x00000000,
	  0x00000000, 0x00000C00 },	/* Errata 326 */

	{ NB_CFG, AMD_DR_ALL, AMD_PTYPE_DC | AMD_PTYPE_MC,
	  0x00000000, 1 << 22,
	  0x00000000, 1 << 22 },	/* [ApicInitIDLo]=1 */

	{ BU_CFG2, AMD_DR_Bx, AMD_PTYPE_ALL,
	  1 << 29, 0x00000000,
	  1 << 29, 0x00000000 },	/* For Bx Smash1GPages=1 */

	{ DC_CFG, AMD_DR_ALL, AMD_PTYPE_ALL,
	  1 << 24, 0x00000000,
	  1 << 24, 0x00000000 },	/* Erratum #261 [DIS_PIGGY_BACK_SCRUB]=1 */

	{ LS_CFG, AMD_DR_GT_B0, AMD_PTYPE_ALL,
	  0 << 1, 0x00000000,
	  1 << 1, 0x00000000 },		/* IDX_MATCH_ALL=0 */

	{ BU_CFG, AMD_DR_GT_B0, AMD_PTYPE_ALL,
	  1 << 21, 0x00000000,
	  1 << 21, 0x00000000 },	/* Erratum #254 DR B1 BU_CFG[21]=1 */

	{ BU_CFG, AMD_DR_LT_B3, AMD_PTYPE_ALL,
	  1 << 23, 0x00000000,
	  1 << 23, 0x00000000 },	/* Erratum #309 BU_CFG[23]=1 */

	/* CPUID_EXT_FEATURES */
	{ CPUIDFEATURES, AMD_DR_ALL, AMD_PTYPE_DC | AMD_PTYPE_MC,
	  1 << 28, 0x00000000,
	  1 << 28, 0x00000000 },	/* [HyperThreadFeatEn]=1 */

	{ CPUIDFEATURES, AMD_DR_ALL, AMD_PTYPE_DC,
	  0x00000000, 1 << (33-32),
	  0x00000000, 1 << (33-32) },	/* [ExtendedFeatEn]=1 */
};


/*
 * Default PCI and errata settings.
 */
static const struct {
	u8  function;
	u16 offset;
	u32 revision;
	u32 platform;
	u32 data;
	u32 mask;
} fam10_pci_default[] = {

	/* Function 0 - HT Config */

	{ 0, 0x68, AMD_DR_ALL, AMD_PTYPE_ALL,
	  0x004E4800, 0x006E6800 },	/* [19:17] for 8bit APIC config,
	  [14:13] BufPriRel = 2h [11] RspPassPW set,
	  [22:21] DsNpReqLmt = 10b */

	/* Errata 281 Workaround */
	{ 0, 0x68, (AMD_DR_B0 | AMD_DR_B1),
	  AMD_PTYPE_SVR, 0x00200000, 0x00600000 },	/* [22:21] DsNpReqLmt0 = 01b */

	{ 0, 0x84, AMD_DR_ALL, AMD_PTYPE_ALL,
	  0x00002000, 0x00002000 },	/* [13] LdtStopTriEn = 1 */

	{ 0, 0xA4, AMD_DR_ALL, AMD_PTYPE_ALL,
	  0x00002000, 0x00002000 },	/* [13] LdtStopTriEn = 1 */

	{ 0, 0xC4, AMD_DR_ALL, AMD_PTYPE_ALL,
	  0x00002000, 0x00002000 },	/* [13] LdtStopTriEn = 1 */

	{ 0, 0xE4, AMD_DR_ALL, AMD_PTYPE_ALL,
	  0x00002000, 0x00002000 },	/* [13] LdtStopTriEn = 1 */

	/* Link Global Extended Control Register */
	{ 0, 0x16C, AMD_DR_ALL, AMD_PTYPE_ALL,
	  0x0000C000, 0x0000E000 },	/* [15:13] ForceFullT0 = 110b */

	/* Function 1 - Map Init */

	/* Before reading F1x114_x2 or F1x114_x3 software must
	 * initialize the registers or NB Array MCA errors may
	 * occur.  BIOS should initialize index 0h of F1x114_x2 and
	 * F1x114_x3 to prevent reads from F1x114 from generating NB
	 * Array MCA errors.  BKDG Doc #3116 Rev 1.07
	 */

	{ 1, 0x110, AMD_DR_ALL, AMD_PTYPE_ALL,
	  0x20000000, 0xFFFFFFFF },	/* Select extended MMIO Base */

	{ 1, 0x114, AMD_DR_ALL, AMD_PTYPE_ALL,
	  0x00000000, 0xFFFFFFFF },	/* Clear map  */

	{ 1, 0x110, AMD_DR_ALL, AMD_PTYPE_ALL,
	  0x30000000, 0xFFFFFFFF },	/* Select extended MMIO Base */

	{ 1, 0x114, AMD_DR_ALL, AMD_PTYPE_ALL,
	  0x00000000, 0xFFFFFFFF },	/* Clear map  */

	/* Function 2 - DRAM Controller */

	/* Function 3 - Misc. Control */
	{ 3, 0x40, AMD_DR_ALL, AMD_PTYPE_ALL,
	  0x00000100, 0x00000100 },	/* [8] MstrAbrtEn */

	{ 3, 0x44, AMD_DR_ALL, AMD_PTYPE_ALL,
	  0x4A30005C, 0x4A30005C },	/* [30] SyncOnDramAdrParErrEn = 1,
					   [27] NbMcaToMstCpuEn = 1,
					   [25] DisPciCfgCpuErrRsp = 1,
					   [21] SyncOnAnyErrEn = 1,
					   [20] SyncOnWDTEn = 1,
					   [6] CpuErrDis = 1,
					   [4] SyncPktPropDis = 1,
					   [3] SyncPktGenDis = 1,
					   [2] SyncOnUcEccEn = 1 */

	/* XBAR buffer settings */
	{ 3, 0x6C, AMD_DR_ALL, AMD_PTYPE_ALL,
	  0x00018052, 0x700780F7 },

	/* Errata 281 Workaround */
	{ 3, 0x6C, ( AMD_DR_B0 | AMD_DR_B1),
	  AMD_PTYPE_SVR, 0x00010094, 0x700780F7 },

	{ 3, 0x6C, AMD_DR_ALL, AMD_PTYPE_UMA,
	  0x60018051, 0x700780F7 },

	{ 3, 0x70, AMD_DR_ALL, AMD_PTYPE_ALL,
	  0x00041153, 0x777777F7 },

	{ 3, 0x70, AMD_DR_ALL, AMD_PTYPE_UMA,
	  0x61221151, 0x777777F7 },

	{ 3, 0x74, AMD_DR_ALL, AMD_PTYPE_UMA,
	  0x00080101, 0x000F7777 },

	{ 3, 0x7C, AMD_DR_ALL, AMD_PTYPE_ALL,
	  0x00090914, 0x707FFF1F },

	/* Errata 281 Workaround */
	{ 3, 0x7C, ( AMD_DR_B0 | AMD_DR_B1),
	 AMD_PTYPE_SVR, 0x00144514, 0x707FFF1F },

	{ 3, 0x7C, AMD_DR_ALL, AMD_PTYPE_UMA,
	  0x00070814, 0x007FFF1F },

	{ 3, 0x140, AMD_DR_ALL, AMD_PTYPE_ALL,
	  0x00800756, 0x00F3FFFF },

	{ 3, 0x140, AMD_DR_ALL, AMD_PTYPE_UMA,
	  0x00C37756, 0x00F3FFFF },

	{ 3, 0x144, AMD_DR_ALL, AMD_PTYPE_UMA,
	  0x00000036, 0x000000FF },

	/* Errata 281 Workaround */
	{ 3, 0x144, ( AMD_DR_B0 | AMD_DR_B1),
	 AMD_PTYPE_SVR, 0x00000001, 0x0000000F },
	 	/* [3:0] RspTok = 0001b */

	{ 3, 0x148, AMD_DR_ALL, AMD_PTYPE_UMA,
	  0x8000052A, 0xD5FFFFFF },

	/* ACPI Power State Control Reg1 */
	{ 3, 0x80, AMD_DR_ALL, AMD_PTYPE_ALL,
	  0xE6002200, 0xFFFFFFFF },

	/* ACPI Power State Control Reg2 */
	{ 3, 0x84, AMD_DR_ALL, AMD_PTYPE_ALL,
	  0xA0E641E6, 0xFFFFFFFF },

	{ 3, 0xA0, AMD_DR_ALL, AMD_PTYPE_MOB | AMD_PTYPE_DSK,
	  0x00000080, 0x00000080 },	/* [7] PSIVidEnable */

	{ 3, 0xA0, AMD_DR_ALL, AMD_PTYPE_ALL,
	  0x00001800, 0x000003800 },	/* [13:11] PllLockTime = 3 */

	/* Reported Temp Control Register */
	{ 3, 0xA4, AMD_DR_ALL, AMD_PTYPE_ALL,
	  0x00000080, 0x00000080 },	/* [7] TempSlewDnEn = 1 */

	/* Clock Power/Timing Control 0 Register */
	{ 3, 0xD4, AMD_DR_ALL, AMD_PTYPE_ALL,
	  0xC0000F00, 0xF0000F00 },	/*  [31] NbClkDivApplyAll = 1,
		[30:28] NbClkDiv = 100b,[11:8] ClkRampHystSel = 1111b */

	/* Clock Power/Timing Control 1 Register */
	{ 3, 0xD8, AMD_DR_ALL, AMD_PTYPE_ALL,
	  0x03000016, 0x0F000077 },	/* [6:4] VSRampTime = 1,
		[2:0] VSSlamTime = 6, [27:24] ReConDel = 3 */


	/* Clock Power/Timing Control 2 Register */
	{ 3, 0xDC, AMD_DR_ALL, AMD_PTYPE_ALL,
	  0x00005000, 0x00007000 },	/* [14:12] NbsynPtrAdj = 5 */


	/* Extended NB MCA Config Register */
	{ 3, 0x180, AMD_DR_ALL, AMD_PTYPE_ALL,
	  0x007003E2, 0x007003E2 },	/* [22:20] = SyncFloodOn_Err = 7,
					   [9] SyncOnUncNbAryEn = 1 ,
					   [8] SyncOnProtEn = 1,
					   [7] SyncFloodOnTgtAbtErr = 1,
					   [6] SyncFloodOnDatErr = 1,
					   [5] DisPciCfgCpuMstAbtRsp = 1,
					   [1] SyncFloodOnUsPwDataErr = 1 */

	/* L3 Control Register */
	{ 3, 0x1B8, AMD_DR_ALL, AMD_PTYPE_ALL,
	  0x00001000, 0x00001000 },	/* [12] = L3PrivReplEn */

	/* IBS Control Register */
	{ 3, 0x1CC, AMD_DR_ALL, AMD_PTYPE_ALL,
	  0x00000100, 0x00000100 },	/* [8] = LvtOffsetVal */
};


/*
 * Default HyperTransport Phy and errata settings.
 */
static const struct {
	u16 htreg;	/* HT Phy Register index */
	u32 revision;
	u32 platform;
	u32 linktype;
	u32 data;
	u32 mask;
} fam10_htphy_default[] = {

	{ 0x520A, AMD_DR_Bx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00004400, 0x00006400 },	/* HT_PHY_DLL_REG */

	{ 0x530A, AMD_DR_Bx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00004400, 0x00006400 },	/* HT_PHY_DLL_REG */

	{ 0xCF, AMD_DR_ALL, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00000000, 0x000000FF },	/* Provide clear setting for logical
					   completeness */

	{ 0xDF, AMD_DR_ALL, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00000000, 0x000000FF },	/* Provide clear setting for logical
					   completeness */

	{ 0xCF, AMD_DR_ALL, AMD_PTYPE_ALL, HTPHY_LINKTYPE_HT1,
	  0x0000006D, 0x000000FF },	/* HT_PHY_HT1_FIFO_PTR_OPT_VALUE */

	{ 0xDF, AMD_DR_ALL, AMD_PTYPE_ALL,  HTPHY_LINKTYPE_HT1,
	  0x0000006D, 0x000000FF }, 	/* HT_PHY_HT1_FIFO_PTR_OPT_VALUE */

	/* Link Phy Receiver Loop Filter Registers */
	{ 0xD1, AMD_DR_ALL, AMD_PTYPE_ALL, HTPHY_LINKTYPE_HT3,
	  0x08040000, 0x3FFFC000 },	/* [29:22] LfcMax = 20h,
					   [21:14] LfcMin = 10h */

	{ 0xC1, AMD_DR_ALL, AMD_PTYPE_ALL, HTPHY_LINKTYPE_HT3,
	  0x08040000, 0x3FFFC000 },	/* [29:22] LfcMax = 20h,
					   [21:14] LfcMin = 10h */

	{ 0xD1, AMD_DR_ALL, AMD_PTYPE_ALL, HTPHY_LINKTYPE_HT1,
	  0x04020000, 0x3FFFC000 },	/* [29:22] LfcMax = 10h,
					   [21:14] LfcMin = 08h */

	{ 0xC1, AMD_DR_ALL, AMD_PTYPE_ALL, HTPHY_LINKTYPE_HT1,
	  0x04020000, 0x3FFFC000 },	/* [29:22] LfcMax = 10h,
					   [21:14] LfcMin = 08h */
};
