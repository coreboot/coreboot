/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <stdint.h>
#include <soc/iomap.h>
#include <soc/iosf.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>

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
	unsigned long smmrrh;
	unsigned long smmrrl;
	unsigned long base_k, size_k;
	const unsigned long four_gig_kib = (4 << (30 - 10));
	int index = 0;

	/* Read standard PCI resources. */
	pci_dev_read_resources(dev);

	/* PCIe memory-mapped config space access - 256 MiB. */
	mmconf = iosf_bunit_read(BUNIT_MMCONF_REG) & ~((1 << 28) - 1);
	mmio_range(dev, BUNIT_MMCONF_REG, mmconf, CONFIG_ECAM_MMCONF_BUS_NUMBER * MiB);

	/* 0 -> 0xa0000 */
	base_k = RES_IN_KiB(0);
	size_k = RES_IN_KiB(0xa0000) - base_k;
	ram_resource_kb(dev, index++, base_k, size_k);

	/* The SMMRR registers are 1MiB granularity with smmrrh being
	 * inclusive of the SMM region. */
	smmrrl = (iosf_bunit_read(BUNIT_SMRRL) & 0xffff) << 10;
	smmrrh = ((iosf_bunit_read(BUNIT_SMRRH) & 0xffff) + 1) << 10;

	/* 0xc0000 -> smrrl - cacheable and usable */
	base_k = RES_IN_KiB(0xc0000);
	size_k = smmrrl - base_k;
	ram_resource_kb(dev, index++, base_k, size_k);

	if (smmrrh > smmrrl)
		reserved_ram_resource_kb(dev, index++, smmrrl, smmrrh - smmrrl);

	/* All address space between bmbound and smmrrh is unusable. */
	bmbound_k = RES_IN_KiB(nc_read_top_of_low_memory());
	mmio_resource_kb(dev, index++, smmrrh, bmbound_k - smmrrh);

	/*
	 * The BMBOUND_HI register matches register bits of 31:24 with address
	 * bits of 35:28. Therefore, shift register to align properly.
	 */
	bmbound_hi = iosf_bunit_read(BUNIT_BMBOUND_HI) & ~((1 << 24) - 1);
	bmbound_hi = RES_IN_KiB(bmbound_hi) << 4;
	if (bmbound_hi > four_gig_kib)
		ram_resource_kb(dev, index++, four_gig_kib, bmbound_hi - four_gig_kib);

	/*
	 * Reserve everything between A segment and 1MB:
	 *
	 * 0xa0000 - 0xbffff: legacy VGA
	 * 0xc0000 - 0xfffff: RAM
	 */
	mmio_resource_kb(dev, index++, (0xa0000 >> 10), (0xc0000 - 0xa0000) >> 10);
	reserved_ram_resource_kb(dev, index++, (0xc0000 >> 10), (0x100000 - 0xc0000) >> 10);
}

static void nc_generate_ssdt(const struct device *dev)
{
	generate_cpu_entries(dev);

	acpigen_write_scope("\\");
	acpigen_write_name_dword("TOLM", nc_read_top_of_low_memory());
	acpigen_pop_len();
}

static struct device_operations nc_ops = {
	.read_resources   = nc_read_resources,
	.acpi_fill_ssdt   = nc_generate_ssdt,
	.ops_pci          = &soc_pci_ops,
};

static const struct pci_driver nc_driver __pci_driver = {
	.ops    = &nc_ops,
	.vendor = PCI_VID_INTEL,
	.device = SOC_DEVID,
};
