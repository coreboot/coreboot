/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 Models 0x00 - 0x0F Power Plane Initialization
 *
 * Performs the "BIOS Requirements for Power Plane Initialization" as described
 * in the BKDG.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x15/OR
 * @e \$Revision: 55600 $   @e \$Date: 2011-06-23 12:39:18 -0600 (Thu, 23 Jun 2011) $
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
#include "cpuF15PowerMgmt.h"
#include "cpuF15OrPowerMgmt.h"
#include "cpuApicUtilities.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "cpuFamilyTranslation.h"
#include "Table.h"
#include "OptionMultiSocket.h"
#include "F15OrPowerPlane.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_CPU_FAMILY_0X15_OR_F15ORPOWERPLANE_FILECODE

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
F15OrPmVrmLowPowerModeEnable (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       AMD_CPU_EARLY_PARAMS  *CpuEarlyParams,
  IN       PCI_ADDR              PciAddress,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  );


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
extern OPTION_MULTISOCKET_CONFIGURATION OptionMultiSocketConfiguration;

/*---------------------------------------------------------------------------------------*/
/**
 * Family 15h core 0 entry point for performing power plane initialization.
 *
 * The steps are as follows:
 *    1. Configure D18F3xD8[VSRampSlamTime] based on platform
 *       requirements.
 *    2. Configure F3xD4[PowerStepUp & PowerStepDown]
 *    3. Optionally configure F3xA0[PsiVidEn & PsiVid]
 *
 * @param[in]  FamilySpecificServices  The current Family Specific Services.
 * @param[in]  CpuEarlyParams          Service parameters
 * @param[in]  StdHeader               Config handle for library and services.
 *
 */
VOID
F15OrPmPwrPlaneInit (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       AMD_CPU_EARLY_PARAMS  *CpuEarlyParams,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  PCI_ADDR  PciAddress;
  UINT32    Core;
  UINT32    LocalPciRegister;
  UINT32    AndMask;
  UINT32    OrMask;
  PLATFORM_FEATS  Features;

  OptionMultiSocketConfiguration.GetCurrPciAddr (&PciAddress, StdHeader);
  GetCurrentCore (&Core, StdHeader);
  ASSERT (Core == 0);

  // Configure D18F3xD8[VSRampSlamTime] based on platform requirements.
  // Before characterization has taken place, no calculations are necessary.
  PciAddress.Address.Function = FUNC_3;
  PciAddress.Address.Register = CPTC1_REG;
  LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
  OrMask = 0x00000000;
  AndMask = 0xFFFFFFFF;
  ((CLK_PWR_TIMING_CTRL1_REGISTER *) &OrMask)->VSRampSlamTime = 1;
  OptionMultiSocketConfiguration.ModifyCurrSocketPci (&PciAddress, AndMask, OrMask, StdHeader);

  // Configure PowerStepUp/PowerStepDown
  PciAddress.Address.Register = CPTC0_REG;
  AndMask = 0xFFFFFFFF;
  ((CLK_PWR_TIMING_CTRL_REGISTER *) &AndMask)->PowerStepUp = 0;
  ((CLK_PWR_TIMING_CTRL_REGISTER *) &AndMask)->PowerStepDown = 0;
  OrMask = 0x00000000;
  Features.PlatformValue = 0;
  GetPlatformFeatures (&Features, &CpuEarlyParams->PlatformConfig, StdHeader);
  if (Features.PlatformFeatures.PlatformSingleLink == 1) {
    ((CLK_PWR_TIMING_CTRL_REGISTER *) &OrMask)->PowerStepUp = 8;
    ((CLK_PWR_TIMING_CTRL_REGISTER *) &OrMask)->PowerStepDown = 8;
  } else {
    ((CLK_PWR_TIMING_CTRL_REGISTER *) &OrMask)->PowerStepUp = 3;
    ((CLK_PWR_TIMING_CTRL_REGISTER *) &OrMask)->PowerStepDown = 3;
  }
  OptionMultiSocketConfiguration.ModifyCurrSocketPci (&PciAddress, AndMask, OrMask, StdHeader);

  if (IsWarmReset (StdHeader)) {
    // Configure PsiVid
    F15OrPmVrmLowPowerModeEnable (FamilySpecificServices, CpuEarlyParams, PciAddress, StdHeader);
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Sets up PSI_L operation.
 *
 * This function implements the LowPowerThreshold parameter.
 *
 * @param[in]  FamilySpecificServices  The current Family Specific Services.
 * @param[in]  CpuEarlyParams          Contains VrmLowPowerThreshold parameter.
 * @param[in]  PciAddress              Segment, bus, device number of the node to transition.
 * @param[in]  StdHeader               Config handle for library and services.
 *
 */
VOID
STATIC
F15OrPmVrmLowPowerModeEnable (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       AMD_CPU_EARLY_PARAMS  *CpuEarlyParams,
  IN       PCI_ADDR              PciAddress,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINT32    Pstate;
  UINT32    PstateCurrent;
  UINT32    NextPstateCurrent;
  UINT32    AndMask;
  UINT32    OrMask;
  UINT32    PreviousVID;
  UINT32    PstateVID;
  UINT32    HwPsMaxVal;
  UINT64    PstateMsr;
  BOOLEAN   EnablePsi;

  if (CpuEarlyParams->PlatformConfig.VrmProperties[CoreVrm].LowPowerThreshold != 0) {
    EnablePsi = FALSE;
    PreviousVID = 0x7F;           // Initialize to invalid zero volt VID code
    PstateVID = 0x7F;
    PciAddress.Address.Function = FUNC_3;
    PciAddress.Address.Register = CPTC2_REG;
    LibAmdPciRead (AccessWidth32, PciAddress, &HwPsMaxVal, StdHeader);

    for (Pstate = 0; Pstate <= (UINT32) ((CLK_PWR_TIMING_CTRL2_REGISTER *) &HwPsMaxVal)->PstateMaxVal; Pstate++) {
      if (FamilySpecificServices->GetProcIddMax (FamilySpecificServices, (UINT8) Pstate, &PstateCurrent, StdHeader)) {
        LibAmdMsrRead ((UINT32) (Pstate + PS_REG_BASE), &PstateMsr, StdHeader);
        PstateVID = (UINT32) (((PSTATE_MSR *) &PstateMsr)->CpuVid);
        if ((Pstate + 1) > (UINT32) ((CLK_PWR_TIMING_CTRL2_REGISTER *) &HwPsMaxVal)->PstateMaxVal) {
          NextPstateCurrent = 0;
        } else if (FamilySpecificServices->GetProcIddMax (FamilySpecificServices, (UINT8) (Pstate + 1), &NextPstateCurrent, StdHeader)) {
          NextPstateCurrent = CpuEarlyParams->PlatformConfig.VrmProperties[CoreVrm].InrushCurrentLimit + NextPstateCurrent;
        }
        if ((PstateCurrent <= CpuEarlyParams->PlatformConfig.VrmProperties[CoreVrm].LowPowerThreshold) && (NextPstateCurrent <= CpuEarlyParams->PlatformConfig.VrmProperties[CoreVrm].LowPowerThreshold) && (PstateVID != PreviousVID)) {
          EnablePsi = TRUE;
          break;
        }
        PreviousVID = PstateVID;
      }
    }

    PciAddress.Address.Function = FUNC_3;
    PciAddress.Address.Register = PW_CTL_MISC_REG;
    OrMask = 0x00000000;
    AndMask = 0xFFFFFFFF;
    ((POWER_CTRL_MISC_REGISTER *) &AndMask)->PsiVid = 0;
    if (EnablePsi) {
      ((POWER_CTRL_MISC_REGISTER *) &OrMask)->PsiVid = PstateVID;
      ((POWER_CTRL_MISC_REGISTER *) &OrMask)->PsiVidEn = 1;
    } else {
      ((POWER_CTRL_MISC_REGISTER *) &AndMask)->PsiVidEn = 0;
    }
    OptionMultiSocketConfiguration.ModifyCurrSocketPci (&PciAddress, AndMask, OrMask, StdHeader);
  }
}
