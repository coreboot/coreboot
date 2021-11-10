/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <acpi/acpigen.h>
#include <intelblocks/acpi.h>
#include <soc/cpu.h>
#include <smp/spinlock.h>
#include <device/device.h>
#include <device/path.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/mp.h>

DECLARE_SPIN_LOCK(cpu_lock);
static u8 global_cpu_type[CONFIG_MAX_CPUS];

static bool is_big_core(void)
{
	return get_soc_cpu_type() == CPUID_CORE_TYPE_INTEL_CORE;
}

static u32 get_cpu_index(void)
{
	u32 cpu_index = 0;
	struct device *dev;
	u32 my_apic_id = lapicid();

	for (dev = dev_find_lapic(0); dev; dev = dev->next) {
		if (my_apic_id > dev->path.apic.apic_id)
			cpu_index++;
	}

	return cpu_index;
}

/*
 * This function determines the type (big or small) of the CPU that is executing
 * it and stores the information (in a thread-safe manner) in an global_cpu_type
 * array.
 * It requires the SoC to implement a function `get_soc_cpu_type()` which will be
 * called in a critical section to determine the type of the executing CPU.
 */
static void set_cpu_type(void *unused)
{
	spin_lock(&cpu_lock);
	u8 cpu_index = get_cpu_index();

	if (is_big_core())
		global_cpu_type[cpu_index] = 1;

	spin_unlock(&cpu_lock);
}

static void run_set_cpu_type(void *unused)
{
	if (mp_run_on_all_cpus(set_cpu_type, NULL) != CB_SUCCESS) {
		printk(BIOS_ERR, "cpu_hybrid: Failed to set global_cpu_type with CPU type info\n");
		return;
	}
}

BOOT_STATE_INIT_ENTRY(BS_DEV_INIT_CHIPS, BS_ON_EXIT, run_set_cpu_type, NULL);
