/** @file

Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>

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

#ifndef __DEFS_MEMMAP_H__
#define __DEFS_MEMMAP_H__

/* Bit definitions for RasModes */
#define CH_INDEPENDENT		0
#define FULL_MIRROR_1LM		BIT0
#define FULL_MIRROR_2LM		BIT1
#define CH_LOCKSTEP		BIT2
#define RK_SPARE		BIT3
#define PARTIAL_MIRROR_1LM	BIT5
#define PARTIAL_MIRROR_2LM	BIT6
#define STAT_VIRT_LOCKSTEP	BIT7

#define MEMTYPE_1LM_MASK       (1 << 0)
#define MEMTYPE_2LM_MASK       (1 << 1)
#define MEMTYPE_VOLATILE_MASK  (MEMTYPE_1LM_MASK | MEMTYPE_2LM_MASK)

/* ACPI SRAT Memory Flags */
#define SRAT_ACPI_MEMORY_ENABLED               (1 << 0)
#define SRAT_ACPI_MEMORY_HOT_REMOVE_SUPPORTED  (1 << 1)
#define SRAT_ACPI_MEMORY_NONVOLATILE           (1 << 2)

#define MEM_TYPE_RESERVED (1 << 8)
#define MEM_ADDR_64MB_SHIFT_BITS 26

//------------------------------------------------------------------------------------
// Uncomment line(s) below to override macro definitions in FSP MemoryMapDataHob.h
//------------------------------------------------------------------------------------
// #define MAX_UNIQUE_NGN_DIMM_INTERLEAVE    2
// #define MAX_SPARE_RANK                    2
// #define MAX_SOCKET                4     // Potentially need to refactor
// #define MAX_IMC                   4   // Potentially need to refactor
// #define MAX_HBM_IO                        4
// #define MAX_MC_CH                         2
// #define MAX_DIMM                          2                         // Max DIMM per channel
// #define MAX_RANK_DIMM                     2
// #define MAX_DRAM_CLUSTERS                 4
// #define MAX_SAD_RULES                     16
// #define MAX_FPGA_REMOTE_SAD_RULES         2     // Maximum FPGA sockets exists on ICX platform

#endif