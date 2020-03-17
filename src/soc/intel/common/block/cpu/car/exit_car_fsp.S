/*
 * This file is part of the coreboot project.
 *
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
 * after the API call.  More info can be found in the FSP Integration
 * Guide included with the FSP binary.
 */

.text
.global chipset_teardown_car
chipset_teardown_car:

	/* Set up new stack. */
	mov	post_car_stack_top, %esp

	/* Call C code */
	call	main
