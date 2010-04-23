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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

static u32 mct_ControlRC(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, u32 MrsChipSel, u32 CtrlWordNum)
{
	u8 Dimms, DimmNum, MaxDimm, Speed;
	u32 val;

	DimmNum = MrsChipSel >> 20;

	/* assume dct=0; */
	/* if (dct == 1) */
	/* DimmNum ++; */
	/* cl +=8; */

	MaxDimm = mctGet_NVbits(NV_MAX_DIMMS);
	Speed = pDCTstat->DIMMAutoSpeed;
	/* if (dct == 0) */
	Dimms = pDCTstat->MAdimms[0];

	val = 0;
	if (CtrlWordNum == 0)
		val |= 1 << 1;
	else if (CtrlWordNum == 1) {
		if (!((pDCTstat->DimmDRPresent | pDCTstat->DimmQRPresent) & (1 << DimmNum)))
			val |= 0xC; /* if single rank, set DBA1 and DBA0 */
	}
	else if (CtrlWordNum == 2) {
		if (MaxDimm == 4) {
			if (Speed == 4) {
				if (((pDCTstat->DimmQRPresent & (1 << DimmNum)) && (Dimms == 1)) || Dimms == 2)
					if (!(pDCTstat->MirrPresU_NumRegR & (1 << DimmNum)))
						val |= 1 << 2;
			} else {
				if (pDCTstat->MirrPresU_NumRegR & (1 << DimmNum))
					val |= 2;
			}
		} else {
			if (Dimms > 1)
				val |= 2;
		}
	} else if (CtrlWordNum == 3) {
		val = pDCTstat->CtrlWrd3 >> (DimmNum << 2);
	} else if (CtrlWordNum == 4) {
		val = pDCTstat->CtrlWrd4 >> (DimmNum << 2);
	} else if (CtrlWordNum == 5) {
		val = pDCTstat->CtrlWrd5 >> (DimmNum << 2);
	} else if (CtrlWordNum == 8) {
		if (MaxDimm == 4)
			if (Speed == 4)
				if (pDCTstat->MirrPresU_NumRegR & (1 << DimmNum))
					val |= 1 << 2;
	} else if (CtrlWordNum == 9) {
		val |= 0xD;	/* DBA1, DBA0, DA3 = 0 */
	}
	val &= 0xf;

	val = MrsChipSel | ((val >> 2) & 3) << 16 | MrsChipSel | ((val >> 2) & 3);

	/* transfer Control word number to address [BA2,A2,A1,A0] */
	if (CtrlWordNum > 7) {
		val |= 1 << 18;
		CtrlWordNum &= 7;
	}
	val |= CtrlWordNum;

	return val;
}

static void mct_SendCtrlWrd(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, u32 val)
{
	u32 dev = pDCTstat->dev_dct;

	val |= Get_NB32(dev, 0x7C) & ~0xFFFFFF;
	val |= 1 << SendControlWord;
	Set_NB32(dev, 0x7C, val);

	do {
		val = Get_NB32(dev, 0x7C);
	} while (val & (1 << SendControlWord));
}

void mct_DramControlReg_Init_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct)
{
	u8 MrsChipSel;
	u32 dev = pDCTstat->dev_dct;
	u32 val, cw;

	mct_Wait(1600);

	mct_Wait(1200);

	for (MrsChipSel = 0; MrsChipSel < 8; MrsChipSel ++, MrsChipSel ++) {
		if (pDCTstat->CSPresent & (1 << MrsChipSel)) {
			val = Get_NB32(dev, 0xA8);
			val &= ~(0xF << 8);

			switch (MrsChipSel) {
			case 0:
			case 1:
				val |= 3 << 8;
			case 2:
			case 3:
				val |= (3 << 2) << 8;
			case 4:
			case 5:
				val |= (3 << 4) << 8;
			case 6:
			case 7:
				val |= (3 << 6) << 8;
			}
			Set_NB32(dev, 0xA8, val);

			for (cw=0; cw <=15; cw ++) {
				mct_Wait(1600);
				if (!(cw==6 || cw==7)) {
					val = mct_ControlRC(pMCTstat, pDCTstat, MrsChipSel << 20, cw);
					mct_SendCtrlWrd(pMCTstat, pDCTstat, val);
				}
			}
		}
	}

	mct_Wait(1200);
}

void FreqChgCtrlWrd(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat)
{
	u32 SaveSpeed = pDCTstat->DIMMAutoSpeed;
	u32 MrsChipSel;
	u32 dev = pDCTstat->dev_dct;
	u32 val;

	pDCTstat->DIMMAutoSpeed = pDCTstat->TargetFreq;
	for (MrsChipSel=0; MrsChipSel < 8; MrsChipSel++, MrsChipSel++) {
		if (pDCTstat->CSPresent & (1 << MrsChipSel)) {
			val = Get_NB32(dev, 0xA8);
			val &= ~(0xFF << 8);
			val |= (0x3 << MrsChipSel) << 8;
			Set_NB32(dev, 0xA8, val);

			mct_Wait(1600);
			switch (pDCTstat->TargetFreq) {
			case 6:
				mct_SendCtrlWrd(pMCTstat, pDCTstat, MrsChipSel << 20 | 0x4000A);
				break;
			case 5:
				mct_SendCtrlWrd(pMCTstat, pDCTstat, MrsChipSel << 20 | 0x40012);
				break;
			case 7:
				mct_SendCtrlWrd(pMCTstat, pDCTstat, MrsChipSel << 20 | 0x4001A);
				break;
			}

			mct_Wait(1600);

			val = mct_ControlRC(pMCTstat, pDCTstat, MrsChipSel << 20, 2);
			mct_SendCtrlWrd(pMCTstat, pDCTstat, val);

			mct_Wait(1600);

			/* Resend control word 8 */
			val = mct_ControlRC(pMCTstat, pDCTstat, MrsChipSel << 20, 8);
			mct_SendCtrlWrd(pMCTstat, pDCTstat, val);

			mct_Wait(1600);
		}
	}
	pDCTstat->DIMMAutoSpeed = SaveSpeed;
}
