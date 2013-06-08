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
