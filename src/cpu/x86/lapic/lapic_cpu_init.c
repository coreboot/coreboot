/*
	2005.12 yhlu add coreboot_ram cross the vga font buffer handling
	2005.12 yhlu add CONFIG_RAMBASE above 1M support for SMP
	2008.05 stepan add support for going back to sipi wait state
*/

#include <cpu/x86/lapic.h>
#include <delay.h>
#include <string.h>
#include <console/console.h>
#include <arch/hlt.h>
#include <device/device.h>
#include <device/path.h>
#include <smp/atomic.h>
#include <smp/spinlock.h>
#include <cpu/cpu.h>
#include <cpu/intel/speedstep.h>

#if CONFIG_SMP == 1

/* We do not want this struct to be visible outside this file.
 * The external interface is via functions.
 */

struct apcore {
	u32 stack[4096 - sizeof(struct cpu_info)];
	struct cpu_info info;
};

#define TOS(x) (&apcores[(x)].stack[ARRAY_SIZE(apcores[x].stack)-1])

struct apcore apcores[CONFIG_MAX_CPUS];

/* This is a lot more paranoid now, since Linux can NOT handle
 * being told there is a CPU when none exists. So any errors
 * will return 0, meaning no CPU.
 *
 * We actually handling that case by noting which cpus startup
 * and not telling anyone about the ones that dont.
 */
static unsigned long get_valid_start_eip(unsigned long orig_start_eip)
{
	return (unsigned long)orig_start_eip & 0xffff; // 16 bit to avoid 0xa0000
}

#if CONFIG_HAVE_ACPI_RESUME == 1
char *lowmem_backup;
char *lowmem_backup_ptr;
int  lowmem_backup_size;
#endif

extern char _secondary_start[];

static void copy_secondary_start_to_1m_below(void)
{
	extern char _secondary_start_end[];
	unsigned long code_size;
	unsigned long start_eip;

	/* _secondary_start need to be masked 20 above bit, because 16 bit code in secondary.S
	   Also We need to copy the _secondary_start to the below 1M region
	*/
	start_eip = get_valid_start_eip((unsigned long)_secondary_start);
	code_size = (unsigned long)_secondary_start_end - (unsigned long)_secondary_start;

#if CONFIG_HAVE_ACPI_RESUME == 1
	/* need to save it for RAM resume */
	lowmem_backup_size = code_size;
	lowmem_backup = malloc(code_size);
	lowmem_backup_ptr = (char *)start_eip;

	if (lowmem_backup == NULL)
		die("Out of backup memory\n");

	memcpy(lowmem_backup, lowmem_backup_ptr, lowmem_backup_size);
#endif
	/* copy the _secondary_start to the ram below 1M*/
	memcpy((unsigned char *)start_eip, (unsigned char *)_secondary_start, code_size);

	printk(BIOS_DEBUG, "start_eip=0x%08lx, offset=0x%08lx, code_size=0x%08lx\n", start_eip, ((unsigned long)_secondary_start - start_eip), code_size);
}

static int lapic_start_cpu(unsigned long apicid)
{
	int timeout;
	unsigned long send_status, accept_status, start_eip;
	int j, num_starts, maxlvt;

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
		printk(BIOS_ERR, "CPU %ld: First apic write timed out. Disabling\n",
			 apicid);
		// too bad.
		printk(BIOS_ERR, "ESR is 0x%lx\n", lapic_read(LAPIC_ESR));
		if (lapic_read(LAPIC_ESR)) {
			printk(BIOS_ERR, "Try to reset ESR\n");
			lapic_write_around(LAPIC_ESR, 0);
			printk(BIOS_ERR, "ESR is 0x%lx\n", lapic_read(LAPIC_ESR));
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
		printk(BIOS_ERR, "CPU %ld: Second apic write timed out. Disabling\n",
			 apicid);
		// too bad.
		return 0;
	}

	start_eip = get_valid_start_eip((unsigned long)_secondary_start);

#if !CONFIG_CPU_AMD_MODEL_10XXX
	num_starts = 2;
#else
	num_starts = 1;
#endif

	/*
	 * Run STARTUP IPI loop.
	 */
	printk(BIOS_SPEW, "#startup loops: %d.\n", num_starts);

	maxlvt = 4;

	for (j = 1; j <= num_starts; j++) {
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
					| (start_eip >> 12));

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
		printk(BIOS_WARNING, "APIC delivery error (%lx).\n", accept_status);
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
 * am starting allows me to veryify it has started before
 * start_cpu returns.
 */

/* N.B. if we move to serial smp init we don't need this spin lock.
 * Consider for the future.
 */

static spinlock_t start_cpu_lock = SPIN_LOCK_UNLOCKED;
static unsigned int last_cpu_index = 0;
volatile unsigned long secondary_stack, secondary_cpu_info;

int start_cpu(device_t cpu)
{
	struct cpu_info *info;
	unsigned long stack_end;
	unsigned long apicid;
	unsigned long count;
	int result;
	unsigned int index = last_cpu_index + 1;

	spin_lock(&start_cpu_lock);

	/* Get the cpu's apicid */
	apicid = cpu->path.apic.apic_id;

	/* Get an index for the new processor */
	if (index >= CONFIG_MAX_CPUS){
		printk(BIOS_ERR, "CONFIG_MAX_CPUS(%d) too small!\n", CONFIG_MAX_CPUS);
		spin_unlock(&start_cpu_lock);
		return 0;
	}

	last_cpu_index = index;

	/* Find end of the new processors stack */
	stack_end = (unsigned long) TOS(index);

	/* Record the index and which cpu structure we are using */
	info = &apcores[index].info;
	info->index = index;
	info->cpu   = cpu;

	/* Advertise the new stack to start_cpu */
	secondary_stack = stack_end;
	secondary_cpu_info = (unsigned long) info;
	printk(BIOS_SPEW, "start_cpu CPU %d secondary_stack %#lx info %p\n",
		index, secondary_stack, info);
	/* Until the cpu starts up report the cpu is not enabled */
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

#if CONFIG_AP_IN_SIPI_WAIT == 1

/**
 * Sending INIT IPI to self is equivalent of asserting #INIT with a bit of delay.
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
	lapic_write_around(LAPIC_ICR, LAPIC_INT_LEVELTRIG | LAPIC_INT_ASSERT | LAPIC_DM_INIT);

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
void secondary_cpu_init(struct cpu_info *info)
{
	atomic_inc(&active_cpus);
#if CONFIG_SERIAL_CPU_INIT == 1
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
	cpu_initialize(info);
#if CONFIG_SERIAL_CPU_INIT == 1
	spin_unlock(&start_cpu_lock);
#endif
	cpu_work(info);
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
	#if CONFIG_SERIAL_CPU_INIT == 0
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
#if CONFIG_SERIAL_CPU_INIT == 1
		udelay(10);
#endif
	}

}

static void wait_other_cpus_stop(struct bus *cpu_bus)
{
	device_t cpu;
	int old_active_count, active_count;
	long loopcount = 0;

	/* Now loop until the other cpus have finished initializing */
	old_active_count = 1;
	active_count = atomic_read(&active_cpus);
	while(active_count > 1) {
		if (active_count != old_active_count) {
			printk(BIOS_INFO, "Waiting for %d CPUS to stop\n", active_count - 1);
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

#if CONFIG_SMP == 1
	copy_secondary_start_to_1m_below(); // why here? In case some day we can start core1 in amd_sibling_init
#endif

#if CONFIG_HAVE_SMI_HANDLER
	smm_init();
#endif

	cpus_ready_for_init();

#if CONFIG_SMP == 1
	#if CONFIG_SERIAL_CPU_INIT == 0
	/* start all aps at first, so we can init ECC all together */
	start_other_cpus(cpu_bus, info->cpu);
	#endif
#endif

	/* Initialize the bootstrap processor */
	cpu_initialize(info);

#if CONFIG_SMP == 1
	#if CONFIG_SERIAL_CPU_INIT == 1
	start_other_cpus(cpu_bus, info->cpu);
	#endif

	/* Now wait the rest of the cpus stop*/
	wait_other_cpus_stop(cpu_bus);
#endif
}

#if CONFIG_SMP == 1
/* work primitives */

/* start_work is only intended to be called by the bsp. */
/* A built in assumption of this code is that you know what 
 * you're doing. This is firmware, not pthreads. You should 
 * not call this function for a core if:
 * - the core does not exist
 * - the core is not initialized
 * - the core is busy
 * Any of these return a -1, else the core is started and
 * 0 is returned. 
 */
static int start_work(unsigned int core, workfunc f, u32 a, u32 b, u32 c)
{
	struct cpu_info *info;
	volatile workfunc *ptr;
	volatile u32 *params;

	if (core >= CONFIG_MAX_CPUS){
		printk(BIOS_EMERG, "start_work: invalid core %d\n", core);
		return -1;
	}
	info = &apcores[core].info;
	if (! info->cpu->initialized){
		printk(BIOS_EMERG, "start_work: core not initialized %d\n", core);
		return -1;
	}
	ptr = &info->work;
	if (*ptr){
		printk(BIOS_EMERG, "start_work: core is busy %d\n", core);
		return -1;
	}
	params = (u32 *)&info->params;
	params[0] = a;
	params[1] = b;
	params[2] = c;

	printk(BIOS_INFO, "BSP starts work on core %d\n", core);

	/* This interface depends on arguments being written, 
	 * then the function pointer being written. 
	 */
	barrier();
	*ptr = f;
	return 0;
}

/* wait for the work to finish and return the result. Wait at
 * most maxusec microseconds. 
 */
static int wait_work(unsigned int core, u32 *retval, unsigned int maxusec)
{
	unsigned int usec;
	struct cpu_info *info;
	volatile workfunc *ptr;
	u32 result;

	if (core >= CONFIG_MAX_CPUS){
		printk(BIOS_EMERG, "start_work: invalid core %d\n", core);
		return -1;
	}
	info = &apcores[core].info;
	if (! info->cpu->initialized){
		printk(BIOS_EMERG, "start_work: core not initialized %d\n", core);
		return -1;
	}
	ptr = &info->work;
	for(usec = 0; usec < maxusec && *ptr; usec++)
		udelay(1);

	/* N.B. since only the BSP starts cores, there is not 
	 * problem checking the pointer since access to it is 
	 * serialized by the single-threaded BSP code. 
	 */
	if (*ptr){
		printk(BIOS_INFO, "core %d still running after %d microseconds\n",
			core, usec);
		return -1;
	}
	result = info->result;
	printk(BIOS_INFO, "Result is %#x\n", result);
	if (retval)
		*retval = result;
	return 0;
}

/* start the work and wait for it to finish */
int run_work(unsigned int core, workfunc f, u32 a, u32 b, u32 c, u32 *retval,
	unsigned int timeout)
{
	if (start_work(core, f, a, b, c))
		return -1;
	return wait_work(core, retval, timeout);
}
#endif /* CONFIG_SMP == 1 */
