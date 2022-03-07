/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <console/console.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <stdint.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <timer.h>

#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include <soc/systemagent.h>
#include <soc/acpi.h>

#define _1ms 1
#define WAITING_STEP 100

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
	case 0: /* 256MB */
		*base = pciexbar_reg &
			((1 << 31) | (1 << 30) | (1 << 29) | (1 << 28));
		*len = 256 * 1024 * 1024;
		return 1;
	case 1: /* 128M */
		*base = pciexbar_reg & ((1 << 31) | (1 << 30) | (1 << 29) |
					(1 << 28) | (1 << 27));
		*len = 128 * 1024 * 1024;
		return 1;
	case 2: /* 64M */
		*base = pciexbar_reg & ((1 << 31) | (1 << 30) | (1 << 29) |
					(1 << 28) | (1 << 27) | (1 << 26));
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

struct fixed_mmio_descriptor {
	unsigned int index;
	u32 size;
	int (*get_resource)(struct device *dev, unsigned int index, u32 *base,
			    u32 *size);
	const char *description;
};

struct fixed_mmio_descriptor mc_fixed_resources[] = {
	{PCIEXBAR, 0, get_pcie_bar, "PCIEXBAR"},
	{MCHBAR, MCH_BASE_SIZE, get_bar, "MCHBAR"},
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
		if (!mc_fixed_resources[i].get_resource(dev, index, &base,
							&size))
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
	mask = ((1ULL << 20) - 1);
	mask = ~mask;

	value = 0;

	if (entry->is_64_bit) {
		value = pci_read_config32(dev, entry->reg + 4);
		value <<= 32;
	}

	value |= (uint64_t)pci_read_config32(dev, entry->reg);
	value &= mask;

	if (entry->is_limit)
		value |= ~mask;

	*result = value;
}

#define MAP_ENTRY(reg_, is_64_, is_limit_, desc_)                        \
	{                                                                \
		.reg = reg_, .is_64_bit = is_64_, .is_limit = is_limit_, \
		.description = desc_,                                    \
	}

#define MAP_ENTRY_BASE_64(reg_, desc_) MAP_ENTRY(reg_, 1, 0, desc_)
#define MAP_ENTRY_LIMIT_64(reg_, desc_) MAP_ENTRY(reg_, 1, 1, desc_)
#define MAP_ENTRY_BASE_32(reg_, desc_) MAP_ENTRY(reg_, 0, 0, desc_)

enum {
	TOUUD_REG,
	TOLUD_REG,
	TSEG_REG,
	/* Must be last. */
	NUM_MAP_ENTRIES
};

static struct map_entry memory_map[NUM_MAP_ENTRIES] = {
		[TOUUD_REG] = MAP_ENTRY_BASE_64(TOUUD, "TOUUD"),
		[TOLUD_REG] = MAP_ENTRY_BASE_32(TOLUD, "TOLUD"),
		[TSEG_REG] = MAP_ENTRY_BASE_32(TSEGMB, "TSEGMB"),
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
}

static void mc_add_dram_resources(struct device *dev)
{
	unsigned long base_k, size_k;
	unsigned long touud_k;
	unsigned long index;
	struct resource *resource;
	uint64_t mc_values[NUM_MAP_ENTRIES];
	uintptr_t top_of_ram;

	/* Read in the MAP registers and report their values. */
	mc_read_map_entries(dev, &mc_values[0]);
	mc_report_map_entries(dev, &mc_values[0]);

	/*
	 * These are the host memory ranges that should be added:
	 * - 0 -> 0xa0000: cacheable
	 * - 0xc0000 -> 0x100000 : reserved
	 * - 0x100000 -> top_of_ram : cacheable
	 * - top_of_ram -> TSEG: uncacheable
	 * - TESG -> TOLUD: cacheable with standard MTRRs and reserved
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
	index = 0;
	top_of_ram = (uintptr_t)cbmem_top();

	/* 0 - > 0xa0000 */
	base_k = 0;
	size_k = (0xa0000 >> 10) - base_k;
	ram_resource(dev, index++, base_k, size_k);

	/* 0x100000 -> top_of_ram */
	base_k = 0x100000 >> 10;
	size_k = (top_of_ram >> 10) - base_k;
	ram_resource(dev, index++, base_k, size_k);

	/* top_of_ram -> TSEG */
	resource = new_resource(dev, index++);
	resource->base = top_of_ram;
	resource->size = mc_values[TSEG_REG] - resource->base;
	resource->flags = IORESOURCE_MEM | IORESOURCE_FIXED |
			  IORESOURCE_STORED | IORESOURCE_RESERVE |
			  IORESOURCE_ASSIGNED;

	/* TSEG -> TOLUD */
	resource = new_resource(dev, index++);
	resource->base = mc_values[TSEG_REG];
	resource->size = mc_values[TOLUD_REG] - resource->base;
	resource->flags = IORESOURCE_MEM | IORESOURCE_FIXED |
			  IORESOURCE_STORED | IORESOURCE_RESERVE |
			  IORESOURCE_ASSIGNED | IORESOURCE_CACHEABLE;
	printk(BIOS_DEBUG,
		"SMM memory location: 0x%llx  SMM memory size: 0x%llx\n",
		resource->base, resource->size);

	/* 4GiB -> TOUUD */
	base_k = 4096 * 1024; /* 4GiB */
	touud_k = mc_values[TOUUD_REG] >> 10;
	size_k = touud_k - base_k;
	if (touud_k > base_k)
		ram_resource(dev, index++, base_k, size_k);

	/*
	 * Reserve everything between A segment and 1MB:
	 *
	 * 0xa0000 - 0xbffff: legacy VGA
	 * 0xc0000 - 0xfffff: reserved RAM
	 */
	mmio_resource(dev, index++, (0xa0000 >> 10), (0xc0000 - 0xa0000) >> 10);
	reserved_ram_resource(dev, index++, (0xc0000 >> 10),
			      (0x100000 - 0xc0000) >> 10);
}

static void systemagent_read_resources(struct device *dev)
{
	/* Read standard PCI resources. */
	pci_dev_read_resources(dev);

	/* Add all fixed MMIO resources. */
	mc_add_fixed_mmio_resources(dev);

	/* Calculate and add DRAM resources. */
	mc_add_dram_resources(dev);
}

static void systemagent_init(struct device *dev)
{
	struct stopwatch sw;
	void *bios_reset_cpl =
		(void *)(DEFAULT_MCHBAR + MCH_BAR_BIOS_RESET_CPL);
	uint32_t reg = read32(bios_reset_cpl);

	/* Stage0 BIOS Reset Complete (RST_CPL) */
	reg |= RST_CPL_BIT;
	write32(bios_reset_cpl, reg);

	/*
	* Poll for bit 8 in same reg (RST_CPL).
	* We wait here till 1 ms for the bit to get set.
	*/
	stopwatch_init_msecs_expire(&sw, _1ms);
	while (!(read32(bios_reset_cpl) & PCODE_INIT_DONE)) {
		if (stopwatch_expired(&sw)) {
			printk(BIOS_DEBUG, "Failed to set RST_CPL bit\n");
			return;
		}
		udelay(WAITING_STEP);
	}
	printk(BIOS_DEBUG, "Set BIOS_RESET_CPL\n");
}

static struct device_operations systemagent_ops = {
	.read_resources = systemagent_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = systemagent_init,
	.ops_pci = &soc_pci_ops,
#if CONFIG(HAVE_ACPI_TABLES)
	.write_acpi_tables = systemagent_write_acpi_tables,
#endif
};

/* IDs for System Agent device of Intel Denverton SoC */
static const unsigned short systemagent_ids[] = {
	PCI_DID_INTEL_DNV_SA,
	PCI_DID_INTEL_DNVAD_SA,
	0
};

static const struct pci_driver systemagent_driver __pci_driver = {
	.ops = &systemagent_ops,
	.vendor = PCI_VID_INTEL,
	.devices = systemagent_ids
};
