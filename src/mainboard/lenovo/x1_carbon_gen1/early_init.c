/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <northbridge/intel/sandybridge/raminit.h>
#include <southbridge/intel/common/gpio.h>

static unsigned int get_spd_index(void)
{
	const int spd_gpio_vector[] = {25, 45, -1};
	unsigned int spd_index = get_gpios(spd_gpio_vector);

	/* 4gb model = 0, 8gb model = 1 */
	/* int extended_memory_version = get_gpio(44); */

	/*
	 * So far there is no need to parse gpio 44, as the 4GiB use
	 * the hynix or elpida memory and 8 GiB versions use samsung.
	 * All version use both channels.
	 * But we might miss some versions.
	 */

	/*
	 * GPIO45 GPIO25
	 * 0      0       elpida
	 * 0      1       hynix
	 * 1      0       samsung
	 * 1      1       reserved
	 */

	if (spd_index == 3)
		die("Unsupported Memory. (detected 'reserved' memory configuration).");

	return spd_index;
}

void mb_get_spd_map(struct spd_info *spdi)
{
	spdi->addresses[0] = SPD_MEMORY_DOWN;
	spdi->addresses[2] = SPD_MEMORY_DOWN;
	spdi->spd_index = get_spd_index();
}
