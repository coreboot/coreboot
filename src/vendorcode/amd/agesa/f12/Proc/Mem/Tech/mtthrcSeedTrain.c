/* $NoKeywords:$ */
/**
 * @file
 *
 * mtthrcSt.c
 *
 * Phy assisted DQS receiver enable seedless training
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Tech)
 * @e \$Revision: 42643 $ @e \$Date: 2010-11-24 13:51:41 -0600 (Wed, 24 Nov 2010) $
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
#include "mt.h"
#include "mttEdgeDetect.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_TECH_MTTHRCSEEDTRAIN_FILECODE
/*----------------------------------------------------------------------------
3 *                          DEFINITIONS AND MACROS
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
MemTRdPosRxEnSeedSetDly3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   UINT16 RcvEnDly,
  IN OUT   UINT8 ByteLane
  );

VOID
STATIC
MemTRdPosRxEnSeedCheckRxEndly3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );
/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
/*-----------------------------------------------------------------------------
 *
 *
 *     This function checks each bytelane for no window error.
 *
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in,out]   OptParam   - Optional parameter
 *
 *     @return    TRUE
 * ----------------------------------------------------------------------------
 */
BOOLEAN
MemTTrackRxEnSeedlessRdWrNoWindBLError (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *OptParam
  )
{
  UINT8 i;
  SWEEP_INFO SweepData;
  SweepData = *(SWEEP_INFO*)OptParam;
  for (i = 0; i < ((TechPtr->NBPtr->MCTPtr->Status[SbEccDimms] && TechPtr->NBPtr->IsSupported[EccByteTraining]) ? 9 : 8) ; i++) {
    //
    /// Skip Bytelanes that have already reached the desired result
    //
    if ((SweepData.ResultFound & ((UINT16)1 << i)) == 0) {
      if (SweepData.TrnDelays[i] == SweepData.EndDelay) {
        if ((SweepData.EndResult & ((UINT16) (1 << i))) != 0) {
          TechPtr->ByteLaneError[i] = TRUE;
        } else {
          TechPtr->ByteLaneError[i] = FALSE;
        }
      }
    }
  }
  return TRUE;
}
/*-----------------------------------------------------------------------------
 *
 *
 *      This function checks each bytelane for small window error.
 *
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in,out]   OptParam   - Optional parameter
 *
 *     @return    TRUE
 * ----------------------------------------------------------------------------
 */
BOOLEAN
MemTTrackRxEnSeedlessRdWrSmallWindBLError (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *OptParam
  )
{
  SWEEP_INFO SweepData;
  SweepData = *(SWEEP_INFO*)OptParam;
  if (SweepData.Error == TRUE) {
    TechPtr->ByteLaneError[TechPtr->Bytelane] = TRUE;
  } else {
    TechPtr->ByteLaneError[TechPtr->Bytelane] = FALSE;
  }
  return TRUE;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function sets the RxEn delay
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in,out]   *RcvEnDly   - Receiver Enable Delay
 *     @param[in,out]   *ByteLane   - Bytelane
 *
*/
VOID
STATIC
MemTRdPosRxEnSeedSetDly3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   UINT16 RcvEnDly,
  IN OUT   UINT8 ByteLane
  )
{
  TechPtr->NBPtr->ChannelPtr->RcvEnDlys[(TechPtr->ChipSel >> 1) * TechPtr->DlyTableWidth () + ByteLane] = RcvEnDly;
  TechPtr->NBPtr->SetTrainDly (TechPtr->NBPtr, AccessRcvEnDly, DIMM_BYTE_ACCESS ((TechPtr->ChipSel >> 1), ByteLane), RcvEnDly);
  TechPtr->NBPtr->FamilySpecificHook[ResetRxFifoPtr] (TechPtr->NBPtr, TechPtr->NBPtr);
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function determines if the currert RxEn delay settings have failed
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
*/
VOID
STATIC
MemTRdPosRxEnSeedCheckRxEndly3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  UINT8  MaxDlyDimm;
  TechPtr->FindMaxDlyForMaxRdLat (TechPtr, &MaxDlyDimm);
  TechPtr->NBPtr->SetMaxLatency (TechPtr->NBPtr, TechPtr->MaxDlyForMaxRdLat);
  TechPtr->DqsRdWrPosSaved = 0;
  MemTTrainDQSEdgeDetect (TechPtr);
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function executes RdDQS training and if fails adjusts the RxEn Gross results for
 *     each bytelane
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  TRUE - All bytelanes pass
 *     @return  FALSE - Some bytelanes fail
*/
BOOLEAN
MemTRdPosWithRxEnDlySeeds3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  UINT8  ByteLane;
  UINT16 PassTestRxEnDly[MAX_BYTELANES_PER_CHANNEL + 1];
  UINT16 FailTestRxEnDly[MAX_BYTELANES_PER_CHANNEL + 1];
  UINT16 FinalRxEnCycle[MAX_BYTELANES_PER_CHANNEL + 1];
  UINT16 RxOrig[MAX_BYTELANES_PER_CHANNEL];
  UINT8  i;
  UINT8  j;
  UINT8  NumBLWithTargetFound;
  UINT8  MaxByteLanes;
  INT16  RxEn;
  BOOLEAN status;
  BOOLEAN EsbNoDqsPosSave;
  BOOLEAN OutOfRange[MAX_BYTELANES_PER_CHANNEL];
  BOOLEAN ByteLanePass[MAX_BYTELANES_PER_CHANNEL];
  BOOLEAN ByteLaneFail[MAX_BYTELANES_PER_CHANNEL];
  BOOLEAN RxEnMemClkTested[MAX_BYTELANES_PER_CHANNEL][MAX_POS_RX_EN_SEED_GROSS_RANGE];
  BOOLEAN RxEnMemClkSt[MAX_BYTELANES_PER_CHANNEL][MAX_POS_RX_EN_SEED_GROSS_RANGE];
  BOOLEAN RxEnDlyTargetFound[MAX_BYTELANES_PER_CHANNEL];
  BOOLEAN DlyWrittenToReg[MAX_BYTELANES_PER_CHANNEL];
  UINT16 RxEnDlyTargetValue[MAX_BYTELANES_PER_CHANNEL];
  UINT8 AllByteLanesOutOfRange;
  UINT8 AllByteLanesSaved;
  UINT8 TotalByteLanesCheckedForSaved;
  UINT8  MemClkCycle;
  MEM_NB_BLOCK  *NBPtr;
  CH_DEF_STRUCT *ChannelPtr;
  NBPtr = TechPtr->NBPtr;
  ChannelPtr = TechPtr->NBPtr->ChannelPtr;
  NumBLWithTargetFound = 0;
  status = FALSE;
  EsbNoDqsPosSave = TechPtr->NBPtr->MCTPtr->ErrStatus[EsbNoDqsPos];
  NBPtr->RdDqsDlyRetrnStat = RDDQSDLY_RTN_SUSPEND;
  IDS_HDT_CONSOLE (MEM_FLOW, "\n\nStart HW RxEn Seedless training\n\n");
  // 1. Program D18F2x9C_x0D0F_0[F,8:0]30_dct[1:0][BlockRxDqsLock] = 1.
  NBPtr->SetBitField (NBPtr, BFBlockRxDqsLock, 0x0100);
  IDS_HDT_CONSOLE (MEM_FLOW, "\tChip Select: %02x \n", TechPtr->ChipSel);
  IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\t       Byte:  00  01  02  03  04  05  06  07  ECC\n");
  IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\t\tRxEn Orig: ");
  //
  // Start sweep loops for RxEn Seedless Training
  //
  MaxByteLanes = (TechPtr->NBPtr->MCTPtr->Status[SbEccDimms] && TechPtr->NBPtr->IsSupported[EccByteTraining]) ? 9 : 8;    //dmach
  //
  //Initialialize BL variables
  //
  for (ByteLane = 0; ByteLane < MaxByteLanes; ByteLane++) {
    OutOfRange[ByteLane] = FALSE;
    ByteLanePass[ByteLane] = FALSE;
    ByteLaneFail[ByteLane] = FALSE;
    // 2. RxEnOrig = D18F2x9C_x0000_00[2A:10]_dct[1:0][DqsRcvEnGrossDelay, DqsRcvEnFineDelay] result
    //    from 2.10.6.8.2 [Phy Assisted DQS Receiver Enable Training]
    RxOrig[ByteLane] = TechPtr->RxOrig[ByteLane]; // Original RxEn Dly based on PRE results
    RxEnDlyTargetFound[ByteLane] = FALSE;
    RxEnDlyTargetValue[ByteLane] = 0;
    IDS_HDT_CONSOLE (MEM_FLOW, "%03x ", RxOrig[ByteLane]);
    for (i = 0; i < MAX_POS_RX_EN_SEED_GROSS_RANGE; i++) {
      RxEnMemClkTested[ByteLane][i] = FALSE;
    }
  }
  // Start MemClk delay sweep
  for (i = 0; i < MAX_POS_RX_EN_SEED_GROSS_RANGE; i++) {
    IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\ti: %02x\n", i);
    // Start direction sweep (0, - Positive, 1 - negative)
    for (j = 0; j < MAX_POS_RX_EN_SEED_GROSS_DIR; j++) {
      // Edge detect may run twice to see Pass to fail transition
      // It is not run if the value are already saved
      // Fail test is only done if pass is found
      IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\tj: %02x\n", j);
      // Reset Bytelane Flags for next sweep
      for (ByteLane = 0; ByteLane < MaxByteLanes; ByteLane++) {
        ByteLaneFail[ByteLane] = FALSE;
        ByteLanePass[ByteLane] = FALSE;
        OutOfRange[ByteLane] = FALSE;
      }
      if (i == 0 && j == 1) {
        continue; // Since i & j are the same skip
      }
      IDS_HDT_CONSOLE_DEBUG_CODE (
        IDS_HDT_CONSOLE (MEM_FLOW, "\t             Byte:  00  01  02  03  04  05  06  07  ECC\n");
        IDS_HDT_CONSOLE (MEM_FLOW, "\t  Target BL Found: ");
        for (ByteLane = 0; ByteLane < MaxByteLanes; ByteLane++) {
         IDS_HDT_CONSOLE (MEM_FLOW, " %c  ", ((RxEnDlyTargetFound[ByteLane] == TRUE) ? 'Y' : 'N'));
        }
        IDS_HDT_CONSOLE (MEM_FLOW, "\n\t  Target BL Value: ");
        for (ByteLane = 0; ByteLane < MaxByteLanes; ByteLane++) {
          IDS_HDT_CONSOLE (MEM_FLOW, "%03x ", RxEnDlyTargetValue[ByteLane]);
        }
      );
      //
      //  Find the RxEn Delay for the Pass condition in the Pass to Fail transition
      //              "PassTestRxEnDly"
      //
      IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\t Setting PassTestRxEnDly\n");
      IDS_HDT_CONSOLE (MEM_FLOW, "\t  PassTestRxEnDly: ");
      PassTestRxEnDly[ByteLane] = RxOrig[ByteLane];
      for (ByteLane = 0; ByteLane < MaxByteLanes; ByteLane++) {
        if (RxEnDlyTargetFound[ByteLane] == FALSE) {
          // Calculate "PassTestRxEnDly" from  current "RxEnDly"
          // 3. RxEnOffset = MOD(RxEnOrig + 0x10, 0x40)
          RxEn = (j == 0) ? ((INT16)RxOrig[ByteLane] + 0x10 + (0x40*i)) : ((INT16)RxOrig[ByteLane] + 0x10 - (0x40*i));
          // Check if RxEnDly is in a valid range
          if ((RxEn >= NBPtr->MinRxEnSeedGross) && (RxEn <= NBPtr->MaxRxEnSeedTotal)) {
            PassTestRxEnDly[ByteLane] = (UINT16)RxEn;
            // 4. For each DqsRcvEn value beginning from RxEnOffset incrementing by 1 MEMCLK:
            //    A. Program D18F2x9C_x0000_00[2A:10]_dct[1:0][DqsRcvEnGrossDelay, DqsRcvEnFineDelay] with
            //    the current value.
            MemTRdPosRxEnSeedSetDly3 (TechPtr, PassTestRxEnDly[ByteLane], ByteLane);
            OutOfRange[ByteLane] = FALSE;
          } else {
            OutOfRange[ByteLane] = TRUE;
          }
        } else {
          PassTestRxEnDly[ByteLane] = RxEnDlyTargetValue[ByteLane];
        }
        IDS_HDT_CONSOLE (MEM_FLOW, "%03x ", PassTestRxEnDly[ByteLane]);
      }
      // Check if all BLs out of Range at "PassTestRxEnDly"
      IDS_HDT_CONSOLE (MEM_FLOW, "\n\t       OutOfRange: ");
      AllByteLanesOutOfRange = 0;
      for (ByteLane = 0; ByteLane < MaxByteLanes; ByteLane++) {
        if (OutOfRange[ByteLane]) {
          AllByteLanesOutOfRange++;
        }
        IDS_HDT_CONSOLE (MEM_FLOW, " %c  ", (OutOfRange[ByteLane] == TRUE) ? 'Y' : 'N');
      }
      if (AllByteLanesOutOfRange == MaxByteLanes) {
        continue;  // All BLs out of range, so skip
      }
      // Check if all BLs saved Results  at "PassTestRxEnDly"
      AllByteLanesSaved = 0;
      for (ByteLane = 0; ByteLane < MaxByteLanes; ByteLane++) {
        MemClkCycle = (UINT8) (PassTestRxEnDly[ByteLane] >> 5);
        if (RxEnDlyTargetFound[ByteLane] == FALSE) {
          if (!RxEnMemClkTested[ByteLane][MemClkCycle]) {
            AllByteLanesSaved++;
          }
        }
      }
      // Check if "RxEnDlyValueForPassCond" passed
      if (AllByteLanesSaved != 0) {
        // At least one BL has not been saved, so check if "PassTestRxEnDly" passed
        IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\t Checking if PassTestRxEnDly Passes?\n\n");
        // 4B. Perform 2.10.6.8.5 [DQS Position Training].
        //     Record the result for the current DqsRcvEn setting as a pass or fail depending if a data eye is found.
        MemTRdPosRxEnSeedCheckRxEndly3 (TechPtr);
        IDS_HDT_CONSOLE_DEBUG_CODE (
          IDS_HDT_CONSOLE (MEM_FLOW, "\n\t             Byte:  00  01  02  03  04  05  06  07  ECC\n");
          IDS_HDT_CONSOLE (MEM_FLOW, "\t\t     Err Status: ");
          for (ByteLane = 0; ByteLane < MaxByteLanes; ByteLane++) {
            IDS_HDT_CONSOLE (MEM_FLOW, " %c  ", (TechPtr->ByteLaneError[ByteLane] == TRUE) ? 'F' : 'P');
          }
        );
      } else {
        // All BLs saved, so use saved results for "PassTestRxEnDly"
        IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\tAll BLs Saved at PassTestRxEnDly\n");
        IDS_HDT_CONSOLE_DEBUG_CODE (
          IDS_HDT_CONSOLE (MEM_FLOW, "\t             Byte:  00  01  02  03  04  05  06  07  ECC\n");
          IDS_HDT_CONSOLE (MEM_FLOW, "\t    Save Err Stat: ");
          for (ByteLane = 0; ByteLane < MaxByteLanes; ByteLane++) {
            MemClkCycle = (UINT8) (PassTestRxEnDly[ByteLane] >> 5);
            IDS_HDT_CONSOLE (MEM_FLOW, " %c  ", ((RxEnMemClkSt[ByteLane][MemClkCycle] == TRUE) ? 'F' : 'P'));
          }
        );
      }
      // Update Saved values for "PassTestRxEnDly"
      for (ByteLane = 0; ByteLane < MaxByteLanes; ByteLane++) {
        if (RxEnDlyTargetFound[ByteLane] == FALSE) {
          if (OutOfRange[ByteLane] == FALSE) {
            MemClkCycle = (UINT8) (PassTestRxEnDly[ByteLane] >> 5);
            if (!RxEnMemClkTested[ByteLane][MemClkCycle]) {
              RxEnMemClkTested[ByteLane][MemClkCycle] = TRUE;
              RxEnMemClkSt[ByteLane][MemClkCycle] = TechPtr->ByteLaneError[ByteLane];
            }
          }
        }
      }
      //
      //  Find the RxEn Delay for the Fail condition in the Pass to Fail transition
      //              "FailTestRxEnDly"
      //
      for (ByteLane = 0; ByteLane < MaxByteLanes; ByteLane++) {
        DlyWrittenToReg[ByteLane] = FALSE;
      }
      for (ByteLane = 0; ByteLane < MaxByteLanes; ByteLane++) {
        FailTestRxEnDly[ByteLane] = PassTestRxEnDly[ByteLane] + 0x40;
      }
      IDS_HDT_CONSOLE_DEBUG_CODE (
        IDS_HDT_CONSOLE (MEM_FLOW, "\n\t             Byte:  00  01  02  03  04  05  06  07  ECC\n");
        IDS_HDT_CONSOLE (MEM_FLOW, "\t  FailTestRxEnDly: ");
        for (ByteLane = 0; ByteLane < MaxByteLanes; ByteLane++) {
          IDS_HDT_CONSOLE (MEM_FLOW, "%03x ", FailTestRxEnDly[ByteLane]);
        }
      );
      // Check if all BLs Saved Results at FailTestRxEnDly
      IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\tSetting FailTestRxEnDly");
      AllByteLanesSaved = 0;
      TotalByteLanesCheckedForSaved = 0;
      for (ByteLane = 0; ByteLane < MaxByteLanes; ByteLane++) {
        if (RxEnDlyTargetFound[ByteLane] == FALSE) {
          MemClkCycle = (UINT8) (FailTestRxEnDly[ByteLane] >> 5);
          // Check if RxEnDly + 40 is valid
          if ((FailTestRxEnDly[ByteLane] >= NBPtr->MinRxEnSeedGross) && (FailTestRxEnDly[ByteLane] <= NBPtr->MaxRxEnSeedTotal)) {
            if (RxEnMemClkTested[ByteLane][MemClkCycle]) {
              AllByteLanesSaved++;
            }
            OutOfRange[ByteLane] = FALSE;
          } else {
            OutOfRange[ByteLane] = TRUE;
          }
          TotalByteLanesCheckedForSaved++;
        }
      }
      // Check if all BLs out of Range condition at FailTestRxEnDly
      AllByteLanesOutOfRange = 0;
      IDS_HDT_CONSOLE (MEM_FLOW, "\n\t       OutOfRange: ");
      for (ByteLane = 0; ByteLane < MaxByteLanes; ByteLane++) {
        if (OutOfRange[ByteLane]) {
          AllByteLanesOutOfRange++;
        }
        IDS_HDT_CONSOLE (MEM_FLOW, " %c  ", (OutOfRange[ByteLane] == TRUE) ? 'Y' : 'N');
      }
      if (AllByteLanesOutOfRange == MaxByteLanes) {
        continue;  // All BLs out of range, so skip
      }
      // Setting FailTestRxEnDly for any BL that was not saved
      IDS_HDT_CONSOLE (MEM_FLOW, "\n\t  FailTestRxEnDly: ");
      for (ByteLane = 0; ByteLane < MaxByteLanes; ByteLane++) {
        if (RxEnDlyTargetFound[ByteLane] == FALSE) {
          MemClkCycle = (UINT8) (PassTestRxEnDly[ByteLane] >> 5);
          // Check if New RxEnDly has Passed
          if ((RxEnMemClkTested[ByteLane][MemClkCycle] ? RxEnMemClkSt[ByteLane][MemClkCycle] : TechPtr->ByteLaneError[ByteLane]) == FALSE) {
            if (OutOfRange[ByteLane] == FALSE) {
              // BL has passed at "New RxEnDly", so check if "New RxEnDly" + 0x40 fails
              MemClkCycle = (UINT8) (FailTestRxEnDly[ByteLane] >> 5);
              if (!RxEnMemClkTested[ByteLane][MemClkCycle]) {
                // Only Set Delays for ByteLanes that have not been already tested
                MemTRdPosRxEnSeedSetDly3 (TechPtr, FailTestRxEnDly[ByteLane], ByteLane);
                DlyWrittenToReg[ByteLane] = TRUE;
                IDS_HDT_CONSOLE (MEM_FLOW, " %c  ", 'Y');
              } else {
                IDS_HDT_CONSOLE (MEM_FLOW, " %c  ", 'N');
              }
              ByteLanePass[ByteLane] = TRUE;
            } else {
              IDS_HDT_CONSOLE (MEM_FLOW, " %c  ", 'O');
            }
          } else {
            IDS_HDT_CONSOLE (MEM_FLOW, " %c  ", 'F');
          }
        } else {
          IDS_HDT_CONSOLE (MEM_FLOW, " %c  ", 'N');
        }
      }
      // Check if BLs that passed at PassTestRxEnDly fail at FailTestRxEnDly
      if (AllByteLanesSaved != TotalByteLanesCheckedForSaved) {
          // At least one BL has not been saved, so check if FailTestRxEnDly passed
        IDS_HDT_CONSOLE (MEM_FLOW, "\n\n\t\t Checking if FailTestRxEnDly Fails?\n");
        MemTRdPosRxEnSeedCheckRxEndly3 (TechPtr);
        IDS_HDT_CONSOLE_DEBUG_CODE (
          IDS_HDT_CONSOLE (MEM_FLOW, "\n\t         Byte:  00  01  02  03  04  05  06  07  ECC\n");
          IDS_HDT_CONSOLE (MEM_FLOW, "\t     Err Status: ");
          for (ByteLane = 0; ByteLane < MaxByteLanes; ByteLane++) {
            IDS_HDT_CONSOLE (MEM_FLOW, " %c  ", (TechPtr->ByteLaneError[ByteLane] == TRUE) ? 'F' : 'P');
          }
        );
      } else {
        // All BLs saved, so use saved results for FailTestRxEnDly
        IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\tAll BLs Saved at PassTestRxEnDly\n");
        IDS_HDT_CONSOLE_DEBUG_CODE (
          IDS_HDT_CONSOLE (MEM_FLOW, "\t               Byte:  00  01  02  03  04  05  06  07  ECC\n");
          IDS_HDT_CONSOLE (MEM_FLOW, "\t      Save Err Stat: ");
          for (ByteLane = 0; ByteLane < MaxByteLanes; ByteLane++) {
            MemClkCycle = (UINT8) (FailTestRxEnDly[ByteLane] >> 5);
            IDS_HDT_CONSOLE (MEM_FLOW, " %c  ", (RxEnMemClkSt[ByteLane][MemClkCycle] == TRUE) ? 'F' : 'P');
          }
        );
      }
      //
      //  If BL failes at "FailTestRxEnDly" set FinalRxEnCycle
      //
      // Setting FinalRxEnCycle for any BL that Failed at FailTestRxEnDly
      IDS_HDT_CONSOLE (MEM_FLOW, "\n   Set FinalRxEnCycle: ");
      for (ByteLane = 0; ByteLane < MaxByteLanes; ByteLane++) {
        if (RxEnDlyTargetFound[ByteLane] == FALSE) {
          MemClkCycle = (UINT8) (FailTestRxEnDly[ByteLane] >> 5);
          if (RxEnMemClkTested[ByteLane][MemClkCycle] ? RxEnMemClkSt[ByteLane][MemClkCycle] == TRUE : (TechPtr->ByteLaneError[ByteLane] && DlyWrittenToReg[ByteLane])) {
            FinalRxEnCycle[ByteLane] = PassTestRxEnDly[ByteLane] - 0x10;
            if (((UINT16) FinalRxEnCycle[ByteLane] >= NBPtr->MinRxEnSeedGross) && ((UINT16) FinalRxEnCycle[ByteLane] <= NBPtr->MaxRxEnSeedTotal)) {
              // Since FailTestRxEnDly, we can set FinalRxEnCycle
              MemTRdPosRxEnSeedSetDly3 (TechPtr, (UINT16) FinalRxEnCycle[ByteLane], ByteLane);
              ByteLaneFail[ByteLane] = TRUE;
              OutOfRange[ByteLane] = FALSE;
              IDS_HDT_CONSOLE (MEM_FLOW, " %c  ", 'Y');
            } else {
              OutOfRange[ByteLane] = TRUE;
              IDS_HDT_CONSOLE (MEM_FLOW, " %c  ", 'N');
            }
          } else {
            IDS_HDT_CONSOLE (MEM_FLOW, " %c  ", 'F');
            OutOfRange[ByteLane] = FALSE;
          }
        } else {
          IDS_HDT_CONSOLE (MEM_FLOW, " %c  ", 'Y');
        }
      }
      // Update Saved values for FailTestRxEnDly
      for (ByteLane = 0; ByteLane < MaxByteLanes; ByteLane++) {
        if (RxEnDlyTargetFound[ByteLane] == FALSE) {
          if (OutOfRange[ByteLane] == FALSE) {
            MemClkCycle = (UINT8) (FailTestRxEnDly[ByteLane] >> 5);
            if (!RxEnMemClkTested[ByteLane][MemClkCycle] && DlyWrittenToReg[ByteLane]) {
              RxEnMemClkTested[ByteLane][MemClkCycle] = TRUE;
              RxEnMemClkSt[ByteLane][MemClkCycle] = TechPtr->ByteLaneError[ByteLane];
            }
          }
        }
      }
      // Check for out of Range condition
      AllByteLanesOutOfRange = 0;
      IDS_HDT_CONSOLE (MEM_FLOW, "\n\t       OutOfRange: ");
      for (ByteLane = 0; ByteLane < MaxByteLanes; ByteLane++) {
        if (OutOfRange[ByteLane]) {
          AllByteLanesOutOfRange++;
        }
        IDS_HDT_CONSOLE (MEM_FLOW, " %c  ", (OutOfRange[ByteLane] == TRUE) ? 'Y' : 'N');
      }
      if (AllByteLanesOutOfRange == MaxByteLanes) {
        continue; // All BLs out of range so skip
      }
      IDS_HDT_CONSOLE_DEBUG_CODE (
        IDS_HDT_CONSOLE (MEM_FLOW, "\n     FinalRxEnCycle: ");
        for (ByteLane = 0; ByteLane < MaxByteLanes; ByteLane++) {
          IDS_HDT_CONSOLE (MEM_FLOW, "%03x ", (UINT16) FinalRxEnCycle[ByteLane]);
        }
        IDS_HDT_CONSOLE (MEM_FLOW, "\n       ByteLaneFail: ");
        for (ByteLane = 0; ByteLane < MaxByteLanes; ByteLane++) {
          IDS_HDT_CONSOLE (MEM_FLOW, " %c  ", (ByteLaneFail[ByteLane] == TRUE) ? 'Y' : 'N');
        }
        IDS_HDT_CONSOLE (MEM_FLOW, "\n       ByteLanePass: ");
        for (ByteLane = 0; ByteLane < MaxByteLanes; ByteLane++) {
          IDS_HDT_CONSOLE (MEM_FLOW, " %c  ", (ByteLanePass[ByteLane] == TRUE) ? 'Y' : 'N');
        }
      );
      //
      // Check for exit condition
      // PassTestRxEnDly = Pass and FailTestRxEnDly[ByteLane] = Fail
      // If found, use "FinalRxEnCycle" as final RxEnDly value
      //
      // 5. Process the array of results and determine a pass-to-fail transition.
      NumBLWithTargetFound = 0;
      for (ByteLane = 0; ByteLane < MaxByteLanes; ByteLane++) {
        if (RxEnDlyTargetFound[ByteLane] == FALSE) {
          // Check if the current BL has found its target
          if (ByteLanePass[ByteLane] == TRUE && ByteLaneFail[ByteLane] == TRUE) {
            RxEnDlyTargetFound[ByteLane] = TRUE;
            NumBLWithTargetFound++;
            RxEnDlyTargetValue[ByteLane] = FinalRxEnCycle[ByteLane];
          } else {
            RxEnDlyTargetFound[ByteLane] = FALSE;
          }
        } else {
          // BL has already failed and passed, so increment both flags
          NumBLWithTargetFound++;
        }
      }
      IDS_HDT_CONSOLE (MEM_FLOW, "\n");
      // Check for exit condition
      if (NumBLWithTargetFound == MaxByteLanes) {
        // Exit condition found, so setting new RDQS based on RxEn-0x10 \n\n
        IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\t\t Setting new RDQS based on FinalRxEnCycle \n\n");
        // 5 A. DqsRcvEnCycle = the total delay value of the pass result.
        //   B. Program D18F2x9C_x0000_00[2A:10]_dct[1:0][DqsRcvEnGrossDelay, DqsRcvEnFineDelay] =
        //   DqsRcvEnCycle - 0x10.
        NBPtr->RdDqsDlyRetrnStat = RDDQSDLY_RTN_NEEDED;
        MemTRdPosRxEnSeedCheckRxEndly3 (TechPtr);
        IDS_HDT_CONSOLE (MEM_FLOW, "\n");
        status = TRUE;
        break;
      } else {
        status = FALSE;
      }
    }
    // Check for exit condition
    if (NumBLWithTargetFound == MaxByteLanes) {
      status = TRUE;
      break;
    } else {
      status = FALSE;
    }
    IDS_HDT_CONSOLE (MEM_FLOW, "\n");
  }
  TechPtr->NBPtr->MCTPtr->ErrStatus[EsbNoDqsPos] = EsbNoDqsPosSave;
  if (i == MAX_POS_RX_EN_SEED_GROSS_RANGE) {
    TechPtr->NBPtr->MCTPtr->ErrStatus[EsbNoDqsPos] = TRUE;
  }

  // 6. Program D18F2x9C_x0D0F_0[F,8:0]30_dct[1:0][BlockRxDqsLock] = 0.
  NBPtr->SetBitField (NBPtr, BFBlockRxDqsLock, 0);
  IDS_HDT_CONSOLE (MEM_FLOW, "\nEnd HW RxEn Seedless training\n\n");
  return status;
}
