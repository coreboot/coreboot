/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/hpet.h>
#include <arch/ioapic.h>
#include <commonlib/bsd/helpers.h>
#include <cpu/x86/lapic_def.h>
#include <device/pci_ops.h>
#include <intelblocks/systemagent_server.h>
#include <soc/iomap.h>
#include <soc/systemagent.h>
#include <security/intel/txt/txt_register.h>

static bool get_vtd_bar(struct device *dev, uint64_t *base, uint64_t *size)
{
	struct sa_server_mem_map_descriptor vtd_bar_descriptor = {
		.reg_offset = VTBAR,
		.is_64_bit = true,
		.alignment = 4 * KiB, /* Bit(0) is VT-d enable bit. */
	};

	if (!(pci_read_config32(dev, VTBAR) & VTD_CHIPSET_BASE_ADDRESS_ENABLE))
		return false;

	*base = sa_server_read_map_entry(PCI_BDF(dev), &vtd_bar_descriptor);
	*size = 4 * KiB;
	return true;
}

static bool get_pcie_mmcfg_bar(struct device *dev, uint64_t *base, uint64_t *size)
{
	/* The PCIe MMCFG limit in registers is not correct thus using the configuration value here. */
	*base = CONFIG_ECAM_MMCONF_BASE_ADDRESS;
	*size = CONFIG_ECAM_MMCONF_LENGTH;
	return true;
}

static bool get_dpr(struct device *dev, uint64_t *base, uint64_t *size)
{
	union dpr_register dpr = { .raw = pci_read_config32(dev, DPR) };
	*size = dpr.size * MiB;
	/* DPR base is read as 0s so it is calculated based on the assumption that DPR is below TSEG. */
	*base = sa_server_get_tseg_base() - *size;
	return true;
}

static bool get_tseg(struct device *dev, uint64_t *base, uint64_t *size)
{
	*base = sa_server_get_tseg_base();
	*size = sa_server_get_tseg_size();
	return true;
}

void sa_server_soc_add_fixed_mmio_resources(struct device *dev, int *resource_cnt)
{
	struct sa_server_mmio_descriptor vtd_descriptor = {
		.get_resource = get_vtd_bar,
		.description = "VT-d BAR",
	};

	sa_server_add_mmio_resources(dev, resource_cnt, &vtd_descriptor, 1);

	if (!sa_server_is_on_pch_domain(dev))
		return;

	struct sa_server_mmio_descriptor mmio_descriptors[] = {
		{
			.get_resource = get_pcie_mmcfg_bar,
			.description = "PCIe MMCFG BAR",
		},
		{
			.get_resource = get_dpr,
			.description = "DMA Protection Range",
		},
		{
			.get_resource = get_tseg,
			.description = "TSEG",
		},
		{
			.base = RESERVED_BASE_ADDRESS,
			.size = RESERVED_BASE_SIZE,
			.description = "PCH Reserved",
		},
		{
			.base = IO_APIC_ADDR,
			.size = 1 * MiB,
			.description = "I/O APIC",
		},
		{
			.base = HPET_BASE_ADDRESS,
			.size = 1 * MiB,
			.description = "HPET/TXT/TPM",
		},
		{
			.base = LAPIC_DEFAULT_BASE,
			.size = 2 * MiB,
			.description = "Local APIC",
		},
	};

	sa_server_add_mmio_resources(dev, resource_cnt, mmio_descriptors,
						ARRAY_SIZE(mmio_descriptors));
}
