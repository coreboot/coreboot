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
	uint64_t smmrrh;
	uint64_t smmrrl;
	int index = 0;

	/* Read standard PCI resources. */
	pci_dev_read_resources(dev);

	/* PCIe memory-mapped config space access - 256 MiB. */
	mmconf = iosf_bunit_read(BUNIT_MMCONF_REG) & ~((1 << 28) - 1);
	mmio_range(dev, BUNIT_MMCONF_REG, mmconf, CONFIG_ECAM_MMCONF_BUS_NUMBER * MiB);

	/* 0 -> 0xa0000 */
	ram_from_to(dev, index++, 0, 0xa0000);

	/* The SMMRR registers are 1MiB granularity with smmrrh being
	 * inclusive of the SMM region. */
	smmrrl = (iosf_bunit_read(BUNIT_SMRRL) & 0xffff) * MiB;
	smmrrh = ((iosf_bunit_read(BUNIT_SMRRH) & 0xffff) + 1) * MiB;

	/* 0xc0000 -> smrrl - cacheable and usable */
	ram_from_to(dev, index++, 0xc0000, smmrrl);

	if (smmrrh > smmrrl)
		reserved_ram_from_to(dev, index++, smmrrl, smmrrh);

	/* All address space between bmbound and smmrrh is unusable. */
	bmbound = nc_read_top_of_low_memory();
	mmio_from_to(dev, index++, smmrrh, bmbound);

	/*
	 * The BMBOUND_HI register matches register bits of 31:24 with address
	 * bits of 35:28. Therefore, shift register to align properly.
	 */
	bmbound_hi = iosf_bunit_read(BUNIT_BMBOUND_HI) & ~((1 << 24) - 1);
	bmbound_hi <<= 4;
	upper_ram_end(dev, index++, bmbound_hi);

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
