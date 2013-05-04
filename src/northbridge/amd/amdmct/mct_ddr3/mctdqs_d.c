/*
 * This file is part of the coreboot project.
 *
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

static void CalcEccDQSPos_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u16 like,
				u8 scale, u8 ChipSel);
static void GetDQSDatStrucVal_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 ChipSel);
static u8 MiddleDQS_D(u8 min, u8 max);
static void TrainReadDQS_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat,
				u8 cs_start);
static void TrainWriteDQS_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat,
				u8 cs_start);
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
static void SetTargetWTIO_D(u32 TestAddr);
static void ResetTargetWTIO_D(void);
static void ReadDQSTestPattern_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat,
					u32 TestAddr_lo);
static void mctEngDQSwindow_Save_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 ChipSel,
					u8 RnkDlyFilterMin, u8 RnkDlyFilterMax);
void ResetDCTWrPtr_D(u32 dev, u32 index_reg, u32 index);
u8 mct_DisableDimmEccEn_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat);
static void mct_SetDQSDelayCSR_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat,
					u8 ChipSel);
static void mct_SetDQSDelayAllCSR_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat,
					u8 cs_start);
u32 mct_GetMCTSysAddr_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 Channel,
				u8 receiver, u8 *valid);
static void SetupDqsPattern_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat,
				u32 *buffer);

static void StoreWrRdDQSDatStrucVal_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 ChipSel,
				      u8 RnkDlyFilterMin, u8 RnkDlyFilterMax);

static void StoreDQSDatStrucVal_D(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat, u8 ChipSel);

#define DQS_TRAIN_DEBUG 0

static void print_debug_dqs(const char *str, u32 val, u8 level)
{
#if DQS_TRAIN_DEBUG > 0
	if (DQS_TRAIN_DEBUG >= level) {
		printk(BIOS_DEBUG, "%s%x\n", str, val);
	}
#endif
}

static void print_debug_dqs_pair(const char *str, u32 val, const char *str2, u32 val2, u8 level)
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
			val = Get_NB32(pDCTstat->dev_dct, 0x78);
			val |= 1 <<DqsRcvEnTrain;
			Set_NB32(pDCTstat->dev_dct, 0x78, val);
			val = Get_NB32(pDCTstat->dev_dct, 0x78 + 0x100);
			val |= 1 <<DqsRcvEnTrain;
			Set_NB32(pDCTstat->dev_dct, 0x78 + 0x100, val);
			mct_TrainRcvrEn_D(pMCTstat, pDCTstat, Pass);
		}
	}
}

static void SetEccDQSRdWrPos_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 ChipSel)
{
	u8 channel;
	u8 direction;

	for (channel = 0; channel < 2; channel++){
		for (direction = 0; direction < 2; direction++) {
			pDCTstat->Channel = channel;	/* Channel A or B */
			pDCTstat->Direction = direction; /* Read or write */
			CalcEccDQSPos_D(pMCTstat, pDCTstat, pDCTstat->CH_EccDQSLike[channel], pDCTstat->CH_EccDQSScale[channel], ChipSel);
			print_debug_dqs_pair("\t\tSetEccDQSRdWrPos: channel ", channel, direction==DQS_READDIR? " R dqs_delay":" W dqs_delay",	pDCTstat->DQSDelay, 2);
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
	u8 DQSDelay0, DQSDelay1;
	u16 DQSDelay;

	if (pDCTstat->Status & (1 << SB_Registered)) {
		return;
	}

	pDCTstat->ByteLane = like & 0xff;
	GetDQSDatStrucVal_D(pMCTstat, pDCTstat, ChipSel);
	DQSDelay0 = pDCTstat->DQSDelay;

	pDCTstat->ByteLane = (like >> 8) & 0xff;
	GetDQSDatStrucVal_D(pMCTstat, pDCTstat, ChipSel);
	DQSDelay1 = pDCTstat->DQSDelay;

	if (DQSDelay0>DQSDelay1) {
		DQSDelay = DQSDelay0 - DQSDelay1;
	} else {
		DQSDelay = DQSDelay1 - DQSDelay0;
	}

	DQSDelay = DQSDelay * (~scale);

	DQSDelay += 0x80;	/* round it */

	DQSDelay >>= 8;		/* 256 */

	if (DQSDelay0>DQSDelay1) {
		DQSDelay = DQSDelay1 - DQSDelay;
	} else {
		DQSDelay += DQSDelay1;
	}

	pDCTstat->DQSDelay = (u8)DQSDelay;
}

static void TrainDQSRdWrPos_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat,
				u8 cs_start)
{
	u32 Errors;
	u8 Channel, DQSWrDelay;
	u8 _DisableDramECC = 0;
	u32 PatternBuffer[292];
	u8 _Wrap32Dis = 0, _SSE2 = 0;
	u8 dqsWrDelay_end;

	u32 addr;
	u32 cr4;
	u32 lo, hi;

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
	dqsWrDelay_end = 0x20;

	Errors = 0;
	for (Channel = 0; Channel < 2; Channel++) {
		print_debug_dqs("\tTrainDQSRdWrPos: 1 Channel ",Channel, 1);
		pDCTstat->Channel = Channel;

		if (pDCTstat->DIMMValidDCT[Channel] == 0)	/* mct_BeforeTrainDQSRdWrPos_D */
			continue;
		pDCTstat->DqsRdWrPos_Saved = 0;
		for ( DQSWrDelay = 0; DQSWrDelay < dqsWrDelay_end; DQSWrDelay++) {
			pDCTstat->DQSDelay = DQSWrDelay;
			pDCTstat->Direction = DQS_WRITEDIR;
			mct_SetDQSDelayAllCSR_D(pMCTstat, pDCTstat, cs_start);

			print_debug_dqs("\t\tTrainDQSRdWrPos: 21 DQSWrDelay ", DQSWrDelay, 2);
			TrainReadDQS_D(pMCTstat, pDCTstat, cs_start);
			print_debug_dqs("\t\tTrainDQSRdWrPos: 21 DqsRdWrPos_Saved ", pDCTstat->DqsRdWrPos_Saved, 2);
			if (pDCTstat->DqsRdWrPos_Saved == 0xFF)
				break;

			print_debug_dqs("\t\tTrainDQSRdWrPos: 22 TrainErrors ",pDCTstat->TrainErrors, 2);
			if (pDCTstat->TrainErrors == 0) {
					break;
			}
			Errors |= pDCTstat->TrainErrors;
		}

		pDCTstat->DqsRdWrPos_Saved = 0;
		if (DQSWrDelay < dqsWrDelay_end) {
			Errors = 0;

			print_debug_dqs("\tTrainDQSRdWrPos: 231 DQSWrDelay ", DQSWrDelay, 1);
			TrainWriteDQS_D(pMCTstat, pDCTstat, cs_start);
		}
		print_debug_dqs("\tTrainDQSRdWrPos: 232 Errors ", Errors, 1);
		pDCTstat->ErrStatus |= Errors;
	}

#if DQS_TRAIN_DEBUG > 0
	{
		u8 val;
		u8 i;
		u8 Channel, Receiver, Dir;
		u8 *p;

		for (Dir = 0; Dir < 2; Dir++) {
			if (Dir == 1) {
				print_debug("TrainDQSRdWrPos: CH_D_DIR_B_DQS WR:\n");
			} else {
				print_debug("TrainDQSRdWrPos: CH_D_DIR_B_DQS RD:\n");
			}
			for (Channel = 0; Channel < 2; Channel++) {
				print_debug("Channel:"); print_debug_hex8(Channel); print_debug("\n");
				for (Receiver = cs_start; Receiver < (cs_start + 2); Receiver += 2) {
					print_debug("\t\tReceiver:"); print_debug_hex8(Receiver);
					p = pDCTstat->CH_D_DIR_B_DQS[Channel][Receiver >> 1][Dir];
					print_debug(": ");
					for (i=0;i<8; i++) {
						val  = p[i];
						print_debug_hex8(val);
						print_debug(" ");
					}
					print_debug("\n");
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
	if (!_SSE2){
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
		for (i=0; i<16*18; i++)
			buf[i] = TestPatternJD1b_D[i];
	} else {
		pDCTstat->Pattern = 0;	/* 9 cache lines, sequential qwords */
		for (i=0; i<16*9; i++)
			buf[i] = TestPatternJD1a_D[i];
	}
	pDCTstat->PtrPatternBufA = (u32)buf;
}

static void TrainDQSPos_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat,
				u8 cs_start)
{
	u32 Errors;
	u8 ChipSel, DQSDelay;
	u8 RnkDlySeqPassMin=0, RnkDlySeqPassMax=0xFF, RnkDlyFilterMin=0, RnkDlyFilterMax=0xFF;
	u8 RnkDlySeqPassMinTot=0, RnkDlySeqPassMaxTot=0xFF, RnkDlyFilterMinTot=0, RnkDlyFilterMaxTot=0xFF;
	u8 LastTest ,LastTestTot;
	u32 TestAddr;
	u8 ByteLane;
	u8 MutualCSPassW[128];
	u8 BanksPresent;
	u8 dqsDelay_end;
	u8 tmp, valid, tmp1;
	u16 word;

	/* MutualCSPassW: each byte represents a bitmap of pass/fail per
	 * ByteLane.  The indext within MutualCSPassW is the delay value
	 * given the results.
	 */
	print_debug_dqs("\t\t\tTrainDQSPos begin ", 0, 3);

	Errors = 0;
	BanksPresent = 0;

	dqsDelay_end = 32;
	/* Bitmapped status per delay setting, 0xff=All positions
	 * passing (1= PASS). Set the entire array.
	 */
	for (DQSDelay=0; DQSDelay<128; DQSDelay++) {
		MutualCSPassW[DQSDelay] = 0xFF;
	}

	for (ChipSel = cs_start; ChipSel < (cs_start + 2); ChipSel++) { /* logical register chipselects 0..7 */
		print_debug_dqs("\t\t\t\tTrainDQSPos: 11 ChipSel ", ChipSel, 4);

		if (!mct_RcvrRankEnabled_D(pMCTstat, pDCTstat, pDCTstat->Channel, ChipSel)) {
			print_debug_dqs("\t\t\t\tmct_RcvrRankEnabled_D CS not enabled ", ChipSel, 4);
			continue;
		}

		BanksPresent = 1; 	/* flag for at least one bank is present */
		TestAddr = mct_GetMCTSysAddr_D(pMCTstat, pDCTstat, pDCTstat->Channel, ChipSel, &valid);
		if (!valid) {
			print_debug_dqs("\t\t\t\tAddress not supported on current CS ", TestAddr, 4);
			continue;
		}

		print_debug_dqs("\t\t\t\tTrainDQSPos: 12 TestAddr ", TestAddr, 4);
		SetUpperFSbase(TestAddr);	/* fs:eax=far ptr to target */

		if (pDCTstat->Direction == DQS_READDIR) {
			print_debug_dqs("\t\t\t\tTrainDQSPos: 13 for read ", 0, 4);
			WriteDQSTestPattern_D(pMCTstat, pDCTstat, TestAddr << 8);
		}

		for (DQSDelay = 0; DQSDelay < dqsDelay_end; DQSDelay++) {
			print_debug_dqs("\t\t\t\t\tTrainDQSPos: 141 DQSDelay ", DQSDelay, 5);

			tmp = 0xFF;
			tmp1 = DQSDelay;
			if (pDCTstat->Direction == DQS_READDIR) {
				tmp &= MutualCSPassW[DQSDelay];
				tmp1 += dqsDelay_end;
			}
			tmp &= MutualCSPassW[tmp1];

			if (tmp == 0) {
				continue;/* skip current delay value if other chipselects have failed all 8 bytelanes */
			}

			pDCTstat->DQSDelay = DQSDelay;
			mct_SetDQSDelayAllCSR_D(pMCTstat, pDCTstat, cs_start);
			print_debug_dqs("\t\t\t\t\tTrainDQSPos: 142 MutualCSPassW ", MutualCSPassW[DQSDelay], 5);

			if (pDCTstat->Direction == DQS_WRITEDIR) {
				print_debug_dqs("\t\t\t\t\tTrainDQSPos: 143 for write", 0, 5);
				WriteDQSTestPattern_D(pMCTstat, pDCTstat, TestAddr << 8);
			}

			print_debug_dqs("\t\t\t\t\tTrainDQSPos: 144 Pattern ", pDCTstat->Pattern, 5);
			ReadDQSTestPattern_D(pMCTstat, pDCTstat, TestAddr << 8);
			/* print_debug_dqs("\t\t\t\t\tTrainDQSPos: 145 MutualCSPassW ", MutualCSPassW[DQSDelay], 5); */
			word = CompareDQSTestPattern_D(pMCTstat, pDCTstat, TestAddr << 8); /* 0=fail, 1=pass */
			print_debug_dqs("\t\t\t\t\tTrainDQSPos: 144 compare 1 ", word, 3);

			print_debug_dqs("\t\t\t\t\tTrainDQSPos: 144 DqsRdWrPos_Saved ", pDCTstat->DqsRdWrPos_Saved, 3);
			word &= ~(pDCTstat->DqsRdWrPos_Saved); /* mask out bytelanes that already passed */
			word &= ~(pDCTstat->DqsRdWrPos_Saved << 8);
			print_debug_dqs("\t\t\t\t\tTrainDQSPos: 144 compare 2 ", word, 3);

			tmp = DQSDelay;
			if (pDCTstat->Direction == DQS_READDIR) {
				MutualCSPassW[tmp] &= word >> 8;
				tmp += dqsDelay_end;
			}
			MutualCSPassW[tmp] &= word & 0xFF;

			print_debug_dqs("\t\t\t\t\tTrainDQSPos: 146 \tMutualCSPassW ", MutualCSPassW[DQSDelay], 5);

			SetTargetWTIO_D(TestAddr);
			FlushDQSTestPattern_D(pDCTstat, TestAddr << 8);
			ResetTargetWTIO_D();
		}

	}

	if (pDCTstat->Direction == DQS_READDIR) {
		dqsDelay_end <<= 1;
	}

	if (BanksPresent) {
		#if 0		/* show the bitmap */
		for (ByteLane = 0; ByteLane < 8; ByteLane++) { /* just print ByteLane 0 */
			for (DQSDelay = 0; DQSDelay < dqsDelay_end; DQSDelay++) {
				if (!(MutualCSPassW[DQSDelay] &(1 << ByteLane))) {
					printk(BIOS_DEBUG, ".");
				} else {
					printk(BIOS_DEBUG, "*");
				}
			}
			printk(BIOS_DEBUG, "\n");
		}
		#endif
		for (ByteLane = 0; ByteLane < 8; ByteLane++) {
			print_debug_dqs("\t\t\t\tTrainDQSPos: 31 ByteLane ",ByteLane, 4);
			if (!(pDCTstat->DqsRdWrPos_Saved &(1 << ByteLane))) {
				pDCTstat->ByteLane = ByteLane;
				LastTest = DQS_FAIL;		/* Analyze the results */
				LastTestTot = DQS_FAIL;
				/* RnkDlySeqPassMin = 0; */
				/* RnkDlySeqPassMax = 0; */
				RnkDlyFilterMax = 0;
				RnkDlyFilterMin = 0;
				RnkDlyFilterMaxTot = 0;
				RnkDlyFilterMinTot = 0;
				for (DQSDelay = 0; DQSDelay < dqsDelay_end; DQSDelay++) {
					if (MutualCSPassW[DQSDelay] & (1 << ByteLane)) {
						print_debug_dqs("\t\t\t\t\tTrainDQSPos: 321 DQSDelay ", DQSDelay, 5);
						print_debug_dqs("\t\t\t\t\tTrainDQSPos: 322 MutualCSPassW ", MutualCSPassW[DQSDelay], 5);
						if (pDCTstat->Direction == DQS_READDIR)
							tmp = 0x20;
						else
							tmp = 0;
						if (DQSDelay >= tmp) {
							RnkDlySeqPassMax = DQSDelay;
							if (LastTest == DQS_FAIL) {
								RnkDlySeqPassMin = DQSDelay; /* start sequential run */
							}
							if ((RnkDlySeqPassMax - RnkDlySeqPassMin)>(RnkDlyFilterMax-RnkDlyFilterMin)){
								RnkDlyFilterMin = RnkDlySeqPassMin;
								RnkDlyFilterMax = RnkDlySeqPassMax;
							}
							LastTest = DQS_PASS;
						}

						if (pDCTstat->Direction == DQS_READDIR) {
							RnkDlySeqPassMaxTot = DQSDelay;
							if (LastTestTot == DQS_FAIL)
								RnkDlySeqPassMinTot = DQSDelay;
							if ((RnkDlySeqPassMaxTot - RnkDlySeqPassMinTot)>(RnkDlyFilterMaxTot-RnkDlyFilterMinTot)){
								RnkDlyFilterMinTot = RnkDlySeqPassMinTot;
								RnkDlyFilterMaxTot = RnkDlySeqPassMaxTot;
							}
							LastTestTot = DQS_PASS;
						}
					} else {
						LastTest = DQS_FAIL;
						LastTestTot = DQS_FAIL;
					}
				}
				print_debug_dqs("\t\t\t\tTrainDQSPos: 33 RnkDlySeqPassMax ", RnkDlySeqPassMax, 4);
				if (RnkDlySeqPassMax == 0) {
					Errors |= 1 << SB_NODQSPOS; /* no passing window */
				} else {
					print_debug_dqs_pair("\t\t\t\tTrainDQSPos: 34 RnkDlyFilter: ", RnkDlyFilterMin, " ",  RnkDlyFilterMax, 4);
					if (((RnkDlyFilterMax - RnkDlyFilterMin) < MIN_DQS_WNDW)){
						Errors |= 1 << SB_SMALLDQS;
					} else {
						u8 middle_dqs;
						/* mctEngDQSwindow_Save_D Not required for arrays */
						if (pDCTstat->Direction == DQS_READDIR)
							middle_dqs = MiddleDQS_D(RnkDlyFilterMinTot, RnkDlyFilterMaxTot);
						else
							middle_dqs = MiddleDQS_D(RnkDlyFilterMin, RnkDlyFilterMax);
						pDCTstat->DQSDelay = middle_dqs;
						mct_SetDQSDelayCSR_D(pMCTstat, pDCTstat, cs_start);  /* load the register with the value */
						if (pDCTstat->Direction == DQS_READDIR)
							StoreWrRdDQSDatStrucVal_D(pMCTstat, pDCTstat, cs_start, RnkDlyFilterMinTot, RnkDlyFilterMaxTot); /* store the value into the data structure */
						else
							StoreWrRdDQSDatStrucVal_D(pMCTstat, pDCTstat, cs_start, RnkDlyFilterMin, RnkDlyFilterMax); /* store the value into the data structure */
						print_debug_dqs("\t\t\t\tTrainDQSPos: 42 middle_dqs : ",middle_dqs, 4);
						pDCTstat->DqsRdWrPos_Saved |= 1 << ByteLane;
					}
				}
			}
		} /* if (pDCTstat->DqsRdWrPos_Saved &(1 << ByteLane)) */
	}
/* skipLocMiddle: */
	pDCTstat->TrainErrors = Errors;

	print_debug_dqs("\t\t\tTrainDQSPos: Errors ", Errors, 3);
}

static void mctEngDQSwindow_Save_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 ChipSel,
					u8 RnkDlyFilterMin, u8 RnkDlyFilterMax)
{
	pDCTstat->CH_D_DIR_MaxMin_B_Dly[pDCTstat->Channel]
		[pDCTstat->Direction]
		[0]
		[pDCTstat->ByteLane] = RnkDlyFilterMin;
	pDCTstat->CH_D_DIR_MaxMin_B_Dly[pDCTstat->Channel]
		[pDCTstat->Direction]
		[1]
		[pDCTstat->ByteLane] = RnkDlyFilterMax;
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
	 * Rev A/B only support DIMM0/1 when 800Mhz and above + 0x100 to next dimm
	 * Rev C support DIMM0/1/2/3 when 800Mhz and above  + 0x100 to next dimm
	 */

	/* FindDQSDatDimmVal_D is not required since we use an array */
	u8 dn = 0;

	dn = ChipSel>>1; /* if odd or even logical DIMM */

	pDCTstat->CH_D_DIR_B_DQS[pDCTstat->Channel][dn][pDCTstat->Direction][pDCTstat->ByteLane] =
					pDCTstat->DQSDelay;
}

static void StoreWrRdDQSDatStrucVal_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 ChipSel,
					u8 RnkDlyFilterMin, u8 RnkDlyFilterMax)
{
	u8 dn;

	if (pDCTstat->Direction == DQS_WRITEDIR) {
		dn = ChipSel >> 1;
		RnkDlyFilterMin += pDCTstat->CH_D_B_TxDqs[pDCTstat->Channel][dn][pDCTstat->ByteLane];
		RnkDlyFilterMax += pDCTstat->CH_D_B_TxDqs[pDCTstat->Channel][dn][pDCTstat->ByteLane];
		pDCTstat->DQSDelay += pDCTstat->CH_D_B_TxDqs[pDCTstat->Channel][dn][pDCTstat->ByteLane];
	} else {
		RnkDlyFilterMin <<= 1;
		RnkDlyFilterMax <<= 1;
		pDCTstat->DQSDelay <<= 1;
	}
	mctEngDQSwindow_Save_D(pMCTstat, pDCTstat, ChipSel, RnkDlyFilterMin, RnkDlyFilterMax);
	StoreDQSDatStrucVal_D(pMCTstat, pDCTstat, ChipSel);
}

static void GetDQSDatStrucVal_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 ChipSel)
{
	u8 dn = 0;

	/* When 400, 533, 667, it will support dimm0/1/2/3,
	 * and set conf for dimm0, hw will copy to dimm1/2/3
	 * set for dimm1, hw will copy to dimm3
	 * Rev A/B only support DIMM0/1 when 800Mhz and above + 0x100 to next dimm
	 * Rev C support DIMM0/1/2/3 when 800Mhz and above  + 0x100 to next dimm
	 */

	/* FindDQSDatDimmVal_D is not required since we use an array */
	dn = ChipSel >> 1; /*if odd or even logical DIMM */

	pDCTstat->DQSDelay =
		pDCTstat->CH_D_DIR_B_DQS[pDCTstat->Channel][dn][pDCTstat->Direction][pDCTstat->ByteLane];
}

/* FindDQSDatDimmVal_D is not required since we use an array */

static u8 MiddleDQS_D(u8 min, u8 max)
{
	u8 size;
	size = max-min;
	if (size % 2)
		size++;		/* round up if the size isn't even. */
	return ( min + (size >> 1));
}

static void TrainReadDQS_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat,
				u8 cs_start)
{
	print_debug_dqs("\t\tTrainReadPos ", 0, 2);
	pDCTstat->Direction = DQS_READDIR;
	TrainDQSPos_D(pMCTstat, pDCTstat, cs_start);
}

static void TrainWriteDQS_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat,
				u8 cs_start)
{
	pDCTstat->Direction = DQS_WRITEDIR;
	print_debug_dqs("\t\tTrainWritePos", 0, 2);
	TrainDQSPos_D(pMCTstat, pDCTstat, cs_start);
}

static void proc_IOCLFLUSH_D(u32 addr_hi)
{
	SetTargetWTIO_D(addr_hi);
	proc_CLFLUSH(addr_hi);
	ResetTargetWTIO_D();
}

static u8 ChipSelPresent_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat,
				u8 Channel, u8 ChipSel)
{
	u32 val;
	u32 reg;
	u32 dev = pDCTstat->dev_dct;
	u32 reg_off;
	u8 ret = 0;

	if (!pDCTstat->GangedMode) {
		reg_off = 0x100 * Channel;
	} else {
		reg_off = 0;
	}

	if (ChipSel < MAX_CS_SUPPORTED){
		reg = 0x40 + (ChipSel << 2) + reg_off;
		val = Get_NB32(dev, reg);
		if (val & ( 1 << 0))
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
	bitmap = 0xFFFF;	/* bytelane test bitmap, 1=pass */
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
					if  (((value_r >> j) & 0xff) != ((value_r_test >> j) & 0xff)) {
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

		if (bytelane == 0){
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
	if (pDCTstat->Pattern == 0){
		FlushDQSTestPattern_L9(addr_lo);
	} else {
		FlushDQSTestPattern_L18(addr_lo);
	}
}

static void SetTargetWTIO_D(u32 TestAddr)
{
	u32 lo, hi;
	hi = TestAddr >> 24;
	lo = TestAddr << 8;
	_WRMSR(0xC0010016, lo, hi);		/* IORR0 Base */
	hi = 0xFF;
	lo = 0xFC000800;			/* 64MB Mask */
	_WRMSR(0xC0010017, lo, hi);		/* IORR0 Mask */
}

static void ResetTargetWTIO_D(void)
{
	u32 lo, hi;

	hi = 0;
	lo = 0;
	_WRMSR(0xc0010017, lo, hi); /* IORR0 Mask */
}

static void ReadDQSTestPattern_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat,
				u32 TestAddr_lo)
{
	/* Read a pattern of 72 bit times (per DQ), to test dram functionality.
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
		ReadL9TestPattern(TestAddr_lo);
	else
		ReadL18TestPattern(TestAddr_lo);
	_MFENCE;
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

void ResetDCTWrPtr_D(u32 dev, u32 index_reg, u32 index)
{
	u32 val;

	val = Get_NB32_index_wait(dev, index_reg, index);
	Set_NB32_index_wait(dev, index_reg, index, val);
}

/* mctEngDQSwindow_Save_D not required with arrays */

void mct_TrainDQSPos_D(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstatA)
{
	u8 Node;
	u8 ChipSel;
	struct DCTStatStruc *pDCTstat;

	for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
		pDCTstat = pDCTstatA + Node;
		if (pDCTstat->DCTSysLimit) {
			for (ChipSel = 0; ChipSel < MAX_CS_SUPPORTED; ChipSel += 2) {
				TrainDQSRdWrPos_D(pMCTstat, pDCTstat, ChipSel);
				SetEccDQSRdWrPos_D(pMCTstat, pDCTstat, ChipSel);
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
	val = Get_NB32(dev, reg);
	if (val & (1<<DimmEcEn)) {
		_DisableDramECC |= 0x01;
		val &= ~(1<<DimmEcEn);
		Set_NB32(dev, reg, val);
	}
	if (!pDCTstat->GangedMode) {
		reg = 0x190;
		val = Get_NB32(dev, reg);
		if (val & (1<<DimmEcEn)) {
			_DisableDramECC |= 0x02;
			val &= ~(1<<DimmEcEn);
			Set_NB32(dev, reg, val);
		}
	}
	return _DisableDramECC;
}

void mct_EnableDimmEccEn_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 _DisableDramECC)
{
	u32 val;
	u32 reg;
	u32 dev;

	/* Enable ECC correction if it was previously disabled */

	dev = pDCTstat->dev_dct;

	if ((_DisableDramECC & 0x01) == 0x01) {
		reg = 0x90;
		val = Get_NB32(dev, reg);
		val |= (1<<DimmEcEn);
		Set_NB32(dev, reg, val);
	}
	if ((_DisableDramECC & 0x02) == 0x02) {
		reg = 0x190;
		val = Get_NB32(dev, reg);
		val |= (1<<DimmEcEn);
		Set_NB32(dev, reg, val);
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
	u32 index_reg = 0x98 + 0x100 * pDCTstat->Channel;
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

		val = Get_NB32_index_wait(dev, index_reg, index);
		if (ByteLane < 8) {
			if (pDCTstat->Direction == DQS_WRITEDIR) {
				dqs_delay += pDCTstat->CH_D_B_TxDqs[pDCTstat->Channel][ChipSel>>1][ByteLane];
			} else {
				dqs_delay <<= 1;
			}
		}
		val &= ~(0x7f << shift);
		val |= (dqs_delay << shift);
		Set_NB32_index_wait(dev, index_reg, index, val);
	}
}

/*
 * mct_SetDQSDelayAllCSR_D:
 * Write the Delay value to all eight byte lanes.
 */
static void mct_SetDQSDelayAllCSR_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat,
					u8 cs_start)
{
	u8 ByteLane;
	u8 ChipSel = cs_start;

	for (ChipSel = cs_start; ChipSel < (cs_start + 2); ChipSel++) {
		if ( mct_RcvrRankEnabled_D(pMCTstat, pDCTstat, pDCTstat->Channel, ChipSel)) {
			for (ByteLane = 0; ByteLane < 8; ByteLane++) {
				pDCTstat->ByteLane = ByteLane;
				mct_SetDQSDelayCSR_D(pMCTstat, pDCTstat, ChipSel);
			}
		}
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
	u32 reg_off = 0;
	u32 reg;
	u32 dword;
	u32 dev = pDCTstat->dev_dct;

	*valid = 0;


	if (!pDCTstat->GangedMode) {
		reg_off = 0x100 * Channel;
	}

	/* get the local base addr of the chipselect */
	reg = 0x40 + (receiver << 2) + reg_off;
	val = Get_NB32(dev, reg);

	val &= ~0x0F;

	/* unganged mode DCT0+DCT1, sys addr of DCT1=node
	 * base+DctSelBaseAddr+local ca base*/
	if ((Channel) && (pDCTstat->GangedMode == 0) && ( pDCTstat->DIMMValidDCT[0] > 0)) {
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
		/* sys addr=node base+local cs base */
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
		while(val < MCT_TRNG_KEEPOUT_END)
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
	print_debug_dqs("mct_GetMCTSysAddr_D: HoleBase ", pDCTstat->DCTHoleBase, 2);
	print_debug_dqs("mct_GetMCTSysAddr_D: Cachetop ", pMCTstat->Sub4GCacheTop, 2);

exitGetAddr:
	return val;
}

static void mct_Write1LTestPattern_D(struct MCTStatStruc *pMCTstat,
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
