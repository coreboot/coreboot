/**
 * @file
 *
 * NB library functions.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CIMx-NB
 * @e sub-project:
 * @e \$Revision:$   @e \$Date:$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
 *
 ***************************************************************************/
/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */

#include "NbPlatform.h"
#include "amdDebugOutLib.h"

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

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Get silicon type and revision info.
 *
 *
 *
 * @param[in]   NbConfigPtr configuration structure pointer.
 * @retval      NB_INFO Northbrige Info Structure.
 */
/*----------------------------------------------------------------------------------------*/
NB_INFO
LibNbGetRevisionInfo (
  IN      AMD_NB_CONFIG   *NbConfigPtr
  )
{
  NB_INFO RevisionInfo;
  UINT16  DeviceId;
  UINT8   RevisionId;
  UINT32  PrivateId;
  LibNbPciRead (NbConfigPtr->NbPciAddress.AddressValue | 0x8, AccessWidth8, &RevisionId, NbConfigPtr);
  RevisionInfo.Revision = RevisionId;
  LibNbPciRead (NbConfigPtr->NbPciAddress.AddressValue | 0x2, AccessWidth16, &DeviceId, NbConfigPtr);
  switch (DeviceId) {
  case  0x5956:
    RevisionInfo.Type = NB_RD890TV;
    break;
  case  0x5957:
    RevisionInfo.Type = NB_RX780;
    break;
  case  0x5958:
    RevisionInfo.Type = NB_RD780;
    break;
  case  0x5A10:
    RevisionInfo.Type = NB_SR5690;
    break;
  case  0x5A11:
    RevisionInfo.Type = NB_RD890;
    break;
  case  0x5A12:
    RevisionInfo.Type = NB_SR5670;
    break;
  case  0x5A13:
    RevisionInfo.Type = NB_SR5650;
    break;
  case  0x5A14:
    RevisionInfo.Type = NB_990FX;
    LibNbPciIndexRead (NbConfigPtr->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG7D, AccessWidth32, &PrivateId, NbConfigPtr);
    PrivateId = (PrivateId >> 21) & 0x0f;
    if (PrivateId == 1) {
      RevisionInfo.Type = NB_990FX;
    }
    if (PrivateId == 2) {
      RevisionInfo.Type = NB_990X;
    }
    if (PrivateId == 3) {
      RevisionInfo.Type = NB_970;
    }
    break;
  default:
    RevisionInfo.Type = NB_UNKNOWN;
    CIMX_ASSERT (FALSE);
  }
  return  RevisionInfo;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Call Back routine.
 *
 *
 *
 * @param[in] CallBackId   Callback ID.
 * @param[in] Data         Callback specific data.
 * @param[in] NbConfigPtr  Northbridge configuration structure pointer.
 */
/*----------------------------------------------------------------------------------------*/
AGESA_STATUS
LibNbCallBack (
  IN       UINT32          CallBackId,
  IN OUT   UINTN           Data,
  IN OUT   AMD_NB_CONFIG   *NbConfigPtr
  )
{
  AGESA_STATUS  Status;
  CALLOUT_ENTRY CallBackPtr = GET_BLOCK_CONFIG_PTR (NbConfigPtr)->StandardHeader.CalloutPtr;

  Status = AGESA_UNSUPPORTED;
  if (CallBackPtr != NULL) {
    CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (NbConfigPtr), CIMX_NB_TRACE), "[NBLIB]LibNbCallBack CallBackId = 0x%x\n", CallBackId));
    Status = (*CallBackPtr) (CallBackId, Data, GET_BLOCK_CONFIG_PTR (NbConfigPtr));
    CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (NbConfigPtr), CIMX_NB_TRACE), "[NBLIB]LibNbCallBack Return = 0x%x\n", Status));
  }
  return Status;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Call Back routine.
 *
 *
 *
 * @param[in] SystemApi    Pointer to System API
 * @param[in] ConfigPtr  Northbridge block configuration structure pointer.
 */
/*----------------------------------------------------------------------------------------*/
AGESA_STATUS
LibSystemApiCall (
  IN       SYSTEM_API             SystemApi,
  IN OUT   AMD_NB_CONFIG_BLOCK   *ConfigPtr
  )
{
  API_WORKSPACE Workspace;
  UINT8         NorthbridgeId;

  LibAmdMemFill (&Workspace, 0, sizeof (API_WORKSPACE), (AMD_CONFIG_PARAMS *)&(ConfigPtr->StandardHeader));
  Workspace.ConfigPtr = ConfigPtr;
  Workspace.Status = AGESA_SUCCESS;
  for (NorthbridgeId = 0; NorthbridgeId <= ConfigPtr->NumberOfNorthbridges; NorthbridgeId++) {
    ConfigPtr->Northbridges[NorthbridgeId].ConfigPtr = &Workspace.ConfigPtr;
  }
  if (SystemApi != NULL) {
    (*SystemApi)(ConfigPtr);
  }
  return Workspace.Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Call Back routine.
 *
 *
 *
 * @param[in] NbApi       Pointer to NB API
 * @param[in] ConfigPtr   Northbridge block configuration structure pointer
 */
/*----------------------------------------------------------------------------------------*/
AGESA_STATUS
LibNbApiCall (
  IN       NB_API                NbApi,
  IN OUT   AMD_NB_CONFIG_BLOCK   *ConfigPtr
  )
{
  UINT8         NorthbridgeId;
  AGESA_STATUS  Status;

  Status = AGESA_SUCCESS;
  for (NorthbridgeId = 0; NorthbridgeId <= ConfigPtr->NumberOfNorthbridges; NorthbridgeId++) {
    AMD_NB_CONFIG *NbConfigPtr = &ConfigPtr->Northbridges[NorthbridgeId];
    ConfigPtr->CurrentNorthbridge = NorthbridgeId;
    if (!LibNbIsDevicePresent (NbConfigPtr->NbPciAddress, NbConfigPtr)) {
      REPORT_EVENT (AGESA_WARNING, GENERAL_ERROR_NB_NOT_PRESENT, 0 , 0, 0, 0, NbConfigPtr);
      continue;
    }
    if (NbApi != NULL) {
      Status = (*NbApi) (NbConfigPtr);
      if (Status == AGESA_FATAL) {
        break;
      }
    }
  }
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Write PCI register.
 *
 *
 *
 * @param[in] Address     Compressed PCIE address identical to PCI_ADDR.AddressValue
 * @param[in] Width       Access width.
 * @param[in] Value       Pointer to new register value.
 * @param[in] NbConfigPtr Northbridge configuration structure pointer.
 */
/*----------------------------------------------------------------------------------------*/

VOID
LibNbPciWrite (
  IN      UINT32          Address,
  IN      ACCESS_WIDTH    Width,
  IN      VOID            *Value,
  IN      AMD_NB_CONFIG   *NbConfigPtr
  )
{
  PCI_ADDR  DeviceAddress;
  DeviceAddress.AddressValue = Address;
  LibAmdPciWrite (Width, DeviceAddress, Value, (AMD_CONFIG_PARAMS *)((NbConfigPtr == NULL)?NULL:GET_BLOCK_CONFIG_PTR (NbConfigPtr)));
}


/*----------------------------------------------------------------------------------------*/
/**
 * Read PCI register
 *
 *
 *
 * @param[in] Address     Compressed PCIE address identical to PCI_ADDR.AddressValue
 * @param[in] Width       Access width.
 * @param[in] Value       Pointer to save register value.
 * @param[in] NbConfigPtr Northbridge configuration structure pointer.
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
LibNbPciRead (
  IN       UINT32         Address,
  IN       ACCESS_WIDTH   Width,
     OUT   VOID           *Value,
  IN       AMD_NB_CONFIG  *NbConfigPtr
  )
{
  PCI_ADDR  DeviceAddress;
  DeviceAddress.AddressValue = Address;
  LibAmdPciRead (Width, DeviceAddress, Value, (AMD_CONFIG_PARAMS *)((NbConfigPtr == NULL)?NULL:GET_BLOCK_CONFIG_PTR (NbConfigPtr)));
}

/*----------------------------------------------------------------------------------------*/
/**
 * Read/Modify/Write  PCI register
 *
 *
 *
 * @param[in] Address     Compressed PCIE address identical to PCI_ADDR.AddressValue
 * @param[in] Width       Access width.
 * @param[in] Mask        AND Mask.
 * @param[in] Data        OR Mask.
 * @param[in] NbConfigPtr Northbridge configuration structure pointer.
 */
/*----------------------------------------------------------------------------------------*/
VOID
LibNbPciRMW (
  IN       UINT32         Address,
  IN       ACCESS_WIDTH   Width,
  IN       UINT32         Mask,
  IN       UINT32         Data,
  IN       AMD_NB_CONFIG  *NbConfigPtr
  )
{
  UINT32  Value;
  LibNbPciRead (Address,  Width, &Value, NbConfigPtr);
  Value = (Value & Mask) | Data;
  LibNbPciWrite (Address, Width, &Value, NbConfigPtr);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Read PCI Index/Data Address space
 *
 *
 *
 * @param[in] Address     Compressed PCIE address identical to PCI_ADDR.AddressValue
 * @param[in] Index       Index Address.
 * @param[in] Width       Access width of Index/Data register.
 * @param[in] Value       Pointer to save register value.
 * @param[in] NbConfigPtr Northbridge configuration structure pointer.
 */
/*----------------------------------------------------------------------------------------*/

VOID
LibNbPciIndexRead (
  IN       UINT32        Address,
  IN       UINT32        Index,
  IN       ACCESS_WIDTH  Width,
     OUT   UINT32        *Value,
  IN       AMD_NB_CONFIG *NbConfigPtr
  )
{
  UINT32  IndexOffset;
  IndexOffset = (1 << ((Width < 0x80)? (Width - 1): (Width - 0x81)));
  LibNbPciWrite (Address, Width, &Index, NbConfigPtr);
  LibNbPciRead (Address + IndexOffset, Width, Value, NbConfigPtr);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Write PCI Index/Data Address space
 *
 *
 *
 * @param[in] Address     Compressed PCIE address identical to PCI_ADDR.AddressValue
 * @param[in] Index       Index Address.
 * @param[in] Width       Access width of Index/Data register.
 * @param[in] Value       Pointer to save register value.
 * @param[in] NbConfigPtr Northbridge configuration structure pointer.
 */
/*----------------------------------------------------------------------------------------*/

VOID
LibNbPciIndexWrite (
  IN      UINT32        Address,
  IN      UINT32        Index,
  IN      ACCESS_WIDTH  Width,
  IN      UINT32        *Value,
  IN      AMD_NB_CONFIG *NbConfigPtr
  )
{
  UINT32  IndexOffset;
  IndexOffset = (1 << ((Width < 0x80)? (Width - 1): (Width - 0x81)));
  LibNbPciWrite (Address, Width, &Index, NbConfigPtr);
  LibNbPciWrite (Address + IndexOffset , Width, Value, NbConfigPtr);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Read/Modify/Write  PCI Index/Data Address space
 *
 *
 *
 * @param[in] Address     Compressed PCIE address identical to PCI_ADDR.AddressValue
 * @param[in] Index       Index Address.
 * @param[in] Width       Access width of Index/Data register.
 * @param[in] Mask       AND Mask.
 * @param[in] Data       OR Mask.
 * @param[in] NbConfigPtr Northbridge configuration structure pointer.
 */
/*----------------------------------------------------------------------------------------*/

VOID
LibNbPciIndexRMW (
  IN      UINT32        Address,
  IN      UINT32        Index,
  IN      ACCESS_WIDTH  Width,
  IN      UINT32        Mask,
  IN      UINT32        Data,
  IN      AMD_NB_CONFIG *NbConfigPtr
  )
{
  UINT32  Value;
  LibNbPciIndexRead (Address, Index, Width, &Value, NbConfigPtr);
  Value = (Value & Mask) | Data;
  LibNbPciIndexWrite (Address, Index, Width, &Value, NbConfigPtr);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Program table of indirect register.
 *
 *
 *
 * @param[in] Address     Compressed PCIE address identical to PCI_ADDR.AddressValue
 * @param[in] Index       Index Address. Index address OR with INDIRECT_REG_ENTRY.Register
 * @param[in] pTable      Pointer to indirect register table.
 * @param[in] Length      Number of entry in indirect register table.
 * @param[in] NbConfigPtr Northbridge configuration structure pointer.
 */
/*----------------------------------------------------------------------------------------*/
VOID
LibNbIndirectTableInit (
  IN      UINT32              Address,
  IN      UINT32              Index,
  IN      INDIRECT_REG_ENTRY  *pTable,
  IN      UINTN               Length,
  IN      AMD_NB_CONFIG       *NbConfigPtr
  )
{
  UINTN i;
  for (i = 0; i < Length; i++) {
    LibNbPciIndexRMW (Address, Index | pTable[i].Register , AccessS3SaveWidth32, pTable[i].Mask, pTable[i].Data, NbConfigPtr);
  }
}

/*----------------------------------------------------------------------------------------*/
/*
 * Find PCI capability pointer
 *
 *
 *
 *
 *
 */
/*----------------------------------------------------------------------------------------*/

UINT8
LibNbFindPciCapability (
  IN      UINT32        Address,
  IN      UINT8         CapabilityId,
  IN      AMD_NB_CONFIG *NbConfigPtr
  )
{
  UINT8     CapabilityPtr;
  UINT8     CurrentCapabilityId;
  PCI_ADDR  Device;
  Device.AddressValue = Address;
  CapabilityPtr = 0x34;
  if (!LibNbIsDevicePresent (Device, NbConfigPtr)) {
    return  0;
  }
  while (CapabilityPtr != 0) {
    LibNbPciRead (Address | CapabilityPtr, AccessWidth8 , &CapabilityPtr, NbConfigPtr);
    if (CapabilityPtr) {
      LibNbPciRead (Address | CapabilityPtr , AccessWidth8 , &CurrentCapabilityId, NbConfigPtr);
      if (CurrentCapabilityId == CapabilityId) break;
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
 *
 *
 */
/*----------------------------------------------------------------------------------------*/

UINT16
LibNbFindPcieExtendedCapability (
  IN      UINT32        Address,
  IN      UINT16        ExtendedCapabilityId,
  IN      AMD_NB_CONFIG *NbConfigPtr
  )
{
  UINT16  CapabilityPtr;
  UINT32  ExtendedCapabilityIdBlock;
  if (LibNbFindPciCapability (Address, 0x10, NbConfigPtr) != 0) {
    CapabilityPtr = 0x100;
    LibNbPciRead (Address | CapabilityPtr , AccessWidth32 , &ExtendedCapabilityIdBlock, NbConfigPtr);
    if (ExtendedCapabilityIdBlock != 0 && (UINT16)ExtendedCapabilityIdBlock != 0xffff) {
      do {
        if ((UINT16)ExtendedCapabilityIdBlock == ExtendedCapabilityId) {
          return  CapabilityPtr;
        }
        CapabilityPtr = (UINT16) ((ExtendedCapabilityIdBlock >> 20) & 0xfff);
        LibNbPciRead (Address | CapabilityPtr , AccessWidth32 , &ExtendedCapabilityIdBlock, NbConfigPtr);
      } while (CapabilityPtr !=  0);
    }
  }
  return  0;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Read IO space
 *
 *
 *
 * @param[in] Address     IO Port address.
 * @param[in] Width       Access width
 * @param[in] Value       Pointer to save IO port value;
 * @param[in] NbConfigPtr Northbridge configuration structure pointer.
 */
/*----------------------------------------------------------------------------------------*/

VOID
LibNbIoRead (
  IN       UINT16         Address,
  IN       ACCESS_WIDTH   Width,
     OUT   VOID           *Value,
  IN       AMD_NB_CONFIG  *NbConfigPtr
  )
{
  LibAmdIoRead (Width, Address, Value, (AMD_CONFIG_PARAMS *)((NbConfigPtr == NULL)?NULL:GET_BLOCK_CONFIG_PTR (NbConfigPtr)));
}

/*----------------------------------------------------------------------------------------*/
/**
 * Write IO space
 *
 *
 *
 * @param[in] Address     IO Port address.
 * @param[in] Width       Access width
 * @param[in] Value       Pointer to new IO port value
 * @param[in] NbConfigPtr Northbridge configuration structure pointer.
 */
VOID
LibNbIoWrite (
  IN      UINT16         Address,
  IN      ACCESS_WIDTH   Width,
  IN      VOID           *Value,
  IN      AMD_NB_CONFIG  *NbConfigPtr
  )
{
  LibAmdIoWrite (Width, Address, Value, (AMD_CONFIG_PARAMS *)((NbConfigPtr == NULL)?NULL:GET_BLOCK_CONFIG_PTR (NbConfigPtr)));
}


/*----------------------------------------------------------------------------------------*/
/**
 * Read/Modify/Write IO space
 *
 *
 *
 * @param[in] Address     IO Port address.
 * @param[in] Width       Access width
 * @param[in] Mask       AND Mask
 * @param[in] Data       OR Mask
 * @param[in] NbConfigPtr Northbridge configuration structure pointer.
 */
/*----------------------------------------------------------------------------------------*/

VOID
LibNbIoRMW (
  IN      UINT16         Address,
  IN      ACCESS_WIDTH   Width,
  IN      UINT32         Mask,
  IN      UINT32         Data,
  IN      AMD_NB_CONFIG  *NbConfigPtr
  )
{
  UINT32  Value;
  LibNbIoRead (Address, Width, &Value, NbConfigPtr);
  Value = (Value & Mask) | Data;
  LibNbIoWrite (Address, Width, &Value, NbConfigPtr);
}


/*----------------------------------------------------------------------------------------*/
/**
 * Read CPU HT link Phy register
 *
 *
 *
 * @param[in] Node        Node device Address (0x18 - Node 0, 0x19 - Mode 1 etc.)
 * @param[in] Link        HT Link ID (0 - Link 0, 1 - Link 1 etc.)
 * @param[in] Register    Register address.
 * @param[in] Value       Pointer to save register value
 * @param[in] NbConfigPtr Northbridge configuration block pointer.
 */
/*----------------------------------------------------------------------------------------*/

VOID
LibNbCpuHTLinkPhyRead (
  IN       UINT8         Node,
  IN       UINT8         Link,
  IN       UINT16        Register,
     OUT   UINT32        *Value,
  IN       AMD_NB_CONFIG *NbConfigPtr
  )
{
  UINT32    Data;
  PCI_ADDR  CpuPciAddress;
  UINT8     LinkId;
  LinkId = Link & 0xf;
  CpuPciAddress.AddressValue = MAKE_SBDFO (0, 0, Node, 4, 0);
  LibNbPciRMW (CpuPciAddress.AddressValue | (LinkId * 8 + 0x180), AccessWidth32, 0x0, Register | ((Register & 0xfe00)?BIT29:0), NbConfigPtr);
  do {
    LibNbPciRead (CpuPciAddress.AddressValue | (LinkId * 8 + 0x180), AccessWidth32, &Data, NbConfigPtr);
  } while ((Data & BIT31) == 0);
  LibNbPciRead (CpuPciAddress.AddressValue | (LinkId * 8 + 0x184), AccessWidth32, Value, NbConfigPtr);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Write CPU HT link Phy register
 *
 *
 *
 * @param[in] Node        Node device Address (0x18 - Node 0, 0x19 - Mode 1 etc.)
 * @param[in] Link        HT Link ID (0 - Link 0, 1 - Link 1 etc.)
 * @param[in] Register    Register address.
 * @param[in] Value       Pointer to new register value
 * @param[in] NbConfigPtr Northbridge configuration block pointer.
 */
/*----------------------------------------------------------------------------------------*/

VOID
LibNbCpuHTLinkPhyWrite (
  IN       UINT8         Node,
  IN       UINT8         Link,
  IN       UINT16        Register,
  IN       UINT32        *Value,
  IN       AMD_NB_CONFIG *NbConfigPtr
  )
{
  UINT32    Data;
  PCI_ADDR  CpuPciAddress;
  UINT8     LinkId;
  LinkId = Link & 0xf;
  CpuPciAddress.AddressValue = MAKE_SBDFO (0, 0, Node, 4, 0);
  LibNbPciWrite (CpuPciAddress.AddressValue | (LinkId * 8 + 0x184), AccessWidth32, Value, NbConfigPtr);
  LibNbPciRMW (CpuPciAddress.AddressValue | (LinkId * 8 + 0x180), AccessWidth32, 0x0, Register | BIT30 | ((Register & 0xfe00)?BIT29:0), NbConfigPtr);
  do {
    LibNbPciRead (CpuPciAddress.AddressValue | (LinkId * 8 + 0x180), AccessWidth32, &Data, NbConfigPtr);
  } while ((Data & BIT31) == 0);
}


/*----------------------------------------------------------------------------------------*/
/**
 *  Read/Modify/Write CPU HT link Phy register
 *
 *
 *
 * @param[in] Node        Node device Address (0x18 - Node 0, 0x19 - Mode 1 etc.)
 * @param[in] Link        HT Link ID (0 - Link 0, 1 - Link 1 etc.)
 * @param[in] Register    Register address.
 * @param[in] Mask        AND Mask.
 * @param[in] Data        OR Mask.
 * @param[in] NbConfigPtr Northbridge configuration block pointer.
 */
/*----------------------------------------------------------------------------------------*/
VOID
LibNbCpuHTLinkPhyRMW (
  IN       UINT8           Node,
  IN       UINT8           Link,
  IN       UINT16          Register,
  IN       UINT32          Mask,
  IN       UINT32          Data,
  IN       AMD_NB_CONFIG   *NbConfigPtr
  )
{
  UINT32  Value;
  LibNbCpuHTLinkPhyRead (Node, Link, Register, &Value, NbConfigPtr);
  Value = (Value & Mask) | Data;
  LibNbCpuHTLinkPhyWrite (Node, Link, Register, &Value, NbConfigPtr);
}


/*----------------------------------------------------------------------------------------*/
/**
 * Enable Clock Config space access.
 *    Enable access to Clock Config Space at 0:0:1 PCI address.
 *
 *
 * @param[in] pConfig Northbridge configuration block pointer.
 */
/*----------------------------------------------------------------------------------------*/

VOID
LibNbEnableClkConfig (
  IN      AMD_NB_CONFIG     *pConfig
  )
{
  LibNbPciRMW (NB_SBDFO | NB_PCI_REG4C, AccessS3SaveWidth8, (UINT32)~BIT0, BIT0, pConfig);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Disable Clock Config space access.
 *    Disable access to Clock Config Space at 0:0:1 PCI address.
 *
 *
 * @param[in] pConfig Northbridge configuration block pointer.
 */
/*----------------------------------------------------------------------------------------*/

VOID
LibNbDisableClkConfig (
  IN      AMD_NB_CONFIG     *pConfig
  )
{
  LibNbPciRMW (NB_SBDFO | NB_PCI_REG4C, AccessS3SaveWidth8, (UINT32)~BIT0, 0x0 , pConfig);
}


/*----------------------------------------------------------------------------------------*/
/**
 * Check if PCI Device Present
 *
 *
 *
 * @param[in] Device      Device PCI address.
 * @param[in] NbConfigPtr Northbridge configuration block pointer.
 *
 * @retval    TRUE        Device present.
 * @retval    FALSE       Device not present.
 */
/*----------------------------------------------------------------------------------------*/

BOOLEAN
LibNbIsDevicePresent (
  IN      PCI_ADDR          Device,
  IN      AMD_NB_CONFIG     *NbConfigPtr
  )
{
  UINT32 VendorId;
  LibNbPciRead (Device.AddressValue, AccessWidth32, &VendorId, NbConfigPtr);
  return (VendorId == 0xffffffff)?FALSE:TRUE;
}
/*----------------------------------------------------------------------------------------*/
/**
 * Check if IOMMU enabled
 *
 *
 *
 * @param[in] NbConfigPtr Northbridge configuration block pointer.
 *
 * @retval    TRUE        IOMMU not enabled.
 * @retval    FALSE       IOMMU not enabled.
 */
/*----------------------------------------------------------------------------------------*/
BOOLEAN
LibNbIsIommuEnabled (
  IN      AMD_NB_CONFIG     *NbConfigPtr
  )
{
  PCI_ADDR  IommuAddress;
  IommuAddress.AddressValue = NbConfigPtr->NbPciAddress.AddressValue;
  IommuAddress.Address.Function = 2;
  if (LibNbIsDevicePresent (IommuAddress, NbConfigPtr)) {
    UINT8 Value;
    LibNbPciRead (IommuAddress.AddressValue | 0x44, AccessWidth8, &Value, NbConfigPtr);
    if ((Value & BIT0) != 0) {
      return  TRUE;
    }
  }
  return FALSE;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Reverse bit in DWORD.
 *    Reverse bits in bitfield inside DWORD.
 *
 *
 * @param[in] Data        Value to reverse.
 * @param[in] StartBit    Start bit.
 * @param[in] StopBit     Stop bit.
 * @retval                Reversed Value.
 */
/*----------------------------------------------------------------------------------------*/

UINT32
LibNbBitReverse (
  IN    UINT32    Data,
  IN    UINT8     StartBit,
  IN    UINT8     StopBit
  )
{

  UINT32 Bitr;
  UINT32 Bitl;
  UINT32 Distance;

  while (StartBit < StopBit) {
    Bitr = Data & (1 << StartBit );
    Bitl = Data & (1 << StopBit );
    Distance =    StopBit - StartBit;
    Data = (Data & ((UINT32)~(Bitl | Bitr))) | (Bitr << Distance ) | (Bitl >> Distance);
    StartBit++;
    StopBit--;
  }
  return Data;
}
/*----------------------------------------------------------------------------------------*/
/**
 * Read CPU family
 *
 *
 *
 * @retval  0xXX00000    CPU family.
 *
 */
UINT32
LibNbGetCpuFamily (
  VOID
  )
{
  CPUID_DATA Cpuid;
  CpuidRead (0x1, &Cpuid);
  return Cpuid.EAX_Reg & 0xff00000;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Load Firmware block
 *
 *
 *
 * @param[in]   Address             Address to load firmware
 * @param[in]   Size                Firmware block size
 * @param[in]   FirmwareBlockPtr    Pointer to firmware block
 * @param[in]   NbConfigPtr         Northbridge configuration block pointer.
 *
 */
VOID
LibNbLoadMcuFirmwareBlock (
  IN      UINT16          Address,
  IN      UINT16          Size,
  IN      UINT32          *FirmwareBlockPtr,
  IN      AMD_NB_CONFIG   *NbConfigPtr
  )
{
  UINT32    i;
  PCI_ADDR  ClkPciAddress;
  UINT32    Selector;

  Selector =  (Address >=  0x200)?0x0000000:0x10000;
  ClkPciAddress = NbConfigPtr->NbPciAddress;
  ClkPciAddress.Address.Function = 1;
  LibNbEnableClkConfig (NbConfigPtr);
  for (i = 0; i < Size; i++) {
    LibNbPciIndexWrite (ClkPciAddress.AddressValue | MC_CLK_INDEX, Selector | (Address + (i * 4)), AccessWidth32, &FirmwareBlockPtr[i], NbConfigPtr);
  }
  LibNbDisableClkConfig (NbConfigPtr);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Read SMU firmware ram
 *
 *
 *
 * @param[in]   Address             Address to read
 * @param[in]   NbConfigPtr         Northbridge configuration block pointer.
 *
 */
UINT32
LibNbReadMcuRam (
  IN      UINT16          Address,
  IN      AMD_NB_CONFIG   *NbConfigPtr
  )
{
  UINT32    Value;
  PCI_ADDR  ClkPciAddress;
  UINT32    Selector;

  Selector = (Address >=  0x200) ? 0x0000000 : 0x10000;
  ClkPciAddress = NbConfigPtr->NbPciAddress;
  ClkPciAddress.Address.Function = 1;
  LibNbEnableClkConfig (NbConfigPtr);
  LibNbPciIndexRead (ClkPciAddress.AddressValue | MC_CLK_INDEX, Selector | (Address), AccessWidth32, &Value, NbConfigPtr);
  LibNbDisableClkConfig (NbConfigPtr);
  return Value;
}

/*----------------------------------------------------------------------------------------*/
/**
 * MCU Control
 *
 *
 *
 * @param[in]   Operation       Set/Reset MCU controller
 * @param[in]   NbConfigPtr     Northbridge configuration block pointer.
 */
VOID
LibNbMcuControl (
  IN      NB_MCU_MODE     Operation,
  IN      AMD_NB_CONFIG   *NbConfigPtr
  )
{
  PCI_ADDR  ClkPciAddress;
  UINT32    Value;

  Value =  (Operation == AssertReset)?0x00000ee1:0x00000ee2;
  ClkPciAddress = NbConfigPtr->NbPciAddress;
  ClkPciAddress.Address.Function = 1;
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (NbConfigPtr), CIMX_NBPCIE_TRACE), "LibNbMcuControl Operation [0x%x]\n", Operation));
  LibNbEnableClkConfig (NbConfigPtr);
  LibNbPciIndexWrite (ClkPciAddress.AddressValue | MC_CLK_INDEX, 0x00030000, AccessWidth32, &Value, NbConfigPtr);
  LibNbDisableClkConfig (NbConfigPtr);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Read/Modify/Write memory space
 *
 *
 *
 * @param[in] Address     Memory address.
 * @param[in] Width       Access width
 * @param[in] Mask        AND Mask
 * @param[in] Data        OR Mask
 * @param[in] NbConfigPtr Northbridge configuration structure pointer.
 */
/*----------------------------------------------------------------------------------------*/

VOID
LibNbMemRMW (
  IN      UINT64          Address,
  IN      ACCESS_WIDTH    Width,
  IN      UINT32          Mask,
  IN      UINT32          Data,
  IN      AMD_NB_CONFIG   *NbConfigPtr
  )
{
  UINT32  Value;
  LibNbMemRead (Address, Width, &Value, NbConfigPtr);
  Value = (Value & Mask) | Data;
  LibNbMemWrite (Address, Width, &Value, NbConfigPtr);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Read memory space
 *
 *
 *
 * @param[in] Address     Memory address.
 * @param[in] Width       Access width
 * @param[in] Value       Pointer to memory to store value
 * @param[in] NbConfigPtr Northbridge configuration structure pointer.
 */
/*----------------------------------------------------------------------------------------*/

VOID
LibNbMemRead (
  IN      UINT64          Address,
  IN      ACCESS_WIDTH    Width,
  IN      VOID            *Value,
  IN      AMD_NB_CONFIG   *NbConfigPtr
  )
{
  LibAmdMemRead (Width, Address, Value, (AMD_CONFIG_PARAMS *)((NbConfigPtr == NULL)?NULL:GET_BLOCK_CONFIG_PTR (NbConfigPtr)));
}

/*----------------------------------------------------------------------------------------*/
/**
 * Write memory space
 *
 *
 *
 * @param[in] Address     Memory address.
 * @param[in] Width       Access width
 * @param[in] Value       Pointer to memory to get value
 * @param[in] NbConfigPtr Northbridge configuration structure pointer.
 */
/*----------------------------------------------------------------------------------------*/

VOID
LibNbMemWrite (
  IN       UINT64          Address,
  IN       ACCESS_WIDTH    Width,
     OUT   VOID            *Value,
  IN       AMD_NB_CONFIG   *NbConfigPtr
  )
{
  LibAmdMemWrite (Width, Address, Value, (AMD_CONFIG_PARAMS *)((NbConfigPtr == NULL)?NULL:GET_BLOCK_CONFIG_PTR (NbConfigPtr)));
}

/*----------------------------------------------------------------------------------------*/
/**
 * Scan Pci Bridge
 *
 *
 *
 * @param[in]   This            Pointer to PCI topology scan protocol
 * @param[in]   Bridge          Address of PCI to PCI bridge to scan.
 */

SCAN_STATUS
LibNbScanPciBridgeBuses (
  IN      PCI_SCAN_PROTOCOL   *This,
  IN      PCI_ADDR            Bridge
  )
{
  SCAN_STATUS Status;
  UINT8       CurrentBus;
  UINT8       MinBus;
  UINT8       MaxBus;
  PCI_ADDR    Device;

  CIMX_ASSERT (This != NULL);
  if (This->ScanBus == NULL) {
    return  SCAN_FINISHED;
  }
  LibNbPciRead (Bridge.AddressValue | 0x19, AccessWidth8,  &MinBus, This->pConfig);
  LibNbPciRead (Bridge.AddressValue | 0x1A, AccessWidth8,  &MaxBus, This->pConfig);
  if (MinBus == 0 || MaxBus == 0) {
    return  SCAN_FINISHED;
  }
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (This->pConfig), CIMX_NBPCIE_TRACE), "    Scan bridge %d:%d:%d \n", Bridge.Address.Bus, Bridge.Address.Device, Bridge.Address.Function));
  for (CurrentBus = MinBus; CurrentBus <= MaxBus; CurrentBus++) {
    Device.AddressValue = MAKE_SBDFO (0, CurrentBus, 0, 0, 0);
    Status = This->ScanBus (This, Device);
  }
  return  SCAN_FINISHED;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Scan Pci Bus
 *
 *
 *
 * @param[in]   This            Pointer to PCI topology scan protocol
 * @param[in]   Device          Pci address device to start bus scan from
 */
/*----------------------------------------------------------------------------------------*/
SCAN_STATUS
LibNbScanPciBus (
  IN      PCI_SCAN_PROTOCOL   *This,
  IN      PCI_ADDR            Device
  )
{
  SCAN_STATUS Status;
  UINT32      CurrentDevice;
  CIMX_ASSERT (This != NULL);
  if (This->ScanDevice == NULL) {
    return  SCAN_FINISHED;
  }
  for (CurrentDevice = Device.Address.Device; CurrentDevice <= 0x1f; CurrentDevice++) {
    Device.Address.Device = CurrentDevice;
    if (LibNbIsDevicePresent (Device, This->pConfig)) {
      Status = This->ScanDevice (This, Device);
      if (Status == SCAN_STOP_BUS_ENUMERATION) {
        return Status;
      }

    }
  }
  return  SCAN_FINISHED;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Scan Pci Device
 *
 *
 *
 * @param[in]   This            Pointer to PCI topology scan protocol
 * @param[in]   Device          Pci address device to scan
 */
/*----------------------------------------------------------------------------------------*/

SCAN_STATUS
LibNbScanPciDevice (
  IN      PCI_SCAN_PROTOCOL   *This,
  IN      PCI_ADDR            Device
  )
{
  SCAN_STATUS Status;
  UINT8       Header;
  UINT32      CurrentFunction;
  UINT32      MaxFunction;
  CIMX_ASSERT (This != NULL);
  if (This->ScanFunction == NULL) {
    return  SCAN_FINISHED;
  }
  LibNbPciRead (Device.AddressValue | 0x0E , AccessWidth8, &Header, This->pConfig);
  MaxFunction = (Header & 0x80)?7:0;
  for (CurrentFunction = Device.Address.Function; CurrentFunction <= MaxFunction; CurrentFunction++) {
    Device.Address.Function = CurrentFunction;
    if (LibNbIsDevicePresent (Device, This->pConfig)) {
      CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (This->pConfig), CIMX_NBPCIE_TRACE), "    Scan function %d:%d:%d \n", Device.Address.Bus, Device.Address.Device, Device.Address.Function));
      Status = This->ScanFunction (This, Device);
      if (Status == SCAN_STOP_DEVICE_ENUMERATION || Status == SCAN_STOP_BUS_ENUMERATION) {
        return Status;
      }
    }
  }
  return  SCAN_FINISHED;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Set default Indexes
 *
 *
 * @param[in]   NbConfigPtr     Northbridge configuration block pointer.
 */
/*----------------------------------------------------------------------------------------*/

VOID
LibNbSetDefaultIndexes (
  IN      AMD_NB_CONFIG   *NbConfigPtr
  )
{
  PCI_ADDR  PciAddress;
  PORT      PortId;
  LibNbPciRMW (NbConfigPtr->NbPciAddress.AddressValue | NB_HTIU_INDEX, AccessWidth32, 0x0, 0x0, NbConfigPtr);
  LibNbPciRMW (NbConfigPtr->NbPciAddress.AddressValue | NB_MISC_INDEX, AccessWidth32, 0x0, 0x0, NbConfigPtr);
  LibNbPciRMW (NbConfigPtr->NbPciAddress.AddressValue | NB_BIF_INDEX,  AccessWidth32, 0x0, SB_CORE, NbConfigPtr);
  PciAddress.AddressValue = NbConfigPtr->NbPciAddress.AddressValue;
  for (PortId = MIN_PORT_ID; PortId <= MAX_PORT_ID; PortId++) {
    PciAddress.Address.Device = PortId;
    LibNbPciRMW (PciAddress.AddressValue | NB_BIF_INDEX,  AccessWidth32, 0x0, 0x0, NbConfigPtr);
  }
}
