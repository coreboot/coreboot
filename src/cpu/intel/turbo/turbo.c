/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/intel/turbo.h>
#include <cpu/x86/msr.h>

#if CONFIG(CPU_INTEL_TURBO_NOT_PACKAGE_SCOPED)
static inline int get_global_turbo_state(void)
{
	return TURBO_UNKNOWN;
}

static inline void set_global_turbo_state(int state)
{
}
#else
static int g_turbo_state = TURBO_UNKNOWN;

static inline int get_global_turbo_state(void)
{
	return g_turbo_state;
}

static inline void set_global_turbo_state(int state)
{
	g_turbo_state = state;
}
#endif

static const char *const turbo_state_desc[] = {
	[TURBO_UNKNOWN]		= "unknown",
	[TURBO_UNAVAILABLE]	= "unavailable",
	[TURBO_DISABLED]	= "available but hidden",
	[TURBO_ENABLED]		= "available and visible"
};

/*
 * Try to update the global Turbo state.
 */
static int update_turbo_state(void)
{
	struct cpuid_result cpuid_regs;
	int turbo_en, turbo_cap;
	msr_t msr;
	int turbo_state = get_global_turbo_state();

	cpuid_regs = cpuid(CPUID_LEAF_PM);
	turbo_cap = !!(cpuid_regs.eax & PM_CAP_TURBO_MODE);

	msr = rdmsr(IA32_MISC_ENABLE);
	turbo_en = !(msr.hi & H_MISC_DISABLE_TURBO);

	if (!turbo_cap && turbo_en) {
		/* Unavailable */
		turbo_state = TURBO_UNAVAILABLE;
	} else if (!turbo_cap && !turbo_en) {
		/* Available but disabled */
		turbo_state = TURBO_DISABLED;
	} else if (turbo_cap && turbo_en) {
		/* Available */
		turbo_state = TURBO_ENABLED;
	}

	set_global_turbo_state(turbo_state);
	printk(BIOS_INFO, "Turbo is %s\n", turbo_state_desc[turbo_state]);

	return turbo_state;
}

/*
 * Determine the current state of Turbo and cache it for later. Turbo is package
 * level config so it does not need to be enabled on every core.
 */
int get_turbo_state(void)
{
	int turbo_state = get_global_turbo_state();

	/* Return cached state if available */
	if (turbo_state == TURBO_UNKNOWN)
		turbo_state = update_turbo_state();

	return turbo_state;
}

/*
 * Try to enable Turbo mode.
 */
void enable_turbo(void)
{
	msr_t msr;

	/* Only possible if turbo is available but hidden */
	if (get_turbo_state() == TURBO_DISABLED) {
		/* Clear Turbo Disable bit in Misc Enables */
		msr = rdmsr(IA32_MISC_ENABLE);
		msr.hi &= ~H_MISC_DISABLE_TURBO;
		wrmsr(IA32_MISC_ENABLE, msr);

		/* Update cached turbo state */
		update_turbo_state();
	}
}

/*
 * Try to disable Turbo mode.
 */
void disable_turbo(void)
{
	msr_t msr;

	/* Only possible if turbo is available and visible */
	if (get_turbo_state() == TURBO_ENABLED) {
		/* Set Turbo Disable bit in Misc Enables */
		msr = rdmsr(IA32_MISC_ENABLE);
		msr.hi |= H_MISC_DISABLE_TURBO;
		wrmsr(IA32_MISC_ENABLE, msr);

		/* Update cached turbo state */
		update_turbo_state();
	}
}
