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

static u32 mct_MR1Odt_RDimm(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct, u32 MrsChipSel)
{
	u8 Speed = pDCTstat->Speed;
	u32 ret;
	u8 DimmsInstalled, DimmNum, ChipSelect;

	ChipSelect = (MrsChipSel >> 20) & 0xF;
	DimmNum = ChipSelect & 0xFE;
	DimmsInstalled = pDCTstat->MAdimms[dct];
	if (dct == 1)
		DimmNum ++;
	ret = 0;

	if (mctGet_NVbits(NV_MAX_DIMMS) == 4) {
		if (DimmsInstalled == 1)
			ret |= 1 << 2;
		else {
			if (pDCTstat->CSPresent & 0xF0) {
				if (pDCTstat->DimmQRPresent & (1 << DimmNum)) {
					if (!(ChipSelect & 1))
						ret |= 1 << 2;
				} else
					ret |= 0x204;
			} else {
				if (Speed < 6)
					ret |= 0x44;
				else
					ret |= 0x204;
			}
		}
	} else if (DimmsInstalled == 1)
		ret |= 1 << 2;
	else if (Speed < 6)
		ret |= 0x44;
	else
		ret |= 0x204;

	//ret = 0;
	return ret;
}

static u32 mct_DramTermDyn_RDimm(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dimm)
{
	u8 DimmsInstalled = dimm;
	u32 DramTermDyn = 0;
	u8 Speed = pDCTstat->Speed;

	if (mctGet_NVbits(NV_MAX_DIMMS) == 4) {
		if (pDCTstat->CSPresent & 0xF0) {
			if (DimmsInstalled == 1)
				if (Speed == 7)
					DramTermDyn |= 1 << 10;
				else
					DramTermDyn |= 1 << 11;
			else
				if (Speed == 4)
					DramTermDyn |= 1 << 11;
				else
					DramTermDyn |= 1 << 10;
		} else {
			if (DimmsInstalled != 1) {
				if (Speed == 7)
					DramTermDyn |= 1 << 10;
				else
					DramTermDyn |= 1 << 11;
			}
		}
	} else {
		if (DimmsInstalled != 1)
			DramTermDyn |= 1 << 11;
	}
	return DramTermDyn;
}
