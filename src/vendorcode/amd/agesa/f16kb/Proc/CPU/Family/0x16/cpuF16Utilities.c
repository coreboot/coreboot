/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_16 specific utility functions.
 *
 * Provides numerous utility functions specific to family 16h.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x16
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
#include "Ids.h"
#include "cpuRegisters.h"
#include "cpuFamilyTranslation.h"
#include "cpuPstateTables.h"
#include "cpuF16PowerMgmt.h"
#include "cpuApicUtilities.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "cpuF16Utilities.h"
#include "cpuEarlyInit.h"
#include "cpuPostInit.h"
#include "cpuFeatures.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)
#define FILECODE PROC_CPU_FAMILY_0X16_CPUF16UTILITIES_FILECODE

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

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

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
F16DisablePstate (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       UINT8 StateNumber,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64 LocalMsrRegister;

  ASSERT (StateNumber < NM_PS_REG);
  LibAmdMsrRead (PS_REG_BASE + (UINT32) StateNumber, &LocalMsrRegister, StdHeader);
  ((F16_PSTATE_MSR *) &LocalMsrRegister)->PsEnable = 0;
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
F16TransitionPstate (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       UINT8              StateNumber,
  IN       BOOLEAN            WaitForTransition,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64       LocalMsrRegister;

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
F16GetTscRate (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
     OUT   UINT32 *FrequencyInMHz,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8        NumBoostStates;
  UINT32       LocalPciRegister;
  UINT64       LocalMsrRegister;
  PCI_ADDR     PciAddress;
  PSTATE_CPU_FAMILY_SERVICES  *FamilyServices;

  LibAmdMsrRead (0xC0010015, &LocalMsrRegister, StdHeader);
  if ((LocalMsrRegister & 0x01000000) != 0) {
    FamilyServices = NULL;
    GetFeatureServicesOfCurrentCore (&PstateFamilyServiceTable, (CONST VOID **)&FamilyServices, StdHeader);
    ASSERT (FamilyServices != NULL);
    PciAddress.AddressValue = MAKE_SBDFO (0, 0, 24, FUNC_4, CPB_CTRL_REG);
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    NumBoostStates = (UINT8) ((F16_CPB_CTRL_REGISTER *) &LocalPciRegister)->NumBoostStates;
    return (FamilyServices->GetPstateFrequency (FamilyServices, NumBoostStates, FrequencyInMHz, StdHeader));
  } else {
    return (FamilySpecificServices->GetCurrentNbFrequency (FamilySpecificServices, FrequencyInMHz, StdHeader));
  }
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
F16LaunchApCore (
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
  UINT32    LowCore;
  UINT32    HighCore;
  PCI_ADDR  PciAddress;
  BOOLEAN   LaunchFlag;

  NodeRelativeCoreNum = CoreNum - PrimaryCoreNum;
  PciAddress.Address.Bus = 0;
  PciAddress.Address.Device = 0x18;
  PciAddress.Address.Function = FUNC_0;
  PciAddress.Address.Register = CORE_CTRL;
  LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);

  GetGivenModuleCoreRange (0, 0, &LowCore, &HighCore, StdHeader);
  ASSERT ((NodeRelativeCoreNum != 0) || (NodeRelativeCoreNum <= HighCore))

  if ((LocalPciRegister & (1 << NodeRelativeCoreNum)) == 0) {
    LocalPciRegister |= (1 << NodeRelativeCoreNum);
    LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    LaunchFlag = TRUE;
  } else {
    LaunchFlag = FALSE;
  }

  return (LaunchFlag);
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Returns whether or not BIOS is responsible for configuring the NB COFVID.
 *
 *  @CpuServiceMethod{::F_CPU_IS_NBCOF_INIT_NEEDED}.
 *
 *  @param[in]   FamilySpecificServices   The current Family Specific Services.
 *  @param[in]   PciAddress               The northbridge to query by pci base address.
 *  @param[out]  NbVidUpdateAll           Do all NbVids need to be updated
 *  @param[in]   StdHeader                Header for library and services
 *
 *  @retval      TRUE                    Perform northbridge frequency and voltage config.
 *  @retval      FALSE                   Do not configure them.
 */
BOOLEAN
F16GetNbCofVidUpdate (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       PCI_ADDR *PciAddress,
     OUT   BOOLEAN *NbVidUpdateAll,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  *NbVidUpdateAll = FALSE;
  return FALSE;
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
F16IsNbPstateEnabled (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32   LocalPciRegister;
  PCI_ADDR PciAddress;
  BOOLEAN  PowerMode;
  BOOLEAN  SkipHwCfg;

  SkipHwCfg = FALSE;

  IDS_OPTION_HOOK (IDS_NBPSDIS_OVERRIDE, &SkipHwCfg, StdHeader);

  // Defaults to Power Optimized Mode
  PowerMode = TRUE;

  // If system is optimized for performance, disable NB P-States
  if (PlatformConfig->PlatformProfile.PlatformPowerPolicy == Performance) {
    PowerMode = FALSE;
  }

  PciAddress.AddressValue = F16_NB_PSTATE_CTRL_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
  if ((((((F16_NB_PSTATE_CTRL_REGISTER *) &LocalPciRegister)->NbPstateMaxVal != 0) &&
      (!IsNonCoherentHt1 (StdHeader))) || SkipHwCfg) && (PowerMode)) {
    return TRUE;
  }
  return FALSE;
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Set warm reset status and count
 *
 *  @CpuServiceMethod{::F_CPU_SET_WARM_RESET_FLAG}.
 *
 *  This function will use bit9, and bit 10 of register F0x6C as a warm reset status and count.
 *
 *  @param[in]  FamilySpecificServices   The current Family Specific Services.
 *  @param[in]  StdHeader                Handle of Header for calling lib functions and services.
 *  @param[in]  Request                  Indicate warm reset status
 *
 */
VOID
F16SetAgesaWarmResetFlag (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN       WARM_RESET_REQUEST *Request
  )
{
  PCI_ADDR  PciAddress;
  UINT32    PciData;

  PciAddress.AddressValue = MAKE_SBDFO (0, 0 , PCI_DEV_BASE, FUNC_0, HT_INIT_CTRL);
  LibAmdPciRead (AccessWidth32, PciAddress, &PciData, StdHeader);

  // bit[5] - indicate a warm reset is or is not required
  PciData &= ~(HT_INIT_BIOS_RST_DET_0);
  PciData = PciData | (Request->RequestBit << 5);

  // bit[10,9] - indicate warm reset status and count
  PciData &= ~(HT_INIT_BIOS_RST_DET_1 | HT_INIT_BIOS_RST_DET_2);
  PciData |= Request->StateBits << 9;

  LibAmdPciWrite (AccessWidth32, PciAddress, &PciData, StdHeader);
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Get warm reset status and count
 *
 *  @CpuServiceMethod{::F_CPU_GET_WARM_RESET_FLAG}.
 *
 *  This function will bit9, and bit 10 of register F0x6C as a warm reset status and count.
 *
 *  @param[in]  FamilySpecificServices   The current Family Specific Services.
 *  @param[in]  StdHeader                Config handle for library and services
 *  @param[out] Request                  Indicate warm reset status
 *
 */
VOID
F16GetAgesaWarmResetFlag (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       AMD_CONFIG_PARAMS *StdHeader,
     OUT   WARM_RESET_REQUEST *Request
  )
{
  PCI_ADDR  PciAddress;
  UINT32    PciData;

  PciAddress.AddressValue = MAKE_SBDFO (0, 0 , PCI_DEV_BASE, FUNC_0, HT_INIT_CTRL);
  LibAmdPciRead (AccessWidth32, PciAddress, &PciData, StdHeader);

  // bit[5] - indicate a warm reset is or is not required
  Request->RequestBit = (UINT8) ((PciData & HT_INIT_BIOS_RST_DET_0) >> 5);
  // bit[10,9] - indicate warm reset status and count
  Request->StateBits = (UINT8) ((PciData & (HT_INIT_BIOS_RST_DET_1 | HT_INIT_BIOS_RST_DET_2)) >> 9);
}

/*---------------------------------------------------------------------------------------*/
/**
 * Return a number zero or one, based on the Core ID position in the initial APIC Id.
 *
 * @CpuServiceMethod{::F_CORE_ID_POSITION_IN_INITIAL_APIC_ID}.
 *
 * @param[in]     FamilySpecificServices  The current Family Specific Services.
 * @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 * @retval        CoreIdPositionZero      Core Id is not low
 * @retval        CoreIdPositionOne       Core Id is low
 */
CORE_ID_POSITION
F16CpuAmdCoreIdPositionInInitialApicId (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT64 InitApicIdCpuIdLo;

  //  Check bit_54 [InitApicIdCpuIdLo] to find core id position.
  LibAmdMsrRead (MSR_NB_CFG, &InitApicIdCpuIdLo, StdHeader);
  InitApicIdCpuIdLo = ((InitApicIdCpuIdLo & BIT54) >> 54);
  return ((InitApicIdCpuIdLo == 0) ? CoreIdPositionZero : CoreIdPositionOne);
}


