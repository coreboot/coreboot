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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
 *****************************************************************************
 *
 * Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *
 * AMD is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with AMD.  This header does *NOT* give you permission to use the Materials
 * or any rights under AMD's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 *
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by AMD shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 *
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 *
 * AMD does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by AMD, or
 * result from use of the Materials or any related information.
 *
 * You agree that you will not reverse engineer or decompile the Materials.
 *
 * NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, AMD retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgement of AMD's proprietary rights in them.
 *
 * EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
 * direct product thereof will be exported directly or indirectly, into any
 * country prohibited by the United States Export Administration Act and the
 * regulations thereunder, without the required authorization from the U.S.
 * government nor will be used for any purpose prohibited by the same.
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
RDATA_GROUP (G1_PEICC)

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
