/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <console/console.h>
#include <device/dram/ddr2.h>
#include <device/dram/ddr3.h>
#include <device/smbus_host.h>
#include <lib.h>
#include <memory_info.h>
#include <spd.h>

#include "gm45.h"
#include "chip.h"

static u8 get_dimm_mod_type(const sysinfo_t *sysinfo, const int idx)
{
	if (sysinfo->spd_type == DDR2) {
		return smbus_read_byte(sysinfo->spd_map[idx], 20) & SPD_DDR2_DIMM_TYPE_MASK;
	} else {
		return smbus_read_byte(sysinfo->spd_map[idx], 3) & 0xf;
	}
}

static void ddr3_read_ids(const sysinfo_t *sysinfo, struct dimm_info *dimm, const int idx)
{
	const u8 addr = sysinfo->spd_map[idx];
	for (int k = 0; k < SPD_DDR3_SERIAL_LEN; k++) {
		dimm->serial[k] = smbus_read_byte(addr, SPD_DDR3_SERIAL_NUM + k);
	}
	for (int k = 0; k < SPD_DDR3_PART_LEN; k++) {
		dimm->module_part_number[k] = smbus_read_byte(addr, SPD_DDR3_PART_NUM + k);
	}
	dimm->mod_id = (smbus_read_byte(addr, SPD_DDR3_MOD_ID2) << 8) |
		       (smbus_read_byte(addr, SPD_DDR3_MOD_ID1) << 0);
}

static u32 get_mem_clock_mt(const int clock_index)
{
	switch (clock_index) {
	case MEM_CLOCK_1067MT:	return 1067;
	case MEM_CLOCK_800MT:	return 800;
	case MEM_CLOCK_667MT:	return 667;
	default:		return 0;
	}
}

void setup_sdram_meminfo(const sysinfo_t *sysinfo)
{
	struct memory_info *mem_info = cbmem_add(CBMEM_ID_MEMINFO, sizeof(struct memory_info));
	if (!mem_info)
		die("Failed to add memory info to CBMEM.\n");

	memset(mem_info, 0, sizeof(struct memory_info));

	const u16 ddr_type = (sysinfo->spd_type == DDR2) ? MEMORY_TYPE_DDR2 : MEMORY_TYPE_DDR3;
	const u32 ddr_freq_mt = get_mem_clock_mt(sysinfo->selected_timings.mem_clock);

	int dimm_cnt = 0;

	int ch;
	FOR_EACH_POPULATED_CHANNEL(sysinfo->dimms, ch) {
		struct dimm_info *dimm = &mem_info->dimm[dimm_cnt];
		const int idx = ch * 2;
		const int ranks = sysinfo->dimms[ch].ranks;
		dimm->dimm_size = (256 << sysinfo->dimms[ch].chip_capacity) * ranks;
		dimm->ddr_type = ddr_type;
		dimm->ddr_frequency = ddr_freq_mt;
		dimm->rank_per_dimm = ranks;
		dimm->channel_num = ch;
		dimm->dimm_num = 0;
		dimm->bank_locator = ch;
		/* TODO: Handle DDR2 SPDs */
		if (sysinfo->spd_type == DDR3) {
			ddr3_read_ids(sysinfo, dimm, idx);
		}
		dimm->mod_type = get_dimm_mod_type(sysinfo, idx);
		dimm->bus_width = MEMORY_BUS_WIDTH_64;
		dimm_cnt++;
	}
	mem_info->dimm_cnt = dimm_cnt;

	mem_info->ecc_type = MEMORY_ARRAY_ECC_NONE;
	mem_info->max_capacity_mib = 8192;
	mem_info->number_of_devices = 2;
}
