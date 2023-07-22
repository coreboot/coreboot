/* SPDX-License-Identifier: GPL-2.0-only */

#include <northbridge/intel/sandybridge/raminit.h>
#include <southbridge/intel/bd82x6x/pch.h>

const struct southbridge_usb_port mainboard_usb_ports[] = {
	{ 1, 0, 0 },  /* SSP1: right */
	{ 1, 0, 1 },  /* SSP2: left, EHCI Debug */
	{ 1, 1, 3 },  /* SSP3: dock USB3 */
	{ 1, 1, -1 }, /* B0P4: wwan USB */
	{ 1, 1, 2 },  /* B0P5: dock USB2 */
	{ 0, 0, -1 }, /* B0P6 */
	{ 0, 0, -1 }, /* B0P7 */
	{ 1, 2, -1 }, /* B0P8: unknown */
	{ 1, 0, -1 }, /* B1P1: smart card reader */
	{ 0, 2, 5 },  /* B1P2 */
	{ 1, 1, -1 }, /* B1P3: fingerprint reader */
	{ 0, 0, -1 }, /* B1P4 */
	{ 1, 1, -1 }, /* B1P5: wlan USB */
	{ 1, 1, -1 }, /* B1P6: Camera */
};

void mb_get_spd_map(struct spd_info *spdi)
{
	/* C1S0 is a soldered RAM with no real SPD. Use stored SPD. */
	spdi->addresses[0] = SPD_MEMORY_DOWN;
	spdi->addresses[2] = 0x51;
	spdi->spd_index = 0;
}
