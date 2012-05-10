/**
 * @file
 *
 * Northbridge non-coherent support for Family 10h processors.
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
#include "htNbNonCoherentFam10.h"
#include "Filecode.h"
#define FILECODE PROC_HT_FAM10_HTNBNONCOHERENTFAM10_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Enable config access to a non-coherent chain for the given bus range.
 *
 * @HtNbMethod{::F_SET_CONFIG_ADDR_MAP}
 *
 * @param[in]     ConfigMapIndex the map entry to set
 * @param[in]     SecBus         The secondary bus number to use
 * @param[in]     SubBus         The subordinate bus number to use
 * @param[in]     TargetNode     The Node  that shall be the recipient of the traffic
 * @param[in]     TargetLink     The Link that shall be the recipient of the traffic
 * @param[in]     State          our global state
 * @param[in]     Nb             this northbridge
 */
VOID
Fam10SetConfigAddrMap (
  IN       UINT8       ConfigMapIndex,
  IN       UINT8       SecBus,
  IN       UINT8       SubBus,
  IN       UINT8       TargetNode,
  IN       UINT8       TargetLink,
  IN       STATE_DATA  *State,
  IN       NORTHBRIDGE *Nb
  )
{
  UINT8 CurNode;
  PCI_ADDR Reg;
  UINT32 Temp;

  Reg = Nb->MakeLinkBase (TargetNode, TargetLink, Nb);

  ASSERT (SecBus <= SubBus);
  ASSERT (TargetNode <= State->NodesDiscovered);
  ASSERT (TargetLink < Nb->MaxLinks);
  Temp = SecBus;
  Reg.Address.Register += HTHOST_ISOC_REG;
  LibAmdPciWriteBits (Reg, 15, 8, &Temp, Nb->ConfigHandle);

  Temp = ((UINT32)SubBus << 24) + ((UINT32)SecBus << 16) + ((UINT32)TargetLink << 8) +
    ((UINT32)TargetNode << 4) + (UINT32)3;
  for (CurNode = 0; CurNode < (State->NodesDiscovered + 1); CurNode++) {
    Reg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (CurNode),
                                   MakePciBusFromNode (CurNode),
                                   MakePciDeviceFromNode (CurNode),
                                   CPU_ADDR_FUNC_01,
                                   REG_ADDR_CONFIG_MAP0_1XE0 + (4 * ConfigMapIndex));
    LibAmdPciWrite (AccessWidth32, Reg, &Temp, Nb->ConfigHandle);
  }
}
