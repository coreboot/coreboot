/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Intel GM965 (Crestline) - SMBIOS memory information setup
 *
 * Populates CBMEM memory_info structure for SMBIOS Type 16 and Type 17
 * based on the DIMM configuration discovered during raminit.
 */

#include <string.h>
#include <cbmem.h>
#include <console/console.h>
#include <device/dram/ddr2.h>
#include <device/smbus_host.h>
#include <lib.h>
#include <memory_info.h>
#include <northbridge/intel/gm965/gm965.h>
#include <spd.h>
#include <smbios.h>

static u16 get_mem_clock_mt(mem_clock_t mc)
{
	switch (mc) {
	case MEM_CLOCK_533MT: return 533;
	case MEM_CLOCK_667MT: return 667;
	default: return 0;
	}
}

/* Extract SPD identification fields (IDs, part number, voltage) from cached raw SPD */
static void ddr2_extract_ids(const sysinfo_t *si, struct dimm_info *dimm, int slot)
{
	const u8 *spd = si->raw_spd[slot];
	if (!spd[0])  /* zeroed means no DIMM */
		return;

	/* Serial number (SPD bytes 95-98) */
	for (int i = 0; i < 4; i++) {
		dimm->serial[i] = spd[SPD_ASSEMBLY_SERIAL_NUMBER + i];
	}

	/* Part number (SPD bytes 73-88) - 16 ASCII bytes, null-terminated */
	char part[17];
	for (int i = 0; i < 16; i++) {
		part[i] = spd[SPD_MANUFACTURER_PART_NUMBER + i];
	}
	part[16] = '\0';
	strncpy((char *)dimm->module_part_number, part, DIMM_INFO_PART_NUMBER_SIZE);
	dimm->module_part_number[DIMM_INFO_PART_NUMBER_SIZE - 1] = '\0';

	/* Manufacturer ID from SPD bytes 64-65 (little-endian 16-bit) */
	dimm->mod_id = (u16)spd[64] | ((u16)spd[65] << 8);

	/* Module type from SPD byte 20 */
	dimm->mod_type = spd[20] & SPD_DDR2_DIMM_TYPE_MASK;

	/* DDR2 operates at 1.8V; just set the standard voltage */
	if (spd[8] != 5)
		printk(BIOS_WARNING, "DIMM%d SPD voltage byte (0x%02x) doesn't indicate 1.8V\n",
		       slot, spd[8]);
	dimm->vdd_voltage = 1800;
	dimm->vdd_min_voltage = 1800;
	dimm->vdd_max_voltage = 1800;
}

/* Fill CBMEM memory_info for SMBIOS Type 16/17 */
void setup_sdram_meminfo(const sysinfo_t *si)
{
	struct memory_info *mem_info = cbmem_add(CBMEM_ID_MEMINFO, sizeof(*mem_info));
	if (!mem_info) {
		printk(BIOS_ERR, "Failed to allocate memory info CBMEM\n");
		return;
	}
	memset(mem_info, 0, sizeof(*mem_info));

	/* System-level memory array information */
	mem_info->ecc_type = MEMORY_ARRAY_ECC_NONE;
	mem_info->max_capacity_mib = 4096; /* GM965 max = 4 GB */
	mem_info->number_of_devices = 2;   /* Two DIMM slots */

	const u16 ddr_freq_mt = get_mem_clock_mt(si->timings.mem_clock);

	int dimm_cnt = 0;

	for (int slot = 0; slot < 4; slot++) {
		if (!si->dimms[slot].present)
			continue;

		struct dimm_info *dimm = &mem_info->dimm[dimm_cnt];
		const int ch = slot / 2;
		const int ranks = si->dimms[slot].dual_rank ? 2 : 1;

		/* Capacity from SPD (size of one rank * number of ranks) */
		dimm->dimm_size = si->dimms[slot].rank_capacity_mb * ranks;

		dimm->ddr_type = MEMORY_TYPE_DDR2;
		dimm->ddr_frequency = ddr_freq_mt;  /* deprecated but set */
		dimm->configured_speed_mts = ddr_freq_mt;
		dimm->max_speed_mts = 0;  /* Unknown */
		dimm->rank_per_dimm = ranks;
		dimm->channel_num = ch;
		dimm->dimm_num = slot % 2;
		dimm->bank_locator = ch;

		/* GM965 only supports 64-bit non-ECC modules */
		dimm->bus_width = MEMORY_BUS_WIDTH_64;

		/* Extract identification fields from cached raw SPD */
		ddr2_extract_ids(si, dimm, slot);

		/* DDR2 SO-DIMMs are unbuffered and synchronous */
		dimm->type_detail = MEMORY_TYPE_DETAIL_SYNCHRONOUS
				  | MEMORY_TYPE_DETAIL_UNBUFFERED;

		dimm_cnt++;
	}

	mem_info->dimm_cnt = dimm_cnt;

	printk(BIOS_DEBUG, "SMBIOS memory info: %d DIMMs, max capacity %d MB\n",
	       dimm_cnt, mem_info->max_capacity_mib);
}
