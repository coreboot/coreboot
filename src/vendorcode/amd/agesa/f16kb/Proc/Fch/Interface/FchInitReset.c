/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH Init during Power-On Reset
 *
 * Prepare FCH environment during power on stage
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 */
/*;********************************************************************************
;
 * Copyright (c) 2008 - 2013, Advanced Micro Devices, Inc.
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
;*********************************************************************************/

#include    "FchPlatform.h"
#include    "FchTaskLauncher.h"
#include    "heapManager.h"
#include    "Ids.h"
#include    "Filecode.h"
#define FILECODE PROC_FCH_INTERFACE_FCHINITRESET_FILECODE

extern FCH_TASK_ENTRY        *FchInitResetTaskTable[];
extern FCH_RESET_INTERFACE   FchResetInterfaceDefault;

FCH_RESET_DATA_BLOCK*
FchInitResetLoadPrivateDefault (
  IN       AMD_RESET_PARAMS      *ResetParams
  );

AGESA_STATUS
FchInitReset (
  IN  AMD_RESET_PARAMS     *ResetParams
  );

AGESA_STATUS
FchResetConstructor (
  IN       AMD_RESET_PARAMS          *ResetParams
  );
/**
 * FchInitReset - Config Fch during power on stage.
 *
 *
 *
 * @param[in] ResetParams
 *
 */
AGESA_STATUS
FchInitReset (
  IN  AMD_RESET_PARAMS     *ResetParams
  )
{
  FCH_RESET_DATA_BLOCK         *FchParams;

  // Load private data block with default
  FchParams = FchInitResetLoadPrivateDefault (ResetParams);

  // Override external data with input parameters
  FchParams->StdHeader = &ResetParams->StdHeader;
  FchParams->FchReset  = ResetParams->FchInterface;
  FchParams->Gpp.GppFunctionEnable = ResetParams->FchInterface.GppEnable;

  // Override internal data with IDS (Optional, internal build only)
  IDS_OPTION_CALLOUT (IDS_CALLOUT_FCH_INIT_RESET, FchParams, &ResetParams->StdHeader);

  AgesaFchOemCallout (FchParams);
  return FchTaskLauncher (&FchInitResetTaskTable[0], FchParams, TpFchInitResetDispatching);
}


/**
 * A constructor for FCH build parameter structure at InitReset stage
 *
 * Sets inputs to valid, basic level, defaults.
 *
 * @param[in] ResetParams
 *
 * @retval AGESA_SUCCESS      Constructors are not allowed to fail
*/
AGESA_STATUS
FchResetConstructor (
  IN       AMD_RESET_PARAMS          *ResetParams
  )
{
  ResetParams->FchInterface = FchResetInterfaceDefault;
  return AGESA_SUCCESS;
}

