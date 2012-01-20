/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU Late Init API
 *
 * Contains code for doing any late CPU initialization.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 51822 $   @e \$Date: 2011-04-27 18:58:43 -0600 (Wed, 27 Apr 2011) $
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
 *                            M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "Ids.h"
#include "cpuLateInit.h"
#include "cpuRegisters.h"
#include "GeneralServices.h"
#include "cpuServices.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_CPULATEINIT_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
DisableCf8ExtCfg (
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );
/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*---------------------------------------------------------------------------------------*/
/**
 * Performs CPU related initialization at the late entry point
 *
 * This function should be the last function run by the AGESA
 * CPU module and prepares the processor for the operating system
 * bootstrap load process.
 *
 * @param[in]  StdHeader         Config handle for library and services
 * @param[in]  PlatformConfig    Contains the runtime modifiable feature input data.
 *
 * @retval     AGESA_SUCCESS
 *
 */
AGESA_STATUS
AmdCpuLate (
  IN       AMD_CONFIG_PARAMS   *StdHeader,
  IN       PLATFORM_CONFIGURATION *PlatformConfig
  )
{
  AP_EXE_PARAMS ApParams;

  if ((PlatformConfig->PlatformProfile.AdvancedPerformanceProfile.HardwarePrefetchMode != HARDWARE_PREFETCHER_AUTO) ||
      (PlatformConfig->PlatformProfile.AdvancedPerformanceProfile.SoftwarePrefetchMode != SOFTWARE_PREFETCHES_AUTO)) {
    ApParams.StdHeader = *StdHeader;
    ApParams.FunctionNumber = AP_LATE_TASK_CPU_LATE_INIT;
    ApParams.RelatedDataBlock = (VOID *) PlatformConfig;
    ApParams.RelatedBlockLength = sizeof (PLATFORM_CONFIGURATION);
    RunLateApTaskOnAllAPs (&ApParams, StdHeader);
    CpuLateInitApTask (&ApParams);
  }
  DisableCf8ExtCfg (StdHeader);
  return (AGESA_SUCCESS);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  CpuLateInitApTask
 *
 *  Description:
 *    This is the last function run on all APs
 *
 *  Parameters:
 *    @param[in] ApExeParams   Handle to config for library and services.
 *
 *    @retval         AGESA_STATUS
 *
 *  Processing:
 *
 */
AGESA_STATUS
CpuLateInitApTask (
  IN       AP_EXE_PARAMS *ApExeParams
  )
{
  UINT64   LocalMsrRegister;
  PLATFORM_CONFIGURATION *PlatformConfig;
  BOOLEAN  CuCfg3Exist;

  PlatformConfig = (PLATFORM_CONFIGURATION *) ApExeParams->RelatedDataBlock;
  // The processor that has compute unit has CU_CFG3 MSR
  switch (GetComputeUnitMapping (&(ApExeParams->StdHeader))) {
  case AllCoresMapping:
    CuCfg3Exist = FALSE;
    break;
  case EvenCoresMapping:
    CuCfg3Exist = TRUE;
    break;
  default:
    CuCfg3Exist = FALSE;
  }

  // DISABLE_HARDWARE_PREFETCH
  if (PlatformConfig->PlatformProfile.AdvancedPerformanceProfile.HardwarePrefetchMode == DISABLE_HARDWARE_PREFETCH) {
    // DC_CFG (MSR_C001_1022)
    //  [13] = 1
    //  [15] = 1
    LibAmdMsrRead (MSR_DC_CFG, &LocalMsrRegister, &(ApExeParams->StdHeader));
    LocalMsrRegister |= (BIT13 | BIT15);
    LibAmdMsrWrite (MSR_DC_CFG, &LocalMsrRegister, &(ApExeParams->StdHeader));
    // CU_CFG3 (MSR_C001_102B)
    //  [3]  = 1
    //  [16] = 1
    //  [17] = 1
    //  [18] = 1
    if ((CuCfg3Exist) && (IsCorePairPrimary (FirstCoreIsComputeUnitPrimary, &(ApExeParams->StdHeader)))) {
      LibAmdMsrRead (MSR_CU_CFG3, &LocalMsrRegister, &(ApExeParams->StdHeader));
      LocalMsrRegister |= (BIT3 | BIT16 | BIT17 | BIT18);
      LibAmdMsrWrite (MSR_CU_CFG3, &LocalMsrRegister, &(ApExeParams->StdHeader));
    }
  }

  // DISABLE_L1_PREFETCHER
  if ((PlatformConfig->PlatformProfile.AdvancedPerformanceProfile.HardwarePrefetchMode == DISABLE_L1_PREFETCHER) ||
             (PlatformConfig->PlatformProfile.AdvancedPerformanceProfile.HardwarePrefetchMode == DISABLE_L1_PREFETCHER_AND_HW_PREFETCHER_TRAINING_ON_SOFTWARE_PREFETCHES )) {
    // CU_CFG3 (MSR_C001_102B)
    //  [3] = 1
    if ((CuCfg3Exist) && (IsCorePairPrimary (FirstCoreIsComputeUnitPrimary, &(ApExeParams->StdHeader)))) {
      LibAmdMsrRead (MSR_CU_CFG3, &LocalMsrRegister, &(ApExeParams->StdHeader));
      LocalMsrRegister |= BIT3;
      LibAmdMsrWrite (MSR_CU_CFG3, &LocalMsrRegister, &(ApExeParams->StdHeader));
    }

  }

  // DISABLE_HW_PREFETCHER_TRAINING_ON_SOFTWARE_PREFETCHES
  if ((PlatformConfig->PlatformProfile.AdvancedPerformanceProfile.HardwarePrefetchMode == DISABLE_HW_PREFETCHER_TRAINING_ON_SOFTWARE_PREFETCHES ) ||
             (PlatformConfig->PlatformProfile.AdvancedPerformanceProfile.HardwarePrefetchMode == DISABLE_L1_PREFETCHER_AND_HW_PREFETCHER_TRAINING_ON_SOFTWARE_PREFETCHES )) {
    // DC_CFG (MSR_C001_1022)
    //  [15] = 1
    LibAmdMsrRead (MSR_DC_CFG, &LocalMsrRegister, &(ApExeParams->StdHeader));
    LocalMsrRegister |= BIT15;
    LibAmdMsrWrite (MSR_DC_CFG, &LocalMsrRegister, &(ApExeParams->StdHeader));

  }

  // DISABLE_SOFTWARE_PREFETCHES
  if (PlatformConfig->PlatformProfile.AdvancedPerformanceProfile.SoftwarePrefetchMode == DISABLE_SOFTWARE_PREFETCHES) {
    // MSR_DE_CFG (MSR_C001_1029)
    //  [7:2] = 0x3F
    if (IsCorePairPrimary (FirstCoreIsComputeUnitPrimary, &(ApExeParams->StdHeader))) {
      LibAmdMsrRead (MSR_DE_CFG, &LocalMsrRegister, &(ApExeParams->StdHeader));
      LocalMsrRegister |= 0xFC;
      LibAmdMsrWrite (MSR_DE_CFG, &LocalMsrRegister, &(ApExeParams->StdHeader));
    }
  }

  return AGESA_SUCCESS;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Clear EnableCf8ExtCfg on all socket
 *
 * Clear F3x8C bit 14 EnableCf8ExtCfg
 *
 * @param[in]  StdHeader         Config handle for library and services
 *
 *
 */
VOID
DisableCf8ExtCfg (
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  AGESA_STATUS  AgesaStatus;
  PCI_ADDR PciAddress;
  UINT32 Socket;
  UINT32 Module;
  UINT32 PciData;
  UINT32 LegacyPciAccess;

  ASSERT (IsBsp (StdHeader, &AgesaStatus));

  for (Socket = 0; Socket < GetPlatformNumberOfSockets (); Socket++) {
    for (Module = 0; Module < GetPlatformNumberOfModules (); Module++) {
      if (GetPciAddress (StdHeader, Socket, Module, &PciAddress, &AgesaStatus)) {
        PciAddress.Address.Function = FUNC_3;
        PciAddress.Address.Register = NB_CFG_HIGH_REG;
        LegacyPciAccess = ((1 << 31) + (PciAddress.Address.Register & 0xFC) + (PciAddress.Address.Function << 8) + (PciAddress.Address.Device << 11) + (PciAddress.Address.Bus << 16) + ((PciAddress.Address.Register & 0xF00) << (24 - 8)));
        // read from PCI register
        LibAmdIoWrite (AccessWidth32, IOCF8, &LegacyPciAccess, StdHeader);
        LibAmdIoRead (AccessWidth32, IOCFC, &PciData, StdHeader);
        // Disable Cf8ExtCfg
        PciData &= 0xFFFFBFFF;
        // write to PCI register
        LibAmdIoWrite (AccessWidth32, IOCF8, &LegacyPciAccess, StdHeader);
        LibAmdIoWrite (AccessWidth32, IOCFC, &PciData, StdHeader);
      }
    }
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Calculate an ACPI style checksum
 *
 * Computes the checksum and stores the value to the checksum
 * field of the passed in ACPI table's header.
 *
 * @param[in]  Table             ACPI table to checksum
 * @param[in]  StdHeader         Config handle for library and services
 *
 */
VOID
ChecksumAcpiTable (
  IN OUT   ACPI_TABLE_HEADER *Table,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8  *BuffTempPtr;
  UINT8  Checksum;
  UINT32 BufferOffset;

  Table->Checksum = 0;
  Checksum = 0;
  BuffTempPtr = (UINT8 *) Table;
  for (BufferOffset = 0; BufferOffset < Table->TableLength; BufferOffset++) {
    Checksum = Checksum - *(BuffTempPtr + BufferOffset);
  }

  Table->Checksum = Checksum;
}

/*---------------------------------------------------------------------------------------*/
/**
 *
 * Run code on every AP in the system.
 *
 * @param[in] ApParams       AP task pointer.
 * @param[in] StdHeader      Handle to config for library and services
 *
 * @return    The most severe AGESA_STATUS returned by an AP.
 *
 */
AGESA_STATUS
RunLateApTaskOnAllAPs (
  IN       AP_EXE_PARAMS     *ApParams,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32                  NumberOfSockets;
  UINT32                  NumberOfCores;
  UINT8                   Socket;
  UINT8                   Core;
  UINT8                   ApicId;
  UINT32                  BscSocket;
  UINT32                  Ignored;
  UINT32                  BscCoreNum;
  AGESA_STATUS            CalledStatus;
  AGESA_STATUS            IgnoredStatus;
  AGESA_STATUS            AgesaStatus;

  ASSERT (IsBsp (StdHeader, &IgnoredStatus));

  AgesaStatus = AGESA_SUCCESS;

  IdentifyCore (StdHeader, &BscSocket, &Ignored, &BscCoreNum, &IgnoredStatus);
  NumberOfSockets = GetPlatformNumberOfSockets ();

  for (Socket = 0; Socket < NumberOfSockets; Socket++) {
    if (GetActiveCoresInGivenSocket (Socket, &NumberOfCores, StdHeader)) {
      for (Core = 0; Core < NumberOfCores; Core++) {
        if ((Socket != BscSocket) || (Core != BscCoreNum)) {
          GetApicId (StdHeader, Socket, Core, &ApicId, &IgnoredStatus);
          AGESA_TESTPOINT (TpIfBeforeRunApFromAllAps, StdHeader);
          CalledStatus = AgesaRunFcnOnAp ((UINTN) ApicId, ApParams);
          AGESA_TESTPOINT (TpIfAfterRunApFromAllAps, StdHeader);
          if (CalledStatus > AgesaStatus) {
            AgesaStatus = CalledStatus;
          }
        }
      }
    }
  }
  return AgesaStatus;
}


/*---------------------------------------------------------------------------------------*/
/**
 *
 * Run code on core 0 of every socket in the system.
 *
 * @param[in] ApParams       AP task pointer.
 * @param[in] StdHeader      Handle to config for library and services
 *
 * @return    The most severe AGESA_STATUS returned by an AP.
 *
 */
AGESA_STATUS
RunLateApTaskOnAllCore0s (
  IN       AP_EXE_PARAMS     *ApParams,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32                  NumberOfSockets;
  UINT8                   Socket;
  UINT8                   ApicId;
  UINT32                  BscSocket;
  UINT32                  IgnoredModule;
  UINT32                  IgnoredCore;
  AGESA_STATUS            CalledStatus;
  AGESA_STATUS            IgnoredStatus;
  AGESA_STATUS            AgesaStatus;

  ASSERT (IsBsp (StdHeader, &IgnoredStatus));

  AgesaStatus = AGESA_SUCCESS;

  IdentifyCore (StdHeader, &BscSocket, &IgnoredModule, &IgnoredCore, &IgnoredStatus);
  NumberOfSockets = GetPlatformNumberOfSockets ();

  for (Socket = 0; Socket < NumberOfSockets; Socket++) {
    if (IsProcessorPresent (Socket, StdHeader)) {
      if (Socket != BscSocket) {
        GetApicId (StdHeader, Socket, 0, &ApicId, &IgnoredStatus);
        AGESA_TESTPOINT (TpIfBeforeRunApFromAllCore0s, StdHeader);
        CalledStatus = AgesaRunFcnOnAp ((UINTN) ApicId, ApParams);
        AGESA_TESTPOINT (TpIfAfterRunApFromAllCore0s, StdHeader);
        if (CalledStatus > AgesaStatus) {
          AgesaStatus = CalledStatus;
        }
      }
    }
  }
  return AgesaStatus;
}
