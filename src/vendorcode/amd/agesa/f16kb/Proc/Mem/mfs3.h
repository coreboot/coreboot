/* $NoKeywords:$ */
/**
 * @file
 *
 * mfS3.h
 *
 * S3 resume memory related functions.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Feat/S3)
 * @e \$Revision: 84150 $ @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 **/
/*****************************************************************************
*
 * Copyright (c) 2008 - 2013, Advanced Micro Devices, Inc.
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

#ifndef _MFS3_H_
#define _MFS3_H_

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */

/*-----------------------------------------------------------------------------
 *                         DEFINITIONS AND MACROS
 *
 *-----------------------------------------------------------------------------
 */
#define PRESELFREF 0
#define POSTSELFREF 1
#define DCT0 0
#define DCT1 1
#define DCT0_MASK 0x1
#define DCT1_MASK 0x2
#define DCT0_NBPSTATE_SUPPORT_MASK 0x4
#define DCT1_NBPSTATE_SUPPORT_MASK 0x8
#define DCT0_DDR3_MASK 0x10
#define DCT1_DDR3_MASK 0x20
#define NODE_WITHOUT_DIMM_MASK 0x80
#define DCT0_ANY_DIMM_MASK 0x55
#define DCT1_ANY_DIMM_MASK 0xAA
#define ANY_DIMM_MASK 0xFF

#define DCT_PHY_FLAG 0
#define DCT_EXTRA_FLAG 1
#define SET_S3_SPECIAL_OFFSET(AccessType, Dct, Offset) ((AccessType << 11) | (Dct << 10) | Offset)

#define RESTORE_TRAINING_MODE 1
#define CAPSULE_REBOOT_MODE 2
#define S3_RESUME_MODE 4
/*----------------------------------------------------------------------------
 *                         TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */
/// struct for all the descriptor for pre exit self refresh and post exit self refresh
typedef struct _DESCRIPTOR_GROUP {
  PCI_DEVICE_DESCRIPTOR PCIDevice[2];  ///< PCI device descriptor
  CONDITIONAL_PCI_DEVICE_DESCRIPTOR CPCIDevice[2];  ///< Conditional PCI device descriptor
  MSR_DEVICE_DESCRIPTOR MSRDevice[2];  ///< MSR device descriptor
  CONDITIONAL_MSR_DEVICE_DESCRIPTOR CMSRDevice[2];  ///< Conditional MSR device descriptor
} DESCRIPTOR_GROUP;

/// Northbridge block to be used in S3 resume and save.
typedef struct _S3_MEM_NB_BLOCK {
  UINT8 MemS3SpecialCaseHeapSize; ///< Heap size for the special case register heap.
  struct _MEM_NB_BLOCK *NBPtr;    ///< Pointer to the north bridge block.
  VOID (*MemS3ExitSelfRefReg) (MEM_NB_BLOCK *NBPtr, AMD_CONFIG_PARAMS *StdHeaderPtr); ///< S3 Exit self refresh register
  VOID (*MemS3GetConPCIMask) (MEM_NB_BLOCK *NBPtr, DESCRIPTOR_GROUP *DescriptPtr); ///< Get conditional mask for PCI register setting
  VOID (*MemS3GetConMSRMask) (MEM_NB_BLOCK *NBPtr, DESCRIPTOR_GROUP *DescriptPtr); ///< Get conditional mask for MSR register setting
  UINT16 (*MemS3GetRegLstPtr) (MEM_NB_BLOCK *NBPtr, DESCRIPTOR_GROUP *DescriptPtr); ///< Get register list pointer for both PCI and MSR register
  BOOLEAN (*MemS3Resume) (struct _S3_MEM_NB_BLOCK *S3NBPtr, UINT8 NodeID);///< Exit Self Refresh
  VOID (*MemS3RestoreScrub) (MEM_NB_BLOCK *NBPtr, UINT8 NodeID);///< Restore scrubber base
  AGESA_STATUS (*MemS3GetDeviceRegLst) (UINT32 ReigsterLstID, VOID **RegisterHeader); ///< Get register list for a device
} S3_MEM_NB_BLOCK;

/// Header for heap space to store the special case register.
typedef struct _S3_SPECIAL_CASE_HEAP_HEADER {
  UINT8 Node;  ///< Node ID for the the header
  UINT8 Offset;  ///< Offset for the target node
} S3_SPECIAL_CASE_HEAP_HEADER;

/// Flag for hob data save and restore
typedef enum _MEM_HOB_DATA_TYPE {
  S3_UMA_SIZE,  ///< UMA size
  S3_UMA_BASE,  ///< UMA base
  S3_UMA_MODE,  ///< UMA mode
  S3_SUB_4G_CACHE_TOP,  ///< Sub 4G Cache Top
  S3_SYSLIMIT,  ///< System limit
  S3_UMA_ATTRIBUTE,  ///< UMA attribute
  S3_VDDIO  ///< VDDIO
} MEM_HOB_DATA_TYPE;

/// Header for heap space to store reduced memory internal data
typedef struct _REDUCED_MEM_BLOCK_HEAP_HEADER {
  UINT16 Version;                 ///< Version of header
  UINT8  NumNodes;                ///< Number of reduced NB blocks in the list
} REDUCED_MEM_BLOCK_HEAP_HEADER;

/// Reduced NB Block to store data during memory context save/restore
typedef struct _REDUCED_NB_BLOCK {
  UINT32 NodeMemSize;             ///< Base[47:16], total DRAM size controlled by both DCT0 and DCT1 of this Node.
  UINT32 NodeSysBase;             ///< System base of this node
  UINT8  NumDcts;                 ///< Number of reduced DCT blocks in the list
} REDUCED_NB_BLOCK;

/// Reduced DCT Block to store data during memory context save/restore
typedef struct _REDUCED_DCT_BLOCK {
  UINT8  Dct;                     ///< Dct Number
  UINT32 DctMemSize;              ///< Base[47:16], total DRAM size controlled by this DCT.
  UINT8  EnabledChipSels;         ///< Number of enabled chip selects on current DCT
  UINT8  BankAddrMap;             ///< Bank Address Mapping
  BOOLEAN BkIntDis;               ///< Bank interleave requested but not enabled on current DCT
} REDUCED_DCT_BLOCK;

/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */
AGESA_STATUS
AmdMemS3Resume (
  IN   AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
MemS3ResumeInitNB (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
MemS3Deallocate (
  IN   AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
MemFS3GetPciDeviceRegisterList (
  IN       PCI_DEVICE_DESCRIPTOR     *Device,
     OUT   PCI_REGISTER_BLOCK_HEADER **RegisterHdr,
  IN       AMD_CONFIG_PARAMS         *StdHeader
  );

AGESA_STATUS
MemFS3GetCPciDeviceRegisterList (
  IN       CONDITIONAL_PCI_DEVICE_DESCRIPTOR *Device,
     OUT   CPCI_REGISTER_BLOCK_HEADER        **RegisterHdr,
  IN       AMD_CONFIG_PARAMS                 *StdHeader
  );

AGESA_STATUS
MemFS3GetMsrDeviceRegisterList (
  IN       MSR_DEVICE_DESCRIPTOR     *Device,
     OUT   MSR_REGISTER_BLOCK_HEADER **RegisterHdr,
  IN       AMD_CONFIG_PARAMS         *StdHeader
  );

AGESA_STATUS
MemFS3GetCMsrDeviceRegisterList (
  IN       CONDITIONAL_MSR_DEVICE_DESCRIPTOR     *Device,
     OUT   CMSR_REGISTER_BLOCK_HEADER            **RegisterHdr,
  IN       AMD_CONFIG_PARAMS                     *StdHeader
  );

AGESA_STATUS
MemFS3GetDeviceList (
  IN OUT   DEVICE_BLOCK_HEADER **DeviceBlockHdrPtr,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
MemFS3Wait10ns (
  IN       UINT32 Count,
  IN OUT   MEM_DATA_STRUCT *MemPtr
  );

BOOLEAN
MemNS3ResumeUNb (
  IN OUT   S3_MEM_NB_BLOCK *S3NBPtr,
  IN       UINT8 NodeID
  );

VOID
MemNS3GetBitFieldNb (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  );

VOID
MemNS3SetBitFieldNb (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  );

VOID
MemNS3RestoreScrubNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Node
  );

AGESA_STATUS
MemS3InitNB (
  IN OUT   S3_MEM_NB_BLOCK **S3NBPtr,
  IN OUT   MEM_DATA_STRUCT **MemPtr,
  IN OUT   MEM_MAIN_DATA_BLOCK *mmData,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
MemNS3SetPreDriverCalUnb (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  );

BOOLEAN
MemNS3DctCfgSelectUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       VOID *Dct
  );

VOID
MemNS3GetNBPStateDepRegUnb (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  );

VOID
MemNS3SetNBPStateDepRegUnb (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  );

VOID
MemNS3SaveNBRegisterUnb (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  );

VOID
MemNS3RestoreNBRegisterUnb (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  );

VOID
MemNS3SetMemClkFreqValUnb (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  );

VOID
MemNS3ChangeMemPStateContextNb (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  );

VOID
MemNS3ForceNBP0Unb (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  );

VOID
MemNS3ReleaseNBPSUnb (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  );

VOID
MemNSaveHobDataUnb (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  );

VOID
MemNRestoreHobDataUnb (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  );
#endif //_MFS3_H_
