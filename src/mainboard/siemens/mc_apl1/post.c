/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <nc_fpga.h>
#include <types.h>

void mainboard_post(uint8_t value)
{
	nc_fpga_post(value);
}
