/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <cpu/x86/mtrr.h>
#include <cpu/x86/cr.h>

/*
 * This path for CAR teardown is taken when CONFIG_FSP_CAR is employed.
 * This version of chipset_teardown_car sets up the stack, then bypasses
 * the rest of arch/x86/exit_car.S and calls main() itself instead of
 * returning to _start. In main(), the TempRamExit FSP API is called
 * to tear down the CAR and set up caching which can be overwritten
 * after the API call.  More info can be found in the Apollo Lake FSP
 * Integration Guide included with the FSP binary.  The below
 * caching settings are based on an 8MiB Flash Size given as a
 * parameter to TempRamInit.
 *
 *	TempRamExit MTRR Settings:
 *	0x00000000  - 0x0009FFFF           | Write Back
 *	0x000C0000  - Top of Low Memory    | Write Back
 *	0xFF800000  - 0xFFFFFFFF Flash Reg | Write Protect
 *	0x100000000 - Top of High Memory   | Write Back
 */

.text
.global chipset_teardown_car
chipset_teardown_car:

	/* Set up new stack. */
	mov	post_car_stack_top, %esp

	/* Call C code */
	call	main
