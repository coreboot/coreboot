/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
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


static void Get_ChannelPS_Cfg0_D(u8 MAAdimms, u8 Speed, u8 MAAload,
				u8 DATAAload, u32 *AddrTmgCTL, u32 *ODC_CTL);


void mctGet_PS_Cfg_D(struct MCTStatStruc *pMCTstat,
			 struct DCTStatStruc *pDCTstat, u32 dct)
{
	u16 val, valx;

	print_tx("dct: ", dct);
	print_tx("Speed: ", pDCTstat->Speed);

	Get_ChannelPS_Cfg0_D(pDCTstat->MAdimms[dct], pDCTstat->Speed,
				pDCTstat->MAload[dct], pDCTstat->DATAload[dct],
				&(pDCTstat->CH_ADDR_TMG[dct]), &(pDCTstat->CH_ODC_CTL[dct]));


	if(pDCTstat->MAdimms[dct] == 1)
		pDCTstat->CH_ODC_CTL[dct] |= 0x20000000;	/* 75ohms */
	else
		pDCTstat->CH_ODC_CTL[dct] |= 0x10000000;	/* 150ohms */

	pDCTstat->_2Tmode = 1;

	/* use byte lane 4 delay for ECC lane */
	pDCTstat->CH_EccDQSLike[0] = 0x0504;
	pDCTstat->CH_EccDQSScale[0] = 0;	/* 100% byte lane 4 */
	pDCTstat->CH_EccDQSLike[1] = 0x0504;
	pDCTstat->CH_EccDQSScale[1] = 0;	/* 100% byte lane 4 */


	/*
	 Overrides and/or exceptions
	*/

	/* 1) QRx4 needs to adjust CS/ODT setup time */
	// FIXME: Add Ax support?
	if (mctGet_NVbits(NV_MAX_DIMMS) == 4) {
		if (pDCTstat->DimmQRPresent != 0) {
			pDCTstat->CH_ADDR_TMG[dct] &= 0xFF00FFFF;
			pDCTstat->CH_ADDR_TMG[dct] |= 0x00000000;
			if (pDCTstat->MAdimms[dct] == 4) {
				pDCTstat->CH_ADDR_TMG[dct] &= 0xFF00FFFF;
				pDCTstat->CH_ADDR_TMG[dct] |= 0x002F0000;
				if (pDCTstat->Speed == 3 || pDCTstat->Speed == 4) {
					pDCTstat->CH_ADDR_TMG[dct] &= 0xFF00FFFF;
					pDCTstat->CH_ADDR_TMG[dct] |= 0x00002F00;
					if (pDCTstat->MAdimms[dct] == 4)
						pDCTstat->CH_ODC_CTL[dct] = 0x00331222;
				}
			}
		}
	}


	/* 2) DRx4 (R/C-J) @ DDR667 needs to adjust CS/ODT setup time */
	if (pDCTstat->Speed == 3 || pDCTstat->Speed == 4) {
		val = pDCTstat->Dimmx4Present;
		if (dct == 0) {
			val &= 0x55;
		} else {
			val &= 0xAA;
			val >>= 1;
		}
		val &= pDCTstat->DIMMValid;
		if (val) {
			//FIXME: skip for Ax
			valx = pDCTstat->DimmDRPresent;
			if (dct == 0) {
				valx &= 0x55;
			} else {
				valx &= 0xAA;
				valx >>= 1;
			}
			if (mctGet_NVbits(NV_MAX_DIMMS) == 8) {
				val &= valx;
				if (val != 0) {
					pDCTstat->CH_ADDR_TMG[dct] &= 0xFFFF00FF;
					pDCTstat->CH_ADDR_TMG[dct] |= 0x00002F00;
				}
			} else {
				val &= valx;
				if (val != 0) {
					if (pDCTstat->Speed == 3 || pDCTstat->Speed == 3) {
						pDCTstat->CH_ADDR_TMG[dct] &= 0xFFFF00FF;
						pDCTstat->CH_ADDR_TMG[dct] |= 0x00002F00;
					}
				}

			}
		}
	}


	pDCTstat->CH_ODC_CTL[dct] = procOdtWorkaround(pDCTstat, dct, pDCTstat->CH_ODC_CTL[dct]);

	print_tx("CH_ODC_CTL: ", pDCTstat->CH_ODC_CTL[dct]);
	print_tx("CH_ADDR_TMG: ", pDCTstat->CH_ADDR_TMG[dct]);


}


/*===============================================================================
 * Vendor is responsible for correct settings.
 * M2/Unbuffered 4 Slot - AMD Design Guideline.
 *===============================================================================
 * #1, BYTE, Speed (DCTStatstruc.Speed) (Secondary Key)
 * #2, BYTE, number of Address bus loads on the Channel. (Tershery Key)
 *           These must be listed in ascending order.
 *           FFh (0xFE) has special meaning of 'any', and must be listed first for each speed grade.
 * #3, DWORD, Address Timing Control Register Value
 * #4, DWORD, Output Driver Compensation Control Register Value
 * #5, BYTE, Number of DIMMs (Primary Key)
 */
static const u8 Table_ATC_ODC_8D_D[] = {
	0xFE, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x22, 0x12, 0x11, 0x00, 1,
	0xFE, 0xFF, 0x00, 0x00, 0x37, 0x00, 0x22, 0x12, 0x11, 0x00, 2,
	   1, 0xFF, 0x00, 0x00, 0x2F, 0x00, 0x22, 0x12, 0x11, 0x00, 3,
	   2, 0xFF, 0x00, 0x00, 0x2F, 0x00, 0x22, 0x12, 0x11, 0x00, 3,
	   3, 0xFF, 0x2F, 0x00, 0x2F, 0x00, 0x22, 0x12, 0x11, 0x00, 3,
	   4, 0xFF, 0x2F, 0x00, 0x2F, 0x00, 0x22, 0x12, 0x33, 0x00, 3,
	   1, 0xFF, 0x00, 0x00, 0x2F, 0x00, 0x22, 0x12, 0x11, 0x00, 4,
	   2, 0xFF, 0x00, 0x00, 0x2F, 0x00, 0x22, 0x12, 0x11, 0x00, 4,
	   3, 0xFF, 0x2F, 0x00, 0x2F, 0x00, 0x22, 0x12, 0x33, 0x00, 4,
	   4, 0xFF, 0x2F, 0x00, 0x2F, 0x00, 0x22, 0x12, 0x33, 0x00, 4,
	0xFF
};

static const u8 Table_ATC_ODC_4D_D[] = {
	0xFE, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x22, 0x12, 0x11, 0x00, 1,
	0xFE, 0xFF, 0x00, 0x00, 0x37, 0x00, 0x22, 0x12, 0x11, 0x00, 2,
	0xFE, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x22, 0x12, 0x11, 0x00, 3,
	0xFE, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x22, 0x12, 0x11, 0x00, 4,
	0xFF
};

static const u8 Table_ATC_ODC_8D_D_Ax[] = {
	1,0xff,0x00,0x00,0x2F,0x0,0x22,0x12,0x11,0x00, 0xFE,
	2,0xff,0x00,0x00,0x2C,0x0,0x22,0x12,0x11,0x00, 0xFE,
	3,0xff,0x00,0x00,0x2C,0x0,0x22,0x12,0x11,0x00, 0xFE,
	4,0xff,0x00,0x33,0x2F,0x0,0x22,0x12,0x11,0x00, 0xFE,
	0xFF
};

static const u8 Table_ATC_ODC_4D_D_Ax[] = {
	1,0xff,0x00,0x00,0x2F,0x00,0x22,0x12,0x11,0x00, 0xFE,
	2,0xff,0x00,0x2C,0x2C,0x00,0x22,0x12,0x11,0x00, 0xFE,
	3,0xff,0x00,0x00,0x2C,0x00,0x22,0x12,0x11,0x00, 0xFE,
	4,0xff,0x00,0x33,0x2F,0x00,0x22,0x12,0x11,0x00, 0xFE,
	0xFF
};


static void Get_ChannelPS_Cfg0_D(u8 MAAdimms, u8 Speed, u8 MAAload,
				u8 DATAAload, u32 *AddrTmgCTL, u32 *ODC_CTL)
{
	const u8 *p;

	*AddrTmgCTL = 0;
	*ODC_CTL = 0;

	if(mctGet_NVbits(NV_MAX_DIMMS) == 8) {
		/* 8 DIMM Table */
		p = Table_ATC_ODC_8D_D;
		//FIXME Add Ax support
	} else {
		/* 4 DIMM Table*/
		p = Table_ATC_ODC_4D_D;
		//FIXME Add Ax support
	}

	while (*p != 0xFF) {
		if ((MAAdimms == *(p+10)) || (*(p+10 ) == 0xFE)) {
			if((*p == Speed) || (*p == 0xFE)) {
				if(MAAload <= *(p+1)) {
					*AddrTmgCTL = stream_to_int((u8*)(p+2));
					*ODC_CTL = stream_to_int((u8*)(p+6));
					break;
				}
			}
		}
	p+=11;
	}
}

