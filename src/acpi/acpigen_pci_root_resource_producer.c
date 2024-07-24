/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <acpi/acpigen_pci.h>
#include <arch/pci_io_cfg.h>
#include <arch/vga.h>
#include <console/console.h>
#include <device/device.h>
#include <types.h>

static void write_ssdt_domain_io_producer_range_helper(const char *domain_name,
						       resource_t base, resource_t limit)
{
	printk(BIOS_DEBUG, "%s _CRS: adding IO range [%04llx-%04llx]\n", domain_name, base, limit);
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
	printk(BIOS_DEBUG, "%s _CRS: adding MMIO range [%016llx-%016llx]\n",
	       domain_name, base, limit);
	acpigen_resource_producer_mmio(base, limit,
		MEM_RSRC_FLAG_MEM_READ_WRITE | MEM_RSRC_FLAG_MEM_ATTR_NON_CACHE);
}

void pci_domain_fill_ssdt(const struct device *domain)
{
	const char *acpi_scope = acpi_device_path(domain);
	printk(BIOS_DEBUG, "%s ACPI scope: '%s'\n", __func__, acpi_scope);
	acpigen_write_scope(acpi_device_path(domain));

	acpigen_write_name("_CRS");
	acpigen_write_resourcetemplate_header();

	/* PCI bus number range in domain */
	printk(BIOS_DEBUG, "%s _CRS: adding busses [%02x-%02x] in segment group %x\n",
	       acpi_device_name(domain), domain->downstream->secondary,
	       domain->downstream->max_subordinate, domain->downstream->segment_group);
	acpigen_resource_producer_bus_number(domain->downstream->secondary,
					     domain->downstream->max_subordinate);

	if (is_domain0(domain)) {
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
		if (res->flags & IORESOURCE_RESERVE)
			continue;
		/* Don't add MMIO producer ranges for DRAM regions */
		if (res->flags & IORESOURCE_STORED)
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

	if (domain->downstream->bridge_ctrl & PCI_BRIDGE_CTL_VGA) {
		printk(BIOS_DEBUG, "%s _CRS: adding VGA resource\n", acpi_device_name(domain));
		acpigen_resource_producer_mmio(VGA_MMIO_BASE, VGA_MMIO_LIMIT,
			MEM_RSRC_FLAG_MEM_READ_WRITE | MEM_RSRC_FLAG_MEM_ATTR_CACHE);
	}

	acpigen_write_resourcetemplate_footer();

	acpigen_write_SEG(domain->downstream->segment_group);
	acpigen_write_BBN(domain->downstream->secondary);
	/* Scope */
	acpigen_pop_len();
}
