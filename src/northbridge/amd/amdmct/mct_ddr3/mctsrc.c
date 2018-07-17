/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
 * Copyright (C) 2015 - 2016 Raptor Engineering, LLC
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

/******************************************************************************
 Description: Receiver En and DQS Timing Training feature for DDR 3 MCT
******************************************************************************/

#include <inttypes.h>
#include <console/console.h>
#include <string.h>
#include "mct_d.h"
#include "mct_d_gcc.h"
#include <cpu/x86/msr.h>

static void dqsTrainRcvrEn_SW_Fam10(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 Pass);
static void dqsTrainRcvrEn_SW_Fam15(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 Pass);
static void mct_InitDQSPos4RcvrEn_D(struct MCTStatStruc *pMCTstat,
					 struct DCTStatStruc *pDCTstat);
static void InitDQSPos4RcvrEn_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 Channel);
static void CalcEccDQSRcvrEn_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 Channel);
static void mct_SetMaxLatency_D(struct DCTStatStruc *pDCTstat, u8 Channel, u16 DQSRcvEnDly);
static void mct_DisableDQSRcvEn_D(struct DCTStatStruc *pDCTstat);

static uint8_t is_fam15h(void)
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

/* Warning:  These must be located so they do not cross a logical 16-bit
   segment boundary! */
const u32 TestPattern0_D[] = {
	0x55555555, 0x55555555, 0x55555555, 0x55555555,
	0x55555555, 0x55555555, 0x55555555, 0x55555555,
	0x55555555, 0x55555555, 0x55555555, 0x55555555,
	0x55555555, 0x55555555, 0x55555555, 0x55555555,
};
const u32 TestPattern1_D[] = {
	0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa,
	0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa,
	0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa,
	0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa,
};
const u32 TestPattern2_D[] = {
	0x12345678, 0x87654321, 0x23456789, 0x98765432,
	0x59385824, 0x30496724, 0x24490795, 0x99938733,
	0x40385642, 0x38465245, 0x29432163, 0x05067894,
	0x12349045, 0x98723467, 0x12387634, 0x34587623,
};

static void SetupRcvrPattern(struct MCTStatStruc *pMCTstat,
		struct DCTStatStruc *pDCTstat, u32 *buffer, u8 pass)
{
	/*
	 * 1. Copy the alpha and Beta patterns from ROM to Cache,
	 *     aligning on 16 byte boundary
	 * 2. Set the ptr to DCTStatstruc.PtrPatternBufA for Alpha
	 * 3. Set the ptr to DCTStatstruc.PtrPatternBufB for Beta
	 */
	u32 *buf_a;
	u32 *buf_b;
	u32 *p_A;
	u32 *p_B;
	u8 i;

	buf_a = (u32 *)(((u32)buffer + 0x10) & (0xfffffff0));
	buf_b = buf_a + 32; /* ?? */
	p_A = (u32 *)SetupDqsPattern_1PassB(pass);
	p_B = (u32 *)SetupDqsPattern_1PassA(pass);

	for (i = 0; i < 16; i++) {
		buf_a[i] = p_A[i];
		buf_b[i] = p_B[i];
	}

	pDCTstat->PtrPatternBufA = (u32)buf_a;
	pDCTstat->PtrPatternBufB = (u32)buf_b;
}

void mct_TrainRcvrEn_D(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, u8 Pass)
{
	if (mct_checkNumberOfDqsRcvEn_1Pass(Pass)) {
		if (is_fam15h())
			dqsTrainRcvrEn_SW_Fam15(pMCTstat, pDCTstat, Pass);
		else
			dqsTrainRcvrEn_SW_Fam10(pMCTstat, pDCTstat, Pass);
	}
}

static uint16_t fam15_receiver_enable_training_seed(struct DCTStatStruc *pDCTstat, uint8_t dct, uint8_t dimm, uint8_t rank, uint8_t package_type)
{
	uint32_t dword;
	uint16_t seed = 0;

	uint8_t MaxDimmsInstallable = mctGet_NVbits(NV_MAX_DIMMS_PER_CH);

	uint8_t channel = dct;
	if (package_type == PT_GR) {
		/* Get the internal node number */
		dword = Get_NB32(pDCTstat->dev_nbmisc, 0xe8);
		dword = (dword >> 30) & 0x3;
		if (dword == 1) {
			channel += 2;
		}
	}

	if (pDCTstat->Status & (1 << SB_Registered)) {
		if (package_type == PT_GR) {
			/* Socket G34: Fam15h BKDG v3.14 Table 99 */
			if (MaxDimmsInstallable == 1) {
				if (channel == 0)
					seed = 0x43;
				else if (channel == 1)
					seed = 0x3f;
				else if (channel == 2)
					seed = 0x3a;
				else if (channel == 3)
					seed = 0x35;
			} else if (MaxDimmsInstallable == 2) {
				if (channel == 0)
					seed = 0x54;
				else if (channel == 1)
					seed = 0x4d;
				else if (channel == 2)
					seed = 0x45;
				else if (channel == 3)
					seed = 0x40;
			} else if (MaxDimmsInstallable == 3) {
				if (channel == 0)
					seed = 0x6b;
				else if (channel == 1)
					seed = 0x5e;
				else if (channel == 2)
					seed = 0x4b;
				else if (channel == 3)
					seed = 0x3d;
			}
		} else if (package_type == PT_C3) {
			/* Socket C32: Fam15h BKDG v3.14 Table 100 */
			if ((MaxDimmsInstallable == 1) || (MaxDimmsInstallable == 2)) {
				if (channel == 0)
					seed = 0x3f;
				else if (channel == 1)
					seed = 0x3e;
			} else if (MaxDimmsInstallable == 3) {
				if (channel == 0)
					seed = 0x47;
				else if (channel == 1)
					seed = 0x38;
			}
		}
	} else if (pDCTstat->Status & (1 << SB_LoadReduced)) {
		if (package_type == PT_GR) {
			/* Socket G34: Fam15h BKDG v3.14 Table 99 */
			if (MaxDimmsInstallable == 1) {
				if (channel == 0)
					seed = 0x123;
				else if (channel == 1)
					seed = 0x122;
				else if (channel == 2)
					seed = 0x112;
				else if (channel == 3)
					seed = 0x102;
			}
		} else if (package_type == PT_C3) {
			/* Socket C32: Fam15h BKDG v3.14 Table 100 */
			if (channel == 0)
				seed = 0x132;
			else if (channel == 1)
				seed = 0x122;
		}
	} else {
		if (package_type == PT_GR) {
			/* Socket G34: Fam15h BKDG v3.14 Table 99 */
			if (MaxDimmsInstallable == 1) {
				if (channel == 0)
					seed = 0x3e;
				else if (channel == 1)
					seed = 0x38;
				else if (channel == 2)
					seed = 0x37;
				else if (channel == 3)
					seed = 0x31;
			} else if (MaxDimmsInstallable == 2) {
				if (channel == 0)
					seed = 0x51;
				else if (channel == 1)
					seed = 0x4a;
				else if (channel == 2)
					seed = 0x46;
				else if (channel == 3)
					seed = 0x3f;
			} else if (MaxDimmsInstallable == 3) {
				if (channel == 0)
					seed = 0x5e;
				else if (channel == 1)
					seed = 0x52;
				else if (channel == 2)
					seed = 0x48;
				else if (channel == 3)
					seed = 0x3c;
			}
		} else if (package_type == PT_C3) {
			/* Socket C32: Fam15h BKDG v3.14 Table 100 */
			if ((MaxDimmsInstallable == 1) || (MaxDimmsInstallable == 2)) {
				if (channel == 0)
					seed = 0x39;
				else if (channel == 1)
					seed = 0x32;
			} else if (MaxDimmsInstallable == 3) {
				if (channel == 0)
					seed = 0x45;
				else if (channel == 1)
					seed = 0x37;
			}
		} else if (package_type == PT_M2) {
			/* Socket AM3: Fam15h BKDG v3.14 Table 101 */
			seed = 0x3a;
		} else if (package_type == PT_FM2) {
			/* Socket FM2: Fam15h Model10 BKDG 3.12 Table 43 */
			seed = 0x32;
		}
	}

	printk(BIOS_DEBUG, "%s: using seed: %04x\n", __func__, seed);

	return seed;
}

void read_dqs_write_timing_control_registers(uint16_t *current_total_delay, uint32_t dev, uint8_t dct, uint8_t dimm, uint32_t index_reg)
{
	uint8_t lane;
	uint32_t dword;

	for (lane = 0; lane < MAX_BYTE_LANES; lane++) {
		uint32_t wdt_reg;
		if ((lane == 0) || (lane == 1))
			wdt_reg = 0x30;
		if ((lane == 2) || (lane == 3))
			wdt_reg = 0x31;
		if ((lane == 4) || (lane == 5))
			wdt_reg = 0x40;
		if ((lane == 6) || (lane == 7))
			wdt_reg = 0x41;
		if (lane == 8)
			wdt_reg = 0x32;
		wdt_reg += dimm * 3;
		dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, wdt_reg);
		if ((lane == 7) || (lane == 5) || (lane == 3) || (lane == 1))
			current_total_delay[lane] = (dword & 0x00ff0000) >> 16;
		if ((lane == 8) || (lane == 6) || (lane == 4) || (lane == 2) || (lane == 0))
			current_total_delay[lane] = dword & 0x000000ff;
	}
}

#ifdef UNUSED_CODE
static void write_dqs_write_timing_control_registers(uint16_t *current_total_delay, uint32_t dev, uint8_t dct, uint8_t dimm, uint32_t index_reg)
{
	uint8_t lane;
	uint32_t dword;

	for (lane = 0; lane < MAX_BYTE_LANES; lane++) {
		uint32_t ret_reg;
		if ((lane == 0) || (lane == 1))
			ret_reg = 0x30;
		if ((lane == 2) || (lane == 3))
			ret_reg = 0x31;
		if ((lane == 4) || (lane == 5))
			ret_reg = 0x40;
		if ((lane == 6) || (lane == 7))
			ret_reg = 0x41;
		if (lane == 8)
			ret_reg = 0x32;
		ret_reg += dimm * 3;
		dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, ret_reg);
		if ((lane == 7) || (lane == 5) || (lane == 3) || (lane == 1)) {
			dword &= ~(0xff << 16);
			dword |= (current_total_delay[lane] & 0xff) << 16;
		}
		if ((lane == 8) || (lane == 6) || (lane == 4) || (lane == 2) || (lane == 0)) {
			dword &= ~0xff;
			dword |= current_total_delay[lane] & 0xff;
		}
		Set_NB32_index_wait_DCT(dev, dct, index_reg, ret_reg, dword);
	}
}
#endif

static void write_write_data_timing_control_registers(uint16_t *current_total_delay, uint32_t dev, uint8_t dct, uint8_t dimm, uint32_t index_reg)
{
	uint8_t lane;
	uint32_t dword;

	for (lane = 0; lane < MAX_BYTE_LANES; lane++) {
		uint32_t wdt_reg;

		/* Calculate Write Data Timing register location */
		if ((lane == 0) || (lane == 1) || (lane == 2) || (lane == 3))
			wdt_reg = 0x1;
		if ((lane == 4) || (lane == 5) || (lane == 6) || (lane == 7))
			wdt_reg = 0x2;
		if (lane == 8)
			wdt_reg = 0x3;
		wdt_reg |= (dimm << 8);

		/* Set Write Data Timing register values */
		dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, wdt_reg);
		if ((lane == 7) || (lane == 3)) {
			dword &= ~(0x7f << 24);
			dword |= (current_total_delay[lane] & 0x7f) << 24;
		}
		if ((lane == 6) || (lane == 2)) {
			dword &= ~(0x7f << 16);
			dword |= (current_total_delay[lane] & 0x7f) << 16;
		}
		if ((lane == 5) || (lane == 1)) {
			dword &= ~(0x7f << 8);
			dword |= (current_total_delay[lane] & 0x7f) << 8;
		}
		if ((lane == 8) || (lane == 4) || (lane == 0)) {
			dword &= ~0x7f;
			dword |= current_total_delay[lane] & 0x7f;
		}
		Set_NB32_index_wait_DCT(dev, dct, index_reg, wdt_reg, dword);
	}
}

void read_dqs_receiver_enable_control_registers(uint16_t *current_total_delay, uint32_t dev, uint8_t dct, uint8_t dimm, uint32_t index_reg)
{
	uint8_t lane;
	uint32_t mask;
	uint32_t dword;

	if (is_fam15h())
		mask = 0x3ff;
	else
		mask = 0x1ff;

	for (lane = 0; lane < MAX_BYTE_LANES; lane++) {
		uint32_t ret_reg;
		if ((lane == 0) || (lane == 1))
			ret_reg = 0x10;
		if ((lane == 2) || (lane == 3))
			ret_reg = 0x11;
		if ((lane == 4) || (lane == 5))
			ret_reg = 0x20;
		if ((lane == 6) || (lane == 7))
			ret_reg = 0x21;
		if (lane == 8)
			ret_reg = 0x12;
		ret_reg += dimm * 3;
		dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, ret_reg);
		if ((lane == 7) || (lane == 5) || (lane == 3) || (lane == 1)) {
			current_total_delay[lane] = (dword & (mask << 16)) >> 16;
		}
		if ((lane == 8) || (lane == 6) || (lane == 4) || (lane == 2) || (lane == 0)) {
			current_total_delay[lane] = dword & mask;
		}
	}
}

void write_dqs_receiver_enable_control_registers(uint16_t *current_total_delay, uint32_t dev, uint8_t dct, uint8_t dimm, uint32_t index_reg)
{
	uint8_t lane;
	uint32_t mask;
	uint32_t dword;

	if (is_fam15h())
		mask = 0x3ff;
	else
		mask = 0x1ff;

	for (lane = 0; lane < MAX_BYTE_LANES; lane++) {
		uint32_t ret_reg;
		if ((lane == 0) || (lane == 1))
			ret_reg = 0x10;
		if ((lane == 2) || (lane == 3))
			ret_reg = 0x11;
		if ((lane == 4) || (lane == 5))
			ret_reg = 0x20;
		if ((lane == 6) || (lane == 7))
			ret_reg = 0x21;
		if (lane == 8)
			ret_reg = 0x12;
		ret_reg += dimm * 3;
		dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, ret_reg);
		if ((lane == 7) || (lane == 5) || (lane == 3) || (lane == 1)) {
			dword &= ~(mask << 16);
			dword |= (current_total_delay[lane] & mask) << 16;
		}
		if ((lane == 8) || (lane == 6) || (lane == 4) || (lane == 2) || (lane == 0)) {
			dword &= ~mask;
			dword |= current_total_delay[lane] & mask;
		}
		Set_NB32_index_wait_DCT(dev, dct, index_reg, ret_reg, dword);
	}
}

static void read_dram_phase_recovery_control_registers(uint16_t *current_total_delay, uint32_t dev, uint8_t dct, uint8_t dimm, uint32_t index_reg)
{
	uint8_t lane;
	uint32_t dword;

	for (lane = 0; lane < MAX_BYTE_LANES; lane++) {
		uint32_t prc_reg;

		/* Calculate DRAM Phase Recovery Control register location */
		if ((lane == 0) || (lane == 1) || (lane == 2) || (lane == 3))
			prc_reg = 0x50;
		if ((lane == 4) || (lane == 5) || (lane == 6) || (lane == 7))
			prc_reg = 0x51;
		if (lane == 8)
			prc_reg = 0x52;

		dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, prc_reg);
		if ((lane == 7) || (lane == 3)) {
			current_total_delay[lane] = (dword >> 24) & 0x7f;
		}
		if ((lane == 6) || (lane == 2)) {
			current_total_delay[lane] = (dword >> 16) & 0x7f;
		}
		if ((lane == 5) || (lane == 1)) {
			current_total_delay[lane] = (dword >> 8) & 0x7f;
		}
		if ((lane == 8) || (lane == 4) || (lane == 0)) {
			current_total_delay[lane] = dword & 0x7f;
		}
	}
}

static void write_dram_phase_recovery_control_registers(uint16_t *current_total_delay, uint32_t dev, uint8_t dct, uint8_t dimm, uint32_t index_reg)
{
	uint8_t lane;
	uint32_t dword;

	for (lane = 0; lane < MAX_BYTE_LANES; lane++) {
		uint32_t prc_reg;

		/* Calculate DRAM Phase Recovery Control register location */
		if ((lane == 0) || (lane == 1) || (lane == 2) || (lane == 3))
			prc_reg = 0x50;
		if ((lane == 4) || (lane == 5) || (lane == 6) || (lane == 7))
			prc_reg = 0x51;
		if (lane == 8)
			prc_reg = 0x52;

		/* Set DRAM Phase Recovery Control register values */
		dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, prc_reg);
		if ((lane == 7) || (lane == 3)) {
			dword &= ~(0x7f << 24);
			dword |= (current_total_delay[lane] & 0x7f) << 24;
		}
		if ((lane == 6) || (lane == 2)) {
			dword &= ~(0x7f << 16);
			dword |= (current_total_delay[lane] & 0x7f) << 16;
		}
		if ((lane == 5) || (lane == 1)) {
			dword &= ~(0x7f << 8);
			dword |= (current_total_delay[lane] & 0x7f) << 8;
		}
		if ((lane == 8) || (lane == 4) || (lane == 0)) {
			dword &= ~0x7f;
			dword |= current_total_delay[lane] & 0x7f;
		}
		Set_NB32_index_wait_DCT(dev, dct, index_reg, prc_reg, dword);
	}
}

void read_dqs_read_data_timing_registers(uint16_t *delay, uint32_t dev, uint8_t dct, uint8_t dimm, uint32_t index_reg)
{
	uint8_t shift;
	uint32_t dword;
	uint32_t mask;

	if (is_fam15h()) {
		mask = 0x3e;
		shift = 1;
	}
	else {
		mask = 0x3f;
		shift = 0;
	}

	/* Lanes 0 - 3 */
	dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x5 | (dimm << 8));
	delay[3] = ((dword >> 24) & mask) >> shift;
	delay[2] = ((dword >> 16) & mask) >> shift;
	delay[1] = ((dword >> 8) & mask) >> shift;
	delay[0] = (dword & mask) >> shift;

	/* Lanes 4 - 7 */
	dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x6 | (dimm << 8));
	delay[7] = ((dword >> 24) & mask) >> shift;
	delay[6] = ((dword >> 16) & mask) >> shift;
	delay[5] = ((dword >> 8) & mask) >> shift;
	delay[4] = (dword & mask) >> shift;

	/* Lane 8 (ECC) */
	dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x7 | (dimm << 8));
	delay[8] = (dword & mask) >> shift;
}

void write_dqs_read_data_timing_registers(uint16_t *delay, uint32_t dev, uint8_t dct, uint8_t dimm, uint32_t index_reg)
{
	uint8_t shift;
	uint32_t dword;
	uint32_t mask;

	if (is_fam15h()) {
		mask = 0x3e;
		shift = 1;
	}
	else {
		mask = 0x3f;
		shift = 0;
	}

	/* Lanes 0 - 3 */
	dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x5 | (dimm << 8));
	dword &= ~(mask << 24);
	dword &= ~(mask << 16);
	dword &= ~(mask << 8);
	dword &= ~mask;
	dword |= ((delay[3] << shift) & mask) << 24;
	dword |= ((delay[2] << shift) & mask) << 16;
	dword |= ((delay[1] << shift) & mask) << 8;
	dword |= (delay[0] << shift) & mask;
	Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x5 | (dimm << 8), dword);

	/* Lanes 4 - 7 */
	dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x6 | (dimm << 8));
	dword &= ~(mask << 24);
	dword &= ~(mask << 16);
	dword &= ~(mask << 8);
	dword &= ~mask;
	dword |= ((delay[7] << shift) & mask) << 24;
	dword |= ((delay[6] << shift) & mask) << 16;
	dword |= ((delay[5] << shift) & mask) << 8;
	dword |= (delay[4] << shift) & mask;
	Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x6 | (dimm << 8), dword);

	/* Lane 8 (ECC) */
	dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x7 | (dimm << 8));
	dword &= ~mask;
	dword |= (delay[8] << shift) & mask;
	Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x7 | (dimm << 8), dword);
}

static uint32_t convert_testaddr_and_channel_to_address(struct DCTStatStruc *pDCTstat, uint32_t testaddr, uint8_t channel)
{
	SetUpperFSbase(testaddr);
	testaddr <<= 8;

	if ((pDCTstat->Status & (1<<SB_128bitmode)) && channel) {
		testaddr += 8;	/* second channel */
	}

	return testaddr;
}

/* DQS Receiver Enable Training (Family 10h)
 * Algorithm detailed in:
 * The Fam10h BKDG Rev. 3.62 section 2.8.9.9.2
 */
static void dqsTrainRcvrEn_SW_Fam10(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 Pass)
{
	u8 Channel;
	u8 _2Ranks;
	u8 Addl_Index = 0;
	u8 Receiver;
	u8 _DisableDramECC = 0, _Wrap32Dis = 0, _SSE2 = 0;
	u16 CTLRMaxDelay;
	u16 MaxDelay_CH[2];
	u32 TestAddr0, TestAddr1, TestAddr0B, TestAddr1B;
	u32 PatternBuffer[64+4]; /* FIXME: need increase 8? */
	u32 Errors;

	u32 val;
	u32 reg;
	u32 dev;
	u32 index_reg;
	u32 ch_start, ch_end, ch;
	msr_t msr;
	u32 cr4;

	uint32_t dword;
	uint8_t dimm;
	uint8_t rank;
	uint8_t lane;
	uint16_t current_total_delay[MAX_BYTE_LANES];
	uint16_t candidate_total_delay[8];
	uint8_t data_test_pass_sr[2][8];	/* [rank][lane] */
	uint8_t data_test_pass[8];		/* [lane] */
	uint8_t data_test_pass_prev[8];		/* [lane] */
	uint8_t window_det_toggle[8];
	uint8_t trained[8];
	uint64_t result_qword1;
	uint64_t result_qword2;

	u8 valid;

	print_debug_dqs("\nTrainRcvEn: Node", pDCTstat->Node_ID, 0);
	print_debug_dqs("TrainRcvEn: Pass", Pass, 0);

	dev = pDCTstat->dev_dct;
	ch_start = 0;
	if (!pDCTstat->GangedMode) {
		ch_end = 2;
	} else {
		ch_end = 1;
	}

	for (ch = ch_start; ch < ch_end; ch++) {
		reg = 0x78;
		val = Get_NB32_DCT(dev, ch, reg);
		val &= ~(0x3ff << 22);
		val |= (0x0c8 << 22);		/* MaxRdLatency = 0xc8 */
		Set_NB32_DCT(dev, ch, reg, val);
	}

	if (Pass == FirstPass) {
		mct_InitDQSPos4RcvrEn_D(pMCTstat, pDCTstat);
	} else {
		pDCTstat->DimmTrainFail = 0;
		pDCTstat->CSTrainFail = ~pDCTstat->CSPresent;
	}

	cr4 = read_cr4();
	if (cr4 & (1 << 9)) {	/* save the old value */
		_SSE2 = 1;
	}
	cr4 |= (1 << 9);	/* OSFXSR enable SSE2 */
	write_cr4(cr4);

	msr = rdmsr(HWCR);
	/* FIXME: Why use SSEDIS */
	if (msr.lo & (1 << 17)) {	/* save the old value */
		_Wrap32Dis = 1;
	}
	msr.lo |= (1 << 17);	/* HWCR.wrap32dis */
	msr.lo &= ~(1 << 15);	/* SSEDIS */
	wrmsr(HWCR, msr);	/* Setting wrap32dis allows 64-bit memory references in real mode */

	_DisableDramECC = mct_DisableDimmEccEn_D(pMCTstat, pDCTstat);

	SetupRcvrPattern(pMCTstat, pDCTstat, PatternBuffer, Pass);

	Errors = 0;
	dev = pDCTstat->dev_dct;

	for (Channel = 0; Channel < 2; Channel++) {
		print_debug_dqs("\tTrainRcvEn51: Node ", pDCTstat->Node_ID, 1);
		print_debug_dqs("\tTrainRcvEn51: Channel ", Channel, 1);
		pDCTstat->Channel = Channel;

		CTLRMaxDelay = 0;
		MaxDelay_CH[Channel] = 0;
		index_reg = 0x98;

		Receiver = mct_InitReceiver_D(pDCTstat, Channel);
		/* There are four receiver pairs, loosely associated with chipselects.
		 * This is essentially looping over each DIMM.
		 */
		for (; Receiver < 8; Receiver += 2) {
			Addl_Index = (Receiver >> 1) * 3 + 0x10;
			dimm = (Receiver >> 1);

			print_debug_dqs("\t\tTrainRcvEnd52: index ", Addl_Index, 2);

			if (!mct_RcvrRankEnabled_D(pMCTstat, pDCTstat, Channel, Receiver)) {
				continue;
			}

			/* Clear data structures */
			for (lane = 0; lane < 8; lane++) {
				data_test_pass_prev[lane] = 0;
				trained[lane] = 0;
			}

			/* 2.8.9.9.2 (1, 6)
			 * Retrieve gross and fine timing fields from write DQS registers
			 */
			read_dqs_write_timing_control_registers(current_total_delay, dev, Channel, dimm, index_reg);

			/* 2.8.9.9.2 (1)
			 * Program the Write Data Timing and Write ECC Timing register to
			 * the values stored in the DQS Write Timing Control register
			 * for each lane
			 */
			write_write_data_timing_control_registers(current_total_delay, dev, Channel, dimm, index_reg);

			/* 2.8.9.9.2 (2)
			 * Program the Read DQS Timing Control and the Read DQS ECC Timing Control registers
			 * to 1/2 MEMCLK for all lanes
			 */
			for (lane = 0; lane < MAX_BYTE_LANES; lane++) {
				uint32_t rdt_reg;
				if ((lane == 0) || (lane == 1) || (lane == 2) || (lane == 3))
					rdt_reg = 0x5;
				if ((lane == 4) || (lane == 5) || (lane == 6) || (lane == 7))
					rdt_reg = 0x6;
				if (lane == 8)
					rdt_reg = 0x7;
				rdt_reg |= (dimm << 8);
				if (lane == 8)
					dword = 0x0000003f;
				else
					dword = 0x3f3f3f3f;
				Set_NB32_index_wait_DCT(dev, Channel, index_reg, rdt_reg, dword);
			}

			/* 2.8.9.9.2 (3)
			 * Select two test addresses for each rank present
			 */
			TestAddr0 = mct_GetRcvrSysAddr_D(pMCTstat, pDCTstat, Channel, Receiver, &valid);
			if (!valid) {	/* Address not supported on current CS */
				continue;
			}

			TestAddr0B = TestAddr0 + (BigPagex8_RJ8 << 3);

			if (mct_RcvrRankEnabled_D(pMCTstat, pDCTstat, Channel, Receiver+1)) {
				TestAddr1 = mct_GetRcvrSysAddr_D(pMCTstat, pDCTstat, Channel, Receiver+1, &valid);
				if (!valid) {	/* Address not supported on current CS */
					continue;
				}
				TestAddr1B = TestAddr1 + (BigPagex8_RJ8 << 3);
				_2Ranks = 1;
			} else {
				_2Ranks = TestAddr1 = TestAddr1B = 0;
			}

			print_debug_dqs("\t\tTrainRcvEn53: TestAddr0 ", TestAddr0, 2);
			print_debug_dqs("\t\tTrainRcvEn53: TestAddr0B ", TestAddr0B, 2);
			print_debug_dqs("\t\tTrainRcvEn53: TestAddr1 ", TestAddr1, 2);
			print_debug_dqs("\t\tTrainRcvEn53: TestAddr1B ", TestAddr1B, 2);

			/* 2.8.9.9.2 (4, 5)
			 * Write 1 cache line of the appropriate test pattern to each test address
			 */
			mct_Write1LTestPattern_D(pMCTstat, pDCTstat, TestAddr0, 0); /* rank 0 of DIMM, testpattern 0 */
			mct_Write1LTestPattern_D(pMCTstat, pDCTstat, TestAddr0B, 1); /* rank 0 of DIMM, testpattern 1 */
			if (_2Ranks) {
				mct_Write1LTestPattern_D(pMCTstat, pDCTstat, TestAddr1, 0); /*rank 1 of DIMM, testpattern 0 */
				mct_Write1LTestPattern_D(pMCTstat, pDCTstat, TestAddr1B, 1); /*rank 1 of DIMM, testpattern 1 */
			}

#if DQS_TRAIN_DEBUG > 0
			for (lane = 0; lane < 8; lane++) {
				print_debug_dqs("\t\tTrainRcvEn54: lane: ", lane, 2);
				print_debug_dqs("\t\tTrainRcvEn54: current_total_delay ", current_total_delay[lane], 2);
			}
#endif

			/* 2.8.9.9.2 (6)
			 * Write gross and fine timing fields to read DQS registers
			 */
			write_dqs_receiver_enable_control_registers(current_total_delay, dev, Channel, dimm, index_reg);

			/* 2.8.9.9.2 (7)
			 * Loop over all delay values up to 1 MEMCLK (0x40 delay steps) from the initial delay values
			 *
			 * FIXME
			 * It is not clear if training should be discontinued if any test failures occur in the first
			 * 1 MEMCLK window, or if it should be discontinued if no successes occur in the first 1 MEMCLK
			 * window.  Therefore, loop over up to 2 MEMCLK (0x80 delay steps) to be on the safe side.
			 */
			uint16_t current_delay_step;

			for (current_delay_step = 0; current_delay_step < 0x80; current_delay_step++) {
				print_debug_dqs("\t\t\tTrainRcvEn541: current_delay_step ", current_delay_step, 3);

				/* 2.8.9.9.2 (7 D)
				* Terminate if all lanes are trained
				*/
				uint8_t all_lanes_trained = 1;
				for (lane = 0; lane < 8; lane++)
					if (!trained[lane])
						all_lanes_trained = 0;

				if (all_lanes_trained)
					break;

				/* 2.8.9.9.2 (7 A)
				 * Loop over all ranks
				 */
				for (rank = 0; rank < (_2Ranks + 1); rank++) {
					/* 2.8.9.9.2 (7 A a-d)
					 * Read the first test address of the current rank
					 * Store the first data beat for analysis
					 * Reset read pointer in the DRAM controller FIFO
					 * Read the second test address of the current rank
					 * Store the first data beat for analysis
					 * Reset read pointer in the DRAM controller FIFO
					 */
					if (rank & 1) {
						/* 2.8.9.9.2 (7 D)
						 * Invert read instructions to alternate data read order on the bus
						 */
						proc_IOCLFLUSH_D((rank == 0)?TestAddr0B:TestAddr1B);
						result_qword2 = read64_fs(convert_testaddr_and_channel_to_address(pDCTstat, (rank == 0)?TestAddr0B:TestAddr1B, Channel));
						write_dqs_receiver_enable_control_registers(current_total_delay, dev, Channel, dimm, index_reg);
						proc_IOCLFLUSH_D((rank == 0)?TestAddr0:TestAddr1);
						result_qword1 = read64_fs(convert_testaddr_and_channel_to_address(pDCTstat, (rank == 0)?TestAddr0:TestAddr1, Channel));
						write_dqs_receiver_enable_control_registers(current_total_delay, dev, Channel, dimm, index_reg);
					} else {
						proc_IOCLFLUSH_D((rank == 0)?TestAddr0:TestAddr1);
						result_qword1 = read64_fs(convert_testaddr_and_channel_to_address(pDCTstat, (rank == 0)?TestAddr0:TestAddr1, Channel));
						write_dqs_receiver_enable_control_registers(current_total_delay, dev, Channel, dimm, index_reg);
						proc_IOCLFLUSH_D((rank == 0)?TestAddr0B:TestAddr1B);
						result_qword2 = read64_fs(convert_testaddr_and_channel_to_address(pDCTstat, (rank == 0)?TestAddr0B:TestAddr1B, Channel));
						write_dqs_receiver_enable_control_registers(current_total_delay, dev, Channel, dimm, index_reg);
					}
					/* 2.8.9.9.2 (7 A e)
					 * Compare both read patterns and flag passing ranks/lanes
					 */
					uint8_t result_lane_byte1;
					uint8_t result_lane_byte2;
					for (lane = 0; lane < 8; lane++) {
						if (trained[lane] == 1) {
#if DQS_TRAIN_DEBUG > 0
							print_debug_dqs("\t\t\t\t\t\t\t\t lane already trained: ", lane, 4);
#endif
							continue;
						}

						result_lane_byte1 = (result_qword1 >> (lane * 8)) & 0xff;
						result_lane_byte2 = (result_qword2 >> (lane * 8)) & 0xff;
						if ((result_lane_byte1 == 0x55) && (result_lane_byte2 == 0xaa))
							data_test_pass_sr[rank][lane] = 1;
						else
							data_test_pass_sr[rank][lane] = 0;
#if DQS_TRAIN_DEBUG > 0
						print_debug_dqs_pair("\t\t\t\t\t\t\t\t ", 0x55, "  |  ", result_lane_byte1, 4);
						print_debug_dqs_pair("\t\t\t\t\t\t\t\t ", 0xaa, "  |  ", result_lane_byte2, 4);
#endif
					}
				}

				/* 2.8.9.9.2 (7 B)
				 * If DIMM is dual rank, only use delays that pass testing for both ranks
				 */
				for (lane = 0; lane < 8; lane++) {
					if (_2Ranks) {
						if ((data_test_pass_sr[0][lane]) && (data_test_pass_sr[1][lane]))
							data_test_pass[lane] = 1;
						else
							data_test_pass[lane] = 0;
					} else {
						data_test_pass[lane] = data_test_pass_sr[0][lane];
					}
				}

				/* 2.8.9.9.2 (7 E)
				 * For each lane, update the DQS receiver delay setting in support of next iteration
				 */
				for (lane = 0; lane < 8; lane++) {
					if (trained[lane] == 1)
						continue;

					/* 2.8.9.9.2 (7 C a)
					 * Save the total delay of the first success after a failure for later use
					 */
					if ((data_test_pass[lane] == 1) && (data_test_pass_prev[lane] == 0)) {
						candidate_total_delay[lane] = current_total_delay[lane];
						window_det_toggle[lane] = 0;
					}

					/* 2.8.9.9.2 (7 C b)
					 * If the current delay failed testing add 1/8 UI to the current delay
					 */
					if (data_test_pass[lane] == 0)
						current_total_delay[lane] += 0x4;

					/* 2.8.9.9.2 (7 C c)
					 * If the current delay passed testing alternately add either 1/32 UI or 1/4 UI to the current delay
					 * If 1.25 UI of delay have been added with no failures the lane is considered trained
					 */
					if (data_test_pass[lane] == 1) {
						/* See if lane is trained */
						if ((current_total_delay[lane] - candidate_total_delay[lane]) >= 0x28) {
							trained[lane] = 1;

							/* Calculate and set final lane delay value
							 * The final delay is the candidate delay + 7/8 UI
							 */
							current_total_delay[lane] = candidate_total_delay[lane] + 0x1c;
						} else {
							if (window_det_toggle[lane] == 0) {
								current_total_delay[lane] += 0x1;
								window_det_toggle[lane] = 1;
							} else {
								current_total_delay[lane] += 0x8;
								window_det_toggle[lane] = 0;
							}
						}
					}
				}

				/* Update delays in hardware */
				write_dqs_receiver_enable_control_registers(current_total_delay, dev, Channel, dimm, index_reg);

				/* Save previous results for comparison in the next iteration */
				for (lane = 0; lane < 8; lane++)
					data_test_pass_prev[lane] = data_test_pass[lane];
			}

#if DQS_TRAIN_DEBUG > 0
			for (lane = 0; lane < 8; lane++)
				print_debug_dqs_pair("\t\tTrainRcvEn55: Lane ", lane, " current_total_delay ", current_total_delay[lane], 2);
#endif

			/* Find highest delay value and save for later use */
			for (lane = 0; lane < 8; lane++)
				if (current_total_delay[lane] > CTLRMaxDelay)
					CTLRMaxDelay = current_total_delay[lane];

			/* See if any lanes failed training, and set error flags appropriately
			 * For all trained lanes, save delay values for later use
			 */
			for (lane = 0; lane < 8; lane++) {
				if (trained[lane]) {
					pDCTstat->CH_D_B_RCVRDLY[Channel][Receiver >> 1][lane] = current_total_delay[lane];
				} else {
					printk(BIOS_WARNING, "TrainRcvrEn: WARNING: Lane %d of receiver %d on channel %d failed training!\n", lane, Receiver, Channel);

					/* Set error flags */
					pDCTstat->ErrStatus |= 1 << SB_NORCVREN;
					Errors |= 1 << SB_NORCVREN;
					pDCTstat->ErrCode = SC_FatalErr;
					pDCTstat->CSTrainFail |= 1 << Receiver;
					pDCTstat->DimmTrainFail |= 1 << (Receiver + Channel);
				}
			}

			/* 2.8.9.9.2 (8)
			 * Flush the receiver FIFO
			 * Write one full cache line of non-0x55/0xaa data to one of the test addresses, then read it back to flush the FIFO
			 */
			/* FIXME
			 * This does not seem to be needed, and has a tendency to lock up the
			 * boot process while attempting to write the test pattern.
			 */
		}
		MaxDelay_CH[Channel] = CTLRMaxDelay;
	}

	CTLRMaxDelay = MaxDelay_CH[0];
	if (MaxDelay_CH[1] > CTLRMaxDelay)
		CTLRMaxDelay = MaxDelay_CH[1];

	for (Channel = 0; Channel < 2; Channel++) {
		mct_SetMaxLatency_D(pDCTstat, Channel, CTLRMaxDelay); /* program Ch A/B MaxAsyncLat to correspond with max delay */
	}

	for (Channel = 0; Channel < 2; Channel++) {
		ResetDCTWrPtr_D(dev, Channel, index_reg, Addl_Index);
	}

	if (_DisableDramECC) {
		mct_EnableDimmEccEn_D(pMCTstat, pDCTstat, _DisableDramECC);
	}

	if (Pass == FirstPass) {
		/*Disable DQSRcvrEn training mode */
		mct_DisableDQSRcvEn_D(pDCTstat);
	}

	if (!_Wrap32Dis) {
		msr = rdmsr(HWCR);
		msr.lo &= ~(1<<17);	/* restore HWCR.wrap32dis */
		wrmsr(HWCR, msr);
	}
	if (!_SSE2) {
		cr4 = read_cr4();
		cr4 &= ~(1<<9);		/* restore cr4.OSFXSR */
		write_cr4(cr4);
	}

#if DQS_TRAIN_DEBUG > 0
	{
		u8 ChannelDTD;
		printk(BIOS_DEBUG, "TrainRcvrEn: CH_MaxRdLat:\n");
		for (ChannelDTD = 0; ChannelDTD < 2; ChannelDTD++) {
			printk(BIOS_DEBUG, "Channel:%x: %x\n",
			       ChannelDTD, pDCTstat->CH_MaxRdLat[ChannelDTD][0]);
		}
	}
#endif

#if DQS_TRAIN_DEBUG > 0
	{
		u16 valDTD;
		u8 ChannelDTD, ReceiverDTD;
		u8 i;
		u16 *p;

		printk(BIOS_DEBUG, "TrainRcvrEn: CH_D_B_RCVRDLY:\n");
		for (ChannelDTD = 0; ChannelDTD < 2; ChannelDTD++) {
			printk(BIOS_DEBUG, "Channel:%x\n", ChannelDTD);
			for (ReceiverDTD = 0; ReceiverDTD < 8; ReceiverDTD+=2) {
				printk(BIOS_DEBUG, "\t\tReceiver:%x:", ReceiverDTD);
				p = pDCTstat->CH_D_B_RCVRDLY[ChannelDTD][ReceiverDTD>>1];
				for (i = 0; i < 8; i++) {
					valDTD = p[i];
					printk(BIOS_DEBUG, " %03x", valDTD);
				}
				printk(BIOS_DEBUG, "\n");
			}
		}
	}
#endif

	printk(BIOS_DEBUG, "TrainRcvrEn: Status %x\n", pDCTstat->Status);
	printk(BIOS_DEBUG, "TrainRcvrEn: ErrStatus %x\n", pDCTstat->ErrStatus);
	printk(BIOS_DEBUG, "TrainRcvrEn: ErrCode %x\n", pDCTstat->ErrCode);
	printk(BIOS_DEBUG, "TrainRcvrEn: Done\n\n");
}

/* DQS Receiver Enable Training Pattern Generation (Family 15h)
 * Algorithm detailed in:
 * The Fam15h BKDG Rev. 3.14 section 2.10.5.8.2 (4)
 */
static void generate_dram_receiver_enable_training_pattern_fam15(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, uint8_t dct, uint8_t Receiver)
{
	uint32_t dword;
	uint32_t dev = pDCTstat->dev_dct;

	/* 2.10.5.7.1.1
	 * It appears that the DCT only supports 8-beat burst length mode,
	 * so do nothing here...
	 */

	/* Wait for CmdSendInProg == 0 */
	do {
		dword = Get_NB32_DCT(dev, dct, 0x250);
	} while (dword & (0x1 << 12));

	/* Set CmdTestEnable = 1 */
	dword = Get_NB32_DCT(dev, dct, 0x250);
	dword |= (0x1 << 2);
	Set_NB32_DCT(dev, dct, 0x250, dword);

	/* 2.10.5.8.6.1.1 Send Activate Command */
	dword = Get_NB32_DCT(dev, dct, 0x28c);
	dword &= ~(0xff << 22);				/* CmdChipSelect = Receiver */
	dword |= ((0x1 << Receiver) << 22);
	dword &= ~(0x7 << 19);				/* CmdBank = 0 */
	dword &= ~(0x3ffff);				/* CmdAddress = 0 */
	dword |= (0x1 << 31);				/* SendActCmd = 1 */
	Set_NB32_DCT(dev, dct, 0x28c, dword);

	/* Wait for SendActCmd == 0 */
	do {
		dword = Get_NB32_DCT(dev, dct, 0x28c);
	} while (dword & (0x1 << 31));

	/* Wait 75 MEMCLKs. */
	precise_memclk_delay_fam15(pMCTstat, pDCTstat, dct, 75);

	/* 2.10.5.8.6.1.2 */
	Set_NB32_DCT(dev, dct, 0x274, 0x0);		/* DQMask = 0 */
	Set_NB32_DCT(dev, dct, 0x278, 0x0);

	dword = Get_NB32_DCT(dev, dct, 0x27c);
	dword &= ~(0xff);				/* EccMask = 0 */
	if (pDCTstat->DimmECCPresent == 0)
		dword |= 0xff;				/* EccMask = 0xff */
	Set_NB32_DCT(dev, dct, 0x27c, dword);

	/* 2.10.5.8.6.1.2 */
	dword = Get_NB32_DCT(dev, dct, 0x270);
	dword &= ~(0x7ffff);				/* DataPrbsSeed = 55555 */
	dword |= (0x44443);				/* Use AGESA seed */
	Set_NB32_DCT(dev, dct, 0x270, dword);

	/* 2.10.5.8.2 (4) */
	dword = Get_NB32_DCT(dev, dct, 0x260);
	dword &= ~(0x1fffff);				/* CmdCount = 192 */
	dword |= 192;
	Set_NB32_DCT(dev, dct, 0x260, dword);

	/* Configure Target A */
	dword = Get_NB32_DCT(dev, dct, 0x254);
	dword &= ~(0x7 << 24);				/* TgtChipSelect = Receiver */
	dword |= (Receiver & 0x7) << 24;
	dword &= ~(0x7 << 21);				/* TgtBank = 0 */
	dword &= ~(0x3ff);				/* TgtAddress = 0 */
	Set_NB32_DCT(dev, dct, 0x254, dword);

	dword = Get_NB32_DCT(dev, dct, 0x250);
	dword |= (0x1 << 3);				/* ResetAllErr = 1 */
	dword &= ~(0x1 << 4);				/* StopOnErr = 0 */
	dword &= ~(0x3 << 8);				/* CmdTgt = 0 (Target A) */
	dword &= ~(0x7 << 5);				/* CmdType = 0 (Read) */
	dword |= (0x1 << 11);				/* SendCmd = 1 */
	Set_NB32_DCT(dev, dct, 0x250, dword);

	/* 2.10.5.8.6.1.2 Wait for TestStatus == 1 and CmdSendInProg == 0 */
	do {
		dword = Get_NB32_DCT(dev, dct, 0x250);
	} while ((dword & (0x1 << 12)) || (!(dword & (0x1 << 10))));

	dword = Get_NB32_DCT(dev, dct, 0x250);
	dword &= ~(0x1 << 11);				/* SendCmd = 0 */
	Set_NB32_DCT(dev, dct, 0x250, dword);

	/* 2.10.5.8.6.1.1 Send Precharge Command */
	/* Wait 25 MEMCLKs. */
	precise_memclk_delay_fam15(pMCTstat, pDCTstat, dct, 25);

	dword = Get_NB32_DCT(dev, dct, 0x28c);
	dword &= ~(0xff << 22);				/* CmdChipSelect = Receiver */
	dword |= ((0x1 << Receiver) << 22);
	dword &= ~(0x7 << 19);				/* CmdBank = 0 */
	dword &= ~(0x3ffff);				/* CmdAddress = 0x400 */
	dword |= 0x400;
	dword |= (0x1 << 30);				/* SendPchgCmd = 1 */
	Set_NB32_DCT(dev, dct, 0x28c, dword);

	/* Wait for SendPchgCmd == 0 */
	do {
		dword = Get_NB32_DCT(dev, dct, 0x28c);
	} while (dword & (0x1 << 30));

	/* Wait 25 MEMCLKs. */
	precise_memclk_delay_fam15(pMCTstat, pDCTstat, dct, 25);

	/* Set CmdTestEnable = 0 */
	dword = Get_NB32_DCT(dev, dct, 0x250);
	dword &= ~(0x1 << 2);
	Set_NB32_DCT(dev, dct, 0x250, dword);
}

/* DQS Receiver Enable Training (Family 15h)
 * Algorithm detailed in:
 * The Fam15h BKDG Rev. 3.14 section 2.10.5.8.2
 * This algorithm runs once at the lowest supported MEMCLK,
 * then once again at the highest supported MEMCLK.
 */
static void dqsTrainRcvrEn_SW_Fam15(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 Pass)
{
	u8 Channel;
	u8 _2Ranks;
	u8 Addl_Index = 0;
	u8 Receiver;
	u8 _DisableDramECC = 0, _Wrap32Dis = 0, _SSE2 = 0;
	u32 Errors;

	u32 val;
	u32 dev;
	u32 index_reg;
	u32 ch_start, ch_end, ch;
	u32 msr;
	u32 cr4;
	u32 lo, hi;

	uint32_t dword;
	uint8_t dimm;
	uint8_t rank;
	uint8_t lane;
	uint8_t nibble;
	uint8_t mem_clk;
	uint16_t min_mem_clk;
	uint16_t initial_seed;
	uint8_t train_both_nibbles;
	uint16_t current_total_delay[MAX_BYTE_LANES];
	uint16_t nibble0_current_total_delay[MAX_BYTE_LANES];
	uint16_t dqs_ret_pass1_total_delay[MAX_BYTE_LANES];
	uint16_t rank0_current_total_delay[MAX_BYTE_LANES];
	uint16_t phase_recovery_delays[MAX_BYTE_LANES];
	uint16_t seed[MAX_BYTE_LANES];
	uint16_t seed_gross[MAX_BYTE_LANES];
	uint16_t seed_fine[MAX_BYTE_LANES];
	uint16_t seed_pre_gross[MAX_BYTE_LANES];

	uint8_t package_type = mctGet_NVbits(NV_PACK_TYPE);
	uint16_t fam15h_freq_tab[] = {0, 0, 0, 0, 333, 0, 400, 0, 0, 0, 533, 0, 0, 0, 667, 0, 0, 0, 800, 0, 0, 0, 933};

	uint8_t lane_count;
	lane_count = get_available_lane_count(pMCTstat, pDCTstat);

	print_debug_dqs("\nTrainRcvEn: Node", pDCTstat->Node_ID, 0);
	print_debug_dqs("TrainRcvEn: Pass", Pass, 0);

	min_mem_clk = mctGet_NVbits(NV_MIN_MEMCLK);

	train_both_nibbles = 0;
	if (pDCTstat->Dimmx4Present)
		if (is_fam15h())
			train_both_nibbles = 1;

	dev = pDCTstat->dev_dct;
	index_reg = 0x98;
	ch_start = 0;
	ch_end = 2;

	for (ch = ch_start; ch < ch_end; ch++) {
		uint8_t max_rd_latency = 0x55;
		uint8_t p_state;

		/* 2.10.5.6 */
		fam15EnableTrainingMode(pMCTstat, pDCTstat, ch, 1);

		/* 2.10.5.2 */
		for (p_state = 0; p_state < 3; p_state++) {
			val = Get_NB32_DCT_NBPstate(dev, ch, p_state, 0x210);
			val &= ~(0x3ff << 22);			/* MaxRdLatency = max_rd_latency */
			val |= (max_rd_latency & 0x3ff) << 22;
			Set_NB32_DCT_NBPstate(dev, ch, p_state, 0x210, val);
		}
	}

	if (Pass != FirstPass) {
		pDCTstat->DimmTrainFail = 0;
		pDCTstat->CSTrainFail = ~pDCTstat->CSPresent;
	}

	cr4 = read_cr4();
	if (cr4 & (1 << 9)) {	/* save the old value */
		_SSE2 = 1;
	}
	cr4 |= (1 << 9);	/* OSFXSR enable SSE2 */
	write_cr4(cr4);

	msr = HWCR;
	_RDMSR(msr, &lo, &hi);
	/* FIXME: Why use SSEDIS */
	if (lo & (1 << 17)) {	/* save the old value */
		_Wrap32Dis = 1;
	}
	lo |= (1 << 17);	/* HWCR.wrap32dis */
	lo &= ~(1 << 15);	/* SSEDIS */
	_WRMSR(msr, lo, hi);	/* Setting wrap32dis allows 64-bit memory references in real mode */

	_DisableDramECC = mct_DisableDimmEccEn_D(pMCTstat, pDCTstat);

	Errors = 0;
	dev = pDCTstat->dev_dct;

	for (Channel = 0; Channel < 2; Channel++) {
		print_debug_dqs("\tTrainRcvEn51: Node ", pDCTstat->Node_ID, 1);
		print_debug_dqs("\tTrainRcvEn51: Channel ", Channel, 1);
		pDCTstat->Channel = Channel;

		mem_clk = Get_NB32_DCT(dev, Channel, 0x94) & 0x1f;

		Receiver = mct_InitReceiver_D(pDCTstat, Channel);
		/* There are four receiver pairs, loosely associated with chipselects.
		 * This is essentially looping over each DIMM.
		 */
		for (; Receiver < 8; Receiver += 2) {
			Addl_Index = (Receiver >> 1) * 3 + 0x10;
			dimm = (Receiver >> 1);

			print_debug_dqs("\t\tTrainRcvEnd52: index ", Addl_Index, 2);

			if (!mct_RcvrRankEnabled_D(pMCTstat, pDCTstat, Channel, Receiver)) {
				continue;
			}

			/* Retrieve the total delay values from pass 1 of DQS receiver enable training */
			if (Pass != FirstPass) {
				read_dqs_receiver_enable_control_registers(dqs_ret_pass1_total_delay, dev, Channel, dimm, index_reg);
			}

			/* 2.10.5.8.2
			 * Loop over all ranks
			 */
			if (mct_RcvrRankEnabled_D(pMCTstat, pDCTstat, Channel, Receiver+1))
				_2Ranks = 1;
			else
				_2Ranks = 0;
			for (rank = 0; rank < (_2Ranks + 1); rank++) {
				for (nibble = 0; nibble < (train_both_nibbles + 1); nibble++) {
					/* 2.10.5.8.2 (1)
					 * Specify the target DIMM and nibble to be trained
					 */
					dword = Get_NB32_index_wait_DCT(dev, Channel, index_reg, 0x00000008);
					dword &= ~(0x3 << 4);		/* TrDimmSel = dimm */
					dword |= ((dimm & 0x3) << 4);
					dword &= ~(0x1 << 2);		/* TrNibbleSel = nibble */
					dword |= ((nibble & 0x1) << 2);
					Set_NB32_index_wait_DCT(dev, Channel, index_reg, 0x00000008, dword);

					/* 2.10.5.8.2 (2)
					 * Retrieve gross and fine timing fields from write DQS registers
					 */
					read_dqs_write_timing_control_registers(current_total_delay, dev, Channel, dimm, index_reg);

					/* 2.10.5.8.2.1
					 * Generate the DQS Receiver Enable Training Seed Values
					 */
					if (Pass == FirstPass) {
						initial_seed = fam15_receiver_enable_training_seed(pDCTstat, Channel, dimm, rank, package_type);

						/* Adjust seed for the minimum platform supported frequency */
						initial_seed = (uint16_t) (((((uint64_t) initial_seed) *
							fam15h_freq_tab[mem_clk] * 100) / (min_mem_clk * 100)));

						for (lane = 0; lane < lane_count; lane++) {
							uint16_t wl_pass1_delay;
							wl_pass1_delay = current_total_delay[lane];

							seed[lane] = initial_seed + wl_pass1_delay;
						}
					} else {
						uint8_t addr_prelaunch = 0;		/* TODO: Fetch the correct value from RC2[0] */
						uint16_t register_delay;
						int16_t seed_prescaling;

						memcpy(current_total_delay, dqs_ret_pass1_total_delay, sizeof(current_total_delay));
						if ((pDCTstat->Status & (1 << SB_Registered))) {
							if (addr_prelaunch)
								register_delay = 0x30;
							else
								register_delay = 0x20;
						} else if ((pDCTstat->Status & (1 << SB_LoadReduced))) {
							/* TODO
							 * Load reduced DIMM support unimplemented
							 */
							register_delay = 0x0;
						} else {
							register_delay = 0x0;
						}

						for (lane = 0; lane < lane_count; lane++) {
							seed_prescaling = current_total_delay[lane] - register_delay - 0x20;
							seed[lane] = (uint16_t) (register_delay + ((((uint64_t) seed_prescaling) * fam15h_freq_tab[mem_clk] * 100) / (min_mem_clk * 100)));
						}
					}

					for (lane = 0; lane < lane_count; lane++) {
						seed_gross[lane] = (seed[lane] >> 5) & 0x1f;
						seed_fine[lane] = seed[lane] & 0x1f;

						/*if (seed_gross[lane] == 0)
							seed_pre_gross[lane] = 0;
						else */if (seed_gross[lane] & 0x1)
							seed_pre_gross[lane] = 1;
						else
							seed_pre_gross[lane] = 2;

						/* Calculate phase recovery delays */
						phase_recovery_delays[lane] = ((seed_pre_gross[lane] & 0x1f) << 5) | (seed_fine[lane] & 0x1f);

						/* Set the gross delay.
						* NOTE: While the BKDG states to only program DqsRcvEnGrossDelay, this appears
						* to have been a misprint as DqsRcvEnFineDelay should be set to zero as well.
						*/
						current_total_delay[lane] = ((seed_gross[lane] & 0x1f) << 5);
					}

					/* 2.10.5.8.2 (2) / 2.10.5.8.2.1 (5 6)
					 * Program PhRecFineDly and PhRecGrossDly
					 */
					write_dram_phase_recovery_control_registers(phase_recovery_delays, dev, Channel, dimm, index_reg);

					/* 2.10.5.8.2 (2) / 2.10.5.8.2.1 (7)
					 * Program the DQS Receiver Enable delay values for each lane
					 */
					write_dqs_receiver_enable_control_registers(current_total_delay, dev, Channel, dimm, index_reg);

					/* 2.10.5.8.2 (3)
					 * Program DqsRcvTrEn = 1
					 */
					dword = Get_NB32_index_wait_DCT(dev, Channel, index_reg, 0x00000008);
					dword |= (0x1 << 13);
					Set_NB32_index_wait_DCT(dev, Channel, index_reg, 0x00000008, dword);

					/* 2.10.5.8.2 (4)
					 * Issue 192 read requests to the target rank
					 */
					generate_dram_receiver_enable_training_pattern_fam15(pMCTstat, pDCTstat, Channel, Receiver + (rank & 0x1));

					/* 2.10.5.8.2 (5)
					 * Program DqsRcvTrEn = 0
					 */
					dword = Get_NB32_index_wait_DCT(dev, Channel, index_reg, 0x00000008);
					dword &= ~(0x1 << 13);
					Set_NB32_index_wait_DCT(dev, Channel, index_reg, 0x00000008, dword);

					/* 2.10.5.8.2 (6)
					 * Read PhRecGrossDly, PhRecFineDly
					 */
					read_dram_phase_recovery_control_registers(phase_recovery_delays, dev, Channel, dimm, index_reg);

					/* 2.10.5.8.2 (7)
					 * Calculate and program the DQS Receiver Enable delay values
					 */
					for (lane = 0; lane < lane_count; lane++) {
						current_total_delay[lane] = (phase_recovery_delays[lane] & 0x1f);
						current_total_delay[lane] |= ((seed_gross[lane] + ((phase_recovery_delays[lane] >> 5) & 0x1f) - seed_pre_gross[lane] + 1) << 5);
						if (nibble == 1) {
							/* 2.10.5.8.2 (1)
							 * Average the trained values of both nibbles on x4 DIMMs
							 */
							current_total_delay[lane] = (nibble0_current_total_delay[lane] + current_total_delay[lane]) / 2;
						}
					}

#if DQS_TRAIN_DEBUG > 1
					for (lane = 0; lane < 8; lane++)
						printk(BIOS_DEBUG, "\t\tTrainRcvEn55: Channel: %d dimm: %d nibble: %d lane %d current_total_delay: %04x CH_D_B_RCVRDLY: %04x\n",
							Channel, dimm, nibble, lane, current_total_delay[lane], pDCTstat->CH_D_B_RCVRDLY[Channel][dimm][lane]);
#endif
					write_dqs_receiver_enable_control_registers(current_total_delay, dev, Channel, dimm, index_reg);

					if (nibble == 0) {
						/* Back up the Nibble 0 delays for later use */
						memcpy(nibble0_current_total_delay, current_total_delay, sizeof(current_total_delay));
					}

					/* Exit nibble training if current DIMM is not x4 */
					if ((pDCTstat->Dimmx4Present & (1 << (dimm + Channel))) == 0)
						break;
				}

				if (_2Ranks) {
					if (rank == 0) {
						/* Back up the Rank 0 delays for later use */
						memcpy(rank0_current_total_delay, current_total_delay, sizeof(current_total_delay));
					}
					if (rank == 1) {
						/* 2.10.5.8.2 (8)
						 * Compute the average delay across both ranks and program the result into
						 * the DQS Receiver Enable delay registers
						 */
						for (lane = 0; lane < lane_count; lane++) {
							current_total_delay[lane] = (rank0_current_total_delay[lane] + current_total_delay[lane]) / 2;
							if (lane == 8)
								pDCTstat->CH_D_BC_RCVRDLY[Channel][dimm] = current_total_delay[lane];
							else
								pDCTstat->CH_D_B_RCVRDLY[Channel][dimm][lane] = current_total_delay[lane];
						}
						write_dqs_receiver_enable_control_registers(current_total_delay, dev, Channel, dimm, index_reg);
					}
				} else {
					/* Save the current delay for later use by other routines */
					for (lane = 0; lane < lane_count; lane++) {
						if (lane == 8)
							pDCTstat->CH_D_BC_RCVRDLY[Channel][dimm] = current_total_delay[lane];
						else
							pDCTstat->CH_D_B_RCVRDLY[Channel][dimm][lane] = current_total_delay[lane];
					}
				}
			}

#if DQS_TRAIN_DEBUG > 0
			for (lane = 0; lane < 8; lane++)
				print_debug_dqs_pair("\t\tTrainRcvEn56: Lane ", lane, " current_total_delay ", current_total_delay[lane], 2);
#endif
		}
	}

	/* Calculate and program MaxRdLatency for both channels */
	Calc_SetMaxRdLatency_D_Fam15(pMCTstat, pDCTstat, 0, 0);
	Calc_SetMaxRdLatency_D_Fam15(pMCTstat, pDCTstat, 1, 0);

	if (_DisableDramECC) {
		mct_EnableDimmEccEn_D(pMCTstat, pDCTstat, _DisableDramECC);
	}

	if (Pass == FirstPass) {
		/*Disable DQSRcvrEn training mode */
		mct_DisableDQSRcvEn_D(pDCTstat);
	}

	if (!_Wrap32Dis) {
		msr = HWCR;
		_RDMSR(msr, &lo, &hi);
		lo &= ~(1<<17);		/* restore HWCR.wrap32dis */
		_WRMSR(msr, lo, hi);
	}
	if (!_SSE2) {
		cr4 = read_cr4();
		cr4 &= ~(1<<9);		/* restore cr4.OSFXSR */
		write_cr4(cr4);
	}

#if DQS_TRAIN_DEBUG > 0
	{
		u8 ChannelDTD;
		printk(BIOS_DEBUG, "TrainRcvrEn: CH_MaxRdLat:\n");
		for (ChannelDTD = 0; ChannelDTD < 2; ChannelDTD++) {
			printk(BIOS_DEBUG, "Channel:%x: %x\n",
			       ChannelDTD, pDCTstat->CH_MaxRdLat[ChannelDTD][0]);
		}
	}
#endif

#if DQS_TRAIN_DEBUG > 0
	{
		u16 valDTD;
		u8 ChannelDTD, ReceiverDTD;
		u8 i;
		u16 *p;

		printk(BIOS_DEBUG, "TrainRcvrEn: CH_D_B_RCVRDLY:\n");
		for (ChannelDTD = 0; ChannelDTD < 2; ChannelDTD++) {
			printk(BIOS_DEBUG, "Channel:%x\n", ChannelDTD);
			for (ReceiverDTD = 0; ReceiverDTD < 8; ReceiverDTD+=2) {
				printk(BIOS_DEBUG, "\t\tReceiver:%x:", ReceiverDTD);
				p = pDCTstat->CH_D_B_RCVRDLY[ChannelDTD][ReceiverDTD>>1];
				for (i = 0; i < 8; i++) {
					valDTD = p[i];
					printk(BIOS_DEBUG, " %03x", valDTD);
				}
				printk(BIOS_DEBUG, "\n");
			}
		}
	}
#endif

	printk(BIOS_DEBUG, "TrainRcvrEn: Status %x\n", pDCTstat->Status);
	printk(BIOS_DEBUG, "TrainRcvrEn: ErrStatus %x\n", pDCTstat->ErrStatus);
	printk(BIOS_DEBUG, "TrainRcvrEn: ErrCode %x\n", pDCTstat->ErrCode);
	printk(BIOS_DEBUG, "TrainRcvrEn: Done\n\n");
}

static void write_max_read_latency_to_registers(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, uint8_t dct, uint16_t *latency)
{
	uint32_t dword;
	uint8_t nb_pstate;

	for (nb_pstate = 0; nb_pstate < 2; nb_pstate++) {
		dword = Get_NB32_DCT_NBPstate(pDCTstat->dev_dct, dct, nb_pstate, 0x210);
		dword &= ~(0x3ff << 22);
		dword |= ((latency[nb_pstate] & 0x3ff) << 22);
		Set_NB32_DCT_NBPstate(pDCTstat->dev_dct, dct, nb_pstate, 0x210, dword);
	}
}

/* DQS MaxRdLatency Training (Family 15h)
 * Algorithm detailed in:
 * The Fam15h BKDG Rev. 3.14 section 2.10.5.8.5.1
 * This algorithm runs at the highest supported MEMCLK.
 */
void dqsTrainMaxRdLatency_SW_Fam15(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat)
{
	u8 Channel;
	u8 Receiver;
	u8 _DisableDramECC = 0, _Wrap32Dis = 0, _SSE2 = 0;
	u32 Errors;

	u32 dev;
	u32 index_reg;
	u32 ch_start, ch_end;
	u32 msr;
	u32 cr4;
	u32 lo, hi;

	uint32_t dword;
	uint8_t dimm;
	uint8_t lane;
	uint8_t mem_clk;
	uint32_t nb_clk;
	uint8_t nb_pstate;
	uint16_t current_total_delay[MAX_BYTE_LANES];
	uint16_t current_rdqs_total_delay[MAX_BYTE_LANES];
	uint8_t current_worst_case_total_delay_dimm;
	uint16_t current_worst_case_total_delay_value;

	uint8_t lane_count;
	lane_count = get_available_lane_count(pMCTstat, pDCTstat);

	uint16_t fam15h_freq_tab[] = {0, 0, 0, 0, 333, 0, 400, 0, 0, 0, 533, 0, 0, 0, 667, 0, 0, 0, 800, 0, 0, 0, 933};

	print_debug_dqs("\nTrainMaxRdLatency: Node", pDCTstat->Node_ID, 0);

	dev = pDCTstat->dev_dct;
	index_reg = 0x98;
	ch_start = 0;
	ch_end = 2;

	cr4 = read_cr4();
	if (cr4 & (1 << 9)) {	/* save the old value */
		_SSE2 = 1;
	}
	cr4 |= (1 << 9);	/* OSFXSR enable SSE2 */
	write_cr4(cr4);

	msr = HWCR;
	_RDMSR(msr, &lo, &hi);
	/* FIXME: Why use SSEDIS */
	if (lo & (1 << 17)) {	/* save the old value */
		_Wrap32Dis = 1;
	}
	lo |= (1 << 17);	/* HWCR.wrap32dis */
	lo &= ~(1 << 15);	/* SSEDIS */
	_WRMSR(msr, lo, hi);	/* Setting wrap32dis allows 64-bit memory references in real mode */

	_DisableDramECC = mct_DisableDimmEccEn_D(pMCTstat, pDCTstat);

	Errors = 0;
	dev = pDCTstat->dev_dct;

	for (Channel = 0; Channel < 2; Channel++) {
		print_debug_dqs("\tTrainMaxRdLatency51: Node ", pDCTstat->Node_ID, 1);
		print_debug_dqs("\tTrainMaxRdLatency51: Channel ", Channel, 1);
		pDCTstat->Channel = Channel;

		if (pDCTstat->DIMMValidDCT[Channel] == 0)
			continue;

		mem_clk = Get_NB32_DCT(dev, Channel, 0x94) & 0x1f;

		Receiver = mct_InitReceiver_D(pDCTstat, Channel);

		/* Find DIMM with worst case receiver enable delays */
		current_worst_case_total_delay_dimm = 0;
		current_worst_case_total_delay_value = 0;

		/* There are four receiver pairs, loosely associated with chipselects.
		 * This is essentially looping over each DIMM.
		 */
		for (; Receiver < 8; Receiver += 2) {
			dimm = (Receiver >> 1);

			print_debug_dqs("\t\tTrainMaxRdLatency52: Receiver ", Receiver, 2);

			if (!mct_RcvrRankEnabled_D(pMCTstat, pDCTstat, Channel, Receiver)) {
				continue;
			}

			/* Retrieve the total delay values from pass 1 of DQS receiver enable training */
			read_dqs_receiver_enable_control_registers(current_total_delay, dev, Channel, dimm, index_reg);
			read_dqs_read_data_timing_registers(current_rdqs_total_delay, dev, Channel, dimm, index_reg);

			for (lane = 0; lane < lane_count; lane++) {
				current_total_delay[lane] += current_rdqs_total_delay[lane];
				if (current_total_delay[lane] > current_worst_case_total_delay_value) {
					current_worst_case_total_delay_dimm = dimm;
					current_worst_case_total_delay_value = current_total_delay[lane];
				}
			}

#if DQS_TRAIN_DEBUG > 0
			for (lane = 0; lane < lane_count; lane++)
				print_debug_dqs_pair("\t\tTrainMaxRdLatency56: Lane ", lane, " current_total_delay ", current_total_delay[lane], 2);
#endif
		}

		/* 2.10.5.8.5.1.1 */
		Calc_SetMaxRdLatency_D_Fam15(pMCTstat, pDCTstat, Channel, 1);

		/* 2.10.5.8.5.1.[2,3]
		 * Write the DRAM training pattern to the test address
		 */
		write_dram_dqs_training_pattern_fam15(pMCTstat, pDCTstat, Channel, current_worst_case_total_delay_dimm << 1, 0xff, 0);

		/* 2.10.5.8.5.1.4
		 * Incrementally test each MaxRdLatency candidate
		 */
		for (; pDCTstat->CH_MaxRdLat[Channel][0] < 0x3ff; pDCTstat->CH_MaxRdLat[Channel][0]++) {
			write_max_read_latency_to_registers(pMCTstat, pDCTstat, Channel, pDCTstat->CH_MaxRdLat[Channel]);
			read_dram_dqs_training_pattern_fam15(pMCTstat, pDCTstat, Channel, current_worst_case_total_delay_dimm << 1, 0xff, 0);
			dword = Get_NB32_DCT(dev, Channel, 0x268) & 0x3ffff;
			if (!dword)
				break;
			Set_NB32_index_wait_DCT(dev, Channel, index_reg, 0x00000050, 0x13131313);
		}
		dword = Get_NB32_DCT(dev, Channel, 0x268) & 0x3ffff;
		if (dword)
			printk(BIOS_ERR, "WARNING: MaxRdLatency training FAILED!  Attempting to continue but your system may be unstable...\n");

		/* 2.10.5.8.5.1.5 */
		nb_pstate = 0;
		mem_clk = Get_NB32_DCT(dev, Channel, 0x94) & 0x1f;
		if (fam15h_freq_tab[mem_clk] == 0) {
			return;
		}
		dword = Get_NB32(pDCTstat->dev_nbctl, (0x160 + (nb_pstate * 4)));		/* Retrieve NbDid, NbFid */
		nb_clk = (200 * (((dword >> 1) & 0x1f) + 0x4)) / (((dword >> 7) & 0x1)?2:1);

		pDCTstat->CH_MaxRdLat[Channel][0]++;
		pDCTstat->CH_MaxRdLat[Channel][0] += ((((uint64_t)15 * 100000000000ULL) / ((uint64_t)fam15h_freq_tab[mem_clk] * 1000000ULL))
							 * ((uint64_t)nb_clk * 1000)) / 1000000000ULL;

		write_max_read_latency_to_registers(pMCTstat, pDCTstat, Channel, pDCTstat->CH_MaxRdLat[Channel]);
	}

	if (_DisableDramECC) {
		mct_EnableDimmEccEn_D(pMCTstat, pDCTstat, _DisableDramECC);
	}

	if (!_Wrap32Dis) {
		msr = HWCR;
		_RDMSR(msr, &lo, &hi);
		lo &= ~(1<<17);		/* restore HWCR.wrap32dis */
		_WRMSR(msr, lo, hi);
	}
	if (!_SSE2) {
		cr4 = read_cr4();
		cr4 &= ~(1<<9);		/* restore cr4.OSFXSR */
		write_cr4(cr4);
	}

#if DQS_TRAIN_DEBUG > 0
	{
		u8 ChannelDTD;
		printk(BIOS_DEBUG, "TrainMaxRdLatency: CH_MaxRdLat:\n");
		for (ChannelDTD = 0; ChannelDTD < 2; ChannelDTD++) {
			printk(BIOS_DEBUG, "Channel:%x: %x\n",
			       ChannelDTD, pDCTstat->CH_MaxRdLat[ChannelDTD][0]);
		}
	}
#endif

	printk(BIOS_DEBUG, "TrainMaxRdLatency: Status %x\n", pDCTstat->Status);
	printk(BIOS_DEBUG, "TrainMaxRdLatency: ErrStatus %x\n", pDCTstat->ErrStatus);
	printk(BIOS_DEBUG, "TrainMaxRdLatency: ErrCode %x\n", pDCTstat->ErrCode);
	printk(BIOS_DEBUG, "TrainMaxRdLatency: Done\n\n");
}

u8 mct_InitReceiver_D(struct DCTStatStruc *pDCTstat, u8 dct)
{
	if (pDCTstat->DIMMValidDCT[dct] == 0) {
		return 8;
	} else {
		return 0;
	}
}

static void mct_DisableDQSRcvEn_D(struct DCTStatStruc *pDCTstat)
{
	u8 ch_end, ch;
	u32 reg;
	u32 dev;
	u32 val;

	dev = pDCTstat->dev_dct;
	if (pDCTstat->GangedMode) {
		ch_end = 1;
	} else {
		ch_end = 2;
	}

	for (ch = 0; ch < ch_end; ch++) {
		reg = 0x78;
		val = Get_NB32_DCT(dev, ch, reg);
		val &= ~(1 << DqsRcvEnTrain);
		Set_NB32_DCT(dev, ch, reg, val);
	}
}

/* mct_ModifyIndex_D
 * Function only used once so it was inlined.
 */

/* mct_GetInitFlag_D
 * Function only used once so it was inlined.
 */

/* Set F2x[1, 0]9C_x[2B:10] DRAM DQS Receiver Enable Timing Control Registers
 * See BKDG Rev. 3.62 page 268 for more information
 */
void mct_SetRcvrEnDly_D(struct DCTStatStruc *pDCTstat, u16 RcvrEnDly,
			u8 FinalValue, u8 Channel, u8 Receiver, u32 dev,
			u32 index_reg, u8 Addl_Index, u8 Pass)
{
	u32 index;
	u8 i;
	u16 *p;
	u32 val;

	if (RcvrEnDly == 0x1fe) {
		/*set the boundary flag */
		pDCTstat->Status |= 1 << SB_DQSRcvLimit;
	}

	/* DimmOffset not needed for CH_D_B_RCVRDLY array */
	for (i = 0; i < 8; i++) {
		if (FinalValue) {
			/*calculate dimm offset */
			p = pDCTstat->CH_D_B_RCVRDLY[Channel][Receiver >> 1];
			RcvrEnDly = p[i];
		}

		/* if flag = 0, set DqsRcvEn value to reg. */
		/* get the register index from table */
		index = Table_DQSRcvEn_Offset[i >> 1];
		index += Addl_Index;	/* DIMMx DqsRcvEn byte0 */
		val = Get_NB32_index_wait_DCT(dev, Channel, index_reg, index);
		if (i & 1) {
			/* odd byte lane */
			val &= ~(0x1ff << 16);
			val |= ((RcvrEnDly & 0x1ff) << 16);
		} else {
			/* even byte lane */
			val &= ~0x1ff;
			val |= (RcvrEnDly & 0x1ff);
		}
		Set_NB32_index_wait_DCT(dev, Channel, index_reg, index, val);
	}

}

/* Calculate MaxRdLatency
 * Algorithm detailed in the Fam10h BKDG Rev. 3.62 section 2.8.9.9.5
 */
static void mct_SetMaxLatency_D(struct DCTStatStruc *pDCTstat, u8 Channel, u16 DQSRcvEnDly)
{
	u32 dev;
	u32 reg;
	u32 SubTotal;
	u32 index_reg;
	u32 val;

	uint8_t cpu_val_n;
	uint8_t cpu_val_p;

	u16 freq_tab[] = {400, 533, 667, 800};

	/* Set up processor-dependent values */
	if (pDCTstat->LogicalCPUID & AMD_DR_Dx) {
		/* Revision D and above */
		cpu_val_n = 4;
		cpu_val_p = 29;
	} else if (pDCTstat->LogicalCPUID & AMD_DR_Cx) {
		/* Revision C */
		uint8_t package_type = mctGet_NVbits(NV_PACK_TYPE);
		if ((package_type == PT_L1)		/* Socket F (1207) */
			|| (package_type == PT_M2)	/* Socket AM3 */
			|| (package_type == PT_S1)) {	/* Socket S1g <x> */
			cpu_val_n = 10;
			cpu_val_p = 11;
		} else {
			cpu_val_n = 4;
			cpu_val_p = 29;
		}
	} else {
		/* Revision B and below */
		cpu_val_n = 10;
		cpu_val_p = 11;
	}

	if (pDCTstat->GangedMode)
		Channel = 0;

	dev = pDCTstat->dev_dct;
	index_reg = 0x98;

	/* Multiply the CAS Latency by two to get a number of 1/2 MEMCLKs units.*/
	val = Get_NB32_DCT(dev, Channel, 0x88);
	SubTotal = ((val & 0x0f) + 4) << 1;	/* SubTotal is 1/2 Memclk unit */

	/* If registered DIMMs are being used then
	 *  add 1 MEMCLK to the sub-total.
	 */
	val = Get_NB32_DCT(dev, Channel, 0x90);
	if (!(val & (1 << UnBuffDimm)))
		SubTotal += 2;

	/* If the address prelaunch is setup for 1/2 MEMCLKs then
	 *  add 1, else add 2 to the sub-total.
	 *  if (AddrCmdSetup || CsOdtSetup || CkeSetup) then K := K + 2;
	 */
	val = Get_NB32_index_wait_DCT(dev, Channel, index_reg, 0x04);
	if (!(val & 0x00202020))
		SubTotal += 1;
	else
		SubTotal += 2;

	/* If the F2x[1, 0]78[RdPtrInit] field is 4, 5, 6 or 7 MEMCLKs,
	 * then add 4, 3, 2, or 1 MEMCLKs, respectively to the sub-total. */
	val = Get_NB32_DCT(dev, Channel, 0x78);
	SubTotal += 8 - (val & 0x0f);

	/* Convert bits 7-5 (also referred to as the coarse delay) of
	 * the current (or worst case) DQS receiver enable delay to
	 * 1/2 MEMCLKs units, rounding up, and add this to the sub-total.
	 */
	SubTotal += DQSRcvEnDly >> 5;	/* Retrieve gross delay portion of value */

	/* Add "P" to the sub-total. "P" represents part of the
	 * processor specific constant delay value in the DRAM
	 * clock domain.
	 */
	SubTotal <<= 1;		/*scale 1/2 MemClk to 1/4 MemClk */
	SubTotal += cpu_val_p;	/*add "P" 1/2MemClk */
	SubTotal >>= 1;		/*scale 1/4 MemClk back to 1/2 MemClk */

	/* Convert the sub-total (in 1/2 MEMCLKs) to northbridge
	 * clocks (NCLKs)
	 */
	SubTotal *= 200 * ((Get_NB32(pDCTstat->dev_nbmisc, 0xd4) & 0x1f) + 4);
	SubTotal /= freq_tab[((Get_NB32_DCT(pDCTstat->dev_dct, Channel, 0x94) & 0x7) - 3)];
	SubTotal = (SubTotal + (2 - 1)) / 2;	/* Round up */

	/* Add "N" NCLKs to the sub-total. "N" represents part of the
	 * processor specific constant value in the northbridge
	 * clock domain.
	 */
	SubTotal += (cpu_val_n) / 2;

	pDCTstat->CH_MaxRdLat[Channel][0] = SubTotal;
	if (pDCTstat->GangedMode) {
		pDCTstat->CH_MaxRdLat[1][0] = SubTotal;
	}

	/* Program the F2x[1, 0]78[MaxRdLatency] register with
	 * the total delay value (in NCLKs).
	 */
	reg = 0x78;
	val = Get_NB32_DCT(dev, Channel, reg);
	val &= ~(0x3ff << 22);
	val |= (SubTotal & 0x3ff) << 22;

	/* program MaxRdLatency to correspond with current delay */
	Set_NB32_DCT(dev, Channel, reg, val);
}

static void mct_InitDQSPos4RcvrEn_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat)
{
	/* Initialize the DQS Positions in preparation for
	 * Receiver Enable Training.
	 * Write Position is 1/2 Memclock Delay
	 * Read Position is 1/2 Memclock Delay
	 */
	u8 i;
	for (i = 0; i < 2; i++) {
		InitDQSPos4RcvrEn_D(pMCTstat, pDCTstat, i);
	}
}

static void InitDQSPos4RcvrEn_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 Channel)
{
	/* Initialize the DQS Positions in preparation for
	 * Receiver Enable Training.
	 * Write Position is no Delay
	 * Read Position is 1/2 Memclock Delay
	 */

	u8 i, j;
	u32 dword;
	u8 dn = 4; /* TODO: Rev C could be 4 */
	u32 dev = pDCTstat->dev_dct;
	u32 index_reg = 0x98;

	/* FIXME: add Cx support */
	dword = 0x00000000;
	for (i = 1; i <= 3; i++) {
		for (j = 0; j < dn; j++)
			/* DIMM0 Write Data Timing Low */
			/* DIMM0 Write ECC Timing */
			Set_NB32_index_wait_DCT(dev, Channel, index_reg, i + 0x100 * j, dword);
	}

	/* errata #180 */
	dword = 0x2f2f2f2f;
	for (i = 5; i <= 6; i++) {
		for (j = 0; j < dn; j++)
			/* DIMM0 Read DQS Timing Control Low */
			Set_NB32_index_wait_DCT(dev, Channel, index_reg, i + 0x100 * j, dword);
	}

	dword = 0x0000002f;
	for (j = 0; j < dn; j++)
		/* DIMM0 Read DQS ECC Timing Control */
		Set_NB32_index_wait_DCT(dev, Channel, index_reg, 7 + 0x100 * j, dword);
}

void SetEccDQSRcvrEn_D(struct DCTStatStruc *pDCTstat, u8 Channel)
{
	u32 dev;
	u32 index_reg;
	u32 index;
	u8 ChipSel;
	u16 *p;
	u32 val;

	dev = pDCTstat->dev_dct;
	index_reg = 0x98;
	index = 0x12;
	p = pDCTstat->CH_D_BC_RCVRDLY[Channel];
	print_debug_dqs("\t\tSetEccDQSRcvrPos: Channel ", Channel,  2);
	for (ChipSel = 0; ChipSel < MAX_CS_SUPPORTED; ChipSel += 2) {
		val = p[ChipSel>>1];
		Set_NB32_index_wait_DCT(dev, Channel, index_reg, index, val);
		print_debug_dqs_pair("\t\tSetEccDQSRcvrPos: ChipSel ",
					ChipSel, " rcvr_delay ",  val, 2);
		index += 3;
	}
}

static void CalcEccDQSRcvrEn_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 Channel)
{
	u8 ChipSel;
	u16 EccDQSLike;
	u8 EccDQSScale;
	u32 val, val0, val1;
	int16_t delay_differential;

	EccDQSLike = pDCTstat->CH_EccDQSLike[Channel];
	EccDQSScale = pDCTstat->CH_EccDQSScale[Channel];

	for (ChipSel = 0; ChipSel < MAX_CS_SUPPORTED; ChipSel += 2) {
		if (mct_RcvrRankEnabled_D(pMCTstat, pDCTstat, Channel, ChipSel)) {
			u16 *p;
			p = pDCTstat->CH_D_B_RCVRDLY[Channel][ChipSel>>1];

			if (pDCTstat->Status & (1 << SB_Registered)) {
				val0 = p[0x2];
				val1 = p[0x3];

				delay_differential = (int16_t)val1 - (int16_t)val0;
				delay_differential += (int16_t)val1;

				val = delay_differential;
			} else {
				/* DQS Delay Value of Data Bytelane
				 * most like ECC byte lane */
				val0 = p[EccDQSLike & 0x07];
				/* DQS Delay Value of Data Bytelane
				 * 2nd most like ECC byte lane */
				val1 = p[(EccDQSLike>>8) & 0x07];

				if (val0 > val1) {
					val = val0 - val1;
				} else {
					val = val1 - val0;
				}

				val *= ~EccDQSScale;
				val >>= 8; /* /256 */

				if (val0 > val1) {
					val -= val1;
				} else {
					val += val0;
				}
			}

			pDCTstat->CH_D_BC_RCVRDLY[Channel][ChipSel>>1] = val;
		}
	}
	SetEccDQSRcvrEn_D(pDCTstat, Channel);
}

/* 2.8.9.9.4
 * ECC Byte Lane Training
 * DQS Receiver Enable Delay
 */
void mctSetEccDQSRcvrEn_D(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstatA)
{
	u8 Node;
	u8 i;

	for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
		struct DCTStatStruc *pDCTstat;
		pDCTstat = pDCTstatA + Node;
		if (!pDCTstat->NodePresent)
			break;
		if (pDCTstat->DCTSysLimit) {
			for (i = 0; i < 2; i++)
				CalcEccDQSRcvrEn_D(pMCTstat, pDCTstat, i);
		}
	}
}

void phyAssistedMemFnceTraining(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstatA, int16_t single_node_number)
{
	u8 Node = 0;
	struct DCTStatStruc *pDCTstat;

	printk(BIOS_DEBUG, "%s: Start\n", __func__);

	uint8_t start_node = 0;
	uint8_t end_node = MAX_NODES_SUPPORTED;

	if (single_node_number >= 0) {
		start_node = single_node_number;
		end_node = single_node_number + 1;
	}

	/* FIXME: skip for Ax */
	for (Node = start_node; Node < end_node; Node++) {
		pDCTstat = pDCTstatA + Node;
		if (!pDCTstat->NodePresent)
			continue;

		if (pDCTstat->DCTSysLimit) {
			if (is_fam15h()) {
				/* Fam15h BKDG v3.14 section 2.10.5.3.3
				 * This picks up where InitDDRPhy left off
				 */
				uint8_t dct;
				uint8_t index;
				uint32_t dword;
				uint32_t datc_backup;
				uint32_t training_dword;
				uint32_t fence2_config_dword;
				uint32_t fence_tx_pad_config_dword;
				uint32_t index_reg = 0x98;
				uint32_t dev = pDCTstat->dev_dct;

				for (dct = 0; dct < 2; dct++) {
					if (!pDCTstat->DIMMValidDCT[dct])
						continue;

					printk(BIOS_SPEW, "%s: training node %d DCT %d\n", __func__, Node, dct);

					/* Back up D18F2x9C_x0000_0004_dct[1:0] */
					datc_backup = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x00000004);

					/* FenceTrSel = 0x2 */
					dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x00000008);
					dword &= ~(0x3 << 6);
					dword |= (0x2 << 6);
					Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x00000008, dword);

					/* Set phase recovery seed values */
					Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x00000050, 0x13131313);
					Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x00000051, 0x13131313);
					Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x00000052, 0x00000013);

					training_dword = fenceDynTraining_D(pMCTstat, pDCTstat, dct);

					/* Save calculated fence value to the TX DLL */
					dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0000000c);
					dword &= ~(0x1f << 26);
					dword |= ((training_dword & 0x1f) << 26);
					Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0000000c, dword);

					/* D18F2x9C_x0D0F_0[F,8:0]0F_dct[1:0][AlwaysEnDllClks]=0x1 */
					for (index = 0; index < 0x9; index++) {
						dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f000f | (index << 8));
						dword &= ~(0x7 << 12);
						dword |= (0x1 << 12);
						Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f000f | (index << 8), dword);
					}

					/* FenceTrSel = 0x1 */
					dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x00000008);
					dword &= ~(0x3 << 6);
					dword |= (0x1 << 6);
					Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x00000008, dword);

					/* Set phase recovery seed values */
					Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x00000050, 0x13131313);
					Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x00000051, 0x13131313);
					Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x00000052, 0x00000013);

					training_dword = fenceDynTraining_D(pMCTstat, pDCTstat, dct);

					/* Save calculated fence value to the RX DLL */
					dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0000000c);
					dword &= ~(0x1f << 21);
					dword |= ((training_dword & 0x1f) << 21);
					Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0000000c, dword);

					/* D18F2x9C_x0D0F_0[F,8:0]0F_dct[1:0][AlwaysEnDllClks]=0x0 */
					for (index = 0; index < 0x9; index++) {
						dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f000f | (index << 8));
						dword &= ~(0x7 << 12);
						Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f000f | (index << 8), dword);
					}

					/* FenceTrSel = 0x3 */
					dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x00000008);
					dword &= ~(0x3 << 6);
					dword |= (0x3 << 6);
					Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x00000008, dword);

					/* Set phase recovery seed values */
					Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x00000050, 0x13131313);
					Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x00000051, 0x13131313);
					Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x00000052, 0x00000013);

					fence_tx_pad_config_dword = fenceDynTraining_D(pMCTstat, pDCTstat, dct);

					/* Save calculated fence value to the TX Pad */
					dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0000000c);
					dword &= ~(0x1f << 16);
					dword |= ((fence_tx_pad_config_dword & 0x1f) << 16);
					Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0000000c, dword);

					/* Program D18F2x9C_x0D0F_[C,8,2][2:0]31_dct[1:0] */
					training_dword = fence_tx_pad_config_dword;
					if (fence_tx_pad_config_dword < 16)
						training_dword |= (0x1 << 4);
					else
						training_dword = 0;
					for (index = 0; index < 0x3; index++) {
						dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f2031 | (index << 8));
						dword &= ~(0x1f);
						dword |= (training_dword & 0x1f);
						Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f2031 | (index << 8), dword);
					}
					for (index = 0; index < 0x3; index++) {
						dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f8031 | (index << 8));
						dword &= ~(0x1f);
						dword |= (training_dword & 0x1f);
						Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f8031 | (index << 8), dword);
					}
					for (index = 0; index < 0x3; index++) {
						dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0fc031 | (index << 8));
						dword &= ~(0x1f);
						dword |= (training_dword & 0x1f);
						Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0fc031 | (index << 8), dword);
					}

					/* Assemble Fence2 configuration word (Fam15h BKDG v3.14 page 331) */
					dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0000000c);
					fence2_config_dword = 0;

					/* TxPad */
					training_dword = (dword >> 16) & 0x1f;
					if (training_dword < 16)
						training_dword |= 0x10;
					else
						training_dword = 0;
					fence2_config_dword |= training_dword;

					/* RxDll */
					training_dword = (dword >> 21) & 0x1f;
					if (training_dword < 16)
						training_dword |= 0x10;
					else
						training_dword = 0;
					fence2_config_dword |= (training_dword << 10);

					/* TxDll */
					training_dword = (dword >> 26) & 0x1f;
					if (training_dword < 16)
						training_dword |= 0x10;
					else
						training_dword = 0;
					fence2_config_dword |= (training_dword << 5);

					/* Program D18F2x9C_x0D0F_0[F,8:0]31_dct[1:0] */
					for (index = 0; index < 0x9; index++) {
						dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f0031 | (index << 8));
						dword &= ~(0x7fff);
						dword |= fence2_config_dword;
						Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f0031 | (index << 8), dword);
					}

					/* Restore D18F2x9C_x0000_0004_dct[1:0] */
					Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x00000004, datc_backup);

					printk(BIOS_SPEW, "%s: done training node %d DCT %d\n", __func__, Node, dct);
				}
			} else {
				fenceDynTraining_D(pMCTstat, pDCTstat, 0);
				fenceDynTraining_D(pMCTstat, pDCTstat, 1);
			}
		}
	}

	printk(BIOS_DEBUG, "%s: Done\n", __func__);
}

uint32_t fenceDynTraining_D(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, uint8_t dct)
{
	u16 avRecValue;
	u32 val;
	u32 dev;
	u32 index_reg = 0x98;
	u32 index;

	dev = pDCTstat->dev_dct;

	if (is_fam15h()) {
		/* Set F2x[1,0]9C_x08[PhyFenceTrEn] */
		val = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x08);
		val |= 1 << PhyFenceTrEn;
		Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x08, val);

		/* Wait 2000 MEMCLKs */
		precise_memclk_delay_fam15(pMCTstat, pDCTstat, dct, 2000);

		/* Clear F2x[1,0]9C_x08[PhyFenceTrEn] */
		val = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x08);
		val &= ~(1 << PhyFenceTrEn);
		Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x08, val);

		/* BIOS reads the phase recovery engine registers
		 * F2x[1,0]9C_x[51:50] and F2x[1,0]9C_x52.
		 * Average the fine delay components only.
		 */
		avRecValue = 0;
		for (index = 0x50; index <= 0x52; index++) {
			val = Get_NB32_index_wait_DCT(dev, dct, index_reg, index);
			avRecValue += val & 0x1f;
			if (index != 0x52) {
				avRecValue += (val >> 8) & 0x1f;
				avRecValue += (val >> 16) & 0x1f;
				avRecValue += (val >> 24) & 0x1f;
			}
		}

		val = avRecValue / 9;
		if (avRecValue % 9)
			val++;
		avRecValue = val;

		if (avRecValue < 6)
			avRecValue = 0;
		else
			avRecValue -= 6;

		return avRecValue;
	} else {
		/* BIOS first programs a seed value to the phase recovery engine
		 *  (recommended 19) registers.
		 * Dram Phase Recovery Control Register (F2x[1,0]9C_x[51:50] and
		 * F2x[1,0]9C_x52.) .
		 */
		for (index = 0x50; index <= 0x52; index ++) {
			val = (FenceTrnFinDlySeed & 0x1F);
			if (index != 0x52) {
				val |= val << 8 | val << 16 | val << 24;
			}
			Set_NB32_index_wait_DCT(dev, dct, index_reg, index, val);
		}

		/* Set F2x[1,0]9C_x08[PhyFenceTrEn]=1. */
		val = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x08);
		val |= 1 << PhyFenceTrEn;
		Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x08, val);

		/* Wait 200 MEMCLKs. */
		mct_Wait(50000);		/* wait 200us */

		/* Clear F2x[1,0]9C_x08[PhyFenceTrEn]=0. */
		val = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x08);
		val &= ~(1 << PhyFenceTrEn);
		Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x08, val);

		/* BIOS reads the phase recovery engine registers
		 * F2x[1,0]9C_x[51:50] and F2x[1,0]9C_x52. */
		avRecValue = 0;
		for (index = 0x50; index <= 0x52; index ++) {
			val = Get_NB32_index_wait_DCT(dev, dct, index_reg, index);
			avRecValue += val & 0x7F;
			if (index != 0x52) {
				avRecValue += (val >> 8) & 0x7F;
				avRecValue += (val >> 16) & 0x7F;
				avRecValue += (val >> 24) & 0x7F;
			}
		}

		val = avRecValue / 9;
		if (avRecValue % 9)
			val++;
		avRecValue = val;

		/* Write the (averaged value -8) to F2x[1,0]9C_x0C[PhyFence]. */
		/* inlined mct_AdjustFenceValue() */
		/* TODO: The RBC0 is not supported. */
		/* if (pDCTstat->LogicalCPUID & AMD_RB_C0)
			avRecValue -= 3;
		else
		*/
		if (pDCTstat->LogicalCPUID & AMD_DR_Dx)
			avRecValue -= 8;
		else if (pDCTstat->LogicalCPUID & AMD_DR_Cx)
			avRecValue -= 8;
		else if (pDCTstat->LogicalCPUID & AMD_DR_Bx)
			avRecValue -= 8;

		val = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0C);
		val &= ~(0x1F << 16);
		val |= (avRecValue & 0x1F) << 16;
		Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0C, val);

		/* Rewrite F2x[1,0]9C_x04-DRAM Address/Command Timing Control Register
		 * delays (both channels).
		 */
		val = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x04);
		Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x04, val);

		return avRecValue;
	}
}

void mct_Wait(u32 cycles)
{
	u32 saved;
	u32 hi, lo, msr;

	/* Wait # of 50ns cycles
	   This seems like a hack to me...  */

	cycles <<= 3;		/* x8 (number of 1.25ns ticks) */

	msr = 0x10;			/* TSC */
	_RDMSR(msr, &lo, &hi);
	saved = lo;
	do {
		_RDMSR(msr, &lo, &hi);
	} while (lo - saved < cycles);
}
