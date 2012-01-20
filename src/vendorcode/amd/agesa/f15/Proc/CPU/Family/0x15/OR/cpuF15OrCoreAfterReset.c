/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 Orochi after warm reset sequence for core P-states
 *
 * Performs the "Core Minimum P-State Transition Sequence After Warm Reset"
 * as described in the BKDG.
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
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "cpuF15PowerMgmt.h"
#include "cpuF15OrPowerMgmt.h"
#include "cpuRegisters.h"
#include "GeneralServices.h"
#include "cpuApicUtilities.h"
#include "cpuFamilyTranslation.h"
#include "OptionMultiSocket.h"
#include "cpuF15OrCoreAfterReset.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)


#define FILECODE PROC_CPU_FAMILY_0X15_OR_CPUF15ORCOREAFTERRESET_FILECODE

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
VOID
STATIC
F15OrPmCoreAfterResetPhase1OnCore (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
STATIC
F15OrPmCoreAfterResetPhase2OnCore (
  IN       VOID              *HwPsMaxVal,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
extern OPTION_MULTISOCKET_CONFIGURATION OptionMultiSocketConfiguration;
/*---------------------------------------------------------------------------------------*/
/**
 * Family 15h Orochi core 0 entry point for performing the necessary steps for core
 * P-states after a warm reset has occurred.
 *
 * The steps are as follows:
 *    1. Write 0 to MSRC001_0062[PstateCmd] on all cores in the processor.
 *    2. Wait for MSRC001_0071[CurCpuFid, CurCpuDid] = [CpuFid, CpuDid] from
 *       MSRC001_00[6B:64] indexed by MSRC001_0071[CurPstateLimit].
 *    3. Write MSRC001_0061[PstateMaxVal] to MSRC001_0062[PstateCmd] on all
 *       cores in the processor.
 *    4. Wait for MSRC001_0071[CurCpuFid, CurCpuDid] = [CpuFid, CpuDid] from
 *       MSRC001_00[6B:64] indexed by MSRC001_0061[PstateMaxVal].
 *    5. If MSRC001_0071[CurPstateLimit] != MSRC001_0071[CurPstate], wait for
 *       MSRC001_0071[CurCpuVid] = [CpuVid] from MSRC001_00[6B:64] indexed by
 *       MSRC001_0061[PstateMaxVal].
 *    6. Wait for MSRC001_0063[CurPstate] = MSRC001_0062[PstateCmd].
 *
 * @param[in]  FamilySpecificServices  The current Family Specific Services.
 * @param[in]  CpuEarlyParamsPtr       Service parameters
 * @param[in]  StdHeader               Config handle for library and services.
 *
 */
VOID
F15OrPmCoreAfterReset (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       AMD_CPU_EARLY_PARAMS  *CpuEarlyParamsPtr,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINT32       Core;
  UINT32       HwPsMaxVal;
  PCI_ADDR     PciAddress;
  AP_TASK      TaskPtr;
  IDS_SKIP_HOOK (IDS_SKIP_PM_TRANSITION_STEP, CpuEarlyParamsPtr, StdHeader) {

    OptionMultiSocketConfiguration.GetCurrPciAddr (&PciAddress, StdHeader);
    GetCurrentCore (&Core, StdHeader);
    ASSERT (Core == 0);

    PciAddress.Address.Function = FUNC_3;
    PciAddress.Address.Register = CPTC2_REG;
    LibAmdPciRead (AccessWidth32, PciAddress, &HwPsMaxVal, StdHeader);
    HwPsMaxVal = ((CLK_PWR_TIMING_CTRL2_REGISTER *) &HwPsMaxVal)->PstateMaxVal;

    // Launch each local core to perform steps 1 through 3.
    TaskPtr.FuncAddress.PfApTask = F15OrPmCoreAfterResetPhase1OnCore;
    TaskPtr.DataTransfer.DataSizeInDwords = 0;
    TaskPtr.ExeFlags = WAIT_FOR_CORE;
    ApUtilRunCodeOnAllLocalCoresAtEarly (&TaskPtr, StdHeader, CpuEarlyParamsPtr);

    // Launch each local core to perform steps 4 through 6.
    TaskPtr.FuncAddress.PfApTaskI = F15OrPmCoreAfterResetPhase2OnCore;
    TaskPtr.DataTransfer.DataSizeInDwords = 1;
    TaskPtr.DataTransfer.DataPtr = &HwPsMaxVal;
    TaskPtr.DataTransfer.DataTransferFlags = 0;
    TaskPtr.ExeFlags = WAIT_FOR_CORE;
    ApUtilRunCodeOnAllLocalCoresAtEarly (&TaskPtr, StdHeader, CpuEarlyParamsPtr);
  }
}


/*---------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * Support routine for F15OrPmCoreAfterReset to perform MSR initialization on all
 * cores of a family 15h socket.
 *
 * This function implements steps 1 - 3 on each core.
 *
 * @param[in]  StdHeader          Config handle for library and services.
 *
 */
VOID
STATIC
F15OrPmCoreAfterResetPhase1OnCore (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64 CofvidSts;
  UINT64 LocalMsrRegister;
  UINT64 PstateCtrl;

  // 1. Write 0 to MSRC001_0062[PstateCmd] on all cores in the processor.
  PstateCtrl = 0;
  LibAmdMsrWrite (MSR_PSTATE_CTL, &PstateCtrl, StdHeader);

  // 2. Wait for MSRC001_0071[CurCpuFid, CurCpuDid] = [CpuFid, CpuDid] from
  //    MSRC001_00[6B:64] indexed by MSRC001_0071[CurPstateLimit].
  do {
    LibAmdMsrRead (MSR_COFVID_STS, &CofvidSts, StdHeader);
    LibAmdMsrRead ((UINT32) (MSR_PSTATE_0 + (UINT32) (((COFVID_STS_MSR *) &CofvidSts)->CurPstateLimit)), &LocalMsrRegister, StdHeader);
  } while ((((COFVID_STS_MSR *) &CofvidSts)->CurCpuFid != ((PSTATE_MSR *) &LocalMsrRegister)->CpuFid) ||
           (((COFVID_STS_MSR *) &CofvidSts)->CurCpuDid != ((PSTATE_MSR *) &LocalMsrRegister)->CpuDid));

  // 3. Write MSRC001_0061[PstateMaxVal] to MSRC001_0062[PstateCmd] on all
  //    cores in the processor.
  LibAmdMsrRead (MSR_PSTATE_CURRENT_LIMIT, &LocalMsrRegister, StdHeader);
  ((PSTATE_CTRL_MSR *) &PstateCtrl)->PstateCmd = ((PSTATE_CURLIM_MSR *) &LocalMsrRegister)->PstateMaxVal;
  LibAmdMsrWrite (MSR_PSTATE_CTL, &PstateCtrl, StdHeader);
}

/*---------------------------------------------------------------------------------------*/
/**
 * Support routine for F15OrPmCoreAfterReset to perform MSR initialization on all
 * cores of a family 15h socket.
 *
 * This function implements steps 4 - 6 on each core.
 *
 * @param[in]  HwPsMaxVal         Index of the highest enabled HW P-state.
 * @param[in]  StdHeader          Config handle for library and services.
 *
 */
VOID
STATIC
F15OrPmCoreAfterResetPhase2OnCore (
  IN       VOID              *HwPsMaxVal,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64 TargetPsMsr;
  UINT64 LocalMsrRegister;
  UINT64 PstateCtrl;

  // 4. Wait for MSRC001_0071[CurCpuFid, CurCpuDid] = [CpuFid, CpuDid] from
  //    MSRC001_00[6B:64] indexed by D18F3xDC[PstateMaxVal].
  LibAmdMsrRead ((*(UINT32 *) HwPsMaxVal) + MSR_PSTATE_0, &TargetPsMsr, StdHeader);
  do {
    LibAmdMsrRead (MSR_COFVID_STS, &LocalMsrRegister, StdHeader);
  } while ((((COFVID_STS_MSR *) &LocalMsrRegister)->CurCpuFid != ((PSTATE_MSR *) &TargetPsMsr)->CpuFid) ||
           (((COFVID_STS_MSR *) &LocalMsrRegister)->CurCpuDid != ((PSTATE_MSR *) &TargetPsMsr)->CpuDid));

  // 5. If MSRC001_0071[CurPstateLimit] != MSRC001_0071[CurPstate], wait for
  //    MSRC001_0071[CurCpuVid] = [CpuVid] from MSRC001_00[6B:64] indexed by
  //    MSRC001_0061[PstateMaxVal].
  if (((COFVID_STS_MSR *) &LocalMsrRegister)->CurPstateLimit != ((COFVID_STS_MSR *) &LocalMsrRegister)->CurPstate) {
    do {
      LibAmdMsrRead (MSR_COFVID_STS, &LocalMsrRegister, StdHeader);
    } while ((((COFVID_STS_MSR *) &LocalMsrRegister)->CurCpuVid != ((PSTATE_MSR *) &TargetPsMsr)->CpuVid));
  }

  // 6. Wait for MSRC001_0063[CurPstate] = MSRC001_0062[PstateCmd].
  LibAmdMsrRead (MSR_PSTATE_CTL, &PstateCtrl, StdHeader);
  do {
    LibAmdMsrRead (MSR_PSTATE_STS, &LocalMsrRegister, StdHeader);
  } while (((PSTATE_STS_MSR *) &LocalMsrRegister)->CurPstate != ((PSTATE_CTRL_MSR *) &PstateCtrl)->PstateCmd);
}
