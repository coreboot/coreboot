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
				u8 DATAAload, u32 *AddrTmgCTL, u32 *ODC_CTL,
				u8 *CMDmode);


void mctGet_PS_Cfg_D(struct MCTStatStruc *pMCTstat,
			 struct DCTStatStruc *pDCTstat, u32 dct)
{
	print_tx("dct: ", dct);
	print_tx("Speed: ", pDCTstat->Speed);

	Get_ChannelPS_Cfg0_D(pDCTstat->MAdimms[dct], pDCTstat->Speed,
				pDCTstat->MAload[dct], pDCTstat->DATAload[dct],
				&(pDCTstat->CH_ADDR_TMG[dct]), &(pDCTstat->CH_ODC_CTL[dct]),
				&pDCTstat->_2Tmode);

//	print_tx("1 CH_ODC_CTL: ", pDCTstat->CH_ODC_CTL[dct]);
//	print_tx("1 CH_ADDR_TMG: ", pDCTstat->CH_ADDR_TMG[dct]);

	if(pDCTstat->MAdimms[dct] == 1)
		pDCTstat->CH_ODC_CTL[dct] |= 0x20000000;	/* 75ohms */
	else
		pDCTstat->CH_ODC_CTL[dct] |= 0x10000000;	/* 150ohms */


	/*
	 * Overrides and/or workarounds
	 */
	pDCTstat->CH_ODC_CTL[dct] = procOdtWorkaround(pDCTstat, dct, pDCTstat->CH_ODC_CTL[dct]);

	print_tx("4 CH_ODC_CTL: ", pDCTstat->CH_ODC_CTL[dct]);
	print_tx("4 CH_ADDR_TMG: ", pDCTstat->CH_ADDR_TMG[dct]);
}

/*=============================================================================
 * Vendor is responsible for correct settings.
 * M2/Unbuffered 4 Slot - AMD Design Guideline.
 *=============================================================================
 * #1, BYTE, Speed (DCTStatstruc.Speed)
 * #2, BYTE, number of Address bus loads on the Channel.
 *     These must be listed in ascending order.
 *     FFh (-1) has special meaning of 'any', and must be listed first for
 *     each speed grade.
 * #3, DWORD, Address Timing Control Register Value
 * #4, DWORD, Output Driver Compensation Control Register Value
 */

static const u8 Table_ATC_ODC_D_Bx[] = {
	1, 0xFF, 0x00, 0x2F, 0x2F, 0x0, 0x22, 0x13, 0x11, 0x0,
	2,   12, 0x00, 0x2F, 0x2F, 0x0, 0x22, 0x13, 0x11, 0x0,
	2,   16, 0x00, 0x2F, 0x00, 0x0, 0x22, 0x13, 0x11, 0x0,
	2,   20, 0x00, 0x2F, 0x38, 0x0, 0x22, 0x13, 0x11, 0x0,
	2,   24, 0x00, 0x2F, 0x37, 0x0, 0x22, 0x13, 0x11, 0x0,
	2,   32, 0x00, 0x2F, 0x34, 0x0, 0x22, 0x13, 0x11, 0x0,
	3,   12, 0x20, 0x22, 0x20, 0x0, 0x22, 0x13, 0x11, 0x0,
	3,   16, 0x20, 0x22, 0x30, 0x0, 0x22, 0x13, 0x11, 0x0,
	3,   20, 0x20, 0x22, 0x2C, 0x0, 0x22, 0x13, 0x11, 0x0,
	3,   24, 0x20, 0x22, 0x2A, 0x0, 0x22, 0x13, 0x11, 0x0,
	3,   32, 0x20, 0x22, 0x2B, 0x0, 0x22, 0x13, 0x11, 0x0,
	4, 0xFF, 0x20, 0x25, 0x20, 0x0, 0x22, 0x33, 0x11, 0x0,
	5, 0xFF, 0x20, 0x20, 0x2F, 0x0, 0x22, 0x32, 0x11, 0x0,
	0xFF
};

static const u8 Table_ATC_ODC_D_Ax[] = {
	1, 0xFF, 0x00, 0x2F, 0x2F, 0x0, 0x22, 0x13, 0x11, 0x0,
	2,   12, 0x00, 0x2F, 0x2F, 0x0, 0x22, 0x13, 0x11, 0x0,
	2,   16, 0x00, 0x2F, 0x00, 0x0, 0x22, 0x13, 0x11, 0x0,
	2,   20, 0x00, 0x2F, 0x38, 0x0, 0x22, 0x13, 0x11, 0x0,
	2,   24, 0x00, 0x2F, 0x37, 0x0, 0x22, 0x13, 0x11, 0x0,
	2,   32, 0x00, 0x2F, 0x34, 0x0, 0x22, 0x13, 0x11, 0x0,
	3,   12, 0x20, 0x22, 0x20, 0x0, 0x22, 0x13, 0x11, 0x0,
	3,   16, 0x20, 0x22, 0x30, 0x0, 0x22, 0x13, 0x11, 0x0,
	3,   20, 0x20, 0x22, 0x2C, 0x0, 0x22, 0x13, 0x11, 0x0,
	3,   24, 0x20, 0x22, 0x2A, 0x0, 0x22, 0x13, 0x11, 0x0,
	3,   32, 0x20, 0x22, 0x2B, 0x0, 0x22, 0x13, 0x11, 0x0,
	4, 0xFF, 0x20, 0x25, 0x20, 0x0, 0x22, 0x33, 0x11, 0x0,
	5, 0xFF, 0x20, 0x20, 0x2F, 0x0, 0x22, 0x32, 0x11, 0x0,
	0xFF
};


static void Get_ChannelPS_Cfg0_D( u8 MAAdimms, u8 Speed, u8 MAAload,
				u8 DATAAload, u32 *AddrTmgCTL, u32 *ODC_CTL,
				u8 *CMDmode)
{
	u8 const *p;

	*AddrTmgCTL = 0;
	*ODC_CTL = 0;
	*CMDmode = 1;

	// FIXME: add Ax support
	if(MAAdimms == 0) {
		*ODC_CTL = 0x00111222;
		if(Speed == 3)
			*AddrTmgCTL = 0x00202220;
		else if (Speed == 2)
			*AddrTmgCTL = 0x002F2F00;
		else if (Speed == 1)
			*AddrTmgCTL = 0x002F2F00;
		else if (Speed == 4)
			*AddrTmgCTL = 0x00202520;
		else if (Speed == 5)
			*AddrTmgCTL = 0x002F2020;
		else
			*AddrTmgCTL = 0x002F2F2F;
	} else if(MAAdimms == 1) {
		if(Speed == 4) {
			*CMDmode = 2;
			*AddrTmgCTL = 0x00202520;
			*ODC_CTL = 0x00113222;
		} else if(Speed == 5) {
			*CMDmode = 2;
			*AddrTmgCTL = 0x002F2020;
			*ODC_CTL = 0x00113222;
		} else {
			*CMDmode = 1;
			*ODC_CTL = 0x00111222;
			if(Speed == 3) {
				*AddrTmgCTL = 0x00202220;
			} else if(Speed == 2) {
				if (MAAload == 4)
					*AddrTmgCTL = 0x002B2F00;
				else if (MAAload == 16)
					*AddrTmgCTL = 0x002B2F00;
				else if (MAAload == 8)
					*AddrTmgCTL = 0x002F2F00;
				else
					*AddrTmgCTL = 0x002F2F00;
			} else if(Speed == 1) {
				*AddrTmgCTL = 0x002F2F00;
			} else if(Speed == 5) {
				*AddrTmgCTL = 0x002F2020;
			} else {
				*AddrTmgCTL = 0x002F2F2F;
			}
		}
	} else {
		*CMDmode = 2;
		p = Table_ATC_ODC_D_Bx;
	do {
		if(Speed == *p) {
			if(MAAload <= *(p+1)) {
				*AddrTmgCTL = stream_to_int(p+2);
				*ODC_CTL = stream_to_int(p+6);
				break;
			}
		}
		p+=10;
	} while (0xFF == *p);
	}
}
