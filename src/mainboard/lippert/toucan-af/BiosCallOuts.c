/* SPDX-License-Identifier: GPL-2.0-only */

#include <AGESA.h>
#include <console/console.h>
#include <northbridge/amd/agesa/BiosCallOuts.h>
#include <SB800.h>
#include <southbridge/amd/cimx/sb800/gpio_oem.h>

/* Should AGESA_GNB_PCIE_SLOT_RESET use agesa_NoopSuccess?
 *
 * COM Express doesn't provide dedicated resets for individual lanes
 * and it's not needed for the on-board Intel I210 GbE controller.
 */

static AGESA_STATUS board_BeforeDramInit (UINT32 Func, UINTN Data, VOID *ConfigPtr);

const BIOS_CALLOUT_STRUCT BiosCallouts[] =
{
	{AGESA_DO_RESET,			agesa_Reset },
	{AGESA_READ_SPD,			agesa_ReadSpd },
	{AGESA_READ_SPD_RECOVERY,		agesa_NoopUnsupported },
	{AGESA_RUNFUNC_ONAP,			agesa_RunFuncOnAp },
	{AGESA_GNB_PCIE_SLOT_RESET,		agesa_NoopUnsupported },
	{AGESA_HOOKBEFORE_DRAM_INIT,		board_BeforeDramInit},
	{AGESA_HOOKBEFORE_DRAM_INIT_RECOVERY,	agesa_NoopSuccess },
	{AGESA_HOOKBEFORE_DQS_TRAINING,		agesa_NoopSuccess },
	{AGESA_HOOKBEFORE_EXIT_SELF_REF,	agesa_NoopSuccess },
};
const int BiosCalloutsLen = ARRAY_SIZE(BiosCallouts);

/*	Call the host environment interface to provide a user hook opportunity. */
static AGESA_STATUS board_BeforeDramInit (UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	MEM_DATA_STRUCT *MemData = ConfigPtr;

	printk(BIOS_INFO, "Setting DDR3 voltage: ");
	FCH_IOMUX(65) = 1; // GPIO65: VMEM_LV_EN# lowers VMEM from 1.5 to 1.35V
	switch (MemData->ParameterListPtr->DDR3Voltage) {
	case VOLT1_25: // board is not able to provide this
		MemData->ParameterListPtr->DDR3Voltage = VOLT1_35; // sorry
		printk(BIOS_INFO, "can't provide 1.25 V, using ");
		// fall through
	default: // AGESA.h says in mixed case 1.5V DIMMs get excluded
	case VOLT1_35:
		FCH_GPIO(65) = 0x08; // = output, disable PU, set to 0
		printk(BIOS_INFO, "1.35 V\n");
		break;
	case VOLT1_5:
		FCH_GPIO(65) = 0xC8; // = output, disable PU, set to 1
		printk(BIOS_INFO, "1.5 V\n");
	}

	return AGESA_SUCCESS;
}
