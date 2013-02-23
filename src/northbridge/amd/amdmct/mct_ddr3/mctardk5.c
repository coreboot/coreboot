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

static void Get_ChannelPS_Cfg0_D(u8 MAAdimms, u8 Speed, u8 MAAload,
				u32 *AddrTmgCTL, u32 *ODC_CTL,
				u8 *CMDmode);

void mctGet_PS_Cfg_D(struct MCTStatStruc *pMCTstat,
			 struct DCTStatStruc *pDCTstat, u32 dct)
{
	Get_ChannelPS_Cfg0_D(pDCTstat->MAdimms[dct], pDCTstat->Speed,
				pDCTstat->MAload[dct],
				&(pDCTstat->CH_ADDR_TMG[dct]), &(pDCTstat->CH_ODC_CTL[dct]),
				&pDCTstat->_2Tmode);

	pDCTstat->CH_EccDQSLike[0]  = 0x0403;
	pDCTstat->CH_EccDQSScale[0] = 0x70;
	pDCTstat->CH_EccDQSLike[1]  = 0x0403;
	pDCTstat->CH_EccDQSScale[1] = 0x70;

	pDCTstat->CH_ODC_CTL[dct] |= 0x20000000;	/* 60ohms */
}

/*
 *  In: MAAdimms   - number of DIMMs on the channel
 *    : Speed      - Speed (see DCTStatstruc.Speed for definition)
 *    : MAAload    - number of address bus loads on the channel
 * Out: AddrTmgCTL - Address Timing Control Register Value
 *    : ODC_CTL    - Output Driver Compensation Control Register Value
 *    : CMDmode    - CMD mode
 */
static void Get_ChannelPS_Cfg0_D( u8 MAAdimms, u8 Speed, u8 MAAload,
				u32 *AddrTmgCTL, u32 *ODC_CTL,
				u8 *CMDmode)
{
	*AddrTmgCTL = 0;
	*ODC_CTL = 0;
	*CMDmode = 1;

	if(MAAdimms == 1) {
		if(MAAload >= 16) {
			if(Speed == 4)
				*AddrTmgCTL = 0x003B0000;
			else if (Speed == 5)
				*AddrTmgCTL = 0x00380000;
			else if (Speed == 6)
				*AddrTmgCTL = 0x00360000;
			else
				*AddrTmgCTL = 0x00340000;
		} else {
			*AddrTmgCTL = 0x00000000;
		}
		*ODC_CTL = 0x00113222;
		*CMDmode = 1;
	} else /* if(MAAdimms == 0) */ {
		if(Speed == 4) {
			*CMDmode = 1;
			*AddrTmgCTL = 0x00390039;
		} else if(Speed == 5) {
			*CMDmode = 1;
			*AddrTmgCTL = 0x00350037;
		} else if(Speed == 6) {
			*CMDmode = 2;
			*AddrTmgCTL = 0x00000035;
		} else {
			*CMDmode = 2;
			*AddrTmgCTL = 0x00000033;
		}
		*ODC_CTL = 0x00223323;
	}
}
