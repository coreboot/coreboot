/**
 * @file
 *
 * Link optimization support.
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
#include "IdsHt.h"
#include "htInterface.h"
#include "htInterfaceGeneral.h"
#include "htNotify.h"
#include "htNb.h"
#include "htNbHardwareFam10.h"
#include "htNbOptimization.h"
#include "Filecode.h"
#define FILECODE PROC_HT_NBCOMMON_HTNBOPTIMIZATION_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

/***************************************************************************
 ***                            Link Optimization                        ***
 ***************************************************************************/

/*----------------------------------------------------------------------------------------*/
/**
 * Get Link features into system data structure.
 *
 * @HtNbMethod{::F_GATHER_LINK_FEATURES}
 *
 * For a specific discovered CPU Link, populate the port list with the frequency
 * capabilities.  Support for other link oriented capabilities, currently:
 * - Unit ID Clumping. Set to disabled. This doesn't mean the CPU doesn't support clumping,
 *   it just means:
 *   - The CPU doesn't clump its host unit ids, and
 *   - We don't have to check as carefully in SetLinkData whether the port is an IO host link.
 *
 * @param[in,out] ThisPort       The PortList structure entry for this link's port
 * @param[in]     Interface      Access to non-HT support functions.
 * @param[in]     Nb             this northbridge
 */
VOID
GatherLinkFeatures (
  IN OUT   PORT_DESCRIPTOR  *ThisPort,
  IN       HT_INTERFACE     *Interface,
  IN       NORTHBRIDGE      *Nb
  )
{
  PCI_ADDR Reg;
  UINT32 Frequency;
  UINT32 ExtendedFrequency;

  Reg = ThisPort->Pointer;
  Reg.Address.Register += HTHOST_FREQ_REV_REG;
  LibAmdPciReadBits (Reg, 30, 16, &Frequency, Nb->ConfigHandle);
  Reg = ThisPort->Pointer;
  Reg.Address.Register += HTHOST_FREQ_EXTENSION;
  LibAmdPciReadBits (Reg, 15, 1, &ExtendedFrequency, Nb->ConfigHandle);
  ThisPort->PrvFrequencyCap = ((Frequency | (ExtendedFrequency << HT_FREQUENCY_2800M)) &
                               Nb->NorthBridgeFreqMask (ThisPort->NodeID, Interface, Nb));
  // Check for Internal link restriction not to run at 1000 MHz (but allow lower)
  if (IsPackageLinkInternal (Nb->GetPackageLink (ThisPort->NodeID, ThisPort->Link, Nb))) {
    ThisPort->PrvFrequencyCap &= ~(HT_FREQUENCY_LIMIT_1000M & ~HT_FREQUENCY_LIMIT_800M);
  }
  ThisPort->ClumpingSupport = HT_CLUMPING_DISABLE;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Change the hardware state for all Links according to the now optimized data in the
 * port list data structure for link reganging.
 *
 * @HtNbMethod{::F_SET_LINK_REGANG}
 *
 * @param[in]     Node   the node on which to regang a link
 * @param[in]     Link   the sublink 0 of the sublink pair to regang
 * @param[in]     Nb     this northbridge
 */
VOID
SetLinkRegang (
  IN       UINT8       Node,
  IN       UINT8       Link,
  IN       NORTHBRIDGE *Nb
  )
{
  PCI_ADDR Reg;
  UINT32 Temp;

  Temp = 1;
  Reg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (Node),
                                 MakePciBusFromNode (Node),
                                 MakePciDeviceFromNode (Node),
                                 CPU_HTNB_FUNC_00,
                                 REG_HT_LINK_EXT_CONTROL0_0X170 + (4 * Link));

  LibAmdPciWriteBits (Reg, 0, 0, &Temp, Nb->ConfigHandle);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Change the hardware state for all Links according to the now optimized data in the
 * port list data structure  for Unit Id Clumping.
 *
 * @HtNbMethod{::F_SET_LINK_UNITID_CLUMPING}
 *
 * This applies to the host root of a non-coherent chain.
 *
 * @param[in]     Node              the node on which to enable clumping
 * @param[in]     Link              the link for which to enable clumping
 * @param[in]     ClumpingEnables   the unit id clumping enables
 * @param[in]     Nb                this northbridge
 */
VOID
SetLinkUnitIdClumping (
  IN       UINT8       Node,
  IN       UINT8       Link,
  IN       UINT32      ClumpingEnables,
  IN       NORTHBRIDGE *Nb
  )
{
  PCI_ADDR Reg;

  // Host Unit Ids are not clumped.
  ASSERT ((ClumpingEnables & 0x3) == 0);

  Reg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (Node),
                                 MakePciBusFromNode (Node),
                                 MakePciDeviceFromNode (Node),
                                 CPU_HTNB_FUNC_00,
                                 REG_HT_LINK_CLUMPING0_0X110 + (4 * Link));

  LibAmdPciWriteBits (Reg, 31, 0, &ClumpingEnables, Nb->ConfigHandle);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Change the hardware state for all Links according to the now optimized data in the
 * port list data structure for link frequency.
 *
 * @HtNbMethod{::F_SET_LINK_FREQUENCY}
 *
 * Handle extended frequencies.  For HT3 frequencies, ensure Retry and Scrambling are
 * set. For HT1, clear them.
 *
 * @param[in]     Node        the node on which to set frequency for a link
 * @param[in]     Link        the link to set frequency
 * @param[in]     Frequency   the frequency to set
 * @param[in]     Nb          this northbridge
 */
VOID
SetLinkFrequency (
  IN       UINT8          Node,
  IN       UINT8          Link,
  IN       UINT8          Frequency,
  IN       NORTHBRIDGE    *Nb
  )
{
  UINT32 Temp;
  PCI_ADDR Reg;

  ASSERT ((Frequency >= HT_FREQUENCY_600M && Frequency <= HT_FREQUENCY_3200M)
          || (Frequency == HT_FREQUENCY_200M) || (Frequency == HT_FREQUENCY_400M));

  // Handle extended frequencies, 2800 MHz and above.  31 > Frequency > 16 in this case.
  if (Frequency > HT_FREQUENCY_2600M) {
    Temp = 1;
  } else {
    // Clear it if not extended.
    Temp = 0;
  }
  Reg = Nb->MakeLinkBase (Node, Link, Nb);
  Reg.Address.Register += HTHOST_FREQ_EXTENSION;
  LibAmdPciWriteBits (Reg, 0, 0, &Temp, Nb->ConfigHandle);
  Reg = Nb->MakeLinkBase (Node, Link, Nb);
  Reg.Address.Register += HTHOST_FREQ_REV_REG;
  Temp = (Frequency & 0x0F);
  LibAmdPciWriteBits (Reg, 11, 8, &Temp, Nb->ConfigHandle);
  //  Gen1 = 200Mhz -> 1000MHz, Gen3 = 1200MHz -> 2600MHz
  if (Frequency > HT_FREQUENCY_1000M) {
    // Enable  for Gen3 frequencies
    Temp = 1;
  } else {
    // Disable  for Gen1 frequencies
    Temp = 0;
  }
  // HT3 retry mode enable / disable
  Reg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (Node),
                                 MakePciBusFromNode (Node),
                                 MakePciDeviceFromNode (Node),
                                 CPU_HTNB_FUNC_00,
                                 REG_HT_LINK_RETRY0_0X130 + (4 * Link));
  LibAmdPciWriteBits (Reg, 0, 0, &Temp, Nb->ConfigHandle);
  // and Scrambling enable / disable
  Reg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (Node),
                                 MakePciBusFromNode (Node),
                                 MakePciDeviceFromNode (Node),
                                 CPU_HTNB_FUNC_00,
                                 REG_HT_LINK_EXT_CONTROL0_0X170 + (4 * Link));
  LibAmdPciWriteBits (Reg, 3, 3, &Temp, Nb->ConfigHandle);
}
