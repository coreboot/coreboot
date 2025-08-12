/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/mmu.h>
#include <console/console.h>
#include <soc/mmu.h>
#include <soc/mmu_common.h>
#include <soc/symbols_common.h>

/* DRAM region: DDR_SPACE, DDR_SPACE_1 and DDR_SPACE_2 */
#define MAX_DRAM_SPACE_INDEX 3

__weak bool soc_modem_carve_out(void **start, void **end) { return false; }

struct region *qc_get_soc_dram_space_config(size_t ddr_size, int *count)
{
	static struct region config[MAX_DRAM_SPACE_INDEX];
	static int populated_count = 0;
	int i;

	if (!populated_count) {
		void *region_start[MAX_DRAM_SPACE_INDEX] = {
				_dram,
				_dram_space_1,
				_dram_space_2
		};
		size_t size[MAX_DRAM_SPACE_INDEX] = {
				REGION_SIZE(dram),
				REGION_SIZE(dram_space_1),
				REGION_SIZE(dram_space_2)
		};

		/* Handle the case where DDR space is contiguous */
		if (!_edram)
			size[0] = ddr_size;

		for (i = 0; i < MAX_DRAM_SPACE_INDEX && ddr_size > 0; i++) {
			config[i].offset = (size_t)region_start[i];
			size_t to_map = MIN(ddr_size, size[i]);
			config[i].size = to_map;
			ddr_size -= to_map;
		}

		populated_count = i;

		if (ddr_size)
			printk(BIOS_CRIT, "Too much DRAM for available windows (%zu bytes left over)!\n",
				    ddr_size);
	}

	*count = populated_count;

	return config;
}

void qc_mmu_dram_config_post_dram_init(size_t ddr_size)
{
	void *start = NULL;
	void *end = NULL;

	if (soc_modem_carve_out(&start, &end)) {
		if (_dram_space_1)
			die("Using carve out together with DRAM windows not supported");
		mmu_config_range((void *)_dram, start - (void *)_dram, CACHED_RAM);
		mmu_config_range(end, (void *)_dram + ddr_size - end, CACHED_RAM);
	} else {
		int count;
		struct region *config = qc_get_soc_dram_space_config(ddr_size, &count);
		for (int i = 0; i < count; i++)
			mmu_config_range((void *)config[i].offset, config[i].size, CACHED_RAM);
	}

	mmu_config_range((void *)_aop_code_ram, REGION_SIZE(aop_code_ram),
							CACHED_RAM);
	mmu_config_range((void *)_aop_data_ram, REGION_SIZE(aop_data_ram),
							CACHED_RAM);
}
