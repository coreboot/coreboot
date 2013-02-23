/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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


u8 mct_checkNumberOfDqsRcvEn_Pass(u8 pass)
{
	return 1;
}


u32 SetupDqsPattern_PassA(u8 Pass)
{
	u32 ret;
	if(Pass == FirstPass)
		ret = (u32) TestPattern1_D;
	else
		ret = (u32) TestPattern2_D;

	return ret;
}


u32 SetupDqsPattern_PassB(u8 Pass)
{
	u32 ret;
	if(Pass == FirstPass)
		ret = (u32) TestPattern0_D;
	else
		ret = (u32) TestPattern2_D;

	return ret;
}


u8 mct_Get_Start_RcvrEnDly_Pass(struct DCTStatStruc *pDCTstat,
					u8 Channel, u8 Receiver,
					u8 Pass)
{
	u8 RcvrEnDly;

	if (Pass == FirstPass)
		RcvrEnDly = 0;
	else {
		u8 max = 0;
		u8 val;
		u8 i;
		u8 *p = pDCTstat->CH_D_B_RCVRDLY[Channel][Receiver>>1];
		u8 bn;
		bn = 8;
//		print_tx("mct_Get_Start_RcvrEnDly_Pass: Channel:", Channel);
//		print_tx("mct_Get_Start_RcvrEnDly_Pass: Receiver:", Receiver);
		for ( i=0;i<bn; i++) {
			val  = p[i];
//			print_tx("mct_Get_Start_RcvrEnDly_Pass: i:", i);
//			print_tx("mct_Get_Start_RcvrEnDly_Pass: val:", val);
			if(val > max) {
				max = val;
			}
		}
		RcvrEnDly = max;
//		while(1) {; }
//		RcvrEnDly += secPassOffset; //FIXME Why
	}

	return RcvrEnDly;
}



u8 mct_Average_RcvrEnDly_Pass(struct DCTStatStruc *pDCTstat,
				u8 RcvrEnDly, u8 RcvrEnDlyLimit,
				u8 Channel, u8 Receiver, u8 Pass)
{
	u8 i;
	u8 *p;
	u8 *p_1;
	u8 val;
	u8 val_1;
	u8 valid = 1;
	u8 bn;

	bn = 8;

	p = pDCTstat->CH_D_B_RCVRDLY[Channel][Receiver>>1];

	if (Pass == SecondPass) { /* second pass must average values */
		//FIXME: which byte?
		p_1 = pDCTstat->B_RCVRDLY_1;
//		p_1 = pDCTstat->CH_D_B_RCVRDLY_1[Channel][Receiver>>1];
		for(i=0; i<bn; i++) {
			val = p[i];
			/* left edge */
			if (val != (RcvrEnDlyLimit - 1)) {
				val -= Pass1MemClkDly;
				val_1 = p_1[i];
				val += val_1;
				val >>= 1;
				p[i] = val;
			} else {
				valid = 0;
				break;
			}
		}
		if (!valid) {
			pDCTstat->ErrStatus |= 1<<SB_NORCVREN;
		} else {
			pDCTstat->DimmTrainFail &= ~(1<<(Receiver + Channel));
		}
	} else {
		for(i=0; i < bn; i++) {
			val = p[i];
			/* Add 1/2 Memlock delay */
			//val += Pass1MemClkDly;
			val += 0x5; // NOTE: middle value with DQSRCVEN_SAVED_GOOD_TIMES
			//val += 0x02;
			p[i] = val;
			pDCTstat->DimmTrainFail &= ~(1<<(Receiver + Channel));
		}
	}

	return RcvrEnDly;
}
