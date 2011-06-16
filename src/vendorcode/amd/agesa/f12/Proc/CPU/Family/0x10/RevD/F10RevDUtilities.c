/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_10 revision Dx specific utility functions.
 *
 * Provides numerous utility functions specific to family 10h rev D.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/F10
 * @e \$Revision: 48937 $   @e \$Date: 2011-03-15 03:37:15 +0800 (Tue, 15 Mar 2011) $
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
#include "cpuF10PowerMgmt.h"
#include "GeneralServices.h"
#include "cpuEarlyInit.h"
#include "cpuRegisters.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_CPU_FAMILY_0X10_REVD_F10REVDUTILITIES_FILECODE

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

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * Set down core register on a revision D processor.
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
F10CommonRevDSetDownCoreRegister (
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
  case 4:
    CoreDisableBits = DOWNCORE_MASK_FOUR;
    break;
  case 5:
    CoreDisableBits = DOWNCORE_MASK_FIVE;
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
      TempVar32_a = ((TempVar32_a >> 12) & 0x3) | ((TempVar32_a >> 13) & 0x4);
      if (TempVar32_a == 0) {
        CoreDisableBits &= 0x1;
      } else if (TempVar32_a == 1) {
        CoreDisableBits &= 0x3;
      } else if (TempVar32_a == 2) {
        CoreDisableBits &= 0x7;
      } else if (TempVar32_a == 3) {
        CoreDisableBits &= 0x0F;
      } else if (TempVar32_a == 4) {
        CoreDisableBits &= 0x1F;
      } else if (TempVar32_a == 5) {
        CoreDisableBits &= 0x3F;
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


CONST CPU_CORE_LEVELING_FAMILY_SERVICES ROMDATA F10RevDCoreLeveling =
{
  0,
  F10CommonRevDSetDownCoreRegister
};

/*---------------------------------------------------------------------------------------*/
/**
 *  Get CPU pstate current on a revision D processor.
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
F10CommonRevDGetProcIddMax (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       UINT8                  Pstate,
     OUT   UINT32                 *ProcIddMax,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32       IddDiv;
  UINT32       CmpCap;
  UINT32       MultiNodeCpu;
  UINT32       NbCaps;
  UINT32       Socket;
  UINT32       Module;
  UINT32       Ignored;
  UINT32       MsrAddress;
  UINT64       PstateMsr;
  BOOLEAN      IsPstateEnabled;
  PCI_ADDR     PciAddress;
  AGESA_STATUS IgnoredSts;

  IsPstateEnabled = FALSE;

  MsrAddress = (UINT32) (Pstate + PS_REG_BASE);
  ASSERT (MsrAddress <= PS_MAX_REG);

  LibAmdMsrRead (MsrAddress, &PstateMsr, StdHeader);
  if (((PSTATE_MSR *) &PstateMsr)->PsEnable == 1) {
    IdentifyCore (StdHeader, &Socket, &Module, &Ignored, &IgnoredSts);
    GetPciAddress (StdHeader, Socket, Module, &PciAddress, &IgnoredSts);
    PciAddress.Address.Function = FUNC_3;
    PciAddress.Address.Register = NB_CAPS_REG;
    LibAmdPciRead (AccessWidth32, PciAddress, &NbCaps, StdHeader); // F3xE8

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
      IddDiv = 10;
      break;
    }
    MultiNodeCpu = (UINT32) (((NB_CAPS_REGISTER *) &NbCaps)->MultiNodeCpu + 1);
    CmpCap = (UINT32) (((NB_CAPS_REGISTER *) &NbCaps)->CmpCapHi << 2);
    CmpCap |= (UINT32) (((NB_CAPS_REGISTER *) &NbCaps)->CmpCapLo);
    CmpCap++;
    *ProcIddMax = (UINT32) ((PSTATE_MSR *) &PstateMsr)->IddValue * IddDiv * CmpCap * MultiNodeCpu;
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
F10CommonRevDGetNbCofVidUpdate (
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
F10CommonRevDGetNbPstateInfo (
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
  UINT32   LocalPciRegister;
  UINT64   LocalMsrRegister;
  BOOLEAN  PstateIsValid;

  PstateIsValid = FALSE;
  if (NbPstate == 0) {
    PciAddress->Address.Function = FUNC_3;
    PciAddress->Address.Register = CPTC0_REG;
    LibAmdPciRead (AccessWidth32, *PciAddress, &LocalPciRegister, StdHeader);
    *FreqNumeratorInMHz = ((((CLK_PWR_TIMING_CTRL_REGISTER *) &LocalPciRegister)->NbFid + 4) * 200);
    *FreqDivisor = 1;
    LibAmdMsrRead (MSR_COFVID_STS, &LocalMsrRegister, StdHeader);
    *VoltageInuV = (1550000 - (12500 * ((UINT32) ((COFVID_STS_MSR *) &LocalMsrRegister)->CurNbVid)));
    PstateIsValid = TRUE;
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
 *  @param[out]    MinFreqInMHz            The node's minimum northbridge frequency.
 *  @param[out]    MaxFreqInMHz            The node's maximum northbridge frequency.
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 *  @retval        AGESA_STATUS            Northbridge frequency is valid
 */
AGESA_STATUS
F10RevDGetMinMaxNbFrequency (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       PCI_ADDR               *PciAddress,
     OUT   UINT32                 *MinFreqInMHz,
     OUT   UINT32                 *MaxFreqInMHz,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32       LocalPciRegister;

  PciAddress->Address.Function = FUNC_3;
  PciAddress->Address.Register = CPTC0_REG;
  LibAmdPciRead (AccessWidth32, *PciAddress, &LocalPciRegister, StdHeader);
  *MinFreqInMHz = ((((CLK_PWR_TIMING_CTRL_REGISTER *) &LocalPciRegister)->NbFid + 4) * 200);
  *MaxFreqInMHz = *MinFreqInMHz;

  return AGESA_SUCCESS;
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
F10CommonRevDGetNumberOfPhysicalCores (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32       CmpCap;
  UINT32       CmpCapOnNode;
  UINT32       Socket;
  UINT32       Module;
  UINT32       Core;
  UINT32       LocalPciRegister;
  PCI_ADDR     PciAddress;
  AGESA_STATUS IgnoredSts;

  CmpCap = 0;
  IdentifyCore (StdHeader, &Socket, &Module, &Core, &IgnoredSts);
  for (Module = 0; Module < GetPlatformNumberOfModules (); Module++) {
    if (GetPciAddress (StdHeader, Socket, Module, &PciAddress, &IgnoredSts)) {
      PciAddress.Address.Function = FUNC_3;
      PciAddress.Address.Register = NB_CAPS_REG;
      LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
      CmpCapOnNode = (UINT8) (((NB_CAPS_REGISTER *) &LocalPciRegister)->CmpCapHi << 2);
      CmpCapOnNode |= (UINT8) (((NB_CAPS_REGISTER *) &LocalPciRegister)->CmpCapLo);
      CmpCapOnNode++;
      CmpCap += CmpCapOnNode;
    }
  }
  return ((UINT8) CmpCap);
}

