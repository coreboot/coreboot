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

static uint8_t is_fam15h(void)
{
	uint8_t fam15h = 0;
	uint32_t family;

	family = cpuid_eax(0x80000001);
	family = ((family & 0xf00000) >> 16) | ((family & 0xf00) >> 8);

	if (family >= 0x6f)
		/* Family 15h or later */
		fam15h = 1;

	return fam15h;
}

u8 mct_checkNumberOfDqsRcvEn_1Pass(u8 pass)
{
	u8 ret = 1;

	if (is_fam15h()) {
		/* Fam15h needs two passes */
		ret = 1;
	} else {
		if (pass == SecondPass)
			ret = 0;
	}

	return ret;
}

u32 SetupDqsPattern_1PassA(u8 pass)
{
	return (u32) TestPattern1_D;
}

u32 SetupDqsPattern_1PassB(u8 pass)
{
	return (u32) TestPattern0_D;
}

static u16 mct_Average_RcvrEnDly_1Pass(struct DCTStatStruc *pDCTstat, u8 Channel, u8 Receiver,
					u8 Pass)
{
	u16 i, MaxValue;
	u16 *p;
	u16 val;

	MaxValue = 0;
	p = pDCTstat->CH_D_B_RCVRDLY[Channel][Receiver >> 1];

	for (i = 0; i < 8; i++) {
		/* get left value from DCTStatStruc.CHA_D0_B0_RCVRDLY*/
		val = p[i];
		/* get right value from DCTStatStruc.CHA_D0_B0_RCVRDLY_1*/
		val += Pass1MemClkDly;
		/* write back the value to stack */
		if (val > MaxValue)
			MaxValue = val;

		p[i] = val;
	}
	/* pDCTstat->DimmTrainFail &= ~(1<<Receiver+Channel); */

	return MaxValue;
}

u8 mct_SaveRcvEnDly_D_1Pass(struct DCTStatStruc *pDCTstat, u8 pass)
{
	u8 ret;
	ret = 0;
	if ((pDCTstat->DqsRcvEn_Pass == 0xff) && (pass== FirstPass))
		ret = 2;
	return ret;
}

u16 mct_Average_RcvrEnDly_Pass(struct DCTStatStruc *pDCTstat,
				u16 RcvrEnDly, u16 RcvrEnDlyLimit,
				u8 Channel, u8 Receiver, u8 Pass)

{
	return mct_Average_RcvrEnDly_1Pass(pDCTstat, Channel, Receiver, Pass);
}
