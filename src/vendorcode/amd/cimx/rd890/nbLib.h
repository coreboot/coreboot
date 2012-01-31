/**
 * @file
 *
 * CNB Library function
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

#ifndef _NBLIB_H_
#define _NBLIB_H_

#pragma pack(push, 1)

/// NB_MCU_MODE
typedef enum {
  AssertReset,                              ///< Assert reset
  DeAssertReset                             ///< Deassert reset
} NB_MCU_MODE;

/// SMU Firmware revision
typedef struct {
  UINT16              MajorRev;             ///< Major revision
  UINT16              MinorRev;             ///< Minor revision
} SMU_FIRMWARE_REV;

/// Firmware block
typedef struct {
  UINT16              Address;              ///< Block Address
  UINT16              Length;               ///< Block length in DWORD
  UINT32              *Data;                ///< Pointer to data array
} SMU_FIRMWARE_BLOCK;

/// Firmware header
typedef struct {
  SMU_FIRMWARE_REV    Revision;              ///< Revision info
  UINT16              NumberOfBlock;         ///< Number of blocks
  SMU_FIRMWARE_BLOCK  *BlockArray;           ///< Pointer to block definition array
} SMU_FIRMWARE_HEADER;


NB_INFO
LibNbGetRevisionInfo (
  IN      AMD_NB_CONFIG       *NbConfigPtr
  );

AGESA_STATUS
LibNbCallBack (
  IN      UINT32              CallBackId,
  IN  OUT UINTN               Data,
  IN  OUT AMD_NB_CONFIG       *NbConfigPtr
  );

VOID
LibNbPciWrite (
  IN      UINT32              Address,
  IN      ACCESS_WIDTH        Width,
  IN      VOID                *Value,
  IN      AMD_NB_CONFIG       *NbConfigPtr
  );

VOID
LibNbPciRead (
  IN       UINT32              Address,
  IN       ACCESS_WIDTH        Width,
     OUT   VOID                *Value,
  IN       AMD_NB_CONFIG       *NbConfigPtr
  );

VOID
LibNbPciRMW (
  IN      UINT32              Address,
  IN      ACCESS_WIDTH        Width,
  IN      UINT32              Mask,
  IN      UINT32              Data,
  IN      AMD_NB_CONFIG       *NbConfigPtr
  );

VOID
LibNbPciIndexRead (
  IN       UINT32              Address,
  IN       UINT32              Index,
  IN       ACCESS_WIDTH        Width,
     OUT   UINT32              *Value,
  IN       AMD_NB_CONFIG       *NbConfigPtr
  );

VOID
LibNbPciIndexWrite (
  IN      UINT32              Address,
  IN      UINT32              Index,
  IN      ACCESS_WIDTH        Width,
  IN      UINT32              *Value,
  IN      AMD_NB_CONFIG       *NbConfigPtr
  );

VOID
LibNbPciIndexRMW (
  IN      UINT32              Address,
  IN      UINT32              Index,
  IN      ACCESS_WIDTH        Width,
  IN      UINT32              Mask,
  IN      UINT32              Data,
  IN      AMD_NB_CONFIG       *NbConfigPtr
  );

VOID
LibNbIndirectTableInit (
  IN      UINT32              Address,
  IN      UINT32              Index,
  IN      INDIRECT_REG_ENTRY  *pTable,
  IN      UINTN               Length,
  IN      AMD_NB_CONFIG       *NbConfigPtr
  );

UINT8
LibNbFindPciCapability (
  IN      UINT32              Address,
  IN      UINT8               CapabilityId,
  IN      AMD_NB_CONFIG       *NbConfigPtr
  );

VOID
LibNbIoRMW (
  IN      UINT16              Address,
  IN      ACCESS_WIDTH                Width,
  IN      UINT32              Mask,
  IN      UINT32              Data,
  IN      AMD_NB_CONFIG       *NbConfigPtr
  );

VOID
LibNbCpuHTLinkPhyRead (
  IN        UINT8               Node,
  IN        UINT8               Link,
  IN        UINT16              Register,
     OUT    UINT32              *Value,
  IN        AMD_NB_CONFIG        *NbConfigPtr
 );

VOID
LibNbCpuHTLinkPhyWrite (
  IN      UINT8               Node,
  IN      UINT8               Link,
  IN      UINT16              Register,
  IN      UINT32              *Value,
  IN      AMD_NB_CONFIG       *NbConfigPtr
);

VOID
LibNbCpuHTLinkPhyRMW (
  IN      UINT8               Node,
  IN      UINT8               Link,
  IN      UINT16              Register,
  IN      UINT32              Mask,
  IN      UINT32              Data,
  IN      AMD_NB_CONFIG       *NbConfigPtr
);

VOID
LibNbEnableClkConfig (
  IN      AMD_NB_CONFIG       *pConfig
  );

VOID
LibNbDisableClkConfig (
  IN      AMD_NB_CONFIG       *pConfig
  );

BOOLEAN
LibNbIsDevicePresent (
  IN      PCI_ADDR            Device,
  IN      AMD_NB_CONFIG       *NbConfigPtr
  );

UINT32
LibNbBitReverse (
  IN      UINT32              Data,
  IN      UINT8               StartBit,
  IN      UINT8               StopBit
  );

UINT32
LibNbGetCpuFamily (
  VOID
  );

VOID
LibNbIoWrite (
  IN      UINT16              Address,
  IN      ACCESS_WIDTH        Width,
  IN      VOID                *Value,
  IN      AMD_NB_CONFIG       *NbConfigPtr
  );

VOID
LibNbIoRead (
  IN       UINT16             Address,
  IN       ACCESS_WIDTH       Width,
     OUT   VOID               *Value,
  IN       AMD_NB_CONFIG      *NbConfigPtr
  );

VOID
LibNbLoadMcuFirmwareBlock (
  IN      UINT16          Address,
  IN      UINT16          Size,
  IN      UINT32          *FirmwareBlockPtr,
  IN      AMD_NB_CONFIG   *NbConfigPtr
  );

UINT32
LibNbReadMcuRam (
  IN      UINT16          Address,
  IN      AMD_NB_CONFIG   *NbConfigPtr
  );

VOID
LibNbMcuControl (
  IN      NB_MCU_MODE     Operation,
  IN      AMD_NB_CONFIG   *NbConfigPtr
  );


AGESA_STATUS
LibSystemApiCall (
  IN       SYSTEM_API            SystemApi,
  IN OUT   AMD_NB_CONFIG_BLOCK   *ConfigPtr
  );

AGESA_STATUS
LibNbApiCall (
  IN       NB_API                NbApi,
  IN OUT   AMD_NB_CONFIG_BLOCK   *ConfigPtr
  );

VOID
LibNbMemRMW (
  IN      UINT64          Address,
  IN      ACCESS_WIDTH    Width,
  IN      UINT32          Mask,
  IN      UINT32          Data,
  IN      AMD_NB_CONFIG   *NbConfigPtr
  );

VOID
LibNbMemRead (
  IN      UINT64          Address,
  IN      ACCESS_WIDTH    Width,
  IN      VOID            *Value,
  IN      AMD_NB_CONFIG   *NbConfigPtr
  );

VOID
LibNbMemWrite (
  IN      UINT64          Address,
  IN      ACCESS_WIDTH    Width,
     OUT  VOID            *Value,
  IN      AMD_NB_CONFIG   *NbConfigPtr
  );

struct _PCI_SCAN_PROTOCOL;
typedef struct _PCI_SCAN_PROTOCOL PCI_SCAN_PROTOCOL;
typedef UINT32  SCAN_STATUS;

typedef SCAN_STATUS (*SCAN_ENTRY) (PCI_SCAN_PROTOCOL *This, PCI_ADDR  Device);

#define SCAN_FINISHED                   0x0
#define SCAN_STOP_DEVICE_ENUMERATION    0x1
#define SCAN_STOP_BUS_ENUMERATION       0x2


/// PCI topology scan protocol
struct _PCI_SCAN_PROTOCOL {
  SCAN_ENTRY    ScanBus;                ///< Pointer to function to scan device on PCI bus.
  SCAN_ENTRY    ScanDevice;             ///< Pointer to function to scan function on PCI device.
  SCAN_ENTRY    ScanFunction;           ///< Pointer to scan PCI function.
  AMD_NB_CONFIG *pConfig;               ///< NB configuration info.
};

SCAN_STATUS
LibNbScanPciBus (
  IN      PCI_SCAN_PROTOCOL   *This,
  IN      PCI_ADDR            Device
  );

SCAN_STATUS
LibNbScanPciDevice (
  IN      PCI_SCAN_PROTOCOL   *This,
  IN      PCI_ADDR            Device
  );

SCAN_STATUS
LibNbScanPciBridgeBuses (
  IN      PCI_SCAN_PROTOCOL   *This,
  IN      PCI_ADDR            Bridge
  );

VOID
LibNbSetDefaultIndexes (
  IN      AMD_NB_CONFIG   *NbConfigPtr
  );

UINT16
LibNbFindPcieExtendedCapability (
  IN      UINT32        Address,
  IN      UINT16        ExtendedCapabilityId,
  IN      AMD_NB_CONFIG *NbConfigPtr
  );

BOOLEAN
LibNbIsIommuEnabled (
  IN      AMD_NB_CONFIG     *NbConfigPtr
  );

#pragma pack(pop)

#endif
