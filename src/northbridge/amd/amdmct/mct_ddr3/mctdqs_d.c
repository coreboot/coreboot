/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
 * Copyright (C) 2015 - 2016 Raptor Engineering, LLC
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

#include <inttypes.h>
#include <console/console.h>
#include <string.h>
#include "mct_d.h"
#include "mct_d_gcc.h"
#include <cpu/amd/mtrr.h>

static void CalcEccDQSPos_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u16 like,
				u8 scale, u8 ChipSel);
static void GetDQSDatStrucVal_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 ChipSel);
static void WriteDQSTestPattern_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat,
					u32 TestAddr_lo);
static void WriteL18TestPattern_D(struct DCTStatStruc *pDCTstat,
					u32 TestAddr_lo);
static void WriteL9TestPattern_D(struct DCTStatStruc *pDCTstat,
					u32 TestAddr_lo);
static u16 CompareDQSTestPattern_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat,
					u32 addr_lo);
static void FlushDQSTestPattern_D(struct DCTStatStruc *pDCTstat,
					u32 addr_lo);
static void mct_SetDQSDelayCSR_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat,
					u8 ChipSel);
static void SetupDqsPattern_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat,
				u32 *buffer);

static void StoreDQSDatStrucVal_D(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat, u8 ChipSel);

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

#define DQS_TRAIN_DEBUG 0
// #define PRINT_PASS_FAIL_BITMAPS 1

void print_debug_dqs(const char *str, u32 val, u8 level)
{
#if DQS_TRAIN_DEBUG > 0
	if (DQS_TRAIN_DEBUG >= level) {
		printk(BIOS_DEBUG, "%s%x\n", str, val);
	}
#endif
}

void print_debug_dqs_pair(const char *str, u32 val, const char *str2, u32 val2, u8 level)
{
#if DQS_TRAIN_DEBUG > 0
	if (DQS_TRAIN_DEBUG >= level) {
		printk(BIOS_DEBUG, "%s%08x%s%08x\n", str, val, str2, val2);
	}
#endif
}

/*Warning:  These must be located so they do not cross a logical 16-bit segment boundary!*/
static const u32 TestPatternJD1a_D[] = {
	0x00000000,0x00000000,0xFFFFFFFF,0xFFFFFFFF, /* QW0-1, ALL-EVEN */
	0x00000000,0x00000000,0x00000000,0x00000000, /* QW2-3, ALL-EVEN */
	0x00000000,0x00000000,0xFFFFFFFF,0xFFFFFFFF, /* QW4-5, ALL-EVEN */
	0x00000000,0x00000000,0x00000000,0x00000000, /* QW6-7, ALL-EVEN */
	0xFeFeFeFe,0xFeFeFeFe,0x01010101,0x01010101, /* QW0-1, DQ0-ODD */
	0xFeFeFeFe,0xFeFeFeFe,0x01010101,0x01010101, /* QW2-3, DQ0-ODD */
	0x01010101,0x01010101,0xFeFeFeFe,0xFeFeFeFe, /* QW4-5, DQ0-ODD */
	0xFeFeFeFe,0xFeFeFeFe,0x01010101,0x01010101, /* QW6-7, DQ0-ODD */
	0x02020202,0x02020202,0x02020202,0x02020202, /* QW0-1, DQ1-ODD */
	0xFdFdFdFd,0xFdFdFdFd,0xFdFdFdFd,0xFdFdFdFd, /* QW2-3, DQ1-ODD */
	0xFdFdFdFd,0xFdFdFdFd,0x02020202,0x02020202, /* QW4-5, DQ1-ODD */
	0x02020202,0x02020202,0x02020202,0x02020202, /* QW6-7, DQ1-ODD */
	0x04040404,0x04040404,0xfBfBfBfB,0xfBfBfBfB, /* QW0-1, DQ2-ODD */
	0x04040404,0x04040404,0x04040404,0x04040404, /* QW2-3, DQ2-ODD */
	0xfBfBfBfB,0xfBfBfBfB,0xfBfBfBfB,0xfBfBfBfB, /* QW4-5, DQ2-ODD */
	0xfBfBfBfB,0xfBfBfBfB,0xfBfBfBfB,0xfBfBfBfB, /* QW6-7, DQ2-ODD */
	0x08080808,0x08080808,0xF7F7F7F7,0xF7F7F7F7, /* QW0-1, DQ3-ODD */
	0x08080808,0x08080808,0x08080808,0x08080808, /* QW2-3, DQ3-ODD */
	0xF7F7F7F7,0xF7F7F7F7,0x08080808,0x08080808, /* QW4-5, DQ3-ODD */
	0xF7F7F7F7,0xF7F7F7F7,0xF7F7F7F7,0xF7F7F7F7, /* QW6-7, DQ3-ODD */
	0x10101010,0x10101010,0x10101010,0x10101010, /* QW0-1, DQ4-ODD */
	0xeFeFeFeF,0xeFeFeFeF,0x10101010,0x10101010, /* QW2-3, DQ4-ODD */
	0xeFeFeFeF,0xeFeFeFeF,0xeFeFeFeF,0xeFeFeFeF, /* QW4-5, DQ4-ODD */
	0xeFeFeFeF,0xeFeFeFeF,0x10101010,0x10101010, /* QW6-7, DQ4-ODD */
	0xdFdFdFdF,0xdFdFdFdF,0xdFdFdFdF,0xdFdFdFdF, /* QW0-1, DQ5-ODD */
	0xdFdFdFdF,0xdFdFdFdF,0x20202020,0x20202020, /* QW2-3, DQ5-ODD */
	0xdFdFdFdF,0xdFdFdFdF,0xdFdFdFdF,0xdFdFdFdF, /* QW4-5, DQ5-ODD */
	0xdFdFdFdF,0xdFdFdFdF,0xdFdFdFdF,0xdFdFdFdF, /* QW6-7, DQ5-ODD */
	0xBfBfBfBf,0xBfBfBfBf,0xBfBfBfBf,0xBfBfBfBf, /* QW0-1, DQ6-ODD */
	0x40404040,0x40404040,0xBfBfBfBf,0xBfBfBfBf, /* QW2-3, DQ6-ODD */
	0x40404040,0x40404040,0xBfBfBfBf,0xBfBfBfBf, /* QW4-5, DQ6-ODD */
	0x40404040,0x40404040,0xBfBfBfBf,0xBfBfBfBf, /* QW6-7, DQ6-ODD */
	0x80808080,0x80808080,0x7F7F7F7F,0x7F7F7F7F, /* QW0-1, DQ7-ODD */
	0x80808080,0x80808080,0x7F7F7F7F,0x7F7F7F7F, /* QW2-3, DQ7-ODD */
	0x80808080,0x80808080,0x7F7F7F7F,0x7F7F7F7F, /* QW4-5, DQ7-ODD */
	0x80808080,0x80808080,0x80808080,0x80808080  /* QW6-7, DQ7-ODD */
};
static const u32 TestPatternJD1b_D[] = {
	0x00000000,0x00000000,0x00000000,0x00000000, /* QW0,CHA-B, ALL-EVEN */
	0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF, /* QW1,CHA-B, ALL-EVEN */
	0x00000000,0x00000000,0x00000000,0x00000000, /* QW2,CHA-B, ALL-EVEN */
	0x00000000,0x00000000,0x00000000,0x00000000, /* QW3,CHA-B, ALL-EVEN */
	0x00000000,0x00000000,0x00000000,0x00000000, /* QW4,CHA-B, ALL-EVEN */
	0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF, /* QW5,CHA-B, ALL-EVEN */
	0x00000000,0x00000000,0x00000000,0x00000000, /* QW6,CHA-B, ALL-EVEN */
	0x00000000,0x00000000,0x00000000,0x00000000, /* QW7,CHA-B, ALL-EVEN */
	0xFeFeFeFe,0xFeFeFeFe,0xFeFeFeFe,0xFeFeFeFe, /* QW0,CHA-B, DQ0-ODD */
	0x01010101,0x01010101,0x01010101,0x01010101, /* QW1,CHA-B, DQ0-ODD */
	0xFeFeFeFe,0xFeFeFeFe,0xFeFeFeFe,0xFeFeFeFe, /* QW2,CHA-B, DQ0-ODD */
	0x01010101,0x01010101,0x01010101,0x01010101, /* QW3,CHA-B, DQ0-ODD */
	0x01010101,0x01010101,0x01010101,0x01010101, /* QW4,CHA-B, DQ0-ODD */
	0xFeFeFeFe,0xFeFeFeFe,0xFeFeFeFe,0xFeFeFeFe, /* QW5,CHA-B, DQ0-ODD */
	0xFeFeFeFe,0xFeFeFeFe,0xFeFeFeFe,0xFeFeFeFe, /* QW6,CHA-B, DQ0-ODD */
	0x01010101,0x01010101,0x01010101,0x01010101, /* QW7,CHA-B, DQ0-ODD */
	0x02020202,0x02020202,0x02020202,0x02020202, /* QW0,CHA-B, DQ1-ODD */
	0x02020202,0x02020202,0x02020202,0x02020202, /* QW1,CHA-B, DQ1-ODD */
	0xFdFdFdFd,0xFdFdFdFd,0xFdFdFdFd,0xFdFdFdFd, /* QW2,CHA-B, DQ1-ODD */
	0xFdFdFdFd,0xFdFdFdFd,0xFdFdFdFd,0xFdFdFdFd, /* QW3,CHA-B, DQ1-ODD */
	0xFdFdFdFd,0xFdFdFdFd,0xFdFdFdFd,0xFdFdFdFd, /* QW4,CHA-B, DQ1-ODD */
	0x02020202,0x02020202,0x02020202,0x02020202, /* QW5,CHA-B, DQ1-ODD */
	0x02020202,0x02020202,0x02020202,0x02020202, /* QW6,CHA-B, DQ1-ODD */
	0x02020202,0x02020202,0x02020202,0x02020202, /* QW7,CHA-B, DQ1-ODD */
	0x04040404,0x04040404,0x04040404,0x04040404, /* QW0,CHA-B, DQ2-ODD */
	0xfBfBfBfB,0xfBfBfBfB,0xfBfBfBfB,0xfBfBfBfB, /* QW1,CHA-B, DQ2-ODD */
	0x04040404,0x04040404,0x04040404,0x04040404, /* QW2,CHA-B, DQ2-ODD */
	0x04040404,0x04040404,0x04040404,0x04040404, /* QW3,CHA-B, DQ2-ODD */
	0xfBfBfBfB,0xfBfBfBfB,0xfBfBfBfB,0xfBfBfBfB, /* QW4,CHA-B, DQ2-ODD */
	0xfBfBfBfB,0xfBfBfBfB,0xfBfBfBfB,0xfBfBfBfB, /* QW5,CHA-B, DQ2-ODD */
	0xfBfBfBfB,0xfBfBfBfB,0xfBfBfBfB,0xfBfBfBfB, /* QW6,CHA-B, DQ2-ODD */
	0xfBfBfBfB,0xfBfBfBfB,0xfBfBfBfB,0xfBfBfBfB, /* QW7,CHA-B, DQ2-ODD */
	0x08080808,0x08080808,0x08080808,0x08080808, /* QW0,CHA-B, DQ3-ODD */
	0xF7F7F7F7,0xF7F7F7F7,0xF7F7F7F7,0xF7F7F7F7, /* QW1,CHA-B, DQ3-ODD */
	0x08080808,0x08080808,0x08080808,0x08080808, /* QW2,CHA-B, DQ3-ODD */
	0x08080808,0x08080808,0x08080808,0x08080808, /* QW3,CHA-B, DQ3-ODD */
	0xF7F7F7F7,0xF7F7F7F7,0xF7F7F7F7,0xF7F7F7F7, /* QW4,CHA-B, DQ3-ODD */
	0x08080808,0x08080808,0x08080808,0x08080808, /* QW5,CHA-B, DQ3-ODD */
	0xF7F7F7F7,0xF7F7F7F7,0xF7F7F7F7,0xF7F7F7F7, /* QW6,CHA-B, DQ3-ODD */
	0xF7F7F7F7,0xF7F7F7F7,0xF7F7F7F7,0xF7F7F7F7, /* QW7,CHA-B, DQ3-ODD */
	0x10101010,0x10101010,0x10101010,0x10101010, /* QW0,CHA-B, DQ4-ODD */
	0x10101010,0x10101010,0x10101010,0x10101010, /* QW1,CHA-B, DQ4-ODD */
	0xeFeFeFeF,0xeFeFeFeF,0xeFeFeFeF,0xeFeFeFeF, /* QW2,CHA-B, DQ4-ODD */
	0x10101010,0x10101010,0x10101010,0x10101010, /* QW3,CHA-B, DQ4-ODD */
	0xeFeFeFeF,0xeFeFeFeF,0xeFeFeFeF,0xeFeFeFeF, /* QW4,CHA-B, DQ4-ODD */
	0xeFeFeFeF,0xeFeFeFeF,0xeFeFeFeF,0xeFeFeFeF, /* QW5,CHA-B, DQ4-ODD */
	0xeFeFeFeF,0xeFeFeFeF,0xeFeFeFeF,0xeFeFeFeF, /* QW6,CHA-B, DQ4-ODD */
	0x10101010,0x10101010,0x10101010,0x10101010, /* QW7,CHA-B, DQ4-ODD */
	0xdFdFdFdF,0xdFdFdFdF,0xdFdFdFdF,0xdFdFdFdF, /* QW0,CHA-B, DQ5-ODD */
	0xdFdFdFdF,0xdFdFdFdF,0xdFdFdFdF,0xdFdFdFdF, /* QW1,CHA-B, DQ5-ODD */
	0xdFdFdFdF,0xdFdFdFdF,0xdFdFdFdF,0xdFdFdFdF, /* QW2,CHA-B, DQ5-ODD */
	0x20202020,0x20202020,0x20202020,0x20202020, /* QW3,CHA-B, DQ5-ODD */
	0xdFdFdFdF,0xdFdFdFdF,0xdFdFdFdF,0xdFdFdFdF, /* QW4,CHA-B, DQ5-ODD */
	0xdFdFdFdF,0xdFdFdFdF,0xdFdFdFdF,0xdFdFdFdF, /* QW5,CHA-B, DQ5-ODD */
	0xdFdFdFdF,0xdFdFdFdF,0xdFdFdFdF,0xdFdFdFdF, /* QW6,CHA-B, DQ5-ODD */
	0xdFdFdFdF,0xdFdFdFdF,0xdFdFdFdF,0xdFdFdFdF, /* QW7,CHA-B, DQ5-ODD */
	0xBfBfBfBf,0xBfBfBfBf,0xBfBfBfBf,0xBfBfBfBf, /* QW0,CHA-B, DQ6-ODD */
	0xBfBfBfBf,0xBfBfBfBf,0xBfBfBfBf,0xBfBfBfBf, /* QW1,CHA-B, DQ6-ODD */
	0x40404040,0x40404040,0x40404040,0x40404040, /* QW2,CHA-B, DQ6-ODD */
	0xBfBfBfBf,0xBfBfBfBf,0xBfBfBfBf,0xBfBfBfBf, /* QW3,CHA-B, DQ6-ODD */
	0x40404040,0x40404040,0x40404040,0x40404040, /* QW4,CHA-B, DQ6-ODD */
	0xBfBfBfBf,0xBfBfBfBf,0xBfBfBfBf,0xBfBfBfBf, /* QW5,CHA-B, DQ6-ODD */
	0x40404040,0x40404040,0x40404040,0x40404040, /* QW6,CHA-B, DQ6-ODD */
	0xBfBfBfBf,0xBfBfBfBf,0xBfBfBfBf,0xBfBfBfBf, /* QW7,CHA-B, DQ6-ODD */
	0x80808080,0x80808080,0x80808080,0x80808080, /* QW0,CHA-B, DQ7-ODD */
	0x7F7F7F7F,0x7F7F7F7F,0x7F7F7F7F,0x7F7F7F7F, /* QW1,CHA-B, DQ7-ODD */
	0x80808080,0x80808080,0x80808080,0x80808080, /* QW2,CHA-B, DQ7-ODD */
	0x7F7F7F7F,0x7F7F7F7F,0x7F7F7F7F,0x7F7F7F7F, /* QW3,CHA-B, DQ7-ODD */
	0x80808080,0x80808080,0x80808080,0x80808080, /* QW4,CHA-B, DQ7-ODD */
	0x7F7F7F7F,0x7F7F7F7F,0x7F7F7F7F,0x7F7F7F7F, /* QW5,CHA-B, DQ7-ODD */
	0x80808080,0x80808080,0x80808080,0x80808080, /* QW6,CHA-B, DQ7-ODD */
	0x80808080,0x80808080,0x80808080,0x80808080  /* QW7,CHA-B, DQ7-ODD */
};

void TrainReceiverEn_D(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstatA, u8 Pass)
{
	u8 Node;
	struct DCTStatStruc *pDCTstat;
	u32 val;

	for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
		pDCTstat = pDCTstatA + Node;

		if (pDCTstat->DCTSysLimit) {
			if (!is_fam15h()) {
				val = Get_NB32_DCT(pDCTstat->dev_dct, 0, 0x78);
				val |= 1 <<DqsRcvEnTrain;
				Set_NB32_DCT(pDCTstat->dev_dct, 0, 0x78, val);
				val = Get_NB32_DCT(pDCTstat->dev_dct, 1, 0x78);
				val |= 1 <<DqsRcvEnTrain;
				Set_NB32_DCT(pDCTstat->dev_dct, 1, 0x78, val);
			}
			mct_TrainRcvrEn_D(pMCTstat, pDCTstat, Pass);
		}
	}
}

void TrainMaxRdLatency_En_D(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstatA)
{
	uint8_t node;
	struct DCTStatStruc *pDCTstat;

	for (node = 0; node < MAX_NODES_SUPPORTED; node++) {
		pDCTstat = pDCTstatA + node;

		if (pDCTstat->DCTSysLimit) {
			if (is_fam15h()) {
				dqsTrainMaxRdLatency_SW_Fam15(pMCTstat, pDCTstat);
			} else {
				/* FIXME
				 * Implement Family 10h MaxRdLatency training
				 */
			}
		}
	}
}

static void SetEccDQSRdWrPos_D_Fam10(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 ChipSel)
{
	u8 channel;
	u8 direction;

	for (channel = 0; channel < 2; channel++) {
		for (direction = 0; direction < 2; direction++) {
			pDCTstat->Channel = channel;	/* Channel A or B */
			pDCTstat->Direction = direction; /* Read or write */
			CalcEccDQSPos_D(pMCTstat, pDCTstat, pDCTstat->CH_EccDQSLike[channel], pDCTstat->CH_EccDQSScale[channel], ChipSel);
			print_debug_dqs_pair("\t\tSetEccDQSRdWrPos: channel ", channel, direction == DQS_READDIR? " R dqs_delay":" W dqs_delay",	pDCTstat->DQSDelay, 2);
			pDCTstat->ByteLane = 8;
			StoreDQSDatStrucVal_D(pMCTstat, pDCTstat, ChipSel);
			mct_SetDQSDelayCSR_D(pMCTstat, pDCTstat, ChipSel);
		}
	}
}

static void CalcEccDQSPos_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat,
				u16 like, u8 scale, u8 ChipSel)
{
	uint8_t DQSDelay0, DQSDelay1;
	int16_t delay_differential;
	uint16_t DQSDelay;

	if (pDCTstat->Status & (1 << SB_Registered)) {
		pDCTstat->ByteLane = 0x2;
		GetDQSDatStrucVal_D(pMCTstat, pDCTstat, ChipSel);
		DQSDelay0 = pDCTstat->DQSDelay;

		pDCTstat->ByteLane = 0x3;
		GetDQSDatStrucVal_D(pMCTstat, pDCTstat, ChipSel);
		DQSDelay1 = pDCTstat->DQSDelay;

		if (pDCTstat->Direction == DQS_READDIR) {
			DQSDelay = DQSDelay1;
		} else {
			delay_differential = (int16_t)DQSDelay1 - (int16_t)DQSDelay0;
			delay_differential += (int16_t)DQSDelay1;

			DQSDelay = delay_differential;
		}
	} else {
		pDCTstat->ByteLane = like & 0xff;
		GetDQSDatStrucVal_D(pMCTstat, pDCTstat, ChipSel);
		DQSDelay0 = pDCTstat->DQSDelay;

		pDCTstat->ByteLane = (like >> 8) & 0xff;
		GetDQSDatStrucVal_D(pMCTstat, pDCTstat, ChipSel);
		DQSDelay1 = pDCTstat->DQSDelay;

		if (DQSDelay0 > DQSDelay1) {
			DQSDelay = DQSDelay0 - DQSDelay1;
		} else {
			DQSDelay = DQSDelay1 - DQSDelay0;
		}

		DQSDelay = DQSDelay * (~scale);

		DQSDelay += 0x80;	/* round it */

		DQSDelay >>= 8;		/* 256 */

		if (DQSDelay0 > DQSDelay1) {
			DQSDelay = DQSDelay1 - DQSDelay;
		} else {
			DQSDelay += DQSDelay1;
		}
	}

	pDCTstat->DQSDelay = (u8)DQSDelay;
}

static void read_dqs_write_data_timing_registers(uint16_t* delay, uint32_t dev, uint8_t dct, uint8_t dimm, uint32_t index_reg)
{
	uint32_t dword;
	uint32_t mask;

	if (is_fam15h())
		mask = 0xff;
	else
		mask = 0x7f;

	/* Lanes 0 - 3 */
	dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x1 | (dimm << 8));
	delay[3] = (dword >> 24) & mask;
	delay[2] = (dword >> 16) & mask;
	delay[1] = (dword >> 8) & mask;
	delay[0] = dword & mask;

	/* Lanes 4 - 7 */
	dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x2 | (dimm << 8));
	delay[7] = (dword >> 24) & mask;
	delay[6] = (dword >> 16) & mask;
	delay[5] = (dword >> 8) & mask;
	delay[4] = dword & mask;

	/* Lane 8 (ECC) */
	dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x3 | (dimm << 8));
	delay[8] = dword & mask;
}

static void write_dqs_write_data_timing_registers(uint16_t* delay, uint32_t dev, uint8_t dct, uint8_t dimm, uint32_t index_reg)
{
	uint32_t dword;
	uint32_t mask;

	if (is_fam15h())
		mask = 0xff;
	else
		mask = 0x7f;

	/* Lanes 0 - 3 */
	dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x1 | (dimm << 8));
	dword &= ~(mask << 24);
	dword &= ~(mask << 16);
	dword &= ~(mask << 8);
	dword &= ~mask;
	dword |= (delay[3] & mask) << 24;
	dword |= (delay[2] & mask) << 16;
	dword |= (delay[1] & mask) << 8;
	dword |= delay[0] & mask;
	Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x1 | (dimm << 8), dword);

	/* Lanes 4 - 7 */
	dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x2 | (dimm << 8));
	dword &= ~(mask << 24);
	dword &= ~(mask << 16);
	dword &= ~(mask << 8);
	dword &= ~mask;
	dword |= (delay[7] & mask) << 24;
	dword |= (delay[6] & mask) << 16;
	dword |= (delay[5] & mask) << 8;
	dword |= delay[4] & mask;
	Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x2 | (dimm << 8), dword);

	/* Lane 8 (ECC) */
	dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x3 | (dimm << 8));
	dword &= ~mask;
	dword |= delay[8] & mask;
	Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x3 | (dimm << 8), dword);
}

/* DQS Position Training
 * Algorithm detailed in the Fam10h BKDG Rev. 3.62 section 2.8.9.9.3
 */
static void TrainDQSRdWrPos_D_Fam10(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat)
{
	u32 Errors;
	u8 Channel;
	u8 Receiver;
	u8 _DisableDramECC = 0;
	u32 PatternBuffer[304];	/* 288 + 16 */
	u8 _Wrap32Dis = 0, _SSE2 = 0;

	u32 dev;
	u32 addr;
	u8 valid;
	u32 cr4;
	u32 lo, hi;
	u32 index_reg;
	uint32_t TestAddr;

	uint8_t dual_rank;
	uint8_t iter;
	uint8_t lane;
	uint16_t bytelane_test_results;
	uint16_t current_write_dqs_delay[MAX_BYTE_LANES];
	uint16_t current_read_dqs_delay[MAX_BYTE_LANES];
	uint16_t write_dqs_delay_stepping_done[MAX_BYTE_LANES];
	uint8_t dqs_read_results_array[2][MAX_BYTE_LANES][64];		/* [rank][lane][step] */
	uint8_t dqs_write_results_array[2][MAX_BYTE_LANES][128];	/* [rank][lane][step] */

	uint8_t last_pos = 0;
	uint8_t cur_count = 0;
	uint8_t best_pos = 0;
	uint8_t best_count = 0;

	print_debug_dqs("\nTrainDQSRdWrPos: Node_ID ", pDCTstat->Node_ID, 0);
	cr4 = read_cr4();
	if (cr4 & (1<<9)) {
		_SSE2 = 1;
	}
	cr4 |= (1<<9);		/* OSFXSR enable SSE2 */
	write_cr4(cr4);

	addr = HWCR;
	_RDMSR(addr, &lo, &hi);
	if (lo & (1<<17)) {
		_Wrap32Dis = 1;
	}
	lo |= (1<<17);		/* HWCR.wrap32dis */
	_WRMSR(addr, lo, hi);	/* allow 64-bit memory references in real mode */

	/* Disable ECC correction of reads on the dram bus. */
	_DisableDramECC = mct_DisableDimmEccEn_D(pMCTstat, pDCTstat);

	SetupDqsPattern_D(pMCTstat, pDCTstat, PatternBuffer);

	/* mct_BeforeTrainDQSRdWrPos_D */

	dev = pDCTstat->dev_dct;
	pDCTstat->Direction = DQS_READDIR;

	/* 2.8.9.9.3 (2)
	 * Loop over each channel, lane, and rank
	 */

	/* NOTE
	 * The BKDG originally stated to iterate over lane, then rank, however this process is quite slow
	 * compared to an equivalent loop over rank, then lane as the latter allows multiple lanes to be
	 * tested simultaneously, thus improving performance by around 8x.
	 */

	Errors = 0;
	for (Channel = 0; Channel < 2; Channel++) {
		print_debug_dqs("\tTrainDQSRdWrPos: 1 Channel ", Channel, 1);
		pDCTstat->Channel = Channel;

		if (pDCTstat->DIMMValidDCT[Channel] == 0)	/* mct_BeforeTrainDQSRdWrPos_D */
			continue;

		index_reg = 0x98;

		dual_rank = 0;
		Receiver = mct_InitReceiver_D(pDCTstat, Channel);
		/* There are four receiver pairs, loosely associated with chipselects.
		* This is essentially looping over each rank of each DIMM.
		*/
		for (; Receiver < 8; Receiver++) {
			if ((Receiver & 0x1) == 0) {
				/* Even rank of DIMM */
				if (mct_RcvrRankEnabled_D(pMCTstat, pDCTstat, Channel, Receiver+1))
					dual_rank = 1;
				else
					dual_rank = 0;
			}

			if (!mct_RcvrRankEnabled_D(pMCTstat, pDCTstat, Channel, Receiver)) {
				continue;
			}

			/* Select the base test address for the current rank */
			TestAddr = mct_GetMCTSysAddr_D(pMCTstat, pDCTstat, Channel, Receiver, &valid);
			if (!valid) {	/* Address not supported on current CS */
				continue;
			}

			print_debug_dqs("\t\t\t\tTrainDQSRdWrPos: 14 TestAddr ", TestAddr, 4);
			SetUpperFSbase(TestAddr);	/* fs:eax = far ptr to target */

			print_debug_dqs("\t\t\t\tTrainDQSRdWrPos: 12 Receiver ", Receiver, 2);

			/* 2.8.9.9.3 (DRAM Write Data Timing Loop)
			 * Iterate over all possible DQS delay values (0x0 - 0x7f)
			 */
			uint8_t test_write_dqs_delay = 0;
			uint8_t test_read_dqs_delay = 0;
			uint8_t passing_dqs_delay_found[MAX_BYTE_LANES];

			/* Initialize variables */
			for (lane = 0; lane < MAX_BYTE_LANES; lane++) {
				current_write_dqs_delay[lane] = 0;
				passing_dqs_delay_found[lane] = 0;
				write_dqs_delay_stepping_done[lane] = 0;
			}

			for (test_write_dqs_delay = 0; test_write_dqs_delay < 128; test_write_dqs_delay++) {
				print_debug_dqs("\t\t\t\tTrainDQSRdWrPos: 16 test_write_dqs_delay ", test_write_dqs_delay, 6);

				/* Break out of loop if passing window already found, */
				if (write_dqs_delay_stepping_done[0] && write_dqs_delay_stepping_done[1]
					&& write_dqs_delay_stepping_done[2] && write_dqs_delay_stepping_done[3]
					&& write_dqs_delay_stepping_done[4] && write_dqs_delay_stepping_done[5]
					&& write_dqs_delay_stepping_done[6] && write_dqs_delay_stepping_done[7])
					break;

				/* Commit the current Write Data Timing settings to the hardware registers */
				write_dqs_write_data_timing_registers(current_write_dqs_delay, dev, Channel, (Receiver >> 1), index_reg);

				/* Write the DRAM training pattern to the base test address */
				WriteDQSTestPattern_D(pMCTstat, pDCTstat, TestAddr << 8);

				/* 2.8.9.9.3 (DRAM Read DQS Timing Control Loop)
				 * Iterate over all possible DQS delay values (0x0 - 0x3f)
				 */
				for (test_read_dqs_delay = 0; test_read_dqs_delay < 64; test_read_dqs_delay++) {
					print_debug_dqs("\t\t\t\t\tTrainDQSRdWrPos: 161 test_read_dqs_delay ", test_read_dqs_delay, 6);

					/* Initialize Read DQS Timing Control settings for this iteration */
					for (lane = 0; lane < MAX_BYTE_LANES; lane++)
						if (!write_dqs_delay_stepping_done[lane])
							current_read_dqs_delay[lane] = test_read_dqs_delay;

					/* Commit the current Read DQS Timing Control settings to the hardware registers */
					write_dqs_read_data_timing_registers(current_read_dqs_delay, dev, Channel, (Receiver >> 1), index_reg);

					/* Initialize test result variable */
					bytelane_test_results = 0xff;

					/* Read the DRAM training pattern from the base test address three times
					 * NOTE
					 * While the BKDG states to read three times this is probably excessive!
					 * Decrease training time by only reading the test pattern once per iteration
					 */
					for (iter = 0; iter < 1; iter++) {
						/* Flush caches */
						SetTargetWTIO_D(TestAddr);
						FlushDQSTestPattern_D(pDCTstat, TestAddr << 8);
						ResetTargetWTIO_D();

						/* Read and compare pattern */
						bytelane_test_results &= (CompareDQSTestPattern_D(pMCTstat, pDCTstat, TestAddr << 8) & 0xff); /* [Lane 7 :: Lane 0] 0 = fail, 1 = pass */

						/* If all lanes have already failed testing bypass remaining re-read attempt(s) */
						if (bytelane_test_results == 0x0)
							break;
					}

					/* Store any lanes that passed testing for later use */
					for (lane = 0; lane < 8; lane++)
						if (!write_dqs_delay_stepping_done[lane])
							dqs_read_results_array[Receiver & 0x1][lane][test_read_dqs_delay] = (!!(bytelane_test_results & (1 << lane)));

					print_debug_dqs("\t\t\t\t\tTrainDQSRdWrPos: 162 bytelane_test_results ", bytelane_test_results, 6);
				}

				for (lane = 0; lane < MAX_BYTE_LANES; lane++) {
					if (write_dqs_delay_stepping_done[lane])
						continue;

					/* Determine location and length of longest consecutive string of passing values
					 * Output is stored in best_pos and best_count
					 */
					last_pos = 0;
					cur_count = 0;
					best_pos = 0;
					best_count = 0;
					for (iter = 0; iter < 64; iter++) {
						if ((dqs_read_results_array[Receiver & 0x1][lane][iter]) && (iter < 63)) {
							/* Pass */
							cur_count++;
						} else {
							/* Failure or end of loop */
							if (cur_count > best_count) {
								best_count = cur_count;
								best_pos = last_pos;
							}
							cur_count = 0;
							last_pos = iter;
						}
					}

					if (best_count > 2) {
						/* Exit the DRAM Write Data Timing Loop after programming the Read DQS Timing Control
						 * register with the center of the passing window
						 */
						current_read_dqs_delay[lane] = (best_pos + (best_count / 2));
						passing_dqs_delay_found[lane] = 1;

						/* Commit the current Read DQS Timing Control settings to the hardware registers */
						write_dqs_read_data_timing_registers(current_read_dqs_delay, dev, Channel, (Receiver >> 1), index_reg);

						/* Exit the DRAM Write Data Timing Loop */
						write_dqs_delay_stepping_done[lane] = 1;

						print_debug_dqs("\t\t\t\tTrainDQSRdWrPos: 142 largest passing region ", best_count, 4);
						print_debug_dqs("\t\t\t\tTrainDQSRdWrPos: 143 largest passing region start ", best_pos, 4);
					}

					/* Increment the DQS Write Delay value if needed for the next DRAM Write Data Timing Loop iteration */
					if (!write_dqs_delay_stepping_done[lane])
						current_write_dqs_delay[lane]++;
				}
			}

			/* Flag failure(s) if present */
			for (lane = 0; lane < 8; lane++) {
				if (!passing_dqs_delay_found[lane]) {
					print_debug_dqs("\t\t\t\tTrainDQSRdWrPos: 121 Unable to find passing region for lane ", lane, 2);

					/* Flag absence of passing window */
					Errors |= 1 << SB_NODQSPOS;
				}
			}

			/* Iterate over all possible Write Data Timing values (0x0 - 0x7f)
			 * Note that the Read DQS Timing Control was calibrated / centered in the prior nested loop
			 */
			for (test_write_dqs_delay = 0; test_write_dqs_delay < 128; test_write_dqs_delay++) {
				/* Initialize Write Data Timing settings for this iteration */
				for (lane = 0; lane < MAX_BYTE_LANES; lane++)
					current_write_dqs_delay[lane] = test_write_dqs_delay;

				/* Commit the current Write Data Timing settings to the hardware registers */
				write_dqs_write_data_timing_registers(current_write_dqs_delay, dev, Channel, (Receiver >> 1), index_reg);

				/* Write the DRAM training pattern to the base test address */
				WriteDQSTestPattern_D(pMCTstat, pDCTstat, TestAddr << 8);

				/* Flush caches */
				SetTargetWTIO_D(TestAddr);
				FlushDQSTestPattern_D(pDCTstat, TestAddr << 8);
				ResetTargetWTIO_D();

				/* Read and compare pattern from the base test address */
				bytelane_test_results = (CompareDQSTestPattern_D(pMCTstat, pDCTstat, TestAddr << 8) & 0xff); /* [Lane 7 :: Lane 0] 0 = fail, 1 = pass */

				/* Store any lanes that passed testing for later use */
				for (lane = 0; lane < 8; lane++)
					dqs_write_results_array[Receiver & 0x1][lane][test_write_dqs_delay] = (!!(bytelane_test_results & (1 << lane)));
			}

			for (lane = 0; lane < 8; lane++) {
				if ((!dual_rank) || (dual_rank && (Receiver & 0x1))) {

#ifdef PRINT_PASS_FAIL_BITMAPS
					for (iter = 0; iter < 64; iter++) {
						if (dqs_read_results_array[0][lane][iter])
							printk(BIOS_DEBUG, "+");
						else
							printk(BIOS_DEBUG, ".");
					}
					printk(BIOS_DEBUG, "\n");
					for (iter = 0; iter < 64; iter++) {
						if (dqs_read_results_array[1][lane][iter])
							printk(BIOS_DEBUG, "+");
						else
							printk(BIOS_DEBUG, ".");
					}
					printk(BIOS_DEBUG, "\n\n");
					for (iter = 0; iter < 128; iter++) {
						if (dqs_write_results_array[0][lane][iter])
							printk(BIOS_DEBUG, "+");
						else
							printk(BIOS_DEBUG, ".");
					}
					printk(BIOS_DEBUG, "\n");
					for (iter = 0; iter < 128; iter++) {
						if (dqs_write_results_array[1][lane][iter])
							printk(BIOS_DEBUG, "+");
						else
							printk(BIOS_DEBUG, ".");
					}
					printk(BIOS_DEBUG, "\n\n");
#endif

					/* Base rank of single-rank DIMM, or odd rank of dual-rank DIMM */
					if (dual_rank) {
						/* Intersect the passing windows of both ranks */
						for (iter = 0; iter < 64; iter++)
							if (!dqs_read_results_array[1][lane][iter])
								dqs_read_results_array[0][lane][iter] = 0;
						for (iter = 0; iter < 128; iter++)
							if (!dqs_write_results_array[1][lane][iter])
								dqs_write_results_array[0][lane][iter] = 0;
					}

					/* Determine location and length of longest consecutive string of passing values for read DQS timing
					 * Output is stored in best_pos and best_count
					 */
					last_pos = 0;
					cur_count = 0;
					best_pos = 0;
					best_count = 0;
					for (iter = 0; iter < 64; iter++) {
						if ((dqs_read_results_array[0][lane][iter]) && (iter < 63)) {
							/* Pass */
							cur_count++;
						} else {
							/* Failure or end of loop */
							if (cur_count > best_count) {
								best_count = cur_count;
								best_pos = last_pos;
							}
							cur_count = 0;
							last_pos = iter;
						}
					}
					print_debug_dqs("\t\t\t\tTrainDQSRdWrPos: 144 largest read passing region ", best_count, 4);
					if (best_count > 0) {
						if (best_count < MIN_DQS_WNDW) {
							/* Flag excessively small passing window */
							Errors |= 1 << SB_SMALLDQS;
						}

						/* Find the center of the passing window */
						current_read_dqs_delay[lane] = (best_pos + (best_count / 2));

						/* Commit the current Read DQS Timing Control settings to the hardware registers */
						write_dqs_read_data_timing_registers(current_read_dqs_delay, dev, Channel, (Receiver >> 1), index_reg);

						/* Save the final Read DQS Timing Control settings for later use */
						pDCTstat->CH_D_DIR_B_DQS[Channel][Receiver >> 1][DQS_READDIR][lane] = current_read_dqs_delay[lane];
					} else {
						print_debug_dqs("\t\t\t\tTrainDQSRdWrPos: 122 Unable to find read passing region for lane ", lane, 2);

						/* Flag absence of passing window */
						Errors |= 1 << SB_NODQSPOS;
					}

					/* Determine location and length of longest consecutive string of passing values for write DQS timing
					 * Output is stored in best_pos and best_count
					 */
					last_pos = 0;
					cur_count = 0;
					best_pos = 0;
					best_count = 0;
					for (iter = 0; iter < 128; iter++) {
						if ((dqs_write_results_array[0][lane][iter]) && (iter < 127)) {
							/* Pass */
							cur_count++;
						} else {
							/* Failure or end of loop */
							if (cur_count > best_count) {
								best_count = cur_count;
								best_pos = last_pos;
							}
							cur_count = 0;
							last_pos = iter;
						}
					}
					print_debug_dqs("\t\t\t\tTrainDQSRdWrPos: 145 largest write passing region ", best_count, 4);
					if (best_count > 0) {
						if (best_count < MIN_DQS_WNDW) {
							/* Flag excessively small passing window */
							Errors |= 1 << SB_SMALLDQS;
						}

						/* Find the center of the passing window */
						current_write_dqs_delay[lane] = (best_pos + (best_count / 2));

						/* Commit the current Write Data Timing settings to the hardware registers */
						write_dqs_write_data_timing_registers(current_write_dqs_delay, dev, Channel, (Receiver >> 1), index_reg);

						/* Save the final Write Data Timing settings for later use */
						pDCTstat->CH_D_DIR_B_DQS[Channel][Receiver >> 1][DQS_WRITEDIR][lane] = current_write_dqs_delay[lane];
					} else {
						print_debug_dqs("\t\t\t\tTrainDQSRdWrPos: 123 Unable to find write passing region for lane ", lane, 2);

						/* Flag absence of passing window */
						Errors |= 1 << SB_NODQSPOS;
					}
				}
			}

		}
	}

	pDCTstat->TrainErrors |= Errors;
	pDCTstat->ErrStatus |= Errors;

#if DQS_TRAIN_DEBUG > 0
	{
		u8 val;
		u8 i;
		u8 ChannelDTD, ReceiverDTD, Dir;
		u8 *p;

		for (Dir = 0; Dir < 2; Dir++) {
			if (Dir == 1) {
				printk(BIOS_DEBUG, "TrainDQSRdWrPos: CH_D_DIR_B_DQS WR:\n");
			} else {
				printk(BIOS_DEBUG, "TrainDQSRdWrPos: CH_D_DIR_B_DQS RD:\n");
			}
			for (ChannelDTD = 0; ChannelDTD < 2; ChannelDTD++) {
				printk(BIOS_DEBUG, "Channel: %02x\n", ChannelDTD);
				for (ReceiverDTD = 0; ReceiverDTD < MAX_CS_SUPPORTED; ReceiverDTD += 2) {
					printk(BIOS_DEBUG, "\t\tReceiver: %02x:", ReceiverDTD);
					p = pDCTstat->CH_D_DIR_B_DQS[ChannelDTD][ReceiverDTD >> 1][Dir];
					for (i = 0; i < 8; i++) {
						val  = p[i];
						printk(BIOS_DEBUG, " %02x", val);
					}
					printk(BIOS_DEBUG, "\n");
				}
			}
		}

	}
#endif
	if (_DisableDramECC) {
		mct_EnableDimmEccEn_D(pMCTstat, pDCTstat, _DisableDramECC);
	}
	if (!_Wrap32Dis) {
		addr = HWCR;
		_RDMSR(addr, &lo, &hi);
		lo &= ~(1<<17);		/* restore HWCR.wrap32dis */
		_WRMSR(addr, lo, hi);
	}
	if (!_SSE2) {
		cr4 = read_cr4();
		cr4 &= ~(1<<9);		/* restore cr4.OSFXSR */
		write_cr4(cr4);
	}

	printk(BIOS_DEBUG, "TrainDQSRdWrPos: Status %x\n", pDCTstat->Status);
	printk(BIOS_DEBUG, "TrainDQSRdWrPos: TrainErrors %x\n", pDCTstat->TrainErrors);
	printk(BIOS_DEBUG, "TrainDQSRdWrPos: ErrStatus %x\n", pDCTstat->ErrStatus);
	printk(BIOS_DEBUG, "TrainDQSRdWrPos: ErrCode %x\n", pDCTstat->ErrCode);
	printk(BIOS_DEBUG, "TrainDQSRdWrPos: Done\n\n");
}

/* Calcuate and set MaxRdLatency
 * Algorithm detailed in the Fam15h BKDG Rev. 3.14 section 2.10.5.8.5
 */
void Calc_SetMaxRdLatency_D_Fam15(struct MCTStatStruc *pMCTstat,
		struct DCTStatStruc *pDCTstat, uint8_t dct, uint8_t calc_min)
{
	uint8_t dimm;
	uint8_t lane;
	uint32_t dword;
	uint32_t dword2;
	uint32_t max_delay;
	uint8_t mem_clk = 0;
	uint8_t nb_pstate;
	uint32_t nb_clk;
	uint32_t p = 0;
	uint32_t n = 0;
	uint32_t t = 0;
	uint16_t current_phy_phase_delay[MAX_BYTE_LANES];
	uint16_t current_read_dqs_delay[MAX_BYTE_LANES];

	uint32_t index_reg = 0x98;
	uint32_t dev = pDCTstat->dev_dct;
	uint16_t fam15h_freq_tab[] = {0, 0, 0, 0, 333, 0, 400, 0, 0, 0, 533, 0, 0, 0, 667, 0, 0, 0, 800, 0, 0, 0, 933};

#if DQS_TRAIN_DEBUG > 0
	printk(BIOS_DEBUG, "%s: Start\n", __func__);
#endif

	uint8_t lane_count;
	lane_count = get_available_lane_count(pMCTstat, pDCTstat);

	mem_clk = Get_NB32_DCT(dev, dct, 0x94) & 0x1f;
	if (fam15h_freq_tab[mem_clk] == 0) {
		pDCTstat->CH_MaxRdLat[dct][0] = 0x55;
		pDCTstat->CH_MaxRdLat[dct][1] = 0x55;
		return;
	}

	/* P is specified in PhyCLKs (1/2 MEMCLKs) */
	for (nb_pstate = 0; nb_pstate < 2; nb_pstate++) {
		/* 2.10.5.8.5 (2) */
		dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x00000004);
		if ((!(dword & (0x1 << 21))) && (!(dword & (0x1 << 13))) && (!(dword & (0x1 << 5))))
			p += 1;
		else
			p += 2;

		/* 2.10.5.8.5 (3) */
		dword = Get_NB32_DCT_NBPstate(dev, dct, nb_pstate, 0x210) & 0xf;	/* Retrieve RdPtrInit */
		p += (9 - dword);

		/* 2.10.5.8.5 (4) */
		if (!calc_min)
			p += 5;

		/* 2.10.5.8.5 (5) */
		dword = Get_NB32_DCT(dev, dct, 0xa8);
		dword2 = Get_NB32_DCT(dev, dct, 0x90);
		if ((!(dword & (0x1 << 5))) && (!(dword2 & (0x1 << 16))))
			p += 2;

		/* 2.10.5.8.5 (6) */
		dword = Get_NB32_DCT(dev, dct, 0x200) & 0x1f;	/* Retrieve Tcl */
		p += (2 * (dword - 1));

		/* 2.10.5.8.5 (7) */
		max_delay = 0;
		for (dimm = 0; dimm < 4; dimm++) {
			if (!mct_RcvrRankEnabled_D(pMCTstat, pDCTstat, dct, dimm * 2))
				continue;

			read_dqs_receiver_enable_control_registers(current_phy_phase_delay, dev, dct, dimm, index_reg);
			read_dqs_read_data_timing_registers(current_read_dqs_delay, dev, dct, dimm, index_reg);
			for (lane = 0; lane < lane_count; lane++)
				if ((current_phy_phase_delay[lane] + current_read_dqs_delay[lane]) > max_delay)
					max_delay = (current_phy_phase_delay[lane] + current_read_dqs_delay[lane]);
		}
		p += (max_delay >> 5);

		/* 2.10.5.8.5 (8) */
		if (!calc_min)
			p += 5;

		/* 2.10.5.8.5 (9) */
		t += 800;

		/* 2.10.5.8.5 (10) */
		dword = Get_NB32(pDCTstat->dev_nbctl, (0x160 + (nb_pstate * 4)));		/* Retrieve NbDid, NbFid */
		nb_clk = (200 * (((dword >> 1) & 0x1f) + 0x4)) / (((dword >> 7) & 0x1)?2:1);
		n = (((((uint64_t)p * 1000000000000ULL)/(((uint64_t)fam15h_freq_tab[mem_clk] * 1000000ULL) * 2)) + ((uint64_t)t)) * ((uint64_t)nb_clk * 1000)) / 1000000000ULL;

		/* 2.10.5.8.5 (11) */
		if (!calc_min)
			n -= 1;

		/* 2.10.5.8.5 (12) */
		if (!calc_min) {
			dword = Get_NB32_DCT_NBPstate(dev, dct, nb_pstate, 0x210);
			dword &= ~(0x3ff << 22);
			dword |= (((n - 1) & 0x3ff) << 22);
			Set_NB32_DCT_NBPstate(dev, dct, nb_pstate, 0x210, dword);
		}

		/* Save result for later use */
		pDCTstat->CH_MaxRdLat[dct][nb_pstate] = n - 1;

#if DQS_TRAIN_DEBUG > 0
		printk(BIOS_DEBUG, "%s: CH_MaxRdLat[%d][%d]: %03x\n", __func__, dct, nb_pstate, pDCTstat->CH_MaxRdLat[dct][nb_pstate]);
#endif
	}

#if DQS_TRAIN_DEBUG > 0
	printk(BIOS_DEBUG, "%s: Done\n", __func__);
#endif
}

static void start_dram_dqs_training_pattern_fam15(struct MCTStatStruc *pMCTstat,
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

	/* 2.10.5.8.6.1.1 Send Activate Command (Target A) */
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

	/* 2.10.5.8.6.1.1 Send Activate Command (Target B) */
	dword = Get_NB32_DCT(dev, dct, 0x28c);
	dword &= ~(0xff << 22);				/* CmdChipSelect = Receiver */
	dword |= ((0x1 << Receiver) << 22);
	dword &= ~(0x7 << 19);				/* CmdBank = 1 */
	dword |= (0x1 << 19);
	dword &= ~(0x3ffff);				/* CmdAddress = 0 */
	dword |= (0x1 << 31);				/* SendActCmd = 1 */
	Set_NB32_DCT(dev, dct, 0x28c, dword);

	/* Wait for SendActCmd == 0 */
	do {
		dword = Get_NB32_DCT(dev, dct, 0x28c);
	} while (dword & (0x1 << 31));

	/* Wait 75 MEMCLKs. */
	precise_memclk_delay_fam15(pMCTstat, pDCTstat, dct, 75);
}

static void stop_dram_dqs_training_pattern_fam15(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, uint8_t dct, uint8_t Receiver)
{
	uint32_t dword;
	uint32_t dev = pDCTstat->dev_dct;

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

void read_dram_dqs_training_pattern_fam15(struct MCTStatStruc *pMCTstat,
	struct DCTStatStruc *pDCTstat, uint8_t dct,
	uint8_t Receiver, uint8_t lane, uint8_t stop_on_error)
{
	uint32_t dword;
	uint32_t dev = pDCTstat->dev_dct;

	start_dram_dqs_training_pattern_fam15(pMCTstat, pDCTstat, dct, Receiver);

	/* 2.10.5.8.6.1.2 */
	/* Configure DQMask */
	if (lane < 4) {
		Set_NB32_DCT(dev, dct, 0x274, ~(0xff << (lane * 8)));
		Set_NB32_DCT(dev, dct, 0x278, ~0x0);
		dword = Get_NB32_DCT(dev, dct, 0x27c);
		dword |= 0xff;				/* EccMask = 0xff */
		Set_NB32_DCT(dev, dct, 0x27c, dword);
	} else if (lane < 8) {
		Set_NB32_DCT(dev, dct, 0x274, ~0x0);
		Set_NB32_DCT(dev, dct, 0x278, ~(0xff << ((lane - 4) * 8)));
		dword = Get_NB32_DCT(dev, dct, 0x27c);
		dword |= 0xff;				/* EccMask = 0xff */
		Set_NB32_DCT(dev, dct, 0x27c, dword);
	} else if (lane == 8) {
		Set_NB32_DCT(dev, dct, 0x274, ~0x0);
		Set_NB32_DCT(dev, dct, 0x278, ~0x0);
		dword = Get_NB32_DCT(dev, dct, 0x27c);
		dword &= ~(0xff);			/* EccMask = 0x0 */
		Set_NB32_DCT(dev, dct, 0x27c, dword);
	} else if (lane == 0xff) {
		Set_NB32_DCT(dev, dct, 0x274, ~0xffffffff);
		Set_NB32_DCT(dev, dct, 0x278, ~0xffffffff);
		dword = Get_NB32_DCT(dev, dct, 0x27c);
		if (get_available_lane_count(pMCTstat, pDCTstat) < 9)
			dword |= 0xff;			/* EccMask = 0xff */
		else
			dword &= ~(0xff);		/* EccMask = 0x0 */
		Set_NB32_DCT(dev, dct, 0x27c, dword);
	} else {
		Set_NB32_DCT(dev, dct, 0x274, ~0x0);
		Set_NB32_DCT(dev, dct, 0x278, ~0x0);
		dword = Get_NB32_DCT(dev, dct, 0x27c);
		dword |= 0xff;				/* EccMask = 0xff */
		Set_NB32_DCT(dev, dct, 0x27c, dword);
	}

	dword = Get_NB32_DCT(dev, dct, 0x270);
	dword &= ~(0x7ffff);				/* DataPrbsSeed = 55555 */
// 	dword |= (0x55555);
	dword |= (0x44443);				/* Use AGESA seed */
	Set_NB32_DCT(dev, dct, 0x270, dword);

	/* 2.10.5.8.4 */
	dword = Get_NB32_DCT(dev, dct, 0x260);
	dword &= ~(0x1fffff);				/* CmdCount = 256 */
	dword |= 256;
	Set_NB32_DCT(dev, dct, 0x260, dword);

	/* Configure Target A */
	dword = Get_NB32_DCT(dev, dct, 0x254);
	dword &= ~(0x7 << 24);				/* TgtChipSelect = Receiver */
	dword |= (Receiver & 0x7) << 24;
	dword &= ~(0x7 << 21);				/* TgtBank = 0 */
	dword &= ~(0x3ff);				/* TgtAddress = 0 */
	Set_NB32_DCT(dev, dct, 0x254, dword);

	/* Configure Target B */
	dword = Get_NB32_DCT(dev, dct, 0x258);
	dword &= ~(0x7 << 24);				/* TgtChipSelect = Receiver */
	dword |= (Receiver & 0x7) << 24;
	dword &= ~(0x7 << 21);				/* TgtBank = 1 */
	dword |= (0x1 << 21);
	dword &= ~(0x3ff);				/* TgtAddress = 0 */
	Set_NB32_DCT(dev, dct, 0x258, dword);

	dword = Get_NB32_DCT(dev, dct, 0x250);
	dword |= (0x1 << 3);				/* ResetAllErr = 1 */
	dword &= ~(0x1 << 4);				/* StopOnErr = stop_on_error */
	dword |= (stop_on_error & 0x1) << 4;
	dword &= ~(0x3 << 8);				/* CmdTgt = 1 (Alternate between Target A and Target B) */
	dword |= (0x1 << 8);
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

	stop_dram_dqs_training_pattern_fam15(pMCTstat, pDCTstat, dct, Receiver);
}

void write_dram_dqs_training_pattern_fam15(struct MCTStatStruc *pMCTstat,
	struct DCTStatStruc *pDCTstat, uint8_t dct,
	uint8_t Receiver, uint8_t lane, uint8_t stop_on_error)
{
	uint32_t dword;
	uint32_t dev = pDCTstat->dev_dct;

	start_dram_dqs_training_pattern_fam15(pMCTstat, pDCTstat, dct, Receiver);

	/* 2.10.5.8.6.1.2 */
	/* Configure DQMask */
	if (lane < 4) {
		Set_NB32_DCT(dev, dct, 0x274, ~(0xff << (lane * 8)));
		Set_NB32_DCT(dev, dct, 0x278, ~0x0);
		dword = Get_NB32_DCT(dev, dct, 0x27c);
		dword |= 0xff;				/* EccMask = 0xff */
		Set_NB32_DCT(dev, dct, 0x27c, dword);
	} else if (lane < 8) {
		Set_NB32_DCT(dev, dct, 0x274, ~0x0);
		Set_NB32_DCT(dev, dct, 0x278, ~(0xff << ((lane - 4) * 8)));
		dword = Get_NB32_DCT(dev, dct, 0x27c);
		dword |= 0xff;				/* EccMask = 0xff */
		Set_NB32_DCT(dev, dct, 0x27c, dword);
	} else if (lane == 8) {
		Set_NB32_DCT(dev, dct, 0x274, ~0x0);
		Set_NB32_DCT(dev, dct, 0x278, ~0x0);
		dword = Get_NB32_DCT(dev, dct, 0x27c);
		dword &= ~(0xff);			/* EccMask = 0x0 */
		Set_NB32_DCT(dev, dct, 0x27c, dword);
	} else if (lane == 0xff) {
		Set_NB32_DCT(dev, dct, 0x274, ~0xffffffff);
		Set_NB32_DCT(dev, dct, 0x278, ~0xffffffff);
		dword = Get_NB32_DCT(dev, dct, 0x27c);
		if (get_available_lane_count(pMCTstat, pDCTstat) < 9)
			dword |= 0xff;			/* EccMask = 0xff */
		else
			dword &= ~(0xff);		/* EccMask = 0x0 */
		Set_NB32_DCT(dev, dct, 0x27c, dword);
	} else {
		Set_NB32_DCT(dev, dct, 0x274, ~0x0);
		Set_NB32_DCT(dev, dct, 0x278, ~0x0);
		dword = Get_NB32_DCT(dev, dct, 0x27c);
		dword |= 0xff;				/* EccMask = 0xff */
		Set_NB32_DCT(dev, dct, 0x27c, dword);
	}

	dword = Get_NB32_DCT(dev, dct, 0x270);
	dword &= ~(0x7ffff);				/* DataPrbsSeed = 55555 */
// 	dword |= (0x55555);
	dword |= (0x44443);				/* Use AGESA seed */
	Set_NB32_DCT(dev, dct, 0x270, dword);

	/* 2.10.5.8.4 */
	dword = Get_NB32_DCT(dev, dct, 0x260);
	dword &= ~(0x1fffff);				/* CmdCount = 256 */
	dword |= 256;
	Set_NB32_DCT(dev, dct, 0x260, dword);

	/* Configure Target A */
	dword = Get_NB32_DCT(dev, dct, 0x254);
	dword &= ~(0x7 << 24);				/* TgtChipSelect = Receiver */
	dword |= (Receiver & 0x7) << 24;
	dword &= ~(0x7 << 21);				/* TgtBank = 0 */
	dword &= ~(0x3ff);				/* TgtAddress = 0 */
	Set_NB32_DCT(dev, dct, 0x254, dword);

	/* Configure Target B */
	dword = Get_NB32_DCT(dev, dct, 0x258);
	dword &= ~(0x7 << 24);				/* TgtChipSelect = Receiver */
	dword |= (Receiver & 0x7) << 24;
	dword &= ~(0x7 << 21);				/* TgtBank = 1 */
	dword |= (0x1 << 21);
	dword &= ~(0x3ff);				/* TgtAddress = 0 */
	Set_NB32_DCT(dev, dct, 0x258, dword);

	dword = Get_NB32_DCT(dev, dct, 0x250);
	dword |= (0x1 << 3);				/* ResetAllErr = 1 */
	dword &= ~(0x1 << 4);				/* StopOnErr = stop_on_error */
	dword |= (stop_on_error & 0x1) << 4;
	dword &= ~(0x3 << 8);				/* CmdTgt = 1 (Alternate between Target A and Target B) */
	dword |= (0x1 << 8);
	dword &= ~(0x7 << 5);				/* CmdType = 1 (Write) */
	dword |= (0x1 << 5);
	dword |= (0x1 << 11);				/* SendCmd = 1 */
	Set_NB32_DCT(dev, dct, 0x250, dword);

	/* 2.10.5.8.6.1.2 Wait for TestStatus == 1 and CmdSendInProg == 0 */
	do {
		dword = Get_NB32_DCT(dev, dct, 0x250);
	} while ((dword & (0x1 << 12)) || (!(dword & (0x1 << 10))));

	dword = Get_NB32_DCT(dev, dct, 0x250);
	dword &= ~(0x1 << 11);				/* SendCmd = 0 */
	Set_NB32_DCT(dev, dct, 0x250, dword);

	stop_dram_dqs_training_pattern_fam15(pMCTstat, pDCTstat, dct, Receiver);
}

/* DQS Position Training
 * Algorithm detailed in the Fam15h BKDG Rev. 3.14 section 2.10.5.8.4
 */
static uint8_t TrainDQSRdWrPos_D_Fam15(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, uint8_t dct, uint8_t receiver_start, uint8_t receiver_end, uint8_t lane_start, uint8_t lane_end)
{
	uint8_t dimm;
	uint8_t lane;
	uint32_t dword;
	uint32_t Errors;
	uint8_t Receiver;
	uint8_t dual_rank;
	uint8_t write_iter;
	uint8_t read_iter;
	uint8_t check_antiphase;
	uint8_t passing_read_dqs_delay_found;
	uint8_t passing_write_dqs_delay_found;
	uint16_t initial_write_dqs_delay[MAX_BYTE_LANES];
	uint16_t initial_read_dqs_delay[MAX_BYTE_LANES];
	uint16_t initial_write_data_timing[MAX_BYTE_LANES];
	uint16_t current_write_data_delay[MAX_BYTE_LANES];
	uint16_t current_read_dqs_delay[MAX_BYTE_LANES];
	uint16_t current_write_dqs_delay[MAX_BYTE_LANES];
	uint8_t passing_dqs_delay_found[MAX_BYTE_LANES];
	uint8_t dqs_results_array[2][(lane_end - lane_start)][32][48];		/* [rank][lane][write step][read step + 16] */

	uint8_t last_pos = 0;
	uint8_t cur_count = 0;
	uint8_t best_pos = 0;
	uint8_t best_count = 0;

	uint32_t index_reg = 0x98;
	uint32_t dev = pDCTstat->dev_dct;

	uint8_t lane_count;
	lane_count = get_available_lane_count(pMCTstat, pDCTstat);

	/* Calculate and program MaxRdLatency */
	Calc_SetMaxRdLatency_D_Fam15(pMCTstat, pDCTstat, dct, 0);

	Errors = 0;
	dual_rank = 0;

	/* There are four receiver pairs, loosely associated with chipselects.
	 * This is essentially looping over each rank within each DIMM.
	 */
	for (Receiver = receiver_start; Receiver < receiver_end; Receiver++) {
		dimm = (Receiver >> 1);
		if ((Receiver & 0x1) == 0) {
			/* Even rank of DIMM */
			if (mct_RcvrRankEnabled_D(pMCTstat, pDCTstat, dct, Receiver+1))
				dual_rank = 1;
			else
				dual_rank = 0;
		}

		if (!mct_RcvrRankEnabled_D(pMCTstat, pDCTstat, dct, Receiver)) {
			continue;
		}

#if DQS_TRAIN_DEBUG > 0
		printk(BIOS_DEBUG, "TrainDQSRdWrPos: Training DQS read/write position for receiver %d (DIMM %d)\n", Receiver, dimm);
#endif

		/* Initialize variables */
		for (lane = lane_start; lane < lane_end; lane++) {
			passing_dqs_delay_found[lane] = 0;
		}
		if ((Receiver & 0x1) == 0) {
			/* Even rank of DIMM */
			memset(dqs_results_array, 0, sizeof(dqs_results_array));

			/* Read initial read / write DQS delays */
			read_dqs_write_timing_control_registers(initial_write_dqs_delay, dev, dct, dimm, index_reg);
			read_dqs_read_data_timing_registers(initial_read_dqs_delay, dev, dct, dimm, index_reg);

			/* Read current settings of other (previously trained) lanes */
			read_dqs_write_data_timing_registers(initial_write_data_timing, dev, dct, dimm, index_reg);
		}

		/* Initialize iterators */
		memcpy(current_write_data_delay, initial_write_data_timing, sizeof(current_write_data_delay));

		for (lane = lane_start; lane < lane_end; lane++) {
			passing_read_dqs_delay_found = 0;
			passing_write_dqs_delay_found = 0;

			/* 2.10.5.8.4 (2)
			 * For each Write Data Delay value from Write DQS Delay to Write DQS Delay + 1 UI
			 */
			for (current_write_data_delay[lane] = initial_write_dqs_delay[lane]; current_write_data_delay[lane] < (initial_write_dqs_delay[lane] + 0x20); current_write_data_delay[lane]++) {
				print_debug_dqs("\t\t\t\tTrainDQSRdWrPos: 16 current_write_data_delay[lane] ", current_write_data_delay[lane], 6);

				/* 2.10.5.8.4 (2 A)
				 * Commit the current Write Data Timing settings to the hardware registers
				 */
				write_dqs_write_data_timing_registers(current_write_data_delay, dev, dct, dimm, index_reg);

				/* 2.10.5.8.4 (2 B)
				 * Write the DRAM training pattern to the test address
				 */
				write_dram_dqs_training_pattern_fam15(pMCTstat, pDCTstat, dct, Receiver, lane, 0);

				/* Read current settings of other (previously trained) lanes */
				read_dqs_read_data_timing_registers(current_read_dqs_delay, dev, dct, dimm, index_reg);

				/* 2.10.5.8.4 (2 C)
				 * For each Read DQS Delay value from 0 to 1 UI
				 */
				for (current_read_dqs_delay[lane] = 0; current_read_dqs_delay[lane] < 0x20; current_read_dqs_delay[lane]++) {
					print_debug_dqs("\t\t\t\t\tTrainDQSRdWrPos: 161 current_read_dqs_delay[lane] ", current_read_dqs_delay[lane], 6);

					if (current_read_dqs_delay[lane] >= (32 - 16)) {
						check_antiphase = 1;
					} else {
						check_antiphase = 0;
					}

					/* 2.10.5.8.4 (2 A i)
					 * Commit the current Read DQS Timing Control settings to the hardware registers
					 */
					write_dqs_read_data_timing_registers(current_read_dqs_delay, dev, dct, dimm, index_reg);

					/* 2.10.5.8.4 (2 A ii)
					 * Read the DRAM training pattern from the test address
					 */
					read_dram_dqs_training_pattern_fam15(pMCTstat, pDCTstat, dct, Receiver, lane, ((check_antiphase == 0)?1:0));

					if (check_antiphase == 0) {
						/* Check for early abort before analyzing per-nibble status */
						dword = Get_NB32_DCT(dev, dct, 0x264);
						if ((dword & 0x1ffffff) != 0) {
							print_debug_dqs("\t\t\t\t\tTrainDQSRdWrPos: 162 early abort: F2x264 ", dword, 6);
							dqs_results_array[Receiver & 0x1][lane - lane_start][current_write_data_delay[lane] - initial_write_dqs_delay[lane]][current_read_dqs_delay[lane] + 16] = 0;	/* Fail */
							continue;
						}
					}

					/* 2.10.5.8.4 (2 A iii)
					 * Record pass / fail status
					 */
					dword = Get_NB32_DCT(dev, dct, 0x268) & 0x3ffff;
					print_debug_dqs("\t\t\t\t\tTrainDQSRdWrPos: 163 read results: F2x268 ", dword, 6);
					if (dword & (0x3 << (lane * 2)))
						dqs_results_array[Receiver & 0x1][lane - lane_start][current_write_data_delay[lane] - initial_write_dqs_delay[lane]][current_read_dqs_delay[lane] + 16] = 0;	/* Fail */
					else
						dqs_results_array[Receiver & 0x1][lane - lane_start][current_write_data_delay[lane] - initial_write_dqs_delay[lane]][current_read_dqs_delay[lane] + 16] = 1;	/* Pass */
					if (check_antiphase == 1) {
						/* Check antiphase results */
						dword = Get_NB32_DCT(dev, dct, 0x26c) & 0x3ffff;
						if (dword & (0x3 << (lane * 2)))
							dqs_results_array[Receiver & 0x1][lane - lane_start][current_write_data_delay[lane] - initial_write_dqs_delay[lane]][16 - (32 - current_read_dqs_delay[lane])] = 0;	/* Fail */
						else
							dqs_results_array[Receiver & 0x1][lane - lane_start][current_write_data_delay[lane] - initial_write_dqs_delay[lane]][16 - (32 - current_read_dqs_delay[lane])] = 1;	/* Pass */
					}
				}
			}

			if (dual_rank && (Receiver & 0x1)) {
				/* Overlay the previous rank test results with the current rank */
				for (write_iter = 0; write_iter < 32; write_iter++) {
					for (read_iter = 0; read_iter < 48; read_iter++) {
						if ((dqs_results_array[0][lane - lane_start][write_iter][read_iter])
							&& (dqs_results_array[1][lane - lane_start][write_iter][read_iter]))
							dqs_results_array[1][lane - lane_start][write_iter][read_iter] = 1;
						else
							dqs_results_array[1][lane - lane_start][write_iter][read_iter] = 0;
					}
				}
			}

			/* Determine location and length of longest consecutive string of read passing values
			 * Output is stored in best_pos and best_count
			 */
			last_pos = 0;
			cur_count = 0;
			best_pos = 0;
			best_count = 0;
			for (write_iter = 0; write_iter < 32; write_iter++) {
				for (read_iter = 0; read_iter < 48; read_iter++) {
					if ((dqs_results_array[Receiver & 0x1][lane - lane_start][write_iter][read_iter]) && (read_iter < 47)) {
						/* Pass */
						cur_count++;
					} else {
						/* Failure or end of loop */
						if (cur_count > best_count) {
							best_count = cur_count;
							best_pos = last_pos;
						}
						cur_count = 0;
						last_pos = read_iter + 1;
					}
				}
				last_pos = 0;
			}

			if (best_count > 2) {
				uint16_t region_center = (best_pos + (best_count / 2));

				if (region_center < 16) {
					printk(BIOS_WARNING, "TrainDQSRdWrPos: negative DQS recovery delay detected!"
							"  Attempting to continue but your system may be unstable...\n");
					region_center = 0;
				} else {
					region_center -= 16;
				}

				/* Restore current settings of other (previously trained) lanes to the active array */
				memcpy(current_read_dqs_delay, initial_read_dqs_delay, sizeof(current_read_dqs_delay));

				/* Program the Read DQS Timing Control register with the center of the passing window */
				current_read_dqs_delay[lane] = region_center;
				passing_dqs_delay_found[lane] = 1;

				/* Commit the current Read DQS Timing Control settings to the hardware registers */
				write_dqs_read_data_timing_registers(current_read_dqs_delay, dev, dct, dimm, index_reg);

				/* Save the final Read DQS Timing Control settings for later use */
				pDCTstat->CH_D_DIR_B_DQS[dct][Receiver >> 1][DQS_READDIR][lane] = current_read_dqs_delay[lane];

				print_debug_dqs("\t\t\t\tTrainDQSRdWrPos: 142 largest read passing region ", best_count, 4);
				print_debug_dqs("\t\t\t\tTrainDQSRdWrPos: 143 largest read passing region start ", best_pos, 4);
				print_debug_dqs("\t\t\t\tTrainDQSRdWrPos: 144 largest read passing region center (raw hardware value) ", region_center, 4);
			} else {
				 /* Restore current settings of other (previously trained) lanes to the active array */
				memcpy(current_read_dqs_delay, initial_read_dqs_delay, sizeof(current_read_dqs_delay));

				/* Reprogram the Read DQS Timing Control register with the original settings */
				write_dqs_read_data_timing_registers(initial_read_dqs_delay, dev, dct, dimm, index_reg);
			}

			/* Determine location and length of longest consecutive string of write passing values
			 * Output is stored in best_pos and best_count
			 */
			last_pos = 0;
			cur_count = 0;
			best_pos = 0;
			best_count = 0;
			for (read_iter = 0; read_iter < 48; read_iter++) {
				for (write_iter = 0; write_iter < 32; write_iter++) {
					if ((dqs_results_array[Receiver & 0x1][lane - lane_start][write_iter][read_iter]) && (write_iter < 31)) {
						/* Pass */
						cur_count++;
					} else {
						/* Failure or end of loop */
						if (cur_count > best_count) {
							best_count = cur_count;
							best_pos = last_pos;
						}
						cur_count = 0;
						last_pos = write_iter + 1;
					}
				}
				last_pos = 0;
			}

			if (best_count > 2) {
				/* Restore current settings of other (previously trained) lanes to the active array */
				memcpy(current_write_dqs_delay, initial_write_data_timing, sizeof(current_write_data_delay));

				/* Program the Write DQS Timing Control register with the optimal region within the passing window */
				if (pDCTstat->Status & (1 << SB_LoadReduced))
					current_write_dqs_delay[lane] = ((best_pos + initial_write_dqs_delay[lane]) + (best_count / 3));
				else
					current_write_dqs_delay[lane] = ((best_pos + initial_write_dqs_delay[lane]) + (best_count / 2));
				passing_write_dqs_delay_found = 1;

				/* Commit the current Write DQS Timing Control settings to the hardware registers */
				write_dqs_write_data_timing_registers(current_write_dqs_delay, dev, dct, dimm, index_reg);

				/* Save the final Write Data Timing settings for later use */
				pDCTstat->CH_D_DIR_B_DQS[dct][Receiver >> 1][DQS_WRITEDIR][lane] = current_write_dqs_delay[lane];

				print_debug_dqs("\t\t\t\tTrainDQSRdWrPos: 145 largest write passing region ", best_count, 4);
				print_debug_dqs("\t\t\t\tTrainDQSRdWrPos: 146 largest write passing region start ", best_pos, 4);
			} else {
				/* Restore current settings of other (previously trained) lanes to the active array */
				memcpy(current_write_dqs_delay, initial_write_data_timing, sizeof(current_write_data_delay));

				/* Reprogram the Write DQS Timing Control register with the original settings */
				write_dqs_write_data_timing_registers(current_write_dqs_delay, dev, dct, dimm, index_reg);
			}

			if (passing_read_dqs_delay_found && passing_write_dqs_delay_found)
				passing_dqs_delay_found[lane] = 1;
		}

#ifdef PRINT_PASS_FAIL_BITMAPS
		for (lane = lane_start; lane < lane_end; lane++) {
			for (write_iter = 0; write_iter < 32; write_iter++) {
				for (read_iter = 0; read_iter < 48; read_iter++) {
					if (dqs_results_array[Receiver & 0x1][lane - lane_start][write_iter][read_iter]) {
						printk(BIOS_DEBUG, "+");
					} else {
						if (read_iter < 16)
							printk(BIOS_DEBUG, ":");
						else
							printk(BIOS_DEBUG, ".");
					}
				}
				printk(BIOS_DEBUG, "\n");
			}
			printk(BIOS_DEBUG, "\n\n");
		}
#endif

		/* Flag failure(s) if present */
		for (lane = lane_start; lane < lane_end; lane++) {
			if (!passing_dqs_delay_found[lane]) {
				print_debug_dqs("\t\t\t\tTrainDQSRdWrPos: 121 Unable to find passing region for lane ", lane, 2);

				/* Flag absence of passing window */
				Errors |= 1 << SB_NODQSPOS;
			}
		}

		pDCTstat->TrainErrors |= Errors;
		pDCTstat->ErrStatus |= Errors;

#if DQS_TRAIN_DEBUG > 0
		{
			u8 val;
			u8 i;
			u8 ChannelDTD, ReceiverDTD, Dir;
			u8 *p;

			for (Dir = 0; Dir < 2; Dir++) {
				if (Dir == 1) {
					printk(BIOS_DEBUG, "TrainDQSRdWrPos: CH_D_DIR_B_DQS WR:\n");
				} else {
					printk(BIOS_DEBUG, "TrainDQSRdWrPos: CH_D_DIR_B_DQS RD:\n");
				}
				for (ChannelDTD = 0; ChannelDTD < 2; ChannelDTD++) {
					printk(BIOS_DEBUG, "Channel: %02x\n", ChannelDTD);
					for (ReceiverDTD = 0; ReceiverDTD < MAX_CS_SUPPORTED; ReceiverDTD += 2) {
						printk(BIOS_DEBUG, "\t\tReceiver: %02x:", ReceiverDTD);
						p = pDCTstat->CH_D_DIR_B_DQS[ChannelDTD][ReceiverDTD >> 1][Dir];
						for (i = 0; i < 8; i++) {
							val  = p[i];
							printk(BIOS_DEBUG, " %02x", val);
						}
						printk(BIOS_DEBUG, "\n");
					}
				}
			}

		}
#endif
	}

	/* Return 1 on success, 0 on failure */
	return !Errors;
}

/* DQS Receiver Enable Cycle Training
 * Algorithm detailed in the Fam15h BKDG Rev. 3.14 section 2.10.5.8.3
 */
static void TrainDQSReceiverEnCyc_D_Fam15(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat)
{
	u32 Errors;
	u8 Receiver;
	u8 _DisableDramECC = 0;
	u8 _Wrap32Dis = 0, _SSE2 = 0;

	u32 addr;
	u32 cr4;
	u32 lo, hi;

	uint8_t dct;
	uint8_t prev;
	uint8_t dimm;
	uint8_t lane;
	uint32_t dword;
	uint32_t rx_en_offset;
	uint8_t internal_lane;
	uint8_t dct_training_success;
	uint8_t lane_success_count;
	uint16_t initial_phy_phase_delay[MAX_BYTE_LANES];
	uint16_t current_phy_phase_delay[MAX_BYTE_LANES];
	uint16_t current_read_dqs_delay[MAX_BYTE_LANES];
	uint8_t lane_training_success[MAX_BYTE_LANES];
	uint8_t dqs_results_array[1024];

 	uint16_t ren_step = 0x40;
	uint32_t index_reg = 0x98;
	uint32_t dev = pDCTstat->dev_dct;

	uint8_t lane_count;
	lane_count = get_available_lane_count(pMCTstat, pDCTstat);

	print_debug_dqs("\nTrainDQSReceiverEnCyc: Node_ID ", pDCTstat->Node_ID, 0);
	cr4 = read_cr4();
	if (cr4 & (1<<9)) {
		_SSE2 = 1;
	}
	cr4 |= (1<<9);		/* OSFXSR enable SSE2 */
	write_cr4(cr4);

	addr = HWCR;
	_RDMSR(addr, &lo, &hi);
	if (lo & (1<<17)) {
		_Wrap32Dis = 1;
	}
	lo |= (1<<17);		/* HWCR.wrap32dis */
	_WRMSR(addr, lo, hi);	/* allow 64-bit memory references in real mode */

	/* Disable ECC correction of reads on the dram bus. */
	_DisableDramECC = mct_DisableDimmEccEn_D(pMCTstat, pDCTstat);

	Errors = 0;

	for (dct = 0; dct < 2; dct++) {
		/* Program D18F2x9C_x0D0F_E003_dct[1:0][DisAutoComp, DisablePredriverCal] */
		/* NOTE: DisablePredriverCal only takes effect when set on DCT 0 */
		dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0fe003);
		dword &= ~(0x3 << 13);
		dword |= (0x1 << 13);
		Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0fe003, dword);
	}

	for (dct = 0; dct < 2; dct++) {
		/* 2.10.5.6 */
		fam15EnableTrainingMode(pMCTstat, pDCTstat, dct, 1);

		/* 2.10.5.8.3 */
		Receiver = mct_InitReceiver_D(pDCTstat, dct);

		/* Indicate success unless training the DCT explicitly fails */
		dct_training_success = 1;

		/* There are four receiver pairs, loosely associated with chipselects.
		 * This is essentially looping over each DIMM.
		 */
		for (; Receiver < 8; Receiver += 2) {
			dimm = (Receiver >> 1);

			if (!mct_RcvrRankEnabled_D(pMCTstat, pDCTstat, dct, Receiver)) {
				continue;
			}

			/* Initialize variables */
			memset(lane_training_success, 0, sizeof(lane_training_success));
			memset(current_phy_phase_delay, 0, sizeof(current_phy_phase_delay));

			/* 2.10.5.8.3 (2) */
			read_dqs_receiver_enable_control_registers(initial_phy_phase_delay, dev, dct, dimm, index_reg);

			/* Reset the read data timing registers to 1UI before calculating MaxRdLatency */
			for (internal_lane = 0; internal_lane < MAX_BYTE_LANES; internal_lane++)
				current_read_dqs_delay[internal_lane] = 0x20;
			write_dqs_read_data_timing_registers(current_read_dqs_delay, dev, dct, dimm, index_reg);

			for (lane = 0; lane < lane_count; lane++) {
				/* Initialize variables */
				memset(dqs_results_array, 0, sizeof(dqs_results_array));
				lane_success_count = 0;

				/* 2.10.5.8.3 (1) */
				dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f0030 | (lane << 8));
				dword |= (0x1 << 8);								/* BlockRxDqsLock = 1 */
				Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f0030 | (lane << 8), dword);

				/* 2.10.5.8.3 (3) */
				rx_en_offset = (initial_phy_phase_delay[lane] + 0x10) % 0x40;

				/* 2.10.5.8.3 (4) */
#if DQS_TRAIN_DEBUG > 0
				printk(BIOS_DEBUG, "TrainDQSReceiverEnCyc_D_Fam15 Receiver %d lane %d initial phy delay %04x: iterating from %04x to %04x\n", Receiver, lane, initial_phy_phase_delay[lane], rx_en_offset, 0x3ff);
#endif
				for (current_phy_phase_delay[lane] = rx_en_offset; current_phy_phase_delay[lane] < 0x3ff; current_phy_phase_delay[lane] += ren_step) {
#if DQS_TRAIN_DEBUG > 0
					printk(BIOS_DEBUG, "%s: Receiver %d lane %d current phy delay: %04x\n", __func__, Receiver, lane, current_phy_phase_delay[lane]);
#endif

					/* 2.10.5.8.3 (4 A) */
					write_dqs_receiver_enable_control_registers(current_phy_phase_delay, dev, dct, dimm, index_reg);

					/* Calculate and program MaxRdLatency */
					Calc_SetMaxRdLatency_D_Fam15(pMCTstat, pDCTstat, dct, 0);

					/* 2.10.5.8.3 (4 B) */
					dqs_results_array[current_phy_phase_delay[lane]] = TrainDQSRdWrPos_D_Fam15(pMCTstat, pDCTstat, dct, Receiver, Receiver + 2, lane, lane + 1);

					if (dqs_results_array[current_phy_phase_delay[lane]])
						lane_success_count++;

					/* Don't bother testing larger values if the end of the passing window was already found */
					if (!dqs_results_array[current_phy_phase_delay[lane]] && (lane_success_count > 1))
						break;
				}

				uint16_t phase_delay;
				for (phase_delay = 0; phase_delay < 0x3ff; phase_delay++)
					if (dqs_results_array[phase_delay])
						lane_training_success[lane] = 1;

				if (!lane_training_success[lane]) {
					if (pDCTstat->tcwl_delay[dct] >= 1) {
						Errors |= 1 << SB_FatalError;
						printk(BIOS_ERR, "%s: lane %d failed to train!  "
							"Training for receiver %d on DCT %d aborted\n",
							__func__, lane, Receiver, dct);
					}

					/* Restore BlockRxDqsLock setting to normal operation in preparation for retraining */
					dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f0030 | (lane << 8));
					dword &= ~(0x1 << 8);								/* BlockRxDqsLock = 0 */
					Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f0030 | (lane << 8), dword);

					break;
				}

#ifdef PRINT_PASS_FAIL_BITMAPS
				for (phase_delay = 0; phase_delay < 0x3ff; phase_delay++) {
					if (dqs_results_array[phase_delay])
						printk(BIOS_DEBUG, "+");
					else
						printk(BIOS_DEBUG, ".");
				}
				printk(BIOS_DEBUG, "\n");
#endif

				/* 2.10.5.8.3 (5) */
				prev = dqs_results_array[rx_en_offset];
				for (current_phy_phase_delay[lane] = rx_en_offset + ren_step; current_phy_phase_delay[lane] < 0x3ff; current_phy_phase_delay[lane] += ren_step) {
					if ((dqs_results_array[current_phy_phase_delay[lane]] == 0) && (prev == 1)) {
						/* Restore last known good delay */
						current_phy_phase_delay[lane] -= ren_step;

						/* 2.10.5.8.3 (5 A B) */
						if (current_phy_phase_delay[lane] < 0x10)
							current_phy_phase_delay[lane] = 0x0;
						else
							current_phy_phase_delay[lane] -= 0x10;

						/* Update hardware registers with final values */
						write_dqs_receiver_enable_control_registers(current_phy_phase_delay, dev, dct, dimm, index_reg);
						TrainDQSRdWrPos_D_Fam15(pMCTstat, pDCTstat, dct, Receiver, Receiver + 2, lane, lane + 1);
						break;
					}
					prev = dqs_results_array[current_phy_phase_delay[lane]];
				}

				/* 2.10.5.8.3 (6) */
				dword = Get_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f0030 | (lane << 8));
				dword &= ~(0x1 << 8);								/* BlockRxDqsLock = 0 */
				Set_NB32_index_wait_DCT(dev, dct, index_reg, 0x0d0f0030 | (lane << 8), dword);
			}

			for (lane = 0; lane < lane_count; lane++) {
				if (!lane_training_success[lane]) {
					dct_training_success = 0;
					Errors |= 1 << SB_NODQSPOS;
				}
			}

#if DQS_TRAIN_DEBUG > 0
			printk(BIOS_DEBUG, "TrainDQSReceiverEnCyc_D_Fam15 DQS receiver enable timing: ");
			for (lane = 0; lane < lane_count; lane++) {
				printk(BIOS_DEBUG, " %03x", current_phy_phase_delay[lane]);
			}
			printk(BIOS_DEBUG, "\n");
#endif
		}

		if (!dct_training_success) {
			if (pDCTstat->tcwl_delay[dct] < 1) {
				/* Increase TCWL */
				pDCTstat->tcwl_delay[dct]++;
				/* Request retraining */
				Errors |= 1 << SB_RetryConfigTrain;
			}
		}
	}

	pDCTstat->TrainErrors |= Errors;
	pDCTstat->ErrStatus |= Errors;

#if DQS_TRAIN_DEBUG > 0
	{
		u8 val;
		u8 i;
		u8 ChannelDTD, ReceiverDTD, Dir;
		u8 *p;

		for (Dir = 0; Dir < 2; Dir++) {
			if (Dir == 1) {
				printk(BIOS_DEBUG, "TrainDQSRdWrPos: CH_D_DIR_B_DQS WR:\n");
			} else {
				printk(BIOS_DEBUG, "TrainDQSRdWrPos: CH_D_DIR_B_DQS RD:\n");
			}
			for (ChannelDTD = 0; ChannelDTD < 2; ChannelDTD++) {
				printk(BIOS_DEBUG, "Channel: %02x\n", ChannelDTD);
				for (ReceiverDTD = 0; ReceiverDTD < MAX_CS_SUPPORTED; ReceiverDTD += 2) {
					printk(BIOS_DEBUG, "\t\tReceiver: %02x:", ReceiverDTD);
					p = pDCTstat->CH_D_DIR_B_DQS[ChannelDTD][ReceiverDTD >> 1][Dir];
					for (i = 0; i < 8; i++) {
						val  = p[i];
						printk(BIOS_DEBUG, " %02x", val);
					}
					printk(BIOS_DEBUG, "\n");
				}
			}
		}

	}
#endif
	if (_DisableDramECC) {
		mct_EnableDimmEccEn_D(pMCTstat, pDCTstat, _DisableDramECC);
	}
	if (!_Wrap32Dis) {
		addr = HWCR;
		_RDMSR(addr, &lo, &hi);
		lo &= ~(1<<17);		/* restore HWCR.wrap32dis */
		_WRMSR(addr, lo, hi);
	}
	if (!_SSE2) {
		cr4 = read_cr4();
		cr4 &= ~(1<<9);		/* restore cr4.OSFXSR */
		write_cr4(cr4);
	}

	printk(BIOS_DEBUG, "TrainDQSReceiverEnCyc: Status %x\n", pDCTstat->Status);
	printk(BIOS_DEBUG, "TrainDQSReceiverEnCyc: TrainErrors %x\n", pDCTstat->TrainErrors);
	printk(BIOS_DEBUG, "TrainDQSReceiverEnCyc: ErrStatus %x\n", pDCTstat->ErrStatus);
	printk(BIOS_DEBUG, "TrainDQSReceiverEnCyc: ErrCode %x\n", pDCTstat->ErrCode);
	printk(BIOS_DEBUG, "TrainDQSReceiverEnCyc: Done\n\n");
}

static void SetupDqsPattern_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u32 *buffer)
{
	/* 1. Set the Pattern type (0 or 1) in DCTStatstruc.Pattern
	 * 2. Copy the pattern from ROM to Cache, aligning on 16 byte boundary
	 * 3. Set the ptr to Cacheable copy in DCTStatstruc.PtrPatternBufA
	 */

	u32 *buf;
	u16 i;

	buf = (u32 *)(((u32)buffer + 0x10) & (0xfffffff0));
	if (pDCTstat->Status & (1<<SB_128bitmode)) {
		pDCTstat->Pattern = 1;	/* 18 cache lines, alternating qwords */
		for (i = 0; i < 16*18; i++)
			buf[i] = TestPatternJD1b_D[i];
	} else {
		pDCTstat->Pattern = 0;	/* 9 cache lines, sequential qwords */
		for (i = 0; i < 16*9; i++)
			buf[i] = TestPatternJD1a_D[i];
	}
	pDCTstat->PtrPatternBufA = (u32)buf;
}

static void StoreDQSDatStrucVal_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 ChipSel)
{
	/* Store the DQSDelay value, found during a training sweep, into the DCT
	 * status structure for this node
	 */

	/* When 400, 533, 667, it will support dimm0/1/2/3,
	 * and set conf for dimm0, hw will copy to dimm1/2/3
	 * set for dimm1, hw will copy to dimm3
	 * Rev A/B only support DIMM0/1 when 800MHz and above + 0x100 to next dimm
	 * Rev C support DIMM0/1/2/3 when 800MHz and above  + 0x100 to next dimm
	 */

	/* FindDQSDatDimmVal_D is not required since we use an array */
	u8 dn = 0;

	dn = ChipSel>>1; /* if odd or even logical DIMM */

	pDCTstat->CH_D_DIR_B_DQS[pDCTstat->Channel][dn][pDCTstat->Direction][pDCTstat->ByteLane] =
					pDCTstat->DQSDelay;
}

static void GetDQSDatStrucVal_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 ChipSel)
{
	u8 dn = 0;

	/* When 400, 533, 667, it will support dimm0/1/2/3,
	 * and set conf for dimm0, hw will copy to dimm1/2/3
	 * set for dimm1, hw will copy to dimm3
	 * Rev A/B only support DIMM0/1 when 800MHz and above + 0x100 to next dimm
	 * Rev C support DIMM0/1/2/3 when 800MHz and above  + 0x100 to next dimm
	 */

	/* FindDQSDatDimmVal_D is not required since we use an array */
	dn = ChipSel >> 1; /*if odd or even logical DIMM */

	pDCTstat->DQSDelay =
		pDCTstat->CH_D_DIR_B_DQS[pDCTstat->Channel][dn][pDCTstat->Direction][pDCTstat->ByteLane];
}

/* FindDQSDatDimmVal_D is not required since we use an array */

void proc_IOCLFLUSH_D(u32 addr_hi)
{
	SetTargetWTIO_D(addr_hi);
	proc_CLFLUSH(addr_hi);
	ResetTargetWTIO_D();
}

u8 ChipSelPresent_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat,
				u8 Channel, u8 ChipSel)
{
	u32 val;
	u32 reg;
	u32 dev = pDCTstat->dev_dct;
	uint8_t dct = 0;
	u8 ret = 0;

	if (!pDCTstat->GangedMode)
		dct = Channel;
	else
		dct = 0;

	if (ChipSel < MAX_CS_SUPPORTED) {
		reg = 0x40 + (ChipSel << 2);
		val = Get_NB32_DCT(dev, dct, reg);
		if (val & (1 << 0))
			ret = 1;
	}

	return ret;
}

/* proc_CLFLUSH_D located in mct_gcc.h */

static void WriteDQSTestPattern_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat,
					u32 TestAddr_lo)
{
	/* Write a pattern of 72 bit times (per DQ), to test dram functionality.
	 * The pattern is a stress pattern which exercises both ISI and
	 * crosstalk.  The number of cache lines to fill is dependent on DCT
	 * width mode and burstlength.
	 * Mode BL  Lines Pattern no.
	 * ----+---+-------------------
	 * 64	4	  9	0
	 * 64	8	  9	0
	 * 64M	4	  9	0
	 * 64M	8	  9	0
	 * 128	4	  18	1
	 * 128	8	  N/A	-
	 */
	if (pDCTstat->Pattern == 0)
		WriteL9TestPattern_D(pDCTstat, TestAddr_lo);
	else
		WriteL18TestPattern_D(pDCTstat, TestAddr_lo);
}

static void WriteL18TestPattern_D(struct DCTStatStruc *pDCTstat,
					u32 TestAddr_lo)
{
	u8 *buf;

	buf = (u8 *)pDCTstat->PtrPatternBufA;
	WriteLNTestPattern(TestAddr_lo, buf, 18);

}

static void WriteL9TestPattern_D(struct DCTStatStruc *pDCTstat,
					u32 TestAddr_lo)
{
	u8 *buf;

	buf = (u8 *)pDCTstat->PtrPatternBufA;
	WriteLNTestPattern(TestAddr_lo, buf, 9);
}

static u16 CompareDQSTestPattern_D(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat, u32 addr_lo)
{
	/* Compare a pattern of 72 bit times (per DQ), to test dram functionality.
	 * The pattern is a stress pattern which exercises both ISI and
	 * crosstalk.  The number of cache lines to fill is dependent on DCT
	 * width mode and burstlength.
	 * Mode BL  Lines Pattern no.
	 * ----+---+-------------------
	 * 64	4	  9	0
	 * 64	8	  9	0
	 * 64M	4	  9	0
	 * 64M	8	  9	0
	 * 128	4	  18	1
	 * 128	8	  N/A	-
	 */

	u32 *test_buf;
	u16 MEn1Results, bitmap;
	u8 bytelane;
	u8 i;
	u32 value;
	u8 j;
	u32 value_test;
	u32 value_r = 0, value_r_test = 0;
	u8 pattern, channel, BeatCnt;
	struct DCTStatStruc *ptrAddr;

	ptrAddr = pDCTstat;
	pattern = pDCTstat->Pattern;
	channel = pDCTstat->Channel;
	test_buf = (u32 *)pDCTstat->PtrPatternBufA;

	if (pattern && channel) {
		addr_lo += 8; /* second channel */
		test_buf += 2;
	}

	bytelane = 0;		/* bytelane counter */
	bitmap = 0xFFFF;	/* bytelane test bitmap, 1 = pass */
	MEn1Results = 0xFFFF;
	BeatCnt = 0;
	for (i = 0; i < (9 * 64 / 4); i++) { /* sizeof testpattern. /4 due to next loop */
		value = read32_fs(addr_lo);
		value_test = *test_buf;

		print_debug_dqs_pair("\t\t\t\t\t\ttest_buf = ", (u32)test_buf, " value = ", value_test, 7);
		print_debug_dqs_pair("\t\t\t\t\t\ttaddr_lo = ", addr_lo, " value = ", value, 7);

		if (pDCTstat->Direction == DQS_READDIR) {
			if (BeatCnt != 0) {
				value_r = *test_buf;
				if (pattern) /* if multi-channel */
					value_r_test = read32_fs(addr_lo - 16);
				else
					value_r_test = read32_fs(addr_lo - 8);
			}
			print_debug_dqs_pair("\t\t\t\t\t\t\ttest_buf = ", (u32)test_buf, " value_r_test = ", value_r, 7);
			print_debug_dqs_pair("\t\t\t\t\t\t\ttaddr_lo = ", addr_lo, " value_r = ", value_r_test, 7);
		}

		for (j = 0; j < (4 * 8); j += 8) { /* go through a 32bit data, on 1 byte step. */
			if (((value >> j) & 0xff) != ((value_test >> j) & 0xff)) {
				bitmap &= ~(1 << bytelane);
			}

			if (pDCTstat->Direction == DQS_READDIR) {
				if (BeatCnt != 0) {
					if (((value_r >> j) & 0xff) != ((value_r_test >> j) & 0xff)) {
						MEn1Results &= ~(1 << bytelane);
					}
				}
			}
			bytelane++;
			bytelane &= 0x7;
		}

		print_debug_dqs("\t\t\t\t\t\tbitmap = ", bitmap, 7);
		print_debug_dqs("\t\t\t\t\t\tMEn1Results = ", MEn1Results, 7);

		if (!bitmap)
			break;

		if (bytelane == 0) {
			BeatCnt += 4;
			if (!(pDCTstat->Status & (1 << SB_128bitmode))) {
				if (BeatCnt == 8) BeatCnt = 0; /* 8 beat burst */
			} else {
				if (BeatCnt == 4) BeatCnt = 0; /* 4 beat burst */
			}
			if (pattern == 1) { /* dual channel */
				addr_lo += 8; /* skip over other channel's data */
				test_buf += 2;
			}
		}
		addr_lo += 4;
		test_buf += 1;
	}

	if (pDCTstat->Direction == DQS_READDIR) {
		bitmap &= 0xFF;
		bitmap |= MEn1Results << 8;
	}

	print_debug_dqs("\t\t\t\t\t\tbitmap = ", bitmap, 6);

	return bitmap;
}

static void FlushDQSTestPattern_D(struct DCTStatStruc *pDCTstat,
					u32 addr_lo)
{
	/* Flush functions in mct_gcc.h */
	if (pDCTstat->Pattern == 0) {
		FlushDQSTestPattern_L9(addr_lo);
	} else {
		FlushDQSTestPattern_L18(addr_lo);
	}
}

void SetTargetWTIO_D(u32 TestAddr)
{
	u32 lo, hi;
	hi = TestAddr >> 24;
	lo = TestAddr << 8;
	_WRMSR(0xC0010016, lo, hi);		/* IORR0 Base */
	hi = 0xFF;
	lo = 0xFC000800;			/* 64MB Mask */
	_WRMSR(0xC0010017, lo, hi);		/* IORR0 Mask */
}

void ResetTargetWTIO_D(void)
{
	u32 lo, hi;

	hi = 0;
	lo = 0;
	_WRMSR(0xc0010017, lo, hi); /* IORR0 Mask */
}

u32 SetUpperFSbase(u32 addr_hi)
{
	/* Set the upper 32-bits of the Base address, 4GB aligned) for the
	 * FS selector.
	 */
	u32 lo, hi;
	u32 addr;
	lo = 0;
	hi = addr_hi>>24;
	addr = FS_Base;
	_WRMSR(addr, lo, hi);
	return addr_hi << 8;
}

void ResetDCTWrPtr_D(u32 dev, uint8_t dct, u32 index_reg, u32 index)
{
	u32 val;

	val = Get_NB32_index_wait_DCT(dev, dct, index_reg, index);
	Set_NB32_index_wait_DCT(dev, dct, index_reg, index, val);
}

void mct_TrainDQSPos_D(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstatA)
{
	u8 Node;
	u8 ChipSel;
	struct DCTStatStruc *pDCTstat;

	for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
		pDCTstat = pDCTstatA + Node;
		if (pDCTstat->DCTSysLimit) {
			if (is_fam15h()) {
				TrainDQSReceiverEnCyc_D_Fam15(pMCTstat, pDCTstat);
			} else {
				TrainDQSRdWrPos_D_Fam10(pMCTstat, pDCTstat);
				for (ChipSel = 0; ChipSel < MAX_CS_SUPPORTED; ChipSel += 2) {
					SetEccDQSRdWrPos_D_Fam10(pMCTstat, pDCTstat, ChipSel);
				}
			}
		}
	}
}

/* mct_BeforeTrainDQSRdWrPos_D
 * Function is inline.
 */
u8 mct_DisableDimmEccEn_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat)
{
	u8 _DisableDramECC = 0;
	u32 val;
	u32 reg;
	u32 dev;

	/*Disable ECC correction of reads on the dram bus. */

	dev = pDCTstat->dev_dct;
	reg = 0x90;
	val = Get_NB32_DCT(dev, 0, reg);
	if (val & (1<<DimmEcEn)) {
		_DisableDramECC |= 0x01;
		val &= ~(1<<DimmEcEn);
		Set_NB32_DCT(dev, 0, reg, val);
	}
	if (!pDCTstat->GangedMode) {
		val = Get_NB32_DCT(dev, 1, reg);
		if (val & (1<<DimmEcEn)) {
			_DisableDramECC |= 0x02;
			val &= ~(1<<DimmEcEn);
			Set_NB32_DCT(dev, 1, reg, val);
		}
	}
	return _DisableDramECC;
}

void mct_EnableDimmEccEn_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 _DisableDramECC)
{
	u32 val;
	u32 dev;

	/* Enable ECC correction if it was previously disabled */
	dev = pDCTstat->dev_dct;

	if ((_DisableDramECC & 0x01) == 0x01) {
		val = Get_NB32_DCT(dev, 0, 0x90);
		val |= (1<<DimmEcEn);
		Set_NB32_DCT(dev, 0, 0x90, val);
	}
	if ((_DisableDramECC & 0x02) == 0x02) {
		val = Get_NB32_DCT(dev, 1, 0x90);
		val |= (1<<DimmEcEn);
		Set_NB32_DCT(dev, 1, 0x90, val);
	}
}

/*
 * Set DQS delay value to related register
 */
static void mct_SetDQSDelayCSR_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 ChipSel)
{
	u8 ByteLane;
	u32 val;
	u32 index_reg = 0x98;
	u8 shift;
	u32 dqs_delay = (u32)pDCTstat->DQSDelay;
	u32 dev = pDCTstat->dev_dct;
	u32 index;

	ByteLane = pDCTstat->ByteLane;

	if (!(pDCTstat->DqsRdWrPos_Saved & (1 << ByteLane))) {
		/* Channel is offset */
		if (ByteLane < 4) {
			index = 1;
		} else if (ByteLane <8) {
			index = 2;
		} else {
			index = 3;
		}

		if (pDCTstat->Direction == DQS_READDIR) {
			index += 4;
		}

		/* get the proper register index */
		shift = ByteLane % 4;
		shift <<= 3; /* get bit position of bytelane, 8 bit */

		index += (ChipSel>>1) << 8;

		val = Get_NB32_index_wait_DCT(dev, pDCTstat->Channel, index_reg, index);
		if (ByteLane < 8) {
			if (pDCTstat->Direction == DQS_WRITEDIR) {
				dqs_delay += pDCTstat->persistentData.CH_D_B_TxDqs[pDCTstat->Channel][ChipSel>>1][ByteLane];
			} else {
				dqs_delay <<= 1;
			}
		}
		val &= ~(0x7f << shift);
		val |= (dqs_delay << shift);
		Set_NB32_index_wait_DCT(dev, pDCTstat->Channel, index_reg, index, val);
	}
}

u8 mct_RcvrRankEnabled_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat,
				u8 Channel, u8 ChipSel)
{
	u8 ret;

	ret = ChipSelPresent_D(pMCTstat, pDCTstat, Channel, ChipSel);
	return ret;
}

u32 mct_GetRcvrSysAddr_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat,
				u8 channel, u8 receiver, u8 *valid)
{
	return mct_GetMCTSysAddr_D(pMCTstat, pDCTstat, channel, receiver, valid);
}

u32 mct_GetMCTSysAddr_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat,
				u8 Channel, u8 receiver, u8 *valid)
{
	u32 val;
	uint8_t dct = 0;
	u32 reg;
	u32 dword;
	u32 dev = pDCTstat->dev_dct;

	*valid = 0;


	if (!pDCTstat->GangedMode) {
		dct = Channel;
	}

	/* get the local base addr of the chipselect */
	reg = 0x40 + (receiver << 2);
	val = Get_NB32_DCT(dev, dct, reg);

	val &= ~0xe007c01f;

	/* unganged mode DCT0+DCT1, sys addr of DCT1 = node
	 * base+DctSelBaseAddr+local ca base*/
	if ((Channel) && (pDCTstat->GangedMode == 0) && (pDCTstat->DIMMValidDCT[0] > 0)) {
		reg = 0x110;
		dword = Get_NB32(dev, reg);
		dword &= 0xfffff800;
		dword <<= 8;	/* scale [47:27] of F2x110[31:11] to [39:8]*/
		val += dword;

		/* if DCTSelBaseAddr < Hole, and eax > HoleBase, then add Hole size to test address */
		if ((val >= pDCTstat->DCTHoleBase) && (pDCTstat->DCTHoleBase > dword)) {
			dword = (~(pDCTstat->DCTHoleBase >> (24 - 8)) + 1) & 0xFF;
			dword <<= (24 - 8);
			val += dword;
		}
	} else {
		/* sys addr = node base+local cs base */
		val += pDCTstat->DCTSysBase;

		/* New stuff */
		if (pDCTstat->DCTHoleBase && (val >= pDCTstat->DCTHoleBase)) {
			val -= pDCTstat->DCTSysBase;
			dword = Get_NB32(pDCTstat->dev_map, 0xF0); /* get Hole Offset */
			val += (dword & 0x0000ff00) << (24-8-8);
		}
	}

	/* New stuff */
	val += ((1 << 21) >> 8);	/* Add 2MB offset to avoid compat area */
	if (val >= MCT_TRNG_KEEPOUT_START) {
		while (val < MCT_TRNG_KEEPOUT_END)
			val += (1 << (15-8));	/* add 32K */
	}

	/* Add a node seed */
	val += (((1 * pDCTstat->Node_ID) << 20) >> 8);	/* Add 1MB per node to avoid aliases */

	/* HW remap disabled? */
	if (!(pDCTstat->Status & (1 << SB_HWHole))) {
		if (!(pDCTstat->Status & (1 << SB_SWNodeHole))) {
			/* SW memhole disabled */
			u32 lo, hi;
			_RDMSR(TOP_MEM, &lo, &hi);
			lo >>= 8;
			if ((val >= lo) && (val < _4GB_RJ8)) {
				val = 0;
				*valid = 0;
				goto exitGetAddr;
			} else {
				*valid = 1;
				goto exitGetAddrWNoError;
			}
		} else {
			*valid = 1;
			goto exitGetAddrWNoError;
		}
	} else {
		*valid = 1;
		goto exitGetAddrWNoError;
	}

exitGetAddrWNoError:

	/* Skip if Address is in UMA region */
	dword = pMCTstat->Sub4GCacheTop;
	dword >>= 8;
	if (dword != 0) {
		if ((val >= dword) && (val < _4GB_RJ8)) {
			val = 0;
			*valid = 0;
		} else {
			*valid = 1;
		}
	}
	print_debug_dqs("mct_GetMCTSysAddr_D: receiver ", receiver, 2);
	print_debug_dqs("mct_GetMCTSysAddr_D: Channel ", Channel, 2);
	print_debug_dqs("mct_GetMCTSysAddr_D: base_addr ", val, 2);
	print_debug_dqs("mct_GetMCTSysAddr_D: valid ", *valid, 2);
	print_debug_dqs("mct_GetMCTSysAddr_D: status ", pDCTstat->Status, 2);
	print_debug_dqs("mct_GetMCTSysAddr_D: SysBase ", pDCTstat->DCTSysBase, 2);
	print_debug_dqs("mct_GetMCTSysAddr_D: HoleBase ", pDCTstat->DCTHoleBase, 2);
	print_debug_dqs("mct_GetMCTSysAddr_D: Cachetop ", pMCTstat->Sub4GCacheTop, 2);

exitGetAddr:
	return val;
}

void mct_Write1LTestPattern_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat,
				u32 TestAddr, u8 pattern)
{

	u8 *buf;

	/* Issue the stream of writes. When F2x11C[MctWrLimit] is reached
	 * (or when F2x11C[FlushWr] is set again), all the writes are written
	 * to DRAM.
	 */

	SetUpperFSbase(TestAddr);

	if (pattern)
		buf = (u8 *)pDCTstat->PtrPatternBufB;
	else
		buf = (u8 *)pDCTstat->PtrPatternBufA;

	WriteLNTestPattern(TestAddr << 8, buf, 1);
}

void mct_Read1LTestPattern_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u32 addr)
{
	u32 value;

	/* BIOS issues the remaining (Ntrain - 2) reads after checking that
	 * F2x11C[PrefDramTrainMode] is cleared. These reads must be to
	 * consecutive cache lines (i.e., 64 bytes apart) and must not cross
	 * a naturally aligned 4KB boundary. These reads hit the prefetches and
	 * read the data from the prefetch buffer.
	 */

	/* get data from DIMM */
	SetUpperFSbase(addr);

	/* 1st move causes read fill (to exclusive or shared)*/
	value = read32_fs(addr << 8);
}
