#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/x86/lapic.h>
#include <cpu/intel/hyperthreading.h>
#include <device/device.h>
#include <pc80/mc146818rtc.h>
#include <smp/spinlock.h>
#include <assert.h>

#if CONFIG_BROADCAST_SIPI == 0
#if !CONFIG_SERIAL_CPU_INIT
#error Intel hyper-threading requires serialized cpu init
#endif

static int first_time = 1;
static int disable_siblings = !CONFIG_LOGICAL_CPUS;

void intel_sibling_init(device_t cpu)
{
	unsigned int core_ids, apic_ids, threads;

	/* See if I am a sibling cpu */
	if (cpu->path.apic.apic_id & (siblings -1)) {
		if (disable_siblings) {
			cpu->enabled = 0;
		}
		return;
	}
	/* I am the primary cpu start up my siblings */
	for(i = 1; i < siblings; i++) {
		struct device_path cpu_path;
		device_t new;
		/* Build the cpu device path */
		cpu_path.type = DEVICE_PATH_APIC;
		cpu_path.apic.apic_id = cpu->path.apic.apic_id + i;

		/* Allocate new cpu device structure iff sibling CPU
		 * was not in static device tree.
		 */
		new = alloc_find_dev(cpu->bus, &cpu_path);

		if (!new) {
			continue;
		}

		printk(BIOS_DEBUG, "CPU: %u has sibling %u\n",
			cpu->path.apic.apic_id,
			new->path.apic.apic_id);
	}
}

#else /* CONFIG_BROADCAST_SIPI */
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
	if (cpuid_eax(0) >= 4)
		core_ids += (cpuid_eax(4) >> 26) & 0x3f;

	threads = (apic_ids / core_ids);
	return !!(lapicid() & (threads-1));
}
#endif /* CONFIG_BROADCAST_SIPI */

