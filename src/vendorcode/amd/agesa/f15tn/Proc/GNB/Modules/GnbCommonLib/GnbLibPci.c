/* $NoKeywords:$ */
/**
 * @file
 *
 * Various PCI service routines.
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


#include  "AGESA.h"
#include  "amdlib.h"
#include  "Gnb.h"
#include  "GnbLibPciAcc.h"
#include  "GnbLibPci.h"
#include  "GnbLib.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBCOMMONLIB_GNBLIBPCI_FILECODE

/*----------------------------------------------------------------------------------------*/
/*
 * Check if device present
 *
 *
 *
 * @param[in] Address         PCI address (as described in PCI_ADDR)
 * @param[in] StdHeader       Standard configuration header
 * @retval    TRUE            Device is present
 * @retval    FALSE           Device is not present
 */

BOOLEAN
GnbLibPciIsDevicePresent (
  IN      UINT32              Address,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32  DeviceId;
  GnbLibPciRead (Address, AccessWidth32, &DeviceId, StdHeader);
  if (DeviceId == 0xffffffff) {
    return FALSE;
  } else {
    return TRUE;
  }
}


/*----------------------------------------------------------------------------------------*/
/*
 * Check if device is bridge
 *
 *
 *
 * @param[in] Address         PCI address (as described in PCI_ADDR)
 * @param[in] StdHeader       Standard configuration header
 * @retval    TRUE            Device is a bridge
 * @retval    FALSE           Device is not a bridge
 */

BOOLEAN
GnbLibPciIsBridgeDevice (
  IN      UINT32              Address,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8   Header;
  GnbLibPciRead (Address | 0xe, AccessWidth8, &Header, StdHeader);
  if ((Header & 0x7f) == 1) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/*----------------------------------------------------------------------------------------*/
/*
 * Check if device is multifunction
 *
 *
 *
 * @param[in] Address         PCI address (as described in PCI_ADDR)
 * @param[in] StdHeader       Standard configuration header
 * @retval    TRUE            Device is a multifunction device.
 * @retval    FALSE           Device is a single function device.
 *
 */
BOOLEAN
GnbLibPciIsMultiFunctionDevice (
  IN      UINT32              Address,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8   Header;
  GnbLibPciRead (Address | 0xe, AccessWidth8, &Header, StdHeader);
  if ((Header & 0x80) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/*----------------------------------------------------------------------------------------*/
/*
 * Check if device is PCIe device
 *
 *
 *
 * @param[in] Address         PCI address (as described in PCI_ADDR)
 * @param[in] StdHeader       Standard configuration header
 * @retval    TRUE            Device is a PCIe device
 * @retval    FALSE           Device is not a PCIe device
 *
 */

BOOLEAN
GnbLibPciIsPcieDevice (
  IN      UINT32              Address,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  if (GnbLibFindPciCapability (Address, PCIE_CAP_ID, StdHeader) != 0 ) {
    return TRUE;
  } else {
    return FALSE;
  }
}


/*----------------------------------------------------------------------------------------*/
/*
 * Find PCI capability pointer
 *
 *
 *
 * @param[in] Address         PCI address (as described in PCI_ADDR)
 * @param[in] CapabilityId    PCI capability ID
 * @param[in] StdHeader       Standard configuration header
 * @retval                    Register address of capability pointer
 *
 */

UINT8
GnbLibFindPciCapability (
  IN      UINT32              Address,
  IN      UINT8               CapabilityId,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8     CapabilityPtr;
  UINT8     CurrentCapabilityId;
  CapabilityPtr = 0x34;
  if (!GnbLibPciIsDevicePresent (Address, StdHeader)) {
    return  0;
  }
  while (CapabilityPtr != 0) {
    GnbLibPciRead (Address | CapabilityPtr, AccessWidth8 , &CapabilityPtr, StdHeader);
    if (CapabilityPtr != 0) {
      GnbLibPciRead (Address | CapabilityPtr , AccessWidth8 , &CurrentCapabilityId, StdHeader);
      if (CurrentCapabilityId == CapabilityId) {
        break;
      }
      CapabilityPtr++;
    }
  }
  return  CapabilityPtr;
}
/*----------------------------------------------------------------------------------------*/
/*
 * Find PCIe extended capability pointer
 *
 *
 *
 * @param[in] Address               PCI address (as described in PCI_ADDR)
 * @param[in] ExtendedCapabilityId  Extended PCIe capability ID
 * @param[in] StdHeader             Standard configuration header
 * @retval                          Register address of extended capability pointer
 *
 */

#if 0 /* Not used */
UINT16
GnbLibFindPcieExtendedCapability (
  IN      UINT32              Address,
  IN      UINT16              ExtendedCapabilityId,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT16  CapabilityPtr;
  UINT32  ExtendedCapabilityIdBlock;
  if (GnbLibPciIsPcieDevice (Address, StdHeader)) {
    GnbLibPciRead (Address | 0x100 , AccessWidth32 , &ExtendedCapabilityIdBlock, StdHeader);
    if ((ExtendedCapabilityIdBlock != 0) && ((UINT16)ExtendedCapabilityIdBlock != 0xffff)) {
      do {
        CapabilityPtr = (UINT16) ((ExtendedCapabilityIdBlock >> 20) & 0xfff);
        if ((UINT16)ExtendedCapabilityIdBlock == ExtendedCapabilityId) {
          return  CapabilityPtr;
        }
        GnbLibPciRead (Address | CapabilityPtr , AccessWidth32 , &ExtendedCapabilityIdBlock, StdHeader);
      } while (((ExtendedCapabilityIdBlock >> 20) & 0xfff) !=  0);
    }
  }
  return  0;
}
#endif
/*----------------------------------------------------------------------------------------*/
/*
 * Scan range of device on PCI bus.
 *
 *
 *
 * @param[in] Start           Start address to start scan from
 * @param[in] End             End address of scan
 * @param[in] ScanData        Supporting data
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
GnbLibPciScan (
  IN      PCI_ADDR            Start,
  IN      PCI_ADDR            End,
  IN      GNB_PCI_SCAN_DATA   *ScanData
  )
{
  UINTN       Bus;
  UINTN       Device;
  UINTN       LastDevice;
  UINTN       Function;
  UINTN       LastFunction;
  PCI_ADDR    PciDevice;
  SCAN_STATUS Status;

  for (Bus = Start.Address.Bus; Bus <= End.Address.Bus; Bus++) {
    Device = (Bus == Start.Address.Bus) ? Start.Address.Device : 0x00;
    LastDevice = (Bus == End.Address.Bus) ? End.Address.Device : 0x1F;
    for ( ; Device <= LastDevice; Device++) {
      if ((Bus == Start.Address.Bus) && (Device == Start.Address.Device)) {
        Function = Start.Address.Function;
      } else {
        Function = 0x0;
      }
      PciDevice.AddressValue = MAKE_SBDFO (0, Bus, Device, Function, 0);
      if (!GnbLibPciIsDevicePresent (PciDevice.AddressValue, ScanData->StdHeader)) {
        continue;
      }
      if (GnbLibPciIsMultiFunctionDevice (PciDevice.AddressValue, ScanData->StdHeader)) {
        if ((Bus == End.Address.Bus) && (Device == End.Address.Device)) {
          LastFunction = Start.Address.Function;
        } else {
          LastFunction = 0x7;
        }
      } else {
        LastFunction = 0x0;
      }
      for ( ; Function <= LastFunction; Function++) {
        PciDevice.AddressValue = MAKE_SBDFO (0, Bus, Device, Function, 0);
        if (GnbLibPciIsDevicePresent (PciDevice.AddressValue, ScanData->StdHeader)) {
          Status = ScanData->GnbScanCallback (PciDevice, ScanData);
          if ((Status & SCAN_SKIP_FUNCTIONS) != 0) {
            Function = LastFunction + 1;
          }
          if ((Status & SCAN_SKIP_DEVICES) != 0) {
            Device = LastDevice + 1;
          }
          if ((Status & SCAN_SKIP_BUSES) != 0) {
            Bus = End.Address.Bus + 1;
          }
        }
      }
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Scan all subordinate buses
 *
 *
 * @param[in]   Bridge            PCI bridge address
 * @param[in,out] ScanData        Scan configuration data
 *
 */
VOID
GnbLibPciScanSecondaryBus (
  IN       PCI_ADDR             Bridge,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  )
{
  PCI_ADDR  StartRange;
  PCI_ADDR  EndRange;
  UINT8     SecondaryBus;
  GnbLibPciRead (Bridge.AddressValue | 0x19, AccessWidth8, &SecondaryBus, ScanData->StdHeader);
  if (SecondaryBus != 0) {
    StartRange.AddressValue = MAKE_SBDFO (0, SecondaryBus, 0, 0, 0);
    EndRange.AddressValue = MAKE_SBDFO (0, SecondaryBus, 0x1f, 0x7, 0);
    GnbLibPciScan (StartRange, EndRange, ScanData);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get PCIe device type
 *
 *
 *
 * @param[in] Device      PCI address of device.
 * @param[in] StdHeader  Northbridge configuration structure pointer.
 *
 * @retval    PCIE_DEVICE_TYPE
 */
 /*----------------------------------------------------------------------------------------*/

PCIE_DEVICE_TYPE
GnbLibGetPcieDeviceType (
  IN      PCI_ADDR            Device,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8       PcieCapPtr;
  UINT8       Value;

  PcieCapPtr = GnbLibFindPciCapability (Device.AddressValue, PCIE_CAP_ID, StdHeader);
  if (PcieCapPtr != 0) {
    GnbLibPciRead (Device.AddressValue | (PcieCapPtr + 0x2) , AccessWidth8, &Value, StdHeader);
    return Value >> 4;
  }
  return PcieNotPcieDevice;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Save config space area
 *
 *
 *
 * @param[in] Address                 PCI address of device.
 * @param[in] StartRegisterAddress    Start register address.
 * @param[in] EndRegisterAddress      End register address.
 * @param[in] Width                   Acess width.
 * @param[in] StdHeader               Standard header.
 *
 */
 /*----------------------------------------------------------------------------------------*/

VOID
GnbLibS3SaveConfigSpace (
  IN      UINT32              Address,
  IN      UINT16              StartRegisterAddress,
  IN      UINT16              EndRegisterAddress,
  IN      ACCESS_WIDTH        Width,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT16    Index;
  UINT16    Delta;
  UINT16    Length;
  Length = (StartRegisterAddress < EndRegisterAddress) ? (EndRegisterAddress - StartRegisterAddress) : (StartRegisterAddress - EndRegisterAddress);
  Delta = LibAmdAccessWidth (Width);
  for (Index = 0; Index <= Length; Index = Index + Delta) {
    GnbLibPciRMW (
      Address | ((StartRegisterAddress < EndRegisterAddress) ? (StartRegisterAddress + Index) : (StartRegisterAddress - Index)),
      Width,
      0xffffffff,
      0x0,
      StdHeader
      );
  }
}
