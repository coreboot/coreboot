/** @file
  CXL GUID HOB definitions

  @copyright
  INTEL CONFIDENTIAL
  Copyright 2019 - 2021 Intel Corporation. <BR>

  The source code contained or described herein and all documents related to the
  source code ("Material") are owned by Intel Corporation or its suppliers or
  licensors. Title to the Material remains with Intel Corporation or its suppliers
  and licensors. The Material may contain trade secrets and proprietary    and
  confidential information of Intel Corporation and its suppliers and licensors,
  and is protected by worldwide copyright and trade secret laws and treaty
  provisions. No part of the Material may be used, copied, reproduced, modified,
  published, uploaded, posted, transmitted, distributed, or disclosed in any way
  without Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other intellectual
  property right is granted to or conferred upon you by disclosure or delivery
  of the Materials, either expressly, by implication, inducement, estoppel or
  otherwise. Any license under such intellectual property rights must be
  express and approved by Intel in writing.

  Unless otherwise agreed by Intel in writing, you may not remove or alter
  this notice or any other notice embedded in Materials by Intel or
  Intel's suppliers or licensors in any way.
**/

#ifndef _CXL_NODE_HOB_
#define _CXL_NODE_HOB_

#define CXL_NODE_HOB_GUID { 0xdd8ae009, 0xda5a, 0x44a3, { 0xbe, 0x18, 0xda, 0x0c, 0x16, 0xc5, 0xaf, 0x5c } }

#ifndef MAX_CXL_HDM_RANGES
#define MAX_CXL_HDM_RANGES        0x2        // Maximum number of CXL HDM ranges per CXL end device.
#endif

#ifndef MAX_CXL_PER_SOCKET
#define MAX_CXL_PER_SOCKET          8
#endif

#define CXL_NODE_ATTR_MEM           BIT0
#define CXL_NODE_ATTR_PERSISTENT    BIT1
#define CXL_NODE_ATTR_MEM_HW_INIT   BIT2
#define CXL_NODE_ATTR_ACCELERATOR   BIT3

typedef UINT32 CXL_NODE_ATTR;

#pragma pack(1)

//
// CDAT DSEMTS EFI Memory Type and Attribute
//
typedef enum {
  CxlEfiConventionalMemory        = 0x0,
  CxlEfiConventionalMemorySp      = 0x1,
  CxlEfiReservedMemoryType        = 0x2,
  MaxCxlEfiMemType
} CXL_EFI_MEM_TYPE;

typedef struct {
  UINT32              RdLatency;
  UINT32              WrLatency;
  UINT32              RdBandwidth;
  UINT32              WrBandwidth;
} CXL_PERF_DATA;

typedef struct {
  UINT64                       SerialNumber;
  UINT16                       VendorId;
  CXL_NODE_ATTR                Attr;
  UINT32                       Address;
  UINT32                       Size;
  UINT8                        Ways;
  UINT8                        SocketBitmap;
  CXL_EFI_MEM_TYPE             EfiMemType;
  CXL_PERF_DATA                InitiatorPerfData;        // Performance data between device egress and initiator.
  CXL_PERF_DATA                TargetPerfData;           // Performance data of entire target memory region.
  CXL_PERF_DATA                InitiatorTargetPerfData;  // performance data between initiator and the device memory.
} CXL_NODE_INFO;

typedef struct {
  UINT8            CxlNodeCount;
  //
  // CXL node info for UEFI memory map and ACPI tables construction
  //
  CXL_NODE_INFO    CxlNodeInfo[MAX_CXL_PER_SOCKET * MAX_CXL_HDM_RANGES];
} CXL_NODE_SOCKET;

#pragma pack()

#endif //#ifndef _CXL_NODE_HOB_