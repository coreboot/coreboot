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

