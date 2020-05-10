/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cpu/cpu.h>
#include <device/device.h>
#include <cpu/x86/lapic.h>

static void qemu_cpu_init(struct device *dev)
{
	setup_lapic();
}

static struct device_operations cpu_dev_ops = {
	.init = qemu_cpu_init,
};

static const struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_ANY, 0 },
	{ 0, 0 },
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};

struct chip_operations cpu_qemu_x86_ops = {
	CHIP_NAME("QEMU x86 CPU")
};
