/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef CPU_X86_LAPIC_H
#define CPU_X86_LAPIC_H

#include <arch/mmio.h>
#include <arch/cpu.h>
#include <cpu/x86/lapic_def.h>
#include <cpu/x86/msr.h>
#include <halt.h>
#include <stdint.h>

static __always_inline uint32_t xapic_read(unsigned int reg)
{
	return read32((volatile void *)(uintptr_t)(LAPIC_DEFAULT_BASE + reg));
}

static __always_inline void xapic_write(unsigned int reg, uint32_t v)
{
	write32((volatile void *)(uintptr_t)(LAPIC_DEFAULT_BASE + reg), v);
}

static __always_inline void xapic_send_ipi(uint32_t icrlow, uint32_t icrhi)
{
	xapic_write(LAPIC_ICR2, icrhi);
	xapic_write(LAPIC_ICR, icrlow);
}

static __always_inline int xapic_busy(void)
{
	return xapic_read(LAPIC_ICR) & LAPIC_ICR_BUSY;
}

static __always_inline uint32_t x2apic_read(unsigned int reg)
{
	uint32_t value, index;
	msr_t msr;

	index = X2APIC_MSR_BASE_ADDRESS + (uint32_t)(reg >> 4);
	msr = rdmsr(index);
	value = msr.lo;
	return value;
}

static __always_inline void x2apic_write(unsigned int reg, uint32_t v)
{
	uint32_t index;
	msr_t msr;

	index = X2APIC_MSR_BASE_ADDRESS + (uint32_t)(reg >> 4);
	msr.hi = 0x0;
	msr.lo = v;
	wrmsr(index, msr);
}

static __always_inline void x2apic_send_ipi(uint32_t icrlow, uint32_t icrhi)
{
	msr_t icr;
	icr.hi = icrhi;
	icr.lo = icrlow;
	wrmsr(X2APIC_MSR_ICR_ADDRESS, icr);
}

static __always_inline bool is_x2apic_mode(void)
{
	if (CONFIG(XAPIC_ONLY))
		return false;

	if (CONFIG(X2APIC_ONLY))
		return true;

	msr_t msr;
	msr = rdmsr(LAPIC_BASE_MSR);
	return ((msr.lo & LAPIC_BASE_X2APIC_ENABLED) == LAPIC_BASE_X2APIC_ENABLED);
}

static __always_inline uint32_t lapic_read(unsigned int reg)
{
	if (is_x2apic_mode())
		return x2apic_read(reg);
	else
		return xapic_read(reg);
}

static __always_inline void lapic_write(unsigned int reg, uint32_t v)
{
	if (is_x2apic_mode())
		x2apic_write(reg, v);
	else
		xapic_write(reg, v);
}

static __always_inline void lapic_update32(unsigned int reg, uint32_t mask, uint32_t or)
{
	if (is_x2apic_mode()) {
		uint32_t index;
		msr_t msr;
		index = X2APIC_MSR_BASE_ADDRESS + (uint32_t)(reg >> 4);
		msr = rdmsr(index);
		msr.lo &= mask;
		msr.lo |= or;
		wrmsr(index, msr);
	} else {
		uint32_t value;
		value = xapic_read(reg);
		value &= mask;
		value |= or;
		xapic_write(reg, value);
	}
}

static __always_inline void lapic_send_ipi(uint32_t icrlow, uint32_t apicid)
{
	if (is_x2apic_mode())
		x2apic_send_ipi(icrlow, apicid);
	else
		xapic_send_ipi(icrlow, SET_LAPIC_DEST_FIELD(apicid));
}

static __always_inline int lapic_busy(void)
{
	if (is_x2apic_mode())
		return 0;
	else
		return xapic_busy();
}

static __always_inline unsigned int initial_lapicid(void)
{
	uint32_t lapicid;
	if (is_x2apic_mode() && cpuid_get_max_func() >= 0xb)
		lapicid = cpuid_ext(0xb, 0).edx;
	else
		lapicid = cpuid_ebx(1) >> 24;
	return lapicid;
}

static __always_inline unsigned int lapicid(void)
{
	uint32_t lapicid = lapic_read(LAPIC_ID);

	/* check x2apic mode and return accordingly */
	if (!is_x2apic_mode())
		lapicid >>= 24;
	return lapicid;
}

static __always_inline void lapic_send_ipi_self(uint32_t icrlow)
{
	int i = 1000;

	/* LAPIC_DEST_SELF does not support all delivery mode -fields. */
	lapic_send_ipi(icrlow, lapicid());

	/* In case of X2APIC force a short delay, to prevent deadlock in a case
	 * the immediately following code acquires some lock, like with printk().
	 */
	while (CONFIG(X2APIC_ONLY) && i--)
		cpu_relax();
}

static __always_inline void lapic_send_ipi_others(uint32_t icrlow)
{
	lapic_send_ipi(LAPIC_DEST_ALLBUT | icrlow, 0);
}

#if !CONFIG(AP_IN_SIPI_WAIT)
/* If we need to go back to sipi wait, we use the long non-inlined version of
 * this function in lapic_cpu_stop.c
 */
static __always_inline void stop_this_cpu(void)
{
	/* Called by an AP when it is ready to halt and wait for a new task */
	halt();
}
#else
void stop_this_cpu(void);
#endif

void enable_lapic(void);
void enable_lapic_mode(bool try_set_x2apic);
void disable_lapic(void);
void setup_lapic_interrupts(void);

#endif /* CPU_X86_LAPIC_H */
