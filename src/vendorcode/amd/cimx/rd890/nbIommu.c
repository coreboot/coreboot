/**
 * @file
 *
 * Routines for IOMMU.
 *
 * Implement the IOMMU init and ACPI feature.
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
#include "amdSbLib.h"

#define Int32FromChar(a,b,c,d) ((a) << 0 | (b) << 8 | (c) << 16 | (d) << 24)

/*----------------------------------------------------------------------------------------
 *                             R D 8 9 0 / S   D A T A
 *----------------------------------------------------------------------------------------
 */

// IOMMU Initialization

INDIRECT_REG_ENTRY
CONST
STATIC
IommuL1Table[] = {
  // 01.  0x0C [30:28]=7  L1VirtOrderQueues     Increase maximum number virtual queues
  //                                            for all devices
  { L1CFG_SEL_WR_EN | L1CFG_SEL_GPP1  | L1REG_0C, 0x8FFFFFFF, 0x70000000 },
  { L1CFG_SEL_WR_EN | L1CFG_SEL_GPP2  | L1REG_0C, 0x8FFFFFFF, 0x70000000 },
  { L1CFG_SEL_WR_EN | L1CFG_SEL_SB    | L1REG_0C, 0x8FFFFFFF, 0x70000000 },
  { L1CFG_SEL_WR_EN | L1CFG_SEL_GPP3A | L1REG_0C, 0x8FFFFFFF, 0x70000000 },
  { L1CFG_SEL_WR_EN | L1CFG_SEL_GPP3B | L1REG_0C, 0x8FFFFFFF, 0x70000000 },
  { L1CFG_SEL_WR_EN | L1CFG_SEL_VC1   | L1REG_0C, 0x8FFFFFFF, 0x70000000 },
  // 02.  0x07 [11]        L1DEBUG_1            Multiple error logs possible
  { L1CFG_SEL_WR_EN | L1CFG_SEL_GPP1  | L1REG_07, (UINT32)~(BIT8 + BIT9 + BIT10), BIT11 + BIT5 },
  { L1CFG_SEL_WR_EN | L1CFG_SEL_GPP2  | L1REG_07, (UINT32)~(BIT8 + BIT9 + BIT10), BIT11 + BIT5},
  { L1CFG_SEL_WR_EN | L1CFG_SEL_SB    | L1REG_07, (UINT32)~(BIT8 + BIT9 + BIT10), BIT11 + BIT5},
  { L1CFG_SEL_WR_EN | L1CFG_SEL_GPP3A | L1REG_07, (UINT32)~(BIT8 + BIT9 + BIT10), BIT11 + BIT5},
  { L1CFG_SEL_WR_EN | L1CFG_SEL_GPP3B | L1REG_07, (UINT32)~(BIT8 + BIT9 + BIT10), BIT11 + BIT5},
  { L1CFG_SEL_WR_EN | L1CFG_SEL_VC1   | L1REG_07, (UINT32)~(BIT8 + BIT9 + BIT10), BIT11 + BIT5},
  // 02.  0x06 [0]        L1DEBUG_0             Phantom function disable
  { L1CFG_SEL_WR_EN | L1CFG_SEL_GPP1  | L1REG_06, (UINT32)~BIT0, 0 },
  { L1CFG_SEL_WR_EN | L1CFG_SEL_GPP2  | L1REG_06, (UINT32)~BIT0, 0 },
  { L1CFG_SEL_WR_EN | L1CFG_SEL_SB    | L1REG_06, (UINT32)~BIT0, 0 },
  { L1CFG_SEL_WR_EN | L1CFG_SEL_GPP3A | L1REG_06, (UINT32)~BIT0, 0 },
  { L1CFG_SEL_WR_EN | L1CFG_SEL_GPP3B | L1REG_06, (UINT32)~BIT0, 0 },
  { L1CFG_SEL_WR_EN | L1CFG_SEL_VC1   | L1REG_06, (UINT32)~BIT0, 0 }
};

INDIRECT_REG_ENTRY
CONST
STATIC
IommuL2Table[] = {
  // 01.  0x0C [29]=1    IFifoClientPriority    Set attribute to VC1 L1 client high priority
  { L2CFG_SEL_WR_EN | L2REG_0C,    0xD0000000,  0x20000000 },
  // 02.  0x10 [9:8]=2   DTCInvalidationSel     DTC cache invalidation sequential precise
  { L2CFG_SEL_WR_EN | L2REG_10,    0xFFFFFC00,  0x00000200 },
  // 03.  0x14 [9:8]=2   ITCInvalidationSel     ... cache invalidation sequential precise
  { L2CFG_SEL_WR_EN | L2REG_14,    0xFFFFFC00,  0x00000200 },
  // 04.  0x18 [9:8]=2   IPTCAInvalidationSel   ... cache invalidation sequential precise
  { L2CFG_SEL_WR_EN | L2REG_18,    0xFFFFFC00,  0x00000200 },
  // 05.  0x1C [9:8]=2   IPTCBInvalidationSel   ... cache invalidation sequential precise
  { L2CFG_SEL_WR_EN | L2REG_1C,    0xFFFFFC00,  0x00000200 },
  // 06.  0x50 [9:8]=2   PDCInvalidationSel     ... cache invalidation sequential precise
  { L2CFG_SEL_WR_EN | L2REG_50,    0xFFFFFC00,  0x00000200 },
  // 07.  0x10 [4]=1     DTCParityEn            DTC cache parity protection
  { L2CFG_SEL_WR_EN | L2REG_10,    (UINT32)~BIT4,       BIT4       },
  // 08.  0x14 [4]=1     ITCParityEn            ... cache parity protection
  { L2CFG_SEL_WR_EN | L2REG_14,    (UINT32)~BIT4,       BIT4       },
  // 09.  0x18 [4]=1     PTCAParityEn           ... cache parity protection
  { L2CFG_SEL_WR_EN | L2REG_18,    (UINT32)~BIT4,       BIT4       },
  // 10. 0x1C [4]=1      PTCBParityEn           ... cache parity protection
  { L2CFG_SEL_WR_EN | L2REG_1C,    (UINT32)~BIT4,       BIT4       },
  // 11. 0x50 [4]=1      PDCParityEn            ... cache parity protection
  { L2CFG_SEL_WR_EN | L2REG_50,    (UINT32)~BIT4,       BIT4       },
  // 12. 0x80 [0]=1      ERRRuleLock0           Lock fault detection rule sets
  //     0x30 [0]=1      ERRRuleLock1
  { L2CFG_SEL_WR_EN | L2REG_80,    (UINT32)~BIT0,       BIT0       },
  { L2CFG_SEL_WR_EN | L2REG_30,    (UINT32)~BIT0,       BIT0       },
  // 13. 0x56 [2]=0      L2_CP_CONTROL          Disable CP flush on invalidation
  //     0x56 [1]=1      L2_CP_CONTROL          Enable CP flush on wait
  { L2CFG_SEL_WR_EN | L2REG_56,    0xFFFFFFF9,  BIT1       },
  // A21
  { L2CFG_SEL_WR_EN | L2REG_06,    0xFFFFFFFF,  BIT6 + BIT7 + BIT5 + BIT8 },
  { L2CFG_SEL_WR_EN | L2REG_47,    0xFFFFFFFF,  BIT1 + BIT3 + BIT0 + BIT4 + BIT5 },
  { L2CFG_SEL_WR_EN | L2REG_07,    0xFFFFFFFF,  BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT8 + BIT6},

};

// IOMMU ACPI Initialization

IOMMU_IVRS_HEADER
STATIC
RD890S_DfltHeader = {
//  'SRVI',
  Int32FromChar ('S', 'R', 'V', 'I'),
  48,
  1,
  0,
  {'A', 'M', 'D', ' ', ' ', 0},
  {'R', 'D', '8', '9', '0', 'S', 0, 0},
  {'1', ' ', ' ', 0},
  {'A','M','D',' '},
  {'1', ' ', ' ', 0},
  0,
  0
};

IOMMU_EXCLUSIONTABLE
STATIC
RD890S_DfltExclusion = {
  sizeof (UINTN) + sizeof (IOMMU_EXCLUSIONRANGE) * 0,
  {{0, 0}}
};

IOMMU_DEVICELIST
STATIC
RD890S_DfltDevices = {
  (sizeof (UINT16) + sizeof (UINT16) * 12),
  {
    DEVICEID_NB,                            // Type 2 entry, Device 0, Func 0 <-- NB all functions
    DEVICEID_GPP1_0,                        // Type 2 entry, Device 2, Func 0 <-- GPP1 port 0
    DEVICEID_GPP1_1,                        // Type 2 entry, Device 3, Func 0 <-- GPP1 port 1
    DEVICEID_GPP3A_0,                       // Type 2 entry, Device 4, Func 0 <-- GPP3a port 0
    DEVICEID_GPP3A_1,                       // Type 2 entry, Device 5, Func 0 <-- GPP3a port 1
    DEVICEID_GPP3A_2,                       // Type 2 entry, Device 6, Func 0 <-- GPP3a port 2
    DEVICEID_GPP3A_3,                       // Type 2 entry, Device 7, Func 0 <-- GPP3a port 3
    DEVICEID_GPP3A_4,                       // Type 2 entry, Device 9, Func 0 <-- GPP3a port 4
    DEVICEID_GPP3A_5,                       // Type 2 entry, Device A, Func 0 <-- GPP3a port 5
    DEVICEID_GPP2_0,                        // Type 2 entry, Device B, Func 0 <-- GPP2 port 0
    DEVICEID_GPP2_1,                        // Type 2 entry, Device C, Func 0 <-- GPP2 port 1
    DEVICEID_GPP3B_0,                       // Type 2 entry, Device D, Func 0 <-- GPP3b port 0
  }
};

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
*/

// IOMMU Library

BOOLEAN
NbIommuEnabled (
  IN OUT   AMD_NB_CONFIG_BLOCK  *ConfigPtr
  );

BOOLEAN
IommuCheckEnable (
  IN       PCI_ADDR           IommuPciAddress,
  IN       AMD_NB_CONFIG      *pConfig
  );

BOOLEAN
IommuCheckHp (
  IN       UINT16             DeviceId,
  IN       AMD_NB_CONFIG      *pConfig
  );

BOOLEAN
IommuCheckPhantom (
  IN       UINT16             DeviceId,
  IN       AMD_NB_CONFIG      *pConfig
  );

UINT32
IommuGetL1 (
  IN       UINT16             DeviceId
  );

UINT8
IommuGetLog2 (
  IN       UINT32             Value
  );

VOID
IommuRecordBusDevFuncInfo (
  IN OUT  IOMMU_PCI_TOPOLOGY *PciPtr,
  IN      UINT16             DeviceId,
  IN      AMD_NB_CONFIG      *pConfig
  );


AGESA_STATUS
IommuInit (
  IN OUT   AMD_NB_CONFIG      *pConfig
  );

VOID
IommuInitL2CacheControl (
  IN       IOMMU_PCI_TOPOLOGY *PciPtr,
  IN OUT   AMD_NB_CONFIG      *pConfig
);

VOID
IommuPlaceHeader (
  IN OUT   VOID               *BufferPtr,
  IN OUT   AMD_NB_CONFIG      *pConfig
  );

VOID
IommuPlaceIvhdAndScanDevices (
  IN OUT   VOID               *BufferPtr,
  IN OUT   AMD_NB_CONFIG      *pConfig
  );

VOID
IommuPlaceIvmdAndExclusions (
  IN OUT   VOID               *BufferPtr,
  IN OUT   AMD_NB_CONFIG      *pConfig
  );

VOID
IommuIvhdNorthbridgeDevices (
  IN OUT   IOMMU_PCI_TOPOLOGY *PciPtr,
  IN OUT   IOMMU_IVHD_ENTRY   *IvhdPtr,
  IN OUT   AMD_NB_CONFIG      *pConfig
  );

VOID
IommuIvhdSouthbridgeDevices (
  IN OUT   IOMMU_PCI_TOPOLOGY *PciPtr,
  IN OUT   IOMMU_IVHD_ENTRY   *IvhdPtr,
  IN OUT   AMD_NB_CONFIG      *pConfig
  );

VOID
IommuIvhdApicsAndHpets (
  IN OUT   IOMMU_IVHD_ENTRY   *IvhdPtr,
  IN OUT   AMD_NB_CONFIG      *pConfig
  );

VOID
IommuCreateDeviceEntry (
  IN OUT   IOMMU_PCI_TOPOLOGY *PciPtr,
  IN       UINT16             DeviceId,
  IN OUT   IOMMU_IVHD_ENTRY   *IvhdPtr,
  IN       AMD_NB_CONFIG      *pConfig
  );

VOID
IommuCreate4ByteEntry (
  IN       UINT8            Type,
  IN       UINT8            Data,
  IN       UINT16           Word1,
  IN OUT   IOMMU_IVHD_ENTRY *IvhdPtr
  );

VOID
IommuCreate8ByteEntry (
  IN       UINT8            Type,
  IN       UINT8            Data,
  IN       UINT16           Word1,
  IN       UINT8            Byte4,
  IN       UINT16           Word5,
  IN       UINT8            Byte7,
  IN OUT   IOMMU_IVHD_ENTRY *IvhdPtr
  );

VOID
IommuFinalizeIvrs (
  IN OUT   VOID               *BufferPtr,
  IN       AMD_NB_CONFIG      *pConfig
  );

// IOMMU ACPI Final

UINT64
IommuGetApicBaseAddress (
  IN       VOID               *DevicePtr,
  IN       AMD_NB_CONFIG      *pConfig
  );

UINT8
IommuGetApicId (
  IN       UINT64             BaseAddress,
  IN       VOID               *MadtPtr,
  IN       AMD_NB_CONFIG      *pConfig
  );

AGESA_STATUS
NbIommuHwInit (
  IN OUT   AMD_NB_CONFIG_BLOCK  *ConfigPtr
  );

AGESA_STATUS
NbIommuHwTopologyInit (
  IN OUT   AMD_NB_CONFIG_BLOCK  *ConfigPtr
  );

AGESA_STATUS
IommuTopologyInit (
  IN OUT   AMD_NB_CONFIG  *pConfig
  );

/*----------------------------------------------------------------------------------------
 *                             E X P O R T E D
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------*/
/**
 * Check if IOMMU enable on platform
 *
 * @param[in] ConfigPtr   Northbridges configuration block pointer.
 * @retval    AGESA_SUCCESS     IOMMU initialized and table created
 * @retval    AGESA_UNSUPPORTED IOMMU not enabled or not found
 * @retval    AGESA_ERROR       IOMMU initialization failed.
 *
 */
BOOLEAN
NbIommuEnabled (
  IN OUT   AMD_NB_CONFIG_BLOCK  *ConfigPtr
  )
{
  UINT8             NorthbridgeId;
  BOOLEAN           Result;
  Result = FALSE;
  for (NorthbridgeId = 0; NorthbridgeId <= ConfigPtr->NumberOfNorthbridges; NorthbridgeId++) {
    if (ConfigPtr->Northbridges[NorthbridgeId].pNbConfig->IommuBaseAddress != 0) {
      Result = TRUE;
      break;
    }
  }
  return Result;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Northbridge Iommu Initialization for all NB in system.
 *
 * @param[in] ConfigPtr   Northbridges configuration block pointer.
 * @retval    AGESA_SUCCESS     IOMMU initialized and table created
 * @retval    AGESA_UNSUPPORTED IOMMU not enabled or not found
 * @retval    AGESA_ERROR       IOMMU initialization failed.
 *
 */
AGESA_STATUS
NbIommuInit (
  IN OUT   AMD_NB_CONFIG_BLOCK  *ConfigPtr
  )
{
  AGESA_STATUS      Status;

  Status = AGESA_SUCCESS;

  CIMX_TRACE ((TRACE_DATA (ConfigPtr, CIMX_NB_TRACE), "[NBIOMMU]NbIommuInit Enter\n"));

  if (NbIommuEnabled (ConfigPtr)) {
    NbIommuHwInit (ConfigPtr);
    NbIommuAcpiInit (ConfigPtr);
    NbIommuHwTopologyInit (ConfigPtr);
  } else {
    Status = AGESA_UNSUPPORTED;
  }

  CIMX_TRACE ((TRACE_DATA (ConfigPtr, CIMX_NB_TRACE), "[NBIOMMU]NbIommuInit Exit [Status = 0x%x]\n", Status));
  return Status;
}




/*----------------------------------------------------------------------------------------*/
/**
 * Northbridge Iommu Initialization for all NB in system.
 *
 * @param[in] ConfigPtr   Northbridges configuration block pointer.
 * @retval    AGESA_SUCCESS     IOMMU initialized and table created
 * @retval    AGESA_UNSUPPORTED IOMMU not enabled or not found
 * @retval    AGESA_ERROR       IOMMU initialization failed.
 *
 */
AGESA_STATUS
NbIommuInitS3 (
  IN OUT   AMD_NB_CONFIG_BLOCK  *ConfigPtr
  )
{
  AGESA_STATUS      Status;

  Status = AGESA_SUCCESS;
  CIMX_TRACE ((TRACE_DATA (ConfigPtr, CIMX_NB_TRACE), "[NBIOMMU]NbIommuInitS3 Enter\n"));

  if (NbIommuEnabled (ConfigPtr)) {
    NbIommuHwInit (ConfigPtr);
    NbIommuHwTopologyInit (ConfigPtr);
  } else {
    Status = AGESA_UNSUPPORTED;
  }
  CIMX_TRACE ((TRACE_DATA (ConfigPtr, CIMX_NB_TRACE), "[NBIOMMU]NbIommuInitS3 Exit [Status = 0x%x]\n", Status));
  return Status;
}




/*----------------------------------------------------------------------------------------*/
/**
 * Northbridge Iommu HW Initialization for all NB in system.
 *
 * @param[in] ConfigPtr   Northbridges configuration block pointer.
 * @retval    AGESA_SUCCESS     IOMMU initialized and table created
 * @retval    AGESA_ERROR       IOMMU initialization failed.
 *
 */
AGESA_STATUS
NbIommuHwInit (
  IN OUT   AMD_NB_CONFIG_BLOCK  *ConfigPtr
  )
{
  UINT8             NorthbridgeId;
  AMD_NB_CONFIG     *pConfig;

  for (NorthbridgeId = 0; NorthbridgeId <= ConfigPtr->NumberOfNorthbridges; NorthbridgeId++) {
    pConfig = &ConfigPtr->Northbridges[NorthbridgeId];
    ConfigPtr->CurrentNorthbridge = NorthbridgeId;
    IommuInit (pConfig);
  }
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Northbridge Iommu HW Initialization for all NB in system.
 *
 * @param[in] ConfigPtr   Northbridges configuration block pointer.
 * @retval    AGESA_SUCCESS     IOMMU initialized and table created
 * @retval    AGESA_ERROR       IOMMU initialization failed.
 *
 */
AGESA_STATUS
NbIommuHwTopologyInit (
  IN OUT   AMD_NB_CONFIG_BLOCK  *ConfigPtr
  )
{
  UINT8             NorthbridgeId;
  AMD_NB_CONFIG     *pConfig;

  for (NorthbridgeId = 0; NorthbridgeId <= ConfigPtr->NumberOfNorthbridges; NorthbridgeId++) {
    pConfig = &ConfigPtr->Northbridges[NorthbridgeId];
    ConfigPtr->CurrentNorthbridge = NorthbridgeId;
    IommuTopologyInit (pConfig);
  }
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Northbridge Iommu Initialization for all NB in system.
 *
 * @param[in] ConfigPtr   Northbridges configuration block pointer.
 * @retval    AGESA_SUCCESS     IOMMU initialized and table created
 * @retval    AGESA_UNSUPPORTED IOMMU not enabled or not found
 * @retval    AGESA_ERROR       IOMMU initialization failed.
 *
 */
AGESA_STATUS
NbIommuAcpiInit (
  IN OUT   AMD_NB_CONFIG_BLOCK  *ConfigPtr
  )
{
  UINT8             NorthbridgeId;
  AGESA_STATUS      Status;
  AMD_NB_CONFIG     *pConfig;
  NB_BUFFER_PARAMS  Ivrs;
  UINTN             IvrsHandle;

  Status = AGESA_SUCCESS;
  Ivrs.BufferLength = 0;
  Ivrs.BufferHandle = IVRS_HANDLE;
  Ivrs.BufferPtr = NULL;
  pConfig = &ConfigPtr->Northbridges[0];
  ConfigPtr->CurrentNorthbridge = 0;
  IvrsHandle = 0;

  CIMX_TRACE ((TRACE_DATA (ConfigPtr, CIMX_NB_TRACE), "[NBIOMMU]NbIommuAcpiInit Enter\n"));

  // Get a buffer for IVRS
  Ivrs.BufferLength = IVRS_BUFFER_SIZE;
  Status = LibNbCallBack (PHCB_AmdAllocateBuffer, (UINTN)&Ivrs, &ConfigPtr->Northbridges[0]);
  if (Status != AGESA_SUCCESS || Ivrs.BufferPtr == NULL) {
    // Table creation failed
    return AGESA_ERROR;
  }

  // Clear buffer before using
  LibAmdMemFill (Ivrs.BufferPtr, 0, Ivrs.BufferLength, (AMD_CONFIG_PARAMS *)&(pConfig->sHeader));

  // PLACE OUR ACPI IVRS TABLE
  // 1. Create IVRS header
  // 2. For each northbridge place IVHD
  // 3. For northbridge 0 only, place IVMD exclusion entries
  for (NorthbridgeId = 0; NorthbridgeId <= ConfigPtr->NumberOfNorthbridges; NorthbridgeId++) {
    pConfig = &ConfigPtr->Northbridges[NorthbridgeId];
    ConfigPtr->CurrentNorthbridge = NorthbridgeId;
    if (NorthbridgeId == 0) {
      IommuPlaceHeader (Ivrs.BufferPtr, pConfig);
    }
    IommuPlaceIvhdAndScanDevices (Ivrs.BufferPtr, pConfig);
    IommuPlaceIvmdAndExclusions (Ivrs.BufferPtr, pConfig);
  }
  IommuFinalizeIvrs (Ivrs.BufferPtr, pConfig);

  LibAmdSetAcpiTable (Ivrs.BufferPtr, TRUE, &IvrsHandle);

  CIMX_TRACE ((TRACE_DATA (ConfigPtr, CIMX_NB_TRACE), "[NBIOMMU]NbIommuAcpiInit [IVRS TableAddress = 0x%x]\n", (UINT32)(Ivrs.BufferPtr)));
  CIMX_TRACE ((TRACE_DATA (ConfigPtr, CIMX_NB_TRACE), "[NBIOMMU]NbIommuAcpiInit Exit [Status = 0x%x]\n", Status));

  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Northbridge Iommu IVRS fixup for APICS
 *
 * @param[in] ConfigPtr  Northbridges configuration block pointer.
 * @retval    AGESA_SUCCESS    IOMMU initialized and table patched, or no patching required
 * @retval    AGESA_ERROR      IOMMU enabled but no previously generated IVRC table found.
 *
 */
AGESA_STATUS
NbIommuAcpiFixup (
  IN OUT   AMD_NB_CONFIG_BLOCK  *ConfigPtr
  )
{
  AGESA_STATUS  Status;
  UINT8         ApicId;
  UINT64        ApicBaseAddress;
  UINT8         NorthbridgeId;
  BOOLEAN       IommuFound;
  VOID          *DevicePtr;
  VOID          *IvhdPtr;
  VOID          *IvrsPtr;
  VOID          *MadtPtr;
  AMD_NB_CONFIG *pConfig;
  PCI_ADDR      IommuPciAddress;
  UINTN         IvrsHandle;


  pConfig = &ConfigPtr->Northbridges[0];
  IommuFound = FALSE;
  ApicId = 0xFF;
  ApicBaseAddress = 0;

  CIMX_TRACE ((TRACE_DATA (ConfigPtr, CIMX_NB_TRACE), "[NBIOMMU]NbIommuAcpiFixup Enter\n"));

  for (NorthbridgeId = 0; NorthbridgeId <= ConfigPtr->NumberOfNorthbridges; NorthbridgeId++) {
    pConfig = &ConfigPtr->Northbridges[NorthbridgeId];
    ConfigPtr->CurrentNorthbridge = NorthbridgeId;
    IommuPciAddress = pConfig->NbPciAddress;
    IommuPciAddress.Address.Function = NB_IOMMU;
    if (IommuCheckEnable (IommuPciAddress, pConfig)) {
      IommuFound = TRUE;
    }
  }

  // Any Iommus enabled?  If no, we don't need to patch anything
  if (!IommuFound) {
    return AGESA_SUCCESS;
  }

  // Check for an IVRS
  // Check IVRS for a type 10 block (IVHD)
  // Check for an MADT
  // If these conditions fail, abort

//  Status = LibAmdGetAcpiTable ('SRVI', &IvrsPtr, &IvrsHandle);
  Status = LibAmdGetAcpiTable (Int32FromChar ('S', 'R', 'V', 'I'), &IvrsPtr, &IvrsHandle);
  if (Status != AGESA_SUCCESS) {
//    REPORT_EVENT (AGESA_ERROR, GENERAL_ERROR_LOCATE_ACPI_TABLE, 'SRVI', 0, 0, 0, pConfig);
    REPORT_EVENT (AGESA_ERROR, GENERAL_ERROR_LOCATE_ACPI_TABLE, Int32FromChar ('S', 'R', 'V', 'I'), 0, 0, 0, pConfig);
    return AGESA_ERROR;
  }

//  Status = LibAmdGetAcpiTable ('CIPA', &MadtPtr, NULL);
  Status = LibAmdGetAcpiTable (Int32FromChar ('C', 'I', 'P', 'A'), &MadtPtr, NULL);
  if (Status != AGESA_SUCCESS) {
//    REPORT_EVENT (AGESA_ERROR, GENERAL_ERROR_LOCATE_ACPI_TABLE, 'CIPA', 0, 0, 0, pConfig);
    REPORT_EVENT (AGESA_ERROR, GENERAL_ERROR_LOCATE_ACPI_TABLE, Int32FromChar ('C', 'I', 'P', 'A'), 0, 0, 0, pConfig);
    return AGESA_ERROR;
  }

  IvhdPtr = LibAmdGetFirstIvrsBlockEntry (TYPE_IVHD, IvrsPtr);
  if (IvhdPtr == NULL) {
    return AGESA_ERROR;
  }

  // An IVRS can contain one or more IVHD entries (one per IOMMU)
  // Each IVHD entry can contain one or more APIC entries

  while (IvhdPtr != NULL) {
    DevicePtr = LibAmdGetFirstDeviceEntry (DE_SPECIAL, IvhdPtr);
    do {
      // Be sure to only fix APIC entries
      if (*(UINT8*) ((UINT8*)DevicePtr + DE_SPECIAL_VARIETY) == VARIETY_IOAPIC) {
        ApicBaseAddress = IommuGetApicBaseAddress (DevicePtr, pConfig);
        ApicId = IommuGetApicId (ApicBaseAddress, MadtPtr, pConfig);
        *(UINT8*)((UINT8*)DevicePtr + DE_SPECIAL_ID) = ApicId;
      }
      DevicePtr = LibAmdGetNextDeviceEntry (DE_SPECIAL, DevicePtr, IvhdPtr);
    } while (DevicePtr != NULL);

    IvhdPtr = LibAmdGetNextIvrsBlockEntry (TYPE_IVHD, IvhdPtr, IvrsPtr);
  }

  LibAmdSetAcpiTable (IvrsPtr, TRUE, &IvrsHandle);

  CIMX_TRACE ((TRACE_DATA (ConfigPtr, CIMX_NB_TRACE), "[NBIOMMU]NbIommuAcpiFixup [IVRS TableAddress = 0x%x]\n", (UINT32)IvrsPtr));
  CIMX_TRACE ((TRACE_DATA (ConfigPtr, CIMX_NB_TRACE), "[NBIOMMU]NbIommuAcpiFixup [APIC TableAddress = 0x%x]\n", (UINT32)MadtPtr));
  CIMX_TRACE ((TRACE_DATA (ConfigPtr, CIMX_NB_TRACE), "[NBIOMMU]NbIommuAcpiFixup Exit\n"));

  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------
 *                             P R I V A T E
 *----------------------------------------------------------------------------------------
 */
UINT32  IommuMmioInitTable[] = {
  0x8,    0x0,
  0xC,    0x08000000,
  0x10,   0x0,
  0x14,   0x08000000,
  0x2000, 0x0,
  0x2008, 0x0
};

/*----------------------------------------------------------------------------------------*/
/**
 * Nb Iommu Initialization.
 *
 * @param[in] pConfig       Northbridge configuration pointer
 * @retval    AGESA_SUCCESS     IOMMU enable and initialized succesfully.
 * @retval    AGESA_UNSUPPORTED IOMMU not initialized.
 */
AGESA_STATUS
IommuInit (
  IN OUT   AMD_NB_CONFIG  *pConfig
  )
{
  UINT8     CapBase;
  PCI_ADDR  IommuPciAddress;
  UINTN     i;

  IommuPciAddress = pConfig->NbPciAddress;
  IommuPciAddress.Address.Function = NB_IOMMU;

  // If the base address = 0, don't enable IOMMU
  if (pConfig->pNbConfig->IommuBaseAddress == 0) {
    return AGESA_UNSUPPORTED;
  }

  // NBMISCIND:0x75 IOC_FEATURE_CNTL_10_0[10]=1
  // 0=disable
  // 1=enable

  LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG75, AccessS3SaveWidth32, (UINT32)~(BIT28), BIT0, pConfig);

  // Get Capabilities pointer 32h (points to 40h) - capability ID 0x0F.  Not found, we have no IOMMU.
  CapBase = LibNbFindPciCapability (IommuPciAddress.AddressValue, IOMMU_CAPID, pConfig);
  if (CapBase == 0) {
    return AGESA_UNSUPPORTED;
  }

  // IOMMU_ADAPTER_ID_W - RW - 32 bits - nbconfigfunc2:0x68
  // SUBSYSTEM_VENDOR_ID_W 15:0 0x0 Sets the subsystem vendor ID register header
  // SUBSYSTEM_ID_W 31:16 0x0 Sets the subsystem ID register in the configuration header
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NB_TRACE), "[NBIOMMU]Iommu Device Found [PCI Address = 0x%x]\n", IommuPciAddress.AddressValue));
  if (pConfig->pNbConfig->SSID == PCI_INVALID) {
    LibNbPciRead (IommuPciAddress.AddressValue, AccessWidth32, &pConfig->pNbConfig->SSID, pConfig);
  }
  if (pConfig->pNbConfig->SSID) {
    LibNbPciWrite (IommuPciAddress.AddressValue | 0x68, AccessS3SaveWidth32, &pConfig->pNbConfig->SSID, pConfig);
  }

  // Get Capabilities pointer 32h (points to 40h) - capability ID 0x0F
  // Set Cap_Offset+04h [31:14] Base Address Low [31:14]
  // Set Cap_Offset+08h [31:0] Base Address High [64:32]
  // Set Cap_Offset+04h [0] Enable
  LibNbPciRMW ((IommuPciAddress.AddressValue | (CapBase + 8)), AccessS3SaveWidth32, 0x0, ((UINT32*)&pConfig->pNbConfig->IommuBaseAddress)[1], pConfig);
  LibNbPciRMW ((IommuPciAddress.AddressValue | (CapBase + 4)), AccessS3SaveWidth32, 0x0, ((UINT32*)&pConfig->pNbConfig->IommuBaseAddress)[0], pConfig);

  // Enable zeroing of address for zero-byte reads when IOMMU enabled
  LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG01, AccessS3SaveWidth32, (UINT32)~(BIT8 | BIT9), BIT8, pConfig);

  // 8.3.1 L1 Initialization
  LibNbIndirectTableInit (IommuPciAddress.AddressValue | L1CFG_INDEX,
    0,
    (INDIRECT_REG_ENTRY*)FIX_PTR_ADDR(&IommuL1Table[0],NULL),
    (sizeof (IommuL1Table) / sizeof (INDIRECT_REG_ENTRY)),
    pConfig
    );

  // 8.3.3.1 L2 Common Initialization
  LibNbIndirectTableInit (IommuPciAddress.AddressValue | L2CFG_INDEX,
    0,
    (INDIRECT_REG_ENTRY*)FIX_PTR_ADDR(&IommuL2Table[0], NULL),
    (sizeof (IommuL2Table) / sizeof (INDIRECT_REG_ENTRY)),
    pConfig
    );
  //Configure PDC cache to 12-way set associative cache for A21
  if (LibNbGetRevisionInfo (pConfig).Revision > NB_REV_A11) {
    LibNbPciIndexRMW (IommuPciAddress.AddressValue | L2CFG_INDEX, L2CFG_SEL_WR_EN | L2REG_52, AccessS3SaveWidth32, 0x0, 0xF0000002 , pConfig);
  }
  // Start and lock the Iommu settings
  LibNbPciRMW ((IommuPciAddress.AddressValue | (CapBase + 4)), AccessS3SaveWidth32, 0xFFFFFFFF, (UINT32)BIT0, pConfig);

  //Reset IOMMU MMIO registers on system reset
  for (i = 0; i < (sizeof (IommuMmioInitTable) / sizeof (UINT32)); i = i + 2) {
    LibNbMemRMW (pConfig->pNbConfig->IommuBaseAddress + IommuMmioInitTable[i], AccessS3SaveWidth32, 0x0, IommuMmioInitTable[i + 1], pConfig);
  }
  return AGESA_SUCCESS;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Iommu Initialization of topology specific data.
 *
 * @param[in] pConfig       Northbridge configuration pointer
 * @retval    AGESA_SUCCESS     IOMMU enable and initialized succesfully.
 * @retval    AGESA_UNSUPPORTED IOMMU not initialized.
 */
AGESA_STATUS
IommuTopologyInit (
  IN OUT   AMD_NB_CONFIG  *pConfig
  )
{
  // Set L2 Caches Hash Control based on maximum bus, device, function
  IommuInitL2CacheControl ((IOMMU_PCI_TOPOLOGY*) &pConfig->pNbConfig->IommuTpologyInfo, pConfig);
  return AGESA_SUCCESS;
}

L2_HASH_CONTROL HashControls[] = {
  {
    L2_DTC_CONTROL
  },
  {
    L2_ITC_CONTROL
  },
  {
    L2_PTC_A_CONTROL
  },
  {
    L2_PTC_B_CONTROL
  },
  {
    L2_PDC_CONTROL
  }
};

/*----------------------------------------------------------------------------------------*/
/**
 * Set L2 Cache Hash Control based on maximum Bus, Dev, Function found
 *
 * @param[in] PciPtr            Array of bus, device, function
 * @param[in] pConfig           Northbridge configuration structure pointer.
 */
VOID
IommuInitL2CacheControl (
  IN       IOMMU_PCI_TOPOLOGY *PciPtr,
  IN OUT   AMD_NB_CONFIG      *pConfig
  )
{
  UINT32    Value;
  PCI_ADDR  IommuPciAddress;
  UINTN     i;
  UINT8     FuncBitsUsed;
  UINT8     DevBitsUsed;
  UINT8     BusBitsUsed;

  IommuPciAddress = pConfig->NbPciAddress;
  IommuPciAddress.Address.Function = NB_IOMMU;

  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NB_TRACE), "    L2Cache Init Max Bus = 0x%x Max Device = 0x%x Mux Func = 0x%x\n", PciPtr->MaxBus, PciPtr->MaxDevice, PciPtr->MaxFunction));

  FuncBitsUsed = CIMX_MAX (IommuGetLog2 (PciPtr->MaxFunction + 1), 3);
  DevBitsUsed = IommuGetLog2 (PciPtr->MaxDevice + 1);
  BusBitsUsed = IommuGetLog2 (PciPtr->MaxBus + 1);

  for (i = 0; i < (sizeof (HashControls) / sizeof (L2_HASH_CONTROL)); i++) {
    UINT8 NBits;
    UINT8 NFuncBits;
    UINT8 NDevBits;
    UINT8 NBusBits;
    LibNbPciIndexRead (IommuPciAddress.AddressValue | L2CFG_INDEX, L2CFG_SEL_WR_EN | HashControls[i].HashControl, AccessWidth32, &Value, pConfig);
    NBits = (UINT8) (Value >> 28) - IommuGetLog2 ((Value >> 16) & 0xff);
    NFuncBits = CIMX_MIN (NBits, 0x3);
    NBits = NBits - NFuncBits;
    NDevBits = CIMX_MIN ( NBits, DevBitsUsed + FuncBitsUsed - NFuncBits);
    NBusBits = NBits - NDevBits;
    CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NB_TRACE), "    NBusBits = %d, NDevBits = %d, NFuncBits = %d  \n", NBusBits, NDevBits, NFuncBits));
    LibNbPciIndexRMW (
      IommuPciAddress.AddressValue | L2CFG_INDEX,
      L2CFG_SEL_WR_EN | (HashControls[i].HashControl + 1),
      AccessS3SaveWidth32,
      0xFFFFFE00,
      (NFuncBits | (NDevBits << 2) | (NBusBits << 5)) & 0x1FF,
      pConfig
      );
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Check to see if current PCI Address is an IOMMU
 *
 * @param[in] IommuPciAddress   PCI Address to check
 * @param[in] pConfig           Northbridge configuration structure pointer.
 * @retval                      TRUE if Iommu is enabled and found
 */
BOOLEAN
IommuCheckEnable (
  IN      PCI_ADDR       IommuPciAddress,
  IN      AMD_NB_CONFIG  *pConfig
  )
{
  UINT8 CapBase;

  if (pConfig->pNbConfig->IommuBaseAddress == 0x0) {
    return FALSE;
  }
  CapBase = LibNbFindPciCapability (IommuPciAddress.AddressValue, IOMMU_CAPID, pConfig);
  if (CapBase == 0) {
    return FALSE;
  } else {
    return TRUE;
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Check an RD890 PCIE bridge to see if hot plug is enabled
 *
 * @param[in] DeviceId  Device Id to check
 * @param[in] pConfig   Northbridge configuration structure pointer.
 * @retval    TRUE      if current device supports hotplug
 */
BOOLEAN
IommuCheckHp (
  IN      UINT16         DeviceId,
  IN      AMD_NB_CONFIG  *pConfig
  )
{
  UINT32    PciData;
  PCI_ADDR  PciAddress;

  PciAddress.AddressValue = MAKE_SBDFO (0, (DeviceId >> 8) & 0xFF, (DeviceId >> 3) & 0x1F, DeviceId & 0x7, 0);

  LibNbPciRead (PciAddress.AddressValue | NB_PCIP_REG6C, AccessWidth32, &PciData, pConfig);

  // Check for hot plug by reading PCIE_SLOT_CAP pcieConfigDev[13:2]:0x6C [6] HOTPLUG_CAPABLE
  PciData &= BIT6;
  if (PciData != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Check a PCIE device to see if it supports phantom functions
 *
 * @param[in] DeviceId  Device Id to check
 * @param[in] pConfig   Northbridge configuration structure pointer.
 * @retval    TRUE      if current device supports phantom functions
 */
BOOLEAN
IommuCheckPhantom (
  IN      UINT16         DeviceId,
  IN      AMD_NB_CONFIG  *pConfig
  )
{
  UINT32    PciData;
  UINT8     PcieCapBase;
  PCI_ADDR  PciAddress;

  PciAddress.AddressValue = MAKE_SBDFO (0, (DeviceId >> 8) & 0xFF, (DeviceId >> 3) & 0x1F, DeviceId & 0x7, 0);

  // Check for phantom functions by reading PCIE Device Capabilities register (base + 4) [4:3] 0 = not supported
  PcieCapBase = LibNbFindPciCapability (PciAddress.AddressValue, PCIE_CAPID, pConfig);
  if (PcieCapBase != 0) {
    LibNbPciRead (((PciAddress.AddressValue) | (PcieCapBase + 4)), AccessWidth32, &PciData, pConfig);
    PciData &= PCIE_PHANTOMMASK;
    if (PciData != 0) {
      return TRUE;
    }
  }
  return FALSE;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Check to see if current PCI Address is a multi-port PCIE core
 *
 * @param[in] DeviceId  16-bit device id to check
 * @retval              L1 configuration select
 */
UINT32
IommuGetL1 (
  IN       UINT16 DeviceId
  )
{
  // This function translates an RD890 multi-port pci core to the offset of the L1 entry
  // corresponding to it.  An unknown device returns as invalid
  switch (DeviceId) {
  case DEVICEID_GPP1_0 :
    return L1CFG_SEL_GPP1;
  case DEVICEID_GPP1_1 :
    return L1CFG_SEL_GPP1;
  case DEVICEID_GPP2_0 :
    return L1CFG_SEL_GPP2;
  case DEVICEID_GPP2_1 :
    return L1CFG_SEL_GPP2;
  case DEVICEID_GPP3A_0 :
    return L1CFG_SEL_GPP3A;
  case DEVICEID_GPP3A_1 :
    return L1CFG_SEL_GPP3A;
  case DEVICEID_GPP3A_2 :
    return L1CFG_SEL_GPP3A;
  case DEVICEID_GPP3A_3 :
    return L1CFG_SEL_GPP3A;
  case DEVICEID_GPP3A_4 :
    return L1CFG_SEL_GPP3A;
  case DEVICEID_GPP3A_5 :
    return L1CFG_SEL_GPP3A;
  case DEVICEID_GPP3B_0 :
    return L1CFG_SEL_GPP3B;
  default:
    return PCI_INVALID;
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Place IVHD Device Entries
 *
 * @param[in] Value Value to find the logarithm of
 * @retval          Logarithm of input Value
 */
UINT8
IommuGetLog2 (
  IN       UINT32 Value
  )
{
  UINT8 Result;

  Result = 0;

  // This code will round a 32bit value to the next highest power of 2
  Value--;
  Value |= Value >> 1;
  Value |= Value >> 2;
  Value |= Value >> 4;
  Value |= Value >> 8;
  Value |= Value >> 16;
  Value++;

  // Calculate the logarithm
  while (Value >>= 1) {
    Result++;
  }

  return Result;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Place IVRS Header for IOMMU ACPI table
 *
 * @param[in, out]  BufferPtr   Pointer to buffer to return IVRS.
 * @param[in]       pConfig     Northbridge configuration structure pointer.
 */
VOID
IommuPlaceHeader (
  IN OUT   VOID           *BufferPtr,
  IN OUT   AMD_NB_CONFIG  *pConfig
  )
{
  IOMMU_IVRS_HEADER *HeaderPtr;
  HeaderPtr = (IOMMU_IVRS_HEADER *)BufferPtr;
  LibAmdMemCopy (HeaderPtr, &RD890S_DfltHeader, sizeof (IOMMU_IVRS_HEADER), (AMD_CONFIG_PARAMS *)&(pConfig->sHeader));
}

/*----------------------------------------------------------------------------------------*/
/**
 * Place IVMD (memory device) Create all IVMD entries for a single exclusion table
 *
 * @param[in, out]  BufferPtr Pointer to text buffer to return IVRS
 * @param[in]       pConfig   Northbridge configuration pointer
 */
VOID
IommuPlaceIvmdAndExclusions (
  IN OUT   VOID           *BufferPtr,
  IN OUT   AMD_NB_CONFIG  *pConfig
  )
{
  UINT8                 EntryCount;
  UINT8                 CurrentExclusion;
  AGESA_STATUS          Status;
  IOMMU_EXCLUSIONTABLE  *pExclusion;
  IOMMU_IVRS_HEADER     *HeaderPtr;
  IOMMU_IVMD_ENTRY      *IvmdPtr;

  pExclusion = &RD890S_DfltExclusion;
  HeaderPtr = (IOMMU_IVRS_HEADER *)BufferPtr;
  IvmdPtr = (IOMMU_IVMD_ENTRY *)BufferPtr;

  Status = LibNbCallBack (PHCB_AmdGetExclusionTable, (UINTN)&pExclusion, pConfig);
  if (Status == AGESA_SUCCESS) {
    EntryCount = (UINT8) ((pExclusion->TableLength - sizeof (UINTN)) / sizeof (IOMMU_EXCLUSIONRANGE));
    for (CurrentExclusion = 0; CurrentExclusion < EntryCount; CurrentExclusion++) {
      IvmdPtr = (IOMMU_IVMD_ENTRY*) ((UINT8*)HeaderPtr + HeaderPtr->Length);
      IvmdPtr->Type = TYPE_IVMD_ALL;    // 20h = All peripherals
      IvmdPtr->Flags = 0x7;             // Exclusion range
      IvmdPtr->Length = 32;             // 32 byte structure
      IvmdPtr->DeviceId = 0;            // Reserved for type 20h
      IvmdPtr->AuxData = 0;             // Reserved for type 20h
      IvmdPtr->Reserved = 0;
      IvmdPtr->BlockStartAddress = pExclusion->ExclusionRange[CurrentExclusion].Start;
      IvmdPtr->BlockLength = pExclusion->ExclusionRange[CurrentExclusion].Length;
      HeaderPtr->Length += 32;          // Update size of IVRS
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Place IVHD (hardware device) Entry for IOMMU ACPI table
 *
 * @param[in, out]  BufferPtr Pointer to text buffer to return IVRS
 * @param[in]       pConfig   Northbridge configuration pointer
 */
VOID
IommuPlaceIvhdAndScanDevices (
  IN OUT   VOID           *BufferPtr,
  IN OUT   AMD_NB_CONFIG  *pConfig
  )
{
  UINT32              Value;
  IOMMU_PCI_TOPOLOGY  PciFlags;
  IOMMU_IVRS_HEADER   *HeaderPtr;
  IOMMU_IVHD_ENTRY    *IvhdPtr;
  PCI_ADDR            IommuPciAddress;

  HeaderPtr = (IOMMU_IVRS_HEADER *)BufferPtr;
  IvhdPtr = (IOMMU_IVHD_ENTRY *)BufferPtr;
  //PciFlags.PhantomFunction = FALSE;
  PciFlags.MaxBus = 0;
  PciFlags.MaxDevice = 0;
  PciFlags.MaxFunction = 0;
  IommuPciAddress = pConfig->NbPciAddress;
  IommuPciAddress.Address.Function = NB_IOMMU;

  IvhdPtr = (IOMMU_IVHD_ENTRY*) ((UINT8*)HeaderPtr + HeaderPtr->Length);
  IvhdPtr->Type = TYPE_IVHD;        // Hardware block
  IvhdPtr->Flags = FLAGS_COHERENT | FLAGS_IOTLBSUP | FLAGS_ISOC | FLAGS_RESPASSPW | FLAGS_PASSPW;
  IvhdPtr->Length = 24;             // Length = 24 with no devices
  IvhdPtr->DeviceId = (UINT16)((IommuPciAddress.AddressValue >> 12) & 0xFFFF); // Change 32 bit ID into 16 bit
  IvhdPtr->CapabilityOffset = (UINT16) (LibNbFindPciCapability (IommuPciAddress.AddressValue, IOMMU_CAPID, pConfig));
  IvhdPtr->BaseAddress = pConfig->pNbConfig->IommuBaseAddress;
  IvhdPtr->PciSegment = 0;
  LibNbPciRead (IommuPciAddress.AddressValue | (IvhdPtr->CapabilityOffset + 0x10), AccessWidth32, &Value, pConfig);
  IvhdPtr->IommuInfo = (UINT16)(Value & 0x1f); //Set MSInum.
  IvhdPtr->IommuInfo |= ((0x13) << 8); //set UnitID
  IvhdPtr->Reserved = 0;

  IommuIvhdNorthbridgeDevices (&PciFlags, IvhdPtr, pConfig);
  if (IommuPciAddress.Address.Bus == 0) {
    IommuIvhdSouthbridgeDevices (&PciFlags, IvhdPtr, pConfig);
  }
  IommuIvhdApicsAndHpets (IvhdPtr, pConfig);
  pConfig->pNbConfig->IommuTpologyInfo = *((UINT32*) &PciFlags);
  HeaderPtr->Length += IvhdPtr->Length;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Place IVHD Device Entries
 *
 * @param[in, out]  PciPtr    Pci topology flags
 * @param[in, out]  IvhdPtr   Pointer to IVHD where entry is appended
 * @param[in]       pConfig   NB config block
 */
VOID
IommuIvhdNorthbridgeDevices (
  IN OUT   IOMMU_PCI_TOPOLOGY *PciPtr,
  IN OUT   IOMMU_IVHD_ENTRY   *IvhdPtr,
  IN OUT   AMD_NB_CONFIG      *pConfig
  )
{
  UINT16            CurrentDevice;
  UINT16            DeviceId;
  UINT8             EntryCount;
  IOMMU_DEVICELIST  *pDevices;
  PCI_ADDR          NbPciAddress;
  PCI_ADDR          IommuPciAddress;

  pDevices = &RD890S_DfltDevices;
  NbPciAddress = pConfig->NbPciAddress;
  IommuPciAddress = pConfig->NbPciAddress;

  IommuPciAddress.Address.Function = NB_IOMMU;
  EntryCount = (UINT8) ((pDevices->TableLength - sizeof (UINT16)) / sizeof (UINT16));

  // Run RD890S device table, fixed for current bus
  for (CurrentDevice = 0; CurrentDevice < EntryCount; CurrentDevice++) {
    DeviceId = (UINT16) (NbPciAddress.Address.Bus << 8) | pDevices->Device[CurrentDevice];
    IommuCreateDeviceEntry (PciPtr, DeviceId, IvhdPtr, pConfig);

    // CHECK HOTPLUG OR PHANTOM FUNCTION SUPPORT
    // For each device, reset PhantomEnable, but set it as a one-shot.  If any device under the northbridge PCIE bridge
    // device has phantom function support enabled, set the L1.  Additionally, check the bridge for hotplug, and set the
    // L1 if so.

    //PciPtr->PhantomFunction = FALSE;
    //if (PciPtr->PhantomFunction || IommuCheckHp (DeviceId, pConfig)) {
    //  if (IommuGetL1 (DeviceId) != PCI_INVALID && LibNbGetRevisionInfo (pConfig).Revision != NB_REV_A11) {
    //    // Determine from deviceID which L1
    //    LibNbPciIndexRMW (IommuPciAddress.AddressValue | L1CFG_INDEX, L1CFG_SEL_WR_EN | IommuGetL1 (DeviceId), AccessS3SaveWidth32, (UINT32)~BIT0, BIT0, pConfig);
    //  }
    //}
    //PciPtr->PhantomFunction = FALSE;
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Place IVHD Device Entries
 *
 * @param[in, out]  PciPtr        PCI topology flags
 * @param[in, out]  IvhdPtr       Pointer to IVHD where entry is appended
 * @param[in]       pConfig       NB config structute
 */
VOID
IommuIvhdSouthbridgeDevices (
  IN OUT   IOMMU_PCI_TOPOLOGY *PciPtr,
  IN OUT   IOMMU_IVHD_ENTRY   *IvhdPtr,
  IN OUT   AMD_NB_CONFIG      *pConfig
  )
{
  UINT16    DeviceId;
  PCI_ADDR  IommuPciAddress;
  IommuPciAddress = pConfig->NbPciAddress;
  IommuPciAddress.Address.Function = NB_IOMMU;

  // Assume Device 0x10 Function 0 - Device 0x17 Function 7 belong to the SB
  //PciPtr->PhantomFunction = FALSE;
  for (DeviceId = (0x10 << 3); (DeviceId < (0x18 << 3)); DeviceId++) {
    IommuCreateDeviceEntry (PciPtr, DeviceId, IvhdPtr, pConfig);
    //if (PciPtr->PhantomFunction) {
    //    // Enable SB phantom enable
    //  LibNbPciIndexRMW (IommuPciAddress.AddressValue | L1CFG_INDEX, L1CFG_SEL_WR_EN | L1CFG_SEL_SB, AccessS3SaveWidth32, (UINT32)~BIT0, BIT0, pConfig);
    //}
    //PciPtr->PhantomFunction = FALSE;
  }

#if defined (IVHD_APIC_SUPPORT) || defined (IVHD_HPET_SUPPORT)
  DeviceId = (SB_DEV << 3);                                       // Bus 0 Dev 14 Func 0
#endif

#ifdef  IVHD_APIC_SUPPORT
  // Southbridge IOAPIC
  IommuCreate8ByteEntry (DE_SPECIAL, DATA_ALLINTS, 0, 0xFF, DeviceId, VARIETY_IOAPIC, IvhdPtr);
#endif

#ifdef  IVHD_HPET_SUPPORT
  // Southbridge HPET
  IommuCreate8ByteEntry (DE_SPECIAL, DATA_ALLINTS, 0, 0, DeviceId, VARIETY_HPET, IvhdPtr);
#endif

}

/*----------------------------------------------------------------------------------------*/
/**
 * Place IVHD Device Entries
 *
 * @param[in, out]  IvhdPtr     Pointer to buffer to return IVRS.
 * @param[in]       pConfig       NB config structute
 */
VOID
IommuIvhdApicsAndHpets (
  IN OUT   IOMMU_IVHD_ENTRY *IvhdPtr,
  IN OUT   AMD_NB_CONFIG    *pConfig
  )
{
#ifdef  IVHD_APIC_SUPPORT
  PCI_ADDR  PciAddress;
  UINT16    DeviceId;
  UINT32    PciData;

  PciAddress = pConfig->NbPciAddress;

  // Northbridge IOAPIC
  DeviceId = (UINT16)((PciAddress.Address.Bus << 8)) | 1;          // Bus X Dev 0 Func 0
  LibNbPciRead (PciAddress.AddressValue | 0x4C, AccessWidth32, &PciData, pConfig);
  if (PciData & (UINT32)BIT1) {
    IommuCreate8ByteEntry (DE_SPECIAL, DATA_NOINTS, 0, 0xFF, DeviceId, VARIETY_IOAPIC, IvhdPtr);
  }
#endif
}

/*----------------------------------------------------------------------------------------*/
/**
 * Place IVHD device entry (type 2 for single function or 3/4 for multifunction) at end of IVHD entry
 *
 * @param[in, out]  PciPtr        Pci topology flags
 * @param[in]       DeviceId      DeviceID of entry to potentially create
 * @param[in, out]  IvhdPtr       Pointer to IVHD
 * @param[in]       pConfig       NB config structute
 */
VOID
IommuCreateDeviceEntry (
  IN OUT   IOMMU_PCI_TOPOLOGY *PciPtr,
  IN       UINT16             DeviceId,
  IN OUT   IOMMU_IVHD_ENTRY   *IvhdPtr,
  IN       AMD_NB_CONFIG      *pConfig
  )
{
  // 8 BYTE DEVICE ENTRY:
  // [0] Type (0x2 = Device, 0x3 = Device Range Start, 0x4 = Device Range End
  // [1] DeviceID LSB (Device/Function)
  // [2] DeviceID MSB (Bus)
  // [3] Data (0 = No legacy interrupts)

  // DEVICEID
  // A 16 bit bus/device/function DeviceId consists of:
  // [15:8] Bus
  // [7:3] Device
  // [2:0] Function

  PCI_ADDR  PciAddress;
  UINT32    PciData;
  UINT8     DeviceCount;
  UINT8     FunctionCount;
  UINT8     PcieCapBase;
  UINT8     PcixCapBase;

  BOOLEAN   LegacyBridge;
  BOOLEAN   MultiFunction;
  BOOLEAN   SubFunction;
  BOOLEAN   DiscreteEntry;
  UINT8     HighFunction;
  UINT32    ClassCode;
  UINT16    ExtendedCapabilityPtr;
  SB_INFO   SbInfo;
  UINT8     SataEnableRegValue;
  BOOLEAN   SrIovDevice;

  PcieCapBase = 0;
  PcixCapBase = 0;
  LegacyBridge = FALSE;
  MultiFunction = FALSE;
  SubFunction = FALSE;
  DiscreteEntry = FALSE;
  HighFunction = 0;
  SataEnableRegValue = 0;
  SrIovDevice = FALSE;

  //For SB700, get combined mode status
  SbInfo = LibAmdSbGetRevisionInfo ((pConfig == NULL)?NULL:GET_BLOCK_CONFIG_PTR (pConfig));
  if (SbInfo.Type == SB_SB700) {
    LibNbPciRead (MAKE_SBDFO (0, 0, 0x14, 0, SATA_ENABLE_REG), AccessWidth8, &SataEnableRegValue , pConfig);
  }


  // If the device to check does not exist, exit
  PciAddress.AddressValue = MAKE_SBDFO (0, (DeviceId >> 8) & 0xFF, (DeviceId >> 3) & 0x1F, DeviceId & 0x7, 0);
  if (!LibNbIsDevicePresent (PciAddress, pConfig)) {
    return;
  };
  LibNbPciRead (PciAddress.AddressValue | PCI_CLASS, AccessWidth32, &ClassCode, pConfig);
  ClassCode = (ClassCode >> 16) & 0xFFFF;   // Keep class code and sub-class only

  // THREE STAGES TO THIS FUNCTION
  // 1. Check for multifunction or special devices
  // 2. Place device entry for the current device ID
  // 3. If a bridge, decide if we need to traverse further

  // STEP 1 - CHECK FUNCTIONS ON THIS DEVICE
  // To make decisions, we will need several pieces of information about this device not found with current SBDFO
  // 1. Multifunction device - To determine if a device entry, or device range entry is needed - check function 0 only
  // 2. DisableRange - We will create single entries a device containing a PCI or PCIE bridge
  // 3. How many functions on this device?

  PciAddress.AddressValue = MAKE_SBDFO (0, (DeviceId >> 8) & 0xFF, (DeviceId >> 3) & 0x1F, 0, 0);
  LibNbPciRead (PciAddress.AddressValue | PCI_HEADER, AccessWidth32, &PciData, pConfig);
  if ((PciData & PCI_MULTIFUNCTION) != 0) {
    MultiFunction = TRUE;
  } else {
    DiscreteEntry = TRUE;
  }
  if ((DeviceId & 0x7) != 0 && MultiFunction) {
    SubFunction = TRUE;
  }

  if (MultiFunction) {
    for (FunctionCount = 0; FunctionCount < 8; FunctionCount++) {
      PciAddress.AddressValue = MAKE_SBDFO (0, (DeviceId >> 8) & 0xFF, (DeviceId >> 3) & 0x1F, FunctionCount, 0);
      LibNbPciRead (PciAddress.AddressValue | PCI_HEADER, AccessWidth32, &PciData, pConfig);
      if (PciData != PCI_INVALID) {
        HighFunction = FunctionCount;
      }
      LibNbPciRead (PciAddress.AddressValue | PCI_CLASS, AccessWidth32, &PciData, pConfig);
      if (((PciData >> 16) & 0xFFFF) == PCI_BRIDGE_CLASS) {
        DiscreteEntry = TRUE;
      }
    }
  }

  // For SR IOV devices set for all functions to be available
  if (MultiFunction && (!DiscreteEntry) && (!SubFunction)) {
    PciAddress.AddressValue = MAKE_SBDFO (0, (DeviceId >> 8) & 0xFF, (DeviceId >> 3) & 0x1F, 0, 0);
    ExtendedCapabilityPtr = LibNbFindPcieExtendedCapability (PciAddress.AddressValue, 0x10, pConfig);
    if (ExtendedCapabilityPtr != 0) {
      SrIovDevice = TRUE;
    }
  }
    // STEP 2 - PLACE DEVICE ENTRY
    // We have already decided whether we should use discrete type2 entries, or ranged type3/4 entries
    // Place each device entry at the end of the current IVHD
    // In each case, increment the maximum bus/device/function for L2 cache control done after the IVHD is created

  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NB_TRACE), "[NBIOMMU]Placing Entry for Device [0x%x]\n", DeviceId));

  //if (IommuCheckPhantom (DeviceId, pConfig)) {
  //  PciPtr->PhantomFunction = TRUE;
  //}

  if (!MultiFunction || DiscreteEntry) {
    //For Device 0x14, function 0, Set DATA_ALLINTS
    if (DeviceId == (SB_DEV << 3)) {
      IommuCreate4ByteEntry (DE_SELECT, DATA_ALLINTS, DeviceId, IvhdPtr);
    } else if (DeviceId == DEVICEID_IDE) {
      // For IDE device 0x14, function 1, first check if in combined mode
      if (SataEnableRegValue & SATA_COMBINED_MODE) {
        // Create Alias entry in combined mode
        IommuCreate8ByteEntry (DE_ALIASSELECT, DATA_NOINTS, DEVICEID_IDE, 0, DEVICEID_SATA, 00, IvhdPtr);
      } else {
        //Create select entry if not in the combined mode
        IommuCreate4ByteEntry (DE_SELECT, 0, DeviceId, IvhdPtr);
      }
    } else {
      // For all other single function devices other than device 0x14, functions 0 or 1, create select entry
      IommuCreate4ByteEntry (DE_SELECT, 0, DeviceId, IvhdPtr);
    }

    // Record the largest bus, device, function which will be used as a mask by the Iommu L2 cache
    // Record if phantom device present for current device.  Only set it if present, do not clear.
//    if (IommuCheckPhantom (DeviceId, pConfig)) {
//      PciPtr->PhantomFunction = TRUE;
//    }
    IommuRecordBusDevFuncInfo (PciPtr, DeviceId, pConfig);
  }

  if (MultiFunction && (!DiscreteEntry) && (!SubFunction)) {

    // This is a multifunction device without a bridge, so create a type 3 and 4 device entry
    IommuCreate4ByteEntry (DE_START, 0, DeviceId, IvhdPtr);
    if (SrIovDevice) {
      IommuCreate4ByteEntry (DE_END, 0, (DeviceId | 0x00FF), IvhdPtr);
    } else {
      IommuCreate4ByteEntry (DE_END, 0, DeviceId + HighFunction, IvhdPtr);
    }

    // Record the largest bus, device, function which will be used as a mask by the Iommu L2 cache
    // Record if phantom device present for current device.  Only set it if present, do not clear.
//    if (IommuCheckPhantom (DeviceId, pConfig)) {
//      PciPtr->PhantomFunction = TRUE;
//    }
    IommuRecordBusDevFuncInfo (PciPtr, DeviceId + HighFunction, pConfig);
  }

  if (ClassCode == PCI_BRIDGE_CLASS) {
    UINTN   Type;
    UINT32  BusData;
    // STEP 3 - BRIDGE DEVICE
    // These are treated a little differently.  We already created the entry for the bridge itself...
    // For a PCIe bridge, continue down the bridge device creating more entries until we find an endpoint
    // For a PCI bridge, define the entire sub-bus range as belonging to this source id
    // For a PCIX bridge, figure out which mode it is operating in
    PciAddress.AddressValue = MAKE_SBDFO (0, (DeviceId >> 8) & 0xFF, (DeviceId >> 3) & 0x1F, DeviceId & 0x7, 0);
    LibNbPciRead (PciAddress.AddressValue | PCI_BUS, AccessWidth32, &BusData, pConfig);
    PcieCapBase = LibNbFindPciCapability (PciAddress.AddressValue, PCIE_CAPID, pConfig);
    PcixCapBase = LibNbFindPciCapability (PciAddress.AddressValue, PCIX_CAPID, pConfig);

    Type = 0;
    if (PcieCapBase != 0) {
      Type = 1;
      LibNbPciRead (PciAddress.AddressValue | PcieCapBase, AccessWidth32, &PciData, pConfig);
      PciData = (PciData >> 16) & PCIE_PORTMASK;    // [7:4] are Device/Port type, 01
      if (PciData == PCIE_PCIE2PCIX) {
        Type = 2;
      }
    }
    if (PcixCapBase != 0) {
      Type = 2;
    }

    //For Hot plug capable devices, create 'Start of range' and 'End of range' IVRS'.
    // This will override Type 1 and Type 2.
    if (IommuCheckHp (DeviceId, pConfig)) {
      Type = 3;
    }

    switch (Type) {
    case  0:
      //PCI
      IommuRecordBusDevFuncInfo (PciPtr, DeviceId, pConfig);
      IommuCreate8ByteEntry (DE_ALIASSTART, DATA_NOINTS, (UINT16) (BusData & 0xFF00), 0, DeviceId, 0, IvhdPtr);
      IommuCreate4ByteEntry (DE_END, 0, (UINT16) (((BusData & 0xFF0000) >> 8) + 0xFF), IvhdPtr);
      break;
    case  1:
      //Pcie (non hot plug)
      for (DeviceCount = 0; DeviceCount <= 0x1f; DeviceCount++) {
        for (FunctionCount = 0; FunctionCount <= 0x7; FunctionCount++) {
          IommuCreateDeviceEntry (PciPtr, ((UINT16) (BusData & 0xFF00)) | (DeviceCount << 3) | FunctionCount, IvhdPtr, pConfig);
        }
      }
      break;
    case  2:
      //PCIx
      IommuRecordBusDevFuncInfo (PciPtr, (UINT16) (BusData & 0xFF00), pConfig);
      IommuCreate8ByteEntry (DE_ALIASSTART, DATA_NOINTS, (UINT16) ((BusData & 0xFF00) | ( 1 << 3)), 0, (UINT16) (BusData & 0xFF00), 0, IvhdPtr);
      IommuCreate4ByteEntry (DE_END, 0, (UINT16) (((BusData & 0xFF0000) >> 8) + 0xFF), IvhdPtr);
      break;
    case 3:
      //For Hot plug ports, set all devices and functions behind the secondary bus.
      IommuCreate4ByteEntry (DE_START, 0, (UINT16) (BusData & 0xFF00), IvhdPtr);            // Secondary bus, Device 0, Function 0
      IommuCreate4ByteEntry (DE_END, 0, (UINT16) ((BusData & 0xFF00) | (0x1F << 3) | 7), IvhdPtr); // Secondary bus, Device 1f, Function 7
      break;
    default:
      CIMX_ASSERT (FALSE);
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Place IVHD device entry (type 2 for single function or 3/4 for multifunction) at end of IVHD entry
 *
 * @param[in, out]  PciPtr        Pci topology flags
 * @param[in]       DeviceId      DeviceID
 * @param[in]       pConfig       NB config structute
 */
VOID
IommuRecordBusDevFuncInfo (
  IN OUT   IOMMU_PCI_TOPOLOGY *PciPtr,
  IN       UINT16             DeviceId,
  IN       AMD_NB_CONFIG      *pConfig
  )
{
  UINT16    ExtendedCapabilityPtr;
  PCI_ADDR  Device;
  Device.AddressValue = MAKE_SBDFO (0, DeviceId >> 8, (DeviceId >> 3) & 0x1f, DeviceId & 0x7, 0);
#ifdef  EXCLUDE_SB_DEVICE_FROM_L2_HASH
  if ((UINT8)Device.Address.Bus == 0) {
    AMD_NB_CONFIG_BLOCK *ConfigPtr = GET_BLOCK_CONFIG_PTR (pConfig);
    if (ConfigPtr->PlatformType == ServerPlatform) {
      return;
    }
  }
#endif
  Device.AddressValue = MAKE_SBDFO (0, DeviceId >> 8, (DeviceId >> 3) & 0x1f, DeviceId & 0x7, 0);
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NB_TRACE), "    Device Data For L2 Hash Bus = 0x%x Device = 0x%x Func = 0x%x\n", Device.Address.Bus, Device.Address.Device, Device.Address.Function));
  ExtendedCapabilityPtr = LibNbFindPcieExtendedCapability (Device.AddressValue, 0x10, pConfig);
  if (ExtendedCapabilityPtr != 0) {
    UINT16  TotalVF;
    LibNbPciRead (Device.AddressValue | (ExtendedCapabilityPtr + 0xE), AccessWidth16, &TotalVF, pConfig);
    PciPtr->MaxFunction = CIMX_MAX (PciPtr->MaxFunction, TotalVF);
  }
  PciPtr->MaxBus = CIMX_MAX (PciPtr->MaxBus, (UINT8)Device.Address.Bus);
  PciPtr->MaxDevice = CIMX_MAX (PciPtr->MaxDevice, (UINT8)Device.Address.Device);
  PciPtr->MaxFunction = CIMX_MAX (PciPtr->MaxFunction, (UINT16) (UINT8)Device.Address.Function);
}


/*----------------------------------------------------------------------------------------*/
/**
 * Append data entry to IVRS
 *
 * @param[in]      Type       IVRC entry type
 * @param[in]      Data       IVRC entry data
 * @param[in]      Word1      IVRC entry data
 * @param[in, out] IvhdPtr    Current IVHD pointer
 *
 */
VOID
IommuCreate4ByteEntry (
  IN       UINT8            Type,
  IN       UINT8            Data,
  IN       UINT16           Word1,
  IN OUT   IOMMU_IVHD_ENTRY *IvhdPtr
  )
{
  UINT32 Buffer;
  UINT16 AlignedDeviceEntryIndex;
  UINT16 DeviceEntryIndex;

  Buffer = Type + (Word1 << 8) + (Data << 24);
  DeviceEntryIndex = (IvhdPtr->Length - 24) / sizeof (UINT32);
  AlignedDeviceEntryIndex = DeviceEntryIndex;

#ifdef IVHD_MIN_8BYTE_ALIGNMENT
  AlignedDeviceEntryIndex = (DeviceEntryIndex + 1) & 0xfffe;
#endif

  IvhdPtr->DeviceEntry[AlignedDeviceEntryIndex] = Buffer;
  IvhdPtr->Length += (4 + (AlignedDeviceEntryIndex - DeviceEntryIndex) * 4);
  CIMX_TRACE ((TRACE_DATA (NULL, CIMX_NB_TRACE), "[NBIOMMU]Added entry - [0x%x]\n", Buffer));
}

/*----------------------------------------------------------------------------------------*/
/**
 * Append data entry to IVRS
 *
 * @param[in]      Type       IVRC entry type
 * @param[in]      Data       IVRC entry data
 * @param[in]      Word1      IVRC entry data
 * @param[in]      Byte4      IVRC entry data
 * @param[in]      Word5      IVRC entry data
 * @param[in]      Byte7      IVRC entry data
 * @param[in, out] IvhdPtr    Current IVHD pointer
 *
 */
VOID
IommuCreate8ByteEntry (
  IN       UINT8            Type,
  IN       UINT8            Data,
  IN       UINT16           Word1,
  IN       UINT8            Byte4,
  IN       UINT16           Word5,
  IN       UINT8            Byte7,
  IN OUT   IOMMU_IVHD_ENTRY *IvhdPtr
  )
{
  UINT16 AlignedDeviceEntryIndex;
  UINT16 DeviceEntryIndex;
  UINT64 Buffer;

  Buffer = Type + (Word1 << 8) + ((UINT32)Data << 24);
  ((UINT32*)&Buffer)[1] = Byte4 + (Word5 << 8) + (Byte7 << 24);
  DeviceEntryIndex = (IvhdPtr->Length - 24) / sizeof (UINT32);
  AlignedDeviceEntryIndex = DeviceEntryIndex;

#if defined (IVHD_MIN_8BYTE_ALIGNMENT) || defined (IVHD_SIZE_ALIGNMENT)
  AlignedDeviceEntryIndex = (DeviceEntryIndex + 1) & 0xfffe;
#endif
  IvhdPtr->DeviceEntry[AlignedDeviceEntryIndex] = ((UINT32*)&Buffer)[0];
  IvhdPtr->DeviceEntry[AlignedDeviceEntryIndex + 1] = ((UINT32*)&Buffer)[1];
  IvhdPtr->Length += (8 + (AlignedDeviceEntryIndex - DeviceEntryIndex) * 4);
  CIMX_TRACE ((TRACE_DATA (NULL, CIMX_NB_TRACE), "[NBIOMMU]Added entry - [0x%llx]\n", Buffer));
}

/*----------------------------------------------------------------------------------------*/
/**
 * Set checksum, IvInfo, finish IVRS table
 *
 * @param[in, out] BufferPtr     Pointer to text buffer to return IVRS.
 * @param[in, out] pConfig       Northbridge configuration structure pointer.
 *
 */
VOID
IommuFinalizeIvrs (
  IN OUT   VOID           *BufferPtr,
  IN       AMD_NB_CONFIG  *pConfig
  )
{
  IOMMU_IVRS_HEADER *HeaderPtr;
  PCI_ADDR          IommuPciAddress;
  UINT32            PciData;

  HeaderPtr = (IOMMU_IVRS_HEADER *)BufferPtr;
  IommuPciAddress = pConfig->NbPciAddress;
  IommuPciAddress.Address.Function = NB_IOMMU;

  // Find common IvInfo (largest shared) 0x50
  // [22] = ATS Translation Reserved
  // [21:15] = VA Size
  // [14:8] = PA Size

  LibNbPciRead (IommuPciAddress.AddressValue | RD890S_CAP_MISC, AccessWidth32, &PciData, pConfig);
  PciData &= (IVINFO_ATSMASK | IVINFO_VAMASK | IVINFO_PAMASK);
  HeaderPtr->IvInfo = PciData;

  //LibAmdUpdateAcpiTableChecksum (HeaderPtr);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Nb Iommu Fixup of IVRS APIC entries
 *
 * @param[in]      DevicePtr     Pointer to current device entry
 * @param[in]      pConfig       Northbridge configuration structure pointer.
 *
 */
UINT64
IommuGetApicBaseAddress (
  IN      VOID           *DevicePtr,
  IN      AMD_NB_CONFIG  *pConfig
  )
{
  PCI_ADDR  PciAddress;
  UINT16    DeviceId;
  UINT32    Data;

  // If no pointer provided, return no base address
  if (DevicePtr == NULL) {
    return 0;
  }

  // Special entry can be IOAPIC or other(HPET).  We only care about the IOAPIC.
  if (*(UINT8*) ((UINT8*)DevicePtr + DE_SPECIAL_VARIETY) != VARIETY_IOAPIC) {
    return 0;
  }

  DeviceId = *(UINT16*) ((UINT8*)DevicePtr + DE_DEVICEID);
  PciAddress.AddressValue = MAKE_SBDFO (0, (DeviceId >> 8) & 0xFF, (DeviceId >> 3) & 0x1F, 0, 0);

  // An APIC entry will only be created for AMD northbridge or southbridges, so
  // we can assume PCI dev/func = 0, 0 will be a northbridge IOAPIC device
  // and any other will be a southbridge IOAPIC device.  If the device was not
  // already enabled and known to be an AMD device, no entry would have been created.

  if ((PciAddress.Address.Device == NB_PCI_DEV) && (PciAddress.Address.Function == NB_HOST)) {

    // We have an AMD NB, check function 0
    Data = 1;
    PciAddress.Address.Function = 0;
    LibNbPciWrite (PciAddress.AddressValue | 0xF8, AccessS3SaveWidth32, &Data, pConfig);
    LibNbPciRead (PciAddress.AddressValue | 0xFC, AccessWidth32, &Data, pConfig);
  } else {
    SB_INFO  SbInfo;
    SbInfo = LibAmdSbGetRevisionInfo ((pConfig == NULL)?NULL:GET_BLOCK_CONFIG_PTR (pConfig));
    if (SbInfo.Type == SB_SB700) {
      PciAddress.Address.Function = 0;
      LibNbPciRead (PciAddress.AddressValue | 0x74, AccessWidth32, &Data, pConfig);
    } else {
      LibAmdSbPmioRead ( 0x34, AccessWidth32, &Data, pConfig);
    }
  }
  return ((UINT64) (Data & 0xFFFFFF00));
}

/*----------------------------------------------------------------------------------------*/
/**
 * Nb Iommu Fixup of IVRS APIC entries
 *
 * @param[in]      BaseAddress   Base address to match
 * @param[in]      MadtPtr       Pointer to current device entry
 * @param[in]      pConfig       Northbridge configuration structure pointer.
 *
 */
UINT8
IommuGetApicId (
  IN      UINT64         BaseAddress,
  IN      VOID           *MadtPtr,
  IN      AMD_NB_CONFIG  *pConfig
  )
{
  VOID  *EntryPtr;

  EntryPtr = LibAmdGetFirstMadtStructure (MADT_APIC_TYPE, MadtPtr);

  do {
    // If our base address for a known device matches this MADT, get the APIC ID
    if (*(UINT32*) ((UINT8*)EntryPtr + MADT_APIC_BASE) == (UINT32)BaseAddress) {
      return *(UINT8*) ((UINT8*)EntryPtr + MADT_APIC_ID);
    }
    EntryPtr = LibAmdGetNextMadtStructure (MADT_APIC_TYPE, EntryPtr, MadtPtr);
  } while (EntryPtr != NULL);
  return 0xFF;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Disconnect unused PCIe core from IOMMU block.
 *
 * @param[in]      CoreId        Pcie Core Id
 * @param[in]      pConfig       Northbridge configuration structure pointer.
 *
 */
VOID
NbIommuDisconnectPcieCore (
  IN      CORE            CoreId,
  IN      AMD_NB_CONFIG   *pConfig
  )
{
  PCI_ADDR  IommuPciAddress;
  UINT32    Value;
  IommuPciAddress = pConfig->NbPciAddress;
  IommuPciAddress.Address.Function = NB_IOMMU;
  Value = 1 << ((0x4310 >> (CoreId * 4)) & 0xf);
  LibNbPciIndexRMW (IommuPciAddress.AddressValue | L2CFG_INDEX, L2CFG_SEL_WR_EN | L2REG_46, AccessS3SaveWidth32, 0xFFFFFFFF, Value , pConfig);
}
