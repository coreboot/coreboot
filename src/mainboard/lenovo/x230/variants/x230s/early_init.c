/* SPDX-License-Identifier: GPL-2.0-only */

#include <northbridge/intel/sandybridge/raminit_native.h>
#include <southbridge/intel/bd82x6x/pch.h>

const struct southbridge_usb_port mainboard_usb_ports[] = {
	{1, 3, 0},  /* SSP1: Right */
	{1, 3, 1},  /* SSP2: Left, EHCI Debug */
	{0, 1, 3},  /* SSP3 */
	{1, 3, -1}, /* B0P4: WWAN USB */
	{0, 1, 2},  /* B0P5 */
	{0, 1, -1}, /* B0P6 */
	{0, 1, -1}, /* B0P7 */
	{0, 1, -1}, /* B0P8 */
	{0, 1, -1}, /* B1P1 */
	{0, 1, 5},  /* B1P2 */
	{1, 1, -1}, /* B1P3: Fingerprint Reader */
	{0, 1, -1}, /* B1P4 */
	{1, 3, -1}, /* B1P5: WLAN USB */
	{1, 1, -1}, /* B1P6: Camera */
};

void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
	read_spd(&spd[0], 0x50, id_only);
}
