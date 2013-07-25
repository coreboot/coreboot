/* $NoKeywords:$ */
/**
 * @file
 *
 * mttRdDqs2DTraining.c
 *
 * RD DQS 2 dimentional training
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

#define FILECODE PROC_MEM_TECH_MTTRDDQS2DTRAINING_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define MAX_DELAYS    9   /* 8 data bytes + 1 ECC byte */
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
MemT2DRdDQSProcessConvolution (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       MEM_2D_ENTRY *Data
  );

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
  extern MEM_PSC_FLOW_BLOCK* memPlatSpecFlowArray[];

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function executes 2D training for Rd DQS
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  TRUE - No Errors occurred
 *     @return  FALSE - Errors occurred
 */

BOOLEAN
MemTAmdRdDqs2DTraining (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  MEM_NB_BLOCK *NBPtr;
  MEM_DATA_STRUCT *MemPtr;
  UINT8 Dct;
  UINT8 ChipSel;
  MEM_2D_ENTRY Data;
  UINT8 Lane;
  UINT8 Vref;
  BOOLEAN Status;
  BOOLEAN DCT_x4Present;
  UINT8 MaxLanes;
  PSO_TABLE *PsoTable;
  RD_DQS_2D  *VrefPtr;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;
  NBPtr = TechPtr->NBPtr;
  MemPtr = NBPtr->MemPtr;
  AGESA_TESTPOINT (TpProcMem2dRdDqsTraining, &(MemPtr->StdHeader));
  PsoTable = MemPtr->ParameterListPtr->PlatformMemoryConfiguration;
  //
  // Set environment settings before training
  //
  IDS_HDT_CONSOLE (MEM_STATUS, "\n\nStart RD DQS 2D training.\n\n");
  MemTBeginTraining (TechPtr);
  //
  // Allocate heap for the 2D RdDqs/Vref Data structure
  //
  DCT_x4Present = FALSE;
  // Check DCTs for x4 DIMMs
  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    NBPtr->SwitchDCT (NBPtr, Dct);
    if (NBPtr->ChannelPtr->DimmNibbleAccess != 0) {
      DCT_x4Present = TRUE;
      break;
    }
  }

  NBPtr->SwitchDCT (NBPtr, 0);
  // If DCT0 or DCT1 have x4 DIMMs, additonal allocate space
  if (DCT_x4Present == TRUE) {
    // Per Nibble
    MaxLanes = TechPtr->NBPtr->MCTPtr->Status[SbEccDimms] ? 18 : 16;
  } else {
    // Per Byte
    MaxLanes = TechPtr->NBPtr->MCTPtr->Status[SbEccDimms] ? 9 : 8;
  }
  MaxLanes = 8; /// @todo Family specific hook
  AllocHeapParams.RequestedBufferSize = MaxLanes * NBPtr->TotalMaxVrefRange * sizeof (RD_DQS_2D);
  AllocHeapParams.BufferHandle = AMD_MEM_2D_RDQS_HANDLE;
  AllocHeapParams.Persist = HEAP_LOCAL_CACHE;
  if (HeapAllocateBuffer (&AllocHeapParams, &MemPtr->StdHeader) == AGESA_SUCCESS) {
    VrefPtr = (RD_DQS_2D *) AllocHeapParams.BufferPtr;
  } else {
    SetMemError (AGESA_FATAL, NBPtr->MCTPtr);
    PutEventLog (AGESA_FATAL, MEM_ERROR_HEAP_ALLOCATE_FOR_2D, 0, 0, 0, 0, &MemPtr->StdHeader);
    return TRUE;
  }
  for (Lane = 0; Lane < MaxLanes; Lane++) {
    Data.Lane[Lane].Vref = &VrefPtr[Lane * NBPtr->TotalMaxVrefRange];
  }
  //
  // Setup hardware training engine
  //
  TechPtr->Direction = DQS_READ_DIR;
  TechPtr->TrainingType = TRN_DQS_POSITION;
  NBPtr->FamilySpecificHook[SetupHwTrainingEngine] (NBPtr, &TechPtr->TrainingType);

  Data.Vnom = NBPtr->TotalMaxVrefRange / 2; // Set Nominal Vref
  Data.MaxRdDqsSweep = NBPtr->TotalRdDQSDlyRange / 2; // Set Nominal Vref
  ASSERT (NBPtr->TotalRdDQSDlyRange <= MAX_RD_DQS_ENTRIES);
  //
  // Execute 2d Rd DQS training for all Dcts/Chipselects
  //
  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    IDS_HDT_CONSOLE (MEM_STATUS, "\tDct %d\n", Dct);
    NBPtr->SwitchDCT (NBPtr, Dct);
    Status = FALSE;
    if (MemTCheck2DTrainingPerConfig (TechPtr)) {
      for (ChipSel = 0; ChipSel < NBPtr->CsPerChannel; ChipSel = ChipSel + NBPtr->CsPerDelay ) {
        if ( (NBPtr->MCTPtr->Status[SbLrdimms]) ? ((NBPtr->ChannelPtr->LrDimmPresent & ((UINT8) 1 << (ChipSel >> 1))) != 0) :
             ((NBPtr->DCTPtr->Timings.CsEnabled & ((UINT16) 1 << ChipSel)) != 0) ) {
          //Initialize storage
          for (Lane = 0; Lane < MemT2DGetMaxLanes (TechPtr); Lane++) {
            for (Vref = 0; Vref < NBPtr->TotalMaxVrefRange; Vref++) {
              Data.Lane[Lane].Vref[Vref].PosRdDqsDly = 0;
              Data.Lane[Lane].Vref[Vref].NegRdDqsDly = 0;
            }
          }
          TechPtr->ChipSel = ChipSel;
          IDS_HDT_CONSOLE (MEM_FLOW,"\tChip Select: %02x \n", TechPtr->ChipSel);
          //
          // 1. Sample the data eyes for each channel:
          //
          TechPtr->Local2DData = &Data;
          if (TechPtr->NBPtr->MemN2DRdDQSDataCollection (NBPtr)) {
            //
            // 2. Process the array of results with a diamond convolution mask, summing the number passing sample points.
            //
            // Determine Diamond Mask Height
            if (MemT2DRdDQSHeight (TechPtr, &Data)) {
              // Apply Mask
              if (MemT2DRdDQSApplyMask (TechPtr, &Data)) {
                // Convolution
                if (MemT2DRdDQSProcessConvolution (TechPtr, &Data)) {
                  //
                  // 3. Program the final DQS delay values.
                  //
                  if (MemT2DRdDQSPrograMaxRdDQSDly (TechPtr, &Data)) {
                    //
                    // Find the Smallest Positive or Negative Margin for current CS
                    //
                    if (MemT2DRdDQSFindCsVrefMargin (TechPtr, &Data)) {
                      Status = TRUE;
                      //
                      // DATAEYE - Allocate Temp storage for Generate Composite Eyes,
                      //           Save composite eye for CS Pair into Allocated Storage.
                      TechPtr->TechnologySpecificHook[DataEyeSaveCompositeEyes] (TechPtr, &Data);
                    }
                  }
                }
              }
            }
          }
          if (Status == FALSE) {
            SetMemError (AGESA_ERROR, NBPtr->MCTPtr);
            PutEventLog (AGESA_ERROR, MEM_ERROR_2D_DQS_ERROR, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
          }
        } // Chipselect
      } // End of ChipSel for loop.

      //
      // DATAEYE - Compress Eyes, Deallocate temp storage. Allocate memory for compressed eyes,
      //           copy eyes to new struct, fixup pointers.
      //
      TechPtr->TechnologySpecificHook[DataEyeCompressEyes] (TechPtr, &Data);
      //
      // Find the Max and Min Vref values for each DCT
      //
      if (Status == TRUE) {
        if (MemT2DRdDQSFinalVrefMargin (TechPtr, &Data)) {
          //
          // Program the Max Vref value
          //
          IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\tProgramming Final Vref for channel\n\n");
          MemT2DProgramVref (TechPtr, NBPtr->ChannelPtr->MaxVref);
          Status = TRUE;
        } else {
          SetMemError (AGESA_ERROR, NBPtr->MCTPtr);
          PutEventLog (AGESA_ERROR, MEM_ERROR_2D_DQS_VREF_MARGIN_ERROR, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
        }
      }
    }
  }
  //
  // Restore environment settings after training
  //
  if (HeapDeallocateBuffer (AMD_MEM_2D_RDQS_HANDLE, &MemPtr->StdHeader) != AGESA_SUCCESS) {
    SetMemError (AGESA_FATAL, NBPtr->MCTPtr);
    PutEventLog (AGESA_FATAL, MEM_ERROR_HEAP_DEALLOCATE_FOR_2D, 0, 0, 0, 0, &MemPtr->StdHeader);
  }
  IDS_HDT_CONSOLE (MEM_STATUS, "\tEnd\n");
  MemTEndTraining (TechPtr);
  IDS_HDT_CONSOLE (MEM_FLOW, "\n\nEnd RD DQS 2D training\n\n");
  return (BOOLEAN) (NBPtr->MCTPtr->ErrCode < AGESA_FATAL);
}
/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function determines the maximum number of lanes to program 2D RdDQS training
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  TRUE - Configuration valid
 *              FALSE - Configuration invalid
 */
BOOLEAN
MemTCheck2DTrainingPerConfig (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  UINT8 i;
  if (TechPtr->NBPtr->RefPtr->ForceTrainMode == FORCE_TRAIN_AUTO) {
    i = 0;
    while (memPlatSpecFlowArray[i] != NULL) {
      if ((memPlatSpecFlowArray[i])->S2D (TechPtr->NBPtr, (memPlatSpecFlowArray[i])->EntryOfTables)) {
        return TRUE;
      }
      i++;
    }
  }
  return FALSE;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function determines the maximum number of lanes to program 2D RdDQS training
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  TRUE - Max Number of Lanes
 */
UINT8
MemT2DGetMaxLanes (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  UINT8 MaxLanes;
  if ((TechPtr->NBPtr->ChannelPtr->DimmNibbleAccess & (1 << (TechPtr->ChipSel >> 1))) != 0) {
    // Per Nibble
    MaxLanes = TechPtr->NBPtr->MCTPtr->Status[SbEccDimms] ? 18 : 16;
  } else {
    // Per Byte
    MaxLanes = TechPtr->NBPtr->MCTPtr->Status[SbEccDimms] ? 9 : 8;
  }
  MaxLanes = 8;
  return MaxLanes;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function programs Vref to internal or external control for 2D RdDQS training
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */
VOID
MemT2DProgramIntExtVrefSelect (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  TechPtr->NBPtr->SetBitField (TechPtr->NBPtr, BFVrefSel, (TechPtr->NBPtr->RefPtr->ExternalVrefCtl ? 0x0000 : 0x0001));
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function programs Vref for 2D RdDQS training
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]       Vref - Vref value
 *
 */
VOID
MemT2DProgramVref (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Vref
  )
{
  ID_INFO CallOutIdInfo;
  CallOutIdInfo.IdField.SocketId = TechPtr->NBPtr->MCTPtr->SocketId;
  CallOutIdInfo.IdField.ModuleId = TechPtr->NBPtr->MCTPtr->DieId;
  if (TechPtr->NBPtr->RefPtr->ExternalVrefCtl == FALSE) {
    //
    // Internal vref control
    /// @todo : Separate Family-Specific Funtionality
    //
    //
    // This is 1/2 VrefDAC value Sign bit is shifted into place.
    //
    ASSERT (Vref < 32);
    if (Vref < 15) {
      Vref = (31 - Vref) << 1;
    } else {
      Vref = (Vref - 15) << 1;
    }
    TechPtr->NBPtr->SetBitField (TechPtr->NBPtr, BFVrefDAC, Vref << 2);
  } else {
    // External vref control
    AGESA_TESTPOINT (TpProcMemBefore2dTrainExtVrefChange, &(TechPtr->NBPtr->MemPtr->StdHeader));
    TechPtr->NBPtr->MemPtr->ParameterListPtr->ExternalVrefValue = Vref;
    IDS_HDT_CONSOLE (MEM_FLOW, "\n2D training External Vref callout \n");
    //
    /// @todo: Implement UEFI DXE Callout for AgesaExternal2dTrainVrefChange before uncommenting this
    //
    // AgesaExternal2dTrainVrefChange ((UINTN) CallOutIdInfo.IdInformation, TechPtr->NBPtr->MemPtr);
    AGESA_TESTPOINT (TpProcMemAfter2dTrainExtVrefChange, &(TechPtr->NBPtr->MemPtr->StdHeader));
  }
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function programs RdDQS for 2D RdDQS training
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]       RdDQSDly -  RdDqs value
 *
 */
VOID
MemT2DPrograRdDQSDly (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 RdDQSDly
  )
{
  UINT32 RdDqsTime;
  // Program BL registers for both nibble (x4) and bytes (x8, x16)
  RdDqsTime = 0;
  RdDqsTime = (RdDQSDly & 0x1F) << 8;
  RdDqsTime = RdDqsTime | (RdDQSDly & 0x1F);
  if ((TechPtr->ChipSel / TechPtr->NBPtr->CsPerDelay) == 0) {
    TechPtr->NBPtr->SetBitField (TechPtr->NBPtr, BFDataByteRxDqsDLLDimm0Broadcast, RdDqsTime);
  } else if ((TechPtr->ChipSel / TechPtr->NBPtr->CsPerDelay) == 1) {
    TechPtr->NBPtr->SetBitField (TechPtr->NBPtr, BFDataByteRxDqsDLLDimm1Broadcast, RdDqsTime);
  } else if ((TechPtr->ChipSel / TechPtr->NBPtr->CsPerDelay) == 2) {
    TechPtr->NBPtr->SetBitField (TechPtr->NBPtr, BFDataByteRxDqsDLLDimm2Broadcast, RdDqsTime);
  } else if ((TechPtr->ChipSel / TechPtr->NBPtr->CsPerDelay) == 3) {
    TechPtr->NBPtr->SetBitField (TechPtr->NBPtr, BFDataByteRxDqsDLLDimm3Broadcast, RdDqsTime);
  }
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function stores data for 2D RdDQS training
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]   *Data   - Pointer to Result data structure
 *     @param[in]   *InPhaseResult[]   - Array of inphase results
 *     @param[in]   *PhaseResult180[]   - Array of Phase 180 results
 *
 *     @return  TRUE - No Errors occurred
 */
VOID
StoreResult (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       MEM_2D_ENTRY *Data,
  IN       UINT32 InPhaseResult[],
  IN       UINT32 PhaseResult180[]
  )
{
  UINT8 Lane;
  UINT8 Vref;
  UINT8 RdDqsDly;
  UINT32 Result;
  UINT32 Result180;
  UINT8 Index;
  Vref = TechPtr->NBPtr->Vref;
  RdDqsDly = TechPtr->NBPtr->RdDqsDly;
  for (Lane = 0; Lane < MemT2DGetMaxLanes (TechPtr); Lane++) {
    for (RdDqsDly = 0; RdDqsDly < Data->MaxRdDqsSweep; RdDqsDly++) {
      if ((TechPtr->NBPtr->ChannelPtr->DimmNibbleAccess & (1 << (TechPtr->ChipSel >> 1))) == 0) {
        // x8, so combine "Nibble X" and "Nibble X+1" results
        Index = Lane * 2;
        Result = (InPhaseResult[RdDqsDly] >> Index) & 0x03;
        Result180 = (PhaseResult180[RdDqsDly] >> Index) & 0x03;
      } else {
        // x4, so use "Nibble" results
        Result = (InPhaseResult[RdDqsDly] >> Lane) & 0x01;
        Result180 = (PhaseResult180[RdDqsDly] >> Lane) & 0x01;
      }
      Data->Lane[Lane].Vref[Vref].PosRdDqsDly |= (Result == 0) ? (1 << (Data->MaxRdDqsSweep - 1 - RdDqsDly)) : 0;
      Data->Lane[Lane].Vref[Vref].NegRdDqsDly |= (Result180 == 0) ? (1 << (Data->MaxRdDqsSweep - 1 - RdDqsDly)) : 0;
    }
  }
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function determines the height of data for 2D RdDQS training
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]   *Data   - Pointer to Result data structure
 *
 *     @return  TRUE - No Errors occurred
 *     @return  FALSE - Errors occurred
 */
BOOLEAN
MemT2DRdDQSHeight (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       MEM_2D_ENTRY *Data
  )
{
  UINT8 Lane;
  for (Lane = 0; Lane < MemT2DGetMaxLanes (TechPtr); Lane++) {
    Data->Lane[Lane].HalfDiamondHeight = 0x0F;
  }
  IDS_HDT_CONSOLE_DEBUG_CODE (
    IDS_HDT_CONSOLE (MEM_FLOW, "\n");
    IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\t  Lane: ");
    for (Lane = 0; Lane < MemT2DGetMaxLanes (TechPtr); Lane++) {
      IDS_HDT_CONSOLE (MEM_FLOW, "%02x ", Lane);
    }
    IDS_HDT_CONSOLE (MEM_FLOW, "\n");
    IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tHeight: ");
    for (Lane = 0; Lane < MemT2DGetMaxLanes (TechPtr); Lane++) {
      IDS_HDT_CONSOLE (MEM_FLOW, "%02x ", (2*(Data->Lane[Lane].HalfDiamondHeight) + 1));
    }
    IDS_HDT_CONSOLE (MEM_FLOW, "\n");
  );
  return TRUE;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function gets the width for 2D RdDQS training
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]   *Data   - Pointer to Result data structure
 *
 *     @return  width
 */
UINT8
MemGet2dRdDQSWidth (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       MEM_2D_ENTRY *Data
  )
{
  return TechPtr->NBPtr->DiamondWidthRd;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function gets the step height for the dimond mask for 2D RdDQS training
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]   *Data   - Pointer to Result data structure
 *     @param[in]   Vref   - current Vref value
 *     @param[in]   Lane   - current Lane
 *
 *     @return  TRUE - Step found and value should be updated
 *              FALSE - Step not found and value should not be updated
 */
BOOLEAN
MemCheck2dRdDQSDiamondMaskStep (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       MEM_2D_ENTRY *Data,
  IN       UINT8 Vref,
  IN       UINT8 Lane
  )
{
  UINT8 M;
  UINT8 VrefVal;
  UINT8 width;
  UINT8 i;
  BOOLEAN status;
  // m = -1 * height/width
  // (y-b)/m = x
  status = FALSE;
  if (Vref > (Data->Vnom - 1)) {
    VrefVal = (Vref + 1) - Data->Vnom;
  } else {
    VrefVal = Vref;
  }
  width = (MemGet2dRdDQSWidth (TechPtr, Data) - 1) / 2;
  M = Data->Lane[Lane].HalfDiamondHeight / width;
  i = 1;
  while (i <= Data->Lane[Lane].HalfDiamondHeight) {
    i = i + M;
    if (VrefVal == i) {
      status = TRUE;
    }
  }
  return status;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function applies a mask fo 2D RdDQS training
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]   *Data   - Pointer to Result data structure
 *
 *     @return  TRUE - No Errors occurred
 *     @return  FALSE - Errors occurred
 */
BOOLEAN
MemT2DRdDQSApplyMask (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       MEM_2D_ENTRY *Data
  )
{
  UINT8 RdDqsDly;
  UINT8 Lane;
  UINT8 Height;
  UINT8 Width;
  UINT32 PosNegData;
  UINT8 Vref;
  UINT8 count;
  UINT8 Dly;
  UINT8 endWidth;
  UINT8 startWidth;
  UINT8 origEndWidth;
  UINT8 origStartWidth;
  UINT8 maxOverLapWidth;
  UINT8 startOverLapWidth;
  BOOLEAN maxHeightExceeded;
  BOOLEAN negVrefComplete;
  BOOLEAN PosRdDqsToNegRdDqs;
  BOOLEAN NegRdDqsToPosRdDqs;
  MEM_NB_BLOCK *NBPtr;
  NBPtr = TechPtr->NBPtr;
  //
  // Initialize Convolution
  //
  for (Lane = 0; Lane < MemT2DGetMaxLanes (TechPtr); Lane++) {
    for (RdDqsDly = 0; RdDqsDly < NBPtr->TotalRdDQSDlyRange; RdDqsDly++) {
      Data->Lane[Lane].Convolution[RdDqsDly] = 0;
      NBPtr->FamilySpecificHook[Adjust2DDelayStepSize] (NBPtr, &RdDqsDly);
    }
  }
  endWidth = 0;
  startWidth = 0;
  origEndWidth = 0;
  origStartWidth = 0;
  startOverLapWidth = 0;
  maxOverLapWidth = 0;
  maxHeightExceeded = FALSE;
  IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\tDetermining Width");
  //
  // Get the Width of Diamond
  //
  Width = MemGet2dRdDQSWidth (TechPtr, Data);
  IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\t\tWidth: %02x\n", Width);
  ASSERT (Width != 0);

  IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\tExecuting convolution function\n");
  //
  // Perform the convolution by sweeping the mask function centered at nominal Vref. Results in a one
  // dimensional array with FOM values at each delay for each lane. Choose the delay setting at the peak
  // FOM value.
  //
  for (Lane = 0; Lane < MemT2DGetMaxLanes (TechPtr); Lane++) {
    Height = Data->Lane[Lane].HalfDiamondHeight;
    ASSERT (Height < Data->Vnom);
    //
    // RdDqsDly is divided around "Data->MaxRdDqsSweep" into positive and negative directions
    // Positive direction -> RdDqsDly = 0 to (Data->MaxRdDqsSweep - 1)
    // Negative direction -> RdDqsDly = Data->MaxRdDqsSweep to (NBPtr->TotalRdDQSDlyRange - 1)
    //
    for (RdDqsDly = 0; RdDqsDly < NBPtr->TotalRdDQSDlyRange; RdDqsDly++) {
      // Vref loop is divided around "Data->Vnom - 1" into positive and negative directions
      // Negative direction -> Vref = 0 ("Data->Vnom - 1") to Height("Data->Vnom - 1" - Height)
      // Positive direction -> Vref = "Data->Vnom" to Height("Data->Vnom" + Height)
      //
      negVrefComplete = FALSE;
      PosRdDqsToNegRdDqs = FALSE;
      NegRdDqsToPosRdDqs = FALSE;
      for (Vref = 0; Vref < (NBPtr->TotalMaxVrefRange - 1); Vref++) {
        // Initial negative direction where Vref = 0 ("Data->Vnom - 1"), so we need to set
        // initial startWidth and endWidth for +/- RdDqs
        //
        // Create common delay based on +/- RdDqs
        if (RdDqsDly > (Data->MaxRdDqsSweep - 1)) {
          Dly = RdDqsDly - Data->MaxRdDqsSweep;
        } else {
          Dly = RdDqsDly;
        }
        if (Vref == 0 ) {
          // Initialize -Vref
          maxHeightExceeded = FALSE; // reset for start of -Vref
          // Case 1: if +RdDqs - Check for lower bound (Width/2 > RdDqs > 0)
          //       : if -RdDqs - Check for lower bound (Width/2 + Data->MaxRdDqsSweep > RdDqs > Data->MaxRdDqsSweep)
          if (Dly < Width / 2) {
            endWidth = Dly + Width / 2 + 1;
            startWidth = 0;
          } else if ((Dly + Width / 2) > (Data->MaxRdDqsSweep - 1)) {
            // Case 2: if +RdDqs - Check for upper bound ((Data->MaxRdDqsSweep - 1) < RdDqs < ((Data->MaxRdDqsSweep - 1) - Width/2))
            //       : if -RdDqs - Check for lower bound ((DatNBPtra->TotalRdDQSDlyRange - 1) < RdDqs < ((NBPtr->TotalRdDQSDlyRange - 1) - Width/2))
            endWidth = Data->MaxRdDqsSweep;
            startWidth = Dly - Width / 2;
          } else {
            // Set the initial "startWidth" and "endWidth" for +/- RdDqs
            endWidth = Dly + Width / 2 + 1;
            startWidth = Dly - Width / 2;
          }
          origEndWidth = endWidth;
          origStartWidth = startWidth;
        } else if (Vref == Data->Vnom) {
          // Initialize +Vref
          endWidth = origEndWidth;
          startWidth = origStartWidth;
          maxHeightExceeded = FALSE; // reset for start of +Vref
          negVrefComplete = TRUE;
        } else if ((Vref > (Data->Vnom + Height)) && negVrefComplete == TRUE) {
          break; //switch to next RdDqs Dly if height exceeded for +vref and -vref complete
        } else {
          if (startWidth >= endWidth) {
            if (RdDqsDly == (NBPtr->TotalRdDQSDlyRange - 1)) {
              // Special condition for end of -RdDqs range
              startWidth = Data->MaxRdDqsSweep - 1;
              endWidth = Data->MaxRdDqsSweep;
            } else {
             // Width = 0, but Height not reached,
              startWidth = Dly;
              endWidth = Dly + 1;
            }
          } else {
            // Check for Case 1 and Case 2 above
            if ((RdDqsDly + Width / 2) > (NBPtr->TotalRdDQSDlyRange - 1)) {
              endWidth = origEndWidth;
            }
          }
          maxHeightExceeded = FALSE;
        }
        IDS_HDT_CONSOLE_DEBUG_CODE (
          if (Lane == 0) {
            if (RdDqsDly == (Data->MaxRdDqsSweep - (Width / 2)) ) {
              Data->DiamondLeft[Vref] = startWidth;
              Data->DiamondRight[Vref] = endWidth - 1;
            }
          }
        );
        //
        // Determine the correct RdDqs (+/-) and Vref (+/-)direction
        //
        if (maxHeightExceeded == FALSE) {
          if (RdDqsDly < Data->MaxRdDqsSweep) {
            if (Vref > (Data->Vnom - 1)) {
              PosNegData = Data->Lane[Lane].Vref[Vref].PosRdDqsDly; // +RdDqs Dly, +Vref
            } else {
              PosNegData = Data->Lane[Lane].Vref[(Data->Vnom - 1) - Vref].PosRdDqsDly; // +RdDqs Dly, -Vref
            }
          } else {
            if (Vref > (Data->Vnom - 1)) {
              PosNegData = Data->Lane[Lane].Vref[Vref].NegRdDqsDly; // -RdDqs Dly, +Vref
            } else {
              PosNegData = Data->Lane[Lane].Vref[(Data->Vnom - 1) - Vref].NegRdDqsDly; // -RdDqs Dly, -Vref
            }
          }
          //
          // Case 1: Non-overlap condition:
          //         Count the number of passes from "startWidth" to "endWidth"
          //
          for (count = startWidth; count < endWidth; count++) {
            Data->Lane[Lane].Convolution[RdDqsDly] = (UINT8) ((PosNegData >> count) & 0x1) + Data->Lane[Lane].Convolution[RdDqsDly];
          }
          // Case 2: Overlay between +RdDqs and -RdDqs starting from +RdDqs
          //         Count the number of passes from "startWidth" to "endWidth"
          //
          if ((RdDqsDly <= (Data->MaxRdDqsSweep - 1) && (RdDqsDly > ((Data->MaxRdDqsSweep - 1) - Width / 2)))) {
            startOverLapWidth = 0;
            if (Vref == 0 || Vref == Data->Vnom) {
              maxOverLapWidth = (RdDqsDly + Width / 2) - (Data->MaxRdDqsSweep - 1); // Initial overlap max width size
            } else if (maxOverLapWidth == 0) {
              maxOverLapWidth = startOverLapWidth; // Stop counting after overlap region complete
            }
            // Ensure that +/- vref is set correctly
            if (Vref > (Data->Vnom - 1)) {
              PosNegData = Data->Lane[Lane].Vref[Vref].NegRdDqsDly;
            } else {
              PosNegData = Data->Lane[Lane].Vref[(Data->Vnom - 1) - Vref].NegRdDqsDly;
            }
            // Need to count the number of passes when range extends from Pos RdDqs to Neg RdDqs
            for (count = startOverLapWidth; count < maxOverLapWidth; count++) {
              Data->Lane[Lane].Convolution[RdDqsDly] = (UINT8) ((PosNegData >> count) & 0x1) + Data->Lane[Lane].Convolution[RdDqsDly];
            }
            if (maxOverLapWidth > 0) {
              if (MemCheck2dRdDQSDiamondMaskStep (TechPtr, Data, Vref, Lane) || (Vref == 1) || (Vref == Data->Vnom)) {
                maxOverLapWidth--;  // Reduce overlap width outside of diamond mask
              }
              PosRdDqsToNegRdDqs = TRUE;
            }
          }
          if (((RdDqsDly - Data->MaxRdDqsSweep) < Width / 2) && (RdDqsDly > (Data->MaxRdDqsSweep - 1))) {
            //
            // Case 3: Overlay between -RdDqs and +RdDqs starting from -RdDqs
            //         Count the number of passes from "startWidth" to "endWidth"
            //
            maxOverLapWidth = Data->MaxRdDqsSweep;
            if (Vref == 0 || Vref == Data->Vnom) {
              startOverLapWidth = RdDqsDly - Width / 2; // Initial overlap start point
            } else if (startOverLapWidth > maxOverLapWidth) {
              maxOverLapWidth = maxOverLapWidth - 1; // Continue to count until MaxHeight excceded
            }
            // Ensure that vref + or - is set correctly
            if (Vref > (Data->Vnom - 1)) {
              PosNegData = Data->Lane[Lane].Vref[Vref].PosRdDqsDly;
            } else {
              PosNegData = Data->Lane[Lane].Vref[(Data->Vnom - 1) - Vref].PosRdDqsDly;
            }
            // Need to count the number of passes when range extends from Pos RdDqs to Neg RdDqs
            for (count = startOverLapWidth; count < maxOverLapWidth; count++) {
              Data->Lane[Lane].Convolution[RdDqsDly] = (UINT8) ((PosNegData >> count) & 0x1) + Data->Lane[Lane].Convolution[RdDqsDly];
            }
            if (startOverLapWidth < maxOverLapWidth) {
              if (MemCheck2dRdDQSDiamondMaskStep (TechPtr, Data, Vref, Lane) || (Vref == 1) || (Vref == Data->Vnom)) {
                startOverLapWidth++;  // Reduce overlap width outside of diamond mask
              }
              NegRdDqsToPosRdDqs = TRUE;
            }
          }
        }
        if (MemCheck2dRdDQSDiamondMaskStep (TechPtr, Data, Vref, Lane) || (Vref == 1) || (Vref == Data->Vnom)) {
          if (PosRdDqsToNegRdDqs) {
            startWidth++;
            endWidth = Data->MaxRdDqsSweep;
            PosRdDqsToNegRdDqs = FALSE;
          } else if (NegRdDqsToPosRdDqs) {
            startWidth = 0;
            endWidth--;
            NegRdDqsToPosRdDqs = FALSE;
          } else {
            startWidth++;
            endWidth--;
          }
        }
        NBPtr->FamilySpecificHook[Adjust2DVrefStepSize] (NBPtr, &Vref);
      }
      NBPtr->FamilySpecificHook[Adjust2DDelayStepSize] (NBPtr, &RdDqsDly);
    }
  }
  IDS_HDT_CONSOLE_DEBUG_CODE (
    IDS_HDT_CONSOLE (MEM_FLOW, "\n");
    IDS_HDT_CONSOLE (MEM_FLOW, "\t\t   Diamond Shape: \n");
    for (Vref = 0; Vref < (NBPtr->TotalMaxVrefRange - 1); Vref++) {
      IDS_HDT_CONSOLE (MEM_FLOW, "\n");
      for (RdDqsDly = (Data->MaxRdDqsSweep - Width); RdDqsDly < Data->MaxRdDqsSweep; RdDqsDly++) {
        if (Vref < (Data->Vnom - 1)) {
          if (RdDqsDly == Data->DiamondLeft[(NBPtr->TotalMaxVrefRange - 2) - Vref]) {
            IDS_HDT_CONSOLE (MEM_FLOW, " | ");
          } else if (RdDqsDly == Data->DiamondRight[(NBPtr->TotalMaxVrefRange - 2) - Vref]) {
            IDS_HDT_CONSOLE (MEM_FLOW, " | -> Width = %02x", (Data->DiamondRight[(NBPtr->TotalMaxVrefRange - 2) - Vref]) - (Data->DiamondLeft[(NBPtr->TotalMaxVrefRange - 2) - Vref]));
          } else {
            IDS_HDT_CONSOLE (MEM_FLOW, " ");
          }
        } else {
          if (RdDqsDly == Data->DiamondLeft[Vref - (Data->Vnom - 1)]) {
            IDS_HDT_CONSOLE (MEM_FLOW, " | ");
          } else if (RdDqsDly == Data->DiamondRight[Vref - (Data->Vnom - 1)]) {
            IDS_HDT_CONSOLE (MEM_FLOW, " | -> Width = %02x", (Data->DiamondRight[Vref - (Data->Vnom - 1)]) - (Data->DiamondLeft[Vref - (Data->Vnom - 1)]));
          } else {
            IDS_HDT_CONSOLE (MEM_FLOW, " ");
          }
        }
      }
    }
    IDS_HDT_CONSOLE (MEM_FLOW, "\n");
    IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\t Convolution results after processing raw data:\n");
    IDS_HDT_CONSOLE (MEM_FLOW, "\t\t Delay: ");
    for (RdDqsDly = 0; RdDqsDly < NBPtr->TotalRdDQSDlyRange; RdDqsDly++) {
      IDS_HDT_CONSOLE (MEM_FLOW, " %02x ", RdDqsDly <= (Data->MaxRdDqsSweep - 1) ? (Data->MaxRdDqsSweep - 1) - RdDqsDly :  (NBPtr->TotalRdDQSDlyRange - 1) - RdDqsDly);
    }
    IDS_HDT_CONSOLE (MEM_FLOW, "\n");
    for (Lane = 0; Lane < MemT2DGetMaxLanes (TechPtr); Lane++) {
      IDS_HDT_CONSOLE (MEM_FLOW, "\t\tLane: %02x\n", Lane);
      IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tConv: ");
      for (RdDqsDly = 0; RdDqsDly < NBPtr->TotalRdDQSDlyRange; RdDqsDly++) {
        IDS_HDT_CONSOLE (MEM_FLOW, "%03x ", Data->Lane[Lane].Convolution[RdDqsDly]);
      }
      IDS_HDT_CONSOLE (MEM_FLOW, "\n");
    }
  );
  return TRUE;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function Examines the convolution function and determines the Max RDqs for
 *     2D RdDQS training
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]   *Data   - Pointer to Result data structure
 *
 *     @return  TRUE - No Errors occurred
 *     @return  FALSE - Errors occurred
 */

BOOLEAN
MemT2DRdDQSProcessConvolution (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       MEM_2D_ENTRY *Data
  )
{
  UINT8 RdDqsDly;
  UINT8 Lane;
  UINT16 MaxFOM;
  UINT8 MaxRange;
  UINT8 CurrRange;
  MEM_NB_BLOCK *NBPtr;
  BOOLEAN status;

  NBPtr = TechPtr->NBPtr;
  status = TRUE;
  IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\tDetermining MaxRdDqs based on Convolution function\n");
  // Determine the MaxRdDqs Dly for the convolution function
  //   - Choose the delay setting at the peak FOM value.
  for (Lane = 0; Lane < MemT2DGetMaxLanes (TechPtr); Lane++) {
    // Find largest value as MaxFOM
    MaxFOM = 0;
    for (RdDqsDly = 0; RdDqsDly < NBPtr->TotalRdDQSDlyRange; RdDqsDly++) {
      if (Data->Lane[Lane].Convolution[RdDqsDly] > MaxFOM) {
        MaxFOM = Data->Lane[Lane].Convolution[RdDqsDly];
      }
    }
    status = MaxFOM > 0 ? TRUE : FALSE; // It is an error if all convolution points are zero

    // Then find the midpoint of the largest consecutive window w/ that MaxFOM
    // In cases of an even number of consecutive points w/ that MaxFOM exists,
    //  choose the midpoint to the right
    // All things being equal, favor the right side of a bi-modal eye
    // Stressful SSO patterns shift the eye right!
    MaxRange = 0;
    CurrRange = 0;
    for (RdDqsDly = 0; (MaxFOM > 0) && RdDqsDly < NBPtr->TotalRdDQSDlyRange; RdDqsDly++) {
      if (Data->Lane[Lane].Convolution[RdDqsDly] == MaxFOM) {
        CurrRange++;
        if (CurrRange >= MaxRange) {
          Data->Lane[Lane].MaxRdDqs = RdDqsDly - ((CurrRange - 1) / 2);
          MaxRange = CurrRange;
        }
      } else {
        CurrRange = 0;
      }
    }

    if (Data->Lane[Lane].MaxRdDqs > Data->MaxRdDqsSweep) {
      status = FALSE; // Error
    }
    // Set Actual register value
    if (Data->Lane[Lane].MaxRdDqs < Data->MaxRdDqsSweep) {
      Data->Lane[Lane].MaxRdDqs = (Data->MaxRdDqsSweep - 1) - Data->Lane[Lane].MaxRdDqs;
    } else {
      status = FALSE; // Error
    }
  }

  IDS_HDT_CONSOLE_DEBUG_CODE (
    IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\t\t    Lane: ");
    for (Lane = 0; Lane < MemT2DGetMaxLanes (TechPtr); Lane++) {
      IDS_HDT_CONSOLE (MEM_FLOW, "%02x ", Lane);
    }
    IDS_HDT_CONSOLE (MEM_FLOW, "\n");
    IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tMaxRdDqs: ");
    for (Lane = 0; Lane < MemT2DGetMaxLanes (TechPtr); Lane++) {
      IDS_HDT_CONSOLE (MEM_FLOW, "%02x ", Data->Lane[Lane].MaxRdDqs);
    }
    IDS_HDT_CONSOLE (MEM_FLOW, "\n");
  );

  if (status == FALSE) {
    SetMemError (AGESA_FATAL, NBPtr->MCTPtr);
    PutEventLog (AGESA_FATAL, MEM_ERROR_INVALID_2D_RDDQS_VALUE, 0, 0, 0, 0, &TechPtr->NBPtr->MemPtr->StdHeader);
  }
  return status;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function programs the Max RDqs for 2D RdDQS training from convolution
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]   *Data   - Pointer to Result data structure
 *
 *     @return  TRUE - No Errors occurred
 *     @return  FALSE - Errors occurred
 */
BOOLEAN
MemT2DRdDQSPrograMaxRdDQSDly (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       MEM_2D_ENTRY *Data
  )
{
  UINT8 Lane;
  UINT8 LaneHighRdDqs2dDlys;
  UINT8 LaneLowRdDqs2dDlys;
  IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\tProgramming MaxRdDysDly per Lane\n\n");
  for (Lane = 0; Lane < MemT2DGetMaxLanes (TechPtr); Lane++) {
    if ((TechPtr->NBPtr->ChannelPtr->DimmNibbleAccess & (1 << (TechPtr->ChipSel >> 1))) == 0) {
      // Program Byte based for x8 and x16
      TechPtr->NBPtr->SetTrainDly (TechPtr->NBPtr, AccessRdDqsDly, DIMM_BYTE_ACCESS ((TechPtr->ChipSel / TechPtr->NBPtr->CsPerDelay), Lane), (UINT16)Data->Lane[Lane].MaxRdDqs);
      TechPtr->NBPtr->ChannelPtr->RdDqsDlys[(TechPtr->ChipSel / TechPtr->NBPtr->CsPerDelay) * MAX_DELAYS + Lane] = Data->Lane[Lane].MaxRdDqs;
    } else {
      // Program nibble based x4, so use "Nibble"
      TechPtr->NBPtr->SetTrainDly (TechPtr->NBPtr, AccessRdDqs2dDly, DIMM_NBBL_ACCESS ((TechPtr->ChipSel / TechPtr->NBPtr->CsPerDelay), Lane), (UINT16)Data->Lane[Lane].MaxRdDqs);
      TechPtr->NBPtr->ChannelPtr->RdDqs2dDlys[(TechPtr->ChipSel / TechPtr->NBPtr->CsPerDelay) * MAX_NUMBER_LANES + Lane] = Data->Lane[Lane].MaxRdDqs;
      // For each pair of nibbles (high (Odd Nibble) and Low (Even nibble)), find the largest and use that as the RdDqsDly value
      if ((Lane & 0x1) == 0) {
        LaneHighRdDqs2dDlys = Data->Lane[Lane + 1].MaxRdDqs;
        LaneLowRdDqs2dDlys = Data->Lane[Lane].MaxRdDqs;
        if (LaneHighRdDqs2dDlys > LaneLowRdDqs2dDlys) {
          TechPtr->NBPtr->ChannelPtr->RdDqsDlys[(TechPtr->ChipSel / TechPtr->NBPtr->CsPerDelay) * MAX_DELAYS + (Lane >> 1)] = LaneHighRdDqs2dDlys;
        } else {
          TechPtr->NBPtr->ChannelPtr->RdDqsDlys[(TechPtr->ChipSel / TechPtr->NBPtr->CsPerDelay) * MAX_DELAYS + (Lane >> 1)] = LaneLowRdDqs2dDlys;
        }
      }
      TechPtr->NBPtr->DctCachePtr->Is2Dx4 = TRUE;
    }
  }
  return TRUE;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function finds the Positive and negative Vref Margin for the current CS
 *     for 2D RdDQS training
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]   *Data   - Pointer to Result data structure
 *
 *     @return  TRUE - No Errors occurred
 *     @return  FALSE - Errors occurred
 */
BOOLEAN
MemT2DRdDQSFindCsVrefMargin (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       MEM_2D_ENTRY *Data
  )
{
  UINT8 SmallestMaxVrefNeg;
  UINT8 Lane;
  UINT8 RdDqsDly;
  UINT8 Vref;
  UINT8 MaxVrefPositive;
  UINT8 MaxVrefNegative;
  UINT8 SmallestMaxVrefPos;
  UINT32 PosNegData;
  SmallestMaxVrefPos = 0xFF;
  SmallestMaxVrefNeg = 0;
  MaxVrefPositive = 0;
  MaxVrefNegative = 0xFF;
  IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\tFinding Smallest Max Positive and Negative Vref\n\n");
  for (Lane = 0; Lane < MemT2DGetMaxLanes (TechPtr); Lane++) {
    RdDqsDly = (Data->MaxRdDqsSweep - 1) - Data->Lane[Lane].MaxRdDqs;
    for (Vref = 0; Vref < (Data->Vnom - 1); Vref++) {
      // Neg Vref - (searching from top of array down)
      PosNegData = Data->Lane[Lane].Vref[Vref].PosRdDqsDly;
      if ((UINT8) ((PosNegData >> RdDqsDly) & 0x1) == 1) {
        MaxVrefNegative = Vref;
        break;
      }
      TechPtr->NBPtr->FamilySpecificHook[Adjust2DVrefStepSize] (TechPtr->NBPtr, &Vref);
    }
    for (Vref = (Data->Vnom - 1); Vref < (TechPtr->NBPtr->TotalMaxVrefRange - 1); Vref++) {
      // Pos Vref - (searching from Vnom + 1 of array down)
      PosNegData = Data->Lane[Lane].Vref[Vref].PosRdDqsDly;
      if ((UINT8) ((PosNegData >> RdDqsDly) & 0x1) == 0) {
        // Convert to register setting
        MaxVrefPositive = Vref - 1;// - Data->Vnom;
        break;
      } else {
        // If Vref = 1F passes, then smallest Vref = 0x1F
        if (Vref == ((TechPtr->NBPtr->TotalMaxVrefRange - 1) - 1)) {
          MaxVrefPositive = 0x1E;
          break;
        }
      }
      TechPtr->NBPtr->FamilySpecificHook[Adjust2DVrefStepSize] (TechPtr->NBPtr, &Vref);
    }
    if (MaxVrefPositive < SmallestMaxVrefPos) {
      // Find the smallest Max Pos Vref
      SmallestMaxVrefPos = MaxVrefPositive;
    }
    if (MaxVrefNegative > SmallestMaxVrefNeg) {
      // Find the largest Max Neg Vref
      SmallestMaxVrefNeg = MaxVrefNegative;
    }
  }
  if (SmallestMaxVrefPos != (Data->Vnom - 2)) {
    Data->SmallestPosMaxVrefperCS[TechPtr->ChipSel] = SmallestMaxVrefPos - Data->Vnom + 1;
  } else {
    Data->SmallestPosMaxVrefperCS[TechPtr->ChipSel] = 0;
  }
  Data->SmallestNegMaxVrefperCS[TechPtr->ChipSel] = (Data->Vnom - 1) - SmallestMaxVrefNeg;
  IDS_HDT_CONSOLE_DEBUG_CODE (
    IDS_HDT_CONSOLE (MEM_FLOW, "Smallest Max Positive Vref Offset from V-Nom for ChipSel %02x = + %02x\n", TechPtr->ChipSel, Data->SmallestPosMaxVrefperCS[TechPtr->ChipSel]);
    if (Data->SmallestPosMaxVrefperCS[TechPtr->ChipSel] == 0) {
      IDS_HDT_CONSOLE (MEM_FLOW, "Smallest Max Negative Vref Offset from V-Nom for ChipSel %02x = 00\n");
    } else {
      IDS_HDT_CONSOLE (MEM_FLOW, "Smallest Max Negative Vref Offset from V-Nom for ChipSel %02x = - %02x\n", TechPtr->ChipSel, Data->SmallestNegMaxVrefperCS[TechPtr->ChipSel]);
    }
  );
  return TRUE;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function finds the final Vref Margin for 2D RdDQS training
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]   *Data   - Pointer to Result data structure
 *
 *     @return  TRUE - No Errors occurred
 *     @return  FALSE - Errors occurred
 */
BOOLEAN
MemT2DRdDQSFinalVrefMargin (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       MEM_2D_ENTRY *Data
  )
{
  UINT8 ChipSel;
  UINT8 SmallestMaxPosVref;
  UINT8 SmallestMaxNegVref;
  UINT8 OffsetFromVref;
  UINT8 MaxRegVref;
  SmallestMaxNegVref = 0x7F;
  SmallestMaxPosVref = 0x7F;
  IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\tFinding Final Vref for channel\n\n");
  for (ChipSel = 0; ChipSel < TechPtr->NBPtr->CsPerChannel; ChipSel = ChipSel + TechPtr->NBPtr->CsPerDelay ) {
    if ( (TechPtr->NBPtr->MCTPtr->Status[SbLrdimms]) ? ((TechPtr->NBPtr->ChannelPtr->LrDimmPresent & ((UINT8) 1 << (ChipSel >> 1))) != 0) :
         ((TechPtr->NBPtr->DCTPtr->Timings.CsEnabled & ((UINT16) 1 << ChipSel)) != 0) ) {
      if (Data->SmallestPosMaxVrefperCS[ChipSel] < SmallestMaxPosVref) {
        SmallestMaxPosVref = Data->SmallestPosMaxVrefperCS[ChipSel];
      }
      if (Data->SmallestNegMaxVrefperCS[ChipSel] < SmallestMaxNegVref) {
        SmallestMaxNegVref = Data->SmallestNegMaxVrefperCS[ChipSel];
      }
    }
  }
  IDS_HDT_CONSOLE (MEM_FLOW, "Final Max Vref Offset From Vnom =");
  if (SmallestMaxPosVref > SmallestMaxNegVref) {
    OffsetFromVref = (SmallestMaxPosVref - SmallestMaxNegVref) / 2;
    if (OffsetFromVref != 0) {
      IDS_HDT_CONSOLE (MEM_FLOW, " + ");
    }
    TechPtr->NBPtr->ChannelPtr->MaxVref = (Data->Vnom - 1) + OffsetFromVref;
  } else {
    OffsetFromVref = (SmallestMaxNegVref - SmallestMaxPosVref) / 2;
    if (OffsetFromVref != 0) {
      IDS_HDT_CONSOLE (MEM_FLOW, " - ");
    }
    TechPtr->NBPtr->ChannelPtr->MaxVref = (Data->Vnom - 1) - OffsetFromVref;
  }
  IDS_HDT_CONSOLE (MEM_FLOW, "%02x \n", OffsetFromVref);
  MaxRegVref = TechPtr->NBPtr->ChannelPtr->MaxVref;
  if (MaxRegVref <= ((TechPtr->NBPtr->TotalMaxVrefRange / 2) - 1)) {
    MaxRegVref = (TechPtr->NBPtr->TotalMaxVrefRange - 1) - MaxRegVref;
  } else {
    MaxRegVref = MaxRegVref - (TechPtr->NBPtr->TotalMaxVrefRange / 2 - 1);
  }
  /// @todo: Need Family specific hook for MaxRegVref
  MaxRegVref = ((MaxRegVref * 2) & 0x3F);
  IDS_HDT_CONSOLE (MEM_FLOW, "Actual Max Vref programmed = %02x \n", MaxRegVref);
  return TRUE;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function displays ther results of the 2D search
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]   *Data   - Pointer to Result data structure
 *
 *     @return
 */
VOID
MemT2DRdDqsDisplaySearch (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       MEM_2D_ENTRY *Data
  )
{
  IDS_HDT_CONSOLE_DEBUG_CODE (
    UINT8 Lane;
    INT8 Vref;
    // Display data collected
    IDS_HDT_CONSOLE (MEM_FLOW, "\t\tDisplaying Data collected\n\n");
    for (Lane = 0; Lane < MemT2DGetMaxLanes (TechPtr); Lane++) {
      IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tLane: %02x\n", Lane);
      IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\t Vref NegRdDqs PosRdDqs\n");
      for (Vref = TechPtr->NBPtr->TotalMaxVrefRange - 2; Vref >= 0; Vref--) {
        if (Vref < (Data->Vnom - 1)) {
          IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\t - ");
          IDS_HDT_CONSOLE (MEM_FLOW, "%02x ", ((Data->Vnom -1) - Vref));
        } else if (Vref == (Data->Vnom - 1)) {
          IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\t   00 ");
        } else {
          IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\t + ");
          IDS_HDT_CONSOLE (MEM_FLOW, "%02x ", Vref - (Data->Vnom - 1));
        }
        IDS_HDT_CONSOLE (MEM_FLOW, "%08x", TechPtr->Local2DData->Lane[Lane].Vref[Vref].NegRdDqsDly); // debug
        IDS_HDT_CONSOLE (MEM_FLOW, "%08x \n", TechPtr->Local2DData->Lane[Lane].Vref[Vref].PosRdDqsDly); //debug
        TechPtr->NBPtr->FamilySpecificHook[Adjust2DVrefStepSize] (TechPtr->NBPtr, &Vref);
      }
    }
  )
}
