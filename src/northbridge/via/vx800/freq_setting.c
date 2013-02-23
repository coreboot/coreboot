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

void CalcCLAndFreq(DRAM_SYS_ATTR * DramAttr);

/*
 Set DRAM Frequency
*/
void DRAMFreqSetting(DRAM_SYS_ATTR * DramAttr)
{

	u8 Data = 0;

	PRINT_DEBUG_MEM("Dram Frequency setting \r");

	//calculate dram frequency using SPD data
	CalcCLAndFreq(DramAttr);

	//init some Dramc control by Simon Chu slide
	//Must use "CPU delay" to make sure VLINK is dis-connect
	Data = pci_read_config8(PCI_DEV(0, 0, 7), 0x47);
	Data = (u8) (Data | 0x04);
	pci_write_config8(PCI_DEV(0, 0, 7), 0x47, Data);

	//in order to make sure NB command buffer don`t have pending request(C2P cycle)
	//CPU DELAY
	WaitMicroSec(20);

	//Before Set Dram Frequency, we must set 111 by Simon Chu slide.
	Data = pci_read_config8(MEMCTRL, 0x90);
	Data = (u8) ((Data & 0xf8) | 7);
	pci_write_config8(MEMCTRL, 0x90, Data);

	WaitMicroSec(20);

	//Set Dram Frequency.
	Data = pci_read_config8(MEMCTRL, 0x90);
	switch (DramAttr->DramFreq) {
	case DIMMFREQ_400:
		Data = (u8) ((Data & 0xf8) | 3);
		break;
	case DIMMFREQ_533:
		Data = (u8) ((Data & 0xf8) | 4);
		break;
	case DIMMFREQ_667:
		Data = (u8) ((Data & 0xf8) | 5);
		break;
	case DIMMFREQ_800:
		Data = (u8) ((Data & 0xf8) | 6);
		break;
	default:
		Data = (u8) ((Data & 0xf8) | 1);;
	}
	pci_write_config8(MEMCTRL, 0x90, Data);

	//CPU Delay
	WaitMicroSec(20);

	// Manual       reset and adjust DLL when DRAM change frequency
	Data = pci_read_config8(MEMCTRL, 0x6B);
	Data = (u8) ((Data & 0x2f) | 0xC0);
	pci_write_config8(MEMCTRL, 0x6B, Data);

	//CPU Delay
	WaitMicroSec(20);

	Data = pci_read_config8(MEMCTRL, 0x6B);
	Data = (u8) (Data | 0x10);
	pci_write_config8(MEMCTRL, 0x6B, Data);

	//CPU Delay
	WaitMicroSec(20);

	Data = pci_read_config8(MEMCTRL, 0x6B);
	Data = (u8) (Data & 0x3f);
	pci_write_config8(MEMCTRL, 0x6B, Data);

	//disable V_LINK Auto-Disconnect, or else program may stopped at some place and
	//we cannot find the reason
	Data = pci_read_config8(PCI_DEV(0, 0, 7), 0x47);
	Data = (u8) (Data & 0xFB);
	pci_write_config8(PCI_DEV(0, 0, 7), 0x47, Data);

}

/*
 calculate CL and dram freq
 DDR1
 +---+---+---+---+---+---+---+---+
 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
 +---+---+---+---+---+---+---+---+
 |TBD| 4 |3.5| 3 |2.5| 2 |1.5| 1 |
 +---+---+---+---+---+---+---+---+
 DDR2
 +---+---+---+---+---+---+---+---+
 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
 +---+---+---+---+---+---+---+---+
 |TBD| 6 | 5 | 4 | 3 | 2 |TBD|TBD|
 +---+---+---+---+---+---+---+---+
*/
static const u8 CL_DDR1[7] = { 10, 15, 20, 25, 30, 35, 40 };
static const u8 CL_DDR2[7] = { 0, 0, 20, 30, 40, 50, 60 };

void CalcCLAndFreq(DRAM_SYS_ATTR * DramAttr)
{
	u8 AllDimmSupportedCL, Tmp;
	u8 CLMask, tmpMask;
	u8 SckId, BitId, TmpId;
	u16 CycTime, TmpCycTime;

	/*1.list the CL value that all DIMM supported */
	AllDimmSupportedCL = 0xFF;
	if (RAMTYPE_SDRAMDDR2 == DramAttr->DramType)
		AllDimmSupportedCL &= 0x7C;	/*bit2,3,4,5,6 */
	else			/*DDR1 */
		AllDimmSupportedCL &= 0x7F;	/*bit0,1,2,3,4,5,6 */
	for (SckId = 0; SckId < MAX_SOCKETS; SckId++) {
		if (DramAttr->DimmInfo[SckId].bPresence) {	/*all DIMM supported CL */
			AllDimmSupportedCL &=
			    (DramAttr->
			     DimmInfo[SckId].SPDDataBuf[SPD_SDRAM_CAS_LATENCY]);
		}
	}
	if (!AllDimmSupportedCL) {	/*if equal 0, no supported CL */
		PRINT_DEBUG_MEM("SPD Data Error, Can not get CL !!!! \r");
		for (;;) ;
	}

	/*Get CL Value */
	CLMask = 0x40;		/*from Bit6 */

	for (BitId = 7; BitId > 0; BitId--) {
		if ((AllDimmSupportedCL & CLMask) == CLMask) {	/*find the first bit */
			if (RAMTYPE_SDRAMDDR2 == DramAttr->DramType)
				DramAttr->CL = CL_DDR2[BitId - 1];
			else	/*DDR1 */
				DramAttr->CL = CL_DDR1[BitId - 1];
			break;
		}
		CLMask >>= 1;
	}

	/*according the CL value calculate the cycle time, for X or X-1 or X-2 */
	CycTime = 0;
	TmpCycTime = 0;

	for (SckId = 0; SckId < MAX_SOCKETS; SckId++) {
		if (DramAttr->DimmInfo[SckId].bPresence) {
			Tmp =
			    (DramAttr->
			     DimmInfo[SckId].SPDDataBuf[SPD_SDRAM_CAS_LATENCY]);
			tmpMask = 0x40;
			for (TmpId = 7; TmpId > 0; TmpId--) {
				if ((Tmp & tmpMask) == tmpMask)
					break;
				tmpMask >>= 1;
			}
			if (TmpId - BitId == 0) {	/*get Cycle time for X, SPD BYTE9 */
				TmpCycTime =
				    DramAttr->
				    DimmInfo[SckId].SPDDataBuf
				    [SPD_SDRAM_TCLK_X];
			} else if (TmpId - BitId == 1) {	/*get Cycle time for X-1, SPD BYTE23 */
				TmpCycTime =
				    DramAttr->
				    DimmInfo[SckId].SPDDataBuf
				    [SPD_SDRAM_TCLK_X_1];
			} else if (TmpId - BitId == 2) {	/*get cycle time for X-2, SPD BYTE25 */
				TmpCycTime =
				    DramAttr->
				    DimmInfo[SckId].SPDDataBuf
				    [SPD_SDRAM_TCLK_X_2];
			} else {
				//error!!!
			}
			if (TmpCycTime > CycTime)	/*get the most cycle time,there is some problem! */
				CycTime = TmpCycTime;
		}
	}

	if (CycTime <= 0) {
		//error!
		for (;;) ;
	}

	/* cycle time value
	   0x25-->2.5ns Freq=400  DDR800
	   0x30-->3.0ns Freq=333  DDR667
	   0x3D-->3.75ns Freq=266 DDR533
	   0x50-->5.0ns Freq=200  DDR400
	   0x60-->6.0ns Freq=166  DDR333
	   0x75-->7.5ns Freq=133  DDR266
	   0xA0-->10.0ns Freq=100 DDR200
	 */
	if (CycTime <= 0x25) {
		DramAttr->DramFreq = DIMMFREQ_800;
		DramAttr->DramCyc = 250;
	} else if (CycTime <= 0x30) {
		DramAttr->DramFreq = DIMMFREQ_667;
		DramAttr->DramCyc = 300;
	} else if (CycTime <= 0x3d) {
		DramAttr->DramFreq = DIMMFREQ_533;
		DramAttr->DramCyc = 375;
	} else if (CycTime <= 0x50) {
		DramAttr->DramFreq = DIMMFREQ_400;
		DramAttr->DramCyc = 500;
	} else if (CycTime <= 0x60) {
		DramAttr->DramFreq = DIMMFREQ_333;
		DramAttr->DramCyc = 600;
	} else if (CycTime <= 0x75) {
		DramAttr->DramFreq = DIMMFREQ_266;
		DramAttr->DramCyc = 750;
	} else if (CycTime <= 0xA0) {
		DramAttr->DramFreq = DIMMFREQ_200;
		DramAttr->DramCyc = 1000;
	}
	//if set the frequence mannul
	PRINT_DEBUG_MEM("Dram Frequency:");
	PRINT_DEBUG_MEM_HEX16(DramAttr->DramFreq);
	PRINT_DEBUG_MEM(" \r");
}
