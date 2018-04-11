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
	 * relocation or CPU driver initialization. However, note that
	 * this callback is called after SMM handlers have been loaded.
	 */
	void (*pre_mp_smm_init)(void);
	/*
	 * Optional function to use to trigger SMM to perform relocation. If
	 * not provided, smm_initiate_relocation() is used.
	 */
	void (*per_cpu_smm_trigger)(void);
	/*
	 * This function is called while each CPU is in the SMM relocation
	 * handler. Its primary purpose is to adjust the SMBASE for the
	 * permanent handler. The parameters passed are the current cpu
	 * running the relocation handler, current SMBASE of relocation handler,
	 * and the pre-calculated staggered CPU SMBASE address of the permanent
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
 * 6. adjust_smm_params(is_perm = 0)
 * 7. adjust_smm_params(is_perm = 1)
 * 8. pre_mp_smm_init()
 * 9. per_cpu_smm_trigger() in parallel for all cpus which calls
 *    relocation_handler() in SMM.
 * 10. mp_initialize_cpu() for each cpu
 * 11. post_mp_init()
 */
int mp_init_with_smm(struct bus *cpu_bus, const struct mp_ops *mp_ops);


/*
 * After APs are up and PARALLEL_MP_AP_WORK is enabled one can issue work
 * to all the APs to perform. Currently the BSP is the only CPU that is allowed
 * to issue work. i.e. the APs should not call any of these functions.
 *
 * Input parameter expire_us <= 0 to specify an infinite timeout.
 *
 * All functions return < 0 on error, 0 on success.
 */
int mp_run_on_aps(void (*func)(void), long expire_us);

/* Like mp_run_on_aps() but also runs func on BSP. */
int mp_run_on_all_cpus(void (*func)(void), long expire_us);

/*
 * Park all APs to prepare for OS boot. This is handled automatically
 * by the coreboot infrastructure.
 */
int mp_park_aps(void);

/*
 * SMM helpers to use with initializing CPUs.
 */

/* Send SMI to self without any serialization. */
void smm_initiate_relocation_parallel(void);
/* Send SMI to self with single execution. */
void smm_initiate_relocation(void);
/* Make a CPU wait until the barrier is released */
void barrier_wait(atomic_t *b);
/*
 * Make a CPU wait until the barrier is released, or timeout occurs
 * returns 1 if timeout occurs before barier is released.
 * returns 0 if barrier is released before timeout.
 */
int barrier_wait_timeout(atomic_t *b, uint32_t timeout_ms);
/* Release a barrier so that other CPUs waiting for that barrier can continue */
void release_barrier(atomic_t *b);

#endif /* _X86_MP_H_ */
