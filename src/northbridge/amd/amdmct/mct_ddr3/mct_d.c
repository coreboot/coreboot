/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015-2017 Raptor Engineering, LLC
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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

/* Description: Main memory controller system configuration for DDR 3 */

/* KNOWN ISSUES - ERRATA
 *
 * Trtp is not calculated correctly when the controller is in 64-bit mode, it
 * is 1 busclock off.	No fix planned.	 The controller is not ordinarily in
 * 64-bit mode.
 *
 * 32 Byte burst not supported. No fix planned. The controller is not
 * ordinarily in 64-bit mode.
 *
 * Trc precision does not use extra Jedec defined fractional component.
 * InsteadTrc (course) is rounded up to nearest 1 ns.
 *
 * Mini and Micro DIMM not supported. Only RDIMM, UDIMM, SO-DIMM defined types
 * supported.
 */

#include "mct_d_gcc.h"
#include "mct_d.h"
#include <console/console.h>
#include <northbridge/amd/amdfam10/debug.h>
#include <northbridge/amd/amdfam10/raminit.h>
#include <northbridge/amd/amdfam10/amdfam10.h>
#include <reset.h>
#include <cpu/x86/msr.h>
#include <arch/acpi.h>
#include <string.h>
#include <device/dram/ddr3.h>
#include "s3utils.h"

static u8 ReconfigureDIMMspare_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstatA);
static void DQSTiming_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstatA,
				uint8_t allow_config_restore);
static void LoadDQSSigTmgRegs_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstatA);
static void HTMemMapInit_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstatA);
static void SyncDCTsReady_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstatA);
static void ClearDCT_D(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, u8 dct);
static u8 AutoCycTiming_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct);
static void GetPresetmaxF_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat);
static void SPDGetTCL_D(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, u8 dct);
static u8 AutoConfig_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct);
static void SPDSetBanks_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct);
static void StitchMemory_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct);
static u16 Get_Fk_D(u8 k);
static u8 Get_DIMMAddress_D(struct DCTStatStruc *pDCTstat, u8 i);
static void mct_preInitDCT(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat);
static void mct_initDCT(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat);
static void mct_DramInit(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct);
static void mct_SyncDCTsReady(struct DCTStatStruc *pDCTstat);
static void Get_Trdrd(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, u8 dct);
static void mct_AfterGetCLT(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct);
static u8 mct_SPDCalcWidth(struct MCTStatStruc *pMCTstat,\
					struct DCTStatStruc *pDCTstat, u8 dct);
static void mct_AfterStitchMemory(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct);
static u8 mct_DIMMPresence(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct);
static void Set_OtherTiming(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct);
static void Get_Twrwr(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, u8 dct);
static void Get_Twrrd(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, u8 dct);
static void Get_TrwtTO(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, u8 dct);
static void Get_TrwtWB(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat);
static void Get_DqsRcvEnGross_Diff(struct DCTStatStruc *pDCTstat,
					u32 dev, uint8_t dct, u32 index_reg);
static void Get_WrDatGross_Diff(struct DCTStatStruc *pDCTstat, u8 dct,
					u32 dev, u32 index_reg);
static u16 Get_DqsRcvEnGross_MaxMin(struct DCTStatStruc *pDCTstat,
				u32 dev, uint8_t dct, u32 index_reg, u32 index);
static void mct_FinalMCT_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat);
static u16 Get_WrDatGross_MaxMin(struct DCTStatStruc *pDCTstat, u8 dct,
				u32 dev, u32 index_reg, u32 index);
static void mct_InitialMCT_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat);
static void mct_init(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat);
static void clear_legacy_Mode(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat);
static void mct_HTMemMapExt(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstatA);
static void SetCSTriState(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct);
static void SetCKETriState(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct);
static void SetODTTriState(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct);
static void InitDDRPhy(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct);
static u32 mct_NodePresent_D(void);
static void mct_OtherTiming(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstatA);
static void mct_ResetDataStruct_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstatA);
static void mct_EarlyArbEn_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct);
static void mct_BeforeDramInit_Prod_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct);
static void mct_ProgramODT_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct);
void mct_ClrClToNB_D(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat);
static u8 CheckNBCOFEarlyArbEn(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat);
void mct_ClrWbEnhWsbDis_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat);
static void mct_BeforeDQSTrain_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstatA);
static void AfterDramInit_D(struct DCTStatStruc *pDCTstat, u8 dct);
static void mct_ResetDLL_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct);
static u32 mct_DisDllShutdownSR(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u32 DramConfigLo, u8 dct);
static void mct_EnDllShutdownSR(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct);
void ChangeMemClk(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat);

static u8 Get_Latency_Diff(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct);
static void SyncSetting(struct DCTStatStruc *pDCTstat);
static uint8_t crcCheck(struct DCTStatStruc *pDCTstat, uint8_t dimm);

uint8_t is_ecc_enabled(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat);

/*See mctAutoInitMCT header for index relationships to CL and T*/
static const u16 Table_F_k[]	= {00,200,266,333,400,533 };
static const u8 Tab_BankAddr[]	= {0x3F,0x01,0x09,0x3F,0x3F,0x11,0x0A,0x19,0x12,0x1A,0x21,0x22,0x23};
const u8 Table_DQSRcvEn_Offset[] = {0x00,0x01,0x10,0x11,0x2};

/****************************************************************************
   Describe how platform maps MemClk pins to logical DIMMs. The MemClk pins
   are identified based on BKDG definition of Fn2x88[MemClkDis] bitmap.
   AGESA will base on this value to disable unused MemClk to save power.

   If MEMCLK_MAPPING or MEMCLK_MAPPING contains all zeroes, AGESA will use
   default MemClkDis setting based on package type.

   Example:
   BKDG definition of Fn2x88[MemClkDis] bitmap for AM3 package is like below:
        Bit AM3/S1g3 pin name
        0   M[B,A]_CLK_H/L[0]
        1   M[B,A]_CLK_H/L[1]
        2   M[B,A]_CLK_H/L[2]
        3   M[B,A]_CLK_H/L[3]
        4   M[B,A]_CLK_H/L[4]
        5   M[B,A]_CLK_H/L[5]
        6   M[B,A]_CLK_H/L[6]
        7   M[B,A]_CLK_H/L[7]

   And platform has the following routing:
        CS0   M[B,A]_CLK_H/L[4]
        CS1   M[B,A]_CLK_H/L[2]
        CS2   M[B,A]_CLK_H/L[3]
        CS3   M[B,A]_CLK_H/L[5]

   Then:
                       ;    CS0        CS1        CS2        CS3        CS4        CS5        CS6        CS7
   MEMCLK_MAPPING  EQU    00010000b, 00000100b, 00001000b, 00100000b, 00000000b, 00000000b, 00000000b, 00000000b
*/

/* ==========================================================================================
 * Set up clock pin to DIMM mappings,
 * NOTE: If you are not sure about the pin mappings, you can keep all MemClk signals active,
 * just set all entries in the relevant table(s) to 0xff.
 * ==========================================================================================
 */
static const u8 Tab_L1CLKDis[]  = {0x20, 0x20, 0x10, 0x10, 0x08, 0x08, 0x04, 0x04};
static const u8 Tab_AM3CLKDis[] = {0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00};
static const u8 Tab_S1CLKDis[]  = {0xA2, 0xA2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/* C32: Enable CS0 - CS3 clocks (DIMM0 - DIMM1) */
static const u8 Tab_C32CLKDis[] = {0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00};

/* G34: Enable CS0 - CS3 clocks (DIMM0 - DIMM1) */
static const u8 Tab_G34CLKDis[] = {0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00};

/* FM2: Enable all the clocks for the dimms */
static const u8 Tab_FM2CLKDis[] = {0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00};

static const u8 Tab_ManualCLKDis[]= {0x10, 0x04, 0x08, 0x20, 0x00, 0x00, 0x00, 0x00};
/* ========================================================================================== */

static const u8 Table_Comp_Rise_Slew_20x[] = {7, 3, 2, 2, 0xFF};
static const u8 Table_Comp_Rise_Slew_15x[] = {7, 7, 3, 2, 0xFF};
static const u8 Table_Comp_Fall_Slew_20x[] = {7, 5, 3, 2, 0xFF};
static const u8 Table_Comp_Fall_Slew_15x[] = {7, 7, 5, 3, 0xFF};

uint8_t dct_ddr_voltage_index(struct DCTStatStruc *pDCTstat, uint8_t dct)
{
	uint8_t dimm;
	uint8_t ddr_voltage_index = 0;

	/* If no DIMMs are present on this DCT, report 1.5V operation and skip checking the hardware */
	if (pDCTstat->DIMMValidDCT[dct] == 0)
		return 0x1;

	/* Find current DDR supply voltage for this DCT */
	for (dimm = 0; dimm < MAX_DIMMS_SUPPORTED; dimm++) {
		if (pDCTstat->DIMMValidDCT[dct] & (1 << dimm))
			ddr_voltage_index |= pDCTstat->DimmConfiguredVoltage[dimm];
	}
	if (ddr_voltage_index > 0x7) {
		printk(BIOS_DEBUG, "%s: Insufficient DDR supply voltage indicated!  Configuring processor for 1.25V operation, but this attempt may fail...\n", __func__);
		ddr_voltage_index = 0x4;
	}
	if (ddr_voltage_index == 0x0) {
		printk(BIOS_DEBUG, "%s: No DDR supply voltage indicated!  Configuring processor for 1.5V operation, but this attempt may fail...\n", __func__);
		ddr_voltage_index = 0x1;
	}

	return ddr_voltage_index;
}

static uint16_t fam15h_mhz_to_memclk_config(uint16_t freq)
{
	uint16_t fam15h_freq_tab[] = {0, 0, 0, 0, 333, 0, 400, 0, 0, 0, 533, 0, 0, 0, 667, 0, 0, 0, 800, 0, 0, 0, 933};
	uint16_t iter;

	/* Compute the index value for the given frequency */
	for (iter = 0; iter <= 0x16; iter++) {
		if (fam15h_freq_tab[iter] == freq) {
			freq = iter;
			break;
		}
	}
	if (freq == 0)
		freq = 0x4;

	return freq;
}

static uint16_t fam10h_mhz_to_memclk_config(uint16_t freq)
{
	uint16_t fam10h_freq_tab[] = {0, 0, 0, 400, 533, 667, 800};
	uint16_t iter;

	/* Compute the index value for the given frequency */
	for (iter = 0; iter <= 0x6; iter++) {
		if (fam10h_freq_tab[iter] == freq) {
			freq = iter;
			break;
		}
	}
	if (freq == 0)
		freq = 0x3;

	return freq;
}

static inline uint8_t is_model10_1f(void)
{
	uint8_t model101f = 0;
	uint32_t family;

	family = cpuid_eax(0x80000001);
	family = ((family & 0x0ff000) >> 12);

	if (family >= 0x10 && family <= 0x1f)
		/* Model 0x10 to 0x1f */
		model101f = 1;

	return model101f;
}

uint8_t is_ecc_enabled(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat)
{
	uint8_t ecc_enabled = 1;

	if (!pMCTstat->try_ecc)
		ecc_enabled = 0;

	if (pDCTstat->NodePresent && (pDCTstat->DIMMValidDCT[0] || pDCTstat->DIMMValidDCT[1]))
		if (!(pDCTstat->Status & (1 << SB_ECCDIMMs)))
			ecc_enabled = 0;

	return !!ecc_enabled;
}

uint8_t get_available_lane_count(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat)
{
	if (is_ecc_enabled(pMCTstat, pDCTstat))
		return 9;
	else
		return 8;
}

uint16_t mhz_to_memclk_config(uint16_t freq)
{
	if (is_fam15h())
		return fam15h_mhz_to_memclk_config(freq);
	else
		return fam10h_mhz_to_memclk_config(freq) + 1;
}

uint32_t fam10h_address_timing_compensation_code(struct DCTStatStruc *pDCTstat, uint8_t dct)
{
	uint8_t MaxDimmsInstallable = mctGet_NVbits(NV_MAX_DIMMS_PER_CH);

	uint8_t package_type;
	uint32_t calibration_code = 0;

	package_type = mctGet_NVbits(NV_PACK_TYPE);
	uint16_t MemClkFreq = (Get_NB32_DCT(pDCTstat->dev_dct, dct, 0x94) & 0x7) + 1;

	/* Obtain number of DIMMs on channel */
	uint8_t dimm_count = pDCTstat->MAdimms[dct];
	uint8_t rank_count_dimm0;
	uint8_t rank_count_dimm1;

	if (package_type == PT_GR) {
		/* Socket G34 */
		if (pDCTstat->Status & (1 << SB_Registered)) {
			/* RDIMM */
			/* Fam10h BKDG Rev. 3.62 section 2.8.9.5.8 Tables 60 - 61 */
			if (MaxDimmsInstallable == 1) {
				if (MemClkFreq == 0x4) {
					/* DDR3-800 */
					calibration_code = 0x00000000;
				} else if (MemClkFreq == 0x5) {
					/* DDR3-1066 */
					calibration_code = 0x003c3c3c;
				} else if (MemClkFreq == 0x6) {
					/* DDR3-1333 */
					calibration_code = 0x003a3a3a;
				}
			} else if (MaxDimmsInstallable == 2) {
				if (dimm_count == 1) {
					/* 1 DIMM detected */
					if (MemClkFreq == 0x4) {
						/* DDR3-800 */
						calibration_code = 0x00000000;
					} else if (MemClkFreq == 0x5) {
						/* DDR3-1066 */
						calibration_code = 0x003c3c3c;
					} else if (MemClkFreq == 0x6) {
						/* DDR3-1333 */
						calibration_code = 0x003a3a3a;
					}
				} else if (dimm_count == 2) {
					/* 2 DIMMs detected */
					if (MemClkFreq == 0x4) {
						/* DDR3-800 */
						calibration_code = 0x00000000;
					} else if (MemClkFreq == 0x5) {
						/* DDR3-1066 */
						calibration_code = 0x003a3c3a;
					} else if (MemClkFreq == 0x6) {
						/* DDR3-1333 */
						calibration_code = 0x00383a38;
					}
				}
			} else if (MaxDimmsInstallable == 3) {
				/* TODO
				 * 3 DIMM/channel support unimplemented
				 */
			}
		} else {
			/* UDIMM */
			/* Fam10h BKDG Rev. 3.62 section 2.8.9.5.8 Table 56 */
			if (dimm_count == 1) {
				/* 1 DIMM detected */
				rank_count_dimm0 = pDCTstat->DimmRanks[(1 * 2) + dct];

				if (MemClkFreq == 0x4) {
					/* DDR3-800 */
					if (rank_count_dimm0 == 1)
						calibration_code = 0x00000000;
					else
						calibration_code = 0x003b0000;
				} else if (MemClkFreq == 0x5) {
					/* DDR3-1066 */
					if (rank_count_dimm0 == 1)
						calibration_code = 0x00000000;
					else
						calibration_code = 0x00380000;
				} else if (MemClkFreq == 0x6) {
					/* DDR3-1333 */
					if (rank_count_dimm0 == 1)
						calibration_code = 0x00000000;
					else
						calibration_code = 0x00360000;
				}
			} else if (dimm_count == 2) {
				/* 2 DIMMs detected */
				rank_count_dimm0 = pDCTstat->DimmRanks[(0 * 2) + dct];
				rank_count_dimm1 = pDCTstat->DimmRanks[(1 * 2) + dct];

				if (MemClkFreq == 0x4) {
					/* DDR3-800 */
					calibration_code = 0x00390039;
				} else if (MemClkFreq == 0x5) {
					/* DDR3-1066 */
					calibration_code = 0x00350037;
				} else if (MemClkFreq == 0x6) {
					/* DDR3-1333 */
					calibration_code = 0x00000035;
				}
			}
		}
	} else {
		/* TODO
		 * Other socket support unimplemented
		 */
	}

	return calibration_code;
}

static uint32_t fam15h_phy_predriver_calibration_code(struct DCTStatStruc *pDCTstat, uint8_t dct, uint8_t drive_strength)
{
	uint8_t lrdimm = 0;
	uint8_t package_type;
	uint8_t ddr_voltage_index;
	uint32_t calibration_code = 0;
	uint16_t MemClkFreq = Get_NB32_DCT(pDCTstat->dev_dct, dct, 0x94) & 0x1f;

	ddr_voltage_index = dct_ddr_voltage_index(pDCTstat, dct);
	package_type = mctGet_NVbits(NV_PACK_TYPE);

	if (!lrdimm) {
		/* Not an LRDIMM */
		if ((package_type == PT_M2) || (package_type == PT_GR)) {
			/* Socket AM3 or G34 */
			if (ddr_voltage_index & 0x4) {
				/* 1.25V */
				/* Fam15h BKDG Rev. 3.14 section 2.10.5.3.4 Table 43 */
				if ((MemClkFreq == 0x4) || (MemClkFreq == 0x6)) {
					/* DDR3-667 - DDR3-800 */
					if (drive_strength == 0x0)
						calibration_code = 0xfff;
					else if (drive_strength == 0x1)
						calibration_code = 0xb6d;
					else if (drive_strength == 0x2)
						calibration_code = 0x924;
					else if (drive_strength == 0x3)
						calibration_code = 0x6db;
				} else if ((MemClkFreq == 0xa) || (MemClkFreq == 0xe)) {
					/* DDR3-1066 - DDR3-1333 */
					if (drive_strength == 0x0)
						calibration_code = 0xfff;
					else if (drive_strength == 0x1)
						calibration_code = 0xfff;
					else if (drive_strength == 0x2)
						calibration_code = 0xdb6;
					else if (drive_strength == 0x3)
						calibration_code = 0x924;
				} else if ((MemClkFreq == 0x12) || (MemClkFreq == 0x16)) {
					/* DDR3-1600 - DDR3-1866 */
					if (drive_strength == 0x0)
						calibration_code = 0xfff;
					else if (drive_strength == 0x1)
						calibration_code = 0xfff;
					else if (drive_strength == 0x2)
						calibration_code = 0xfff;
					else if (drive_strength == 0x3)
						calibration_code = 0xfff;
				}
			} else if (ddr_voltage_index & 0x2) {
				/* 1.35V */
				/* Fam15h BKDG Rev. 3.14 section 2.10.5.3.4 Table 42 */
				if ((MemClkFreq == 0x4) || (MemClkFreq == 0x6)) {
					/* DDR3-667 - DDR3-800 */
					if (drive_strength == 0x0)
						calibration_code = 0xfff;
					else if (drive_strength == 0x1)
						calibration_code = 0x924;
					else if (drive_strength == 0x2)
						calibration_code = 0x6db;
					else if (drive_strength == 0x3)
						calibration_code = 0x492;
				} else if ((MemClkFreq == 0xa) || (MemClkFreq == 0xe)) {
					/* DDR3-1066 - DDR3-1333 */
					if (drive_strength == 0x0)
						calibration_code = 0xfff;
					else if (drive_strength == 0x1)
						calibration_code = 0xdb6;
					else if (drive_strength == 0x2)
						calibration_code = 0xbd6;
					else if (drive_strength == 0x3)
						calibration_code = 0x6db;
				} else if ((MemClkFreq == 0x12) || (MemClkFreq == 0x16)) {
					/* DDR3-1600 - DDR3-1866 */
					if (drive_strength == 0x0)
						calibration_code = 0xfff;
					else if (drive_strength == 0x1)
						calibration_code = 0xfff;
					else if (drive_strength == 0x2)
						calibration_code = 0xfff;
					else if (drive_strength == 0x3)
						calibration_code = 0xdb6;
				}
			} else if (ddr_voltage_index & 0x1) {
				/* 1.5V */
				/* Fam15h BKDG Rev. 3.14 section 2.10.5.3.4 Table 41 */
				if ((MemClkFreq == 0x4) || (MemClkFreq == 0x6)) {
					/* DDR3-667 - DDR3-800 */
					if (drive_strength == 0x0)
						calibration_code = 0xb6d;
					else if (drive_strength == 0x1)
						calibration_code = 0x6db;
					else if (drive_strength == 0x2)
						calibration_code = 0x492;
					else if (drive_strength == 0x3)
						calibration_code = 0x492;
				} else if ((MemClkFreq == 0xa) || (MemClkFreq == 0xe)) {
					/* DDR3-1066 - DDR3-1333 */
					if (drive_strength == 0x0)
						calibration_code = 0xfff;
					else if (drive_strength == 0x1)
						calibration_code = 0x924;
					else if (drive_strength == 0x2)
						calibration_code = 0x6db;
					else if (drive_strength == 0x3)
						calibration_code = 0x6db;
				} else if ((MemClkFreq == 0x12) || (MemClkFreq == 0x16)) {
					/* DDR3-1600 - DDR3-1866 */
					if (drive_strength == 0x0)
						calibration_code = 0xfff;
					else if (drive_strength == 0x1)
						calibration_code = 0xfff;
					else if (drive_strength == 0x2)
						calibration_code = 0xfff;
					else if (drive_strength == 0x3)
						calibration_code = 0xb6d;
				}
			}
		} else if (package_type == PT_C3) {
			/* Socket C32 */
			if (ddr_voltage_index & 0x4) {
				/* 1.25V */
				/* Fam15h BKDG Rev. 3.14 section 2.10.5.3.4 Table 46 */
				if ((MemClkFreq == 0x4) || (MemClkFreq == 0x6)) {
					/* DDR3-667 - DDR3-800 */
					if (drive_strength == 0x0)
						calibration_code = 0xfff;
					else if (drive_strength == 0x1)
						calibration_code = 0xb6d;
					else if (drive_strength == 0x2)
						calibration_code = 0x924;
					else if (drive_strength == 0x3)
						calibration_code = 0x6db;
				} else if (MemClkFreq == 0xa) {
					/* DDR3-1066 */
					if (drive_strength == 0x0)
						calibration_code = 0xfff;
					else if (drive_strength == 0x1)
						calibration_code = 0xfff;
					else if (drive_strength == 0x2)
						calibration_code = 0xdb6;
					else if (drive_strength == 0x3)
						calibration_code = 0x924;
				} else if (MemClkFreq == 0xe) {
					/* DDR3-1333 */
					if (drive_strength == 0x0)
						calibration_code = 0xb6d;
					else if (drive_strength == 0x1)
						calibration_code = 0x6db;
					else if (drive_strength == 0x2)
						calibration_code = 0x492;
					else if (drive_strength == 0x3)
						calibration_code = 0x492;
				} else if ((MemClkFreq == 0x12) || (MemClkFreq == 0x16)) {
					/* DDR3-1600 - DDR3-1866 */
					if (drive_strength == 0x0)
						calibration_code = 0xfff;
					else if (drive_strength == 0x1)
						calibration_code = 0xfff;
					else if (drive_strength == 0x2)
						calibration_code = 0xfff;
					else if (drive_strength == 0x3)
						calibration_code = 0xfff;
				}
			} else if (ddr_voltage_index & 0x2) {
				/* 1.35V */
				/* Fam15h BKDG Rev. 3.14 section 2.10.5.3.4 Table 45 */
				if ((MemClkFreq == 0x4) || (MemClkFreq == 0x6)) {
					/* DDR3-667 - DDR3-800 */
					if (drive_strength == 0x0)
						calibration_code = 0xfff;
					else if (drive_strength == 0x1)
						calibration_code = 0x924;
					else if (drive_strength == 0x2)
						calibration_code = 0x6db;
					else if (drive_strength == 0x3)
						calibration_code = 0x492;
				} else if (MemClkFreq == 0xa) {
					/* DDR3-1066 */
					if (drive_strength == 0x0)
						calibration_code = 0xfff;
					else if (drive_strength == 0x1)
						calibration_code = 0xdb6;
					else if (drive_strength == 0x2)
						calibration_code = 0xb6d;
					else if (drive_strength == 0x3)
						calibration_code = 0x6db;
				} else if (MemClkFreq == 0xe) {
					/* DDR3-1333 */
					if (drive_strength == 0x0)
						calibration_code = 0xfff;
					else if (drive_strength == 0x1)
						calibration_code = 0x924;
					else if (drive_strength == 0x2)
						calibration_code = 0x6db;
					else if (drive_strength == 0x3)
						calibration_code = 0x492;
				} else if ((MemClkFreq == 0x12) || (MemClkFreq == 0x16)) {
					/* DDR3-1600 - DDR3-1866 */
					if (drive_strength == 0x0)
						calibration_code = 0xfff;
					else if (drive_strength == 0x1)
						calibration_code = 0xfff;
					else if (drive_strength == 0x2)
						calibration_code = 0xfff;
					else if (drive_strength == 0x3)
						calibration_code = 0xdb6;
				}
			} else if (ddr_voltage_index & 0x1) {
				/* 1.5V */
				/* Fam15h BKDG Rev. 3.14 section 2.10.5.3.4 Table 44 */
				if ((MemClkFreq == 0x4) || (MemClkFreq == 0x6)) {
					/* DDR3-667 - DDR3-800 */
					if (drive_strength == 0x0)
						calibration_code = 0xb6d;
					else if (drive_strength == 0x1)
						calibration_code = 0x6db;
					else if (drive_strength == 0x2)
						calibration_code = 0x492;
					else if (drive_strength == 0x3)
						calibration_code = 0x492;
				} else if (MemClkFreq == 0xa) {
					/* DDR3-1066 */
					if (drive_strength == 0x0)
						calibration_code = 0xfff;
					else if (drive_strength == 0x1)
						calibration_code = 0x924;
					else if (drive_strength == 0x2)
						calibration_code = 0x6db;
					else if (drive_strength == 0x3)
						calibration_code = 0x6db;
				} else if (MemClkFreq == 0xe) {
					/* DDR3-1333 */
					if (drive_strength == 0x0)
						calibration_code = 0xb6d;
					else if (drive_strength == 0x1)
						calibration_code = 0x6db;
					else if (drive_strength == 0x2)
						calibration_code = 0x492;
					else if (drive_strength == 0x3)
						calibration_code = 0x492;
				} else if ((MemClkFreq == 0x12) || (MemClkFreq == 0x16)) {
					/* DDR3-1600 - DDR3-1866 */
					if (drive_strength == 0x0)
						calibration_code = 0xfff;
					else if (drive_strength == 0x1)
						calibration_code = 0xfff;
					else if (drive_strength == 0x2)
						calibration_code = 0xfff;
					else if (drive_strength == 0x3)
						calibration_code = 0xb6d;
				}
			}
		} else if (package_type == PT_FM2) {
			/* Socket FM2 */
			if (ddr_voltage_index & 0x1) {
				/* 1.5V */
				/* Fam15h BKDG Rev. 3.12 section 2.9.5.4.4 Table 22 */
				if ((MemClkFreq == 0x4) || (MemClkFreq == 0x6)) {
					/* DDR3-667 - DDR3-800 */
					calibration_code = 0xb24;
				} else if (MemClkFreq >= 0xa) {
					/* DDR3-1066 or higher */
					calibration_code = 0xff6;
				}
			}
		}
	} else {
		/* LRDIMM */

		/* TODO
		 * Implement LRDIMM support
		 * See Fam15h BKDG Rev. 3.14 section 2.10.5.3.4 Tables 47 - 49
		 */
	}

	return calibration_code;
}

static uint32_t fam15h_phy_predriver_cmd_addr_calibration_code(struct DCTStatStruc *pDCTstat, uint8_t dct, uint8_t drive_strength)
{
	uint8_t ddr_voltage_index;
	uint32_t calibration_code = 0;
	uint16_t MemClkFreq = Get_NB32_DCT(pDCTstat->dev_dct, dct, 0x94) & 0x1f;

	ddr_voltage_index = dct_ddr_voltage_index(pDCTstat, dct);

	if (ddr_voltage_index & 0x4) {
		/* 1.25V */
		/* Fam15h BKDG Rev. 3.14 section 2.10.5.3.4 Table 52 */
		if ((MemClkFreq == 0x4) || (MemClkFreq == 0x6)) {
			/* DDR3-667 - DDR3-800 */
			if (drive_strength == 0x0)
				calibration_code = 0x492;
			else if (drive_strength == 0x1)
				calibration_code = 0x492;
			else if (drive_strength == 0x2)
				calibration_code = 0x492;
			else if (drive_strength == 0x3)
				calibration_code = 0x492;
		} else if ((MemClkFreq == 0xa) || (MemClkFreq == 0xe)) {
			/* DDR3-1066 - DDR3-1333 */
			if (drive_strength == 0x0)
				calibration_code = 0xdad;
			else if (drive_strength == 0x1)
				calibration_code = 0x924;
			else if (drive_strength == 0x2)
				calibration_code = 0x6db;
			else if (drive_strength == 0x3)
				calibration_code = 0x492;
		} else if ((MemClkFreq == 0x12) || (MemClkFreq == 0x16)) {
			/* DDR3-1600 - DDR3-1866 */
			if (drive_strength == 0x0)
				calibration_code = 0xff6;
			else if (drive_strength == 0x1)
				calibration_code = 0xdad;
			else if (drive_strength == 0x2)
				calibration_code = 0xb64;
			else if (drive_strength == 0x3)
				calibration_code = 0xb64;
		}
	} else if (ddr_voltage_index & 0x2) {
		/* 1.35V */
		/* Fam15h BKDG Rev. 3.14 section 2.10.5.3.4 Table 51 */
		if ((MemClkFreq == 0x4) || (MemClkFreq == 0x6)) {
			/* DDR3-667 - DDR3-800 */
			if (drive_strength == 0x0)
				calibration_code = 0x492;
			else if (drive_strength == 0x1)
				calibration_code = 0x492;
			else if (drive_strength == 0x2)
				calibration_code = 0x492;
			else if (drive_strength == 0x3)
				calibration_code = 0x492;
		} else if ((MemClkFreq == 0xa) || (MemClkFreq == 0xe)) {
			/* DDR3-1066 - DDR3-1333 */
			if (drive_strength == 0x0)
				calibration_code = 0x924;
			else if (drive_strength == 0x1)
				calibration_code = 0x6db;
			else if (drive_strength == 0x2)
				calibration_code = 0x6db;
			else if (drive_strength == 0x3)
				calibration_code = 0x6db;
		} else if ((MemClkFreq == 0x12) || (MemClkFreq == 0x16)) {
			/* DDR3-1600 - DDR3-1866 */
			if (drive_strength == 0x0)
				calibration_code = 0xb6d;
			else if (drive_strength == 0x1)
				calibration_code = 0xb6d;
			else if (drive_strength == 0x2)
				calibration_code = 0x924;
			else if (drive_strength == 0x3)
				calibration_code = 0x924;
		}
	} else if (ddr_voltage_index & 0x1) {
		/* 1.5V */
		/* Fam15h BKDG Rev. 3.14 section 2.10.5.3.4 Table 50 */
		if ((MemClkFreq == 0x4) || (MemClkFreq == 0x6)) {
			/* DDR3-667 - DDR3-800 */
			if (drive_strength == 0x0)
				calibration_code = 0x492;
			else if (drive_strength == 0x1)
				calibration_code = 0x492;
			else if (drive_strength == 0x2)
				calibration_code = 0x492;
			else if (drive_strength == 0x3)
				calibration_code = 0x492;
		} else if ((MemClkFreq == 0xa) || (MemClkFreq == 0xe)) {
			/* DDR3-1066 - DDR3-1333 */
			if (drive_strength == 0x0)
				calibration_code = 0x6db;
			else if (drive_strength == 0x1)
				calibration_code = 0x6db;
			else if (drive_strength == 0x2)
				calibration_code = 0x6db;
			else if (drive_strength == 0x3)
				calibration_code = 0x6db;
		} else if ((MemClkFreq == 0x12) || (MemClkFreq == 0x16)) {
			/* DDR3-1600 - DDR3-1866 */
			if (drive_strength == 0x0)
				calibration_code = 0xb6d;
			else if (drive_strength == 0x1)
				calibration_code = 0xb6d;
			else if (drive_strength == 0x2)
				calibration_code = 0xb6d;
			else if (drive_strength == 0x3)
				calibration_code = 0xb6d;
		}
	}

	return calibration_code;
}

static uint32_t fam15h_phy_predriver_clk_calibration_code(struct DCTStatStruc *pDCTstat, uint8_t dct, uint8_t drive_strength)
{
	uint8_t ddr_voltage_index;
	uint32_t calibration_code = 0;
	uint16_t MemClkFreq = Get_NB32_DCT(pDCTstat->dev_dct, dct, 0x94) & 0x1f;

	ddr_voltage_index = dct_ddr_voltage_index(pDCTstat, dct);

	if (ddr_voltage_index & 0x4) {
		/* 1.25V */
		/* Fam15h BKDG Rev. 3.14 section 2.10.5.3.4 Table 55 */
		if ((MemClkFreq == 0x4) || (MemClkFreq == 0x6)) {
			/* DDR3-667 - DDR3-800 */
			if (drive_strength == 0x0)
				calibration_code = 0xdad;
			else if (drive_strength == 0x1)
				calibration_code = 0xdad;
			else if (drive_strength == 0x2)
				calibration_code = 0x924;
			else if (drive_strength == 0x3)
				calibration_code = 0x924;
		} else if ((MemClkFreq == 0xa) || (MemClkFreq == 0xe)) {
			/* DDR3-1066 - DDR3-1333 */
			if (drive_strength == 0x0)
				calibration_code = 0xff6;
			else if (drive_strength == 0x1)
				calibration_code = 0xff6;
			else if (drive_strength == 0x2)
				calibration_code = 0xff6;
			else if (drive_strength == 0x3)
				calibration_code = 0xff6;
		} else if ((MemClkFreq == 0x12) || (MemClkFreq == 0x16)) {
			/* DDR3-1600 - DDR3-1866 */
			if (drive_strength == 0x0)
				calibration_code = 0xff6;
			else if (drive_strength == 0x1)
				calibration_code = 0xff6;
			else if (drive_strength == 0x2)
				calibration_code = 0xff6;
			else if (drive_strength == 0x3)
				calibration_code = 0xff6;
		}
	} else if (ddr_voltage_index & 0x2) {
		/* 1.35V */
		/* Fam15h BKDG Rev. 3.14 section 2.10.5.3.4 Table 54 */
		if ((MemClkFreq == 0x4) || (MemClkFreq == 0x6)) {
			/* DDR3-667 - DDR3-800 */
			if (drive_strength == 0x0)
				calibration_code = 0xdad;
			else if (drive_strength == 0x1)
				calibration_code = 0xdad;
			else if (drive_strength == 0x2)
				calibration_code = 0x924;
			else if (drive_strength == 0x3)
				calibration_code = 0x924;
		} else if ((MemClkFreq == 0xa) || (MemClkFreq == 0xe)) {
			/* DDR3-1066 - DDR3-1333 */
			if (drive_strength == 0x0)
				calibration_code = 0xff6;
			else if (drive_strength == 0x1)
				calibration_code = 0xff6;
			else if (drive_strength == 0x2)
				calibration_code = 0xff6;
			else if (drive_strength == 0x3)
				calibration_code = 0xdad;
		} else if ((MemClkFreq == 0x12) || (MemClkFreq == 0x16)) {
			/* DDR3-1600 - DDR3-1866 */
			if (drive_strength == 0x0)
				calibration_code = 0xff6;
			else if (drive_strength == 0x1)
				calibration_code = 0xff6;
			else if (drive_strength == 0x2)
				calibration_code = 0xff6;
			else if (drive_strength == 0x3)
				calibration_code = 0xdad;
		}
	} else if (ddr_voltage_index & 0x1) {
		/* 1.5V */
		/* Fam15h BKDG Rev. 3.14 section 2.10.5.3.4 Table 53 */
		if ((MemClkFreq == 0x4) || (MemClkFreq == 0x6)) {
			/* DDR3-667 - DDR3-800 */
			if (drive_strength == 0x0)
				calibration_code = 0x924;
			else if (drive_strength == 0x1)
				calibration_code = 0x924;
			else if (drive_strength == 0x2)
				calibration_code = 0x924;
			else if (drive_strength == 0x3)
				calibration_code = 0x924;
		} else if ((MemClkFreq == 0xa) || (MemClkFreq == 0xe)) {
			/* DDR3-1066 - DDR3-1333 */
			if (drive_strength == 0x0)
				calibration_code = 0xff6;
			else if (drive_strength == 0x1)
				calibration_code = 0xff6;
			else if (drive_strength == 0x2)
				calibration_code = 0xff6;
			else if (drive_strength == 0x3)
				calibration_code = 0xb6d;
		} else if ((MemClkFreq == 0x12) || (MemClkFreq == 0x16)) {
			/* DDR3-1600 - DDR3-1866 */
			if (drive_strength == 0x0)
				calibration_code = 0xff6;
			else if (drive_strength == 0x1)
				calibration_code = 0xff6;
			else if (drive_strength == 0x2)
				calibration_code = 0xff6;
			else if (drive_strength == 0x3)
				calibration_code = 0xff6;
		}
	}

	return calibration_code;
}

uint32_t fam15h_output_driver_compensation_code(struct DCTStatStruc *pDCTstat, uint8_t dct)
{
	uint8_t MaxDimmsInstallable = mctGet_NVbits(NV_MAX_DIMMS_PER_CH);

	uint8_t package_type;
	uint32_t calibration_code = 0;

	package_type = mctGet_NVbits(NV_PACK_TYPE);
	uint16_t MemClkFreq = Get_NB32_DCT(pDCTstat->dev_dct, dct, 0x94) & 0x1f;

	/* Obtain number of DIMMs on channel */
	uint8_t dimm_count = pDCTstat->MAdimms[dct];
	uint8_t rank_count_dimm0;
	uint8_t rank_count_dimm1;

	if (package_type == PT_GR) {
		/* Socket G34 */
		if (pDCTstat->Status & (1 << SB_Registered)) {
			/* RDIMM */
			/* Fam15h BKDG Rev. 3.14 section 2.10.5.3.4 Table 74 */
			if (MaxDimmsInstallable == 1) {
				rank_count_dimm0 = pDCTstat->DimmRanks[(1 * 2) + dct];

				if (MemClkFreq == 0x4) {
					/* DDR3-667 */
					calibration_code = 0x00112222;
				} else if (MemClkFreq == 0x6) {
					/* DDR3-800 */
					calibration_code = 0x10112222;
				} else if (MemClkFreq == 0xa) {
					/* DDR3-1066 */
					calibration_code = 0x20112222;
				} else if ((MemClkFreq == 0xe) || (MemClkFreq == 0x12)) {
					/* DDR3-1333 - DDR3-1600 */
					calibration_code = 0x30112222;
				} else if (MemClkFreq == 0x16) {
					/* DDR3-1866 */
					calibration_code = 0x30332222;
				}

				if (rank_count_dimm0 == 4) {
					calibration_code &= ~(0xff << 16);
					calibration_code |= 0x22 << 16;
				}
			} else if (MaxDimmsInstallable == 2) {
				rank_count_dimm0 = pDCTstat->DimmRanks[(0 * 2) + dct];
				rank_count_dimm1 = pDCTstat->DimmRanks[(1 * 2) + dct];

				if (dimm_count == 1) {
					/* 1 DIMM detected */
					if (MemClkFreq == 0x4) {
						/* DDR3-667 */
						calibration_code = 0x00112222;
					} else if (MemClkFreq == 0x6) {
						/* DDR3-800 */
						calibration_code = 0x10112222;
					} else if (MemClkFreq == 0xa) {
						/* DDR3-1066 */
						calibration_code = 0x20112222;
					} else if ((MemClkFreq == 0xe) || (MemClkFreq == 0x12)) {
						/* DDR3-1333 - DDR3-1600 */
						calibration_code = 0x30112222;
					}

					if ((rank_count_dimm0 == 4) || (rank_count_dimm1 == 4)) {
						calibration_code &= ~(0xff << 16);
						calibration_code |= 0x22 << 16;
					}
				} else if (dimm_count == 2) {
					/* 2 DIMMs detected */
					rank_count_dimm0 = pDCTstat->DimmRanks[(0 * 2) + dct];
					rank_count_dimm1 = pDCTstat->DimmRanks[(1 * 2) + dct];

					if (MemClkFreq == 0x4) {
						/* DDR3-667 */
						calibration_code = 0x10222222;
					} else if (MemClkFreq == 0x6) {
						/* DDR3-800 */
						calibration_code = 0x20222222;
					} else if (MemClkFreq == 0xa) {
						/* DDR3-1066 */
						calibration_code = 0x30222222;
					} else if (MemClkFreq == 0xe) {
						/* DDR3-1333 */
						calibration_code = 0x30222222;
					} else if (MemClkFreq == 0x12) {
						/* DDR3-1600 */
						calibration_code = 0x30222222;
					}
				}
			} else if (MaxDimmsInstallable == 3) {
				/* TODO
				 * 3 DIMM/channel support unimplemented
				 */
			}
		} else if (pDCTstat->Status & (1 << SB_LoadReduced)) {
			/* LRDIMM */
			/* TODO
			 * LRDIMM support unimplemented
			 */
		} else {
			/* UDIMM */
			/* Fam15h BKDG Rev. 3.14 section 2.10.5.3.4 Table 73 */
			if (MaxDimmsInstallable == 1) {
				if (MemClkFreq == 0x4) {
					/* DDR3-667 */
					calibration_code = 0x00112222;
				} else if (MemClkFreq == 0x6) {
					/* DDR3-800 */
					calibration_code = 0x10112222;
				} else if (MemClkFreq == 0xa) {
					/* DDR3-1066 */
					calibration_code = 0x20112222;
				} else if ((MemClkFreq == 0xe) || (MemClkFreq == 0x12)) {
					/* DDR3-1333 - DDR3-1600 */
					calibration_code = 0x30112222;
				} else if (MemClkFreq == 0x16) {
					/* DDR3-1866 */
					calibration_code = 0x30332222;
				}
			} else if (MaxDimmsInstallable == 2) {
				if (dimm_count == 1) {
					/* 1 DIMM detected */
					if (MemClkFreq == 0x4) {
						/* DDR3-667 */
						calibration_code = 0x00112222;
					} else if (MemClkFreq == 0x6) {
						/* DDR3-800 */
						calibration_code = 0x10112222;
					} else if (MemClkFreq == 0xa) {
						/* DDR3-1066 */
						calibration_code = 0x20112222;
					} else if ((MemClkFreq == 0xe) || (MemClkFreq == 0x12)) {
						/* DDR3-1333 - DDR3-1600 */
						calibration_code = 0x30112222;
					}
				} else if (dimm_count == 2) {
					/* 2 DIMMs detected */
					rank_count_dimm0 = pDCTstat->DimmRanks[(0 * 2) + dct];
					rank_count_dimm1 = pDCTstat->DimmRanks[(1 * 2) + dct];

					if (MemClkFreq == 0x4) {
						/* DDR3-667 */
						calibration_code = 0x10222222;
					} else if (MemClkFreq == 0x6) {
						/* DDR3-800 */
						calibration_code = 0x20222222;
					} else if (MemClkFreq == 0xa) {
						/* DDR3-1066 */
						calibration_code = 0x30222222;
					} else if (MemClkFreq == 0xe) {
						/* DDR3-1333 */
						calibration_code = 0x30222222;
					} else if (MemClkFreq == 0x12) {
						/* DDR3-1600 */
						if ((rank_count_dimm0 == 1) && (rank_count_dimm1 == 1))
							calibration_code = 0x30222222;
						else
							calibration_code = 0x30112222;
					}
				}
			} else if (MaxDimmsInstallable == 3) {
				/* TODO
				 * 3 DIMM/channel support unimplemented
				 */
			}
		}
	} else if (package_type == PT_C3) {
		/* Socket C32 */
		if (pDCTstat->Status & (1 << SB_Registered)) {
			/* RDIMM */
			/* Fam15h BKDG Rev. 3.14 section 2.10.5.3.4 Table 77 */
			if (MaxDimmsInstallable == 1) {
				rank_count_dimm0 = pDCTstat->DimmRanks[(1 * 2) + dct];

				if (MemClkFreq == 0x4) {
					/* DDR3-667 */
					calibration_code = 0x00112222;
				} else if (MemClkFreq == 0x6) {
					/* DDR3-800 */
					calibration_code = 0x10112222;
				} else if (MemClkFreq == 0xa) {
					/* DDR3-1066 */
					calibration_code = 0x20112222;
				} else if ((MemClkFreq == 0xe) || (MemClkFreq == 0x12)) {
					/* DDR3-1333 - DDR3-1600 */
					calibration_code = 0x30112222;
				}

				if (rank_count_dimm0 == 4) {
					calibration_code &= ~(0xff << 16);
					calibration_code |= 0x22 << 16;
				}
			} else if (MaxDimmsInstallable == 2) {
				rank_count_dimm0 = pDCTstat->DimmRanks[(0 * 2) + dct];
				rank_count_dimm1 = pDCTstat->DimmRanks[(1 * 2) + dct];

				if (dimm_count == 1) {
					/* 1 DIMM detected */
					if (MemClkFreq == 0x4) {
						/* DDR3-667 */
						calibration_code = 0x00112222;
					} else if (MemClkFreq == 0x6) {
						/* DDR3-800 */
						calibration_code = 0x10112222;
					} else if (MemClkFreq == 0xa) {
						/* DDR3-1066 */
						calibration_code = 0x20112222;
					} else if ((MemClkFreq == 0xe) || (MemClkFreq == 0x12)) {
						/* DDR3-1333 - DDR3-1600 */
						calibration_code = 0x30112222;
					}

					if ((rank_count_dimm0 == 4) || (rank_count_dimm1 == 4)) {
						calibration_code &= ~(0xff << 16);
						calibration_code |= 0x22 << 16;
					}
				} else if (dimm_count == 2) {
					/* 2 DIMMs detected */
					rank_count_dimm0 = pDCTstat->DimmRanks[(0 * 2) + dct];
					rank_count_dimm1 = pDCTstat->DimmRanks[(1 * 2) + dct];

					if (MemClkFreq == 0x4) {
						/* DDR3-667 */
						calibration_code = 0x10222222;
					} else if (MemClkFreq == 0x6) {
						/* DDR3-800 */
						calibration_code = 0x20222222;
					} else if (MemClkFreq == 0xa) {
						/* DDR3-1066 */
						calibration_code = 0x30222222;
					} else if (MemClkFreq == 0xe) {
						/* DDR3-1333 */
						calibration_code = 0x30222222;
					} else if (MemClkFreq == 0x12) {
						/* DDR3-1600 */
						calibration_code = 0x30222222;
					}
				}
			} else if (MaxDimmsInstallable == 3) {
				/* TODO
				 * 3 DIMM/channel support unimplemented
				 */
			}
		} else if (pDCTstat->Status & (1 << SB_LoadReduced)) {
			/* LRDIMM */
			/* TODO
			 * LRDIMM support unimplemented
			 */
		} else {
			/* UDIMM */
			/* Fam15h BKDG Rev. 3.14 section 2.10.5.3.4 Table 73 */
			if (MaxDimmsInstallable == 1) {
				if (MemClkFreq == 0x4) {
					/* DDR3-667 */
					calibration_code = 0x00112222;
				} else if (MemClkFreq == 0x6) {
					/* DDR3-800 */
					calibration_code = 0x10112222;
				} else if (MemClkFreq == 0xa) {
					/* DDR3-1066 */
					calibration_code = 0x20112222;
				} else if ((MemClkFreq == 0xe) || (MemClkFreq == 0x12)) {
					/* DDR3-1333 - DDR3-1600 */
					calibration_code = 0x30112222;
				}
			} else if (MaxDimmsInstallable == 2) {
				if (dimm_count == 1) {
					/* 1 DIMM detected */
					if (MemClkFreq == 0x4) {
						/* DDR3-667 */
						calibration_code = 0x00112222;
					} else if (MemClkFreq == 0x6) {
						/* DDR3-800 */
						calibration_code = 0x10112222;
					} else if (MemClkFreq == 0xa) {
						/* DDR3-1066 */
						calibration_code = 0x20112222;
					} else if ((MemClkFreq == 0xe) || (MemClkFreq == 0x12)) {
						/* DDR3-1333 - DDR3-1600 */
						calibration_code = 0x30112222;
					}
				} else if (dimm_count == 2) {
					/* 2 DIMMs detected */
					rank_count_dimm0 = pDCTstat->DimmRanks[(0 * 2) + dct];
					rank_count_dimm1 = pDCTstat->DimmRanks[(1 * 2) + dct];

					if (MemClkFreq == 0x4) {
						/* DDR3-667 */
						calibration_code = 0x10222222;
					} else if (MemClkFreq == 0x6) {
						/* DDR3-800 */
						calibration_code = 0x20222222;
					} else if (MemClkFreq == 0xa) {
						/* DDR3-1066 */
						calibration_code = 0x30222222;
					} else if (MemClkFreq == 0xe) {
						/* DDR3-1333 */
						calibration_code = 0x30222222;
					} else if (MemClkFreq == 0x12) {
						/* DDR3-1600 */
						if ((rank_count_dimm0 == 1) && (rank_count_dimm1 == 1))
							calibration_code = 0x30222222;
						else
							calibration_code = 0x30112222;
					}
				}
			} else if (MaxDimmsInstallable == 3) {
				/* TODO
				 * 3 DIMM/channel support unimplemented
				 */
			}
		}
	} else if (package_type == PT_FM2) {
		/* Socket FM2 */
		/* Assume UDIMM */
		/* Fam15h Model10h BKDG Rev. 3.12 section 2.9.5.6.6 Table 32 */
		if (MaxDimmsInstallable == 1) {
			rank_count_dimm0 = pDCTstat->DimmRanks[(1 * 2) + dct];

			if (MemClkFreq == 0x4) {
				/* DDR3-667 */
				calibration_code = 0x00112222;
			} else if (MemClkFreq == 0x6) {
				/* DDR3-800 */
				calibration_code = 0x10112222;
			} else if (MemClkFreq == 0xa) {
				/* DDR3-1066 */
				calibration_code = 0x20112222;
			} else if (MemClkFreq >= 0xe) {
				/* DDR3-1333 or higher */
				calibration_code = 0x30112222;
			}
		} else if (MaxDimmsInstallable == 2) {
			rank_count_dimm0 = pDCTstat->DimmRanks[(0 * 2) + dct];
			rank_count_dimm1 = pDCTstat->DimmRanks[(1 * 2) + dct];

			if (dimm_count == 1) {
				/* 1 DIMM detected */
				if ((MemClkFreq == 0x4) || (MemClkFreq == 0x6)) {
					/* DDR3-667 or DDR3-800 */
					calibration_code = 0x00112222;
				} else if (MemClkFreq == 0xa) {
					/* DDR3-1066 */
					calibration_code = 0x10112222;
				} else if (MemClkFreq == 0xe) {
					/* DDR3-1333 */
					calibration_code = 0x20112222;
				} else if (MemClkFreq >= 0x12) {
					/* DDR3-1600 or higher */
					calibration_code = 0x30112222;
				}
			} else if (dimm_count == 2) {
				/* 2 DIMMs detected */
				rank_count_dimm0 = pDCTstat->DimmRanks[(0 * 2) + dct];
				rank_count_dimm1 = pDCTstat->DimmRanks[(1 * 2) + dct];

				if (MemClkFreq == 0x4) {
					/* DDR3-667 */
					calibration_code = 0x10222322;
				} else if (MemClkFreq == 0x6) {
					/* DDR3-800 */
					calibration_code = 0x20222322;
				} else if (MemClkFreq >= 0xa) {
					/* DDR3-1066 or higher */
					calibration_code = 0x30222322;
				}
			}
		} else if (MaxDimmsInstallable == 3) {
			/* TODO
			 * 3 DIMM/channel support unimplemented
			 */
		}
	} else {
		/* TODO
		 * Other socket support unimplemented
		 */
	}

	return calibration_code;
}

uint32_t fam15h_address_timing_compensation_code(struct DCTStatStruc *pDCTstat, uint8_t dct)
{
	uint8_t MaxDimmsInstallable = mctGet_NVbits(NV_MAX_DIMMS_PER_CH);

	uint8_t package_type;
	uint32_t calibration_code = 0;

	package_type = mctGet_NVbits(NV_PACK_TYPE);
	uint16_t MemClkFreq = Get_NB32_DCT(pDCTstat->dev_dct, dct, 0x94) & 0x1f;

	/* Obtain number of DIMMs on channel */
	uint8_t dimm_count = pDCTstat->MAdimms[dct];
	uint8_t rank_count_dimm0;
	uint8_t rank_count_dimm1;

	if (package_type == PT_GR) {
		/* Socket G34 */
		if (pDCTstat->Status & (1 << SB_Registered)) {
			/* RDIMM */
			/* Fam15h BKDG Rev. 3.14 section 2.10.5.3.4 Table 74 */
			if (MaxDimmsInstallable == 1) {
				if ((MemClkFreq == 0x4) || (MemClkFreq == 0x6)) {
					/* DDR3-667 - DDR3-800 */
					calibration_code = 0x00000000;
				} else if (MemClkFreq == 0xa) {
					/* DDR3-1066 */
					calibration_code = 0x003c3c3c;
				} else if (MemClkFreq == 0xe) {
					/* DDR3-1333 */
					calibration_code = 0x003a3a3a;
				} else if ((MemClkFreq == 0x12) || (MemClkFreq == 0x16)) {
					/* DDR3-1600 - DDR3-1866 */
					calibration_code = 0x00393939;
				}
			} else if (MaxDimmsInstallable == 2) {
				if (dimm_count == 1) {
					/* 1 DIMM detected */
					if ((MemClkFreq == 0x4) || (MemClkFreq == 0x6)) {
						/* DDR3-667 - DDR3-800 */
						calibration_code = 0x00000000;
					} else if (MemClkFreq == 0xa) {
						/* DDR3-1066 */
						calibration_code = 0x00393c39;
					} else if (MemClkFreq == 0xe) {
						/* DDR3-1333 */
						calibration_code = 0x00373a37;
					} else if (MemClkFreq == 0x12) {
						/* DDR3-1600 */
						calibration_code = 0x00363936;
					}
				} else if (dimm_count == 2) {
					/* 2 DIMMs detected */
					if ((MemClkFreq == 0x4) || (MemClkFreq == 0x6)) {
						/* DDR3-667 - DDR3-800 */
						calibration_code = 0x00000000;
					} else if (MemClkFreq == 0xa) {
						/* DDR3-1066 */
						calibration_code = 0x003a3c3a;
					} else if (MemClkFreq == 0xe) {
						/* DDR3-1333 */
						calibration_code = 0x00383a38;
					} else if (MemClkFreq == 0x12) {
						/* DDR3-1600 */
						calibration_code = 0x00353935;
					}
				}
			} else if (MaxDimmsInstallable == 3) {
				/* TODO
				 * 3 DIMM/channel support unimplemented
				 */
			}
		} else if (pDCTstat->Status & (1 << SB_LoadReduced)) {
			/* LRDIMM */
			/* TODO
			 * LRDIMM support unimplemented
			 */
		} else {
			/* UDIMM */
			/* Fam15h BKDG Rev. 3.14 section 2.10.5.3.4 Table 76 */
			if (MaxDimmsInstallable == 1) {
				rank_count_dimm0 = pDCTstat->DimmRanks[(1 * 2) + dct];

				if (MemClkFreq == 0x4) {
					/* DDR3-667 */
					if (rank_count_dimm0 == 1)
						calibration_code = 0x00000000;
					else
						calibration_code = 0x003b0000;
				} else if (MemClkFreq == 0x6) {
					/* DDR3-800 */
					if (rank_count_dimm0 == 1)
						calibration_code = 0x00000000;
					else
						calibration_code = 0x003b0000;
				} else if (MemClkFreq == 0xa) {
					/* DDR3-1066 */
					calibration_code = 0x00383837;
				} else if (MemClkFreq == 0xe) {
					/* DDR3-1333 */
					calibration_code = 0x00363635;
				} else if (MemClkFreq == 0x12) {
					/* DDR3-1600 */
					if (rank_count_dimm0 == 1)
						calibration_code = 0x00353533;
					else
						calibration_code = 0x00003533;
				} else if (MemClkFreq == 0x16) {
					/* DDR3-1866 */
					calibration_code = 0x00333330;
				}
			} else if (MaxDimmsInstallable == 2) {
				if (dimm_count == 1) {
					/* 1 DIMM detected */
					rank_count_dimm0 = pDCTstat->DimmRanks[(1 * 2) + dct];

					if (MemClkFreq == 0x4) {
						/* DDR3-667 */
						if (rank_count_dimm0 == 1)
							calibration_code = 0x00000000;
						else
							calibration_code = 0x003b0000;
					} else if (MemClkFreq == 0x6) {
						/* DDR3-800 */
						if (rank_count_dimm0 == 1)
							calibration_code = 0x00000000;
						else
							calibration_code = 0x003b0000;
					} else if (MemClkFreq == 0xa) {
						/* DDR3-1066 */
						calibration_code = 0x00383837;
					} else if (MemClkFreq == 0xe) {
						/* DDR3-1333 */
						calibration_code = 0x00363635;
					} else if (MemClkFreq == 0x12) {
						/* DDR3-1600 */
						if (rank_count_dimm0 == 1)
							calibration_code = 0x00353533;
						else
							calibration_code = 0x00003533;
					}
				} else if (dimm_count == 2) {
					/* 2 DIMMs detected */
					rank_count_dimm0 = pDCTstat->DimmRanks[(0 * 2) + dct];
					rank_count_dimm1 = pDCTstat->DimmRanks[(1 * 2) + dct];

					if (MemClkFreq == 0x4) {
						/* DDR3-667 */
						calibration_code = 0x00390039;
					} else if (MemClkFreq == 0x6) {
						/* DDR3-800 */
						calibration_code = 0x00390039;
					} else if (MemClkFreq == 0xa) {
						/* DDR3-1066 */
						calibration_code = 0x003a3a3a;
					} else if (MemClkFreq == 0xe) {
						/* DDR3-1333 */
						calibration_code = 0x00003939;
					} else if (MemClkFreq == 0x12) {
						/* DDR3-1600 */
						if ((rank_count_dimm0 == 1) && (rank_count_dimm1 == 1))
							calibration_code = 0x00003738;
					}
				}
			} else if (MaxDimmsInstallable == 3) {
				/* TODO
				 * 3 DIMM/channel support unimplemented
				 */
			}
		}
	} else if (package_type == PT_C3) {
		/* Socket C32 */
		if (pDCTstat->Status & (1 << SB_Registered)) {
			/* RDIMM */
			/* Fam15h BKDG Rev. 3.14 section 2.10.5.3.4 Table 77 */
			if (MaxDimmsInstallable == 1) {
				if ((MemClkFreq == 0x4) || (MemClkFreq == 0x6)) {
					/* DDR3-667 - DDR3-800 */
					calibration_code = 0x00000000;
				} else if (MemClkFreq == 0xa) {
					/* DDR3-1066 */
					calibration_code = 0x003c3c3c;
				} else if (MemClkFreq == 0xe) {
					/* DDR3-1333 */
					calibration_code = 0x003a3a3a;
				} else if ((MemClkFreq == 0x12) || (MemClkFreq == 0x16)) {
					/* DDR3-1600 - DDR3-1866 */
					calibration_code = 0x00393939;
				}
			} else if (MaxDimmsInstallable == 2) {
				if (dimm_count == 1) {
					/* 1 DIMM detected */
					if ((MemClkFreq == 0x4) || (MemClkFreq == 0x6)) {
						/* DDR3-667 - DDR3-800 */
						calibration_code = 0x00000000;
					} else if (MemClkFreq == 0xa) {
						/* DDR3-1066 */
						calibration_code = 0x00393c39;
					} else if (MemClkFreq == 0xe) {
						/* DDR3-1333 */
						calibration_code = 0x00373a37;
					} else if (MemClkFreq == 0x12) {
						/* DDR3-1600 */
						calibration_code = 0x00363936;
					}
				} else if (dimm_count == 2) {
					/* 2 DIMMs detected */
					if ((MemClkFreq == 0x4) || (MemClkFreq == 0x6)) {
						/* DDR3-667 - DDR3-800 */
						calibration_code = 0x00000000;
					} else if (MemClkFreq == 0xa) {
						/* DDR3-1066 */
						calibration_code = 0x003a3c3a;
					} else if (MemClkFreq == 0xe) {
						/* DDR3-1333 */
						calibration_code = 0x00383a38;
					} else if (MemClkFreq == 0x12) {
						/* DDR3-1600 */
						calibration_code = 0x00353935;
					}
				}
			} else if (MaxDimmsInstallable == 3) {
				/* TODO
				 * 3 DIMM/channel support unimplemented
				 */
			}
		} else if (pDCTstat->Status & (1 << SB_LoadReduced)) {
			/* LRDIMM */
			/* TODO
			 * LRDIMM support unimplemented
			 */
		} else {
			/* UDIMM */
			/* Fam15h BKDG Rev. 3.14 section 2.10.5.3.4 Table 76 */
			if (MaxDimmsInstallable == 1) {
				rank_count_dimm0 = pDCTstat->DimmRanks[(1 * 2) + dct];

				if (MemClkFreq == 0x4) {
					/* DDR3-667 */
					if (rank_count_dimm0 == 1)
						calibration_code = 0x00000000;
					else
						calibration_code = 0x003b0000;
				} else if (MemClkFreq == 0x6) {
					/* DDR3-800 */
					if (rank_count_dimm0 == 1)
						calibration_code = 0x00000000;
					else
						calibration_code = 0x003b0000;
				} else if (MemClkFreq == 0xa) {
					/* DDR3-1066 */
					calibration_code = 0x00383837;
				} else if (MemClkFreq == 0xe) {
					/* DDR3-1333 */
					calibration_code = 0x00363635;
				} else if (MemClkFreq == 0x12) {
					/* DDR3-1600 */
					if (rank_count_dimm0 == 1)
						calibration_code = 0x00353533;
					else
						calibration_code = 0x00003533;
				} else if (MemClkFreq == 0x16) {
					/* DDR3-1866 */
					calibration_code = 0x00333330;
				}
			} else if (MaxDimmsInstallable == 2) {
				if (dimm_count == 1) {
					/* 1 DIMM detected */
					rank_count_dimm0 = pDCTstat->DimmRanks[(1 * 2) + dct];

					if (MemClkFreq == 0x4) {
						/* DDR3-667 */
						if (rank_count_dimm0 == 1)
							calibration_code = 0x00000000;
						else
							calibration_code = 0x003b0000;
					} else if (MemClkFreq == 0x6) {
						/* DDR3-800 */
						if (rank_count_dimm0 == 1)
							calibration_code = 0x00000000;
						else
							calibration_code = 0x003b0000;
					} else if (MemClkFreq == 0xa) {
						/* DDR3-1066 */
						calibration_code = 0x00383837;
					} else if (MemClkFreq == 0xe) {
						/* DDR3-1333 */
						calibration_code = 0x00363635;
					} else if (MemClkFreq == 0x12) {
						/* DDR3-1600 */
						if (rank_count_dimm0 == 1)
							calibration_code = 0x00353533;
						else
							calibration_code = 0x00003533;
					}
				} else if (dimm_count == 2) {
					/* 2 DIMMs detected */
					rank_count_dimm0 = pDCTstat->DimmRanks[(0 * 2) + dct];
					rank_count_dimm1 = pDCTstat->DimmRanks[(1 * 2) + dct];

					if (MemClkFreq == 0x4) {
						/* DDR3-667 */
						calibration_code = 0x00390039;
					} else if (MemClkFreq == 0x6) {
						/* DDR3-800 */
						calibration_code = 0x00390039;
					} else if (MemClkFreq == 0xa) {
						/* DDR3-1066 */
						calibration_code = 0x003a3a3a;
					} else if (MemClkFreq == 0xe) {
						/* DDR3-1333 */
						calibration_code = 0x00003939;
					} else if (MemClkFreq == 0x12) {
						/* DDR3-1600 */
						if ((rank_count_dimm0 == 1) && (rank_count_dimm1 == 1))
							calibration_code = 0x00003738;
					}
				}
			} else if (MaxDimmsInstallable == 3) {
				/* TODO
				 * 3 DIMM/channel support unimplemented
				 */
			}
		}
	} else if (package_type == PT_FM2) {
		/* Socket FM2 */
		/* Assume UDIMM */
		/* Fam15h Model10h BKDG Rev. 3.12 section 2.9.5.6.6 Table 32 */
		if (dimm_count == 1) {
			/* 1 DIMM detected */
			rank_count_dimm0 = pDCTstat->DimmRanks[(1 * 2) + dct];

			if ((MemClkFreq == 0x4) || (MemClkFreq == 0x6)) {
				/* DDR3-667 or DDR3-800 */
				if (rank_count_dimm0 == 1)
					calibration_code = 0x00000000;
				else
					calibration_code = 0x003b0000;
			} else if (MemClkFreq == 0xa) {
				/* DDR3-1066 */
				if (rank_count_dimm0 == 1)
					calibration_code = 0x00000000;
				else
					calibration_code = 0x00380000;
			} else if (MemClkFreq == 0xe) {
				/* DDR3-1333 */
				if (rank_count_dimm0 == 1)
					calibration_code = 0x00000000;
				else
					calibration_code = 0x00360000;
			} else if (MemClkFreq >= 0x12) {
				/* DDR3-1600 or higher */
				calibration_code = 0x00000000;
			}

		} else if (dimm_count == 2) {
			/* 2 DIMMs detected */
			rank_count_dimm0 = pDCTstat->DimmRanks[(0 * 2) + dct];
			rank_count_dimm1 = pDCTstat->DimmRanks[(1 * 2) + dct];

			if ((MemClkFreq == 0x4) || (MemClkFreq == 0x6)) {
				/* DDR3-667 or DDR3-800 */
				calibration_code = 0x00390039;
			} else if (MemClkFreq == 0xa) {
				/* DDR3-1066 */
				calibration_code = 0x00350037;
			} else if (MemClkFreq == 0xe) {
				/* DDR3-1333 */
				calibration_code = 0x00000035;
			} else if (MemClkFreq == 0x12) {
				/* DDR3-1600 */
				calibration_code = 0x0000002b;
			} else if (MemClkFreq > 0x12) {
				/* DDR3-1866 or greater */
				calibration_code = 0x00000031;
			}
		} else if (MaxDimmsInstallable == 3) {
			/* TODO
			 * 3 DIMM/channel support unimplemented
			 */
		}
	} else {
		/* TODO
		 * Other socket support unimplemented
		 */
	}

	return calibration_code;
}

uint8_t fam15h_slow_access_mode(struct DCTStatStruc *pDCTstat, uint8_t dct)
{
	uint8_t MaxDimmsInstallable = mctGet_NVbits(NV_MAX_DIMMS_PER_CH);

	uint8_t package_type;
	uint32_t slow_access = 0;

	package_type = mctGet_NVbits(NV_PACK_TYPE);
	uint16_t MemClkFreq = Get_NB32_DCT(pDCTstat->dev_dct, dct, 0x94) & 0x1f;

	/* Obtain number of DIMMs on channel */
	uint8_t dimm_count = pDCTstat->MAdimms[dct];
	uint8_t rank_count_dimm0;
	uint8_t rank_count_dimm1;

	if (package_type == PT_GR) {
		/* Socket G34 */
		if (pDCTstat->Status & (1 << SB_Registered)) {
			/* RDIMM */
			/* Fam15h BKDG Rev. 3.14 section 2.10.5.3.4 Table 74 */
			slow_access = 0;
		} else if (pDCTstat->Status & (1 << SB_LoadReduced)) {
			/* LRDIMM */
			/* Fam15h BKDG Rev. 3.14 section 2.10.5.3.4 Table 75 */
			slow_access = 0;
		} else {
			/* UDIMM */
			/* Fam15h BKDG Rev. 3.14 section 2.10.5.3.4 Table 73 */
			if (MaxDimmsInstallable == 1) {
				rank_count_dimm0 = pDCTstat->DimmRanks[(1 * 2) + dct];

				if ((MemClkFreq == 0x4) || (MemClkFreq == 0x6)
					|| (MemClkFreq == 0xa) | (MemClkFreq == 0xe)) {
					/* DDR3-667 - DDR3-1333 */
					slow_access = 0;
				} else if ((MemClkFreq == 0x12) || (MemClkFreq == 0x16)) {
					/* DDR3-1600 - DDR3-1866 */
					if (rank_count_dimm0 == 1)
						slow_access = 0;
					else
						slow_access = 1;
				}
			} else if (MaxDimmsInstallable == 2) {
				if (dimm_count == 1) {
					/* 1 DIMM detected */
					rank_count_dimm0 = pDCTstat->DimmRanks[(1 * 2) + dct];

					if ((MemClkFreq == 0x4) || (MemClkFreq == 0x6)
						|| (MemClkFreq == 0xa) | (MemClkFreq == 0xe)) {
						/* DDR3-667 - DDR3-1333 */
						slow_access = 0;
					} else if (MemClkFreq == 0x12) {
						/* DDR3-1600 */
						if (rank_count_dimm0 == 1)
							slow_access = 0;
						else
							slow_access = 1;
					}
				} else if (dimm_count == 2) {
					/* 2 DIMMs detected */
					rank_count_dimm0 = pDCTstat->DimmRanks[(0 * 2) + dct];
					rank_count_dimm1 = pDCTstat->DimmRanks[(1 * 2) + dct];

					if ((MemClkFreq == 0x4) || (MemClkFreq == 0x6)
						|| (MemClkFreq == 0xa)) {
						/* DDR3-667 - DDR3-1066 */
						slow_access = 0;
					} else if ((MemClkFreq == 0xe) || (MemClkFreq == 0x12)) {
						/* DDR3-1333 - DDR3-1600 */
						slow_access = 1;
					}
				}
			} else if (MaxDimmsInstallable == 3) {
				/* TODO
				 * 3 DIMM/channel support unimplemented
				 */
			}
		}
	} else if (package_type == PT_C3) {
		/* Socket C32 */
		if (pDCTstat->Status & (1 << SB_Registered)) {
			/* RDIMM */
			/* Fam15h BKDG Rev. 3.14 section 2.10.5.3.4 Table 77 */
			slow_access = 0;
		} else if (pDCTstat->Status & (1 << SB_LoadReduced)) {
			/* LRDIMM */
			/* Fam15h BKDG Rev. 3.14 section 2.10.5.3.4 Table 78 */
			slow_access = 0;
		} else {
			/* UDIMM */
			/* Fam15h BKDG Rev. 3.14 section 2.10.5.3.4 Table 76 */
			if (MaxDimmsInstallable == 1) {
				rank_count_dimm0 = pDCTstat->DimmRanks[(1 * 2) + dct];

				if ((MemClkFreq == 0x4) || (MemClkFreq == 0x6)
					|| (MemClkFreq == 0xa) | (MemClkFreq == 0xe)) {
					/* DDR3-667 - DDR3-1333 */
					slow_access = 0;
				} else if ((MemClkFreq == 0x12) || (MemClkFreq == 0x16)) {
					/* DDR3-1600 - DDR3-1866 */
					if (rank_count_dimm0 == 1)
						slow_access = 0;
					else
						slow_access = 1;
				}
			} else if (MaxDimmsInstallable == 2) {
				if (dimm_count == 1) {
					/* 1 DIMM detected */
					rank_count_dimm0 = pDCTstat->DimmRanks[(1 * 2) + dct];

					if ((MemClkFreq == 0x4) || (MemClkFreq == 0x6)
						|| (MemClkFreq == 0xa) | (MemClkFreq == 0xe)) {
						/* DDR3-667 - DDR3-1333 */
						slow_access = 0;
					} else if (MemClkFreq == 0x12) {
						/* DDR3-1600 */
						if (rank_count_dimm0 == 1)
							slow_access = 0;
						else
							slow_access = 1;
					}
				} else if (dimm_count == 2) {
					/* 2 DIMMs detected */
					rank_count_dimm0 = pDCTstat->DimmRanks[(0 * 2) + dct];
					rank_count_dimm1 = pDCTstat->DimmRanks[(1 * 2) + dct];

					if ((MemClkFreq == 0x4) || (MemClkFreq == 0x6)
						|| (MemClkFreq == 0xa)) {
						/* DDR3-667 - DDR3-1066 */
						slow_access = 0;
					} else if ((MemClkFreq == 0xe) || (MemClkFreq == 0x12)) {
						/* DDR3-1333 - DDR3-1600 */
						slow_access = 1;
					}
				}
			} else if (MaxDimmsInstallable == 3) {
				/* TODO
				 * 3 DIMM/channel support unimplemented
				 */
			}
		}
	} else if (package_type == PT_FM2) {
		/* Socket FM2 */
		/* UDIMM */
		/* Fam15h Model10 BKDG Rev. 3.12 section 2.9.5.6.6 Table 32 */
		if (MaxDimmsInstallable == 1) {
			rank_count_dimm0 = pDCTstat->DimmRanks[(1 * 2) + dct];

			if ((MemClkFreq == 0x4) || (MemClkFreq == 0x6)
				|| (MemClkFreq == 0xa) | (MemClkFreq == 0xe)) {
				/* DDR3-667 - DDR3-1333 */
				slow_access = 0;
			} else if (MemClkFreq >= 0x12) {
				/* DDR3-1600 or higher */
				if (rank_count_dimm0 == 1)
					slow_access = 0;
				else
					slow_access = 1;
			}
		} else if (MaxDimmsInstallable == 2) {
			if (dimm_count == 1) {
				/* 1 DIMM detected */
				rank_count_dimm0 = pDCTstat->DimmRanks[(1 * 2) + dct];

				if ((MemClkFreq == 0x4) || (MemClkFreq == 0x6)
					|| (MemClkFreq == 0xa) | (MemClkFreq == 0xe)) {
					/* DDR3-667 - DDR3-1333 */
					slow_access = 0;
				} else if (MemClkFreq >= 0x12) {
					/* DDR3-1600 or higher */
					if (rank_count_dimm0 == 1)
						slow_access = 0;
					else
						slow_access = 1;
				}
			} else if (dimm_count == 2) {
				/* 2 DIMMs detected */
				rank_count_dimm0 = pDCTstat->DimmRanks[(0 * 2) + dct];
				rank_count_dimm1 = pDCTstat->DimmRanks[(1 * 2) + dct];

				if ((MemClkFreq == 0x4) || (MemClkFreq == 0x6)
					|| (MemClkFreq == 0xa)) {
					/* DDR3-667 - DDR3-1066 */
					slow_access = 0;
				} else if (MemClkFreq >= 0xe) {
					/* DDR3-1333 or higher */
					slow_access = 1;
				}
			}
		} else if (MaxDimmsInstallable == 3) {
			/* TODO
			 * 3 DIMM/channel support unimplemented
			 */
		}
	} else {
		/* TODO
		 * Other socket support unimplemented
		 */
	}

	return slow_access;
}

static uint8_t fam15h_odt_tristate_enable_code(struct DCTStatStruc *pDCTstat, uint8_t dct)
{
	uint8_t MaxDimmsInstallable = mctGet_NVbits(NV_MAX_DIMMS_PER_CH);

	uint8_t package_type;
	uint8_t odt_tristate_code = 0;

	package_type = mctGet_NVbits(NV_PACK_TYPE);

	/* Obtain number of DIMMs on channel */
	uint8_t dimm_count = pDCTstat->MAdimms[dct];
	uint8_t rank_count_dimm0;
	uint8_t rank_count_dimm1;

	if (package_type == PT_GR) {
		/* Socket G34 */
		if (pDCTstat->Status & (1 << SB_Registered)) {
			/* RDIMM */
			/* Fam15h BKDG Rev. 3.14 section 2.10.5.10.1 Table 104 */
			if (MaxDimmsInstallable == 1) {
				rank_count_dimm0 = pDCTstat->DimmRanks[(1 * 2) + dct];

				if (rank_count_dimm0 == 1)
					odt_tristate_code = 0xe;
				else
					odt_tristate_code = 0xa;
			} else if (MaxDimmsInstallable == 2) {
				if (dimm_count == 1) {
					/* 1 DIMM detected */
					rank_count_dimm1 = pDCTstat->DimmRanks[(1 * 2) + dct];

					if (rank_count_dimm1 == 1)
						odt_tristate_code = 0xd;
					else
						odt_tristate_code = 0x5;
				} else if (dimm_count == 2) {
					/* 2 DIMMs detected */
					rank_count_dimm0 = pDCTstat->DimmRanks[(0 * 2) + dct];
					rank_count_dimm1 = pDCTstat->DimmRanks[(1 * 2) + dct];

					if ((rank_count_dimm0 == 1) && (rank_count_dimm1 == 1))
						odt_tristate_code = 0xc;
					else if ((rank_count_dimm0 == 1) && (rank_count_dimm1 >= 2))
						odt_tristate_code = 0x4;
					else if ((rank_count_dimm0 >= 2) && (rank_count_dimm1 == 1))
						odt_tristate_code = 0x8;
					else
						odt_tristate_code = 0x0;
				}
			} else if (MaxDimmsInstallable == 3) {
				/* TODO
				 * 3 DIMM/channel support unimplemented
				 */
			}
		} else if (pDCTstat->Status & (1 << SB_LoadReduced)) {
			/* LRDIMM */

			/* TODO
			 * Implement LRDIMM support
			 * See Fam15h BKDG Rev. 3.14 section 2.10.5.10.1 Table 105
			 */
		} else {
			/* UDIMM */
			/* Fam15h BKDG Rev. 3.14 section 2.10.5.10.1 Table 103 */
			if (MaxDimmsInstallable == 1) {
				rank_count_dimm0 = pDCTstat->DimmRanks[(1 * 2) + dct];

				if (rank_count_dimm0 == 1)
					odt_tristate_code = 0xe;
				else
					odt_tristate_code = 0xa;
			} else if (MaxDimmsInstallable == 2) {
				if (dimm_count == 1) {
					/* 1 DIMM detected */
					rank_count_dimm0 = pDCTstat->DimmRanks[(1 * 2) + dct];

					if (rank_count_dimm0 == 1)
						odt_tristate_code = 0xd;
					else
						odt_tristate_code = 0x5;
				} else if (dimm_count == 2) {
					/* 2 DIMMs detected */
					rank_count_dimm0 = pDCTstat->DimmRanks[(0 * 2) + dct];
					rank_count_dimm1 = pDCTstat->DimmRanks[(1 * 2) + dct];

					if ((rank_count_dimm0 == 1) && (rank_count_dimm1 == 1))
						odt_tristate_code = 0xc;
					else if ((rank_count_dimm0 == 1) && (rank_count_dimm1 == 2))
						odt_tristate_code = 0x4;
					else if ((rank_count_dimm0 == 2) && (rank_count_dimm1 == 1))
						odt_tristate_code = 0x8;
					else
						odt_tristate_code = 0x0;
				}
			} else if (MaxDimmsInstallable == 3) {
				/* TODO
				 * 3 DIMM/channel support unimplemented
				 */
			}
		}
	} else if (package_type == PT_C3) {
		/* Socket C32 */
		if (pDCTstat->Status & (1 << SB_Registered)) {
			/* RDIMM */
			/* Fam15h BKDG Rev. 3.14 section 2.10.5.10.1 Table 107 */
			if (MaxDimmsInstallable == 1) {
				rank_count_dimm0 = pDCTstat->DimmRanks[(1 * 2) + dct];

				if (rank_count_dimm0 == 1)
					odt_tristate_code = 0xe;
				else
					odt_tristate_code = 0xa;
			} else if (MaxDimmsInstallable == 2) {
				if (dimm_count == 1) {
					/* 1 DIMM detected */
					rank_count_dimm1 = pDCTstat->DimmRanks[(1 * 2) + dct];

					if (rank_count_dimm1 == 1)
						odt_tristate_code = 0xd;
					else
						odt_tristate_code = 0x5;
				} else if (dimm_count == 2) {
					/* 2 DIMMs detected */
					rank_count_dimm0 = pDCTstat->DimmRanks[(0 * 2) + dct];
					rank_count_dimm1 = pDCTstat->DimmRanks[(1 * 2) + dct];

					if ((rank_count_dimm0 == 1) && (rank_count_dimm1 == 1))
						odt_tristate_code = 0xc;
					else if ((rank_count_dimm0 == 1) && (rank_count_dimm1 >= 2))
						odt_tristate_code = 0x4;
					else if ((rank_count_dimm0 >= 2) && (rank_count_dimm1 == 1))
						odt_tristate_code = 0x8;
					else
						odt_tristate_code = 0x0;
				}
			} else if (MaxDimmsInstallable == 3) {
				/* TODO
				 * 3 DIMM/channel support unimplemented
				 */
			}
		} else if (pDCTstat->Status & (1 << SB_LoadReduced)) {
			/* LRDIMM */

			/* TODO
			 * Implement LRDIMM support
			 * See Fam15h BKDG Rev. 3.14 section 2.10.5.10.1 Table 108
			 */
		} else {
			/* UDIMM */
			/* Fam15h BKDG Rev. 3.14 section 2.10.5.10.1 Table 106 */
			if (MaxDimmsInstallable == 1) {
				rank_count_dimm0 = pDCTstat->DimmRanks[(1 * 2) + dct];

				if (rank_count_dimm0 == 1)
					odt_tristate_code = 0xe;
				else
					odt_tristate_code = 0xa;
			} else if (MaxDimmsInstallable == 2) {
				if (dimm_count == 1) {
					/* 1 DIMM detected */
					rank_count_dimm0 = pDCTstat->DimmRanks[(1 * 2) + dct];

					if (rank_count_dimm0 == 1)
						odt_tristate_code = 0xd;
					else
						odt_tristate_code = 0x5;
				} else if (dimm_count == 2) {
					/* 2 DIMMs detected */
					rank_count_dimm0 = pDCTstat->DimmRanks[(0 * 2) + dct];
					rank_count_dimm1 = pDCTstat->DimmRanks[(1 * 2) + dct];

					if ((rank_count_dimm0 == 1) && (rank_count_dimm1 == 1))
						odt_tristate_code = 0xc;
					else if ((rank_count_dimm0 == 1) && (rank_count_dimm1 == 2))
						odt_tristate_code = 0x4;
					else if ((rank_count_dimm0 == 2) && (rank_count_dimm1 == 1))
						odt_tristate_code = 0x8;
					else
						odt_tristate_code = 0x0;
				}
			} else if (MaxDimmsInstallable == 3) {
				/* TODO
				 * 3 DIMM/channel support unimplemented
				 */
			}
		}
	} else if (package_type == PT_FM2) {
		/* Socket FM2 */
		/* UDIMM */
		odt_tristate_code = 0x0;
	} else {
		/* TODO
		 * Other socket support unimplemented
		 */
	}

	return odt_tristate_code;
}

static uint8_t fam15h_cs_tristate_enable_code(struct DCTStatStruc *pDCTstat, uint8_t dct)
{
	uint8_t MaxDimmsInstallable = mctGet_NVbits(NV_MAX_DIMMS_PER_CH);

	uint8_t package_type;
	uint8_t cs_tristate_code = 0;

	package_type = mctGet_NVbits(NV_PACK_TYPE);

	/* Obtain number of DIMMs on channel */
	uint8_t dimm_count = pDCTstat->MAdimms[dct];
	uint8_t rank_count_dimm0;
	uint8_t rank_count_dimm1;

	if (package_type == PT_GR) {
		/* Socket G34 */
		if (pDCTstat->Status & (1 << SB_Registered)) {
			/* RDIMM */
			/* Fam15h BKDG Rev. 3.14 section 2.10.5.10.1 Table 104 */
			if (MaxDimmsInstallable == 1) {
				rank_count_dimm0 = pDCTstat->DimmRanks[(1 * 2) + dct];

				if (rank_count_dimm0 < 4)
					cs_tristate_code = 0xfc;
				else
					cs_tristate_code = 0xcc;
			} else if (MaxDimmsInstallable == 2) {
				if (dimm_count == 1) {
					/* 1 DIMM detected */
					rank_count_dimm1 = pDCTstat->DimmRanks[(1 * 2) + dct];

					if (rank_count_dimm1 < 4)
						cs_tristate_code = 0xf3;
					else
						cs_tristate_code = 0x33;
				} else if (dimm_count == 2) {
					/* 2 DIMMs detected */
					rank_count_dimm0 = pDCTstat->DimmRanks[(0 * 2) + dct];
					rank_count_dimm1 = pDCTstat->DimmRanks[(1 * 2) + dct];

					if ((rank_count_dimm0 < 4) && (rank_count_dimm1 < 4))
						cs_tristate_code = 0xf0;
					else if ((rank_count_dimm0 < 4) && (rank_count_dimm1 == 4))
						cs_tristate_code = 0x30;
					else if ((rank_count_dimm0 == 4) && (rank_count_dimm1 < 4))
						cs_tristate_code = 0xc0;
					else
						cs_tristate_code = 0x0;
				}
			} else if (MaxDimmsInstallable == 3) {
				/* TODO
				 * 3 DIMM/channel support unimplemented
				 */
			}
		} else if (pDCTstat->Status & (1 << SB_LoadReduced)) {
			/* LRDIMM */

			/* TODO
			 * Implement LRDIMM support
			 * See Fam15h BKDG Rev. 3.14 section 2.10.5.10.1 Table 105
			 */
		} else {
			/* UDIMM */
			/* Fam15h BKDG Rev. 3.14 section 2.10.5.10.1 Table 103 */
			if (MaxDimmsInstallable == 1) {
				rank_count_dimm0 = pDCTstat->DimmRanks[(1 * 2) + dct];

				if (rank_count_dimm0 == 1)
					cs_tristate_code = 0xfe;
				else
					cs_tristate_code = 0xfc;
			} else if (MaxDimmsInstallable == 2) {
				if (dimm_count == 1) {
					/* 1 DIMM detected */
					rank_count_dimm0 = pDCTstat->DimmRanks[(1 * 2) + dct];

					if (rank_count_dimm0 == 1)
						cs_tristate_code = 0xfb;
					else
						cs_tristate_code = 0xf3;
				} else if (dimm_count == 2) {
					/* 2 DIMMs detected */
					rank_count_dimm0 = pDCTstat->DimmRanks[(0 * 2) + dct];
					rank_count_dimm1 = pDCTstat->DimmRanks[(1 * 2) + dct];

					if ((rank_count_dimm0 == 1) && (rank_count_dimm1 == 1))
						cs_tristate_code = 0xfa;
					else if ((rank_count_dimm0 == 1) && (rank_count_dimm1 == 2))
						cs_tristate_code = 0xf2;
					else if ((rank_count_dimm0 == 2) && (rank_count_dimm1 == 1))
						cs_tristate_code = 0xf8;
					else
						cs_tristate_code = 0xf0;
				}
			} else if (MaxDimmsInstallable == 3) {
				/* TODO
				 * 3 DIMM/channel support unimplemented
				 */
			}
		}
	} else if (package_type == PT_C3) {
		/* Socket C32 */
		if (pDCTstat->Status & (1 << SB_Registered)) {
			/* RDIMM */
			/* Fam15h BKDG Rev. 3.14 section 2.10.5.10.1 Table 107 */
			if (MaxDimmsInstallable == 1) {
				rank_count_dimm0 = pDCTstat->DimmRanks[(1 * 2) + dct];

				if (rank_count_dimm0 < 4)
					cs_tristate_code = 0xfc;
				else
					cs_tristate_code = 0xcc;
			} else if (MaxDimmsInstallable == 2) {
				if (dimm_count == 1) {
					/* 1 DIMM detected */
					rank_count_dimm1 = pDCTstat->DimmRanks[(1 * 2) + dct];

					if (rank_count_dimm1 < 4)
						cs_tristate_code = 0xf3;
					else
						cs_tristate_code = 0x33;
				} else if (dimm_count == 2) {
					/* 2 DIMMs detected */
					rank_count_dimm0 = pDCTstat->DimmRanks[(0 * 2) + dct];
					rank_count_dimm1 = pDCTstat->DimmRanks[(1 * 2) + dct];

					if ((rank_count_dimm0 < 4) && (rank_count_dimm1 < 4))
						cs_tristate_code = 0xf0;
					else if ((rank_count_dimm0 < 4) && (rank_count_dimm1 == 4))
						cs_tristate_code = 0x30;
					else if ((rank_count_dimm0 == 4) && (rank_count_dimm1 < 4))
						cs_tristate_code = 0xc0;
					else
						cs_tristate_code = 0x0;
				}
			} else if (MaxDimmsInstallable == 3) {
				/* TODO
				 * 3 DIMM/channel support unimplemented
				 */
			}
		} else if (pDCTstat->Status & (1 << SB_LoadReduced)) {
			/* LRDIMM */

			/* TODO
			 * Implement LRDIMM support
			 * See Fam15h BKDG Rev. 3.14 section 2.10.5.10.1 Table 108
			 */
		} else {
			/* UDIMM */
			/* Fam15h BKDG Rev. 3.14 section 2.10.5.10.1 Table 106 */
			if (MaxDimmsInstallable == 1) {
				rank_count_dimm0 = pDCTstat->DimmRanks[(1 * 2) + dct];

				if (rank_count_dimm0 == 1)
					cs_tristate_code = 0xfe;
				else
					cs_tristate_code = 0xfc;
			} else if (MaxDimmsInstallable == 2) {
				if (dimm_count == 1) {
					/* 1 DIMM detected */
					rank_count_dimm0 = pDCTstat->DimmRanks[(1 * 2) + dct];

					if (rank_count_dimm0 == 1)
						cs_tristate_code = 0xfb;
					else
						cs_tristate_code = 0xf3;
				} else if (dimm_count == 2) {
					/* 2 DIMMs detected */
					rank_count_dimm0 = pDCTstat->DimmRanks[(0 * 2) + dct];
					rank_count_dimm1 = pDCTstat->DimmRanks[(1 * 2) + dct];

					if ((rank_count_dimm0 == 1) && (rank_count_dimm1 == 1))
						cs_tristate_code = 0xfa;
					else if ((rank_count_dimm0 == 1) && (rank_count_dimm1 == 2))
						cs_tristate_code = 0xf2;
					else if ((rank_count_dimm0 == 2) && (rank_count_dimm1 == 1))
						cs_tristate_code = 0xf8;
					else
						cs_tristate_code = 0xf0;
				}
			} else if (MaxDimmsInstallable == 3) {
				/* TODO
				 * 3 DIMM/channel support unimplemented
				 */
			}
		}
	} else if (package_type == PT_FM2) {
		/* Socket FM2 */
		/* UDIMM */
		cs_tristate_code = 0x0;
	} else {
		/* TODO
		 * Other socket support unimplemented
		 */
	}

	return cs_tristate_code;
}

void set_2t_configuration(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct)
{
	printk(BIOS_DEBUG, "%s: Start\n", __func__);

	uint32_t dev;
	uint32_t reg;
	uint32_t dword;

	uint8_t enable_slow_access_mode = 0;
	dev = pDCTstat->dev_dct;

	if (is_fam15h()) {
		if (pDCTstat->_2Tmode)
			enable_slow_access_mode = 1;
	} else {
		if (pDCTstat->_2Tmode == 2)
			enable_slow_access_mode = 1;
	}

	reg = 0x94;				/* DRAM Configuration High */
	dword = Get_NB32_DCT(dev, dct, reg);
	if (enable_slow_access_mode)
		dword |= (0x1 << 20);		/* Set 2T CMD mode */
	else
		dword &= ~(0x1 << 20);		/* Clear 2T CMD mode */
	Set_NB32_DCT(dev, dct, reg, dword);

	printk(BIOS_DEBUG, "%s: Done\n", __func__);
}

void precise_ndelay_fam15(struct MCTStatStruc *pMCTstat, uint32_t nanoseconds) {
	msr_t tsc_msr;
	uint64_t cycle_count = (((uint64_t)pMCTstat->TSCFreq) * nanoseconds) / 1000;
	uint64_t start_timestamp;
	uint64_t current_timestamp;

	tsc_msr = rdmsr(0x00000010);
	start_timestamp = (((uint64_t)tsc_msr.hi) << 32) | tsc_msr.lo;
	do {
		tsc_msr = rdmsr(0x00000010);
		current_timestamp = (((uint64_t)tsc_msr.hi) << 32) | tsc_msr.lo;
	} while ((current_timestamp - start_timestamp) < cycle_count);
}

void precise_memclk_delay_fam15(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat, uint8_t dct, uint32_t clocks) {
	uint16_t memclk_freq;
	uint32_t delay_ns;
	uint16_t fam15h_freq_tab[] = {0, 0, 0, 0, 333, 0, 400, 0, 0, 0, 533, 0, 0, 0, 667, 0, 0, 0, 800, 0, 0, 0, 933};

	memclk_freq = Get_NB32_DCT(pDCTstat->dev_dct, dct, 0x94) & 0x1f;

	if (fam15h_freq_tab[memclk_freq] == 0) {
		printk(BIOS_DEBUG, "ERROR: precise_memclk_delay_fam15 for DCT %d (delay %d clocks) failed to obtain valid memory frequency!"
			" (pDCTstat: %p pDCTstat->dev_dct: %08x memclk_freq: %02x)\n", dct, clocks, pDCTstat, pDCTstat->dev_dct, memclk_freq);
	}
	delay_ns = (((uint64_t)clocks * 1000) / fam15h_freq_tab[memclk_freq]);
	precise_ndelay_fam15(pMCTstat, delay_ns);
}

static void read_spd_bytes(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, uint8_t dimm)
{
	uint8_t addr;
	uint16_t byte;

	addr = Get_DIMMAddress_D(pDCTstat, dimm);
	pDCTstat->spd_data.spd_address[dimm] = addr;

	for (byte = 0; byte < 256; byte++) {
		pDCTstat->spd_data.spd_bytes[dimm][byte] = mctRead_SPD(addr, byte);
	}
}

#ifdef DEBUG_DIMM_SPD
static void dump_spd_bytes(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, uint8_t dimm)
{
	uint16_t byte;

	printk(BIOS_DEBUG, "SPD dump for DIMM %d\n   ", dimm);
	for (byte = 0; byte < 16; byte++) {
		printk(BIOS_DEBUG, "%02x ", byte);
	}
	for (byte = 0; byte < 256; byte++) {
		if ((byte & 0xf) == 0x0) {
			printk(BIOS_DEBUG, "\n%02x ", byte >> 4);
		}
		printk(BIOS_DEBUG, "%02x ", pDCTstat->spd_data.spd_bytes[dimm][byte]);
	}
	printk(BIOS_DEBUG, "\n");
}
#endif

static void set_up_cc6_storage_fam15(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, uint8_t num_nodes)
{
	uint8_t interleaved;
	uint8_t destination_node;
	int8_t range;
	int8_t max_range;
	uint8_t max_node;
	uint64_t max_range_limit;
	uint8_t byte;
	uint32_t dword;
	uint32_t dword2;
	uint64_t qword;

	interleaved = 0;
	if (pMCTstat->GStatus & (1 << GSB_NodeIntlv))
		interleaved = 1;

	printk(BIOS_INFO,
		"%s: Initializing CC6 DRAM storage area for node %d"
		" (interleaved: %d)\n",
		 __func__, pDCTstat->Node_ID, interleaved);

	/* Find highest DRAM range (DramLimitAddr) */
	max_node = 0;
	max_range = -1;
	max_range_limit = 0;
	for (range = 0; range < 8; range++) {
		dword = Get_NB32(pDCTstat->dev_map, 0x40 + (range * 0x8));
		if (!(dword & 0x3))
			continue;

		dword = Get_NB32(pDCTstat->dev_map, 0x44 + (range * 0x8));
		dword2 = Get_NB32(pDCTstat->dev_map, 0x144 + (range * 0x8));
		qword = 0xffffff;
		qword |= ((((uint64_t)dword) >> 16) & 0xffff) << 24;
		qword |= (((uint64_t)dword2) & 0xff) << 40;

		if (qword > max_range_limit) {
			max_range = range;
			max_range_limit = qword;
			max_node = dword & 0x7;
		}
	}

	if (max_range >= 0) {
		printk(BIOS_INFO,
			"%s:\toriginal (node %d) max_range_limit: %16llx DRAM"
			" limit: %16llx\n",
			__func__, max_node, max_range_limit,
			(((uint64_t)(Get_NB32(pDCTstat->dev_map, 0x124)
				 & 0x1fffff)) << 27) | 0x7ffffff);

		if (interleaved)
			/* Move upper limit down by 16M * the number of nodes */
			max_range_limit -= (0x1000000ULL * num_nodes);
		else
			/* Move upper limit down by 16M */
			max_range_limit -= 0x1000000ULL;

		printk(BIOS_INFO, "%s:\tnew max_range_limit: %16llx\n",
			__func__, max_range_limit);

		/* Disable the range */
		dword = Get_NB32(pDCTstat->dev_map, 0x40 + (max_range * 0x8));
		byte = dword & 0x3;
		dword &= ~(0x3);
		Set_NB32(pDCTstat->dev_map, 0x40 + (max_range * 0x8), dword);

		/* Store modified range */
		dword = Get_NB32(pDCTstat->dev_map, 0x44 + (max_range * 0x8));
		dword &= ~(0xffff << 16);		/* DramLimit[39:24] = max_range_limit[39:24] */
		dword |= ((max_range_limit >> 24) & 0xffff) << 16;
		Set_NB32(pDCTstat->dev_map, 0x44 + (max_range * 0x8), dword);

		dword = Get_NB32(pDCTstat->dev_map, 0x144 + (max_range * 0x8));
		dword &= ~0xff;			/* DramLimit[47:40] = max_range_limit[47:40] */
		dword |= (max_range_limit >> 40) & 0xff;
		Set_NB32(pDCTstat->dev_map, 0x144 + (max_range * 0x8), dword);

		/* Reenable the range */
		dword = Get_NB32(pDCTstat->dev_map, 0x40 + (max_range * 0x8));
		dword |= byte;
		Set_NB32(pDCTstat->dev_map, 0x40 + (max_range * 0x8), dword);
	}

	/* Determine save state destination node */
	if (interleaved)
		destination_node = Get_NB32(pDCTstat->dev_host, 0x60) & 0x7;
	else
		destination_node = max_node;

	/* Set save state destination node */
	dword = Get_NB32(pDCTstat->dev_link, 0x128);
	dword &= ~(0x3f << 12);				/* CoreSaveStateDestNode = destination_node */
	dword |= (destination_node & 0x3f) << 12;
	Set_NB32(pDCTstat->dev_link, 0x128, dword);

	printk(BIOS_INFO, "%s:\tTarget node: %d\n", __func__, destination_node);

	printk(BIOS_INFO, "%s:\tDone\n", __func__);
}

static void lock_dram_config(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat)
{
	uint32_t dword;

	dword = Get_NB32(pDCTstat->dev_dct, 0x118);
	dword |= 0x1 << 19;		/* LockDramCfg = 1 */
	Set_NB32(pDCTstat->dev_dct, 0x118, dword);
}

static void set_cc6_save_enable(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, uint8_t enable)
{
	uint32_t dword;

	dword = Get_NB32(pDCTstat->dev_dct, 0x118);
	dword &= ~(0x1 << 18);		/* CC6SaveEn = enable */
	dword |= (enable & 0x1) << 18;
	Set_NB32(pDCTstat->dev_dct, 0x118, dword);
}

void mctAutoInitMCT_D(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstatA)
{
	/*
	 * Memory may be mapped contiguously all the way up to 4GB (depending on setup
	 * options).  It is the responsibility of PCI subsystem to create an uncacheable
	 * IO region below 4GB and to adjust TOP_MEM downward prior to any IO mapping or
	 * accesses.  It is the same responsibility of the CPU sub-system prior to
	 * accessing LAPIC.
	 *
	 * Slot Number is an external convention, and is determined by OEM with accompanying
	 * silk screening.  OEM may choose to use Slot number convention which is consistent
	 * with DIMM number conventions.  All AMD engineering platforms do.
	 *
	 * Build Requirements:
	 * 1. MCT_SEG0_START and MCT_SEG0_END macros to begin and end the code segment,
	 *    defined in mcti.inc.
	 *
	 * Run-Time Requirements:
	 * 1. Complete Hypertransport Bus Configuration
	 * 2. SMBus Controller Initialized
	 * 1. BSP in Big Real Mode
	 * 2. Stack at SS:SP, located somewhere between A000:0000 and F000:FFFF
	 * 3. Checksummed or Valid NVRAM bits
	 * 4. MCG_CTL = -1, MC4_CTL_EN = 0 for all CPUs
	 * 5. MCi_STS from shutdown/warm reset recorded (if desired) prior to entry
	 * 6. All var MTRRs reset to zero
	 * 7. State of NB_CFG.DisDatMsk set properly on all CPUs
	 * 8. All CPUs at 2GHz Speed (unless DQS training is not installed).
	 * 9. All cHT links at max Speed/Width (unless DQS training is not installed).
	 *
	 *
	 * Global relationship between index values and item values:
	 *
	 * pDCTstat.CASL pDCTstat.Speed
	 * j CL(j)       k   F(k)
	 * --------------------------
	 * 0 2.0         -   -
	 * 1 3.0         1   200 MHz
	 * 2 4.0         2   266 MHz
	 * 3 5.0         3   333 MHz
	 * 4 6.0         4   400 MHz
	 * 5 7.0         5   533 MHz
	 * 6 8.0         6   667 MHz
	 * 7 9.0         7   800 MHz
	 */
	u8 Node, NodesWmem;
	u32 node_sys_base;
	uint8_t dimm;
	uint8_t nvram;
	uint8_t enable_cc6;
	uint8_t ecc_enabled;
	uint8_t allow_config_restore;

	uint8_t s3resume = acpi_is_wakeup_s3();

restartinit:

	if (!mctGet_NVbits(NV_ECC_CAP) || !mctGet_NVbits(NV_ECC))
		pMCTstat->try_ecc = 0;
	else
		pMCTstat->try_ecc = 1;

	mctInitMemGPIOs_A_D();		/* Set any required GPIOs*/
	if (s3resume) {
		printk(BIOS_DEBUG, "mctAutoInitMCT_D: mct_ForceNBPState0_En_Fam15\n");
		for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
			struct DCTStatStruc *pDCTstat;
			pDCTstat = pDCTstatA + Node;

			mct_ForceNBPState0_En_Fam15(pMCTstat, pDCTstat);
		}

#if IS_ENABLED(CONFIG_HAVE_ACPI_RESUME)
		printk(BIOS_DEBUG, "mctAutoInitMCT_D: Restoring DCT configuration from NVRAM\n");
		if (restore_mct_information_from_nvram(0) != 0)
			printk(BIOS_CRIT, "%s: ERROR: Unable to restore DCT configuration from NVRAM\n", __func__);
		pMCTstat->GStatus |= 1 << GSB_ConfigRestored;
#endif

		if (is_fam15h()) {
			printk(BIOS_DEBUG, "mctAutoInitMCT_D: mct_ForceNBPState0_Dis_Fam15\n");
			for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
				struct DCTStatStruc *pDCTstat;
				pDCTstat = pDCTstatA + Node;

				mct_ForceNBPState0_Dis_Fam15(pMCTstat, pDCTstat);
			}
		}
	} else {
		NodesWmem = 0;
		node_sys_base = 0;
		for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
			struct DCTStatStruc *pDCTstat;
			pDCTstat = pDCTstatA + Node;

			/* Zero out data structures to avoid false detection of DIMMs */
			memset(pDCTstat, 0, sizeof(struct DCTStatStruc));

			/* Initialize data structures */
			pDCTstat->Node_ID = Node;
			pDCTstat->dev_host = PA_HOST(Node);
			pDCTstat->dev_map = PA_MAP(Node);
			pDCTstat->dev_dct = PA_DCT(Node);
			pDCTstat->dev_nbmisc = PA_NBMISC(Node);
			pDCTstat->dev_link = PA_LINK(Node);
			pDCTstat->dev_nbctl = PA_NBCTL(Node);
			pDCTstat->NodeSysBase = node_sys_base;

			if (mctGet_NVbits(NV_PACK_TYPE) == PT_GR) {
				uint32_t dword;
				pDCTstat->Dual_Node_Package = 1;

				/* Get the internal node number */
				dword = Get_NB32(pDCTstat->dev_nbmisc, 0xe8);
				dword = (dword >> 30) & 0x3;
				pDCTstat->Internal_Node_ID = dword;
			} else {
				pDCTstat->Dual_Node_Package = 0;
			}

			printk(BIOS_DEBUG, "%s: mct_init Node %d\n", __func__, Node);
			mct_init(pMCTstat, pDCTstat);
			mctNodeIDDebugPort_D();
			pDCTstat->NodePresent = NodePresent_D(Node);
			if (pDCTstat->NodePresent) {
				pDCTstat->LogicalCPUID = mctGetLogicalCPUID_D(Node);

				printk(BIOS_DEBUG, "%s: mct_InitialMCT_D\n", __func__);
				mct_InitialMCT_D(pMCTstat, pDCTstat);

				printk(BIOS_DEBUG, "%s: mctSMBhub_Init\n", __func__);
				mctSMBhub_Init(Node);		/* Switch SMBUS crossbar to proper node */

				printk(BIOS_DEBUG, "%s: mct_preInitDCT\n", __func__);
				mct_preInitDCT(pMCTstat, pDCTstat);
			}
			node_sys_base = pDCTstat->NodeSysBase;
			node_sys_base += (pDCTstat->NodeSysLimit + 2) & ~0x0F;
		}

		/* If the boot fails make sure training is attempted after reset */
		nvram = 0;
		set_option("allow_spd_nvram_cache_restore", &nvram);

#if IS_ENABLED(CONFIG_DIMM_VOLTAGE_SET_SUPPORT)
		printk(BIOS_DEBUG, "%s: DIMMSetVoltage\n", __func__);
		DIMMSetVoltages(pMCTstat, pDCTstatA);	/* Set the DIMM voltages (mainboard specific) */
#endif
		if (!IS_ENABLED(CONFIG_DIMM_VOLTAGE_SET_SUPPORT)) {
			/* Assume 1.5V operation */
			for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
				struct DCTStatStruc *pDCTstat;
				pDCTstat = pDCTstatA + Node;

				if (!pDCTstat->NodePresent)
					continue;

				for (dimm = 0; dimm < MAX_DIMMS_SUPPORTED; dimm++) {
					if (pDCTstat->DIMMValid & (1 << dimm))
						pDCTstat->DimmConfiguredVoltage[dimm] = 0x1;
				}
			}
		}

		/* If DIMM configuration has not changed since last boot restore training values */
		allow_config_restore = 1;
		for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
			struct DCTStatStruc *pDCTstat;
			pDCTstat = pDCTstatA + Node;

			if (pDCTstat->NodePresent)
				if (!pDCTstat->spd_data.nvram_spd_match)
					allow_config_restore = 0;
		}

		/* FIXME
		 * Stability issues have arisen on multiple Family 15h systems
		 * when configuration restoration is enabled.  In all cases these
		 * stability issues resolved by allowing the RAM to go through a
		 * full training cycle.
		 *
		 * Debug and reenable this!
		 */
		allow_config_restore = 0;

		for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
			struct DCTStatStruc *pDCTstat;
			pDCTstat = pDCTstatA + Node;

			if (pDCTstat->NodePresent) {
				printk(BIOS_DEBUG, "%s: mctSMBhub_Init\n", __func__);
				mctSMBhub_Init(Node);		/* Switch SMBUS crossbar to proper node*/

				printk(BIOS_DEBUG, "%s: mct_initDCT\n", __func__);
				mct_initDCT(pMCTstat, pDCTstat);
				if (pDCTstat->ErrCode == SC_FatalErr) {
					goto fatalexit;		/* any fatal errors?*/
				} else if (pDCTstat->ErrCode < SC_StopError) {
					NodesWmem++;
				}
			}
		}
		if (NodesWmem == 0) {
			printk(BIOS_ALERT, "Unable to detect valid memory on any nodes.  Halting!\n");
			goto fatalexit;
		}

		printk(BIOS_DEBUG, "mctAutoInitMCT_D: SyncDCTsReady_D\n");
		SyncDCTsReady_D(pMCTstat, pDCTstatA);	/* Make sure DCTs are ready for accesses.*/

		printk(BIOS_DEBUG, "mctAutoInitMCT_D: HTMemMapInit_D\n");
		HTMemMapInit_D(pMCTstat, pDCTstatA);	/* Map local memory into system address space.*/
		mctHookAfterHTMap();

		if (!is_fam15h()) {
			printk(BIOS_DEBUG, "mctAutoInitMCT_D: CPUMemTyping_D\n");
			CPUMemTyping_D(pMCTstat, pDCTstatA);	/* Map dram into WB/UC CPU cacheability */
		}

		printk(BIOS_DEBUG, "mctAutoInitMCT_D: mctHookAfterCPU\n");
		mctHookAfterCPU();			/* Setup external northbridge(s) */

		/* FIXME
		 * Previous training values should only be used if the current desired
		 * speed is the same as the speed used in the previous boot.
		 * How to get the desired speed at this point in the code?
		 */

		printk(BIOS_DEBUG, "mctAutoInitMCT_D: DQSTiming_D\n");
		DQSTiming_D(pMCTstat, pDCTstatA, allow_config_restore);	/* Get Receiver Enable and DQS signal timing*/

		if (!is_fam15h()) {
			printk(BIOS_DEBUG, "mctAutoInitMCT_D: UMAMemTyping_D\n");
			UMAMemTyping_D(pMCTstat, pDCTstatA);	/* Fix up for UMA sizing */
		}

		if (!allow_config_restore) {
			printk(BIOS_DEBUG, "mctAutoInitMCT_D: :OtherTiming\n");
			mct_OtherTiming(pMCTstat, pDCTstatA);
		}

		if (ReconfigureDIMMspare_D(pMCTstat, pDCTstatA)) { /* RESET# if 1st pass of DIMM spare enabled*/
			goto restartinit;
		}

		InterleaveNodes_D(pMCTstat, pDCTstatA);
		InterleaveChannels_D(pMCTstat, pDCTstatA);

		ecc_enabled = 1;
		for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
			struct DCTStatStruc *pDCTstat;
			pDCTstat = pDCTstatA + Node;

			if (pDCTstat->NodePresent)
				if (!is_ecc_enabled(pMCTstat, pDCTstat))
					ecc_enabled = 0;
		}

		if (ecc_enabled) {
			printk(BIOS_DEBUG, "mctAutoInitMCT_D: ECCInit_D\n");
			if (!ECCInit_D(pMCTstat, pDCTstatA)) {			/* Setup ECC control and ECC check-bits*/
				/* Memory was not cleared during ECC setup */
				/* mctDoWarmResetMemClr_D(); */
				printk(BIOS_DEBUG, "mctAutoInitMCT_D: MCTMemClr_D\n");
				MCTMemClr_D(pMCTstat,pDCTstatA);
			}
		}

		if (is_fam15h()) {
			printk(BIOS_DEBUG, "mctAutoInitMCT_D: CPUMemTyping_D\n");
			CPUMemTyping_D(pMCTstat, pDCTstatA);	/* Map dram into WB/UC CPU cacheability */

			printk(BIOS_DEBUG, "mctAutoInitMCT_D: UMAMemTyping_D\n");
			UMAMemTyping_D(pMCTstat, pDCTstatA);	/* Fix up for UMA sizing */

			printk(BIOS_DEBUG, "mctAutoInitMCT_D: mct_ForceNBPState0_Dis_Fam15\n");
			for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
				struct DCTStatStruc *pDCTstat;
				pDCTstat = pDCTstatA + Node;

				mct_ForceNBPState0_Dis_Fam15(pMCTstat, pDCTstat);
			}
		}

		if (is_fam15h()) {
			enable_cc6 = 0;
			if (get_option(&nvram, "cpu_cc6_state") == CB_SUCCESS)
				enable_cc6 = !!nvram;

			if (enable_cc6) {
				uint8_t num_nodes;

				num_nodes = 0;
				for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
					struct DCTStatStruc *pDCTstat;
					pDCTstat = pDCTstatA + Node;

					if (pDCTstat->NodePresent)
						num_nodes++;
				}

				for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
					struct DCTStatStruc *pDCTstat;
					pDCTstat = pDCTstatA + Node;

					if (pDCTstat->NodePresent)
						set_up_cc6_storage_fam15(pMCTstat, pDCTstat, num_nodes);
				}

				for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
					struct DCTStatStruc *pDCTstat;
					pDCTstat = pDCTstatA + Node;

					if (pDCTstat->NodePresent) {
						set_cc6_save_enable(pMCTstat, pDCTstat, 1);
						lock_dram_config(pMCTstat, pDCTstat);
					}
				}
			}
		}

		mct_FinalMCT_D(pMCTstat, pDCTstatA);
		printk(BIOS_DEBUG, "mctAutoInitMCT_D Done: Global Status: %x\n", pMCTstat->GStatus);
	}

	return;

fatalexit:
	die("mct_d: fatalexit");
}

void initialize_mca(uint8_t bsp, uint8_t suppress_errors) {
	uint8_t node;
	uint32_t mc4_status_high;
	uint32_t mc4_status_low;

	for (node = 0; node < MAX_NODES_SUPPORTED; node++) {
		if (bsp && (node > 0))
			break;

		mc4_status_high = pci_read_config32(PCI_DEV(0, 0x18 + node, 3), 0x4c);
		mc4_status_low = pci_read_config32(PCI_DEV(0, 0x18 + node, 3), 0x48);
		if ((mc4_status_high & (0x1 << 31)) && (mc4_status_high != 0xffffffff)) {
			if (!suppress_errors)
				printk(BIOS_WARNING, "WARNING: MC4 Machine Check Exception detected on node %d!\n"
					"Signature: %08x%08x\n", node, mc4_status_high, mc4_status_low);

			/* Clear MC4 error status */
			pci_write_config32(PCI_DEV(0, 0x18 + node, 3), 0x48, 0x0);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 3), 0x4c, 0x0);
		}
	}
}

static u8 ReconfigureDIMMspare_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstatA)
{
	u8 ret;

	if (mctGet_NVbits(NV_CS_SpareCTL)) {
		if (MCT_DIMM_SPARE_NO_WARM) {
			/* Do no warm-reset DIMM spare */
			if (pMCTstat->GStatus & (1 << GSB_EnDIMMSpareNW)) {
				LoadDQSSigTmgRegs_D(pMCTstat, pDCTstatA);
				ret = 0;
			} else {
				mct_ResetDataStruct_D(pMCTstat, pDCTstatA);
				pMCTstat->GStatus |= 1 << GSB_EnDIMMSpareNW;
				ret = 1;
			}
		} else {
			/* Do warm-reset DIMM spare */
			if (mctGet_NVbits(NV_DQSTrainCTL))
				mctWarmReset_D();
			ret = 0;
		}
	} else {
		ret = 0;
	}

	return ret;
}

/* Enable or disable phy-assisted training mode
 * Phy-assisted training mode applies to the follow DRAM training procedures:
 * Write Levelization Training (2.10.5.8.1)
 * DQS Receiver Enable Training (2.10.5.8.2)
 */
void fam15EnableTrainingMode(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, uint8_t dct, uint8_t enable)
{
	uint8_t index;
	uint32_t dword;
	uint32_t index_reg = 0x98;
	uint32_t dev = pDCTstat->dev_dct;

	if (enable) {
		/* Enable training mode */
		dword = Get_NB32_DCT(dev, dct, 0x78);			/* DRAM Control */
		dword &= ~(0x1 << 17);					/* AddrCmdTriEn = 0 */
		Set_NB32_DCT(dev, dct, 0x78, dword);			/* DRAM Control */

		dword = Get_NB32_DCT(dev, dct, 0x8c);			/* DRAM Timing High */
		dword |= (0x1 << 18);					/* DisAutoRefresh = 1 */
		Set_NB32_DCT(dev, dct, 0x8c, dword);			/* DRAM Timing High */

		dword = Get_NB32_DCT(dev, dct, 0x94);			/* DRAM Configuration High */
		dword &= ~(0xf << 24);					/* DcqBypassMax = 0 */
		dword &= ~(0x1 << 22);					/* BankSwizzleMode = 0 */
		dword &= ~(0x1 << 15);					/* PowerDownEn = 0 */
		dword &= ~(0x3 << 10);					/* ZqcsInterval = 0 */
		Set_NB32_DCT(dev, dct, 0x94, dword);			/* DRAM Configuration High */

		dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0000000d);
		dword &= ~(0xf << 16);					/* RxMaxDurDllNoLock = 0 */
		dword &= ~(0xf);					/* TxMaxDurDllNoLock = 0 */
		Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0000000d, dword);

		for (index = 0; index < 0x9; index++) {
			dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f0010 | (index << 8));
			dword &= ~(0x1 << 12);				/* EnRxPadStandby = 0 */
			Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f0010 | (index << 8), dword);
		}

		dword = Get_NB32_DCT(dev, dct, 0xa4);			/* DRAM Controller Temperature Throttle */
		dword &= ~(0x1 << 11);					/* BwCapEn = 0 */
		dword &= ~(0x1 << 8);					/* ODTSEn = 0 */
		Set_NB32_DCT(dev, dct, 0xa4, dword);			/* DRAM Controller Temperature Throttle */

		dword = Get_NB32_DCT(dev, dct, 0x110);			/* DRAM Controller Select Low */
		dword &= ~(0x1 << 2);					/* DctSelIntLvEn = 0 */
		Set_NB32_DCT(dev, dct, 0x110, dword);			/* DRAM Controller Select Low */

		dword = Get_NB32_DCT(pDCTstat->dev_nbmisc, dct, 0x58);	/* Scrub Rate Control */
		dword &= ~(0x1f << 24);					/* L3Scrub = 0 */
		dword &= ~(0x1f);					/* DramScrub = 0 */
		Set_NB32_DCT(pDCTstat->dev_nbmisc, dct, 0x58, dword);	/* Scrub Rate Control */

		dword = Get_NB32_DCT(pDCTstat->dev_nbmisc, dct, 0x5c);	/* DRAM Scrub Address Low */
		dword &= ~(0x1);					/* ScrubReDirEn = 0 */
		Set_NB32_DCT(pDCTstat->dev_nbmisc, dct, 0x5c, dword);	/* DRAM Scrub Address Low */

		dword = Get_NB32_DCT(pDCTstat->dev_nbmisc, dct, 0x1b8);	/* L3 Control 1 */
		dword |= (0x1 << 4);					/* L3ScrbRedirDis = 1 */
		Set_NB32_DCT(pDCTstat->dev_nbmisc, dct, 0x1b8, dword);	/* L3 Control 1 */

		/* Fam15h BKDG section 2.10.5.5.1 */
		dword = Get_NB32_DCT(dev, dct, 0x218);			/* DRAM Timing 5 */
		dword &= ~(0xf << 24);					/* TrdrdSdSc = 0xb */
		dword |= (0xb << 24);
		dword &= ~(0xf << 16);					/* TrdrdSdDc = 0xb */
		dword |= (0xb << 16);
		dword &= ~(0xf);					/* TrdrdDd = 0xb */
		dword |= 0xb;
		Set_NB32_DCT(dev, dct, 0x218, dword);			/* DRAM Timing 5 */

		/* Fam15h BKDG section 2.10.5.5.2 */
		dword = Get_NB32_DCT(dev, dct, 0x214);			/* DRAM Timing 4 */
		dword &= ~(0xf << 16);					/* TwrwrSdSc = 0xb */
		dword |= (0xb << 16);
		dword &= ~(0xf << 8);					/* TwrwrSdDc = 0xb */
		dword |= (0xb << 8);
		dword &= ~(0xf);					/* TwrwrDd = 0xb */
		dword |= 0xb;
		Set_NB32_DCT(dev, dct, 0x214, dword);			/* DRAM Timing 4 */

		/* Fam15h BKDG section 2.10.5.5.3 */
		dword = Get_NB32_DCT(dev, dct, 0x218);			/* DRAM Timing 5 */
		dword &= ~(0xf << 8);					/* Twrrd = 0xb */
		dword |= (0xb << 8);
		Set_NB32_DCT(dev, dct, 0x218, dword);			/* DRAM Timing 5 */

		/* Fam15h BKDG section 2.10.5.5.4 */
		dword = Get_NB32_DCT(dev, dct, 0x21c);			/* DRAM Timing 6 */
		dword &= ~(0x1f << 8);					/* TrwtTO = 0x16 */
		dword |= (0x16 << 8);
		dword &= ~(0x1f << 16);					/* TrwtWB = TrwtTO + 1 */
		dword |= ((((dword >> 8) & 0x1f) + 1) << 16);
		Set_NB32_DCT(dev, dct, 0x21c, dword);			/* DRAM Timing 6 */
	} else {
		/* Disable training mode */
		uint8_t lane;
		uint8_t dimm;
		int16_t max_cdd_we_delta;
		int16_t cdd_trwtto_we_delta;
		uint8_t receiver;
		uint8_t lane_count;
		uint8_t x4_present = 0;
		uint8_t x8_present = 0;
		uint8_t memclk_index;
		uint8_t interleave_channels = 0;
		uint16_t trdrdsddc;
		uint16_t trdrddd;
		uint16_t cdd_trdrddd;
		uint16_t twrwrsddc;
		uint16_t twrwrdd;
		uint16_t cdd_twrwrdd;
		uint16_t twrrd;
		uint16_t cdd_twrrd;
		uint16_t cdd_trwtto;
		uint16_t trwtto;
		uint8_t first_dimm;
		uint16_t delay;
		uint16_t delay2;
		uint8_t min_value;
		uint8_t write_early;
		uint8_t read_odt_delay;
		uint8_t write_odt_delay;
		uint8_t buffer_data_delay;
		int16_t latency_difference;
		uint16_t difference;
		uint16_t current_total_delay_1[MAX_BYTE_LANES];
		uint16_t current_total_delay_2[MAX_BYTE_LANES];
		uint8_t ddr_voltage_index;
		uint8_t max_dimms_installable;

		/* FIXME
		 * This should be platform configurable
		 */
		uint8_t dimm_event_l_pin_support = 0;

		if (pDCTstat->DIMMValidDCT[dct] == 0)
			ddr_voltage_index = 1;
		else
			ddr_voltage_index = dct_ddr_voltage_index(pDCTstat, dct);

		ddr_voltage_index = dct_ddr_voltage_index(pDCTstat, dct);
		max_dimms_installable = mctGet_NVbits(NV_MAX_DIMMS_PER_CH);

		lane_count = get_available_lane_count(pMCTstat, pDCTstat);

		if (pDCTstat->Dimmx4Present & ((dct)?0xaa:0x55))
			x4_present = 1;
		if (pDCTstat->Dimmx8Present & ((dct)?0xaa:0x55))
			x8_present = 1;
		memclk_index = Get_NB32_DCT(dev, dct, 0x94) & 0x1f;

		if (pDCTstat->DIMMValidDCT[0] && pDCTstat->DIMMValidDCT[1] && mctGet_NVbits(NV_Unganged))
			interleave_channels = 1;

		dword = (Get_NB32_DCT(dev, dct, 0x240) >> 4) & 0xf;
		if (dword > 6)
			read_odt_delay = dword - 6;
		else
			read_odt_delay = 0;

		dword = Get_NB32_DCT(dev, dct, 0x240);
		delay = (dword >> 4) & 0xf;
		if (delay > 6)
			read_odt_delay = delay - 6;
		else
			read_odt_delay = 0;
		delay = (dword >> 12) & 0x7;
		if (delay > 6)
			write_odt_delay = delay - 6;
		else
			write_odt_delay = 0;

		dword = (Get_NB32_DCT(dev, dct, 0xa8) >> 24) & 0x3;
		write_early = dword / 2;

		latency_difference = Get_NB32_DCT(dev, dct, 0x200) & 0x1f;
		dword = Get_NB32_DCT(dev, dct, 0x20c) & 0x1f;
		latency_difference -= dword;

		if (pDCTstat->Status & (1 << SB_LoadReduced)) {
			/* LRDIMM */

			/* TODO
			 * Implement LRDIMM support
			 * See Fam15h BKDG Rev. 3.14 section 2.10.5.5
			 */
		} else {
			buffer_data_delay = 0;
		}

		/* TODO:
		 * Adjust trdrdsddc if four-rank DIMMs are installed per
		 * section 2.10.5.5.1 of the Family 15h BKDG.
		 * cdd_trdrdsddc will also need to be calculated in that process.
		 */
		trdrdsddc = 3;

		/* Calculate the Critical Delay Difference for TrdrdDd */
		cdd_trdrddd = 0;
		first_dimm = 1;
		for (receiver = 0; receiver < 8; receiver += 2) {
			dimm = (receiver >> 1);

			if (!mct_RcvrRankEnabled_D(pMCTstat, pDCTstat, dct, receiver))
				continue;

			read_dqs_receiver_enable_control_registers(current_total_delay_2, dev, dct, dimm, index_reg);

			if (first_dimm) {
				memcpy(current_total_delay_1, current_total_delay_2, sizeof(current_total_delay_1));
				first_dimm = 0;
			}

			for (lane = 0; lane < lane_count; lane++) {
				if (current_total_delay_1[lane] > current_total_delay_2[lane])
					difference = current_total_delay_1[lane] - current_total_delay_2[lane];
				else
					difference = current_total_delay_2[lane] - current_total_delay_1[lane];

				if (difference > cdd_trdrddd)
					cdd_trdrddd = difference;
			}
		}

		/* Convert the difference to MEMCLKs */
		cdd_trdrddd = (((cdd_trdrddd + (1 << 6) - 1) >> 6) & 0xf);

		/* Calculate Trdrddd */
		delay = (read_odt_delay + 3) * 2;
		delay2 = cdd_trdrddd + 7;
		if (delay2 > delay)
			delay = delay2;
		trdrddd = (delay + 1) / 2;	/* + 1 is equivalent to ceiling function here */
		if (trdrdsddc > trdrddd)
			trdrddd = trdrdsddc;

		/* TODO:
		 * Adjust twrwrsddc if four-rank DIMMs are installed per
		 * section 2.10.5.5.1 of the Family 15h BKDG.
		 * cdd_twrwrsddc will also need to be calculated in that process.
		 */
		twrwrsddc = 4;

		/* Calculate the Critical Delay Difference for TwrwrDd */
		cdd_twrwrdd = 0;
		first_dimm = 1;
		for (receiver = 0; receiver < 8; receiver += 2) {
			dimm = (receiver >> 1);

			if (!mct_RcvrRankEnabled_D(pMCTstat, pDCTstat, dct, receiver))
				continue;

			read_dqs_write_timing_control_registers(current_total_delay_2, dev, dct, dimm, index_reg);

			if (first_dimm) {
				memcpy(current_total_delay_1, current_total_delay_2, sizeof(current_total_delay_1));
				first_dimm = 0;
			}

			for (lane = 0; lane < lane_count; lane++) {
				if (current_total_delay_1[lane] > current_total_delay_2[lane])
					difference = current_total_delay_1[lane] - current_total_delay_2[lane];
				else
					difference = current_total_delay_2[lane] - current_total_delay_1[lane];

				if (difference > cdd_twrwrdd)
					cdd_twrwrdd = difference;
			}
		}

		/* Convert the difference to MEMCLKs */
		cdd_twrwrdd = (((cdd_twrwrdd + (1 << 6) - 1) >> 6) & 0xf);

		/* Calculate Twrwrdd */
		delay = (write_odt_delay + 3) * 2;
		delay2 = cdd_twrwrdd + 7;
		if (delay2 > delay)
			delay = delay2;
		twrwrdd = (delay + 1) / 2;	/* + 1 is equivalent to ceiling function here */
		if (twrwrsddc > twrwrdd)
			twrwrdd = twrwrsddc;

		dword = Get_NB32_DCT(dev, dct, 0x78);			/* DRAM Control */
		dword |= (0x1 << 17);					/* AddrCmdTriEn = 1 */
		Set_NB32_DCT(dev, dct, 0x78, dword);			/* DRAM Control */

		dword = Get_NB32_DCT(dev, dct, 0x8c);			/* DRAM Timing High */
		dword &= ~(0x1 << 18);					/* DisAutoRefresh = 0 */
		Set_NB32_DCT(dev, dct, 0x8c, dword);			/* DRAM Timing High */

		/* Configure power saving options */
		dword = Get_NB32_DCT(dev, dct, 0xa8);			/* Dram Miscellaneous 2 */
		dword |= (0x1 << 22);					/* PrtlChPDEnhEn = 0x1 */
		dword |= (0x1 << 21);					/* AggrPDEn = 0x1 */
		Set_NB32_DCT(dev, dct, 0xa8, dword);			/* Dram Miscellaneous 2 */

		/* Configure partial power down delay */
		dword = Get_NB32(dev, 0x244);				/* DRAM Controller Miscellaneous 3 */
		dword &= ~0xf;						/* PrtlChPDDynDly = 0x2 */
		dword |= 0x2;
		Set_NB32(dev, 0x244, dword);				/* DRAM Controller Miscellaneous 3 */

		/* Configure power save delays */
		delay = 0xa;
		delay2 = 0x3;

		/* Family 15h BKDG Table 214 */
		if ((pDCTstat->Status & (1 << SB_Registered))
			|| (pDCTstat->Status & (1 << SB_LoadReduced))) {
			if (memclk_index <= 0x6) {
				if (ddr_voltage_index < 0x4)
					/* 1.5 or 1.35V */
					delay2 = 0x3;
				else
					/* 1.25V */
					delay2 = 0x4;
			}
			else if ((memclk_index == 0xa)
				|| (memclk_index == 0xe))
				delay2 = 0x4;
			else if (memclk_index == 0x12)
				delay2 = 0x5;
			else if (memclk_index == 0x16)
				delay2 = 0x6;
		} else {
			if (memclk_index <= 0x6)
				delay2 = 0x3;
			else if ((memclk_index == 0xa)
				|| (memclk_index == 0xe))
				delay2 = 0x4;
			else if (memclk_index == 0x12)
				delay2 = 0x5;
			else if (memclk_index == 0x16)
				delay2 = 0x6;
		}

		/* Family 15h BKDG Table 215 */
		if (memclk_index <= 0x6)
			delay = 0xa;
		else if (memclk_index == 0xa)
			delay = 0xd;
		else if (memclk_index == 0xe)
			delay = 0x10;
		else if (memclk_index == 0x12)
			delay = 0x14;
		else if (memclk_index == 0x16)
			delay = 0x17;

		dword = Get_NB32_DCT(dev, dct, 0x248);			/* Dram Power Management 0 */
		dword &= ~(0x3f << 24);					/* AggrPDDelay = 0x0 */
		dword &= ~(0x3f << 16);					/* PchgPDEnDelay = 0x1 */
		dword |= (0x1 << 16);
		dword &= ~(0x1f << 8);					/* Txpdll = delay */
		dword |= ((delay & 0x1f) << 8);
		dword &= ~0xf;						/* Txp = delay2 */
		dword |= delay2 & 0xf;
		Set_NB32_DCT(dev, dct, 0x248, dword);			/* Dram Power Management 0 */

		/* Family 15h BKDG Table 216 */
		if (memclk_index <= 0x6) {
			delay = 0x5;
			delay2 = 0x3;
		} else if (memclk_index == 0xa) {
			delay = 0x6;
			delay2 = 0x3;
		} else if (memclk_index == 0xe) {
			delay = 0x7;
			delay2 = 0x4;
		} else if (memclk_index == 0x12) {
			delay = 0x8;
			delay2 = 0x4;
		} else if (memclk_index == 0x16) {
			delay = 0xa;
			delay2 = 0x5;
		}

		dword = Get_NB32_DCT(dev, dct, 0x24c);			/* Dram Power Management 1 */
		dword &= ~(0x3f << 24);					/* Tcksrx = delay */
		dword |= ((delay & 0x3f) << 24);
		dword &= ~(0x3f << 16);					/* Tcksre = delay */
		dword |= ((delay & 0x3f) << 16);
		dword &= ~(0x3f << 8);					/* Tckesr = delay2 + 1 */
		dword |= (((delay2 + 1) & 0x3f) << 8);
		dword &= ~0xf;						/* Tpd = delay2 */
		dword |= delay2 & 0xf;
		Set_NB32_DCT(dev, dct, 0x24c, dword);			/* Dram Power Management 1 */

		dword = Get_NB32_DCT(dev, dct, 0x94);			/* DRAM Configuration High */
		dword |= (0xf << 24);					/* DcqBypassMax = 0xf */
		dword |= (0x1 << 22);					/* BankSwizzleMode = 1 */
		dword |= (0x1 << 15);					/* PowerDownEn = 1 */
		dword &= ~(0x3 << 10);					/* ZqcsInterval = 0x2 */
		dword |= (0x2 << 10);
		Set_NB32_DCT(dev, dct, 0x94, dword);			/* DRAM Configuration High */

		if (x4_present && x8_present) {
			/* Mixed channel of 4x and 8x DIMMs */
			dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0000000d);
			dword &= ~(0x3 << 24);					/* RxDLLWakeupTime = 0 */
			dword &= ~(0x7 << 20);					/* RxCPUpdPeriod = 0 */
			dword &= ~(0xf << 16);					/* RxMaxDurDllNoLock = 0 */
			dword &= ~(0x3 << 8);					/* TxDLLWakeupTime = 0 */
			dword &= ~(0x7 << 4);					/* TxCPUpdPeriod = 0 */
			dword &= ~(0xf);					/* TxMaxDurDllNoLock = 0 */
			Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0000000d, dword);
		} else {
			dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0000000d);
			dword &= ~(0x3 << 24);					/* RxDLLWakeupTime = 3 */
			dword |= (0x3 << 24);
			dword &= ~(0x7 << 20);					/* RxCPUpdPeriod = 3 */
			dword |= (0x3 << 20);
			dword &= ~(0xf << 16);					/* RxMaxDurDllNoLock = 7 */
			dword |= (0x7 << 16);
			dword &= ~(0x3 << 8);					/* TxDLLWakeupTime = 3 */
			dword |= (0x3 << 8);
			dword &= ~(0x7 << 4);					/* TxCPUpdPeriod = 3 */
			dword |= (0x3 << 4);
			dword &= ~(0xf);					/* TxMaxDurDllNoLock = 7 */
			dword |= 0x7;
			Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0000000d, dword);
		}

		if ((memclk_index <= 0x12) && (x4_present != x8_present)) {
			/* MemClkFreq <= 800MHz
			 * Not a mixed channel of x4 and x8 DIMMs
			 */
			for (index = 0; index < 0x9; index++) {
				dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f0010 | (index << 8));
				dword |= (0x1 << 12);				/* EnRxPadStandby = 1 */
				Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f0010 | (index << 8), dword);
			}
		} else {
			for (index = 0; index < 0x9; index++) {
				dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f0010 | (index << 8));
				dword &= ~(0x1 << 12);				/* EnRxPadStandby = 0 */
				Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f0010 | (index << 8), dword);
			}
		}

		/* Calculate the Critical Delay Difference for Twrrd */
		cdd_twrrd = 0;
		for (receiver = 0; receiver < 8; receiver += 2) {
			dimm = (receiver >> 1);

			if (!mct_RcvrRankEnabled_D(pMCTstat, pDCTstat, dct, receiver))
				continue;

			read_dqs_write_timing_control_registers(current_total_delay_1, dev, dct, dimm, index_reg);
			read_dqs_receiver_enable_control_registers(current_total_delay_2, dev, dct, dimm, index_reg);

			for (lane = 0; lane < lane_count; lane++) {
				if (current_total_delay_1[lane] > current_total_delay_2[lane])
					difference = current_total_delay_1[lane] - current_total_delay_2[lane];
				else
					difference = current_total_delay_2[lane] - current_total_delay_1[lane];

				if (difference > cdd_twrrd)
					cdd_twrrd = difference;
			}
		}

		/* Convert the difference to MEMCLKs */
		cdd_twrrd = (((cdd_twrrd + (1 << 6) - 1) >> 6) & 0xf);

		/* Fam15h BKDG section 2.10.5.5.3 */
		if (pDCTstat->Status & (1 << SB_LoadReduced)) {
			/* LRDIMM */

			/* TODO
			 * Implement LRDIMM support
			 * See Fam15h BKDG Rev. 3.14 section 2.10.5.5
			 */
			twrrd = 0xb;
		} else {
			max_cdd_we_delta = (((int16_t)cdd_twrrd + 1 - ((int16_t)write_early * 2)) + 1) / 2;
			if (max_cdd_we_delta < 0)
				max_cdd_we_delta = 0;
			if (((uint16_t)max_cdd_we_delta) > write_odt_delay)
				dword = max_cdd_we_delta;
			else
				dword = write_odt_delay;
			dword += 3;
			if (latency_difference < dword) {
				dword -= latency_difference;
				if (dword < 1)
					twrrd = 1;
				else
					twrrd = dword;
			} else {
				twrrd = 1;
			}
		}

		/* Calculate the Critical Delay Difference for TrwtTO */
		cdd_trwtto = 0;
		for (receiver = 0; receiver < 8; receiver += 2) {
			dimm = (receiver >> 1);

			if (!mct_RcvrRankEnabled_D(pMCTstat, pDCTstat, dct, receiver))
				continue;

			read_dqs_receiver_enable_control_registers(current_total_delay_1, dev, dct, dimm, index_reg);
			read_dqs_write_timing_control_registers(current_total_delay_2, dev, dct, dimm, index_reg);

			for (lane = 0; lane < lane_count; lane++) {
				if (current_total_delay_1[lane] > current_total_delay_2[lane])
					difference = current_total_delay_1[lane] - current_total_delay_2[lane];
				else
					difference = current_total_delay_2[lane] - current_total_delay_1[lane];

				if (difference > cdd_trwtto)
					cdd_trwtto = difference;
			}
		}

		/* Convert the difference to MEMCLKs */
		cdd_trwtto = (((cdd_trwtto + (1 << 6) - 1) >> 6) & 0xf);

		/* Fam15h BKDG section 2.10.5.5.4 */
		if (max_dimms_installable == 1)
			min_value = 0;
		else
			min_value = read_odt_delay + buffer_data_delay;
		cdd_trwtto_we_delta = (((int16_t)cdd_trwtto - 1 + ((int16_t)write_early * 2)) + 1) / 2;
		cdd_trwtto_we_delta += latency_difference + 3;
		if (cdd_trwtto_we_delta < 0)
			cdd_trwtto_we_delta = 0;
		if ((cdd_trwtto_we_delta) > min_value)
			trwtto = cdd_trwtto_we_delta;
		else
			trwtto = min_value;

		dword = Get_NB32_DCT(dev, dct, 0xa4);			/* DRAM Controller Temperature Throttle */
		dword &= ~(0x1 << 11);					/* BwCapEn = 0 */
		dword &= ~(0x1 << 8);					/* ODTSEn = dimm_event_l_pin_support */
		dword |= (dimm_event_l_pin_support & 0x1) << 8;
		Set_NB32_DCT(dev, dct, 0xa4, dword);			/* DRAM Controller Temperature Throttle */

		dword = Get_NB32_DCT(dev, dct, 0x110);			/* DRAM Controller Select Low */
		dword &= ~(0x1 << 2);					/* DctSelIntLvEn = interleave_channels */
		dword |= (interleave_channels & 0x1) << 2;
		dword |= (0x3 << 6);					/* DctSelIntLvAddr = 0x3 */
		Set_NB32_DCT(dev, dct, 0x110, dword);			/* DRAM Controller Select Low */

		/* NOTE
		 * ECC-related setup is performed as part of ECCInit_D and must not be located here,
		 * otherwise semi-random lockups will occur due to misconfigured scrubbing hardware!
		 */

		/* Fam15h BKDG section 2.10.5.5.2 */
		dword = Get_NB32_DCT(dev, dct, 0x214);			/* DRAM Timing 4 */
		dword &= ~(0xf << 16);					/* TwrwrSdSc = 0x1 */
		dword |= (0x1 << 16);
		dword &= ~(0xf << 8);					/* TwrwrSdDc = twrwrsddc */
		dword |= ((twrwrsddc & 0xf) << 8);
		dword &= ~(0xf);					/* TwrwrDd = twrwrdd */
		dword |= (twrwrdd & 0xf);
		Set_NB32_DCT(dev, dct, 0x214, dword);			/* DRAM Timing 4 */

		/* Fam15h BKDG section 2.10.5.5.3 */
		dword = Get_NB32_DCT(dev, dct, 0x218);			/* DRAM Timing 5 */
		dword &= ~(0xf << 24);					/* TrdrdSdSc = 0x1 */
		dword |= (0x1 << 24);
		dword &= ~(0xf << 16);					/* TrdrdSdDc = trdrdsddc */
		dword |= ((trdrdsddc & 0xf) << 16);
		dword &= ~(0xf << 8);					/* Twrrd = twrrd */
		dword |= ((twrrd & 0xf) << 8);
		dword &= ~(0xf);					/* TrdrdDd = trdrddd */
		dword |= (trdrddd & 0xf);
		Set_NB32_DCT(dev, dct, 0x218, dword);			/* DRAM Timing 5 */

		/* Fam15h BKDG section 2.10.5.5.4 */
		dword = Get_NB32_DCT(dev, dct, 0x21c);			/* DRAM Timing 6 */
		dword &= ~(0x1f << 8);					/* TrwtTO = trwtto */
		dword |= ((trwtto & 0x1f) << 8);
		dword &= ~(0x1f << 16);					/* TrwtWB = TrwtTO + 1 */
		dword |= ((((dword >> 8) & 0x1f) + 1) << 16);
		Set_NB32_DCT(dev, dct, 0x21c, dword);			/* DRAM Timing 6 */

		/* Enable prefetchers */
		dword = Get_NB32(dev, 0x11c);				/* Memory Controller Configuration High */
		dword &= ~(0x1 << 13);					/* PrefIoDis = 0 */
		dword &= ~(0x1 << 12);					/* PrefCpuDis = 0 */
		Set_NB32(dev, 0x11c, dword);				/* Memory Controller Configuration High */
	}
}

static void exit_training_mode_fam15(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstatA)
{
	uint8_t node;
	uint8_t dct;

	for (node = 0; node < MAX_NODES_SUPPORTED; node++) {
		struct DCTStatStruc *pDCTstat;
		pDCTstat = pDCTstatA + node;

		if (pDCTstat->NodePresent)
			for (dct = 0; dct < 2; dct++)
				fam15EnableTrainingMode(pMCTstat, pDCTstat, dct, 0);
	}
}

static void DQSTiming_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstatA, uint8_t allow_config_restore)
{
	uint8_t Node;
	u8 nv_DQSTrainCTL;
	uint8_t retry_requested;

	if (pMCTstat->GStatus & (1 << GSB_EnDIMMSpareNW)) {
		return;
	}

	/* Set initial TCWL offset to zero */
	for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
		uint8_t dct;
		struct DCTStatStruc *pDCTstat;
		pDCTstat = pDCTstatA + Node;
		for (dct = 0; dct < 2; dct++)
			pDCTstat->tcwl_delay[dct] = 0;
	}

retry_dqs_training_and_levelization:
	nv_DQSTrainCTL = !allow_config_restore;

	mct_BeforeDQSTrain_D(pMCTstat, pDCTstatA);
	phyAssistedMemFnceTraining(pMCTstat, pDCTstatA, -1);

	if (is_fam15h()) {
		struct DCTStatStruc *pDCTstat;
		for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
			pDCTstat = pDCTstatA + Node;
			if (pDCTstat->NodePresent) {
				if (pDCTstat->DIMMValidDCT[0])
					InitPhyCompensation(pMCTstat, pDCTstat, 0);
				if (pDCTstat->DIMMValidDCT[1])
					InitPhyCompensation(pMCTstat, pDCTstat, 1);
			}
		}
	}

	mctHookBeforeAnyTraining(pMCTstat, pDCTstatA);
	if (!is_fam15h()) {
		/* TODO: should be in mctHookBeforeAnyTraining */
		_WRMSR(0x26C, 0x04040404, 0x04040404);
		_WRMSR(0x26D, 0x04040404, 0x04040404);
		_WRMSR(0x26E, 0x04040404, 0x04040404);
		_WRMSR(0x26F, 0x04040404, 0x04040404);
	}

	if (nv_DQSTrainCTL) {
		mct_WriteLevelization_HW(pMCTstat, pDCTstatA, FirstPass);

		if (is_fam15h()) {
			/* Receiver Enable Training Pass 1 */
			TrainReceiverEn_D(pMCTstat, pDCTstatA, FirstPass);
		}

		mct_WriteLevelization_HW(pMCTstat, pDCTstatA, SecondPass);

		if (is_fam15h()) {

			/* TODO:
			 * Determine why running TrainReceiverEn_D in SecondPass
			 * mode yields less stable training values than when run
			 * in FirstPass mode as in the HACK below.
			 */
			TrainReceiverEn_D(pMCTstat, pDCTstatA, FirstPass);
		} else {
			TrainReceiverEn_D(pMCTstat, pDCTstatA, FirstPass);
		}

		mct_TrainDQSPos_D(pMCTstat, pDCTstatA);

		/* Determine if DQS training requested a retrain attempt */
		retry_requested = 0;
		for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
			struct DCTStatStruc *pDCTstat;
			pDCTstat = pDCTstatA + Node;

			if (pDCTstat->NodePresent) {
				if (pDCTstat->TrainErrors & (1 << SB_FatalError)) {
					printk(BIOS_ERR, "DIMM training FAILED!  Restarting system...");
					soft_reset();
				}
				if (pDCTstat->TrainErrors & (1 << SB_RetryConfigTrain)) {
					retry_requested = 1;

					/* Clear previous errors */
					pDCTstat->TrainErrors &= ~(1 << SB_RetryConfigTrain);
					pDCTstat->TrainErrors &= ~(1 << SB_NODQSPOS);
					pDCTstat->ErrStatus &= ~(1 << SB_RetryConfigTrain);
					pDCTstat->ErrStatus &= ~(1 << SB_NODQSPOS);
				}
			}
		}

		/* Retry training and levelization if requested */
		if (retry_requested) {
			printk(BIOS_DEBUG, "%s: Restarting training on algorithm request\n", __func__);
			/* Reset frequency to minimum */
			for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
				struct DCTStatStruc *pDCTstat;
				pDCTstat = pDCTstatA + Node;
				if (pDCTstat->NodePresent) {
					uint8_t original_target_freq = pDCTstat->TargetFreq;
					uint8_t original_auto_speed = pDCTstat->DIMMAutoSpeed;
					pDCTstat->TargetFreq = mhz_to_memclk_config(mctGet_NVbits(NV_MIN_MEMCLK));
					pDCTstat->Speed = pDCTstat->DIMMAutoSpeed = pDCTstat->TargetFreq;
					SetTargetFreq(pMCTstat, pDCTstatA, Node);
					pDCTstat->TargetFreq = original_target_freq;
					pDCTstat->DIMMAutoSpeed = original_auto_speed;
				}
			}
			/* Apply any DIMM timing changes */
			for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
				struct DCTStatStruc *pDCTstat;
				pDCTstat = pDCTstatA + Node;
				if (pDCTstat->NodePresent) {
					AutoCycTiming_D(pMCTstat, pDCTstat, 0);
					if (!pDCTstat->GangedMode)
						if (pDCTstat->DIMMValidDCT[1] > 0)
							AutoCycTiming_D(pMCTstat, pDCTstat, 1);
				}
			}
			goto retry_dqs_training_and_levelization;
		}

		TrainMaxRdLatency_En_D(pMCTstat, pDCTstatA);

		if (is_fam15h())
			exit_training_mode_fam15(pMCTstat, pDCTstatA);
		else
			mctSetEccDQSRcvrEn_D(pMCTstat, pDCTstatA);
	} else {
		mct_WriteLevelization_HW(pMCTstat, pDCTstatA, FirstPass);

		mct_WriteLevelization_HW(pMCTstat, pDCTstatA, SecondPass);

#if IS_ENABLED(CONFIG_HAVE_ACPI_RESUME)
		printk(BIOS_DEBUG, "mctAutoInitMCT_D: Restoring DIMM training configuration from NVRAM\n");
		if (restore_mct_information_from_nvram(1) != 0)
			printk(BIOS_CRIT, "%s: ERROR: Unable to restore DCT configuration from NVRAM\n", __func__);
#endif

		if (is_fam15h())
			exit_training_mode_fam15(pMCTstat, pDCTstatA);

		pMCTstat->GStatus |= 1 << GSB_ConfigRestored;
	}

	if (is_fam15h()) {
		struct DCTStatStruc *pDCTstat;

		/* Switch DCT control register to DCT 0 per Erratum 505 */
		for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
			pDCTstat = pDCTstatA + Node;
			if (pDCTstat->NodePresent) {
				fam15h_switch_dct(pDCTstat->dev_map, 0);
			}
		}
	}

	/* FIXME - currently uses calculated value	TrainMaxReadLatency_D(pMCTstat, pDCTstatA); */
	mctHookAfterAnyTraining();
}

static void LoadDQSSigTmgRegs_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstatA)
{
	u8 Node, Receiver, Channel, Dir, DIMM;
	u32 dev;
	u32 index_reg;
	u32 reg;
	u32 index;
	u32 val;
	u8 ByteLane;
	u8 txdqs;

	for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
		struct DCTStatStruc *pDCTstat;
		pDCTstat = pDCTstatA + Node;

		if (pDCTstat->DCTSysLimit) {
			dev = pDCTstat->dev_dct;
			for (Channel = 0;Channel < 2; Channel++) {
				/* there are four receiver pairs,
				   loosely associated with chipselects.*/
				index_reg = 0x98;
				for (Receiver = 0; Receiver < 8; Receiver += 2) {
					/* Set Receiver Enable Values */
					mct_SetRcvrEnDly_D(pDCTstat,
						0, /* RcvrEnDly */
						1, /* FinalValue, From stack */
						Channel,
						Receiver,
						dev, index_reg,
						(Receiver >> 1) * 3 + 0x10, /* Addl_Index */
						2); /* Pass Second Pass ? */
					/* Restore Write levelization training data */
					for (ByteLane = 0; ByteLane < 9; ByteLane ++) {
						txdqs = pDCTstat->persistentData.CH_D_B_TxDqs[Channel][Receiver >> 1][ByteLane];
						index = Table_DQSRcvEn_Offset[ByteLane >> 1];
						index += (Receiver >> 1) * 3 + 0x10 + 0x20; /* Addl_Index */
						val = Get_NB32_index_wait_DCT(dev, Channel, 0x98, index);
						if (ByteLane & 1) { /* odd byte lane */
							val &= ~(0xFF << 16);
							val |= txdqs << 16;
						} else {
							val &= ~0xFF;
							val |= txdqs;
						}
						Set_NB32_index_wait_DCT(dev, Channel, 0x98, index, val);
					}
				}
			}
			for (Channel = 0; Channel < 2; Channel++) {
				SetEccDQSRcvrEn_D(pDCTstat, Channel);
			}

			for (Channel = 0; Channel < 2; Channel++) {
				u8 *p;
				index_reg = 0x98;

				/* NOTE:
				 * when 400, 533, 667, it will support dimm0/1/2/3,
				 * and set conf for dimm0, hw will copy to dimm1/2/3
				 * set for dimm1, hw will copy to dimm3
				 * Rev A/B only support DIMM0/1 when 800MHz and above
				 *   + 0x100 to next dimm
				 * Rev C support DIMM0/1/2/3 when 800MHz and above
				 *   + 0x100 to next dimm
				*/
				for (DIMM = 0; DIMM < 4; DIMM++) {
					if (DIMM == 0) {
						index = 0;	/* CHA Write Data Timing Low */
					} else {
						if (pDCTstat->Speed >= mhz_to_memclk_config(mctGet_NVbits(NV_MIN_MEMCLK))) {
							index = 0x100 * DIMM;
						} else {
							break;
						}
					}
					for (Dir = 0; Dir < 2; Dir++) {/* RD/WR */
						p = pDCTstat->CH_D_DIR_B_DQS[Channel][DIMM][Dir];
						val = stream_to_int(p); /* CHA Read Data Timing High */
						Set_NB32_index_wait_DCT(dev, Channel, index_reg, index+1, val);
						val = stream_to_int(p+4); /* CHA Write Data Timing High */
						Set_NB32_index_wait_DCT(dev, Channel, index_reg, index+2, val);
						val = *(p+8); /* CHA Write ECC Timing */
						Set_NB32_index_wait_DCT(dev, Channel, index_reg, index+3, val);
						index += 4;
					}
				}
			}

			for (Channel = 0; Channel < 2; Channel++) {
				reg = 0x78;
				val = Get_NB32_DCT(dev, Channel, reg);
				val &= ~(0x3ff<<22);
				val |= ((u32) pDCTstat->CH_MaxRdLat[Channel][0] << 22);
				val &= ~(1<<DqsRcvEnTrain);
				Set_NB32_DCT(dev, Channel, reg, val);	/* program MaxRdLatency to correspond with current delay*/
			}
		}
	}
}

static void HTMemMapInit_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstatA)
{
	u8 Node;
	u32 NextBase, BottomIO;
	u8 _MemHoleRemap, DramHoleBase, DramHoleOffset;
	u32 HoleSize, DramSelBaseAddr;

	u32 val;
	u32 base;
	u32 limit;
	u32 dev, devx;
	struct DCTStatStruc *pDCTstat;

	_MemHoleRemap = mctGet_NVbits(NV_MemHole);

	if (pMCTstat->HoleBase == 0) {
		DramHoleBase = mctGet_NVbits(NV_BottomIO);
	} else {
		DramHoleBase = pMCTstat->HoleBase >> (24-8);
	}

	BottomIO = DramHoleBase << (24-8);

	NextBase = 0;
	pDCTstat = pDCTstatA + 0;
	dev = pDCTstat->dev_map;

	for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
		pDCTstat = pDCTstatA + Node;
		devx = pDCTstat->dev_map;
		DramSelBaseAddr = 0;
		if (!pDCTstat->GangedMode) {
			DramSelBaseAddr = pDCTstat->NodeSysLimit - pDCTstat->DCTSysLimit;
			/*In unganged mode, we must add DCT0 and DCT1 to DCTSysLimit */
			val = pDCTstat->NodeSysLimit;
			if ((val & 0xFF) == 0xFE) {
				DramSelBaseAddr++;
				val++;
			}
			pDCTstat->DCTSysLimit = val;
		}

		base  = pDCTstat->DCTSysBase;
		limit = pDCTstat->DCTSysLimit;
		if (limit > base) {
			base  += NextBase;
			limit += NextBase;
			DramSelBaseAddr += NextBase;
			printk(BIOS_DEBUG, " Node: %02x  base: %02x  limit: %02x  BottomIO: %02x\n", Node, base, limit, BottomIO);

			if (_MemHoleRemap) {
				if ((base < BottomIO) && (limit >= BottomIO)) {
					/* HW Dram Remap */
					pDCTstat->Status |= 1 << SB_HWHole;
					pMCTstat->GStatus |= 1 << GSB_HWHole;
					pDCTstat->DCTSysBase = base;
					pDCTstat->DCTSysLimit = limit;
					pDCTstat->DCTHoleBase = BottomIO;
					pMCTstat->HoleBase = BottomIO;
					HoleSize = _4GB_RJ8 - BottomIO; /* HoleSize[39:8] */
					if ((DramSelBaseAddr > 0) && (DramSelBaseAddr < BottomIO))
						base = DramSelBaseAddr;
					val = ((base + HoleSize) >> (24-8)) & 0xFF;
					DramHoleOffset = val;
					val <<= 8; /* shl 16, rol 24 */
					val |= DramHoleBase << 24;
					val |= 1  << DramHoleValid;
					Set_NB32(devx, 0xF0, val); /* Dram Hole Address Reg */
					pDCTstat->DCTSysLimit += HoleSize;
					base = pDCTstat->DCTSysBase;
					limit = pDCTstat->DCTSysLimit;
				} else if (base == BottomIO) {
					/* SW Node Hoist */
					pMCTstat->GStatus |= 1<<GSB_SpIntRemapHole;
					pDCTstat->Status |= 1<<SB_SWNodeHole;
					pMCTstat->GStatus |= 1<<GSB_SoftHole;
					pMCTstat->HoleBase = base;
					limit -= base;
					base = _4GB_RJ8;
					limit += base;
					pDCTstat->DCTSysBase = base;
					pDCTstat->DCTSysLimit = limit;
				} else {
					/* No Remapping.  Normal Contiguous mapping */
					pDCTstat->DCTSysBase = base;
					pDCTstat->DCTSysLimit = limit;
				}
			} else {
				/*No Remapping.  Normal Contiguous mapping*/
				pDCTstat->DCTSysBase = base;
				pDCTstat->DCTSysLimit = limit;
			}
			base |= 3;		/* set WE,RE fields*/
			pMCTstat->SysLimit = limit;
		}
		Set_NB32(dev, 0x40 + (Node << 3), base); /* [Node] + Dram Base 0 */

		val = limit & 0xFFFF0000;
		val |= Node;
		Set_NB32(dev, 0x44 + (Node << 3), val);	/* set DstNode */

		printk(BIOS_DEBUG, " Node: %02x  base: %02x  limit: %02x\n", Node, base, limit);
		limit = pDCTstat->DCTSysLimit;
		if (limit) {
			NextBase = (limit & 0xFFFF0000) + 0x10000;
		}
	}

	/* Copy dram map from Node 0 to Node 1-7 */
	for (Node = 1; Node < MAX_NODES_SUPPORTED; Node++) {
		u32 reg;
		pDCTstat = pDCTstatA + Node;
		devx = pDCTstat->dev_map;

		if (pDCTstat->NodePresent) {
			printk(BIOS_DEBUG, " Copy dram map from Node 0 to Node %02x\n", Node);
			reg = 0x40;		/*Dram Base 0*/
			do {
				val = Get_NB32(dev, reg);
				Set_NB32(devx, reg, val);
				reg += 4;
			} while (reg < 0x80);
		} else {
			break;			/* stop at first absent Node */
		}
	}

	/*Copy dram map to F1x120/124*/
	mct_HTMemMapExt(pMCTstat, pDCTstatA);
}

void MCTMemClr_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstatA)
{

	/* Initiates a memory clear operation for all node. The mem clr
	 * is done in parallel. After the memclr is complete, all processors
	 * status are checked to ensure that memclr has completed.
	 */
	u8 Node;
	uint32_t dword;
	struct DCTStatStruc *pDCTstat;

	if (!mctGet_NVbits(NV_DQSTrainCTL)) {
		/* FIXME: callback to wrapper: mctDoWarmResetMemClr_D */
	} else {	/* NV_DQSTrainCTL == 1 */
		for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
			pDCTstat = pDCTstatA + Node;

			if (pDCTstat->NodePresent) {
				DCTMemClr_Init_D(pMCTstat, pDCTstat);
			}
		}
		for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
			pDCTstat = pDCTstatA + Node;

			if (pDCTstat->NodePresent) {
				DCTMemClr_Sync_D(pMCTstat, pDCTstat);
			}
		}
	}

	for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
		pDCTstat = pDCTstatA + Node;

		/* Enable prefetchers */
		dword = Get_NB32(pDCTstat->dev_dct, 0x11c);	/* Memory Controller Configuration High */
		dword &= ~(0x1 << 13);				/* PrefIoDis = 0 */
		dword &= ~(0x1 << 12);				/* PrefCpuDis = 0 */
		Set_NB32(pDCTstat->dev_dct, 0x11c, dword);	/* Memory Controller Configuration High */
	}
}

void DCTMemClr_Init_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat)
{
	u32 val;
	u32 dev;
	uint32_t dword;

	/* Initiates a memory clear operation on one node */
	if (pDCTstat->DCTSysLimit) {
		dev = pDCTstat->dev_dct;

		/* Disable prefetchers */
		dword = Get_NB32(dev, 0x11c);		/* Memory Controller Configuration High */
		dword |= 0x1 << 13;			/* PrefIoDis = 1 */
		dword |= 0x1 << 12;			/* PrefCpuDis = 1 */
		Set_NB32(dev, 0x11c, dword);		/* Memory Controller Configuration High */

		do {
			val = Get_NB32(dev, 0x110);
		} while (val & (1 << MemClrBusy));

		val |= (1 << MemClrInit);
		Set_NB32(dev, 0x110, val);
	}
}

void DCTMemClr_Sync_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat)
{
	uint32_t dword;
	uint32_t dev = pDCTstat->dev_dct;

	printk(BIOS_DEBUG, "%s: Start\n", __func__);

	/* Ensure that a memory clear operation has completed on one node */
	if (pDCTstat->DCTSysLimit) {
		printk(BIOS_DEBUG, "%s: Waiting for memory clear to complete", __func__);
		do {
			dword = Get_NB32(dev, 0x110);

			printk(BIOS_DEBUG, ".");
		} while (dword & (1 << MemClrBusy));

		printk(BIOS_DEBUG, "\n");
		do {
			printk(BIOS_DEBUG, ".");
			dword = Get_NB32(dev, 0x110);
		} while (!(dword & (1 << Dr_MemClrStatus)));
		printk(BIOS_DEBUG, "\n");
	}

	/* Enable prefetchers */
	dword = Get_NB32(dev, 0x11c);		/* Memory Controller Configuration High */
	dword &= ~(0x1 << 13);			/* PrefIoDis = 0 */
	dword &= ~(0x1 << 12);			/* PrefCpuDis = 0 */
	Set_NB32(dev, 0x11c, dword);		/* Memory Controller Configuration High */

	printk(BIOS_DEBUG, "%s: Done\n", __func__);
}

u8 NodePresent_D(u8 Node)
{
	/*
	 * Determine if a single Hammer Node exists within the network.
	 */
	u32 dev;
	u32 val;
	u32 dword;
	u8 ret = 0;

	dev = PA_HOST(Node);		/*test device/vendor id at host bridge  */
	val = Get_NB32(dev, 0);
	dword = mct_NodePresent_D();	/* FIXME: BOZO -11001022h rev for F */
	if (val == dword) {		/* AMD Hammer Family CPU HT Configuration */
		if (oemNodePresent_D(Node, &ret))
			goto finish;
		/* Node ID register */
		val = Get_NB32(dev, 0x60);
		val &= 0x07;
		dword = Node;
		if (val  == dword)	/* current nodeID = requested nodeID ? */
			ret = 1;
	}
finish:
	return ret;
}

static void DCTPreInit_D(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat, u8 dct)
{
	/*
	 * Run DCT pre-initialization tasks
	 */
	uint32_t dword;

	/* Reset DCT registers */
	ClearDCT_D(pMCTstat, pDCTstat, dct);
	pDCTstat->stopDCT[dct] = 1;	/* preload flag with 'disable' */

	if (!is_fam15h()) {
		/* Enable DDR3 support */
		dword = Get_NB32_DCT(pDCTstat->dev_dct, dct, 0x94);
		dword |= 1 << Ddr3Mode;
		Set_NB32_DCT(pDCTstat->dev_dct, dct, 0x94, dword);
	}

	/* Read the SPD information into the data structures */
	if (mct_DIMMPresence(pMCTstat, pDCTstat, dct) < SC_StopError) {
		printk(BIOS_DEBUG, "\t\tDCTPreInit_D: mct_DIMMPresence Done\n");
	}
}

static void DCTInit_D(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat, u8 dct)
{
	/*
	 * Initialize DRAM on single Athlon 64/Opteron Node.
	 */
	uint32_t dword;

	if (!is_fam15h()) {
		/* (Re)-enable DDR3 support */
		dword = Get_NB32_DCT(pDCTstat->dev_dct, dct, 0x94);
		dword |= 1 << Ddr3Mode;
		Set_NB32_DCT(pDCTstat->dev_dct, dct, 0x94, dword);
	}

	if (mct_SPDCalcWidth(pMCTstat, pDCTstat, dct) < SC_StopError) {
		printk(BIOS_DEBUG, "\t\tDCTInit_D: mct_SPDCalcWidth Done\n");
		if (AutoCycTiming_D(pMCTstat, pDCTstat, dct) < SC_StopError) {
			printk(BIOS_DEBUG, "\t\tDCTInit_D: AutoCycTiming_D Done\n");

			/* SkewMemClk must be set before MemClkFreqVal is set
			 * This relies on DCTInit_D being called for DCT 1 after
			 * it has already been called for DCT 0...
			 */
			if (is_fam15h()) {
				/* Set memory clock skew if needed */
				if (dct == 1) {
					if (!pDCTstat->stopDCT[0]) {
						printk(BIOS_DEBUG, "\t\tDCTInit_D: enabling intra-channel clock skew\n");
						dword = Get_NB32_index_wait_DCT(pDCTstat->dev_dct, 0, 0x98, 0x0d0fe00a);
						dword |= (0x1 << 4);				/* SkewMemClk = 1 */
						Set_NB32_index_wait_DCT(pDCTstat->dev_dct, 0, 0x98, 0x0d0fe00a, dword);
					}
				}
			}

			if (AutoConfig_D(pMCTstat, pDCTstat, dct) < SC_StopError) {
				printk(BIOS_DEBUG, "\t\tDCTInit_D: AutoConfig_D Done\n");
				if (PlatformSpec_D(pMCTstat, pDCTstat, dct) < SC_StopError) {
					printk(BIOS_DEBUG, "\t\tDCTInit_D: PlatformSpec_D Done\n");
					pDCTstat->stopDCT[dct] = 0;
				}
			}
		}
	}
}

static void DCTFinalInit_D(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat, u8 dct)
{
	uint32_t dword;

	/* Finalize DRAM init on a single node */
	if (!pDCTstat->stopDCT[dct]) {
		if (!(pMCTstat->GStatus & (1 << GSB_EnDIMMSpareNW))) {
			printk(BIOS_DEBUG, "\t\tDCTFinalInit_D: StartupDCT_D Start\n");
			StartupDCT_D(pMCTstat, pDCTstat, dct);
			printk(BIOS_DEBUG, "\t\tDCTFinalInit_D: StartupDCT_D Done\n");
		}
	}

	if (pDCTstat->stopDCT[dct]) {
		dword = 1 << DisDramInterface;
		Set_NB32_DCT(pDCTstat->dev_dct, dct, 0x94, dword);

		dword = Get_NB32_DCT(pDCTstat->dev_dct, dct, 0x90);
		dword &= ~(1 << ParEn);
		Set_NB32_DCT(pDCTstat->dev_dct, dct, 0x90, dword);

		/* To maximize power savings when DisDramInterface = 1b,
		 * all of the MemClkDis bits should also be set.
		 */
		Set_NB32_DCT(pDCTstat->dev_dct, dct, 0x88, 0xff000000);
	} else {
		mct_EnDllShutdownSR(pMCTstat, pDCTstat, dct);
	}
}

static void SyncDCTsReady_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstatA)
{
	/* Wait (and block further access to dram) for all DCTs to be ready,
	 * by polling all InitDram bits and waiting for possible memory clear
	 * operations to be complete.  Read MemClkFreqVal bit to see if
	 * the DIMMs are present in this node.
	 */
	u8 Node;
	u32 val;

	for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
		struct DCTStatStruc *pDCTstat;
		pDCTstat = pDCTstatA + Node;
		mct_SyncDCTsReady(pDCTstat);
	}

	if (!is_fam15h()) {
		/* v6.1.3 */
		/* re-enable phy compensation engine when dram init is completed on all nodes. */
		for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
			struct DCTStatStruc *pDCTstat;
			pDCTstat = pDCTstatA + Node;
			if (pDCTstat->NodePresent) {
				if (pDCTstat->DIMMValidDCT[0] > 0 || pDCTstat->DIMMValidDCT[1] > 0) {
					/* re-enable phy compensation engine when dram init on both DCTs is completed. */
					val = Get_NB32_index_wait(pDCTstat->dev_dct, 0x98, 0x8);
					val &= ~(1 << DisAutoComp);
					Set_NB32_index_wait(pDCTstat->dev_dct, 0x98, 0x8, val);
				}
			}
		}
	}

	/* wait 750us before any memory access can be made. */
	mct_Wait(15000);
}

void StartupDCT_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct)
{
	/* Read MemClkFreqVal bit to see if the DIMMs are present in this node.
	 * If the DIMMs are present then set the DRAM Enable bit for this node.
	 *
	 * Setting dram init starts up the DCT state machine, initializes the
	 * dram devices with MRS commands, and kicks off any
	 * HW memory clear process that the chip is capable of.	The sooner
	 * that dram init is set for all nodes, the faster the memory system
	 * initialization can complete.	Thus, the init loop is unrolled into
	 * two loops so as to start the processes for non BSP nodes sooner.
	 * This procedure will not wait for the process to finish.
	 * Synchronization is handled elsewhere.
	 */
	u32 val;
	u32 dev;

	dev = pDCTstat->dev_dct;
	val = Get_NB32_DCT(dev, dct, 0x94);
	if (val & (1<<MemClkFreqVal)) {
		mctHookBeforeDramInit();	/* generalized Hook */
		if (!(pMCTstat->GStatus & (1 << GSB_EnDIMMSpareNW)))
		    mct_DramInit(pMCTstat, pDCTstat, dct);
		AfterDramInit_D(pDCTstat, dct);
		mctHookAfterDramInit();		/* generalized Hook*/
	}
}

static void ClearDCT_D(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, u8 dct)
{
	u32 reg_end;
	u32 dev = pDCTstat->dev_dct;
	u32 reg = 0x40;
	u32 val = 0;

	if (pMCTstat->GStatus & (1 << GSB_EnDIMMSpareNW)) {
		reg_end = 0x78;
	} else {
		reg_end = 0xA4;
	}

	while (reg < reg_end) {
		if ((reg & 0xFF) == 0x84) {
			if (is_fam15h()) {
				val = Get_NB32_DCT(dev, dct, reg);
				val &= ~(0x1 << 23);	/* Clear PchgPDModeSel */
				val &= ~0x3;		/* Clear BurstCtrl */
			}
		}
		if ((reg & 0xFF) == 0x90) {
			if (pDCTstat->LogicalCPUID & AMD_DR_Dx) {
				val = Get_NB32_DCT(dev, dct, reg); /* get DRAMConfigLow */
				val |= 0x08000000; /* preserve value of DisDllShutdownSR for only Rev.D */
			}
		}
		Set_NB32_DCT(dev, dct, reg, val);
		val = 0;
		reg += 4;
	}

	val = 0;
	dev = pDCTstat->dev_map;
	reg = 0xF0;
	Set_NB32(dev, reg, val);
}

void SPD2ndTiming(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, u8 dct)
{
	u8 i;
	u16 Twr, Trtp;
	u16 Trp, Trrd, Trcd, Tras, Trc;
	u8 Trfc[4];
	u16 Tfaw;
	u16 Tcwl;	/* Fam15h only */
	u32 DramTimingLo, DramTimingHi;
	u8 tCK16x;
	u16 Twtr;
	uint8_t Etr[2];
	u8 LDIMM;
	u8 MTB16x;
	u8 byte;
	u32 dword;
	u32 dev;
	u32 val;

	printk(BIOS_DEBUG, "%s: Start\n", __func__);

	/* Gather all DIMM mini-max values for cycle timing data */
	Trp = 0;
	Trrd = 0;
	Trcd = 0;
	Trtp = 0;
	Tras = 0;
	Trc = 0;
	Twr = 0;
	Twtr = 0;
	for (i = 0; i < 2; i++)
		Etr[i] = 0;
	for (i = 0; i < 4; i++)
		Trfc[i] = 0;
	Tfaw = 0;

	for (i = 0; i< MAX_DIMMS_SUPPORTED; i++) {
		LDIMM = i >> 1;
		if (pDCTstat->DIMMValid & (1 << i)) {
			val = pDCTstat->spd_data.spd_bytes[dct + i][SPD_MTBDivisor];	/* MTB = Dividend/Divisor */
			MTB16x = ((pDCTstat->spd_data.spd_bytes[dct + i][SPD_MTBDividend] & 0xff) << 4);
			MTB16x /= val; /* transfer to MTB*16 */

			byte = pDCTstat->spd_data.spd_bytes[dct + i][SPD_tRPmin];
			val = byte * MTB16x;
			if (Trp < val)
				Trp = val;

			byte = pDCTstat->spd_data.spd_bytes[dct + i][SPD_tRRDmin];
			val = byte * MTB16x;
			if (Trrd < val)
				Trrd = val;

			byte = pDCTstat->spd_data.spd_bytes[dct + i][SPD_tRCDmin];
			val = byte * MTB16x;
			if (Trcd < val)
				Trcd = val;

			byte = pDCTstat->spd_data.spd_bytes[dct + i][SPD_tRTPmin];
			val = byte * MTB16x;
			if (Trtp < val)
				Trtp = val;

			byte = pDCTstat->spd_data.spd_bytes[dct + i][SPD_tWRmin];
			val = byte * MTB16x;
			if (Twr < val)
				Twr = val;

			byte = pDCTstat->spd_data.spd_bytes[dct + i][SPD_tWTRmin];
			val = byte * MTB16x;
			if (Twtr < val)
				Twtr = val;

			val = pDCTstat->spd_data.spd_bytes[dct + i][SPD_Upper_tRAS_tRC] & 0xff;
			val >>= 4;
			val <<= 8;
			val |= pDCTstat->spd_data.spd_bytes[dct + i][SPD_tRCmin] & 0xff;
			val *= MTB16x;
			if (Trc < val)
				Trc = val;

			byte = pDCTstat->spd_data.spd_bytes[dct + i][SPD_Density] & 0xf;
			if (Trfc[LDIMM] < byte)
				Trfc[LDIMM] = byte;

			val = pDCTstat->spd_data.spd_bytes[dct + i][SPD_Upper_tRAS_tRC] & 0xf;
			val <<= 8;
			val |= (pDCTstat->spd_data.spd_bytes[dct + i][SPD_tRASmin] & 0xff);
			val *= MTB16x;
			if (Tras < val)
				Tras = val;

			val = pDCTstat->spd_data.spd_bytes[dct + i][SPD_Upper_tFAW] & 0xf;
			val <<= 8;
			val |= pDCTstat->spd_data.spd_bytes[dct + i][SPD_tFAWmin] & 0xff;
			val *= MTB16x;
			if (Tfaw < val)
				Tfaw = val;

			/* Determine if the DIMMs on this channel support 95°C ETR */
			if (pDCTstat->spd_data.spd_bytes[dct + i][SPD_Thermal] & 0x1)
				Etr[dct] = 1;
		}	/* Dimm Present */
	}

	/* Convert  DRAM CycleTiming values and store into DCT structure */
	byte = pDCTstat->DIMMAutoSpeed;
	if (is_fam15h()) {
		if (byte == 0x16)
			tCK16x = 17;
		else if (byte == 0x12)
			tCK16x = 20;
		else if (byte == 0xe)
			tCK16x = 24;
		else if (byte == 0xa)
			tCK16x = 30;
		else if (byte == 0x6)
			tCK16x = 40;
		else
			tCK16x = 48;
	} else {
		if (byte == 7)
			tCK16x = 20;
		else if (byte == 6)
			tCK16x = 24;
		else if (byte == 5)
			tCK16x = 30;
		else
			tCK16x = 40;
	}

	/* Notes:
	 1. All secondary time values given in SPDs are in binary with units of ns.
	 2. Some time values are scaled by 16, in order to have least count of 0.25 ns
	    (more accuracy).  JEDEC SPD spec. shows which ones are x1 and x4.
	 3. Internally to this SW, cycle time, tCK16x, is scaled by 16 to match time values
	*/

	/* Tras */
	pDCTstat->DIMMTras = (u16)Tras;
	val = Tras / tCK16x;
	if (Tras % tCK16x) {	/* round up number of busclocks */
		val++;
	}
	if (val < Min_TrasT)
		val = Min_TrasT;
	else if (val > Max_TrasT)
		val = Max_TrasT;
	pDCTstat->Tras = val;

	/* Trp */
	pDCTstat->DIMMTrp = Trp;
	val = Trp / tCK16x;
	if (Trp % tCK16x) {	/* round up number of busclocks */
		val++;
	}
	if (val < Min_TrpT)
		val = Min_TrpT;
	else if (val > Max_TrpT)
		val = Max_TrpT;
	pDCTstat->Trp = val;

	/* Trrd */
	pDCTstat->DIMMTrrd = Trrd;
	val = Trrd / tCK16x;
	if (Trrd % tCK16x) {	/* round up number of busclocks */
		val++;
	}
	if (val < Min_TrrdT)
		val = Min_TrrdT;
	else if (val > Max_TrrdT)
		val = Max_TrrdT;
	pDCTstat->Trrd = val;

	/* Trcd */
	pDCTstat->DIMMTrcd = Trcd;
	val = Trcd / tCK16x;
	if (Trcd % tCK16x) {	/* round up number of busclocks */
		val++;
	}
	if (val < Min_TrcdT)
		val = Min_TrcdT;
	else if (val > Max_TrcdT)
		val = Max_TrcdT;
	pDCTstat->Trcd = val;

	/* Trc */
	pDCTstat->DIMMTrc = Trc;
	val = Trc / tCK16x;
	if (Trc % tCK16x) {	/* round up number of busclocks */
		val++;
	}
	if (val < Min_TrcT)
		val = Min_TrcT;
	else if (val > Max_TrcT)
		val = Max_TrcT;
	pDCTstat->Trc = val;

	/* Trtp */
	pDCTstat->DIMMTrtp = Trtp;
	val = Trtp / tCK16x;
	if (Trtp % tCK16x) {
		val ++;
	}
	if (val < Min_TrtpT)
		val = Min_TrtpT;
	else if (val > Max_TrtpT)
		val = Max_TrtpT;
	pDCTstat->Trtp = val;

	/* Twr */
	pDCTstat->DIMMTwr = Twr;
	val = Twr / tCK16x;
	if (Twr % tCK16x) {	/* round up number of busclocks */
		val++;
	}
	if (val < Min_TwrT)
		val = Min_TwrT;
	else if (val > Max_TwrT)
		val = Max_TwrT;
	pDCTstat->Twr = val;

	/* Twtr */
	pDCTstat->DIMMTwtr = Twtr;
	val = Twtr / tCK16x;
	if (Twtr % tCK16x) {	/* round up number of busclocks */
		val++;
	}
	if (val < Min_TwtrT)
		val = Min_TwtrT;
	else if (val > Max_TwtrT)
		val = Max_TwtrT;
	pDCTstat->Twtr = val;

	/* Trfc0-Trfc3 */
	for (i = 0; i < 4; i++)
		pDCTstat->Trfc[i] = Trfc[i];

	/* Tfaw */
	pDCTstat->DIMMTfaw = Tfaw;
	val = Tfaw / tCK16x;
	if (Tfaw % tCK16x) {	/* round up number of busclocks */
		val++;
	}
	if (val < Min_TfawT)
		val = Min_TfawT;
	else if (val > Max_TfawT)
		val = Max_TfawT;
	pDCTstat->Tfaw = val;

	mctAdjustAutoCycTmg_D();

	if (is_fam15h()) {
		/* Compute Tcwl (Fam15h BKDG v3.14 Table 203) */
		if (pDCTstat->Speed <= 0x6)
			Tcwl = 0x5;
		else if (pDCTstat->Speed == 0xa)
			Tcwl = 0x6;
		else if (pDCTstat->Speed == 0xe)
			Tcwl = 0x7;
		else if (pDCTstat->Speed == 0x12)
			Tcwl = 0x8;
		else if (pDCTstat->Speed == 0x16)
			Tcwl = 0x9;
		else
			Tcwl = 0x5;	/* Power-on default */

		/* Apply offset */
		Tcwl += pDCTstat->tcwl_delay[dct];
	}

	/* Program DRAM Timing values */
	if (is_fam15h()) {
		dev = pDCTstat->dev_dct;

		dword = Get_NB32_DCT(dev, dct, 0x8c);				/* DRAM Timing High */
		if (Etr[dct])
			val = 3;						/* Tref = 3.9us */
		else
			val = 2;						/* Tref = 7.8us */
		dword &= ~(0x3 << 16);
		dword |= (val & 0x3) << 16;
		Set_NB32_DCT(dev, dct, 0x8c, dword);				/* DRAM Timing High */

		dword = Get_NB32_DCT(dev, dct, 0x200);				/* DRAM Timing 0 */
		dword &= ~(0x3f1f1f1f);
		dword |= (pDCTstat->Tras & 0x3f) << 24;				/* Tras */
		val = pDCTstat->Trp;
		val = mct_AdjustSPDTimings(pMCTstat, pDCTstat, val);
		dword |= (val & 0x1f) << 16;					/* Trp */
		dword |= (pDCTstat->Trcd & 0x1f) << 8;				/* Trcd */
		dword |= (pDCTstat->CASL & 0x1f);				/* Tcl */
		Set_NB32_DCT(dev, dct, 0x200, dword);				/* DRAM Timing 0 */

		dword = Get_NB32_DCT(dev, dct, 0x204);				/* DRAM Timing 1 */
		dword &= ~(0x0f3f0f3f);
		dword |= (pDCTstat->Trtp & 0xf) << 24;				/* Trtp */
		if (pDCTstat->Tfaw != 0) {
			val = pDCTstat->Tfaw;
			val = mct_AdjustSPDTimings(pMCTstat, pDCTstat, val);
			if ((val > 0x5) && (val < 0x2b))
				dword |= (val & 0x3f) << 16;			/* FourActWindow */
		}
		dword |= (pDCTstat->Trrd & 0xf) << 8;				/* Trrd */
		dword |= (pDCTstat->Trc & 0x3f);				/* Trc */
		Set_NB32_DCT(dev, dct, 0x204, dword);				/* DRAM Timing 1 */

		/* Trfc0-Trfc3 */
		for (i = 0; i < 4; i++)
			if (pDCTstat->Trfc[i] == 0x0)
				pDCTstat->Trfc[i] = 0x1;
		dword = Get_NB32_DCT(dev, dct, 0x208);				/* DRAM Timing 2 */
		dword &= ~(0x07070707);
		dword |= (pDCTstat->Trfc[3] & 0x7) << 24;			/* Trfc3 */
		dword |= (pDCTstat->Trfc[2] & 0x7) << 16;			/* Trfc2 */
		dword |= (pDCTstat->Trfc[1] & 0x7) << 8;			/* Trfc1 */
		dword |= (pDCTstat->Trfc[0] & 0x7);				/* Trfc0 */
		Set_NB32_DCT(dev, dct, 0x208, dword);				/* DRAM Timing 2 */

		dword = Get_NB32_DCT(dev, dct, 0x20c);				/* DRAM Timing 3 */
		dword &= ~(0x00000f00);
		dword |= (pDCTstat->Twtr & 0xf) << 8;				/* Twtr */
		dword &= ~(0x0000001f);
		dword |= (Tcwl & 0x1f);						/* Tcwl */
		Set_NB32_DCT(dev, dct, 0x20c, dword);				/* DRAM Timing 3 */

		dword = Get_NB32_DCT(dev, dct, 0x22c);				/* DRAM Timing 10 */
		dword &= ~(0x0000001f);
		dword |= (pDCTstat->Twr & 0x1f);				/* Twr */
		Set_NB32_DCT(dev, dct, 0x22c, dword);				/* DRAM Timing 10 */

		if (pDCTstat->Speed > mhz_to_memclk_config(mctGet_NVbits(NV_MIN_MEMCLK))) {
			/* Enable phy-assisted training mode */
			fam15EnableTrainingMode(pMCTstat, pDCTstat, dct, 1);
		}

		/* Other setup (not training specific) */
		dword = Get_NB32_DCT(dev, dct, 0x90);				/* DRAM Configuration Low */
		dword &= ~(0x1 << 23);						/* ForceAutoPchg = 0 */
		dword &= ~(0x1 << 20);						/* DynPageCloseEn = 0 */
		Set_NB32_DCT(dev, dct, 0x90, dword);				/* DRAM Configuration Low */

		Set_NB32_DCT(dev, dct, 0x228, 0x14141414);			/* DRAM Timing 9 */
	} else {
		DramTimingLo = 0;	/* Dram Timing Low init */
		val = pDCTstat->CASL - 4; /* pDCTstat.CASL to reg. definition */
		DramTimingLo |= val;

		val = pDCTstat->Trcd - Bias_TrcdT;
		DramTimingLo |= val<<4;

		val = pDCTstat->Trp - Bias_TrpT;
		val = mct_AdjustSPDTimings(pMCTstat, pDCTstat, val);
		DramTimingLo |= val<<7;

		val = pDCTstat->Trtp - Bias_TrtpT;
		DramTimingLo |= val<<10;

		val = pDCTstat->Tras - Bias_TrasT;
		DramTimingLo |= val<<12;

		val = pDCTstat->Trc - Bias_TrcT;
		DramTimingLo |= val<<16;

		val = pDCTstat->Trrd - Bias_TrrdT;
		DramTimingLo |= val<<22;

		DramTimingHi = 0;	/* Dram Timing High init */
		val = pDCTstat->Twtr - Bias_TwtrT;
		DramTimingHi |= val<<8;

		val = 2;		/* Tref = 7.8us */
		DramTimingHi |= val<<16;

		val = 0;
		for (i = 4; i > 0; i--) {
			val <<= 3;
			val |= Trfc[i-1];
		}
		DramTimingHi |= val << 20;

		dev = pDCTstat->dev_dct;
		/* Twr */
		val = pDCTstat->Twr;
		if (val == 10)
			val = 9;
		else if (val == 12)
			val = 10;
		val = mct_AdjustSPDTimings(pMCTstat, pDCTstat, val);
		val -= Bias_TwrT;
		val <<= 4;
		dword = Get_NB32_DCT(dev, dct, 0x84);
		dword &= ~0x70;
		dword |= val;
		Set_NB32_DCT(dev, dct, 0x84, dword);

		/* Tfaw */
		val = pDCTstat->Tfaw;
		val = mct_AdjustSPDTimings(pMCTstat, pDCTstat, val);
		val -= Bias_TfawT;
		val >>= 1;
		val <<= 28;
		dword = Get_NB32_DCT(dev, dct, 0x94);
		dword &= ~0xf0000000;
		dword |= val;
		Set_NB32_DCT(dev, dct, 0x94, dword);

		/* dev = pDCTstat->dev_dct; */

		if (pDCTstat->Speed > mhz_to_memclk_config(mctGet_NVbits(NV_MIN_MEMCLK))) {
			val = Get_NB32_DCT(dev, dct, 0x88);
			val &= 0xFF000000;
			DramTimingLo |= val;
		}
		Set_NB32_DCT(dev, dct, 0x88, DramTimingLo);	/*DCT Timing Low*/

		if (pDCTstat->Speed > mhz_to_memclk_config(mctGet_NVbits(NV_MIN_MEMCLK))) {
			DramTimingHi |= 1 << DisAutoRefresh;
		}
		DramTimingHi |= 0x000018FF;
		Set_NB32_DCT(dev, dct, 0x8c, DramTimingHi);	/*DCT Timing Hi*/
	}

	/* dump_pci_device(PCI_DEV(0, 0x18+pDCTstat->Node_ID, 2)); */

	printk(BIOS_DEBUG, "%s: Done\n", __func__);
}

static u8 AutoCycTiming_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, uint8_t dct)
{
	/* Initialize  DCT Timing registers as per DIMM SPD.
	 * For primary timing (T, CL) use best case T value.
	 * For secondary timing params., use most aggressive settings
	 * of slowest DIMM.
	 *
	 * There are three components to determining "maximum frequency":
	 * SPD component, Bus load component, and "Preset" max frequency
	 * component.
	 *
	 * The SPD component is a function of the min cycle time specified
	 * by each DIMM, and the interaction of cycle times from all DIMMs
	 * in conjunction with CAS latency. The SPD component only applies
	 * when user timing mode is 'Auto'.
	 *
	 * The Bus load component is a limiting factor determined by electrical
	 * characteristics on the bus as a result of varying number of device
	 * loads. The Bus load component is specific to each platform but may
	 * also be a function of other factors. The bus load component only
	 * applies when user timing mode is 'Auto'.
	 *
	 * The Preset component is subdivided into three items and is
	 * the minimum of the set: Silicon revision, user limit
	 * setting when user timing mode is 'Auto' and memclock mode
	 * is 'Limit', OEM build specification of the maximum
	 * frequency. The Preset component is only applies when user
	 * timing mode is 'Auto'.
	 */

	printk(BIOS_DEBUG, "%s: Start\n", __func__);

	/* Get primary timing (CAS Latency and Cycle Time) */
	if (pDCTstat->Speed == 0) {
		mctGet_MaxLoadFreq(pDCTstat);

		/* and Factor in presets (setup options, Si cap, etc.) */
		GetPresetmaxF_D(pMCTstat, pDCTstat);

		/* Go get best T and CL as specified by DIMM mfgs. and OEM */
		SPDGetTCL_D(pMCTstat, pDCTstat, dct);

		/* skip callback mctForce800to1067_D */
		pDCTstat->Speed = pDCTstat->DIMMAutoSpeed;
		pDCTstat->CASL = pDCTstat->DIMMCASL;

	}
	mct_AfterGetCLT(pMCTstat, pDCTstat, dct);

	SPD2ndTiming(pMCTstat, pDCTstat, dct);

	printk(BIOS_DEBUG, "AutoCycTiming: Status %x\n", pDCTstat->Status);
	printk(BIOS_DEBUG, "AutoCycTiming: ErrStatus %x\n", pDCTstat->ErrStatus);
	printk(BIOS_DEBUG, "AutoCycTiming: ErrCode %x\n", pDCTstat->ErrCode);
	printk(BIOS_DEBUG, "AutoCycTiming: Done\n\n");

	mctHookAfterAutoCycTmg();

	return pDCTstat->ErrCode;
}

static void GetPresetmaxF_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat)
{
	/* Get max frequency from OEM platform definition, from any user
	 * override (limiting) of max frequency, and from any Si Revision
	 * Specific information.  Return the least of these three in
	 * DCTStatStruc.PresetmaxFreq.
	 */
	/* TODO: Set the proper max frequency in wrappers/mcti_d.c. */
	u16 proposedFreq;
	u16 word;

	printk(BIOS_DEBUG, "%s: Start\n", __func__);

	/* Get CPU Si Revision defined limit (NPT) */
	if (is_fam15h())
		proposedFreq = 933;
	else
		proposedFreq = 800;	 /* Rev F0 programmable max memclock is */

	/*Get User defined limit if  "limit" mode */
	if (mctGet_NVbits(NV_MCTUSRTMGMODE) == 1) {
		word = Get_Fk_D(mctGet_NVbits(NV_MemCkVal) + 1);
		if (word < proposedFreq)
			proposedFreq = word;

		/* Get Platform defined limit */
		word = mctGet_NVbits(NV_MAX_MEMCLK);
		if (word < proposedFreq)
			proposedFreq = word;

		word = pDCTstat->PresetmaxFreq;
		if (word > proposedFreq)
			word = proposedFreq;

		pDCTstat->PresetmaxFreq = word;
	}
	/* Check F3xE8[DdrMaxRate] for maximum DRAM data rate support */

	printk(BIOS_DEBUG, "%s: Done\n", __func__);
}

static void SPDGetTCL_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, uint8_t dct)
{
	/* Find the best T and CL primary timing parameter pair, per Mfg.,
	 * for the given set of DIMMs, and store into DCTStatStruc
	 * (.DIMMAutoSpeed and .DIMMCASL). See "Global relationship between
	 *  index values and item values" for definition of CAS latency
	 *  index (j) and Frequency index (k).
	 */
	u8 i, CASLatLow, CASLatHigh;
	u16 tAAmin16x;
	u8 MTB16x;
	u16 tCKmin16x;
	u16 tCKproposed16x;
	u8 CLactual, CLdesired, CLT_Fail;
	uint16_t min_frequency_tck16x;

	u8 byte = 0, bytex = 0;

	CASLatLow = 0xFF;
	CASLatHigh = 0xFF;
	tAAmin16x = 0;
	tCKmin16x = 0;
	CLT_Fail = 0;

	printk(BIOS_DEBUG, "%s: Start\n", __func__);

	if (is_fam15h()) {
		uint16_t minimum_frequency_mhz = mctGet_NVbits(NV_MIN_MEMCLK);
		if (minimum_frequency_mhz == 0)
			minimum_frequency_mhz = 333;
		min_frequency_tck16x = 16000 / minimum_frequency_mhz;
	} else {
		min_frequency_tck16x = 40;
	}

	for (i = 0; i < MAX_DIMMS_SUPPORTED; i++) {
		if (pDCTstat->DIMMValid & (1 << i)) {
			/* Step 1: Determine the common set of supported CAS Latency
			 * values for all modules on the memory channel using the CAS
			 * Latencies Supported in SPD bytes 14 and 15.
			 */
			byte = pDCTstat->spd_data.spd_bytes[dct + i][SPD_CASLow];
			CASLatLow &= byte;
			byte = pDCTstat->spd_data.spd_bytes[dct + i][SPD_CASHigh];
			CASLatHigh &= byte;
			/* Step 2: Determine tAAmin(all) which is the largest tAAmin
			   value for all modules on the memory channel (SPD byte 16). */
			byte = pDCTstat->spd_data.spd_bytes[dct + i][SPD_MTBDivisor];

			MTB16x = ((pDCTstat->spd_data.spd_bytes[dct + i][SPD_MTBDividend] & 0xFF)<<4);
			MTB16x /= byte; /* transfer to MTB*16 */

			byte = pDCTstat->spd_data.spd_bytes[dct + i][SPD_tAAmin];
			if (tAAmin16x < byte * MTB16x)
				tAAmin16x = byte * MTB16x;
			/* Step 3: Determine tCKmin(all) which is the largest tCKmin
			   value for all modules on the memory channel (SPD byte 12). */
			byte = pDCTstat->spd_data.spd_bytes[dct + i][SPD_tCKmin];

			if (tCKmin16x < byte * MTB16x)
				tCKmin16x = byte * MTB16x;
		}
	}
	/* calculate tCKproposed16x (proposed clock period in ns * 16) */
	tCKproposed16x =  16000 / pDCTstat->PresetmaxFreq;
	if (tCKmin16x > tCKproposed16x)
		tCKproposed16x = tCKmin16x;

	/* TODO: get user manual tCK16x(Freq.) and overwrite current tCKproposed16x if manual. */
	if (is_fam15h()) {
		if (tCKproposed16x == 17)
			pDCTstat->TargetFreq = 0x16;
		else if (tCKproposed16x <= 20) {
			pDCTstat->TargetFreq = 0x12;
			tCKproposed16x = 20;
		} else if (tCKproposed16x <= 24) {
			pDCTstat->TargetFreq = 0xe;
			tCKproposed16x = 24;
		} else if (tCKproposed16x <= 30) {
			pDCTstat->TargetFreq = 0xa;
			tCKproposed16x = 30;
		} else if (tCKproposed16x <= 40) {
			pDCTstat->TargetFreq = 0x6;
			tCKproposed16x = 40;
		} else {
			pDCTstat->TargetFreq = 0x4;
			tCKproposed16x = 48;
		}
	} else {
		if (tCKproposed16x == 20)
			pDCTstat->TargetFreq = 7;
		else if (tCKproposed16x <= 24) {
			pDCTstat->TargetFreq = 6;
			tCKproposed16x = 24;
		} else if (tCKproposed16x <= 30) {
			pDCTstat->TargetFreq = 5;
			tCKproposed16x = 30;
		} else {
			pDCTstat->TargetFreq = 4;
			tCKproposed16x = 40;
		}
	}
	/* Running through this loop twice:
	   - First time find tCL at target frequency
	   - Second time find tCL at 400MHz */

	for (;;) {
		CLT_Fail = 0;
		/* Step 4: For a proposed tCK value (tCKproposed) between tCKmin(all) and tCKmax,
		   determine the desired CAS Latency. If tCKproposed is not a standard JEDEC
		   value (2.5, 1.875, 1.5, or 1.25 ns) then tCKproposed must be adjusted to the
		   next lower standard tCK value for calculating CLdesired.
		   CLdesired = ceiling (tAAmin(all) / tCKproposed)
		   where tAAmin is defined in Byte 16. The ceiling function requires that the
		   quotient be rounded up always. */
		CLdesired = tAAmin16x / tCKproposed16x;
		if (tAAmin16x % tCKproposed16x)
			CLdesired ++;
		/* Step 5: Chose an actual CAS Latency (CLactual) that is greather than or equal
		   to CLdesired and is supported by all modules on the memory channel as
		   determined in step 1. If no such value exists, choose a higher tCKproposed
		   value and repeat steps 4 and 5 until a solution is found. */
		for (i = 0, CLactual = 4; i < 15; i++, CLactual++) {
			if ((CASLatHigh << 8 | CASLatLow) & (1 << i)) {
				if (CLdesired <= CLactual)
					break;
			}
		}
		if (i == 15)
			CLT_Fail = 1;
		/* Step 6: Once the calculation of CLactual is completed, the BIOS must also
		   verify that this CAS Latency value does not exceed tAAmax, which is 20 ns
		   for all DDR3 speed grades, by multiplying CLactual times tCKproposed. If
		   not, choose a lower CL value and repeat steps 5 and 6 until a solution is found. */
		if (CLactual * tCKproposed16x > 320)
			CLT_Fail = 1;
		/* get CL and T */
		if (!CLT_Fail) {
			bytex = CLactual;
			if (is_fam15h()) {
				if (tCKproposed16x == 17)
					byte = 0x16;
				else if (tCKproposed16x == 20)
					byte = 0x12;
				else if (tCKproposed16x == 24)
					byte = 0xe;
				else if (tCKproposed16x == 30)
					byte = 0xa;
				else if (tCKproposed16x == 40)
					byte = 0x6;
				else
					byte = 0x4;
			} else {
				if (tCKproposed16x == 20)
					byte = 7;
				else if (tCKproposed16x == 24)
					byte = 6;
				else if (tCKproposed16x == 30)
					byte = 5;
				else
					byte = 4;
			}
		} else {
			/* mctHookManualCLOverride */
			/* TODO: */
		}

		if (tCKproposed16x != min_frequency_tck16x) {
			if (pMCTstat->GStatus & (1 << GSB_EnDIMMSpareNW)) {
				pDCTstat->DIMMAutoSpeed = byte;
				pDCTstat->DIMMCASL = bytex;
				break;
			} else {
				pDCTstat->TargetCASL = bytex;
				tCKproposed16x = min_frequency_tck16x;
			}
		} else {
			pDCTstat->DIMMAutoSpeed = byte;
			pDCTstat->DIMMCASL = bytex;
			break;
		}
	}

	printk(BIOS_DEBUG, "SPDGetTCL_D: DIMMCASL %x\n", pDCTstat->DIMMCASL);
	printk(BIOS_DEBUG, "SPDGetTCL_D: DIMMAutoSpeed %x\n", pDCTstat->DIMMAutoSpeed);

	printk(BIOS_DEBUG, "SPDGetTCL_D: Status %x\n", pDCTstat->Status);
	printk(BIOS_DEBUG, "SPDGetTCL_D: ErrStatus %x\n", pDCTstat->ErrStatus);
	printk(BIOS_DEBUG, "SPDGetTCL_D: ErrCode %x\n", pDCTstat->ErrCode);
	printk(BIOS_DEBUG, "SPDGetTCL_D: Done\n\n");
}

u8 PlatformSpec_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct)
{
	if (!is_fam15h()) {
		mctGet_PS_Cfg_D(pMCTstat, pDCTstat, dct);

		if (pDCTstat->GangedMode == 1) {
			mctGet_PS_Cfg_D(pMCTstat, pDCTstat, 1);
			mct_BeforePlatformSpec(pMCTstat, pDCTstat, 1);
		}

		set_2t_configuration(pMCTstat, pDCTstat, dct);

		mct_BeforePlatformSpec(pMCTstat, pDCTstat, dct);
		mct_PlatformSpec(pMCTstat, pDCTstat, dct);
		if (pDCTstat->DIMMAutoSpeed == mhz_to_memclk_config(mctGet_NVbits(NV_MIN_MEMCLK)))
			InitPhyCompensation(pMCTstat, pDCTstat, dct);
	}
	mctHookAfterPSCfg();

	return pDCTstat->ErrCode;
}

static u8 AutoConfig_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct)
{
	u32 DramControl, DramTimingLo, Status;
	u32 DramConfigLo, DramConfigHi, DramConfigMisc, DramConfigMisc2;
	u32 val;
	u32 dev;
	u16 word;
	u32 dword;
	u8 byte;
	uint32_t offset;

	DramConfigLo = 0;
	DramConfigHi = 0;
	DramConfigMisc = 0;
	DramConfigMisc2 = 0;

	/* set bank addressing and Masks, plus CS pops */
	SPDSetBanks_D(pMCTstat, pDCTstat, dct);
	if (pDCTstat->ErrCode == SC_StopError)
		goto AutoConfig_exit;

	/* map chip-selects into local address space */
	StitchMemory_D(pMCTstat, pDCTstat, dct);
	InterleaveBanks_D(pMCTstat, pDCTstat, dct);

	/* temp image of status (for convenience). RO usage! */
	Status = pDCTstat->Status;

	dev = pDCTstat->dev_dct;

	/* Build Dram Control Register Value */
	DramConfigMisc2 = Get_NB32_DCT(dev, dct, 0xa8);		/* Dram Miscellaneous 2 */
	DramControl = Get_NB32_DCT(dev, dct, 0x78);		/* Dram Control */

	/* FIXME: Skip mct_checkForDxSupport */
	/* REV_CALL mct_DoRdPtrInit if not Dx */
	if (pDCTstat->LogicalCPUID & AMD_DR_Bx)
		val = 5;
	else
		val = 6;
	DramControl &= ~0xFF;
	DramControl |= val;	/* RdPtrInit = 6 for Cx CPU */

	if (mctGet_NVbits(NV_CLKHZAltVidC3))
		DramControl |= 1<<16; /* check */

	DramControl |= 0x00002A00;

	/* FIXME: Skip for Ax versions */
	/* callback not required - if (!mctParityControl_D()) */
	if (Status & (1 << SB_128bitmode))
		DramConfigLo |= 1 << Width128;	/* 128-bit mode (normal) */

	word = dct;
	dword = X4Dimm;
	while (word < 8) {
		if (pDCTstat->Dimmx4Present & (1 << word))
			DramConfigLo |= 1 << dword;	/* X4Dimm[3:0] */
		word++;
		word++;
		dword++;
	}

	if (Status & (1 << SB_Registered)) {
		/* Registered DIMMs */
		if (!is_fam15h()) {
			DramConfigLo |= 1 << ParEn;
		}
	} else {
		/* Unbuffered DIMMs */
		DramConfigLo |= 1 << UnBuffDimm;
	}

	if (mctGet_NVbits(NV_ECC_CAP))
		if (Status & (1 << SB_ECCDIMMs))
			if (mctGet_NVbits(NV_ECC))
				DramConfigLo |= 1 << DimmEcEn;

	DramConfigLo = mct_DisDllShutdownSR(pMCTstat, pDCTstat, DramConfigLo, dct);

	/* Build Dram Config Hi Register Value */
	if (is_fam15h())
		offset = 0x0;
	else
		offset = 0x1;
	dword = pDCTstat->Speed;
	DramConfigHi |= dword - offset;	/* get MemClk encoding */
	DramConfigHi |= 1 << MemClkFreqVal;

	if (!is_fam15h())
		if (Status & (1 << SB_Registered))
			if ((pDCTstat->Dimmx4Present != 0) && (pDCTstat->Dimmx8Present != 0))
				/* set only if x8 Registered DIMMs in System*/
				DramConfigHi |= 1 << RDqsEn;

	if (pDCTstat->LogicalCPUID & AMD_FAM15_ALL) {
		DramConfigLo |= 1 << 25;	/* PendRefPaybackS3En = 1 */
		DramConfigLo |= 1 << 24;	/* StagRefEn = 1 */
		DramConfigHi |= 1 << 16;	/* PowerDownMode = 1 */
	} else {
		if (mctGet_NVbits(NV_CKE_CTL))
			/*Chip Select control of CKE*/
			DramConfigHi |= 1 << 16;
	}

	if (!is_fam15h()) {
		/* Control Bank Swizzle */
		if (0) /* call back not needed mctBankSwizzleControl_D()) */
			DramConfigHi &= ~(1 << BankSwizzleMode);
		else
			DramConfigHi |= 1 << BankSwizzleMode; /* recommended setting (default) */
	}

	/* Check for Quadrank DIMM presence */
	if (pDCTstat->DimmQRPresent != 0) {
		byte = mctGet_NVbits(NV_4RANKType);
		if (byte == 2)
			DramConfigHi |= 1 << 17;	/* S4 (4-Rank SO-DIMMs) */
		else if (byte == 1)
			DramConfigHi |= 1 << 18;	/* R4 (4-Rank Registered DIMMs) */
	}

	if (0) /* call back not needed mctOverrideDcqBypMax_D) */
		val = mctGet_NVbits(NV_BYPMAX);
	else
		val = 0x0f; /* recommended setting (default) */
	DramConfigHi |= val << 24;

	if (pDCTstat->LogicalCPUID & (AMD_DR_Dx | AMD_DR_Cx | AMD_DR_Bx | AMD_FAM15_ALL))
		DramConfigHi |= 1 << DcqArbBypassEn;

	/* Build MemClkDis Value from Dram Timing Lo and
	   Dram Config Misc Registers
	 1. We will assume that MemClkDis field has been preset prior to this
	    point.
	 2. We will only set MemClkDis bits if a DIMM is NOT present AND if:
	    NV_AllMemClks <>0 AND SB_DiagClks == 0 */

	/* Dram Timing Low (owns Clock Enable bits) */
	DramTimingLo = Get_NB32_DCT(dev, dct, 0x88);
	if (mctGet_NVbits(NV_AllMemClks) == 0) {
		/* Special Jedec SPD diagnostic bit - "enable all clocks" */
		if (!(pDCTstat->Status & (1<<SB_DiagClks))) {
			const u8 *p;
			const u32 *q;
			p = Tab_ManualCLKDis;
			q = (u32 *)p;

			byte = mctGet_NVbits(NV_PACK_TYPE);
			if (byte == PT_L1)
				p = Tab_L1CLKDis;
			else if (byte == PT_M2 || byte == PT_AS)
				p = Tab_AM3CLKDis;
			else if (byte == PT_C3)
				p = Tab_C32CLKDis;
			else if (byte == PT_GR)
				p = Tab_G34CLKDis;
			else if (byte == PT_FM2)
				p = Tab_FM2CLKDis;
			else
				p = Tab_S1CLKDis;

			dword = 0;
			byte = 0xFF;
			while (dword < MAX_CS_SUPPORTED) {
				if (pDCTstat->CSPresent & (1<<dword)) {
					/* re-enable clocks for the enabled CS */
					val = p[dword];
					byte &= ~val;
				}
				dword++;
			}
			DramTimingLo &= ~(0xff << 24);
			DramTimingLo |= byte << 24;
		}
	}

	DramConfigMisc2 = mct_SetDramConfigMisc2(pDCTstat, dct, DramConfigMisc2, DramControl);

	printk(BIOS_DEBUG, "AutoConfig_D: DramControl:     %08x\n", DramControl);
	printk(BIOS_DEBUG, "AutoConfig_D: DramTimingLo:    %08x\n", DramTimingLo);
	printk(BIOS_DEBUG, "AutoConfig_D: DramConfigMisc:  %08x\n", DramConfigMisc);
	printk(BIOS_DEBUG, "AutoConfig_D: DramConfigMisc2: %08x\n", DramConfigMisc2);
	printk(BIOS_DEBUG, "AutoConfig_D: DramConfigLo:    %08x\n", DramConfigLo);
	printk(BIOS_DEBUG, "AutoConfig_D: DramConfigHi:    %08x\n", DramConfigHi);

	/* Write Values to the registers */
	Set_NB32_DCT(dev, dct, 0x78, DramControl);
	Set_NB32_DCT(dev, dct, 0x88, DramTimingLo);
	Set_NB32_DCT(dev, dct, 0xa0, DramConfigMisc);
	Set_NB32_DCT(dev, dct, 0xa8, DramConfigMisc2);
	Set_NB32_DCT(dev, dct, 0x90, DramConfigLo);
	ProgDramMRSReg_D(pMCTstat, pDCTstat, dct);

	if (is_fam15h())
		InitDDRPhy(pMCTstat, pDCTstat, dct);

	/* Write the DRAM Configuration High register, including memory frequency change */
	dword = Get_NB32_DCT(dev, dct, 0x94);
	DramConfigHi |= dword;
	mct_SetDramConfigHi_D(pMCTstat, pDCTstat, dct, DramConfigHi);
	mct_EarlyArbEn_D(pMCTstat, pDCTstat, dct);
	mctHookAfterAutoCfg();

	/* dump_pci_device(PCI_DEV(0, 0x18+pDCTstat->Node_ID, 2)); */

	printk(BIOS_DEBUG, "AutoConfig: Status %x\n", pDCTstat->Status);
	printk(BIOS_DEBUG, "AutoConfig: ErrStatus %x\n", pDCTstat->ErrStatus);
	printk(BIOS_DEBUG, "AutoConfig: ErrCode %x\n", pDCTstat->ErrCode);
	printk(BIOS_DEBUG, "AutoConfig: Done\n\n");

AutoConfig_exit:
	return pDCTstat->ErrCode;
}

static void SPDSetBanks_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct)
{
	/* Set bank addressing, program Mask values and build a chip-select
	 * population map. This routine programs PCI 0:24N:2x80 config register
	 * and PCI 0:24N:2x60,64,68,6C config registers (CS Mask 0-3).
	 */
	u8 ChipSel, Rows, Cols, Ranks, Banks;
	u32 BankAddrReg, csMask;

	u32 val;
	u32 reg;
	u32 dev;
	u8 byte;
	u16 word;
	u32 dword;

	dev = pDCTstat->dev_dct;

	BankAddrReg = 0;
	for (ChipSel = 0; ChipSel < MAX_CS_SUPPORTED; ChipSel+=2) {
		byte = ChipSel;
		if ((pDCTstat->Status & (1 << SB_64MuxedMode)) && ChipSel >=4)
			byte -= 3;

		if (pDCTstat->DIMMValid & (1<<byte)) {
			byte = pDCTstat->spd_data.spd_bytes[ChipSel + dct][SPD_Addressing];
			Rows = (byte >> 3) & 0x7; /* Rows:0b = 12-bit,... */
			Cols = byte & 0x7; /* Cols:0b = 9-bit,... */

			byte = pDCTstat->spd_data.spd_bytes[ChipSel + dct][SPD_Density];
			Banks = (byte >> 4) & 7; /* Banks:0b = 3-bit,... */

			byte = pDCTstat->spd_data.spd_bytes[ChipSel + dct][SPD_Organization];
			Ranks = ((byte >> 3) & 7) + 1;

			/* Configure Bank encoding
			 * Use a 6-bit key into a lookup table.
			 * Key (index) = RRRBCC, where CC is the number of Columns minus 9,
			 * RRR is the number of Rows minus 12, and B is the number of banks
			 * minus 3.
			 */
			byte = Cols;
			if (Banks == 1)
				byte |= 4;

			byte |= Rows << 3;	/* RRRBCC internal encode */

			for (dword = 0; dword < 13; dword++) {
				if (byte == Tab_BankAddr[dword])
					break;
			}

			if (dword > 12)
				continue;

			/* bit no. of CS field in address mapping reg.*/
			dword <<= (ChipSel<<1);
			BankAddrReg |= dword;

			/* Mask value=(2pow(rows+cols+banks+3)-1)>>8,
			   or 2pow(rows+cols+banks-5)-1*/
			csMask = 0;

			byte = Rows + Cols;		/* cl = rows+cols*/
			byte += 21;			/* row:12+col:9 */
			byte -= 2;			/* 3 banks - 5 */

			if (pDCTstat->Status & (1 << SB_128bitmode))
				byte++;		/* double mask size if in 128-bit mode*/

			csMask |= 1 << byte;
			csMask--;

			/*set ChipSelect population indicator even bits*/
			pDCTstat->CSPresent |= (1<<ChipSel);
			if (Ranks >= 2)
				/*set ChipSelect population indicator odd bits*/
				pDCTstat->CSPresent |= 1 << (ChipSel + 1);

			reg = 0x60+(ChipSel<<1);	/*Dram CS Mask Register */
			val = csMask;
			val &= 0x1FF83FE0;	/* Mask out reserved bits.*/
			Set_NB32_DCT(dev, dct, reg, val);
		} else {
			if (pDCTstat->DIMMSPDCSE & (1<<ChipSel))
				pDCTstat->CSTestFail |= (1<<ChipSel);
		}	/* if DIMMValid*/
	}	/* while ChipSel*/

	SetCSTriState(pMCTstat, pDCTstat, dct);
	SetCKETriState(pMCTstat, pDCTstat, dct);
	SetODTTriState(pMCTstat, pDCTstat, dct);

	if (pDCTstat->Status & (1 << SB_128bitmode)) {
		SetCSTriState(pMCTstat, pDCTstat, 1); /* force dct1) */
		SetCKETriState(pMCTstat, pDCTstat, 1); /* force dct1) */
		SetODTTriState(pMCTstat, pDCTstat, 1); /* force dct1) */
	}

	word = pDCTstat->CSPresent;
	mctGetCS_ExcludeMap();		/* mask out specified chip-selects */
	word ^= pDCTstat->CSPresent;
	pDCTstat->CSTestFail |= word;	/* enable ODT to disabled DIMMs */
	if (!pDCTstat->CSPresent)
		pDCTstat->ErrCode = SC_StopError;

	reg = 0x80;		/* Bank Addressing Register */
	Set_NB32_DCT(dev, dct, reg, BankAddrReg);

	pDCTstat->CSPresent_DCT[dct] = pDCTstat->CSPresent;
	/* dump_pci_device(PCI_DEV(0, 0x18+pDCTstat->Node_ID, 2)); */

	printk(BIOS_DEBUG, "SPDSetBanks: CSPresent %x\n", pDCTstat->CSPresent_DCT[dct]);
	printk(BIOS_DEBUG, "SPDSetBanks: Status %x\n", pDCTstat->Status);
	printk(BIOS_DEBUG, "SPDSetBanks: ErrStatus %x\n", pDCTstat->ErrStatus);
	printk(BIOS_DEBUG, "SPDSetBanks: ErrCode %x\n", pDCTstat->ErrCode);
	printk(BIOS_DEBUG, "SPDSetBanks: Done\n\n");
}

static void SPDCalcWidth_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat)
{
	/* Per SPDs, check the symmetry of DIMM pairs (DIMM on Channel A
	 *  matching with DIMM on Channel B), the overall DIMM population,
	 * and determine the width mode: 64-bit, 64-bit muxed, 128-bit.
	 */
	u8 i;
	u8 byte, byte1;

	/* Check Symmetry of Channel A and Channel B DIMMs
	  (must be matched for 128-bit mode).*/
	for (i = 0; i < MAX_DIMMS_SUPPORTED; i += 2) {
		if ((pDCTstat->DIMMValid & (1 << i)) && (pDCTstat->DIMMValid & (1<<(i+1)))) {
			byte = pDCTstat->spd_data.spd_bytes[i][SPD_Addressing] & 0x7;
			byte1 = pDCTstat->spd_data.spd_bytes[i + 1][SPD_Addressing] & 0x7;
			if (byte != byte1) {
				pDCTstat->ErrStatus |= (1<<SB_DimmMismatchO);
				break;
			}

			byte =	 pDCTstat->spd_data.spd_bytes[i][SPD_Density] & 0x0f;
			byte1 =	 pDCTstat->spd_data.spd_bytes[i + 1][SPD_Density] & 0x0f;
			if (byte != byte1) {
				pDCTstat->ErrStatus |= (1<<SB_DimmMismatchO);
				break;
			}

			byte = pDCTstat->spd_data.spd_bytes[i][SPD_Organization] & 0x7;
			byte1 = pDCTstat->spd_data.spd_bytes[i + 1][SPD_Organization] & 0x7;
			if (byte != byte1) {
				pDCTstat->ErrStatus |= (1<<SB_DimmMismatchO);
				break;
			}

			byte = (pDCTstat->spd_data.spd_bytes[i][SPD_Organization] >> 3) & 0x7;
			byte1 = (pDCTstat->spd_data.spd_bytes[i + 1][SPD_Organization] >> 3) & 0x7;
			if (byte != byte1) {
				pDCTstat->ErrStatus |= (1<<SB_DimmMismatchO);
				break;
			}

			byte = pDCTstat->spd_data.spd_bytes[i][SPD_DMBANKS] & 7;	 /* #ranks-1 */
			byte1 = pDCTstat->spd_data.spd_bytes[i + 1][SPD_DMBANKS] & 7;	  /* #ranks-1 */
			if (byte != byte1) {
				pDCTstat->ErrStatus |= (1<<SB_DimmMismatchO);
				break;
			}

		}
	}

}

static void StitchMemory_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct)
{
	/* Requires that Mask values for each bank be programmed first and that
	 * the chip-select population indicator is correctly set.
	 */
	u8 b = 0;
	u32 nxtcsBase, curcsBase;
	u8 p, q;
	u32 Sizeq, BiggestBank;
	u8 _DSpareEn;

	u16 word;
	u32 dev;
	u32 reg;
	u32 val;

	dev = pDCTstat->dev_dct;

	_DSpareEn = 0;

	/* CS Sparing 1 = enabled, 0 = disabled */
	if (mctGet_NVbits(NV_CS_SpareCTL) & 1) {
		if (MCT_DIMM_SPARE_NO_WARM) {
			/* Do no warm-reset DIMM spare */
			if (pMCTstat->GStatus & 1 << GSB_EnDIMMSpareNW) {
				word = pDCTstat->CSPresent;
				val = bsf(word);
				word &= ~(1<<val);
				if (word)
					/* Make sure at least two chip-selects are available */
					_DSpareEn = 1;
				else
					pDCTstat->ErrStatus |= 1 << SB_SpareDis;
			}
		} else {
			if (!mctGet_NVbits(NV_DQSTrainCTL)) { /*DQS Training 1 = enabled, 0 = disabled */
				word = pDCTstat->CSPresent;
				val = bsf(word);
				word &= ~(1 << val);
				if (word)
					/* Make sure at least two chip-selects are available */
					_DSpareEn = 1;
				else
					pDCTstat->ErrStatus |= 1 << SB_SpareDis;
			}
		}
	}

	nxtcsBase = 0;		/* Next available cs base ADDR[39:8] */
	for (p = 0; p < MAX_DIMMS_SUPPORTED; p++) {
		BiggestBank = 0;
		for (q = 0; q < MAX_CS_SUPPORTED; q++) { /* from DIMMS to CS */
			if (pDCTstat->CSPresent & (1 << q)) {  /* bank present? */
				reg  = 0x40 + (q << 2);  /* Base[q] reg.*/
				val = Get_NB32_DCT(dev, dct, reg);
				if (!(val & 3)) {	/* (CSEnable|Spare == 1)bank is enabled already? */
					reg = 0x60 + (q << 1); /*Mask[q] reg.*/
					val = Get_NB32_DCT(dev, dct, reg);
					val >>= 19;
					val++;
					val <<= 19;
					Sizeq = val;  /* never used */
					if (val > BiggestBank) {
						/*Bingo! possibly Map this chip-select next! */
						BiggestBank = val;
						b = q;
					}
				}
			}	/*if bank present */
		}	/* while q */
		if (BiggestBank !=0) {
			curcsBase = nxtcsBase;		/* curcsBase = nxtcsBase*/
			/* DRAM CS Base b Address Register offset */
			reg = 0x40 + (b << 2);
			if (_DSpareEn) {
				BiggestBank = 0;
				val = 1 << Spare;	/* Spare Enable*/
			} else {
				val = curcsBase;
				val |= 1 << CSEnable;	/* Bank Enable */
			}
			if (((reg - 0x40) >> 2) & 1) {
				if (!(pDCTstat->Status & (1 << SB_Registered))) {
					u16  dimValid;
					dimValid = pDCTstat->DIMMValid;
					if (dct & 1)
						dimValid <<= 1;
					if ((dimValid & pDCTstat->MirrPresU_NumRegR) != 0) {
						val |= 1 << onDimmMirror;
					}
				}
			}
			Set_NB32_DCT(dev, dct, reg, val);
			if (_DSpareEn)
				_DSpareEn = 0;
			else
				/* let nxtcsBase+=Size[b] */
				nxtcsBase += BiggestBank;
		}

		/* bank present but disabled?*/
		if (pDCTstat->CSTestFail & (1 << p)) {
			/* DRAM CS Base b Address Register offset */
			reg = (p << 2) + 0x40;
			val = 1 << TestFail;
			Set_NB32_DCT(dev, dct, reg, val);
		}
	}

	if (nxtcsBase) {
		pDCTstat->DCTSysLimit = nxtcsBase - 1;
		mct_AfterStitchMemory(pMCTstat, pDCTstat, dct);
	}

	/* dump_pci_device(PCI_DEV(0, 0x18+pDCTstat->Node_ID, 2)); */

	printk(BIOS_DEBUG, "StitchMemory: Status %x\n", pDCTstat->Status);
	printk(BIOS_DEBUG, "StitchMemory: ErrStatus %x\n", pDCTstat->ErrStatus);
	printk(BIOS_DEBUG, "StitchMemory: ErrCode %x\n", pDCTstat->ErrCode);
	printk(BIOS_DEBUG, "StitchMemory: Done\n\n");
}

static u16 Get_Fk_D(u8 k)
{
	return Table_F_k[k]; /* FIXME: k or k<<1 ? */
}

static u8 DIMMPresence_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat)
{
	/* Check DIMMs present, verify checksum, flag SDRAM type,
	 * build population indicator bitmaps, and preload bus loading
	 * of DIMMs into DCTStatStruc.
	 * MAAload = number of devices on the "A" bus.
	 * MABload = number of devices on the "B" bus.
	 * MAAdimms = number of DIMMs on the "A" bus slots.
	 * MABdimms = number of DIMMs on the "B" bus slots.
	 * DATAAload = number of ranks on the "A" bus slots.
	 * DATABload = number of ranks on the "B" bus slots.
	 */
	u16 i, j, k;
	u8 smbaddr;
	u8 SPDCtrl;
	u16 RegDIMMPresent, LRDIMMPresent, MaxDimms;
	u8 devwidth;
	u16 DimmSlots;
	u8 byte = 0, bytex;
	uint8_t crc_status;

	/* preload data structure with addrs */
	mctGet_DIMMAddr(pDCTstat, pDCTstat->Node_ID);

	DimmSlots = MaxDimms = mctGet_NVbits(NV_MAX_DIMMS);

	SPDCtrl = mctGet_NVbits(NV_SPDCHK_RESTRT);

	RegDIMMPresent = 0;
	LRDIMMPresent = 0;
	pDCTstat->DimmQRPresent = 0;

	for (i = 0; i < MAX_DIMMS_SUPPORTED; i++) {
		if (i >= MaxDimms)
			break;

		if ((pDCTstat->DimmQRPresent & (1 << i)) || (i < DimmSlots)) {
			int status;
			smbaddr = Get_DIMMAddress_D(pDCTstat, i);
			status = mctRead_SPD(smbaddr, SPD_ByteUse);
			if (status >= 0) {
				/* Verify result */
				status = mctRead_SPD(smbaddr, SPD_ByteUse);
			}
			if (status >= 0) { /* SPD access is ok */
				pDCTstat->DIMMPresent |= 1 << i;
				read_spd_bytes(pMCTstat, pDCTstat, i);
#ifdef DEBUG_DIMM_SPD
				dump_spd_bytes(pMCTstat, pDCTstat, i);
#endif
				crc_status = crcCheck(pDCTstat, i);
				if (!crc_status) {
					/* Try again in case there was a transient glitch */
					read_spd_bytes(pMCTstat, pDCTstat, i);
					crc_status = crcCheck(pDCTstat, i);
				}
				if ((crc_status) || (SPDCtrl == 2)) { /* CRC is OK */
					byte = pDCTstat->spd_data.spd_bytes[i][SPD_TYPE];
					if (byte == JED_DDR3SDRAM) {
						/*Dimm is 'Present'*/
						pDCTstat->DIMMValid |= 1 << i;
					}
				} else {
					printk(BIOS_WARNING, "Node %d DIMM %d: SPD checksum invalid\n", pDCTstat->Node_ID, i);
					pDCTstat->DIMMSPDCSE = 1 << i;
					if (SPDCtrl == 0) {
						pDCTstat->ErrStatus |= 1 << SB_DIMMChkSum;
						pDCTstat->ErrCode = SC_StopError;
					} else {
						/*if NV_SPDCHK_RESTRT is set to 1, ignore faulty SPD checksum*/
						pDCTstat->ErrStatus |= 1<<SB_DIMMChkSum;
						byte = pDCTstat->spd_data.spd_bytes[i][SPD_TYPE];
						if (byte == JED_DDR3SDRAM)
							pDCTstat->DIMMValid |= 1 << i;
					}
				}

				/* Zero DIMM SPD data cache if DIMM not present / valid */
				if (!(pDCTstat->DIMMValid & (1 << i)))
					memset(pDCTstat->spd_data.spd_bytes[i], 0, sizeof(pDCTstat->spd_data.spd_bytes[i]));

				/* Get module information for SMBIOS */
				if (pDCTstat->DIMMValid & (1 << i)) {
					pDCTstat->DimmManufacturerID[i] = 0;
					for (k = 0; k < 8; k++)
						pDCTstat->DimmManufacturerID[i] |= ((uint64_t)pDCTstat->spd_data.spd_bytes[i][SPD_MANID_START + k]) << (k * 8);
					for (k = 0; k < SPD_PARTN_LENGTH; k++)
						pDCTstat->DimmPartNumber[i][k] = pDCTstat->spd_data.spd_bytes[i][SPD_PARTN_START + k];
					pDCTstat->DimmPartNumber[i][SPD_PARTN_LENGTH] = 0;
					pDCTstat->DimmRevisionNumber[i] = 0;
					for (k = 0; k < 2; k++)
						pDCTstat->DimmRevisionNumber[i] |= ((uint16_t)pDCTstat->spd_data.spd_bytes[i][SPD_REVNO_START + k]) << (k * 8);
					pDCTstat->DimmSerialNumber[i] = 0;
					for (k = 0; k < 4; k++)
						pDCTstat->DimmSerialNumber[i] |= ((uint32_t)pDCTstat->spd_data.spd_bytes[i][SPD_SERIAL_START + k]) << (k * 8);
					pDCTstat->DimmRows[i] = (pDCTstat->spd_data.spd_bytes[i][SPD_Addressing] & 0x38) >> 3;
					pDCTstat->DimmCols[i] = pDCTstat->spd_data.spd_bytes[i][SPD_Addressing] & 0x7;
					pDCTstat->DimmRanks[i] = ((pDCTstat->spd_data.spd_bytes[i][SPD_Organization] & 0x38) >> 3) + 1;
					pDCTstat->DimmBanks[i] = 1ULL << (((pDCTstat->spd_data.spd_bytes[i][SPD_Density] & 0x70) >> 4) + 3);
					pDCTstat->DimmWidth[i] = 1ULL << ((pDCTstat->spd_data.spd_bytes[i][SPD_BusWidth] & 0x7) + 3);
					pDCTstat->DimmChipSize[i] = 1ULL << ((pDCTstat->spd_data.spd_bytes[i][SPD_Density] & 0xf) + 28);
					pDCTstat->DimmChipWidth[i] = 1ULL << ((pDCTstat->spd_data.spd_bytes[i][SPD_Organization] & 0x7) + 2);
				}
				/* Check supported voltage(s) */
				pDCTstat->DimmSupportedVoltages[i] = pDCTstat->spd_data.spd_bytes[i][SPD_Voltage] & 0x7;
				pDCTstat->DimmSupportedVoltages[i] ^= 0x1;	/* Invert LSB to convert from SPD format to internal bitmap format */
				/* Check module type */
				byte = pDCTstat->spd_data.spd_bytes[i][SPD_DIMMTYPE] & 0x7;
				if (byte == JED_RDIMM || byte == JED_MiniRDIMM) {
					RegDIMMPresent |= 1 << i;
					pDCTstat->DimmRegistered[i] = 1;
				} else {
					pDCTstat->DimmRegistered[i] = 0;
				}
				if (byte == JED_LRDIMM) {
					LRDIMMPresent |= 1 << i;
					pDCTstat->DimmLoadReduced[i] = 1;
				} else {
					pDCTstat->DimmLoadReduced[i] = 0;
				}
				/* Check ECC capable */
				byte = pDCTstat->spd_data.spd_bytes[i][SPD_BusWidth];
				if (byte & JED_ECC) {
					/* DIMM is ECC capable */
					pDCTstat->DimmECCPresent |= 1 << i;
				}
				/* Check if x4 device */
				devwidth = pDCTstat->spd_data.spd_bytes[i][SPD_Organization] & 0x7; /* 0:x4,1:x8,2:x16 */
				if (devwidth == 0) {
					/* DIMM is made with x4 or x16 drams */
					pDCTstat->Dimmx4Present |= 1 << i;
				} else if (devwidth == 1) {
					pDCTstat->Dimmx8Present |= 1 << i;
				} else if (devwidth == 2) {
					pDCTstat->Dimmx16Present |= 1 << i;
				}

				byte = (pDCTstat->spd_data.spd_bytes[i][SPD_Organization] >> 3);
				byte &= 7;
				if (byte == 3) { /* 4ranks */
					/* if any DIMMs are QR, we have to make two passes through DIMMs*/
					if (pDCTstat->DimmQRPresent == 0) {
						MaxDimms <<= 1;
					}
					if (i < DimmSlots) {
						pDCTstat->DimmQRPresent |= (1 << i) | (1 << (i+4));
					} else {
						pDCTstat->MAdimms[i & 1] --;
					}
					byte = 1;	/* upper two ranks of QR DIMM will be counted on another DIMM number iteration*/
				} else if (byte == 1) { /* 2ranks */
					pDCTstat->DimmDRPresent |= 1 << i;
				}
				bytex = devwidth;
				if (devwidth == 0)
					bytex = 16;
				else if (devwidth == 1)
					bytex = 8;
				else if (devwidth == 2)
					bytex = 4;

				byte++;		/* al+1 = rank# */
				if (byte == 2)
					bytex <<= 1;	/*double Addr bus load value for dual rank DIMMs*/

				j = i & (1<<0);
				pDCTstat->DATAload[j] += byte;	/*number of ranks on DATA bus*/
				pDCTstat->MAload[j] += bytex;	/*number of devices on CMD/ADDR bus*/
				pDCTstat->MAdimms[j]++;		/*number of DIMMs on A bus */

				/* check address mirror support for unbuffered dimm */
				/* check number of registers on a dimm for registered dimm */
				byte = pDCTstat->spd_data.spd_bytes[i][SPD_AddressMirror];
				if (RegDIMMPresent & (1 << i)) {
					if ((byte & 3) > 1)
						pDCTstat->MirrPresU_NumRegR |= 1 << i;
				} else {
					if ((byte & 1) == 1)
						pDCTstat->MirrPresU_NumRegR |= 1 << i;
				}
				/* Get byte62: Reference Raw Card information. We dont need it now. */
				/* byte = pDCTstat->spd_data.spd_bytes[i][SPD_RefRawCard]; */
				/* Get Byte65/66 for register manufacture ID code */
				if ((0x97 == pDCTstat->spd_data.spd_bytes[i][SPD_RegManufactureID_H]) &&
				    (0x80 == pDCTstat->spd_data.spd_bytes[i][SPD_RegManufactureID_L])) {
					if (0x16 == pDCTstat->spd_data.spd_bytes[i][SPD_RegManRevID])
						pDCTstat->RegMan2Present |= 1 << i;
					else
						pDCTstat->RegMan1Present |= 1 << i;
				}
				/* Get control word value for RC3 */
				byte = pDCTstat->spd_data.spd_bytes[i][70];
				pDCTstat->CtrlWrd3 |= ((byte >> 4) & 0xf) << (i << 2);	/* RC3 = SPD byte 70 [7:4] */
				/* Get control word values for RC4 and RC5 */
				byte = pDCTstat->spd_data.spd_bytes[i][71];
				pDCTstat->CtrlWrd4 |= (byte & 0xf) << (i << 2);		/* RC4 = SPD byte 71 [3:0] */
				pDCTstat->CtrlWrd5 |= ((byte >> 4) & 0xf) << (i << 2);	/* RC5 = SPD byte 71 [7:4] */
			}
		}
	}
	printk(BIOS_DEBUG, "\t DIMMPresence: DIMMValid=%x\n", pDCTstat->DIMMValid);
	printk(BIOS_DEBUG, "\t DIMMPresence: DIMMPresent=%x\n", pDCTstat->DIMMPresent);
	printk(BIOS_DEBUG, "\t DIMMPresence: RegDIMMPresent=%x\n", RegDIMMPresent);
	printk(BIOS_DEBUG, "\t DIMMPresence: LRDIMMPresent=%x\n", LRDIMMPresent);
	printk(BIOS_DEBUG, "\t DIMMPresence: DimmECCPresent=%x\n", pDCTstat->DimmECCPresent);
	printk(BIOS_DEBUG, "\t DIMMPresence: DimmPARPresent=%x\n", pDCTstat->DimmPARPresent);
	printk(BIOS_DEBUG, "\t DIMMPresence: Dimmx4Present=%x\n", pDCTstat->Dimmx4Present);
	printk(BIOS_DEBUG, "\t DIMMPresence: Dimmx8Present=%x\n", pDCTstat->Dimmx8Present);
	printk(BIOS_DEBUG, "\t DIMMPresence: Dimmx16Present=%x\n", pDCTstat->Dimmx16Present);
	printk(BIOS_DEBUG, "\t DIMMPresence: DimmPlPresent=%x\n", pDCTstat->DimmPlPresent);
	printk(BIOS_DEBUG, "\t DIMMPresence: DimmDRPresent=%x\n", pDCTstat->DimmDRPresent);
	printk(BIOS_DEBUG, "\t DIMMPresence: DimmQRPresent=%x\n", pDCTstat->DimmQRPresent);
	printk(BIOS_DEBUG, "\t DIMMPresence: DATAload[0]=%x\n", pDCTstat->DATAload[0]);
	printk(BIOS_DEBUG, "\t DIMMPresence: MAload[0]=%x\n", pDCTstat->MAload[0]);
	printk(BIOS_DEBUG, "\t DIMMPresence: MAdimms[0]=%x\n", pDCTstat->MAdimms[0]);
	printk(BIOS_DEBUG, "\t DIMMPresence: DATAload[1]=%x\n", pDCTstat->DATAload[1]);
	printk(BIOS_DEBUG, "\t DIMMPresence: MAload[1]=%x\n", pDCTstat->MAload[1]);
	printk(BIOS_DEBUG, "\t DIMMPresence: MAdimms[1]=%x\n", pDCTstat->MAdimms[1]);

	if (pDCTstat->DIMMValid != 0) {	/* If any DIMMs are present...*/
		if (RegDIMMPresent != 0) {
			if ((RegDIMMPresent ^ pDCTstat->DIMMValid) !=0) {
				/* module type DIMM mismatch (reg'ed, unbuffered) */
				pDCTstat->ErrStatus |= 1<<SB_DimmMismatchM;
				pDCTstat->ErrCode = SC_StopError;
			} else{
				/* all DIMMs are registered */
				pDCTstat->Status |= 1<<SB_Registered;
			}
		}
		if (LRDIMMPresent != 0) {
			if ((LRDIMMPresent ^ pDCTstat->DIMMValid) !=0) {
				/* module type DIMM mismatch (reg'ed, unbuffered) */
				pDCTstat->ErrStatus |= 1<<SB_DimmMismatchM;
				pDCTstat->ErrCode = SC_StopError;
			} else{
				/* all DIMMs are registered */
				pDCTstat->Status |= 1<<SB_LoadReduced;
			}
		}
		if (pDCTstat->DimmECCPresent != 0) {
			if ((pDCTstat->DimmECCPresent ^ pDCTstat->DIMMValid) == 0) {
				/* all DIMMs are ECC capable */
				pDCTstat->Status |= 1<<SB_ECCDIMMs;
			}
		}
		if (pDCTstat->DimmPARPresent != 0) {
			if ((pDCTstat->DimmPARPresent ^ pDCTstat->DIMMValid) == 0) {
				/*all DIMMs are Parity capable */
				pDCTstat->Status |= 1<<SB_PARDIMMs;
			}
		}
	} else {
		/* no DIMMs present or no DIMMs that qualified. */
		pDCTstat->ErrStatus |= 1<<SB_NoDimms;
		pDCTstat->ErrCode = SC_StopError;
	}

	printk(BIOS_DEBUG, "\t DIMMPresence: Status %x\n", pDCTstat->Status);
	printk(BIOS_DEBUG, "\t DIMMPresence: ErrStatus %x\n", pDCTstat->ErrStatus);
	printk(BIOS_DEBUG, "\t DIMMPresence: ErrCode %x\n", pDCTstat->ErrCode);
	printk(BIOS_DEBUG, "\t DIMMPresence: Done\n\n");

	mctHookAfterDIMMpre();

	return pDCTstat->ErrCode;
}

static u8 Get_DIMMAddress_D(struct DCTStatStruc *pDCTstat, u8 i)
{
	u8 *p;

	p = pDCTstat->DIMMAddr;
	/* mct_BeforeGetDIMMAddress(); */
	return p[i];
}

static void mct_preInitDCT(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat)
{
	u8 err_code;
	uint8_t nvram;
	uint8_t allow_config_restore;

	/* Preconfigure DCT0 */
	DCTPreInit_D(pMCTstat, pDCTstat, 0);

	/* Configure DCT1 if unganged and enabled*/
	if (!pDCTstat->GangedMode) {
		if (pDCTstat->DIMMValidDCT[1] > 0) {
			err_code = pDCTstat->ErrCode;		/* save DCT0 errors */
			pDCTstat->ErrCode = 0;
			DCTPreInit_D(pMCTstat, pDCTstat, 1);
			if (pDCTstat->ErrCode == 2)		/* DCT1 is not Running */
				pDCTstat->ErrCode = err_code;	/* Using DCT0 Error code to update pDCTstat.ErrCode */
		}
	}

#if IS_ENABLED(CONFIG_HAVE_ACPI_RESUME)
	calculate_and_store_spd_hashes(pMCTstat, pDCTstat);

	if (load_spd_hashes_from_nvram(pMCTstat, pDCTstat) < 0) {
		pDCTstat->spd_data.nvram_spd_match = 0;
	} else {
		compare_nvram_spd_hashes(pMCTstat, pDCTstat);
	}
#else
	pDCTstat->spd_data.nvram_spd_match = 0;
#endif

	/* Check to see if restoration of SPD data from NVRAM is allowed */
	allow_config_restore = 0;
	if (get_option(&nvram, "allow_spd_nvram_cache_restore") == CB_SUCCESS)
		allow_config_restore = !!nvram;

#if IS_ENABLED(CONFIG_HAVE_ACPI_RESUME)
	if (pMCTstat->nvram_checksum != calculate_nvram_mct_hash())
		allow_config_restore = 0;
#else
	allow_config_restore = 0;
#endif

	if (!allow_config_restore)
		pDCTstat->spd_data.nvram_spd_match = 0;
}

static void mct_initDCT(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat)
{
	u32 val;
	u8 err_code;

	/* Config. DCT0 for Ganged or unganged mode */
	DCTInit_D(pMCTstat, pDCTstat, 0);
	DCTFinalInit_D(pMCTstat, pDCTstat, 0);
	if (pDCTstat->ErrCode == SC_FatalErr) {
		/* Do nothing goto exitDCTInit; any fatal errors? */
	} else {
		/* Configure DCT1 if unganged and enabled */
		if (!pDCTstat->GangedMode) {
			if (pDCTstat->DIMMValidDCT[1] > 0) {
				err_code = pDCTstat->ErrCode;		/* save DCT0 errors */
				pDCTstat->ErrCode = 0;
				DCTInit_D(pMCTstat, pDCTstat, 1);
				DCTFinalInit_D(pMCTstat, pDCTstat, 1);
				if (pDCTstat->ErrCode == 2)		/* DCT1 is not Running */
					pDCTstat->ErrCode = err_code;	/* Using DCT0 Error code to update pDCTstat.ErrCode */
			} else {
				val = 1 << DisDramInterface;
				Set_NB32_DCT(pDCTstat->dev_dct, 1, 0x94, val);

				val = Get_NB32_DCT(pDCTstat->dev_dct, 1, 0x90);
				val &= ~(1 << ParEn);
				Set_NB32_DCT(pDCTstat->dev_dct, 1, 0x90, val);

				/* To maximize power savings when DisDramInterface = 1b,
				 * all of the MemClkDis bits should also be set.
				 */
				Set_NB32_DCT(pDCTstat->dev_dct, 1, 0x88, 0xff000000);
			}
		}
	}
}

static void mct_DramInit(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct)
{
	mct_BeforeDramInit_Prod_D(pMCTstat, pDCTstat, dct);
	mct_DramInit_Sw_D(pMCTstat, pDCTstat, dct);
	/* mct_DramInit_Hw_D(pMCTstat, pDCTstat, dct); */
}

static u8 mct_setMode(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat)
{
	u8 byte;
	u8 bytex;
	u32 val;
	u32 reg;

	byte = bytex = pDCTstat->DIMMValid;
	bytex &= 0x55;		/* CHA DIMM pop */
	pDCTstat->DIMMValidDCT[0] = bytex;

	byte &= 0xAA;		/* CHB DIMM popa */
	byte >>= 1;
	pDCTstat->DIMMValidDCT[1] = byte;

	if (byte != bytex) {
		pDCTstat->ErrStatus &= ~(1 << SB_DimmMismatchO);
	} else {
		byte = mctGet_NVbits(NV_Unganged);
		if (byte)
			pDCTstat->ErrStatus |= (1 << SB_DimmMismatchO); /* Set temp. to avoid setting of ganged mode */

		if ((!(pDCTstat->ErrStatus & (1 << SB_DimmMismatchO))) && (pDCTstat->LogicalCPUID & AMD_FAM10_ALL)) {
			/* Ganged channel mode not supported on Family 15h or higher */
			pDCTstat->GangedMode = 1;
			/* valid 128-bit mode population. */
			pDCTstat->Status |= 1 << SB_128bitmode;
			reg = 0x110;
			val = Get_NB32(pDCTstat->dev_dct, reg);
			val |= 1 << DctGangEn;
			Set_NB32(pDCTstat->dev_dct, reg, val);
		}
		if (byte)	/* NV_Unganged */
			pDCTstat->ErrStatus &= ~(1 << SB_DimmMismatchO); /* Clear so that there is no DIMM mismatch error */
	}

	return pDCTstat->ErrCode;
}

u32 Get_NB32(u32 dev, u32 reg)
{
	return pci_read_config32(dev, reg);
}

void Set_NB32(u32 dev, u32 reg, u32 val)
{
	pci_write_config32(dev, reg, val);
}


u32 Get_NB32_index(u32 dev, u32 index_reg, u32 index)
{
	u32 dword;

	Set_NB32(dev, index_reg, index);
	dword = Get_NB32(dev, index_reg+0x4);

	return dword;
}

void Set_NB32_index(u32 dev, u32 index_reg, u32 index, u32 data)
{
	Set_NB32(dev, index_reg, index);
	Set_NB32(dev, index_reg + 0x4, data);
}

u32 Get_NB32_index_wait(u32 dev, u32 index_reg, u32 index)
{
	u32 dword;

	index &= ~(1 << DctAccessWrite);
	Set_NB32(dev, index_reg, index);
	do {
		dword = Get_NB32(dev, index_reg);
	} while (!(dword & (1 << DctAccessDone)));
	dword = Get_NB32(dev, index_reg + 0x4);

	return dword;
}

void Set_NB32_index_wait(u32 dev, u32 index_reg, u32 index, u32 data)
{
	u32 dword;

	Set_NB32(dev, index_reg + 0x4, data);
	index |= (1 << DctAccessWrite);
	Set_NB32(dev, index_reg, index);
	do {
		dword = Get_NB32(dev, index_reg);
	} while (!(dword & (1 << DctAccessDone)));

}

u8 mct_BeforePlatformSpec(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct)
{
	printk(BIOS_DEBUG, "%s: Start\n", __func__);

	/* mct_checkForCxDxSupport_D */
	if (pDCTstat->LogicalCPUID & AMD_DR_GT_Bx) {
		/* Family 10h Errata 322: Address and Command Fine Delay Values May Be Incorrect */
		/* 1. Write 00000000h to F2x[1,0]9C_xD08E000 */
		Set_NB32_index_wait_DCT(pDCTstat->dev_dct, dct, 0x98, 0x0D08E000, 0);
		/* 2. If DRAM Configuration Register[MemClkFreq] (F2x[1,0]94[2:0]) is
		   greater than or equal to 011b (DDR-800 and higher),
		   then write 00000080h to F2x[1,0]9C_xD02E001,
		   else write 00000090h to F2x[1,0]9C_xD02E001. */
		if (pDCTstat->Speed >= mhz_to_memclk_config(mctGet_NVbits(NV_MIN_MEMCLK)))
			Set_NB32_index_wait_DCT(pDCTstat->dev_dct, dct, 0x98, 0x0D02E001, 0x80);
		else
			Set_NB32_index_wait_DCT(pDCTstat->dev_dct, dct, 0x98, 0x0D02E001, 0x90);
	}

	printk(BIOS_DEBUG, "%s: Done\n", __func__);

	return pDCTstat->ErrCode;
}

u8 mct_PlatformSpec(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct)
{
	/* Get platform specific config/timing values from the interface layer
	 * and program them into DCT.
	 */

	printk(BIOS_DEBUG, "%s: Start\n", __func__);

	u32 dev = pDCTstat->dev_dct;
	u32 index_reg;
	u8 i, i_start, i_end;

	if (pDCTstat->GangedMode) {
		SyncSetting(pDCTstat);
		/* mct_SetupSync_D */
		i_start = 0;
		i_end = 2;
	} else {
		i_start = dct;
		i_end = dct + 1;
	}
	for (i = i_start; i < i_end; i++) {
		index_reg = 0x98;
		Set_NB32_index_wait_DCT(dev, i, index_reg, 0x00, pDCTstat->CH_ODC_CTL[i]);	/* Channel A/B Output Driver Compensation Control */
		Set_NB32_index_wait_DCT(dev, i, index_reg, 0x04, pDCTstat->CH_ADDR_TMG[i]);	/* Channel A/B Output Driver Compensation Control */
		printk(BIOS_SPEW, "Programmed DCT %d timing/termination pattern %08x %08x\n", dct, pDCTstat->CH_ADDR_TMG[i], pDCTstat->CH_ODC_CTL[i]);
	}

	printk(BIOS_DEBUG, "%s: Done\n", __func__);

	return pDCTstat->ErrCode;
}

static void mct_SyncDCTsReady(struct DCTStatStruc *pDCTstat)
{
	u32 dev;
	u32 val;

	if (pDCTstat->NodePresent) {
		dev = pDCTstat->dev_dct;

		if ((pDCTstat->DIMMValidDCT[0]) || (pDCTstat->DIMMValidDCT[1])) {
			/* This Node has DRAM */
			do {
				val = Get_NB32(dev, 0x110);
			} while (!(val & (1 << DramEnabled)));
		}
	}	/* Node is present */
}

static void mct_AfterGetCLT(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct)
{
	if (!pDCTstat->GangedMode) {
		if (dct == 0) {
			pDCTstat->DIMMValid = pDCTstat->DIMMValidDCT[dct];
			if (pDCTstat->DIMMValidDCT[dct] == 0)
				pDCTstat->ErrCode = SC_StopError;
		} else {
			pDCTstat->CSPresent = 0;
			pDCTstat->CSTestFail = 0;
			pDCTstat->DIMMValid = pDCTstat->DIMMValidDCT[dct];
			if (pDCTstat->DIMMValidDCT[dct] == 0)
				pDCTstat->ErrCode = SC_StopError;
		}
	}
}

static u8 mct_SPDCalcWidth(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct)
{
	u8 ret;
	u32 val;

	if (dct == 0) {
		SPDCalcWidth_D(pMCTstat, pDCTstat);
		ret = mct_setMode(pMCTstat, pDCTstat);
	} else {
		ret = pDCTstat->ErrCode;
	}

	if (pDCTstat->DIMMValidDCT[0] == 0) {
		val = Get_NB32_DCT(pDCTstat->dev_dct, 0, 0x94);
		val |= 1 << DisDramInterface;
		Set_NB32_DCT(pDCTstat->dev_dct, 0, 0x94, val);

		val = Get_NB32_DCT(pDCTstat->dev_dct, 0, 0x90);
		val &= ~(1 << ParEn);
		Set_NB32_DCT(pDCTstat->dev_dct, 0, 0x90, val);
	}
	if (pDCTstat->DIMMValidDCT[1] == 0) {
		val = Get_NB32_DCT(pDCTstat->dev_dct, 1, 0x94);
		val |= 1 << DisDramInterface;
		Set_NB32_DCT(pDCTstat->dev_dct, 1, 0x94, val);

		val = Get_NB32_DCT(pDCTstat->dev_dct, 1, 0x90);
		val &= ~(1 << ParEn);
		Set_NB32_DCT(pDCTstat->dev_dct, 1, 0x90, val);
	}

	printk(BIOS_DEBUG, "SPDCalcWidth: Status %x\n", pDCTstat->Status);
	printk(BIOS_DEBUG, "SPDCalcWidth: ErrStatus %x\n", pDCTstat->ErrStatus);
	printk(BIOS_DEBUG, "SPDCalcWidth: ErrCode %x\n", pDCTstat->ErrCode);
	printk(BIOS_DEBUG, "SPDCalcWidth: Done\n");
	/* Disable dram interface before DRAM init */

	return ret;
}

static void mct_AfterStitchMemory(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct)
{
	u32 val;
	u32 dword;
	u32 dev;
	u32 reg;
	u8 _MemHoleRemap;
	u32 DramHoleBase;

	_MemHoleRemap = mctGet_NVbits(NV_MemHole);
	DramHoleBase = mctGet_NVbits(NV_BottomIO);
	DramHoleBase <<= 8;
	/* Increase hole size so;[31:24]to[31:16]
	 * it has granularity of 128MB shl eax,8
	 * Set 'effective' bottom IOmov DramHoleBase,eax
	 */
	pMCTstat->HoleBase = (DramHoleBase & 0xFFFFF800) << 8;

	/* In unganged mode, we must add DCT0 and DCT1 to DCTSysLimit */
	if (!pDCTstat->GangedMode) {
		dev = pDCTstat->dev_dct;
		pDCTstat->NodeSysLimit += pDCTstat->DCTSysLimit;
		/* if DCT0 and DCT1 both exist, set DctSelBaseAddr[47:27] to the top of DCT0 */
		if (dct == 0) {
			if (pDCTstat->DIMMValidDCT[1] > 0) {
				dword = pDCTstat->DCTSysLimit + 1;
				dword += pDCTstat->NodeSysBase;
				dword >>= 8; /* scale [39:8] to [47:27],and to F2x110[31:11] */
				if ((dword >= DramHoleBase) && _MemHoleRemap) {
					pMCTstat->HoleBase = (DramHoleBase & 0xFFFFF800) << 8;
					val = pMCTstat->HoleBase;
					val >>= 16;
					val = (((~val) & 0xFF) + 1);
					val <<= 8;
					dword += val;
				}
				reg = 0x110;
				val = Get_NB32(dev, reg);
				val &= 0x7F;
				val |= dword;
				val |= 3;  /* Set F2x110[DctSelHiRngEn], F2x110[DctSelHi] */
				Set_NB32(dev, reg, val);

				reg = 0x114;
				val = dword;
				Set_NB32(dev, reg, val);
			}
		} else {
			/* Program the DctSelBaseAddr value to 0
			   if DCT 0 is disabled */
			if (pDCTstat->DIMMValidDCT[0] == 0) {
				dword = pDCTstat->NodeSysBase;
				dword >>= 8;
				if ((dword >= DramHoleBase) && _MemHoleRemap) {
					pMCTstat->HoleBase = (DramHoleBase & 0xFFFFF800) << 8;
					val = pMCTstat->HoleBase;
					val >>= 8;
					val &= ~(0xFFFF);
					val |= (((~val) & 0xFFFF) + 1);
					dword += val;
				}
				reg = 0x114;
				val = dword;
				Set_NB32(dev, reg, val);

				reg = 0x110;
				val |= 3;	/* Set F2x110[DctSelHiRngEn], F2x110[DctSelHi] */
				Set_NB32(dev, reg, val);
			}
		}
	} else {
		pDCTstat->NodeSysLimit += pDCTstat->DCTSysLimit;
	}
	printk(BIOS_DEBUG, "AfterStitch pDCTstat->NodeSysBase = %x\n", pDCTstat->NodeSysBase);
	printk(BIOS_DEBUG, "mct_AfterStitchMemory: pDCTstat->NodeSysLimit = %x\n", pDCTstat->NodeSysLimit);
}

static u8 mct_DIMMPresence(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct)
{
	u8 ret;

	if (dct == 0)
		ret = DIMMPresence_D(pMCTstat, pDCTstat);
	else
		ret = pDCTstat->ErrCode;

	return ret;
}

/* mct_BeforeGetDIMMAddress inline in C */

static void mct_OtherTiming(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstatA)
{
	u8 Node;

	for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
		struct DCTStatStruc *pDCTstat;
		pDCTstat = pDCTstatA + Node;
		if (pDCTstat->NodePresent) {
			if (pDCTstat->DIMMValidDCT[0]) {
				pDCTstat->DIMMValid = pDCTstat->DIMMValidDCT[0];
				Set_OtherTiming(pMCTstat, pDCTstat, 0);
			}
			if (pDCTstat->DIMMValidDCT[1] && !pDCTstat->GangedMode) {
				pDCTstat->DIMMValid = pDCTstat->DIMMValidDCT[1];
				Set_OtherTiming(pMCTstat, pDCTstat, 1);
			}
		}	/* Node is present*/
	}	/* while Node */
}

static void Set_OtherTiming(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct)
{
	u32 reg;
	u32 val;
	u32 dword;
	u32 dev = pDCTstat->dev_dct;

	Get_DqsRcvEnGross_Diff(pDCTstat, dev, dct, 0x98);
	Get_WrDatGross_Diff(pDCTstat, dct, dev, 0x98);
	Get_Trdrd(pMCTstat, pDCTstat, dct);
	Get_Twrwr(pMCTstat, pDCTstat, dct);
	Get_Twrrd(pMCTstat, pDCTstat, dct);
	Get_TrwtTO(pMCTstat, pDCTstat, dct);
	Get_TrwtWB(pMCTstat, pDCTstat);

	if (!is_fam15h()) {
		reg = 0x8c;		/* Dram Timing Hi */
		val = Get_NB32_DCT(dev, dct, reg);
		val &= 0xffff0300;
		dword = pDCTstat->TrwtTO;
		val |= dword << 4;
		dword = pDCTstat->Twrrd & 3;
		val |= dword << 10;
		dword = pDCTstat->Twrwr & 3;
		val |= dword << 12;
		dword = (pDCTstat->Trdrd - 0x3) & 3;
		val |= dword << 14;
		dword = pDCTstat->TrwtWB;
		val |= dword;
		Set_NB32_DCT(dev, dct, reg, val);

		reg = 0x78;
		val = Get_NB32_DCT(dev, dct, reg);
		val &= 0xffffc0ff;
		dword = pDCTstat->Twrrd >> 2;
		val |= dword << 8;
		dword = pDCTstat->Twrwr >> 2;
		val |= dword << 10;
		dword = (pDCTstat->Trdrd - 0x3) >> 2;
		val |= dword << 12;
		Set_NB32_DCT(dev, dct, reg, val);
	}
}

static void Get_Trdrd(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, u8 dct)
{
	int8_t Trdrd;

	Trdrd = ((int8_t)(pDCTstat->DqsRcvEnGrossMax - pDCTstat->DqsRcvEnGrossMin) >> 1) + 1;
	if (Trdrd > 8)
		Trdrd = 8;
	if (Trdrd < 3)
		Trdrd = 3;
	pDCTstat->Trdrd = Trdrd;
}

static void Get_Twrwr(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, u8 dct)
{
	int8_t Twrwr = 0;

	Twrwr = ((int8_t)(pDCTstat->WrDatGrossMax - pDCTstat->WrDatGrossMin) >> 1) + 2;

	if (Twrwr < 2)
		Twrwr = 2;
	else if (Twrwr > 9)
		Twrwr = 9;

	pDCTstat->Twrwr = Twrwr;
}

static void Get_Twrrd(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, u8 dct)
{
	u8 LDplus1;
	int8_t Twrrd;

	LDplus1 = Get_Latency_Diff(pMCTstat, pDCTstat, dct);

	Twrrd = ((int8_t)(pDCTstat->WrDatGrossMax - pDCTstat->DqsRcvEnGrossMin) >> 1) + 4 - LDplus1;

	if (Twrrd < 2)
		Twrrd = 2;
	else if (Twrrd > 10)
		Twrrd = 10;
	pDCTstat->Twrrd = Twrrd;
}

static void Get_TrwtTO(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, u8 dct)
{
	u8 LDplus1;
	int8_t TrwtTO;

	LDplus1 = Get_Latency_Diff(pMCTstat, pDCTstat, dct);

	TrwtTO = ((int8_t)(pDCTstat->DqsRcvEnGrossMax - pDCTstat->WrDatGrossMin) >> 1) + LDplus1;

	pDCTstat->TrwtTO = TrwtTO;
}

static void Get_TrwtWB(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat)
{
	/* TrwtWB ensures read-to-write data-bus turnaround.
	   This value should be one more than the programmed TrwtTO.*/
	pDCTstat->TrwtWB = pDCTstat->TrwtTO;
}

static u8 Get_Latency_Diff(struct MCTStatStruc *pMCTstat,
			   struct DCTStatStruc *pDCTstat, u8 dct)
{
	u32 dev = pDCTstat->dev_dct;
	u32 val1, val2;

	val1 = Get_NB32_DCT(dev, dct, 0x88) & 0xF;
	val2 = (Get_NB32_DCT(dev, dct, 0x84) >> 20) & 7;

	return val1 - val2;
}

static void Get_DqsRcvEnGross_Diff(struct DCTStatStruc *pDCTstat,
					u32 dev, uint8_t dct, u32 index_reg)
{
	u8 Smallest, Largest;
	u32 val;
	u8 byte, bytex;

	/* The largest DqsRcvEnGrossDelay of any DIMM minus the
	   DqsRcvEnGrossDelay of any other DIMM is equal to the Critical
	   Gross Delay Difference (CGDD) */
	/* DqsRcvEn byte 1,0 */
	val = Get_DqsRcvEnGross_MaxMin(pDCTstat, dev, dct, index_reg, 0x10);
	Largest = val & 0xFF;
	Smallest = (val >> 8) & 0xFF;

	/* DqsRcvEn byte 3,2 */
	val = Get_DqsRcvEnGross_MaxMin(pDCTstat, dev, dct, index_reg, 0x11);
	byte = val & 0xFF;
	bytex = (val >> 8) & 0xFF;
	if (bytex < Smallest)
		Smallest = bytex;
	if (byte > Largest)
		Largest = byte;

	/* DqsRcvEn byte 5,4 */
	val = Get_DqsRcvEnGross_MaxMin(pDCTstat, dev, dct, index_reg, 0x20);
	byte = val & 0xFF;
	bytex = (val >> 8) & 0xFF;
	if (bytex < Smallest)
		Smallest = bytex;
	if (byte > Largest)
		Largest = byte;

	/* DqsRcvEn byte 7,6 */
	val = Get_DqsRcvEnGross_MaxMin(pDCTstat, dev, dct, index_reg, 0x21);
	byte = val & 0xFF;
	bytex = (val >> 8) & 0xFF;
	if (bytex < Smallest)
		Smallest = bytex;
	if (byte > Largest)
		Largest = byte;

	if (pDCTstat->DimmECCPresent> 0) {
		/*DqsRcvEn Ecc */
		val = Get_DqsRcvEnGross_MaxMin(pDCTstat, dev, dct, index_reg, 0x12);
		byte = val & 0xFF;
		bytex = (val >> 8) & 0xFF;
		if (bytex < Smallest)
			Smallest = bytex;
		if (byte > Largest)
			Largest = byte;
	}

	pDCTstat->DqsRcvEnGrossMax = Largest;
	pDCTstat->DqsRcvEnGrossMin = Smallest;
}

static void Get_WrDatGross_Diff(struct DCTStatStruc *pDCTstat,
					u8 dct, u32 dev, u32 index_reg)
{
	u8 Smallest = 0, Largest = 0;
	u32 val;
	u8 byte, bytex;

	/* The largest WrDatGrossDlyByte of any DIMM minus the
	  WrDatGrossDlyByte of any other DIMM is equal to CGDD */
	if (pDCTstat->DIMMValid & (1 << 0)) {
		val = Get_WrDatGross_MaxMin(pDCTstat, dct, dev, index_reg, 0x01);	/* WrDatGrossDlyByte byte 0,1,2,3 for DIMM0 */
		Largest = val & 0xFF;
		Smallest = (val >> 8) & 0xFF;
	}
	if (pDCTstat->DIMMValid & (1 << 2)) {
		val = Get_WrDatGross_MaxMin(pDCTstat, dct, dev, index_reg, 0x101);	/* WrDatGrossDlyByte byte 0,1,2,3 for DIMM1 */
		byte = val & 0xFF;
		bytex = (val >> 8) & 0xFF;
		if (bytex < Smallest)
			Smallest = bytex;
		if (byte > Largest)
			Largest = byte;
	}

	/* If Cx, 2 more dimm need to be checked to find out the largest and smallest */
	if (pDCTstat->LogicalCPUID & AMD_DR_Cx) {
		if (pDCTstat->DIMMValid & (1 << 4)) {
			val = Get_WrDatGross_MaxMin(pDCTstat, dct, dev, index_reg, 0x201);	/* WrDatGrossDlyByte byte 0,1,2,3 for DIMM2 */
			byte = val & 0xFF;
			bytex = (val >> 8) & 0xFF;
			if (bytex < Smallest)
				Smallest = bytex;
			if (byte > Largest)
				Largest = byte;
		}
		if (pDCTstat->DIMMValid & (1 << 6)) {
			val = Get_WrDatGross_MaxMin(pDCTstat, dct, dev, index_reg, 0x301);	/* WrDatGrossDlyByte byte 0,1,2,3 for DIMM2 */
			byte = val & 0xFF;
			bytex = (val >> 8) & 0xFF;
			if (bytex < Smallest)
				Smallest = bytex;
			if (byte > Largest)
				Largest = byte;
		}
	}

	pDCTstat->WrDatGrossMax = Largest;
	pDCTstat->WrDatGrossMin = Smallest;
}

static u16 Get_DqsRcvEnGross_MaxMin(struct DCTStatStruc *pDCTstat,
					u32 dev, uint8_t dct, u32 index_reg,
					u32 index)
{
	u8 Smallest, Largest;
	u8 i;
	u8 byte;
	u32 val;
	u16 word;
	u8 ecc_reg = 0;

	Smallest = 7;
	Largest = 0;

	if (index == 0x12)
		ecc_reg = 1;

	for (i = 0; i < 8; i+=2) {
		if (pDCTstat->DIMMValid & (1 << i)) {
			val = Get_NB32_index_wait_DCT(dev, dct, index_reg, index);
			val &= 0x00E000E0;
			byte = (val >> 5) & 0xFF;
			if (byte < Smallest)
				Smallest = byte;
			if (byte > Largest)
				Largest = byte;
			if (!(ecc_reg)) {
				byte = (val >> (16 + 5)) & 0xFF;
				if (byte < Smallest)
					Smallest = byte;
				if (byte > Largest)
					Largest = byte;
			}
		}
		index += 3;
	}	/* while ++i */

	word = Smallest;
	word <<= 8;
	word |= Largest;

	return word;
}

static u16 Get_WrDatGross_MaxMin(struct DCTStatStruc *pDCTstat,
					u8 dct, u32 dev, u32 index_reg,
					u32 index)
{
	u8 Smallest, Largest;
	u8 i, j;
	u32 val;
	u8 byte;
	u16 word;

	Smallest = 3;
	Largest = 0;
	for (i = 0; i < 2; i++) {
		val = Get_NB32_index_wait_DCT(dev, dct, index_reg, index);
		val &= 0x60606060;
		val >>= 5;
		for (j = 0; j < 4; j++) {
			byte = val & 0xFF;
			if (byte < Smallest)
				Smallest = byte;
			if (byte > Largest)
				Largest = byte;
			val >>= 8;
		}	/* while ++j */
		index++;
	}	/*while ++i*/

	if (pDCTstat->DimmECCPresent > 0) {
		index++;
		val = Get_NB32_index_wait_DCT(dev, dct, index_reg, index);
		val &= 0x00000060;
		val >>= 5;
		byte = val & 0xFF;
		if (byte < Smallest)
			Smallest = byte;
		if (byte > Largest)
			Largest = byte;
	}

	word = Smallest;
	word <<= 8;
	word |= Largest;

	return word;
}

static void mct_PhyController_Config(struct MCTStatStruc *pMCTstat,
				     struct DCTStatStruc *pDCTstat, u8 dct)
{
	uint8_t index;
	uint32_t dword;
	u32 index_reg = 0x98;
	u32 dev = pDCTstat->dev_dct;

	if (pDCTstat->LogicalCPUID & (AMD_DR_DAC2_OR_C3 | AMD_RB_C3 | AMD_FAM15_ALL)) {
		if (is_fam15h()) {
			/* Set F2x[1, 0]98_x0D0F0F13 DllDisEarlyU and DllDisEarlyL to save power */
			for (index = 0; index < 0x9; index++) {
				dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f0013 | (index << 8));
				dword |= (0x1 << 1);				/* DllDisEarlyU = 1 */
				dword |= 0x1;					/* DllDisEarlyL = 1 */
				Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f0013 | (index << 8), dword);
			}
		}

		if (pDCTstat->Dimmx4Present == 0) {
			/* Set bit7 RxDqsUDllPowerDown to register F2x[1, 0]98_x0D0F0F13 for
			 * additional power saving when x4 DIMMs are not present.
			 */
			for (index = 0; index < 0x9; index++) {
				dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f0013 | (index << 8));
				dword |= (0x1 << 7);				/* RxDqsUDllPowerDown = 1 */
				Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f0013 | (index << 8), dword);
			}
		}
	}

	if (pDCTstat->LogicalCPUID & (AMD_DR_DAC2_OR_C3 | AMD_FAM15_ALL)) {
		if (pDCTstat->DimmECCPresent == 0) {
			/* Set bit4 PwrDn to register F2x[1, 0]98_x0D0F0830 for power saving */
			dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f0830);
			dword |= 1 << 4; /* BIOS should set this bit if ECC DIMMs are not present */
			Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f0830, dword);
		}
	}

}

static void mct_FinalMCT_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstatA)
{
	u8 Node;
	struct DCTStatStruc *pDCTstat;
	u32 val;

	for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
		pDCTstat = pDCTstatA + Node;

		if (pDCTstat->NodePresent) {
			mct_PhyController_Config(pMCTstat, pDCTstat, 0);
			mct_PhyController_Config(pMCTstat, pDCTstat, 1);

			if (!is_fam15h()) {
				/* Family 10h CPUs */
				mct_ExtMCTConfig_Cx(pDCTstat);
				mct_ExtMCTConfig_Bx(pDCTstat);
				mct_ExtMCTConfig_Dx(pDCTstat);
			} else {
				/* Family 15h CPUs */
				uint8_t nvram;
				uint8_t enable_experimental_memory_speed_boost;

				/* Check to see if cache partitioning is allowed */
				enable_experimental_memory_speed_boost = 0;
				if (get_option(&nvram, "experimental_memory_speed_boost") == CB_SUCCESS)
					enable_experimental_memory_speed_boost = !!nvram;

				val = 0x0ce00f00;		/* FlushWrOnStpGnt = 0x0 */
				val |= 0x10 << 2;		/* MctWrLimit = 0x10 */
				val |= 0x1;			/* DctWrLimit = 0x1 */
				Set_NB32(pDCTstat->dev_dct, 0x11c, val);

				val = Get_NB32(pDCTstat->dev_dct, 0x1b0);
				val &= ~0x3;			/* AdapPrefMissRatio = 0x1 */
				val |= 0x1;
				val &= ~(0x3 << 2);		/* AdapPrefPositiveStep = 0x0 */
				val &= ~(0x3 << 4);		/* AdapPrefNegativeStep = 0x0 */
				val &= ~(0x7 << 8);		/* CohPrefPrbLmt = 0x1 */
				val |= (0x1 << 8);
				val |= (0x1 << 12);		/* EnSplitDctLimits = 0x1 */
				if (enable_experimental_memory_speed_boost)
					val |= (0x1 << 20);	/* DblPrefEn = 0x1 */
				val |= (0x7 << 22);		/* PrefFourConf = 0x7 */
				val |= (0x7 << 25);		/* PrefFiveConf = 0x7 */
				val &= ~(0xf << 28);		/* DcqBwThrotWm = 0x0 */
				Set_NB32(pDCTstat->dev_dct, 0x1b0, val);

				uint8_t wm1;
				uint8_t wm2;

				switch (pDCTstat->Speed) {
				case 0x4:
					wm1 = 0x3;
					wm2 = 0x4;
					break;
				case 0x6:
					wm1 = 0x3;
					wm2 = 0x5;
					break;
				case 0xa:
					wm1 = 0x4;
					wm2 = 0x6;
					break;
				case 0xe:
					wm1 = 0x5;
					wm2 = 0x8;
					break;
				case 0x12:
					wm1 = 0x6;
					wm2 = 0x9;
					break;
				default:
					wm1 = 0x7;
					wm2 = 0xa;
					break;
				}

				val = Get_NB32(pDCTstat->dev_dct, 0x1b4);
				val &= ~(0x3ff);
				val |= ((wm2 & 0x1f) << 5);
				val |= (wm1 & 0x1f);
				Set_NB32(pDCTstat->dev_dct, 0x1b4, val);
			}
		}
	}

	/* ClrClToNB_D postponed until we're done executing from ROM */
	mct_ClrWbEnhWsbDis_D(pMCTstat, pDCTstat);

	/* set F3x8C[DisFastTprWr] on all DR, if L3Size = 0 */
	if (pDCTstat->LogicalCPUID & AMD_DR_ALL) {
		if (!(cpuid_edx(0x80000006) & 0xFFFC0000)) {
			val = Get_NB32(pDCTstat->dev_nbmisc, 0x8C);
			val |= 1 << 24;
			Set_NB32(pDCTstat->dev_nbmisc, 0x8C, val);
		}
	}
}

void mct_ForceNBPState0_En_Fam15(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat)
{
	/* Force the NB P-state to P0 */
	uint32_t dword;
	uint32_t dword2;

	dword = Get_NB32(pDCTstat->dev_nbctl, 0x174);
	if (!(dword & 0x1)) {
		dword = Get_NB32(pDCTstat->dev_nbctl, 0x170);
		pDCTstat->SwNbPstateLoDis = (dword >> 14) & 0x1;
		pDCTstat->NbPstateDisOnP0 = (dword >> 13) & 0x1;
		pDCTstat->NbPstateThreshold = (dword >> 9) & 0x7;
		pDCTstat->NbPstateHi = (dword >> 6) & 0x3;
		dword &= ~(0x1 << 14);		/* SwNbPstateLoDis = 0 */
		dword &= ~(0x1 << 13);		/* NbPstateDisOnP0 = 0 */
		dword &= ~(0x7 << 9);		/* NbPstateThreshold = 0 */
		dword &= ~(0x3 << 3);		/* NbPstateLo = NbPstateMaxVal */
		dword |= ((dword & 0x3) << 3);
		Set_NB32(pDCTstat->dev_nbctl, 0x170, dword);

		if (!is_model10_1f()) {
			/* Wait until CurNbPState == NbPstateLo */
			do {
				dword2 = Get_NB32(pDCTstat->dev_nbctl, 0x174);
			} while (((dword2 >> 19) & 0x7) != (dword & 0x3));
		}
		dword = Get_NB32(pDCTstat->dev_nbctl, 0x170);
		dword &= ~(0x3 << 6);		/* NbPstateHi = 0 */
		dword |= (0x3 << 14);		/* SwNbPstateLoDis = 1 */
		Set_NB32(pDCTstat->dev_nbctl, 0x170, dword);

		if (!is_model10_1f()) {
			/* Wait until CurNbPState == 0 */
			do {
				dword2 = Get_NB32(pDCTstat->dev_nbctl, 0x174);
			} while (((dword2 >> 19) & 0x7) != 0);
		}
	}
}

void mct_ForceNBPState0_Dis_Fam15(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat)
{
	/* Restore normal NB P-state functionailty */
	uint32_t dword;

	dword = Get_NB32(pDCTstat->dev_nbctl, 0x174);
	if (!(dword & 0x1)) {
		dword = Get_NB32(pDCTstat->dev_nbctl, 0x170);
		dword &= ~(0x1 << 14);					/* SwNbPstateLoDis*/
		dword |= ((pDCTstat->SwNbPstateLoDis & 0x1) << 14);
		dword &= ~(0x1 << 13);					/* NbPstateDisOnP0 */
		dword |= ((pDCTstat->NbPstateDisOnP0 & 0x1) << 13);
		dword &= ~(0x7 << 9);					/* NbPstateThreshold */
		dword |= ((pDCTstat->NbPstateThreshold & 0x7) << 9);
		dword &= ~(0x3 << 6);					/* NbPstateHi */
		dword |= ((pDCTstat->NbPstateHi & 0x3) << 3);
		Set_NB32(pDCTstat->dev_nbctl, 0x170, dword);
	}
}

static void mct_InitialMCT_D(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat)
{
	if (is_fam15h()) {
		msr_t p0_state_msr;
		uint8_t cpu_fid;
		uint8_t cpu_did;
		uint32_t cpu_divisor;
		uint8_t boost_states;

		/* Retrieve the number of boost states */
		boost_states = (Get_NB32(pDCTstat->dev_link, 0x15c) >> 2) & 0x7;

		/* Retrieve and store the TSC frequency (P0 COF) */
		p0_state_msr = rdmsr(0xc0010064 + boost_states);
		cpu_fid = p0_state_msr.lo & 0x3f;
		cpu_did = (p0_state_msr.lo >> 6) & 0x7;
		cpu_divisor = (0x1 << cpu_did);
		pMCTstat->TSCFreq = (100 * (cpu_fid + 0x10)) / cpu_divisor;

		printk(BIOS_DEBUG, "mct_InitialMCT_D: mct_ForceNBPState0_En_Fam15\n");
		mct_ForceNBPState0_En_Fam15(pMCTstat, pDCTstat);
	} else {
		/* K10 BKDG v3.62 section 2.8.9.2 */
		printk(BIOS_DEBUG, "mct_InitialMCT_D: clear_legacy_Mode\n");
		clear_legacy_Mode(pMCTstat, pDCTstat);

		/* Northbridge configuration */
		mct_SetClToNB_D(pMCTstat, pDCTstat);
		mct_SetWbEnhWsbDis_D(pMCTstat, pDCTstat);
	}
}

static u32 mct_NodePresent_D(void)
{
	u32 val;
	if (is_fam15h()) {
		if (is_model10_1f()) {
			val = 0x14001022;
		} else {
			val = 0x16001022;
		}
	} else {
		val = 0x12001022;
	}
	return val;
}

static void mct_init(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat)
{
	u32 lo, hi;
	u32 addr;

	pDCTstat->GangedMode = 0;
	pDCTstat->DRPresent = 1;

	/* enable extend PCI configuration access */
	addr = 0xC001001F;
	_RDMSR(addr, &lo, &hi);
	if (hi & (1 << (46-32))) {
		pDCTstat->Status |= 1 << SB_ExtConfig;
	} else {
		hi |= 1 << (46-32);
		_WRMSR(addr, lo, hi);
	}
}

static void clear_legacy_Mode(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat)
{
	u32 reg;
	u32 val;
	u32 dev = pDCTstat->dev_dct;

	/* Clear Legacy BIOS Mode bit */
	reg = 0x94;
	val = Get_NB32_DCT(dev, 0, reg);
	val &= ~(1<<LegacyBiosMode);
	Set_NB32_DCT(dev, 0, reg, val);

	val = Get_NB32_DCT(dev, 1, reg);
	val &= ~(1<<LegacyBiosMode);
	Set_NB32_DCT(dev, 1, reg, val);
}

static void mct_HTMemMapExt(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstatA)
{
	u8 Node;
	u32 Drambase, Dramlimit;
	u32 val;
	u32 reg;
	u32 dev;
	u32 devx;
	u32 dword;
	struct DCTStatStruc *pDCTstat;

	pDCTstat = pDCTstatA + 0;
	dev = pDCTstat->dev_map;

	/* Copy dram map from F1x40/44,F1x48/4c,
	  to F1x120/124(Node0),F1x120/124(Node1),...*/
	for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
		pDCTstat = pDCTstatA + Node;
		devx = pDCTstat->dev_map;

		/* get base/limit from Node0 */
		reg = 0x40 + (Node << 3);		/* Node0/Dram Base 0 */
		val = Get_NB32(dev, reg);
		Drambase = val >> (16 + 3);

		reg = 0x44 + (Node << 3);		/* Node0/Dram Base 0 */
		val = Get_NB32(dev, reg);
		Dramlimit = val >> (16 + 3);

		/* set base/limit to F1x120/124 per Node */
		if (pDCTstat->NodePresent) {
			reg = 0x120;		/* F1x120,DramBase[47:27] */
			val = Get_NB32(devx, reg);
			val &= 0xFFE00000;
			val |= Drambase;
			Set_NB32(devx, reg, val);

			reg = 0x124;
			val = Get_NB32(devx, reg);
			val &= 0xFFE00000;
			val |= Dramlimit;
			Set_NB32(devx, reg, val);

			if (pMCTstat->GStatus & (1 << GSB_HWHole)) {
				reg = 0xF0;
				val = Get_NB32(devx, reg);
				val |= (1 << DramMemHoistValid);
				val &= ~(0xFF << 24);
				dword = (pMCTstat->HoleBase >> (24 - 8)) & 0xFF;
				dword <<= 24;
				val |= dword;
				Set_NB32(devx, reg, val);
			}

		}
	}
}

static void SetCSTriState(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct)
{
	u32 val;
	u32 dev = pDCTstat->dev_dct;
	u32 index_reg = 0x98;
	u16 word;

	if (is_fam15h()) {
		word = fam15h_cs_tristate_enable_code(pDCTstat, dct);
	} else {
		/* Tri-state unused chipselects when motherboard
		termination is available */

		/* FIXME: skip for Ax */

		word = pDCTstat->CSPresent;
		if (pDCTstat->Status & (1 << SB_Registered)) {
			word |= (word & 0x55) << 1;
		}
		word = (~word) & 0xff;
	}

	val = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0000000c);
	val &= ~0xff;
	val |= word;
	Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0000000c, val);
}

static void SetCKETriState(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct)
{
	u32 val;
	u32 dev;
	u32 index_reg = 0x98;
	u16 word;

	/* Tri-state unused CKEs when motherboard termination is available */

	/* FIXME: skip for Ax */

	dev = pDCTstat->dev_dct;
	word = pDCTstat->CSPresent;

	val = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0000000c);
	val &= ~(0x3 << 12);
	if ((word & 0x55) == 0)
		val |= 1 << 12;
	if ((word & 0xaa) == 0)
		val |= 1 << 13;
	Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0000000c, val);
}

static void SetODTTriState(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct)
{
	u32 val;
	u32 dev;
	u32 index_reg = 0x98;
	u8 cs;
	u8 odt;
	u8 max_dimms;

	dev = pDCTstat->dev_dct;

	if (is_fam15h()) {
		odt = fam15h_odt_tristate_enable_code(pDCTstat, dct);
	} else {
		/* FIXME: skip for Ax */

		/* Tri-state unused ODTs when motherboard termination is available */
		max_dimms = (u8) mctGet_NVbits(NV_MAX_DIMMS);
		odt = 0x0f;	/* ODT tri-state setting */

		if (pDCTstat->Status & (1 <<SB_Registered)) {
			for (cs = 0; cs < 8; cs += 2) {
				if (pDCTstat->CSPresent & (1 << cs)) {
					odt &= ~(1 << (cs / 2));
					if (mctGet_NVbits(NV_4RANKType) != 0) { /* quad-rank capable platform */
						if (pDCTstat->CSPresent & (1 << (cs + 1)))
							odt &= ~(4 << (cs / 2));
					}
				}
			}
		} else {		/* AM3 package */
			val = ~(pDCTstat->CSPresent);
			odt = val & 9;	/* swap bits 1 and 2 */
			if (val & (1 << 1))
				odt |= 1 << 2;
			if (val & (1 << 2))
				odt |= 1 << 1;
		}
	}

	val = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0000000c);
	val &= ~(0xf << 8);		/* ODTTri = odt */
	val |= (odt & 0xf) << 8;
	Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0000000c, val);
}

/* Family 15h */
static void InitDDRPhy(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct)
{
	uint8_t index;
	uint32_t dword;
	uint8_t ddr_voltage_index;
	uint8_t amd_voltage_level_index = 0;
	uint32_t index_reg = 0x98;
	uint32_t dev = pDCTstat->dev_dct;

	printk(BIOS_DEBUG, "%s: Start\n", __func__);

	/* Find current DDR supply voltage for this DCT */
	ddr_voltage_index = dct_ddr_voltage_index(pDCTstat, dct);

	/* Fam15h BKDG v3.14 section 2.10.5.3
	 * The remainder of the Phy Initialization algorithm picks up in phyAssistedMemFnceTraining
	 */
	Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0000000b, 0x80000000);
	Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0fe013, 0x00000118);

	/* Program desired VDDIO level */
	if (ddr_voltage_index & 0x4) {
		/* 1.25V */
		amd_voltage_level_index = 0x2;
	} else if (ddr_voltage_index & 0x2) {
		/* 1.35V */
		amd_voltage_level_index = 0x1;
	} else if (ddr_voltage_index & 0x1) {
		/* 1.50V */
		amd_voltage_level_index = 0x0;
	}

	/* D18F2x9C_x0D0F_0[F,8:0]1F_dct[1:0][RxVioLvl] */
	for (index = 0; index < 0x9; index++) {
		dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f001f | (index << 8));
		dword &= ~(0x3 << 3);
		dword |= (amd_voltage_level_index << 3);
		Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f001f | (index << 8), dword);
	}

	/* D18F2x9C_x0D0F_[C,8,2][2:0]1F_dct[1:0][RxVioLvl] */
	for (index = 0; index < 0x3; index++) {
		dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f201f | (index << 8));
		dword &= ~(0x3 << 3);
		dword |= (amd_voltage_level_index << 3);
		Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f201f | (index << 8), dword);
	}
	for (index = 0; index < 0x2; index++) {
		dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f801f | (index << 8));
		dword &= ~(0x3 << 3);
		dword |= (amd_voltage_level_index << 3);
		Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f801f | (index << 8), dword);
	}
	for (index = 0; index < 0x1; index++) {
		dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0fc01f | (index << 8));
		dword &= ~(0x3 << 3);
		dword |= (amd_voltage_level_index << 3);
		Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0fc01f | (index << 8), dword);
	}

	/* D18F2x9C_x0D0F_4009_dct[1:0][CmpVioLvl, ComparatorAdjust] */
	/* NOTE: CmpVioLvl and ComparatorAdjust only take effect when set on DCT 0 */
	dword = Get_NB32_index_wait_DCT(dev, 0, index_reg, 0x0d0f4009);
	dword &= ~(0x0000c00c);
	dword |= (amd_voltage_level_index << 14);
	dword |= (amd_voltage_level_index << 2);
	Set_NB32_index_wait_DCT(dev, 0, index_reg, 0x0d0f4009, dword);

	printk(BIOS_DEBUG, "%s: Done\n", __func__);
}

void InitPhyCompensation(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct)
{
	u8 i;
	u32 index_reg = 0x98;
	u32 dev = pDCTstat->dev_dct;
	u32 valx = 0;
	uint8_t index;
	uint32_t dword;
	const u8 *p;

	printk(BIOS_DEBUG, "%s: DCT %d: Start\n", __func__, dct);

	if (is_fam15h()) {
		/* Algorithm detailed in the Fam15h BKDG Rev. 3.14 section 2.10.5.3.4 */
		uint32_t tx_pre;
		uint32_t drive_strength;

		/* Program D18F2x9C_x0D0F_E003_dct[1:0][DisAutoComp] */
		dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0fe003);
		dword |= (0x1 << 14);
		Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0fe003, dword);

		/* Program D18F2x9C_x0D0F_E003_dct[1:0][DisablePredriverCal] */
		/* NOTE: DisablePredriverCal only takes effect when set on DCT 0 */
		dword = Get_NB32_index_wait_DCT(dev, 0, index_reg, 0x0d0fe003);
		dword |= (0x1 << 13);
		Set_NB32_index_wait_DCT(dev, 0, index_reg, 0x0d0fe003, dword);

		/* Determine TxPreP/TxPreN for data lanes (Stage 1) */
		dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x00000000);
		drive_strength = (dword >> 20) & 0x7;	/* DqsDrvStren */
		tx_pre = fam15h_phy_predriver_calibration_code(pDCTstat, dct, drive_strength);

		/* Program TxPreP/TxPreN for data lanes (Stage 1) */
		for (index = 0; index < 0x9; index++) {
			dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f0006 | (index << 8));
			dword &= ~(0xffff);
			dword |= tx_pre;
			Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f0006 | (index << 8), dword);
		}

		/* Determine TxPreP/TxPreN for data lanes (Stage 2) */
		dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x00000000);
		drive_strength = (dword >> 16) & 0x7;	/* DataDrvStren */
		tx_pre = fam15h_phy_predriver_calibration_code(pDCTstat, dct, drive_strength);

		/* Program TxPreP/TxPreN for data lanes (Stage 2) */
		for (index = 0; index < 0x9; index++) {
			dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f000a | (index << 8));
			dword &= ~(0xffff);
			dword |= tx_pre;
			Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f000a | (index << 8), dword);
		}
		for (index = 0; index < 0x9; index++) {
			dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f0002 | (index << 8));
			dword &= ~(0xffff);
			dword |= (0x8000 | tx_pre);
			Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f0002 | (index << 8), dword);
		}

		/* Determine TxPreP/TxPreN for command/address lines (Stage 1) */
		dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x00000000);
		drive_strength = (dword >> 4) & 0x7;	/* CsOdtDrvStren */
		tx_pre = fam15h_phy_predriver_cmd_addr_calibration_code(pDCTstat, dct, drive_strength);

		/* Program TxPreP/TxPreN for command/address lines (Stage 1) */
		dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f8006);
		dword &= ~(0xffff);
		dword |= tx_pre;
		Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f8006, dword);
		dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f800a);
		dword &= ~(0xffff);
		dword |= tx_pre;
		Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f800a, dword);
		dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f8002);
		dword &= ~(0xffff);
		dword |= (0x8000 | tx_pre);
		Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f8002, dword);

		/* Determine TxPreP/TxPreN for command/address lines (Stage 2) */
		dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x00000000);
		drive_strength = (dword >> 8) & 0x7;	/* AddrCmdDrvStren */
		tx_pre = fam15h_phy_predriver_cmd_addr_calibration_code(pDCTstat, dct, drive_strength);

		/* Program TxPreP/TxPreN for command/address lines (Stage 2) */
		dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f8106);
		dword &= ~(0xffff);
		dword |= tx_pre;
		Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f8106, dword);
		dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f810a);
		dword &= ~(0xffff);
		dword |= tx_pre;
		Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f810a, dword);
		dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0fc006);
		dword &= ~(0xffff);
		dword |= tx_pre;
		Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0fc006, dword);
		dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0fc00a);
		dword &= ~(0xffff);
		dword |= tx_pre;
		Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0fc00a, dword);
		dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0fc00e);
		dword &= ~(0xffff);
		dword |= tx_pre;
		Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0fc00e, dword);
		dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0fc012);
		dword &= ~(0xffff);
		dword |= tx_pre;
		Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0fc012, dword);
		dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f8102);
		dword &= ~(0xffff);
		dword |= (0x8000 | tx_pre);
		Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f8102, dword);

		/* Determine TxPreP/TxPreN for command/address lines (Stage 3) */
		dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x00000000);
		drive_strength = (dword >> 0) & 0x7;	/* CkeDrvStren */
		tx_pre = fam15h_phy_predriver_cmd_addr_calibration_code(pDCTstat, dct, drive_strength);

		/* Program TxPreP/TxPreN for command/address lines (Stage 3) */
		dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0fc002);
		dword &= ~(0xffff);
		dword |= (0x8000 | tx_pre);
		Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0fc002, dword);

		/* Determine TxPreP/TxPreN for clock lines */
		dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x00000000);
		drive_strength = (dword >> 12) & 0x7;	/* ClkDrvStren */
		tx_pre = fam15h_phy_predriver_clk_calibration_code(pDCTstat, dct, drive_strength);

		/* Program TxPreP/TxPreN for clock lines */
		dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f2002);
		dword &= ~(0xffff);
		dword |= (0x8000 | tx_pre);
		Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f2002, dword);
		dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f2102);
		dword &= ~(0xffff);
		dword |= (0x8000 | tx_pre);
		Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f2102, dword);
		dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f2202);
		dword &= ~(0xffff);
		dword |= (0x8000 | tx_pre);
		Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f2202, dword);

		if (!is_model10_1f()) {
			/* Be extra safe and wait for the predriver calibration
			 * to be applied to the hardware.  The BKDG does not
			 * require this, but it does take some time for the
			 * data to propagate, so it's probably a good idea.
			 */
			uint8_t predriver_cal_pending = 1;
			printk(BIOS_DEBUG, "Waiting for predriver calibration to be applied...");
			while (predriver_cal_pending) {
				predriver_cal_pending = 0;
				for (index = 0; index < 0x9; index++) {
					if (Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f0002 | (index << 8)) & 0x8000)
						predriver_cal_pending = 1;
				}
			}
			printk(BIOS_DEBUG, "done!\n");
		}
	} else {
		dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x00);
		dword = 0;
		for (i = 0; i < 6; i++) {
			switch (i) {
				case 0:
				case 4:
					p = Table_Comp_Rise_Slew_15x;
					valx = p[(dword >> 16) & 3];
					break;
				case 1:
				case 5:
					p = Table_Comp_Fall_Slew_15x;
					valx = p[(dword >> 16) & 3];
					break;
				case 2:
					p = Table_Comp_Rise_Slew_20x;
					valx = p[(dword >> 8) & 3];
					break;
				case 3:
					p = Table_Comp_Fall_Slew_20x;
					valx = p[(dword >> 8) & 3];
					break;
			}
			dword |= valx << (5 * i);
		}

		Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0a, dword);
	}

	printk(BIOS_DEBUG, "%s: DCT %d: Done\n", __func__, dct);
}

static void mct_EarlyArbEn_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct)
{
	printk(BIOS_DEBUG, "%s: Start\n", __func__);

	if (!is_fam15h()) {
		u32 reg;
		u32 val;
		u32 dev = pDCTstat->dev_dct;

		/* GhEnhancement #18429 modified by askar: For low NB CLK :
		* Memclk ratio, the DCT may need to arbitrate early to avoid
		* unnecessary bubbles.
		* bit 19 of F2x[1,0]78 Dram  Control Register, set this bit only when
		* NB CLK : Memclk ratio is between 3:1 (inclusive) to 4:5 (inclusive)
		*/
		reg = 0x78;
		val = Get_NB32_DCT(dev, dct, reg);

		if (pDCTstat->LogicalCPUID & (AMD_DR_Cx | AMD_DR_Dx))
			val |= (1 << EarlyArbEn);
		else if (CheckNBCOFEarlyArbEn(pMCTstat, pDCTstat))
			val |= (1 << EarlyArbEn);

		Set_NB32_DCT(dev, dct, reg, val);
	}

	printk(BIOS_DEBUG, "%s: Done\n", __func__);
}

static u8 CheckNBCOFEarlyArbEn(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat)
{
	u32 reg;
	u32 val;
	u32 tmp;
	u32 rem;
	u32 dev = pDCTstat->dev_dct;
	u32  hi, lo;
	u8 NbDid = 0;

	/* Check if NB COF >= 4*Memclk, if it is not, return a fatal error
	 */

	/* 3*(Fn2xD4[NBFid]+4)/(2^NbDid)/(3+Fn2x94[MemClkFreq]) */
	_RDMSR(0xC0010071, &lo, &hi);
	if (lo & (1 << 22))
		NbDid |= 1;

	reg = 0x94;
	val = Get_NB32_DCT(dev, 0, reg);
	if (!(val & (1 << MemClkFreqVal)))
		val = Get_NB32_DCT(dev, 1, reg);	/* get the DCT1 value */

	val &= 0x07;
	val += 3;
	if (NbDid)
		val <<= 1;
	tmp = val;

	dev = pDCTstat->dev_nbmisc;
	reg = 0xD4;
	val = Get_NB32(dev, reg);
	val &= 0x1F;
	val += 3;
	val *= 3;
	val = val / tmp;
	rem = val % tmp;
	tmp >>= 1;

	/* Yes this could be nicer but this was how the asm was.... */
	if (val < 3) {				/* NClk:MemClk < 3:1 */
		return 0;
	} else if (val > 4) {			/* NClk:MemClk >= 5:1 */
		return 0;
	} else if ((val == 4) && (rem > tmp)) { /* NClk:MemClk > 4.5:1 */
		return 0;
	} else {
		return 1;			/* 3:1 <= NClk:MemClk <= 4.5:1*/
	}
}

static void mct_ResetDataStruct_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstatA)
{
	uint8_t Node;
	struct DCTStatStruc *pDCTstat;

	/* Initialize Data structures by clearing all entries to 0 */
	memset(pMCTstat, 0, sizeof(*pMCTstat));

	for (Node = 0; Node < 8; Node++) {
		pDCTstat = pDCTstatA + Node;
		memset(pDCTstat, 0, sizeof(*pDCTstat) - sizeof(pDCTstat->persistentData));
	}
}

static void mct_BeforeDramInit_Prod_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct)
{
	printk(BIOS_DEBUG, "%s: Start\n", __func__);

	mct_ProgramODT_D(pMCTstat, pDCTstat, dct);

	printk(BIOS_DEBUG, "%s: Done\n", __func__);
}

static void mct_ProgramODT_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct)
{
	u8 i;
	u32 dword;
	u32 dev = pDCTstat->dev_dct;

	printk(BIOS_DEBUG, "%s: Start\n", __func__);

	uint8_t MaxDimmsInstallable = mctGet_NVbits(NV_MAX_DIMMS_PER_CH);

	if (is_fam15h()) {
		/* Obtain number of DIMMs on channel */
		uint8_t dimm_count = pDCTstat->MAdimms[dct];
		uint8_t rank_count_dimm0;
		uint8_t rank_count_dimm1;
		uint32_t odt_pattern_0;
		uint32_t odt_pattern_1;
		uint32_t odt_pattern_2;
		uint32_t odt_pattern_3;
		uint8_t write_odt_duration;
		uint8_t read_odt_duration;
		uint8_t write_odt_delay;
		uint8_t read_odt_delay;

		/* NOTE
		 * Rank count per DIMM and DCT is encoded by pDCTstat->DimmRanks[(<dimm number> * 2) + dct]
		 */

		/* Select appropriate ODT pattern for installed DIMMs
		 * Refer to the Fam15h BKDG Rev. 3.14, page 149 onwards
		 */
		if (pDCTstat->Status & (1 << SB_Registered)) {
			if (MaxDimmsInstallable == 2) {
				if (dimm_count == 1) {
					/* 1 DIMM detected */
					rank_count_dimm1 = pDCTstat->DimmRanks[(1 * 2) + dct];
					if (rank_count_dimm1 == 1) {
						odt_pattern_0 = 0x00000000;
						odt_pattern_1 = 0x00000000;
						odt_pattern_2 = 0x00000000;
						odt_pattern_3 = 0x00020000;
					} else if (rank_count_dimm1 == 2) {
						odt_pattern_0 = 0x00000000;
						odt_pattern_1 = 0x00000000;
						odt_pattern_2 = 0x00000000;
						odt_pattern_3 = 0x08020000;
					} else if (rank_count_dimm1 == 4) {
						odt_pattern_0 = 0x00000000;
						odt_pattern_1 = 0x00000000;
						odt_pattern_2 = 0x020a0000;
						odt_pattern_3 = 0x080a0000;
					} else {
						/* Fallback */
						odt_pattern_0 = 0x00000000;
						odt_pattern_1 = 0x00000000;
						odt_pattern_2 = 0x00000000;
						odt_pattern_3 = 0x08020000;
					}
				} else {
					/* 2 DIMMs detected */
					rank_count_dimm0 = pDCTstat->DimmRanks[(0 * 2) + dct];
					rank_count_dimm1 = pDCTstat->DimmRanks[(1 * 2) + dct];
					if ((rank_count_dimm0 < 4) && (rank_count_dimm1 < 4)) {
						odt_pattern_0 = 0x00000000;
						odt_pattern_1 = 0x01010202;
						odt_pattern_2 = 0x00000000;
						odt_pattern_3 = 0x09030603;
					} else if ((rank_count_dimm0 < 4) && (rank_count_dimm1 == 4)) {
						odt_pattern_0 = 0x01010000;
						odt_pattern_1 = 0x01010a0a;
						odt_pattern_2 = 0x01090000;
						odt_pattern_3 = 0x01030e0b;
					} else if ((rank_count_dimm0 == 4) && (rank_count_dimm1 < 4)) {
						odt_pattern_0 = 0x00000202;
						odt_pattern_1 = 0x05050202;
						odt_pattern_2 = 0x00000206;
						odt_pattern_3 = 0x0d070203;
					} else if ((rank_count_dimm0 == 4) && (rank_count_dimm1 == 4)) {
						odt_pattern_0 = 0x05050a0a;
						odt_pattern_1 = 0x05050a0a;
						odt_pattern_2 = 0x050d0a0e;
						odt_pattern_3 = 0x05070a0b;
					} else {
						/* Fallback */
						odt_pattern_0 = 0x00000000;
						odt_pattern_1 = 0x00000000;
						odt_pattern_2 = 0x00000000;
						odt_pattern_3 = 0x00000000;
					}
				}
			} else {
				/* FIXME
				 * 3 DIMMs per channel UNIMPLEMENTED
				 */
				odt_pattern_0 = 0x00000000;
				odt_pattern_1 = 0x00000000;
				odt_pattern_2 = 0x00000000;
				odt_pattern_3 = 0x00000000;
			}
		} else if (pDCTstat->Status & (1 << SB_LoadReduced)) {
			/* TODO
			 * Load reduced dimms UNIMPLEMENTED
			 */
			odt_pattern_0 = 0x00000000;
			odt_pattern_1 = 0x00000000;
			odt_pattern_2 = 0x00000000;
			odt_pattern_3 = 0x00000000;
		} else {
			if (MaxDimmsInstallable == 2) {
				if (dimm_count == 1) {
					/* 1 DIMM detected */
					rank_count_dimm1 = pDCTstat->DimmRanks[(1 * 2) + dct];
					if (rank_count_dimm1 == 1) {
						odt_pattern_0 = 0x00000000;
						odt_pattern_1 = 0x00000000;
						odt_pattern_2 = 0x00000000;
						odt_pattern_3 = 0x00020000;
					} else if (rank_count_dimm1 == 2) {
						odt_pattern_0 = 0x00000000;
						odt_pattern_1 = 0x00000000;
						odt_pattern_2 = 0x00000000;
						odt_pattern_3 = 0x08020000;
					} else {
						/* Fallback */
						odt_pattern_0 = 0x00000000;
						odt_pattern_1 = 0x00000000;
						odt_pattern_2 = 0x00000000;
						odt_pattern_3 = 0x08020000;
					}
				} else {
					/* 2 DIMMs detected */
					odt_pattern_0 = 0x00000000;
					odt_pattern_1 = 0x01010202;
					odt_pattern_2 = 0x00000000;
					odt_pattern_3 = 0x09030603;
				}
			} else {
				/* FIXME
				 * 3 DIMMs per channel UNIMPLEMENTED
				 */
				odt_pattern_0 = 0x00000000;
				odt_pattern_1 = 0x00000000;
				odt_pattern_2 = 0x00000000;
				odt_pattern_3 = 0x00000000;
			}
		}

		if (pDCTstat->Status & (1 << SB_LoadReduced)) {
			/* TODO
			 * Load reduced dimms UNIMPLEMENTED
			 */
			write_odt_duration = 0x0;
			read_odt_duration = 0x0;
			write_odt_delay = 0x0;
			read_odt_delay = 0x0;
		} else {
			uint8_t tcl;
			uint8_t tcwl;
			tcl = Get_NB32_DCT(dev, dct, 0x200) & 0x1f;
			tcwl = Get_NB32_DCT(dev, dct, 0x20c) & 0x1f;

			write_odt_duration = 0x6;
			read_odt_duration = 0x6;
			write_odt_delay = 0x0;
			if (tcl > tcwl)
				read_odt_delay = tcl - tcwl;
			else
				read_odt_delay = 0x0;
		}

		/* Program ODT pattern */
		Set_NB32_DCT(dev, dct, 0x230, odt_pattern_1);
		Set_NB32_DCT(dev, dct, 0x234, odt_pattern_0);
		Set_NB32_DCT(dev, dct, 0x238, odt_pattern_3);
		Set_NB32_DCT(dev, dct, 0x23c, odt_pattern_2);
		dword = Get_NB32_DCT(dev, dct, 0x240);
		dword &= ~(0x7 << 12);				/* WrOdtOnDuration = write_odt_duration */
		dword |= (write_odt_duration & 0x7) << 12;
		dword &= ~(0x7 << 8);				/* WrOdtTrnOnDly = write_odt_delay */
		dword |= (write_odt_delay & 0x7) << 8;
		dword &= ~(0xf << 4);				/* RdOdtOnDuration = read_odt_duration */
		dword |= (read_odt_duration & 0xf) << 4;
		dword &= ~(0xf);				/* RdOdtTrnOnDly = read_odt_delay */
		dword |= (read_odt_delay & 0xf);
		Set_NB32_DCT(dev, dct, 0x240, dword);

		printk(BIOS_SPEW, "Programmed DCT %d ODT pattern %08x %08x %08x %08x\n", dct, odt_pattern_0, odt_pattern_1, odt_pattern_2, odt_pattern_3);
	} else if (pDCTstat->LogicalCPUID & AMD_DR_Dx) {
		if (pDCTstat->Speed == 3)
			dword = 0x00000800;
		else
			dword = 0x00000000;
		for (i = 0; i < 2; i++) {
			Set_NB32_DCT(dev, i, 0x98, 0x0D000030);
			Set_NB32_DCT(dev, i, 0x9C, dword);
			Set_NB32_DCT(dev, i, 0x98, 0x4D040F30);

			/* Obtain number of DIMMs on channel */
			uint8_t dimm_count = pDCTstat->MAdimms[i];
			uint8_t rank_count_dimm0;
			uint8_t rank_count_dimm1;
			uint32_t odt_pattern_0;
			uint32_t odt_pattern_1;
			uint32_t odt_pattern_2;
			uint32_t odt_pattern_3;

			/* Select appropriate ODT pattern for installed DIMMs
			 * Refer to the Fam10h BKDG Rev. 3.62, page 120 onwards
			 */
			if (pDCTstat->Status & (1 << SB_Registered)) {
				if (MaxDimmsInstallable == 2) {
					if (dimm_count == 1) {
						/* 1 DIMM detected */
						rank_count_dimm1 = pDCTstat->DimmRanks[(1 * 2) + i];
						if (rank_count_dimm1 == 1) {
							odt_pattern_0 = 0x00000000;
							odt_pattern_1 = 0x00000000;
							odt_pattern_2 = 0x00000000;
							odt_pattern_3 = 0x00020000;
						} else if (rank_count_dimm1 == 2) {
							odt_pattern_0 = 0x00000000;
							odt_pattern_1 = 0x00000000;
							odt_pattern_2 = 0x00000000;
							odt_pattern_3 = 0x02080000;
						} else if (rank_count_dimm1 == 4) {
							odt_pattern_0 = 0x00000000;
							odt_pattern_1 = 0x00000000;
							odt_pattern_2 = 0x020a0000;
							odt_pattern_3 = 0x080a0000;
						} else {
							/* Fallback */
							odt_pattern_0 = 0x00000000;
							odt_pattern_1 = 0x00000000;
							odt_pattern_2 = 0x00000000;
							odt_pattern_3 = 0x00000000;
						}
					} else {
						/* 2 DIMMs detected */
						rank_count_dimm0 = pDCTstat->DimmRanks[(0 * 2) + i];
						rank_count_dimm1 = pDCTstat->DimmRanks[(1 * 2) + i];
						if ((rank_count_dimm0 < 4) && (rank_count_dimm1 < 4)) {
							odt_pattern_0 = 0x00000000;
							odt_pattern_1 = 0x01010202;
							odt_pattern_2 = 0x00000000;
							odt_pattern_3 = 0x09030603;
						} else if ((rank_count_dimm0 < 4) && (rank_count_dimm1 == 4)) {
							odt_pattern_0 = 0x01010000;
							odt_pattern_1 = 0x01010a0a;
							odt_pattern_2 = 0x01090000;
							odt_pattern_3 = 0x01030e0b;
						} else if ((rank_count_dimm0 == 4) && (rank_count_dimm1 < 4)) {
							odt_pattern_0 = 0x00000202;
							odt_pattern_1 = 0x05050202;
							odt_pattern_2 = 0x00000206;
							odt_pattern_3 = 0x0d070203;
						} else if ((rank_count_dimm0 == 4) && (rank_count_dimm1 == 4)) {
							odt_pattern_0 = 0x05050a0a;
							odt_pattern_1 = 0x05050a0a;
							odt_pattern_2 = 0x050d0a0e;
							odt_pattern_3 = 0x05070a0b;
						} else {
							/* Fallback */
							odt_pattern_0 = 0x00000000;
							odt_pattern_1 = 0x00000000;
							odt_pattern_2 = 0x00000000;
							odt_pattern_3 = 0x00000000;
						}
					}
				} else {
					/* FIXME
					 * 3 DIMMs per channel UNIMPLEMENTED
					 */
					odt_pattern_0 = 0x00000000;
					odt_pattern_1 = 0x00000000;
					odt_pattern_2 = 0x00000000;
					odt_pattern_3 = 0x00000000;
				}
			} else {
				if (MaxDimmsInstallable == 2) {
					if (dimm_count == 1) {
						/* 1 DIMM detected */
						rank_count_dimm1 = pDCTstat->DimmRanks[(1 * 2) + i];
						if (rank_count_dimm1 == 1) {
							odt_pattern_0 = 0x00000000;
							odt_pattern_1 = 0x00000000;
							odt_pattern_2 = 0x00000000;
							odt_pattern_3 = 0x00020000;
						} else if (rank_count_dimm1 == 2) {
							odt_pattern_0 = 0x00000000;
							odt_pattern_1 = 0x00000000;
							odt_pattern_2 = 0x00000000;
							odt_pattern_3 = 0x02080000;
						} else {
							/* Fallback */
							odt_pattern_0 = 0x00000000;
							odt_pattern_1 = 0x00000000;
							odt_pattern_2 = 0x00000000;
							odt_pattern_3 = 0x00000000;
						}
					} else {
						/* 2 DIMMs detected */
						odt_pattern_0 = 0x00000000;
						odt_pattern_1 = 0x01010202;
						odt_pattern_2 = 0x00000000;
						odt_pattern_3 = 0x09030603;
					}
				} else {
					/* FIXME
					 * 3 DIMMs per channel UNIMPLEMENTED
					 */
					odt_pattern_0 = 0x00000000;
					odt_pattern_1 = 0x00000000;
					odt_pattern_2 = 0x00000000;
					odt_pattern_3 = 0x00000000;
				}
			}

			/* Program ODT pattern */
			Set_NB32_index_wait_DCT(dev, i, 0xf0, 0x180, odt_pattern_1);
			Set_NB32_index_wait_DCT(dev, i, 0xf0, 0x181, odt_pattern_0);
			Set_NB32_index_wait_DCT(dev, i, 0xf0, 0x182, odt_pattern_3);
			Set_NB32_index_wait_DCT(dev, i, 0xf0, 0x183, odt_pattern_2);

			printk(BIOS_SPEW, "Programmed ODT pattern %08x %08x %08x %08x\n", odt_pattern_0, odt_pattern_1, odt_pattern_2, odt_pattern_3);
		}
	}

	printk(BIOS_DEBUG, "%s: Done\n", __func__);
}

static void mct_EnDllShutdownSR(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct)
{
	u32 dev = pDCTstat->dev_dct, val;

	/* Write 0000_07D0h to register F2x[1, 0]98_x4D0FE006 */
	if (pDCTstat->LogicalCPUID & (AMD_DR_DAC2_OR_C3)) {
		Set_NB32_DCT(dev, dct, 0x9C, 0x1C);
		Set_NB32_DCT(dev, dct, 0x98, 0x4D0FE006);
		Set_NB32_DCT(dev, dct, 0x9C, 0x13D);
		Set_NB32_DCT(dev, dct, 0x98, 0x4D0FE007);

		val = Get_NB32_DCT(dev, dct, 0x90);
		val &= ~(1 << 27/* DisDllShutdownSR */);
		Set_NB32_DCT(dev, dct, 0x90, val);
	}
}

static u32 mct_DisDllShutdownSR(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u32 DramConfigLo, u8 dct)
{
	u32 dev = pDCTstat->dev_dct;

	/* Write 0000_07D0h to register F2x[1, 0]98_x4D0FE006 */
	if (pDCTstat->LogicalCPUID & (AMD_DR_DAC2_OR_C3)) {
		Set_NB32_DCT(dev, dct, 0x9C, 0x7D0);
		Set_NB32_DCT(dev, dct, 0x98, 0x4D0FE006);
		Set_NB32_DCT(dev, dct, 0x9C, 0x190);
		Set_NB32_DCT(dev, dct, 0x98, 0x4D0FE007);

		DramConfigLo |=  /* DisDllShutdownSR */ 1 << 27;
	}

	return DramConfigLo;
}

void mct_SetClToNB_D(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat)
{
	u32 lo, hi;
	u32 msr;

	/* FIXME: Maybe check the CPUID? - not for now. */
	/* pDCTstat->LogicalCPUID; */

	msr = BU_CFG2;
	_RDMSR(msr, &lo, &hi);
	lo |= 1 << ClLinesToNbDis;
	_WRMSR(msr, lo, hi);
}

void mct_ClrClToNB_D(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat)
{

	u32 lo, hi;
	u32 msr;

	/* FIXME: Maybe check the CPUID? - not for now. */
	/* pDCTstat->LogicalCPUID; */

	msr = BU_CFG2;
	_RDMSR(msr, &lo, &hi);
	if (!pDCTstat->ClToNB_flag)
		lo &= ~(1<<ClLinesToNbDis);
	_WRMSR(msr, lo, hi);

}

void mct_SetWbEnhWsbDis_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat)
{
	u32 lo, hi;
	u32 msr;

	/* FIXME: Maybe check the CPUID? - not for now. */
	/* pDCTstat->LogicalCPUID; */

	msr = BU_CFG;
	_RDMSR(msr, &lo, &hi);
	hi |= (1 << WbEnhWsbDis_D);
	_WRMSR(msr, lo, hi);
}

void mct_ClrWbEnhWsbDis_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat)
{
	u32 lo, hi;
	u32 msr;

	/* FIXME: Maybe check the CPUID? - not for now. */
	/* pDCTstat->LogicalCPUID; */

	msr = BU_CFG;
	_RDMSR(msr, &lo, &hi);
	hi &= ~(1 << WbEnhWsbDis_D);
	_WRMSR(msr, lo, hi);
}

void ProgDramMRSReg_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct)
{
	u32 DramMRS, dword;
	u8 byte;

	DramMRS = 0;

	/* Set chip select CKE control mode */
	if (mctGet_NVbits(NV_CKE_CTL)) {
		if (pDCTstat->CSPresent == 3) {
			u16 word;
			word = pDCTstat->DIMMSPDCSE;
			if (dct == 0)
				word &= 0b01010100;
			else
				word &= 0b10101000;
			if (word == 0)
				DramMRS |= 1 << 23;
		}
	}

	if (is_fam15h()) {
		DramMRS |= (0x1 << 23);		/* PchgPDModeSel = 1 */
	} else {
		/*
		DRAM MRS Register
		DrvImpCtrl: drive impedance control.01b(34 ohm driver; Ron34 = Rzq/7)
		*/
		DramMRS |= 1 << 2;
		/* Dram nominal termination: */
		byte = pDCTstat->MAdimms[dct];
		if (!(pDCTstat->Status & (1 << SB_Registered))) {
			DramMRS |= 1 << 7; /* 60 ohms */
			if (byte & 2) {
				if (pDCTstat->Speed < 6)
					DramMRS |= 1 << 8; /* 40 ohms */
				else
					DramMRS |= 1 << 9; /* 30 ohms */
			}
		}
		/* Dram dynamic termination: Disable(1DIMM), 120ohm(>=2DIMM) */
		if (!(pDCTstat->Status & (1 << SB_Registered))) {
			if (byte >= 2) {
				if (pDCTstat->Speed == 7)
					DramMRS |= 1 << 10;
				else
					DramMRS |= 1 << 11;
			}
		} else {
			DramMRS |= mct_DramTermDyn_RDimm(pMCTstat, pDCTstat, byte);
		}

		/* Qoff = 0, output buffers enabled */
		/* Tcwl */
		DramMRS |= (pDCTstat->Speed - 4) << 20;
		/* ASR = 1, auto self refresh */
		/* SRT = 0 */
		DramMRS |= 1 << 18;
	}

	/* burst length control */
	if (pDCTstat->Status & (1 << SB_128bitmode))
		DramMRS |= 1 << 1;

	dword = Get_NB32_DCT(pDCTstat->dev_dct, dct, 0x84);
	if (is_fam15h()) {
		dword &= ~0x00800003;
		dword |= DramMRS;
	} else {
		dword &= ~0x00fc2f8f;
		dword |= DramMRS;
	}
	Set_NB32_DCT(pDCTstat->dev_dct, dct, 0x84, dword);
}

void mct_SetDramConfigHi_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u32 dct, u32 DramConfigHi)
{
	/* Bug#15114: Comp. update interrupted by Freq. change can cause
	 * subsequent update to be invalid during any MemClk frequency change:
	 * Solution: From the bug report:
	 *  1. A software-initiated frequency change should be wrapped into the
	 *     following sequence :
	 *	a) Disable Compensation (F2[1, 0]9C_x08[30])
	 *	b) Reset the Begin Compensation bit (D3CMP->COMP_CONFIG[0]) in
	 *	   all the compensation engines
	 *	c) Do frequency change
	 *	d) Enable Compensation (F2[1, 0]9C_x08[30])
	 *  2. A software-initiated Disable Compensation should always be
	 *     followed by step b) of the above steps.
	 * Silicon Status: Fixed In Rev B0
	 *
	 * Errata#177: DRAM Phy Automatic Compensation Updates May Be Invalid
	 * Solution: BIOS should disable the phy automatic compensation prior
	 * to initiating a memory clock frequency change as follows:
	 *  1. Disable PhyAutoComp by writing 1'b1 to F2x[1, 0]9C_x08[30]
	 *  2. Reset the Begin Compensation bits by writing 32'h0 to
	 *     F2x[1, 0]9C_x4D004F00
	 *  3. Perform frequency change
	 *  4. Enable PhyAutoComp by writing 1'b0 to F2x[1, 0]9C_08[30]
	 *  In addition, any time software disables the automatic phy
	 *   compensation it should reset the begin compensation bit per step 2.
	 *   Silicon Status: Fixed in DR-B0
	 */

	u32 dev = pDCTstat->dev_dct;
	u32 index_reg = 0x98;
	u32 index;

	uint32_t dword;

	printk(BIOS_DEBUG, "%s: Start\n", __func__);

	if (is_fam15h()) {
		/* Initial setup for frequency change
		 * 9C_x0000_0004 must be configured before MemClkFreqVal is set
		 */

		/* Program D18F2x9C_x0D0F_E006_dct[1:0][PllLockTime] = 0x190 */
		dword = Get_NB32_index_wait_DCT(pDCTstat->dev_dct, dct, index_reg, 0x0d0fe006);
		dword &= ~(0x0000ffff);
		dword |= 0x00000190;
		Set_NB32_index_wait_DCT(pDCTstat->dev_dct, dct, index_reg, 0x0d0fe006, dword);

		dword = Get_NB32_DCT(dev, dct, 0x94);
		dword &= ~(1 << MemClkFreqVal);
		Set_NB32_DCT(dev, dct, 0x94, dword);

		dword = DramConfigHi;
		dword &= ~(1 << MemClkFreqVal);
		Set_NB32_DCT(dev, dct, 0x94, dword);

		mctGet_PS_Cfg_D(pMCTstat, pDCTstat, dct);
		set_2t_configuration(pMCTstat, pDCTstat, dct);
		mct_BeforePlatformSpec(pMCTstat, pDCTstat, dct);
		mct_PlatformSpec(pMCTstat, pDCTstat, dct);
	} else {
		index = 0x08;
		dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, index);
		if (!(dword & (1 << DisAutoComp)))
			Set_NB32_index_wait_DCT(dev, dct, index_reg, index, dword | (1 << DisAutoComp));

		mct_Wait(100);
	}

	printk(BIOS_DEBUG, "mct_SetDramConfigHi_D: DramConfigHi:    %08x\n", DramConfigHi);

	/* Program the DRAM Configuration High register */
	Set_NB32_DCT(dev, dct, 0x94, DramConfigHi);

	if (is_fam15h()) {
		/* Wait until F2x[1, 0]94[FreqChgInProg]=0. */
		do {
			printk(BIOS_DEBUG, "*");
			dword = Get_NB32_DCT(pDCTstat->dev_dct, dct, 0x94);
		} while (dword & (1 << FreqChgInProg));
		printk(BIOS_DEBUG, "\n");

		/* Program D18F2x9C_x0D0F_E006_dct[1:0][PllLockTime] = 0xf */
		dword = Get_NB32_index_wait_DCT(pDCTstat->dev_dct, dct, index_reg, 0x0d0fe006);
		dword &= ~(0x0000ffff);
		dword |= 0x0000000f;
		Set_NB32_index_wait_DCT(pDCTstat->dev_dct, dct, index_reg, 0x0d0fe006, dword);
	}

	/* Clear MC4 error status */
	pci_write_config32(pDCTstat->dev_nbmisc, 0x48, 0x0);
	pci_write_config32(pDCTstat->dev_nbmisc, 0x4c, 0x0);

	printk(BIOS_DEBUG, "%s: Done\n", __func__);
}

static void mct_BeforeDQSTrain_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstatA)
{
	if (!is_fam15h()) {
		u8 Node;
		struct DCTStatStruc *pDCTstat;

		/* Errata 178
		 *
		 * Bug#15115: Uncertainty In The Sync Chain Leads To Setup Violations
		 *            In TX FIFO
		 * Solution: BIOS should program DRAM Control Register[RdPtrInit] =
		 *            5h, (F2x[1, 0]78[3:0] = 5h).
		 * Silicon Status: Fixed In Rev B0
		 *
		 * Bug#15880: Determine validity of reset settings for DDR PHY timing.
		 * Solution: At least, set WrDqs fine delay to be 0 for DDR3 training.
		 */
		for (Node = 0; Node < 8; Node++) {
			pDCTstat = pDCTstatA + Node;

			if (pDCTstat->NodePresent) {
				mct_BeforeDQSTrainSamp(pDCTstat); /* only Bx */
				mct_ResetDLL_D(pMCTstat, pDCTstat, 0);
				mct_ResetDLL_D(pMCTstat, pDCTstat, 1);
			}
		}
	}
}

/* Erratum 350 */
static void mct_ResetDLL_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct)
{
	u8 Receiver;
	u32 dev = pDCTstat->dev_dct;
	u32 addr;
	u32 lo, hi;
	u8 wrap32dis = 0;
	u8 valid = 0;

	/* Skip reset DLL for B3 */
	if (pDCTstat->LogicalCPUID & AMD_DR_B3) {
		return;
	}

	/* Skip reset DLL for Family 15h */
	if (is_fam15h()) {
		return;
	}

	addr = HWCR;
	_RDMSR(addr, &lo, &hi);
	if (lo & (1<<17)) {		/* save the old value */
		wrap32dis = 1;
	}
	lo |= (1<<17);			/* HWCR.wrap32dis */
	/* Setting wrap32dis allows 64-bit memory references in 32bit mode */
	_WRMSR(addr, lo, hi);

	pDCTstat->Channel = dct;
	Receiver = mct_InitReceiver_D(pDCTstat, dct);
	/* there are four receiver pairs, loosely associated with chipselects.*/
	for (; Receiver < 8; Receiver += 2) {
		if (mct_RcvrRankEnabled_D(pMCTstat, pDCTstat, dct, Receiver)) {
			addr = mct_GetRcvrSysAddr_D(pMCTstat, pDCTstat, dct, Receiver, &valid);
			if (valid) {
				mct_Read1LTestPattern_D(pMCTstat, pDCTstat, addr);	/* cache fills */

				/* Write 0000_8000h to register F2x[1,0]9C_xD080F0C */
				Set_NB32_index_wait_DCT(dev, dct, 0x98, 0xD080F0C, 0x00008000);
				mct_Wait(80); /* wait >= 300ns */

				/* Write 0000_0000h to register F2x[1,0]9C_xD080F0C */
				Set_NB32_index_wait_DCT(dev, dct, 0x98, 0xD080F0C, 0x00000000);
				mct_Wait(800); /* wait >= 2us */
				break;
			}
		}
	}

	if (!wrap32dis) {
		addr = HWCR;
		_RDMSR(addr, &lo, &hi);
		lo &= ~(1<<17);		/* restore HWCR.wrap32dis */
		_WRMSR(addr, lo, hi);
	}
}

void mct_EnableDatIntlv_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat)
{
	u32 dev = pDCTstat->dev_dct;
	u32 val;

	/*  Enable F2x110[DctDatIntlv] */
	/* Call back not required mctHookBeforeDatIntlv_D() */
	/* FIXME Skip for Ax */
	if (!pDCTstat->GangedMode) {
		val = Get_NB32(dev, 0x110);
		val |= 1 << 5;			/* DctDatIntlv */
		Set_NB32(dev, 0x110, val);

		/* FIXME Skip for Cx */
		dev = pDCTstat->dev_nbmisc;
		val = Get_NB32(dev, 0x8C);	/* NB Configuration Hi */
		val |= 1 << (36-32);		/* DisDatMask */
		Set_NB32(dev, 0x8C, val);
	}
}

void SetDllSpeedUp_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct)
{
	if (!is_fam15h()) {
		u32 val;
		u32 dev = pDCTstat->dev_dct;

		if (pDCTstat->Speed >= mhz_to_memclk_config(800)) { /* DDR1600 and above */
			/* Set bit13 PowerDown to register F2x[1, 0]98_x0D080F10 */
			Set_NB32_DCT(dev, dct, 0x98, 0x0D080F10);
			val = Get_NB32_DCT(dev, dct, 0x9C);
			val |= 1 < 13;
			Set_NB32_DCT(dev, dct, 0x9C, val);
			Set_NB32_DCT(dev, dct, 0x98, 0x4D080F10);

			/* Set bit13 PowerDown to register F2x[1, 0]98_x0D080F11 */
			Set_NB32_DCT(dev, dct, 0x98, 0x0D080F11);
			val = Get_NB32_DCT(dev, dct, 0x9C);
			val |= 1 < 13;
			Set_NB32_DCT(dev, dct, 0x9C, val);
			Set_NB32_DCT(dev, dct, 0x98, 0x4D080F11);

			/* Set bit13 PowerDown to register F2x[1, 0]98_x0D088F30 */
			Set_NB32_DCT(dev, dct, 0x98, 0x0D088F30);
			val = Get_NB32_DCT(dev, dct, 0x9C);
			val |= 1 < 13;
			Set_NB32_DCT(dev, dct, 0x9C, val);
			Set_NB32_DCT(dev, dct, 0x98, 0x4D088F30);

			/* Set bit13 PowerDown to register F2x[1, 0]98_x0D08CF30 */
			Set_NB32_DCT(dev, dct, 0x98, 0x0D08CF30);
			val = Get_NB32_DCT(dev, dct, 0x9C);
			val |= 1 < 13;
			Set_NB32_DCT(dev, dct, 0x9C, val);
			Set_NB32_DCT(dev, dct, 0x98, 0x4D08CF30);
		}
	}
}

static void SyncSetting(struct DCTStatStruc *pDCTstat)
{
	/* set F2x78[ChSetupSync] when F2x[1, 0]9C_x04[AddrCmdSetup, CsOdtSetup,
	 * CkeSetup] setups for one DCT are all 0s and at least one of the setups,
	 * F2x[1, 0]9C_x04[AddrCmdSetup, CsOdtSetup, CkeSetup], of the other
	 * controller is 1
	 */
	u32 cha, chb;
	u32 dev = pDCTstat->dev_dct;
	u32 val;

	cha = pDCTstat->CH_ADDR_TMG[0] & 0x0202020;
	chb = pDCTstat->CH_ADDR_TMG[1] & 0x0202020;

	if ((cha != chb) && ((cha == 0) || (chb == 0))) {
		val = Get_NB32(dev, 0x78);
		val |= 1 << ChSetupSync;
		Set_NB32(dev, 0x78, val);
	}
}

static void AfterDramInit_D(struct DCTStatStruc *pDCTstat, u8 dct) {

	u32 val;
	u32 dev = pDCTstat->dev_dct;

	if (pDCTstat->LogicalCPUID & (AMD_DR_B2 | AMD_DR_B3)) {
		mct_Wait(10000);	/* Wait 50 us*/
		val = Get_NB32(dev, 0x110);
		if (!(val & (1 << DramEnabled))) {
			/* If 50 us expires while DramEnable =0 then do the following */
			val = Get_NB32_DCT(dev, dct, 0x90);
			val &= ~(1 << Width128);		/* Program Width128 = 0 */
			Set_NB32_DCT(dev, dct, 0x90, val);

			val = Get_NB32_index_wait_DCT(dev, dct, 0x98, 0x05);	/* Perform dummy CSR read to F2x09C_x05 */

			if (pDCTstat->GangedMode) {
				val = Get_NB32_DCT(dev, dct, 0x90);
				val |= 1 << Width128;		/* Program Width128 = 0 */
				Set_NB32_DCT(dev, dct, 0x90, val);
			}
		}
	}
}

/* ==========================================================
 *  6-bit Bank Addressing Table
 *  RR = rows-13 binary
 *  B = Banks-2 binary
 *  CCC = Columns-9 binary
 * ==========================================================
 *  DCT	CCCBRR	Rows	Banks	Columns	64-bit CS Size
 *  Encoding
 *  0000	000000	13	2	9	128MB
 *  0001	001000	13	2	10	256MB
 *  0010	001001	14	2	10	512MB
 *  0011	010000	13	2	11	512MB
 *  0100	001100	13	3	10	512MB
 *  0101	001101	14	3	10	1GB
 *  0110	010001	14	2	11	1GB
 *  0111	001110	15	3	10	2GB
 *  1000	010101	14	3	11	2GB
 *  1001	010110	15	3	11	4GB
 *  1010	001111	16	3	10	4GB
 *  1011	010111	16	3	11	8GB
 */
uint8_t crcCheck(struct DCTStatStruc *pDCTstat, uint8_t dimm)
{
	u16 crc_calc = spd_ddr3_calc_crc(pDCTstat->spd_data.spd_bytes[dimm],
		sizeof(pDCTstat->spd_data.spd_bytes[dimm]));
	u16 checksum_spd = pDCTstat->spd_data.spd_bytes[dimm][SPD_byte_127] << 8
		| pDCTstat->spd_data.spd_bytes[dimm][SPD_byte_126];

	return crc_calc == checksum_spd;
}

int32_t abs(int32_t val)
{
	if (val < 0)
		return -val;
	return val;
}
