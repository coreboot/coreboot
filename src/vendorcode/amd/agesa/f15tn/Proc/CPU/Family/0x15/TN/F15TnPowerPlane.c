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
    for (VSRampSlamTime = ((sizeof (F15TnVSRampSlamWaitTimes) / sizeof (F15TnVSRampSlamWaitTimes[0])) - 1); VSRampSlamTime > 0; VSRampSlamTime--) {
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



