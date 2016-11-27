/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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

#include <inttypes.h>
#include <console/console.h>
#include <string.h>
#include "mct_d.h"
#include "mct_d_gcc.h"

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

static void SetEccWrDQS_D(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat)
{
	u8 ByteLane, DimmNum, OddByte, Addl_Index, Channel;
	u8 EccRef1, EccRef2, EccDQSScale;
	u32 val;
	u16 word;

	for (Channel = 0; Channel < 2; Channel ++) {
		for (DimmNum = 0; DimmNum < C_MAX_DIMMS; DimmNum ++) { /* we use DimmNum instead of DimmNumx3 */
			for (ByteLane = 0; ByteLane < 9; ByteLane ++) {
				/* Get RxEn initial value from WrDqs */
				if (ByteLane & 1)
					OddByte = 1;
				else
					OddByte = 0;
				if (ByteLane < 2)
					Addl_Index = 0x30;
				else if (ByteLane < 4)
					Addl_Index = 0x31;
				else if (ByteLane < 6)
					Addl_Index = 0x40;
				else if (ByteLane < 8)
					Addl_Index = 0x41;
				else
					Addl_Index = 0x32;
				Addl_Index += DimmNum * 3;

				val = Get_NB32_index_wait_DCT(pDCTstat->dev_dct, Channel, 0x98, Addl_Index);
				if (OddByte)
					val >>= 16;
				/* Save WrDqs to stack for later usage */
				pDCTstat->CH_D_B_TxDqs[Channel][DimmNum][ByteLane] = val & 0xFF;
				EccDQSScale = pDCTstat->CH_EccDQSScale[Channel];
				word = pDCTstat->CH_EccDQSLike[Channel];
				if ((word & 0xFF) == ByteLane) EccRef1 = val & 0xFF;
				if (((word >> 8) & 0xFF) == ByteLane) EccRef2 = val & 0xFF;
			}
		}
	}
}

static void EnableAutoRefresh_D(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat)
{
	u32 val;

	val = Get_NB32_DCT(pDCTstat->dev_dct, 0, 0x8C);
	val &= ~(1 << DisAutoRefresh);
	Set_NB32_DCT(pDCTstat->dev_dct, 0, 0x8C, val);

	val = Get_NB32_DCT(pDCTstat->dev_dct, 1, 0x8C);
	val &= ~(1 << DisAutoRefresh);
	Set_NB32_DCT(pDCTstat->dev_dct, 1, 0x8C, val);
}

static void DisableAutoRefresh_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat)
{
	u32 val;

	val = Get_NB32_DCT(pDCTstat->dev_dct, 0, 0x8C);
	val |= 1 << DisAutoRefresh;
	Set_NB32_DCT(pDCTstat->dev_dct, 0, 0x8C, val);

	val = Get_NB32_DCT(pDCTstat->dev_dct, 1, 0x8C);
	val |= 1 << DisAutoRefresh;
	Set_NB32_DCT(pDCTstat->dev_dct, 1, 0x8C, val);
}


static uint8_t PhyWLPass1(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct)
{
	u8 dimm;
	u16 DIMMValid;
	uint8_t status = 0;
	void *DCTPtr;

	dct &= 1;

	DCTPtr = (void *)(pDCTstat->C_DCTPtr[dct]);
	pDCTstat->DIMMValid = pDCTstat->DIMMValidDCT[dct];
	pDCTstat->CSPresent = pDCTstat->CSPresent_DCT[dct];

	if (pDCTstat->GangedMode & 1)
		pDCTstat->CSPresent = pDCTstat->CSPresent_DCT[0];

	if (pDCTstat->DIMMValid) {
		DIMMValid = pDCTstat->DIMMValid;
		PrepareC_DCT(pMCTstat, pDCTstat, dct);
		for (dimm = 0; dimm < MAX_DIMMS_SUPPORTED; dimm ++) {
			if (DIMMValid & (1 << (dimm << 1))) {
				status |= AgesaHwWlPhase1(pMCTstat, pDCTstat, dct, dimm, FirstPass);
				status |= AgesaHwWlPhase2(pMCTstat, pDCTstat, dct, dimm, FirstPass);
				status |= AgesaHwWlPhase3(pMCTstat, pDCTstat, dct, dimm, FirstPass);
			}
		}
	}

	return status;
}

static uint8_t PhyWLPass2(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, uint8_t dct, uint8_t final)
{
	u8 dimm;
	u16 DIMMValid;
	uint8_t status = 0;
	void *DCTPtr;

	dct &= 1;

	DCTPtr = (void *)&(pDCTstat->C_DCTPtr[dct]); /* todo: */
	pDCTstat->DIMMValid = pDCTstat->DIMMValidDCT[dct];
	pDCTstat->CSPresent = pDCTstat->CSPresent_DCT[dct];

	if (pDCTstat->GangedMode & 1)
		pDCTstat->CSPresent = pDCTstat->CSPresent_DCT[0];

	if (pDCTstat->DIMMValid) {
		DIMMValid = pDCTstat->DIMMValid;
		PrepareC_DCT(pMCTstat, pDCTstat, dct);
		pDCTstat->Speed = pDCTstat->DIMMAutoSpeed = pDCTstat->TargetFreq;
		pDCTstat->CASL = pDCTstat->DIMMCASL = pDCTstat->TargetCASL;
		SPD2ndTiming(pMCTstat, pDCTstat, dct);
		if (!is_fam15h()) {
			ProgDramMRSReg_D(pMCTstat, pDCTstat, dct);
			PlatformSpec_D(pMCTstat, pDCTstat, dct);
			fenceDynTraining_D(pMCTstat, pDCTstat, dct);
		}
		Restore_OnDimmMirror(pMCTstat, pDCTstat);
		StartupDCT_D(pMCTstat, pDCTstat, dct);
		Clear_OnDimmMirror(pMCTstat, pDCTstat);
		SetDllSpeedUp_D(pMCTstat, pDCTstat, dct);
		DisableAutoRefresh_D(pMCTstat, pDCTstat);
		for (dimm = 0; dimm < MAX_DIMMS_SUPPORTED; dimm ++) {
			if (DIMMValid & (1 << (dimm << 1))) {
				status |= AgesaHwWlPhase1(pMCTstat, pDCTstat, dct, dimm, SecondPass);
				status |= AgesaHwWlPhase2(pMCTstat, pDCTstat, dct, dimm, SecondPass);
				status |= AgesaHwWlPhase3(pMCTstat, pDCTstat, dct, dimm, SecondPass);
			}
		}
	}

	return status;
}

static uint16_t fam15h_next_highest_memclk_freq(uint16_t memclk_freq)
{
	uint16_t fam15h_next_highest_freq_tab[] = {0, 0, 0, 0, 0x6, 0, 0xa, 0, 0, 0, 0xe, 0, 0, 0, 0x12, 0, 0, 0, 0x16, 0, 0, 0, 0x16};
	return fam15h_next_highest_freq_tab[memclk_freq];
}

/* Write Levelization Training
 * Algorithm detailed in the Fam10h BKDG Rev. 3.62 section 2.8.9.9.1
 */
static void WriteLevelization_HW(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstatA, uint8_t Node, uint8_t Pass)
{
	uint8_t status;
	uint8_t timeout;
	uint16_t final_target_freq;

	struct DCTStatStruc *pDCTstat;
	pDCTstat = pDCTstatA + Node;

	pDCTstat->C_MCTPtr  = &(pDCTstat->s_C_MCTPtr);
	pDCTstat->C_DCTPtr[0] = &(pDCTstat->s_C_DCTPtr[0]);
	pDCTstat->C_DCTPtr[1] = &(pDCTstat->s_C_DCTPtr[1]);

	/* Disable auto refresh by configuring F2x[1, 0]8C[DisAutoRefresh] = 1 */
	DisableAutoRefresh_D(pMCTstat, pDCTstat);

	/* Disable ZQ calibration short command by F2x[1,0]94[ZqcsInterval]=00b */
	DisableZQcalibration(pMCTstat, pDCTstat);
	PrepareC_MCT(pMCTstat, pDCTstat);

	if (pDCTstat->GangedMode & (1 << 0)) {
		pDCTstat->DIMMValidDCT[1] = pDCTstat->DIMMValidDCT[0];
	}

	if (Pass == FirstPass) {
		timeout = 0;
		do {
			status = 0;
			timeout++;
			status |= PhyWLPass1(pMCTstat, pDCTstat, 0);
			status |= PhyWLPass1(pMCTstat, pDCTstat, 1);
			if (status)
				printk(BIOS_INFO,
					"%s: Retrying write levelling due to invalid value(s) detected in first phase\n",
					__func__);
		} while (status && (timeout < 8));
		if (status)
			printk(BIOS_INFO,
				"%s: Uncorrectable invalid value(s) detected in first phase of write levelling\n",
				__func__);
	}

	if (Pass == SecondPass) {
		if (pDCTstat->TargetFreq > mhz_to_memclk_config(mctGet_NVbits(NV_MIN_MEMCLK))) {
			/* 8.Prepare the memory subsystem for the target MEMCLK frequency.
			 * NOTE: BIOS must program both DCTs to the same frequency.
			 * NOTE: Fam15h steps the frequency, Fam10h slams the frequency.
			 */
			uint8_t global_phy_training_status = 0;
			final_target_freq = pDCTstat->TargetFreq;

			while (pDCTstat->Speed != final_target_freq) {
				if (is_fam15h())
					pDCTstat->TargetFreq = fam15h_next_highest_memclk_freq(pDCTstat->Speed);
				else
					pDCTstat->TargetFreq = final_target_freq;
				SetTargetFreq(pMCTstat, pDCTstatA, Node);
				timeout = 0;
				do {
					status = 0;
					timeout++;
					status |= PhyWLPass2(pMCTstat, pDCTstat, 0, (pDCTstat->TargetFreq == final_target_freq));
					status |= PhyWLPass2(pMCTstat, pDCTstat, 1, (pDCTstat->TargetFreq == final_target_freq));
					if (status)
						printk(BIOS_INFO,
							"%s: Retrying write levelling due to invalid value(s) detected in last phase\n",
							__func__);
				} while (status && (timeout < 8));
				global_phy_training_status |= status;
			}

			pDCTstat->TargetFreq = final_target_freq;

			if (global_phy_training_status)
				printk(BIOS_WARNING,
					"%s: Uncorrectable invalid value(s) detected in second phase of write levelling; "
					"continuing but system may be unstable!\n",
					__func__);

			uint8_t dct;
			for (dct = 0; dct < 2; dct++) {
				sDCTStruct *pDCTData = pDCTstat->C_DCTPtr[dct];
				memcpy(pDCTData->WLGrossDelayFinalPass, pDCTData->WLGrossDelayPrevPass, sizeof(pDCTData->WLGrossDelayPrevPass));
				memcpy(pDCTData->WLFineDelayFinalPass, pDCTData->WLFineDelayPrevPass, sizeof(pDCTData->WLFineDelayPrevPass));
				pDCTData->WLCriticalGrossDelayFinalPass = pDCTData->WLCriticalGrossDelayPrevPass;
			}
		}
	}

	SetEccWrDQS_D(pMCTstat, pDCTstat);
	EnableAutoRefresh_D(pMCTstat, pDCTstat);
	EnableZQcalibration(pMCTstat, pDCTstat);
}

void mct_WriteLevelization_HW(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstatA, uint8_t Pass)
{
	u8 Node;

	for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
		struct DCTStatStruc *pDCTstat;
		pDCTstat = pDCTstatA + Node;

		if (pDCTstat->NodePresent) {
			mctSMBhub_Init(Node);
			Clear_OnDimmMirror(pMCTstat, pDCTstat);
			WriteLevelization_HW(pMCTstat, pDCTstatA, Node, Pass);
			Restore_OnDimmMirror(pMCTstat, pDCTstat);
		}
	}
}
