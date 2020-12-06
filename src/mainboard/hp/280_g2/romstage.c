/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <fsp/soc_binding.h>
#include <soc/romstage.h>
#include <spd_bin.h>
#include <string.h>
#include <types.h>

/* Rcomp resistors are located on the CPU package */
static const u16 rcomp_resistors[3] = { 121, 75, 100 };

/* Rcomp targets for RdOdt, WrDS, WrDSCmd, WrDSCtl, WrDSClk */
static const u16 rcomp_targets[5] = { 50, 26, 20, 20, 26 };

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	FSP_M_CONFIG *const mem_cfg = &mupd->FspmConfig;

	struct spd_block blk = {
		.addr_map = { 0x51, 0x50 },
	};

	get_spd_smbus(&blk);

	mem_cfg->DqPinsInterleaved = true;

	mem_cfg->UserBd = BOARD_TYPE_DESKTOP;

	mem_cfg->MemorySpdDataLen = blk.len;
	mem_cfg->MemorySpdPtr00 = (u32)blk.spd_array[0];
	mem_cfg->MemorySpdPtr10 = (u32)blk.spd_array[1];

	assert(sizeof(mem_cfg->RcompResistor) == sizeof(rcomp_resistors));
	assert(sizeof(mem_cfg->RcompTarget)   == sizeof(rcomp_targets));

	memcpy(mem_cfg->RcompResistor, rcomp_resistors, sizeof(mem_cfg->RcompResistor));
	memcpy(mem_cfg->RcompTarget,   rcomp_targets,   sizeof(mem_cfg->RcompTarget));

	/* These settings are most likely useless if using a release build of FSP */
	mem_cfg->PcdDebugInterfaceFlags = 2;	/* Enable UART */
	mem_cfg->PcdSerialIoUartNumber  = 2;	/* Use UART #2 */
	mem_cfg->PcdSerialDebugBaudRate = 7;	/* 115200 baud */
	mem_cfg->PcdSerialDebugLevel    = 3;	/* Log <= Info */

	/* Trace Hub */
	mem_cfg->PcdDebugInterfaceFlags |= 1 << 5;

	/* Allow changing memory timings after MRC is done */
	mem_cfg->RealtimeMemoryTiming = 1;
	mem_cfg->SaOcSupport = 1;
}
