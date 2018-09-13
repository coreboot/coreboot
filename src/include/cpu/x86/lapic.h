#ifndef CPU_X86_LAPIC_H
#define CPU_X86_LAPIC_H

#include <compiler.h>
#include <cpu/x86/lapic_def.h>
#include <cpu/x86/msr.h>
#include <halt.h>
#include <smp/node.h>

static __always_inline unsigned long lapic_read(unsigned long reg)
{
	return *((volatile unsigned long *)(LAPIC_DEFAULT_BASE+reg));
}

static __always_inline void lapic_write(unsigned long reg, unsigned long v)
{
	*((volatile unsigned long *)(LAPIC_DEFAULT_BASE+reg)) = v;
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

static __always_inline unsigned long lapicid(void)
{
	return lapic_read(LAPIC_ID) >> 24;
}

#if !IS_ENABLED(CONFIG_AP_IN_SIPI_WAIT)
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

#if !defined(__PRE_RAM__)

#define xchg(ptr, v) ((__typeof__(*(ptr)))__xchg((unsigned long)(v), (ptr), \
	sizeof(*(ptr))))

struct __xchg_dummy { unsigned long a[100]; };
#define __xg(x) ((struct __xchg_dummy *)(x))

/*
 * Note: no "lock" prefix even on SMP: xchg always implies lock anyway
 * Note 2: xchg has side effect, so that attribute volatile is necessary,
 *	  but generally the primitive is invalid, *ptr is output argument. --ANK
 */
static inline unsigned long __xchg(unsigned long x, volatile void *ptr,
	int size)
{
	switch (size) {
	case 1:
		__asm__ __volatile__("xchgb %b0,%1"
			: "=q" (x)
			: "m" (*__xg(ptr)), "0" (x)
			: "memory");
		break;
	case 2:
		__asm__ __volatile__("xchgw %w0,%1"
			: "=r" (x)
			: "m" (*__xg(ptr)), "0" (x)
			: "memory");
		break;
	case 4:
		__asm__ __volatile__("xchgl %0,%1"
			: "=r" (x)
			: "m" (*__xg(ptr)), "0" (x)
			: "memory");
		break;
	}
	return x;
}

static inline void lapic_write_atomic(unsigned long reg, unsigned long v)
{
	(void)xchg((volatile unsigned long *)(LAPIC_DEFAULT_BASE+reg), v);
}


#ifdef X86_GOOD_APIC
# define FORCE_READ_AROUND_WRITE 0
# define lapic_read_around(x) lapic_read(x)
# define lapic_write_around(x, y) lapic_write((x), (y))
#else
# define FORCE_READ_AROUND_WRITE 1
# define lapic_read_around(x) lapic_read(x)
# define lapic_write_around(x, y) lapic_write_atomic((x), (y))
#endif

static inline int lapic_remote_read(int apicid, int reg, unsigned long *pvalue)
{
	int timeout;
	unsigned long status;
	int result;
	lapic_wait_icr_idle();
	lapic_write_around(LAPIC_ICR2, SET_LAPIC_DEST_FIELD(apicid));
	lapic_write_around(LAPIC_ICR, LAPIC_DM_REMRD | (reg >> 4));
	timeout = 0;
	do {
#if 0
		udelay(100);
#endif
		status = lapic_read(LAPIC_ICR) & LAPIC_ICR_RR_MASK;
	} while (status == LAPIC_ICR_RR_INPROG && timeout++ < 1000);

	result = -1;
	if (status == LAPIC_ICR_RR_VALID) {
		*pvalue = lapic_read(LAPIC_RRR);
		result = 0;
	}
	return result;
}

void do_lapic_init(void);

/* See if I need to initialize the local APIC */
static inline int need_lapic_init(void)
{
	return IS_ENABLED(CONFIG_SMP) || IS_ENABLED(CONFIG_IOAPIC);
}

static inline void setup_lapic(void)
{
	if (need_lapic_init())
		do_lapic_init();
	else
		disable_lapic();
}

struct device;
int start_cpu(struct device *cpu);

#endif /* !__PRE_RAM__ */

#endif /* CPU_X86_LAPIC_H */
