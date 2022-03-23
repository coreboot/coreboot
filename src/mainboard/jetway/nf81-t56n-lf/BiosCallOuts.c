/* SPDX-License-Identifier: GPL-2.0-only */

#include <AGESA.h>
#include <northbridge/amd/agesa/BiosCallOuts.h>

#include <amdlib.h>
#include <amdblocks/acpimmio.h>
#include <vendorcode/amd/cimx/sb800/SB800.h>
#include <stdint.h>

static AGESA_STATUS board_GnbPcieSlotReset (UINT32 Func, UINTN Data, VOID *ConfigPtr);
static AGESA_STATUS board_BeforeDramInit (UINT32 Func, UINTN Data, VOID *ConfigPtr);

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
static AGESA_STATUS board_BeforeDramInit (UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	/* Unlike e.g. AMD Inagua, Persimmon is unable to vary the RAM voltage.
	 * Make sure the right speed settings are selected.
	 */
	((MEM_DATA_STRUCT*)ConfigPtr)->ParameterListPtr->DDR3Voltage = VOLT1_5;
	return AGESA_SUCCESS;
}

/* PCIE slot reset control */
static AGESA_STATUS board_GnbPcieSlotReset (UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	AGESA_STATUS Status;
	PCIe_SLOT_RESET_INFO	*ResetInfo;

	uint32_t	GpioMmioAddr;
	uint8_t	 Data8;

	ResetInfo = ConfigPtr;
	Status = AGESA_UNSUPPORTED;
	GpioMmioAddr = (uintptr_t)acpimmio_gpio_100;
	switch (ResetInfo->ResetId)
	{
	case 46:	/* GPIO50 = SBGPIO_PCIE_RST# affects LAN0, LAN1, PCIe slot */
		switch (ResetInfo->ResetControl) {
		case AssertSlotReset:
			Data8 = Read64Mem8(GpioMmioAddr+SB_GPIO_REG50);
			Data8 &= ~(uint8_t)BIT6;
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
