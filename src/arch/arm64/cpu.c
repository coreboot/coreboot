/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <stdint.h>
#include <stdlib.h>
#include <arch/lib_helpers.h>
#include <cpu/cpu.h>
#include <console/console.h>
#include <smp/node.h>
#include "cpu-internal.h"

struct cpu_info cpu_infos[CONFIG_MAX_CPUS];
struct cpu_info *bsp_cpu_info;

struct cpu_info *cpu_info(void)
{
	return cpu_info_for_cpu(smp_processor_id());
}

size_t cpus_online(void)
{
	int i;
	size_t num = 0;

	for (i = 0; i < ARRAY_SIZE(cpu_infos); i++) {
		if (cpu_online(cpu_info_for_cpu(i)))
			num++;
	}

	return num;
}

static inline int action_queue_empty(struct cpu_action_queue *q)
{
	return load_acquire_exclusive(&q->todo) == NULL;
}

static inline int action_completed(struct cpu_action_queue *q,
					struct cpu_action *action)
{
	return load_acquire(&q->completed) == action;
}

static inline void wait_for_action_queue_slot(struct cpu_action_queue *q)
{
	while (!action_queue_empty(q))
		wfe();
}

static void wait_for_action_complete(struct cpu_action_queue *q,
					struct cpu_action *a)
{
	while (!action_completed(q, a))
		wfe();
}

static struct cpu_action *wait_for_action(struct cpu_action_queue *q,
						struct cpu_action *local)
{
	struct cpu_action *action;

	while (action_queue_empty(q))
		wfe();

	/*
	 * Keep original address, but use a local copy for async processing.
	 */
	do {
		action = load_acquire_exclusive(&q->todo);
		*local = *action;
	} while (!store_release_exclusive(&q->todo, NULL));

	return action;
}

static void queue_action(struct cpu_action_queue *q, struct cpu_action *action)
{
	do {
		wait_for_action_queue_slot(q);
		if (load_acquire_exclusive(&q->todo) != NULL)
			continue;
	} while (!store_release_exclusive(&q->todo, action));
}

static void action_queue_complete(struct cpu_action_queue *q,
					struct cpu_action *action)
{
	/* Mark completion and send events to waiters. */
	store_release(&q->completed, action);
	sev();
}

static void action_run(struct cpu_action *action)
{
	action->run(action->arg);
}

static void action_run_on_cpu(struct cpu_info *ci, struct cpu_action *action,
				int sync)
{
	struct cpu_action_queue *q = &ci->action_queue;

	/* Don't run actions on non-online cpus. */
	if (!cpu_online(ci))
		return;

	if (ci->id == smp_processor_id()) {
		action->run(action->arg);
		return;
	}

	queue_action(q, action);
	/* Wait for CPU to pick it up. Empty slot means it was picked up. */
	wait_for_action_queue_slot(q);
	/* Wait for completion if requested. */
	if (sync)
		wait_for_action_complete(q, action);
}

static int __arch_run_on_cpu(unsigned int cpu, struct cpu_action *action,
				int sync)
{
	struct cpu_info *ci;

	if (cpu >= CONFIG_MAX_CPUS)
		return -1;

	ci = cpu_info_for_cpu(cpu);

	action_run_on_cpu(ci, action, sync);

	return 0;
}

int arch_run_on_cpu(unsigned int cpu, struct cpu_action *action)
{
	return __arch_run_on_cpu(cpu, action, 1);
}

int arch_run_on_cpu_async(unsigned int cpu, struct cpu_action *action)
{
	return __arch_run_on_cpu(cpu, action, 0);
}

static int __arch_run_on_all_cpus(struct cpu_action *action, int sync)
{
	int i;

	for (i = 0; i < CONFIG_MAX_CPUS; i++)
		action_run_on_cpu(cpu_info_for_cpu(i), action, sync);

	return 0;
}

static int __arch_run_on_all_cpus_but_self(struct cpu_action *action, int sync)
{
	int i;
	struct cpu_info *me = cpu_info();

	for (i = 0; i < CONFIG_MAX_CPUS; i++) {
		struct cpu_info *ci = cpu_info_for_cpu(i);
		if (ci == me)
			continue;
		action_run_on_cpu(ci, action, sync);
	}

	return 0;
}

int arch_run_on_all_cpus(struct cpu_action *action)
{
	return __arch_run_on_all_cpus(action, 1);
}

int arch_run_on_all_cpus_async(struct cpu_action *action)
{
	return __arch_run_on_all_cpus(action, 0);
}

int arch_run_on_all_cpus_but_self(struct cpu_action *action)
{
	return __arch_run_on_all_cpus_but_self(action, 1);
}

int arch_run_on_all_cpus_but_self_async(struct cpu_action *action)
{
	return __arch_run_on_all_cpus_but_self(action, 0);
}


void arch_cpu_wait_for_action(void)
{
	struct cpu_info *ci = cpu_info();
	struct cpu_action_queue *q = &ci->action_queue;

	while (1) {
		struct cpu_action *orig;
		struct cpu_action action;

		orig = wait_for_action(q, &action);

		action_run(&action);
		action_queue_complete(q, orig);
	}
}

#if IS_ENABLED(CONFIG_SMP)
int boot_cpu(void)
{
	return cpu_is_bsp();
}
#endif
