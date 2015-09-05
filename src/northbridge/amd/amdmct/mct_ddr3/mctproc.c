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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

/* mct_SetDramConfigMisc2_Cx & mct_SetDramConfigMisc2_Dx */
u32 mct_SetDramConfigMisc2(struct DCTStatStruc *pDCTstat, u8 dct, u32 misc2)
{
	u32 val;

	if (pDCTstat->LogicalCPUID & (AMD_DR_Dx | AMD_DR_Cx)) {
		if (pDCTstat->Status & (1 << SB_Registered)) {
			misc2 |= 1 << SubMemclkRegDly;
			if (mctGet_NVbits(NV_MAX_DIMMS) == 8)
				misc2 |= 1 << Ddr3FourSocketCh;
			else
				misc2 &= ~(1 << Ddr3FourSocketCh);
		}

		if (pDCTstat->LogicalCPUID & AMD_DR_Cx)
			misc2 |= 1 << OdtSwizzle;
		val = Get_NB32(pDCTstat->dev_dct, dct * 0x100 + 0x78);

		val &= 7;
		val = ((~val) & 0xff) + 1;
		val += 6;
		val &= 0x7;
		misc2 &= 0xfff8ffff;
		misc2 |= val << 16;	/* DataTxFifoWrDly */
		if (pDCTstat->LogicalCPUID & AMD_DR_Dx)
			misc2 |= 1 << 7; /* ProgOdtEn */
	}
	return misc2;
}

void mct_ExtMCTConfig_Cx(struct DCTStatStruc *pDCTstat)
{
	u32 val;

	if (pDCTstat->LogicalCPUID & (AMD_DR_Cx)) {
		/* Revision C */
		Set_NB32(pDCTstat->dev_dct, 0x11c, 0x0ce00fc0 | 1 << 29/* FlushWrOnStpGnt */);

		val = Get_NB32(pDCTstat->dev_dct, 0x1b0);
		val &= ~0x73f;
		val |= 0x101;	/* BKDG recommended settings */

		Set_NB32(pDCTstat->dev_dct, 0x1b0, val);
	}
}
