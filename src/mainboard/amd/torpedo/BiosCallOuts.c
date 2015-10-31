/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "AGESA.h"
#include "amdlib.h"
#include <northbridge/amd/agesa/BiosCallOuts.h>
#include "Ids.h"
#include "OptionsIds.h"
#include "heapManager.h"
#include "Hudson-2.h"
#include <stdlib.h>
#include <southbridge/amd/cimx/sb700/gpio_oem.h>

static AGESA_STATUS board_BeforeDramInit (UINT32 Func, UINTN Data, VOID *ConfigPtr);
static AGESA_STATUS board_GnbPcieSlotReset (UINT32 Func, UINTN Data, VOID *ConfigPtr);

const BIOS_CALLOUT_STRUCT BiosCallouts[] =
{
	{AGESA_DO_RESET,			agesa_Reset },
	{AGESA_READ_SPD,			agesa_ReadSpd },
	{AGESA_READ_SPD_RECOVERY,		agesa_NoopUnsupported },
	{AGESA_RUNFUNC_ONAP,			agesa_RunFuncOnAp },
	{AGESA_GNB_PCIE_SLOT_RESET,		board_GnbPcieSlotReset },
	{AGESA_GET_IDS_INIT_DATA,		agesa_EmptyIdsInitData },
	{AGESA_HOOKBEFORE_DRAM_INIT,		board_BeforeDramInit },
	{AGESA_HOOKBEFORE_DRAM_INIT_RECOVERY,	agesa_NoopSuccess },
	{AGESA_HOOKBEFORE_DQS_TRAINING,		agesa_NoopSuccess },
	{AGESA_HOOKBEFORE_EXIT_SELF_REF,	agesa_NoopSuccess },
};
const int BiosCalloutsLen = ARRAY_SIZE(BiosCallouts);

/*  Call the host environment interface to provide a user hook opportunity. */
static AGESA_STATUS board_BeforeDramInit (UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
  AGESA_STATUS      Status;
  UINTN             FcnData;
  MEM_DATA_STRUCT   *MemData;
  UINT32            AcpiMmioAddr;
  UINT32            GpioMmioAddr;
  UINT8             Data8;
  UINT16            Data16;

  FcnData = Data;
  MemData = ConfigPtr;

  Status  = AGESA_SUCCESS;
  /* Get SB MMIO Base (AcpiMmioAddr) */
  WriteIo8 (0xCD6, 0x27);
  Data8   = ReadIo8(0xCD7);
  Data16  = Data8<<8;
  WriteIo8 (0xCD6, 0x26);
  Data8   = ReadIo8(0xCD7);
  Data16  |= Data8;
  AcpiMmioAddr = (UINT32)Data16 << 16;
  GpioMmioAddr = AcpiMmioAddr + GPIO_BASE;

  switch(MemData->ParameterListPtr->DDR3Voltage){
    case VOLT1_35:
      Data8 =  Read64Mem8 (GpioMmioAddr+SB_GPIO_REG178);
      Data8 &= ~(UINT8)BIT6;
      Write64Mem8(GpioMmioAddr+SB_GPIO_REG178, Data8);
      Data8 =  Read64Mem8 (GpioMmioAddr+SB_GPIO_REG179);
      Data8 |= (UINT8)BIT6;
      Write64Mem8(GpioMmioAddr+SB_GPIO_REG179, Data8);
      break;
    case VOLT1_25:
      Data8 =  Read64Mem8 (GpioMmioAddr+SB_GPIO_REG178);
      Data8 &= ~(UINT8)BIT6;
      Write64Mem8(GpioMmioAddr+SB_GPIO_REG178, Data8);
      Data8 =  Read64Mem8 (GpioMmioAddr+SB_GPIO_REG179);
      Data8 &= ~(UINT8)BIT6;
      Write64Mem8(GpioMmioAddr+SB_GPIO_REG179, Data8);
      break;
    case VOLT1_5:
    default:
      Data8 =  Read64Mem8 (GpioMmioAddr+SB_GPIO_REG178);
      Data8 |= (UINT8)BIT6;
      Write64Mem8(GpioMmioAddr+SB_GPIO_REG178, Data8);
  }
  return Status;
}

/* PCIE slot reset control */
static AGESA_STATUS board_GnbPcieSlotReset (UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
  AGESA_STATUS Status;
  UINTN                 FcnData;
  PCIe_SLOT_RESET_INFO  *ResetInfo;

  UINT32  GpioMmioAddr;
  UINT32  AcpiMmioAddr;
  UINT8   Data8;
  UINT16  Data16;

  FcnData   = Data;
  ResetInfo = ConfigPtr;
  // Get SB MMIO Base (AcpiMmioAddr)
  WriteIo8(0xCD6, 0x27);
  Data8 = ReadIo8(0xCD7);
  Data16=Data8<<8;
  WriteIo8(0xCD6, 0x26);
  Data8 = ReadIo8(0xCD7);
  Data16|=Data8;
  AcpiMmioAddr = (UINT32)Data16 << 16;
  Status = AGESA_UNSUPPORTED;
  GpioMmioAddr = AcpiMmioAddr + GPIO_BASE;

  if (ResetInfo->ResetControl == DeassertSlotReset) {
    if (ResetInfo->ResetId & (BIT2+BIT3)) {    //de-assert
      // [GPIO] GPIO45: PE_GPIO1 MXM_POWER_ENABLE, SET HIGH
      Data8 = Read64Mem8(GpioMmioAddr+SB_GPIO_REG45);
      if (Data8 & BIT7) {
        Data8 = Read64Mem8(GpioMmioAddr+SB_GPIO_REG28);
        while (!(Data8 & BIT7)) {
          Data8 = Read64Mem8(GpioMmioAddr+SB_GPIO_REG28);
        }
        // GPIO44: PE_GPIO0 MXM Reset
        Data8 = Read64Mem8(GpioMmioAddr+SB_GPIO_REG44);
        Data8 |= BIT6 ;
        Write64Mem8 (GpioMmioAddr+SB_GPIO_REG44, Data8);
        Status = AGESA_SUCCESS;
      }
    } else {
      Status = AGESA_UNSUPPORTED;
    }
    // Travis
    Data8 = Read64Mem8(GpioMmioAddr+SB_GPIO_REG24);
    Data8 |= BIT6;
    Write64Mem8 (GpioMmioAddr+SB_GPIO_REG24, Data8);
    //DE-Assert ALL PCIE RESET
    // APU GPP0 (Dev 4)
        Data8 = Read64Mem8(GpioMmioAddr+SB_GPIO_REG25);
        Data8 |= BIT6 ;
    Write64Mem8 (GpioMmioAddr+SB_GPIO_REG25, Data8);
    // APU GPP1 (Dev 5)
    Data8 = Read64Mem8(GpioMmioAddr+SB_GPIO_REG01);
    Data8 |= BIT6;
    Write64Mem8 (GpioMmioAddr+SB_GPIO_REG01, Data8);
    // APU GPP2 (Dev 6)
    Data8 = Read64Mem8(GpioMmioAddr+SB_GPIO_REG00);
    Data8 |= BIT6;
    Write64Mem8 (GpioMmioAddr+SB_GPIO_REG00, Data8);
    // APU GPP3 (Dev 7)
    Data8 = Read64Mem8(GpioMmioAddr+SB_GPIO_REG27);
    Data8 |= BIT6;
    Write64Mem8 (GpioMmioAddr+SB_GPIO_REG27, Data8);
  } else {
    if (ResetInfo->ResetId & (BIT2+BIT3)) {  //Pcie Slot Reset is supported
      // GPIO44: PE_GPIO0 MXM Reset
      Data8 = Read64Mem8(GpioMmioAddr+SB_GPIO_REG44);
      Data8 &= ~(UINT8)BIT6;
      Write64Mem8 (GpioMmioAddr+SB_GPIO_REG44, Data8);
        Status = AGESA_SUCCESS;
      }
    // Travis
    Data8 = Read64Mem8(GpioMmioAddr+SB_GPIO_REG24);
        Data8 &= ~(UINT8)BIT6 ;
    Write64Mem8 (GpioMmioAddr+SB_GPIO_REG24, Data8);
    //Assert ALL PCIE RESET
    // APU GPP0 (Dev 4)
        Data8 = Read64Mem8(GpioMmioAddr+SB_GPIO_REG25);
    Data8 &= ~(UINT8)BIT6;
    Write64Mem8 (GpioMmioAddr+SB_GPIO_REG25, Data8);
    // APU GPP1 (Dev 5)
    Data8 = Read64Mem8(GpioMmioAddr+SB_GPIO_REG01);
    Data8 &= ~(UINT8)BIT6;
    Write64Mem8 (GpioMmioAddr+SB_GPIO_REG01, Data8);
    // APU GPP2 (Dev 6)
    Data8 = Read64Mem8(GpioMmioAddr+SB_GPIO_REG00);
    Data8 &= ~(UINT8)BIT6;
    Write64Mem8 (GpioMmioAddr+SB_GPIO_REG00, Data8);
    // APU GPP3 (Dev 7)
    Data8 = Read64Mem8(GpioMmioAddr+SB_GPIO_REG27);
    Data8 &= ~(UINT8)BIT6;
    Write64Mem8 (GpioMmioAddr+SB_GPIO_REG27, Data8);
  }
  return  Status;
}
