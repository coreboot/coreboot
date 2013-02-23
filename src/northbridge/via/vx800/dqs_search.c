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

void SetDQSOutputCHA(DRAM_SYS_ATTR * DramAttr);
void SetDQSOutputCHB(DRAM_SYS_ATTR * DramAttr);

/*===================================================================
Function   : DRAMDQSOutputSearchCHA()
Precondition :
Input      :
		   DramAttr:  pointer point to  DRAM_SYS_ATTR  which consist the DDR and Dimm information
		                    in MotherBoard
Output     : Void
Purpose   : set DQS output delay register reg70  and DQ output delay register reg71
===================================================================*/

#define CH_A      0
#define CH_B      1
void DRAMDQSOutputSearch(DRAM_SYS_ATTR * DramAttr)
{
	if (DramAttr->RankNumChA > 0)
		SetDQSOutputCHA(DramAttr);
}

/*===================================================================
Function   : SetDQSOutputCHA()
Precondition :
Input      :
		   DramAttr:  pointer point to  DRAM_SYS_ATTR  which consist the DDR and Dimm information
		                    in MotherBoard
Output     : Void
Purpose   :  according the frequence set CHA DQS output
===================================================================*/
void SetDQSOutputCHA(DRAM_SYS_ATTR * DramAttr)
{
	u8 Reg70, Reg71;
	u8 Index;

	if (DramAttr->DramFreq == DIMMFREQ_400)
		Index = 3;
	else if (DramAttr->DramFreq == DIMMFREQ_533)
		Index = 2;
	else if (DramAttr->DramFreq == DIMMFREQ_667)
		Index = 1;
	else if (DramAttr->DramFreq == DIMMFREQ_800)
		Index = 0;
	else
		Index = 3;

	if (DramAttr->RankNumChA > 2) {
		Reg70 = Fixed_DQSA_3_4_Rank_Table[Index][0];
		Reg71 = Fixed_DQSA_3_4_Rank_Table[Index][1];
	} else {
		Reg70 = Fixed_DQSA_1_2_Rank_Table[Index][0];
		Reg71 = Fixed_DQSA_1_2_Rank_Table[Index][1];
	}
	pci_write_config8(MEMCTRL, 0x70, Reg70);
	pci_write_config8(MEMCTRL, 0x71, Reg71);
}

//################
//     STEP 12   #
//################

/*===================================================================
Function   : DRAMDQSInputSearch()
Precondition :
Input      :
		   DramAttr:  pointer point to  DRAM_SYS_ATTR  which consist the DDR and Dimm information
		                    in MotherBoard
Output     : Void
Purpose   : search DQS input delay for CHA/CHB
===================================================================*/

void DRAMDQSInputSearch(DRAM_SYS_ATTR * DramAttr)
{
	u8 Data;
	//auto mode
	Data = 0x0;
	pci_write_config8(MEMCTRL, 0x77, Data);
}
