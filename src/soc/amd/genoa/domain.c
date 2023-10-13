/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/data_fabric.h>
#include <amdblocks/root_complex.h>
#include <amdblocks/smn.h>
#include <arch/ioapic.h>
#include <console/console.h>
#include <device/device.h>
#include <types.h>

#define IOHC_IOAPIC_BASE_ADDR_LO 0x2f0

static void genoa_domain_set_resources(struct device *domain)
{
	if (domain->link_list->bridge_ctrl & PCI_BRIDGE_CTL_VGA) {
		printk(BIOS_DEBUG, "Setting VGA decoding for domain 0x%x\n",
		       domain->path.domain.domain);
		const union df_vga_en vga_en = {
			.ve = 1,
			.dst_fabric_id = get_iohc_fabric_id(domain),
		};
		data_fabric_broadcast_write32(DF_VGA_EN, vga_en.raw);
	}

	pci_domain_set_resources(domain);

	/* Enable IOAPIC memory decoding */
	struct resource *res = probe_resource(domain, IOMMU_IOAPIC_IDX);
	if (res) {
		const uint32_t iohc_misc_base = get_iohc_misc_smn_base(domain);
		uint32_t ioapic_base = smn_read32(iohc_misc_base | IOHC_IOAPIC_BASE_ADDR_LO);
		ioapic_base |= (1 << 0);
		smn_write32(iohc_misc_base | IOHC_IOAPIC_BASE_ADDR_LO, ioapic_base);
	}
}

struct device_operations genoa_pci_domain_ops = {
	.read_resources	  = amd_pci_domain_read_resources,
	.set_resources	  = genoa_domain_set_resources,
	.scan_bus	  = amd_pci_domain_scan_bus,
};
