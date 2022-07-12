/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <cpu/cpu.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/lapic_def.h>
#include <cpu/x86/msr.h>
#include <console/console.h>
#include <smp/node.h>
#include <stdint.h>

void enable_lapic_mode(bool try_set_x2apic)
{
	uintptr_t apic_base;
	bool use_x2apic = false;
	msr_t msr;

	msr = rdmsr(LAPIC_BASE_MSR);
	if (!(msr.lo & LAPIC_BASE_MSR_ENABLE)) {
		msr.hi &= 0xffffff00;
		msr.lo &= ~LAPIC_BASE_MSR_ADDR_MASK;
		msr.lo |= LAPIC_DEFAULT_BASE;
		msr.lo |= LAPIC_BASE_MSR_ENABLE;
		wrmsr(LAPIC_BASE_MSR, msr);
		msr = rdmsr(LAPIC_BASE_MSR);
	}

	ASSERT(msr.lo & LAPIC_BASE_MSR_ENABLE);

	apic_base = msr.lo & LAPIC_BASE_MSR_ADDR_MASK;
	ASSERT(apic_base == LAPIC_DEFAULT_BASE);

	if (try_set_x2apic)
		use_x2apic = !!(cpu_get_feature_flags_ecx() & CPUID_X2APIC);

	if (use_x2apic == !!(msr.lo & LAPIC_BASE_MSR_X2APIC_MODE)) {
		printk(BIOS_INFO, "LAPIC 0x%x in %s mode.\n", lapicid(),
				  use_x2apic ? "X2APIC" : "XAPIC");
	} else if (use_x2apic) {
		msr.lo |= LAPIC_BASE_MSR_X2APIC_MODE;
		wrmsr(LAPIC_BASE_MSR, msr);
		msr = rdmsr(LAPIC_BASE_MSR);
		ASSERT(!!(msr.lo & LAPIC_BASE_MSR_X2APIC_MODE));
		printk(BIOS_INFO, "LAPIC 0x%x switched to X2APIC mode.\n", lapicid());
	} else {
		die("Switching from X2APIC to XAPIC mode is not implemented.");
	}

}

void enable_lapic(void)
{
	bool try_set_x2apic = true;

	if (CONFIG(XAPIC_ONLY) || CONFIG(X2APIC_LATE_WORKAROUND))
		try_set_x2apic = false;

	enable_lapic_mode(try_set_x2apic);
}

void disable_lapic(void)
{
	msr_t msr;
	msr = rdmsr(LAPIC_BASE_MSR);
	msr.lo &= ~(LAPIC_BASE_MSR_ENABLE | LAPIC_BASE_MSR_X2APIC_MODE);
	wrmsr(LAPIC_BASE_MSR, msr);
}

uintptr_t cpu_get_lapic_addr(void)
{
	return LAPIC_DEFAULT_BASE;
}

void setup_lapic_interrupts(void)
{
	/*
	 * Set Task Priority to 'accept all'.
	 */
	lapic_update32(LAPIC_TASKPRI, ~LAPIC_TPRI_MASK, 0);

	/* Set spurious interrupt vector to 0 and keep LAPIC enabled to
	   be able to clear LVT register mask bits. */
	lapic_update32(LAPIC_SPIV, ~LAPIC_VECTOR_MASK, LAPIC_SPIV_ENABLE);

	/* Put the local APIC in virtual wire mode */
	uint32_t mask = LAPIC_LVT_MASKED | LAPIC_LVT_LEVEL_TRIGGER | LAPIC_INPUT_POLARITY |
			LAPIC_DELIVERY_MODE_MASK;

	if (boot_cpu())
		lapic_update32(LAPIC_LVT0, ~mask, LAPIC_DELIVERY_MODE_EXTINT);
	else
		lapic_update32(LAPIC_LVT0, ~mask, LAPIC_LVT_MASKED |
						  LAPIC_DELIVERY_MODE_EXTINT);

	lapic_update32(LAPIC_LVT1, ~mask, LAPIC_DELIVERY_MODE_NMI);
}
