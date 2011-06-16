/* $NoKeywords:$ */
/**
 * @file
 *
 * mmflowln.c
 *
 * Main Memory initialization sequence for LN
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Main/LN)
 * @e \$Revision: 49794 $ @e \$Date: 2011-03-29 13:59:05 +0800 (Tue, 29 Mar 2011) $
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
#include "Ids.h"
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "mnln.h"
#include "mt.h"
#include "cpuFamilyTranslation.h"
#include "Filecode.h"
#include "GeneralServices.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_MAIN_LN_MMFLOWLN_FILECODE
/* features */
#include "mftds.h"

extern MEM_FEAT_BLOCK_MAIN MemFeatMain;

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
MemMFlowLN (
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
 *      systems that only support LN processors.
 *
 *     @param[in,out]   *MemMainPtr   - Pointer to the MEM_MAIN_DATA_BLOCK
 *
 *     @return          AGESA_STATUS
 *                          - AGESA_ALERT
 *                          - AGESA_FATAL
 *                          - AGESA_SUCCESS
 *                          - AGESA_WARNING
 */
AGESA_STATUS
MemMFlowLN (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  )
{
  MEM_NB_BLOCK  *NBPtr;
  MEM_DATA_STRUCT *MemPtr;

  NBPtr = MemMainPtr->NBPtr;
  MemPtr = MemMainPtr->MemPtr;

  GetLogicalIdOfSocket (MemPtr->DiesPerSystem[BSP_DIE].SocketId, &(MemPtr->DiesPerSystem[BSP_DIE].LogicalCpuid), &(MemPtr->StdHeader));
  if (!MemNIsIdSupportedLN (NBPtr, &(MemPtr->DiesPerSystem[BSP_DIE].LogicalCpuid))) {
    MemPtr->IsFlowControlSupported = FALSE;
    return AGESA_FATAL;
  } else {
    MemPtr->IsFlowControlSupported = TRUE;
  }

  MemFInitTableDrive (&NBPtr[BSP_DIE], MTBeforeInitializeMCT);

  //----------------------------------------------------------------
  // Low voltage DDR3
  //----------------------------------------------------------------
  // Levelize DDR3 voltage based on socket, as each socket has its own voltage for dimms.
  AGESA_TESTPOINT (TpProcMemLvDdr3, &(MemMainPtr->MemPtr->StdHeader));
  if (!MemFeatMain.LvDDR3 (MemMainPtr)) {
    return AGESA_FATAL;
  }

  //----------------------------------------------------------------
  // Initialize DRAM and DCTs, and Create Memory Map
  //----------------------------------------------------------------
  AGESA_TESTPOINT (TpProcMemInitMCT, &(MemMainPtr->MemPtr->StdHeader));
  // Initialize Memory Controller and Dram
  IDS_HDT_CONSOLE (MEM_STATUS, "Node 0\n");

  if (!NBPtr[BSP_DIE].InitMCT (&NBPtr[BSP_DIE])) {
    return AGESA_FATAL; //fatalexit
  }

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
    ASSERT(FALSE); // Size of memory on BSP = 0, so no DIMM found
    return AGESA_FATAL;
  }

  //----------------------------------------------------------------
  // Synchronize DCTs
  //----------------------------------------------------------------
  AGESA_TESTPOINT (TpProcMemSynchronizeDcts, &(MemMainPtr->MemPtr->StdHeader));
  if (!NBPtr[BSP_DIE].SyncDctsReady (&NBPtr[BSP_DIE])) {
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
  // Interleave banks
  //----------------------------------------------------------------
  if (NBPtr[BSP_DIE].FeatPtr->InterleaveBanks (&NBPtr[BSP_DIE])) {
    if (NBPtr[BSP_DIE].MCTPtr->ErrCode == AGESA_FATAL) {
      return AGESA_FATAL;
    }
  }

  //----------------------------------------------------------------
  // Interleave channels
  //----------------------------------------------------------------
  if (NBPtr[BSP_DIE].FeatPtr->InterleaveChannels (&NBPtr[BSP_DIE])) {
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

  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
