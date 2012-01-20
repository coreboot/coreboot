/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU Cache Flush On Halt Function for Family 15h Orochi.
 *
 * Contains code to initialize Cache Flush On Halt feature for Family 15h Orochi.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x15/OR
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
 *----------------------------------------------------------------------------
 */


/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "cpuRegisters.h"
#include "cpuServices.h"
#include "cpuFamilyTranslation.h"
#include "cpuPostInit.h"
#include "cpuF15PowerMgmt.h"
#include "cpuF15OrPowerMgmt.h"
#include "cpuFeatures.h"
#include "F15PackageType.h"
#include "OptionMultiSocket.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X15_OR_CPUF15ORCACHEFLUSHONHALT_FILECODE

/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
extern OPTION_MULTISOCKET_CONFIGURATION OptionMultiSocketConfiguration;

/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
VOID
SetF15OrCacheFlushOnHaltRegister (
  IN       CPU_CFOH_FAMILY_SERVICES     *FamilySpecificServices,
  IN       UINT64                       EntryPoint,
  IN       PLATFORM_CONFIGURATION       *PlatformConfig,
  IN       AMD_CONFIG_PARAMS            *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          P U B L I C     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *    Enable Cpu Cache Flush On Halt Function
 *
 *    @param[in]       FamilySpecificServices   The current Family Specific Services.
 *    @param[in]       EntryPoint               Timepoint designator.
 *    @param[in]       PlatformConfig           Contains the runtime modifiable feature input data.
 *    @param[in]       StdHeader                Config Handle for library, services.
 */
VOID
SetF15OrCacheFlushOnHaltRegister (
  IN       CPU_CFOH_FAMILY_SERVICES     *FamilySpecificServices,
  IN       UINT64                       EntryPoint,
  IN       PLATFORM_CONFIGURATION       *PlatformConfig,
  IN       AMD_CONFIG_PARAMS            *StdHeader
  )
{
  UINT32       AndMask;
  UINT32       OrMask;
  PCI_ADDR     PciAddress;

  if ((EntryPoint & (CPU_FEAT_AFTER_POST_MTRR_SYNC | CPU_FEAT_AFTER_RESUME_MTRR_SYNC)) != 0) {
    // Set D18F3xDC[CacheFlushOnHaltCtl] != 0
    PciAddress.Address.Function = FUNC_3;
    PciAddress.Address.Register = CPTC2_REG;
    OrMask = 0;
    AndMask = 0xFC00FFFF;
    ((CLK_PWR_TIMING_CTRL2_REGISTER *) &OrMask)->CacheFlushOnHaltCtl = 7;
    ((CLK_PWR_TIMING_CTRL2_REGISTER *) &OrMask)->CacheFlushOnHaltTmr = 0x28;
    OptionMultiSocketConfiguration.ModifyCurrSocketPci (&PciAddress, AndMask, OrMask, StdHeader); // F3xDC

    PciAddress.Address.Function = FUNC_4;
    PciAddress.Address.Register = CSTATE_CTRL1_REG;
    OrMask = 0;
    AndMask = 0xFF11FF11;
    // D18F4x118[CpuPrbEnCstAct0] = 1
    // D18F4x118[CpuPrbEnCstAct1] = 1
    // D18F4x118[CacheFlushEnCstAct0] = 1
    ((CSTATE_CTRL1_REGISTER *) &OrMask)->CpuPrbEnCstAct0 = 1;
    ((CSTATE_CTRL1_REGISTER *) &OrMask)->CpuPrbEnCstAct1 = 1;
    ((CSTATE_CTRL1_REGISTER *) &OrMask)->CacheFlushEnCstAct0 = 1;

    // Set C-state Action Field 0
    ((CSTATE_CTRL1_REGISTER *) &OrMask)->CacheFlushTmrSelCstAct0 = 2;
    // Set C-state Action Field 1
    ((CSTATE_CTRL1_REGISTER *) &OrMask)->CacheFlushEnCstAct1 = 1;
    ((CSTATE_CTRL1_REGISTER *) &OrMask)->CacheFlushTmrSelCstAct1 = 1;

    OptionMultiSocketConfiguration.ModifyCurrSocketPci (&PciAddress, AndMask, OrMask, StdHeader); // F4x118

    // D18F4x128[CacheFlushSucMonThreshold] = 0
    PciAddress.Address.Function = FUNC_4;
    PciAddress.Address.Register = CSTATE_POLICY_CTRL1_REG;
    OrMask = 0;
    AndMask = 0xFFFFFFFF;
    ((CSTATE_POLICY_CTRL1_REGISTER *) &AndMask)->CacheFlushSucMonThreshold = 0;
    OptionMultiSocketConfiguration.ModifyCurrSocketPci (&PciAddress, AndMask, OrMask, StdHeader); // F4x128

    // D18F3x84[ClkDivisorSmafAct7] = 0
    // D18F3x84[CpuPrbEnSmafAct7]   = 1
    PciAddress.Address.Function = FUNC_3;
    PciAddress.Address.Register = ACPI_PWR_STATE_CTRL_HI_REG;
    OrMask = 0;
    AndMask = 0xFFFFFFFF;
    ((ACPI_PWR_STATE_CTRL_HI_REGISTER *) &AndMask)->ClkDivisorSmafAct7 = 0;
    ((ACPI_PWR_STATE_CTRL_HI_REGISTER *) &OrMask)->CpuPrbEnSmafAct7 = 1;
    OptionMultiSocketConfiguration.ModifyCurrSocketPci (&PciAddress, AndMask, OrMask, StdHeader); // F3x84

    //Override the default setting
    IDS_OPTION_HOOK (IDS_CACHE_FLUSH_HLT, NULL, StdHeader);
  }
}

CONST CPU_CFOH_FAMILY_SERVICES ROMDATA F15OrCacheFlushOnHalt =
{
  0,
  SetF15OrCacheFlushOnHaltRegister
};
