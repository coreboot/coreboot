/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <acpi/acpi.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/intel/microcode.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/mp.h>
#include <cpu/x86/mtrr.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/mp_init.h>
#include <soc/cpu.h>

static const void *microcode_patch;

void get_microcode_info(const void **microcode, int *parallel)
{
	*microcode = intel_mp_current_microcode();
	*parallel = 1;
}

const void *intel_mp_current_microcode(void)
{
	return microcode_patch;
}

static void each_cpu_init(struct device *cpu)
{
	printk(BIOS_SPEW, "%s dev: %s, cpu: %d, apic_id: 0x%x\n",
		__func__, dev_path(cpu), cpu_index(), cpu->path.apic.apic_id);

	setup_lapic();
}

static struct device_operations cpu_dev_ops = {
	.init = each_cpu_init,
};

static const struct cpu_device_id cpu_table[] = {
	{X86_VENDOR_INTEL, CPUID_COOPERLAKE_SP_A0},
	{0, 0},
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

static int get_thread_count(void)
{
	unsigned int num_phys = 0, num_virts = 0;

	cpu_read_topology(&num_phys, &num_virts);
	printk(BIOS_SPEW, "Detected %u cores and %u threads\n", num_phys, num_virts);
	/*
	 * Currently we do not know a way to figure out how many CPUs we have total
	 * on multi-socketed. So we pretend all sockets are populated with CPUs with
	 * same thread/core fusing.
	 * TODO: properly figure out number of active sockets OR refactor MPinit code
	 * to remove requirements of having to know total number of CPUs in advance.
	 */
	return num_virts * CONFIG_MAX_SOCKET;
}

static const struct mp_ops mp_ops = {
	.pre_mp_init = pre_mp_init,
	.get_cpu_count = get_thread_count,
	.get_microcode_info = get_microcode_info
};

void cpx_init_cpus(struct device *dev)
{
	microcode_patch = intel_microcode_find();

	if (!microcode_patch)
		printk(BIOS_ERR, "microcode not found in CBFS!\n");

	intel_microcode_load_unlocked(microcode_patch);

	if (mp_init_with_smm(dev->link_list, &mp_ops) < 0)
		printk(BIOS_ERR, "MP initialization failure.\n");
}
