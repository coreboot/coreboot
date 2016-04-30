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
 */

#ifndef _X86_MP_H_
#define _X86_MP_H_

#include <arch/smp/atomic.h>
#include <cpu/x86/smm.h>

#define CACHELINE_SIZE 64

struct cpu_info;
struct bus;

static inline void mfence(void)
{
	__asm__ __volatile__("mfence\t\n": : :"memory");
}

typedef void (*mp_callback_t)(void);

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
	mp_callback_t bsp_call;
} __attribute__((aligned(CACHELINE_SIZE)));

#define _MP_FLIGHT_RECORD(barrier_, ap_func_, bsp_func_) \
	{							\
		.barrier = ATOMIC_INIT(barrier_),		\
		.cpus_entered = ATOMIC_INIT(0),			\
		.ap_call = ap_func_,				\
		.bsp_call = bsp_func_,				\
	}

#define MP_FR_BLOCK_APS(ap_func_, bsp_func_) \
	_MP_FLIGHT_RECORD(0, ap_func_, bsp_func_)

#define MP_FR_NOBLOCK_APS(ap_func_, bsp_func_) \
	_MP_FLIGHT_RECORD(1, ap_func_, bsp_func_)

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

/* The sequence of the callbacks are in calling order. */
struct mp_ops {
	/*
	 * Optionally provide a callback prior to kicking off MP
	 * startup. This callback is done prior to loading the SIPI
	 * vector but after gathering the MP state information. Please
	 * see the sequence below.
	 */
	void (*pre_mp_init)(void);
	/*
	 * Return the number of logical x86 execution contexts that
	 * need to be brought out of SIPI state as well as have SMM
	 * handlers installed.
	 */
	int (*get_cpu_count)(void);
	/*
	 * Optionally fill in permanent SMM region and save state size. If
	 * this callback is not present no SMM handlers will be installed.
	 * The perm_smsize is the size available to house the permanent SMM
	 * handler.
	 */
	void (*get_smm_info)(uintptr_t *perm_smbase, size_t *perm_smsize,
				size_t *smm_save_state_size);
	/*
	 * Optionally fill in pointer to microcode and indicate if the APs
	 * can load the microcode in parallel.
	 */
	void (*get_microcode_info)(const void **microcode, int *parallel);
	/*
	 * Optionally provide a function which adjusts the APIC id
	 * map to cpu number. By default the cpu number and APIC id
	 * are 1:1. To change the APIC id for a given cpu return the
	 * new APIC id. It's called for each cpu as indicated by
	 * get_cpu_count().
	 */
	int (*adjust_cpu_apic_entry)(int cpu, int cur_apic_id);
	/*
	 * Optionally adjust SMM handler parameters to override the default
	 * values.  The is_perm variable indicates if the parameters to adjust
	 * are for the relocation handler or the permanent handler. This
	 * function is therefore called twice -- once for each handler.
	 * By default the parameters for each SMM handler are:
	 *       stack_size     num_concurrent_stacks num_concurrent_save_states
	 * relo: save_state_size    get_cpu_count()          1
	 * perm: save_state_size    get_cpu_count()          get_cpu_count()
	 */
	void (*adjust_smm_params)(struct smm_loader_params *slp, int is_perm);
	/*
	 * Optionally provide a callback prior to the APs starting SMM
	 * relocation or cpu driver initialization. However, note that
	 * this callback is called after SMM handlers have been loaded.
	 */
	void (*pre_mp_smm_init)(void);
	/*
	 * Optional function to use to trigger SMM to perform relocation. If
	 * not provided, smm_initiate_relocation() is used.
	 */
	void (*per_cpu_smm_trigger)(void);
	/*
	 * This function is called while each cpu is in the SMM relocation
	 * handler. Its primary purpose is to adjust the SMBASE for the
	 * permanent handler. The parameters passed are the current cpu
	 * running the relocation handler, current SMBASE of relocation handler,
	 * and the pre-calculated staggered cpu SMBASE address of the permanent
	 * SMM handler.
	 */
	void (*relocation_handler)(int cpu, uintptr_t curr_smbase,
		uintptr_t staggered_smbase);
	/*
	 * Optionally provide a callback that is called after the APs
	 * and the BSP have gone through the initialion sequence.
	 */
	void (*post_mp_init)(void);
};

/*
 * mp_init_with_smm() returns < 0 on failure and 0 on success. The mp_ops
 * argument is used to drive the multiprocess initialization. Unless otherwise
 * stated each callback is called on the BSP only. The sequence of operations
 * is the following:
 * 1. pre_mp_init()
 * 2. get_cpu_count()
 * 3. get_smm_info()
 * 4. get_microcode_info()
 * 5. adjust_cpu_apic_entry() for each number of get_cpu_count()
 * 6. adjust_smm_params(is_perm=0)
 * 7. adjust_smm_params(is_perm=1)
 * 8. pre_mp_smm_init()
 * 9. per_cpu_smm_trigger() in parallel for all cpus which calls
 *    relocation_handler() in SMM.
 * 10. mp_initialize_cpu() for each cpu
 * 11. post_mp_init()
 */
int mp_init_with_smm(struct bus *cpu_bus, const struct mp_ops *mp_ops);

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
void mp_initialize_cpu(void);

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
