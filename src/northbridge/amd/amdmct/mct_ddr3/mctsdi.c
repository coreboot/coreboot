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

static void mct_DramControlReg_Init_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct);

static void mct_DCTAccessDone(struct DCTStatStruc *pDCTstat, u8 dct)
{
	u32 reg_off = 0x100 * dct;
	u32 dev = pDCTstat->dev_dct;
	u32 val;

	do {
		val = Get_NB32(dev, reg_off + 0x98);
	} while (!(val & (1 << DctAccessDone)));
}

static u32 swapAddrBits(struct DCTStatStruc *pDCTstat, u32 MR_register_setting, u8 MrsChipSel, u8 dct)
{
	u16 word;
	u32 ret;

	if (!(pDCTstat->Status & (1 << SB_Registered))) {
		word = pDCTstat->MirrPresU_NumRegR;
		if (dct == 0) {
			word &= 0x55;
			word <<= 1;
		} else
			word &= 0xAA;

		if (word & (1 << MrsChipSel)) {
			/* A3<->A4,A5<->A6,A7<->A8,BA0<->BA1 */
			ret = 0;
			if (MR_register_setting & (1 << 3)) ret |= 1 << 4;
			if (MR_register_setting & (1 << 4)) ret |= 1 << 3;
			if (MR_register_setting & (1 << 5)) ret |= 1 << 6;
			if (MR_register_setting & (1 << 6)) ret |= 1 << 5;
			if (MR_register_setting & (1 << 7)) ret |= 1 << 8;
			if (MR_register_setting & (1 << 8)) ret |= 1 << 7;
			if (MR_register_setting & (1 << 16)) ret |= 1 << 17;
			if (MR_register_setting & (1 << 17)) ret |= 1 << 16;
			MR_register_setting &= ~0x301f8;
			MR_register_setting |= ret;
		}
	}
	return MR_register_setting;
}

static void mct_SendMrsCmd(struct DCTStatStruc *pDCTstat, u8 dct, u32 EMRS)
{
	u32 reg_off = 0x100 * dct;
	u32 dev = pDCTstat->dev_dct;
	u32 val;

	val = Get_NB32(dev, reg_off + 0x7C);
	val &= ~0xFFFFFF;
	val |= EMRS;
	val |= 1 << SendMrsCmd;
	Set_NB32(dev, reg_off + 0x7C, val);

	do {
		val = Get_NB32(dev, reg_off + 0x7C);
	} while (val & (1 << SendMrsCmd));
}

static u32 mct_MR2(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct, u32 MrsChipSel)
{
	u32 reg_off = 0x100 * dct;
	u32 dev = pDCTstat->dev_dct;
	u32 dword, ret;

	ret = 0x20000;
	ret |= MrsChipSel;

	/* program MrsAddress[5:3]=CAS write latency (CWL):
	 * based on F2x[1,0]84[Tcwl] */
	dword = Get_NB32(dev, reg_off + 0x84);
	dword = mct_AdjustSPDTimings(pMCTstat, pDCTstat, dword);

	ret |= ((dword >> 20) & 7) << 3;

	/* program MrsAddress[6]=auto self refresh method (ASR):
	   based on F2x[1,0]84[ASR]
	   program MrsAddress[7]=self refresh temperature range (SRT):
	   based on F2x[1,0]84[ASR and SRT] */
	ret |= ((dword >> 18) & 3) << 6;

	/* program MrsAddress[10:9]=dynamic termination during writes (RTT_WR)
	   based on F2x[1,0]84[DramTermDyn] */
	ret |= ((dword >> 10) & 3) << 9;

	return ret;
}

static u32 mct_MR3(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct, u32 MrsChipSel)
{
	u32 reg_off = 0x100 * dct;
	u32 dev = pDCTstat->dev_dct;
	u32 dword, ret;

	ret = 0x30000;
	ret |= MrsChipSel;

	/* program MrsAddress[1:0]=multi purpose register address location
	   (MPR Location):based on F2x[1,0]84[MprLoc]
	   program MrsAddress[2]=multi purpose register
	   (MPR):based on F2x[1,0]84[MprEn]
	*/
	dword = Get_NB32(dev, reg_off + 0x84);
	ret |= (dword >> 24) & 7;

	return ret;
}

static u32 mct_MR1(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct, u32 MrsChipSel)
{
	u32 reg_off = 0x100 * dct;
	u32 dev = pDCTstat->dev_dct;
	u32 dword, ret;

	ret = 0x10000;
	ret |= MrsChipSel;

	/* program MrsAddress[5,1]=output driver impedance control (DIC):
	 * based on F2x[1,0]84[DrvImpCtrl] */
	dword = Get_NB32(dev, reg_off + 0x84);
	if (dword & (1 << 3))
		ret |= 1 << 5;
	if (dword & (1 << 2))
		ret |= 1 << 1;

	/* program MrsAddress[9,6,2]=nominal termination resistance of ODT (RTT):
	   based on F2x[1,0]84[DramTerm] */
	if (!(pDCTstat->Status & (1 << SB_Registered))) {
		if (dword & (1 << 9))
			ret |= 1 << 9;
		if (dword & (1 << 8))
			ret |= 1 << 6;
		if (dword & (1 << 7))
			ret |= 1 << 2;
	} else {
		ret |= mct_MR1Odt_RDimm(pMCTstat, pDCTstat, dct, MrsChipSel);
	}

	/* program MrsAddress[11]=TDQS: based on F2x[1,0]94[RDqsEn] */
	if (Get_NB32(dev, reg_off + 0x94) & (1 << RDqsEn)) {
		u8 bit;
		/* Set TDQS=1b for x8 DIMM, TDQS=0b for x4 DIMM, when mixed x8 & x4 */
		bit = (ret >> 21) << 1;
		if ((dct & 1) != 0)
			bit ++;
		if (pDCTstat->Dimmx8Present & (1 << bit))
			ret |= 1 << 11;
	}

	if (dword & (1 << 13))
		ret |= 1 << 12;

	return ret;
}

static u32 mct_MR0(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct, u32 MrsChipSel)
{
	u32 reg_off = 0x100 * dct;
	u32 dev = pDCTstat->dev_dct;
	u32 dword, ret, dword2;

	ret = 0x00000;
	ret |= MrsChipSel;

	/* program MrsAddress[1:0]=burst length and control method
	   (BL):based on F2x[1,0]84[BurstCtrl] */
	dword = Get_NB32(dev, reg_off + 0x84);
	ret |= dword & 3;

	/* program MrsAddress[3]=1 (BT):interleaved */
	ret |= 1 << 3;

	/* program MrsAddress[6:4,2]=read CAS latency
	   (CL):based on F2x[1,0]88[Tcl] */
	dword2 = Get_NB32(dev, reg_off + 0x88);
	ret |= (dword2 & 0xF) << 4; /* F2x88[3:0] to MrsAddress[6:4,2]=xxx0b */

	/* program MrsAddress[12]=0 (PPD):slow exit */
	if (dword & (1 << 23))
		ret |= 1 << 12;

	/* program MrsAddress[11:9]=write recovery for auto-precharge
	   (WR):based on F2x[1,0]84[Twr] */
	ret |= ((dword >> 4) & 7) << 9;

	/* program MrsAddress[8]=1 (DLL):DLL reset
	   just issue DLL reset at first time */
	ret |= 1 << 8;

	return ret;
}

static void mct_SendZQCmd(struct DCTStatStruc *pDCTstat, u8 dct)
{
	u32 reg_off = 0x100 * dct;
	u32 dev = pDCTstat->dev_dct;
	u32 dword;

	/*1.Program MrsAddress[10]=1
	  2.Set SendZQCmd=1
	 */
	dword = Get_NB32(dev, reg_off + 0x7C);
	dword &= ~0xFFFFFF;
	dword |= 1 << 10;
	dword |= 1 << SendZQCmd;
	Set_NB32(dev, reg_off + 0x7C, dword);

	/* Wait for SendZQCmd=0 */
	do {
		dword = Get_NB32(dev, reg_off + 0x7C);
	} while (dword & (1 << SendZQCmd));

	/* 4.Wait 512 MEMCLKs */
	mct_Wait(300);
}

void mct_DramInit_Sw_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct)
{
	u8 MrsChipSel;
	u32 dword;
	u32 reg_off = 0x100 * dct;
	u32 dev = pDCTstat->dev_dct;

	if (pDCTstat->DIMMAutoSpeed == 4) {
		/* 3.Program F2x[1,0]7C[EnDramInit]=1 */
		dword = Get_NB32(dev, reg_off + 0x7C);
		dword |= 1 << EnDramInit;
		Set_NB32(dev, reg_off + 0x7C, dword);
		mct_DCTAccessDone(pDCTstat, dct);

		/* 4.wait 200us */
		mct_Wait(40000);

		/* 5.On revision C processors, program F2x[1, 0]7C[DeassertMemRstX] = 1. */
		dword = Get_NB32(dev, reg_off + 0x7C);
		dword |= 1 << DeassertMemRstX;
		Set_NB32(dev, reg_off + 0x7C, dword);

		/* 6.wait 500us */
		mct_Wait(200000);

		/* 7.Program F2x[1,0]7C[AssertCke]=1 */
		dword = Get_NB32(dev, reg_off + 0x7C);
		dword |= 1 << AssertCke;
		Set_NB32(dev, reg_off + 0x7C, dword);

		/* 8.wait 360ns */
		mct_Wait(80);

		/* The following steps are performed with registered DIMMs only and
		 * must be done for each chip select pair */
		if (pDCTstat->Status & (1 << SB_Registered))
			mct_DramControlReg_Init_D(pMCTstat, pDCTstat, dct);
	}

	/* The following steps are performed once for unbuffered DIMMs and once for each
	 * chip select on registered DIMMs: */
	for (MrsChipSel = 0; MrsChipSel < 8; MrsChipSel++) {
		if (pDCTstat->CSPresent & (1 << MrsChipSel)) {
			u32 EMRS;
			/* 13.Send EMRS(2) */
			EMRS = mct_MR2(pMCTstat, pDCTstat, dct, MrsChipSel << 20);
			EMRS = swapAddrBits(pDCTstat, EMRS, MrsChipSel, dct);
			mct_SendMrsCmd(pDCTstat, dct, EMRS);
			/* 14.Send EMRS(3). Ordinarily at this time, MrsAddress[2:0]=000b */
			EMRS= mct_MR3(pMCTstat, pDCTstat, dct, MrsChipSel << 20);
			EMRS = swapAddrBits(pDCTstat, EMRS, MrsChipSel, dct);
			mct_SendMrsCmd(pDCTstat, dct, EMRS);
			/* 15.Send EMRS(1) */
			EMRS= mct_MR1(pMCTstat, pDCTstat, dct, MrsChipSel << 20);
			EMRS = swapAddrBits(pDCTstat, EMRS, MrsChipSel, dct);
			mct_SendMrsCmd(pDCTstat, dct, EMRS);
			/* 16.Send MRS with MrsAddress[8]=1(reset the DLL) */
			EMRS= mct_MR0(pMCTstat, pDCTstat, dct, MrsChipSel << 20);
			EMRS = swapAddrBits(pDCTstat, EMRS, MrsChipSel, dct);
			mct_SendMrsCmd(pDCTstat, dct, EMRS);

			if (pDCTstat->DIMMAutoSpeed == 4)
				if (!(pDCTstat->Status & (1 << SB_Registered)))
					break; /* For UDIMM, only send MR commands once per channel */
		}
		if (pDCTstat->LogicalCPUID & (AMD_DR_Bx/* | AMD_RB_C0 */)) /* TODO: We dont support RB_C0 now. need to be added and tested. */
			if (!(pDCTstat->Status & (1 << SB_Registered)))
				MrsChipSel ++;
	}

	mct_Wait(100000);

	if (pDCTstat->DIMMAutoSpeed == 4) {
		/* 17.Send two ZQCL commands */
		mct_SendZQCmd(pDCTstat, dct);
		mct_SendZQCmd(pDCTstat, dct);
		/* 18.Program F2x[1,0]7C[EnDramInit]=0 */
		dword = Get_NB32(dev, reg_off + 0x7C);
		dword &= ~(1 << EnDramInit);
		Set_NB32(dev, reg_off + 0x7C, dword);
		mct_DCTAccessDone(pDCTstat, dct);
	}
}
