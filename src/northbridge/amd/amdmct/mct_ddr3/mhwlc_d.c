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

/*
 *-----------------------------------------------------------------------------
 *			MODULES USED
 *
 *-----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *			PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
u32 swapAddrBits_wl(sDCTStruct *pDCTData, u32 MRSValue);
u32 swapBankBits(sDCTStruct *pDCTData, u32 MRSValue);
void prepareDimms(sMCTStruct *pMCTData, sDCTStruct *pDCTData, u8 dimm, BOOL wl);
void programODT(sMCTStruct *pMCTData, sDCTStruct *pDCTData, u8 dimm);
void procConifg(sMCTStruct *pMCTData,sDCTStruct *pDCTData, u8 dimm, u8 pass);
void setWLByteDelay(sDCTStruct *pDCTData, u8 ByteLane, u8 dimm, u8 targetAddr);
void getWLByteDelay(sDCTStruct *pDCTData, u8 ByteLane, u8 dimm);
/*
 *-----------------------------------------------------------------------------
 *		EXPORTED FUNCTIONS
 *
 *-----------------------------------------------------------------------------
 */

/*-----------------------------------------------------------------------------
 * void AgesaHwWlPhase1(SPDStruct *SPDData,MCTStruct *MCTData, DCTStruct *DCTData,
 *                  u8 Dimm, u8 Pass)
 *
 *  Description:
 *       This function initialized Hardware based write levelization phase 1
 *
 *   Parameters:
 *       IN  OUT   *SPDData - Pointer to buffer with information about each DIMMs
 *                            SPD information
 *                 *MCTData - Pointer to buffer with runtime parameters,
 *                 *DCTData - Pointer to buffer with information about each DCT
 *
 *       IN        DIMM - Logical DIMM number
 *                 Pass - First or Second Pass
 *       OUT
 *-----------------------------------------------------------------------------
 */
void AgesaHwWlPhase1(sMCTStruct *pMCTData, sDCTStruct *pDCTData,
		u8 dimm, u8 pass)
{
	u8 ByteLane;
	u32 Value, Addr;
	u16 Addl_Data_Offset, Addl_Data_Port;

	pDCTData->WLPass = pass;
	/* 1. Specify the target DIMM that is to be trained by programming
	 * F2x[1, 0]9C_x08[TrDimmSel].
	 */
	set_DCT_ADDR_Bits(pDCTData, pDCTData->DctTrain, pDCTData->NodeId, FUN_DCT,
			DRAM_ADD_DCT_PHY_CONTROL_REG, TrDimmSelStart,
			TrDimmSelEnd,(u32)dimm);
	/* 2. Prepare the DIMMs for write levelization using DDR3-defined
	 * MR commands. */
	prepareDimms(pMCTData, pDCTData,dimm, TRUE);
	/* 3. After the DIMMs are configured, BIOS waits 40 MEMCLKs to
	 *    satisfy DDR3-defined internal DRAM timing.
	 */
	pMCTData->AgesaDelay(40);
	/* 4. Configure the processor's DDR phy for write levelization training: */
	procConifg(pMCTData,pDCTData, dimm, pass);
	/* 5. Begin write levelization training:
	 *  Program F2x[1, 0]9C_x08[WrtLevelTrEn]=1. */
	if (pDCTData->LogicalCPUID & (AMD_DR_Cx | AMD_DR_Dx))
		set_DCT_ADDR_Bits(pDCTData, pDCTData->DctTrain, pDCTData->NodeId, FUN_DCT,
				DRAM_ADD_DCT_PHY_CONTROL_REG, WrtLvTrEn, WrtLvTrEn, 1);
	else
	{
		/* Broadcast write to all D3Dbyte chipset register offset 0xc
		 * Set bit 0 (wrTrain)
		 * Program bit 4 to nibble being trained (only matters for x4dimms)
		 * retain value of 3:2 (Trdimmsel)
		 * reset bit 5 (FrzPR)
		 */
		if (pDCTData->DctTrain)
		{
			Addl_Data_Offset=0x198;
			Addl_Data_Port=0x19C;
		}
		else
		{
			Addl_Data_Offset=0x98;
			Addl_Data_Port=0x9C;
		}
		Addr=0x0D00000C;
		AmdMemPCIWriteBits(MAKE_SBDFO(0,0,24+(pDCTData->NodeId),FUN_DCT,Addl_Data_Offset), 31, 0, &Addr);
		while ((get_Bits(pDCTData,FUN_DCT,pDCTData->NodeId, FUN_DCT, Addl_Data_Offset,
				DctAccessDone, DctAccessDone)) == 0);
		AmdMemPCIReadBits(MAKE_SBDFO(0,0,24+(pDCTData->NodeId),FUN_DCT,Addl_Data_Port), 31, 0, &Value);
		Value = bitTestSet(Value, 0);	/* enable WL training */
		Value = bitTestReset(Value, 4); /* for x8 only */
		Value = bitTestReset(Value, 5); /* for hardware WL training */
		AmdMemPCIWriteBits(MAKE_SBDFO(0,0,24+(pDCTData->NodeId),FUN_DCT,Addl_Data_Port), 31, 0, &Value);
		Addr=0x4D030F0C;
		AmdMemPCIWriteBits(MAKE_SBDFO(0,0,24+(pDCTData->NodeId),FUN_DCT,Addl_Data_Offset), 31, 0, &Addr);
		while ((get_Bits(pDCTData,FUN_DCT,pDCTData->NodeId, FUN_DCT, Addl_Data_Offset,
				DctAccessDone, DctAccessDone)) == 0);
	}

	/* Wait 200 MEMCLKs. If executing pass 2, wait 32 MEMCLKs. */
	pMCTData->AgesaDelay(140);
	/* Program F2x[1, 0]9C_x08[WrtLevelTrEn]=0. */
	set_DCT_ADDR_Bits(pDCTData, pDCTData->DctTrain, pDCTData->NodeId, FUN_DCT,
			DRAM_ADD_DCT_PHY_CONTROL_REG, WrtLvTrEn, WrtLvTrEn, 0);
	/* Read from registers F2x[1, 0]9C_x[51:50] and F2x[1, 0]9C_x52
	 * to get the gross and fine delay settings
	 * for the target DIMM and save these values. */
	ByteLane = 0;
	while (ByteLane < MAX_BYTE_LANES)
	{
		getWLByteDelay(pDCTData,ByteLane, dimm);
		setWLByteDelay(pDCTData,ByteLane, dimm, 1);
		ByteLane++;
	}

	/* 6. Configure DRAM Phy Control Register so that the phy stops driving
	 *    write levelization ODT. */
	set_DCT_ADDR_Bits(pDCTData, pDCTData->DctTrain, pDCTData->NodeId, FUN_DCT,
			DRAM_ADD_DCT_PHY_CONTROL_REG, WrLvOdtEn, WrLvOdtEn, 0);

	/* Wait 10 MEMCLKs to allow for ODT signal settling. */
	pMCTData->AgesaDelay(10);

	/* 7. Program the target DIMM back to normal operation by configuring
	 * the following (See section 2.8.5.4.1.1
	 * [Phy Assisted Write Levelization] on page 97 pass 1, step #2):
	 * Configure all ranks of the target DIMM for normal operation.
	 * Enable the output drivers of all ranks of the target DIMM.
	 * For a two DIMM system, program the Rtt value for the target DIMM
	 * to the normal operating termination:
	 */
	prepareDimms(pMCTData, pDCTData,dimm,FALSE);
}

/*----------------------------------------------------------------------------
 *	LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/*-----------------------------------------------------------------------------
 * u32 swapAddrBits_wl(sDCTStruct *pDCTData, u32 MRSValue)
 *
 * Description:
 *	This function swaps the bits in MSR register value
 *
 * Parameters:
 *	IN  OUT   *DCTData - Pointer to buffer with information about each DCT
 *	IN	u32: MRS value
 *	OUT	u32: Swapped BANK BITS
 *
 * ----------------------------------------------------------------------------
 */
u32 swapAddrBits_wl(sDCTStruct *pDCTData, u32 MRSValue)
{
	u32 tempW, tempW1;

	tempW1 = get_Bits(pDCTData, pDCTData->CurrDct, pDCTData->NodeId,
			FUN_DCT, DRAM_INIT, MrsChipSelStart, MrsChipSelEnd);
	if (tempW1 & 1)
	{
		if ((pDCTData->Status[DCT_STATUS_OnDimmMirror]))
		{
			/* swap A3/A4,A5/A6,A7/A8 */
			tempW = MRSValue;
			tempW1 = MRSValue;
			tempW &= 0x0A8;
			tempW1 &= 0x0150;
			MRSValue &= 0xFE07;
			MRSValue |= (tempW<<1);
			MRSValue |= (tempW1>>1);
		}
	}
	return MRSValue;
}

/*-----------------------------------------------------------------------------
 *  u32 swapBankBits(sDCTStruct *pDCTData, u32 MRSValue)
 *
 *  Description:
 *       This function swaps the bits in MSR register value
 *
 *   Parameters:
 *       IN  OUT   *DCTData - Pointer to buffer with information about each DCT
 *       IN	u32: MRS value
 *       OUT       u32: Swapped BANK BITS
 *
 * ----------------------------------------------------------------------------
 */
u32 swapBankBits(sDCTStruct *pDCTData, u32 MRSValue)
{
	u32 tempW, tempW1;

	tempW1 = get_Bits(pDCTData, pDCTData->CurrDct, pDCTData->NodeId,
			FUN_DCT, DRAM_INIT, MrsChipSelStart, MrsChipSelEnd);
	if (tempW1 & 1)
	{
		if ((pDCTData->Status[DCT_STATUS_OnDimmMirror]))
		{
			/* swap BA0/BA1 */
			tempW = MRSValue;
			tempW1 = MRSValue;
			tempW &= 0x01;
			tempW1 &= 0x02;
			MRSValue = 0;
			MRSValue |= (tempW<<1);
			MRSValue |= (tempW1>>1);
		}
	}
	return MRSValue;
}

/*-----------------------------------------------------------------------------
 *  void prepareDimms(sMCTStruct *pMCTData, sDCTStruct *DCTData, u8 Dimm, BOOL WL)
 *
 *  Description:
 *       This function prepares DIMMS for training
 *
 *   Parameters:
 *       IN  OUT   *DCTData - Pointer to buffer with information about each DCT
 *		 *SPDData - Pointer to buffer with information about each DIMMs
 *			    SPD information
 *		 *MCTData - Pointer to buffer with runtime parameters,
 *       IN	Dimm - Logical DIMM number
 *		 WL - indicates if the routine is used for Write levelization
 *		      training
 *
 *       OUT
 *
 * ----------------------------------------------------------------------------
 */
void prepareDimms(sMCTStruct *pMCTData, sDCTStruct *pDCTData, u8 dimm, BOOL wl)
{
	u32 tempW, tempW1, tempW2, MrsBank;
	u8 rank, currDimm, MemClkFreq;

	MemClkFreq = get_Bits(pDCTData, pDCTData->CurrDct, pDCTData->NodeId,
			FUN_DCT, DRAM_CONFIG_HIGH, 0, 2);
	/* Configure the DCT to send initialization MR commands to the target DIMM
	 * by programming the F2x[1,0]7C register using the following steps.
	 */
	rank = 0;
	while ((rank < pDCTData->DimmRanks[dimm]) && (rank < 2))
	{
		/* Program F2x[1, 0]7C[MrsChipSel[2:0]] for the current rank to be trained. */
		set_Bits(pDCTData, pDCTData->CurrDct, pDCTData->NodeId, FUN_DCT,
			DRAM_INIT, MrsChipSelStart, MrsChipSelEnd, dimm*2+rank);
		/* Program F2x[1, 0]7C[MrsBank[2:0]] for the appropriate internal DRAM
		 * register that defines the required DDR3-defined function for write
		 * levelization.
		 */
		MrsBank = swapBankBits(pDCTData,1);
		set_Bits(pDCTData, pDCTData->CurrDct, pDCTData->NodeId, FUN_DCT,
			DRAM_INIT, MrsBankStart, MrsBankEnd, MrsBank);
		/* Program F2x[1, 0]7C[MrsAddress[15:0]] to the required DDR3-defined function
		 * for write levelization.
		 */
		tempW = 0;/* DLL_DIS = 0, DIC = 0, AL = 0, TDQS = 0 */

		/* Set TDQS=1b for x8 DIMM, TDQS=0b for x4 DIMM, when mixed x8 & x4 */
		tempW2 = get_Bits(pDCTData, pDCTData->CurrDct, pDCTData->NodeId,
				FUN_DCT, DRAM_CONFIG_HIGH, RDqsEn, RDqsEn);
		if (tempW2)
		{
			if (pDCTData->DimmX8Present[dimm])
				tempW |= 0x800;
		}

		/* determine Rtt_Nom for WL & Normal mode */
		if (pDCTData->Status[DCT_STATUS_REGISTERED]) {
			tempW1 = RttNomTargetRegDimm(pMCTData, pDCTData, dimm, wl, MemClkFreq, rank);
		} else {
			if (wl)
			{
				if (pDCTData->MaxDimmsInstalled == 1)
				{
					if ((pDCTData->DimmRanks[dimm] == 2) && (rank == 0))
					{
						tempW1 = 0x00;	/* Rtt_Nom=OFF */
					}
					else
					{
						tempW1 = 0x04;	/* Rtt_Nom=RZQ/4=60 Ohm */
					}
				}
				else	/* 2 Dimms or more per channel */
				{
					if ((pDCTData->DimmRanks[dimm] == 2) && (rank == 1))
					{
						tempW1 = 0x00;	/* Rtt_Nom=OFF */
					}
					else
					{
						if (MemClkFreq == 6) {
							tempW1 = 0x04;	/* Rtt_Nom=RZQ/4=60 Ohm */
						} else {
							tempW1 = 0x40;/* Rtt_Nom=RZQ/2=120 Ohm */
						}
					}
				}
			}
			else {	/* 1 or 4 Dimms per channel */
				if ((pDCTData->MaxDimmsInstalled == 1) || (pDCTData->MaxDimmsInstalled == 4))
				{
					tempW1 = 0x04;	/* Rtt_Nom=RZQ/4=60 Ohm */
				}
				else	/* 2 or 3 Dimms per channel */
				{
					if (MemClkFreq < 5) {
						tempW1 = 0x0044;	/* Rtt_Nom=RZQ/6=40 Ohm */
					} else {
						tempW1 = 0x0204;	/* Rtt_Nom=RZQ/8=30 Ohm */
					}
				}
			}
		}
		tempW=tempW|tempW1;

		/* All ranks of the target DIMM are set to write levelization mode. */
		if (wl)
		{
			tempW1 = bitTestSet(tempW, MRS_Level);
			if (rank == 0)
			{
				/* Enable the output driver of the first rank of the target DIMM. */
				tempW = tempW1;
			}
			else
			{
				/* Disable the output drivers of all other ranks for
				 * the target DIMM. */
				tempW = bitTestSet(tempW1, Qoff);
			}
		}
		/* program MrsAddress[5,1]=output driver impedance control (DIC):
		 * based on F2x[1,0]84[DrvImpCtrl] */
		tempW1 = get_Bits(pDCTData, pDCTData->CurrDct, pDCTData->NodeId,
				FUN_DCT, DRAM_MRS_REGISTER, DrvImpCtrlStart, DrvImpCtrlEnd);
		if (bitTest(tempW1,1))
		{tempW = bitTestSet(tempW, 5);}
		if (bitTest(tempW1,0))
		{tempW = bitTestSet(tempW, 1);}

		tempW = swapAddrBits_wl(pDCTData,tempW);

		set_Bits(pDCTData, pDCTData->CurrDct, pDCTData->NodeId, FUN_DCT,
			DRAM_INIT, MrsAddressStart, MrsAddressEnd, tempW);
		/* Program F2x[1, 0]7C[SendMrsCmd]=1 to initiate the command to
		 * the specified DIMM.
		 */
		set_Bits(pDCTData, pDCTData->CurrDct, pDCTData->NodeId, FUN_DCT,
			DRAM_INIT, SendMrsCmd, SendMrsCmd, 1);
		/* Wait for F2x[1, 0]7C[SendMrsCmd] to be cleared by hardware. */
		while ((get_Bits(pDCTData, pDCTData->CurrDct, pDCTData->NodeId,
				FUN_DCT, DRAM_INIT, SendMrsCmd, SendMrsCmd)) == 0x1)
		{
		}
		/* Program F2x[1, 0]7C[MrsBank[2:0]] for the appropriate internal DRAM
		 * register that defines the required DDR3-defined function for Rtt_WR.
		 */
		MrsBank = swapBankBits(pDCTData,2);
		set_Bits(pDCTData, pDCTData->CurrDct, pDCTData->NodeId, FUN_DCT,
			DRAM_INIT, MrsBankStart, MrsBankEnd, MrsBank);
		/* Program F2x[1, 0]7C[MrsAddress[15:0]] to the required DDR3-defined function
		 * for Rtt_WR (DRAMTermDyn).
		 */
		tempW = 0;/* PASR = 0,*/
		/* program MrsAddress[7,6,5:3]=SRT,ASR,CWL,
		 * based on F2x[1,0]84[19,18,22:20]=,SRT,ASR,Tcwl */
		tempW1 = get_Bits(pDCTData, pDCTData->CurrDct, pDCTData->NodeId,
				FUN_DCT, DRAM_MRS_REGISTER, PCI_MIN_LOW, PCI_MAX_HIGH);
		if (bitTest(tempW1,19))
		{tempW = bitTestSet(tempW, 7);}
		if (bitTest(tempW1,18))
		{tempW = bitTestSet(tempW, 6);}
		/* tempW=tempW|(((tempW1>>20)&0x7)<<3); */
		tempW=tempW|((tempW1&0x00700000)>>17);
		/* workaround for DR-B0 */
		if ((pDCTData->LogicalCPUID & AMD_DR_Bx) && (pDCTData->Status[DCT_STATUS_REGISTERED]))
			tempW+=0x8;
		/* determine Rtt_WR for WL & Normal mode */
		if (pDCTData->Status[DCT_STATUS_REGISTERED]) {
			tempW1 = RttWrRegDimm(pMCTData, pDCTData, dimm, wl, MemClkFreq, rank);
		} else {
			if (wl)
			{
				tempW1 = 0x00;	/* Rtt_WR=off */
			}
			else
			{
				if (pDCTData->MaxDimmsInstalled == 1)
				{
					tempW1 = 0x00;	/* Rtt_WR=off */
				}
				else
				{
					if (MemClkFreq == 6) {
						tempW1 = 0x200;	/* Rtt_WR=RZQ/4=60 Ohm */
					} else {
						tempW1 = 0x400;	/* Rtt_WR=RZQ/2 */
					}
				}
			}
		}
		tempW=tempW|tempW1;
		tempW = swapAddrBits_wl(pDCTData,tempW);
		set_Bits(pDCTData, pDCTData->CurrDct, pDCTData->NodeId, FUN_DCT,
			DRAM_INIT, MrsAddressStart, MrsAddressEnd, tempW);
		/* Program F2x[1, 0]7C[SendMrsCmd]=1 to initiate the command to
		   the specified DIMM.*/
		set_Bits(pDCTData, pDCTData->CurrDct, pDCTData->NodeId, FUN_DCT,
			DRAM_INIT, SendMrsCmd, SendMrsCmd, 1);
		/* Wait for F2x[1, 0]7C[SendMrsCmd] to be cleared by hardware. */
		while ((get_Bits(pDCTData, pDCTData->CurrDct, pDCTData->NodeId,
				FUN_DCT, DRAM_INIT, SendMrsCmd, SendMrsCmd)) == 0x1)
		{
		}

		rank++;
	}

	/* Configure the non-target DIMM normally. */
	currDimm = 0;
	while (currDimm < MAX_LDIMMS)
	{
		if (pDCTData->DimmPresent[currDimm])
		{
			if (currDimm != dimm)
			{
				rank = 0;
				while ((rank < pDCTData->DimmRanks[currDimm]) && (rank < 2))
				{

					/* Program F2x[1, 0]7C[MrsChipSel[2:0]] for the current rank
					 * to be trained.
					 */
					set_Bits(pDCTData, pDCTData->CurrDct, pDCTData->NodeId,
						FUN_DCT, DRAM_INIT, MrsChipSelStart, MrsChipSelEnd, currDimm*2+rank);
					/* Program F2x[1, 0]7C[MrsBank[2:0]] for the appropriate internal
					 * DRAM register that defines the required DDR3-defined function
					 * for write levelization.
					 */
					MrsBank = swapBankBits(pDCTData,1);
					set_Bits(pDCTData, pDCTData->CurrDct, pDCTData->NodeId,
						FUN_DCT, DRAM_INIT, MrsBankStart, MrsBankEnd, MrsBank);
					/* Program F2x[1, 0]7C[MrsAddress[15:0]] to the required
					 * DDR3-defined function for write levelization.
					 */
					tempW = 0;/* DLL_DIS = 0, DIC = 0, AL = 0, TDQS = 0, Level=0, Qoff=0 */

					/* Set TDQS=1b for x8 DIMM, TDQS=0b for x4 DIMM, when mixed x8 & x4 */
					tempW2 = get_Bits(pDCTData, pDCTData->CurrDct, pDCTData->NodeId,
							FUN_DCT, DRAM_CONFIG_HIGH, RDqsEn, RDqsEn);
					if (tempW2)
					{
						if (pDCTData->DimmX8Present[currDimm])
							tempW |= 0x800;
					}

					/* determine Rtt_Nom for WL & Normal mode */
					if (pDCTData->Status[DCT_STATUS_REGISTERED]) {
						tempW1 = RttNomNonTargetRegDimm(pMCTData, pDCTData, currDimm, wl, MemClkFreq, rank);
					} else {
						if (wl)
						{
							if ((pDCTData->DimmRanks[currDimm] == 2) && (rank == 1))
							{
								tempW1 = 0x00;	/* Rtt_Nom=OFF */
							}
							else
							{
								if (MemClkFreq < 5) {
									tempW1 = 0x0044;/* Rtt_Nom=RZQ/6=40 Ohm */
								} else {
									tempW1 = 0x0204;/* Rtt_Nom=RZQ/8=30 Ohm */
								}
							}
						}
						else {	/* 1 or 4 Dimms per channel */
							if ((pDCTData->MaxDimmsInstalled == 4))
							{
								tempW1 = 0x04;	/* Rtt_Nom=RZQ/4=60 Ohm */
							}
							else {	/* 2 or 3 Dimms per channel */
								if (MemClkFreq < 5) {
									tempW1 = 0x0044;	/* Rtt_Nom=RZQ/6=40 Ohm */
								} else {
									tempW1 = 0x0204;	/* Rtt_Nom=RZQ/8=30 Ohm */
								}
							}
						}
					}
					tempW=tempW|tempW1;
					/* program MrsAddress[5,1]=output driver impedance control (DIC):
					 * based on F2x[1,0]84[DrvImpCtrl] */
					tempW1 = get_Bits(pDCTData, pDCTData->CurrDct, pDCTData->NodeId,
							FUN_DCT, DRAM_MRS_REGISTER, DrvImpCtrlStart, DrvImpCtrlEnd);
					if (bitTest(tempW1,1))
					{tempW = bitTestSet(tempW, 5);}
					if (bitTest(tempW1,0))
					{tempW = bitTestSet(tempW, 1);}
					tempW = swapAddrBits_wl(pDCTData,tempW);
					set_Bits(pDCTData, pDCTData->CurrDct, pDCTData->NodeId,
						FUN_DCT, DRAM_INIT, MrsAddressStart, MrsAddressEnd, tempW);
					/* Program F2x[1, 0]7C[SendMrsCmd]=1 to initiate the command
					 * to the specified DIMM.
					 */
					set_Bits(pDCTData, pDCTData->CurrDct, pDCTData->NodeId,
						FUN_DCT, DRAM_INIT, SendMrsCmd, SendMrsCmd, 1);
					/* Wait for F2x[1, 0]7C[SendMrsCmd] to be cleared by hardware. */
					while ((get_Bits(pDCTData, pDCTData->CurrDct,
							pDCTData->NodeId, FUN_DCT, DRAM_INIT,
							SendMrsCmd, SendMrsCmd)) == 1);
					/* Program F2x[1, 0]7C[MrsBank[2:0]] for the appropriate internal DRAM
					 * register that defines the required DDR3-defined function for Rtt_WR.
					 */
					MrsBank = swapBankBits(pDCTData,2);
					set_Bits(pDCTData, pDCTData->CurrDct, pDCTData->NodeId, FUN_DCT,
						DRAM_INIT, MrsBankStart, MrsBankEnd, MrsBank);
					/* Program F2x[1, 0]7C[MrsAddress[15:0]] to the required DDR3-defined function
					 * for Rtt_WR (DRAMTermDyn).
					 */
					tempW = 0;/* PASR = 0,*/
					/* program MrsAddress[7,6,5:3]=SRT,ASR,CWL,
					 * based on F2x[1,0]84[19,18,22:20]=,SRT,ASR,Tcwl */
					tempW1 = get_Bits(pDCTData, pDCTData->CurrDct, pDCTData->NodeId,
							FUN_DCT, DRAM_MRS_REGISTER, PCI_MIN_LOW, PCI_MAX_HIGH);
					if (bitTest(tempW1,19))
					{tempW = bitTestSet(tempW, 7);}
					if (bitTest(tempW1,18))
					{tempW = bitTestSet(tempW, 6);}
					/* tempW=tempW|(((tempW1>>20)&0x7)<<3); */
					tempW=tempW|((tempW1&0x00700000)>>17);
					/* workaround for DR-B0 */
					if ((pDCTData->LogicalCPUID & AMD_DR_Bx) && (pDCTData->Status[DCT_STATUS_REGISTERED]))
						tempW+=0x8;
					/* determine Rtt_WR for WL & Normal mode */
					if (pDCTData->Status[DCT_STATUS_REGISTERED]) {
						tempW1 = RttWrRegDimm(pMCTData, pDCTData, currDimm, wl, MemClkFreq, rank);
					} else {
						if (wl)
						{
							tempW1 = 0x00;	/* Rtt_WR=off */
						}
						else
						{
							if (MemClkFreq == 6) {
								tempW1 = 0x200;	/* Rtt_WR=RZQ/4=60 Ohm */
							} else {
								tempW1 = 0x400;	/* Rtt_WR=RZQ/2 */
							}
						}
					}
					tempW=tempW|tempW1;
					tempW = swapAddrBits_wl(pDCTData,tempW);
					set_Bits(pDCTData, pDCTData->CurrDct, pDCTData->NodeId, FUN_DCT,
						DRAM_INIT, MrsAddressStart, MrsAddressEnd, tempW);
					/* Program F2x[1, 0]7C[SendMrsCmd]=1 to initiate the command to
					   the specified DIMM.*/
					set_Bits(pDCTData, pDCTData->CurrDct, pDCTData->NodeId, FUN_DCT,
						DRAM_INIT, SendMrsCmd, SendMrsCmd, 1);
					/* Wait for F2x[1, 0]7C[SendMrsCmd] to be cleared by hardware. */
					while ((get_Bits(pDCTData, pDCTData->CurrDct, pDCTData->NodeId,
							FUN_DCT, DRAM_INIT, SendMrsCmd, SendMrsCmd)) == 0x1)
					{
					}
					rank++;
				}
			}
		}
		currDimm++;
	}
}

/*-----------------------------------------------------------------------------
 * void programODT(sMCTStruct *pMCTData, DCTStruct *DCTData, u8 dimm)
 *
 *  Description:
 *       This function programs the ODT values for the NB
 *
 *   Parameters:
 *       IN  OUT   *DCTData - Pointer to buffer with information about each DCT
 *       IN
 *       OUT
 * ----------------------------------------------------------------------------
 */
void programODT(sMCTStruct *pMCTData, sDCTStruct *pDCTData, u8 dimm)
{
	u8 WrLvOdt1=0;

	if (pDCTData->Status[DCT_STATUS_REGISTERED] == 0) {
		if ((pDCTData->DctCSPresent & 0x05) == 0x05) {
			WrLvOdt1 = 0x03;
		} else if (bitTest((u32)pDCTData->DctCSPresent,(u8)(dimm*2+1))) {
			WrLvOdt1 = (u8)bitTestSet(WrLvOdt1, dimm+2);
		} else {
			WrLvOdt1 = (u8)bitTestSet(WrLvOdt1, dimm);
		}
	} else {
		WrLvOdt1 = WrLvOdtRegDimm(pMCTData, pDCTData, dimm);
	}

	set_DCT_ADDR_Bits(pDCTData, pDCTData->DctTrain, pDCTData->NodeId, FUN_DCT,
			DRAM_ADD_DCT_PHY_CONTROL_REG, 8, 11, (u32)WrLvOdt1);

}

/*-----------------------------------------------------------------------------
 * void procConifg(MCTStruct *MCTData,DCTStruct *DCTData, u8 Dimm, u8 Pass)
 *
 *  Description:
 *       This function programs the ODT values for the NB
 *
 *   Parameters:
 *       IN  OUT   *DCTData - Pointer to buffer with information about each DCT
 *		 *MCTData - Pointer to buffer with runtime parameters,
 *       IN	Dimm - Logical DIMM
 *		 Pass - First of Second Pass
 *       OUT
 * ----------------------------------------------------------------------------
 */
void procConifg(sMCTStruct *pMCTData,sDCTStruct *pDCTData, u8 dimm, u8 pass)
{
	u8 ByteLane, Seed_Gross, Seed_Fine;
	u32 Value, Addr;
	u16 Addl_Data_Offset, Addl_Data_Port;

	/* Program F2x[1, 0]9C_x08[WrLvOdt[3:0]] to the proper ODT settings for the
	 * current memory subsystem configuration.
	 */
	programODT(pMCTData, pDCTData, dimm);

	/* Program F2x[1,0]9C_x08[WrLvOdtEn]=1 */
	if (pDCTData->LogicalCPUID & (AMD_DR_Cx | AMD_DR_Dx))
		set_DCT_ADDR_Bits(pDCTData, pDCTData->DctTrain, pDCTData->NodeId, FUN_DCT,
				DRAM_ADD_DCT_PHY_CONTROL_REG, WrLvOdtEn, WrLvOdtEn, (u32)1);
	else
	{
		/* Program WrLvOdtEn=1 through set bit 12 of D3CSODT reg offset 0 for Rev.B*/
		if (pDCTData->DctTrain)
		{
			Addl_Data_Offset=0x198;
			Addl_Data_Port=0x19C;
		}
		else
		{
			Addl_Data_Offset=0x98;
			Addl_Data_Port=0x9C;
		}
		Addr=0x0D008000;
		AmdMemPCIWriteBits(MAKE_SBDFO(0,0,24+(pDCTData->NodeId),FUN_DCT,Addl_Data_Offset), 31, 0, &Addr);
		while ((get_Bits(pDCTData,FUN_DCT,pDCTData->NodeId, FUN_DCT, Addl_Data_Offset,
				DctAccessDone, DctAccessDone)) == 0);
		AmdMemPCIReadBits(MAKE_SBDFO(0,0,24+(pDCTData->NodeId),FUN_DCT,Addl_Data_Port), 31, 0, &Value);
		Value = bitTestSet(Value, 12);
		AmdMemPCIWriteBits(MAKE_SBDFO(0,0,24+(pDCTData->NodeId),FUN_DCT,Addl_Data_Port), 31, 0, &Value);
		Addr=0x4D088F00;
		AmdMemPCIWriteBits(MAKE_SBDFO(0,0,24+(pDCTData->NodeId),FUN_DCT,Addl_Data_Offset), 31, 0, &Addr);
		while ((get_Bits(pDCTData,FUN_DCT,pDCTData->NodeId, FUN_DCT, Addl_Data_Offset,
				DctAccessDone, DctAccessDone)) == 0);
	}

	/* Wait 10 MEMCLKs to allow for ODT signal settling. */
	pMCTData->AgesaDelay(10);
	ByteLane = 0;
	if (pass == 1)
	{
		if (pDCTData->Status[DCT_STATUS_REGISTERED])
		{
			if(pDCTData->RegMan1Present & ((1<<(dimm*2+pDCTData->DctTrain))))
			{
				Seed_Gross = 0x02;
				Seed_Fine = 0x16;
			}
			else
			{
				Seed_Gross = 0x02;
				Seed_Fine = 0x00;
			}
		}
		else
		{
			Seed_Gross = 0x00;
			Seed_Fine = 0x1A;
		}
		while(ByteLane < MAX_BYTE_LANES)
		{
			/* Program an initialization value to registers F2x[1, 0]9C_x[51:50] and
			 * F2x[1, 0]9C_x52 to set the gross and fine delay for all the byte lane fields
			 * If the target frequency is different than 400MHz, BIOS must
			 * execute two training passes for each DIMM.
			 * For pass 1 at a 400MHz MEMCLK frequency, use an initial total delay value
			 * of 01Fh. This represents a 1UI (UI=.5MEMCLK) delay and is determined
			 * by design.
			 */
			pDCTData->WLGrossDelay[MAX_BYTE_LANES*dimm+ByteLane] = Seed_Gross;
			pDCTData->WLFineDelay[MAX_BYTE_LANES*dimm+ByteLane] = Seed_Fine;
			ByteLane++;
		}
	} else if (pDCTData->Status[DCT_STATUS_REGISTERED]) {		/* For Pass 2 */
		/* From BKDG, Write Leveling Seed Value. */
		/* TODO: The unbuffered DIMMs are unstable on the code below. So temporarily it is
		 * only for registered DIMMs. */
		u32 RegisterDelay, SeedTotal;
		u8 MemClkFreq;
		u16 freq_tab[] = {400, 533, 667, 800};
		while(ByteLane < MAX_BYTE_LANES)
		{
			MemClkFreq = get_Bits(pDCTData, pDCTData->CurrDct, pDCTData->NodeId,
					      FUN_DCT, DRAM_CONFIG_HIGH, 0, 2);
			if (pDCTData->Status[DCT_STATUS_REGISTERED])
				RegisterDelay = 0x20; /* TODO: ((RCW2 & BIT0) == 0) ? 0x20 : 0x30; */
			else
				RegisterDelay = 0;
			SeedTotal = (pDCTData->WLFineDelay[MAX_BYTE_LANES*dimm+ByteLane] & 0x1F) |
				pDCTData->WLGrossDelay[MAX_BYTE_LANES*dimm+ByteLane] << 5;
			/* SeedTotalPreScaling = (the total delay value in F2x[1, 0]9C_x[4A:30] from pass 1 of write levelization
			   training) - RegisterDelay. */
			/* MemClkFreq: 3: 400Mhz; 4: 533Mhz; 5: 667Mhz; 6: 800Mhz */
			SeedTotal = (u16) (RegisterDelay + ((((u32) SeedTotal - RegisterDelay) *
							     freq_tab[MemClkFreq-3]) / 400));
			Seed_Gross = (SeedTotal & 0x20) != 0 ? 1 : 2;
			Seed_Fine = SeedTotal & 0x1F;
			pDCTData->WLGrossDelay[MAX_BYTE_LANES*dimm+ByteLane] = Seed_Gross;
			pDCTData->WLFineDelay[MAX_BYTE_LANES*dimm+ByteLane] = Seed_Fine;
			ByteLane ++;
		}
	}

	setWLByteDelay(pDCTData, ByteLane, dimm, 0);
}

/*-----------------------------------------------------------------------------
 *  void setWLByteDelay(DCTStruct *DCTData, u8 ByteLane, u8 Dimm){
 *
 *  Description:
 *       This function writes the write levelization byte delay for the Phase
 *       Recovery control registers
 *
 *   Parameters:
 *       IN  OUT   *DCTData - Pointer to buffer with information about each DCT
 *       IN	Dimm - Dimm Number
 *		 DCTData->WLGrossDelay[index+ByteLane] - gross write delay for each
 *						     logical DIMM
 *		 DCTData->WLFineDelay[index+ByteLane] - fine write delay for each
 *						    logical DIMM
 *		 ByteLane - target byte lane to write
 *	  targetAddr -    0: write to DRAM phase recovery control register
 *			  1: write to DQS write register
 *       OUT
 *
 *-----------------------------------------------------------------------------
 */
void setWLByteDelay(sDCTStruct *pDCTData, u8 ByteLane, u8 dimm, u8 targetAddr)
{
	u8 fineStartLoc, fineEndLoc, grossStartLoc, grossEndLoc, tempB, index, offsetAddr;
	u32 addr, fineDelayValue, grossDelayValue, ValueLow, ValueHigh, EccValue, tempW;

	if (targetAddr == 0)
	{
		index = (u8)(MAX_BYTE_LANES * dimm);
		ValueLow = 0;
		ValueHigh = 0;
		ByteLane = 0;
		EccValue = 0;
		while (ByteLane < MAX_BYTE_LANES)
		{
			/* This subtract 0xC workaround might be temporary. */
			if ((pDCTData->WLPass==2) && (pDCTData->RegMan1Present & (1<<(dimm*2+pDCTData->DctTrain))))
			{
				tempW = (pDCTData->WLGrossDelay[index+ByteLane] << 5) | pDCTData->WLFineDelay[index+ByteLane];
				tempW -= 0xC;
				pDCTData->WLGrossDelay[index+ByteLane] = (u8)(tempW >> 5);
				pDCTData->WLFineDelay[index+ByteLane] = (u8)(tempW & 0x1F);
			}
			grossDelayValue = pDCTData->WLGrossDelay[index+ByteLane];
			/* Adjust seed gross delay overflow (greater than 3):
			 *      - Program seed gross delay as 2 (gross is 4 or 6) or 1 (gross is 5).
			 *      - Keep original seed gross delay for later reference.
			 */
			if(grossDelayValue >= 3)
			{
				grossDelayValue = (grossDelayValue&1)? 1 : 2;
			}
			fineDelayValue = pDCTData->WLFineDelay[index+ByteLane];
			if (ByteLane < 4)
				ValueLow |= ((grossDelayValue << 5) | fineDelayValue) << 8*ByteLane;
			else if(ByteLane < 8)
				ValueHigh |= ((grossDelayValue << 5) | fineDelayValue) << 8*(ByteLane-4);
			else
				EccValue = ((grossDelayValue << 5) | fineDelayValue);
			ByteLane++;
		}
		set_DCT_ADDR_Bits(pDCTData, pDCTData->DctTrain, pDCTData->NodeId, FUN_DCT,
				DRAM_CONT_ADD_PHASE_REC_CTRL_LOW, 0, 31, (u32)ValueLow);
		set_DCT_ADDR_Bits(pDCTData, pDCTData->DctTrain, pDCTData->NodeId, FUN_DCT,
				DRAM_CONT_ADD_PHASE_REC_CTRL_HIGH, 0, 31, (u32)ValueHigh);
		set_DCT_ADDR_Bits(pDCTData, pDCTData->DctTrain, pDCTData->NodeId, FUN_DCT,
				DRAM_CONT_ADD_ECC_PHASE_REC_CTRL, 0, 31, (u32)EccValue);
	}
	else
	{
		index = (u8)(MAX_BYTE_LANES * dimm);
		grossDelayValue = pDCTData->WLGrossDelay[index+ByteLane];
		fineDelayValue = pDCTData->WLFineDelay[index+ByteLane];

		tempB = 0;
		offsetAddr = (u8)(3 * dimm);
		if (ByteLane < 2)
		{
			tempB = (u8)(16 * ByteLane);
			addr = DRAM_CONT_ADD_DQS_TIMING_CTRL_BL_01;
		}
		else if (ByteLane <4)
		{
			tempB = (u8)(16 * ByteLane);
			addr = DRAM_CONT_ADD_DQS_TIMING_CTRL_BL_01 + 1;
		}
		else if (ByteLane <6)
		{
			tempB = (u8)(16 * ByteLane);
			addr = DRAM_CONT_ADD_DQS_TIMING_CTRL_BL_45;
		}
		else if (ByteLane <8)
		{
			tempB = (u8)(16 * ByteLane);
			addr = DRAM_CONT_ADD_DQS_TIMING_CTRL_BL_45 + 1;
		}
		else
		{
			tempB = 0;
			addr = DRAM_CONT_ADD_DQS_TIMING_CTRL_BL_01 + 2;
		}
		addr += offsetAddr;

		fineStartLoc = (u8)(tempB % 32);
		fineEndLoc = (u8)(fineStartLoc + 4);
		grossStartLoc = (u8)(fineEndLoc + 1);
		grossEndLoc = (u8)(grossStartLoc + 1);

		set_DCT_ADDR_Bits(pDCTData, pDCTData->DctTrain, pDCTData->NodeId, FUN_DCT,
				(u16)addr, fineStartLoc, fineEndLoc,(u32)fineDelayValue);
		set_DCT_ADDR_Bits(pDCTData, pDCTData->DctTrain, pDCTData->NodeId, FUN_DCT,
				(u16)addr, grossStartLoc, grossEndLoc, (u32)grossDelayValue);
	}

}

/*-----------------------------------------------------------------------------
 *  void getWLByteDelay(DCTStruct *DCTData, u8 ByteLane, u8 Dimm)
 *
 *  Description:
 *       This function reads the write levelization byte delay from the Phase
 *       Recovery control registers
 *
 *   Parameters:
 *       IN  OUT   *DCTData - Pointer to buffer with information about each DCT
 *       IN	Dimm - Dimm Number
 *		 ByteLane - target byte lane to read
 *       OUT
 *		 DCTData->WLGrossDelay[index+ByteLane] - gross write delay for current
 *						     byte for logical DIMM
 *		 DCTData->WLFineDelay[index+ByteLane] - fine write delay for current
 *						    byte for logical DIMM
 *
 *-----------------------------------------------------------------------------
 */
void getWLByteDelay(sDCTStruct *pDCTData, u8 ByteLane, u8 dimm)
{
	u8 fineStartLoc, fineEndLoc, grossStartLoc, grossEndLoc, tempB, tempB1, index;
	u32 addr, fine, gross;
	tempB = 0;
	index = (u8)(MAX_BYTE_LANES*dimm);
	if (ByteLane < 4)
	{
		tempB = (u8)(8 * ByteLane);
		addr = DRAM_CONT_ADD_PHASE_REC_CTRL_LOW;
	}
	else if (ByteLane < 8)
	{
		tempB1 = (u8)(ByteLane - 4);
		tempB = (u8)(8 * tempB1);
		addr = DRAM_CONT_ADD_PHASE_REC_CTRL_HIGH;
	}
	else
	{
		tempB = 0;
		addr = DRAM_CONT_ADD_ECC_PHASE_REC_CTRL;
	}
	fineStartLoc = tempB;
	fineEndLoc = (u8)(fineStartLoc + 4);
	grossStartLoc = (u8)(fineEndLoc + 1);
	grossEndLoc = (u8)(grossStartLoc + 1);

	fine = get_ADD_DCT_Bits(pDCTData, pDCTData->DctTrain, pDCTData->NodeId,
				FUN_DCT, (u16)addr, fineStartLoc, fineEndLoc);
	gross = get_ADD_DCT_Bits(pDCTData, pDCTData->DctTrain, pDCTData->NodeId,
				FUN_DCT, (u16)addr, grossStartLoc, grossEndLoc);
	/* Adjust seed gross delay overflow (greater than 3):
	 * - Adjust the trained gross delay to the original seed gross delay.
	 */
	if(pDCTData->WLGrossDelay[index+ByteLane] >= 3)
	{
		gross += pDCTData->WLGrossDelay[index+ByteLane];
		if(pDCTData->WLGrossDelay[index+ByteLane] & 1)
			gross -= 1;
		else
			gross -= 2;
	}
	else if((pDCTData->WLGrossDelay[index+ByteLane] == 0) && (gross == 3))
	{
		/* If seed gross delay is 0 but PRE result gross delay is 3, it is negative.
		 * We will then round the negative number to 0.
		 */
		gross = 0;
		fine = 0;
	}
	pDCTData->WLFineDelay[index+ByteLane] = (u8)fine;
	pDCTData->WLGrossDelay[index+ByteLane] = (u8)gross;
}
