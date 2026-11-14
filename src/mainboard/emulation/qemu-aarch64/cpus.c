/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <symbols.h>
#include <device/device.h>
#include <bootmem.h>
#include <fit.h>

DECLARE_REGION(fdt);

static void qemu_aarch64_scan_bus(struct device *dev)
{
	struct bus *bus = alloc_bus(dev);
	struct device_tree_node *node;
	struct device_tree *tree;

	tree = fdt_unflatten((void *)_fdt);
	if (tree == NULL)
		return;

	bool has_gicv3 = dt_find_next_compat_child(tree->root, NULL, "arm,gic-v3") != NULL;
	bool has_gicv2 = dt_find_next_compat_child(tree->root, NULL, "arm,cortex-a15-gic") != NULL;

	printk(BIOS_INFO, "GICv2: %s, GICv3: %s\n", has_gicv2 ? "present" : "absent",
	       has_gicv3 ? "present" : "absent");

	/**
	 * Ensure that the platform has a GICv2 controller. The BL31 firmware is
	 * currently always compiled with GICv2 support.
	 * TODO: Add multiple BL31 firmware configurations for different GIC versions.
	 */
	if (!has_gicv2)
		die("No GICv2 found. Unsupported platform");

	const size_t max_cpus = has_gicv2 ? MIN(8, CONFIG_MAX_CPUS) :
				MIN(512, CONFIG_MAX_CPUS);
	for (int i = 0; i < max_cpus; i++) {
		char path[14];
		snprintf(path, sizeof(path), "/cpus/cpu@%u", i);
		if (!(node = dt_find_node_by_path(tree, path, NULL, NULL, 0)))
			break;
		void *data = NULL;
		size_t size = 0;
		uint64_t mpidr = 0;

		dt_find_bin_prop(node, "reg", (const void **)&data, &size);

		if (!data || (size != sizeof(uint64_t) && size != sizeof(uint32_t)))
			continue;

		if (size == sizeof(uint64_t))
			mpidr = be64toh(*(uint64_t *)data);
		else
			mpidr = be32toh(*(uint32_t *)data);

		struct device_path devpath = {.type = DEVICE_PATH_CPU, .cpu.id = mpidr};
		struct device *cpu = alloc_dev(bus, &devpath);
		assert(cpu);
		cpu->name = dt_find_string_prop(node, "compatible");
	}
}

struct device_operations qemu_aarch64_cpu_ops = {
	.scan_bus = qemu_aarch64_scan_bus,
};
