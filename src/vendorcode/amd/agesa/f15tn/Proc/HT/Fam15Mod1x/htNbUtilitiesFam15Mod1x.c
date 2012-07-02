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
