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

/* 512KB cache */
static uint32_t microcode_updates[] = {
	/* WARNING - Intel has a new data structure that has variable length
	 * microcode update lengths.  They are encoded in int 8 and 9.  A
	 * dummy header of nulls must terminate the list.
	 */

	/* Old microcode file not present in Intel's microcode.dat. */
#include "microcode_m02f2203.h"

	/* files from Intel's microcode.dat */
#include "microcode-1343-m04f252b.h"
#include "microcode-1346-m10f252c.h"
#include "microcode-1338-m02f292d.h"
#include "microcode-1340-m08f292f.h"
#include "microcode-1107-m10f2421.h"
#include "microcode-1339-m04f292e.h"
#include "microcode-1105-m08f2420.h"
#include "microcode-1336-m02f2610.h"
#include "microcode-1101-m02f2738.h"
#include "microcode-1100-m04f2737.h"
#include "microcode-1341-m01f2529.h"
#include "microcode-1102-m08f2739.h"
#include "microcode-1104-m04f241e.h"
#include "microcode-1342-m02f252a.h"
#include "microcode-1106-m02f241f.h"

	/*  Dummy terminator  */
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
};

static void model_f2x_init(device_t cpu)
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
	.init     = model_f2x_init,
};

static struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, 0x0f22 },
	{ X86_VENDOR_INTEL, 0x0f24 },
	{ X86_VENDOR_INTEL, 0x0f25 },
	{ X86_VENDOR_INTEL, 0x0f26 },
	{ X86_VENDOR_INTEL, 0x0f27 },
	{ X86_VENDOR_INTEL, 0x0f29 },
	{ 0, 0 },
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};
