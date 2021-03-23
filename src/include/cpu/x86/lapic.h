#ifndef CPU_X86_LAPIC_H
#define CPU_X86_LAPIC_H

#include <arch/mmio.h>
#include <arch/cpu.h>
#include <cpu/x86/lapic_def.h>
#include <cpu/x86/msr.h>
#include <halt.h>
#include <stdint.h>

static inline bool is_x2apic_mode(void)
{
	msr_t msr;
	msr = rdmsr(LAPIC_BASE_MSR);
	return (msr.lo & LAPIC_BASE_MSR_X2APIC_MODE);
}

static inline void x2apic_send_ipi(uint32_t icrlow, uint32_t apicid)
{
	msr_t icr;
	icr.hi = apicid;
	icr.lo = icrlow;
	wrmsr(X2APIC_MSR_ICR_ADDRESS, icr);
}

static __always_inline uint32_t lapic_read(unsigned int reg)
{
	uint32_t value, index;
	msr_t msr;

	if (is_x2apic_mode()) {
		index = X2APIC_MSR_BASE_ADDRESS + (uint32_t)(reg >> 4);
		msr = rdmsr(index);
		value = msr.lo;
	} else {
		value = read32((volatile void *)(uintptr_t)(LAPIC_DEFAULT_BASE + reg));
	}
	return value;
}

static __always_inline void lapic_write(unsigned int reg, uint32_t v)
{
	msr_t msr;
	uint32_t index;
	if (is_x2apic_mode()) {
		index = X2APIC_MSR_BASE_ADDRESS + (uint32_t)(reg >> 4);
		msr.hi = 0x0;
		msr.lo = v;
		wrmsr(index, msr);
	} else {
		write32((volatile void *)(uintptr_t)(LAPIC_DEFAULT_BASE + reg), v);
	}
}

static __always_inline void lapic_wait_icr_idle(void)
{
	do { } while (lapic_read(LAPIC_ICR) & LAPIC_ICR_BUSY);
}

static inline void enable_lapic(void)
{
	msr_t msr;
	msr = rdmsr(LAPIC_BASE_MSR);
	msr.hi &= 0xffffff00;
	msr.lo &= ~LAPIC_BASE_MSR_ADDR_MASK;
	msr.lo |= LAPIC_DEFAULT_BASE;
	msr.lo |= LAPIC_BASE_MSR_ENABLE;
	wrmsr(LAPIC_BASE_MSR, msr);
}

static inline void disable_lapic(void)
{
	msr_t msr;
	msr = rdmsr(LAPIC_BASE_MSR);
	msr.lo &= ~LAPIC_BASE_MSR_ENABLE;
	wrmsr(LAPIC_BASE_MSR, msr);
}

static __always_inline unsigned int initial_lapicid(void)
{
	uint32_t lapicid;
	if (is_x2apic_mode())
		lapicid = lapic_read(LAPIC_ID);
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

#if !CONFIG(AP_IN_SIPI_WAIT)
/* If we need to go back to sipi wait, we use the long non-inlined version of
 * this function in lapic_cpu_init.c
 */
static __always_inline void stop_this_cpu(void)
{
	/* Called by an AP when it is ready to halt and wait for a new task */
	halt();
}
#else
void stop_this_cpu(void);
#endif

static inline void lapic_write_atomic(unsigned long reg, uint32_t v)
{
	volatile uint32_t *ptr;

	ptr = (volatile uint32_t *)(LAPIC_DEFAULT_BASE + reg);

	asm volatile ("xchgl %0, %1\n"
		      : "+r" (v), "+m" (*(ptr))
		      : : "memory", "cc");
}

# define lapic_read_around(x) lapic_read(x)
# define lapic_write_around(x, y) lapic_write_atomic((x), (y))

void lapic_virtual_wire_mode_init(void);

/* See if I need to initialize the local APIC */
static inline int need_lapic_init(void)
{
	return CONFIG(SMP) || CONFIG(IOAPIC);
}

static inline void setup_lapic(void)
{
	if (need_lapic_init())
		lapic_virtual_wire_mode_init();
	else
		disable_lapic();
}

struct device;
int start_cpu(struct device *cpu);

#endif /* CPU_X86_LAPIC_H */
