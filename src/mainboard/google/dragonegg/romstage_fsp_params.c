/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <baseboard/variants.h>
#include <cbfs.h>
#include <console/console.h>
#include <soc/romstage.h>

static uintptr_t mainboard_get_spd_data(void)
{
	char *spd_file;
	size_t spd_file_len;
	int spd_index;
	const size_t spd_len = CONFIG_DIMM_SPD_SIZE;
	const char *spd_bin = "spd.bin";

	spd_index = variant_memory_sku();
	assert(spd_index >= 0);
	printk(BIOS_INFO, "SPD index %d\n", spd_index);

	/* Load SPD data from CBFS */
	spd_file = cbfs_boot_map_with_leak(spd_bin, CBFS_TYPE_SPD,
					   &spd_file_len);
	if (!spd_file)
		die("SPD data not found.");

	/* make sure we have at least one SPD in the file. */
	if (spd_file_len < spd_len)
		die("Missing SPD data.");

	/* Make sure we did not overrun the buffer */
	if (spd_file_len < ((spd_index + 1) * spd_len))
		die("Invalid SPD index.");

	spd_index *= spd_len;

	return (uintptr_t)(spd_file + spd_index);
}

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	FSP_M_CONFIG *mem_cfg = &mupd->FspmConfig;
	struct lpddr4_config mem_params;

	memset(&mem_params, 0, sizeof(mem_params));
	variant_memory_params(&mem_params);

	if (mem_params.dq_map && mem_params.dq_map_size)
		memcpy(&mem_cfg->DqByteMapCh0, mem_params.dq_map,
				mem_params.dq_map_size);

	if (mem_params.dqs_map && mem_params.dqs_map_size)
		memcpy(&mem_cfg->DqsMapCpu2DramCh0, mem_params.dqs_map,
				mem_params.dqs_map_size);

	memcpy(&mem_cfg->RcompResistor, mem_params.rcomp_resistor,
		mem_params.rcomp_resistor_size);

	memcpy(&mem_cfg->RcompTarget, mem_params.rcomp_target,
			mem_params.rcomp_target_size);

	mem_cfg->MemorySpdPtr00 = mainboard_get_spd_data();
	mem_cfg->MemorySpdPtr10 = mem_cfg->MemorySpdPtr00;
	mem_cfg->MemorySpdDataLen = CONFIG_DIMM_SPD_SIZE;
	mem_cfg->DqPinsInterleaved = 0;
	mem_cfg->CaVrefConfig = 0x2;
	mem_cfg->ECT = 1; /* Early Command Training Enabled */
	mem_cfg->RefClk = 0; /* Auto Select CLK freq */
	mem_cfg->SpdAddressTable[0] = 0x0;
	mem_cfg->SpdAddressTable[1] = 0x0;
	mem_cfg->SpdAddressTable[2] = 0x0;
	mem_cfg->SpdAddressTable[3] = 0x0;
}
