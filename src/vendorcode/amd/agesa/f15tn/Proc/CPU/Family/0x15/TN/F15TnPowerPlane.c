/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 Models 0x10 - 0x1F Power Plane Initialization
 *
 * Performs the "BIOS Requirements for Power Plane Initialization" as described
 * in the BKDG.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x15/TN
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
#include "cpuF15PowerMgmt.h"
#include "cpuF15TnPowerMgmt.h"
#include "cpuApicUtilities.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "cpuFamilyTranslation.h"
#include "Table.h"
#include "F15TnPowerPlane.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_CPU_FAMILY_0X15_TN_F15TNPOWERPLANE_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

// Register encodings for D18F3xD8[VSRampSlamTime]
STATIC CONST UINT32 ROMDATA F15TnVSRampSlamWaitTimes[8] =
{
  500,    // 000b: 5.00us
  375,    // 001b: 3.75us
  300,    // 010b: 3.00us
  240,    // 011b: 2.40us
  200,    // 100b: 2.00us
  150,    // 101b: 1.50us
  120,    // 110b: 1.20us
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

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

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
F15TnPmPwrPlaneInit (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       AMD_CPU_EARLY_PARAMS  *CpuEarlyParams,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  PCI_ADDR              PciAddress;
  UINT32                SystemSlewRate;
  UINT32                WaitTime;
  UINT32                VSRampSlamTime;
  UINT32                LocalPciRegister;
  CLK_PWR_TIMING_CTRL1_REGISTER   ClkPwrTimingCtrl1;
  BOOLEAN               SkipPowerPlan;


  SkipPowerPlan = FALSE;
  IDS_OPTION_CALLOUT (IDS_CALLOUT_POWER_PLAN_INIT, &SkipPowerPlan, StdHeader);
  if (!SkipPowerPlan) {
    // Step 1 - Configure D18F3xD8[VSRampSlamTime] based on platform requirements.
    // Voltage Ramp Time = maximum time to change voltage by 15mV rounded to the next higher encoding.
    SystemSlewRate = (CpuEarlyParams->PlatformConfig.VrmProperties[CoreVrm].SlewRate <=
                      CpuEarlyParams->PlatformConfig.VrmProperties[NbVrm].SlewRate) ?
                      CpuEarlyParams->PlatformConfig.VrmProperties[CoreVrm].SlewRate :
                      CpuEarlyParams->PlatformConfig.VrmProperties[NbVrm].SlewRate;

    ASSERT (SystemSlewRate != 0);

    // First, calculate the time it takes to change 15mV using the VRM slew rate.
    WaitTime = (15000 * 100) / SystemSlewRate;
    if (((15000 * 100) % SystemSlewRate) != 0) {
      WaitTime++;
    }

    // Next, round it to the appropriate encoded value.  We will start from encoding 111b which corresponds
    // to the fastest slew rate, and work our way down to 000b, which represents the slowest an acceptable
    // VRM can be.
    for (VSRampSlamTime = (ARRAY_SIZE(F15TnVSRampSlamWaitTimes)- 1); VSRampSlamTime > 0; VSRampSlamTime--) {
      if (WaitTime <= F15TnVSRampSlamWaitTimes[VSRampSlamTime]) {
        break;
      }
    }

    if (WaitTime > F15TnVSRampSlamWaitTimes[0]) {
      // The VRMs on this motherboard are too slow for this CPU.
      IDS_ERROR_TRAP;
    }

    // Lastly, program D18F3xD8[VSRampSlamTime] with the appropriate encoded value.
    PciAddress.AddressValue = CPTC1_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &ClkPwrTimingCtrl1, StdHeader);
    ClkPwrTimingCtrl1.VSRampSlamTime = VSRampSlamTime;
    LibAmdPciWrite (AccessWidth32, PciAddress, &ClkPwrTimingCtrl1, StdHeader);

    // Configure PowerStepUp/PowerStepDown
    PciAddress.AddressValue = CPTC0_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    ((CLK_PWR_TIMING_CTRL_REGISTER *) &LocalPciRegister)->PowerStepUp = 8;
    ((CLK_PWR_TIMING_CTRL_REGISTER *) &LocalPciRegister)->PowerStepDown = 8;
    LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
  }
}



