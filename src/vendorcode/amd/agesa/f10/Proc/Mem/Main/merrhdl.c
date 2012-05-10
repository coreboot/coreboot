/**
 * @file
 *
 * merrhdl.c
 *
 * Memory error handling
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Main)
 * @e \$Revision: 6474 $ @e \$Date: 2008-06-20 03:07:59 -0500 (Fri, 20 Jun 2008) $
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
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "heapManager.h"
#include "merrhdl.h"
#include "Filecode.h"
#define FILECODE PROC_MEM_MAIN_MERRHDL_FILECODE

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
 *  This function handle errors occur in memory code.
 *
 *
 *     @param[in,out]   *MCTPtr - pointer to DIE_STRUCT.
 *     @param[in,out]   DCT - DCT that needs to be handled.
 *     @param[in,out]   ChipSelMask - Chip select mask that needs to be handled
 *     @param[in,out]   *StdHeader - pointer to AMD_CONFIG_PARAMS
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */
BOOLEAN
MemErrHandle (
  IN       DIE_STRUCT *MCTPtr,
  IN       UINT8  DCT,
  IN       UINT16  ChipSelMask,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  BOOLEAN ErrorRecovery;
  BOOLEAN IgnoreErr;
  DCT_STRUCT *DCTPtr;
  UINT8 CurrentDCT;
  LOCATE_HEAP_PTR LocHeap;
  MEM_NB_BLOCK *NBPtr;
  MEM_MAIN_DATA_BLOCK mmData;

  DCTPtr = MCTPtr->DctData;
  ErrorRecovery = TRUE;
  IgnoreErr = FALSE;
  IDS_OPTION_HOOK (IDS_MEM_ERROR_RECOVERY, &ErrorRecovery, StdHeader);

  if (ErrorRecovery) {
    if (DCT == EXCLUDE_ALL_DCT) {
      // Exclude all DCTs on a node
      for (CurrentDCT = 0; CurrentDCT < MCTPtr->DctCount; CurrentDCT++) {
        DCTPtr[CurrentDCT].Timings.CsTestFail = DCTPtr[CurrentDCT].Timings.CsPresent;
      }
    } else if (ChipSelMask == EXCLUDE_ALL_CHIPSEL) {
      // Exclude the specified DCT
      DCTPtr[DCT].Timings.CsTestFail = DCTPtr[DCT].Timings.CsPresent;
    } else {
      // Exclude the chip select that has been marked out
      DCTPtr[DCT].Timings.CsTestFail |= ChipSelMask & DCTPtr[DCT].Timings.CsPresent;
      IDS_OPTION_HOOK (IDS_LOADCARD_ERROR_RECOVERY, &DCTPtr[DCT], StdHeader);
    }

    // Exclude the failed dimm to recovery from error
    if (MCTPtr->NodeMemSize != 0) {
      LocHeap.BufferHandle = AMD_MEM_AUTO_HANDLE;
      if (HeapLocateBuffer (&LocHeap, StdHeader) == AGESA_SUCCESS) {
        // NB block has already been constructed by main block.
        // No need to construct it here.
        NBPtr = (MEM_NB_BLOCK *)LocHeap.BufferPtr;
        if (!NBPtr->SharedPtr->NodeMap[MCTPtr->NodeId].IsValid) {
          // Memory map has not been calculated, no need to remap memory across node here.
          // Only need to remap memory within the node.
          NBPtr = &NBPtr[MCTPtr->NodeId];
          NBPtr->FeatPtr->ExcludeDIMM (NBPtr);
        } else {
          // Need to remap memory across the whole system.
          mmData.MemPtr = NBPtr->MemPtr;
          mmData.mmSharedPtr = NBPtr->SharedPtr;
          mmData.NBPtr = NBPtr;
          mmData.TechPtr = (MEM_TECH_BLOCK *) (&NBPtr[NBPtr->MemPtr->DieCount]);
          mmData.DieCount = NBPtr->MemPtr->DieCount;
          if (!MemFeatMain.ExcludeDIMM (&mmData)) {
            return FALSE;
          }
        }
      }
      // If allocation fails, that means the code is not running at BSP.
      // Parallel training is in process.
      // Remap for parallel training will be done when control returns to BSP.
    }
    return TRUE;
  } else {
    IDS_OPTION_HOOK (IDS_MEM_IGNORE_ERROR, &IgnoreErr, StdHeader);
    if (IgnoreErr) {
      return TRUE;
    }
    SetMemError (AGESA_FATAL, MCTPtr);
    ASSERT(FALSE); // ErrorRecovery is FALSE
    return FALSE;
  }
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

