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
