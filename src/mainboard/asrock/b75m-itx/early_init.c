/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <bootblock_common.h>
#include <southbridge/intel/bd82x6x/pch.h>

const struct southbridge_usb_port mainboard_usb_ports[] = {
	{1, 0, 0}, {1, 0, 0}, {1, 1, 1}, {1, 1, 1}, {1, 1, 2}, {1, 1, 2}, {1, 0, 3},
	{1, 0, 3}, {1, 0, 4}, {1, 0, 4}, {1, 0, 6}, {1, 1, 5}, {1, 1, 5}, {1, 0, 6},
};
