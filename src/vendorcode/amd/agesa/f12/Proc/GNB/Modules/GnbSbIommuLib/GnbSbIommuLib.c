/* $NoKeywords:$ */
/**
 * @file
 *
 * SB services
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 39931 $   @e \$Date: 2010-10-16 18:19:16 -0700 (Sat, 16 Oct 2010) $
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "AGESA.h"
#include  "Ids.h"
#include  "amdlib.h"
#include  "Gnb.h"
#include  "GnbCommonLib.h"
#include  "GnbIvrsLib.h"
#include  "GnbSbLib.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBSBIOMMULIB_GNBSBIOMMULIB_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */



/*----------------------------------------------------------------------------------------*/
/**
 * Create IVHD entry
 *
 *
 * @param[in]  Ivhd            IVHD header pointer
 * @param[in]  StdHeader       Standard configuration header
 *
 */
VOID
SbCreateIvhdEntries (
     OUT   IVRS_IVHD_ENTRY            *Ivhd,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  )
{
  PCI_ADDR  Start;
  PCI_ADDR  End;
  PCI_ADDR  PciAddress;
  UINT32    Value;
  IDS_HDT_CONSOLE (GNB_TRACE, "SbCreateIvhdEntries Entry\n");
  PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x14, 4, 0);
// P2P alias entry
  GnbLibPciRead (PciAddress.AddressValue | 0x18, AccessWidth32, &Value, StdHeader);
  Start.AddressValue = MAKE_SBDFO (0, (Value >> 8) & 0xff, 0, 0, 0);
  End.AddressValue = MAKE_SBDFO (0, (Value >> 16) & 0xff, 0x1f, 0x7, 0);
  GnbIvhdAddDeviceAliasRangeEntry (Start, End, PciAddress, 0, Ivhd, StdHeader);
  PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x14, 0, 0);
// HPET
  GnbIvhdAddSpecialDeviceEntry (IvhdSpecialDeviceHpet, PciAddress, 0, 0, Ivhd, StdHeader);
// APIC
  GnbIvhdAddSpecialDeviceEntry (
    IvhdSpecialDeviceIoapic,
    PciAddress,
    GnbLiGetIoapicId (SbGetSbIoApicBaseAddress (StdHeader), StdHeader),
    0xD7,
    Ivhd,
    StdHeader
    );
  IDS_HDT_CONSOLE (GNB_TRACE, "SbCreateIvhdEntries Exit\n");
}
