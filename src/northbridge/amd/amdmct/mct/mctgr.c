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


static const u8 Tab_GRCLKDis[] = {	8,0,8,8,0,0,8,0 };


u32 mct_AdjustMemClkDis_GR(struct DCTStatStruc *pDCTstat, u32 dct,
				u32 DramTimingLo)
{
	/* Greayhound format -> Griffin format */
	u32 NewDramTimingLo;
	u32 dev = pDCTstat->dev_dct;
	u32 reg;
	u32 reg_off = 0x100 * dct;
	u32 val;
	int i;

	DramTimingLo = val;
	/* Dram Timing Low (owns Clock Enable bits) */
	NewDramTimingLo = Get_NB32(dev, 0x88 + reg_off);
	if(mctGet_NVbits(NV_AllMemClks)==0) {
		/*Special Jedec SPD diagnostic bit - "enable all clocks"*/
		if(!(pDCTstat->Status & (1<<SB_DiagClks))) {
			for(i=0; i<MAX_DIMMS_SUPPORTED; i++) {
				val = Tab_GRCLKDis[i];
				if(val<8) {
					if(!(pDCTstat->DIMMValidDCT[dct] & (1<<val))) {
						/* disable memclk */
						NewDramTimingLo |= (1<<(i+1));
					}
				}
			}
		}
	}
	DramTimingLo &= ~(0xff<<24);
	DramTimingLo |= NewDramTimingLo & (0xff<<24);
	DramTimingLo &= (0x4d<<24); /* FIXME - enable all MemClks for now */

	return DramTimingLo;
}


u32 mct_AdjustDramConfigLo_GR(struct DCTStatStruc *pDCTstat, u32 dct, u32 val)
{
	/* Greayhound format -> Griffin format */
	/*FIXME - BurstLength32 must be 0 when F3x44[DramEccEn]=1. */
/*
			; mov	cx,PA_NBMISC+44h	;MCA NB Configuration
			; call Get_NB32n_D
			; bt eax,22				;EccEn
			; .if(CARRY?)
				; btr eax,BurstLength32
			; .endif
*/
	return val;
}


void mct_AdjustMemHoist_GR(struct DCTStatStruc *pDCTstat, u32 base, u32 HoleSize)
{
	u32 val;
	if(base >= pDCTstat->DCTHoleBase) {
		u32 dev = pDCTstat->dev_dct;
		base += HoleSize;
		base >>= 27 - 8;
		val = Get_NB32(dev, 0x110);
		val &= ~(0xfff<<11);
		val |= (base & 0xfff)<<11;
		Set_NB32(dev, 0x110, val);
	}
}
