/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_12 P-State power check
 *
 * Performs the "Processor-Systemboard Power Delivery Compatibility Check" as
 * described in the BKDG.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/F12
 * @e \$Revision: 44324 $   @e \$Date: 2010-12-22 17:16:51 +0800 (Wed, 22 Dec 2010) $
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
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "cpuF12PowerMgmt.h"
#include "cpuRegisters.h"
#include "cpuApicUtilities.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "cpuEarlyInit.h"
#include "cpuFamilyTranslation.h"
#include "cpuF12PowerCheck.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_CPU_FAMILY_0X12_CPUF12POWERCHECK_FILECODE

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
F12PmPwrCheckCore (
  IN       VOID *ErrorData,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
STATIC
F12PmPwrChkCopyPstate (
  IN       UINT8 Dest,
  IN       UINT8 Src,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * Family 12h core 0 entry point for performing the family 12h Processor-
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
 *       a. Clear both D18F4x15C[BoostSrc] and D18F4x15C[NumBoostStates] to 0.
 *       b. If the processor's current P-State is disabled by the power check,
 *          then the BIOS must request a transition to an enabled P-state
 *          using MSRC001_0062[PstateCmd] and wait for MSRC001_0063[CurPstate]
 *          to reflect the new value.
 *       c. Copy the contents of the enabled P-state MSRs to the highest
 *          performance P-state locations.
 *       d. Request a P-state transition to the P-state MSR containing the
 *          COF/VID values currently applied.
 *       e. Adjust the following P-state parameters affected by the P-state
 *          MSR copy by subtracting the number of P-states that are disabled
 *          by the power check.
 *          1. D18F3x64[HtcPstateLimit]
 *          2. D18F3xDC[PstateMaxVal]
 *    3. If all P-States are over the limit, the BIOS must:
 *       a. Clear both D18F4x15C[BoostSrc] and D18F4x15C[NumBoostStates] to 0.
 *       b. If the processor's current P-State is != D18F3xDC[PstateMaxVal], then
 *          write D18F3xDC[PstateMaxVal] to MSRC001_0062[PstateCmd] and wait for
 *          MSRC001_0063[CurPstate] to reflect the new value.
 *       c. If D18F3xDC[PstateMaxVal]!= 000b, copy the contents of the P-state
 *          MSR pointed to by D18F3xDC[PstateMaxVal] to MSRC001_0064 and set
 *          MSRC001_0064[PstateEn]
 *       d. Write 000b to MSRC001_0062[PstateCmd] and wait for MSRC001_0063
 *          [CurPstate] to reflect the new value.
 *       e. Adjust the following P-state parameters to zero:
 *          1. D18F3x64[HtcPstateLimit]
 *          2. D18F3xDC[PstateMaxVal]
 *
 * @param[in]  FamilySpecificServices  The current Family Specific Services.
 * @param[in]  CpuEarlyParams          Service parameters.
 * @param[in]  StdHeader               Config handle for library and services.
 *
 */
VOID
F12PmPwrCheck (
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
  UINT32      PstateLimit;
  PCI_ADDR    PciAddress;
  UINT64      LocalMsrRegister;
  AP_TASK     TaskPtr;
  AGESA_STATUS IgnoredSts;
  PWRCHK_ERROR_DATA ErrorData;

  // get the socket number
  IdentifyCore (StdHeader, &Socket, &Module, &Core, &IgnoredSts);
  ErrorData.SocketNumber = (UINT8) Socket;

  ASSERT (Core == 0);

  // get the Max P-state value
  for (PsMaxVal = NM_PS_REG - 1; PsMaxVal != 0; --PsMaxVal) {
    LibAmdMsrRead (PS_REG_BASE + PsMaxVal, &LocalMsrRegister, StdHeader);
    if (((PSTATE_MSR *) &LocalMsrRegister)->PsEnable == 1) {
      break;
    }
  }

  ErrorData.HwPstateNumber = (UINT8) (PsMaxVal + 1);

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

  // If all P-state registers are disabled, move P[PsMaxVal] to P0
  // and transition to P0, then wait for CurPstate = 0

  ErrorData.AllowablePstateNumber = ((PsMaxVal + 1) - DisPsNum);

  // We only need to log this event on the BSC
  if (ErrorData.AllowablePstateNumber == 0) {
    PutEventLog (AGESA_FATAL,
                 CPU_EVENT_PM_ALL_PSTATE_OVERCURRENT,
                 Socket, 0, 0, 0, StdHeader);
  }

  if (DisPsNum != 0) {
    // Clear both D18F4x15C[BoostSrc] and D18F4x15C[NumBoostStates] to 0
    PciAddress.AddressValue = CPB_CTRL_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    ((CPB_CTRL_REGISTER *) &LocalPciRegister)->BoostSrc = 0;
    ((CPB_CTRL_REGISTER *) &LocalPciRegister)->NumBoostStates = 0;
    LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);

    TaskPtr.FuncAddress.PfApTaskI = F12PmPwrCheckCore;
    TaskPtr.DataTransfer.DataSizeInDwords = SIZE_IN_DWORDS (PWRCHK_ERROR_DATA);
    TaskPtr.DataTransfer.DataPtr = &ErrorData;
    TaskPtr.DataTransfer.DataTransferFlags = 0;
    TaskPtr.ExeFlags = WAIT_FOR_CORE;
    ApUtilRunCodeOnAllLocalCoresAtEarly (&TaskPtr, StdHeader, CpuEarlyParams);

    // Final Step
    //    D18F3x64[HtPstatelimit] -= disPsNum
    //    D18F3xDC[PstateMaxVal]-= disPsNum

    PciAddress.AddressValue = HTC_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader); // D18F3x64
    PstateLimit = ((HTC_REGISTER *) &LocalPciRegister)->HtcPstateLimit;
    if (PstateLimit > DisPsNum) {
      PstateLimit -= DisPsNum;
    } else {
      PstateLimit = 0;
    }
    ((HTC_REGISTER *) &LocalPciRegister)->HtcPstateLimit = PstateLimit;
    LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader); // D18F3x64

    PciAddress.AddressValue = CPTC2_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader); // D18F3xDC
    PstateLimit = ((CLK_PWR_TIMING_CTRL2_REGISTER *) &LocalPciRegister)->PstateMaxVal;
    if (PstateLimit > DisPsNum) {
      PstateLimit -= DisPsNum;
    } else {
      PstateLimit = 0;
    }
    ((CLK_PWR_TIMING_CTRL2_REGISTER *) &LocalPciRegister)->PstateMaxVal = PstateLimit;
    LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader); // D18F3xDC
  }
}


/*---------------------------------------------------------------------------------------*/
/**
 * Core-level error handler called if any p-states were determined to be out
 * of range for the mother board.
 *
 * This function implements steps 2b-d and 3b-d on each core.
 *
 * @param[in]  ErrorData            Details about the error condition.
 * @param[in]  StdHeader            Config handle for library and services.
 *
 */
VOID
STATIC
F12PmPwrCheckCore (
  IN       VOID *ErrorData,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8  i;
  UINT8  PsMaxVal;
  UINT8  DisPsNum;
  UINT8  CurrentPs;
  UINT64 LocalMsrRegister;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;

  GetCpuServicesOfCurrentCore ((const CPU_SPECIFIC_SERVICES **) &FamilySpecificServices, StdHeader);

  PsMaxVal = (((PWRCHK_ERROR_DATA *) ErrorData)->HwPstateNumber - 1);
  DisPsNum = (((PWRCHK_ERROR_DATA *) ErrorData)->HwPstateNumber -
             ((PWRCHK_ERROR_DATA *) ErrorData)->AllowablePstateNumber);

  LibAmdMsrRead (MSR_PSTATE_STS, &LocalMsrRegister, StdHeader);
  CurrentPs = (UINT8) (((PSTATE_STS_MSR *) &LocalMsrRegister)->CurPstate);

  if (((PWRCHK_ERROR_DATA *) ErrorData)->AllowablePstateNumber == 0) {

    // Step 1
    // Transition to Pstate Max if not there already

    if (CurrentPs != PsMaxVal) {
      FamilySpecificServices->TransitionPstate (FamilySpecificServices, PsMaxVal, (BOOLEAN) TRUE, StdHeader);
    }


    // Step 2
    // If Pstate Max is not P0, copy Pstate max contents to P0 and switch
    // to P0.

    if (PsMaxVal != 0) {
      F12PmPwrChkCopyPstate (0, PsMaxVal, StdHeader);
      FamilySpecificServices->TransitionPstate (FamilySpecificServices, (UINT8) 0, (BOOLEAN) TRUE, StdHeader);
    }
  } else {

    // move remaining P-state register(s) up
    // Step 1
    // Transition to a valid Pstate if current Pstate has been disabled

    if (CurrentPs < DisPsNum) {
      FamilySpecificServices->TransitionPstate (FamilySpecificServices, DisPsNum, (BOOLEAN) TRUE, StdHeader);
      CurrentPs = DisPsNum;
    }

    // Step 2
    // Move enabled Pstates up and disable the remainder

    for (i = 0; (i + DisPsNum) <= PsMaxVal; ++i) {
      F12PmPwrChkCopyPstate (i, (i + DisPsNum), StdHeader);
    }

    // Step 3
    // Transition to current COF/VID at shifted location

    CurrentPs = (CurrentPs - DisPsNum);
    FamilySpecificServices->TransitionPstate (FamilySpecificServices, CurrentPs, (BOOLEAN) TRUE, StdHeader);
  }
  i = ((PWRCHK_ERROR_DATA *) ErrorData)->AllowablePstateNumber;
  if (i == 0) {
    i++;
  }
  while (i <= PsMaxVal) {
    FamilySpecificServices->DisablePstate (FamilySpecificServices, i, StdHeader);
    i++;
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
F12PmPwrChkCopyPstate (
  IN       UINT8 Dest,
  IN       UINT8 Src,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64 LocalMsrRegister;

  LibAmdMsrRead ((UINT32) (PS_REG_BASE + Src), &LocalMsrRegister, StdHeader);
  LibAmdMsrWrite ((UINT32) (PS_REG_BASE + Dest), &LocalMsrRegister, StdHeader);
}

