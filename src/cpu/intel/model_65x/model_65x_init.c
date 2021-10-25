/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <cpu/cpu.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/lapic.h>
#include <cpu/intel/microcode.h>
#include <cpu/x86/cache.h>
#include <cpu/intel/l2_cache.h>

static void model_65x_init(struct device *dev)
{
	/* Update the microcode */
	intel_update_microcode_from_cbfs();
	/* Initialize L2 cache */
	p6_configure_l2_cache();

	/* Turn on caching if we haven't already */
	enable_cache();
	x86_setup_mtrrs();
	x86_mtrr_check();

	/* Enable the local CPU APICs */
	setup_lapic();
};

static struct device_operations cpu_dev_ops = {
	.init     = model_65x_init,
};

/*
 * Intel Pentium II Processor Specification Update
 * http://download.intel.com/design/PentiumII/specupdt/24333749.pdf
 *
 * Mobile Intel Pentium II Processor Specification Update
 * http://download.intel.com/design/intarch/specupdt/24388757.pdf
 *
 * Intel Pentium II Xeon Processor Specification Update
 * http://download.intel.com/support/processors/pentiumii/xeon/24377632.pdf
 */
static const struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, 0x0650 }, /* PII/Celeron, dA0/mdA0/A0 */
	{ X86_VENDOR_INTEL, 0x0651 }, /* PII/Celeron, dA1/A1 */
	{ X86_VENDOR_INTEL, 0x0652 }, /* PII/Celeron/Xeon, dB0/mdB0/B0 */
	{ X86_VENDOR_INTEL, 0x0653 }, /* PII/Xeon, dB1/B1 */
	{ 0, 0 },
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};
