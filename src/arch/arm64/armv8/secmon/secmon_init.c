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
#include <arch/lib_helpers.h>
#include <arch/secmon.h>
#include <arch/transition.h>
#include <console/console.h>
#include <rmodule.h>
#include <stddef.h>

static void secmon_wait(void)
{
	/*
	 * TODO(furquan): This should be a point of no-return. Once we have PSCI
	 * support we need to respond to kernel calls
	 */
	while (1)
		wfe();
}

static void secmon_el3_init(void)
{
	uint32_t scr;

	scr = raw_read_scr_el3();

	/* Enable SMC */
	scr &= ~(SCR_SMC_MASK);
	scr |= SCR_SMC_ENABLE;

	raw_write_scr_el3(scr);
	isb();
}

static void secmon_init(void *arg)
{
	struct exc_state exc_state;
	struct secmon_params *params = arg;

	printk(BIOS_DEBUG, "ARM64: secmon in %s\n", __func__);

	secmon_el3_init();

	/*
	 * Check if the arg is non-NULL
	 * 1) If yes, we make an EL2 transition to that entry point
	 * 2) If no, we just wait
	 */
	if (params == NULL) {
		secmon_wait();
	}

	memset(&exc_state, 0, sizeof(exc_state));
	exc_state.elx.spsr = get_eret_el(params->elx_el, params->elx_mode);

	transition_with_entry(params->entry, params->arg, &exc_state);
}

/*
 * This variable holds entry point for secmon init code. Once the stacks are
 * setup by the stage_entry.S, it jumps to c_entry.
 */
void (*c_entry)(void*) = &secmon_init;
