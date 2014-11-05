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
#include <arch/psci.h>
#include <arch/secmon.h>
#include <arch/smc.h>
#include <console/console.h>
#include <stddef.h>
#include "secmon.h"

/* Save initial secmon params per CPU to handle turn up. */
static struct secmon_params *init_params[CONFIG_MAX_CPUS];

static void start_up_cpu(void *arg)
{
	struct secmon_params *params = init_params[cpu_info()->id];

	if (params == NULL)
		psci_turn_off_self();
	psci_turn_on_self(params->entry, params->arg);
}

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
	struct cpu_action action = {
		.run = start_up_cpu,
	};

	exception_hwinit();
	cpu_init(bsp);

	init_params[cpu_info()->id] = params;

	if (!cpu_is_bsp())
		secmon_wait_for_action();

	smc_init();
	psci_init();

	/* Make sure all non-BSP CPUs take action before the BSP. */
	arch_run_on_all_cpus_but_self_async(&action);
	/* Turn on BSP. */
	start_up_cpu(NULL);

	printk(BIOS_ERR, "CPU turn on failed for BSP.\n");
	while (1)
		;
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
