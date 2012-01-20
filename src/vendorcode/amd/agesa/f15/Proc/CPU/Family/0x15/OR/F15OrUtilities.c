/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 models 0 - 0Fh specific utility functions.
 *
 * Provides numerous utility functions specific to family 15h OR.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x15/OR
 * @e \$Revision: 58928 $   @e \$Date: 2011-09-08 16:43:14 -0600 (Thu, 08 Sep 2011) $
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
#include "cpuFamilyTranslation.h"
#include "cpuF15PowerMgmt.h"
#include "cpuF15OrPowerMgmt.h"
#include "cpuApicUtilities.h"
#include "cpuServices.h"
#include "cpuEarlyInit.h"
#include "GeneralServices.h"
#include "OptionMultiSocket.h"
#include "F15OrUtilities.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)
#define FILECODE PROC_CPU_FAMILY_0X15_OR_F15ORUTILITIES_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern OPTION_MULTISOCKET_CONFIGURATION OptionMultiSocketConfiguration;
/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/**
 * Node ID MSR register fields.
 * Provide the layout of fields in the Node ID MSR.
 */
typedef struct {
  UINT64    NodeId:3;                 ///< The core is on the node with this node id.
  UINT64    NodesPerProcessor:3;      ///< The number of Nodes in this processor.
  UINT64    BiosScratch:6;            ///< BiosScratch, use as the AP core heap index.
  UINT64    :(63 - 11);               ///< Reserved.
} NODE_ID_MSR_FIELDS;

/// Node ID MSR.
typedef union {
  NODE_ID_MSR_FIELDS   Fields;        ///< Access the register as individual fields
  UINT64               Value;         ///< Access the register value.
} NODE_ID_MSR;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

VOID
STATIC
F15OrNbPstateDisCore (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

BOOLEAN
F15OrSetDownCoreRegister (
  IN       CPU_CORE_LEVELING_FAMILY_SERVICES *FamilySpecificServices,
  IN       UINT32 *Socket,
  IN       UINT32 *Module,
  IN       UINT32 *LeveledCores,
  IN       CORE_LEVELING_TYPE CoreLevelMode,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

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
F15OrGetProcIddMax (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       UINT8                  Pstate,
     OUT   UINT32                 *ProcIddMax,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32       IddDiv;
  UINT32       NumberOfPhysicalCores;
  UINT32       MsrAddress;
  UINT64       PstateMsr;
  BOOLEAN      IsPstateEnabled;
  CPUID_DATA   CpuId;

  IsPstateEnabled = FALSE;

  MsrAddress = (UINT32) (Pstate + PS_REG_BASE);
  ASSERT (MsrAddress <= PS_MAX_REG);

  LibAmdMsrRead (MsrAddress, &PstateMsr, StdHeader);
  if (((PSTATE_MSR *) &PstateMsr)->PsEnable == 1) {
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
    LibAmdCpuidRead (AMD_CPUID_ASIZE_PCCOUNT, &CpuId, StdHeader);
    NumberOfPhysicalCores = ((CpuId.ECX_Reg & 0xFF) + 1);

    *ProcIddMax = (UINT32) ((PSTATE_MSR *) &PstateMsr)->IddValue * IddDiv * NumberOfPhysicalCores;
    IsPstateEnabled = TRUE;
  }
  return IsPstateEnabled;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Set down core register on Orochi
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
F15OrSetDownCoreRegister (
  IN       CPU_CORE_LEVELING_FAMILY_SERVICES *FamilySpecificServices,
  IN       UINT32 *Socket,
  IN       UINT32 *Module,
  IN       UINT32 *LeveledCores,
  IN       CORE_LEVELING_TYPE CoreLevelMode,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8     Xbar2SriFreeListCBC;
  UINT8     L3FreeListCBC;
  UINT32    TempVar32_a;
  UINT32    CoreDisableBits;
  UINT32    NumberOfEnabledCores;
  UINT32    NumberOfEnabledCU;
  PCI_ADDR  PciAddress;
  BOOLEAN   IsUpdated;
  AGESA_STATUS                    AgesaStatus;
  NB_CAPS_REGISTER                NbCaps;
  FREE_LIST_BUFFER_COUNT_REGISTER FreeListBufferCount;
  L3_BUFFER_COUNT_REGISTER        L3BufferCnt;

  IsUpdated = FALSE;

  if (CoreLevelMode == CORE_LEVEL_COMPUTE_UNIT) {
    switch (*LeveledCores) {
    case 1:
      CoreDisableBits = DOWNCORE_MASK_SINGLE;
      break;
    case 2:
      CoreDisableBits = DOWNCORE_MASK_DUAL_COMPUTE_UNIT;
      break;
    case 3:
      CoreDisableBits = DOWNCORE_MASK_TRI_COMPUTE_UNIT;
      break;
    case 4:
      CoreDisableBits = DOWNCORE_MASK_FOUR_COMPUTE_UNIT;
      break;
    default:
      CoreDisableBits = 0;
      break;
    }

  } else {
    switch (*LeveledCores) {
    case 1:
      CoreDisableBits = DOWNCORE_MASK_SINGLE;
      break;
    case 2:
      CoreDisableBits = DOWNCORE_MASK_DUAL;
      break;
    case 4:
      CoreDisableBits = DOWNCORE_MASK_FOUR;
      break;
    case 6:
      CoreDisableBits = DOWNCORE_MASK_SIX;
      break;
    default:
      CoreDisableBits = 0;
      break;
    }
  }

  if (CoreDisableBits != 0) {
    if (GetPciAddress (StdHeader, (UINT8) *Socket, (UINT8) *Module, &PciAddress, &AgesaStatus)) {
      PciAddress.Address.Function = FUNC_5;
      PciAddress.Address.Register = NORTH_BRIDGE_CAPABILITIES_2_REG;
      LibAmdPciRead (AccessWidth32, PciAddress, &TempVar32_a, StdHeader);
      TempVar32_a = (TempVar32_a & 0xFF) + 1;
      TempVar32_a = (1 << TempVar32_a) - 1;
      CoreDisableBits &= TempVar32_a;
      NumberOfEnabledCores = ~(CoreDisableBits | ~(TempVar32_a));

      PciAddress.Address.Function = FUNC_3;
      PciAddress.Address.Register = DOWNCORE_CTRL;
      LibAmdPciRead (AccessWidth32, PciAddress, &TempVar32_a, StdHeader);
      if ((TempVar32_a | CoreDisableBits) != TempVar32_a) {
        TempVar32_a |= CoreDisableBits;
        LibAmdPciWrite (AccessWidth32, PciAddress, &TempVar32_a, StdHeader);
        IsUpdated = TRUE;

        for (NumberOfEnabledCU = 0; NumberOfEnabledCores != 0; NumberOfEnabledCores >>= 2) {
          NumberOfEnabledCU += ((NumberOfEnabledCores & 3) != 0) ? 1 : 0;
        }
        switch (NumberOfEnabledCU) {
        case 1:
          Xbar2SriFreeListCBC = 0x16;
          L3FreeListCBC = 0x1C;
          break;
        case 2:
          Xbar2SriFreeListCBC = 0x14;
          L3FreeListCBC = 0x18;
          break;
        case 3:
          Xbar2SriFreeListCBC = 0x12;
          L3FreeListCBC = 0x14;
          break;
        case 4:
          Xbar2SriFreeListCBC = 0x10;
          L3FreeListCBC = 0x10;
          break;
        default:
          Xbar2SriFreeListCBC = 0x16;
          L3FreeListCBC = 0xE;
          break;
        }
        //D18F3x1A0[8:4] L3FreeListCBC:
        //BIOS: IF (NumOfCompUnitsOnNode==1) THEN 1Ch ELSEIF (NumOfCompUnitsOnNode==2)
        //THEN 18h ELSEIF (NumOfCompUnitsOnNode==3) THEN 14h ELSEIF
        //(NumOfCompUnitsOnNode==4) THEN 10h ELSEIF (NumOfCompUnitsOnNode==5) THEN 11h
        //ELSE 0Eh ENDIF.
        PciAddress.Address.Function = FUNC_3;
        PciAddress.Address.Register = L3_BUFFER_COUNT_REG;
        LibAmdPciRead (AccessWidth32, PciAddress, &L3BufferCnt, StdHeader);
        L3BufferCnt.L3FreeListCBC = L3FreeListCBC;
        LibAmdPciWrite (AccessWidth32, PciAddress, &L3BufferCnt, StdHeader);

        //D18F3x7C[4:0]Xbar2SriFreeListCBC:
        //BIOS: IF (L3Enabled) THEN 16h ELSEIF (D18F5x80[Enabled[3]]==1) THEN 10h ELSEIF
        //(D18F5x80[Enabled[2]]==1) THEN 12h ELSEIF (D18F5x80[Enabled[1]]==1) THEN 14h ELSE 16h ENDIF.
        PciAddress.Address.Function = FUNC_3;
        PciAddress.Address.Register = NB_CAPS_REG;
        LibAmdPciRead (AccessWidth32, PciAddress, &NbCaps, StdHeader);
        if (NbCaps.L3Capable == 0) {
          PciAddress.Address.Function = FUNC_3;
          PciAddress.Address.Register = FREE_LIST_BUFFER_COUNT_REG;
          LibAmdPciRead (AccessWidth32, PciAddress, &FreeListBufferCount, StdHeader);
          FreeListBufferCount.Xbar2SriFreeListCBC = Xbar2SriFreeListCBC;
          LibAmdPciWrite (AccessWidth32, PciAddress, &FreeListBufferCount, StdHeader);
        }
      }
    }
  }

  return IsUpdated;
}


CONST CPU_CORE_LEVELING_FAMILY_SERVICES ROMDATA F15OrCoreLeveling =
{
  0,
  F15OrSetDownCoreRegister
};


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
F15OrGetCurrentNbFrequency (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
     OUT   UINT32 *FrequencyInMHz,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32  NbFid;
  UINT32  NbDid;
  UINT32  LocalPciRegister;
  PCI_ADDR     PciAddress;

  if (OptionMultiSocketConfiguration.GetCurrPciAddr (&PciAddress, StdHeader)) {
    PciAddress.Address.Function = FUNC_5;
    PciAddress.Address.Register = NB_PSTATE_STATUS;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    NbFid = ((NB_PSTATE_STS_REGISTER *) &LocalPciRegister)->CurNbFid;
    NbDid = ((NB_PSTATE_STS_REGISTER *) &LocalPciRegister)->CurNbDid;
    *FrequencyInMHz = (((NbFid + 4) * 200) / (1 << NbDid));
  }
  return AGESA_SUCCESS;
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
 *  @retval        AGESA_SUCCESS           Northbridge frequency is valid
 */
AGESA_STATUS
F15OrGetMinMaxNbFrequency (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       PCI_ADDR               *PciAddress,
     OUT   UINT32                 *MinFreqInMHz,
     OUT   UINT32                 *MaxFreqInMHz,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  INT8         NbPsMaxVal;
  UINT32       LocalPciRegister;
  UINT32       FreqNumerator;
  UINT32       FreqDivisor;
  BOOLEAN      CustomNbPs;
  AGESA_STATUS AgesaStatus;

  CustomNbPs      = FALSE;
  AgesaStatus     = AGESA_ERROR;

  // Obtain the max NB frequency on the node
  PciAddress->Address.Function = FUNC_5;
  PciAddress->Address.Register = NB_PSTATE_0;
  LibAmdPciRead (AccessWidth32, *PciAddress, &LocalPciRegister, StdHeader);
  if (((NB_PSTATE_REGISTER *) &LocalPciRegister)->NbPstateEn == 1) {
    FreqNumerator = ((((NB_PSTATE_REGISTER *) &LocalPciRegister)->NbFid + 4) * 200);
    FreqDivisor   = (1 << ((NB_PSTATE_REGISTER *) &LocalPciRegister)->NbDid);

    *MaxFreqInMHz = (FreqNumerator / FreqDivisor);
    AgesaStatus = AGESA_SUCCESS;
  }

  // If platform configuration disable NB P-states, return the NB P0 frequency
  // as both the min and max frequency on the node.
  if (PlatformConfig->PlatformProfile.PlatformPowerPolicy == Performance) {
    *MinFreqInMHz = *MaxFreqInMHz;
  } else {
    PciAddress->Address.Function = FUNC_5;
    PciAddress->Address.Register = NB_PSTATE_CTRL;
    LibAmdPciRead (AccessWidth32, *PciAddress, &LocalPciRegister, StdHeader);
    NbPsMaxVal = (INT8) ((NB_PSTATE_CTRL_REGISTER *) &LocalPciRegister)->NbPstateMaxVal;

    // Obtain the min NB frequency on the node, starting from NB Pmin
    for (; NbPsMaxVal >= 0; NbPsMaxVal--) {
      PciAddress->Address.Function = FUNC_5;
      PciAddress->Address.Register = (NB_PSTATE_0 + (4 * NbPsMaxVal));
      LibAmdPciRead (AccessWidth32, *PciAddress, &LocalPciRegister, StdHeader);

      // Ensure that the NB Pstate is enabled
      if (((NB_PSTATE_REGISTER *) &LocalPciRegister)->NbPstateEn == 1) {
        FreqNumerator = ((((NB_PSTATE_REGISTER *) &LocalPciRegister)->NbFid + 4) * 200);
        FreqDivisor   = (1 << ((NB_PSTATE_REGISTER *) &LocalPciRegister)->NbDid);

        *MinFreqInMHz = (FreqNumerator / FreqDivisor);
        AgesaStatus = AGESA_SUCCESS;
        break;
      }
    }
  }
  IDS_OPTION_HOOK (IDS_NBPS_MIN_FREQ, MinFreqInMHz, StdHeader);
  return AgesaStatus;
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
F15OrGetNbPstateInfo (
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
  BOOLEAN  PstateIsValid;

  PstateIsValid = FALSE;

  // If NB P1, P2, or P3 is requested, make sure that NB Pstate is enabled
  if ((NbPstate == 0) || (FamilySpecificServices->IsNbPstateEnabled (FamilySpecificServices, PlatformConfig, StdHeader))) {
    PciAddress->Address.Function = FUNC_5;
    PciAddress->Address.Register = NB_PSTATE_CTRL;
    LibAmdPciRead (AccessWidth32, *PciAddress, &LocalPciRegister, StdHeader);

    if (NbPstate <= ((NB_PSTATE_CTRL_REGISTER *) &LocalPciRegister)->NbPstateMaxVal) {
      PciAddress->Address.Register = (NB_PSTATE_0 + (4 * NbPstate));
      LibAmdPciRead (AccessWidth32, *PciAddress, &LocalPciRegister, StdHeader);

      // Ensure that requested NbPstate is enabled
      if (((NB_PSTATE_REGISTER *) &LocalPciRegister)->NbPstateEn == 1) {
        *FreqNumeratorInMHz = ((((NB_PSTATE_REGISTER *) &LocalPciRegister)->NbFid + 4) * 200);
        *FreqDivisor = (1 << ((NB_PSTATE_REGISTER *) &LocalPciRegister)->NbDid);
        *VoltageInuV = (1550000 - (12500 * (((NB_PSTATE_REGISTER *) &LocalPciRegister)->NbVid)));
        PstateIsValid = TRUE;
      }
    }
  }
  return PstateIsValid;
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
F15OrGetNumberOfPhysicalCores (
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
      PciAddress.Address.Function = FUNC_5;
      PciAddress.Address.Register = NORTH_BRIDGE_CAPABILITIES_2_REG;
      LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
      CmpCapOnNode = (UINT8) (LocalPciRegister & 0xFF);
      CmpCapOnNode++;
      CmpCap += CmpCapOnNode;
    }
  }
  return ((UINT8) CmpCap);
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Use the Mailbox Register to get the Ap Mailbox info for the current core.
 *
 *  @CpuServiceMethod{::F_CPU_AMD_GET_AP_MAILBOX_FROM_HARDWARE}.
 *
 *  Access the mailbox register used with this NB family.  This is valid until the
 *  point that some init code initializes the mailbox register for its normal use.
 *  The Machine Check Misc (Thresholding) register is available as both a PCI config
 *  register and a MSR, so it can be used as a mailbox from HT to other functions.
 *
 *  @param[in]     FamilySpecificServices  The current Family Specific Services.
 *  @param[out]    ApMailboxInfo           The AP Mailbox info
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 */
VOID
F15OrGetApMailboxFromHardware (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
     OUT   AP_MAILBOXES           *ApMailboxInfo,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT64 MailboxInfo;

  LibAmdMsrRead (MSR_MC_MISC_LINK_THRESHOLD, &MailboxInfo, StdHeader);
  // Mailbox info is in bits 32 thru 43, 12 bits.
  ApMailboxInfo->ApMailInfo.Info = (((UINT32) (MailboxInfo >> 32)) & (UINT32)0x00000FFF);
  LibAmdMsrRead (MSR_MC_MISC_L3_THRESHOLD, &MailboxInfo, StdHeader);
  // Mailbox info is in bits 32 thru 43, 12 bits.
  ApMailboxInfo->ApMailExtInfo.Info = (((UINT32) (MailboxInfo >> 32)) & (UINT32)0x00000FFF);
}


/*---------------------------------------------------------------------------------------*/
/**
 *  Set the system AP core number in the AP's Mailbox.
 *
 *  @CpuServiceMethod{::F_CPU_SET_AP_CORE_NUMBER}.
 *
 *  Access the mailbox register used with this NB family.  This is only intended to
 *  run on the BSC at the time of initial AP launch.
 *
 *  @param[in]     FamilySpecificServices  The current Family Specific Services.
 *  @param[in]     Socket                 The AP's socket
 *  @param[in]     Module                 The AP's module
 *  @param[in]     ApCoreNumber           The AP's unique core number
 *  @param[in]     StdHeader              Handle of Header for calling lib functions and services.
 *
 */
VOID
F15OrSetApCoreNumber (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       UINT32                 Socket,
  IN       UINT32                 Module,
  IN       UINT32                 ApCoreNumber,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32   LocalPciRegister;
  PCI_ADDR PciAddress;
  AGESA_STATUS IgnoredStatus;

  GetPciAddress (StdHeader, Socket, Module, &PciAddress, &IgnoredStatus);
  PciAddress.Address.Function = FUNC_3;
  PciAddress.Address.Register = 0x170;
  LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
  ((AP_MAIL_EXT_INFO *) &LocalPciRegister)->Fields.HeapIndex = ApCoreNumber;
  LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
}


/*---------------------------------------------------------------------------------------*/
/**
 *  Get this AP's system core number from hardware.
 *
 *  @CpuServiceMethod{::F_CPU_GET_AP_CORE_NUMBER}.
 *
 *  Returns the system core number from the scratch MSR, where
 *  it was saved at heap initialization.
 *
 *  @param[in]     FamilySpecificServices  The current Family Specific Services.
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 *  @return        The AP's unique core number
 */
UINT32
F15OrGetApCoreNumber (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  NODE_ID_MSR NodeIdMsr;

  LibAmdMsrRead (0xC001100C, &NodeIdMsr.Value, StdHeader);
  return (UINT32) NodeIdMsr.Fields.BiosScratch;
}


/*---------------------------------------------------------------------------------------*/
/**
 *  Move the AP's core number from the mailbox to hardware.
 *
 *  @CpuServiceMethod{::F_CPU_TRANSFER_AP_CORE_NUMBER}.
 *
 *  Transfers this AP's system core number from the mailbox to
 *  the NodeId MSR and initializes the other NodeId fields.
 *
 *  @param[in]     FamilySpecificServices  The current Family Specific Services.
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 */
VOID
F15OrTransferApCoreNumber (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  AP_MAILBOXES Mailboxes;
  NODE_ID_MSR NodeIdMsr;
  UINT64 ExtFeatures;

  NodeIdMsr.Value = 0;
  FamilySpecificServices->GetApMailboxFromHardware (FamilySpecificServices, &Mailboxes, StdHeader);
  NodeIdMsr.Fields.BiosScratch = Mailboxes.ApMailExtInfo.Fields.HeapIndex;
  NodeIdMsr.Fields.NodeId = Mailboxes.ApMailInfo.Fields.Node;
  NodeIdMsr.Fields.NodesPerProcessor = Mailboxes.ApMailInfo.Fields.ModuleType;
  LibAmdMsrWrite (0xC001100C, &NodeIdMsr.Value, StdHeader);

  // Indicate that the NodeId MSR is supported.
  LibAmdMsrRead (MSR_CPUID_EXT_FEATS, &ExtFeatures, StdHeader);
  ExtFeatures = (ExtFeatures | BIT51);
  LibAmdMsrWrite (MSR_CPUID_EXT_FEATS, &ExtFeatures, StdHeader);
}


/*---------------------------------------------------------------------------------------*/
/**
 * Disable NB P-state.
 *   - clear F5x1[6C:64]
 *   - clear F5x170[NbPstateMaxVal]
 *   - set   F5x170[SwNbPstateLoDis]
 *   - clear MSRC001_00[6B:64][NbPstate]
 *
 * @param[in]     FamilySpecificServices  The current Family Specific Services
 * @param[in]     CpuEarlyParamsPtr       Service Parameters
 * @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 */
VOID
F15OrNbPstateDis (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       AMD_CPU_EARLY_PARAMS   *CpuEarlyParamsPtr,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32       i;
  UINT32       PciData;
  UINT32       AndMask;
  AP_TASK      TaskPtr;
  PCI_ADDR     PciAddress;

  // Check whether NB P-state is disabled
  if (!FamilySpecificServices->IsNbPstateEnabled (FamilySpecificServices, &CpuEarlyParamsPtr->PlatformConfig, StdHeader)) {

    IDS_HDT_CONSOLE (CPU_TRACE, "  NB Pstates disabled\n");

    OptionMultiSocketConfiguration.GetCurrPciAddr (&PciAddress, StdHeader);

    AndMask = 0x00000000;
    // If NbPstateHi is not NB P0, get the Pstate pointed to by NbPstateHi and copy it's value to NB P0
    PciAddress.Address.Function = FUNC_5;
    PciAddress.Address.Register = NB_PSTATE_CTRL;
    LibAmdPciRead (AccessWidth32, PciAddress, &PciData, StdHeader);
    if (((NB_PSTATE_CTRL_REGISTER *) &PciData)->NbPstateHi != 0) {
      PciAddress.Address.Register = NB_PSTATE_0 + (((NB_PSTATE_CTRL_REGISTER *) &PciData)->NbPstateHi * 4);
      LibAmdPciRead (AccessWidth32, PciAddress, &PciData, StdHeader);
      PciAddress.Address.Register = NB_PSTATE_0;
      OptionMultiSocketConfiguration.ModifyCurrSocketPci (&PciAddress, AndMask, PciData, StdHeader);
    }

    // Clear F5x1[6C:64]
    for (i = 1; i < NM_NB_PS_REG; i++) {
      PciAddress.Address.Register = NB_PSTATE_0 + (i * 4);
      OptionMultiSocketConfiguration.ModifyCurrSocketPci (&PciAddress, AndMask, AndMask, StdHeader);
    }

    // Clear F5x170[NbPstateMaxVal] and set F5x170[SwNbPstateLoDis]
    PciAddress.Address.Register = NB_PSTATE_CTRL;
    AndMask = 0xFFFFFFFF;
    PciData = 0x00000000;
    ((NB_PSTATE_CTRL_REGISTER *) &AndMask)->NbPstateMaxVal = 0;
    ((NB_PSTATE_CTRL_REGISTER *) &PciData)->SwNbPstateLoDis = 1;
    OptionMultiSocketConfiguration.ModifyCurrSocketPci (&PciAddress, AndMask, PciData, StdHeader);

    // Clear MSRC001_00[6B:64][NbPstate] on cores
    TaskPtr.FuncAddress.PfApTask = F15OrNbPstateDisCore;
    TaskPtr.DataTransfer.DataSizeInDwords = 0;
    TaskPtr.DataTransfer.DataPtr = NULL;
    TaskPtr.ExeFlags = WAIT_FOR_CORE;
    ApUtilRunCodeOnAllLocalCoresAtEarly (&TaskPtr, StdHeader, CpuEarlyParamsPtr);

    // Once we are done disabling NB Pstates, clear F5x170[SwNbPstateLoDis]
    AndMask = 0xFFFFFFFF;
    PciData = 0x00000000;
    ((NB_PSTATE_CTRL_REGISTER *) &AndMask)->SwNbPstateLoDis = 0;
    OptionMultiSocketConfiguration.ModifyCurrSocketPci (&PciAddress, AndMask, PciData, StdHeader);
  }
}


/*---------------------------------------------------------------------------------------*/
/**
 * Disable NB P-state on core.
 *   - clear MSRC001_00[6B:64][NbPstate].
 *
 * @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 */
VOID
STATIC
F15OrNbPstateDisCore (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 i;
  UINT64 MsrData;

  // Only one core per compute unit needs to clear NbPstate in P-state MSRs
  if (IsCorePairPrimary (FirstCoreIsComputeUnitPrimary, StdHeader)) {
    for (i = MSR_PSTATE_0; i <= MSR_PSTATE_7; i++) {
      LibAmdMsrRead (i, &MsrData, StdHeader);
      ((PSTATE_MSR *) &MsrData)->NbPstate = 0;
      LibAmdMsrWrite (i, &MsrData, StdHeader);
    }
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * A Family Specific Workaround method, to override CPU TDP Limit 2 setting.
 *
 * \@TableTypeFamSpecificInstances.
 *
 * @param[in]     Data       The table data value, for example to indicate which CPU and Platform types matched.
 * @param[in]     StdHeader  Config params for library, services.
 */
VOID
F15OrOverrideNodeTdpLimit (
  IN       UINT32              Data,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32                OrMask;
  UINT32                LocalPciRegister;
  BOOLEAN               IsMultiNodeCpu;
  PCI_ADDR              PciAddress;

  IsMultiNodeCpu = FALSE;
  // check if it is MCM part
  if (OptionMultiSocketConfiguration.GetCurrPciAddr (&PciAddress, StdHeader)) {
    PciAddress.Address.Function = FUNC_3;
    PciAddress.Address.Register = NB_CAPS_REG;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    IsMultiNodeCpu = (BOOLEAN) (((NB_CAPS_REGISTER *) &LocalPciRegister)->MultiNodeCpu == 1);
  }

  if (IsMultiNodeCpu) {
    PciAddress.Address.Function = FUNC_4;
    PciAddress.Address.Register = 0x10C;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);

    // The correct value is the half of the fused value
    OrMask = LocalPciRegister & 0xFFFFF000;
    LocalPciRegister = ((LocalPciRegister & 0x00000FFF) >> 1) | OrMask;
    LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * A Family Specific Workaround method, to override CPU Node TDP Accumulator Throttle Threshold setting.
 *
 * \@TableTypeFamSpecificInstances.
 *
 * @param[in]     Data       The table data value, for example to indicate which CPU and Platform types matched.
 * @param[in]     StdHeader  Config params for library, services.
 */
VOID
F15OrOverrideNodeTdpAccumulatorThrottleThreshold (
  IN       UINT32              Data,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8                 CmpCap;
  UINT32                OrMask;
  UINT32                CUStatus;
  UINT32                LocalPciRegister;
  PCI_ADDR              PciAddress;

  if (OptionMultiSocketConfiguration.GetCurrPciAddr (&PciAddress, StdHeader)) {
    PciAddress.Address.Function = FUNC_5;
    PciAddress.Address.Register = 0x84;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    CmpCap = (UINT8) (LocalPciRegister & 0x000000FF);
    CmpCap++;

    // check if the part is fused with 1 core enabled per compute unit
    PciAddress.Address.Register = 0x80;
    LibAmdPciRead (AccessWidth32, PciAddress, &CUStatus, StdHeader);
    if ((CUStatus & 0x000F0000) != 0) {
      CmpCap = CmpCap >> 1;
    }

    PciAddress.Address.Register = 0xBC;
    LibAmdPciRead (AccessWidth32, PciAddress, &OrMask, StdHeader);
    OrMask = (UINT32) ((OrMask & 0x000FFFFF) * CmpCap);

    PciAddress.Address.Register = 0xB4;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    // The correct value is F5xBC[CmpUnitTdpAccThrottleThreshold] x ((F5x84[CmpCap] + 1) / 2).
    LocalPciRegister = (LocalPciRegister & 0xFFF00000) | (OrMask & 0x000FFFFF);
    LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * A Family Specific Workaround method, to sync internal node 1 SbiAddr setting.
 *
 * \@TableTypeFamSpecificInstances.
 *
 * @param[in]     Data       The table data value, for example to indicate which CPU and Platform types matched.
 * @param[in]     StdHeader  Config params for library, services.
 */
VOID
F15OrSyncInternalNode1SbiAddr (
  IN       UINT32              Data,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32       Socket;
  UINT32       Module;
  UINT32       DataOr;
  UINT32       DataAnd;
  UINT32       ModuleType;
  PCI_ADDR     PciAddress;
  AGESA_STATUS AgesaStatus;
  UINT32       SyncToModule;
  AP_MAIL_INFO ApMailboxInfo;
  UINT32       LocalPciRegister;

  ApMailboxInfo.Info = 0;

  GetApMailbox (&ApMailboxInfo.Info, StdHeader);
  ASSERT (ApMailboxInfo.Fields.Socket < MAX_SOCKETS);
  ASSERT (ApMailboxInfo.Fields.Module < MAX_DIES);
  Socket = ApMailboxInfo.Fields.Socket;
  Module = ApMailboxInfo.Fields.Module;
  ModuleType = ApMailboxInfo.Fields.ModuleType;

  // sync is just needed on multinode cpu
  if (ModuleType != 0) {
    // check if it is internal node 0 of every socket
    if (Module == 0) {
      if (GetPciAddress (StdHeader, Socket, Module, &PciAddress, &AgesaStatus)) {
        PciAddress.Address.Function = FUNC_3;
        PciAddress.Address.Register = 0x1E4;
        // read internal node 0 F3x1E4[6:4]
        LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
        DataOr = LocalPciRegister & ((UINT32) (7 << 4));
        DataAnd = ~(UINT32) (7 << 4);
        for (SyncToModule = 1; SyncToModule < GetPlatformNumberOfModules (); SyncToModule++) {
          if (GetPciAddress (StdHeader, Socket, SyncToModule, &PciAddress, &AgesaStatus)) {
            PciAddress.Address.Function = FUNC_3;
            PciAddress.Address.Register = 0x1E4;
            // sync the other internal node F3x1E4[6:4]
            LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
            LocalPciRegister &= DataAnd;
            LocalPciRegister |= DataOr;
            LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
          }
        }
      }
    }
  }
}

