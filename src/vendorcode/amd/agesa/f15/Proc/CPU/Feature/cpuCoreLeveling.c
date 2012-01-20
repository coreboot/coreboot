/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU Core Leveling Function.
 *
 * Contains code to Level the number of core in a multi-socket system
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 56279 $   @e \$Date: 2011-07-11 13:11:28 -0600 (Mon, 11 Jul 2011) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
 *----------------------------------------------------------------------------
 */


/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "AMD.h"
#include "Topology.h"
#include "cpuRegisters.h"
#include "GeneralServices.h"
#include "cpuServices.h"
#include "cpuFamilyTranslation.h"
#include "cpuFeatures.h"
#include "cpuEarlyInit.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G2_PEI)
#define FILECODE PROC_CPU_FEATURE_CPUCORELEVELING_FILECODE
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

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
extern CPU_FAMILY_SUPPORT_TABLE CoreLevelingFamilyServiceTable;

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
AGESA_STATUS
CoreLevelingAtEarly (
  IN       UINT64                 EntryPoint,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          P U B L I C     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 *  Should core leveling be enabled
 *
 * @param[in]    PlatformConfig     Contains the runtime modifiable feature input data.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @retval       TRUE               core leveling is supported.
 * @retval       FALSE              core leveling cannot be enabled.
 *
 */
BOOLEAN
STATIC
IsCoreLevelingEnabled (
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  CORE_LEVELING_TYPE  CoreLevelMode;

  CoreLevelMode = (CORE_LEVELING_TYPE) PlatformConfig->CoreLevelingMode;
  if (CoreLevelMode != CORE_LEVEL_NONE) {
    return (TRUE);
  } else {
    return (FALSE);
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Performs core leveling for the system.
 *
 * This function implements the AMD_CPU_EARLY_PARAMS.CoreLevelingMode parameter.
 * The possible modes are:
 *    -0    CORE_LEVEL_LOWEST            Level to lowest common denominator
 *    -1    CORE_LEVEL_TWO               Level to 2 cores
 *    -2    CORE_LEVEL_POWER_OF_TWO      Level to 1,2,4 or 8
 *    -3    CORE_LEVEL_NONE              Do no leveling
 *    -4    CORE_LEVEL_COMPUTE_UNIT      Level cores to one core per compute unit
 *
 * @param[in]  EntryPoint        Timepoint designator.
 * @param[in]  PlatformConfig    Contains the leveling mode parameter
 * @param[in]  StdHeader         Config handle for library and services
 *
 * @return       The most severe status of any family specific service.
 *
 */
AGESA_STATUS
CoreLevelingAtEarly (
  IN       UINT64                 EntryPoint,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32    CoreNumPerComputeUnit;
  UINT32    MinNumOfComputeUnit;
  UINT32    EnabledComputeUnit;
  UINT32    Socket;
  UINT32    Module;
  UINT32    NumberOfSockets;
  UINT32    NumberOfModules;
  UINT32    MinCoreCountOnNode;
  UINT32    MaxCoreCountOnNode;
  UINT32    LowCore;
  UINT32    HighCore;
  UINT32    LeveledCores;
  UINT32    RequestedCores;
  UINT32    TotalEnabledCoresOnNode;
  BOOLEAN   RegUpdated;
  AP_MAIL_INFO ApMailboxInfo;
  CORE_LEVELING_TYPE  CoreLevelMode;
  CPU_CORE_LEVELING_FAMILY_SERVICES  *FamilySpecificServices;
  WARM_RESET_REQUEST Request;

  IDS_HDT_CONSOLE (CPU_TRACE, "CoreLevelingAtEarly\n  CoreLevelMode: %d\n", PlatformConfig->CoreLevelingMode);

  MaxCoreCountOnNode = 0;
  MinCoreCountOnNode = 0xFFFFFFFF;
  LeveledCores = 0;
  CoreNumPerComputeUnit = 1;
  MinNumOfComputeUnit = 0xFF;

  ASSERT (PlatformConfig->CoreLevelingMode < CoreLevelModeMax);

  // Get OEM IO core level mode
  CoreLevelMode = (CORE_LEVELING_TYPE) PlatformConfig->CoreLevelingMode;

  // Get socket count
  NumberOfSockets = GetPlatformNumberOfSockets ();
  GetApMailbox (&ApMailboxInfo.Info, StdHeader);
  NumberOfModules = ApMailboxInfo.Fields.ModuleType + 1;

  // Collect cpu core info
  for (Socket = 0; Socket < NumberOfSockets; Socket++) {
    if (IsProcessorPresent (Socket, StdHeader)) {
      for (Module = 0; Module < NumberOfModules; Module++) {
        if (GetGivenModuleCoreRange (Socket, Module, &LowCore, &HighCore, StdHeader)) {
          // Get the highest and lowest core count in all nodes
          TotalEnabledCoresOnNode = HighCore - LowCore + 1;
          if (TotalEnabledCoresOnNode < MinCoreCountOnNode) {
            MinCoreCountOnNode = TotalEnabledCoresOnNode;
          }
          if (TotalEnabledCoresOnNode > MaxCoreCountOnNode) {
            MaxCoreCountOnNode = TotalEnabledCoresOnNode;
          }
          EnabledComputeUnit = TotalEnabledCoresOnNode;
          switch (GetComputeUnitMapping (StdHeader)) {
          case AllCoresMapping:
            // All cores are in their own compute unit.
            break;
          case EvenCoresMapping:
            // Cores are paired in compute units.
            CoreNumPerComputeUnit = 2;
            EnabledComputeUnit = (TotalEnabledCoresOnNode / 2);
            break;
          default:
            ASSERT (FALSE);
          }
          // Get minimum of compute unit.  This will either be the minimum number of cores (AllCoresMapping),
          // or less (EvenCoresMapping).
          if (EnabledComputeUnit < MinNumOfComputeUnit) {
            MinNumOfComputeUnit = EnabledComputeUnit;
          }
          IDS_HDT_CONSOLE (CPU_TRACE, "  Socket %d Module %d MaxCoreCountOnNode %d MinCoreCountOnNode %d TotalEnabledCoresOnNode %d EnabledComputeUnit %d MinNumOfComputeUnit %d\n", \
                                      Socket, Module, MaxCoreCountOnNode, MinCoreCountOnNode, TotalEnabledCoresOnNode, EnabledComputeUnit, MinNumOfComputeUnit);
        }
      }
    }
  }

  // Get LeveledCores
  switch (CoreLevelMode) {
  case CORE_LEVEL_LOWEST:
    if (MinCoreCountOnNode == MaxCoreCountOnNode) {
      return (AGESA_SUCCESS);
    }
    LeveledCores = (MinCoreCountOnNode / CoreNumPerComputeUnit) * CoreNumPerComputeUnit;
    break;
  case CORE_LEVEL_TWO:
    LeveledCores = 2 / NumberOfModules;
    if (LeveledCores != 0) {
      LeveledCores = (LeveledCores <= MinCoreCountOnNode) ? LeveledCores : MinCoreCountOnNode;
    } else {
      return (AGESA_WARNING);
    }
    if ((LeveledCores * NumberOfModules) != 2) {
      PutEventLog (
      AGESA_WARNING,
      CPU_WARNING_ADJUSTED_LEVELING_MODE,
      2, (LeveledCores * NumberOfModules), 0, 0, StdHeader
      );
    }
    break;
  case CORE_LEVEL_POWER_OF_TWO:
    // Level to power of 2 (1, 2, 4, 8...)
    LeveledCores = 1;
    while (MinCoreCountOnNode >= (LeveledCores * 2)) {
      LeveledCores = LeveledCores * 2;
    }
    break;
  case CORE_LEVEL_COMPUTE_UNIT:
    // Level cores to one core per compute unit, with additional reduction to level
    // all processors to match the processor with the minimum number of cores.
    if (CoreNumPerComputeUnit == 1) {
      // If there is one core per compute unit, this is the same as CORE_LEVEL_LOWEST.
      if (MinCoreCountOnNode == MaxCoreCountOnNode) {
        return (AGESA_SUCCESS);
      }
      LeveledCores = MinCoreCountOnNode;
    } else {
      // If there are more than one core per compute unit, level to the number of compute units.
      LeveledCores = MinNumOfComputeUnit;
    }
    break;
  case CORE_LEVEL_ONE:
    LeveledCores = 1;
    if (NumberOfModules > 1) {
      PutEventLog (
      AGESA_WARNING,
      CPU_WARNING_ADJUSTED_LEVELING_MODE,
      1, NumberOfModules, 0, 0, StdHeader
      );
    }
    break;
  case CORE_LEVEL_THREE:
  case CORE_LEVEL_FOUR:
  case CORE_LEVEL_FIVE:
  case CORE_LEVEL_SIX:
  case CORE_LEVEL_SEVEN:
  case CORE_LEVEL_EIGHT:
  case CORE_LEVEL_NINE:
  case CORE_LEVEL_TEN:
  case CORE_LEVEL_ELEVEN:
  case CORE_LEVEL_TWELVE:
  case CORE_LEVEL_THIRTEEN:
  case CORE_LEVEL_FOURTEEN:
  case CORE_LEVEL_FIFTEEN:
    // MCM processors can not have an odd number of cores. For an odd CORE_LEVEL_N, MCM processors will be
    // leveled as though CORE_LEVEL_N+1 was chosen.
    // Processors with compute units disable all cores in an entire compute unit at a time, or on an MCM processor,
    // two compute units at a time. For example, on an SCM processor with two cores per compute unit, the effective
    // explicit levels are CORE_LEVEL_ONE, CORE_LEVEL_TWO, CORE_LEVEL_FOUR, CORE_LEVEL_SIX, and
    // CORE_LEVEL_EIGHT. The same example for an MCM processor with two cores per compute unit has effective
    // explicit levels of CORE_LEVEL_TWO, CORE_LEVEL_FOUR, CORE_LEVEL_EIGHT, and CORE_LEVEL_TWELVE.
    RequestedCores = CoreLevelMode - CORE_LEVEL_THREE + 3;
    LeveledCores = (RequestedCores + NumberOfModules - 1) / NumberOfModules;
    LeveledCores = (LeveledCores / CoreNumPerComputeUnit) * CoreNumPerComputeUnit;
    LeveledCores = (LeveledCores <= MinCoreCountOnNode) ? LeveledCores : MinCoreCountOnNode;
    if (LeveledCores != 1) {
      LeveledCores = (LeveledCores / CoreNumPerComputeUnit) * CoreNumPerComputeUnit;
    }
    if ((LeveledCores * NumberOfModules * CoreNumPerComputeUnit) != RequestedCores) {
      PutEventLog (
      AGESA_WARNING,
      CPU_WARNING_ADJUSTED_LEVELING_MODE,
      RequestedCores, (LeveledCores * NumberOfModules * CoreNumPerComputeUnit), 0, 0, StdHeader
      );
    }
    break;
  default:
    ASSERT (FALSE);
  }

  // Set down core register
  for (Socket = 0; Socket < NumberOfSockets; Socket++) {
    if (IsProcessorPresent (Socket, StdHeader)) {
      GetFeatureServicesOfSocket (&CoreLevelingFamilyServiceTable, Socket, (CONST VOID **)&FamilySpecificServices, StdHeader);
      if (FamilySpecificServices != NULL) {
        for (Module = 0; Module < NumberOfModules; Module++) {
          IDS_HDT_CONSOLE (CPU_TRACE, "  SetDownCoreRegister: Socket %d Module %d LeveledCores %d CoreLevelMode %d\n", Socket, Module, LeveledCores, CoreLevelMode);
          RegUpdated = FamilySpecificServices->SetDownCoreRegister (FamilySpecificServices, &Socket, &Module, &LeveledCores, CoreLevelMode, StdHeader);
          // If the down core register is updated, trigger a warm reset.
          if (RegUpdated) {
            GetWarmResetFlag (StdHeader, &Request);
            Request.RequestBit = TRUE;
            Request.StateBits = Request.PostStage - 1;
            IDS_HDT_CONSOLE (CPU_TRACE, "  Request a warm reset.\n");
            SetWarmResetFlag (StdHeader, &Request);
          }
        }
      }
    }
  }

  return (AGESA_SUCCESS);
}


CONST CPU_FEATURE_DESCRIPTOR ROMDATA CpuFeatureCoreLeveling =
{
  CoreLeveling,
  (CPU_FEAT_AFTER_PM_INIT),
  IsCoreLevelingEnabled,
  CoreLevelingAtEarly
};

/*----------------------------------------------------------------------------------------
 *                          L O C A L      F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

