/* SPDX-License-Identifier: GPL-2.0-or-later */

#include "chip.h"
#include <acpi/acpigen.h>
#include <arch/mmu.h>
#include <bootmem.h>
#include <cbfs.h>
#include <device/device.h>
#include <commonlib/device_tree.h>
#include <mainboard/addressmap.h>
#include <stdint.h>
#include <symbols.h>

static size_t ram_size(void)
{
	return (size_t)cbmem_top() - (uintptr_t)_dram;
}

static void mainboard_init(void *chip_info)
{
	mmu_config_range(_dram, ram_size(), MA_MEM | MA_RW);
}

void smbios_cpu_get_core_counts(u16 *core_count, u16 *thread_count)
{
	*core_count = 0;
	struct device *dev = NULL;
	while ((dev = dev_find_path(dev, DEVICE_PATH_GICC_V3)))
		*core_count += 1;

	*thread_count = 1;
}

static void qemu_aarch64_init(struct device *dev)
{
	struct memory_info *mem_info;

	mem_info = cbmem_add(CBMEM_ID_MEMINFO, sizeof(*mem_info));
	if (mem_info == NULL)
		return;

	memset(mem_info, 0, sizeof(*mem_info));

	mem_info->ecc_type = MEMORY_ARRAY_ECC_UNKNOWN;
	mem_info->max_capacity_mib = 0x800000;	// Fixed at 8 TiB for qemu-sbsa
	mem_info->number_of_devices = mem_info->dimm_cnt = 1;

	mem_info->dimm[0].dimm_size = ram_size() / MiB;
	mem_info->dimm[0].ddr_type = MEMORY_TYPE_DRAM;
	mem_info->dimm[0].ddr_frequency = 0;
	mem_info->dimm[0].channel_num = mem_info->dimm[0].dimm_num = 0;
	mem_info->dimm[0].bank_locator = 0;

	mem_info->dimm[0].bus_width = 0x03;	// 64-bit, no parity
	mem_info->dimm[0].vdd_voltage = 0;
	mem_info->dimm[0].max_speed_mts = mem_info->dimm[0].configured_speed_mts = 0;
}

static unsigned long mb_write_acpi_tables(const struct device *dev, unsigned long current,
					  acpi_rsdp_t *rsdp)
{
	printk(BIOS_DEBUG, "ACPI:    * DBG2\n");
	return acpi_pl011_write_dbg2_uart(rsdp, current, SBSA_UART_BASE, "\\_SB.COM0");
}


static void mainboard_enable(struct device *dev)
{
	dev->ops->init = qemu_aarch64_init;
	dev->ops->write_acpi_tables = mb_write_acpi_tables;
}


struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
	.init = mainboard_init,
};

struct chip_operations mainboard_emulation_qemu_sbsa_ops = { };

static void qemu_aarch64_domain_read_resources(struct device *dev)
{
	struct resource *res;
	int index = 0;

	/* Initialize the system-wide I/O space constraints. */
	res = new_resource(dev, index++);
	res->limit = 0xffffUL;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED;

	/* Initialize the system-wide memory resources constraints. */
	res = new_resource(dev, index++);
	res->base = SBSA_PCIE_MMIO_BASE;
	res->limit = SBSA_PCIE_MMIO_LIMIT;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED;

	res = new_resource(dev, index++);
	res->base = SBSA_PCIE_MMIO_HIGH_BASE;
	res->limit = SBSA_PCIE_MMIO_HIGH_LIMIT;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED;

	mmio_range(dev, index++, SBSA_PCIE_ECAM_BASE, SBSA_PCIE_ECAM_SIZE);

	ram_range(dev, index++, (uintptr_t)_dram, ram_size());

	mmio_range(dev, index++, SBSA_FLASH_BASE, SBSA_FLASH_SIZE);
	reserved_ram_range(dev, index++, SBSA_SECMEM_BASE, SBSA_SECMEM_SIZE);
}

struct device_operations qemu_aarch64_pci_domain_ops = {
	.read_resources    = qemu_aarch64_domain_read_resources,
	.set_resources     = pci_domain_set_resources,
	.scan_bus          = pci_host_bridge_scan_bus,
};

static void qemu_sbsa_fill_cpu_ssdt(const struct device *dev)
{
	acpigen_write_processor_device(dev->path.gicc_v3.mpidr);
	acpigen_write_processor_device_end();
}

struct device_operations qemu_sbsa_cpu_ops = {
	.acpi_fill_ssdt = qemu_sbsa_fill_cpu_ssdt,
};

DECLARE_REGION(fdt_pointer)
static void qemu_aarch64_scan_bus(struct device *dev)
{
	struct bus *bus = alloc_bus(dev);
	uintptr_t fdt_blob = *(uintptr_t *)_fdt_pointer;
	struct device_tree *tree;
	struct device_tree_node *node;
	char path[14];
	u16 fdt_cpu_count = 0;
	struct mainboard_emulation_qemu_sbsa_config *config = dev->chip_info;

	tree = fdt_unflatten((void *)fdt_blob);
	if (tree == NULL)
		return;

	snprintf(path, sizeof(path), "/cpus/cpu@%d", fdt_cpu_count);
	while ((node = dt_find_node_by_path(tree, path, NULL, NULL, 0)) != NULL) {
		struct device_tree_property *prop;
		int64_t mpidr = -1;
		list_for_each(prop, node->properties, list_node) {
			if (!strcmp("reg", prop->prop.name)) {
				mpidr = be64toh(*(uint64_t *)prop->prop.data);
				break;
			}
		}
		if (mpidr >= 0) {
			struct device_path devpath = { .type = DEVICE_PATH_GICC_V3,
				.gicc_v3 = { .mpidr = mpidr,
					     .vgic_mi = config->vgic_maintenance_interrupt,
					     .pi_gsiv = config->performance_interrupt_gsiv, },

			};
			struct device *cpu = alloc_dev(bus, &devpath);
			assert(cpu);
			cpu->ops = &qemu_sbsa_cpu_ops;
		}
		snprintf(path, sizeof(path), "/cpus/cpu@%d", ++fdt_cpu_count);
	}
}

struct device_operations qemu_aarch64_cpu_ops = {
	.scan_bus = qemu_aarch64_scan_bus,
};
