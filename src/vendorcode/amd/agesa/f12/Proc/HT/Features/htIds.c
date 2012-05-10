/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD IDS HyperTransport Implementation.
 *
 * Contains AMD AGESA Integrated Debug HT related support.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  HyperTransport
 * @e \$Revision: 44324 $   @e \$Date: 2010-12-22 17:16:51 +0800 (Wed, 22 Dec 2010) $
 */
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
 ***************************************************************************/


#include "AGESA.h"
#include "amdlib.h"
#include "Ids.h"
#include "Topology.h"
#include "htFeat.h"
#include "IdsHt.h"
#include "htInterface.h"
#include "htInterfaceGeneral.h"
#include "htNb.h"
#include "heapManager.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)
#define FILECODE PROC_HT_FEATURES_HTIDS_FILECODE


/*-------------------------------------------------------------------------------------*/
/**
 * Apply an IDS port override to the desired HT link.
 *
 * The IDS port override allows absolute control of a link's frequency and width, such as
 * would be used for board characterization and test.  The IDS backend code is responsible
 * for handling the NV items and building them into a port override list.  Here we search
 * that list for any overrides which apply, and update the data used by the HT feature code.
 *
 * @param[in]      IsSourcePort         Since we handle both ports on a match, only do that if TRUE.
 * @param[in,out]  Port0                The PORTLIST item for the first endpoint of a link.
 * @param[in,out]  Port1                The PORTLIST item for the second endpoint of a link.
 * @param[in,out]  PortOverrideList     IN: A pointer to the port override list or NULL,
 *                                      OUT: A pointer to the port override list.
 * @param[in]      State                access to ht interface and nb support methods.
 *
 */
VOID
HtIdsGetPortOverride (
  IN       BOOLEAN                    IsSourcePort,
  IN OUT   PORT_DESCRIPTOR            *Port0,
  IN OUT   PORT_DESCRIPTOR            *Port1,
  IN OUT   HTIDS_PORT_OVERRIDE_LIST   *PortOverrideList,
  IN       STATE_DATA                 *State
  )
{
  LOCATE_HEAP_PTR LocHeapParams;
  UINT8 SocketA;
  UINT8 SocketB;
  UINT8 PackageLinkA;
  UINT8 PackageLinkB;
  HTIDS_PORT_OVERRIDE_LIST   p;

  if (IsSourcePort) {
    ASSERT (PortOverrideList != NULL);
    // The caller can cache the override list by providing the pointer (to the heap buffer).
    // If the pointer to the port override list is null, then check if it is on the heap,
    // and update the caller's pointer so it is cached.
    // If the buffer is not in heap, call the IDS backend to get the NV data (which is likely also
    // in heap).
    if (*PortOverrideList == NULL) {
      // locate the table in heap
      LocHeapParams.BufferHandle = IDS_HT_DATA_HANDLE;
      if (HeapLocateBuffer (&LocHeapParams, State->ConfigHandle) == AGESA_SUCCESS) {
        *PortOverrideList = (HTIDS_PORT_OVERRIDE_LIST)LocHeapParams.BufferPtr;
      } else {
        // Ask IDS backend code for the list
        IDS_OPTION_HOOK (IDS_HT_CONTROL, PortOverrideList, State->ConfigHandle);
      }
    }
    ASSERT (*PortOverrideList != NULL);

    // Search the port override list to see if there is an override that applies to this link.
    // The match criteria are if either endpoint of the current port list item matches
    // port override.
    p = *PortOverrideList;
    SocketA = State->HtInterface->GetSocketFromMap (Port0->NodeID, State);
    PackageLinkA = State->Nb->GetPackageLink (Port0->NodeID, Port0->Link, State->Nb);
    SocketB = State->HtInterface->GetSocketFromMap (Port1->NodeID, State);
    PackageLinkB = State->Nb->GetPackageLink (Port1->NodeID, Port1->Link, State->Nb);

    while ((p != NULL) && (p->Socket != HT_LIST_TERMINAL)) {
      if (((p->Socket == SocketA) || (p->Socket == HT_LIST_MATCH_ANY)) &&
          ((p->Link == PackageLinkA) ||
           ((p->Link == HT_LIST_MATCH_ANY) && (!IsPackageLinkInternal (PackageLinkA))) ||
           ((p->Link == HT_LIST_MATCH_INTERNAL_LINK) && (IsPackageLinkInternal (PackageLinkA)))) ||
          ((p->Socket == SocketB) || (p->Socket == HT_LIST_MATCH_ANY)) &&
          ((p->Link == PackageLinkB) ||
           ((p->Link == HT_LIST_MATCH_ANY) && (!IsPackageLinkInternal (PackageLinkA))) ||
           ((p->Link == HT_LIST_MATCH_INTERNAL_LINK) && (IsPackageLinkInternal (PackageLinkB))))) {
        // Found a match, update width and frequency of both endpoints.
        if (p->WidthIn != HT_LIST_TERMINAL) {
          Port0->SelWidthIn = p->WidthIn;
          Port1->SelWidthOut = p->WidthIn;
        }
        if (p->WidthOut != HT_LIST_TERMINAL) {
          Port0->SelWidthOut = p->WidthOut;
          Port1->SelWidthIn = p->WidthOut;
        }
        if (p->Frequency != HT_LIST_TERMINAL) {
          Port0->SelFrequency = p->Frequency;
          Port1->SelFrequency = p->Frequency;
        }
        break;
      } else {
        p++;
      }
    }
  }
}

