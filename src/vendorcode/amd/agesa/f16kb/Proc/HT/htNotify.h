/* $NoKeywords:$ */
/**
 * @file
 *
 * HT Notify interface.
 *
 * This file provides internal interface to event and status
 * notification.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  HyperTransport
 * @e \$Revision: 84150 $        @e  \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 */
/*
*****************************************************************************
*
 * Copyright (c) 2008 - 2013, Advanced Micro Devices, Inc.
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

#ifndef _HT_NOTIFY_H_
#define _HT_NOTIFY_H_

/*----------------------------------------------------------------------------------------*/
/* Event specific event data definitions.
 * All structures must be 4 UINT32's in size, no more, no less.
 */

/// For event ::HT_EVENT_HW_SYNCFLOOD
typedef struct {
  UINT32 Node;                   ///< The Node on which observed
  UINT32 Link;                   ///< The Link on that Node which reported synch flood
  UINT32 Reserved1;              ///< Reserved.
  UINT32 Reserved2;              ///< Reserved.
} HT_EVENT_DATA_HW_SYNCFLOOD;

/// For event ::HT_EVENT_HW_HTCRC
typedef struct {
  UINT32 Node;                   ///< The Node on which event is observed
  UINT32 Link;                   ///< The Link on that Node which reported CRC error
  UINT32 LaneMask;               ///< The CRC lane mask for the Link
  UINT32 Reserved1;              ///< Reserved.
} HT_EVENT_DATA_HW_HT_CRC;

/// For event ::HT_EVENT_NCOH_BUS_MAX_EXCEED
typedef struct {
  UINT32 Node;                   ///< the Node with this non-coherent chain
  UINT32 Link;                   ///< the Link on that Node to this chain
  UINT32 Bus;                    ///< the current bus number
  UINT32 Reserved1;              ///< Reserved.
} HT_EVENT_DATA_NCOH_BUS_MAX_EXCEED;

/// For event ::HT_EVENT_NCOH_CFG_MAP_EXCEED
typedef struct {
  UINT32 Node;                   ///< the Node with this non-coherent chain
  UINT32 Link;                   ///< the Link on that Node to this chain
  UINT32 Reserved1;              ///< Reserved.
  UINT32 Reserved2;              ///< Reserved.
} HT_EVENT_DATA_NCOH_CFG_MAP_EXCEED;

/// For event ::HT_EVENT_NCOH_BUID_EXCEED
typedef struct {
  UINT32 Node;                   ///< the Node with this non-coherent chain
  UINT32 Link;                   ///< the Link on that Node to this chain
  UINT32 Depth;                  ///< the position on the chain, zero is CPU host
  UINT16 CurrentBuid;            ///< the current available BUID
  UINT16 UnitCount;              ///< the number of ids which would be consumed by this device
} HT_EVENT_DATA_NCOH_BUID_EXCEED;

/// For event ::HT_EVENT_NCOH_DEVICE_FAILED
typedef struct {
  UINT32 Node;                   ///< the Node with this non-coherent chain
  UINT32 Link;                   ///< the Link on that Node to this chain
  UINT32 Depth;                  ///< the position on the chain, zero is CPU host
  UINT32 AttemptedBuid;          ///< the BUID we tried to assign to that device
} HT_EVENT_DATA_NCOH_DEVICE_FAILED;

/// For event ::HT_EVENT_NCOH_AUTO_DEPTH
typedef struct {
  UINT32 Node;                   ///< the Node with this non-coherent chain
  UINT32 Link;                   ///< the Link on that Node to this chain
  UINT32 Depth;                  ///< the position on the chain of the last device, zero is CPU host
  UINT32 Reserved1;              ///< Reserved.
} HT_EVENT_DATA_NCOH_AUTO_DEPTH;

/// For event ::HT_EVENT_OPT_REQUIRED_CAP_RETRY,
///           ::HT_EVENT_OPT_REQUIRED_CAP_GEN3.
typedef struct {
  UINT32 Node;                   ///< the Node with this non-coherent chain
  UINT32 Link;                   ///< the Link on that Node to this chain
  UINT32 Depth;                  ///< the position on the chain, zero is CPU host
  UINT32 Reserved1;              ///< Reserved.
} HT_EVENT_DATA_OPT_REQUIRED_CAP;

/// For event ::HT_EVENT_OPT_UNUSED_LINKS
typedef struct {
  UINT32 NodeA;                  ///< One of the nodes connected
  UINT32 LinkA;                  ///< its unusable link
  UINT32 NodeB;                  ///< The other connected node
  UINT32 LinkB;                  ///< its unusable link
} HT_EVENT_DATA_OPT_UNUSED_LINKS;

/// For event ::HT_EVENT_OPT_LINK_PAIR_EXCEED
typedef struct {
  UINT32 NodeA;                  ///< One of the nodes connected
  UINT32 NodeB;                  ///< The other connected node
  UINT32 MasterLink;             ///< NodeA's unusable Master link
  UINT32 AltLink;                ///< NodeA's unusable Alternatelink
} HT_EVENT_DATA_OPT_LINK_PAIR_EXCEED;

/// For event ::HT_EVENT_COH_NO_TOPOLOGY.
/// There is no routing for this system's topology.
typedef struct {
  UINT32 TotalNodes;             ///< the number of Nodes in the unmatched topology
  UINT32 Reserved1;              ///< Reserved.
  UINT32 Reserved2;              ///< Reserved.
  UINT32 Reserved3;              ///< Reserved.
} HT_EVENT_DATA_COH_NO_TOPOLOGY;

/// For event ::HT_EVENT_COH_PROCESSOR_TYPE_MIX
typedef struct {
  UINT32 Node;                   ///< the Node from which the incompatible family was found
  UINT32 Link;                   ///< the Link to the incompatible Node
  UINT32 TotalNodes;             ///< the number of Nodes found at that point
  UINT32 Reserved1;              ///< Reserved.
} HT_EVENT_DATA_COH_PROCESSOR_TYPE_MIX;

/// For event ::HT_EVENT_COH_NODE_DISCOVERED
typedef struct {
  UINT32 Node;                   ///< the Node from which the new Node was found
  UINT32 Link;                   ///< the Link to the new Node
  UINT32 NewNode;                ///< the Node id of the newly discovered Node
  UINT32 TempRoute;              ///< the new Node is temporarily at this id
} HT_EVENT_DATA_COH_NODE_DISCOVERED;

/// For event ::HT_EVENT_COH_MPCAP_MISMATCH
typedef struct {
  UINT32 Node;                   ///< the Node from which condition was observed
  UINT32 Link;                   ///< the Link on the current Node
  UINT32 SysMpCap;               ///< the current aggregate system capability (the minimum found so far)
  UINT32 TotalNodes;             ///< the number of Nodes found, before this was observed
} HT_EVENT_DATA_COH_MP_CAP_MISMATCH;

/*----------------------------------------------------------------------------------------*/
/* Event specific Notify functions.
 */

VOID
NotifyAlertHwSyncFlood (
  IN       UINT8        Node,
  IN       UINT8        Link,
  IN       STATE_DATA   *State
  );

VOID
NotifyAlertHwHtCrc (
  IN       UINT8      Node,
  IN       UINT8      Link,
  IN       UINT8      LaneMask,
  IN       STATE_DATA *State
  );

VOID
NotifyErrorNcohBusMaxExceed (
  IN       UINT8      Node,
  IN       UINT8      Link,
  IN       UINT8      Bus,
  IN       STATE_DATA *State
  );

VOID
NotifyErrorNcohCfgMapExceed (
  IN       UINT8      Node,
  IN       UINT8      Link,
  IN       STATE_DATA *State
  );

VOID
NotifyErrorNcohBuidExceed (
  IN       UINT8      Node,
  IN       UINT8      Link,
  IN       UINT8      Depth,
  IN       UINT8      Id,
  IN       UINT8      Units,
  IN       STATE_DATA *State
  );

VOID
NotifyErrorNcohDeviceFailed (
  IN       UINT8      Node,
  IN       UINT8      Link,
  IN       UINT8      Depth,
  IN       UINT8      Id,
  IN       STATE_DATA *State
  );

VOID
NotifyInfoNcohAutoDepth (
  IN       UINT8      Node,
  IN       UINT8      Link,
  IN       UINT8      Depth,
  IN       STATE_DATA *State
  );

VOID
NotifyWarningOptRequiredCapRetry (
  IN       UINT8      Node,
  IN       UINT8      Link,
  IN       UINT8      Depth,
  IN       STATE_DATA *State
  );

VOID
NotifyWarningOptRequiredCapGen3 (
  IN       UINT8      Node,
  IN       UINT8      Link,
  IN       UINT8      Depth,
  IN       STATE_DATA *State
  );

VOID
NotifyWarningOptUnusedLinks (
  IN       UINT32    NodeA,
  IN       UINT32    LinkA,
  IN       UINT32    NodeB,
  IN       UINT32    LinkB,
  IN       STATE_DATA *State
  );

VOID
NotifyWarningOptLinkPairExceed (
  IN       UINT32    NodeA,
  IN       UINT32    NodeB,
  IN       UINT32    MasterLink,
  IN       UINT32    AltLink,
  IN       STATE_DATA *State
  );

VOID
NotifyErrorCohNoTopology (
  IN       UINT8      Nodes,
  IN       STATE_DATA *State
  );

VOID
NotifyFatalCohProcessorTypeMix (
  IN       UINT8      Node,
  IN       UINT8      Link,
  IN       UINT8      Nodes,
  IN       STATE_DATA *State
  );

VOID
NotifyInfoCohNodeDiscovered (
  IN       UINT8      Node,
  IN       UINT8      Link,
  IN       UINT8      NewNode,
  IN       UINT8      TempRoute,
  IN       STATE_DATA *State
  );

VOID
NotifyFatalCohMpCapMismatch (
  IN       UINT8      Node,
  IN       UINT8      Link,
  IN       UINT8      Cap,
  IN       UINT8      Nodes,
  IN       STATE_DATA *State
  );

#endif /* _HT_NOTIFY_H_ */
