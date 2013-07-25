/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_16 Kabini Pstate feature support functions.
 *
 * Provides the functions necessary to initialize the Pstate feature.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x16/KB
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (c) 2008 - 2013, Advanced Micro Devices, Inc.
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
#include "GeneralServices.h"
#include "cpuPstateTables.h"
#include "cpuRegisters.h"
#include "Table.h"
#include "cpuFamilyTranslation.h"
#include "cpuFamRegisters.h"
#include "cpuF16Utilities.h"
#include "F16KbUtilities.h"
#include "cpuF16PowerMgmt.h"
#include "F16KbPowerMgmt.h"
#include "CommonReturns.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X16_KB_F16KBPSTATE_FILECODE


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
F16KbGetPowerStepValueInTime (
  IN OUT   UINT32  *PowerStepPtr
  );

VOID
STATIC
F16KbGetPllValueInTime (
  IN OUT   UINT32  *PllLockTimePtr
  );


AGESA_STATUS
F16KbGetPstateTransLatency (
  IN       PSTATE_CPU_FAMILY_SERVICES *PstateCpuServices,
  IN       PSTATE_LEVELING        *PStateLevelingBufferStructPtr,
  IN       PCI_ADDR               *PciAddress,
     OUT   UINT32                 *TransitionLatency,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

AGESA_STATUS
F16KbGetPstateFrequency (
  IN       PSTATE_CPU_FAMILY_SERVICES *PstateCpuServices,
  IN       UINT8 StateNumber,
     OUT   UINT32 *FrequencyInMHz,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
F16KbGetPstatePower (
  IN       PSTATE_CPU_FAMILY_SERVICES *PstateCpuServices,
  IN       UINT8 StateNumber,
     OUT   UINT32 *PowerInMw,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
F16KbGetPstateMaxState (
  IN       PSTATE_CPU_FAMILY_SERVICES *PstateCpuServices,
     OUT   UINT32              *MaxPStateNumber,
     OUT   UINT8               *NumberOfBoostStates,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

AGESA_STATUS
F16KbGetPstateRegisterInfo (
  IN       PSTATE_CPU_FAMILY_SERVICES *PstateCpuServices,
  IN       UINT32              PState,
     OUT   BOOLEAN             *PStateEnabled,
  IN OUT   UINT32              *IddVal,
  IN OUT   UINT32              *IddDiv,
     OUT   UINT32              *SwPstateNumber,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );



/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific call to check if Pstate PSD is dependent.
 *
 * @param[in]     PstateCpuServices  Pstate CPU services.
 * @param[in,out] PlatformConfig     Contains the runtime modifiable feature input data.
 * @param[in]     StdHeader          Config Handle for library, services.
 *
 * @retval       TRUE               PSD is dependent.
 * @retval       FALSE              PSD is independent.
 *
 */
BOOLEAN
STATIC
F16KbIsPstatePsdDependent (
  IN       PSTATE_CPU_FAMILY_SERVICES *PstateCpuServices,
  IN OUT   PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  IDS_HDT_CONSOLE (CPU_TRACE, "  F16KbIsPstatePsdDependent\n");
  // Family 16h KB defaults to independent PSD
  IDS_HDT_CONSOLE (CPU_TRACE, "    P-state PSD is independent.\n");
  return FALSE;
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific call to get Pstate Transition Latency.
 *
 *  Calculate TransitionLatency by power step value and pll value.
 *
 *  @param[in]     PstateCpuServices                 Pstate CPU services.
 *  @param[in]     PStateLevelingBufferStructPtr     Pstate row data buffer pointer
 *  @param[in]     PciAddress                        Pci address
 *  @param[out]    TransitionLatency                 The transition latency.
 *  @param[in]     StdHeader                         Header for library and services
 *
 *  @retval      AGESA_SUCCESS Always succeeds.
 */
AGESA_STATUS
F16KbGetPstateTransLatency (
  IN       PSTATE_CPU_FAMILY_SERVICES *PstateCpuServices,
  IN       PSTATE_LEVELING        *PStateLevelingBufferStructPtr,
  IN       PCI_ADDR               *PciAddress,
     OUT   UINT32                 *TransitionLatency,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32               CpuFid;
  UINT32               HwP0CpuFid;
  UINT32               PowerStepDown;
  UINT32               PowerStepUp;
  UINT32               PllLockTime;
  UINT32               LocalPciRegister;
  UINT32               k;
  UINT32               PStateMaxValueOnCurrentCore;
  UINT32               TransAndBusMastLatency;
  UINT64               MsrData;
  BOOLEAN              CpuFidSameFlag;

  IDS_HDT_CONSOLE (CPU_TRACE, "  F16KbGetPstateTransLatency\n");

  CpuFidSameFlag = TRUE;
  HwP0CpuFid = 0;

  // Get PStateMaxValue
  PciAddress->Address.Register = CPTC2_REG;
  PciAddress->Address.Function = FUNC_3;
  LibAmdPciRead (AccessWidth32, *PciAddress, &LocalPciRegister, StdHeader);
  PStateMaxValueOnCurrentCore = ((CLK_PWR_TIMING_CTRL2_REGISTER *) &LocalPciRegister)->HwPstateMaxVal;

  //
  //Check if MSRC001_00[6B:64][CpuFid] is the same value for all P-states
  //
  for (k = 0; k <= PStateMaxValueOnCurrentCore; k++) {
    LibAmdMsrRead ((MSR_PSTATE_0 + k), &MsrData, StdHeader);
    CpuFid = (UINT32) (((PSTATE_MSR *) &MsrData)->CpuFid);
    if (k == 0) {
      HwP0CpuFid = CpuFid;
    }
    if (HwP0CpuFid != CpuFid) {
      CpuFidSameFlag = FALSE;
      break;
    }
  }

  PciAddress->Address.Register = CPTC0_REG;
  PciAddress->Address.Function = FUNC_3;
  LibAmdPciRead (AccessWidth32, *PciAddress, &LocalPciRegister, StdHeader);

  // PowerStepDown - Bits 20:23
  PowerStepDown = ((CLK_PWR_TIMING_CTRL_REGISTER *) &LocalPciRegister)->PowerStepDown;

  // PowerStepUp   - Bits 24:27
  PowerStepUp = ((CLK_PWR_TIMING_CTRL_REGISTER *) &LocalPciRegister)->PowerStepUp;

  // Convert the raw numbers in TempVar8_a and TempVar8_b into time
  F16KbGetPowerStepValueInTime (&PowerStepDown);
  F16KbGetPowerStepValueInTime (&PowerStepUp);

  //
  //(15 * (F3xD4[PowerStepDown] + F3xD4[PowerStepUp]) /1000) us
  //
  TransAndBusMastLatency =
  (15 * (PowerStepDown + PowerStepUp) + 999) / 1000;

  if (!CpuFidSameFlag) {
    //
    //+ F3xA0[PllLockTime]
    //
    PciAddress->Address.Register = PW_CTL_MISC_REG;
    LibAmdPciRead (AccessWidth32, *PciAddress, &LocalPciRegister, StdHeader);

    PllLockTime = ((POWER_CTRL_MISC_REGISTER *) &LocalPciRegister)->PllLockTime;
    F16KbGetPllValueInTime (&PllLockTime);
    TransAndBusMastLatency += PllLockTime;
  }

  *TransitionLatency = TransAndBusMastLatency;

  return (AGESA_SUCCESS);
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific call to calculates the frequency in megahertz of the desired P-state.
 *
 *  @param[in]   PstateCpuServices        Pstate CPU services.
 *  @param[in]   StateNumber              The P-State to analyze.
 *  @param[out]  FrequencyInMHz           The P-State's frequency in MegaHertz
 *  @param[in]   StdHeader                Header for library and services
 *
 *  @retval      AGESA_SUCCESS   Always Succeeds.
 */
AGESA_STATUS
F16KbGetPstateFrequency (
  IN       PSTATE_CPU_FAMILY_SERVICES *PstateCpuServices,
  IN       UINT8 StateNumber,
     OUT   UINT32 *FrequencyInMHz,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8 TempValue;
  UINT32 CpuDid;
  UINT32 CpuFid;
  UINT64 LocalMsrRegister;
  PCI_ADDR PciAddress;
  HTC_REGISTER HtcReg;
  CLK_PWR_TIMING_CTRL2_REGISTER ClkPwrTimingCtrl2;


  IDS_HDT_CONSOLE (CPU_TRACE, "  F16KbGetPstateFrequency - P%d\n", StateNumber);

  PciAddress.AddressValue = CPTC2_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, PciAddress, &ClkPwrTimingCtrl2, StdHeader);

  PciAddress.AddressValue = HTC_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, PciAddress, &HtcReg, StdHeader);

  ASSERT ((StateNumber <= ClkPwrTimingCtrl2.HwPstateMaxVal) || ((StateNumber == HtcReg.HtcPstateLimit)));

  LibAmdMsrRead (PS_REG_BASE + (UINT32) StateNumber, &LocalMsrRegister, StdHeader);

  CpuDid = (UINT32) (((PSTATE_MSR *) &LocalMsrRegister)->CpuDid);
  CpuFid = (UINT32) (((PSTATE_MSR *) &LocalMsrRegister)->CpuFid);

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
  IDS_HDT_CONSOLE (CPU_TRACE, "    FrequencyInMHz=%d, CpuFid=%d, CpuDid=%d\n", *FrequencyInMHz, CpuFid, CpuDid);

  return (AGESA_SUCCESS);
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific call to calculates the power in milliWatts of the desired P-state.
 *
 *  @param[in]     PstateCpuServices      Pstate CPU services.
 *  @param[in]   StateNumber              Which P-state to analyze
 *  @param[out]  PowerInMw                The Power in milliWatts of that P-State
 *  @param[in]   StdHeader                Header for library and services
 *
 *  @retval      AGESA_SUCCESS Always succeeds.
 */
AGESA_STATUS
F16KbGetPstatePower (
  IN       PSTATE_CPU_FAMILY_SERVICES *PstateCpuServices,
  IN       UINT8 StateNumber,
     OUT   UINT32 *PowerInMw,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32  CpuVid;
  UINT32  IddValue;
  UINT32  IddDiv;
  UINT32  V_x100000;
  UINT32  Power;
  UINT64  LocalMsrRegister;

  IDS_HDT_CONSOLE (CPU_TRACE, "  F16KbGetPstatePower - P%d\n", StateNumber);

  ASSERT (StateNumber < NM_PS_REG);
  LibAmdMsrRead (PS_REG_BASE + (UINT32) StateNumber, &LocalMsrRegister, StdHeader);
  ASSERT (((PSTATE_MSR *) &LocalMsrRegister)->PsEnable == 1);
  CpuVid = (UINT32) (((PSTATE_MSR *) &LocalMsrRegister)->CpuVid);
  IddValue = (UINT32) (((PSTATE_MSR *) &LocalMsrRegister)->IddValue);
  IddDiv = (UINT32) (((PSTATE_MSR *) &LocalMsrRegister)->IddDiv);

  if (CpuVid >= 0xF8) {
    V_x100000 = 0;
  } else {
    V_x100000 = 155000L - (625L * CpuVid);
  }

  Power = V_x100000 * IddValue;

  switch (IddDiv) {
  case 0:
    *PowerInMw = Power / 100L;
    break;
  case 1:
    *PowerInMw = Power / 1000L;
    break;
  case 2:
    *PowerInMw = Power / 10000L;
    break;
  default:
    // IddDiv is set to an undefined value.  This is due to either a misfused CPU, or
    // an invalid P-state MSR write.
    ASSERT (FALSE);
    *PowerInMw = 0;
    break;
  }
  IDS_HDT_CONSOLE (CPU_TRACE, "  PowerInMw=%d, CpuVid=%d, IddValue=%d, IddDiv=%d\n", *PowerInMw, CpuVid, IddValue, IddDiv);

  return (AGESA_SUCCESS);
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific call to get CPU pstate max state.
 *
 *  @param[in]     PstateCpuServices      Pstate CPU services.
 *  @param[out]    MaxPStateNumber        The max hw pstate value on the current socket.
 *  @param[out]    NumberOfBoostStates    The number of boosted P-states on the current socket.
 *  @param[in]     StdHeader              Handle of Header for calling lib functions and services.
 *
 *  @retval      AGESA_SUCCESS Always succeeds.
 */
AGESA_STATUS
F16KbGetPstateMaxState (
  IN       PSTATE_CPU_FAMILY_SERVICES *PstateCpuServices,
     OUT   UINT32              *MaxPStateNumber,
     OUT   UINT8               *NumberOfBoostStates,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32               NumBoostStates;
  UINT64               MsrValue;
  UINT32               LocalPciRegister;
  PCI_ADDR             PciAddress;

  IDS_HDT_CONSOLE (CPU_TRACE, "  F16KbGetPstateMaxState\n");

  LocalPciRegister = 0;

  // For F16 Kabini CPU, skip boosted p-state. The boosted p-state number = F4x15C[NumBoostStates].
  PciAddress.AddressValue = CPB_CTRL_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader); // F4x15C

  NumBoostStates = ((CPB_CTRL_REGISTER *) &LocalPciRegister)->NumBoostStates;
  *NumberOfBoostStates = (UINT8) NumBoostStates;

  //
  // Read PstateMaxVal [6:4] from MSR C001_0061
  // So, we will know the max pstate state in this socket.
  //
  LibAmdMsrRead (MSR_PSTATE_CURRENT_LIMIT, &MsrValue, StdHeader);
  *MaxPStateNumber = (UINT32) (((PSTATE_CURLIM_MSR *) &MsrValue)->PstateMaxVal) + NumBoostStates;
  IDS_HDT_CONSOLE (CPU_TRACE, "    MaxPStateNumber=%d, NumBoostStates=%d\n", *MaxPStateNumber, NumBoostStates);

  return (AGESA_SUCCESS);
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific call to get CPU pstate register information.
 *
 *  @param[in]     PstateCpuServices         Pstate CPU services.
 *  @param[in]     PState                    Input Pstate number for query.
 *  @param[out]    PStateEnabled             Boolean flag return pstate enable.
 *  @param[in,out] IddVal                    Pstate current value.
 *  @param[in,out] IddDiv                    Pstate current divisor.
 *  @param[out]    SwPstateNumber            Software P-state number.
 *  @param[in]     StdHeader                 Handle of Header for calling lib functions and services.
 *
 *  @retval      AGESA_SUCCESS Always succeeds.
 */
AGESA_STATUS
F16KbGetPstateRegisterInfo (
  IN       PSTATE_CPU_FAMILY_SERVICES *PstateCpuServices,
  IN       UINT32              PState,
     OUT   BOOLEAN             *PStateEnabled,
  IN OUT   UINT32              *IddVal,
  IN OUT   UINT32              *IddDiv,
     OUT   UINT32              *SwPstateNumber,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32    LocalPciRegister;
  UINT64    LocalMsrRegister;
  PCI_ADDR  PciAddress;

  IDS_HDT_CONSOLE (CPU_TRACE, "  F16KbGetPstateRegisterInfo - P%d\n", PState);

  ASSERT (PState < NM_PS_REG);

  // For F16 Kabini CPU, skip boosted p-state. The boosted p-state number = F4x15C[NumBoostStates].
  PciAddress.AddressValue = CPB_CTRL_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader); // F4x15C

  // Read PSTATE MSRs
  LibAmdMsrRead (PS_REG_BASE + (UINT32) PState, &LocalMsrRegister, StdHeader);

  *SwPstateNumber = PState;

  if (((PSTATE_MSR *) &LocalMsrRegister)->PsEnable == 1) {
    // PState enable = bit 63
    *PStateEnabled = TRUE;
    //
    // Check input pstate belongs to Boosted-Pstate, if yes, return *PStateEnabled = FALSE.
    //
    if (PState < ((CPB_CTRL_REGISTER *) &LocalPciRegister)->NumBoostStates) {
      *PStateEnabled = FALSE;
    } else {
      *SwPstateNumber = PState - ((CPB_CTRL_REGISTER *) &LocalPciRegister)->NumBoostStates;
    }
    IDS_HDT_CONSOLE (CPU_TRACE, "  Pstate %d is enabled. SwPstateNumber=%d\n", PState, *SwPstateNumber);
  } else {
    *PStateEnabled = FALSE;
  }

  // Bits 39:32 (high 32 bits [7:0])
  *IddVal = (UINT32) ((PSTATE_MSR *) &LocalMsrRegister)->IddValue;
  // Bits 41:40 (high 32 bits [9:8])
  *IddDiv = (UINT32) ((PSTATE_MSR *) &LocalMsrRegister)->IddDiv;

  IDS_HDT_CONSOLE (CPU_TRACE, "    IddVal=%d, IddDiv=%d\n", *IddVal, *IddDiv);
  return (AGESA_SUCCESS);
}


CONST PSTATE_CPU_FAMILY_SERVICES ROMDATA F16KbPstateServices =
{
  0,
  (PF_PSTATE_PSD_IS_NEEDED) CommonReturnTrue,
  F16KbIsPstatePsdDependent,
  F16KbGetPstateTransLatency,
  F16KbGetPstateFrequency,
  (PF_CPU_SET_PSTATE_LEVELING_REG) CommonReturnAgesaSuccess,
  F16KbGetPstatePower,
  F16KbGetPstateMaxState,
  F16KbGetPstateRegisterInfo
};


/*---------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */


/**
 *---------------------------------------------------------------------------------------
 *
 *  F16KbGetPowerStepValueInTime
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
VOID
STATIC
F16KbGetPowerStepValueInTime (
  IN OUT   UINT32  *PowerStepPtr
  )
{
  UINT32 TempVar_a;

  IDS_HDT_CONSOLE (CPU_TRACE, "  F16KbGetPowerStepValueInTime\n");

  TempVar_a = *PowerStepPtr;

  if (TempVar_a < 0x4) {
    *PowerStepPtr = 400 - (TempVar_a * 100);
  } else if (TempVar_a <  0x9) {
    *PowerStepPtr = 130 - (TempVar_a * 10);
  } else {
    *PowerStepPtr = 90 - (TempVar_a * 5);
  }
  IDS_HDT_CONSOLE (CPU_TRACE, "  PowerStepPtr=%d\n", *PowerStepPtr);
}

/**
 *---------------------------------------------------------------------------------------
 *
 *  F16KbGetPllValueInTime
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
VOID
STATIC
F16KbGetPllValueInTime (
  IN OUT   UINT32  *PllLockTimePtr
  )
{
  IDS_HDT_CONSOLE (CPU_TRACE, "  F16KbGetPllValueInTime\n");

  if (*PllLockTimePtr < 4) {
    *PllLockTimePtr = *PllLockTimePtr + 1;
  } else if (*PllLockTimePtr == 4) {
    *PllLockTimePtr = 8;
  } else if (*PllLockTimePtr == 5) {
    *PllLockTimePtr = 16;
  } else
    *PllLockTimePtr = 0;
  IDS_HDT_CONSOLE (CPU_TRACE, "    PllLockTimePtr=%d\n", *PllLockTimePtr);
}


