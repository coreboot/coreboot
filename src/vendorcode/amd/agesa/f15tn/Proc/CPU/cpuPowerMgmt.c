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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
 ****************************************************************************
 *
 * Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *
 * AMD is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with AMD.  This header does *NOT* give you permission to use the Materials
 * or any rights under AMD's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 *
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by AMD shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 *
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 *
 * AMD does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by AMD, or
 * result from use of the Materials or any related information.
 *
 * You agree that you will not reverse engineer or decompile the Materials.
 *
 * NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, AMD retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgement of AMD's proprietary rights in them.
 *
 * EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
 * direct product thereof will be exported directly or indirectly, into any
 * country prohibited by the United States Export Administration Act and the
 * regulations thereunder, without the required authorization from the U.S.
 * government nor will be used for any purpose prohibited by the same.
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
RDATA_GROUP (G1_PEICC)

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
  UINT8           MyNumberOfSteps;
  UINT32          ExeResetFlags;
  SYS_PM_TBL_STEP *FamilyTablePtr;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;
  BOOLEAN         ThisIsWarmReset;
  BOOLEAN         NoResetLimit;
  BOOLEAN         NotConflictResetLimit;
  BOOLEAN         WarmResetOnly;
  BOOLEAN         ColdResetOnly;

  GetCpuServicesOfCurrentCore ((CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
  FamilySpecificServices->GetSysPmTableStruct (FamilySpecificServices, (CONST VOID **) &FamilyTablePtr, &MyNumberOfSteps, StdHeader);

  if (*(UINT8 *)Step < MyNumberOfSteps) {
    if (FamilyTablePtr[*(UINT8 *)Step].FuncPtr != NULL) {
      ExeResetFlags = FamilyTablePtr[*(UINT8 *)Step].ExeFlags & (PM_EXEFLAGS_COLD_ONLY | PM_EXEFLAGS_WARM_ONLY);
      ThisIsWarmReset = IsWarmReset (StdHeader);
      NoResetLimit = (ExeResetFlags == 0) ? TRUE : FALSE;
      NotConflictResetLimit = (BOOLEAN) (ExeResetFlags != (PM_EXEFLAGS_COLD_ONLY | PM_EXEFLAGS_WARM_ONLY));
      WarmResetOnly = (BOOLEAN) ((ExeResetFlags & PM_EXEFLAGS_WARM_ONLY) == PM_EXEFLAGS_WARM_ONLY);
      ColdResetOnly = (BOOLEAN) ((ExeResetFlags & PM_EXEFLAGS_COLD_ONLY) == PM_EXEFLAGS_COLD_ONLY);

      IDS_HDT_CONSOLE (CPU_TRACE, "  \tIsWarmReset = %d.\n", ThisIsWarmReset);
      IDS_HDT_CONSOLE (CPU_TRACE, "  \tNoResetLimit = %d\n", NoResetLimit);
      IDS_HDT_CONSOLE (CPU_TRACE, "  \tNotConflictResetLimit = %d\n", NotConflictResetLimit);
      IDS_HDT_CONSOLE (CPU_TRACE, "  \tWarmResetOnly = %d\n", WarmResetOnly);
      IDS_HDT_CONSOLE (CPU_TRACE, "  \tColdResetOnly = %d\n", ColdResetOnly);

      ASSERT (NotConflictResetLimit);

      if (NoResetLimit ||
          (NotConflictResetLimit &&
           ((WarmResetOnly && ThisIsWarmReset) || (ColdResetOnly && !ThisIsWarmReset)))) {
        FamilyTablePtr[*(UINT8 *)Step].FuncPtr (FamilySpecificServices, CpuEarlyParamsPtr, StdHeader);
      } else {
        IDS_HDT_CONSOLE (CPU_TRACE, "  \t\tThis PM init step was skipped!\n");
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
