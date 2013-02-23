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

void InterleaveChannels_D(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstatA)
{

	u8 Node;
	u32 DramBase, DctSelBase;
	u8 DctSelIntLvAddr, DctSelHi;
	u8 HoleValid = 0;
	u32 HoleSize, HoleBase = 0;
	u32 val, tmp;
	u32 dct0_size, dct1_size;
	struct DCTStatStruc *pDCTstat;

	/* HoleValid - indicates whether the current Node contains hole.
	 * HoleSize - indicates whether there is IO hole in the whole system
	 * memory.
	 */

	/* call back to wrapper not needed ManualChannelInterleave_D(); */
	/* call back - DctSelIntLvAddr = mctGet_NVbits(NV_ChannelIntlv);*/	/* override interleave */
	/* Manually set: typ=5, otherwise typ=7. */
	DctSelIntLvAddr = mctGet_NVbits(NV_ChannelIntlv); /* typ=5: Hash*: exclusive OR of address bits[20:16, 6]. */

	if (DctSelIntLvAddr & 1) {
		DctSelIntLvAddr >>= 1;
		HoleSize = 0;
		if ((pMCTstat->GStatus & (1 << GSB_SoftHole)) ||
		     (pMCTstat->GStatus & (1 << GSB_HWHole))) {
			if (pMCTstat->HoleBase) {
				HoleBase = pMCTstat->HoleBase >> 8;
				HoleSize = HoleBase & 0xFFFF0000;
				HoleSize |= ((~HoleBase) + 1) & 0xFFFF;
			}
		}
		Node = 0;
		while (Node < MAX_NODES_SUPPORTED) {
			pDCTstat = pDCTstatA + Node;
			val = Get_NB32(pDCTstat->dev_map, 0xF0);
			if (val & (1 << DramHoleValid))
				HoleValid = 1;
			if (!pDCTstat->GangedMode && pDCTstat->DIMMValidDCT[0] && pDCTstat->DIMMValidDCT[1]) {
				DramBase = pDCTstat->NodeSysBase >> 8;
				dct1_size = ((pDCTstat->NodeSysLimit) + 2) >> 8;
				dct0_size = Get_NB32(pDCTstat->dev_dct, 0x114);
				if (dct0_size >= 0x10000) {
					dct0_size -= HoleSize;
				}

				dct0_size -= DramBase;
				dct1_size -= dct0_size;
				DctSelHi = 0x05;		/* DctSelHiRngEn = 1, DctSelHi = 0 */
				if (dct1_size == dct0_size) {
					dct1_size = 0;
					DctSelHi = 0x04;	/* DctSelHiRngEn = 0 */
				} else if (dct1_size > dct0_size ) {
					dct1_size = dct0_size;
					DctSelHi = 0x07;	/* DctSelHiRngEn = 1, DctSelHi = 1 */
				}
				dct0_size = dct1_size;
				dct0_size += DramBase;
				dct0_size += dct1_size;
				if (dct0_size >= HoleBase)	/* if DctSelBaseAddr > HoleBase */
					dct0_size += HoleSize;
				DctSelBase = dct0_size;

				if (dct1_size == 0)
					dct0_size = 0;
				dct0_size -= dct1_size;		/* DctSelBaseOffset = DctSelBaseAddr - Interleaved region */
				Set_NB32(pDCTstat->dev_dct, 0x114, dct0_size);

				if (dct1_size == 0)
					dct1_size = DctSelBase;
				val = Get_NB32(pDCTstat->dev_dct, 0x110);
				val &= 0x7F8;
				val |= dct1_size;
				val |= DctSelHi;
				val |= (DctSelIntLvAddr << 6) & 0xFF;
				Set_NB32(pDCTstat->dev_dct, 0x110, val);

				if (HoleValid) {
					tmp = DramBase;
					val = DctSelBase;
					if (val < HoleBase) {	/* DctSelBaseAddr < DramHoleBase */
						val -= DramBase;
						val >>= 1;
						tmp += val;
					}
					tmp += HoleSize;
					val = Get_NB32(pDCTstat->dev_map, 0xF0);	/* DramHoleOffset */
					val &= 0xFFFF007F;
					val |= (tmp & ~0xFFFF007F);
					Set_NB32(pDCTstat->dev_map, 0xF0, val);
				}
			}
			printk(BIOS_DEBUG, "InterleaveChannels_D: Node %x\n", Node);
			printk(BIOS_DEBUG, "InterleaveChannels_D: Status %x\n", pDCTstat->Status);
			printk(BIOS_DEBUG, "InterleaveChannels_D: ErrStatus %x\n", pDCTstat->ErrStatus);
			printk(BIOS_DEBUG, "InterleaveChannels_D: ErrCode %x\n", pDCTstat->ErrCode);
			Node++;
		}
	}
	printk(BIOS_DEBUG, "InterleaveChannels_D: Done\n\n");
}
