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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
*****************************************************************************
*
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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
#include "Ids.h"
#include "cpuServices.h"
#include "Filecode.h"
#define FILECODE PROC_FCH_HWACPI_FAMILY_HUDSON2_HUDSON2HWACPILATESERVICE_FILECODE

#define AMD_CPUID_APICID_LPC_BID    0x00000001ul  // Local APIC ID, Logical Processor Count, Brand ID

/**
 * C3PopupSetting - Program Fch C state function
 *
 *
 *
 * @param[in] FchDataPtr   Fch configuration structure pointer.
 *
 */
VOID
C3PopupSetting (
  IN  VOID     *FchDataPtr
  )
{
#define NON_SUPPORT_PREVIOUS_C3 TRUE
#ifndef NON_SUPPORT_PREVIOUS_C3
  UINT32       Value;
  BOOLEAN      ProcessorPresent;

  //
  // C-State and VID/FID Change
  //
  ProcessorPresent = GetActiveCoresInGivenSocket (0, &Value, ((FCH_DATA_BLOCK *) FchDataPtr)->StdHeader);

  if (ProcessorPresent && (Value > 1)) {
    //
    //PM 0x80[2]=1, For system with dual core CPU, set this bit to 1 to automatically clear BM_STS when the C3 state is being initiated.
    //PM 0x80[1]=1, For system with dual core CPU, set this bit to 1 and BM_STS will cause C3 to wakeup regardless of BM_RLD
    //PM 0x7E[6]=1, Enable pop-up for C3. For internal bus mastering or BmReq# from the NB, the FCH will de-assert
    //LDTSTP# (pop-up) to allow DMA traffic, then assert LDTSTP# again after some idle time.
    //
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG80, AccessWidth8, (UINT32)~(BIT1 + BIT2), (BIT1 + BIT2));
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG7E, AccessWidth8, (UINT32)~BIT6, BIT6);
  }

  //
  //PM 0x80 [8] = 0 for system with NB
  //Note: North bridge has AllowLdtStop built for both display and PCIE traffic to wake up the HT link.
  //BmReq# needs to be ignored otherwise may cause LDTSTP# not to toggle.
  //PM_IO 0x80[3]=1, Ignore BM_STS_SET message from NB
  //
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG80, AccessWidth16, (UINT32)~(BIT9 + BIT8 + BIT7 + BIT4 + BIT3 + BIT2 + BIT1 + BIT0), 0x21F);

  //
  //LdtStartTime = 10h for minimum LDTSTP# de-assertion duration of 16us in StutterMode. This is to guarantee that
  //the HT link has been safely reconnected before it can be disconnected again. If C3 pop-up is enabled, the 16us also
  //serves as the minimum idle time before LDTSTP# can be asserted again. This allows DMA to finish before the HT
  //link is disconnected.
  //
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG94 + 2, AccessWidth8, 0, 0x10);

  //
  //This setting provides 16us delay before the assertion of LDTSTOP# when C3 is entered. The
  //
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG98 + 1, AccessWidth8, 0, 0x10);

  //
  // ASIC info
  //
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG7C, AccessWidth8, 0, 0x85);
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG7C + 1, AccessWidth8, 0, 0x01);
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG7E + 1, AccessWidth8, (UINT32)~(BIT7 + BIT5), BIT7 + BIT5);
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG88 + 1, AccessWidth8, (UINT32)~BIT4, BIT4);
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG98 + 3, AccessWidth8, 0, 0x10);
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGB4 + 1, AccessWidth8, 0, 0x0B);
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG88, AccessWidth8, (UINT32)~(BIT4 + BIT5), BIT4 + BIT5);
#else
  // C-State and VID/FID Change
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG88, AccessWidth8, (UINT32)~(BIT5), BIT5);

  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG80, AccessWidth16, (UINT32)~(BIT2), BIT2);

  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG80, AccessWidth16, (UINT32)~(BIT1), BIT1);

  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG7E, AccessWidth8, (UINT32)~(BIT6), BIT6);

  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG94, AccessWidth8, 0, 0x01);

  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG89, AccessWidth8, (UINT32)~BIT4, BIT4);
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG88, AccessWidth8, (UINT32)~BIT4, BIT4);

  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG9B, AccessWidth8, (UINT32)~(BIT6 + BIT5 + BIT4), BIT4);

  RwMem (ACPI_MMIO_BASE + PMIO_BASE + 0x9B , AccessWidth8, (UINT32)~(BIT1 + BIT0), 0);
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG96, AccessWidth8, 0, 0x10);
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG99, AccessWidth8, 0, 0x10);
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG8E, AccessWidth8, 0, 0x80);
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG97, AccessWidth8, (UINT32)~(BIT1 + BIT0), 0);

  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG80, AccessWidth16, (UINT32)~(BIT4), BIT4);

  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG80, AccessWidth16, (UINT32)~(BIT9), BIT9);

  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG80, AccessWidth16, (UINT32)~(BIT7), 0);
#endif

}

/**
 * GcpuRelatedSetting - Program Gcpu C related function
 *
 *
 *
 * @param[in] FchDataPtr   Fch configuration structure pointer.
 *
 */
VOID
GcpuRelatedSetting (
  IN  VOID     *FchDataPtr
  )
{
  UINT8        FchAcDcMsg;
  UINT8        FchTimerTickTrack;
  UINT8        FchClockInterruptTag;
  UINT8        FchOhciTrafficHanding;
  UINT8        FchEhciTrafficHanding;
  UINT8        FchGcpuMsgCMultiCore;
  UINT8        FchGcpuMsgCStage;
  UINT32       Value;
  FCH_DATA_BLOCK         *LocalCfgPtr;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;

  FchAcDcMsg = (UINT8) LocalCfgPtr->Gcpu.AcDcMsg;
  FchTimerTickTrack = (UINT8) LocalCfgPtr->Gcpu.TimerTickTrack;
  FchClockInterruptTag = (UINT8) LocalCfgPtr->Gcpu.ClockInterruptTag;
  FchOhciTrafficHanding = (UINT8) LocalCfgPtr->Gcpu.OhciTrafficHanding;
  FchEhciTrafficHanding = (UINT8) LocalCfgPtr->Gcpu.EhciTrafficHanding;
  FchGcpuMsgCMultiCore = (UINT8) LocalCfgPtr->Gcpu.GcpuMsgCMultiCore;
  FchGcpuMsgCStage = (UINT8) LocalCfgPtr->Gcpu.GcpuMsgCStage;

  ReadMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGA0, AccessWidth32, &Value);
  Value = Value & 0xC07F00A0;

  if ( FchAcDcMsg ) {
    Value = Value | BIT0;
  }

  if ( FchTimerTickTrack ) {
    Value = Value | BIT1;
  }

  if ( FchClockInterruptTag ) {
    Value = Value | BIT10;
  }

  if ( FchOhciTrafficHanding ) {
    Value = Value | BIT13;
  }

  if ( FchEhciTrafficHanding ) {
    Value = Value | BIT15;
  }

  if ( FchGcpuMsgCMultiCore ) {
    Value = Value | BIT23;
  }

  if ( FchGcpuMsgCMultiCore ) {
    Value = (Value | (BIT6 + BIT4 + BIT3 + BIT2));
  }

  WriteMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGA0, AccessWidth32, &Value);
}

/**
 * MtC1eEnable - Program Mt C1E Enable Function
 *
 *
 *
 * @param[in] FchDataPtr
 *
 */
VOID
MtC1eEnable (
  IN  VOID     *FchDataPtr
  )
{
  FCH_DATA_BLOCK         *LocalCfgPtr;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;

  if ( LocalCfgPtr->HwAcpi.MtC1eEnable ) {
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG7A, AccessWidth16, (UINT32)~BIT15, BIT15);
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG7A, AccessWidth16, (UINT32)~(BIT3 + BIT2 + BIT1 + BIT0), 0x01);
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG80, AccessWidth16, (UINT32)~(BIT13 + BIT7), BIT13 + BIT7);
  }
}

/**
 * StressResetModeLate - Stress Reset Mode
 *
 *
 *
 * @param[in] FchDataPtr
 *
 */
VOID
StressResetModeLate (
  IN  VOID     *FchDataPtr
  )
{
  UINT8                  ResetValue;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  switch ( LocalCfgPtr->HwAcpi.StressResetMode ) {
  case 0:
    return;
  case 1:
    ResetValue = FCH_KBC_RESET_COMMAND;
    LibAmdIoWrite (AccessWidth8, FCH_KBDRST_BASE_IO, &ResetValue, StdHeader);
    break;
  case 2:
    ResetValue = FCH_PCI_RESET_COMMAND06;
    LibAmdIoWrite (AccessWidth8, FCH_PCIRST_BASE_IO, &ResetValue, StdHeader);
    break;
  case 3:
    ResetValue = FCH_PCI_RESET_COMMAND0E;
    LibAmdIoWrite (AccessWidth8, FCH_PCIRST_BASE_IO, &ResetValue, StdHeader);
    break;
  case 4:
    LocalCfgPtr->HwAcpi.StressResetMode = 3;
    return;
  default:
    ASSERT (FALSE);
    return;
  }
  while (LocalCfgPtr->HwAcpi.StressResetMode) {
  }
}



