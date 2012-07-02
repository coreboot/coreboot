/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU Execution Cache Allocation functions.
 *
 * Contains code for doing Execution Cache Allocation for ROM space
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *
 * AMD is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with AMD.  This header does *NOT* give you permission to use the Materials
 * or any rights under AMD's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 *
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by AMD shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 *
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 *
 * AMD does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by AMD, or
 * result from use of the Materials or any related information.
 *
 * You agree that you will not reverse engineer or decompile the Materials.
 *
 * NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, AMD retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgement of AMD's proprietary rights in them.
 *
 * EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
 * direct product thereof will be exported directly or indirectly, into any
 * country prohibited by the United States Export Administration Act and the
 * regulations thereunder, without the required authorization from the U.S.
 * government nor will be used for any purpose prohibited by the same.
 ******************************************************************************
 */

#ifndef _CPU_CACHE_INIT_H_
#define _CPU_CACHE_INIT_H_

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
#define BSP_STACK_SIZE_64K        65536
#define BSP_STACK_SIZE_32K        32768

#define CORE0_STACK_SIZE          16384
#define CORE1_STACK_SIZE          4096

#define AMD_MTRR_FIX4K_BASE     0x268
#define AMD_MTRR_VARIABLE_BASE6 0x20C
#define AMD_MTRR_VARIABLE_BASE7 0x20E

#define WP_IO 0x0505050505050505ull

#define AGESA_CACHE_SIZE_REDUCED              1
#define AGESA_CACHE_REGIONS_ACROSS_1MB        2
#define AGESA_CACHE_REGIONS_ACROSS_4GB        3
#define AGESA_REGION_NOT_ALIGNED_ON_BOUNDARY  4
#define AGESA_CACHE_START_ADDRESS_LESS_D0000  5
#define AGESA_THREE_CACHE_REGIONS_ABOVE_1MB   6
#define AGESA_DEALLOCATE_CACHE_REGIONS        7

/*----------------------------------------------------------------------------
 *                         TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */
/// Cache-As-Ram Executable region allocation modes
typedef enum {
  LimitedByL2Size,                            ///< Execution space must be allocated from L2
  InfiniteExe,                                ///< Family can support unlimited Execution space
  MaxCarExeMode                               ///< Used as limit or bounds check
} CAR_EXE_MODE;

/// Cache Information
typedef struct {
  IN       UINT32  BspStackSize;              ///< Stack size of BSP
  IN       UINT32  Core0StackSize;            ///< Stack size of primary cores
  IN       UINT32  Core1StackSize;            ///< Stack size of all non primary cores
  IN       UINT32  MemTrainingBufferSize;     ///< Memory training buffer size
  IN       UINT32  SharedMemSize;             ///< Shared memory size
  IN       UINT64  VariableMtrrMask;          ///< Mask to apply before variable MTRR writes
  IN       UINT64  VariableMtrrHeapMask;      ///< Mask to apply before variable MTRR writes for use in heap init.
  IN       UINT64  HeapBaseMask;              ///< Mask used for the heap MTRR settings
  IN CAR_EXE_MODE  CarExeType;                ///< Indicates which algorithm to use when allocating EXE space
} CACHE_INFO;

/// Merged memory region overlap type
typedef enum {
  EmptySet,                                   ///< One of the regions is zero length
  Disjoint,                                   ///< The two regions do not touch
  Adjacent,                                   ///< one region is next to the other, no gap
  CommonEnd,                                  ///< regions overlap with a common end point
  Extending,                                  ///< the 2nd region is extending the size of the 1st
  Contained,                                  ///< the 2nd region is wholely contained inside the 1st
  CommonStartContained,                       ///< the 2nd region is contained in the 1st with a common start
  Identity,                                   ///< the two regions are the same
  CommonStartExtending,                       ///< the 2nd region has same start as 1st, but is larger size
  NotCombinable                               ///< the combined regions do not follow the cache block rules
} OVERLAP_TYPE;

/// Result of merging two memory regions for cache coverage
typedef struct {
  IN OUT  UINT32        MergedStartAddr;      ///< Start address of the merged regions
  IN OUT  UINT32        MergedSize;           ///< Size of the merged regions
     OUT  UINT32        OverlapAmount;        ///< the size of the overlapping section
     OUT  OVERLAP_TYPE  OverlapType;          ///< indicates how the two regions overlap
} MERGED_CACHE_REGION;

/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */
AGESA_STATUS
AllocateExecutionCache (
  IN AMD_CONFIG_PARAMS *StdHeader,
  IN EXECUTION_CACHE_REGION *AmdExeAddrMapPtr
  );

#endif  // _CPU_CACHE_INIT_H_

