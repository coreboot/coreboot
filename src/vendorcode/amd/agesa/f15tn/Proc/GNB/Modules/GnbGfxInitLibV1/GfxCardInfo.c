/* $NoKeywords:$ */
/**
 * @file
 *
 *  Supporting services to collect discrete GFX card info
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "AGESA.h"
#include  "Ids.h"
#include  "amdlib.h"
#include  "Gnb.h"
#include  "GnbGfx.h"
#include "GnbCommonLib.h"
#include  "GfxCardInfo.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBGFXINITLIBV1_GFXCARDINFO_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

typedef struct {
  GNB_PCI_SCAN_DATA     ScanData;
  GFX_CARD_CARD_INFO    *GfxCardInfo;
  PCI_ADDR              BaseBridge;
  UINT8                 BusNumber;
} GFX_SCAN_DATA;


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

SCAN_STATUS
GfxScanPcieDevice (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  );



/*----------------------------------------------------------------------------------------*/
/**
 * Get information about all discrete GFX card in system
 *
 *
 *
 * @param[out] GfxCardInfo     Pointer to GFX card info structure
 * @param[in]  StdHeader  Standard configuration header
 */

VOID
GfxGetDiscreteCardInfo (
     OUT   GFX_CARD_CARD_INFO  *GfxCardInfo,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  GFX_SCAN_DATA GfxScanData;
  PCI_ADDR      Start;
  PCI_ADDR      End;
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxGetDiscreteCardInfo Enter\n");
  Start.AddressValue = MAKE_SBDFO (0, 0, 2, 0, 0);
  End.AddressValue = MAKE_SBDFO (0, 0, 0x1f, 7, 0);
  GfxScanData.BusNumber = 5;
  GfxScanData.ScanData.GnbScanCallback = GfxScanPcieDevice;
  GfxScanData.ScanData.StdHeader = StdHeader;
  GfxScanData.GfxCardInfo = GfxCardInfo;
  GnbLibPciScan (Start, End, &GfxScanData.ScanData);
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxGetDiscreteCardInfo Exit\n");
}


/*----------------------------------------------------------------------------------------*/
/**
 * Evaluate device
 *
 *
 *
 * @param[in]     Device          PCI Address
 * @param[in,out] ScanData        Scan configuration data
 * @retval                        Scan Status of 0
 */

SCAN_STATUS
GfxScanPcieDevice (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  )
{
  UINT8   ClassCode;
  UINT32  VendorId;

  IDS_HDT_CONSOLE (GFX_MISC, "  Evaluate device [%d:%d:%d]\n",
    Device.Address.Bus, Device.Address.Device, Device.Address.Function
    );

  if (GnbLibPciIsBridgeDevice (Device.AddressValue, ScanData->StdHeader)) {
    UINT32    SaveBusConfiguration;
    UINT32    Value;

    if (Device.Address.Bus == 0) {
      ((GFX_SCAN_DATA *) ScanData)->BaseBridge = Device;
    }
    GnbLibPciRead (Device.AddressValue | 0x18, AccessWidth32, &SaveBusConfiguration, ScanData->StdHeader);
    Value = (((0xFF << 8) | ((GFX_SCAN_DATA *) ScanData)->BusNumber) << 8) | Device.Address.Bus;
    GnbLibPciWrite (Device.AddressValue | 0x18, AccessWidth32, &Value, ScanData->StdHeader);
    ((GFX_SCAN_DATA *) ScanData)->BusNumber++;

    GnbLibPciScanSecondaryBus (Device, ScanData);

    ((GFX_SCAN_DATA *) ScanData)->BusNumber--;
    GnbLibPciWrite (Device.AddressValue | 0x18, AccessWidth32, &SaveBusConfiguration, ScanData->StdHeader);
    return 0;
  }
  GnbLibPciRead (Device.AddressValue | 0x0b, AccessWidth8, &ClassCode, ScanData->StdHeader);
  if (ClassCode == 3) {
    IDS_HDT_CONSOLE (GFX_MISC, "  Found GFX Card\n"
      );

    GnbLibPciRead (Device.AddressValue | 0x00, AccessWidth32, &VendorId, ScanData->StdHeader);
    if (!GnbLibPciIsPcieDevice (Device.AddressValue, ScanData->StdHeader)) {
      IDS_HDT_CONSOLE (GFX_MISC, "  GFX Card is PCI device\n"
        );
      ((GFX_SCAN_DATA *) ScanData)->GfxCardInfo->PciGfxCardBitmap |= (1 << ((GFX_SCAN_DATA *) ScanData)->BaseBridge.Address.Device);
      return 0;
    }
    if ((UINT16) VendorId == 0x1002) {
      IDS_HDT_CONSOLE (GFX_MISC, "  GFX Card is AMD PCIe device\n"
        );
      ((GFX_SCAN_DATA *) ScanData)->GfxCardInfo->AmdPcieGfxCardBitmap |= (1 << ((GFX_SCAN_DATA *) ScanData)->BaseBridge.Address.Device);
    }
    ((GFX_SCAN_DATA *) ScanData)->GfxCardInfo->PcieGfxCardBitmap |= (1 << ((GFX_SCAN_DATA *) ScanData)->BaseBridge.Address.Device);
  }
  return 0;
}
