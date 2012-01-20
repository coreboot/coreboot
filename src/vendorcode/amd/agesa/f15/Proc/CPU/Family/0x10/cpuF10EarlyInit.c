/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_10 after warm reset sequence
 *
 * Performs the "CPU Core Minimum P-State Transition Sequence After Warm Reset"
 * as described in the BKDG.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x10
 * @e \$Revision: 56279 $   @e \$Date: 2011-07-11 13:11:28 -0600 (Mon, 11 Jul 2011) $
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
#include "cpuF10PowerMgmt.h"
#include "cpuRegisters.h"
#include "cpuApicUtilities.h"
#include "cpuFamilyTranslation.h"
#include "cpuF10Utilities.h"
#include "cpuF10EarlyInit.h"
#include "GeneralServices.h"
#include "cpuServices.h"
#include "OptionMultiSocket.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_CPU_FAMILY_0X10_CPUF10EARLYINIT_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */
/// Enum for handling code branching while transitioning to the
/// minimum P-state after a warm reset
typedef enum {
  EXIT_SEQUENCE,        ///< Exit the sequence
  STEP7,                ///< Go to step 7
  STEP17,               ///< Go to step 17
  STEP20                ///< Go to step 20
} GO_TO_STEP;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
STATIC
F10PmVoltageAlignmentAfterResetCore (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
STATIC
F10PmAfterResetCore (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
STATIC
WaitForCpuFidAndDidToMatch (
  IN       UINT32             PstateNumber,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
extern OPTION_MULTISOCKET_CONFIGURATION OptionMultiSocketConfiguration;
/*---------------------------------------------------------------------------------------*/
/**
 * Family 10h core 0 entry point for performing the necessary steps after
 * a warm reset has occurred.
 *
 * The steps are as follows:
 *    1. Modify F3xDC[PstateMaxVal] to reflect the lowest performance P-state
 *       supported, as indicated in MSRC001_00[68:64][PstateEn]
 *    2. If MSRC001_0071[CurNbDid] = 0, set MSRC001_001F[GfxNbPstateDis]
 *    3. If MSRC001_0071[CurPstate] != F3xDC[PstateMaxVal], go to step 20
 *    4. If F3xDC[PstateMaxVal] = 0 or F3xDC[PstateMaxVal] != 4, go to step 7
 *    5. If MSRC001_0061[CurPstateLimit] <= F3xDC[PstateMaxVal]-1, go to step 17
 *    6. Exit the sequence
 *    7. Copy the P-state register pointed to by F3xDC[PstateMaxVal] to the P-state
 *       register pointed to by F3xDC[PstateMaxVal]+1
 *    8. Write F3xDC[PstateMaxVal]+1 to F3xDC[PstateMaxVal]
 *    9. Write (the new) F3xDC[PstateMaxVal] to MSRC001_0062[PstateCmd]
 *   10. Wait for MSRC001_0071[CurCpuFid/CurCpuDid] = CpuFid/CpuDid from the P-state
 *       register pointed to by (the new) F3xDC[PstateMaxVal]
 *   11. Copy (the new) F3xDC[PstateMaxVal]-1 to MSRC001_0062[PstateCmd]
 *   12. Wait for MSRC001_0071[CurCpuFid/CurCpuDid] = CpuFid/CpuDid from the P-state
 *       register pointed to by (the new) F3xDC[PstateMaxVal]-1
 *   13. If MSRC001_0071[CurNbDid] = 1, set MSRC001_001F[GfxNbPstateDis]
 *   14. If required, transition the NB COF and VID to the NbDid and NbVid from the
 *       P-state register pointed to by MSRC001_0061[CurPstateLimit] using the NB COF
 *       and VID transition sequence after a warm reset
 *   15. Write MSRC001_00[68:64][PstateEn]=0 for the P-state pointed to by F3xDC[PstateMaxVal]
 *   16. Write (the new) F3xDC[PstateMaxVal]-1 to F3xDC[PstateMaxVal] and exit the sequence
 *   17. Copy MSRC001_0061[PstateMaxVal] - 1 to MSRC001_0062[PstateCmd]
 *   18. Wait for MSRC001_0071[CurCpuFid/CurCpuDid] = CpuFid/CpuDid from the P-state
 *       register pointed to by F3xDC[PstateMaxVal]-1
 *   19. If MSRC001_0071[CurNbDid] = 0, set MSRC001_001F[GfxNbPstateDis]
 *   20. Copy MSRC001_0061[PstateMaxVal] to MSRC001_0062[PstateCmd]
 *   21. Wait for MSRC001_0071[CurCpuFid/CurCpuDid] = CpuFid/CpuDid from the P-state
 *       register pointed to by F3xDC[PstateMaxVal]
 *   22. If MSRC001_0071[CurNbDid] = 1, set MSRC001_001F[GfxNbPstateDis]
 *   23. Issue an LDTSTOP assertion in the IO hub and exit sequence
 *   24. If required, transition the NB COF and VID to the NbDid and NbVid from the
 *       P-state register pointed to by F3xDC[PstateMaxVal] using the NB COF and VID
 *       transition sequence after a warm reset
 *
 * @param[in]  FamilySpecificServices  The current Family Specific Services.
 * @param[in]  CpuEarlyParamsPtr       Service parameters
 * @param[in]  StdHeader               Config handle for library and services.
 *
 */
VOID
F10PmAfterReset (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       AMD_CPU_EARLY_PARAMS  *CpuEarlyParamsPtr,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINT32    PsMaxVal;
  UINT32    MsrAddr;
  UINT32    Core;
  UINT32    AndMask;
  UINT32    OrMask;
  UINT32    CpbNum;
  UINT64    LocalMsrRegister;
  UINT64    CurrentStatus;
  UINT64    TargetPsMsr;
  PCI_ADDR  PciAddress;
  AP_TASK   TaskPtr;

  GetCurrentCore (&Core, StdHeader);
  ASSERT (Core == 0);

  // Core P-State Voltage Alignment After Warm Reset
  CpbNum = F10GetNumberOfBoostedPstatesOnCore (StdHeader);
  if (CpbNum == 1) {
    // Step 1 Write MSRC001_0063[ CurPstate] to MSRC001_0062[ PstateCmd] on every core in the processor.
    TaskPtr.FuncAddress.PfApTask = F10PmVoltageAlignmentAfterResetCore;
    TaskPtr.DataTransfer.DataSizeInDwords = 0;
    TaskPtr.ExeFlags = WAIT_FOR_CORE;
    ApUtilRunCodeOnAllLocalCoresAtEarly (&TaskPtr, StdHeader, CpuEarlyParamsPtr);

    // Step 2 Wait for MSRC001_0071[ CurCpuVid] >= [CpuVid] from MSRC001_00[ 68:64] indexed by 4x15C[ NumBoost States]
    // Get target P-state indexed by F4x15C[NumBoostStates]
    LibAmdMsrRead ((MSR_PSTATE_0 + CpbNum), &TargetPsMsr, StdHeader);
    do {
      LibAmdMsrRead (MSR_COFVID_STS, &CurrentStatus, StdHeader);
    } while (((COFVID_STS_MSR *) &CurrentStatus)->CurCpuVid < ((PSTATE_MSR *) &TargetPsMsr)->CpuVid);
  }

  // Core Minimum P-State Transition Sequence After Warm Reset
  // Step 1 Modify F3xDC[PstateMaxVal] to reflect the lowest performance
  //        P-state supported, as indicated in MSRC001_00[68:64][PstateEn]
  for (MsrAddr = PS_MAX_REG; MsrAddr > PS_REG_BASE; --MsrAddr) {
    LibAmdMsrRead (MsrAddr, &LocalMsrRegister, StdHeader);
    if (((PSTATE_MSR *) &LocalMsrRegister)->PsEnable == 1) {
      break;
    }
  }
  PsMaxVal = MsrAddr - PS_REG_BASE;
  PciAddress.Address.Function = FUNC_3;
  PciAddress.Address.Register = CPTC2_REG;
  AndMask = 0xFFFFFFFF;
  OrMask = 0x00000000;
  ((CLK_PWR_TIMING_CTRL2_REGISTER *) &AndMask)->PstateMaxVal = 0;
  ((CLK_PWR_TIMING_CTRL2_REGISTER *) &OrMask)->PstateMaxVal = PsMaxVal;
  OptionMultiSocketConfiguration.ModifyCurrSocketPci (&PciAddress, AndMask, OrMask, StdHeader);

  // Launch each local core to perform the remaining steps.
  TaskPtr.FuncAddress.PfApTask = F10PmAfterResetCore;
  TaskPtr.DataTransfer.DataSizeInDwords = 0;
  TaskPtr.ExeFlags = WAIT_FOR_CORE;
  ApUtilRunCodeOnAllLocalCoresAtEarly (&TaskPtr, StdHeader, CpuEarlyParamsPtr);
}


/*---------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * Support routine for F10PmAfterReset to perform MSR initialization on all
 * cores of a family 10h socket.
 *
 * This function implements steps 2 - 24 on each core.
 *
 * @param[in]  StdHeader          Config handle for library and services.
 *
 */
VOID
STATIC
F10PmAfterResetCore (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 PsMaxVal;
  UINT32 SwPsMaxVal;
  UINT32 LocalPciRegister;
  UINT64 LocalMsrRegister;
  UINT64 SavedMsr;
  UINT64 CurrentLimitMsr;
  PCI_ADDR PciAddress;
  GO_TO_STEP GoToStep;
  CPU_LOGICAL_ID LogicalId;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;

  // Step 2 If MSR C001_0071[CurNbDid] = 0, set MSR C001_001F[GfxNbPstateDis]
  GetLogicalIdOfCurrentCore (&LogicalId, StdHeader);
  GetCpuServicesFromLogicalId (&LogicalId, (const CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
  if ((LogicalId.Revision & (AMD_F10_C3 | AMD_F10_DA_C2)) != 0) {
    LibAmdMsrRead (MSR_COFVID_STS, &LocalMsrRegister, StdHeader);
    if (((COFVID_STS_MSR *) &LocalMsrRegister)->CurNbDid == 0) {
      LibAmdMsrRead (NB_CFG, &LocalMsrRegister, StdHeader);
      LocalMsrRegister |= BIT62;
      LibAmdMsrWrite (NB_CFG, &LocalMsrRegister, StdHeader);
    }
  }

  GoToStep = EXIT_SEQUENCE;

  OptionMultiSocketConfiguration.GetCurrPciAddr (&PciAddress, StdHeader);
  PciAddress.Address.Function = FUNC_3;
  PciAddress.Address.Register = CPTC2_REG;
  LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
  PsMaxVal = ((CLK_PWR_TIMING_CTRL2_REGISTER *) &LocalPciRegister)->PstateMaxVal;

  LibAmdMsrRead (MSR_PSTATE_CURRENT_LIMIT, &CurrentLimitMsr, StdHeader);
  SwPsMaxVal = (UINT32) (((PSTATE_CURLIM_MSR *) &CurrentLimitMsr)->PstateMaxVal);

  // Step 3 If MSRC001_0071[CurPstate] != F3xDC[PstateMaxVal], go to step 20
  LibAmdMsrRead (MSR_COFVID_STS, &LocalMsrRegister, StdHeader);
  if (((COFVID_STS_MSR *) &LocalMsrRegister)->CurPstate != PsMaxVal) {
    GoToStep = STEP20;
  } else {
    // Step 4 If F3xDC[PstateMaxVal] = 0 || F3xDC[PstateMaxVal] != 4, go to step 7
    if ((PsMaxVal == 0) || (PsMaxVal != 4)) {
      GoToStep = STEP7;
    } else {
      // Step 5 If MSRC001_0061[CurPstateLimit] <= F3xDC[PstateMaxVal]-1, go to step 17
      if (((PSTATE_CURLIM_MSR *) &CurrentLimitMsr)->CurPstateLimit <= (PsMaxVal - 1)) {
        GoToStep = STEP17;
      }
    }
  }
  switch (GoToStep) {
  default:
  case EXIT_SEQUENCE:
    // Step 6 Exit the sequence
    break;
  case STEP7:
    // Workaround for S3 ----Save the value of [The PState[4:0] Registers] MSRC001_00[68:64]
    //                      pointed to by F3xDC[PstateMaxVal] + 1
    LibAmdMsrRead ((MSR_PSTATE_0 + (PsMaxVal + 1)), &SavedMsr, StdHeader);

    // Step 7 Copy the P-state register pointed to by F3xDC[PstateMaxVal] to the P-state
    //        register pointed to by F3xDC[PstateMaxVal]+1
    LibAmdMsrRead ((MSR_PSTATE_0 + PsMaxVal), &LocalMsrRegister, StdHeader);
    LibAmdMsrWrite ((MSR_PSTATE_0 + (PsMaxVal + 1)), &LocalMsrRegister, StdHeader);

    // Step 8 Write F3xDC[PstateMaxVal]+1 to F3xDC[PstateMaxVal]
    PciAddress.Address.Function = FUNC_3;
    PciAddress.Address.Register = CPTC2_REG;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    ((CLK_PWR_TIMING_CTRL2_REGISTER *) &LocalPciRegister)->PstateMaxVal = PsMaxVal + 1;
    LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);

    // Step 9 Write (the new) F3xDC[PstateMaxVal] to MSRC001_0062[PstateCmd]
    FamilySpecificServices->TransitionPstate (FamilySpecificServices, (UINT8) (PsMaxVal + 1), (BOOLEAN) FALSE, StdHeader);

    // Step 10 Wait for MSRC001_0071[CurCpuFid/CurCpuDid] = CpuFid/CpuDid from the P-state
    //         register pointed to by (the new) F3xDC[PstateMaxVal]
    WaitForCpuFidAndDidToMatch ((UINT32) (PsMaxVal + 1), StdHeader);

    // Step 11 Copy (the new) F3xDC[PstateMaxVal]-1 to MSRC001_0062[PstateCmd]
    FamilySpecificServices->TransitionPstate (FamilySpecificServices, (UINT8) PsMaxVal, (BOOLEAN) FALSE, StdHeader);

    // Step 12 Wait for MSRC001_0071[CurCpuFid/CurCpuDid] = CpuFid/CpuDid from the P-state
    //         register pointed to by (the new) F3xDC[PstateMaxVal]-1
    WaitForCpuFidAndDidToMatch (PsMaxVal, StdHeader);

    // Step 13 If MSRC001_0071[CurNbDid] = 1, set MSRC001_001F[GfxNbPstateDis]
    if ((LogicalId.Revision & (AMD_F10_C3 | AMD_F10_DA_C2)) != 0) {
      LibAmdMsrRead (MSR_COFVID_STS, &LocalMsrRegister, StdHeader);
      if (((COFVID_STS_MSR *) &LocalMsrRegister)->CurNbDid == 1) {
        LibAmdMsrRead (NB_CFG, &LocalMsrRegister, StdHeader);
        LocalMsrRegister |= BIT62;
        LibAmdMsrWrite (NB_CFG, &LocalMsrRegister, StdHeader);
      }
    }

    // Step 14 If required, transition the NB COF and VID to the NbDid and NbVid from the
    //         P-state register pointed to by MSRC001_0061[CurPstateLimit] using the NB COF
    //         and VID transition sequence after a warm reset

    // Step 15 Write 0 to PstateEn of the P-state register pointed to by (the new) F3xDC[PstateMaxVal]
    // Workaround for S3----Restore the value of [The PState[4:0] Registers] MSRC001_00[68:64]
    //                   pointed to by F3xDC[PstateMaxVal] + 1
    ((PSTATE_MSR *) &SavedMsr)->PsEnable = 0;
    LibAmdMsrWrite ((MSR_PSTATE_0 + (PsMaxVal + 1)), &SavedMsr, StdHeader);

    // Step 16 Write (the new) F3xDC[PstateMaxVal]-1 to F3xDC[PstateMaxVal]
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    ((CLK_PWR_TIMING_CTRL2_REGISTER *) &LocalPciRegister)->PstateMaxVal = PsMaxVal;
    LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    break;
  case STEP17:
    // Step 17 Copy MSRC001_0061[PstateMaxVal]-1 to MSRC001_0062[PstateCmd]
    FamilySpecificServices->TransitionPstate (FamilySpecificServices, (UINT8) (SwPsMaxVal - 1), (BOOLEAN) FALSE, StdHeader);

    // Step 18 Wait for MSRC001_0071[CurCpuFid/CurCpuDid] = CpuFid/CpuDid from the P-state
    //         register pointed to by F3xDC[PstateMaxVal]-1
    WaitForCpuFidAndDidToMatch ((UINT32) (PsMaxVal - 1), StdHeader);

    // Step 19 If MSR C001_0071[CurNbDid] = 0, set MSR C001_001F[GfxNbPstateDis]
    if ((LogicalId.Revision & (AMD_F10_C3 | AMD_F10_DA_C2)) != 0) {
      LibAmdMsrRead (MSR_COFVID_STS, &LocalMsrRegister, StdHeader);
      if (((COFVID_STS_MSR *) &LocalMsrRegister)->CurNbDid == 0) {
        LibAmdMsrRead (NB_CFG, &LocalMsrRegister, StdHeader);
        LocalMsrRegister |= BIT62;
        LibAmdMsrWrite (NB_CFG, &LocalMsrRegister, StdHeader);
      }
    }

    // Fall through from step 19 to step 20
  case STEP20:
    // Step 20 Copy MSRC001_0061[PstateMaxVal] to MSRC001_0062[PstateCmd]
    FamilySpecificServices->TransitionPstate (FamilySpecificServices, (UINT8) SwPsMaxVal, (BOOLEAN) FALSE, StdHeader);

    // Step 21 Wait for MSRC001_0071[CurCpuFid/CurCpuDid] = CpuFid/CpuDid from the P-state
    //         register pointed to by F3xDC[PstateMaxVal]
    WaitForCpuFidAndDidToMatch (PsMaxVal, StdHeader);

    // Step 22 If MSR C001_0071[CurNbDid] = 1, set MSR C001_001F[GfxNbPstateDis] and exit
    //         the sequence
    if ((LogicalId.Revision & (AMD_F10_C3 | AMD_F10_DA_C2)) != 0) {
      LibAmdMsrRead (MSR_COFVID_STS, &LocalMsrRegister, StdHeader);
      if (((COFVID_STS_MSR *) &LocalMsrRegister)->CurNbDid == 1) {
        LibAmdMsrRead (NB_CFG, &LocalMsrRegister, StdHeader);
        LocalMsrRegister |= BIT62;
        LibAmdMsrWrite (NB_CFG, &LocalMsrRegister, StdHeader);
        break;
      }
    }

    // Step 23 Issue an LDTSTOP and exit the sequence

    // Step 24 If required, transition the NB COF and VID to the NbDid and NbVid from the
    //         P-state register pointed to by F3xDC[PstateMaxVal] using the NB COF and VID
    //         transition sequence after a warm reset
    break;
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Support routine for F10PmAfterResetCore to wait for Cpu FID and DID to
 * match a specific P-state.
 *
 * This function implements steps 11, 13, 18, and 20 on each core as needed.
 *
 * @param[in]  PstateNumber       P-state settings to match
 * @param[in]  StdHeader          Config handle for library and services.
 *
 */
VOID
STATIC
WaitForCpuFidAndDidToMatch (
  IN       UINT32             PstateNumber,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT64  TargetPsMsr;
  UINT64  CurrentStatus;

  // Get target P-state settings
  LibAmdMsrRead ((MSR_PSTATE_0 + PstateNumber), &TargetPsMsr, StdHeader);

  // Wait for current CPU FID/DID to match target FID/DID
  do {
    LibAmdMsrRead (MSR_COFVID_STS, &CurrentStatus, StdHeader);
  } while ((((COFVID_STS_MSR *) &CurrentStatus)->CurCpuFid != ((PSTATE_MSR *) &TargetPsMsr)->CpuFid) ||
           (((COFVID_STS_MSR *) &CurrentStatus)->CurCpuDid != ((PSTATE_MSR *) &TargetPsMsr)->CpuDid));
}

/*---------------------------------------------------------------------------------------*/
/**
 * Support routine for F10PmAfterReset to Core P-State Voltage Alignment for CPB on all
 * cores of a family 10h socket.
 *
 * @param[in]  StdHeader          Config handle for library and services.
 *
 */
VOID
STATIC
F10PmVoltageAlignmentAfterResetCore (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64 CurrentStatus;

  LibAmdMsrRead (MSR_PSTATE_STS, &CurrentStatus, StdHeader);
  LibAmdMsrWrite (MSR_PSTATE_CTL, &CurrentStatus, StdHeader);
}

