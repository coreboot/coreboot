/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_10 Asymmetric Boost Initialization
 *
 * Performs the "BIOS Configuration for Asymmetric Boost" as
 * described in the BKDG.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/F10
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
#include "cpuRegisters.h"
#include "cpuApicUtilities.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "cpuFamilyTranslation.h"
#include "cpuF10PowerMgmt.h"
#include "F10PmAsymBoostInit.h"
#include "OptionMultiSocket.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_CPU_FAMILY_0X10_F10PMASYMBOOSTINIT_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern OPTION_MULTISOCKET_CONFIGURATION OptionMultiSocketConfiguration;
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
SetAsymBoost (
  IN       VOID *AsymBoostRegister,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * Family 10h core 0 entry point for performing the "Asymmetric Boost
 * Configuration" algorithm.
 *
 * The algorithm is as follows:
 *    // Determine whether the processor support boost
 *    if (CPUID CPUID Fn8000_0007[CPB]==1)&& CPUID Fn8000_0008[NC]==5) {
 *        Core0 MSRC001_0064[CpuFid] += F3x10C[AsymmetricBoostCore0]
 *        Core1 MSRC001_0064[CpuFid] += F3x10C[AsymmetricBoostCore1]
 *        Core2 MSRC001_0064[CpuFid] += F3x10C[AsymmetricBoostCore2]
 *        Core3 MSRC001_0064[CpuFid] += F3x10C[AsymmetricBoostCore3]
 *        Core4 MSRC001_0064[CpuFid] += F3x10C[AsymmetricBoostCore4]
 *        Core5 MSRC001_0064[CpuFid] += F3x10C[AsymmetricBoostCore5]
 *    }
 *
 * @param[in]  FamilySpecificServices  The current Family Specific Services.
 * @param[in]  CpuEarlyParamsPtr       Service related parameters (unused).
 * @param[in]  StdHeader               Config handle for library and services.
 *
 */
VOID
F10PmAsymBoostInit (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       AMD_CPU_EARLY_PARAMS  *CpuEarlyParamsPtr,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  AP_TASK                 TaskPtr;
  UINT32                  LocalPciRegister;
  PCI_ADDR                PciAddress;
  CPUID_DATA              CpuidData;

  // Check if CPB is supported. if yes, skip boosted p-state.
  LibAmdCpuidRead (AMD_CPUID_APM, &CpuidData, StdHeader);
  if (((CpuidData.EDX_Reg & 0x00000200) >> 9) == 1) {
    LibAmdCpuidRead (CPUID_LONG_MODE_ADDR, &CpuidData, StdHeader);
    if ((CpuidData.ECX_Reg & 0x000000FF) == 5) {
      OptionMultiSocketConfiguration.GetCurrPciAddr (&PciAddress, StdHeader);
      // Read F3x10C [Boost Offset]
      PciAddress.AddressValue = F3x10C_ADDR;
      LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);

      TaskPtr.FuncAddress.PfApTaskI = SetAsymBoost;
      TaskPtr.ExeFlags = WAIT_FOR_CORE;
      TaskPtr.DataTransfer.DataTransferFlags = 0;
      TaskPtr.DataTransfer.DataSizeInDwords = 1;
      TaskPtr.DataTransfer.DataPtr = &LocalPciRegister;
      ApUtilRunCodeOnAllLocalCoresAtEarly (&TaskPtr, StdHeader, CpuEarlyParamsPtr);
    }
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Set Asymmetric Boost.
 *
 * This function set Asymmetric Boost.
 *
 * @param[in]  AsymBoostRegister Contains the value of Asymmetric Boost register
 * @param[in]  StdHeader         Config handle for library and services
 *
 */
VOID
STATIC
SetAsymBoost (
  IN       VOID *AsymBoostRegister,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8  ControlByte;
  UINT32 Core;
  UINT32 Ignored;
  UINT64 MsrValue;
  AGESA_STATUS IgnoredSts;

  IdentifyCore (StdHeader, &Ignored, &Ignored, &Core, &IgnoredSts);
  ControlByte = (UINT8) ((Core & 0xFF) * 2);
  LibAmdMsrRead (MSR_PSTATE_0, &MsrValue, StdHeader);
  // Bits 5:0
  ((PSTATE_MSR *) &MsrValue)->CpuFid += ((*(UINT32*) AsymBoostRegister >> ControlByte) & 0x3);
  LibAmdMsrWrite (MSR_PSTATE_0, &MsrValue, StdHeader);
}

