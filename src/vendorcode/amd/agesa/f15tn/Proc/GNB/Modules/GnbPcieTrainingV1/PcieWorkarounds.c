/* $NoKeywords:$ */
/**
 * @file
 *
 * Various workarounds
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
#include  "GnbPcie.h"
#include  "GnbCommonLib.h"
#include  "GnbPcieConfig.h"
#include  "GnbRegistersLN.h"
#include  "PcieWorkarounds.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBPCIETRAININGV1_PCIEWORKAROUNDS_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

extern BUILD_OPT_CFG UserOptions;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */
AGESA_STATUS
PcieConfigureBridgeResources (
  IN      PCI_ADDR             Port,
  IN      AMD_CONFIG_PARAMS    *StdHeader
  );

VOID
PcieFreeBridgeResources (
  IN      PCI_ADDR             Port,
  IN      AMD_CONFIG_PARAMS    *StdHeader
  );

GFX_WORKAROUND_STATUS
PcieDeskewWorkaround (
  IN      PCI_ADDR            Device,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  );

GFX_WORKAROUND_STATUS
PcieNvWorkaround (
  IN      PCI_ADDR            Device,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
PcieProgramCpuMmio (
  OUT    UINT32              *SaveValues,
  IN     AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
PcieRestoreCpuMmio (
  IN     UINT32              *RestoreValues,
  IN     AMD_CONFIG_PARAMS   *StdHeader
  );

BOOLEAN
PcieIsDeskewCardDetected (
  IN      UINT16             DeviceId
  );

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * ATI RV370/RV380 card workaround
 *
 *
 *
 * @param[in]  Port        PCI addreses of the port
 * @param[in]  StdHeader   Standard configuration header
 * @retval     GFX_WORKAROUND_STATUS   Return the GFX Card Workaround status
 */
GFX_WORKAROUND_STATUS
PcieGfxCardWorkaround (
  IN     PCI_ADDR               Port,
  IN     AMD_CONFIG_PARAMS      *StdHeader
  )
{
  GFX_WORKAROUND_STATUS   Status;
  UINT16                  DeviceId;
  UINT16                  VendorId;
  UINT8                   DevClassCode;
  UINT32                  SaveValueData[2];
  PCI_ADDR                Ep;

  Status = GFX_WORKAROUND_SUCCESS;

  Ep.AddressValue = MAKE_SBDFO (0, Port.Address.Bus + Port.Address.Device, 0, 0, 0);
  if (PcieConfigureBridgeResources (Port, StdHeader) == AGESA_SUCCESS) {
    GnbLibPciRead (Ep.AddressValue | 0x00, AccessWidth16, &DeviceId, StdHeader);
    Status = GFX_WORKAROUND_DEVICE_NOT_READY;
    if (DeviceId != 0xffff) {
      GnbLibPciRead (Ep.AddressValue | 0x02, AccessWidth16, &VendorId, StdHeader);
      if (VendorId != 0xffff) {
        GnbLibPciRead (Ep.AddressValue | 0x0B, AccessWidth8, &DevClassCode, StdHeader);
        Status = GFX_WORKAROUND_SUCCESS;
        if (DevClassCode == 3) {
          PcieProgramCpuMmio (SaveValueData, StdHeader);
          if (VendorId == 0x1002 && PcieIsDeskewCardDetected (DeviceId)) {
            Status = PcieDeskewWorkaround (Ep, StdHeader);
          } else if (VendorId == 0x10DE) {
            Status = PcieNvWorkaround (Ep, StdHeader);
          }
          PcieRestoreCpuMmio (SaveValueData, StdHeader);
        }
      }
    }
    PcieFreeBridgeResources (Port, StdHeader);
  }
  return Status;
}


/*----------------------------------------------------------------------------------------*/
/**
 * RV370/RV380 Deskew workaround
 *
 *
 *
 * @param[in] Device     Pcie Address of ATI RV370/RV380 card.
 * @param[in] StdHeader  Standard configuration header
 */
GFX_WORKAROUND_STATUS
PcieDeskewWorkaround (
  IN      PCI_ADDR              Device,
  IN      AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINTN       MmioBase;
  UINT16      MmioData1;
  UINT32      MmioData2;

  MmioBase = UserOptions.CfgTempPcieMmioBaseAddress;
  if (MmioBase == 0) {
    return GFX_WORKAROUND_SUCCESS;
  }
  GnbLibPciWrite (Device.AddressValue | 0x18, AccessWidth32, &MmioBase, StdHeader);
  GnbLibPciRMW (Device.AddressValue | 0x04, AccessWidth8 , (UINT32)~BIT1, (UINT32)BIT1, StdHeader);
  GnbLibMemRMW (MmioBase + 0x120, AccessWidth16, 0, 0xb700, StdHeader);
  GnbLibMemRead (MmioBase + 0x120, AccessWidth16, &MmioData1, StdHeader);
  if (MmioData1 == 0xb700) {
    GnbLibMemRMW (MmioBase + 0x124, AccessWidth32, 0, 0x13, StdHeader);
    GnbLibMemRead (MmioBase + 0x124, AccessWidth32, &MmioData2, StdHeader);
    if (MmioData2 == 0x13) {
      GnbLibMemRead (MmioBase + 0x12C, AccessWidth32, &MmioData2, StdHeader);
      if (MmioData2 & BIT8) {
        return  GFX_WORKAROUND_RESET_DEVICE;
      }
    }
  }
  GnbLibPciRMW (Device.AddressValue | 0x04, AccessWidth8, (UINT32)~BIT1, 0x0, StdHeader);
  GnbLibPciRMW (Device.AddressValue | 0x18, AccessWidth32, 0x0, 0x0, StdHeader);

  return GFX_WORKAROUND_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 *  NV43 card workaround (lost SSID)
 *
 *
 *
 * @param[in] Device     Pcie Address of NV43 card.
 * @param[in] StdHeader  Standard configuration header
 */
GFX_WORKAROUND_STATUS
PcieNvWorkaround (
  IN     PCI_ADDR              Device,
  IN     AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINT32      DeviceSSID;
  UINTN       MmioBase;
  UINT32      MmioData3;

  MmioBase = UserOptions.CfgTempPcieMmioBaseAddress;
  if (MmioBase == 0) {
    return GFX_WORKAROUND_SUCCESS;
  }
  GnbLibPciRMW (Device.AddressValue | 0x30, AccessWidth32, 0x0, ((UINT32)MmioBase) | 1, StdHeader);
  GnbLibPciRMW (Device.AddressValue | 0x4, AccessWidth8, 0x0, 0x2, StdHeader);
  GnbLibPciRead (Device.AddressValue | 0x2c, AccessWidth32, &DeviceSSID, StdHeader);
  GnbLibMemRead (MmioBase + 0x54, AccessWidth32, &MmioData3, StdHeader);
  if (DeviceSSID != MmioData3) {
    GnbLibPciRMW (Device.AddressValue | 0x40, AccessWidth32, 0x0, MmioData3, StdHeader);
  }
  GnbLibPciRMW (Device.AddressValue | 0x30, AccessWidth32, 0x0, 0x0, StdHeader);
  GnbLibPciRMW (Device.AddressValue | 0x4, AccessWidth8, 0x0, 0x0, StdHeader);
  return GFX_WORKAROUND_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 *  Allocate temporary resources for Pcie P2P bridge
 *
 *
 *
 * @param[in] Port         Pci Address of Port to initialize.
 * @param[in] StdHeader    Standard configuration header
 */
AGESA_STATUS
PcieConfigureBridgeResources (
  IN      PCI_ADDR              Port,
  IN      AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINT32      Value;
  UINT32      MmioBase;

  MmioBase = UserOptions.CfgTempPcieMmioBaseAddress;
  if (MmioBase == 0) {
    return AGESA_WARNING;
  }
  Value = Port.Address.Bus + ((Port.Address.Bus + Port.Address.Device) << 8) + ((Port.Address.Bus + Port.Address.Device) << 16);
  GnbLibPciWrite (Port.AddressValue | DxF0x18_ADDRESS, AccessWidth32, &Value, StdHeader);
  Value = MmioBase + (MmioBase >> 16);
  GnbLibPciWrite (Port.AddressValue | DxF0x20_ADDRESS, AccessWidth32, &Value, StdHeader);
  Value = 0x000fff0;
  GnbLibPciWrite (Port.AddressValue | DxF0x24_ADDRESS, AccessWidth32, &Value, StdHeader);
  Value = 0x2;
  GnbLibPciWrite (Port.AddressValue | 0x4 , AccessWidth8, &Value, StdHeader);
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 *  Free temporary resources for Pcie P2P bridge
 *
 *
 *
 * @param[in] Port       Pci Address of Port to clear resource allocation.
 * @param[in] StdHeader  Standard configuration header
 */
VOID
PcieFreeBridgeResources (
  IN      PCI_ADDR            Port,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32 Value;

  Value = 0;
  GnbLibPciWrite (Port.AddressValue | 0x4 , AccessWidth8,  &Value, StdHeader);
  GnbLibPciWrite (Port.AddressValue | DxF0x18_ADDRESS, AccessWidth32, &Value, StdHeader);
  GnbLibPciWrite (Port.AddressValue | DxF0x20_ADDRESS, AccessWidth32, &Value, StdHeader);
  GnbLibPciWrite (Port.AddressValue | DxF0x24_ADDRESS, AccessWidth32, &Value, StdHeader);

}


/*----------------------------------------------------------------------------------------*/
/*
 * Save CPU MMIO register
 *
 *
 *
 * @param[out] UINT32        SaveValues
 * @param[in]  StdHeader     Standard configuration header
 *
 */
VOID
PcieProgramCpuMmio (
     OUT   UINT32             *SaveValues,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  //Save CPU MMIO Register
  GnbLibPciRead (MAKE_SBDFO (0, 0, 0x18, 0x1, 0xB8), AccessWidth32, SaveValues, StdHeader);
  GnbLibPciRead (MAKE_SBDFO (0, 0, 0x18, 0x1, 0xBC), AccessWidth32, SaveValues + 1, StdHeader);

  //Write Temp Pcie MMIO to CPU
  GnbLibPciRMW (MAKE_SBDFO (0, 0, 0x18, 0x1, 0xBC), AccessWidth32, 0, (UserOptions.CfgTempPcieMmioBaseAddress >> 16) << 8, StdHeader);
  GnbLibPciRMW (MAKE_SBDFO (0, 0, 0x18, 0x1, 0xB8), AccessWidth32, 0, ((UserOptions.CfgTempPcieMmioBaseAddress >> 16) << 8) | 0x3, StdHeader);

}

/*----------------------------------------------------------------------------------------*/
/*
 * Restore CPU MMIO register
 *
 *
 *
 * @param[in] PCIe_PLATFORM_CONFIG    Pcie
 * @param[in] StdHeader               Standard configuration header
 */
VOID
PcieRestoreCpuMmio (
  IN      UINT32                *RestoreValues,
  IN      AMD_CONFIG_PARAMS     *StdHeader
  )
{
  //Restore CPU MMIO Register
  GnbLibPciRMW (MAKE_SBDFO (0, 0, 0x18, 0x1, 0xB8), AccessWidth32, 0, *RestoreValues, StdHeader);
  GnbLibPciRMW (MAKE_SBDFO (0, 0, 0x18, 0x1, 0xBC), AccessWidth32, 0, *(RestoreValues + 1), StdHeader);

}

/*----------------------------------------------------------------------------------------*/
/*
 * Check if card required test for deskew workaround
 *
 *
 *
 * @param[in] DeviceId    Device ID
 */

BOOLEAN
PcieIsDeskewCardDetected (
  IN      UINT16    DeviceId
  )
{
  if ((DeviceId >= 0x3150 && DeviceId <= 0x3152) || (DeviceId == 0x3154) ||
     (DeviceId == 0x3E50) || (DeviceId == 0x3E54) ||
     ((DeviceId & 0xfff8) == 0x5460) || ((DeviceId & 0xfff8)  == 0x5B60)) {
    return TRUE;
  }
  return FALSE;
}


