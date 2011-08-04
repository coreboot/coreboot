/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_14 Power Management Initialization Steps
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 45626 $   @e \$Date: 2011-01-19 09:58:02 -0700 (Wed, 19 Jan 2011) $
 *
 */
/*
 *****************************************************************************
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
 * ***************************************************************************
 *
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
#include "cpuF14SoftwareThermal.h"
#include "cpuF14PowerPlane.h"
#include "cpuF14PowerCheck.h"
#include "cpuF14LowPowerInit.h"
#include "Filecode.h"
#define FILECODE PROC_CPU_FAMILY_0X14_CPUF14POWERMGMTSYSTEMTABLES_FILECODE

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
GetF14SysPmTable (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
     OUT   CONST VOID **SysPmTblPtr,
     OUT   UINT8 *NumberOfElements,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*  Family 14h Table */
/* ---------------------- */
CONST SYS_PM_TBL_STEP ROMDATA CpuF14SysPmTableArray[] =
{
  IDS_INITIAL_F14_PM_STEP

  // Step 1 - Power Plane Initialization
  //   Execute both cold & warm
  {
    0,                                                // ExeFlags
    F14PmPwrPlaneInit                           // Function Pointer
  },

  // Step x - Optimizations for lower power
  //   Execute both cold & warm
  {
    0,                                                // ExeFlags
    F14OptimizeForLowPowerInit                // Function Pointer
  },

  // Step 2 - Current Delivery Check
  //   Execute both cold & warm
  {
    0,                                                // ExeFlags
    F14PmPwrCheck                               // Function Pointer
  },

  // Step x - Software Thermal Control Init
  //   Execute both cold & warm
  {
    0,                                                // ExeFlags
    F14PmThermalInit                            // Function Pointer
  }
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
GetF14SysPmTable (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
     OUT   CONST VOID **SysPmTblPtr,
     OUT   UINT8 *NumberOfElements,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  *NumberOfElements = (sizeof (CpuF14SysPmTableArray) / sizeof (SYS_PM_TBL_STEP));
  *SysPmTblPtr = CpuF14SysPmTableArray;
}
