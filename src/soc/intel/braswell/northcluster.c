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
#define RES_IN_KiB(r) ((r) >> 10)

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
	unsigned long mmconf;
	unsigned long bmbound_k;
	unsigned long bmbound_hi;
	uintptr_t smm_base;
	size_t smm_size;
	unsigned long tseg_base_k;
	unsigned long tseg_top_k;
	unsigned long fsp_res_base_k;
	unsigned long base_k, size_k;
	const unsigned long four_gig_kib = (4 << (30 - 10));
	void *fsp_reserved_memory_area;
	int index = 0;

	/* Read standard PCI resources. */
	pci_dev_read_resources(dev);

	/* Determine TSEG data */
	smm_region(&smm_base, &smm_size);
	tseg_base_k = RES_IN_KiB(smm_base);
	tseg_top_k = tseg_base_k + RES_IN_KiB(smm_size);

	/* Determine the base of the FSP reserved memory */
	fsp_reserved_memory_area = cbmem_find(CBMEM_ID_FSP_RESERVED_MEMORY);
	if (fsp_reserved_memory_area) {
		fsp_res_base_k = RES_IN_KiB((unsigned int)fsp_reserved_memory_area);
	} else {
		/* If no FSP reserved area */
		fsp_res_base_k = tseg_base_k;
	}

	/* PCIe memory-mapped config space access - 256 MiB. */
	mmconf = iosf_bunit_read(BUNIT_MMCONF_REG) & ~((1 << 28) - 1);
	mmio_resource(dev, BUNIT_MMCONF_REG, RES_IN_KiB(mmconf), 256 * 1024);

	/* 0 -> 0xa0000 */
	base_k = RES_IN_KiB(0);
	size_k = RES_IN_KiB(0xa0000) - base_k;
	ram_resource(dev, index++, base_k, size_k);

	/* High memory -> fsp_res_base - cacheable and usable */
	base_k = RES_IN_KiB(0x100000);
	size_k = fsp_res_base_k - base_k;
	ram_resource(dev, index++, base_k, size_k);

	/* fsp_res_base -> tseg_top - Reserved */
	base_k = fsp_res_base_k;
	size_k = tseg_top_k - base_k;
	reserved_ram_resource(dev, index++, base_k, size_k);

	/* TSEG TOP -> bmbound is memory backed mmio. */
	bmbound_k = RES_IN_KiB(nc_read_top_of_low_memory());
	mmio_resource(dev, index++, tseg_top_k, bmbound_k - tseg_top_k);

	/*
	 * The BMBOUND_HI register matches register bits of 31:24 with address
	 * bits of 35:28. Therefore, shift register to align properly.
	 */
	bmbound_hi = iosf_bunit_read(BUNIT_BMBOUND_HI) & ~((1 << 24) - 1);
	bmbound_hi = RES_IN_KiB(bmbound_hi) << 4;
	if (bmbound_hi > four_gig_kib)
		ram_resource(dev, index++, four_gig_kib, bmbound_hi - four_gig_kib);

	/*
	 * Reserve everything between A segment and 1MB:
	 *
	 * 0xa0000 - 0xbffff: legacy VGA
	 * 0xc0000 - 0xfffff: RAM
	 */
	mmio_resource(dev, index++, (0xa0000 >> 10), (0xc0000 - 0xa0000) >> 10);
	reserved_ram_resource(dev, index++, (0xc0000 >> 10), (0x100000 - 0xc0000) >> 10);

	/*
	 * Reserve local APIC
	 */
	base_k = RES_IN_KiB(LAPIC_DEFAULT_BASE);
	size_k = RES_IN_KiB(0x00100000);
	mmio_resource(dev, index++, base_k, size_k);
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
