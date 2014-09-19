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

#endif /* __ARCH_PSCI_H__ */
