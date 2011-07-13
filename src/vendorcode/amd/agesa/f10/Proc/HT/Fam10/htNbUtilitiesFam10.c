/**
 * @file
 *
 * Northbridge utility routines.
 *
 * These routines are needed for support of more than one feature area.
 * Collect them in this file so build options don't remove them.
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
#include "htNbHardwareFam10.h"
#include "htNbUtilitiesFam10.h"
#include "Filecode.h"
#define FILECODE PROC_HT_FAM10_HTNBUTILITIESFAM10_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Return the number of cores (1 based count) on Node.
 *
 * @HtNbMethod{::F_GET_NUM_CORES_ON_NODE}
 *
 * @param[in]     Node   the Node that will be examined
 * @param[in]     Nb     this northbridge
 *
 * @return        the number of cores
 */
UINT8
Fam10GetNumCoresOnNode (
  IN       UINT8       Node,
  IN       NORTHBRIDGE *Nb
  )
{
  UINT32 Result;
  UINT32 Leveling;
  UINT32 Cores;
  UINT8 i;
  PCI_ADDR Reg;

  ASSERT ((Node < MAX_NODES));
  // Read CmpCap
  Reg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (Node),
                                 MakePciBusFromNode (Node),
                                 MakePciDeviceFromNode (Node),
                                 CPU_NB_FUNC_03,
                                 REG_NB_CAPABILITY_3XE8);

  LibAmdPciReadBits (Reg, 13, 12, &Cores, Nb->ConfigHandle);

  // Support Downcoring
  Result = Cores;
  Cores++;
  Reg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (Node),
                                 MakePciBusFromNode (Node),
                                 MakePciDeviceFromNode (Node),
                                 CPU_NB_FUNC_03,
                                 REG_NB_DOWNCORE_3X190);
  LibAmdPciReadBits (Reg, 3, 0, &Leveling, Nb->ConfigHandle);
  for (i = 0; i < Cores; i++) {
    if ((Leveling & ((UINT32) 1 << i)) != 0) {
      Result--;
    }
  }
  return (UINT8) (Result + 1);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Return the number of cores (1 based count) on Node.
 *
 * @HtNbMethod{::F_GET_NUM_CORES_ON_NODE}.
 *
 * @param[in]     Node   the Node that will be examined
 * @param[in]     Nb     this northbridge
 *
 * @return        the number of cores
 */
UINT8
Fam10RevDGetNumCoresOnNode (
  IN       UINT8       Node,
  IN       NORTHBRIDGE *Nb
  )
{
  UINT32 Result;
  UINT32 Leveling;
  UINT32 Cores;
  UINT32 Cores2;
  UINT8 i;
  PCI_ADDR Reg;

  ASSERT ((Node < MAX_NODES));
  // Read CmpCap
  Reg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (Node),
                                 MakePciBusFromNode (Node),
                                 MakePciDeviceFromNode (Node),
                                 CPU_NB_FUNC_03,
                                 REG_NB_CAPABILITY_3XE8);

  LibAmdPciReadBits (Reg, 13, 12, &Cores, Nb->ConfigHandle);
  LibAmdPciReadBits (Reg, 15, 15, &Cores2, Nb->ConfigHandle);
  Cores = Cores + (Cores2 << 2);

  // Support Downcoring
  Result = Cores;
  Cores++;
  Reg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (Node),
                                 MakePciBusFromNode (Node),
                                 MakePciDeviceFromNode (Node),
                                 CPU_NB_FUNC_03,
                                 REG_NB_DOWNCORE_3X190);
  LibAmdPciReadBits (Reg, 5, 0, &Leveling, Nb->ConfigHandle);
  for (i = 0; i < Cores; i++) {
    if ((Leveling & ((UINT32) 1 << i)) != 0) {
      Result--;
    }
  }
  return (UINT8) (Result + 1);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get the next link for iterating over the links on a node in the correct order.
 *
 * @HtNbMethod{::F_GET_NEXT_LINK}
 *
 * Family 10h specific implementation use the Internal Link field in
 * the northbridge to prioritize internal links in the order.
 *
 * @param[in]     Node The node on which to iterate links.
 * @param[in,out] Link IN: the current iteration context, OUT: the next link.
 * @param[in]     Nb   This Northbridge, access to config pointer.
 *
 * @retval LinkIteratorExternal    The current Link is an external link.
 * @retval LinkIteratorInternal    The current Link is an internal link.
 * @retval LinkIteratorEnd         There is no next link (Link is back to BEGIN).
 *
 */
LINK_ITERATOR_STATUS
Fam10GetNextLink (
  IN       UINT8       Node,
  IN OUT   UINT8       *Link,
  IN       NORTHBRIDGE *Nb
  )
{
  PCI_ADDR Reg;
  UINT32   InternalLinks;
  UINT32   ExternalLinks;
  UINT32   HigherLinks;
  BOOLEAN  IsInternalLink;
  LINK_ITERATOR_STATUS Status;

  ASSERT ((Node < MAX_NODES));
  ASSERT ((*Link < Nb->MaxLinks) || (*Link == LINK_ITERATOR_BEGIN));
  InternalLinks = 0;
  ExternalLinks = 0;

  // Read IntLnkRoute from the Link Initialization Status register.
  // (Note that this register field is not reserved prior to rev D, but should be zero.)
  Reg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (Node),
                                 MakePciBusFromNode (Node),
                                 MakePciDeviceFromNode (Node),
                                 CPU_HTNB_FUNC_00,
                                 REG_HT_LINK_INITIALIZATION_0X1A0);

  LibAmdPciReadBits (Reg, 23, 16, &InternalLinks, Nb->ConfigHandle);
  // The external links are all possible links which are not Internal
  ExternalLinks = (((1 << Nb->MaxLinks) - 1) ^ InternalLinks);
  // Can't have no possible links!
  ASSERT ((ExternalLinks != 0) || (InternalLinks != 0));


  if (*Link == LINK_ITERATOR_BEGIN) {
    // If the request is for the first link (BEGIN), get it
    if (InternalLinks != 0) {
      *Link = LibAmdBitScanForward (InternalLinks);
      Status = LinkIteratorInternal;
    } else {
      *Link = LibAmdBitScanForward (ExternalLinks);
      Status = LinkIteratorExternal;
    }
  } else {
    // If the iterator is not at the beginning, search for the next Link starting from the
    // current link.
    HigherLinks = InternalLinks & ~((1 << (*Link + 1)) - 1);
    IsInternalLink = (BOOLEAN) ((InternalLinks & (1 << *Link)) != 0);
    if (IsInternalLink && (HigherLinks != 0)) {
      // We are still on internal links and there are more to do.
      *Link = LibAmdBitScanForward (HigherLinks);
      Status = LinkIteratorInternal;
    } else {
      if (IsInternalLink) {
        // We are transitioning now from internal to external, so get the first external link
        HigherLinks = ExternalLinks;
      } else {
        // We are already iterating over external links, so get the next one
        HigherLinks = ExternalLinks & ~((1 << (*Link + 1)) - 1);
      }
      if (HigherLinks != 0) {
        *Link = LibAmdBitScanForward (HigherLinks);
        Status = LinkIteratorExternal;
      } else {
        // The end of all links
        *Link = LINK_ITERATOR_BEGIN;
        Status = LinkIteratorEnd;
      }
    }
  }
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get Info about Module Type of this northbridge
 *
 * @HtNbMethod{::F_GET_MODULE_INFO}
 *
 * Provide the Processor module type, single or multi, and the node's module id.
 *
 * @param[in]     Node                the Node
 * @param[out]    ModuleType          0 for Single, 1 for Multi
 * @param[out]    Module              The module number of this node (0 if Single)
 * @param[in]     Nb                  this northbridge
 *
 */
VOID
Fam10GetModuleInfo (
  IN       UINT8       Node,
     OUT   UINT8       *ModuleType,
     OUT   UINT8       *Module,
  IN       NORTHBRIDGE *Nb
  )
{
  PCI_ADDR Reg;
  UINT32 MultNodeCpu;
  UINT32 IntNodeNum;

  ASSERT (Node < MAX_NODES);

  Reg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (Node),
                                 MakePciBusFromNode (Node),
                                 MakePciDeviceFromNode (Node),
                                 CPU_NB_FUNC_03,
                                 REG_NB_CAPABILITY_3XE8);
  LibAmdPciReadBits (Reg, 29, 29, &MultNodeCpu, Nb->ConfigHandle);
  LibAmdPciReadBits (Reg, 31, 30, &IntNodeNum, Nb->ConfigHandle);

  *ModuleType = (UINT8) MultNodeCpu;
  *Module = (UINT8) IntNodeNum;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Implement the hardware method of doing Socket Naming, by accessing this northbridge's Socket Id register.
 *
 * @HtNbMethod{::F_GET_SOCKET}
 *
 * The hardware socket naming method is not available for Family 10h prior to rev D.
 *
 * @param[in]   Node    The node for which we want the socket id.
 * @param[in]   TempNode The temporary node id route where the node can be accessed.
 * @param[in]   Nb      Our Northbridge.
 *
 * @return      The Socket Id
 */
UINT8
Fam10GetSocket (
  IN       UINT8       Node,
  IN       UINT8       TempNode,
  IN       NORTHBRIDGE *Nb
  )
{
  ASSERT ((Node < MAX_NODES));
  ASSERT (TempNode < MAX_NODES);
  ASSERT (Nb != NULL);
  return (Node);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Implement the hardware method of doing Socket Naming, by accessing this northbridge's Socket Id register.
 *
 * @HtNbMethod{::F_GET_SOCKET}
 *
 * The Socket Id is strapped to the Sbi Control Register, F3X1E4[6:4]SbiAddr.
 *
 * @param[in]   Node    The node for which we want the socket id.
 * @param[in]   TempNode The temporary node id route where the node can be accessed.
 * @param[in]   Nb      Our Northbridge.
 *
 * @return      The Socket Id
 */
UINT8
Fam10RevDGetSocket (
  IN       UINT8       Node,
  IN       UINT8       TempNode,
  IN       NORTHBRIDGE *Nb
  )
{
  UINT32 Socket;
  PCI_ADDR Reg;

  ASSERT ((TempNode < MAX_NODES));
  ASSERT ((Node < MAX_NODES));
  // Read SbiAddr
  Reg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (TempNode),
                                 MakePciBusFromNode (TempNode),
                                 MakePciDeviceFromNode (TempNode),
                                 CPU_NB_FUNC_03,
                                 REG_NB_SBI_CONTROL_3X1E4);
  LibAmdPciReadBits (Reg, 6, 4, &Socket, Nb->ConfigHandle);
  return ((UINT8) Socket);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Post info to AP cores via a mailbox.
 *
 * @HtNbMethod{::F_POST_MAILBOX}
 *
 * Use the link MCA counter register as a PCI -> MSR mailbox, for info such as node id,
 * and module info.
 *
 * @param[in]     Node          the Node
 * @param[in]     ApMailboxes   The info to post
 * @param[in]     Nb            this northbridge
 *
 */
VOID
Fam10PostMailbox (
  IN       UINT8 Node,
  IN       AP_MAILBOXES ApMailboxes,
  IN       NORTHBRIDGE *Nb
  )
{
  PCI_ADDR Reg;

  ASSERT (Node < MAX_NODES);

  Reg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (Node),
                                 MakePciBusFromNode (Node),
                                 MakePciDeviceFromNode (Node),
                                 CPU_NB_FUNC_03,
                                 REG_NB_MCA_LINK_THRESHOLD_3X168);
  LibAmdPciWriteBits (Reg, 11, 0, &ApMailboxes.ApMailInfo.Info, Nb->ConfigHandle);
  Reg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (Node),
                                 MakePciBusFromNode (Node),
                                 MakePciDeviceFromNode (Node),
                                 CPU_NB_FUNC_03,
                                 REG_NB_MCA_L3_THRESHOLD_3X170);
  LibAmdPciWriteBits (Reg, 11, 0, &ApMailboxes.ApMailExtInfo.Info, Nb->ConfigHandle);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Retrieve info from a node's mailbox.
 *
 * @HtNbMethod{::F_RETRIEVE_MAILBOX}
 *
 * Use the link MCA counter register as a PCI -> MSR mailbox, for info such as node id,
 * and module info.
 *
 * @param[in]     Node          the Node
 * @param[in]     Nb            this northbridge
 *
 * @return        The ap mailbox info
 *
 */
AP_MAIL_INFO
Fam10RetrieveMailbox (
  IN       UINT8 Node,
  IN       NORTHBRIDGE *Nb
  )
{
  PCI_ADDR Reg;
  AP_MAIL_INFO ApMailInfo;

  ASSERT (Node < MAX_NODES);

  Reg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (Node),
                                 MakePciBusFromNode (Node),
                                 MakePciDeviceFromNode (Node),
                                 CPU_NB_FUNC_03,
                                 REG_NB_MCA_LINK_THRESHOLD_3X168);
  LibAmdPciReadBits (Reg, 11, 0, &ApMailInfo.Info, Nb->ConfigHandle);
  return ApMailInfo;
}
