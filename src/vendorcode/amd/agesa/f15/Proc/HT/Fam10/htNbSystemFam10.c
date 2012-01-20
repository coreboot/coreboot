/* $NoKeywords:$ */
/**
 * @file
 *
 * System Tuning Family 10h specific routines
 *
 * Support for Traffic Distribution and buffer tunings which
 * can not be done in a register table.
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
#include "amdlib.h"
#include "Ids.h"
#include "Topology.h"
#include "htFeat.h"
#include "htInterface.h"
#include "htNb.h"
#include "htNbCommonHardware.h"
#include "htNbSystemFam10.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_HT_FAM10_HTNBSYSTEMFAM10_FILECODE
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
/**
 * Register Fields for an individual link pair.
 */
typedef struct {
  UINT32 Enable:1;            ///< Enable distribution on this pair.
  UINT32 Asymmetric:1;        ///< Links are different widths.
  UINT32 MasterSelect:3;      ///< The master link.
  UINT32 AlternateSelect:3;   ///< The alternate link.
} PAIR_SELECT_FIELDS;

/**
 * Register access union for ::PAIR_SELECT_FIELDS.
 */
typedef union {
  UINT32 Value;               ///< access as a 32 bit value or register.
  PAIR_SELECT_FIELDS Fields;  ///< access individual fields.
} PAIR_SELECT;

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/***************************************************************************
 ***               FAMILY/NORTHBRIDGE SPECIFIC FUNCTIONS                 ***
 ***************************************************************************/

/*----------------------------------------------------------------------------------------*/
/**
 * Set the traffic distribution register for the Links provided.
 *
 * @HtNbMethod{::F_WRITE_TRAFFIC_DISTRIBUTION}
 *
 * @param[in]     Links01   coherent Links from Node 0 to 1
 * @param[in]     Links10   coherent Links from Node 1 to 0
 * @param[in]     Nb        this northbridge
 */
VOID
Fam10WriteTrafficDistribution (
  IN       UINT32       Links01,
  IN       UINT32       Links10,
  IN       NORTHBRIDGE  *Nb
  )
{
  UINT32 Temp;
  PCI_ADDR TrafficDistReg;

  TrafficDistReg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (0),
                                            MakePciBusFromNode (0),
                                            MakePciDeviceFromNode (0),
                                            CPU_HTNB_FUNC_00,
                                            REG_HT_TRAFFIC_DIST_0X164);

  // Node 0
  // DstLnk
  LibAmdPciWriteBits (TrafficDistReg, 23, 16, &Links01, Nb->ConfigHandle);
  // DstNode = 1, cHTPrbDistEn = 1, cHTRspDistEn = 1, cHTReqDistEn = 1
  Temp = 0x0107;
  LibAmdPciWriteBits (TrafficDistReg, 15, 0, &Temp, Nb->ConfigHandle);

  TrafficDistReg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (1),
                                            MakePciBusFromNode (1),
                                            MakePciDeviceFromNode (1),
                                            CPU_HTNB_FUNC_00,
                                            REG_HT_TRAFFIC_DIST_0X164);

  // Node 1
  // DstLnk
  LibAmdPciWriteBits (TrafficDistReg, 23, 16, &Links10, Nb->ConfigHandle);
  // DstNode = 0, cHTPrbDistEn = 1, cHTRspDistEn = 1, cHTReqDistEn = 1
  Temp = 0x0007;
  LibAmdPciWriteBits (TrafficDistReg, 15, 0, &Temp, Nb->ConfigHandle);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Write a link pair to the link pair distribution and fixups.
 *
 * @HtNbMethod{::F_WRITE_LINK_PAIR_DISTRIBUTION}
 *
 * Set the links as a pair using the link pair index provided.  Set asymmetric attribute as
 * provided. If the Master Link is not currently used as the route, fixup the routes for all
 * nodes which specify the alternate link.
 *
 * @param[in]    Node           Set the pair on this node
 * @param[in]    ConnectedNode  The Node to which this link pair directly connects.
 * @param[in]    Pair           Using this pair set in the register
 * @param[in]    Asymmetric     True if different widths
 * @param[in]    MasterLink     Set this as the master link and in the route
 * @param[in]    AlternateLink  Set this as the alternate link
 * @param[in]    Nb             this northbridge
 *
 */
VOID
Fam10WriteLinkPairDistribution (
  IN       UINT8        Node,
  IN       UINT8        ConnectedNode,
  IN       UINT8        Pair,
  IN       BOOLEAN      Asymmetric,
  IN       UINT8        MasterLink,
  IN       UINT8        AlternateLink,
  IN       NORTHBRIDGE  *Nb
  )
{
  PCI_ADDR Reg;
  UINT32 CurrentRoute;
  UINT32 MasterRoute;
  UINT32 AlternateRoute;
  PAIR_SELECT Selection;
  UINT32 RouteIndex;

  ASSERT ((Node < MAX_NODES) && (ConnectedNode < MAX_NODES));
  ASSERT (Pair < MAX_LINK_PAIRS);
  ASSERT (MasterLink < Nb->MaxLinks);
  ASSERT (AlternateLink < Nb->MaxLinks);

  // Make the master link the route for all routes to or through NodeB, by replacing all occurrences of
  // Alternate link with Master link.  If routing used the master link, no update is necessary.
  MasterRoute = (((1 << Nb->BroadcastSelfBit) | Nb->SelfRouteResponseMask | Nb->SelfRouteRequestMask) << (MasterLink + 1));
  AlternateRoute = (((1 << Nb->BroadcastSelfBit) | Nb->SelfRouteResponseMask | Nb->SelfRouteRequestMask) << (AlternateLink + 1));
  Reg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (Node),
                                 MakePciBusFromNode (Node),
                                 MakePciDeviceFromNode (Node),
                                 CPU_HTNB_FUNC_00,
                                 REG_ROUTE0_0X40);
  for (RouteIndex = 0; RouteIndex < MAX_NODES; RouteIndex++) {
    Reg.Address.Register = REG_ROUTE0_0X40 + (RouteIndex * 4);
    LibAmdPciReadBits (Reg, 31, 0, &CurrentRoute, Nb->ConfigHandle);
    if ((CurrentRoute & AlternateRoute) != 0) {
      // Since Master and Alternate are redundant, the route must use one or the other but not both.
      ASSERT ((CurrentRoute & MasterRoute) == 0);
      // Set the master route for Request, Response or Broadcast only if the alternate was used for that case.
      // Example, use of a link as a broadcast link is typically not the same route register as its use for Request, Response.
      CurrentRoute = ((CurrentRoute & ~AlternateRoute) |
                      ((((CurrentRoute & AlternateRoute) >> (AlternateLink + 1)) << (MasterLink + 1)) & MasterRoute));
      LibAmdPciWriteBits (Reg, 31, 0, &CurrentRoute, Nb->ConfigHandle);
    }
  }

  // Set the Link Pair and Enable it
  Selection.Fields.Enable = 1;
  Selection.Fields.Asymmetric = Asymmetric;
  Selection.Fields.MasterSelect = MasterLink;
  Selection.Fields.AlternateSelect = AlternateLink;
  Reg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (Node),
                                 MakePciBusFromNode (Node),
                                 MakePciDeviceFromNode (Node),
                                 CPU_HTNB_FUNC_00,
                                 REG_HT_LINK_PAIR_DIST_0X1E0);
  LibAmdPciWriteBits (
    Reg,
    ((PAIR_SELECT_OFFSET * (Pair + 1)) - 1),
    (PAIR_SELECT_OFFSET * Pair),
    &Selection.Value,
    Nb->ConfigHandle
    );
}

/*----------------------------------------------------------------------------------------*/
/**
 * Family 10h specific tunings.
 *
 * @HtNbMethod{::F_BUFFER_OPTIMIZATIONS}
 *
 * Buffer tunings are inherently northbridge specific. Check for specific configs
 * which require adjustments and apply any standard workarounds to this Node.
 *
 * @param[in]     Node   the Node to tune
 * @param[in]     State  global state
 * @param[in]     Nb     this northbridge
 */
VOID
Fam10BufferOptimizations (
  IN       UINT8       Node,
  IN       STATE_DATA  *State,
  IN       NORTHBRIDGE *Nb
  )
{
  UINT32 Temp;
  PCI_ADDR currentPtr;
  PCI_ADDR GangedReg;
  UINT8 i;

  ASSERT (Node < MAX_NODES);

  //
  // Link to XCS Token Count Tuning
  //
  // For each active Link that we reganged (so this unfortunately can't go into the PCI reg
  // table), we have to switch the Link to XCS Token Counts to the ganged state.
  // We do this here for the non - uma case, which is to write the values that would have
  // been power on defaults if the Link was ganged at cold reset.
  //
  for (i = 0; i < (State->TotalLinks * 2); i++) {
    if (((*State->PortList)[i].NodeID == Node) && ((*State->PortList)[i].Type == PORTLIST_TYPE_CPU)) {
      // If the Link is greater than 4, this is a subLink 1, so it is not reganged.
      if ((*State->PortList)[i].Link < 4) {
        currentPtr.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (Node),
                                              MakePciBusFromNode (Node),
                                              MakePciDeviceFromNode (Node),
                                              CPU_NB_FUNC_03,
                                              REG_NB_LINK_XCS_TOKEN0_3X148 + (4 * (*State->PortList)[i].Link)
          );
        if ((*State->PortList)[i].SelRegang) {
          // Handle all the regang Token count adjustments

          // SubLink 0: [Probe0tok] = 2 [Rsp0tok] = 2 [PReq0tok] = 2 [Req0tok] = 2
          Temp = 0xAA;
          LibAmdPciWriteBits (currentPtr, 7, 0, &Temp, Nb->ConfigHandle);
          // SubLink 1: [Probe1tok] = 0 [Rsp1tok] = 0 [PReq1tok] = 0 [Req1tok] = 0
          Temp = 0;
          LibAmdPciWriteBits (currentPtr, 23, 16, &Temp, Nb->ConfigHandle);
          // [FreeTok] = 3
          Temp = 3;
          LibAmdPciWriteBits (currentPtr, 15, 14, &Temp, Nb->ConfigHandle);

        } else {
          // Read the regang bit in hardware
          GangedReg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode ((*State->PortList)[i].NodeID),
                                               MakePciBusFromNode ((*State->PortList)[i].NodeID),
                                               MakePciDeviceFromNode ((*State->PortList)[i].NodeID),
                                               CPU_HTNB_FUNC_00,
                                               REG_HT_LINK_EXT_CONTROL0_0X170 + (4 * (*State->PortList)[i].Link));
          LibAmdPciReadBits (GangedReg, 0, 0, &Temp, Nb->ConfigHandle);
          if (Temp == 1) {
            // handle a minor adjustment for strapped ganged Links.  If SelRegang is false we
            // didn't do the regang, so if the bit is on then it's hardware strapped.
            //

            // [FreeTok] = 3
            Temp = 3;
            LibAmdPciWriteBits (currentPtr, 15, 14, &Temp, Nb->ConfigHandle);
          }
        }
      }
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Family 10h specific tunings.
 *
 * @HtNbMethod{::F_BUFFER_OPTIMIZATIONS}
 *
 * Buffer tunings are inherently northbridge specific. Check for specific configs
 * which require adjustments and apply any standard workarounds to this Node.
 *
 * @param[in]     Node   the Node to tune
 * @param[in]     State  global state
 * @param[in]     Nb     this northbridge
 */
VOID
Fam10RevDBufferOptimizations (
  IN       UINT8       Node,
  IN       STATE_DATA  *State,
  IN       NORTHBRIDGE *Nb
  )
{
  UINT32 Temp;
  PCI_ADDR Reg;
  UINT8 i;
  FINAL_LINK_STATE FinalLinkState;
  UINT32 WidthIn;
  UINT32 WidthOut;

  ASSERT (Node < MAX_NODES);

  //
  // Internal link fixup.
  // When powering off internal link 2, a performance optimization may be possible where its buffers
  // can be made available to the external paired sublink.  If the conditions are met, do the fix up here.
  //
  for (i = 0; i < (State->TotalLinks * 2); i++) {
    if (((*State->PortList)[i].NodeID == Node) && ((*State->PortList)[i].Type == PORTLIST_TYPE_CPU)) {
      // Is this a sublink 0 paired with internal link 2?
      if (((*State->PortList)[i].Link < 4) &&
          (Nb->GetPackageLink (Node, ((*State->PortList)[i].Link + 4), Nb) == HT_LIST_MATCH_INTERNAL_LINK_2)) {
        FinalLinkState = State->HtInterface->GetIgnoreLink (Node, ((*State->PortList)[i].Link + 4), Nb->DefaultIgnoreLinkList, State);
        // Are we ignoring the internal link 2 with Power Off?
        if (FinalLinkState == POWERED_OFF) {
          // Read the regang bit in hardware.
          Reg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (Node),
                                         MakePciBusFromNode (Node),
                                         MakePciDeviceFromNode (Node),
                                         CPU_HTNB_FUNC_00,
                                         REG_HT_LINK_EXT_CONTROL0_0X170 + (4 * (*State->PortList)[i].Link));
          LibAmdPciReadBits (Reg, 0, 0, &Temp, Nb->ConfigHandle);
          // If it's already ganged, skip to the width fix up.
          if (Temp == 0) {
            // Clear EndOfChain / XmitOff on internal sublink
            Reg = Nb->MakeLinkBase (Node, ((*State->PortList)[i].Link + 4), Nb);
            Reg.Address.Register += HTHOST_LINK_CONTROL_REG;
            Temp = 0;
            State->HtFeatures->SetHtControlRegisterBits (Reg, 7, 6, &Temp, State);

            // Gang the link
            Nb->SetLinkRegang (Node, (*State->PortList)[i].Link, Nb);
          }

          // Set InLnSt = PHY_OFF in register table.
          // Set sublink 0 widths to 8 bits
          if ((*State->PortList)[i].SelWidthOut > 8) {
            (*State->PortList)[i].SelWidthOut = 8;
          }
          if ((*State->PortList)[i].SelWidthIn > 8) {
            (*State->PortList)[i].SelWidthIn = 8;
          }
          WidthOut = State->HtFeatures->ConvertWidthToBits ((*State->PortList)[i].SelWidthOut);
          WidthIn = State->HtFeatures->ConvertWidthToBits ((*State->PortList)[i].SelWidthIn);
          Temp = (WidthIn & 7) | ((WidthOut & 7) << 4);
          Reg = Nb->MakeLinkBase (Node, (*State->PortList)[i].Link, Nb);
          Reg.Address.Register += HTHOST_LINK_CONTROL_REG;
          State->HtFeatures->SetHtControlRegisterBits (Reg, 31, 24, &Temp, State);
        }
      }
    }
  }
}

