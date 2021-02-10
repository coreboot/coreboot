/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/cpu.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/x86/lapic.h>
#include <device/device.h>
#include <soc/cpu.h>

void mp_init_cpus(struct bus *cpu_bus)
{
}

static void zen_2_3_init(struct device *dev)
{
	setup_lapic();
	set_cstate_io_addr();
}

static struct device_operations cpu_dev_ops = {
	.init = zen_2_3_init,
};

static struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_AMD, CEZANNE_A0_CPUID},
	{ 0, 0 },
};

static const struct cpu_driver zen_2_3 __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};
