/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 Orochi C6 C-state feature support functions.
 *
 * Provides the functions necessary to initialize the C6 feature.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/F15/OR
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
#include "cpuFeatures.h"
#include "cpuC6State.h"
#include "cpuApicUtilities.h"
#include "cpuF15PowerMgmt.h"
#include "cpuF15OrPowerMgmt.h"
#include "cpuServices.h"
#include "cpuFamilyTranslation.h"
#include "OptionFamily15hEarlySample.h"
#include "OptionMultiSocket.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X15_OR_F15ORC6STATE_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern F15_OR_ES_C6_SUPPORT F15OrEarlySampleC6Support;
extern F15_OR_ES_MCU_PATCH F15OrEarlySampleLoadMcuPatch;
extern OPTION_MULTISOCKET_CONFIGURATION OptionMultiSocketConfiguration;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
F15OrReloadMicrocodePatchAfterMemInit (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

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
F15OrIsC6Supported (
  IN       C6_FAMILY_SERVICES      *C6Services,
  IN       UINT32                  Socket,
  IN       PLATFORM_CONFIGURATION  *PlatformConfig,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  )
{
  BOOLEAN  IsEnabled;

  IsEnabled = TRUE;
  IsEnabled = IsFeatureEnabled (IoCstate, PlatformConfig, StdHeader);

  F15OrEarlySampleC6Support.F15OrIsC6SupportedHook (&IsEnabled, StdHeader);

  return IsEnabled;
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Enable C6 on a family 15h CPU.
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
F15OrInitializeC6 (
  IN       C6_FAMILY_SERVICES *C6Services,
  IN       UINT64 EntryPoint,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32   LocalPciRegister;
  UINT32   PciMask;
  PCI_ADDR PciAddress;

  if ((EntryPoint & CPU_FEAT_AFTER_PM_INIT) != 0) {
    // Initialize F4x118
    // bits[24] PwrGateEnCstAct1 = 1
    PciAddress.Address.Function = FUNC_4;
    PciAddress.Address.Register = CSTATE_CTRL1_REG;
    LocalPciRegister = 0x01000000;
    PciMask = 0xFFFFFFFF;
    OptionMultiSocketConfiguration.ModifyCurrSocketPci (&PciAddress, PciMask, LocalPciRegister, StdHeader);
  }

  return AGESA_SUCCESS;
}

/**
 *  Reload microcode patch after memory is initialized.
 *
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 */
VOID
F15OrReloadMicrocodePatchAfterMemInit (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  F15OrEarlySampleLoadMcuPatch.F15OrUpdateMcuPatchHook (StdHeader);
}

CONST C6_FAMILY_SERVICES ROMDATA F15OrC6Support =
{
    0,
    F15OrIsC6Supported,
    F15OrInitializeC6,
    F15OrReloadMicrocodePatchAfterMemInit
};
