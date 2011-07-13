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
 * @e \$Revision: 44323 $   @e \$Date: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
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
#include "amdlib.h"
#include "Ids.h"
#include "cpuRegisters.h"
#include "cpuFamilyTranslation.h"
#include "cpuF10PowerMgmt.h"
#include "cpuApicUtilities.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "cpuF10Utilities.h"
#include "cpuPostInit.h"
#include "Filecode.h"
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
AGESA_STATUS
F10GetNbFrequency (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       PCI_ADDR *PciAddress,
     OUT   UINT32 *FrequencyInMHz,
     OUT   UINT32 *VoltageInuV,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );
VOID
F10GetHtLinkFeatures (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  OUT      UINTN                  *Link,
  IN       PCI_ADDR               *LinkBase,
  OUT      HT_HOST_FEATS          *HtHostFeats,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );
BOOLEAN
F10DoesLinkHaveHtPhyFeats (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       PCI_ADDR           CapabilitySet,
  IN       UINT32             Link,
  IN       HT_PHY_LINK_FEATS  *HtPhyLinkType,
  OUT      BOOLEAN            *MatchedSublink1,
  OUT      HT_FREQUENCIES     *Frequency0,
  OUT      HT_FREQUENCIES     *Frequency1,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );
VOID
F10SetHtPhyRegister (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       HT_PHY_TYPE_ENTRY_DATA  *HtPhyEntry,
  IN       PCI_ADDR                 CapabilitySet,
  IN       UINT32                   Link,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  );
VOID
F10SetRegisterForHtLinkTokenEntry (
  IN       TABLE_ENTRY_DATA       *Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

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
  UINT32    PciRegister;
  UINT32    Socket;
  UINT32    Module;
  UINT32    Ignored;
  UINT64    MsrRegister;
  PCI_ADDR  PciAddress;
  AGESA_STATUS IgnoredSts;

  IdentifyCore (StdHeader, &Socket, &Module, &Ignored, &IgnoredSts);
  GetPciAddress (StdHeader, Socket, Module, &PciAddress, &IgnoredSts);

  PciAddress.Address.Function = FUNC_3;
  PciAddress.Address.Register = PW_CTL_MISC_REG;
  LibAmdPciRead (AccessWidth32, PciAddress, &PciRegister, StdHeader);
  if (((POWER_CTRL_MISC_REGISTER *) &PciRegister)->SlamVidMode == 1) {
    LibAmdMsrRead (MSR_COFVID_CTL, &MsrRegister, StdHeader);
    ((COFVID_CTRL_MSR *) &MsrRegister)->CpuVid = VidCode;
    LibAmdMsrWrite (MSR_COFVID_CTL, &MsrRegister, StdHeader);
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
  UINT32    PciRegister;
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
  LibAmdPciRead (AccessWidth32, PciAddress, &PciRegister, StdHeader);

  if (SlamMode) {
    RegisterEncoding = (UINT8) ((CLK_PWR_TIMING_CTRL1_REGISTER *) &PciRegister)->VSSlamTime;
  } else {
    RegisterEncoding = (UINT8) ((CLK_PWR_TIMING_CTRL1_REGISTER *) &PciRegister)->VSRampTime;
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
  UINT64 MsrRegister;

  if (((SW_VOLT_TRANS_NB *) InputData)->SlamMode) {
    VidCode = ((SW_VOLT_TRANS_NB *) InputData)->VidCode;
  } else {
    LibAmdMsrRead (MSR_COFVID_STS, &MsrRegister, StdHeader);
    VidCode = (UINT32) (((COFVID_STS_MSR *) &MsrRegister)->CurNbVid);
    if (VidCode > ((SW_VOLT_TRANS_NB *) InputData)->VidCode) {
      --VidCode;
    } else if (VidCode < ((SW_VOLT_TRANS_NB *) InputData)->VidCode) {
      ++VidCode;
    }
  }
  LibAmdMsrRead (MSR_COFVID_CTL, &MsrRegister, StdHeader);
  ((COFVID_CTRL_MSR *) &MsrRegister)->NbVid = VidCode;
  LibAmdMsrWrite (MSR_COFVID_CTL, &MsrRegister, StdHeader);

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
  UINT32   PciRegister;
  UINT64   MsrRegister;
  BOOLEAN  IsPviMode;
  PCI_ADDR LocalPciAddress;

  // Get F3xA0[PviMode]
  LocalPciAddress.AddressValue = PciAddress->AddressValue;
  LocalPciAddress.Address.Function = FUNC_3;
  LocalPciAddress.Address.Register = PW_CTL_MISC_REG;
  LibAmdPciRead (AccessWidth32, LocalPciAddress, &PciRegister, StdHeader);
  if (((POWER_CTRL_MISC_REGISTER *) &PciRegister)->PviMode == 1) {
    IsPviMode = TRUE;
  } else {
    IsPviMode = FALSE;
  }

  // Get P0's voltage
  LibAmdMsrRead (PS_REG_BASE, &MsrRegister, StdHeader);
  P0VidCode = (UINT8) (((PSTATE_MSR *) &MsrRegister)->CpuVid);

  // If SVI, we only care about CPU VID.
  // If PVI, determine the higher voltage between NB and CPU
  if (IsPviMode) {
    NbVid = (UINT8) (((PSTATE_MSR *) &MsrRegister)->NbVid);
    if (P0VidCode > NbVid) {
      P0VidCode = NbVid;
    }
  }

  // Get Pmin's index
  LibAmdMsrRead (MSR_PSTATE_CURRENT_LIMIT, &MsrRegister, StdHeader);
  MsrAddr = (UINT32) ((((PSTATE_CURLIM_MSR *) &MsrRegister)->PstateMaxVal) + PS_REG_BASE);

  // Get Pmin's VID
  LibAmdMsrRead (MsrAddr, &MsrRegister, StdHeader);
  PminVidCode = (UINT8) (((PSTATE_MSR *) &MsrRegister)->CpuVid);

  // If SVI, we only care about CPU VID.
  // If PVI, determine the higher voltage b/t NB and CPU
  if (IsPviMode) {
    NbVid = (UINT8) (((PSTATE_MSR *) &MsrRegister)->NbVid);
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
  ASSERT (CpuEarlyParams->PlatformConfig.VrmProperties.SlewRate != 0);

  // Calculate Slam Time
  //   VSSlamTime = 0.4us/mV (or 0.2us/mV) * Vhigh - Vlow
  //   In our case, we will scale the values by 100 to avoid
  //   decimals.

  VoltageDifference = (UINT32) ((LowVoltageVid - HighVoltageVid) * 12500);
  SlamTime = (VoltageDifference / CpuEarlyParams->PlatformConfig.VrmProperties.SlewRate) + CpuEarlyParams->PlatformConfig.VrmProperties.AdditionalDelay;
  if (VoltageDifference % CpuEarlyParams->PlatformConfig.VrmProperties.SlewRate) {
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
 *  Calculates the power in milliWatts of the desired P-state.
 *
 *  @CpuServiceMethod{::F_CPU_GET_PSTATE_POWER}.
 *
 *  @param[in]   FamilySpecificServices   The current Family Specific Services.
 *  @param[in]   StateNumber              Which P-state to analyze
 *  @param[out]  PowerInMw                The Power in milliWatts of that P-State
 *  @param[in]   StdHeader                Header for library and services
 *
 *  @retval      AGESA_SUCCESS Always succeeds.
 */
AGESA_STATUS
F10GetPstatePower (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       UINT8 StateNumber,
     OUT   UINT32 *PowerInMw,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32  CpuVid;
  UINT32  IddValue;
  UINT32  IddDiv;
  UINT32  Socket;
  UINT32  Module;
  UINT32  Ignored;
  BOOLEAN PviFlag;
  UINT32  V_x10000;
  UINT32  Power;
  PCI_ADDR PciAddress;
  UINT32  TempVar_a;
  UINT64  MsrRegister;
  AGESA_STATUS IgnoredSts;

  ASSERT (StateNumber < NM_PS_REG);
  LibAmdMsrRead (PS_REG_BASE + (UINT32) StateNumber, &MsrRegister, StdHeader);
  ASSERT (((PSTATE_MSR *) &MsrRegister)->PsEnable == 1);
  CpuVid = (UINT32) (((PSTATE_MSR *) &MsrRegister)->CpuVid);
  IddValue = (UINT32) (((PSTATE_MSR *) &MsrRegister)->IddValue);
  IddDiv = (UINT32) (((PSTATE_MSR *) &MsrRegister)->IddDiv);

  IdentifyCore (StdHeader, &Socket, &Module, &Ignored, &IgnoredSts);
  GetPciAddress (StdHeader, Socket, Module, &PciAddress, &IgnoredSts);

  PciAddress.Address.Function = FUNC_3;
  PciAddress.Address.Register = POWER_CTRL_MISCELLANEOUS_REG;
  LibAmdPciRead (AccessWidth32, PciAddress, &TempVar_a, StdHeader);
  if ((TempVar_a & 0x00000100) != 0) {
    PviFlag = TRUE;
  } else {
    PviFlag = FALSE;
  }
  if (PviFlag) {
    // Set CpuVid value in case CPU is in PVI mode
    if (CpuVid > 0x5D) {
      CpuVid = 0x3F;
    } else if (CpuVid > 0x3E) {
      CpuVid = CpuVid - 0x1F;
    } else {
      CpuVid = (CpuVid >> 1);
    }

    // PVI Encoding
    if (CpuVid >= 0x20) {
      V_x10000 = 7625L - (125L * (CpuVid - 0x20));
    } else {
      V_x10000 = 15500L - (250L * CpuVid);
    }
  } else {
    if (CpuVid >= 0x7C) {
      V_x10000 = 0;
    } else {
      V_x10000 = 15500L - (125L * CpuVid);
    }
  }

  Power = V_x10000 * IddValue;

  switch (IddDiv) {
  case 0:
    *PowerInMw = Power / 10L;
    break;
  case 1:
    *PowerInMw = Power / 100L;
    break;
  case 2:
    *PowerInMw = Power / 1000L;
    break;
  default:
    // IddDiv is set to an undefined value.  This is due to either a misfused CPU, or
    // an invalid P-state MSR write.
    ASSERT (FALSE);
    *PowerInMw = 0;
    break;
  }
  return (AGESA_SUCCESS);
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Calculates the frequency in megahertz of the desired P-state.
 *
 *  @CpuServiceMethod{::F_CPU_GET_PSTATE_FREQ}.
 *
 *  @param[in]   FamilySpecificServices   The current Family Specific Services.
 *  @param[in]   StateNumber              The P-State to analyze.
 *  @param[out]  FrequencyInMHz           The P-State's frequency in MegaHertz
 *  @param[in]   StdHeader                Header for library and services
 *
 *  @retval      AGESA_SUCCESS   Always Succeeds.
 */
AGESA_STATUS
F10GetPstateFrequency (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       UINT8 StateNumber,
     OUT   UINT32 *FrequencyInMHz,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8 TempValue;
  UINT32 CpuDid;
  UINT32 CpuFid;
  UINT64 MsrRegister;

  ASSERT (StateNumber < NM_PS_REG);
  LibAmdMsrRead (PS_REG_BASE + (UINT32) StateNumber, &MsrRegister, StdHeader);
  ASSERT (((PSTATE_MSR *) &MsrRegister)->PsEnable == 1);
  CpuDid = (UINT32) (((PSTATE_MSR *) &MsrRegister)->CpuDid);
  CpuFid = (UINT32) (((PSTATE_MSR *) &MsrRegister)->CpuFid);

  switch (CpuDid) {
  case 0:
    TempValue = 1;
    break;
  case 1:
    TempValue = 2;
    break;
  case 2:
    TempValue = 4;
    break;
  case 3:
    TempValue = 8;
    break;
  case 4:
    TempValue  =  16;
    break;
  default:
    // CpuDid is set to an undefined value.  This is due to either a misfused CPU, or
    // an invalid P-state MSR write.
    ASSERT (FALSE);
    TempValue = 1;
    break;
  }
  *FrequencyInMHz = (100 * (CpuFid + 0x10) / TempValue);
  return (AGESA_SUCCESS);
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
  UINT64 MsrRegister;

  ASSERT (StateNumber < NM_PS_REG);
  LibAmdMsrRead (PS_REG_BASE + (UINT32) StateNumber, &MsrRegister, StdHeader);
  ((PSTATE_MSR *) &MsrRegister)->PsEnable = 0;
  LibAmdMsrWrite (PS_REG_BASE + (UINT32) StateNumber, &MsrRegister, StdHeader);
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
  UINT64 MsrRegister;

  ASSERT (StateNumber < NM_PS_REG);
  LibAmdMsrRead (PS_REG_BASE + (UINT32) StateNumber, &MsrRegister, StdHeader);
  ASSERT (((PSTATE_MSR *) &MsrRegister)->PsEnable == 1);
  LibAmdMsrRead (MSR_PSTATE_CTL, &MsrRegister, StdHeader);
  ((PSTATE_CTRL_MSR *) &MsrRegister)->PstateCmd = (UINT64) StateNumber;
  LibAmdMsrWrite (MSR_PSTATE_CTL, &MsrRegister, StdHeader);
  if (WaitForTransition) {
    do {
      LibAmdMsrRead (MSR_PSTATE_STS, &MsrRegister, StdHeader);
    } while (((PSTATE_STS_MSR *) &MsrRegister)->CurPstate != (UINT64) StateNumber);
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
  UINT32 Socket;
  UINT32 Module;
  UINT32 Ignored;
  UINT64 MsrRegister;
  PCI_ADDR PciAddress;
  AGESA_STATUS IgnoredSts;

  LibAmdMsrRead (0xC0010015, &MsrRegister, StdHeader);
  if ((MsrRegister & 0x01000000) != 0) {
    return (FamilySpecificServices->GetPstateFrequency (FamilySpecificServices, 0, FrequencyInMHz, StdHeader));
  } else {
    IdentifyCore (StdHeader, &Socket, &Module, &Ignored, &IgnoredSts);
    GetPciAddress (StdHeader, Socket, Module, &PciAddress, &IgnoredSts);
    return (FamilySpecificServices->GetNbFrequency (FamilySpecificServices, &PciAddress, FrequencyInMHz, &Ignored, StdHeader));
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Determines the NB clock on the desired node.
 *
 *  @CpuServiceMethod{::F_CPU_GET_NB_FREQ}.
 *
 *  @param[in]   FamilySpecificServices   The current Family Specific Services.
 *  @param[in]   PciAddress               The northbridge to query
 *  @param[out]  FrequencyInMHz           Northbridge clock frequency in MHz.
 *  @param[out]  VoltageInuV              Northbridge voltage in uV.
 *  @param[in]   StdHeader                Header for library and services
 *
 *  @retval      AGESA_UNSUPPORTED   Unknown revs of F10 will return unsupported.
 */
AGESA_STATUS
F10GetNbFrequency (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       PCI_ADDR *PciAddress,
     OUT   UINT32 *FrequencyInMHz,
     OUT   UINT32 *VoltageInuV,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  // This was called by an unknown rev of F10 CPU.
  return (AGESA_UNSUPPORTED);
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
  UINT32    PciRegister;
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
    LibAmdPciRead (AccessWidth32, PciAddress, &PciRegister, StdHeader);
    if ((PciRegister & HT_INIT_CTRL_REQ_DIS) != 0) {
      PciRegister &= ~HT_INIT_CTRL_REQ_DIS;
      LibAmdPciWrite (AccessWidth32, PciAddress, &PciRegister, StdHeader);
      LaunchFlag = TRUE;
    } else {
      LaunchFlag = FALSE;
    }
    break;

  case 1:
    PciAddress.Address.Register = HT_TRANS_CTRL;
    LibAmdPciRead (AccessWidth32, PciAddress, &PciRegister, StdHeader);
    if ((PciRegister & HT_TRANS_CTRL_CPU1_EN) == 0) {
      PciRegister |= HT_TRANS_CTRL_CPU1_EN;
      LibAmdPciWrite (AccessWidth32, PciAddress, &PciRegister, StdHeader);
      LaunchFlag = TRUE;
    } else {
      LaunchFlag = FALSE;
    }
    break;

  case 2:
    PciAddress.Address.Register = ECS_HT_TRANS_CTRL;
    LibAmdPciRead (AccessWidth32, PciAddress, &PciRegister, StdHeader);

    if ((PciRegister & ECS_HT_TRANS_CTRL_CPU2_EN) == 0) {
      PciRegister |= ECS_HT_TRANS_CTRL_CPU2_EN;
      LibAmdPciWrite (AccessWidth32, PciAddress, &PciRegister,
                      StdHeader);
      LaunchFlag = TRUE;
    } else {
      LaunchFlag = FALSE;
    }
    break;

  case 3:
    PciAddress.Address.Register = ECS_HT_TRANS_CTRL;
    LibAmdPciRead (AccessWidth32, PciAddress, &PciRegister, StdHeader);
    if ((PciRegister & ECS_HT_TRANS_CTRL_CPU3_EN) == 0) {
      PciRegister |= ECS_HT_TRANS_CTRL_CPU3_EN;
      LibAmdPciWrite (AccessWidth32, PciAddress, &PciRegister, StdHeader);
      LaunchFlag = TRUE;
    } else {
      LaunchFlag = FALSE;
    }
    break;

  case 4:
    PciAddress.Address.Register = ECS_HT_TRANS_CTRL;
    LibAmdPciRead (AccessWidth32, PciAddress, &PciRegister, StdHeader);
    if ((PciRegister & ECS_HT_TRANS_CTRL_CPU4_EN) == 0) {
      PciRegister |= ECS_HT_TRANS_CTRL_CPU4_EN;
      LibAmdPciWrite (AccessWidth32, PciAddress, &PciRegister, StdHeader);
      LaunchFlag = TRUE;
    } else {
      LaunchFlag = FALSE;
    }
    break;

  case 5:
    PciAddress.Address.Register = ECS_HT_TRANS_CTRL;
    LibAmdPciRead (AccessWidth32, PciAddress, &PciRegister, StdHeader);
    if ((PciRegister & ECS_HT_TRANS_CTRL_CPU5_EN) == 0) {
      PciRegister |= ECS_HT_TRANS_CTRL_CPU5_EN;
      LibAmdPciWrite (AccessWidth32, PciAddress, &PciRegister, StdHeader);
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
 *    This function will return the CpuFid and CpuDid in MHz, using the formula
 *    described in the BKDG MSRC001_00[68:64] P-State [4:0] Registers:bit 8:0
 *
 *    @param[in]       FamilySpecificServices   The current Family Specific Services.
 *    @param[in]       PStateNumber             P-state number to check.
 *    @param[in]       Frequency                Leveled target frequency for PStateNumber.
 *    @param[out]      *CpuFidPtr               New leveled FID.
 *    @param[out]      *CpuDidPtr1              New leveled DID info 1.
 *    @param[out]      *CpuDidPtr2              New leveled DID info 2.
 *    @param[in]       *StdHeader               Header for library and services.
 *
 *    @retval       AGESA_WARNING           This P-State does not need to be modified.
 *    @retval       AGESA_SUCCESS           This P-State must be modified to be level.
 */
AGESA_STATUS
F10GetFrequencyXlatRegInfo (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       UINT8                  PStateNumber,
  IN       UINT32                 Frequency,
     OUT   UINT32                 *CpuFidPtr,
     OUT   UINT32                 *CpuDidPtr1,
     OUT   UINT32                 *CpuDidPtr2,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32 i;
  UINT32 j;
  AGESA_STATUS Status;
  UINT32 FrequencyInMHz;

  FrequencyInMHz = 0;
  *CpuDidPtr2 = 0xFFFF;

  Status = AGESA_SUCCESS;

  FamilySpecificServices->GetPstateFrequency (FamilySpecificServices, PStateNumber, &FrequencyInMHz, StdHeader);
  if (FrequencyInMHz == Frequency) {
    Status |= AGESA_WARNING;
  }

  // CPU Frequency = 100 MHz * (CpuFid + 10h) / (2^CpuDid)
  // In this for loop i = 2^CpuDid


  for (i = 1; i < 17; (i += i)) {
    for (j = 0; j < 64; j++) {
      if (Frequency == ((100 * (j + 0x10)) / i )) {
        *CpuFidPtr = j;
        if (i == 1) {
          *CpuDidPtr1 = 0;
        } else if (i == 2) {
          *CpuDidPtr1 = 1;
        } else if (i == 4) {
          *CpuDidPtr1 = 2;
        } else if (i == 8) {
          *CpuDidPtr1 = 3;
        } else if (i == 16) {
          *CpuDidPtr1 = 4;
        } else {
          *CpuFidPtr = 0xFFFF;
          *CpuDidPtr1 = 0xFFFF;
        }
        // Success
        return Status;
      }
    }
  }

  // Error Condition
  *CpuFidPtr = 0x00FF;
  *CpuDidPtr1 = 0x00FF;
  *CpuDidPtr2 = 0x00FF;

  return AGESA_ERROR;
}

/*---------------------------------------------------------------------------------------*/
/**
 * This function sets the Pstate MSR to each APs base on Pstate Buffer.
 *
 * @CpuServiceMethod{::F_CPU_SET_PSTATE_LEVELING_REG}.
 *
 * This function should be called for every core in the system.
 *
 * @param[in]       FamilySpecificServices   The current Family Specific Services.
 * @param[in]       CpuAmdPState             Gathered P-state data structure for whole system.
 * @param[in]       StdHeader                Config for library and services.
 *
 * @retval          AGESA_STATUS    @todo document return *values*.
 *
 */
AGESA_STATUS
F10PstateLevelingCoreMsrModify (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       S_CPU_AMD_PSTATE   *CpuAmdPState,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT32             i;
  UINT32             Ignored;
  UINT32             k;
  UINT32             TempVar_d;
  UINT32             TempVar_e;
  UINT32             TempVar_f;
  UINT64             MsrValue;
  AGESA_STATUS       Status;
  UINT32             Socket;
  UINT32             Module;
  UINT32             Core;
  PSTATE_LEVELING    *PStateBufferPtr;
  PSTATE_LEVELING    *PStateBufferPtrTmp;
  S_CPU_AMD_PSTATE   *CpuAmdPstatePtr;
  UINT32             LogicalSocketCount;
  PCI_ADDR           PciAddress;
  UINT32             PciRegister;

  Ignored = 0;
  CpuAmdPstatePtr = (S_CPU_AMD_PSTATE *) CpuAmdPState;
  PStateBufferPtrTmp = CpuAmdPstatePtr->PStateLevelingStruc;
  PStateBufferPtr = CpuAmdPstatePtr->PStateLevelingStruc;
  LogicalSocketCount = CpuAmdPstatePtr->TotalSocketInSystem;
  PciAddress.AddressValue = 0;

  //
  //Try to find the Pstate buffer specific to this core(socket).
  //
  IdentifyCore (StdHeader, &Socket, &Module, &Core, &Status);
  for (i = 0; i < LogicalSocketCount; i++) {
    CpuGetPStateLevelStructure (&PStateBufferPtrTmp, CpuAmdPstatePtr, i, StdHeader);
    if (PStateBufferPtrTmp->SocketNumber == Socket) {
      break;
    }
  }

  if (PStateBufferPtr[0].OnlyOneEnabledPState) {
    //
    //If all processors have only 1 enabled P-state, the following sequence should be performed on all cores:
    //

    //1. Write the appropriate CpuFid value resulting from the matched CPU COF to MSRC001_0064[CpuFid].
    LibAmdMsrRead (MSR_PSTATE_0, &MsrValue, StdHeader);
    Status = F10GetFrequencyXlatRegInfo (FamilySpecificServices, 0, PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[0].CoreFreq, &TempVar_d, &TempVar_e, &Ignored, StdHeader);
    // Bits 5:0
    ((PSTATE_MSR *) &MsrValue)->CpuFid = TempVar_d;
    // Bits 8:6
    ((PSTATE_MSR *) &MsrValue)->CpuDid = TempVar_e;
    // Bits 39:32
    ((PSTATE_MSR *) &MsrValue)->IddValue = PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[0].IddValue;
    // Bits 41:40
    ((PSTATE_MSR *) &MsrValue)->IddDiv = PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[0].IddDiv;
    // Enable the P-State
    ((PSTATE_MSR *) &MsrValue)->PsEnable = 1;
    LibAmdMsrWrite (MSR_PSTATE_0, &MsrValue, StdHeader);

    //2. Copy MSRC001_0064 to MSRC001_0065.
    LibAmdMsrWrite (MSR_PSTATE_1, &MsrValue, StdHeader);

    //3. Write 001b to F3xDC[PstatemaxVal].
    GetPciAddress (StdHeader, Socket, Module, &PciAddress, &Status);
    PciAddress.Address.Register = CPTC2_REG;
    PciAddress.Address.Function = FUNC_3;
    LibAmdPciRead (AccessWidth32, PciAddress, &PciRegister, StdHeader);
    ((CLK_PWR_TIMING_CTRL2_REGISTER *) &PciRegister)->PstateMaxVal = 1;
    LibAmdPciWrite (AccessWidth32, PciAddress, &PciRegister, StdHeader);

    //4. Write 001b to MSRC001_0062[PstateCmd].
    FamilySpecificServices->TransitionPstate (FamilySpecificServices, (UINT8) 1, (BOOLEAN) FALSE, StdHeader);

    //5. Wait for MSRC001_0071[CurCpuFid] = MSRC001_0065[CpuFid].
    do {
      LibAmdMsrRead (MSR_COFVID_STS, &MsrValue, StdHeader);
    } while (((COFVID_STS_MSR *) &MsrValue)->CurCpuFid != TempVar_d);

    //6. Write 000b to MSRC001_0062[PstateCmd].
    FamilySpecificServices->TransitionPstate (FamilySpecificServices, (UINT8) 0, (BOOLEAN) FALSE, StdHeader);

    //7. Wait for MSRC001_0071[CurCpuFid] = MSRC001_0064[CpuFid].
    do {
      LibAmdMsrRead (MSR_COFVID_STS, &MsrValue, StdHeader);
    } while (((COFVID_STS_MSR *) &MsrValue)->CurCpuFid != TempVar_d);

    //8. Write 0b to MSRC001_0065[PstateEn].
    LibAmdMsrRead (MSR_PSTATE_1, &MsrValue, StdHeader);
    ((PSTATE_MSR *) &MsrValue)->PsEnable = 0;
    LibAmdMsrWrite (MSR_PSTATE_1, &MsrValue, StdHeader);

    //9. Write 000b to F3xDC[PstateMaxVal] and exit the sequence (no further steps are required).
    LibAmdPciRead (AccessWidth32, PciAddress, &PciRegister, StdHeader);
    ((CLK_PWR_TIMING_CTRL2_REGISTER *) &PciRegister)->PstateMaxVal = 0;
    LibAmdPciWrite (AccessWidth32, PciAddress, &PciRegister, StdHeader);

  } else {
    TempVar_f = MSR_PSTATE_0;

    for (k = 0; k <= PStateBufferPtrTmp->PStateCoreStruct[0].PStateMaxValue; k++, TempVar_f++) {
      // If pState is not disabled then do update
      LibAmdMsrRead (TempVar_f, &MsrValue, StdHeader);

      if (PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[k].PStateEnable == 1) {
        Status = F10GetFrequencyXlatRegInfo (FamilySpecificServices, (UINT8) k, PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[k].CoreFreq, &TempVar_d, &TempVar_e, &Ignored, StdHeader);
        if (Status != AGESA_ERROR) {
          // Bits 5:0
          ((PSTATE_MSR *) &MsrValue)->CpuFid = TempVar_d;
          // Bits 8:6
          ((PSTATE_MSR *) &MsrValue)->CpuDid = TempVar_e;
        }

        // Bits 39:32
        ((PSTATE_MSR *) &MsrValue)->IddValue = PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[k].IddValue;
        // Bits 41:40
        ((PSTATE_MSR *) &MsrValue)->IddDiv = PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[k].IddDiv;
        // Enable the P-State
        ((PSTATE_MSR *) &MsrValue)->PsEnable = 1;
        LibAmdMsrWrite (TempVar_f, &MsrValue, StdHeader);
      } else {
        // Disable the P-State
        ((PSTATE_MSR *) &MsrValue)->PsEnable = 0;
        LibAmdMsrWrite (TempVar_f, &MsrValue, StdHeader);
      }
    }
  }
  return AGESA_SUCCESS;
}

/**
 *---------------------------------------------------------------------------------------
 *
 *  F10GetPowerStepValueInTime
 *
 *  Description:
 *    Convert power step value in time
 *
 *  Parameters:
 *    @param[out]     *PowerStepPtr
 *
 *    @retval       VOID
 *
 *---------------------------------------------------------------------------------------
 **/
STATIC VOID
F10GetPowerStepValueInTime (
  IN OUT   UINT32  *PowerStepPtr
  )
{
  UINT32 TempVar_a;

  TempVar_a = *PowerStepPtr;

  if (TempVar_a < 0x4) {
    *PowerStepPtr = 400 - (TempVar_a * 100);
  } else if (TempVar_a <  0x9) {
    *PowerStepPtr = 130 - (TempVar_a * 10);
  } else {
    *PowerStepPtr = 90 - (TempVar_a * 5);
  }
}


/**
 *---------------------------------------------------------------------------------------
 *
 *  F10GetPllValueInTime
 *
 *  Description:
 *    Convert PLL Value in time
 *
 *  Parameters:
 *    @param[out]     *PllLockTimePtr
 *
 *    @retval       VOID
 *
 *---------------------------------------------------------------------------------------
 **/
STATIC VOID
F10GetPllValueInTime (
  IN OUT   UINT32  *PllLockTimePtr
  )
{
  if (*PllLockTimePtr < 4) {
    *PllLockTimePtr = *PllLockTimePtr + 1;
  } else if (*PllLockTimePtr == 4) {
    *PllLockTimePtr = 8;
  } else if (*PllLockTimePtr == 5) {
    *PllLockTimePtr = 16;
  } else
    *PllLockTimePtr = 0;
}
/*---------------------------------------------------------------------------------------*/
/**
 *  Get Pstate Transition Latency.
 *
 *  @CpuServiceMethod{::F_CPU_PSTATE_TRANSITION_LATENCY}.
 *
 *  Calculate TransitionLatency by power step value and pll value.
 *
 *  @param[in]     FamilySpecificServices            The current Family Specific Services.
 *  @param[in]     PStateLevelingBufferStructPtr     Pstate row data buffer pointer
 *  @param[in]     PciAddress                        Pci address
 *  @param[out]    TransitionLatency                 The transition latency.
 *  @param[in]     StdHeader                         Header for library and services
 *
 *  @retval      AGESA_SUCCESS Always succeeds.
 */
AGESA_STATUS
F10GetPstateTransLatency (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       PSTATE_LEVELING        *PStateLevelingBufferStructPtr,
  IN       PCI_ADDR               *PciAddress,
     OUT   UINT32                 *TransitionLatency,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32               TempVar_b;
  UINT32               TempVar_c;
  UINT32               TempVar_d;
  UINT32               TempVar8_a;
  UINT32               TempVar8_b;
  UINT32               Ignored;
  UINT32               k;
  UINT32               CpuFidSameFlag;
  UINT8                PStateMaxValueOnCurrentCore;
  UINT32               TransAndBusMastLatency;

  CpuFidSameFlag = 1;

  F10GetFrequencyXlatRegInfo (
    FamilySpecificServices,
    0,
    PStateLevelingBufferStructPtr->PStateCoreStruct[0].PStateStruct[0].CoreFreq,
    &TempVar_b,
    &TempVar_c,
    &Ignored,
    StdHeader
    );

  TempVar_d = TempVar_b;
  PStateMaxValueOnCurrentCore = PStateLevelingBufferStructPtr->PStateCoreStruct[0].PStateMaxValue;

  //
  //Check if MSRC001_00[68:64][CpuFid] is the same value for all P-states where
  //MSRC001_00[68:64][PstateEn]=1
  //
  for (k = 1; k <= PStateMaxValueOnCurrentCore; k++) {
    if (PStateLevelingBufferStructPtr->PStateCoreStruct[0].PStateStruct[k].PStateEnable != 0) {
      F10GetFrequencyXlatRegInfo (
        FamilySpecificServices,
        (UINT8) k,
        PStateLevelingBufferStructPtr->PStateCoreStruct[0].PStateStruct[k].CoreFreq,
        &TempVar_b,
        &TempVar_c,
        &Ignored,
        StdHeader
        );
    }

    if (TempVar_d != TempVar_b) {
      CpuFidSameFlag = 0;
      break;
    }
  }

  PciAddress->Address.Register = 0xD4;
  PciAddress->Address.Function = FUNC_3;
  LibAmdPciRead (AccessWidth32, *PciAddress, &TempVar_d, StdHeader);

  // PowerStepDown - Bits 20:23
  TempVar8_a = (TempVar_d & 0x00F00000) >> 20;

  // PowerStepUp   - Bits 24:27
  TempVar8_b = (TempVar_d & 0x0F000000) >> 24;

  // Convert the raw numbers in TempVar8_a and TempVar8_b into time
  F10GetPowerStepValueInTime (&TempVar8_a);
  F10GetPowerStepValueInTime (&TempVar8_b);

  //
  //(12 * (F3xD4[PowerStepDown] + F3xD4[PowerStepUp]) /1000) us
  //
  TransAndBusMastLatency =
  (12 * (TempVar8_a + TempVar8_b) + 999) / 1000;

  if (CpuFidSameFlag == 0) {
    //
    //+ F3xA0[PllLockTime]
    //
    PciAddress->Address.Register = 0xA0;
    LibAmdPciRead (AccessWidth32, *PciAddress, &TempVar_d, StdHeader);

    TempVar8_a = (0x00003800 & TempVar_d) >> 11;
    F10GetPllValueInTime (&TempVar8_a);
    TransAndBusMastLatency += TempVar8_a;
  }

  *TransitionLatency = TransAndBusMastLatency;

  return (AGESA_SUCCESS);
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Get CPU pstate register Informations.
 *
 *  @CpuServiceMethod{::F_CPU_GET_PSTATE_REGISTER_INFO}.
 *
 *    This function will check if PState is Enabled by reading MSR.
 *    This function also returns the MSR Value, that contains IddValue, IddDiv.
 *
 *  @param[in]     FamilySpecificServices    The current Family Specific Services.
 *  @param[in]     PState                    Input Pstate number for query.
 *  @param[out]    PStateEnabled             Boolean flag return pstate enable.
 *  @param[in,out] IddVal                    Pstate current value.
 *  @param[in,out] IddDiv                    Pstate current divisor.
 *  @param[in]     StdHeader                 Handle of Header for calling lib functions and services.
 *
 *  @retval      AGESA_SUCCESS Always succeeds.
 */
AGESA_STATUS
F10GetPstateRegisterInfo (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       UINT32              PState,
     OUT   BOOLEAN             *PStateEnabled,
  IN OUT   UINT32              *IddVal,
  IN OUT   UINT32              *IddDiv,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT64 MsrRegister;

  ASSERT (PState < NM_PS_REG);

  // Read PSTATE MSRs
  LibAmdMsrRead (PS_REG_BASE + (UINT32) PState, &MsrRegister, StdHeader);

  if (((PSTATE_MSR *) &MsrRegister)->PsEnable == 1) {
    // PState enable = bit 63
    *PStateEnabled = TRUE;
  } else {
    *PStateEnabled = FALSE;
  }

  // Bits 39:32 (high 32 bits [7:0])
  *IddVal = (UINT32) ((PSTATE_MSR *) &MsrRegister)->IddValue;
  // Bits 41:40 (high 32 bits [9:8])
  *IddDiv = (UINT32) ((PSTATE_MSR *) &MsrRegister)->IddDiv;

  return (AGESA_SUCCESS);
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Get CPU pstate max state.
 *
 *  @CpuServiceMethod{::F_CPU_GET_PSTATE_MAX_STATE}.
 *
 *    This function returns the MaxPStateNumber.
 *
 *  @param[in]     FamilySpecificServices    The current Family Specific Services.
 *  @param[out]    MaxPStateNumber             Boolean flag return pstate enable.
 *  @param[in]     StdHeader              Handle of Header for calling lib functions and services.
 *
 *  @retval      AGESA_SUCCESS Always succeeds.
 */
AGESA_STATUS
F10GetPstateMaxState (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
     OUT   UINT32              *MaxPStateNumber,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT64              MsrValue;

  //
  // Read PstateMaxVal [6:4] from MSR C001_0061
  // So, we will know the max pstate state in this socket.
  //
  LibAmdMsrRead (MSR_PSTATE_CURRENT_LIMIT, &MsrValue, StdHeader);
  *MaxPStateNumber = (UINT32) (((PSTATE_CURLIM_MSR *) & MsrValue)->PstateMaxVal);

  return (AGESA_SUCCESS);
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
 * Provide the features of the given HT link.
 *
 *  @CpuServiceMethod{::F_GET_HT_LINK_FEATURES}.
 *
 * This method is different than the HT Phy Features method, because for the phy registers
 * sublink 1 matches and should be programmed if the link is ganged but for PCI config
 * registers sublink 1 is reserved if the link is ganged.
 *
 * @param[in]     FamilySpecificServices    The current Family Specific Services.
 * @param[out]    Link                      The link number, for accessing non-capability set registers.
 * @param[in]     LinkBase                  The base HT Host capability PCI address for the link.
 * @param[out]    HtHostFeats               The link's features.
 * @param[in]     StdHeader                 Standard Head Pointer
 */
VOID
F10GetHtLinkFeatures (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
     OUT   UINTN                  *Link,
  IN       PCI_ADDR               *LinkBase,
     OUT   HT_HOST_FEATS          *HtHostFeats,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  PCI_ADDR  PciAddress;
  UINT32    RegValue;
  UINT32    ExtendedFreq;
  UINTN     LinkOffset;

  ASSERT (FamilySpecificServices != NULL);

  // No features present unless link is good and connected.
  HtHostFeats->HtHostValue = 0;

  // Compute link number
  *Link = (((LinkBase->Address.Function == 4) ? 4 : 0) + ((LinkBase->Address.Register - 0x80) >> 5));

  // Check coherency (HTHOST_LINK_TYPE_REG = 0x18)
  PciAddress = *LinkBase;
  PciAddress.Address.Register = PciAddress.Address.Register + HT_LINK_TYPE_REG_OFFSET;
  LibAmdPciReadBits (PciAddress, 4, 0, &RegValue, StdHeader);
  if (RegValue  == 3) {
    HtHostFeats->HtHostFeatures.Coherent = 1;
  } else if (RegValue == 7) {
    HtHostFeats->HtHostFeatures.NonCoherent = 1;
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
    // Check ganged.
    LinkOffset = (*Link) * 4;
    PciAddress = *LinkBase;
    PciAddress.Address.Function = 0;
    PciAddress.Address.Register = ((UINT32)LinkOffset + 0x170);
    LibAmdPciReadBits (PciAddress, 0, 0, &RegValue, StdHeader);
    if (RegValue == 0) {
      HtHostFeats->HtHostFeatures.UnGanged = 1;
    } else {
      HtHostFeats->HtHostFeatures.Ganged = 1;
    }
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Checks to see if the HT phy register table entry should be applied
 *
 * @CpuServiceMethod{::F_DOES_LINK_HAVE_HTFPY_FEATS}.
 *
 * This function determines if the link type field matches the HT link
 * passed in.
 *
 * This method will match for sublink 1 if the link is ganged and sublink 0 matches.
 *
 * @param[in]     FamilySpecificServices    The current Family Specific Services.
 * @param[in]     CapabilitySet    Address of the HT capability block
 * @param[in]     Link             Zero based HT link to check
 * @param[in]     HtPhyLinkType    Link type field from a register table entry to compare against
 * @param[out]    MatchedSublink1  TRUE: It is actually just sublink 1 that matches, FALSE: any other condition.
 * @param[out]    Frequency0       The frequency of sublink0 (200 MHz if not connected).
 * @param[out]    Frequency1       The frequency of sublink1 (200 MHz if not connected).
 * @param[in]     StdHeader        Standard Head Pointer
 *
 * @retval        TRUE             Link matches
 * @retval        FALSE            Link does not match
 *
 */
BOOLEAN
F10DoesLinkHaveHtPhyFeats (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       PCI_ADDR           CapabilitySet,
  IN       UINT32             Link,
  IN       HT_PHY_LINK_FEATS  *HtPhyLinkType,
     OUT   BOOLEAN            *MatchedSublink1,
     OUT   HT_FREQUENCIES     *Frequency0,
     OUT   HT_FREQUENCIES     *Frequency1,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT32    RegValue;
  UINT32    ExtendedFreq;
  PCI_ADDR  PciAddress;
  PCI_ADDR  SubLink1Address;
  HT_PHY_LINK_FEATS LinkType;
  BOOLEAN   IsReallyCheckingBoth;

  ASSERT (Link < 4);
  ASSERT (HtPhyLinkType != NULL);
  // error checks: No unknown link type bits set and not a "match none"
  ASSERT ((HtPhyLinkType->HtPhyLinkValue & ~(HTPHY_LINKTYPE_ALL)) == 0);
  ASSERT (HtPhyLinkType->HtPhyLinkValue != 0);

  *Frequency0 = 0;
  *Frequency1 = 0;
  IsReallyCheckingBoth = FALSE;
  *MatchedSublink1 = FALSE;
  LinkType.HtPhyLinkValue = 0;

  // Set the link indicators.  This assumes each sublink set is contiguous, that is, links 3, 2, 1, 0 and 7, 6, 5, 4.
  LinkType.HtPhyLinkValue |= (HTPHY_LINKTYPE_SL0_LINK0 << Link);
  LinkType.HtPhyLinkValue |= (HTPHY_LINKTYPE_SL1_LINK4 << Link);

  // if ganged, don't read sublink 1, but use sublink 0 to check.
  SubLink1Address = CapabilitySet;

  // Check ganged. Since we got called for sublink 0, sublink 1 is implemented also,
  // but only access it if it is also unganged.
  Link *= 4;
  PciAddress = CapabilitySet;
  PciAddress.Address.Function = 0;
  PciAddress.Address.Register = (Link + 0x170);
  LibAmdPciRead (AccessWidth32, PciAddress, &RegValue, StdHeader);
  RegValue = (RegValue & 0x01);
  if (RegValue == 0) {
    // Then really read sublink1, rather than using sublink0
    SubLink1Address.Address.Function = 4;
    IsReallyCheckingBoth = TRUE;
  }

  // Checks for Sublink 0

  // Check coherency (HTHOST_LINK_TYPE_REG = 0x18)
  PciAddress = CapabilitySet;
  PciAddress.Address.Register = PciAddress.Address.Register + HT_LINK_TYPE_REG_OFFSET;
  LibAmdPciRead (AccessWidth32, PciAddress, &RegValue, StdHeader);
  if ((RegValue & 0x1F) == 3) {
    LinkType.HtPhyLinkFeatures.HtPhySL0Coh = 1;
  } else if ((RegValue & 0x1F) == 7) {
    LinkType.HtPhyLinkFeatures.HtPhySL0NonCoh = 1;
  }

  // If link was not connected, don't check other attributes, make sure
  // to return zero, no match. (Phy may be powered off.)
  if ((LinkType.HtPhyLinkFeatures.HtPhySL0Coh) || (LinkType.HtPhyLinkFeatures.HtPhySL0NonCoh)) {
    // Check gen3
    PciAddress = CapabilitySet;
    PciAddress.Address.Register = PciAddress.Address.Register + HT_LINK_EXTENDED_FREQ;
    LibAmdPciRead (AccessWidth32, PciAddress, &ExtendedFreq, StdHeader);
    PciAddress = CapabilitySet;
    PciAddress.Address.Register = PciAddress.Address.Register + HT_LINK_FREQ_OFFSET;
    LibAmdPciRead (AccessWidth32, PciAddress, &RegValue, StdHeader);
    RegValue = (((ExtendedFreq & 0x1) << 4) | ((RegValue & 0x00000F00) >> 8));
    *Frequency0 = RegValue;
    if (RegValue > 6) {
      LinkType.HtPhyLinkFeatures.HtPhySL0Ht3 = 1;
    } else {
      LinkType.HtPhyLinkFeatures.HtPhySL0Ht1 = 1;
    }
  } else {
    LinkType.HtPhyLinkValue &= ~(HTPHY_LINKTYPE_SL0_ALL);
  }

  // Checks for Sublink 1
  // Check coherency (HTHOST_LINK_TYPE_REG = 0x18)
  PciAddress = SubLink1Address;
  PciAddress.Address.Register = PciAddress.Address.Register + HT_LINK_TYPE_REG_OFFSET;
  LibAmdPciRead (AccessWidth32, PciAddress, &RegValue, StdHeader);
  if ((RegValue & 0x1F) == 3) {
    LinkType.HtPhyLinkFeatures.HtPhySL1Coh = 1;
  } else if ((RegValue & 0x1F) == 7) {
    LinkType.HtPhyLinkFeatures.HtPhySL1NonCoh = 1;
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
  } else {
    LinkType.HtPhyLinkValue &= ~(HTPHY_LINKTYPE_SL1_ALL);
  }

  // For Deemphasis checking, indicate whether it was actually sublink 1 that matched.
  // If the link is ganged or only sublink 0 matched, or the link features didn't match, this is false.
  if ((LinkType.HtPhyLinkValue & HtPhyLinkType->HtPhyLinkValue) != 0) {
    if (IsReallyCheckingBoth &&
        (((LinkType.HtPhyLinkValue & HtPhyLinkType->HtPhyLinkValue) & (HTPHY_LINKTYPE_SL1_ALL)) != 0)) {
      *MatchedSublink1 = TRUE;
    }
    return TRUE;  // Link matches at least one of the desired characteristics
  } else {
    return FALSE; // Link does not match any criteria
  }
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
 * Set the HT Link Token Count registers (F3X1[54,50,4C,48]).
 *
 * @TableEntryTypeMethod{::HtTokenPciRegister}.
 *
 * Make the current core's PCI address with the function and register for the entry.
 * For all HT links, check the link's feature set for a match to the entry.
 * Read - Modify - Write the PCI register, clearing masked bits, and setting the data bits.
 *
 * @param[in]     Entry             The Link Token register entry to perform
 * @param[in]     PlatformConfig    Config handle for platform specific information
 * @param[in]     StdHeader         Config handle for library and services.
 *
 */
VOID
F10SetRegisterForHtLinkTokenEntry (
  IN       TABLE_ENTRY_DATA       *Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINTN                 Link;
  UINTN                 LinkCount;
  UINT32                MySocket;
  UINT32                MyModule;
  AGESA_STATUS          IgnoredStatus;
  UINT32                Ignored;
  CPU_LOGICAL_ID        CpuFamilyRevision;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;
  PCI_ADDR              CapabilitySet;
  HT_HOST_FEATS         HtHostFeats;
  PERFORMANCE_PROFILE_FEATS PlatformProfile;
  UINTN                 ProcessorCount;
  UINT32                RegisterData;
  PCI_ADDR              PciAddress;

  // Errors:  Possible values in unused entry space, extra type features, value range checks.
  // Check that the entry type is correct and the actual supplied entry data is appropriate for that entry.
  ASSERT (((Entry->HtTokenEntry.LinkFeats.HtHostValue & ~((HT_HOST_FEATURES_ALL) | (HT_HOST_AND))) == 0) &&
          ((Entry->HtTokenEntry.PerformanceFeats.PerformanceProfileValue & ~((PERFORMANCE_PROFILE_ALL) | (PERFORMANCE_AND))) == 0) &&
          (Entry->HtTokenEntry.Mask != 0));

  HtHostFeats.HtHostValue = 0;
  IdentifyCore (StdHeader, &MySocket, &MyModule, &Ignored, &IgnoredStatus);
  GetPciAddress (StdHeader, MySocket, MyModule, &CapabilitySet, &IgnoredStatus);
  GetLogicalIdOfCurrentCore (&CpuFamilyRevision, StdHeader);
  GetCpuServicesFromLogicalId (&CpuFamilyRevision, &FamilySpecificServices, StdHeader);

  // Check if the actual processor count is in either range.
  ProcessorCount = GetNumberOfProcessors (StdHeader);
  if (IsEitherCountInRange (ProcessorCount, ProcessorCount, Entry->HtTokenEntry.ProcessorCounts.ProcessorCountRanges)) {
    // Check for any performance profile features.
    GetPerformanceFeatures (&PlatformProfile, PlatformConfig, StdHeader);
    if (DoesEntryTypeSpecificInfoMatch (PlatformProfile.PerformanceProfileValue,
                                        Entry->HtTokenEntry.PerformanceFeats.PerformanceProfileValue)) {
      // Check the link features.
      LinkCount = 0;
      while (LinkCount < 4) {
        if (FindHtHostCapability (LinkCount, &CapabilitySet, StdHeader)) {
          FamilySpecificServices->GetHtLinkFeatures (FamilySpecificServices, &Link, &CapabilitySet, &HtHostFeats, StdHeader);
          if (DoesEntryTypeSpecificInfoMatch (HtHostFeats.HtHostValue, Entry->HtTokenEntry.LinkFeats.HtHostValue)) {
            // Do the HT Host PCI register update.
            PciAddress = CapabilitySet;
            PciAddress.Address.Function = 3;
            PciAddress.Address.Register = LINK_TO_XCS_TOKEN_COUNT_REG_3X148 + ((UINT32)Link * 4);
            LibAmdPciRead (AccessWidth32, PciAddress, &RegisterData, StdHeader);
            RegisterData = RegisterData & (~(Entry->HtTokenEntry.Mask));
            RegisterData = RegisterData | Entry->HtTokenEntry.Data;
            LibAmdPciWrite (AccessWidth32, PciAddress, &RegisterData, StdHeader);
          }
        } else {
          // No more Capabilities.
          break;
        }
        LinkCount ++;
      }
    }
  }
}

