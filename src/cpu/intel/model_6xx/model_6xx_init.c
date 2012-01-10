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
	/* WARNING - Intel has a new data structure that has variable length
	 * microcode update lengths.  They are encoded in int 8 and 9.  A
	 * dummy header of nulls must terminate the list.
	 */

#include "microcode-99-B_c6_612.h"
#include "microcode-43-B_c6_617.h"
#include "microcode-51-B_c6_616.h"
#include "microcode-153-d2_619.h"

#include "microcode-308-MU163336.h"
#include "microcode-309-MU163437.h"

#include "microcode-358-MU166d05.h"
#include "microcode-359-MU166d06.h"
#include "microcode-386-MU16600a.h"
#include "microcode-398-MU166503.h"
#include "microcode-399-MU166a0b.h"
#include "microcode-400-MU166a0c.h"
#include "microcode-401-MU166a0d.h"
#include "microcode-402-MU166d07.h"

#include "microcode-566-mu26a003.h"
#include "microcode-588-mu26a101.h"
#include "microcode-620-MU26a401.h"

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
	x86_setup_mtrrs();
	x86_mtrr_check();

	/* Update the microcode */
	intel_update_microcode(microcode_updates);

	/* Enable the local cpu apics */
	setup_lapic();
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
static struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, 0x0611 }, /* Pentium Pro, B0 */
	{ X86_VENDOR_INTEL, 0x0612 }, /* Pentium Pro, C0 */
	{ X86_VENDOR_INTEL, 0x0616 }, /* Pentium Pro, sA0 */
	{ X86_VENDOR_INTEL, 0x0617 }, /* Pentium Pro, sA1 */
	{ X86_VENDOR_INTEL, 0x0619 }, /* Pentium Pro, sB1 */

	{ X86_VENDOR_INTEL, 0x0633 }, /* PII, C0 */
	{ X86_VENDOR_INTEL, 0x0634 }, /* PII, C1 */

	{ X86_VENDOR_INTEL, 0x0660 }, /* Celeron, A0 */
	{ X86_VENDOR_INTEL, 0x0665 }, /* Celeron, B0 */
	{ X86_VENDOR_INTEL, 0x066a }, /* PII, mdxA0/dmmA0 + others */

	{ X86_VENDOR_INTEL, 0x0680 },
	{ X86_VENDOR_INTEL, 0x0681 }, /* PIII, cA2/cA2c/A2/BA2/PA2/MA2 */
	{ X86_VENDOR_INTEL, 0x0683 }, /* PIII/Celeron, cB0/cB0c/B0/BB0/PB0/MB0*/
	{ X86_VENDOR_INTEL, 0x0686 }, /* PIII/Celeron, cC0/C0/BC0/PC0/MC0 */
	{ X86_VENDOR_INTEL, 0x068a }, /* PIII/Celeron, cD0/D0/BD0/PD0 */

	{ X86_VENDOR_INTEL, 0x06a0 }, /* PIII, A0 */
	{ X86_VENDOR_INTEL, 0x06a1 }, /* PIII, A1 */
	{ X86_VENDOR_INTEL, 0x06a4 }, /* PIII, B0 */
	{ 0, 0 },
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};
