/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_14 specific utility functions.
 *
 * Provides numerous utility functions specific to family 10h.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/F14
 * @e \$Revision: 39275 $   @e \$Date: 2010-10-09 08:22:05 +0800 (Sat, 09 Oct 2010) $
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
#include "cpuCommonF14Utilities.h"
#include "cpuF14PowerMgmt.h"
#include "OptionFamily14hEarlySample.h"
#include "NbSmuLib.h"
#include "GnbRegistersON.h"
#include "Filecode.h"
#define FILECODE PROC_CPU_FAMILY_0X14_CPUCOMMONF14UTILITIES_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern F14_ES_CORE_SUPPORT F14EarlySampleCoreSupport;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */
CONST UINT16 ROMDATA F14MaxNbFreqAtMinVidFreqTable[] =
{
  25,         // 00000b
  50,         // 00001b
  100,        // 00010b
  150,        // 00011b
  167,        // 00100b
  183,        // 00101b
  200,        // 00110b
  217,        // 00111b
  233,        // 01000b
  250,        // 01001b
  267,        // 01010b
  283,        // 01011b
  300,        // 01100b
  317,        // 01101b
  333,        // 01110b
  350,        // 01111b
  366,        // 10000b
  383,        // 10001b
  400,        // 10010b
  417,        // 10011b
  433,        // 10100b
  450,        // 10101b
  467,        // 10110b
  483,        // 10111b
  500,        // 11000b
  517,        // 11001b
  533,        // 11010b
  550,        // 11011b
  563,        // 11100b
  575,        // 11101b
  588,        // 11110b
  600         // 11111b
};

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
UINT32
F14GetApCoreNumber (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

CORE_ID_POSITION
F14CpuAmdCoreIdPositionInInitialApicId (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

UINT32
STATIC
RoundedDivision (
  IN       UINT32 Dividend,
  IN       UINT32 Divisor
  );
/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

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
F14SetAgesaWarmResetFlag (
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
 *  @param[in]   FamilySpecificServices   The current Family Specific Services.
 *  @param[in]   StdHeader                Config handle for library and services
 *  @param[out]  Request                  Indicate warm reset status
 *
 */
VOID
F14GetAgesaWarmResetFlag (
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
 *  Use the Mailbox Register to get the Ap Mailbox info for the current core.
 *
 *  @CpuServiceMethod{::F_CPU_AMD_GET_AP_MAILBOX_FROM_HARDWARE}.
 *
 *  Access the mailbox register used with this NB family.  This is valid until the
 *  point that some init code initializes the mailbox register for its normal use.
 *
 *  @param[in]     FamilySpecificServices  The current Family Specific Services.
 *  @param[out]    ApMailboxInfo           The AP Mailbox info
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 */
VOID
F14GetApMailboxFromHardware (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
     OUT   AP_MAILBOXES           *ApMailboxInfo,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  // For Family 14h, we will return socket 0, node 0, module 0, module type 0, and 0 for
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
 *  Returns the system core number.  For family 14h, this is simply the
 *  initial APIC ID.
 *
 *  @param[in]     FamilySpecificServices  The current Family Specific Services.
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 *  @return        The AP's unique core number
 */
UINT32
F14GetApCoreNumber (
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
F14CpuAmdCoreIdPositionInInitialApicId (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  return (CoreIdPositionOne);
}

/*---------------------------------------------------------------------------------------*/
/**
 * Sets up a valid set of NB P-states based on the value of MEMCLK, transitions
 * to the desired NB P-state, and returns the current NB frequency in megahertz.
 *
 * @param[in]     TargetMemclk            The target MEMCLK in megahertz, or zero to
 *                                        indicate NB P-state change only.
 * @param[in]     TargetMemclkEncoded     The target MEMCLK's register encoding.
 * @param[in]     TargetNbPstate          The NB P-state to exit in.
 * @param[in]     CurrentNbFreq           Current NB operating frequency in megahertz.
 * @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 * @retval        TRUE                    Transition to TargetNbPstate was successful.
 * @retval        FALSE                   Transition to TargetNbPstate was unsuccessful.
 */
BOOLEAN
F14NbPstateInit (
  IN       UINT32             TargetMemclk,
  IN       UINT32             TargetMemclkEncoded,
  IN       UINT32             TargetNbPstate,
     OUT   UINT32             *CurrentNbFreq,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT32                EncodedNbPs1Vid;
  UINT32                EncodedNbPs0NclkDiv;
  UINT32                EncodedNbPs1NclkDiv;
  UINT32                NbP0Cof;
  UINT32                NbP1Cof;
  UINT32                NbPstateNumerator;
  UINT32                TargetNumerator;
  UINT32                TargetDenominator;
  BOOLEAN               ReturnStatus;
  BOOLEAN               WaitForTransition;
  PCI_ADDR              PciAddress;
  D18F3xD4_STRUCT       Cptc0;
  D18F3xDC_STRUCT       Cptc2;
  D18F6x90_STRUCT       NbPsCfgLow;
  D18F6x98_STRUCT       NbPsCtrlSts;
  FCRxFE00_6000_STRUCT  FCRxFE00_6000;
  FCRxFE00_6002_STRUCT  FCRxFE00_6002;
  FCRxFE00_7006_STRUCT  FCRxFE00_7006;
  FCRxFE00_7009_STRUCT  FCRxFE00_7009;

  // F14 only supports NB P0 and NB P1
  ASSERT (TargetNbPstate < 2);

  WaitForTransition = FALSE;
  ReturnStatus = TRUE;

  // Get D18F3xD4[MainPllOpFreqId] frequency
  PciAddress.AddressValue = CPTC0_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, PciAddress, &Cptc0.Value, StdHeader);

  // Calculate the numerator to be used for NB P-state calculations
  NbPstateNumerator = (UINT32) (4 * ((Cptc0.Field.MainPllOpFreqId + 0x10) * 100));

  if (TargetMemclk != 0) {
    // Determine the appropriate numerator / denominator of the target memclk
    switch (TargetMemclk) {
    case DDR800_FREQUENCY:
      TargetNumerator = 400;
      TargetDenominator = 1;
      break;
    case DDR1066_FREQUENCY:
      TargetNumerator = 1600;
      TargetDenominator = 3;
      break;
    case DDR1333_FREQUENCY:
      TargetNumerator = 2000;
      TargetDenominator = 3;
      break;
    default:
      // An invalid memclk has been passed in.
      ASSERT (FALSE);
      TargetNumerator = TargetMemclk;
      TargetDenominator = 1;
      break;
    }

    FCRxFE00_6000.Value = NbSmuReadEfuse (FCRxFE00_6000_ADDRESS, StdHeader);
    FCRxFE00_6002.Value = NbSmuReadEfuse (FCRxFE00_6002_ADDRESS, StdHeader);
    FCRxFE00_7006.Value = NbSmuReadEfuse (FCRxFE00_7006_ADDRESS, StdHeader);
    FCRxFE00_7009.Value = NbSmuReadEfuse (FCRxFE00_7009_ADDRESS, StdHeader);

    F14EarlySampleCoreSupport.F14NbPstateInitHook (&FCRxFE00_6000,
                                                   &FCRxFE00_6002,
                                                   &FCRxFE00_7006,
                                                   &FCRxFE00_7009,
                                                   NbPstateNumerator,
                                                   StdHeader);

    // Determine NB P0 settings
    if ((TargetNumerator * FCRxFE00_7009.Field.NbPs0NclkDiv) < (NbPstateNumerator * TargetDenominator)) {
      // Program D18F3xDC[NbPs0NclkDiv] to the minimum divisor where
      // (target memclk frequency >= (D18F3xD4[MainPllOpFreqId] freq) / divisor)
      EncodedNbPs0NclkDiv = ((NbPstateNumerator * TargetDenominator) / TargetNumerator);
      if (((NbPstateNumerator * TargetDenominator) % TargetNumerator) != 0) {
        EncodedNbPs0NclkDiv++;
      }
      // Ensure that the encoded divisor is even to give 50% duty cycle
      EncodedNbPs0NclkDiv = ((EncodedNbPs0NclkDiv + 1) & 0xFFFFFFFE);

      ASSERT (EncodedNbPs0NclkDiv >= 8);
      ASSERT (EncodedNbPs0NclkDiv <= 0x3F);
    } else {
      EncodedNbPs0NclkDiv = FCRxFE00_7009.Field.NbPs0NclkDiv;
    }

    // Check to see if the DIMMs are too fast for the CPU (NB P0 COF < (Memclk / 2))
    if ((TargetNumerator * EncodedNbPs0NclkDiv) > (NbPstateNumerator * TargetDenominator * 2)) {
      // Indicate the error to the memory code so the DIMMs can be derated.
      ReturnStatus = FALSE;
    }

    // Apply the appropriate P0 frequency
    PciAddress.AddressValue = CPTC2_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &Cptc2.Value, StdHeader);
    if (Cptc2.Field.NbPs0NclkDiv != EncodedNbPs0NclkDiv) {
      WaitForTransition = TRUE;
    Cptc2.Field.NbPs0NclkDiv = EncodedNbPs0NclkDiv;
    LibAmdPciWrite (AccessWidth32, PciAddress, &Cptc2.Value, StdHeader);
    }
    NbP0Cof = RoundedDivision (NbPstateNumerator, EncodedNbPs0NclkDiv);

    // Determine NB P1 settings if necessary
    PciAddress.AddressValue = NB_PSTATE_CFG_LOW_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &NbPsCfgLow.Value, StdHeader);
    if (NbPsCfgLow.Field.NbPsCap == 1) {
      if ((TargetNumerator * FCRxFE00_7006.Field.NbPs1NclkDiv) > (NbPstateNumerator * TargetDenominator * 2)) {
        // Program D18F6x90[NbPs1NclkDiv] to the maximum divisor where
        // (target memclk frequency / 2 <= (D18F3xD4[MainPllOpFreqId] freq) / divisor)
        EncodedNbPs1NclkDiv = ((NbPstateNumerator * TargetDenominator * 2) / TargetNumerator);

        // Ensure that the encoded divisor is even to give 50% duty cycle
        EncodedNbPs1NclkDiv &= 0xFFFFFFFE;
        ASSERT (EncodedNbPs1NclkDiv >= 8);
        ASSERT (EncodedNbPs1NclkDiv <= 0x3F);

        // Calculate the new effective P1 frequency to determine the voltage
        NbP1Cof = RoundedDivision (NbPstateNumerator, EncodedNbPs1NclkDiv);

        if (NbP1Cof <= F14MaxNbFreqAtMinVidFreqTable[FCRxFE00_7006.Field.MaxNbFreqAtMinVid]) {
          // Program D18F6x90[NbPs1Vid] = FCRxFE00_6002[NbPs1VidAddl]
          EncodedNbPs1Vid = FCRxFE00_6002.Field.NbPs1VidAddl;
        } else {
          // Program D18F6x90[NbPs1Vid] = FCRxFE00_6002[NbPs1VidHigh]
          EncodedNbPs1Vid = FCRxFE00_6002.Field.NbPs1VidHigh;
        }
      } else {
        // Fused frequency and voltage are legal
        EncodedNbPs1Vid = FCRxFE00_6000.Field.NbPs1Vid;
        EncodedNbPs1NclkDiv = FCRxFE00_7006.Field.NbPs1NclkDiv;
        NbP1Cof = RoundedDivision (NbPstateNumerator, EncodedNbPs1NclkDiv);
      }

      if (NbP0Cof < NbP1Cof) {
        // NB P1 frequency is faster than NB P0.  Fix it up by slowing
        // P1 to match P0.
        EncodedNbPs1NclkDiv  = EncodedNbPs0NclkDiv;
        NbP1Cof = NbP0Cof;
      }

      // Program the new NB P1 settings
      NbPsCfgLow.Field.NbPs1NclkDiv = EncodedNbPs1NclkDiv;
      NbPsCfgLow.Field.NbPs1Vid = EncodedNbPs1Vid;
      LibAmdPciWrite (AccessWidth32, PciAddress, &NbPsCfgLow.Value, StdHeader);
    } else {
      // NB P-states are not enabled
      NbP1Cof = 0;
    }
    *CurrentNbFreq = NbP0Cof;
    if (WaitForTransition) {
      // Ensure that the frequency has settled before returning to memory code.
      PciAddress.AddressValue = CPTC2_PCI_ADDR;
      do {
        LibAmdPciRead (AccessWidth32, PciAddress, &Cptc2.Value, StdHeader);
      } while (Cptc2.Field.NclkFreqDone != 1);
    }
  } else {
    // Get NB P0 COF
    PciAddress.AddressValue = CPTC2_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &Cptc2.Value, StdHeader);
    NbP0Cof = RoundedDivision (NbPstateNumerator, Cptc2.Field.NbPs0NclkDiv);

    // Read NB P-state status
    PciAddress.AddressValue = NB_PSTATE_CTRL_STS_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &NbPsCtrlSts.Value, StdHeader);

    // Read low config register
    PciAddress.AddressValue = NB_PSTATE_CFG_LOW_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &NbPsCfgLow.Value, StdHeader);
    if (TargetNbPstate == 1) {
      // If target is P1, the CPU MUST be in P0, otherwise the P1 settings
      // cannot be realized.  This is a programming error.
      ASSERT (NbPsCtrlSts.Field.NbPs1Act == 0);

      if (NbPsCfgLow.Field.NbPsCap == 1) {
        // The part is capable of NB P-states.  Transition to P1.
        NbPsCfgLow.Field.NbPsForceSel = 1;
        LibAmdPciWrite (AccessWidth32, PciAddress, &NbPsCfgLow.Value, StdHeader);

        WaitForTransition = TRUE;
        *CurrentNbFreq = RoundedDivision (NbPstateNumerator, NbPsCfgLow.Field.NbPs1NclkDiv);
      } else {
        // No NB P-states.  Return FALSE, and set current frequency to P0.
        *CurrentNbFreq = NbP0Cof;
        ReturnStatus = FALSE;
      }
    } else {
      // Target P0
      *CurrentNbFreq = NbP0Cof;
      if (NbPsCtrlSts.Field.NbPs1Act != 0) {
        // Request transition to P0
        NbPsCfgLow.Field.NbPsForceSel = 0;
        LibAmdPciWrite (AccessWidth32, PciAddress, &NbPsCfgLow.Value, StdHeader);
        WaitForTransition = TRUE;
      }
    }
    if (WaitForTransition) {
  // Ensure that the frequency has settled before returning to memory code.
      PciAddress.AddressValue = NB_PSTATE_CTRL_STS_PCI_ADDR;
  do {
        LibAmdPciRead (AccessWidth32, PciAddress, &NbPsCtrlSts.Value, StdHeader);
      } while (NbPsCtrlSts.Field.NbPs1Act != TargetNbPstate);
    }
  }

  return ReturnStatus;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Performs integer division, and rounds the quotient up if the remainder is greater
 * than or equal to 50% of the divisor.
 *
 * @param[in]     Dividend                The target MEMCLK in megahertz.
 * @param[in]     Divisor                 The target MEMCLK's register encoding.
 *
 * @return        The rounded quotient
 */
UINT32
STATIC
RoundedDivision (
  IN       UINT32 Dividend,
  IN       UINT32 Divisor
  )
{
  UINT32 Quotient;

  ASSERT (Divisor != 0);

  Quotient = Dividend / Divisor;
  if (((Dividend % Divisor) * 2) >= Divisor) {
    Quotient++;
  }
  return Quotient;
}
