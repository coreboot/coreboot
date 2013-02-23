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

void mct_BeforeDQSTrainSamp(struct DCTStatStruc *pDCTstat)
{
	u32 val;

	if (pDCTstat->LogicalCPUID & AMD_DR_Bx) {
		Set_NB32(pDCTstat->dev_dct, 0x98, 0x0D004007);
		val = Get_NB32(pDCTstat->dev_dct, 0x9C);
		val |= 0x3FF;
		Set_NB32(pDCTstat->dev_dct, 0x9C, val);
		Set_NB32(pDCTstat->dev_dct, 0x98, 0x4D0F4F07);

		Set_NB32(pDCTstat->dev_dct, 0x198, 0x0D004007);
		val = Get_NB32(pDCTstat->dev_dct, 0x19C);
		val |= 0x3FF;
		Set_NB32(pDCTstat->dev_dct, 0x19C, val);
		Set_NB32(pDCTstat->dev_dct, 0x198, 0x4D0F4F07);
	}
}

void mct_ExtMCTConfig_Bx(struct DCTStatStruc *pDCTstat)
{
	if (pDCTstat->LogicalCPUID & (AMD_DR_Bx)) {
		Set_NB32(pDCTstat->dev_dct, 0x11C, 0x0FE40FC0 | 1 << 29/* FlushWrOnStpGnt */);
	}
}
