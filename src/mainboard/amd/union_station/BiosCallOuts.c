/* SPDX-License-Identifier: GPL-2.0-only */

#include <AGESA.h>
#include <amdlib.h>
#include <amdblocks/acpimmio.h>
#include <northbridge/amd/agesa/BiosCallOuts.h>
#include <SB800.h>
#include <southbridge/amd/cimx/sb800/gpio_oem.h>

static AGESA_STATUS board_BeforeDramInit(UINT32 Func, UINTN Data, VOID *ConfigPtr);
static AGESA_STATUS board_GnbPcieSlotReset(UINT32 Func, UINTN Data, VOID *ConfigPtr);

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

/* Call the host environment interface to provide a user hook opportunity. */
static AGESA_STATUS board_BeforeDramInit(UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	AGESA_STATUS       Status;
	MEM_DATA_STRUCT    *MemData;
	UINT32             GpioMmioAddr;
	UINT8              Data8;
	UINT8              TempData8;

	MemData = ConfigPtr;

	Status = AGESA_SUCCESS;
	GpioMmioAddr = (uintptr_t)acpimmio_gpio_100;

	Data8 = Read64Mem8(GpioMmioAddr + SB_GPIO_REG178);
	Data8 &= ~BIT5;
	TempData8 = Read64Mem8(GpioMmioAddr + SB_GPIO_REG178);
	TempData8 &= 0x03;
	TempData8 |= Data8;
	Write64Mem8(GpioMmioAddr + SB_GPIO_REG178, TempData8);

	Data8 |= BIT2 | BIT3;
	Data8 &= ~BIT4;
	TempData8 = Read64Mem8(GpioMmioAddr + SB_GPIO_REG178);
	TempData8 &= 0x23;
	TempData8 |= Data8;
	Write64Mem8(GpioMmioAddr + SB_GPIO_REG178, TempData8);

	Data8 = Read64Mem8(GpioMmioAddr + SB_GPIO_REG179);
	Data8 &= ~BIT5;
	TempData8 = Read64Mem8(GpioMmioAddr + SB_GPIO_REG179);
	TempData8 &= 0x03;
	TempData8 |= Data8;
	Write64Mem8(GpioMmioAddr + SB_GPIO_REG179, TempData8);

	Data8 |= BIT2 | BIT3;
	Data8 &= ~BIT4;
	TempData8 = Read64Mem8(GpioMmioAddr + SB_GPIO_REG179);
	TempData8 &= 0x23;
	TempData8 |= Data8;
	Write64Mem8(GpioMmioAddr + SB_GPIO_REG179, TempData8);

	switch (MemData->ParameterListPtr->DDR3Voltage) {
	case VOLT1_35:
		Data8 = Read64Mem8(GpioMmioAddr + SB_GPIO_REG178);
		Data8 &= ~(UINT8)BIT6;
		Write64Mem8(GpioMmioAddr + SB_GPIO_REG178, Data8);
		Data8 = Read64Mem8(GpioMmioAddr + SB_GPIO_REG179);
		Data8 |= (UINT8)BIT6;
		Write64Mem8(GpioMmioAddr + SB_GPIO_REG179, Data8);
		break;
	case VOLT1_25:
		Data8 = Read64Mem8(GpioMmioAddr + SB_GPIO_REG178);
		Data8 &= ~(UINT8)BIT6;
		Write64Mem8(GpioMmioAddr + SB_GPIO_REG178, Data8);
		Data8 = Read64Mem8(GpioMmioAddr + SB_GPIO_REG179);
		Data8 &= ~(UINT8)BIT6;
		Write64Mem8(GpioMmioAddr + SB_GPIO_REG179, Data8);
		break;
	case VOLT1_5:
	default:
		Data8 = Read64Mem8(GpioMmioAddr + SB_GPIO_REG178);
		Data8 |= (UINT8)BIT6;
		Write64Mem8(GpioMmioAddr + SB_GPIO_REG178, Data8);
		Data8 = Read64Mem8(GpioMmioAddr + SB_GPIO_REG179);
		Data8 &= ~(UINT8)BIT6;
		Write64Mem8(GpioMmioAddr + SB_GPIO_REG179, Data8);
	}
	return Status;
}

/* PCIE slot reset control */
static AGESA_STATUS board_GnbPcieSlotReset(UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	AGESA_STATUS            Status;
	PCIe_SLOT_RESET_INFO    *ResetInfo;
	UINT32                  GpioMmioAddr;
	UINT8                   Data8;

	GpioMmioAddr = (uintptr_t)acpimmio_gpio_100;

	ResetInfo = ConfigPtr;
	Status = AGESA_UNSUPPORTED;
	switch (ResetInfo->ResetId) {
	case 4:
		switch (ResetInfo->ResetControl) {
		case AssertSlotReset:
			Data8 = Read64Mem8(GpioMmioAddr + SB_GPIO_REG21);
			Data8 &= ~(UINT8)BIT6;
			/* MXM_GPIO0. GPIO21 */
			Write64Mem8(GpioMmioAddr + SB_GPIO_REG21, Data8);
			Status = AGESA_SUCCESS;
			break;
		case DeassertSlotReset:
			Data8 = Read64Mem8(GpioMmioAddr + SB_GPIO_REG21);
			Data8 |= BIT6;
			/* MXM_GPIO0. GPIO21 */
			Write64Mem8(GpioMmioAddr + SB_GPIO_REG21, Data8);
			Status = AGESA_SUCCESS;
			break;
		}
		break;
	case 6:
		switch (ResetInfo->ResetControl) {
		case AssertSlotReset:
			Data8 = Read64Mem8(GpioMmioAddr + SB_GPIO_REG25);
			Data8 &= ~(UINT8)BIT6;
			/* PCIE_RST#_LAN, GPIO25 */
			Write64Mem8(GpioMmioAddr + SB_GPIO_REG25, Data8);
			Status = AGESA_SUCCESS;
			break;
		case DeassertSlotReset:
			Data8 = Read64Mem8(GpioMmioAddr + SB_GPIO_REG25);
			Data8 |= BIT6;
			/* PCIE_RST#_LAN, GPIO25 */
			Write64Mem8(GpioMmioAddr + SB_GPIO_REG25, Data8);
			Status = AGESA_SUCCESS;
			break;
		}
		break;
	case 7:
		switch (ResetInfo->ResetControl) {
		case AssertSlotReset:
			Data8 = Read64Mem8(GpioMmioAddr + SB_GPIO_REG02);
			Data8 &= ~(UINT8)BIT6;
			/* MPCIE_RST0, GPIO02 */
			Write64Mem8(GpioMmioAddr + SB_GPIO_REG02, Data8);
			Status = AGESA_SUCCESS;
			break;
		case DeassertSlotReset:
			Data8 = Read64Mem8(GpioMmioAddr + SB_GPIO_REG02);
			Data8 |= BIT6;
			/* MPCIE_RST0, GPIO02 */
			Write64Mem8(GpioMmioAddr + SB_GPIO_REG02, Data8);
			Status = AGESA_SUCCESS;
			break;
		}
		break;
	}
	return Status;
}
