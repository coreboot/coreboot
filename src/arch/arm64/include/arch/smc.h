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

#ifndef __ARCH_SMC_H__
#define __ARCH_SMC_H__

#include <stdint.h>

enum {
	FUNC_ID_CALL_TYPE_SHIFT = 31,
	FUNC_ID_CALL_TYPE_MASK = (1 << FUNC_ID_CALL_TYPE_SHIFT),
	FUNC_ID_FASTCALL = (1 << FUNC_ID_CALL_TYPE_SHIFT),
	FUNC_ID_STDCALL = (0 << FUNC_ID_CALL_TYPE_SHIFT),

	FUNC_ID_CALL_CONVENTION_SHIFT = 30,
	FUNC_ID_CALL_CONVENTION_MASK = (1 << FUNC_ID_CALL_CONVENTION_SHIFT),
	FUNC_ID_SMC32 = (0 << FUNC_ID_CALL_CONVENTION_SHIFT),
	FUNC_ID_SMC64 = (1 << FUNC_ID_CALL_CONVENTION_SHIFT),

	FUNC_ID_ENTITY_SHIFT = 24,
	FUNC_ID_ENTITY_MASK = (0x3f << FUNC_ID_ENTITY_SHIFT),

	FUNC_ID_FUNC_NUMBER_SHIFT = 0,
	FUNC_ID_FUNC_NUMBER_MASK = (0xffff << FUNC_ID_FUNC_NUMBER_SHIFT),

	FUNC_ID_MASK = FUNC_ID_CALL_TYPE_MASK | FUNC_ID_CALL_CONVENTION_MASK |
	               FUNC_ID_ENTITY_MASK | FUNC_ID_FUNC_NUMBER_MASK,

	SMC_NUM_ARGS = 8, /* The last is optional hypervisor id. */
	SMC_NUM_RESULTS = 4,

	SMC_UNKNOWN_FUNC = 0xffffffff,
};

#define SMC_FUNC(entity, number, call_convention, call_type) \
	((call_type) | (call_convention) | \
		((entity) << FUNC_ID_ENTITY_SHIFT) | (number))

#define SMC_FUNC_FAST(entity, number, call_convention) \
	SMC_FUNC((entity), (number), (call_convention), FUNC_ID_FASTCALL)

#define SMC_FUNC_FAST32(entity, number) \
	SMC_FUNC_FAST((entity), (number), FUNC_ID_SMC32)

#define SMC_FUNC_FAST64(entity, number) \
	SMC_FUNC_FAST((entity), (number), FUNC_ID_SMC64)

struct smc_call {
	uint64_t args[SMC_NUM_ARGS];
	uint64_t results[SMC_NUM_RESULTS];
};

/* SMC immediate value needs to be 0. */
/* Check mod AARCHx mode against calling convention. */

static inline uint64_t smc64_arg(const struct smc_call *smc, unsigned i)
{
	return smc->args[i];
}

static inline uint32_t smc32_arg(const struct smc_call *smc, unsigned i)
{
	return smc64_arg(smc, i);
}

static inline void smc64_result(struct smc_call *smc, unsigned i, uint64_t v)
{
	smc->results[i] = v;
}

static inline void smc32_result(struct smc_call *smc, unsigned i, uint32_t v)
{
	uint64_t v64 = v;
	smc64_result(smc, i, v64);
}

static inline void smc32_return(struct smc_call *smc, int32_t v)
{
	smc32_result(smc, 0, v);
}

static inline uint32_t smc_hypervisor_id(const struct smc_call *smc)
{
	/* Set in W7 */
	return smc32_arg(smc, 7);
}

static inline uint32_t smc_session_id(const struct smc_call *smc)
{
	/* Set in W6 */
	return smc32_arg(smc, 6);
}

static inline uint32_t smc_function_id(const struct smc_call *smc)
{
	/* Function ID in W0. */
	return smc32_arg(smc, 0)  & FUNC_ID_MASK;
}

/* Initialize the SMC layer. */
void smc_init(void);

/* Register a handler for a given function range, inclusive. */
int smc_register_range(uint32_t min, uint32_t max, int (*)(struct smc_call *));

#endif /* __ARCH_SMC_H__ */
