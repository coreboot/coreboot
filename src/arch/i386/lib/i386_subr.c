#include <cpu/p5/io.h>
#include <cpu/p5/macros.h>
#include <cpu/p6/msr.h>
#include <cpu/p6/apic.h>
#include <cpu/p6/mtrr.h>
#include <printk.h>
#include <pci.h>
#include <subr.h>
#include <string.h>
#include <arch/i386_subr.h>

void cache_on(unsigned long totalram)
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
	setup_mtrrs(totalram);
#endif

	post_code(0x6A);
	printk_info("done.\n");
}

void interrupts_on()
{
	/* this is so interrupts work. This is very limited scope -- 
	 * linux will do better later, we hope ...
	 */
	/* this is the first way we learned to do it. It fails on real SMP 
	 * stuff. So we have to do things differently ... 
	 * see the Intel mp1.4 spec, page A-3
	 */

#if defined(SMP)
	/* Only Pentium Pro and later have those MSR stuff */
	unsigned long low, high;

	printk_info("Setting up local apic...");

	/* Enable the local apic */
	rdmsr(APIC_BASE_MSR, low, high);
	low |= APIC_BASE_MSR_ENABLE;
	low &= ~APIC_BASE_MSR_ADDR_MASK;
	low |= APIC_DEFAULT_BASE;
	wrmsr(APIC_BASE_MSR, low, high);


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
#else /* SMP */
#ifdef i686
	/* Only Pentium Pro and later have those MSR stuff */
	unsigned long low, high;

	printk_info("Disabling local apic...");

	rdmsr(APIC_BASE_MSR, low, high);
	low &= ~APIC_BASE_MSR_ENABLE;
	wrmsr(APIC_BASE_MSR, low, high);
#endif /* i686 */
#endif /* SMP */
	printk_info("done.\n");
	post_code(0x9b);
}


