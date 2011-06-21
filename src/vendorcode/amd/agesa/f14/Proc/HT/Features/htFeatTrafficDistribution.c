/* $NoKeywords:$ */
/**
 * @file
 *
 * Traffic Distribution Routines.
 *
 * Contains routines for traffic distribution
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  HyperTransport
 * @e \$Revision: 35978 $   @e \$Date: 2010-08-07 02:18:50 +0800 (Sat, 07 Aug 2010) $
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
#include "htNb.h"
#include "htNotify.h"
#include "htFeatTrafficDistribution.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_HT_FEATURES_HTFEATTRAFFICDISTRIBUTION_FILECODE
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

/*----------------------------------------------------------------------------------------*/
/**
 * Identify Links which can have traffic distribution.
 *
 * @HtFeatMethod{::F_TRAFFIC_DISTRIBUTION}
 *
 * If there are redundant links between any nodes, traffic distribution allows the
 * redundant links to be used to improve performance.
 *
 * There are two types of traffic distribution. Their use is mutually exclusive, both
 * can not be used at once.
 *
 * Coherent Traffic Distribution is for systems of exactly two nodes only.  All links must
 * be symmetrical (the same width).  As many links as are connected can be distributed over.
 *
 * Link Pair Traffic Distribution works with redundant pairs of links between any two nodes,
 * it does not matter how many nodes are in the system or how many have a redundant link pair.
 * A node can have redundant link pairs with more than one other node.
 * The link pair can be asymmetric, the largest link must be used as the master.  However,
 * between any pair of nodes there is only one pair of redundant links, and there is a limit
 * to the total number of pairs each node can have.  So not all links will necessarily be
 * made usable.
 *
 * @param[in]   State   port list data
 */
VOID
TrafficDistribution (
  IN       STATE_DATA *State
  )
{
  UINT32 Links01;
  UINT32 Links10;
  UINT8  LinkCount;
  UINT8  i;
  UINT8  LastLink;
  BOOLEAN IsAsymmetric;
  UINT8 RedundantLinkCount[MAX_NODES][MAX_NODES];
  UINT8 MasterLinkPort[MAX_NODES][MAX_NODES];
  UINT8 AlternateLinkPort[MAX_NODES][MAX_NODES];
  UINT8 NodeA;
  UINT8 NodeB;
  UINT8 PairCount;

  LastLink = 0xFF;
  IsAsymmetric = FALSE;

  // Traffic Distribution is only used when there are exactly two Nodes in the system
  // and when all the links are symmetric, same width.
  if ((State->NodesDiscovered + 1) == 2) {
    Links01 = 0;
    Links10 = 0;
    LinkCount = 0;
    for (i = 0; i < (State->TotalLinks * 2); i += 2) {
      if (((*State->PortList)[i].Type == PORTLIST_TYPE_CPU) &&
          ((*State->PortList)[i + 1].Type == PORTLIST_TYPE_CPU)) {
        if ((LastLink != 0xFF) &&
            ((*State->PortList)[i].SelWidthOut != (*State->PortList)[LastLink].SelWidthOut) &&
            ((*State->PortList)[i + 1].SelWidthOut != (*State->PortList)[LastLink + 1].SelWidthOut)) {
          IsAsymmetric = TRUE;
          break;
        }
        Links01 |= (UINT32)1 << (*State->PortList)[i].Link;
        Links10 |= (UINT32)1 << (*State->PortList)[i + 1].Link;
        LinkCount++;
        LastLink = i;
      }
    }
    ASSERT (LinkCount != 0);
    //  Don't setup Traffic Distribution if only one Link is being used or there were asymmetric widths
    if ((LinkCount != 1) && !IsAsymmetric) {
      IDS_HDT_CONSOLE (HT_TRACE, "Applying coherent traffic distribution.\n");
      State->Nb->WriteTrafficDistribution (Links01, Links10, State->Nb);
      // If we did Traffic Distribution, we must not do Link Pair, so get out of here.
      return;
    }
  }
  // Either there are more than two nodes, Asymmetric links, or no redundant links.
  // See if we can use Link Pair Traffic Distribution
  LibAmdMemFill (&RedundantLinkCount, 0, (MAX_NODES * MAX_NODES), State->ConfigHandle);
  for (i = 0; i < (State->TotalLinks * 2); i += 2) {
    if (((*State->PortList)[i].Type == PORTLIST_TYPE_CPU) &&
        ((*State->PortList)[i + 1].Type == PORTLIST_TYPE_CPU)) {
      NodeA = (*State->PortList)[i].NodeID;
      NodeB = (*State->PortList)[i + 1].NodeID;
      if (RedundantLinkCount[NodeA][NodeB] == 0) {
        // This is the first link connecting two nodes
        ASSERT (RedundantLinkCount[NodeB][NodeA] == 0);
        MasterLinkPort[NodeA][NodeB] = i;
        MasterLinkPort[NodeB][NodeA] = i + 1;
      } else {
        // This is a redundant link.  If it is larger than the current master link,
        // make it the new master link.
        //
        if (((*State->PortList)[MasterLinkPort[NodeA][NodeB]].SelWidthOut < (*State->PortList)[i].SelWidthOut) &&
            ((*State->PortList)[MasterLinkPort[NodeB][NodeA]].SelWidthOut < (*State->PortList)[i + 1].SelWidthOut)) {
          // Make the old master link the alternate, we don't need to check, it is bigger.
          AlternateLinkPort[NodeA][NodeB] = MasterLinkPort[NodeA][NodeB];
          AlternateLinkPort[NodeB][NodeA] = MasterLinkPort[NodeB][NodeA];
          MasterLinkPort[NodeA][NodeB] = i;
          MasterLinkPort[NodeB][NodeA] = i + 1;
        } else {
          // Since the new link isn't bigger than the Master, check if it is bigger than the alternate,
          // if we have an alternate. If we don't have an alternate yet, make this link the alternate.
          if (RedundantLinkCount[NodeA][NodeB] == 1) {
            AlternateLinkPort[NodeA][NodeB] = i;
            AlternateLinkPort[NodeB][NodeA] = i + 1;
          } else {
            if (((*State->PortList)[AlternateLinkPort[NodeA][NodeB]].SelWidthOut < (*State->PortList)[i].SelWidthOut) &&
                ((*State->PortList)[AlternateLinkPort[NodeB][NodeA]].SelWidthOut < (*State->PortList)[i + 1].SelWidthOut)) {
              // Warning: the alternate link is an unusable redundant link
              // Then make the new link the alternate link.
              NotifyWarningOptUnusedLinks (
                NodeA,
                (*State->PortList)[AlternateLinkPort[NodeA][NodeB]].Link,
                NodeB,
                (*State->PortList)[AlternateLinkPort[NodeB][NodeA]].Link,
                State
                );
              ASSERT (RedundantLinkCount[NodeB][NodeA] > 1);
              AlternateLinkPort[NodeA][NodeB] = i;
              AlternateLinkPort[NodeB][NodeA] = i + 1;
            } else {
              // Warning the current link is an unusable redundant link
              NotifyWarningOptUnusedLinks (NodeA, (*State->PortList)[i].Link, NodeB, (*State->PortList)[i].Link, State);
            }
          }
        }
      }
      RedundantLinkCount[NodeA][NodeB]++;
      RedundantLinkCount[NodeB][NodeA]++;
    }
  }
  // If we found any, now apply up to 4 per node
  for (NodeA = 0; NodeA < MAX_NODES; NodeA++) {
    PairCount = 0;
    for (NodeB = 0; NodeB < MAX_NODES; NodeB++) {
      if (RedundantLinkCount[NodeA][NodeB] > 1) {
        // Then there is a pair of links (at least, but we only care about the pair not the extras)
        if (PairCount < MAX_LINK_PAIRS) {
          // Program it
          if ((*State->PortList)[MasterLinkPort[NodeA][NodeB]].SelWidthOut
              != (*State->PortList)[AlternateLinkPort[NodeA][NodeB]].SelWidthOut) {
            IsAsymmetric = TRUE;
          } else {
            IsAsymmetric = FALSE;
          }
          State->Nb->WriteLinkPairDistribution (
            NodeA,
            NodeB,
            PairCount,
            IsAsymmetric,
            (*State->PortList)[MasterLinkPort[NodeA][NodeB]].Link,
            (*State->PortList)[AlternateLinkPort[NodeA][NodeB]].Link,
            State->Nb
            );
          PairCount++;
        } else {
          // Warning: More link pairs than can be distributed
          NotifyWarningOptLinkPairExceed (
            NodeA, NodeB,
            (*State->PortList)[MasterLinkPort[NodeA][NodeB]].Link,
            (*State->PortList)[AlternateLinkPort[NodeA][NodeB]].Link,
            State);
          // Disable the link pair from the other node, the analysis loop made sure there
          // can only be a single link pair between a pair of nodes.
          RedundantLinkCount[NodeB][NodeA] = 1;
        }
      }
    }
    IDS_HDT_CONSOLE (
      HT_TRACE,
      ((PairCount != 0) ?
       "Node %d applying %d link pair distributions.\n" :
       ""),
      NodeA,
      PairCount
      );
  }
}
