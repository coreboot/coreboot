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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*****************************************************************************
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
RDATA_GROUP (G1_PEICC)

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
  PlatformConfig->UserOptionCrat = UserOptions.OptionCrat;
  PlatformConfig->UserOptionCdit = UserOptions.OptionCdit;
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

