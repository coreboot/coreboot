/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2001 Eric Biederman
 * Copyright (C) 2001 Ronald G. Minnich
 * Copyright (C) 2005 Yinghai Lu
 * Copyright (C) 2008 coresystems GmbH
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>,
 * Raptor Engineering
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

#include <cpu/x86/cr.h>
#include <cpu/x86/gdt.h>
#include <cpu/x86/lapic.h>
#include <arch/acpi.h>
#include <delay.h>
#include <halt.h>
#include <lib.h>
#include <string.h>
#include <symbols.h>
#include <console/console.h>
#include <device/device.h>
#include <device/path.h>
#include <smp/atomic.h>
#include <smp/spinlock.h>
#include <cpu/cpu.h>
#include <cpu/intel/speedstep.h>
#include <thread.h>

/* This is a lot more paranoid now, since Linux can NOT handle
 * being told there is a CPU when none exists. So any errors
 * will return 0, meaning no CPU.
 *
 * We actually handling that case by noting which cpus startup
 * and not telling anyone about the ones that don't.
 */

/* Start-UP IPI vector must be 4kB aligned and below 1MB. */
#define AP_SIPI_VECTOR 0x1000

static char *lowmem_backup;
static char *lowmem_backup_ptr;
static int  lowmem_backup_size;

static inline void setup_secondary_gdt(void)
{
	u16 *gdt_limit;
#ifdef __x86_64__
	u64 *gdt_base;
#else
	u32 *gdt_base;
#endif

	gdt_limit = (void *)&_secondary_gdt_addr;
	gdt_base = (void *)&gdt_limit[1];

	*gdt_limit = (uintptr_t)&gdt_end - (uintptr_t)&gdt - 1;
	*gdt_base = (uintptr_t)&gdt;
}

static void copy_secondary_start_to_lowest_1M(void)
{
	unsigned long code_size;

	/* Fill in secondary_start's local gdt. */
	setup_secondary_gdt();

	code_size = (unsigned long)_secondary_start_end
		- (unsigned long)_secondary_start;

	if (acpi_is_wakeup_s3()) {
		/* need to save it for RAM resume */
		lowmem_backup_size = code_size;
		lowmem_backup = malloc(code_size);
		lowmem_backup_ptr = (char *)AP_SIPI_VECTOR;

		if (lowmem_backup == NULL)
			die("Out of backup memory\n");

		memcpy(lowmem_backup, lowmem_backup_ptr, lowmem_backup_size);
	}

	/* copy the _secondary_start to the RAM below 1M*/
	memcpy((unsigned char *)AP_SIPI_VECTOR,
		(unsigned char *)_secondary_start, code_size);

	printk(BIOS_DEBUG, "start_eip=0x%08lx, code_size=0x%08lx\n",
		(unsigned long int)AP_SIPI_VECTOR, code_size);
}

static void recover_lowest_1M(void)
{
	if (acpi_is_wakeup_s3())
		memcpy(lowmem_backup_ptr, lowmem_backup, lowmem_backup_size);
}

static int lapic_start_cpu(unsigned long apicid)
{
	int timeout;
	unsigned long send_status, accept_status;
	int j, maxlvt;

	/*
	 * Starting actual IPI sequence...
	 */

	printk(BIOS_SPEW, "Asserting INIT.\n");

	/*
	 * Turn INIT on target chip
	 */
	lapic_write_around(LAPIC_ICR2, SET_LAPIC_DEST_FIELD(apicid));

	/*
	 * Send IPI
	 */

	lapic_write_around(LAPIC_ICR, LAPIC_INT_LEVELTRIG | LAPIC_INT_ASSERT
				| LAPIC_DM_INIT);

	printk(BIOS_SPEW, "Waiting for send to finish...\n");
	timeout = 0;
	do {
		printk(BIOS_SPEW, "+");
		udelay(100);
		send_status = lapic_read(LAPIC_ICR) & LAPIC_ICR_BUSY;
	} while (send_status && (timeout++ < 1000));
	if (timeout >= 1000) {
		printk(BIOS_ERR, "CPU %ld: First APIC write timed out. "
			"Disabling\n", apicid);
		// too bad.
		printk(BIOS_ERR, "ESR is 0x%lx\n", lapic_read(LAPIC_ESR));
		if (lapic_read(LAPIC_ESR)) {
			printk(BIOS_ERR, "Try to reset ESR\n");
			lapic_write_around(LAPIC_ESR, 0);
			printk(BIOS_ERR, "ESR is 0x%lx\n",
				lapic_read(LAPIC_ESR));
		}
		return 0;
	}
#if !IS_ENABLED(CONFIG_CPU_AMD_MODEL_10XXX) \
	&& !IS_ENABLED(CONFIG_CPU_INTEL_MODEL_206AX) \
	&& !IS_ENABLED(CONFIG_CPU_INTEL_MODEL_2065X)
	mdelay(10);
#endif

	printk(BIOS_SPEW, "Deasserting INIT.\n");

	/* Target chip */
	lapic_write_around(LAPIC_ICR2, SET_LAPIC_DEST_FIELD(apicid));

	/* Send IPI */
	lapic_write_around(LAPIC_ICR, LAPIC_INT_LEVELTRIG | LAPIC_DM_INIT);

	printk(BIOS_SPEW, "Waiting for send to finish...\n");
	timeout = 0;
	do {
		printk(BIOS_SPEW, "+");
		udelay(100);
		send_status = lapic_read(LAPIC_ICR) & LAPIC_ICR_BUSY;
	} while (send_status && (timeout++ < 1000));
	if (timeout >= 1000) {
		printk(BIOS_ERR, "CPU %ld: Second APIC write timed out. "
			"Disabling\n", apicid);
		// too bad.
		return 0;
	}

	/*
	 * Run STARTUP IPI loop.
	 */
	printk(BIOS_SPEW, "#startup loops: %d.\n", CONFIG_NUM_IPI_STARTS);

	maxlvt = 4;

	for (j = 1; j <= CONFIG_NUM_IPI_STARTS; j++) {
		printk(BIOS_SPEW, "Sending STARTUP #%d to %lu.\n", j, apicid);
		lapic_read_around(LAPIC_SPIV);
		lapic_write(LAPIC_ESR, 0);
		lapic_read(LAPIC_ESR);
		printk(BIOS_SPEW, "After apic_write.\n");

		/*
		 * STARTUP IPI
		 */

		/* Target chip */
		lapic_write_around(LAPIC_ICR2, SET_LAPIC_DEST_FIELD(apicid));

		/* Boot on the stack */
		/* Kick the second */
		lapic_write_around(LAPIC_ICR, LAPIC_DM_STARTUP
					| (AP_SIPI_VECTOR >> 12));

		/*
		 * Give the other CPU some time to accept the IPI.
		 */
		udelay(300);

		printk(BIOS_SPEW, "Startup point 1.\n");

		printk(BIOS_SPEW, "Waiting for send to finish...\n");
		timeout = 0;
		do {
			printk(BIOS_SPEW, "+");
			udelay(100);
			send_status = lapic_read(LAPIC_ICR) & LAPIC_ICR_BUSY;
		} while (send_status && (timeout++ < 1000));

		/*
		 * Give the other CPU some time to accept the IPI.
		 */
		udelay(200);
		/*
		 * Due to the Pentium erratum 3AP.
		 */
		if (maxlvt > 3) {
			lapic_read_around(LAPIC_SPIV);
			lapic_write(LAPIC_ESR, 0);
		}
		accept_status = (lapic_read(LAPIC_ESR) & 0xEF);
		if (send_status || accept_status)
			break;
	}
	printk(BIOS_SPEW, "After Startup.\n");
	if (send_status)
		printk(BIOS_WARNING, "APIC never delivered???\n");
	if (accept_status)
		printk(BIOS_WARNING, "APIC delivery error (%lx).\n",
			accept_status);
	if (send_status || accept_status)
		return 0;
	return 1;
}

/* Number of cpus that are currently running in coreboot */
static atomic_t active_cpus = ATOMIC_INIT(1);

/* start_cpu_lock covers last_cpu_index and secondary_stack.
 * Only starting one CPU at a time let's me remove the logic
 * for select the stack from assembly language.
 *
 * In addition communicating by variables to the CPU I
 * am starting allows me to verify it has started before
 * start_cpu returns.
 */

DECLARE_SPIN_LOCK(start_cpu_lock);
static unsigned int last_cpu_index = 0;
static void *stacks[CONFIG_MAX_CPUS];
volatile unsigned long secondary_stack;
volatile unsigned int secondary_cpu_index;

int start_cpu(struct device *cpu)
{
	struct cpu_info *info;
	unsigned long stack_end;
	unsigned long stack_base;
	unsigned long *stack;
	unsigned long apicid;
	unsigned int index;
	unsigned long count;
	int i;
	int result;

	spin_lock(&start_cpu_lock);

	/* Get the CPU's apicid */
	apicid = cpu->path.apic.apic_id;

	/* Get an index for the new processor */
	index = ++last_cpu_index;

	/* Find end of the new processor's stack */
	stack_end = ((unsigned long)_estack) - (CONFIG_STACK_SIZE*index) -
			sizeof(struct cpu_info);

	stack_base = ((unsigned long)_estack) - (CONFIG_STACK_SIZE*(index+1));
	printk(BIOS_SPEW, "CPU%d: stack_base %p, stack_end %p\n", index,
		(void *)stack_base, (void *)stack_end);
	/* poison the stack */
	for (stack = (void *)stack_base, i = 0; i < CONFIG_STACK_SIZE; i++)
		stack[i/sizeof(*stack)] = 0xDEADBEEF;
	stacks[index] = stack;
	/* Record the index and which CPU structure we are using */
	info = (struct cpu_info *)stack_end;
	info->index = index;
	info->cpu   = cpu;
	thread_init_cpu_info_non_bsp(info);

	/* Advertise the new stack and index to start_cpu */
	secondary_stack = stack_end;
	secondary_cpu_index = index;

	/* Until the CPU starts up report the CPU is not enabled */
	cpu->enabled = 0;
	cpu->initialized = 0;

	/* Start the CPU */
	result = lapic_start_cpu(apicid);

	if (result) {
		result = 0;
		/* Wait 1s or until the new CPU calls in */
		for (count = 0; count < 100000; count++) {
			if (secondary_stack == 0) {
				result = 1;
				break;
			}
			udelay(10);
		}
	}
	secondary_stack = 0;
	spin_unlock(&start_cpu_lock);
	return result;
}

#if IS_ENABLED(CONFIG_AP_IN_SIPI_WAIT)

/**
 * Sending INIT IPI to self is equivalent of asserting #INIT with a bit of
 * delay.
 * An undefined number of instruction cycles will complete. All global locks
 * must be released before INIT IPI and no printk is allowed after this.
 * De-asserting INIT IPI is a no-op on later Intel CPUs.
 *
 * If you set DEBUG_HALT_SELF to 1, printk's after INIT IPI are enabled
 * but running thread may halt without releasing the lock and effectively
 * deadlock other CPUs.
 */
#define DEBUG_HALT_SELF 0

/**
 * Normally this function is defined in lapic.h as an always inline function
 * that just keeps the CPU in a hlt() loop. This does not work on all CPUs.
 * I think all hyperthreading CPUs might need this version, but I could only
 * verify this on the Intel Core Duo
 */
void stop_this_cpu(void)
{
	int timeout;
	unsigned long send_status;
	unsigned long id;

	id = lapicid();

	printk(BIOS_DEBUG, "CPU %ld going down...\n", id);

	/* send an LAPIC INIT to myself */
	lapic_write_around(LAPIC_ICR2, SET_LAPIC_DEST_FIELD(id));
	lapic_write_around(LAPIC_ICR, LAPIC_INT_LEVELTRIG |
				LAPIC_INT_ASSERT | LAPIC_DM_INIT);

	/* wait for the ipi send to finish */
#if DEBUG_HALT_SELF
	printk(BIOS_SPEW, "Waiting for send to finish...\n");
#endif
	timeout = 0;
	do {
#if DEBUG_HALT_SELF
		printk(BIOS_SPEW, "+");
#endif
		udelay(100);
		send_status = lapic_read(LAPIC_ICR) & LAPIC_ICR_BUSY;
	} while (send_status && (timeout++ < 1000));
	if (timeout >= 1000) {
#if DEBUG_HALT_SELF
		printk(BIOS_ERR, "timed out\n");
#endif
	}
	mdelay(10);

#if DEBUG_HALT_SELF
	printk(BIOS_SPEW, "Deasserting INIT.\n");
#endif
	/* Deassert the LAPIC INIT */
	lapic_write_around(LAPIC_ICR2, SET_LAPIC_DEST_FIELD(id));
	lapic_write_around(LAPIC_ICR, LAPIC_INT_LEVELTRIG | LAPIC_DM_INIT);

#if DEBUG_HALT_SELF
	printk(BIOS_SPEW, "Waiting for send to finish...\n");
#endif
	timeout = 0;
	do {
#if DEBUG_HALT_SELF
		printk(BIOS_SPEW, "+");
#endif
		udelay(100);
		send_status = lapic_read(LAPIC_ICR) & LAPIC_ICR_BUSY;
	} while (send_status && (timeout++ < 1000));
	if (timeout >= 1000) {
#if DEBUG_HALT_SELF
		printk(BIOS_ERR, "timed out\n");
#endif
	}

	halt();
}
#endif

/* C entry point of secondary cpus */
asmlinkage void secondary_cpu_init(unsigned int index)
{
	atomic_inc(&active_cpus);

	if (!IS_ENABLED(CONFIG_PARALLEL_CPU_INIT))
		spin_lock(&start_cpu_lock);

#ifdef __SSE3__
	/*
	 * Seems that CR4 was cleared when AP start via lapic_start_cpu()
	 * Turn on CR4.OSFXSR and CR4.OSXMMEXCPT when SSE options enabled
	 */
	u32 cr4_val;
	cr4_val = read_cr4();
	cr4_val |= (CR4_OSFXSR | CR4_OSXMMEXCPT);
	write_cr4(cr4_val);
#endif
	cpu_initialize(index);

	if (!IS_ENABLED(CONFIG_PARALLEL_CPU_INIT))
		spin_unlock(&start_cpu_lock);

	atomic_dec(&active_cpus);

	stop_this_cpu();
}

static void start_other_cpus(struct bus *cpu_bus, struct device *bsp_cpu)
{
	struct device *cpu;
	/* Loop through the cpus once getting them started */

	for (cpu = cpu_bus->children; cpu; cpu = cpu->sibling) {
		if (cpu->path.type != DEVICE_PATH_APIC)
			continue;

		if (IS_ENABLED(CONFIG_PARALLEL_CPU_INIT) && (cpu == bsp_cpu))
			continue;

		if (!cpu->enabled)
			continue;

		if (cpu->initialized)
			continue;

		if (!start_cpu(cpu))
			/* Record the error in cpu? */
			printk(BIOS_ERR, "CPU 0x%02x would not start!\n",
				cpu->path.apic.apic_id);

		if (!IS_ENABLED(CONFIG_PARALLEL_CPU_INIT))
			udelay(10);
	}

}

static void smm_other_cpus(struct bus *cpu_bus, device_t bsp_cpu)
{
	device_t cpu;
	int pre_count = atomic_read(&active_cpus);

	/* Loop through the cpus once to let them run through SMM relocator */

	for (cpu = cpu_bus->children; cpu; cpu = cpu->sibling) {
		if (cpu->path.type != DEVICE_PATH_APIC)
			continue;

		printk(BIOS_ERR, "considering CPU 0x%02x for SMM init\n",
			cpu->path.apic.apic_id);

		if (cpu == bsp_cpu)
			continue;

		if (!cpu->enabled)
			continue;

		if (!start_cpu(cpu))
			/* Record the error in cpu? */
			printk(BIOS_ERR, "CPU 0x%02x would not start!\n",
				cpu->path.apic.apic_id);

		/* FIXME: endless loop */
		while (atomic_read(&active_cpus) != pre_count)
			;
	}
}

static void wait_other_cpus_stop(struct bus *cpu_bus)
{
	struct device *cpu;
	int old_active_count, active_count;
	long loopcount = 0;
	int i;

	/* Now loop until the other cpus have finished initializing */
	old_active_count = 1;
	active_count = atomic_read(&active_cpus);
	while (active_count > 1) {
		if (active_count != old_active_count) {
			printk(BIOS_INFO, "Waiting for %d CPUS to stop\n",
				active_count - 1);
			old_active_count = active_count;
		}
		udelay(10);
		active_count = atomic_read(&active_cpus);
		loopcount++;
	}
	for (cpu = cpu_bus->children; cpu; cpu = cpu->sibling) {
		if (cpu->path.type != DEVICE_PATH_APIC)
			continue;
		if (cpu->path.apic.apic_id == SPEEDSTEP_APIC_MAGIC)
			continue;
		if (!cpu->initialized)
			printk(BIOS_ERR, "CPU 0x%02x did not initialize!\n",
				cpu->path.apic.apic_id);
	}
	printk(BIOS_DEBUG, "All AP CPUs stopped (%ld loops)\n", loopcount);
	checkstack(_estack, 0);
	for (i = 1; i < CONFIG_MAX_CPUS && i <= last_cpu_index; i++)
		checkstack((void *)stacks[i] + CONFIG_STACK_SIZE, i);
}

void initialize_cpus(struct bus *cpu_bus)
{
	struct device_path cpu_path;
	struct cpu_info *info;

	/* Find the info struct for this CPU */
	info = cpu_info();

	if (need_lapic_init()) {
		/* Ensure the local APIC is enabled */
		enable_lapic();

		/* Get the device path of the boot CPU */
		cpu_path.type           = DEVICE_PATH_APIC;
		cpu_path.apic.apic_id = lapicid();
	} else {
		/* Get the device path of the boot CPU */
		cpu_path.type = DEVICE_PATH_CPU;
		cpu_path.cpu.id       = 0;
	}

	/* Find the device structure for the boot CPU */
	info->cpu = alloc_find_dev(cpu_bus, &cpu_path);

	// why here? In case some day we can start core1 in amd_sibling_init
	if (is_smp_boot())
		copy_secondary_start_to_lowest_1M();

	if (!IS_ENABLED(CONFIG_SERIALIZED_SMM_INITIALIZATION))
		smm_init();

	/* start all aps at first, so we can init ECC all together */
	if (is_smp_boot() && IS_ENABLED(CONFIG_PARALLEL_CPU_INIT))
		start_other_cpus(cpu_bus, info->cpu);

	/* Initialize the bootstrap processor */
	cpu_initialize(0);

	if (is_smp_boot() && !IS_ENABLED(CONFIG_PARALLEL_CPU_INIT))
		start_other_cpus(cpu_bus, info->cpu);

	/* Now wait the rest of the cpus stop*/
	if (is_smp_boot())
		wait_other_cpus_stop(cpu_bus);

	if (IS_ENABLED(CONFIG_SERIALIZED_SMM_INITIALIZATION)) {
		/* At this point, all APs are sleeping:
		 * smm_init() will queue a pending SMI on all cpus
		 * and smm_other_cpus() will start them one by one */
		smm_init();

		if (is_smp_boot()) {
			last_cpu_index = 0;
			smm_other_cpus(cpu_bus, info->cpu);
		}
	}

	smm_init_completion();

	if (is_smp_boot())
		recover_lowest_1M();
}

#if !IS_ENABLED(CONFIG_HAVE_SMI_HANDLER)
/* Empty stubs for platforms without SMI handlers. */
void smm_init(void)
{
}

void smm_init_completion(void)
{
}
#endif
