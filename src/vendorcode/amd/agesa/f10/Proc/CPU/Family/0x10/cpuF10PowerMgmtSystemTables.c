/**
 * @file
 *
 * AMD Family_10 Power Management related stuff
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 6410 $   @e \$Date: 2008-06-17 06:08:25 -0500 (Tue, 17 Jun 2008) $
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
#include "Ids.h"
#include "cpuRegisters.h"
#include "cpuApicUtilities.h"
#include "cpuFamilyTranslation.h"
#include "IdsF10AllService.h"
#include "cpuPowerMgmtSystemTables.h"
#include "cpuF10EarlyInit.h"
#include "cpuF10SoftwareThermal.h"
#include "cpuF10PowerPlane.h"
#include "cpuF10PowerCheck.h"
#include "F10PmNbCofVidInit.h"
#include "F10PmNbPstateInit.h"
#include "Filecode.h"
#define FILECODE PROC_CPU_FAMILY_0X10_CPUF10POWERMGMTSYSTEMTABLES_FILECODE

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

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
GetF10SysPmTable (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
     OUT   CONST VOID **SysPmTblPtr,
     OUT   UINT8 *NumberOfElements,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/*  Family 10h Only Table */
/* ---------------------- */
CONST SYS_PM_TBL_STEP ROMDATA CpuF10SysPmTableArray[] =
{
  IDS_INITIAL_F10_PM_STEP

  // Step 1 - Configure F3x[84:80].  Handled by PCI register table.
  // Step 2 - Configure Northbridge COF and VID.
  //   Execute both cold & warm
  {
    0,
    F10PmNbCofVidInit
  },

  // Step 3 - Dual-plane Only Support.  Not required by any Arch2008 supported CPU.
  // Step 4 - Configure Nb-Pstates.
  //   Execute only after warm reset
  {
    PM_EXEFLAGS_WARM_ONLY,                            // ExeFlags
    F10PmNbPstateInit
  },
  // Step 5 - Power Plane Initialization
  //   Execute both cold & warm
  {
    0,                                                // ExeFlags
    F10CpuAmdPmPwrPlaneInit                           // Function Pointer
  },

  // Step 6 - Pmin Transition After Reset
  //   Execute only after warm reset
  {
    PM_EXEFLAGS_WARM_ONLY,                            // ExeFlags
    F10PmAfterReset                             // Function Pointer
  },

  // Step 7 - Current Delivery Check
  //   Execute only after warm reset
  {
    PM_EXEFLAGS_WARM_ONLY,                            // ExeFlags
    F10PmPwrCheck                               // Function Pointer
  },

  // Step x - Software Thermal Control Init
  //   Execute only after warm reset
  {
    PM_EXEFLAGS_WARM_ONLY,                            // ExeFlags
    F10PmThermalInit                            // Function Pointer
  },
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
GetF10SysPmTable (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
     OUT   CONST VOID **SysPmTblPtr,
     OUT   UINT8 *NumberOfElements,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  *NumberOfElements = (sizeof (CpuF10SysPmTableArray) / sizeof (SYS_PM_TBL_STEP));
  *SysPmTblPtr = CpuF10SysPmTableArray;
}
