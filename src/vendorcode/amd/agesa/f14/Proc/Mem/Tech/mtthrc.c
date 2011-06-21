/* $NoKeywords:$ */
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
 * @e \$Revision: 38442 $ @e \$Date: 2010-09-24 06:39:57 +0800 (Fri, 24 Sep 2010) $
 *
 **/
/*
 *****************************************************************************
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
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

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

  TechPtr->TrainingType = TRN_RCVR_ENABLE;

  AGESA_TESTPOINT (TpProcMemReceiverEnableTraining , &(NBPtr->MemPtr->StdHeader));
  IDS_HDT_CONSOLE (MEM_STATUS, "\nStart HW RxEn training\n");

  // Set environment settings before training
  MemTBeginTraining (TechPtr);
  //
  // Setup hardware training engine (if applicable)
  //
  NBPtr->FamilySpecificHook[SetupHwTrainingEngine] (NBPtr, &TechPtr->TrainingType);

  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    IDS_HDT_CONSOLE (MEM_STATUS, "\tDct %d\n", Dct);
    NBPtr->SwitchDCT (NBPtr, Dct);
    //training for each rank
    for (ChipSel = 0; ChipSel < MAX_CS_PER_CHANNEL; ChipSel++) {
      if (NBPtr->GetSysAddr (NBPtr, ChipSel, &TestAddrRJ16)) {
        if (!(NBPtr->MCTPtr->Status[SbLrdimms]) || ((NBPtr->ChannelPtr->LrDimmPresent & ((UINT8) 1 << (ChipSel >> 1))) != 0)) {
          // 1.Prepare the DIMMs for training
          NBPtr->SetBitField (NBPtr, BFTrDimmSel, ChipSel >> 1);

          TechPtr->ChipSel = ChipSel;
          TechPtr->Pass = Pass;
          NBPtr->FamilySpecificHook[InitPerNibbleTrn] (NBPtr, NULL);
          for (TechPtr->TrnNibble = NIBBLE_0; TechPtr->TrnNibble <= (NBPtr->FamilySpecificHook[TrainRxEnPerNibble] (NBPtr, &ChipSel)? NIBBLE_0 : NIBBLE_1); TechPtr->TrnNibble++) {
            // 2.Prepare the phy for DQS receiver enable training.
            IDS_HDT_CONSOLE (MEM_STATUS, "\t\tCS %d\n", ChipSel);
            IDS_HDT_CONSOLE (MEM_FLOW, "\t\tTestAddr %x0000\n", TestAddrRJ16);
            IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\t     Byte:  00  01  02  03  04  05  06  07  ECC\n");
            NBPtr->MemNPrepareRcvrEnDlySeed (NBPtr);

            // 3.BIOS initiates the phy assisted receiver enable training
            NBPtr->SetBitField (NBPtr, BFDqsRcvTrEn, 1);

            // 4.BIOS begins sending out of back-to-back reads to create
            //   a continuous stream of DQS edges on the DDR interface
            NBPtr->GenHwRcvEnReads (NBPtr, TestAddrRJ16);

            // 7.Program [DqsRcvTrEn]=0 to stop the DQS receive enable training.
            NBPtr->SetBitField (NBPtr, BFDqsRcvTrEn, 0);

            // 8.Get the gross and fine delay values.
            // 9.Calculate the corresponding final delay values
            MemTProgramRcvrEnDly (TechPtr, ChipSel, Pass);
          }
        }
      }
    }
  }
  // Restore environment settings after training
  MemTEndTraining (TechPtr);
  IDS_HDT_CONSOLE (MEM_FLOW, "End HW RxEn training\n\n");

  return (BOOLEAN) (NBPtr->MCTPtr->ErrCode < AGESA_FATAL);
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
  UINT8  ByteLane;
  UINT16 RcvEnDly;
  UINT16 CsPairRcvEnDly;
  UINT16 RankRcvEnDly[9];
  NBPtr = TechPtr->NBPtr;
  ChannelPtr = TechPtr->NBPtr->ChannelPtr;
  IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\t      PRE: ");
  for (ByteLane = 0; ByteLane < (NBPtr->MCTPtr->Status[SbEccDimms] ? 9 : 8) ; ByteLane++) {
    RcvEnDly = (UINT8) NBPtr->GetTrainDly (NBPtr, AccessPhRecDly, DIMM_BYTE_ACCESS (ChipSel >> 1, ByteLane));
    IDS_HDT_CONSOLE (MEM_FLOW, "%03x ", RcvEnDly);

    RcvEnDly = RcvEnDly + TechPtr->DiffSeedGrossSeedPreGross[ByteLane];

    // Add 1 UI to get to the midpoint of preamble
    RcvEnDly += 0x20;
    TechPtr->Bytelane = ByteLane;
    RankRcvEnDly[ByteLane] = RcvEnDly;
    if (NBPtr->FamilySpecificHook[TrainRxEnAdjustDlyPerNibble] (NBPtr, &RcvEnDly)) {
      if ((ChipSel & 1) == 1) {
        // For each rank pair on a dual-rank DIMM, compute the average value of the total delays saved during the
        //  training of each rank and program the result in D18F2x[1,0]9C_x0000_00[24:10][DqsRcvEnGrossDelay,
        //  DqsRcvEnFineDelay].
        CsPairRcvEnDly = ChannelPtr->RcvEnDlys[(ChipSel >> 1) * TechPtr->DlyTableWidth () + ByteLane];
        RcvEnDly = (CsPairRcvEnDly + RcvEnDly + 1) / 2;
      }
    }
    ChannelPtr->RcvEnDlys[(ChipSel >> 1) * TechPtr->DlyTableWidth () + ByteLane] = RcvEnDly;
    NBPtr->SetTrainDly (NBPtr, AccessRcvEnDly, DIMM_BYTE_ACCESS ((ChipSel >> 1), ByteLane), RcvEnDly);
  }

  IDS_HDT_CONSOLE_DEBUG_CODE (
    IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\t\t     RxEn: ");
    for (ByteLane = 0; ByteLane < (NBPtr->MCTPtr->Status[SbEccDimms] ? 9 : 8); ByteLane++) {
      IDS_HDT_CONSOLE (MEM_FLOW, "%03x ", RankRcvEnDly[ByteLane]);
    }
    if (NBPtr->FamilySpecificHook[TrainRxEnGetAvgDlyPerNibble] (NBPtr, NULL)) {
      if ((ChipSel & 1) == 1) {
        IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\t\t      Avg: ");
        for (ByteLane = 0; ByteLane < (NBPtr->MCTPtr->Status[SbEccDimms] ? 9 : 8); ByteLane++) {
          IDS_HDT_CONSOLE (MEM_FLOW, "%03x ", ChannelPtr->RcvEnDlys[(ChipSel >> 1) * TechPtr->DlyTableWidth () + ByteLane]);
        }
      }
    }
    IDS_HDT_CONSOLE (MEM_FLOW, "\n\n");
  );
}
