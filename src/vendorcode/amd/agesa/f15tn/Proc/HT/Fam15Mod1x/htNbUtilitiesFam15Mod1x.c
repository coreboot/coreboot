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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
*****************************************************************************
*
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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
#include "htNbUtilitiesFam15Mod1x.h"
#include "Filecode.h"
#define FILECODE PROC_HT_FAM15MOD1X_HTNBUTILITIESFAM15MOD1X_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------*/
/**
 * Write the total number of cores to the Node
 *
 * @HtNbMethod{::F_SET_TOTAL_NODES_AND_CORES}
 *
 * @param[in]     Node         the Node that will be examined
 * @param[in]     TotalNodes   the total number of Nodes
 * @param[in]     TotalCores   the total number of cores
 * @param[in]     Nb           this northbridge
 */
VOID
Fam15Mod1xSetTotalCores (
  IN       UINT8       Node,
  IN       UINT8       TotalNodes,
  IN       UINT8       TotalCores,
  IN       NORTHBRIDGE *Nb
  )
{
  PCI_ADDR NodeIDReg;
  UINT32 Temp;

  NodeIDReg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (Node),
                                       MakePciBusFromNode (Node),
                                       MakePciDeviceFromNode (Node),
                                       CPU_HTNB_FUNC_00,
                                       REG_NODE_ID_0X60);

  Temp = ((TotalCores - 1) & HTREG_NODE_CPUCNT_4_0);
  LibAmdPciWriteBits (NodeIDReg, 20, 16, &Temp, Nb->ConfigHandle);
}

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
Fam15Mod1xGetNumCoresOnNode (
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
                                 CPU_NB_FUNC_05,
                                 REG_NB_CAPABILITY_2_5X84);

  LibAmdPciReadBits (Reg, 7, 0, &Result, Nb->ConfigHandle);

  // Support Downcoring
  Cores = Result;
  Cores++;
  Reg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (Node),
                                 MakePciBusFromNode (Node),
                                 MakePciDeviceFromNode (Node),
                                 CPU_NB_FUNC_03,
                                 REG_NB_DOWNCORE_3X190);
  LibAmdPciReadBits (Reg, 31, 0, &Leveling, Nb->ConfigHandle);
  for (i = 0; i < Cores; i++) {
    if ((Leveling & ((UINT32) 1 << i)) != 0) {
      Result--;
    }
  }
  return (UINT8) (Result + 1);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get the Count (1 based) of Nodes in the system.
 *
 * @HtNbMethod{::F_GET_NODE_COUNT}
 *
 * This is intended to support AP Core HT init, since the Discovery State data is not
 * available (State->NodesDiscovered), there needs to be this way to find the number
 * of Nodes, which is just one.
 *
 * @param[in]     Nb           this northbridge
 *
 * @return        The number of nodes
 */
UINT8
Fam15Mod1xGetNodeCount (
  IN       NORTHBRIDGE *Nb
  )
{
  ASSERT (Nb != NULL);
  return (1);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get the enable compute unit status for this node.
 *
 * @HtNbMethod{::F_GET_ENABLED_COMPUTE_UNITS}
 *
 * @param[in]   Node    The node for which we want the enabled compute units.
 * @param[in]   Nb      Our Northbridge.
 *
 * @return      The Enabled Compute Unit value
 */
UINT8
Fam15Mod1xGetEnabledComputeUnits (
  IN       UINT8       Node,
  IN       NORTHBRIDGE *Nb
  )
{
  UINT32 Enabled;
  PCI_ADDR Reg;

  ASSERT ((Node < MAX_NODES));

  Reg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (Node),
                                 MakePciBusFromNode (Node),
                                 MakePciDeviceFromNode (Node),
                                 CPU_NB_FUNC_05,
                                 REG_NB_COMPUTE_UNIT_5X80);
  LibAmdPciReadBits (Reg, 1, 0, &Enabled, Nb->ConfigHandle);
  return ((UINT8) Enabled);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get the dual core compute unit status for this node.
 *
 * @HtNbMethod{::PF_GET_DUALCORE_COMPUTE_UNITS}
 *
 * @param[in]   Node    The node for which we want the dual core status
 * @param[in]   Nb      Our Northbridge.
 *
 * @return      The dual core compute unit status.
 */
UINT8
Fam15Mod1xGetDualcoreComputeUnits (
  IN       UINT8       Node,
  IN       NORTHBRIDGE *Nb
  )
{
  UINT32 Dual;
  PCI_ADDR Reg;

  ASSERT ((Node < MAX_NODES));

  Reg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (Node),
                                 MakePciBusFromNode (Node),
                                 MakePciDeviceFromNode (Node),
                                 CPU_NB_FUNC_05,
                                 REG_NB_COMPUTE_UNIT_5X80);
  LibAmdPciReadBits (Reg, 17, 16, &Dual, Nb->ConfigHandle);
  return ((UINT8) Dual);
}
