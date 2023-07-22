/* SPDX-License-Identifier: GPL-2.0-only */

#include <southbridge/intel/bd82x6x/pch.h>

const struct southbridge_usb_port mainboard_usb_ports[] = {
	{1, 1, 0},	/* P0: USB 3.0 1 (OC0) */
	{1, 1, 0},	/* P1: USB 3.0 2 (OC0) */
	{0, 0, 0},
	{1, 1, -1},	/* P3: Camera (no OC) */
	{1, 0, -1},	/* P4: WLAN (no OC) */
	{1, 0, -1},	/* P5: WWAN (no OC) */
	{0, 0, 0},
	{0, 0, 0},
	{0, 0, 0},
	{1, 1, 4},	/* P9: USB 2.0 (AUO4) (OC4) */
	{0, 0, 0},
	{0, 0, 0},
	{0, 0, 0},
	{1, 0, -1},	/* P13: Bluetooth (no OC) */
};
