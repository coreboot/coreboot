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
#include "cpuServices.h"
#include "Filecode.h"
#define FILECODE PROC_FCH_HWACPI_HWACPILATE_FILECODE

#define AMD_CPUID_APICID_LPC_BID    0x00000001ul  // Local APIC ID, Logical Processor Count, Brand ID







///
/// PCI_IRQ_REG_BLOCK- FCH PCI IRQ registers block
///
typedef struct _PCI_IRQ_REG_BLOCK {
  UINT8                PciIrqIndex;       // PciIrqIndex - selects which PCI interrupt to map
  UINT8                PciIrqData;        // PciIrqData  - Interrupt #
} PCI_IRQ_REG_BLOCK;

STATIC PCI_IRQ_REG_BLOCK FchInternalDeviceIrqForApicMode[] = {
    { (FCH_IRQ_INTA | FCH_IRQ_IOAPIC), 0x10},
    { (FCH_IRQ_INTB | FCH_IRQ_IOAPIC), 0x11},
    { (FCH_IRQ_INTC | FCH_IRQ_IOAPIC), 0x12},
    { (FCH_IRQ_INTD | FCH_IRQ_IOAPIC), 0x13},
    { (FCH_IRQ_INTE | FCH_IRQ_IOAPIC), 0x14},
    { (FCH_IRQ_INTF | FCH_IRQ_IOAPIC), 0x15},
    { (FCH_IRQ_INTG | FCH_IRQ_IOAPIC), 0x16},
    { (FCH_IRQ_INTH | FCH_IRQ_IOAPIC), 0x17},
    { (FCH_IRQ_HDAUDIO | FCH_IRQ_IOAPIC), 0x10},
    { (FCH_IRQ_GEC | FCH_IRQ_IOAPIC), 0x10},
    { (FCH_IRQ_SD | FCH_IRQ_IOAPIC), 0x10},
    { (FCH_IRQ_GPPINT0 | FCH_IRQ_IOAPIC), 0x10},
    { (FCH_IRQ_IDE | FCH_IRQ_IOAPIC), 0x11},
    { (FCH_IRQ_USB18INTB | FCH_IRQ_IOAPIC), 0x11},
    { (FCH_IRQ_USB19INTB | FCH_IRQ_IOAPIC), 0x11},
    { (FCH_IRQ_USB22INTB | FCH_IRQ_IOAPIC), 0x11},
    { (FCH_IRQ_GPPINT1 + FCH_IRQ_IOAPIC), 0x11},
    { (FCH_IRQ_USB18INTA | FCH_IRQ_IOAPIC), 0x12},
    { (FCH_IRQ_USB19INTA | FCH_IRQ_IOAPIC), 0x12},
    { (FCH_IRQ_USB22INTA | FCH_IRQ_IOAPIC), 0x12},
    { (FCH_IRQ_USB20INTC | FCH_IRQ_IOAPIC), 0x12},
    { (FCH_IRQ_GPPINT2 | FCH_IRQ_IOAPIC), 0x12},
    { (FCH_IRQ_SATA | FCH_IRQ_IOAPIC), 0x13},
    { (FCH_IRQ_GPPINT3 | FCH_IRQ_IOAPIC), 0x13},
  };

#define NUM_OF_DEVICE_FOR_APICIRQ  ARRAY_SIZE(FchInternalDeviceIrqForApicMode)

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
  AMD_CONFIG_PARAMS      *StdHeader;
  UINT8                  i;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  if ( IsGCPU (LocalCfgPtr) ) {
    GcpuRelatedSetting (LocalCfgPtr);
  } else {
    //TNBU C3PopupSetting (LocalCfgPtr);
  }

  // Mt C1E Enable
  MtC1eEnable (LocalCfgPtr);

  if (LocalCfgPtr->Gpp.SerialDebugBusEnable == TRUE ) {
    RwMem (ACPI_MMIO_BASE + SERIAL_DEBUG_BASE +  FCH_SDB_REG00, AccessWidth8, 0xFF, 0x05);
  }

  StressResetModeLate (LocalCfgPtr);
  SbSleepTrapControl (FALSE); /* TODO: Checkout if we need to disable sleep trap in Non-SMI mode. */
  for (i = 0; i < NUM_OF_DEVICE_FOR_APICIRQ; i++) {
    LibAmdIoWrite (AccessWidth8, FCH_IOMAP_REGC00, &FchInternalDeviceIrqForApicMode[i].PciIrqIndex, StdHeader);
    LibAmdIoWrite (AccessWidth8, FCH_IOMAP_REGC01, &FchInternalDeviceIrqForApicMode[i].PciIrqData, StdHeader);
  }
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
       (Family == 0x16)  ) {
    return TRUE;
  } else {
    return FALSE;
  }
}

