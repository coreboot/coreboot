/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cbmem.h>
#include <symbols.h>
#include <device/device.h>
#include <device/fw_cfg.h>
#include <bootmem.h>
#include <mainboard/addressmap.h>
#include <fit.h>

void bootmem_platform_add_ranges(void)
{
	bootmem_add_range((uintptr_t)_bl31, REGION_SIZE(bl31), BM_MEM_BL31);
}

/* Do additional device_tree modifications. */
static int dt_platform_fixup(struct device_tree_fixup *fixup,
			     struct device_tree *tree)
{
	struct device_tree_node *dt_node;
	u32 addr_cells = 1, size_cells = 1;
	char path[16];

	/* Add PSCI node */
	dt_node = dt_find_node_by_path(tree, "/psci", &addr_cells, &size_cells, 1);
	dt_add_string_prop(dt_node, "compatible", "arm,psci-1.0");
	dt_add_string_prop(dt_node, "method", "smc");

	/* Update or add CPU nodes in the device tree */
	struct device *dev = NULL;
	while ((dev = dev_find_path(dev, DEVICE_PATH_CPU)) != NULL) {
		snprintf(path, sizeof(path), "/cpus/cpu@%u", dev->path.cpu.id);
		dt_node = dt_find_node_by_path(tree, path, &addr_cells, &size_cells, 1);
		assert(dt_node);
		dt_add_string_prop(dt_node, "enable-method", "psci");
		dt_add_string_prop(dt_node, "device_type", "cpu");
		dt_add_string_prop(dt_node, "compatible", dev->name);
		dt_add_u32_prop(dt_node, "reg", dev->path.cpu.id);
	}

	return 0;
}

static struct device_tree_fixup dt_fixup = {
	.fixup = dt_platform_fixup,
};

static void mainboard_enable(struct device *dev)
{
	if (CONFIG(PAYLOAD_FIT_SUPPORT))
		list_insert_after(&dt_fixup.list_node, &device_tree_fixups);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};

struct chip_operations mainboard_emulation_qemu_aarch64_ops = { };

static void qemu_aarch64_domain_read_resources(struct device *dev)
{
	struct resource *res;
	int index = 0;
	/* Initialize the system-wide I/O space constraints. */
	res = new_resource(dev, index++);
	res->limit = 0xffff;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED;

	/* Initialize the system-wide memory resources constraints. */
	res = new_resource(dev, index++);
	res->base = VIRT_PCIE_LOW_MMIO_BASE;
	res->limit = VIRT_PCIE_LOW_MMIO_LIMIT;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED;

	res = new_resource(dev, index++);
	res->base = VIRT_PCIE_HIGH_MMIO_BASE;
	res->limit = VIRT_PCIE_HIGH_MMIO_LIMIT;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED;

	mmio_range(dev, index++, VIRT_PCIE_HIGH_ECAM_BASE, VIRT_PCIE_ECAM_SIZE);

	ram_from_to(dev, index++, (uintptr_t)_dram, cbmem_top());
}

struct device_operations qemu_aarch64_pci_domain_ops = {
	.read_resources    = qemu_aarch64_domain_read_resources,
	.set_resources     = pci_domain_set_resources,
	.scan_bus          = pci_host_bridge_scan_bus,
};
