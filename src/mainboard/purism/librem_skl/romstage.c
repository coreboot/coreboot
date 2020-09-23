/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <soc/romstage.h>
#include <spd_bin.h>
#include <string.h>

static void mainboard_fill_dq_map_data(void *dq_map_ch0, void *dq_map_ch1)
{
	/* DQ byte map */
	const u8 dq_map[2][12] = {
		  { 0x0F, 0xF0, 0x00, 0xF0, 0x0F, 0xF0,
		    0x0F, 0x00, 0xFF, 0x00, 0xFF, 0x00 },
		  { 0x33, 0xCC, 0x00, 0xCC, 0x33, 0xCC,
		    0x33, 0x00, 0xFF, 0x00, 0xFF, 0x00 } };
	memcpy(dq_map_ch0, dq_map[0], sizeof(dq_map[0]));
	memcpy(dq_map_ch1, dq_map[1], sizeof(dq_map[1]));
}

static void mainboard_fill_dqs_map_data(void *dqs_map_ch0, void *dqs_map_ch1)
{
	/* DQS CPU<>DRAM map */
	const u8 dqs_map[2][8] = {
		{ 0, 1, 3, 2, 4, 5, 6, 7 },
		{ 1, 0, 4, 5, 2, 3, 6, 7 } };
	memcpy(dqs_map_ch0, dqs_map[0], sizeof(dqs_map[0]));
	memcpy(dqs_map_ch1, dqs_map[1], sizeof(dqs_map[1]));
}

static void mainboard_fill_rcomp_res_data(void *rcomp_ptr)
{
	/* Rcomp resistor */
	const u16 RcompResistor[3] = { 121, 81, 100 };
	memcpy(rcomp_ptr, RcompResistor, sizeof(RcompResistor));
}

static void mainboard_fill_rcomp_strength_data(void *rcomp_strength_ptr)
{
	/* Rcomp target */
	const u16 RcompTarget[5] = { 100, 40, 20, 20, 26 };
	memcpy(rcomp_strength_ptr, RcompTarget, sizeof(RcompTarget));
}

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	FSP_M_CONFIG *mem_cfg;
	struct spd_block blk = {
		.addr_map = { 0x50 },
	};

	mem_cfg = &mupd->FspmConfig;

	get_spd_smbus(&blk);
	dump_spd_info(&blk);
	assert(blk.spd_array[0][0] != 0);

	mainboard_fill_dq_map_data(&mem_cfg->DqByteMapCh0,
				   &mem_cfg->DqByteMapCh1);
	mainboard_fill_dqs_map_data(&mem_cfg->DqsMapCpu2DramCh0,
				    &mem_cfg->DqsMapCpu2DramCh1);
	mainboard_fill_rcomp_res_data(&mem_cfg->RcompResistor);
	mainboard_fill_rcomp_strength_data(&mem_cfg->RcompTarget);

	mem_cfg->DqPinsInterleaved = TRUE;
	mem_cfg->MemorySpdDataLen = blk.len;
	mem_cfg->MemorySpdPtr00 = (uintptr_t) blk.spd_array[0];

	/* Enable and set SATA HSIO adjustments for ports 0 and 2 */
	mem_cfg->PchSataHsioRxGen3EqBoostMagEnable[0] = 1;
	mem_cfg->PchSataHsioRxGen3EqBoostMagEnable[2] = 1;
	mem_cfg->PchSataHsioRxGen3EqBoostMag[0] = 1;
	mem_cfg->PchSataHsioRxGen3EqBoostMag[2] = 1;
}
