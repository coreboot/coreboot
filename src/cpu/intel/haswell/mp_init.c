/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 ChromeOS Authors
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <console/console.h>
#include <stdint.h>
#include <rmodule.h>
#include <arch/cpu.h>
#include <cpu/cpu.h>
#include <cpu/intel/microcode.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/smm.h>
#include <delay.h>
#include <device/device.h>
#include <device/path.h>
#include <lib.h>
#include <smp/atomic.h>
#include <smp/spinlock.h>
#include <thread.h>
#include "haswell.h"

/* This needs to match the layout in the .module_parametrs section. */
struct sipi_params {
	u16 gdtlimit;
	u32 gdt;
	u16 unused;
	u32 idt_ptr;
	u32 stack_top;
	u32 stack_size;
	u32 microcode_ptr;
	u32 msr_table_ptr;
	u32 msr_count;
	u32 c_handler;
	u32 c_handler_arg;
	u8 apic_to_cpu_num[CONFIG_MAX_CPUS];
} __attribute__((packed));

/* This also needs to match the assembly code for saved MSR encoding. */
struct saved_msr {
	u32 index;
	u32 lo;
	u32 hi;
} __attribute__((packed));


/* The sipi vector rmodule is included in the ramstage using 'objdump -B'. */
extern char _binary_sipi_vector_start[];
/* These symbols are defined in c_start.S. */
extern char gdt[];
extern char gdt_end[];
extern char idtarg[];

/* This table keeps track of each CPU's APIC id. */
static u8 apic_id_table[CONFIG_MAX_CPUS];
static device_t cpu_devs[CONFIG_MAX_CPUS];

/* Number of APs checked that have checked in. */
static atomic_t num_aps;
/* Number of APs that have relocated their SMM handler. */
static atomic_t num_aps_relocated_smm;
/* Barrier to stop APs from performing SMM relocation. */
static int smm_relocation_barrier_begin __attribute__ ((aligned (64)));
/* Determine if hyperthreading is disabled. */
int ht_disabled;

static inline void mfence(void)
{
	__asm__ __volatile__("mfence\t\n": : :"memory");
}

static inline void wait_for_barrier(volatile int *barrier)
{
	while (*barrier == 0) {
		asm ("pause");
	}
}

static inline void release_barrier(volatile int *barrier)
{
	*barrier = 1;
}

static void ap_wait_for_smm_relocation_begin(void)
{
	wait_for_barrier(&smm_relocation_barrier_begin);
}

/* This function pointer is used by the non-BSP CPUs to initiate relocation. It
 * points to either a serial or parallel SMM initiation. */
static void (*ap_initiate_smm_relocation)(void) = &smm_initiate_relocation;


/* Returns 1 if timeout waiting for APs. 0 if target aps found. */
static int wait_for_aps(atomic_t *val, int target, int total_delay,
                        int delay_step)
{
	int timeout = 0;
	int delayed = 0;
	while (atomic_read(val) != target) {
		udelay(delay_step);
		delayed += delay_step;
		if (delayed >= total_delay) {
			timeout = 1;
			break;
		}
	}

	return timeout;
}

void release_aps_for_smm_relocation(int do_parallel)
{
	/* Change the AP SMM initiation function, and ensure it is visible
	 * before releasing the APs. */
	if (do_parallel) {
		ap_initiate_smm_relocation = &smm_initiate_relocation_parallel;
		mfence();
	}
	release_barrier(&smm_relocation_barrier_begin);
	/* Wait for CPUs to relocate their SMM handler up to 100ms. */
	if (wait_for_aps(&num_aps_relocated_smm, atomic_read(&num_aps),
	                 100000 /* 100 ms */, 200 /* us */))
		printk(BIOS_DEBUG, "Timed out waiting for AP SMM relocation\n");
}

/* The mtrr code sets up ROM caching on the BSP, but not the others. However,
 * the boot loader payload disables this. In order for Linux not to complain
 * ensure the caching is disabled for the APs before going to sleep. */
static void cleanup_rom_caching(void)
{
	x86_mtrr_disable_rom_caching();
}

/* By the time APs call ap_init() caching has been setup, and microcode has
 * been loaded. */
static void asmlinkage ap_init(unsigned int cpu, void *microcode_ptr)
{
	struct cpu_info *info;

	/* Signal that the AP has arrived. */
	atomic_inc(&num_aps);

	/* Ensure the local apic is enabled */
	enable_lapic();

	info = cpu_info();
	info->index = cpu;
	info->cpu = cpu_devs[cpu];
	thread_init_cpu_info_non_bsp(info);

	apic_id_table[info->index] = lapicid();
	info->cpu->path.apic.apic_id = apic_id_table[info->index];

	/* Call through the cpu driver's initialization. */
	cpu_initialize(info->index);

	ap_wait_for_smm_relocation_begin();

	ap_initiate_smm_relocation();

	/* Indicate that SMM relocation has occurred on this thread. */
	atomic_inc(&num_aps_relocated_smm);

	/* After SMM relocation a 2nd microcode load is required. */
	intel_microcode_load_unlocked(microcode_ptr);

	/* The MTRR resources are core scoped. Therefore, there is no need
	 * to do the same work twice. Additionally, this check keeps the
	 * ROM cache enabled on the BSP since its hyperthread sibling won't
	 * call cleanup_rom_caching(). */
	if ((lapicid() & 1) == 0)
		cleanup_rom_caching();

	/* FIXME(adurbin): park CPUs properly -- preferably somewhere in a
	 * reserved part of memory that the OS cannot get to. */
	stop_this_cpu();
}

static void setup_default_sipi_vector_params(struct sipi_params *sp)
{
	int i;
	u8 apic_id;
	u8 apic_id_inc;

	sp->gdt = (u32)&gdt;
	sp->gdtlimit = (u32)&gdt_end - (u32)&gdt - 1;
	sp->idt_ptr = (u32)&idtarg;
	sp->stack_size = CONFIG_STACK_SIZE;
	sp->stack_top = (u32)&_estack;
	/* Adjust the stack top to take into account cpu_info. */
	sp->stack_top -= sizeof(struct cpu_info);

	/* Default to linear APIC id space if HT is enabled. If it is
	 * disabled the APIC ids increase by 2 as the odd numbered APIC
	 * ids are not present.*/
	apic_id_inc = (ht_disabled) ? 2 : 1;
	for (i = 0, apic_id = 0; i < CONFIG_MAX_CPUS; i++) {
		sp->apic_to_cpu_num[i] = apic_id;
		apic_id += apic_id_inc;
	}
}

#define NUM_FIXED_MTRRS 11
static unsigned int fixed_mtrrs[NUM_FIXED_MTRRS] = {
	MTRRfix64K_00000_MSR, MTRRfix16K_80000_MSR, MTRRfix16K_A0000_MSR,
	MTRRfix4K_C0000_MSR, MTRRfix4K_C8000_MSR, MTRRfix4K_D0000_MSR,
	MTRRfix4K_D8000_MSR, MTRRfix4K_E0000_MSR, MTRRfix4K_E8000_MSR,
	MTRRfix4K_F0000_MSR, MTRRfix4K_F8000_MSR,
};

static inline struct saved_msr *save_msr(int index, struct saved_msr *entry)
{
	msr_t msr;

	msr = rdmsr(index);
	entry->index = index;
	entry->lo = msr.lo;
	entry->hi = msr.hi;

	/* Return the next entry. */
	entry++;
	return entry;
}

static int save_bsp_msrs(char *start, int size)
{
	int msr_count;
	int num_var_mtrrs;
	struct saved_msr *msr_entry;
	int i;
	msr_t msr;

	/* Determine number of MTRRs need to be saved. */
	msr = rdmsr(MTRRcap_MSR);
	num_var_mtrrs = msr.lo & 0xff;

	/* 2 * num_var_mtrrs for base and mask. +1 for IA32_MTRR_DEF_TYPE. */
	msr_count = 2 * num_var_mtrrs + NUM_FIXED_MTRRS + 1;

	if ((msr_count * sizeof(struct saved_msr)) > size) {
		printk(BIOS_CRIT, "Cannot mirror all %d msrs.\n", msr_count);
		return -1;
	}

	msr_entry = (void *)start;
	for (i = 0; i < NUM_FIXED_MTRRS; i++) {
		msr_entry = save_msr(fixed_mtrrs[i], msr_entry);
	}

	for (i = 0; i < num_var_mtrrs; i++) {
		msr_entry = save_msr(MTRRphysBase_MSR(i), msr_entry);
		msr_entry = save_msr(MTRRphysMask_MSR(i), msr_entry);
	}

	msr_entry = save_msr(MTRRdefType_MSR, msr_entry);

	return msr_count;
}

/* The SIPI vector is loaded at the SMM_DEFAULT_BASE. The reason is at the
 * memory range is already reserved so the OS cannot use it. That region is
 * free to use for AP bringup before SMM is initialized. */
static u32 sipi_vector_location = SMM_DEFAULT_BASE;
static int sipi_vector_location_size = SMM_DEFAULT_SIZE;

static int load_sipi_vector(const void *microcode_patch)
{
	struct rmodule sipi_mod;
	int module_size;
	int num_msrs;
	struct sipi_params *sp;
	char *mod_loc = (void *)sipi_vector_location;
	const int loc_size = sipi_vector_location_size;

	if (rmodule_parse(&_binary_sipi_vector_start, &sipi_mod)) {
		printk(BIOS_CRIT, "Unable to parse sipi module.\n");
		return -1;
	}

	if (rmodule_entry_offset(&sipi_mod) != 0) {
		printk(BIOS_CRIT, "SIPI module entry offset is not 0!\n");
		return -1;
	}

	if (rmodule_load_alignment(&sipi_mod) != 4096) {
		printk(BIOS_CRIT, "SIPI module load alignment(%d) != 4096.\n",
		       rmodule_load_alignment(&sipi_mod));
		return -1;
	}

	module_size = rmodule_memory_size(&sipi_mod);

	/* Align to 4 bytes. */
	module_size += 3;
	module_size &= ~3;

	if (module_size > loc_size) {
		printk(BIOS_CRIT, "SIPI module size (%d) > region size (%d).\n",
		       module_size, loc_size);
		return -1;
	}

	num_msrs = save_bsp_msrs(&mod_loc[module_size], loc_size - module_size);

	if (num_msrs < 0) {
		printk(BIOS_CRIT, "Error mirroring BSP's msrs.\n");
		return -1;
	}

	if (rmodule_load(mod_loc, &sipi_mod)) {
		printk(BIOS_CRIT, "Unable to load SIPI module.\n");
		return -1;
	}

	sp = rmodule_parameters(&sipi_mod);

	if (sp == NULL) {
		printk(BIOS_CRIT, "SIPI module has no parameters.\n");
		return -1;
	}

	setup_default_sipi_vector_params(sp);
	/* Setup MSR table. */
	sp->msr_table_ptr = (u32)&mod_loc[module_size];
	sp->msr_count = num_msrs;
	/* Provide pointer to microcode patch. */
	sp->microcode_ptr = (u32)microcode_patch;
	/* The microcode pointer is passed on through to the c handler so
	 * that it can be loaded again after SMM relocation. */
	sp->c_handler_arg = (u32)microcode_patch;
	sp->c_handler = (u32)&ap_init;

	/* Make sure SIPI vector hits RAM so the APs that come up will see
	 * the startup code even if the caches are disabled.  */
	wbinvd();

	return 0;
}

static int allocate_cpu_devices(struct bus *cpu_bus, int *total_hw_threads)
{
	int i;
	int num_threads;
	int num_cores;
	int max_cpus;
	struct cpu_info *info;
	msr_t msr;

	info = cpu_info();
	cpu_devs[info->index] = info->cpu;
	apic_id_table[info->index] = info->cpu->path.apic.apic_id;

	msr = rdmsr(CORE_THREAD_COUNT_MSR);
	num_threads = (msr.lo >> 0) & 0xffff;
	num_cores = (msr.lo >> 16) & 0xffff;
	printk(BIOS_DEBUG, "CPU has %u cores, %u threads enabled.\n",
	       num_cores, num_threads);

	max_cpus = num_threads;
	*total_hw_threads = num_threads;
	if (num_threads > CONFIG_MAX_CPUS) {
		printk(BIOS_CRIT, "CPU count(%d) exceeds CONFIG_MAX_CPUS(%d)\n",
		       num_threads, CONFIG_MAX_CPUS);
		max_cpus = CONFIG_MAX_CPUS;
	}

	/* Determine if hyperthreading is enabled. If not, the APIC id space
	 * is sparse with ids incrementing by 2 instead of 1. */
	ht_disabled = num_threads == num_cores;

	for (i = 1; i < max_cpus; i++) {
		struct device_path cpu_path;
		device_t new;

		/* Build the cpu device path */
		cpu_path.type = DEVICE_PATH_APIC;
		cpu_path.apic.apic_id = info->cpu->path.apic.apic_id + i;
		if (ht_disabled)
			cpu_path.apic.apic_id = cpu_path.apic.apic_id * 2;

		/* Allocate the new cpu device structure */
		new = alloc_find_dev(cpu_bus, &cpu_path);
		if (new == NULL) {
			printk(BIOS_CRIT, "Could not allocate cpu device\n");
			max_cpus--;
		}
		cpu_devs[i] = new;
	}

	return max_cpus;
}

int setup_ap_init(struct bus *cpu_bus, int *max_cpus,
                  const void *microcode_patch)
{
	int num_cpus;
	int hw_threads;

	/* Default to currently running CPU. */
	num_cpus = allocate_cpu_devices(cpu_bus, &hw_threads);

	/* Load the SIPI vector. */
	if (load_sipi_vector(microcode_patch))
		return -1;

	*max_cpus = num_cpus;

	if (num_cpus < hw_threads) {
		printk(BIOS_CRIT,
		       "ERROR: More HW threads (%d) than support (%d).\n",
		       hw_threads, num_cpus);
		return -1;
	}

	return 0;
}

/* Returns 1 for timeout. 0 on success. */
static int apic_wait_timeout(int total_delay, int delay_step)
{
	int total = 0;
	int timeout = 0;

	while (lapic_read(LAPIC_ICR) & LAPIC_ICR_BUSY) {
		udelay(delay_step);
		total += delay_step;
		if (total >= total_delay) {
			timeout = 1;
			break;
		}
	}

	return timeout;
}

int start_aps(struct bus *cpu_bus, int ap_count)
{
	int sipi_vector;

	if (ap_count == 0)
		return 0;

	/* The vector is sent as a 4k aligned address in one byte. */
	sipi_vector = sipi_vector_location >> 12;

	if (sipi_vector > 256) {
		printk(BIOS_CRIT, "SIPI vector too large! 0x%08x\n",
		       sipi_vector);
		return -1;
	}

	printk(BIOS_DEBUG, "Attempting to start %d APs\n", ap_count);

	if ((lapic_read(LAPIC_ICR) & LAPIC_ICR_BUSY)) {
		printk(BIOS_DEBUG, "Waiting for ICR not to be busy...");
		if (apic_wait_timeout(1000 /* 1 ms */, 50)) {
			printk(BIOS_DEBUG, "timed out. Aborting.\n");
			return -1;
		} else
			printk(BIOS_DEBUG, "done.\n");
	}

	/* Send INIT IPI to all but self. */
	lapic_write_around(LAPIC_ICR2, SET_LAPIC_DEST_FIELD(0));
	lapic_write_around(LAPIC_ICR, LAPIC_DEST_ALLBUT | LAPIC_INT_ASSERT |
	                   LAPIC_DM_INIT);
	printk(BIOS_DEBUG, "Waiting for 10ms after sending INIT.\n");
	mdelay(10);

	/* Send 1st SIPI */
	if ((lapic_read(LAPIC_ICR) & LAPIC_ICR_BUSY)) {
		printk(BIOS_DEBUG, "Waiting for ICR not to be busy...");
		if (apic_wait_timeout(1000 /* 1 ms */, 50)) {
			printk(BIOS_DEBUG, "timed out. Aborting.\n");
			return -1;
		} else
			printk(BIOS_DEBUG, "done.\n");
	}

	lapic_write_around(LAPIC_ICR2, SET_LAPIC_DEST_FIELD(0));
	lapic_write_around(LAPIC_ICR, LAPIC_DEST_ALLBUT | LAPIC_INT_ASSERT |
	                   LAPIC_DM_STARTUP | sipi_vector);
	printk(BIOS_DEBUG, "Waiting for 1st SIPI to complete...");
	if (apic_wait_timeout(10000 /* 10 ms */, 50 /* us */)) {
		printk(BIOS_DEBUG, "timed out.\n");
		return -1;
	} else {
		printk(BIOS_DEBUG, "done.\n");
	}
	/* Wait for CPUs to check in up to 200 us. */
	wait_for_aps(&num_aps, ap_count, 200 /* us */, 15 /* us */);

	/* Send 2nd SIPI */
	if ((lapic_read(LAPIC_ICR) & LAPIC_ICR_BUSY)) {
		printk(BIOS_DEBUG, "Waiting for ICR not to be busy...");
		if (apic_wait_timeout(1000 /* 1 ms */, 50)) {
			printk(BIOS_DEBUG, "timed out. Aborting.\n");
			return -1;
		} else
			printk(BIOS_DEBUG, "done.\n");
	}

	lapic_write_around(LAPIC_ICR2, SET_LAPIC_DEST_FIELD(0));
	lapic_write_around(LAPIC_ICR, LAPIC_DEST_ALLBUT | LAPIC_INT_ASSERT |
	                   LAPIC_DM_STARTUP | sipi_vector);
	printk(BIOS_DEBUG, "Waiting for 2nd SIPI to complete...");
	if (apic_wait_timeout(10000 /* 10 ms */, 50 /* us */)) {
		printk(BIOS_DEBUG, "timed out.\n");
		return -1;
	} else {
		printk(BIOS_DEBUG, "done.\n");
	}

	/* Wait for CPUs to check in. */
	if (wait_for_aps(&num_aps, ap_count, 10000 /* 10 ms */, 50 /* us */)) {
		printk(BIOS_DEBUG, "Not all APs checked in: %d/%d.\n",
		       atomic_read(&num_aps), ap_count);
		return -1;
	}

	return 0;
}

void smm_initiate_relocation_parallel(void)
{
	if ((lapic_read(LAPIC_ICR) & LAPIC_ICR_BUSY)) {
		printk(BIOS_DEBUG, "Waiting for ICR not to be busy...");
		if (apic_wait_timeout(1000 /* 1 ms */, 50)) {
			printk(BIOS_DEBUG, "timed out. Aborting.\n");
			return;
		} else
			printk(BIOS_DEBUG, "done.\n");
	}

	lapic_write_around(LAPIC_ICR2, SET_LAPIC_DEST_FIELD(lapicid()));
	lapic_write_around(LAPIC_ICR, LAPIC_INT_ASSERT | LAPIC_DM_SMI);
	if (apic_wait_timeout(1000 /* 1 ms */, 100 /* us */)) {
		printk(BIOS_DEBUG, "SMI Relocation timed out.\n");
	} else
		printk(BIOS_DEBUG, "Relocation complete.\n");

}

DECLARE_SPIN_LOCK(smm_relocation_lock);

void smm_initiate_relocation(void)
{
	spin_lock(&smm_relocation_lock);
	smm_initiate_relocation_parallel();
	spin_unlock(&smm_relocation_lock);
}

