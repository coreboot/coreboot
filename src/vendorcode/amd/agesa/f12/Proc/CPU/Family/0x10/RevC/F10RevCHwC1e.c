/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_10 HW C1e feature support functions.
 *
 * Provides the functions necessary to initialize the hardware C1e feature.
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
#include "cpuHwC1e.h"
#include "cpuApicUtilities.h"
#include "cpuF10PowerMgmt.h"
#include "cpuFamilyTranslation.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)
#define FILECODE PROC_CPU_FAMILY_0X10_REVC_F10REVCHWC1E_FILECODE

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
F10InitializeHwC1eOnCore (
  IN       VOID *IntPendMsr,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 *  Should hardware C1e be enabled
 *
 * @param[in]    HwC1eServices      Pointer to this CPU's HW C1e family services.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @retval       TRUE               HW C1e is supported.
 *
 */
BOOLEAN
STATIC
F10IsHwC1eSupported (
  IN       HW_C1E_FAMILY_SERVICES *HwC1eServices,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  CPU_LOGICAL_ID LogicalId;

  GetLogicalIdOfCurrentCore (&LogicalId, StdHeader);

  if (((LogicalId.Revision & AMD_F10_RB_ALL) & ~(AMD_F10_RB_C3)) != 0) {
    return FALSE;
  }
  return TRUE;
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Enable Hardware C1e on a family 10h CPU.
 *
 * @param[in]    HwC1eServices      Pointer to this CPU's HW C1e family services.
 * @param[in]    EntryPoint         Timepoint designator.
 * @param[in]    PlatformConfig     Contains the runtime modifiable feature input data.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @return       AGESA_SUCCESS      Always succeeds.
 *
 */
AGESA_STATUS
STATIC
F10InitializeHwC1e (
  IN       HW_C1E_FAMILY_SERVICES *HwC1eServices,
  IN       UINT64 EntryPoint,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 C1eData;
  UINT64 LocalMsrRegister;
  AP_TASK TaskPtr;

  LocalMsrRegister = 0;
  C1eData     = PlatformConfig->C1ePlatformData;

  if (PlatformConfig->C1eMode == C1eModeAuto) {
    C1eData = PlatformConfig->C1ePlatformData3;
  }

  ((INTPEND_MSR *) &LocalMsrRegister)->IoMsgAddr = C1eData;
  ((INTPEND_MSR *) &LocalMsrRegister)->IoRd = 1;
  ((INTPEND_MSR *) &LocalMsrRegister)->C1eOnCmpHalt = 1;
  ((INTPEND_MSR *) &LocalMsrRegister)->SmiOnCmpHalt = 0;

  TaskPtr.FuncAddress.PfApTaskI = F10InitializeHwC1eOnCore;
  TaskPtr.DataTransfer.DataSizeInDwords = 2;
  TaskPtr.DataTransfer.DataPtr = &LocalMsrRegister;
  TaskPtr.DataTransfer.DataTransferFlags = 0;
  TaskPtr.ExeFlags = WAIT_FOR_CORE;
  ApUtilRunCodeOnAllLocalCoresAtEarly (&TaskPtr, StdHeader, NULL);

  return AGESA_SUCCESS;
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Enable Hardware C1e on a family 10h core.
 *
 * @param[in]    IntPendMsr         MSR value to write to C001_0055 as determined by core 0.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 */
VOID
STATIC
F10InitializeHwC1eOnCore (
  IN       VOID *IntPendMsr,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64  LocalMsrRegister;

  // Enable C1e
  LibAmdMsrWrite (MSR_INTPEND, (UINT64 *) IntPendMsr, StdHeader);

  // Set OS Visible Workaround Status BIT1 to indicate that C1e
  // is enabled.
  LibAmdMsrRead (MSR_OSVW_Status, &LocalMsrRegister, StdHeader);
  LocalMsrRegister |= BIT1;
  LibAmdMsrWrite (MSR_OSVW_Status, &LocalMsrRegister, StdHeader);
}


CONST HW_C1E_FAMILY_SERVICES ROMDATA F10HwC1e =
{
  0,
  F10IsHwC1eSupported,
  F10InitializeHwC1e
};
