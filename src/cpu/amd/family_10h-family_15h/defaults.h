/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
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

#include <northbridge/amd/amdmct/amddefs.h>
#include <cpu/amd/mtrr.h>

/*
 * Default MSR and errata settings.
 */
static const struct {
	u32 msr;
	uint64_t revision;
	u32 platform;
	u32 data_lo;
	u32 data_hi;
	u32 mask_lo;
	u32 mask_hi;
} fam10_msr_default[] = {
	{ TOP_MEM2, (AMD_FAM10_ALL | AMD_FAM15_ALL), AMD_PTYPE_ALL,
	  0x00000000, 0x00000000,
	  0xFFFFFFFF, 0xFFFFFFFF },

	{ SYSCFG, (AMD_FAM10_ALL | AMD_FAM15_ALL), AMD_PTYPE_ALL,
	  3 << 21, 0x00000000,
	  3 << 21, 0x00000000 },	/* [MtrrTom2En]=1,[TOM2EnWB] = 1*/

	{ MC1_CTL_MASK, AMD_OR_B2, AMD_PTYPE_ALL,
	  1 << 18, 0x00000000,
	  1 << 18, 0x00000000 },	/* Erratum 586: [DEIBP]=1 */

	{ MC1_CTL_MASK, AMD_OR_B2, AMD_PTYPE_ALL,
	  1 << 15, 0x00000000,
	  1 << 15, 0x00000000 },	/* Erratum 593: [BSRP]=1 */

	{ MC1_CTL_MASK, AMD_OR_C0, AMD_PTYPE_ALL,
	  1 << 15, 0x00000000,
	  1 << 15, 0x00000000 },	/* Erratum 739: [BSRP]=1 */

	{ 0xc0011000, AMD_FAM15_ALL, AMD_PTYPE_ALL,
	  1 << 16, 0x00000000,
	  1 << 16, 0x00000000 },	/* Erratum 608: [bit 16]=1 */

	{ 0xc0011000, AMD_OR_C0, AMD_PTYPE_ALL,
	  1 << 15, 0x00000000,
	  1 << 15, 0x00000000 },	/* Erratum 727: [bit 15]=1 */

	{ MC4_CTL_MASK, (AMD_FAM10_ALL | AMD_FAM15_ALL), AMD_PTYPE_ALL,
	  0xF << 19, 0x00000000,
	  0xF << 19, 0x00000000 },	/* [RtryHt[0..3]]=1 */

	{ MC4_CTL_MASK, (AMD_FAM10_ALL | AMD_FAM15_ALL), AMD_PTYPE_ALL,
	  1 << 10, 0x00000000,
	  1 << 10, 0x00000000 },	/* [GartTblWkEn]=1 */

	{ DC_CFG, AMD_FAM10_ALL, AMD_PTYPE_SVR,
	  0x00000000, 0x00000004,
	  0x00000000, 0x0000000C },	/* Family 10h: [REQ_CTR] = 1 for Server */

	{ DC_CFG, AMD_DR_Bx, AMD_PTYPE_SVR,
	  0x00000000, 0x00000000,
	  0x00000000, 0x00000C00 },	/* Erratum 326 */

	{ NB_CFG, (AMD_FAM10_ALL | AMD_FAM15_ALL), AMD_PTYPE_DC | AMD_PTYPE_MC,
	  0x00000000, 1 << 22,
	  0x00000000, 1 << 22 },	/* [ApicInitIDLo]=1 */

	{ NB_CFG, AMD_FAM15_ALL, AMD_PTYPE_DC | AMD_PTYPE_MC,
	  1 << 23, 0x00000000,
	  1 << 23, 0x00000000 },	/* Erratum 663: [bit 23]=1 */

	{ BU_CFG2, AMD_DR_Bx, AMD_PTYPE_ALL,
	  1 << 29, 0x00000000,
	  1 << 29, 0x00000000 },	/* For Bx Smash1GPages=1 */

	{ DC_CFG, AMD_FAM10_ALL, AMD_PTYPE_ALL,
	  1 << 24, 0x00000000,
	  1 << 24, 0x00000000 },	/* Erratum #261 [DIS_PIGGY_BACK_SCRUB]=1 */

	{ LS_CFG, AMD_DR_GT_B0, AMD_PTYPE_ALL,
	  0 << 1, 0x00000000,
	  1 << 1, 0x00000000 },		/* IDX_MATCH_ALL=0 */

	{ IC_CFG, AMD_OR_C0, AMD_PTYPE_ALL,
	  0x00000000, 1 << (39-32),
	  0x00000000, 1 << (39-32)},	/* C0 or above [DisLoopPredictor]=1 */

	{ IC_CFG, AMD_OR_C0, AMD_PTYPE_ALL,
	  0xf << 1, 0x00000000,
	  0xf << 1, 0x00000000},	/* C0 or above [DisIcWayFilter]=0xf */

	{ BU_CFG, AMD_DR_LT_B3, AMD_PTYPE_ALL,
	  1 << 21, 0x00000000,
	  1 << 21, 0x00000000 },	/* Erratum #254 DR B1 BU_CFG[21]=1 */

	{ BU_CFG, AMD_DR_LT_B3, AMD_PTYPE_ALL,
	  1 << 23, 0x00000000,
	  1 << 23, 0x00000000 },	/* Erratum #309 BU_CFG[23]=1 */

	{ BU_CFG, AMD_FAM15_ALL, AMD_PTYPE_ALL,
	  0 << 10, 0x00000000,
	  1 << 10, 0x00000000 },	/* [DcacheAgressivePriority]=0 */

	/* CPUID_EXT_FEATURES */
	{ CPUIDFEATURES, (AMD_FAM10_ALL | AMD_FAM15_ALL), AMD_PTYPE_DC | AMD_PTYPE_MC,
	  1 << 28, 0x00000000,
	  1 << 28, 0x00000000 },	/* [HyperThreadFeatEn]=1 */

	{ CPUIDFEATURES, (AMD_FAM10_ALL | AMD_FAM15_ALL), AMD_PTYPE_DC,
	  0x00000000, 1 << (33-32),
	  0x00000000, 1 << (33-32) },	/* [ExtendedFeatEn]=1 */

	{ DE_CFG, AMD_OR_B2, AMD_PTYPE_ALL,
	  1 << 10, 0x00000000,
	  1 << 10, 0x00000000 },	/* Bx [ResyncPredSingleDispDis]=1 */

	{ BU_CFG2, AMD_DRBH_Cx, AMD_PTYPE_ALL,
	  0x00000000, 1 << (35-32),
	  0x00000000, 1 << (35-32) },	/* Erratum 343 (set to 0 after CAR, in post_cache_as_ram()/model_10xxx_init() )  */

	{ BU_CFG3, AMD_OR_B2, AMD_PTYPE_ALL,
	  0x00000000, 1 << (42-32),
	  0x00000000, 1 << (42-32)},	/* Bx [PwcDisableWalkerSharing]=1 */

	{ BU_CFG3, AMD_OR_C0, AMD_PTYPE_ALL,
	  1 << 22, 0x00000000,
	  1 << 22, 0x00000000},		/* C0 or above [PfcDoubleStride]=1 */

	{ EX_CFG, AMD_OR_C0, AMD_PTYPE_ALL,
	  0x00000000, 1 << (54-32),
	  0x00000000, 1 << (54-32)},	/* C0 or above [LateSbzResync]=1 */

	{ LS_CFG2, AMD_OR_C0, AMD_PTYPE_ALL,
	  1 << 23, 0x00000000,
	  1 << 23, 0x00000000},		/* C0 or above [DisScbThreshold]=1 */

	{ LS_CFG2, AMD_OR_C0, AMD_PTYPE_ALL,
	  1 << 14, 0x00000000,
	  1 << 14, 0x00000000},		/* C0 or above [ForceSmcCheckFlowStDis]=1 */

	{ LS_CFG2, AMD_OR_C0, AMD_PTYPE_ALL,
	  1 << 12, 0x00000000,
	  1 << 12, 0x00000000},		/* C0 or above [ForceBusLockDis]=1 */

	{ OSVW_ID_Length, AMD_DR_Bx | AMD_DR_Cx | AMD_DR_Dx, AMD_PTYPE_ALL,
	  0x00000004, 0x00000000,
	  0x00000004, 0x00000000},	/* B0 or Above, OSVW_ID_Length is 0004h */

	{ OSVW_Status, AMD_DR_Cx | AMD_DR_Dx, AMD_PTYPE_MC,
	  0x0000000C, 0x00000000,
	  0x0000000C, 0x00000000},	/* Cx and Dx multiple-link processor */

	{ OSVW_ID_Length, AMD_FAM15_ALL, AMD_PTYPE_ALL,
	  0x00000005, 0x00000000,
	  0x0000ffff, 0x00000000},	/* OSVW_ID_Length = 0x5 */

	{ OSVW_Status, AMD_FAM15_ALL, AMD_PTYPE_ALL,
	  0x00000010, 0x00000000,
	  0xffffffff, 0x00000000},	/* OsvwId4 = 0x1 */

	{ BU_CFG2, AMD_DR_Dx, AMD_PTYPE_ALL,
	  0x00000000, 1 << (50-32),
	  0x00000000, 1 << (50-32)},	/* D0 or Above, RdMmExtCfgQwEn*/

	{ BU_CFG2, AMD_FAM15_ALL, AMD_PTYPE_ALL,
	  0x00000000, 0x0 << (36-32),
	  0x00000000, 0x3 << (36-32)},	/* [ThrottleNbInterface]=0 */

	{ BU_CFG2, AMD_FAM15_ALL, AMD_PTYPE_ALL,
	  1 << 10, 0x00000000,
	  1 << 10, 0x00000000},		/* [VicResyncChkEn]=1 */

	{ BU_CFG2, AMD_FAM15_ALL, AMD_PTYPE_ALL,
	  1 << 11, 0x00000000,
	  1 << 11, 0x00000000},		/* Erratum 503: [bit 11]=1 */

	{ CPU_ID_EXT_FEATURES_MSR, AMD_DR_Dx, AMD_PTYPE_ALL,
	  0x00000000, 1 << (51 - 32),
	  0x00000000, 1 << (51 - 32)},	/* G34_PKG | C32_PKG | S1G4_PKG | ASB2_PKG */

	{ CPU_ID_EXT_FEATURES_MSR, AMD_FAM15_ALL, AMD_PTYPE_ALL,
	  0x00000000, 1 << (56 - 32),
	  0x00000000, 1 << (56 - 32)},	/* [PerfCtrExtNB]=1 */

	{ CPU_ID_EXT_FEATURES_MSR, AMD_FAM15_ALL, AMD_PTYPE_ALL,
	  0x00000000, 1 << (55 - 32),
	  0x00000000, 1 << (55 - 32)},	/* [PerfCtrExtCore]=1 */

	{ IBS_OP_DATA3, AMD_FAM15_ALL, AMD_PTYPE_ALL,
	  0 << 16, 0x00000000,
	  1 << 16, 0x00000000},		/* [IbsDcMabHit]=0 */

	{ MC4_MISC0, AMD_FAM15_ALL, AMD_PTYPE_ALL,
	  0x00000000, 0x1 << (52-32),
	  0x00000000, 0xf << (52-32)},	/* [LvtOffset]=1 */

	{ MC4_MISC1, AMD_FAM15_ALL, AMD_PTYPE_ALL,
	  0x00000000, 0x1 << (52-32),
	  0x00000000, 0xf << (52-32)},	/* [LvtOffset]=1 */

	{ MC4_MISC2, AMD_FAM15_ALL, AMD_PTYPE_ALL,
	  0x00000000, 0x1 << (52-32),
	  0x00000000, 0xf << (52-32)},	/* [LvtOffset]=1 */
};


/*
 * Default PCI and errata settings.
 */
static const struct {
	u8  function;
	u16 offset;
	uint64_t revision;
	u32 platform;
	u32 data;
	u32 mask;
} fam10_pci_default[] = {

	/* Function 0 - HT Config */
	{ 0, 0x68, (AMD_FAM10_ALL | AMD_FAM15_ALL), AMD_PTYPE_ALL,
	  0x000e0000, 0x000e0000 },		/* [19:17] for 8bit APIC config */

	{ 0, 0x68, (AMD_FAM10_ALL | AMD_FAM15_ALL), AMD_PTYPE_ALL,
	  0x00400000, 0x00600000 },		/* [22:21] DsNpReqLmt = 10b */

	{ 0, 0x68, AMD_FAM10_LT_D, AMD_PTYPE_ALL,
	  0x00004000, 0x00006000 },		/* [14:13] BufRelPri = 2h */

	{ 0, 0x68, (AMD_FAM10_REV_D | AMD_FAM15_ALL), AMD_PTYPE_ALL,
	  0x00002000, 0x00006000 },		/* [14:13] BufRelPri = 1h */

	{ 0, 0x68, (AMD_FAM10_ALL | AMD_FAM15_ALL), AMD_PTYPE_ALL,
	  0x00000800, 0x00000800 },		/* [11] RspPassPW = 1 */

	/* Errata 281 Workaround */
	{ 0, 0x68, (AMD_DR_B0 | AMD_DR_B1),
	  AMD_PTYPE_SVR, 0x00200000, 0x00600000 },	/* [22:21] DsNpReqLmt0 = 01b */

	{ 0, 0x84, AMD_FAM10_ALL, AMD_PTYPE_ALL,
	  0x00002000, 0x00002000 },	/* [13] LdtStopTriEn = 1 */

	{ 0, 0xA4, AMD_FAM10_ALL, AMD_PTYPE_ALL,
	  0x00002000, 0x00002000 },	/* [13] LdtStopTriEn = 1 */

	{ 0, 0xC4, AMD_FAM10_ALL, AMD_PTYPE_ALL,
	  0x00002000, 0x00002000 },	/* [13] LdtStopTriEn = 1 */

	{ 0, 0xE4, AMD_FAM10_ALL, AMD_PTYPE_ALL,
	  0x00002000, 0x00002000 },	/* [13] LdtStopTriEn = 1 */

	/* Link Global Retry Control Register */
	{ 0, 0x150, (AMD_FAM10_ALL | AMD_FAM15_ALL), AMD_PTYPE_ALL,
	  0x00073900, 0x00073f70 },	/* TotalRetryAttempts = 0x7,
					   HtRetryCrcDatInsDynEn = 0x1,
					   HtRetryCrcCmdPackDynEn = 0x1,
					   HtRetryCrcDatIns = 0x4,
					   HtRetryCrcCmdPack = 0x1,
					   ForceErrType = 0x0,
					   MultRetryErr = 0x0 */

	/* Errata 600 */
	{ 0, 0x150, AMD_OR_B2, AMD_PTYPE_ALL,
	  0x00000000, 0x00000e00 },	/* HtRetryCrcDatIns = 0x0 */

	/*  Errata 351
	 * System software should program the Link Extended Control Registers[LS2En]
	 * (F0x[18C:170][8]) to 0b for all links. System software should also
	 * program Link Global Extended Control Register[ForceFullT0]
	 * (F0x16C[15:13]) to 000b */

	{ 0, 0x170, AMD_FAM10_ALL, AMD_PTYPE_ALL, /* Fix FAM10_ALL when fixed in rev guide */
	  0x00000000, 0x00000100 },
	{ 0, 0x174, AMD_FAM10_ALL, AMD_PTYPE_ALL,
	  0x00000000, 0x00000100 },
	{ 0, 0x178, AMD_FAM10_ALL, AMD_PTYPE_ALL,
	  0x00000000, 0x00000100 },
	{ 0, 0x17C, AMD_FAM10_ALL, AMD_PTYPE_ALL,
	  0x00000000, 0x00000100 },
	{ 0, 0x180, AMD_FAM10_ALL, AMD_PTYPE_ALL,
	  0x00000000, 0x00000100 },
	{ 0, 0x184, AMD_FAM10_ALL, AMD_PTYPE_ALL,
	  0x00000000, 0x00000100 },
	{ 0, 0x188, AMD_FAM10_ALL, AMD_PTYPE_ALL,
	  0x00000000, 0x00000100 },
	{ 0, 0x18C, AMD_FAM10_ALL, AMD_PTYPE_ALL,
	  0x00000000, 0x00000100 },

	/* Link Global Extended Control Register */
	{ 0, 0x16C, AMD_FAM10_ALL, AMD_PTYPE_ALL,
	  0x00000014, 0x0000003F },	/* [15:13] ForceFullT0 = 0b,
					 * Set T0Time 14h per BKDG */

	{ 0, 0x170, AMD_FAM15_ALL, AMD_PTYPE_ALL,
	  0x00000100, 0x00000100 },
	{ 0, 0x174, AMD_FAM15_ALL, AMD_PTYPE_ALL,
	  0x00000100, 0x00000100 },
	{ 0, 0x178, AMD_FAM15_ALL, AMD_PTYPE_ALL,
	  0x00000100, 0x00000100 },
	{ 0, 0x17C, AMD_FAM15_ALL, AMD_PTYPE_ALL,
	  0x00000100, 0x00000100 },
	{ 0, 0x180, AMD_FAM15_ALL, AMD_PTYPE_ALL,
	  0x00000100, 0x00000100 },
	{ 0, 0x184, AMD_FAM15_ALL, AMD_PTYPE_ALL,
	  0x00000100, 0x00000100 },
	{ 0, 0x188, AMD_FAM15_ALL, AMD_PTYPE_ALL,
	  0x00000100, 0x00000100 },
	{ 0, 0x18C, AMD_FAM15_ALL, AMD_PTYPE_ALL,
	  0x00000100, 0x00000100 },

	/* Link Global Extended Control Register */
	{ 0, 0x16C, AMD_FAM15_ALL, AMD_PTYPE_ALL,
	  0x00000014, 0x0000003F },	/* [15:13] ForceFullT0 = 111b,
					 * Set T0Time 26h per BKDG */

	{ 0, 0x16C, AMD_FAM15_ALL, AMD_PTYPE_ALL,
	  0x7 << 13, 0x7 << 13 },	/* [15:13] ForceFullT0 = 7h */

	{ 0, 0x16C, AMD_FAM15_ALL, AMD_PTYPE_ALL,
	  0x26, 0x3f },	/* [5:0] T0Time = 26h */


	/* Function 1 - Map Init */

	/* Before reading F1x114_x2 or F1x114_x3 software must
	 * initialize the registers or NB Array MCA errors may
	 * occur.  BIOS should initialize index 0h of F1x114_x2 and
	 * F1x114_x3 to prevent reads from F1x114 from generating NB
	 * Array MCA errors.  BKDG Doc #3116 Rev 1.07
	 */

	{ 1, 0x110, AMD_FAM10_ALL, AMD_PTYPE_ALL,
	  0x20000000, 0xFFFFFFFF },	/* Select extended MMIO Base */

	{ 1, 0x114, AMD_FAM10_ALL, AMD_PTYPE_ALL,
	  0x00000000, 0xFFFFFFFF },	/* Clear map  */

	{ 1, 0x110, AMD_FAM10_ALL, AMD_PTYPE_ALL,
	  0x30000000, 0xFFFFFFFF },	/* Select extended MMIO Base */

	{ 1, 0x114, AMD_FAM10_ALL, AMD_PTYPE_ALL,
	  0x00000000, 0xFFFFFFFF },	/* Clear map  */

	/* Function 2 - DRAM Controller */

	/* Function 3 - Misc. Control */
	{ 3, 0x40, (AMD_FAM10_ALL | AMD_FAM15_ALL), AMD_PTYPE_ALL,
	  0x00000100, 0x00000100 },	/* [8] MstrAbrtEn */

	{ 3, 0x44, (AMD_FAM10_ALL | AMD_FAM15_ALL), AMD_PTYPE_ALL,
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
	{ 3, 0x6c, AMD_FAM10_ALL & ~(AMD_DR_Dx), AMD_PTYPE_ALL,
	  0x00018052, 0x700780f7 },	/* IsocRspDBC = 0x0,
					   UpRspDBC = 0x1,
					   DatBuf24 = 0x1,
					   DnRspDBC = 0x1,
					   DnReqDBC = 0x1,
					   UpReqDBC = 0x2 */

	/* XBAR buffer settings */
	{ 3, 0x6c, AMD_DR_Dx, AMD_PTYPE_ALL,
	  0x00028052, 0x700780f7 },	/* IsocRspDBC = 0x0,
					   UpRspDBC = 0x2,
					   DatBuf24 = 0x1,
					   DnRspDBC = 0x1,
					   DnReqDBC = 0x1,
					   UpReqDBC = 0x2 */

	/* XBAR buffer settings */
	{ 3, 0x6c, AMD_FAM15_ALL, AMD_PTYPE_ALL,
	  0x10010052, 0x700700f7 },	/* IsocRspDBC = 0x1,
					   UpRspDBC = 0x1,
					   DnRspDBC = 0x1,
					   DnReqDBC = 0x1,
					   UpReqDBC = 0x2 */

	/* Errata 281 Workaround */
	{ 3, 0x6c, (AMD_DR_B0 | AMD_DR_B1),
	  AMD_PTYPE_SVR, 0x00010094, 0x700780F7 },

	{ 3, 0x6c, AMD_FAM10_ALL, AMD_PTYPE_UMA,
	  0x60018051, 0x700780F7 },

	{ 3, 0x70, AMD_FAM10_ALL & ~(AMD_DR_Dx), AMD_PTYPE_ALL,
	  0x00041153, 0x777777f7 },	/* IsocRspCBC = 0x0,
					   IsocPreqCBC = 0x0,
					   IsocReqCBC = 0x0,
					   UpRspCBC = 0x4,
					   DnPreqCBC = 0x1,
					   UpPreqCBC = 0x1,
					   DnRspCBC = 0x1,
					   DnReqCBC = 0x1,
					   UpReqCBC = 0x3 */

	{ 3, 0x70, AMD_DR_Dx, AMD_PTYPE_ALL,
	  0x00051153, 0x777777f7 },	/* IsocRspCBC = 0x0,
					   IsocPreqCBC = 0x0,
					   IsocReqCBC = 0x0,
					   UpRspCBC = 0x5,
					   DnPreqCBC = 0x1,
					   UpPreqCBC = 0x1,
					   DnRspCBC = 0x1,
					   DnReqCBC = 0x1,
					   UpReqCBC = 0x3 */

	{ 3, 0x70, AMD_FAM15_ALL, AMD_PTYPE_ALL,
	  0x10171155, 0x777777f7 },	/* IsocRspCBC = 0x1,
					   IsocPreqCBC = 0x0,
					   IsocReqCBC = 0x1,
					   UpRspCBC = 0x7,
					   DnPreqCBC = 0x1,
					   UpPreqCBC = 0x1,
					   DnRspCBC = 0x1,
					   DnReqCBC = 0x1,
					   UpReqCBC = 0x5 */

	{ 3, 0x70, AMD_FAM10_ALL, AMD_PTYPE_UMA,
	  0x61221151, 0x777777f7 },	/* IsocRspCBC = 0x6,
					   IsocPreqCBC = 0x1,
					   IsocReqCBC = 0x2,
					   UpRspCBC = 0x2,
					   DnPreqCBC = 0x1,
					   UpPreqCBC = 0x1,
					   DnRspCBC = 0x1,
					   DnReqCBC = 0x1,
					   UpReqCBC = 0x1 */

	{ 3, 0x74, AMD_FAM10_ALL, ~AMD_PTYPE_UMA,
	  0x00081111, 0xf7ff7777 },	/* DRReqCBC = 0x0,
					   IsocPreqCBC = 0x0,
					   IsocReqCBC = 0x0,
					   ProbeCBC = 0x8,
					   DnPreqCBC = 0x1,
					   UpPreqCBC = 0x1,
					   DnReqCBC = 0x1,
					   UpReqCBC = 0x1 */

	{ 3, 0x74, AMD_FAM10_ALL, AMD_PTYPE_UMA,
	  0x00480101, 0xf7ff7777 },	/* DRReqCBC = 0x0,
					   IsocPreqCBC = 0x0,
					   IsocReqCBC = 0x4,
					   ProbeCBC = 0x8,
					   DnPreqCBC = 0x0,
					   UpPreqCBC = 0x1,
					   DnReqCBC = 0x0,
					   UpReqCBC = 0x1 */

	{ 3, 0x74, AMD_FAM15_ALL, AMD_PTYPE_ALL,
	  0x00172111, 0xf7ff7777 },	/* DRReqCBC = 0x0,
					   IsocPreqCBC = 0x0,
					   IsocReqCBC = 0x1,
					   ProbeCBC = 0x7,
					   DnPreqCBC = 0x2,
					   UpPreqCBC = 0x1,
					   DnReqCBC = 0x1,
					   UpReqCBC = 0x1 */

	{ 3, 0x7c, AMD_FAM10_ALL & ~(AMD_DR_Dx), AMD_PTYPE_ALL,
	 0x00090914, 0x707fff1f },	/* XBar2SriFreeListCBInc = 0x0,
					   Sri2XbarFreeRspDBC = 0x0,
					   Sri2XbarFreeXreqDBC = 0x9,
					   Sri2XbarFreeRspCBC = 0x0,
					   Sri2XbarFreeXreqCBC = 0x9,
					   Xbar2SriFreeListCBC = 0x14 */

	{ 3, 0x7c, AMD_DR_Dx, AMD_PTYPE_ALL,
	 0x00090a18, 0x707fff1f },	/* XBar2SriFreeListCBInc = 0x0,
					   Sri2XbarFreeRspDBC = 0x0,
					   Sri2XbarFreeXreqDBC = 0x9,
					   Sri2XbarFreeRspCBC = 0x0,
					   Sri2XbarFreeXreqCBC = 0x9,
					   Xbar2SriFreeListCBC = 0x14 */

	/* Errata 281 Workaround */
	{ 3, 0x7C, ( AMD_DR_B0 | AMD_DR_B1),
	 AMD_PTYPE_SVR, 0x00144514, 0x707FFF1F },

	{ 3, 0x7c, AMD_FAM15_ALL, AMD_PTYPE_ALL,
	  0x040d0f16, 0x77ffff1f },	/* XBar2SriFreeListCBInc = 0x0,
					   SrqExtFreeListBC = 0x8,
					   Sri2XbarFreeRspDBC = 0x0,
					   Sri2XbarFreeXreqDBC = 0xd,
					   Sri2XbarFreeRspCBC = 0x0,
					   Sri2XbarFreeXreqCBC = 0xf,
					   Xbar2SriFreeListCBC = 0x16 */

	{ 3, 0x7C, AMD_FAM10_ALL, AMD_PTYPE_UMA,
	  0x00070814, 0x007FFF1F },

	{ 3, 0x140, AMD_FAM10_ALL, AMD_PTYPE_ALL,
	  0x00800756, 0x00F3FFFF },

	{ 3, 0x140, AMD_FAM10_ALL, AMD_PTYPE_UMA,
	  0x00C37756, 0x00F3FFFF },

	{ 3, 0x144, AMD_FAM10_ALL, AMD_PTYPE_UMA,
	  0x00000036, 0x000000FF },

	{ 3, 0x140, AMD_FAM15_ALL, AMD_PTYPE_ALL,
	  0x00a11755, 0x00f3ffff },

	/* Errata 281 Workaround */
	{ 3, 0x144, ( AMD_DR_B0 | AMD_DR_B1),
	 AMD_PTYPE_SVR, 0x00000001, 0x0000000F },
		/* [3:0] RspTok = 0001b */

	{ 3, 0x144, AMD_FAM15_ALL, AMD_PTYPE_ALL,
	  0x00000028, 0x000000ff },

	{ 3, 0x148, AMD_FAM10_ALL, AMD_PTYPE_UMA,
	  0x8000052A, 0xD5FFFFFF },

	/* Core Interface Buffer Count */
	{ 3, 0x1a0, AMD_FAM15_ALL, AMD_PTYPE_ALL,
	  0x00034004, 0x00037007 },	/* CpuToNbFreeBufCnt = 0x3,
					   L3ToSriReqCBC = 0x4,
					   L3FreeListCBC = default,
					   CpuCmdBufCnt = 0x4 */

	/* ACPI Power State Control Reg1 */
	{ 3, 0x80, AMD_FAM10_ALL, AMD_PTYPE_ALL,
	  0xE6002200, 0xFFFFFFFF },

	/* ACPI Power State Control Reg1 */
	{ 3, 0x80, AMD_FAM15_ALL, AMD_PTYPE_ALL,
	  0xe20be200, 0xefefef00 },

	/* ACPI Power State Control Reg2 */
	{ 3, 0x84, AMD_FAM10_ALL, AMD_PTYPE_ALL,
	  0xA0E641E6, 0xFFFFFFFF },

	/* ACPI Power State Control Reg2 */
	{ 3, 0x84, AMD_FAM15_ALL, AMD_PTYPE_ALL,
	  0x01e200e2, 0xefef00ef },

	{ 3, 0xA0, AMD_FAM10_ALL, AMD_PTYPE_MOB | AMD_PTYPE_DSK,
	  0x00000080, 0x00000080 },	/* [7] PSIVidEnable */

	{ 3, 0xA0, AMD_DR_Bx, AMD_PTYPE_ALL,
	  0x00002800, 0x000003800 },	/* [13:11] PllLockTime = 5 */

	{ 3, 0xA0, ((AMD_FAM10_ALL | AMD_FAM15_ALL) & ~(AMD_DR_Bx)), AMD_PTYPE_ALL,
	  0x00000800, 0x000003800 },	/* [13:11] PllLockTime = 1 */

	/* Reported Temp Control Register */
	{ 3, 0xA4, (AMD_FAM10_ALL | AMD_FAM15_ALL), AMD_PTYPE_ALL,
	  0x00000080, 0x00000080 },	/* [7] TempSlewDnEn = 1 */

	/* Clock Power/Timing Control 0 Register */
	{ 3, 0xD4, (AMD_FAM10_ALL | AMD_FAM15_ALL), AMD_PTYPE_ALL,
	  0xC0000F00, 0xF0000F00 },	/*  [31] NbClkDivApplyAll = 1,
		[30:28] NbClkDiv = 100b,[11:8] ClkRampHystSel = 1111b */

	/* Clock Power/Timing Control 1 Register */
	{ 3, 0xD8, (AMD_FAM10_ALL | AMD_FAM15_ALL), AMD_PTYPE_ALL,
	  0x03000010, 0x0F000070 },	/* [6:4] VSRampTime = 1,
					 * [27:24] ReConDel = 3 */

	/* Clock Power/Timing Control 1 Register */
	{ 3, 0xD8, AMD_FAM10_ALL, AMD_PTYPE_ALL,
	  0x00000006, 0x00000007 },	/* [2:0] VSSlamTime = 6 */


	/* Clock Power/Timing Control 2 Register */
	{ 3, 0xDC, (AMD_FAM10_ALL | AMD_FAM15_ALL), AMD_PTYPE_ALL,
	  0x00005000, 0x00007000 },	/* [14:12] NbsynPtrAdj = 5 */


	/* Extended NB MCA Config Register */
	{ 3, 0x180, (AMD_FAM10_ALL | AMD_FAM15_ALL), AMD_PTYPE_ALL,
	  0x007003E2, 0x007003E2 },	/* [22:20] = SyncFloodOn_Err = 7,
					   [9] SyncOnUncNbAryEn = 1 ,
					   [8] SyncOnProtEn = 1,
					   [7] SyncFloodOnTgtAbtErr = 1,
					   [6] SyncFloodOnDatErr = 1,
					   [5] DisPciCfgCpuMstAbtRsp = 1,
					   [1] SyncFloodOnUsPwDataErr = 1 */

	/* NB Configuration 2 */
	{ 3, 0x188, AMD_DR_GT_B0, AMD_PTYPE_ALL,
	  0x00000010, 0x00000010 },	/* EnStpGntOnFlushMaskWakeup = 0x1 */

	/* NB Configuration 2 */
	{ 3, 0x188, AMD_FAM15_ALL, AMD_PTYPE_ALL,
	  0x00000200, 0x00000200 },	/* DisL3HiPriFreeListAlloc = 0x1 */

	/* errata 346 - Fam10 C2, C3
	 *  System software should set F3x188[22] to 1b. */
	{ 3, 0x188, AMD_DR_Cx, AMD_PTYPE_ALL,
	  0x00400000, 0x00400000 },

	/* L3 Control Register */
	{ 3, 0x1b8, (AMD_FAM10_ALL | AMD_FAM15_ALL), AMD_PTYPE_ALL,
	  0x00001000, 0x00001000 },	/* [12] = L3PrivReplEn */

	/* Errata 504 workaround */
	{ 3, 0x1b8, AMD_FAM15_ALL, AMD_PTYPE_ALL,
	  0x00040000, 0x00040000 },	/* [18] = 1b */

	/* IBS Control Register */
	{ 3, 0x1cc, (AMD_FAM10_ALL | AMD_FAM15_ALL), AMD_PTYPE_ALL,
	  0x00000100, 0x00000100 },	/* [8] = LvtOffsetVal */

	/* Erratum 619 - Family 15h Bx
	 * System software should set F5x88[14] to 1b. */
	{ 5, 0x88, AMD_OR_B2, AMD_PTYPE_ALL,
	  1 << 14, 1 << 14 },

	/* L3 Control 2 */
	{ 3, 0x1b8, AMD_FAM15_ALL, AMD_PTYPE_ALL,
	  0x00000090, 0x000001d0 },	/* ImplRdProjDelayThresh = 0x2,
					   ImplRdAnySubUnavail = 0x1 */
};


/*
 * Default HyperTransport Phy and errata settings.
 */
static const struct {
	u16 htreg;	/* HT Phy Register index */
	uint64_t revision;
	u32 platform;
	u32 linktype;
	u32 data;
	u32 mask;
} fam10_htphy_default[] = {

	/* Errata 344 - Fam10 C2/C3, D0/D1
	 * System software should set bit 6 of F4x1[9C, 94, 8C, 84]_x[78:70, 68:60]. */
	{ 0x60, AMD_DR_Cx | AMD_DR_Dx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00000040, 0x00000040 },
	{ 0x61, AMD_DR_Cx | AMD_DR_Dx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00000040, 0x00000040 },
	{ 0x62, AMD_DR_Cx | AMD_DR_Dx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00000040, 0x00000040 },
	{ 0x63, AMD_DR_Cx | AMD_DR_Dx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00000040, 0x00000040 },
	{ 0x64, AMD_DR_Cx | AMD_DR_Dx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00000040, 0x00000040 },
	{ 0x65, AMD_DR_Cx | AMD_DR_Dx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00000040, 0x00000040 },
	{ 0x66, AMD_DR_Cx | AMD_DR_Dx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00000040, 0x00000040 },
	{ 0x67, AMD_DR_Cx | AMD_DR_Dx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00000040, 0x00000040 },
	{ 0x68, AMD_DR_Cx | AMD_DR_Dx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00000040, 0x00000040 },

	{ 0x70, AMD_DR_Cx | AMD_DR_Dx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00000040, 0x00000040 },
	{ 0x71, AMD_DR_Cx | AMD_DR_Dx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00000040, 0x00000040 },
	{ 0x72, AMD_DR_Cx | AMD_DR_Dx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00000040, 0x00000040 },
	{ 0x73, AMD_DR_Cx | AMD_DR_Dx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00000040, 0x00000040 },
	{ 0x74, AMD_DR_Cx | AMD_DR_Dx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00000040, 0x00000040 },
	{ 0x75, AMD_DR_Cx | AMD_DR_Dx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00000040, 0x00000040 },
	{ 0x76, AMD_DR_Cx | AMD_DR_Dx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00000040, 0x00000040 },
	{ 0x77, AMD_DR_Cx | AMD_DR_Dx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00000040, 0x00000040 },
	{ 0x78, AMD_DR_Cx | AMD_DR_Dx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00000040, 0x00000040 },

	/* Errata 354 - Fam10 C2, C3
	 * System software should set bit 6 of F4x1[9C,94,8C,84]_x[58:50, 48:40] for all links. */
	{ 0x40, AMD_DR_Cx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00000040, 0x00000040 },
	{ 0x41, AMD_DR_Cx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00000040, 0x00000040 },
	{ 0x42, AMD_DR_Cx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00000040, 0x00000040 },
	{ 0x43, AMD_DR_Cx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00000040, 0x00000040 },
	{ 0x44, AMD_DR_Cx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00000040, 0x00000040 },
	{ 0x45, AMD_DR_Cx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00000040, 0x00000040 },
	{ 0x46, AMD_DR_Cx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00000040, 0x00000040 },
	{ 0x47, AMD_DR_Cx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00000040, 0x00000040 },
	{ 0x48, AMD_DR_Cx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00000040, 0x00000040 },

	{ 0x50, AMD_DR_Cx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00000040, 0x00000040 },
	{ 0x51, AMD_DR_Cx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00000040, 0x00000040 },
	{ 0x52, AMD_DR_Cx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00000040, 0x00000040 },
	{ 0x53, AMD_DR_Cx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00000040, 0x00000040 },
	{ 0x54, AMD_DR_Cx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00000040, 0x00000040 },
	{ 0x55, AMD_DR_Cx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00000040, 0x00000040 },
	{ 0x56, AMD_DR_Cx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00000040, 0x00000040 },
	{ 0x57, AMD_DR_Cx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00000040, 0x00000040 },
	{ 0x58, AMD_DR_Cx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00000040, 0x00000040 },

	/* Errata 327 - Fam10 C2/C3, D0/D1
	 * BIOS should set the Link Phy Impedance Register[RttCtl]
	 * (F4x1[9C, 94, 8C, 84]_x[D0, C0][31:29]) to 010b and
	 * Link Phy Impedance Register[RttIndex]
	 * (F4x1[9C, 94, 8C, 84]_x[D0, C0][20:16]) to 00100b */
	{ 0xC0, AMD_DR_Cx | AMD_DR_Dx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x40040000, 0xe01F0000 },
	{ 0xD0, AMD_DR_Cx | AMD_DR_Dx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x40040000, 0xe01F0000 },

	{ 0x520A,AMD_DR_Cx | AMD_DR_Dx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00004000, 0x00006000 },	/* HT_PHY_DLL_REG */

	{ 0x530A, AMD_DR_Cx | AMD_DR_Dx, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00004000, 0x00006000 },	/* HT_PHY_DLL_REG */

	{ 0x520A, AMD_DR_ALL, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00004400, 0x00006400 },	/* HT_PHY_DLL_REG */

	{ 0x530A, AMD_DR_ALL, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x00004400, 0x00006400 },	/* HT_PHY_DLL_REG */

	{ 0xCF, AMD_FAM10_ALL, AMD_PTYPE_ALL, HTPHY_LINKTYPE_HT3,
	  0x0000005a, 0x000000ff },	/* Use common "safe" setting for K10 */

	{ 0xDF, AMD_FAM10_ALL, AMD_PTYPE_ALL, HTPHY_LINKTYPE_HT3,
	  0x0000005a, 0x000000ff },	/* Use common "safe" setting for K10 */

	{ 0xCF, AMD_FAM10_ALL, AMD_PTYPE_ALL, HTPHY_LINKTYPE_HT1,
	  0x0000006d, 0x000000ff },	/* HT_PHY_HT1_FIFO_PTR_OPT_VALUE */

	{ 0xDF, AMD_FAM10_ALL, AMD_PTYPE_ALL,  HTPHY_LINKTYPE_HT1,
	  0x0000006d, 0x000000ff },	/* HT_PHY_HT1_FIFO_PTR_OPT_VALUE */

	/* Link Phy Receiver Loop Filter Registers */
	{ 0xD1, AMD_FAM10_ALL, AMD_PTYPE_ALL, HTPHY_LINKTYPE_HT3,
	  0x08040000, 0x3FFFC000 },	/* [29:22] LfcMax = 20h,
					   [21:14] LfcMin = 10h */

	{ 0xC1, AMD_FAM10_ALL, AMD_PTYPE_ALL, HTPHY_LINKTYPE_HT3,
	  0x08040000, 0x3FFFC000 },	/* [29:22] LfcMax = 20h,
					   [21:14] LfcMin = 10h */

	{ 0xD1, AMD_FAM10_ALL, AMD_PTYPE_ALL, HTPHY_LINKTYPE_HT1,
	  0x04020000, 0x3FFFC000 },	/* [29:22] LfcMax = 10h,
					   [21:14] LfcMin = 08h */

	{ 0xC1, AMD_FAM10_ALL, AMD_PTYPE_ALL, HTPHY_LINKTYPE_HT1,
	  0x04020000, 0x3FFFC000 },	/* [29:22] LfcMax = 10h,
					   [21:14] LfcMin = 08h */

	{ 0xC0, AMD_FAM10_ALL, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x40040000, 0xe01F0000 },	/* [31:29] RttCtl = 02h,
					  [20:16] RttIndex = 04h */

	{ 0xCF, AMD_FAM15_ALL, AMD_PTYPE_ALL, HTPHY_LINKTYPE_HT3,
	  0x00000a2a, 0x000000ff },	/* P0RcvRdPtr = 0xa,
					   P0XmtRdPtr = 0x2
					   P1RcvRdPtr = 0xa
					   P1XmtRdPtr = 0x0 */

	{ 0xDF, AMD_FAM15_ALL, AMD_PTYPE_ALL, HTPHY_LINKTYPE_HT3,
	  0x00000a2a, 0x000000ff },	/* P0RcvRdPtr = 0xa,
					   P0XmtRdPtr = 0x2
					   P1RcvRdPtr = 0xa
					   P1XmtRdPtr = 0x0 */

	{ 0xCF, AMD_FAM15_ALL, AMD_PTYPE_ALL, HTPHY_LINKTYPE_HT1,
	  0x00000d4d, 0x000000ff },	/* P0RcvRdPtr = 0xd,
					   P0XmtRdPtr = 0x4
					   P1RcvRdPtr = 0xd
					   P1XmtRdPtr = 0x0 */

	{ 0xDF, AMD_FAM15_ALL, AMD_PTYPE_ALL,  HTPHY_LINKTYPE_HT1,
	  0x00000d4d, 0x000000ff },	/* P0RcvRdPtr = 0xd,
					   P0XmtRdPtr = 0x4
					   P1RcvRdPtr = 0xd
					   P1XmtRdPtr = 0x0 */

	/* Link Phy Receiver Loop Filter Registers */
	{ 0xD1, AMD_FAM15_ALL, AMD_PTYPE_ALL, HTPHY_LINKTYPE_HT3,
	  0x08040000, 0x3FFFC000 },	/* [29:22] LfcMax = 20h,
					   [21:14] LfcMin = 10h */

	{ 0xC1, AMD_FAM15_ALL, AMD_PTYPE_ALL, HTPHY_LINKTYPE_HT3,
	  0x08040000, 0x3FFFC000 },	/* [29:22] LfcMax = 20h,
					   [21:14] LfcMin = 10h */

	{ 0xD1, AMD_FAM15_ALL, AMD_PTYPE_ALL, HTPHY_LINKTYPE_HT1,
	  0x04020000, 0x3FFFC000 },	/* [29:22] LfcMax = 10h,
					   [21:14] LfcMin = 08h */

	{ 0xC1, AMD_FAM15_ALL, AMD_PTYPE_ALL, HTPHY_LINKTYPE_HT1,
	  0x04020000, 0x3FFFC000 },	/* [29:22] LfcMax = 10h,
					   [21:14] LfcMin = 08h */

	{ 0xC0, AMD_FAM15_ALL, AMD_PTYPE_ALL, HTPHY_LINKTYPE_ALL,
	  0x40040000, 0xe01F0000 },	/* [31:29] RttCtl = 02h,
					   [20:16] RttIndex = 04h */

	{ 0xc4, AMD_FAM15_ALL, AMD_PTYPE_ALL, HTPHY_LINKTYPE_HT3,
	  0x00013480, 0x0003fc80 },	/* [17:10] DCV = 0x4d,
					       [7] DfeEn = 0x1 */

	{ 0xd4, AMD_FAM15_ALL, AMD_PTYPE_ALL, HTPHY_LINKTYPE_HT3,
	  0x00013480, 0x0003fc80 },     /* [17:10] DCV = 0x4d,
					       [7] DfeEn = 0x1 */
};
