/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_16 Models 0x00 - 0x0F Kabini Power Management related initialization table
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x16/KB
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
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
#include "Ids.h"
#include "cpuRegisters.h"
#include "cpuApicUtilities.h"
#include "cpuFamilyTranslation.h"
#include "cpuPowerMgmtSystemTables.h"
#include "F16KbUtilities.h"
#include "F16KbCoreAfterReset.h"
#include "F16KbNbAfterReset.h"
#include "F16KbPowerCheck.h"
#include "IdsF16KbAllService.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_CPU_FAMILY_0X16_KB_F16KBPOWERMGMTSYSTEMTABLES_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
GetF16KbSysPmTable (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
     OUT   CONST VOID **SysPmTblPtr,
     OUT   UINT8 *NumberOfElements,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*  Family 16h Kabini Only Table */
/* ---------------------- */
CONST SYS_PM_TBL_STEP ROMDATA CpuF16KbSysPmTableArray[] =
{
  /// @todo
  IDS_INITIAL_F16_KB_PM_STEP
  // Step 1 - Configure F3x[84:80].  Handled by PCI register table.

  // Step 2 - Disable NB Pstate, if required
  //   Execute both cold & warm
  {
    0,                                                // ExeFlags
    F16KbNbPstateDis                                  // Function Pointer
  },

  // Step 3 - Core Minimum P-state Transition Sequence After Warm Reset
  //   Execute only after warm reset
  {
    PM_EXEFLAGS_WARM_ONLY,                            // ExeFlags
    F16KbPmCoreAfterReset                             // Function Pointer
  },

  // Step 4 - NB P-state COF and VID Synchronization After Warm Reset
  //   Execute only after warm reset
  {
    PM_EXEFLAGS_WARM_ONLY,                            // ExeFlags
    F16KbPmNbAfterReset                               // Function Pointer
  },

  // Step 5 - Power Check
  //   Execute only after warm reset
  {
    0,                                                // ExeFlags
    F16KbPmPwrCheck                                   // Function Pointer
  },

  IDS_F16_KB_PM_CUSTOM_STEP
};

/*---------------------------------------------------------------------------------------*/
/**
 *  Returns the appropriate table of steps to perform to initialize the power management
 *  subsystem.
 *
 *  @CpuServiceMethod{::F_CPU_GET_FAMILY_SPECIFIC_ARRAY}.
 *
 *  @param[in]   FamilySpecificServices   The current Family Specific Services.
 *  @param[out]  SysPmTblPtr              Points to the first entry in the table.
 *  @param[out]  NumberOfElements         Number of valid entries in the table.
 *  @param[in]   StdHeader                Header for library and services.
 *
 */
VOID
GetF16KbSysPmTable (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
     OUT   CONST VOID **SysPmTblPtr,
     OUT   UINT8 *NumberOfElements,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  *NumberOfElements = (sizeof (CpuF16KbSysPmTableArray) / sizeof (SYS_PM_TBL_STEP));
  *SysPmTblPtr = CpuF16KbSysPmTableArray;
}
