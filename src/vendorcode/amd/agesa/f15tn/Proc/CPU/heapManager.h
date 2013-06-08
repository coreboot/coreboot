/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Heap Manager and Heap Allocation APIs, and related functions.
 *
 * Contains code that initialize, maintain, and allocate the heap space.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 64574 $   @e \$Date: 2012-01-25 01:01:51 -0600 (Wed, 25 Jan 2012) $
 *
 */
/*
 ******************************************************************************
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
 ******************************************************************************
 */

#ifndef _HEAP_MANAGER_H_
#define _HEAP_MANAGER_H_

/*---------------------------------------------------------------------------------------
 *          M I X E D   (Definitions And Macros / Typedefs, Structures, Enums)
 *---------------------------------------------------------------------------------------
 */


/*---------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *---------------------------------------------------------------------------------------
 */
#define AMD_MTRR_VARIABLE_BASE0 0x200
#define AMD_MTRR_VARIABLE_HEAP_BASE 0x20A
#define AMD_MTRR_VARIABLE_HEAP_MASK (AMD_MTRR_VARIABLE_HEAP_BASE + 1)

#define AMD_HEAP_START_ADDRESS  0x400000ul
#define AMD_HEAP_REGION_END_ADDRESS 0xBFFFFFul
#define AMD_HEAP_SIZE_PER_CORE  0x010000ul
#define AMD_HEAP_INVALID_HEAP_OFFSET 0xFFFFFFFFul
#define AMD_HEAP_MTRR_MASK ((0xFFFFFFFFFFFFF800ull & (((UINT64)AMD_HEAP_SIZE_PER_CORE ^ (-1)) + 1)) | 0x800)
#define AMD_HEAP_SIZE_DWORD_PER_CORE  (AMD_HEAP_SIZE_PER_CORE / 4)

#define AMD_TEMP_TOM            0x20000000ul  // Set TOM to 512 MB (temporary value)
#define AMD_VAR_MTRR_ENABLE_BIT 0x100000ul    // bit 20

#define AMD_HEAP_RAM_ADDRESS    0xB0000ul

#define HEAP_SIGNATURE_VALID    0x50414548ul // Signature: 'HEAP'
#define HEAP_SIGNATURE_INVALID  0x00000000ul // Signature cleared

///Heap Manager Life cycle
#define HEAP_DO_NOT_EXIST_YET        1
#define HEAP_LOCAL_CACHE             2
#define HEAP_TEMP_MEM                3
#define HEAP_SYSTEM_MEM              4
#define HEAP_DO_NOT_EXIST_ANYMORE    5
#define HEAP_S3_RESUME               6
#define HEAP_RUNTIME_SYSTEM_MEM      7

///Heap callout
#define HEAP_CALLOUT_BOOTTIME        0
#define HEAP_CALLOUT_RUNTIME         1

#define AMD_MTRR_FIX64k_00000    0x250
#define AMD_MTRR_FIX16k_80000    0x258
#define AMD_MTRR_FIX16k_A0000    0x259
#define AMD_MTRR_FIX4k_C0000     0x268
#define AMD_MTRR_FIX4k_C8000     0x269
#define AMD_MTRR_FIX4k_D0000     0x26A
#define AMD_MTRR_FIX4k_D8000     0x26B
#define AMD_MTRR_FIX4k_E0000     0x26C
#define AMD_MTRR_FIX4k_E8000     0x26D
#define AMD_MTRR_FIX4k_F0000     0x26E
#define AMD_MTRR_FIX4k_F8000     0x26F

#define AMD_MTRR_FIX64K_WB_DRAM  0x1E
#define AMD_MTRR_FIX64K_WT_DRAM  0x1C
#define AMD_MTRR_FIX64K_UC_DRAM  0x18
#define AMD_MTRR_FIX16K_WB_DRAM  0x1E1E1E1E1E1E1E1Eull
#define AMD_MTRR_FIX16K_WT_DRAM  0x1C1C1C1C1C1C1C1Cull
#define AMD_MTRR_FIX16K_UC_DRAM  0x1818181818181818ull
#define AMD_MTRR_FIX4K_WB_DRAM   0x1E1E1E1E1E1E1E1Eull
#define AMD_MTRR_FIX4K_WT_DRAM   0x1C1C1C1C1C1C1C1Cull
#define AMD_MTRR_FIX4K_UC_DRAM   0x1818181818181818ull

/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */
/// Allocate Heap Parameters
typedef struct _ALLOCATE_HEAP_PARAMS {
  UINT32  RequestedBufferSize;                    ///< Size of buffer.
  UINT32  BufferHandle;                           ///< An unique ID of buffer.
  UINT8   Persist;                                ///< A flag. If marked, to be stored and passed to AmdInitLate.
  UINT8   *BufferPtr;                             ///< Pointer to buffer.
} ALLOCATE_HEAP_PARAMS;

/// Locate Heap Parameters
typedef struct _LOCATE_HEAP_PTR {
  UINT32  BufferHandle;                           ///< An unique ID of buffer.
  UINT32 BufferSize;                              ///< Data buffer size.
  UINT8   *BufferPtr;                             ///< Pointer to buffer.
} LOCATE_HEAP_PTR;

/// Heap Node Header
typedef struct _BUFFER_NODE {
  UINT32        BufferHandle;                     ///< An unique ID of buffer.
  UINT32        BufferSize;                       ///< Size of buffer.
  UINT8         Persist;                          ///< A flag. If marked, to be stored and passed to AmdInitLate.
  UINT8         PadSize;                          ///< Size of pad.
  UINT32        OffsetOfNextNode;                 ///< Offset of next node (relative to the base).
} BUFFER_NODE;

/// Heap Manager
typedef struct _HEAP_MANAGER {
  UINT32 Signature;                               ///< a signature to indicate if the heap is valid.
  UINT32 UsedSize;                                ///< Used size of heap.
  UINT32 FirstActiveBufferOffset;                 ///< Offset of the first active buffer.
  UINT32 FirstFreeSpaceOffset;                    ///< Offset of the first free space.
} HEAP_MANAGER;

/// AGESA Buffer Handles (These are reserved)
typedef enum {
  AMD_INIT_RESET_HANDLE = 0x000A000,              ///< Assign 0x000A000 buffer handle to AmdInitReset routine.
  AMD_INIT_EARLY_HANDLE,                          ///< Assign 0x000A001 buffer handle to AmdInitEarly routine.
  AMD_INIT_POST_HANDLE,                           ///< Assign 0x000A002 buffer handle to AmdInitPost routine.
  AMD_INIT_ENV_HANDLE,                            ///< Assign 0x000A003 buffer handle to AmdInitEnv routine.
  AMD_INIT_MID_HANDLE,                            ///< Assign 0x000A004 buffer handle to AmdInitMid routine.
  AMD_INIT_LATE_HANDLE,                           ///< Assign 0x000A005 buffer handle to AmdInitLate routine.
  AMD_INIT_RESUME_HANDLE,                         ///< Assign 0x000A006 buffer handle to AmdInitResume routine.
  AMD_LATE_RUN_AP_TASK_HANDLE,                    ///< Assign 0x000A007 buffer handle to AmdLateRunApTask routine.
  AMD_S3_SAVE_HANDLE,                             ///< Assign 0x000A008 buffer handle to AmdS3Save routine.
  AMD_S3_LATE_RESTORE_HANDLE,                     ///< Assign 0x000A009 buffer handle to AmdS3LateRestore routine.
  AMD_S3_SCRIPT_SAVE_TABLE_HANDLE,                ///< Assign 0x000A00A buffer handle to be used for S3 save table
  AMD_S3_SCRIPT_TEMP_BUFFER_HANDLE,               ///< Assign 0x000A00B buffer handle to be used for S3 save table
  AMD_CPU_AP_TASKING_HANDLE,                      ///< Assign 0x000A00C buffer handle to AP tasking input parameters.
  AMD_REC_MEM_SOCKET_HANDLE,                      ///< Assign 0x000A00D buffer handle to save socket with memory in memory recovery mode.
  AMD_MEM_AUTO_HANDLE,                            ///< Assign 0x000A00E buffer handle to AmdMemAuto routine.
  AMD_MEM_SPD_HANDLE,                             ///< Assign 0x000A00F buffer handle to AmdMemSpd routine.
  AMD_MEM_DATA_HANDLE,                            ///< Assign 0x000A010 buffer handle to MemData
  AMD_MEM_TRAIN_BUFFER_HANDLE,                    ///< Assign 0x000A011 buffer handle to allocate buffer for training
  AMD_MEM_S3_DATA_HANDLE,                         ///< Assign 0x000A012 buffer handle to special case register for S3
  AMD_MEM_S3_NB_HANDLE,                           ///< Assign 0x000A013 buffer handle to NB block for S3
  AMD_MEM_S3_MR0_DATA_HANDLE,                     ///< Assign 0x000A014 buffer handle to MR0 data block for S3
  AMD_UMA_INFO_HANDLE,                            ///< Assign 0x000A015 buffer handle to be used for Uma information
  AMD_DMI_MEM_DEV_INFO_HANDLE,                    ///< Assign 0x000A016 buffer handle to DMI Type16 17 19 20 information
  HT_STATE_DATA_HANDLE,                           ///< Assign 0x000A017 buffer handle to HT State Data
  PRESERVE_MAIL_BOX_HANDLE,                       ///< Assign 0x000A018 buffer handle for Preserve Mailbox Feature.
  EVENT_LOG_BUFFER_HANDLE,                        ///< Assign 0x000A019 buffer handle to Event Log
  IDS_CONTROL_HANDLE,                             ///< Assign 0x000A01A buffer handle to AmdIds routine.
  IDS_HT_DATA_HANDLE,                             ///< Assign 0x000A01B buffer handle to Ht IDS control
  IDS_HDT_OUT_BUFFER_HANDLE,                      ///< Assign 0x000A01C buffer handle to be used for HDTOUT support.
  IDS_CHECK_POINT_PERF_HANDLE,                    ///< Assign 0x000A01D buffer handle to Performance analysis
  AMD_PCIE_COMPLEX_DATA_HANDLE,                   ///< Assign 0x000A01E buffer handle to be used for PCIe support
  AMD_MEM_SYS_DATA_HANDLE,                        ///< Assign 0x000A01F buffer handle to be used for memory data structure
  AMD_GNB_SMU_CONFIG_HANDLE,                      ///< Assign 0x000A020 buffer handle to be used for GNB SMU configuration
  AMD_PP_FUSE_TABLE_HANDLE,                       ///< Assign 0x000A021 buffer handle to be used for TT fuse table
  AMD_GFX_PLATFORM_CONFIG_HANDLE,                 ///< Assign 0x000A022 buffer handle to be used for Gfx platform configuration
  AMD_GNB_TEMP_DATA_HANDLE,                       ///< Assign 0x000A024 buffer handle for GNB general purpose data block
  AMD_MEM_2D_RDQS_HANDLE,                         ///< Assign 0x000A025 buffer handle for 2D training
  AMD_GNB_IOMMU_SCRATCH_MEM_HANDLE,               ///< Assign 0x000A026 buffer handle to be used for GNB IOMMU scratch memory
  AMD_MEM_S3_SAVE_HANDLE,                         ///< Assign 0x000A027 buffer handle for memory data saved right after memory init
  AMD_MEM_2D_RDQS_RIM_HANDLE,                     ///< Assign 0x000A028 buffer handle for 2D training Eye RIM Search
  AMD_CPU_NB_PSTATE_FIXUP_HANDLE,                 ///< Assign 0x000A029 buffer handle for an NB P-state workaround
  AMD_MEM_CRAT_INFO_BUFFER_HANDLE,                ///< Assign 0x000A02B buffer handle for CRAT Memory affinity component structure
  AMD_MEM_MISC_HANDLES_START = 0x1000000,         ///< Reserve 0x1000000 to 0x1FFFFFF buffer handle
  AMD_MEM_MISC_HANDLES_END = 0x1FFFFFF,           ///< miscellaneous memory init tasks' buffers.
  AMD_HEAP_IN_MAIN_MEMORY_HANDLE = 0x8000000,     ///< Assign 0x8000000 to AMD_HEAP_IN_MAIN_MEMORY_HANDLE.
  SOCKET_DIE_MAP_HANDLE = 0x534F4B54,             ///< 'sokt'
  NODE_ID_MAP_HANDLE = 0x4E4F4445,                ///< 'node'
  HOP_COUNT_TABLE_HANDLE = 0x484F5053,            ///< 'hops'
  LOCAL_AP_MAIL_BOX_CACHE_HANDLE = 0x414D4258,    ///< 'ambx'
  AMD_FCH_RESET_DATA_BLOCK_HANDLE = 0x46434852,   ///< 'FCHR' Buffer handle for FCH private data block at InitReset
  AMD_FCH_DATA_BLOCK_HANDLE = 0x46434845,         ///< 'FCHE' Buffer handle for FCH private data block at InitEnv
  IDS_TRAP_TABLE_HANDLE = 0x49524547,             ///< 'IREG' Handle for IDS register table
  IDS_SAVE_IDTR_HANDLE = 0x49445452,              ///< 'IDTR'
  IDS_BSC_IDT_HANDLE = 0x42534349,                ///< 'BSCI' BSC Idt table
  IDS_NV_TO_CMOS_HANDLE = 0x534D4349,             ///< 'ICMS' Handle for IDS CMOS save
  IDS_GRA_HANDLE = 0x41524749,                    ///< 'IGRA' Handle for IDS GRA save
  IDS_EXTEND_HANDLE = 0x54584549                  ///< 'IEXT' Handle for IDS extend module
} AGESA_BUFFER_HANDLE;


/*---------------------------------------------------------------------------------------
 *                        F U N C T I O N    P R O T O T Y P E
 *---------------------------------------------------------------------------------------
 */

AGESA_STATUS
HeapManagerInit (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
HeapAllocateBuffer (
  IN OUT   ALLOCATE_HEAP_PARAMS *AllocateHeapParams,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
HeapDeallocateBuffer (
  IN       UINT32 BufferHandle,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
HeapLocateBuffer (
  IN OUT   LOCATE_HEAP_PTR *LocateHeap,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

UINT64
HeapGetBaseAddress (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
EventLogInitialization (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );
#endif // _HEAP_MANAGER_H_
