/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cpu/cpu.h>
#include <device/device.h>

static void power9_cpu_init(struct device *dev)
{
}

static struct device_operations cpu_dev_ops = {
	.init = power9_cpu_init,
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
};

struct chip_operations cpu_power8_qemu_ops = {
	CHIP_NAME("POWER9 CPU")
};
