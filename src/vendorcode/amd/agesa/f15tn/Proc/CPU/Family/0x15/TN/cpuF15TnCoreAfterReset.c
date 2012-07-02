/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 Trinity after warm reset sequence for core P-states
 *
 * Performs the "Core Minimum P-State Transition Sequence After Warm Reset"
 * as described in the BKDG.
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
 * Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *
 * AMD is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with AMD.  This header does *NOT* give you permission to use the Materials
 * or any rights under AMD's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 *
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by AMD shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 *
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 *
 * AMD does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by AMD, or
 * result from use of the Materials or any related information.
 *
 * You agree that you will not reverse engineer or decompile the Materials.
 *
 * NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, AMD retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgement of AMD's proprietary rights in them.
 *
 * EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
 * direct product thereof will be exported directly or indirectly, into any
 * country prohibited by the United States Export Administration Act and the
 * regulations thereunder, without the required authorization from the U.S.
 * government nor will be used for any purpose prohibited by the same.
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "cpuF15PowerMgmt.h"
#include "cpuF15TnPowerMgmt.h"
#include "cpuRegisters.h"
#include "GeneralServices.h"
#include "cpuApicUtilities.h"
#include "cpuFamilyTranslation.h"
#include "OptionMultiSocket.h"
#include "cpuF15TnCoreAfterReset.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)


#define FILECODE PROC_CPU_FAMILY_0X15_TN_CPUF15TNCOREAFTERRESET_FILECODE

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
F15TnPmCoreAfterResetPhase1OnCore (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
STATIC
F15TnPmCoreAfterResetPhase2OnCore (
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
 * Family 15h Trinity core 0 entry point for performing the necessary steps for core
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
F15TnPmCoreAfterReset (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       AMD_CPU_EARLY_PARAMS  *CpuEarlyParamsPtr,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINT32       Core;
  UINT32       HwPsMaxVal;
  PCI_ADDR     PciAddress;
  AP_TASK      TaskPtr;

  IDS_HDT_CONSOLE (CPU_TRACE, "  F15TnPmCoreAfterReset\n");

  GetCurrentCore (&Core, StdHeader);
  ASSERT (Core == 0);

  OptionMultiSocketConfiguration.GetCurrPciAddr (&PciAddress, StdHeader);
  PciAddress.Address.Function = FUNC_3;
  PciAddress.Address.Register = CPTC2_REG;
  LibAmdPciRead (AccessWidth32, PciAddress, &HwPsMaxVal, StdHeader);
  HwPsMaxVal = ((CLK_PWR_TIMING_CTRL2_REGISTER *) &HwPsMaxVal)->PstateMaxVal;

  // Launch each local core to perform steps 1 through 3.
  TaskPtr.FuncAddress.PfApTask = F15TnPmCoreAfterResetPhase1OnCore;
  TaskPtr.DataTransfer.DataSizeInDwords = 0;
  TaskPtr.ExeFlags = WAIT_FOR_CORE;
  ApUtilRunCodeOnAllLocalCoresAtEarly (&TaskPtr, StdHeader, CpuEarlyParamsPtr);

  // Launch each local core to perform steps 4 through 6.
  TaskPtr.FuncAddress.PfApTaskI = F15TnPmCoreAfterResetPhase2OnCore;
  TaskPtr.DataTransfer.DataSizeInDwords = 1;
  TaskPtr.DataTransfer.DataPtr = &HwPsMaxVal;
  TaskPtr.DataTransfer.DataTransferFlags = 0;
  TaskPtr.ExeFlags = WAIT_FOR_CORE;
  ApUtilRunCodeOnAllLocalCoresAtEarly (&TaskPtr, StdHeader, CpuEarlyParamsPtr);
}


/*---------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * Support routine for F15TnPmCoreAfterReset to perform MSR initialization on all
 * cores of a family 15h socket.
 *
 * This function implements steps 1 - 3 on each core.
 *
 * @param[in]  StdHeader          Config handle for library and services.
 *
 */
VOID
STATIC
F15TnPmCoreAfterResetPhase1OnCore (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64 CofvidSts;
  UINT64 LocalMsrRegister;
  UINT64 PstateCtrl;

  IDS_HDT_CONSOLE (CPU_TRACE, "  F15TnPmCoreAfterResetPhase1OnCore\n");

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
 * Support routine for F15TnPmCoreAfterReset to perform MSR initialization on all
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
F15TnPmCoreAfterResetPhase2OnCore (
  IN       VOID              *HwPsMaxVal,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64 TargetPsMsr;
  UINT64 LocalMsrRegister;
  UINT64 PstateCtrl;

  IDS_HDT_CONSOLE (CPU_TRACE, "  F15TnPmCoreAfterResetPhase2OnCore\n");

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
    } while (GetF15TnCurCpuVid (&LocalMsrRegister) != GetF15TnCpuVid (&TargetPsMsr));
  }

  // 6. Wait for MSRC001_0063[CurPstate] = MSRC001_0062[PstateCmd].
  LibAmdMsrRead (MSR_PSTATE_CTL, &PstateCtrl, StdHeader);
  do {
    LibAmdMsrRead (MSR_PSTATE_STS, &LocalMsrRegister, StdHeader);
  } while (((PSTATE_STS_MSR *) &LocalMsrRegister)->CurPstate != ((PSTATE_CTRL_MSR *) &PstateCtrl)->PstateCmd);
}
