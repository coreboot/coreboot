/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_10 specific utility functions.
 *
 * Provides numerous utility functions specific to family 10h.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/F10
 * @e \$Revision: 50057 $   @e \$Date: 2011-04-01 13:30:57 +0800 (Fri, 01 Apr 2011) $
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
#include "Ids.h"
#include "cpuRegisters.h"
#include "cpuFamilyTranslation.h"
#include "cpuPstateTables.h"
#include "cpuF10PowerMgmt.h"
#include "cpuApicUtilities.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "cpuF10Utilities.h"
#include "cpuPostInit.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_CPU_FAMILY_0X10_CPUF10UTILITIES_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
// Register encodings for F3xD8[VSRampTime/VSSlamTime]
CONST UINT32 ROMDATA VSSlamTime[8] =
{
  10,   // 000b: 10us
  20,   // 001b: 20us
  30,   // 010b: 30us
  40,   // 011b: 40us
  60,   // 100b: 60us
  100,  // 101b: 100us
  200,  // 110b: 200us
  500   // 111b: 500us
};

extern CPU_FAMILY_SUPPORT_TABLE            PstateFamilyServiceTable;

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

/*---------------------------------------------------------------------------------------*/
/**
 *  Performs the necessary steps for the 'Software Initiated CPU
 *  Voltage Transitions.'
 *
 *  @param[in]   VidCode       VID code to transition to
 *  @param[in]   StdHeader     Header for library and services
 *
 */
VOID
F10PmSwVoltageTransition (
  IN       UINT32 VidCode,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32    LocalPciRegister;
  UINT32    Socket;
  UINT32    Module;
  UINT32    Ignored;
  UINT64    LocalMsrRegister;
  PCI_ADDR  PciAddress;
  AGESA_STATUS IgnoredSts;

  IdentifyCore (StdHeader, &Socket, &Module, &Ignored, &IgnoredSts);
  GetPciAddress (StdHeader, Socket, Module, &PciAddress, &IgnoredSts);

  PciAddress.Address.Function = FUNC_3;
  PciAddress.Address.Register = PW_CTL_MISC_REG;
  LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
  if (((POWER_CTRL_MISC_REGISTER *) &LocalPciRegister)->SlamVidMode == 1) {
    LibAmdMsrRead (MSR_COFVID_CTL, &LocalMsrRegister, StdHeader);
    ((COFVID_CTRL_MSR *) &LocalMsrRegister)->CpuVid = VidCode;
    LibAmdMsrWrite (MSR_COFVID_CTL, &LocalMsrRegister, StdHeader);
    F10WaitOutVoltageTransition  (TRUE, StdHeader);
  } else
    return;
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Performs the necessary steps for the 'Software Initiated NB
 *  Voltage Transitions.'
 *
 *  This can only be run by a local core 0.
 *
 *  @param[in]   VidCode       VID code to transition to
 *  @param[in]   SlamMode      Whether voltage is to be slammed, or stepped
 *  @param[in]   StdHeader     Header for library and services
 *
 */
VOID
F10PmSwVoltageTransitionServerNb (
  IN       UINT32 VidCode,
  IN       BOOLEAN SlamMode,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 Core;
  UINT32 NbVidStatus;
  UINT32 Socket;
  UINT32 IgnoredModule;
  UINT32 IgnoredCore;
  UINT32 CoreNum;
  AP_TASK TaskPtr;
  AGESA_STATUS IgnoredSts;
  SW_VOLT_TRANS_NB RemoteInput;

  RemoteInput.VidCode = VidCode;
  RemoteInput.SlamMode = SlamMode;
  TaskPtr.FuncAddress.PfApTaskIO = F10SwVoltageTransitionServerNbCore;
  TaskPtr.DataTransfer.DataSizeInDwords = SIZE_IN_DWORDS (SW_VOLT_TRANS_NB);
  TaskPtr.DataTransfer.DataPtr = &RemoteInput;
  TaskPtr.DataTransfer.DataTransferFlags = 0;
  TaskPtr.ExeFlags = WAIT_FOR_CORE;

  IdentifyCore (StdHeader, &Socket, &IgnoredModule, &IgnoredCore, &IgnoredSts);
  GetActiveCoresInCurrentSocket (&CoreNum, StdHeader);

  do {
    NbVidStatus = TaskPtr.FuncAddress.PfApTaskIO (&RemoteInput, StdHeader);
    for (Core = 1; Core < (UINT8) CoreNum; Core++) {
      NbVidStatus |= ApUtilRunCodeOnSocketCore ((UINT8)Socket, (UINT8)Core, &TaskPtr, StdHeader);
    }
    F10WaitOutVoltageTransition (SlamMode, StdHeader);
  } while (NbVidStatus != 0);
  return;
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Returns current VsSlamTime in microseconds.
 *
 *  @param[out]  VsTimeUsecs   Provides the wait time needed for a Slam Voltage transition.
 *  @param[in]   SlamMode      Whether voltage is to be slammed, or stepped
 *  @param[in]   StdHeader     Header for library and services
 *
 */
VOID
F10GetCurrentVsTimeInUsecs (
     OUT   UINT32  *VsTimeUsecs,
  IN       BOOLEAN SlamMode,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8     RegisterEncoding;
  UINT32    LocalPciRegister;
  UINT32    Socket;
  UINT32    Module;
  UINT32    Ignored;
  CONST UINT16    SlamTimes[8] = {10, 20, 30, 40, 60, 100, 200, 500};
  PCI_ADDR  PciAddress;
  AGESA_STATUS IgnoredSts;

  IdentifyCore (StdHeader, &Socket, &Module, &Ignored, &IgnoredSts);
  GetPciAddress (StdHeader, Socket, Module, &PciAddress, &IgnoredSts);

  PciAddress.Address.Function = FUNC_3;
  PciAddress.Address.Register = CPTC1_REG;
  LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);

  if (SlamMode) {
    RegisterEncoding = (UINT8) ((CLK_PWR_TIMING_CTRL1_REGISTER *) &LocalPciRegister)->VSSlamTime;
  } else {
    RegisterEncoding = (UINT8) ((CLK_PWR_TIMING_CTRL1_REGISTER *) &LocalPciRegister)->VSRampTime;
  }

  *VsTimeUsecs = (UINT32) SlamTimes[RegisterEncoding];
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Spins until VsSlamTime microseconds have expired.
 *
 *  @param[in]   SlamMode      Whether voltage is to be slammed, or stepped
 *  @param[in]   StdHeader     Header for library and services
 *
 */
VOID
F10WaitOutVoltageTransition (
  IN       BOOLEAN SlamMode,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 VsTimeUsecs;

  F10GetCurrentVsTimeInUsecs (&VsTimeUsecs, SlamMode, StdHeader);
  WaitMicroseconds (VsTimeUsecs, StdHeader);
  return;
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Code required to be run on every local core in order to perform
 *  the steps necessary for 'Software Initiated NB Voltage
 *  Transitions.'
 *
 *  @param[out]  InputData     Family specific data needed to perform a Voltage transition.
 *  @param[in]   StdHeader     Header for library and services.
 *
 *  @retval     zero           All Voltage Transitions are completed.
 *  @retval     one            There are Voltage transitions remaining to reach target.
 *
 */
UINT32
F10SwVoltageTransitionServerNbCore (
  IN       VOID  *InputData,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 VidCode;
  UINT64 LocalMsrRegister;

  if (((SW_VOLT_TRANS_NB *) InputData)->SlamMode) {
    VidCode = ((SW_VOLT_TRANS_NB *) InputData)->VidCode;
  } else {
    LibAmdMsrRead (MSR_COFVID_STS, &LocalMsrRegister, StdHeader);
    VidCode = (UINT32) (((COFVID_STS_MSR *) &LocalMsrRegister)->CurNbVid);
    if (VidCode > ((SW_VOLT_TRANS_NB *) InputData)->VidCode) {
      --VidCode;
    } else if (VidCode < ((SW_VOLT_TRANS_NB *) InputData)->VidCode) {
      ++VidCode;
    }
  }
  LibAmdMsrRead (MSR_COFVID_CTL, &LocalMsrRegister, StdHeader);
  ((COFVID_CTRL_MSR *) &LocalMsrRegister)->NbVid = VidCode;
  LibAmdMsrWrite (MSR_COFVID_CTL, &LocalMsrRegister, StdHeader);

  if (VidCode == ((SW_VOLT_TRANS_NB *) InputData)->VidCode) {
    return 0;
  } else {
    return 1;
  }
}


/*---------------------------------------------------------------------------------------*/
/**
 * Calculate and reprogram F3xD8[VSSlamTime] based on the algorithm in the BKDG.
 *
 * This function determines the largest voltage step that the core will have
 * to make, calculates how much time it will take for the voltage to stabilize,
 * and programs the necessary encoded value for the amount of time discovered.
 *
 * @param[in]  PciAddress         Segment/bus/device of a module on the socket
 *                                to program.
 * @param[in]  CpuEarlyParams     Service parameters
 * @param[in]  StdHeader          Config handle for library and services.
 *
 */
VOID
F10ProgramVSSlamTimeOnSocket (
  IN       PCI_ADDR             *PciAddress,
  IN       AMD_CPU_EARLY_PARAMS *CpuEarlyParams,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{
  UINT8    NbVid;
  UINT8    P0VidCode;
  UINT8    PminVidCode;
  UINT32   AndMask;
  UINT32   MsrAddr;
  UINT32   OrMask;
  UINT32   LocalPciRegister;
  UINT64   LocalMsrRegister;
  BOOLEAN  IsPviMode;
  PCI_ADDR LocalPciAddress;

  // Get F3xA0[PviMode]
  LocalPciAddress.AddressValue = PciAddress->AddressValue;
  LocalPciAddress.Address.Function = FUNC_3;
  LocalPciAddress.Address.Register = PW_CTL_MISC_REG;
  LibAmdPciRead (AccessWidth32, LocalPciAddress, &LocalPciRegister, StdHeader);
  if (((POWER_CTRL_MISC_REGISTER *) &LocalPciRegister)->PviMode == 1) {
    IsPviMode = TRUE;
  } else {
    IsPviMode = FALSE;
  }

  // Get P0's voltage
  LibAmdMsrRead (PS_REG_BASE, &LocalMsrRegister, StdHeader);
  P0VidCode = (UINT8) (((PSTATE_MSR *) &LocalMsrRegister)->CpuVid);

  // If SVI, we only care about CPU VID.
  // If PVI, determine the higher voltage between NB and CPU
  if (IsPviMode) {
    NbVid = (UINT8) (((PSTATE_MSR *) &LocalMsrRegister)->NbVid);
    if (P0VidCode > NbVid) {
      P0VidCode = NbVid;
    }
  }

  // Get Pmin's index
  LibAmdMsrRead (MSR_PSTATE_CURRENT_LIMIT, &LocalMsrRegister, StdHeader);
  MsrAddr = (UINT32) ((((PSTATE_CURLIM_MSR *) &LocalMsrRegister)->PstateMaxVal) + PS_REG_BASE);

  // Get Pmin's VID
  LibAmdMsrRead (MsrAddr, &LocalMsrRegister, StdHeader);
  PminVidCode = (UINT8) (((PSTATE_MSR *) &LocalMsrRegister)->CpuVid);

  // If SVI, we only care about CPU VID.
  // If PVI, determine the higher voltage b/t NB and CPU
  if (IsPviMode) {
    NbVid = (UINT8) (((PSTATE_MSR *) &LocalMsrRegister)->NbVid);
    if (PminVidCode > NbVid) {
      PminVidCode = NbVid;
    }
  }

  // Program F3xD8[VSSlamTime]
  LocalPciAddress.Address.Register = CPTC1_REG;
  AndMask = 0xFFFFFFFF;
  ((CLK_PWR_TIMING_CTRL1_REGISTER *) &AndMask)->VSSlamTime = 0;
  OrMask = 0x00000000;
  ((CLK_PWR_TIMING_CTRL1_REGISTER *) &OrMask)->VSSlamTime =
    F10GetSlamTimeEncoding (P0VidCode, PminVidCode, CpuEarlyParams, VSSlamTime, StdHeader);
  ModifyCurrentSocketPci (&LocalPciAddress, AndMask, OrMask, StdHeader);
}


/*---------------------------------------------------------------------------------------*/
/**
 * Returns the encoded voltage stabilization slam time for the executing
 * family 10h core.
 *
 * This function looks up the appropriate encoded value for the desired
 * VID codes.
 *
 * @param[in]  HighVoltageVid     VID code of the higher voltage.
 * @param[in]  LowVoltageVid      VID code of the lower voltage.
 * @param[in]  CpuEarlyParams     Service parameters
 * @param[in]  SlamTimeTable      Look-up table of slam times.
 * @param[in]  StdHeader          Config handle for library and services.
 *
 * @retval     Encoded register value.
 *
 */
UINT32
F10GetSlamTimeEncoding (
  IN       UINT8  HighVoltageVid,
  IN       UINT8  LowVoltageVid,
  IN       AMD_CPU_EARLY_PARAMS *CpuEarlyParams,
  IN       CONST UINT32 *SlamTimeTable,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 SlamTime;
  UINT32 EncodedSlamTime;
  UINT32 VoltageDifference;

  ASSERT (LowVoltageVid >= HighVoltageVid);
  ASSERT (CpuEarlyParams->PlatformConfig.VrmProperties[CoreVrm].SlewRate != 0);

  // Calculate Slam Time
  //   VSSlamTime = 0.4us/mV (or 0.2us/mV) * Vhigh - Vlow
  //   In our case, we will scale the values by 100 to avoid
  //   decimals.

  VoltageDifference = (UINT32) ((LowVoltageVid - HighVoltageVid) * 12500);
  SlamTime = (VoltageDifference / CpuEarlyParams->PlatformConfig.VrmProperties[CoreVrm].SlewRate) + CpuEarlyParams->PlatformConfig.VrmProperties[CoreVrm].AdditionalDelay;
  if (VoltageDifference % CpuEarlyParams->PlatformConfig.VrmProperties[CoreVrm].SlewRate) {
    SlamTime++;
  }

  // Now round up to nearest register setting
  for (EncodedSlamTime = 0; EncodedSlamTime < 8; EncodedSlamTime++) {
    if (SlamTime <= SlamTimeTable[EncodedSlamTime]) {
      break;
    }
  }

  if (EncodedSlamTime > 7) {
    // The VRMs are too slow for this CPU.  Set to max, and fire an error trap.
    IDS_ERROR_TRAP;
    EncodedSlamTime = 7;
  }

  return (EncodedSlamTime);
}


/*---------------------------------------------------------------------------------------*/
/**
 *  Disables the desired P-state.
 *
 *  @CpuServiceMethod{::F_CPU_DISABLE_PSTATE}.
 *
 *  @param[in]   FamilySpecificServices   The current Family Specific Services.
 *  @param[in]   StateNumber              The P-State to disable.
 *  @param[in]   StdHeader                Header for library and services
 *
 *  @retval      AGESA_SUCCESS Always succeeds.
 */
AGESA_STATUS
F10DisablePstate (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       UINT8 StateNumber,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64 LocalMsrRegister;

  ASSERT (StateNumber < NM_PS_REG);
  LibAmdMsrRead (PS_REG_BASE + (UINT32) StateNumber, &LocalMsrRegister, StdHeader);
  ((PSTATE_MSR *) &LocalMsrRegister)->PsEnable = 0;
  LibAmdMsrWrite (PS_REG_BASE + (UINT32) StateNumber, &LocalMsrRegister, StdHeader);
  return (AGESA_SUCCESS);
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Transitions the executing core to the desired P-state.
 *
 *  @CpuServiceMethod{::F_CPU_TRANSITION_PSTATE}.
 *
 *  @param[in]   FamilySpecificServices   The current Family Specific Services.
 *  @param[in]   StateNumber              The new P-State to make effective.
 *  @param[in]   WaitForTransition        True if the caller wants the transition completed upon return.
 *  @param[in]   StdHeader                Header for library and services
 *
 *  @retval      AGESA_SUCCESS       Always Succeeds
 */
AGESA_STATUS
F10TransitionPstate (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       UINT8              StateNumber,
  IN       BOOLEAN            WaitForTransition,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64 LocalMsrRegister;

  LibAmdMsrRead (MSR_PSTATE_CURRENT_LIMIT, &LocalMsrRegister, StdHeader);
  ASSERT (((PSTATE_CURLIM_MSR *) &LocalMsrRegister)->PstateMaxVal >= StateNumber);
  LibAmdMsrRead (MSR_PSTATE_CTL, &LocalMsrRegister, StdHeader);
  ((PSTATE_CTRL_MSR *) &LocalMsrRegister)->PstateCmd = (UINT64) StateNumber;
  LibAmdMsrWrite (MSR_PSTATE_CTL, &LocalMsrRegister, StdHeader);
  if (WaitForTransition) {
    do {
      LibAmdMsrRead (MSR_PSTATE_STS, &LocalMsrRegister, StdHeader);
    } while (((PSTATE_STS_MSR *) &LocalMsrRegister)->CurPstate != (UINT64) StateNumber);
  }
  return (AGESA_SUCCESS);
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Determines the rate at which the executing core's time stamp counter is
 *  incrementing.
 *
 *  @CpuServiceMethod{::F_CPU_GET_TSC_RATE}.
 *
 *  @param[in]   FamilySpecificServices   The current Family Specific Services.
 *  @param[out]  FrequencyInMHz           TSC actual frequency.
 *  @param[in]   StdHeader                Header for library and services.
 *
 *  @return      The most severe status of all called services
 */
AGESA_STATUS
F10GetTscRate (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
     OUT   UINT32 *FrequencyInMHz,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64 LocalMsrRegister;
  PSTATE_CPU_FAMILY_SERVICES  *FamilyServices;

  FamilyServices = NULL;
  GetFeatureServicesOfCurrentCore (&PstateFamilyServiceTable, &FamilyServices, StdHeader);
  ASSERT (FamilyServices != NULL);

  LibAmdMsrRead (MSR_HWCR, &LocalMsrRegister, StdHeader);
  if ((LocalMsrRegister & 0x01000000) != 0) {
    return (FamilyServices->GetPstateFrequency (FamilyServices, F10GetNumberOfBoostedPstatesOnCore (StdHeader), FrequencyInMHz, StdHeader));
  } else {
    return (FamilySpecificServices->GetCurrentNbFrequency (FamilySpecificServices, FrequencyInMHz, StdHeader));
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Determines the NB clock on the desired node.
 *
 *  @CpuServiceMethod{::F_CPU_GET_NB_FREQ}.
 *
 *  @param[in]   FamilySpecificServices   The current Family Specific Services.
 *  @param[out]  FrequencyInMHz           Northbridge clock frequency in MHz.
 *  @param[in]   StdHeader                Header for library and services.
 *
 *  @return      AGESA_SUCCESS            FrequencyInMHz is valid.
 */
AGESA_STATUS
F10GetCurrentNbFrequency (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
     OUT   UINT32 *FrequencyInMHz,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32  Socket;
  UINT32  Module;
  UINT32  Core;
  UINT32  NbFid;
  UINT32  LocalPciRegister;
  UINT64  LocalMsrRegister;
  PCI_ADDR PciAddress;
  AGESA_STATUS ReturnCode;

  // get the local node ID
  IdentifyCore (StdHeader, &Socket, &Module, &Core, &ReturnCode);
  if (ReturnCode == AGESA_SUCCESS) {
    GetPciAddress (StdHeader, Socket, Module, &PciAddress, &ReturnCode);
    if (ReturnCode == AGESA_SUCCESS) {
      PciAddress.Address.Function = FUNC_3;
      PciAddress.Address.Register = CPTC0_REG;
      LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
      NbFid = ((CLK_PWR_TIMING_CTRL_REGISTER *) &LocalPciRegister)->NbFid;
      LibAmdMsrRead (MSR_COFVID_STS, &LocalMsrRegister, StdHeader);
      if (((COFVID_STS_MSR *) &LocalMsrRegister)->CurNbDid == 0) {
        *FrequencyInMHz = ((NbFid + 4) * 200);
      } else {
        *FrequencyInMHz = (((NbFid + 4) * 200) / 2);
      }
    }
  }
  return ReturnCode;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Initially launches the desired core to run from the reset vector.
 *
 * @CpuServiceMethod{::F_CPU_AP_INITIAL_LAUNCH}.
 *
 * @param[in]   FamilySpecificServices   The current Family Specific Services.
 * @param[in]   SocketNum                The Processor on which the core is to be launched
 * @param[in]   ModuleNum                The Module in that processor containing that core
 * @param[in]   CoreNum                  The Core to launch
 * @param[in]   PrimaryCoreNum           The id of the module's primary core.
 * @param[in]   StdHeader                Header for library and services
 *
 * @retval      TRUE          The core was launched
 * @retval      FALSE         The core was previously launched
 */
BOOLEAN
F10LaunchApCore (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       UINT32 SocketNum,
  IN       UINT32 ModuleNum,
  IN       UINT32 CoreNum,
  IN       UINT32 PrimaryCoreNum,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32    NodeRelativeCoreNum;
  UINT32    LocalPciRegister;
  PCI_ADDR  PciAddress;
  BOOLEAN   LaunchFlag;
  AGESA_STATUS Ignored;

  // Code Start
  LaunchFlag = FALSE;
  NodeRelativeCoreNum = CoreNum - PrimaryCoreNum;
  GetPciAddress (StdHeader, SocketNum, ModuleNum, &PciAddress, &Ignored);
  PciAddress.Address.Function = FUNC_0;

  switch (NodeRelativeCoreNum) {
  case 0:
    PciAddress.Address.Register = HT_INIT_CTRL;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    if ((LocalPciRegister & HT_INIT_CTRL_REQ_DIS) != 0) {
      LocalPciRegister &= ~HT_INIT_CTRL_REQ_DIS;
      LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
      LaunchFlag = TRUE;
    } else {
      LaunchFlag = FALSE;
    }
    break;

  case 1:
    PciAddress.Address.Register = HT_TRANS_CTRL;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    if ((LocalPciRegister & HT_TRANS_CTRL_CPU1_EN) == 0) {
      LocalPciRegister |= HT_TRANS_CTRL_CPU1_EN;
      LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
      LaunchFlag = TRUE;
    } else {
      LaunchFlag = FALSE;
    }
    break;

  case 2:
    PciAddress.Address.Register = ECS_HT_TRANS_CTRL;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);

    if ((LocalPciRegister & ECS_HT_TRANS_CTRL_CPU2_EN) == 0) {
      LocalPciRegister |= ECS_HT_TRANS_CTRL_CPU2_EN;
      LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister,
                      StdHeader);
      LaunchFlag = TRUE;
    } else {
      LaunchFlag = FALSE;
    }
    break;

  case 3:
    PciAddress.Address.Register = ECS_HT_TRANS_CTRL;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    if ((LocalPciRegister & ECS_HT_TRANS_CTRL_CPU3_EN) == 0) {
      LocalPciRegister |= ECS_HT_TRANS_CTRL_CPU3_EN;
      LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
      LaunchFlag = TRUE;
    } else {
      LaunchFlag = FALSE;
    }
    break;

  case 4:
    PciAddress.Address.Register = ECS_HT_TRANS_CTRL;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    if ((LocalPciRegister & ECS_HT_TRANS_CTRL_CPU4_EN) == 0) {
      LocalPciRegister |= ECS_HT_TRANS_CTRL_CPU4_EN;
      LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
      LaunchFlag = TRUE;
    } else {
      LaunchFlag = FALSE;
    }
    break;

  case 5:
    PciAddress.Address.Register = ECS_HT_TRANS_CTRL;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    if ((LocalPciRegister & ECS_HT_TRANS_CTRL_CPU5_EN) == 0) {
      LocalPciRegister |= ECS_HT_TRANS_CTRL_CPU5_EN;
      LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
      LaunchFlag = TRUE;
    } else {
      LaunchFlag = FALSE;
    }
    break;


  default:
    break;
  }

  return (LaunchFlag);
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Get CPU Specific Platform Type Info.
 *
 *  @CpuServiceMethod{::F_CPU_GET_PLATFORM_TYPE_SPECIFIC_INFO}.
 *
 *    This function returns Returns the platform features.
 *
 *  @param[in]     FamilySpecificServices         The current Family Specific Services.
 *  @param[in,out] Features                       The Features supported by this platform.
 *  @param[in]     StdHeader                      Handle of Header for calling lib functions and services.
 *
 *  @retval      AGESA_SUCCESS Always succeeds.
 */
AGESA_STATUS
F10GetPlatformTypeSpecificInfo (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN OUT   PLATFORM_FEATS         *Features,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  return (AGESA_SUCCESS);
}

/*---------------------------------------------------------------------------------------*/
/**
 * Provide the features of the next HT link.
 *
 *  @CpuServiceMethod{::F_GET_NEXT_HT_LINK_FEATURES}.
 *
 * This method is different than the HT Phy Features method, because for the phy registers
 * sublink 1 matches and should be programmed if the link is ganged but for PCI config
 * registers sublink 1 is reserved if the link is ganged.
 *
 * @param[in]     FamilySpecificServices    The current Family Specific Services.
 * @param[in,out] Link                      Initially zero, each call returns the link number;
 *                                          caller passes it back unmodified each call.
 * @param[in,out] LinkBase                  Initially the PCI bus, device, function=0, offset=0;
 *                                          Each call returns the HT Host Capability function and offset;
 *                                          Caller may use it to access registers, but must @b not modify it;
 *                                          Each new call passes the previous value as input.
 * @param[out]    HtHostFeats               The link's features.
 * @param[in]     StdHeader                 Standard Head Pointer
 *
 * @retval        TRUE                      Valid link and features found.
 * @retval        FALSE                     No more links.
 */
BOOLEAN
F10GetNextHtLinkFeatures (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN OUT   UINTN                  *Link,
  IN OUT   PCI_ADDR               *LinkBase,
     OUT   HT_HOST_FEATS          *HtHostFeats,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  PCI_ADDR  PciAddress;
  UINT32    RegValue;
  UINT32    ExtendedFreq;
  UINTN     LinkOffset;
  BOOLEAN   Result;

  ASSERT (FamilySpecificServices != NULL);

  // No features present unless link is good and connected.
  HtHostFeats->HtHostValue = 0;

  Result = TRUE;

  // Find next link.
  if (LinkBase->Address.Register == 0) {
    // Beginning iteration now.
    LinkBase->Address.Register = HT_CAPABILITIES_POINTER;
    LibAmdPciReadBits (*LinkBase, 7, 0, &RegValue, StdHeader);
  } else {
    // Get next link offset.
    LibAmdPciReadBits (*LinkBase, 15, 8, &RegValue, StdHeader);
  }
  if (RegValue == 0) {
    // Are we at the end?  Check if we can move to another function.
    if (LinkBase->Address.Function == 0) {
      LinkBase->Address.Function = 4;
      LinkBase->Address.Register = HT_CAPABILITIES_POINTER;
      LibAmdPciReadBits (*LinkBase, 7, 0, &RegValue, StdHeader);
    }
  }

  if (RegValue != 0) {
    // Not at end, process the found link.
    LinkBase->Address.Register = RegValue;
    // Compute link number
    *Link = (((LinkBase->Address.Function == 4) ? 4 : 0) + ((LinkBase->Address.Register - 0x80) >> 5));

    // Handle pending link power off, check End of Chain, Xmit Off.
    PciAddress = *LinkBase;
    PciAddress.Address.Register = PciAddress.Address.Register + HT_LINK_CONTROL_REG_OFFSET;
    LibAmdPciReadBits (PciAddress, 7, 6, &RegValue, StdHeader);
    if (RegValue == 0) {
      // Check coherency (HTHOST_LINK_TYPE_REG = 0x18)
      PciAddress = *LinkBase;
      PciAddress.Address.Register = PciAddress.Address.Register + HT_LINK_TYPE_REG_OFFSET;
      LibAmdPciReadBits (PciAddress, 4, 0, &RegValue, StdHeader);
      if (RegValue  == 3) {
        HtHostFeats->HtHostFeatures.Coherent = 1;
      } else if (RegValue == 7) {
        HtHostFeats->HtHostFeatures.NonCoherent = 1;
      }
    }

    // If link was not connected, don't check other attributes, make sure
    // to return zero, no match.
    if ((HtHostFeats->HtHostFeatures.Coherent == 1) || (HtHostFeats->HtHostFeatures.NonCoherent == 1)) {
      // Check gen3
      PciAddress = *LinkBase;
      PciAddress.Address.Register = PciAddress.Address.Register + HT_LINK_EXTENDED_FREQ;
      LibAmdPciRead (AccessWidth32, PciAddress, &ExtendedFreq, StdHeader);
      PciAddress = *LinkBase;
      PciAddress.Address.Register = PciAddress.Address.Register + HT_LINK_FREQ_OFFSET;
      LibAmdPciRead (AccessWidth32, PciAddress, &RegValue, StdHeader);
      RegValue = (((ExtendedFreq & 0x1) << 4) | ((RegValue & 0x00000F00) >> 8));
      if (RegValue > 6) {
        HtHostFeats->HtHostFeatures.Ht3 = 1;
      } else {
        HtHostFeats->HtHostFeatures.Ht1 = 1;
      }
      // Check ganged. Must check the bit for sublink 0.
      LinkOffset = (*Link > 3) ? ((*Link - 4) * 4) : (*Link * 4);
      PciAddress = *LinkBase;
      PciAddress.Address.Function = 0;
      PciAddress.Address.Register = ((UINT32)LinkOffset + 0x170);
      LibAmdPciReadBits (PciAddress, 0, 0, &RegValue, StdHeader);
      if (RegValue == 0) {
        HtHostFeats->HtHostFeatures.UnGanged = 1;
      } else {
        if (*Link < 4) {
          HtHostFeats->HtHostFeatures.Ganged = 1;
        } else {
          // If this is a sublink 1 but it will be ganged, clear all features.
          HtHostFeats->HtHostValue = 0;
        }
      }
    }
  } else {
    // End of links.
    Result = FALSE;
  }
  return Result;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Checks to see if the HT phy register table entry should be applied
 *
 * @CpuServiceMethod{::F_NEXT_LINK_HAS_HTFPY_FEATS}.
 *
 * Find the next link which matches, if any.
 * This method will match for sublink 1 if the link is ganged and sublink 0 matches.
 *
 * @param[in]     FamilySpecificServices    The current Family Specific Services.
 * @param[in,out] HtHostCapability Initially the PCI bus, device, function=0, offset=0;
 *                                         Each call returns the HT Host Capability function and offset;
 *                                         Caller may use it to access registers, but must @b not modify it;
 *                                         Each new call passes the previous value as input.
 * @param[in,out] Link             Initially zero, each call returns the link number; caller passes it back unmodified each call.
 * @param[in]     HtPhyLinkType    Link type field from a register table entry to compare against
 * @param[out]    MatchedSublink1  TRUE: It is actually just sublink 1 that matches, FALSE: any other condition.
 * @param[out]    Frequency0       The frequency of sublink0 (200 MHz if not connected).
 * @param[out]    Frequency1       The frequency of sublink1 (200 MHz if not connected).
 * @param[in]     StdHeader        Standard Head Pointer
 *
 * @retval        TRUE             Link matches
 * @retval        FALSE            No more links
 *
 */
BOOLEAN
F10NextLinkHasHtPhyFeats (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN OUT   PCI_ADDR           *HtHostCapability,
  IN OUT   UINT32             *Link,
  IN       HT_PHY_LINK_FEATS  *HtPhyLinkType,
     OUT   BOOLEAN            *MatchedSublink1,
     OUT   HT_FREQUENCIES     *Frequency0,
     OUT   HT_FREQUENCIES     *Frequency1,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT32    RegValue;
  UINT32    ExtendedFreq;
  UINT32    InternalLinks;
  UINT32    Width;
  PCI_ADDR  PciAddress;
  PCI_ADDR  SubLink1Address;
  HT_PHY_LINK_FEATS LinkType;
  BOOLEAN   IsReallyCheckingBoth;
  BOOLEAN   IsFound;
  BOOLEAN   Result;

  ASSERT (*Link < 4);
  ASSERT (HtPhyLinkType != NULL);
  // error checks: No unknown link type bits set and not a "match none"
  ASSERT ((HtPhyLinkType->HtPhyLinkValue & ~(HTPHY_LINKTYPE_ALL | HTPHY_LINKTYPE_SL0_AND | HTPHY_LINKTYPE_SL1_AND)) == 0);
  ASSERT (HtPhyLinkType->HtPhyLinkValue != 0);

  Result = FALSE;
  IsFound = FALSE;
  while (!IsFound) {
    *Frequency0 = 0;
    *Frequency1 = 0;
    IsReallyCheckingBoth = FALSE;
    *MatchedSublink1 = FALSE;
    LinkType.HtPhyLinkValue = 0;

    // Find next link.
    PciAddress = *HtHostCapability;
    if (PciAddress.Address.Register == 0) {
      // Beginning iteration now.
      PciAddress.Address.Register = HT_CAPABILITIES_POINTER;
      LibAmdPciReadBits (PciAddress, 7, 0, &RegValue, StdHeader);
    } else {
      // Get next link offset.
      LibAmdPciReadBits (PciAddress, 15, 8, &RegValue, StdHeader);
    }
    if (RegValue != 0) {
      HtHostCapability->Address.Register = RegValue;
      // Compute link number of this sublink pair (so we don't need to account for function).
      *Link = ((HtHostCapability->Address.Register - 0x80) >> 5);

      // Set the link indicators.  This assumes each sublink set is contiguous, that is, links 3, 2, 1, 0 and 7, 6, 5, 4.
      LinkType.HtPhyLinkValue |= (HTPHY_LINKTYPE_SL0_LINK0 << *Link);
      LinkType.HtPhyLinkValue |= (HTPHY_LINKTYPE_SL1_LINK4 << *Link);

      // Read IntLnkRoute from the Link Initialization Status register.
      // (Note that this register field is not reserved prior to rev D, but should be zero.)
      PciAddress = *HtHostCapability;
      PciAddress.Address.Function = 0;
      PciAddress.Address.Register = 0x1A0;
      LibAmdPciReadBits (PciAddress, 23, 16, &InternalLinks, StdHeader);

      // if ganged, don't read sublink 1, but use sublink 0 to check.
      SubLink1Address = *HtHostCapability;

      // Check ganged. Since we got called for sublink 0, sublink 1 is implemented also,
      // but only access it if it is also unganged.
      PciAddress = *HtHostCapability;
      PciAddress.Address.Function = 0;
      PciAddress.Address.Register = ((*Link * 4) + 0x170);
      LibAmdPciRead (AccessWidth32, PciAddress, &RegValue, StdHeader);
      RegValue = (RegValue & 0x01);
      if (RegValue == 0) {
        // Then really read sublink1, rather than using sublink0
        SubLink1Address.Address.Function = 4;
        IsReallyCheckingBoth = TRUE;
      }

      // Checks for Sublink 0

      // Handle pending link power off, check End of Chain, Xmit Off.
      PciAddress = *HtHostCapability;
      PciAddress.Address.Register = PciAddress.Address.Register + HT_LINK_CONTROL_REG_OFFSET;
      LibAmdPciReadBits (PciAddress, 7, 6, &RegValue, StdHeader);
      if (RegValue == 0) {
        // Check coherency (HTHOST_LINK_TYPE_REG = 0x18)
        PciAddress = *HtHostCapability;
        PciAddress.Address.Register = PciAddress.Address.Register + HT_LINK_TYPE_REG_OFFSET;
        LibAmdPciRead (AccessWidth32, PciAddress, &RegValue, StdHeader);
        if ((RegValue & 0x1F) == 3) {
          LinkType.HtPhyLinkFeatures.HtPhySL0Coh = 1;
        } else if ((RegValue & 0x1F) == 7) {
          LinkType.HtPhyLinkFeatures.HtPhySL0NonCoh = 1;
        }
      }

      // If link was not connected, don't check other attributes, make sure
      // to return zero, no match. (Phy may be powered off.)
      if ((LinkType.HtPhyLinkFeatures.HtPhySL0Coh) || (LinkType.HtPhyLinkFeatures.HtPhySL0NonCoh)) {
        // Check gen3
        PciAddress = *HtHostCapability;
        PciAddress.Address.Register = PciAddress.Address.Register + HT_LINK_EXTENDED_FREQ;
        LibAmdPciRead (AccessWidth32, PciAddress, &ExtendedFreq, StdHeader);
        PciAddress = *HtHostCapability;
        PciAddress.Address.Register = PciAddress.Address.Register + HT_LINK_FREQ_OFFSET;
        LibAmdPciRead (AccessWidth32, PciAddress, &RegValue, StdHeader);
        RegValue = (((ExtendedFreq & 0x1) << 4) | ((RegValue & 0x00000F00) >> 8));
        *Frequency0 = RegValue;
        if (RegValue > 6) {
          LinkType.HtPhyLinkFeatures.HtPhySL0Ht3 = 1;
        } else {
          LinkType.HtPhyLinkFeatures.HtPhySL0Ht1 = 1;
        }
        // Check internal / external
        if ((InternalLinks & (1 << *Link)) == 0) {
          // External
          LinkType.HtPhyLinkFeatures.HtPhySL0External = 1;
        } else {
          // Internal
          LinkType.HtPhyLinkFeatures.HtPhySL0Internal = 1;
        }
      } else {
        LinkType.HtPhyLinkValue &= ~(HTPHY_LINKTYPE_SL0_ALL);
      }

      // Checks for Sublink 1
      // Handle pending link power off, check End of Chain, Xmit Off.
      PciAddress = SubLink1Address;
      PciAddress.Address.Register = PciAddress.Address.Register + HT_LINK_CONTROL_REG_OFFSET;
      LibAmdPciReadBits (PciAddress, 7, 6, &RegValue, StdHeader);
      LibAmdPciReadBits (PciAddress, 31, 24, &Width, StdHeader);
      if ((RegValue == 0) && (IsReallyCheckingBoth || (Width == 0x11))) {
        // Check coherency (HTHOST_LINK_TYPE_REG = 0x18)
        PciAddress = SubLink1Address;
        PciAddress.Address.Register = PciAddress.Address.Register + HT_LINK_TYPE_REG_OFFSET;
        LibAmdPciRead (AccessWidth32, PciAddress, &RegValue, StdHeader);
        if ((RegValue & 0x1F) == 3) {
          LinkType.HtPhyLinkFeatures.HtPhySL1Coh = 1;
        } else if ((RegValue & 0x1F) == 7) {
          LinkType.HtPhyLinkFeatures.HtPhySL1NonCoh = 1;
        }
      }

      if ((LinkType.HtPhyLinkFeatures.HtPhySL1Coh) || (LinkType.HtPhyLinkFeatures.HtPhySL1NonCoh)) {
        // Check gen3
        PciAddress = SubLink1Address;
        PciAddress.Address.Register = PciAddress.Address.Register + HT_LINK_EXTENDED_FREQ;
        LibAmdPciRead (AccessWidth32, PciAddress, &ExtendedFreq, StdHeader);
        PciAddress = SubLink1Address;
        PciAddress.Address.Register = PciAddress.Address.Register + HT_LINK_FREQ_OFFSET;
        LibAmdPciRead (AccessWidth32, PciAddress, &RegValue, StdHeader);
        RegValue = (((ExtendedFreq & 0x1) << 4) | ((RegValue & 0x00000F00) >> 8));
        *Frequency1 = RegValue;
        if (RegValue > 6) {
          LinkType.HtPhyLinkFeatures.HtPhySL1Ht3 = 1;
        } else {
          LinkType.HtPhyLinkFeatures.HtPhySL1Ht1 = 1;
        }
        // Check internal / external.  Note that we do really check sublink 1 regardless of ganging.
        if ((InternalLinks & (1 << (*Link + 4))) == 0) {
          // External
          LinkType.HtPhyLinkFeatures.HtPhySL1External = 1;
        } else {
          // Internal
          LinkType.HtPhyLinkFeatures.HtPhySL1Internal = 1;
        }
      } else {
        LinkType.HtPhyLinkValue &= ~(HTPHY_LINKTYPE_SL1_ALL);
      }

      // Determine if the link matches the entry criteria.
      // For Deemphasis checking, indicate whether it was actually sublink 1 that matched.
      // If the link is ganged or only sublink 0 matched, or the link features didn't match, this is false.
      if (((HtPhyLinkType->HtPhyLinkValue & HTPHY_LINKTYPE_SL0_AND) == 0) &&
          ((HtPhyLinkType->HtPhyLinkValue & HTPHY_LINKTYPE_SL1_AND) == 0)) {
        // Match if any feature matches (OR)
        Result = (BOOLEAN) ((LinkType.HtPhyLinkValue & HtPhyLinkType->HtPhyLinkValue) != 0);
      } else {
        // Match if all features match (AND)
        Result = (BOOLEAN) ((HtPhyLinkType->HtPhyLinkValue & ~(HTPHY_LINKTYPE_SL0_AND | HTPHY_LINKTYPE_SL1_AND)) ==
                           (LinkType.HtPhyLinkValue & HtPhyLinkType->HtPhyLinkValue));
      }
      if (Result) {
        if (IsReallyCheckingBoth &&
            (((LinkType.HtPhyLinkValue & HtPhyLinkType->HtPhyLinkValue) & (HTPHY_LINKTYPE_SL1_ALL)) != 0)) {
          *MatchedSublink1 = TRUE;
        }
        IsFound = TRUE;
      } else {
        // Go to next link
      }
    } else {
      // No more links
      IsFound = TRUE;
    }
  }
  return Result;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Applies an HT Phy read-modify-write based on an HT Phy register table entry.
 *
 * @CpuServiceMethod{::F_SET_HT_PHY_REGISTER}.
 *
 * This function performs the necessary sequence of PCI reads, writes, and waits
 * necessary to program an HT Phy register.
 *
 * @param[in]  FamilySpecificServices    The current Family Specific Services.
 * @param[in]  HtPhyEntry    HT Phy register table entry to apply
 * @param[in]  CapabilitySet The link's HT Host base address.
 * @param[in]  Link          Zero based, node, link number (not package link).
 * @param[in]  StdHeader     Config handle for library and services
 *
 */
VOID
F10SetHtPhyRegister (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       HT_PHY_TYPE_ENTRY_DATA  *HtPhyEntry,
  IN       PCI_ADDR                 CapabilitySet,
  IN       UINT32                   Link,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  )
{
  UINT32    Temp;
  UINT32    PhyReg;
  PCI_ADDR  PhyBase;

  // Determine the PCI config address of the HT Phy portal
  PhyBase = CapabilitySet;
  PhyBase.Address.Function = FUNC_4;
  PhyBase.Address.Register = ((Link << 3) + REG_HT4_PHY_OFFSET_BASE_4X180);

  LibAmdPciRead (AccessWidth32, PhyBase, &PhyReg, StdHeader);

  // Handle direct map registers if needed
  PhyReg &= ~(HTPHY_DIRECT_OFFSET_MASK);
  if (HtPhyEntry->Address > 0x1FF) {
    PhyReg |= HTPHY_DIRECT_MAP;
  }

  PhyReg |= (HtPhyEntry->Address);
  // Ask the portal to read the HT Phy Register contents
  LibAmdPciWrite (AccessWidth32, PhyBase, &PhyReg, StdHeader);
  do
  {
    LibAmdPciRead (AccessWidth32, PhyBase, &Temp, StdHeader);
  } while (!(Temp & HTPHY_IS_COMPLETE_MASK));

  // Get the current register contents and do the update requested by the table
  PhyBase.AddressValue += 4;
  LibAmdPciRead (AccessWidth32, PhyBase, &Temp, StdHeader);
  Temp &= ~(HtPhyEntry->Mask);
  Temp |= (HtPhyEntry->Data);
  LibAmdPciWrite (AccessWidth32, PhyBase, &Temp, StdHeader);

  PhyBase.AddressValue -= 4;
  // Ask the portal to write our updated value to the HT Phy
  PhyReg |= HTPHY_WRITE_CMD;
  LibAmdPciWrite (AccessWidth32, PhyBase, &PhyReg, StdHeader);
  do
  {
    LibAmdPciRead (AccessWidth32, PhyBase, &Temp, StdHeader);
  } while (!(Temp & HTPHY_IS_COMPLETE_MASK));
}

/*---------------------------------------------------------------------------------------*/
/**
 * Returns the number of core performance boost states.
 *
 * This function will return the number of boosted states present on
 * the executing core.  This is useful when trying to determine the
 * "software P0" state.
 *
 * @param[in]  StdHeader     Config handle for library and services
 *
 * @return      The number of boosted core P-states.
 *
 */
UINT8
F10GetNumberOfBoostedPstatesOnCore (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8        NumBoostStates;
  UINT32       Socket;
  UINT32       Module;
  UINT32       Ignored;
  PCI_ADDR     PciAddress;
  UINT32       LocalPciRegister;
  CPUID_DATA   CpuidData;
  AGESA_STATUS IgnoredSts;

  LibAmdCpuidRead (AMD_CPUID_APM, &CpuidData, StdHeader);
  if (((CpuidData.EDX_Reg & 0x00000200) >> 9) == 1) {
    IdentifyCore (StdHeader, &Socket, &Module, &Ignored, &IgnoredSts);
    GetPciAddress (StdHeader, Socket, Module, &PciAddress, &IgnoredSts);
    PciAddress.Address.Function = FUNC_4;
    PciAddress.Address.Register = CPB_CTRL_REG;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    NumBoostStates = (UINT8) (((CPB_CTRL_REGISTER *) &LocalPciRegister)->NumBoostStates);
  } else {
    NumBoostStates = 0;
  }

  return NumBoostStates;
}


