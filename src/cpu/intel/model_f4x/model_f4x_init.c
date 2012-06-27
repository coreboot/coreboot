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

	#include "microcode-1735-m01f480c.h"
	#include "microcode-1460-m9df4305.h"
	#include "microcode-2492-m02f480e.h"
	#include "microcode-1470-m9df4703.h"
	#include "microcode-1521-m5ff4807.h"
	#include "microcode-1466-m02f4116.h"
	#include "microcode-1469-m9df4406.h"
	#include "microcode-1471-mbdf4117.h"
	#include "microcode-1637-m5cf4a04.h"
	#include "microcode-1462-mbdf4903.h"
	#include "microcode-1498-m5df4a02.h"

	/*  Dummy terminator  */
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
};

static void model_f4x_init(device_t cpu)
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
	.init = model_f4x_init,
};

static struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, 0x0f41 }, /* Xeon */
	{ 0, 0 },
};

static const struct cpu_driver model_f4x __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};
