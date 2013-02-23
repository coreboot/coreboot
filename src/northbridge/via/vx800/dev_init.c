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

void DRAMSetVRNum(DRAM_SYS_ATTR *DramAttr, u8 PhyRank /* physical rank */,
		  u8 VirRank /* virtual rank */, BOOLEAN Enable);
void SetEndingAddr(DRAM_SYS_ATTR *DramAttr, u8 VirRank /* Ending address
                   register number indicator (INDEX */, INT8 Value /* (value)
                   add or subtract value to this and after banks. */);
void InitDDR2CHA(DRAM_SYS_ATTR *DramAttr);
void InitDDR2CHB(DRAM_SYS_ATTR *DramAttr);
void InitDDR2CHC(DRAM_SYS_ATTR *DramAttr);

CB_STATUS VerifyChc(void);

/*===================================================================
Function   : DRAMRegInitValue()
Precondition :
Input      :
		   DramAttr:  pointer point to  DRAM_SYS_ATTR  which consist the DDR and Dimm information
		                    in MotherBoard
Output     : Void
Purpose   : Set necessary register before DRAM initialize
===================================================================*/

static const u8 DramRegTbl[][3] = {
	/* Reg AND   OR */
	{0x50, 0x11, 0xEE},	// DDR default MA7 for DRAM init
	{0x51, 0x11, 0x60},	// DDR default MA3 for CHB init
	{0x52, 0x00, 0x33},	// DDR use BA0=M17, BA1=M18,
	{0x53, 0x00, 0x3F},	// DDR    BA2=M19

	{0x54, 0x00, 0x00},	// default PR0=VR0; PR1=VR1
	{0x55, 0x00, 0x00},	// default PR2=VR2; PR3=VR3
	{0x56, 0x00, 0x00},	// default PR4=VR4; PR5=VR5
	{0x57, 0x00, 0x00},	// default PR4=VR4; PR5=VR5

	{0x60, 0x00, 0x00},	// disable fast turn-around
	{0x65, 0x00, 0xD9},	// AGP timer = 0XD; Host timer = 8;
	{0x66, 0x00, 0x88},	// DRAMC Queue Size = 4; park at the last bus
				// owner,Priority promotion timer = 8
	{0x68, 0x00, 0x0C},
	{0x69, 0xF0, 0x04},	// set RX69[3:0]=0000b
	{0x6A, 0x00, 0x00},	// refresh counter
	{0x6E, 0xF8, 0x80},	// must set 6E[7], or else DDR2 probe test
				// will fail
	/*
	 * In here, we not set RX70~RX74, because we just init DRAM but no
	 * need R/W DRAM, when we check DQS input/output delay, then we need
	 * R/W DRAM.
	 */

	// {0x79, 0x00, 0x8F },
	{0x85, 0x00, 0x00},
	// {0x90, 0x87, 0x78 },
	// {0x91, 0x00, 0x46 },
	{0x40, 0x00, 0x00},

	{0, 0, 0}
};

void DRAMRegInitValue(DRAM_SYS_ATTR *DramAttr)
{
	u8 Idx, CL, Data;

	for (Idx = 0; DramRegTbl[Idx][0] != 0; Idx++) {
		Data = pci_read_config8(MEMCTRL, DramRegTbl[Idx][0]);
		Data &= DramRegTbl[Idx][1];
		Data |= DramRegTbl[Idx][2];
		pci_write_config8(MEMCTRL, DramRegTbl[Idx][0], Data);
	}

	Data = 0x80;
	pci_write_config8(PCI_DEV(0, 0, 4), 0xa3, Data);

	// Set DRAM controller mode. */
	Data = pci_read_config8(MEMCTRL, 0x6c);
	Data &= 0xFB;
	if (ENABLE_CHC == 0) {
		Data |= 0x4;	/* Only CHA 64 bit mode */
		pci_write_config8(MEMCTRL, 0x6c, Data);
	} else {
		Data |= 0x0;	/* CHA + CHC */
		pci_write_config8(MEMCTRL, 0x6c, Data);

		// Data = 0xAA;
		// pci_write_config8(MEMCTRL, 0xb1, Data);

		// set CHB DQSB input delay, or else will meet error which
		// is some byte is right but another bit is error.
		Data = pci_read_config8(MEMCTRL, 0xff);
		Data = (Data & 0x03) | 0x3D;
		pci_write_config8(MEMCTRL, 0xff, Data);

		// enable CHC  RXDB[7]
		// Data = pci_read_config8(MEMCTRL, 0xdb);
		// Data = (Data & 0x7F) | 0x80;
		// pci_write_config8(MEMCTRL, 0xdb, Data);

		// rx62[2:0], CHA and CHB CL
		Data = pci_read_config8(MEMCTRL, 0x62);
		CL = Data & 0x07;

		// If CL = 6, so I set CHB CL = 5 default.
		if (CL >= 4)
			CL = 3;

		/* Set CHC Read CL rxDC[6:7]. */
		Data = pci_read_config8(MEMCTRL, 0xdc);
		Data = (Data & 0x3F) | (CL << 6);
		pci_write_config8(MEMCTRL, 0xdc, Data);

		/* Set CHC write CL rxDF[6:7]. */
		Data = pci_read_config8(MEMCTRL, 0xdf);
		Data = (Data & 0x3F) | (CL << 6);
		pci_write_config8(MEMCTRL, 0xdf, Data);

		/* Set CHC ODT RxDC[5:0] */
		Data = pci_read_config8(MEMCTRL, 0xdc);
		Data = (Data & 0xC0) | 0x03;
		pci_write_config8(MEMCTRL, 0xdc, Data);

		/* Set column type RXDD[6] and enable ODT PAD RXDD[7]. */
		Data = pci_read_config8(MEMCTRL, 0xdd);
		Data |= 0x80;
		Idx = DramAttr->DimmInfo[2].SPDDataBuf[SPD_SDRAM_COL_ADDR];
		if ((Idx & 0x0F) == 10)
			Data |= 0x40;	/* MA9~MA0 */
		else
			Data &= 0xBF;	/* MA8~MA0 */
		pci_write_config8(MEMCTRL, 0xdd, Data);
	}

	// Disable Read DRAM fast ready ;Rx51[7]
	// Disable Read Around Write    ;Rx51[6]

	// Disable Consecutive Read     ;RX52[1:0]
	// Disable Speculative Read
}

/*===================================================================
Function   : DRAMInitializeProc()
Precondition :
Input      :
		   DramAttr:  pointer point to  DRAM_SYS_ATTR  which consist the DDR and Dimm information
		                    in MotherBoard
Output     : Void
Purpose   : DRAM initialize according to the bios porting guid
===================================================================*/

#define EXIST_TEST_PATTERN		0x55555555
#define NOT_EXIST_TEST_PATTERN		0xAAAAAAAA

static BOOLEAN ChkForExistLowBank(void)
{
	u32 *Address, data32;

	/* Check pattern */
	Address = (u32 *) 8;
	*Address = EXIST_TEST_PATTERN;
	Address = (u32 *) 4;
	*Address = EXIST_TEST_PATTERN;

	// _asm {WBINVD}
	WaitMicroSec(100);
	Address = (u32 *) 8;
	data32 = *Address;
	if (data32 != EXIST_TEST_PATTERN)
		return FALSE;
	Address = (u32 *) 4;
	data32 = *Address;
	if (data32 != EXIST_TEST_PATTERN)
		return FALSE;

	/* Check not Pattern */
	Address = (u32 *) 8;
	*Address = NOT_EXIST_TEST_PATTERN;
	Address = (u32 *) 4;
	*Address = NOT_EXIST_TEST_PATTERN;
	// _asm {WBINVD}
	WaitMicroSec(100);

	Address = (u32 *) 8;
	data32 = *Address;
	if (data32 != (u32) (NOT_EXIST_TEST_PATTERN))
		return FALSE;
	Address = (u32 *) 4;
	data32 = *Address;
	if (data32 != (u32) (NOT_EXIST_TEST_PATTERN))
		return FALSE;

	return TRUE;
}

void DRAMInitializeProc(DRAM_SYS_ATTR *DramAttr)
{
	u8 shift, idx;
	BOOLEAN Status;

	shift = 1;
	for (idx = 0; idx < MAX_RANKS; idx++) {
		if ((DramAttr->RankPresentMap & shift) != 0) {
			/*
			 * Set VR# to physical rank indicated = PR + physical
			 * rank enable bit.
			 */
			DRAMSetVRNum(DramAttr, idx, idx, TRUE);
			SetEndingAddr(DramAttr, idx, 0x10); /* Assume 1G size */
			if (idx < 4) /* CHA init */
				InitDDR2CHA(DramAttr);	// temp wjb 2007/1 only for compiling
			// in the function InitDDR2,the parameter is no need
			Status = ChkForExistLowBank();
			if (Status == TRUE) {
				PRINT_DEBUG_MEM(" S\r");
			} else {
				PRINT_DEBUG_MEM(" F\r");
			}

			/*
			 * Set VR# to physical rank indicated = 00h + physical
			 * rank enable bit.
			 */
			DRAMSetVRNum(DramAttr, idx, 0, FALSE);
			SetEndingAddr(DramAttr, idx, -16);
		}
		shift <<= 1;
	}
	if (ENABLE_CHC)
		InitDDR2CHC(DramAttr);

}

/*===================================================================
Function   : DRAMSetVRNUM()
Precondition :
Input      :
		   DramAttr:  pointer point to  DRAM_SYS_ATTR  which consist the DDR and Dimm information
		                    in MotherBoard
		  PhyRank:   Physical Rank number
		  VirRank:    Virtual Rank number
		  Enable:      Enable/Disable Physical Rank
Output     : Void
Purpose   : Set virtual rank number for physical rank
                 Program the specific physical rank with specific virtual rank number
                 Program when necessary, otherwise don't touch the pr-vr-mapping registers
===================================================================*/

void DRAMSetVRNum(DRAM_SYS_ATTR *DramAttr, u8 PhyRank /* physical rank */,
		  u8 VirRank /* virtual rank */, BOOLEAN Enable)
{
	u8 Data, AndData, OrData;

	Data = pci_read_config8(MEMCTRL, (0x54 + (PhyRank >> 1)));

	OrData = 0;
	if (Enable)
		OrData |= 0x08;
	OrData |= VirRank;
	if ((PhyRank & 0x01) == 0x00) {
		AndData = 0x0F;	// keep the value of odd rank on PR # is even(keep 1,3,5,7)
		OrData <<= 4;	// VR #, value to be set
	} else {
		AndData = 0xF0;	// keep the value of even rank on PR # is odd(keep 0,2,4,6)
	}

	Data &= AndData;
	Data |= OrData;
	pci_write_config8(MEMCTRL, (0x54 + (PhyRank >> 1)), Data);
}

/*===================================================================
Function   : SetEndingAddr()
Precondition :
Input      :
		   DramAttr:  pointer point to  DRAM_SYS_ATTR  which consist the DDR and Dimm information
		                    in MotherBoard
		  VirRank:    Virtual Rank number
		  Value:       (value) add or subtract value to this and after banks
Output     : Void
Purpose   : Set ending address of virtual rank specified by VirRank
===================================================================*/

void SetEndingAddr(DRAM_SYS_ATTR *DramAttr, u8 VirRank /* ending address
		   register number indicator (INDEX */, INT8 Value /* (value)
		   add or subtract value to this and after banks */) {
	u8 Data;

	/* Read register,Rx40-Rx47(0,1,2,3,4,5,6,7) and set ending address. */
	Data = pci_read_config8(MEMCTRL, 0x40 + VirRank);
	Data = (u8) (Data + Value);
	pci_write_config8(MEMCTRL, 0x40 + VirRank, Data);

	/* Program the virank's begining address to zero. */
	Data = 0x00;
	pci_write_config8(MEMCTRL, 0x48 + VirRank, Data);
}

/*===================================================================
Function   : InitDDR2()
Precondition :
Input      :
		   DramAttr:  pointer point to  DRAM_SYS_ATTR  which consist the DDR and Dimm information
		                    in MotherBoard
Output     : Void
Purpose   : Initialize DDR2 by standard sequence
===================================================================*/

//                               DLL:         Enable                              Reset
static const u32 CHA_MRS_DLL_150[2] = { 0x00020200, 0x00000800 };	// with 150 ohm (A17=1, A9=1), (A11=1)(cpu address)
static const u32 CHA_MRS_DLL_75[2] = { 0x00020020, 0x00000800 };	// with 75 ohm  (A17=1, A5=1), (A11=1)(cpu address)

//               CPU(DRAM)
// { DLL: Enable. A17(BA0)=1 and A3(MA0)=0 }
// { DLL: reset.  A11(MA8)=1 }
//
//                      DDR2                    CL=2    CL=3    CL=4    CL=5     CL=6(Burst type=interleave)(WR fine tune in code)
static const u16 CHA_DDR2_MRS_table[5] = { 0x0150, 0x01D0, 0x0250, 0x02D0, 0x350 };	// BL=4 ;Use 1X-bandwidth MA table to init DRAM

//                                                       MA11        MA10(AP)      MA9
#define CHA_MRS_DDR2_TWR2		(0 << 13) + (0 << 20) + (1 << 12)	// Value = 001000h
#define CHA_MRS_DDR2_TWR3		(0 << 13) + (1 << 20) + (0 << 12)	// Value = 100000h
#define CHA_MRS_DDR2_TWR4		(0 << 13) + (1 << 20) + (1 << 12)	// Value = 101000h
#define CHA_MRS_DDR2_TWR5		(1 << 13) + (0 << 20) + (0 << 12)	// Value = 002000h
#define CHA_MRS_DDR2_TWR6		(1 << 13) + (0 << 20) + (1 << 12)	// Value = 003000h

//                              DDR2             Twr=2                  Twr=3              Twr=4                  Twr=5
static const u32 CHA_DDR2_Twr_table[5] = {
	CHA_MRS_DDR2_TWR2, CHA_MRS_DDR2_TWR3, CHA_MRS_DDR2_TWR4,
	CHA_MRS_DDR2_TWR5, CHA_MRS_DDR2_TWR6
};

#define CHA_OCD_Exit_150ohm		0x20200	// EMRS(1), BA0=1, MA9=MA8=MA7=0,MA6=1,MA2=0 (DRAM bus address)
//                A17=1, A12=A11=A10=0,A9=1 ,A5=0  (CPU address)
#define CHA_OCD_Default_150ohm		0x21E00	// EMRS(1), BA0=1, MA9=MA8=MA7=1,MA6=1,MA2=0 (DRAM bus address)
//               A17=1, A12=A11=A10=1,A9=1 ,A5=0  (CPU address)
#define CHA_OCD_Exit_75ohm		0x20020	// EMRS(1), BA0=1, MA9=MA8=MA7=0,MA6=0,MA2=1 (DRAM bus address)
//              A17=1, A12=A11=A10=0,A9=0 ,A5=1  (CPU address)
#define CHA_OCD_Default_75ohm		0x21C20	// EMRS(1), BA0=1, MA9=MA8=MA7=1,MA6=0,MA2=1 (DRAM bus address)
//              A17=1, A12=A11=A10=1,A9=0 ,A5=1  (CPU address)

void InitDDR2CHA(DRAM_SYS_ATTR *DramAttr)
{
	u8 Data, Reg6BVal, Idx, CL, BL, Twr, DimmNum;
	u32 AccessAddr;

	/* Step 2 */
	/* Disable bank paging and multi page. */
	Data = pci_read_config8(MEMCTRL, 0x69);
	Data &= ~0x03;
	pci_write_config8(MEMCTRL, 0x69, Data);

	Reg6BVal = pci_read_config8(MEMCTRL, 0x6b);
	Reg6BVal &= ~0x07;

	/* Step 3 */
	/* At least one NOP cycle will be issued after the 1m sec device
	 * deselect.
	 */
	Data = Reg6BVal | 0x01;
	pci_write_config8(MEMCTRL, 0x6b, Data);

	/* Step 4 */
	/* Read a double word from any address of the DIMM. */
	DimmRead(0x0);

	/* Step 5 */
	/*
	 * A minimum pause of 200u sec will be provided after the NOP.
	 * - <<<    reduce BOOT UP time >>> -
	 * Loop 200us
	 */
	for (Idx = 0; Idx < 0x10; Idx++)
		WaitMicroSec(100);

	// Step 6.
	// Precharge all (PALL) will be issued to the DDR.
	Data = Reg6BVal | 0x02;
	pci_write_config8(MEMCTRL, 0x6b, Data);

	// Step7.
	// Read a double word from any address of the DIMM
	DimmRead(0x0);

	// Step 8.
	// MSR Eable will be issued to the DDR
	Data = Reg6BVal | 0x03;
	pci_write_config8(MEMCTRL, 0x6b, Data);

	/* Step 9, 10.
	 *
	 * Check ODT value for EMRS(1) command according to ODTLookUp_TBL
	 * in driving_setting.c if there is one DIMM in MB's one channel,
	 * the DDR2's ODT is 150ohm if there is two DIMM in MB's one channel,
	 * the DDR2's ODT is 75 ohm.
	 */
	DimmNum = DramAttr->DimmNumChA;

	if (DimmNum == 1) { /* DDR's ODT is 150ohm */
		AccessAddr = (u32) CHA_MRS_DLL_150[0];
		DimmRead(AccessAddr); /* Issue EMRS DLL Enable. */
		PRINT_DEBUG_MEM("Step 9 Address ");
		PRINT_DEBUG_MEM_HEX32(AccessAddr);
		PRINT_DEBUG_MEM("\r");

		AccessAddr = (u32) CHA_MRS_DLL_150[1];
		DimmRead(AccessAddr); /* Issue MRS DLL Reset. */
		PRINT_DEBUG_MEM("Step 10 Address ");
		PRINT_DEBUG_MEM_HEX32(AccessAddr);
		PRINT_DEBUG_MEM("\r");
	} else if (DimmNum == 2) { /* DDR's ODT is 75ohm */
		AccessAddr = (u32) CHA_MRS_DLL_75[0];
		DimmRead(AccessAddr); /* Issue EMRS DLL Enable. */
		AccessAddr = (u32) CHA_MRS_DLL_75[1];
		DimmRead(AccessAddr); /* Issue MRS DLL Reset. */
	} else {
		PRINT_DEBUG_MEM("Dimm NUM ERROR:");
		PRINT_DEBUG_MEM_HEX8(DimmNum);
		PRINT_DEBUG_MEM("\r");
	}

	/* Step 11. Precharge all (PALL) will be issued to the DDR. */
	Data = Reg6BVal | 0x02;
	pci_write_config8(MEMCTRL, 0x6b, Data);

	/* Step 12. Read a double word from any address of the DIMM. */
	DimmRead(0x0);

	/* Step 13. Execute 8 CBR refresh. */
	Data = Reg6BVal | 0x04;
	pci_write_config8(MEMCTRL, 0x6b, Data);

	// issue 14,15 , 16
	//reads and wait 100us between each read
	for (Idx = 0; Idx < 8; Idx++) {
		DimmRead(0x0);
		WaitMicroSec(100);
	}

	/* Step 17. Enable MRS for MAA. */
	Data = Reg6BVal | 0x03;
	pci_write_config8(MEMCTRL, 0x6b, Data);

	/*
	 * Step 18. The SDRAM parameters (Burst Length, CAS# Latency,
	 * Write recovery etc.)
	 */

	/* Burst Length: really offset Rx6c[3] */
	Data = pci_read_config8(MEMCTRL, 0x6c);
	BL = (Data & 0x08) >> 3;

	/* CL: really offset RX62[2:0] */
	Data = pci_read_config8(MEMCTRL, 0x62);
	CL = Data & 0x03;

	AccessAddr = (u32) (CHA_DDR2_MRS_table[CL]);
	if (BL)
		AccessAddr += 8;

	/* Write recovery: really offset Rx63[7-5] */
	Data = pci_read_config8(MEMCTRL, 0x63);
	Twr = (Data & 0xE0) >> 5;

	AccessAddr += CHA_DDR2_Twr_table[Twr];
	// AccessAddr = 0x1012D8;
	DimmRead(AccessAddr); /* Set MRS command. */
	PRINT_DEBUG_MEM("Step 18 Address");
	PRINT_DEBUG_MEM_HEX32(AccessAddr);
	PRINT_DEBUG_MEM("\r");

	/* Step 19, 20 */
	if (DimmNum == 1) { /* DDR's ODT is 150ohm */
		AccessAddr = (u32) CHA_OCD_Default_150ohm;
		DimmRead(AccessAddr);	/* Issue EMRS OCD Default. */
		PRINT_DEBUG_MEM("Step 19 Address ");
		PRINT_DEBUG_MEM_HEX32(AccessAddr);
		PRINT_DEBUG_MEM("\r");

		AccessAddr = (u32) CHA_OCD_Exit_150ohm;
		DimmRead(AccessAddr); /* Issue EMRS OCD Calibration Mode Exit. */
		PRINT_DEBUG_MEM("Step 20 Address ");
		PRINT_DEBUG_MEM_HEX32(AccessAddr);
		PRINT_DEBUG_MEM("\r");
	} else if (DimmNum == 2) { /* DDR's ODT is 75ohm */
		AccessAddr = (u32) CHA_OCD_Default_75ohm;
		DimmRead(AccessAddr); /* Issue EMRS OCD Default. */
		AccessAddr = (u32) CHA_OCD_Exit_75ohm;
		DimmRead(AccessAddr); /* Issue EMRS OCD Calibration Mode Exit. */
	} else {
		PRINT_DEBUG_MEM("Dimm NUM ERROR: ");
		PRINT_DEBUG_MEM_HEX8(DimmNum);
		PRINT_DEBUG_MEM("\r");
	}

	/*
	 * Step 21. After MRS the device should be ready for full
	 * functionality within 3 clocks after Tmrd is met.
	 */
	Data = Reg6BVal;
	pci_write_config8(MEMCTRL, 0x6b, Data);

	/* Enable bank paging and multi page. */
	Data = pci_read_config8(MEMCTRL, 0x69);
	Data |= 0x03;
	pci_write_config8(MEMCTRL, 0x69, Data);
}

/*===================================================================
Function   : InitDDR2_CHB()
Precondition :
Input      :
		   DramAttr:  pointer point to  DRAM_SYS_ATTR  which consist the DDR and Dimm information
		                    in MotherBoard
Output     : Void
Purpose   : Initialize DDR2 of CHB by standard sequence
Reference  :
===================================================================*/
/*//		                 DLL:	      Enable				  Reset
static const  u32 CHB_MRS_DLL_150[2] =	{ 0x00020200 | (1 << 20), 0x00000800 };	// with 150 ohm (A17=1, A9=1), (A11=1)(cpu address)
//u32 CHB_MRS_DLL_75[2]  =	{ 0x00020020 | (1 << 20), 0x00000800 };	// with 75 ohm  (A17=1, A5=1), (A11=1)(cpu address)
//		 CPU(DRAM)
// { DLL: Enable. A17(BA0)=1 and A3(MA0)=0 }
// { DLL: reset.  A11(MA8)=1 }
//
//                      DDR2 			CL=2	CL=3	CL=4	CL=5 	(Burst type=interleave)(WR fine tune in code)
static const  u16 CHB_DDR2_MRS_table[4] ={ 0x0150, 0x01D0, 0x0250, 0x02D0 };	// BL=4 ;Use 1X-bandwidth MA table to init DRAM

//			 				 MA11	     MA10(AP)      MA9
#define CHB_MRS_DDR2_TWR2		(0 << 13) + (0 << 20) + (1 << 12)	// Value = 001000h
#define CHB_MRS_DDR2_TWR3		(0 << 13) + (1 << 20) + (0 << 12)	// Value = 100000h
#define CHB_MRS_DDR2_TWR4		(0 << 13) + (1 << 20) + (1 << 12)	// Value = 101000h
#define CHB_MRS_DDR2_TWR5		(1 << 13) + (0 << 20) + (0 << 12)	// Value = 002000h
#define CHB_MRS_DDR2_TWR6		(1 << 13) + (0 << 20) + (1 << 12)	// Value = 003000h

//				DDR2 		 Twr=2			Twr=3		   Twr=4		  Twr=5
static const u32 CHB_DDR2_Twr_table[5] = { CHB_MRS_DDR2_TWR2,	CHB_MRS_DDR2_TWR3, CHB_MRS_DDR2_TWR4, CHB_MRS_DDR2_TWR5, CHB_MRS_DDR2_TWR6 };

#define CHB_OCD_Exit_150ohm		0x20200 | (1 << 20) 		 // EMRS(1), BA0=1, MA9=MA8=MA7=0,MA6=1,MA2=0 (DRAM bus address)
//	          A17=1, A12=A11=A10=0,A9=1 ,A5=0  (CPU address)
#define CHB_OCD_Default_150ohm	0x21E00 | (1 << 20)     	// EMRS(1), BA0=1, MA9=MA8=MA7=1,MA6=1,MA2=0 (DRAM bus address)
//	         A17=1, A12=A11=A10=1,A9=1 ,A5=0  (CPU address)
//#define CHB_OCD_Exit_75ohm		0x20020 | (1 << 20) 	      // EMRS(1), BA0=1, MA9=MA8=MA7=0,MA6=0,MA2=1 (DRAM bus address)
//	        A17=1, A12=A11=A10=0,A9=0 ,A5=1  (CPU address)
//#define CHB_OCD_Default_75ohm	0x21C20 | (1 << 20)      // EMRS(1), BA0=1, MA9=MA8=MA7=1,MA6=0,MA2=1 (DRAM bus address)
//	        A17=1, A12=A11=A10=1,A9=0 ,A5=1  (CPU address)
void InitDDR2CHB(
               DRAM_SYS_ATTR          *DramAttr
             )

{
    u8     Data;
    u8	    Idx, CL, BL, Twr;
    u32   AccessAddr;

    Data = 0x80;
    pci_write_config8(MEMCTRL, 0x54, Data);

    // step3.
    //disable bank paging and multi page
    Data=pci_read_config8(MEMCTRL, 0x69);
    Data &= ~0x03;
    pci_write_config8(MEMCTRL, 0x69, Data);

    Data=pci_read_config8(MEMCTRL, 0xd3);
    Data |= 0x80;
    pci_write_config8(MEMCTRL, 0xd3, Data);

    //step 4. Initialize CHB begin
    Data=pci_read_config8(MEMCTRL, 0xd3);
    Data |= 0x40;
    pci_write_config8(MEMCTRL, 0xd3, Data);

    //Step 5. NOP command enable
    Data=pci_read_config8(MEMCTRL, 0xd7);
    Data &= 0xC7;
    Data  |= 0x08;
    pci_write_config8(MEMCTRL, 0xd7, Data);

    //Step 6.  issue a nop cycle,RegD3[7]  0 -> 1
    Data=pci_read_config8(MEMCTRL, 0xd3);
    Data &= 0x7F;
    pci_write_config8(MEMCTRL, 0xd3, Data);
    Data |=  0x80;
    pci_write_config8(MEMCTRL, 0xd3, Data);

    // Step 7.
    // A minimum pause of 200u sec will be provided after the NOP.
    // - <<<	reduce BOOT UP time >>>	-
    // Loop 200us
    for (Idx = 0; Idx < 0x10; Idx++)
        WaitMicroSec(10);

    // Step 8.
    // all banks precharge command enable
    Data=pci_read_config8(MEMCTRL, 0xd7);
    Data &= 0xC7;
    Data |= 0x10;
    pci_write_config8(MEMCTRL, 0xd7, Data);

   //step 9. issue a precharge all cycle,RegD3[7]  0 -> 1
    Data=pci_read_config8(MEMCTRL, 0xd3);
    Data &= 0x7F;
    pci_write_config8(MEMCTRL, 0xd3, Data);
    Data |=  0x80;
    pci_write_config8(MEMCTRL, 0xd3, Data);

   //step10. EMRS enable
    Data=pci_read_config8(MEMCTRL, 0xd7);
    Data &= 0xC7;
    Data |= 0x18;
    pci_write_config8(MEMCTRL, 0xd7, Data);

    Data=pci_read_config8(MEMCTRL, 0xd3);
    Data &= 0xC7;
    Data |= 0x08;
    pci_write_config8(MEMCTRL, 0xd3, Data);

    //step11. EMRS DLL Enable and Disable DQS
    AccessAddr = CHB_MRS_DLL_150[0] >> 3;
    Data =(u8) (AccessAddr & 0xff);
    pci_write_config8(MEMCTRL, 0xd9, Data);

    Data = (u8)((AccessAddr & 0xff00) >> 8);
    pci_write_config8(MEMCTRL, 0xda, Data);

    Data=pci_read_config8(MEMCTRL, 0xd7);
    Data &= 0xF9;
    Data |= (u8)((AccessAddr & 0x30000) >> 15);
    pci_write_config8(MEMCTRL, 0xd7, Data);

    //step12.  issue EMRS cycle
    Data=pci_read_config8(MEMCTRL, 0xd3);
    Data &= 0x7F;
    pci_write_config8(MEMCTRL, 0xd3, Data);
    Data |=  0x80;
    pci_write_config8(MEMCTRL, 0xd3, Data);

    //step13. MSR enable
    Data=pci_read_config8(MEMCTRL, 0xd7);
    Data &= 0xC7;
    Data |= 0x18;
    pci_write_config8(MEMCTRL, 0xd7, Data);

    Data=pci_read_config8(MEMCTRL, 0xd3);
    Data &= 0xC7;
    Data |= 0x00;
    pci_write_config8(MEMCTRL, 0xd3, Data);

    //step 14. MSR DLL Reset
    AccessAddr = CHB_MRS_DLL_150[1] >> 3;
    Data =(u8) (AccessAddr & 0xff);
    pci_write_config8(MEMCTRL, 0xd9, Data);

    Data = (u8)((AccessAddr & 0xff00) >> 8);
    pci_write_config8(MEMCTRL, 0xda, Data);

    Data=pci_read_config8(MEMCTRL, 0xd7);
    Data &= 0xF9;
    Data |= (u8)((AccessAddr & 0x30000) >> 15);
    pci_write_config8(MEMCTRL, 0xd7, Data);

    //step15.  issue MRS cycle
    Data=pci_read_config8(MEMCTRL, 0xd3);
    Data &= 0x7F;
    pci_write_config8(MEMCTRL, 0xd3, Data);
    Data |=  0x80;
    pci_write_config8(MEMCTRL, 0xd3, Data);

    //clear the address
    Data = 0x00;
    pci_write_config8(MEMCTRL, 0xda, Data);

     //step16.  all banks precharge command enable
    Data=pci_read_config8(MEMCTRL, 0xd7);
    Data &= 0xC7;
    Data |= 0x10;
    pci_write_config8(MEMCTRL, 0xd7, Data);


   // step17. issue precharge all cycle
    Data=pci_read_config8(MEMCTRL, 0xd3);
    Data &= 0x7F;
    pci_write_config8(MEMCTRL, 0xd3, Data);
    Data |=  0x80;
    pci_write_config8(MEMCTRL, 0xd3, Data);

    //step18.  CBR cycle enable
    Data=pci_read_config8(MEMCTRL, 0xd7);
    Data &= 0xC7;
    Data |= 0x20;
    pci_write_config8(MEMCTRL, 0xd7, Data);

    //step 19.20.21
    //repeat issue 8 CBR cycle, between each cycle stop 100us
    for (Idx = 0; Idx < 8; Idx++)
    {
         // issue CBR cycle
    Data=pci_read_config8(MEMCTRL, 0xd3);
    Data &= 0x7F;
    pci_write_config8(MEMCTRL, 0xd3, Data);
    Data |=  0x80;
    pci_write_config8(MEMCTRL, 0xd3, Data);

    WaitMicroSec(200);
    }

    //step22. MSR enable
    Data=pci_read_config8(MEMCTRL, 0xd7);
    Data &= 0xC7;
    Data |= 0x18;
    pci_write_config8(MEMCTRL, 0xd7, Data);

    Data=pci_read_config8(MEMCTRL, 0xd3);
    Data &= 0xC7;
    Data |= 0x00;
    pci_write_config8(MEMCTRL, 0xd3, Data);


    //the SDRAM parameters.(Burst Length, CAS# Latency , Write recovery etc.)
    //-------------------------------------------------------------
    //Burst Length : really offset Rx6c[1]
    Data=pci_read_config8(MEMCTRL, 0x6C);
    BL = (Data & 0x02) >> 1;

    // CL = really offset RX62[2:0]
    Data=pci_read_config8(MEMCTRL, 0x62);
    CL = Data & 0x03;

    AccessAddr  = (u32)(CHB_DDR2_MRS_table[CL]);
    if (BL)
    {
        AccessAddr += 8;
    }

    //Write recovery  : really offset Rx63[7:5]
    Data=pci_read_config8(MEMCTRL, 0x63);
    Twr = (Data & 0xE0) >> 5;

    AccessAddr += CHB_DDR2_Twr_table[Twr];
    //MSR Address use addr[20:3]
    AccessAddr >>= 3;

   //step 23. MSR command
    Data = (u8)(AccessAddr & 0xFF);
    pci_write_config8(MEMCTRL, 0xD9, Data);

    Data = (u8)((AccessAddr & 0xFF00) >> 8);
    pci_write_config8(MEMCTRL, 0xda, Data);

    Data=pci_read_config8(MEMCTRL, 0xd7);
    Data &= 0xF9;
    Data |= (u8)(((AccessAddr & 0x30000)>>16) << 1);
    pci_write_config8(MEMCTRL, 0xd7, Data);

     //step 24.  issue MRS cycle
    Data=pci_read_config8(MEMCTRL, 0xd3);
    Data &= 0x7F;
    pci_write_config8(MEMCTRL, 0xd3, Data);
    Data |=  0x80;
    pci_write_config8(MEMCTRL, 0xd3, Data);

    //step 25. EMRS enable
    Data=pci_read_config8(MEMCTRL, 0xd7);
    Data &= 0xC7;
    Data |= 0x18;
    pci_write_config8(MEMCTRL, 0xd7, Data);

    Data=pci_read_config8(MEMCTRL, 0xd3);
    Data &= 0xC7;
    Data |= 0x08;
    pci_write_config8(MEMCTRL, 0xd3, Data);


    //step 26. OCD default
     AccessAddr = (CHB_OCD_Default_150ohm) >> 3;
    Data =(u8) (AccessAddr & 0xff);
    pci_write_config8(MEMCTRL, 0xd9, Data);

    Data = (u8)((AccessAddr & 0xff00) >> 8);
    pci_write_config8(MEMCTRL, 0xda, Data);

    Data=pci_read_config8(MEMCTRL, 0xd7);
    Data &= 0xF9;
    Data |= (u8)((AccessAddr & 0x30000) >> 15);
    pci_write_config8(MEMCTRL, 0xd7, Data);

    //step 27.  issue EMRS cycle
    Data=pci_read_config8(MEMCTRL, 0xd3);
    Data &= 0x7F;
    pci_write_config8(MEMCTRL, 0xd3, Data);
    Data |=  0x80;
    pci_write_config8(MEMCTRL, 0xd3, Data);

     //step 25. EMRS enable
    Data=pci_read_config8(MEMCTRL, 0xd7);
    Data &= 0xC7;
    Data |= 0x18;
    pci_write_config8(MEMCTRL, 0xd7, Data);

    Data=pci_read_config8(MEMCTRL, 0xd3);
    Data &= 0xC7;
    Data |= 0x08;
    pci_write_config8(MEMCTRL, 0xd3, Data);

    //step 28. OCD Exit
     AccessAddr = (CHB_OCD_Exit_150ohm) >> 3;
     Data =(u8) (AccessAddr & 0xff);
    pci_write_config8(MEMCTRL, 0xd9, Data);

    Data = (u8)((AccessAddr & 0xff00) >> 8);
    pci_write_config8(MEMCTRL, 0xda, Data);

    Data=pci_read_config8(MEMCTRL, 0xd7);
    Data &= 0xF9;
    Data |= (u8)((AccessAddr & 0x30000) >> 15);
    pci_write_config8(MEMCTRL, 0xd7, Data);

     //step 29. issue EMRS cycle
    Data=pci_read_config8(MEMCTRL, 0xd3);
    Data &= 0x7F;
    pci_write_config8(MEMCTRL, 0xd3, Data);
    Data |=  0x80;
    pci_write_config8(MEMCTRL, 0xd3, Data);

    //clear  all the address
    Data = 0x00;
    pci_write_config8(MEMCTRL, 0xd9, Data);

    Data = 0x00;
    pci_write_config8(MEMCTRL, 0xda, Data);

    Data=pci_read_config8(MEMCTRL, 0xd7);
    Data &= 0xF9;
    pci_write_config8(MEMCTRL, 0xd7, Data);

    //step 30. normal SDRAM Mode
    Data=pci_read_config8(MEMCTRL, 0xd7);
    Data &= 0xC7;
    Data |= 0x00;
    pci_write_config8(MEMCTRL, 0xd7, Data);

    Data=pci_read_config8(MEMCTRL, 0xd3);
    Data &= 0xC7;
    Data |= 0x00;
    pci_write_config8(MEMCTRL, 0xd3, Data);

    //step 31.  exit the initialization mode
    Data=pci_read_config8(MEMCTRL, 0xd3);
    Data &= 0xBF;
    pci_write_config8(MEMCTRL, 0xd3, Data);


    //step 32. Enable bank paging and multi page
    Data=pci_read_config8(MEMCTRL, 0x69);
    Data |= 0x03;
    pci_write_config8(MEMCTRL, 0x69, Data);
}
*/

/*===================================================================
Function   : InitDDR2CHC()
Precondition :
Input      :
		   DramAttr:  pointer point to  DRAM_SYS_ATTR  which consist the DDR and Dimm information
		                    in MotherBoard
Output     : Void
Purpose   : Initialize DDR2 of CHC by standard sequence
Reference  :
===================================================================*/
//                      DDR2                 CL=2          CL=3 CL=4   CL=5     (Burst type=interleave)(WR fine tune in code)
static const u16 CHC_MRS_table[4] = { 0x22B, 0x23B, 0x24B, 0x25B };	// Use 1X-bandwidth MA table to init DRAM

void InitDDR2CHC(DRAM_SYS_ATTR *DramAttr)
{
	u8 Data, Idx, CL, Twr;
	u32 AccessAddr;
	CB_STATUS Status;

	/* Step 3. Clear RxDF[2] to disable Tri-state output. */
	Data = pci_read_config8(MEMCTRL, 0xdf);
	Data &= 0xFB;
	pci_write_config8(MEMCTRL, 0xdf, Data);

	/*
	 * Step 4. Enable the initialization mode of DRAM Controller C with
	 * NB's PLL clock.
	 */
	Data = pci_read_config8(MEMCTRL, 0xdb);
	Data |= 0x60;
	pci_write_config8(MEMCTRL, 0xdb, Data);

	/* Step 5. NOP command enable. */
	Data = pci_read_config8(MEMCTRL, 0xdb);
	Data &= 0xE3;
	Data |= 0x00;
	pci_write_config8(MEMCTRL, 0xdb, Data);

	/* Step 6. Issue a nop cycle, RegDB[1] 0 -> 1. */
	Data = pci_read_config8(MEMCTRL, 0xdb);
	Data |= 0x2;
	pci_write_config8(MEMCTRL, 0xdb, Data);
	Data &= 0xFD;
	pci_write_config8(MEMCTRL, 0xdb, Data);

	/*
	 * Step 7.
	 * A minimum pause of 200u sec will be provided after the NOP.
	 * - <<<    reduce BOOT UP time >>> -
	 * Loop 200us
	 */
	for (Idx = 0; Idx < 0x10; Idx++)
		WaitMicroSec(100);

	/* Step 8. Signal bank precharge command enable. */
	Data = pci_read_config8(MEMCTRL, 0xdb);
	Data &= 0xE3;
	Data |= 0x14;
	pci_write_config8(MEMCTRL, 0xdb, Data);

	/* Set MA10 = 1, precharge all bank. */
	Data = 0x00;
	pci_write_config8(MEMCTRL, 0xf8, Data);

	Data = 0x04;
	pci_write_config8(MEMCTRL, 0xf9, Data);

	/* step 9. Issue a precharge all cycle, RegD3[7] 0 -> 1. */
	Data = pci_read_config8(MEMCTRL, 0xdb);
	Data |= 0x2;
	pci_write_config8(MEMCTRL, 0xdb, Data);
	Data &= 0xFD;
	pci_write_config8(MEMCTRL, 0xdb, Data);

	/* Step 10. MRS enable. */
	Data = pci_read_config8(MEMCTRL, 0xdb);
	Data &= 0xE3;
	Data |= 0x1C;
	pci_write_config8(MEMCTRL, 0xdb, Data);

	/* Step 11. EMRS DLL enable and Disable DQS. */
	Data = 0x40;
	pci_write_config8(MEMCTRL, 0xf8, Data);

	Data = 0x24;
	pci_write_config8(MEMCTRL, 0xf9, Data);

	/* Step 12. Issue EMRS cycle. */
	Data = pci_read_config8(MEMCTRL, 0xdb);
	Data |= 0x2;
	pci_write_config8(MEMCTRL, 0xdb, Data);
	Data &= 0xFD;
	pci_write_config8(MEMCTRL, 0xdb, Data);

	/* Step 13. MSR enable. */
	Data = pci_read_config8(MEMCTRL, 0xdb);
	Data &= 0xE3;
	Data |= 0x1C;
	pci_write_config8(MEMCTRL, 0xdb, Data);

	/* Step 14. MSR DLL Reset. */
	Data = 0x00;
	pci_write_config8(MEMCTRL, 0xf8, Data);

	Data = 0x01;
	pci_write_config8(MEMCTRL, 0xf9, Data);

	/* Step 15. Issue MRS cycle. */
	Data = pci_read_config8(MEMCTRL, 0xdb);
	Data |= 0x2;
	pci_write_config8(MEMCTRL, 0xdb, Data);
	Data &= 0xFD;
	pci_write_config8(MEMCTRL, 0xdb, Data);

	/* Step 16. Signal banks precharge command enable. */
	Data = pci_read_config8(MEMCTRL, 0xdb);
	Data &= 0xE3;
	Data |= 0x14;
	pci_write_config8(MEMCTRL, 0xdb, Data);

	/* Set MA10 = 1, precharge all bank. */
	Data = 0x00;
	pci_write_config8(MEMCTRL, 0xf8, Data);

	Data = 0x04;
	pci_write_config8(MEMCTRL, 0xf9, Data);

	/* Step 17. Issue precharge all cycle. */
	Data = pci_read_config8(MEMCTRL, 0xdb);
	Data |= 0x2;
	pci_write_config8(MEMCTRL, 0xdb, Data);
	Data &= 0xFD;
	pci_write_config8(MEMCTRL, 0xdb, Data);

	/* Step 18. CBR cycle enable. */
	Data = pci_read_config8(MEMCTRL, 0xdb);
	Data &= 0xE3;
	Data |= 0x18;
	pci_write_config8(MEMCTRL, 0xdb, Data);

	Data = 0x00;
	pci_write_config8(MEMCTRL, 0xf8, Data);

	Data = 0x00;
	pci_write_config8(MEMCTRL, 0xf9, Data);

	//step 19.20.21
	//repeat issue 8 CBR cycle, between each cycle stop 100us
	for (Idx = 0; Idx < 8; Idx++) {
		// issue CBR cycle
		Data = pci_read_config8(MEMCTRL, 0xdb);
		Data |= 0x2;
		pci_write_config8(MEMCTRL, 0xdb, Data);
		Data &= 0xFD;
		pci_write_config8(MEMCTRL, 0xdb, Data);
		WaitMicroSec(100);
	}

	//the SDRAM parameters.(, CAS# Latency , Write recovery etc.)
	//------------------------------------------------------------

	// CL = really offset RXDC[7:6]
	Data = pci_read_config8(MEMCTRL, 0xdc);
	CL = (Data & 0xC0) >> 6;

	AccessAddr = (u32) (CHC_MRS_table[CL]);

	//Write recovery  : really offset Rx63[7:5]
	Data = pci_read_config8(MEMCTRL, 0x63);
	Twr = (Data & 0xE0) >> 5;

	AccessAddr += Twr * 0x200;

	//step22. MSR enable
	Data = pci_read_config8(MEMCTRL, 0xdb);
	Data &= 0xE3;
	Data |= 0x1C;
	pci_write_config8(MEMCTRL, 0xdb, Data);

	//step 23. MSR command
	Data = (u8) (AccessAddr & 0xFF);
	pci_write_config8(MEMCTRL, 0xf8, Data);

	Data = (u8) ((AccessAddr & 0xFF00) >> 8);
	pci_write_config8(MEMCTRL, 0xf9, Data);

	//step 24.  issue MRS cycle
	Data = pci_read_config8(MEMCTRL, 0xdb);
	Data |= 0x2;
	pci_write_config8(MEMCTRL, 0xdb, Data);
	Data &= 0xFD;
	pci_write_config8(MEMCTRL, 0xdb, Data);

	//step 25. EMRS enable
	Data = pci_read_config8(MEMCTRL, 0xdb);
	Data &= 0xE3;
	Data |= 0x1C;
	pci_write_config8(MEMCTRL, 0xdb, Data);

	//step 26. OCD default
	Data = 0xC0;
	pci_write_config8(MEMCTRL, 0xf8, Data);

	Data = 0x27;
	pci_write_config8(MEMCTRL, 0xf9, Data);

	//step 27.  issue EMRS cycle
	Data = pci_read_config8(MEMCTRL, 0xdb);
	Data |= 0x2;
	pci_write_config8(MEMCTRL, 0xdb, Data);
	Data &= 0xFD;
	pci_write_config8(MEMCTRL, 0xdb, Data);

	//step 28. OCD Exit
	Data = 0x40;
	pci_write_config8(MEMCTRL, 0xf8, Data);

	Data = 0x24;
	pci_write_config8(MEMCTRL, 0xf9, Data);

	//step 29. issue EMRS cycle
	Data = pci_read_config8(MEMCTRL, 0xdb);
	Data |= 0x2;
	pci_write_config8(MEMCTRL, 0xdb, Data);
	Data &= 0xFD;
	pci_write_config8(MEMCTRL, 0xdb, Data);

	Status = VerifyChc();
	if (Status != CB_SUCCESS)
		PRINT_DEBUG_MEM("Error!!!!CHC init error!\r");
	//step 31.  exit the initialization mode
	Data = pci_read_config8(MEMCTRL, 0xdb);
	Data &= 0x9F;
	pci_write_config8(MEMCTRL, 0xdb, Data);
}

CB_STATUS VerifyChc(void)
{
	u8 Data, ByteVal, Index, pad;
	u16 row;

	//first write the pad to all the address

	//the  row bits is 13 and  rank bit is 2, so the  address bits is 15 and the value is 0x7fff
	//verify each MA[0:12],BA[0:1]
	pad = 1;
	for (row = 0; row < 0x8000; row++) {
		/* Set the write value, Verify each MD[15:0]. */
		for (Data = pad, Index = 0; Index < 16; Index++) {
			Data <<= 1;
			if (Data == 0)
				Data = 1;
			pci_write_config8(PCI_DEV(0, 0, 7), 0xC0 + Index, Data);
		}

		/* Issue the bank active command. */
		// bank active command enable
		Data = pci_read_config8(MEMCTRL, 0xdb);
		Data &= 0xE3;
		Data |= 0x10;
		pci_write_config8(MEMCTRL, 0xdb, Data);

		Data = (u8) (row && 0xFF);
		pci_write_config8(MEMCTRL, 0xf8, Data);

		Data = (u8) ((row && 0xFF) >> 8);
		pci_write_config8(MEMCTRL, 0xf9, Data);

		/* Issue active cycle. */
		Data = pci_read_config8(MEMCTRL, 0xdb);
		Data |= 0x2;
		pci_write_config8(MEMCTRL, 0xdb, Data);
		Data &= 0xFD;
		pci_write_config8(MEMCTRL, 0xdb, Data);

		/* Issue ready/completion for read/write. */
		// read/completion command enable
		Data = pci_read_config8(MEMCTRL, 0xdb);
		Data &= 0xE3;
		Data |= 0x04;
		pci_write_config8(MEMCTRL, 0xdb, Data);

		Data = 0x00;
		pci_write_config8(MEMCTRL, 0xf8, Data);

		Data = 0x00;
		pci_write_config8(MEMCTRL, 0xf9, Data);

		/* Issue read/completion cycle. */
		Data = pci_read_config8(MEMCTRL, 0xdb);
		Data |= 0x2;
		pci_write_config8(MEMCTRL, 0xdb, Data);
		Data &= 0xFD;
		pci_write_config8(MEMCTRL, 0xdb, Data);

		/* Issue write command. */
		// write command enable
		Data = pci_read_config8(MEMCTRL, 0xdb);
		Data &= 0xE3;
		Data |= 0x0C;
		pci_write_config8(MEMCTRL, 0xdb, Data);

		Data = 0x00;
		pci_write_config8(MEMCTRL, 0xf8, Data);

		Data = (u8) ((row & 0x60) << 5);
		pci_write_config8(MEMCTRL, 0xf9, Data);

		/* Issue write cycle. */
		Data = pci_read_config8(MEMCTRL, 0xdb);
		Data |= 0x2;
		pci_write_config8(MEMCTRL, 0xdb, Data);
		Data &= 0xFD;
		pci_write_config8(MEMCTRL, 0xdb, Data);

		////issue ready/completion for read/write
		// read/completion command enable
		Data = pci_read_config8(MEMCTRL, 0xdb);
		Data &= 0xE3;
		Data |= 0x04;
		pci_write_config8(MEMCTRL, 0xdb, Data);

		Data = 0x00;
		pci_write_config8(MEMCTRL, 0xf8, Data);

		Data = 0x00;
		pci_write_config8(MEMCTRL, 0xf9, Data);

		/* Issue read/completion cycle. */
		Data = pci_read_config8(MEMCTRL, 0xdb);
		Data |= 0x2;
		pci_write_config8(MEMCTRL, 0xdb, Data);
		Data &= 0xFD;
		pci_write_config8(MEMCTRL, 0xdb, Data);

		/* Issue the bank active command. */
		// bank active command enable
		Data = pci_read_config8(MEMCTRL, 0xdb);
		Data &= 0xE3;
		Data |= 0x10;
		pci_write_config8(MEMCTRL, 0xdb, Data);

		Data = (u8) (row && 0xFF);
		pci_write_config8(MEMCTRL, 0xf8, Data);

		Data = (u8) ((row && 0xFF) >> 8);
		pci_write_config8(MEMCTRL, 0xf9, Data);

		//  issue active cycle
		Data = pci_read_config8(MEMCTRL, 0xdb);
		Data |= 0x2;
		pci_write_config8(MEMCTRL, 0xdb, Data);
		Data &= 0xFD;
		pci_write_config8(MEMCTRL, 0xdb, Data);

		////issue ready/completion for read/write
		// read/completion command enable
		Data = pci_read_config8(MEMCTRL, 0xdb);
		Data &= 0xE3;
		Data |= 0x04;
		pci_write_config8(MEMCTRL, 0xdb, Data);

		Data = 0x00;
		pci_write_config8(MEMCTRL, 0xf8, Data);

		Data = 0x00;
		pci_write_config8(MEMCTRL, 0xf9, Data);

		//  issue read/completion cycle
		Data = pci_read_config8(MEMCTRL, 0xdb);
		Data |= 0x2;
		pci_write_config8(MEMCTRL, 0xdb, Data);
		Data &= 0xFD;
		pci_write_config8(MEMCTRL, 0xdb, Data);

		////issue read command
		// read/completion command enable
		Data = pci_read_config8(MEMCTRL, 0xdb);
		Data &= 0xE3;
		Data |= 0x08;
		pci_write_config8(MEMCTRL, 0xdb, Data);

		Data = 0x00;
		pci_write_config8(MEMCTRL, 0xf8, Data);

		Data = (u8) ((row & 0x60) << 5);
		pci_write_config8(MEMCTRL, 0xf9, Data);

		//  issue read cycle
		Data = pci_read_config8(MEMCTRL, 0xdb);
		Data |= 0x2;
		pci_write_config8(MEMCTRL, 0xdb, Data);
		Data &= 0xFD;
		pci_write_config8(MEMCTRL, 0xdb, Data);

		////issue ready/completion for read/write
		// read/completion command enable
		Data = pci_read_config8(MEMCTRL, 0xdb);
		Data &= 0xE3;
		Data |= 0x04;
		pci_write_config8(MEMCTRL, 0xdb, Data);

		Data = 0x00;
		pci_write_config8(MEMCTRL, 0xf8, Data);

		Data = 0x00;
		pci_write_config8(MEMCTRL, 0xf9, Data);

		/* Issue read/completion cycle. */
		Data = pci_read_config8(MEMCTRL, 0xdb);
		Data |= 0x2;
		pci_write_config8(MEMCTRL, 0xdb, Data);
		Data &= 0xFD;
		pci_write_config8(MEMCTRL, 0xdb, Data);

		/* Verify the value. */
		for (ByteVal = pad, Index = 0; Index < 16; Index++) {
			Data = pci_read_config8(PCI_DEV(0, 0, 7), 0xD0 + Index);
			if (ByteVal != Data) {
				PRINT_DEBUG_MEM("Error! row = %x, index =%x, "
						"data = %x, byteval=%x\r");
			}
			ByteVal <<= 1;
			if (ByteVal == 0)
				ByteVal = 1;
		}
		pad <<= 1;
		if (pad == 0)
			pad = 1;
	}

	return CB_SUCCESS;
}
