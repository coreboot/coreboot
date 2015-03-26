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
 * Foundation, Inc.
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

/*
 * Stages and rmodules have 2 entry points: BSP and non-BSP. Provided
 * a pointer the correct non-BSP entry point will be returned. The
 * first instruction is for BSP and the 2nd is for non-BSP. Instructions
 * are all 32-bit on arm64.
 */
static inline void *secondary_entry_point(void *e)
{
	uintptr_t nonbsp = (uintptr_t)e;

	return (void *)(nonbsp + sizeof(uint32_t));
}

/*
 * The arm64_cpu_startup() initializes a CPU's exception stack and regular
 * stack as well initializing the C environment for the processor. It
 * calls into the array of function pointers at symbol c_entry depending
 * on BSP state. Note that arm64_cpu_startup contains secondary entry
 * point which can be obtained by secondary_entry_point().
 */
void arm64_cpu_startup(void);

/*
 * The arm64_cpu_startup_resume() initializes a CPU's exception stack and
 * regular stack as well initializing the C environment for the processor. It
 * calls into the array of function pointers at symbol c_entry depending
 * on BSP state. Note that arm64_cpu_startup contains secondary entry
 * point which can be obtained by secondary_entry_point().
 * Additionally, it also restores saved register data and enables MMU, caches
 * and exceptions before jumping to C environment for both BSP and non-BSP CPUs.
 */
void arm64_cpu_startup_resume(void);

/*
 * The arm64_arch_timer_init() initializes the per CPU's cntfrq register of
 * ARM arch timer.
 */
void arm64_arch_timer_init(void);

/*
 * The cortex_a57_cpu_power_down sequence as per A57/A53/A72 TRM.
 * L2 flush by HW(0) or SW(1), if system/HW driven L2 flush is supported.
 */
#define NO_L2_FLUSH 0
#define L2_FLUSH_HW 0
#define L2_FLUSH_SW 1

#if IS_ENABLED(CONFIG_ARCH_ARM64_CORTEX_A57_POWER_DOWN_SUPPORT)
void cortex_a57_cpu_power_down(int l2_flush);
#else
static inline void cortex_a57_cpu_power_down(int l2_flush) {}
#endif

#endif /* __ARCH_CPU_H__ */
