/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <southbridge/intel/bd82x6x/pch.h>

const struct southbridge_usb_port mainboard_usb_ports[] = {
	{ 1, 0, 0 },
	{ 1, 1, 1 },
	{ 1, 1, 3 },
	{ 1, 1, 3 },
	{ 1, 1, -1 },
	{ 1, 1, -1 },
	{ 1, 0, 2 },
	{ 1, 0, 2 },
	{ 1, 1, 6 },
	{ 1, 1, 5 },
	{ 1, 1, 6 },
	{ 1, 1, 6 },
	{ 1, 1, 7 },
	{ 1, 1, 6 },
};
