/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/vga.h>
#include <cbmem.h>
#include <console/console.h>
#include <cpu/x86/lapic_def.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/acpi.h>
#include <soc/chip_common.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include <soc/util.h>
#include <fsp/util.h>
#include <security/intel/txt/txt_platform.h>
#include <security/intel/txt/txt.h>
#include <soc/config.h>
#include <soc/numa.h>
#include <soc/soc_util.h>
#include <stdint.h>

struct proximity_domains pds = {
	.num_pds = 0,
	.pds = NULL,
};

struct map_entry {
	uint32_t    reg;
	int         is_64_bit;
	int         is_limit;
	int         mask_bits;
	const char *description;
};

enum {
	TOHM_REG,
	MMIOL_REG,
	MMCFG_BASE_REG,
	MMCFG_LIMIT_REG,
	TOLM_REG,
	/* NCMEM and ME ranges are mutually exclusive */
	NCMEM_BASE_REG,
	NCMEM_LIMIT_REG,
	ME_BASE_REG,
	ME_LIMIT_REG,
	TSEG_BASE_REG,
	TSEG_LIMIT_REG,
	VTDBAR_REG,
	/* Must be last. */
	NUM_MAP_ENTRIES
};

static struct map_entry memory_map[NUM_MAP_ENTRIES] = {
		[TOHM_REG] = MAP_ENTRY_LIMIT_64(VTD_TOHM_CSR, 26, "TOHM"),
		[MMIOL_REG] = MAP_ENTRY_BASE_32(VTD_MMIOL_CSR, "MMIOL"),
		[MMCFG_BASE_REG] = MAP_ENTRY_BASE_64(VTD_MMCFG_BASE_CSR, "MMCFG_BASE"),
		[MMCFG_LIMIT_REG] = MAP_ENTRY_LIMIT_64(VTD_MMCFG_LIMIT_CSR, 26, "MMCFG_LIMIT"),
		[TOLM_REG] = MAP_ENTRY_LIMIT_32(VTD_TOLM_CSR, 26, "TOLM"),
#if CONFIG(SOC_INTEL_HAS_NCMEM)
		[NCMEM_BASE_REG] = MAP_ENTRY_BASE_64(VTD_NCMEM_BASE_CSR, "NCMEM_BASE"),
		[NCMEM_LIMIT_REG] = MAP_ENTRY_LIMIT_64(VTD_NCMEM_LIMIT_CSR, 19, "NCMEM_LIMIT"),
#else
		[ME_BASE_REG] = MAP_ENTRY_BASE_64(VTD_ME_BASE_CSR, "ME_BASE"),
		[ME_LIMIT_REG] = MAP_ENTRY_LIMIT_64(VTD_ME_LIMIT_CSR, 19, "ME_LIMIT"),
#endif
		[TSEG_BASE_REG] = MAP_ENTRY_BASE_32(VTD_TSEG_BASE_CSR, "TSEGMB_BASE"),
		[TSEG_LIMIT_REG] = MAP_ENTRY_LIMIT_32(VTD_TSEG_LIMIT_CSR, 20, "TSEGMB_LIMIT"),
		[VTDBAR_REG] = MAP_ENTRY_BASE_32(VTD_BAR_CSR, "VTD_BAR"),
};

static void read_map_entry(struct device *dev, struct map_entry *entry,
	uint64_t *result)
{
	uint64_t value;
	uint64_t mask;

	if (!entry->reg) {
		*result = 0;
		return;
	}
	if (entry->reg == VTD_BAR_CSR && !(pci_read_config32(dev, entry->reg) & 1)) {
		/* VTDBAR is not enabled */
		*result = 0;
		return;
	}

	mask = ((1ULL << entry->mask_bits) - 1);
	mask = ~mask;

	value = 0;

	if (entry->is_64_bit) {
		value = pci_read_config32(dev, entry->reg + sizeof(uint32_t));
		value <<= 32;
	}

	value |= (uint64_t)pci_read_config32(dev, entry->reg);
	value &= mask;

	if (entry->is_limit)
		value |= ~mask;

	*result = value;
}

static void mc_read_map_entries(struct device *dev, uint64_t *values)
{
	int i;
	for (i = 0; i < NUM_MAP_ENTRIES; i++)
		read_map_entry(dev, &memory_map[i], &values[i]);
}

static void mc_report_map_entries(struct device *dev, uint64_t *values)
{
	int i;
	for (i = 0; i < NUM_MAP_ENTRIES; i++) {
		if (!memory_map[i].description)
			continue;

		printk(BIOS_DEBUG, "%s: MC MAP: %s: 0x%llx\n",
		       dev_path(dev), memory_map[i].description, values[i]);
	}
}

static void configure_dpr(struct device *dev)
{
	const uintptr_t cbmem_top_mb = ALIGN_UP((uintptr_t)cbmem_top(), MiB) / MiB;
	union dpr_register dpr = { .raw = pci_read_config32(dev, VTD_LTDPR) };

	/* The DPR lock bit has to be set sufficiently early. It looks like
	 * it cannot be set anymore after FSP-S.
	 */
	dpr.lock = 1;
	dpr.epm = 1;
	dpr.size = dpr.top - cbmem_top_mb;
	pci_write_config32(dev, VTD_LTDPR, dpr.raw);
}

/*
 * Host Memory Map:
 *
 * +--------------------------+ TOCM (2 pow 46 - 1)
 * |     Reserved             |
 * +--------------------------+
 * |     MMIOH (relocatable)  |
 * +--------------------------+
 * |     PCISeg               |
 * +--------------------------+ TOHM
 * |    High DRAM Memory      |
 * +--------------------------+ 4GiB (0x100000000)
 * +--------------------------+ 0xFFFF_FFFF
 * |     Firmware             |
 * +--------------------------+ 0xFF00_0000
 * |     Reserved             |
 * +--------------------------+ 0xFEF0_0000
 * |     Local xAPIC          |
 * +--------------------------+ 0xFEE0_0000
 * |     HPET/LT/TPM/Others   |
 * +--------------------------+ 0xFED0_0000
 * |     I/O xAPIC            |
 * +--------------------------+ 0xFEC0_0000
 * |     Reserved             |
 * +--------------------------+ 0xFEB8_0000
 * |     Reserved             |
 * +--------------------------+ 0xFEB0_0000
 * |     Reserved             |
 * +--------------------------+ 0xFE00_0000
 * |     MMIOL (relocatable)  |
 * |     P2SB PCR cfg BAR     | (0xfd000000 - 0xfdffffff
 * |     BAR space            | [mem 0x90000000-0xfcffffff] available for PCI devices
 * +--------------------------+ 0x9000_0000
 * |PCIe MMCFG (relocatable)  | CONFIG_ECAM_MMCONF_BASE_ADDRESS 64 or 256MB
 * |                          | (0x80000000 - 0x8fffffff, 0x40000)
 * +--------------------------+ TOLM
 * |     MEseg (relocatable)  | 32, 64, 128 or 256 MB (0x78000000 - 0x7fffffff, 0x20000)
 * +--------------------------+
 * |     Tseg (relocatable)   | N x 8MB (0x70000000 - 0x77ffffff, 0x20000)
 * +--------------------------+
 * |     DPR                  |
 * +--------------------------+ 1M aligned DPR base
 * |     Unused memory        |
 * +--------------------------+ cbmem_top
 * |     Reserved - CBMEM     | (0x6fffe000 - 0x6fffffff, 0x2000)
 * +--------------------------+
 * |     Reserved - FSP       | (0x6fbfe000 - 0x6fffdfff, 0x400000)
 * +--------------------------+ top_of_ram (0x6fbfdfff)
 * |     Low DRAM Memory      |
 * +--------------------------+ FFFFF (1MB)
 * |     E & F segments       |
 * +--------------------------+ E0000
 * |     C & D segments       |
 * +--------------------------+ C0000
 * |     VGA & SMM Memory     |
 * +--------------------------+ A0000
 * |    Conventional Memory   |
 * |      (DOS Range)         |
 * +--------------------------+ 0
 */

static void mc_add_dram_resources(struct device *dev, int *res_count)
{
	const struct resource *res;
	uint64_t mc_values[NUM_MAP_ENTRIES];
	uint64_t top_of_ram;
	int index = *res_count;
	struct range_entry fsp_mem;

	/* Read in the MAP registers and report their values. */
	mc_read_map_entries(dev, &mc_values[0]);
	mc_report_map_entries(dev, &mc_values[0]);

	if (mc_values[VTDBAR_REG]) {
		res = mmio_range(dev, VTD_BAR_CSR, mc_values[VTDBAR_REG], 8 * KiB);
		LOG_RESOURCE("vtd_bar", dev, res);
	}

	/* Only add dram resources once. */
	if (dev->upstream->secondary != 0 || dev->upstream->segment_group != 0)
		return;

	/* Conventional Memory (DOS region, 0x0 to 0x9FFFF) */
	res = ram_from_to(dev, index++, 0, 0xa0000);
	LOG_RESOURCE("legacy_ram", dev, res);

	/* 1MB -> top_of_ram */
	fsp_find_reserved_memory(&fsp_mem);
	top_of_ram = range_entry_base(&fsp_mem) - 1;
	res = ram_from_to(dev, index++, 1 * MiB, top_of_ram);
	LOG_RESOURCE("low_ram", dev, res);

	/* top_of_ram -> cbmem_top */
	res = ram_from_to(dev, index++, top_of_ram, (uintptr_t)cbmem_top());
	LOG_RESOURCE("cbmem_ram", dev, res);

	/* Mark TSEG/SMM region as reserved */
	res = reserved_ram_from_to(dev, index++, mc_values[TSEG_BASE_REG],
				   mc_values[TSEG_LIMIT_REG] + 1);
	LOG_RESOURCE("mmio_tseg", dev, res);

	/* Reserve DPR region */
	union dpr_register dpr = { .raw = pci_read_config32(dev, VTD_LTDPR) };
	if (dpr.size) {
		/*
		 * cbmem_top -> DPR base:
		 * DPR has a 1M granularity so it's possible if cbmem_top is not 1M
		 * aligned that some memory does not get marked as assigned.
		 */
		res = reserved_ram_from_to(dev, index++, (uintptr_t)cbmem_top(),
			(dpr.top - dpr.size) * MiB);
		LOG_RESOURCE("unused_dram", dev, res);

		/* DPR base -> DPR top */
		res = reserved_ram_from_to(dev, index++, (dpr.top - dpr.size) * MiB,
					   dpr.top * MiB);
		LOG_RESOURCE("dpr", dev, res);
	}

	/* Mark TSEG/SMM region as reserved */
	res = reserved_ram_from_to(dev, index++, mc_values[TSEG_BASE_REG],
				   mc_values[TSEG_LIMIT_REG] + 1);
	LOG_RESOURCE("mmio_tseg", dev, res);

	/* Mark region between TSEG - TOLM (eg. MESEG) as reserved */
	res = reserved_ram_from_to(dev, index++, mc_values[TSEG_LIMIT_REG] + 1,
				   mc_values[TOLM_REG]);
	LOG_RESOURCE("mmio_tolm", dev, res);

	if (CONFIG(SOC_INTEL_HAS_CXL)) {
		/* 4GiB -> CXL Memory */
		uint32_t gi_mem_size;
		gi_mem_size = get_generic_initiator_mem_size(); /* unit: 64MB */
		/*
		 * Memory layout when there is CXL HDM (Host-managed Device Memory):
		 * --------------  <- TOHM
		 * CXL memory regions (pds global variable records the base/size of them)
		 * Processor attached high memory
		 * --------------  <- 0x100000000 (4GB)
		 */
		res = upper_ram_end(dev, index++,
			mc_values[TOHM_REG] - ((uint64_t)gi_mem_size << 26) + 1);
		LOG_RESOURCE("high_ram", dev, res);

		/* CXL Memory */
		uint8_t i;
		for (i = 0; i < pds.num_pds; i++) {
			if (pds.pds[i].pd_type == PD_TYPE_PROCESSOR)
				continue;

			unsigned long flags = IORESOURCE_CACHEABLE;
			int cxl_mode = get_cxl_mode();
			if (cxl_mode == XEONSP_CXL_SP_MEM)
				flags |= IORESOURCE_SOFT_RESERVE;
			else
				flags |= IORESOURCE_STORED;

			res = fixed_mem_range_flags(dev, index++,
				(uint64_t)pds.pds[i].base << 26,
				(uint64_t)pds.pds[i].size << 26, flags);
			if (cxl_mode == XEONSP_CXL_SP_MEM)
				LOG_RESOURCE("specific_purpose_memory", dev, res);
			else
				LOG_RESOURCE("CXL_memory", dev, res);
		}
	} else {
		/* 4GiB -> TOHM */
		res = upper_ram_end(dev, index++, mc_values[TOHM_REG] + 1);
		LOG_RESOURCE("high_ram", dev, res);
	}

	/* add MMIO CFG resource */
	res = mmio_from_to(dev, index++, mc_values[MMCFG_BASE_REG],
			   mc_values[MMCFG_LIMIT_REG] + 1);
	LOG_RESOURCE("mmiocfg_res", dev, res);

	/* add Local APIC resource */
	res = mmio_range(dev, index++, LAPIC_DEFAULT_BASE, 0x00001000);
	LOG_RESOURCE("apic_res", dev, res);

	/*
	 * Add legacy region as reserved - 0xa000 - 1MB
	 * Reserve everything between A segment and 1MB:
	 *
	 * 0xa0000 - 0xbffff: legacy VGA
	 * 0xc0000 - 0xfffff: RAM
	 */
	res = mmio_range(dev, index++, VGA_MMIO_BASE, VGA_MMIO_SIZE);
	LOG_RESOURCE("legacy_mmio", dev, res);

	res = reserved_ram_from_to(dev, index++, 0xc0000, 1 * MiB);
	LOG_RESOURCE("legacy_write_protect", dev, res);

	*res_count = index;
}

static void mmapvtd_read_resources(struct device *dev)
{
	int index = 0;

	/* Read standard PCI resources. */
	pci_dev_read_resources(dev);

	/* set up DPR */
	configure_dpr(dev);

	/* Calculate and add DRAM resources. */
	mc_add_dram_resources(dev, &index);
}

static void mmapvtd_init(struct device *dev)
{
}

static struct device_operations mmapvtd_ops = {
	.read_resources    = mmapvtd_read_resources,
	.set_resources     = pci_dev_set_resources,
	.enable_resources  = pci_dev_enable_resources,
	.init              = mmapvtd_init,
	.ops_pci           = &soc_pci_ops,
};

static const unsigned short mmapvtd_ids[] = {
	MMAP_VTD_CFG_REG_DEVID, /* Memory Map/Intel® VT-d Configuration Registers */
	0
};

static const struct pci_driver mmapvtd_driver __pci_driver = {
	.ops      = &mmapvtd_ops,
	.vendor   = PCI_VID_INTEL,
	.devices  = mmapvtd_ids
};

#if !CONFIG(SOC_INTEL_MMAPVTD_ONLY_FOR_DPR)
static void vtd_read_resources(struct device *dev)
{
	pci_dev_read_resources(dev);

	configure_dpr(dev);
}

static struct device_operations vtd_ops = {
	.read_resources    = vtd_read_resources,
	.set_resources     = pci_dev_set_resources,
	.enable_resources  = pci_dev_enable_resources,
	.ops_pci           = &soc_pci_ops,
};

/* VTD devices on other stacks */
static const struct pci_driver vtd_driver __pci_driver = {
	.ops      = &vtd_ops,
	.vendor   = PCI_VID_INTEL,
	.device   = MMAP_VTD_STACK_CFG_REG_DEVID,
};
#endif

static void dmi3_init(struct device *dev)
{
	if (CONFIG(INTEL_TXT) && skip_intel_txt_lockdown())
		return;
	/* Disable error injection */
	pci_or_config16(dev, ERRINJCON, 1 << 0);

	/*
	 * DMIRCBAR registers are not TXT lockable, but the BAR enable
	 * bit is. TXT requires that DMIRCBAR be disabled for security.
	 */
	pci_and_config32(dev, DMIRCBAR, ~(1 << 0));
}

static struct device_operations dmi3_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= dmi3_init,
	.ops_pci		= &soc_pci_ops,
};

static const struct pci_driver dmi3_driver __pci_driver = {
	.ops		= &dmi3_ops,
	.vendor		= PCI_VID_INTEL,
	.device		= DMI3_DEVID,
};

static void iio_dfx_global_init(struct device *dev)
{
	if (CONFIG(INTEL_TXT) && skip_intel_txt_lockdown())
		return;

	uint16_t reg16;
	pci_or_config16(dev, IIO_DFX_LCK_CTL, 0x3ff);
	reg16 = pci_read_config16(dev, IIO_DFX_TSWCTL0);
	reg16 &= ~(1 << 4); // allow ib mmio cfg
	reg16 &= ~(1 << 5); // ignore acs p2p ma lpbk
	reg16 |= (1 << 3); // me disable
	pci_write_config16(dev, IIO_DFX_TSWCTL0, reg16);
}

static const unsigned short iio_dfx_global_ids[] = {
	0x202d,
	0x203d,
	0
};

static struct device_operations iio_dfx_global_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= iio_dfx_global_init,
	.ops_pci		= &soc_pci_ops,
};

static const struct pci_driver iio_dfx_global_driver __pci_driver = {
	.ops		= &iio_dfx_global_ops,
	.vendor		= PCI_VID_INTEL,
	.devices	= iio_dfx_global_ids,
};
