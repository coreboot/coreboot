/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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
 * Foundation, Inc.
 */

#include <console/console.h>
#include <stdint.h>
#include <rmodule.h>
#include <arch/cpu.h>
#include <cpu/cpu.h>
#include <cpu/intel/microcode.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/gdt.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/name.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/smm.h>
#include <cpu/x86/mp.h>
#include <delay.h>
#include <device/device.h>
#include <device/path.h>
#include <lib.h>
#include <smp/atomic.h>
#include <smp/spinlock.h>
#include <symbols.h>
#include <thread.h>

#define MAX_APIC_IDS 256
/* This needs to match the layout in the .module_parametrs section. */
struct sipi_params {
	uint16_t gdtlimit;
	uint32_t gdt;
	uint16_t unused;
	uint32_t idt_ptr;
	uint32_t stack_top;
	uint32_t stack_size;
	uint32_t microcode_lock; /* 0xffffffff means parallel loading. */
	uint32_t microcode_ptr;
	uint32_t msr_table_ptr;
	uint32_t msr_count;
	uint32_t c_handler;
	atomic_t ap_count;
} __attribute__((packed));

/* This also needs to match the assembly code for saved MSR encoding. */
struct saved_msr {
	uint32_t index;
	uint32_t lo;
	uint32_t hi;
} __attribute__((packed));


/* The sipi vector rmodule is included in the ramstage using 'objdump -B'. */
extern char _binary_sipi_vector_start[];

/* The SIPI vector is loaded at the SMM_DEFAULT_BASE. The reason is at the
 * memory range is already reserved so the OS cannot use it. That region is
 * free to use for AP bringup before SMM is initialized. */
static const uint32_t sipi_vector_location = SMM_DEFAULT_BASE;
static const int sipi_vector_location_size = SMM_DEFAULT_SIZE;

struct mp_flight_plan {
	int num_records;
	struct mp_flight_record *records;
};

static struct mp_flight_plan mp_info;

struct cpu_map {
	struct device *dev;
	int apic_id;
};

/* Keep track of apic and device structure for each cpu. */
static struct cpu_map cpus[CONFIG_MAX_CPUS];

static inline void barrier_wait(atomic_t *b)
{
	while (atomic_read(b) == 0) {
		asm ("pause");
	}
	mfence();
}

static inline void release_barrier(atomic_t *b)
{
	mfence();
	atomic_set(b, 1);
}

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

static void ap_do_flight_plan(void)
{
	int i;

	for (i = 0; i < mp_info.num_records; i++) {
		struct mp_flight_record *rec = &mp_info.records[i];

		atomic_inc(&rec->cpus_entered);
		barrier_wait(&rec->barrier);

		if (rec->ap_call != NULL) {
			rec->ap_call(rec->ap_arg);
		}
	}
}

/* By the time APs call ap_init() caching has been setup, and microcode has
 * been loaded. */
static void asmlinkage ap_init(unsigned int cpu)
{
	struct cpu_info *info;
	int apic_id;

	/* Ensure the local apic is enabled */
	enable_lapic();

	info = cpu_info();
	info->index = cpu;
	info->cpu = cpus[cpu].dev;
	thread_init_cpu_info_non_bsp(info);

	apic_id = lapicid();
	info->cpu->path.apic.apic_id = apic_id;
	cpus[cpu].apic_id = apic_id;

	printk(BIOS_INFO, "AP: slot %d apic_id %x.\n", cpu, apic_id);

	/* Walk the flight plan */
	ap_do_flight_plan();

	/* Park the AP. */
	stop_this_cpu();
}

static void setup_default_sipi_vector_params(struct sipi_params *sp)
{
	sp->gdt = (uint32_t)&gdt;
	sp->gdtlimit = (uint32_t)&gdt_end - (u32)&gdt - 1;
	sp->idt_ptr = (uint32_t)&idtarg;
	sp->stack_size = CONFIG_STACK_SIZE;
	sp->stack_top = (uint32_t)&_estack;
	/* Adjust the stack top to take into account cpu_info. */
	sp->stack_top -= sizeof(struct cpu_info);
}

#define NUM_FIXED_MTRRS 11
static const unsigned int fixed_mtrrs[NUM_FIXED_MTRRS] = {
	MTRR_FIX_64K_00000, MTRR_FIX_16K_80000, MTRR_FIX_16K_A0000,
	MTRR_FIX_4K_C0000, MTRR_FIX_4K_C8000, MTRR_FIX_4K_D0000,
	MTRR_FIX_4K_D8000, MTRR_FIX_4K_E0000, MTRR_FIX_4K_E8000,
	MTRR_FIX_4K_F0000, MTRR_FIX_4K_F8000,
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
	msr = rdmsr(MTRR_CAP_MSR);
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
		msr_entry = save_msr(MTRR_PHYS_BASE(i), msr_entry);
		msr_entry = save_msr(MTRR_PHYS_MASK(i), msr_entry);
	}

	msr_entry = save_msr(MTRR_DEF_TYPE_MSR, msr_entry);

	return msr_count;
}

static atomic_t *load_sipi_vector(struct mp_params *mp_params)
{
	struct rmodule sipi_mod;
	int module_size;
	int num_msrs;
	struct sipi_params *sp;
	char *mod_loc = (void *)sipi_vector_location;
	const int loc_size = sipi_vector_location_size;
	atomic_t *ap_count = NULL;

	if (rmodule_parse(&_binary_sipi_vector_start, &sipi_mod)) {
		printk(BIOS_CRIT, "Unable to parse sipi module.\n");
		return ap_count;
	}

	if (rmodule_entry_offset(&sipi_mod) != 0) {
		printk(BIOS_CRIT, "SIPI module entry offset is not 0!\n");
		return ap_count;
	}

	if (rmodule_load_alignment(&sipi_mod) != 4096) {
		printk(BIOS_CRIT, "SIPI module load alignment(%d) != 4096.\n",
		       rmodule_load_alignment(&sipi_mod));
		return ap_count;
	}

	module_size = rmodule_memory_size(&sipi_mod);

	/* Align to 4 bytes. */
	module_size = ALIGN(module_size, 4);

	if (module_size > loc_size) {
		printk(BIOS_CRIT, "SIPI module size (%d) > region size (%d).\n",
		       module_size, loc_size);
		return ap_count;
	}

	num_msrs = save_bsp_msrs(&mod_loc[module_size], loc_size - module_size);

	if (num_msrs < 0) {
		printk(BIOS_CRIT, "Error mirroring BSP's msrs.\n");
		return ap_count;
	}

	if (rmodule_load(mod_loc, &sipi_mod)) {
		printk(BIOS_CRIT, "Unable to load SIPI module.\n");
		return ap_count;
	}

	sp = rmodule_parameters(&sipi_mod);

	if (sp == NULL) {
		printk(BIOS_CRIT, "SIPI module has no parameters.\n");
		return ap_count;
	}

	setup_default_sipi_vector_params(sp);
	/* Setup MSR table. */
	sp->msr_table_ptr = (uint32_t)&mod_loc[module_size];
	sp->msr_count = num_msrs;
	/* Provide pointer to microcode patch. */
	sp->microcode_ptr = (uint32_t)mp_params->microcode_pointer;
	/* Pass on abiility to load microcode in parallel. */
	if (mp_params->parallel_microcode_load) {
		sp->microcode_lock = 0;
	} else {
		sp->microcode_lock = ~0;
	}
	sp->c_handler = (uint32_t)&ap_init;
	ap_count = &sp->ap_count;
	atomic_set(ap_count, 0);

	return ap_count;
}

static int allocate_cpu_devices(struct bus *cpu_bus, struct mp_params *p)
{
	int i;
	int max_cpus;
	struct cpu_info *info;

	max_cpus = p->num_cpus;
	if (max_cpus > CONFIG_MAX_CPUS) {
		printk(BIOS_CRIT, "CPU count(%d) exceeds CONFIG_MAX_CPUS(%d)\n",
		       max_cpus, CONFIG_MAX_CPUS);
		max_cpus = CONFIG_MAX_CPUS;
	}

	info = cpu_info();
	for (i = 1; i < max_cpus; i++) {
		struct device_path cpu_path;
		struct device *new;
		int apic_id;

		/* Build the cpu device path */
		cpu_path.type = DEVICE_PATH_APIC;

		/* Assuming linear APIC space allocation. */
		apic_id = info->cpu->path.apic.apic_id + i;
		if (p->adjust_apic_id != NULL) {
			apic_id = p->adjust_apic_id(i, apic_id);
		}
		cpu_path.apic.apic_id = apic_id;

		/* Allocate the new cpu device structure */
		new = alloc_find_dev(cpu_bus, &cpu_path);
		if (new == NULL) {
			printk(BIOS_CRIT, "Could not allocte cpu device\n");
			max_cpus--;
		}
		cpus[i].dev = new;
	}

	return max_cpus;
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

static int start_aps(struct bus *cpu_bus, int ap_count, atomic_t *num_aps)
{
	int sipi_vector;
	/* Max location is 4KiB below 1MiB */
	const int max_vector_loc = ((1 << 20) - (1 << 12)) >> 12;

	if (ap_count == 0)
		return 0;

	/* The vector is sent as a 4k aligned address in one byte. */
	sipi_vector = sipi_vector_location >> 12;

	if (sipi_vector > max_vector_loc) {
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
	wait_for_aps(num_aps, ap_count, 200 /* us */, 15 /* us */);

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
	if (wait_for_aps(num_aps, ap_count, 10000 /* 10 ms */, 50 /* us */)) {
		printk(BIOS_DEBUG, "Not all APs checked in: %d/%d.\n",
		       atomic_read(num_aps), ap_count);
		return -1;
	}

	return 0;
}

static int bsp_do_flight_plan(struct mp_params *mp_params)
{
	int i;
	int ret = 0;
	const int timeout_us = 100000;
	const int step_us = 100;
	int num_aps = mp_params->num_cpus - 1;

	for (i = 0; i < mp_params->num_records; i++) {
		struct mp_flight_record *rec = &mp_params->flight_plan[i];

		/* Wait for APs if the record is not released. */
		if (atomic_read(&rec->barrier) == 0) {
			/* Wait for the APs to check in. */
			if (wait_for_aps(&rec->cpus_entered, num_aps,
			                 timeout_us, step_us)) {
				printk(BIOS_ERR, "MP record %d timeout.\n", i);
				ret = -1;
			}
		}

		if (rec->bsp_call != NULL) {
			rec->bsp_call(rec->bsp_arg);
		}

		release_barrier(&rec->barrier);
	}
	return ret;
}

static void init_bsp(struct bus *cpu_bus)
{
	struct device_path cpu_path;
	struct cpu_info *info;
	char processor_name[49];

	/* Print processor name */
	fill_processor_name(processor_name);
	printk(BIOS_INFO, "CPU: %s.\n", processor_name);

	/* Ensure the local apic is enabled */
	enable_lapic();

	/* Set the device path of the boot cpu. */
	cpu_path.type = DEVICE_PATH_APIC;
	cpu_path.apic.apic_id = lapicid();

	/* Find the device structure for the boot cpu. */
	info = cpu_info();
	info->cpu = alloc_find_dev(cpu_bus, &cpu_path);

	if (info->index != 0)
		printk(BIOS_CRIT, "BSP index(%d) != 0!\n", info->index);

	/* Track BSP in cpu_map structures. */
	cpus[info->index].dev = info->cpu;
	cpus[info->index].apic_id = cpu_path.apic.apic_id;
}

int mp_init(struct bus *cpu_bus, struct mp_params *p)
{
	int num_cpus;
	int num_aps;
	atomic_t *ap_count;

	init_bsp(cpu_bus);

	if (p == NULL || p->flight_plan == NULL || p->num_records < 1) {
		printk(BIOS_CRIT, "Invalid MP parameters\n");
		return -1;
	}

	/* Default to currently running CPU. */
	num_cpus = allocate_cpu_devices(cpu_bus, p);

	if (num_cpus < p->num_cpus) {
		printk(BIOS_CRIT,
		       "ERROR: More cpus requested (%d) than supported (%d).\n",
		       p->num_cpus, num_cpus);
		return -1;
	}

	/* Copy needed parameters so that APs have a reference to the plan. */
	mp_info.num_records = p->num_records;
	mp_info.records = p->flight_plan;

	/* Load the SIPI vector. */
	ap_count = load_sipi_vector(p);
	if (ap_count == NULL)
		return -1;

	/* Make sure SIPI data hits RAM so the APs that come up will see
	 * the startup code even if the caches are disabled.  */
	wbinvd();

	/* Start the APs providing number of APs and the cpus_entered field. */
	num_aps = p->num_cpus - 1;
	if (start_aps(cpu_bus, num_aps, ap_count) < 0) {
		mdelay(1000);
		printk(BIOS_DEBUG, "%d/%d eventually checked in?\n",
		       atomic_read(ap_count), num_aps);
		return -1;
	}

	/* Walk the flight plan for the BSP. */
	return bsp_do_flight_plan(p);
}

void mp_initialize_cpu(void *unused)
{
	/* Call back into driver infrastructure for the AP initialization.   */
	struct cpu_info *info = cpu_info();
	cpu_initialize(info->index);
}

int mp_get_apic_id(int cpu_slot)
{
	if (cpu_slot >= CONFIG_MAX_CPUS || cpu_slot < 0)
		return -1;

	return cpus[cpu_slot].apic_id;
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

/* Send SMI to self with single user serialization. */
void smm_initiate_relocation(void)
{
	spin_lock(&smm_relocation_lock);
	smm_initiate_relocation_parallel();
	spin_unlock(&smm_relocation_lock);
}
