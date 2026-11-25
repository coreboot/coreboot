/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cpu/cpu.h>
#include <device/device.h>
#include <cpu/x86/lapic.h>

static void qemu_cpu_init(struct device *dev)
{
}

static void qemu_cpu_final(struct device *dev)
{
	/* QEMU doesn't support the IOAPIC route. lpc_init() enables it unconditionally. */
	uint32_t mask = LAPIC_LVT_MASKED | LAPIC_LVT_LEVEL_TRIGGER | LAPIC_INPUT_POLARITY |
			LAPIC_DELIVERY_MODE_MASK;
	/*
	 * Unmask LINT0 so the legacy PIC can drive virtual-wire ExtINT
	 * until/unless GSI0 takes over.
	 */
	lapic_update32(LAPIC_LVT0, ~mask, LAPIC_DELIVERY_MODE_EXTINT);
}

static struct device_operations cpu_dev_ops = {
	.init = qemu_cpu_init,
	.final = qemu_cpu_final,
};

static const struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_ANY, 0, 0 },
	CPU_TABLE_END
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};

struct chip_operations cpu_qemu_x86_ops = {
	.name = "QEMU x86 CPU",
};
