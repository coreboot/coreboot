/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/x86/cr.h>
#include <cpu/x86/gdt.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/smi_deprecated.h>
#include <acpi/acpi.h>
#include <delay.h>
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
#include <smp/node.h>
#include <stdlib.h>
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
#if ENV_X86_64
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

static uint32_t wait_for_ipi_completion(const int timeout_ms)
{
	int loops = timeout_ms * 10;
	uint32_t send_status;

	/* wait for the ipi send to finish */
	printk(BIOS_SPEW, "Waiting for send to finish...\n");
	do {
		printk(BIOS_SPEW, "+");
		udelay(100);
		send_status = lapic_busy();
	} while (send_status && (--loops > 0));

	return send_status;
}

static int lapic_start_cpu(unsigned long apicid)
{
	const int timeout_100ms = 100;
	uint32_t send_status, accept_status;
	int j, maxlvt;

	/*
	 * Starting actual IPI sequence...
	 */

	printk(BIOS_SPEW, "Asserting INIT.\n");

	/*
	 * Turn INIT on target chip
	 */
	lapic_send_ipi(LAPIC_INT_LEVELTRIG | LAPIC_INT_ASSERT | LAPIC_DM_INIT, apicid);

	send_status = wait_for_ipi_completion(timeout_100ms);
	if (send_status) {
		printk(BIOS_ERR, "CPU %ld: First APIC write timed out. "
			"Disabling\n", apicid);
		// too bad.
		printk(BIOS_ERR, "ESR is 0x%x\n", lapic_read(LAPIC_ESR));
		if (lapic_read(LAPIC_ESR)) {
			printk(BIOS_ERR, "Try to reset ESR\n");
			lapic_write(LAPIC_ESR, 0);
			printk(BIOS_ERR, "ESR is 0x%x\n",
				lapic_read(LAPIC_ESR));
		}
		return 0;
	}
	mdelay(10);

	printk(BIOS_SPEW, "Deasserting INIT.\n");

	lapic_send_ipi(LAPIC_INT_LEVELTRIG | LAPIC_DM_INIT, apicid);

	send_status = wait_for_ipi_completion(timeout_100ms);
	if (send_status) {
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
		lapic_read(LAPIC_SPIV);
		lapic_write(LAPIC_ESR, 0);
		lapic_read(LAPIC_ESR);
		printk(BIOS_SPEW, "After apic_write.\n");

		/*
		 * STARTUP IPI
		 */

		lapic_send_ipi(LAPIC_DM_STARTUP | (AP_SIPI_VECTOR >> 12), apicid);

		/*
		 * Give the other CPU some time to accept the IPI.
		 */
		udelay(300);

		printk(BIOS_SPEW, "Startup point 1.\n");

		send_status = wait_for_ipi_completion(timeout_100ms);

		/*
		 * Give the other CPU some time to accept the IPI.
		 */
		udelay(200);
		/*
		 * Due to the Pentium erratum 3AP.
		 */
		if (maxlvt > 3) {
			lapic_read(LAPIC_SPIV);
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
		printk(BIOS_WARNING, "APIC delivery error (%x).\n",
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

static int start_cpu(struct device *cpu)
{
	struct cpu_info *info;
	uintptr_t stack_top;
	uintptr_t stack_base;
	unsigned long apicid;
	unsigned int index;
	unsigned long count;
	int result;

	spin_lock(&start_cpu_lock);

	/* Get the CPU's apicid */
	apicid = cpu->path.apic.apic_id;

	/* Get an index for the new processor */
	index = ++last_cpu_index;

	/* Find boundaries of the new processor's stack */
	stack_top = ALIGN_DOWN((uintptr_t)_estack, CONFIG_STACK_SIZE);
	stack_top -= (CONFIG_STACK_SIZE*index);
	stack_base = stack_top - CONFIG_STACK_SIZE;
	stack_top -= sizeof(struct cpu_info);
	printk(BIOS_SPEW, "CPU%d: stack_base %p, stack_top %p\n", index,
		(void *)stack_base, (void *)stack_top);
	stacks[index] = (void *)stack_base;

	/* Record the index and which CPU structure we are using */
	info = (struct cpu_info *)stack_top;
	info->index = index;
	info->cpu   = cpu;
	cpu_add_map_entry(info->index);

	/* Advertise the new stack and index to start_cpu */
	secondary_stack = stack_top;
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

/* C entry point of secondary cpus */
asmlinkage void secondary_cpu_init(unsigned int index)
{
	atomic_inc(&active_cpus);

	spin_lock(&start_cpu_lock);

#ifdef __SSE3__
	/*
	 * Seems that CR4 was cleared when AP start via lapic_start_cpu()
	 * Turn on CR4.OSFXSR and CR4.OSXMMEXCPT when SSE options enabled
	 */
	CRx_TYPE cr4_val;
	cr4_val = read_cr4();
	cr4_val |= (CR4_OSFXSR | CR4_OSXMMEXCPT);
	write_cr4(cr4_val);
#endif

	/* Ensure the local APIC is enabled */
	enable_lapic();
	setup_lapic_interrupts();

	cpu_initialize(index);

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

		if (!cpu->enabled)
			continue;

		if (cpu->initialized)
			continue;

		if (!start_cpu(cpu))
			/* Record the error in cpu? */
			printk(BIOS_ERR, "CPU 0x%02x would not start!\n",
				cpu->path.apic.apic_id);

		udelay(10);
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

	/* Ensure the local APIC is enabled */
	if (is_smp_boot()) {
		enable_lapic();
		setup_lapic_interrupts();
	} else {
		disable_lapic();
	}

	/* Get the device path of the boot CPU */
	cpu_path.type = DEVICE_PATH_APIC;
	cpu_path.apic.apic_id = lapicid();

	/* Find the device structure for the boot CPU */
	info->cpu = alloc_find_dev(cpu_bus, &cpu_path);
	cpu_add_map_entry(info->index);

	// why here? In case some day we can start core1 in amd_sibling_init
	if (is_smp_boot())
		copy_secondary_start_to_lowest_1M();

	if (CONFIG(SMM_LEGACY_ASEG))
		smm_init();

	/* Initialize the bootstrap processor */
	cpu_initialize(0);

	if (is_smp_boot())
		start_other_cpus(cpu_bus, info->cpu);

	/* Now wait the rest of the cpus stop*/
	if (is_smp_boot())
		wait_other_cpus_stop(cpu_bus);

	if (CONFIG(SMM_LEGACY_ASEG))
		smm_init_completion();

	if (is_smp_boot())
		recover_lowest_1M();
}
