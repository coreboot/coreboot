/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_16 Kabini C6 C-state feature support functions.
 *
 * Provides the functions necessary to initialize the C6 feature.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/F16/KB
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
#include "Ids.h"
#include "cpuRegisters.h"
#include "cpuFeatures.h"
#include "cpuC6State.h"
#include "cpuApicUtilities.h"
#include "cpuF16PowerMgmt.h"
#include "F16KbPowerMgmt.h"
#include "cpuServices.h"
#include "cpuFamilyTranslation.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X16_KB_F16KBC6STATE_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
BOOLEAN
STATIC
F16KbIsC6Supported (
  IN       C6_FAMILY_SERVICES      *C6Services,
  IN       UINT32                  Socket,
  IN       PLATFORM_CONFIGURATION  *PlatformConfig,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  );

AGESA_STATUS
STATIC
F16KbInitializeC6 (
  IN       C6_FAMILY_SERVICES *C6Services,
  IN       UINT64 EntryPoint,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS *StdHeader
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

/*---------------------------------------------------------------------------------------*/
/**
 *  Is C6 supported on this CPU
 *
 * @param[in]    C6Services         Pointer to this CPU's C6 family services.
 * @param[in]    Socket             This core's zero-based socket number.
 * @param[in]    PlatformConfig     Contains the runtime modifiable feature input data.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @retval       TRUE               C6 state is supported.
 * @retval       FALSE              C6 state is not supported.
 *
 */
BOOLEAN
STATIC
F16KbIsC6Supported (
  IN       C6_FAMILY_SERVICES      *C6Services,
  IN       UINT32                  Socket,
  IN       PLATFORM_CONFIGURATION  *PlatformConfig,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  )
{
  return (IsFeatureEnabled (IoCstate, PlatformConfig, StdHeader));
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Enable C6 on a family 16h Kabini CPU.
 *
 * @param[in]    C6Services         Pointer to this CPU's C6 family services.
 * @param[in]    EntryPoint         Timepoint designator.
 * @param[in]    PlatformConfig     Contains the runtime modifiable feature input data.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @return       AGESA_SUCCESS      Always succeeds.
 *
 */
AGESA_STATUS
STATIC
F16KbInitializeC6 (
  IN       C6_FAMILY_SERVICES *C6Services,
  IN       UINT64 EntryPoint,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  PCI_ADDR                PciAddress;
  CSTATE_CTRL1_REGISTER   CstateCtrl1;
  POPUP_PSTATE_REGISTER   PopDownPstate;
  CLK_PWR_TIMING_CTRL2_REGISTER   ClkPwrTimingCtrl2;

  if ((EntryPoint & CPU_FEAT_AFTER_PM_INIT) != 0) {
    // Initialize F4x118
    PciAddress.AddressValue = CSTATE_CTRL1_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &CstateCtrl1, StdHeader);
    // Set C-state Action Field 0
    // bits[8]       PwrGateEnCstAct0    = 0x1
    // bits[9]       PwrOffEnCstAct0     = 0x1
    CstateCtrl1.PwrGateEnCstAct0 = 1;
    CstateCtrl1.PwrOffEnCstAct0 = 1;
    // Set C-state Action Field 1
    // bits[24]      PwrGateEnCstAct1    = 0x1
    // bits[25]      PwrOffEnCstAct1     = 0x1
    CstateCtrl1.PwrGateEnCstAct1 = 1;
    CstateCtrl1.PwrOffEnCstAct1 = 1;
    LibAmdPciWrite (AccessWidth32, PciAddress, &CstateCtrl1, StdHeader);

    // Initialize F3xA8[PopDownPstate] = F3xDC[HwPstateMaxVal]
    PciAddress.AddressValue = CPTC2_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &ClkPwrTimingCtrl2, StdHeader);
    PciAddress.AddressValue = POPUP_PSTATE_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &PopDownPstate, StdHeader);
    PopDownPstate.PopDownPstate = ClkPwrTimingCtrl2.HwPstateMaxVal;
    LibAmdPciWrite (AccessWidth32, PciAddress, &PopDownPstate, StdHeader);
  } else if ((EntryPoint & (CPU_FEAT_AFTER_RESUME_MTRR_SYNC | CPU_FEAT_BEFORE_RELINQUISH_AP)) != 0) {
    // Initialize F4x118
    PciAddress.AddressValue = CSTATE_CTRL1_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &CstateCtrl1, StdHeader);
    // Set C-state Action Field 0
    // bits[1]       CacheFlushEnCstAct0 = 0x1
    CstateCtrl1.CacheFlushEnCstAct0 = 1;
    // Set C-state Action Field 1
    // bits[17]      CacheFlushEnCstAct1 = 0x1
    CstateCtrl1.CacheFlushEnCstAct1 = 1;
    LibAmdPciWrite (AccessWidth32, PciAddress, &CstateCtrl1, StdHeader);
  }
  return AGESA_SUCCESS;
}

CONST C6_FAMILY_SERVICES ROMDATA F16KbC6Support =
{
  0,
  F16KbIsC6Supported,
  F16KbInitializeC6,
  ReloadMicrocodePatchAfterMemInit
};
