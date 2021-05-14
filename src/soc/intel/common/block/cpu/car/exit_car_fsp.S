/* SPDX-License-Identifier: GPL-2.0-only */

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
	mov	_estack, %esp
	/* Align the stack. */
	andl	$0xfffffff0, %esp

	/* Call C code */
	call	main
