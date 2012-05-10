/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_10 NB Pstate Initialization
 *
 * Performs the action described in F3x1F0[NbPstate] as
 * described in the BKDG.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/F10
 * @e \$Revision: 44324 $   @e \$Date: 2010-12-22 17:16:51 +0800 (Wed, 22 Dec 2010) $
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
#include "GeneralServices.h"
#include "cpuFamilyTranslation.h"
#include "F10PmNbPstateInit.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_CPU_FAMILY_0X10_F10PMNBPSTATEINIT_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */
/// Structure used for modifying the P-state
/// MSRs on fuse enable CPUs.
typedef struct {
  UINT8   NbVid1;             ///< Destination NB VID code
  UINT8   NbPstate;           ///< Status of NbVidUpdateAll
} NB_PSTATE_INIT;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
STATIC
PmNbPstateInitCore (
  IN       VOID *NbPstateParams,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * Family 10h core 0 entry point for performing the actions described in the
 * description of F3x1F0[NbPstate].
 *
 * If F3x1F0[NbPstate] is non zero, it specifies the highest performance
 * P-state in which to enable NbDid.  Each core must loop through their
 * P-state MSRs, enabling NbDid and changing NbVid to a lower voltage.
 *
 * @param[in]  FamilySpecificServices  The current Family Specific Services.
 * @param[in]  CpuEarlyParamsPtr       Service related parameters (unused).
 * @param[in]  StdHeader               Config handle for library and services.
 *
 */
VOID
F10PmNbPstateInit (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       AMD_CPU_EARLY_PARAMS  *CpuEarlyParamsPtr,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINT32         Core;
  UINT32         Module;
  UINT32         LocalPciRegister;
  UINT32         Socket;
  AP_TASK        TaskPtr;
  PCI_ADDR       PciAddress;
  AGESA_STATUS   IgnoredSts;
  NB_PSTATE_INIT ApParams;

  if (FamilySpecificServices->IsNbPstateEnabled (FamilySpecificServices, &CpuEarlyParamsPtr->PlatformConfig, StdHeader)) {
    if (CpuEarlyParamsPtr->PlatformConfig.PlatformProfile.PlatformPowerPolicy == BatteryLife) {
      IdentifyCore (StdHeader, &Socket, &Module, &Core, &IgnoredSts);
      ASSERT (Core == 0);
      GetPciAddress (StdHeader, Socket, Module, &PciAddress, &IgnoredSts);
      PciAddress.Address.Function = FUNC_3;
      PciAddress.Address.Register = 0x1F0;
      LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
      if ((LocalPciRegister & 0x00070000) != 0) {
        ApParams.NbPstate = (UINT8) ((LocalPciRegister & 0x00070000) >> 16);
        ASSERT (ApParams.NbPstate < NM_PS_REG);

        PciAddress.Address.Function = FUNC_4;
        PciAddress.Address.Register = 0x1F4;
        LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
        ApParams.NbVid1 = (UINT8) ((LocalPciRegister & 0x00003F80) >> 7);

        TaskPtr.FuncAddress.PfApTaskI = PmNbPstateInitCore;
        TaskPtr.DataTransfer.DataSizeInDwords = SIZE_IN_DWORDS (NB_PSTATE_INIT);
        TaskPtr.DataTransfer.DataPtr = &ApParams;
        TaskPtr.DataTransfer.DataTransferFlags = 0;
        TaskPtr.ExeFlags = WAIT_FOR_CORE;
        ApUtilRunCodeOnAllLocalCoresAtEarly (&TaskPtr, StdHeader, CpuEarlyParamsPtr);

      }
    }
  }
}


/*---------------------------------------------------------------------------------------*/
/**
 * Support routine for F10PmNbPstateInit.
 *
 * This function modifies NbVid and NbDid on each core.
 *
 * @param[in]  NbPstateParams     Appropriate NbVid1 and NbPstate as determined by core 0.
 * @param[in]  StdHeader          Config handle for library and services.
 *
 */
VOID
STATIC
PmNbPstateInitCore (
  IN       VOID *NbPstateParams,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 MsrAddress;
  UINT64 LocalMsrRegister;

  for (MsrAddress = (PS_REG_BASE + ((NB_PSTATE_INIT *) NbPstateParams)->NbPstate); MsrAddress <= PS_MAX_REG; MsrAddress++) {
    LibAmdMsrRead (MsrAddress, &LocalMsrRegister, StdHeader);
    if (((PSTATE_MSR *) &LocalMsrRegister)->PsEnable == 1) {
      ((PSTATE_MSR *) &LocalMsrRegister)->NbDid = 1;
      ((PSTATE_MSR *) &LocalMsrRegister)->NbVid = ((NB_PSTATE_INIT *) NbPstateParams)->NbVid1;
      LibAmdMsrWrite (MsrAddress, &LocalMsrRegister, StdHeader);
    }
  }
}

