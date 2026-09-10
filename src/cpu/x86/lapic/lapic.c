/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/ioapic.h>
#include <assert.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/lapic_def.h>
#include <cpu/x86/msr.h>
#include <smp/node.h>
#include <types.h>

static bool quirk_x2apic_allowed;

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

	if (CONFIG(X2APIC_LATE_WORKAROUND) && use_x2apic)
		quirk_x2apic_allowed = true;
}

void enable_lapic(void)
{
	bool try_set_x2apic = true;

	if (CONFIG(XAPIC_ONLY))
		try_set_x2apic = false;
	else if (CONFIG(X2APIC_LATE_WORKAROUND))
		try_set_x2apic = quirk_x2apic_allowed;

	enable_lapic_mode(try_set_x2apic);
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

	/* Set spurious interrupt vector to 0xF and keep LAPIC enabled to
	   be able to clear LVT register mask bits. */
	lapic_update32(LAPIC_SPIV, ~LAPIC_VECTOR_MASK, LAPIC_SPIV_ENABLE | 0xF);

	uint32_t mask = LAPIC_LVT_MASKED | LAPIC_LVT_LEVEL_TRIGGER | LAPIC_INPUT_POLARITY |
			LAPIC_DELIVERY_MODE_MASK;

	/* Put LINT0 in virtual wire mode but leave it masked. */
	lapic_update32(LAPIC_LVT0, ~mask, LAPIC_LVT_MASKED | LAPIC_DELIVERY_MODE_EXTINT);
	lapic_update32(LAPIC_LVT1, ~mask, LAPIC_DELIVERY_MODE_NMI);
}

/* No IOAPIC in this link: tell lapic_enable_extint() to unmask LINT0. */
int __weak ioapic_enable_extint(void) { return -1; }
void __weak ioapic_disable_extint(void) { }

void lapic_disable_extint(void)
{
	uint32_t mask = LAPIC_LVT_MASKED | LAPIC_LVT_LEVEL_TRIGGER | LAPIC_INPUT_POLARITY |
			LAPIC_DELIVERY_MODE_MASK;

	ioapic_disable_extint();
	lapic_update32(LAPIC_LVT0, ~mask, LAPIC_LVT_MASKED | LAPIC_DELIVERY_MODE_EXTINT);
}

void lapic_enable_extint(void)
{
	uint32_t mask = LAPIC_LVT_MASKED | LAPIC_LVT_LEVEL_TRIGGER | LAPIC_INPUT_POLARITY |
			LAPIC_DELIVERY_MODE_MASK;

	/*
	 * Prefer ExtINT via IOAPIC GSI0 when that route is ready. If GSI0 is
	 * not registered yet (or never will be), unmask LINT0 so the legacy
	 * PIC can drive virtual-wire ExtINT until/unless GSI0 takes over.
	 */
	if (ioapic_enable_extint() < 0)
		lapic_update32(LAPIC_LVT0, ~mask, LAPIC_DELIVERY_MODE_EXTINT);
}
