/* $NoKeywords:$ */
/**
 * @file
 *
 * mnidendimmor.c
 *
 * Or northbridge constructor for dimm identification translator.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/OR)
 * @e \$Revision: 51640 $ @e \$Date: 2011-04-26 03:42:21 -0600 (Tue, 26 Apr 2011) $
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
#include "mm.h"
#include "mn.h"
#include "OptionMemory.h"       // need def for MEM_FEAT_BLOCK_NB
#include "mnor.h"
#include "Ids.h"
#include "cpuRegisters.h"
#include "cpuFamRegisters.h"
#include "cpuFamilyTranslation.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)


#define FILECODE PROC_MEM_NB_OR_MNIDENDIMMOR_FILECODE

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
MemNIdentifyDimmConstructorOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN       UINT8 NodeID
  );

BOOLEAN
STATIC
MemNFixupSysAddrOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       VOID *SysAddrPtr
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
 *   This function initializes the northbridge block for dimm identification translator
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *MemPtr  - Pointer to the MEM_DATA_STRUCT
 *     @param[in,out]   NodeID   - ID of current node to construct
 *     @return          TRUE     - This is the correct constructor for the targeted node.
 *     @return          FALSE    - This isn't the correct constructor for the targeted node.
 */

BOOLEAN
MemNIdentifyDimmConstructorOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN       UINT8 NodeID
  )
{
  //
  // Determine if this is the expected NB Type
  //
  GetLogicalIdOfSocket (MemPtr->DiesPerSystem[NodeID].SocketId, &(MemPtr->DiesPerSystem[NodeID].LogicalCpuid), &(MemPtr->StdHeader));
  if (!MemNIsIdSupportedOr (NBPtr, &(MemPtr->DiesPerSystem[NodeID].LogicalCpuid))) {
    return FALSE;
  }

  NBPtr->NodeCount = MAX_NODES_SUPPORTED_OR;
  NBPtr->DctCount = MAX_DCTS_PER_NODE_OR;
  NBPtr->CsRegMsk = 0x7FF83FE0;
  NBPtr->MemPtr = MemPtr;
  NBPtr->MCTPtr = &(MemPtr->DiesPerSystem[NodeID]);
  NBPtr->PciAddr.AddressValue = MemPtr->DiesPerSystem[NodeID].PciAddr.AddressValue;
  NBPtr->Node = ((UINT8) NBPtr->PciAddr.Address.Device) - 24;
  NBPtr->Ganged = FALSE;
  MemNInitNBRegTableOr (NBPtr, NBPtr->NBRegTable);
  NBPtr->MemNCmnGetSetFieldNb = MemNCmnGetSetFieldOr;
  NBPtr->GetBitField = MemNGetBitFieldNb;
  NBPtr->SetBitField = MemNSetBitFieldNb;
  NBPtr->GetSocketRelativeChannel = MemNGetSocketRelativeChannelOr;
  NBPtr->FamilySpecificHook[DCTSelectSwitch] = MemNDctCfgSelectUnb;
  NBPtr->FamilySpecificHook[FixupSysAddr] = MemNFixupSysAddrOr;

  NBPtr->Dct = 0;
  MemNSetBitFieldNb (NBPtr, BFDctCfgSel, 0);

  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function is used to workaround erratum 637
 *
 *     @param[in,out]   *NBPtr      - Pointer to the MEM_NB_BLOCK
 *     @param[in]       *SysAddrPtr - Pointer SysAddr variable
 *
 */

BOOLEAN
STATIC
MemNFixupSysAddrOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       VOID *SysAddrPtr
  )
{
  UINT64 SysAddr;
  UINT8  SourceNode;
  UINT8  CoreStateSaveDestNode;
  UINT64 Cc6BaseAddress;
  UINT32 IntlvEn;
  UINT32 IntlvSel;
  PCI_ADDR PciAddr;

  // Save NBPtr->PciAddr, so it can be used later to access different node.
  PciAddr = NBPtr->PciAddr;

  SysAddr = *((UINT64 *) SysAddrPtr);
  if ((SysAddr >> 24) == 0xFDF7) {
    // Calculate the address of the source node
    SourceNode = (UINT8) ((SysAddr >> 20) & 0x7);

    // Find CoreStateSaveDestNode based on the access source
    NBPtr->PciAddr.Address.Device = 0x18 + SourceNode;
    CoreStateSaveDestNode = (UINT8) NBPtr->GetBitField (NBPtr, BFCoreStateSaveDestNode);

    // Calculate Cc6BaseAddress from the destination node's DRAM Limit System Address
    NBPtr->PciAddr.Address.Device = 0x18 + CoreStateSaveDestNode;
    Cc6BaseAddress = ((UINT64) NBPtr->GetBitField (NBPtr, BFDramLimitAddr) << 27);
    IntlvEn = NBPtr->GetBitField (NBPtr, BFDramIntlvEn);

    // Check if Node Interleaving is enabled
    if (IntlvEn != 0) {
      // Node Interleaving is enabled, obtain the interleave position
      IntlvSel = NBPtr->GetBitField (NBPtr, BFDramIntlvSel);
      *((UINT64 *) SysAddrPtr) = Cc6BaseAddress | ((IntlvEn ^ 0x7) << 24) | ((SysAddr & 0xFFF000) * (IntlvEn + 1)) | (IntlvSel << 12) | (SysAddr & 0xFFF);
    } else {
      // Node Interleaving is disabled
      *((UINT64 *) SysAddrPtr) = Cc6BaseAddress | (0x7 << 24) | (SysAddr & 0xFFFFFF);
    }
  }

  // Restore NBPtr->PciAddr
  NBPtr->PciAddr = PciAddr;

  return TRUE;
}

