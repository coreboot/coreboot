/* $NoKeywords:$ */
/**
 * @file
 *
 * Coherent Discovery Routines.
 *
 * Contains routines for discovery, along with Temporary routing.
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
#include "Ids.h"
#include "Topology.h"
#include "htFeat.h"
#include "htInterface.h"
#include "htNotify.h"
#include "htNb.h"
#include "htFeatDynamicDiscovery.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_HT_FEATURES_HTFEATDYNAMICDISCOVERY_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define LOGICAL_PROCESSOR_NONE 0xFF

/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */
/**
 * Status result from exploring for a new node on a link.
 */
typedef enum {
  ExploreNodeStatusNew,    ///< A new node was discovered
  ExploreNodeStatusGood,   ///< A new link to an already known node was discovered
  ExploreNodeStatusStop,   ///< Discovery must halt now.
  ExploreNodeStatusIgnore, ///< A new node was ignored on purpose.
  ExploreNodeStatusMax     ///< Use for bounds check and limit only
} EXPLORE_NODE_STATUS;

/**
 * Save all the information needed about a node at its discovery.
 *
 * When we can access the node at a known temporary route, read everything needed
 * to do node to socket mapping, post to ap mailbox at later times.
 */
typedef struct {
  UINT8       LogicalProcessor;     ///< Independent of Node,Socket group nodes into logical
                                    ///< processors based on discovery.
  UINT8       CurrentNode;          ///< The node from which discovery occurred.
  UINT8       CurrentLink;          ///< The link on that node which we explored.
  UINT8       PackageLink;          ///< The package level link corresponding to CurrentLink.
  UINT8       CurrentModuleType;    ///< The current node's module type, Single or Multiple.
  UINT8       CurrentModule;        ///< This current node's module id.
  UINT8       HardwareSocket;       ///< Save the hardware socket strap (for hardware socket method).
  UINT8       NewModuleType;        ///< The new node's module type, Single or Multiple.
  UINT8       NewModule;            ///< The new node's module id.
} NEW_NODE_SAVED_INFO_ITEM;

/**
 * A "no info" initializer for saved new node info.
 */
STATIC CONST NEW_NODE_SAVED_INFO_ITEM ROMDATA NoInfoSavedYet =
{
  LOGICAL_PROCESSOR_NONE, 0, 0, 0, 0, 0, 0, 0
};

/**
 * A list of all the new node info, indexed by each new node's nodeid.
 */
typedef NEW_NODE_SAVED_INFO_ITEM (*NEW_NODE_SAVED_INFO_LIST) [MAX_NODES];

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

/***************************************************************************
 ***              GENERIC HYPERTRANSPORT DISCOVERY CODE                  ***
 ***************************************************************************/

/*-----------------------------------------------------------------------------------*/
/**
 * Ensure a request / response route from target Node to bsp.
 *
 * Since target Node is always a predecessor of actual target Node, each Node gets a
 * route to actual target on the Link that goes to target.  The routing produced by
 * this routine is adequate for config access during discovery, but NOT for coherency.
 *
 * @param[in] TargetNode     the path to actual target goes through target
 * @param[in] ActualTarget   the ultimate target being routed to
 * @param[in] State           our global state, port config info
 *
 */
VOID
STATIC
routeFromBSP (
  IN       UINT8        TargetNode,
  IN       UINT8        ActualTarget,
  IN       STATE_DATA   *State
  )
{
  UINT8 PredecessorNode;
  UINT8 PredecessorLink;
  UINT8 CurrentPair;

  if (TargetNode == 0) {
    return;  //  BSP has no predecessor, stop
  }

  // Search for the Link that connects TargetNode to its predecessor
  CurrentPair = 0;
  while ((*State->PortList)[CurrentPair*2 + 1].NodeID != TargetNode) {
    CurrentPair++;
    ASSERT (CurrentPair < State->TotalLinks);
  }

  PredecessorNode = (*State->PortList)[ (CurrentPair * 2)].NodeID;
  PredecessorLink = (*State->PortList)[ (CurrentPair * 2)].Link;

  // Recursively call self to ensure the route from the BSP to the Predecessor
  // Node is established
  routeFromBSP (PredecessorNode, ActualTarget, State);

  State->Nb->WriteRoutingTable (PredecessorNode, ActualTarget, PredecessorLink, State->Nb);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Test Compatibility of a new node, and handle failure.
 *
 * Make the compatibility test call for the northbridge.
 * If the new node is incompatible, force 1P.  Notify the event.
 * Additionally, invoke the northbridge stop link method, to
 * implement isolation of the BSP from any incompatible node.
 *
 * @param[in]  CurrentNode The node we are exploring from
 * @param[in]  CurrentLink The Link on that node to explore.
 * @param[in]  State       Access to Northbridge interface.
 *
 * @retval TRUE            Check is Ok
 * @retval FALSE           Check failed and is handled
 */
BOOLEAN
STATIC
CheckCompatible (
  IN       UINT8       CurrentNode,
  IN       UINT8       CurrentLink,
  IN       STATE_DATA  *State
  )
{
  UINT8 NodeToKill;
  BOOLEAN Result;

  Result = TRUE;

  // Check the northbridge of the Node we just found, to make sure it is compatible
  // before doing anything else to it.
  //
  if (State->Nb->IsIllegalTypeMix ((CurrentNode + 1), State->Nb)) {
    IDS_ERROR_TRAP;

    // Notify BIOS of event
    NotifyFatalCohProcessorTypeMix (
      CurrentNode,
      CurrentLink,
      State->NodesDiscovered,
      State
      );

    // If Node is not compatible, force boot to 1P
    // If they are not compatible stop cHT init and:
    //     1. Disable all cHT Links on the BSP
    //     2. Configure the BSP routing tables as a UP.
    //     3. Notify main BIOS.
    //
    State->NodesDiscovered = 0;
    State->TotalLinks = 0;
    // Abandon our coherent Link data structure.  At this point there may
    // be coherent Links on the BSP that are not yet in the portList, and
    // we have to turn them off anyway.  So depend on the hardware to tell us.
    //
    for (CurrentLink = 0; CurrentLink < State->Nb->MaxLinks; CurrentLink++) {
      // Stop all Links which are connected, coherent, and ready
      if (State->Nb->VerifyLinkIsCoherent (0, CurrentLink, State->Nb)) {
        State->Nb->StopLink (0, CurrentLink, State, State->Nb);
      }
    }

    for (NodeToKill = 0; NodeToKill < MAX_NODES; NodeToKill++) {
      State->Nb->WriteFullRoutingTable (0, NodeToKill, ROUTE_TO_SELF, ROUTE_TO_SELF, 0, State->Nb);
    }

    State->HtInterface->CleanMapsAfterError (State);

    // End Coherent Discovery
    Result = FALSE;
  }
  return Result;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Check the system MP capability with a new node and handle any failure.
 *
 * Invoke the northbridge MP capability check.  If it fails, notify the event and force
 * 1P.  Should not need to stop links on the BSP.
 *
 * @param[in]  CurrentNode The node we are exploring from
 * @param[in]  CurrentLink The Link on that node to explore.
 * @param[in]  State       Access to Northbridge interface.
 *
 * @retval TRUE            Check is Ok
 * @retval FALSE           Check Failed and is handled
 */
BOOLEAN
STATIC
CheckCapable (
  IN       UINT8       CurrentNode,
  IN       UINT8       CurrentLink,
  IN       STATE_DATA  *State
  )
{
  UINT8 NodeToKill;
  BOOLEAN Result;

  Result = TRUE;

  // Check the capability of northbridges against the currently known configuration
  if (State->Nb->IsExceededCapable ((CurrentNode + 1), State, State->Nb)) {
    IDS_ERROR_TRAP;
    // Notify BIOS of event
    NotifyFatalCohMpCapMismatch (
      CurrentNode,
      CurrentLink,
      State->SysMpCap,
      State->NodesDiscovered,
      State
      );

    State->NodesDiscovered = 0;
    State->TotalLinks = 0;

    for (NodeToKill = 0; NodeToKill < MAX_NODES; NodeToKill++) {
      State->Nb->WriteFullRoutingTable (0, NodeToKill, ROUTE_TO_SELF, ROUTE_TO_SELF, 0, State->Nb);
    }

    State->HtInterface->CleanMapsAfterError (State);

    // End Coherent Discovery
    Result = FALSE;
  }
  return Result;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Make all the tests needed to determine if a link should be added to the system data structure.
 *
 * The link should be added to the system data structure if it is:
 * - not being Ignored on this boot
 * - not having a hard failure
 * - coherent and connected
 * - not already in the system data structure
 * - not subject to some special handling case.
 * .
 *
 * @param[in]  CurrentNode The node we are exploring from
 * @param[in]  CurrentLink The Link on that node to explore.
 * @param[in]  State       Access to Northbridge interface.
 *
 * @retval FALSE          This link should not be added.
 * @retval TRUE           This link should explored and added to the system.
 */
BOOLEAN
STATIC
IsLinkToAdd (
  IN       UINT8       CurrentNode,
  IN       UINT8       CurrentLink,
  IN       STATE_DATA  *State
  )
{
  BOOLEAN Linkfound;
  UINTN Port;
  FINAL_LINK_STATE FinalLinkState;
  BOOLEAN Result;

  Result = FALSE;

  FinalLinkState = State->HtInterface->GetIgnoreLink (CurrentNode, CurrentLink, State->Nb->DefaultIgnoreLinkList, State);
  if ((FinalLinkState != MATCHED) && (FinalLinkState != POWERED_OFF)) {
    if (!State->Nb->ReadTrueLinkFailStatus (CurrentNode, CurrentLink, State, State->Nb)) {
      // Make sure that the Link is connected, coherent, and ready
      if (State->Nb->VerifyLinkIsCoherent (CurrentNode, CurrentLink, State->Nb)) {
        // Test to see if the CurrentLink has already been explored
        Linkfound = FALSE;
        for (Port = 0; Port < State->TotalLinks; Port++) {
          if ((((*State->PortList)[ (Port * 2 + 1)].NodeID == CurrentNode) &&
               ((*State->PortList)[ (Port * 2 + 1)].Link == CurrentLink)) ||
              (((*State->PortList)[ (Port * 2)].NodeID == CurrentNode) &&
               ((*State->PortList)[ (Port * 2)].Link == CurrentLink))) {
            Linkfound = TRUE;
            break;
          }
        }
        if (!Linkfound) {
          if (!State->Nb->HandleSpecialLinkCase (CurrentNode, CurrentLink, State, State->Nb)) {
            Result = TRUE;
          }
        }
      }
    }
  } else {
    if (FinalLinkState == POWERED_OFF) {
      State->Nb->StopLink (CurrentNode, CurrentLink, State, State->Nb);
    }
  }
  return Result;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Explore for a new node over a link, handling whatever is found.
 *
 * Open a temporary route over a link on the current node.
 * Make checks for compatibility and capability in the proper sequence.
 * If the node found is new, set a token to it, so it will be recognized in the
 * future, and notify an event for finding a new node.
 * If the node is already found (token is set), just return status.
 *
 * @param[in]     CurrentNode         The node we are exploring from
 * @param[in]     CurrentLink         The Link on that node to explore.
 * @param[in]     LogicalProcessor    The processor to update in the maps.
 * @param[in,out] NewNodeSavedInfo    The saved info for nodes in that processor.
 * @param[in]     State               Access to Northbridge interface.
 *
 * @retval ExploreNodeStatusNew    A new node was found
 * @retval ExploreNodeStatusGood   This is a good link to an already known node
 * @retval ExploreNodeStatusStop   Stop Coherent Discovery
 */
EXPLORE_NODE_STATUS
STATIC
ExploreNode (
  IN       UINT8       CurrentNode,
  IN       UINT8       CurrentLink,
  IN       UINT8       LogicalProcessor,
  IN OUT   NEW_NODE_SAVED_INFO_LIST NewNodeSavedInfo,
  IN       STATE_DATA  *State
  )
{
  UINT8 Token;
  EXPLORE_NODE_STATUS Status;

  // Modify CurrentNode's routing table to use CurrentLink to send
  // traffic to CurrentNode + 1
  //
  State->Nb->WriteRoutingTable (CurrentNode, (CurrentNode + 1), CurrentLink, State->Nb);
  if (!State->Nb->HandleSpecialNodeCase ((CurrentNode + 1), CurrentLink, State, State->Nb)) {
    if (CheckCompatible (CurrentNode, CurrentLink, State)) {
      // Read Token from Current + 1
      Token = State->Nb->ReadToken ((CurrentNode + 1), State->Nb);
      ASSERT (Token <= State->NodesDiscovered);
      if (Token == 0) {
        State->NodesDiscovered++;
        ASSERT (State->NodesDiscovered < MAX_NODES);
        if (CheckCapable (CurrentNode, CurrentLink, State)) {
          Token = State->NodesDiscovered;
          State->Nb->WriteToken ((CurrentNode + 1), Token, State->Nb);
          // Fill in Saved New Node info for the discovered node.
          // We do this so we don't have to keep a temporary route open to it.
          // So we save everything that might be needed to set the socket and node
          // maps for either the software or hardware method.
          //
          (*NewNodeSavedInfo)[Token].LogicalProcessor = LogicalProcessor;
          (*NewNodeSavedInfo)[Token].CurrentNode = CurrentNode;
          (*NewNodeSavedInfo)[Token].CurrentLink = CurrentLink;
          (*NewNodeSavedInfo)[Token].PackageLink = State->Nb->GetPackageLink (CurrentNode, CurrentLink, State->Nb);
          (*NewNodeSavedInfo)[Token].HardwareSocket = State->Nb->GetSocket (Token, (CurrentNode + 1), State->Nb);
          State->Nb->GetModuleInfo (
            CurrentNode,
            &((*NewNodeSavedInfo)[Token].CurrentModuleType),
            &((*NewNodeSavedInfo)[Token].CurrentModule),
            State->Nb
            );
          State->Nb->GetModuleInfo (
            (CurrentNode + 1),
            &((*NewNodeSavedInfo)[Token].NewModuleType),
            &((*NewNodeSavedInfo)[Token].NewModule),
            State->Nb
            );

          // Notify BIOS with info
          NotifyInfoCohNodeDiscovered (
            CurrentNode,
            CurrentLink,
            Token,
            (CurrentNode + 1),
            State
            );
          Status = ExploreNodeStatusNew;
        } else {
          // Failed Capable
          Status = ExploreNodeStatusStop;
        }
      } else {
        // Not a new node, token already set
        Status = ExploreNodeStatusGood;
      }
    } else {
      // Failed Compatible
      Status = ExploreNodeStatusStop;
    }
  } else {
    // Ignore this node
    Status = ExploreNodeStatusIgnore;
  }

  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Process all the saved new node info for the current processor.
 *
 * When all nodes in the processor have been discovered, we can process all the saved
 * info about the nodes.  We add each node to the socket and node maps.
 *
 * @param[in]    LogicalProcessor    The processor to update in the maps.
 * @param[in]    NewNodeSavedInfo    The saved info for nodes in that processor.
 * @param[in]    State               Our system representation.
 */
VOID
STATIC
ProcessSavedNodeInfo (
  IN       UINT8                    LogicalProcessor,
  IN       NEW_NODE_SAVED_INFO_LIST NewNodeSavedInfo,
  IN       STATE_DATA  *State
  )
{
  UINT8 NewNode;
  UINT8 HardwareSocket;

  // Can't have more processors than nodes, just more (or equal) nodes than processors.
  ASSERT (LogicalProcessor <= (State->NodesDiscovered));
  HardwareSocket = 0xFF;
  // Find the Hardware Socket value to use (if we are using the hardware socket naming method).
  // The new nodes are the ones in this processor, so find the one that is module 0.
  for (NewNode = 0; NewNode < (State->NodesDiscovered + 1); NewNode++) {
    if (((*NewNodeSavedInfo)[NewNode].LogicalProcessor == LogicalProcessor) &&
        ((*NewNodeSavedInfo)[NewNode].NewModule == 0)) {
      HardwareSocket = (*NewNodeSavedInfo)[NewNode].HardwareSocket;
      break;
    }
  }
  // We must have found a result, however, the hardware socket value doesn't have to be correct
  // unless we are using the hardware socket naming method.  Northbridge code should return the
  // node number for the hardware socket if hardware socket strapping is not supported (i.e. no sbi).
  ASSERT (HardwareSocket != 0xFF);

  // Set the node to socket maps for this processor.  Node zero is always handled specially,
  // so skip it in this loop.
  for (NewNode = 1; NewNode < (State->NodesDiscovered + 1); NewNode++) {
    if ((*NewNodeSavedInfo)[NewNode].LogicalProcessor == LogicalProcessor) {
      // For the currently discovered logical processor, update node to socket
      // map for all the processor's nodes.
      State->HtInterface->SetNodeToSocketMap (
        (*NewNodeSavedInfo)[NewNode].CurrentNode,
        (*NewNodeSavedInfo)[NewNode].CurrentModule,
        (*NewNodeSavedInfo)[NewNode].PackageLink,
        NewNode,
        HardwareSocket,
        (*NewNodeSavedInfo)[NewNode].NewModule,
        State);
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Create and add a new link to the system data structure.
 *
 * Add the two port list data structures, source first, initializing
 * the two node ids and the link values.  The node id of the remote
 * node is its token value.  Also, update the adjacency matrix and
 * node degree table.
 *
 * @param[in]  CurrentNode The node we are exploring from
 * @param[in]  CurrentLink The Link on that node to explore.
 * @param[in]  TempRoute   The temporary node route that goes over that link.
 * @param[in]  State       Access to Northbridge interface.
 *
 */
VOID
STATIC
AddLinkToSystem (
  IN       UINT8       CurrentNode,
  IN       UINT8       CurrentLink,
  IN       UINT8       TempRoute,
  IN       STATE_DATA  *State
  )
{
  UINT8 Token;

  ASSERT (State->TotalLinks < MAX_PLATFORM_LINKS);

  Token = State->Nb->ReadToken (TempRoute, State->Nb);

  (*State->PortList)[State->TotalLinks * 2].Type = PORTLIST_TYPE_CPU;
  (*State->PortList)[State->TotalLinks * 2].Link = CurrentLink;
  (*State->PortList)[State->TotalLinks * 2].NodeID = CurrentNode;

  (*State->PortList)[State->TotalLinks * 2 + 1].Type = PORTLIST_TYPE_CPU;
  (*State->PortList)[State->TotalLinks * 2 + 1].Link = State->Nb->ReadDefaultLink (TempRoute, State->Nb);
  (*State->PortList)[State->TotalLinks * 2 + 1].NodeID = Token;

  State->TotalLinks++;

  if ( !State->Fabric->SysMatrix[CurrentNode][Token] ) {
    State->Fabric->SysDegree[CurrentNode]++;
    State->Fabric->SysDegree[Token]++;
    State->Fabric->SysMatrix[CurrentNode][Token] = TRUE;
    State->Fabric->SysMatrix[Token][CurrentNode] = TRUE;
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Start discovery from a new node.
 *
 * If the node is not the BSP, establish a route between the node and the
 * BSP for request/response.
 * Set the node id, and enable routing on this node.  This gives us control
 * on that node to isolate links, by specifying each link in turn as the route
 * to a possible new node.
 *
 * @param[in]  CurrentNode The node we are exploring from
 * @param[in]  State       Access to Northbridge interface.
 *
 */
VOID
STATIC
StartFromANewNode (
  IN       UINT8       CurrentNode,
  IN       STATE_DATA  *State
  )
{
  if (CurrentNode != 0) {
    // Set path from BSP to CurrentNode
    routeFromBSP (CurrentNode, CurrentNode, State);

    // Set path from BSP to CurrentNode for CurrentNode + 1 if
    // CurrentNode + 1 != MAX_NODES
    //
    if ((CurrentNode + 1) != MAX_NODES) {
      routeFromBSP (CurrentNode, (CurrentNode + 1), State);
    }

    // Configure CurrentNode to route traffic to the BSP through its
    // default Link
    //
    State->Nb->WriteRoutingTable (CurrentNode, 0, State->Nb->ReadDefaultLink (CurrentNode, State->Nb), State->Nb);
  }

  // Set CurrentNode's NodeID field to CurrentNode
  State->Nb->WriteNodeID (CurrentNode, CurrentNode, State->Nb);

  // Enable routing tables on CurrentNode
  State->Nb->EnableRoutingTables (CurrentNode, State->Nb);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Back up from exploring a one-deep internal node.
 *
 * When a newly discovered node has internal package links to another
 * node in the same processor, discovery moves to that node to do the
 * internal links.  Afterwards, this routine provides recovery from that.
 * The node needs to respond again using deflnk rather than routing, so
 * that connections from other nodes to that one can be identified.
 *
 * @param[in]  CurrentNode The node we are exploring from
 * @param[in]  State       Access to Northbridge interface.
 *
 */
VOID
STATIC
BackUpFromANode (
  IN       UINT8       CurrentNode,
  IN       STATE_DATA  *State
  )
{
  if (CurrentNode != 0) {
    // Disable routing tables on CurrentNode
    State->Nb->DisableRoutingTables (CurrentNode, State->Nb);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Dynamically Discover all coherent devices in the system.
 *
 * @HtFeatMethod{::F_COHERENT_DISCOVERY}
 *
 * Initialize some basics like Node IDs and total Nodes found in the
 * process. As we go we also build a representation of the discovered
 * system which we will use later to program the routing tables.
 * During this step, the routing is via default Link back to BSP and
 * to each new Node on the Link it was discovered on (no coherency is
 * active yet).
 *
 * In the case of multiple nodes per processor, do a one deep exploration of internal links
 * to ensure those node pairs are always numbered n, n + 1.
 *
 * @param[in,out] State     our global state
 *
 */
VOID
CoherentDiscovery (
  IN OUT   STATE_DATA *State
  )
{
  UINT8 CurrentNode;
  UINT8 OneDeepNode;
  UINT8 OneDeepLink;
  UINT8 CurrentLink;
  UINT8 LogicalProcessor;
  EXPLORE_NODE_STATUS ExplorationStatus;
  LINK_ITERATOR_STATUS LinkIteratorStatus;
  NEW_NODE_SAVED_INFO_ITEM NewNodeSavedInfoItems [MAX_NODES];
  NEW_NODE_SAVED_INFO_LIST NewNodeSavedInfo;

  // Initially no info exists for any node, but the BSP is part of logical processor zero.
  for (CurrentNode = 0; CurrentNode < MAX_NODES; CurrentNode++) {
    NewNodeSavedInfoItems [CurrentNode] = NoInfoSavedYet;
  }
  NewNodeSavedInfoItems[0].LogicalProcessor = 0;
  NewNodeSavedInfoItems[0].HardwareSocket = State->Nb->GetSocket (0, 0, State->Nb);
  State->Nb->GetModuleInfo (0, &NewNodeSavedInfoItems[0].NewModuleType, &NewNodeSavedInfoItems[0].NewModule, State->Nb);
  NewNodeSavedInfo = (NEW_NODE_SAVED_INFO_LIST) NewNodeSavedInfoItems;

  CurrentNode = 0;
  CurrentLink = LINK_ITERATOR_BEGIN;
  LogicalProcessor = 0;
  // An initial status, for node zero if you will.
  ExplorationStatus = ExploreNodeStatusGood;

  //
  // Entries are always added in pairs, the even indices are the 'source'
  // side closest to the BSP, the odd indices are the 'destination' side
  //

  while ((CurrentNode <= State->NodesDiscovered) && (ExplorationStatus != ExploreNodeStatusStop)) {
    StartFromANewNode (CurrentNode, State);

    //
    // Explore all internal links
    //
    LinkIteratorStatus = State->Nb->GetNextLink (CurrentNode, &CurrentLink, State->Nb);

    while ((LinkIteratorStatus == LinkIteratorInternal) &&
           (ExplorationStatus != ExploreNodeStatusStop)) {
      if (IsLinkToAdd (CurrentNode, CurrentLink, State)) {
        ExplorationStatus = ExploreNode (CurrentNode, CurrentLink, LogicalProcessor, NewNodeSavedInfo, State);
        if ((ExplorationStatus == ExploreNodeStatusGood) ||
            (ExplorationStatus == ExploreNodeStatusNew)) {
          AddLinkToSystem (CurrentNode, CurrentLink, (CurrentNode + 1), State);
        }
      }
      LinkIteratorStatus = State->Nb->GetNextLink (CurrentNode, &CurrentLink, State->Nb);
    }
    if (CurrentNode == 0) {
      // The BSP processor is completely discovered now.
      ProcessSavedNodeInfo (LogicalProcessor, NewNodeSavedInfo, State);
      LogicalProcessor++;
    }

    //
    // Explore all the external links from this node.
    //

    // Starting this iteration using the link that we last got in the iteration above.
    while ((LinkIteratorStatus == LinkIteratorExternal) &&
           (ExplorationStatus != ExploreNodeStatusStop)) {
      if (IsLinkToAdd (CurrentNode, CurrentLink, State)) {
        ExplorationStatus = ExploreNode (CurrentNode, CurrentLink, LogicalProcessor, NewNodeSavedInfo, State);
        if (ExplorationStatus == ExploreNodeStatusNew) {
          AddLinkToSystem (CurrentNode, CurrentLink, (CurrentNode + 1), State);
          // If this is a new node, we need to explore to its internal mate, if any.
          // This allows us to keep internal node pairs as ids n, n+1
          // We use special link and node variables so we can keep our context.
          OneDeepLink = 0xFF;
          OneDeepNode = State->Nb->ReadToken ((CurrentNode + 1), State->Nb);
          StartFromANewNode (OneDeepNode, State);
          LinkIteratorStatus = State->Nb->GetNextLink (OneDeepNode, &OneDeepLink, State->Nb);
          while ((LinkIteratorStatus == LinkIteratorInternal) &&
                 (ExplorationStatus != ExploreNodeStatusStop)) {
            if (IsLinkToAdd (OneDeepNode, OneDeepLink, State)) {
              ExplorationStatus = ExploreNode (OneDeepNode, OneDeepLink, LogicalProcessor, NewNodeSavedInfo, State);
              if ((ExplorationStatus == ExploreNodeStatusGood) ||
                  (ExplorationStatus == ExploreNodeStatusNew)) {
                AddLinkToSystem (OneDeepNode, OneDeepLink, (OneDeepNode + 1), State);
              }
            }
            LinkIteratorStatus = State->Nb->GetNextLink (OneDeepNode, &OneDeepLink, State->Nb);
          }
          // Since we completed all the node's internal links, we found all the nodes in that processor.
          ProcessSavedNodeInfo (LogicalProcessor, NewNodeSavedInfo, State);
          LogicalProcessor++;
          // Restore node to discoverable state.  Otherwise you can't tell what links it is connected on.
          BackUpFromANode (OneDeepNode, State);
        } else {
          if (ExplorationStatus == ExploreNodeStatusGood) {
            AddLinkToSystem (CurrentNode, CurrentLink, (CurrentNode + 1), State);
          }
        }
      }
      LinkIteratorStatus = State->Nb->GetNextLink (CurrentNode, &CurrentLink, State->Nb);
    }
    CurrentNode++;
  }
}
