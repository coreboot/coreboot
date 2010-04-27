#include <cpu/x86/lapic.h>
#include <console/console.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>

void setup_lapic(void)
{
	/* this is so interrupts work. This is very limited scope --
	 * linux will do better later, we hope ...
	 */
	/* this is the first way we learned to do it. It fails on real SMP
	 * stuff. So we have to do things differently ...
	 * see the Intel mp1.4 spec, page A-3
	 */

#if NEED_LAPIC == 1
	/* Only Pentium Pro and later have those MSR stuff */
	msr_t msr;

	printk(BIOS_INFO, "Setting up local apic...");

	/* Enable the local apic */
	msr = rdmsr(LAPIC_BASE_MSR);
	msr.lo |= LAPIC_BASE_MSR_ENABLE;
	msr.lo &= ~LAPIC_BASE_MSR_ADDR_MASK;
	msr.lo |= LAPIC_DEFAULT_BASE;
	wrmsr(LAPIC_BASE_MSR, msr);

	/*
	 * Set Task Priority to 'accept all'.
	 */
	lapic_write_around(LAPIC_TASKPRI,
		lapic_read_around(LAPIC_TASKPRI) & ~LAPIC_TPRI_MASK);

	/* Put the local apic in virtual wire mode */
	lapic_write_around(LAPIC_SPIV,
		(lapic_read_around(LAPIC_SPIV) & ~(LAPIC_VECTOR_MASK))
		| LAPIC_SPIV_ENABLE);
	lapic_write_around(LAPIC_LVT0,
		(lapic_read_around(LAPIC_LVT0) &
			~(LAPIC_LVT_MASKED | LAPIC_LVT_LEVEL_TRIGGER |
				LAPIC_LVT_REMOTE_IRR | LAPIC_INPUT_POLARITY |
				LAPIC_SEND_PENDING |LAPIC_LVT_RESERVED_1 |
				LAPIC_DELIVERY_MODE_MASK))
		| (LAPIC_LVT_REMOTE_IRR |LAPIC_SEND_PENDING |
			LAPIC_DELIVERY_MODE_EXTINT)
		);
	lapic_write_around(LAPIC_LVT1,
		(lapic_read_around(LAPIC_LVT1) &
			~(LAPIC_LVT_MASKED | LAPIC_LVT_LEVEL_TRIGGER |
				LAPIC_LVT_REMOTE_IRR | LAPIC_INPUT_POLARITY |
				LAPIC_SEND_PENDING |LAPIC_LVT_RESERVED_1 |
				LAPIC_DELIVERY_MODE_MASK))
		| (LAPIC_LVT_REMOTE_IRR |LAPIC_SEND_PENDING |
			LAPIC_DELIVERY_MODE_NMI)
		);

	printk(BIOS_DEBUG, " apic_id: 0x%02lx ", lapicid());

#else /* !NEED_LLAPIC */
	/* Only Pentium Pro and later have those MSR stuff */
	msr_t msr;

	printk(BIOS_INFO, "Disabling local apic...");

	msr = rdmsr(LAPIC_BASE_MSR);
	msr.lo &= ~LAPIC_BASE_MSR_ENABLE;
	wrmsr(LAPIC_BASE_MSR, msr);
#endif /* !NEED_LAPIC */
	printk(BIOS_INFO, "done.\n");
	post_code(0x9b);
}
