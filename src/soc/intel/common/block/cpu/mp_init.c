/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <bootstate.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mp.h>
#include <cpu/intel/microcode.h>
#include <intelblocks/cfg.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/fast_spi.h>
#include <intelblocks/mp_init.h>
#include <intelblocks/msr.h>
#include <soc/cpu.h>

/* SoC override function */
__weak void soc_core_init(struct device *dev)
{
	/* no-op */
}

__weak void soc_init_cpus(struct bus *cpu_bus)
{
	/* no-op */
}

static void init_one_cpu(struct device *dev)
{
	soc_core_init(dev);

	const void *microcode_patch = intel_microcode_find();
	intel_microcode_load_unlocked(microcode_patch);
}

static struct device_operations cpu_dev_ops = {
	.init = init_one_cpu,
};

static const struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, CPUID_SKYLAKE_C0 },
	{ X86_VENDOR_INTEL, CPUID_SKYLAKE_D0 },
	{ X86_VENDOR_INTEL, CPUID_SKYLAKE_HQ0 },
	{ X86_VENDOR_INTEL, CPUID_SKYLAKE_HR0 },
	{ X86_VENDOR_INTEL, CPUID_KABYLAKE_G0 },
	{ X86_VENDOR_INTEL, CPUID_KABYLAKE_H0 },
	{ X86_VENDOR_INTEL, CPUID_KABYLAKE_Y0 },
	{ X86_VENDOR_INTEL, CPUID_KABYLAKE_HA0 },
	{ X86_VENDOR_INTEL, CPUID_KABYLAKE_HB0 },
	{ X86_VENDOR_INTEL, CPUID_CANNONLAKE_A0 },
	{ X86_VENDOR_INTEL, CPUID_CANNONLAKE_B0 },
	{ X86_VENDOR_INTEL, CPUID_CANNONLAKE_C0 },
	{ X86_VENDOR_INTEL, CPUID_CANNONLAKE_D0 },
	{ X86_VENDOR_INTEL, CPUID_APOLLOLAKE_A0 },
	{ X86_VENDOR_INTEL, CPUID_APOLLOLAKE_B0 },
	{ X86_VENDOR_INTEL, CPUID_APOLLOLAKE_E0 },
	{ X86_VENDOR_INTEL, CPUID_GLK_A0 },
	{ X86_VENDOR_INTEL, CPUID_GLK_B0 },
	{ X86_VENDOR_INTEL, CPUID_GLK_R0 },
	{ X86_VENDOR_INTEL, CPUID_WHISKEYLAKE_V0 },
	{ X86_VENDOR_INTEL, CPUID_WHISKEYLAKE_W0 },
	{ X86_VENDOR_INTEL, CPUID_COFFEELAKE_U0 },
	{ X86_VENDOR_INTEL, CPUID_COFFEELAKE_B0 },
	{ X86_VENDOR_INTEL, CPUID_COFFEELAKE_P0 },
	{ X86_VENDOR_INTEL, CPUID_COFFEELAKE_R0 },
	{ X86_VENDOR_INTEL, CPUID_ICELAKE_A0 },
	{ X86_VENDOR_INTEL, CPUID_ICELAKE_B0 },
	{ X86_VENDOR_INTEL, CPUID_COMETLAKE_U_A0 },
	{ X86_VENDOR_INTEL, CPUID_COMETLAKE_U_K0_S0 },
	{ X86_VENDOR_INTEL, CPUID_COMETLAKE_H_S_6_2_G0 },
	{ X86_VENDOR_INTEL, CPUID_COMETLAKE_H_S_6_2_G1 },
	{ X86_VENDOR_INTEL, CPUID_COMETLAKE_H_S_10_2_P0 },
	{ X86_VENDOR_INTEL, CPUID_COMETLAKE_H_S_10_2_Q0_P1 },
	{ X86_VENDOR_INTEL, CPUID_TIGERLAKE_A0 },
	{ X86_VENDOR_INTEL, CPUID_TIGERLAKE_B0 },
	{ X86_VENDOR_INTEL, CPUID_ELKHARTLAKE_A0 },
	{ X86_VENDOR_INTEL, CPUID_ELKHARTLAKE_B0 },
	{ X86_VENDOR_INTEL, CPUID_JASPERLAKE_A0 },
	{ X86_VENDOR_INTEL, CPUID_ALDERLAKE_S_A0 },
	{ X86_VENDOR_INTEL, CPUID_ALDERLAKE_P_A0 },
	{ X86_VENDOR_INTEL, CPUID_ALDERLAKE_M_A0 },
	{ 0, 0 },
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};

/*
 * MP Init callback function to Find CPU Topology. This function is common
 * among all SOCs and thus its in Common CPU block.
 */
int get_cpu_count(void)
{
	unsigned int num_virt_cores, num_phys_cores;

	cpu_read_topology(&num_phys_cores, &num_virt_cores);

	printk(BIOS_DEBUG, "Detected %u core, %u thread CPU.\n",
	       num_phys_cores, num_virt_cores);

	return num_virt_cores;
}

/*
 * MP Init callback function(get_microcode_info) to find the Microcode at
 * Pre MP Init phase. This function is common among all SOCs and thus its in
 * Common CPU block.
 * This function also fills in the microcode patch (in *microcode), and also
 * sets the argument *parallel to 1, which allows microcode loading in all
 * APs to occur in parallel during MP Init.
 */
void get_microcode_info(const void **microcode, int *parallel)
{
	*microcode = intel_microcode_find();
	*parallel = 1;
}

/*
 * Perform BSP and AP initialization
 * This function can be called in below cases:
 * 1. During coreboot is doing MP initialization as part of BS_DEV_INIT_CHIPS (exclude
 *    this call if user has selected USE_INTEL_FSP_MP_INIT).
 * 2. coreboot would like to take APs control back after FSP-S has done with MP
 *    initialization based on user select USE_INTEL_FSP_MP_INIT.
 */
void init_cpus(void)
{
	struct device *dev = dev_find_path(NULL, DEVICE_PATH_CPU_CLUSTER);
	assert(dev != NULL);

	if (dev && dev->link_list)
		soc_init_cpus(dev->link_list);
}

static void coreboot_init_cpus(void *unused)
{
	if (CONFIG(USE_INTEL_FSP_MP_INIT))
		return;

	const void *microcode_patch = intel_microcode_find();
	intel_microcode_load_unlocked(microcode_patch);

	init_cpus();
}

static void wrapper_x86_setup_mtrrs(void *unused)
{
	x86_setup_mtrrs_with_detect();
}

/* Ensure to re-program all MTRRs based on DRAM resource settings */
static void post_cpus_init(void *unused)
{
	if (mp_run_on_all_cpus(&wrapper_x86_setup_mtrrs, NULL) < 0)
		printk(BIOS_ERR, "MTRR programming failure\n");

	x86_mtrr_check();
}

/* Do CPU MP Init before FSP Silicon Init */
BOOT_STATE_INIT_ENTRY(BS_DEV_INIT_CHIPS, BS_ON_ENTRY, coreboot_init_cpus, NULL);
BOOT_STATE_INIT_ENTRY(BS_WRITE_TABLES, BS_ON_EXIT, post_cpus_init, NULL);
BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, post_cpus_init, NULL);
