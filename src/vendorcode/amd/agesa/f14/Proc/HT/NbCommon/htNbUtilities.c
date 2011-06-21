/* $NoKeywords:$ */
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
 * @e \$Revision: 35136 $   @e \$Date: 2010-07-16 11:29:48 +0800 (Fri, 16 Jul 2010) $
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
#include "htNotify.h"
#include "htNb.h"
#include "htNbHardwareFam10.h"
#include "htNbUtilities.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_HT_NBCOMMON_HTNBUTILITIES_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Return the HT Host capability base PCI config address for a Link.
 *
 * @HtNbMethod{::F_MAKE_LINK_BASE}
 *
 * @param[in]     Node    the Node this Link is on
 * @param[in]     Link    the Link
 * @param[in]     Nb      this northbridge
 *
 * @return  the pci config address
 */
PCI_ADDR
MakeLinkBase (
  IN       UINT8       Node,
  IN       UINT8       Link,
  IN       NORTHBRIDGE *Nb
  )
{
  PCI_ADDR LinkBase;

  ASSERT (Nb != NULL);
  if (Link < 4) {
    LinkBase.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (Node),
                                        MakePciBusFromNode (Node),
                                        MakePciDeviceFromNode (Node),
                                        CPU_HTNB_FUNC_00,
                                        REG_HT_CAP_BASE_0X80 + Link*HT_HOST_CAP_SIZE);
  } else {
    LinkBase.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (Node),
                                        MakePciBusFromNode (Node),
                                        MakePciDeviceFromNode (Node),
                                        CPU_HTNB_FUNC_04,
                                        REG_HT_CAP_BASE_0X80 + (Link - 4)*HT_HOST_CAP_SIZE);
  }
  return LinkBase;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Return the LinkFailed status AFTER an attempt is made to clear the bit.
 *
 * @HtNbMethod{::F_READ_TRUE_LINK_FAIL_STATUS}
 *
 * Dependency!:  HT_FEATURES::SetHtControlRegisterBits
 *
 * Also, call event notify if a Hardware Fault caused a sync flood on a previous boot.
 *
 * The table below summarizes correct responses of this routine.
 * <TABLE>
 *   <TR><TD> Family </TD> <TD> before </TD> <TD> after </TD> <TD> unconnected </TD> <TD> Notify? </TD> <TD> return </TD></TR>
 *   <TR><TD>  10    </TD> <TD>   0    </TD> <TD>   0   </TD> <TD>     0       </TD> <TD>   No    </TD> <TD> FALSE  </TD></TR>
 *   <TR><TD>  10    </TD> <TD>   1    </TD> <TD>   0   </TD> <TD>     0       </TD> <TD>   Yes   </TD> <TD> FALSE  </TD></TR>
 *   <TR><TD>  10    </TD> <TD>   1    </TD> <TD>   0   </TD> <TD>     3       </TD> <TD>   No    </TD> <TD> TRUE   </TD></TR>
 * </TABLE>
 *
 * @param[in]     Node   the Node that will be examined
 * @param[in]     Link   the Link on that Node to examine
 * @param[in]     State  access to call back routine
 * @param[in]     Nb     this northbridge
 *
 * @retval        TRUE   the Link is not connected or has hard error
 * @retval        FALSE  the Link is connected
 */
BOOLEAN
ReadTrueLinkFailStatus (
  IN       UINT8       Node,
  IN       UINT8       Link,
  IN       STATE_DATA  *State,
  IN       NORTHBRIDGE *Nb
  )
{
  UINT32    Before;
  UINT32    After;
  UINT32    Unconnected;
  UINT32    Crc;
  PCI_ADDR  Reg;

  ASSERT ((Node < MAX_NODES) && (Link < Nb->MaxLinks));

  Reg = Nb->MakeLinkBase (Node, Link, Nb);
  Reg.Address.Register +=  HTHOST_LINK_CONTROL_REG;

  // Save the CRC status before doing anything else.
  // Read, Clear, re-read the error bits in the Link Control Register
  // (FN0_84/A4/C4[4] = LinkFail bit),
  // and check the connection status, TransOff and EndOfChain.
  //
  LibAmdPciReadBits (Reg, 9, 8, &Crc, Nb->ConfigHandle);
  LibAmdPciReadBits (Reg, 4, 4, &Before, Nb->ConfigHandle);
  State->HtFeatures->SetHtControlRegisterBits (Reg, 4, 4, &Before, State);
  LibAmdPciReadBits (Reg, 4, 4, &After, Nb->ConfigHandle);
  LibAmdPciReadBits (Reg, 7, 6, &Unconnected, Nb->ConfigHandle);

  if (Before != After) {
    if (Unconnected == 0) {
      if (Crc != 0) {
        // A sync flood occurred due to HT CRC
        // Pass the Node and Link on which the generic sync flood event occurred.
        NotifyAlertHwHtCrc (Node, Link, (UINT8)Crc, State);
      } else {
        // Some sync flood occurred
        // Pass the Node and Link on which the generic sync flood event occurred.
        NotifyAlertHwSyncFlood (Node, Link, State);
      }
    }
  }
  return (BOOLEAN) ((After != 0) || (Unconnected != 0));
}

/*----------------------------------------------------------------------------------------*/
/**
 * Write the total number of cores and Nodes to the Node
 *
 * @HtNbMethod{::F_SET_TOTAL_NODES_AND_CORES}
 *
 * @param[in]     Node         the Node that will be examined
 * @param[in]     TotalNodes   the total number of Nodes
 * @param[in]     TotalCores   the total number of cores
 * @param[in]     Nb           this northbridge
 */
VOID
SetTotalNodesAndCores (
  IN       UINT8       Node,
  IN       UINT8       TotalNodes,
  IN       UINT8       TotalCores,
  IN       NORTHBRIDGE *Nb
  )
{
  PCI_ADDR NodeIDReg;
  UINT32 Temp;

  ASSERT ((Node < MAX_NODES) && (TotalNodes <= MAX_NODES));
  NodeIDReg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (Node),
                                       MakePciBusFromNode (Node),
                                       MakePciDeviceFromNode (Node),
                                       CPU_HTNB_FUNC_00,
                                       REG_NODE_ID_0X60);

  Temp = ((TotalCores - 1) & HTREG_NODE_CPUCNT_4_0);
  LibAmdPciWriteBits (NodeIDReg, 20, 16, &Temp, Nb->ConfigHandle);
  Temp = TotalNodes - 1;
  LibAmdPciWriteBits (NodeIDReg,  6,  4, &Temp, Nb->ConfigHandle);

  NodeIDReg.Address.Register = REG_HT_EXTENDED_NODE_ID_F0X160;

  Temp = (((TotalCores - 1) & HTREG_EXTNODE_CPUCNT_7_5) >> 5);
  LibAmdPciWriteBits (NodeIDReg, 18, 16, &Temp, Nb->ConfigHandle);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get the Count (1 based) of Nodes in the system.
 *
 * @HtNbMethod{::F_GET_NODE_COUNT}
 *
 * This is intended to support AP Core HT init, since the Discovery State data is not
 * available (State->NodesDiscovered), there needs to be this way to find the number
 * of Nodes.  The Node count can be read from the BSP.
 *
 * @param[in]     Nb           this northbridge
 *
 * @return        The number of nodes
 */
UINT8
GetNodeCount (
  IN       NORTHBRIDGE *Nb
  )
{
  PCI_ADDR NodeIDReg;
  UINT32 Temp;

  NodeIDReg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (0),
                                       MakePciBusFromNode (0),
                                       MakePciDeviceFromNode (0),
                                       CPU_HTNB_FUNC_00,
                                       REG_NODE_ID_0X60);
  LibAmdPciReadBits (NodeIDReg,  6,  4, &Temp, Nb->ConfigHandle);
  return ((UINT8) (++Temp));
}

/*----------------------------------------------------------------------------------------*/
/**
 * Limit coherent config accesses to cpus as indicated by Nodecnt.
 *
 * @HtNbMethod{::F_LIMIT_NODES}
 *
 * @param[in]     Node    the Node that will be examined
 * @param[in]     Nb      this northbridge
 */
VOID
LimitNodes (
  IN       UINT8       Node,
  IN       NORTHBRIDGE *Nb
  )
{
  UINT32 Temp;
  PCI_ADDR Reg;

  Temp = 1;
  ASSERT ((Node < MAX_NODES));
  Reg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (Node),
                                 MakePciBusFromNode (Node),
                                 MakePciDeviceFromNode (Node),
                                 CPU_HTNB_FUNC_00,
                                 REG_LINK_TRANS_CONTROL_0X68);
  LibAmdPciWriteBits (Reg, 15, 15, &Temp, Nb->ConfigHandle);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get the Package Link number, given the node and real link number.
 *
 * @HtNbMethod{::F_GET_PACKAGE_LINK}
 *
 * Based on the link to package link mapping from BKDG, look up package link for
 * the input link on the internal node number corresponding to Node id.
 *
 * @param[in]   Node       the node which has this link
 * @param[in]   Link       the link on that node
 * @param[in]   Nb         this northbridge
 *
 * @return      the Package Link, HT_LIST_TERMINAL Not connected in package, HT_LIST_MATCH_INTERNAL_LINK package internal link.
 *
 */
UINT8
GetPackageLink (
  IN       UINT8       Node,
  IN       UINT8       Link,
  IN       NORTHBRIDGE *Nb
  )
{
  UINT8 ModuleType;
  UINT8 Module;
  UINTN PackageLinkMapItem;
  UINT8 PackageLink;

  ASSERT ((Node < MAX_NODES) && (Link < Nb->MaxLinks));
  PackageLink = HT_LIST_TERMINAL;

  Nb->GetModuleInfo (Node, &ModuleType, &Module, Nb);

  if (ModuleType != 0) {
    ASSERT (Nb->PackageLinkMap != NULL);
    // Use table to find this module's package link
    PackageLinkMapItem = 0;
    while ((*Nb->PackageLinkMap)[PackageLinkMapItem].Link != HT_LIST_TERMINAL) {
      if (((*Nb->PackageLinkMap)[PackageLinkMapItem].Module == Module) &&
          ((*Nb->PackageLinkMap)[PackageLinkMapItem].Link == Link)) {
        PackageLink = (*Nb->PackageLinkMap)[PackageLinkMapItem].PackageLink;
        break;
      }
      PackageLinkMapItem++;
    }
  } else {
    PackageLink = Link;
  }
  return PackageLink;
}
