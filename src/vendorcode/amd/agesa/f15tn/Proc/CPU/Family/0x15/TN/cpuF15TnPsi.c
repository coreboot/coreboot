/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 TN PSI Initialization
 *
 * Enables Power Status Indicator (PSI) feature
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
 * Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *
 * AMD is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with AMD.  This header does *NOT* give you permission to use the Materials
 * or any rights under AMD's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 *
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by AMD shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 *
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 *
 * AMD does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by AMD, or
 * result from use of the Materials or any related information.
 *
 * You agree that you will not reverse engineer or decompile the Materials.
 *
 * NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, AMD retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgement of AMD's proprietary rights in them.
 *
 * EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
 * direct product thereof will be exported directly or indirectly, into any
 * country prohibited by the United States Export Administration Act and the
 * regulations thereunder, without the required authorization from the U.S.
 * government nor will be used for any purpose prohibited by the same.
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "cpuRegisters.h"
#include "cpuF15PowerMgmt.h"
#include "cpuF15TnPowerMgmt.h"
#include "cpuFeatures.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "cpuFamilyTranslation.h"
#include "CommonReturns.h"
#include "cpuPsi.h"
#include "OptionMultiSocket.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_CPU_FAMILY_0X15_TN_CPUF15TNPSI_FILECODE

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
F15TnPmVrmLowPowerModeEnable (
  IN       CPU_SPECIFIC_SERVICES    *FamilySpecificServices,
  IN       PLATFORM_CONFIGURATION   *PlatformConfig,
  IN       PCI_ADDR                 PciAddress,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
extern OPTION_MULTISOCKET_CONFIGURATION   OptionMultiSocketConfiguration;

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
F15TnInitializePsi (
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
    OptionMultiSocketConfiguration.GetCurrPciAddr (&PciAddress, StdHeader);
    PciAddress.AddressValue = MAKE_SBDFO (0, 0, 24, 0, 0);
    // Configure PsiVid
    F15TnPmVrmLowPowerModeEnable (FamilySpecificServices, PlatformConfig, PciAddress, StdHeader);
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
F15TnPmVrmLowPowerModeEnable (
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

  IDS_HDT_CONSOLE (CPU_TRACE, "  F15TnPmVrmLowPowerModeEnable\n");

  if (PlatformConfig->VrmProperties[CoreVrm].LowPowerThreshold != 0) {
    // Set up PSI0_L for VDD
    CoreVrmLowPowerThreshold = PlatformConfig->VrmProperties[CoreVrm].LowPowerThreshold;
    IDS_HDT_CONSOLE (CPU_TRACE, "    Core VRM - LowPowerThreshold: %d\n", CoreVrmLowPowerThreshold);
    PreviousVid = 0xFF;
    CurrentVid = 0xFF;

    PciAddress.AddressValue = CPTC2_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &ClkPwrTimingCtrl2, StdHeader);
    HwPstateMaxVal = ClkPwrTimingCtrl2.PstateMaxVal;
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
      CurrentNbVid = (UINT32) GetF15TnNbVid (&NbPstateReg);

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


CONST PSI_FAMILY_SERVICES ROMDATA F15TnPsiSupport =
{
  0,
  (PF_PSI_IS_SUPPORTED) CommonReturnTrue,
  F15TnInitializePsi
};
