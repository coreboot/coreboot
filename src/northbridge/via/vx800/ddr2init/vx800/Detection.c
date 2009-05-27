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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#define SMBUS_ADDR_CH_A_1         0xA0	// Dimmx
#define SMBUS_ADDR_CH_A_2         0xA2	// Dimmx
#define SMBUS_ADDR_CH_B_1         0xA4	// Dimmx
#define SMBUS_ADDR_CH_B_2         0xA6	// Dimmx

/*read data*/
CB_STATUS GetSPDData(u8 Slot, u8 Length, u8 * Buf);

void DRAMCmdRate(DRAM_SYS_ATTR * DramAttr);



CB_STATUS GetInfoFromSPD(DRAM_SYS_ATTR * DramAttr);

CB_STATUS GetSPDData(u8 Slot, u8 Length, u8 * Buf)
{
	// CB_STATUS Status = CB_NOT_READY;
	u8 Val;
	u8 i;

	if (1 > Length || NULL == Buf)
		return CB_INVALID_PARAMETER;

	for (i = 0; i < Length; i++) {
		Val = get_spd_data(ctrl.channel0[Slot], i);
		*(Buf + i) = Val;
	}
	return CB_SUCCESS;
}

CB_STATUS DRAMDetect(DRAM_SYS_ATTR * DramAttr)
{
	CB_STATUS Status = CB_SUCCESS;

	PRINT_DEBUG_MEM("Dram Detection \r");

	/*Read D0F3Rx6C , detect memory type DDR1 or DDR2 */
	// 353 supports DDR2 only
	DramAttr->DramType = RAMTYPE_SDRAMDDR2;
	/*get information for SPD */
	Status = GetInfoFromSPD(DramAttr);
	if (CB_SUCCESS == Status) {
		/*64bit or 128Bit */
		//
		//  if (RAMTYPE_SDRAMDDR == DramAttr->DramType)

		/*select command rate */
		DRAMCmdRate(DramAttr);
	}
	return Status;
}


// Determine 1T or 2T Command Rate:
// To enable 1T command Rate, the       system will satisfy the following 3 conditions:
// 1. Each DRAM channel may have 1 or 2 ranks of DIMM. 3/4 ranks can not support 1T command rate
//    It's for loading issue. 1T can supports (a). only one socket with two ranks OR
//    (b). two sockets each with 1 rank.
// 2. User wishes       to enable 1T command rate mode and turn on by Setup menu
// 3. If 1T command rate can    be enabled, just set EBP bit here.
void DRAMCmdRate(DRAM_SYS_ATTR * DramAttr)
{
	u8 Data;

	// 5.1t/2t command rate, use the stable set
	//offset50
	DramAttr->CmdRate = 2;
	Data = pci_read_config8(MEMCTRL, 0x50);
	Data = (u8) (Data & 0xEE);
	pci_write_config8(MEMCTRL, 0x50, Data);
}

/*get SPD data and set RANK presence map*/
/*
Sockets0,1 is Channel A / Sockets2,3 is Channel B
socket0 SPD device address 0x50 / socket1 SPD device address 0x51
socket2 SPD device address 0x52 / socket3 SPD device address 0x53
*/
CB_STATUS GetInfoFromSPD(DRAM_SYS_ATTR * DramAttr)
{
	CB_STATUS Status;
	u8 *pSPDDataBuf;
	u8 ModuleDataWidth;
	u8 ChipWidth;
	u8 RankNum;
	u8 LoadNum;
	u8 Sockets, i;
	BOOLEAN bFind;
	bFind = FALSE;
	Status = CB_DEVICE_ERROR;

	for (Sockets = 0; Sockets < MAX_SOCKETS; Sockets++) {
		pSPDDataBuf = DramAttr->DimmInfo[Sockets].SPDDataBuf;
		pSPDDataBuf[SPD_MEMORY_TYPE] =
		    get_spd_data(ctrl.channel0[Sockets], SPD_MEMORY_TYPE);
		if (pSPDDataBuf[SPD_MEMORY_TYPE] == 0) {
			Status = CB_NOT_READY;
		} else {
			Status =
			    GetSPDData(Sockets, SPD_DATA_SIZE,
				       pSPDDataBuf);
			PRINT_DEBUG_MEM("SPD : \r");
			for (i = 0; i < SPD_DATA_SIZE; i++) {
				PRINT_DEBUG_MEM(" ");
				PRINT_DEBUG_MEM_HEX8(pSPDDataBuf[i]);
			}
		}
		if (CB_SUCCESS == Status) {
			/*if Dram Controller detected type not same as the type got from SPD, There are ERROR */
			if (pSPDDataBuf[SPD_MEMORY_TYPE] !=
			    DramAttr->DramType) {
				Status = CB_DEVICE_ERROR;	/*Memory int error */
				PRINT_DEBUG_MEM
				    ("Memory Device ERROR: Dram Controller detected type != type got from SPD \r");
				break;
			}
			DramAttr->DimmInfo[Sockets].bPresence = TRUE;
			/*calculate load number (chips number) */
			ModuleDataWidth =
			    (u8) (DramAttr->DimmInfo[Sockets].
				  SPDDataBuf[SPD_SDRAM_MOD_DATA_WIDTH +
					     1]);
			ModuleDataWidth = (u8) (ModuleDataWidth << 8);
			ModuleDataWidth |=
			    (u8) (DramAttr->DimmInfo[Sockets].
				  SPDDataBuf[SPD_SDRAM_MOD_DATA_WIDTH]);
			ChipWidth =
			    (u8) ((DramAttr->DimmInfo[Sockets].
				   SPDDataBuf[SPD_SDRAM_WIDTH]) & 0x7F);
			LoadNum = (u8) (ModuleDataWidth / ChipWidth);

			/*set the RANK map */
			RankNum = (u8) (pSPDDataBuf[SPD_SDRAM_DIMM_RANKS] & 0x3);	/*get bit0,1, the Most number of supported RANK is 2 */
			if (RAMTYPE_SDRAMDDR2 == DramAttr->DramType)
				RankNum++;	/*for DDR bit[0,1] 01->1 RANK  10->2 RANK; for DDR2 bit[0,1] = 00 -> 1 RANK  01 -> 2 RANK */
			if (RankNum != 2 && RankNum != 1) {	/*every DIMM have 1 or 2 ranks */
				Status = CB_DEVICE_ERROR;
				PRINT_DEBUG_MEM
				    ("Memory Device ERROR: the number of RANK not support!\r");
				break;
			}

			if (Sockets < 2) {	/*sockets0,1 is channel A */
				DramAttr->RankNumChA =
				    (u8) (DramAttr->RankNumChA + RankNum);
				DramAttr->DimmNumChA++;
				DramAttr->LoadNumChA =
				    (u8) (DramAttr->LoadNumChA * LoadNum *
					  RankNum);
			} else {	/*sockets2,3 is channel B */

				DramAttr->RankNumChB =
				    (u8) (DramAttr->RankNumChB + RankNum);
				DramAttr->DimmNumChB++;
				DramAttr->LoadNumChB =
				    (u8) (DramAttr->LoadNumChB * LoadNum *
					  RankNum);;
			}
			RankNum |= 1;	/*set rank map */
			DramAttr->RankPresentMap |=
			    (RankNum << (Sockets * 2));
			bFind = TRUE;
		}
	}
	PRINT_DEBUG_MEM("Rank Present Map:");
	PRINT_DEBUG_MEM_HEX8(DramAttr->RankPresentMap);
	PRINT_DEBUG_MEM("\r");

	if (bFind)
		Status = CB_SUCCESS;

	return Status;
}
