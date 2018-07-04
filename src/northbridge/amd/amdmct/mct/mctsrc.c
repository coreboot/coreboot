/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 Advanced Micro Devices, Inc.
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

#include "mct_d.h"
#include <cpu/x86/cr.h>

/******************************************************************************
 Description: Receiver En and DQS Timing Training feature for DDR 2 MCT
******************************************************************************/

static void dqsTrainRcvrEn_SW(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 Pass);
static u8 mct_SavePassRcvEnDly_D(struct DCTStatStruc *pDCTstat,
					u8 rcvrEnDly, u8 Channel,
					u8 receiver, u8 Pass);
static u8 mct_CompareTestPatternQW0_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat,
					u32 addr, u8 channel,
					u8 pattern, u8 Pass);
static void mct_InitDQSPos4RcvrEn_D(struct MCTStatStruc *pMCTstat,
					 struct DCTStatStruc *pDCTstat);
static void InitDQSPos4RcvrEn_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 Channel);
static void CalcEccDQSRcvrEn_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 Channel);
static void mct_SetFinalRcvrEnDly_D(struct DCTStatStruc *pDCTstat,
				u8 RcvrEnDly, u8 where,
				u8 Channel, u8 Receiver,
				u32 dev, u32 index_reg,
				u8 Addl_Index, u8 Pass);
static void mct_SetMaxLatency_D(struct DCTStatStruc *pDCTstat, u8 Channel, u8 DQSRcvEnDly);
static void fenceDynTraining_D(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, u8 dct);
static void mct_DisableDQSRcvEn_D(struct DCTStatStruc *pDCTstat);

/* Warning:  These must be located so they do not cross a logical 16-bit
   segment boundary! */
const u32 TestPattern0_D[] = {
	0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa,
	0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa,
	0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa,
	0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa,
};
const u32 TestPattern1_D[] = {
	0x55555555, 0x55555555, 0x55555555, 0x55555555,
	0x55555555, 0x55555555, 0x55555555, 0x55555555,
	0x55555555, 0x55555555, 0x55555555, 0x55555555,
	0x55555555, 0x55555555, 0x55555555, 0x55555555,
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
	buf_b = buf_a + 32; //??
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
	if (mct_checkNumberOfDqsRcvEn_1Pass(Pass))
		dqsTrainRcvrEn_SW(pMCTstat, pDCTstat, Pass);
}


static void dqsTrainRcvrEn_SW(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 Pass)
{
	u8 Channel, RcvrEnDly, RcvrEnDlyRmin;
	u8 Test0, Test1, CurrTest, CurrTestSide0, CurrTestSide1;
	u8 CTLRMaxDelay, _2Ranks, PatternA, PatternB;
	u8 Addl_Index = 0;
	u8 Receiver;
	u8 _DisableDramECC = 0, _Wrap32Dis = 0, _SSE2 = 0;
	u8 RcvrEnDlyLimit, Final_Value, MaxDelay_CH[2];
	u32 TestAddr0, TestAddr1, TestAddr0B, TestAddr1B;
	u32 PatternBuffer[64+4]; /* FIXME: need increase 8? */
	u32 Errors;

	u32 val;
	u32 reg;
	u32 dev;
	u32 index_reg;
	u32 ch_start, ch_end, ch;
	u32 msr;
	u32 cr4;
	u32 lo, hi;

	u8 valid;
	u32 tmp;
	u8 LastTest;

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
		reg = 0x78 + (0x100 * ch);
		val = Get_NB32(dev, reg);
		val &= ~(0x3ff << 22);
		val |= (0x0c8 << 22);		/* Max Rd Lat */
		Set_NB32(dev, reg, val);
	}

	Final_Value = 1;
	if (Pass == FirstPass) {
		mct_InitDQSPos4RcvrEn_D(pMCTstat, pDCTstat);
	} else {
		pDCTstat->DimmTrainFail = 0;
		pDCTstat->CSTrainFail = ~pDCTstat->CSPresent;
	}
	print_t("TrainRcvrEn: 1\n");

	cr4 = read_cr4();
	if (cr4 & (1 << 9)) {	/* save the old value */
		_SSE2 = 1;
	}
	cr4 |= (1 << 9);	/* OSFXSR enable SSE2 */
	write_cr4(cr4);
	print_t("TrainRcvrEn: 2\n");

	msr = HWCR;
	_RDMSR(msr, &lo, &hi);
	//FIXME: Why use SSEDIS
	if (lo & (1 << 17)) {	/* save the old value */
		_Wrap32Dis = 1;
	}
	lo |= (1 << 17);	/* HWCR.wrap32dis */
	lo &= ~(1 << 15);	/* SSEDIS */
	_WRMSR(msr, lo, hi);	/* Setting wrap32dis allows 64-bit memory references in real mode */
	print_t("TrainRcvrEn: 3\n");

	_DisableDramECC = mct_DisableDimmEccEn_D(pMCTstat, pDCTstat);


	if (pDCTstat->Speed == 1) {
		pDCTstat->T1000 = 5000;	/* get the T1000 figure (cycle time (ns)*1K */
	} else if (pDCTstat->Speed == 2) {
		pDCTstat->T1000 = 3759;
	} else if (pDCTstat->Speed == 3) {
		pDCTstat->T1000 = 3003;
	} else if (pDCTstat->Speed == 4) {
		pDCTstat->T1000 = 2500;
	} else if (pDCTstat->Speed  == 5) {
		pDCTstat->T1000 = 1876;
	} else {
		pDCTstat->T1000 = 0;
	}

	SetupRcvrPattern(pMCTstat, pDCTstat, PatternBuffer, Pass);
	print_t("TrainRcvrEn: 4\n");

	Errors = 0;
	dev = pDCTstat->dev_dct;
	CTLRMaxDelay = 0;

	for (Channel = 0; Channel < 2; Channel++) {
		print_debug_dqs("\tTrainRcvEn51: Node ", pDCTstat->Node_ID, 1);
		print_debug_dqs("\tTrainRcvEn51: Channel ", Channel, 1);
		pDCTstat->Channel = Channel;

		MaxDelay_CH[Channel] = 0;
		index_reg = 0x98 + 0x100 * Channel;

		Receiver = mct_InitReceiver_D(pDCTstat, Channel);
		/* There are four receiver pairs, loosely associated with chipselects. */
		for (; Receiver < 8; Receiver += 2) {
			Addl_Index = (Receiver >> 1) * 3 + 0x10;
			LastTest = DQS_FAIL;

			/* mct_ModifyIndex_D */
			RcvrEnDlyRmin = RcvrEnDlyLimit = 0xff;

			print_debug_dqs("\t\tTrainRcvEnd52: index ", Addl_Index, 2);

			if (!mct_RcvrRankEnabled_D(pMCTstat, pDCTstat, Channel, Receiver)) {
				print_t("\t\t\tRank not enabled_D\n");
				continue;
			}

			TestAddr0 = mct_GetRcvrSysAddr_D(pMCTstat, pDCTstat, Channel, Receiver, &valid);
			if (!valid) {	/* Address not supported on current CS */
				print_t("\t\t\tAddress not supported on current CS\n");
				continue;
			}

			TestAddr0B = TestAddr0 + (BigPagex8_RJ8 << 3);

			if (mct_RcvrRankEnabled_D(pMCTstat, pDCTstat, Channel, Receiver+1)) {
				TestAddr1 = mct_GetRcvrSysAddr_D(pMCTstat, pDCTstat, Channel, Receiver+1, &valid);
				if (!valid) {	/* Address not supported on current CS */
					print_t("\t\t\tAddress not supported on current CS+1\n");
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

			/*
			 * Get starting RcvrEnDly value
			 */
			RcvrEnDly = mct_Get_Start_RcvrEnDly_1Pass(Pass);

			/* mct_GetInitFlag_D*/
			if (Pass == FirstPass) {
				pDCTstat->DqsRcvEn_Pass = 0;
			} else {
				pDCTstat->DqsRcvEn_Pass = 0xFF;
			}
			pDCTstat->DqsRcvEn_Saved = 0;


			while (RcvrEnDly < RcvrEnDlyLimit) {	/* sweep Delay value here */
				print_debug_dqs("\t\t\tTrainRcvEn541: RcvrEnDly ", RcvrEnDly, 3);

				/* callback not required
				if (mct_AdjustDelay_D(pDCTstat, RcvrEnDly))
					goto skipDly;
				*/

				/* Odd steps get another pattern such that even
				 and odd steps alternate. The pointers to the
				 patterns will be swaped at the end of the loop
				 so that they correspond. */
				if (RcvrEnDly & 1) {
					PatternA = 1;
					PatternB = 0;
				} else {
					/* Even step */
					PatternA = 0;
					PatternB = 1;
				}

				mct_Write1LTestPattern_D(pMCTstat, pDCTstat, TestAddr0, PatternA); /* rank 0 of DIMM, testpattern 0 */
				mct_Write1LTestPattern_D(pMCTstat, pDCTstat, TestAddr0B, PatternB); /* rank 0 of DIMM, testpattern 1 */
				if (_2Ranks) {
					mct_Write1LTestPattern_D(pMCTstat, pDCTstat, TestAddr1, PatternA); /*rank 1 of DIMM, testpattern 0 */
					mct_Write1LTestPattern_D(pMCTstat, pDCTstat, TestAddr1B, PatternB); /*rank 1 of DIMM, testpattern 1 */
				}

				mct_SetRcvrEnDly_D(pDCTstat, RcvrEnDly, 0, Channel, Receiver, dev, index_reg, Addl_Index, Pass);

				CurrTest = DQS_FAIL;
				CurrTestSide0 = DQS_FAIL;
				CurrTestSide1 = DQS_FAIL;

				mct_Read1LTestPattern_D(pMCTstat, pDCTstat, TestAddr0);	/*cache fills */
				Test0 = mct_CompareTestPatternQW0_D(pMCTstat, pDCTstat, TestAddr0, Channel, PatternA, Pass);/* ROM vs cache compare */
				proc_IOCLFLUSH_D(TestAddr0);
				ResetDCTWrPtr_D(dev, index_reg, Addl_Index);

				print_debug_dqs("\t\t\tTrainRcvEn542: Test0 result ", Test0, 3);

				// != 0x00 mean pass

				if (Test0 == DQS_PASS) {
					mct_Read1LTestPattern_D(pMCTstat, pDCTstat, TestAddr0B);	/*cache fills */
					/* ROM vs cache compare */
					Test1 = mct_CompareTestPatternQW0_D(pMCTstat, pDCTstat, TestAddr0B, Channel, PatternB, Pass);
					proc_IOCLFLUSH_D(TestAddr0B);
					ResetDCTWrPtr_D(dev, index_reg, Addl_Index);

					print_debug_dqs("\t\t\tTrainRcvEn543: Test1 result ", Test1, 3);

					if (Test1 == DQS_PASS) {
						CurrTestSide0 = DQS_PASS;
					}
				}
				if (_2Ranks) {
					mct_Read1LTestPattern_D(pMCTstat, pDCTstat, TestAddr1);	/*cache fills */
					/* ROM vs cache compare */
					Test0 = mct_CompareTestPatternQW0_D(pMCTstat, pDCTstat, TestAddr1, Channel, PatternA, Pass);
					proc_IOCLFLUSH_D(TestAddr1);
					ResetDCTWrPtr_D(dev, index_reg, Addl_Index);

					print_debug_dqs("\t\t\tTrainRcvEn544: Test0 result ", Test0, 3);

					if (Test0 == DQS_PASS) {
						mct_Read1LTestPattern_D(pMCTstat, pDCTstat, TestAddr1B);	/*cache fills */
						/* ROM vs cache compare */
						Test1 = mct_CompareTestPatternQW0_D(pMCTstat, pDCTstat, TestAddr1B, Channel, PatternB, Pass);
						proc_IOCLFLUSH_D(TestAddr1B);
						ResetDCTWrPtr_D(dev, index_reg, Addl_Index);

						print_debug_dqs("\t\t\tTrainRcvEn545: Test1 result ", Test1, 3);
						if (Test1 == DQS_PASS) {
							CurrTestSide1 = DQS_PASS;
						}
					}
				}

				if (_2Ranks) {
					if ((CurrTestSide0 == DQS_PASS) && (CurrTestSide1 == DQS_PASS)) {
						CurrTest = DQS_PASS;
					}
				} else if (CurrTestSide0 == DQS_PASS) {
					CurrTest = DQS_PASS;
				}


				/* record first pass DqsRcvEn to stack */
				valid = mct_SavePassRcvEnDly_D(pDCTstat, RcvrEnDly, Channel, Receiver, Pass);

				/* Break(1:RevF,2:DR) or not(0) FIXME: This comment deosn't make sense */
				if (valid == 2 || (LastTest == DQS_FAIL && valid == 1)) {
					RcvrEnDlyRmin = RcvrEnDly;
					break;
				}

				LastTest = CurrTest;

				/* swap the rank 0 pointers */
				tmp = TestAddr0;
				TestAddr0 = TestAddr0B;
				TestAddr0B = tmp;

				/* swap the rank 1 pointers */
				tmp = TestAddr1;
				TestAddr1 = TestAddr1B;
				TestAddr1B = tmp;

				print_debug_dqs("\t\t\tTrainRcvEn56: RcvrEnDly ", RcvrEnDly, 3);

				RcvrEnDly++;

			}	/* while RcvrEnDly */

			print_debug_dqs("\t\tTrainRcvEn61: RcvrEnDly ", RcvrEnDly, 2);
			print_debug_dqs("\t\tTrainRcvEn61: RcvrEnDlyRmin ", RcvrEnDlyRmin, 3);
			print_debug_dqs("\t\tTrainRcvEn61: RcvrEnDlyLimit ", RcvrEnDlyLimit, 3);
			if (RcvrEnDlyRmin == RcvrEnDlyLimit) {
				/* no passing window */
				pDCTstat->ErrStatus |= 1 << SB_NORCVREN;
				Errors |= 1 << SB_NORCVREN;
				pDCTstat->ErrCode = SC_FatalErr;
			}

			if (RcvrEnDly > (RcvrEnDlyLimit - 1)) {
				/* passing window too narrow, too far delayed*/
				pDCTstat->ErrStatus |= 1 << SB_SmallRCVR;
				Errors |= 1 << SB_SmallRCVR;
				pDCTstat->ErrCode = SC_FatalErr;
				RcvrEnDly = RcvrEnDlyLimit - 1;
				pDCTstat->CSTrainFail |= 1 << Receiver;
				pDCTstat->DimmTrainFail |= 1 << (Receiver + Channel);
			}

			// CHB_D0_B0_RCVRDLY set in mct_Average_RcvrEnDly_Pass
			mct_Average_RcvrEnDly_Pass(pDCTstat, RcvrEnDly, RcvrEnDlyLimit, Channel, Receiver, Pass);

			mct_SetFinalRcvrEnDly_D(pDCTstat, RcvrEnDly, Final_Value, Channel, Receiver, dev, index_reg, Addl_Index, Pass);

			if (pDCTstat->ErrStatus & (1 << SB_SmallRCVR)) {
				Errors |= 1 << SB_SmallRCVR;
			}

			RcvrEnDly += Pass1MemClkDly;
			if (RcvrEnDly > CTLRMaxDelay) {
				CTLRMaxDelay = RcvrEnDly;
			}

		}	/* while Receiver */

		MaxDelay_CH[Channel] = CTLRMaxDelay;
	}	/* for Channel */

	CTLRMaxDelay = MaxDelay_CH[0];
	if (MaxDelay_CH[1] > CTLRMaxDelay)
		CTLRMaxDelay = MaxDelay_CH[1];

	for (Channel = 0; Channel < 2; Channel++) {
		mct_SetMaxLatency_D(pDCTstat, Channel, CTLRMaxDelay); /* program Ch A/B MaxAsyncLat to correspond with max delay */
	}

	ResetDCTWrPtr_D(dev, index_reg, Addl_Index);

	if (_DisableDramECC) {
		mct_EnableDimmEccEn_D(pMCTstat, pDCTstat, _DisableDramECC);
	}

	if (Pass == FirstPass) {
		/*Disable DQSRcvrEn training mode */
		print_t("TrainRcvrEn: mct_DisableDQSRcvEn_D\n");
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
		u8 Channel;
		printk(BIOS_DEBUG, "TrainRcvrEn: CH_MaxRdLat:\n");
		for (Channel = 0; Channel < 2; Channel++) {
			printk(BIOS_DEBUG, "Channel: %02x: %02x\n", Channel, pDCTstat->CH_MaxRdLat[Channel]);
		}
	}
#endif

#if DQS_TRAIN_DEBUG > 0
	{
		u8 val;
		u8 Channel, Receiver;
		u8 i;
		u8 *p;

		printk(BIOS_DEBUG, "TrainRcvrEn: CH_D_B_RCVRDLY:\n");
		for (Channel = 0; Channel < 2; Channel++) {
			printk(BIOS_DEBUG, "Channel: %02x\n", Channel);
			for (Receiver = 0; Receiver < 8; Receiver+=2) {
				printk(BIOS_DEBUG, "\t\tReceiver: %02x: ", Receiver);
				p = pDCTstat->persistentData.CH_D_B_RCVRDLY[Channel][Receiver>>1];
				for (i = 0; i < 8; i++) {
					val  = p[i];
					printk(BIOS_DEBUG, "%02x ", val);
				}
			printk(BIOS_DEBUG, "\n");
			}
		}
	}
#endif

	print_tx("TrainRcvrEn: Status ", pDCTstat->Status);
	print_tx("TrainRcvrEn: ErrStatus ", pDCTstat->ErrStatus);
	print_tx("TrainRcvrEn: ErrCode ", pDCTstat->ErrCode);
	print_t("TrainRcvrEn: Done\n");
}


u8 mct_InitReceiver_D(struct DCTStatStruc *pDCTstat, u8 dct)
{
	if (pDCTstat->DIMMValidDCT[dct] == 0) {
		return 8;
	} else {
		return 0;
	}
}


static void mct_SetFinalRcvrEnDly_D(struct DCTStatStruc *pDCTstat, u8 RcvrEnDly, u8 where, u8 Channel, u8 Receiver, u32 dev, u32 index_reg, u8 Addl_Index, u8 Pass/*, u8 *p*/)
{
	/*
	 * Program final DqsRcvEnDly to additional index for DQS receiver
	 *  enabled delay
	 */
	mct_SetRcvrEnDly_D(pDCTstat, RcvrEnDly, where, Channel, Receiver, dev, index_reg, Addl_Index, Pass);
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
		reg = 0x78 + 0x100 * ch;
		val = Get_NB32(dev, reg);
		val &= ~(1 << DqsRcvEnTrain);
		Set_NB32(dev, reg, val);
	}
}


/* mct_ModifyIndex_D
 * Function only used once so it was inlined.
 */


/* mct_GetInitFlag_D
 * Function only used once so it was inlined.
 */


void mct_SetRcvrEnDly_D(struct DCTStatStruc *pDCTstat, u8 RcvrEnDly,
			u8 FinalValue, u8 Channel, u8 Receiver, u32 dev,
			u32 index_reg, u8 Addl_Index, u8 Pass)
{
	u32 index;
	u8 i;
	u8 *p;
	u32 val;

	if (RcvrEnDly == 0xFE) {
		/*set the boudary flag */
		pDCTstat->Status |= 1 << SB_DQSRcvLimit;
	}

	/* DimmOffset not needed for CH_D_B_RCVRDLY array */


	for (i = 0; i < 8; i++) {
		if (FinalValue) {
			/*calculate dimm offset */
			p = pDCTstat->persistentData.CH_D_B_RCVRDLY[Channel][Receiver >> 1];
			RcvrEnDly = p[i];
		}

		/* if flag = 0, set DqsRcvEn value to reg. */
		/* get the register index from table */
		index = Table_DQSRcvEn_Offset[i >> 1];
		index += Addl_Index;	/* DIMMx DqsRcvEn byte0 */
		val = Get_NB32_index_wait(dev, index_reg, index);
		if (i & 1) {
			/* odd byte lane */
			val &= ~(0xFF << 16);
			val |= (RcvrEnDly << 16);
		} else {
			/* even byte lane */
			val &= ~0xFF;
			val |= RcvrEnDly;
		}
		Set_NB32_index_wait(dev, index_reg, index, val);
	}

}

static void mct_SetMaxLatency_D(struct DCTStatStruc *pDCTstat, u8 Channel, u8 DQSRcvEnDly)
{
	u32 dev;
	u32 reg;
	u16 SubTotal;
	u32 index_reg;
	u32 reg_off;
	u32 val;
	u32 valx;

	if (pDCTstat->GangedMode)
		Channel = 0;

	dev = pDCTstat->dev_dct;
	reg_off = 0x100 * Channel;
	index_reg = 0x98 + reg_off;

	/* Multiply the CAS Latency by two to get a number of 1/2 MEMCLKs units.*/
	val = Get_NB32(dev, 0x88 + reg_off);
	SubTotal = ((val & 0x0f) + 1) << 1;	/* SubTotal is 1/2 Memclk unit */

	/* If registered DIMMs are being used then
	 *  add 1 MEMCLK to the sub-total.
	 */
	val = Get_NB32(dev, 0x90 + reg_off);
	if (!(val & (1 << UnBuffDimm)))
		SubTotal += 2;

	/* If the address prelaunch is setup for 1/2 MEMCLKs then
	 *  add 1, else add 2 to the sub-total.
	 *  if (AddrCmdSetup || CsOdtSetup || CkeSetup) then K := K + 2;
	 */
	val = Get_NB32_index_wait(dev, index_reg, 0x04);
	if (!(val & 0x00202020))
		SubTotal += 1;
	else
		SubTotal += 2;

	/* If the F2x[1, 0]78[RdPtrInit] field is 4, 5, 6 or 7 MEMCLKs,
	 * then add 4, 3, 2, or 1 MEMCLKs, respectively to the sub-total. */
	val = Get_NB32(dev, 0x78 + reg_off);
	SubTotal += 8 - (val & 0x0f);

	/* Convert bits 7-5 (also referred to as the course delay) of
	 * the current (or worst case) DQS receiver enable delay to
	 * 1/2 MEMCLKs units, rounding up, and add this to the sub-total.
	 */
	SubTotal += DQSRcvEnDly >> 5;	/*BOZO-no rounding up */

	/* Add 5.5 to the sub-total. 5.5 represents part of the
	 * processor specific constant delay value in the DRAM
	 * clock domain.
	 */
	SubTotal <<= 1;		/*scale 1/2 MemClk to 1/4 MemClk */
	SubTotal += 11;		/*add 5.5 1/2MemClk */

	/* Convert the sub-total (in 1/2 MEMCLKs) to northbridge
	 * clocks (NCLKs) as follows (assuming DDR400 and assuming
	 * that no P-state or link speed changes have occurred).
	 */

	/* New formula:
	 * SubTotal *= 3*(Fn2xD4[NBFid]+4)/(3+Fn2x94[MemClkFreq])/2 */
	val = Get_NB32(dev, 0x94 + reg_off);

	/* SubTotal div 4 to scale 1/4 MemClk back to MemClk */
	val &= 7;
	if (val == 4) {
		val++;		/* adjust for DDR2-1066 */
	}
	valx = (val + 3) << 2;

	val = Get_NB32(pDCTstat->dev_nbmisc, 0xD4);
	SubTotal *= ((val & 0x1f) + 4) * 3;

	SubTotal /= valx;
	if (SubTotal % valx) {	/* round up */
		SubTotal++;
	}

	/* Add 5 NCLKs to the sub-total. 5 represents part of the
	 * processor specific constant value in the northbridge
	 * clock domain.
	 */
	SubTotal += 5;

	pDCTstat->CH_MaxRdLat[Channel] = SubTotal;
	if (pDCTstat->GangedMode) {
		pDCTstat->CH_MaxRdLat[1] = SubTotal;
	}

	/* Program the F2x[1, 0]78[MaxRdLatency] register with
	 * the total delay value (in NCLKs).
	 */

	reg = 0x78 + reg_off;
	val = Get_NB32(dev, reg);
	val &= ~(0x3ff << 22);
	val |= (SubTotal & 0x3ff) << 22;

	/* program MaxRdLatency to correspond with current delay */
	Set_NB32(dev, reg, val);
}


static u8 mct_SavePassRcvEnDly_D(struct DCTStatStruc *pDCTstat,
			u8 rcvrEnDly, u8 Channel,
			u8 receiver, u8 Pass)
{
	u8 i;
	u8 mask_Saved, mask_Pass;
	u8 *p;

	/* calculate dimm offset
	 * not needed for CH_D_B_RCVRDLY array
	 */

	/* cmp if there has new DqsRcvEnDly to be recorded */
	mask_Pass = pDCTstat->DqsRcvEn_Pass;

	if (Pass == SecondPass) {
		mask_Pass = ~mask_Pass;
	}

	mask_Saved = pDCTstat->DqsRcvEn_Saved;
	if (mask_Pass != mask_Saved) {

		/* find desired stack offset according to channel/dimm/byte */
		if (Pass == SecondPass) {
			// FIXME: SecondPass is never used for Barcelona p = pDCTstat->persistentData.CH_D_B_RCVRDLY_1[Channel][receiver>>1];
			p = 0; // Keep the compiler happy.
		} else {
			mask_Saved &= mask_Pass;
			p = pDCTstat->persistentData.CH_D_B_RCVRDLY[Channel][receiver>>1];
		}
		for (i = 0; i < 8; i++) {
			/* cmp per byte lane */
			if (mask_Pass & (1 << i)) {
				if (!(mask_Saved & (1 << i))) {
					/* save RcvEnDly to stack, according to
					the related Dimm/byte lane */
					p[i] = (u8)rcvrEnDly;
					mask_Saved |= 1 << i;
				}
			}
		}
		pDCTstat->DqsRcvEn_Saved = mask_Saved;
	}
	return mct_SaveRcvEnDly_D_1Pass(pDCTstat, Pass);
}


static u8 mct_CompareTestPatternQW0_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat,
					u32 addr, u8 channel,
					u8 pattern, u8 Pass)
{
	/* Compare only the first beat of data.  Since target addrs are cache
	 * line aligned, the Channel parameter is used to determine which
	 * cache QW to compare.
	 */

	u8 *test_buf;
	u8 i;
	u8 result;
	u8 value;


	if (Pass == FirstPass) {
		if (pattern == 1) {
			test_buf = (u8 *)TestPattern1_D;
		} else {
			test_buf = (u8 *)TestPattern0_D;
		}
	} else {		// Second Pass
		test_buf = (u8 *)TestPattern2_D;
	}

	SetUpperFSbase(addr);
	addr <<= 8;

	if ((pDCTstat->Status & (1<<SB_128bitmode)) && channel) {
		addr += 8;	/* second channel */
		test_buf += 8;
	}

	print_debug_dqs_pair("\t\t\t\t\t\t  test_buf = ", (u32)test_buf, "  |  addr_lo = ", addr,  4);
	for (i = 0; i < 8; i++) {
		value = read32_fs(addr);
		print_debug_dqs_pair("\t\t\t\t\t\t\t\t ", test_buf[i], "  |  ", value, 4);

		if (value == test_buf[i]) {
			pDCTstat->DqsRcvEn_Pass |= (1<<i);
		} else {
			pDCTstat->DqsRcvEn_Pass &= ~(1<<i);
		}
	}

	result = DQS_FAIL;

	if (Pass == FirstPass) {
		/* if first pass, at least one byte lane pass
		 * ,then DQS_PASS = 1 and will set to related reg.
		 */
		if (pDCTstat->DqsRcvEn_Pass != 0) {
			result = DQS_PASS;
		} else {
			result = DQS_FAIL;
		}

	} else {
		/* if second pass, at least one byte lane fail
		 * ,then DQS_FAIL = 1 and will set to related reg.
		 */
		if (pDCTstat->DqsRcvEn_Pass != 0xFF) {
			result = DQS_FAIL;
		} else {
			result = DQS_PASS;
		}
	}

	/* if second pass, we can't find the fail until FFh,
	 * then let it fail to save the final delay
	 */
	if ((Pass == SecondPass) && (pDCTstat->Status & (1 << SB_DQSRcvLimit))) {
		result = DQS_FAIL;
		pDCTstat->DqsRcvEn_Pass = 0;
	}

	/* second pass needs to be inverted
	 * FIXME? this could be inverted in the above code to start with...
	 */
	if (Pass == SecondPass) {
		if (result == DQS_PASS) {
			result = DQS_FAIL;
		} else if (result == DQS_FAIL) { /* FIXME: doesn't need to be else if */
			result = DQS_PASS;
		}
	}


	return result;
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
	u8 dn = 2; // TODO: Rev C could be 4
	u32 dev = pDCTstat->dev_dct;
	u32 index_reg = 0x98 + 0x100 * Channel;


	// FIXME: add Cx support
	dword = 0x00000000;
	for (i = 1; i <= 3; i++) {
		for (j = 0; j < dn; j++)
			/* DIMM0 Write Data Timing Low */
			/* DIMM0 Write ECC Timing */
			Set_NB32_index_wait(dev, index_reg, i + 0x100 * j, dword);
	}

	/* errata #180 */
	dword = 0x2f2f2f2f;
	for (i = 5; i <= 6; i++) {
		for (j = 0; j < dn; j++)
			/* DIMM0 Read DQS Timing Control Low */
			Set_NB32_index_wait(dev, index_reg, i + 0x100 * j, dword);
	}

	dword = 0x0000002f;
	for (j = 0; j < dn; j++)
		/* DIMM0 Read DQS ECC Timing Control */
		Set_NB32_index_wait(dev, index_reg, 7 + 0x100 * j, dword);
}


void SetEccDQSRcvrEn_D(struct DCTStatStruc *pDCTstat, u8 Channel)
{
	u32 dev;
	u32 index_reg;
	u32 index;
	u8 ChipSel;
	u8 *p;
	u32 val;

	dev = pDCTstat->dev_dct;
	index_reg = 0x98 + Channel * 0x100;
	index = 0x12;
	p = pDCTstat->persistentData.CH_D_BC_RCVRDLY[Channel];
	print_debug_dqs("\t\tSetEccDQSRcvrPos: Channel ", Channel,  2);
	for (ChipSel = 0; ChipSel < MAX_CS_SUPPORTED; ChipSel += 2) {
		val = p[ChipSel>>1];
		Set_NB32_index_wait(dev, index_reg, index, val);
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

	EccDQSLike = pDCTstat->CH_EccDQSLike[Channel];
	EccDQSScale = pDCTstat->CH_EccDQSScale[Channel];

	for (ChipSel = 0; ChipSel < MAX_CS_SUPPORTED; ChipSel += 2) {
		if (mct_RcvrRankEnabled_D(pMCTstat, pDCTstat, Channel, ChipSel)) {
			u8 *p;
			p = pDCTstat->persistentData.CH_D_B_RCVRDLY[Channel][ChipSel>>1];

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
			val >>= 8; // /256

			if (val0 > val1) {
				val -= val1;
			} else {
				val += val0;
			}

			pDCTstat->persistentData.CH_D_BC_RCVRDLY[Channel][ChipSel>>1] = val;
		}
	}
	SetEccDQSRcvrEn_D(pDCTstat, Channel);
}

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
			struct DCTStatStruc *pDCTstatA)
{

	u8 Node = 0;
	struct DCTStatStruc *pDCTstat;

	// FIXME: skip for Ax
	while (Node < MAX_NODES_SUPPORTED) {
		pDCTstat = pDCTstatA + Node;

		if (pDCTstat->DCTSysLimit) {
			fenceDynTraining_D(pMCTstat, pDCTstat, 0);
			fenceDynTraining_D(pMCTstat, pDCTstat, 1);
		}
		Node++;
	}
}


static void fenceDynTraining_D(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, u8 dct)
{
	u16 avRecValue;
	u32 val;
	u32 dev;
	u32 index_reg = 0x98 + 0x100 * dct;
	u32 index;

	/* BIOS first programs a seed value to the phase recovery engine
	 *  (recommended 19) registers.
	 * Dram Phase Recovery Control Register (F2x[1,0]9C_x[51:50] and
	 * F2x[1,0]9C_x52.) .
	 */

	dev = pDCTstat->dev_dct;
	for (index = 0x50; index <= 0x52; index ++) {
		val = (FenceTrnFinDlySeed & 0x1F);
		if (index != 0x52) {
			val |= val << 8 | val << 16 | val << 24;
		}
		Set_NB32_index_wait(dev, index_reg, index, val);
	}


	/* Set F2x[1,0]9C_x08[PhyFenceTrEn]=1. */
	val = Get_NB32_index_wait(dev, index_reg, 0x08);
	val |= 1 << PhyFenceTrEn;
	Set_NB32_index_wait(dev, index_reg, 0x08, val);

	/* Wait 200 MEMCLKs. */
	mct_Wait(50000);		/* wait 200us */

	/* Clear F2x[1,0]9C_x08[PhyFenceTrEn]=0. */
	val = Get_NB32_index_wait(dev, index_reg, 0x08);
	val &= ~(1 << PhyFenceTrEn);
	Set_NB32_index_wait(dev, index_reg, 0x08, val);

	/* BIOS reads the phase recovery engine registers
	 * F2x[1,0]9C_x[51:50] and F2x[1,0]9C_x52. */
	avRecValue = 0;
	for (index = 0x50; index <= 0x52; index ++) {
		val = Get_NB32_index_wait(dev, index_reg, index);
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
	avRecValue -= 8;
	val = Get_NB32_index_wait(dev, index_reg, 0x0C);
	val &= ~(0x1F << 16);
	val |= (avRecValue & 0x1F) << 16;
	Set_NB32_index_wait(dev, index_reg, 0x0C, val);

	/* Rewrite F2x[1,0]9C_x04-DRAM Address/Command Timing Control Register
	 * delays (both channels). */
	val = Get_NB32_index_wait(dev, index_reg, 0x04);
	Set_NB32_index_wait(dev, index_reg, 0x04, val);
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
