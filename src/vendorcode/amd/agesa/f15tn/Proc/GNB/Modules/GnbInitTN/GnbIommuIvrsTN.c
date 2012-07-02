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


