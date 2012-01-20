/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 Orochi Low Power P-state Initialization
 *
 * Enables Low Power P-state.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x15/OR
 * @e \$Revision: 54780 $   @e \$Date: 2011-06-12 21:25:20 -0600 (Sun, 12 Jun 2011) $
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
#include "cpuServices.h"
#include "cpuRegisters.h"
#include "cpuFamilyTranslation.h"
#include "cpuF15PowerMgmt.h"
#include "cpuF15OrPowerMgmt.h"
#include "CommonReturns.h"
#include "cpuLowPwrPstate.h"
#include "OptionMultiSocket.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X15_OR_F15ORLOWPWRPSTATE_FILECODE

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
extern OPTION_MULTISOCKET_CONFIGURATION OptionMultiSocketConfiguration;

/*---------------------------------------------------------------------------------------*/
/**
 * This routine will be run by every cores for enabling low power Pstate.
 *
 * This function must be run after P-states initialization and before creating ACPI objects
 *
 * @param[in]  LowPwrPstateServices    The current CPU's family services.
 * @param[in]  PlatformConfig          Contains the runtime modifiable feature input data.
 * @param[in]  StdHeader               Config handle for library and services.
 *
 * @retval     AGESA_SUCCESS           Always succeeds.
 *
 */
AGESA_STATUS
STATIC
F15OrInitializeLowPwrPstate (
  IN       LOW_PWR_PSTATE_FAMILY_SERVICES  *LowPwrPstateServices,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT8  OriginalPstate;
  UINT8  PstateMaxVal;
  UINT8  CurPstateLimit;
  UINT8  PstateToWaitFor;
  UINT32 Socket;
  UINT32 Module;
  UINT32 Core;
  UINT32 PciData;
  UINT64 LocalMsrRegister;
  CPU_SPECIFIC_SERVICES   *FamilySpecificServices;
  PCI_ADDR PciAddress;
  PCI_ADDR IntNode0PciAddress;
  AGESA_STATUS IgnoredSts;

  if (IsCorePairPrimary (FirstCoreIsComputeUnitPrimary, StdHeader)) {
    FamilySpecificServices = NULL;
    OriginalPstate = 0;
    GetCpuServicesOfCurrentCore ((CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
    ASSERT (FamilySpecificServices != NULL);

    IdentifyCore (StdHeader, &Socket, &Module, &Core, &IgnoredSts);
    GetPciAddress (StdHeader, Socket, Module, &PciAddress, &IgnoredSts);

    // Step 1 --- Read MSR_C001_0063[CurPstate] and store the value in OriginalPstate
    LibAmdMsrRead (MSR_PSTATE_STS, &LocalMsrRegister, StdHeader);
    OriginalPstate = (UINT8) ((PSTATE_STS_MSR *) &LocalMsrRegister)->CurPstate;

    // Step 2 --- Write 0 to MSR_C001_0062[PstateCmd]
    LibAmdMsrRead (MSR_PSTATE_CTL, &LocalMsrRegister, StdHeader);
    ((PSTATE_CTRL_MSR *) &LocalMsrRegister)->PstateCmd = (UINT64) 0;
    LibAmdMsrWrite (MSR_PSTATE_CTL, &LocalMsrRegister, StdHeader);

    // Step 3 --- Wait for MSR_C001_0063[CurPstate] == MSR_C001_0061[CurPstateLimit]
    LibAmdMsrRead (MSR_PSTATE_CURRENT_LIMIT, &LocalMsrRegister, StdHeader);
    CurPstateLimit = (UINT8) ((PSTATE_CURLIM_MSR *) &LocalMsrRegister)->CurPstateLimit;
    do {
      LibAmdMsrRead (MSR_PSTATE_STS, &LocalMsrRegister, StdHeader);
    } while (((PSTATE_STS_MSR *) &LocalMsrRegister)->CurPstate != (UINT64) CurPstateLimit);

    // Step 4 --- Copy MSR_C001_00[6B:64] pointed to by F3xDC[PstateMaxVal] to MSR_C001_00[6B:64]
    //            pointed to by F3xDC[PstateMaxVal]+1
    PciAddress.Address.Function = FUNC_3;
    PciAddress.Address.Register = CPTC2_REG;
    LibAmdPciRead (AccessWidth32, PciAddress, &PciData, StdHeader);
    PstateMaxVal = (UINT8) ((CLK_PWR_TIMING_CTRL2_REGISTER *) &PciData)->PstateMaxVal;
    // In case that F3xDC[PstateMaxVal] was increased by step 5 during the first time of running this function.
    // Get the real PstateMaxVal by checking C001_00[6B:64][PsEnable]
    while (PstateMaxVal != 0) {
      LibAmdMsrRead ((PS_REG_BASE + PstateMaxVal), &LocalMsrRegister, StdHeader);
      if (((PSTATE_MSR *) &LocalMsrRegister)->PsEnable == 1) {
        break;
      }
      PstateMaxVal--;
    }

    LibAmdMsrRead ((PS_REG_BASE + PstateMaxVal), &LocalMsrRegister, StdHeader);
    LibAmdMsrWrite ((PS_REG_BASE + PstateMaxVal + 1), &LocalMsrRegister, StdHeader);

    // Step 5 --- Increment the value in F3xDC[PstateMaxVal] by 1
    PstateMaxVal++;
    if (Core == 0) {
      ((CLK_PWR_TIMING_CTRL2_REGISTER *) &PciData)->PstateMaxVal = PstateMaxVal;
      OptionMultiSocketConfiguration.ModifyCurrSocketPci (&PciAddress, 0, PciData, StdHeader);
    }

    // Step 6 --- Write 100b to CpuFid from MSR_C001_00[6B:64] indexed by F3xDC[PstateMaxVal]
    // Step 7 --- Write 10b to CpuDid from MSR_C001_00[6B:64] indexed by F3xDC[PstateMaxVal]
    // Step 8 --- Write 0b to PstateEn from MSR_C001_00[6B:64] indexed by F3xDC[PstateMaxVal]
    LibAmdMsrRead ((PS_REG_BASE + PstateMaxVal), &LocalMsrRegister, StdHeader);
    ((PSTATE_MSR *) &LocalMsrRegister)->CpuFid = 4;
    ((PSTATE_MSR *) &LocalMsrRegister)->CpuDid = 2;
    ((PSTATE_MSR *) &LocalMsrRegister)->PsEnable = 0;
    LibAmdMsrWrite ((PS_REG_BASE + PstateMaxVal), &LocalMsrRegister, StdHeader);

    // Step 9 --- If F3x64[HtcTmpLmt] == 0, write 7Fh to F3x64[HtcTmpLmt]
    // Step 10 --- Write 1b to F3x64[HtcEn]
    GetPciAddress (StdHeader, Socket, 0, &IntNode0PciAddress, &IgnoredSts);
    if (Core == 0) {
      IntNode0PciAddress.Address.Function = FUNC_3;
      IntNode0PciAddress.Address.Register = HTC_REG;
      LibAmdPciRead (AccessWidth32, IntNode0PciAddress, &PciData, StdHeader);
      if (((HTC_REGISTER *) &PciData)->HtcTmpLmt == 0) {
        ((HTC_REGISTER *) &PciData)->HtcTmpLmt = 0x7F;
      }
      ((HTC_REGISTER *) &PciData)->HtcEn = 1;
      IDS_OPTION_HOOK (IDS_HTC_CTRL, &PciData, StdHeader);
      LibAmdPciWrite (AccessWidth32, IntNode0PciAddress, &PciData, StdHeader);
    }

    // Step 11 --- Write OriginalPstate to MSR_C001_0062[PstateCmd]
    LibAmdMsrRead (MSR_PSTATE_CTL, &LocalMsrRegister, StdHeader);
    ((PSTATE_CTRL_MSR *) &LocalMsrRegister)->PstateCmd = (UINT64) OriginalPstate;
    LibAmdMsrWrite (MSR_PSTATE_CTL, &LocalMsrRegister, StdHeader);

    // Step 12 --- If (MSR_C001_0061[CurPstateLimit] > OriginalPstate)
    //                 Wait for (MSR_C001_0063[CurPstate] == MSR_C001_0061[CurPstateLimit])
    //             Else
    //                 Wait for (MSR_C001_0063[CurPstate] == OriginalPstate
    LibAmdMsrRead (MSR_PSTATE_CURRENT_LIMIT, &LocalMsrRegister, StdHeader);
    CurPstateLimit = (UINT8) ((PSTATE_CURLIM_MSR *) &LocalMsrRegister)->CurPstateLimit;
    PstateToWaitFor = (CurPstateLimit > OriginalPstate) ? CurPstateLimit : OriginalPstate;
    do {
      LibAmdMsrRead (MSR_PSTATE_STS, &LocalMsrRegister, StdHeader);
    } while (((PSTATE_STS_MSR *) &LocalMsrRegister)->CurPstate != (UINT64) PstateToWaitFor);

    // Step 13 --- Write F3x64[HtcPstateLimit] and F3xA8[PopDownPstate] with the value from
    //             F3xDC[PstateMaxVal] and exit the sequence
    if (Core == 0) {
      ((HTC_REGISTER *) &PciData)->HtcPstateLimit = PstateMaxVal;
      LibAmdPciWrite (AccessWidth32, IntNode0PciAddress, &PciData, StdHeader);
      PciAddress.Address.Register = POPUP_PSTATE_REG;
      LibAmdPciRead (AccessWidth32, PciAddress, &PciData, StdHeader);
      ((POPUP_PSTATE_REGISTER *) &PciData)->PopDownPstate = PstateMaxVal;
      OptionMultiSocketConfiguration.ModifyCurrSocketPci (&PciAddress, 0, PciData, StdHeader);
    }
  }
  return AGESA_SUCCESS;
}


CONST LOW_PWR_PSTATE_FAMILY_SERVICES ROMDATA F15OrLowPwrPstateSupport =
{
  0,
  (PF_LOW_PWR_PSTATE_IS_SUPPORTED) CommonReturnTrue,
  F15OrInitializeLowPwrPstate
};
