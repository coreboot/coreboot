/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
 *----------------------------------------------------------------------------
 *				MODULES USED
 *
 *----------------------------------------------------------------------------
 */

#undef FILECODE
#define FILECODE 0xF001

#include "comlib.h"
#include "h3finit.h"
#include "h3ffeat.h"
#include "h3ncmn.h"
#include "h3gtopo.h"
#include "AsPsNb.h"
/* this is pre-ram so include the required C files here */
#include "comlib.c"
#include "AsPsNb.c"
#include "h3ncmn.c"

/*----------------------------------------------------------------------------
 *			DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

#undef FILECODE
#define FILECODE 0xF001

/* APIC defines from amdgesa.inc, which can't be included in to c code. */
#define APIC_Base_BSP	8
#define APIC_Base	0x1b

/*----------------------------------------------------------------------------
 *			TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *			PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *			EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *			LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
#ifndef HT_BUILD_NC_ONLY
/*
 **************************************************************************
 *			Routing table decompressor
 **************************************************************************
 */

/*
 **************************************************************************
 *	Graph Support routines
 * These routines provide support for dealing with the graph representation
 * of the topologies, along with the routing table information for that topology.
 * The routing information is compressed and these routines currently decompress
 * 'on the fly'.  A graph is represented as a set of routes. All the edges in the
 * graph are routes; a direct route from node i to node j exists in the graph IFF
 * there is an edge directly connecting node i to node j.  All other routes designate
 * the edge which the route to that node initially takes, by designating a node
 * to which a direct connection exists.  That is, the route to non-adjacent node j
 * from node i specifies node k where node i directly connects to node k.
 *
 *
 * pseudo definition of compressed graph:
 * typedef struct
 * {
 *	BIT  broadcast[8];
 *	uint4 responseRoute;
 *	uint4 requestRoute;
 * } sRoute;
 * typedef struct
 * {
 *	u8 size;
 *	sRoute graph[size][size];
 * } sGraph;
 *
 **************************************************************************
 */

/*----------------------------------------------------------------------------------------
 * u8
 * graphHowManyNodes(u8 *graph)
 *
 *  Description:
 *	 Returns the number of nodes in the compressed graph
 *
 *  Parameters:
 *	@param[in] u8 graph = a compressed graph
 *	@param[out] u8 results = the number of nodes in the graph
 * ---------------------------------------------------------------------------------------
 */
static u8 graphHowManyNodes(u8 *graph)
{
	return graph[0];
}

/*----------------------------------------------------------------------------------------
 * BOOL
 * graphIsAdjacent(u8 *graph, u8 nodeA, u8 nodeB)
 *
 *  Description:
 * Returns true if NodeA is directly connected to NodeB, false otherwise
 * (if NodeA == NodeB also returns false)
 * Relies on rule that directly connected nodes always route requests directly.
 *
 *  Parameters:
 *	@param[in]    u8    graph   = the graph to examine
 *	@param[in]    u8    nodeA   = the node number of the first node
 *	@param[in]    u8    nodeB   = the node number of the second node
 *	@param[out]   BOOL    results  = true if nodeA connects to nodeB false if not
 * ---------------------------------------------------------------------------------------
 */
static BOOL graphIsAdjacent(u8 *graph, u8 nodeA, u8 nodeB)
{
	u8 size = graph[0];
	ASSERT(size <= MAX_NODES);
	ASSERT((nodeA < size) && (nodeB < size));
	return (graph[1+(nodeA*size+nodeB)*2+1] & 0x0F) == nodeB;
}

/*----------------------------------------------------------------------------------------
 * u8
 * graphGetRsp(u8 *graph, u8 nodeA, u8 nodeB)
 *
 *  Description:
 *	Returns the graph node used by nodeA to route responses targeted at nodeB.
 *	This will be a node directly connected to nodeA (possibly nodeB itself),
 *	or "Route to Self" if nodeA and nodeB are the same node.
 *	Note that all node numbers are abstract node numbers of the topology graph,
 *	it is the responsibility of the caller to apply any permutation needed.
 *
 *  Parameters:
 *	@param[in]    u8    graph   = the graph to examine
 *	@param[in]    u8    nodeA   = the node number of the first node
 *	@param[in]    u8    nodeB   = the node number of the second node
 *	@param[out]   u8    results = The response route node
 * ---------------------------------------------------------------------------------------
 */
static u8 graphGetRsp(u8 *graph, u8 nodeA, u8 nodeB)
{
	u8 size = graph[0];
	ASSERT(size <= MAX_NODES);
	ASSERT((nodeA < size) && (nodeB < size));
	return (graph[1+(nodeA*size+nodeB)*2+1] & 0xF0)>>4;
}

/*----------------------------------------------------------------------------------------
 * u8
 * graphGetReq(u8 *graph, u8 nodeA, u8 nodeB)
 *
 *  Description:
 *	 Returns the graph node used by nodeA to route requests targeted at nodeB.
 *	This will be a node directly connected to nodeA (possibly nodeB itself),
 *	or "Route to Self" if nodeA and nodeB are the same node.
 *	Note that all node numbers are abstract node numbers of the topology graph,
 *	it is the responsibility of the caller to apply any permutation needed.
 *
 *  Parameters:
 *	@param[in]    u8    graph   = the graph to examine
 *	@param[in]    u8    nodeA   = the node number of the first node
 *	@param[in]    u8    nodeB   = the node number of the second node
 *	@param[out]   u8    results = The request route node
 * ---------------------------------------------------------------------------------------
 */
static u8 graphGetReq(u8 *graph, u8 nodeA, u8 nodeB)
{
	u8 size = graph[0];
	ASSERT(size <= MAX_NODES);
	ASSERT((nodeA < size) && (nodeB < size));
	return (graph[1+(nodeA*size+nodeB)*2+1] & 0x0F);
}

/*----------------------------------------------------------------------------------------
 * u8
 * graphGetBc(u8 *graph, u8 nodeA, u8 nodeB)
 *
 *  Description:
 *	 Returns a bit vector of nodes that nodeA should forward a broadcast from
 *	 nodeB towards
 *
 *  Parameters:
 *	@param[in]    u8    graph   = the graph to examine
 *	@param[in]    u8    nodeA   = the node number of the first node
 *	@param[in]    u8    nodeB   = the node number of the second node
 *	OU    u8    results = the broadcast routes for nodeA from nodeB
 * ---------------------------------------------------------------------------------------
 */
static u8 graphGetBc(u8 *graph, u8 nodeA, u8 nodeB)
{
	u8 size = graph[0];
	ASSERT(size <= MAX_NODES);
	ASSERT((nodeA < size) && (nodeB < size));
	return graph[1+(nodeA*size+nodeB)*2];
}


/***************************************************************************
 ***		GENERIC HYPERTRANSPORT DISCOVERY CODE			***
 ***************************************************************************/

/*----------------------------------------------------------------------------------------
 * void
 * routeFromBSP(u8 targetNode, u8 actualTarget, sMainData *pDat)
 *
 *  Description:
 *	 Ensure a request / response route from target node to bsp.  Since target node is
 *	 always a predecessor of actual target node, each node gets a route to actual target
 *	 on the link that goes to target.  The routing produced by this routine is adequate
 *	 for config access during discovery, but NOT for coherency.
 *
 *  Parameters:
 *	@param[in]    u8    targetNode   = the path to actual target goes through target
 *	@param[in]    u8    actualTarget = the ultimate target being routed to
 *	@param[in]    sMainData*  pDat   = our global state, port config info
 * ---------------------------------------------------------------------------------------
 */
static void routeFromBSP(u8 targetNode, u8 actualTarget, sMainData *pDat)
{
	u8 predecessorNode, predecessorLink, currentPair;

	if (targetNode == 0)
		return;  /*  BSP has no predecessor, stop */

	/*  Search for the link that connects targetNode to its predecessor */
	currentPair = 0;
	while (pDat->PortList[currentPair*2+1].NodeID != targetNode)
	{
		currentPair++;
		ASSERT(currentPair < pDat->TotalLinks);
	}

	predecessorNode = pDat->PortList[currentPair*2].NodeID;
	predecessorLink = pDat->PortList[currentPair*2].Link;

	/*  Recursively call self to ensure the route from the BSP to the Predecessor */
	/*  Node is established */
	routeFromBSP(predecessorNode, actualTarget, pDat);

	pDat->nb->writeRoutingTable(predecessorNode, actualTarget, predecessorLink, pDat->nb);
}

/*----------------------------------------------------------------------------------------
 * u8
 * convertNodeToLink(u8 srcNode, u8 targetNode, sMainData *pDat)
 *
 *  Description:
 *	 Return the link on source node which connects to target node
 *
 *  Parameters:
 *	@param[in]    u8    srcNode    = the source node
 *	@param[in]    u8    targetNode = the target node to find the link to
 *	@param[in]    sMainData*  pDat = our global state
 *	@param[out]   u8    results    = the link on source which connects to target
 * ---------------------------------------------------------------------------------------
 */
static u8 convertNodeToLink(u8 srcNode, u8 targetNode, sMainData *pDat)
{
	u8 targetlink = INVALID_LINK;
	u8 k;

	for (k = 0; k < pDat->TotalLinks*2; k += 2)
	{
		if ((pDat->PortList[k+0].NodeID == srcNode) && (pDat->PortList[k+1].NodeID == targetNode))
		{
			targetlink = pDat->PortList[k+0].Link;
			break;
		}
		else if ((pDat->PortList[k+1].NodeID == srcNode) && (pDat->PortList[k+0].NodeID == targetNode))
		{
			targetlink = pDat->PortList[k+1].Link;
			break;
		}
	}
	ASSERT(targetlink != INVALID_LINK);

	return targetlink;
}


/*----------------------------------------------------------------------------------------
 * void
 * htDiscoveryFloodFill(sMainData *pDat)
 *
 *  Description:
 *	 Discover all coherent devices in the system, initializing some basics like node IDs
 *	 and total nodes found in the process. As we go we also build a representation of the
 *	 discovered system which we will use later to program the routing tables.  During this
 *	 step, the routing is via default link back to BSP and to each new node on the link it
 *	 was discovered on (no coherency is active yet).
 *
 *  Parameters:
 *	@param[in]    sMainData*  pDat = our global state
 * ---------------------------------------------------------------------------------------
 */
static void htDiscoveryFloodFill(sMainData *pDat)
{
	u8 currentNode = 0;
	u8 currentLink;

	/* Entries are always added in pairs, the even indices are the 'source'
	 * side closest to the BSP, the odd indices are the 'destination' side
	 */

	while (currentNode <= pDat->NodesDiscovered)
	{
		u32 temp;

		if (currentNode != 0)
		{
			/* Set path from BSP to currentNode */
			routeFromBSP(currentNode, currentNode, pDat);

			/* Set path from BSP to currentNode for currentNode+1 if
			 * currentNode+1 != MAX_NODES
			 */
			if (currentNode+1 != MAX_NODES)
				routeFromBSP(currentNode, currentNode+1, pDat);

			/* Configure currentNode to route traffic to the BSP through its
			 * default link
			 */
			pDat->nb->writeRoutingTable(currentNode, 0, pDat->nb->readDefLnk(currentNode, pDat->nb), pDat->nb);
		}

		/* Set currentNode's NodeID field to currentNode */
		pDat->nb->writeNodeID(currentNode, currentNode, pDat->nb);

		/* Enable routing tables on currentNode*/
		pDat->nb->enableRoutingTables(currentNode, pDat->nb);

		for (currentLink = 0; currentLink < pDat->nb->maxLinks; currentLink++)
		{
			BOOL linkfound;
			u8 token;

			if (pDat->HtBlock->AMD_CB_IgnoreLink && pDat->HtBlock->AMD_CB_IgnoreLink(currentNode, currentLink))
				continue;

			if (pDat->nb->readTrueLinkFailStatus(currentNode, currentLink, pDat, pDat->nb))
				continue;

			/* Make sure that the link is connected, coherent, and ready */
			if (!pDat->nb->verifyLinkIsCoherent(currentNode, currentLink, pDat->nb))
				continue;


			/* Test to see if the currentLink has already been explored */
			linkfound = FALSE;
			for (temp = 0; temp < pDat->TotalLinks; temp++)
			{
				if ((pDat->PortList[temp*2+1].NodeID == currentNode) &&
				   (pDat->PortList[temp*2+1].Link == currentLink))
				{
					linkfound = TRUE;
					break;
				}
			}
			if (linkfound)
			{
				/* We had already expored this link */
				continue;
			}

			if (pDat->nb->handleSpecialLinkCase(currentNode, currentLink, pDat, pDat->nb))
			{
				continue;
			}

			/* Modify currentNode's routing table to use currentLink to send
			 * traffic to currentNode+1
			 */
			pDat->nb->writeRoutingTable(currentNode, currentNode+1, currentLink, pDat->nb);

			/* Check the northbridge of the node we just found, to make sure it is compatible
			 * before doing anything else to it.
			 */
			if (!pDat->nb->isCompatible(currentNode+1, pDat->nb))
			{
				u8 nodeToKill;

				/* Notify BIOS of event (while variables are still the same) */
				if (pDat->HtBlock->AMD_CB_EventNotify)
				{
					sHtEventCohFamilyFeud evt;
					evt.eSize = sizeof(sHtEventCohFamilyFeud);
					evt.node = currentNode;
					evt.link = currentLink;
					evt.totalNodes = pDat->NodesDiscovered;

					pDat->HtBlock->AMD_CB_EventNotify(HT_EVENT_CLASS_ERROR,
									HT_EVENT_COH_FAMILY_FEUD,
									(u8 *)&evt);
				}

				/* If node is not compatible, force boot to 1P
				 * If they are not compatible stop cHT init and:
				 *	1. Disable all cHT links on the BSP
				 *	2. Configure the BSP routing tables as a UP.
				 *	3. Notify main BIOS.
				 */
				pDat->NodesDiscovered = 0;
				currentNode = 0;
				pDat->TotalLinks = 0;
				/* Abandon our coherent link data structure.  At this point there may
				 * be coherent links on the BSP that are not yet in the portList, and
				 * we have to turn them off anyway.  So depend on the hardware to tell us.
				 */
				for (currentLink = 0; currentLink < pDat->nb->maxLinks; currentLink++)
				{
					/* Stop all links which are connected, coherent, and ready */
					if (pDat->nb->verifyLinkIsCoherent(currentNode, currentLink, pDat->nb))
						pDat->nb->stopLink(currentNode, currentLink, pDat->nb);
				}

				for (nodeToKill = 0; nodeToKill < pDat->nb->maxNodes; nodeToKill++)
				{
					pDat->nb->writeFullRoutingTable(0, nodeToKill, ROUTETOSELF, ROUTETOSELF, 0, pDat->nb);
				}

				/* End Coherent Discovery */
				STOP_HERE;
				break;
			}

			/* Read token from Current+1 */
			token = pDat->nb->readToken(currentNode+1, pDat->nb);
			ASSERT(token <= pDat->NodesDiscovered);
			if (token == 0)
			{
				pDat->NodesDiscovered++;
				ASSERT(pDat->NodesDiscovered < pDat->nb->maxNodes);
				/* Check the capability of northbridges against the currently known configuration */
				if (!pDat->nb->isCapable(currentNode+1, pDat, pDat->nb))
				{
					u8 nodeToKill;

					/* Notify BIOS of event  */
					if (pDat->HtBlock->AMD_CB_EventNotify)
					{
						sHtEventCohMpCapMismatch evt;
						evt.eSize = sizeof(sHtEventCohMpCapMismatch);
						evt.node = currentNode;
						evt.link = currentLink;
						evt.sysMpCap = pDat->sysMpCap;
						evt.totalNodes = pDat->NodesDiscovered;

						pDat->HtBlock->AMD_CB_EventNotify(HT_EVENT_CLASS_ERROR,
									HT_EVENT_COH_MPCAP_MISMATCH,
									(u8 *)&evt);
					}

					pDat->NodesDiscovered = 0;
					currentNode = 0;
					pDat->TotalLinks = 0;

					for (nodeToKill = 0; nodeToKill < pDat->nb->maxNodes; nodeToKill++)
					{
						pDat->nb->writeFullRoutingTable(0, nodeToKill, ROUTETOSELF, ROUTETOSELF, 0, pDat->nb);
					}

					/* End Coherent Discovery */
					STOP_HERE;
					break;
				}

				token = pDat->NodesDiscovered;
				pDat->nb->writeToken(currentNode+1, token, pDat->nb);
				/* Inform that we have discovered a node, so that logical id to
				 * socket mapping info can be recorded.
				 */
				if (pDat->HtBlock->AMD_CB_EventNotify)
				{
					sHtEventCohNodeDiscovered evt;
					evt.eSize = sizeof(sHtEventCohNodeDiscovered);
					evt.node = currentNode;
					evt.link = currentLink;
					evt.newNode = token;

					pDat->HtBlock->AMD_CB_EventNotify(HT_EVENT_CLASS_INFO,
								HT_EVENT_COH_NODE_DISCOVERED,
								(u8 *)&evt);
				}
			}

			if (pDat->TotalLinks == MAX_PLATFORM_LINKS)
			{
				/*
				 * Exceeded our capacity to describe all coherent links found in the system.
				 * Error strategy:
				 * Auto recovery is not possible because data space is already all used.
				 * If the callback is not implemented or returns we will continue to initialize
				 * the fabric we are capable of representing, adding no more nodes or links.
				 * This should yield a bootable topology, but likely not the one intended.
				 * We cannot continue discovery, there may not be any way to route a new
				 * node back to the BSP if we can't add links to our representation of the system.
				 */
				if (pDat->HtBlock->AMD_CB_EventNotify)
				{
					sHtEventCohLinkExceed evt;
					evt.eSize = sizeof(sHtEventCohLinkExceed);
					evt.node = currentNode;
					evt.link = currentLink;
					evt.targetNode = token;
					evt.totalNodes = pDat->NodesDiscovered;
					evt.maxLinks = pDat->nb->maxLinks;

					pDat->HtBlock->AMD_CB_EventNotify(HT_EVENT_CLASS_ERROR,
									HT_EVENT_COH_LINK_EXCEED,
									(u8 *)&evt);
				}
				/* Force link and node loops to halt */
				STOP_HERE;
				currentNode = pDat->NodesDiscovered;
				break;
			}

			pDat->PortList[pDat->TotalLinks*2].Type = PORTLIST_TYPE_CPU;
			pDat->PortList[pDat->TotalLinks*2].Link = currentLink;
			pDat->PortList[pDat->TotalLinks*2].NodeID = currentNode;

			pDat->PortList[pDat->TotalLinks*2+1].Type = PORTLIST_TYPE_CPU;
			pDat->PortList[pDat->TotalLinks*2+1].Link = pDat->nb->readDefLnk(currentNode+1, pDat->nb);
			pDat->PortList[pDat->TotalLinks*2+1].NodeID = token;

			pDat->TotalLinks++;

			if ( !pDat->sysMatrix[currentNode][token] )
			{
				pDat->sysDegree[currentNode]++;
				pDat->sysDegree[token]++;
				pDat->sysMatrix[currentNode][token] = TRUE;
				pDat->sysMatrix[token][currentNode] = TRUE;
			}
		}
		currentNode++;
	}
}


/***************************************************************************
 ***		ISOMORPHISM BASED ROUTING TABLE GENERATION CODE		***
 ***************************************************************************/

/*----------------------------------------------------------------------------------------
 * BOOL
 * isoMorph(u8 i, sMainData *pDat)
 *
 *  Description:
 *	 Is graphA isomorphic to graphB?
 *	 if this function returns true, then Perm will contain the permutation
 *	 required to transform graphB into graphA.
 *	 We also use the degree of each node, that is the number of connections it has, to
 *	 speed up rejection of non-isomorphic graphs (if there is a node in graphA with n
 *	 connections, there must be at least one unmatched in graphB with n connections).
 *
 *  Parameters:
 *	@param[in] u8 i   = the discovered node which we are trying to match
 *		    with a permutation the topology
 *	@param[in]/@param[out] sMainData* pDat  = our global state, degree and adjacency matrix,
 *				  output a permutation if successful
 *	@param[out] BOOL results = the graphs are (or are not) isomorphic
 * ---------------------------------------------------------------------------------------
 */
static BOOL isoMorph(u8 i, sMainData *pDat)
{
	u8 j, k;
	u8 nodecnt;

	/* We have only been called if nodecnt == pSelected->size ! */
	nodecnt = pDat->NodesDiscovered+1;

	if (i != nodecnt)
	{
		/*  Keep building the permutation */
		for (j = 0; j < nodecnt; j++)
		{
			/*  Make sure the degree matches */
			if (pDat->sysDegree[i] != pDat->dbDegree[j])
				continue;

			/*  Make sure that j hasn't been used yet (ought to use a "used" */
			/*  array instead, might be faster) */
			for (k = 0; k < i; k++)
			{
				if (pDat->Perm[k] == j)
					break;
			}
			if (k != i)
				continue;
			pDat->Perm[i] = j;
			if (isoMorph(i+1, pDat))
				return TRUE;
		}
		return FALSE;
	} else {
		/*  Test to see if the permutation is isomorphic */
		for (j = 0; j < nodecnt; j++)
		{
			for (k = 0; k < nodecnt; k++)
			{
				if ( pDat->sysMatrix[j][k] !=
				   pDat->dbMatrix[pDat->Perm[j]][pDat->Perm[k]] )
					return FALSE;
			}
		}
		return TRUE;
	}
}


/*----------------------------------------------------------------------------------------
 * void
 * lookupComputeAndLoadRoutingTables(sMainData *pDat)
 *
 *  Description:
 *	 Using the description of the fabric topology we discovered, try to find a match
 *	 among the supported topologies.  A supported topology description matches
 *	 the discovered fabric if the nodes can be matched in such a way that all the nodes connected
 *	 in one set are exactly the nodes connected in the other (formally, that the graphs are
 *	 isomorphic).  Which links are used is not really important to matching.  If the graphs
 *	 match, then there is a permutation of one that translates the node positions and linkages
 *	 to the other.
 *
 *	 In order to make the isomorphism test efficient, we test for matched number of nodes
 *	 (a 4 node fabric is not isomorphic to a 2 node topology), and provide degrees of nodes
 *	 to the isomorphism test.
 *
 *	 The generic routing table solution for any topology is predetermined and represented
 *	 as part of the topology.  The permutation we computed tells us how to interpret the
 *	 routing onto the fabric we discovered.	 We do this working backward from the last
 *	 node discovered to the BSP, writing the routing tables as we go.
 *
 *  Parameters:
 *	@param[in]    sMainData* pDat = our global state, the discovered fabric,
 *	@param[out]			degree matrix, permutation
 * ---------------------------------------------------------------------------------------
 */
static void lookupComputeAndLoadRoutingTables(sMainData *pDat)
{
	u8 **pTopologyList;
	u8 *pSelected;

	int i, j, k, size;

	size = pDat->NodesDiscovered + 1;
	/* Use the provided topology list or the internal, default one. */
	pTopologyList = pDat->HtBlock->topolist;
	if (pTopologyList == NULL)
	{
		getAmdTopolist(&pTopologyList);
	}

	pSelected = *pTopologyList;
	while (pSelected != NULL)
	{
		if (graphHowManyNodes(pSelected) == size)
		{
			/*  Build Degree vector and Adjency Matrix for this entry */
			for (i = 0; i < size; i++)
			{
				pDat->dbDegree[i] = 0;
				for (j = 0; j < size; j++)
				{
					if (graphIsAdjacent(pSelected, i, j))
					{
						pDat->dbMatrix[i][j] = 1;
						pDat->dbDegree[i]++;
					}
					else
					{
						pDat->dbMatrix[i][j] = 0;
					}
				}
			}
			if (isoMorph(0, pDat))
				break;  /*  A matching topology was found */
		}

		pTopologyList++;
		pSelected = *pTopologyList;
	}

	if (pSelected != NULL)
	{
		/*  Compute the reverse Permutation */
		for (i = 0; i < size; i++)
		{
			pDat->ReversePerm[pDat->Perm[i]] = i;
		}

		/*  Start with the last discovered node, and move towards the BSP */
		for (i = size-1; i >= 0; i--)
		{
			for (j = 0; j < size; j++)
			{
				u8 ReqTargetLink, RspTargetLink;
				u8 ReqTargetNode, RspTargetNode;

				u8 AbstractBcTargetNodes = graphGetBc(pSelected, pDat->Perm[i], pDat->Perm[j]);
				u32 BcTargetLinks = 0;

				for (k = 0; k < MAX_NODES; k++)
				{
					if (AbstractBcTargetNodes & ((u32)1<<k))
					{
						BcTargetLinks |= (u32)1 << convertNodeToLink(i, pDat->ReversePerm[k], pDat);
					}
				}

				if (i == j)
				{
					ReqTargetLink = ROUTETOSELF;
					RspTargetLink = ROUTETOSELF;
				}
				else
				{
					ReqTargetNode = graphGetReq(pSelected, pDat->Perm[i], pDat->Perm[j]);
					ReqTargetLink = convertNodeToLink(i, pDat->ReversePerm[ReqTargetNode], pDat);

					RspTargetNode = graphGetRsp(pSelected, pDat->Perm[i], pDat->Perm[j]);
					RspTargetLink = convertNodeToLink(i, pDat->ReversePerm[RspTargetNode], pDat);
				}

				pDat->nb->writeFullRoutingTable(i, j, ReqTargetLink, RspTargetLink, BcTargetLinks, pDat->nb);
			}
			/* Clean up discovery 'footprint' that otherwise remains in the routing table.  It didn't hurt
			 * anything, but might cause confusion during debug and validation.  Do this by setting the
			 * route back to all self routes. Since it's the node that would be one more than actually installed,
			 * this only applies if less than maxNodes were found.
			 */
			if (size < pDat->nb->maxNodes)
			{
				pDat->nb->writeFullRoutingTable(i, size, ROUTETOSELF, ROUTETOSELF, 0, pDat->nb);
			}
		}

	}
	else
	{
		/*
		 * No Matching Topology was found
		 * Error Strategy:
		 * Auto recovery doesn't seem likely, Force boot as 1P.
		 * For reporting, logging, provide number of nodes
		 * If not implemented or returns, boot as BSP uniprocessor.
		 */
		if (pDat->HtBlock->AMD_CB_EventNotify)
		{
			sHtEventCohNoTopology evt;
			evt.eSize = sizeof(sHtEventCohNoTopology);
			evt.totalNodes = pDat->NodesDiscovered;

			pDat->HtBlock->AMD_CB_EventNotify(HT_EVENT_CLASS_ERROR,
						HT_EVENT_COH_NO_TOPOLOGY,
						(u8 *)&evt);
		}
		STOP_HERE;
		/* Force 1P */
		pDat->NodesDiscovered = 0;
		pDat->TotalLinks = 0;
		pDat->nb->enableRoutingTables(0, pDat->nb);
	}
}
#endif /* HT_BUILD_NC_ONLY */


/*----------------------------------------------------------------------------------------
 * void
 * finializeCoherentInit(sMainData *pDat)
 *
 *  Description:
 *	 Find the total number of cores and update the number of nodes and cores in all cpus.
 *	 Limit cpu config access to installed cpus.
 *
 *  Parameters:
 *	@param[in] sMainData* pDat = our global state, number of nodes discovered.
 * ---------------------------------------------------------------------------------------
 */
static void finializeCoherentInit(sMainData *pDat)
{
	u8 curNode;

	u8 totalCores = 0;
	for (curNode = 0; curNode < pDat->NodesDiscovered+1; curNode++)
	{
		totalCores += pDat->nb->getNumCoresOnNode(curNode, pDat->nb);
	}

	for (curNode = 0; curNode < pDat->NodesDiscovered+1; curNode++)
	{
		pDat->nb->setTotalNodesAndCores(curNode, pDat->NodesDiscovered+1, totalCores, pDat->nb);
	}

	for (curNode = 0; curNode < pDat->NodesDiscovered+1; curNode++)
	{
		pDat->nb->limitNodes(curNode, pDat->nb);
	}

}

/*----------------------------------------------------------------------------------------
 * void
 * coherentInit(sMainData *pDat)
 *
 *  Description:
 *	 Perform discovery and initialization of the coherent fabric.
 *
 *  Parameters:
 *	@param[in] sMainData* pDat = our global state
 * ---------------------------------------------------------------------------------------
 */
static void coherentInit(sMainData *pDat)
{
#ifdef HT_BUILD_NC_ONLY
	/* Replace discovery process with:
	 * No other nodes, no coherent links
	 * Enable routing tables on currentNode, for power on self route
	 */
	pDat->NodesDiscovered = 0;
	pDat->TotalLinks = 0;
	pDat->nb->enableRoutingTables(0, pDat->nb);
#else
	u8 i, j;

	pDat->NodesDiscovered = 0;
	pDat->TotalLinks = 0;
	for (i = 0; i < MAX_NODES; i++)
	{
		pDat->sysDegree[i] = 0;
		for (j = 0; j < MAX_NODES; j++)
		{
			pDat->sysMatrix[i][j] = 0;
		}
	}

	htDiscoveryFloodFill(pDat);
	lookupComputeAndLoadRoutingTables(pDat);
#endif
	finializeCoherentInit(pDat);
}

/***************************************************************************
 ***			    Non-coherent init code			  ***
 ***				  Algorithms				  ***
 ***************************************************************************/
/*----------------------------------------------------------------------------------------
 * void
 * processLink(u8 node, u8 link, sMainData *pDat)
 *
 *  Description:
 *	 Process a non-coherent link, enabling a range of bus numbers, and setting the device
 *	 ID for all devices found
 *
 *  Parameters:
 *	@param[in] u8 node = Node on which to process nc init
 *	@param[in] u8 link = The non-coherent link on that node
 *	@param[in] sMainData* pDat = our global state
 * ---------------------------------------------------------------------------------------
 */
static void processLink(u8 node, u8 link, sMainData *pDat)
{
	u8 secBus, subBus;
	u32 currentBUID;
	u32 temp;
	u32 unitIDcnt;
	SBDFO currentPtr;
	u8 depth;
	const u8 *pSwapPtr;

	SBDFO lastSBDFO = ILLEGAL_SBDFO;
	u8 lastLink = 0;

	ASSERT(node < pDat->nb->maxNodes && link < pDat->nb->maxLinks);

	if ((pDat->HtBlock->AMD_CB_OverrideBusNumbers == NULL)
	   || !pDat->HtBlock->AMD_CB_OverrideBusNumbers(node, link, &secBus, &subBus))
	{
		/* Assign Bus numbers */
		if (pDat->AutoBusCurrent >= pDat->HtBlock->AutoBusMax)
		{
			/* If we run out of Bus Numbers notify, if call back unimplemented or if it
			 * returns, skip this chain
			 */
			if (pDat->HtBlock->AMD_CB_EventNotify)
			{
				sHTEventNcohBusMaxExceed evt;
				evt.eSize = sizeof(sHTEventNcohBusMaxExceed);
				evt.node = node;
				evt.link = link;
				evt.bus = pDat->AutoBusCurrent;

				pDat->HtBlock->AMD_CB_EventNotify(HT_EVENT_CLASS_ERROR,HT_EVENT_NCOH_BUS_MAX_EXCEED,(u8 *)&evt);
			}
			STOP_HERE;
			return;
		}

		if (pDat->UsedCfgMapEntires >= 4)
		{
			/* If we have used all the PCI Config maps we can't add another chain.
			 * Notify and if call back is unimplemented or returns, skip this chain.
			 */
			if (pDat->HtBlock->AMD_CB_EventNotify)
			{
				sHtEventNcohCfgMapExceed evt;
				evt.eSize = sizeof(sHtEventNcohCfgMapExceed);
				evt.node = node;
				evt.link = link;

				pDat->HtBlock->AMD_CB_EventNotify(HT_EVENT_CLASS_ERROR,
							HT_EVENT_NCOH_CFG_MAP_EXCEED,
							(u8 *)&evt);
			}
			STOP_HERE;
			return;
		}

		secBus = pDat->AutoBusCurrent;
		subBus = secBus + pDat->HtBlock->AutoBusIncrement-1;
		pDat->AutoBusCurrent += pDat->HtBlock->AutoBusIncrement;
	}

	pDat->nb->setCFGAddrMap(pDat->UsedCfgMapEntires, secBus, subBus, node, link, pDat, pDat->nb);
	pDat->UsedCfgMapEntires++;

	if ((pDat->HtBlock->AMD_CB_ManualBUIDSwapList != NULL)
	 && pDat->HtBlock->AMD_CB_ManualBUIDSwapList(node, link, &pSwapPtr))
	{
		/* Manual non-coherent BUID assignment */

		/* Assign BUID's per manual override */
		while (*pSwapPtr != 0xFF)
		{
			currentPtr = MAKE_SBDFO(0, secBus, *pSwapPtr, 0, 0);
			pSwapPtr++;

			do
			{
				AmdPCIFindNextCap(&currentPtr);
				ASSERT(currentPtr != ILLEGAL_SBDFO);
				AmdPCIRead(currentPtr, &temp);
			} while (!IS_HT_SLAVE_CAPABILITY(temp));

			currentBUID = *pSwapPtr;
			pSwapPtr++;
			AmdPCIWriteBits(currentPtr, 20, 16, &currentBUID);
		}

		/* Build chain of devices */
		depth = 0;
		pSwapPtr++;
		while (*pSwapPtr != 0xFF)
		{
			pDat->PortList[pDat->TotalLinks*2].NodeID = node;
			if (depth == 0)
			{
				pDat->PortList[pDat->TotalLinks*2].Type = PORTLIST_TYPE_CPU;
				pDat->PortList[pDat->TotalLinks*2].Link = link;
			}
			else
			{
				pDat->PortList[pDat->TotalLinks*2].Type = PORTLIST_TYPE_IO;
				pDat->PortList[pDat->TotalLinks*2].Link = 1-lastLink;
				pDat->PortList[pDat->TotalLinks*2].HostLink = link;
				pDat->PortList[pDat->TotalLinks*2].HostDepth = depth-1;
				pDat->PortList[pDat->TotalLinks*2].Pointer = lastSBDFO;
			}

			pDat->PortList[pDat->TotalLinks*2+1].Type = PORTLIST_TYPE_IO;
			pDat->PortList[pDat->TotalLinks*2+1].NodeID = node;
			pDat->PortList[pDat->TotalLinks*2+1].HostLink = link;
			pDat->PortList[pDat->TotalLinks*2+1].HostDepth = depth;

			currentPtr = MAKE_SBDFO(0, secBus, (*pSwapPtr & 0x3F), 0, 0);
			do
			{
				AmdPCIFindNextCap(&currentPtr);
				ASSERT(currentPtr != ILLEGAL_SBDFO);
				AmdPCIRead(currentPtr, &temp);
			} while (!IS_HT_SLAVE_CAPABILITY(temp));
			pDat->PortList[pDat->TotalLinks*2+1].Pointer = currentPtr;
			lastSBDFO = currentPtr;

			/* Bit 6 indicates whether orientation override is desired.
			 * Bit 7 indicates the upstream link if overriding.
			 */
			/* assert catches at least the one known incorrect setting */
			ASSERT ((*pSwapPtr & 0x40) || (!(*pSwapPtr & 0x80)));
			if (*pSwapPtr & 0x40)
			{
				/* Override the device's orientation */
				lastLink = *pSwapPtr >> 7;
			}
			else
			{
				/* Detect the device's orientation */
				AmdPCIReadBits(currentPtr, 26, 26, &temp);
				lastLink = (u8)temp;
			}
			pDat->PortList[pDat->TotalLinks*2+1].Link = lastLink;

			depth++;
			pDat->TotalLinks++;
			pSwapPtr++;
		}
	}
	else
	{
		/* Automatic non-coherent device detection */
		depth = 0;
		currentBUID = 1;
		while (1)
		{
			currentPtr = MAKE_SBDFO(0, secBus, 0, 0, 0);

			AmdPCIRead(currentPtr, &temp);
			if (temp == 0xFFFFFFFF)
				/* No device found at currentPtr */
				break;

			if (pDat->TotalLinks == MAX_PLATFORM_LINKS)
			{
				/*
				 * Exceeded our capacity to describe all non-coherent links found in the system.
				 * Error strategy:
				 * Auto recovery is not possible because data space is already all used.
				 */
				if (pDat->HtBlock->AMD_CB_EventNotify)
				{
					sHtEventNcohLinkExceed evt;
					evt.eSize = sizeof(sHtEventNcohLinkExceed);
					evt.node = node;
					evt.link = link;
					evt.depth = depth;
					evt.maxLinks = pDat->nb->maxLinks;

					pDat->HtBlock->AMD_CB_EventNotify(HT_EVENT_CLASS_ERROR,
								HT_EVENT_NCOH_LINK_EXCEED,
								(u8 *)&evt);
				}
				/* Force link loop to halt */
				STOP_HERE;
				break;
			}

			pDat->PortList[pDat->TotalLinks*2].NodeID = node;
			if (depth == 0)
			{
				pDat->PortList[pDat->TotalLinks*2].Type = PORTLIST_TYPE_CPU;
				pDat->PortList[pDat->TotalLinks*2].Link = link;
			}
			else
			{
				pDat->PortList[pDat->TotalLinks*2].Type = PORTLIST_TYPE_IO;
				pDat->PortList[pDat->TotalLinks*2].Link = 1-lastLink;
				pDat->PortList[pDat->TotalLinks*2].HostLink = link;
				pDat->PortList[pDat->TotalLinks*2].HostDepth = depth-1;
				pDat->PortList[pDat->TotalLinks*2].Pointer = lastSBDFO;
			}

			pDat->PortList[pDat->TotalLinks*2+1].Type = PORTLIST_TYPE_IO;
			pDat->PortList[pDat->TotalLinks*2+1].NodeID = node;
			pDat->PortList[pDat->TotalLinks*2+1].HostLink = link;
			pDat->PortList[pDat->TotalLinks*2+1].HostDepth = depth;

			do
			{
				AmdPCIFindNextCap(&currentPtr);
				ASSERT(currentPtr != ILLEGAL_SBDFO);
				AmdPCIRead(currentPtr, &temp);
			} while (!IS_HT_SLAVE_CAPABILITY(temp));

			AmdPCIReadBits(currentPtr, 25, 21, &unitIDcnt);
			if ((unitIDcnt + currentBUID > 31) || ((secBus == 0) && (unitIDcnt + currentBUID > 24)))
			{
				/* An error handler for the case where we run out of BUID's on a chain */
				if (pDat->HtBlock->AMD_CB_EventNotify)
				{
					sHtEventNcohBuidExceed evt;
					evt.eSize = sizeof(sHtEventNcohBuidExceed);
					evt.node = node;
					evt.link = link;
					evt.depth = depth;
					evt.currentBUID = (uint8)currentBUID;
					evt.unitCount = (uint8)unitIDcnt;

					pDat->HtBlock->AMD_CB_EventNotify(HT_EVENT_CLASS_ERROR,HT_EVENT_NCOH_BUID_EXCEED,(u8 *)&evt);
				}
				STOP_HERE;
				break;
			}
			AmdPCIWriteBits(currentPtr, 20, 16, &currentBUID);


			currentPtr += MAKE_SBDFO(0, 0, currentBUID, 0, 0);
			AmdPCIReadBits(currentPtr, 20, 16, &temp);
			if (temp != currentBUID)
			{
				/* An error handler for this critical error */
				if (pDat->HtBlock->AMD_CB_EventNotify)
				{
					sHtEventNcohDeviceFailed evt;
					evt.eSize = sizeof(sHtEventNcohDeviceFailed);
					evt.node = node;
					evt.link = link;
					evt.depth = depth;
					evt.attemptedBUID = (uint8)currentBUID;

					pDat->HtBlock->AMD_CB_EventNotify(HT_EVENT_CLASS_ERROR,HT_EVENT_NCOH_DEVICE_FAILED,(u8 *)&evt);
				}
				STOP_HERE;
				break;
			}

			AmdPCIReadBits(currentPtr, 26, 26, &temp);
			pDat->PortList[pDat->TotalLinks*2+1].Link = (u8)temp;
			pDat->PortList[pDat->TotalLinks*2+1].Pointer = currentPtr;

			lastLink = (u8)temp;
			lastSBDFO = currentPtr;

			depth++;
			pDat->TotalLinks++;
			currentBUID += unitIDcnt;
		}
		if (pDat->HtBlock->AMD_CB_EventNotify)
		{
			/* Provide information on automatic device results */
			sHtEventNcohAutoDepth evt;
			evt.eSize = sizeof(sHtEventNcohAutoDepth);
			evt.node = node;
			evt.link = link;
			evt.depth = (depth - 1);

			pDat->HtBlock->AMD_CB_EventNotify(HT_EVENT_CLASS_INFO,HT_EVENT_NCOH_AUTO_DEPTH,(u8 *)&evt);
		}
	}
}


/*----------------------------------------------------------------------------------------
 * void
 * ncInit(sMainData *pDat)
 *
 *  Description:
 *	 Initialize the non-coherent fabric. Begin with the compat link on the BSP, then
 *	 find and initialize all other non-coherent chains.
 *
 *  Parameters:
 *	@param[in]  sMainData*  pDat = our global state
 * ---------------------------------------------------------------------------------------
 */
static void ncInit(sMainData *pDat)
{
	u8 node, link;
	u8 compatLink;

	compatLink = pDat->nb->readSbLink(pDat->nb);
	processLink(0, compatLink, pDat);

	for (node = 0; node <= pDat->NodesDiscovered; node++)
	{
		for (link = 0; link < pDat->nb->maxLinks; link++)
		{
			if (pDat->HtBlock->AMD_CB_IgnoreLink && pDat->HtBlock->AMD_CB_IgnoreLink(node, link))
				continue;   /*  Skip the link */

			if (node == 0 && link == compatLink)
				continue;

			if (pDat->nb->readTrueLinkFailStatus(node, link, pDat, pDat->nb))
				continue;

			if (pDat->nb->verifyLinkIsNonCoherent(node, link, pDat->nb))
				processLink(node, link, pDat);
		}
	}
}

/***************************************************************************
 ***				Link Optimization			  ***
 ***************************************************************************/

/*----------------------------------------------------------------------------------------
 * void
 * regangLinks(sMainData *pDat)
 *
 *  Description:
 *	 Test the sublinks of a link to see if they qualify to be reganged.  If they do,
 *	 update the port list data to indicate that this should be done.  Note that no
 *	 actual hardware state is changed in this routine.
 *
 *  Parameters:
 *	@param[in,out] sMainData*  pDat = our global state
 * ---------------------------------------------------------------------------------------
 */
static void regangLinks(sMainData *pDat)
{
#ifndef HT_BUILD_NC_ONLY
	u8 i, j;
	for (i = 0; i < pDat->TotalLinks*2; i += 2)
	{
		ASSERT(pDat->PortList[i].Type < 2 && pDat->PortList[i].Link < pDat->nb->maxLinks);  /*  Data validation */
		ASSERT(pDat->PortList[i+1].Type < 2 && pDat->PortList[i+1].Link < pDat->nb->maxLinks); /*  data validation */
		ASSERT(!(pDat->PortList[i].Type == PORTLIST_TYPE_IO && pDat->PortList[i+1].Type == PORTLIST_TYPE_CPU));  /*  ensure src is closer to the bsp than dst */

		/* Regang is false unless we pass all conditions below */
		pDat->PortList[i].SelRegang = FALSE;
		pDat->PortList[i+1].SelRegang = FALSE;

		if ( (pDat->PortList[i].Type != PORTLIST_TYPE_CPU) || (pDat->PortList[i+1].Type != PORTLIST_TYPE_CPU))
			continue;   /*  Only process cpu to cpu links */

		for (j = i+2; j < pDat->TotalLinks*2; j += 2)
		{
			if ( (pDat->PortList[j].Type != PORTLIST_TYPE_CPU) || (pDat->PortList[j+1].Type != PORTLIST_TYPE_CPU) )
				continue;   /*  Only process cpu to cpu links */

			if (pDat->PortList[i].NodeID != pDat->PortList[j].NodeID)
				continue;   /*  Links must be from the same source */

			if (pDat->PortList[i+1].NodeID != pDat->PortList[j+1].NodeID)
				continue;   /*  Link must be to the same target */

			if ((pDat->PortList[i].Link & 3) != (pDat->PortList[j].Link & 3))
				continue;   /*  Ensure same source base port */

			if ((pDat->PortList[i+1].Link & 3) != (pDat->PortList[j+1].Link & 3))
				continue;   /*  Ensure same destination base port */

			if ((pDat->PortList[i].Link & 4) != (pDat->PortList[i+1].Link & 4))
				continue;   /*  Ensure sublink0 routes to sublink0 */

			ASSERT((pDat->PortList[j].Link & 4) == (pDat->PortList[j+1].Link & 4)); /*  (therefore sublink1 routes to sublink1) */

			if (pDat->HtBlock->AMD_CB_SkipRegang &&
				pDat->HtBlock->AMD_CB_SkipRegang(pDat->PortList[i].NodeID,
							pDat->PortList[i].Link & 0x03,
							pDat->PortList[i+1].NodeID,
							pDat->PortList[i+1].Link & 0x03))
			{
				continue;   /*  Skip regang */
			}


			pDat->PortList[i].Link &= 0x03; /*  Force to point to sublink0 */
			pDat->PortList[i+1].Link &= 0x03;
			pDat->PortList[i].SelRegang = TRUE; /*  Enable link reganging */
			pDat->PortList[i+1].SelRegang = TRUE;
			pDat->PortList[i].PrvWidthOutCap = HT_WIDTH_16_BITS;
			pDat->PortList[i+1].PrvWidthOutCap = HT_WIDTH_16_BITS;
			pDat->PortList[i].PrvWidthInCap = HT_WIDTH_16_BITS;
			pDat->PortList[i+1].PrvWidthInCap = HT_WIDTH_16_BITS;

			/*  Delete PortList[j, j+1], slow but easy to debug implementation */
			pDat->TotalLinks--;
			Amdmemcpy(&(pDat->PortList[j]), &(pDat->PortList[j+2]), sizeof(sPortDescriptor)*(pDat->TotalLinks*2-j));
			Amdmemset(&(pDat->PortList[pDat->TotalLinks*2]), INVALID_LINK, sizeof(sPortDescriptor)*2);

			/* //High performance, but would make debuging harder due to 'shuffling' of the records */
			/* //Amdmemcpy(PortList[TotalPorts-2], PortList[j], SIZEOF(sPortDescriptor)*2); */
			/* //TotalPorts -=2; */

			break; /*  Exit loop, advance to PortList[i+2] */
		}
	}
#endif /* HT_BUILD_NC_ONLY */
}

/*----------------------------------------------------------------------------------------
 * void
 * selectOptimalWidthAndFrequency(sMainData *pDat)
 *
 *  Description:
 *	 For all links:
 *	 Examine both sides of a link and determine the optimal frequency and width,
 *	 taking into account externally provided limits and enforcing any other limit
 *	 or matching rules as applicable except sublink balancing.   Update the port
 *	 list date with the optimal settings.
 *	 Note no hardware state changes in this routine.
 *
 *  Parameters:
 *	@param[in,out]  sMainData*  pDat = our global state, port list data
 * ---------------------------------------------------------------------------------------
 */
static void selectOptimalWidthAndFrequency(sMainData *pDat)
{
	u8 i, j;
	u32 temp;
	u16 cbPCBFreqLimit;
	u8 cbPCBABDownstreamWidth;
	u8 cbPCBBAUpstreamWidth;

	for (i = 0; i < pDat->TotalLinks*2; i += 2)
	{
#if CONFIG_EXPERT && CONFIG_LIMIT_HT_SPEED_200
		cbPCBFreqLimit = 0x0001;
#elif CONFIG_EXPERT && CONFIG_LIMIT_HT_SPEED_300
		cbPCBFreqLimit = 0x0003;
#elif CONFIG_EXPERT && CONFIG_LIMIT_HT_SPEED_400
		cbPCBFreqLimit = 0x0007;
#elif CONFIG_EXPERT && CONFIG_LIMIT_HT_SPEED_500
		cbPCBFreqLimit = 0x000F;
#elif CONFIG_EXPERT && CONFIG_LIMIT_HT_SPEED_600
		cbPCBFreqLimit = 0x001F;
#elif CONFIG_EXPERT && CONFIG_LIMIT_HT_SPEED_800
		cbPCBFreqLimit = 0x003F;
#elif CONFIG_EXPERT && CONFIG_LIMIT_HT_SPEED_1000
		cbPCBFreqLimit = 0x007F;
#elif CONFIG_EXPERT && CONFIG_LIMIT_HT_SPEED_1200
		cbPCBFreqLimit = 0x00FF;
#elif CONFIG_EXPERT && CONFIG_LIMIT_HT_SPEED_1400
		cbPCBFreqLimit = 0x01FF;
#elif CONFIG_EXPERT && CONFIG_LIMIT_HT_SPEED_1600
		cbPCBFreqLimit = 0x03FF;
#elif CONFIG_EXPERT && CONFIG_LIMIT_HT_SPEED_1800
		cbPCBFreqLimit = 0x07FF;
#elif CONFIG_EXPERT && CONFIG_LIMIT_HT_SPEED_2000
		cbPCBFreqLimit = 0x0FFF;
#elif CONFIG_EXPERT && CONFIG_LIMIT_HT_SPEED_2200
		cbPCBFreqLimit = 0x1FFF;
#elif CONFIG_EXPERT && CONFIG_LIMIT_HT_SPEED_2400
		cbPCBFreqLimit = 0x3FFF;
#elif CONFIG_EXPERT && CONFIG_LIMIT_HT_SPEED_2600
		cbPCBFreqLimit = 0x7FFF;
#else
		cbPCBFreqLimit = 0xFFFF;		// Maximum allowed by autoconfiguration
#endif

#if CONFIG_EXPERT && CONFIG_LIMIT_HT_DOWN_WIDTH_8
		cbPCBABDownstreamWidth = 8;
#else
		cbPCBABDownstreamWidth = 16;
#endif

#if CONFIG_EXPERT && CONFIG_LIMIT_HT_UP_WIDTH_8
		cbPCBBAUpstreamWidth = 8;
#else
		cbPCBBAUpstreamWidth = 16;
#endif

		if ( (pDat->PortList[i].Type == PORTLIST_TYPE_CPU) && (pDat->PortList[i+1].Type == PORTLIST_TYPE_CPU))
		{
			if (pDat->HtBlock->AMD_CB_Cpu2CpuPCBLimits)
			{
				pDat->HtBlock->AMD_CB_Cpu2CpuPCBLimits(
						pDat->PortList[i].NodeID,
						pDat->PortList[i].Link,
						pDat->PortList[i+1].NodeID,
						pDat->PortList[i+1].Link,
						&cbPCBABDownstreamWidth,
						&cbPCBBAUpstreamWidth, &cbPCBFreqLimit
						);
			}
		}
		else
		{
			if (pDat->HtBlock->AMD_CB_IOPCBLimits)
			{
				pDat->HtBlock->AMD_CB_IOPCBLimits(
						pDat->PortList[i+1].NodeID,
						pDat->PortList[i+1].HostLink,
						pDat->PortList[i+1].HostDepth,
						&cbPCBABDownstreamWidth,
						 &cbPCBBAUpstreamWidth, &cbPCBFreqLimit
						);
			}
		}


		temp = pDat->PortList[i].PrvFrequencyCap;
		temp &= pDat->PortList[i+1].PrvFrequencyCap;
		temp &= cbPCBFreqLimit;
		pDat->PortList[i].CompositeFrequencyCap = (u16)temp;
		pDat->PortList[i+1].CompositeFrequencyCap = (u16)temp;

		ASSERT (temp != 0);
		for (j = 15; ; j--)
		{
			if (temp & ((u32)1 << j))
				break;
		}

		pDat->PortList[i].SelFrequency = j;
		pDat->PortList[i+1].SelFrequency = j;

		temp = pDat->PortList[i].PrvWidthOutCap;
		if (pDat->PortList[i+1].PrvWidthInCap < temp)
			temp = pDat->PortList[i+1].PrvWidthInCap;
		if (cbPCBABDownstreamWidth < temp)
			temp = cbPCBABDownstreamWidth;
		pDat->PortList[i].SelWidthOut = (u8)temp;
		pDat->PortList[i+1].SelWidthIn = (u8)temp;

		temp = pDat->PortList[i].PrvWidthInCap;
		if (pDat->PortList[i+1].PrvWidthOutCap < temp)
			temp = pDat->PortList[i+1].PrvWidthOutCap;
		if (cbPCBBAUpstreamWidth < temp)
			temp = cbPCBBAUpstreamWidth;
		pDat->PortList[i].SelWidthIn = (u8)temp;
		pDat->PortList[i+1].SelWidthOut = (u8)temp;

	}
}

/*----------------------------------------------------------------------------------------
 * void
 * hammerSublinkFixup(sMainData *pDat)
 *
 *  Description:
 *	 Iterate through all links, checking the frequency of each sublink pair.  Make the
 *	 adjustment to the port list data so that the frequencies are at a valid ratio,
 *	 reducing frequency as needed to achieve this. (All links support the minimum 200 MHz
 *	 frequency.)  Repeat the above until no adjustments are needed.
 *	 Note no hardware state changes in this routine.
 *
 *  Parameters:
 *	@param[in,out] sMainData* pDat = our global state, link state and port list
 * ---------------------------------------------------------------------------------------
 */
static void hammerSublinkFixup(sMainData *pDat)
{
#ifndef HT_BUILD_NC_ONLY
	u8 i, j, k;
	BOOL changes, downgrade;

	u8 hiIndex;
	u8 hiFreq, loFreq;

	u32 temp;

	do
	{
		changes = FALSE;
		for (i = 0; i < pDat->TotalLinks*2; i++)
		{
			if (pDat->PortList[i].Type != PORTLIST_TYPE_CPU) /*  Must be a CPU link */
				continue;
			if (pDat->PortList[i].Link < 4) /*  Only look for for sublink1's */
				continue;

			for (j = 0; j < pDat->TotalLinks*2; j++)
			{
				/*  Step 1. Find the matching sublink0 */
				if (pDat->PortList[j].Type != PORTLIST_TYPE_CPU)
					continue;
				if (pDat->PortList[j].NodeID != pDat->PortList[i].NodeID)
					continue;
				if (pDat->PortList[j].Link != (pDat->PortList[i].Link & 0x03))
					continue;

				/*  Step 2. Check for an illegal frequency ratio */
				if (pDat->PortList[i].SelFrequency >= pDat->PortList[j].SelFrequency)
				{
					hiIndex = i;
					hiFreq = pDat->PortList[i].SelFrequency;
					loFreq = pDat->PortList[j].SelFrequency;
				}
				else
				{
					hiIndex = j;
					hiFreq = pDat->PortList[j].SelFrequency;
					loFreq = pDat->PortList[i].SelFrequency;
				}

				if (hiFreq == loFreq)
					break; /*  The frequencies are 1:1, no need to do anything */

				downgrade = FALSE;

				if (hiFreq == 13)
				{
					if ((loFreq != 7) &&  /* {13, 7} 2400MHz / 1200MHz 2:1 */
						(loFreq != 4) &&  /* {13, 4} 2400MHz /  600MHz 4:1 */
						(loFreq != 2) )   /* {13, 2} 2400MHz /  400MHz 6:1 */
						downgrade = TRUE;
				}
				else if (hiFreq == 11)
				{
					if ((loFreq != 6))    /* {11, 6} 2000MHz / 1000MHz 2:1 */
						downgrade = TRUE;
				}
				else if (hiFreq == 9)
				{
					if ((loFreq != 5) &&  /* { 9, 5} 1600MHz /  800MHz 2:1 */
						(loFreq != 2) &&  /* { 9, 2} 1600MHz /  400MHz 4:1 */
						(loFreq != 0) )   /* { 9, 0} 1600MHz /  200Mhz 8:1 */
						downgrade = TRUE;
				}
				else if (hiFreq == 7)
				{
					if ((loFreq != 4) &&  /* { 7, 4} 1200MHz /  600MHz 2:1 */
						(loFreq != 0) )   /* { 7, 0} 1200MHz /  200MHz 6:1 */
						downgrade = TRUE;
				}
				else if (hiFreq == 5)
				{
					if ((loFreq != 2) &&  /* { 5, 2}  800MHz /  400MHz 2:1 */
						(loFreq != 0) )   /* { 5, 0}  800MHz /  200MHz 4:1 */
						downgrade = TRUE;
				}
				else if (hiFreq == 2)
				{
					if ((loFreq != 0))    /* { 2, 0}  400MHz /  200MHz 2:1 */
						downgrade = TRUE;
				}
				else
				{
					downgrade = TRUE; /*  no legal ratios for hiFreq */
				}

				/*  Step 3. Downgrade the higher of the two frequencies, and set nochanges to FALSE */
				if (downgrade)
				{
					/*  Although the problem was with the port specified by hiIndex, we need to */
					/*  downgrade both ends of the link. */
					hiIndex = hiIndex & 0xFE; /*  Select the 'upstream' (i.e. even) port */

					temp = pDat->PortList[hiIndex].CompositeFrequencyCap;

					/*  Remove hiFreq from the list of valid frequencies */
					temp = temp & ~((uint32)1 << hiFreq);
					ASSERT (temp != 0);
					pDat->PortList[hiIndex].CompositeFrequencyCap = (uint16)temp;
					pDat->PortList[hiIndex+1].CompositeFrequencyCap = (uint16)temp;

					for (k = 15; ; k--)
					{
						if (temp & ((u32)1 << k))
							break;
					}

					pDat->PortList[hiIndex].SelFrequency = k;
					pDat->PortList[hiIndex+1].SelFrequency = k;

					changes = TRUE;
				}
			}
		}
	} while (changes); /*  Repeat until a valid configuration is reached */
#endif /* HT_BUILD_NC_ONLY */
}

/*----------------------------------------------------------------------------------------
 * void
 * linkOptimization(sMainData *pDat)
 *
 *  Description:
 *	 Based on link capabilities, apply optimization rules to come up with the real best
 *	 settings, including several external limit decision from call backs. This includes
 *	 handling of sublinks.	Finally, after the port list data is updated, set the hardware
 *	 state for all links.
 *
 *  Parameters:
 *	@param[in]  sMainData* pDat = our global state
 * ---------------------------------------------------------------------------------------
 */
static void linkOptimization(sMainData *pDat)
{
	pDat->nb->gatherLinkData(pDat, pDat->nb);
	regangLinks(pDat);
	selectOptimalWidthAndFrequency(pDat);
	hammerSublinkFixup(pDat);
	pDat->nb->setLinkData(pDat, pDat->nb);
}


/*----------------------------------------------------------------------------------------
 * void
 * trafficDistribution(sMainData *pDat)
 *
 *  Description:
 *	 In the case of a two node system with both sublinks used, enable the traffic
 *	 distribution feature.
 *
 *  Parameters:
 *	  @param[in]	    sMainData*	  pDat		 = our global state, port list data
 * ---------------------------------------------------------------------------------------
 */
static void trafficDistribution(sMainData *pDat)
{
#ifndef HT_BUILD_NC_ONLY
	u32 links01, links10;
	u8 linkCount;
	u8 i;

	/*  Traffic Distribution is only used when there are exactly two nodes in the system */
	if (pDat->NodesDiscovered+1 != 2)
		return;

	links01 = 0;
	links10 = 0;
	linkCount = 0;
	for (i = 0; i < pDat->TotalLinks*2; i += 2)
	{
		if ((pDat->PortList[i].Type == PORTLIST_TYPE_CPU) && (pDat->PortList[i+1].Type == PORTLIST_TYPE_CPU))
		{
			links01 |= (u32)1 << pDat->PortList[i].Link;
			links10 |= (u32)1 << pDat->PortList[i+1].Link;
			linkCount++;
		}
	}
	ASSERT(linkCount != 0);
	if (linkCount == 1)
		return; /*  Don't setup Traffic Distribution if only one link is being used */

	pDat->nb->writeTrafficDistribution(links01, links10, pDat->nb);
#endif /* HT_BUILD_NC_ONLY */
}

/*----------------------------------------------------------------------------------------
 * void
 * tuning(sMainData *pDat)
 *
 *  Description:
 *	 Handle system and performance tunings, such as traffic distribution, fifo and
 *	 buffer tuning, and special config tunings.
 *
 *  Parameters:
 *	@param[in] sMainData* pDat = our global state, port list data
 * ---------------------------------------------------------------------------------------
 */
static void tuning(sMainData *pDat)
{
	u8 i;

	/* See if traffic distribution can be done and do it if so
	 * or allow system specific customization
	 */
	if ((pDat->HtBlock->AMD_CB_CustomizeTrafficDistribution == NULL)
		|| !pDat->HtBlock->AMD_CB_CustomizeTrafficDistribution())
	{
		trafficDistribution(pDat);
	}

	/* For each node, invoke northbridge specific buffer tunings or
	 * system specific customizations.
	 */
	for (i=0; i < pDat->NodesDiscovered + 1; i++)
	{
		if ((pDat->HtBlock->AMD_CB_CustomizeBuffers == NULL)
		   || !pDat->HtBlock->AMD_CB_CustomizeBuffers(i))
		{
			pDat->nb->bufferOptimizations(i, pDat, pDat->nb);
		}
	}
}

/*----------------------------------------------------------------------------------------
 * BOOL
 * isSanityCheckOk()
 *
 *  Description:
 *	 Perform any general sanity checks which should prevent HT from running if they fail.
 *	 Currently only the "Must run on BSP only" check.
 *
 *  Parameters:
 *	@param[out] result BOOL  = true if check is ok, false if it failed
 * ---------------------------------------------------------------------------------------
 */
static BOOL isSanityCheckOk(void)
{
	uint64 qValue;

	AmdMSRRead(APIC_Base, &qValue);

	return ((qValue.lo & ((u32)1 << APIC_Base_BSP)) != 0);
}

/***************************************************************************
 ***				 HT Initialize				   ***
 ***************************************************************************/

/*----------------------------------------------------------------------------------------
 * void
 * htInitialize(AMD_HTBLOCK *pBlock)
 *
 *  Description:
 *	 This is the top level external interface for Hypertransport Initialization.
 *	 Create our initial internal state, initialize the coherent fabric,
 *	 initialize the non-coherent chains, and perform any required fabric tuning or
 *	 optimization.
 *
 *  Parameters:
 *	@param[in] AMD_HTBLOCK*  pBlock = Our Initial State including possible
 *				  topologies and routings, non coherent bus
 *				  assignment info, and actual
 *				  wrapper or OEM call back routines.
 * ---------------------------------------------------------------------------------------
 */
void amdHtInitialize(AMD_HTBLOCK *pBlock)
{
	sMainData pDat;
	cNorthBridge nb;

	if (isSanityCheckOk())
	{
		newNorthBridge(0, &nb);

		pDat.HtBlock = pBlock;
		pDat.nb = &nb;
		pDat.sysMpCap = nb.maxNodes;
		nb.isCapable(0, &pDat, pDat.nb);
		coherentInit(&pDat);

		pDat.AutoBusCurrent = pBlock->AutoBusStart;
		pDat.UsedCfgMapEntires = 0;
		ncInit(&pDat);
		linkOptimization(&pDat);
		tuning(&pDat);
	}
}
