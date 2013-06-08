/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU Reset API, and related functions.
 *
 * Contains code that initialized the CPU after early reset.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
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
#include "amdlib.h"
#include "Ids.h"
#include "cpuRegisters.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "Table.h"
#include "cpuApicUtilities.h"
#include "cpuEarlyInit.h"
#include "Topology.h"
#include "cpuFamilyTranslation.h"
#include "cpuFeatures.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_CPU_CPUEARLYINIT_FILECODE


/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern BUILD_OPT_CFG UserOptions;
/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
STATIC
GetPerformEarlyFlag (
  IN OUT   UINT32   *PerformEarlyFlag,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

VOID
McaInitializationAtEarly (
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
 * Initializer routine that will be invoked by AmdCpuEarly to initialize the input
 * structure for the Cpu Init @ Early routine.
 *
 * @param[in]       StdHeader         Opaque handle to standard config header
 * @param[in]       PlatformConfig    Config handle for platform specific information
 * @param[in,out]   CpuEarlyParamsPtr Service Interface structure to initialize.
 *
 * @retval          AGESA_SUCCESS     Always Succeeds
 */
VOID
AmdCpuEarlyInitializer (
  IN       AMD_CONFIG_PARAMS      *StdHeader,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN OUT   AMD_CPU_EARLY_PARAMS   *CpuEarlyParamsPtr
  )
{
  ASSERT (CpuEarlyParamsPtr != NULL);

  CpuEarlyParamsPtr->MemInitPState = (UINT8) UserOptions.CfgMemInitPstate;
  CpuEarlyParamsPtr->PlatformConfig = *PlatformConfig;
}
/*---------------------------------------------------------------------------------------*/
/**
 * Performs CPU related initialization at the early entry point
 *
 * This function performs a large list of initialization items.  These items
 * include:
 *
 *    -1      local APIC initialization
 *    -2      MSR table initialization
 *    -3      PCI table initialization
 *    -4      HT Phy PCI table initialization
 *    -5      microcode patch loading
 *    -6      namestring determination/programming
 *    -7      AP initialization
 *    -8      power management initialization
 *    -9      core leveling
 *
 * This routine must be run by all cores in the system.  Please note that
 * all APs that enter will never exit.
 *
 * @param[in]  StdHeader         Config handle for library and services
 * @param[in]  PlatformConfig    Config handle for platform specific information
 *
 * @retval     AGESA_SUCCESS
 *
 */
AGESA_STATUS
AmdCpuEarly (
  IN       AMD_CONFIG_PARAMS      *StdHeader,
  IN       PLATFORM_CONFIGURATION *PlatformConfig
  )
{
  UINT8         WaitStatus;
  UINT8         i;
  UINT8         StartCore;
  UINT8         EndCore;
  UINT32        NodeNum;
  UINT32        PrimaryCore;
  UINT32        SocketNum;
  UINT32        ModuleNum;
  UINT32        HighCore;
  UINT32        ApHeapIndex;
  UINT32        CurrentPerformEarlyFlag;
  UINT32        TargetApicId;
  AP_WAIT_FOR_STATUS WaitForStatus;
  AGESA_STATUS  Status;
  AGESA_STATUS  CalledStatus;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;
  AMD_CPU_EARLY_PARAMS CpuEarlyParams;
  S_PERFORM_EARLY_INIT_ON_CORE *EarlyTableOnCore;

  Status = AGESA_SUCCESS;
  CalledStatus = AGESA_SUCCESS;

  AmdCpuEarlyInitializer (StdHeader, PlatformConfig, &CpuEarlyParams);

  IDS_OPTION_HOOK (IDS_CPU_Early_Override, &CpuEarlyParams, StdHeader);

  GetCpuServicesOfCurrentCore ((CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
  EarlyTableOnCore = NULL;
  FamilySpecificServices->GetEarlyInitOnCoreTable (FamilySpecificServices, (CONST S_PERFORM_EARLY_INIT_ON_CORE **)&EarlyTableOnCore, &CpuEarlyParams, StdHeader);
  if (EarlyTableOnCore != NULL) {
    GetPerformEarlyFlag (&CurrentPerformEarlyFlag, StdHeader);
    for (i = 0; EarlyTableOnCore[i].PerformEarlyInitOnCore != NULL; i++) {
      if ((EarlyTableOnCore[i].PerformEarlyInitFlag & CurrentPerformEarlyFlag) != 0) {
        IDS_HDT_CONSOLE (CPU_TRACE, "  Perform core init step %d\n", i);
        EarlyTableOnCore[i].PerformEarlyInitOnCore (FamilySpecificServices, &CpuEarlyParams, StdHeader);
      }
    }
  }

  // B S P    C O D E    T O    I N I T I A L I Z E    A Ps
  // -------------------------------------------------------
  // -------------------------------------------------------
  // IMPORTANT: Here we determine if we are BSP or AP
  if (IsBsp (StdHeader, &CalledStatus)) {

    // Even though the bsc does not need to send itself a heap index, this sequence performs other important initialization.
    // Use '0' as a dummy heap index value.
    GetSocketModuleOfNode (0, &SocketNum, &ModuleNum, StdHeader);
    GetCpuServicesOfSocket (SocketNum, (CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
    FamilySpecificServices->SetApCoreNumber (FamilySpecificServices, SocketNum, ModuleNum, 0, StdHeader);
    FamilySpecificServices->TransferApCoreNumber (FamilySpecificServices, StdHeader);

    // Clear BSP's Status Byte
    ApUtilWriteControlByte (CORE_ACTIVE, StdHeader);

    NodeNum = 0;
    ApHeapIndex = 1;
    while (NodeNum < MAX_NODES &&
          GetSocketModuleOfNode (NodeNum, &SocketNum, &ModuleNum, StdHeader)) {
      GetCpuServicesOfSocket (SocketNum, (CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
      GetGivenModuleCoreRange (SocketNum, ModuleNum, &PrimaryCore, &HighCore, StdHeader);
      if (NodeNum == 0) {
        StartCore = (UINT8) PrimaryCore + 1;
      } else {
        StartCore = (UINT8) PrimaryCore;
      }

      EndCore = (UINT8) HighCore;
      for (i = StartCore; i <= EndCore; i++) {
        FamilySpecificServices->SetApCoreNumber (FamilySpecificServices, SocketNum, ModuleNum, ApHeapIndex, StdHeader);
        IDS_HDT_CONSOLE (CPU_TRACE, "  Launch socket %d core %d\n", SocketNum, i);
        if (FamilySpecificServices->LaunchApCore (FamilySpecificServices, SocketNum, ModuleNum, i, PrimaryCore, StdHeader)) {
          IDS_HDT_CONSOLE (CPU_TRACE, "  Waiting for socket %d core %d\n", SocketNum, i);
          GetLocalApicIdForCore (SocketNum, i, &TargetApicId, StdHeader);
          WaitStatus = CORE_IDLE;
          WaitForStatus.Status = &WaitStatus;
          WaitForStatus.NumberOfElements = 1;
          WaitForStatus.RetryCount = WAIT_INFINITELY;
          WaitForStatus.WaitForStatusFlags = WAIT_STATUS_EQUALITY;
          ApUtilWaitForCoreStatus (TargetApicId, &WaitForStatus, StdHeader);
          ApHeapIndex++;
        }
      }
      NodeNum++;
    }

    // B S P    P h a s e - 1   E N D

    IDS_OPTION_HOOK (IDS_BEFORE_PM_INIT, &CpuEarlyParams, StdHeader);

    AGESA_TESTPOINT (TpProcCpuBeforePMFeatureInit, StdHeader);
    IDS_HDT_CONSOLE (CPU_TRACE, "  Dispatch CPU features before early power mgmt init\n");
    CalledStatus = DispatchCpuFeatures (CPU_FEAT_BEFORE_PM_INIT, PlatformConfig, StdHeader);
    if (CalledStatus > Status) {
      Status = CalledStatus;
    }

    AGESA_TESTPOINT (TpProcCpuPowerMgmtInit, StdHeader);
    CalledStatus = PmInitializationAtEarly (&CpuEarlyParams, StdHeader);
    if (CalledStatus > Status) {
      Status = CalledStatus;
    }

    AGESA_TESTPOINT (TpProcCpuEarlyFeatureInit, StdHeader);
    IDS_HDT_CONSOLE (CPU_TRACE, "  Dispatch CPU features after early power mgmt init\n");
    CalledStatus = DispatchCpuFeatures (CPU_FEAT_AFTER_PM_INIT, PlatformConfig, StdHeader);

    IDS_OPTION_HOOK (IDS_BEFORE_AP_EARLY_HALT, &CpuEarlyParams, StdHeader);

    // Sleep all APs
    IDS_HDT_CONSOLE (CPU_TRACE, "  Halting all APs\n");
    ApUtilWriteControlByte (CORE_IDLE_HLT, StdHeader);
  } else {
    ApEntry (StdHeader, &CpuEarlyParams);
  }

  if (CalledStatus > Status) {
    Status = CalledStatus;
  }

  return (Status);
}

/*---------------------------------------------------------------------------------------
 *                           L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * Initialize Machine Check Architecture registers
 *
 * This function initializes the MCA MSRs.  On cold reset, these registers
 * have an invalid data that must be cleared on all cores.
 *
 * @param[in]  StdHeader         Config handle for library and services
 *
 *---------------------------------------------------------------------------------------
 */
VOID
McaInitialization (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT16      TempVar16_a;
  UINT32      MsrAddress;
  UINT64      MsrData;
  CPUID_DATA  CpuIdDataStruct;

  if (!(IsWarmReset (StdHeader))) {
    // Run CPUID to verify that the processor supports MCE and MCA
    // i.e. edx[7], and edx[14]
    // CPUID_MODEL = 1
    LibAmdCpuidRead (1, &CpuIdDataStruct, StdHeader);
    if ((CpuIdDataStruct.EDX_Reg & 0x4080) != 0) {
      // Check to see if the MCG_CTL_P bit is set
      // MCG = Global Machine Check Exception Reporting Control Register
      LibAmdMsrRead (MSR_MCG_CAP, &MsrData, StdHeader);
      if ((MsrData & MCG_CTL_P) != 0) {
        TempVar16_a = (UINT16) ((MsrData & 0x000000FF) << 2);
        TempVar16_a += MSR_MC0_CTL;

        // Initialize the data
        MsrData = 0;
        for (MsrAddress = MSR_MC0_CTL; MsrAddress < TempVar16_a; MsrAddress++) {
          LibAmdMsrWrite (MsrAddress, &MsrData, StdHeader);
        }
      }
    }
  }
}


/*---------------------------------------------------------------------------------------*/
/**
 * Initialize Machine Check Architecture registers
 *
 * This function acts as a wrapper for calling the McaInitialization
 * routine at AmdInitEarly.
 *
 *  @param[in]   FamilyServices      The current Family Specific Services.
 *  @param[in]   EarlyParams         Service parameters.
 *  @param[in]   StdHeader           Config handle for library and services.
 *
 */
VOID
McaInitializationAtEarly (
  IN       CPU_SPECIFIC_SERVICES  *FamilyServices,
  IN       AMD_CPU_EARLY_PARAMS   *EarlyParams,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  McaInitialization (StdHeader);
}

/*---------------------------------------------------------------------------------------*/
/**
 * Runs the given task on all cores (including self) on the socket of the executing
 * core 0.
 *
 * This function is used to invoke all APs on the socket of the executing core 0 to
 * run a specified AGESA procedure.
 *
 * @param[in]  TaskPtr      Function descriptor
 * @param[in]  StdHeader    Config handle for library and services
 * @param[in]  CpuEarlyParamsPtr Required input parameters for early CPU initialization
 *
 */
VOID
ApUtilRunCodeOnAllLocalCoresAtEarly (
  IN       AP_TASK *TaskPtr,
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN       AMD_CPU_EARLY_PARAMS *CpuEarlyParamsPtr
  )
{
  UINT32 Core;
  UINT32 Socket;
  UINT32 IgnoredModule;
  UINT32 IgnoredCore;
  UINT32 ActiveCores;
  AGESA_STATUS IgnoredSts;

  IdentifyCore (StdHeader, &Socket, &IgnoredModule, &IgnoredCore, &IgnoredSts);
  GetActiveCoresInCurrentSocket (&ActiveCores, StdHeader);

  for (Core = 1; Core < (UINT8) ActiveCores; ++Core) {
    ApUtilRunCodeOnSocketCore ((UINT8)Socket, (UINT8)Core, TaskPtr, StdHeader);
  }
  ApUtilTaskOnExecutingCore (TaskPtr, StdHeader, (VOID *) CpuEarlyParamsPtr);
}

/*---------------------------------------------------------------------------------------*/
/**
 * Get current condition, such as warm/cold reset, to determine if related function
 * need to be performed at early stage
 *
 *  @param[in, out]   PerformEarlyFlag      Perform early flag.
 *  @param[in]        StdHeader             Config handle for library and services.
 *
 */
VOID
STATIC
GetPerformEarlyFlag (
  IN OUT   UINT32   *PerformEarlyFlag,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  *PerformEarlyFlag = 0;
  if (IsWarmReset (StdHeader)) {
    *PerformEarlyFlag |= PERFORM_EARLY_WARM_RESET;
  } else {
    *PerformEarlyFlag |= PERFORM_EARLY_COLD_BOOT;
  }
  return;
}
