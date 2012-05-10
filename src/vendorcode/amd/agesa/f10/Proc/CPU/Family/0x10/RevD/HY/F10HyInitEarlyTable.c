/**
 * @file
 *
 * Implements the workaround for erratum 419.
 *
 * Returns the table of initialization steps to perform at
 * AmdInitEarly.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/FAMILY/0x10/RevD/HY
 * @e \$Revision$   @e \$Date$
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
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "cpuRegisters.h"
#include "cpuFamilyTranslation.h"
#include "F10PackageType.h"
#include "Filecode.h"
#define FILECODE PROC_CPU_FAMILY_0X10_REVD_HY_F10HYINITEARLYTABLE_FILECODE


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

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
extern F_PERFORM_EARLY_INIT_ON_CORE McaInitializationAtEarly;
extern F_PERFORM_EARLY_INIT_ON_CORE SetRegistersFromTablesAtEarly;
extern F_PERFORM_EARLY_INIT_ON_CORE SetBrandIdRegistersAtEarly;
extern F_PERFORM_EARLY_INIT_ON_CORE LocalApicInitializationAtEarly;
extern F_PERFORM_EARLY_INIT_ON_CORE LoadMicrocodePatchAtEarly;
extern F_GET_EARLY_INIT_TABLE GetCommonEarlyInitOnCoreTable;

CONST PF_PERFORM_EARLY_INIT_ON_CORE ROMDATA F10HyC32D0EarlyInitOnCoreTable[] =
{
  McaInitializationAtEarly,
  SetRegistersFromTablesAtEarly,
  LocalApicInitializationAtEarly,
  LoadMicrocodePatchAtEarly,
  SetBrandIdRegistersAtEarly,
  NULL
};

/*------------------------------------------------------------------------------------*/
/**
 * Initializer routine that may be invoked at AmdCpuEarly to return the steps
 * appropriate for the executing Rev D core.
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
GetF10HyEarlyInitOnCoreTable (
  IN       CPU_SPECIFIC_SERVICES                *FamilyServices,
     OUT   CONST PF_PERFORM_EARLY_INIT_ON_CORE  **Table,
  IN       AMD_CPU_EARLY_PARAMS                 *EarlyParams,
  IN       AMD_CONFIG_PARAMS                    *StdHeader
  );
VOID
GetF10HyEarlyInitOnCoreTable (
  IN       CPU_SPECIFIC_SERVICES                *FamilyServices,
     OUT   CONST PF_PERFORM_EARLY_INIT_ON_CORE  **Table,
  IN       AMD_CPU_EARLY_PARAMS                 *EarlyParams,
  IN       AMD_CONFIG_PARAMS                    *StdHeader
  )
{
  UINT32         ProcessorPackageType;
  CPU_LOGICAL_ID LogicalId;

  GetLogicalIdOfCurrentCore (&LogicalId, StdHeader);
  ProcessorPackageType = LibAmdGetPackageType (StdHeader);

  // Check if this CPU is affected by erratum 419.
  if (((LogicalId.Revision & AMD_F10_HY_SCM_D0) != 0) && ((ProcessorPackageType & (PACKAGE_TYPE_G34 | PACKAGE_TYPE_FR2_FR5_FR6)) == 0)) {
    // Return initialization steps such that the microcode patch is applied before
    // brand string determination is performed.
    *Table = F10HyC32D0EarlyInitOnCoreTable;
  } else {
    // No workaround is necessary.  Return the standard table.
    GetCommonEarlyInitOnCoreTable (FamilyServices, Table, EarlyParams, StdHeader);
  }
}
