/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_16 Kabini after warm reset sequence for NB P-states
 *
 * Performs the "NB COF and VID Transition Sequence After Warm Reset"
 * as described in the BKDG.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x16/KB
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
#include "cpuF16PowerMgmt.h"
#include "F16KbPowerMgmt.h"
#include "cpuRegisters.h"
#include "cpuApicUtilities.h"
#include "cpuFamilyTranslation.h"
#include "GeneralServices.h"
#include "cpuServices.h"
#include "F16KbNbAfterReset.h"
#include "Gnb.h"
#include "GnbPcie.h"
#include "GnbHandleLib.h"
#include "GnbRegisterAccKB.h"
#include "GnbRegistersKB.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X16_KB_F16KBNBAFTERRESET_FILECODE

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
F16KbPmNbAfterResetOnCore (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
STATIC
TransitionToNbLow (
  IN       PCI_ADDR           PciAddress,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

VOID
STATIC
TransitionToNbHigh (
  IN       PCI_ADDR           PciAddress,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

VOID
STATIC
WaitForNbTransitionToComplete (
  IN       PCI_ADDR           PciAddress,
  IN       UINT32             PstateIndex,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * Family 16h Kabini core 0 entry point for performing the necessary steps after
 * a warm reset has occurred.
 *
 * The steps are as follows:
 *
 *   1.    Temp1=D18F5x170[SwNbPstateLoDis].
 *   2.    Temp2=D18F5x170[NbPstateDisOnP0].
 *   3.    Temp3=D18F5x170[NbPstateThreshold].
 *   4.    Temp4=D18F5x170[NbPstateGnbSlowDis].
 *   5.    If MSRC001_0070[NbPstate]=0, go to step 6. If MSRC001_0070[NbPstate]=1, go to step 11.
 *   6.    Write 1 to D18F5x170[NbPstateGnbSlowDis].
 *   7.    Write 0 to D18F5x170[SwNbPstateLoDis, NbPstateDisOnP0, NbPstateThreshold].
 *   8.    Wait for D18F5x174[CurNbPstate] = D18F5x170[NbPstateLo] and D18F5x174[CurNbFid, CurNb-
 *         Did]=[NbFid, NbDid] from D18F5x1[6C:60] indexed by D18F5x170[NbPstateLo].
 *   9.    Set D18F5x170[SwNbPstateLoDis]=1.
 *   10.   Wait for D18F5x174[CurNbPstate] = D18F5x170[NbPstateHi] and D18F5x174[CurNbFid, CurNb-
 *         Did]=[NbFid, NbDid] from D18F5x1[6C:60] indexed by D18F5x170[NbPstateHi]. Go to step 15.
 *   11.   Write 1 to D18F5x170[SwNbPstateLoDis].
 *   12.   Wait for D18F5x174[CurNbPstate] = D18F5x170[NbPstateHi] and D18F5x174[CurNbFid, CurNb-
 *         Did]=[NbFid, NbDid] from D18F5x1[6C:60] indexed by D18F5x170[NbPstateHi].
 *   13.   Write 0 to D18F5x170[SwNbPstateLoDis, NbPstateDisOnP0, NbPstateThreshold].
 *   14.   Wait for D18F5x174[CurNbPstate] = D18F5x170[NbPstateLo] and D18F5x174[CurNbFid, CurNb-
 *         Did]=[NbFid, NbDid] from D18F5x1[6C:60] indexed by D18F5x170[NbPstateLo].
 *   15.   Set D18F5x170[SwNbPstateLoDis]=Temp1, D18F5x170[NbPstateDisOnP0]=Temp2, D18F5x170[NbP-
 *         stateThreshold]=Temp3, and D18F5x170[NbPstateGnbSlowDis]=Temp4.
 *
 * @param[in]  FamilySpecificServices  The current Family Specific Services.
 * @param[in]  CpuEarlyParamsPtr       Service parameters
 * @param[in]  StdHeader               Config handle for library and services.
 *
 */
VOID
F16KbPmNbAfterReset (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       AMD_CPU_EARLY_PARAMS  *CpuEarlyParamsPtr,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINT32    Socket;
  UINT32    Module;
  UINT32    Core;
  UINT32    TaskedCore;
  UINT32    Ignored;
  AP_TASK   TaskPtr;
  AGESA_STATUS IgnoredSts;

  IDS_HDT_CONSOLE (CPU_TRACE, "  F16KbPmNbAfterReset\n");

  IdentifyCore (StdHeader, &Socket, &Module, &Core, &IgnoredSts);

  ASSERT (Core == 0);

  // Launch one core per node.
  TaskPtr.FuncAddress.PfApTask = F16KbPmNbAfterResetOnCore;
  TaskPtr.DataTransfer.DataSizeInDwords = 0;
  TaskPtr.ExeFlags = WAIT_FOR_CORE;
  for (Module = 0; Module < GetPlatformNumberOfModules (); Module++) {
    if (GetGivenModuleCoreRange (Socket, Module, &TaskedCore, &Ignored, StdHeader)) {
      if (TaskedCore != 0) {
        ApUtilRunCodeOnSocketCore ((UINT8) Socket, (UINT8) TaskedCore, &TaskPtr, StdHeader);
      }
    }
  }
  ApUtilTaskOnExecutingCore (&TaskPtr, StdHeader, (VOID *) CpuEarlyParamsPtr);
}

/*---------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * Support routine for F16KbPmNbAfterReset to perform MSR initialization on one
 * core of each die in a family 16h socket.
 *
 * This function implements steps 1 - 15 on each core.
 *
 * @param[in]  StdHeader          Config handle for library and services.
 *
 */
VOID
STATIC
F16KbPmNbAfterResetOnCore (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32    NbPsCtrlOnEntry;
  UINT32    NbPsCtrlOnExit;
  UINT64    LocalMsrRegister;
  PCI_ADDR  PciAddress;

  IDS_HDT_CONSOLE (CPU_TRACE, "  F16KbPmNbAfterResetOnCore\n");

  // 1. Temp1 = D18F5x170[SwNbPstateLoDis].
  // 2. Temp2 = D18F5x170[NbPstateDisOnP0].
  // 3. Temp3 = D18F5x170[NbPstateThreshold].
  // 4. Temp4 = D18F5x170[NbPstateGnbSlowDis].
  PciAddress.AddressValue = NB_PSTATE_CTRL_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, PciAddress, &NbPsCtrlOnEntry, StdHeader);

  // Check if NB P-states were disabled, and if so, prevent any changes from occurring.
  if (((NB_PSTATE_CTRL_REGISTER *) &NbPsCtrlOnEntry)->SwNbPstateLoDis == 0) {
    // 5. If MSRC001_0070[NbPstate] = 1, go to step 11
    LibAmdMsrRead (MSR_COFVID_CTL, &LocalMsrRegister, StdHeader);
    if (((COFVID_CTRL_MSR *) &LocalMsrRegister)->NbPstate == 0) {
      // 6. Write 1 to D18F5x170[NbPstateGnbSlowDis].
      PciAddress.AddressValue = NB_PSTATE_CTRL_PCI_ADDR;
      LibAmdPciRead (AccessWidth32, PciAddress, &NbPsCtrlOnExit, StdHeader);
      ((NB_PSTATE_CTRL_REGISTER *) &NbPsCtrlOnExit)->NbPstateGnbSlowDis = 1;
      LibAmdPciWrite (AccessWidth32, PciAddress, &NbPsCtrlOnExit, StdHeader);

      // 7. Write 0 to D18F5x170[SwNbPstateLoDis, NbPstateDisOnP0, NbPstateThreshold].
      // 8. Wait for D18F5x174[CurNbPstate] = D18F5x170[NbPstateLo] and D18F5x174[CurNbFid,
      //    CurNbDid] = [NbFid, NbDid] from D18F5x1[6C:60] indexed by D18F5x170[NbPstateLo].
      TransitionToNbLow (PciAddress, StdHeader);

      // 9. Set D18F5x170[SwNbPstateLoDis] = 1.
      // 10. Wait for D18F5x174[CurNbPstate] = D18F5x170[NbPstateHi] and D18F5x174[CurNbFid,
      //     CurNbDid] = [NbFid, NbDid] from D18F5x1[6C:60] indexed by D18F5x170[NbPstateHi].
      //     Go to step 15.
      TransitionToNbHigh (PciAddress, StdHeader);
    } else {
      // 11. Set D18F5x170[SwNbPstateLoDis] = 1.
      // 12. Wait for D18F5x174[CurNbPstate] = D18F5x170[NbPstateHi] and D18F5x174[CurNbFid,
      //     CurNbDid] = [NbFid, NbDid] from D18F5x1[6C:60] indexed by D18F5x170[NbPstateHi].
      TransitionToNbHigh (PciAddress, StdHeader);

      // 13. Write 0 to D18F5x170[SwNbPstateLoDis, NbPstateDisOnP0, NbPstateThreshold].
      // 14. Wait for D18F5x174[CurNbPstate] = D18F5x170[NbPstateLo] and D18F5x174[CurNbFid,
      //     CurNbDid] = [NbFid, NbDid] from D18F5x1[6C:60] indexed by D18F5x170[NbPstateLo].
      TransitionToNbLow (PciAddress, StdHeader);
    }

    // 15. Set D18F5x170[SwNbPstateLoDis]=Temp1, D18F5x170[NbPstateDisOnP0]=Temp2, D18F5x170[NbP-
    //     stateThreshold]=Temp3, and D18F5x170[NbPstateGnbSlowDis]=Temp4.
    LibAmdPciRead (AccessWidth32, PciAddress, &NbPsCtrlOnExit, StdHeader);
    ((NB_PSTATE_CTRL_REGISTER *) &NbPsCtrlOnExit)->SwNbPstateLoDis = ((NB_PSTATE_CTRL_REGISTER *) &NbPsCtrlOnEntry)->SwNbPstateLoDis;
    ((NB_PSTATE_CTRL_REGISTER *) &NbPsCtrlOnExit)->NbPstateDisOnP0 = ((NB_PSTATE_CTRL_REGISTER *) &NbPsCtrlOnEntry)->NbPstateDisOnP0;
    ((NB_PSTATE_CTRL_REGISTER *) &NbPsCtrlOnExit)->NbPstateThreshold = ((NB_PSTATE_CTRL_REGISTER *) &NbPsCtrlOnEntry)->NbPstateThreshold;
    ((NB_PSTATE_CTRL_REGISTER *) &NbPsCtrlOnExit)->NbPstateGnbSlowDis = ((NB_PSTATE_CTRL_REGISTER *) &NbPsCtrlOnEntry)->NbPstateGnbSlowDis;
    LibAmdPciWrite (AccessWidth32, PciAddress, &NbPsCtrlOnExit, StdHeader);
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Support routine for F16KbPmNbAfterResetOnCore to transition to the low NB P-state.
 *
 * This function implements steps 7, 8, 13, and 14 as needed.
 *
 * @param[in]  PciAddress         Segment, bus, device number of the node to transition.
 * @param[in]  StdHeader          Config handle for library and services.
 *
 */
VOID
STATIC
TransitionToNbLow (
  IN       PCI_ADDR           PciAddress,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  NB_PSTATE_CTRL_REGISTER   NbPsCtrl;

  IDS_HDT_CONSOLE (CPU_TRACE, "  TransitionToNbLow\n");

  // 7/13. Write 0 to D18F5x170[SwNbPstateLoDis, NbPstateDisOnP0, NbPstateThreshold].
  PciAddress.AddressValue = NB_PSTATE_CTRL_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, PciAddress, &NbPsCtrl, StdHeader);
  NbPsCtrl.SwNbPstateLoDis = 0;
  NbPsCtrl.NbPstateDisOnP0 = 0;
  NbPsCtrl.NbPstateThreshold = 0;
  LibAmdPciWrite (AccessWidth32, PciAddress, &NbPsCtrl, StdHeader);

  // 8/14. Wait for D18F5x174[CurNbPstate] = D18F5x170[NbPstateLo] and D18F5x174[CurNbFid,
  //       CurNbDid] = [NbFid, NbDid] from D18F5x1[6C:60] indexed by D18F5x170[NbPstateLo].
  WaitForNbTransitionToComplete (PciAddress, NbPsCtrl.NbPstateLo, StdHeader);
}

/*---------------------------------------------------------------------------------------*/
/**
 * Support routine for F16KbPmNbAfterResetOnCore to transition to the high NB P-state.
 *
 * This function implements steps 9, 10, 11, and 12 as needed.
 *
 * @param[in]  PciAddress         Segment, bus, device number of the node to transition.
 * @param[in]  StdHeader          Config handle for library and services.
 *
 */
VOID
STATIC
TransitionToNbHigh (
  IN       PCI_ADDR           PciAddress,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  NB_PSTATE_CTRL_REGISTER   NbPsCtrl;

  IDS_HDT_CONSOLE (CPU_TRACE, "  TransitionToNbHigh\n");

  // 9/10. Set D18F5x170[SwNbPstateLoDis] = 1.
  PciAddress.AddressValue = NB_PSTATE_CTRL_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, PciAddress, &NbPsCtrl, StdHeader);
  NbPsCtrl.SwNbPstateLoDis = 1;
  LibAmdPciWrite (AccessWidth32, PciAddress, &NbPsCtrl, StdHeader);

  // 11/12. Wait for D18F5x174[CurNbPstate] = D18F5x170[NbPstateHi] and D18F5x174[CurNbFid,
  //       CurNbDid] = [NbFid, NbDid] from D18F5x1[6C:60] indexed by D18F5x170[NbPstateHi].
  WaitForNbTransitionToComplete (PciAddress, NbPsCtrl.NbPstateHi, StdHeader);
}

/*---------------------------------------------------------------------------------------*/
/**
 * Support routine for F16KbPmAfterResetCore to wait for NB FID and DID to
 * match a specific P-state.
 *
 * This function implements steps 8, 10, 12, and 14 as needed.
 *
 * @param[in]  PciAddress         Segment, bus, device number of the node to transition.
 * @param[in]  PstateIndex        P-state settings to match.
 * @param[in]  StdHeader          Config handle for library and services.
 *
 */
VOID
STATIC
WaitForNbTransitionToComplete (
  IN       PCI_ADDR           PciAddress,
  IN       UINT32             PstateIndex,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  NB_PSTATE_REGISTER       TargetNbPs;
  NB_PSTATE_STS_REGISTER   NbPsSts;

  IDS_HDT_CONSOLE (CPU_TRACE, "  WaitForNbTransitionToComplete\n");

  PciAddress.Address.Function = FUNC_5;
  PciAddress.Address.Register = NB_PSTATE_0 + (PstateIndex << 2);
  LibAmdPciRead (AccessWidth32, PciAddress, &TargetNbPs, StdHeader);
  PciAddress.Address.Register = NB_PSTATE_STATUS;
  do {
    LibAmdPciRead (AccessWidth32, PciAddress, &NbPsSts, StdHeader);
  } while ((NbPsSts.CurNbPstate != PstateIndex ||
           (NbPsSts.CurNbFid != TargetNbPs.NbFid)) ||
           (NbPsSts.CurNbDid != TargetNbPs.NbDid));
}
