/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_10 revision Cx specific utility functions.
 *
 * Provides numerous utility functions specific to family 10h rev C.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/F10
 * @e \$Revision: 56279 $   @e \$Date: 2011-07-11 13:11:28 -0600 (Mon, 11 Jul 2011) $
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
#include "Ids.h"
#include "cpuRegisters.h"
#include "cpuFamilyTranslation.h"
#include "cpuF10PowerMgmt.h"
#include "GeneralServices.h"
#include "cpuEarlyInit.h"
#include "cpuPostInit.h"
#include "cpuFeatures.h"
#include "OptionMultiSocket.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G2_PEI)
#define FILECODE PROC_CPU_FAMILY_0X10_REVC_F10REVCUTILITIES_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern OPTION_MULTISOCKET_CONFIGURATION OptionMultiSocketConfiguration;
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
 * Set down core register on a revision C processor.
 *
 * This function set F3x190 Downcore Control Register[5:0]
 *
 * @param[in]   FamilySpecificServices   The current Family Specific Services.
 * @param[in]   Socket                   Socket ID.
 * @param[in]   Module                   Module ID in socket.
 * @param[in]   LeveledCores             Number of core.
 * @param[in]   CoreLevelMode            Core level mode.
 * @param[in]   StdHeader                Header for library and services.
 *
 * @retval      TRUE                     Down Core register is updated.
 * @retval      FALSE                    Down Core register is not updated.
 */
BOOLEAN
F10CommonRevCSetDownCoreRegister (
  IN       CPU_CORE_LEVELING_FAMILY_SERVICES *FamilySpecificServices,
  IN       UINT32 *Socket,
  IN       UINT32 *Module,
  IN       UINT32 *LeveledCores,
  IN       CORE_LEVELING_TYPE CoreLevelMode,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32    TempVar32_a;
  UINT32    CoreDisableBits;
  PCI_ADDR  PciAddress;
  BOOLEAN   IsUpdated;
  AGESA_STATUS AgesaStatus;

  IsUpdated = FALSE;

  switch (*LeveledCores) {
  case 1:
    CoreDisableBits = DOWNCORE_MASK_SINGLE;
    break;
  case 2:
    CoreDisableBits = DOWNCORE_MASK_DUAL;
    break;
  case 3:
    CoreDisableBits = DOWNCORE_MASK_TRI;
    break;
  default:
    CoreDisableBits = 0;
    break;
  }

  if (CoreDisableBits != 0) {
    if (GetPciAddress (StdHeader, (UINT8) *Socket, (UINT8) *Module, &PciAddress, &AgesaStatus)) {
      PciAddress.Address.Function = FUNC_3;
      PciAddress.Address.Register = NORTH_BRIDGE_CAPABILITIES_REG;

      LibAmdPciRead (AccessWidth32, PciAddress, &TempVar32_a, StdHeader);
      TempVar32_a = (TempVar32_a >> 12) & 0x3;
      if (TempVar32_a == 0) {
        CoreDisableBits &= 0x1;
      } else if (TempVar32_a == 1) {
        CoreDisableBits &= 0x3;
      } else if (TempVar32_a == 2) {
        CoreDisableBits &= 0x7;
      } else if (TempVar32_a == 3) {
        CoreDisableBits &= 0x0F;
      }
      PciAddress.Address.Register = DOWNCORE_CTRL;
      LibAmdPciRead (AccessWidth32, PciAddress, &TempVar32_a, StdHeader);
      if ((TempVar32_a | CoreDisableBits) != TempVar32_a) {
        TempVar32_a |= CoreDisableBits;
        LibAmdPciWrite (AccessWidth32, PciAddress, &TempVar32_a, StdHeader);
        IsUpdated = TRUE;
      }
    }
  }

  return IsUpdated;
}


CONST CPU_CORE_LEVELING_FAMILY_SERVICES ROMDATA F10RevCCoreLeveling =
{
  0,
  F10CommonRevCSetDownCoreRegister
};


/*---------------------------------------------------------------------------------------*/
/**
 *  Get CPU pstate current on a revision C processor.
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
F10CommonRevCGetProcIddMax (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       UINT8                  Pstate,
     OUT   UINT32                 *ProcIddMax,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32       IddDiv;
  UINT32       CmpCap;
  UINT32       LocalPciRegister;
  UINT32       MsrAddress;
  UINT32       SinglePlaneNbIdd;
  UINT64       PstateMsr;
  BOOLEAN      IsPstateEnabled;
  PCI_ADDR     PciAddress;

  IsPstateEnabled = FALSE;

  MsrAddress = (UINT32) (Pstate + PS_REG_BASE);

  ASSERT (MsrAddress <= PS_MAX_REG);

  LibAmdMsrRead (MsrAddress, &PstateMsr, StdHeader);
  if (((PSTATE_MSR *) &PstateMsr)->PsEnable == 1) {
    OptionMultiSocketConfiguration.GetCurrPciAddr (&PciAddress, StdHeader);

    PciAddress.Address.Function = FUNC_3;
    PciAddress.Address.Register = NB_CAPS_REG;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader); // F3xE8
    CmpCap = (UINT32) (((NB_CAPS_REGISTER *) &LocalPciRegister)->CmpCapLo);
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

    PciAddress.Address.Register = PW_CTL_MISC_REG;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader); // F3xE8
    if (((POWER_CTRL_MISC_REGISTER *) &LocalPciRegister)->PviMode == 1) {
      *ProcIddMax = (UINT32) ((PSTATE_MSR *) &PstateMsr)->IddValue * IddDiv * CmpCap;
    } else {
      PciAddress.Address.Register = PRCT_INFO_REG;
      LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader); // F3xE8
      SinglePlaneNbIdd = ((PRODUCT_INFO_REGISTER *) &LocalPciRegister)->SinglePlaneNbIdd;
      SinglePlaneNbIdd <<= 1;
      *ProcIddMax = ((UINT32) ((PSTATE_MSR *) &PstateMsr)->IddValue * IddDiv * CmpCap) - SinglePlaneNbIdd;
    }
    IsPstateEnabled = TRUE;
  }
  return IsPstateEnabled;
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
F10CommonRevCGetNbCofVidUpdate (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       PCI_ADDR *PciAddress,
     OUT   BOOLEAN *NbVidUpdateAll,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 ProductInfoRegister;

  PciAddress->Address.Register = PRCT_INFO_REG;
  PciAddress->Address.Function = FUNC_3;
  LibAmdPciRead (AccessWidth32, *PciAddress, &ProductInfoRegister, StdHeader);
  *NbVidUpdateAll = (BOOLEAN) (((PRODUCT_INFO_REGISTER *) &ProductInfoRegister)->NbVidUpdateAll == 1);
  return (BOOLEAN) (((PRODUCT_INFO_REGISTER *) &ProductInfoRegister)->NbCofVidUpdate == 1);
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
F10CommonRevCGetNbPstateInfo (
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
  UINT32   NbFid;
  UINT32   NbVid;
  UINT32   LocalPciRegister;
  UINT32   ProductInfoRegister;
  UINT64   LocalMsrRegister;
  BOOLEAN  PstateIsValid;

  PstateIsValid = TRUE;
  if (NbPstate == 0) {
    *FreqDivisor = 1;
  } else if ((NbPstate == 1) && FamilySpecificServices->IsNbPstateEnabled (FamilySpecificServices, PlatformConfig, StdHeader)) {
    *FreqDivisor = 2;
  } else {
    PstateIsValid = FALSE;
  }
  if (PstateIsValid) {
    PciAddress->Address.Function = FUNC_3;
    PciAddress->Address.Register = PRCT_INFO_REG;
    LibAmdPciRead (AccessWidth32, *PciAddress, &ProductInfoRegister, StdHeader);
    if ((((PRODUCT_INFO_REGISTER *) &ProductInfoRegister)->NbCofVidUpdate) == 0) {
      PciAddress->Address.Register = CPTC0_REG;
      LibAmdPciRead (AccessWidth32, *PciAddress, &LocalPciRegister, StdHeader);
      NbFid = ((CLK_PWR_TIMING_CTRL_REGISTER *) &LocalPciRegister)->NbFid;
      LibAmdMsrRead (MSR_COFVID_STS, &LocalMsrRegister, StdHeader);
      NbVid = (UINT32) ((COFVID_STS_MSR *) &LocalMsrRegister)->CurNbVid;
    } else {
      NbFid = ((PRODUCT_INFO_REGISTER *) &ProductInfoRegister)->SinglePlaneNbFid;
      NbVid = ((PRODUCT_INFO_REGISTER *) &ProductInfoRegister)->SinglePlaneNbVid;
      PciAddress->Address.Register = PW_CTL_MISC_REG;
      LibAmdPciRead (AccessWidth32, *PciAddress, &LocalPciRegister, StdHeader);
      if (((POWER_CTRL_MISC_REGISTER *) &LocalPciRegister)->PviMode == 0) {
        NbFid += ((PRODUCT_INFO_REGISTER *) &ProductInfoRegister)->DualPlaneNbFidOff;
        NbVid -= ((PRODUCT_INFO_REGISTER *) &ProductInfoRegister)->DualPlaneNbVidOff;
      }
    }
    *FreqNumeratorInMHz = ((NbFid + 4) * 200);
    *VoltageInuV = (1550000 - (12500 * NbVid));
  }
  return PstateIsValid;
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Returns the node's minimum and maximum northbridge frequency.
 *
 *  @CpuServiceMethod{::F_CPU_GET_MIN_MAX_NB_FREQ}.
 *
 *  @param[in]     FamilySpecificServices  The current Family Specific Services.
 *  @param[in]     PlatformConfig          Platform profile/build option config structure.
 *  @param[in]     PciAddress              The segment, bus, and device numbers of the CPU in question.
 *  @param[out]    MinFreqInMHz            The node's miminum northbridge frequency.
 *  @param[out]    MaxFreqInMHz            The node's maximum northbridge frequency.
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 *  @retval        AGESA_STATUS            Northbridge frequency is valid
 */
AGESA_STATUS
F10RevCGetMinMaxNbFrequency (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       PCI_ADDR               *PciAddress,
     OUT   UINT32                 *MinFreqInMHz,
     OUT   UINT32                 *MaxFreqInMHz,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32         NbPstateEn;
  UINT32         NbFid;
  UINT32         FreqDivisor;
  UINT32         FreqNumerator;
  UINT32         LocalPciRegister;
  UINT32         ProductInfoRegister;
  CPU_LOGICAL_ID LogicalId;

  GetLogicalIdOfCurrentCore (&LogicalId, StdHeader);

  FreqDivisor = 1;

  // If NB P-state is supported, return the frequency of NB P-state 1
  if ((PlatformConfig->PlatformProfile.PlatformPowerPolicy != Performance) &&
     ((LogicalId.Revision & AMD_F10_C3) != 0)) {
    PciAddress->Address.Function = FUNC_3;
    PciAddress->Address.Register = 0x1F0;
    LibAmdPciReadBits (*PciAddress, 18, 16, &NbPstateEn, StdHeader);

    if (NbPstateEn != 0) {
      FreqDivisor = 2;
    }
  }

  PciAddress->Address.Function = FUNC_3;
  PciAddress->Address.Register = PRCT_INFO_REG;
  LibAmdPciRead (AccessWidth32, *PciAddress, &ProductInfoRegister, StdHeader);

  if ((((PRODUCT_INFO_REGISTER *) &ProductInfoRegister)->NbCofVidUpdate) == 0) {
    PciAddress->Address.Register = CPTC0_REG;
    LibAmdPciRead (AccessWidth32, *PciAddress, &LocalPciRegister, StdHeader);
    NbFid = ((CLK_PWR_TIMING_CTRL_REGISTER *) &LocalPciRegister)->NbFid;
  } else {
    NbFid = ((PRODUCT_INFO_REGISTER *) &ProductInfoRegister)->SinglePlaneNbFid;
    PciAddress->Address.Register = PW_CTL_MISC_REG;
    LibAmdPciRead (AccessWidth32, *PciAddress, &LocalPciRegister, StdHeader);
    if (((POWER_CTRL_MISC_REGISTER *) &LocalPciRegister)->PviMode == 0) {
      NbFid += ((PRODUCT_INFO_REGISTER *) &ProductInfoRegister)->DualPlaneNbFidOff;
    }
  }

  FreqNumerator = ((NbFid + 4) * 200);
  *MaxFreqInMHz = FreqNumerator;
  *MinFreqInMHz = (FreqNumerator / FreqDivisor);

  return AGESA_SUCCESS;

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
F10CommonRevCIsNbPstateEnabled (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32         NbPstate;
  PCI_ADDR       PciAddress;
  CPU_LOGICAL_ID LogicalId;
  BOOLEAN        Result;

  Result = FALSE;

  GetLogicalIdOfCurrentCore (&LogicalId, StdHeader);
  if (((LogicalId.Revision & AMD_F10_C3) != 0) && (!IsNonCoherentHt1 (StdHeader))) {
    OptionMultiSocketConfiguration.GetCurrPciAddr (&PciAddress, StdHeader);
    PciAddress.Address.Function = FUNC_3;
    PciAddress.Address.Register = 0x1F0;
    LibAmdPciReadBits (PciAddress, 18, 16, &NbPstate, StdHeader);
    if (NbPstate != 0) {
      Result = TRUE;
    }
  }
  return Result;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Get the number of physical cores of current processor.
 *
 * @CpuServiceMethod{::F_CPU_NUMBER_OF_PHYSICAL_CORES}.
 *
 * @param[in]      FamilySpecificServices         The current Family Specific Services.
 * @param[in]      StdHeader                      Handle of Header for calling lib functions and services.
 *
 * @return         The number of physical cores.
 */
UINT8
F10CommonRevCGetNumberOfPhysicalCores (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32       LocalPciRegister;
  PCI_ADDR     PciAddress;

  OptionMultiSocketConfiguration.GetCurrPciAddr (&PciAddress, StdHeader);
  PciAddress.Address.Function = FUNC_3;
  PciAddress.Address.Register = NB_CAPS_REG;
  LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
  return (UINT8) (((NB_CAPS_REGISTER *) &LocalPciRegister)->CmpCapLo + 1);
}

