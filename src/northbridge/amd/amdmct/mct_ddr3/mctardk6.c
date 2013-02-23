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

/* The socket type F (1207), Fr2, G (1207) are not tested.
 */

static void Get_ChannelPS_Cfg0_D(u8 MAAdimms, u8 Speed, u8 MAAload,
				u8 DATAAload, u32 *AddrTmgCTL, u32 *ODC_CTL,
				u8 *CMDmode);


void mctGet_PS_Cfg_D(struct MCTStatStruc *pMCTstat,
			 struct DCTStatStruc *pDCTstat, u32 dct)
{
	Get_ChannelPS_Cfg0_D(pDCTstat->MAdimms[dct], pDCTstat->Speed,
				pDCTstat->MAload[dct], pDCTstat->DATAload[dct],
				&(pDCTstat->CH_ADDR_TMG[dct]), &(pDCTstat->CH_ODC_CTL[dct]),
				&pDCTstat->_2Tmode);

	if (pDCTstat->GangedMode == 1 && dct == 0)
		Get_ChannelPS_Cfg0_D(pDCTstat->MAdimms[1], pDCTstat->Speed,
				     pDCTstat->MAload[1], pDCTstat->DATAload[1],
				     &(pDCTstat->CH_ADDR_TMG[1]), &(pDCTstat->CH_ODC_CTL[1]),
				     &pDCTstat->_2Tmode);

	pDCTstat->CH_EccDQSLike[0]  = 0x0302;
	pDCTstat->CH_EccDQSLike[1]  = 0x0302;

}

/*
 *  In: MAAdimms   - number of DIMMs on the channel
 *    : Speed      - Speed (see DCTStatstruc.Speed for definition)
 *    : MAAload    - number of address bus loads on the channel
 *    : DATAAload  - number of ranks on the channel
 * Out: AddrTmgCTL - Address Timing Control Register Value
 *    : ODC_CTL    - Output Driver Compensation Control Register Value
 *    : CMDmode    - CMD mode
 */
static void Get_ChannelPS_Cfg0_D( u8 MAAdimms, u8 Speed, u8 MAAload,
				u8 DATAAload, u32 *AddrTmgCTL, u32 *ODC_CTL,
				u8 *CMDmode)
{
	*AddrTmgCTL = 0;
	*ODC_CTL = 0;
	*CMDmode = 1;

	if (mctGet_NVbits(NV_MAX_DIMMS) == 4) {
		if(Speed == 4) {
			*AddrTmgCTL = 0x00000000;
		} else if (Speed == 5) {
			*AddrTmgCTL = 0x003C3C3C;
			if (MAAdimms > 1)
				*AddrTmgCTL = 0x003A3C3A;
		} else if (Speed == 6) {
			if (MAAdimms == 1)
				*AddrTmgCTL = 0x003A3A3A;
			else
				*AddrTmgCTL = 0x00383A38;
		} else {
			if (MAAdimms == 1)
				*AddrTmgCTL = 0x00373937;
			else
				*AddrTmgCTL = 0x00353935;
		}
	}
	else {
		if(Speed == 4) {
			*AddrTmgCTL = 0x00000000;
			if (MAAdimms == 3)
				*AddrTmgCTL = 0x00380038;
		} else if (Speed == 5) {
			if (MAAdimms == 1)
				*AddrTmgCTL = 0x003C3C3C;
			else if (MAAdimms == 2)
				*AddrTmgCTL = 0x003A3C3A;
			else
				*AddrTmgCTL = 0x00373C37;
		} else if (Speed == 6) {
			if (MAAdimms == 1)
				*AddrTmgCTL = 0x003A3A3A;
			else if (MAAdimms == 2)
				*AddrTmgCTL = 0x00383A38;
			else
				*AddrTmgCTL = 0x00343A34;
		} else {
			if (MAAdimms == 1)
				*AddrTmgCTL = 0x00393939;
			else if (MAAdimms == 2)
				*AddrTmgCTL = 0x00363936;
			else
				*AddrTmgCTL = 0x00303930;
		}
	}

	if ((MAAdimms == 1) && (MAAload < 4))
		*ODC_CTL = 0x20113222;
	else
		*ODC_CTL = 0x20223222;

	*CMDmode = 1;
}
