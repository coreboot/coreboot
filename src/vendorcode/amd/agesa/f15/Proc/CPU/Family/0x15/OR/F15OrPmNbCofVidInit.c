/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 Orochi NB COF VID Initialization
 *
 * Performs the "BIOS Northbridge COF and VID Configuration" as
 * described in the BKDG.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x15/OR
 * @e \$Revision: 51891 $   @e \$Date: 2011-04-28 12:39:55 -0600 (Thu, 28 Apr 2011) $
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
#include "cpuRegisters.h"
#include "cpuF15PowerMgmt.h"
#include "cpuF15OrPowerMgmt.h"
#include "cpuApicUtilities.h"
#include "OptionMultiSocket.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "cpuFamilyTranslation.h"
#include "F15OrPmNbCofVidInit.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X15_OR_F15ORPMNBCOFVIDINIT_FILECODE


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
F15OrPmNbCofVidInitOnCore (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
extern OPTION_MULTISOCKET_CONFIGURATION OptionMultiSocketConfiguration;

/*---------------------------------------------------------------------------------------*/
/**
 * Family 15h Orochi core 0 entry point for performing the "Mixed Northbridge Frequency
 * Configuration Sequence"
 *
 * BIOS must match F5x1[6C:60][NbFid, NbDid, NbPstateEn] between all
 * processors of a multi-socket system.  The lowest setting from all
 * processors is used as the common F5x1[6C:60][NbFid, NbDid].  All
 * processors must have the same number of NB P-states.
 *
 * For each node in the system {
 *   For (i = 0; i <= F5x170[NbPstateMaxVal]; i++) {
 *     NewNbFreq = the lowest NBCOF from all processors for NB P-state i
 *     NewNbFid = F5x1[6C:60][NbFid] that corresponds to NewNbFreq
 *     NewNbDid = F5x1[6C:60][NbDid] that corresponds to NewNbFreq
 *     Write NewNbFid and NewNbDid to F5x1[6C:60][NbFid, NbDid] indexed
 *       by NB P-state i
 *   }
 *   If (F5x170[NbPstateMaxVal] == 0) {
 *     Save F5x170 and F5x1[6C:60] indexed by NB P-state 1
 *     Copy F5x1[6C:60] indexed by NB P-state 0 to F5x1[6C:60] indexed by NB P-state 1
 *     Write 1 to F5x170[NbPstateMaxVal, NbPstateLo]
 *     Write 0 to F5x170[SwNbPstateLoDis, NbPstateDisOnP0, NbPstateThreshold]
 *     Wait for F5x174[CurNbPstate] = F5x170[NbPstateLo] and F5x174[CurNbFid, CurNb-
 *       Did]=[NbFid, NbDid] from F5x1[6C:60] indexed by F5x170[NbPstateLo]
 *     Restore F5x170 and F5x1[6C:60] indexed by NB P-state 1
 *     Wait for F5x174[CurNbPstate] = F5x170[NbPstateHi]
 *   }
 * }
 *
 * @param[in]  FamilySpecificServices  The current Family Specific Services.
 * @param[in]  CpuEarlyParamsPtr       Service related parameters (unused).
 * @param[in]  StdHeader               Config handle for library and services.
 *
 */
VOID
F15OrPmNbCofVidInit (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       AMD_CPU_EARLY_PARAMS  *CpuEarlyParamsPtr,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINT32        Socket;
  UINT32        Module;
  UINT32        Core;
  UINT32        i;
  UINT32        NbFreq;
  UINT32        NbDiv;
  UINT32        LocalPciRegister;
  UINT32        AndMask;
  UINT32        OrMask;
  UINT32        Ignored;
  UINT32        NbPsCtrl;
  UINT32        TaskedCore;
  BOOLEAN       PstateSettingsChanged;
  BOOLEAN       PstatesMatch;
  BOOLEAN       PstateEnabledAll;
  AP_TASK       TaskPtr;
  PCI_ADDR      PciAddress;
  AGESA_STATUS  IgnoredSts;

  // Get the local node ID
  IdentifyCore (StdHeader, &Socket, &Module, &Core, &IgnoredSts);

  ASSERT (Core == 0);

  PstateSettingsChanged = FALSE;
  GetPciAddress (StdHeader, Socket, Module, &PciAddress, &IgnoredSts);
  PciAddress.Address.Function = FUNC_5;
  PciAddress.Address.Register = NB_PSTATE_CTRL;
  LibAmdPciRead (AccessWidth32, PciAddress, &NbPsCtrl, StdHeader);
  for (i = 0; i <= ((NB_PSTATE_CTRL_REGISTER *) &NbPsCtrl)->NbPstateMaxVal; i++) {
    if (OptionMultiSocketConfiguration.GetSystemNbPstateSettings (i, &CpuEarlyParamsPtr->PlatformConfig, &NbFreq, &NbDiv, &PstatesMatch, &PstateEnabledAll, StdHeader)) {
      if (PstateEnabledAll) {
        // Valid system-wide NB P-state
        if (!PstatesMatch) {
          // Configure NbPstate[i] to match the slowest
          PciAddress.Address.Register = (NB_PSTATE_0 + (4 * i));
          LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
          OrMask = 0x00000000;
          ((NB_PSTATE_REGISTER *) &OrMask)->NbFid = ((NbFreq / 200) - 4);
          ((NB_PSTATE_REGISTER *) &OrMask)->NbDid = (UINT32) LibAmdBitScanForward (NbDiv);
          if ((((NB_PSTATE_REGISTER *) &OrMask)->NbFid != ((NB_PSTATE_REGISTER *) &LocalPciRegister)->NbFid) ||
              (((NB_PSTATE_REGISTER *) &OrMask)->NbDid != ((NB_PSTATE_REGISTER *) &LocalPciRegister)->NbDid)) {
            AndMask = 0xFFFFFFFF;
            ((NB_PSTATE_REGISTER *) &AndMask)->NbFid = 0;
            ((NB_PSTATE_REGISTER *) &AndMask)->NbDid = 0;
            OptionMultiSocketConfiguration.ModifyCurrSocketPci (&PciAddress, AndMask, OrMask, StdHeader);
            PstateSettingsChanged = TRUE;
          }
        }
      } else {
        // At least one processor in the system does not have NbPstate[i]
        PciAddress.Address.Register = NB_PSTATE_CTRL;
        AndMask = 0xFFFFFFFF;
        ((NB_PSTATE_CTRL_REGISTER *) &AndMask)->NbPstateMaxVal = 0;
        OrMask = 0;
        if (i != 0) {
          ((NB_PSTATE_CTRL_REGISTER *) &OrMask)->NbPstateMaxVal = (i - 1);
        }
        // Modify NbPstateMaxVal to reflect the system value
        OptionMultiSocketConfiguration.ModifyCurrSocketPci (&PciAddress, AndMask, OrMask, StdHeader);

        // Disable this NB P-state
        PciAddress.Address.Register = (NB_PSTATE_0 + (4 * i));
        AndMask = 0xFFFFFFFF;
        ((NB_PSTATE_REGISTER *) &AndMask)->NbPstateEn = 0;
        OrMask = 0;
        OptionMultiSocketConfiguration.ModifyCurrSocketPci (&PciAddress, AndMask, OrMask, StdHeader);

        // Log error for the invalid configuration
        PutEventLog (AGESA_ERROR,
                     CPU_ERROR_PM_NB_PSTATE_MISMATCH,
                     Socket, i, 0, 0, StdHeader);
        break;
      }
    }
  }

  if (PstateSettingsChanged) {
    PciAddress.Address.Register = NB_PSTATE_CTRL;
    LibAmdPciRead (AccessWidth32, PciAddress, &NbPsCtrl, StdHeader);
    if (((NB_PSTATE_CTRL_REGISTER *) &NbPsCtrl)->NbPstateMaxVal == 0) {
      // Launch one core per node.
      TaskPtr.FuncAddress.PfApTask = F15OrPmNbCofVidInitOnCore;
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
  }
}

/*---------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * Support routine for F15OrPmNbCofVidInit to perform the actual NB P-state transition
 * to the leveled NB P-state settings on one core of each die in a family 15h socket.
 *
 * The following steps are performed:
 *   1. Save F5x170 and F5x1[6C:60] indexed by NB P-state 1
 *   2. Copy F5x1[6C:60] indexed by NB P-state 0 to F5x1[6C:60] indexed by NB P-state 1
 *   3, Write 1 to F5x170[NbPstateMaxVal, NbPstateLo]
 *   4. Write 0 to F5x170[SwNbPstateLoDis, NbPstateDisOnP0, NbPstateThreshold]
 *   5. Wait for F5x174[CurNbPstate] = F5x170[NbPstateLo] and F5x174[CurNbFid, CurNb-
 *      Did]=[NbFid, NbDid] from F5x1[6C:60] indexed by F5x170[NbPstateLo]
 *   6. Restore F5x170 and F5x1[6C:60] indexed by NB P-state 1
 *   7. Wait for F5x174[CurNbPstate] = F5x170[NbPstateHi]
 *
 * @param[in]  StdHeader          Config handle for library and services.
 *
 */
VOID
STATIC
F15OrPmNbCofVidInitOnCore (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32        NbPsCtrl;
  UINT32        NbPs0;
  UINT32        NbPs1;
  UINT32        LocalPciRegister;
  PCI_ADDR      PciAddress;

  OptionMultiSocketConfiguration.GetCurrPciAddr (&PciAddress, StdHeader);

  // Save F5x170 and F5x164
  PciAddress.Address.Function = FUNC_5;
  PciAddress.Address.Register = NB_PSTATE_CTRL;
  LibAmdPciRead (AccessWidth32, PciAddress, &NbPsCtrl, StdHeader);

  PciAddress.Address.Register = NB_PSTATE_0;
  LibAmdPciRead (AccessWidth32, PciAddress, &NbPs0, StdHeader);
  PciAddress.Address.Register = NB_PSTATE_1;
  LibAmdPciRead (AccessWidth32, PciAddress, &NbPs1, StdHeader);

  // Copy F5x160 to F5x164
  LibAmdPciWrite (AccessWidth32, PciAddress, &NbPs0, StdHeader);

  // Write 1 to F5x170[NbPstateMaxVal, NbPstateLo]
  PciAddress.Address.Register = NB_PSTATE_CTRL;
  LocalPciRegister = NbPsCtrl;
  ((NB_PSTATE_CTRL_REGISTER *) &LocalPciRegister)->NbPstateMaxVal = 1;
  ((NB_PSTATE_CTRL_REGISTER *) &LocalPciRegister)->NbPstateLo = 1;
  LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);

  // Write 0 to F5x170[SwNbPstateLoDis, NbPstateDisOnP0, NbPstateThreshold]
  ((NB_PSTATE_CTRL_REGISTER *) &LocalPciRegister)->SwNbPstateLoDis = 0;
  ((NB_PSTATE_CTRL_REGISTER *) &LocalPciRegister)->NbPstateDisOnP0 = 0;
  ((NB_PSTATE_CTRL_REGISTER *) &LocalPciRegister)->NbPstateThreshold = 0;
  LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);

  // Wait for F5x174[CurNbPstate] = F5x170[NbPstateLo] (written to 1 above) and
  // F5x174[CurNbFid, CurNbDid] = F5x164[NbFid, NbDid]
  PciAddress.Address.Register = NB_PSTATE_STATUS;
  do {
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
  } while ((((NB_PSTATE_STS_REGISTER *) &LocalPciRegister)->CurNbPstate != 1) &&
           (((NB_PSTATE_STS_REGISTER *) &LocalPciRegister)->CurNbFid != ((NB_PSTATE_REGISTER *) &NbPs0)->NbFid) &&
           (((NB_PSTATE_STS_REGISTER *) &LocalPciRegister)->CurNbDid != ((NB_PSTATE_REGISTER *) &NbPs0)->NbDid));

  // Restore F5x170 and F5x164
  PciAddress.Address.Register = NB_PSTATE_CTRL;
  LibAmdPciWrite (AccessWidth32, PciAddress, &NbPsCtrl, StdHeader);
  PciAddress.Address.Register = NB_PSTATE_1;
  LibAmdPciWrite (AccessWidth32, PciAddress, &NbPs1, StdHeader);

  // Wait for F5x174[CurNbPstate] = F5x170[NbPstateHi]
  PciAddress.Address.Register = NB_PSTATE_STATUS;
  do {
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
  } while (((NB_PSTATE_STS_REGISTER *) &LocalPciRegister)->CurNbPstate != ((NB_PSTATE_CTRL_REGISTER *) &NbPsCtrl)->NbPstateHi);
}
