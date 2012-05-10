/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch HwAcpi controller
 *
 * Init HwAcpi Controller features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 44324 $   @e \$Date: 2010-12-22 17:16:51 +0800 (Wed, 22 Dec 2010) $
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
****************************************************************************
*/
#include "FchPlatform.h"
#include "amdlib.h"
#include "cpuServices.h"
#include "Filecode.h"
#define FILECODE PROC_FCH_HWACPI_HWACPILATE_FILECODE

#define AMD_CPUID_APICID_LPC_BID    0x00000001  // Local APIC ID, Logical Processor Count, Brand ID

extern VOID  HpetInit                          (IN VOID  *FchDataPtr);
extern VOID  C3PopupSetting                    (IN VOID  *FchDataPtr);
extern VOID  GcpuRelatedSetting              (IN VOID  *FchDataPtr);
extern VOID  StressResetModeLate               (IN VOID  *FchDataPtr);

/**
 * FchInitLateHwAcpi - Prepare HwAcpi controller to boot to OS.
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitLateHwAcpi (
  IN  VOID     *FchDataPtr
  )
{
  FCH_DATA_BLOCK         *LocalCfgPtr;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;

  HpetInit (LocalCfgPtr);

  if ( IsGCPU (LocalCfgPtr) ) {
    GcpuRelatedSetting (LocalCfgPtr);
  } else {
    C3PopupSetting (LocalCfgPtr);
  }

  // Mt C1E Enable
  MtC1eEnable (LocalCfgPtr);

  if (LocalCfgPtr->SerialDb.SerialDebugBusEnable == 1 ) {
    RwMem (ACPI_MMIO_BASE + SERIAL_DEBUG_BASE +  FCH_SDB_REG00, AccessWidth8, 0xFF, 0x05);
  }

  StressResetModeLate (LocalCfgPtr);
}

/**
 * IsGCPU - Is Gcpu Cpu?
 *
 *
 * @retval  TRUE or FALSE
 *
 */
BOOLEAN
IsGCPU (
  IN  VOID     *FchDataPtr
  )
{
  UINT8                   ExtendedFamily;
  UINT8                   ExtendedModel;
  UINT8                   BaseFamily;
  UINT8                   BaseModel;
  UINT8                   Stepping;
  UINT8                   Family;
  UINT8                   Model;
  CPUID_DATA              CpuId;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  LibAmdCpuidRead (AMD_CPUID_APICID_LPC_BID, &CpuId, StdHeader);

  ExtendedFamily = (UINT8) ((CpuId.EAX_Reg >> 20) & 0xff);
  ExtendedModel = (UINT8) ((CpuId.EAX_Reg >> 16) & 0xf);
  BaseFamily = (UINT8) ((CpuId.EAX_Reg >> 8) & 0xf);
  BaseModel = (UINT8) ((CpuId.EAX_Reg >> 4) & 0xf);
  Stepping = (UINT8) ((CpuId.EAX_Reg >> 0) & 0xf);
  Family = BaseFamily + ExtendedFamily;
  Model = (ExtendedModel << 4) + BaseModel;

  if ( (Family == 0x12) || \
       (Family == 0x14) || \
       (Family == 0x16) || \
       ((Family == 0x15) && ((Model == 0x10) || (Model == 0x30))) ) {
    return TRUE;
  } else {
    return FALSE;
  }
}

