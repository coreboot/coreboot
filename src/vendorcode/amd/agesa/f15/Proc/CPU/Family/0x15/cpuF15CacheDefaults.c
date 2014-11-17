/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 ROM Execution Cache Defaults
 *
 * Contains default values for ROM execution cache setup
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x15
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "cpuCacheInit.h"
#include "cpuFamilyTranslation.h"
#include "amdlib.h"
#include "GeneralServices.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_CPU_FAMILY_0X15_CPUF15CACHEDEFAULTS_FILECODE


/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
GetF15CacheInfo (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
     OUT   CONST VOID **CacheInfoPtr,
     OUT   UINT8 *NumberOfElements,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
#define MEM_TRAINING_BUFFER_SIZE  16384
#define VAR_MTRR_MASK             0x0000FFFFFFFFFFFFul
#define VAR_MTRR_MASK_CP          0x0000FFFFFFFEFFFFul

#define HEAP_BASE_MASK_CP         0x0000FFFFFFFEFF00ul
#define HEAP_BASE_MASK            0x0000FFFFFFFFFFFFul

#define SHARED_MEM_SIZE       0

CONST CACHE_INFO ROMDATA CpuF15CacheInfo =
{
  BSP_STACK_SIZE_64K,
  CORE0_STACK_SIZE,
  CORE1_STACK_SIZE,
  MEM_TRAINING_BUFFER_SIZE,
  SHARED_MEM_SIZE,
  VAR_MTRR_MASK,
  VAR_MTRR_MASK,
  HEAP_BASE_MASK,
  InfiniteExe
};

CONST CACHE_INFO ROMDATA CpuF15CacheInfoCP =
{
  BSP_STACK_SIZE_64K,
  CORE0_STACK_SIZE,
  CORE1_STACK_SIZE,
  MEM_TRAINING_BUFFER_SIZE,
  SHARED_MEM_SIZE,
  VAR_MTRR_MASK,
  VAR_MTRR_MASK_CP,
  HEAP_BASE_MASK_CP,
  InfiniteExe
};


/*---------------------------------------------------------------------------------------*/
/**
 *  Returns the family specific properties of the cache, and its usage.
 *
 *  @CpuServiceMethod{::F_CPU_GET_FAMILY_SPECIFIC_ARRAY}.
 *
 *  @param[in]   FamilySpecificServices   The current Family Specific Services.
 *  @param[out]  CacheInfoPtr             Points to the cache info properties on exit.
 *  @param[out]  NumberOfElements         Will be one to indicate one entry.
 *  @param[in]   StdHeader                Header for library and services.
 *
 */
VOID
GetF15CacheInfo (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
     OUT   CONST VOID **CacheInfoPtr,
     OUT   UINT8 *NumberOfElements,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 Enabled;
  UINT32 DualCore;
  UINT32 Node;
  PCI_ADDR  PciAddress;
  CPU_SPECIFIC_SERVICES *FamilyServices;
  AP_MAILBOXES     ApMailboxes;
  CORE_PAIR_MAP *CorePairMap;
  AGESA_STATUS IgnoredStatus;

  if (!IsBsp (StdHeader, &IgnoredStatus)) {
    GetCpuServicesOfCurrentCore ((CONST CPU_SPECIFIC_SERVICES **)&FamilyServices, StdHeader);
    ASSERT (FamilyServices != NULL);

    FamilyServices->GetApMailboxFromHardware (FamilyServices, &ApMailboxes, StdHeader);
    Node = ApMailboxes.ApMailInfo.Fields.Node;

    // Since pre-heap, get compute unit status from hardware, using mailbox info.
    PciAddress.AddressValue = MAKE_SBDFO (0, 0, 24, 0, 0);
    PciAddress.Address.Device = PciAddress.Address.Device + Node;
    PciAddress.Address.Function = FUNC_5;
    PciAddress.Address.Register = COMPUTE_UNIT_STATUS;
    LibAmdPciReadBits (PciAddress, 3, 0, &Enabled, StdHeader);
    LibAmdPciReadBits (PciAddress, 19, 16, &DualCore, StdHeader);

    // Find the core to compute unit mapping for this node.
    CorePairMap = FamilyServices->CorePairMap;
    if ((Enabled != 0) && (CorePairMap != NULL)) {
      while (CorePairMap->Enabled != 0xFF) {
        if ((Enabled == CorePairMap->Enabled) && (DualCore == CorePairMap->DualCore)) {
          break;
        }
        CorePairMap++;
      }
      // The assert is for finding a processor configured in a way the core pair map doesn't support.
      ASSERT (CorePairMap->Enabled != 0xFF);
      switch (CorePairMap->Mapping) {
      case AllCoresMapping:
        // No cores are sharing a compute unit
        *CacheInfoPtr = &CpuF15CacheInfo;
        break;
      case EvenCoresMapping:
        // Cores are paired into compute units
        *CacheInfoPtr = &CpuF15CacheInfoCP;
        break;
      default:
        ASSERT (FALSE);
      }
    }
  } else {
    // the BSC is always just the first slice, we could return either one. Return the non for safest.
    *CacheInfoPtr = &CpuF15CacheInfo;
  }
  *NumberOfElements = 1;
}

