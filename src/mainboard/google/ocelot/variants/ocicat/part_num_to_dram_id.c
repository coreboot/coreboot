/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>

struct part_num_spd_index {
	const char part_num[DIMM_INFO_PART_NUMBER_SIZE];
	int spd_index;
};

static const struct part_num_spd_index part_number_map[] = {
	{ "H58G56BK8BX068", 0 },
	{ "H58G66BK7BX067", 1 },
	{ "H58G66BK8BX067", 2 },
	{ "H58GE6AK8BX104", 3 },
	{ "H58G66CK8BX147", 2 },
};

int variant_memory_sku(void)
{
	int index;
	size_t num_elements = ARRAY_SIZE(part_number_map);
	const char *part_num = mainboard_get_dram_part_num();

	if (part_num == NULL)
		die("No part number in CBI, halting boot.\n");

	for (index = 0; index < num_elements; index++) {
		if (!strcmp(part_num, part_number_map[index].part_num))
			/* exact match found */
			return part_number_map[index].spd_index;
	}

	/* We didn't find a match for part_num, halt boot */
	die("No part number found looking for %s, halting boot.\n", part_num);
}
