/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <cpu/cpu.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/lapic.h>
#include <cpu/intel/microcode.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/name.h>

static void model_6bx_init(struct device *cpu)
{
	char processor_name[49];

	/* Turn on caching if we haven't already */
	enable_cache();

	/* Update the microcode */
	intel_update_microcode_from_cbfs();

	/* Print processor name */
	fill_processor_name(processor_name);
	printk(BIOS_INFO, "CPU: %s.\n", processor_name);

	/* Setup MTRRs */
	x86_setup_mtrrs();
	x86_mtrr_check();

	/* Enable the local CPU APICs */
	setup_lapic();
}

static struct device_operations cpu_dev_ops = {
	.init     = model_6bx_init,
};

/*
 * Pentium III Processor Identification and Package Information.
 * http://www.intel.com/design/pentiumiii/qit/update.pdf
 *
 * Intel Pentium III Processor Specification Update
 * http://download.intel.com/design/intarch/specupdt/24445358.pdf
 */
static const struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, 0x06b1 }, /* Pentium III/Celeron, tA1/A1/FPA1 */
	{ X86_VENDOR_INTEL, 0x06b4 }, /* Pentium III, tB1/FPB1 */
	{ 0, 0 },
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};
