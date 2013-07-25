/* $NoKeywords:$ */
/**
 * @file
 *
 * mttRdDqs2DEyeRimmSearch.c
 *
 * RD DQS 2 Dimentional Training using Eye Rim Sampling
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Tech)
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
#include "Filecode.h"
#include "OptionMemory.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_TECH_MTTRDDQS2DEYERIMSEARCH_FILECODE
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
MemTInitializeEyeRimSearch (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

VOID
MemTEyeFill (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

BOOLEAN
CheckSaveResAtEdge (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN   UINT8 lane,
  IN   INT8 y,
  IN   INT8 x,
  IN   INT8 xdir
  );

UINT8
DetermineSavedState (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN   UINT8 lane,
  IN   INT8 y,
  IN   INT8 x
  );

UINT8
GetPassFailValue (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN   UINT8 lane,
  IN   INT8 y,
  IN   INT8 x
  );

VOID
SetPassFailValue (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN   UINT8 lane,
  IN   INT8 y,
  IN   INT8 x,
  IN   UINT8 result
  );

VOID
SetSavedState (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN   UINT8 lane,
  IN   INT8 y,
  IN   INT8 x,
  IN   UINT8 result
  );

INT8 MemTGet1DTrainedEyeCenter (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN UINT8 lane
  );

BOOLEAN
CheckForFail (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN   UINT8 lane,
  IN   INT8 y,
  IN   INT8 x
  );

BOOLEAN
AllocateSaveLaneStorage (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

INT8
xlateY (
  IN INT8 y
  );

BOOLEAN
ClearSampledPassResults (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

/* -----------------------------------------------------------------------------*/
/**
 *
 *    Initialize Eye Rim Search
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  TRUE
 */
BOOLEAN
MemTInitializeEyeRimSearch (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  UINT8 lane;
  UINT8 vref;
  MEM_2D_ENTRY *Data;
  MEM_2D_RIM_ENTRY  *RimData;
  Data = TechPtr->Local2DData;
  RimData = TechPtr->SavedData;
  //
  // Set Boundaries
  //
  RimData->xMax = Data->MaxRdDqsSweep - 1;
  RimData->yMax = (TechPtr->NBPtr->TotalMaxVrefRange / 2) - 1;
  RimData->xMin = RimData->xMax * -1;
  RimData->yMin = RimData->yMax * -1;

  RimData->ParallelSampling = EYERIM_PARALLEL_SAMPLING;
  RimData->BroadcastDelays = EYERIM_BROADCAST_DELAYS;
  RimData->Dirs[0] = 1;
  RimData->Dirs[1] = -1;

  RimData->SampleCount = 0;
  RimData->VrefUpdates = 0;
  RimData->RdDqsDlyUpdates = 0;
  for (lane = 0; lane < MemT2DGetMaxLanes (TechPtr); lane ++ ) {
    for ( vref = 0; vref < TechPtr->NBPtr->TotalMaxVrefRange; vref ++ ) {
      // 00b = state not saved
      // 01b = state saved
      RimData->LaneSaved[lane].Vref[vref].PosRdDqsDly = 0;
      RimData->LaneSaved[lane].Vref[vref].NegRdDqsDly = 0;
      // 00b = Fail
      // 01b = Pass
      Data->Lane[lane].Vref[vref].PosRdDqsDly = 0;
      Data->Lane[lane].Vref[vref].NegRdDqsDly = 0;
    }
  }
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *     This function collects data for Eye Rim Search
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  TRUE - No Errors occurred
 *     @return  FALSE - Errors occurred
 */
BOOLEAN
MemT2DRdDQSEyeRimSearch (
  IN OUT   MEM_TECH_BLOCK *TechPtr
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
  MEM_2D_ENTRY *Data;
  MEM_2D_RIM_ENTRY  RimData;
  MEM_NB_BLOCK *NBPtr;
  RD_DQS_2D  *VrefPtr;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;
  NBPtr = TechPtr->NBPtr;
  Data = TechPtr->Local2DData;

  //if (AllocateSaveLaneStorage(TechPtr)) {
  //  return FALSE;
  //}
  // Allocate Storage for Rim Search
  AllocHeapParams.RequestedBufferSize = MemT2DGetMaxLanes (TechPtr) * TechPtr->NBPtr->TotalMaxVrefRange * sizeof (RD_DQS_2D);
  AllocHeapParams.BufferHandle = AMD_MEM_2D_RDQS_RIM_HANDLE;
  AllocHeapParams.Persist = HEAP_LOCAL_CACHE;
  if (HeapAllocateBuffer (&AllocHeapParams, &TechPtr->NBPtr->MemPtr->StdHeader) == AGESA_SUCCESS) {
    VrefPtr = (RD_DQS_2D *) AllocHeapParams.BufferPtr;
  } else {
    SetMemError (AGESA_FATAL, TechPtr->NBPtr->MCTPtr);
    PutEventLog (AGESA_FATAL, MEM_ERROR_HEAP_ALLOCATE_FOR_2D, 0, 0, 0, 0, &TechPtr->NBPtr->MemPtr->StdHeader);
    return FALSE;
  }
  for (lane = 0; lane < MemT2DGetMaxLanes (TechPtr); lane++) {
    RimData.LaneSaved[lane].Vref = &VrefPtr[lane * TechPtr->NBPtr->TotalMaxVrefRange];
  }
  TechPtr->SavedData = &RimData;
  MemTInitializeEyeRimSearch (TechPtr);
  MemT2DProgramIntExtVrefSelect (TechPtr);
  InitialCS = TechPtr->ChipSel;
  NBPtr->FamilySpecificHook[Adjust2DPhaseMaskBasedOnEcc] (NBPtr, &NBPtr);
  NBPtr->InitializeRdDqs2dVictimContinuousWrites (NBPtr);
  //
  // EnableDisable continuous writes on the agressor channels
  //
  IDS_HDT_CONSOLE (MEM_FLOW,"\n\tEye Rim Search, ParallelSampling: %c, BroadcastDelays: %c \n", (RimData.ParallelSampling == TRUE) ? 'Y' : 'N', (RimData.BroadcastDelays == TRUE) ? 'Y' : 'N');

  for (Aggr = 0; Aggr < (NBPtr->MaxAggressorDimms[(NBPtr->Dct + 1) & 1] > 0 ? NBPtr->MaxAggressorDimms[(NBPtr->Dct + 1) & 1] : 1) ; Aggr += (NBPtr->IsSupported[PerDimmAggressors2D] ? 2 : NBPtr->CsPerDelay) ) {
    ClearSampledPassResults (TechPtr);
    //
    //  Enable continuous writes on the aggressors
    //
    NBPtr->AgressorContinuousWrites (NBPtr, Aggr, TRUE);
    for (ChipSel = InitialCS; ChipSel < (InitialCS + NBPtr->CsPerDelay); ChipSel++) {
      xmax = RimData.xMax;
      xmin = RimData.xMin;
      ymax = RimData.yMax;
      ymin = RimData.yMin;
      if ((NBPtr->DCTPtr->Timings.CsEnabled & ((UINT16) 1 << ChipSel)) != 0) {
        TechPtr->ChipSel = ChipSel;
        for (lane = 0; lane < MemT2DGetMaxLanes (TechPtr); lane ++ ) {
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
              xi = MemTGet1DTrainedEyeCenter (TechPtr, lane);
              yi = 0; // Initial center is always at Vref nominal
              x = xi;
              y = yi;
              while (
                ((xdir > 0 && x >= xi && x <= xmax) ||
                (xdir < 0 && x <= xi && x >= xmin)) &&
                ((ydir > 0 && y >= yi && y <= ymax) ||
                (ydir < 0 && y <= yi && y >= ymin)) &&
                !(y == yi && (xdir * (xi - x) >= 2) &&
                CheckSaveResAtEdge (TechPtr, lane, y, x, xdir))
                ) {
                //
                // Decide if result is already sampled, or need to take the sample
                //
                if (0 == DetermineSavedState (TechPtr, lane, y, x)) {
                  RimData.SampleCount++;
                  //
                  // Result for this point is not in the cache, sample it
                  //
                  for (slane = 0; slane < MemT2DGetMaxLanes (TechPtr); slane ++ ) {
                    if (!RimData.ParallelSampling && ( slane !=  lane)) {
                      continue;
                    }
                    //
                    // Calculate the relative offset from the initial trained position for this lane
                    //
                    xo = MemTGet1DTrainedEyeCenter (TechPtr, slane);
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
                        TechPtr->SavedData->VrefUpdates++;
                        NBPtr->Vref = xlateY (yt);
                        MemT2DProgramVref (TechPtr, xlateY (yt));
                      }
                    }
                    if (RimData.BroadcastDelays) {
                      // When BroadcastDelays, only set RdDqs once
                      if (slane == lane) {
                        //
                        // If current lane
                        //
                        if ( NBPtr->RdDqsDly != (UINT8) (xt & RimData.xMax)) {
                          //
                          // If the not already set
                          // Account for rollover.
                          //
                          TechPtr->SavedData->RdDqsDlyUpdates++;
                          NBPtr->RdDqsDly = (UINT8) (xt & RimData.xMax);
                          MemT2DPrograRdDQSDly (TechPtr, (UINT8) (xt & RimData.xMax));
                        }
                      }
                    } else {
                      //
                      /// @todo: Set individual lanes
                      //
                    }
                  }
                  //
                  // Perform Memory RW test
                  //
                  InPhaseResult = 0;
                  PhaseResult180 = 0;
                  NBPtr->InitializeRdDqs2dVictimChipSelContinuousWrites (NBPtr);
                  for (SeedCount = 0; SeedCount < NBPtr->MaxSeedCount; SeedCount++) {
                    //
                    // Begin continuous reads and writes on the victim channels
                    //
                    NBPtr->StartRdDqs2dVictimContinuousWrites (NBPtr, SeedCount);
                    //
                    // Occasionally check if all trained lanes have already failed
                    //
                    if ((NBPtr->MaxSeedCount < 4) || ((SeedCount % (NBPtr->MaxSeedCount / 4)) == 0)) {
                      InPhaseResult |= NBPtr->InPhaseCompareRdDqs2DPattern (NBPtr, TechPtr->TestBufPtr, TechPtr->PatternBufPtr, TechPtr->PatternLength * 64);
                      PhaseResult180 |= NBPtr->Phase180CompareRdDqs2DPattern (NBPtr, TechPtr->TestBufPtr, TechPtr->PatternBufPtr, TechPtr->PatternLength * 64);
                      if (((InPhaseResult & NBPtr->PhaseLaneMask) == NBPtr->PhaseLaneMask) && ((PhaseResult180& NBPtr->PhaseLaneMask) == NBPtr->PhaseLaneMask)) {
                        break;
                      }
                    }
                  }
                  //
                  // Obtain the results
                  //
                  for (slane = 0; slane < MemT2DGetMaxLanes (TechPtr); slane ++ ) {
                    if (!RimData.ParallelSampling && (slane != lane)) {
                      continue;
                    }
                    //
                    // Calculate the relative offset from legacy trained
                    //
                    xo = MemTGet1DTrainedEyeCenter (TechPtr, RimData.BroadcastDelays?lane:slane);
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
                    if (CheckForFail (TechPtr, slane, yt, xt)) {
                      //
                      // Don't overwrite a fail
                      //
                      if (xt > 0) {
                        if ((TechPtr->NBPtr->ChannelPtr->DimmNibbleAccess & (1 << (TechPtr->ChipSel >> 1))) == 0) {
                          // x8, so combine "Nibble X" and "Nibble X+1" results
                          Result = (InPhaseResult >> (slane * 2)) & 0x03;
                        } else {
                          // x4, so use "Nibble" results
                          Result = (InPhaseResult >> slane) & 0x01;
                        }
                        SetPassFailValue (TechPtr, slane, yt, xt, (UINT8) (Result & 0x0F));
                        SetSavedState (TechPtr, slane, yt, xt, (UINT8) (Result & 0x0F));
                      } else {
                        if ((TechPtr->NBPtr->ChannelPtr->DimmNibbleAccess & (1 << (TechPtr->ChipSel >> 1))) == 0) {
                          // x8, so combine "Nibble X" and "Nibble X+1" results
                          Result = (PhaseResult180 >> (slane * 2)) & 0x03;
                        } else {
                          // x4, so use "Nibble" results
                          Result = (PhaseResult180 >> slane) & 0x01;
                        }
                        SetPassFailValue (TechPtr, slane, yt, xt, (UINT8) (Result & 0x0F));
                        SetSavedState (TechPtr, slane, yt, xt, (UINT8) (Result & 0x0F));
                      }
                    }
                  }
                }
                // Decide the next sample point based on the result of the current lane
                result = GetPassFailValue (TechPtr, lane, y, x);
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
    NBPtr->AgressorContinuousWrites (NBPtr, Aggr, FALSE);
  }
  IDS_HDT_CONSOLE (MEM_FLOW,"\n\tSampleCount:%d",TechPtr->SavedData->SampleCount);
  IDS_HDT_CONSOLE (MEM_FLOW,"\t\tVref Updates:%d",TechPtr->SavedData->VrefUpdates);
  IDS_HDT_CONSOLE (MEM_FLOW,"\t\tRdDqs Dly Updates:%d\n",TechPtr->SavedData->RdDqsDlyUpdates);
  //
  // Finilazing Victim Continuous writes
  //
  NBPtr->FinalizeRdDqs2dVictimContinuousWrites (NBPtr);
  TechPtr->ChipSel = InitialCS;
  //
  // Fill eye based on Rim Search results
  //
  MemTEyeFill (TechPtr);
  //
  // Display the results the completed eye
  //
  MemT2DRdDqsDisplaySearch (TechPtr, Data);
  //
  // Restore environment settings after training
  //
  if (HeapDeallocateBuffer (AMD_MEM_2D_RDQS_RIM_HANDLE, &TechPtr->NBPtr->MemPtr->StdHeader) != AGESA_SUCCESS) {
    SetMemError (AGESA_FATAL, TechPtr->NBPtr->MCTPtr);
    PutEventLog (AGESA_FATAL, MEM_ERROR_HEAP_DEALLOCATE_FOR_2D, 0, 0, 0, 0, &TechPtr->NBPtr->MemPtr->StdHeader);
  }
  return TRUE;
}


/* -----------------------------------------------------------------------------*/
/**
 *     Fill the data eye
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return
 */
VOID
MemTEyeFill (
  IN OUT   MEM_TECH_BLOCK *TechPtr
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

  xMax = TechPtr->SavedData->xMax;
  yMax = TechPtr->SavedData->yMax;
  xMin = TechPtr->SavedData->xMin;
  yMin = TechPtr->SavedData->yMin;
  for (lane = 0; lane < MemT2DGetMaxLanes (TechPtr); lane++) {
    for (x = xMin ; x <= xMax ; x++) {
      FirstPassFound = FALSE;
      yLastPass = yMin;
      //
      // Scan for the last passing value
      //
      for (y = yMax ; y >= yLastPass ; y--) {
        result = GetPassFailValue (TechPtr, lane, y, x);
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
          SetPassFailValue (TechPtr, lane, y, x, result);
          yLastPass = y;
        } else {
          ASSERT (FALSE);
        }
      }
      //
      // Scan for the first pass, the fill until the last pass
      //
      for (y = yMin ; y < yLastPass ; y++) {
        result = GetPassFailValue (TechPtr, lane, y, x);
        if (result == 0) {
          //
          // Not Saved, if we've already found a first Passing value, mark it as a PASS
          //   otherwise, mark it as FAIL. (Should already be cleared)
          //
          if (FirstPassFound == TRUE) {
            SetPassFailValue (TechPtr, lane, y, x, result);
          }
        } else if (result == 2) {
          //
          // FAIL, so Mark as FAIL (Do nothing)
          //
        } else if (result == 3) {
          //
          // PASS, Mark it and set FirstPassFound
          //
          SetPassFailValue (TechPtr, lane, y, x, result);
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
 *    Get the 1D trained center
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]  lane - current lane
 *
 *     @return
 */
INT8
MemTGet1DTrainedEyeCenter (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 lane
  )
{
  if ((TechPtr->NBPtr->ChannelPtr->DimmNibbleAccess & (1 << (TechPtr->ChipSel >> 1))) == 0) {
    // Program Byte based for x8 and x16
    return (INT8)TechPtr->NBPtr->ChannelPtr->RdDqsDlys[(TechPtr->ChipSel / TechPtr->NBPtr->CsPerDelay) * MAX_DELAYS + lane];
  } else {
    return (INT8)TechPtr->NBPtr->ChannelPtr->RdDqsDlys[(TechPtr->ChipSel / TechPtr->NBPtr->CsPerDelay) * MAX_DELAYS + (lane >> 1)];
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *    Determine if the saved value is at or close to the edge
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]   *lane   - current lane
 *     @param[in]   y   -  vref value
 *     @param[in]   x   -  RdDqs value
 *     @param[in]   xdir   -  x-direction
 *
 *     @return  TRUE - close to edge, FALSE - not close to edge
 */
BOOLEAN
CheckSaveResAtEdge (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 lane,
  IN       INT8 y,
  IN       INT8 x,
  IN       INT8 xdir
  )
{
  if (x > 0) {
    if (((TechPtr->Local2DData->Lane[lane].Vref[xlateY (y)].PosRdDqsDly >> (TechPtr->SavedData->xMax - (x & TechPtr->SavedData->xMax) + (-1 * xdir))) & 0x1) == 0) {
      if (((TechPtr->SavedData->LaneSaved[lane].Vref[xlateY (y)].PosRdDqsDly >> (TechPtr->SavedData->xMax - (x & TechPtr->SavedData->xMax) + (-1 * xdir))) & 0x1) == 1) {
        if (((TechPtr->Local2DData->Lane[lane].Vref[xlateY (y)].PosRdDqsDly >> (TechPtr->SavedData->xMax - (x & TechPtr->SavedData->xMax) + (-2 * xdir))) & 0x1) == 0) {
          if (((TechPtr->SavedData->LaneSaved[lane].Vref[xlateY (y)].PosRdDqsDly >> (TechPtr->SavedData->xMax - (x & TechPtr->SavedData->xMax) + (-2 * xdir))) & 0x1) == 1) {
            return TRUE;
          } else {
            return FALSE;
          }
        } else {
          return FALSE;
        }
      } else {
        return FALSE;
      }
    } else {
      return FALSE;
    }
  } else {
    if (((TechPtr->Local2DData->Lane[lane].Vref[xlateY (y)].NegRdDqsDly >> (TechPtr->SavedData->xMax - (x & TechPtr->SavedData->xMax) + (-1 * xdir))) & 0x1) == 0) {
      if (((TechPtr->SavedData->LaneSaved[lane].Vref[xlateY (y)].NegRdDqsDly >> (TechPtr->SavedData->xMax - (x & TechPtr->SavedData->xMax) + (-1 * xdir))) & 0x1) == 1) {
        if (((TechPtr->Local2DData->Lane[lane].Vref[xlateY (y)].NegRdDqsDly >> (TechPtr->SavedData->xMax - (x & TechPtr->SavedData->xMax) + (-2 * xdir))) & 0x1) == 0) {
          if (((TechPtr->SavedData->LaneSaved[lane].Vref[xlateY (y)].NegRdDqsDly >> (TechPtr->SavedData->xMax - (x & TechPtr->SavedData->xMax) + (-2 * xdir))) & 0x1) == 1) {
            return TRUE;
          } else {
            return FALSE;
          }
        } else {
          return FALSE;
        }
      } else {
        return FALSE;
      }
    } else {
      return FALSE;
    }
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *    determine if a BL has been saved
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]   lane   - current lane
 *     @param[in]   y   -  vref value
 *     @param[in]   x   -  RdDqs value
 *
 *     @return  1 - value saved, 0 - value not saved
 */
UINT8
DetermineSavedState (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 lane,
  IN       INT8 y,
  IN       INT8 x
  )
{
  if (x > 0) {
    return (UINT8) (TechPtr->SavedData->LaneSaved[lane].Vref[xlateY (y)].PosRdDqsDly >> (TechPtr->SavedData->xMax - (x & TechPtr->SavedData->xMax)) & 0x1);
  } else {
    return (UINT8) (TechPtr->SavedData->LaneSaved[lane].Vref[xlateY (y)].NegRdDqsDly >> (TechPtr->SavedData->xMax - (x & TechPtr->SavedData->xMax)) & 0x1);
  }
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *    Determine if a failure has occured
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]   lane   - current lane
 *     @param[in]   y   -  vref value
 *     @param[in]   x   -  RdDqs value
 *
 *     @return  2 - Fail, 3 - Pass
 */
BOOLEAN
CheckForFail (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 lane,
  IN       INT8 y,
  IN       INT8 x
  )
{
  if (x > 0) {
    if ((TechPtr->SavedData->LaneSaved[lane].Vref[xlateY (y)].PosRdDqsDly >> (TechPtr->SavedData->xMax - (x & TechPtr->SavedData->xMax)) & 0x1) == 0) {
      // value not saved, so it is not fail
      return TRUE;
    } else {
      // value saved, so examine result
      if ((TechPtr->SavedData->LaneSaved[lane].Vref[xlateY (y)].PosRdDqsDly >> (TechPtr->SavedData->xMax - (x & TechPtr->SavedData->xMax)) & 0x1) == 0) {
        // result = fail
        return FALSE;
      } else {
        // result = pass
        return TRUE;
      }
    }
  } else {
    if ((TechPtr->SavedData->LaneSaved[lane].Vref[xlateY (y)].NegRdDqsDly >> (TechPtr->SavedData->xMax - (x & TechPtr->SavedData->xMax)) & 0x1) == 0) {
      // value not saved, so it is not fail
      return TRUE;
    } else {
      // value saved, so examine result
      if ((TechPtr->SavedData->LaneSaved[lane].Vref[xlateY (y)].NegRdDqsDly >> (TechPtr->SavedData->xMax - (x & TechPtr->SavedData->xMax)) & 0x1) == 0) {
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
 *    Get pass fail state of lane
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]   lane   - current lane
 *     @param[in]   y   -  vref value
 *     @param[in]   x   -  RdDqs value
 *
 *     @return  0 - Value not saved, 2 - Fail, 3 - Pass
 */
UINT8
GetPassFailValue (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 lane,
  IN       INT8 y,
  IN       INT8 x
  )
{
  if (x > 0) {
    if ((TechPtr->SavedData->LaneSaved[lane].Vref[xlateY (y)].PosRdDqsDly >> (TechPtr->SavedData->xMax - (x & TechPtr->SavedData->xMax)) & 0x1) == 0) {
      // value not saved
      return 0;
    } else {
      // value saved, so return pass/fail
      return ((TechPtr->Local2DData->Lane[lane].Vref[xlateY (y)].PosRdDqsDly >> (TechPtr->SavedData->xMax - (x & TechPtr->SavedData->xMax)) & 0x1) == 0) ? 2 : 3;
    }
  } else {
    if ((TechPtr->SavedData->LaneSaved[lane].Vref[xlateY (y)].NegRdDqsDly >> (TechPtr->SavedData->xMax - (x & TechPtr->SavedData->xMax)) & 0x1) == 0) {
      // value not saved
      return 0;
    } else {
      // value saved, so return pass/fail
      return ((TechPtr->Local2DData->Lane[lane].Vref[xlateY (y)].NegRdDqsDly >> (TechPtr->SavedData->xMax - (x & TechPtr->SavedData->xMax)) & 0x1) == 0) ? 2 : 3;
    }
  }
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *    Set the Pass/Fail state of lane
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]   *lane   - current lane
 *     @param[in]   y   -  vref value
 *     @param[in]   x   -  RdDqs value
 *     @param[in]   result   -  result value
 *
 *     @return  Saved Value
 */
VOID
SetPassFailValue (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 lane,
  IN       INT8 y,
  IN       INT8 x,
  IN       UINT8 result
  )
{
  if (x > 0) {
    TechPtr->Local2DData->Lane[lane].Vref[xlateY (y)].PosRdDqsDly |= (result == 0) ? (1 << (TechPtr->SavedData->xMax - (x & TechPtr->SavedData->xMax))) : 0;
  } else {
    TechPtr->Local2DData->Lane[lane].Vref[xlateY (y)].NegRdDqsDly |= (result == 0) ? (1 << (TechPtr->SavedData->xMax - (x & TechPtr->SavedData->xMax))) : 0;
  }
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *    Set the save state of lane
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]   *lane   - current lane
 *     @param[in]   y   -  vref value
 *     @param[in]   x   -  RdDqs value
 *     @param[in]   result   -  result value
 *
 *     @return  Saved Value
 */
VOID
SetSavedState (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 lane,
  IN       INT8 y,
  IN       INT8 x,
  IN       UINT8 result
  )
{
  if (x > 0) {
    TechPtr->SavedData->LaneSaved[lane].Vref[xlateY (y)].PosRdDqsDly |= (1 << (TechPtr->SavedData->xMax - (x & TechPtr->SavedData->xMax)));
  } else {
    TechPtr->SavedData->LaneSaved[lane].Vref[xlateY (y)].NegRdDqsDly |= (1 << (TechPtr->SavedData->xMax - (x & TechPtr->SavedData->xMax)));
  }
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *    Allocate data storage
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  FALSE - No Errors occurred
 *     @return  TRUE - Errors occurred
 */
BOOLEAN
AllocateSaveLaneStorage (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  RD_DQS_2D  *VrefPtr;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;
  UINT8 Lane;

  AllocHeapParams.RequestedBufferSize = MemT2DGetMaxLanes (TechPtr) * TechPtr->NBPtr->TotalMaxVrefRange * sizeof (RD_DQS_2D);
  AllocHeapParams.BufferHandle = AMD_MEM_2D_RDQS_RIM_HANDLE;
  AllocHeapParams.Persist = HEAP_LOCAL_CACHE;
  if (HeapAllocateBuffer (&AllocHeapParams, &TechPtr->NBPtr->MemPtr->StdHeader) == AGESA_SUCCESS) {
    VrefPtr = (RD_DQS_2D *) AllocHeapParams.BufferPtr;
  } else {
    SetMemError (AGESA_FATAL, TechPtr->NBPtr->MCTPtr);
    PutEventLog (AGESA_FATAL, MEM_ERROR_HEAP_ALLOCATE_FOR_2D, 0, 0, 0, 0, &TechPtr->NBPtr->MemPtr->StdHeader);
    return TRUE;
  }
  for (Lane = 0; Lane < MemT2DGetMaxLanes (TechPtr); Lane++) {
    TechPtr->SavedData->LaneSaved[Lane].Vref = &VrefPtr[Lane * TechPtr->NBPtr->TotalMaxVrefRange];
  }
  return FALSE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *    Translate Vref into a positive, linear value that can be used as an
 *      array index.
 *
 *     @param[in]   y   -  vref value
 *
 *     @return  Saved Value
 */
INT8
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
 *     Re-walking the eye rim for each aggressor combination, which invalidates previous Passes
 *     in the sample array.  Previous Fails in the sample array remain valid. Knowledge of previous Fails
 *     and speeds sampling for the subsequent walks, esp. when used in conjunction w/ ParallelSampling
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  TRUE
 */
BOOLEAN
ClearSampledPassResults (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  UINT8 lane;
  UINT8 vref;
  MEM_2D_ENTRY *Data;
  MEM_2D_RIM_ENTRY  *RimData;
  Data = TechPtr->Local2DData;
  RimData = TechPtr->SavedData;
  for (lane = 0; lane < MemT2DGetMaxLanes (TechPtr); lane ++ ) {
    for ( vref = 0; vref < TechPtr->NBPtr->TotalMaxVrefRange; vref ++ ) {
      RimData->LaneSaved[lane].Vref[vref].PosRdDqsDly &= ~(Data->Lane[lane].Vref[vref].PosRdDqsDly);
      Data->Lane[lane].Vref[vref].PosRdDqsDly = 0;
      RimData->LaneSaved[lane].Vref[vref].NegRdDqsDly &= ~(Data->Lane[lane].Vref[vref].NegRdDqsDly);
      Data->Lane[lane].Vref[vref].NegRdDqsDly = 0;
    }
  }
  return TRUE;
}
