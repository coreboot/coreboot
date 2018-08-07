/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
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
 */

/* Call-backs */
#include <delay.h>
#include <cpu/amd/msr.h>
#include <console/console.h>
#include "mcti.h"

#define NVRAM_DDR2_800 0
#define NVRAM_DDR2_667 1
#define NVRAM_DDR2_533 2
#define NVRAM_DDR2_400 3

#define NVRAM_DDR3_1600 0
#define NVRAM_DDR3_1333 1
#define NVRAM_DDR3_1066 2
#define NVRAM_DDR3_800  3

static inline uint8_t isfam15h(void)
{
	uint8_t fam15h = 0;
	uint32_t family;

	family = cpuid_eax(0x80000001);
	family = ((family & 0xf00000) >> 16) | ((family & 0xf00) >> 8);

	if (family >= 0x6f)
		/* Family 15h or later */
		fam15h = 1;

	return fam15h;
}

/* The recommended maximum GFX Upper Memory Area
 * size is 256M, however, to be on the safe side
 * move TOM down by 512M.
 */
#define MAXIMUM_GFXUMA_SIZE 0x20000000

/* Do not allow less than 16M of DRAM in 32-bit space.
 * This number is not hardware constrained and can be
 * changed as needed.
 */
#define MINIMUM_DRAM_BELOW_4G 0x1000000

u16 mctGet_NVbits(u8 index)
{
	u16 val = 0;
	int nvram;

	switch (index) {
	case NV_PACK_TYPE:
#if CONFIG_CPU_SOCKET_TYPE == 0x10	/* Socket F */
		val = 0;
#elif CONFIG_CPU_SOCKET_TYPE == 0x11	/* AM3 */
		val = 1;
#elif CONFIG_CPU_SOCKET_TYPE == 0x13	/* ASB2 */
		val = 4;
#elif CONFIG_CPU_SOCKET_TYPE == 0x14	/* C32 */
		val = 5;
#elif CONFIG_CPU_SOCKET_TYPE == 0x15	/* G34 */
		val = 3;
#elif CONFIG_CPU_SOCKET_TYPE == 0x16	/* FM2 */
		val = 6;
//#elif SYSTEM_TYPE == MOBILE
//		val = 2;
#endif
		break;
	case NV_MAX_NODES:
		val = MAX_NODES_SUPPORTED;
		break;
	case NV_MAX_DIMMS:
		val = MAX_DIMMS_SUPPORTED;
		//val = 8;
		break;
	case NV_MAX_DIMMS_PER_CH:
		/* FIXME
		 * Mainboards need to be able to specify the maximum number of DIMMs installable per channel
		 * For now assume a maximum of 2 DIMMs per channel can be installed
		 */
		val = 2;
		break;
	case NV_MAX_MEMCLK:
		/* Maximum platform supported memclk */
		val =  MEM_MAX_LOAD_FREQ;

		if (get_option(&nvram, "max_mem_clock") == CB_SUCCESS) {
			int limit = val;
			if (IS_ENABLED(CONFIG_DIMM_DDR3))
				limit = ddr3_limits[nvram & 0xf];
			else if (IS_ENABLED(CONFIG_DIMM_DDR2))
				limit = ddr2_limits[nvram & 0x3];
			val = min(limit, val);
		}
		break;
	case NV_MIN_MEMCLK:
		/* Minimum platform supported memclk */
		if (isfam15h())
			val =  MEM_MIN_PLATFORM_FREQ_FAM15;
		else
			val =  MEM_MIN_PLATFORM_FREQ_FAM10;
		break;
	case NV_ECC_CAP:
#if SYSTEM_TYPE == SERVER
		val = 1;	/* memory bus ECC capable */
#else
		val = 0;	/* memory bus ECC not capable */
#endif
		break;
	case NV_4RANKType:
		/* Quad Rank DIMM slot type */
		val = 0;	/* normal */
		//val = 1;	/* R4 (registered DIMMs in AMD server configuration) */
		//val = 2;	/* S4 (Unbuffered SO-DIMMS) */
		break;
	case NV_BYPMAX:
#if !IS_ENABLED(CONFIG_GFXUMA)
		val = 4;
#elif IS_ENABLED(CONFIG_GFXUMA)
		val = 7;
#endif
		break;
	case NV_RDWRQBYP:
#if !IS_ENABLED(CONFIG_GFXUMA)
		val = 2;
#elif IS_ENABLED(CONFIG_GFXUMA)
		val = 3;
#endif
		break;
	case NV_MCTUSRTMGMODE:
		val = 0;	/* Automatic (recommended) */
		//val = 1;	/* Limited */
		//val = 2;	/* Manual */
		break;
	case NV_MemCkVal:
		//val = 0;	/* 200MHz */
		//val = 1;	/* 266MHz */
		val = 2;	/* 333MHz */
		break;
	case NV_BankIntlv:
		/* Bank (chip select) interleaving */
		//val = 0;	/* disabled */
		val = 1;	/* enabled (recommended) */

		if (get_option(&nvram, "interleave_chip_selects") == CB_SUCCESS)
			val = !!nvram;
		break;
	case NV_MemHole:
		//val = 0;	/* Disabled */
		val = 1;	/* Enabled (recommended) */
		break;
	case NV_AllMemClks:
		val = 0;	/* Normal (only to slots that have enabled DIMMs) */
		//val = 1;	/* Enable all memclocks */
		break;
	case NV_SPDCHK_RESTRT:
		val = 0;	/* Exit current node initialization if any DIMM has SPD checksum error */
		//val = 1;	/* Ignore faulty SPD checksum (DIMM will still be disabled), continue current node initialization */
		//val = 2;	/* Override faulty SPD checksum (DIMM will be enabled), continue current node intialization */

		if (get_option(&nvram, "dimm_spd_checksum") == CB_SUCCESS)
			val = nvram & 0x3;

		if (val > 2)
			val = 2;

		break;
	case NV_DQSTrainCTL:
		//val = 0;	/*Skip dqs training */
		val = 1;	/* Perform dqs training */
		break;
	case NV_NodeIntlv:
		val = 0;	/* Disabled (recommended) */
		//val = 1;	/* Enable */

		if (get_option(&nvram, "interleave_nodes") == CB_SUCCESS)
			val = !!nvram;
		break;
	case NV_BurstLen32:
#if !IS_ENABLED(CONFIG_GFXUMA)
		val = 0;	/* 64 byte mode */
#elif IS_ENABLED(CONFIG_GFXUMA)
		val = 1;	/* 32 byte mode */
#endif
		break;
	case NV_CKE_PDEN:
		//val = 0;	/* Disable */
		val = 1;	/* Enable */
		break;
	case NV_CKE_CTL:
		val = 0;	/* per channel control */
		//val = 1;	/* per chip select control */
		break;
	case NV_CLKHZAltVidC3:
		val = 0;	/* disable */
		//val = 1;	/* enable */
		break;
	case NV_BottomIO:
	case NV_BottomUMA:
		/* address bits [31:24] */
#if !IS_ENABLED(CONFIG_GFXUMA)
		val = (CONFIG_MMCONF_BASE_ADDRESS >> 24);
#elif IS_ENABLED(CONFIG_GFXUMA)
  #if (CONFIG_MMCONF_BASE_ADDRESS < (MAXIMUM_GFXUMA_SIZE + MINIMUM_DRAM_BELOW_4G))
  #error "MMCONF_BASE_ADDRESS is too small"
  #endif
		val = ((CONFIG_MMCONF_BASE_ADDRESS - MAXIMUM_GFXUMA_SIZE) >> 24);
#endif
		break;
	case NV_ECC:
#if (SYSTEM_TYPE == SERVER)
		val = 1;	/* Enable */
#else
		val = 0;	/* Disable */
#endif

		if (get_option(&nvram, "ECC_memory") == CB_SUCCESS)
			val = !!nvram;
		break;
	case NV_NBECC:
#if (SYSTEM_TYPE == SERVER)
		val = 1;	/* Enable */
#else
		val = 0;	/* Disable */
#endif
		break;
	case NV_ChipKill:
#if (SYSTEM_TYPE == SERVER)
		val = 1;	/* Enable */
#else
		val = 0;	/* Disable */
#endif
		break;
	case NV_ECCRedir:
		/*
		 * 0: Disable
		 * 1: Enable
		 */
		val = 0;

		if (get_option(&nvram, "ECC_redirection") == CB_SUCCESS)
			val = !!nvram;
		break;
	case NV_DramBKScrub:
		/*
		 * 0x00: Disabled
		 * 0x01: 40ns
		 * 0x02: 80ns
		 * 0x03: 160ns
		 * 0x04: 320ns
		 * 0x05: 640ns
		 * 0x06: 1.28us
		 * 0x07: 2.56us
		 * 0x08: 5.12us
		 * 0x09: 10.2us
		 * 0x0a: 20.5us
		 * 0x0b: 41us
		 * 0x0c: 81.9us
		 * 0x0d: 163.8us
		 * 0x0e: 327.7us
		 * 0x0f: 655.4us
		 * 0x10: 1.31ms
		 * 0x11: 2.62ms
		 * 0x12: 5.24ms
		 * 0x13: 10.49ms
		 * 0x14: 20.97sms
		 * 0x15: 42ms
		 * 0x16: 84ms
		 */
		val = 0;

		if ((get_option(&nvram, "ecc_scrub_rate") == CB_SUCCESS) && (nvram <= 0x16))
			val = nvram;
		break;
	case NV_L2BKScrub:
		val = 0;	/* Disabled - See L2Scrub in BKDG */
		break;
	case NV_L3BKScrub:
		val = 0;	/* Disabled - See L3Scrub in BKDG */
		break;
	case NV_DCBKScrub:
		val = 0;	/* Disabled - See DcacheScrub in BKDG */
		break;
	case NV_CS_SpareCTL:
		val = 0;	/* Disabled */
		//val = 1;	/* Enabled */
		break;
	case NV_SyncOnUnEccEn:
		val = 0;	/* Disabled */
		//val = 1;	/* Enabled */
		break;
	case NV_Unganged:
		/* channel interleave is better performance than ganged mode at this time */
		val = 1;		/* Enabled */
		//val = 0;	/* Disabled */

		if (get_option(&nvram, "interleave_memory_channels") == CB_SUCCESS)
			val = !!nvram;
		break;
	case NV_ChannelIntlv:
		val = 5;	/* Not currently checked in mctchi_d.c */
	/* Bit 0 =     0 - Disable
	 *             1 - Enable
	 * Bits[2:1] = 00b - Address bits 6
	 *             01b - Address bits 1
	 *             10b - Hash*, XOR of address bits [20:16, 6]
	 *             11b - Hash*, XOR of address bits [20:16, 9]
	 */
		break;
	}

	return val;
}


void mctHookAfterDIMMpre(void)
{
}


void mctGet_MaxLoadFreq(struct DCTStatStruc *pDCTstat)
{
	pDCTstat->PresetmaxFreq = mctGet_NVbits(NV_MAX_MEMCLK);

	/* Determine the number of installed DIMMs */
	int ch1_count = 0;
	int ch2_count = 0;
	uint8_t ch1_registered = 0;
	uint8_t ch2_registered = 0;
	uint8_t ch1_voltage = 0;
	uint8_t ch2_voltage = 0;
	uint8_t highest_rank_count[2];
	uint8_t dimm;
	int i;
	for (i = 0; i < 15; i = i + 2) {
		if (pDCTstat->DIMMValid & (1 << i))
			ch1_count++;
		if (pDCTstat->DIMMValid & (1 << (i + 1)))
			ch2_count++;
	}
	for (i = 0; i < MAX_DIMMS_SUPPORTED; i = i + 2) {
		if (pDCTstat->DimmRegistered[i])
			ch1_registered = 1;
		if (pDCTstat->DimmRegistered[i + 1])
			ch2_registered = 1;
	}
	if (IS_ENABLED(CONFIG_DEBUG_RAM_SETUP)) {
		printk(BIOS_DEBUG, "mctGet_MaxLoadFreq: Channel 1: %d DIMM(s) detected\n", ch1_count);
		printk(BIOS_DEBUG, "mctGet_MaxLoadFreq: Channel 2: %d DIMM(s) detected\n", ch2_count);
	}

#if IS_ENABLED(CONFIG_DIMM_DDR3)
	for (i = 0; i < MAX_DIMMS_SUPPORTED; i = i + 2) {
		if (pDCTstat->DIMMValid & (1 << i))
			ch1_voltage |= pDCTstat->DimmConfiguredVoltage[i];
		if (pDCTstat->DIMMValid & (1 << (i + 1)))
			ch2_voltage |= pDCTstat->DimmConfiguredVoltage[i + 1];
	}
#endif

	for (i = 0; i < 2; i++) {
		highest_rank_count[i] = 0x0;
		for (dimm = 0; dimm < MAX_DIMMS_SUPPORTED; dimm++) {
			if (pDCTstat->DimmRanks[dimm] > highest_rank_count[i])
				highest_rank_count[i] = pDCTstat->DimmRanks[dimm];
		}
	}

	/* Set limits if needed */
	pDCTstat->PresetmaxFreq = mct_MaxLoadFreq(max(ch1_count, ch2_count), max(highest_rank_count[0], highest_rank_count[1]), (ch1_registered || ch2_registered), (ch1_voltage | ch2_voltage), pDCTstat->PresetmaxFreq);
}

void mctAdjustAutoCycTmg_D(void)
{
}


void mctHookAfterAutoCycTmg(void)
{
}


void mctGetCS_ExcludeMap(void)
{
}


void mctHookAfterAutoCfg(void)
{
}


void mctHookAfterPSCfg(void)
{
}


void mctHookAfterHTMap(void)
{
}


void mctHookAfterCPU(void)
{
}


#if IS_ENABLED(CONFIG_DIMM_DDR2)
void mctSaveDQSSigTmg_D(void)
{
}

void mctGetDQSSigTmg_D(void)
{
}
#endif

void mctHookBeforeECC(void)
{
}

void mctHookAfterECC(void)
{
}

#ifdef UNUSED_CODE
void mctInitMemGPIOs_A(void)
{
}
#endif


void mctInitMemGPIOs_A_D(void)
{
}


void mctNodeIDDebugPort_D(void)
{
}


void mctWarmReset_D(void)
{
}


void mctHookBeforeDramInit(void)
{
}


void mctHookAfterDramInit(void)
{
}

#if IS_ENABLED(CONFIG_DIMM_DDR3)
void vErratum372(struct DCTStatStruc *pDCTstat)
{
	msr_t msr = rdmsr(NB_CFG_MSR);

	int nbPstate1supported = !(msr.hi & (1 << (NB_GfxNbPstateDis -32)));

	// is this the right way to check for NB pstate 1 or DDR3-1333 ?
	if (((pDCTstat->PresetmaxFreq == 1333)||(nbPstate1supported))
	    && (!pDCTstat->GangedMode)) {
		/* DisableCf8ExtCfg */
		msr.hi &= ~(3 << (51 - 32));
		wrmsr(NB_CFG_MSR, msr);
	}
}

void vErratum414(struct DCTStatStruc *pDCTstat)
{
	int dct = 0;
	for (; dct < 2 ; dct++) {
		int dRAMConfigHi = Get_NB32(pDCTstat->dev_dct,0x94 + (0x100 * dct));
		int powerDown =  dRAMConfigHi & (1 << PowerDownEn);
		int ddr3 = dRAMConfigHi & (1 << Ddr3Mode);
		int dRAMMRS = Get_NB32(pDCTstat->dev_dct,0x84 + (0x100 * dct));
		int pchgPDModeSel = dRAMMRS & (1 << PchgPDModeSel);
		if (powerDown && ddr3 && pchgPDModeSel)
			Set_NB32(pDCTstat->dev_dct,0x84 + (0x100 * dct), dRAMMRS & ~(1 << PchgPDModeSel));
	}
}
#endif


void mctHookBeforeAnyTraining(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstatA)
{
#if IS_ENABLED(CONFIG_DIMM_DDR3)
  /* FIXME :  as of 25.6.2010 errata 350 and 372 should apply to  ((RB|BL|DA)-C[23])|(HY-D[01])|(PH-E0) but I don't find constants for all of them */
	if (pDCTstatA->LogicalCPUID & (AMD_DRBH_Cx | AMD_DR_Dx)) {
		vErratum372(pDCTstatA);
		vErratum414(pDCTstatA);
	}
#endif
}

#if IS_ENABLED(CONFIG_DIMM_DDR3)
u32 mct_AdjustSPDTimings(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstatA, u32 val)
{
	if (pDCTstatA->LogicalCPUID & AMD_DR_Bx) {
		if (pDCTstatA->Status & (1 << SB_Registered)) {
			val ++;
		}
	}
	return val;
}
#endif

void mctHookAfterAnyTraining(void)
{
}

uint64_t mctGetLogicalCPUID_D(u8 node)
{
	return mctGetLogicalCPUID(node);
}

#if IS_ENABLED(CONFIG_DIMM_DDR2)
u8 mctSetNodeBoundary_D(void)
{
	return 0;
}
#endif
