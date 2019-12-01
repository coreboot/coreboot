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

#include <AGESA.h>
#include <amdlib.h>
#include <amdblocks/acpimmio.h>
#include <northbridge/amd/agesa/BiosCallOuts.h>
#include <SB800.h>
#include <stdlib.h>

static AGESA_STATUS board_BeforeDramInit (UINT32 Func, UINTN Data, VOID *ConfigPtr);
static AGESA_STATUS board_GnbPcieSlotReset (UINT32 Func, UINTN Data, VOID *ConfigPtr);

const BIOS_CALLOUT_STRUCT BiosCallouts[] =
{
	{AGESA_DO_RESET,			agesa_Reset },
	{AGESA_READ_SPD,			agesa_ReadSpd },
	{AGESA_READ_SPD_RECOVERY,		agesa_NoopUnsupported },
	{AGESA_RUNFUNC_ONAP,			agesa_RunFuncOnAp },
	{AGESA_GNB_PCIE_SLOT_RESET,		board_GnbPcieSlotReset },
	{AGESA_HOOKBEFORE_DRAM_INIT,		board_BeforeDramInit },
	{AGESA_HOOKBEFORE_DRAM_INIT_RECOVERY,	agesa_NoopSuccess },
	{AGESA_HOOKBEFORE_DQS_TRAINING,		agesa_NoopSuccess },
	{AGESA_HOOKBEFORE_EXIT_SELF_REF,	agesa_NoopSuccess },
};
const int BiosCalloutsLen = ARRAY_SIZE(BiosCallouts);

/*	Call the host environment interface to provide a user hook opportunity. */
static AGESA_STATUS board_BeforeDramInit (UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	// Unlike e.g. AMD Inagua, Persimmon is unable to vary the RAM voltage.
	// Make sure the right speed settings are selected.
	((MEM_DATA_STRUCT*)ConfigPtr)->ParameterListPtr->DDR3Voltage = VOLT1_5;
	return AGESA_SUCCESS;
}

/* PCIE slot reset control */
static AGESA_STATUS board_GnbPcieSlotReset (UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	AGESA_STATUS Status;
	UINTN					FcnData;
	PCIe_SLOT_RESET_INFO	*ResetInfo;

	UINT32	GpioMmioAddr;
	UINT32	AcpiMmioAddr;
	UINT8	 Data8;
	UINT16	Data16;

	FcnData = Data;
	ResetInfo = ConfigPtr;
	// Get SB800 MMIO Base (AcpiMmioAddr)
	Data8 = pm_io_read8(0x27);
	Data16=Data8<<8;
	Data8 = pm_io_read8(0x26);
	Data16|=Data8;
	AcpiMmioAddr = (UINT32)Data16 << 16;
	Status = AGESA_UNSUPPORTED;
	GpioMmioAddr = AcpiMmioAddr + GPIO_BASE;
	switch (ResetInfo->ResetId)
	{
	case 46:	// GPIO50 = SBGPIO_PCIE_RST# affects LAN0, LAN1, PCIe slot
		switch (ResetInfo->ResetControl) {
		case AssertSlotReset:
			Data8 = Read64Mem8(GpioMmioAddr+SB_GPIO_REG50);
			Data8 &= ~(UINT8)BIT6;
			Write64Mem8(GpioMmioAddr+SB_GPIO_REG50, Data8);
			Status = AGESA_SUCCESS;
			break;
		case DeassertSlotReset:
			Data8 = Read64Mem8(GpioMmioAddr+SB_GPIO_REG50);
			Data8 |= BIT6;
			Write64Mem8 (GpioMmioAddr+SB_GPIO_REG50, Data8);
			Status = AGESA_SUCCESS;
			break;
		}
		break;
	}
	return	Status;
}
