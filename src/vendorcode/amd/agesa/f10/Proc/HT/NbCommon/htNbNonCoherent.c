/**
 * @file
 *
 *  Northbridge generic non-coherent support routines.
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
#include "htNbNonCoherent.h"
#include "Filecode.h"
#define FILECODE PROC_HT_NBCOMMON_HTNBNONCOHERENT_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

/***************************************************************************
 ***                       Non-coherent init code                        ***
 ***                    Northbridge access routines                      ***
 ***************************************************************************/

/*----------------------------------------------------------------------------------------*/
/**
 * Return the Link to the Southbridge
 *
 * @HtNbMethod{::F_READ_SB_LINK}
 *
 * @param[in] Nb this northbridge
 *
 * @return the Link to the southbridge
 */
UINT8
ReadSouthbridgeLink (
  IN       NORTHBRIDGE *Nb
  )
{
  UINT32 Temp;
  PCI_ADDR Reg;
  Reg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (0),
                                 MakePciBusFromNode (0),
                                 MakePciDeviceFromNode (0),
                                 CPU_HTNB_FUNC_00,
                                 REG_UNIT_ID_0X64);
  LibAmdPciReadBits (Reg, 10, 8, &Temp, Nb->ConfigHandle);
  return (UINT8)Temp;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Verify that the Link is non-coherent, connected, and ready
 *
 * @HtNbMethod{::F_VERIFY_LINK_IS_NON_COHERENT}
 *
 * @param[in]     Node    the Node that will be examined
 * @param[in]     Link    the Link on that Node to examine
 * @param[in]     Nb      this northbridge
 *
 * @retval        TRUE    The Link has the following status
 *                            - LinkCon=1,        Link is connected
 *                            - InitComplete=1,   Link initialization is complete
 *                            - NC=1,             Link is noncoherent
 *                            - UniP-cLDT=0,      Link is not Uniprocessor cLDT
 *                            - LinkConPend=0     Link connection is not pending
 *  @retval       FALSE   The Link has some other status
 */
BOOLEAN
VerifyLinkIsNonCoherent (
  IN       UINT8       Node,
  IN       UINT8       Link,
  IN       NORTHBRIDGE *Nb
  )
{
  UINT32 LinkType;
  PCI_ADDR LinkBase;

  ASSERT ((Node < MAX_NODES) && (Link < MAX_NODES));

  LinkBase = Nb->MakeLinkBase (Node, Link, Nb);
  LinkBase.Address.Register += HTHOST_LINK_TYPE_REG;

  // FN0_98/A4/C4 = LDT Type Register
  LibAmdPciRead (AccessWidth32, LinkBase, &LinkType, Nb->ConfigHandle);

  // Verify LinkCon = 1, InitComplete = 1, NC = 1, UniP-cLDT = 0, LinkConPend = 0
  return (BOOLEAN) ((LinkType & HTHOST_TYPE_MASK) ==  HTHOST_TYPE_NONCOHERENT);
}
