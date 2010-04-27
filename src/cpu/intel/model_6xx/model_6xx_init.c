#include <console/console.h>
#include <device/device.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <cpu/cpu.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/lapic.h>
#include <cpu/intel/microcode.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/mtrr.h>

static uint32_t microcode_updates[] = {
	/* WARNING - Intel has a new data structure that has variable length
	 * microcode update lengths.  They are encoded in int 8 and 9.  A
	 * dummy header of nulls must terminate the list.
	 */
#include "microcode_MU16810d.h"
#include "microcode_MU16830c.h"
	/*  Dummy terminator  */
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
};


static void model_6xx_init(device_t dev)
{
	/* Turn on caching if we haven't already */
	x86_enable_cache();
	x86_setup_mtrrs(36);
	x86_mtrr_check();

	/* Update the microcode */
	intel_update_microcode(microcode_updates);

	/* Enable the local cpu apics */
	setup_lapic();
};

static struct device_operations cpu_dev_ops = {
	.init     = model_6xx_init,
};
static struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, 0x0650 },
	{ X86_VENDOR_INTEL, 0x0652 },
	{ X86_VENDOR_INTEL, 0x0660 }, /* Celeron (Mendocino) */
	{ X86_VENDOR_INTEL, 0x0665 },
	{ X86_VENDOR_INTEL, 0x0672 },
	{ X86_VENDOR_INTEL, 0x0673 },
	{ X86_VENDOR_INTEL, 0x0680 },
	{ X86_VENDOR_INTEL, 0x0681 },
	{ X86_VENDOR_INTEL, 0x0683 },
	{ X86_VENDOR_INTEL, 0x0686 },
	{ X86_VENDOR_INTEL, 0x06A0 },
	{ X86_VENDOR_INTEL, 0x06A1 },
	{ X86_VENDOR_INTEL, 0x06A4 },
	{ 0, 0 },
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};
