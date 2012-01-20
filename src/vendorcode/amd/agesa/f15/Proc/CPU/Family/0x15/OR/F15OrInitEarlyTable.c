/* $NoKeywords:$ */
/**
 * @file
 *
 * Initialize the Family 15h Orochi specific way of running early initialization.
 *
 * Returns the table of initialization steps to perform at
 * AmdInitEarly.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/FAMILY/0x15/OR
 * @e \$Revision: 55600 $   @e \$Date: 2011-06-23 12:39:18 -0600 (Thu, 23 Jun 2011) $
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
#include "OptionFamily15hEarlySample.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)
#define FILECODE PROC_CPU_FAMILY_0X15_OR_F15ORINITEARLYTABLE_FILECODE


/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
F15OrLoadMicrocodePatchAtEarly (
  IN       CPU_SPECIFIC_SERVICES  *FamilyServices,
  IN       AMD_CPU_EARLY_PARAMS   *EarlyParams,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

VOID
GetF15OrEarlyInitOnCoreTable (
  IN       CPU_SPECIFIC_SERVICES                *FamilyServices,
     OUT   CONST S_PERFORM_EARLY_INIT_ON_CORE   **Table,
  IN       AMD_CPU_EARLY_PARAMS                 *EarlyParams,
  IN       AMD_CONFIG_PARAMS                    *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
extern F_PERFORM_EARLY_INIT_ON_CORE McaInitializationAtEarly;
extern F_PERFORM_EARLY_INIT_ON_CORE SetRegistersFromTablesAtEarly;
extern F_PERFORM_EARLY_INIT_ON_CORE F15SetBrandIdRegistersAtEarly;
extern F_PERFORM_EARLY_INIT_ON_CORE LocalApicInitializationAtEarly;
extern F15_OR_ES_MCU_PATCH F15OrEarlySampleLoadMcuPatch;

/*----------------------------------------------------------------------------------------
 *                          D A T A    D E C L A R A T I O N S
 *----------------------------------------------------------------------------------------
 */
CONST S_PERFORM_EARLY_INIT_ON_CORE ROMDATA F15OrEarlyInitOnCoreTable[] =
{
  {McaInitializationAtEarly, PERFORM_EARLY_ANY_CONDITION},
  {SetRegistersFromTablesAtEarly, PERFORM_EARLY_ANY_CONDITION},
  {F15SetBrandIdRegistersAtEarly, PERFORM_EARLY_ANY_CONDITION},
  {LocalApicInitializationAtEarly, PERFORM_EARLY_ANY_CONDITION},
  {F15OrLoadMicrocodePatchAtEarly, PERFORM_EARLY_WARM_RESET},
  {NULL, 0}
};

/*------------------------------------------------------------------------------------*/
/**
 * Initializer routine that may be invoked at AmdCpuEarly to return the steps that a
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
GetF15OrEarlyInitOnCoreTable (
  IN       CPU_SPECIFIC_SERVICES                *FamilyServices,
     OUT   CONST S_PERFORM_EARLY_INIT_ON_CORE   **Table,
  IN       AMD_CPU_EARLY_PARAMS                 *EarlyParams,
  IN       AMD_CONFIG_PARAMS                    *StdHeader
  )
{
  *Table = F15OrEarlyInitOnCoreTable;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Update microcode patch in current processor for Family15h OR.
 *
 * This function acts as a wrapper for calling the LoadMicrocodePatch
 * routine at AmdInitEarly.
 *
 * This particualr version implements a workaround to a potential problem caused
 * when upgrading the microcode on Orochi B1 processors.
 *
 *  @param[in]   FamilyServices      The current Family Specific Services.
 *  @param[in]   EarlyParams         Service parameters.
 *  @param[in]   StdHeader           Config handle for library and services.
 *
 */
VOID
F15OrLoadMicrocodePatchAtEarly (
  IN       CPU_SPECIFIC_SERVICES  *FamilyServices,
  IN       AMD_CPU_EARLY_PARAMS   *EarlyParams,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT64         MsrValue;
  UINT64         BuCfg2MsrValue;
  UINT64         CuCfgMsrValue;
  BOOLEAN        IsPatchLoaded;

  AGESA_TESTPOINT (TpProcCpuLoadUcode, StdHeader);

  if (IsCorePairPrimary (FirstCoreIsComputeUnitPrimary, StdHeader)) {

    F15OrEarlySampleLoadMcuPatch.F15OrESAvoidNbCyclesStart (StdHeader, &BuCfg2MsrValue);

    // Erratum #655
    // Set MSR C001_1023[1] = 1b, prior to writing to MSR C001_1020
    LibAmdMsrRead (MSR_CU_CFG, &CuCfgMsrValue, StdHeader);
    MsrValue = CuCfgMsrValue | BIT1;
    LibAmdMsrWrite (MSR_CU_CFG, &MsrValue, StdHeader);

    IsPatchLoaded =  F15OrEarlySampleLoadMcuPatch.F15OrUpdateMcuPatchHook (StdHeader);

    // Erratum #655
    // Restore MSR C001_1023[1] = previous setting
    LibAmdMsrWrite (MSR_CU_CFG, &CuCfgMsrValue, StdHeader);

    F15OrEarlySampleLoadMcuPatch.F15OrESAvoidNbCyclesEnd (StdHeader, &BuCfg2MsrValue);
    F15OrEarlySampleLoadMcuPatch.F15OrESAfterPatchLoaded (StdHeader, IsPatchLoaded);
  }
}
