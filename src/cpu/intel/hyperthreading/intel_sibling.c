#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/x86/lapic.h>
#include <cpu/intel/hyperthreading.h>
#include <device/device.h>
#include <pc80/mc146818rtc.h>
#include <smp/spinlock.h>
#include <assert.h>

/* Return true if running thread does not have the smallest lapic ID
 * within a CPU core.
 */
int intel_ht_sibling(void)
{
	unsigned int core_ids, apic_ids, threads;

	apic_ids = 1;
	if (cpuid_eax(0) >= 1)
		apic_ids = (cpuid_ebx(1) >> 16) & 0xff;
	if (apic_ids < 1)
		apic_ids = 1;

	core_ids = 1;
	if (cpuid_eax(0) >= 4) {
		struct cpuid_result result;
		result = cpuid_ext(4, 0);
		core_ids += (result.eax >> 26) & 0x3f;
	}

	threads = (apic_ids / core_ids);
	return !!(lapicid() & (threads-1));
}
