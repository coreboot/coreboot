/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_14 P-State power check
 *
 * Performs the "Processor-Systemboard Power Delivery Compatibility Check" as
 * described in the BKDG.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/F14
 * @e \$Revision: 46951 $   @e \$Date: 2011-02-11 12:37:59 -0700 (Fri, 11 Feb 2011) $
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
#include "amdlib.h"
#include "cpuF14PowerMgmt.h"
#include "cpuRegisters.h"
#include "cpuApicUtilities.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "cpuEarlyInit.h"
#include "cpuFamilyTranslation.h"
#include "cpuF14PowerCheck.h"
#include "Filecode.h"
#define FILECODE PROC_CPU_FAMILY_0X14_CPUF14POWERCHECK_FILECODE

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
F14PmPwrCheckCore (
  IN       VOID *ErrorData,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
STATIC
F14PmPwrChkCopyPstate (
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
 * Family 14h Ontario core 0 entry point for performing the family 14h Ontario Processor-
 * Systemboard Power Delivery Check.
 *
 * The steps are as follows:
 *    1. Starting with hardware P0, loop through all P-states until a passing state is
 *       found.  A passing state is one in which the current required by the
 *       CPU is less than the maximum amount of current that the system can
 *       provide to the CPU.  If P0 is under the limit, no further action is
 *       necessary.
 *    2. If at least one P-State is under the limit & at least one P-State is
 *       over the limit, the BIOS must:
 *       a. Program D18F4x15C[BoostSrc]=0.
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
 *       a. Program D18F4x15C[BoostSrc]=0.
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
F14PmPwrCheck (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       AMD_CPU_EARLY_PARAMS  *CpuEarlyParams,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINT8       DisHwPsNum;
  UINT8       DisSwPsNum;
  UINT8       PsMaxVal;
  UINT8       Pstate;
  UINT8       PstateLimit;
  UINT8       NumberBoostStates;
  UINT32      ProcIddMax;
  UINT32      Socket;
  UINT32      Module;
  UINT32      Core;
  PCI_ADDR    PciAddress;
  UINT64      LocalMsrRegister;
  BOOLEAN     ThermalPstateEn;
  NB_CAPS_REGISTER              NbCaps;
  HTC_REGISTER                  HtcReg;
  CLK_PWR_TIMING_CTRL2_REGISTER ClkPwrTimingCtrl2;
  CPB_CTRL_REGISTER             CpbCtrl;
  CPU_LOGICAL_ID                CpuFamilyRevision;
  AP_TASK     TaskPtr;
  AGESA_STATUS IgnoredSts;
  PWRCHK_ERROR_DATA ErrorData;

  // get the socket number
  IdentifyCore (StdHeader, &Socket, &Module, &Core, &IgnoredSts);
  ErrorData.SocketNumber = (UINT8) Socket;

  ASSERT (Core == 0);

  // save ThermalPstateEn
  //   TRUE if the P-state indicated by D18F3x64[HtcPstateLimit] is enabled;
  //   FALSE otherwise.
  PciAddress.AddressValue = HTC_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, PciAddress, &HtcReg, StdHeader); // D18F3x64
  LibAmdMsrRead (PS_REG_BASE + HtcReg.HtcPstateLimit, &LocalMsrRegister, StdHeader);
  if (((PSTATE_MSR *) &LocalMsrRegister)->PsEnable == 1) {
    ThermalPstateEn = TRUE;
  } else {
    ThermalPstateEn = FALSE;
  }

  // get the Max P-state value
  for (PsMaxVal = NM_PS_REG - 1; PsMaxVal != 0; --PsMaxVal) {
    LibAmdMsrRead (PS_REG_BASE + PsMaxVal, &LocalMsrRegister, StdHeader);
    if (((PSTATE_MSR *) &LocalMsrRegister)->PsEnable == 1) {
      break;
    }
  }

  ErrorData.HwPstateNumber = (UINT8) (PsMaxVal + 1);

  // get NumberBoostStates
  GetLogicalIdOfCurrentCore (&CpuFamilyRevision, StdHeader);
  if ((CpuFamilyRevision.Revision & (AMD_F14_ON_Ax | AMD_F14_ON_Bx)) != 0) {
    NumberBoostStates = 0;
  } else {
    PciAddress.AddressValue = CPB_CTRL_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &CpbCtrl, StdHeader);  // D18F4x15C
    NumberBoostStates = (UINT8) CpbCtrl.NumBoostStates;
  }

  // update PstateMaxVal if warranted by HtcPstateLimit
  PciAddress.AddressValue = NB_CAPS_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, PciAddress, &NbCaps, StdHeader);
  if (NbCaps.HtcCapable == 1) {
    if (HtcReg.HtcTmpLmt != 0) {
      PciAddress.AddressValue = CPTC2_PCI_ADDR;
      LibAmdPciRead (AccessWidth32, PciAddress, &ClkPwrTimingCtrl2, StdHeader); // D18F3xDC
      if (HtcReg.HtcPstateLimit > ClkPwrTimingCtrl2.PstateMaxVal) {
        ClkPwrTimingCtrl2.PstateMaxVal = HtcReg.HtcPstateLimit;
        LibAmdPciWrite (AccessWidth32, PciAddress, &ClkPwrTimingCtrl2, StdHeader); // D18F3xDC
      }
    }
  }

  DisHwPsNum = 0;
  for (Pstate = 0; Pstate < ErrorData.HwPstateNumber; Pstate++) {
    if (FamilySpecificServices->GetProcIddMax (FamilySpecificServices, Pstate, &ProcIddMax, StdHeader)) {
      if (ProcIddMax > CpuEarlyParams->PlatformConfig.VrmProperties[CoreVrm].CurrentLimit) {
        // Add to event log the Pstate that exceeded the current limit
        PutEventLog (AGESA_WARNING,
                     CPU_EVENT_PM_PSTATE_OVERCURRENT,
                     Socket, Pstate, 0, 0, StdHeader);
        DisHwPsNum++;
      } else {
        break;
      }
    }
  }

  // get the number of software Pstate that is disabled by delivery check
  if (NumberBoostStates < DisHwPsNum) {
    DisSwPsNum = DisHwPsNum - NumberBoostStates;
  } else {
    DisSwPsNum = 0;
  }
  // If all P-state registers are disabled, move P[PsMaxVal] to P0
  // and transition to P0, then wait for CurPstate = 0

  ErrorData.AllowablePstateNumber = ((PsMaxVal + 1) - DisHwPsNum);

  // We only need to log this event on the BSC
  if (ErrorData.AllowablePstateNumber == 0) {
    PutEventLog (AGESA_FATAL,
                 CPU_EVENT_PM_ALL_PSTATE_OVERCURRENT,
                 Socket, 0, 0, 0, StdHeader);
  }

  if (DisHwPsNum != 0) {
    // Program F4x15C[BoostSrc] = 0
    if ((CpuFamilyRevision.Revision & (AMD_F14_ON_Ax | AMD_F14_ON_Bx)) == 0) {
      PciAddress.AddressValue = CPB_CTRL_PCI_ADDR;
      LibAmdPciRead (AccessWidth32, PciAddress, &CpbCtrl, StdHeader);  // D18F4x15C
      CpbCtrl.BoostSrc = 0;
      LibAmdPciWrite (AccessWidth32, PciAddress, &CpbCtrl, StdHeader);  // D18F4x15C
    }

    TaskPtr.FuncAddress.PfApTaskI = F14PmPwrCheckCore;
    TaskPtr.DataTransfer.DataSizeInDwords = SIZE_IN_DWORDS (PWRCHK_ERROR_DATA);
    TaskPtr.DataTransfer.DataPtr = &ErrorData;
    TaskPtr.DataTransfer.DataTransferFlags = 0;
    TaskPtr.ExeFlags = WAIT_FOR_CORE;
    ApUtilRunCodeOnAllLocalCoresAtEarly (&TaskPtr, StdHeader, CpuEarlyParams);

    // Final Step
    //    D18F3x64[HtPstatelimit] -= disPsNum
    //    D18F3xDC[PstateMaxVal]-= disPsNum
    PciAddress.AddressValue = HTC_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &HtcReg, StdHeader); // D18F3x64
    PciAddress.AddressValue = NB_CAPS_PCI_ADDR; // D18F3xE8
    LibAmdPciRead (AccessWidth32, PciAddress, &NbCaps, StdHeader);
    if (ThermalPstateEn || HtcReg.HtcTmpLmt == 0 || NbCaps.HtcCapable == 0) {
      PstateLimit = (UINT8) HtcReg.HtcPstateLimit;
      if (PstateLimit > DisHwPsNum) {
        PstateLimit = (UINT8) (PstateLimit - DisSwPsNum);
    } else {
        PstateLimit = NumberBoostStates;
    }
      HtcReg.HtcPstateLimit = PstateLimit;
      PciAddress.AddressValue = HTC_PCI_ADDR;
      LibAmdPciWrite (AccessWidth32, PciAddress, &HtcReg, StdHeader); // D18F3x64

    PciAddress.AddressValue = CPTC2_PCI_ADDR;
      LibAmdPciRead (AccessWidth32, PciAddress, &ClkPwrTimingCtrl2, StdHeader); // D18F3xDC
      PstateLimit = (UINT8) ClkPwrTimingCtrl2.PstateMaxVal;
      if (PstateLimit > DisHwPsNum) {
        PstateLimit = (UINT8) (PstateLimit - DisSwPsNum);
    } else {
        PstateLimit = NumberBoostStates;
      }
      ClkPwrTimingCtrl2.PstateMaxVal = PstateLimit;
      LibAmdPciWrite (AccessWidth32, PciAddress, &ClkPwrTimingCtrl2, StdHeader); // D18F3xDC
    }
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
F14PmPwrCheckCore (
  IN       VOID *ErrorData,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8  i;
  UINT8  HardwarePsMaxVal;
  UINT8  DisHwPsNum;
  UINT8  DisSwPsNum;
  UINT8  CurrentSoftwarePs;
  UINT8  CurrentHardwarePs;
  UINT8  NumberBoostStates;
  UINT64 LocalMsrRegister;
  CPU_LOGICAL_ID        CpuFamilyRevision;
  PCI_ADDR              PciAddress;
  CPB_CTRL_REGISTER     CpbCtrl;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;

  GetCpuServicesOfCurrentCore ((const CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
  GetLogicalIdOfCurrentCore (&CpuFamilyRevision, StdHeader);

  HardwarePsMaxVal = (((PWRCHK_ERROR_DATA *) ErrorData)->HwPstateNumber - 1);
  DisHwPsNum = (((PWRCHK_ERROR_DATA *) ErrorData)->HwPstateNumber -
             ((PWRCHK_ERROR_DATA *) ErrorData)->AllowablePstateNumber);

  LibAmdMsrRead (MSR_PSTATE_STS, &LocalMsrRegister, StdHeader);
  CurrentSoftwarePs = (UINT8) (((PSTATE_STS_MSR *) &LocalMsrRegister)->CurPstate);

  if ((CpuFamilyRevision.Revision & (AMD_F14_ON_Ax | AMD_F14_ON_Bx)) != 0) {
    NumberBoostStates = 0;
  } else {
    PciAddress.AddressValue = CPB_CTRL_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &CpbCtrl, StdHeader);  // D18F4x15C
    NumberBoostStates = (UINT8) CpbCtrl.NumBoostStates;
  }

  CurrentHardwarePs = CurrentSoftwarePs + NumberBoostStates;

  if (NumberBoostStates < DisHwPsNum) {
    DisSwPsNum = DisHwPsNum - NumberBoostStates;
  } else {
    DisSwPsNum = 0;
  }

  if (((PWRCHK_ERROR_DATA *) ErrorData)->AllowablePstateNumber == 0) {

    // Step 1
    // Transition to Pstate Max if not there already

    if (CurrentHardwarePs != HardwarePsMaxVal) {
      FamilySpecificServices->TransitionPstate (FamilySpecificServices, (HardwarePsMaxVal - NumberBoostStates), (BOOLEAN) TRUE, StdHeader);
      CurrentSoftwarePs = HardwarePsMaxVal - NumberBoostStates;
    }


    // Step 2
    // If CurrentSoftwarePs is not P0, copy CurrentSoftwarePs contents to Software P0 and switch
    // to P0.

    if (CurrentSoftwarePs != 0) {
      F14PmPwrChkCopyPstate (NumberBoostStates, CurrentSoftwarePs, StdHeader);
      LibAmdMsrRead ((PS_REG_BASE + NumberBoostStates), &LocalMsrRegister, StdHeader);
      ((PSTATE_MSR *) &LocalMsrRegister)->PsEnable = 1;
      LibAmdMsrWrite ((PS_REG_BASE + NumberBoostStates), &LocalMsrRegister, StdHeader);
      FamilySpecificServices->TransitionPstate (FamilySpecificServices, (UINT8) 0, (BOOLEAN) TRUE, StdHeader);
    }
  } else {

    // move remaining P-state register(s) up
    // Step 1
    // Transition to a valid Pstate if current Pstate has been disabled

    if (CurrentHardwarePs < DisHwPsNum) {
      FamilySpecificServices->TransitionPstate (FamilySpecificServices, (HardwarePsMaxVal - NumberBoostStates), (BOOLEAN) TRUE, StdHeader);
      CurrentSoftwarePs = HardwarePsMaxVal - NumberBoostStates;
    }

    if (DisSwPsNum != 0) {
    // Step 2
    // Move enabled Pstates up and disable the remainder

      for (i = 0; (i + DisHwPsNum) <= HardwarePsMaxVal; ++i) {
        F14PmPwrChkCopyPstate ((i + NumberBoostStates), (i + DisHwPsNum), StdHeader);
    }
    // Step 3
    // Transition to current COF/VID at shifted location

      CurrentSoftwarePs = (CurrentSoftwarePs - DisSwPsNum);
      FamilySpecificServices->TransitionPstate (FamilySpecificServices, CurrentSoftwarePs, (BOOLEAN) TRUE, StdHeader);
    }
  }

  if (((PWRCHK_ERROR_DATA *) ErrorData)->AllowablePstateNumber == 0) {
    // only software P0 should be enabled.
    i = NumberBoostStates + 1;
  } else {
    if (DisSwPsNum == 0) {
      // No software Pstate is disabed, set i = HardwarePsMaxVal + 1 to skip below 'while loop'.
      i = HardwarePsMaxVal + 1;
    } else {
      // get the first software Pstate that should be disabled.
      i = HardwarePsMaxVal - DisSwPsNum + 1;
  }
  }
  while (i <= HardwarePsMaxVal) {
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
F14PmPwrChkCopyPstate (
  IN       UINT8 Dest,
  IN       UINT8 Src,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64 LocalMsrRegister;

  LibAmdMsrRead ((UINT32) (PS_REG_BASE + Src), &LocalMsrRegister, StdHeader);
  LibAmdMsrWrite ((UINT32) (PS_REG_BASE + Dest), &LocalMsrRegister, StdHeader);
}

