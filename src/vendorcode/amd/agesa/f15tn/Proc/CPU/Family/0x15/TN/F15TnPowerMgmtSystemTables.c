/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 Models 0x10 - 0x1F Power Management related initialization table
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x15/TN
 * @e \$Revision: 63692 $   @e \$Date: 2012-01-03 22:13:28 -0600 (Tue, 03 Jan 2012) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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
#include "cpuF15TnCoreAfterReset.h"
#include "cpuF15TnNbAfterReset.h"
#include "F15TnPowerPlane.h"
#include "cpuF15TnPowerCheck.h"
#include "F15TnUtilities.h"
#include "IdsF15TnAllService.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_CPU_FAMILY_0X15_TN_F15TNPOWERMGMTSYSTEMTABLES_FILECODE

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
GetF15TnSysPmTable (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
     OUT   CONST VOID **SysPmTblPtr,
     OUT   UINT8 *NumberOfElements,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*  Family 15h Only Table */
/* ---------------------- */
CONST SYS_PM_TBL_STEP ROMDATA CpuF15TnSysPmTableArray[] =
{
  IDS_INITIAL_F15_TN_PM_STEP

  // Step 1 - Configure F3x[84:80].  Handled by PCI register table.
  // Step 2 - Power Plane Initialization
  //   Execute both cold & warm
  {
    0,                                                // ExeFlags
    F15TnPmPwrPlaneInit                               // Function Pointer
  },

  // Step 3 - Adjust NB VID
  //   Execute only after cold reset
  {
    PM_EXEFLAGS_COLD_ONLY,                            // ExeFlags
    F15TnNbPstateVidAdjustAfterReset                  // Function Pointer
  },

  // Step 4 - Disable NB Pstate, if required
  //   Execute both cold & warm
  {
    0,                                                // ExeFlags
    F15TnNbPstateDis                                  // Function Pointer
  },

  // Step 5 - Core Minimum P-state Transition Sequence After Warm Reset
  //   Execute only after warm reset
  {
    PM_EXEFLAGS_WARM_ONLY,                            // ExeFlags
    F15TnPmCoreAfterReset                             // Function Pointer
  },

  // Step 6 - NB P-state COF and VID Synchronization After Warm Reset
  //   Execute only after warm reset
  {
    PM_EXEFLAGS_WARM_ONLY,                            // ExeFlags
    F15TnPmNbAfterReset                               // Function Pointer
  },

  // Step 7 - Power Check
  //   Execute both cold & warm
  {
    0,                                                // ExeFlags
    F15TnPmPwrCheck                                     // Function Pointer
  },

  IDS_F15_TN_PM_CUSTOM_STEP

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
GetF15TnSysPmTable (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
     OUT   CONST VOID **SysPmTblPtr,
     OUT   UINT8 *NumberOfElements,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  *NumberOfElements = (sizeof (CpuF15TnSysPmTableArray) / sizeof (SYS_PM_TBL_STEP));
  *SysPmTblPtr = CpuF15TnSysPmTableArray;
}
