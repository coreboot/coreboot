/* SPDX-License-Identifier: GPL-2.0-only */

#include <AGESA.h>
#include <amdlib.h>
#include <amdblocks/acpimmio.h>
#include <northbridge/amd/agesa/BiosCallOuts.h>
#include <SB800.h>

static AGESA_STATUS board_BeforeDramInit (UINT32 Func, UINTN Data, VOID *ConfigPtr);

const BIOS_CALLOUT_STRUCT BiosCallouts[] =
{
	{AGESA_DO_RESET,			agesa_Reset },
	{AGESA_READ_SPD,			agesa_ReadSpd },
	{AGESA_READ_SPD_RECOVERY,		agesa_NoopUnsupported },
	{AGESA_RUNFUNC_ONAP,			agesa_RunFuncOnAp },
	{AGESA_HOOKBEFORE_DQS_TRAINING,		agesa_NoopSuccess },
	{AGESA_HOOKBEFORE_DRAM_INIT,		board_BeforeDramInit },
	{AGESA_HOOKBEFORE_EXIT_SELF_REF,	agesa_NoopSuccess },
	{AGESA_GNB_PCIE_SLOT_RESET,		agesa_NoopUnsupported },
};
const int BiosCalloutsLen = ARRAY_SIZE(BiosCallouts);

/* Call the host environment interface to provide a user hook opportunity. */
static AGESA_STATUS board_BeforeDramInit (UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	AGESA_STATUS       Status;
	MEM_DATA_STRUCT    *MemData;
	UINT32             GpioMmioAddr;
	UINT8              Data8;
	UINT8              TempData8;

	MemData = ConfigPtr;

	Status = AGESA_SUCCESS;
	GpioMmioAddr = (uintptr_t)acpimmio_gpio_100;

	Data8 = Read64Mem8(GpioMmioAddr+SB_GPIO_REG178);
	Data8 &= ~BIT5;
	TempData8 = Read64Mem8(GpioMmioAddr+SB_GPIO_REG178);
	TempData8 &= 0x03;
	TempData8 |= Data8;
	Write64Mem8(GpioMmioAddr+SB_GPIO_REG178, TempData8);

	Data8 |= BIT2+BIT3;
	Data8 &= ~BIT4;
	TempData8 = Read64Mem8(GpioMmioAddr+SB_GPIO_REG178);
	TempData8 &= 0x23;
	TempData8 |= Data8;
	Write64Mem8(GpioMmioAddr+SB_GPIO_REG178, TempData8);
	Data8 = Read64Mem8(GpioMmioAddr+SB_GPIO_REG179);
	Data8 &= ~BIT5;
	TempData8 = Read64Mem8(GpioMmioAddr+SB_GPIO_REG179);
	TempData8 &= 0x03;
	TempData8 |= Data8;
	Write64Mem8(GpioMmioAddr+SB_GPIO_REG179, TempData8);
	Data8 |= BIT2+BIT3;
	Data8 &= ~BIT4;
	TempData8 = Read64Mem8(GpioMmioAddr+SB_GPIO_REG179);
	TempData8 &= 0x23;
	TempData8 |= Data8;
	Write64Mem8(GpioMmioAddr+SB_GPIO_REG179, TempData8);

	/*
	 * this seems to be just copy-pasted from the AMD reference boards and
	 * needs some investigation
	 */
	switch (MemData->ParameterListPtr->DDR3Voltage) {
	case VOLT1_35:
		Data8 = Read64Mem8(GpioMmioAddr+SB_GPIO_REG178);
		Data8 &= ~(UINT8)BIT6;
		Write64Mem8(GpioMmioAddr+SB_GPIO_REG178, Data8);
		Data8 = Read64Mem8(GpioMmioAddr+SB_GPIO_REG179);
		Data8 |= (UINT8)BIT6;
		Write64Mem8(GpioMmioAddr+SB_GPIO_REG179, Data8);
		break;
	case VOLT1_25:
		Data8 = Read64Mem8(GpioMmioAddr+SB_GPIO_REG178);
		Data8 &= ~(UINT8)BIT6;
		Write64Mem8(GpioMmioAddr+SB_GPIO_REG178, Data8);
		Data8 = Read64Mem8(GpioMmioAddr+SB_GPIO_REG179);
		Data8 &= ~(UINT8)BIT6;
		Write64Mem8(GpioMmioAddr+SB_GPIO_REG179, Data8);
		break;
	case VOLT1_5:
	default:
		Data8 = Read64Mem8(GpioMmioAddr+SB_GPIO_REG178);
		Data8 |= (UINT8)BIT6;
		Write64Mem8(GpioMmioAddr+SB_GPIO_REG178, Data8);
		Data8 = Read64Mem8(GpioMmioAddr+SB_GPIO_REG179);
		Data8 &= ~(UINT8)BIT6;
		Write64Mem8(GpioMmioAddr+SB_GPIO_REG179, Data8);
	}
	/* disable memory clear for boot time reduction */
	MemData->ParameterListPtr->EnableMemClr = FALSE;
	return Status;
}
