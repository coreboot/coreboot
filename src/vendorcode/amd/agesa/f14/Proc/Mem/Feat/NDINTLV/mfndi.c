/* $NoKeywords:$ */
/**
 * @file
 *
 * mfndi.c
 *
 * Feature applies Node memory interleaving
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Feat/Ndintlv)
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
#include "amdlib.h"
#include "mm.h"
#include "mn.h"
#include "mfndi.h"
#include "Ids.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_MEM_FEAT_NDINTLV_MFNDI_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define _4GB_ (0x10000)

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
 *  Perform a check to see if node interleaving can be enabled on each node.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  Node interleaving can be enabled.
 *     @return          FALSE - Node interleaving cannot be enabled.
 */

BOOLEAN
MemFCheckInterleaveNodes (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  DIE_STRUCT *MCTPtr;

  ASSERT (NBPtr != NULL);

  MCTPtr = NBPtr->MCTPtr;

  if (MCTPtr->NodeMemSize != 0) {
    if (!NBPtr->SharedPtr->NodeIntlv.IsValid) {
      NBPtr->SharedPtr->NodeIntlv.NodeMemSize = MCTPtr->NodeMemSize;
      NBPtr->SharedPtr->NodeIntlv.Dct0MemSize = MCTPtr->DctData[0].Timings.DctMemSize;
      NBPtr->SharedPtr->NodeIntlv.IsValid = TRUE;
    } else {
      if ((NBPtr->SharedPtr->NodeIntlv.NodeMemSize != MCTPtr->NodeMemSize) ||
          (NBPtr->SharedPtr->NodeIntlv.Dct0MemSize != MCTPtr->DctData[0].Timings.DctMemSize)) {
        return FALSE;
      }
    }
  }
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  Applies Node memory interleaving for each node.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  This feature is enabled.
 *     @return          FALSE - This feature is not enabled.
 */

BOOLEAN
MemFInterleaveNodes (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 NodeCnt;
  UINT8 BitShift;
  UINT32 MemSize;
  UINT32 Dct0MemSize;
  UINT32 NodeSysBase;
  UINT32 NodeSysLimit;
  UINT32 HoleBase;
  UINT32 HoleSize;
  UINT32 HoleOffset;
  MEM_PARAMETER_STRUCT *RefPtr;
  DIE_STRUCT *MCTPtr;

  ASSERT (NBPtr != NULL);

  RefPtr = NBPtr->RefPtr;
  MCTPtr = NBPtr->MCTPtr;
  if (RefPtr->GStatus[GsbSoftHole] || RefPtr->GStatus[GsbHWHole]) {
    HoleBase = RefPtr->HoleBase;
    HoleSize = _4GB_ - HoleBase;
  } else {
    HoleBase = 0;
    HoleSize = 0;
  }

  NodeCnt = NBPtr->SharedPtr->NodeIntlv.NodeCnt;
  Dct0MemSize = NBPtr->SharedPtr->NodeIntlv.Dct0MemSize;
  MemSize = NBPtr->SharedPtr->NodeIntlv.NodeMemSize;

  BitShift = LibAmdBitScanForward (NodeCnt);
  Dct0MemSize <<= BitShift;
  if (HoleSize != 0) {
    RefPtr->GStatus[GsbHWHole] = TRUE;
    HoleOffset = HoleSize;
    if (Dct0MemSize >= HoleBase) {
      Dct0MemSize += HoleSize;
    } else {
      HoleOffset += Dct0MemSize;
    }
  } else {
    HoleOffset = 0;
  }

  MemSize = (MemSize << BitShift) + HoleSize;

  MCTPtr->NodeSysBase = 0;
  MCTPtr->NodeSysLimit = MemSize - 1;

  NBPtr->SetBitField (NBPtr, BFDramIntlvSel, NBPtr->SharedPtr->NodeIntlv.NodeIntlvSel);
  NBPtr->SetBitField (NBPtr, BFDramBaseAddr, 0);
  NBPtr->SetBitField (NBPtr, BFDramIntlvEn, NodeCnt - 1);
  NBPtr->SetBitField (NBPtr, BFDramLimitAddr, (MemSize - 1) >> (27 - 16));

  if (HoleSize != 0) {
    MCTPtr->Status[SbHWHole] = TRUE;
    // DramHoleBase will be set when sync address map to other nodes.
    NBPtr->SetBitField (NBPtr, BFDramHoleOffset, HoleOffset >> (23 - 16));
    NBPtr->SetBitField (NBPtr, BFDramHoleValid, 1);
  }

  if ((MCTPtr->DctData[1].Timings.DctMemSize != 0) && (!NBPtr->Ganged)) {
    NBPtr->SetBitField (NBPtr, BFDctSelBaseAddr, Dct0MemSize >> (27 - 16));
    NBPtr->SetBitField (NBPtr, BFDctSelBaseOffset, Dct0MemSize >> (26 - 16));
  }

  NodeSysBase = NodeCnt - 1;
  NodeSysLimit = ((MemSize - 1)& 0xFFFFFF00) | NBPtr->SharedPtr->NodeIntlv.NodeIntlvSel;
  NBPtr->SharedPtr->NodeMap[NBPtr->Node].IsValid = TRUE;
  NBPtr->SharedPtr->NodeMap[NBPtr->Node].SysBase = NodeSysBase;
  NBPtr->SharedPtr->NodeMap[NBPtr->Node].SysLimit = NodeSysLimit;

  NBPtr->SharedPtr->NodeIntlv.NodeIntlvSel++;
  return TRUE;
}
