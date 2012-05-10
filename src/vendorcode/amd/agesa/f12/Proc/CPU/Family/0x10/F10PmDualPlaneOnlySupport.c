/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_10 Dual-plane Only Support
 *
 * Performs the "BIOS Configuration for Dual-plane Only Support" as
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
#include "cpuApicUtilities.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "cpuFamilyTranslation.h"
#include "cpuF10PowerMgmt.h"
#include "F10PackageType.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_CPU_FAMILY_0X10_F10PMDUALPLANEONLYSUPPORT_FILECODE

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
UINT32
STATIC
SetPstateMSR (
  IN       VOID  *CPB,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * Family 10h core 0 entry point for performing the "Dual-plane Only Support" algorithm.
 *
 * The algorithm is as follows:
 *    // Determine whether algorithm applies to this processor
 *    if (CPUID Fn8000_0001_EBX[PkgType] == 0001b && (revision C or E) {
 *        // Determine whether processor is supported in this infrastructure
 *        if (((F3x1FC[DualPlaneOnly] == 1) && (this is a dual-plane platform))
 *            || ((F3x1FC[AM3r2Only] == 1) && (this is an AM3r2 platform))) {
 *            // Fixup the P-state MSRs
 *            for (each core in the system) {
 *            if (CPUID Fn8000_0007[CPB]) {
 *                Copy MSRC001_0065 as MinPstate;
 *                Copy MSRC001_0068 to MSRC001_0065;
 *                Copy MinPstate to MSRC001_0068;
 *            } else {
 *                Copy MSRC001_0068 to MSRC001_0064;
 *                Program MSRC001_0068 = 0;
 *            } // endif
 *            for (each MSR in MSRC001_00[68:64]) {
 *                if (value in MSRC001_00[68:64][IddValue] != 0) {
 *                    Set PstateEn in current MSR to 1;
 *                } // endif
 *            } // endfor
 *        } // endfor
 *        Set F3xDC[PstateMaxVal] = lowest-performance enabled P-state;
 *        Set F3xA8[PopDownPstate] = lowest-performance enabled P-state;
 *        Set F3x64[HtcPstateLimit] = lowest-performance enabled P-state;
 *        } // endif
 *    } // endif
 *
 * @param[in]  FamilySpecificServices  The current Family Specific Services.
 * @param[in]  CpuEarlyParamsPtr       Service related parameters (unused).
 * @param[in]  StdHeader               Config handle for library and services.
 *
 */
VOID
F10PmDualPlaneOnlySupport (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       AMD_CPU_EARLY_PARAMS  *CpuEarlyParamsPtr,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  AP_TASK                 TaskPtr;
  UINT32                  CPB;
  UINT32                  Core;
  UINT32                  Socket;
  UINT32                  Module;
  UINT32                  Pvimode;
  UINT32                  LowestPsEn;
  UINT32                  LocalPciRegister;
  UINT32                  ActiveCores;
  UINT32                  ProcessorPackageType;
  PCI_ADDR                PciAddress;
  CPUID_DATA              CpuidData;
  CPU_LOGICAL_ID          LogicalId;
  AGESA_STATUS            IgnoredSts;

  // get the local node ID
  IdentifyCore (StdHeader, &Socket, &Module, &Core, &IgnoredSts);
  GetPciAddress (StdHeader, Socket, Module, &PciAddress, &IgnoredSts);

  // get the package type
  ProcessorPackageType = LibAmdGetPackageType (StdHeader);
  GetLogicalIdOfCurrentCore (&LogicalId, StdHeader);
  if (((LogicalId.Revision & (AMD_F10_Cx | AMD_F10_Ex)) != 0) && ((ProcessorPackageType & PACKAGE_TYPE_AM2R2_AM3) != 0)) {
    PciAddress.AddressValue = PRCT_INFO_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    PciAddress.AddressValue = PW_CTL_MISC_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &Pvimode, StdHeader);
    if ((((LocalPciRegister & 0x80000000) != 0) && (((POWER_CTRL_MISC_REGISTER *) &Pvimode)->PviMode == 0))
      || ((LocalPciRegister & 0x04000000) != 0)) {
      CPB = 0;
      LibAmdCpuidRead (AMD_CPUID_APM, &CpuidData, StdHeader);
      if (((CpuidData.EDX_Reg & 0x00000200) >> 9) == 1) {
        CPB = 1;
      }

      TaskPtr.FuncAddress.PfApTaskIO = SetPstateMSR;
      TaskPtr.ExeFlags = TASK_HAS_OUTPUT | WAIT_FOR_CORE;
      TaskPtr.DataTransfer.DataTransferFlags = 0;
      TaskPtr.DataTransfer.DataSizeInDwords = 1;
      TaskPtr.DataTransfer.DataPtr = &CPB;

      GetActiveCoresInCurrentSocket (&ActiveCores, StdHeader);
      for (Core = 1; Core < (UINT8) ActiveCores; ++Core) {
        ApUtilRunCodeOnSocketCore ((UINT8)0, (UINT8)Core, &TaskPtr, StdHeader);
      }
      LowestPsEn = ApUtilTaskOnExecutingCore (&TaskPtr, StdHeader, (VOID *) CpuEarlyParamsPtr);

      PciAddress.AddressValue = CPTC2_PCI_ADDR;
      LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
      ((CLK_PWR_TIMING_CTRL2_REGISTER *) &LocalPciRegister)->PstateMaxVal = LowestPsEn;
      LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);

      PciAddress.AddressValue = POPUP_PSTATE_PCI_ADDR;
      LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
      ((POPUP_PSTATE_REGISTER *) &LocalPciRegister)->PopDownPstate = LowestPsEn;
      LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);

      PciAddress.AddressValue = HTC_PCI_ADDR;
      LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
      ((HTC_REGISTER *) &LocalPciRegister)->HtcPstateLimit = LowestPsEn;
      LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    }
  }
}
/*---------------------------------------------------------------------------------------*/
/**
 * Set P-State MSR.
 *
 * This function set the P-state MSRs per each core in the system.
 *
 * @param[in]  CPB               Contains the value of Asymmetric Boost register
 * @param[in]  StdHeader         Config handle for library and services
 *
 * @return         Return the lowest-performance enabled P-state
 */
UINT32
STATIC
SetPstateMSR (
  IN       VOID  *CPB,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 dtemp;
  UINT32 LowestPsEn;
  UINT64 MsrValue;
  UINT64 MinMsrValue;

  if (*(UINT32*) CPB != 0) {
    LibAmdMsrRead (MSR_PSTATE_1, &MinMsrValue, StdHeader);
    LibAmdMsrRead (MSR_PSTATE_4, &MsrValue, StdHeader);
    LibAmdMsrWrite (MSR_PSTATE_1, &MsrValue, StdHeader);
    LibAmdMsrWrite (MSR_PSTATE_4, &MinMsrValue, StdHeader);
  } else {
    LibAmdMsrRead (MSR_PSTATE_4, &MsrValue, StdHeader);
    LibAmdMsrWrite (MSR_PSTATE_0, &MsrValue, StdHeader);
    MsrValue = 0;
    LibAmdMsrWrite (MSR_PSTATE_4, &MsrValue, StdHeader);
  }

  LowestPsEn = 0;
  for (dtemp = MSR_PSTATE_0; dtemp <= MSR_PSTATE_4; dtemp++) {
    LibAmdMsrRead (dtemp, &MsrValue, StdHeader);
    if (((PSTATE_MSR *) &MsrValue)->IddValue != 0) {
      MsrValue = MsrValue | BIT63;
      LibAmdMsrWrite (dtemp, &MsrValue, StdHeader);
      LowestPsEn =  dtemp - MSR_PSTATE_0;
    }
  }
  return (LowestPsEn);
}

