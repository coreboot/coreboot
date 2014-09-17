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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#include <stdint.h>
#include <stdlib.h>
#include <arch/barrier.h>
#include <arch/lib_helpers.h>
#include <cpu/cpu.h>
#include <console/console.h>
#include <gic.h>
#include "cpu-internal.h"

static struct cpu_info cpu_infos[CONFIG_MAX_CPUS];

static inline struct cpu_info *cpu_info_for_cpu(unsigned int id)
{
	return &cpu_infos[id];
}

struct cpu_info *cpu_info(void)
{
	return cpu_info_for_cpu(smp_processor_id());
}

static int cpu_online(struct cpu_info *ci)
{
	return load_acquire(&ci->online) != 0;
}

static void cpu_mark_online(struct cpu_info *ci)
{
	store_release(&ci->online, 1);
}

static inline void cpu_disable_dev(device_t dev)
{
	dev->enabled = 0;
}

static struct cpu_driver *locate_cpu_driver(uint32_t midr)
{
	struct cpu_driver *cur;

	for (cur = cpu_drivers; cur != ecpu_drivers; cur++) {
		const struct cpu_device_id *id_table = cur->id_table;

		for (; id_table->midr != CPU_ID_END; id_table++) {
			if (id_table->midr == midr)
				return cur;
		}
	}
	return NULL;
}

static int cpu_set_device_operations(device_t dev)
{
	uint32_t midr;
	struct cpu_driver *driver;

	midr = raw_read_midr_el1();
	driver = locate_cpu_driver(midr);

	if (driver == NULL) {
		printk(BIOS_WARNING, "No CPU driver for MIDR %08x\n", midr);
		return -1;
	}
	dev->ops = driver->ops;
	return 0;
}

/* Set up default SCR values. */
static void el3_init(void)
{
	uint32_t scr;

	if (get_current_el() != EL3)
		return;

	scr = raw_read_scr_el3();
	/* Default to non-secure EL1 and EL0. */
	scr &= ~(SCR_NS_MASK);
	scr |= SCR_NS_ENABLE;
	/* Disable IRQ, FIQ, and external abort interrupt routing. */
	scr &= ~(SCR_IRQ_MASK | SCR_FIQ_MASK | SCR_EA_MASK);
	scr |= SCR_IRQ_DISABLE | SCR_FIQ_DISABLE | SCR_EA_DISABLE;
	/* Enable HVC */
	scr &= ~(SCR_HVC_MASK);
	scr |= SCR_HVC_ENABLE;
	/* Disable SMC */
	scr &= ~(SCR_SMC_MASK);
	scr |= SCR_SMC_DISABLE;
	/* Disable secure instruction fetches. */
	scr &= ~(SCR_SIF_MASK);
	scr |= SCR_SIF_DISABLE;
	/* All lower exception levels 64-bit by default. */
	scr &= ~(SCR_RW_MASK);
	scr |= SCR_LOWER_AARCH64;
	/* Disable secure EL1 access to secure timer. */
	scr &= ~(SCR_ST_MASK);
	scr |= SCR_ST_DISABLE;
	/* Don't trap on WFE or WFI instructions. */
	scr &= ~(SCR_TWI_MASK | SCR_TWE_MASK);
	scr |= SCR_TWI_DISABLE | SCR_TWE_DISABLE;
	raw_write_scr_el3(scr);
	isb();
}

static void init_this_cpu(void *arg)
{
	struct cpu_info *ci = arg;
	device_t dev = ci->cpu;

	cpu_set_device_operations(dev);

	el3_init();

	/* Initialize the GIC. */
	gic_init();

	if (dev->ops != NULL && dev->ops->init != NULL) {
		dev->initialized = 1;
		printk(BIOS_DEBUG, "%s init\n", dev_path(dev));
		dev->ops->init(dev);
	}
}

/* Fill in cpu_info structures according to device tree. */
static void init_cpu_info(struct bus *bus)
{
	device_t cur;

	for (cur = bus->children; cur != NULL; cur = cur->sibling) {
		struct cpu_info *ci;
		unsigned int id = cur->path.cpu.id;

		if (cur->path.type != DEVICE_PATH_CPU)
			continue;

		/* IDs are currently mapped 1:1 with logical CPU numbers. */
		if (id >= CONFIG_MAX_CPUS) {
			printk(BIOS_WARNING,
				"CPU id %x too large. Disabling.\n", id);
			cpu_disable_dev(cur);
			continue;
		}

		ci = cpu_info_for_cpu(id);
		if (ci->cpu != NULL) {
			printk(BIOS_WARNING,
				"Duplicate ID %x in device tree.\n", id);
			cpu_disable_dev(cur);
		}

		ci->cpu = cur;
		ci->id = cur->path.cpu.id;
	}

	/* Mark current cpu online. */
	cpu_mark_online(cpu_info());
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

	/* Don't run actions on non-online or enabled devices. */
	if (!cpu_online(ci) || ci->cpu == NULL || !ci->cpu->enabled)
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

void arch_secondary_cpu_init(void)
{
	struct cpu_info *ci = cpu_info();
	struct cpu_action_queue *q = &ci->action_queue;

	/* Mark this CPU online. */
	cpu_mark_online(ci);

	while (1) {
		struct cpu_action *orig;
		struct cpu_action action;

		orig = wait_for_action(q, &action);

		action_run(&action);
		action_queue_complete(q, orig);
	}
}

void arch_initialize_cpus(device_t cluster, struct cpu_control_ops *cntrl_ops)
{
	size_t max_cpus;
	size_t i;
	struct cpu_info *ci;
	void (*entry)(void);
	struct bus *bus;

	if (cluster->path.type != DEVICE_PATH_CPU_CLUSTER) {
		printk(BIOS_ERR,
			"CPU init failed. Device is not a CPU_CLUSTER: %s\n",
			dev_path(cluster));
		return;
	}

	bus = cluster->link_list;

	/* Check if no children under this device. */
	if (bus == NULL)
		return;

	entry = prepare_secondary_cpu_startup();

	/* Initialize the cpu_info structures. */
	init_cpu_info(bus);
	max_cpus = cntrl_ops->total_cpus();

	if (max_cpus > CONFIG_MAX_CPUS) {
		printk(BIOS_WARNING,
			"max_cpus (%zu) exceeds CONFIG_MAX_CPUS (%zu).\n",
			max_cpus, (size_t)CONFIG_MAX_CPUS);
		max_cpus = CONFIG_MAX_CPUS;
	}

	for (i = 0; i < max_cpus; i++) {
		device_t dev;
		struct cpu_action action;

		ci = cpu_info_for_cpu(i);
		dev = ci->cpu;

		/* Disregard CPUs not in device tree. */
		if (dev == NULL)
			continue;

		/* Skip disabled CPUs. */
		if (!dev->enabled)
			continue;

		if (!cpu_online(ci)) {
			/* Start the CPU. */
			printk(BIOS_DEBUG, "Starting CPU%x\n", ci->id);
			if (cntrl_ops->start_cpu(ci->id, entry)) {
				printk(BIOS_ERR,
					"Failed to start CPU%x\n", ci->id);
				continue;
			}
			/* Wait for CPU to come online. */
			while (!cpu_online(ci));
			printk(BIOS_DEBUG, "CPU%x online.\n", ci->id);
		}

		/* Send it the init action. */
		action.run = init_this_cpu;
		action.arg = ci;
		action_run_on_cpu(ci, &action, 1);
	}
}
