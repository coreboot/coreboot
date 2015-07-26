/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

static uint16_t memclk_to_freq(uint16_t memclk) {
	uint16_t fam10h_freq_tab[] = {0, 0, 0, 400, 533, 667, 800};
	uint16_t fam15h_freq_tab[] = {0, 0, 0, 0, 333, 0, 400, 0, 0, 0, 533, 0, 0, 0, 667, 0, 0, 0, 800, 0, 0, 0, 933};

	uint16_t mem_freq = 0;

	if (is_fam15h()) {
		if (memclk < 0x17) {
			mem_freq = fam15h_freq_tab[memclk];
		}
	} else {
		if ((memclk > 0x0) && (memclk < 0x8)) {
			mem_freq = fam10h_freq_tab[memclk - 1];
		}
	}

	return mem_freq;
}

static uint32_t rc_word_value_to_ctl_bits(uint32_t value) {
	return ((value >> 2) & 3) << 16 | ((value & 3) << 3);
}

static u32 mct_ControlRC(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, u32 MrsChipSel, u32 CtrlWordNum)
{
	u8 Dimms, DimmNum;
	u32 val;
	u32 dct = 0;
	uint8_t ddr_voltage_index;
	uint16_t mem_freq;
	uint8_t package_type = mctGet_NVbits(NV_PACK_TYPE);
	uint8_t MaxDimmsInstallable = mctGet_NVbits(NV_MAX_DIMMS_PER_CH);

	DimmNum = (MrsChipSel >> 20) & 0xFE;

	/* assume dct=0; */
	/* if (dct == 1) */
	/* DimmNum ++; */
	/* cl +=8; */

	mem_freq = memclk_to_freq(pDCTstat->DIMMAutoSpeed);

	if (pDCTstat->CSPresent_DCT[0] > 0) {
		dct = 0;
	} else if (pDCTstat->CSPresent_DCT[1] > 0 ) {
		dct = 1;
		DimmNum++;
	}
	Dimms = pDCTstat->MAdimms[dct];

	ddr_voltage_index = dct_ddr_voltage_index(pDCTstat, dct);

	val = 0;
	if (CtrlWordNum == 0)
		val = 0x2;
	else if (CtrlWordNum == 1) {
		if (!((pDCTstat->DimmDRPresent | pDCTstat->DimmQRPresent) & (1 << DimmNum)))
			val = 0xC; /* if single rank, set DBA1 and DBA0 */
	} else if (CtrlWordNum == 2) {
		if (package_type == PT_GR) {
			/* Socket G34 */
			if (MaxDimmsInstallable == 2) {
				if (Dimms > 1)
					val = 0x4;
			}
		}
	} else if (CtrlWordNum == 3) {
		val = (pDCTstat->CtrlWrd3 >> (DimmNum << 2)) & 0xFF;
	} else if (CtrlWordNum == 4) {
		val = (pDCTstat->CtrlWrd4 >> (DimmNum << 2)) & 0xFF;
	} else if (CtrlWordNum == 5) {
		val = (pDCTstat->CtrlWrd5 >> (DimmNum << 2)) & 0xFF;
	} else if (CtrlWordNum == 8) {
		if (package_type == PT_GR) {
			/* Socket G34 */
			if (MaxDimmsInstallable == 2) {
				val = 0x0;
			}
		}
	} else if (CtrlWordNum == 9) {
		val = 0xD;	/* DBA1, DBA0, DA3 = 0 */
	} else if (CtrlWordNum == 10) {
		val = 0x0;	/* Lowest operating frequency */
	} else if (CtrlWordNum == 11) {
		if (ddr_voltage_index & 0x4)
			val = 0x2;	/* 1.25V */
		else if (ddr_voltage_index & 0x2)
			val = 0x1;	/* 1.35V */
		else
			val = 0x0;	/* 1.5V */
	} else if (CtrlWordNum >= 12) {
		val = 0x0;	/* Unset */
	}
	val &= 0xf;

	printk(BIOS_SPEW, "Preparing to send DIMM RC%d: %02x\n", CtrlWordNum, val);

	val = MrsChipSel | rc_word_value_to_ctl_bits(val);

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
	uint8_t dct = 0;
	u32 dev = pDCTstat->dev_dct;

	if (pDCTstat->CSPresent_DCT[0] > 0) {
		dct = 0;
	} else if (pDCTstat->CSPresent_DCT[1] > 0 ){
		dct = 1;
	}

	val |= Get_NB32_DCT(dev, dct, 0x7C) & ~0xFFFFFF;
	val |= 1 << SendControlWord;
	Set_NB32_DCT(dev, dct, 0x7C, val);

	do {
		val = Get_NB32_DCT(dev, dct, 0x7C);
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
			val = Get_NB32_DCT(dev, dct, 0xa8);
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
			Set_NB32_DCT(dev, dct, 0xa8, val);

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
	uint16_t mem_freq;

	pDCTstat->DIMMAutoSpeed = pDCTstat->TargetFreq;
	mem_freq = memclk_to_freq(pDCTstat->TargetFreq);
	for (MrsChipSel=0; MrsChipSel < 8; MrsChipSel++, MrsChipSel++) {
		if (pDCTstat->CSPresent & (1 << MrsChipSel)) {
			/* 2. Program F2x[1, 0]A8[CtrlWordCS]=bit mask for target chip selects. */
			val = Get_NB32_DCT(dev, 0, 0xA8); /* TODO: dct 0 / 1 select */
			val &= ~(0xFF << 8);
			val |= (0x3 << (MrsChipSel & 0xFE)) << 8;
			Set_NB32_DCT(dev, 0, 0xA8, val); /* TODO: dct 0 / 1 select */

			/* Resend control word 10 */
			uint8_t freq_ctl_val = 0;
			mct_Wait(1600);
			switch (mem_freq) {
				case 333:
				case 400:
					freq_ctl_val = 0x0;
					break;
				case 533:
					freq_ctl_val = 0x1;
					break;
				case 667:
					freq_ctl_val = 0x2;
					break;
				case 800:
					freq_ctl_val = 0x3;
					break;
				case 933:
					freq_ctl_val = 0x4;
					break;
			}

			printk(BIOS_SPEW, "Preparing to send DIMM RC%d: %02x\n", 10, freq_ctl_val);

			mct_SendCtrlWrd(pMCTstat, pDCTstat, MrsChipSel << 20 | 0x40002 | rc_word_value_to_ctl_bits(freq_ctl_val));

			mct_Wait(1600);

			/* Resend control word 2 */
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
