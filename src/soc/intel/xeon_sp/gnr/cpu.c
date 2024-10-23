/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/intel/common/common.h>
#include <cpu/intel/microcode.h>
#include <cpu/intel/smm_reloc.h>
#include <cpu/x86/mp.h>
#include <cpu/x86/mtrr.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/mp_init.h>
#include <soc/smmrelocate.h>
#include <soc/soc_util.h>
#include <soc/util.h>

#include "chip.h"

static const void *microcode_patch;

static const config_t *chip_config = NULL;

bool cpu_soc_is_in_untrusted_mode(void)
{
	// FIXME: not implemented yet
	return false;
}

void get_microcode_info(const void **microcode, int *parallel)
{
	*microcode = intel_microcode_find();
	*parallel = 0;
}

static void each_cpu_init(struct device *cpu)
{
	printk(BIOS_SPEW, "%s dev: %s, cpu: %lu, apic_id: 0x%x\n",
		__func__, dev_path(cpu), cpu_index(), cpu->path.apic.apic_id);

	/* Only lock and let vmx enabled by FSP to avoid FSP always triggering power good reset
	   due to vmx configuration conflict */
	set_feature_ctrl_lock();
}

static struct device_operations cpu_dev_ops = {
	.init = each_cpu_init,
};

static const struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, CPUID_GRANITERAPIDS, CPUID_ALL_STEPPINGS_MASK },
	{ X86_VENDOR_INTEL, CPUID_SIERRAFOREST, CPUID_ALL_STEPPINGS_MASK },
	CPU_TABLE_END
};

static const struct cpu_driver driver __cpu_driver = {
	.ops = &cpu_dev_ops,
	.id_table = cpu_table,
};

/*
 * Do essential initialization tasks before APs can be fired up
 */
static void pre_mp_init(void)
{
	x86_setup_mtrrs_with_detect();
	x86_mtrr_check();
}

static void post_mp_init(void)
{
	if (CONFIG(HAVE_SMI_HANDLER))
		global_smi_enable();
}

static const struct mp_ops mp_ops = {
	.pre_mp_init = pre_mp_init,
	.get_cpu_count = get_platform_thread_count,
#if CONFIG(HAVE_SMI_HANDLER)
	.get_smm_info = get_smm_info,
	.pre_mp_smm_init = smm_southbridge_clear_state,
	.relocation_handler = smm_relocation_handler,
#endif
	.get_microcode_info = get_microcode_info,
	.post_mp_init = post_mp_init,
};

void mp_init_cpus(struct bus *bus)
{
	/*
	 * chip_config is used in CPU device callback. Other than CPU 0,
	 * rest of the CPU devices do not have chip_info updated.
	 */
	chip_config = bus->dev->chip_info;

	microcode_patch = intel_microcode_find();
	intel_microcode_load_unlocked(microcode_patch);

	enum cb_err ret = mp_init_with_smm(bus, &mp_ops);
	if (ret != CB_SUCCESS)
		printk(BIOS_ERR, "MP initialization failure %d.\n", ret);
}
