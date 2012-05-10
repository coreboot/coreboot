/* $NoKeywords:$ */
/**
 * @file
 *
 * Coherent Feature Northbridge routines.
 *
 * Provide access to hardware for routing, coherent discovery.
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
#include "amdlib.h"
#include "Ids.h"
#include "Topology.h"
#include "htFeat.h"
#include "htNb.h"
#include "htNbCommonHardware.h"
#include "htNbCoherent.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_HT_NBCOMMON_HTNBCOHERENT_FILECODE
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

/***************************************************************************
 ***               FAMILY/NORTHBRIDGE SPECIFIC FUNCTIONS                 ***
 ***************************************************************************/

/*----------------------------------------------------------------------------------------*/
/**
 * Establish a Temporary route from one Node to another.
 *
 * @HtNbMethod{::F_WRITE_ROUTING_TABLE}
 *
 * This routine will modify the routing tables on the
 * SourceNode to cause it to route both request and response traffic to the
 * targetNode through the specified Link.
 *
 * @note: This routine is to be used for early discovery and initialization.  The
 * final routing tables must be loaded some other way because this
 * routine does not address the issue of probes, or independent request
 * response paths.
 *
 * @param[in]     Node     the Node that will have it's routing tables modified.
 * @param[in]     Target   For routing to Node target
 * @param[in]     Link     Link from Node to target
 * @param[in]     Nb       this northbridge
 */
VOID
WriteRoutingTable (
  IN       UINT8       Node,
  IN       UINT8       Target,
  IN       UINT8       Link,
  IN       NORTHBRIDGE *Nb
  )
{
  PCI_ADDR Reg;
  UINT32 Temp;

  ASSERT ((Node < MAX_NODES) && (Target < MAX_NODES) && (Link < Nb->MaxLinks));
  Temp = (Nb->SelfRouteResponseMask | Nb->SelfRouteRequestMask) << (Link + 1);
  Reg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (Node),
                                 MakePciBusFromNode (Node),
                                 MakePciDeviceFromNode (Node),
                                 CPU_HTNB_FUNC_00,
                                 REG_ROUTE0_0X40 + (Target * 4));
  LibAmdPciWrite (AccessWidth32, Reg, &Temp, Nb->ConfigHandle);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Modifies the NodeID register on the target Node
 *
 * @HtNbMethod{::F_WRITE_NODEID}
 *
 * @param[in]     Node   the Node that will have its NodeID altered.
 * @param[in]     NodeID the new value for NodeID
 * @param[in]     Nb     this northbridge
 */
VOID
WriteNodeID (
  IN       UINT8       Node,
  IN       UINT8       NodeID,
  IN       NORTHBRIDGE *Nb
  )
{
  PCI_ADDR Reg;
  UINT32 Temp;
  Temp = NodeID;
  ASSERT ((Node < MAX_NODES) && (NodeID < MAX_NODES));
  Reg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (Node),
                                 MakePciBusFromNode (Node),
                                 MakePciDeviceFromNode (Node),
                                 CPU_HTNB_FUNC_00,
                                 REG_NODE_ID_0X60);
  LibAmdPciWriteBits (Reg, 2, 0, &Temp, Nb->ConfigHandle);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Read the Default Link
 *
 * @HtNbMethod{::F_READ_DEFAULT_LINK}
 *
 * Read the DefLnk (the source Link of the current packet) from Node. Since this code
 * is running on the BSP, this should be the Link pointing back towards the BSP.
 *
 * @param[in]     Node   the Node that will have its NodeID altered.
 * @param[in]     Nb     this northbridge
 *
 * @return The HyperTransport Link where the request to
 *         read the default Link came from.
 */
UINT8
ReadDefaultLink (
  IN       UINT8       Node,
  IN       NORTHBRIDGE *Nb
  )
{
  UINT32 DefaultLink;
  PCI_ADDR Reg;
  UINT32 Temp;

  DefaultLink = 0;
  Reg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (Node),
                                 MakePciBusFromNode (Node),
                                 MakePciDeviceFromNode (Node),
                                 CPU_HTNB_FUNC_00,
                                 REG_LINK_INIT_CONTROL_0X6C);

  ASSERT ((Node < MAX_NODES));
  LibAmdPciReadBits (Reg, 3, 2, &DefaultLink, Nb->ConfigHandle);
  LibAmdPciReadBits (Reg, 8, 8, &Temp, Nb->ConfigHandle);
  DefaultLink |= (Temp << 2);
  return (UINT8)DefaultLink;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Turns routing tables on for a given Node
 *
 * @HtNbMethod{::F_ENABLE_ROUTING_TABLES}
 *
 * @param[in] Node the Node that will have it's routing tables enabled
 * @param[in] Nb   this northbridge
 */
VOID
EnableRoutingTables (
  IN       UINT8       Node,
  IN       NORTHBRIDGE *Nb
  )
{
  PCI_ADDR Reg;
  UINT32 Temp;
  Temp = 0;
  ASSERT ((Node < MAX_NODES));
  Reg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (Node),
                                 MakePciBusFromNode (Node),
                                 MakePciDeviceFromNode (Node),
                                 CPU_HTNB_FUNC_00,
                                 REG_LINK_INIT_CONTROL_0X6C);
  LibAmdPciWriteBits (Reg, 0, 0, &Temp, Nb->ConfigHandle);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Turns routing tables off for a given Node
 *
 * @HtNbMethod{::F_DISABLE_ROUTING_TABLES}
 *
 * @param[in] Node the Node that will have it's routing tables disabled
 * @param[in] Nb   this northbridge
 */
VOID
DisableRoutingTables (
  IN       UINT8       Node,
  IN       NORTHBRIDGE *Nb
  )
{
  PCI_ADDR Reg;
  UINT32 Temp;
  Temp = 1;
  ASSERT ((Node < MAX_NODES));
  Reg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (Node),
                                 MakePciBusFromNode (Node),
                                 MakePciDeviceFromNode (Node),
                                 CPU_HTNB_FUNC_00,
                                 REG_LINK_INIT_CONTROL_0X6C);
  LibAmdPciWriteBits (Reg, 0, 0, &Temp, Nb->ConfigHandle);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Verify that the Link is coherent, connected, and ready
 *
 * @HtNbMethod{::F_VERIFY_LINK_IS_COHERENT}
 *
 * @param[in]     Node    the Node that will be examined
 * @param[in]     Link    the Link on that Node to examine
 * @param[in]     Nb      this northbridge
 *
 * @retval        TRUE    The Link has the following status
 *                        -  LinkCon=1,           Link is connected
 *                        -  InitComplete=1,      Link initialization is complete
 *                        -  NC=0,                Link is coherent
 *                        -  UniP-cLDT=0,         Link is not Uniprocessor cLDT
 *                        -  LinkConPend=0        Link connection is not pending
 *  @retval       FALSE   The Link has some other status
*/
BOOLEAN
VerifyLinkIsCoherent (
  IN       UINT8       Node,
  IN       UINT8       Link,
  IN       NORTHBRIDGE *Nb
  )
{
  UINT32 LinkType;
  PCI_ADDR LinkBase;

  ASSERT ((Node < MAX_NODES) && (Link < Nb->MaxLinks));

  LinkBase = Nb->MakeLinkBase (Node, Link, Nb);

  //  FN0_98/A4/C4 = LDT Type Register
  LinkBase.Address.Register += HTHOST_LINK_TYPE_REG;
  LibAmdPciRead (AccessWidth32, LinkBase, &LinkType, Nb->ConfigHandle);

  //  Verify LinkCon = 1, InitComplete = 1, NC = 0, UniP-cLDT = 0, LinkConPend = 0
  return (BOOLEAN) ((LinkType & HTHOST_TYPE_MASK) ==  HTHOST_TYPE_COHERENT);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Read the token stored in the scratchpad register field.
 *
 * @HtNbMethod{::F_READ_TOKEN}
 *
 * Use the CPU core count as a scratch pad.
 *
 * @note The location used to store the token is arbitrary.  The only requirement is
 * that the location warm resets to zero, and that using it will have no ill-effects
 * during HyperTransport initialization.
 *
 * @param[in]     Node    the Node that will be examined
 * @param[in]     Nb      this northbridge
 *
 * @return        the Token read from the Node
 */
UINT8
ReadToken (
  IN       UINT8       Node,
  IN       NORTHBRIDGE *Nb
  )
{
  UINT32 Temp;
  PCI_ADDR Reg;

  ASSERT ((Node < MAX_NODES));
  // Use CpuCnt as a scratch register
  Reg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (Node),
                                 MakePciBusFromNode (Node),
                                 MakePciDeviceFromNode (Node),
                                 CPU_HTNB_FUNC_00,
                                 REG_NODE_ID_0X60);
  LibAmdPciReadBits (Reg, 19, 16, &Temp, Nb->ConfigHandle);

  return (UINT8)Temp;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Write the token stored in the scratchpad register
 *
 * @HtNbMethod{::F_WRITE_TOKEN}
 *
 * Use the CPU core count as a scratch pad.
 *
 * @note The location used to store the token is arbitrary.  The only requirement is
 * that the location warm resets to zero, and that using it will have no ill-effects
 * during HyperTransport initialization.
 *
 * @param[in]     Node  the Node that marked with token
 * @param[in]     Value the token Value
 * @param[in]     Nb    this northbridge
 */
VOID
WriteToken (
  IN       UINT8       Node,
  IN       UINT8       Value,
  IN       NORTHBRIDGE *Nb
  )
{
  PCI_ADDR Reg;
  UINT32 Temp;
  Temp = Value;
  ASSERT ((Node < MAX_NODES));
  // Use CpuCnt as a scratch register
  Reg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (Node),
                                 MakePciBusFromNode (Node),
                                 MakePciDeviceFromNode (Node),
                                 CPU_HTNB_FUNC_00,
                                 REG_NODE_ID_0X60);
  LibAmdPciWriteBits (Reg, 19, 16, &Temp, Nb->ConfigHandle);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Full Routing Table Register initialization
 *
 * @HtNbMethod{::F_WRITE_FULL_ROUTING_TABLE}
 *
 * Write the routing table entry for Node to target, using the request Link, response
 * Link, and broadcast Links provided.
 *
 * @param[in]     Node            the Node that will be examined
 * @param[in]     Target          the Target Node for these routes
 * @param[in]     ReqLink         the Link for requests to Target
 * @param[in]     RspLink         the Link for responses to Target
 * @param[in]     BroadcastLinks  the broadcast Links
 * @param[in]     Nb              this northbridge
 */
VOID
WriteFullRoutingTable (
  IN       UINT8       Node,
  IN       UINT8       Target,
  IN       UINT8       ReqLink,
  IN       UINT8       RspLink,
  IN       UINT32      BroadcastLinks,
  IN       NORTHBRIDGE *Nb
  )
{
  PCI_ADDR Reg;
  UINT32 Value;

  Value = 0;
  ASSERT ((Node < MAX_NODES) && (Target < MAX_NODES));
  if (ReqLink == ROUTE_TO_SELF) {
    Value |= Nb->SelfRouteRequestMask;
  } else {
    Value |= Nb->SelfRouteRequestMask << (ReqLink + 1);
  }

  if (RspLink == ROUTE_TO_SELF) {
    Value |= Nb->SelfRouteResponseMask;
  } else {
    Value |= Nb->SelfRouteResponseMask << (RspLink + 1);
  }

  // Allow us to accept a Broadcast ourselves, then set broadcasts for routes
  Value |= (UINT32)1 << Nb->BroadcastSelfBit;
  Value |= (UINT32)BroadcastLinks << (Nb->BroadcastSelfBit + 1);

  Reg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (Node),
                                 MakePciBusFromNode (Node),
                                 MakePciDeviceFromNode (Node),
                                 CPU_HTNB_FUNC_00,
                                 REG_ROUTE0_0X40 + (Target * 4));
  LibAmdPciWrite (AccessWidth32, Reg, &Value, Nb->ConfigHandle);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Determine whether a Node is compatible with the discovered configuration so far.
 *
 * @HtNbMethod{::F_IS_ILLEGAL_TYPE_MIX}.
 *
 * Currently, that means the family, extended family of the new Node are the
 * same as the BSP's.
 *
 * @param[in] Node the Node
 * @param[in] Nb   this northbridge
 *
 * @retval   TRUE    the new node is not compatible
 * @retval   FALSE   the new node is compatible
 */
BOOLEAN
IsIllegalTypeMix (
  IN       UINT8       Node,
  IN       NORTHBRIDGE *Nb
  )
{
  return ((BOOLEAN) ((Nb->MakeKey (Node, Nb) & Nb->CompatibleKey) == 0));
}

/*----------------------------------------------------------------------------------------*/
/**
 * Fix (hopefully) exceptional conditions.
 *
 * @HtNbMethod{::F_HANDLE_SPECIAL_NODE_CASE}.
 *
 * Currently, this routine is implemented for all coherent HT families to check
 * vendor ID of coherent Node. If the vendor ID is 0x1022 then return FALSE,
 * or return TRUE.
 *
 * @param[in] Node  The Node which need to be checked.
 * @param[in] Link  The link to check for special conditions.
 * @param[in] State our global state.
 * @param[in] Nb    this northbridge.
 *
 * @retval    TRUE  This node received special handling.
 * @retval    FALSE This node was not handled specially, handle it normally.
 *
 */
BOOLEAN
HandleSpecialNodeCase (
  IN       UINT8       Node,
  IN       UINT8       Link,
  IN       STATE_DATA  *State,
  IN       NORTHBRIDGE *Nb
  )
{
  BOOLEAN Result;
  PCI_ADDR Reg;
  UINT32 VendorID;

  Result = TRUE;

  Reg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (Node),
                                 MakePciBusFromNode (Node),
                                 MakePciDeviceFromNode (Node),
                                 0,
                                 0);

  LibAmdPciReadBits (Reg, 15, 0, &VendorID, Nb->ConfigHandle);
  if (VendorID == 0x1022) {
    Result = FALSE;
  }

  return Result;
}
