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

#ifndef __ARCH_PSCI_H__
#define __ARCH_PSCI_H__

#include <stdint.h>
#include <arch/cpu.h>
#include <arch/smc.h>

/* PSCI v0.2 power state encoding for CPU_SUSPEND function */
#define PSCI_0_2_POWER_STATE_ID_MASK	0xffff
#define PSCI_0_2_POWER_STATE_ID_SHIFT	0
#define PSCI_0_2_POWER_STATE_TYPE_SHIFT	16
#define PSCI_0_2_POWER_STATE_TYPE_MASK	\
		(0x1 << PSCI_0_2_POWER_STATE_TYPE_SHIFT)
#define PSCI_0_2_POWER_STATE_AFFL_SHIFT	24
#define PSCI_0_2_POWER_STATE_AFFL_MASK	\
		(0x3 << PSCI_0_2_POWER_STATE_AFFL_SHIFT)

#define PSCI_POWER_STATE_TYPE_STANDBY		0
#define PSCI_POWER_STATE_TYPE_POWER_DOWN	1

struct psci_power_state {
	u16	id;
	u8	type;
	u8	affinity_level;
};

/* Return Values */
enum {
	PSCI_RET_SUCCESS = 0,
	PSCI_RET_NOT_SUPPORTED = -1,
	PSCI_RET_INVALID_PARAMETERS = -2,
	PSCI_RET_DENIED = -3,
	PSCI_RET_ALREADY_ON = -4,
	PSCI_RET_ON_PENDING = -5,
	PSCI_RET_INTERNAL_FAILURE = -6,
	PSCI_RET_NOT_PRESENT = -7,
	PSCI_RET_DISABLED = -8,
};

/* Generic PSCI state. */
enum {
	PSCI_STATE_OFF = 0,
	PSCI_STATE_ON_PENDING,
	PSCI_STATE_ON,
};

/* Affinity level support. */
enum {
	PSCI_AFFINITY_LEVEL_0,
	PSCI_AFFINITY_LEVEL_1,
	PSCI_AFFINITY_LEVEL_2,
	PSCI_AFFINITY_LEVEL_3,
	PSCI_AFFINITY_ROOT,
	PSCI_AFFINITY_LEVEL_HIGHEST = PSCI_AFFINITY_ROOT,
};

static inline int psci_level_below(int level)
{
	return level - 1;
}

struct psci_node;

struct psci_cpu_state {
	struct cpu_info *ci;
	struct cpu_action startup;
	struct cpu_action resume;
	/* Ancestor of target to update state in CPU_ON case. */
	struct psci_node *ancestor;
};

struct psci_node_group {
	size_t num;
	struct psci_node *nodes;
};

struct psci_node {
	uint64_t mpidr;
	/* Affinity level of node. */
	int level;
	/* Generic power state of this entity. */
	int state;
	/* The SoC can stash its own state accounting in here. */
	int soc_state;
	/* Parent of curernt entity. */
	struct psci_node *parent;
	/*
	 * CPUs are leaves in the tree. They don't have children. The
	 * CPU-specific bits of storage can be shared with the children
	 * storage.
	 */
	union {
		struct psci_node_group children;
		struct psci_cpu_state cpu_state;
	};
};

static inline struct psci_node *psci_node_parent(const struct psci_node *n)
{
	return n->parent;
}

static inline int psci_root_node(const struct psci_node *n)
{
	return psci_node_parent(n) == NULL;
}

enum {
	PSCI_CMD_ON,
	PSCI_CMD_OFF,
	PSCI_CMD_SUSPEND,
	PSCI_CMD_RESUME,
};

/*
 * PSCI actions are serialized into a command for the SoC to process. There are
 * 2 phases of a command being processed: prepare and commit. The prepare() is
 * called with the PSCI locks held for the state of the PSCI nodes. If
 * successful, the appropriate locks will be dropped and commit() will be
 * called with the same structure. It is permissible for the SoC support code
 * to modify the struture passed in (e.g. to update the requested state_id to
 * reflect dynamic constraints on how deep of a state to enter).
 */
struct psci_cmd {
	/* Command type. */
	int type;
	/*
	 * PSCI state id for PSCI_CMD_OFF and PSCI_CMD_STANDBY commands.
	 * A value of -1 indicates a CPU_OFF request.
	 */
	int state_id;
	struct psci_power_state *state;
	/*
	 * target is the command's target, but it can affect up to the
	 * ancestor entity. If target == ancestor then it only affects
	 * target, otherwise all entites up the hierarchy including ancestor.
	 */
	struct psci_node *target;
	struct psci_node *ancestor;
};

struct psci_soc_ops {
	/*
	 * Return number of entities one level below given parent affinitly
	 * level and mpidr.
	 */
	size_t (*children_at_level)(int parent_level, uint64_t mpidr);

	int (*cmd_prepare)(struct psci_cmd *cmd);
	int (*cmd_commit)(struct psci_cmd *cmd);
};

/* Each SoC needs to provide the functions in the psci_soc_ops structure. */
extern struct psci_soc_ops soc_psci_ops;

/* PSCI Functions. */
enum {
	/* 32-bit System level functions. */
	PSCI_VERSION = SMC_FUNC_FAST32(0x4, 0x0),
	PSCI_SYSTEM_OFF = SMC_FUNC_FAST32(0x4, 0x8),
	PSCI_SYSTEM_RESET = SMC_FUNC_FAST32(0x4, 0x9),

	/* 32-bit CPU support functions. */
	PSCI_CPU_SUSPEND32 = SMC_FUNC_FAST32(0x4, 0x1),
	PSCI_CPU_OFF32 = SMC_FUNC_FAST32(0x4, 0x2),
	PSCI_CPU_ON32 = SMC_FUNC_FAST32(0x4, 0x3),

	/* 64-bit CPU support functions. */
	PSCI_CPU_SUSPEND64 = SMC_FUNC_FAST64(0x4, 0x1),
	PSCI_CPU_ON64 = SMC_FUNC_FAST64(0x4, 0x3),
};

/* Parameter arguments. */
enum {
	PSCI_PARAM_0 = 1,
	PSCI_PARAM_1,
	PSCI_PARAM_2,
	PSCI_PARAM_3,
	PSCI_RETURN_0 = 1,
	PSCI_RETURN_1,
	PSCI_RETURN_2,
	PSCI_RETURN_3,
};

struct psci_func {
	uint32_t id;
	struct smc_call *smc;
};

static inline void psci_power_state_unpack(uint32_t power_state,
				    struct psci_power_state *state)
{
	state->id = (power_state & PSCI_0_2_POWER_STATE_ID_MASK) >>
			PSCI_0_2_POWER_STATE_ID_SHIFT;
	state->type = (power_state & PSCI_0_2_POWER_STATE_TYPE_MASK) >>
			PSCI_0_2_POWER_STATE_TYPE_SHIFT;
	state->affinity_level =
			(power_state & PSCI_0_2_POWER_STATE_AFFL_MASK) >>
			PSCI_0_2_POWER_STATE_AFFL_SHIFT;
}

static inline void psci_func_init(struct psci_func *pf, struct smc_call *smc)
{
	pf->id = smc_function_id(smc);
	pf->smc = smc;
}

static inline uint64_t psci64_arg(struct psci_func *pf, unsigned i)
{
	return smc64_arg(pf->smc, i);
}

static inline uint32_t psci32_arg(struct psci_func *pf, unsigned i)
{
	return psci64_arg(pf, i);
}

static inline void psci64_result(struct psci_func *pf, unsigned i, uint64_t v)
{
	smc64_result(pf->smc, i, v);
}

static inline void psci32_result(struct psci_func *pf, unsigned i, uint32_t v)
{
	uint64_t v64 = v;
	psci64_result(pf, i, v64);
}

static inline void psci32_return(struct psci_func *pf, int32_t val)
{
	psci32_result(pf, 0, val);
}

static inline void psci64_return(struct psci_func *pf, int64_t val)
{
	psci64_result(pf, 0, val);
}

void psci_init(uintptr_t cpu_on_entry);
void psci_soc_init(uintptr_t cpu_on_entry);

/* Turn on the current CPU within the PSCI subsystem. */
void psci_turn_on_self(const struct cpu_action *action);
int psci_turn_off_self(void);
/* Entry point for CPUs just turning on or waking up. */
void psci_cpu_entry(void);

#endif /* __ARCH_PSCI_H__ */
