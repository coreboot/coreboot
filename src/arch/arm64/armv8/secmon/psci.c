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

#include <string.h>
#include <stdlib.h>
#include <smp/spinlock.h>
#include <arch/cpu.h>
#include <arch/psci.h>
#include <arch/smc.h>
#include <arch/transition.h>
#include <arch/lib_helpers.h>
#include <console/console.h>
#include "secmon.h"

enum {
	PSCI_CPU_STATE_OFF = 0,
	PSCI_CPU_STATE_ON_PENDING,
	PSCI_CPU_STATE_ON,
};

struct psci_cpu_state {
	uint64_t mpidr;
	void *entry;
	void *arg;
	int state;
};

DECLARE_SPIN_LOCK(psci_spinlock);

static struct psci_cpu_state psci_state[CONFIG_MAX_CPUS];


static inline void psci_lock(void)
{
	spin_lock(&psci_spinlock);
}

static inline void psci_unlock(void)
{
	spin_unlock(&psci_spinlock);
}

static inline int psci_cpu_state_locked(int i)
{
	return psci_state[i].state;
}

static inline void psci_cpu_set_state_locked(int i, int s)
{
	psci_state[i].state = s;
}

static struct cpu_info *mpidr_to_cpu_info(uint64_t mpidr)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(psci_state); i++) {
		if (mpidr == psci_state[i].mpidr)
			return cpu_info_for_cpu(i);
	}

	return NULL;
}

static void psci_cpu_on_callback(void *arg)
{
	struct psci_cpu_state *s = arg;

	psci_turn_on_self(s->entry, s->arg);
}

static void psci_cpu_on(struct psci_func *pf)
{
	uint64_t entry;
	uint64_t target_mpidr;
	uint64_t context_id;
	struct cpu_info *ci;
	int cpu_state;
	struct cpu_action action;

	target_mpidr = psci64_arg(pf, PSCI_PARAM_0);
	entry = psci64_arg(pf, PSCI_PARAM_1);
	context_id = psci64_arg(pf, PSCI_PARAM_2);

	ci = mpidr_to_cpu_info(target_mpidr);

	if (ci == NULL) {
		psci32_return(pf, PSCI_RET_INVALID_PARAMETERS);
		return;
	}

	psci_lock();
	cpu_state = psci_cpu_state_locked(ci->id);

	if (cpu_state == PSCI_CPU_STATE_ON_PENDING) {
		psci32_return(pf, PSCI_RET_ON_PENDING);
		psci_unlock();
		return;
	} else if (cpu_state == PSCI_CPU_STATE_ON) {
		psci32_return(pf, PSCI_RET_ALREADY_ON);
		psci_unlock();
		return;
	}

	psci_cpu_set_state_locked(ci->id, PSCI_CPU_STATE_ON_PENDING);
	/* Set the parameters and initialize the action. */
	psci_state[ci->id].entry = (void *)(uintptr_t)entry;
	psci_state[ci->id].arg = (void *)(uintptr_t)context_id;
	action.run = &psci_cpu_on_callback;
	action.arg = &psci_state[ci->id];

	if (arch_run_on_cpu_async(ci->id, &action)) {
		psci32_return(pf, PSCI_RET_INTERNAL_FAILURE);
		psci_unlock();
		return;
	}

	psci_unlock();

	psci32_return(pf, PSCI_RET_SUCCESS);
}

static void psci_cpu_off(struct psci_func *pf)
{
	psci_lock();
	psci_cpu_set_state_locked(cpu_info()->id, PSCI_CPU_STATE_OFF);
	psci_unlock();

	/* TODO(adurbin): writeback cache and actually turn off CPU. */
	secmon_trampoline(&secmon_wait_for_action, NULL);
}

static int psci_handler(struct smc_call *smc)
{
	struct psci_func pf_storage;
	struct psci_func *pf = &pf_storage;

	psci_func_init(pf, smc);

	switch (pf->id) {
	case PSCI_CPU_ON64:
		psci_cpu_on(pf);
		break;
	case PSCI_CPU_OFF64:
		psci_cpu_off(pf);
		break;
	default:
		psci32_return(pf, PSCI_RET_NOT_SUPPORTED);
		break;
	}

	return 0;
}

void psci_init(void)
{
	struct cpu_info *ci;
	uint64_t mpidr;

	/* Set this CPUs MPIDR clearing the bits that are not per-cpu. */
	ci = cpu_info();
	mpidr = raw_read_mpidr_el1();
	mpidr &= ~(1ULL << 31); /* RES1 */
	mpidr &= ~(1ULL << 30); /* U */
	mpidr &= ~(1ULL << 24); /* MT */
	psci_state[ci->id].mpidr = mpidr;

	if (!cpu_is_bsp())
		return;

	/* Register PSCI handlers. */
	if (smc_register_range(PSCI_CPU_OFF64, PSCI_CPU_ON64, &psci_handler))
		printk(BIOS_ERR, "Couldn't register PSCI handler.\n");
}

void psci_turn_on_self(void *entry, void *arg)
{
	struct exc_state state;
	int target_el;
	struct cpu_info *ci = cpu_info();

	psci_lock();
	psci_cpu_set_state_locked(ci->id, PSCI_CPU_STATE_ON);
	psci_unlock();

	/* Target EL is determined if HVC is enabled or not. */
	target_el = (raw_read_scr_el3() & SCR_HVC_ENABLE) ? EL2 : EL1;

	memset(&state, 0, sizeof(state));
	state.elx.spsr = get_eret_el(target_el, SPSR_USE_H);
	transition_with_entry(entry, arg, &state);
}
