/*
	2005.12 yhlu add coreboot_ram cross the vga font buffer handling
	2005.12 yhlu add _RAMBASE above 1M support for SMP
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

#if CONFIG_SMP == 1

#if _RAMBASE >= 0x100000
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
#endif

static void copy_secondary_start_to_1m_below(void) 
{
#if _RAMBASE >= 0x100000
        extern char _secondary_start[];
        extern char _secondary_start_end[];
        unsigned long code_size;
        unsigned long start_eip;

        /* _secondary_start need to be masked 20 above bit, because 16 bit code in secondary.S
                Also We need to copy the _secondary_start to the below 1M region
        */
        start_eip = get_valid_start_eip((unsigned long)_secondary_start);
        code_size = (unsigned long)_secondary_start_end - (unsigned long)_secondary_start;

        /* copy the _secondary_start to the ram below 1M*/
        memcpy((unsigned char *)start_eip, (unsigned char *)_secondary_start, code_size);

        printk_debug("start_eip=0x%08lx, offset=0x%08lx, code_size=0x%08lx\n", start_eip, ((unsigned long)_secondary_start - start_eip), code_size);
#endif
}

static int lapic_start_cpu(unsigned long apicid)
{
	int timeout;
	unsigned long send_status, accept_status, start_eip;
	int j, num_starts, maxlvt;
	extern char _secondary_start[];
		
	/*
	 * Starting actual IPI sequence...
	 */

	printk_spew("Asserting INIT.\n");

	/*
	 * Turn INIT on target chip
	 */
	lapic_write_around(LAPIC_ICR2, SET_LAPIC_DEST_FIELD(apicid));

	/*
	 * Send IPI
	 */
	
	lapic_write_around(LAPIC_ICR, LAPIC_INT_LEVELTRIG | LAPIC_INT_ASSERT
				| LAPIC_DM_INIT);

	printk_spew("Waiting for send to finish...\n");
	timeout = 0;
	do {
		printk_spew("+");
		udelay(100);
		send_status = lapic_read(LAPIC_ICR) & LAPIC_ICR_BUSY;
	} while (send_status && (timeout++ < 1000));
	if (timeout >= 1000) {
		printk_err("CPU %ld: First apic write timed out. Disabling\n",
			 apicid);
		// too bad. 
		printk_err("ESR is 0x%lx\n", lapic_read(LAPIC_ESR));
		if (lapic_read(LAPIC_ESR)) {
			printk_err("Try to reset ESR\n");
			lapic_write_around(LAPIC_ESR, 0);
			printk_err("ESR is 0x%lx\n", lapic_read(LAPIC_ESR));
		}
		return 0;
	}
	mdelay(10);

	printk_spew("Deasserting INIT.\n");

	/* Target chip */
	lapic_write_around(LAPIC_ICR2, SET_LAPIC_DEST_FIELD(apicid));

	/* Send IPI */
	lapic_write_around(LAPIC_ICR, LAPIC_INT_LEVELTRIG | LAPIC_DM_INIT);
	
	printk_spew("Waiting for send to finish...\n");
	timeout = 0;
	do {
		printk_spew("+");
		udelay(100);
		send_status = lapic_read(LAPIC_ICR) & LAPIC_ICR_BUSY;
	} while (send_status && (timeout++ < 1000));
	if (timeout >= 1000) {
		printk_err("CPU %ld: Second apic write timed out. Disabling\n",
			 apicid);
		// too bad. 
		return 0;
	}

#if _RAMBASE >= 0x100000
	start_eip = get_valid_start_eip((unsigned long)_secondary_start);
#else
	start_eip = (unsigned long)_secondary_start;
#endif

	num_starts = 2;

	/*
	 * Run STARTUP IPI loop.
	 */
	printk_spew("#startup loops: %d.\n", num_starts);

	maxlvt = 4;

	for (j = 1; j <= num_starts; j++) {
		printk_spew("Sending STARTUP #%d to %lu.\n", j, apicid);
		lapic_read_around(LAPIC_SPIV);
		lapic_write(LAPIC_ESR, 0);
		lapic_read(LAPIC_ESR);
		printk_spew("After apic_write.\n");

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

		printk_spew("Startup point 1.\n");

		printk_spew("Waiting for send to finish...\n");
		timeout = 0;
		do {
			printk_spew("+");
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
	printk_spew("After Startup.\n");
	if (send_status)
		printk_warning("APIC never delivered???\n");
	if (accept_status)
		printk_warning("APIC delivery error (%lx).\n", accept_status);
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

static spinlock_t start_cpu_lock = SPIN_LOCK_UNLOCKED;
static unsigned last_cpu_index = 0;
volatile unsigned long secondary_stack;

int start_cpu(device_t cpu)
{
	extern unsigned char _estack[];
	struct cpu_info *info;
	unsigned long stack_end;
	unsigned long apicid;
	unsigned long index;
	unsigned long count;
	int result;

	spin_lock(&start_cpu_lock);

	/* Get the cpu's apicid */
	apicid = cpu->path.apic.apic_id;

	/* Get an index for the new processor */
	index = ++last_cpu_index;
	
	/* Find end of the new processors stack */
#if (CONFIG_LB_MEM_TOPK>1024) && (_RAMBASE < 0x100000) && ((CONFIG_CONSOLE_VGA==1) || (CONFIG_PCI_ROM_RUN == 1))
	if(index<1) { // only keep bsp on low 
		stack_end = ((unsigned long)_estack) - (STACK_SIZE*index) - sizeof(struct cpu_info);
	} else {
		// for all APs, let use stack after pgtbl, 20480 is the pgtbl size for every cpu
		stack_end = 0x100000+(20480 + STACK_SIZE)*CONFIG_MAX_CPUS - (STACK_SIZE*index);
#if (0x100000+(20480 + STACK_SIZE)*CONFIG_MAX_CPU) > (CONFIG_LB_MEM_TOPK<<10)
		#warning "We may need to increase CONFIG_LB_MEM_TOPK, it need to be more than (0x100000+(20480 + STACK_SIZE)*CONFIG_MAX_CPU)\n"
#endif
		if(stack_end > (CONFIG_LB_MEM_TOPK<<10)) {
			printk_debug("start_cpu: Please increase the CONFIG_LB_MEM_TOPK more than %luK\n", stack_end>>10);
			die("Can not go on\n");
		}
		stack_end -= sizeof(struct cpu_info);
	}
#else
	stack_end = ((unsigned long)_estack) - (STACK_SIZE*index) - sizeof(struct cpu_info);
#endif

	
	/* Record the index and which cpu structure we are using */
	info = (struct cpu_info *)stack_end;
	info->index = index;
	info->cpu   = cpu;

	/* Advertise the new stack to start_cpu */
	secondary_stack = stack_end;

	/* Until the cpu starts up report the cpu is not enabled */
	cpu->enabled = 0;
	cpu->initialized = 0;

	/* Start the cpu */
	result = lapic_start_cpu(apicid);

	if (result) {
		result = 0;
		/* Wait 1s or until the new the new cpu calls in */
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

	printk_debug("CPU %ld going down...\n", id);

	/* send an LAPIC INIT to myself */
	lapic_write_around(LAPIC_ICR2, SET_LAPIC_DEST_FIELD(id));
	lapic_write_around(LAPIC_ICR, LAPIC_INT_LEVELTRIG | LAPIC_INT_ASSERT | LAPIC_DM_INIT);

	/* wait for the ipi send to finish */
#if 0
	// When these two printk_spew calls are not removed, the
	// machine will hang when log level is SPEW. Why?
	printk_spew("Waiting for send to finish...\n");
#endif
	timeout = 0;
	do {
#if 0
		printk_spew("+");
#endif
		udelay(100);
		send_status = lapic_read(LAPIC_ICR) & LAPIC_ICR_BUSY;
	} while (send_status && (timeout++ < 1000));
	if (timeout >= 1000) {
		printk_err("timed out\n");
	}
	mdelay(10);

	printk_spew("Deasserting INIT.\n");
	/* Deassert the LAPIC INIT */
	lapic_write_around(LAPIC_ICR2, SET_LAPIC_DEST_FIELD(id));	
	lapic_write_around(LAPIC_ICR, LAPIC_INT_LEVELTRIG | LAPIC_DM_INIT);

	printk_spew("Waiting for send to finish...\n");
	timeout = 0;
	do {
		printk_spew("+");
		udelay(100);
		send_status = lapic_read(LAPIC_ICR) & LAPIC_ICR_BUSY;
	} while (send_status && (timeout++ < 1000));
	if (timeout >= 1000) {
		printk_err("timed out\n");
	}

	while(1) {
		hlt();
	}
}
#endif

/* C entry point of secondary cpus */
void secondary_cpu_init(void)
{
	atomic_inc(&active_cpus);
#if SERIAL_CPU_INIT == 1
  #if CONFIG_MAX_CPUS>2
	spin_lock(&start_cpu_lock);
  #endif
#endif
	cpu_initialize();
#if SERIAL_CPU_INIT == 1
  #if CONFIG_MAX_CPUS>2
	spin_unlock(&start_cpu_lock);
  #endif
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
	#if SERIAL_CPU_INIT == 0
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
			printk_err("CPU 0x%02x would not start!\n",
				cpu->path.apic.apic_id);
		}
#if SERIAL_CPU_INIT == 1
  #if CONFIG_MAX_CPUS>2
		udelay(10);
  #endif
#endif
	}

}

static void wait_other_cpus_stop(struct bus *cpu_bus)
{
	device_t cpu;
	int old_active_count, active_count;
	/* Now loop until the other cpus have finished initializing */
	old_active_count = 1;
	active_count = atomic_read(&active_cpus);
	while(active_count > 1) {
		if (active_count != old_active_count) {
			printk_info("Waiting for %d CPUS to stop\n", active_count - 1);
			old_active_count = active_count;
		}
		udelay(10);
		active_count = atomic_read(&active_cpus);
	}
	for(cpu = cpu_bus->children; cpu; cpu = cpu->sibling) {
		if (cpu->path.type != DEVICE_PATH_APIC) {
			continue;
		}
		if (!cpu->initialized) {
			printk_err("CPU 0x%02x did not initialize!\n", 
				cpu->path.apic.apic_id);
		}
	}
	printk_debug("All AP CPUs stopped\n");
}

#else /* CONFIG_SMP */
#define initialize_other_cpus(root) do {} while(0)
#endif /* CONFIG_SMP */

#if WAIT_BEFORE_CPUS_INIT==0
	#define cpus_ready_for_init() do {} while(0)
#else
	void cpus_ready_for_init(void);
#endif

#if HAVE_SMI_HANDLER
void smm_init(void);
#endif

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

#if HAVE_SMI_HANDLER
	smm_init();
#endif

        cpus_ready_for_init(); 

#if CONFIG_SMP == 1
	#if SERIAL_CPU_INIT == 0
	/* start all aps at first, so we can init ECC all together */
        start_other_cpus(cpu_bus, info->cpu);
	#endif
#endif

        /* Initialize the bootstrap processor */
        cpu_initialize();

#if CONFIG_SMP == 1
        #if SERIAL_CPU_INIT == 1
        start_other_cpus(cpu_bus, info->cpu);
        #endif

	/* Now wait the rest of the cpus stop*/
	wait_other_cpus_stop(cpu_bus);
#endif
}

