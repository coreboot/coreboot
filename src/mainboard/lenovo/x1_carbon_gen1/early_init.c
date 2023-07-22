/* SPDX-License-Identifier: GPL-2.0-only */

#include <string.h>
#include <console/console.h>
#include <northbridge/intel/sandybridge/raminit.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/common/gpio.h>

const struct southbridge_usb_port mainboard_usb_ports[] = {
	/* enabled, current, OC pin */
	{ 0, 3, 0 }, /* P00 disconnected */
	{ 1, 1, 1 }, /* P01 left or right */
	{ 0, 1, 3 }, /* P02 disconnected */
	{ 1, 3, -1 },/* P03 WWAN */
	{ 0, 1, 2 }, /* P04 disconnected */
	{ 0, 1, -1 },/* P05 disconnected */
	{ 0, 1, -1 },/* P06 disconnected */
	{ 0, 2, -1 },/* P07 disconnected */
	{ 0, 1, -1 },/* P08 disconnected */
	{ 1, 2, 5 }, /* P09 left or right */
	{ 1, 3, -1 },/* P10 FPR */
	{ 1, 3, -1 },/* P11 Bluetooth */
	{ 1, 1, -1 },/* P12 WLAN */
	{ 1, 1, -1 },/* P13 Camera */
};

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
