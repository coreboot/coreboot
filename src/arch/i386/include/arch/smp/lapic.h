#ifndef ARCH_SMP_LAPIC_H
#define ARCH_SMP_LAPIC_H

#include <cpu/p6/msr.h>
#include <cpu/p6/apic.h>
#include <arch/hlt.h>

static void enable_lapic(void)
{

	msr_t msr;
	msr = rdmsr(0x1b);
	msr.hi &= 0xffffff00;
	msr.lo &= 0x000007ff;
	msr.lo |= APIC_DEFAULT_BASE | (1 << 11);
	wrmsr(0x1b, msr);
}

static void disable_lapic(void)
{
	msr_t msr;
	msr = rdmsr(0x1b);
	msr.lo &= ~ (1 << 11);
	wrmsr(0x1b, msr);
}

static inline unsigned long lapicid(void)
{
	return apic_read(APIC_ID) >> 24;
}

static void stop_this_cpu(void)
{
	unsigned apicid;
	apicid = lapicid();

	/* Send an APIC INIT to myself */
	apic_write(APIC_ICR2, SET_APIC_DEST_FIELD(apicid));
	apic_write(APIC_ICR, APIC_INT_LEVELTRIG | APIC_INT_ASSERT | APIC_DM_INIT);
	/* Wait for the ipi send to finish */
	apic_wait_icr_idle();

	/* Deassert the APIC INIT */
	apic_write(APIC_ICR2, SET_APIC_DEST_FIELD(apicid));
	apic_write(APIC_ICR,  APIC_INT_LEVELTRIG | APIC_DM_INIT);
	/* Wait for the ipi send to finish */
	apic_wait_icr_idle();

	/* If I haven't halted spin forever */
	for(;;) {
		hlt();
	}
}

#endif /* ARCH_SMP_LAPIC_H */
