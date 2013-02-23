/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 One Laptop per Child, Association, Inc.
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

void DRAMClearEndingAddress(DRAM_SYS_ATTR * DramAttr);

void DRAMSizingEachRank(DRAM_SYS_ATTR * DramAttr);

BOOLEAN DoDynamicSizing1XM(DRAM_SYS_ATTR * DramAttr,
			   u8 * nRA, u8 * nCA, u8 * nBS, u8 PhyRank);

void DRAMSetRankMAType(DRAM_SYS_ATTR * DramAttr);

void DRAMSetEndingAddress(DRAM_SYS_ATTR * DramAttr);

void DRAMPRToVRMapping(DRAM_SYS_ATTR * DramAttr);

/*===================================================================
Function   : DRAMBankInterleave()
Precondition :
Input        :
		   DramAttr: pointer point to  DRAMSYSATTR  which consist the DDR and Dimm information in MotherBoard
Output     :  Void
Purpose   :  STEP 13 Set Bank Interleave  VIANB3DRAMREG69[7:6] 00:No Interleave 01:2 Bank 10:4 Bank	11:8 Bank
                   Scan all DIMMs on board to find out the lowest Bank Interleave among these DIMMs and set register.
===================================================================*/
void DRAMBankInterleave(DRAM_SYS_ATTR * DramAttr)
{
	u8 Data, SpdBAData;
	DIMM_INFO *CurrentDimminfo;
	u8 Bank = 3, Shift, RankNO, Count;
	Shift = 1;
	for (RankNO = 0; RankNO < 4; RankNO += 2)	//all_even  0 RankNO 4 6
	{
		if ((DramAttr->RankPresentMap & Shift) != 0) {
			CurrentDimminfo = &(DramAttr->DimmInfo[RankNO >> 1]);	//this Rank in a dimm
			SpdBAData =
			    (u8) (CurrentDimminfo->SPDDataBuf
				  [SPD_SDRAM_NO_OF_BANKS]);
			if (SpdBAData == 4)
				Count = 2;
			else if (SpdBAData == 8)
				Count = 3;
			else
				Count = 0;
			if (Count < Bank)
				Bank = Count;
		}
		Shift <<= 2;
	}

	Data = pci_read_config8(MEMCTRL, 0x69);
	Data &= ~0xc0;
	Data |= (Bank << 6);
	pci_write_config8(MEMCTRL, 0x69, Data);

	if (DramAttr->DimmNumChB > 0) {
		CurrentDimminfo = &(DramAttr->DimmInfo[3]);	//this Rank in a dimm
		SpdBAData =
		    (u8) (CurrentDimminfo->SPDDataBuf[SPD_SDRAM_NO_OF_BANKS]);
		if (SpdBAData == 4)
			Bank = 2;
		else if (SpdBAData == 2)
			Bank = 1;
		else
			Bank = 0;
		pci_write_config8(MEMCTRL, 0x87, Bank);
	}
}

/*===================================================================
Function   : DRAMSizingMATypeM()
Precondition :
Input        :
		   DramAttr: pointer point to  DRAMSYSATTR  which consist the DDR and Dimm information in MotherBoard
Output     :  Void
 Purpose  : STEP 14  1 DRAM Sizing 2  Fill MA type 3 Prank to vrankMapping
===================================================================*/
void DRAMSizingMATypeM(DRAM_SYS_ATTR * DramAttr)
{
	DRAMClearEndingAddress(DramAttr);
	DRAMSizingEachRank(DramAttr);
	//DRAMReInitDIMMBL           (DramAttr);
	DRAMSetRankMAType(DramAttr);
	DRAMSetEndingAddress(DramAttr);
	DRAMPRToVRMapping(DramAttr);
}

/*===================================================================
Function   : DRAMClearEndingAddress()
Precondition :
Input        :
		   DramAttr: pointer point to  DRAMSYSATTR  which consist the DDR and Dimm information in MotherBoard
Output     : Void
Purpose   : clear Ending and Start adress from 0x40-4f to zero
===================================================================*/
void DRAMClearEndingAddress(DRAM_SYS_ATTR * DramAttr)
{
	u8 Data, Reg;
	Data = 0;
	for (Reg = 0x40; Reg <= 0x4f; Reg++) {
		pci_write_config8(MEMCTRL, Reg, Data);
	}
}

/*===================================================================
Function   : DRAMSizingEachRank()
Precondition :
Input        :
		   DramAttr: pointer point to  DRAMSYSATTR  which consist the DDR and Dimm information in MotherBoard
Output     : Void
Purpose   : Sizing each Rank invidually, by number of rows column banks pins, be care about 128bit
===================================================================*/
void DRAMSizingEachRank(DRAM_SYS_ATTR * DramAttr)
{
	u8 Slot, RankIndex, Rows, Columns, Banks;
	u32 Size;
	BOOLEAN HasThreeBitBA;
	u8 Data;

	HasThreeBitBA = FALSE;
	for (Slot = 0; Slot < 2; Slot++) {
		if (!DramAttr->DimmInfo[Slot].bPresence)
			continue;
		Rows = DramAttr->DimmInfo[Slot].SPDDataBuf[SPD_SDRAM_ROW_ADDR];
		Columns =
		    DramAttr->DimmInfo[Slot].SPDDataBuf[SPD_SDRAM_COL_ADDR];
		Banks = DramAttr->DimmInfo[Slot].SPDDataBuf[SPD_SDRAM_NO_OF_BANKS];	//this is Bank number not Bank address bit
		if (Banks == 4)
			Banks = 2;
		else if (Banks == 8)
			Banks = 3;
		else
			Banks = 0;
		Size = (u32) (1 << (Rows + Columns + Banks + 3));
		RankIndex = 2 * Slot;
		DramAttr->RankSize[RankIndex] = Size;
		//if this module have two ranks
		if ((DramAttr->
		     DimmInfo[Slot].SPDDataBuf[SPD_SDRAM_DIMM_RANKS] & 0x07) ==
		    0x01) {
			RankIndex++;
			DramAttr->RankSize[RankIndex] = Size;
		}

		PRINT_DEBUG_MEM("rows: ");
		PRINT_DEBUG_MEM_HEX8(Rows);
		PRINT_DEBUG_MEM(", columns:");
		PRINT_DEBUG_MEM_HEX8(Columns);
		PRINT_DEBUG_MEM(", banks:");
		PRINT_DEBUG_MEM_HEX8(Banks);
		PRINT_DEBUG_MEM("\r");

		if (Banks == 3)
			HasThreeBitBA = TRUE;
	}

	//must set BA2 enable if any 8-bank device exists
	if (HasThreeBitBA) {
		Data = pci_read_config8(MEMCTRL, 0x53);
		Data |= 0x80;
		pci_write_config8(MEMCTRL, 0x53, Data);
	}
#if 1
	for (RankIndex = 0; DramAttr->RankSize[RankIndex] != 0; RankIndex++) {
		PRINT_DEBUG_MEM("Rank:");
		PRINT_DEBUG_MEM_HEX8(RankIndex);
		PRINT_DEBUG_MEM(", Size:");
		PRINT_DEBUG_MEM_HEX32(DramAttr->RankSize[RankIndex] >> 20);
		PRINT_DEBUG_MEM("\r");
	}
#endif
}

/*===================================================================
Function   : DRAMSetRankMAType()
Precondition :
Input       :
		  DramAttr: pointer point to  DRAMSYSATTR  which consist the DDR and Dimm information in MotherBoard
Output     : Void
Purpose   : set the matype Reg by MAMapTypeTbl, which the rule can be found in memoryinit
===================================================================*/
void DRAMSetRankMAType(DRAM_SYS_ATTR * DramAttr)
{
	u8 SlotNum, Data, j, Reg, or, and;
	u8 ShiftBits[] = { 5, 1, 5, 1 };	/* Rank 0/1 MA Map Type is 7:5, Rank 2/3 MA Map Type is 3:1. See  Fun3Rx50. */
	u8 MAMapTypeTbl[] = {	/* Table 12 of P4M800 Pro DataSheet. */
		2, 9, 0,	/* Bank Address Bits, Column Address Bits, Rank MA Map Type */
		2, 10, 1,
		2, 11, 2,
		2, 12, 3,
		3, 10, 5,
		3, 11, 6,
		3, 12, 7,
		0, 0, 0
	};
	Data = pci_read_config8(MEMCTRL, 0x50);
	Data &= 0x1;
	pci_write_config8(MEMCTRL, 0x50, Data);
	// disable MA32/16 MA33/17 swap   in memory init it has this Reg fill
	Data = pci_read_config8(MEMCTRL, 0x6b);
	Data &= ~0x08;
	pci_write_config8(MEMCTRL, 0x6b, Data);

	Data = 0x00;
	for (SlotNum = 0; SlotNum < MAX_DIMMS; SlotNum++) {
		if (DramAttr->DimmInfo[SlotNum].bPresence) {
			for (j = 0; MAMapTypeTbl[j] != 0; j += 3) {
				if ((1 << MAMapTypeTbl[j]) ==
				    DramAttr->
				    DimmInfo[SlotNum].SPDDataBuf
				    [SPD_SDRAM_NO_OF_BANKS]
				    && MAMapTypeTbl[j + 1] ==
				    DramAttr->
				    DimmInfo[SlotNum].SPDDataBuf
				    [SPD_SDRAM_COL_ADDR]) {
					break;
				}
			}
			if (0 == MAMapTypeTbl[j]) {
				PRINT_DEBUG_MEM
				    ("UNSUPPORTED Bank, Row and Column Addr Bits!\r");
				return;
			}
			or = MAMapTypeTbl[j + 2] << ShiftBits[SlotNum];
			if (DramAttr->CmdRate == 1)
				or |= 0x01 << (ShiftBits[SlotNum] - 1);

			Reg = SlotNum / 2;
			if ((SlotNum & 0x01) == 0x01) {
				and = 0xf1;	// BUGBUG: it should be 0xf0
			} else {
				and = 0x1f;	// BUGBUG: it should be 0x0f
			}
			Data = pci_read_config8(MEMCTRL, 0x50 + Reg);
			Data &= and;
			Data |= or;
			pci_write_config8(MEMCTRL, 0x50 + Reg, Data);
		}
	}
	//may have some Reg filling at add 3-52 11 and 3-53   in his function
}

/*===================================================================
Function   : DRAMSetEndingAddress()
Precondition :
Input      :
		  DramAttr: pointer point to  DRAMSYSATTR  which consist the DDR and Dimm information in MotherBoard
Output     : Void
Purpose   : realize the Vrank 40...Reg (Start and Ending Regs). Vrank have  same order with phy Rank, Size is actual Size
===================================================================*/
void DRAMSetEndingAddress(DRAM_SYS_ATTR * DramAttr)
{
	u8 Shift = 1, Data, RankNO, Size, Start = 0, End = 0, Vrank;
	for (RankNO = 0; RankNO < 4; RankNO++) {
		if ((DramAttr->RankPresentMap & Shift) != 0) {
			Size = (u8) (DramAttr->RankSize[RankNO] >> 26);	// current Size in the unit of 64M
			if (Size != 0) {

				End = End + Size;	// calculate current ending address,   add the current Size to ending
				Vrank = RankNO;	// get virtual Rank
				Data = End;	// set begin/End address register to correspondig virtual       Rank #
				pci_write_config8(MEMCTRL, 0x40 + Vrank, Data);
				Data = Start;
				pci_write_config8(MEMCTRL, 0x48 + Vrank, Data);
				PRINT_DEBUG_MEM("Rank: ");
				PRINT_DEBUG_MEM_HEX8(Vrank);
				PRINT_DEBUG_MEM(", Start:");
				PRINT_DEBUG_MEM_HEX8(Start);
				PRINT_DEBUG_MEM(", End:");
				PRINT_DEBUG_MEM_HEX8(End);
				PRINT_DEBUG_MEM("\r");

				Start = End;
			}
		}
		Shift <<= 1;
	}

	if (DramAttr->RankNumChB > 0) {
		//this is a bug,fixed is to 2,so the max LL size is 128M
		Data = 0x02;
		pci_write_config8(MEMCTRL, 0x44, Data);
	}
	Data = End * 4;
	pci_write_config8(PCI_DEV(0, 17, 7), 0x60, Data);
	// We should directly write to south Bridge, not in north bridge
	// program LOW TOP Address
	Data = pci_read_config8(MEMCTRL, 0x88);
	pci_write_config8(MEMCTRL, 0x85, Data);

	// also program vlink mirror
	// We should directly write to south Bridge, not in north bridge
	pci_write_config8(PCI_DEV(0, 17, 7), 0xe5, Data);
}

/*===================================================================
Function   : DRAMPRToVRMapping()
Precondition :
Input       :
		  DramAttr: pointer point to  DRAMSYSATTR  which consist the DDR and Dimm information in MotherBoard
Output     : Void
Purpose   : set the Vrank-prank map with the same order
===================================================================*/
void DRAMPRToVRMapping(DRAM_SYS_ATTR * DramAttr)
{
	u8 Shift, Data, and, or, DimmNO = 0, PhyRankNO, Reg;

	for (Reg = 0x54; Reg <= 0x57; Reg++)	//clear the map-reg
	{
		Data = 0;
		pci_write_config8(MEMCTRL, Reg, Data);
	}

	Shift = 1;
	for (PhyRankNO = 0; PhyRankNO < MAX_RANKS; PhyRankNO++) {
		if ((DramAttr->RankPresentMap & Shift) != 0) {
			or = PhyRankNO;	// get virtual Rank   ,same with PhyRank
			or |= 0x08;

			if ((PhyRankNO & 0x01) == 0x01)	// get mask for register
				and = 0xf0;
			else {
				and = 0x0f;
				or <<= 4;
			}
			DimmNO = (PhyRankNO >> 1);
			Data = pci_read_config8(MEMCTRL, 0x54 + DimmNO);
			Data &= and;
			Data |= or;
			pci_write_config8(MEMCTRL, 0x54 + DimmNO, Data);
		}
		Shift <<= 1;
	}
}
