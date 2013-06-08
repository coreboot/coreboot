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
