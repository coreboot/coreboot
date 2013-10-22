/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef _X86_MP_H_
#define _X86_MP_H_

#include <arch/smp/atomic.h>

#define CACHELINE_SIZE 64

struct cpu_info;
struct bus;

static inline void mfence(void)
{
	__asm__ __volatile__("mfence\t\n": : :"memory");
}

typedef void (*mp_callback_t)(void *arg);

/*
 * A mp_flight_record details a sequence of calls for the APs to perform
 * along with the BSP to coordinate sequencing. Each flight record either
 * provides a barrier for each AP before calling the callback or the APs
 * are allowed to perform the callback without waiting. Regardless, each
 * record has the cpus_entered field incremented for each record. When
 * the BSP observes that the cpus_entered matches the number of APs
 * the bsp_call is called with bsp_arg and upon returning releases the
 * barrier allowing the APs to make further progress.
 *
 * Note that ap_call() and bsp_call() can be NULL. In the NULL case the
 * callback will just not be called.
 */
struct mp_flight_record {
	atomic_t barrier;
	atomic_t cpus_entered;
	mp_callback_t ap_call;
	void *ap_arg;
	mp_callback_t bsp_call;
	void *bsp_arg;
} __attribute__((aligned(CACHELINE_SIZE)));

#define _MP_FLIGHT_RECORD(barrier_, ap_func_, ap_arg_, bsp_func_, bsp_arg_) \
	{							\
		.barrier = ATOMIC_INIT(barrier_),		\
		.cpus_entered = ATOMIC_INIT(0),			\
		.ap_call = ap_func_,				\
		.ap_arg = ap_arg_,				\
		.bsp_call = bsp_func_,				\
		.bsp_arg = bsp_arg_,				\
	}

#define MP_FR_BLOCK_APS(ap_func_, ap_arg_, bsp_func_, bsp_arg_) \
	_MP_FLIGHT_RECORD(0, ap_func_, ap_arg_, bsp_func_, bsp_arg_)

#define MP_FR_NOBLOCK_APS(ap_func_, ap_arg_, bsp_func_, bsp_arg_) \
	_MP_FLIGHT_RECORD(1, ap_func_, ap_arg_, bsp_func_, bsp_arg_)

/* The mp_params structure provides the arguments to the mp subsystem
 * for bringing up APs. */
struct mp_params {
	int num_cpus; /* Total cpus include BSP */
	int parallel_microcode_load;
	const void *microcode_pointer;
	/* adjust_apic_id() is called for every potential apic id in the
	 * system up from 0 to CONFIG_MAX_CPUS. Return adjusted apic_id. */
	int (*adjust_apic_id)(int index, int apic_id);
	/* Flight plan  for APs and BSP. */
	struct mp_flight_record *flight_plan;
	int num_records;
};

/*
 * mp_init() will set up the SIPI vector and bring up the APs according to
 * mp_params. Each flight record will be executed according to the plan. Note
 * that the MP infrastructure uses SMM default area without saving it. It's
 * up to the chipset or mainboard to either e820 reserve this area or save this
 * region prior to calling mp_init() and restoring it after mp_init returns.
 *
 * At the time mp_init() is called the MTRR MSRs are mirrored into APs then
 * caching is enabled before running the flight plan.
 *
 * The MP initialization has the following properties:
 * 1. APs are brought up in parallel.
 * 2. The ordering of coreboot cpu number and APIC ids is not deterministic.
 *    Therefore, one cannot rely on this property or the order of devices in
 *    the device tree unless the chipset or mainboard know the APIC ids
 *    a priori.
 *
 * mp_init() returns < 0 on error, 0 on success.
 */
int mp_init(struct bus *cpu_bus, struct mp_params *params);

/*
 * Useful functions to use in flight records when sequencing APs.
 */

/* Calls cpu_initialize(info->index) which calls the coreboot CPU drivers. */
void mp_initialize_cpu(void *unused);

/* Returns apic id for coreboot cpu number or < 0 on failure. */
int mp_get_apic_id(int cpu_slot);

/*
 * SMM helpers to use with initializing CPUs.
 */

/* Send SMI to self without any serialization. */
void smm_initiate_relocation_parallel(void);
/* Send SMI to self with single execution. */
void smm_initiate_relocation(void);

#endif /* _X86_MP_H_ */
