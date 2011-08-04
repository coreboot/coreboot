/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_14 Optimizations for lower power consumption
 *
 * Sets some registers for tablet parts at AmdInitEarly.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/F14
 * @e \$Revision: 45578 $   @e \$Date: 2011-01-18 19:20:41 -0500 (Tue, 18 Jan 2011) $
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
 * 
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "cpuFamilyTranslation.h"
#include "cpuF14PowerMgmt.h"
#include "cpuF14LowPowerInit.h"
#include "Filecode.h"

#define FILECODE PROC_CPU_FAMILY_0X14_CPUF14LOWPOWERINIT_FILECODE

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
 * Family 14h model 0 - 0xF core 0 entry point for programming registers for lower
 * power consumption.
 *
 * Set up D18F6x94[CpuPstateThrEn, CpuPstateThr], and D18F4x134[IntRateCC6DecrRate
 * according to the BKDG.
 *
 * @param[in]  FamilySpecificServices  The current Family Specific Services.
 * @param[in]  CpuEarlyParams          Service parameters
 * @param[in]  StdHeader               Config handle for library and services.
 *
 */
VOID
F14OptimizeForLowPowerInit (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       AMD_CPU_EARLY_PARAMS  *CpuEarlyParams,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINT8                 NumBoostStates;
  UINT32                LocalPciRegister;
  BOOLEAN               OptimizeForLowPower;
  BOOLEAN               IsRevC;
  PCI_ADDR              PciAddress;
  CPU_LOGICAL_ID        CpuFamilyRevision;

  PciAddress.AddressValue = PRODUCT_INFO_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);

  if ((((PRODUCT_INFO_REGISTER *) &LocalPciRegister)->LowPowerDefault == 1) &&
      (CpuEarlyParams->PlatformConfig.PlatformProfile.PlatformPowerPolicy == BatteryLife)) {
    OptimizeForLowPower = TRUE;
  } else {
    OptimizeForLowPower = FALSE;
  }

  // Get F4x15C [4:2] NumBoostStates
  // Get IsRevC
  NumBoostStates = 0;
  IsRevC = FALSE;
  GetLogicalIdOfCurrentCore (&CpuFamilyRevision, StdHeader);
  if ((CpuFamilyRevision.Revision & AMD_F14_ON_Cx) != 0) {
    IsRevC = TRUE;
    PciAddress.AddressValue = CPB_CTRL_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    NumBoostStates = (UINT8) ((CPB_CTRL_REGISTER *) &LocalPciRegister)->NumBoostStates;
  }

  // F6x94[2:0] CpuPstateThr
  PciAddress.AddressValue = NB_PSTATE_CFG_HIGH_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
  if (OptimizeForLowPower) {
    ((NB_PSTATE_CFG_HIGH_REGISTER *) &LocalPciRegister)->CpuPstateThr = 0;
  } else {
    if (NumBoostStates == 0) {
      ((NB_PSTATE_CFG_HIGH_REGISTER *) &LocalPciRegister)->CpuPstateThr = 1;
    } else {
      ((NB_PSTATE_CFG_HIGH_REGISTER *) &LocalPciRegister)->CpuPstateThr = 2;
    }
  }
  // F6x94[3] CpuPstateThrEn = 1
  ((NB_PSTATE_CFG_HIGH_REGISTER *) &LocalPciRegister)->CpuPstateThrEn = 1;
  LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);

  // F4x134[31:27] IntRateCC6DecrRate
  PciAddress.AddressValue = CSTATE_MON_CTRL3_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
  ((CSTATE_MON_CTRL3_REGISTER *) &LocalPciRegister)->IntRateCC6DecrRate = (OptimizeForLowPower || IsRevC) ? 0x18 : 0x8;
  LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
}
