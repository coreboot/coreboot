/* $NoKeywords:$ */
/**
 * @file
 *
 * mmflowtn.c
 *
 * Main Memory initialization sequence for TN
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Main/TN)
 * @e \$Revision: 64574 $ @e \$Date: 2012-01-25 01:01:51 -0600 (Wed, 25 Jan 2012) $
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
#include "Ids.h"
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "mntn.h"
#include "mt.h"
#include "mmlvddr3.h"
#include "cpuFamilyTranslation.h"
#include "Filecode.h"
#include "GeneralServices.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_MEM_MAIN_TN_MMFLOWTN_FILECODE
/* features */
#include "mftds.h"

extern MEM_FEAT_BLOCK_MAIN MemFeatMain;
extern OPTION_MEM_FEATURE_MAIN MemMS3Save;

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
AGESA_STATUS
MemMFlowTN (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  );

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function defines the memory initialization flow for
 *      systems that only support TN processors.
 *
 *     @param[in,out]   *MemMainPtr   - Pointer to the MEM_MAIN_DATA_BLOCK
 *
 *     @return          AGESA_STATUS
 *                          - AGESA_FATAL
 *                          - AGESA_CRITICAL
 *                          - AGESA_SUCCESS
 */
AGESA_STATUS
MemMFlowTN (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  )
{
  MEM_NB_BLOCK  *NBPtr;
  MEM_TECH_BLOCK *TechPtr;
  MEM_DATA_STRUCT *MemPtr;
  ID_INFO CallOutIdInfo;

  NBPtr = MemMainPtr->NBPtr;
  TechPtr = MemMainPtr->TechPtr;
  MemPtr = MemMainPtr->MemPtr;

  GetLogicalIdOfSocket (MemPtr->DiesPerSystem[BSP_DIE].SocketId, &(MemPtr->DiesPerSystem[BSP_DIE].LogicalCpuid), &(MemPtr->StdHeader));
  if (!MemNIsIdSupportedTN (NBPtr, &(MemPtr->DiesPerSystem[BSP_DIE].LogicalCpuid))) {
    MemPtr->IsFlowControlSupported = FALSE;
    return AGESA_FATAL;
  } else {
    MemPtr->IsFlowControlSupported = TRUE;
  }

  MemFInitTableDrive (&NBPtr[BSP_DIE], MTBeforeInitializeMCT);

  //----------------------------------------------------------------
  // Initialize MCT
  //----------------------------------------------------------------
  AGESA_TESTPOINT (TpProcMemInitializeMCT, &(MemMainPtr->MemPtr->StdHeader));
  if (!NBPtr[BSP_DIE].InitializeMCT (&NBPtr[BSP_DIE])) {
    return AGESA_FATAL;
  }

  //----------------------------------------------------------------
  // Low voltage DDR3
  //----------------------------------------------------------------
  // Levelize DDR3 voltage based on socket, as each socket has its own voltage for dimms.
  AGESA_TESTPOINT (TpProcMemLvDdr3, &(MemMainPtr->MemPtr->StdHeader));
  if (!MemFeatMain.LvDDR3 (MemMainPtr)) {
    return AGESA_FATAL;
  }

  //----------------------------------------------------------------
  // Initialize DRAM and DCTs
  //----------------------------------------------------------------
  AGESA_TESTPOINT (TpProcMemInitMCT, &(MemMainPtr->MemPtr->StdHeader));
  // Initialize Memory Controller and Dram
  if (!NBPtr[BSP_DIE].InitMCT (&NBPtr[BSP_DIE])) {
    return AGESA_FATAL; //fatalexit
  }

  MemFInitTableDrive (&NBPtr[BSP_DIE], MTBeforeDInit);

  //------------------------------------------------
  // Finalize target frequency
  //------------------------------------------------
  if (!MemMLvDdr3PerformanceEnhFinalize (MemMainPtr)) {
    return AGESA_FATAL;
  }

  //------------------------------------------------
  // Callout before Dram Init
  //------------------------------------------------
  AGESA_TESTPOINT (TpProcMemBeforeAgesaHookBeforeDramInit, &(MemMainPtr->MemPtr->StdHeader));
  CallOutIdInfo.IdField.SocketId = NBPtr[BSP_DIE].MCTPtr->SocketId;
  CallOutIdInfo.IdField.ModuleId = NBPtr[BSP_DIE].MCTPtr->DieId;
  IDS_HDT_CONSOLE (MEM_FLOW, "\nCalling out to Platform BIOS on Socket %d, Module %d...\n", CallOutIdInfo.IdField.SocketId, CallOutIdInfo.IdField.ModuleId);
  AgesaHookBeforeDramInit ((UINTN) CallOutIdInfo.IdInformation, MemMainPtr->MemPtr);
  IDS_HDT_CONSOLE (MEM_FLOW, "\nVDDIO = 1.%dV\n", (NBPtr[BSP_DIE].RefPtr->DDR3Voltage == VOLT1_5) ? 5 :
                                        (NBPtr[BSP_DIE].RefPtr->DDR3Voltage == VOLT1_35) ? 35 :
                                        (NBPtr[BSP_DIE].RefPtr->DDR3Voltage == VOLT1_25) ? 25 : 999);
  AGESA_TESTPOINT (TpProcMemAfterAgesaHookBeforeDramInit, &(NBPtr->MemPtr->StdHeader));

  //-------------------------------------------------
  // Do dram init and create memory map
  //-------------------------------------------------
  IDS_OPTION_HOOK (IDS_BEFORE_DRAM_INIT, &NBPtr[BSP_DIE], &(MemMainPtr->MemPtr->StdHeader));
  NBPtr[BSP_DIE].StartupDCT (&NBPtr[BSP_DIE]);

  // Create memory map
  AGESA_TESTPOINT (TpProcMemSystemMemoryMapping, &(MemMainPtr->MemPtr->StdHeader));
  if (!NBPtr[BSP_DIE].HtMemMapInit (&NBPtr[BSP_DIE])) {
    return AGESA_FATAL;
  }

  //----------------------------------------------------
  // If there is no dimm on the system, do fatal exit
  //----------------------------------------------------
  if (NBPtr[BSP_DIE].RefPtr->SysLimit == 0) {
    PutEventLog (AGESA_FATAL, MEM_ERROR_NO_DIMM_FOUND_ON_SYSTEM, 0, 0, 0, 0, &(MemMainPtr->MemPtr->StdHeader));
    return AGESA_FATAL;
  }

  //----------------------------------------------------------------
  // CpuMemTyping
  //----------------------------------------------------------------
  AGESA_TESTPOINT (TpProcMemMtrrConfiguration, &(MemMainPtr->MemPtr->StdHeader));
  if (!NBPtr[BSP_DIE].CpuMemTyping (&NBPtr[BSP_DIE])) {
    return AGESA_FATAL;
  }

  //----------------------------------------------------------------
  // Before Training Table values
  //----------------------------------------------------------------
  MemFInitTableDrive (&NBPtr[BSP_DIE], MTBeforeTrn);

  //----------------------------------------------------------------
  // Memory Context Restore
  //----------------------------------------------------------------
  if (!MemFeatMain.MemRestore (MemMainPtr)) {
    // Do DQS training only if memory context restore fails

    //----------------------------------------------------------------
    // Training
    //----------------------------------------------------------------
    MemMainPtr->mmSharedPtr->DimmExcludeFlag = TRAINING;
    AGESA_TESTPOINT (TpProcMemDramTraining, &(MemMainPtr->MemPtr->StdHeader));
    IDS_SKIP_HOOK (IDS_BEFORE_DQS_TRAINING, MemMainPtr, &(MemMainPtr->MemPtr->StdHeader)) {
      if (!MemFeatMain.Training (MemMainPtr)) {
        return AGESA_FATAL;
      }
    }
    IDS_HDT_CONSOLE (MEM_FLOW, "\nEnd DQS training\n\n");
  }

  //----------------------------------------------------------------
  // Disable chipselects that fail training
  //----------------------------------------------------------------
  MemMainPtr->mmSharedPtr->DimmExcludeFlag = END_TRAINING;
  MemFeatMain.ExcludeDIMM (MemMainPtr);
  MemMainPtr->mmSharedPtr->DimmExcludeFlag = NORMAL;

  //----------------------------------------------------------------
  // OtherTiming
  //----------------------------------------------------------------
  AGESA_TESTPOINT (TpProcMemOtherTiming, &(MemMainPtr->MemPtr->StdHeader));
  if (!NBPtr[BSP_DIE].OtherTiming (&NBPtr[BSP_DIE])) {
    return AGESA_FATAL;
  }

  //----------------------------------------------------------------
  // After Training Table values
  //----------------------------------------------------------------
  MemFInitTableDrive (&NBPtr[BSP_DIE], MTAfterTrn);

  //----------------------------------------------------------------
  // Interleave channels
  //----------------------------------------------------------------
  if (NBPtr[BSP_DIE].FeatPtr->InterleaveChannels (&NBPtr[BSP_DIE])) {
    if (NBPtr[BSP_DIE].MCTPtr->ErrCode == AGESA_FATAL) {
      return AGESA_FATAL;
    }
  }

  //----------------------------------------------------------------
  // Interleave banks
  //----------------------------------------------------------------
  if (NBPtr[BSP_DIE].FeatPtr->InterleaveBanks (&NBPtr[BSP_DIE])) {
    if (NBPtr[BSP_DIE].MCTPtr->ErrCode == AGESA_FATAL) {
      return AGESA_FATAL;
    }
  }

  //----------------------------------------------------------------
  // After Programming Interleave registers
  //----------------------------------------------------------------
  MemFInitTableDrive (&NBPtr[BSP_DIE], MTAfterInterleave);

  //----------------------------------------------------------------
  // Memory Clear
  //----------------------------------------------------------------
  AGESA_TESTPOINT (TpProcMemMemClr, &(MemMainPtr->MemPtr->StdHeader));
  if (!MemFeatMain.MemClr (MemMainPtr)) {
    return AGESA_FATAL;
  }

  //----------------------------------------------------------------
  // C6 Storage Allocation
  //----------------------------------------------------------------
  NBPtr[BSP_DIE].AllocateC6Storage (&NBPtr[BSP_DIE]);

  //----------------------------------------------------------------
  // UMA Allocation & UMAMemTyping
  //----------------------------------------------------------------
  AGESA_TESTPOINT (TpProcMemUMAMemTyping, &(MemMainPtr->MemPtr->StdHeader));
  if (!MemFeatMain.UmaAllocation (MemMainPtr)) {
    return AGESA_FATAL;
  }

  //----------------------------------------------------------------
  // Interleave region
  //----------------------------------------------------------------
  NBPtr[BSP_DIE].FeatPtr->InterleaveRegion (&NBPtr[BSP_DIE]);

  //----------------------------------------------------------------
  // OnDimm Thermal
  //----------------------------------------------------------------
  if (NBPtr[BSP_DIE].FeatPtr->OnDimmThermal (&NBPtr[BSP_DIE])) {
    if (NBPtr[BSP_DIE].MCTPtr->ErrCode == AGESA_FATAL) {
      return AGESA_FATAL;
    }
  }

  //----------------------------------------------------------------
  // Finalize MCT
  //----------------------------------------------------------------
  if (!NBPtr[BSP_DIE].FinalizeMCT (&NBPtr[BSP_DIE])) {
    return AGESA_FATAL;
  }

  //----------------------------------------------------------------
  // After Finalize MCT
  //----------------------------------------------------------------
  MemFInitTableDrive (&NBPtr[BSP_DIE], MTAfterFinalizeMCT);

  //----------------------------------------------------------------
  // Memory Context Save
  //----------------------------------------------------------------
  MemFeatMain.MemSave (MemMainPtr);

  //----------------------------------------------------------------
  // Memory DMI support
  //----------------------------------------------------------------
  if (!MemFeatMain.MemDmi (MemMainPtr)) {
    return AGESA_CRITICAL;
  }


  //----------------------------------------------------------------
  // Save memory S3 data
  //----------------------------------------------------------------
  if (!MemMS3Save (MemMainPtr)) {
    return AGESA_CRITICAL;
  }

  //----------------------------------------------------------------
  // Switch back to DCT 0 before sending control back
  //----------------------------------------------------------------
  NBPtr[BSP_DIE].SwitchDCT (&NBPtr[BSP_DIE], 0);

  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
