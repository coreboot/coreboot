/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/cpu.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/lapic_def.h>
#include <cpu/x86/msr.h>
#include <console/console.h>
#include <stdint.h>

void enable_lapic(void)
{
	msr_t msr;

	msr = rdmsr(LAPIC_BASE_MSR);
	msr.hi &= 0xffffff00;
	msr.lo &= ~LAPIC_BASE_MSR_ADDR_MASK;
	msr.lo |= LAPIC_DEFAULT_BASE;
	msr.lo |= LAPIC_BASE_MSR_ENABLE;
	wrmsr(LAPIC_BASE_MSR, msr);

	printk(BIOS_INFO, "Setting up local APIC 0x%x\n", lapicid());
}

void disable_lapic(void)
{
	msr_t msr;
	msr = rdmsr(LAPIC_BASE_MSR);
	msr.lo &= ~LAPIC_BASE_MSR_ENABLE;
	wrmsr(LAPIC_BASE_MSR, msr);
}

uintptr_t cpu_get_lapic_addr(void)
{
	return LAPIC_DEFAULT_BASE;
}

/* See if I need to initialize the local APIC */
static int need_lapic_init(void)
{
	return CONFIG(SMP) || CONFIG(IOAPIC);
}

static void lapic_virtual_wire_mode_init(void)
{
	/*
	 * Set Task Priority to 'accept all'.
	 */
	lapic_update32(LAPIC_TASKPRI, ~LAPIC_TPRI_MASK, 0);

	/* Put the local APIC in virtual wire mode */
	lapic_update32(LAPIC_SPIV, ~LAPIC_VECTOR_MASK, LAPIC_SPIV_ENABLE);

	uint32_t mask = LAPIC_LVT_MASKED | LAPIC_LVT_LEVEL_TRIGGER | LAPIC_LVT_REMOTE_IRR |
			LAPIC_INPUT_POLARITY | LAPIC_SEND_PENDING | LAPIC_LVT_RESERVED_1 |
			LAPIC_DELIVERY_MODE_MASK;

	lapic_update32(LAPIC_LVT0, ~mask, LAPIC_LVT_REMOTE_IRR | LAPIC_SEND_PENDING |
					  LAPIC_DELIVERY_MODE_EXTINT);

	lapic_update32(LAPIC_LVT1, ~mask, LAPIC_LVT_REMOTE_IRR | LAPIC_SEND_PENDING |
					  LAPIC_DELIVERY_MODE_NMI);

}

void setup_lapic(void)
{
	/* Enable the local APIC */
	if (need_lapic_init())
		enable_lapic();
	else if (!CONFIG(UDELAY_LAPIC))
		disable_lapic();

	/* This programming is for PIC mode i8259 interrupts to be delivered to CPU
	   while LAPIC is enabled. */
	if (need_lapic_init())
		lapic_virtual_wire_mode_init();
}
