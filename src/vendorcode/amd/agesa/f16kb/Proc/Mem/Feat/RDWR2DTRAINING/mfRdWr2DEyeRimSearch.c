/* $NoKeywords:$ */
/**
 * @file
 *
 * mfRdWr2DEyeRimSearch.c
 *
 * Eye Rim Sampling Algorithm for use in 2D Read DQS or 2D Write Data Training
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Feat/RdWr2DTraining)
 * @e \$Revision: 84150 $ @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
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
#include "merrhdl.h"
#include "OptionMemory.h"
#include "mfRdWr2DTraining.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_FEAT_RDWR2DTRAINING_MFRDWR2DEYERIMSEARCH_FILECODE
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
MemFInitializeEyeRimSearch (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
STATIC
MemTEyeFill (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT8
STATIC
DetermineSavedState (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN   UINT8 lane,
  IN   INT8 y,
  IN   INT8 x
  );

UINT8
STATIC
GetPassFailValue (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN   UINT8 lane,
  IN   INT8 y,
  IN   INT8 x
  );

VOID
STATIC
SetPassFailValue (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN   UINT8 lane,
  IN   INT8 y,
  IN   INT8 x,
  IN   UINT8 result
  );

VOID
STATIC
SetSavedState (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN   UINT8 lane,
  IN   INT8 y,
  IN   INT8 x,
  IN   UINT8 result
  );

INT8
STATIC
Get1DTrainedEyeCenter (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN UINT8 lane
  );

BOOLEAN
STATIC
CheckForFail (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN   UINT8 lane,
  IN   INT8 y,
  IN   INT8 x
  );


INT8
STATIC
xlateY (
  IN INT8 y
  );

BOOLEAN
STATIC
ClearSampledPassResults (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT32
STATIC
CompareInPhase (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT32
STATIC
Compare180Phase (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
STATIC
ProgramVref (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Vref
  );

VOID
STATIC
StartAggressors (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       BOOLEAN TurnOn
  );

/* -----------------------------------------------------------------------------*/
/**
 *
 *     Initialize Eye Rim Search
 *
 *     @param[in,out] *NBPtr - Pointer to the MEM_NB_BLOCK
 *
 *     @return       BOOLEAN
 *                             TRUE
 */
BOOLEAN
STATIC
MemFInitializeEyeRimSearch (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  MEM_TECH_BLOCK *TechPtr;
  UINT8 lane;
  UINT8 vref;
  MEM_RD_WR_2D_ENTRY *Data;
  MEM_RD_WR_2D_RIM_ENTRY  *RimData;

  ASSERT (NBPtr != NULL);
  TechPtr = NBPtr->TechPtr;
  Data = TechPtr->RdWr2DData;
  RimData = Data->SavedData;
  //
  // Set Boundaries
  //
  RimData->xMax = Data->MaxRdWrSweep - 1;
  RimData->yMax = (NBPtr->TotalMaxVrefRange / 2) - 1;
  RimData->xMin = RimData->xMax * -1;
  RimData->yMin = RimData->yMax * -1;

  RimData->ParallelSampling = EYERIM_PARALLEL_SAMPLING;
  RimData->BroadcastDelays = EYERIM_BROADCAST_DELAYS;
  RimData->Dirs[0] = 1;
  RimData->Dirs[1] = -1;

  RimData->SampleCount = 0;
  RimData->VrefUpdates = 0;
  RimData->RdWrDlyUpdates = 0;
  for (lane = 0; lane < MemFRdWr2DGetMaxLanes (NBPtr); lane ++ ) {
    for ( vref = 0; vref < NBPtr->TotalMaxVrefRange; vref ++ ) {
      // 00b = state not saved
      // 01b = state saved
      RimData->LaneSaved[lane].Vref[vref].PosRdWrDly = 0;
      RimData->LaneSaved[lane].Vref[vref].NegRdWrDly = 0;
      // 00b = Fail
      // 01b = Pass
      Data->Lane[lane].Vref[vref].PosRdWrDly = 0;
      Data->Lane[lane].Vref[vref].NegRdWrDly = 0;
    }
  }
  if (TechPtr->Direction == DQS_READ_DIR) {
    NBPtr->MaxSeedCount = MAX_2D_RD_SEED_COUNT;
  } else {
    //
    // Set the seed count in terms of the Total desired bit times
    //
    NBPtr->MaxSeedCount = (UINT8) (NBPtr->TotalBitTimes2DWrTraining / (256 * 8 * MIN ( 1, NBPtr->MaxAggressorDimms[NBPtr->Dct])));
  };
  TechPtr->DqsRdWrPosSaved = 0;
  IDS_HDT_CONSOLE (MEM_FLOW, "\n\tTotal Bit Times: %d     Max Seed Count: %d\n",(TechPtr->Direction == DQS_READ_DIR) ? NBPtr->TotalBitTimes2DRdTraining:NBPtr->TotalBitTimes2DWrTraining,NBPtr->MaxSeedCount);
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *     This function collects data for Eye Rim Search
 *
 *     @param[in,out] *NBPtr    - Pointer to the MEM_NB_BLOCK
 *     @param[in,out] *OptParam - Not Used
 *
 *     @return       BOOLEAN
 *                                TRUE - No Errors occurred
 *                                FALSE - Errors occurred
 *
 */
BOOLEAN
MemFRdWr2DEyeRimSearch (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       VOID * OptParam
  )
{
  UINT8 lane;
  UINT8 j;
  UINT8 k;
  INT8 ydir;
  INT8 xdir;
  INT8 xi;
  INT8 yi;
  INT8 x;
  INT8 y;
  INT8 xmax;
  INT8 ymax;
  INT8 xmin;
  INT8 ymin;
  INT8 xo;
  INT8 xt;
  INT8 yo;
  INT8 yt;
  UINT8 slane;
  UINT8 result;
  INT8 states[2];
  UINT8 InitialCS;
  UINT8 ChipSel;
  UINT8 Aggr;
  UINT8 SeedCount;
  UINT32 InPhaseResult;
  UINT32 PhaseResult180;
  UINT32 Result;
  MEM_RD_WR_2D_ENTRY *Data;
  MEM_RD_WR_2D_RIM_ENTRY RimData;
  MEM_TECH_BLOCK *TechPtr;
  RD_WR_2D  *VrefPtr;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;

  ASSERT (NBPtr != NULL);
  TechPtr = NBPtr->TechPtr;
  Data = TechPtr->RdWr2DData;
  //
  // Allocate Storage for Rim Search
  //
  AllocHeapParams.RequestedBufferSize = MemFRdWr2DGetMaxLanes (NBPtr) * NBPtr->TotalMaxVrefRange * sizeof (RD_WR_2D);
  AllocHeapParams.BufferHandle = AMD_MEM_2D_RD_WR_RIM_HANDLE;
  AllocHeapParams.Persist = HEAP_LOCAL_CACHE;
  if (HeapAllocateBuffer (&AllocHeapParams, &NBPtr->MemPtr->StdHeader) == AGESA_SUCCESS) {
    VrefPtr = (RD_WR_2D *) AllocHeapParams.BufferPtr;
  } else {
    SetMemError (AGESA_FATAL, NBPtr->MCTPtr);
    PutEventLog (AGESA_FATAL, MEM_ERROR_HEAP_ALLOCATE_FOR_2D, 0, 0, 0, 0, &NBPtr->MemPtr->StdHeader);
    return FALSE;
  }
  for (lane = 0; lane < MemFRdWr2DGetMaxLanes (NBPtr); lane++) {
    RimData.LaneSaved[lane].Vref = &VrefPtr[lane * NBPtr->TotalMaxVrefRange];
  }
  Data->SavedData = &RimData;

  MemFInitializeEyeRimSearch (NBPtr);
  NBPtr->FamilySpecificHook[RdWr2DSelectIntExtVref] (NBPtr, NULL);
  InitialCS = TechPtr->ChipSel;
  NBPtr->FamilySpecificHook[Adjust2DPhaseMaskBasedOnEcc] (NBPtr, &NBPtr);
  NBPtr->FamilySpecificHook[RdWr2DInitVictim] (NBPtr, NULL);
  //
  // EnableDisable continuous writes on the agressor channels
  //

  for (Aggr = 0; Aggr < MAX (1 , NBPtr->MaxAggressorDimms[NBPtr->Dct]) ; Aggr += (NBPtr->IsSupported[PerDimmAggressors2D] ? 2 : NBPtr->CsPerDelay) ) {
    ClearSampledPassResults (NBPtr);
    //
    //  Enable continuous writes on the aggressors
    //
    StartAggressors (NBPtr, TRUE);
    for (ChipSel = InitialCS; ChipSel < (InitialCS + NBPtr->CsPerDelay); ChipSel++) {
      xmax = RimData.xMax;
      xmin = RimData.xMin;
      ymax = RimData.yMax;
      ymin = RimData.yMin;
      if ((NBPtr->DCTPtr->Timings.CsEnabled & ((UINT16) 1 << ChipSel)) != 0) {
        TechPtr->ChipSel = ChipSel;
        Data->RdWrDly = Data->MaxRdWrSweep;
        for (lane = 0; lane < MemFRdWr2DGetMaxLanes (NBPtr); lane ++ ) {
          //
          // Two loops to handle each quadrant from the trained point
          //
          for ( j = 0; j < 2; j++) {
            ydir = RimData.Dirs[j];
            for ( k = 0; k < 2; k++) {
              xdir = RimData.Dirs[k];
              //
              // Sample Loops - stay w/n the defined quadrant
              // assume xmax = -1 * xmin, ymax = -1 * ymin
              // initial point must always pass
              // end point - boundary or two consecutive fails along the y-axis
              //
              // Initial dx, dy step state
              // Starting dy at 8 to reduce samples necessary searching for the eye height
              states[0] = 0;
              states[1] = 8;
              // xi and yi are inital trained points, assumed to be inside the eye
              // These set the coordinate syst em for the quadrants
              xi = Get1DTrainedEyeCenter (NBPtr, lane);
              yi = 0; // Initial center is always at Vref nominal
              x = xi;
              y = yi;
              while (
                ((xdir > 0 && x >= xi && x <= xmax) ||
                (xdir < 0 && x <= xi && x >= xmin)) &&
                ((ydir > 0 && y >= yi && y <= ymax) ||
                (ydir < 0 && y <= yi && y >= ymin)) &&
                !(y == yi && (xdir * (xi - x) >= 2))
                ) {
                //
                // Decide if result is already sampled, or need to take the sample
                //
                if (0 == DetermineSavedState (NBPtr, lane, y, x)) {
                  RimData.SampleCount++;
                  //
                  // Result for this point is not in the cache, sample it
                  //
                  for (slane = 0; slane < MemFRdWr2DGetMaxLanes (NBPtr); slane ++ ) {
                    if (!RimData.ParallelSampling && ( slane !=  lane)) {
                      continue;
                    }
                    //
                    // Calculate the relative offset from the initial trained position for this lane
                    //
                    xo = Get1DTrainedEyeCenter (NBPtr, slane);
                    yo = 0;
                    xt = xo + (x - xi);
                    yt = yo + (y - yi);
                    if (xt > xmax || xt < xmin || yt > ymax || yt < ymin) {
                      continue;
                    }
                    //
                    // Update the vref and lane delays (yt, xt)
                    //
                    if (slane == lane) {
                      //
                      // For processors w/ a single Vref per channel, only set Vref once
                      //
                      if ( NBPtr->Vref != xlateY (yt)) {
                        //
                        // If not already set
                        //
                        RimData.VrefUpdates++;
                        NBPtr->Vref = xlateY (yt);
                        ProgramVref (NBPtr, NBPtr->Vref);
                      }
                    }
                    if (RimData.BroadcastDelays) {
                      //
                      // When BroadcastDelays, only set RdDqs once
                      //
                      if (slane == lane) {
                        //
                        // If current lane
                        //
                        if ( Data->RdWrDly != (UINT8) (xt & RimData.xMax)) {
                          //
                          // If the not already set
                          // Account for rollover.
                          //
                          RimData.RdWrDlyUpdates++;
                          Data->RdWrDly = (UINT8) (xt & RimData.xMax);
                          NBPtr->FamilySpecificHook[RdWr2DProgramDelays] (NBPtr, &Data->RdWrDly);
                        }
                      }
                    } else {
                      //
                      // For Reads, Program all RdDqs Delay Values the same
                      // For Writes, Program all WrDat Values with same value offset by WrDqs for that lane.
                      //
                      if ( Data->RdWrDly != (UINT8) (xt & RimData.xMax)) {
                        //
                        // If the not already set
                        // Account for rollover.
                        //
                        RimData.RdWrDlyUpdates++;
                        Data->RdWrDly = (UINT8) (xt & RimData.xMax);
                        MemTSetDQSDelayAllCSR (TechPtr, (UINT8) (xt & RimData.xMax));
                      }
                    }
                  }
                  //
                  // Perform Memory RW test
                  //
                  InPhaseResult = 0;
                  PhaseResult180 = 0;
                  NBPtr->FamilySpecificHook[RdWr2DInitVictimChipSel] (NBPtr, NULL);
                  for (SeedCount = 0; SeedCount < NBPtr->MaxSeedCount; SeedCount++) {
                    //
                    // Begin continuous reads and writes on the victim channels
                    //
                    NBPtr->FamilySpecificHook[RdWr2DStartVictim] (NBPtr, &SeedCount);
                    //
                    // Occasionally check if all trained lanes have already failed
                    //
                    if ((NBPtr->MaxSeedCount < 4) || ((SeedCount % (NBPtr->MaxSeedCount / 4)) == 0)) {
                      InPhaseResult |= CompareInPhase (NBPtr);
                      PhaseResult180 |= Compare180Phase (NBPtr);
                      if (((InPhaseResult & NBPtr->PhaseLaneMask) == NBPtr->PhaseLaneMask) &&
                          ((PhaseResult180& NBPtr->PhaseLaneMask) == NBPtr->PhaseLaneMask)) {
                        break;
                      }
                    }
                  }
                  //
                  // Obtain the results
                  //
                  for (slane = 0; slane < MemFRdWr2DGetMaxLanes (NBPtr); slane ++ ) {
                    if (!RimData.ParallelSampling && (slane != lane)) {
                      continue;
                    }
                    //
                    // Calculate the relative offset from legacy trained
                    //
                    xo = Get1DTrainedEyeCenter (NBPtr, RimData.BroadcastDelays?lane:slane);
                    yo = 0;
                    xt = xo + (x - xi);
                    yt = yo + (y - yi);
                    if (xt > xmax || xt < xmin || yt > ymax || yt < ymin) {
                      continue;
                    }
                    //
                    // In this example, data{}{}{} = 1 is a Fail, 0 is a Pass
                    // In-Phase Results
                    //
                    if (CheckForFail (NBPtr, slane, yt, xt)) {
                      //
                      // Don't overwrite a fail
                      //
                      if (xt >= 0) {
                        if ((NBPtr->ChannelPtr->DimmNibbleAccess & (1 << (TechPtr->ChipSel >> 1))) == 0) {
                          // x8, so combine "Nibble X" and "Nibble X+1" results
                          Result = (InPhaseResult >> (slane * 2)) & 0x03;
                        } else {
                          // x4, so use "Nibble" results
                          Result = (InPhaseResult >> slane) & 0x01;
                        }
                        SetPassFailValue (NBPtr, slane, yt, xt, (UINT8) (Result & 0x0F));
                        SetSavedState (NBPtr, slane, yt, xt, (UINT8) (Result & 0x0F));
                      } else {
                        if ((NBPtr->ChannelPtr->DimmNibbleAccess & (1 << (TechPtr->ChipSel >> 1))) == 0) {
                          // x8, so combine "Nibble X" and "Nibble X+1" results
                          Result = (PhaseResult180 >> (slane * 2)) & 0x03;
                        } else {
                          // x4, so use "Nibble" results
                          Result = (PhaseResult180 >> slane) & 0x01;
                        }
                        SetPassFailValue (NBPtr, slane, yt, xt, (UINT8) (Result & 0x0F));
                        SetSavedState (NBPtr, slane, yt, xt, (UINT8) (Result & 0x0F));
                      }
                    }
                  }
                }
                // Decide the next sample point based on the result of the current lane
                result = GetPassFailValue (NBPtr, lane, y, x);
                InPhaseResult = 0;
                PhaseResult180 = 0;
                // States && comments are relative to the ++ Quadrant
                if (result == 3) {
                  // Current Pass
                  if (states[0] > 0 || states[1] > 0) {
                    if (states[1] > 1 &&  y *  ydir <=  ymax - states[1]) {
                      // Current Pass, Continue searching up by leaps
                      states[0] = 0;
                    } else if (states[1] > 1 && y * ydir <= ymax - 4) {
                      // Current Pass, Continue searching up by leaps
                      states[0] = 0;
                      states[1] = 4;
                    } else if (states[1] > 1 && y * ydir <= ymax - 2) {
                      // Current Pass, Continue searching up by leaps
                      states[0] = 0;
                      states[1] = 2;
                    } else if (y * ydir <= ymax - 1) {
                      // Current Pass, Continue searching up by one
                      states[0] = 0;
                      states[1] = 1;
                    } else if (y * ydir == ymax) {
                      // Current Pass and at the top edge, Move right
                      states[0] = 1;
                      states[1] = 0;
                    } else {
                      ASSERT (FALSE);
                    }
                  } else {
                    // Move right one
                    states[0] = 1;
                    states[1] = 0;
                  }
                } else if (result == 2) {
                  // Current Fail
                  if (states[0] > 0) {
                    // Search down and left
                    states[0] = -1;
                    states[1] = -1;
                  } else if (states[1] > 1 || states[1] < 0) {
                    // Search down
                    states[0] = 0;
                    states[1] = -1;
                  } else if (states[1] == 1) {
                    // Move down and right
                    states[0] = 1;
                    states[1] = -1;
                  } else {
                    ASSERT (FALSE);
                  }
                } else {
                  ASSERT (FALSE);
                }
                // Update the coordinates based on the new state and quadrant direction
                x += xdir * states[0];
                y += ydir * states[1];
              }
            }
          }
        }
      }
    }
    //
    // Disable continuous writes on the agressor channels
    //
    StartAggressors (NBPtr, FALSE);
  }
  IDS_HDT_CONSOLE (MEM_FLOW,"\n\tSampleCount:%d",RimData.SampleCount);
  IDS_HDT_CONSOLE (MEM_FLOW,"\t\tVref Updates:%d",RimData.VrefUpdates);
  IDS_HDT_CONSOLE (MEM_FLOW,"\t\tRdDqs Dly Updates:%d\n",RimData.RdWrDlyUpdates);
  //
  // Finilazing Victim Continuous writes
  //
  NBPtr->FamilySpecificHook[RdWr2DFinalizeVictim] (NBPtr, NULL);
  TechPtr->ChipSel = InitialCS;
  //
  // Fill eye based on Rim Search results
  //
  MemTEyeFill (NBPtr);
  //
  // Display the results the completed eye
  //
  MemFRdWr2DDisplaySearch (NBPtr, Data);
  //
  // Restore environment settings after training
  //
  if (HeapDeallocateBuffer (AMD_MEM_2D_RD_WR_RIM_HANDLE, &NBPtr->MemPtr->StdHeader) != AGESA_SUCCESS) {
    SetMemError (AGESA_FATAL, NBPtr->MCTPtr);
    PutEventLog (AGESA_FATAL, MEM_ERROR_HEAP_DEALLOCATE_FOR_2D, 0, 0, 0, 0, &NBPtr->MemPtr->StdHeader);
  }
  return TRUE;
}


/* -----------------------------------------------------------------------------*/
/**
 *     Fill the data eye
 *
 *     @param[in,out] *NBPtr - Pointer to the MEM_NB_BLOCK
 *
 */
VOID
STATIC
MemTEyeFill (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  INT8 x;
  INT8 y;
  UINT8 lane;
  UINT8 result;
  INT8  yLastPass;
  UINT8 xMax;
  UINT8 yMax;
  UINT8 xMin;
  UINT8 yMin;
  BOOLEAN FirstPassFound;
  MEM_RD_WR_2D_RIM_ENTRY *SavedData;

  ASSERT (NBPtr != NULL);
  SavedData = ((MEM_RD_WR_2D_ENTRY*)NBPtr->TechPtr->RdWr2DData)->SavedData;

  xMax = SavedData->xMax;
  yMax = SavedData->yMax;
  xMin = SavedData->xMin;
  yMin = SavedData->yMin;
  for (lane = 0; lane < MemFRdWr2DGetMaxLanes (NBPtr); lane++) {
    for (x = xMin ; x <= xMax ; x++) {
      FirstPassFound = FALSE;
      yLastPass = yMin;
      //
      // Scan for the last passing value
      //
      for (y = yMax ; y >= yLastPass ; y--) {
        result = GetPassFailValue (NBPtr, lane, y, x);
        if (result == 0) {
          //
          // Not Saved, Mark it as FAIL. (Should already be cleared)
          //
        } else if (result == 2) {
          //
          // FAIL, so Mark as FAIL (Do nothing)
          //
        } else if (result == 3) {
          //
          // PASS, Mark it and save y value (This will end the loop)
          //
          SetPassFailValue (NBPtr, lane, y, x, result);
          yLastPass = y;
        } else {
          ASSERT (FALSE);
        }
      }
      //
      // Scan for the first pass, the fill until the last pass
      //
      for (y = yMin ; y < yLastPass ; y++) {
        result = GetPassFailValue (NBPtr, lane, y, x);
        if (result == 0) {
          //
          // Not Saved, if we've already found a first Passing value, mark it as a PASS
          //   otherwise, mark it as FAIL. (Should already be cleared)
          //
          if (FirstPassFound == TRUE) {
            SetPassFailValue (NBPtr, lane, y, x, result);
          }
        } else if (result == 2) {
          //
          // FAIL, so Mark as FAIL (Do nothing)
          //
        } else if (result == 3) {
          //
          // PASS, Mark it and set FirstPassFound
          //
          SetPassFailValue (NBPtr, lane, y, x, result);
          FirstPassFound = TRUE;
        } else {
          ASSERT (FALSE);
        }
      } // y Loop
    } //x Loop
  } // Lane Loop
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     Get the 1D trained center
 *
 *     @param[in,out] *NBPtr - Pointer to the MEM_NB_BLOCK
 *     @param[in]      lane  - UINT8 of current lane
 *
 *     @return          INT8 - Eye Center
 */
INT8
STATIC
Get1DTrainedEyeCenter (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 lane
  )
{
  MEM_TECH_BLOCK *TechPtr;
  ASSERT (NBPtr != NULL);
  TechPtr = NBPtr->TechPtr;
  if (TechPtr->Direction == DQS_READ_DIR) {
    if ((NBPtr->ChannelPtr->DimmNibbleAccess & (1 << (TechPtr->ChipSel >> 1))) == 0) {
      // Program Byte based for x8 and x16
      return (INT8)NBPtr->ChannelPtr->RdDqsDlys[(TechPtr->ChipSel / NBPtr->CsPerDelay) * MAX_DELAYS + lane];
    } else {
      return (INT8)NBPtr->ChannelPtr->RdDqsDlys[(TechPtr->ChipSel / NBPtr->CsPerDelay) * MAX_DELAYS + (lane >> 1)];
    }
  } else {
    return (INT8) (NBPtr->ChannelPtr->WrDatDlys[(TechPtr->ChipSel / NBPtr->CsPerDelay) * MAX_DELAYS + lane] -
                   NBPtr->ChannelPtr->WrDqsDlys[(TechPtr->ChipSel / NBPtr->CsPerDelay) * MAX_DELAYS + lane]);
  }
}


/* -----------------------------------------------------------------------------*/
/**
 *
 *     Determine if a Byte Lane result has been saved
 *
 *     @param[in,out] *NBPtr - Pointer to MEM_NB_BLOCK
 *     @param[in]      lane  - Current lane
 *     @param[in]      y     - Vref value
 *     @param[in]      x     - Delay
 *
 *     @return         UINT8
 *                         1 - value saved
 *                         0 - value not saved
 */
UINT8
STATIC
DetermineSavedState (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 lane,
  IN       INT8 y,
  IN       INT8 x
  )
{
  MEM_RD_WR_2D_RIM_ENTRY *SavedData;

  ASSERT (NBPtr != NULL);
  SavedData = ((MEM_RD_WR_2D_ENTRY*)NBPtr->TechPtr->RdWr2DData)->SavedData;

  if (x >= 0) {
    return (UINT8) (SavedData->LaneSaved[lane].Vref[xlateY (y)].PosRdWrDly >> (SavedData->xMax - (x & SavedData->xMax)) & 0x1);
  } else {
    return (UINT8) (SavedData->LaneSaved[lane].Vref[xlateY (y)].NegRdWrDly >> (SavedData->xMax - (x & SavedData->xMax)) & 0x1);
  }
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     Determine if a failure has occured
 *
 *     @param[in,out] *NBPtr - Pointer to MEM_NB_BLOCK
 *     @param[in]      lane  - Current lane
 *     @param[in]      y     - Vref value
 *     @param[in]      x     - Delay Value
 *
 *     @return         BOOLEAN
 *                             FALSE - Fail
 *                             TRUE  - Pass
 */
BOOLEAN
STATIC
CheckForFail (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 lane,
  IN       INT8 y,
  IN       INT8 x
  )
{
  MEM_RD_WR_2D_RIM_ENTRY *SavedData;

  ASSERT (NBPtr != NULL);
  SavedData = ((MEM_RD_WR_2D_ENTRY*)NBPtr->TechPtr->RdWr2DData)->SavedData;

  if (x >= 0) {
    if ((SavedData->LaneSaved[lane].Vref[xlateY (y)].PosRdWrDly >> (SavedData->xMax - (x & SavedData->xMax)) & 0x1) == 0) {
      // value not saved, so it is not fail
      return TRUE;
    } else {
      // value saved, so examine result
      if ((SavedData->LaneSaved[lane].Vref[xlateY (y)].PosRdWrDly >> (SavedData->xMax - (x & SavedData->xMax)) & 0x1) == 0) {
        // result = fail
        return FALSE;
      } else {
        // result = pass
        return TRUE;
      }
    }
  } else {
    if ((SavedData->LaneSaved[lane].Vref[xlateY (y)].NegRdWrDly >> (SavedData->xMax - (x & SavedData->xMax)) & 0x1) == 0) {
      // value not saved, so it is not fail
      return TRUE;
    } else {
      // value saved, so examine result
      if ((SavedData->LaneSaved[lane].Vref[xlateY (y)].NegRdWrDly >> (SavedData->xMax - (x & SavedData->xMax)) & 0x1) == 0) {
        // result = fail
        return FALSE;
      } else {
        // result = pass
        return TRUE;
      }
    }
  }
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     Get pass fail state of lane
 *
 *     @param[in,out] *NBPtr - Pointer to MEM_NB_BLOCK
 *     @param[in]      lane  - Current lane
 *     @param[in]      y     - Vref value
 *     @param[in]      x     - Delay Value
 *
 *     @return         UINT8
 *                         0 - Value not saved,
 *                         2 - Fail,
 *                         3 - Pass
 */
UINT8
STATIC
GetPassFailValue (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 lane,
  IN       INT8 y,
  IN       INT8 x
  )
{
  MEM_TECH_BLOCK *TechPtr;
  MEM_RD_WR_2D_ENTRY* RdWr2DData;
  MEM_RD_WR_2D_RIM_ENTRY *SavedData;

  ASSERT (NBPtr != NULL);
  TechPtr = NBPtr->TechPtr;
  RdWr2DData = TechPtr->RdWr2DData;
  SavedData = RdWr2DData->SavedData;

  if (x >= 0) {
    if ((SavedData->LaneSaved[lane].Vref[xlateY (y)].PosRdWrDly >> (SavedData->xMax - (x & SavedData->xMax)) & 0x1) == 0) {
      // value not saved
      return 0;
    } else {
      // value saved, so return pass/fail
      return ((RdWr2DData->Lane[lane].Vref[xlateY (y)].PosRdWrDly >> (SavedData->xMax - (x & SavedData->xMax)) & 0x1) == 0) ? 2 : 3;
    }
  } else {
    if ((SavedData->LaneSaved[lane].Vref[xlateY (y)].NegRdWrDly >> (SavedData->xMax - (x & SavedData->xMax)) & 0x1) == 0) {
      // value not saved
      return 0;
    } else {
      // value saved, so return pass/fail
      return ((RdWr2DData->Lane[lane].Vref[xlateY (y)].NegRdWrDly >> (SavedData->xMax - (x & SavedData->xMax)) & 0x1) == 0) ? 2 : 3;
    }
  }
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *    Set the Pass/Fail state of lane
 *
 *     @param[in,out] *NBPtr  - Pointer to MEM_NB_BLOCK
 *     @param[in]      lane   - Current lane
 *     @param[in]      y      - Vref value
 *     @param[in]      x      - Delay Value
 *     @param[in]      result - result value
 *
 */
VOID
STATIC
SetPassFailValue (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 lane,
  IN       INT8 y,
  IN       INT8 x,
  IN       UINT8 result
  )
{
  MEM_TECH_BLOCK *TechPtr;
  MEM_RD_WR_2D_ENTRY* RdWr2DData;
  MEM_RD_WR_2D_RIM_ENTRY *SavedData;

  ASSERT (NBPtr != NULL);
  TechPtr = NBPtr->TechPtr;
  RdWr2DData = TechPtr->RdWr2DData;
  SavedData = RdWr2DData->SavedData;

  if (x >= 0) {
    RdWr2DData->Lane[lane].Vref[xlateY (y)].PosRdWrDly |= (result == 0) ? (1 << (SavedData->xMax - (x & SavedData->xMax))) : 0;
  } else {
    RdWr2DData->Lane[lane].Vref[xlateY (y)].NegRdWrDly |= (result == 0) ? (1 << (SavedData->xMax - (x & SavedData->xMax))) : 0;
  }
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *    Set the save state of lane
 *
 *     @param[in,out] *NBPtr  - Pointer to MEM_NB_BLOCK
 *     @param[in]      lane   - Current lane
 *     @param[in]      y      - Vref value
 *     @param[in]      x      - Delay Value
 *     @param[in]      result - result value
 *
 */
VOID
STATIC
SetSavedState (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 lane,
  IN       INT8 y,
  IN       INT8 x,
  IN       UINT8 result
  )
{
  MEM_RD_WR_2D_RIM_ENTRY *SavedData;

  ASSERT (NBPtr != NULL);
  SavedData = ((MEM_RD_WR_2D_ENTRY*)NBPtr->TechPtr->RdWr2DData)->SavedData;

  if (x >= 0) {
    SavedData->LaneSaved[lane].Vref[xlateY (y)].PosRdWrDly |= (1 << (SavedData->xMax - (x & SavedData->xMax)));
  } else {
    SavedData->LaneSaved[lane].Vref[xlateY (y)].NegRdWrDly |= (1 << (SavedData->xMax - (x & SavedData->xMax)));
  }
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     Translate Vref into a positive, linear value that can be used as an
 *     array index.
 *
 *     @param[in]     y - INT8 of the (signed) Vref value
 *
 *     @return    UINT8 - Translated Value
 */
INT8
STATIC
xlateY (
  IN INT8 y
  )
{
  ASSERT ( y > -0x10);
  ASSERT ( y < 0x10);
  return (y + 0xF) & 0x1F;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     Re-walk the eye rim for each aggressor combination, which invalidates
 *     previous Passes in the sample array.  Previous Fails in the sample array
 *     remain valid. Knowledge of previous fails and speeds sampling for the
 *     subsequent walks, esp. when used in conjunction w/ ParallelSampling.
 *
 *     @param[in,out] *NBPtr - Pointer to the MEM_NB_BLOCK
 *
 *     @return       BOOLEAN
 *                             TRUE
 */
BOOLEAN
STATIC
ClearSampledPassResults (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 lane;
  UINT8 vref;
  MEM_RD_WR_2D_ENTRY *Data;
  MEM_RD_WR_2D_RIM_ENTRY  *RimData;

  ASSERT (NBPtr != NULL);

  Data = (MEM_RD_WR_2D_ENTRY*)NBPtr->TechPtr->RdWr2DData;
  RimData = Data->SavedData;

  for (lane = 0; lane < MemFRdWr2DGetMaxLanes (NBPtr); lane ++ ) {
    for ( vref = 0; vref < NBPtr->TotalMaxVrefRange; vref ++ ) {
      RimData->LaneSaved[lane].Vref[vref].PosRdWrDly &= ~(Data->Lane[lane].Vref[vref].PosRdWrDly);
      Data->Lane[lane].Vref[vref].PosRdWrDly = 0;
      RimData->LaneSaved[lane].Vref[vref].NegRdWrDly &= ~(Data->Lane[lane].Vref[vref].NegRdWrDly);
      Data->Lane[lane].Vref[vref].NegRdWrDly = 0;
    }
  }
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     Perform in-phase comparison
 *
 *     @param[in,out] *NBPtr  - Pointer to the MEM_NB_BLOCK
 *
 *     @return         UINT32 - Bitmap of results of comparison
 *                              1 = FAIL
 *                              0 = PASS
 */

UINT32
STATIC
CompareInPhase (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT32 Result;
  Result = 0;
  NBPtr->FamilySpecificHook[RdWr2DCompareInPhase] (NBPtr, &Result);
  return Result;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *    Perform 180 Degree out-of-phase comparison
 *
 *     @param[in,out] *NBPtr  - Pointer to the MEM_NB_BLOCK
 *
 *     @return         UINT32 - Bitmap of results of comparison
 *                              1 = FAIL
 *                              0 = PASS
 */

UINT32
STATIC
Compare180Phase (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT32 Result;
  Result = 0;
  NBPtr->FamilySpecificHook[RdWr2DCompare180Phase] (NBPtr, &Result);
  return Result;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     Program Vref after scaling to accomodate the register definition
 *
 *     @param[in,out] *NBPtr - Pointer to the MEM_NB_BLOCK
 *     @param[in]      Vref  - UINT8 value of Vref relative to the sample point
 *
 */

VOID
STATIC
ProgramVref (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Vref
  )
{
  UINT8 ScaledVref;
  ScaledVref = Vref;
  NBPtr->FamilySpecificHook[RdWr2DScaleVref] (NBPtr, &ScaledVref);
  NBPtr->FamilySpecificHook[RdWr2DProgramVref] (NBPtr, &ScaledVref);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     Turn Aggressor Channels On or Off
 *
 *     @param[in,out] *NBPtr - Pointer to the MEM_NB_BLOCK
 *     @param[in]      TurnOn - BOOLEAN
 *                     TRUE - Turn On, False = Turn Off.
 */

VOID
STATIC
StartAggressors (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       BOOLEAN TurnOn
  )
{
  NBPtr->FamilySpecificHook[TurnOnAggressorChannels] (NBPtr, &TurnOn);
}
