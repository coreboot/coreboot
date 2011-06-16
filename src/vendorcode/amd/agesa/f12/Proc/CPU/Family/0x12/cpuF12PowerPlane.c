/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_12 Power Plane Initialization
 *
 * Performs the "BIOS Requirements for Power Plane Initialization" as described
 * in the BKDG.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/F12
 * @e \$Revision: 47330 $   @e \$Date: 2011-02-18 10:39:06 +0800 (Fri, 18 Feb 2011) $
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
#include "cpuServices.h"
#include "cpuF12PowerMgmt.h"
#include "cpuF12PowerPlane.h"
#include "OptionFamily12hEarlySample.h"
#include "GnbRegistersLN.h"
#include "NbSmuLib.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_CPU_FAMILY_0X12_CPUF12POWERPLANE_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern F12_ES_CORE_SUPPORT F12EarlySampleCoreSupport;

// Register encodings for D18F3xD8[VSRampSlamTime]
STATIC CONST UINT32 ROMDATA F12VSRampSlamWaitTimes[8] =
{
  625,    // 000b: 6.25us
  500,    // 001b: 5.00us
  417,    // 010b: 4.17us
  313,    // 011b: 3.13us
  250,    // 100b: 2.50us
  167,    // 101b: 1.67us
  125,    // 110b: 1.25us
  100     // 111b: 1.00us
};

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
F12PmVrmLowPowerModeEnable (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       AMD_CPU_EARLY_PARAMS  *CpuEarlyParams,
  IN       PCI_ADDR              PciAddress,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  );


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * Family 12h core 0 entry point for performing power plane initialization.
 *
 * The steps are as follows:
 *    1. BIOS must initialize D18F3xD8[VSRampSlamTime].
 *    2. BIOS must configure D18F3xA0[PsiVidEn & PsiVid] and
 *       D18F3x128[NbPsiVidEn & NbPsiVid].
 *    3. BIOS must program D18F3xDC[NbPs0Vid] = FCRxFE00_6000[NbPs0Vid] - 1.
 *       BIOS must program D18F3xDC[NbPs0Vid] = FCRxFE00_6000[NbPs0Vid].
 *
 * @param[in]  FamilySpecificServices  The current Family Specific Services.
 * @param[in]  CpuEarlyParams          Service parameters
 * @param[in]  StdHeader               Config handle for library and services.
 *
 */
VOID
F12PmPwrPlaneInit (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       AMD_CPU_EARLY_PARAMS  *CpuEarlyParams,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINT32                SystemSlewRate;
  UINT32                WaitTime;
  UINT32                VSRampSlamTime;
  UINT32                LocalPciRegister;
  UINT32                VoltageDifference;
  UINT32                SingleVidStepTransitionTime;
  UINT32                TransitionTime;
  PCI_ADDR              PciAddress;
  FCRxFE00_6000_STRUCT  FCRxFE00_6000;

  // Step 1 - Configure D18F3xD8[VSRampSlamTime] based on platform requirements.
  // Voltage Ramp Time = maximum time to change voltage by 12.5mV rounded to the next higher encoding.
  SystemSlewRate = (CpuEarlyParams->PlatformConfig.VrmProperties[CoreVrm].SlewRate <=
                    CpuEarlyParams->PlatformConfig.VrmProperties[NbVrm].SlewRate) ?
                    CpuEarlyParams->PlatformConfig.VrmProperties[CoreVrm].SlewRate :
                    CpuEarlyParams->PlatformConfig.VrmProperties[NbVrm].SlewRate;

  ASSERT (SystemSlewRate != 0);

  // First, calculate the time it takes to change 12.5mV using the VRM slew rate.
  WaitTime = (12500 * 100) / SystemSlewRate;
  if (((12500 * 100) % SystemSlewRate) != 0) {
    WaitTime++;
  }

  // Next, round it to the appropriate encoded value.  We will start from encoding 111b which corresponds
  // to the fastest slew rate, and work our way down to 000b, which represents the slowest an acceptable
  // VRM can be.
  for (VSRampSlamTime = ((sizeof (F12VSRampSlamWaitTimes) / sizeof (F12VSRampSlamWaitTimes[0])) - 1); VSRampSlamTime > 0; VSRampSlamTime--) {
    if (WaitTime <= F12VSRampSlamWaitTimes[VSRampSlamTime]) {
      break;
    }
  }

  if (WaitTime > F12VSRampSlamWaitTimes[0]) {
    // The VRMs on this motherboard are too slow for this CPU.
    IDS_ERROR_TRAP;
  }

  // Lastly, program D18F3xD8[VSRampSlamTime] with the appropriate encoded value.
  PciAddress.AddressValue = CPTC1_PCI_ADDR;
  LibAmdPciWriteBits (PciAddress, CPTC1_VSRAMPSLAMTIME_END, CPTC1_VSRAMPSLAMTIME_START, &VSRampSlamTime, StdHeader);

  // Step 2 - Configure D18F3xA0[PsiVidEn & PsiVid] and D18F3x128[NbPsiVidEn & NbPsiVid].
  F12PmVrmLowPowerModeEnable (FamilySpecificServices, CpuEarlyParams, PciAddress, StdHeader);

  // Step 3 - Program D18F3xDC[NbPs0Vid] = FCRxFE00_6000[NbPs0Vid] - 1.
  //          Wait out the appropriate voltage stabilization time.
  //          Program D18F3xDC[NbPs0Vid] = FCRxFE00_6000[NbPs0Vid].
  //          Wait out the appropriate voltage stabilization time.
  FCRxFE00_6000.Value = NbSmuReadEfuse (FCRxFE00_6000_ADDRESS, StdHeader);

  F12EarlySampleCoreSupport.F12PowerPlaneInitHook (&FCRxFE00_6000, StdHeader);

  PciAddress.AddressValue = CPTC2_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
  if (((CLK_PWR_TIMING_CTRL2_REGISTER *) &LocalPciRegister)->NbPs0Vid >= FCRxFE00_6000.Field.NbPs0Vid) {
    VoltageDifference = ((((CLK_PWR_TIMING_CTRL2_REGISTER *) &LocalPciRegister)->NbPs0Vid - FCRxFE00_6000.Field.NbPs0Vid) + 1);
  } else {
    VoltageDifference = ((FCRxFE00_6000.Field.NbPs0Vid - ((CLK_PWR_TIMING_CTRL2_REGISTER *) &LocalPciRegister)->NbPs0Vid) - 1);
  }
  SingleVidStepTransitionTime = WaitTime / 100;
  if ((WaitTime % 100) != 0) {
    SingleVidStepTransitionTime++;
  }
  TransitionTime = SingleVidStepTransitionTime * VoltageDifference;

  ((CLK_PWR_TIMING_CTRL2_REGISTER *) &LocalPciRegister)->NbPs0Vid = FCRxFE00_6000.Field.NbPs0Vid - 1;
  LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);

  WaitMicroseconds (TransitionTime, StdHeader);

  ((CLK_PWR_TIMING_CTRL2_REGISTER *) &LocalPciRegister)->NbPs0Vid = FCRxFE00_6000.Field.NbPs0Vid;
  LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);

  WaitMicroseconds (SingleVidStepTransitionTime, StdHeader);
}

/*---------------------------------------------------------------------------------------*/
/**
 * Sets up PSI_L operation.
 *
 * This function implements the AMD_CPU_EARLY_PARAMS.VrmLowPowerThreshold parameter.
 *
 * @param[in]  FamilySpecificServices  The current Family Specific Services.
 * @param[in]  CpuEarlyParams          Contains VrmLowPowerThreshold parameter.
 * @param[in]  PciAddress              PCI address of the executing core's config space.
 * @param[in]  StdHeader               Config handle for library and services.
 *
 */
VOID
STATIC
F12PmVrmLowPowerModeEnable (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       AMD_CPU_EARLY_PARAMS  *CpuEarlyParams,
  IN       PCI_ADDR              PciAddress,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINT32    Pstate;
  UINT32    PstateMaxVal;
  UINT32    PstateCurrent;
  UINT32    NextPstateCurrent;
  UINT32    NextPstateCurrentRaw;
  UINT32    LocalPciRegister;
  UINT32    PreviousVid;
  UINT32    CurrentVid;
  UINT32    C6Vid;
  UINT32    HwPsMaxVal;
  UINT64    PstateMsr;
  BOOLEAN   IsPsiEnabled;

  // Set up PSI_L for VDD
  IsPsiEnabled = FALSE;
  PreviousVid = 0x7F;
  CurrentVid = 0x7F;
  PciAddress.Address.Function = FUNC_3;
  PciAddress.Address.Register = CPTC2_REG;
  LibAmdPciRead (AccessWidth32, PciAddress, &HwPsMaxVal, StdHeader);

  if (CpuEarlyParams->PlatformConfig.VrmProperties[CoreVrm].LowPowerThreshold != 0) {
    PstateMaxVal = (UINT32) ((CLK_PWR_TIMING_CTRL2_REGISTER *) &HwPsMaxVal)->PstateMaxVal;
    FamilySpecificServices->GetProcIddMax (FamilySpecificServices, (UINT8) 0, &PstateCurrent, StdHeader);
    for (Pstate = 0; Pstate <= PstateMaxVal; Pstate++) {
      LibAmdMsrRead ((UINT32) (Pstate + PS_REG_BASE), &PstateMsr, StdHeader);
      CurrentVid = (UINT32) ((PSTATE_MSR *) &PstateMsr)->CpuVid;
      if (Pstate == PstateMaxVal) {
        NextPstateCurrentRaw = 0;
        NextPstateCurrent = 0;
      } else {
        FamilySpecificServices->GetProcIddMax (FamilySpecificServices, (UINT8) (Pstate + 1), &NextPstateCurrentRaw, StdHeader);
        NextPstateCurrent = NextPstateCurrentRaw + CpuEarlyParams->PlatformConfig.VrmProperties[CoreVrm].InrushCurrentLimit;
      }
      if ((PstateCurrent <= CpuEarlyParams->PlatformConfig.VrmProperties[CoreVrm].LowPowerThreshold) &&
          (NextPstateCurrent <= CpuEarlyParams->PlatformConfig.VrmProperties[CoreVrm].LowPowerThreshold) &&
          (CurrentVid != PreviousVid)) {
        IsPsiEnabled = TRUE;
        break;
      } else {
        PstateCurrent = NextPstateCurrentRaw;
        PreviousVid = CurrentVid;
      }
    }

    // At this point, if IsPsiEnabled is still FALSE, then a suitable threshold
    // is not found.
    if (!IsPsiEnabled) {
      PciAddress.AddressValue = CPTC3_PCI_ADDR;
      LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
      C6Vid = ((CLK_PWR_TIMING_CTRL3_REGISTER *) &LocalPciRegister)->C6Vid;
      // Set threshold to C6Vid and set IsPsiEnabled to TRUE only if C6Vid value
      // is larger than the last seen VID code.
      if (C6Vid > PreviousVid) {
        CurrentVid = C6Vid;
        IsPsiEnabled = TRUE;
      }
    }
  }
  PciAddress.AddressValue = PW_CTL_MISC_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
  if (IsPsiEnabled) {
    ((POWER_CTRL_MISC_REGISTER *) &LocalPciRegister)->PsiVid = CurrentVid;
    ((POWER_CTRL_MISC_REGISTER *) &LocalPciRegister)->PsiVidEn = 1;
  } else {
    ((POWER_CTRL_MISC_REGISTER *) &LocalPciRegister)->PsiVidEn = 0;
  }
  LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);


  // Set up NBPSI_L for VDDNB
  PciAddress.AddressValue = CPTC3_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
  if (CpuEarlyParams->PlatformConfig.VrmProperties[NbVrm].LowPowerThreshold != 0) {
    ((CLK_PWR_TIMING_CTRL3_REGISTER *) &LocalPciRegister)->NbPsiVid = 0;
    ((CLK_PWR_TIMING_CTRL3_REGISTER *) &LocalPciRegister)->NbPsiVidEn = 1;
  } else {
    ((CLK_PWR_TIMING_CTRL3_REGISTER *) &LocalPciRegister)->NbPsiVidEn = 0;
  }
  LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
}
