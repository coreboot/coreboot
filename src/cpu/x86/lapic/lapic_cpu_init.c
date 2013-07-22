/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2001 Eric Biederman
 * Copyright (C) 2001 Ronald G. Minnich
 * Copyright (C) 2005 Yinghai Lu
 * Copyright (C) 2008 coresystems GmbH
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

#include <cpu/x86/lapic.h>
#include <delay.h>
#include <lib.h>
#include <string.h>
#include <console/console.h>
#include <arch/hlt.h>
#include <device/device.h>
#include <device/path.h>
#include <smp/atomic.h>
#include <smp/spinlock.h>
#include <cpu/cpu.h>
#include <cpu/intel/speedstep.h>
#include <thread.h>

#if CONFIG_SMP && CONFIG_MAX_CPUS > 1
/* This is a lot more paranoid now, since Linux can NOT handle
 * being told there is a CPU when none exists. So any errors
 * will return 0, meaning no CPU.
 *
 * We actually handling that case by noting which cpus startup
 * and not telling anyone about the ones that don't.
 */

/* Start-UP IPI vector must be 4kB aligned and below 1MB. */
#define AP_SIPI_VECTOR 0x1000

#if CONFIG_HAVE_ACPI_RESUME
char *lowmem_backup;
char *lowmem_backup_ptr;
int  lowmem_backup_size;
#endif

extern char _secondary_start[];
extern char _secondary_gdt_addr[];
extern char gdt[];
extern char gdt_end[];

static inline void setup_secondary_gdt(void)
{
	u16 *gdt_limit;
	u32 *gdt_base;

	gdt_limit = (void *)&_secondary_gdt_addr;
	gdt_base = (void *)&gdt_limit[1];

	*gdt_limit = (u32)&gdt_end - (u32)&gdt - 1;
	*gdt_base = (u32)&gdt;
}

static void copy_secondary_start_to_lowest_1M(void)
{
	extern char _secondary_start_end[];
	unsigned long code_size;

	/* Fill in secondary_start's local gdt. */
	setup_secondary_gdt();

	code_size = (unsigned long)_secondary_start_end - (unsigned long)_secondary_start;

#if CONFIG_HAVE_ACPI_RESUME
	/* need to save it for RAM resume */
	lowmem_backup_size = code_size;
	lowmem_backup = malloc(code_size);
	lowmem_backup_ptr = (char *)AP_SIPI_VECTOR;

	if (lowmem_backup == NULL)
		die("Out of backup memory\n");

	memcpy(lowmem_backup, lowmem_backup_ptr, lowmem_backup_size);
#endif
	/* copy the _secondary_start to the ram below 1M*/
	memcpy((unsigned char *)AP_SIPI_VECTOR, (unsigned char *)_secondary_start, code_size);

	printk(BIOS_DEBUG, "start_eip=0x%08lx, code_size=0x%08lx\n",
		(long unsigned int)AP_SIPI_VECTOR, code_size);
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
#if !CONFIG_CPU_AMD_MODEL_10XXX && !CONFIG_CPU_INTEL_MODEL_206AX
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
		printk(BIOS_ERR, "CPU %ld: Second apic write timed out. "
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
 * Only starting one cpu at a time let's me remove the logic
 * for select the stack from assembly language.
 *
 * In addition communicating by variables to the cpu I
 * am starting allows me to verify it has started before
 * start_cpu returns.
 */

static spinlock_t start_cpu_lock = SPIN_LOCK_UNLOCKED;
static unsigned int last_cpu_index = 0;
static void *stacks[CONFIG_MAX_CPUS];
volatile unsigned long secondary_stack;
volatile unsigned int secondary_cpu_index;

int start_cpu(device_t cpu)
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
	for(stack = (void *)stack_base, i = 0; i < CONFIG_STACK_SIZE; i++)
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

	/* Start the cpu */
	result = lapic_start_cpu(apicid);

	if (result) {
		result = 0;
		/* Wait 1s or until the new cpu calls in */
		for(count = 0; count < 100000 ; count++) {
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

#if CONFIG_AP_IN_SIPI_WAIT

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

	id = lapic_read(LAPIC_ID) >> 24;

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

	while(1) {
		hlt();
	}
}
#endif

#ifdef __SSE3__
static __inline__ __attribute__((always_inline)) unsigned long readcr4(void)
{
	unsigned long value;
	__asm__ __volatile__ (
			"mov %%cr4, %[value]"
			: [value] "=a" (value));
	return value;
}

static __inline__ __attribute__((always_inline)) void writecr4(unsigned long Data)
{
	__asm__ __volatile__ (
			"mov %%eax, %%cr4"
			:
			: "a" (Data)
			);
}
#endif

/* C entry point of secondary cpus */
void asmlinkage secondary_cpu_init(unsigned int index)
{
	atomic_inc(&active_cpus);
#if CONFIG_SERIAL_CPU_INIT
	spin_lock(&start_cpu_lock);
#endif

#ifdef __SSE3__
	/*
	 * Seems that CR4 was cleared when AP start via lapic_start_cpu()
	 * Turn on CR4.OSFXSR and CR4.OSXMMEXCPT when SSE options enabled
	 */
	u32 cr4_val;
	cr4_val = readcr4();
	cr4_val |= (1 << 9 | 1 << 10);
	writecr4(cr4_val);
#endif
	cpu_initialize(index);
#if CONFIG_SERIAL_CPU_INIT
	spin_unlock(&start_cpu_lock);
#endif

	atomic_dec(&active_cpus);

	stop_this_cpu();
}

static void start_other_cpus(struct bus *cpu_bus, device_t bsp_cpu)
{
	device_t cpu;
	/* Loop through the cpus once getting them started */

	for(cpu = cpu_bus->children; cpu ; cpu = cpu->sibling) {
		if (cpu->path.type != DEVICE_PATH_APIC) {
			continue;
		}
	#if !CONFIG_SERIAL_CPU_INIT
		if(cpu==bsp_cpu) {
			continue;
		}
	#endif

		if (!cpu->enabled) {
			continue;
		}

		if (cpu->initialized) {
			continue;
		}

		if (!start_cpu(cpu)) {
			/* Record the error in cpu? */
			printk(BIOS_ERR, "CPU 0x%02x would not start!\n",
				cpu->path.apic.apic_id);
		}
#if CONFIG_SERIAL_CPU_INIT
		udelay(10);
#endif
	}

}

static void wait_other_cpus_stop(struct bus *cpu_bus)
{
	device_t cpu;
	int old_active_count, active_count;
	long loopcount = 0;
	int i;

	/* Now loop until the other cpus have finished initializing */
	old_active_count = 1;
	active_count = atomic_read(&active_cpus);
	while(active_count > 1) {
		if (active_count != old_active_count) {
			printk(BIOS_INFO, "Waiting for %d CPUS to stop\n",
				active_count - 1);
			old_active_count = active_count;
		}
		udelay(10);
		active_count = atomic_read(&active_cpus);
		loopcount++;
	}
	for(cpu = cpu_bus->children; cpu; cpu = cpu->sibling) {
		if (cpu->path.type != DEVICE_PATH_APIC) {
			continue;
		}
		if (cpu->path.apic.apic_id == SPEEDSTEP_APIC_MAGIC) {
			continue;
		}
		if (!cpu->initialized) {
			printk(BIOS_ERR, "CPU 0x%02x did not initialize!\n",
				cpu->path.apic.apic_id);
		}
	}
	printk(BIOS_DEBUG, "All AP CPUs stopped (%ld loops)\n", loopcount);
	for(i = 1; i <= last_cpu_index; i++)
		checkstack((void *)stacks[i] + CONFIG_STACK_SIZE, i);
}

#endif /* CONFIG_SMP */

void initialize_cpus(struct bus *cpu_bus)
{
	struct device_path cpu_path;
	struct cpu_info *info;

	/* Find the info struct for this cpu */
	info = cpu_info();

#if NEED_LAPIC == 1
	/* Ensure the local apic is enabled */
	enable_lapic();

	/* Get the device path of the boot cpu */
	cpu_path.type           = DEVICE_PATH_APIC;
	cpu_path.apic.apic_id = lapicid();
#else
	/* Get the device path of the boot cpu */
	cpu_path.type           = DEVICE_PATH_CPU;
	cpu_path.cpu.id       = 0;
#endif

	/* Find the device structure for the boot cpu */
	info->cpu = alloc_find_dev(cpu_bus, &cpu_path);

#if CONFIG_SMP && CONFIG_MAX_CPUS > 1
	// why here? In case some day we can start core1 in amd_sibling_init
	copy_secondary_start_to_lowest_1M();
#endif

#if CONFIG_HAVE_SMI_HANDLER
	smm_init();
#endif

#if CONFIG_SMP && CONFIG_MAX_CPUS > 1
	#if !CONFIG_SERIAL_CPU_INIT
	/* start all aps at first, so we can init ECC all together */
	start_other_cpus(cpu_bus, info->cpu);
	#endif
#endif

	/* Initialize the bootstrap processor */
	cpu_initialize(0);

#if CONFIG_SMP && CONFIG_MAX_CPUS > 1
	#if CONFIG_SERIAL_CPU_INIT
	start_other_cpus(cpu_bus, info->cpu);
	#endif

	/* Now wait the rest of the cpus stop*/
	wait_other_cpus_stop(cpu_bus);
#endif
}
