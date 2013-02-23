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


void mct_DramInit_Hw_D(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, u8 dct)
{
	u32 val;
	u32 reg;
	u32 dev = pDCTstat->dev_dct;

	/*flag for selecting HW/SW DRAM Init HW DRAM Init */
	reg = 0x90 + 0x100 * dct; /*DRAM Configuration Low */
	val = Get_NB32(dev, reg);
	val |= (1<<InitDram);
	Set_NB32(dev, reg, val);
}
