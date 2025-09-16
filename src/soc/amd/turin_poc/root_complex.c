/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <acpi/acpi_device.h>
#include <amdblocks/ioapic.h>
#include <amdblocks/smn.h>
#include <amdblocks/root_complex.h>
#include <arch/ioapic.h>
#include <device/device.h>
#include <device/pci.h>
#include <types.h>

#define IOHC_MPDMA_MMIO_IDX	0x20000300

#define MPDMA_C2PMSG_0_OFFSET	0x10900

/*
 * The order of IOHCs here is not random. They are sorted so that:
 * 1. The First IOHC is the one with primary FCH. We want the LPC/SMBUS
 *    devices be on bus 0.
 * 2. The rest of IOHCs are listed in an order so that ECAM MMIO region is one
 *    continuous block for all domains.
 *
 * AGESA/OpenSIL sets up the PCI configuration decoding ranges in line with
 * this.
 */
static const struct domain_iohc_info iohc_info[] = {
	[0] = {
		.fabric_id = 0x24,
		.misc_smn_base = SMN_IOHC_MISC_BASE_13D1,
	},
	[1] = {
		.fabric_id = 0x25,
		.misc_smn_base = SMN_IOHC_MISC_BASE_1D61,
	},
	[2] = {
		.fabric_id = 0x26,
		.misc_smn_base = SMN_IOHC_MISC_BASE_13E1,
	},
	[3] = {
		.fabric_id = 0x27,
		.misc_smn_base = SMN_IOHC_MISC_BASE_1D51,
	},
	[4] = {
		.fabric_id = 0x23,
		.misc_smn_base = SMN_IOHC_MISC_BASE_1D41,
	},
	[5] = {
		.fabric_id = 0x22,
		.misc_smn_base = SMN_IOHC_MISC_BASE_13C1,
	},
	[6] = {
		.fabric_id = 0x21,
		.misc_smn_base = SMN_IOHC_MISC_BASE_1D71,
	},
	[7] = {
		.fabric_id = 0x20,
		.misc_smn_base = SMN_IOHC_MISC_BASE_13B1,
	},
};

const struct domain_iohc_info *get_iohc_info(size_t *count)
{
	*count = ARRAY_SIZE(iohc_info);
	return iohc_info;
}

static const struct non_pci_mmio_reg non_pci_mmio[] = {
	{ 0x2d8, 0xfffffff00000ull,   1 * MiB, NON_PCI_RES_IDX_AUTO },
	{ 0x2e0, 0xfffffff00000ull,   1 * MiB, NON_PCI_RES_IDX_AUTO },
	{ 0x2e8, 0xfffffff00000ull,   1 * MiB, NON_PCI_RES_IDX_AUTO },
	/*
	 * The hardware has a 256 byte alignment requirement for the IOAPIC
	 * MMIO base, but OpenSIL configures 64k-aligned base address and this
	 * is reported as 256 byte resource.
	 */
	{ 0x2f0, 0xffffffffff00ull,	  256, IOMMU_IOAPIC_IDX },
	{ 0x2f8, 0xfffffff00000ull,   1 * MiB, NON_PCI_RES_IDX_AUTO },
	{ 0x300, 0xfffffff00000ull,   1 * MiB, IOHC_MPDMA_MMIO_IDX },
	{ 0x308, 0xfffffffff000ull,   4 * KiB, NON_PCI_RES_IDX_AUTO },
	{ 0x310, 0xfffffff00000ull,   1 * MiB, NON_PCI_RES_IDX_AUTO },
	{ 0x318, 0xfffffff80000ull, 512 * KiB, NON_PCI_RES_IDX_AUTO },
	{ 0x338, 0xfffffff00000ull,   1 * MiB, NON_PCI_RES_IDX_AUTO },
};

const struct non_pci_mmio_reg *get_iohc_non_pci_mmio_regs(size_t *count)
{
	*count = ARRAY_SIZE(non_pci_mmio);
	return non_pci_mmio;
}

#if ENV_RAMSTAGE

static const char *gnb_acpi_name(const struct device *dev)
{
	return "GNB";
}

static void acpigen_write_mpdma_device(const struct device *dev)
{
	struct resource *res;
	uint64_t bar_size;
	uint32_t mpdma_redir_entry;
	struct device *domain = (struct device *)dev_get_domain(dev);
	struct acpi_irq mpdma_irq = ACPI_IRQ_EDGE_HIGH(0);

	if (dev_get_domain_id(dev) == 7) {
		printk(BIOS_DEBUG, "%s.DMA0\n", acpi_device_path(dev_get_domain(dev)));
		acpigen_write_device("DMA0");
		acpigen_write_name_string("_HID", "AMDI0096");
		acpigen_write_name_integer("_UID", 1);
		acpigen_pop_len(); /* Device */
	}

	if (dev_get_domain_id(dev) != 0)
		return;

	printk(BIOS_DEBUG, "%s.TMPM\n", acpi_device_path(dev_get_domain(dev)));
	acpigen_write_device("TMPM");
	acpigen_write_name_string("_HID", "AMDI0095");
	acpigen_write_name_integer("_UID", 0);

	res = probe_resource(dev, IOMMU_IOAPIC_IDX);
	if (!res) {
		acpigen_write_name_integer("_STA", ACPI_STATUS_DEVICE_ALL_OFF);
		acpigen_pop_len(); /* Device */
		return;
	}

	/* Calculate IOAPIC redirection entry offset based on RB index */
	mpdma_redir_entry = ioapic_get_max_vectors((uintptr_t)res->base);
	mpdma_redir_entry *= dev_get_domain_id(domain);
	/* Add offset of FCH IOAPIC redirection entries */
	mpdma_redir_entry += ioapic_get_max_vectors((uintptr_t)IO_APIC_ADDR);
	/* MPDMA has a fixed redirection entry of 28 */
	mpdma_redir_entry += 28;

	mpdma_irq.pin = mpdma_redir_entry;

	res = probe_resource(dev, IOHC_MPDMA_MMIO_IDX);
	if (!res) {
		acpigen_write_name_integer("_STA", ACPI_STATUS_DEVICE_ALL_OFF);
		acpigen_pop_len(); /* Device */
		return;
	}

	if (res->size) {
		bar_size = res->size - MPDMA_C2PMSG_0_OFFSET;
	} else if (res->limit) {
		bar_size = res->limit - res->base - MPDMA_C2PMSG_0_OFFSET + 1;
	} else {
		acpigen_write_name_integer("_STA", ACPI_STATUS_DEVICE_ALL_OFF);
		acpigen_pop_len(); /* Device */
		return;
	}

	acpigen_write_name("_CRS");
	acpigen_write_resourcetemplate_header();
	acpigen_write_mem32fixed(1, res->base + MPDMA_C2PMSG_0_OFFSET, bar_size);
	acpi_device_write_interrupt(&mpdma_irq);
	acpigen_write_resourcetemplate_footer();

	acpigen_write_name_integer("_STA", ACPI_STATUS_DEVICE_ALL_ON);

	acpigen_pop_len(); /* Device */
}

static void gnb_fill_ssdt(const struct device *dev)
{
	const char *acpi_scope = acpi_device_path(dev_get_domain(dev));

	acpigen_write_scope(acpi_scope);

	acpigen_write_mpdma_device(dev);

	acpigen_pop_len(); /* Scope */
}

struct device_operations turin_root_complex_operations = {
	/* The root complex has no PCI BARs implemented, so there's no need to call
	   pci_dev_read_resources for it */
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.acpi_name		= gnb_acpi_name,
	.acpi_fill_ssdt		= gnb_fill_ssdt,
};

#endif /* ENV_RAMSTAGE */
