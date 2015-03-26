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

#include <gic.h>
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

DECLARE_SPIN_LOCK(psci_spinlock);

/* Root of PSCI node tree. */
static struct psci_node psci_root;

/* Array of all the psci_nodes in system.  */
static size_t psci_num_nodes;
static struct psci_node **psci_nodes;

static inline void psci_lock(void)
{
	spin_lock(&psci_spinlock);
}

static inline void psci_unlock(void)
{
	spin_unlock(&psci_spinlock);
}

static inline int psci_state_locked(const struct psci_node *e)
{
	return e->state;
}

static inline void psci_set_state_locked(struct psci_node *e, int s)
{
	e->state = s;
}

static struct psci_node *psci_node_lookup(uint64_t mpidr, int level)
{
	size_t i;

	/* The array of node pointers are in depth-first order of the tree. */
	for (i = 0; i < psci_num_nodes; i++) {
		struct psci_node *current = psci_nodes[i];

		if (current->mpidr > mpidr)
			break;
		if (current->mpidr < mpidr)
			continue;
		if (current->level == level)
			return current;
	}
	return NULL;
}

static inline struct psci_node *node_self(void)
{
	return psci_node_lookup(cpu_info()->mpidr, PSCI_AFFINITY_LEVEL_0);
}

/* Find the ancestor of node affected by a state transition limited by level. */
static struct psci_node *psci_find_ancestor(struct psci_node *e, int level,
						int state)
{
	struct psci_node *p;

	/* If all siblings of the node are already off then parent can be
	 * set to off as well. */
	if (state == PSCI_STATE_OFF) {
		while (1) {
			size_t i;
			struct psci_node *s;

			if (psci_root_node(e))
				return e;

			p = psci_node_parent(e);

			if (p->level > level)
				return e;

			for (i = 0; i < p->children.num; i++) {
				s = &p->children.nodes[i];
				/* Don't check target. */
				if (s == e)
					continue;
				if (psci_state_locked(s) != PSCI_STATE_OFF)
					return e;
			}

			e = p;
		}
	}

	/* All ancestors in state OFF are affected. */
	if (state == PSCI_STATE_ON_PENDING) {
		while (1) {
			/* At the root. Return last affected node. */
			if (psci_root_node(e))
				return e;

			p = psci_node_parent(e);

			if (p->level > level)
				return e;

			/* This parent is already ON. */
			if (psci_state_locked(p) != PSCI_STATE_OFF)
				return e;

			e = p;
		}
	}

	/* Default to returning node passed in. */
	return e;
}

static void psci_set_hierarchy_state(struct psci_node *from,
					struct psci_node *to,
					int state)
{
	struct psci_node *end;

	end = psci_node_parent(to);

	while (from != end) {
		/* Raced with another CPU as state is already set. */
		if (psci_state_locked(from) == state)
			break;
		psci_set_state_locked(from, state);
		from = psci_node_parent(from);
	}
}

static void psci_cpu_on_callback(void *arg)
{
	struct exc_state state;
	int target_el;
	struct psci_node *e = arg;

	psci_lock();
	psci_set_hierarchy_state(e, e->cpu_state.ancestor, PSCI_STATE_ON);
	psci_unlock();

	/* Target EL is determined if HVC is enabled or not. */
	target_el = (raw_read_scr_el3() & SCR_HVC_ENABLE) ? EL2 : EL1;

	memset(&state, 0, sizeof(state));
	state.elx.spsr = get_eret_el(target_el, SPSR_USE_H);
	transition_with_entry(e->cpu_state.startup.run,
				e->cpu_state.startup.arg, &state);
}

static void psci_cpu_on_prepare(struct psci_cmd *cmd,
				const struct cpu_action *a)
{
	struct psci_node *ancestor;
	struct psci_node *e;
	int state = PSCI_STATE_ON_PENDING;

	e = cmd->target;
	e->cpu_state.startup = *a;
	ancestor = psci_find_ancestor(e, PSCI_AFFINITY_LEVEL_HIGHEST, state);
	e->cpu_state.ancestor = ancestor;
	cmd->ancestor = ancestor;
}

static int psci_schedule_cpu_on(struct psci_node *e)
{
	struct cpu_info *ci;
	struct cpu_action action = {
		.run = &psci_cpu_on_callback,
		.arg = e,
	};

	ci = e->cpu_state.ci;
	if (ci == NULL || arch_run_on_cpu_async(ci->id, &action)) {
		psci_set_hierarchy_state(e, e->cpu_state.ancestor,
						PSCI_STATE_OFF);
		return PSCI_RET_INTERNAL_FAILURE;
	}

	return PSCI_RET_SUCCESS;
}

static void psci_cpu_resume_prepare(struct psci_cmd *cmd,
				const struct cpu_action *a)
{
	struct psci_node *ancestor;
	struct psci_node *e;
	int state = PSCI_STATE_ON_PENDING;

	e = cmd->target;
	e->cpu_state.resume = *a;
	ancestor = psci_find_ancestor(e, PSCI_AFFINITY_LEVEL_HIGHEST, state);
	e->cpu_state.ancestor = ancestor;
	cmd->ancestor = ancestor;
}

static void psci_schedule_cpu_resume(struct psci_node *e)
{
	struct cpu_info *ci;
	struct cpu_action *action;

	if (e->cpu_state.resume.run == NULL)
		return;

	ci = e->cpu_state.ci;
	action = &e->cpu_state.resume;

	arch_run_on_cpu(ci->id, action);
}

void psci_turn_on_self(const struct cpu_action *action)
{
	struct psci_node *e = node_self();
	struct psci_cmd cmd = {
		.type = PSCI_CMD_ON,
	};

	if (e == NULL) {
		printk(BIOS_ERR, "Couldn't turn on self: mpidr %llx\n",
			cpu_info()->mpidr);
		return;
	}

	cmd.target = e;

	psci_lock();
	psci_cpu_on_prepare(&cmd, action);
	psci_set_hierarchy_state(e, cmd.ancestor, PSCI_STATE_ON_PENDING);
	psci_unlock();

	psci_schedule_cpu_on(e);
}

void psci_cpu_entry(void)
{
	gic_enable();

	/*
	 * Just wait for an action to be performed.
	 */
	psci_schedule_cpu_resume(node_self());
	secmon_wait_for_action();
}

static void psci_cpu_resume(void *arg)
{
	uint64_t power_state = (uint64_t)arg;
	struct psci_node *e;
	struct psci_power_state state;
	struct psci_cmd cmd = {
		.type = PSCI_CMD_RESUME,
	};

	psci_power_state_unpack(power_state, &state);

	psci_lock();

	e = node_self();
	/* clear the resume action after resume */
	e->cpu_state.resume.run = NULL;
	e->cpu_state.resume.arg = NULL;

	cmd.target = e;
	cmd.state = &state;
	soc_psci_ops.cmd_prepare(&cmd);

	psci_unlock();

	soc_psci_ops.cmd_commit(&cmd);

	psci_lock();
	psci_set_hierarchy_state(e, e->cpu_state.ancestor, PSCI_STATE_ON);
	psci_unlock();

	psci_schedule_cpu_on(node_self());
}

static void psci_cpu_suspend(struct psci_func *pf)
{
	uint64_t power_state;
	uint64_t entry;
	uint64_t context_id;
	struct psci_node *e;
	struct psci_power_state state;
	struct cpu_action action;
	struct cpu_action resume_action;
	struct psci_cmd cmd = {
		.type = PSCI_CMD_SUSPEND,
	};
	int ret;

	power_state = psci64_arg(pf, PSCI_PARAM_0);
	entry = psci64_arg(pf, PSCI_PARAM_1);
	context_id = psci64_arg(pf, PSCI_PARAM_2);
	psci_power_state_unpack(power_state, &state);

	psci_lock();

	e = node_self();
	cmd.target = e;
	cmd.state = &state;
	action.run = (void *)entry;
	action.arg = (void *)context_id;
	resume_action.run = &psci_cpu_resume;
	resume_action.arg = (void*)power_state;

	psci_cpu_on_prepare(&cmd, &action);
	psci_cpu_resume_prepare(&cmd, &resume_action);

	ret = soc_psci_ops.cmd_prepare(&cmd);

	if (ret == PSCI_RET_SUCCESS)
		psci_set_hierarchy_state(e, cmd.ancestor, PSCI_STATE_OFF);

	psci_unlock();

	if (ret != PSCI_RET_SUCCESS)
		return psci32_return(pf, ret);

	gic_disable();

	ret = soc_psci_ops.cmd_commit(&cmd);

	/* PSCI_POWER_STATE_TYPE_STANDBY mode only */

	psci_lock();
	resume_action.run = NULL;
	resume_action.arg = NULL;
	psci_cpu_resume_prepare(&cmd, &resume_action);
	psci_unlock();

	if (ret != PSCI_RET_SUCCESS)
		return psci32_return(pf, ret);

	psci_lock();
	psci_set_hierarchy_state(e, e->cpu_state.ancestor, PSCI_STATE_ON);
	psci_unlock();

	psci32_return(pf, PSCI_RET_SUCCESS);
}

static void psci_cpu_on(struct psci_func *pf)
{
	uint64_t entry;
	uint64_t target_mpidr;
	uint64_t context_id;
	int cpu_state;
	int ret;
	struct psci_node *e;
	struct cpu_action action;
	struct psci_cmd cmd = {
		.type = PSCI_CMD_ON,
	};

	target_mpidr = psci64_arg(pf, PSCI_PARAM_0);
	entry = psci64_arg(pf, PSCI_PARAM_1);
	context_id = psci64_arg(pf, PSCI_PARAM_2);

	e = psci_node_lookup(target_mpidr, PSCI_AFFINITY_LEVEL_0);

	if (e == NULL) {
		psci32_return(pf, PSCI_RET_INVALID_PARAMETERS);
		return;
	}

	psci_lock();
	cpu_state = psci_state_locked(e);

	if (cpu_state == PSCI_STATE_ON_PENDING) {
		psci32_return(pf, PSCI_RET_ON_PENDING);
		psci_unlock();
		return;
	} else if (cpu_state == PSCI_STATE_ON) {
		psci32_return(pf, PSCI_RET_ALREADY_ON);
		psci_unlock();
		return;
	}

	cmd.target = e;
	action.run = (void *)entry;
	action.arg = (void *)context_id;
	psci_cpu_on_prepare(&cmd, &action);

	ret = soc_psci_ops.cmd_prepare(&cmd);

	if (ret == PSCI_RET_SUCCESS)
		psci_set_hierarchy_state(e, cmd.ancestor,
					PSCI_STATE_ON_PENDING);

	psci_unlock();

	if (ret != PSCI_RET_SUCCESS)
		return psci32_return(pf, ret);

	ret = soc_psci_ops.cmd_commit(&cmd);

	if (ret != PSCI_RET_SUCCESS) {
		psci_lock();
		psci_set_hierarchy_state(e, cmd.ancestor, PSCI_STATE_OFF);
		psci_unlock();
		return psci32_return(pf, ret);
	}

	psci32_return(pf, psci_schedule_cpu_on(e));
}

static int psci_turn_off_node(struct psci_node *e, int level,
					int state_id)
{
	int ret;
	struct psci_cmd cmd = {
		.type = PSCI_CMD_OFF,
		.state_id = state_id,
		.target = e,
	};

	psci_lock();

	cmd.ancestor = psci_find_ancestor(e, level, PSCI_STATE_OFF);

	ret = soc_psci_ops.cmd_prepare(&cmd);

	if (ret == PSCI_RET_SUCCESS)
		psci_set_hierarchy_state(e, cmd.ancestor, PSCI_STATE_OFF);

	psci_unlock();

	if (ret != PSCI_RET_SUCCESS)
		return ret;

	gic_disable();

	/* Should never return. */
	ret = soc_psci_ops.cmd_commit(&cmd);

	/* Adjust ret to be an error. */
	if (ret == PSCI_RET_SUCCESS)
		ret = PSCI_RET_INTERNAL_FAILURE;

	/* Turn things back on. */
	psci_lock();
	psci_set_hierarchy_state(e, cmd.ancestor, PSCI_STATE_ON);
	psci_unlock();

	return ret;
}

int psci_turn_off_self(void)
{
	struct psci_node *e = node_self();

	if (e == NULL) {
		printk(BIOS_ERR, "No PSCI node for MPIDR %llx.\n",
			cpu_info()->mpidr);
		return PSCI_RET_INTERNAL_FAILURE;
	}

	/* -1 state id indicates to SoC to make its own decision for
	 * internal state when powering off the node. */
	return psci_turn_off_node(e, PSCI_AFFINITY_LEVEL_HIGHEST, -1);
}

static int psci_handler(struct smc_call *smc)
{
	struct psci_func pf_storage;
	struct psci_func *pf = &pf_storage;

	psci_func_init(pf, smc);

	switch (pf->id) {
	case PSCI_CPU_SUSPEND64:
		psci_cpu_suspend(pf);
		break;
	case PSCI_CPU_ON64:
		psci_cpu_on(pf);
		break;
	case PSCI_CPU_OFF32:
		psci32_return(pf, psci_turn_off_self());
		break;
	default:
		psci32_return(pf, PSCI_RET_NOT_SUPPORTED);
		break;
	}

	return 0;
}

static void psci_link_cpu_info(void *arg)
{
	struct psci_node *e = node_self();

	if (e == NULL) {
		printk(BIOS_ERR, "No PSCI node for MPIDR %llx.\n",
			cpu_info()->mpidr);
		return;
	}

	e->cpu_state.ci = cpu_info();
}

static int psci_init_node(struct psci_node *e,
				struct psci_node *parent,
				int level, uint64_t mpidr)
{
	size_t i;
	uint64_t mpidr_inc;
	struct psci_node_group *ng;
	size_t num_children;

	memset(e, 0, sizeof(*e));
	e->mpidr = mpidr;
	psci_set_state_locked(e, PSCI_STATE_OFF);
	e->parent = parent;
	e->level = level;

	if (level == PSCI_AFFINITY_LEVEL_0)
		return 0;

	num_children = soc_psci_ops.children_at_level(level, mpidr);

	if (num_children == 0)
		return 0;

	ng = &e->children;
	ng->num = num_children;
	ng->nodes = malloc(ng->num * sizeof(struct psci_node));
	if (ng->nodes == NULL) {
		printk(BIOS_DEBUG, "PSCI: Allocation failure at level %d\n",
			level);
		return -1;
	}

	/* Switch to next level below. */
	level = psci_level_below(level);
	mpidr_inc = mpidr_mask(!!(level == PSCI_AFFINITY_LEVEL_3),
				!!(level == PSCI_AFFINITY_LEVEL_2),
				!!(level == PSCI_AFFINITY_LEVEL_1),
				!!(level == PSCI_AFFINITY_LEVEL_0));

	for (i = 0; i < ng->num; i++) {
		struct psci_node *c = &ng->nodes[i];

		/* Recursively initialize the nodes. */
		if (psci_init_node(c, e, level, mpidr))
			return -1;
		mpidr += mpidr_inc;
	}

	return 0;
}

static size_t psci_count_children(struct psci_node *e)
{
	size_t i;
	size_t count;

	if (e->level == PSCI_AFFINITY_LEVEL_0)
		return 0;

	count = e->children.num;
	for (i = 0; i < e->children.num; i++)
		count +=  psci_count_children(&e->children.nodes[i]);

	return count;
}

static size_t psci_write_nodes(struct psci_node *e, size_t index)
{
	size_t i;

	/*
	 * Recursively save node pointers in array. Node pointers are
	 * ordered in ascending mpidr and descending level within same mpidr.
	 * i.e. each node is saved in depth-first order of the tree.
	 */
	if (e->level != PSCI_AFFINITY_ROOT) {
		psci_nodes[index] = e;
		index++;
	}

	if (e->level == PSCI_AFFINITY_LEVEL_0)
		return index;

	for (i = 0; i < e->children.num; i++)
		index = psci_write_nodes(&e->children.nodes[i], index);

	return index;
}

static int psci_allocate_nodes(void)
{
	int level;
	size_t num_children;
	uint64_t mpidr;
	struct psci_node *e;

	mpidr = 0;
	level = PSCI_AFFINITY_ROOT;

	/* Find where the root should start. */
	while (psci_level_below(level) >= PSCI_AFFINITY_LEVEL_0) {
		num_children = soc_psci_ops.children_at_level(level, mpidr);

		if (num_children == 0) {
			printk(BIOS_ERR, "PSCI: No children at level %d!\n",
				level);
			return -1;
		}

		/* The root starts where the affinity levels branch. */
		if (num_children > 1)
			break;

		level = psci_level_below(level);
	}

	if (psci_init_node(&psci_root, NULL, level, mpidr)) {
		printk(BIOS_ERR, "PSCI init node failure.\n");
		return -1;
	}

	num_children = psci_count_children(&psci_root);
	/* Count the root node if isn't a fake node. */
	if (psci_root.level != PSCI_AFFINITY_ROOT)
		num_children++;

	psci_nodes = malloc(num_children * sizeof(void *));
	psci_num_nodes = num_children;

	if (psci_nodes == NULL) {
		printk(BIOS_ERR, "PSCI node pointer array failure.\n");
		return -1;
	}

	num_children = psci_write_nodes(&psci_root, 0);
	if (num_children != psci_num_nodes) {
		printk(BIOS_ERR, "Wrong nodes written: %zd vs %zd.\n",
			num_children, psci_num_nodes);
		return -1;
	}

	/*
	 * By default all nodes are set to PSCI_STATE_OFF. In order not
	 * to race with other CPUs turning themselves off set the BSPs
	 * affinity node to ON.
	 */
	e = node_self();
	if (e == NULL) {
		printk(BIOS_ERR, "No PSCI node for BSP.\n");
		return -1;
	}
	psci_set_state_locked(e, PSCI_STATE_ON);

	return 0;
}

void psci_init(uintptr_t cpu_on_entry)
{
	struct cpu_action action = {
		.run = &psci_link_cpu_info,
	};

	if (psci_allocate_nodes()) {
		printk(BIOS_ERR, "PSCI support not enabled.\n");
		return;
	}

	if (arch_run_on_all_cpus_async(&action))
		printk(BIOS_ERR, "Error linking cpu_info to PSCI nodes.\n");

	/* Register PSCI handlers. */
	if (smc_register_range(PSCI_CPU_SUSPEND32, PSCI_CPU_ON32,
			       &psci_handler))
		printk(BIOS_ERR, "Couldn't register PSCI handler.\n");

	if (smc_register_range(PSCI_CPU_SUSPEND64, PSCI_CPU_ON64,
			       &psci_handler))
		printk(BIOS_ERR, "Couldn't register PSCI handler.\n");

	/* Inform SoC layer of CPU_ON entry point. */
	psci_soc_init(cpu_on_entry);
}
