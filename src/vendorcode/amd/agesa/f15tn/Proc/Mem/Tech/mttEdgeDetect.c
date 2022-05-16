/* $NoKeywords:$ */
/**
 * @file
 *
 * mttEdgeDetect.c
 *
 * DQS R/W position training utilizing Data Eye Edge Detection for optimization
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Tech)
 * @e \$Revision: 63425 $ @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 **/
/*****************************************************************************
*
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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
#include "AdvancedApi.h"
#include "GeneralServices.h"
#include "Ids.h"
#include "heapManager.h"
#include "mm.h"
#include "mn.h"
#include "mu.h"
#include "mt.h"
#include "mport.h"
#include "mttEdgeDetect.h"
#include "OptionMemory.h"
#include "merrhdl.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_TECH_MTTEDGEDETECT_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */


#define LAST_DELAY    (-128)
#define INC_DELAY     1
#define DEC_DELAY     0



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

/**
 * Sweep Table For Byte Training without insertion delay
 *
*/
CONST DQS_POS_SWEEP_TABLE SweepTableByte[] =
{
  //     Begin  End   Inc/Dec  Step  EndResult Edge
  {      0x00,  0x1F, INC_DELAY,  4, 0xFFFF,   LEFT_EDGE},  /// For Left Edge, start from 0 and Increment to 0x1F by 4 until all PASS
  { LAST_DELAY, 0x00, DEC_DELAY, -1, 0xFE00,   LEFT_EDGE},  ///  Then go back down to 0x00 by 1 until all FAIL
  {      0x1F,  0x00, DEC_DELAY, -4, 0xFFFF,   RIGHT_EDGE}, /// For Right Edge, start from 0x1F down to 0 until all PASS.
  { LAST_DELAY, 0x1F, INC_DELAY,  1, 0xFE00,   RIGHT_EDGE}  ///  Then go back up by 1 until all FAIL.
};
/**
 * Sweep Table For Byte Training with insertion delay
 *
*/
CONST DQS_POS_SWEEP_TABLE InsSweepTableByte[] =
{
  //     Begin  End   Inc/Dec  Step  EndResult Edge
  {      0x00, -0x20, DEC_DELAY, -4, 0xFE00, LEFT_EDGE},  /// For Left Edge, start from 0 and Decrement to -0x20 by -4 until all FAIL
  { LAST_DELAY, 0x1F, INC_DELAY,  1, 0xFFFF, LEFT_EDGE},  ///  Then go back up to 0x1F by 1 until all PASS
  {      0x1F,  0x00, DEC_DELAY, -4, 0xFFFF, RIGHT_EDGE}, /// For Right Edge, start from 0x1F down to 0 until all PASS.
  { LAST_DELAY, 0x1F, INC_DELAY,  1, 0xFE00, RIGHT_EDGE}  ///  Then go back up by 1 until all FAIL.
};

BOOLEAN
STATIC
MemTTrainDQSRdWrEdgeDetect (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

BOOLEAN
STATIC
MemTInitTestPatternAddress (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   SWEEP_INFO *SweepPtr
  );

BOOLEAN
STATIC
MemTContinueSweep (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   SWEEP_INFO *SweepPtr
  );

BOOLEAN
STATIC
MemTSetNextDelay (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   SWEEP_INFO *SweepPtr
  );

UINT8
STATIC
MemTScaleDelayVal (
  IN OUT MEM_TECH_BLOCK *TechPtr,
  IN INT8 Delay
  );

BOOLEAN
STATIC
MemTDataEyeSave (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   SWEEP_INFO *SweepPtr,
  IN       UINT8 ByteLane
  );

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
extern MEM_FEAT_TRAIN_SEQ memTrainSequenceDDR3[];
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function executes DQS position training for all a Memory channel using
 *     the Edge Detection algorithm.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

BOOLEAN
MemTTrainDQSEdgeDetectSw (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  MEM_NB_BLOCK *NBPtr;
  BOOLEAN Status;

  Status = FALSE;
  NBPtr = TechPtr->NBPtr;
  TechPtr->TrainingType = TRN_DQS_POSITION;
  //
  // Initialize the Pattern
  //
  if (AGESA_SUCCESS == NBPtr->TrainingPatternInit (NBPtr)) {
    //
    // Setup hardware training engine (if applicable)
    //
    NBPtr->FamilySpecificHook[SetupHwTrainingEngine] (NBPtr, &TechPtr->TrainingType);
    //
    // Start Edge Detection
    //
    Status |= MemTTrainDQSRdWrEdgeDetect (TechPtr);
    //
    // Finalize the Pattern
    //
    Status &= (AGESA_SUCCESS == NBPtr->TrainingPatternFinalize (NBPtr));
  }
  return Status;
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This Executes Read DQS and Write Data Position training on a chip select pair
 *  using the Edge Detection algorithm.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  TRUE - No Errors occurred
 *     @return  FALSE - Errors occurred

 */

BOOLEAN
STATIC
MemTTrainDQSRdWrEdgeDetect (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  MEM_DATA_STRUCT *MemPtr;
  MEM_NB_BLOCK  *NBPtr;
  UINT8 WrDqDelay;
  UINT8 Dct;
  UINT8 ChipSel;
  UINT8 i;
  BOOLEAN Status;
  UINT8 TimesFail;
  UINT8 TimesRetrain;

  NBPtr = TechPtr->NBPtr;
  MemPtr = NBPtr->MemPtr;
  TimesRetrain = DEFAULT_TRAINING_TIMES;
  IDS_OPTION_HOOK (IDS_MEM_RETRAIN_TIMES, &TimesRetrain, &MemPtr->StdHeader);
  //
  // Set environment settings before training
  //
  IDS_HDT_CONSOLE (MEM_STATUS, "\nStart Read/Write Data Eye Edge Detection.\n");
  MemTBeginTraining (TechPtr);
  //
  // Do Rd DQS /Wr Data Position training for all Dcts/Chipselects
  //
  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    IDS_HDT_CONSOLE (MEM_STATUS, "\tDct %d\n", Dct);
    NBPtr->SwitchDCT (NBPtr, Dct);
    //
    // Chip Select Loop
    //
    TechPtr->RestartChipSel = -1;
    for (ChipSel = 0; ChipSel < NBPtr->CsPerChannel; ChipSel = ChipSel + NBPtr->CsPerDelay ) {
      //
      // Init Bit Error Masks
      //
      LibAmdMemFill (&NBPtr->ChannelPtr->FailingBitMask[ (ChipSel * MAX_BYTELANES_PER_CHANNEL) ],
        0xFF,
        (MAX_BYTELANES_PER_CHANNEL * NBPtr->CsPerDelay),
        &MemPtr->StdHeader);
      if ( (NBPtr->MCTPtr->Status[SbLrdimms]) ? ((NBPtr->ChannelPtr->LrDimmPresent & ((UINT8) 1 << (ChipSel >> 1))) != 0) :
           ((NBPtr->DCTPtr->Timings.CsEnabled & ((UINT16) 1 << ChipSel)) != 0) ) {

        TechPtr->ChipSel = ChipSel;
        IDS_HDT_CONSOLE (MEM_STATUS, "\t\tCS %d\n", ChipSel);
        IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tIncrease WrDat, Train RdDqs:\n");

        TechPtr->DqsRdWrPosSaved = 0;
        //
        // Use a list of Approximate Write Data delay values and train Read DQS Position for
        // each until a valid Data eye is found.
        //
        Status = FALSE;
        TimesFail = 0;
        NBPtr->FamilySpecificHook[InitializeRxEnSeedlessTraining] (NBPtr, NBPtr);
        ERROR_HANDLE_RETRAIN_BEGIN (TimesFail, TimesRetrain) {
          i = 0;
          while (NBPtr->GetApproximateWriteDatDelay (NBPtr, i, &WrDqDelay)) {
            TechPtr->SmallDqsPosWindow = FALSE;
            //
            // Set Write Delay approximation
            //
            TechPtr->Direction = DQS_WRITE_DIR;
            IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\t\tWrite Delay: %02x", WrDqDelay);
            MemTSetDQSDelayAllCSR (TechPtr, WrDqDelay);
            //
            // Attempt Read Training
            //
            TechPtr->Direction = DQS_READ_DIR;
            Status = memTrainSequenceDDR3[NBPtr->TrainingSequenceIndex].MemTechFeatBlock->RdPosTraining (TechPtr);
            if (Status) {
              //
              // If Read DQS Training was successful, Train Write Data (DQ) Position
              //
              TechPtr->DqsRdWrPosSaved = 0;
              IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\t\tTrain WrDat:\n\n");
              TechPtr->Direction = DQS_WRITE_DIR;
              if (NBPtr->FamilySpecificHook[BeforeWrDatTrn] (NBPtr, &ChipSel)) {
                Status = MemTTrainDQSEdgeDetect (TechPtr);
              }
              break;
            }
            i++;
          }
        ERROR_HANDLE_RETRAIN_END ((Status == FALSE), TimesFail)
        }

        //
        // If we went through the table, Fail.
        //
        if (Status == FALSE) {
          // On training failure, check and record whether training fails due to small window or no window
          if (TechPtr->SmallDqsPosWindow) {
            NBPtr->MCTPtr->ErrStatus[EsbSmallDqs] = TRUE;
          } else {
            NBPtr->MCTPtr->ErrStatus[EsbNoDqsPos] = TRUE;
          }

          SetMemError (AGESA_ERROR, NBPtr->MCTPtr);
          if (TechPtr->Direction == DQS_READ_DIR) {
            PutEventLog (AGESA_ERROR, MEM_ERROR_NO_DQS_POS_RD_WINDOW, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
          } else {
            PutEventLog (AGESA_ERROR, MEM_ERROR_NO_DQS_POS_WR_WINDOW, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
          }
          NBPtr->DCTPtr->Timings.CsTrainFail |= (UINT16)1 << ChipSel;
          // If the even chip select failed training always fail the odd, if present.
          if (((ChipSel & 0x01) == 0) && (NBPtr->CsPerDelay == 2)) {
            if (NBPtr->DCTPtr->Timings.CsPresent & ((UINT16)1 << (ChipSel + 1))) {
              NBPtr->DCTPtr->Timings.CsTrainFail |= (UINT16)1 << (ChipSel + 1);
            }
          }
          if (!NBPtr->MemPtr->ErrorHandling (NBPtr->MCTPtr, NBPtr->Dct, NBPtr->DCTPtr->Timings.CsTrainFail, &NBPtr->MemPtr->StdHeader)) {
            ASSERT (FALSE);
          }
        }
      } else {
      //
      // Clear Bit Error Masks if these CS will not be trained.
      //
      LibAmdMemFill (&NBPtr->ChannelPtr->FailingBitMask[ (ChipSel * MAX_BYTELANES_PER_CHANNEL) ],
        0x00,
        (MAX_BYTELANES_PER_CHANNEL * NBPtr->CsPerDelay),
        &NBPtr->MemPtr->StdHeader);
      }
    }
  }
  //
  // Restore environment settings after training
  //
  MemTEndTraining (TechPtr);
  IDS_HDT_CONSOLE (MEM_FLOW, "End Read/Write Data Eye Edge Detection\n\n");
  return (BOOLEAN) (NBPtr->MCTPtr->ErrCode < AGESA_FATAL);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function executes DQS position training for both read and write, using
 *     the Edge Detection Algorithm.  This method searches for the beginning and end
 *     of the Data Eye with out scanning every DSQ delay value. The following is a
 *     detailed description of the algorithm:
 *
 *     Four-Stage Data Eye Sweep
 *
 *     -Search starts at Delay value of 0.
 *     -Search left in steps of 4/32UI looking for all Byte lanes Passing. Left from zero rolls over to a negative value.
 *     -Negative values are translated to the high end of the delay range, but using Insertion delay comparison.
 *     -For each passing byte lane, freeze delay at first passing value, but set mask so next steps will not compare for byte lanes that previously passed
 *     -Switch to search right in steps of 1/32UI looking for fail.
 *     -For each lane, starting delay for 1/32 sweep right is first passing delay from 4/32 sweep left.
 *     -For each failing byte lane, freeze delay at first failing value, but set mask so next steps will not compare for byte lanes that previously failed
 *     -Search right until all byte lanes have failed
 *     -For each lane, right edge used by BIOS will be first failing delay value minus 1/32

 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  TRUE - All bytelanes pass
 *     @return  FALSE - Some bytelanes fail
*/
BOOLEAN
MemTTrainDQSEdgeDetect (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  MEM_NB_BLOCK         *NBPtr;
  DIE_STRUCT           *MCTPtr;
  CONST DQS_POS_SWEEP_TABLE  *SweepTablePtr;
  UINT8                SweepTableSize;
  SWEEP_INFO           SweepData;
  BOOLEAN              Status;
  UINT16               CurrentResult;
  UINT16               AlignedResult;
  UINT16               OffsetResult;
  UINT8                StageIndex;
  UINT8                CsIndex;
  UINT8                i;

  Status = TRUE;
  //
  // Initialize Object Pointers
  //
  NBPtr = TechPtr->NBPtr;
  MCTPtr = NBPtr->MCTPtr;
  //
  // Initialize stack variables
  //
  LibAmdMemFill (&SweepData, 0, sizeof (SWEEP_INFO), &NBPtr->MemPtr->StdHeader);
  //
  /// Get Pointer to Sweep Table
  //
  if (TechPtr->Direction == DQS_READ_DIR) {
    SweepTablePtr = InsSweepTableByte;
    SweepTableSize = GET_SIZE_OF (InsSweepTableByte);
  } else {
    SweepTablePtr = SweepTableByte;
    SweepTableSize = GET_SIZE_OF (SweepTableByte);
  }

  //
  /// Set up the test Pattern, exit if no Memory
  //
  if (MemTInitTestPatternAddress (TechPtr, &SweepData) == FALSE) {
    LibAmdMemFill (&NBPtr->ChannelPtr->FailingBitMask[ (TechPtr->ChipSel * MAX_BYTELANES_PER_CHANNEL) ],
      0,
      (MAX_BYTELANES_PER_CHANNEL * NBPtr->CsPerDelay),
      &NBPtr->MemPtr->StdHeader);
    return FALSE;
  }
  //
  // Clear Error Flag
  //
  SweepData.Error = FALSE;
  NBPtr->FamilySpecificHook[InitialzeRxEnSeedlessByteLaneError] (NBPtr, NBPtr);
  //
  /// Process Sweep table, using entries from the table to determine Starting and Ending Delays
  /// as well as the Step size and criteria for evaluating whether the correct result is found.
  ///
  /// Delay values at this level are an abstract range of values which gets scaled to the actual value
  /// before it is written to the hardware. This allows NB specific code to handle the scaling as a
  ///  function of frequency or other conditions.
  //
  for (StageIndex = 0; (StageIndex < SweepTableSize) && (SweepData.Error == FALSE); StageIndex++) {

    IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tSTAGE: %d\t", StageIndex);
    //
    /// Initialize SweepData variables
    //
    SweepData.BeginDelay = SweepTablePtr->BeginDelay;
    SweepData.EndDelay = SweepTablePtr->EndDelay;
    SweepData.Step = 0;                                    /// Step Value will be 0 to start.
    SweepData.EndResult = SweepTablePtr->EndResult;
    if (!(MCTPtr->Status[SbEccDimms] && NBPtr->IsSupported[EccByteTraining])) {
      SweepData.EndResult |= 0x0100;
    }
    SweepData.Edge = SweepTablePtr->MinMax;
    SweepData.InsertionDelayMsk = 0;
    SweepData.ResultFound = 0x0000;
    //
    // Set Training Delays Pointer.
    //
    if (TechPtr->Direction == DQS_READ_DIR) {
      SweepData.TrnDelays = (INT8 *) ((SweepData.Edge == RIGHT_EDGE) ? NBPtr->ChannelPtr->RdDqsMaxDlys : NBPtr->ChannelPtr->RdDqsMinDlys);
    } else {
      SweepData.TrnDelays = (INT8 *) ((SweepData.Edge == RIGHT_EDGE) ? NBPtr->ChannelPtr->WrDatMaxDlys : NBPtr->ChannelPtr->WrDatMinDlys);
    };
    //
    /// Set initial TrnDelay Values if necessary
    //
    IDS_HDT_CONSOLE (MEM_FLOW, "Sweeping %s DQS, %s from ", (TechPtr->Direction == DQS_READ_DIR) ?"Read":"Write", (SweepTablePtr->ScanDir == INC_DELAY) ? "incrementing":"decrementing");
    if (SweepData.BeginDelay != LAST_DELAY) {
      IDS_HDT_CONSOLE (MEM_FLOW, "%02x", (UINT16) MemTScaleDelayVal (TechPtr, SweepData.BeginDelay));
      for (i = 0; i < ((MCTPtr->Status[SbEccDimms] && NBPtr->IsSupported[EccByteTraining]) ? 9 : 8); i++) {
        SweepData.TrnDelays[i] = SweepData.BeginDelay;
      }
    } else {
      IDS_HDT_CONSOLE (MEM_FLOW, "Current Delay");
      SweepData.Step = SweepTablePtr->Step;
    }
    IDS_HDT_CONSOLE (MEM_FLOW, " by %02x, until all bytelanes %s.\n\n", (UINT16) MemTScaleDelayVal (TechPtr, ABS (SweepTablePtr->Step)), (SweepData.EndResult == 0xFFFF)?"PASS":"FAIL");

    //-------------------------------------------------------------------
    //   Sweep DQS Delays
    //   MemTContinueSweep function returns false to break out of loop.
    //   There are no other breaks out of this loop.
    //-------------------------------------------------------------------
    while (MemTContinueSweep (TechPtr, &SweepData)) {
      IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\t\tByte Lane    : 08 07 06 05 04 03 02 01 00\n");
      IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\t\tDQS Delays   : %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
      (UINT16) MemTScaleDelayVal (TechPtr, SweepData.TrnDelays[8]),
      (UINT16) MemTScaleDelayVal (TechPtr, SweepData.TrnDelays[7]), (UINT16) MemTScaleDelayVal (TechPtr, SweepData.TrnDelays[6]),
      (UINT16) MemTScaleDelayVal (TechPtr, SweepData.TrnDelays[5]), (UINT16) MemTScaleDelayVal (TechPtr, SweepData.TrnDelays[4]),
      (UINT16) MemTScaleDelayVal (TechPtr, SweepData.TrnDelays[3]), (UINT16) MemTScaleDelayVal (TechPtr, SweepData.TrnDelays[2]),
      (UINT16) MemTScaleDelayVal (TechPtr, SweepData.TrnDelays[1]), (UINT16) MemTScaleDelayVal (TechPtr, SweepData.TrnDelays[0])
       );
      //
      /// Set Step Value
      //
      SweepData.Step = SweepTablePtr->Step;
      CurrentResult = 0xFFFF;
      //
      /// Chip Select Loop: Test the Pattern for all populated CS that are controlled by the current delay registers
      //
      for (CsIndex = 0; CsIndex < NBPtr->CsPerDelay ; CsIndex++, TechPtr->ChipSel++) {
        ASSERT (CsIndex < MAX_CS_PER_CHANNEL);
        ASSERT (TechPtr->ChipSel < MAX_CS_PER_CHANNEL);
        if (SweepData.CsAddrValid[CsIndex] == TRUE) {
          //
          /// If this is a Write Dqs sweep, Write the pattern now.
          //
          if (TechPtr->Direction == DQS_WRITE_DIR) {
            NBPtr->WritePattern (NBPtr, SweepData.TestAddrRJ16[CsIndex], TechPtr->PatternBufPtr, TechPtr->PatternLength);
          }
          //
          /// Read the Pattern Back
          //
          NBPtr->ReadPattern (NBPtr, TechPtr->TestBufPtr, SweepData.TestAddrRJ16[CsIndex], TechPtr->PatternLength);
          //
          /// Compare the Pattern and Merge the results using InsertionDelayMsk
          //
          AlignedResult = NBPtr->CompareTestPattern (NBPtr, TechPtr->TestBufPtr, TechPtr->PatternBufPtr, TechPtr->PatternLength * 64);
          CurrentResult &= AlignedResult | SweepData.InsertionDelayMsk;
          if (SweepData.InsertionDelayMsk != 0) {
            OffsetResult = NBPtr->InsDlyCompareTestPattern (NBPtr, TechPtr->TestBufPtr, TechPtr->PatternBufPtr, TechPtr->PatternLength * 64);
            CurrentResult &= (OffsetResult | (~SweepData.InsertionDelayMsk));
          }
          //
          /// Flush the Test Pattern
          //
          NBPtr->FlushPattern (NBPtr, SweepData.TestAddrRJ16[CsIndex], TechPtr->PatternLength);
          NBPtr->FamilySpecificHook[ResetRxFifoPtr] (NBPtr, NBPtr);
        }
      } /// End Chip Select Loop
      TechPtr->ChipSel = TechPtr->ChipSel - CsIndex;
      IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\t\tResult       :  %c  %c  %c  %c  %c  %c  %c  %c  %c \n",
        (SweepData.ResultFound & ((UINT16) 1 << (8))) ? ' ':(CurrentResult & ((UINT16) 1 << (8))) ? 'P':'.',
        (SweepData.ResultFound & ((UINT16) 1 << (7))) ? ' ':(CurrentResult & ((UINT16) 1 << (7))) ? 'P':'.',
        (SweepData.ResultFound & ((UINT16) 1 << (6))) ? ' ':(CurrentResult & ((UINT16) 1 << (6))) ? 'P':'.',
        (SweepData.ResultFound & ((UINT16) 1 << (5))) ? ' ':(CurrentResult & ((UINT16) 1 << (5))) ? 'P':'.',
        (SweepData.ResultFound & ((UINT16) 1 << (4))) ? ' ':(CurrentResult & ((UINT16) 1 << (4))) ? 'P':'.',
        (SweepData.ResultFound & ((UINT16) 1 << (3))) ? ' ':(CurrentResult & ((UINT16) 1 << (3))) ? 'P':'.',
        (SweepData.ResultFound & ((UINT16) 1 << (2))) ? ' ':(CurrentResult & ((UINT16) 1 << (2))) ? 'P':'.',
        (SweepData.ResultFound & ((UINT16) 1 << (1))) ? ' ':(CurrentResult & ((UINT16) 1 << (1))) ? 'P':'.',
        (SweepData.ResultFound & ((UINT16) 1 << (0))) ? ' ':(CurrentResult & ((UINT16) 1 << (0))) ? 'P':'.'
        );
      //
      /// Merge current result into cumulative result and make it positive.
      //
      SweepData.ResultFound |= ~(CurrentResult ^ SweepData.EndResult);

      IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\t\tResultFound  :  %c  %c  %c  %c  %c  %c  %c  %c  %c \n\n",
        (SweepData.ResultFound & ((UINT16) 1 << (8))) ? 'Y':' ',
        (SweepData.ResultFound & ((UINT16) 1 << (7))) ? 'Y':' ',
        (SweepData.ResultFound & ((UINT16) 1 << (6))) ? 'Y':' ',
        (SweepData.ResultFound & ((UINT16) 1 << (5))) ? 'Y':' ',
        (SweepData.ResultFound & ((UINT16) 1 << (4))) ? 'Y':' ',
        (SweepData.ResultFound & ((UINT16) 1 << (3))) ? 'Y':' ',
        (SweepData.ResultFound & ((UINT16) 1 << (2))) ? 'Y':' ',
        (SweepData.ResultFound & ((UINT16) 1 << (1))) ? 'Y':' ',
        (SweepData.ResultFound & ((UINT16) 1 << (0))) ? 'Y':' '
        );
    } /// End of Delay Sweep
    //
    /// Place Final delay values at last passing delay.
    //
    if (SweepData.ResultFound == 0xFFFF) {
      if ( ABS (SweepData.Step) == 1) {
        for (i = 0; i < ((MCTPtr->Status[SbEccDimms] && NBPtr->IsSupported[EccByteTraining]) ? 9 : 8) ; i++) {
          if ((SweepData.EndResult & ((UINT16) (1 << i))) == 0) {
            SweepData.TrnDelays[i] = SweepData.TrnDelays[i] - SweepData.Step;
          }
        }
      }
    }
    //
    // Update Pointer to Sweep Table
    //
    SweepTablePtr++;
  }///End of Edge Detect loop
  //
  /// If No Errors are detected, Calculate Data Eye Width and Center
  //
  if (SweepData.Error == FALSE) {
    IDS_HDT_CONSOLE (MEM_FLOW, "\t\tData Eye Results:\n\n");
    IDS_HDT_CONSOLE (MEM_FLOW, "\t\tByte     Left     Right\n");
    IDS_HDT_CONSOLE (MEM_FLOW, "\t\tLane     Edge     Edge     Width     Center\n");
    for (i = 0; i < ((MCTPtr->Status[SbEccDimms] && NBPtr->IsSupported[EccByteTraining]) ? 9 : 8) ; i++) {
      IDS_HDT_CONSOLE (MEM_FLOW, "\t\t %0d", i);
      TechPtr->Bytelane = i;
      if (!MemTDataEyeSave (TechPtr, &SweepData, i)) {
        break;
      }
      IDS_HDT_CONSOLE (MEM_FLOW, "\n");
      if (SweepData.Error == TRUE) {
        Status = FALSE;
      }
    }
  } else {
    Status = FALSE;
    IDS_HDT_CONSOLE (MEM_FLOW, "\t\t--DATA EYE NOT FOUND--\n\n");
    NBPtr->FamilySpecificHook[TrackRxEnSeedlessRdWrNoWindBLError] (NBPtr, &SweepData);
  }
  return Status;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     Initialize the Test Pattern Address for two chip selects and, if this
 *     is a Write Data Eye, write the initial test pattern.
 *
 *     Test Address is stored in the Sweep info struct.  If Memory is not present
 *     then return with False.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in,out]   *SweepPtr -  Pointer to SWEEP_INFO structure.
 *
 *     @return  BOOLEAN
 *              TRUE  - Memory is present
 *              FALSE - No memory present on this Chip Select pair.
 *
**
 */
BOOLEAN
STATIC
MemTInitTestPatternAddress (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   SWEEP_INFO *SweepPtr
  )
{
  MEM_NB_BLOCK  *NBPtr;
  UINT8 ChipSel;
  UINT8 CsIndex;
  BOOLEAN BanksPresent;

  NBPtr = TechPtr->NBPtr;
  BanksPresent = FALSE;
  ChipSel = TechPtr->ChipSel;
  for (CsIndex = 0; CsIndex < NBPtr->CsPerDelay; ChipSel++, CsIndex++, TechPtr->ChipSel++) {
    ASSERT (CsIndex < MAX_CS_PER_CHANNEL);
    ASSERT (ChipSel < MAX_CS_PER_CHANNEL);
    ASSERT (TechPtr->ChipSel < MAX_CS_PER_CHANNEL);
    //
    /// If memory is present on this cs, get the test addr
    //
    if (NBPtr->GetSysAddr (NBPtr, ChipSel, &(SweepPtr->TestAddrRJ16[CsIndex]))) {
      if (!(NBPtr->MCTPtr->Status[SbLrdimms]) || ((NBPtr->ChannelPtr->LrDimmPresent & ((UINT8) 1 << (ChipSel >> 1))) != 0)) {
        BanksPresent = TRUE;
        SweepPtr->CsAddrValid[CsIndex] = TRUE;
        //
        /// If this is a Read Dqs sweep, Write the pattern now.
        //
        if (TechPtr->Direction == DQS_READ_DIR) {
          IDS_HDT_CONSOLE (MEM_FLOW, "\tTestAddr: %x0000\n", SweepPtr->TestAddrRJ16[CsIndex]);
          NBPtr->WritePattern (NBPtr, SweepPtr->TestAddrRJ16[CsIndex], TechPtr->PatternBufPtr, TechPtr->PatternLength);
        }
      }
    } else {
      SweepPtr->CsAddrValid[CsIndex] = FALSE;
    }
  } /// End Chip Select Loop
  TechPtr->ChipSel = TechPtr->ChipSel - CsIndex;
  //
  /// return FALSE if no ChipSelects present.
  //
  return BanksPresent;
}

/* -----------------------------------------------------------------------------*/
/**
 *     Test Conditions for exiting the training loop, set the next delay value,
 *     and return status
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in,out]   *SweepPtr -  Pointer to SWEEP_INFO structure.
 *
 *     @return  BOOLEAN
 *              TRUE  - Continue to test with next delay setting
 *              FALSE - Exit training loop.  Either the result has been found or
 *                      end of delay range has been reached.
*/
BOOLEAN
STATIC
MemTContinueSweep (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   SWEEP_INFO *SweepPtr
  )
{
  BOOLEAN Status;
  Status = FALSE;
  if (SweepPtr->ResultFound != 0xFFFF) {
    Status = MemTSetNextDelay (TechPtr, SweepPtr);
  }
  TechPtr->NBPtr->FamilySpecificHook[RegAccessFence] (TechPtr->NBPtr, NULL);
  return Status;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function sets the next delay value for each bytelane that needs to
 *      be advanced.  It checks the bounds of the delay to see if we are at the
 *      end of the range.  If we are to close to advance a whole step value, but
 *      not at the boundary, then we set the delay to the boundary.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in,out]   *SweepPtr -  Pointer to SWEEP_INFO structure.
 *
 */

BOOLEAN
STATIC
MemTSetNextDelay (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   SWEEP_INFO *SweepPtr
  )
{
  DIE_STRUCT           *MCTPtr;
  UINT8 i;

  MCTPtr =  TechPtr->NBPtr->MCTPtr;
  //
  ///< Loop through bytelanes
  //
  for (i = 0; i < ((MCTPtr->Status[SbEccDimms] && TechPtr->NBPtr->IsSupported[EccByteTraining]) ? 9 : 8) ; i++) {
    //
    /// Skip Bytelanes that have already reached the desired result
    //
    if ( (SweepPtr->ResultFound  & ((UINT16)1 << i)) == 0) {
      //
      /// If a bytelane has reached the end, flag an error and exit
      //
      if (SweepPtr->TrnDelays[i] == SweepPtr->EndDelay) {
        if ((SweepPtr->EndResult & ((UINT16) (1 << i))) != 0) {
          MCTPtr->ErrStatus[EsbNoDqsPos] = TRUE;
          SweepPtr->Error = TRUE;
        }
        return FALSE;
      }
      //
      /// If the Current delay value is less than a step away from EndDelay,
      //
      if ( ABS (SweepPtr->EndDelay - SweepPtr->TrnDelays[i]) < ABS (SweepPtr->Step)) {
        /// set to EndDelay.
        //
        SweepPtr->TrnDelays[i] = SweepPtr->EndDelay;
      } else {
        //
        /// Otherwise, add the step value to it
        SweepPtr->TrnDelays[i] = SweepPtr->TrnDelays[i] + SweepPtr->Step;
      }
      //
      /// Set InsertionDelayMsk bit if Delay < 0 for this bytelane
      //
      if (SweepPtr->TrnDelays[i] < 0) {
        SweepPtr->InsertionDelayMsk |= ((UINT16) 1 << i);
      } else {
        SweepPtr->InsertionDelayMsk &= ~((UINT16) 1 << i);
      }
      //
      /// Write the scaled value to the Delay Register
      //
      TechPtr->SetDQSDelayCSR (TechPtr, i, MemTScaleDelayVal (TechPtr, SweepPtr->TrnDelays[i]));
    }
  }
  return TRUE;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function accepts a delay value in 32nd of a UI and converts it to an
 *     actual register value, taking into consideration NB type, rd/wr,
 *     and frequency.
 *
 *     Delay = (Min + (Delay * ( (Max - Min) / TRN_DELAY_MAX) )) & Mask
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]       *Delay     - INT8 of delay value;
 *
 *     @return  UINT8 of the adjusted delay value
*/
UINT8
STATIC
MemTScaleDelayVal (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       INT8 Delay
  )
{
  MEM_NB_BLOCK  *NBPtr;
  TRN_DLY_PARMS Parms;
  TRN_DLY_TYPE DelayType;
  UINT8 NewDelay;
  INT8 Factor;
  INT8 ScaledDelay;

  NBPtr = TechPtr->NBPtr;
  //
  // Determine Delay Type, Get Delay Parameters, and return scaled Delay value
  //
  DelayType = (TechPtr->Direction == DQS_WRITE_DIR) ? AccessWrDatDly : AccessRdDqsDly;
  NBPtr->GetTrainDlyParms (NBPtr, DelayType, &Parms);
  Factor = ((Parms.Max - Parms.Min) / TRN_DELAY_MAX);
  ScaledDelay = Delay * Factor;
  NewDelay = (Parms.Min + ScaledDelay) & Parms.Mask;
  return NewDelay;
}





/* -----------------------------------------------------------------------------*/
/**
 *
 *       This function calculates the Center of the Data eye for the specified byte lane
 *       and stores its DQS Delay value for reference.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in,out]   *SweepPtr -  Pointer to SWEEP_INFO structure.
 *     @param[in]       ByteLane  -  Bytelane number being targeted
 *
 */
BOOLEAN
STATIC
MemTDataEyeSave (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   SWEEP_INFO *SweepPtr,
  IN       UINT8 ByteLane
  )
{
  MEM_NB_BLOCK *NBPtr;
  UINT8 EyeCenter;
  UINT8 DlyMin;
  UINT8 DlyMax;
  UINT8 EyeWidth;
  UINT8 Dimm;
  CH_DEF_STRUCT *ChanPtr;

  NBPtr = TechPtr->NBPtr;
  ChanPtr = NBPtr->ChannelPtr;

  ASSERT (ByteLane < ((NBPtr->MCTPtr->Status[SbEccDimms] && NBPtr->IsSupported[EccByteTraining]) ? 9 : 8));
  //
  // Calculate Data Eye edges, Width, and Center in real terms.
  //
  if (TechPtr->Direction == DQS_READ_DIR) {
    DlyMin = MemTScaleDelayVal (TechPtr, ChanPtr->RdDqsMinDlys[ByteLane]);
    DlyMax = MemTScaleDelayVal (TechPtr, ChanPtr->RdDqsMaxDlys[ByteLane]);
    EyeWidth = MemTScaleDelayVal (TechPtr, (ChanPtr->RdDqsMaxDlys[ByteLane] - ChanPtr->RdDqsMinDlys[ByteLane]));
    EyeCenter = MemTScaleDelayVal (TechPtr, ((ChanPtr->RdDqsMinDlys[ByteLane] + ChanPtr->RdDqsMaxDlys[ByteLane] + 1) / 2));
    if (!NBPtr->FamilySpecificHook[RdDqsDlyRestartChk] (NBPtr, &EyeCenter)) {
      return FALSE;
    }
    ChanPtr->RdDqsMinDlys[ByteLane] = DlyMin;
    ChanPtr->RdDqsMaxDlys[ByteLane] = DlyMax;
    NBPtr->FamilySpecificHook[ForceRdDqsPhaseB] (NBPtr, &EyeCenter);
  } else {
    DlyMin = MemTScaleDelayVal (TechPtr, ChanPtr->WrDatMinDlys[ByteLane]);
    DlyMax = MemTScaleDelayVal (TechPtr, ChanPtr->WrDatMaxDlys[ByteLane]);
    EyeWidth = MemTScaleDelayVal (TechPtr, (ChanPtr->WrDatMaxDlys[ByteLane] - ChanPtr->WrDatMinDlys[ByteLane]));
    EyeCenter = MemTScaleDelayVal (TechPtr, ((ChanPtr->WrDatMinDlys[ByteLane] + ChanPtr->WrDatMaxDlys[ByteLane] + 1) / 2));
    ChanPtr->WrDatMinDlys[ByteLane] = DlyMin;
    ChanPtr->WrDatMaxDlys[ByteLane] = DlyMax;
  }
  //
  // Flag error for small window.
  //
  if (EyeWidth < MemTScaleDelayVal (TechPtr, NBPtr->MinDataEyeWidth (NBPtr))) {
    TechPtr->SmallDqsPosWindow = TRUE;
    SweepPtr->Error = TRUE;
    NBPtr->FamilySpecificHook[TrackRxEnSeedlessRdWrSmallWindBLError] (NBPtr, NULL);
  }

  IDS_HDT_CONSOLE (MEM_FLOW, "        %02x       %02x       %02x         %02x", DlyMin, DlyMax, EyeWidth, EyeCenter);

  TechPtr->SetDQSDelayCSR (TechPtr, ByteLane, EyeCenter);
  if (!SweepPtr->Error) {
    TechPtr->DqsRdWrPosSaved |= (UINT8)1 << ByteLane;
  }
  TechPtr->DqsRdWrPosSaved |= 0xFE00;

  Dimm = (TechPtr->ChipSel / NBPtr->CsPerDelay) * TechPtr->DlyTableWidth () + ByteLane;
  if (TechPtr->Direction == DQS_READ_DIR) {
    ChanPtr->RdDqsDlys[Dimm] = EyeCenter;
  } else {
    ChanPtr->WrDatDlys[Dimm] = EyeCenter + ChanPtr->WrDqsDlys[Dimm];
  }

  return TRUE;
}
