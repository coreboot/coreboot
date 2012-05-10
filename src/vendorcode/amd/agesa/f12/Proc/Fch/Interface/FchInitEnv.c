/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH Initialization.
 *
 * Init IOAPIC/IOMMU/Misc NB features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 49837 $   @e \$Date: 2011-03-30 04:31:05 +0800 (Wed, 30 Mar 2011) $
 *
 */
/*;********************************************************************************
;
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
;*********************************************************************************/

#include "FchPlatform.h"
#include "FchTaskLauncher.h"
#include "heapManager.h"
#include "Ids.h"
#include "Filecode.h"
#define FILECODE PROC_FCH_INTERFACE_FCHINITENV_FILECODE

extern FCH_TASK_ENTRY    *FchInitEnvTaskTable[];
extern FCH_INTERFACE     FchInterfaceDefault;

/*----------------------------------------------------------------------------------------*/
/**
 * FchInitEnv - Config Fch before PCI emulation
 *
 *
 *
 * @param[in] EnvParams
 *
 */
AGESA_STATUS
FchInitEnv (
  IN       AMD_ENV_PARAMS            *EnvParams
  )
{
  UINT8               i;
  UINT8               Data;
  FCH_DATA_BLOCK      *FchParams;
  AGESA_STATUS        Status;

  IDS_HDT_CONSOLE (FCH_TRACE, "  FchInitEnv Enter... \n");
  FchParams = FchInitEnvCreatePrivateData (EnvParams);

  // Override internal data with IDS (Optional, internal build only)
  IDS_OPTION_CALLOUT (IDS_CALLOUT_FCH_INIT_ENV, FchParams, FchParams->StdHeader);

  //
  //to_do-Initialize PCI IRQ routing registers for INTA#-INTH#
  //
  for (i = 0; i < 8; i++) {
    Data = i | BIT7;                                  // Select IRQ routing to APIC
    LibAmdIoWrite (AccessWidth8, FCH_IOMAP_REGC00, &Data, &EnvParams->StdHeader);
    Data = i | BIT4;
    LibAmdIoWrite (AccessWidth8, FCH_IOMAP_REGC01, &Data, &EnvParams->StdHeader);
  }

  AgesaFchOemCallout (FchParams);
  Status = FchTaskLauncher (&FchInitEnvTaskTable[0], FchParams);
  IDS_HDT_CONSOLE (FCH_TRACE, "  FchInitEnv Exit... Status = [0x%x]\n", Status);
  return Status;
}


/**
 * A constructor for FCH build parameter structure at InitEnv stage
 *
 * Sets inputs to valid, basic level, defaults.
 *
 * @param[in,out] EnvParams       InitEnv configuration data block
 *
 * @retval AGESA_SUCCESS          Constructors are not allowed to fail
*/
AGESA_STATUS
FchEnvConstructor (
  IN       AMD_ENV_PARAMS            *EnvParams
  )
{
  EnvParams->FchInterface = FchInterfaceDefault;
  return AGESA_SUCCESS;
}


