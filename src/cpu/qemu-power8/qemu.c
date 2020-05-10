/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cpu/cpu.h>
#include <device/device.h>

static void qemu_cpu_init(struct device *dev)
{
}

static struct device_operations cpu_dev_ops = {
	.init = qemu_cpu_init,
};

static const struct cpu_device_id cpu_table[] = {
	{ 0, 0 },
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};

struct chip_operations cpu_power8_qemu_ops = {
	CHIP_NAME("QEMU POWER8 CPU")
};
