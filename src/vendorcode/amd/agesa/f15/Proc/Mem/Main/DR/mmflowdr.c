/* $NoKeywords:$ */
/**
 * @file
 *
 * mmflowdr.c
 *
 * Main Memory initialization sequence for DR
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Main/DR)
 * @e \$Revision: 56279 $ @e \$Date: 2011-07-11 13:11:28 -0600 (Mon, 11 Jul 2011) $
 *
 **/
/*****************************************************************************
*
* Copyright (C) 2012 Advanced Micro Devices, Inc.
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
#include "Ids.h"
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "mndr.h"
#include "mt.h"
#include "cpuFamilyTranslation.h"
#include "Filecode.h"
#include "GeneralServices.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_MEM_MAIN_DR_MMFLOWDR_FILECODE
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
 *      systems that only support RB processors.
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
MemMFlowDr (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  )
{
  UINT8   Node;
  UINT8   NodeCnt;
  MEM_NB_BLOCK  *NBPtr;
  MEM_TECH_BLOCK *TechPtr;
  MEM_DATA_STRUCT *MemPtr;

  NBPtr = MemMainPtr->NBPtr;
  TechPtr = MemMainPtr->TechPtr;
  NodeCnt = MemMainPtr->DieCount;
  MemPtr = MemMainPtr->MemPtr;

  GetLogicalIdOfSocket (MemPtr->DiesPerSystem[BSP_DIE].SocketId, &(MemPtr->DiesPerSystem[BSP_DIE].LogicalCpuid), &(MemPtr->StdHeader));
  if (!MemNIsIdSupportedDr (NBPtr, &(MemPtr->DiesPerSystem[BSP_DIE].LogicalCpuid))) {
    MemPtr->IsFlowControlSupported = FALSE;
    return AGESA_FATAL;
  } else {
    MemPtr->IsFlowControlSupported = TRUE;
  }

  for (Node = 0; Node < NodeCnt; Node++) {
    MemFInitTableDrive (&NBPtr[Node], MTBeforeInitializeMCT);
  }

  //----------------------------------------------------------------
  // Initialize MCT
  //----------------------------------------------------------------
  AGESA_TESTPOINT (TpProcMemInitializeMCT, &(MemMainPtr->MemPtr->StdHeader));
  for (Node = 0; Node < NodeCnt; Node++) {
    if (!NBPtr[Node].InitializeMCT (&NBPtr[Node])) {
      return AGESA_FATAL;
    }
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
  // Initialize DRAM and DCTs, and Create Memory Map
  //----------------------------------------------------------------
  AGESA_TESTPOINT (TpProcMemInitMCT, &(MemMainPtr->MemPtr->StdHeader));
  for (Node = 0; Node < NodeCnt; Node++) {
    // Initialize Memory Controller and Dram
    IDS_HDT_CONSOLE (MEM_STATUS, "Node %d\n", Node);

    if (!NBPtr[Node].InitMCT (&NBPtr[Node])) {
      return AGESA_FATAL; //fatalexit
    }

    // Create memory map
    AGESA_TESTPOINT (TpProcMemSystemMemoryMapping, &(MemMainPtr->MemPtr->StdHeader));
    if (!NBPtr[Node].HtMemMapInit (&NBPtr[Node])) {
      return AGESA_FATAL;
    }
  }

  //----------------------------------------------------
  // If there is no dimm on the system, do fatal exit
  //----------------------------------------------------
  if (NBPtr[BSP_DIE].RefPtr->SysLimit == 0) {
    PutEventLog (AGESA_FATAL, MEM_ERROR_NO_DIMM_FOUND_ON_SYSTEM, 0, 0, 0, 0, &(MemMainPtr->MemPtr->StdHeader));
    ASSERT (FALSE);
    return AGESA_FATAL;
  }

  //----------------------------------------------------------------
  // Synchronize DCTs
  //----------------------------------------------------------------
  AGESA_TESTPOINT (TpProcMemSynchronizeDcts, &(MemMainPtr->MemPtr->StdHeader));
  for (Node = 0; Node < NodeCnt; Node++) {
    if (!NBPtr[Node].SyncDctsReady (&NBPtr[Node])) {
      return AGESA_FATAL;
    }
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
  for (Node = 0; Node < NodeCnt; Node++) {
    MemFInitTableDrive (&NBPtr[Node], MTBeforeTrn);
  }

  //----------------------------------------------------------------
  // Memory Context Restore
  //----------------------------------------------------------------
  if (!MemFeatMain.MemRestore (MemMainPtr)) {
    // Do DQS training only if memory context restore fails

    //----------------------------------------------------------------
    // Training
    //----------------------------------------------------------------
    AGESA_TESTPOINT (TpProcMemDramTraining, &(MemMainPtr->MemPtr->StdHeader));
    MemMainPtr->mmSharedPtr->DimmExcludeFlag = TRAINING;
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
  for (Node = 0; Node < NodeCnt; Node++) {
    if (!NBPtr[Node].OtherTiming (&NBPtr[Node])) {
      return AGESA_FATAL;
    }
  }

  //----------------------------------------------------------------
  // After Training Table values
  //----------------------------------------------------------------
  for (Node = 0; Node < NodeCnt; Node++) {
    MemFInitTableDrive (&NBPtr[Node], MTAfterTrn);
  }

  //----------------------------------------------------------------
  // SetDqsEccTimings
  //----------------------------------------------------------------
  AGESA_TESTPOINT (TpProcMemSetDqsEccTmgs, &(MemMainPtr->MemPtr->StdHeader));
  for (Node = 0; Node < NodeCnt; Node++) {
    if (!TechPtr[Node].SetDqsEccTmgs (&TechPtr[Node])) {
      return AGESA_FATAL;
    }
  }

  //----------------------------------------------------------------
  // Online Spare
  //----------------------------------------------------------------
  if (!MemFeatMain.OnlineSpare (MemMainPtr)) {
    return AGESA_FATAL;
  }

  //----------------------------------------------------------------
  // Interleave banks
  //----------------------------------------------------------------
  for (Node = 0; Node < NodeCnt; Node++) {
    if (NBPtr[Node].FeatPtr->InterleaveBanks (&NBPtr[Node])) {
      if (NBPtr[Node].MCTPtr->ErrCode == AGESA_FATAL) {
        return AGESA_FATAL;
      }
    }
  }

  //----------------------------------------------------------------
  // Interleave Nodes
  //----------------------------------------------------------------
  if (!MemFeatMain.InterleaveNodes (MemMainPtr)) {
    return AGESA_FATAL;
  }

  //----------------------------------------------------------------
  // Interleave channels
  //----------------------------------------------------------------
  for (Node = 0; Node < NodeCnt; Node++) {
    if (NBPtr[Node].FeatPtr->InterleaveChannels (&NBPtr[Node])) {
      if (NBPtr[Node].MCTPtr->ErrCode == AGESA_FATAL) {
        return AGESA_FATAL;
      }
    }
  }

  //----------------------------------------------------------------
  // After Programming Interleave registers
  //----------------------------------------------------------------
  for (Node = 0; Node < NodeCnt; Node++) {
    MemFInitTableDrive (&NBPtr[Node], MTAfterInterleave);
  }

  //----------------------------------------------------------------
  // UMA Allocation & UMAMemTyping
  //----------------------------------------------------------------
  AGESA_TESTPOINT (TpProcMemUMAMemTyping, &(MemMainPtr->MemPtr->StdHeader));
  if (!MemFeatMain.UmaAllocation (MemMainPtr)) {
    return AGESA_FATAL;
  }

  // ECC
  //----------------------------------------------------------------
  if (!MemFeatMain.InitEcc (MemMainPtr)) {
    return AGESA_FATAL;
  }

  //----------------------------------------------------------------
  // Memory Clear
  //----------------------------------------------------------------
  AGESA_TESTPOINT (TpProcMemMemClr, &(MemMainPtr->MemPtr->StdHeader));
  if (!MemFeatMain.MemClr (MemMainPtr)) {
    return AGESA_FATAL;
  }

  //----------------------------------------------------------------
  // OnDimm Thermal
  //----------------------------------------------------------------
  for (Node = 0; Node < NodeCnt; Node++) {
    if (NBPtr[Node].FeatPtr->OnDimmThermal (&NBPtr[Node])) {
      if (NBPtr[Node].MCTPtr->ErrCode == AGESA_FATAL) {
        return AGESA_FATAL;
      }
    }
  }

  //----------------------------------------------------------------
  // Finalize MCT
  //----------------------------------------------------------------
  for (Node = 0; Node < NodeCnt; Node++) {
    if (!NBPtr[Node].FinalizeMCT (&NBPtr[Node])) {
      return AGESA_FATAL;
    }
  }

  //----------------------------------------------------------------
  // After Finalize MCT
  //----------------------------------------------------------------
  for (Node = 0; Node < NodeCnt; Node++) {
    MemFInitTableDrive (&NBPtr[Node], MTAfterFinalizeMCT);
  }

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
