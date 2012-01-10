#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <cpu/cpu.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/lapic.h>
#include <cpu/intel/microcode.h>
#include <cpu/x86/cache.h>

static uint32_t microcode_updates[] = {
	#include "microcode-1376-m8069547.h"
	#include "microcode-1373-m1069507.h"
	#include "microcode-1374-m2069507.h"

	/*  Dummy terminator  */
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
};

static void model_69x_init(device_t dev)
{
	/* Turn on caching if we haven't already */
	x86_enable_cache();
	x86_setup_mtrrs();
	x86_mtrr_check();

	/* Update the microcode */
	intel_update_microcode(microcode_updates);

	/* Enable the local cpu apics */
	setup_lapic();
};

static struct device_operations cpu_dev_ops = {
	.init     = model_69x_init,
};

static struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, 0x0690 }, /* Pentium M */
	{ X86_VENDOR_INTEL, 0x0695 },
	{ 0, 0 },
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};
