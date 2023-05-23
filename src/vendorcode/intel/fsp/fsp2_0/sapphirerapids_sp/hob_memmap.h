/** @file

Copyright (c) 2019-2023, Intel Corporation. All rights reserved.<BR>

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
