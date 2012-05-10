/**
 * @file
 *
 * mttwl3.c
 *
 * Technology Phy assisted write levelization for DDR3
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Tech/DDR3)
 * @e \$Revision: 44323 $ @e \$Date: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
 *
 **/
/*****************************************************************************
*
* Copyright (c) 2011, Advanced Micro Devices, Inc.
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
* 
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
#include "Ids.h"
#include "mm.h"
#include "mn.h"
#include "mu.h"
#include "mt.h"
#include "mtsdi3.h"
#include "merrhdl.h"
#include "OptionMemory.h"
#include "PlatformMemoryConfiguration.h"
#include "GeneralServices.h"
#include "Filecode.h"
#define FILECODE PROC_MEM_TECH_DDR3_MTTWL3_FILECODE
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

BOOLEAN
STATIC
MemTWriteLevelizationHw3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Pass
  );

VOID
STATIC
MemTWLPerDimmHw3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Dimm,
  IN       UINT8 Pass
  );

VOID
STATIC
MemTPrepareDIMMs3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 TargetDIMM,
  IN       BOOLEAN Wl
  );

VOID
STATIC
MemTProcConfig3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Dimm,
  IN       UINT8 Pass
  );

VOID
STATIC
MemTBeginWLTrain3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Dimm
  );

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */


/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function executes first pass of Phy assisted write levelization
 *      for a specific node (DDR800).
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */

BOOLEAN
MemTWriteLevelizationHw3Pass1 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  return MemTWriteLevelizationHw3 (TechPtr, 1);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function executes second pass of Phy assisted write levelization
 *      for a specific node (DDR1066 and above).
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */

BOOLEAN
MemTWriteLevelizationHw3Pass2 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  // If current speed is higher than start-up speed, do second pass of WL
  if (TechPtr->NBPtr->DCTPtr->Timings.Speed > TechPtr->NBPtr->StartupSpeed) {
    return MemTWriteLevelizationHw3 (TechPtr, 2);
  }
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function prepares for Phy assisted training.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */

BOOLEAN
MemTPreparePhyAssistedTraining (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  //  Disable auto refresh by configuring F2x[1, 0]8C[DisAutoRefresh] = 1.
  TechPtr->NBPtr->BrdcstSet (TechPtr->NBPtr, BFDisAutoRefresh, 1);
  //  Disable ZQ calibration short command by configuring F2x[1, 0]94[ZqcsInterval] = 00b.
  TechPtr->NBPtr->BrdcstSet (TechPtr->NBPtr, BFZqcsInterval, 0);
  return (BOOLEAN) (TechPtr->NBPtr->MCTPtr->ErrCode < AGESA_FATAL);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function revert to normal settings when exiting from Phy assisted training.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */

BOOLEAN
MemTExitPhyAssistedTraining (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  //  13.Program F2x[1, 0]8C[DisAutoRefresh] = 0.
  TechPtr->NBPtr->BrdcstSet (TechPtr->NBPtr, BFDisAutoRefresh, 0);
  //  14.Program F2x[1, 0]94[ZqcsInterval] to the proper interval for the current memory configuration.
  TechPtr->NBPtr->BrdcstSet (TechPtr->NBPtr, BFZqcsInterval, 2);
  return (BOOLEAN) (TechPtr->NBPtr->MCTPtr->ErrCode < AGESA_FATAL);
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function executed hardware based write levelization for a specific die
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in] Pass - Pass number (1 (400Mhz) or 2 (>400Mhz))
 *
 *     @pre   Auto refresh and ZQCL must be disabled
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */

BOOLEAN
STATIC
MemTWriteLevelizationHw3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Pass
  )
{
  MEM_NB_BLOCK  *NBPtr;
  DCT_STRUCT *DCTPtr;
  UINT8 Dct;
  UINT8 Dimm;

  NBPtr = TechPtr->NBPtr;

  IDS_HDT_CONSOLE ("!\nStart write leveling\n");
  AGESA_TESTPOINT (TpProcMemWriteLevelizationTraining, &(NBPtr->MemPtr->StdHeader));
  // Begin DQS Write timing training
  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    NBPtr->SwitchDCT (NBPtr, Dct);
    IDS_HDT_CONSOLE ("!\tDct %d\n", Dct);
    DCTPtr = NBPtr->DCTPtr;

    //training for each Dimm
    for (Dimm = 0; Dimm < MAX_DIMMS_PER_CHANNEL; Dimm++) {
      if ((DCTPtr->Timings.CsEnabled & ((UINT16)3 << (Dimm << 1))) != 0) {

        IDS_HDT_CONSOLE ("!\t\tCS %d\n", Dimm << 1);
        MemTWLPerDimmHw3 (TechPtr, Dimm, Pass);
      }
    }
  }
  IDS_HDT_CONSOLE ("End write leveling\n\n");
  return (BOOLEAN) (NBPtr->MCTPtr->ErrCode < AGESA_FATAL);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function initializes per DIMM write levelization
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in] Dimm - DIMM to be trained
 *     @param[in] Pass - Pass number (1 (400Mhz) or 2 (>400Mhz))
 *
 */

VOID
STATIC
MemTWLPerDimmHw3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Dimm,
  IN       UINT8 Pass
  )
{
  MEM_DATA_STRUCT *MemPtr;
  MEM_NB_BLOCK  *NBPtr;

  NBPtr = TechPtr->NBPtr;
  MemPtr = NBPtr->MemPtr;

  ASSERT (Dimm < MAX_DIMMS_PER_CHANNEL);

  // 1. Specify the target Dimm that is to be trained by programming
  //     F2x[1, 0]9C_x08[TrDimmSel].
  NBPtr->SetBitField (NBPtr, BFTrDimmSel, Dimm);

  // 2. Prepare the DIMMs for write levelization using DDR3-defined
  // MR commands.
  MemTPrepareDIMMs3 (TechPtr, Dimm, TRUE);

  // 3. After the DIMMs are configured, BIOS waits 40 MEMCLKs to
  //     satisfy DDR3-defined internal DRAM timing.
  MemUWait10ns (10, MemPtr);

  // 4. Configure the processor's DDR phy for write levelization training:
  MemTProcConfig3 (TechPtr, Dimm, Pass);

  // 5. Begin write levelization training
  MemTBeginWLTrain3 (TechPtr, Dimm);

  // 7. Program the target Dimm back to normal operation
  MemTPrepareDIMMs3 (TechPtr, Dimm, FALSE);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function prepares the DIMMS for Write Levelization
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in] TargetDIMM - DIMM to be trained
 *     @param[in] Wl - Indicates if WL mode should be enabled
 *
 */

VOID
STATIC
MemTPrepareDIMMs3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 TargetDIMM,
  IN       BOOLEAN Wl
  )
{
  MEM_NB_BLOCK  *NBPtr;
  UINT8 ChipSel;

  NBPtr = TechPtr->NBPtr;

  AGESA_TESTPOINT (TpProcMemWlPrepDimms, &(NBPtr->MemPtr->StdHeader));
  ASSERT (TargetDIMM < MAX_DIMMS_PER_CHANNEL);
  for (ChipSel = 0; ChipSel < MAX_CS_PER_CHANNEL; ChipSel++) {
    if ((NBPtr->DCTPtr->Timings.CsPresent & ((UINT16)1 << ChipSel)) != 0) {
      if (Wl) {
        // Program WrLvOdt
        NBPtr->SetBitField (NBPtr, BFWrLvOdt, NBPtr->ChannelPtr->PhyWLODT[ChipSel >> 1]);
      }
      NBPtr->SetBitField (NBPtr, BFMrsChipSel, ChipSel);
      // Set MR1 to F2x7C[MrsAddress], F2x7C[MrsBank]=1
      MemTEMRS13 (TechPtr, Wl, TargetDIMM);
      // Send command
      NBPtr->SendMrsCmd (NBPtr);
      // Set MR2 to F2x7C[MrsAddress], F2x7C[MrsBank]=1
      MemTEMRS23 (TechPtr);
      // Send command
      NBPtr->SendMrsCmd (NBPtr);
    }
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function programs seed values for Write Levelization
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in] Dimm - DIMM to be trained
 *     @param[in] Pass - Pass for WL training (1 - 400Mhz or 2 - >400Mhz)
 *
 */

VOID
STATIC
MemTProcConfig3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Dimm,
  IN       UINT8 Pass
  )
{
  DIE_STRUCT *MCTPtr;
  CH_DEF_STRUCT *ChannelPtr;
  MEM_NB_BLOCK *NBPtr;
  UINT16 WrDqsDly;
  // Memclk Delay incurred by register.
  UINT8 MemClkRegDly;
  UINT8 ByteLane;
  UINT8 DefaultSeed;
  UINT8 CurrentSeed;
  UINT8 *Seed;
  UINT8 RCW2;
  UINT16 Speed;

  NBPtr = TechPtr->NBPtr;
  MCTPtr = NBPtr->MCTPtr;
  ChannelPtr = TechPtr->NBPtr->ChannelPtr;

  AGESA_TESTPOINT (TpProcMemWlConfigDimms, &(NBPtr->MemPtr->StdHeader));
  RCW2 = ChannelPtr->CtrlWrd02[Dimm];
  Speed = TechPtr->NBPtr->DCTPtr->Timings.Speed;

  IDS_HDT_CONSOLE ("\n\t\t\tSeeds: ");
  //  Program an initialization Value to registers F2x[1, 0]9C_x[51:50] and F2x[1, 0]9C_x52 to set
  //  the gross and fine delay for all the byte lane fields. If the target frequency is different than 400MHz,
  //  BIOS must execute two training passes for each Dimm. For pass 1 at a 400MHz MEMCLK frequency,
  //  use an initial total delay value.
  if (Pass == 1) {
    // Get the default value of seed
    DefaultSeed = 0x1A;
    if (MCTPtr->Status[SbRegistered]) {
      DefaultSeed = ((RCW2 & BIT0) == 0) ? 0x41 : 0x51;
    }

    if (Speed == DDR667_FREQUENCY) {
      DefaultSeed = (UINT8) ((DefaultSeed * 333 + 399) / 400); //round up
    }
    ASSERT (Speed >= DDR667_FREQUENCY);

    // Get platform override seed
    Seed = (UINT8 *) FindPSOverrideEntry (NBPtr->RefPtr->PlatformMemoryConfiguration, PSO_WL_SEED, MCTPtr->SocketId, ChannelPtr->ChannelID);

    for (ByteLane = 0; ByteLane < 9; ByteLane++) {
      // This includes ECC as byte 8
      CurrentSeed = ((Seed != NULL) ? Seed[ByteLane] : DefaultSeed);
      NBPtr->SetTrainDly (NBPtr, AccessPhRecDly, DIMM_BYTE_ACCESS (Dimm, ByteLane), CurrentSeed);
      ChannelPtr->WrDqsDlys[Dimm * TechPtr->DlyTableWidth () + ByteLane] = CurrentSeed;
      IDS_HDT_CONSOLE ("%02x ", CurrentSeed);
    }
  } else {
    //10.Multiply the previously saved delay values in Pass 1, step #5 by (target frequency)/400 to find
    //the gross and fine delay initialization values at the target frequency. Use these values as the initial
    //seed values when executing Pass 2, step #4.
    for (ByteLane = 0; ByteLane < 9; ByteLane++) {
      // This includes ECC as byte 8
      WrDqsDly = ChannelPtr->WrDqsDlys[Dimm * TechPtr->DlyTableWidth () + ByteLane];
      //
      // For Registered Dimms
      //
      if (MCTPtr->Status[SbRegistered]) {
        MemClkRegDly = ((RCW2 & BIT0) == 0) ? 0x20 : 0x30;
        WrDqsDly = (UINT16) (MemClkRegDly + ((((UINT32) WrDqsDly - MemClkRegDly) * Speed) / TechPtr->PrevSpeed));
      } else {
        //
        // Unbuffered Dimms
        //
        WrDqsDly = (UINT16) (((UINT32) WrDqsDly * Speed) / TechPtr->PrevSpeed);
      }

      ChannelPtr->WrDqsDlys[Dimm * TechPtr->DlyTableWidth () + ByteLane] = (UINT8) WrDqsDly;

      if (NBPtr->IsSupported[WLSeedAdjust]) {
        // Adjust seed to avoid overflowing PRE for the case SeedGross >= 3
        if (WrDqsDly >= 0x60) {
          if ((WrDqsDly & 0x20) != 0) {
            // If (SeedGross is odd) then SeedPreGross = 1
            WrDqsDly = (WrDqsDly & 0x1F) | 0x20;
          } else {
            // If (SeedGross is even) then SeedPreGross = 2
            WrDqsDly = (WrDqsDly & 0x1F) | 0x40;
          }
        }
      }

      NBPtr->SetTrainDly (NBPtr, AccessPhRecDly, DIMM_BYTE_ACCESS (Dimm, ByteLane), WrDqsDly);
      IDS_HDT_CONSOLE ("%02x ", WrDqsDly);
    }
  }
  IDS_HDT_CONSOLE ("\n");
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function begins WL training for a specific DIMM
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in] Dimm - DIMM to be trained
 *
 */

VOID
STATIC
MemTBeginWLTrain3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Dimm
  )
{
  MEM_DATA_STRUCT *MemPtr;
  DIE_STRUCT *MCTPtr;
  MEM_NB_BLOCK *NBPtr;
  UINT8 ByteLane;
  UINT8 Seed;
  UINT8 Delay;

  NBPtr = TechPtr->NBPtr;
  MemPtr = NBPtr->MemPtr;
  MCTPtr = NBPtr->MCTPtr;
  // Assert ODT pins for write leveling
  NBPtr->SetBitField (NBPtr, BFWrLvOdtEn, 1);

  // Wait 10 MEMCLKs to allow for ODT signal settling.
  MemUWait10ns (3, MemPtr);

  IDS_HDT_CONSOLE ("\t\t\tWrtLvTrEn = 1\n");
  // Program F2x[1, 0]9C_x08[WrtLlTrEn]=1.
  NBPtr->SetBitField (NBPtr, BFWrtLvTrEn, 1);

  //  Wait 200 MEMCLKs. If executing pass 2, wait 32 MEMCLKs.
  MemUWait10ns (50, MemPtr);

  //  Program F2x[1, 0]9C_x08[WrtLlTrEn]=0.
  NBPtr->SetBitField (NBPtr, BFWrtLvTrEn, 0);

  //  Read from registers F2x[1, 0]9C_x[51:50] and F2x[1, 0]9C_x52 to get the gross and fine Delay settings
  //  for the target Dimm and save these values.
  IDS_HDT_CONSOLE ("\t\t\t  PRE: ");
  for (ByteLane = 0; ByteLane < (MCTPtr->Status[SbEccDimms] ? 9 : 8) ; ByteLane++) {
    // This includes ECC as byte 8
    Seed = NBPtr->ChannelPtr->WrDqsDlys[(Dimm * TechPtr->DlyTableWidth ()) + ByteLane];
    Delay = (UINT8)NBPtr->GetTrainDly (NBPtr, AccessPhRecDly, DIMM_BYTE_ACCESS (Dimm, ByteLane));
//    IDS_HDT_CONSOLE ("%v1%02x ", Delay);
    IDS_HDT_CONSOLE ("%02x ", Delay);

    if ((Delay > (Seed + 0x20)) || (Seed > (Delay + 0x20))) {
      //
      // If PRE comes back with more than Seed +/- 0x20, then this is an
      //   unexpected condition.  Log the condition.
      //
      PutEventLog (AGESA_ERROR, MEM_ERROR_WL_PRE_OUT_OF_RANGE, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, ((Seed << 8) + Delay), &NBPtr->MemPtr->StdHeader);
    }
    if ((NBPtr->IsSupported[WLSeedAdjust]) && (Seed >= 0x60)) {
      // Recover WrDqsGrossDly:
      // WrDqsGrossDly = SeedGross + PhRecGrossDlyByte - SeedPreGross
      if ((Seed & 0x20) != 0) {
        // If (SeedGross is odd) then SeedPreGross = 1
        Delay += (Seed & 0xE0) - 0x20;
      } else {
        // If (SeedGross is even) then SeedPreGross = 2
        Delay += (Seed & 0xE0) - 0x40;
      }
    } else if (((Seed >> 5) == 0) && ((Delay >> 5) == 3)) {
      IDS_OPTION_HOOK (IDS_CHECK_NEGATIVE_WL, &Delay, &(TechPtr->NBPtr->MemPtr->StdHeader));
      // If seed has gross delay of 0 and PRE has gross delay of 3,
      // then round the total delay of TxDqs to 0.
      Delay = 0;
    }
    NBPtr->SetTrainDly (NBPtr, AccessWrDqsDly, DIMM_BYTE_ACCESS (Dimm, ByteLane), Delay);
    NBPtr->ChannelPtr->WrDqsDlys[(Dimm * TechPtr->DlyTableWidth ()) + ByteLane] = Delay;
//    IDS_HDT_CONSOLE ("%v2%02x ", Delay);
  }
#if 0
  IDS_HDT_CONSOLE ("%v0");
  IDS_HDT_CONSOLE ("\t\t\tPRE:   %vh1\n");
  IDS_HDT_CONSOLE ("\t\t\tWrDqs: %vh2\n\n");
#endif
    IDS_HDT_CONSOLE ("\n\t\t\tWrDqs: ");
    for (ByteLane = 0; ByteLane < (MCTPtr->Status[SbEccDimms] ? 9 : 8); ByteLane++) {
      IDS_HDT_CONSOLE ("%02x ", NBPtr->ChannelPtr->WrDqsDlys[(Dimm * TechPtr->DlyTableWidth ()) + ByteLane]);
    }
    IDS_HDT_CONSOLE("\n\n");

  // Disable write leveling ODT pins
  NBPtr->SetBitField (NBPtr, BFWrLvOdtEn, 0);

  // Wait 10 MEMCLKs to allow for ODT signal settling.
  MemUWait10ns (3, MemPtr);

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function programs register after Phy assisted training is finish.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */

BOOLEAN
MemTExitPhyAssistedTrainingClient3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  TechPtr->NBPtr->BrdcstSet (TechPtr->NBPtr, BFRxPtrInitReq, 1);
  TechPtr->NBPtr->BrdcstSet (TechPtr->NBPtr, BFDisDllShutdownSR, 1);
  TechPtr->NBPtr->BrdcstSet (TechPtr->NBPtr, BFEnterSelfRef, 1);
  TechPtr->NBPtr->PollBitField (TechPtr->NBPtr, BFEnterSelfRef, 0, PCI_ACCESS_TIMEOUT, TRUE);
  TechPtr->NBPtr->BrdcstSet (TechPtr->NBPtr, BFDbeGskMemClkAlignMode, 2);
  TechPtr->NBPtr->BrdcstSet (TechPtr->NBPtr, BFExitSelfRef, 1);
  TechPtr->NBPtr->PollBitField (TechPtr->NBPtr, BFExitSelfRef, 0, PCI_ACCESS_TIMEOUT, TRUE);
  TechPtr->NBPtr->BrdcstSet (TechPtr->NBPtr, BFDisDllShutdownSR, 0);
  return (BOOLEAN) (TechPtr->NBPtr->MCTPtr->ErrCode < AGESA_FATAL);
}

