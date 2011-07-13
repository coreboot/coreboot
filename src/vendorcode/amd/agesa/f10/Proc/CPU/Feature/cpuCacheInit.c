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
 * @e \$Revision: 44323 $   @e \$Date: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
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
 *
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
#define FILECODE PROC_CPU_FEATURE_CPUCACHEINIT_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

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
CalculateOccupyExeCache (
  IN AMD_CONFIG_PARAMS *StdHeader
  );

/*---------------------------------------------------------------------------------------*/
/**
 * This function will setup ROM execution cache.
 *
 * This function should only be called once.  The execution cache regions are passed
 * in, the max number of execution cache regions are three.  If the start address of
 * all three regions are zero, then no execution cache is allocated.
 *
 *   -1  available cache size is less than requested, the ROM execution cache
 *       region is reduced or eliminated.
 *   -2  at least one execution cache regions across the 1MB line, the ROM execution
 *       cache size is reduced.
 *   -3  at least one execution cache regions across the 4GB line, the ROM execution
 *       cache size is reduced.
 *   -4  the start address of a region is not at the boundary of cache size
 *   -5  execution cache start address less than D0000
 *   -6  more than 2 execution cache regions are above 1MB
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
 * @todo on Tilapia, this routine returns AGESA_ERROR.
 */
AGESA_STATUS
AllocateExecutionCache (
  IN AMD_CONFIG_PARAMS *StdHeader,
  IN EXECUTION_CACHE_REGION *AmdExeAddrMapPtr
  )
{
  AGESA_STATUS               AgesaStatus;
  AMD_GET_EXE_SIZE_PARAMS    AmdGetExeSize;
  UINT64                     MsrData;
  UINT32                     RemainingExecutionCacheSize;
  UINT32                     VariableMttrBase;
  UINT32                     AgesaInfo;
  UINT32                     StartAddr;
  UINT32                     ExeCacheSize;
  UINT32                     StartFixMtrr;
  UINT32                     CurrentMtrr;
  UINT32                     EndFixMtrr;
  UINT32                     CurrentAllocatedExeCacheSize;
  UINT8                      i;
  UINT8                      Ignored;
  UINT64                     OccupyExeCacheStartAddr;
  UINT64                     OccupExeCacheSize;
  CACHE_INFO                 *CacheInfoPtr;
  CPU_SPECIFIC_SERVICES      *FamilySpecificServices;

  //
  // if start address of all three regions are zero, then the default values are used
  //
  if (AmdExeAddrMapPtr[0].ExeCacheStartAddr == 0) {
    if (AmdExeAddrMapPtr[1].ExeCacheStartAddr == 0) {
      if (AmdExeAddrMapPtr[2].ExeCacheStartAddr == 0) {
        // No regions defined by the caller
        return AGESA_SUCCESS;
      }
    }
  }

  // turn on modification bit
  LibAmdMsrRead (MSR_SYS_CFG, &MsrData, StdHeader);
  MsrData |= 0x80000;
  LibAmdMsrWrite (MSR_SYS_CFG, &MsrData, StdHeader);

  // get available L2 cache size for ROM execution
  AmdGetExeSize.StdHeader = *StdHeader;
  AgesaStatus = AmdGetAvailableExeCacheSize (&AmdGetExeSize);
  RemainingExecutionCacheSize = AmdGetExeSize.AvailableExeCacheSize;
  CurrentAllocatedExeCacheSize = CalculateOccupyExeCache (StdHeader);
  if (CurrentAllocatedExeCacheSize >= RemainingExecutionCacheSize) {
    RemainingExecutionCacheSize = 0;
  } else {
    RemainingExecutionCacheSize = RemainingExecutionCacheSize - CurrentAllocatedExeCacheSize;
  }

  GetCpuServicesOfCurrentCore (&FamilySpecificServices, StdHeader);
  FamilySpecificServices->GetCacheInfo (FamilySpecificServices, (CONST VOID **)&CacheInfoPtr, &Ignored, StdHeader);

  // Setup MTTRs for region 0 to region 2
  VariableMttrBase = AMD_MTRR_VARIABLE_BASE6;
  for (i = 0; i < 3; i++) {
    // Exit if no more cache available
    if (RemainingExecutionCacheSize == 0) {
      break;
    }

    // Skip the region if ExeCacheSize = 0
    if (AmdExeAddrMapPtr[i].ExeCacheSize == 0) {
      continue;
    }

    // Calculate available execution cache size for region 0 to 2
    if (RemainingExecutionCacheSize >= AmdExeAddrMapPtr[i].ExeCacheSize) {
      RemainingExecutionCacheSize = RemainingExecutionCacheSize - AmdExeAddrMapPtr[i].ExeCacheSize;
    } else {
      AgesaStatus = AGESA_WARNING;
      AgesaInfo = AGESA_CACHE_SIZE_REDUCED;
      AmdExeAddrMapPtr[i].ExeCacheSize = RemainingExecutionCacheSize;
      RemainingExecutionCacheSize = 0;
      PutEventLog (AgesaStatus,
                   (CPU_EVENT_EXECUTION_CACHE_ALLOCATION_ERROR | (UINT8) AgesaInfo),
                   i, AmdExeAddrMapPtr[i].ExeCacheStartAddr, AmdExeAddrMapPtr[i].ExeCacheSize, 0, StdHeader);
    }

    // Align starting addresses on 32K boundary
    AmdExeAddrMapPtr[i].ExeCacheStartAddr =
      AmdExeAddrMapPtr[i].ExeCacheStartAddr & 0xFFFF8000;

    // Boundary alignment check mechanism
    if ((AmdExeAddrMapPtr[i].ExeCacheStartAddr % AmdExeAddrMapPtr[i].ExeCacheSize) != 0) {
      AgesaStatus = AGESA_ERROR;
      AgesaInfo = AGESA_REGION_NOT_ALIGNED_ON_BOUNDARY;
      PutEventLog (AgesaStatus,
                   (CPU_EVENT_EXECUTION_CACHE_ALLOCATION_ERROR | (UINT8) AgesaInfo),
                   i, AmdExeAddrMapPtr[i].ExeCacheStartAddr, AmdExeAddrMapPtr[i].ExeCacheSize, 0, StdHeader);
      break;
    }

    // Check start address
    if (AmdExeAddrMapPtr[i].ExeCacheStartAddr < 0xD0000) {
      AgesaStatus = AGESA_ERROR;
      AgesaInfo = AGESA_CACHE_START_ADDRESS_LESS_D0000;
      PutEventLog (AgesaStatus,
                   (CPU_EVENT_EXECUTION_CACHE_ALLOCATION_ERROR | (UINT8) AgesaInfo),
                   i, AmdExeAddrMapPtr[i].ExeCacheStartAddr, AmdExeAddrMapPtr[i].ExeCacheSize, 0, StdHeader);
      break;
    }

    StartAddr = AmdExeAddrMapPtr[i].ExeCacheStartAddr;
    ExeCacheSize = AmdExeAddrMapPtr[i].ExeCacheSize;

    ExeCacheSize --;

    if (StartAddr < 0x100000) {
      // Region below 1MB
      // Fixed MTTR region
      if ((StartAddr + ExeCacheSize) > 0xFFFFF) {
        ExeCacheSize = 0xFFFFF - StartAddr;
        AgesaStatus = AGESA_WARNING;
        AgesaInfo = AGESA_CACHE_REGIONS_ACROSS_1MB;
        AmdExeAddrMapPtr[i].ExeCacheSize = ExeCacheSize + 1;
        PutEventLog (AgesaStatus,
                     (CPU_EVENT_EXECUTION_CACHE_ALLOCATION_ERROR | (UINT8) AgesaInfo),
                     i, StartAddr, ExeCacheSize, 0, StdHeader);
      }

      // Find start and end of MTTR
      StartFixMtrr = AMD_MTRR_FIX4K_BASE + ((StartAddr >> 15) & 0x7);
      EndFixMtrr = AMD_MTRR_FIX4K_BASE + (((StartAddr + ExeCacheSize) >> 15) & 0x7);

      //
      //Check Mtrr before we use it, if Mtrr has been used, we need to add RemainingExecutionCacheSize back.
      //
      for (CurrentMtrr = StartFixMtrr; CurrentMtrr <= EndFixMtrr; CurrentMtrr++) {
        LibAmdMsrRead (CurrentMtrr, &MsrData, StdHeader);
        if (MsrData != 0) {
          RemainingExecutionCacheSize = RemainingExecutionCacheSize + 0x8000;
        }
      }

      // Setup MTTRs
      MsrData = WP_IO;
      for (CurrentMtrr = StartFixMtrr; CurrentMtrr <= EndFixMtrr; CurrentMtrr++) {
        LibAmdMsrWrite (CurrentMtrr, &MsrData, StdHeader);
      }
    } else {
      // Region above 1MB
      // Variable MTTR region
      if (VariableMttrBase > AMD_MTRR_VARIABLE_BASE7) {
        AgesaStatus = AGESA_ERROR;
        AgesaInfo = AGESA_THREE_CACHE_REGIONS_ABOVE_1MB;
        PutEventLog (AgesaStatus,
                     (CPU_EVENT_EXECUTION_CACHE_ALLOCATION_ERROR | (UINT8) AgesaInfo),
                     i, StartAddr, ExeCacheSize, 0, StdHeader);
        continue;
      }
      if ((0xFFFFFFFF - StartAddr) < ExeCacheSize) {
        ExeCacheSize = 0xFFFFFFFF - StartAddr;
        AgesaStatus = AGESA_WARNING;
        AgesaInfo = AGESA_CACHE_REGIONS_ACROSS_4GB;
        AmdExeAddrMapPtr[i].ExeCacheSize = ExeCacheSize + 1;
        PutEventLog (AgesaStatus,
                     (CPU_EVENT_EXECUTION_CACHE_ALLOCATION_ERROR | (UINT8) AgesaInfo),
                     i, StartAddr, ExeCacheSize, 0, StdHeader);
      }

      //
      //Check Mtrr before we use it.
      //
      LibAmdMsrRead (VariableMttrBase, &MsrData, StdHeader);
      if (MsrData != 0) {

        //
        //Check expanded
        //
        OccupyExeCacheStartAddr = MsrData & (0xFFFF8000);
        LibAmdMsrRead ((VariableMttrBase + 1), &MsrData, StdHeader);
        OccupExeCacheSize = (~((MsrData & (0xFFFF8000)) - 1))&0xFFFF8000;

        //
        //Region below || Region above
        //
        if ( ((StartAddr + ExeCacheSize + 1) <= OccupyExeCacheStartAddr) ||
             (StartAddr >= (OccupyExeCacheStartAddr + OccupExeCacheSize)) ) {
          //
          //Not overlap the original one, but it need to re-process and set an pair of empty Mtrr
          //
          VariableMttrBase += 2;
          RemainingExecutionCacheSize = RemainingExecutionCacheSize + AmdExeAddrMapPtr[i].ExeCacheSize;
          //
          //Resume loop count
          //
          i--;
          continue;
        } else if (OccupyExeCacheStartAddr == StartAddr) {

          //
          //Overlap with same start address
          //
          if (OccupExeCacheSize > (ExeCacheSize + 1)) {
            //AgesaInfo = AGESA_DEALLOCATE_CACHE_REGIONS;
            break;
          }
          RemainingExecutionCacheSize = RemainingExecutionCacheSize + (UINT32)OccupExeCacheSize;
        } else {
          //
          //Overlap with different start address
          //
          //AgesaInfo = AGESA_DEALLOCATE_CACHE_REGIONS;
          break;
        }
      }

      MsrData = (UINT64) (StartAddr | (WP_IO & 0xFull));

      LibAmdMsrWrite (VariableMttrBase, &MsrData, StdHeader);
      MsrData = (UINT64) ( 0xFFFFFFFF00000000ull | ((0xFFFFFFFFull - ExeCacheSize) | 0x800ull));
      MsrData &= CacheInfoPtr->VariableMtrrMask;
      LibAmdMsrWrite ((VariableMttrBase + 1), &MsrData, StdHeader);
      VariableMttrBase += 2;
    }
  }

  // Turn on MTTR enable bit and turn off modification bit
  LibAmdMsrRead (MSR_SYS_CFG, &MsrData, StdHeader);
  MsrData &= 0xFFFFFFFFFFF7FFFFull;
  LibAmdMsrWrite (MSR_SYS_CFG, &MsrData, StdHeader);

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

  GetCpuServicesOfCurrentCore (&FamilySpecificServices, &AmdGetExeSizeParams->StdHeader);

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

  FamilySpecificServices->GetCacheInfo (FamilySpecificServices, (CONST VOID **)&CacheInfoPtr, &Ignored, &AmdGetExeSizeParams->StdHeader);

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
 * @retval      Value              Allocated size in bytes
 *
 */
UINT32
STATIC
CalculateOccupyExeCache (
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
