/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 CPB Initialization
 *
 * Enables core performance boost.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/F15/TN
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
#include "GeneralServices.h"
#include "cpuFamilyTranslation.h"
#include "cpuF15PowerMgmt.h"
#include "cpuF15TnPowerMgmt.h"
#include "cpuFeatures.h"
#include "cpuCpb.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X15_TN_F15TNCPB_FILECODE

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

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * BSC entry point for checking whether or not CPB is supported.
 *
 * @param[in]  CpbServices             The current CPU's family services.
 * @param[in]  PlatformConfig          Contains the runtime modifiable feature input data.
 * @param[in]  Socket                  Zero based socket number to check.
 * @param[in]  StdHeader               Config handle for library and services.
 *
 * @retval     TRUE                    CPB is supported.
 * @retval     FALSE                   CPB is not supported.
 *
 */
BOOLEAN
STATIC
F15TnIsCpbSupported (
  IN       CPB_FAMILY_SERVICES    *CpbServices,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       UINT32                 Socket,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  CPB_CTRL_REGISTER     CpbControl;
  PCI_ADDR              PciAddress;

  PciAddress.AddressValue = CPB_CTRL_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, PciAddress, &CpbControl, StdHeader);
  return (BOOLEAN) (CpbControl.NumBoostStates != 0);
}


/*---------------------------------------------------------------------------------------*/
/**
 * BSC entry point for for enabling Core Performance Boost.
 *
 * Set up D18F4x15C[BoostSrc] and start the PDMs according to the BKDG.
 *
 * @param[in]  CpbServices             The current CPU's family services.
 * @param[in]  PlatformConfig          Contains the runtime modifiable feature input data.
 * @param[in]  EntryPoint              Current CPU feature dispatch point.
 * @param[in]  Socket                  Zero based socket number to check.
 * @param[in]  StdHeader               Config handle for library and services.
 *
 * @retval     AGESA_SUCCESS           Always succeeds.
 *
 */
AGESA_STATUS
STATIC
F15TnInitializeCpb (
  IN       CPB_FAMILY_SERVICES    *CpbServices,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       UINT64                 EntryPoint,
  IN       UINT32                 Socket,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  CPB_CTRL_REGISTER     CpbControl;
  PCI_ADDR              PciAddress;
  F15_PSTATE_MSR        PstateMsrData;
  UINT32                Pbx;

  if ((EntryPoint & CPU_FEAT_AFTER_PM_INIT) != 0) {
    PciAddress.AddressValue = CPB_CTRL_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &CpbControl, StdHeader);
    if (CpbControl.NumBoostStates == 0) {
      CpbControl.ApmMasterEn = 0;
    } else {
      CpbControl.ApmMasterEn = 1;
    }
    LibAmdPciWrite (AccessWidth32, PciAddress, &CpbControl, StdHeader);

  } else if ((EntryPoint & (CPU_FEAT_AFTER_POST_MTRR_SYNC | CPU_FEAT_AFTER_RESUME_MTRR_SYNC)) != 0) {
    PciAddress.AddressValue = CPB_CTRL_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &CpbControl, StdHeader);
    if ((CpbControl.BoostSrc == 0) && (CpbControl.NumBoostStates != 0)) {
      // If any boosted P-state is still enabled, set BoostSrc = 1.
      for (Pbx = 0; Pbx < CpbControl.NumBoostStates; Pbx++) {
        LibAmdMsrRead (PS_REG_BASE + Pbx, (UINT64 *)&PstateMsrData, StdHeader);
        if (PstateMsrData.PsEnable == 1) {
          CpbControl.BoostSrc = 1;
          LibAmdPciWrite (AccessWidth32, PciAddress, &CpbControl, StdHeader);
          break;
        }
      }
    }
  }
  return AGESA_SUCCESS;
}

CONST CPB_FAMILY_SERVICES ROMDATA F15TnCpbSupport =
{
  0,
  F15TnIsCpbSupported,
  F15TnInitializeCpb
};
