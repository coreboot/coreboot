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

void mct_DramInit_Hw_D(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, u8 dct)
{
	u32 val;
	u32 reg;
	u32 dev = pDCTstat->dev_dct;

	/*flag for selecting HW/SW DRAM Init HW DRAM Init */
	reg = 0x90; /*DRAM Configuration Low */
	val = Get_NB32_DCT(dev, dct, reg);
	val |= (1<<InitDram);
	Set_NB32_DCT(dev, dct, reg, val);
}
