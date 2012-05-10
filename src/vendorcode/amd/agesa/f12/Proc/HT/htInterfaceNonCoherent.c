/* $NoKeywords:$ */
/**
 * @file
 *
 * External Interface implementation for non-coherent features.
 *
 * Contains routines for accessing the interface to the client BIOS,
 * for non-coherent features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  HyperTransport
 * @e \$Revision: 44324 $   @e \$Date: 2010-12-22 17:16:51 +0800 (Wed, 22 Dec 2010) $
 *
 */
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
#include "htInterfaceNonCoherent.h"
#include "htNb.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_HT_HTINTERFACENONCOHERENT_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define UNUSED_ZERO_32 ((UINT32)0)

/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
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
/**
 * Get Manual BUID assignment list.
 *
 * @HtInterfaceMethod{::F_GET_MANUAL_BUID_SWAP_LIST}
 *
 * This routine is called every time a non-coherent chain is processed.  BUID
 * assignment may be controlled explicitly on a non-coherent chain. Swaps controls
 * the BUID assignment and FinalIds provides the device to device Linking.  Device
 * orientation can be detected automatically, or explicitly.  See documentation for
 * more details.
 *
 * If a manual swap list is not supplied, automatic non-coherent init assigns BUIDs
 * starting at 1 and incrementing sequentially based on each device's unit count.
 *
 * @param[in]    Node    The Node on which this chain is located
 * @param[in]    Link    The Link on the host for this chain
 * @param[out]   List    supply a pointer to a list.
 *                       List is NOT valid unless routine returns TRUE.
 * @param[in]    State   the input data
 *
 * @retval      TRUE     use a manual list
 * @retval      FALSE    initialize the Link automatically
 */
BOOLEAN
GetManualBuidSwapList (
  IN       UINT8            Node,
  IN       UINT8            Link,
     OUT   BUID_SWAP_LIST   **List,
  IN       STATE_DATA       *State
  )
{
  MANUAL_BUID_SWAP_LIST *p;
  BOOLEAN result;
  UINT8 Socket;
  UINT8 PackageLink;

  ASSERT ((Node < MAX_NODES) && (List != NULL));

  result = FALSE;
  Socket = State->HtInterface->GetSocketFromMap (Node, State);
  PackageLink = State->Nb->GetPackageLink (Node, Link, State->Nb);

  if (State->HtBlock->ManualBuidSwapList != NULL) {
    p = State->HtBlock->ManualBuidSwapList;

    while (p->Socket != HT_LIST_TERMINAL) {
      if (((p->Socket == Socket) || (p->Socket == HT_LIST_MATCH_ANY)) &&
          ((p->Link == PackageLink) || (p->Link == HT_LIST_MATCH_ANY))) {
        // Found a match implies TRUE, ignore the Link
        result = TRUE;
        *List = &(p->SwapList);
        break;
      } else {
        p++;
      }
    }
  }
  // List is not valid if Result is FALSE.
  return result;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Override capabilities of a device.
 *
 * @HtInterfaceMethod{::F_GET_DEVICE_CAP_OVERRIDE}
 *
 * This routine is called once for every Link on every IO device.  Update the width
 * and frequency capability if needed for this device.  This is used along with
 * device capabilities, the limit call backs, and northbridge limits to compute the
 * default settings.  The components of the device's PCI config address are provided,
 * so its settings can be consulted if need be. The input width and frequency are the
 * reported device capabilities.
 *
 * @param[in]     HostNode      The Node on which this chain is located
 * @param[in]     HostLink      The Link on the host for this chain
 * @param[in]     Depth         The Depth in the I/O chain from the Host
 * @param[in]     PciAddress    The Device's PCI config address (for callout)
 * @param[in]     DevVenId      The Device's PCI Vendor + Device ID (offset 0x00)
 * @param[in]     Revision      The Device's PCI Revision
 * @param[in]     Link          The Device's Link number (0 or 1)
 * @param[in,out] LinkWidthIn   modify to change the Link Width In
 * @param[in,out] LinkWidthOut  modify to change the Link Width Out
 * @param[in,out] FreqCap       modify to change the Link's frequency capability
 * @param[in,out] Clumping      modify to change unit id clumping capability
 * @param[in]     State         the input data and config header
 *
 */
VOID
GetDeviceCapOverride (
  IN       UINT8       HostNode,
  IN       UINT8       HostLink,
  IN       UINT8       Depth,
  IN       PCI_ADDR    PciAddress,
  IN       UINT32      DevVenId,
  IN       UINT8       Revision,
  IN       UINT8       Link,
  IN OUT   UINT8       *LinkWidthIn,
  IN OUT   UINT8       *LinkWidthOut,
  IN OUT   UINT32      *FreqCap,
  IN OUT   UINT32      *Clumping,
  IN       STATE_DATA  *State
  )
{
  DEVICE_CAP_OVERRIDE *p;
  UINT8 HostSocket;
  UINT8 PackageLink;
  DEVICE_CAP_CALLOUT_PARAMS CalloutParams;
  AGESA_STATUS CalloutStatus;

  ASSERT ((HostNode < MAX_NODES) && (Depth < 32) && ((Link == 0) || (Link == 1)));

  HostSocket = State->HtInterface->GetSocketFromMap (HostNode, State);
  PackageLink = State->Nb->GetPackageLink (HostNode, HostLink, State->Nb);

  if (State->HtBlock->DeviceCapOverrideList != NULL) {
    p = State->HtBlock->DeviceCapOverrideList;

    while (p->HostSocket != HT_LIST_TERMINAL) {
      if (((p->HostSocket == HostSocket) || (p->HostSocket == HT_LIST_MATCH_ANY)) &&
          ((p->HostLink == PackageLink) || (p->HostLink == HT_LIST_MATCH_ANY)) &&
          ((p->Depth == Depth) || (p->Depth == HT_LIST_MATCH_ANY)) &&
          ((p->Link == Link) || (p->Link == HT_LIST_MATCH_ANY)) &&
          // Found a potential match. Check the additional optional matches.
          ((p->Options.IsCheckDevVenId == 0) || (p->DevVenId == DevVenId)) &&
          ((p->Options.IsCheckRevision == 0) || (p->Revision == Revision))) {
        //
        // Found a match.  Check what override actions are desired.
        // Unlike the PCB limit routines, which handle the info returned,
        // deviceCapOverride is actually overriding the settings, so we need
        // to check that the field actually has an update.
        // The Callout is a catch all for situations the data is not up to handling.
        // It is expected, but not enforced, that either the data overrides are used,
        // or the callout is used, rather than both.
        //
        if (p->Options.IsOverrideWidthIn != 0) {
          *LinkWidthIn = p->LinkWidthIn;
        }
        if (p->Options.IsOverrideWidthOut != 0) {
          *LinkWidthOut = p->LinkWidthOut;
        }
        if (p->Options.IsOverrideFreq != 0) {
          *FreqCap = p->FreqCap;
        }
        if (p->Options.IsOverrideClumping != 0) {
          *Clumping = p->Clumping;
        }
        if (p->Options.IsDoCallout != 0) {
          //
          // Pass the actual info being matched, not the matched struct data.
          // This callout is expected to be built in as part of the options file, and does not use the
          // callout interface, even though we use the consistent interface declaration for the routine.
          // So, the first two int parameters have no meaning in this case.
          // It is not meaningful for the callout to have any status but Success.
          //
          CalloutParams.HostSocket = HostSocket;
          CalloutParams.HostLink = PackageLink;
          CalloutParams.Depth = Depth;
          CalloutParams.DevVenId = DevVenId;
          CalloutParams.Revision = Revision;
          CalloutParams.Link = Link;
          CalloutParams.PciAddress = PciAddress;
          CalloutParams.LinkWidthIn = LinkWidthIn;
          CalloutParams.LinkWidthOut = LinkWidthOut;
          CalloutParams.FreqCap = FreqCap;
          CalloutParams.Clumping = Clumping;
          CalloutParams.StdHeader = *((AMD_CONFIG_PARAMS *) (State->ConfigHandle));
          CalloutStatus = p->Callout (UNUSED_ZERO_32, UNUSED_ZERO_32, (VOID *) &CalloutParams);
          ASSERT (CalloutStatus == AGESA_SUCCESS);
        }
        break;
      } else {
        p++;
      }
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get limits for non-coherent Links.
 *
 * @HtInterfaceMethod{::F_GET_IO_PCB_LIMITS}
 *
 * For each non-coherent connection this routine is called once.  Update the
 * frequency and width if needed for this Link (usually based on board restriction).
 * This is used with device capabilities, device overrides, and northbridge limits to
 * compute the default settings. The input width and frequency are valid, but do not
 * necessarily reflect the minimum setting that will be chosen.
 *
 * @param[in]     HostNode                   The Node on which this Link is located
 * @param[in]     HostLink                   The Link about to be initialized
 * @param[in]     Depth                      The Depth in the I/O chain from the Host
 * @param[in,out] DownstreamLinkWidthLimit   modify to change the Link Width In
 * @param[in,out] UpstreamLinkWidthLimit     modify to change the Link Width Out
 * @param[in,out] PcbFreqCap                 modify to change the Link's frequency capability
 * @param[in]     State                      the input data
 */
VOID
GetIoPcbLimits (
  IN       UINT8        HostNode,
  IN       UINT8        HostLink,
  IN       UINT8        Depth,
  IN OUT   UINT8        *DownstreamLinkWidthLimit,
  IN OUT   UINT8        *UpstreamLinkWidthLimit,
  IN OUT   UINT32       *PcbFreqCap,
  IN       STATE_DATA   *State
  )
{
  IO_PCB_LIMITS *p;
  UINT8 Socket;
  UINT8 PackageLink;

  ASSERT ((HostNode < MAX_NODES) && (HostLink < MAX_NODES));

  Socket = State->HtInterface->GetSocketFromMap (HostNode, State);
  PackageLink = State->Nb->GetPackageLink (HostNode, HostLink, State->Nb);

  if (State->HtBlock->IoPcbLimitsList != NULL) {
    p = State->HtBlock->IoPcbLimitsList;

    while (p->HostSocket != HT_LIST_TERMINAL) {
      if (((p->HostSocket == Socket) || (p->HostSocket == HT_LIST_MATCH_ANY)) &&
          ((p->HostLink == PackageLink) || (p->HostLink == HT_LIST_MATCH_ANY)) &&
          ((p->Depth == Depth) || (p->Depth == HT_LIST_MATCH_ANY))) {
        // Found a match, return the override info
        *DownstreamLinkWidthLimit = p->DownstreamLinkWidthLimit;
        *UpstreamLinkWidthLimit = p->UpstreamLinkWidthLimit;
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
 * Manually control bus number assignment.
 *
 * @HtInterfaceMethod{::F_GET_OVERRIDE_BUS_NUMBERS}
 *
 * This routine is called every time a non-coherent chain is processed.  If a system
 * can not use the auto Bus numbering feature for non-coherent chain bus assignments,
 * this routine can provide explicit control.  For each chain, provide the bus number
 * range to use.
 *
 * The outputs SecBus and SubBus are not valid unless this routine returns TRUE
 *
 * @param[in]     Node      The Node on which this chain is located
 * @param[in]     Link      The Link on the host for this chain
 * @param[out]    SecBus    Secondary Bus number for this non-coherent chain
 * @param[out]    SubBus    Subordinate Bus number
 * @param[in]     State     the input data
 *
 * @retval        TRUE      this routine is supplying the bus numbers.
 * @retval        FALSE     use auto Bus numbering, bus outputs not valid.
 */
BOOLEAN
GetOverrideBusNumbers (
  IN       UINT8        Node,
  IN       UINT8        Link,
     OUT   UINT8        *SecBus,
     OUT   UINT8        *SubBus,
  IN       STATE_DATA   *State
  )
{
  OVERRIDE_BUS_NUMBERS *p;
  BOOLEAN result;
  UINT8 Socket;
  UINT8 PackageLink;

  ASSERT ((Node < MAX_NODES) && (Link < MAX_NODES));

  result = FALSE;
  Socket = State->HtInterface->GetSocketFromMap (Node, State);
  PackageLink = State->Nb->GetPackageLink (Node, Link, State->Nb);

  if (State->HtBlock->OverrideBusNumbersList != NULL) {
    p = State->HtBlock->OverrideBusNumbersList;

    while (p->Socket != HT_LIST_TERMINAL) {
      if (((p->Socket == Socket) || (p->Socket == HT_LIST_MATCH_ANY)) &&
          ((p->Link == PackageLink) || (p->Link == HT_LIST_MATCH_ANY))) {
        // Found a match, return the bus overrides
        *SecBus = p->SecBus;
        *SubBus = p->SubBus;
        ASSERT (*SubBus > *SecBus);
        result = TRUE;
        break;
      } else {
        p++;
      }
    }
  }
  // SecBus, SubBus are not valid if Result is FALSE.
  return result;
}

