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

#include <inttypes.h>
#include <console/console.h>
#include <string.h>
#include "mct_d.h"
#include "mct_d_gcc.h"

/* mct_SetDramConfigMisc2_Cx & mct_SetDramConfigMisc2_Dx */
u32 mct_SetDramConfigMisc2(struct DCTStatStruc *pDCTstat,
				uint8_t dct, uint32_t misc2, uint32_t DramControl)
{
	u32 val;

	uint8_t MaxDimmsInstallable = mctGet_NVbits(NV_MAX_DIMMS_PER_CH);

	if (pDCTstat->LogicalCPUID & AMD_FAM15_ALL) {
		uint8_t cs_mux_45;
		uint8_t cs_mux_67;
		uint32_t f2x80;

		misc2 &= ~(0x1 << 28);			/* FastSelfRefEntryDis = 0x0 */
		if (MaxDimmsInstallable == 3) {
			/* FIXME 3 DIMMS per channel unimplemented */
			cs_mux_45 = 0;
		} else {
			uint32_t f2x60 = Get_NB32_DCT(pDCTstat->dev_dct, dct, 0x60);
			f2x80 = Get_NB32_DCT(pDCTstat->dev_dct, dct, 0x80);
			if ((((f2x80 & 0xf) == 0x7) || ((f2x80 & 0xf) == 0x9))
				&& ((f2x60 & 0x3) == 0x3))
				cs_mux_45 = 1;
			else if ((((f2x80 & 0xf) == 0xa) || ((f2x80 & 0xf) == 0xb))
				&& ((f2x60 & 0x3) > 0x1))
				cs_mux_45 = 1;
			else
				cs_mux_45 = 0;
		}

		if (MaxDimmsInstallable == 1) {
			cs_mux_67 = 0;
		} else if (MaxDimmsInstallable == 2) {
			uint32_t f2x64 = Get_NB32_DCT(pDCTstat->dev_dct, dct, 0x64);
			f2x80 = Get_NB32_DCT(pDCTstat->dev_dct, dct, 0x80);
			if (((((f2x80 >> 4) & 0xf) == 0x7) || (((f2x80 >> 4) & 0xf) == 0x9))
				&& ((f2x64 & 0x3) == 0x3))
				cs_mux_67 = 1;
			else if (((((f2x80 >> 4) & 0xf) == 0xa) || (((f2x80 >> 4) & 0xf) == 0xb))
				&& ((f2x64 & 0x3) > 0x1))
				cs_mux_67 = 1;
			else
				cs_mux_67 = 0;
		} else {
			/* FIXME 3 DIMMS per channel unimplemented */
			cs_mux_67 = 0;
		}

		misc2 &= ~(0x1 << 27);		/* CsMux67 = cs_mux_67 */
		misc2 |= ((cs_mux_67 & 0x1) << 27);
		misc2 &= ~(0x1 << 26);		/* CsMux45 = cs_mux_45 */
		misc2 |= ((cs_mux_45 & 0x1) << 26);
	} else if (pDCTstat->LogicalCPUID & (AMD_DR_Dx | AMD_DR_Cx)) {
		if (pDCTstat->Status & (1 << SB_Registered)) {
			misc2 |= 1 << SubMemclkRegDly;
			if (mctGet_NVbits(NV_MAX_DIMMS) == 8)
				misc2 |= 1 << Ddr3FourSocketCh;
			else
				misc2 &= ~(1 << Ddr3FourSocketCh);
		}

		if (pDCTstat->LogicalCPUID & AMD_DR_Cx)
			misc2 |= 1 << OdtSwizzle;

		val = DramControl;
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
