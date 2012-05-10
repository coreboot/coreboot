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
 * @e \$Revision: 6626 $   @e \$Date: 2008-07-04 02:01:02 +0800 (Fri, 04 Jul 2008) $
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
#include "amdlib.h"
#include "cpuRegisters.h"
#include "GeneralServices.h"
#include "cpuServices.h"
#include "cpuFamilyTranslation.h"
#include "cpuFeatures.h"
#include "cpuEarlyInit.h"
#include "Filecode.h"
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
 *    -1    CORE_LEVEL_LOWEST            Level to lowest common denominator
 *    -2    CORE_LEVEL_TWO               Level to 2 cores
 *    -3    CORE_LEVEL_POWER_OF_TWO      Level to 1,2,4 or 8
 *    -4    CORE_LEVEL_NONE              Do no leveling
 *
 * @param[in]  EntryPoint        Timepoint designator.
 * @param[in]  PlatformConfig    Contains the leveling mode parameter
 * @param[in]  StdHeader         Config handle for library and services
 *
 * @return       The most severe status of any family specific service.
 *
 */
STATIC AGESA_STATUS
CoreLevelingAtEarly (
  IN       UINT64                 EntryPoint,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
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
  AP_MAIL_INFO ApMailboxInfo;
  CORE_LEVELING_TYPE  CoreLevelMode;
  CPU_CORE_LEVELING_FAMILY_SERVICES  *FamilySpecificServices;

  MaxCoreCountOnNode = 0;
  MinCoreCountOnNode = 0xFFFFFFFF;
  LeveledCores = 0;

  ASSERT (PlatformConfig->CoreLevelingMode < CoreLevelModeMax);

  // Get OEM IO core level mode
  CoreLevelMode = (CORE_LEVELING_TYPE) PlatformConfig->CoreLevelingMode;

  // Get socket count
  NumberOfSockets = GetPlatformNumberOfSockets ();
  GetApMailbox (&ApMailboxInfo.Info, StdHeader);
  NumberOfModules = ApMailboxInfo.Fields.ModuleType + 1;

  // Collect cpu core info
  // Get the highest and lowest core count in all nodes
  for (Socket = 0; Socket < NumberOfSockets; Socket++) {
    if (IsProcessorPresent (Socket, StdHeader)) {
      for (Module = 0; Module < NumberOfModules; Module++) {
        if (GetGivenModuleCoreRange (Socket, Module, &LowCore, &HighCore, StdHeader)) {
          TotalEnabledCoresOnNode = HighCore - LowCore + 1;
          if (TotalEnabledCoresOnNode < MinCoreCountOnNode) {
            MinCoreCountOnNode = TotalEnabledCoresOnNode;
          }
          if (TotalEnabledCoresOnNode > MaxCoreCountOnNode) {
            MaxCoreCountOnNode = TotalEnabledCoresOnNode;
          }
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
    LeveledCores = MinCoreCountOnNode;
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
  case CORE_LEVEL_ONE:
    LeveledCores = 1;
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
    RequestedCores = CoreLevelMode - CORE_LEVEL_THREE + 3;
    LeveledCores = (RequestedCores + NumberOfModules - 1) / NumberOfModules;
    LeveledCores = (LeveledCores <= MinCoreCountOnNode) ? LeveledCores : MinCoreCountOnNode;
    if ((LeveledCores * NumberOfModules) != RequestedCores) {
      PutEventLog (
      AGESA_WARNING,
      CPU_WARNING_ADJUSTED_LEVELING_MODE,
      RequestedCores, (LeveledCores * NumberOfModules), 0, 0, StdHeader
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
          FamilySpecificServices->SetDownCoreRegister (FamilySpecificServices, &Socket, &Module, &LeveledCores, StdHeader);
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

