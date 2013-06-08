/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 models 10h - 1Fh specific utility functions.
 *
 * Provides numerous utility functions specific to family 15h TN.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x15/TN
 * @e \$Revision: 65284 $   @e \$Date: 2012-02-12 23:29:39 -0600 (Sun, 12 Feb 2012) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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
#include "cpuFamilyTranslation.h"
#include "cpuF15PowerMgmt.h"
#include "cpuF15TnPowerMgmt.h"
#include "cpuApicUtilities.h"
#include "cpuEarlyInit.h"
#include "GeneralServices.h"
#include "OptionMultiSocket.h"
#include "F15TnUtilities.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)
#define FILECODE PROC_CPU_FAMILY_0X15_TN_F15TNUTILITIES_FILECODE

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
F15TnNbPstateDisCore (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
STATIC
F15TnGetNbFreqNumeratorInMHz (
  IN       UINT32                NbFid,
     OUT   UINT32                *FreqNumeratorInMHz,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  );

VOID
STATIC
F15TnGetNbFreqDivisor (
  IN       UINT32                NbDid,
     OUT   UINT32                *FreqDivisor,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  );

AGESA_STATUS
STATIC
F15TnCalculateNbFrequencyInMHz (
  IN       UINT32                NbFid,
  IN       UINT32                NbDid,
     OUT   UINT32                *FrequencyInMHz,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
STATIC
F15TnCovertVidInuV (
  IN       UINT32                Vid,
     OUT   UINT32                *VoltageInuV,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  );

VOID
STATIC
F15TnCmnGetIddDivisor (
  IN       UINT32                IddDiv,
     OUT   UINT32                *IddDivisor,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  );

VOID
STATIC
F15TnCmnCalculateCurrentInmA (
  IN       UINT32                IddValue,
  IN       UINT32                IddDiv,
     OUT   UINT32                *CurrentInmA,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  );

BOOLEAN
F15TnSetDownCoreRegister (
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
extern OPTION_MULTISOCKET_CONFIGURATION OptionMultiSocketConfiguration;

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
F15TnGetProcIddMax (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       UINT8                  Pstate,
     OUT   UINT32                 *ProcIddMax,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32       MsrAddress;
  PSTATE_MSR   PstateMsr;
  BOOLEAN      IsPstateEnabled;
  PCI_ADDR     PciAddress;
  NB_CAPS_2_REGISTER  NbCap2;
  UINT32       ProcIddMaxPerCore;

  IDS_HDT_CONSOLE (CPU_TRACE, "  F15TnGetProcIddMax - P%d\n", Pstate);

  IsPstateEnabled = FALSE;

  MsrAddress = (UINT32) (Pstate + PS_REG_BASE);
  ASSERT (MsrAddress <= PS_MAX_REG);

  LibAmdMsrRead (MsrAddress, (UINT64 *) &PstateMsr, StdHeader);
  F15TnCmnCalculateCurrentInmA ((UINT32) PstateMsr.IddValue, (UINT32) PstateMsr.IddDiv, &ProcIddMaxPerCore, StdHeader);
  PciAddress.AddressValue = NB_CAPS_REG2_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, PciAddress, &NbCap2, StdHeader);
  *ProcIddMax = (UINT32) ProcIddMaxPerCore * (NbCap2.CmpCap + 1);
  IDS_HDT_CONSOLE (CPU_TRACE, "    Pstate %d ProcIddMax %d CmpCap %d\n", Pstate, *ProcIddMax, NbCap2.CmpCap);
  if (PstateMsr.PsEnable == 1) {
    IsPstateEnabled = TRUE;
  }
  return IsPstateEnabled;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Set down core register on Trinity
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
F15TnSetDownCoreRegister (
  IN       CPU_CORE_LEVELING_FAMILY_SERVICES *FamilySpecificServices,
  IN       UINT32 *Socket,
  IN       UINT32 *Module,
  IN       UINT32 *LeveledCores,
  IN       CORE_LEVELING_TYPE CoreLevelMode,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32    LocalPciRegister;
  UINT32    CoreDisableBits;
  PCI_ADDR  PciAddress;
  BOOLEAN   IsUpdated;
  AGESA_STATUS AgesaStatus;

  IsUpdated = FALSE;
  CoreDisableBits = 0;

  if (CoreLevelMode == CORE_LEVEL_COMPUTE_UNIT) {
    // CoreLevelMode == CORE_LVEL_COMPUTE_UNIT is not supported.
  } else {
    switch (*LeveledCores) {
    // Only down core to 2 cores will take effect through a warm reset.
    case 2:
      CoreDisableBits = DOWNCORE_MASK_DUAL;
      break;
    }
  }

  if (CoreDisableBits != 0) {
    if (GetPciAddress (StdHeader, (UINT8) *Socket, (UINT8) *Module, &PciAddress, &AgesaStatus)) {
      PciAddress.Address.Function = FUNC_5;
      PciAddress.Address.Register = NORTH_BRIDGE_CAPABILITIES_2_REG;
      LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
      LocalPciRegister = (LocalPciRegister & 0xFF) + 1;
      LocalPciRegister = (1 << LocalPciRegister) - 1;
      CoreDisableBits &= LocalPciRegister;

      PciAddress.Address.Function = FUNC_3;
      PciAddress.Address.Register = DOWNCORE_CTRL;
      LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
      if ((LocalPciRegister | CoreDisableBits) != LocalPciRegister) {
        LocalPciRegister |= CoreDisableBits;
        LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
        IsUpdated = TRUE;
      }
    }
  }

  return IsUpdated;
}


CONST CPU_CORE_LEVELING_FAMILY_SERVICES ROMDATA F15TnCoreLeveling =
{
  0,
  F15TnSetDownCoreRegister
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
F15TnGetCurrentNbFrequency (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
     OUT   UINT32 *FrequencyInMHz,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  NB_PSTATE_STS_REGISTER NbPstateStsReg;
  PCI_ADDR     PciAddress;
  AGESA_STATUS ReturnCode;

  IDS_HDT_CONSOLE (CPU_TRACE, "  F15TnGetCurrentNbFrequency\n");

  PciAddress.AddressValue = NB_PSTATE_STATUS_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, PciAddress, &NbPstateStsReg, StdHeader);
  ReturnCode = F15TnCalculateNbFrequencyInMHz (
                 NbPstateStsReg.CurNbFid,
                 NbPstateStsReg.CurNbDid,
                 FrequencyInMHz,
                 StdHeader
                 );
  return ReturnCode;
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
F15TnGetMinMaxNbFrequency (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       PCI_ADDR               *PciAddress,
     OUT   UINT32                 *MinFreqInMHz,
     OUT   UINT32                 *MaxFreqInMHz,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  INT8         NbPsMaxVal;
  UINT8        i;
  UINT32       LocalPciRegister;
  AGESA_STATUS AgesaStatus;

  AgesaStatus = AGESA_ERROR;

  // Obtain the max NB frequency on the node
  PciAddress->Address.Function = FUNC_5;
  PciAddress->Address.Register = NB_PSTATE_CTRL;
  LibAmdPciRead (AccessWidth32, *PciAddress, &LocalPciRegister, StdHeader);
  NbPsMaxVal = (INT8) ((NB_PSTATE_CTRL_REGISTER *) &LocalPciRegister)->NbPstateMaxVal;

  // Starting from NB Pmax
  for (i = 0; i <= NbPsMaxVal; i++) {
    PciAddress->Address.Function = FUNC_5;
    PciAddress->Address.Register = (NB_PSTATE_0 + (4 * i));
    LibAmdPciRead (AccessWidth32, *PciAddress, &LocalPciRegister, StdHeader);

    // Ensure that the NB Pstate is enabled
    if (((NB_PSTATE_REGISTER *) &LocalPciRegister)->NbPstateEn == 1) {
      AgesaStatus = F15TnCalculateNbFrequencyInMHz (((NB_PSTATE_REGISTER *) &LocalPciRegister)->NbFid,
                                                    ((NB_PSTATE_REGISTER *) &LocalPciRegister)->NbDid,
                                                    MaxFreqInMHz,
                                                    StdHeader);
      break;
    }
  }
  // If all of NbPstates are disabled, get MaxFreqInMHz from CurNbPstate
  if (i > NbPsMaxVal) {
    PciAddress->Address.Register = NB_PSTATE_STATUS;
    LibAmdPciRead (AccessWidth32, *PciAddress, &LocalPciRegister, StdHeader);
    F15TnCalculateNbFrequencyInMHz (((NB_PSTATE_STS_REGISTER *) &LocalPciRegister)->CurNbFid,
                                      ((NB_PSTATE_STS_REGISTER *) &LocalPciRegister)->CurNbDid,
                                      MaxFreqInMHz,
                                      StdHeader);
    // No one NbPstate is enabled, so set Min = Max
    *MinFreqInMHz = *MaxFreqInMHz;
    ASSERT (FALSE);
  } else {
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
      for (/* NbPsMaxVal */; NbPsMaxVal >= 0; NbPsMaxVal--) {
        PciAddress->Address.Function = FUNC_5;
        PciAddress->Address.Register = (NB_PSTATE_0 + (4 * NbPsMaxVal));
        LibAmdPciRead (AccessWidth32, *PciAddress, &LocalPciRegister, StdHeader);

        // Ensure that the NB Pstate is enabled
        if (((NB_PSTATE_REGISTER *) &LocalPciRegister)->NbPstateEn == 1) {
          AgesaStatus = F15TnCalculateNbFrequencyInMHz (((NB_PSTATE_REGISTER *) &LocalPciRegister)->NbFid,
                                                        ((NB_PSTATE_REGISTER *) &LocalPciRegister)->NbDid,
                                                        MinFreqInMHz,
                                                        StdHeader);
          break;
        }
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
F15TnGetNbPstateInfo (
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
  BOOLEAN  PstateIsValid;
  NB_PSTATE_CTRL_REGISTER   NbPstateCtrlReg;
  NB_PSTATE_REGISTER        NbPstateReg;

  IDS_HDT_CONSOLE (CPU_TRACE, "  F15TnGetNbPstateInfo - NB P%d\n", NbPstate);

  ASSERT ((PciAddress->Address.Segment == 0) && (PciAddress->Address.Bus == 0) && (PciAddress->Address.Device == 0x18));

  PstateIsValid = FALSE;

  // If NB P1, P2, or P3 is requested, make sure that NB Pstate is enabled
  if ((NbPstate == 0) || (FamilySpecificServices->IsNbPstateEnabled (FamilySpecificServices, PlatformConfig, StdHeader))) {
    PciAddress->AddressValue = NB_PSTATE_CTRL_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, *PciAddress, &NbPstateCtrlReg, StdHeader);

    ASSERT ((NbPstate < NM_NB_PS_REG) && (NbPstateCtrlReg.NbPstateMaxVal < NM_NB_PS_REG));
    if (NbPstate <= NbPstateCtrlReg.NbPstateMaxVal) {
      PciAddress->Address.Register = (NB_PSTATE_0 + (sizeof (NB_PSTATE_REGISTER) * NbPstate));
      LibAmdPciRead (AccessWidth32, *PciAddress, &NbPstateReg, StdHeader);
      IDS_HDT_CONSOLE (CPU_TRACE, "    En:%d Fid:%x Did:%x Vid:%x\n", NbPstateReg.NbPstateEn, NbPstateReg.NbFid, NbPstateReg.NbDid, GetF15TnNbVid (&NbPstateReg));

      // Check if at least NB P0 is enabled.
      ASSERT ((NbPstate == 0) ? (NbPstateReg.NbPstateEn == 1) : TRUE);
      // Ensure that requested NbPstate is enabled
      if (NbPstateReg.NbPstateEn == 1) {
        // Check for P-state Bandwidth Requirements on
        // "All NB P-states must be defined such that D18F5x1[6C:60][NbFid] <= 2Eh"
        ASSERT (NbPstateReg.NbFid <= 0x2E);
        F15TnGetNbFreqNumeratorInMHz (NbPstateReg.NbFid, FreqNumeratorInMHz, StdHeader);
        F15TnGetNbFreqDivisor (NbPstateReg.NbDid, FreqDivisor, StdHeader);
        // Check for P-state Bandwidth Requirements on
        // "NBCOF >= 700MHz"
        ASSERT ((*FreqNumeratorInMHz / *FreqDivisor) >= 700);

        NbVid = GetF15TnNbVid (&NbPstateReg);
        F15TnCovertVidInuV  (NbVid, VoltageInuV, StdHeader);
        PstateIsValid = TRUE;
        IDS_HDT_CONSOLE (CPU_TRACE, "    NB Pstate %d is Valid. NbVid=%d VoltageInuV=%d\n", NbPstate, NbVid, *VoltageInuV);
      }
    }
  }
  return PstateIsValid;
}


/*---------------------------------------------------------------------------------------*/
/**
 *  Get NB pstate current.
 *
 *  @CpuServiceMethod{::F_CPU_GET_NB_IDD_MAX}.
 *
 *    This function returns the NbIddMax.
 *
 *  @param[in]     FamilySpecificServices    The current Family Specific Services.
 *  @param[in]     NbPstate                  The NB P-state to check.
 *  @param[out]    NbIddMax                  NB P-state current in mA.
 *  @param[in]     StdHeader                 Handle of Header for calling lib functions and services.
 *
 *  @retval        TRUE                      NB P-state is enabled, and NbIddMax is valid.
 *  @retval        FALSE                     NB P-state is disabled
 */
BOOLEAN
F15TnGetNbIddMax (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       UINT8                  NbPstate,
     OUT   UINT32                 *NbIddMax,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  BOOLEAN      IsNbPsEnabled;
  PCI_ADDR     PciAddress;
  NB_PSTATE_CTRL_REGISTER   NbPstateCtrlReg;
  NB_PSTATE_REGISTER        NbPstateReg;

  IDS_HDT_CONSOLE (CPU_TRACE, "  F15TnGetNbIddMax - NB P%d\n", NbPstate);

  IsNbPsEnabled = FALSE;

  PciAddress.AddressValue = NB_PSTATE_CTRL_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, PciAddress, &NbPstateCtrlReg, StdHeader);

  ASSERT (NbPstate < NM_NB_PS_REG);
  if (NbPstate <= NbPstateCtrlReg.NbPstateMaxVal) {
    PciAddress.Address.Register = (NB_PSTATE_0 + (sizeof (NB_PSTATE_REGISTER) * NbPstate));
    LibAmdPciRead (AccessWidth32, PciAddress, &NbPstateReg, StdHeader);

    // Ensure that requested NbPstate is enabled
    if (NbPstateReg.NbPstateEn == 1) {
      F15TnCmnCalculateCurrentInmA (NbPstateReg.NbIddValue, NbPstateReg.NbIddDiv, NbIddMax, StdHeader);
      IsNbPsEnabled = TRUE;
      IDS_HDT_CONSOLE (CPU_TRACE, "  NB Pstate %d is Valid. NbIddMax %d\n", NbPstate, *NbIddMax);
    }
  }
  return IsNbPsEnabled;
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
F15TnGetNumberOfPhysicalCores (
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
F15TnGetApMailboxFromHardware (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
     OUT   AP_MAILBOXES           *ApMailboxInfo,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  // For Family 15h Trinity, we will return socket 0, node 0, module 0, module type 0, and 0 for
  // the system degree
  ApMailboxInfo->ApMailInfo.Info = (UINT32) 0x00000000;
  ApMailboxInfo->ApMailExtInfo.Info = (UINT32) 0x00000000;
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
F15TnGetApCoreNumber (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  CPUID_DATA Cpuid;

  LibAmdCpuidRead (0x1, &Cpuid, StdHeader);
  return ((Cpuid.EBX_Reg >> 24) & 0xFF);
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
F15TnIsNbPstateEnabled (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  PCI_ADDR PciAddress;
  BOOLEAN  PowerMode;
  BOOLEAN  SkipHwCfg;
  NB_PSTATE_STS_REGISTER   NbPstateSts;
  NB_PSTATE_CTRL_REGISTER  NbPstateCtrl;


  PciAddress.AddressValue = NB_PSTATE_STATUS_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, PciAddress, (VOID *) &NbPstateSts, StdHeader);
  if (NbPstateSts.NbPstateDis == 1) {
    return FALSE;
  }

  SkipHwCfg = FALSE;
  IDS_OPTION_HOOK (IDS_NBPSDIS_OVERRIDE, &SkipHwCfg, StdHeader);
  if (!SkipHwCfg) {
  }

  // Defaults to Power Optimized Mode
  PowerMode = TRUE;

  // If system is optimized for performance, disable NB P-States
  if (PlatformConfig->PlatformProfile.PlatformPowerPolicy == Performance) {
    PowerMode = FALSE;
  }

  PciAddress.AddressValue = NB_PSTATE_CTRL_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, PciAddress, (VOID *) &NbPstateCtrl, StdHeader);
  if (((NbPstateCtrl.NbPstateMaxVal != 0) || SkipHwCfg) && (PowerMode)) {
    return TRUE;
  }
  return FALSE;
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
F15TnNbPstateDis (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       AMD_CPU_EARLY_PARAMS   *CpuEarlyParamsPtr,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32       i;
  UINT32       PciData;
  UINT32       NbPsCtrl;
  UINT32       NbPsCtrlOrg;
  UINT32       AndMask;
  BOOLEAN      SkipNbPsLoPart;
  AP_TASK      TaskPtr;
  PCI_ADDR     PciAddress;

  IDS_HDT_CONSOLE (CPU_TRACE, "  F15TnNbPstateDis\n");

  // Check whether NB P-state is disabled
  if (!FamilySpecificServices->IsNbPstateEnabled (FamilySpecificServices, &CpuEarlyParamsPtr->PlatformConfig, StdHeader)) {

    IDS_HDT_CONSOLE (CPU_TRACE, "  NB Pstates disabled\n");

    PciAddress.AddressValue = NB_PSTATE_CTRL_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &NbPsCtrl, StdHeader);
    NbPsCtrlOrg = NbPsCtrl;

    AndMask = 0x00000000;
    // If CurNbPstate is not NB P0, get the Pstate pointed to by CurNbPstate and copy it's value to NB P0 to P3 and clear NbPstateHi
    PciAddress.Address.Register = NB_PSTATE_STATUS;
    LibAmdPciRead (AccessWidth32, PciAddress, &PciData, StdHeader);

    SkipNbPsLoPart = FALSE;

    if (((NB_PSTATE_STS_REGISTER *) &PciData)->CurNbPstate != 0) {
      PciAddress.Address.Register = NB_PSTATE_0 + (((NB_PSTATE_STS_REGISTER *) &PciData)->CurNbPstate * 4);
      LibAmdPciRead (AccessWidth32, PciAddress, &PciData, StdHeader);

      for (i = 1; i < NM_NB_PS_REG; i++) {
        PciAddress.Address.Register = NB_PSTATE_0 + (i * 4);
        OptionMultiSocketConfiguration.ModifyCurrSocketPci (&PciAddress, AndMask, PciData, StdHeader);
      }

      if (!SkipNbPsLoPart) {
        // Program D18F5x170 to transition the NB P-state:
        // 1) NbPstateLo = NbPstateMaxVal.
        // 2) SwNbPstateLoDis = NbPstateDisOnP0 = NbPstateThreshold = 0.

        PciAddress.Address.Register = NB_PSTATE_CTRL;
        ((NB_PSTATE_CTRL_REGISTER *) &NbPsCtrl)->NbPstateLo = ((NB_PSTATE_CTRL_REGISTER *) &NbPsCtrl)->NbPstateMaxVal;
        LibAmdPciWrite (AccessWidth32, PciAddress, &NbPsCtrl, StdHeader);
        ((NB_PSTATE_CTRL_REGISTER *) &NbPsCtrl)->SwNbPstateLoDis = 0;
        ((NB_PSTATE_CTRL_REGISTER *) &NbPsCtrl)->NbPstateDisOnP0 = 0;
        ((NB_PSTATE_CTRL_REGISTER *) &NbPsCtrl)->NbPstateThreshold = 0;
        LibAmdPciWrite (AccessWidth32, PciAddress, &NbPsCtrl, StdHeader);

        // Wait for D18F5x174[CurNbPstate] to equal NbPstateLo.
        PciAddress.Address.Register = NB_PSTATE_STATUS;
        do {
          LibAmdPciRead (AccessWidth32, PciAddress, &PciData, StdHeader);
        } while (((NB_PSTATE_STS_REGISTER *) &PciData)->CurNbPstate != ((NB_PSTATE_CTRL_REGISTER *) &NbPsCtrl)->NbPstateLo);
      }
    }

    // Program D18F5x170 to force the NB P-state:
    // 1) NbPstateHi = target NB P-state.
    // 2) SwNbPstateLoDis = 1
    // And clear F5x170[NbPstateMaxVal]
    PciAddress.Address.Register = NB_PSTATE_CTRL;
    ((NB_PSTATE_CTRL_REGISTER *) &NbPsCtrl)->NbPstateHi = 0;
    ((NB_PSTATE_CTRL_REGISTER *) &NbPsCtrl)->NbPstateMaxVal = 0;
    LibAmdPciWrite (AccessWidth32, PciAddress, &NbPsCtrl, StdHeader);
    ((NB_PSTATE_CTRL_REGISTER *) &NbPsCtrl)->SwNbPstateLoDis = 1;
    LibAmdPciWrite (AccessWidth32, PciAddress, &NbPsCtrl, StdHeader);

    // Wait for D18F5x174[CurNbPstate] to equal the target NB P-state.
    PciAddress.Address.Register = NB_PSTATE_STATUS;
    do {
      LibAmdPciRead (AccessWidth32, PciAddress, &PciData, StdHeader);
    } while (((NB_PSTATE_STS_REGISTER *) &PciData)->CurNbPstate != 0);

    // Clear F5x1[6C:64]
    AndMask = 0x00000000;
    for (i = 1; i < NM_NB_PS_REG; i++) {
      PciAddress.Address.Register = NB_PSTATE_0 + (i * 4);
      OptionMultiSocketConfiguration.ModifyCurrSocketPci (&PciAddress, AndMask, AndMask, StdHeader);
    }

    // Clear MSRC001_00[6B:64][NbPstate] on cores
    TaskPtr.FuncAddress.PfApTask = F15TnNbPstateDisCore;
    TaskPtr.DataTransfer.DataSizeInDwords = 0;
    TaskPtr.DataTransfer.DataPtr = NULL;
    TaskPtr.ExeFlags = WAIT_FOR_CORE;
    ApUtilRunCodeOnAllLocalCoresAtEarly (&TaskPtr, StdHeader, CpuEarlyParamsPtr);


    // BIOS performs the following to release the NB P-state force:
    // 1. Restore the initial D18F5x170[SwNbPstateLoDis, NbPstateDisOnP0, NbPstateLo] values.
    // 2. Restore the initial D18F5x170[NbPstateThreshold, NbPstateHi] values.
    PciAddress.Address.Register = NB_PSTATE_CTRL;
    LibAmdPciRead (AccessWidth32, PciAddress, &PciData, StdHeader);
    ((NB_PSTATE_CTRL_REGISTER *) &PciData)->NbPstateLo = 0;
    LibAmdPciWrite (AccessWidth32, PciAddress, &PciData, StdHeader);
    ((NB_PSTATE_CTRL_REGISTER *) &PciData)->SwNbPstateLoDis = ((NB_PSTATE_CTRL_REGISTER *) &NbPsCtrlOrg)->SwNbPstateLoDis;
    ((NB_PSTATE_CTRL_REGISTER *) &PciData)->NbPstateDisOnP0 = ((NB_PSTATE_CTRL_REGISTER *) &NbPsCtrlOrg)->NbPstateDisOnP0;
    LibAmdPciWrite (AccessWidth32, PciAddress, &PciData, StdHeader);

    ((NB_PSTATE_CTRL_REGISTER *) &PciData)->NbPstateHi = 0;
    ((NB_PSTATE_CTRL_REGISTER *) &PciData)->NbPstateMaxVal = 0;
    LibAmdPciWrite (AccessWidth32, PciAddress, &PciData, StdHeader);
    ((NB_PSTATE_CTRL_REGISTER *) &PciData)->NbPstateThreshold = ((NB_PSTATE_CTRL_REGISTER *) &NbPsCtrlOrg)->NbPstateThreshold;
    LibAmdPciWrite (AccessWidth32, PciAddress, &PciData, StdHeader);
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
F15TnNbPstateDisCore (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 i;
  UINT64 MsrData;

  IDS_HDT_CONSOLE (CPU_TRACE, "  F15TnNbPstateDisCore\n");

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
 * Get NB Frequency Numerator in MHz
 *
 * @param[in]     NbFid                   NB Frequency ID to convert
 * @param[out]    FreqNumeratorInMHz      The desire NB FID's frequency numerator in megahertz.
 * @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 */
VOID
STATIC
F15TnGetNbFreqNumeratorInMHz (
  IN       UINT32                NbFid,
     OUT   UINT32                *FreqNumeratorInMHz,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  IDS_HDT_CONSOLE (CPU_TRACE, "  F15TnGetNbFreqNumeratorInMHz - NbFid=%d\n", NbFid);
  *FreqNumeratorInMHz = (NbFid + 4) * 100;
  IDS_HDT_CONSOLE (CPU_TRACE, "    FreqNumeratorInMHz=%d\n", *FreqNumeratorInMHz);
}


/*---------------------------------------------------------------------------------------*/
/**
 * Get NB Frequency Divisor
 *
 * @param[in]     NbDid                   NB Divisor ID to convert.
 * @param[out]    FreqDivisor             The desire NB DID's frequency divisor.
 * @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 */
VOID
STATIC
F15TnGetNbFreqDivisor (
  IN       UINT32                NbDid,
     OUT   UINT32                *FreqDivisor,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  IDS_HDT_CONSOLE (CPU_TRACE, "  F15TnGetNbFreqDivisor - NbDid=%d\n", NbDid);
  *FreqDivisor = (1 << NbDid);
  IDS_HDT_CONSOLE (CPU_TRACE, "    FreqDivisor=%d\n", *FreqDivisor);
}


/*---------------------------------------------------------------------------------------*/
/**
 * Calculate NB Frequency in MHz
 *
 * @param[in]     NbFid                   NB Frequency ID to convert
 * @param[in]     NbDid                   NB Divisor ID to convert.
 * @param[out]    FrequencyInMHz          The Northbridge clock frequency in megahertz.
 * @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 * @return        AGESA_SUCCESS           FrequencyInMHz is valid.
 */
AGESA_STATUS
STATIC
F15TnCalculateNbFrequencyInMHz (
  IN       UINT32                NbFid,
  IN       UINT32                NbDid,
     OUT   UINT32                *FrequencyInMHz,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32        FreqNumeratorInMHz;
  UINT32        FreqDivisor;
  AGESA_STATUS  ReturnStatus;

  IDS_HDT_CONSOLE (CPU_TRACE, "  F15TnCalculateNbFrequencyInMHz - NbFid=%x, NbDid=%x\n", NbFid, NbDid);

  ReturnStatus = AGESA_SUCCESS;
  F15TnGetNbFreqNumeratorInMHz (NbFid, &FreqNumeratorInMHz, StdHeader);
  F15TnGetNbFreqDivisor (NbDid, &FreqDivisor, StdHeader);
  *FrequencyInMHz = FreqNumeratorInMHz / FreqDivisor;
  IDS_HDT_CONSOLE (CPU_TRACE, "    FrequencyInMHz=%d\n", *FrequencyInMHz);

  return ReturnStatus;
}


/*---------------------------------------------------------------------------------------*/
/**
 * Convert VID to microvolts(uV)
 *
 * @param[in]     Vid                     The voltage ID of SVI2 encoding to be converted.
 * @param[out]    VoltageInuV             The voltage in microvolts.
 * @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 */
VOID
STATIC
F15TnCovertVidInuV (
  IN       UINT32                Vid,
     OUT   UINT32                *VoltageInuV,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  IDS_HDT_CONSOLE (CPU_TRACE, "  F15TnCovertVidInuV\n");
  // Maximum 1.55V, 6.25mV per stpe
  *VoltageInuV = ConvertVidInuV(Vid);
  IDS_HDT_CONSOLE (CPU_TRACE, "    Vid=%x, VoltageInuV=%d\n", Vid, *VoltageInuV);
}


/*---------------------------------------------------------------------------------------*/
/**
 * Get Core/NB Idd Divisor
 *
 * @param[in]     IddDiv                  Core/NB current divisor to convert.
 * @param[out]    IddDivisor              The desire Core/NB current divisor.
 * @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 */
VOID
STATIC
F15TnCmnGetIddDivisor (
  IN       UINT32                IddDiv,
     OUT   UINT32                *IddDivisor,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  IDS_HDT_CONSOLE (CPU_TRACE, "  F15TnCmnGetIddDivisor - IddDiv=%d\n", IddDiv);

  switch (IddDiv) {
  case 0:
    *IddDivisor = 1000;
    break;
  case 1:
    *IddDivisor = 100;
    break;
  case 2:
    *IddDivisor = 10;
    break;
  default:  // IddDiv = 3 is reserved. Use 10
    *IddDivisor = 10;
    ASSERT (FALSE);
    break;
  }
  IDS_HDT_CONSOLE (CPU_TRACE, "    IddDivisor=%d\n", *IddDivisor);
}


/*---------------------------------------------------------------------------------------*/
/**
 * Calculate Core/NB current in mA
 *
 * @param[in]     IddValue                Core/NB current value.
 * @param[in]     IddDiv                  Core/NB current divisor.
 * @param[out]    CurrentInmA             The Core/NB current in milliampere.
 * @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 */
VOID
STATIC
F15TnCmnCalculateCurrentInmA (
  IN       UINT32                IddValue,
  IN       UINT32                IddDiv,
     OUT   UINT32                *CurrentInmA,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINT32        IddDivisor;

  IDS_HDT_CONSOLE (CPU_TRACE, "  F15TnCmnCalculateCurrentInmA - IddValue=%x, IddDiv=%x\n", IddValue, IddDiv);

  F15TnCmnGetIddDivisor (IddDiv, &IddDivisor, StdHeader);
  *CurrentInmA = IddValue * IddDivisor;

  IDS_HDT_CONSOLE (CPU_TRACE, "    CurrentInmA=%d\n", *CurrentInmA);
}




