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

#if CONFIG_SMP
/* This is a lot more paranoid now, since Linux can NOT handle
 * being told there is a CPU when none exists. So any errors
 * will return 0, meaning no CPU.
 *
 * We actually handling that case by noting which cpus startup
 * and not telling anyone about the ones that dont.
 */

/* Start-UP IPI vector must be 4kB aligned and below 1MB. */
#define AP_SIPI_VECTOR 0x1000

#if CONFIG_HAVE_ACPI_RESUME
char *lowmem_backup;
char *lowmem_backup_ptr;
int  lowmem_backup_size;
#endif

extern char _secondary_start[];

static void copy_secondary_start_to_1m_below(void)
{
	extern char _secondary_start_end[];
	unsigned long code_size;

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

	printk(BIOS_DEBUG, "start_eip=0x%08lx, code_size=0x%08lx\n", (long unsigned int)AP_SIPI_VECTOR, code_size);
}

static struct bus *current_cpu_bus;

static int lapic_start_cpus(struct bus *cpu_bus)
{
	int timeout;
	unsigned long send_status, accept_status;
	int maxlvt;

	/*
	 * Starting actual IPI sequence...
	 */

	current_cpu_bus = cpu_bus;

	printk(BIOS_SPEW, "Asserting INIT.\n");

	/* Send INIT SIPI to target chip */
	lapic_write_around(LAPIC_ICR2, 0);
	lapic_write_around(LAPIC_ICR, LAPIC_INT_ASSERT
				| LAPIC_DM_INIT | LAPIC_DEST_ALLBUT);

	printk(BIOS_DEBUG, "Waiting for send to finish...\n");
	timeout = 0;
	do {
		printk(BIOS_SPEW, "+");
		udelay(100);
		send_status = lapic_read(LAPIC_ICR) & LAPIC_ICR_BUSY;
	} while (send_status && (timeout++ < 1000));
	if (timeout >= 1000) {
		printk(BIOS_DEBUG, "First apic write timed out. Disabling\n");
		// too bad.
		printk(BIOS_DEBUG, "ESR is 0x%lx\n", lapic_read(LAPIC_ESR));
		if (lapic_read(LAPIC_ESR)) {
			printk(BIOS_DEBUG, "Try to reset ESR\n");
			lapic_write_around(LAPIC_ESR, 0);
			printk(BIOS_DEBUG, "ESR is 0x%lx\n", lapic_read(LAPIC_ESR));
		}
		return 0;
	}

	maxlvt = 4;

	printk(BIOS_SPEW, "Sending STARTUP.\n");
	lapic_read_around(LAPIC_SPIV);
	lapic_write(LAPIC_ESR, 0);
	lapic_read(LAPIC_ESR);
	printk(BIOS_SPEW, "After apic_write.\n");

	/*
	 * STARTUP IPI
	 */

	/* Target chip */
	lapic_write_around(LAPIC_ICR2, 0);

	/* Boot on the stack */
	/* Kick the second */
	lapic_write_around(LAPIC_ICR, LAPIC_INT_ASSERT | LAPIC_DM_STARTUP | LAPIC_DEST_ALLBUT
			   | ((AP_SIPI_VECTOR >> 12) & 0xff));

	/*
	 * Give the other CPU some time to accept the IPI.
	 */
	udelay(300);

	printk(BIOS_DEBUG, "Startup point 1.\n");

	printk(BIOS_DEBUG, "Waiting for send to finish...\n");
	timeout = 0;
	do {
		printk(BIOS_DEBUG, "+");
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

	printk(BIOS_DEBUG, "After Startup.\n");
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

volatile unsigned long secondary_stack;
extern unsigned char _estack[];

static void stop_all_ap_cpus(void)
{
	unsigned long send_status;
	int timeout;
	/* send an LAPIC INIT to all but myself */
	lapic_write_around(LAPIC_ICR2, 0);
	lapic_write_around(LAPIC_ICR, LAPIC_INT_ASSERT | LAPIC_DM_INIT | LAPIC_DEST_ALLBUT);

	/* wait for the ipi send to finish */
	printk(BIOS_SPEW, "Waiting for send to finish...\n");
	timeout = 0;
	do {
		printk(BIOS_SPEW, "+");
		udelay(100);
		send_status = lapic_read(LAPIC_ICR) & LAPIC_ICR_BUSY;
	} while (send_status && (timeout++ < 1000));
	if (timeout >= 1000) {
		printk(BIOS_ERR, "timed out\n");
	}
	mdelay(10);
}

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
void secondary_cpu_init(int index)
{
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
	atomic_inc(&active_cpus);
	cpu_initialize(current_cpu_bus, index);
	atomic_dec(&active_cpus);
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
	stop_all_ap_cpus();
	printk(BIOS_DEBUG, "All AP CPUs stopped (%ld loops)\n", loopcount);
}

#endif /* CONFIG_SMP */

void initialize_cpus(struct bus *cpu_bus)
{
	struct device_path cpu_path;

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

#if CONFIG_SMP
	copy_secondary_start_to_1m_below(); // why here? In case some day we can start core1 in amd_sibling_init
#endif

#if CONFIG_HAVE_SMI_HANDLER
	smm_init();
#endif

	cpus_ready_for_init();

	/* Initialize the bootstrap processor */
	cpu_initialize(cpu_bus, 0);

#if CONFIG_SMP
	lapic_start_cpus(cpu_bus);
	/* Now wait the rest of the cpus stop*/
	wait_other_cpus_stop(cpu_bus);
#endif
}

