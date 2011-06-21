/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_10 SW C1e feature support functions.
 *
 * Provides the functions necessary to initialize the software C1e feature.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/F10
 * @e \$Revision: 37157 $   @e \$Date: 2010-09-01 03:24:07 +0800 (Wed, 01 Sep 2010) $
 *
 */
/*
 *****************************************************************************
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
 * ***************************************************************************
 *
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "cpuRegisters.h"
#include "cpuSwC1e.h"
#include "cpuApicUtilities.h"
#include "cpuF10PowerMgmt.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)
#define FILECODE PROC_CPU_FAMILY_0X10_REVC_F10REVCSWC1E_FILECODE

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
F10InitializeSwC1eOnCore (
  IN       VOID *IntPendMsr,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 *  Should software C1e be enabled
 *
 * @param[in]    SwC1eServices      Pointer to this CPU's SW C1e family services.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @retval       TRUE               SW C1e is supported.
 *
 */
BOOLEAN
STATIC
F10IsSwC1eSupported (
  IN       SW_C1E_FAMILY_SERVICES *SwC1eServices,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  return TRUE;
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Enable Software C1e on a family 10h CPU.
 *
 * @param[in]    SwC1eServices      Pointer to this CPU's SW C1e family services.
 * @param[in]    EntryPoint         Timepoint designator.
 * @param[in]    PlatformConfig     Contains the runtime modifiable feature input data.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @return       AGESA_SUCCESS      Always succeeds.
 *
 */
AGESA_STATUS
STATIC
F10InitializeSwC1e (
  IN       SW_C1E_FAMILY_SERVICES *SwC1eServices,
  IN       UINT64 EntryPoint,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64 MsrRegister;
  AP_TASK TaskPtr;

  MsrRegister = 0;
  ((INTPEND_MSR *) &MsrRegister)->IoMsgAddr = PlatformConfig->C1ePlatformData1;
  ((INTPEND_MSR *) &MsrRegister)->IoMsgData = PlatformConfig->C1ePlatformData2;
  ((INTPEND_MSR *) &MsrRegister)->IoRd = 0;
  ((INTPEND_MSR *) &MsrRegister)->C1eOnCmpHalt = 0;
  ((INTPEND_MSR *) &MsrRegister)->SmiOnCmpHalt = 1;

  TaskPtr.FuncAddress.PfApTaskI = F10InitializeSwC1eOnCore;
  TaskPtr.DataTransfer.DataSizeInDwords = 2;
  TaskPtr.DataTransfer.DataPtr = &MsrRegister;
  TaskPtr.DataTransfer.DataTransferFlags = 0;
  TaskPtr.ExeFlags = WAIT_FOR_CORE;
  ApUtilRunCodeOnAllLocalCoresAtEarly (&TaskPtr, StdHeader, NULL);

  return AGESA_SUCCESS;
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Enable Software C1e on a family 10h core.
 *
 * @param[in]    IntPendMsr         MSR value to write to C001_0055 as determined by core 0.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 */
VOID
STATIC
F10InitializeSwC1eOnCore (
  IN       VOID *IntPendMsr,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64  MsrRegister;

  // Enable C1e
  LibAmdMsrWrite (MSR_INTPEND, (UINT64 *) IntPendMsr, StdHeader);

  // Set OS Visible Workaround Status BIT1 to indicate that C1e
  // is enabled.
  LibAmdMsrRead (MSR_OSVW_Status, &MsrRegister, StdHeader);
  MsrRegister |= BIT1;
  LibAmdMsrWrite (MSR_OSVW_Status, &MsrRegister, StdHeader);
}


CONST SW_C1E_FAMILY_SERVICES ROMDATA F10SwC1e =
{
  0,
  F10IsSwC1eSupported,
  F10InitializeSwC1e
};
