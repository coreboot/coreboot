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
 * @e sub-project:  CPU/F15/OR
 * @e \$Revision: 54493 $   @e \$Date: 2011-06-08 15:21:06 -0600 (Wed, 08 Jun 2011) $
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
#include "GeneralServices.h"
#include "cpuFamilyTranslation.h"
#include "cpuF15PowerMgmt.h"
#include "cpuF15OrPowerMgmt.h"
#include "cpuFeatures.h"
#include "cpuCpb.h"
#include "F15PackageType.h"
#include "OptionFamily15hEarlySample.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X15_OR_F15ORCPB_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern F15_OR_ES_CPB_SUPPORT F15OrEarlySampleCpbSupport;
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
F15OrIsCpbSupported (
  IN       CPB_FAMILY_SERVICES    *CpbServices,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       UINT32                 Socket,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32       CpbControl;
  PCI_ADDR     PciAddress;
  AGESA_STATUS IgnoredSts;
  BOOLEAN  IsEnabled;

  IsEnabled = TRUE;

  GetPciAddress (StdHeader, Socket, 0, &PciAddress, &IgnoredSts);
  PciAddress.Address.Function = FUNC_4;
  PciAddress.Address.Register = CPB_CTRL_REG;
  LibAmdPciRead (AccessWidth32, PciAddress, &CpbControl, StdHeader);
  IsEnabled = (BOOLEAN) (((CPB_CTRL_REGISTER *) (&CpbControl))->NumBoostStates != 0);

  F15OrEarlySampleCpbSupport.F15OrIsCpbSupportedHook (&IsEnabled, StdHeader);

  return IsEnabled;
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
F15OrInitializeCpb (
  IN       CPB_FAMILY_SERVICES    *CpbServices,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       UINT64                 EntryPoint,
  IN       UINT32                 Socket,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32       CpbControl;
  UINT32       Module;
  UINT32       PackageType;
  PCI_ADDR     PciAddress;
  AGESA_STATUS IgnoredSts;

  if ((EntryPoint & (CPU_FEAT_BEFORE_PM_INIT | CPU_FEAT_INIT_LATE_END | CPU_FEAT_S3_LATE_RESTORE_END)) != 0) {
    for (Module = 0; Module < (UINT8)GetPlatformNumberOfModules (); Module++) {
      PackageType = LibAmdGetPackageType (StdHeader);
      GetPciAddress (StdHeader, Socket, Module, &PciAddress, &IgnoredSts);
      PciAddress.Address.Function = FUNC_4;
      PciAddress.Address.Register = CPB_CTRL_REG;
      LibAmdPciRead (AccessWidth32, PciAddress, &CpbControl, StdHeader);
      if (PackageType == PACKAGE_TYPE_AM3r2) {
        ((CPB_CTRL_REGISTER *) (&CpbControl))->BoostSrc = 1;
      } else {
        if ((EntryPoint & CPU_FEAT_BEFORE_PM_INIT) != 0) {
          ((CPB_CTRL_REGISTER *) (&CpbControl))->BoostSrc = 1;
        }
      }
      LibAmdPciWrite (AccessWidth32, PciAddress, &CpbControl, StdHeader);
    }
  }
  return AGESA_SUCCESS;
}

CONST CPB_FAMILY_SERVICES ROMDATA F15OrCpbSupport =
{
  0,
  F15OrIsCpbSupported,
  F15OrInitializeCpb
};
