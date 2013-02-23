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

/*
      Driving setting:  ODT/DQS/DQ/CS/MAA/MAB/DCLK
    */

void DrivingODT(DRAM_SYS_ATTR * DramAttr);

void DrivingDQS(DRAM_SYS_ATTR * DramAttr);

void DrivingDQ(DRAM_SYS_ATTR * DramAttr);

void DrivingCS(DRAM_SYS_ATTR * DramAttr);

void DrivingMA(DRAM_SYS_ATTR * DramAttr);

void DrivingDCLK(DRAM_SYS_ATTR * DramAttr);

/* DRAM Driving Adjustment*/
void DRAMDriving(DRAM_SYS_ATTR * DramAttr)
{
	PRINT_DEBUG_MEM("set ODT!\r");
	DrivingODT(DramAttr);

	PRINT_DEBUG_MEM("set DQS!\r");
	DrivingDQS(DramAttr);

	PRINT_DEBUG_MEM(("set DQ!\r"));
	DrivingDQ(DramAttr);

	PRINT_DEBUG_MEM("set CS!\r");
	DrivingCS(DramAttr);

	PRINT_DEBUG_MEM("set MAA!\r");
	DrivingMA(DramAttr);

	PRINT_DEBUG_MEM("set DCLK!\r");
	DrivingDCLK(DramAttr);
}

/*
ODT	Control	for DQ/DQS/CKE/SCMD/DCLKO in ChA & ChB
which include driving enable/range and strong/weak selection

Processing: According to DRAM frequency to ODT control bits.
      		Because function enable bit must be the last one to be set.
      		So the register VIA_NB3DRAM_REGD4 and VIA_NB3DRAM_REGD3 should be
      		the last register	to be programmed.
*/
//-------------------------------------------------------------------------------
//                      ODT Lookup Table
//-------------------------------------------------------------------------------
#define Rank0_ODT				0
#define Rank1_ODT				1
#define Rank2_ODT				2
#define Rank3_ODT				3
#define NA_ODT					0
#define NB_ODT_75ohm			0
#define NB_ODT_150ohm			1

#define DDR2_ODT_75ohm			0x20
#define DDR2_ODT_150ohm			0x40

// Setting of ODT Lookup TBL
//                      RankMAP , Rank 3               Rank 2              Rank 1              Rank 0           , DRAM & NB ODT setting
//                  db  0000b   , Reserved
#define ODTLookup_Tbl_count 8
static const u8 ODTLookup_TBL[ODTLookup_Tbl_count][3] = {
	// 0001b
	{0x01,
	 (Rank3_ODT << 6) + (Rank2_ODT << 4) + (Rank1_ODT << 2) +
	 Rank0_ODT, DDR2_ODT_150ohm + NB_ODT_75ohm},
	// 0010b        , Reserved
	// 0011b
	{0x03,
	 (Rank3_ODT << 6) + (Rank2_ODT << 4) + (Rank0_ODT << 2) +
	 Rank1_ODT, DDR2_ODT_150ohm + NB_ODT_75ohm},
	// 0100b
	{0x04,
	 (Rank3_ODT << 6) + (Rank2_ODT << 4) + (Rank1_ODT << 2) +
	 Rank0_ODT, DDR2_ODT_150ohm + NB_ODT_75ohm},
	// 0101b
	{0x05,
	 (Rank3_ODT << 6) + (Rank0_ODT << 4) + (Rank1_ODT << 2) +
	 Rank2_ODT, DDR2_ODT_75ohm + NB_ODT_150ohm},
	// 0110b        , Reserved
	// 0111b
	{0x07,
	 (Rank3_ODT << 6) + (Rank0_ODT << 4) + (Rank2_ODT << 2) +
	 Rank2_ODT, DDR2_ODT_75ohm + NB_ODT_150ohm},
	// 1000b        , Reserved
	// 1001b        , Reserved
	// 1010b        , Reserved
	// 1011b        , Reserved
	// 1100b
	{0x0c,
	 (Rank2_ODT << 6) + (Rank3_ODT << 4) + (Rank1_ODT << 2) +
	 Rank0_ODT, DDR2_ODT_150ohm + NB_ODT_75ohm},
	// 1101b
	{0x0d,
	 (Rank0_ODT << 6) + (Rank0_ODT << 4) + (Rank1_ODT << 2) +
	 Rank2_ODT, DDR2_ODT_75ohm + NB_ODT_150ohm},
	// 1110b        , Reserved
	// 1111b
	{0x0f,
	 (Rank0_ODT << 6) + (Rank0_ODT << 4) + (Rank2_ODT << 2) +
	 Rank2_ODT, DDR2_ODT_75ohm + NB_ODT_150ohm}
};

#define ODT_Table_Width_DDR2		4
//                                                                                               RxD6   RxD3
static const u8 ODT_Control_DDR2[ODT_Table_Width_DDR2] = { 0xFC, 0x01 };

void DrivingODT(DRAM_SYS_ATTR * DramAttr)
{
	u8 Data;
	u8 i;
	BOOLEAN bFound;

	pci_write_config8(MEMCTRL, 0xD0, 0x88);

	Data = ODT_Control_DDR2[0];
	pci_write_config8(MEMCTRL, 0xd6, Data);

	Data = ODT_Control_DDR2[1];
	pci_write_config8(MEMCTRL, 0xd3, Data);

	Data = pci_read_config8(MEMCTRL, 0x9e);
	//set MD turn_around wait state
	Data &= 0xCF;		/*clear bit4,5 */
	if (DIMMFREQ_400 == DramAttr->DramFreq)
		Data |= 0x0;
	else if (DIMMFREQ_533 == DramAttr->DramFreq)
		Data |= 0x10;
	else if (DIMMFREQ_667 == DramAttr->DramFreq)
		Data |= 0x20;
	else if (DIMMFREQ_800 == DramAttr->DramFreq)
		Data |= 0x20;
	else
		Data |= 0;
	pci_write_config8(MEMCTRL, 0x9e, Data);

	if (DIMMFREQ_400 == DramAttr->DramFreq)
		Data = 0x0;
	else if (DIMMFREQ_533 == DramAttr->DramFreq)
		Data = 0x11;
	else if (DIMMFREQ_667 == DramAttr->DramFreq)
		Data = 0x11;
	else if (DIMMFREQ_800 == DramAttr->DramFreq)
		Data = 0x11;
	else
		Data = 0;
	pci_write_config8(MEMCTRL, 0x9f, Data);

	/*channel A ODT select */
	if (DramAttr->DimmNumChA > 0) {
		Data = pci_read_config8(MEMCTRL, 0xd5);
		Data &= 0x5F;	/*clear bit7,5 */
		if (DramAttr->RankNumChA > 2)
			Data |= 0xA0;	/*if rank number > 2 (3or4), set bit7,5 */
		else
			Data |= 0x00;	/*if rank number is 1or2, clear bit5 */
		pci_write_config8(MEMCTRL, 0xd5, Data);

		Data = pci_read_config8(MEMCTRL, 0xd7);
		Data &= 0xEF;	/*clear bit7 */
		if (DramAttr->RankNumChA > 2)
			Data |= 0x80;	/*if rank number > 2 (3or4), set bit7 */
		else
			Data |= 0x00;	/*if rank number is 1or2,  clear bit7 */
		pci_write_config8(MEMCTRL, 0xd7, Data);

		/*channel A */
		Data = pci_read_config8(MEMCTRL, 0xd5);
		Data &= 0xF3;	//bit2,3
		if (DramAttr->DimmNumChA == 2)	/*2 Dimm, 3or4 Ranks */
			Data |= 0x00;
		else if (DramAttr->DimmNumChA == 1)
			Data |= 0x04;
		pci_write_config8(MEMCTRL, 0xd5, Data);

		if ((DramAttr->RankPresentMap & 0x0F) != 0) {	/*channel A */
			// MAA ODT Lookup Table
			bFound = FALSE;
			for (i = 0; i < ODTLookup_Tbl_count; i++) {
				if ((DramAttr->RankPresentMap & 0x0F) ==
				    ODTLookup_TBL[i][0]) {
					Data = ODTLookup_TBL[i][1];
					bFound = TRUE;
				}
			}
			if (!bFound) {	/*set default value */
				Data =
				    ODTLookup_TBL[ODTLookup_Tbl_count - 1][1];
			}
			pci_write_config8(MEMCTRL, 0x9c, Data);

			//set CHA MD ODT control State Dynamic-on
			Data = pci_read_config8(MEMCTRL, 0xD4);
			Data &= 0xC9;
			Data |= 0x30;
			pci_write_config8(MEMCTRL, 0xD4, Data);

			Data = pci_read_config8(MEMCTRL, 0x9e);
			Data |= 0x01;
			pci_write_config8(MEMCTRL, 0x9e, Data);
		}

	}
	/*channel B */
	if (1 == ENABLE_CHC) {
		//CHB has not auto compensation mode ,so must set it manual,or else CHB initialization will not successful
		//   Data =0x88;
		//pci_write_config8(MEMCTRL, 0xd0, Data);

		Data = pci_read_config8(MEMCTRL, 0xd5);
		Data &= 0xAF;
		if (DramAttr->RankNumChB > 2)	/*rank number 3 or 4 */
			Data |= 0x50;
		else
			Data |= 0x00;
		pci_write_config8(MEMCTRL, 0xd5, Data);

		Data = pci_read_config8(MEMCTRL, 0xd7);
		Data &= 0xBF;	/*clear bit6 */
		if (DramAttr->RankNumChB > 2)
			Data |= 0x40;	/*if rank number > 2 (3or4), set bit7 */
		else
			Data |= 0x00;	/*if rank number is 1or2,  clear bit7 */
		pci_write_config8(MEMCTRL, 0xd7, Data);

		Data = pci_read_config8(MEMCTRL, 0xd5);
		Data &= 0xFC;
		if (DramAttr->DimmNumChB == 2)	/*2 Dimm, 3or4 Ranks */
			Data |= 0x00;	// 2 dimm RxD5[2,0]=0,0b
		else if (DramAttr->DimmNumChB == 1)
			Data |= 0x01;	// 1 dimm RxD5[2,0]=1,1b
		pci_write_config8(MEMCTRL, 0xd5, Data);

		//set CHB MD ODT control State Dynamic-on
		Data = pci_read_config8(MEMCTRL, 0xD4);
		Data &= 0xF6;
		Data |= 0x08;
		pci_write_config8(MEMCTRL, 0xD4, Data);

		//enable CHB differential DQS input
		Data = pci_read_config8(MEMCTRL, 0x9E);
		Data |= 0x02;
		pci_write_config8(MEMCTRL, 0x9E, Data);
	}
	//enable ODT Control
	Data = pci_read_config8(MEMCTRL, 0x9e);
	Data |= 0x80;
	pci_write_config8(MEMCTRL, 0x9e, Data);
}

void DrivingDQS(DRAM_SYS_ATTR * DramAttr)
{
	u8 Data;

	/*channel A */
	if (DramAttr->RankNumChA > 0) {
		Data = DDR2_DQSA_Driving_Table[DramAttr->RankNumChA - 1];
		pci_write_config8(MEMCTRL, 0xe0, Data);
	}

	/*channel B */
	if (1 == ENABLE_CHC) {
		Data = DDR2_DQSB_Driving_Table[DramAttr->RankNumChB - 1];
		pci_write_config8(MEMCTRL, 0xe1, Data);
	}

}

void DrivingDQ(DRAM_SYS_ATTR * DramAttr)
{
	u8 Data;

	/*channel A */
	if (DramAttr->RankNumChA > 0) {
		Data = DDR2_DQA_Driving_Table[DramAttr->RankNumChA - 1];
		pci_write_config8(MEMCTRL, 0xe2, Data);

	}
	/*channel B */
	if (1 == ENABLE_CHC) {
		Data = DDR2_DQB_Driving_Table[DramAttr->RankNumChB - 1];
		pci_write_config8(MEMCTRL, 0xe3, Data);
	}
}

void DrivingCS(DRAM_SYS_ATTR * DramAttr)
{
	u8 Data;
	/*Channel A */
	if (DramAttr->RankNumChA > 0) {
		Data = DDR2_CSA_Driving_Table_x8[DramAttr->RankNumChA - 1];
		pci_write_config8(MEMCTRL, 0xe4, Data);
	}
	/*channel B */
	if (1 == ENABLE_CHC) {
		Data = DDR2_CSB_Driving_Table_x8[DramAttr->RankNumChB - 1];
		pci_write_config8(MEMCTRL, 0xe5, Data);
	}
}

void DrivingMA(DRAM_SYS_ATTR * DramAttr)
{
	u8 Data;
	u8 i, FreqId;

	if (DramAttr->RankNumChA > 0) {
		if (DIMMFREQ_400 == DramAttr->DramFreq)
			FreqId = 1;
		else if (DIMMFREQ_533 == DramAttr->DramFreq)
			FreqId = 2;
		else if (DIMMFREQ_667 == DramAttr->DramFreq)
			FreqId = 3;
		else if (DIMMFREQ_800 == DramAttr->DramFreq)
			FreqId = 4;
		else
			FreqId = 1;
		for (i = 0; i < MA_Table; i++) {
			if (DramAttr->LoadNumChA <=
			    DDR2_MAA_Driving_Table[i][0]) {
				Data = DDR2_MAA_Driving_Table[i][FreqId];
				break;
			}
		}
		pci_write_config8(MEMCTRL, 0xe8, Data);
	}
	if (1 == ENABLE_CHC) {
		for (i = 0; i < MA_Table; i++) {
			if (DramAttr->LoadNumChA <=
			    DDR2_MAB_Driving_Table[i][0]) {
				Data = DDR2_MAB_Driving_Table[i][1];
				break;
			}
		}
		pci_write_config8(MEMCTRL, 0xe9, Data);
	}
}

void DrivingDCLK(DRAM_SYS_ATTR * DramAttr)
{
	u8 Data;
	u8 FreqId;

	if (DIMMFREQ_400 == DramAttr->DramFreq)
		FreqId = 0;
	else if (DIMMFREQ_533 == DramAttr->DramFreq)
		FreqId = 1;
	else if (DIMMFREQ_667 == DramAttr->DramFreq)
		FreqId = 2;
	else if (DIMMFREQ_800 == DramAttr->DramFreq)
		FreqId = 4;
	else
		FreqId = 0;

	/*channel A */
	if (DramAttr->RankNumChA > 0) {
		Data = DDR2_DCLKA_Driving_Table[FreqId];
		pci_write_config8(MEMCTRL, 0xe6, Data);
	}
	/*channel B */
	if (1 == ENABLE_CHC) {
		Data = DDR2_DCLKB_Driving_Table[FreqId];
		pci_write_config8(MEMCTRL, 0xe7, Data);
	}

}
