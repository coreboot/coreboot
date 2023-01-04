/**
Copyright (c) 2019-2021, Intel Corporation. All rights reserved.<BR>

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.
* Neither the name of Intel Corporation nor the names of its contributors may
  be used to endorse or promote products derived from this software without
  specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
  THE POSSIBILITY OF SUCH DAMAGE.

**/


#ifndef _HOB_IIOUDS_H_
#define _HOB_IIOUDS_H_

#include <fsp/util.h>

#include "defs_iio.h"
#include "IioUniversalDataHob.h"

#define FSP_HOB_IIO_UNIVERSAL_DATA_GUID { \
	0xa1, 0x96, 0xf3, 0x7f, 0x7d, 0xee, 0x1e, 0x43, \
	0xba, 0x53, 0x8f, 0xCa, 0x12, 0x7c, 0x44, 0xc0  \
}

#define NUMBER_PORTS_PER_SOCKET  MAX_IIO_PORTS_PER_SOCKET

#pragma pack(1)

/**
 * IIO Stacks
 *
 * Ports(XCC)  Stack(HOB)  IioConfigIou
 * =========================================
 * 0           stack 0     DmiAsPcie
 * 1A..1H      stack 1     IOU0
 * 2A..2H      stack 2     IOU1
 * 3A..3H      stack 3     IOU2
 * 4A..4H      stack 4     IOU3
 * 5A..5H      stack 5     IOU4
 * 6A..6H      stack 6     IOU5
 */

/**
 IIO PCIe Ports
 **/

typedef enum {
	// DmiAsPcie, stack 0
	PORT_0 = 0,
	// IOU0, stack 1
	PORT_1A,
	PORT_1B,
	PORT_1C,
	PORT_1D,
	PORT_1E,
	PORT_1F,
	PORT_1G,
	PORT_1H,
	// IOU1, stack 2
	PORT_2A,
	PORT_2B,
	PORT_2C,
	PORT_2D,
	PORT_2E,
	PORT_2F,
	PORT_2G,
	PORT_2H,
	// IOU2, stack 3
	PORT_3A,
	PORT_3B,
	PORT_3C,
	PORT_3D,
	PORT_3E,
	PORT_3F,
	PORT_3G,
	PORT_3H,
	// IOU3, stack 4
	PORT_4A,
	PORT_4B,
	PORT_4C,
	PORT_4D,
	PORT_4E,
	PORT_4F,
	PORT_4G,
	PORT_4H,
	// IOU4, stack 5
	PORT_5A,
	PORT_5B,
	PORT_5C,
	PORT_5D,
	PORT_5E,
	PORT_5F,
	PORT_5G,
	PORT_5H,
	// IOU5, stack 6, not used for socket 0; for socket 1, swap from DMI as x8
	PORT_6A,
	PORT_6B,
	PORT_6C,
	PORT_6D,
	PORT_6E,
	PORT_6F,
	PORT_6G,
	PORT_6H,
	// IOU6, stack 7, not used
	PORT_7A,
	PORT_7B,
	PORT_7C,
	PORT_7D,
	PORT_7E,
	PORT_7F,
	PORT_7G,
	PORT_7H,

	MAX_PORTS
} PCIE_PORTS;

typedef enum {
  //for ICX
  IioStack0 = 0,	// DmiAsPcie
  IioStack1 = 1,	// IOU0
  IioStack2 = 2,	// IOU1
  IioStack3 = 3,	// IOU2
  IioStack4 = 4,	// IOU3
  IioStack5 = 5,	// IOU4
  //for SPR
  IioStack6 = 6,	// IOU5
  IioStack7 = 7,
  IioStack8 = 8,
  IioStack9 = 9,
  IioStack10 = 10,
  IioStack11 = 11,
  //for later SOC
  IioStack12 = 12,
  IioStack13 = 13,
  IioStack14 = 14,
  IioStack15 = 15,
  IioStack16 = 16,
  IioStack17 = 17,
  IioStack18 = 18,
  IioStackUnknown = 0xFF
} IIO_STACK;

typedef enum {
	TYPE_UBOX = 0,
	TYPE_UBOX_IIO,
	TYPE_MCP,
	TYPE_FPGA,
	TYPE_HFI,
	TYPE_NAC,
	TYPE_GRAPHICS,
	TYPE_DINO,
	TYPE_RESERVED,
	TYPE_DISABLED,              // This item must be prior to stack specific disable types
	TYPE_UBOX_IIO_DIS,
	TYPE_MCP_DIS,
	TYPE_FPGA_DIS,
	TYPE_HFI_DIS,
	TYPE_NAC_DIS,
	TYPE_GRAPHICS_DIS,
	TYPE_DINO_DIS,
	TYPE_RESERVED_DIS,
	TYPE_NONE
} STACK_TYPE;

#pragma pack()

#endif
