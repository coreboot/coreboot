/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/mca.h>
#include <amdblocks/reset.h>
#include <amdblocks/smm.h>
#include <console/console.h>
#include <cpu/amd/msr.h>
#include <cpu/amd/mtrr.h>
#include <cpu/cpu.h>
#include <cpu/x86/mp.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/smm.h>
#include <device/device.h>
#include <device/pci_ops.h>
#include <soc/cpu.h>
#include <soc/iomap.h>
#include <soc/northbridge.h>
#include <soc/pci_devs.h>
#include <soc/smi.h>
#include <types.h>

/*
 * MP and SMM loading initialization.
 */

/*
 * Do essential initialization tasks before APs can be fired up -
 *
 *  1. Prevent race condition in MTRR solution. Enable MTRRs on the BSP. This
 *  creates the MTRR solution that the APs will use. Otherwise APs will try to
 *  apply the incomplete solution as the BSP is calculating it.
 */
static void pre_mp_init(void)
{
	const msr_t syscfg = rdmsr(SYSCFG_MSR);
	if (syscfg.lo & SYSCFG_MSR_TOM2WB)
		x86_setup_mtrrs_with_detect_no_above_4gb();
	else
		x86_setup_mtrrs_with_detect();
	x86_mtrr_check();
}

static int get_cpu_count(void)
{
	return (pci_read_config16(SOC_HT_DEV, D18F0_CPU_CNT) & CPU_CNT_MASK) + 1;
}

static const struct mp_ops mp_ops = {
	.pre_mp_init = pre_mp_init,
	.get_cpu_count = get_cpu_count,
	.get_smm_info = get_smm_info,
	.relocation_handler = smm_relocation_handler,
	.post_mp_init = global_smi_enable,
};

void mp_init_cpus(struct bus *cpu_bus)
{
	if (mp_init_with_smm(cpu_bus, &mp_ops) != CB_SUCCESS)
		die_with_post_code(POST_HW_INIT_FAILURE,
				"mp_init_with_smm failed. Halting.\n");

	/* The flash is now no longer cacheable. Reset to WP for performance. */
	mtrr_use_temp_range(FLASH_BASE_ADDR, CONFIG_ROM_SIZE, MTRR_TYPE_WRPROT);

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
