/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_16 Kabini PSI Initialization
 *
 * Enables Power Status Indicator (PSI) feature
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x16/KB
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
#include "cpuRegisters.h"
#include "cpuF16PowerMgmt.h"
#include "F16KbPowerMgmt.h"
#include "cpuFeatures.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "cpuFamilyTranslation.h"
#include "CommonReturns.h"
#include "cpuPsi.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_CPU_FAMILY_0X16_KB_F16KBPSI_FILECODE

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
F16KbPmVrmLowPowerModeEnable (
  IN       CPU_SPECIFIC_SERVICES    *FamilySpecificServices,
  IN       PLATFORM_CONFIGURATION   *PlatformConfig,
  IN       PCI_ADDR                 PciAddress,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * Entry point for enabling Power Status Indicator
 *
 * This function must be run after all P-State routines have been executed
 *
 * @param[in]  PsiServices             The current CPU's family services.
 * @param[in]  EntryPoint              Timepoint designator.
 * @param[in]  PlatformConfig          Contains the runtime modifiable feature input data.
 * @param[in]  StdHeader               Config handle for library and services.
 *
 * @retval     AGESA_SUCCESS           Always succeeds.
 *
 */
AGESA_STATUS
STATIC
F16KbInitializePsi (
  IN       PSI_FAMILY_SERVICES    *PsiServices,
  IN       UINT64                  EntryPoint,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  PCI_ADDR                  PciAddress;
  CPU_SPECIFIC_SERVICES     *FamilySpecificServices;

  if ((EntryPoint & (CPU_FEAT_AFTER_POST_MTRR_SYNC | CPU_FEAT_AFTER_RESUME_MTRR_SYNC)) != 0) {
    GetCpuServicesOfCurrentCore ((CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
    PciAddress.AddressValue = MAKE_SBDFO (0, 0, 24, 0, 0);
    // Configure PsiVid
    F16KbPmVrmLowPowerModeEnable (FamilySpecificServices, PlatformConfig, PciAddress, StdHeader);
  }

  return AGESA_SUCCESS;
}


/*---------------------------------------------------------------------------------------*/
/**
 * Sets up PSI_L operation.
 *
 * This function implements the LowPowerThreshold parameter.
 *
 * @param[in]  FamilySpecificServices  The current Family Specific Services.
 * @param[in]  PlatformConfig          Contains the runtime modifiable feature input data.
 * @param[in]  PciAddress              Segment, bus, device number of the node to transition.
 * @param[in]  StdHeader               Config handle for library and services.
 *
 */
VOID
STATIC
F16KbPmVrmLowPowerModeEnable (
  IN       CPU_SPECIFIC_SERVICES    *FamilySpecificServices,
  IN       PLATFORM_CONFIGURATION   *PlatformConfig,
  IN       PCI_ADDR                 PciAddress,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  )
{
  PSTATE_MSR                     PstateMsr;
  CLK_PWR_TIMING_CTRL2_REGISTER  ClkPwrTimingCtrl2;
  POWER_CTRL_MISC_REGISTER       PwrCtrlMisc;
  UINT32                         CoreVrmLowPowerThreshold;
  UINT32                         Pstate;
  UINT32                         HwPstateMaxVal;
  UINT32                         PstateCurrent;
  UINT32                         NextPstateCurrent;
  UINT32                         PreviousVid;
  UINT32                         CurrentVid;

  NB_PSTATE_REGISTER             NbPstateReg;
  NB_PSTATE_CTRL_REGISTER        NbPsCtrl;
  MISC_VOLTAGE_REGISTER          MiscVoltageReg;
  UINT32                         NbVrmLowPowerThreshold;
  UINT32                         NbPstate;
  UINT32                         NbPstateMaxVal;
  UINT32                         NbPstateCurrent;
  UINT32                         NextNbPstateCurrent;
  UINT32                         PreviousNbVid;
  UINT32                         CurrentNbVid;

  IDS_HDT_CONSOLE (CPU_TRACE, "  F16KbPmVrmLowPowerModeEnable\n");

  if (PlatformConfig->VrmProperties[CoreVrm].LowPowerThreshold != 0) {
    // Set up PSI0_L for VDD
    CoreVrmLowPowerThreshold = PlatformConfig->VrmProperties[CoreVrm].LowPowerThreshold;
    IDS_HDT_CONSOLE (CPU_TRACE, "    Core VRM - LowPowerThreshold: %d\n", CoreVrmLowPowerThreshold);
    PreviousVid = 0xFF;
    CurrentVid = 0xFF;

    PciAddress.AddressValue = CPTC2_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &ClkPwrTimingCtrl2, StdHeader);
    HwPstateMaxVal = ClkPwrTimingCtrl2.HwPstateMaxVal;
    ASSERT (HwPstateMaxVal < NM_PS_REG);

    IDS_HDT_CONSOLE (CPU_TRACE, "    HwPstateMaxVal %d\n", HwPstateMaxVal);
    // Check P-state from P0 to HwPstateMaxVal
    for (Pstate = 0; Pstate <= HwPstateMaxVal; Pstate++) {
      FamilySpecificServices->GetProcIddMax (FamilySpecificServices, (UINT8) Pstate, &PstateCurrent, StdHeader);

      LibAmdMsrRead ((UINT32) (Pstate + PS_REG_BASE), (UINT64 *) &PstateMsr, StdHeader);
      CurrentVid = (UINT32) PstateMsr.CpuVid;

      if (Pstate == HwPstateMaxVal) {
        NextPstateCurrent = 0;
      } else {
        // Check P-state from P1 to HwPstateMaxVal
        FamilySpecificServices->GetProcIddMax (FamilySpecificServices, (UINT8) (Pstate + 1), &NextPstateCurrent, StdHeader);
      }

      if ((PstateCurrent <= CoreVrmLowPowerThreshold) &&
          (NextPstateCurrent <= CoreVrmLowPowerThreshold) &&
          (CurrentVid != PreviousVid)) {
        // Program PsiVid and PsiVidEn if PSI state is found and stop searching.
        PciAddress.AddressValue = PW_CTL_MISC_PCI_ADDR;
        LibAmdPciRead (AccessWidth32, PciAddress, &PwrCtrlMisc, StdHeader);
        PwrCtrlMisc.PsiVid = CurrentVid;
        PwrCtrlMisc.PsiVidEn = 1;
        LibAmdPciWrite (AccessWidth32, PciAddress, &PwrCtrlMisc, StdHeader);
        IDS_HDT_CONSOLE (CPU_TRACE, "    PsiVid is enabled at P-state %d. PsiVid: %d\n", Pstate, CurrentVid);
        break;
      } else {
        PstateCurrent = NextPstateCurrent;
        PreviousVid = CurrentVid;
      }
    }
  }

  if (PlatformConfig->VrmProperties[NbVrm].LowPowerThreshold != 0) {
    // Set up NBPSI0_L for VDDNB
    NbVrmLowPowerThreshold = PlatformConfig->VrmProperties[NbVrm].LowPowerThreshold;
    IDS_HDT_CONSOLE (CPU_TRACE, "    NB VRM - LowPowerThreshold: %d\n", NbVrmLowPowerThreshold);
    PreviousNbVid = 0xFF;
    CurrentNbVid = 0xFF;

    PciAddress.AddressValue = NB_PSTATE_CTRL_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &NbPsCtrl, StdHeader);
    NbPstateMaxVal = NbPsCtrl.NbPstateMaxVal;
    ASSERT (NbPstateMaxVal < NM_NB_PS_REG);

    IDS_HDT_CONSOLE (CPU_TRACE, "    NbPstateMaxVal %d\n", NbPstateMaxVal);
    for (NbPstate = 0; NbPstate <= NbPstateMaxVal; NbPstate++) {
      // Check only valid NB P-state
      if (FamilySpecificServices->GetNbIddMax (FamilySpecificServices, (UINT8) NbPstate, &NbPstateCurrent, StdHeader) != TRUE) {
        continue;
      }

      PciAddress.Address.Register = (NB_PSTATE_0 + (sizeof (NB_PSTATE_REGISTER) * NbPstate));
      LibAmdPciRead (AccessWidth32, PciAddress, &NbPstateReg, StdHeader);
      CurrentNbVid = (UINT32) GetF16KbNbVid (&NbPstateReg);

      if (NbPstate == NbPstateMaxVal) {
        NextNbPstateCurrent = 0;
      } else {
        // Check only valid NB P-state
        if (FamilySpecificServices->GetNbIddMax (FamilySpecificServices, (UINT8) (NbPstate + 1), &NextNbPstateCurrent, StdHeader) != TRUE) {
          continue;
        }
      }

      if ((NbPstateCurrent <= NbVrmLowPowerThreshold) &&
          (NextNbPstateCurrent <= NbVrmLowPowerThreshold) &&
          (CurrentNbVid != PreviousNbVid)) {
        // Program NbPsi0Vid and NbPsi0VidEn if PSI state is found and stop searching.
        PciAddress.AddressValue = MISC_VOLTAGES_PCI_ADDR;
        LibAmdPciRead (AccessWidth32, PciAddress, &MiscVoltageReg, StdHeader);
        MiscVoltageReg.NbPsi0Vid = CurrentNbVid;
        MiscVoltageReg.NbPsi0VidEn = 1;
        LibAmdPciWrite (AccessWidth32, PciAddress, &MiscVoltageReg, StdHeader);
        IDS_HDT_CONSOLE (CPU_TRACE, "    NbPsi0Vid is enabled at NB P-state %d. NbPsi0Vid: %d\n", NbPstate, CurrentNbVid);
        break;
      } else {
        PreviousNbVid = CurrentNbVid;
      }
    }
  }
}


CONST PSI_FAMILY_SERVICES ROMDATA F16KbPsiSupport =
{
  0,
  (PF_PSI_IS_SUPPORTED) CommonReturnTrue,
  F16KbInitializePsi
};
