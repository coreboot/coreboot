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

#ifndef __ARCH_PSCI_H__
#define __ARCH_PSCI_H__

#include <stdint.h>
#include <arch/cpu.h>
#include <arch/smc.h>

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
	void *entry;
	void *arg;
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

struct psci_soc_ops {
	/*
	 * Return number of entities one level below given parent affinitly
	 * level and mpidr.
	 */
	size_t (*children_at_level)(int parent_level, uint64_t mpidr);
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
	PSCI_CPU_OFF64 = SMC_FUNC_FAST64(0x4, 0x2),
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

void psci_init(void);

/* Turn on the current CPU within the PSCI subsystem. */
void psci_turn_on_self(void *entry, void *arg);
int psci_turn_off_self(void);

#endif /* __ARCH_PSCI_H__ */
