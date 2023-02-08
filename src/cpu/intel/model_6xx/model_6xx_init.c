/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <cpu/cpu.h>
#include <cpu/x86/mtrr.h>
#include <cpu/intel/microcode.h>
#include <cpu/x86/cache.h>

static void model_6xx_init(struct device *dev)
{
	/* Turn on caching if we haven't already */
	enable_cache();
	x86_setup_mtrrs();
	x86_mtrr_check();

	/* Update the microcode */
	intel_update_microcode_from_cbfs();
};

static struct device_operations cpu_dev_ops = {
	.init     = model_6xx_init,
};

/*
 * Intel Pentium Pro Processor Specification Update
 * http://download.intel.com/design/archives/processors/pro/docs/24268935.pdf
 *
 * Intel Pentium II Processor Specification Update
 * http://download.intel.com/design/PentiumII/specupdt/24333749.pdf
 *
 * Mobile Intel Pentium II Processor Specification Update
 * http://download.intel.com/design/intarch/specupdt/24388757.pdf
 *
 * Intel Celeron Processor Identification Information
 * http://www.intel.com/design/celeron/qit/update.pdf
 *
 * Intel Pentium II Xeon Processor Specification Update
 * http://download.intel.com/support/processors/pentiumii/xeon/24377632.pdf
 *
 * Intel Pentium III Processor Identification and Package Information
 * http://www.intel.com/design/pentiumiii/qit/update.pdf
 *
 * Intel Pentium III Processor Specification Update
 * http://download.intel.com/design/intarch/specupdt/24445358.pdf
 *
 * Mobile Intel Pentium III/III-M Processor Specification Update
 * http://download.intel.com/design/intarch/specupdt/24530663.pdf
 */
static const struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, 0x0611, CPUID_EXACT_MATCH_MASK }, /* Pentium Pro, B0 */
	{ X86_VENDOR_INTEL, 0x0612, CPUID_EXACT_MATCH_MASK }, /* Pentium Pro, C0 */
	{ X86_VENDOR_INTEL, 0x0616, CPUID_EXACT_MATCH_MASK }, /* Pentium Pro, sA0 */
	{ X86_VENDOR_INTEL, 0x0617, CPUID_EXACT_MATCH_MASK }, /* Pentium Pro, sA1 */
	{ X86_VENDOR_INTEL, 0x0619, CPUID_EXACT_MATCH_MASK }, /* Pentium Pro, sB1 */

	{ X86_VENDOR_INTEL, 0x0633, CPUID_EXACT_MATCH_MASK }, /* PII, C0 */
	{ X86_VENDOR_INTEL, 0x0634, CPUID_EXACT_MATCH_MASK }, /* PII, C1 */

	{ X86_VENDOR_INTEL, 0x0660, CPUID_EXACT_MATCH_MASK }, /* Celeron, A0 */
	{ X86_VENDOR_INTEL, 0x0665, CPUID_EXACT_MATCH_MASK }, /* Celeron, B0 */
	{ X86_VENDOR_INTEL, 0x066a, CPUID_EXACT_MATCH_MASK }, /* PII, mdxA0/dmmA0 + others */

	{ X86_VENDOR_INTEL, 0x0680, CPUID_EXACT_MATCH_MASK },
	/* PIII, cA2/cA2c/A2/BA2/PA2/MA2 */
	{ X86_VENDOR_INTEL, 0x0681, CPUID_EXACT_MATCH_MASK },
	/* PIII/Celeron, cB0/cB0c/B0/BB0/PB0/MB0*/
	{ X86_VENDOR_INTEL, 0x0683, CPUID_EXACT_MATCH_MASK },
	/* PIII/Celeron, cC0/C0/BC0/PC0/MC0 */
	{ X86_VENDOR_INTEL, 0x0686, CPUID_EXACT_MATCH_MASK },
	/* PIII/Celeron, cD0/D0/BD0/PD0 */
	{ X86_VENDOR_INTEL, 0x068a, CPUID_EXACT_MATCH_MASK },

	{ X86_VENDOR_INTEL, 0x06a0, CPUID_EXACT_MATCH_MASK }, /* PIII, A0 */
	{ X86_VENDOR_INTEL, 0x06a1, CPUID_EXACT_MATCH_MASK }, /* PIII, A1 */
	{ X86_VENDOR_INTEL, 0x06a4, CPUID_EXACT_MATCH_MASK }, /* PIII, B0 */
	CPU_TABLE_END
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};
