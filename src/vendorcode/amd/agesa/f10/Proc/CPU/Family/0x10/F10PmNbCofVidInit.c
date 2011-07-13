/**
 * @file
 *
 * AMD Family_10 NB COF VID Initialization
 *
 * Performs the "BIOS Northbridge COF and VID Configuration" as
 * described in the BKDG.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/F10
 * @e \$Revision: 6524 $   @e \$Date: 2008-06-24 17:00:51 -0500 (Tue, 24 Jun 2008) $
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
#include "cpuF10PowerMgmt.h"
#include "cpuApicUtilities.h"
#include "OptionMultiSocket.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "cpuFamilyTranslation.h"
#include "cpuF10Utilities.h"
#include "cpuCommonF10Utilities.h"
#include "F10PmNbCofVidInit.h"
#include "Filecode.h"
#define FILECODE PROC_CPU_FAMILY_0X10_F10PMNBCOFVIDINIT_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */
/// Structure used for performing the steps outlined in
/// the NB COFVID configuration sequence
typedef struct {
  UINT8   NewNbVid;           ///< Destination NB VID code
  BOOLEAN NbVidUpdateAll;     ///< Status of NbVidUpdateAll
} NB_COF_VID_INIT_WARM;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
STATIC
PmNbCofVidInitP0P1Core (
  IN       VOID *NewNbVid,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
STATIC
PmNbCofVidInitWarmCore (
  IN       VOID *FunctionData,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
extern OPTION_MULTISOCKET_CONFIGURATION OptionMultiSocketConfiguration;

/*---------------------------------------------------------------------------------------*/
/**
 * Family 10h core 0 entry point for performing the "Northbridge COF and
 * VID Configuration" algorithm.
 *
 * The steps are as follows:
 *    1. Determine if the algorithm is necessary by checking if all NB FIDs
 *       match in the coherent fabric.  If so, check to see if NbCofVidUpdate
 *       is zero for all CPUs.  If that is also true, no further steps are
 *       necessary.  If not + cold reset, proceed to step 2.  If not + warm
 *       reset, proceed to step 8.
 *    2. Determine NewNbVid & NewNbFid.
 *    3. Copy Startup Pstate settings to P0/P1 MSRs on all local cores.
 *    4. Copy NewNbVid to P0 NbVid on all local cores.
 *    5. Transition to P1 on all local cores.
 *    6. Transition to P0 on local core 0 only.
 *    7. Copy NewNbFid to F3xD4[NbFid], set NbFidEn, and issue a warm reset.
 *    8. Update all enabled Pstate MSRs' NbVids according to NbVidUpdateAll
 *       on all local cores.
 *    9. Transition to Startup Pstate on all local cores.
 *
 * @param[in]  FamilySpecificServices  The current Family Specific Services.
 * @param[in]  CpuEarlyParamsPtr       Service related parameters (unused).
 * @param[in]  StdHeader               Config handle for library and services.
 *
 */
VOID
F10PmNbCofVidInit (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       AMD_CPU_EARLY_PARAMS  *CpuEarlyParamsPtr,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  BOOLEAN   PerformNbCofVidCfg;
  BOOLEAN   SystemNbCofsMatch;
  UINT8     NewNbFid;
  UINT8     NewNbVid;
  UINT32    Socket;
  UINT32    Module;
  UINT32    Core;
  UINT32    SystemNbCof;
  UINT32    AndMask;
  UINT32    OrMask;
  UINT32    Ignored;
  UINT32    NewNbVoltage;
  WARM_RESET_REQUEST Request;
  AP_TASK   TaskPtr;
  PCI_ADDR  PciAddress;
  AGESA_STATUS IgnoredSts;
  NB_COF_VID_INIT_WARM FunctionData;

  PerformNbCofVidCfg = TRUE;
  OptionMultiSocketConfiguration.GetSystemNbCof (&SystemNbCof, &SystemNbCofsMatch, StdHeader);
  if (SystemNbCofsMatch) {
    if (!OptionMultiSocketConfiguration.GetSystemNbCofVidUpdate (StdHeader)) {
      PerformNbCofVidCfg = FALSE;
    }
  }
  if (PerformNbCofVidCfg) {
    // get the local node ID
    IdentifyCore (StdHeader, &Socket, &Module, &Core, &IgnoredSts);
    GetPciAddress (StdHeader, Socket, Module, &PciAddress, &IgnoredSts);

    ASSERT (Core == 0);

    // get NewNbVid
    FamilySpecificServices->GetNbFrequency (FamilySpecificServices, &PciAddress, &Ignored, &NewNbVoltage, StdHeader);
    ASSERT (((1550000 - NewNbVoltage) % 12500) == 0);
    NewNbVid = (UINT8) ((1550000 - NewNbVoltage) / 12500);
    ASSERT (NewNbVid < 0x80);

    if (!(IsWarmReset (StdHeader))) {

      // determine NewNbFid
      NewNbFid = (UINT8) ((SystemNbCof / 200) - 4);

      TaskPtr.FuncAddress.PfApTaskI = PmNbCofVidInitP0P1Core;
      TaskPtr.DataTransfer.DataSizeInDwords = 1;
      TaskPtr.DataTransfer.DataPtr = &NewNbVid;
      TaskPtr.DataTransfer.DataTransferFlags = 0;
      TaskPtr.ExeFlags = 0;
      ApUtilRunCodeOnAllLocalCoresAtEarly (&TaskPtr, StdHeader, CpuEarlyParamsPtr);

      // Transition core 0 to P0 and wait for change to complete
      FamilySpecificServices->TransitionPstate (FamilySpecificServices, (UINT8) 0, (BOOLEAN) TRUE, StdHeader);

      PciAddress.Address.Register = CPTC0_REG;
      AndMask = 0xFFFFFFFF;
      ((CLK_PWR_TIMING_CTRL_REGISTER *) &AndMask)->NbFid = 0;
      OrMask = 0x00000000;
      ((CLK_PWR_TIMING_CTRL_REGISTER *) &OrMask)->NbFid = NewNbFid;
      ((CLK_PWR_TIMING_CTRL_REGISTER *) &OrMask)->NbFidEn = 1;
      ModifyCurrentSocketPci (&PciAddress, AndMask, OrMask, StdHeader);

      // warm reset request
      FamilySpecificServices->GetWarmResetFlag (FamilySpecificServices, StdHeader, &Request);
      Request.RequestBit = TRUE;
      FamilySpecificServices->SetWarmResetFlag (FamilySpecificServices, StdHeader, &Request);
    } else {
      // warm reset path

      FunctionData.NewNbVid = NewNbVid;
      FamilySpecificServices->IsNbCofInitNeeded (FamilySpecificServices, &PciAddress, &FunctionData.NbVidUpdateAll, StdHeader);

      TaskPtr.FuncAddress.PfApTaskI = PmNbCofVidInitWarmCore;
      TaskPtr.DataTransfer.DataSizeInDwords = SIZE_IN_DWORDS (NB_COF_VID_INIT_WARM);
      TaskPtr.DataTransfer.DataPtr = &FunctionData;
      TaskPtr.DataTransfer.DataTransferFlags = 0;
      TaskPtr.ExeFlags = WAIT_FOR_CORE;
      ApUtilRunCodeOnAllLocalCoresAtEarly (&TaskPtr, StdHeader, CpuEarlyParamsPtr);
    }
  } // skip whole algorithm
}


/*---------------------------------------------------------------------------------------*/
/**
 * Cold reset support routine for F10PmNbCofVidInit.
 *
 * This function implements steps 3, 4, & 5 on each core.
 *
 * @param[in]  NewNbVid           NewNbVid determined by core 0 in step 2.
 * @param[in]  StdHeader          Config handle for library and services.
 *
 */
VOID
STATIC
PmNbCofVidInitP0P1Core (
  IN       VOID *NewNbVid,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 MsrAddress;
  UINT64 MsrRegister;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices = NULL;

  GetCpuServicesOfCurrentCore (&FamilySpecificServices, StdHeader);
  LibAmdMsrRead (MSR_COFVID_STS, &MsrRegister, StdHeader);
  MsrAddress = (UINT32) ((((COFVID_STS_MSR *) &MsrRegister)->StartupPstate) + PS_REG_BASE);
  LibAmdMsrRead (MsrAddress, &MsrRegister, StdHeader);
  LibAmdMsrWrite ((UINT32) (PS_REG_BASE + 1), &MsrRegister, StdHeader);
  ((PSTATE_MSR *) &MsrRegister)->NbVid = *(UINT8 *) NewNbVid;
  LibAmdMsrWrite (PS_REG_BASE, &MsrRegister, StdHeader);
  FamilySpecificServices->TransitionPstate (FamilySpecificServices, (UINT8) 1, (BOOLEAN) FALSE, StdHeader);
}


/*---------------------------------------------------------------------------------------*/
/**
 * Warm reset support routine for F10PmNbCofVidInit.
 *
 * This function implements steps 8 & 9 on each core.
 *
 * @param[in]  FunctionData       Contains NewNbVid determined by core 0 in step
 *                                2, and NbVidUpdateAll.
 * @param[in]  StdHeader          Config handle for library and services.
 *
 */
VOID
STATIC
PmNbCofVidInitWarmCore (
  IN       VOID *FunctionData,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 MsrAddress;
  UINT64 MsrRegister;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;

  GetCpuServicesOfCurrentCore (&FamilySpecificServices, StdHeader);
  for (MsrAddress = PS_REG_BASE; MsrAddress <= PS_MAX_REG; MsrAddress++) {
    LibAmdMsrRead (MsrAddress, &MsrRegister, StdHeader);
    if (((PSTATE_MSR *) &MsrRegister)->PsEnable == 1) {
      if ((((PSTATE_MSR *) &MsrRegister)->NbDid == 0) || ((NB_COF_VID_INIT_WARM *) FunctionData)->NbVidUpdateAll) {
        ((PSTATE_MSR *) &MsrRegister)->NbVid = ((NB_COF_VID_INIT_WARM *) FunctionData)->NewNbVid;
        LibAmdMsrWrite (MsrAddress, &MsrRegister, StdHeader);
      }
    }
  }
  LibAmdMsrRead (MSR_COFVID_STS, &MsrRegister, StdHeader);
  FamilySpecificServices->TransitionPstate (FamilySpecificServices, (UINT8) (((COFVID_STS_MSR *) &MsrRegister)->StartupPstate), (BOOLEAN) FALSE, StdHeader);
}

