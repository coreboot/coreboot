/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU Cache Flush On Halt Function for Family 15h Trinity.
 *
 * Contains code to initialize Cache Flush On Halt feature for Family 15h Trinity.
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
#include "cpuF15TnPowerMgmt.h"
#include "cpuFeatures.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X15_TN_CPUF15TNCACHEFLUSHONHALT_FILECODE

/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */


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
SetF15TnCacheFlushOnHaltRegister (
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
SetF15TnCacheFlushOnHaltRegister (
  IN       CPU_CFOH_FAMILY_SERVICES     *FamilySpecificServices,
  IN       UINT64                       EntryPoint,
  IN       PLATFORM_CONFIGURATION       *PlatformConfig,
  IN       AMD_CONFIG_PARAMS            *StdHeader
  )
{
  PCI_ADDR                              PciAddress;
  CLK_PWR_TIMING_CTRL2_REGISTER         ClkPwrTimingCtrl2;
  CSTATE_POLICY_CTRL1_REGISTER          CstatePolicyCtrl1;
  CSTATE_CTRL1_REGISTER                 CstateCtrl1;

  if ((EntryPoint & (CPU_FEAT_AFTER_POST_MTRR_SYNC | CPU_FEAT_AFTER_RESUME_MTRR_SYNC)) != 0) {
    // Set D18F3xDC[CacheFlushOnHaltCtl] != 0
    // Set D18F3xDC[CacheFlushOnHaltTmr]
    PciAddress.AddressValue = CPTC2_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &ClkPwrTimingCtrl2, StdHeader);
    ClkPwrTimingCtrl2.CacheFlushOnHaltCtl = 7;
    ClkPwrTimingCtrl2.CacheFlushOnHaltTmr = 0x14;
    LibAmdPciWrite (AccessWidth32, PciAddress, &ClkPwrTimingCtrl2, StdHeader);

    // Set D18F4x128[CacheFlushTmr, CacheFlushSucMonThreshold]
    PciAddress.AddressValue = CSTATE_POLICY_CTRL1_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &CstatePolicyCtrl1, StdHeader);
    CstatePolicyCtrl1.CacheFlushTmr = 0x14;
    CstatePolicyCtrl1.CacheFlushSucMonThreshold = 7;
    LibAmdPciWrite (AccessWidth32, PciAddress, &CstatePolicyCtrl1, StdHeader);

    // Set cache flush bits in D18F4x118
    PciAddress.AddressValue = CSTATE_CTRL1_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &CstateCtrl1, StdHeader);
    // Set C-state Action Field 0
    CstateCtrl1.CacheFlushEnCstAct0 = 1;
    CstateCtrl1.CacheFlushTmrSelCstAct0 = 2;
    CstateCtrl1.ClkDivisorCstAct0 = 0;
    // Set C-state Action Field 1
    CstateCtrl1.CacheFlushEnCstAct1 = 1;
    CstateCtrl1.CacheFlushTmrSelCstAct1 = 1;
    CstateCtrl1.ClkDivisorCstAct1 = 0;
    LibAmdPciWrite (AccessWidth32, PciAddress, &CstateCtrl1, StdHeader);

    //Override the default setting
    IDS_OPTION_HOOK (IDS_CACHE_FLUSH_HLT, NULL, StdHeader);
  }
}

CONST CPU_CFOH_FAMILY_SERVICES ROMDATA F15TnCacheFlushOnHalt =
{
  0,
  SetF15TnCacheFlushOnHaltRegister
};
