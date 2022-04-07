/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <AGESA.h>
#include <spd_bin.h>
#include <northbridge/amd/agesa/BiosCallOuts.h>
#include <SB800.h>

#include "gpio_ftns.h"

static AGESA_STATUS board_BeforeDramInit (UINT32 Func, UINTN Data, VOID *ConfigPtr);
static AGESA_STATUS board_ReadSpd_from_cbfs(UINT32 Func, UINTN Data, VOID *ConfigPtr);

const BIOS_CALLOUT_STRUCT BiosCallouts[] =
{
	{AGESA_DO_RESET,			agesa_Reset },
	{AGESA_READ_SPD,			board_ReadSpd_from_cbfs },
	{AGESA_READ_SPD_RECOVERY,		agesa_NoopUnsupported },
	{AGESA_RUNFUNC_ONAP,			agesa_RunFuncOnAp },
	{AGESA_GNB_PCIE_SLOT_RESET,		agesa_NoopSuccess },
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

static AGESA_STATUS board_ReadSpd_from_cbfs(UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	AGESA_READ_SPD_PARAMS *info = ConfigPtr;

	if (!ENV_RAMINIT)
		return AGESA_UNSUPPORTED;

	u8 index = get_spd_offset();

	if (info->MemChannelId > 0)
		return AGESA_UNSUPPORTED;
	if (info->SocketId != 0)
		return AGESA_UNSUPPORTED;
	if (info->DimmId != 0)
		return AGESA_UNSUPPORTED;

	/* Read index 0, first SPD_SIZE bytes of spd.bin file. */
	if (read_ddr3_spd_from_cbfs((u8*)info->Buffer, index) < 0)
		die("No SPD data\n");

	return AGESA_SUCCESS;
}
