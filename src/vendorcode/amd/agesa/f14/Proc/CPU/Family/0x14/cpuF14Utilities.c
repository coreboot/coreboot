/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_14 specific utility functions.
 *
 * Provides numerous utility functions specific to family 14h.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/F14
 * @e \$Revision: 37640 $   @e \$Date: 2010-09-08 23:01:59 +0800 (Wed, 08 Sep 2010) $
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
#include "cpuRegisters.h"
#include "cpuFamilyTranslation.h"
#include "cpuPstateTables.h"
#include "cpuF14PowerMgmt.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "cpuF14Utilities.h"
#include "cpuPostInit.h"
#include "Filecode.h"
#define FILECODE PROC_CPU_FAMILY_0X14_CPUF14UTILITIES_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern CPU_FAMILY_SUPPORT_TABLE            PstateFamilyServiceTable;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
F14ConvertEnabledBitsIntoCount (
     OUT   UINT8 *EnabledCoreCountPtr,
  IN       UINT8 FusedCoreCount,
  IN       UINT8 EnabledCores
  );

BOOLEAN
F14GetNbPstateInfo (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       PCI_ADDR               *PciAddress,
  IN       UINT32                 NbPstate,
     OUT   UINT32                 *FreqNumeratorInMHz,
     OUT   UINT32                 *FreqDivisor,
     OUT   UINT32                 *VoltageInuV,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

BOOLEAN
F14IsNbPstateEnabled (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

BOOLEAN
F14GetProcIddMax (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       UINT8                  Pstate,
     OUT   UINT32                 *ProcIddMax,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

UINT8
F14GetNumberOfCoresForBrandstring (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

VOID
F14ConvertEnabledBitsIntoCount (
     OUT   UINT8 *EnabledCoreCountPtr,
  IN       UINT8 FusedCoreCount,
  IN       UINT8 EnabledCores
  )
{
  UINT8 i;
  UINT8 j;
  UINT8 EnabledCoreCount;

  EnabledCoreCount = 0;

  for (i = 0; i < FusedCoreCount+1; i++) {
    j = 1;
    if (!((BOOLEAN) (EnabledCores) & (j << i))) {
      EnabledCoreCount++;
    }
  }

  *EnabledCoreCountPtr = EnabledCoreCount;
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
F14DisablePstate (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       UINT8 StateNumber,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64 MsrReg;

  ASSERT (StateNumber < NM_PS_REG);
  LibAmdMsrRead (PS_REG_BASE + (UINT32) StateNumber, &MsrReg, StdHeader);
  ((PSTATE_MSR *) &MsrReg)->PsEnable = 0;
  LibAmdMsrWrite (PS_REG_BASE + (UINT32) StateNumber, &MsrReg, StdHeader);
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
F14TransitionPstate (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       UINT8              StateNumber,
  IN       BOOLEAN            WaitForTransition,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64 MsrReg;

  ASSERT (StateNumber < NM_PS_REG);
  LibAmdMsrRead (PS_REG_BASE + (UINT32) StateNumber, &MsrReg, StdHeader);
  ASSERT (((PSTATE_MSR *) &MsrReg)->PsEnable == 1);
  LibAmdMsrRead (MSR_PSTATE_CTL, &MsrReg, StdHeader);
  ((PSTATE_CTRL_MSR *) &MsrReg)->PstateCmd = (UINT64) StateNumber;
  LibAmdMsrWrite (MSR_PSTATE_CTL, &MsrReg, StdHeader);
  if (WaitForTransition) {
    do {
      LibAmdMsrRead (MSR_PSTATE_STS, &MsrReg, StdHeader);
    } while (((PSTATE_STS_MSR *) &MsrReg)->CurPstate != (UINT64) StateNumber);
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
F14GetTscRate (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
     OUT   UINT32 *FrequencyInMHz,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64 MsrReg;
  PSTATE_CPU_FAMILY_SERVICES  *FamilyServices;

  FamilyServices = NULL;
  GetFeatureServicesOfCurrentCore (&PstateFamilyServiceTable, (const VOID **)&FamilyServices, StdHeader);
  ASSERT (FamilyServices != NULL);

  LibAmdMsrRead (0xC0010015, &MsrReg, StdHeader);
  if ((MsrReg & 0x01000000) != 0) {
    return (FamilyServices->GetPstateFrequency (FamilyServices, 0, FrequencyInMHz, StdHeader));
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
 *  @param[in]   StdHeader                Header for library and services
 *
 *  @retval      AGESA_SUCCESS     Always succeeds.
 */
AGESA_STATUS
F14GetCurrentNbFrequency (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
     OUT   UINT32 *FrequencyInMHz,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 PciReg;
  UINT32 MainPllFid;
  PCI_ADDR PciAddress;

  PciAddress.AddressValue = CPTC0_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, PciAddress, &PciReg, StdHeader);

  if (((CLK_PWR_TIMING_CTRL_REGISTER *) &PciReg)->MainPllOpFreqIdEn == 1) {
    MainPllFid = ((CLK_PWR_TIMING_CTRL_REGISTER *) &PciReg)->MainPllOpFreqId;
  } else {
    MainPllFid = 0;
  }

  *FrequencyInMHz = ((MainPllFid + 0x10) * 100);

  ASSERT (*FrequencyInMHz <= 4000);

  return (AGESA_SUCCESS);
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Determines the NB clock on the desired node.
 *
 *  @CpuServiceMethod{::F_CPU_GET_NB_PSTATE_INFO}.
 *
 *  @param[in]     FamilySpecificServices  The current Family Specific Services.
 *  @param[in]     PlatformConfig          Platform profile/build option config structure.
 *  @param[in]     PciAddress              The segment, bus, and device numbers of the CPU in question.
 *  @param[in]     NbPstate                The NB P-state number to check.
 *  @param[out]    FreqNumeratorInMHz      The desired node's frequency numerator in megahertz.
 *  @param[out]    FreqDivisor             The desired node's frequency divisor.
 *  @param[out]    VoltageInuV             The desired node's voltage in microvolts.
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 *  @retval        TRUE                    NbPstate is valid
 *  @retval        FALSE                   NbPstate is disabled or invalid
 */
BOOLEAN
F14GetNbPstateInfo (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       PCI_ADDR               *PciAddress,
  IN       UINT32                 NbPstate,
     OUT   UINT32                 *FreqNumeratorInMHz,
     OUT   UINT32                 *FreqDivisor,
     OUT   UINT32                 *VoltageInuV,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32   NbVid;
  UINT32   PciReg;
  UINT32   MainPllFreq;
  BOOLEAN  PstateIsValid;

  PstateIsValid = FALSE;
  if ((NbPstate == 0) || ((NbPstate == 1) && FamilySpecificServices->IsNbPstateEnabled (FamilySpecificServices, PlatformConfig, StdHeader))) {
    FamilySpecificServices->GetCurrentNbFrequency (FamilySpecificServices, &MainPllFreq, StdHeader);
    *FreqNumeratorInMHz = (MainPllFreq * 4);
    if (NbPstate == 0) {
      PciAddress->Address.Function = FUNC_3;
      PciAddress->Address.Register = CPTC2_REG;
      LibAmdPciRead (AccessWidth32, *PciAddress, &PciReg, StdHeader);
      *FreqDivisor = ((CLK_PWR_TIMING_CTRL2_REGISTER *) &PciReg)->NbPs0NclkDiv;
      NbVid = ((CLK_PWR_TIMING_CTRL2_REGISTER *) &PciReg)->NbPs0Vid;
    } else {
      PciAddress->Address.Function = FUNC_6;
      PciAddress->Address.Register = NB_PSTATE_CFG_LOW_REG;
      LibAmdPciRead (AccessWidth32, *PciAddress, &PciReg, StdHeader);
      *FreqDivisor = ((NB_PSTATE_CFG_LOW_REGISTER *) &PciReg)->NbPs1NclkDiv;
      NbVid = ((NB_PSTATE_CFG_LOW_REGISTER *) &PciReg)->NbPs1Vid;
    }
    *VoltageInuV = (1550000 - (12500 * NbVid));
    PstateIsValid = TRUE;
  }
  return PstateIsValid;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Is the Northbridge PState feature enabled?
 *
 * @CpuServiceMethod{::F_IS_NB_PSTATE_ENABLED}.
 *
 * @param[in]      FamilySpecificServices         The current Family Specific Services.
 * @param[in]      PlatformConfig                 Platform profile/build option config structure.
 * @param[in]      StdHeader                      Handle of Header for calling lib functions and services.
 *
 * @retval         TRUE                           The NB PState feature is enabled.
 * @retval         FALSE                          The NB PState feature is not enabled.
 */
BOOLEAN
F14IsNbPstateEnabled (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32         PciReg;
  PCI_ADDR       PciAddress;

  PciAddress.AddressValue = NB_PSTATE_CFG_LOW_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, PciAddress, &PciReg, StdHeader);
  return ((BOOLEAN) (((NB_PSTATE_CFG_LOW_REGISTER *) &PciReg)->NbPsCap == 1));
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Returns whether or not BIOS is responsible for configuring the NB COFVID.
 *
 *  @CpuServiceMethod{::F_CPU_IS_NBCOF_INIT_NEEDED}.
 *
 *  @param[in]   FamilySpecificServices   The current Family Specific Services.
 *  @param[in]   PciAddress               The northbridge to query by pci base address.
 *  @param[out]  NbCofVidUpdateRequired   TRUE, perform northbridge frequency and voltage config,
 *                                        FALSE, do not configure them.
 *  @param[in]   StdHeader                Header for library and services
 *
 *  @retval      AGESA_SUCCESS           Always succeeds.
 */
AGESA_STATUS
F14GetNbCofVidUpdate (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       PCI_ADDR *PciAddress,
     OUT   BOOLEAN *NbCofVidUpdateRequired,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  *NbCofVidUpdateRequired = FALSE;
  return (AGESA_SUCCESS);
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
F14LaunchApCore (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       UINT32 SocketNum,
  IN       UINT32 ModuleNum,
  IN       UINT32 CoreNum,
  IN       UINT32 PrimaryCoreNum,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32    NodeRelativeCoreNum;
  UINT32    PciReg;
  PCI_ADDR  PciAddress;
  BOOLEAN   LaunchFlag;

  // Code Start
  LaunchFlag = FALSE;
  NodeRelativeCoreNum = CoreNum - PrimaryCoreNum;
  PciAddress.AddressValue = MAKE_SBDFO (0, 0, PCI_DEV_BASE, FUNC_0, 0);

  switch (NodeRelativeCoreNum) {
  case 1:
    PciAddress.Address.Register = HT_TRANS_CTRL;
    LibAmdPciRead (AccessWidth32, PciAddress, &PciReg, StdHeader);
    if ((PciReg & HT_TRANS_CTRL_CPU1_EN) == 0) {
      PciReg |= HT_TRANS_CTRL_CPU1_EN;
      LibAmdPciWrite (AccessWidth32, PciAddress, &PciReg, StdHeader);
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
F14GetPlatformTypeSpecificInfo (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN OUT   PLATFORM_FEATS         *Features,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  return (AGESA_SUCCESS);
}


/*---------------------------------------------------------------------------------------*/
/**
 *  Get CPU pstate current.
 *
 *  @CpuServiceMethod{::F_CPU_GET_IDD_MAX}.
 *
 *    This function returns the ProcIddMax.
 *
 *  @param[in]     FamilySpecificServices    The current Family Specific Services.
 *  @param[in]     Pstate                    The P-state to check.
 *  @param[out]    ProcIddMax                P-state current in mA.
 *  @param[in]     StdHeader                 Handle of Header for calling lib functions and services.
 *
 *  @retval        TRUE                      P-state is enabled
 *  @retval        FALSE                     P-state is disabled
 */
BOOLEAN
F14GetProcIddMax (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       UINT8                  Pstate,
     OUT   UINT32                 *ProcIddMax,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32       IddDiv;
  UINT32       CmpCap;
  UINT32       PciReg;
  UINT32       MsrAddress;
  UINT64       PstateMsr;
  BOOLEAN      IsPstateEnabled;
  PCI_ADDR     PciAddress;

  IsPstateEnabled = FALSE;

  MsrAddress = (UINT32) (Pstate + PS_REG_BASE);

  ASSERT (MsrAddress <= PS_MAX_REG);

  LibAmdMsrRead (MsrAddress, &PstateMsr, StdHeader);
  if (((PSTATE_MSR *) &PstateMsr)->PsEnable == 1) {
    PciAddress.AddressValue = NB_CAPS_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &PciReg, StdHeader); // F3xE8
    CmpCap = (UINT32) (((NB_CAPS_REGISTER *) &PciReg)->CmpCap);
    CmpCap++;

    switch (((PSTATE_MSR *) &PstateMsr)->IddDiv) {
    case 0:
      IddDiv = 1000;
      break;
    case 1:
      IddDiv = 100;
      break;
    case 2:
      IddDiv = 10;
      break;
    default:  // IddDiv = 3 is reserved. Use 10
      ASSERT (FALSE);
      IddDiv = 10;
      break;
    }

    *ProcIddMax = (UINT32) ((PSTATE_MSR *) &PstateMsr)->IddValue * IddDiv * CmpCap;
    IsPstateEnabled = TRUE;
  }
  return IsPstateEnabled;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Get number of processor cores to be used in determining the brand string.
 *
 * @CpuServiceMethod{::F_CPU_NUMBER_OF_BRANDSTRING_CORES}.
 *
 * @param[in]      FamilySpecificServices         The current Family Specific Services.
 * @param[in]      StdHeader                      Handle of Header for calling lib functions and services.
 *
 * @return         The number of cores to be used in brand string calculation.
 */
UINT8
F14GetNumberOfCoresForBrandstring (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  CPUID_DATA    CpuId;

  //
  //CPUID.80000008h.ECX.NC + 1, 000b = 1, 001b = 2, etc.
  //
  LibAmdCpuidRead (CPUID_LONG_MODE_ADDR, &CpuId, StdHeader);
  return ((UINT8) ((CpuId.ECX_Reg & 0xff) + 1));
}
