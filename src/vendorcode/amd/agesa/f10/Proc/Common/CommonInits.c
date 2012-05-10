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
 * @e \$Revision: 7735 $   @e \$Date: 2008-08-27 14:49:19 -0500 (Wed, 27 Aug 2008) $
 *
 */
/*****************************************************************************
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


/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "CommonInits.h"
#include "Filecode.h"
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
  PlatformConfig->PlatformProfile = UserOptions.CfgPerformanceProfile;
  PlatformConfig->PlatformDeemphasisList = UserOptions.CfgPlatformDeemphasisList;
  PlatformConfig->CoreLevelingMode = (UINT8) UserOptions.CfgCoreLevelingMode;
  PlatformConfig->C1eMode = UserOptions.CfgPlatformC1eMode;
  PlatformConfig->C1ePlatformData = UserOptions.CfgPlatformC1eOpData;
  PlatformConfig->CStateMode = UserOptions.CfgPlatformCStateMode;
  PlatformConfig->CStatePlatformData = UserOptions.CfgPlatformCStateOpData;
  PlatformConfig->UserOptionDmi = UserOptions.OptionDmi;
  PlatformConfig->UserOptionPState = UserOptions.OptionAcpiPstates;
  PlatformConfig->UserOptionSrat = UserOptions.OptionSrat;
  PlatformConfig->UserOptionSlit = UserOptions.OptionSlit;
  PlatformConfig->UserOptionWhea = UserOptions.OptionWhea;
  PlatformConfig->PowerCeiling = UserOptions.CfgAmdPstateCapValue;
  PlatformConfig->ForcePstateIndependent = UserOptions.CfgAcpiPstateIndependent;
  PlatformConfig->NumberOfIoApics = UserOptions.CfgPlatNumIoApics;
  PlatformConfig->VrmProperties = UserOptions.CfgPlatVrmCfg;
  PlatformConfig->ProcessorScopeInSb = UserOptions.CfgProcessorScopeInSb;
  PlatformConfig->ProcessorScopeName0 = UserOptions.CfgProcessorScopeName0;
  PlatformConfig->ProcessorScopeName1 = UserOptions.CfgProcessorScopeName1;

  return AGESA_SUCCESS;
}

