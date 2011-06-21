/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_10 CPB Initialization
 *
 * Enables core performance boost.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/F10
 * @e \$Revision: 35636 $   @e \$Date: 2010-07-28 09:24:55 +0800 (Wed, 28 Jul 2010) $
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
#include "GeneralServices.h"
#include "cpuFamilyTranslation.h"
#include "cpuF10PowerMgmt.h"
#include "cpuFeatures.h"
#include "cpuRegisters.h"
#include "cpuCpb.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_CPU_FAMILY_0X10_CPUF10CPB_FILECODE

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
F10IsCpbSupported (
  IN       CPB_FAMILY_SERVICES    *CpbServices,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       UINT32                 Socket,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32       CpbControl;
  PCI_ADDR     PciAddress;
  AGESA_STATUS IgnoredSts;
  CPUID_DATA   CpuidData;

  CpbControl = 0;
  LibAmdCpuidRead (AMD_CPUID_APM, &CpuidData, StdHeader);
  if (((CpuidData.EDX_Reg & 0x00000200) >> 9) == 1) {
    GetPciAddress (StdHeader, Socket, 0, &PciAddress, &IgnoredSts);
    PciAddress.Address.Function = FUNC_4;
    PciAddress.Address.Register = CPB_CTRL_REG;
    LibAmdPciRead (AccessWidth32, PciAddress, &CpbControl, StdHeader);
  }
  return (BOOLEAN) (((CPB_CTRL_REGISTER *) (&CpbControl))->NumBoostStates != 0);
}


/*---------------------------------------------------------------------------------------*/
/**
 * BSC entry point for for enabling Core Performance Boost.
 *
 * Set up F4x15C[BoostSrc] and start the PDMs according to the BKDG.
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
F10InitializeCpb (
  IN       CPB_FAMILY_SERVICES    *CpbServices,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       UINT64                 EntryPoint,
  IN       UINT32                 Socket,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32       CpbControl;
  UINT32       Module;
  PCI_ADDR     PciAddress;
  AGESA_STATUS IgnoredSts;

  if ((EntryPoint & CPU_FEAT_BEFORE_PM_INIT) != 0) {
    for (Module = 0; Module < (UINT8)GetPlatformNumberOfModules (); Module++) {
      GetPciAddress (StdHeader, Socket, Module, &PciAddress, &IgnoredSts);
      PciAddress.Address.Function = FUNC_4;
      PciAddress.Address.Register = CPB_CTRL_REG;
      LibAmdPciRead (AccessWidth32, PciAddress, &CpbControl, StdHeader);
      ((CPB_CTRL_REGISTER *) (&CpbControl))->BoostSrc = 3;
      IDS_OPTION_HOOK (IDS_CPB_CTRL, &CpbControl, StdHeader);
      LibAmdPciWrite (AccessWidth32, PciAddress, &CpbControl, StdHeader);

      PciAddress.Address.Function = FUNC_3;
      PciAddress.Address.Register = POPUP_PSTATE_REG;
      LibAmdPciRead (AccessWidth32, PciAddress, &CpbControl, StdHeader);
      ((POPUP_PSTATE_REGISTER *) (&CpbControl))->CacheFlushPopDownEn = 1;
      LibAmdPciWrite (AccessWidth32, PciAddress, &CpbControl, StdHeader);
    }
  }
  return AGESA_SUCCESS;
}

CONST CPB_FAMILY_SERVICES ROMDATA F10CpbSupport =
{
  0,
  F10IsCpbSupported,
  F10InitializeCpb
};
