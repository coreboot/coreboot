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

static void FreqChgCtrlWrd(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat);


static void AgesaDelay(u32 msec)
{
	mct_Wait(msec*10);
}

void PrepareC_MCT(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat)
{
	pDCTstat->C_MCTPtr->AgesaDelay = AgesaDelay;
	pDCTstat->C_MCTPtr->PlatMaxTotalDimms = mctGet_NVbits(NV_MAX_DIMMS);
	pDCTstat->C_MCTPtr->PlatMaxDimmsDct = pDCTstat->C_MCTPtr->PlatMaxTotalDimms >> 1;
}

void PrepareC_DCT(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct)
{
	u8 dimm;
	u16 DimmValid;
	u16 Dimmx8Present;

	dct &= 1;

	pDCTstat->C_DCTPtr[dct]->DctTrain = dct;

	if (dct == 1) {
		Dimmx8Present = pDCTstat->Dimmx8Present >> 1;
	} else
		Dimmx8Present = pDCTstat->Dimmx8Present;
	Dimmx8Present &= 0x55;

	pDCTstat->C_DCTPtr[dct]->MaxDimmsInstalled = pDCTstat->MAdimms[dct];
	DimmValid = pDCTstat->DIMMValidDCT[dct];

	pDCTstat->C_DCTPtr[dct]->NodeId = pDCTstat->Node_ID;
	pDCTstat->C_DCTPtr[dct]->LogicalCPUID = pDCTstat->LogicalCPUID;

	for (dimm = 0; dimm < MAX_DIMMS; dimm++) {
		if (DimmValid & (1 << dimm))
			pDCTstat->C_DCTPtr[dct]->DimmPresent[dimm] = 1;
		if (Dimmx8Present & (1 << dimm))
			pDCTstat->C_DCTPtr[dct]->DimmX8Present[dimm] = 1;
	}

	if (pDCTstat->GangedMode & (1 << 0))
		pDCTstat->C_DCTPtr[dct]->CurrDct = 0;
	else
		pDCTstat->C_DCTPtr[dct]->CurrDct = dct;

	pDCTstat->C_DCTPtr[dct]->DctCSPresent = pDCTstat->CSPresent_DCT[dct];
	if (!(pDCTstat->GangedMode & (1 << 0)) && (dct == 1))
		pDCTstat->C_DCTPtr[dct]->DctCSPresent = pDCTstat->CSPresent_DCT[0];

	if (pDCTstat->Status & (1 << SB_Registered)) {
		pDCTstat->C_DCTPtr[dct]->Status[DCT_STATUS_REGISTERED] = 1;
		pDCTstat->C_DCTPtr[dct]->Status[DCT_STATUS_OnDimmMirror] = 0;
	} else {
		if (pDCTstat->MirrPresU_NumRegR > 0)
			pDCTstat->C_DCTPtr[dct]->Status[DCT_STATUS_OnDimmMirror] = 1;
		pDCTstat->C_DCTPtr[dct]->Status[DCT_STATUS_REGISTERED] = 0;
	}

	pDCTstat->C_DCTPtr[dct]->RegMan1Present = pDCTstat->RegMan1Present;

	for (dimm = 0; dimm < MAX_TOTAL_DIMMS; dimm++) {
		u8  DimmRanks;
		if (DimmValid & (1 << (dimm << 1))) {
			DimmRanks = 1;
			if (pDCTstat->DimmDRPresent & (1 << (dimm+dct)))
				DimmRanks = 2;
			else if (pDCTstat->DimmQRPresent & (1 << (dimm+dct)))
				DimmRanks = 4;
		} else
			DimmRanks = 0;
		pDCTstat->C_DCTPtr[dct]->DimmRanks[dimm] = DimmRanks;
	}
}

void EnableZQcalibration(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat)
{
	u32 val;

	val = Get_NB32(pDCTstat->dev_dct, 0x94);
	val |= 1 << 11;
	Set_NB32(pDCTstat->dev_dct, 0x94, val);

	val = Get_NB32(pDCTstat->dev_dct, 0x94 + 0x100);
	val |= 1 << 11;
	Set_NB32(pDCTstat->dev_dct, 0x94 + 0x100, val);
}

void DisableZQcalibration(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat)
{
	u32 val;

	val = Get_NB32(pDCTstat->dev_dct, 0x94);
	val &= ~(1 << 11);
	val &= ~(1 << 10);
	Set_NB32(pDCTstat->dev_dct, 0x94, val);

	val = Get_NB32(pDCTstat->dev_dct, 0x94 + 0x100);
	val &= ~(1 << 11);
	val &= ~(1 << 10);
	Set_NB32(pDCTstat->dev_dct, 0x94 + 0x100, val);
}

static void EnterSelfRefresh(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat)
{
	u8 DCT0Present, DCT1Present;
	u32 val;

	DCT0Present = pDCTstat->DIMMValidDCT[0];
	if (pDCTstat->GangedMode)
		DCT1Present = 0;
	else
		DCT1Present = pDCTstat->DIMMValidDCT[1];

	/* Program F2x[1, 0]90[EnterSelfRefresh]=1. */
	if (DCT0Present) {
		val = Get_NB32(pDCTstat->dev_dct, 0x90);
		val |= 1 << EnterSelfRef;
		Set_NB32(pDCTstat->dev_dct, 0x90, val);
	}
	if (DCT1Present) {
		val = Get_NB32(pDCTstat->dev_dct, 0x90 + 0x100);
		val |= 1 << EnterSelfRef;
		Set_NB32(pDCTstat->dev_dct, 0x90 + 0x100, val);
	}
	/* Wait until the hardware resets F2x[1, 0]90[EnterSelfRefresh]=0. */
	if (DCT0Present)
		do {
			val = Get_NB32(pDCTstat->dev_dct, 0x90);
		} while (val & (1 <<EnterSelfRef));
	if (DCT1Present)
		do {
			val = Get_NB32(pDCTstat->dev_dct, 0x90 + 0x100);
		} while (val & (1 <<EnterSelfRef));
}

/*
 * Change memclk for write levelization pass 2
 */
static void ChangeMemClk(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat)
{
	u8 DCT0Present, DCT1Present;
	u32 val;

	DCT0Present = pDCTstat->DIMMValidDCT[0];
	if (pDCTstat->GangedMode)
		DCT1Present = 0;
	else
		DCT1Present = pDCTstat->DIMMValidDCT[1];

	/* Program F2x[1, 0]90[EnterSelfRefresh]=1. */
	if (DCT0Present) {
		val = Get_NB32_index_wait(pDCTstat->dev_dct, 0x98, 8);
		val |= 1 << DisAutoComp;
		Set_NB32_index_wait(pDCTstat->dev_dct, 0x98, 8, val);
	}
	if (DCT1Present) {
		val = Get_NB32_index_wait(pDCTstat->dev_dct, 0x98 + 0x100, 8);
		val |= 1 << DisAutoComp;
		Set_NB32_index_wait(pDCTstat->dev_dct, 0x98 + 0x100, 8, val);
	}

	/* Program F2x[1, 0]94[MemClkFreqVal] = 0. */
	if (DCT0Present) {
		val = Get_NB32(pDCTstat->dev_dct, 0x94);
		val &= ~(1 << MemClkFreqVal);
		Set_NB32(pDCTstat->dev_dct, 0x94, val);
	}
	if (DCT1Present) {
		val = Get_NB32(pDCTstat->dev_dct, 0x94 + 0x100);
		val &= ~(1 << MemClkFreqVal);
		Set_NB32(pDCTstat->dev_dct, 0x94 + 0x100, val);
	}

	/* Program F2x[1, 0]94[MemClkFreq] to specify the target MEMCLK frequency. */
	if (DCT0Present) {
		val = Get_NB32(pDCTstat->dev_dct, 0x94);
		val &= 0xFFFFFFF8;
		val |= pDCTstat->TargetFreq - 1;
		Set_NB32(pDCTstat->dev_dct, 0x94, val);
	}
	if (DCT1Present) {
		val = Get_NB32(pDCTstat->dev_dct, 0x94 + 0x100);
		val &= 0xFFFFFFF8;
		val |= pDCTstat->TargetFreq - 1;
		Set_NB32(pDCTstat->dev_dct, 0x94 + 0x100, val);
	}

	/* Program F2x[1, 0]94[MemClkFreqVal] = 1. */
	if (DCT0Present) {
		val = Get_NB32(pDCTstat->dev_dct, 0x94);
		val |= 1 << MemClkFreqVal;
		Set_NB32(pDCTstat->dev_dct, 0x94, val);
	}
	if (DCT1Present) {
		val = Get_NB32(pDCTstat->dev_dct, 0x94 + 0x100);
		val |= 1 << MemClkFreqVal;
		Set_NB32(pDCTstat->dev_dct, 0x94 + 0x100, val);
	}

	/* Wait until F2x[1, 0]94[FreqChgInProg]=0. */
	if (DCT0Present)
		do {
			val = Get_NB32(pDCTstat->dev_dct, 0x94);
		} while (val & (1 << FreqChgInProg));
	if (DCT1Present)
		do {
			val = Get_NB32(pDCTstat->dev_dct, 0x94 + 0x100);
		} while (val & (1 << FreqChgInProg));

	/* Program F2x[1, 0]94[MemClkFreqVal] = 0. */
	if (DCT0Present) {
		val = Get_NB32_index_wait(pDCTstat->dev_dct, 0x98, 8);
		val &= ~(1 << DisAutoComp);
		Set_NB32_index_wait(pDCTstat->dev_dct, 0x98, 8, val);
	}
	if (DCT1Present) {
		val = Get_NB32_index_wait(pDCTstat->dev_dct, 0x98 + 0x100, 8);
		val &= ~(1 << DisAutoComp);
		Set_NB32_index_wait(pDCTstat->dev_dct, 0x98 + 0x100, 8, val);
	}
}

/* Multiply the previously saved delay values in Pass 1, step #5 by
   (target frequency)/400 to find the gross and fine delay initialization
   values at the target frequency.
 */
void MultiplyDelay(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct)
{
	u16 index;
	u8 Multiplier;
	u8 gross, fine;
	u16 total;

	Multiplier = pDCTstat->TargetFreq;

	for (index=0; index < MAX_BYTE_LANES*MAX_LDIMMS; index ++) {
		gross = pDCTstat->C_DCTPtr[dct]->WLGrossDelay[index];
		fine = pDCTstat->C_DCTPtr[dct]->WLFineDelay[index];

		total = gross << 5 | fine;
		total *= Multiplier;
		if (total % 3)
			total = total / 3 + 1;
		else
			total = total / 3;
		pDCTstat->C_DCTPtr[dct]->WLGrossDelay[index] = (total & 0xFF) >> 5;
		pDCTstat->C_DCTPtr[dct]->WLFineDelay[index] = total & 0x1F;
	}
}

/*
 * the DRAM controller to bring the DRAMs out of self refresh mode.
 */
static void ExitSelfRefresh(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat)
{
	u8 DCT0Present, DCT1Present;
	u32 val;

	DCT0Present = pDCTstat->DIMMValidDCT[0];
	if (pDCTstat->GangedMode)
		DCT1Present = 0;
	else
		DCT1Present = pDCTstat->DIMMValidDCT[1];

	/* Program F2x[1, 0]90[ExitSelfRef]=1 for both DCTs. */
	if (DCT0Present) {
		val = Get_NB32(pDCTstat->dev_dct, 0x90);
		val |= 1 << ExitSelfRef;
		Set_NB32(pDCTstat->dev_dct, 0x90, val);
	}
	if (DCT1Present) {
		val = Get_NB32(pDCTstat->dev_dct, 0x90 + 0x100);
		val |= 1 << ExitSelfRef;
		Set_NB32(pDCTstat->dev_dct, 0x90 + 0x100, val);
	}
	/* Wait until the hardware resets F2x[1, 0]90[ExitSelfRef]=0. */
	if (DCT0Present)
		do {
			val = Get_NB32(pDCTstat->dev_dct, 0x90);
		} while (val & (1 << ExitSelfRef));
	if (DCT1Present)
		do {
			val = Get_NB32(pDCTstat->dev_dct, 0x90 + 0x100);
		} while (val & (1 << ExitSelfRef));
}

void SetTargetFreq(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat)
{
	/* Program F2x[1,0]90[EnterSelfRefresh]=1.
	 * Wait until the hardware resets F2x[1,0]90[EnterSelfRefresh]=0.
	 */
	EnterSelfRefresh(pMCTstat, pDCTstat);

	/*
	 * Program F2x[1,0]9C_x08[DisAutoComp]=1
	 * Program F2x[1,0]94[MemClkFreqVal] = 0.
	 * Program F2x[1,0]94[MemClkFreq] to specify the target MEMCLK frequency.
	 * Program F2x[1,0]94[MemClkFreqVal] = 1.
	 * Wait until F2x[1,0]94[FreqChgInProg]=0.
	 * Program F2x[1,0]9C_x08[DisAutoComp]=0
	 */
	ChangeMemClk(pMCTstat, pDCTstat);

	/* Program F2x[1,0]90[ExitSelfRef]=1 for both DCTs.
	 * Wait until the hardware resets F2x[1, 0]90[ExitSelfRef]=0.
	 */
	ExitSelfRefresh(pMCTstat, pDCTstat);

	/* wait for 500 MCLKs after ExitSelfRef, 500*2.5ns=1250ns */
	mct_Wait(250);

	if (pDCTstat->Status & (1 << SB_Registered)) {
		u8 DCT0Present, DCT1Present;

		DCT0Present = pDCTstat->DIMMValidDCT[0];
		if (pDCTstat->GangedMode)
			DCT1Present = 0;
		else
			DCT1Present = pDCTstat->DIMMValidDCT[1];

		if (!DCT1Present)
			pDCTstat->CSPresent = pDCTstat->CSPresent_DCT[0];
		else if (pDCTstat->GangedMode) {
			pDCTstat->CSPresent = 0;
		} else
			pDCTstat->CSPresent = pDCTstat->CSPresent_DCT[1];

		FreqChgCtrlWrd(pMCTstat, pDCTstat);
	}
}

static void Modify_OnDimmMirror(struct DCTStatStruc *pDCTstat, u8 dct, u8 set)
{
	u32 val;
	u32 reg_off = dct * 0x100 + 0x44;
	while (reg_off < (dct * 0x100 + 0x60)) {
		val = Get_NB32(pDCTstat->dev_dct, reg_off);
		if (val & (1 << CSEnable))
			set ? (val |= 1 << onDimmMirror) : (val &= ~(1<<onDimmMirror));
		Set_NB32(pDCTstat->dev_dct, reg_off, val);
		reg_off += 8;
	}
}

void Restore_OnDimmMirror(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat)
{
	if (pDCTstat->LogicalCPUID & (AMD_DR_Bx /* | AMD_RB_C0 */)) { /* We dont support RB-C0 now */
		if (pDCTstat->MirrPresU_NumRegR & 0x55)
			Modify_OnDimmMirror(pDCTstat, 0, 1); /* dct=0, set */
		if (pDCTstat->MirrPresU_NumRegR & 0xAA)
			Modify_OnDimmMirror(pDCTstat, 1, 1); /* dct=1, set */
	}
}
void Clear_OnDimmMirror(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat)
{
	if (pDCTstat->LogicalCPUID & (AMD_DR_Bx /* | AMD_RB_C0 */)) { /* We dont support RB-C0 now */
		if (pDCTstat->MirrPresU_NumRegR & 0x55)
			Modify_OnDimmMirror(pDCTstat, 0, 0); /* dct=0, clear */
		if (pDCTstat->MirrPresU_NumRegR & 0xAA)
			Modify_OnDimmMirror(pDCTstat, 1, 0); /* dct=1, clear */
	}
}
