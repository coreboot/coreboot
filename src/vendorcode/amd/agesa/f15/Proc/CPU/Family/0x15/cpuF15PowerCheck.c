/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 P-State power check
 *
 * Performs the "Processor-Systemboard Power Delivery Compatibility Check" as
 * described in the BKDG.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x15
 * @e \$Revision: 56273 $   @e \$Date: 2011-07-11 12:53:52 -0600 (Mon, 11 Jul 2011) $
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
#include "cpuF15PowerMgmt.h"
#include "cpuRegisters.h"
#include "cpuApicUtilities.h"
#include "cpuFamilyTranslation.h"
#include "cpuF15PowerCheck.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "OptionMultiSocket.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X15_CPUF15POWERCHECK_FILECODE

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
F15PwrCheckAllCoresGoToLegalPstate (
  IN       VOID *ErrorData,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
STATIC
F15PwrCheckPrimaryCoresAdjustPstates (
  IN       VOID *ErrorData,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
STATIC
F15PwrCheckAllCoresGoToCurrentPs (
  IN     VOID *ErrorData,
  IN     AMD_CONFIG_PARAMS *StdHeader
  );

VOID
STATIC
F15PmPwrChkCopyPstate (
  IN     UINT8 Dest,
  IN     UINT8 Src,
  IN     AMD_CONFIG_PARAMS *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
extern OPTION_MULTISOCKET_CONFIGURATION OptionMultiSocketConfiguration;
/*---------------------------------------------------------------------------------------*/
/**
 * Family 15h core 0 entry point for performing the family 15h Processor-
 * Systemboard Power Delivery Check.
 *
 * The steps are as follows:
 *    1. Starting with SW P0, loop through all P-states until a passing state
 *       is found.  A passing state is one in which the current required by
 *       the CPU is less than the maximum amount of current that the system
 *       can provide to the CPU.  If P0 is under the limit, no further action
 *       is necessary.
 *    2. If at least one P-State is under the limit & at least one P-State is
 *       over the limit, the BIOS must:
 *       a. If the processor's current P-State is disabled by the power check,
 *          then the BIOS must request a transition to an enabled P-state
 *          using MSRC001_0062[PstateCmd] and wait for MSRC001_0063[CurPstate]
 *          to reflect the new value.
 *       b. Program D18F4x15C[BoostSrc] to zero.
 *       c. Copy the contents of the enabled P-state MSRs to the highest
 *          performance P-state locations.
 *       d. Request a P-state transition to the P-state MSR containing the
 *          COF/VID values currently applied.
 *       e. Adjust the following P-state parameters affected by the P-state
 *          MSR copy by subtracting the number of P-states that are disabled
 *          by the power check.
 *          1. F3x64[HtcPstateLimit]
 *          2. F3x68[SwPstateLimit]
 *          3. F3xDC[PstateMaxVal]
 *    3. If all P-States are over the limit, the BIOS must:
 *       a. If the processor's current P-State is !=F3xDC[PstateMaxVal], then
 *          write F3xDC[PstateMaxVal] to MSRC001_0062[PstateCmd] and wait for
 *          MSRC001_0063[CurPstate] to reflect the new value.
 *       b. If MSRC001_0061[PstateMaxVal]!=000b, copy the contents of the P-state
 *          MSR pointed to by F3xDC[PstateMaxVal] to the software P0 MSR.
 *          Write 000b to MSRC001_0062[PstateCmd] and wait for MSRC001_0063
 *          [CurPstate] to reflect the new value.
 *       c. Adjust the following P-state parameters to zero:
 *          1. F3x64[HtcPstateLimit]
 *          2. F3x68[SwPstateLimit]
 *          3. F3xDC[PstateMaxVal]
 *       d. Program D18F4x15C[BoostSrc] to zero.
 *
 * @param[in]  FamilySpecificServices  The current Family Specific Services.
 * @param[in]  CpuEarlyParams          Service parameters
 * @param[in]  StdHeader               Config handle for library and services.
 *
 */
VOID
F15PmPwrCheck (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       AMD_CPU_EARLY_PARAMS  *CpuEarlyParams,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINT8       DisPsNum;
  UINT8       PsMaxVal;
  UINT8       Pstate;
  UINT32      ProcIddMax;
  UINT32      LocalPciRegister;
  UINT32      Socket;
  UINT32      Module;
  UINT32      Core;
  UINT32      AndMask;
  UINT32      OrMask;
  UINT32      PstateLimit;
  UINT32            HighCore;
  UINT32            LowCore;
  UINT32            ModuleIndex;
  UINT64      LocalMsrRegister;
  BOOLEAN           AllPstatesDisabled;
  AP_TASK     TaskPtr;
  PCI_ADDR          PciAddress;
  AGESA_STATUS IgnoredSts;
  PWRCHK_ERROR_DATA ErrorData;

  // get the socket number
  IdentifyCore (StdHeader, &Socket, &Module, &Core, &IgnoredSts);

  ASSERT (Core == 0);

  // get the Max P-state value
  for (PsMaxVal = NM_PS_REG - 1; PsMaxVal != 0; --PsMaxVal) {
    LibAmdMsrRead (PS_REG_BASE + PsMaxVal, &LocalMsrRegister, StdHeader);
    if (((F15_PSTATE_MSR *) &LocalMsrRegister)->PsEnable == 1) {
      break;
    }
  }

  ErrorData.HwPstateNumber = (UINT8) (PsMaxVal + 1);
  GetPciAddress (StdHeader, Socket, Module, &PciAddress, &IgnoredSts);
  PciAddress.Address.Function = FUNC_4;
  PciAddress.Address.Register = CPB_CTRL_REG;
  LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader); // F4x15C
  ErrorData.NumberOfBoostStates = (UINT8) ((F15_CPB_CTRL_REGISTER *) &LocalPciRegister)->NumBoostStates;

  // Starting with SW P0, loop through all P-states until a passing state
  // is found.  A passing state is one in which the current required by
  // the CPU is less than the maximum amount of current that the system
  // can provide to the CPU.  If P0 is under the limit, no further action
  // is necessary.
  DisPsNum = 0;
  AllPstatesDisabled = TRUE;

  for (Pstate = ErrorData.NumberOfBoostStates; Pstate < ErrorData.HwPstateNumber; Pstate++) {
    if (FamilySpecificServices->GetProcIddMax (FamilySpecificServices, Pstate, &ProcIddMax, StdHeader)) {
      if (ProcIddMax > CpuEarlyParams->PlatformConfig.VrmProperties[CoreVrm].CurrentLimit) {
        // Add to event log the Pstate that exceeded the current limit
        PutEventLog (AGESA_WARNING,
                     CPU_EVENT_PM_PSTATE_OVERCURRENT,
                     Socket, Pstate, 0, 0, StdHeader);
        DisPsNum++;
      } else {
        AllPstatesDisabled = FALSE;
        break;
      }
    }
  }

  if (DisPsNum != 0) {
    ErrorData.NumberOfSwPstatesDisabled = DisPsNum;

    if (AllPstatesDisabled) {
      // All P-states are over the limit
    PutEventLog (AGESA_FATAL,
                 CPU_EVENT_PM_ALL_PSTATE_OVERCURRENT,
                 Socket, 0, 0, 0, StdHeader);
      ErrorData.NumberOfSwPstatesDisabled--;
  }

    // Launch APs to transition to a valid P-state
    TaskPtr.FuncAddress.PfApTaskI = F15PwrCheckAllCoresGoToLegalPstate;
    TaskPtr.DataTransfer.DataSizeInDwords = SIZE_IN_DWORDS (PWRCHK_ERROR_DATA);
    TaskPtr.DataTransfer.DataPtr = &ErrorData;
    TaskPtr.DataTransfer.DataTransferFlags = 0;
    TaskPtr.ExeFlags = WAIT_FOR_CORE;
    ApUtilRunCodeOnAllLocalCoresAtEarly (&TaskPtr, StdHeader, CpuEarlyParams);

    // If any software P-states are disabled, then program D18F4x15C[BoostSrc] to zero.
      AndMask = 0xFFFFFFFF;
      ((F15_CPB_CTRL_REGISTER *) &AndMask)->BoostSrc = 0;
      OrMask = 0x00000000;
      OptionMultiSocketConfiguration.ModifyCurrSocketPci (&PciAddress, AndMask, OrMask, StdHeader); // F4x15C

    // Modify P-state MSRs on one core per die
    TaskPtr.FuncAddress.PfApTaskI = F15PwrCheckPrimaryCoresAdjustPstates;

    for (ModuleIndex = 0; ModuleIndex < GetPlatformNumberOfModules (); ModuleIndex++) {
        if (ModuleIndex != Module) {
          if (GetGivenModuleCoreRange (Socket, ModuleIndex, &LowCore, &HighCore, StdHeader)) {
            ApUtilRunCodeOnSocketCore ((UINT8)Socket, (UINT8)LowCore, &TaskPtr, StdHeader);
          }
        }
      }
    F15PwrCheckPrimaryCoresAdjustPstates (&ErrorData, StdHeader);

    // Launch APs to transition to the current P-state at its new location
    TaskPtr.FuncAddress.PfApTaskI = F15PwrCheckAllCoresGoToCurrentPs;
    ApUtilRunCodeOnAllLocalCoresAtEarly (&TaskPtr, StdHeader, CpuEarlyParams);

    // Final Step
    //    F3x64[HtPstatelimit] -= disPsNum
    //    F3x68[SwPstateLimit] -= disPsNum
    //    F3xDC[PstateMaxVal] -= disPsNum

    PciAddress.Address.Function = FUNC_3;
    PciAddress.Address.Register = HTC_REG;
    AndMask = 0xFFFFFFFF;
    ((HTC_REGISTER *) &AndMask)->HtcPstateLimit = 0;
    OrMask = 0x00000000;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader); // F3x64
    PstateLimit = ((HTC_REGISTER *) &LocalPciRegister)->HtcPstateLimit;
    if (PstateLimit > ErrorData.NumberOfSwPstatesDisabled) {
      PstateLimit -= ErrorData.NumberOfSwPstatesDisabled;
      ((HTC_REGISTER *) &OrMask)->HtcPstateLimit = PstateLimit;
    }
    OptionMultiSocketConfiguration.ModifyCurrSocketPci (&PciAddress, AndMask, OrMask, StdHeader); // F3x64

    PciAddress.Address.Register = SW_PS_LIMIT_REG;
    AndMask = 0xFFFFFFFF;
    ((SW_PS_LIMIT_REGISTER *) &AndMask)->SwPstateLimit = 0;
    OrMask = 0x00000000;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader); // F3x68
    PstateLimit = ((SW_PS_LIMIT_REGISTER *) &LocalPciRegister)->SwPstateLimit;
    if (PstateLimit > ErrorData.NumberOfSwPstatesDisabled) {
      PstateLimit -= ErrorData.NumberOfSwPstatesDisabled;
      ((SW_PS_LIMIT_REGISTER *) &OrMask)->SwPstateLimit = PstateLimit;
    }
    OptionMultiSocketConfiguration.ModifyCurrSocketPci (&PciAddress, AndMask, OrMask, StdHeader); // F3x68

    PciAddress.Address.Register = CPTC2_REG;
    AndMask = 0xFFFFFFFF;
    ((CLK_PWR_TIMING_CTRL2_REGISTER *) &AndMask)->PstateMaxVal = 0;
    OrMask = 0x00000000;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader); // F3xDC
    PstateLimit = ((CLK_PWR_TIMING_CTRL2_REGISTER *) &LocalPciRegister)->PstateMaxVal;
    if (PstateLimit > ErrorData.NumberOfSwPstatesDisabled) {
      PstateLimit -= ErrorData.NumberOfSwPstatesDisabled;
      ((CLK_PWR_TIMING_CTRL2_REGISTER *) &OrMask)->PstateMaxVal = PstateLimit;
    }
    OptionMultiSocketConfiguration.ModifyCurrSocketPci (&PciAddress, AndMask, OrMask, StdHeader); // F3xDC
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * First phase core-level error handler called if any p-states were determined
 * to be out of range for the mother board.
 *
 * Transitions to a legal P-state if necessary (steps 2a and 3a) on each core.
 *
 * @param[in]  ErrorData            Details about the error condition.
 * @param[in]  StdHeader            Config handle for library and services.
 *
 */
VOID
STATIC
F15PwrCheckAllCoresGoToLegalPstate (
  IN       VOID *ErrorData,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8                 CurrentPs;
  UINT64 LocalMsrRegister;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;

  if (IsCorePairPrimary (FirstCoreIsComputeUnitPrimary, StdHeader)) {
    LibAmdMsrRead (MSR_PSTATE_STS, &LocalMsrRegister, StdHeader);
    CurrentPs = (UINT8) (((PSTATE_STS_MSR *) &LocalMsrRegister)->CurPstate);

    if (CurrentPs < ((PWRCHK_ERROR_DATA *) ErrorData)->NumberOfSwPstatesDisabled) {
      GetCpuServicesOfCurrentCore ((CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
          FamilySpecificServices->TransitionPstate (FamilySpecificServices, ((PWRCHK_ERROR_DATA *) ErrorData)->NumberOfSwPstatesDisabled, (BOOLEAN) TRUE, StdHeader);
        }
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Core-level error handler called if any p-states were determined to be out
 * of range for the mother board.
 *
 * This function implements steps 2c and the first half of 3b on one core per die.
 *
 * @param[in]  ErrorData            Details about the error condition.
 * @param[in]  StdHeader            Config handle for library and services.
 *
 */
VOID
STATIC
F15PwrCheckPrimaryCoresAdjustPstates (
  IN       VOID *ErrorData,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8  i;
  UINT8  HwPsMaxVal;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;

  GetCpuServicesOfCurrentCore ((CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);

  HwPsMaxVal = (((PWRCHK_ERROR_DATA *) ErrorData)->HwPstateNumber - 1);
        for (i = ((PWRCHK_ERROR_DATA *) ErrorData)->NumberOfBoostStates; (i + ((PWRCHK_ERROR_DATA *) ErrorData)->NumberOfSwPstatesDisabled) <= HwPsMaxVal; i++) {
          F15PmPwrChkCopyPstate (i, (i + ((PWRCHK_ERROR_DATA *) ErrorData)->NumberOfSwPstatesDisabled), StdHeader);
        }

    // Disable the appropriate P-states if any, starting from HW Pmin
  for (i = 0; i < ((PWRCHK_ERROR_DATA *) ErrorData)->NumberOfSwPstatesDisabled; i++) {
      FamilySpecificServices->DisablePstate (FamilySpecificServices, (HwPsMaxVal - i), StdHeader);
    }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Second phase core-level error handler called if any p-states were determined
 * to be out of range for the mother board.
 *
 * Transitions to the core's current P-state in its new location (steps 2d and
 * the second half of 3b) on each core.
 *
 * @param[in]  ErrorData            Details about the error condition.
 * @param[in]  StdHeader            Config handle for library and services.
 *
 */
VOID
STATIC
F15PwrCheckAllCoresGoToCurrentPs (
  IN       VOID *ErrorData,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8                 CurrentPs;
  UINT64                LocalMsrRegister;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;

  if (IsCorePairPrimary (FirstCoreIsComputeUnitPrimary, StdHeader)) {
    GetCpuServicesOfCurrentCore ((CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);

    LibAmdMsrRead (MSR_PSTATE_STS, &LocalMsrRegister, StdHeader);
    CurrentPs = (UINT8) (((PSTATE_STS_MSR *) &LocalMsrRegister)->CurPstate) - ((PWRCHK_ERROR_DATA *) ErrorData)->NumberOfSwPstatesDisabled;

    FamilySpecificServices->TransitionPstate (FamilySpecificServices, CurrentPs, (BOOLEAN) TRUE, StdHeader);
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Copies the contents of one P-State MSR to another.
 *
 * @param[in]  Dest              Destination p-state number
 * @param[in]  Src               Source p-state number
 * @param[in]  StdHeader         Config handle for library and services
 *
 */
VOID
STATIC
F15PmPwrChkCopyPstate (
  IN       UINT8 Dest,
  IN       UINT8 Src,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64 LocalMsrRegister;

  LibAmdMsrRead ((UINT32) (PS_REG_BASE + Src), &LocalMsrRegister, StdHeader);
  LibAmdMsrWrite ((UINT32) (PS_REG_BASE + Dest), &LocalMsrRegister, StdHeader);
}

