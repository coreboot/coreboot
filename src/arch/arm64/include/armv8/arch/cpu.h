/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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

#ifndef __ARCH_CPU_H__
#define __ARCH_CPU_H__

#define asmlinkage

#if !defined(__PRE_RAM__)
#include <arch/barrier.h>
#include <arch/mpidr.h>
#include <device/device.h>

enum {
	CPU_ID_END = 0x00000000,
};

struct cpu_device_id {
	uint32_t midr;
};

struct cpu_driver {
	/* This is excessive as init() is the only one called. */
	struct device_operations *ops;
	const struct cpu_device_id *id_table;
};

/* Action to run. */
struct cpu_action {
	void (*run)(void *arg);
	void *arg;
};

/*
 * Actions are queued to 'todo'. When picked up 'todo' is cleared. The
 * 'completed' field is set to the original 'todo' value when the action
 * is complete.
 */
struct cpu_action_queue {
	struct cpu_action *todo;
	struct cpu_action *completed;
};

struct cpu_info {
	device_t cpu;
	struct cpu_action_queue action_queue;
	unsigned int online;
	/* Current assumption is that id matches smp_processor_id(). */
	unsigned int id;
	uint64_t mpidr;
};

/* Obtain cpu_info for current executing CPU. */
struct cpu_info *cpu_info(void);

extern struct cpu_info *bsp_cpu_info;
extern struct cpu_info cpu_infos[CONFIG_MAX_CPUS];

static inline struct cpu_info *cpu_info_for_cpu(unsigned int id)
{
	return &cpu_infos[id];
}

/* Ran only by BSP at initial boot strapping. */
static inline void cpu_set_bsp(void)
{
	bsp_cpu_info = cpu_info();
}

static inline int cpu_is_bsp(void)
{
	return cpu_info() == bsp_cpu_info;
}

static inline int cpu_online(struct cpu_info *ci)
{
	return load_acquire(&ci->online) != 0;
}

static inline void cpu_mark_online(struct cpu_info *ci)
{
	ci->mpidr = read_affinity_mpidr();
	store_release(&ci->online, 1);
}

/* Provide number of CPUs online. */
size_t cpus_online(void);

/* Control routines for starting CPUs. */
struct cpu_control_ops {
	/* Return the maximum number of CPUs supported. */
	size_t (*total_cpus)(void);
	/*
	 * Start the requested CPU and have it start running entry().
	 * Returns 0 on success, < 0 on error.
	 */
	int (*start_cpu)(unsigned int id, void (*entry)(void));
};

/*
 * Initialize all DEVICE_PATH_CPUS under the DEVICE_PATH_CPU_CLUSTER cluster.
 * type DEVICE_PATH_CPUS. Start up is controlled by cntrl_ops.
 */
void arch_initialize_cpus(device_t cluster, struct cpu_control_ops *cntrl_ops);

/*
 * Run cpu_action returning < 0 on error, 0 on success. There are synchronous
 * and asynchronous methods. Both cases ensure the action has been picked up
 * by the target cpu. The synchronous variants will wait for the action to
 * be completed before returning.
 *
 * Though the current implementation allows queuing actions on the main cpu,
 * the main cpu doesn't process its own queue.
 */
int arch_run_on_cpu(unsigned int cpu, struct cpu_action *action);
int arch_run_on_all_cpus(struct cpu_action *action);
int arch_run_on_all_cpus_but_self(struct cpu_action *action);
int arch_run_on_cpu_async(unsigned int cpu, struct cpu_action *action);
int arch_run_on_all_cpus_async(struct cpu_action *action);
int arch_run_on_all_cpus_but_self_async(struct cpu_action *action);

/* Wait for actions to be perfomed. */
void arch_cpu_wait_for_action(void);

#endif /* !__PRE_RAM__ */

/*
 * Returns logical cpu in range [0:MAX_CPUS). SoC should define this.
 * Additionally, this is needed early in arm64 init so it should not
 * rely on a stack. Standard clobber list is fair game: x0-x7 and x0
 * returns the logical cpu number.
 */
unsigned int smp_processor_id(void);

#endif /* __ARCH_CPU_H__ */
