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
F15PmPwrCheckCore (
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
 *    1. Starting with P0, loop through all P-states until a passing state is
 *       found.  A passing state is one in which the current required by the
 *       CPU is less than the maximum amount of current that the system can
 *       provide to the CPU.  If P0 is under the limit, no further action is
 *       necessary.
 *    2. If at least one P-State is under the limit & at least one P-State is
 *       over the limit, the BIOS must:
 *       a. If the processor's current P-State is disabled by the power check,
 *          then the BIOS must request a transition to an enabled P-state
 *          using MSRC001_0062[PstateCmd] and wait for MSRC001_0063[CurPstate]
 *          to reflect the new value.
 *       b. Copy the contents of the enabled P-state MSRs to the highest
 *          performance P-state locations.
 *       c. Request a P-state transition to the P-state MSR containing the
 *          COF/VID values currently applied.
 *       d. If a subset of boosted P-states are disabled, then copy the contents
 *          of the highest performance boosted P-state still enabled to the
 *          boosted P-states that have been disabled.
 *       e. If all boosted P-states are disabled, then program D18F4x15C[BoostSrc]
 *          to zero.
 *       f. Adjust the following P-state parameters affected by the P-state
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
  PCI_ADDR    PciAddress;
  UINT64      LocalMsrRegister;
  AP_TASK     TaskPtr;
  AGESA_STATUS IgnoredSts;
  PWRCHK_ERROR_DATA ErrorData;
  UINT32      NumModules;
  UINT32      HighCore;
  UINT32      LowCore;
  UINT32      ModuleIndex;


  // get the socket number
  IdentifyCore (StdHeader, &Socket, &Module, &Core, &IgnoredSts);
  ErrorData.SocketNumber = (UINT8) Socket;

  ASSERT (Core == 0);

  // get the Max P-state value
  for (PsMaxVal = NM_PS_REG - 1; PsMaxVal != 0; --PsMaxVal) {
    LibAmdMsrRead (PS_REG_BASE + PsMaxVal, &LocalMsrRegister, StdHeader);
    if (((F15_PSTATE_MSR *) &LocalMsrRegister)->PsEnable == 1) {
      break;
    }
  }

  ErrorData.HwPstateNumber = (UINT8) (PsMaxVal + 1);

 // Starting with P0, loop through all P-states until a passing state is
 // found.  A passing state is one in which the current required by the
 // CPU is less than the maximum amount of current that the system can
 // provide to the CPU.  If P0 is under the limit, no further action is
 // necessary.
  DisPsNum = 0;
  for (Pstate = 0; Pstate < ErrorData.HwPstateNumber; Pstate++) {
    if (FamilySpecificServices->GetProcIddMax (FamilySpecificServices, Pstate, &ProcIddMax, StdHeader)) {
      if (ProcIddMax > CpuEarlyParams->PlatformConfig.VrmProperties[CoreVrm].CurrentLimit) {
        // Add to event log the Pstate that exceeded the current limit
        PutEventLog (AGESA_WARNING,
                     CPU_EVENT_PM_PSTATE_OVERCURRENT,
                     Socket, Pstate, 0, 0, StdHeader);
        DisPsNum++;
      } else {
        break;
      }
    }
  }

  ErrorData.AllowablePstateNumber = ((PsMaxVal + 1) - DisPsNum);

  if (ErrorData.AllowablePstateNumber == 0) {
    PutEventLog (AGESA_FATAL,
                 CPU_EVENT_PM_ALL_PSTATE_OVERCURRENT,
                 Socket, 0, 0, 0, StdHeader);
  }

  if (DisPsNum != 0) {
    GetPciAddress (StdHeader, Socket, Module, &PciAddress, &IgnoredSts);
    PciAddress.Address.Function = FUNC_4;
    PciAddress.Address.Register = CPB_CTRL_REG;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader); // F4x15C
    ErrorData.NumberOfBoostStates = (UINT8) ((F15_CPB_CTRL_REGISTER *) &LocalPciRegister)->NumBoostStates;

    if (DisPsNum >= ErrorData.NumberOfBoostStates) {
      // If all boosted P-states are disabled, then program D18F4x15C[BoostSrc] to zero.
      AndMask = 0xFFFFFFFF;
      ((F15_CPB_CTRL_REGISTER *) &AndMask)->BoostSrc = 0;
      OrMask = 0x00000000;
      OptionMultiSocketConfiguration.ModifyCurrSocketPci (&PciAddress, AndMask, OrMask, StdHeader); // F4x15C

      ErrorData.NumberOfSwPstatesDisabled = DisPsNum - ErrorData.NumberOfBoostStates;
    } else {
      ErrorData.NumberOfSwPstatesDisabled = 0;
    }

    NumModules = GetPlatformNumberOfModules ();

    // Only execute this loop if this is an MCM.
    if (NumModules > 1) {

      // Since the P-State MSRs are shared across a
      // node, we only need to set one core in the node for the modified number of supported p-states
      // to be reported across all of the cores in the module.
      TaskPtr.FuncAddress.PfApTaskI = F15PmPwrCheckCore;
      TaskPtr.DataTransfer.DataSizeInDwords = SIZE_IN_DWORDS (PWRCHK_ERROR_DATA);
      TaskPtr.DataTransfer.DataPtr = &ErrorData;
      TaskPtr.DataTransfer.DataTransferFlags = 0;
      TaskPtr.ExeFlags = WAIT_FOR_CORE;

      for (ModuleIndex = 0; ModuleIndex < NumModules; ModuleIndex++) {
        // Execute the P-State reduction code on the module's primary core only.
        // Skip this code for the BSC's module.
        if (ModuleIndex != Module) {
          if (GetGivenModuleCoreRange (Socket, ModuleIndex, &LowCore, &HighCore, StdHeader)) {
            ApUtilRunCodeOnSocketCore ((UINT8)Socket, (UINT8)LowCore, &TaskPtr, StdHeader);
          }
        }
      }
  }

    // Path for SCM and the BSC
    F15PmPwrCheckCore (&ErrorData, StdHeader);

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
 * Core-level error handler called if any p-states were determined to be out
 * of range for the mother board.
 *
 * This function implements steps 2a-c and 3a-c on each core.
 *
 * @param[in]  ErrorData            Details about the error condition.
 * @param[in]  StdHeader            Config handle for library and services.
 *
 */
VOID
STATIC
F15PmPwrCheckCore (
  IN       VOID *ErrorData,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8  i;
  UINT8  HwPsMaxVal;
  UINT8  SwPsMaxVal;
  UINT8  HwDisPsNum;
  UINT8  CurrentSwPs;
  UINT8  PsDisableCount;
  UINT64 LocalMsrRegister;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;

  if (IsCorePairPrimary (FirstCoreIsComputeUnitPrimary, StdHeader)) {
    // P-state MSRs are shared, so only 1 core per compute unit needs to perform this
    GetCpuServicesOfCurrentCore ((CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
    HwPsMaxVal = (((PWRCHK_ERROR_DATA *) ErrorData)->HwPstateNumber - 1);
    HwDisPsNum = (((PWRCHK_ERROR_DATA *) ErrorData)->HwPstateNumber -
                  ((PWRCHK_ERROR_DATA *) ErrorData)->AllowablePstateNumber);

    LibAmdMsrRead (MSR_PSTATE_STS, &LocalMsrRegister, StdHeader);
    CurrentSwPs = (UINT8) (((PSTATE_STS_MSR *) &LocalMsrRegister)->CurPstate);
    LibAmdMsrRead (MSR_PSTATE_CURRENT_LIMIT, &LocalMsrRegister, StdHeader);
    SwPsMaxVal = (UINT8) (((PSTATE_CURLIM_MSR *) &LocalMsrRegister)->PstateMaxVal);
    PsDisableCount = 0;

    if (((PWRCHK_ERROR_DATA *) ErrorData)->AllowablePstateNumber == 0) {
      // All P-States are over the limit.

      // Step 1
      // Transition to Pstate Max if not there already
      if (CurrentSwPs != SwPsMaxVal) {
        FamilySpecificServices->TransitionPstate (FamilySpecificServices, SwPsMaxVal, (BOOLEAN) TRUE, StdHeader);
      }

      // Step 2
      // If Pstate Max is not P0, copy Pstate max contents to P0 and switch
      // to P0.
      if (SwPsMaxVal != 0) {
        F15PmPwrChkCopyPstate (((PWRCHK_ERROR_DATA *) ErrorData)->NumberOfBoostStates, HwPsMaxVal, StdHeader);
        FamilySpecificServices->TransitionPstate (FamilySpecificServices, (UINT8) 0, (BOOLEAN) TRUE, StdHeader);
      }

      // Disable all SW P-states except P0
      PsDisableCount = ((PWRCHK_ERROR_DATA *) ErrorData)->NumberOfSwPstatesDisabled - 1;
    } else {
      // At least one P-State is under the limit & at least one P-State is
      // over the limit.
      if (((PWRCHK_ERROR_DATA *) ErrorData)->NumberOfBoostStates > HwDisPsNum) {
        // A subset of boosted P-states are disabled.  Copy the contents of the
        // highest performance boosted P-state still enabled to the boosted
        // P-states that have been disabled.
        for (i = 0; i < HwDisPsNum; i++) {
          F15PmPwrChkCopyPstate (i, HwDisPsNum, StdHeader);
        }
      } else if (((PWRCHK_ERROR_DATA *) ErrorData)->NumberOfSwPstatesDisabled != 0) {
        // Move remaining P-state register(s) up
        // Step 1
        // Transition to a valid Pstate if current Pstate has been disabled
        if (CurrentSwPs < ((PWRCHK_ERROR_DATA *) ErrorData)->NumberOfSwPstatesDisabled) {
          FamilySpecificServices->TransitionPstate (FamilySpecificServices, ((PWRCHK_ERROR_DATA *) ErrorData)->NumberOfSwPstatesDisabled, (BOOLEAN) TRUE, StdHeader);
          CurrentSwPs = ((PWRCHK_ERROR_DATA *) ErrorData)->NumberOfSwPstatesDisabled;
        }

        // Step 2
        // Move enabled Pstates up and disable the remainder
        for (i = ((PWRCHK_ERROR_DATA *) ErrorData)->NumberOfBoostStates; (i + ((PWRCHK_ERROR_DATA *) ErrorData)->NumberOfSwPstatesDisabled) <= HwPsMaxVal; i++) {
          F15PmPwrChkCopyPstate (i, (i + ((PWRCHK_ERROR_DATA *) ErrorData)->NumberOfSwPstatesDisabled), StdHeader);
        }

        // Step 3
        // Transition to current COF/VID at shifted location
        CurrentSwPs = (CurrentSwPs - ((PWRCHK_ERROR_DATA *) ErrorData)->NumberOfSwPstatesDisabled);
        FamilySpecificServices->TransitionPstate (FamilySpecificServices, CurrentSwPs, (BOOLEAN) TRUE, StdHeader);

        // Disable the appropriate number of P-states
        PsDisableCount = ((PWRCHK_ERROR_DATA *) ErrorData)->NumberOfSwPstatesDisabled;
      }
    }
    // Disable the appropriate P-states if any, starting from HW Pmin
    for (i = 0; i < PsDisableCount; i++) {
      FamilySpecificServices->DisablePstate (FamilySpecificServices, (HwPsMaxVal - i), StdHeader);
    }
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

