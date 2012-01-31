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

// Identifying an IOMMU:
// RD890S - IOMMU present
// all other (including RD890) - IOMMU not present
// Class = System Base Peripheral (08h)
// Subclass = IOMMU (06h)
// Programming Interface Code = 0h
// Must reside on top/root complex PCI hierarchy
// There is always a NB device at bus 0 device 0 function 0 (fcn 2 for IOMMU) - device ID 0x5A23

// Inputs:
// From OEM: Get exclusion table
// From OEM: Get text buffer

// Outputs:
// To OEM: Complete IVRS table for linking

#ifndef _NBIOMMU_H_
#define _NBIOMMU_H_

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

AGESA_STATUS
NbIommuInit (
  IN OUT   AMD_NB_CONFIG_BLOCK   *ConfigPtr
  );

AGESA_STATUS
NbIommuInitS3 (
  IN OUT   AMD_NB_CONFIG_BLOCK   *ConfigPtr
  );

AGESA_STATUS
NbIommuAcpiInit (
  IN OUT   AMD_NB_CONFIG_BLOCK   *ConfigPtr
  );

AGESA_STATUS
NbIommuAcpiFixup (
  IN OUT   AMD_NB_CONFIG_BLOCK   *ConfigPtr
  );

VOID
NbIommuDisconnectPcieCore (
  IN      CORE            CoreId,
  IN      AMD_NB_CONFIG   *pConfig
  );

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

// IOMMU Architectural
#define IVRS_BUFFER_SIZE        0x2000      // Default 8KB allocated to table
//#define IVHD_MIN_8BYTE_ALIGNMENT          // Align IVHD entries on 8 byte boundary
#define IVHD_SIZE_ALIGNMENT                 // Align IVHD entries on MOD entry-size boundary
#define IVHD_HPET_SUPPORT                   // Create HPET entries
#define IVHD_APIC_SUPPORT                   // Create IOAPIC entries

// IOMMU Northbridge
#define RD890S_CAP_MISC         0x50        // RD890/S Capabilities Misc Info
#define NB_PCI_DEV              0           // PCI NB device number
#define NB_HOST                 0           // Function 0 = NB HOST
#define NB_IOMMU                2           // Function 2 = IOMMU
#define SB_DEV                  0x14        // PCI SB device number
#define SB_SMBUS                3           // Function 3 = SMBUS
#define SB_SATA                 0x11        // SB SATA
#define SATA_ENABLE_REG         0xAD        // Dev. 0x14, Func 0, Reg 0xAD for SATA combined mode
#define SATA_COMBINED_MODE      BIT3        // Bit 3 of SB_ENABLE_REG., 1 = Combined mode

#define IOMMU_CAP_HEADER_OFFSET 0x00
#define IOMMU_BASE_LOW_OFFSET   0x04
#define IOMMU_BASE_HIGH_OFFSET  0x08
#define IOMMU_RANGE_OFFSET      0x0C
#define IOMMU_MISC_OFFSET       0x10

#define DEVICEID_NB       ((0 << 8) + (NB_PCI_DEV << 3) + NB_HOST)
#define DEVICEID_IOMMU    ((0 << 8) + (NB_PCI_DEV << 3) + NB_IOMMU)
#define DEVICEID_GPP1_0   ((0 << 8) + (0x2 << 3) + 0)
#define DEVICEID_GPP1_1   ((0 << 8) + (0x3 << 3) + 0)
#define DEVICEID_GPP2_0   ((0 << 8) + (0xB << 3) + 0)
#define DEVICEID_GPP2_1   ((0 << 8) + (0xC << 3) + 0)
#define DEVICEID_GPP3A_0  ((0 << 8) + (0x4 << 3) + 0)
#define DEVICEID_GPP3A_1  ((0 << 8) + (0x5 << 3) + 0)
#define DEVICEID_GPP3A_2  ((0 << 8) + (0x6 << 3) + 0)
#define DEVICEID_GPP3A_3  ((0 << 8) + (0x7 << 3) + 0)
#define DEVICEID_GPP3A_4  ((0 << 8) + (0x9 << 3) + 0)
#define DEVICEID_GPP3A_5  ((0 << 8) + (0xA << 3) + 0)
#define DEVICEID_GPP3B_0  ((0 << 8) + (0xD << 3) + 0)

#define DEVICEID_SATA     ((0 << 8) + (0x11 << 3) + 0)
#define DEVICEID_IDE      ((0 << 8) + (0x14 << 3) + 1)

#define L1CFG_INDEX             0xF8
// There is an L1 for each device (6), which is selected by [19:16] of L1CFG_INDEX
// e.g. (LibNbPciIndexRead (Address | L1CFGIND, L1_REG_0C | L1_CFG_SEL, AccessWidth32, &Value, pConfig)
#define L1CFG_SEL_WR_EN         0x80000000
#define L1CFG_SEL_GPP1          0x00000000
#define L1CFG_SEL_GPP2          0x00010000
#define L1CFG_SEL_SB            0x00020000
#define L1CFG_SEL_GPP3A         0x00030000
#define L1CFG_SEL_GPP3B         0x00040000
#define L1CFG_SEL_VC1           0x00050000
#define L1REG_06                0x6
#define L1REG_0C                0xC
#define L1REG_0D                0xD
#define L1REG_07                0x7
#define L1CFG_DATA              0xFC

#define L2CFG_INDEX             0xF0
// e.g. (LibNbPciIndexRead (Address | L2CFGIND, L2_REG_0C, AccessWidth16, &Value, pConfig)
#define L2CFG_SEL_WR_EN         0x100
#define L2REG_06                0x6
#define L2REG_07                0x7
#define L2REG_0C                0xC
#define L2REG_10                0x10
#define L2REG_11                0x11
#define L2REG_14                0x14
#define L2REG_15                0x15
#define L2REG_18                0x18
#define L2REG_19                0x19
#define L2REG_1C                0x1C
#define L2REG_1D                0x1D
#define L2REG_46                0x46
#define L2REG_47                0x47
#define L2REG_50                0x50
#define L2REG_51                0x51
#define L2REG_52                0x52
#define L2REG_56                0x56
#define L2REG_30                0x30
#define L2REG_80                0x80
#define L2CFG_DATA              0xF4

// PCI/PCIe Architectural
#define PCIE_CAPID              0x10
#define PCIE_PORTMASK           0xF0              // Device cap reg 2
#define PCIE_PCIE2PCIX          0x70              // Device cap reg 2
#define PCIE_PHANTOMMASK        0x18              // Device cap reg 4
#define PCIX_CAPID              0x07
#define IOMMU_CAPID             0x0F

#define PCI_DVID                0x00
#define PCI_INVALID             0xFFFFFFFF
#define PCI_CLASS               0x08
#define PCI_HEADER              0x0C
#define PCI_MULTIFUNCTION       0x00800000
#define PCI_BUS                 0x18
#define PCI_SUBMASK             0xFF0000
#define PCI_SECMASK             0xFF00
#define PCI_PRIMASK             0xFF
#define PCI_BRIDGE_CLASS        0x0604

// IVRS Table Access
#define TYPE_IVHD               0x10
#define IVINFO_ATSMASK          0x00400000          // [22] = ATS
#define IVINFO_VAMASK           0x003F8000          // [21:15] = Virtual Address Size
#define IVINFO_PAMASK           0x00007F00          // [14:8] = Physical Address Size
#define FLAGS_COHERENT          BIT5
#define FLAGS_IOTLBSUP          BIT4
#define FLAGS_ISOC              BIT3
#define FLAGS_RESPASSPW         BIT2
#define FLAGS_PASSPW            BIT1

#define TYPE_IVMD_ALL           0x20
#define TYPE_IVMD_SELECT        0x21
#define TYPE_IVMD_RANGE         0x22

#define DE_PAD4                 1
#define DE_BYTE0                0
#define DE_BYTE1                1
#define DE_BYTE2                2
#define DE_BYTE3                3
#define DE_SELECT               2
#define DATA_NOINTS             0
#define DATA_LINT_EINT_INIT     BIT7 + BIT6 + BIT1 + BIT0
#define DATA_ALLINTS            0xD7
#define DE_START                3
#define DE_END                  4
#define DE_PAD8                 64
#define DE_BYTE4                4
#define DE_BYTE5                5
#define DE_BYTE6                6
#define DE_BYTE7                7
#define DE_ALIASSELECT          66
#define DE_ALIASSTART           67
#define DE_SPECIAL              72
#define VARIETY_IOAPIC          0x1
#define VARIETY_HPET            0x2
#define DE_SPECIAL_VARIETY      7
#define DE_DEVICEID             5
#define DE_SPECIAL_ID           4

// MADT Table Access
#define MADT_APIC_TYPE          0x1
#define MADT_APIC_ID            0x2
#define MADT_APIC_BASE          0x4

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */



#pragma pack (push, 1)

/// IVRS header
typedef struct {
  UINT32  Signature;                        ///< see IOMMU specification for details
  UINT32  Length;                           ///< see IOMMU specification for details
  UINT8   Revision;                         ///< see IOMMU specification for details
  UINT8   Checksum;                         ///< see IOMMU specification for details
  CHAR8   OemId[6];                         ///< see IOMMU specification for details
  CHAR8   OemTableId[8];                    ///< see IOMMU specification for details
  CHAR8   OemRevision[4];                   ///< see IOMMU specification for details
  CHAR8   CreatorId[4];                     ///< see IOMMU specification for details
  CHAR8   CreatorRevision[4];               ///< see IOMMU specification for details
  UINT32  IvInfo;                           ///< see IOMMU specification for details
  UINT64  Reserved;                         ///< see IOMMU specification for details
} IOMMU_IVRS_HEADER;

/// DeviceID
typedef struct {
  UINT16  TableLength;                      ///< length of table
  UINT16  Device[];                         ///< DeviceID
} IOMMU_DEVICELIST;

/// PCI Topology Based Settings
typedef struct {
//  BOOLEAN PhantomFunction;                    ///< phantom functions present
  UINT8   MaxBus;                           ///< max bus accumulator
  UINT8   MaxDevice;                        ///< max device accumulator
  UINT16  MaxFunction;                      ///< max function accumulator
} IOMMU_PCI_TOPOLOGY;

/// IVHD for each hardware definition (i.e. # of northbridges)
typedef struct {
  UINT8   Type;                             ///< see IOMMU specification for details
  UINT8   Flags;                            ///< see IOMMU specification for details
  UINT16  Length;                           ///< see IOMMU specification for details
  UINT16  DeviceId;                         ///< see IOMMU specification for details
  UINT16  CapabilityOffset;                 ///< see IOMMU specification for details
  UINT64  BaseAddress;                      ///< see IOMMU specification for details
  UINT16  PciSegment;                       ///< see IOMMU specification for details
  UINT16  IommuInfo;                        ///< see IOMMU specification for details
  UINT32  Reserved;                         ///< see IOMMU specification for details
  UINT32  DeviceEntry[];                    ///< see IOMMU specification for details
} IOMMU_IVHD_ENTRY;

/// IVMD for each memory range
typedef struct {
  UINT8   Type;                             ///< see IOMMU specification for details
  UINT8   Flags;                            ///< see IOMMU specification for details
  UINT16  Length;                           ///< see IOMMU specification for details
  UINT16  DeviceId;                         ///< see IOMMU specification for details
  UINT16  AuxData;                          ///< see IOMMU specification for details
  UINT64  Reserved;                         ///< see IOMMU specification for details
  UINT64  BlockStartAddress;                ///< see IOMMU specification for details
  UINT64  BlockLength;                      ///< see IOMMU specification for details
} IOMMU_IVMD_ENTRY;

//#define IVRS_HANDLE 'SRVI'
#define IVRS_HANDLE Int32FromChar ('S', 'R', 'V', 'I')

#define L2_DTC_CONTROL    0x10
#define L2_ITC_CONTROL    0x14
#define L2_PTC_A_CONTROL  0x18
#define L2_PTC_B_CONTROL  0x1C
#define L2_PDC_CONTROL    0x50

#define EXCLUDE_SB_DEVICE_FROM_L2_HASH

/// L2 cache init
typedef struct {
  UINT8 HashControl;                        ///<Control regsiter block address
} L2_HASH_CONTROL;

#pragma pack (pop)

#endif
