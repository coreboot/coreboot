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
 * @e \$Revision: 46142 $   @e \$Date: 2011-01-29 05:35:36 +0800 (Sat, 29 Jan 2011) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (c) 2011, Advanced Micro Devices, Inc.
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


/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "Ids.h"
#include "cpuRegisters.h"
#include "Topology.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "cpuFamilyTranslation.h"
#include "cpuCacheInit.h"
#include "heapManager.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_CPU_FEATURE_CPUCACHEINIT_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
// 4G - 1, ~max ROM space
#define SIZE_INFINITE_EXE_CACHE 0xFFFFFFFF

/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *             L2 cache Association to Way translation table
 *----------------------------------------------------------------------------
 */
CONST UINT8 ROMDATA L2AssocToL2WayTranslationTable[] =
{
  0,
  1,
  2,
  0xFF,
  4,
  0xFF,
  8,
  0xFF,
  16,
  0xFF,
  32,
  48,
  64,
  96,
  128,
  0xFF,
};


/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
UINT8
STATIC
Ceiling (
  IN UINT32 Divisor,
  IN UINT32 Dividend
  );

UINT32
STATIC
CalculateOccupiedExeCache (
  IN AMD_CONFIG_PARAMS *StdHeader
  );

VOID
STATIC
CompareRegions (
  IN      EXECUTION_CACHE_REGION  ARegion,
  IN      EXECUTION_CACHE_REGION  BRegion,
  IN OUT  MERGED_CACHE_REGION     *CRegion,
  IN      AMD_CONFIG_PARAMS       *StdHeader
  );

BOOLEAN
STATIC
IsPowerOfTwo (
  IN      UINT32                  TestNumber
  );

/*---------------------------------------------------------------------------------------*/
/**
 * This function will setup ROM execution cache.
 *
 * The execution cache regions are passed in, the max number of execution cache regions
 * is three.  Several rules are checked for compliance. If a rule test fails then one of
 * these error suffixes will be added to the general CPU_EVENT_EXECUTION_CACHE_ALLOCATION_ERROR
 * in the SubReason field
 *   -1  available cache size is less than requested, the ROM execution cache
 *       region has been reduced or eliminated.
 *   -2  at least one execution cache region crosses the 1MB line, the ROM execution
 *       cache size has been reduced.
 *   -3  at least one execution cache region crosses the 4GB line, the ROM execution
 *       cache size has been reduced.
 *   -4  the start address of a region is not at the boundary of cache size,
 *        the starting address has been adjusted downward
 *   -5  execution cache start address less than D0000, request is ignored
 *   -6  more than 2 execution cache regions are above 1MB, request is ignored
 * If the start address of all three regions are zero, then no execution cache is allocated.
 *
 * @param[in]   StdHeader          Handle to config for library and services
 * @param[in]   AmdExeAddrMapPtr   Pointer to the start of EXECUTION_CACHE_REGION array
 *
 * @retval      AGESA_SUCCESS      No error
 * @retval      AGESA_WARNING      AGESA_CACHE_SIZE_REDUCED; AGESA_CACHE_REGIONS_ACROSS_1MB;
 *                                 AGESA_CACHE_REGIONS_ACROSS_4GB;
 * @retval      AGESA_ERROR        AGESA_REGION_NOT_ALIGNED_ON_BOUNDARY;
 *                                 AGESA_CACHE_START_ADDRESS_LESS_D0000;
 *                                 AGESA_THREE_CACHE_REGIONS_ABOVE_1MB;
 *
 */
AGESA_STATUS
AllocateExecutionCache (
  IN AMD_CONFIG_PARAMS *StdHeader,
  IN EXECUTION_CACHE_REGION *AmdExeAddrMapPtr
  )
{
  AGESA_STATUS               AgesaStatus;
  AMD_GET_EXE_SIZE_PARAMS    AmdGetExeSize;
  UINT32                     CurrentAllocatedExeCacheSize;
  UINT32                     RemainingExecutionCacheSize;
  UINT64                     MsrData;
  UINT64                     SecondMsrData;
  UINT32                     RequestStartAddr;
  UINT32                     RequestSize;
  UINT32                     StartFixMtrr;
  UINT32                     CurrentMtrr;
  UINT32                     EndFixMtrr;
  UINT8                      i;
  UINT8                      Ignored;
  CACHE_INFO                 *CacheInfoPtr;
  CPU_SPECIFIC_SERVICES      *FamilySpecificServices;
  EXECUTION_CACHE_REGION      MtrrV6;
  EXECUTION_CACHE_REGION      MtrrV7;
  MERGED_CACHE_REGION         Result;

  //
  // If start addresses of all three regions are zero, then return early
  //
  if (AmdExeAddrMapPtr[0].ExeCacheStartAddr == 0) {
    if (AmdExeAddrMapPtr[1].ExeCacheStartAddr == 0) {
      if (AmdExeAddrMapPtr[2].ExeCacheStartAddr == 0) {
        // No regions defined by the caller
        return AGESA_SUCCESS;
      }
    }
  }

  // Get available cache size for ROM execution
  AmdGetExeSize.StdHeader = *StdHeader;
  AgesaStatus = AmdGetAvailableExeCacheSize (&AmdGetExeSize);
  CurrentAllocatedExeCacheSize = CalculateOccupiedExeCache (StdHeader);
  ASSERT (CurrentAllocatedExeCacheSize <= AmdGetExeSize.AvailableExeCacheSize);
  IDS_HDT_CONSOLE (CPU_TRACE, "  Cache size available for execution cache: 0x%x\n", AmdGetExeSize.AvailableExeCacheSize);
  RemainingExecutionCacheSize = AmdGetExeSize.AvailableExeCacheSize - CurrentAllocatedExeCacheSize;

  GetCpuServicesOfCurrentCore ((const CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
  FamilySpecificServices->GetCacheInfo (FamilySpecificServices, (const VOID **)&CacheInfoPtr, &Ignored, StdHeader);

  // Process each request entry 0 to 2
  for (i = 0; i < 3; i++) {
    // Exit if no more cache available
    if (RemainingExecutionCacheSize == 0) {
      break;
    }

    // Skip the region if ExeCacheSize = 0
    if (AmdExeAddrMapPtr[i].ExeCacheSize == 0) {
      continue;
    }

    // Align starting addresses on 32K boundary
    AmdExeAddrMapPtr[i].ExeCacheStartAddr =
      AmdExeAddrMapPtr[i].ExeCacheStartAddr & 0xFFFF8000;

    // Adjust size to multiple of 32K (rounding up)
    if ((AmdExeAddrMapPtr[i].ExeCacheSize % 0x8000) != 0) {
      AmdExeAddrMapPtr[i].ExeCacheSize = ((AmdExeAddrMapPtr[i].ExeCacheSize + 0x8000) & 0xFFFF8000);
    }

    // Boundary alignment check and confirm size is an even power of two
    if ( !IsPowerOfTwo (AmdExeAddrMapPtr[i].ExeCacheSize) ||
    ((AmdExeAddrMapPtr[i].ExeCacheStartAddr % AmdExeAddrMapPtr[i].ExeCacheSize) != 0) ) {
      AgesaStatus = AGESA_ERROR;
      PutEventLog (AgesaStatus,
                   (CPU_EVENT_EXECUTION_CACHE_ALLOCATION_ERROR + AGESA_REGION_NOT_ALIGNED_ON_BOUNDARY),
                   i, AmdExeAddrMapPtr[i].ExeCacheStartAddr, AmdExeAddrMapPtr[i].ExeCacheSize, 0, StdHeader);
      break;
    }

    // Check start address boundary
    if (AmdExeAddrMapPtr[i].ExeCacheStartAddr < 0xD0000) {
      AgesaStatus = AGESA_ERROR;
      PutEventLog (AgesaStatus,
                   (CPU_EVENT_EXECUTION_CACHE_ALLOCATION_ERROR + AGESA_CACHE_START_ADDRESS_LESS_D0000),
                   i, AmdExeAddrMapPtr[i].ExeCacheStartAddr, AmdExeAddrMapPtr[i].ExeCacheSize, 0, StdHeader);
      break;
    }

    if (CacheInfoPtr->CarExeType == LimitedByL2Size) {
      // Verify available execution cache size for region 0 to 2 request
      if (RemainingExecutionCacheSize < AmdExeAddrMapPtr[i].ExeCacheSize) {
        // Request is larger than available, reduce the allocation & report the change
        AmdExeAddrMapPtr[i].ExeCacheSize = RemainingExecutionCacheSize;
        RemainingExecutionCacheSize = 0;
        AgesaStatus = AGESA_WARNING;
        PutEventLog (AgesaStatus,
                     (CPU_EVENT_EXECUTION_CACHE_ALLOCATION_ERROR + AGESA_CACHE_SIZE_REDUCED),
                     i, AmdExeAddrMapPtr[i].ExeCacheStartAddr, AmdExeAddrMapPtr[i].ExeCacheSize, 0, StdHeader);
      } else {
        RemainingExecutionCacheSize = RemainingExecutionCacheSize - AmdExeAddrMapPtr[i].ExeCacheSize;
      }
    }
    IDS_HDT_CONSOLE (CPU_TRACE, "  Exe cache allocated: Base 0x%x, Size 0x%x\n", AmdExeAddrMapPtr[i].ExeCacheStartAddr, AmdExeAddrMapPtr[i].ExeCacheSize);

    RequestStartAddr = AmdExeAddrMapPtr[i].ExeCacheStartAddr;
    RequestSize = AmdExeAddrMapPtr[i].ExeCacheSize;

    if (RequestStartAddr < 0x100000) {
      // Region starts below 1MB - Fixed MTTR region,
      // turn on modification bit: MtrrFixDramModEn
      LibAmdMsrRead (MSR_SYS_CFG, &MsrData, StdHeader);
      MsrData |= 0x80000;
      LibAmdMsrWrite (MSR_SYS_CFG, &MsrData, StdHeader);


      // Check for 1M boundary crossing
      if ((RequestStartAddr + RequestSize) > 0x100000) {
        // Request spans the 1M boundary, reduce the size & report the change
        RequestSize = 0x100000 - RequestStartAddr;
        AmdExeAddrMapPtr[i].ExeCacheSize = RequestSize;
        AgesaStatus = AGESA_WARNING;
        PutEventLog (AgesaStatus,
                     (CPU_EVENT_EXECUTION_CACHE_ALLOCATION_ERROR + AGESA_CACHE_REGIONS_ACROSS_1MB),
                     i, RequestStartAddr, RequestSize, 0, StdHeader);
      }

      // Find start MTTR and end MTTR for the requested region
      StartFixMtrr = AMD_MTRR_FIX4K_BASE + ((RequestStartAddr >> 15) & 0x7);
      EndFixMtrr = AMD_MTRR_FIX4K_BASE + ((((RequestStartAddr + RequestSize) - 1) >> 15) & 0x7);

      //
      //Check Mtrr before we use it,
      //  if Mtrr has been used, we need to recover the previously allocated size.
      //    (only work in blocks of 32K size - no splitting of ways)
      for (CurrentMtrr = StartFixMtrr; CurrentMtrr <= EndFixMtrr; CurrentMtrr++) {
        LibAmdMsrRead (CurrentMtrr, &MsrData, StdHeader);
        if ((CacheInfoPtr->CarExeType == LimitedByL2Size) && (MsrData != 0)) {
          // MTRR previously allocated, recover size
          RemainingExecutionCacheSize = RemainingExecutionCacheSize + 0x8000;
        } else {
          // Allocate this MTRR
          MsrData = (UINT64) WP_IO;
          LibAmdMsrWrite (CurrentMtrr, &MsrData, StdHeader);
        }
      }
      // Turn off modification bit: MtrrFixDramModEn
      LibAmdMsrRead (MSR_SYS_CFG, &MsrData, StdHeader);
      MsrData &= 0xFFFFFFFFFFF7FFFFULL;
      LibAmdMsrWrite (MSR_SYS_CFG, &MsrData, StdHeader);


    } else {
      // Region above 1MB -  Variable MTTR region
      //    Need to check both VarMTRRs for each requested region for match or overlap
      //

      // Check for 4G boundary crossing   (using size-1 to keep in 32bit math range)
      if ((0xFFFFFFFFUL - RequestStartAddr) < (RequestSize - 1)) {
        RequestSize = (0xFFFFFFFFUL - RequestStartAddr) + 1;
        AgesaStatus = AGESA_WARNING;
        AmdExeAddrMapPtr[i].ExeCacheSize = RequestSize;
        PutEventLog (AgesaStatus,
                     (CPU_EVENT_EXECUTION_CACHE_ALLOCATION_ERROR + AGESA_CACHE_REGIONS_ACROSS_4GB),
                     i, RequestStartAddr, RequestSize, 0, StdHeader);
      }
      LibAmdMsrRead (AMD_MTRR_VARIABLE_BASE6, &MsrData, StdHeader);
      MtrrV6.ExeCacheStartAddr = ((UINT32) MsrData) & 0xFFFFF000UL;
      LibAmdMsrRead (AMD_MTRR_VARIABLE_BASE6 + 1, &MsrData, StdHeader);
      MtrrV6.ExeCacheSize = (0xFFFFFFFFUL - (((UINT32) MsrData) & 0xFFFFF000UL)) + 1;

      LibAmdMsrRead (AMD_MTRR_VARIABLE_BASE7, &MsrData, StdHeader);
      MtrrV7.ExeCacheStartAddr = ((UINT32) MsrData) & 0xFFFFF000UL;
      LibAmdMsrRead (AMD_MTRR_VARIABLE_BASE7 + 1, &MsrData, StdHeader);
      MtrrV7.ExeCacheSize = (0xFFFFFFFFUL - (((UINT32) MsrData) & 0xFFFFF000UL)) + 1;

      CompareRegions (AmdExeAddrMapPtr[i], MtrrV6, &Result, StdHeader);
      if (Result.OverlapType == EmptySet) {
        // MTRR6 is empty. Allocate request into MTRR6.
        // Note: since all merges are moved down to MTRR6, if MTRR6 is empty so should MTRR7 also be empty
        MtrrV6.ExeCacheStartAddr = AmdExeAddrMapPtr[i].ExeCacheStartAddr;
        MtrrV6.ExeCacheSize = AmdExeAddrMapPtr[i].ExeCacheSize;
      } else if ((Result.OverlapType == Disjoint) ||
                 (Result.OverlapType == NotCombinable)) {
        // MTRR6 is in use, and request does not overlap with MTRR6, check MTRR7
        CompareRegions (AmdExeAddrMapPtr[i], MtrrV7, &Result, StdHeader);
        if (Result.OverlapType == EmptySet) {
          // MTRR7 is empty. Allocate request into MTRR7.
          MtrrV7.ExeCacheStartAddr = AmdExeAddrMapPtr[i].ExeCacheStartAddr;
          MtrrV7.ExeCacheSize = AmdExeAddrMapPtr[i].ExeCacheSize;
        } else if ((Result.OverlapType == Disjoint) ||
                   (Result.OverlapType == NotCombinable)) {
          // MTRR7 is also in use and request does not overlap - error: 3rd region above 1M
          AgesaStatus = AGESA_ERROR;
          PutEventLog (AgesaStatus,
                         (CPU_EVENT_EXECUTION_CACHE_ALLOCATION_ERROR + AGESA_THREE_CACHE_REGIONS_ABOVE_1MB),
                         i, AmdExeAddrMapPtr[i].ExeCacheStartAddr, AmdExeAddrMapPtr[i].ExeCacheSize, 0, StdHeader);
          break;
        } else {
          // Merge request with MTRR7
          MtrrV7.ExeCacheStartAddr = Result.MergedStartAddr;
          MtrrV7.ExeCacheSize = Result.MergedSize;
          if (CacheInfoPtr->CarExeType == LimitedByL2Size) {
            RemainingExecutionCacheSize += Result.OverlapAmount;
          }
        }
      } else {
        // Request overlaps with MTRR6, Merge request with MTRR6
        MtrrV6.ExeCacheStartAddr = Result.MergedStartAddr;
        MtrrV6.ExeCacheSize = Result.MergedSize;
        if (CacheInfoPtr->CarExeType == LimitedByL2Size) {
          RemainingExecutionCacheSize += Result.OverlapAmount;
        }
        CompareRegions (MtrrV6, MtrrV7, &Result, StdHeader);
        if ((Result.OverlapType != Disjoint) &&
            (Result.OverlapType != EmptySet) &&
            (Result.OverlapType != NotCombinable)) {
          // MTRR6 and MTRR7 now overlap, merge them into MTRR6
          MtrrV6.ExeCacheStartAddr = Result.MergedStartAddr;
          MtrrV6.ExeCacheSize = Result.MergedSize;
          MtrrV7.ExeCacheStartAddr = 0;
          MtrrV7.ExeCacheSize = 0;
          if (CacheInfoPtr->CarExeType == LimitedByL2Size) {
            RemainingExecutionCacheSize += Result.OverlapAmount;
          }
        }
      }

      // Set the VarMTRRs.  Base first, then size/mask; this allows for expanding the region safely.
      if (MtrrV6.ExeCacheSize != 0) {
        MsrData = (UINT64) ( 0xFFFFFFFF00000000ULL | ((0xFFFFFFFFUL - (MtrrV6.ExeCacheSize - 1)) | 0x0800UL));
        MsrData &= CacheInfoPtr->VariableMtrrMask;
        SecondMsrData = (UINT64) ( MtrrV6.ExeCacheStartAddr | (UINT64) (WP_IO & 0xFULL));
      } else {
        MsrData = 0;
        SecondMsrData = 0;
      }
      LibAmdMsrWrite (AMD_MTRR_VARIABLE_BASE6, &SecondMsrData, StdHeader);
      LibAmdMsrWrite ((AMD_MTRR_VARIABLE_BASE6 + 1), &MsrData, StdHeader);

      if (MtrrV7.ExeCacheSize != 0) {
        MsrData = (UINT64) ( 0xFFFFFFFF00000000ULL | ((0xFFFFFFFFUL - (MtrrV7.ExeCacheSize - 1)) | 0x0800UL));
        MsrData &= CacheInfoPtr->VariableMtrrMask;
        SecondMsrData = (UINT64) ( MtrrV7.ExeCacheStartAddr | (UINT64) (WP_IO & 0xFULL));
      } else {
        MsrData = 0;
        SecondMsrData = 0;
      }
      LibAmdMsrWrite (AMD_MTRR_VARIABLE_BASE7, &SecondMsrData, StdHeader);
      LibAmdMsrWrite ((AMD_MTRR_VARIABLE_BASE7 + 1), &MsrData, StdHeader);
    } // endif of MTRR region check
  } // end of requests For loop

  return AgesaStatus;
}

/*---------------------------------------------------------------------------------------*/
/**
 * This function calculates available L2 cache space for ROM execution.
 *
 * @param[in]   AmdGetExeSizeParams  Pointer to the start of AmdGetExeSizeParamsPtr structure
 *
 * @retval      AGESA_SUCCESS      No error
 * @retval      AGESA_ALERT        No cache available for execution cache.
 *
 */
AGESA_STATUS
AmdGetAvailableExeCacheSize (
  IN OUT   AMD_GET_EXE_SIZE_PARAMS *AmdGetExeSizeParams
  )
{
  UINT8     WayUsedForCar;
  UINT8     L2Assoc;
  UINT32    L2Size;
  UINT32    L2WaySize;
  UINT32    CurrentCoreNum;
  UINT8     L2Ways;
  UINT8     Ignored;
  UINT32    DieNumber;
  UINT32    TotalCores;
  CPUID_DATA  CpuIdDataStruct;
  CACHE_INFO  *CacheInfoPtr;
  AP_MAIL_INFO ApMailboxInfo;
  AGESA_STATUS IgnoredStatus;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;

  GetCpuServicesOfCurrentCore ((const CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, &AmdGetExeSizeParams->StdHeader);
  FamilySpecificServices->GetCacheInfo (FamilySpecificServices, (const VOID **)&CacheInfoPtr, &Ignored, &AmdGetExeSizeParams->StdHeader);
  // CAR_EXE mode is either "Limited by L2 size" or "Infinite Execution space"
  ASSERT (CacheInfoPtr->CarExeType < MaxCarExeMode);
  if (CacheInfoPtr->CarExeType == InfiniteExe) {
    AmdGetExeSizeParams->AvailableExeCacheSize = SIZE_INFINITE_EXE_CACHE;
    return AGESA_SUCCESS;
  }

  // EXE cache size is limited by size of the L2, minus previous allocations for stack, heap, etc.
  // Check for L2 cache size and way size
  LibAmdCpuidRead (AMD_CPUID_L2L3Cache_L2TLB, &CpuIdDataStruct, &AmdGetExeSizeParams->StdHeader);
  L2Assoc = (UINT8) ((CpuIdDataStruct.ECX_Reg >> 12) & 0x0F);

  // get L2Ways from L2 Association to Way translation table
  L2Ways = L2AssocToL2WayTranslationTable[L2Assoc];
  ASSERT (L2Ways != 0xFF);

  // get L2Size
  L2Size = 1024 * ((CpuIdDataStruct.ECX_Reg >> 16) & 0xFFFF);

  // get each L2WaySize
  L2WaySize = L2Size / L2Ways;

  // Determine the size for execution cache
  if (IsBsp (&AmdGetExeSizeParams->StdHeader, &IgnoredStatus)) {
    // BSC (Boot Strap Core)
    WayUsedForCar = Ceiling (CacheInfoPtr->BspStackSize, L2WaySize) +
                    Ceiling (CacheInfoPtr->MemTrainingBufferSize, L2WaySize) +
                    Ceiling (AMD_HEAP_SIZE_PER_CORE , L2WaySize) +
                    Ceiling (CacheInfoPtr->SharedMemSize, L2WaySize);
  } else {
    // AP (Application Processor)
    GetCurrentCore (&CurrentCoreNum, &AmdGetExeSizeParams->StdHeader);

    GetApMailbox (&ApMailboxInfo.Info, &AmdGetExeSizeParams->StdHeader);
    DieNumber = (1 << ApMailboxInfo.Fields.ModuleType);
    GetActiveCoresInCurrentSocket (&TotalCores, &AmdGetExeSizeParams->StdHeader);
    ASSERT ((TotalCores % DieNumber) == 0);
    if ((CurrentCoreNum % (TotalCores / DieNumber)) == 0) {
      WayUsedForCar = Ceiling (CacheInfoPtr->Core0StackSize , L2WaySize) +
                      Ceiling (CacheInfoPtr->MemTrainingBufferSize, L2WaySize) +
                      Ceiling (AMD_HEAP_SIZE_PER_CORE , L2WaySize) +
                      Ceiling (CacheInfoPtr->SharedMemSize, L2WaySize);
    } else {
      WayUsedForCar = Ceiling (CacheInfoPtr->Core1StackSize , L2WaySize) +
                      Ceiling (AMD_HEAP_SIZE_PER_CORE , L2WaySize) +
                      Ceiling (CacheInfoPtr->SharedMemSize, L2WaySize);
    }
  }

  ASSERT (WayUsedForCar < L2Ways);

  if (WayUsedForCar < L2Ways) {
    AmdGetExeSizeParams->AvailableExeCacheSize = L2WaySize * (L2Ways - WayUsedForCar);
    return AGESA_SUCCESS;
  } else {
    AmdGetExeSizeParams->AvailableExeCacheSize = 0;
    return AGESA_ALERT;
  }
}


/*---------------------------------------------------------------------------------------*/
/**
 * This function rounds a quotient up if the remainder is not zero.
 *
 * @param[in]   Divisor            The divisor
 * @param[in]   Dividend           The dividend
 *
 * @retval      Value              Rounded quotient
 *
 */
UINT8
STATIC
Ceiling (
  IN UINT32 Divisor,
  IN UINT32 Dividend
  )
{
  if ((Divisor % Dividend) == 0) {
    return (UINT8) (Divisor / Dividend);
  } else {
    return (UINT8) ((Divisor / Dividend) + 1);
  }
}


/*---------------------------------------------------------------------------------------*/
/**
 * This function calculates the amount of cache that has already been allocated on the
 * executing core.
 *
 * @param[in]   StdHeader       Handle to config for library and services
 *
 * @returns     Allocated size in bytes
 *
 */
UINT32
STATIC
CalculateOccupiedExeCache (
  IN AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64                     OccupExeCacheSize;
  UINT64                     MsrData;
  UINT8                      i;

  MsrData = 0;
  OccupExeCacheSize = 0;

  //
  //Calculate Variable MTRR base 6~7
  //
  for (i = 0; i < 2; i++) {
    LibAmdMsrRead ((AMD_MTRR_VARIABLE_BASE6 + (2*i)), &MsrData, StdHeader);
    if (MsrData != 0) {
      LibAmdMsrRead ((AMD_MTRR_VARIABLE_BASE6 + (2*i + 1)), &MsrData, StdHeader);
      OccupExeCacheSize = OccupExeCacheSize + ((~((MsrData & (0xFFFF8000)) - 1))&0xFFFF8000);
    }
  }

  //
  //Calculate Fixed MTRR base D0000~F8000
  //
  for (i = 0; i < 6; i++) {
    LibAmdMsrRead ((AMD_MTRR_FIX4K_BASE + 2 + i), &MsrData, StdHeader);
    if (MsrData!= 0) {
      OccupExeCacheSize = OccupExeCacheSize + 0x8000;
    }
  }

  return (UINT32)OccupExeCacheSize;
}


/*---------------------------------------------------------------------------------------*/
/**
 * This function compares two memory regions for overlap and returns the combined
 *  Base,Size  to describe the new combined region.
 *
 * There are 13 cases for how two regions may overlap:  key: [] region A, ** region B
 * 1- [  ] ***      9-  ***  [  ]   disjoint regions
 * 2- [  ]***       10- ***[  ]     adjacent regions
 * 3- [ ***]        11- **[**]      common ending
 * 4- [  *]**       12- *[**  ]     extending
 * 5- [ ** ]        13- *[*]*       contained
 * 6- [***  ]                       common start, contained
 * 7- [***]                         identity
 * 8- [**]**                        common start, extending
 * 0- one of the regions is empty (has base=0)
 *
 * @param[in]     ARegion       pointer to the base,size pair that describes region A
 * @param[in]     BRegion       pointer to the base,size pair that describes region B
 * @param[in,out] CRegion       pointer to the base,size pair that describes region C This struct also has the
 *                              overlap type and the amount of overlap between the regions.
 * @param[in]     StdHeader     Handle to config for library and services
 *
 * @returns       void, nothing
 */

VOID
STATIC
CompareRegions (
  IN       EXECUTION_CACHE_REGION  ARegion,
  IN       EXECUTION_CACHE_REGION  BRegion,
  IN OUT   MERGED_CACHE_REGION     *CRegion,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  )
{
  // Use Int64 to handle regions ending at or above the 4G boundary.
  UINT64        EndOfA;
  UINT64        EndOfB;


  if ((BRegion.ExeCacheStartAddr == 0) ||
      (ARegion.ExeCacheStartAddr == 0)) {
    CRegion->MergedStartAddr  =
    CRegion->MergedSize       =
    CRegion->OverlapAmount    = 0;
    CRegion->OverlapType = EmptySet;
    return;
  }
  if (BRegion.ExeCacheStartAddr < ARegion.ExeCacheStartAddr) {
    //swap regions A & B. this collapses types 9-13 onto 1-5 and reduces the number of tests
    CRegion->MergedStartAddr  = ARegion.ExeCacheStartAddr;
    CRegion->MergedSize       = ARegion.ExeCacheSize;
    ARegion                   = BRegion;
    BRegion.ExeCacheStartAddr = CRegion->MergedStartAddr;
    BRegion.ExeCacheSize      = CRegion->MergedSize;
  }
  CRegion->MergedStartAddr  =
  CRegion->MergedSize       =
  CRegion->OverlapType      =
  CRegion->OverlapAmount    = 0;

  if (ARegion.ExeCacheStartAddr == BRegion.ExeCacheStartAddr) {
    // Common start, cases 6,7, or 8
    if (ARegion.ExeCacheSize == BRegion.ExeCacheSize) {
      // case 7, identity. Need to recover the overlap size
      CRegion->MergedStartAddr  = ARegion.ExeCacheStartAddr;
      CRegion->MergedSize       = ARegion.ExeCacheSize;
      CRegion->OverlapAmount    = ARegion.ExeCacheSize;
      CRegion->OverlapType      = Identity;
    } else if (ARegion.ExeCacheSize < BRegion.ExeCacheSize) {
      // case 8, common start extending
      CRegion->MergedStartAddr  = ARegion.ExeCacheStartAddr;
      CRegion->MergedSize       = BRegion.ExeCacheSize;
      CRegion->OverlapType      = CommonStartExtending;
      CRegion->OverlapAmount    = ARegion.ExeCacheSize;
    } else {
      // case 6, common start contained
      CRegion->MergedStartAddr  = ARegion.ExeCacheStartAddr;
      CRegion->MergedSize       = ARegion.ExeCacheSize;
      CRegion->OverlapType      = CommonStartContained;
      CRegion->OverlapAmount    = BRegion.ExeCacheSize;
    }
  } else {
    // A_Base is less than B_Base. check for cases 1-5
    EndOfA = ((UINT64) ARegion.ExeCacheStartAddr) + ((UINT64) ARegion.ExeCacheSize);

    if (EndOfA < ((UINT64) BRegion.ExeCacheStartAddr)) {
      // case 1, disjoint
      CRegion->MergedStartAddr  =
      CRegion->MergedSize       =
      CRegion->OverlapAmount    = 0;
      CRegion->OverlapType = Disjoint;

    } else if (EndOfA == ((UINT64) BRegion.ExeCacheStartAddr)) {
      // case 2, adjacent
      CRegion->OverlapType = Adjacent;
      CRegion->MergedStartAddr  = ARegion.ExeCacheStartAddr;
      CRegion->MergedSize       = ARegion.ExeCacheSize + BRegion.ExeCacheSize;
      CRegion->OverlapAmount    = 0;
    } else {
      // EndOfA is > B_Base. check for cases 3,4,5
      EndOfB = ((UINT64) BRegion.ExeCacheStartAddr) + ((UINT64) BRegion.ExeCacheSize);

      if ( EndOfA < EndOfB) {
        // case 4, extending
        CRegion->OverlapType = Extending;
        CRegion->MergedStartAddr  = ARegion.ExeCacheStartAddr;
        CRegion->MergedSize       = (UINT32) (EndOfB - ((UINT64) ARegion.ExeCacheStartAddr));
        CRegion->OverlapAmount    = (UINT32) (EndOfA - ((UINT64) BRegion.ExeCacheStartAddr));
      } else {
        // case 3, same end; or case 5, contained
        CRegion->OverlapType = Contained;
        CRegion->MergedStartAddr  = ARegion.ExeCacheStartAddr;
        CRegion->MergedSize       = ARegion.ExeCacheSize;
        CRegion->OverlapAmount    = BRegion.ExeCacheSize;
      }
    }
  } // endif
  // Once we have combined the regions, they must still obey the MTRR size and boundary rules
  if ( CRegion->OverlapType != Disjoint ) {
    if ((!(IsPowerOfTwo (CRegion->MergedSize))) ||
       ((CRegion->MergedStartAddr % CRegion->MergedSize) != 0) ) {
      CRegion->OverlapType = NotCombinable;
    }
  }

}


/*---------------------------------------------------------------------------------------*/
/**
 * This local function tests the parameter for being an even power of two
 *
 * @param[in]   TestNumber    Number to check
 *
 * @retval      TRUE - TestNumber is a power of two,
 * @retval      FALSE - TestNumber is not a power of two
 *
 */
BOOLEAN
STATIC
IsPowerOfTwo (
  IN      UINT32                  TestNumber
  )
{
  UINT32      PowerTwo;

  ASSERT (TestNumber >= 0x8000UL);
  PowerTwo = 0x8000UL;                    // Start at 32K
  while ( TestNumber > PowerTwo ) {
    PowerTwo = PowerTwo * 2;
  }
  return (((TestNumber % PowerTwo) == 0) ? TRUE: FALSE);
}


