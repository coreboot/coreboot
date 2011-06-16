/* $NoKeywords:$ */
/**
 * @file
 *
 * mnprotodr.c
 *
 * Northbridge support functions for Errata and early samples
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/DR)
 * @e \$Revision: 44324 $ @e \$Date: 2010-12-22 17:16:51 +0800 (Wed, 22 Dec 2010) $
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



#include "AGESA.h"
#include "mport.h"
#include "mm.h"
#include "mn.h"
#include "OptionMemory.h"       // need def for MEM_FEAT_BLOCK_NB
#include "mndr.h"
#include "cpuFamRegisters.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_NB_DR_MNPROTODR_FILECODE

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
VOID
STATIC
MemNTrainFenceWHardCodeValDr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

/*
 *-----------------------------------------------------------------------------
 *                                EXPORTED FUNCTIONS
 *
 *-----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function conditionally executes specific Phy fence training function.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemPPhyFenceTrainingDr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  if (NBPtr->GetBitField (NBPtr, BFDdr3Mode) == 0) {
    //DDR2 specific.
    if (NBPtr->MCTPtr->LogicalCpuid.Revision & AMD_F10_C0) {
      MemNTrainFenceWHardCodeValDr (NBPtr);
    } else {
      MemNTrainPhyFenceNb (NBPtr);
    }
  } else {
    //DDR3 specific.
    MemNTrainPhyFenceNb (NBPtr);
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function executes hardcoded Phy fence training.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
STATIC
MemNTrainFenceWHardCodeValDr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Dct;
  UINT8 CurDct;
  UINT16 Speed;

  CurDct = NBPtr->Dct;
  if (NBPtr->MCTPtr->NodeMemSize) {
    for (Dct = 0; Dct < MAX_DCTS_PER_NODE_DR; Dct++) {
      NBPtr->SwitchDCT (NBPtr, Dct);
      Speed = NBPtr->DCTPtr->Timings.Speed;
      NBPtr->SetBitField (NBPtr, BFPhyFence, ((Speed == DDR800_FREQUENCY) || (Speed == DDR1066_FREQUENCY)) ? 20 : 20);
      NBPtr->SetBitField (NBPtr, BFSlowAccessMode, (NBPtr->ChannelPtr->SlowMode) ? 1 : 0);
      NBPtr->SetBitField (NBPtr, BFODCControl, NBPtr->ChannelPtr->DctOdcCtl);
      NBPtr->SetBitField (NBPtr, BFAddrTmgControl, NBPtr->ChannelPtr->DctAddrTmg);
    }
  }

  NBPtr->SwitchDCT (NBPtr, CurDct);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function executes Node memory 1GB boundary alignment.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *NodeSysLimit   - Pointer to the NodeSysLimit
 *
 */

VOID
MemPNodeMemBoundaryDr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   UINT32 *NodeSysLimit
  )
{
  if (NBPtr->GetBitField (NBPtr, BFDdr3Mode) == 0) {
    // only apply to DDR2.
    if (*NodeSysLimit > ((UINT32)1 << (30 - 16))) {
      // if (NodeSysLimit > 1GB) then set to Node limit to 1GB boundary for each node
      *NodeSysLimit += 1;
      *NodeSysLimit &= 0xFFFFC000;
      *NodeSysLimit -= 1;
    }
  }
}

