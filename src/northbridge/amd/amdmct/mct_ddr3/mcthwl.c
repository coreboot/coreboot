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

static void SetTargetFreq(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat);
static void AgesaHwWlPhase1(sMCTStruct *pMCTData,
					sDCTStruct *pDCTData, u8 dimm, u8 pass);
static void EnableZQcalibration(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat);
static void DisableZQcalibration(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat);
static void PrepareC_MCT(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat);
static void PrepareC_DCT(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat, u8 dct);
static void MultiplyDelay(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat, u8 dct);
static void Restore_OnDimmMirror(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat);
static void Clear_OnDimmMirror(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat);

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

				val = Get_NB32_index_wait(pDCTstat->dev_dct, Channel * 0x100 + 0x98, Addl_Index);
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

	val = Get_NB32(pDCTstat->dev_dct, 0x8C);
	val &= ~(1 << DisAutoRefresh);
	Set_NB32(pDCTstat->dev_dct, 0x8C, val);

	val = Get_NB32(pDCTstat->dev_dct, 0x8C + 0x100);
	val &= ~(1 << DisAutoRefresh);
	Set_NB32(pDCTstat->dev_dct, 0x8C + 0x100, val);
}

static void DisableAutoRefresh_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat)
{
	u32 val;

	val = Get_NB32(pDCTstat->dev_dct, 0x8C);
	val |= 1 << DisAutoRefresh;
	Set_NB32(pDCTstat->dev_dct, 0x8C, val);

	val = Get_NB32(pDCTstat->dev_dct, 0x8C + 0x100);
	val |= 1 << DisAutoRefresh;
	Set_NB32(pDCTstat->dev_dct, 0x8C + 0x100, val);
}


static void PhyWLPass1(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct)
{
	u8 dimm;
	u16 DIMMValid;
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
			if (DIMMValid & (1 << (dimm << 1)))
				AgesaHwWlPhase1(pDCTstat->C_MCTPtr, DCTPtr, dimm, FirstPass);
		}
	}
}

static void PhyWLPass2(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct)
{
	u8 dimm;
	u16 DIMMValid;
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
		ProgDramMRSReg_D(pMCTstat, pDCTstat, dct);
		PlatformSpec_D(pMCTstat, pDCTstat, dct);
		fenceDynTraining_D(pMCTstat, pDCTstat, dct);
		Restore_OnDimmMirror(pMCTstat, pDCTstat);
		StartupDCT_D(pMCTstat, pDCTstat, dct);
		Clear_OnDimmMirror(pMCTstat, pDCTstat);
		SetDllSpeedUp_D(pMCTstat, pDCTstat, dct);
		DisableAutoRefresh_D(pMCTstat, pDCTstat);
		MultiplyDelay(pMCTstat, pDCTstat, dct);
		for (dimm = 0; dimm < MAX_DIMMS_SUPPORTED; dimm ++) {
			if (DIMMValid & (1 << (dimm << 1)))
				AgesaHwWlPhase1(pDCTstat->C_MCTPtr, pDCTstat->C_DCTPtr[dct], dimm, SecondPass);
		}
	}
}

static void WriteLevelization_HW(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat)
{
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

	PhyWLPass1(pMCTstat, pDCTstat, 0);
	PhyWLPass1(pMCTstat, pDCTstat, 1);

	if (pDCTstat->TargetFreq > 4) {
		/* 8.Prepare the memory subsystem for the target MEMCLK frequency.
		 * Note: BIOS must program both DCTs to the same frequency.
		 */
		SetTargetFreq(pMCTstat, pDCTstat);
		PhyWLPass2(pMCTstat, pDCTstat, 0);
		PhyWLPass2(pMCTstat, pDCTstat, 1);
	}

	SetEccWrDQS_D(pMCTstat, pDCTstat);
	EnableAutoRefresh_D(pMCTstat, pDCTstat);
	EnableZQcalibration(pMCTstat, pDCTstat);
}

void mct_WriteLevelization_HW(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstatA)
{
	u8 Node;

	for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
		struct DCTStatStruc *pDCTstat;
		pDCTstat = pDCTstatA + Node;

		if (pDCTstat->NodePresent) {
			mctSMBhub_Init(Node);
			Clear_OnDimmMirror(pMCTstat, pDCTstat);
			WriteLevelization_HW(pMCTstat, pDCTstat);
			Restore_OnDimmMirror(pMCTstat, pDCTstat);
		}
	}
}
