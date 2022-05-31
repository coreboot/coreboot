/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/cpu.h>
#include <amdblocks/iomap.h>
#include <amdblocks/mca.h>
#include <amdblocks/reset.h>
#include <cpu/amd/msr.h>
#include <cpu/cpu.h>
#include <cpu/x86/mp.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <device/device.h>
#include <device/pci_ops.h>
#include <soc/pci_devs.h>
#include <soc/cpu.h>
#include <soc/iomap.h>
#include <console/console.h>

/*
 * MP and SMM loading initialization.
 */
void mp_init_cpus(struct bus *cpu_bus)
{
	extern const struct mp_ops amd_mp_ops_with_smm;
	if (mp_init_with_smm(cpu_bus, &amd_mp_ops_with_smm) != CB_SUCCESS)
		die_with_post_code(POST_HW_INIT_FAILURE,
				"mp_init_with_smm failed. Halting.\n");

	/* The flash is now no longer cacheable. Reset to WP for performance. */
	mtrr_use_temp_range(FLASH_BELOW_4GB_MAPPING_REGION_BASE,
			    FLASH_BELOW_4GB_MAPPING_REGION_SIZE, MTRR_TYPE_WRPROT);

	set_warm_reset_flag();
}

static void model_15_init(struct device *dev)
{
	check_mca();

	/*
	 * Per AMD, sync an undocumented MSR with the PSP base address.
	 * Experiments showed that if you write to the MSR after it has
	 * been previously programmed, it causes a general protection fault.
	 * Also, the MSR survives warm reset and S3 cycles, so we need to
	 * test if it was previously written before writing to it.
	 */
	msr_t psp_msr;
	uint32_t psp_bar; /* Note: NDA BKDG names this 32-bit register BAR3 */
	psp_bar = pci_read_config32(SOC_PSP_DEV, PCI_BASE_ADDRESS_4);
	psp_bar &= ~PCI_BASE_ADDRESS_MEM_ATTR_MASK;
	psp_msr = rdmsr(PSP_ADDR_MSR);
	if (psp_msr.lo == 0) {
		psp_msr.lo = psp_bar;
		wrmsr(PSP_ADDR_MSR, psp_msr);
	}
}

static struct device_operations cpu_dev_ops = {
	.init = model_15_init,
};

static struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_AMD, 0x660f01 },
	{ X86_VENDOR_AMD, 0x670f00 },
	{ 0, 0 },
};

static const struct cpu_driver model_15 __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};
