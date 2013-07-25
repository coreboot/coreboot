/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_16 Kabini thermal initialization
 *
 * Performs processor thermal initialization.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x16/KB
 * @e \$Revision: 85817 $   @e \$Date: 2013-01-11 16:58:12 -0600 (Fri, 11 Jan 2013) $
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
#include "Ids.h"
#include "cpuRegisters.h"
#include "GeneralServices.h"
#include "CommonReturns.h"
#include "cpuFeatures.h"
#include "cpuHtc.h"
#include "cpuF16PowerMgmt.h"
#include "F16KbPowerMgmt.h"
#include "Gnb.h"
#include "GnbPcie.h"
#include "GnbRegistersKB.h"
#include "GnbRegisterAccKB.h"
#include "GnbHandleLib.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_CPU_FAMILY_0X16_KB_F16KBHTC_FILECODE


/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
AGESA_STATUS
STATIC
F16KbInitializeHtc (
  IN       HTC_FAMILY_SERVICES    *HtcServices,
  IN       UINT64                  EntryPoint,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

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

/*----------------------------------------------------------------------------------------*/
/**
 *    Main entry point for initializing the Thermal Control
 *    safety net feature.
 *
 *    This must be run by all Family 16h Kabini core 0s in the system.
 *
 * @param[in]  HtcServices             The current CPU's family services.
 * @param[in]  EntryPoint              Timepoint designator.
 * @param[in]  PlatformConfig          Platform profile/build option config structure.
 * @param[in]  StdHeader               Config handle for library and services.
 *
 * @retval     AGESA_SUCCESS           Always succeeds.
 *
 */
AGESA_STATUS
STATIC
F16KbInitializeHtc (
  IN       HTC_FAMILY_SERVICES    *HtcServices,
  IN       UINT64                  EntryPoint,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32                         HtcTempLimit;
  NB_CAPS_REGISTER               NbCaps;
  HTC_REGISTER                   HtcReg;
  CLK_PWR_TIMING_CTRL2_REGISTER  Cptc2;
  POPUP_PSTATE_REGISTER          PopUpPstate;
  PCI_ADDR                       PciAddress;
  UINT32                         D0F0xBC_xC0107097;

  if ((EntryPoint & (CPU_FEAT_AFTER_POST_MTRR_SYNC | CPU_FEAT_AFTER_RESUME_MTRR_SYNC)) != 0) {
    PciAddress.AddressValue = NB_CAPS_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &NbCaps, StdHeader);
    if (NbCaps.HtcCapable == 1) {
      // Enable HTC
      PciAddress.Address.Register = HTC_REG;
      LibAmdPciRead (AccessWidth32, PciAddress, &HtcReg, StdHeader);
      GnbRegisterReadKB (GnbGetHandle (StdHeader), 0x4, 0xC0107097, &D0F0xBC_xC0107097, 0, StdHeader);
      HtcReg.HtcTmpLmt = (D0F0xBC_xC0107097 >> 3) & 0x7F;
      if (HtcReg.HtcTmpLmt != 0) {
        // Enable HTC
        HtcReg.HtcEn = 1;
        PciAddress.Address.Register = CPTC2_REG;
        LibAmdPciRead (AccessWidth32, PciAddress, &Cptc2, StdHeader);
        if (HtcReg.HtcPstateLimit > Cptc2.HwPstateMaxVal) {
          // F3xDC[HwPstateMaxVal] = F3x64[HtcPstateLimit]
          Cptc2.HwPstateMaxVal = HtcReg.HtcPstateLimit;
          LibAmdPciWrite (AccessWidth32, PciAddress, &Cptc2, StdHeader);
          // F3xA8[PopDownPstate] = F3xDC[HwPstateMaxVal]
          PciAddress.Address.Register = POPUP_PSTATE_REG;
          LibAmdPciRead (AccessWidth32, PciAddress, &PopUpPstate, StdHeader);
          PopUpPstate.PopDownPstate = Cptc2.HwPstateMaxVal;
          LibAmdPciWrite (AccessWidth32, PciAddress, &PopUpPstate, StdHeader);
        }
        if ((PlatformConfig->HtcTemperatureLimit >= 520) && (PlatformConfig->LhtcTemperatureLimit != 0)) {
          HtcTempLimit = ((PlatformConfig->HtcTemperatureLimit - 520) / 5);
          if (HtcTempLimit < HtcReg.HtcTmpLmt) {
            HtcReg.HtcTmpLmt = HtcTempLimit;
          }
        }
      } else {
        // Disable HTC
        HtcReg.HtcEn = 0;
      }
      PciAddress.Address.Register = HTC_REG;
      IDS_OPTION_HOOK (IDS_HTC_CTRL, &HtcReg, StdHeader);
      LibAmdPciWrite (AccessWidth32, PciAddress, &HtcReg, StdHeader);
    }
  }
  return AGESA_SUCCESS;
}

CONST HTC_FAMILY_SERVICES ROMDATA F16KbHtcSupport =
{
  0,
  (PF_HTC_IS_SUPPORTED) CommonReturnTrue,
  F16KbInitializeHtc
};
