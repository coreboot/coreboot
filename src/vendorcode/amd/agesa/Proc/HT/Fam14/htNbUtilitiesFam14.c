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
 * @e \$Revision: 34897 $   @e \$Date: 2010-07-14 10:07:10 +0800 (Wed, 14 Jul 2010) $
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
#include "htNbUtilitiesFam14.h"
#include "Filecode.h"
#define FILECODE PROC_HT_FAM14_HTNBUTILITIESFAM14_FILECODE
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
Fam14GetNumCoresOnNode (
  IN       UINT8       Node,
  IN       NORTHBRIDGE *Nb
  )
{
  UINT32 Cores;
  PCI_ADDR Reg;

  ASSERT ((Node < MAX_NODES));
  // Read CmpCap
  Reg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (Node),
                                 MakePciBusFromNode (Node),
                                 MakePciDeviceFromNode (Node),
                                 CPU_NB_FUNC_03,
                                 REG_NB_CAPABILITY_3XE8);

  LibAmdPciReadBits (Reg, 13, 12, &Cores, Nb->ConfigHandle);

  return (UINT8) (Cores + 1);
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
Fam14GetNodeCount (
  IN       NORTHBRIDGE *Nb
  )
{
  ASSERT (Nb != NULL);
  return (1);
}

AP_MAIL_INFO
Fam14RetrieveMailbox (
  IN       UINT8        Node,
  IN       NORTHBRIDGE *Nb
  )
{
  AP_MAIL_INFO  NodeApMailBox;
  ASSERT (Nb != NULL);
  NodeApMailBox.Info = 0;
  return NodeApMailBox;
}
