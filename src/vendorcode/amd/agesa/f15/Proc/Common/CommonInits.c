/* $NoKeywords:$ */
/**
 * @file
 *
 * Common initialization routines.
 *
 * Contains common initialization routines across AGESA entries of phases.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Common
 * @e \$Revision: 56279 $   @e \$Date: 2011-07-11 13:11:28 -0600 (Mon, 11 Jul 2011) $
 *
 */
/*****************************************************************************
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
 */


/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "Ids.h"
#include "Filecode.h"
#include "heapManager.h"
#include "CommonInits.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_COMMON_COMMONINITS_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern BUILD_OPT_CFG UserOptions;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*------------------------------------------------------------------------------------*/

/**
 * Common routine to initialize PLATFORM_CONFIGURATION.
 *
 * @param[in,out]   PlatformConfig   Platform profile/build option config structure
 * @param[in,out]   StdHeader        AMD standard header config param
 *
 * @retval      AGESA_SUCCESS     Always Succeeds.
 *
 */
AGESA_STATUS
CommonPlatformConfigInit (
  IN OUT   PLATFORM_CONFIGURATION    *PlatformConfig,
  IN OUT   AMD_CONFIG_PARAMS         *StdHeader
  )
{
  UINTN i;

  PlatformConfig->PlatformProfile = UserOptions.CfgPerformanceProfile;
  PlatformConfig->PlatformDeemphasisList = UserOptions.CfgPlatformDeemphasisList;
  PlatformConfig->CoreLevelingMode = (UINT8) UserOptions.CfgCoreLevelingMode;
  PlatformConfig->C1eMode = UserOptions.CfgPlatformC1eMode;
  PlatformConfig->C1ePlatformData = UserOptions.CfgPlatformC1eOpData;
  PlatformConfig->C1ePlatformData1 = UserOptions.CfgPlatformC1eOpData1;
  PlatformConfig->C1ePlatformData2 = UserOptions.CfgPlatformC1eOpData2;
  PlatformConfig->C1ePlatformData3 = UserOptions.CfgPlatformC1eOpData3;
  PlatformConfig->CStateMode = UserOptions.CfgPlatformCStateMode;
  PlatformConfig->CStatePlatformData = UserOptions.CfgPlatformCStateOpData;
  PlatformConfig->CStateIoBaseAddress = UserOptions.CfgPlatformCStateIoBaseAddress;
  PlatformConfig->CpbMode = UserOptions.CfgPlatformCpbMode;
  PlatformConfig->UserOptionDmi = UserOptions.OptionDmi;
  PlatformConfig->UserOptionPState = UserOptions.OptionAcpiPstates;
  PlatformConfig->UserOptionSrat = UserOptions.OptionSrat;
  PlatformConfig->UserOptionSlit = UserOptions.OptionSlit;
  PlatformConfig->UserOptionWhea = UserOptions.OptionWhea;
  PlatformConfig->LowPowerPstateForProcHot = UserOptions.CfgLowPowerPstateForProcHot;
  PlatformConfig->PowerCeiling = UserOptions.CfgAmdPstateCapValue;
  PlatformConfig->ForcePstateIndependent = UserOptions.CfgAcpiPstateIndependent;
  PlatformConfig->PStatesInHpcMode = UserOptions.OptionPStatesInHpcMode;
  PlatformConfig->NumberOfIoApics = UserOptions.CfgPlatNumIoApics;
  for (i = 0; i < MaxVrmType; i++) {
    PlatformConfig->VrmProperties[i] = UserOptions.CfgPlatVrmCfg[i];
  }
  PlatformConfig->ProcessorScopeInSb = UserOptions.CfgProcessorScopeInSb;
  PlatformConfig->ProcessorScopeName0 = UserOptions.CfgProcessorScopeName0;
  PlatformConfig->ProcessorScopeName1 = UserOptions.CfgProcessorScopeName1;
  PlatformConfig->GnbHdAudio = UserOptions.CfgGnbHdAudio;
  PlatformConfig->AbmSupport = UserOptions.CfgAbmSupport;
  PlatformConfig->DynamicRefreshRate = UserOptions.CfgDynamicRefreshRate;
  PlatformConfig->LcdBackLightControl = UserOptions.CfgLcdBackLightControl;
  if ((StdHeader->HeapStatus == HEAP_LOCAL_CACHE) ||
      (StdHeader->HeapStatus == HEAP_TEMP_MEM) ||
      (StdHeader->HeapStatus == HEAP_SYSTEM_MEM)) {
    IDS_OPTION_HOOK (IDS_PLATFORMCFG_OVERRIDE, PlatformConfig, StdHeader);
  }
  return AGESA_SUCCESS;
}

