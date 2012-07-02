/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 Trinity after warm reset sequence for NB P-states
 *
 * Performs the "NB COF and VID Transition Sequence After Warm Reset"
 * as described in the BKDG.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x15/TN
 * @e \$Revision: 64197 $   @e \$Date: 2012-01-17 16:18:33 -0600 (Tue, 17 Jan 2012) $
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
#include "cpuApicUtilities.h"
#include "cpuFamilyTranslation.h"
#include "GeneralServices.h"
#include "cpuServices.h"
#include "heapManager.h"
#include "cpuF15TnNbAfterReset.h"
#include "GnbRegisterAccTN.h"
#include "GnbRegistersTN.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X15_TN_CPUF15TNNBAFTERRESET_FILECODE

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
F15TnPmNbAfterResetOnCore (
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
 * Family 15h Trinity core 0 entry point for performing the necessary steps after
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
F15TnPmNbAfterReset (
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
  PCI_ADDR  PciAddress;
  AGESA_STATUS IgnoredSts;
  LOCATE_HEAP_PTR Locate;

  IDS_HDT_CONSOLE (CPU_TRACE, "  F15TnPmNbAfterReset\n");

  IdentifyCore (StdHeader, &Socket, &Module, &Core, &IgnoredSts);

  ASSERT (Core == 0);

  if (FamilySpecificServices->IsNbPstateEnabled (FamilySpecificServices, &CpuEarlyParamsPtr->PlatformConfig, StdHeader)) {
    PciAddress.AddressValue = NB_PSTATE_CTRL_PCI_ADDR;
    Locate.BufferHandle = AMD_CPU_NB_PSTATE_FIXUP_HANDLE;
    if (HeapLocateBuffer (&Locate, StdHeader) == AGESA_SUCCESS) {
      LibAmdPciWrite (AccessWidth32, PciAddress, Locate.BufferPtr, StdHeader);
    } else {
      ASSERT (FALSE);
    }
  }

  // Launch one core per node.
  TaskPtr.FuncAddress.PfApTask = F15TnPmNbAfterResetOnCore;
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

/*---------------------------------------------------------------------------------------*/
/**
 * Family 15h Trinity core 0 entry point for performing the necessary Nb P-state VID adjustment
 * after a cold reset has occurred.
 *
 * @param[in]  FamilySpecificServices  The current Family Specific Services.
 * @param[in]  CpuEarlyParamsPtr       Service parameters
 * @param[in]  StdHeader               Config handle for library and services.
 *
 */
VOID
F15TnNbPstateVidAdjustAfterReset (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       AMD_CPU_EARLY_PARAMS   *CpuEarlyParamsPtr,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  PCI_ADDR                  PciAddress;
  BOOLEAN                   NeitherHiNorLo;
  NB_PSTATE_REGISTER        NbPsReg;
  UINT32                    NbPsVid;
  UINT32                    i;
  NB_PSTATE_CTRL_REGISTER   NbPsCtrl;
  NB_PSTATE_CTRL_REGISTER   NbPsCtrlSave;
  NB_PSTATE_STS_REGISTER    NbPsSts;
  CLK_PWR_TIMING_CTRL_5_REGISTER  ClkPwrTimgCtrl5;
  D0F0xBC_x1F400_STRUCT     D0F0xBC_x1F400;

  // Check if D18F5x188[NbOffsetTrim] has been programmed to 01b (-25mV)
  PciAddress.AddressValue = CPTC5_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, PciAddress, &ClkPwrTimgCtrl5, StdHeader);
  if (ClkPwrTimgCtrl5.NbOffsetTrim == 1) {
    return;
  }

  // Add 25mV (-4 VID steps) to all VddNb VIDs.
  PciAddress.AddressValue = NB_PSTATE_0_PCI_ADDR;

  for (i = 0; i < NM_NB_PS_REG; i++) {
    PciAddress.Address.Register = NB_PSTATE_0 + (i * 4);
    LibAmdPciRead (AccessWidth32, PciAddress, &NbPsReg, StdHeader);
    if (NbPsReg.NbPstateEn == 1) {
      NbPsVid = GetF15TnNbVid (&NbPsReg);
      NbPsVid -= 4;
      SetF15TnNbVid (&NbPsReg, &NbPsVid);
      LibAmdPciWrite (AccessWidth32, PciAddress, &NbPsReg, StdHeader);
    }
  }

  // Check if D18F5x174[CurNbPstate] equals NbPstateHi or NbPstateLo
  PciAddress.Address.Register = NB_PSTATE_STATUS;
  LibAmdPciRead (AccessWidth32, PciAddress, &NbPsSts, StdHeader);
  PciAddress.Address.Register = NB_PSTATE_CTRL;
  LibAmdPciRead (AccessWidth32, PciAddress, &NbPsCtrl, StdHeader);
  // Save NB P-state control setting
  NbPsCtrlSave = NbPsCtrl;

  // Force a NB P-state Transition.
  NeitherHiNorLo = FALSE;
  if (NbPsSts.CurNbPstate == NbPsCtrl.NbPstateHi) {
    TransitionToNbLow (PciAddress, StdHeader);
  } else if (NbPsSts.CurNbPstate == NbPsCtrl.NbPstateLo) {
    TransitionToNbHigh (PciAddress, StdHeader);
  } else {
    NeitherHiNorLo = TRUE;
  }

  // Set OffsetTrim to -25mV:
  //   D18F5x188[NbOffsetTrim]=01b (-25mV)
  //   D0F0xBC_x1F400[SviLoadLineOffsetVddNB]=01b (-25mV)
  PciAddress.Address.Register = CPTC5_REG;
  LibAmdPciRead (AccessWidth32, PciAddress, &ClkPwrTimgCtrl5, StdHeader);
  ClkPwrTimgCtrl5.NbOffsetTrim = 1;
  LibAmdPciWrite (AccessWidth32, PciAddress, &ClkPwrTimgCtrl5, StdHeader);

  GnbRegisterReadTN (D0F0xBC_x1F400_TYPE, D0F0xBC_x1F400_ADDRESS, &D0F0xBC_x1F400, 0, StdHeader);
  D0F0xBC_x1F400.Field.SviLoadLineOffsetVddNB = 1;
  GnbRegisterWriteTN (D0F0xBC_x1F400_TYPE, D0F0xBC_x1F400_ADDRESS, &D0F0xBC_x1F400, 0, StdHeader);

  // Unforce NB P-state back to CurNbPstate value upon entry.
  if (NeitherHiNorLo || (NbPsSts.CurNbPstate == NbPsCtrl.NbPstateHi)) {
    TransitionToNbHigh (PciAddress, StdHeader);
  } else {
    // if (NbPsSts.CurNbPstate == NbPsCtrl.NbPstateLo)
    TransitionToNbLow (PciAddress, StdHeader);
  }

  // Restore NB P-state control setting
  PciAddress.Address.Register = NB_PSTATE_CTRL;
  NbPsCtrl = NbPsCtrlSave;
  LibAmdPciWrite (AccessWidth32, PciAddress, &NbPsCtrl, StdHeader);
}


/*---------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * Support routine for F15TnPmNbAfterReset to perform MSR initialization on one
 * core of each die in a family 15h socket.
 *
 * This function implements steps 1 - 15 on each core.
 *
 * @param[in]  StdHeader          Config handle for library and services.
 *
 */
VOID
STATIC
F15TnPmNbAfterResetOnCore (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32    NbPsCtrlOnEntry;
  UINT32    NbPsCtrlOnExit;
  UINT64    LocalMsrRegister;
  PCI_ADDR  PciAddress;

  IDS_HDT_CONSOLE (CPU_TRACE, "  F15TnPmNbAfterResetOnCore\n");

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
 * Support routine for F15TnPmNbAfterResetOnCore to transition to the low NB P-state.
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
 * Support routine for F15TnPmNbAfterResetOnCore to transition to the high NB P-state.
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
 * Support routine for F15TnPmAfterResetCore to wait for NB FID and DID to
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
