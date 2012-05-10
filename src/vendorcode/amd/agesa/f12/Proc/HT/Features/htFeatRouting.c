/* $NoKeywords:$ */
/**
 * @file
 *
 * Routing Routines
 *
 * Contains routines for isomorphic topology matching,
 * routing determination, and routing initialization.
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
#include "Ids.h"
#include "Topology.h"
#include "htFeat.h"
#include "htInterface.h"
#include "htNotify.h"
#include "htNb.h"
#include "htGraph.h"
#include "htFeatRouting.h"
#include "htTopologies.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_HT_FEATURES_HTFEATROUTING_FILECODE
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
typedef struct {
  UINT8   **CurrentPosition;
  BOOLEAN IsCustomList;
} TOPOLOGY_CONTEXT;

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
 ***          ISOMORPHISM BASED ROUTING TABLE GENERATION CODE            ***
 ***************************************************************************/

/*----------------------------------------------------------------------------------------*/
/**
 * Return the Link on source Node which connects to target Node
 *
 * @param[in] SourceNode   The Node on which to find the Link
 * @param[in] TargetNode   The Link will connect to this Node
 * @param[in] State        Our global state
 *
 * @return    the Link to target
 */
UINT8
STATIC
FindLinkToNode (
  IN       UINT8        SourceNode,
  IN       UINT8        TargetNode,
  IN       STATE_DATA   *State
  )
{
  UINT8 TargetLink;
  UINT8 k;

  // A node linked to itself is not a supported topology graph, this is probably an error in the
  // topology data.  There is not going to be a portlist match for it.
  ASSERT (SourceNode != TargetNode);
  TargetLink = INVALID_LINK;
  for (k = 0; k < State->TotalLinks*2; k += 2) {
    if (((*State->PortList)[k].NodeID == SourceNode) && ((*State->PortList)[k + 1].NodeID == TargetNode)) {
      TargetLink = (*State->PortList)[k].Link;
      break;
    } else if (((*State->PortList)[k + 1].NodeID == SourceNode) && ((*State->PortList)[k].NodeID == TargetNode)) {
      TargetLink = (*State->PortList)[k + 1].Link;
      break;
    }
  }
  ASSERT (TargetLink != INVALID_LINK);

  return TargetLink;
}

/*----------------------------------------------------------------------------------------*/
/**
 *      Is graphA isomorphic to graphB?
 *
 *  If this function returns true, then Perm will contain the permutation
 *  required to transform graphB into graphA.
 *  We also use the degree of each Node, that is the number of connections it has, to
 *  speed up rejection of non-isomorphic graphs (if there is a Node in graphA with n
 *  connections, there must be at least one unmatched in graphB with n connections).
 *
 * @param[in]     Node     the discovered Node which we are trying to match
 *                      with a permutation the topology
 * @param[in,out] State our global state, degree and adjacency matrix,
 *                      output a permutation if successful
 * @retval TRUE         the graphs are isomorphic
 * @retval FALSE        the graphs are not isomorphic
 *
 */
BOOLEAN
STATIC
IsIsomorphic (
  IN       UINT8         Node,
  IN OUT   STATE_DATA    *State
  )
{
  UINT8 j;
  UINT8 k;
  UINT8 Nodecnt;

  // We have only been called if Nodecnt == pSelected->size !
  Nodecnt = State->NodesDiscovered + 1;

  if (Node != Nodecnt) {
    // Keep building the permutation
    for (j = 0; j < Nodecnt; j++) {
      // Make sure the degree matches
      if (State->Fabric->SysDegree[Node] != State->Fabric->DbDegree[j]) {
        continue;
      }

      // Make sure that j hasn't been used yet (ought to use a "used"
      // array instead, might be faster)
      for (k = 0; k < Node; k++) {
        if (State->Fabric->Perm[k] == j) {
          break;
        }
      }
      if (k != Node) {
        continue;
      }
      State->Fabric->Perm[Node] = j;
      if (IsIsomorphic (Node + 1, State)) {
        return TRUE;
      }
    }
    return FALSE;
  } else {
    // Test to see if the permutation is isomorphic
    for (j = 0; j < Nodecnt; j++) {
      for (k = 0; k < Nodecnt; k++) {
        if (State->Fabric->SysMatrix[j][k] != State->Fabric->DbMatrix[State->Fabric->Perm[j]][State->Fabric->Perm[k]] ) {
          return FALSE;
        }
      }
    }
    return TRUE;
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Set Topology List iterator context to the Beginning and provide the first topology.
 *
 * Check the interface for a custom topology list.  If one is found, set context to the
 * first item, and return that item. Otherwise return the first item in the built in list.
 *
 * @param[in,out] TopologyContextHandle  Initialize this context to beginning of lists.
 * @param[out]     NextTopology           The next topology, NULL if end.
 * @param[in]      State                  Access to interface, handles.
 *
 */
VOID
STATIC
BeginTopologies (
     OUT   TOPOLOGY_CONTEXT  *TopologyContextHandle,
     OUT   UINT8             **NextTopology,
  IN       STATE_DATA        *State
  )
{
  if (State->HtBlock->Topolist != NULL) {
    // Start with a custom list
    TopologyContextHandle->CurrentPosition = State->HtBlock->Topolist;
    TopologyContextHandle->IsCustomList = TRUE;
  } else {
    // Start with the built in list
    GetAmdTopolist (&TopologyContextHandle->CurrentPosition);
    TopologyContextHandle->IsCustomList = FALSE;
  }
  *NextTopology = *TopologyContextHandle->CurrentPosition;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Iterate through available topologies.
 *
 * Increment to the next list item.  If we are doing a custom list, when we reach the end
 * switch to the built in list.
 *
 * @param[in,out]  TopologyContextHandle  Maintain iterator's context from one call to the next
 * @param[out]     NextTopology           The next topology, NULL if end.
 *
 */
VOID
STATIC
GetNextTopology (
  IN OUT   TOPOLOGY_CONTEXT  *TopologyContextHandle,
     OUT   UINT8             **NextTopology
  )
{
  // Not valid to continue calling this routine after reaching the end.
  ASSERT (TopologyContextHandle->CurrentPosition != NULL);

  if (TopologyContextHandle->IsCustomList) {
    // We are iterating the custom list from the interface.
    TopologyContextHandle->CurrentPosition++;
    if (*TopologyContextHandle->CurrentPosition == NULL) {
      // We are at the end of the custom list, switch to the built in list.
      TopologyContextHandle->IsCustomList = FALSE;
      GetAmdTopolist (&TopologyContextHandle->CurrentPosition);
    }
  } else {
    // We are iterating the built in list
    TopologyContextHandle->CurrentPosition++;
    // If we are at the end of the built in list, NextTopology == NULL is the AtEnd.
  }
  *NextTopology = *TopologyContextHandle->CurrentPosition;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Using the description of the fabric topology we discovered, try to find a match
 * among the supported topologies.
 *
 * @HtFeatMethod{::F_LOOKUP_COMPUTE_AND_LOAD_ROUTING_TABLES}
 *
 * A supported topology description matches the discovered fabric if the Nodes can be
 * matched in such a way that all the Nodes connected in one set are exactly the
 * Nodes connected in the other (formally, that the graphs are isomorphic).  Which
 * Links are used is not really important to matching.  If the graphs match, then
 * there is a permutation of one that translates the Node positions and Linkages to
 * the other.
 *
 * In order to make the isomorphism test efficient, we test for matched number of Nodes
 * (a 4 Node fabric is not isomorphic to a 2 Node topology), and provide degrees of Nodes
 * to the isomorphism test.
 *
 * The generic routing table solution for any topology is predetermined and represented
 * as part of the topology.  The permutation we computed tells us how to interpret the
 * routing onto the fabric we discovered.  We do this working backward from the last
 * Node discovered to the BSP, writing the routing tables as we go.
 *
 * @param[in,out]    State    the discovered fabric, degree matrix, permutation
 *
 */
VOID
LookupComputeAndLoadRoutingTables (
  IN OUT   STATE_DATA    *State
  )
{
  TOPOLOGY_CONTEXT   TopologyContextHandle;
  UINT8 *Selected;
  UINT8 Size;
  UINT8 PairCounter;
  UINT8 ReqTargetLink;
  UINT8 RspTargetLink;
  UINT8 ReqTargetNode;
  UINT8 RspTargetNode;
  UINT8 AbstractBcTargetNodes;
  UINT32 BcTargetLinks;
  UINT8 NodeCounter;
  UINT8 NodeBeingRouted;
  UINT8 NodeRoutedTo;
  UINT8 BroadcastSourceNode;

  Size = State->NodesDiscovered + 1;
  BeginTopologies (&TopologyContextHandle, &Selected, State);
  while (Selected != NULL) {
    if (GraphHowManyNodes (Selected) == Size) {
      // Build Degree vector and Adjacency Matrix for this entry
      for (NodeCounter = 0; NodeCounter < Size; NodeCounter++) {
        State->Fabric->DbDegree[NodeCounter] = 0;
        for (PairCounter = 0; PairCounter < Size; PairCounter++) {
          if (GraphIsAdjacent (Selected, NodeCounter, PairCounter)) {
            State->Fabric->DbMatrix[NodeCounter][PairCounter] = TRUE;
            State->Fabric->DbDegree[NodeCounter]++;
          } else {
            State->Fabric->DbMatrix[NodeCounter][PairCounter] = FALSE;
          }
        }
      }

      if (IsIsomorphic (0, State)) {
        break;  // A matching topology was found
      }
    }
    GetNextTopology (&TopologyContextHandle, &Selected);
  }

  if (Selected != NULL) {
    // Compute the reverse Permutation
    for (NodeCounter = 0; NodeCounter < Size; NodeCounter++) {
      State->Fabric->ReversePerm[State->Fabric->Perm[NodeCounter]] = NodeCounter;
    }

    // Start with the last discovered Node, and move towards the BSP
    for (NodeCounter = 0; NodeCounter < Size; NodeCounter++) {
      NodeBeingRouted = ((Size - 1) - NodeCounter);
      for (NodeRoutedTo = 0; NodeRoutedTo < Size; NodeRoutedTo++) {
        BcTargetLinks = 0;
        AbstractBcTargetNodes = GraphGetBc (Selected, State->Fabric->Perm[NodeBeingRouted], State->Fabric->Perm[NodeRoutedTo]);

        for (BroadcastSourceNode = 0; BroadcastSourceNode < MAX_NODES; BroadcastSourceNode++) {
          if ((AbstractBcTargetNodes & ((UINT32)1 << BroadcastSourceNode)) != 0) {
            // Accepting broadcast from yourself is handled in Nb, so in the topology graph it is an error.
            ASSERT (NodeBeingRouted != State->Fabric->ReversePerm[BroadcastSourceNode]);
            BcTargetLinks |= (UINT32)1 << FindLinkToNode (NodeBeingRouted, State->Fabric->ReversePerm[BroadcastSourceNode], State);
          }
        }

        if (NodeBeingRouted == NodeRoutedTo) {
          ReqTargetLink = ROUTE_TO_SELF;
          RspTargetLink = ROUTE_TO_SELF;
        } else {
          ReqTargetNode = GraphGetReq (Selected, State->Fabric->Perm[NodeBeingRouted], State->Fabric->Perm[NodeRoutedTo]);
          ReqTargetLink = FindLinkToNode (NodeBeingRouted, State->Fabric->ReversePerm[ReqTargetNode], State);

          RspTargetNode = GraphGetRsp (Selected, State->Fabric->Perm[NodeBeingRouted], State->Fabric->Perm[NodeRoutedTo]);
          RspTargetLink = FindLinkToNode (NodeBeingRouted, State->Fabric->ReversePerm[RspTargetNode], State);
        }
        State->Nb->WriteFullRoutingTable (NodeBeingRouted, NodeRoutedTo, ReqTargetLink, RspTargetLink, BcTargetLinks, State->Nb);
      }
      //  Clean up discovery 'footprint' that otherwise remains in the routing table.  It didn't hurt
      // anything, but might cause confusion during debug and validation.  Do this by setting the
      // route back to all self routes. Since it's the Node that would be one more than actually installed,
      // this only applies if less than MaxNodes were found.
      //
      if (Size < MAX_NODES) {
        State->Nb->WriteFullRoutingTable (NodeBeingRouted, Size, ROUTE_TO_SELF, ROUTE_TO_SELF, 0, State->Nb);
      }
    }
  } else {
    //
    // No Matching Topology was found
    // Error Strategy:
    // Auto recovery doesn't seem likely, Force boot as 1P.
    // For reporting, logging, provide number of Nodes
    // If not implemented or returns, boot as BSP uniprocessor.
    //
    // This can be caused by not supplying an additional topology list, if your board is not one of the built-in topologies.
    //
    NotifyErrorCohNoTopology (State->NodesDiscovered, State);
    IDS_ERROR_TRAP;
    // Force 1P
    State->NodesDiscovered = 0;
    State->TotalLinks = 0;
    State->Nb->EnableRoutingTables (0, State->Nb);
    State->HtInterface->CleanMapsAfterError (State);
  }
  // Save the topology pointer, or NULL, for other features
  State->Fabric->MatchedTopology = Selected;
  IDS_HDT_CONSOLE (
    HT_TRACE,
    "System routed as %s.\n",
    ((TopologyContextHandle.IsCustomList) ?
     "custom topology" :
     (((Selected == amdHtTopologySingleNode) || (Selected == NULL)) ?
      "single node" :
      ((Selected == amdHtTopologyDualNode) ?
       "dual node" :
       ((Selected == amdHtTopologyFourSquare) ?
        "four node box" :
        ((Selected == amdHtTopologyFourKite) ?
         "four node kite" :
         ((Selected == amdHtTopologyFourFully) ?
          "fully connected four-way" :
          ((Selected == amdHtTopologyEightDoubloon) ?
           "MCM max performance" :
           ((Selected == amdHtTopologyEightTwinFullyFourWays) ?
            "MCM max I/O" :
            "AMD builtin topology"))))))))
    );
}

/*----------------------------------------------------------------------------------------*/
/**
 * Make a Hop Count Table for the installed topology.
 *
 * @HtFeatMethod{::F_MAKE_HOP_COUNT_TABLE}
 *
 * For SLIT, create a node x node matrix with the number of hops.  We can do this
 * using the topology and the permutation, counting the nodes visited in the routes between
 * nodes.
 *
 * @param[in,out]   State    access topology, permutation, update hop table
 *
 */
VOID
MakeHopCountTable (
  IN OUT   STATE_DATA    *State
  )
{
  UINT8 Origin;
  UINT8 Target;
  UINT8 Current;
  UINT8 Hops;
  UINT8 Size;

  ASSERT (State->Fabric != NULL);
  if (State->HopCountTable != NULL) {
    if (State->Fabric->MatchedTopology != NULL) {
      Size = GraphHowManyNodes (State->Fabric->MatchedTopology);
      State->HopCountTable->Size = Size;
      //
      // For each node, targeting each node, follow the request path through the database graph,
      // counting the number of edges.
      //
      for (Origin = 0; Origin < Size; Origin++) {
        for (Target = 0; Target < Size; Target++) {
          // If both nodes are the same the answer will be zero
          Hops = 0;
          // Current starts as the database node corresponding to system node Origin.
          Current = State->Fabric->Perm[Origin];
          // Stop if Current is the database node corresponding to system node Target
          while (Current != State->Fabric->Perm[Target]) {
            // This is a hop, so count it. Move Current to the next intermediate database node.
            Hops++;
            Current = GraphGetReq (State->Fabric->MatchedTopology, Current, State->Fabric->Perm[Target]);
          }
          // Put the hop count in the table.
          State->HopCountTable->Hops[ ((Origin * Size) + Target)] = Hops;
        }
      }
    }
  }
}
