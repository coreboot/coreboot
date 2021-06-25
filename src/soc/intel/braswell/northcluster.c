/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <cbmem.h>
#include <cpu/x86/smm.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <cpu/x86/lapic_def.h>
#include <fsp/util.h>
#include <soc/iomap.h>
#include <soc/iosf.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include <stddef.h>

/*
 * Host Memory Map:
 *
 * +--------------------------+ BMBOUND_HI
 * |     Usable DRAM          |
 * +--------------------------+ 4GiB
 * |     PCI Address Space    |
 * +--------------------------+ BMBOUND
 * |     TPM                  |
 * +--------------------------+ IMR2
 * |     TXE                  |
 * +--------------------------+ IMR1
 * |     iGD                  |
 * +--------------------------+
 * |     GTT                  |
 * +--------------------------+ SMMRRH, IRM0
 * |     TSEG                 |
 * +--------------------------+ SMMRRL
 * |     Usable DRAM          |
 * +--------------------------+ 0
 *
 * Note that there are really only a few regions that need to enumerated w.r.t.
 * coreboot's resource model:
 *
 * +--------------------------+ BMBOUND_HI
 * |     Cacheable/Usable     |
 * +--------------------------+ 4GiB
 *
 * +--------------------------+ BMBOUND
 * |    Uncacheable/Reserved  |
 * +--------------------------+ SMMRRH
 * |    Cacheable/Reserved    |
 * +--------------------------+ SMMRRL
 * |     Cacheable/Usable     |
 * +--------------------------+ 0
 */
uint32_t nc_read_top_of_low_memory(void)
{
	static uint32_t tolm;

	if (tolm)
		return tolm;

	tolm = iosf_bunit_read(BUNIT_BMBOUND) & ~((1 << 27) - 1);

	return tolm;
}

static void nc_read_resources(struct device *dev)
{
	uint64_t mmconf;
	uint64_t bmbound;
	uint64_t bmbound_hi;
	uintptr_t smm_base;
	size_t smm_size;
	uint64_t tseg_base;
	uint64_t tseg_top;
	uint64_t fsp_res_base;
	void *fsp_reserved_memory_area;
	int index = 0;

	/* Read standard PCI resources. */
	pci_dev_read_resources(dev);

	/* Determine TSEG data */
	smm_region(&smm_base, &smm_size);
	tseg_base = smm_base;
	tseg_top = tseg_base + smm_size;

	/* Determine the base of the FSP reserved memory */
	fsp_reserved_memory_area = cbmem_find(CBMEM_ID_FSP_RESERVED_MEMORY);
	if (fsp_reserved_memory_area) {
		fsp_res_base = (uintptr_t)fsp_reserved_memory_area;
	} else {
		/* If no FSP reserved area */
		fsp_res_base = tseg_base;
	}

	/* PCIe memory-mapped config space access - 256 MiB. */
	mmconf = iosf_bunit_read(BUNIT_MMCONF_REG) & ~((1 << 28) - 1);
	mmio_range(dev, BUNIT_MMCONF_REG, mmconf, CONFIG_ECAM_MMCONF_BUS_NUMBER * MiB);

	/* 0 -> 0xa0000 */
	ram_from_to(dev, index++, 0, 0xa0000);

	/* High memory -> fsp_res_base - cacheable and usable */
	ram_from_to(dev, index++, 1 * MiB, fsp_res_base);

	/* fsp_res_base -> tseg_top - Reserved */
	reserved_ram_from_to(dev, index++, fsp_res_base, tseg_top);

	/* TSEG TOP -> bmbound is memory backed mmio. */
	bmbound = nc_read_top_of_low_memory();
	mmio_from_to(dev, index++, tseg_top, bmbound);

	/*
	 * The BMBOUND_HI register matches register bits of 31:24 with address
	 * bits of 35:28. Therefore, shift register to align properly.
	 */
	bmbound_hi = iosf_bunit_read(BUNIT_BMBOUND_HI) & ~((1 << 24) - 1);
	bmbound_hi <<= 4;
	if (bmbound_hi > 4ull * GiB)
		ram_from_to(dev, index++, 4ull * GiB, bmbound_hi);

	/*
	 * Reserve everything between A segment and 1MB:
	 *
	 * 0xa0000 - 0xbffff: legacy VGA
	 * 0xc0000 - 0xfffff: RAM
	 */
	mmio_resource_kb(dev, index++, (0xa0000 >> 10), (0xc0000 - 0xa0000) >> 10);
	reserved_ram_resource_kb(dev, index++, (0xc0000 >> 10), (0x100000 - 0xc0000) >> 10);

	/*
	 * Reserve local APIC
	 */
	mmio_range(dev, index++, LAPIC_DEFAULT_BASE, 1 * MiB);
}

static void nc_generate_ssdt(const struct device *dev)
{
	generate_cpu_entries(dev);

	acpigen_write_scope("\\");
	acpigen_write_name_dword("TOLM", nc_read_top_of_low_memory());
	acpigen_pop_len();
}

static struct device_operations nc_ops = {
	.read_resources	= nc_read_resources,
	.acpi_fill_ssdt	= nc_generate_ssdt,
	.ops_pci	= &soc_pci_ops,
};

static const struct pci_driver nc_driver __pci_driver = {
	.ops    = &nc_ops,
	.vendor = PCI_VID_INTEL,
	.device = SOC_DEVID,
};
