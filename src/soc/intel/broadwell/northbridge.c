/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/intel/haswell/haswell.h>
#include <acpi/acpi.h>
#include <device/pci_ops.h>
#include <stdint.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/acpi.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/refcode.h>
#include <soc/systemagent.h>

u8 systemagent_revision(void)
{
	struct device *sa_dev = pcidev_path_on_root(SA_DEVFN_ROOT);
	return pci_read_config8(sa_dev, PCI_REVISION_ID);
}

static int get_pcie_bar(struct device *dev, unsigned int index, u32 *base,
			u32 *len)
{
	u32 pciexbar_reg;

	*base = 0;
	*len = 0;

	pciexbar_reg = pci_read_config32(dev, index);

	if (!(pciexbar_reg & (1 << 0)))
		return 0;

	switch ((pciexbar_reg >> 1) & 3) {
	case 0: // 256MB
		*base = pciexbar_reg & ((1 << 31)|(1 << 30)|(1 << 29)|
					(1 << 28));
		*len = 256 * 1024 * 1024;
		return 1;
	case 1: // 128M
		*base = pciexbar_reg & ((1 << 31)|(1 << 30)|(1 << 29)|
					(1 << 28)|(1 << 27));
		*len = 128 * 1024 * 1024;
		return 1;
	case 2: // 64M
		*base = pciexbar_reg & ((1 << 31)|(1 << 30)|(1 << 29)|
					(1 << 28)|(1 << 27)|(1 << 26));
		*len = 64 * 1024 * 1024;
		return 1;
	}

	return 0;
}

static int get_bar(struct device *dev, unsigned int index, u32 *base, u32 *len)
{
	u32 bar;

	bar = pci_read_config32(dev, index);

	/* If not enabled don't report it. */
	if (!(bar & 0x1))
		return 0;

	/* Knock down the enable bit. */
	*base = bar & ~1;

	return 1;
}

/* There are special BARs that actually are programmed in the MCHBAR. These
 * Intel special features, but they do consume resources that need to be
 * accounted for. */
static int get_bar_in_mchbar(struct device *dev, unsigned int index, u32 *base,
			     u32 *len)
{
	u32 bar;

	bar = mchbar_read32(index);

	/* If not enabled don't report it. */
	if (!(bar & 0x1))
		return 0;

	/* Knock down the enable bit. */
	*base = bar & ~1;

	return 1;
}

struct fixed_mmio_descriptor {
	unsigned int index;
	u32 size;
	int (*get_resource)(struct device *dev, unsigned int index,
			    u32 *base, u32 *size);
	const char *description;
};

struct fixed_mmio_descriptor mc_fixed_resources[] = {
	{ PCIEXBAR, 0,               get_pcie_bar,      "PCIEXBAR" },
	{ MCHBAR,   MCH_BASE_SIZE,   get_bar,           "MCHBAR"   },
	{ DMIBAR,   DMI_BASE_SIZE,   get_bar,           "DMIBAR"   },
	{ EPBAR,    EP_BASE_SIZE,    get_bar,           "EPBAR"    },
	{ GDXCBAR,  GDXC_BASE_SIZE,  get_bar_in_mchbar, "GDXCBAR"  },
	{ EDRAMBAR, EDRAM_BASE_SIZE, get_bar_in_mchbar, "EDRAMBAR" },
};

/*
 * Add all known fixed MMIO ranges that hang off the host bridge/memory
 * controller device.
 */
static void mc_add_fixed_mmio_resources(struct device *dev)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(mc_fixed_resources); i++) {
		u32 base;
		u32 size;
		struct resource *resource;
		unsigned int index;

		size = mc_fixed_resources[i].size;
		index = mc_fixed_resources[i].index;
		if (!mc_fixed_resources[i].get_resource(dev, index,
							&base, &size))
			continue;

		resource = new_resource(dev, mc_fixed_resources[i].index);
		resource->flags = IORESOURCE_MEM | IORESOURCE_FIXED |
				  IORESOURCE_STORED | IORESOURCE_RESERVE |
				  IORESOURCE_ASSIGNED;
		resource->base = base;
		resource->size = size;
		printk(BIOS_DEBUG, "%s: Adding %s @ %x 0x%08lx-0x%08lx.\n",
		       __func__, mc_fixed_resources[i].description, index,
		       (unsigned long)base, (unsigned long)(base + size - 1));
	}
}

/* Host Memory Map:
 *
 * +--------------------------+ TOUUD
 * |                          |
 * +--------------------------+ 4GiB
 * |     PCI Address Space    |
 * +--------------------------+ TOLUD (also maps into MC address space)
 * |     iGD                  |
 * +--------------------------+ BDSM
 * |     GTT                  |
 * +--------------------------+ BGSM
 * |     TSEG                 |
 * +--------------------------+ TSEGMB
 * |     Usage DRAM           |
 * +--------------------------+ 0
 *
 * Some of the base registers above can be equal making the size of those
 * regions 0. The reason is because the memory controller internally subtracts
 * the base registers from each other to determine sizes of the regions. In
 * other words, the memory map is in a fixed order no matter what.
 */

struct map_entry {
	int reg;
	int is_64_bit;
	int is_limit;
	const char *description;
};

static void read_map_entry(struct device *dev, struct map_entry *entry,
			   uint64_t *result)
{
	uint64_t value;
	uint64_t mask;

	/* All registers are on a 1MiB granularity. */
	mask = ((1ULL<<20)-1);
	mask = ~mask;

	value = 0;

	if (entry->is_64_bit) {
		value = pci_read_config32(dev, entry->reg + 4);
		value <<= 32;
	}

	value |= pci_read_config32(dev, entry->reg);
	value &= mask;

	if (entry->is_limit)
		value |= ~mask;

	*result = value;
}

#define MAP_ENTRY(reg_, is_64_, is_limit_, desc_) \
	{ \
		.reg = reg_,           \
		.is_64_bit = is_64_,   \
		.is_limit = is_limit_, \
		.description = desc_,  \
	}

#define MAP_ENTRY_BASE_64(reg_, desc_) \
	MAP_ENTRY(reg_, 1, 0, desc_)
#define MAP_ENTRY_LIMIT_64(reg_, desc_) \
	MAP_ENTRY(reg_, 1, 1, desc_)
#define MAP_ENTRY_BASE_32(reg_, desc_) \
	MAP_ENTRY(reg_, 0, 0, desc_)

enum {
	TOM_REG,
	TOUUD_REG,
	MESEG_BASE_REG,
	MESEG_LIMIT_REG,
	REMAP_BASE_REG,
	REMAP_LIMIT_REG,
	TOLUD_REG,
	BGSM_REG,
	BDSM_REG,
	TSEG_REG,
	// Must be last.
	NUM_MAP_ENTRIES
};

static struct map_entry memory_map[NUM_MAP_ENTRIES] = {
	[TOM_REG] = MAP_ENTRY_BASE_64(TOM, "TOM"),
	[TOUUD_REG] = MAP_ENTRY_BASE_64(TOUUD, "TOUUD"),
	[MESEG_BASE_REG] = MAP_ENTRY_BASE_64(MESEG_BASE, "MESEG_BASE"),
	[MESEG_LIMIT_REG] = MAP_ENTRY_LIMIT_64(MESEG_LIMIT, "MESEG_LIMIT"),
	[REMAP_BASE_REG] = MAP_ENTRY_BASE_64(REMAPBASE, "REMAP_BASE"),
	[REMAP_LIMIT_REG] = MAP_ENTRY_LIMIT_64(REMAPLIMIT, "REMAP_LIMIT"),
	[TOLUD_REG] = MAP_ENTRY_BASE_32(TOLUD, "TOLUD"),
	[BDSM_REG] = MAP_ENTRY_BASE_32(BDSM, "BDSM"),
	[BGSM_REG] = MAP_ENTRY_BASE_32(BGSM, "BGSM"),
	[TSEG_REG] = MAP_ENTRY_BASE_32(TSEG, "TSEGMB"),
};

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
	/* One can validate the BDSM and BGSM against the GGC. */
	printk(BIOS_DEBUG, "MC MAP: GGC: 0x%x\n", pci_read_config16(dev, GGC));
}

static void mc_add_dram_resources(struct device *dev, int *resource_cnt)
{
	unsigned long base_k, size_k;
	unsigned long touud_k;
	unsigned long index;
	struct resource *resource;
	uint64_t mc_values[NUM_MAP_ENTRIES];
	unsigned long dpr_size = 0;
	u32 dpr_reg;

	/* Read in the MAP registers and report their values. */
	mc_read_map_entries(dev, &mc_values[0]);
	mc_report_map_entries(dev, &mc_values[0]);

	/*
	 * DMA Protected Range can be reserved below TSEG for PCODE patch
	 * or TXT/Boot Guard related data.  Rather than report a base address
	 * the DPR register reports the TOP of the region, which is the same
	 * as TSEG base.  The region size is reported in MiB in bits 11:4.
	 */
	dpr_reg = pci_read_config32(dev, DPR);
	if (dpr_reg & DPR_EPM) {
		dpr_size = (dpr_reg & DPR_SIZE_MASK) << 16;
		printk(BIOS_INFO, "DPR SIZE: 0x%lx\n", dpr_size);
	}

	/*
	 * These are the host memory ranges that should be added:
	 * - 0 -> 0xa0000: cacheable
	 * - 0xc0000 -> TSEG : cacheable
	 * - TESG -> BGSM: cacheable with standard MTRRs and reserved
	 * - BGSM -> TOLUD: not cacheable with standard MTRRs and reserved
	 * - 4GiB -> TOUUD: cacheable
	 *
	 * The default SMRAM space is reserved so that the range doesn't
	 * have to be saved during S3 Resume. Once marked reserved the OS
	 * cannot use the memory. This is a bit of an odd place to reserve
	 * the region, but the CPU devices don't have dev_ops->read_resources()
	 * called on them.
	 *
	 * The range 0xa0000 -> 0xc0000 does not have any resources
	 * associated with it to handle legacy VGA memory. If this range
	 * is not omitted the mtrr code will setup the area as cacheable
	 * causing VGA access to not work.
	 *
	 * The TSEG region is mapped as cacheable so that one can perform
	 * SMRAM relocation faster. Once the SMRR is enabled the SMRR takes
	 * precedence over the existing MTRRs covering this region.
	 *
	 * It should be noted that cacheable entry types need to be added in
	 * order. The reason is that the current MTRR code assumes this and
	 * falls over itself if it isn't.
	 *
	 * The resource index starts low and should not meet or exceed
	 * PCI_BASE_ADDRESS_0.
	 */
	index = *resource_cnt;

	/* 0 - > 0xa0000 */
	base_k = 0;
	size_k = (0xa0000 >> 10) - base_k;
	ram_resource(dev, index++, base_k, size_k);

	/* 0xc0000 -> TSEG - DPR */
	base_k = 0xc0000 >> 10;
	size_k = (unsigned long)(mc_values[TSEG_REG] >> 10) - base_k;
	size_k -= dpr_size >> 10;
	ram_resource(dev, index++, base_k, size_k);

	/* TSEG - DPR -> BGSM */
	resource = new_resource(dev, index++);
	resource->base = mc_values[TSEG_REG] - dpr_size;
	resource->size = mc_values[BGSM_REG] - resource->base;
	resource->flags = IORESOURCE_MEM | IORESOURCE_FIXED |
			  IORESOURCE_STORED | IORESOURCE_RESERVE |
			  IORESOURCE_ASSIGNED | IORESOURCE_CACHEABLE;

	/* BGSM -> TOLUD */
	resource = new_resource(dev, index++);
	resource->base = mc_values[BGSM_REG];
	resource->size = mc_values[TOLUD_REG] - resource->base;
	resource->flags = IORESOURCE_MEM | IORESOURCE_FIXED |
			  IORESOURCE_STORED | IORESOURCE_RESERVE |
			  IORESOURCE_ASSIGNED;

	/* 4GiB -> TOUUD */
	base_k = 4096 * 1024; /* 4GiB */
	touud_k = mc_values[TOUUD_REG] >> 10;
	size_k = touud_k - base_k;
	if (touud_k > base_k)
		ram_resource(dev, index++, base_k, size_k);

	/* Reserve everything between A segment and 1MB:
	 *
	 * 0xa0000 - 0xbffff: legacy VGA
	 * 0xc0000 - 0xfffff: RAM
	 */
	mmio_resource(dev, index++, (0xa0000 >> 10), (0xc0000 - 0xa0000) >> 10);
	reserved_ram_resource(dev, index++, (0xc0000 >> 10),
				(0x100000 - 0xc0000) >> 10);

	*resource_cnt = index;
}

static void systemagent_read_resources(struct device *dev)
{
	int index = 0;
	const bool vtd_capable =
		!(pci_read_config32(dev, CAPID0_A) & VTD_DISABLE);

	/* Read standard PCI resources. */
	pci_dev_read_resources(dev);

	/* Add all fixed MMIO resources. */
	mc_add_fixed_mmio_resources(dev);

	/* Add VT-d MMIO resources if capable */
	if (vtd_capable) {
		mmio_resource(dev, index++, GFXVT_BASE_ADDRESS / KiB,
			GFXVT_BASE_SIZE / KiB);
		mmio_resource(dev, index++, VTVC0_BASE_ADDRESS / KiB,
			VTVC0_BASE_SIZE / KiB);
	}

	/* Calculate and add DRAM resources. */
	mc_add_dram_resources(dev, &index);
}

static void systemagent_init(struct device *dev)
{
	/* Enable Power Aware Interrupt Routing. */
	mchbar_clrsetbits8(MCH_PAIR, 0x7, 0x4);	/* Clear 2:0, set Fixed Priority */

	/*
	 * Set bits 0+1 of BIOS_RESET_CPL to indicate to the CPU
	 * that BIOS has initialized memory and power management
	 */
	mchbar_setbits8(BIOS_RESET_CPL, 3);
	printk(BIOS_DEBUG, "Set BIOS_RESET_CPL\n");

	/* Configure turbo power limits 1ms after reset complete bit */
	mdelay(1);
	set_power_limits(28);
}

static struct device_operations systemagent_ops = {
	.read_resources   = systemagent_read_resources,
	.acpi_fill_ssdt   = generate_cpu_entries,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = systemagent_init,
	.ops_pci          = &pci_dev_ops_pci,
};

static const unsigned short systemagent_ids[] = {
	0x0a04, /* Haswell ULT */
	0x1604, /* Broadwell-U/Y */
	0x1610, /* Broadwell-H Desktop */
	0x1614, /* Broadwell-H Mobile */
	0
};

static const struct pci_driver systemagent_driver __pci_driver = {
	.ops     = &systemagent_ops,
	.vendor  = PCI_VID_INTEL,
	.devices = systemagent_ids
};

static struct device_operations pci_domain_ops = {
	.read_resources    = &pci_domain_read_resources,
	.set_resources     = &pci_domain_set_resources,
	.scan_bus          = &pci_domain_scan_bus,
#if CONFIG(HAVE_ACPI_TABLES)
	.write_acpi_tables = &northbridge_write_acpi_tables,
#endif
};

static struct device_operations cpu_bus_ops = {
	.read_resources   = noop_read_resources,
	.set_resources    = noop_set_resources,
	.init             = mp_cpu_bus_init,
};

static void broadwell_enable(struct device *dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		dev->ops = &pci_domain_ops;
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	}
}

static void broadwell_init_pre_device(void *chip_info)
{
	broadwell_run_reference_code();
}

struct chip_operations soc_intel_broadwell_ops = {
	CHIP_NAME("Intel Broadwell")
	.enable_dev = &broadwell_enable,
	.init       = &broadwell_init_pre_device,
};
