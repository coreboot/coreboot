/* $NoKeywords:$ */
/**
 * @file
 *
 * mfRdWr2DTraining.c
 *
 * Common Read/Write 2D Training Feature Function
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
#include "Ids.h"
#include "AdvancedApi.h"
#include "GeneralServices.h"
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

#define FILECODE PROC_MEM_FEAT_RDWR2DTRAINING_MFRDWR2DTRAINING_FILECODE
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
MemFRdWr2DScaleVref (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *Vref
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
 *   This function initializes the 2D Read/Write Training Feature Hooks.
 *
 *     @param[in,out] *NBPtr - Pointer to the MEM_NB_BLOCK
 *
 *     @return       BOOLEAN
 *                             TRUE - Function was implemented
 */

BOOLEAN
MemFRdWr2DTrainingInit (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  ASSERT (NBPtr != NULL);
  NBPtr->FamilySpecificHook[RdWr2DTraining] = MemFAmdRdWr2DTraining;
  NBPtr->FamilySpecificHook[CheckRdWr2DTrainingPerConfig] = MemFCheckRdWr2DTrainingPerConfig;
  NBPtr->FamilySpecificHook[RdWr2DSelectIntExtVref] = MemFRdWr2DProgramIntExtVrefSelect;
  NBPtr->FamilySpecificHook[RdWr2DProgramVref] = MemFRdWr2DProgramVref;
  NBPtr->FamilySpecificHook[RdWr2DScaleVref] = MemFRdWr2DScaleVref;
  NBPtr->FamilySpecificHook[RdWr2DProgramDelays] = MemFRdWr2DProgramDelays;
  NBPtr->FamilySpecificHook[RdWr2DDataCollection] = MemFRdWr2DEyeRimSearch;
  NBPtr->FamilySpecificHook[RdWr2DInitVictim] = MemFRdWr2DInitVictim;
  NBPtr->FamilySpecificHook[RdWr2DInitVictimChipSel] = MemFRdWr2DInitVictimChipSel;
  NBPtr->FamilySpecificHook[RdWr2DStartVictim] = MemFRdWr2DStartVictim;
  NBPtr->FamilySpecificHook[RdWr2DFinalizeVictim] = MemFRdWr2DFinalizeVictim;
  NBPtr->FamilySpecificHook[RdWr2DCompareInPhase] = MemFRdWr2DCompareInPhase;
  NBPtr->FamilySpecificHook[RdWr2DCompare180Phase] = MemFRdWr2DCompare180Phase;
  NBPtr->FamilySpecificHook[RdWr2DProgramDataPattern] = MemFRdWr2DProgramDataPattern;
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function executes 2D training for Read DQS or Write DQ
 *
 *     @param[in,out] *NBPtr    - Pointer to the MEM_NB_BLOCK
 *     @param[in,out] *OptParam - Unused
 *
 *     @return          BOOLEAN
 *                               TRUE  - No Errors occurred
 *                               FALSE - Errors occurred
 */

BOOLEAN
MemFAmdRdWr2DTraining (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID* OptParam
  )
{
  MEM_TECH_BLOCK *TechPtr;
  MEM_DATA_STRUCT *MemPtr;
  MEM_RD_WR_2D_ENTRY Data;
  RD_WR_2D  *VrefPtr;
  PSO_TABLE *PsoTable;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;
  UINT8 Dct;
  UINT8 ChipSel;
  UINT8 Lane;
  UINT8 Vref;
  UINT8 MaxLanes;
  UINT8 TmpLanes;
  UINT8 TotalDlyRange;
  BOOLEAN Status;
  //
  // Initialize Pointers
  //
  ASSERT (NBPtr != NULL);
  TechPtr = NBPtr->TechPtr;
  MemPtr = NBPtr->MemPtr;
  PsoTable = MemPtr->ParameterListPtr->PlatformMemoryConfiguration;
  //
  // Set environment settings before training
  //
  AGESA_TESTPOINT (TpProcMem2dRdDqsTraining, &(MemPtr->StdHeader));
  MemTBeginTraining (TechPtr);
  //
  // Allocate heap for the 2D RdWR/Vref Data structure
  //
  MaxLanes = 0;
  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    NBPtr->SwitchDCT (NBPtr, Dct);
    TmpLanes = MemFRdWr2DGetMaxLanes (NBPtr);
    MaxLanes = MAX (MaxLanes, TmpLanes);
  }

  AllocHeapParams.RequestedBufferSize = MaxLanes * NBPtr->TotalMaxVrefRange * sizeof (RD_WR_2D);
  AllocHeapParams.BufferHandle = AMD_MEM_2D_RD_WR_HANDLE;
  AllocHeapParams.Persist = HEAP_LOCAL_CACHE;
  if (HeapAllocateBuffer (&AllocHeapParams, &MemPtr->StdHeader) == AGESA_SUCCESS) {
    VrefPtr = (RD_WR_2D *) AllocHeapParams.BufferPtr;
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
  TechPtr->TrainingType = TRN_DQS_POSITION;
  NBPtr->FamilySpecificHook[SetupHwTrainingEngine] (NBPtr, &TechPtr->TrainingType);

  Data.Vnom = NBPtr->TotalMaxVrefRange / 2; // Set Nominal Vref
  TotalDlyRange = (TechPtr->Direction == DQS_READ_DIR) ? NBPtr->TotalRdDQSDlyRange : NBPtr->TotalWrDatDlyRange;
  Data.MaxRdWrSweep = TotalDlyRange / 2; // Set Max Sweep Size
  ASSERT (TotalDlyRange <= MAX_RD_WR_DLY_ENTRIES);
  //
  // Execute 2d Rd DQS training for all Dcts/Chipselects
  //
  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    IDS_HDT_CONSOLE (MEM_STATUS, "\tDct %d\n", Dct);
    NBPtr->SwitchDCT (NBPtr, Dct);
    NBPtr->Vref = 0xFF;
    Status = FALSE;
    if (NBPtr->FamilySpecificHook[CheckRdWr2DTrainingPerConfig] (NBPtr, NULL)) {
      for (ChipSel = 0; ChipSel < NBPtr->CsPerChannel; ChipSel = ChipSel + NBPtr->CsPerDelay ) {
        if ( (NBPtr->MCTPtr->Status[SbLrdimms]) ? ((NBPtr->ChannelPtr->LrDimmPresent & ((UINT8) 1 << (ChipSel >> 1))) != 0) :
             ((NBPtr->DCTPtr->Timings.CsEnabled & ((UINT16) 1 << ChipSel)) != 0) ) {
          //
          //Initialize storage
          //
          for (Lane = 0; Lane < MemFRdWr2DGetMaxLanes (NBPtr); Lane++) {
            for (Vref = 0; Vref < NBPtr->TotalMaxVrefRange; Vref++) {
              Data.Lane[Lane].Vref[Vref].PosRdWrDly = 0;
              Data.Lane[Lane].Vref[Vref].NegRdWrDly = 0;
            }
          }
          TechPtr->ChipSel = ChipSel;
          IDS_HDT_CONSOLE (MEM_FLOW,"\tChip Select: %02x \n", TechPtr->ChipSel);
          //
          // 1. Sample the data eyes for each channel:
          //
          TechPtr->RdWr2DData = &Data;
          if (NBPtr->FamilySpecificHook[RdWr2DDataCollection] (NBPtr, NULL)) {
            //
            // 2. Process the array of results with a diamond convolution mask, summing the number passing sample points.
            //
            // Determine Diamond Mask Height
            if (MemFRdWr2DHeight (NBPtr, &Data)) {
              //
              // Apply Mask
              //
              if (MemFRdWr2DApplyMask (NBPtr, &Data)) {
                //
                // Convolution
                //
                if (MemFRdWr2DProcessConvolution (NBPtr, &Data)) {
                  //
                  // 3. Program the final DQS delay values.
                  //
                  if (MemFRdWr2DProgramMaxDelays (NBPtr, &Data)) {
                    //
                    // Find the Smallest Positive or Negative Margin for current CS
                    //
                    if (MemFRdWr2DFindCsVrefMargin (NBPtr, &Data)) {
                      Status = TRUE;
                    }
                  }
                }
              }
            }
          }
          if (Status == FALSE) {
            SetMemError (AGESA_ERROR, NBPtr->MCTPtr);
            PutEventLog (AGESA_ERROR, (TechPtr->Direction == DQS_READ_DIR) ? MEM_ERROR_2D_DQS_ERROR : MEM_ERROR_2D_WRDAT_ERROR, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
          }
        }
      }
      //
      // Find the Max and Min Vref values for each DCT
      //
      if (Status == TRUE) {
        if (MemFRdWr2DFinalVrefMargin (NBPtr, &Data)) {
          //
          // Program the Max Vref value
          //
          IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\tProgramming Final Vref for channel\n");
          NBPtr->FamilySpecificHook[RdWr2DProgramVref] (NBPtr, &NBPtr->ChannelPtr->MaxVref);
          IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tActual Vref programmed = %02x\n",
                           ( NBPtr->GetBitField (NBPtr, BFVrefDAC) >> TSEFO_END (NBPtr->NBRegTable[BFVrefDAC])) );
          Status = TRUE;
        } else {
          SetMemError (AGESA_ERROR, NBPtr->MCTPtr);
          PutEventLog (AGESA_ERROR, (TechPtr->Direction == DQS_READ_DIR) ? MEM_ERROR_2D_DQS_VREF_MARGIN_ERROR: MEM_ERROR_2D_WRDAT_VREF_MARGIN_ERROR, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
        }
      }
    }
  }
  //
  // Restore environment settings after training
  //
  if (HeapDeallocateBuffer (AMD_MEM_2D_RD_WR_HANDLE, &MemPtr->StdHeader) != AGESA_SUCCESS) {
    SetMemError (AGESA_FATAL, NBPtr->MCTPtr);
    PutEventLog (AGESA_FATAL, MEM_ERROR_HEAP_DEALLOCATE_FOR_2D, 0, 0, 0, 0, &MemPtr->StdHeader);
  }
  IDS_HDT_CONSOLE (MEM_STATUS, "\tEnd\n");
  MemTEndTraining (TechPtr);

  IDS_HDT_CONSOLE (MEM_FLOW, "\n\nEnd %s 2D training\n\n",(TechPtr->Direction == DQS_READ_DIR) ? "Read DQS":"Write DQ");
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
 *     This function determines whether the current configuration is a valid
 *     config for applying 2D Training
 *     @todo: Update to work for 2D WR and 2D RD training
 *
 *     @param[in,out] *NBPtr    - Pointer to the MEM_NB_BLOCK
 *     @param[in,out] *OptParam - Unused
 *
 *     @return         BOOLEAN
 *                                TRUE  - Configuration valid
 *                                FALSE - Configuration invalid
 *
 */
BOOLEAN
MemFCheckRdWr2DTrainingPerConfig (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  UINT8 i;
  if (NBPtr->RefPtr->ForceTrainMode == FORCE_TRAIN_AUTO) {
    i = 0;
    while (memPlatSpecFlowArray[i] != NULL) {
      if ((memPlatSpecFlowArray[i])->S2D (NBPtr, (memPlatSpecFlowArray[i])->EntryOfTables)) {
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
 *     This function determines the maximum number of lanes for puposes of 2D
 *     Read or Write training.
 *
 *     @param[in,out] *NBPtr - Pointer to the MEM_NB_BLOCK
 *
 *     @return         UINT8 - Max Number of Lanes
 *
 */
UINT8
MemFRdWr2DGetMaxLanes (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  MEM_TECH_BLOCK *TechPtr;
  UINT8 MaxLanes;

  TechPtr = NBPtr->TechPtr;
  if ((TechPtr->Direction == DQS_READ_DIR) && ((NBPtr->ChannelPtr->DimmNibbleAccess & (1 << (TechPtr->ChipSel >> 1))) != 0)) {
    // Per Nibble
    MaxLanes = (NBPtr->MCTPtr->Status[SbEccDimms] && (NBPtr->IsSupported[EccByteTraining] == TRUE)) ? 18 : 16;
  } else {
    // Per Byte
    MaxLanes = (NBPtr->MCTPtr->Status[SbEccDimms] && (NBPtr->IsSupported[EccByteTraining] == TRUE)) ? 9 : 8;
  }
  return MaxLanes;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function programs Vref to internal or external control for 2D Read
 *     or Write Training
 *
 *     @param[in,out] *NBPtr    - Pointer to the MEM_NB_BLOCK
 *     @param[in,out] *OptParam - Unused
 *
 *     @return          BOOLEAN
 *                                TRUE -  External Vref was selected
 *                                FALSE - Internal Vref was selected
 *
 */
BOOLEAN
MemFRdWr2DProgramIntExtVrefSelect (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  if (NBPtr->TechPtr->Direction == DQS_READ_DIR) {
    NBPtr->SetBitField (NBPtr, BFVrefSel, (NBPtr->RefPtr->ExternalVrefCtl ? 0x0000 : 0x0001));
  }
  return NBPtr->RefPtr->ExternalVrefCtl;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function scales Vref from the range used in Data Collection to
 *     the range that is programmed into the register.
 *
 *     @param[in,out] *NBPtr - Pointer to the MEM_NB_BLOCK
 *     @param[in,out] *Vref  - Pointer to UINT8 Vref Value to scale.
 *
 *     @return        BOOLEAN
 *                             TRUE Function was implemented
 *
 */
BOOLEAN
STATIC
MemFRdWr2DScaleVref (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *Vref
  )
{
  *(UINT8*)Vref = *(UINT8*)Vref * 2;
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function programs Vref for 2D Read/Write Training
 *
 *     @param[in,out] *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]     *VrefPtr - Pointer to Vref value
 *
 *     @return        BOOLEAN
 *                               TRUE - Success
 *                               FAIL (External Callout only)
 *
 */
BOOLEAN
MemFRdWr2DProgramVref (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       VOID *VrefPtr
  )
{
  AGESA_STATUS Status;
  MEM_DATA_STRUCT *MemPtr;
  ID_INFO CallOutIdInfo;
  VOLTAGE_ADJUST Va;
  UINT8 Vref;

  ASSERT (NBPtr != NULL);
  ASSERT (VrefPtr != NULL);
  MemPtr = NBPtr->MemPtr;
  Vref = *(UINT8*)VrefPtr;
  CallOutIdInfo.IdField.SocketId = NBPtr->MCTPtr->SocketId;
  CallOutIdInfo.IdField.ModuleId = NBPtr->MCTPtr->DieId;
  LibAmdMemCopy ((VOID *)&Va, (VOID *)MemPtr, (UINTN)sizeof (Va.StdHeader), &MemPtr->StdHeader);
  Va.MemData = MemPtr;
  Status = AGESA_SUCCESS;
  if (NBPtr->TechPtr->Direction == DQS_READ_DIR) {
    if (NBPtr->RefPtr->ExternalVrefCtl == FALSE) {
      //
      // Internal vref control
      //
      // This is 1/2 VrefDAC value Sign bit is shifted into place.
      //
      ASSERT (Vref < 32);
      if (Vref < 15) {
        Vref = (31 - Vref) << 1;
      } else {
        Vref = (Vref - 15) << 1;
      }
      NBPtr->SetBitField (NBPtr, BFVrefDAC, Vref << 2);
    } else {
      // External vref control
      AGESA_TESTPOINT (TpProcMemBefore2dTrainExtVrefChange, &(NBPtr->MemPtr->StdHeader));
      NBPtr->MemPtr->ParameterListPtr->ExternalVrefValue = Vref;
      IDS_HDT_CONSOLE (MEM_FLOW, "\n2D Read Training External CPU Vref Callout \n");
      Va.VoltageType = VTYPE_CPU_VREF;
      Va.AdjustValue = Vref = (Vref - 15) << 1;
      Status = AgesaExternalVoltageAdjust ((UINTN)CallOutIdInfo.IdInformation, &Va);
      AGESA_TESTPOINT (TpProcMemAfter2dTrainExtVrefChange, &(NBPtr->MemPtr->StdHeader));
    }
  } else {
    //
    // DIMM Vref Control
    //
    Va.VoltageType = VTYPE_DIMM_VREF;
    //
    // Offset by 15 and multiply by 2.
    //
    Va.AdjustValue = Vref = (Vref - 15) << 1;
    Status = AgesaExternalVoltageAdjust ((UINTN)CallOutIdInfo.IdInformation, &Va);
    if (Status != AGESA_SUCCESS) {
      IDS_HDT_CONSOLE (MEM_FLOW, "* Dimm Vref Callout Failed *");
    }
    IDS_HDT_CONSOLE (MEM_FLOW, "\n");
  }
  return (Status == AGESA_SUCCESS) ? TRUE : FALSE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function programs Read DQS or Write DQ Delay values for Read/Write
 *     Training
 *
 *     @param[in,out] *NBPtr - Pointer to the MEM_NB_BLOCK
 *     @param[in]     *Delay - Pointer to UINT8 containing Delay value
 *
 *     @return       BOOLEAN
 *                             TRUE
 *
 */

BOOLEAN
MemFRdWr2DProgramDelays (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       VOID *Delay
  )
{
  UINT32 RdDqsTime;
  UINT8  RdWrDly;

  ASSERT (NBPtr != 0);
  ASSERT (Delay != 0);
  RdWrDly = *(UINT8*) Delay;
  if (NBPtr->TechPtr->Direction == DQS_READ_DIR) {
    // This function should only be used for read training
    ASSERT (NBPtr->TechPtr->Direction == DQS_READ_DIR);
    // Program BL registers for both nibble (x4) and bytes (x8, x16)
    RdDqsTime = 0;
    RdDqsTime = (RdWrDly & 0x1F) << 8;
    RdDqsTime = RdDqsTime | (RdWrDly & 0x1F);
    if ((NBPtr->TechPtr->ChipSel / NBPtr->CsPerDelay) == 0) {
      NBPtr->SetBitField (NBPtr, BFDataByteRxDqsDLLDimm0Broadcast, RdDqsTime);
    } else if ((NBPtr->TechPtr->ChipSel / NBPtr->CsPerDelay) == 1) {
      NBPtr->SetBitField (NBPtr, BFDataByteRxDqsDLLDimm1Broadcast, RdDqsTime);
    } else if ((NBPtr->TechPtr->ChipSel / NBPtr->CsPerDelay) == 2) {
      NBPtr->SetBitField (NBPtr, BFDataByteRxDqsDLLDimm2Broadcast, RdDqsTime);
    } else if ((NBPtr->TechPtr->ChipSel / NBPtr->CsPerDelay) == 3) {
      NBPtr->SetBitField (NBPtr, BFDataByteRxDqsDLLDimm3Broadcast, RdDqsTime);
    }
  } else {
    MemTSetDQSDelayAllCSR (NBPtr->TechPtr, RdWrDly);
  }
  return TRUE;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function stores data for 2D Read DQS and Write DQ Training
 *
 *     @param[in,out] *NBPtr            - Pointer to the MEM_NB_BLOCK
 *     @param[in]     *Data             - Pointer to Result data structure
 *     @param[in]     *InPhaseResult[]  - Array of inphase results
 *     @param[in]     *PhaseResult180[] - Array of Phase 180 results
 *
 *     @return        BOOLEAN
 *                                       TRUE - No Errors occurred
 *                                       FALSE - Errors ccurred
 */
VOID
MemFRdWr2DStoreResult (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_RD_WR_2D_ENTRY *Data,
  IN       UINT32 InPhaseResult[],
  IN       UINT32 PhaseResult180[]
  )
{
  UINT8 Lane;
  UINT8 Vref;
  UINT8 RdWrDly;
  UINT32 Result;
  UINT32 Result180;
  UINT8 Index;
  Vref = NBPtr->Vref;
  RdWrDly = Data->RdWrDly;
  for (Lane = 0; Lane < MemFRdWr2DGetMaxLanes (NBPtr); Lane++) {
    for (RdWrDly = 0; RdWrDly < Data->MaxRdWrSweep; RdWrDly++) {
      if ((NBPtr->ChannelPtr->DimmNibbleAccess & (1 << (NBPtr->TechPtr->ChipSel >> 1))) == 0) {
        // x8, so combine "Nibble X" and "Nibble X+1" results
        Index = Lane * 2;
        Result = (InPhaseResult[RdWrDly] >> Index) & 0x03;
        Result180 = (PhaseResult180[RdWrDly] >> Index) & 0x03;
      } else {
        // x4, so use "Nibble" results
        Result = (InPhaseResult[RdWrDly] >> Lane) & 0x01;
        Result180 = (PhaseResult180[RdWrDly] >> Lane) & 0x01;
      }
      Data->Lane[Lane].Vref[Vref].PosRdWrDly |= (Result == 0) ? (1 << (Data->MaxRdWrSweep - 1 - RdWrDly)) : 0;
      Data->Lane[Lane].Vref[Vref].NegRdWrDly |= (Result180 == 0) ? (1 << (Data->MaxRdWrSweep - 1 - RdWrDly)) : 0;
    }
  }
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function determines the height of data for 2D Read and Write training
 *
 *     @param[in,out] *NBPtr - Pointer to the MEM_NB_BLOCK
 *     @param[in]     *Data  - Pointer to Result data structure
 *
 *     @return         BOOLEAN
 *                             TRUE - No Errors occurred
 */
BOOLEAN
MemFRdWr2DHeight (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_RD_WR_2D_ENTRY *Data
  )
{
  UINT8 Lane;
  for (Lane = 0; Lane < MemFRdWr2DGetMaxLanes (NBPtr); Lane++) {
    Data->Lane[Lane].HalfDiamondHeight = 0x0F;
  }
  IDS_HDT_CONSOLE_DEBUG_CODE (
    IDS_HDT_CONSOLE (MEM_FLOW, "\n");
    IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\t  Lane: ");
    for (Lane = 0; Lane < MemFRdWr2DGetMaxLanes (NBPtr); Lane++) {
      IDS_HDT_CONSOLE (MEM_FLOW, "%02x ", Lane);
    }
    IDS_HDT_CONSOLE (MEM_FLOW, "\n");
    IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tHeight: ");
    for (Lane = 0; Lane < MemFRdWr2DGetMaxLanes (NBPtr); Lane++) {
      IDS_HDT_CONSOLE (MEM_FLOW, "%02x ", (2*(Data->Lane[Lane].HalfDiamondHeight) + 1));
    }
    IDS_HDT_CONSOLE (MEM_FLOW, "\n");
  );
  return TRUE;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function gets the width for 2D RdDQS and WrDat training
 *
 *     @param[in,out] *NBPtr - Pointer to the MEM_NB_BLOCK
 *     @param[in]     *Data  - Pointer to Result data structure
 *
 *     @return         UINT8   Width
 */
UINT8
MemFGetRdWr2DWidth (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_RD_WR_2D_ENTRY *Data
  )
{
  if (NBPtr->TechPtr->Direction == DQS_READ_DIR) {
    return NBPtr->DiamondWidthRd;
  } else {
    return NBPtr->DiamondWidthWr;
  }
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function gets the step height for the diamond mask for 2D RdDQS or
 *       WrDat Training
 *
 *     @param[in,out] *NBPtr - Pointer to the MEM_NB_BLOCK
 *     @param[in]     *Data  - Pointer to Result data structure
 *     @param[in]      Vref  - current Vref value
 *     @param[in]      Lane  - current Lane
 *
 *     @return      BOOLEAN
 *                             TRUE - Step found and value should be updated
 *                             FALSE - Step not found and value should not be updated
 *
 */
BOOLEAN
MemFCheckRdWr2DDiamondMaskStep (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_RD_WR_2D_ENTRY *Data,
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
  width = (MemFGetRdWr2DWidth (NBPtr, Data) - 1) / 2;
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
 *     This function applies a mask for 2D RdDQS or WrDat training
 *
 *     @param[in,out] *NBPtr - Pointer to the MEM_NB_BLOCK
 *     @param[in]     *Data  - Pointer to Result data structure
 *
 *     @return  BOOLEAN
 *                             TRUE - No Errors occurred
 *                             FALSE - Errors ccurred
 *
 */
BOOLEAN
MemFRdWr2DApplyMask (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_RD_WR_2D_ENTRY *Data
  )
{
  MEM_TECH_BLOCK *TechPtr;
  UINT8 RdWrDly;
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
  UINT8 TotalDlyRange;
  BOOLEAN maxHeightExceeded;
  BOOLEAN negVrefComplete;
  BOOLEAN PosRdWrToNegRdWr;
  BOOLEAN NegRdWrToPosRdWr;

  TechPtr = NBPtr->TechPtr;
  TotalDlyRange = (TechPtr->Direction == DQS_READ_DIR) ? NBPtr->TotalRdDQSDlyRange : NBPtr->TotalWrDatDlyRange;
  //
  // Initialize Convolution
  //
  for (Lane = 0; Lane < MemFRdWr2DGetMaxLanes (NBPtr); Lane++) {
    for (RdWrDly = 0; RdWrDly < TotalDlyRange; RdWrDly++) {
      Data->Lane[Lane].Convolution[RdWrDly] = 0;
      NBPtr->FamilySpecificHook[Adjust2DDelayStepSize] (NBPtr, &RdWrDly);
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
  Width = MemFGetRdWr2DWidth (NBPtr, Data);
  IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\t\tWidth: %02x\n", Width);
  ASSERT (Width != 0);

  IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\tExecuting convolution function\n");
  //
  // Perform the convolution by sweeping the mask function centered at nominal Vref. Results in a one
  // dimensional array with FOM values at each delay for each lane. Choose the delay setting at the peak
  // FOM value.
  //
  for (Lane = 0; Lane < MemFRdWr2DGetMaxLanes (NBPtr); Lane++) {
    Height = Data->Lane[Lane].HalfDiamondHeight;
    ASSERT (Height < Data->Vnom);
    //
    // RdWrDly is divided around "Data->MaxRdWrSweep" into positive and negative directions
    // Positive direction -> RdWrDly = 0 to (Data->MaxRdWrSweep - 1)
    // Negative direction -> RdWrDly = Data->MaxRdWrSweep to (TotalDlyRange - 1)
    //
    for (RdWrDly = 0; RdWrDly < TotalDlyRange; RdWrDly++) {
      // Vref loop is divided around "Data->Vnom - 1" into positive and negative directions
      // Negative direction -> Vref = 0 ("Data->Vnom - 1") to Height("Data->Vnom - 1" - Height)
      // Positive direction -> Vref = "Data->Vnom" to Height("Data->Vnom" + Height)
      //
      negVrefComplete = FALSE;
      PosRdWrToNegRdWr = FALSE;
      NegRdWrToPosRdWr = FALSE;
      for (Vref = 0; Vref < (NBPtr->TotalMaxVrefRange - 1); Vref++) {
        // Initial negative direction where Vref = 0 ("Data->Vnom - 1"), so we need to set
        // initial startWidth and endWidth for +/- RdDqs
        //
        // Create common delay based on +/- RdDqs
        if (RdWrDly > (Data->MaxRdWrSweep - 1)) {
          Dly = RdWrDly - Data->MaxRdWrSweep;
        } else {
          Dly = RdWrDly;
        }
        if (Vref == 0 ) {
          // Initialize -Vref
          maxHeightExceeded = FALSE; // reset for start of -Vref
          // Case 1: if +RdDqs - Check for lower bound (Width/2 > RdDqs > 0)
          //       : if -RdDqs - Check for lower bound (Width/2 + Data->MaxRdDqsSweep > RdDqs > Data->MaxRdDqsSweep)
          if (Dly < Width / 2) {
            endWidth = Dly + Width / 2 + 1;
            startWidth = 0;
          } else if ((Dly + Width / 2) > (Data->MaxRdWrSweep - 1)) {
            // Case 2: if +RdWr - Check for upper bound ((Data->MaxRdWrSweep - 1) < RdWr < ((Data->MaxRdWrSweep - 1) - Width/2))
            //       : if -RdWr - Check for lower bound ((DatNBPtr->TotalRdWrDlyRange - 1) < RdWr < ((NBPtr->TotalRdWrDlyRange - 1) - Width/2))
            endWidth = Data->MaxRdWrSweep;
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
            if (RdWrDly == (TotalDlyRange - 1)) {
              // Special condition for end of -RdDqs range
              startWidth = Data->MaxRdWrSweep - 1;
              endWidth = Data->MaxRdWrSweep;
            } else {
             // Width = 0, but Height not reached,
              startWidth = Dly;
              endWidth = Dly + 1;
            }
          } else {
            // Check for Case 1 and Case 2 above
            if ((RdWrDly + Width / 2) > (TotalDlyRange - 1)) {
              endWidth = origEndWidth;
            }
          }
          maxHeightExceeded = FALSE;
        }
        IDS_HDT_CONSOLE_DEBUG_CODE (
          if (Lane == 0) {
            if (RdWrDly == (Data->MaxRdWrSweep - (Width / 2)) ) {
              Data->DiamondLeft[Vref] = startWidth;
              Data->DiamondRight[Vref] = endWidth - 1;
            }
          }
        );
        //
        // Determine the correct Delay (+/-) and Vref (+/-)direction
        //
        if (maxHeightExceeded == FALSE) {
          if (RdWrDly < Data->MaxRdWrSweep) {
            if (Vref > (Data->Vnom - 1)) {
              PosNegData = Data->Lane[Lane].Vref[Vref].PosRdWrDly; // +RdWr Dly, +Vref
            } else {
              PosNegData = Data->Lane[Lane].Vref[(Data->Vnom - 1) - Vref].PosRdWrDly; // +RdDqs Dly, -Vref
            }
          } else {
            if (Vref > (Data->Vnom - 1)) {
              PosNegData = Data->Lane[Lane].Vref[Vref].NegRdWrDly; // -RdWr Dly, +Vref
            } else {
              PosNegData = Data->Lane[Lane].Vref[(Data->Vnom - 1) - Vref].NegRdWrDly; // -RdWr Dly, -Vref
            }
          }
          //
          // Case 1: Non-overlap condition:
          //         Count the number of passes from "startWidth" to "endWidth"
          //
          for (count = startWidth; count < endWidth; count++) {
            Data->Lane[Lane].Convolution[RdWrDly] = (UINT8) ((PosNegData >> count) & 0x1) + Data->Lane[Lane].Convolution[RdWrDly];
          }
          // Case 2: Overlay between +RdWr and -RdWr starting from +RdWr
          //         Count the number of passes from "startWidth" to "endWidth"
          //
          if ((RdWrDly <= (Data->MaxRdWrSweep - 1) && (RdWrDly > ((Data->MaxRdWrSweep - 1) - Width / 2)))) {
            startOverLapWidth = 0;
            if (Vref == 0 || Vref == Data->Vnom) {
              maxOverLapWidth = (RdWrDly + Width / 2) - (Data->MaxRdWrSweep - 1); // Initial overlap max width size
            } else if (maxOverLapWidth == 0) {
              maxOverLapWidth = startOverLapWidth; // Stop counting after overlap region complete
            }
            // Ensure that +/- vref is set correctly
            if (Vref > (Data->Vnom - 1)) {
              PosNegData = Data->Lane[Lane].Vref[Vref].NegRdWrDly;
            } else {
              PosNegData = Data->Lane[Lane].Vref[(Data->Vnom - 1) - Vref].NegRdWrDly;
            }
            // Need to count the number of passes when range extends from Pos RdDqs to Neg RdDqs
            for (count = startOverLapWidth; count < maxOverLapWidth; count++) {
              Data->Lane[Lane].Convolution[RdWrDly] = (UINT8) ((PosNegData >> count) & 0x1) + Data->Lane[Lane].Convolution[RdWrDly];
            }
            if (maxOverLapWidth > 0) {
              if (MemFCheckRdWr2DDiamondMaskStep (NBPtr, Data, Vref, Lane) || (Vref == 1) || (Vref == Data->Vnom)) {
                maxOverLapWidth--;  // Reduce overlap width outside of diamond mask
              }
              PosRdWrToNegRdWr = TRUE;
            }
          }
          if (((RdWrDly - Data->MaxRdWrSweep) < Width / 2) && (RdWrDly > (Data->MaxRdWrSweep - 1))) {
            //
            // Case 3: Overlay between -RdDqs and +RdDqs starting from -RdDqs
            //         Count the number of passes from "startWidth" to "endWidth"
            //
            maxOverLapWidth = Data->MaxRdWrSweep;
            if (Vref == 0 || Vref == Data->Vnom) {
              startOverLapWidth = RdWrDly - Width / 2; // Initial overlap start point
            } else if (startOverLapWidth > maxOverLapWidth) {
              maxOverLapWidth = maxOverLapWidth - 1; // Continue to count until MaxHeight excceded
            }
            // Ensure that vref + or - is set correctly
            if (Vref > (Data->Vnom - 1)) {
              PosNegData = Data->Lane[Lane].Vref[Vref].PosRdWrDly;
            } else {
              PosNegData = Data->Lane[Lane].Vref[(Data->Vnom - 1) - Vref].PosRdWrDly;
            }
            // Need to count the number of passes when range extends from Pos RdDqs to Neg RdDqs
            for (count = startOverLapWidth; count < maxOverLapWidth; count++) {
              Data->Lane[Lane].Convolution[RdWrDly] = (UINT8) ((PosNegData >> count) & 0x1) + Data->Lane[Lane].Convolution[RdWrDly];
            }
            if (startOverLapWidth < maxOverLapWidth) {
              if (MemFCheckRdWr2DDiamondMaskStep (NBPtr, Data, Vref, Lane) || (Vref == 1) || (Vref == Data->Vnom)) {
                startOverLapWidth++;  // Reduce overlap width outside of diamond mask
              }
              NegRdWrToPosRdWr = TRUE;
            }
          }
        }
        if (MemFCheckRdWr2DDiamondMaskStep (NBPtr, Data, Vref, Lane) || (Vref == 1) || (Vref == Data->Vnom)) {
          if (PosRdWrToNegRdWr) {
            startWidth++;
            endWidth = Data->MaxRdWrSweep;
            PosRdWrToNegRdWr = FALSE;
          } else if (NegRdWrToPosRdWr) {
            startWidth = 0;
            endWidth--;
            NegRdWrToPosRdWr = FALSE;
          } else {
            startWidth++;
            endWidth--;
          }
        }
        NBPtr->FamilySpecificHook[Adjust2DVrefStepSize] (NBPtr, &Vref);
      }
      NBPtr->FamilySpecificHook[Adjust2DDelayStepSize] (NBPtr, &RdWrDly);
    }
  }
  IDS_HDT_CONSOLE_DEBUG_CODE (
    IDS_HDT_CONSOLE (MEM_FLOW, "\n");
    IDS_HDT_CONSOLE (MEM_FLOW, "\t\t   Diamond Shape: \n");
    for (Vref = 0; Vref < (NBPtr->TotalMaxVrefRange - 1); Vref++) {
      IDS_HDT_CONSOLE (MEM_FLOW, "\n");
      for (RdWrDly = (Data->MaxRdWrSweep - Width); RdWrDly < Data->MaxRdWrSweep; RdWrDly++) {
        if (Vref < (Data->Vnom - 1)) {
          if (RdWrDly == Data->DiamondLeft[(NBPtr->TotalMaxVrefRange - 2) - Vref]) {
            IDS_HDT_CONSOLE (MEM_FLOW, " | ");
          } else if (RdWrDly == Data->DiamondRight[(NBPtr->TotalMaxVrefRange - 2) - Vref]) {
            IDS_HDT_CONSOLE (MEM_FLOW, " | -> Width = %02x", (Data->DiamondRight[(NBPtr->TotalMaxVrefRange - 2) - Vref]) - (Data->DiamondLeft[(NBPtr->TotalMaxVrefRange - 2) - Vref]));
          } else {
            IDS_HDT_CONSOLE (MEM_FLOW, " ");
          }
        } else {
          if (RdWrDly == Data->DiamondLeft[Vref - (Data->Vnom - 1)]) {
            IDS_HDT_CONSOLE (MEM_FLOW, " | ");
          } else if (RdWrDly == Data->DiamondRight[Vref - (Data->Vnom - 1)]) {
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
    for (RdWrDly = 0; RdWrDly < TotalDlyRange; RdWrDly++) {
      IDS_HDT_CONSOLE (MEM_FLOW, " %02x ", RdWrDly <= (Data->MaxRdWrSweep - 1) ? (Data->MaxRdWrSweep - 1) - RdWrDly :  (TotalDlyRange - 1) - RdWrDly);
    }
    IDS_HDT_CONSOLE (MEM_FLOW, "\n");
    for (Lane = 0; Lane < MemFRdWr2DGetMaxLanes (NBPtr); Lane++) {
      IDS_HDT_CONSOLE (MEM_FLOW, "\t\tLane: %02x\n", Lane);
      IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tConv: ");
      for (RdWrDly = 0; RdWrDly < TotalDlyRange; RdWrDly++) {
        IDS_HDT_CONSOLE (MEM_FLOW, "%03x ", Data->Lane[Lane].Convolution[RdWrDly]);
      }
      IDS_HDT_CONSOLE (MEM_FLOW, "\n");
    }
  );
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function Examines the convolution function and determines the Max Delay
 *     for 2D RdDQS and WrDat training
 *
 *     @param[in,out] *NBPtr - Pointer to the MEM_NB_BLOCK
 *     @param[in]     *Data  - Pointer to Result data structure
 *
 *     @return  BOOLEAN
 *                             TRUE - No Errors occurred
 *                             FALSE - Errors ccurred
 */

BOOLEAN
MemFRdWr2DProcessConvolution (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_RD_WR_2D_ENTRY *Data
  )
{
  MEM_TECH_BLOCK *TechPtr;
  UINT8 RdWrDly;
  UINT8 Lane;
  UINT16 MaxFOM;
  UINT8 MaxRange;
  UINT8 CurrRange;
  UINT8 TotalDlyRange;
  BOOLEAN status;

  ASSERT (NBPtr != NULL);
  TechPtr = NBPtr->TechPtr;
  TotalDlyRange = (TechPtr->Direction == DQS_READ_DIR) ? NBPtr->TotalRdDQSDlyRange : NBPtr->TotalWrDatDlyRange;
  status = TRUE;
  IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\tDetermining Delay based on Convolution function\n");
  // Determine the Max RdDqs or WrDat Dly for the convolution function
  //   - Choose the delay setting at the peak FOM value.
  for (Lane = 0; Lane < MemFRdWr2DGetMaxLanes (NBPtr); Lane++) {
    // Find largest value as MaxFOM
    MaxFOM = 0;
    for (RdWrDly = 0; RdWrDly < TotalDlyRange; RdWrDly++) {
      if (Data->Lane[Lane].Convolution[RdWrDly] > MaxFOM) {
        MaxFOM = Data->Lane[Lane].Convolution[RdWrDly];
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
    for (RdWrDly = 0; (MaxFOM > 0) && RdWrDly < TotalDlyRange; RdWrDly++) {
      if (Data->Lane[Lane].Convolution[RdWrDly] == MaxFOM) {
        CurrRange++;
        if (CurrRange >= MaxRange) {
          Data->Lane[Lane].MaxRdWrDly = RdWrDly - ((CurrRange - 1) / 2);
          MaxRange = CurrRange;
        }
      } else {
        CurrRange = 0;
      }
    }

    if (Data->Lane[Lane].MaxRdWrDly > Data->MaxRdWrSweep) {
      status = FALSE; // Error
    }
    // Set Actual register value
    if (Data->Lane[Lane].MaxRdWrDly < Data->MaxRdWrSweep) {
      Data->Lane[Lane].MaxRdWrDly = (Data->MaxRdWrSweep - 1) - Data->Lane[Lane].MaxRdWrDly;
    } else {
      status = FALSE; // Error
    }
  }

  IDS_HDT_CONSOLE_DEBUG_CODE (
    IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\t\t              Cs %d Lane: ", TechPtr->ChipSel);
    for (Lane = 0; Lane < MemFRdWr2DGetMaxLanes (NBPtr); Lane++) {
      IDS_HDT_CONSOLE (MEM_FLOW, "%02x ", Lane);
    }
    IDS_HDT_CONSOLE (MEM_FLOW, "\n");
    IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\t       Max %s Delay: ", TechPtr->Direction == DQS_READ_DIR ? "Rd Dqs" : "Wr DQ");
    for (Lane = 0; Lane < MemFRdWr2DGetMaxLanes (NBPtr); Lane++) {
      IDS_HDT_CONSOLE (MEM_FLOW, "%02x ", Data->Lane[Lane].MaxRdWrDly);
    }
    IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\t\t1D Trained %s Delay: ", TechPtr->Direction == DQS_READ_DIR ? "Rd Dqs" : "Wr DQ");
    for (Lane = 0; Lane < MemFRdWr2DGetMaxLanes (NBPtr); Lane++) {
      if (TechPtr->Direction == DQS_READ_DIR) {
        IDS_HDT_CONSOLE (MEM_FLOW, "%02x ", NBPtr->ChannelPtr->RdDqsDlys[(TechPtr->ChipSel / NBPtr->CsPerDelay) * MAX_DELAYS + Lane]);
      } else {
        IDS_HDT_CONSOLE (MEM_FLOW, "%02x ", (NBPtr->ChannelPtr->WrDatDlys[(TechPtr->ChipSel / NBPtr->CsPerDelay) * MAX_DELAYS + Lane] - \
                                             NBPtr->ChannelPtr->WrDqsDlys[(TechPtr->ChipSel / NBPtr->CsPerDelay) * MAX_DELAYS + Lane]));
      }
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
 *     This function programs the Max Rd Dqs or Max Wr DQ for 2D training from
 *     convolution
 *
 *     @param[in,out] *NBPtr - Pointer to the MEM_NB_BLOCK
 *     @param[in]     *Data  - Pointer to Result data structure
 *
 *     @return  BOOLEAN
 *                             TRUE - No Errors occurred
 *                             FALSE - Errors ccurred
 */
BOOLEAN
MemFRdWr2DProgramMaxDelays (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_RD_WR_2D_ENTRY *Data
  )
{
  MEM_TECH_BLOCK *TechPtr;
  UINT8 Lane;
  UINT8 LaneHighRdDqs2dDlys;
  UINT8 LaneLowRdDqs2dDlys;
  UINT8 MaxWrDatDly;
  TechPtr = NBPtr->TechPtr;
  IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\tProgramming Max %s Delay per Lane\n\n", TechPtr->Direction == DQS_READ_DIR ? "Rd Dqs" : "Wr DQ");
  for (Lane = 0; Lane < MemFRdWr2DGetMaxLanes (NBPtr); Lane++) {
    if ( TechPtr->Direction == DQS_WRITE_DIR || (NBPtr->ChannelPtr->DimmNibbleAccess & (1 << (TechPtr->ChipSel >> 1))) == 0) {
      // Program Byte based for x8 and x16
      if ( TechPtr->Direction == DQS_READ_DIR) {
        //
        // Read DQS Training
        //
        NBPtr->SetTrainDly (NBPtr, AccessRdDqsDly, DIMM_BYTE_ACCESS ((TechPtr->ChipSel / NBPtr->CsPerDelay), Lane), (UINT16)Data->Lane[Lane].MaxRdWrDly);
        NBPtr->ChannelPtr->RdDqsDlys[(TechPtr->ChipSel / NBPtr->CsPerDelay) * MAX_DELAYS + Lane] = Data->Lane[Lane].MaxRdWrDly;
      } else {
        //
        // Write DQ Training
        //
        MaxWrDatDly = (UINT8) (Data->Lane[Lane].MaxRdWrDly + TechPtr->NBPtr->ChannelPtr->WrDqsDlys[(TechPtr->ChipSel / TechPtr->NBPtr->CsPerDelay) * MAX_DELAYS + Lane]);
        NBPtr->SetTrainDly (NBPtr, AccessWrDatDly, DIMM_BYTE_ACCESS ((TechPtr->ChipSel / NBPtr->CsPerDelay), Lane), MaxWrDatDly);
        NBPtr->ChannelPtr->WrDatDlys[(TechPtr->ChipSel / NBPtr->CsPerDelay) * MAX_DELAYS + Lane] = MaxWrDatDly;
      }
    } else {
      ASSERT (TechPtr->Direction == DQS_READ_DIR);
      // Program nibble based x4, so use "Nibble"
      NBPtr->SetTrainDly (NBPtr, AccessRdDqs2dDly, DIMM_NBBL_ACCESS ((TechPtr->ChipSel / NBPtr->CsPerDelay), Lane), (UINT16)Data->Lane[Lane].MaxRdWrDly);
      NBPtr->ChannelPtr->RdDqs2dDlys[(TechPtr->ChipSel / NBPtr->CsPerDelay) * MAX_NUMBER_LANES + Lane] = Data->Lane[Lane].MaxRdWrDly;
      // For each pair of nibbles (high (Odd Nibble) and Low (Even nibble)), find the largest and use that as the RdDqsDly value
      if ((Lane & 0x1) == 0) {
        LaneHighRdDqs2dDlys = Data->Lane[Lane + 1].MaxRdWrDly;
        LaneLowRdDqs2dDlys = Data->Lane[Lane].MaxRdWrDly;
        if (LaneHighRdDqs2dDlys > LaneLowRdDqs2dDlys) {
          NBPtr->ChannelPtr->RdDqsDlys[(TechPtr->ChipSel / NBPtr->CsPerDelay) * MAX_DELAYS + (Lane >> 1)] = LaneHighRdDqs2dDlys;
        } else {
          NBPtr->ChannelPtr->RdDqsDlys[(TechPtr->ChipSel / NBPtr->CsPerDelay) * MAX_DELAYS + (Lane >> 1)] = LaneLowRdDqs2dDlys;
        }
      }
      NBPtr->DctCachePtr->Is2Dx4 = TRUE;
    }
  }
  return TRUE;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function finds the Positive and negative Vref Margin for the current CS
 *     for 2D RdDQS or WrDat training
 *
 *     @param[in,out] *NBPtr - Pointer to the MEM_NB_BLOCK
 *     @param[in]     *Data  - Pointer to Result data structure
 *
 *     @return  BOOLEAN
 *                             TRUE - No Errors occurred
 *                             FALSE - Errors ccurred
 */
BOOLEAN
MemFRdWr2DFindCsVrefMargin (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_RD_WR_2D_ENTRY *Data
  )
{
  UINT8 SmallestMaxVrefNeg;
  UINT8 Lane;
  UINT8 RdWrDly;
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
  for (Lane = 0; Lane < MemFRdWr2DGetMaxLanes (NBPtr); Lane++) {
    RdWrDly = (Data->MaxRdWrSweep - 1) - Data->Lane[Lane].MaxRdWrDly;
    for (Vref = 0; Vref < (Data->Vnom - 1); Vref++) {
      // Neg Vref - (searching from top of array down)
      PosNegData = Data->Lane[Lane].Vref[Vref].PosRdWrDly;
      if ((UINT8) ((PosNegData >> RdWrDly) & 0x1) == 1) {
        MaxVrefNegative = Vref;
        break;
      }
      NBPtr->FamilySpecificHook[Adjust2DVrefStepSize] (NBPtr, &Vref);
    }
    for (Vref = (Data->Vnom - 1); Vref < (NBPtr->TotalMaxVrefRange - 1); Vref++) {
      // Pos Vref - (searching from Vnom + 1 of array down)
      PosNegData = Data->Lane[Lane].Vref[Vref].PosRdWrDly;
      if ((UINT8) ((PosNegData >> RdWrDly) & 0x1) == 0) {
        // Convert to register setting
        MaxVrefPositive = Vref - 1;// - Data->Vnom;
        break;
      } else {
        // If Vref = 1F passes, then smallest Vref = 0x1F
        if (Vref == ((NBPtr->TotalMaxVrefRange - 1) - 1)) {
          MaxVrefPositive = 0x1E;
          break;
        }
      }
      NBPtr->FamilySpecificHook[Adjust2DVrefStepSize] (NBPtr, &Vref);
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
    Data->SmallestPosMaxVrefperCS[NBPtr->TechPtr->ChipSel] = SmallestMaxVrefPos - Data->Vnom + 1;
  } else {
    Data->SmallestPosMaxVrefperCS[NBPtr->TechPtr->ChipSel] = 0;
  }
  Data->SmallestNegMaxVrefperCS[NBPtr->TechPtr->ChipSel] = (Data->Vnom - 1) - SmallestMaxVrefNeg;
  IDS_HDT_CONSOLE_DEBUG_CODE (
    IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tSmallest Max Positive Vref Offset from V-Nom for ChipSel %02x = + %02x\n", NBPtr->TechPtr->ChipSel, Data->SmallestPosMaxVrefperCS[NBPtr->TechPtr->ChipSel]);
    if (Data->SmallestPosMaxVrefperCS[NBPtr->TechPtr->ChipSel] == 0) {
      IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tSmallest Max Negative Vref Offset from V-Nom for ChipSel %02x = 00\n");
    } else {
      IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tSmallest Max Negative Vref Offset from V-Nom for ChipSel %02x = - %02x\n", NBPtr->TechPtr->ChipSel, Data->SmallestNegMaxVrefperCS[NBPtr->TechPtr->ChipSel]);
    }
  );
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function finds the final Vref Margin for 2D RdDQS or WrDat training
 *
 *     @param[in,out] *NBPtr - Pointer to the MEM_NB_BLOCK
 *     @param[in]     *Data  - Pointer to Result data structure
 *
 *     @return  BOOLEAN
 *                             TRUE - No Errors occurred
 *                             FALSE - Errors ccurred
 */
BOOLEAN
MemFRdWr2DFinalVrefMargin (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_RD_WR_2D_ENTRY *Data
  )
{
  UINT8 ChipSel;
  UINT8 SmallestMaxPosVref;
  UINT8 SmallestMaxNegVref;
  UINT8 OffsetFromVref;
  UINT8 Vnom;
  SmallestMaxNegVref = 0x7F;
  SmallestMaxPosVref = 0x7F;
  Vnom = (Data->Vnom - 1);
  IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\tFinding Final Vref for channel\n\n");
  for (ChipSel = 0; ChipSel < NBPtr->CsPerChannel; ChipSel = ChipSel + NBPtr->CsPerDelay ) {
    if ( (NBPtr->MCTPtr->Status[SbLrdimms]) ? ((NBPtr->ChannelPtr->LrDimmPresent & ((UINT8) 1 << (ChipSel >> 1))) != 0) :
         ((NBPtr->DCTPtr->Timings.CsEnabled & ((UINT16) 1 << ChipSel)) != 0) ) {
      if (Data->SmallestPosMaxVrefperCS[ChipSel] < SmallestMaxPosVref) {
        SmallestMaxPosVref = Data->SmallestPosMaxVrefperCS[ChipSel];
      }
      if (Data->SmallestNegMaxVrefperCS[ChipSel] < SmallestMaxNegVref) {
        SmallestMaxNegVref = Data->SmallestNegMaxVrefperCS[ChipSel];
      }
    }
  }
  //
  // Synchronize minimum and Maximums with other Vref values
  //
  if (NBPtr->TechPtr->Direction == DQS_WRITE_DIR) {
    if (NBPtr->SharedPtr->CommonSmallestMaxNegVref < SmallestMaxNegVref) {
      SmallestMaxNegVref = NBPtr->SharedPtr->CommonSmallestMaxNegVref;
    } else {
      NBPtr->SharedPtr->CommonSmallestMaxNegVref = SmallestMaxNegVref;
    }
    if (NBPtr->SharedPtr->CommonSmallestMaxPosVref < SmallestMaxPosVref) {
      SmallestMaxPosVref = NBPtr->SharedPtr->CommonSmallestMaxPosVref;
    } else {
      NBPtr->SharedPtr->CommonSmallestMaxPosVref = SmallestMaxPosVref;
    }
  }
  NBPtr->FamilySpecificHook[RdWr2DScaleVref] (NBPtr, &SmallestMaxPosVref);
  NBPtr->FamilySpecificHook[RdWr2DScaleVref] (NBPtr, &SmallestMaxNegVref);
  NBPtr->FamilySpecificHook[RdWr2DScaleVref] (NBPtr, &Vnom);

  IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tScaled Smallest Max Positive = + %02x\n", SmallestMaxPosVref);
  IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tScaled Smallest Max Negative =%s%02x\n", ((SmallestMaxNegVref != 0) ? " - " : " "), SmallestMaxNegVref);
  IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tScaled Vnom = %02x\n", Vnom);

  if (SmallestMaxPosVref > SmallestMaxNegVref) {
    OffsetFromVref = (SmallestMaxPosVref - SmallestMaxNegVref) / 2;
    NBPtr->ChannelPtr->MaxVref = Vnom + OffsetFromVref;
  } else {
    OffsetFromVref = (SmallestMaxNegVref - SmallestMaxPosVref) / 2;
    NBPtr->ChannelPtr->MaxVref = Vnom - OffsetFromVref;
  }
  IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tFinal Vref Offset From Vnom =%s%02x\n",
    ((OffsetFromVref != 0) ? ((SmallestMaxPosVref > SmallestMaxNegVref) ? " + ":" - "):" "), OffsetFromVref);
  return TRUE;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function displays ther results of the 2D search
 *
 *     @param[in,out] *NBPtr - Pointer to the MEM_NB_BLOCK
 *     @param[in]     *Data  - Pointer to Result data structure
 *
 */
VOID
MemFRdWr2DDisplaySearch (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_RD_WR_2D_ENTRY *Data
  )
{
  IDS_HDT_CONSOLE_DEBUG_CODE (
    UINT8 Lane;
    INT8 Vref;
    // Display data collected
    IDS_HDT_CONSOLE (MEM_FLOW, "\t\tDisplaying Data collected\n\n");
    for (Lane = 0; Lane < MemFRdWr2DGetMaxLanes (NBPtr); Lane++) {
      IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tLane: %02x\n", Lane);
      IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\t Vref %s\n", (NBPtr->TechPtr->Direction == DQS_READ_DIR) ? "NegRdDqs PosRdDqs" : "PosWrDat");
      for (Vref = NBPtr->TotalMaxVrefRange - 2; Vref >= 0; Vref--) {
        if (Vref < (Data->Vnom - 1)) {
          IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\t - ");
          IDS_HDT_CONSOLE (MEM_FLOW, "%02x ", ((Data->Vnom -1) - Vref));
        } else if (Vref == (Data->Vnom - 1)) {
          IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\t   00 ");
        } else {
          IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\t + ");
          IDS_HDT_CONSOLE (MEM_FLOW, "%02x ", Vref - (Data->Vnom - 1));
        }
        if (NBPtr->TechPtr->Direction == DQS_READ_DIR) {
          IDS_HDT_CONSOLE (MEM_FLOW, "%08x", Data->Lane[Lane].Vref[Vref].NegRdWrDly);
        } else {
          IDS_HDT_CONSOLE (MEM_FLOW, "        ");
        }
        IDS_HDT_CONSOLE (MEM_FLOW, "%08x \n", Data->Lane[Lane].Vref[Vref].PosRdWrDly);
        NBPtr->FamilySpecificHook[Adjust2DVrefStepSize] (NBPtr, &Vref);
      }
    }
  )
}
