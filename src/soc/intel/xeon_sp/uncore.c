/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cbmem.h>
#include <console/console.h>
#include <cpu/x86/lapic_def.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/acpi.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include <soc/util.h>
#include <fsp/util.h>
#include <security/intel/txt/txt_platform.h>
#include <security/intel/txt/txt.h>
#include <stdint.h>

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
	ME_BASE_REG,
	ME_LIMIT_REG,
	TSEG_BASE_REG,
	TSEG_LIMIT_REG,
	/* Must be last. */
	NUM_MAP_ENTRIES
};

static struct map_entry memory_map[NUM_MAP_ENTRIES] = {
		[TOHM_REG] = MAP_ENTRY_LIMIT_64(VTD_TOHM_CSR, 26, "TOHM"),
		[MMIOL_REG] = MAP_ENTRY_BASE_32(VTD_MMIOL_CSR, "MMIOL"),
		[MMCFG_BASE_REG] = MAP_ENTRY_BASE_64(VTD_MMCFG_BASE_CSR, "MMCFG_BASE"),
		[MMCFG_LIMIT_REG] = MAP_ENTRY_LIMIT_64(VTD_MMCFG_LIMIT_CSR, 26, "MMCFG_LIMIT"),
		[TOLM_REG] = MAP_ENTRY_LIMIT_32(VTD_TOLM_CSR, 26, "TOLM"),
		[ME_BASE_REG] = MAP_ENTRY_BASE_64(VTD_ME_BASE_CSR, "ME_BASE"),
		[ME_LIMIT_REG] = MAP_ENTRY_LIMIT_64(VTD_ME_LIMIT_CSR, 19, "ME_LIMIT"),
		[TSEG_BASE_REG] = MAP_ENTRY_BASE_32(VTD_TSEG_BASE_CSR, "TSEGMB_BASE"),
		[TSEG_LIMIT_REG] = MAP_ENTRY_LIMIT_32(VTD_TSEG_LIMIT_CSR, 20, "TSEGMB_LIMIT"),
};

static void read_map_entry(struct device *dev, struct map_entry *entry,
	uint64_t *result)
{
	uint64_t value;
	uint64_t mask;

	/* All registers are on a 1MiB granularity. */
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
		printk(BIOS_DEBUG, "MC MAP: %s: 0x%llx\n",
		       memory_map[i].description, values[i]);
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
	uint64_t base_kb;
	uint64_t size_kb;
	uint64_t mc_values[NUM_MAP_ENTRIES];
	struct resource *resource;
	int index = *res_count;

	/* Only add dram resources once. */
	if (dev->bus->secondary != 0)
		return;

	/* Read in the MAP registers and report their values. */
	mc_read_map_entries(dev, &mc_values[0]);
	mc_report_map_entries(dev, &mc_values[0]);

	/* Conventional Memory (DOS region, 0x0 to 0x9FFFF) */
	base_kb = 0;
	size_kb = (0xa0000 >> 10);
	LOG_MEM_RESOURCE("legacy_ram", dev, index, base_kb, size_kb);
	ram_resource(dev, index++, base_kb, size_kb);

	/* 1MB -> top_of_ram i.e., cbmem_top */
	base_kb = (0x100000 >> 10);
	size_kb = ((uintptr_t)cbmem_top() - 1 * MiB) >> 10;
	LOG_MEM_RESOURCE("low_ram", dev, index, base_kb, size_kb);
	ram_resource(dev, index++, base_kb, size_kb);

	/* Mark TSEG/SMM region as reserved */
	base_kb = (mc_values[TSEG_BASE_REG] >> 10);
	size_kb = (mc_values[TSEG_LIMIT_REG] - mc_values[TSEG_BASE_REG] + 1) >> 10;
	LOG_MEM_RESOURCE("mmio_tseg", dev, index, base_kb, size_kb);
	reserved_ram_resource(dev, index++, base_kb, size_kb);

	/* Reserve and set up DPR */
	configure_dpr(dev);
	union dpr_register dpr = { .raw = pci_read_config32(dev, VTD_LTDPR) };
	if (dpr.size) {
		uint64_t dpr_base_k = (dpr.top - dpr.size) << 10;
		uint64_t dpr_size_k = dpr.size << 10;
		reserved_ram_resource(dev, index++, dpr_base_k, dpr_size_k);
		LOG_MEM_RESOURCE("dpr", dev, index, dpr_base_k, dpr_size_k);
	}

	/* Mark region between TSEG - TOLM (eg. MESEG) as reserved */
	if (mc_values[TSEG_LIMIT_REG] < mc_values[TOLM_REG]) {
		base_kb = ((mc_values[TSEG_LIMIT_REG] + 1) >> 10);
		size_kb = (mc_values[TOLM_REG] - mc_values[TSEG_LIMIT_REG]) >> 10;
		LOG_MEM_RESOURCE("mmio_tolm", dev, index, base_kb, size_kb);
		reserved_ram_resource(dev, index++, base_kb, size_kb);
	}

	/* 4GiB -> TOHM */
	if (mc_values[TOHM_REG] > 0x100000000) {
		base_kb = (0x100000000 >> 10);
		size_kb = (mc_values[TOHM_REG] - 0x100000000 + 1) >> 10;
		LOG_MEM_RESOURCE("high_ram", dev, index, base_kb, size_kb);
		ram_resource(dev, index++, base_kb, size_kb);
	}

	/* add MMIO CFG resource */
	resource = new_resource(dev, index++);
	resource->base = (resource_t) mc_values[MMCFG_BASE_REG];
	resource->size = (resource_t) (mc_values[MMCFG_LIMIT_REG] -
		mc_values[MMCFG_BASE_REG] + 1);
	resource->flags = IORESOURCE_MEM | IORESOURCE_RESERVE |
		IORESOURCE_FIXED | IORESOURCE_STORED | IORESOURCE_ASSIGNED;
	LOG_MEM_RESOURCE("mmiocfg_res", dev, index-1, (resource->base >> 10),
		(resource->size >> 10));

	/* add Local APIC resource */
	resource = new_resource(dev, index++);
	resource->base = LAPIC_DEFAULT_BASE;
	resource->size = 0x00001000;
	resource->flags = IORESOURCE_MEM | IORESOURCE_RESERVE |
		IORESOURCE_FIXED | IORESOURCE_STORED | IORESOURCE_ASSIGNED;
	LOG_MEM_RESOURCE("apic_res", dev, index-1, (resource->base >> 10),
		(resource->size >> 10));

	/*
	 * Add legacy region as reserved - 0xa000 - 1MB
	 * Reserve everything between A segment and 1MB:
	 *
	 * 0xa0000 - 0xbffff: legacy VGA
	 * 0xc0000 - 0xfffff: RAM
	 */
	base_kb = VGA_BASE_ADDRESS >> 10;
	size_kb = VGA_BASE_SIZE >> 10;
	LOG_MEM_RESOURCE("legacy_mmio", dev, index, base_kb, size_kb);
	mmio_resource(dev, index++, base_kb, size_kb);

	base_kb = (0xc0000 >> 10);
	size_kb = (0x100000 - 0xc0000) >> 10;
	LOG_MEM_RESOURCE("legacy_write_protect", dev, index, base_kb, size_kb);
	reserved_ram_resource(dev, index++, base_kb, size_kb);

	*res_count = index;
}

static void mmapvtd_read_resources(struct device *dev)
{
	int index = 0;

	/* Read standard PCI resources. */
	pci_dev_read_resources(dev);

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
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_inject_dsdt  = uncore_inject_dsdt,
#endif
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
