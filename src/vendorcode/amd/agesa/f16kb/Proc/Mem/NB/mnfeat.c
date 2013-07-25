/* $NoKeywords:$ */
/**
 * @file
 *
 * mnfeat.c
 *
 * Common Northbridge features
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB)
 * @e \$Revision: 84482 $ @e \$Date: 2012-12-16 22:48:10 -0600 (Sun, 16 Dec 2012) $
 *
 **/
/*****************************************************************************
*
 * Copyright (c) 2008 - 2013, Advanced Micro Devices, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Advanced Micro Devices, Inc. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* ***************************************************************************
*
*/

/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */

#include "AGESA.h"
#include "AdvancedApi.h"
#include "amdlib.h"
#include "Ids.h"
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "mu.h"
#include "PlatformMemoryConfiguration.h"
#include "merrhdl.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_NB_MNFEAT_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

VOID
STATIC
MemNGenHwRcvEnReadsUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 Address
  );

VOID
STATIC
MemNEnableInfiniteWritePatternUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
STATIC
MemNDisableInfiniteWritePatternUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNInitDqsTrainRcvrEnHwNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNDisableDqsTrainRcvrEnHwNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNInitCPGUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function initializes member functions of HW Rx En Training.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNInitDqsTrainRcvrEnHwNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  NBPtr->MemNPrepareRcvrEnDlySeed = MemNPrepareRcvrEnDlySeedNb;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function disables member functions of Hw Rx En Training.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNDisableDqsTrainRcvrEnHwNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  NBPtr->MemNPrepareRcvrEnDlySeed = (VOID (*) (MEM_NB_BLOCK *)) memDefRet;
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function calculates RcvEn seed value for each rank
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */
VOID
MemNPrepareRcvrEnDlySeedNb (
  IN OUT   MEM_NB_BLOCK  *NBPtr
  )
{
  MEM_TECH_BLOCK *TechPtr;
  CH_DEF_STRUCT *ChannelPtr;
  DIE_STRUCT *MCTPtr;
  UINT16 SeedTotal;
  UINT16 SeedFine;
  UINT16 SeedGross;
  UINT16 SeedPreGross;
  UINT16 SeedTotalPreScaling;
  UINT8  ByteLane;
  UINT16 Speed;
  UINT16 PlatEst;
  UINT8 ChipSel;
  UINT8 Pass;
  UINT16 *PlatEstSeed;
  UINT16 SeedValue[9];
  UINT16 SeedTtl[9];
  UINT16 SeedPre[9];

  TechPtr = NBPtr->TechPtr;
  MCTPtr = NBPtr->MCTPtr;
  ChannelPtr = TechPtr->NBPtr->ChannelPtr;
  Speed = NBPtr->DCTPtr->Timings.Speed;
  SeedTotalPreScaling = 0;
  ChipSel = TechPtr->ChipSel;
  Pass = TechPtr->Pass;

  for (ByteLane = 0; ByteLane < (MCTPtr->Status[SbEccDimms] ? 9 : 8); ByteLane++) {
    TechPtr->Bytelane = ByteLane;
    if (Pass == 1) {
      // Get platform override seed
      PlatEstSeed = (UINT16 *) FindPSOverrideEntry (NBPtr->RefPtr->PlatformMemoryConfiguration, PSO_RXEN_SEED, MCTPtr->SocketId, ChannelPtr->ChannelID, ChipSel >> 1,
                                                    &(NBPtr->MCTPtr->LogicalCpuid), &(NBPtr->MemPtr->StdHeader));
      // For Pass1, BIOS starts with the delay value obtained from the first pass of write
      // levelization training that was done in DDR3 Training and add a delay value of 3Bh.
      PlatEst = 0x3B;
      NBPtr->FamilySpecificHook[OverrideRcvEnSeed] (NBPtr, &PlatEst);
      PlatEst = ((PlatEstSeed != NULL) ? PlatEstSeed[ByteLane] : PlatEst);
      SeedTotal = ChannelPtr->WrDqsDlys[(ChipSel / NBPtr->CsPerDelay) * TechPtr->DlyTableWidth () + ByteLane] + PlatEst;
      SeedValue[ByteLane] = PlatEst;
    } else {
      // For Pass2
      // SeedTotalPreScaling = (the total delay values in D18F2x[1,0]9C_x0000_00[24:10] from pass 1 of
      //  DQS receiver enable training) - 20h. Subtract 1UI to get back to preamble left edge.
      if ((((ChipSel & 1) == 0) || (NBPtr->CsPerDelay == 1)) && NBPtr->FamilySpecificHook[TrainingNibbleZero] (NBPtr, &ChipSel)) {
        // Save Seed for odd CS SeedTotalPreScaling RxEn Value
        TechPtr->PrevPassRcvEnDly[ByteLane] = ChannelPtr->RcvEnDlys[(ChipSel / NBPtr->CsPerDelay) * TechPtr->DlyTableWidth () + ByteLane];
      }
      NBPtr->FamilySpecificHook[OverridePrevPassRcvEnDly] (NBPtr, &TechPtr->PrevPassRcvEnDly[ByteLane]);
      SeedTotalPreScaling = TechPtr->PrevPassRcvEnDly[ByteLane] - 0x20;
      // SeedTotal = SeedTotalPreScaling*target frequency/lowest supported frequency.
      SeedTotal = (UINT16) (((UINT32) SeedTotalPreScaling * Speed) / TechPtr->PrevSpeed);
      NBPtr->FamilySpecificHook[OverrideRcvEnSeedPassN] (NBPtr, &SeedTotal);
    }
    SeedTtl[ByteLane] = SeedTotal;

    // SeedGross = SeedTotal DIV 32.
    SeedGross = SeedTotal >> 5;
    // SeedFine = SeedTotal MOD 32.
    SeedFine = SeedTotal & 0x1F;
    // Next, determine the gross component of SeedTotal. SeedGrossPass1=SeedTotal DIV 32.
    // Then, determine the fine delay component of SeedTotal. SeedFinePass1=SeedTotal MOD 32.
    // Use SeedGrossPass1 to determine SeedPreGrossPass1:

    if ((SeedGross & 0x1) != 0) {
      //if SeedGross is odd
      SeedPreGross = 1;
    } else {
      //if SeedGross is even
      SeedPreGross = 2;
    }
    // (SeedGross - SeedPreGross)
    TechPtr->DiffSeedGrossSeedPreGross[ByteLane] = (SeedGross - SeedPreGross) << 5;

    //BIOS programs registers F2x[1, 0]9C_x[51:50] and F2x[1, 0]9C_x52 with SeedPreGrossPass1
    //and SeedFinePass1 from the preceding steps.

    NBPtr->SetTrainDly (NBPtr, AccessPhRecDly, DIMM_BYTE_ACCESS (ChipSel / NBPtr->CsPerDelay, ByteLane), (SeedPreGross << 5) | SeedFine);
    SeedPre[ByteLane] = (SeedPreGross << 5) | SeedFine;

    // 202688: Program seed value to RcvEnDly also.
    NBPtr->FamilySpecificHook[AdjustHwRcvEnSeedGross] (NBPtr, &SeedGross);
    // Keep seeded value of RcvEnDlys for MaxRdLatency calculation
    ChannelPtr->RcvEnDlys[(ChipSel / NBPtr->CsPerDelay) * TechPtr->DlyTableWidth () + ByteLane] = (SeedGross << 5) | SeedFine;
    NBPtr->SetTrainDly (NBPtr, AccessRcvEnDly, DIMM_BYTE_ACCESS (ChipSel / NBPtr->CsPerDelay, ByteLane), SeedGross << 5);
  }

  IDS_HDT_CONSOLE_DEBUG_CODE (
    if (Pass == 1) {
      IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tSeedValue: ");
      for (ByteLane = 0; ByteLane < (MCTPtr->Status[SbEccDimms] ? 9 : 8); ByteLane++) {
        IDS_HDT_CONSOLE (MEM_FLOW, "%03x ", SeedValue[ByteLane]);
      }
      IDS_HDT_CONSOLE (MEM_FLOW, "\n");
    }
    IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tSeedTotal: ");
    for (ByteLane = 0; ByteLane < (MCTPtr->Status[SbEccDimms] ? 9 : 8); ByteLane++) {
      IDS_HDT_CONSOLE (MEM_FLOW, "%03x ", SeedTtl[ByteLane]);
    }
    IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\t\t  SeedPRE: ");
    for (ByteLane = 0; ByteLane < (MCTPtr->Status[SbEccDimms] ? 9 : 8); ByteLane++) {
      IDS_HDT_CONSOLE (MEM_FLOW, "%03x ", SeedPre[ByteLane]);
    }
    IDS_HDT_CONSOLE (MEM_FLOW, "\n");
  );

  NBPtr->FamilySpecificHook[RegAccessFence] (NBPtr, NULL);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  Waits specified number of MEMCLKs
 *      @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *      @param[in] MemClkCount - Number of MEMCLKs
 *
 * ----------------------------------------------------------------------------
 */
VOID
MemNWaitXMemClksNb (
  IN OUT   MEM_NB_BLOCK  *NBPtr,
  IN       UINT32 MemClkCount
  )
{
  MemUWait10ns ((MemClkCount * 100 + NBPtr->DCTPtr->Timings.Speed - 1) / NBPtr->DCTPtr->Timings.Speed, NBPtr->MemPtr);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function uses the PRBS generator in the DCT to send a DDR Activate command
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]        ChipSelect - Chip select 0-7
 *     @param[in]        Bank - Bank Address 0-7
 *     @param[in]        RowAddress - Row Address [17:0]
 *
 */

VOID
MemNRrwActivateCmd (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 ChipSelect,
  IN       UINT8 Bank,
  IN       UINT32 RowAddress
  )
{
  // Set Chip select
  MemNSetBitFieldNb (NBPtr, BFCmdChipSelect, (1 << ChipSelect));
  // Set Bank Address
  MemNSetBitFieldNb (NBPtr, BFCmdBank, Bank);
  // Set Row Address
  MemNSetBitFieldNb (NBPtr, BFCmdAddress, RowAddress);
  // Send the command
  MemNSetBitFieldNb (NBPtr, BFSendActCmd, 1);
  // Wait for command complete
  MemNPollBitFieldNb (NBPtr, BFSendActCmd, 0, PCI_ACCESS_TIMEOUT, FALSE);
  // Wait 75 MEMCLKs
  NBPtr->WaitXMemClks (NBPtr, 75);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function uses the PRBS generator in the DCT to send a DDR Precharge
 *     or Precharge All command
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]        ChipSelect - Chip select 0-7
 *     @param[in]        Bank - Bank Address 0-7, PRECHARGE_ALL_BANKS = Precharge All
 *
 *
 */

VOID
MemNRrwPrechargeCmd (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 ChipSelect,
  IN       UINT8 Bank
  )
{
  // Wait 25 MEMCLKs
  NBPtr->WaitXMemClks (NBPtr, 25);
  // Set Chip select
  NBPtr->SetBitField (NBPtr, BFCmdChipSelect, (1 << ChipSelect));
  if (Bank == PRECHARGE_ALL_BANKS) {
    // Set Row Address, bit 10
    NBPtr->SetBitField (NBPtr, BFCmdAddress,  NBPtr->GetBitField (NBPtr, BFCmdAddress) | (1 << 10) );
  } else {
    // Clear Row Address, bit 10
    NBPtr->SetBitField (NBPtr, BFCmdAddress,  NBPtr->GetBitField (NBPtr, BFCmdAddress) & (~(1 << 10)) );
    // Set Bank Address
    NBPtr->SetBitField (NBPtr, BFCmdBank, Bank);
  }
  // Send the command
  NBPtr->SetBitField (NBPtr, BFSendPchgCmd, 1);
  // Wait for command complete
  NBPtr->PollBitField (NBPtr, BFSendPchgCmd, 0, PCI_ACCESS_TIMEOUT, FALSE);
  // Wait 25 MEMCLKs
  NBPtr->WaitXMemClks (NBPtr, 25);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function generates a continuous burst of reads for HW RcvEn
 *        training using the Unified Northbridge Reliable Read/Write Engine.
 *
 *     @param[in,out] NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]     Address - Unused by this function
 *
 */
VOID
STATIC
MemNGenHwRcvEnReadsUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 Address
  )
{
  VOID  *DummyPtr;
  DummyPtr = NULL;
  //
  // Issue Stream of Reads from the Target Rank
  //
  NBPtr->ReadPattern (NBPtr, DummyPtr, 0, NBPtr->TechPtr->PatternLength);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function generates a continuous stream of reads from DRAM using the
 *       Unified Northbridge Reliable Read/Write Engine.
 *
 *     @param[in,out] NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out] Buffer  - Unused by this function
 *     @param[in]     Address - Unused by this function
 *     @param[in]     ClCount - Number of cache lines to read
 *
 *     Assumptions:
 *
 *
 *
 */

VOID
MemNContReadPatternUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT32 Address,
  IN       UINT16 ClCount
  )
{
  MEM_TECH_BLOCK *TechPtr;
  RRW_SETTINGS *Rrw;
  UINT8 CmdTgt;
  UINT8 ChipSel;

  TechPtr = NBPtr->TechPtr;
  Rrw = &NBPtr->RrwSettings;

  ChipSel = TechPtr->ChipSel;
  CmdTgt = Rrw->CmdTgt;
  //
  // Wait for RRW Engine to be ready and turn it on
  //
  NBPtr->PollBitField (NBPtr, BFCmdSendInProg, 0, PCI_ACCESS_TIMEOUT, FALSE);
  NBPtr->SetBitField (NBPtr, BFCmdTestEnable, 1);
  //
  // Depending upon the Cmd Target, send Row Activate and set Chipselect
  //   for the Row or Rows that will be used
  //
  MemNRrwActivateCmd (NBPtr, ChipSel, Rrw->TgtBankAddressA, Rrw->TgtRowAddressA);
  NBPtr->SetBitField (NBPtr, BFTgtChipSelectA, ChipSel);
  if (CmdTgt == CMD_TGT_AB) {
    MemNRrwActivateCmd (NBPtr, ChipSel, Rrw->TgtBankAddressB, Rrw->TgtRowAddressB);
    NBPtr->SetBitField (NBPtr, BFTgtChipSelectB, ChipSel);
  }
  // Set Comparison Masks
  NBPtr->SetBitField (NBPtr, BFDramDqMaskLow, Rrw->CompareMaskLow);
  NBPtr->SetBitField (NBPtr, BFDramDqMaskHigh, Rrw->CompareMaskHigh);
  //
  // If All Dimms are ECC Capable Test ECC. Otherwise, mask it off
  //
  NBPtr->SetBitField (NBPtr, BFDramEccMask, (NBPtr->MCTPtr->Status[SbEccDimms] == TRUE) ? Rrw->CompareMaskEcc : 0xFF);
  //
  // Program the PRBS Seed
  //
  NBPtr->SetBitField (NBPtr, BFDataPrbsSeed, Rrw->DataPrbsSeed);
  //
  // Set the Command Count
  //
  NBPtr->SetBitField (NBPtr, BFCmdCount, ClCount);
  //
  // Program the Bubble Count and CmdStreamLen
  //
  NBPtr->SetBitField (NBPtr, BFBubbleCnt, 0);
  NBPtr->SetBitField (NBPtr, BFBubbleCnt2, 0);
  NBPtr->SetBitField (NBPtr, BFCmdStreamLen, 1);
  //
  // Program the Starting Address
  //
  NBPtr->SetBitField (NBPtr, BFTgtBankA, Rrw->TgtBankAddressA);
  NBPtr->SetBitField (NBPtr, BFTgtAddressA, Rrw->TgtColAddressA);
  if (CmdTgt == CMD_TGT_AB) {
    NBPtr->SetBitField (NBPtr, BFTgtBankB, Rrw->TgtBankAddressB);
    NBPtr->SetBitField (NBPtr, BFTgtAddressB, Rrw->TgtColAddressB);
  }
  //
  // Reset All Errors and Disable StopOnErr
  //
  NBPtr->SetBitField (NBPtr, BFResetAllErr, 1);
  NBPtr->SetBitField (NBPtr, BFStopOnErr, 0);
  //
  // Program the CmdTarget
  //
  NBPtr->SetBitField (NBPtr, BFCmdTgt, CmdTgt);
  //
  // Set CmdType to read
  //
  NBPtr->SetBitField (NBPtr, BFCmdType, CMD_TYPE_READ);
  //
  // Start the Commands
  //
  AGESA_TESTPOINT (TpProcMemContinPatternGenRead, &(NBPtr->MemPtr->StdHeader));
  NBPtr->SetBitField (NBPtr, BFSendCmd, 1);
  //
  // Commands have started, wait for the reads to complete then clear the command
  //
  NBPtr->PollBitField (NBPtr, BFTestStatus, 1, PCI_ACCESS_TIMEOUT, FALSE);
  NBPtr->PollBitField (NBPtr, BFCmdSendInProg, 0, PCI_ACCESS_TIMEOUT, FALSE);
  NBPtr->SetBitField (NBPtr, BFSendCmd, 0);
  //
  // Send the Precharge All Command
  //
  MemNRrwPrechargeCmd (NBPtr, ChipSel, PRECHARGE_ALL_BANKS);
  //
  // Turn Off the RRW Engine
  //
  NBPtr->SetBitField (NBPtr, BFCmdTestEnable, 0);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function generates a continuous stream of writes to DRAM using the
 *       Unified Northbridge Reliable Read/Write Engine.
 *
 *     @param[in,out] NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out] Address - Unused by this function
 *     @param[in]     Pattern - Unused by this function
 *     @param[in]     ClCount - Number of cache lines to write
 *
 */

VOID
MemNContWritePatternUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 Address,
  IN       UINT8 Pattern[],
  IN       UINT16 ClCount
  )
{
  MEM_TECH_BLOCK *TechPtr;
  RRW_SETTINGS *Rrw;
  UINT8 CmdTgt;
  UINT8 ChipSel;

  TechPtr = NBPtr->TechPtr;
  Rrw = &NBPtr->RrwSettings;

  ChipSel = TechPtr->ChipSel;
  CmdTgt = Rrw->CmdTgt;
  //
  // Wait for RRW Engine to be ready and turn it on
  //
  NBPtr->PollBitField (NBPtr, BFCmdSendInProg, 0, PCI_ACCESS_TIMEOUT, FALSE);
  NBPtr->SetBitField (NBPtr, BFCmdTestEnable, 1);
  //
  // Depending upon the Cmd Target, send Row Activate and set Chipselect
  //   for the Row or Rows that will be used
  //
  MemNRrwActivateCmd (NBPtr, ChipSel, Rrw->TgtBankAddressA, Rrw->TgtRowAddressA);
  NBPtr->SetBitField (NBPtr, BFTgtChipSelectA, ChipSel);
  if (CmdTgt == CMD_TGT_AB) {
    MemNRrwActivateCmd (NBPtr, ChipSel, Rrw->TgtBankAddressB, Rrw->TgtRowAddressB);
    NBPtr->SetBitField (NBPtr, BFTgtChipSelectB, ChipSel);
  }
  //
  // Program the PRBS Seed
  //
  NBPtr->SetBitField (NBPtr, BFDataPrbsSeed, Rrw->DataPrbsSeed);
  //
  // Set the Command Count
  //
  NBPtr->SetBitField (NBPtr, BFCmdCount, ClCount);
  //
  // Program the Bubble Count and CmdStreamLen
  //
  NBPtr->SetBitField (NBPtr, BFBubbleCnt, 0);
  NBPtr->SetBitField (NBPtr, BFBubbleCnt2, 0);
  NBPtr->SetBitField (NBPtr, BFCmdStreamLen, 1);
  //
  // Program the Starting Address
  //
  NBPtr->SetBitField (NBPtr, BFTgtBankA, Rrw->TgtBankAddressA);
  NBPtr->SetBitField (NBPtr, BFTgtAddressA, Rrw->TgtColAddressA);
  if (CmdTgt == CMD_TGT_AB) {
    NBPtr->SetBitField (NBPtr, BFTgtBankB, Rrw->TgtBankAddressB);
    NBPtr->SetBitField (NBPtr, BFTgtAddressB, Rrw->TgtColAddressB);
  }
  //
  // Program the CmdTarget
  //
  NBPtr->SetBitField (NBPtr, BFCmdTgt, CmdTgt);
  //
  // Set CmdType to Write
  //
  NBPtr->SetBitField (NBPtr, BFCmdType, CMD_TYPE_WRITE);
  //
  // Start the Commands
  //
  AGESA_TESTPOINT (TpProcMemContinPatternGenWrite, &(NBPtr->MemPtr->StdHeader));
  NBPtr->SetBitField (NBPtr, BFSendCmd, 1);
  //
  // Commands have started, wait for the writes to complete then clear the command
  //
  // Wait for TestStatus = 1 and CmdSendInProg = 0.
  NBPtr->PollBitField (NBPtr, BFTestStatus, 1, PCI_ACCESS_TIMEOUT, FALSE);
  NBPtr->PollBitField (NBPtr, BFCmdSendInProg, 0, PCI_ACCESS_TIMEOUT, FALSE);
  NBPtr->SetBitField (NBPtr, BFSendCmd, 0);
  //
  // Send the Precharge All Command
  //
  MemNRrwPrechargeCmd (NBPtr, ChipSel, PRECHARGE_ALL_BANKS);
  //
  // Turn Off the RRW Engine
  //
  NBPtr->SetBitField (NBPtr, BFCmdTestEnable, 0);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *       This function checks the Error status bits for comparison results
 *
 *     @param[in,out]   *NBPtr    - Pointer to the MEM_NB_BLOCK
 *     @param[in]       Buffer[]  -  Not used in this implementation
 *     @param[in]       Pattern[] - Not used in this implementation
 *     @param[in]       ByteCount - Not used in this implementation
 *
 *     @return  PASS - Bitmap of results of comparison
 */

UINT16
MemNCompareTestPatternUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT8 Pattern[],
  IN       UINT16 ByteCount
  )
{


  UINT16 i;
  UINT16 Pass;
  UINT8 ChipSel;
  UINT8 ColumnCount;
  UINT8* FailingBitMaskPtr;
  UINT8 FailingBitMask[9];
  UINT32 NibbleErrSts;

  ChipSel = NBPtr->TechPtr->ChipSel;
  ColumnCount = NBPtr->ChannelPtr->ColumnCount;
  // Calculate Failing Bitmask pointer
  FailingBitMaskPtr = &(NBPtr->ChannelPtr->FailingBitMask[(ColumnCount * NBPtr->TechPtr->ChipSel)]);

  //
  // Get Failing bit data
  //
  *((UINT32*)FailingBitMask) = NBPtr->GetBitField (NBPtr, BFDQErrLow);
  *((UINT32*)&FailingBitMask[4]) = NBPtr->GetBitField (NBPtr, BFDQErrHigh);
  FailingBitMask[8] = (UINT8)NBPtr->GetBitField (NBPtr, BFEccErr);

  Pass = 0x0000;
  //
  // Get Comparison Results - Convert Nibble Masks to Byte Masks
  //
  NibbleErrSts = NBPtr->GetBitField (NBPtr, BFNibbleErrSts);

  for (i = 0; i < ColumnCount ; i++) {
    Pass |= ((NibbleErrSts & 0x03) > 0 ) ? (1 << i) : 0;
    NibbleErrSts >>= 2;
    FailingBitMaskPtr[i] = FailingBitMask[i];
  }
  Pass = ~Pass;
  return Pass;
}

/*-----------------------------------------------------------------------------*/
/**
 *
 *       This function checks the Error status bits for offset comparison results
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       Buffer[]  -  Buffer data from DRAM (Measured data from DRAM) to compare
 *     @param[in]       Pattern[]  - Pattern (Expected data in ROM/CACHE) to compare against
 *     @param[in]       ByteCount - Byte count
 *
 *     @retval  Bitmap of results of comparison
 */
UINT16
STATIC
MemNInsDlyCompareTestPatternUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT8 Pattern[],
  IN       UINT16 ByteCount
  )
{
  UINT16 i;
  UINT16 Pass;
  UINT8 ColumnCount;
  UINT32 NibbleErr180Sts;

  ColumnCount = NBPtr->ChannelPtr->ColumnCount;
  Pass = 0x0000;
  //
  // Get Comparison Results - Convert Nibble Masks to Byte Masks
  //
  NibbleErr180Sts = NBPtr->GetBitField (NBPtr, BFNibbleErr180Sts);

  for (i = 0; i < ColumnCount ; i++) {
    Pass |= ((NibbleErr180Sts & 0x03) > 0 ) ? (1 << i) : 0;
    NibbleErr180Sts >>= 2;
  }
  Pass = ~Pass;

  return Pass;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function assigns read/write function pointers to CPG read/write modules.
 *
 *     @param[in,out] NBPtr  - Pointer to the MEM_NB_BLOCK
 *
 */
VOID
MemNInitCPGUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  NBPtr->WritePattern = MemNContWritePatternUnb;
  NBPtr->ReadPattern = MemNContReadPatternUnb;
  NBPtr->GenHwRcvEnReads = MemNGenHwRcvEnReadsUnb;
  NBPtr->FlushPattern = (VOID (*) (MEM_NB_BLOCK *, UINT32, UINT16)) memDefRet;
  NBPtr->TrainingPatternInit = (AGESA_STATUS (*) (MEM_NB_BLOCK *)) memDefRetSuccess;
  NBPtr->TrainingPatternFinalize = (AGESA_STATUS (*) (MEM_NB_BLOCK *)) memDefRetSuccess;
  NBPtr->CompareTestPattern = MemNCompareTestPatternUnb;
  NBPtr->InsDlyCompareTestPattern = MemNInsDlyCompareTestPatternUnb;
  NBPtr->FamilySpecificHook[SetupHwTrainingEngine] = MemNSetupHwTrainingEngineUnb;
  NBPtr->EnableInfiniteWritePattern = MemNEnableInfiniteWritePatternUnb;
  NBPtr->DisableInfiniteWritePattern = MemNDisableInfiniteWritePatternUnb;
  NBPtr->CPGInit = 0;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function generates a continuous stream of writes infinite writes to DRAM using the
 *       Unified Northbridge Reliable Read/Write Engine.
 *
 *     @param[in,out] NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
STATIC
MemNEnableInfiniteWritePatternUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 ByteLane;
  UINT32 WrDatValue;
  UINT32 WrDqsValue;
  MEM_TECH_BLOCK *TechPtr;
  RRW_SETTINGS *Rrw;
  UINT8 CmdTgt;
  UINT8 ChipSel;
  TechPtr = NBPtr->TechPtr;
  Rrw = &NBPtr->RrwSettings;
  ChipSel = TechPtr->ChipSel;
  CmdTgt = Rrw->CmdTgt;

  // Ensure that DisAutoRefresh and ZqCals are disabled during the use of RRWM
  if (MemNGetBitFieldNb (NBPtr, BFDisAutoRefresh) == 0) {
    NBPtr->OrigDisAutoRefreshState = FALSE;
    MemNSetBitFieldNb (NBPtr, BFDisAutoRefresh, 1);
    MemNSetBitFieldNb (NBPtr, BFZqcsInterval, 0);
  } else {
    NBPtr->OrigDisAutoRefreshState = TRUE;
  }

  //
  // Enable I/O Skew mode
  //
  for (ByteLane = 0; ByteLane < (NBPtr->MCTPtr->Status[SbEccDimms] ? 9 : 8) ; ByteLane++) {
    //Move WrDQS settings from current DIMM to DIMM 0
    WrDqsValue = NBPtr->ChannelPtr->WrDqsDlys[(NBPtr->TechPtr->ChipSel / NBPtr->CsPerDelay) * NBPtr->TechPtr->DlyTableWidth () + ByteLane];
    NBPtr->SetTrainDly (NBPtr, AccessWrDqsDly, DIMM_BYTE_ACCESS (0, ByteLane), (UINT16)WrDqsValue);
    //Move WrDat settings from current DIMM to DIMM 0
    WrDatValue = NBPtr->ChannelPtr->WrDatDlys[(NBPtr->TechPtr->ChipSel / NBPtr->CsPerDelay) * NBPtr->TechPtr->DlyTableWidth () + ByteLane];
    NBPtr->SetTrainDly (NBPtr, AccessWrDatDly, DIMM_BYTE_ACCESS (0, ByteLane), (UINT16)WrDatValue);
  }
  NBPtr->SetBitField (NBPtr, BFIoSkewMode, 0x0001);


  //
  // Enable PRBS
  //

  //
  // Wait for RRW Engine to be ready and turn it on
  //
  NBPtr->PollBitField (NBPtr, BFCmdSendInProg, 0, PCI_ACCESS_TIMEOUT, FALSE);
  NBPtr->SetBitField (NBPtr, BFCmdTestEnable, 1);
  //
  // Depending upon the Cmd Target, send Row Activate and set Chipselect
  //   for the Row or Rows that will be used
  //
  MemNRrwActivateCmd (NBPtr, ChipSel, Rrw->TgtBankAddressA, Rrw->TgtRowAddressA);
  NBPtr->SetBitField (NBPtr, BFTgtChipSelectA, ChipSel);
  //
  // Program the PRBS Seed
  //
  NBPtr->SetBitField (NBPtr, BFDataPrbsSeed, Rrw->DataPrbsSeed);
  //
  // Set the Command Count
  //
  NBPtr->SetBitField (NBPtr, BFCmdCount, 0);
  //
  // Program the Bubble Count and CmdStreamLen
  //
  NBPtr->SetBitField (NBPtr, BFBubbleCnt, 0);
  NBPtr->SetBitField (NBPtr, BFBubbleCnt2, 0);
  NBPtr->SetBitField (NBPtr, BFCmdStreamLen, 1);
  //
  // Program the Starting Address
  //
  NBPtr->SetBitField (NBPtr, BFTgtBankA, Rrw->TgtBankAddressA);
  NBPtr->SetBitField (NBPtr, BFTgtAddressA, Rrw->TgtColAddressA);
  //
  // Program the CmdTarget
  //
  NBPtr->SetBitField (NBPtr, BFCmdTgt, CMD_TGT_A);
  //
  // Set CmdType to write
  //
  NBPtr->SetBitField (NBPtr, BFCmdType, CMD_TYPE_WRITE);
  //
  // Start the Commands
  //
  NBPtr->SetBitField (NBPtr, BFSendCmd, 1);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function disables the infinite stream of writes to DRAM using the
 *       Unified Northbridge Reliable Read/Write Engine.
 *
 *     @param[in,out] NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
STATIC
MemNDisableInfiniteWritePatternUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 ByteLane;
  UINT32 WrDatValue;
  UINT32 WrDqsValue;
  //
  // Disable PRBS
  NBPtr->SetBitField (NBPtr, BFCmdCount, 1);
  //Wait for TestStatus = 1 and CmdSendInProg = 0
  NBPtr->PollBitField (NBPtr, BFCmdSendInProg, 0, PCI_ACCESS_TIMEOUT, FALSE);
  NBPtr->SetBitField (NBPtr, BFSendCmd, 0);
  //
  // Disable I/O Skew Mode
  for (ByteLane = 0; ByteLane < (NBPtr->MCTPtr->Status[SbEccDimms] ? 9 : 8) ; ByteLane++) {
    //Move WrDqs settings from DIMM 0 from saved area back to register
    WrDqsValue = NBPtr->ChannelPtr->WrDqsDlys[0 * NBPtr->TechPtr->DlyTableWidth () + ByteLane];
    NBPtr->SetTrainDly (NBPtr, AccessWrDqsDly, DIMM_BYTE_ACCESS (0, ByteLane), (UINT16)WrDqsValue);
    //Move WrDat settings from DIMM 0 from saved area back to register
    WrDatValue = NBPtr->ChannelPtr->WrDatDlys[0 * NBPtr->TechPtr->DlyTableWidth () + ByteLane];
    NBPtr->SetTrainDly (NBPtr, AccessWrDatDly, DIMM_BYTE_ACCESS (0, ByteLane), (UINT16)WrDatValue);
  }
  NBPtr->SetBitField (NBPtr, BFIoSkewMode, 0x0000);
  //
  // Turn Off the RRW Engine
  //
  MemNRrwPrechargeCmd (NBPtr, NBPtr->TechPtr->ChipSel, PRECHARGE_ALL_BANKS);
  NBPtr->SetBitField (NBPtr, BFCmdTestEnable, 0);
  //
  // Restore DisAutoRefresh and ZQCals to original state
  //
  if (!NBPtr->OrigDisAutoRefreshState) {
    MemNSetBitFieldNb (NBPtr, BFDisAutoRefresh, 0);
    MemNSetBitFieldNb (NBPtr, BFZqcsInterval, 2);
  }

}

/*-----------------------------------------------------------------------------*/
/**
 *
 *       This function checks the 180 Error status bits for RD DQS 2D training
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       Buffer[]  -  Buffer data from DRAM (Measured data from DRAM) to compare
 *     @param[in]       Pattern[]  - Pattern (Expected data in ROM/CACHE) to compare against
 *     @param[in]       ByteCount - Byte count,
 *
 *     @retval  Bitmap of results of comparison
 */
UINT32
MemN180CompareRdDqs2DPatternUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT8 Pattern[],
  IN       UINT16 ByteCount
  )
{
  return NBPtr->GetBitField (NBPtr, BFNibbleErr180Sts);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *       This function checks the In Phase Error status bits for comparison results for RDDQS 2D training
 *
 *     @param[in,out]   *NBPtr    - Pointer to the MEM_NB_BLOCK
 *     @param[in]       Buffer[]  -  Not used in this implementation
 *     @param[in]       Pattern[] - Not used in this implementation
 *     @param[in]       ByteCount - Not used in this implementation
 *
 *     @return  PASS - Bitmap of results of comparison
 */

UINT32
MemNInPhaseCompareRdDqs2DPatternUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT8 Pattern[],
  IN       UINT16 ByteCount
  )
{
  return NBPtr->GetBitField (NBPtr, BFNibbleErrSts);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *       This function starts the Victim for 2D RdDqs Training Continuous Writes
 *
 *     @param[in,out]   *NBPtr    - Pointer to the MEM_NB_BLOCK
 *     @param[in]       SeedCount  -  Seed count
 */
VOID
MemNStartRdDqs2dVictimContinuousWritesUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 SeedCount
  )
{
  // Program the PRBS Seed
  NBPtr->SetBitField (NBPtr, BFDataPrbsSeed, NBPtr->GetPrbs2dRdDqsSeed (NBPtr, SeedCount));

  if (NBPtr->IsSupported[OptimizedPatternWrite2D]) {
    //
    // Set BankAddress according to seed
    //
    NBPtr->SetBitField (NBPtr, BFTgtBankA, (SeedCount * 2) + CPG_BANK_ADDRESS_A);
    NBPtr->SetBitField (NBPtr, BFTgtBankB, (SeedCount * 2) + CPG_BANK_ADDRESS_B);
  } else {
    //
    // Enable continuous writes on the victim channels
    //
    // Set the Command Count
    NBPtr->SetBitField (NBPtr, BFCmdCount, 256);
    NBPtr->SetBitField (NBPtr, BFCmdType, CMD_TYPE_WRITE);
    NBPtr->SetBitField (NBPtr, BFSendCmd, 1);
    // Wait for TestStatus = 1 and CmdSendInProg = 0.
    NBPtr->PollBitField (NBPtr, BFTestStatus, 1, PCI_ACCESS_TIMEOUT, FALSE);
    NBPtr->SetBitField (NBPtr, BFSendCmd, 0);
  }
  //
  // Enable continuous reads on the victim channels
  //
  // Set the Command Count
  ASSERT (NBPtr->MaxAggressorDimms[NBPtr->Dct] != 0);
  NBPtr->SetBitField (NBPtr, BFCmdCount, (256 * NBPtr->RdRolloverMultiple / NBPtr->MaxAggressorDimms[NBPtr->Dct]));
  // Reset All Errors and Disable StopOnErr
  NBPtr->SetBitField (NBPtr, BFCmdType, CMD_TYPE_READ);
  NBPtr->SetBitField (NBPtr, BFSendCmd, 1);
  // Wait for TestStatus = 1 and CmdSendInProg = 0
  NBPtr->PollBitField (NBPtr, BFTestStatus, 1, PCI_ACCESS_TIMEOUT, FALSE);
  //}
  NBPtr->SetBitField (NBPtr, BFSendCmd, 0);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *       This function Initializes the Victim chipSelects for 2D RdDqs Training Continuous Writes
 *
 *     @param[in,out]   *NBPtr    - Pointer to the MEM_NB_BLOCK
 */
VOID
MemNInitializeRdDqs2dVictimChipSelContinuousWritesUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  NBPtr->SetBitField (NBPtr, BFTgtChipSelectA, NBPtr->TechPtr->ChipSel);
  NBPtr->SetBitField (NBPtr, BFTgtChipSelectB, NBPtr->TechPtr->ChipSel);
  NBPtr->SetBitField (NBPtr, BFResetAllErr, 1);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *       This function finalizes the Victim for 2D RdDqs Training
 *
 *     @param[in,out]   *NBPtr    - Pointer to the MEM_NB_BLOCK
 */
VOID
MemNFinalizeRdDqs2dVictimContinuousWritesUnb  (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 InitialCS;
  UINT8 ChipSel;
  InitialCS = NBPtr->TechPtr->ChipSel;
  NBPtr->SetBitField (NBPtr, BFLfsrRollOver, 0);
  for (ChipSel = InitialCS; ChipSel < (InitialCS + NBPtr->CsPerDelay); ChipSel++) {
    // Ensure that Odd and Even CS are trained
    if ((NBPtr->DCTPtr->Timings.CsEnabled & ((UINT16) 1 << ChipSel)) == 0) {
      continue;
    }
    NBPtr->TechPtr->ChipSel = ChipSel;
    // Send the Precharge All Command
    MemNRrwPrechargeCmd (NBPtr, ChipSel, PRECHARGE_ALL_BANKS);
  }
  // Turn Off the RRW Engine
  NBPtr->SetBitField (NBPtr, BFCmdTestEnable, 0);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *       This function Initializes the Victim for 2D RdDqs Training
 *
 *     @param[in,out]   *NBPtr    - Pointer to the MEM_NB_BLOCK
 */
VOID
MemNInitializeRdDqs2dVictimContinuousWritesUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  RRW_SETTINGS *Rrw;
  UINT8 InitialCS;
  UINT8 ChipSel;
  UINT8 SeedCount;
  InitialCS = NBPtr->TechPtr->ChipSel;
  Rrw = &NBPtr->RrwSettings;
  // Program the Bubble Count and CmdStreamLen
  NBPtr->SetBitField (NBPtr, BFBubbleCnt, 32);
  NBPtr->SetBitField (NBPtr, BFBubbleCnt2, 0);
  NBPtr->SetBitField (NBPtr, BFCmdStreamLen, 63);
  // Set Comparison Masks
  NBPtr->SetBitField (NBPtr, BFDramDqMaskLow, Rrw->CompareMaskLow);
  NBPtr->SetBitField (NBPtr, BFDramDqMaskHigh, Rrw->CompareMaskHigh);
  // If All Dimms are ECC Capable Test ECC. Otherwise, mask it off
  NBPtr->SetBitField (NBPtr, BFDramEccMask, (NBPtr->MCTPtr->Status[SbEccDimms] == TRUE) ? Rrw->CompareMaskEcc : 0xFF);
  // Program the Starting Address
  NBPtr->SetBitField (NBPtr, BFTgtBankA, Rrw->TgtBankAddressA);
  NBPtr->SetBitField (NBPtr, BFTgtAddressA, Rrw->TgtColAddressA);
  NBPtr->SetBitField (NBPtr, BFTgtBankB, Rrw->TgtBankAddressB);
  NBPtr->SetBitField (NBPtr, BFTgtAddressB, Rrw->TgtColAddressB);
  NBPtr->SetBitField (NBPtr, BFCmdTgt, CMD_TGT_AB);
  // Wait for RRW Engine to be ready and turn it on
  NBPtr->PollBitField (NBPtr, BFCmdSendInProg, 0, PCI_ACCESS_TIMEOUT, FALSE);
  NBPtr->SetBitField (NBPtr, BFCmdTestEnable, 1);
  for (ChipSel = InitialCS; ChipSel < (InitialCS + NBPtr->CsPerDelay); ChipSel++) {
    // Ensure that Odd and Even CS are trained
    if ((NBPtr->DCTPtr->Timings.CsEnabled & ((UINT16) 1 << ChipSel)) == 0) {
      continue;
    }
    NBPtr->TechPtr->ChipSel = ChipSel;
    for (SeedCount = 0; SeedCount < (NBPtr->IsSupported[OptimizedPatternWrite2D] ? NBPtr->MaxSeedCount : 1 ); SeedCount++) {
      if (NBPtr->IsSupported[OptimizedPatternWrite2D]) {
        //
        // Set BankAddress according to seed
        //
        ASSERT (NBPtr->MaxSeedCount <= 4);
        Rrw->TgtBankAddressA = (SeedCount * 2) + CPG_BANK_ADDRESS_A;
        Rrw->TgtBankAddressB = (SeedCount * 2) + CPG_BANK_ADDRESS_B;
      }
      //
      // Send ACTIVATE to all Banks
      //
      // Set Chip select
      MemNSetBitFieldNb (NBPtr, BFCmdChipSelect, (1 << NBPtr->TechPtr->ChipSel));
      // Set Bank Address
      MemNSetBitFieldNb (NBPtr, BFCmdBank, Rrw->TgtBankAddressA);
      // Set Row Address
      MemNSetBitFieldNb (NBPtr, BFCmdAddress, Rrw->TgtRowAddressA);
      // Send the command
      MemNSetBitFieldNb (NBPtr, BFSendActCmd, 1);
      // Wait for command complete
      MemNPollBitFieldNb (NBPtr, BFSendActCmd, 0, PCI_ACCESS_TIMEOUT, FALSE);
      // Set Chip select
      MemNSetBitFieldNb (NBPtr, BFCmdChipSelect, (1 << NBPtr->TechPtr->ChipSel));
      // Set Bank Address
      MemNSetBitFieldNb (NBPtr, BFCmdBank, Rrw->TgtBankAddressB);
      // Set Row Address
      MemNSetBitFieldNb (NBPtr, BFCmdAddress, Rrw->TgtRowAddressB);
      // Send the command
      MemNSetBitFieldNb (NBPtr, BFSendActCmd, 1);
      // Wait for command complete
      MemNPollBitFieldNb (NBPtr, BFSendActCmd, 0, PCI_ACCESS_TIMEOUT, FALSE);

      if (NBPtr->IsSupported[OptimizedPatternWrite2D]) {
        //
        // Write the Pattern to the bank pairs for each seed.
        //
        MemNInitializeRdDqs2dVictimChipSelContinuousWritesUnb (NBPtr);
        NBPtr->SetBitField (NBPtr, BFTgtBankA, Rrw->TgtBankAddressA);
        NBPtr->SetBitField (NBPtr, BFTgtBankB, Rrw->TgtBankAddressB);
        // Program the PRBS Seed
        NBPtr->SetBitField (NBPtr, BFDataPrbsSeed, NBPtr->GetPrbs2dRdDqsSeed (NBPtr, SeedCount));
        //
        // Enable continuous writes on the victim channels
        //
        // Set the Command Count
        NBPtr->SetBitField (NBPtr, BFCmdCount, 256);
        NBPtr->SetBitField (NBPtr, BFCmdType, CMD_TYPE_WRITE);
        NBPtr->SetBitField (NBPtr, BFSendCmd, 1);
        // Wait for TestStatus = 1 and CmdSendInProg = 0.
        NBPtr->PollBitField (NBPtr, BFTestStatus, 1, PCI_ACCESS_TIMEOUT, FALSE);
        NBPtr->SetBitField (NBPtr, BFSendCmd, 0);
      }
    }
  }
  NBPtr->SetBitField (NBPtr, BFLfsrRollOver, 1);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *       This function enables continuous writes on unused channels
 *     @param[in,out]   *NBPtr    - Pointer to the MEM_NB_BLOCK
 *     @param[in]       *SeedCount -  seed index
 *
 *     @return    Prbs Seed
 *
 */

UINT32
MemNGetPrbs2dRdDqsSeedUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 SeedCount
  )
{
  UINT32 PrbsSeed;
  CONST STATIC UINT32 PrbsSeedTbl[4] = {0x7ea05, 0x44443, 0x22b97, 0x3f167};
  CONST STATIC UINT32 CmdStreamLenTbl[4] = {13, 61, 127, 251};
  ASSERT (SeedCount <= (NBPtr->MaxSeedCount - 1));
  NBPtr->SetBitField (NBPtr, BFCmdStreamLen, CmdStreamLenTbl[SeedCount]);
  PrbsSeed = PrbsSeedTbl[SeedCount];
  ASSERT (PrbsSeed != 0);
  return PrbsSeed;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *       This function enables/disables continuous writes on unused agressor channels
 *     @param[in,out]   *NBPtr    - Pointer to the MEM_NB_BLOCK
 *     @param[in]       SeedCount  -  Seed count
 *     @param[in]       TurnOnInfinite - TRUE - Enable Infinite Mode
 *                      TurnOnInfinite - FALSE - Disable Infinite Mode
 *
 */
VOID
MemNAgressorContinuousWritesUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 SeedCount,
  IN       BOOLEAN TurnOnInfinite
  )
{
  UINT8  CurrChipSel;
  UINT8  CurrDct;
  UINT8  Dct;
  UINT32 Address;
  UINT8  Die;
  MEM_NB_BLOCK *TargetNBPtr;
  BOOLEAN SkipContinuous;
  CurrDct = NBPtr->Dct;
  CurrChipSel = NBPtr->TechPtr->ChipSel;
  SkipContinuous = FALSE;
  for (Die = 0; Die < NBPtr->NodeCount; Die++) {
    if (NBPtr->DieEnabled[Die]) {
      for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
        // Make sure that we are not targeting current DCT, current Node
        if (Die == NBPtr->Node) {
          if (CurrDct == Dct) {
            SkipContinuous = TRUE;
          } else {
            SkipContinuous = FALSE;
          }
        } else {
          SkipContinuous = FALSE;
        }
        // Enable/Disable continuous on aggressors
        if (SkipContinuous == FALSE) {
          if (Die == NBPtr->Node) {
            TargetNBPtr = NBPtr;
          } else {
            ASSERT (NBPtr->AdjacentDieNBPtr != NULL);
            TargetNBPtr = NBPtr->AdjacentDieNBPtr;
          }
          // All context switched at this point
          TargetNBPtr->SwitchDCT (TargetNBPtr, Dct);
          if (TargetNBPtr->DCTPtr->Timings.DctMemSize != 0) {
            // Set Targets for agressors
            TargetNBPtr->TechPtr->ChipSel = TargetNBPtr->CurrentAggressorCSTarget[Dct];
            TargetNBPtr->GetSysAddr (TargetNBPtr, TargetNBPtr->TechPtr->ChipSel, &Address);
            TargetNBPtr->RrwSettings.DataPrbsSeed = TargetNBPtr->GetPrbs2dRdDqsSeed (TargetNBPtr, 0);
            if (TurnOnInfinite) {
              // Enable continuous writes on aggressor channels
              TargetNBPtr->EnableInfiniteWritePattern (TargetNBPtr);
            } else {
              // Disable continous writes on aggressor channels
              TargetNBPtr->DisableInfiniteWritePattern (TargetNBPtr);
              // Set the next target CS for aggressor channel
              if (TargetNBPtr->CurrentAggressorCSTarget[Dct] == TargetNBPtr->MaxAggressorCSEnabled[Dct]) {
                TargetNBPtr->CurrentAggressorCSTarget[Dct] = TargetNBPtr->InitialAggressorCSTarget[Dct];
              } else {
                TargetNBPtr->CurrentAggressorCSTarget[Dct] = TargetNBPtr->CurrentAggressorCSTarget[Dct] + TargetNBPtr->CsPerDelay;
              }
            }
          }
        }
      }
    }
  }
  // Restore Node, DCT and ChipSel
  NBPtr->TechPtr->ChipSel = CurrChipSel;
  NBPtr->SwitchDCT (NBPtr, CurrDct);
}

