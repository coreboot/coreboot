/* $NoKeywords:$ */
/**
 * @file
 *
 * mnidendimmhy.c
 *
 * Hy northbridge constructor for dimm identification translator.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/HY)
 * @e \$Revision: 36520 $ @e \$Date: 2010-08-20 14:57:36 +0800 (Fri, 20 Aug 2010) $
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
#include "mm.h"
#include "mn.h"
#include "OptionMemory.h"       // need def for MEM_FEAT_BLOCK_NB
#include "mnhy.h"
#include "Ids.h"
#include "cpuRegisters.h"
#include "cpuFamRegisters.h"
#include "cpuFamilyTranslation.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_MEM_NB_HY_MNIDENDIMMHY_FILECODE

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
 *   This function initializes the northbridge block for dimm identification translator
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *MemPtr  - Pointer to the MEM_DATA_STRUCT
 *     @param[in,out]   NodeID   - ID of current node to construct
 *     @return          TRUE     - This is the correct constructor for the targeted node.
 *     @return          FALSE    - This isn't the correct constructor for the targeted node.
 */

BOOLEAN
MemNIdentifyDimmConstructorHy (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN       UINT8 NodeID
  )
{
  //
  // Determine if this is the expected NB Type
  //
  GetLogicalIdOfSocket (MemPtr->DiesPerSystem[NodeID].SocketId, &(MemPtr->DiesPerSystem[NodeID].LogicalCpuid), &(MemPtr->StdHeader));
  if (!MemNIsIdSupportedHy (NBPtr, &(MemPtr->DiesPerSystem[NodeID].LogicalCpuid))) {
    return FALSE;
  }

  NBPtr->NodeCount = MAX_NODES_SUPPORTED_HY;
  NBPtr->DctCount = MAX_DCTS_PER_NODE_HY;
  NBPtr->MemPtr = MemPtr;
  NBPtr->MCTPtr = &(MemPtr->DiesPerSystem[NodeID]);
  NBPtr->PciAddr.AddressValue = MemPtr->DiesPerSystem[NodeID].PciAddr.AddressValue;
  NBPtr->Node = ((UINT8) NBPtr->PciAddr.Address.Device) - 24;
  NBPtr->Ganged = FALSE;
  InitNBRegTableHy (NBPtr, NBPtr->NBRegTable);
  NBPtr->MemNCmnGetSetFieldNb = MemNCmnGetSetFieldHy;
  NBPtr->GetBitField = MemNGetBitFieldNb;
  NBPtr->SetBitField = MemNSetBitFieldNb;
  NBPtr->GetSocketRelativeChannel = MemNGetSocketRelativeChannelHy;

  return TRUE;
}
