/* SPDX-License-Identifier: GPL-2.0-or-later */

#include "chip.h"
#include <acpi/acpigen.h>
#include <arch/mmu.h>
#include <bootmem.h>
#include <cbfs.h>
#include <device/device.h>
#include <mainboard/addressmap.h>
#include <ramdetect.h>
#include <stdint.h>
#include <symbols.h>

static size_t ram_size(void)
{
	return (size_t)cbmem_top() - (uintptr_t)_dram;
}

static size_t ram2_size(void)
{
	return (size_t)probe_ramsize((uintptr_t)RDN2_DRAM2_BASE, RDN2_DRAM2_SIZE) * MiB;
}

static void mainboard_init(void *chip_info)
{
	mmu_config_range(_dram, ram_size(), MA_MEM | MA_RW);
	mmu_config_range((void *)RDN2_DRAM2_BASE, ram2_size(), MA_MEM | MA_RW);
}

DECLARE_REGION(fdt_pointer)

void smbios_cpu_get_core_counts(u16 *core_count, u16 *thread_count)
{
	*core_count = 0;
	struct device *dev = NULL;
	while ((dev = dev_find_path(dev, DEVICE_PATH_GICC_V3)))
		*core_count += 1;

	*thread_count = 1;
}


static void rdn2_aarch64_init(struct device *dev)
{
	struct memory_info *mem_info;

	mem_info = cbmem_add(CBMEM_ID_MEMINFO, sizeof(*mem_info));
	if (mem_info == NULL)
		return;

	memset(mem_info, 0, sizeof(*mem_info));

	mem_info->ecc_type = MEMORY_ARRAY_ECC_UNKNOWN;
	mem_info->max_capacity_mib = 0x800000;
	mem_info->number_of_devices = mem_info->dimm_cnt = 1;

	mem_info->dimm[0].dimm_size = (ram_size() + ram2_size()) / MiB;
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
	return acpi_pl011_write_dbg2_uart(rsdp, current, RDN2_UART_NS_BASE, "\\_SB.COM0");
}


static void mainboard_enable(struct device *dev)
{
	dev->ops->init = rdn2_aarch64_init;
	dev->ops->write_acpi_tables = mb_write_acpi_tables;
}


struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.enable_dev = mainboard_enable,
};

struct chip_operations mainboard_arm_rdn2_ops = { };

static void rdn2_aarch64_domain_read_resources(struct device *dev)
{
	struct resource *res;
	int index = 0;

	/* Initialize the system-wide I/O space constraints. */
	res = new_resource(dev, index++);
	res->limit = 0xffffUL;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED;

	/* Initialize the system-wide memory resources constraints. */
	res = new_resource(dev, index++);
	res->base = RDN2_PCIE_MMIO_BASE;
	res->limit = RDN2_PCIE_MMIO_LIMIT;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED;

	res = new_resource(dev, index++);
	res->base = RDN2_PCIE_MMIO_HIGH_BASE;
	res->limit = RDN2_PCIE_MMIO_HIGH_LIMIT;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED;

	ram_range(dev, index++, (uintptr_t)_dram, ram_size());
	reserved_ram_range(dev, index++, (uintptr_t)RDN2_SECMEM_BASE, RDN2_SECMEM_SIZE);

	ram_range(dev, index++, (uintptr_t)RDN2_DRAM2_BASE, ram2_size());

	mmio_range(dev, index++, RDN2_FLASH_BASE, RDN2_FLASH_SIZE);
}

struct device_operations rdn2_aarch64_pci_domain_ops = {
	.read_resources    = rdn2_aarch64_domain_read_resources,
	.set_resources     = pci_domain_set_resources,
	.scan_bus          = pci_host_bridge_scan_bus,
};

static void rdn2_fill_cpu_ssdt(const struct device *dev)
{
	acpigen_write_processor_device(dev->path.gicc_v3.mpidr);
	acpigen_write_processor_device_end();
}

struct device_operations rdn2_cpu_ops = {
	.acpi_fill_ssdt = rdn2_fill_cpu_ssdt,
};

static void rdn2_aarch64_scan_bus(struct device *dev)
{
	u16 i = 0;
	struct bus *bus = alloc_bus(dev);

	for (i = 0; i < 16; i++) {
		printk(BIOS_DEBUG, "Allocating CPU %d\n", i);
		struct device_path devpath = { .type = DEVICE_PATH_GICC_V3,
			.gicc_v3 = { .mpidr = i << 16,
				     .vgic_mi = 0x19,
				     .pi_gsiv = 0x17, },
		};
		struct device *cpu = alloc_dev(bus, &devpath);
		assert(cpu);
		cpu->ops = &rdn2_cpu_ops;
	}

}

struct device_operations rdn2_aarch64_cpu_ops = {
	.scan_bus = rdn2_aarch64_scan_bus,
};
