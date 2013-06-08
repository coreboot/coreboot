/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe late post initialization.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 64352 $   @e \$Date: 2012-01-19 03:54:04 -0600 (Thu, 19 Jan 2012) $
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
/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "AGESA.h"
#include  "Ids.h"
#include  "amdlib.h"
#include  "cpuLateInit.h"
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "GnbIommu.h"
#include  "GnbIvrsLib.h"
#include  "GnbSbIommuLib.h"
#include  "GnbCommonLib.h"
#include  "GnbNbInitLibV4.h"
#include  "GnbIommuIvrs.h"
#include  "GnbRegisterAccTN.h"
#include  "GnbRegistersTN.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBINITTN_GNBIOMMUIVRSTN_FILECODE
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

VOID
GnbCreateIvhdHeaderTN (
  IN       IVRS_BLOCK_TYPE            Type,
     OUT   IVRS_IVHD_ENTRY            *Ivhd,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  );

VOID
GnbCreateIvhdTN (
     OUT   IVRS_IVHD_ENTRY            *Ivhd,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  );

VOID
GnbCreateIvhdrTN (
     OUT   IVRS_IVHD_ENTRY            *Ivhd,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  );

AGESA_STATUS
GnbCreateIvrsEntryTN (
  IN       GNB_HANDLE                 *GnbHandle,
  IN       IVRS_BLOCK_TYPE            Type,
  IN       VOID                       *Ivrs,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  );

BOOLEAN
GnbCheckIommuPresentTN (
  IN       GNB_HANDLE                 *GnbHandle,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Check if IOMMU unit present and enabled
 *
 *
 *
 *
 * @param[in]  GnbHandle       Gnb handle
 * @param[in]  StdHeader       Standard configuration header
 *
 */
BOOLEAN
GnbCheckIommuPresentTN (
  IN       GNB_HANDLE                 *GnbHandle,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  )
{
  if (GnbLibPciIsDevicePresent (MAKE_SBDFO (0, 0, 0, 2, 0), StdHeader)) {
    return TRUE;
  }
  return FALSE;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Create IVRS entry
 *
 *
 * @param[in]  GnbHandle       Gnb handle
 * @param[in]  Type            Entry type
 * @param[in]  Ivrs            IVRS table pointer
 * @param[in]  StdHeader       Standard configuration header
 *
 */

AGESA_STATUS
GnbCreateIvrsEntryTN (
  IN       GNB_HANDLE                 *GnbHandle,
  IN       IVRS_BLOCK_TYPE            Type,
  IN       VOID                       *Ivrs,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  )
{
  IVRS_IVHD_ENTRY                   *Ivhd;
  UINT8                             IommuCapabilityOffset;
  UINT32                            Value;

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbFmCreateIvrsEntry Entry\n");
  if (Type == IvrsIvhdBlock || Type == IvrsIvhdrBlock) {
    // Update IVINFO
    IommuCapabilityOffset = GnbLibFindPciCapability (MAKE_SBDFO (0, 0, 0, 2, 0), IOMMU_CAP_ID, StdHeader);
    GnbLibPciRead (MAKE_SBDFO (0, 0, 0, 2, IommuCapabilityOffset + 0x10), AccessWidth32, &Value, StdHeader);
    ((IOMMU_IVRS_HEADER *) Ivrs)->IvInfo = Value & (IVINFO_HTATSRESV_MASK | IVINFO_VASIZE_MASK | IVINFO_GASIZE_MASK | IVINFO_PASIZE_MASK);

    // Address of IVHD entry
    Ivhd = (IVRS_IVHD_ENTRY*) ((UINT8 *)Ivrs + ((IOMMU_IVRS_HEADER *) Ivrs)->TableLength);
    GnbCreateIvhdHeaderTN (Type, Ivhd, StdHeader);
    if (Type == IvrsIvhdBlock) {
      GnbCreateIvhdTN (Ivhd, StdHeader);
    } else {
      GnbCreateIvhdrTN (Ivhd, StdHeader);
    }
    ((IOMMU_IVRS_HEADER *) Ivrs)->TableLength = ((IOMMU_IVRS_HEADER *) Ivrs)->TableLength + Ivhd->Length;
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "GnbFmCreateIvrsEntry Exit\n");
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Create IVRS entry
 *
 *
 * @param[in]  Type            Block type
 * @param[in]  Ivhd            IVHD header pointer
 * @param[in]  StdHeader       Standard configuration header
 *
 */
VOID
GnbCreateIvhdHeaderTN (
  IN       IVRS_BLOCK_TYPE            Type,
     OUT   IVRS_IVHD_ENTRY            *Ivhd,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  )
{
  UINT32    Value;
  Ivhd->Type = (UINT8) Type;
  Ivhd->Flags = IVHD_FLAG_COHERENT | IVHD_FLAG_IOTLBSUP | IVHD_FLAG_ISOC | IVHD_FLAG_RESPASSPW | IVHD_FLAG_PASSPW | IVHD_FLAG_PPRSUB | IVHD_FLAG_PREFSUP;
  Ivhd->Length = sizeof (IVRS_IVHD_ENTRY);
  Ivhd->DeviceId = 0x2;
  Ivhd->CapabilityOffset = GnbLibFindPciCapability (MAKE_SBDFO (0, 0, 0, 2, 0), IOMMU_CAP_ID, StdHeader);
  Ivhd->PciSegment = 0;
  GnbLibPciRead (MAKE_SBDFO (0, 0, 0, 2, Ivhd->CapabilityOffset + 0x4), AccessWidth32, &Ivhd->BaseAddress, StdHeader);
  GnbLibPciRead (MAKE_SBDFO (0, 0, 0, 2, Ivhd->CapabilityOffset + 0x8), AccessWidth32, (UINT8 *) &Ivhd->BaseAddress + 4, StdHeader);
  Ivhd->BaseAddress = Ivhd->BaseAddress & 0xfffffffffffffffe;
  ASSERT (Ivhd->BaseAddress != 0x0);
  GnbLibPciRead (MAKE_SBDFO (0, 0, 0, 2, Ivhd->CapabilityOffset + 0x10), AccessWidth32, &Value, StdHeader);
  Ivhd->IommuInfo = (UINT16) (Value & 0x1f) | (0x13 << IVHD_INFO_UNITID_OFFSET);
  Ivhd->IommuEfr = (0 << IVHD_EFR_XTSUP_OFFSET) | (0 << IVHD_EFR_NXSUP_OFFSET) | (1 << IVHD_EFR_GTSUP_OFFSET) |
                   (0 << IVHD_EFR_GLXSUP_OFFSET) | (1 << IVHD_EFR_IASUP_OFFSET) | (0 << IVHD_EFR_GASUP_OFFSET) |
                   (0 << IVHD_EFR_HESUP_OFFSET) | (0x8 << IVHD_EFR_PASMAX_OFFSET) | (0 << IVHD_EFR_MSINUMPPR_OFFSET) |
                   (4 << IVHD_EFR_PNCOUNTERS_OFFSET) | (2 << IVHD_EFR_PNBANKS_OFFSET);
}

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
GnbCreateIvhdTN (
     OUT   IVRS_IVHD_ENTRY            *Ivhd,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  )
{
  PCI_ADDR  Start;
  PCI_ADDR  End;
  Start.AddressValue = MAKE_SBDFO (0, 0, 1, 0, 0);
  End.AddressValue = MAKE_SBDFO (0, 0xFF, 0x1F, 6, 0);
  GnbIvhdAddDeviceRangeEntry (Start, End, 0, Ivhd, StdHeader);
  SbCreateIvhdEntries (Ivhd, StdHeader);
}


/*----------------------------------------------------------------------------------------*/
/**
 * Create IVHDR entry
 *
 *
 * @param[in]  Ivhd            IVHD header pointer
 * @param[in]  StdHeader       Standard configuration header
 *
 */
VOID
GnbCreateIvhdrTN (
     OUT   IVRS_IVHD_ENTRY            *Ivhd,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  )
{


}


