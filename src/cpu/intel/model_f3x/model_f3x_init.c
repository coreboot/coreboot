#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <cpu/cpu.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/lapic.h>
#include <cpu/intel/microcode.h>
#include <cpu/intel/hyperthreading.h>
#include <cpu/x86/cache.h>

static uint32_t microcode_updates[] = {
	/* WARNING - Intel has a new data structure that has variable length
	 * microcode update lengths.  They are encoded in int 8 and 9.  A
	 * dummy header of nulls must terminate the list.
	 */

#include "microcode-1290-m0df320a.h"
#include "microcode-1467-m0df330c.h"
#include "microcode-1468-m1df3417.h"

	/*  Dummy terminator  */
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
};

static void model_f3x_init(device_t cpu)
{
	/* Turn on caching if we haven't already */
	x86_enable_cache();

	if (!intel_ht_sibling()) {
		/* MTRRs are shared between threads */
		x86_setup_mtrrs();
		x86_mtrr_check();

		/* Update the microcode */
		intel_update_microcode(microcode_updates);
	}

	/* Enable the local cpu apics */
	setup_lapic();

	/* Start up my cpu siblings */
	intel_sibling_init(cpu);
};

static struct device_operations cpu_dev_ops = {
	.init = model_f3x_init,
};

static struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, 0x0f34 }, /* Xeon */
	{ 0, 0 },
};

static const struct cpu_driver model_f3x __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};
