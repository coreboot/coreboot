/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 specific utility functions.
 *
 * Provides numerous utility functions specific to family 15h.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x15
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
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
#include "Ids.h"
#include "cpuRegisters.h"
#include "cpuFamilyTranslation.h"
#include "cpuPstateTables.h"
#include "cpuF15PowerMgmt.h"
#include "cpuApicUtilities.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "cpuF15Utilities.h"
#include "cpuEarlyInit.h"
#include "cpuPostInit.h"
#include "cpuFeatures.h"
#include "OptionMultiSocket.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)
#define FILECODE PROC_CPU_FAMILY_0X15_CPUF15UTILITIES_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

extern CPU_FAMILY_SUPPORT_TABLE            PstateFamilyServiceTable;
extern OPTION_MULTISOCKET_CONFIGURATION    OptionMultiSocketConfiguration;

// HT Phy registers used in code.
#define HT_PHY_FUSE_PROC_DLL_PROCESS_COMP_RD_SL0 0x4011
#define HT_PHY_FUSE_PROC_DLL_PROCESS_COMP_RD_SL1 0x4411
#define HT_PHY_LINK_PHY_RECEIVER_PROCESS_DLL_CONTROL_RD  0x400F
#define HT_PHY_LINK_PHY_RECEIVER_PROCESS_DLL_CONTROL_SL0 0x520F
#define HT_PHY_LINK_PHY_RECEIVER_PROCESS_DLL_CONTROL_SL1 0x530F

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/**
 * HT PHY DLL Process Compensation Lookup Table.
 *
 * If the hardware provides compensation values, the value is provided by accessing the bitfield
 * [HiBit:LoBit].  Otherwise, a default value will be used.
 *
 */
typedef struct {
  UINT32   DefaultComp;                       ///< The default compensation value if not provided by hardware.
  UINT8    CtlIndexLoBit;                     ///< The low bit position of the compensation value.
  UINT8    CtlIndexHiBit;                     ///< The high bit position of the compensation value.
} HT_PHY_DLL_COMP_LOOKUP_TABLE;

/**
 * Process Compensation Fuses for HT PHY, Link Phy Receiver Process Fuse Control Register.
 */
typedef struct {
  UINT32   :11;
  UINT32   DllProcessComp10:2;                     ///< [12:11] DLL Process Comp bits [1:0], this phy's adjustment.
  UINT32   DllProcessComp2:1;                      ///< [13] DLL Process Comp bit 2, Increment or Decrement.
  UINT32   : (31 - 13);
} LINK_PHY_RECEIVER_PROCESS_FUSE_CONTROL_FIELDS;

/// Access register as fields or uint32 value.
typedef union {
  UINT32   Value;                                         ///< 32 bit value for register access
  LINK_PHY_RECEIVER_PROCESS_FUSE_CONTROL_FIELDS Fields;   ///< The register bit fields
} LINK_PHY_RECEIVER_PROCESS_FUSE_CONTROL;

/**
 * Link Phy Receiver Process DLL Control Register.
 */
typedef struct {
  UINT32   DllProcessFreqCtlIndex2:4;              ///< [3:0] The DLL Compensation override.
  UINT32   : (12 - 4);
  UINT32   DllProcessFreqCtlOverride:1;            ///< [12] Enable DLL Compensation overriding.
  UINT32   : (31 - 12);
} LINK_PHY_RECEIVER_PROCESS_DLL_CONTROL_FIELDS;

/// Access register as fields or uint32 value.
typedef union {
  UINT32    Value;                                       ///< 32 bit value for register access
  LINK_PHY_RECEIVER_PROCESS_DLL_CONTROL_FIELDS Fields;   ///< The register bit fields
} LINK_PHY_RECEIVER_PROCESS_DLL_CONTROL;

/**
 * Provide the HT PHY DLL compensation value for each HT Link frequency.
 *
 * The HT Frequency enum is not contiguous, there are skipped values.  Rather than complicate
 * index calculations, add Invalid entries here marked with an invalid compensation value (invalid
 * because real compensation values are 0 .. 15).
 */
CONST STATIC HT_PHY_DLL_COMP_LOOKUP_TABLE ROMDATA HtPhyDllCompLookupTable[] = {
  {0xAul, 0, 3},               //   HT_FREQUENCY_1200M
  {0xAul, 0, 3},               //   HT_FREQUENCY_1400M
  {0x7ul, 4, 7},               //   HT_FREQUENCY_1600M
  {0x7ul, 4, 7},               //   HT_FREQUENCY_1800M
  {0x5ul, 8, 11},              //   HT_FREQUENCY_2000M
  {0x5ul, 8, 11},              //   HT_FREQUENCY_2200M
  {0x4ul, 12, 15},             //   HT_FREQUENCY_2400M
  {0x3ul, 16, 19},             //   HT_FREQUENCY_2600M
  {0xFFFFFFFFul, 0, 0},        //   Invalid
  {0xFFFFFFFFul, 0, 0},        //   Invalid
  {0x3ul, 20, 23},             //   HT_FREQUENCY_2800M
  {0x2ul, 24, 27},             //   HT_FREQUENCY_3000M
  {0x2ul, 28, 31}              //   HT_FREQUENCY_3200M
};

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
F15DisablePstate (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       UINT8 StateNumber,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64 LocalMsrRegister;

  ASSERT (StateNumber < NM_PS_REG);
  LibAmdMsrRead (PS_REG_BASE + (UINT32) StateNumber, &LocalMsrRegister, StdHeader);
  ((F15_PSTATE_MSR *) &LocalMsrRegister)->PsEnable = 0;
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
F15TransitionPstate (
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
F15GetTscRate (
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
    OptionMultiSocketConfiguration.GetCurrPciAddr (&PciAddress, StdHeader);
    PciAddress.Address.Function = FUNC_4;
    PciAddress.Address.Register = CPB_CTRL_REG;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    NumBoostStates = (UINT8) ((F15_CPB_CTRL_REGISTER *) &LocalPciRegister)->NumBoostStates;
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
F15LaunchApCore (
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
    PciAddress.Address.Register = CORE_CTRL;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    if ((LocalPciRegister & CORE_CTRL_CORE1_EN) == 0) {
      LocalPciRegister |= CORE_CTRL_CORE1_EN;
      LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
      LaunchFlag = TRUE;
    } else {
      LaunchFlag = FALSE;
    }
    break;

  case 2:
    PciAddress.Address.Register = CORE_CTRL;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);

    if ((LocalPciRegister & CORE_CTRL_CORE2_EN) == 0) {
      LocalPciRegister |= CORE_CTRL_CORE2_EN;
      LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister,
                      StdHeader);
      LaunchFlag = TRUE;
    } else {
      LaunchFlag = FALSE;
    }
    break;

  case 3:
    PciAddress.Address.Register = CORE_CTRL;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    if ((LocalPciRegister & CORE_CTRL_CORE3_EN) == 0) {
      LocalPciRegister |= CORE_CTRL_CORE3_EN;
      LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
      LaunchFlag = TRUE;
    } else {
      LaunchFlag = FALSE;
    }
    break;

  case 4:
    PciAddress.Address.Register = CORE_CTRL;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    if ((LocalPciRegister & CORE_CTRL_CORE4_EN) == 0) {
      LocalPciRegister |= CORE_CTRL_CORE4_EN;
      LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
      LaunchFlag = TRUE;
    } else {
      LaunchFlag = FALSE;
    }
    break;

  case 5:
    PciAddress.Address.Register = CORE_CTRL;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    if ((LocalPciRegister & CORE_CTRL_CORE5_EN) == 0) {
      LocalPciRegister |= CORE_CTRL_CORE5_EN;
      LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
      LaunchFlag = TRUE;
    } else {
      LaunchFlag = FALSE;
    }
    break;

  case 6:
    PciAddress.Address.Register = CORE_CTRL;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    if ((LocalPciRegister & CORE_CTRL_CORE6_EN) == 0) {
      LocalPciRegister |= CORE_CTRL_CORE6_EN;
      LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
      LaunchFlag = TRUE;
    } else {
      LaunchFlag = FALSE;
    }
    break;

  case 7:
    PciAddress.Address.Register = CORE_CTRL;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    if ((LocalPciRegister & CORE_CTRL_CORE7_EN) == 0) {
      LocalPciRegister |= CORE_CTRL_CORE7_EN;
      LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
      LaunchFlag = TRUE;
    } else {
      LaunchFlag = FALSE;
    }
      break;

  case 8:
    PciAddress.Address.Register = CORE_CTRL;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    if ((LocalPciRegister & CORE_CTRL_CORE8_EN) == 0) {
      LocalPciRegister |= CORE_CTRL_CORE8_EN;
      LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
      LaunchFlag = TRUE;
    } else {
      LaunchFlag = FALSE;
    }
    break;

  case 9:
    PciAddress.Address.Register = CORE_CTRL;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    if ((LocalPciRegister & CORE_CTRL_CORE9_EN) == 0) {
      LocalPciRegister |= CORE_CTRL_CORE9_EN;
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
F15GetNextHtLinkFeatures (
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
    // end of links.
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
F15NextLinkHasHtPhyFeats (
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
      // Also, if the links are ganged but the width is not 16 bits, treat it is an inactive lane.
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
F15SetHtPhyRegister (
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
  if ((HtPhyEntry->Address > 0x3FF) || ((HtPhyEntry->Address >= 0xE) && (HtPhyEntry->Address <= 0x11))) {
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
 * Applies an HT Phy write to a specified Phy register.
 *
 * @CpuServiceMethod{::F_SET_HT_PHY_REGISTER}.
 *
 * The caller is responsible for performing any read and modify steps.
 * This function performs the necessary sequence of PCI reads, writes, and waits
 * necessary to program an HT Phy register.
 *
 * @param[in]  CapabilitySet The link's HT Host base address.
 * @param[in]  Link          Zero based, node, link number (not package link).
 * @param[in]  Address       The HT Phy register address
 * @param[in]  Data          The data to write to the register
 * @param[in]  StdHeader     Config handle for library and services
 *
 */
VOID
STATIC
F15WriteOnlyHtPhyRegister (
  IN       PCI_ADDR                 CapabilitySet,
  IN       UINT32                   Link,
  IN       UINT32                   Address,
  IN       UINT32                   Data,
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
  if ((Address > 0x3FF) || ((Address >= 0xE) && (Address <= 0x11))) {
    PhyReg |= HTPHY_DIRECT_MAP;
  }

  PhyReg |= (Address);

  // Get the current register contents and do the update requested by the table
  PhyBase.AddressValue += 4;
  LibAmdPciWrite (AccessWidth32, PhyBase, &Data, StdHeader);

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
 * Get the value of an HT PHY register.
 *
 * Reading HT Phy registers is not generally useful, because they return the effective value,
 * not the currently written value.  So be warned, this function is dangerous if used to read
 * a register that will be udpated subsequently elsewhere.
 *
 * This routine is useful for reading hardware status from the HT Phy that can be used to set
 * other phy registers.
 *
 * @param[in]  CapabilitySet The link's HT Host base address.
 * @param[in]  Link          Zero based, node link number (not package link).
 * @param[in]  Address       The HT Phy register address to read
 * @param[in]  StdHeader     Config handle for library and services
 *
 * @return     The register content (in most cases, the effective content not the pending content)
 *
 */
UINT32
STATIC
F15GetHtPhyRegister (
  IN       PCI_ADDR                 CapabilitySet,
  IN       UINT32                   Link,
  IN       UINT32                   Address,
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
  if ((Address > 0x3FF) || ((Address >= 0xE) && (Address <= 0x11))) {
    PhyReg |= HTPHY_DIRECT_MAP;
  }

  PhyReg |= Address;
  // Ask the portal to read the HT Phy Register contents
  LibAmdPciWrite (AccessWidth32, PhyBase, &PhyReg, StdHeader);
  do
  {
    LibAmdPciRead (AccessWidth32, PhyBase, &Temp, StdHeader);
  } while (!(Temp & HTPHY_IS_COMPLETE_MASK));

  // Get the current register contents
  PhyBase.AddressValue += 4;
  LibAmdPciRead (AccessWidth32, PhyBase, &Temp, StdHeader);

  return Temp;
}

/*---------------------------------------------------------------------------------------*/
/**
 * A Family Specific Workaround method, to override HT DLL Compensation.
 *
 * \@TableTypeFamSpecificInstances.
 *
 * The Link Product Information register can be fused to contain an HT PHY DLL Compensation Override table.
 * Based on link frequency, a compensation override can be selected from the value.
 * To accomodate individual link differences in the package, each link can also have a DLL process compensation
 * value set.  This value can apply an adjustment to the compensation value.
 *
 * @param[in]     Data       The table data value, for example to indicate which CPU and Platform types matched.
 * @param[in]     StdHeader  Config params for library, services.
 */
VOID
F15HtPhyOverrideDllCompensation (
  IN       UINT32              Data,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32                ProductLinkInfo;
  UINT32                Link;
  CPU_LOGICAL_ID        CpuFamilyRevision;
  PCI_ADDR              StartingCapabilitySet;
  PCI_ADDR              CapabilitySet;
  PCI_ADDR              PciAddress;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;
  BOOLEAN               MatchedSublink1;
  HT_FREQUENCIES        Freq0;
  HT_FREQUENCIES        Freq1;
  UINTN                 Sublink;
  HT_PHY_LINK_FEATS     DesiredLinkFeats;
  BOOLEAN               IsEarlyRevProcessor;
  BOOLEAN               IsHardwareReportingComp;
  UINTN                 LinkFrequency;
  UINT32                Compensation;
  UINT32                Adjustment;
  BOOLEAN               IsIncrementAdjust;
  LINK_PHY_RECEIVER_PROCESS_FUSE_CONTROL LinkPhyReceiverProcessFuseControl;
  LINK_PHY_RECEIVER_PROCESS_DLL_CONTROL  LinkPhyReceiverProcessDllControl;

  OptionMultiSocketConfiguration.GetCurrPciAddr (&StartingCapabilitySet, StdHeader);
  GetLogicalIdOfCurrentCore (&CpuFamilyRevision, StdHeader);
  GetCpuServicesFromLogicalId (&CpuFamilyRevision, (CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);

  // Check if the hardware reported any compensation values.
  IsEarlyRevProcessor = (BOOLEAN) ((Data == 0) ? TRUE : FALSE);
  PciAddress = StartingCapabilitySet;
  PciAddress.Address.Function = FUNC_5;
  PciAddress.Address.Register = 0x190;
  LibAmdPciRead (AccessWidth32, PciAddress, &ProductLinkInfo, StdHeader);
  IsHardwareReportingComp = (BOOLEAN) (ProductLinkInfo != 0);

  if (!IsEarlyRevProcessor || IsHardwareReportingComp) {
    // Process all the sublink 0's and then all the sublink 1's that are at HT3 frequency.
    for (Sublink = 0; Sublink < 2; Sublink++) {
      CapabilitySet = StartingCapabilitySet;
      Link = 0;
      DesiredLinkFeats.HtPhyLinkValue = ((Sublink == 0) ? HTPHY_LINKTYPE_SL0_HT3 : HTPHY_LINKTYPE_SL0_HT3);
      while (FamilySpecificServices->NextLinkHasHtPhyFeats (
               FamilySpecificServices,
               &CapabilitySet,
               &Link,
               &DesiredLinkFeats,
               &MatchedSublink1,
               &Freq0,
               &Freq1,
               StdHeader)) {

        // Look up compensation value.  Remember that we matched links which are at HT3 frequency, so Freq[1,0]
        // should safely be greater than or equal to 1.2 GHz.
        if (Sublink == 0) {
          LinkFrequency = Freq0 - HT_FREQUENCY_1200M;
        } else {
          LinkFrequency = (MatchedSublink1 ? Freq1 : Freq0) - HT_FREQUENCY_1200M;
        }
        // This assert would catch frequencies higher than we know how to support, or any table overrun bug.
        ASSERT (LinkFrequency < (sizeof (HtPhyDllCompLookupTable) / sizeof (HT_PHY_DLL_COMP_LOOKUP_TABLE)));
        // Since there are invalid entries in the table, for frequency enum skipped values, ensure we did not
        // pick one of those entries.  This should be impossible from real hardware.
        ASSERT (HtPhyDllCompLookupTable[LinkFrequency].DefaultComp != 0xFFFFFFFFul);

        if (IsHardwareReportingComp) {
          LibAmdPciReadBits (
            PciAddress,
            HtPhyDllCompLookupTable[LinkFrequency].CtlIndexHiBit,
            HtPhyDllCompLookupTable[LinkFrequency].CtlIndexLoBit,
            &Compensation,
            StdHeader);
        } else {
          Compensation = HtPhyDllCompLookupTable[LinkFrequency].DefaultComp;
        }

        // Apply any per PHY adjustment
        LinkPhyReceiverProcessFuseControl.Value = F15GetHtPhyRegister (
          CapabilitySet,
          Link,
          ((Sublink == 0) ? HT_PHY_FUSE_PROC_DLL_PROCESS_COMP_RD_SL0 : HT_PHY_FUSE_PROC_DLL_PROCESS_COMP_RD_SL1),
          StdHeader);
        Adjustment = LinkPhyReceiverProcessFuseControl.Fields.DllProcessComp10;
        IsIncrementAdjust = (BOOLEAN) ((LinkPhyReceiverProcessFuseControl.Fields.DllProcessComp2 == 0) ? TRUE : FALSE);
        if (IsIncrementAdjust) {
          Compensation = (((Compensation + Adjustment) > 0x000F) ? 0x000F : (Compensation + Adjustment));
        } else {
          // decrement adjustment
          Compensation = ((Compensation < Adjustment) ? 0 : (Compensation - Adjustment));
        }

        // Update the DLL Compensation
        LinkPhyReceiverProcessDllControl.Value = F15GetHtPhyRegister (
          CapabilitySet,
          Link,
          HT_PHY_LINK_PHY_RECEIVER_PROCESS_DLL_CONTROL_RD,
          StdHeader);
        LinkPhyReceiverProcessDllControl.Fields.DllProcessFreqCtlOverride = 1;
        LinkPhyReceiverProcessDllControl.Fields.DllProcessFreqCtlIndex2 = Compensation;
        F15WriteOnlyHtPhyRegister (
          CapabilitySet,
          Link,
          ((Sublink == 0) ? HT_PHY_LINK_PHY_RECEIVER_PROCESS_DLL_CONTROL_SL0 : HT_PHY_LINK_PHY_RECEIVER_PROCESS_DLL_CONTROL_SL1),
          LinkPhyReceiverProcessDllControl.Value,
          StdHeader);
      }
    }
  }
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
F15CommonGetNbCofVidUpdate (
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
F15IsNbPstateEnabled (
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

  PciAddress.AddressValue = F15_NB_PSTATE_CTRL_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
  if ((((((F15_NB_PSTATE_CTRL_REGISTER *) &LocalPciRegister)->NbPstateMaxVal != 0) &&
      (!IsNonCoherentHt1 (StdHeader))) || SkipHwCfg) && (PowerMode)) {
    return TRUE;
  }
  return FALSE;
}
