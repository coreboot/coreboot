/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <amdblocks/cpu.h>
#include <amdblocks/data_fabric.h>
#include <amdblocks/root_complex.h>
#include <arch/ioapic.h>
#include <arch/vga.h>
#include <console/console.h>
#include <cpu/amd/mtrr.h>
#include <device/device.h>
#include <device/pci_ops.h>
#include <types.h>

void amd_pci_domain_scan_bus(struct device *domain)
{
	uint8_t bus, limit;

	/* TODO: Systems with more than one PCI root need to read the data fabric registers to
	   see which PCI bus numbers get decoded to which PCI root. */
	bus = 0;
	limit = CONFIG_ECAM_MMCONF_BUS_NUMBER - 1;

	/* Set bus first number of PCI root */
	domain->link_list->secondary = bus;
	/* subordinate needs to be the same as secondary before pci_domain_scan_bus call. */
	domain->link_list->subordinate = bus;
	/* Tell allocator about maximum PCI bus number in domain */
	domain->link_list->max_subordinate = limit;

	pci_domain_scan_bus(domain);
}

/* Read the registers and return normalized values */
static void data_fabric_get_mmio_base_size(unsigned int reg,
					   resource_t *mmio_base, resource_t *mmio_limit)
{
	const uint32_t base_reg = data_fabric_broadcast_read32(DF_MMIO_BASE(reg));
	const uint32_t limit_reg = data_fabric_broadcast_read32(DF_MMIO_LIMIT(reg));
	/* The raw register values are bits 47..16  of the actual address */
	*mmio_base = (resource_t)base_reg << DF_MMIO_SHIFT;
	*mmio_limit = (((resource_t)limit_reg + 1) << DF_MMIO_SHIFT) - 1;
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

static void report_data_fabric_mmio(struct device *domain, unsigned int idx,
				    resource_t mmio_base, resource_t mmio_limit)
{
	struct resource *res;
	res = new_resource(domain, idx);
	res->base = mmio_base;
	res->limit = mmio_limit;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED;
}

/* Tell the resource allocator about the usable MMIO ranges configured in the data fabric */
static void add_data_fabric_mmio_regions(struct device *domain, unsigned int *idx)
{
	union df_mmio_control ctrl;
	resource_t mmio_base;
	resource_t mmio_limit;

	/* The last 12GB of the usable address space are reserved and can't be used for MMIO */
	const resource_t reserved_upper_mmio_base =
		(1ULL << get_usable_physical_address_bits()) - DF_RESERVED_TOP_12GB_MMIO_SIZE;

	for (unsigned int i = 0; i < DF_MMIO_REG_SET_COUNT; i++) {
		ctrl.raw = data_fabric_broadcast_read32(DF_MMIO_CONTROL(i));

		/* Relevant MMIO regions need to have both reads and writes enabled */
		if (!ctrl.we || !ctrl.re)
			continue;

		/* Non-posted region contains fixed FCH MMIO devices */
		if (ctrl.np)
			continue;

		/* TODO: Systems with more than one PCI root need to check to which PCI root
		   the MMIO range gets decoded to. */

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

static void report_data_fabric_io(struct device *domain, unsigned int idx,
				  resource_t io_base, resource_t io_limit)
{
	struct resource *res;
	res = new_resource(domain, idx);
	res->base = io_base;
	res->limit = io_limit;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED;
}

/* Tell the resource allocator about the usable I/O space */
static void add_data_fabric_io_regions(struct device *domain, unsigned int *idx)
{
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

		/* TODO: Systems with more than one PCI root need to check to which PCI root
		   the IO range gets decoded to. */

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

void amd_pci_domain_read_resources(struct device *domain)
{
	unsigned int idx = 0;

	add_data_fabric_io_regions(domain, &idx);

	add_data_fabric_mmio_regions(domain, &idx);

	read_non_pci_resources(domain, &idx);
}

static void write_ssdt_domain_io_producer_range_helper(const char *domain_name,
						       resource_t base, resource_t limit)
{
	printk(BIOS_DEBUG, "%s _CRS: adding IO range [%llx-%llx]\n", domain_name, base, limit);
	acpigen_resource_producer_io(base, limit);
}

static void write_ssdt_domain_io_producer_range(const char *domain_name,
						resource_t base, resource_t limit)
{
	/*
	 * Split the IO region at the PCI config IO ports so that the IO resource producer
	 * won't cover the same IO ports that the IO resource consumer for the PCI config IO
	 * ports in the same ACPI device already covers.
	 */
	if (base < PCI_IO_CONFIG_INDEX) {
		write_ssdt_domain_io_producer_range_helper(domain_name,
					base,
					MIN(limit, PCI_IO_CONFIG_INDEX - 1));
	}
	if (limit > PCI_IO_CONFIG_LAST_PORT) {
		write_ssdt_domain_io_producer_range_helper(domain_name,
					MAX(base, PCI_IO_CONFIG_LAST_PORT + 1),
					limit);
	}
}

static void write_ssdt_domain_mmio_producer_range(const char *domain_name,
						  resource_t base, resource_t limit)
{
	printk(BIOS_DEBUG, "%s _CRS: adding MMIO range [%llx-%llx]\n",
	       domain_name, base, limit);
	acpigen_resource_producer_mmio(base, limit,
		MEM_RSRC_FLAG_MEM_READ_WRITE | MEM_RSRC_FLAG_MEM_ATTR_NON_CACHE);
}

void amd_pci_domain_fill_ssdt(const struct device *domain)
{
	const char *acpi_scope = acpi_device_path(domain);
	printk(BIOS_DEBUG, "%s ACPI scope: '%s'\n", __func__, acpi_scope);
	acpigen_write_scope(acpi_device_path(domain));

	acpigen_write_name("_CRS");
	acpigen_write_resourcetemplate_header();

	/* PCI bus number range in domain */
	printk(BIOS_DEBUG, "%s _CRS: adding busses [%x-%x]\n", acpi_device_name(domain),
	       domain->link_list->secondary, domain->link_list->max_subordinate);
	acpigen_resource_producer_bus_number(domain->link_list->secondary,
					     domain->link_list->max_subordinate);

	if (domain->link_list->secondary == 0) {
		/* ACPI 6.4.2.5 I/O Port Descriptor */
		acpigen_write_io16(PCI_IO_CONFIG_INDEX, PCI_IO_CONFIG_LAST_PORT, 1,
				   PCI_IO_CONFIG_PORT_COUNT, 1);
	}

	struct resource *res;
	for (res = domain->resource_list; res != NULL; res = res->next) {
		if (!(res->flags & IORESOURCE_ASSIGNED))
			continue;
		/* Don't add MMIO producer ranges for reserved MMIO regions from non-PCI
		   devices */
		if ((res->flags & IORESOURCE_RESERVE))
			continue;
		switch (res->flags & IORESOURCE_TYPE_MASK) {
		case IORESOURCE_IO:
			write_ssdt_domain_io_producer_range(acpi_device_name(domain),
							    res->base, res->limit);
			break;
		case IORESOURCE_MEM:
			write_ssdt_domain_mmio_producer_range(acpi_device_name(domain),
							      res->base, res->limit);
			break;
		default:
			break;
		}
	}

	if (domain->link_list->bridge_ctrl & PCI_BRIDGE_CTL_VGA) {
		printk(BIOS_DEBUG, "%s _CRS: adding VGA resource\n", acpi_device_name(domain));
		acpigen_resource_producer_mmio(VGA_MMIO_BASE, VGA_MMIO_LIMIT,
			MEM_RSRC_FLAG_MEM_READ_WRITE | MEM_RSRC_FLAG_MEM_ATTR_CACHE);
	}

	acpigen_write_resourcetemplate_footer();

	acpigen_write_BBN(domain->link_list->secondary);

	/* Scope */
	acpigen_pop_len();
}
