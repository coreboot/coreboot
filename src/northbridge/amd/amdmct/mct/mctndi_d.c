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



void InterleaveNodes_D(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstatA)
{

	/* Applies Node memory interleaving if enabled and if all criteria are met. */
	u8 Node;
	u32 Base;
	u32 MemSize, MemSize0 = 0;
	u32 Dct0MemSize = 0, DctSelBase, DctSelBaseOffset;
	u8 Nodes;
	u8 NodesWmem;
	u8 DoIntlv;
	u8 _NdIntCap;
	u8 _SWHole;
	u32 HWHoleSz;
	u32 DramHoleAddrReg;
	u32 HoleBase;
	u32 dev0;
	u32 reg0;
	u32 val;
	u8 i;
	struct DCTStatStruc *pDCTstat;

	DoIntlv = mctGet_NVbits(NV_NodeIntlv);

	_NdIntCap = 0;
	HWHoleSz = 0;	/*For HW remapping, NOT Node hoisting. */

	pDCTstat = pDCTstatA + 0;
	dev0 = pDCTstat->dev_host;
	Nodes = ((Get_NB32(dev0, 0x60) >> 4) & 0x7) + 1;


	dev0 = pDCTstat->dev_map;
	reg0 = 0x40;

	NodesWmem = 0;
	Node = 0;

	while (DoIntlv && (Node < Nodes)) {
		pDCTstat = pDCTstatA + Node;
		if (pMCTstat->GStatus & (1 << GSB_SpIntRemapHole)) {
			pMCTstat->GStatus |= 1 << GSB_HWHole;
			_SWHole = 0;
		} else if (pDCTstat->Status & (1 << SB_SWNodeHole)) {
			_SWHole = 1;
		} else {
			_SWHole = 0;
		}

		if(!_SWHole) {
			Base = Get_NB32(dev0, reg0);
			if (Base & 1) {
				NodesWmem++;
				Base &= 0xFFFF0000;	/* Base[39:8] */

				if (pDCTstat->Status & (1 << SB_HWHole )) {

					/* to get true amount of dram,
					 * subtract out memory hole if HW dram remapping */
					DramHoleAddrReg = Get_NB32(pDCTstat->dev_map, 0xF0);
					HWHoleSz = DramHoleAddrReg >> 16;
					HWHoleSz = (((~HWHoleSz) + 1) & 0xFF);
					HWHoleSz <<= 24-8;
				}
				/* check to see if the amount of memory on each channel
				 * are the same on all nodes */

				DctSelBase = Get_NB32(pDCTstat->dev_dct, 0x114);
				if(DctSelBase) {
					DctSelBase <<= 8;
					if ( pDCTstat->Status & (1 << SB_HWHole)) {
						if (DctSelBase >= 0x1000000) {
							DctSelBase -= HWHoleSz;
						}
					}
					DctSelBaseOffset -= Base;
					if (Node == 0) {
						Dct0MemSize = DctSelBase;
					} else if (DctSelBase != Dct0MemSize) {
						break;
					}
				}

				MemSize = Get_NB32(dev0, reg0 + 4);
				MemSize &= 0xFFFF0000;
				MemSize += 0x00010000;
				MemSize -= Base;
				if ( pDCTstat->Status & (1 << SB_HWHole)) {
					MemSize -= HWHoleSz;
				}
				if (Node == 0) {
					MemSize0 = MemSize;
				} else if (MemSize0 != MemSize) {
					break;
				}
			} else {
				break;
			}
		} else {
			break;
		}
	Node++;
	reg0 += 8;
	}

	if (Node == Nodes) {
		/* if all nodes have memory and no Node had SW memhole */
		if (Nodes == 2 || Nodes == 4 || Nodes == 8)
			_NdIntCap = 1;
	}

	if (!_NdIntCap)
		DoIntlv = 0;


	if (pMCTstat->GStatus & 1 << (GSB_SpIntRemapHole)) {
		HWHoleSz = pMCTstat->HoleBase;
		if (HWHoleSz == 0) {
			HWHoleSz = mctGet_NVbits(NV_BottomIO) & 0xFF;
			HWHoleSz <<= 24-8;
		}
		HWHoleSz = ((~HWHoleSz) + 1) & 0x00FF0000;
	}

	if (DoIntlv) {
		MCTMemClr_D(pMCTstat,pDCTstatA);
		/* Program Interleaving enabled on Node 0 map only.*/
		MemSize0 <<= bsf(Nodes);	/* MemSize=MemSize*2 (or 4, or 8) */
		Dct0MemSize <<= bsf(Nodes);
		MemSize0 += HWHoleSz;
		Base = ((Nodes - 1) << 8) | 3;
		reg0 = 0x40;
		Node = 0;
		while(Node < Nodes) {
			Set_NB32(dev0, reg0, Base);
			MemSize = MemSize0;
			MemSize--;
			MemSize &= 0xFFFF0000;
			MemSize |= Node << 8;	/* set IntlvSel[2:0] field */
			MemSize |= Node;	/* set DstNode[2:0] field */
			Set_NB32(dev0, reg0 + 4, MemSize0);
			reg0 += 8;
			Node++;
		}

		/*  set base/limit to F1x120/124 per Node */
		Node = 0;
		while(Node < Nodes) {
			pDCTstat = pDCTstatA + Node;
			pDCTstat->NodeSysBase = 0;
			MemSize = MemSize0;
			MemSize -= HWHoleSz;
			MemSize--;
			pDCTstat->NodeSysLimit = MemSize;
			Set_NB32(pDCTstat->dev_map, 0x120, Node << 21);
			MemSize = MemSize0;
			MemSize--;
			MemSize >>= 19;
			val = Base;
			val &= 0x700;
			val <<= 13;
			val |= MemSize;
			Set_NB32(pDCTstat->dev_map, 0x124, val);

			if (pMCTstat->GStatus & (1 << GSB_HWHole)) {
				HoleBase = pMCTstat->HoleBase;
				if (Dct0MemSize >= HoleBase) {
					val = HWHoleSz;
					if( Node == 0) {
						val += Dct0MemSize;
					}
				} else {
					val = HWHoleSz + Dct0MemSize;
				}

				val >>= 8;		/* DramHoleOffset */
				HoleBase <<= 8;		/* DramHoleBase */
				val |= HoleBase;
				val |= 1 << DramMemHoistValid;
				val |= 1 << DramHoleValid;
				Set_NB32(pDCTstat->dev_map, 0xF0, val);
			}


			Set_NB32(pDCTstat->dev_dct, 0x114, Dct0MemSize >> 8);	/* DctSelBaseOffset */
			val = Get_NB32(pDCTstat->dev_dct, 0x110);
			val &= 0x7FF;
			val |= Dct0MemSize >> 8;
			Set_NB32(pDCTstat->dev_dct, 0x110, val);	/* DctSelBaseAddr */
			print_tx("InterleaveNodes: DRAM Controller Select Low Register = ", val);
			Node++;
		}


		/* Copy Node 0 into other Nodes' CSRs */
		Node = 1;
		while (Node < Nodes) {
			pDCTstat = pDCTstatA + Node;

			for (i = 0x40; i <= 0x80; i++) {
				val = Get_NB32(dev0, i);
				Set_NB32(pDCTstat->dev_map, i, val);
			}

			val = Get_NB32(dev0, 0xF0);
			Set_NB32(pDCTstat->dev_map, 0xF0, val);
			Node++;
		}
		pMCTstat->GStatus = (1 << GSB_NodeIntlv);
	}
	print_tx("InterleaveNodes_D: Status ", pDCTstat->Status);
	print_tx("InterleaveNodes_D: ErrStatus ", pDCTstat->ErrStatus);
	print_tx("InterleaveNodes_D: ErrCode ", pDCTstat->ErrCode);
	print_t("InterleaveNodes_D: Done\n");
}
