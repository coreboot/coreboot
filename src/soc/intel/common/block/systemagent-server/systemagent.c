/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/vga.h>
#include <commonlib/bsd/helpers.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_type.h>
#include <fsp/util.h>
#include <intelblocks/acpi.h>
#include <intelblocks/systemagent_server.h>
#include <security/intel/txt/txt_register.h>
#include <soc/pci_devs.h>
#include <soc/systemagent.h>

__weak void sa_server_soc_add_fixed_mmio_resources(struct device *dev, int *resource_cnt)
{
}

__weak unsigned long sa_write_acpi_tables(const struct device *dev, unsigned long current,
					  struct acpi_rsdp *rsdp)
{
	return current;
}

uint64_t sa_server_get_touud(void)
{
	struct sa_server_mem_map_descriptor touud_descriptor = {
		.reg_offset = sa_server_soc_reg_to_pci_offset(TOUUD_REG),
		.is_64_bit = true,
		.is_limit = CONFIG(TOUUD_LIMIT),
	};

	return sa_server_read_map_entry(PCI_BDF(SA_DEV_ROOT), &touud_descriptor);
}

uintptr_t sa_server_get_tolud(void)
{
	struct sa_server_mem_map_descriptor tolud_descriptor = {
		.reg_offset = sa_server_soc_reg_to_pci_offset(TOLUD_REG),
		.is_limit = CONFIG(TOLUD_LIMIT),
	};

	return sa_server_read_map_entry(PCI_BDF(SA_DEV_ROOT), &tolud_descriptor);
}

bool sa_server_is_on_pch_domain(const struct device *dev)
{
	return is_dev_on_domain0(dev);
}

void sa_server_add_mmio_resources(struct device *dev, int *resource_cnt,
				  const struct sa_server_mmio_descriptor *mmio_descriptors,
				  size_t count)
{
	int index = *resource_cnt;
	uint64_t base, size;

	for (int i = 0; i < count; i++) {
		base = mmio_descriptors[i].base;
		size = mmio_descriptors[i].size;

		/**
		 * Prefer to using `base` and `size` to retrieving them from `get_resource()` since
		 * some MMIO resources may not stored in any registers and registers may hold an
		 * incorrect value.
		 */
		if (!size)
			if (!mmio_descriptors[i].get_resource ||
			    !mmio_descriptors[i].get_resource(dev, &base, &size))
				continue;

		printk(BIOS_DEBUG,
		       "SA MMIO resource: %-8s -> base = 0x%08llx, size = 0x%08llx\n",
		       mmio_descriptors[i].description, base, size);

		mmio_range(dev, index++, base, size);
	}

	*resource_cnt = index;
}

/*
 * Host Memory Map:
 * +--------------------------+
 * | MMIOH                    |
 * | (relocatable)            |
 * +--------------------------+ TOUUD
 * | High DRAM                |
 * +--------------------------+ 4GB (0x1_0000_0000)
 * | Firmware                 |
 * +--------------------------+ 0xFF00_0000
 * | Reserved                 |
 * +--------------------------+ 0xFEF0_0000
 * | Local xAPIC              |
 * +--------------------------+ 0xFEE0_0000
 * | HPET, Intel TXT, TPM     |
 * +--------------------------+ 0xFED0_0000
 * | I/O xAPIC                |
 * +--------------------------+ 0xFEC0_0000
 * | Reserved                 |
 * +--------------------------+ MMIOL
 * | MMIOL                    |
 * | (relocatable)            |
 * +--------------------------+
 * | PCIe MMCFG               |
 * | (relocatable)            |
 * +--------------------------+ TOLUD
 * | MESEG (relocatable)      |
 * +--------------------------+
 * | TSEG (relocatable)       |
 * +--------------------------+
 * | DMA Protected Range      |
 * | (relocatable)            |
 * +--------------------------+ DPR base, 1M aligned
 * | Unused memory (possible) |
 * +--------------------------+ cbmem_top
 * | FSP Bootloader TOLUM     |
 * +--------------------------+
 * | FSP Reserved             |
 * +--------------------------+ top_of_ram
 * | Low DRAM                 |
 * | Legacy ISA hole          |
 * +--------------------------+ 0x10_0000 (1MB)
 * | C, D, E, F segments      |
 * +--------------------------+ 0xC_0000
 * | VGA (TSEG, MESEG)        |
 * +--------------------------+ 0xA_0000
 * | DOS Range                |
 * +--------------------------+ 0
 */
static void sa_server_add_dram_resources(struct device *dev, int *resource_cnt)
{
	int index = *resource_cnt;

	/* 0 - > 0xa0000 */
	ram_from_to(dev, index++, 0, 0xa0000);

	/* 0xa0000 - > 0xc0000 */
	mmio_range(dev, index++, VGA_MMIO_BASE, VGA_MMIO_SIZE);

	/* 0xc0000 - > 1MB */
	reserved_ram_from_to(dev, index++, 0xc0000, 1 * MiB);

	/* 1MB -> top_of_ram (FSP reserved base) */
	struct range_entry fsp_mem;
	fsp_find_reserved_memory(&fsp_mem);
	uint32_t top_of_ram = range_entry_base(&fsp_mem);
	ram_from_to(dev, index++, 1 * MiB, top_of_ram);

	/* top_of_ram -> cbmem_top, including FSP reserved and bootloader TOLUM. */
	ram_from_to(dev, index++, top_of_ram, cbmem_top());

	/* Mark cbmem_top to TOLUD as reserved, SoC should provide more details for this ranges. */
	reserved_ram_from_to(dev, index++, cbmem_top(), sa_server_get_tolud());

	/* 4GB -> TOUUD */
	upper_ram_end(dev, index++, sa_server_get_touud());

	*resource_cnt = index;
}

static void sa_server_read_resources(struct device *dev)
{
	int index = 0;

	pci_dev_read_resources(dev);

	/*
	 * Only add DRAM resource when calling from PCH domain.
	 */
	if (sa_server_is_on_pch_domain(dev))
		sa_server_add_dram_resources(dev, &index);

	/* Add all SoC fixed MMIO resources. */
	sa_server_soc_add_fixed_mmio_resources(dev, &index);
}

struct device_operations systemagent_server_ops = {
	.read_resources   = sa_server_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.ops_pci          = &pci_dev_ops_pci,
#if CONFIG(HAVE_ACPI_TABLES)
	.write_acpi_tables = sa_write_acpi_tables,
#endif
};

static const unsigned short systemagent_server_ids[] = {
	PCI_DID_INTEL_SNR_ID,
	0
};

static const struct pci_driver systemagent_server_driver __pci_driver = {
	.ops     = &systemagent_server_ops,
	.vendor  = PCI_VID_INTEL,
	.devices = systemagent_server_ids
};
