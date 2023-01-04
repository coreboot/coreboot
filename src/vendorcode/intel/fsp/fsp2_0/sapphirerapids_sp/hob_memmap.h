/** @file
  GUID used for Memory Map Data entries in the HOB list.

  @copyright
  INTEL CONFIDENTIAL
  Copyright 1999 - 2021 Intel Corporation. <BR>

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

#ifndef _HOB_MEMMAP_H_
#define _HOB_MEMMAP_H_

#include "defs_memmap.h"
#include "MemoryMapDataHob.h"

#define FSP_SYSTEM_MEMORYMAP_HOB_GUID { \
	0x15, 0x00, 0x87, 0xf8, 0x94, 0x69, 0x98, 0x4b, 0x95, 0xa2, \
	0xbd, 0x56, 0xda, 0x91, 0xc0, 0x7f \
	}

#define MAX_IMC_PER_SOCKET                MAX_IMC
#define MAX_SRAT_MEM_ENTRIES_PER_IMC      8
#define MAX_SMB_INSTANCE                  2
#define MAX_ACPI_MEMORY_AFFINITY_COUNT ( \
	MAX_SOCKET * MAX_IMC_PER_SOCKET * MAX_SRAT_MEM_ENTRIES_PER_IMC \
	)
#define AMT_MAX_NODE        ((MAX_AMT)*(MAX_SOCKET))   // Max abstract memory target for all sockets

#endif // _HOB_MEMMAP_H_
