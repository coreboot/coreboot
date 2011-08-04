/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_14 Pstate feature support functions.
 *
 * Provides the functions necessary to initialize the Pstate feature.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/F14
 * @e \$Revision: 44325 $   @e \$Date: 2010-12-22 03:29:53 -0700 (Wed, 22 Dec 2010) $
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
#include "Ids.h"
#include "cpuPstateTables.h"
#include "cpuFamilyTranslation.h"
#include "cpuRegisters.h"
#include "cpuF14Utilities.h"
#include "cpuF14PowerMgmt.h"
#include "CommonReturns.h"
#include "Filecode.h"
#define FILECODE PROC_CPU_FAMILY_0X14_CPUF14PSTATE_FILECODE

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

AGESA_STATUS
F14GetPstateTransLatency (
  IN       PSTATE_CPU_FAMILY_SERVICES *PstateCpuServices,
  IN       PSTATE_LEVELING        *PStateLevelingBufferStructPtr,
  IN       PCI_ADDR               *PciAddress,
     OUT   UINT32                 *TransitionLatency,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

AGESA_STATUS
F14GetPstateFrequency (
  IN       PSTATE_CPU_FAMILY_SERVICES *PstateCpuServices,
  IN       UINT8 StateNumber,
     OUT   UINT32 *FrequencyInMHz,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
F14GetPstatePower (
  IN       PSTATE_CPU_FAMILY_SERVICES *PstateCpuServices,
  IN       UINT8 StateNumber,
     OUT   UINT32 *PowerInMw,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
F14GetPstateMaxState (
  IN       PSTATE_CPU_FAMILY_SERVICES *PstateCpuServices,
     OUT   UINT32              *MaxPStateNumber,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

AGESA_STATUS
F14GetPstateRegisterInfo (
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

/**
 *  Family specific call to set core TscFreqSel.
 *
 * @param[in]     PstateCpuServices  Pstate CPU services.
 * @param[in]     StdHeader          Config Handle for library, services.
 *
 */
VOID
STATIC
F14SetTscFreqSel (
  IN       PSTATE_CPU_FAMILY_SERVICES *PstateCpuServices,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64             MsrValue;

  LibAmdMsrRead (MSR_HWCR, &MsrValue, StdHeader);
  MsrValue = MsrValue | BIT24;
  LibAmdMsrWrite (MSR_HWCR, &MsrValue, StdHeader);
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific call to get Pstate Transition Latency.
 *
 *  Follow BKDG, return zero currently.
 *
 *  @param[in]     PstateCpuServices                Pstate CPU services.
 *  @param[in]     PStateLevelingBufferStructPtr    Pstate row data buffer pointer
 *  @param[in]     PciAddress                       Pci address
 *  @param[out]    TransitionLatency                The transition latency.
 *  @param[in]     StdHeader                        Header for library and services
 *
 *  @retval      AGESA_SUCCESS Always succeeds.
 */
AGESA_STATUS
F14GetPstateTransLatency (
  IN       PSTATE_CPU_FAMILY_SERVICES *PstateCpuServices,
  IN       PSTATE_LEVELING        *PStateLevelingBufferStructPtr,
  IN       PCI_ADDR               *PciAddress,
     OUT   UINT32                 *TransitionLatency,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  //
  // TransitionLatency (us) = BusMasterLatency (us) = 0 us, calculation may
  // change due to a potential new encoding.
  //
  *TransitionLatency = 0;
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
F14GetPstateFrequency (
  IN       PSTATE_CPU_FAMILY_SERVICES *PstateCpuServices,
  IN       UINT8 StateNumber,
     OUT   UINT32 *FrequencyInMHz,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 CpuDidLSD;
  UINT32 CpuDidMSD;
  UINT32 CoreClkDivisor;
  UINT32 PciReg;
  UINT64 MsrReg;
  BOOLEAN FrequencyCalculated;
  BOOLEAN ClockDivisorCalculated;
  PCI_ADDR PciAddress;
  UINT32 MainPllOpFreq;
  UINT32 MainPllFid;

  ASSERT (StateNumber < NM_PS_REG);

  LibAmdMsrRead (PS_REG_BASE + (UINT32) StateNumber, &MsrReg, StdHeader);
  ASSERT (((PSTATE_MSR *) &MsrReg)->PsEnable == 1);

  CpuDidLSD = (UINT32) (((PSTATE_MSR *) &MsrReg)->CpuDidLSD);
  CpuDidMSD = (UINT32) (((PSTATE_MSR *) &MsrReg)->CpuDidMSD);

  FrequencyCalculated = FALSE;
  ClockDivisorCalculated = FALSE;
  CoreClkDivisor = 1;

  if ((CpuDidLSD > 3) || (CpuDidMSD > 0x19)) {
    // Either CpuDidLSD or CpuDidMSD is set to an undefined value.
    // This is due to either a misfused CPU, or an invalid P-state MSR write.
    ASSERT (FALSE);
    ClockDivisorCalculated = TRUE;
    FrequencyCalculated = TRUE;
    CoreClkDivisor = 4;
    *FrequencyInMHz = 100;
  }

  if (!ClockDivisorCalculated) {
    CoreClkDivisor = (CpuDidMSD * 4) + CpuDidLSD + 4;
  }
  // Checking for supported divisor value
  ASSERT (((CoreClkDivisor >= 4) && (CoreClkDivisor <= 63)) ||
          ((CoreClkDivisor >= 64) && (CoreClkDivisor <= 106) && ((CoreClkDivisor % 2) == 0)));

  if (!FrequencyCalculated) {
    // Get D18F3xD4[MainPllOpFreqId] frequency
    PciAddress.AddressValue = CPTC0_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &PciReg, StdHeader);

    if (((CLK_PWR_TIMING_CTRL_REGISTER *) &PciReg)->MainPllOpFreqIdEn == 1) {
      MainPllFid = ((CLK_PWR_TIMING_CTRL_REGISTER *) &PciReg)->MainPllOpFreqId;
    } else {
      MainPllFid = 0;
    }
    MainPllOpFreq = ((MainPllFid + 0x10) * 100);

    *FrequencyInMHz = MainPllOpFreq * 4 / CoreClkDivisor;
  }

  return (AGESA_SUCCESS);
}

/*--------------------------------------------------------------------------------------*/
/**
 *
 *  Family specific call to calculates the power in milliWatts of the desired P-state.
 *
 *  @param[in]   PstateCpuServices        Pstate CPU services.
 *  @param[in]   StateNumber              Which P-state to analyze
 *  @param[out]  PowerInMw                The Power in milliWatts of that P-State
 *  @param[in]   StdHeader                Header for library and services
 *
 *  @retval      AGESA_SUCCESS Always succeeds.
 */
AGESA_STATUS
F14GetPstatePower (
  IN       PSTATE_CPU_FAMILY_SERVICES *PstateCpuServices,
  IN       UINT8 StateNumber,
     OUT   UINT32 *PowerInMw,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32  CpuVid;
  UINT32  IddValue;
  UINT32  IddDiv;
  UINT32  V_x10000;
  UINT32  Power;
  UINT64  MsrReg;

  ASSERT (StateNumber < NM_PS_REG);
  LibAmdMsrRead (PS_REG_BASE + (UINT32) StateNumber, &MsrReg, StdHeader);
  ASSERT (((PSTATE_MSR *) &MsrReg)->PsEnable == 1);
  CpuVid = (UINT32) (((PSTATE_MSR *) &MsrReg)->CpuVid);
  IddValue = (UINT32) (((PSTATE_MSR *) &MsrReg)->IddValue);
  IddDiv = (UINT32) (((PSTATE_MSR *) &MsrReg)->IddDiv);

  if (CpuVid >= 0x7C) {
    V_x10000 = 0;
  } else {
    V_x10000 = 15500L - (125L * CpuVid);
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
 *  Family specific call to get CPU pstate max state.
 *
 *  @param[in]     PstateCpuServices      Pstate CPU services.
 *  @param[out]    MaxPStateNumber        Boolean flag return pstate enable.
 *  @param[in]     StdHeader              Handle of Header for calling lib functions and services.
 *
 *  @retval      AGESA_SUCCESS Always succeeds.
 */
AGESA_STATUS
F14GetPstateMaxState (
  IN       PSTATE_CPU_FAMILY_SERVICES *PstateCpuServices,
     OUT   UINT32              *MaxPStateNumber,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT64              MsrValue;
  UINT32              PciReg;
  PCI_ADDR            PciAddress;

  // For F14 CPU, skip boosted p-state. The boosted p-state number = D18F4x15C[NumBoostStates].
  PciAddress.AddressValue = CPB_CTRL_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, PciAddress, &PciReg, StdHeader); // D18F4x15C

  //
  // Read PstateMaxVal [6:4] from MSR C001_0061
  // So, we will know the max pstate state in this socket.
  //
  LibAmdMsrRead (MSR_PSTATE_CURRENT_LIMIT, &MsrValue, StdHeader);
  *MaxPStateNumber = (UINT32) (((PSTATE_CURLIM_MSR *) &MsrValue)->PstateMaxVal) + (UINT32) (((CPB_CTRL_REGISTER *) &PciReg)->NumBoostStates);

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
F14GetPstateRegisterInfo (
  IN       PSTATE_CPU_FAMILY_SERVICES *PstateCpuServices,
  IN       UINT32              PState,
     OUT   BOOLEAN             *PStateEnabled,
  IN OUT   UINT32              *IddVal,
  IN OUT   UINT32              *IddDiv,
     OUT   UINT32              *SwPstateNumber,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT64              LocalMsrReg;
  UINT32              LocalPciReg;
  PCI_ADDR            PciAddress;
  CPU_LOGICAL_ID      CpuFamilyRevision;

  ASSERT (PState < NM_PS_REG);

  // Read PSTATE MSRs
  LibAmdMsrRead (PS_REG_BASE + (UINT32) PState, &LocalMsrReg, StdHeader);

  *SwPstateNumber = PState;

  if (((PSTATE_MSR *) &LocalMsrReg)->PsEnable == 1) {
    // PState enable = bit 63
    *PStateEnabled = TRUE;
    // For F14 CPU, skip boosted p-state. The boosted p-state number = D18F4x15C[NumBoostStates].
    GetLogicalIdOfCurrentCore (&CpuFamilyRevision, StdHeader);
    if ((CpuFamilyRevision.Revision & (AMD_F14_ON_Ax | AMD_F14_ON_Bx)) == 0) {
      // ON_Ax & ON_Bx don't have boosted p-state function
      PciAddress.AddressValue = CPB_CTRL_PCI_ADDR;
      LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciReg, StdHeader); // D18F4x15C
      //
      // Check input pstate belongs to Boosted-Pstate, if yes, return *PStateEnabled = FALSE.
      //
      if (PState < ((CPB_CTRL_REGISTER *) &LocalPciReg)->NumBoostStates) {
        *PStateEnabled = FALSE;
      } else {
        *SwPstateNumber = PState - ((CPB_CTRL_REGISTER *) &LocalPciReg)->NumBoostStates;
      }
    }
  } else {
    *PStateEnabled = FALSE;
  }

  // Bits 39:32 (high 32 bits [7:0])
  *IddVal = (UINT32) ((PSTATE_MSR *) &LocalMsrReg)->IddValue;
  // Bits 41:40 (high 32 bits [9:8])
  *IddDiv = (UINT32) ((PSTATE_MSR *) &LocalMsrReg)->IddDiv;

  return (AGESA_SUCCESS);
}


CONST PSTATE_CPU_FAMILY_SERVICES ROMDATA F14PstateServices =
{
  0,
  (PF_PSTATE_PSD_IS_NEEDED) CommonReturnTrue,
  (PF_PSTATE_PSD_IS_DEPENDENT) CommonReturnTrue,
  F14SetTscFreqSel,
  F14GetPstateTransLatency,
  F14GetPstateFrequency,
  (PF_CPU_SET_PSTATE_LEVELING_REG) CommonReturnAgesaSuccess,
  F14GetPstatePower,
  F14GetPstateMaxState,
  F14GetPstateRegisterInfo
};
