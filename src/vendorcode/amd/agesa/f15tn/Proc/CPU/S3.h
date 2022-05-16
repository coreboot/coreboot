/* $NoKeywords:$ */
/**
 * @file
 *
 *  ACPI S3 support definitions.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
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

#ifndef _S3_H_
#define _S3_H_


/*---------------------------------------------------------------------------------------
 *          M I X E D   (Definitions And Macros / Typedefs, Structures, Enums)
 *---------------------------------------------------------------------------------------
 */


/*---------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *---------------------------------------------------------------------------------------
 */
#define RESTART_FROM_BEGINNING_LIST 0xFFFFFFFFul

/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */

/* Device related definitions */

/// Header at the beginning of a context save buffer.
typedef struct {
  UINT16     Version;                 ///< Version of header
  UINT16     NumDevices;              ///< Number of devices in the list
  UINT16     RelativeOrMaskOffset;    ///< Size of device list + header
} DEVICE_BLOCK_HEADER;

/// S3 device types
typedef enum {
  DEV_TYPE_PCI_PRE_ESR,               ///< PCI device before exiting self-refresh
  DEV_TYPE_PCI,                       ///< PCI device after exiting self-refresh
  DEV_TYPE_CPCI_PRE_ESR,              ///< 'conditional' PCI device before exiting self-refresh
  DEV_TYPE_CPCI,                      ///< 'conditional' PCI device after exiting self-refresh
  DEV_TYPE_MSR_PRE_ESR,               ///< MSR device before exiting self-refresh
  DEV_TYPE_MSR,                       ///< MSR device after exiting self-refresh
  DEV_TYPE_CMSR_PRE_ESR,              ///< 'conditional' MSR device before exiting self-refresh
  DEV_TYPE_CMSR                       ///< 'conditional' MSR device after exiting self-refresh
} S3_DEVICE_TYPES;

/// S3 restoration call points
typedef enum {
  INIT_RESUME,                        ///< AMD_INIT_RESUME
  S3_LATE_RESTORE                     ///< AMD_S3LATE_RESTORE
} CALL_POINTS;

/// S3 device common header
typedef struct {
  UINT32     RegisterListID;          ///< Unique ID of this device
  UINT8      Type;                    ///< Appropriate S3_DEVICE_TYPES type
} DEVICE_DESCRIPTOR;

/// S3 PCI device header
typedef struct {
  UINT32     RegisterListID;          ///< Unique ID of this device
  UINT8      Type;                    ///< DEV_TYPE_PCI / DEV_TYPE_PCI_PRE_ESR
  UINT8      Node;                    ///< Zero-based node number
} PCI_DEVICE_DESCRIPTOR;

/// S3 'conditional' PCI device header
typedef struct {
  UINT32     RegisterListID;          ///< Unique ID of this device
  UINT8      Type;                    ///< DEV_TYPE_CPCI / DEV_TYPE_CPCI_PRE_ESR
  UINT8      Node;                    ///< Zero-based node number
  UINT8      Mask1;                   ///< Conditional mask 1
  UINT8      Mask2;                   ///< Conditional mask 2
} CONDITIONAL_PCI_DEVICE_DESCRIPTOR;

/// S3 MSR device header
typedef struct {
  UINT32     RegisterListID;          ///< Unique ID of this device
  UINT8      Type;                    ///< DEV_TYPE_MSR / DEV_TYPE_MSR_PRE_ESR
} MSR_DEVICE_DESCRIPTOR;

/// S3 'conditional' MSR device header
typedef struct {
  UINT32     RegisterListID;          ///< Unique ID of this device
  UINT8      Type;                    ///< DEV_TYPE_CMSR / DEV_TYPE_CMSR_PRE_ESR
  UINT8      Mask1;                   ///< Conditional mask 1
  UINT8      Mask2;                   ///< Conditional mask 2
} CONDITIONAL_MSR_DEVICE_DESCRIPTOR;

/* Special case related definitions */

/**
 * PCI special case save handler
 *
 * @param[in]     AccessWidth   8, 16, or 32 bit wide access
 * @param[in]     Address       full PCI address of the register to save
 * @param[out]    Value         Value read from the register
 * @param[in]     ConfigPtr     AMD standard header config parameter
 *
 */
typedef VOID (*PF_S3_SPECIAL_PCI_SAVE) (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR     Address,
     OUT   VOID         *Value,
  IN       VOID         *ConfigPtr
  );

/**
 * PCI special case restore handler
 *
 * @param[in]     AccessWidth   8, 16, or 32 bit wide access
 * @param[in]     Address       full PCI address of the register to save
 * @param[in]     Value         Value to write to the register
 * @param[in]     ConfigPtr     AMD standard header config parameter
 *
 */
typedef VOID (*PF_S3_SPECIAL_PCI_RESTORE) (
  IN       ACCESS_WIDTH       AccessWidth,
  IN       PCI_ADDR           PciAddress,
  IN       VOID               *Value,
  IN       VOID               *StdHeader
  );

/**
 * MSR special case save handler
 *
 * @param[in]     MsrAddress    Address of model specific register to save
 * @param[out]    Value         Value read from the register
 * @param[in]     ConfigPtr     AMD standard header config parameter
 *
 */
typedef VOID (*PF_S3_SPECIAL_MSR_SAVE) (
  IN       UINT32             MsrAddress,
     OUT   UINT64             *Value,
  IN       VOID               *StdHeader
  );

/**
 * MSR special case restore handler
 *
 * @param[in]     MsrAddress    Address of model specific register to restore
 * @param[in]     Value         Value to write to the register
 * @param[in]     ConfigPtr     AMD standard header config parameter
 *
 */
typedef VOID (*PF_S3_SPECIAL_MSR_RESTORE) (
  IN       UINT32             MsrAddress,
  IN       UINT64             *Value,
  IN       VOID               *StdHeader
  );

/// PCI special case save/restore structure.
typedef struct {
  PF_S3_SPECIAL_PCI_SAVE     Save;    ///< Save routine
  PF_S3_SPECIAL_PCI_RESTORE  Restore; ///< Restore routine
} PCI_SPECIAL_CASE;

/// MSR special case save/restore structure.
typedef struct {
  PF_S3_SPECIAL_MSR_SAVE     Save;    ///< Save routine
  PF_S3_SPECIAL_MSR_RESTORE  Restore; ///< Restore routine
} MSR_SPECIAL_CASE;

/* Register related definitions */
/// S3 register type bit fields
typedef struct {
  UINT8      SpecialCaseIndex:4;      ///< Special Case array index
  UINT8      RegisterSize:3;          ///< For PCI, 1 = byte, 2 = word, else = dword.
                                      ///< For MSR, don't care
  UINT8      SpecialCaseFlag:1;       ///< Indicates special case
} S3_REGISTER_TYPE;

/// S3 PCI register descriptor.
typedef struct {
  S3_REGISTER_TYPE  Type;             ///< Type[7] = special case flag,
                                      ///< Type[6:3] = register size in bytes,
                                      ///< Type[2:0] = special case index
  UINT8             Function;         ///< PCI function of the register
  UINT16            Offset;           ///< PCI offset of the register
  UINT32            AndMask;          ///< AND mask to be applied to the value before saving
} PCI_REG_DESCRIPTOR;

/// S3 'conditional' PCI register descriptor.
typedef struct {
  S3_REGISTER_TYPE  Type;             ///< Type[7] = special case flag,
                                      ///< Type[6:3] = register size in bytes,
                                      ///< Type[2:0] = special case index
  UINT8             Function;         ///< PCI function of the register
  UINT16            Offset;           ///< PCI offset of the register
  UINT32            AndMask;          ///< AND mask to be applied to the value before saving
  UINT8             Mask1;            ///< conditional mask 1
  UINT8             Mask2;            ///< conditional mask 2
} CONDITIONAL_PCI_REG_DESCRIPTOR;

/// S3 MSR register descriptor.
typedef struct {
  S3_REGISTER_TYPE  Type;             ///< Type[7] = special case flag,
                                      ///< Type[6:3] = reserved,
                                      ///< Type[2:0] = special case index
  UINT32            Address;          ///< MSR address
  UINT64            AndMask;          ///< AND mask to be applied to the value before saving
} MSR_REG_DESCRIPTOR;

/// S3 'conditional' MSR register descriptor.
typedef struct {
  S3_REGISTER_TYPE  Type;             ///< Type[7] = special case flag,
                                      ///< Type[6:3] = reserved,
                                      ///< Type[2:0] = special case index
  UINT32            Address;          ///< MSR address
  UINT64            AndMask;          ///< AND mask to be applied to the value before saving
  UINT8             Mask1;            ///< conditional mask 1
  UINT8             Mask2;            ///< conditional mask 2
} CONDITIONAL_MSR_REG_DESCRIPTOR;

/// Common header at the beginning of an S3 register list.
typedef struct {
  UINT16     Version;                 ///< Version of header
  UINT16     NumRegisters;            ///< Number of registers in the list
} REGISTER_BLOCK_HEADER;

/// S3 PCI register list header.
typedef struct {
  UINT16                  Version;       ///< Version of header
  UINT16                  NumRegisters;  ///< Number of registers in the list
  CONST PCI_REG_DESCRIPTOR      *RegisterList; ///< Pointer to the first register descriptor
  CONST PCI_SPECIAL_CASE        *SpecialCases; ///< Pointer to array of special case handlers
} PCI_REGISTER_BLOCK_HEADER;

/// S3 'conditional' PCI register list header.
typedef struct {
  UINT16                          Version;       ///< Version of header
  UINT16                          NumRegisters;  ///< Number of registers in the list
  CONST CONDITIONAL_PCI_REG_DESCRIPTOR  *RegisterList; ///< Pointer to the first register descriptor
  CONST PCI_SPECIAL_CASE                *SpecialCases; ///< Pointer to array of special case handlers
} CPCI_REGISTER_BLOCK_HEADER;

/// S3 MSR register list header.
typedef struct {
  UINT16                 Version;       ///< Version of header
  UINT16                 NumRegisters;  ///< Number of registers in the list
  CONST MSR_REG_DESCRIPTOR     *RegisterList; ///< Pointer to the first register descriptor
  MSR_SPECIAL_CASE       *SpecialCases; ///< Pointer to array of special case handlers
} MSR_REGISTER_BLOCK_HEADER;

/// S3 'conditional' MSR register list header.
typedef struct {
  UINT16                          Version;       ///< Version of header
  UINT16                          NumRegisters;  ///< Number of registers in the list
  CONDITIONAL_MSR_REG_DESCRIPTOR  *RegisterList; ///< Pointer to the first register descriptor
  MSR_SPECIAL_CASE                *SpecialCases; ///< Pointer to array of special case handlers
} CMSR_REGISTER_BLOCK_HEADER;

/// S3 device descriptor pointers for ease of proper pointer advancement.
typedef union {
  DEVICE_DESCRIPTOR *CommonDeviceHeader;         ///< Common header
  PCI_DEVICE_DESCRIPTOR *PciDevice;              ///< PCI header
  CONDITIONAL_PCI_DEVICE_DESCRIPTOR *CPciDevice; ///< 'conditional' PCI header
  MSR_DEVICE_DESCRIPTOR *MsrDevice;              ///< MSR header
  CONDITIONAL_MSR_DEVICE_DESCRIPTOR *CMsrDevice; ///< 'conditional' MSR header
} DEVICE_DESCRIPTORS;

/// S3 register list header pointers for ease of proper pointer advancement.
typedef union {
  DEVICE_DESCRIPTOR          *CommonDeviceHeader; ///< Common header
  PCI_REGISTER_BLOCK_HEADER  *PciRegisters;       ///< PCI header
  CPCI_REGISTER_BLOCK_HEADER *CPciRegisters;      ///< 'conditional' PCI header
  MSR_REGISTER_BLOCK_HEADER  *MsrRegisters;       ///< MSR header
  CMSR_REGISTER_BLOCK_HEADER *CMsrRegisters;      ///< 'conditional' MSR header
} REGISTER_BLOCK_HEADERS;

/// S3 Volatile Storage Header
typedef struct {
  UINT32  HeapOffset;                             ///< Offset to beginning of heap data
  UINT32  HeapSize;                               ///< Size of the heap data
  UINT32  RegisterDataOffset;                     ///< Offset to beginning of raw save data
  UINT32  RegisterDataSize;                       ///< Size of raw save data
} S3_VOLATILE_STORAGE_HEADER;


/*---------------------------------------------------------------------------------------
 *                        F U N C T I O N    P R O T O T Y P E
 *---------------------------------------------------------------------------------------
 */
UINT32
GetWorstCaseContextSize (
  IN       DEVICE_BLOCK_HEADER *DeviceList,
  IN       CALL_POINTS         CallPoint,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
SaveDeviceListContext (
  IN       DEVICE_BLOCK_HEADER *DeviceList,
  IN       VOID                *Storage,
  IN       CALL_POINTS         CallPoint,
     OUT   UINT32              *ActualBufferSize,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
RestorePreESRContext (
     OUT   VOID **OrMaskPtr,
  IN       VOID *Storage,
  IN       CALL_POINTS       CallPoint,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
RestorePostESRContext (
  IN       VOID *OrMaskPtr,
  IN       VOID *Storage,
  IN       CALL_POINTS       CallPoint,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
AmdS3ParamsInitializer (
     OUT   AMD_S3_PARAMS *S3Params
  );

VOID
GetNonMemoryRelatedDeviceList (
     OUT   DEVICE_BLOCK_HEADER **NonMemoryRelatedDeviceList,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

AGESA_STATUS
S3GetPciDeviceRegisterList (
  IN       PCI_DEVICE_DESCRIPTOR     *Device,
     OUT   PCI_REGISTER_BLOCK_HEADER **RegisterHdr,
  IN       AMD_CONFIG_PARAMS         *StdHeader
  );

AGESA_STATUS
S3GetCPciDeviceRegisterList (
  IN       CONDITIONAL_PCI_DEVICE_DESCRIPTOR *Device,
     OUT   CPCI_REGISTER_BLOCK_HEADER        **RegisterHdr,
  IN       AMD_CONFIG_PARAMS                 *StdHeader
  );

AGESA_STATUS
S3GetMsrDeviceRegisterList (
  IN       MSR_DEVICE_DESCRIPTOR     *Device,
     OUT   MSR_REGISTER_BLOCK_HEADER **RegisterHdr,
  IN       AMD_CONFIG_PARAMS         *StdHeader
  );

AGESA_STATUS
S3GetCMsrDeviceRegisterList (
  IN       CONDITIONAL_MSR_DEVICE_DESCRIPTOR *Device,
     OUT   CMSR_REGISTER_BLOCK_HEADER        **RegisterHdr,
  IN       AMD_CONFIG_PARAMS                 *StdHeader
  );


#endif  // _S3_H_
