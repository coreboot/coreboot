/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/data_fabric.h>
#include <amdblocks/root_complex.h>
#include <arch/ioapic.h>
#include <console/console.h>
#include <cpu/amd/mtrr.h>
#include <cpu/cpu.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <types.h>

void amd_pci_domain_scan_bus(struct device *domain)
{
	uint8_t segment_group, bus, limit;

	if (data_fabric_get_pci_bus_numbers(domain, &segment_group, &bus, &limit) != CB_SUCCESS) {
		printk(BIOS_ERR, "No PCI bus numbers decoded to PCI root.\n");
		return;
	}

	if (segment_group >= PCI_SEGMENT_GROUP_COUNT) {
		printk(BIOS_ERR, "Skipping domain %u due to too large segment group %u.\n",
		       dev_get_domain_id(domain), segment_group);
		return;
	}

	/* TODO: Check if bus >= PCI_BUSES_PER_SEGMENT_GROUP and return in that case */

	/* Make sure to not report more than PCI_BUSES_PER_SEGMENT_GROUP PCI buses */
	limit = MIN(limit, PCI_BUSES_PER_SEGMENT_GROUP - 1);

	/* Set bus first number of PCI root */
	domain->downstream->secondary = bus;
	/* subordinate needs to be the same as secondary before pci_host_bridge_scan_bus call. */
	domain->downstream->subordinate = bus;
	/* Tell allocator about maximum PCI bus number in domain */
	domain->downstream->max_subordinate = limit;
	domain->downstream->segment_group = segment_group;

	pci_host_bridge_scan_bus(domain);
}

static void print_df_mmio_outside_of_cpu_mmio_error(unsigned int reg)
{
	printk(BIOS_WARNING, "DF MMIO register %u outside of CPU MMIO region.\n", reg);
}

static bool is_mmio_region_valid(unsigned int reg, resource_t mmio_base, resource_t mmio_limit)
{
	if (mmio_base > mmio_limit) {
		printk(BIOS_WARNING, "DF MMIO register %u's base is above its limit.\n", reg);
		return false;
	}
	if (mmio_base >= 4ULL * GiB) {
		/* MMIO region above 4GB needs to be above TOP_MEM2 MSR value */
		if (mmio_base < get_top_of_mem_above_4gb()) {
			print_df_mmio_outside_of_cpu_mmio_error(reg);
			return false;
		}
	} else {
		/* MMIO region below 4GB needs to be above TOP_MEM MSR value */
		if (mmio_base < get_top_of_mem_below_4gb()) {
			print_df_mmio_outside_of_cpu_mmio_error(reg);
			return false;
		}
		/* MMIO region below 4GB mustn't cross the 4GB boundary. */
		if (mmio_limit >= 4ULL * GiB) {
			printk(BIOS_WARNING, "DF MMIO register %u crosses 4GB boundary.\n",
			       reg);
			return false;
		}
	}

	return true;
}

static void report_data_fabric_mmio(struct device *domain, unsigned long idx,
				    resource_t mmio_base, resource_t mmio_limit)
{
	struct resource *res;
	res = new_resource(domain, idx);
	res->base = mmio_base;
	res->limit = mmio_limit;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED;
}

/* Tell the resource allocator about the usable MMIO ranges configured in the data fabric */
static void add_data_fabric_mmio_regions(struct device *domain, unsigned long *idx)
{
	const signed int iohc_dest_fabric_id = get_iohc_fabric_id(domain);
	union df_mmio_control ctrl;
	resource_t mmio_base;
	resource_t mmio_limit;

	/* The last 12GB of the usable address space are reserved and can't be used for MMIO */
	const resource_t reserved_upper_mmio_base =
		(1ULL << cpu_phys_address_size()) - DF_RESERVED_TOP_12GB_MMIO_SIZE;

	for (unsigned int i = 0; i < DF_MMIO_REG_SET_COUNT; i++) {
		ctrl.raw = data_fabric_broadcast_read32(DF_MMIO_CONTROL(i));

		/* Relevant MMIO regions need to have both reads and writes enabled */
		if (!ctrl.we || !ctrl.re)
			continue;

		/* Non-posted region contains fixed FCH MMIO devices */
		if (ctrl.np)
			continue;

		/* Only look at MMIO regions that are decoded to the right PCI root */
		if (ctrl.dst_fabric_id != iohc_dest_fabric_id)
			continue;

		data_fabric_get_mmio_base_size(i, &mmio_base, &mmio_limit);

		if (!is_mmio_region_valid(i, mmio_base, mmio_limit))
			continue;

		/* Make sure to not report a region overlapping with the fixed MMIO resources
		   below 4GB or the reserved MMIO range in the last 12GB of the addressable
		   address range. The code assumes that the fixed MMIO resources below 4GB
		   are between IO_APIC_ADDR and the 4GB boundary. */
		if (mmio_base < 4ULL * GiB) {
			if (mmio_base >= IO_APIC_ADDR)
				continue;
			if (mmio_limit >= IO_APIC_ADDR)
				mmio_limit = IO_APIC_ADDR - 1;
		} else {
			if (mmio_base >= reserved_upper_mmio_base)
				continue;
			if (mmio_limit >= reserved_upper_mmio_base)
				mmio_limit = reserved_upper_mmio_base - 1;
		}

		report_data_fabric_mmio(domain, (*idx)++, mmio_base, mmio_limit);
	}
}

static void report_data_fabric_io(struct device *domain, unsigned long idx,
				  resource_t io_base, resource_t io_limit)
{
	struct resource *res;
	res = new_resource(domain, idx);
	res->base = io_base;
	res->limit = io_limit;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED;
}

/* Tell the resource allocator about the usable I/O space */
static void add_data_fabric_io_regions(struct device *domain, unsigned long *idx)
{
	const signed int iohc_dest_fabric_id = get_iohc_fabric_id(domain);
	union df_io_base base_reg;
	union df_io_limit limit_reg;
	resource_t io_base;
	resource_t io_limit;

	for (unsigned int i = 0; i < DF_IO_REG_COUNT; i++) {
		base_reg.raw = data_fabric_broadcast_read32(DF_IO_BASE(i));

		/* Relevant IO regions need to have both reads and writes enabled */
		if (!base_reg.we || !base_reg.re)
			continue;

		limit_reg.raw = data_fabric_broadcast_read32(DF_IO_LIMIT(i));

		/* Only look at IO regions that are decoded to the right PCI root */
		if (limit_reg.dst_fabric_id != iohc_dest_fabric_id)
			continue;

		io_base = base_reg.io_base << DF_IO_ADDR_SHIFT;
		io_limit = ((limit_reg.io_limit + 1) << DF_IO_ADDR_SHIFT) - 1;

		/* Beware that the lower 25 bits of io_base and io_limit can be non-zero
		   despite there only being 16 bits worth of IO port address space. */
		if (io_base > 0xffff) {
			printk(BIOS_WARNING, "DF IO base register %d value outside of valid "
					     "IO port address range.\n", i);
			continue;
		}
		/* If only the IO limit is outside of the valid 16 bit IO port range, report
		   the limit as 0xffff, so that the resource allcator won't put IO BARs outside
		   of the 16 bit IO port address range. */
		io_limit = MIN(io_limit, 0xffff);

		report_data_fabric_io(domain, (*idx)++, io_base, io_limit);
	}
}

static void add_pci_cfg_resources(struct device *domain, unsigned long *idx)
{
	fixed_io_range_reserved(domain, (*idx)++, PCI_IO_CONFIG_INDEX, PCI_IO_CONFIG_PORT_COUNT);
	mmconf_resource(domain, (*idx)++);
}

void amd_pci_domain_read_resources(struct device *domain)
{
	unsigned long idx = 0;

	add_data_fabric_io_regions(domain, &idx);

	add_data_fabric_mmio_regions(domain, &idx);

	read_non_pci_resources(domain, &idx);

	/* Only add the SoC's DRAM memory map and fixed resources once */
	if (is_domain0(domain)) {
		add_pci_cfg_resources(domain, &idx);

		read_soc_memmap_resources(domain, &idx);
	}
}
