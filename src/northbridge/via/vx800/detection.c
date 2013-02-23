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

/* FIXME this should go away */
static const struct mem_controller ctrl = {
	.channel0 = {DIMM0, DIMM1},
};

/* read data */
CB_STATUS GetSPDData(u8 Slot, u8 Length, u8 *Buf);
void DRAMCmdRate(DRAM_SYS_ATTR *DramAttr);
CB_STATUS GetInfoFromSPD(DRAM_SYS_ATTR *DramAttr);

CB_STATUS GetSPDData(u8 Slot, u8 Length, u8 *Buf)
{
	// CB_STATUS Status = CB_NOT_READY;
	u8 Val, i;

	if (1 > Length || NULL == Buf)
		return CB_INVALID_PARAMETER;

	for (i = 0; i < Length; i++) {
		Val = get_spd_data(ctrl.channel0[Slot], i);
		*(Buf + i) = Val;
	}
	return CB_SUCCESS;
}

CB_STATUS DRAMDetect(DRAM_SYS_ATTR *DramAttr)
{
	CB_STATUS Status = CB_SUCCESS;

	PRINT_DEBUG_MEM("DRAM detection \r");

	/* Read D0F3Rx6C, detect memory type DDR1 or DDR2. */
	/* 353 supports DDR2 only */
	DramAttr->DramType = RAMTYPE_SDRAMDDR2;
	/* Get information for SPD. */
	Status = GetInfoFromSPD(DramAttr);
	if (CB_SUCCESS == Status) {
		/* 64bit or 128Bit */

		// if (RAMTYPE_SDRAMDDR == DramAttr->DramType)

		/* Select command rate. */
		DRAMCmdRate(DramAttr);
	}
	return Status;
}

/*
 * Determine 1T or 2T command rate.
 *
 * To enable 1T command rate, the system will satisfy the following
 * three conditions:
 *
 *   1. Each DRAM channel may have 1 or 2 ranks of DIMM. 3/4 ranks can not
 *      support 1T command rate. It's for loading issue. 1T can supports
 *       (a) only one socket with two ranks, OR
 *       (b) two sockets each with 1 rank.
 *   2. User wishes to enable 1T command rate mode and turn on by setup menu.
 *   3. If 1T command rate can be enabled, just set EBP bit here.
 */
void DRAMCmdRate(DRAM_SYS_ATTR *DramAttr)
{
	u8 Data;

	// 5.1t/2t command rate, use the stable set
	//offset50
	DramAttr->CmdRate = 2;
	Data = pci_read_config8(MEMCTRL, 0x50);
	Data = (u8) (Data & 0xEE);
	pci_write_config8(MEMCTRL, 0x50, Data);
}

/*
 * Get SPD data and set RANK presence map.
 *
 * Sockets0,1 is Channel A / Sockets2,3 is Channel B.
 *
 * Socket0 SPD device address 0x50 / socket1 SPD device address 0x51
 * Socket2 SPD device address 0x52 / socket3 SPD device address 0x53
 */
CB_STATUS GetInfoFromSPD(DRAM_SYS_ATTR *DramAttr)
{
	CB_STATUS Status;
	u8 *pSPDDataBuf;
	u8 ModuleDataWidth, ChipWidth, RankNum, LoadNum, Sockets, i;
	BOOLEAN bFind;  /* FIXME: We don't have/want BOOLEAN. */

	bFind = FALSE;  /* FIXME: We don't have/want FALSE. */
	Status = CB_DEVICE_ERROR;

	for (Sockets = 0; Sockets < MAX_SOCKETS; Sockets++) {
		pSPDDataBuf = DramAttr->DimmInfo[Sockets].SPDDataBuf;
		pSPDDataBuf[SPD_MEMORY_TYPE] =
		    get_spd_data(ctrl.channel0[Sockets], SPD_MEMORY_TYPE);
		if (pSPDDataBuf[SPD_MEMORY_TYPE] == 0) {
			Status = CB_NOT_READY;
		} else {
			Status =
			    GetSPDData(Sockets, SPD_DATA_SIZE, pSPDDataBuf);
			PRINT_DEBUG_MEM("SPD : \r");
			for (i = 0; i < SPD_DATA_SIZE; i++) {
				PRINT_DEBUG_MEM(" ");
				PRINT_DEBUG_MEM_HEX8(pSPDDataBuf[i]);
			}
		}
		if (CB_SUCCESS == Status) {
			/*
			 * If DRAM controller detected type not same as the
			 * type got from SPD, there are ERROR.
			 */
			if (pSPDDataBuf[SPD_MEMORY_TYPE] != DramAttr->DramType) {
				Status = CB_DEVICE_ERROR; /* memory int error */
				PRINT_DEBUG_MEM("Memory Device ERROR: DRAM "
                                                "controller detected type != "
                                                "type got from SPD\r");
				break;
			}
			DramAttr->DimmInfo[Sockets].bPresence = TRUE;

			/* Calculate load number (chips number). */
			ModuleDataWidth = (u8) (DramAttr->
				  DimmInfo[Sockets].SPDDataBuf
				  [SPD_SDRAM_MOD_DATA_WIDTH + 1]);
			ModuleDataWidth = (u8) (ModuleDataWidth << 8);
			ModuleDataWidth |= (u8) (DramAttr->
				  DimmInfo[Sockets].SPDDataBuf
				  [SPD_SDRAM_MOD_DATA_WIDTH]);
			ChipWidth = (u8) ((DramAttr->
				   DimmInfo[Sockets].SPDDataBuf
				   [SPD_SDRAM_WIDTH]) & 0x7F);
			LoadNum = (u8) (ModuleDataWidth / ChipWidth);

			/* Set the RANK map. */
                        /* Get bit0,1, the most number of supported RANK is 2. */
			RankNum = (u8) (pSPDDataBuf[SPD_SDRAM_DIMM_RANKS] & 0x3);
			if (RAMTYPE_SDRAMDDR2 == DramAttr->DramType)
                                /*
                                 * For DDR bit[0,1]: 01->1 RANK, 10->2 RANK
                                 * For DDR2 bit[0,1]: 00->1 RANK, 01->2 RANK
                                 */
				RankNum++;

                        /* Every DIMM have 1 or 2 ranks. */
			if (RankNum != 2 && RankNum != 1) {
				Status = CB_DEVICE_ERROR;
				PRINT_DEBUG_MEM("Memory Device ERROR: Number "
                                                "of RANK not supported!\r");
				break;
			}

			if (Sockets < 2) { /* Sockets0,1 is channel A */
				DramAttr->RankNumChA =
				    (u8) (DramAttr->RankNumChA + RankNum);
				DramAttr->DimmNumChA++;
				DramAttr->LoadNumChA =
				    (u8) (DramAttr->LoadNumChA * LoadNum *
					  RankNum);
			} else { /* Sockets2,3 is channel B */
				DramAttr->RankNumChB =
				    (u8) (DramAttr->RankNumChB + RankNum);
				DramAttr->DimmNumChB++;
				DramAttr->LoadNumChB =
				    (u8) (DramAttr->LoadNumChB * LoadNum *
					  RankNum);;
			}
			RankNum |= 1; /* Set rank map. */
			DramAttr->RankPresentMap |= (RankNum << (Sockets * 2));
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
