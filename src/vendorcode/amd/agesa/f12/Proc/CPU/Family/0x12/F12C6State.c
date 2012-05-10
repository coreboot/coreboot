/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_12 C6 C-state feature support functions.
 *
 * Provides the functions necessary to initialize the C6 feature.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/F12
 * @e \$Revision: 44324 $   @e \$Date: 2010-12-22 17:16:51 +0800 (Wed, 22 Dec 2010) $
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
#include "cpuF12PowerMgmt.h"
#include "OptionFamily12hEarlySample.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_CPU_FAMILY_0X12_F12C6STATE_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern F12_ES_C6_SUPPORT F12EarlySampleC6Support;

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
F12IsC6Supported (
  IN       C6_FAMILY_SERVICES      *C6Services,
  IN       UINT32                  Socket,
  IN       PLATFORM_CONFIGURATION  *PlatformConfig,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  )
{
  UINT32   LocalPciRegister;
  BOOLEAN  IsEnabled;
  PCI_ADDR PciAddress;

  IsEnabled = TRUE;
  PciAddress.AddressValue = CPU_STATE_PM_CTRL1_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
  if ((((CPU_STATE_PM_CTRL1_REGISTER *) &LocalPciRegister)->CoreC6Cap == 0) &&
    (((CPU_STATE_PM_CTRL1_REGISTER *) &LocalPciRegister)->PkgC6Cap == 0)) {
    IsEnabled = FALSE;
  }
  return IsEnabled;
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Enable C6 on a family 12h CPU.
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
F12InitializeC6 (
  IN       C6_FAMILY_SERVICES *C6Services,
  IN       UINT64 EntryPoint,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32   i;
  UINT32   MaxEnabledPstate;
  UINT32   LocalPciRegister;
  UINT64   LocalMsrRegister;
  PCI_ADDR PciAddress;

  for (i = MSR_PSTATE_7; i > MSR_PSTATE_0; i--) {
    LibAmdMsrRead (i, &LocalMsrRegister, StdHeader);
    if (((PSTATE_MSR *) &LocalMsrRegister)->PsEnable == 1) {
      break;
    }
  }
  MaxEnabledPstate = i - MSR_PSTATE_0;

  if ((EntryPoint & CPU_FEAT_AFTER_PM_INIT) != 0) {
    F12EarlySampleC6Support.F12InitializeC6 (StdHeader);
  } else {
    // Ensure D18F2x118[C6DramLock] and D18F4x12C[C6Base] are programmed.
    PciAddress.AddressValue = MEM_CFG_LOW_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    ASSERT (((MEM_CFG_LOW_REGISTER *) &LocalPciRegister)->C6DramLock == 1);

    PciAddress.AddressValue = C6_BASE_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    ASSERT (((C6_BASE_REGISTER *) &LocalPciRegister)->C6Base != 0);

    // If PC6 is supported, program D18F4x1AC[PstateIdCoreOffExit] to
    // the index of lowest-performance Pstate with MSRC001_00[6B:64]
    // [PstateEn] == 1 on core 0.
    PciAddress.AddressValue = CPU_STATE_PM_CTRL1_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    if (((CPU_STATE_PM_CTRL1_REGISTER *) &LocalPciRegister)->PkgC6Cap == 1) {
      ((CPU_STATE_PM_CTRL1_REGISTER *) &LocalPciRegister)->PstateIdCoreOffExit = MaxEnabledPstate;
      LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    }

    // Program D18F4x118 to 0000_0101h.
    PciAddress.AddressValue = CSTATE_CTRL1_PCI_ADDR;
    LocalPciRegister = 0x00000101;
    LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
  }

  return AGESA_SUCCESS;
}

CONST C6_FAMILY_SERVICES ROMDATA F12C6Support =
{
  0,
  F12IsC6Supported,
  F12InitializeC6,
  ReloadMicrocodePatchAfterMemInit
};
