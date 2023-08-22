/* SPDX-License-Identifier: GPL-2.0-only */

#include <northbridge/intel/sandybridge/raminit.h>
#include <southbridge/intel/bd82x6x/pch.h>

const struct southbridge_usb_port mainboard_usb_ports[] = {
	{ 1, 0, -1 }, /* USB HUB 1 */
	{ 0, 0, -1 }, /* Unused */
	{ 0, 0, -1 }, /* Unused */
	{ 0, 0, -1 }, /* Unused */
	{ 0, 0, -1 }, /* Unused */
	{ 0, 0, -1 }, /* Unused */
	{ 0, 0, -1 }, /* Unused */
	{ 0, 0, -1 }, /* Unused */
	{ 1, 0, -1 }, /* USB HUB 2 */
	{ 1, 0, -1 }, /* Camera */
	{ 0, 0, -1 }, /* Unused */
	{ 0, 0, -1 }, /* Unused */
	{ 0, 0, -1 }, /* Unused */
	{ 0, 0, -1 }, /* Unused */
};

void mb_get_spd_map(struct spd_info *spdi)
{
	spdi->addresses[0] = SPD_MEMORY_DOWN;
	spdi->addresses[2] = SPD_MEMORY_DOWN;
	spdi->spd_index = 0;
}
