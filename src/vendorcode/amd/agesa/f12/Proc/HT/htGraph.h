/* $NoKeywords:$ */
/**
 * @file
 *
 * Topology Interface.
 *
 * Contains interface to the topology data.
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

#ifndef _HT_GRAPH_H_
#define _HT_GRAPH_H_

/**
 * @page htgraphdesign Graph Support routines
 *
 * These routines provide support for dealing with the graph representation
 * of the topologies, along with the routing table information for that topology.
 * The routing information is compressed and these routines currently decompress
 * 'on the fly'.  A graph is represented as a set of routes. All the edges in the
 * graph are routes; a direct route from Node i to Node j exists in the graph IFF
 * there is an edge directly connecting Node i to Node j.  All other routes designate
 * the edge which the route to that Node initially takes, by designating a Node
 * to which a direct connection exists.  That is, the route to non-adjacent Node j
 * from Node i specifies Node k where Node i directly connects to Node k.
 *
 *@code
 * pseudo definition of compressed graph:
 * typedef struct
 * {
 *     // First byte
 *     UINT8 broadcast[8]:1;   // that is, 8 1-bit values
 *     // Second byte
 *     UINT8 requestRoute:4;   // [3:0]
 *     UINT8 responseRoute:4;  // [7:4]
 * } sRoute;
 * typedef struct
 * {
 *     UINT8 size;
 *     sRoute graph[size][size];
 * } sGraph;
 *@endcode
 */

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */

/*-----------------------------------------------------------------------------
 *                         DEFINITIONS AND MACROS
 *
 *-----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                         TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */
VOID
GetAmdTopolist (
  OUT   UINT8    ***List
  );

UINT8
GraphHowManyNodes (
  IN       UINT8    *Graph
  );

BOOLEAN
GraphIsAdjacent (
  IN       UINT8    *Graph,
  IN       UINT8    NodeA,
  IN       UINT8    NodeB
  );

UINT8
GraphGetRsp (
  IN       UINT8    *Graph,
  IN       UINT8    NodeA,
  IN       UINT8    NodeB
  );

UINT8
GraphGetReq (
  IN       UINT8    *Graph,
  IN       UINT8    NodeA,
  IN       UINT8    NodeB
  );

UINT8
GraphGetBc (
  IN       UINT8    *Graph,
  IN       UINT8    NodeA,
  IN       UINT8    NodeB
  );

#endif  /* _HT_GRAPH_H_ */

