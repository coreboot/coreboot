/* $NoKeywords:$ */
/**
 * @file
 *
 * Configure Max Payload
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision:
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
#include  "GnbPcie.h"
#include  "GnbCommonLib.h"
#include  "GnbPcieConfig.h"
#include  "GnbPcieInitLibV1.h"
#include  "GnbPcieInitLibV4.h"
#include  "PcieMaxPayloadV4.h"
#include  "GnbRegistersTN.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBPCIEINITLIBV4_PCIEMAXPAYLOADV4_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */
typedef struct {
  GNB_PCI_SCAN_DATA       ScanData;
  UINT8                   MaxPayload;
} PCIE_MAX_PAYLOAD_DATA;


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */


SCAN_STATUS
PcieGetMaxPayloadCallback (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  );

SCAN_STATUS
PcieSetMaxPayloadCallback (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  );

AGESA_STATUS
PciePayloadBlackListFeature (
  IN       PCI_ADDR               Device,
  IN       UINT8                  *MaxPayload,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Determine maximum payload size for PCIe segment
 *
 *  Scan through all link in segment to determine maximum payload by EPs.
 *
 * @param[in]   DownstreamPort    PCI address of PCIe port
 * @param[in]   StdHeader         Standard configuration header
 *
 */

VOID
PcieSetMaxPayload (
  IN       PCI_ADDR                DownstreamPort,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  )
{
  PCIE_MAX_PAYLOAD_DATA  PcieMaxPayloadData;

  IDS_HDT_CONSOLE (GNB_TRACE, "  PcieSetMaxPayload for Device = %d:%d:%d\n",
    DownstreamPort.Address.Bus,
    DownstreamPort.Address.Device,
    DownstreamPort.Address.Function
    );
  PcieMaxPayloadData.MaxPayload = MAX_PAYLOAD;
  PcieMaxPayloadData.ScanData.StdHeader = StdHeader;
  PcieMaxPayloadData.ScanData.GnbScanCallback = PcieGetMaxPayloadCallback;
  GnbLibPciScan (DownstreamPort, DownstreamPort, &PcieMaxPayloadData.ScanData);
  PcieMaxPayloadData.ScanData.GnbScanCallback = PcieSetMaxPayloadCallback;
  GnbLibPciScan (DownstreamPort, DownstreamPort, &PcieMaxPayloadData.ScanData);
  IDS_HDT_CONSOLE (GNB_TRACE, "  PcieSetMaxPayloadExit\n");
}

/*----------------------------------------------------------------------------------------*/
/**
 * Evaluate device Max Payload - save SMALLEST Max Payload for PCIe Segment
 *
 *
 *
 * @param[in]     Device          PCI Address
 * @param[in,out] ScanData        Scan configuration data
 * @retval                        Scan Status of 0
 */

SCAN_STATUS
PcieGetMaxPayloadCallback (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  )
{
  SCAN_STATUS             ScanStatus;
  PCIE_MAX_PAYLOAD_DATA   *PcieMaxPayloadData;
  PCIE_DEVICE_TYPE        DeviceType;
  UINT32                  Value;
  UINT8                   PcieCapPtr;
  UINT8                   DeviceMaxPayload;

  PcieMaxPayloadData = (PCIE_MAX_PAYLOAD_DATA*) ScanData;
  ScanStatus = SCAN_SUCCESS;
  IDS_HDT_CONSOLE (GNB_TRACE, "  PcieGetMaxPayloadCallback for Device = %d:%d:%d\n",
    Device.Address.Bus,
    Device.Address.Device,
    Device.Address.Function
    );
  PcieCapPtr = GnbLibFindPciCapability (Device.AddressValue, PCIE_CAP_ID, ScanData->StdHeader);
  if (PcieCapPtr != 0) {
    GnbLibPciRead (
      Device.AddressValue | (PcieCapPtr + PCIE_DEVICE_CAP_REGISTER),
      AccessWidth32,
      &Value,
      ScanData->StdHeader
    );
    DeviceMaxPayload = (UINT8) (Value & 0x7);
    PciePayloadBlackListFeature (Device, &DeviceMaxPayload, ScanData->StdHeader);
    IDS_HDT_CONSOLE (GNB_TRACE, "  Found DeviceMaxPayload as %d (Value = %x\n", DeviceMaxPayload, Value);
    if (DeviceMaxPayload < PcieMaxPayloadData->MaxPayload) {
      PcieMaxPayloadData->MaxPayload = DeviceMaxPayload;
    }
  }
  DeviceType = GnbLibGetPcieDeviceType (Device, ScanData->StdHeader);
  switch (DeviceType) {
  case  PcieDeviceRootComplex:
  case  PcieDeviceDownstreamPort:
  case  PcieDeviceUpstreamPort:
    GnbLibPciScanSecondaryBus (Device, &PcieMaxPayloadData->ScanData);
    break;
  case  PcieDeviceEndPoint:
  case  PcieDeviceLegacyEndPoint:
    break;
  default:
    break;
  }
  return SCAN_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Configure the Max Payload setting to all devices in the PCIe Segment
 *
 *
 *
 * @param[in]     Device          PCI Address
 * @param[in,out] ScanData        Scan configuration data
 * @retval                        Scan Status of 0
 */

SCAN_STATUS
PcieSetMaxPayloadCallback (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  )
{
  SCAN_STATUS             ScanStatus;
  PCIE_MAX_PAYLOAD_DATA   *PcieMaxPayloadData;
  PCIE_DEVICE_TYPE        DeviceType;
  UINT8                   PcieCapPtr;

  PcieMaxPayloadData = (PCIE_MAX_PAYLOAD_DATA*) ScanData;
  ScanStatus = SCAN_SUCCESS;
  IDS_HDT_CONSOLE (GNB_TRACE, "  PcieSetMaxPayloadCallback for Device = %d:%d:%d to %d\n",
    Device.Address.Bus,
    Device.Address.Device,
    Device.Address.Function,
    PcieMaxPayloadData->MaxPayload
    );
  PcieCapPtr = GnbLibFindPciCapability (Device.AddressValue, PCIE_CAP_ID, ScanData->StdHeader);
  if (PcieCapPtr != 0) {
    GnbLibPciRMW (
      Device.AddressValue | (PcieCapPtr + PCIE_DEVICE_CTRL_REGISTER),
      AccessWidth32,
      ~(UINT32) (0x7 << 5),
      ((UINT32)PcieMaxPayloadData->MaxPayload << 5),
      ScanData->StdHeader
    );
  }
  DeviceType = GnbLibGetPcieDeviceType (Device, ScanData->StdHeader);
  switch (DeviceType) {
  case  PcieDeviceRootComplex:
  case  PcieDeviceDownstreamPort:
  case  PcieDeviceUpstreamPort:
    GnbLibPciScanSecondaryBus (Device, &PcieMaxPayloadData->ScanData);
    break;
  case  PcieDeviceEndPoint:
  case  PcieDeviceLegacyEndPoint:
    break;
  default:
    break;
  }
  return SCAN_SUCCESS;
}

UINT16  PayloadBlacklistDeviceTable[] = {
  0x1969, 0x1083, (UINT16) MAX_PAYLOAD_128
};

/*----------------------------------------------------------------------------------------*/
/**
 * Pcie Max_Payload_Size Black List
 *
 *
 *
 * @param[in] Device              PCI_ADDR of PCIe Device to evaluate
 * @param[in] MaxPayload          Pointer to Max_Payload_Size value
 * @param[in] StdHeader           Standard configuration header
 * @retval     AGESA_STATUS
 */

AGESA_STATUS
PciePayloadBlackListFeature (
  IN       PCI_ADDR               Device,
  IN       UINT8                  *MaxPayload,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32  TargetDeviceId;
  UINTN   i;
  UINT32  DeviceId;
  UINT32  VendorId;

  GnbLibPciRead (Device.AddressValue, AccessWidth32, &TargetDeviceId, StdHeader);
  for (i = 0; i < (sizeof (PayloadBlacklistDeviceTable) / sizeof (UINT16)); i = i + 3) {
    VendorId = PayloadBlacklistDeviceTable[i];
    DeviceId = PayloadBlacklistDeviceTable[i + 1];
    if (VendorId == (UINT16)TargetDeviceId) {
      if (DeviceId == 0xFFFF || DeviceId == (TargetDeviceId >> 16)) {
        *MaxPayload = (UINT8) PayloadBlacklistDeviceTable[i + 2];
      }
    }
  }
  return AGESA_SUCCESS;
}
