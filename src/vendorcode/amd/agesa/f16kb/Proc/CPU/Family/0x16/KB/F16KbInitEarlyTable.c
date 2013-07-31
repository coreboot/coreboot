/* $NoKeywords:$ */
/**
 * @file
 *
 * Initialize the Family 16h Kabini specific way of running early initialization.
 *
 * Returns the table of initialization steps to perform at
 * AmdInitEarly.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/FAMILY/0x16/KB
 * @e \$Revision: 86705 $   @e \$Date: 2013-01-24 17:34:21 -0600 (Thu, 24 Jan 2013) $
 *
 */
/*
 ******************************************************************************
 *
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
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "cpuFamilyTranslation.h"
#include "Filecode.h"
#include "cpuEarlyInit.h"
#include "cpuFeatures.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)
#define FILECODE PROC_CPU_FAMILY_0X16_KB_F16KBINITEARLYTABLE_FILECODE


/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern CONST S_PERFORM_EARLY_INIT_ON_CORE ROMDATA F16KbEarlyInitBeforeApLaunchOnCoreTable[];
extern CONST S_PERFORM_EARLY_INIT_ON_CORE ROMDATA F16KbEarlyInitAfterApLaunchOnCoreTable[];
/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

VOID
GetF16KbEarlyInitBeforeApLaunchOnCoreTable (
  IN       CPU_SPECIFIC_SERVICES                *FamilyServices,
     OUT   CONST S_PERFORM_EARLY_INIT_ON_CORE   **Table,
  IN       AMD_CPU_EARLY_PARAMS                 *EarlyParams,
  IN       AMD_CONFIG_PARAMS                    *StdHeader
  );

VOID
GetF16KbEarlyInitAfterApLaunchOnCoreTable (
  IN       CPU_SPECIFIC_SERVICES                *FamilyServices,
     OUT   CONST S_PERFORM_EARLY_INIT_ON_CORE   **Table,
  IN       AMD_CPU_EARLY_PARAMS                 *EarlyParams,
  IN       AMD_CONFIG_PARAMS                    *StdHeader
  );

VOID
F16KbLoadMicrocodePatchAtEarly (
  IN       CPU_SPECIFIC_SERVICES  *FamilyServices,
  IN       AMD_CPU_EARLY_PARAMS   *EarlyParams,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );
/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*------------------------------------------------------------------------------------*/
/**
 * Initializer routine that may be invoked at AmdCpuEarly (Before AP launch) to return the steps that a
 * processor that uses the standard initialization steps should take.
 *
 *  @CpuServiceMethod{::F_GET_EARLY_INIT_TABLE}.
 *
 * @param[in]       FamilyServices    The current Family Specific Services.
 * @param[out]      Table             Table of appropriate init steps for the executing core.
 * @param[in]       EarlyParams       Service Interface structure to initialize.
 * @param[in]       StdHeader         Opaque handle to standard config header.
 *
 */
VOID
GetF16KbEarlyInitBeforeApLaunchOnCoreTable (
  IN       CPU_SPECIFIC_SERVICES                *FamilyServices,
     OUT   CONST S_PERFORM_EARLY_INIT_ON_CORE   **Table,
  IN       AMD_CPU_EARLY_PARAMS                 *EarlyParams,
  IN       AMD_CONFIG_PARAMS                    *StdHeader
  )
{
  *Table = F16KbEarlyInitBeforeApLaunchOnCoreTable;
}

/*------------------------------------------------------------------------------------*/
/**
 * Initializer routine that may be invoked at AmdCpuEarly (After AP launch) to return the steps that a
 * processor that uses the standard initialization steps should take.
 *
 *  @CpuServiceMethod{::F_GET_EARLY_INIT_TABLE}.
 *
 * @param[in]       FamilyServices    The current Family Specific Services.
 * @param[out]      Table             Table of appropriate init steps for the executing core.
 * @param[in]       EarlyParams       Service Interface structure to initialize.
 * @param[in]       StdHeader         Opaque handle to standard config header.
 *
 */
VOID
GetF16KbEarlyInitAfterApLaunchOnCoreTable (
  IN       CPU_SPECIFIC_SERVICES                *FamilyServices,
     OUT   CONST S_PERFORM_EARLY_INIT_ON_CORE   **Table,
  IN       AMD_CPU_EARLY_PARAMS                 *EarlyParams,
  IN       AMD_CONFIG_PARAMS                    *StdHeader
  )
{
  *Table = F16KbEarlyInitAfterApLaunchOnCoreTable;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Update microcode patch in current processor for Family16h KB.
 *
 * This function acts as a wrapper for calling the LoadMicrocodePatch
 * routine at AmdInitEarly.
 *
 *  @param[in]   FamilyServices      The current Family Specific Services.
 *  @param[in]   EarlyParams         Service parameters.
 *  @param[in]   StdHeader           Config handle for library and services.
 *
 */
VOID
F16KbLoadMicrocodePatchAtEarly (
  IN       CPU_SPECIFIC_SERVICES  *FamilyServices,
  IN       AMD_CPU_EARLY_PARAMS   *EarlyParams,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  if (!IsFeatureEnabled (C6Cstate, &EarlyParams->PlatformConfig, StdHeader)) {
    AGESA_TESTPOINT (TpProcCpuLoadUcode, StdHeader);
    LoadMicrocodePatch (StdHeader);
  }
}
