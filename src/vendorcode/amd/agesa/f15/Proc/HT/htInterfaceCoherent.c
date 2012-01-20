/* $NoKeywords:$ */
/**
 * @file
 *
 * External Interface implementation for coherent features.
 *
 * Contains routines for accessing the interface to the client BIOS,
 * for support only required for coherent features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  HyperTransport
 * @e \$Revision: 56279 $   @e \$Date: 2011-07-11 13:11:28 -0600 (Mon, 11 Jul 2011) $
 *
 */
/*
 *****************************************************************************
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
#include "amdlib.h"
#include "Ids.h"
#include "Topology.h"
#include "htFeat.h"
#include "htInterface.h"
#include "htInterfaceGeneral.h"
#include "htInterfaceCoherent.h"
#include "htNb.h"
#include "heapManager.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_HT_HTINTERFACECOHERENT_FILECODE
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

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------*/
/**
 * Get limits for CPU to CPU Links.
 *
 * @HtInterfaceMethod{::F_GET_CPU_2_CPU_PCB_LIMITS}
 *
 * For each coherent connection this routine is called once.  Update the frequency
 * and width if needed for this Link (usually based on board restriction).  This is
 * used with CPU device capabilities and northbridge limits to compute the default
 * settings.  The input width and frequency are valid, but do not necessarily reflect
 * the minimum setting that will be chosen.
 *
 * @param[in]     NodeA One Node on which this Link is located
 * @param[in]     LinkA The Link on this Node
 * @param[in]     NodeB The other Node on which this Link is located
 * @param[in]     LinkB The Link on that Node
 * @param[in,out] ABLinkWidthLimit modify to change the Link Width In
 * @param[in,out] BALinkWidthLimit modify to change the Link Width Out
 * @param[in,out] PcbFreqCap modify to change the Link's frequency capability
 * @param[in]     State the input data
 *
 */
VOID
GetCpu2CpuPcbLimits (
  IN       UINT8        NodeA,
  IN       UINT8        LinkA,
  IN       UINT8        NodeB,
  IN       UINT8        LinkB,
  IN OUT   UINT8        *ABLinkWidthLimit,
  IN OUT   UINT8        *BALinkWidthLimit,
  IN OUT   UINT32       *PcbFreqCap,
  IN       STATE_DATA   *State
  )
{
  CPU_TO_CPU_PCB_LIMITS *p;
  UINT8 SocketA;
  UINT8 SocketB;
  UINT8 PackageLinkA;
  UINT8 PackageLinkB;

  ASSERT ((NodeA < MAX_NODES) && (NodeB < MAX_NODES));
  ASSERT ((LinkA < State->Nb->MaxLinks) && (LinkB < State->Nb->MaxLinks));

  SocketA = State->HtInterface->GetSocketFromMap (NodeA, State);
  PackageLinkA = State->Nb->GetPackageLink (NodeA, LinkA, State->Nb);
  SocketB = State->HtInterface->GetSocketFromMap (NodeB, State);
  PackageLinkB = State->Nb->GetPackageLink (NodeB, LinkB, State->Nb);

  if (State->HtBlock->CpuToCpuPcbLimitsList != NULL) {
    p = State->HtBlock->CpuToCpuPcbLimitsList;

    while (p->SocketA != HT_LIST_TERMINAL) {
      if (((p->SocketA == SocketA) || (p->SocketA == HT_LIST_MATCH_ANY)) &&
          ((p->LinkA == PackageLinkA) || ((p->LinkA == HT_LIST_MATCH_ANY) && (!IsPackageLinkInternal (PackageLinkA))) ||
           ((p->LinkA == HT_LIST_MATCH_INTERNAL_LINK) && (IsPackageLinkInternal (PackageLinkA)))) &&
          ((p->SocketB == SocketB) || (p->SocketB == HT_LIST_MATCH_ANY)) &&
          ((p->LinkB == PackageLinkB) || ((p->LinkB == HT_LIST_MATCH_ANY) && (!IsPackageLinkInternal (PackageLinkB))) ||
           ((p->LinkB == HT_LIST_MATCH_INTERNAL_LINK) && (IsPackageLinkInternal (PackageLinkB))))) {
        // Found a match, update width and frequency
        *ABLinkWidthLimit = p->ABLinkWidthLimit;
        *BALinkWidthLimit = p->BALinkWidthLimit;
        *PcbFreqCap = p->PcbFreqCap;
        break;
      } else {
        p++;
      }
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Skip reganging of subLinks.
 *
 * @HtInterfaceMethod{::F_GET_SKIP_REGANG}
 *
 * This routine is called whenever two subLinks are both connected to the same CPUs.
 * Normally, unganged sublinks between the same two CPUs are reganged.  Return true
 * from this routine to leave the Links unganged.
 *
 * @param[in]     NodeA    One Node on which this Link is located
 * @param[in]     LinkA    The Link on this Node
 * @param[in]     NodeB    The other Node on which this Link is located
 * @param[in]     LinkB    The Link on that Node
 * @param[in]     State     the input data
 *
 * @retval        MATCHED      leave Link unganged
 * @retval        POWERED_OFF  leave link unganged and power off the paired sublink
 * @retval        UNMATCHED    regang Link automatically
 */
FINAL_LINK_STATE
GetSkipRegang (
  IN       UINT8        NodeA,
  IN       UINT8        LinkA,
  IN       UINT8        NodeB,
  IN       UINT8        LinkB,
  IN       STATE_DATA   *State
  )
{
  SKIP_REGANG *p;
  FINAL_LINK_STATE Result;
  UINT8 SocketA;
  UINT8 SocketB;
  UINT8 PackageLinkA;
  UINT8 PackageLinkB;

  ASSERT ((NodeA < MAX_NODES) && (NodeB < MAX_NODES));
  ASSERT ((LinkA < State->Nb->MaxLinks) && (LinkB < State->Nb->MaxLinks));

  Result = UNMATCHED;
  SocketA = State->HtInterface->GetSocketFromMap (NodeA, State);
  PackageLinkA = State->Nb->GetPackageLink (NodeA, LinkA, State->Nb);
  SocketB = State->HtInterface->GetSocketFromMap (NodeB, State);
  PackageLinkB = State->Nb->GetPackageLink (NodeB, LinkB, State->Nb);

  if (State->HtBlock->SkipRegangList != NULL) {
    p = State->HtBlock->SkipRegangList;

    while (p->SocketA != HT_LIST_TERMINAL) {
      if (((p->SocketA == SocketA) || (p->SocketA == HT_LIST_MATCH_ANY)) &&
          ((p->LinkA == PackageLinkA) || ((p->LinkA == HT_LIST_MATCH_ANY) && (!IsPackageLinkInternal (PackageLinkA))) ||
            ((p->LinkA == HT_LIST_MATCH_INTERNAL_LINK) && (IsPackageLinkInternal (PackageLinkA)))) &&
          ((p->SocketB == SocketB) || (p->SocketB == HT_LIST_MATCH_ANY)) &&
          ((p->LinkB == PackageLinkB) || ((p->LinkB == HT_LIST_MATCH_ANY) && (!IsPackageLinkInternal (PackageLinkB))) ||
            ((p->LinkB == HT_LIST_MATCH_INTERNAL_LINK) && (IsPackageLinkInternal (PackageLinkB))))) {
        // Found a match return final link state
        Result = p->LinkState;
        break;
      } else {
        p++;
      }
    }
  }
  return Result;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get a new, empty Hop Count Table, to make one for the installed topology.
 *
 * @HtInterfaceMethod{::F_NEW_HOP_COUNT_TABLE}
 *
 * For SLIT, publish a matrix with the hop count, by allocating a buffer on heap with a
 * known signature.
 *
 * @param[in,out]   State    Keep our buffer handle.
 *
 */
VOID
NewHopCountTable (
  IN OUT   STATE_DATA     *State
  )
{
  ALLOCATE_HEAP_PARAMS AllocHeapParams;

  AllocHeapParams.RequestedBufferSize = sizeof (HOP_COUNT_TABLE);
  AllocHeapParams.BufferHandle = HOP_COUNT_TABLE_HANDLE;
  AllocHeapParams.Persist = HEAP_SYSTEM_MEM;
  if (HeapAllocateBuffer ( &AllocHeapParams, State->ConfigHandle) == AGESA_SUCCESS) {
    State->HopCountTable = (HOP_COUNT_TABLE *)AllocHeapParams.BufferPtr;
  } else {
    State->HopCountTable = NULL;
  }
}
