/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU Power Management functions.
 *
 * Contains code for doing early power management
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 56322 $   @e \$Date: 2011-07-11 16:51:42 -0600 (Mon, 11 Jul 2011) $
 *
 */
/*
 ****************************************************************************
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
#include "cpuRegisters.h"
#include "cpuFamilyTranslation.h"
#include "OptionMultiSocket.h"
#include "cpuApicUtilities.h"
#include "cpuEarlyInit.h"
#include "cpuPowerMgmtSystemTables.h"
#include "cpuServices.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_CPU_CPUPOWERMGMT_FILECODE
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
STATIC
PerformThisPmStep (
  IN       VOID *Step,
  IN       AMD_CONFIG_PARAMS    *StdHeader,
  IN       AMD_CPU_EARLY_PARAMS *CpuEarlyParamsPtr
  );

VOID
STATIC
GoToMemInitPstateCore0 (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN       AMD_CPU_EARLY_PARAMS *CpuEarlyParamsPtr
  );

VOID
STATIC
GoToMemInitPstateCore (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN       AMD_CPU_EARLY_PARAMS *CpuEarlyParamsPtr
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
extern OPTION_MULTISOCKET_CONFIGURATION OptionMultiSocketConfiguration;

/*---------------------------------------------------------------------------------------*/
/**
 * Perform the "BIOS Requirements for P-State Initialization and Transitions."
 *
 * This is the generic arbiter code to be executed by the BSC.  The system power
 * management init tables will be traversed.  This must be run by the system BSC
 * only.
 *
 * @param[in]  CpuEarlyParams    Required input parameters for early CPU initialization
 * @param[in]  StdHeader         Config handle for library and services
 *
 * @return     Most severe AGESA_STATUS level that any system processor encountered
 *
 */
AGESA_STATUS
PmInitializationAtEarly (
  IN       AMD_CPU_EARLY_PARAMS *CpuEarlyParams,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{
  UINT8              i;
  UINT8              NumberOfSystemWideSteps;
  AP_TASK            TaskPtr;
  AGESA_STATUS       ReturnCode;
  WARM_RESET_REQUEST Request;

  // Determine the number of steps to perform
  OptionMultiSocketConfiguration.GetNumberOfSystemPmSteps (&NumberOfSystemWideSteps, StdHeader);

  // Traverse the PM init table
  TaskPtr.FuncAddress.PfApTaskIC = PerformThisPmStep;
  TaskPtr.DataTransfer.DataSizeInDwords = 1;
  TaskPtr.DataTransfer.DataPtr = &i;
  TaskPtr.DataTransfer.DataTransferFlags = 0;
  TaskPtr.ExeFlags = PASS_EARLY_PARAMS;
  for (i = 0; i < NumberOfSystemWideSteps; ++i) {
    IDS_HDT_CONSOLE (CPU_TRACE, "  Perform PM init step %d\n", i);
    OptionMultiSocketConfiguration.BscRunCodeOnAllSystemCore0s (&TaskPtr, StdHeader, CpuEarlyParams);
  }

  // GoToMemInitPstateCore0 only if there is no pending warm reset.
  GetWarmResetFlag (StdHeader, &Request);
  if (Request.RequestBit == FALSE) {
    TaskPtr.FuncAddress.PfApTaskC = GoToMemInitPstateCore0;
    TaskPtr.DataTransfer.DataSizeInDwords = 0;
    TaskPtr.ExeFlags = PASS_EARLY_PARAMS;
    IDS_HDT_CONSOLE (CPU_TRACE, "  Transition all cores to POST P-state\n");
    OptionMultiSocketConfiguration.BscRunCodeOnAllSystemCore0s (&TaskPtr, StdHeader, CpuEarlyParams);
  }

  // Retrieve/Process any errors
  ReturnCode = OptionMultiSocketConfiguration.BscRetrievePmEarlyInitErrors (StdHeader);

  return (ReturnCode);
}


/*---------------------------------------------------------------------------------------*/
/**
 * Performs the next step in the executing core 0's family specific power
 * management table.
 *
 * This function determines if the input step is valid, and invokes the power
 * management step if appropriate.  This must be run by processor core 0s only.
 *
 * @param[in]  Step              Zero based step number
 * @param[in]  StdHeader         Config handle for library and services
 * @param[in]  CpuEarlyParamsPtr Required input parameters for early CPU initialization
 *
 */
VOID
STATIC
PerformThisPmStep (
  IN       VOID *Step,
  IN       AMD_CONFIG_PARAMS    *StdHeader,
  IN       AMD_CPU_EARLY_PARAMS *CpuEarlyParamsPtr
  )
{
  UINT8  MyNumberOfSteps;
  SYS_PM_TBL_STEP *FamilyTablePtr;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;

  GetCpuServicesOfCurrentCore ((CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
  FamilySpecificServices->GetSysPmTableStruct (FamilySpecificServices, (CONST VOID **) &FamilyTablePtr, &MyNumberOfSteps, StdHeader);

  if (*(UINT8 *)Step < MyNumberOfSteps) {
    if (FamilyTablePtr[*(UINT8 *)Step].FuncPtr != NULL) {
      if (!(BOOLEAN) (FamilyTablePtr[*(UINT8 *)Step].ExeFlags & PM_EXEFLAGS_WARM_ONLY) ||
          IsWarmReset (StdHeader)) {
        FamilyTablePtr[*(UINT8 *)Step].FuncPtr (FamilySpecificServices, CpuEarlyParamsPtr, StdHeader);
      }
    }
  }
}


/*---------------------------------------------------------------------------------------*/
/**
 * Transitions the executing processor to the desired P-state.
 *
 * This function implements the AMD_CPU_EARLY_PARAMS.MemInitPState parameter, and is
 * run by all processor core 0s.
 *
 * @param[in]  StdHeader         Config handle for library and services
 * @param[in]  CpuEarlyParamsPtr Required input parameters for early CPU initialization
 *
 */
VOID
STATIC
GoToMemInitPstateCore0 (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN       AMD_CPU_EARLY_PARAMS *CpuEarlyParamsPtr
  )
{
  AP_TASK      TaskPtr;

  TaskPtr.FuncAddress.PfApTaskC = GoToMemInitPstateCore;
  TaskPtr.DataTransfer.DataSizeInDwords = 0;
  TaskPtr.ExeFlags = WAIT_FOR_CORE | PASS_EARLY_PARAMS;
  ApUtilRunCodeOnAllLocalCoresAtEarly (&TaskPtr, StdHeader, CpuEarlyParamsPtr);
}


/*---------------------------------------------------------------------------------------*/
/**
 * Transitions the executing core to the desired P-state.
 *
 * This function implements the AMD_CPU_EARLY_PARAMS.MemInitPState parameter, and is
 * run by all system cores.
 *
 * @param[in]  StdHeader         Config handle for library and services
 * @param[in]  CpuEarlyParamsPtr Required input parameters for early CPU initialization
 *
 */
VOID
STATIC
GoToMemInitPstateCore (
  IN       AMD_CONFIG_PARAMS    *StdHeader,
  IN       AMD_CPU_EARLY_PARAMS *CpuEarlyParamsPtr
  )
{
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;

  GetCpuServicesOfCurrentCore ((CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
  FamilySpecificServices->TransitionPstate (FamilySpecificServices, CpuEarlyParamsPtr->MemInitPState, (BOOLEAN) FALSE, StdHeader);
}
