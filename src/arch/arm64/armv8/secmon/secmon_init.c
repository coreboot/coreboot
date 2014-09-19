/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <arch/barrier.h>
#include <arch/io.h>
#include <arch/exception.h>
#include <arch/lib_helpers.h>
#include <arch/secmon.h>
#include <arch/smc.h>
#include <arch/transition.h>
#include <console/console.h>
#include <rmodule.h>
#include <stddef.h>
#include "secmon.h"

static void cpu_init(int bsp)
{
	struct cpu_info *ci = cpu_info();

	ci->id = smp_processor_id();
	cpu_mark_online(ci);

	if (bsp)
		cpu_set_bsp();
}

static void secmon_init(struct secmon_params *params, int bsp)
{
	struct exc_state exc_state;

	exception_hwinit();
	cpu_init(bsp);

	smc_init();

	/*
	 * Check if the arg is non-NULL
	 * 1) If yes, we make an EL2 transition to that entry point
	 * 2) If no, we just wait
	 */
	if (params != NULL) {
		memset(&exc_state, 0, sizeof(exc_state));
		exc_state.elx.spsr =
			get_eret_el(params->elx_el, params->elx_mode);

		transition_with_entry(params->entry, params->arg, &exc_state);
	}
	secmon_wait_for_action();
}

void secmon_wait_for_action(void)
{
	arch_cpu_wait_for_action();
}

static void secmon_init_bsp(void *arg)
{
	secmon_init(arg, 1);
}

static void secmon_init_nonbsp(void *arg)
{
	secmon_init(arg, 0);
}

/*
 * This variable holds entry point for secmon init code. Once the stacks are
 * setup by the stage_entry.S, it jumps to c_entry.
 */
void (*c_entry[2])(void*) = { &secmon_init_bsp, &secmon_init_nonbsp };
