/* $NoKeywords:$ */
/**
 * @file
 *
 * mmNodeInterleave.c
 *
 * Main Memory Feature implementation file for Node Interleaving
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Main)
 * @e \$Revision: 35136 $ @e \$Date: 2010-07-16 11:29:48 +0800 (Fri, 16 Jul 2010) $
 *
 **/
/*
 *****************************************************************************
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
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "Ids.h"
#include "GeneralServices.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_MEM_MAIN_MMNODEINTERLEAVE_FILECODE

extern MEM_FEAT_BLOCK_MAIN MemFeatMain;
/*-----------------------------------------------------------------------------
*                                EXPORTED FUNCTIONS
*
*-----------------------------------------------------------------------------
*/

BOOLEAN
MemMInterleaveNodes (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  );

/* -----------------------------------------------------------------------------*/
/**
 *
 *  Check and enable node interleaving on all nodes.
 *
 *     @param[in,out]   *MemMainPtr   - Pointer to the MEM_MAIN_DATA_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */
BOOLEAN
MemMInterleaveNodes (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  )
{
  UINT8   Node;
  UINT8   NodeCnt;
  BOOLEAN RetVal;
  MEM_NB_BLOCK  *NBPtr;

  NBPtr = MemMainPtr->NBPtr;
  NodeCnt = 0;
  RetVal = TRUE;

  if (NBPtr->RefPtr->EnableNodeIntlv) {
    if (!MemFeatMain.MemClr (MemMainPtr)) {
      PutEventLog (AGESA_WARNING, MEM_WARNING_NODE_INTERLEAVING_NOT_ENABLED, 0, 0, 0, 0, &NBPtr->MemPtr->StdHeader);
      SetMemError (AGESA_WARNING, NBPtr->MCTPtr);
      return FALSE;
    }

    MemMainPtr->mmSharedPtr->NodeIntlv.IsValid = FALSE;
    MemMainPtr->mmSharedPtr->NodeIntlv.NodeIntlvSel = 0;

    for (Node = 0; Node < MemMainPtr->DieCount; Node++) {
      if (!NBPtr[Node].FeatPtr->CheckInterleaveNodes (&NBPtr[Node])) {
        break;
      }
      if (NBPtr[Node].MCTPtr->NodeMemSize != 0) {
        NodeCnt ++;
      }
    }

    if ((Node == MemMainPtr->DieCount) && (NodeCnt != 0) && ((NodeCnt & (NodeCnt - 1)) == 0)) {
      MemMainPtr->mmSharedPtr->NodeIntlv.NodeCnt = NodeCnt;
      for (Node = 0; Node < MemMainPtr->DieCount; Node++) {
        if (NBPtr[Node].MCTPtr->NodeMemSize != 0) {
          NBPtr[Node].FeatPtr->InterleaveNodes (&NBPtr[Node]);
        }
      }
      for (Node = 0; Node < MemMainPtr->DieCount; Node++) {
        NBPtr[Node].SyncAddrMapToAllNodes (&NBPtr[Node]);
        RetVal &= (BOOLEAN) (NBPtr[Node].MCTPtr->ErrCode < AGESA_FATAL);
      }
    } else {
      //
      // If all nodes cannot be interleaved
      //
      PutEventLog (AGESA_WARNING, MEM_WARNING_NODE_INTERLEAVING_NOT_ENABLED, 0, 0, 0, 0, &NBPtr->MemPtr->StdHeader);
      SetMemError (AGESA_WARNING, NBPtr->MCTPtr);
    }
  }

  return RetVal;
}
