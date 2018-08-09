/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
 * Copyright (C) 2015 - 2016 Raptor Engineering, LLC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <inttypes.h>
#include <console/console.h>
#include <string.h>
#include "mct_d.h"
#include "mct_d_gcc.h"
#include "mwlc_d.h"
#include <northbridge/amd/amdfam10/amdfam10.h>

u32 swapAddrBits_wl(struct DCTStatStruc *pDCTstat, uint8_t dct, uint32_t MRSValue);
u32 swapBankBits(struct DCTStatStruc *pDCTstat, uint8_t dct, uint32_t MRSValue);
void prepareDimms(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat,
	u8 dct, u8 dimm, BOOL wl);
void programODT(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat, uint8_t dct, u8 dimm);
void procConfig(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat, uint8_t dct, uint8_t dimm, uint8_t pass, uint8_t nibble);
void setWLByteDelay(struct DCTStatStruc *pDCTstat, uint8_t dct, u8 ByteLane, u8 dimm, u8 targetAddr, uint8_t pass, uint8_t lane_count);
void getWLByteDelay(struct DCTStatStruc *pDCTstat, uint8_t dct, u8 ByteLane, u8 dimm, uint8_t pass, uint8_t nibble, uint8_t lane_count);

/*-----------------------------------------------------------------------------
 * uint8_t AgesaHwWlPhase1(SPDStruct *SPDData,MCTStruct *MCTData, DCTStruct *DCTData,
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
uint8_t AgesaHwWlPhase1(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat,
		u8 dct, u8 dimm, u8 pass)
{
	u8 ByteLane;
	u32 Value, Addr;
	uint8_t nibble = 0;
	uint8_t train_both_nibbles;
	u16 Addl_Data_Offset, Addl_Data_Port;
	sMCTStruct *pMCTData = pDCTstat->C_MCTPtr;
	sDCTStruct *pDCTData = pDCTstat->C_DCTPtr[dct];
	uint8_t lane_count;

	lane_count = get_available_lane_count(pMCTstat, pDCTstat);

	pDCTData->WLPass = pass;
	/* 1. Specify the target DIMM that is to be trained by programming
	 * F2x[1, 0]9C_x08[TrDimmSel].
	 */
	set_DCT_ADDR_Bits(pDCTData, dct, pDCTData->NodeId, FUN_DCT,
			DRAM_ADD_DCT_PHY_CONTROL_REG, TrDimmSelStart,
			TrDimmSelEnd, (u32)dimm);

	train_both_nibbles = 0;
	if (pDCTstat->Dimmx4Present)
		if (is_fam15h())
			train_both_nibbles = 1;

	for (nibble = 0; nibble < (train_both_nibbles + 1); nibble++) {
		printk(BIOS_SPEW, "AgesaHwWlPhase1: training nibble %d\n", nibble);

		if (is_fam15h()) {
			/* Program F2x[1, 0]9C_x08[WrtLvTrEn]=0 */
			set_DCT_ADDR_Bits(pDCTData, dct, pDCTData->NodeId, FUN_DCT,
					DRAM_ADD_DCT_PHY_CONTROL_REG, WrtLvTrEn, WrtLvTrEn, 0);

			/* Set TrNibbleSel */
			set_DCT_ADDR_Bits(pDCTData, dct, pDCTData->NodeId, FUN_DCT,
					DRAM_ADD_DCT_PHY_CONTROL_REG, 2,
					2, (uint32_t)nibble);
		}

		/* 2. Prepare the DIMMs for write levelization using DDR3-defined
		 * MR commands. */
		prepareDimms(pMCTstat, pDCTstat, dct, dimm, TRUE);

		/* 3. After the DIMMs are configured, BIOS waits 40 MEMCLKs to
		 *    satisfy DDR3-defined internal DRAM timing.
		 */
		if (is_fam15h())
			precise_memclk_delay_fam15(pMCTstat, pDCTstat, dct, 40);
		else
			pMCTData->AgesaDelay(40);

		/* 4. Configure the processor's DDR phy for write levelization training: */
		procConfig(pMCTstat, pDCTstat, dct, dimm, pass, nibble);

		/* 5. Begin write levelization training:
		 *  Program F2x[1, 0]9C_x08[WrtLvTrEn]=1. */
		if (pDCTData->LogicalCPUID & (AMD_DR_Cx | AMD_DR_Dx | AMD_FAM15_ALL))
		{
			set_DCT_ADDR_Bits(pDCTData, dct, pDCTData->NodeId, FUN_DCT,
					DRAM_ADD_DCT_PHY_CONTROL_REG, WrtLvTrEn, WrtLvTrEn, 1);
		}
		else
		{
			/* Broadcast write to all D3Dbyte chipset register offset 0xc
			 * Set bit 0 (wrTrain)
			 * Program bit 4 to nibble being trained (only matters for x4dimms)
			 * retain value of 3:2 (Trdimmsel)
			 * reset bit 5 (FrzPR)
			 */
			if (dct)
			{
				Addl_Data_Offset = 0x198;
				Addl_Data_Port = 0x19C;
			}
			else
			{
				Addl_Data_Offset = 0x98;
				Addl_Data_Port = 0x9C;
			}
			Addr = 0x0D00000C;
			AmdMemPCIWriteBits(MAKE_SBDFO(0,0,24+(pDCTData->NodeId),FUN_DCT,Addl_Data_Offset), 31, 0, &Addr);
			while ((get_Bits(pDCTData,FUN_DCT,pDCTData->NodeId, FUN_DCT, Addl_Data_Offset,
					DctAccessDone, DctAccessDone)) == 0);
			AmdMemPCIReadBits(MAKE_SBDFO(0,0,24+(pDCTData->NodeId),FUN_DCT,Addl_Data_Port), 31, 0, &Value);
			Value = bitTestSet(Value, 0);	/* enable WL training */
			Value = bitTestReset(Value, 4); /* for x8 only */
			Value = bitTestReset(Value, 5); /* for hardware WL training */
			AmdMemPCIWriteBits(MAKE_SBDFO(0,0,24+(pDCTData->NodeId),FUN_DCT,Addl_Data_Port), 31, 0, &Value);
			Addr = 0x4D030F0C;
			AmdMemPCIWriteBits(MAKE_SBDFO(0,0,24+(pDCTData->NodeId),FUN_DCT,Addl_Data_Offset), 31, 0, &Addr);
			while ((get_Bits(pDCTData,FUN_DCT,pDCTData->NodeId, FUN_DCT, Addl_Data_Offset,
					DctAccessDone, DctAccessDone)) == 0);
		}

		if (is_fam15h())
			proc_MFENCE();

		/* Wait 200 MEMCLKs. If executing pass 2, wait 32 MEMCLKs. */
		if (is_fam15h())
			precise_memclk_delay_fam15(pMCTstat, pDCTstat, dct, 200);
		else
			pMCTData->AgesaDelay(140);

		/* Program F2x[1, 0]9C_x08[WrtLevelTrEn]=0. */
		set_DCT_ADDR_Bits(pDCTData, dct, pDCTData->NodeId, FUN_DCT,
				DRAM_ADD_DCT_PHY_CONTROL_REG, WrtLvTrEn, WrtLvTrEn, 0);

		/* Read from registers F2x[1, 0]9C_x[51:50] and F2x[1, 0]9C_x52
		 * to get the gross and fine delay settings
		 * for the target DIMM and save these values. */
		for (ByteLane = 0; ByteLane < lane_count; ByteLane++) {
			getWLByteDelay(pDCTstat, dct, ByteLane, dimm, pass, nibble, lane_count);
		}

		pDCTData->WLCriticalGrossDelayPrevPass = 0x0;

		/* Exit nibble training if current DIMM is not x4 */
		if ((pDCTstat->Dimmx4Present & (1 << (dimm + dct))) == 0)
			break;
	}

	return 0;
}

uint8_t AgesaHwWlPhase2(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat,
		uint8_t dct, uint8_t dimm, uint8_t pass)
{
	u8 ByteLane;
	uint8_t status = 0;
	sDCTStruct *pDCTData = pDCTstat->C_DCTPtr[dct];
	uint8_t lane_count;

	lane_count = get_available_lane_count(pMCTstat, pDCTstat);

	if (is_fam15h()) {
		int32_t gross_diff[lane_count];
		int32_t cgd = pDCTData->WLCriticalGrossDelayPrevPass;
		uint8_t index = (uint8_t)(lane_count * dimm);

		printk(BIOS_SPEW, "\toriginal critical gross delay: %d\n", cgd);

		/* FIXME
		 * For now, disable CGD adjustment as it seems to interfere with registered DIMM training
		 */

		/* Calculate the Critical Gross Delay */
		for (ByteLane = 0; ByteLane < lane_count; ByteLane++) {
			/* Calculate the gross delay differential for this lane */
			gross_diff[ByteLane] = pDCTData->WLSeedGrossDelay[index+ByteLane] + pDCTData->WLGrossDelay[index+ByteLane];
			gross_diff[ByteLane] -= pDCTData->WLSeedPreGrossDelay[index+ByteLane];

			/* WrDqDqsEarly values greater than 2 are reserved */
			if (gross_diff[ByteLane] < -2)
				gross_diff[ByteLane] = -2;

			/* Update the Critical Gross Delay */
			if (gross_diff[ByteLane] < cgd)
				cgd = gross_diff[ByteLane];
		}

		printk(BIOS_SPEW, "\tnew critical gross delay: %d\n", cgd);

		pDCTData->WLCriticalGrossDelayPrevPass = cgd;

		if (pDCTstat->Speed != pDCTstat->TargetFreq) {
			/* FIXME
			 * Using the Pass 1 training values causes major phy training problems on
			 * all Family 15h processors I tested (Pass 1 values are randomly too high,
			 * and Pass 2 cannot lock).
			 * Figure out why this is and fix it, then remove the bypass code below...
			 */
			if (pass == FirstPass) {
				for (ByteLane = 0; ByteLane < lane_count; ByteLane++) {
					pDCTData->WLGrossDelay[index+ByteLane] = pDCTData->WLSeedGrossDelay[index+ByteLane];
					pDCTData->WLFineDelay[index+ByteLane] = pDCTData->WLSeedFineDelay[index+ByteLane];
				}
				return 0;
			}
		}

		/* Compensate for occasional noise/instability causing sporadic training failure */
		for (ByteLane = 0; ByteLane < lane_count; ByteLane++) {
			uint8_t faulty_value_detected = 0;
			uint16_t total_delay_seed = ((pDCTData->WLSeedGrossDelay[index+ByteLane] & 0x1f) << 5) | (pDCTData->WLSeedFineDelay[index+ByteLane] & 0x1f);
			uint16_t total_delay_phy = ((pDCTData->WLGrossDelay[index+ByteLane] & 0x1f) << 5) | (pDCTData->WLFineDelay[index+ByteLane] & 0x1f);
			if (pass == FirstPass) {
				/* Allow a somewhat higher step threshold on the first pass
				 * For the most part, as long as the phy isn't stepping
				 * several clocks at once the values are probably valid.
				 */
				if (abs(total_delay_phy - total_delay_seed) > 0x30)
					faulty_value_detected = 1;
			} else {
				/* Stepping memory clocks between adjacent allowed frequencies
				 *  should not yield large phy value differences...
				 */

				if (abs(total_delay_phy - total_delay_seed) > 0x20)
					faulty_value_detected = 1;
			}
			if (faulty_value_detected) {
				printk(BIOS_INFO, "%s: overriding faulty phy value (seed: %04x phy: %04x step: %04x)\n", __func__,
					total_delay_seed, total_delay_phy, abs(total_delay_phy - total_delay_seed));
				pDCTData->WLGrossDelay[index+ByteLane] = pDCTData->WLSeedGrossDelay[index+ByteLane];
				pDCTData->WLFineDelay[index+ByteLane] = pDCTData->WLSeedFineDelay[index+ByteLane];
				status = 1;
			}
		}
	}

	return status;
}

uint8_t AgesaHwWlPhase3(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat,
		u8 dct, u8 dimm, u8 pass)
{
	u8 ByteLane;
	sMCTStruct *pMCTData = pDCTstat->C_MCTPtr;
	sDCTStruct *pDCTData = pDCTstat->C_DCTPtr[dct];
	uint8_t lane_count;

	lane_count = get_available_lane_count(pMCTstat, pDCTstat);

	if (is_fam15h()) {
		uint32_t dword;
		int32_t gross_diff[lane_count];
		int32_t cgd = pDCTData->WLCriticalGrossDelayPrevPass;
		uint8_t index = (uint8_t)(lane_count * dimm);

		/* Apply offset(s) if needed */
		if (cgd < 0) {
			dword = Get_NB32_DCT(pDCTstat->dev_dct, dct, 0xa8);
			dword &= ~(0x3 << 24);			/* WrDqDqsEarly = abs(cgd) */
			dword |= ((abs(cgd) & 0x3) << 24);
			Set_NB32_DCT(pDCTstat->dev_dct, dct, 0xa8, dword);

			for (ByteLane = 0; ByteLane < lane_count; ByteLane++) {
				/* Calculate the gross delay differential for this lane */
				gross_diff[ByteLane] = pDCTData->WLSeedGrossDelay[index+ByteLane] + pDCTData->WLGrossDelay[index+ByteLane];
				gross_diff[ByteLane] -= pDCTData->WLSeedPreGrossDelay[index+ByteLane];

				/* Prevent underflow in the presence of noise / instability */
				if (gross_diff[ByteLane] < cgd)
					gross_diff[ByteLane] = cgd;

				pDCTData->WLGrossDelay[index+ByteLane] = (gross_diff[ByteLane] + (abs(cgd) & 0x3));
			}
		} else {
			dword = Get_NB32_DCT(pDCTstat->dev_dct, dct, 0xa8);
			dword &= ~(0x3 << 24);			/* WrDqDqsEarly = pDCTData->WrDqsGrossDlyBaseOffset */
			dword |= ((pDCTData->WrDqsGrossDlyBaseOffset & 0x3) << 24);
			Set_NB32_DCT(pDCTstat->dev_dct, dct, 0xa8, dword);
		}
	}

	/* Write the adjusted gross and fine delay settings
	 * to the target DIMM. */
	for (ByteLane = 0; ByteLane < lane_count; ByteLane++) {
		setWLByteDelay(pDCTstat, dct, ByteLane, dimm, 1, pass, lane_count);
	}

	/* 6. Configure DRAM Phy Control Register so that the phy stops driving
	 *    write levelization ODT. */
	set_DCT_ADDR_Bits(pDCTData, dct, pDCTData->NodeId, FUN_DCT,
			DRAM_ADD_DCT_PHY_CONTROL_REG, WrLvOdtEn, WrLvOdtEn, 0);

	if (is_fam15h())
		proc_MFENCE();

	/* Wait 10 MEMCLKs to allow for ODT signal settling. */
	if (is_fam15h())
		precise_memclk_delay_fam15(pMCTstat, pDCTstat, dct, 10);
	else
		pMCTData->AgesaDelay(10);

	/* 7. Program the target DIMM back to normal operation by configuring
	 * the following (See section 2.8.5.4.1.1
	 * [Phy Assisted Write Levelization] on page 97 pass 1, step #2):
	 * Configure all ranks of the target DIMM for normal operation.
	 * Enable the output drivers of all ranks of the target DIMM.
	 * For a two DIMM system, program the Rtt value for the target DIMM
	 * to the normal operating termination:
	 */
	prepareDimms(pMCTstat, pDCTstat, dct, dimm, FALSE);

	return 0;
}

/*----------------------------------------------------------------------------
 *	LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/*-----------------------------------------------------------------------------
 * u32 swapAddrBits_wl(struct DCTStatStruc *pDCTstat, uint8_t dct, u32 MRSValue)
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
u32 swapAddrBits_wl(struct DCTStatStruc *pDCTstat, uint8_t dct, uint32_t MRSValue)
{
	sDCTStruct *pDCTData = pDCTstat->C_DCTPtr[dct];
	u32 tempW, tempW1;

	if (is_fam15h())
		tempW1 = get_Bits(pDCTData, dct, pDCTData->NodeId,
				FUN_DCT, DRAM_INIT, MrsChipSelStartFam15, MrsChipSelEndFam15);
	else
		tempW1 = get_Bits(pDCTData, dct, pDCTData->NodeId,
			FUN_DCT, DRAM_INIT, MrsChipSelStartFam10, MrsChipSelEndFam10);
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
			MRSValue |= (tempW << 1);
			MRSValue |= (tempW1 >> 1);
		}
	}
	return MRSValue;
}

/*-----------------------------------------------------------------------------
 *  u32 swapBankBits(struct DCTStatStruc *pDCTstat, uint8_t dct, u32 MRSValue)
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
u32 swapBankBits(struct DCTStatStruc *pDCTstat, uint8_t dct, u32 MRSValue)
{
	sDCTStruct *pDCTData = pDCTstat->C_DCTPtr[dct];
	u32 tempW, tempW1;

	if (is_fam15h())
		tempW1 = get_Bits(pDCTData, dct, pDCTData->NodeId,
				FUN_DCT, DRAM_INIT, MrsChipSelStartFam15, MrsChipSelEndFam15);
	else
		tempW1 = get_Bits(pDCTData, dct, pDCTData->NodeId,
				FUN_DCT, DRAM_INIT, MrsChipSelStartFam10, MrsChipSelEndFam10);
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
			MRSValue |= (tempW << 1);
			MRSValue |= (tempW1 >> 1);
		}
	}
	return MRSValue;
}

static uint16_t unbuffered_dimm_nominal_termination_emrs(uint8_t number_of_dimms, uint8_t frequency_index, uint8_t rank_count, uint8_t rank)
{
	uint16_t term;

	uint8_t MaxDimmsInstallable = mctGet_NVbits(NV_MAX_DIMMS_PER_CH);

	if (number_of_dimms == 1) {
		if (MaxDimmsInstallable < 3) {
			term = 0x04;	/* Rtt_Nom = RZQ/4 = 60 Ohm */
		} else {
			if (rank_count == 1) {
				term = 0x04;	/* Rtt_Nom = RZQ/4 = 60 Ohm */
			} else {
				if (rank == 0)
					term = 0x04;	/* Rtt_Nom = RZQ/4 = 60 Ohm */
				else
					term = 0x00;	/* Rtt_Nom = OFF */
			}
		}
	} else {
		if (frequency_index < 5)
			term = 0x0044;	/* Rtt_Nom = RZQ/6 = 40 Ohm */
		else
			term = 0x0204;	/* Rtt_Nom = RZQ/8 = 30 Ohm */
	}

	return term;
}

static uint16_t unbuffered_dimm_dynamic_termination_emrs(uint8_t number_of_dimms, uint8_t frequency_index, uint8_t rank_count)
{
	uint16_t term;

	uint8_t MaxDimmsInstallable = mctGet_NVbits(NV_MAX_DIMMS_PER_CH);

	if (number_of_dimms == 1) {
		if (MaxDimmsInstallable < 3) {
			term = 0x00;	/* Rtt_WR = off */
		} else {
			if (rank_count == 1)
				term = 0x00;	/* Rtt_WR = off */
			else
				term = 0x200;	/* Rtt_WR = RZQ/4 = 60 Ohm */
		}
	} else {
		term = 0x400;	/* Rtt_WR = RZQ/2 = 120 Ohm */
	}

	return term;
}

/*-----------------------------------------------------------------------------
 *  void prepareDimms(sMCTStruct *pMCTData, sDCTStruct *DCTData, u8 Dimm, BOOL WL)
 *
 *  Description:
 *       This function prepares DIMMS for training
 *       Fam10h: BKDG Rev. 3.62 section 2.8.9.9.1
 *       Fam15h: BKDG Rev. 3.14 section 2.10.5.8.1
 * ----------------------------------------------------------------------------
 */
void prepareDimms(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat,
	u8 dct, u8 dimm, BOOL wl)
{
	u32 tempW, tempW1, tempW2, MrsBank;
	u8 rank, currDimm, MemClkFreq;
	sMCTStruct *pMCTData = pDCTstat->C_MCTPtr;
	sDCTStruct *pDCTData = pDCTstat->C_DCTPtr[dct];
	uint8_t package_type = mctGet_NVbits(NV_PACK_TYPE);
	uint8_t number_of_dimms = pDCTData->MaxDimmsInstalled;

	if (is_fam15h()) {
		MemClkFreq = get_Bits(pDCTData, dct, pDCTData->NodeId,
			FUN_DCT, DRAM_CONFIG_HIGH, 0, 4);
	} else {
		MemClkFreq = get_Bits(pDCTData, dct, pDCTData->NodeId,
			FUN_DCT, DRAM_CONFIG_HIGH, 0, 2);
	}
	/* Configure the DCT to send initialization MR commands to the target DIMM
	 * by programming the F2x[1,0]7C register using the following steps.
	 */
	rank = 0;
	while ((rank < pDCTData->DimmRanks[dimm]) && (rank < 2))
	{
		/* Program F2x[1, 0]7C[MrsChipSel[2:0]] for the current rank to be trained. */
		if (is_fam15h())
			set_Bits(pDCTData, dct, pDCTData->NodeId, FUN_DCT,
				DRAM_INIT, MrsChipSelStartFam15, MrsChipSelEndFam15, dimm*2+rank);
		else
			set_Bits(pDCTData, dct, pDCTData->NodeId, FUN_DCT,
				DRAM_INIT, MrsChipSelStartFam10, MrsChipSelEndFam10, dimm*2+rank);

		/* Program F2x[1, 0]7C[MrsBank[2:0]] for the appropriate internal DRAM
		 * register that defines the required DDR3-defined function for write
		 * levelization.
		 */
		MrsBank = swapBankBits(pDCTstat, dct, 1);
		if (is_fam15h())
			set_Bits(pDCTData, dct, pDCTData->NodeId, FUN_DCT,
				DRAM_INIT, MrsBankStartFam15, MrsBankEndFam15, MrsBank);
		else
			set_Bits(pDCTData, dct, pDCTData->NodeId, FUN_DCT,
				DRAM_INIT, MrsBankStartFam10, MrsBankEndFam10, MrsBank);

		/* Program F2x[1, 0]7C[MrsAddress[15:0]] to the required DDR3-defined function
		 * for write levelization.
		 */
		tempW = 0;/* DLL_DIS = 0, DIC = 0, AL = 0, TDQS = 0 */

		/* Retrieve normal settings of the MRS control word and clear Rtt_Nom */
		if (is_fam15h()) {
			tempW = mct_MR1(pMCTstat, pDCTstat, dct, dimm*2+rank) & 0xffff;
			tempW &= ~(0x0244);
		} else {
			/* Set TDQS = 1b for x8 DIMM, TDQS = 0b for x4 DIMM, when mixed x8 & x4 */
			tempW2 = get_Bits(pDCTData, dct, pDCTData->NodeId,
					FUN_DCT, DRAM_CONFIG_HIGH, RDqsEn, RDqsEn);
			if (tempW2)
			{
				if (pDCTData->DimmX8Present[dimm])
					tempW |= 0x800;
			}
		}

		/* determine Rtt_Nom for WL & Normal mode */
		if (is_fam15h()) {
			if (wl) {
				if (number_of_dimms > 1) {
					if (rank == 0) {
						/* Get Rtt_WR for the current DIMM and rank */
						tempW2 = fam15_rttwr(pDCTstat, dct, dimm, rank, package_type);
					} else {
						tempW2 = fam15_rttnom(pDCTstat, dct, dimm, rank, package_type);
					}
				} else {
					tempW2 = fam15_rttnom(pDCTstat, dct, dimm, rank, package_type);
				}
			} else {
				tempW2 = fam15_rttnom(pDCTstat, dct, dimm, rank, package_type);
			}
			tempW1 = 0;
			tempW1 |= ((tempW2 & 0x4) >> 2) << 9;
			tempW1 |= ((tempW2 & 0x2) >> 1) << 6;
			tempW1 |= ((tempW2 & 0x1) >> 0) << 2;
		} else {
			if (pDCTData->Status[DCT_STATUS_REGISTERED]) {
				tempW1 = RttNomTargetRegDimm(pMCTData, pDCTData, dimm, wl, MemClkFreq, rank);
			} else {
				if (wl) {
					if (number_of_dimms > 1) {
						if (rank == 0) {
							/* Get Rtt_WR for the current DIMM and rank */
							uint16_t dynamic_term = unbuffered_dimm_dynamic_termination_emrs(pDCTData->MaxDimmsInstalled, MemClkFreq, pDCTData->DimmRanks[dimm]);

							/* Convert dynamic termination code to corresponding nominal termination code */
							if (dynamic_term == 0x200)
								tempW1 = 0x04;
							else if (dynamic_term == 0x400)
								tempW1 = 0x40;
							else
								tempW1 = 0x0;
						} else {
							tempW1 = unbuffered_dimm_nominal_termination_emrs(pDCTData->MaxDimmsInstalled, MemClkFreq, pDCTData->DimmRanks[dimm], rank);
						}
					} else {
						tempW1 = unbuffered_dimm_nominal_termination_emrs(pDCTData->MaxDimmsInstalled, MemClkFreq, pDCTData->DimmRanks[dimm], rank);
					}
				} else {
					tempW1 = unbuffered_dimm_nominal_termination_emrs(pDCTData->MaxDimmsInstalled, MemClkFreq, pDCTData->DimmRanks[dimm], rank);
				}
			}
		}

		/* Apply Rtt_Nom to the MRS control word */
		tempW = tempW|tempW1;

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
				 * the target DIMM.
				 */
				tempW = bitTestSet(tempW1, Qoff);
			}
		}

		/* Program MrsAddress[5,1]=output driver impedance control (DIC) */
		if (is_fam15h()) {
			tempW1 = fam15_dimm_dic(pDCTstat, dct, dimm, rank, package_type);
		} else {
			/* Read DIC from F2x[1,0]84[DrvImpCtrl] */
			tempW1 = get_Bits(pDCTData, dct, pDCTData->NodeId,
					FUN_DCT, DRAM_MRS_REGISTER, DrvImpCtrlStart, DrvImpCtrlEnd);
		}

		/* Apply DIC to the MRS control word */
		if (bitTest(tempW1, 1))
			tempW = bitTestSet(tempW, 5);
		if (bitTest(tempW1, 0))
			tempW = bitTestSet(tempW, 1);

		tempW = swapAddrBits_wl(pDCTstat, dct, tempW);

		if (is_fam15h())
			set_Bits(pDCTData, dct, pDCTData->NodeId, FUN_DCT,
				DRAM_INIT, MrsAddressStartFam15, MrsAddressEndFam15, tempW);
		else
			set_Bits(pDCTData, dct, pDCTData->NodeId, FUN_DCT,
				DRAM_INIT, MrsAddressStartFam10, MrsAddressEndFam10, tempW);

		/* Program F2x[1, 0]7C[SendMrsCmd]=1 to initiate the command to
		 * the specified DIMM.
		 */
		set_Bits(pDCTData, dct, pDCTData->NodeId, FUN_DCT,
			DRAM_INIT, SendMrsCmd, SendMrsCmd, 1);
		/* Wait for F2x[1, 0]7C[SendMrsCmd] to be cleared by hardware. */
		while ((get_Bits(pDCTData, dct, pDCTData->NodeId,
				FUN_DCT, DRAM_INIT, SendMrsCmd, SendMrsCmd)) == 0x1)
		{
		}

		/* Program F2x[1, 0]7C[MrsBank[2:0]] for the appropriate internal DRAM
		 * register that defines the required DDR3-defined function for Rtt_WR.
		 */
		MrsBank = swapBankBits(pDCTstat, dct, 2);
		if (is_fam15h())
			set_Bits(pDCTData, dct, pDCTData->NodeId, FUN_DCT,
				DRAM_INIT, MrsBankStartFam15, MrsBankEndFam15, MrsBank);
		else
			set_Bits(pDCTData, dct, pDCTData->NodeId, FUN_DCT,
				DRAM_INIT, MrsBankStartFam10, MrsBankEndFam10, MrsBank);

		/* Program F2x[1, 0]7C[MrsAddress[15:0]] to the required DDR3-defined function
		 * for Rtt_WR (DRAMTermDyn).
		 */
		tempW = 0;/* PASR = 0,*/

		/* Retrieve normal settings of the MRS control word and clear Rtt_WR */
		if (is_fam15h()) {
			tempW = mct_MR2(pMCTstat, pDCTstat, dct, dimm*2+rank) & 0xffff;
			tempW &= ~(0x0600);
		} else {
			/* program MrsAddress[7,6,5:3]=SRT,ASR,CWL,
			* based on F2x[1,0]84[19,18,22:20]=,SRT,ASR,Tcwl */
			tempW1 = get_Bits(pDCTData, dct, pDCTData->NodeId,
					FUN_DCT, DRAM_MRS_REGISTER, PCI_MIN_LOW, PCI_MAX_HIGH);
			if (bitTest(tempW1,19))
			{tempW = bitTestSet(tempW, 7);}
			if (bitTest(tempW1,18))
			{tempW = bitTestSet(tempW, 6);}
			/* tempW = tempW|(((tempW1 >> 20) & 0x7 )<< 3); */
			tempW = tempW|((tempW1&0x00700000) >> 17);
			/* workaround for DR-B0 */
			if ((pDCTData->LogicalCPUID & AMD_DR_Bx) && (pDCTData->Status[DCT_STATUS_REGISTERED]))
				tempW+=0x8;
		}

		/* determine Rtt_WR for WL & Normal mode */
		if (is_fam15h()) {
			tempW1 = (fam15_rttwr(pDCTstat, dct, dimm, rank, package_type) << 9);
		} else {
			if (pDCTData->Status[DCT_STATUS_REGISTERED])
				tempW1 = RttWrRegDimm(pMCTData, pDCTData, dimm, wl, MemClkFreq, rank);
			else
				tempW1 = unbuffered_dimm_dynamic_termination_emrs(pDCTData->MaxDimmsInstalled, MemClkFreq, pDCTData->DimmRanks[dimm]);
		}

		/* Apply Rtt_WR to the MRS control word */
		tempW = tempW|tempW1;
		tempW = swapAddrBits_wl(pDCTstat, dct, tempW);
		if (is_fam15h())
			set_Bits(pDCTData, dct, pDCTData->NodeId, FUN_DCT,
				DRAM_INIT, MrsAddressStartFam15, MrsAddressEndFam15, tempW);
		else
			set_Bits(pDCTData, dct, pDCTData->NodeId, FUN_DCT,
				DRAM_INIT, MrsAddressStartFam10, MrsAddressEndFam10, tempW);

		/* Program F2x[1, 0]7C[SendMrsCmd]=1 to initiate the command to
		   the specified DIMM.*/
		set_Bits(pDCTData, dct, pDCTData->NodeId, FUN_DCT,
			DRAM_INIT, SendMrsCmd, SendMrsCmd, 1);

		/* Wait for F2x[1, 0]7C[SendMrsCmd] to be cleared by hardware. */
		while ((get_Bits(pDCTData, dct, pDCTData->NodeId,
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
					if (is_fam15h())
						set_Bits(pDCTData, dct, pDCTData->NodeId,
							FUN_DCT, DRAM_INIT, MrsChipSelStartFam15, MrsChipSelEndFam15, currDimm*2+rank);
					else
						set_Bits(pDCTData, dct, pDCTData->NodeId,
							FUN_DCT, DRAM_INIT, MrsChipSelStartFam10, MrsChipSelEndFam10, currDimm*2+rank);

					/* Program F2x[1, 0]7C[MrsBank[2:0]] for the appropriate internal
					 * DRAM register that defines the required DDR3-defined function
					 * for write levelization.
					 */
					MrsBank = swapBankBits(pDCTstat, dct, 1);
					if (is_fam15h())
						set_Bits(pDCTData, dct, pDCTData->NodeId,
							FUN_DCT, DRAM_INIT, MrsBankStartFam15, MrsBankEndFam15, MrsBank);
					else
						set_Bits(pDCTData, dct, pDCTData->NodeId,
							FUN_DCT, DRAM_INIT, MrsBankStartFam10, MrsBankEndFam10, MrsBank);

					/* Program F2x[1, 0]7C[MrsAddress[15:0]] to the required
					 * DDR3-defined function for write levelization.
					 */
					tempW = 0;/* DLL_DIS = 0, DIC = 0, AL = 0, TDQS = 0, Level = 0, Qoff = 0 */

					/* Retrieve normal settings of the MRS control word and clear Rtt_Nom */
					if (is_fam15h()) {
						tempW = mct_MR1(pMCTstat, pDCTstat, dct, currDimm*2+rank) & 0xffff;
						tempW &= ~(0x0244);
					} else {
						/* Set TDQS = 1b for x8 DIMM, TDQS = 0b for x4 DIMM, when mixed x8 & x4 */
						tempW2 = get_Bits(pDCTData, dct, pDCTData->NodeId,
								FUN_DCT, DRAM_CONFIG_HIGH, RDqsEn, RDqsEn);
						if (tempW2)
						{
							if (pDCTData->DimmX8Present[currDimm])
								tempW |= 0x800;
						}
					}

					/* determine Rtt_Nom for WL & Normal mode */
					if (is_fam15h()) {
						tempW2 = fam15_rttnom(pDCTstat, dct, dimm, rank, package_type);
						tempW1 = 0;
						tempW1 |= ((tempW2 & 0x4) >> 2) << 9;
						tempW1 |= ((tempW2 & 0x2) >> 1) << 6;
						tempW1 |= ((tempW2 & 0x1) >> 0) << 2;
					} else {
						if (pDCTData->Status[DCT_STATUS_REGISTERED])
							tempW1 = RttNomNonTargetRegDimm(pMCTData, pDCTData, currDimm, wl, MemClkFreq, rank);
						else
							tempW1 = unbuffered_dimm_nominal_termination_emrs(pDCTData->MaxDimmsInstalled, MemClkFreq, pDCTData->DimmRanks[currDimm], rank);
					}

					/* Apply Rtt_Nom to the MRS control word */
					tempW = tempW|tempW1;

					/* Program MrsAddress[5,1]=output driver impedance control (DIC) */
					if (is_fam15h()) {
						tempW1 = fam15_dimm_dic(pDCTstat, dct, dimm, rank, package_type);
					} else {
						/* Read DIC from F2x[1,0]84[DrvImpCtrl] */
						tempW1 = get_Bits(pDCTData, dct, pDCTData->NodeId,
								FUN_DCT, DRAM_MRS_REGISTER, DrvImpCtrlStart, DrvImpCtrlEnd);
					}

					/* Apply DIC to the MRS control word */
					if (bitTest(tempW1,1))
					{tempW = bitTestSet(tempW, 5);}
					if (bitTest(tempW1,0))
					{tempW = bitTestSet(tempW, 1);}

					tempW = swapAddrBits_wl(pDCTstat, dct, tempW);

					if (is_fam15h())
						set_Bits(pDCTData, dct, pDCTData->NodeId,
							FUN_DCT, DRAM_INIT, MrsAddressStartFam15, MrsAddressEndFam15, tempW);
					else
						set_Bits(pDCTData, dct, pDCTData->NodeId,
							FUN_DCT, DRAM_INIT, MrsAddressStartFam10, MrsAddressEndFam10, tempW);

					/* Program F2x[1, 0]7C[SendMrsCmd]=1 to initiate the command
					 * to the specified DIMM.
					 */
					set_Bits(pDCTData, dct, pDCTData->NodeId,
						FUN_DCT, DRAM_INIT, SendMrsCmd, SendMrsCmd, 1);

					/* Wait for F2x[1, 0]7C[SendMrsCmd] to be cleared by hardware. */
					while ((get_Bits(pDCTData, dct,
							pDCTData->NodeId, FUN_DCT, DRAM_INIT,
							SendMrsCmd, SendMrsCmd)) == 1);

					/* Program F2x[1, 0]7C[MrsBank[2:0]] for the appropriate internal DRAM
					 * register that defines the required DDR3-defined function for Rtt_WR.
					 */
					MrsBank = swapBankBits(pDCTstat, dct, 2);
					if (is_fam15h())
						set_Bits(pDCTData, dct, pDCTData->NodeId, FUN_DCT,
							DRAM_INIT, MrsBankStartFam15, MrsBankEndFam15, MrsBank);
					else
						set_Bits(pDCTData, dct, pDCTData->NodeId, FUN_DCT,
							DRAM_INIT, MrsBankStartFam10, MrsBankEndFam10, MrsBank);

					/* Program F2x[1, 0]7C[MrsAddress[15:0]] to the required DDR3-defined function
					 * for Rtt_WR (DRAMTermDyn).
					 */
					tempW = 0;/* PASR = 0,*/

					/* Retrieve normal settings of the MRS control word and clear Rtt_WR */
					if (is_fam15h()) {
						tempW = mct_MR2(pMCTstat, pDCTstat, dct, currDimm*2+rank) & 0xffff;
						tempW &= ~(0x0600);
					} else {
						/* program MrsAddress[7,6,5:3]=SRT,ASR,CWL,
						* based on F2x[1,0]84[19,18,22:20]=,SRT,ASR,Tcwl */
						tempW1 = get_Bits(pDCTData, dct, pDCTData->NodeId,
								FUN_DCT, DRAM_MRS_REGISTER, PCI_MIN_LOW, PCI_MAX_HIGH);
						if (bitTest(tempW1,19))
						{tempW = bitTestSet(tempW, 7);}
						if (bitTest(tempW1,18))
						{tempW = bitTestSet(tempW, 6);}
						/* tempW = tempW|(((tempW1 >> 20) & 0x7) << 3); */
						tempW = tempW|((tempW1&0x00700000) >> 17);
						/* workaround for DR-B0 */
						if ((pDCTData->LogicalCPUID & AMD_DR_Bx) && (pDCTData->Status[DCT_STATUS_REGISTERED]))
							tempW+=0x8;
					}

					/* determine Rtt_WR for WL & Normal mode */
					if (is_fam15h()) {
						tempW1 = (fam15_rttwr(pDCTstat, dct, dimm, rank, package_type) << 9);
					} else {
						if (pDCTData->Status[DCT_STATUS_REGISTERED])
							tempW1 = RttWrRegDimm(pMCTData, pDCTData, currDimm, wl, MemClkFreq, rank);
						else
							tempW1 = unbuffered_dimm_dynamic_termination_emrs(pDCTData->MaxDimmsInstalled, MemClkFreq, pDCTData->DimmRanks[currDimm]);
					}

					/* Apply Rtt_WR to the MRS control word */
					tempW = tempW|tempW1;
					tempW = swapAddrBits_wl(pDCTstat, dct, tempW);
					if (is_fam15h())
						set_Bits(pDCTData, dct, pDCTData->NodeId, FUN_DCT,
							DRAM_INIT, MrsAddressStartFam15, MrsAddressEndFam15, tempW);
					else
						set_Bits(pDCTData, dct, pDCTData->NodeId, FUN_DCT,
							DRAM_INIT, MrsAddressStartFam10, MrsAddressEndFam10, tempW);

					/* Program F2x[1, 0]7C[SendMrsCmd]=1 to initiate the command to
					   the specified DIMM.*/
					set_Bits(pDCTData, dct, pDCTData->NodeId, FUN_DCT,
						DRAM_INIT, SendMrsCmd, SendMrsCmd, 1);

					/* Wait for F2x[1, 0]7C[SendMrsCmd] to be cleared by hardware. */
					while ((get_Bits(pDCTData, dct, pDCTData->NodeId,
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
void programODT(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat, uint8_t dct, uint8_t dimm)
{
	sMCTStruct *pMCTData = pDCTstat->C_MCTPtr;
	sDCTStruct *pDCTData = pDCTstat->C_DCTPtr[dct];

	u8 WrLvOdt1 = 0;

	if (is_fam15h()) {
		/* On Family15h processors, the value for the specific CS being targeted
		 * is taken from F2x238 / F2x23C as appropriate, then loaded into F2x9C_x0000_0008
		 */

		/* Convert DIMM number to CS */
		uint32_t dword;
		uint8_t cs;
		uint8_t rank = 0;

		cs = (dimm * 2) + rank;

		/* Fetch preprogammed ODT pattern from configuration registers */
		dword = Get_NB32_DCT(pDCTstat->dev_dct, dct, ((cs > 3)?0x23c:0x238));
		if ((cs == 7) || (cs == 3))
			WrLvOdt1 = ((dword >> 24) & 0xf);
		else if ((cs == 6) || (cs == 2))
			WrLvOdt1 = ((dword >> 16) & 0xf);
		else if ((cs == 5) || (cs == 1))
			WrLvOdt1 = ((dword >> 8) & 0xf);
		else if ((cs == 4) || (cs == 0))
			WrLvOdt1 = (dword & 0xf);
	} else {
		if (pDCTData->Status[DCT_STATUS_REGISTERED]) {
			WrLvOdt1 = WrLvOdtRegDimm(pMCTData, pDCTData, dimm);
		} else {
			if ((pDCTData->DctCSPresent & 0x05) == 0x05) {
				WrLvOdt1 = 0x03;
			} else if (bitTest((u32)pDCTData->DctCSPresent,(u8)(dimm*2+1))) {
				WrLvOdt1 = (u8)bitTestSet(WrLvOdt1, dimm+2);
			} else {
				WrLvOdt1 = (u8)bitTestSet(WrLvOdt1, dimm);
			}
		}
	}

	set_DCT_ADDR_Bits(pDCTData, dct, pDCTData->NodeId, FUN_DCT,
			DRAM_ADD_DCT_PHY_CONTROL_REG, 8, 11, (u32)WrLvOdt1);

	printk(BIOS_SPEW, "Programmed DCT %d write levelling ODT pattern %08x from DIMM %d data\n", dct, WrLvOdt1, dimm);

}

#ifdef UNUSED_CODE
static uint16_t fam15h_next_lowest_memclk_freq(uint16_t memclk_freq)
{
	uint16_t fam15h_next_lowest_freq_tab[] = {0, 0, 0, 0, 0x4, 0, 0x4, 0, 0, 0, 0x6, 0, 0, 0, 0xa, 0, 0, 0, 0xe, 0, 0, 0, 0x12};
	return fam15h_next_lowest_freq_tab[memclk_freq];
}
#endif

/*-----------------------------------------------------------------------------
 * void procConfig(MCTStruct *MCTData,DCTStruct *DCTData, u8 Dimm, u8 Pass, u8 Nibble)
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
void procConfig(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat, uint8_t dct, uint8_t dimm, uint8_t pass, uint8_t nibble)
{
	u8 ByteLane, MemClkFreq;
	int32_t Seed_Gross;
	int32_t Seed_Fine;
	uint8_t Seed_PreGross;
	u32 Value, Addr;
	uint32_t dword;
	u16 Addl_Data_Offset, Addl_Data_Port;
	sMCTStruct *pMCTData = pDCTstat->C_MCTPtr;
	sDCTStruct *pDCTData = pDCTstat->C_DCTPtr[dct];
	uint16_t fam10h_freq_tab[] = {0, 0, 0, 400, 533, 667, 800};
	uint16_t fam15h_freq_tab[] = {0, 0, 0, 0, 333, 0, 400, 0, 0, 0, 533, 0, 0, 0, 667, 0, 0, 0, 800, 0, 0, 0, 933};
	uint8_t lane_count;

	lane_count = get_available_lane_count(pMCTstat, pDCTstat);

	if (is_fam15h()) {
		/* MemClkFreq: 0x4: 333MHz; 0x6: 400MHz; 0xa: 533MHz; 0xe: 667MHz; 0x12: 800MHz; 0x16: 933MHz */
		MemClkFreq = get_Bits(pDCTData, dct, pDCTData->NodeId,
					FUN_DCT, DRAM_CONFIG_HIGH, 0, 4);
	} else {
		/* MemClkFreq: 3: 400MHz; 4: 533MHz; 5: 667MHz; 6: 800MHz */
		MemClkFreq = get_Bits(pDCTData, dct, pDCTData->NodeId,
					FUN_DCT, DRAM_CONFIG_HIGH, 0, 2);
	}

	/* Program F2x[1, 0]9C_x08[WrLvOdt[3:0]] to the proper ODT settings for the
	 * current memory subsystem configuration.
	 */
	programODT(pMCTstat, pDCTstat, dct, dimm);

	/* Program F2x[1,0]9C_x08[WrLvOdtEn]=1 */
	if (pDCTData->LogicalCPUID & (AMD_DR_Cx | AMD_DR_Dx | AMD_FAM15_ALL)) {
		set_DCT_ADDR_Bits(pDCTData, dct, pDCTData->NodeId, FUN_DCT,
				DRAM_ADD_DCT_PHY_CONTROL_REG, WrLvOdtEn, WrLvOdtEn, (u32)1);
	}
	else
	{
		/* Program WrLvOdtEn = 1 through set bit 12 of D3CSODT reg offset 0 for Rev.B */
		if (dct)
		{
			Addl_Data_Offset = 0x198;
			Addl_Data_Port = 0x19C;
		}
		else
		{
			Addl_Data_Offset = 0x98;
			Addl_Data_Port = 0x9C;
		}
		Addr = 0x0D008000;
		AmdMemPCIWriteBits(MAKE_SBDFO(0,0,24+(pDCTData->NodeId),FUN_DCT,Addl_Data_Offset), 31, 0, &Addr);
		while ((get_Bits(pDCTData,FUN_DCT,pDCTData->NodeId, FUN_DCT, Addl_Data_Offset,
				DctAccessDone, DctAccessDone)) == 0);
		AmdMemPCIReadBits(MAKE_SBDFO(0,0,24+(pDCTData->NodeId),FUN_DCT,Addl_Data_Port), 31, 0, &Value);
		Value = bitTestSet(Value, 12);
		AmdMemPCIWriteBits(MAKE_SBDFO(0,0,24+(pDCTData->NodeId),FUN_DCT,Addl_Data_Port), 31, 0, &Value);
		Addr = 0x4D088F00;
		AmdMemPCIWriteBits(MAKE_SBDFO(0,0,24+(pDCTData->NodeId),FUN_DCT,Addl_Data_Offset), 31, 0, &Addr);
		while ((get_Bits(pDCTData,FUN_DCT,pDCTData->NodeId, FUN_DCT, Addl_Data_Offset,
				DctAccessDone, DctAccessDone)) == 0);
	}

	if (is_fam15h())
		proc_MFENCE();

	/* Wait 10 MEMCLKs to allow for ODT signal settling. */
	if (is_fam15h())
		precise_memclk_delay_fam15(pMCTstat, pDCTstat, dct, 10);
	else
		pMCTData->AgesaDelay(10);

	/* Program write levelling seed values */
	if (pass == 1)
	{
		/* Pass 1 */
		if (is_fam15h()) {
			uint8_t AddrCmdPrelaunch = 0;		/* TODO: Fetch the correct value from RC2[0] */
			uint8_t package_type = mctGet_NVbits(NV_PACK_TYPE);
			uint16_t Seed_Total = 0;
			pDCTData->WrDqsGrossDlyBaseOffset = 0x0;
			if (package_type == PT_GR) {
				/* Socket G34: Fam15h BKDG v3.14 Table 96 */
				if (pDCTData->Status[DCT_STATUS_REGISTERED]) {
					/* TODO
					 * Implement mainboard-specific seed and
					 * WrDqsGrossDly base overrides.
					 * 0x41 and 0x0 are the "stock" values
					 */
					Seed_Total = 0x41;
					pDCTData->WrDqsGrossDlyBaseOffset = 0x2;
				} else if (pDCTData->Status[DCT_STATUS_LOAD_REDUCED]) {
					Seed_Total = 0x0;
				} else {
					Seed_Total = 0xf;
				}
			} else if (package_type == PT_C3) {
				/* Socket C32: Fam15h BKDG v3.14 Table 97 */
				if (pDCTData->Status[DCT_STATUS_REGISTERED]) {
					Seed_Total = 0x3e;
				} else if (pDCTData->Status[DCT_STATUS_LOAD_REDUCED]) {
					Seed_Total = 0x0;
				} else {
					Seed_Total = 0x12;
				}
			} else if (package_type == PT_M2) {
				/* Socket AM3: Fam15h BKDG v3.14 Table 98 */
				Seed_Total = 0xf;
			} else if (package_type == PT_FM2) {
				/* Socket FM2: Fam15h M10 BKDG 3.12 Table 42 */
				Seed_Total = 0x15;
			}
			if (pDCTData->Status[DCT_STATUS_REGISTERED])
				Seed_Total += ((AddrCmdPrelaunch)?0x10:0x0);

			/* Adjust seed for the minimum platform supported frequency */
			Seed_Total = (int32_t) (((((int64_t) Seed_Total) *
				fam15h_freq_tab[MemClkFreq] * 100) / (mctGet_NVbits(NV_MIN_MEMCLK) * 100)));

			Seed_Gross = (Seed_Total >> 5) & 0x1f;
			Seed_Fine = Seed_Total & 0x1f;

			/* Save seed values for later use */
			for (ByteLane = 0; ByteLane < lane_count; ByteLane++) {
				pDCTData->WLSeedGrossDelay[lane_count*dimm+ByteLane] = Seed_Gross;
				pDCTData->WLSeedFineDelay[lane_count*dimm+ByteLane] = Seed_Fine;

				if (Seed_Gross == 0)
					Seed_PreGross = 0;
				else if (Seed_Gross & 0x1)
					Seed_PreGross = 1;
				else
					Seed_PreGross = 2;

				pDCTData->WLSeedPreGrossDelay[lane_count*dimm+ByteLane] = Seed_PreGross;
			}
		} else {
			if (pDCTData->Status[DCT_STATUS_REGISTERED]) {
				uint8_t AddrCmdPrelaunch = 0;		/* TODO: Fetch the correct value from RC2[0] */

				/* The seed values below assume Pass 1 utilizes a 400MHz clock frequency (DDR3-800) */
				if (AddrCmdPrelaunch == 0) {
					Seed_Gross = 0x02;
					Seed_Fine = 0x01;
				} else {
					Seed_Gross = 0x02;
					Seed_Fine = 0x11;
				}
			} else {
				if (MemClkFreq == 6) {
					/* DDR-800 */
					Seed_Gross = 0x00;
					Seed_Fine = 0x1a;
				} else {
					/* Use settings for DDR-400 (interpolated from BKDG) */
					Seed_Gross = 0x00;
					Seed_Fine = 0x0d;
				}
			}
		}
		for (ByteLane = 0; ByteLane < lane_count; ByteLane++)
		{
			/* Program an initialization value to registers F2x[1, 0]9C_x[51:50] and
			 * F2x[1, 0]9C_x52 to set the gross and fine delay for all the byte lane fields
			 * If the target frequency is different than 400MHz, BIOS must
			 * execute two training passes for each DIMM.
			 * For pass 1 at a 400MHz MEMCLK frequency, use an initial total delay value
			 * of 01Fh. This represents a 1UI (UI=.5MEMCLK) delay and is determined
			 * by design.
			 */
			pDCTData->WLGrossDelay[lane_count*dimm+ByteLane] = Seed_Gross;
			pDCTData->WLFineDelay[lane_count*dimm+ByteLane] = Seed_Fine;
			printk(BIOS_SPEW, "\tLane %02x initial seed: %04x\n", ByteLane, ((Seed_Gross & 0x1f) << 5) | (Seed_Fine & 0x1f));
		}
	} else {
		if (nibble == 0) {
			/* Pass 2 */
			/* From BKDG, Write Leveling Seed Value. */
			if (is_fam15h()) {
				uint32_t RegisterDelay;
				int32_t SeedTotal[lane_count];
				int32_t SeedTotalPreScaling[lane_count];
				uint32_t WrDqDqsEarly;
				uint8_t AddrCmdPrelaunch = 0;		/* TODO: Fetch the correct value from RC2[0] */

				if (pDCTData->Status[DCT_STATUS_REGISTERED]) {
					if (AddrCmdPrelaunch)
						RegisterDelay = 0x30;
					else
						RegisterDelay = 0x20;
				} else {
					RegisterDelay = 0;
				}

				/* Retrieve WrDqDqsEarly */
				dword = Get_NB32_DCT(pDCTstat->dev_dct, dct, 0xa8);
				WrDqDqsEarly = (dword >> 24) & 0x3;

				/* FIXME
				 * Ignore WrDqDqsEarly for now to work around training issues
				 */
				WrDqDqsEarly = 0;

				/* Generate new seed values */
				for (ByteLane = 0; ByteLane < lane_count; ByteLane++) {
					/* Calculate adjusted seed values */
					SeedTotal[ByteLane] = (pDCTData->WLFineDelayPrevPass[lane_count*dimm+ByteLane] & 0x1f) |
						((pDCTData->WLGrossDelayPrevPass[lane_count*dimm+ByteLane] & 0x1f) << 5);
					SeedTotalPreScaling[ByteLane] = (SeedTotal[ByteLane] - RegisterDelay - (0x20 * WrDqDqsEarly));
					SeedTotal[ByteLane] = (int32_t) (RegisterDelay + ((((int64_t) SeedTotalPreScaling[ByteLane]) *
						fam15h_freq_tab[MemClkFreq] * 100) / (fam15h_freq_tab[pDCTData->WLPrevMemclkFreq[dimm]] * 100)));
				}

				/* Generate register values from seeds */
				for (ByteLane = 0; ByteLane < lane_count; ByteLane++) {
					printk(BIOS_SPEW, "\tLane %02x scaled delay: %04x\n", ByteLane, SeedTotal[ByteLane]);

					if (SeedTotal[ByteLane] >= 0) {
						Seed_Gross = SeedTotal[ByteLane] / 32;
						Seed_Fine = SeedTotal[ByteLane] % 32;
					} else {
						Seed_Gross = (SeedTotal[ByteLane] / 32) - 1;
						Seed_Fine = (SeedTotal[ByteLane] % 32) + 32;
					}

					if (Seed_Gross == 0)
						Seed_PreGross = 0;
					else if (Seed_Gross & 0x1)
						Seed_PreGross = 1;
					else
						Seed_PreGross = 2;

					/* The BKDG-recommended algorithm causes problems with registered DIMMs on some systems
					 * due to the long register delays causing premature total delay wrap-around.
					 * Attempt to work around this...
					 */
					Seed_PreGross = Seed_Gross;

					/* Save seed values for later use */
					pDCTData->WLSeedGrossDelay[lane_count*dimm+ByteLane] = Seed_Gross;
					pDCTData->WLSeedFineDelay[lane_count*dimm+ByteLane] = Seed_Fine;
					pDCTData->WLSeedPreGrossDelay[lane_count*dimm+ByteLane] = Seed_PreGross;

					pDCTData->WLGrossDelay[lane_count*dimm+ByteLane] = Seed_PreGross;
					pDCTData->WLFineDelay[lane_count*dimm+ByteLane] = Seed_Fine;

					printk(BIOS_SPEW, "\tLane %02x new seed: %04x\n", ByteLane, ((pDCTData->WLGrossDelay[lane_count*dimm+ByteLane] & 0x1f) << 5) | (pDCTData->WLFineDelay[lane_count*dimm+ByteLane] & 0x1f));
				}
			} else {
				uint32_t RegisterDelay;
				uint32_t SeedTotalPreScaling;
				uint32_t SeedTotal;
				uint8_t AddrCmdPrelaunch = 0;		/* TODO: Fetch the correct value from RC2[0] */
				for (ByteLane = 0; ByteLane < lane_count; ByteLane++)
				{
					if (pDCTData->Status[DCT_STATUS_REGISTERED]) {
						if (AddrCmdPrelaunch == 0)
							RegisterDelay = 0x20;
						else
							RegisterDelay = 0x30;
					} else {
						RegisterDelay = 0;
					}
					SeedTotalPreScaling = ((pDCTData->WLFineDelay[lane_count*dimm+ByteLane] & 0x1f) |
						(pDCTData->WLGrossDelay[lane_count*dimm+ByteLane] << 5)) - RegisterDelay;
					/* SeedTotalPreScaling = (the total delay value in F2x[1, 0]9C_x[4A:30] from pass 1 of write levelization
					training) - RegisterDelay. */
					SeedTotal = (uint16_t) ((((uint64_t) SeedTotalPreScaling) *
										fam10h_freq_tab[MemClkFreq] * 100) / (fam10h_freq_tab[3] * 100));
					Seed_Gross = SeedTotal / 32;
					Seed_Fine = SeedTotal & 0x1f;
					if (Seed_Gross == 0)
						Seed_Gross = 0;
					else if (Seed_Gross & 0x1)
						Seed_Gross = 1;
					else
						Seed_Gross = 2;

					/* The BKDG-recommended algorithm causes problems with registered DIMMs on some systems
					* due to the long register delays causing premature total delay wrap-around.
					* Attempt to work around this...
					*/
					SeedTotal = ((Seed_Gross & 0x1f) << 5) | (Seed_Fine & 0x1f);
					SeedTotal += RegisterDelay;
					Seed_Gross = SeedTotal / 32;
					Seed_Fine = SeedTotal & 0x1f;

					pDCTData->WLGrossDelay[lane_count*dimm+ByteLane] = Seed_Gross;
					pDCTData->WLFineDelay[lane_count*dimm+ByteLane] = Seed_Fine;

					printk(BIOS_SPEW, "\tLane %02x new seed: %04x\n", ByteLane, ((pDCTData->WLGrossDelay[lane_count*dimm+ByteLane] & 0x1f) << 5) | (pDCTData->WLFineDelay[lane_count*dimm+ByteLane] & 0x1f));
				}
			}

			/* Save initial seeds for upper nibble pass */
			for (ByteLane = 0; ByteLane < lane_count; ByteLane++) {
				pDCTData->WLSeedPreGrossPrevNibble[lane_count*dimm+ByteLane] = pDCTData->WLSeedPreGrossDelay[lane_count*dimm+ByteLane];
				pDCTData->WLSeedGrossPrevNibble[lane_count*dimm+ByteLane] = pDCTData->WLGrossDelay[lane_count*dimm+ByteLane];
				pDCTData->WLSeedFinePrevNibble[lane_count*dimm+ByteLane] = pDCTData->WLFineDelay[lane_count*dimm+ByteLane];
			}
		} else {
			/* Restore seed values from lower nibble pass */
			if (is_fam15h()) {
				for (ByteLane = 0; ByteLane < lane_count; ByteLane++) {
					pDCTData->WLSeedGrossDelay[lane_count*dimm+ByteLane] = pDCTData->WLSeedGrossPrevNibble[lane_count*dimm+ByteLane];
					pDCTData->WLSeedFineDelay[lane_count*dimm+ByteLane] = pDCTData->WLSeedFinePrevNibble[lane_count*dimm+ByteLane];
					pDCTData->WLSeedPreGrossDelay[lane_count*dimm+ByteLane] = pDCTData->WLSeedPreGrossPrevNibble[lane_count*dimm+ByteLane];

					pDCTData->WLGrossDelay[lane_count*dimm+ByteLane] = pDCTData->WLSeedPreGrossPrevNibble[lane_count*dimm+ByteLane];
					pDCTData->WLFineDelay[lane_count*dimm+ByteLane] = pDCTData->WLSeedFinePrevNibble[lane_count*dimm+ByteLane];

					printk(BIOS_SPEW, "\tLane %02x new seed: %04x\n", ByteLane, ((pDCTData->WLGrossDelay[lane_count*dimm+ByteLane] & 0x1f) << 5) | (pDCTData->WLFineDelay[lane_count*dimm+ByteLane] & 0x1f));
				}
			} else {
				for (ByteLane = 0; ByteLane < lane_count; ByteLane++) {
					pDCTData->WLGrossDelay[lane_count*dimm+ByteLane] = pDCTData->WLSeedGrossPrevNibble[lane_count*dimm+ByteLane];
					pDCTData->WLFineDelay[lane_count*dimm+ByteLane] = pDCTData->WLSeedFinePrevNibble[lane_count*dimm+ByteLane];

					printk(BIOS_SPEW, "\tLane %02x new seed: %04x\n", ByteLane, ((pDCTData->WLGrossDelay[lane_count*dimm+ByteLane] & 0x1f) << 5) | (pDCTData->WLFineDelay[lane_count*dimm+ByteLane] & 0x1f));
				}
			}
		}
	}

	pDCTData->WLPrevMemclkFreq[dimm] = MemClkFreq;
	setWLByteDelay(pDCTstat, dct, ByteLane, dimm, 0, pass, lane_count);
}

/*-----------------------------------------------------------------------------
 *  void setWLByteDelay(struct DCTStatStruc *pDCTstat, uint8_t dct, u8 ByteLane, u8 Dimm, uint8_t lane_count){
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
void setWLByteDelay(struct DCTStatStruc *pDCTstat, uint8_t dct, u8 ByteLane, u8 dimm, u8 targetAddr, uint8_t pass, uint8_t lane_count)
{
	sDCTStruct *pDCTData = pDCTstat->C_DCTPtr[dct];
	u8 fineStartLoc, fineEndLoc, grossStartLoc, grossEndLoc, tempB, index, offsetAddr;
	u32 addr, fineDelayValue, grossDelayValue, ValueLow, ValueHigh, EccValue, tempW;

	if (targetAddr == 0)
	{
		index = (u8)(lane_count * dimm);
		ValueLow = 0;
		ValueHigh = 0;
		ByteLane = 0;
		EccValue = 0;
		while (ByteLane < lane_count)
		{
			/* This subtract 0xC workaround might be temporary. */
			if ((pDCTData->WLPass == 2) && (pDCTData->RegMan1Present & (1 << (dimm*2+dct)))) {
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
			if (grossDelayValue >= 3)
				grossDelayValue = (grossDelayValue&1)? 1 : 2;
			fineDelayValue = pDCTData->WLFineDelay[index+ByteLane];
			if (ByteLane < 4)
				ValueLow |= ((grossDelayValue << 5) | fineDelayValue) << 8*ByteLane;
			else if (ByteLane < 8)
				ValueHigh |= ((grossDelayValue << 5) | fineDelayValue) << 8*(ByteLane-4);
			else
				EccValue = ((grossDelayValue << 5) | fineDelayValue);
			ByteLane++;
		}
		set_DCT_ADDR_Bits(pDCTData, dct, pDCTData->NodeId, FUN_DCT,
				DRAM_CONT_ADD_PHASE_REC_CTRL_LOW, 0, 31, (u32)ValueLow);
		set_DCT_ADDR_Bits(pDCTData, dct, pDCTData->NodeId, FUN_DCT,
				DRAM_CONT_ADD_PHASE_REC_CTRL_HIGH, 0, 31, (u32)ValueHigh);
		set_DCT_ADDR_Bits(pDCTData, dct, pDCTData->NodeId, FUN_DCT,
				DRAM_CONT_ADD_ECC_PHASE_REC_CTRL, 0, 31, (u32)EccValue);
	}
	else
	{
		/* Fam10h BKDG: Rev. 3.62 2.8.9.9.1 (6)
		 * Fam15h BKDG: Rev. 3.14 2.10.5.8.1
		 */
		index = (u8)(lane_count * dimm);
		grossDelayValue = pDCTData->WLGrossDelay[index+ByteLane];
		fineDelayValue = pDCTData->WLFineDelay[index+ByteLane];

		tempB = 0;
		offsetAddr = (u8)(3 * dimm);
		if (ByteLane < 2) {
			tempB = (u8)(16 * ByteLane);
			addr = DRAM_CONT_ADD_DQS_TIMING_CTRL_BL_01;
		} else if (ByteLane <4) {
			tempB = (u8)(16 * ByteLane);
			addr = DRAM_CONT_ADD_DQS_TIMING_CTRL_BL_01 + 1;
		} else if (ByteLane <6) {
			tempB = (u8)(16 * ByteLane);
			addr = DRAM_CONT_ADD_DQS_TIMING_CTRL_BL_45;
		} else if (ByteLane <8) {
			tempB = (u8)(16 * ByteLane);
			addr = DRAM_CONT_ADD_DQS_TIMING_CTRL_BL_45 + 1;
		} else {
			tempB = 0;
			addr = DRAM_CONT_ADD_DQS_TIMING_CTRL_BL_01 + 2;
		}
		addr += offsetAddr;

		fineStartLoc = (u8)(tempB % 32);
		fineEndLoc = (u8)(fineStartLoc + 4);
		grossStartLoc = (u8)(fineEndLoc + 1);
		grossEndLoc = (u8)(grossStartLoc + 2);

		set_DCT_ADDR_Bits(pDCTData, dct, pDCTData->NodeId, FUN_DCT,
				(u16)addr, fineStartLoc, fineEndLoc,(u32)fineDelayValue);
		set_DCT_ADDR_Bits(pDCTData, dct, pDCTData->NodeId, FUN_DCT,
				(u16)addr, grossStartLoc, grossEndLoc, (u32)grossDelayValue);

		pDCTData->WLFineDelayPrevPass[index+ByteLane] = fineDelayValue;
		pDCTData->WLGrossDelayPrevPass[index+ByteLane] = grossDelayValue;
		if (pass == FirstPass) {
			pDCTData->WLFineDelayFirstPass[index+ByteLane] = fineDelayValue;
			pDCTData->WLGrossDelayFirstPass[index+ByteLane] = grossDelayValue;
			pDCTData->WLCriticalGrossDelayFirstPass = pDCTData->WLCriticalGrossDelayPrevPass;
		}
	}

}

/*-----------------------------------------------------------------------------
 *  void getWLByteDelay(struct DCTStatStruc *pDCTstat, uint8_t dct, u8 ByteLane, u8 Dimm, u8 Nibble, uint8_t lane_count)
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
void getWLByteDelay(struct DCTStatStruc *pDCTstat, uint8_t dct, u8 ByteLane, u8 dimm, uint8_t pass, uint8_t nibble, uint8_t lane_count)
{
	sDCTStruct *pDCTData = pDCTstat->C_DCTPtr[dct];
	u8 fineStartLoc, fineEndLoc, grossStartLoc, grossEndLoc, tempB, tempB1, index;
	u32 addr, fine, gross;
	tempB = 0;
	index = (u8)(lane_count*dimm);
	if (ByteLane < 4) {
		tempB = (u8)(8 * ByteLane);
		addr = DRAM_CONT_ADD_PHASE_REC_CTRL_LOW;
	} else if (ByteLane < 8) {
		tempB1 = (u8)(ByteLane - 4);
		tempB = (u8)(8 * tempB1);
		addr = DRAM_CONT_ADD_PHASE_REC_CTRL_HIGH;
	} else {
		tempB = 0;
		addr = DRAM_CONT_ADD_ECC_PHASE_REC_CTRL;
	}
	fineStartLoc = tempB;
	fineEndLoc = (u8)(fineStartLoc + 4);
	grossStartLoc = (u8)(fineEndLoc + 1);
	grossEndLoc = (u8)(grossStartLoc + 1);

	fine = get_ADD_DCT_Bits(pDCTData, dct, pDCTData->NodeId,
				FUN_DCT, (u16)addr, fineStartLoc, fineEndLoc);
	gross = get_ADD_DCT_Bits(pDCTData, dct, pDCTData->NodeId,
				FUN_DCT, (u16)addr, grossStartLoc, grossEndLoc);

	printk(BIOS_SPEW, "\tLane %02x nibble %01x raw readback: %04x\n", ByteLane, nibble, ((gross & 0x1f) << 5) | (fine & 0x1f));

	/* Adjust seed gross delay overflow (greater than 3):
	 * - Adjust the trained gross delay to the original seed gross delay.
	 */
	if (pDCTData->WLGrossDelay[index+ByteLane] >= 3)
	{
		gross += pDCTData->WLGrossDelay[index+ByteLane];
		if (pDCTData->WLGrossDelay[index+ByteLane] & 1)
			gross -= 1;
		else
			gross -= 2;
	}
	else if ((pDCTData->WLGrossDelay[index+ByteLane] == 0) && (gross == 3))
	{
		/* If seed gross delay is 0 but PRE result gross delay is 3, it is negative.
		 * We will then round the negative number to 0.
		 */
		gross = 0;
		fine = 0;
	}
	printk(BIOS_SPEW, "\tLane %02x nibble %01x adjusted value (pre nibble): %04x\n", ByteLane, nibble, ((gross & 0x1f) << 5) | (fine & 0x1f));

	/* Nibble adjustments */
	if (nibble == 0) {
		pDCTData->WLFineDelay[index+ByteLane] = (uint8_t)fine;
		pDCTData->WLGrossDelay[index+ByteLane] = (uint8_t)gross;
	} else {
		uint32_t WLTotalDelay = ((pDCTData->WLGrossDelay[index+ByteLane] & 0x1f) << 5) | (pDCTData->WLFineDelay[index+ByteLane] & 0x1f);
		WLTotalDelay += ((gross & 0x1f) << 5) | (fine & 0x1f);
		WLTotalDelay /= 2;
		pDCTData->WLFineDelay[index+ByteLane] = (uint8_t)(WLTotalDelay & 0x1f);
		pDCTData->WLGrossDelay[index+ByteLane] = (uint8_t)((WLTotalDelay >> 5) & 0x1f);
	}
	printk(BIOS_SPEW, "\tLane %02x nibble %01x adjusted value (post nibble): %04x\n", ByteLane, nibble, ((pDCTData->WLGrossDelay[index+ByteLane] & 0x1f) << 5) | (pDCTData->WLFineDelay[index+ByteLane] & 0x1f));
}
