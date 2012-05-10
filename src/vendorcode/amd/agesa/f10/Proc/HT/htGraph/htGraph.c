/**
 * @file
 *
 * Routines to deal with topology data.
 *
 * Access the topologies and information about a topology.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  HyperTransport
 * @e \$Revision: 44323 $   @e \$Date: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
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
* 
* ***************************************************************************
*
*/

#include "AGESA.h"
#include "Ids.h"
#include "htGraph.h"
#include "OptionsHt.h"
#include "Filecode.h"
#define FILECODE PROC_HT_HTGRAPH_HTGRAPH_FILECODE

extern OPTION_HT_CONFIGURATION OptionHtConfiguration;

/*----------------------------------------------------------------------------------------*/
/**
 * Returns the AGESA built in topology list
 *
 * @param[out]    List    a pointer to the built in topology list
 */
VOID
GetAmdTopolist (
  OUT   UINT8    ***List
  )
{
  // Cast below to hush CONST warning. The built in list must be CONST to be in ROM statically.
  // The caller of this routine may get a topolist pointer from the interface, however, and
  // that is not CONST, since it could be on the stack.
  //
  *List = (UINT8 **)OptionHtConfiguration.HtOptionBuiltinTopologies;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Returns the number of Nodes in the compressed graph
 *
 * @param[in]     Graph   a compressed graph
 *
 * @return the number of Nodes in the graph
 */
UINT8
GraphHowManyNodes (
  IN       UINT8    *Graph
  )
{
  return Graph[0];
}

/*----------------------------------------------------------------------------------------*/
/**
 * Returns true if NodeA is directly connected to NodeB, false otherwise
 *
 * if NodeA == NodeB also returns false.
 * Relies on rule that directly connected Nodes always route requests directly.
 *
 * @param[in]     Graph   the graph to examine
 * @param[in]     NodeA   the Node number of the first Node
 * @param[in]     NodeB   the Node number of the second Node
 *
 * @retval        TRUE    NodeA connects to NodeB
 * @retval        FALSE   NodeA does not connect to NodeB
 */
BOOLEAN
GraphIsAdjacent (
  IN       UINT8    *Graph,
  IN       UINT8    NodeA,
  IN       UINT8    NodeB
  )
{
  UINT8 size;
  size = Graph[0];
  ASSERT ((NodeA < size) && (NodeB < size));
  return (Graph[1 + (NodeA*size + NodeB)*2 + 1] & 0x0F) == NodeB;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Returns the graph Node used by NodeA to route responses targeted at NodeB.
 *
 *     This will be a Node directly connected to NodeA (possibly NodeB itself),
 *     or "Route to Self" if NodeA and NodeB are the same Node.
 *     Note that all Node numbers are abstract Node numbers of the topology graph,
 *     it is the responsibility of the caller to apply any permutation needed.
 *
 * @param[in]     Graph the graph to examine
 * @param[in]     NodeA the Node number of the first Node
 * @param[in]     NodeB  the Node number of the second Node
 *
 * @return The response route Node
 */
UINT8
GraphGetRsp (
  IN       UINT8    *Graph,
  IN       UINT8    NodeA,
  IN       UINT8    NodeB
  )
{
  UINT8 size;
  size = Graph[0];
  ASSERT ((NodeA < size) && (NodeB < size));
  return (Graph[1 + (NodeA*size + NodeB)*2 + 1] & 0xF0) >> 4;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Returns the graph Node used by NodeA to route requests targeted at NodeB.
 *
 *     This will be a Node directly connected to NodeA (possibly NodeB itself),
 *     or "Route to Self" if NodeA and NodeB are the same Node.
 *     Note that all Node numbers are abstract Node numbers of the topology graph,
 *     it is the responsibility of the caller to apply any permutation needed.
 *
 * @param[in]     Graph the graph to examine
 * @param[in]     NodeA the Node number of the first Node
 * @param[in]     NodeB the Node number of the second Node
 *
 * @return The request route Node
 */
UINT8
GraphGetReq (
  IN       UINT8    *Graph,
  IN       UINT8    NodeA,
  IN       UINT8    NodeB
  )
{
  UINT8 size;
  size = Graph[0];
  ASSERT ((NodeA < size) && (NodeB < size));
  return (Graph[1 + (NodeA*size + NodeB)*2 + 1] & 0x0F);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Returns a bit vector of Nodes that NodeA should forward a broadcast from
 * NodeB towards
 *
 * @param[in]     Graph the graph to examine
 * @param[in]     NodeA the Node number of the first Node
 * @param[in]     NodeB the Node number of the second Node
 *
 * @return the broadcast routes for NodeA from NodeB
 */
UINT8
GraphGetBc (
  IN       UINT8    *Graph,
  IN       UINT8    NodeA,
  IN       UINT8    NodeB
  )
{
  UINT8 size;
  size = Graph[0];
  ASSERT ((NodeA < size) && (NodeB < size));
  return Graph[1 + (NodeA*size + NodeB)*2];
}

