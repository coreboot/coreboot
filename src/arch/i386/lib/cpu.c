#include <console/console.h>
#include <cpu/cpu.h>
#include <mem.h>
#include <arch/io.h>
#include <string.h>
#include <cpu/cpufixup.h>
#include <smp/start_stop.h>
#include <cpu/cpufixup.h>
#include <cpu/p6/mtrr.h>
#include <cpu/p6/msr.h>
#include <cpu/p6/apic.h>
#include <cpu/p5/cpuid.h>
#if 0
#include <cpu/l2_cache.h>
#endif

#if CONFIG_SMP || CONFIG_IOAPIC
#define APIC 1
#endif


static void cache_on(struct mem_range *mem)
{
	post_code(0x60);
	printk_info("Enabling cache...");


	/* we need an #ifdef i586 here at some point ... */
	__asm__ __volatile__("mov %cr0, %eax\n\t"
			     "and $0x9fffffff,%eax\n\t"
			     "mov %eax, %cr0\n\t");
	/* turns out cache isn't really on until you set MTRR registers on 
	 * 686 and later. 
	 * NOTHING FANCY. Linux does a much better job anyway. 
	 * so absolute minimum needed to get it going. 
	 */
	/* OK, linux it turns out does nothing. We have to do it ... */
#if defined(i686) 
	// totalram here is in linux sizing, i.e. units of KB. 
	// set_mtrr is responsible for getting it into the right units!
	setup_mtrrs(mem);
#endif

	post_code(0x6A);
	printk_info("done.\n");
}

static void interrupts_on()
{
	/* this is so interrupts work. This is very limited scope -- 
	 * linux will do better later, we hope ...
	 */
	/* this is the first way we learned to do it. It fails on real SMP 
	 * stuff. So we have to do things differently ... 
	 * see the Intel mp1.4 spec, page A-3
	 */

#if defined(APIC)
	/* Only Pentium Pro and later have those MSR stuff */
	unsigned long low, high;

	printk_info("Setting up local apic...");

	/* Enable the local apic */
	rdmsr(APIC_BASE_MSR, low, high);
	low |= APIC_BASE_MSR_ENABLE;
	low &= ~APIC_BASE_MSR_ADDR_MASK;
	low |= APIC_DEFAULT_BASE;
	wrmsr(APIC_BASE_MSR, low, high);

	/*
	 * Set Task Priority to 'accept all'.
	 */
	apic_write_around(APIC_TASKPRI,
		apic_read_around(APIC_TASKPRI) & ~APIC_TPRI_MASK);

	/* Put the local apic in virtual wire mode */
	apic_write_around(APIC_SPIV, 
		(apic_read_around(APIC_SPIV) & ~(APIC_VECTOR_MASK))
		| APIC_SPIV_ENABLE);
	apic_write_around(APIC_LVT0, 
		(apic_read_around(APIC_LVT0) & 
			~(APIC_LVT_MASKED | APIC_LVT_LEVEL_TRIGGER | 
				APIC_LVT_REMOTE_IRR | APIC_INPUT_POLARITY | 
				APIC_SEND_PENDING |APIC_LVT_RESERVED_1 |
				APIC_DELIVERY_MODE_MASK))
		| (APIC_LVT_REMOTE_IRR |APIC_SEND_PENDING | 
			APIC_DELIVERY_MODE_EXTINT)
		);
	apic_write_around(APIC_LVT1, 
		(apic_read_around(APIC_LVT1) & 
			~(APIC_LVT_MASKED | APIC_LVT_LEVEL_TRIGGER | 
				APIC_LVT_REMOTE_IRR | APIC_INPUT_POLARITY | 
				APIC_SEND_PENDING |APIC_LVT_RESERVED_1 |
				APIC_DELIVERY_MODE_MASK))
		| (APIC_LVT_REMOTE_IRR |APIC_SEND_PENDING | 
			APIC_DELIVERY_MODE_NMI)
		);

	printk_debug(" apic_id: %d ",
		apic_read(APIC_ID));

#else /* APIC */
#ifdef i686
	/* Only Pentium Pro and later have those MSR stuff */
	unsigned long low, high;

	printk_info("Disabling local apic...");

	rdmsr(APIC_BASE_MSR, low, high);
	low &= ~APIC_BASE_MSR_ENABLE;
	wrmsr(APIC_BASE_MSR, low, high);
#endif /* i686 */
#endif /* APIC */
	printk_info("done.\n");
	post_code(0x9b);
}

unsigned long cpu_initialize(struct mem_range *mem)
{
	/* Because we busy wait at the printk spinlock.
	 * It is important to keep the number of printed messages
	 * from secondary cpus to a minimum, when debugging is
	 * disabled.
	 */
	unsigned long processor_id = this_processors_id();
	printk_notice("Initializing CPU #%d\n", processor_id);
	
	/* some cpus need a fixup done. This is the hook for doing that. */
	cpufixup(mem);

	/* Turn on caching if we haven't already */
	cache_on(mem);

	display_cpuid();
	mtrr_check();

#if 0
	/* now that everything is really up, enable the l2 cache if desired. 
	 * The enable can wait until this point, because linuxbios and it's
	 * data areas are tiny, easily fitting into the L1 cache. 
	 */
	configure_l2_cache();
#endif
	interrupts_on();
	printk_info("CPU #%d Initialized\n", processor_id);
	return processor_id;
}

