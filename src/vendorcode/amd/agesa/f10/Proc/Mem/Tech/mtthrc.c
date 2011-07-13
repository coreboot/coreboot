/**
 * @file
 *
 * mtthrc.c
 *
 * Phy assisted DQS receiver enable training
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Tech)
 * @e \$Revision: 7171 $ @e \$Date: 2008-08-05 11:42:14 -0500 (Tue, 05 Aug 2008) $
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
#include "amdlib.h"
#include "Ids.h"
#include "mm.h"
#include "mn.h"
#include "mu.h"
#include "mt.h"
#include "Filecode.h"
#define FILECODE PROC_MEM_TECH_MTTHRC_FILECODE
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
MemTPrepareRcvrEnDlySeed (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 ChipSel,
  IN       UINT8 Pass
  );

VOID
STATIC
MemTProgramRcvrEnDly (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 ChipSel,
  IN       UINT8 Pass
  );

BOOLEAN
STATIC
MemTDqsTrainRcvrEnHw (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Pass
  );

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
extern UINT16 T1minToFreq[];

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function executes first pass of Phy assisted receiver enable training
 *      for current node at DDR800 and below.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @pre   Auto refresh and ZQCL must be disabled
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */
BOOLEAN
MemTDqsTrainRcvrEnHwPass1 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  return MemTDqsTrainRcvrEnHw (TechPtr, 1);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function executes second pass of Phy assisted receiver enable training
 *      for current node at DDR1066 and above.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @pre   Auto refresh and ZQCL must be disabled
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */
BOOLEAN
MemTDqsTrainRcvrEnHwPass2 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  // If current speed is higher than start-up speed, do second pass of WL
  if (TechPtr->NBPtr->DCTPtr->Timings.Speed > TechPtr->NBPtr->StartupSpeed) {
    return MemTDqsTrainRcvrEnHw (TechPtr, 2);
  }
  return TRUE;
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function executes Phy assisted receiver enable training for current node.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]  Pass - Pass of the receiver training
 *
 *     @pre   Auto refresh and ZQCL must be disabled
 *
 */
BOOLEAN
STATIC
MemTDqsTrainRcvrEnHw (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Pass
  )
{
  MEM_NB_BLOCK  *NBPtr;
  UINT32 TestAddrRJ16;
  UINT8  Dct;
  UINT8  ChipSel;

  NBPtr = TechPtr->NBPtr;

  AGESA_TESTPOINT (TpProcMemReceiverEnableTraining , &(NBPtr->MemPtr->StdHeader));
  IDS_HDT_CONSOLE ("!\nStart HW RxEn training\n");

  // Set environment settings before training
  MemTBeginTraining (TechPtr);

  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    IDS_HDT_CONSOLE ("!\tDct %d\n", Dct);
    NBPtr->SwitchDCT (NBPtr, Dct);
    //training for each rank
    for (ChipSel = 0; ChipSel < MAX_CS_PER_CHANNEL; ChipSel++) {
      if (NBPtr->GetSysAddr (NBPtr, ChipSel, &TestAddrRJ16)) {
        if ((ChipSel & 1) == 0) {
          // 1.Prepare the DIMMs for training
          NBPtr->SetBitField (NBPtr, BFTrDimmSel, ChipSel >> 1);

          // 2.Prepare the phy for DQS receiver enable training.
          MemTPrepareRcvrEnDlySeed (TechPtr, ChipSel, Pass);
        }

        IDS_HDT_CONSOLE ("!\t\tCS %d\n", ChipSel);
        IDS_HDT_CONSOLE ("\t\tTestAddr %lx0000\n", TestAddrRJ16);

        // 3.BIOS initiates the phy assisted receiver enable training
        NBPtr->SetBitField (NBPtr, BFDqsRcvTrEn, 1);

        // 4.BIOS begins sending out of back-to-back reads to create
        //   a continuous stream of DQS edges on the DDR interface.
        NBPtr->GenHwRcvEnReads (NBPtr, TestAddrRJ16);

        // 6.Wait 200 MEMCLKs.
        MemUWait10ns (200, NBPtr->MemPtr);

        // 7.Program [DqsRcvTrEn]=0 to stop the DQS receive enable training.
        NBPtr->SetBitField (NBPtr, BFDqsRcvTrEn, 0);

        // 8.Get the gross and fine delay values.
        // 9.Calculate the corresponding final delay values
        MemTProgramRcvrEnDly (TechPtr, ChipSel, Pass);
      }
    }
    // Set Max Latency for both channels
    if (TechPtr->FindMaxDlyForMaxRdLat (TechPtr, &ChipSel)) {
      NBPtr->SetMaxLatency (NBPtr, TechPtr->MaxDlyForMaxRdLat);
    }
    TechPtr->ResetDCTWrPtr (TechPtr, 6);
  }

  // Restore environment settings after training
  MemTEndTraining (TechPtr);
  IDS_HDT_CONSOLE ("End HW RxEn training\n\n");

  return (BOOLEAN) (NBPtr->MCTPtr->ErrCode < AGESA_FATAL);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function calculates RcvEn seed value for each rank
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]  ChipSel - rank to be trained
 *     @param[in]  Pass - Pass of the receiver training
 *
 */
VOID
STATIC
MemTPrepareRcvrEnDlySeed (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 ChipSel,
  IN       UINT8 Pass
  )
{
  MEM_NB_BLOCK  *NBPtr;
  CH_DEF_STRUCT *ChannelPtr;
  UINT16 Seed;
  UINT16 SeedPass1Remainder;
  UINT8  ByteLane;
  UINT16 RcvEnDly;
  UINT16 Speed;
  UINT8  Dimm;

  NBPtr = TechPtr->NBPtr;
  ChannelPtr = TechPtr->NBPtr->ChannelPtr;
  Speed = NBPtr->DCTPtr->Timings.Speed;
  Dimm = ChipSel >> 1;

  IDS_HDT_CONSOLE ("\n\t\t\tSeeds: ");
  for (ByteLane = 0; ByteLane < 8; ByteLane++) {
    if (Pass == 1) {
      // For Pass1, BIOS starts with the delay value obtained from the first pass of write
      // levelization training that was done in DDR3 Training and add a delay value of 3Bh.
      Seed = ChannelPtr->WrDqsDlys[Dimm * TechPtr->DlyTableWidth () + ByteLane] + 0x3B;
    } else {
      // For Pass2, for each byte lane, BIOS uses the results obtained
      // from Pass 1 substract 1 UI to get back to preamble left edge.
      RcvEnDly = ChannelPtr->RcvEnDlys[Dimm * TechPtr->DlyTableWidth () + ByteLane] - 0x20;
      // Scale the seed to new speed
      Seed = (UINT16) (((UINT32) RcvEnDly * Speed) / TechPtr->PrevSpeed);
    }
    // Next, determine the gross component of SeedTotal. SeedGrossPass1=SeedTotal DIV 32.
    // Then, determine the fine delay component of SeedTotal. SeedFinePass1=SeedTotal MOD 32.
    // Use SeedGrossPass1 to determine SeedPreGrossPass1:
    SeedPass1Remainder = Seed & 0x1E0;
    if ((Seed & 0x20) != 0) {
      //SeedPreGrossPass1=1 if SeedGrossPass1 is odd
      Seed = (Seed & 0x1F) | 0x20;
      SeedPass1Remainder = SeedPass1Remainder - 0x20;
    } else {
      //SeedPreGrossPass1=2 if SeedGrossPass1 is even
      Seed = (Seed & 0x1F) | 0x40;
      SeedPass1Remainder = SeedPass1Remainder - 0x40;
    }

    // The last term that BIOS needs to calculate for the seed is called SeedPass1Remainder.
    // SeedPass1Remainder=SeedGrossPass1-SeedPreGrossPass1. BIOS saves this value which is
    // used to determine the final delay value. This is necessary because the phy does not have the full
    // range of delay adjustment.
    ChannelPtr->RcvEnDlys[Dimm * TechPtr->DlyTableWidth () + ByteLane] = SeedPass1Remainder << 6;

    //BIOS programs registers F2x[1, 0]9C_x[51:50] and F2x[1, 0]9C_x52) with SeedPreGrossPass1
    //and SeedFinePass1 from the preceding steps.
    NBPtr->SetTrainDly (NBPtr, AccessPhRecDly, DIMM_BYTE_ACCESS (Dimm, ByteLane), Seed);
    IDS_HDT_CONSOLE ("%02x ", Seed);

    // 202688: Program seed value to RcvEnDly also.
    NBPtr->SetTrainDly (NBPtr, AccessRcvEnDly, DIMM_BYTE_ACCESS (Dimm, ByteLane), Seed);
  }
  IDS_HDT_CONSOLE ("\n");
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function calculates final RcvrEnDly for each rank
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]  ChipSel - Rank to be trained
 *     @param[in]  Pass - Pass of the receiver training
 *
 */
VOID
STATIC
MemTProgramRcvrEnDly (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 ChipSel,
  IN       UINT8 Pass
  )
{
  MEM_NB_BLOCK  *NBPtr;
  CH_DEF_STRUCT *ChannelPtr;
  UINT16 SeedPass1Remainder;
  UINT8  ByteLane;
  UINT16 RcvEnDly;
  UINT8  Dimm;

  NBPtr = TechPtr->NBPtr;
  ChannelPtr = TechPtr->NBPtr->ChannelPtr;
  Dimm = ChipSel >> 1;

  for (ByteLane = 0; ByteLane < 8; ByteLane++) {
    SeedPass1Remainder = (ChannelPtr->RcvEnDlys[(Dimm * TechPtr->DlyTableWidth ()) + ByteLane] >> 6) & 0x1E0;
    RcvEnDly = (UINT8) NBPtr->GetTrainDly (NBPtr, AccessPhRecDly, DIMM_BYTE_ACCESS (Dimm, ByteLane));
    IDS_HDT_CONSOLE ("%02x ", RcvEnDly);

    // Calculate the corresponding final delay values:
    // [DqsRcvEnGrossDelay]= SeedPass1Remainder+PhRecGrossDlyByte
    // [DqsRcvEnFineDelay]=PhRecFineDlyByte.
    RcvEnDly = RcvEnDly + SeedPass1Remainder;

    // Add 1 UI to get to the midpoint of preamble
    RcvEnDly += 0x20;

    if ((ChipSel & 1) == 0) {
      if ((NBPtr->DCTPtr->Timings.CsPresent & ((UINT16) 1 << (ChipSel + 1))) != 0) {
        // If dual-rank, save the trained result of the first rank
        ChannelPtr->RcvEnDlys[Dimm * TechPtr->DlyTableWidth () + ByteLane] |= RcvEnDly;
      } else {
        ChannelPtr->RcvEnDlys[Dimm * TechPtr->DlyTableWidth () + ByteLane] = RcvEnDly;
        NBPtr->SetTrainDly (NBPtr, AccessRcvEnDly, DIMM_BYTE_ACCESS (Dimm, ByteLane), RcvEnDly);
      }
    } else {
      // When having the result from second rank, average with first rank's and program to register
      RcvEnDly = ((ChannelPtr->RcvEnDlys[Dimm * TechPtr->DlyTableWidth () + ByteLane] & 0x3FF) + RcvEnDly + 1) / 2;
      ChannelPtr->RcvEnDlys[Dimm * TechPtr->DlyTableWidth () + ByteLane] = RcvEnDly;
      NBPtr->SetTrainDly (NBPtr, AccessRcvEnDly, DIMM_BYTE_ACCESS (Dimm, ByteLane), RcvEnDly);
    }
    IDS_HDT_CONSOLE ("%02x ", RcvEnDly);
  }
}
