/**
 * @file
 *
 * Coherent Family 10h Routines.
 *
 * Coherent feature Northbridge implementation specific to Family 10h processors.
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
#include "htNbCoherentFam10.h"
#include "Filecode.h"
#define FILECODE PROC_HT_FAM10_HTNBCOHERENTFAM10_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Return whether the current configuration exceeds the capability.
 *
 * @HtNbMethod{::F_IS_EXCEEDED_CAPABLE}.
 *
 * Get Node capability and update the minimum supported system capability.
 *
 * @param[in]     Node     the Node
 * @param[in]     State    sysMpCap (updated) and NodesDiscovered
 * @param[in]     Nb       this northbridge
 *
 * @retval        TRUE     system is not capable of current config.
 * @retval        FALSE    system is capable of current config.
 */
BOOLEAN
Fam10IsExceededCapable (
  IN       UINT8       Node,
  IN       STATE_DATA  *State,
  IN       NORTHBRIDGE *Nb
  )
{
  UINT32 Temp;
  UINT8 MaxNodes;
  PCI_ADDR Reg;

  ASSERT (Node < MAX_NODES);

  Reg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (Node),
                                 MakePciBusFromNode (Node),
                                 MakePciDeviceFromNode (Node),
                                 CPU_NB_FUNC_03,
                                 REG_NB_CAPABILITY_3XE8);

  LibAmdPciReadBits (Reg, 18, 16, &Temp, Nb->ConfigHandle);

  if (Temp != 0) {
    MaxNodes = (UINT8) (1 << (~Temp & 0x3));  // That is, 1, 2, 4, or 8
  } else {
    MaxNodes = 8;
  }
  if (State->SysMpCap > MaxNodes) {
    State->SysMpCap = MaxNodes;
  }
  // Note since sysMpCap is one based and NodesDiscovered is zero based, equal returns true
  //
  return ((BOOLEAN) (State->SysMpCap <= State->NodesDiscovered));
}

/**
 * Stop a link, so that it is isolated from a connected device.
 *
 * @HtNbMethod{::F_STOP_LINK}.
 *
 * Use is for fatal incompatible configurations, or for user interface
 * request to power off a link (IgnoreLink, SkipRegang).
 * Set ConnDly to make the power effective at the warm reset.
 * Set XMT and RCV off.
 *
 * @param[in] Node   the node to stop a link on.
 * @param[in] Link   the link to stop.
 * @param[in] State  access to special routine for writing link control register
 * @param[in] Nb     this northbridge.
 */
VOID
Fam10StopLink (
  IN       UINT8       Node,
  IN       UINT8       Link,
  IN       STATE_DATA  *State,
  IN       NORTHBRIDGE *Nb
  )
{
  UINT32 Temp;
  PCI_ADDR Reg;

  // Set ConnDly
  Reg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (Node),
                                 MakePciBusFromNode (Node),
                                 MakePciDeviceFromNode (Node),
                                 CPU_HTNB_FUNC_00,
                                 REG_LINK_GLOBAL_EXT_CONTROL_0x16C);
  Temp = 1;
  LibAmdPciWriteBits (Reg, 8, 8, &Temp, Nb->ConfigHandle);
  // Set TransOff and EndOfChain
  Reg = Nb->MakeLinkBase (Node, Link, Nb);
  Reg.Address.Register += HTHOST_LINK_CONTROL_REG;
  Temp = 3;
  State->HtFeatures->SetHtControlRegisterBits (Reg, 7, 6, &Temp, State);
}
