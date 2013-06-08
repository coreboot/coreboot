/* $NoKeywords:$ */
/**
 * @file
 *
 * Code for detailed notification of events and status.
 *
 * Routines for logging and reporting details and summary status.
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
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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
#include "htNotify.h"
#include "GeneralServices.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_HT_HTNOTIFY_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Log an event.
 *
 * Errors, events, faults, warnings, and useful information are provided by
 * calling this routine as often as necessary, once for each notification.
 * @sa AGESA.h for class, and event definitions.
 * @sa htNotify.h for event data definitions.
 *
 * @param[in]     EvtClass      What level event is this
 * @param[in]     Event         A unique ID of this event
 * @param[in]     EventData     useful data associated with the event.
 * @param[in]     State         the log area and remaining free space
 */
VOID
STATIC
setEventNotify (
  IN       AGESA_STATUS  EvtClass,
  IN       UINT32        Event,
  IN       CONST UINT8  *EventData,
  IN       STATE_DATA   *State
  )
{
  UINT32 DataParam[NUMBER_OF_EVENT_DATA_PARAMS];

  // Remember the highest event class notified, that becomes our return code.
  if (State->MaxEventClass < EvtClass) {
    State->MaxEventClass = EvtClass;
  }

  // Copy the event data to the log data
  LibAmdMemCopy (
    DataParam,
    (VOID *)EventData,
    (sizeof(UINT32) * NUMBER_OF_EVENT_DATA_PARAMS),
    State->ConfigHandle
    );

  // Log the event
  PutEventLog (
    EvtClass,
    Event,
    DataParam[0],
    DataParam[1],
    DataParam[2],
    DataParam[3],
    State->ConfigHandle
    );
}

/*----------------------------------------------------------------------------------------*/
/**
 * For event HT_EVENT_HW_SYNCFLOOD
 *
 * @param[in] Node      The node on which the fault is reported
 * @param[in] Link      The link from that node
 * @param[in] State     our State
 *
 */
VOID
NotifyAlertHwSyncFlood (
  IN       UINT8        Node,
  IN       UINT8        Link,
  IN       STATE_DATA   *State
  )
{
  HT_EVENT_DATA_HW_SYNCFLOOD Evt;
  // Zero out the event data
  LibAmdMemFill (
    &Evt,
    0,
    (sizeof(UINT32) * NUMBER_OF_EVENT_DATA_PARAMS),
    State->ConfigHandle
    );

  IDS_HDT_CONSOLE (HT_TRACE, "Sync Flood on Node %d Link %d.\n", Node, Link);
  Evt.Node = Node;
  Evt.Link = Link;
  setEventNotify (AGESA_ALERT,
                 HT_EVENT_HW_SYNCFLOOD,
                 (UINT8 *)&Evt, State);
}

/*----------------------------------------------------------------------------------------*/
/**
 * For event HT_EVENT_HW_HTCRC
 *
 * @param[in] Node      The node on which the error is reported
 * @param[in] Link      The link from that node
 * @param[in] LaneMask  The lanes which had CRC
 * @param[in] State     our State
 *
 */
VOID
NotifyAlertHwHtCrc (
  IN       UINT8      Node,
  IN       UINT8      Link,
  IN       UINT8      LaneMask,
  IN       STATE_DATA *State
  )
{
  HT_EVENT_DATA_HW_HT_CRC Evt;
  // Zero out the event data
  LibAmdMemFill (
    &Evt,
    0,
    (sizeof(UINT32) * NUMBER_OF_EVENT_DATA_PARAMS),
    State->ConfigHandle
    );

  IDS_HDT_CONSOLE (HT_TRACE, "CRC Error on Node %d Link %d lanes %x.\n", Node, Link, LaneMask);
  Evt.Node = Node;
  Evt.Link = Link;
  Evt.LaneMask = LaneMask;
  setEventNotify (AGESA_ALERT,
                 HT_EVENT_HW_HTCRC,
                 (UINT8 *)&Evt, State);
}

/*----------------------------------------------------------------------------------------*/
/**
 * For event HT_EVENT_NCOH_BUS_MAX_EXCEED
 *
 * @param[in] Node      The node on which the chain is located
 * @param[in] Link      The link from that node
 * @param[in] Bus       The bus number to assign
 * @param[in] State     our State
 *
 */
VOID
NotifyErrorNcohBusMaxExceed (
  IN       UINT8      Node,
  IN       UINT8      Link,
  IN       UINT8      Bus,
  IN       STATE_DATA *State
  )
{
  HT_EVENT_DATA_NCOH_BUS_MAX_EXCEED Evt;
  // Zero out the event data
  LibAmdMemFill (
    &Evt,
    0,
    (sizeof(UINT32) * NUMBER_OF_EVENT_DATA_PARAMS),
    State->ConfigHandle
    );

  Evt.Node = Node;
  Evt.Link = Link;
  Evt.Bus = Bus;
  setEventNotify (AGESA_ERROR,
                 HT_EVENT_NCOH_BUS_MAX_EXCEED,
                 (UINT8 *)&Evt, State);
}

/*----------------------------------------------------------------------------------------*/
/**
 * For event HT_EVENT_NCOH_CFG_MAP_EXCEED
 *
 * @param[in] Node      The node on which the chain is located
 * @param[in] Link      The link from that node
 * @param[in] State     our State
 *
 */
VOID
NotifyErrorNcohCfgMapExceed (
  IN       UINT8      Node,
  IN       UINT8      Link,
  IN       STATE_DATA *State
  )
{
  HT_EVENT_DATA_NCOH_CFG_MAP_EXCEED Evt;
  // Zero out the event data
  LibAmdMemFill (
    &Evt,
    0,
    (sizeof(UINT32) * NUMBER_OF_EVENT_DATA_PARAMS),
    State->ConfigHandle
    );

  Evt.Node = Node;
  Evt.Link = Link;
  setEventNotify (AGESA_ERROR,
                 HT_EVENT_NCOH_CFG_MAP_EXCEED,
                 (UINT8 *)&Evt, State);
}

/*----------------------------------------------------------------------------------------*/
/**
 * For event HT_EVENT_NCOH_BUID_EXCEED
 *
 * @param[in] Node      The node on which the chain is located
 * @param[in] Link      The link from that node
 * @param[in] Depth     Position on chain
 * @param[in] Id        The Id which was attempted to assigned
 * @param[in] Units     The number of units in this device
 * @param[in] State     our State
 *
 */
VOID
NotifyErrorNcohBuidExceed (
  IN       UINT8      Node,
  IN       UINT8      Link,
  IN       UINT8      Depth,
  IN       UINT8      Id,
  IN       UINT8      Units,
  IN       STATE_DATA *State
  )
{
  HT_EVENT_DATA_NCOH_BUID_EXCEED Evt;
  // Zero out the event data
  LibAmdMemFill (
    &Evt,
    0,
    (sizeof(UINT32) * NUMBER_OF_EVENT_DATA_PARAMS),
    State->ConfigHandle
    );

  Evt.Node = Node;
  Evt.Link = Link;
  Evt.Depth = Depth;
  Evt.CurrentBuid = Id;
  Evt.UnitCount = Units;
  setEventNotify (AGESA_ERROR,
                 HT_EVENT_NCOH_BUID_EXCEED,
                 (UINT8 *)&Evt, State);
}

/*----------------------------------------------------------------------------------------*/
/**
 * For event HT_EVENT_NCOH_DEVICE_FAILED
 *
 * @param[in] Node      The node on which the chain is located
 * @param[in] Link      The link from that node
 * @param[in] Depth     Position on chain
 * @param[in] Id        The Id which was attempted to assigned
 * @param[in] State     our State
 *
 */
VOID
NotifyErrorNcohDeviceFailed (
  IN       UINT8      Node,
  IN       UINT8      Link,
  IN       UINT8      Depth,
  IN       UINT8      Id,
  IN       STATE_DATA *State
  )
{
  HT_EVENT_DATA_NCOH_DEVICE_FAILED Evt;
  // Zero out the event data
  LibAmdMemFill (
    &Evt,
    0,
    (sizeof(UINT32) * NUMBER_OF_EVENT_DATA_PARAMS),
    State->ConfigHandle
    );

  Evt.Node = Node;
  Evt.Link = Link;
  Evt.Depth = Depth;
  Evt.AttemptedBuid = Id;
  setEventNotify (AGESA_ERROR,
                 HT_EVENT_NCOH_DEVICE_FAILED,
                 (UINT8 *)&Evt, State);
}

/*----------------------------------------------------------------------------------------*/
/**
 * For event HT_EVENT_NCOH_AUTO_DEPTH
 *
 * @param[in] Node      The node on which the chain is located
 * @param[in] Link      The link from that node
 * @param[in] Depth     Position on chain
 * @param[in] State     our State
 *
 */
VOID
NotifyInfoNcohAutoDepth (
  IN       UINT8      Node,
  IN       UINT8      Link,
  IN       UINT8      Depth,
  IN       STATE_DATA *State
  )
{
  HT_EVENT_DATA_NCOH_AUTO_DEPTH Evt;
  // Zero out the event data
  LibAmdMemFill (
    &Evt,
    0,
    (sizeof(UINT32) * NUMBER_OF_EVENT_DATA_PARAMS),
    State->ConfigHandle
    );

  Evt.Node = Node;
  Evt.Link = Link;
  Evt.Depth = Depth;
  setEventNotify (AGESA_SUCCESS,
                 HT_EVENT_NCOH_AUTO_DEPTH,
                 (UINT8 *)&Evt, State);
}

/*----------------------------------------------------------------------------------------*/
/**
 * For event HT_EVENT_OPT_REQUIRED_CAP_RETRY
 *
 * @param[in] Node      The node on which the chain is located
 * @param[in] Link      The link from that node
 * @param[in] Depth     Position on chain
 * @param[in] State     our State
 *
 */
VOID
NotifyWarningOptRequiredCapRetry (
  IN       UINT8 Node,
  IN       UINT8 Link,
  IN       UINT8 Depth,
  IN       STATE_DATA *State
  )
{
  HT_EVENT_DATA_OPT_REQUIRED_CAP Evt;
  // Zero out the event data
  LibAmdMemFill (
    &Evt,
    0,
    (sizeof(UINT32) * NUMBER_OF_EVENT_DATA_PARAMS),
    State->ConfigHandle
    );

  Evt.Node = Node;
  Evt.Link = Link;
  Evt.Depth = Depth;
  setEventNotify (AGESA_WARNING,
                 HT_EVENT_OPT_REQUIRED_CAP_RETRY,
                 (UINT8 *)&Evt, State);
}

/*----------------------------------------------------------------------------------------*/
/**
 * For event HT_EVENT_OPT_REQUIRED_CAP_GEN3
 *
 * @param[in] Node      The node on which the chain is located
 * @param[in] Link      The link from that node
 * @param[in] Depth     Position on chain
 * @param[in] State     our State
 *
 */
VOID
NotifyWarningOptRequiredCapGen3 (
  IN       UINT8 Node,
  IN       UINT8 Link,
  IN       UINT8 Depth,
  IN       STATE_DATA *State
  )
{
  HT_EVENT_DATA_OPT_REQUIRED_CAP Evt;
  // Zero out the event data
  LibAmdMemFill (
    &Evt,
    0,
    (sizeof(UINT32) * NUMBER_OF_EVENT_DATA_PARAMS),
    State->ConfigHandle
    );

  Evt.Node = Node;
  Evt.Link = Link;
  Evt.Depth = Depth;
  setEventNotify (AGESA_WARNING,
                 HT_EVENT_OPT_REQUIRED_CAP_GEN3,
                 (UINT8 *)&Evt, State);
}

/*----------------------------------------------------------------------------------------*/
/**
 * For event HT_EVENT_OPT_UNUSED_LINKS
 *
 * @param[in]  NodeA    One of the nodes connected
 * @param[in]  NodeB    The other connected node
 * @param[in]  LinkA    its unusable link
 * @param[in]  LinkB    its unusable link
 * @param[in]  State    our State
 *
 */
VOID
NotifyWarningOptUnusedLinks (
  IN       UINT32    NodeA,
  IN       UINT32    LinkA,
  IN       UINT32    NodeB,
  IN       UINT32    LinkB,
  IN       STATE_DATA *State
  )
{
  HT_EVENT_DATA_OPT_UNUSED_LINKS Evt;
  // Zero out the event data
  LibAmdMemFill (
    &Evt,
    0,
    (sizeof(UINT32) * NUMBER_OF_EVENT_DATA_PARAMS),
    State->ConfigHandle
    );

  Evt.NodeA = NodeA;
  Evt.LinkA = LinkA;
  Evt.NodeB = NodeB;
  Evt.LinkB = LinkB;
  setEventNotify (AGESA_WARNING,
                 HT_EVENT_OPT_UNUSED_LINKS,
                 (UINT8 *)&Evt, State);
}


/*----------------------------------------------------------------------------------------*/
/**
 * For event HT_EVENT_OPT_LINK_PAIR_EXCEED
 *
 * @param[in]  NodeA      One of the nodes connected
 * @param[in]  NodeB      The other connected node
 * @param[in]  MasterLink its unusable Masterlink
 * @param[in]  AltLink    its unusable Alternate link
 * @param[in]  State      our State
 *
 */
VOID
NotifyWarningOptLinkPairExceed (
  IN       UINT32    NodeA,
  IN       UINT32    NodeB,
  IN       UINT32    MasterLink,
  IN       UINT32    AltLink,
  IN       STATE_DATA *State
  )
{
  HT_EVENT_DATA_OPT_LINK_PAIR_EXCEED Evt;
  // Zero out the event data
  LibAmdMemFill (
    &Evt,
    0,
    (sizeof(UINT32) * NUMBER_OF_EVENT_DATA_PARAMS),
    State->ConfigHandle
    );

  Evt.NodeA = NodeA;
  Evt.MasterLink = MasterLink;
  Evt.NodeB = NodeB;
  Evt.AltLink = AltLink;
  setEventNotify (AGESA_WARNING,
                 HT_EVENT_OPT_LINK_PAIR_EXCEED,
                 (UINT8 *)&Evt, State);
}


/*----------------------------------------------------------------------------------------*/
/**
 * For event HT_EVENT_COH_NO_TOPOLOGY
 *
 * @param[in] Nodes     The total number of nodes found so far
 * @param[in] State     our State
 *
 */
VOID
NotifyErrorCohNoTopology (
  IN       UINT8 Nodes,
  IN       STATE_DATA *State
  )
{
  HT_EVENT_DATA_COH_NO_TOPOLOGY Evt;
  // Zero out the event data
  LibAmdMemFill (
    &Evt,
    0,
    (sizeof(UINT32) * NUMBER_OF_EVENT_DATA_PARAMS),
    State->ConfigHandle
    );

  IDS_HDT_CONSOLE (HT_TRACE, "No Topology Matched system with %d nodes found.\n", Nodes);
  Evt.TotalNodes = Nodes;
  setEventNotify (AGESA_ERROR,
                 HT_EVENT_COH_NO_TOPOLOGY,
                 (UINT8 *)&Evt, State);
}

/*----------------------------------------------------------------------------------------*/
/**
 * For event HT_EVENT_COH_PROCESSOR_TYPE_MIX
 *
 * @param[in] Node      The node from which a new node was discovered
 * @param[in] Link      The link from that node
 * @param[in] Nodes     The total number of nodes found so far
 * @param[in] State     our State
 *
 */
VOID
NotifyFatalCohProcessorTypeMix (
  IN       UINT8 Node,
  IN       UINT8 Link,
  IN       UINT8 Nodes,
  IN       STATE_DATA *State
  )
{
  HT_EVENT_DATA_COH_PROCESSOR_TYPE_MIX Evt;
  // Zero out the event data
  LibAmdMemFill (
    &Evt,
    0,
    (sizeof(UINT32) * NUMBER_OF_EVENT_DATA_PARAMS),
    State->ConfigHandle
    );

  IDS_HDT_CONSOLE (HT_TRACE, "Illegal Processor Type Mix.\n");
  Evt.Node = Node;
  Evt.Link = Link;
  Evt.TotalNodes = Nodes;
  setEventNotify (AGESA_CRITICAL,
                 HT_EVENT_COH_PROCESSOR_TYPE_MIX,
                 (UINT8 *)&Evt, State);
}

/*----------------------------------------------------------------------------------------*/
/**
 * For event HT_EVENT_COH_NODE_DISCOVERED
 *
 * @param[in] Node       Node from which a new node was discovered
 * @param[in] Link       The link to that new node
 * @param[in] NewNode    The new node's id
 * @param[in] TempRoute  Temporarily, during discovery, the new node is accessed at this id.
 * @param[in] State      our State
 *
 */
VOID
NotifyInfoCohNodeDiscovered (
  IN       UINT8 Node,
  IN       UINT8 Link,
  IN       UINT8 NewNode,
  IN       UINT8 TempRoute,
  IN       STATE_DATA *State
  )
{
  HT_EVENT_DATA_COH_NODE_DISCOVERED Evt;
  // Zero out the event data
  LibAmdMemFill (
    &Evt,
    0,
    (sizeof(UINT32) * NUMBER_OF_EVENT_DATA_PARAMS),
    State->ConfigHandle
    );

  IDS_HDT_CONSOLE (HT_TRACE, "Adding Node %d.\n", NewNode);
  Evt.Node = Node;
  Evt.Link = Link;
  Evt.NewNode = NewNode;
  Evt.TempRoute = TempRoute;
  setEventNotify (AGESA_SUCCESS,
                 HT_EVENT_COH_NODE_DISCOVERED,
                 (UINT8 *)&Evt, State);
}

/*----------------------------------------------------------------------------------------*/
/**
 * For event HT_EVENT_COH_MPCAP_MISMATCH
 *
 * @param[in] Node      The node from which a new node was discovered
 * @param[in] Link      The link from that node
 * @param[in] Cap       The aggregate system MP Capability
 * @param[in] Nodes     The total number of nodes found so far
 * @param[in] State     our State
 *
 */
VOID
NotifyFatalCohMpCapMismatch (
  IN       UINT8      Node,
  IN       UINT8      Link,
  IN       UINT8      Cap,
  IN       UINT8      Nodes,
  IN       STATE_DATA *State
  )
{
  HT_EVENT_DATA_COH_MP_CAP_MISMATCH Evt;
  // Zero out the event data
  LibAmdMemFill (
    &Evt,
    0,
    (sizeof(UINT32) * NUMBER_OF_EVENT_DATA_PARAMS),
    State->ConfigHandle
    );

  IDS_HDT_CONSOLE (HT_TRACE, "Mp Capability Mismatch.\n");
  Evt.Node = Node;
  Evt.Link = Link;
  Evt.SysMpCap = Cap;
  Evt.TotalNodes = Nodes;
  setEventNotify (AGESA_CRITICAL,
                 HT_EVENT_COH_MPCAP_MISMATCH,
                 (UINT8 *)&Evt, State);
}
