/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Originally based on the Linux kernel (drivers/pci/pci.c).
 * PCI Bus Services, see include/linux/pci.h for further explanation.
 */

#include <acpi/acpi.h>
#include <device/pci_ops.h>
#include <bootmode.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <stdlib.h>
#include <string.h>
#include <delay.h>
#include <device/cardbus.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pcix.h>
#include <device/pciexp.h>
#include <lib.h>
#include <pc80/i8259.h>
#include <security/vboot/vbnv.h>
#include <timestamp.h>
#include <types.h>

u8 pci_moving_config8(struct device *dev, unsigned int reg)
{
	u8 value, ones, zeroes;

	value = pci_read_config8(dev, reg);

	pci_write_config8(dev, reg, 0xff);
	ones = pci_read_config8(dev, reg);

	pci_write_config8(dev, reg, 0x00);
	zeroes = pci_read_config8(dev, reg);

	pci_write_config8(dev, reg, value);

	return ones ^ zeroes;
}

u16 pci_moving_config16(struct device *dev, unsigned int reg)
{
	u16 value, ones, zeroes;

	value = pci_read_config16(dev, reg);

	pci_write_config16(dev, reg, 0xffff);
	ones = pci_read_config16(dev, reg);

	pci_write_config16(dev, reg, 0x0000);
	zeroes = pci_read_config16(dev, reg);

	pci_write_config16(dev, reg, value);

	return ones ^ zeroes;
}

u32 pci_moving_config32(struct device *dev, unsigned int reg)
{
	u32 value, ones, zeroes;

	value = pci_read_config32(dev, reg);

	pci_write_config32(dev, reg, 0xffffffff);
	ones = pci_read_config32(dev, reg);

	pci_write_config32(dev, reg, 0x00000000);
	zeroes = pci_read_config32(dev, reg);

	pci_write_config32(dev, reg, value);

	return ones ^ zeroes;
}

/**
 * Given a device and register, read the size of the BAR for that register.
 *
 * @param dev Pointer to the device structure.
 * @param index Address of the PCI configuration register.
 * @return TODO
 */
struct resource *pci_get_resource(struct device *dev, unsigned long index)
{
	struct resource *resource;
	unsigned long value, attr;
	resource_t moving, limit;

	/* Initialize the resources to nothing. */
	resource = new_resource(dev, index);

	/* Get the initial value. */
	value = pci_read_config32(dev, index);

	/* See which bits move. */
	moving = pci_moving_config32(dev, index);

	/* Initialize attr to the bits that do not move. */
	attr = value & ~moving;

	/* If it is a 64bit resource look at the high half as well. */
	if (((attr & PCI_BASE_ADDRESS_SPACE_IO) == 0) &&
	    ((attr & PCI_BASE_ADDRESS_MEM_LIMIT_MASK) ==
	     PCI_BASE_ADDRESS_MEM_LIMIT_64)) {
		/* Find the high bits that move. */
		moving |=
		    ((resource_t) pci_moving_config32(dev, index + 4)) << 32;
	}

	/* Find the resource constraints.
	 * Start by finding the bits that move. From there:
	 * - Size is the least significant bit of the bits that move.
	 * - Limit is all of the bits that move plus all of the lower bits.
	 * See PCI Spec 6.2.5.1.
	 */
	limit = 0;
	if (moving) {
		resource->size = 1;
		resource->align = resource->gran = 0;
		while (!(moving & resource->size)) {
			resource->size <<= 1;
			resource->align += 1;
			resource->gran += 1;
		}
		resource->limit = limit = moving | (resource->size - 1);

		if (pci_base_address_is_memory_space(attr)) {
			/* Page-align to allow individual mapping of devices. */
			if (resource->align < 12)
				resource->align = 12;
		}
	}

	/*
	 * Some broken hardware has read-only registers that do not
	 * really size correctly.
	 *
	 * Example: the Acer M7229 has BARs 1-4 normally read-only,
	 * so BAR1 at offset 0x10 reads 0x1f1. If you size that register
	 * by writing 0xffffffff to it, it will read back as 0x1f1 -- which
	 * is a violation of the spec.
	 *
	 * We catch this case and ignore it by observing which bits move.
	 *
	 * This also catches the common case of unimplemented registers
	 * that always read back as 0.
	 */
	if (moving == 0) {
		if (value != 0) {
			printk(BIOS_DEBUG, "%s register %02lx(%08lx), read-only ignoring it\n",
			       dev_path(dev), index, value);
		}
		resource->flags = 0;
	} else if (attr & PCI_BASE_ADDRESS_SPACE_IO) {
		/* An I/O mapped base address. */
		resource->flags |= IORESOURCE_IO;
		/* I don't want to deal with 32bit I/O resources. */
		resource->limit = 0xffff;
	} else {
		/* A Memory mapped base address. */
		attr &= PCI_BASE_ADDRESS_MEM_ATTR_MASK;
		resource->flags |= IORESOURCE_MEM;
		if (attr & PCI_BASE_ADDRESS_MEM_PREFETCH)
			resource->flags |= IORESOURCE_PREFETCH;
		attr &= PCI_BASE_ADDRESS_MEM_LIMIT_MASK;
		if (attr == PCI_BASE_ADDRESS_MEM_LIMIT_32) {
			/* 32bit limit. */
			resource->limit = 0xffffffffUL;
		} else if (attr == PCI_BASE_ADDRESS_MEM_LIMIT_1M) {
			/* 1MB limit. */
			resource->limit = 0x000fffffUL;
		} else if (attr == PCI_BASE_ADDRESS_MEM_LIMIT_64) {
			/* 64bit limit. */
			resource->limit = 0xffffffffffffffffULL;
			resource->flags |= IORESOURCE_PCI64;
		} else {
			/* Invalid value. */
			printk(BIOS_ERR, "Broken BAR with value %lx\n", attr);
			printk(BIOS_ERR, " on dev %s at index %02lx\n",
			       dev_path(dev), index);
			resource->flags = 0;
		}
	}

	/* Don't let the limit exceed which bits can move. */
	if (resource->limit > limit)
		resource->limit = limit;

	return resource;
}

/**
 * Given a device and an index, read the size of the BAR for that register.
 *
 * @param dev Pointer to the device structure.
 * @param index Address of the PCI configuration register.
 */
static void pci_get_rom_resource(struct device *dev, unsigned long index)
{
	struct resource *resource;
	unsigned long value;
	resource_t moving;

	/* Initialize the resources to nothing. */
	resource = new_resource(dev, index);

	/* Get the initial value. */
	value = pci_read_config32(dev, index);

	/* See which bits move. */
	moving = pci_moving_config32(dev, index);

	/* Clear the Enable bit. */
	moving = moving & ~PCI_ROM_ADDRESS_ENABLE;

	/* Find the resource constraints.
	 * Start by finding the bits that move. From there:
	 * - Size is the least significant bit of the bits that move.
	 * - Limit is all of the bits that move plus all of the lower bits.
	 * See PCI Spec 6.2.5.1.
	 */
	if (moving) {
		resource->size = 1;
		resource->align = resource->gran = 0;
		while (!(moving & resource->size)) {
			resource->size <<= 1;
			resource->align += 1;
			resource->gran += 1;
		}
		resource->limit = moving | (resource->size - 1);
		resource->flags |= IORESOURCE_MEM | IORESOURCE_READONLY;
	} else {
		if (value != 0) {
			printk(BIOS_DEBUG, "%s register %02lx(%08lx), read-only ignoring it\n",
			       dev_path(dev), index, value);
		}
		resource->flags = 0;
	}
	compact_resources(dev);
}

/**
 * Given a device, read the size of the MSI-X table.
 *
 * @param dev Pointer to the device structure.
 * @return MSI-X table size or 0 if not MSI-X capable device
 */
size_t pci_msix_table_size(struct device *dev)
{
	const size_t pos = pci_find_capability(dev, PCI_CAP_ID_MSIX);
	if (!pos)
		return 0;

	const u16 control = pci_read_config16(dev, pos + PCI_MSIX_FLAGS);
	return (control & PCI_MSIX_FLAGS_QSIZE) + 1;
}

/**
 * Given a device, return the table offset and bar the MSI-X tables resides in.
 *
 * @param dev Pointer to the device structure.
 * @param offset Returned value gives the offset in bytes inside the PCI BAR.
 * @param idx The returned value is the index of the PCI_BASE_ADDRESS register
 *            the MSI-X table is located in.
 * @return Zero on success
 */
int pci_msix_table_bar(struct device *dev, u32 *offset, u8 *idx)
{
	const size_t pos = pci_find_capability(dev, PCI_CAP_ID_MSIX);
	if (!pos || !offset || !idx)
		return 1;

	*offset = pci_read_config32(dev, pos + PCI_MSIX_TABLE);
	*idx = (u8)(*offset & PCI_MSIX_PBA_BIR);
	*offset &= PCI_MSIX_PBA_OFFSET;

	return 0;
}

/**
 * Given a device, return a msix_entry pointer or NULL if no table was found.
 *
 * @param dev Pointer to the device structure.
 *
 * @return NULL on error
 */
struct msix_entry *pci_msix_get_table(struct device *dev)
{
	struct resource *res;
	u32 offset;
	u8 idx;

	if (pci_msix_table_bar(dev, &offset, &idx))
		return NULL;

	if (idx > 5)
		return NULL;

	res = probe_resource(dev, idx * 4 + PCI_BASE_ADDRESS_0);
	if (!res || !res->base || offset >= res->size)
		return NULL;

	if ((res->flags & IORESOURCE_PCI64) &&
	    (uintptr_t)res->base != res->base)
		return NULL;

	return (struct msix_entry *)((uintptr_t)res->base + offset);
}

static unsigned int get_rebar_offset(const struct device *dev, unsigned long index)
{
	uint32_t offset = pciexp_find_extended_cap(dev, PCIE_EXT_CAP_RESIZABLE_BAR);
	if (!offset)
		return 0;

	/* Convert PCI_BASE_ADDRESS_0, ..._1, ..._2 into 0, 1, 2... */
	const unsigned int find_bar_idx = (index - PCI_BASE_ADDRESS_0) /
		sizeof(uint32_t);

	/* Although all of the Resizable BAR Control Registers contain an
	   "NBARs" field, it is only valid in the Control Register for BAR 0 */
	const uint32_t rebar_ctrl0 = pci_read_config32(dev, offset + PCI_REBAR_CTRL_OFFSET);
	const unsigned int nbars = (rebar_ctrl0 & PCI_REBAR_CTRL_NBARS_MASK) >>
		PCI_REBAR_CTRL_NBARS_SHIFT;

	for (unsigned int i = 0; i < nbars; i++, offset += sizeof(uint64_t)) {
		const uint32_t rebar_ctrl = pci_read_config32(
			dev, offset + PCI_REBAR_CTRL_OFFSET);
		const uint32_t bar_idx = rebar_ctrl & PCI_REBAR_CTRL_IDX_MASK;
		if (bar_idx == find_bar_idx)
			return offset;
	}

	return 0;
}

/* Bit 20 = 1 MiB, bit 21 = 2 MiB, bit 22 = 4 MiB, ... bit 63 = 8 EiB */
static uint64_t get_rebar_sizes_mask(const struct device *dev,
				     unsigned long index)
{
	uint64_t size_mask = 0ULL;
	const uint32_t offset = get_rebar_offset(dev, index);
	if (!offset)
		return 0;

	/* Get 1 MB - 128 TB support from CAP register */
	const uint32_t cap = pci_read_config32(dev, offset + PCI_REBAR_CAP_OFFSET);
	/* Shift the bits from 4-31 to 0-27 (i.e., down by 4 bits) */
	size_mask |= ((cap & PCI_REBAR_CAP_SIZE_MASK) >> 4);

	/* Get 256 TB - 8 EB support from CTRL register and store it in bits 28-43 */
	const uint64_t ctrl = pci_read_config32(dev, offset + PCI_REBAR_CTRL_OFFSET);
	/* Shift ctrl mask from bit 16 to bit 28, so that the two
	   masks (fom cap and ctrl) form a contiguous bitmask when
	   concatenated (i.e., up by 12 bits). */
	size_mask |= ((ctrl & PCI_REBAR_CTRL_SIZE_MASK) << 12);

	/* Now that the mask occupies bits 0-43, shift it up to 20-63, so they
	   represent the actual powers of 2. */
	return size_mask << 20;
}

static void pci_store_rebar_size(const struct device *dev,
				 const struct resource *resource)
{
	const unsigned int num_bits = __fls64(resource->size);
	const uint32_t offset = get_rebar_offset(dev, resource->index);
	if (!offset)
		return;

	pci_update_config32(dev, offset + PCI_REBAR_CTRL_OFFSET,
			    ~PCI_REBAR_CTRL_SIZE_MASK,
			    num_bits << PCI_REBAR_CTRL_SIZE_SHIFT);
}

static void configure_adjustable_base(const struct device *dev,
				      unsigned long index,
				      struct resource *res)
{
	/*
	 * Excerpt from an implementation note from the PCIe spec:
	 *
	 * System software uses this capability in place of the above mentioned
	 * method of determining the resource size[0], and prior to assigning
	 * the base address to the BAR. Potential usable resource sizes are
	 * reported by the Function via its Resizable BAR Capability and Control
	 * registers. It is intended that the software allocate the largest of
	 * the reported sizes that it can, since allocating less address space
	 * than the largest reported size can result in lower
	 * performance. Software then writes the size to the Resizable BAR
	 * Control register for the appropriate BAR for the Function. Following
	 * this, the base address is written to the BAR.
	 *
	 * [0] Referring to using the moving bits in the BAR to determine the
	 *     requested size of the MMIO region
	 */
	const uint64_t size_mask = get_rebar_sizes_mask(dev, index);
	if (!size_mask)
		return;

	int max_requested_bits = __fls64(size_mask);
	if (max_requested_bits > CONFIG_PCIEXP_DEFAULT_MAX_RESIZABLE_BAR_BITS) {
		printk(BIOS_WARNING, "WARNING: Device %s requests a BAR with"
		       "%u bits of address space, which coreboot is not"
		       "configured to hand out, truncating to %u bits\n",
		       dev_path(dev), max_requested_bits,
		       CONFIG_PCIEXP_DEFAULT_MAX_RESIZABLE_BAR_BITS);
		max_requested_bits = CONFIG_PCIEXP_DEFAULT_MAX_RESIZABLE_BAR_BITS;
	}

	if (!(res->flags & IORESOURCE_PCI64) && max_requested_bits > 32) {
		printk(BIOS_ERR, "ERROR: Resizable BAR requested"
		       "above 32 bits, but PCI function reported a"
		       "32-bit BAR.");
		return;
	}

	/* Configure the resource parameters for the adjustable BAR */
	res->size = 1ULL << max_requested_bits;
	res->align = max_requested_bits;
	res->gran = max_requested_bits;
	res->limit = (res->flags & IORESOURCE_PCI64) ? UINT64_MAX : UINT32_MAX;
	res->flags |= IORESOURCE_PCIE_RESIZABLE_BAR;

	printk(BIOS_INFO, "%s: Adjusting resource index %lu: base: %llx size: %llx "
	       "align: %d gran: %d limit: %llx\n",
	       dev_path(dev), res->index, res->base, res->size,
	       res->align, res->gran, res->limit);
}

/**
 * Read the base address registers for a given device.
 *
 * @param dev Pointer to the dev structure.
 * @param howmany How many registers to read (6 for device, 2 for bridge).
 */
static void pci_read_bases(struct device *dev, unsigned int howmany)
{
	unsigned long index;

	for (index = PCI_BASE_ADDRESS_0;
	     (index < PCI_BASE_ADDRESS_0 + (howmany << 2));) {
		struct resource *resource;
		resource = pci_get_resource(dev, index);

		const bool is_pcie = pci_find_capability(dev, PCI_CAP_ID_PCIE) != 0;
		if (CONFIG(PCIEXP_SUPPORT_RESIZABLE_BARS) && is_pcie)
			configure_adjustable_base(dev, index, resource);

		index += (resource->flags & IORESOURCE_PCI64) ? 8 : 4;
	}

	compact_resources(dev);
}

static void pci_record_bridge_resource(struct device *dev, resource_t moving,
				       unsigned int index, unsigned long type)
{
	struct resource *resource;
	unsigned long gran;
	resource_t step;

	resource = NULL;

	if (!moving)
		return;

	/* Initialize the constraints on the current bus. */
	resource = new_resource(dev, index);
	resource->size = 0;
	gran = 0;
	step = 1;
	while ((moving & step) == 0) {
		gran += 1;
		step <<= 1;
	}
	resource->gran = gran;
	resource->align = gran;
	resource->limit = moving | (step - 1);
	resource->flags = type | IORESOURCE_PCI_BRIDGE |
			  IORESOURCE_BRIDGE;
}

static void pci_bridge_read_bases(struct device *dev)
{
	resource_t moving_base, moving_limit, moving;

	/* See if the bridge I/O resources are implemented. */
	moving_base = ((u32) pci_moving_config8(dev, PCI_IO_BASE)) << 8;
	moving_base |=
	  ((u32) pci_moving_config16(dev, PCI_IO_BASE_UPPER16)) << 16;

	moving_limit = ((u32) pci_moving_config8(dev, PCI_IO_LIMIT)) << 8;
	moving_limit |=
	  ((u32) pci_moving_config16(dev, PCI_IO_LIMIT_UPPER16)) << 16;

	moving = moving_base & moving_limit;

	/* Initialize the I/O space constraints on the current bus. */
	pci_record_bridge_resource(dev, moving, PCI_IO_BASE, IORESOURCE_IO);

	/* See if the bridge prefmem resources are implemented. */
	moving_base =
	  ((resource_t) pci_moving_config16(dev, PCI_PREF_MEMORY_BASE)) << 16;
	moving_base |=
	  ((resource_t) pci_moving_config32(dev, PCI_PREF_BASE_UPPER32)) << 32;

	moving_limit =
	  ((resource_t) pci_moving_config16(dev, PCI_PREF_MEMORY_LIMIT)) << 16;
	moving_limit |=
	  ((resource_t) pci_moving_config32(dev, PCI_PREF_LIMIT_UPPER32)) << 32;

	moving = moving_base & moving_limit;
	/* Initialize the prefetchable memory constraints on the current bus. */
	pci_record_bridge_resource(dev, moving, PCI_PREF_MEMORY_BASE,
				   IORESOURCE_MEM | IORESOURCE_PREFETCH);

	/* See if the bridge mem resources are implemented. */
	moving_base = ((u32) pci_moving_config16(dev, PCI_MEMORY_BASE)) << 16;
	moving_limit = ((u32) pci_moving_config16(dev, PCI_MEMORY_LIMIT)) << 16;

	moving = moving_base & moving_limit;

	/* Initialize the memory resources on the current bus. */
	pci_record_bridge_resource(dev, moving, PCI_MEMORY_BASE,
				   IORESOURCE_MEM);

	compact_resources(dev);
}

void pci_dev_read_resources(struct device *dev)
{
	pci_read_bases(dev, 6);
	pci_get_rom_resource(dev, PCI_ROM_ADDRESS);
}

void pci_bus_read_resources(struct device *dev)
{
	pci_bridge_read_bases(dev);
	pci_read_bases(dev, 2);
	pci_get_rom_resource(dev, PCI_ROM_ADDRESS1);
}

void pci_domain_read_resources(struct device *dev)
{
	struct resource *res;

	/* Initialize the system-wide I/O space constraints. */
	res = new_resource(dev, IOINDEX_SUBTRACTIVE(0, 0));
	res->limit = 0xffffUL;
	res->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE |
		     IORESOURCE_ASSIGNED;

	/* Initialize the system-wide memory resources constraints. */
	res = new_resource(dev, IOINDEX_SUBTRACTIVE(1, 0));
	res->limit = (1ULL << cpu_phys_address_size()) - 1;
	res->flags = IORESOURCE_MEM | IORESOURCE_SUBTRACTIVE |
		     IORESOURCE_ASSIGNED;
}

void pci_domain_set_resources(struct device *dev)
{
	assign_resources(dev->link_list);
}

static void pci_store_resource(const struct device *const dev,
			       const struct resource *const resource)
{
	unsigned long base_lo, base_hi;

	base_lo = resource->base & 0xffffffff;
	base_hi = (resource->base >> 32) & 0xffffffff;

	/*
	 * Some chipsets allow us to set/clear the I/O bit
	 * (e.g. VIA 82C686A). So set it to be safe.
	 */
	if (resource->flags & IORESOURCE_IO)
		base_lo |= PCI_BASE_ADDRESS_SPACE_IO;

	pci_write_config32(dev, resource->index, base_lo);
	if (resource->flags & IORESOURCE_PCI64)
		pci_write_config32(dev, resource->index + 4, base_hi);
}

static void pci_store_bridge_resource(const struct device *const dev,
				      struct resource *const resource)
{
	resource_t base, end;

	/*
	 * PCI bridges have no enable bit. They are disabled if the base of
	 * the range is greater than the limit. If the size is zero, disable
	 * by setting the base = limit and end = limit - 2^gran.
	 */
	if (resource->size == 0) {
		base = resource->limit;
		end = resource->limit - (1 << resource->gran);
		resource->base = base;
	} else {
		base = resource->base;
		end = resource_end(resource);
	}

	if (resource->index == PCI_IO_BASE) {
		/* Set the I/O ranges. */
		pci_write_config8(dev, PCI_IO_BASE, base >> 8);
		pci_write_config16(dev, PCI_IO_BASE_UPPER16, base >> 16);
		pci_write_config8(dev, PCI_IO_LIMIT, end >> 8);
		pci_write_config16(dev, PCI_IO_LIMIT_UPPER16, end >> 16);
	} else if (resource->index == PCI_MEMORY_BASE) {
		/* Set the memory range. */
		pci_write_config16(dev, PCI_MEMORY_BASE, base >> 16);
		pci_write_config16(dev, PCI_MEMORY_LIMIT, end >> 16);
	} else if (resource->index == PCI_PREF_MEMORY_BASE) {
		/* Set the prefetchable memory range. */
		pci_write_config16(dev, PCI_PREF_MEMORY_BASE, base >> 16);
		pci_write_config32(dev, PCI_PREF_BASE_UPPER32, base >> 32);
		pci_write_config16(dev, PCI_PREF_MEMORY_LIMIT, end >> 16);
		pci_write_config32(dev, PCI_PREF_LIMIT_UPPER32, end >> 32);
	} else {
		/* Don't let me think I stored the resource. */
		resource->flags &= ~IORESOURCE_STORED;
		printk(BIOS_ERR, "invalid resource->index %lx\n", resource->index);
	}
}

static void pci_set_resource(struct device *dev, struct resource *resource)
{
	/* Make certain the resource has actually been assigned a value. */
	if (!(resource->flags & IORESOURCE_ASSIGNED)) {
		if (resource->flags & IORESOURCE_BRIDGE) {
			/* If a bridge resource has no value assigned,
			   we can treat it like an empty resource. */
			resource->size = 0;
		} else {
			printk(BIOS_ERR, "%s %02lx %s size: 0x%010llx not assigned\n",
			       dev_path(dev), resource->index,
			       resource_type(resource), resource->size);
			return;
		}
	}

	/* If this resource is fixed don't worry about it. */
	if (resource->flags & IORESOURCE_FIXED)
		return;

	/* If I have already stored this resource don't worry about it. */
	if (resource->flags & IORESOURCE_STORED)
		return;

	/* If the resource is subtractive don't worry about it. */
	if (resource->flags & IORESOURCE_SUBTRACTIVE)
		return;

	/* Only handle PCI memory and I/O resources for now. */
	if (!(resource->flags & (IORESOURCE_MEM | IORESOURCE_IO)))
		return;

	/* Enable the resources in the command register. */
	if (resource->size) {
		if (resource->flags & IORESOURCE_MEM)
			dev->command |= PCI_COMMAND_MEMORY;
		if (resource->flags & IORESOURCE_IO)
			dev->command |= PCI_COMMAND_IO;
		if (resource->flags & IORESOURCE_PCI_BRIDGE &&
		    CONFIG(PCI_SET_BUS_MASTER_PCI_BRIDGES))
			dev->command |= PCI_COMMAND_MASTER;
	}

	/* Now store the resource. */
	resource->flags |= IORESOURCE_STORED;

	if (!(resource->flags & IORESOURCE_PCI_BRIDGE)) {
		if (CONFIG(PCIEXP_SUPPORT_RESIZABLE_BARS) &&
		    (resource->flags & IORESOURCE_PCIE_RESIZABLE_BAR))
			pci_store_rebar_size(dev, resource);

		pci_store_resource(dev, resource);

	} else {
		pci_store_bridge_resource(dev, resource);
	}

	report_resource_stored(dev, resource, "");
}

void pci_dev_set_resources(struct device *dev)
{
	struct resource *res;
	struct bus *bus;
	u8 line;

	for (res = dev->resource_list; res; res = res->next)
		pci_set_resource(dev, res);

	for (bus = dev->link_list; bus; bus = bus->next) {
		if (bus->children)
			assign_resources(bus);
	}

	/* Set a default latency timer. */
	pci_write_config8(dev, PCI_LATENCY_TIMER, 0x40);

	/* Set a default secondary latency timer. */
	if ((dev->hdr_type & 0x7f) == PCI_HEADER_TYPE_BRIDGE)
		pci_write_config8(dev, PCI_SEC_LATENCY_TIMER, 0x40);

	/* Zero the IRQ settings. */
	line = pci_read_config8(dev, PCI_INTERRUPT_PIN);
	if (line)
		pci_write_config8(dev, PCI_INTERRUPT_LINE, 0);

	/* Set the cache line size, so far 64 bytes is good for everyone. */
	pci_write_config8(dev, PCI_CACHE_LINE_SIZE, 64 >> 2);
}

void pci_dev_enable_resources(struct device *dev)
{
	const struct pci_operations *ops = NULL;
	u16 command;

	/* Set the subsystem vendor and device ID for mainboard devices. */
	if (dev->ops)
		ops = dev->ops->ops_pci;
	if (dev->on_mainboard && ops && ops->set_subsystem) {
		if (CONFIG_SUBSYSTEM_VENDOR_ID)
			dev->subsystem_vendor = CONFIG_SUBSYSTEM_VENDOR_ID;
		else if (!dev->subsystem_vendor)
			dev->subsystem_vendor = pci_read_config16(dev,
							PCI_VENDOR_ID);
		if (CONFIG_SUBSYSTEM_DEVICE_ID)
			dev->subsystem_device = CONFIG_SUBSYSTEM_DEVICE_ID;
		else if (!dev->subsystem_device)
			dev->subsystem_device = pci_read_config16(dev,
							PCI_DEVICE_ID);

		printk(BIOS_DEBUG, "%s subsystem <- %04x/%04x\n",
			dev_path(dev), dev->subsystem_vendor,
			dev->subsystem_device);
		ops->set_subsystem(dev, dev->subsystem_vendor,
			dev->subsystem_device);
	}
	command = pci_read_config16(dev, PCI_COMMAND);
	command |= dev->command;

	/* v3 has
	 * command |= (PCI_COMMAND_PARITY + PCI_COMMAND_SERR);	// Error check.
	 */

	printk(BIOS_DEBUG, "%s cmd <- %02x\n", dev_path(dev), command);
	pci_write_config16(dev, PCI_COMMAND, command);
}

void pci_bus_enable_resources(struct device *dev)
{
	u16 ctrl;

	/*
	 * Enable I/O in command register if there is VGA card
	 * connected with (even it does not claim I/O resource).
	 */
	if (dev->link_list->bridge_ctrl & PCI_BRIDGE_CTL_VGA)
		dev->command |= PCI_COMMAND_IO;
	ctrl = pci_read_config16(dev, PCI_BRIDGE_CONTROL);
	ctrl |= dev->link_list->bridge_ctrl;
	ctrl |= (PCI_BRIDGE_CTL_PARITY | PCI_BRIDGE_CTL_SERR); /* Error check. */
	printk(BIOS_DEBUG, "%s bridge ctrl <- %04x\n", dev_path(dev), ctrl);
	pci_write_config16(dev, PCI_BRIDGE_CONTROL, ctrl);

	pci_dev_enable_resources(dev);
}

void pci_bus_reset(struct bus *bus)
{
	u16 ctl;

	ctl = pci_read_config16(bus->dev, PCI_BRIDGE_CONTROL);
	ctl |= PCI_BRIDGE_CTL_BUS_RESET;
	pci_write_config16(bus->dev, PCI_BRIDGE_CONTROL, ctl);
	mdelay(10);

	ctl &= ~PCI_BRIDGE_CTL_BUS_RESET;
	pci_write_config16(bus->dev, PCI_BRIDGE_CONTROL, ctl);
	delay(1);
}

void pci_dev_set_subsystem(struct device *dev, unsigned int vendor,
			   unsigned int device)
{
	uint8_t offset;

	/* Header type */
	switch (dev->hdr_type & 0x7f) {
	case PCI_HEADER_TYPE_NORMAL:
		offset = PCI_SUBSYSTEM_VENDOR_ID;
		break;
	case PCI_HEADER_TYPE_BRIDGE:
		offset = pci_find_capability(dev, PCI_CAP_ID_SSVID);
		if (!offset)
			return;
		offset += 4; /* Vendor ID at offset 4 */
		break;
	case PCI_HEADER_TYPE_CARDBUS:
		offset = PCI_CB_SUBSYSTEM_VENDOR_ID;
		break;
	default:
		return;
	}

	if (!vendor || !device) {
		pci_write_config32(dev, offset,
			pci_read_config32(dev, PCI_VENDOR_ID));
	} else {
		pci_write_config32(dev, offset,
			((device & 0xffff) << 16) | (vendor & 0xffff));
	}
}

static int should_run_oprom(struct device *dev, struct rom_header *rom)
{
	static int should_run = -1;

	if (CONFIG(VENDORCODE_ELTAN_VBOOT))
		if (rom != NULL)
			if (!verified_boot_should_run_oprom(rom))
				return 0;

	if (should_run >= 0)
		return should_run;

	if (CONFIG(ALWAYS_RUN_OPROM)) {
		should_run = 1;
		return should_run;
	}

	/* Don't run VGA option ROMs, unless we have to print
	 * something on the screen before the kernel is loaded.
	 */
	should_run = display_init_required();

	if (!should_run)
		printk(BIOS_DEBUG, "Not running VGA Option ROM\n");
	return should_run;
}

static int should_load_oprom(struct device *dev)
{
	/* If S3_VGA_ROM_RUN is disabled, skip running VGA option
	 * ROMs when coming out of an S3 resume.
	 */
	if (!CONFIG(S3_VGA_ROM_RUN) && acpi_is_wakeup_s3() &&
		((dev->class >> 8) == PCI_CLASS_DISPLAY_VGA))
		return 0;
	if (CONFIG(ALWAYS_LOAD_OPROM))
		return 1;
	if (should_run_oprom(dev, NULL))
		return 1;

	return 0;
}

static void oprom_pre_graphics_stall(void)
{
	if (CONFIG_PRE_GRAPHICS_DELAY_MS)
		mdelay(CONFIG_PRE_GRAPHICS_DELAY_MS);
}

/** Default handler: only runs the relevant PCI BIOS. */
void pci_dev_init(struct device *dev)
{
	struct rom_header *rom, *ram;

	if (!CONFIG(VGA_ROM_RUN))
		return;

	/* Only execute VGA ROMs. */
	if (((dev->class >> 8) != PCI_CLASS_DISPLAY_VGA))
		return;

	if (!should_load_oprom(dev))
		return;
	timestamp_add_now(TS_OPROM_INITIALIZE);

	rom = pci_rom_probe(dev);
	if (rom == NULL)
		return;

	ram = pci_rom_load(dev, rom);
	if (ram == NULL)
		return;
	timestamp_add_now(TS_OPROM_COPY_END);

	if (!should_run_oprom(dev, rom))
		return;

	/* Wait for any configured pre-graphics delay */
	oprom_pre_graphics_stall();

	run_bios(dev, (unsigned long)ram);

	gfx_set_init_done(1);
	printk(BIOS_DEBUG, "VGA Option ROM was run\n");
	timestamp_add_now(TS_OPROM_END);
}

/** Default device operation for PCI devices */
struct pci_operations pci_dev_ops_pci = {
	.set_subsystem = pci_dev_set_subsystem,
};

struct device_operations default_pci_ops_dev = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
#if CONFIG(HAVE_ACPI_TABLES)
	.write_acpi_tables = pci_rom_write_acpi_tables,
	.acpi_fill_ssdt    = pci_rom_ssdt,
#endif
	.init             = pci_dev_init,
	.ops_pci          = &pci_dev_ops_pci,
};

/** Default device operations for PCI bridges */
struct device_operations default_pci_ops_bus = {
	.read_resources   = pci_bus_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.scan_bus         = pci_scan_bridge,
	.reset_bus        = pci_bus_reset,
};

/** Default device operations for PCI devices marked 'hidden' */
static struct device_operations default_hidden_pci_ops_dev = {
	.read_resources   = noop_read_resources,
	.set_resources    = noop_set_resources,
	.scan_bus         = scan_static_bus,
};

/**
 * Check for compatibility to route legacy VGA cycles through a bridge.
 *
 * Originally, when decoding i/o ports for legacy VGA cycles, bridges
 * should only consider the 10 least significant bits of the port address.
 * This means all VGA registers were aliased every 1024 ports!
 *     e.g. 0x3b0 was also decoded as 0x7b0, 0xbb0 etc.
 *
 * To avoid this mess, a bridge control bit (VGA16) was introduced in
 * 2003 to enable decoding of 16-bit port addresses. As we don't want
 * to make this any more complex for now, we use this bit if possible
 * and only warn if it's not supported (in set_vga_bridge_bits()).
 */
static void pci_bridge_vga_compat(struct bus *const bus)
{
	uint16_t bridge_ctrl;

	bridge_ctrl = pci_read_config16(bus->dev, PCI_BRIDGE_CONTROL);

	/* Ensure VGA decoding is disabled during probing (it should
	   be by default, but we run blobs nowadays) */
	bridge_ctrl &= ~PCI_BRIDGE_CTL_VGA;
	pci_write_config16(bus->dev, PCI_BRIDGE_CONTROL, bridge_ctrl);

	/* If the upstream bridge doesn't support VGA16, we don't have to check */
	bus->no_vga16 |= bus->dev->bus->no_vga16;
	if (bus->no_vga16)
		return;

	/* Test if we can enable 16-bit decoding */
	bridge_ctrl |= PCI_BRIDGE_CTL_VGA16;
	pci_write_config16(bus->dev, PCI_BRIDGE_CONTROL, bridge_ctrl);
	bridge_ctrl = pci_read_config16(bus->dev, PCI_BRIDGE_CONTROL);

	bus->no_vga16 = !(bridge_ctrl & PCI_BRIDGE_CTL_VGA16);
}

/**
 * Detect the type of downstream bridge.
 *
 * This function is a heuristic to detect which type of bus is downstream
 * of a PCI-to-PCI bridge. This functions by looking for various capability
 * blocks to figure out the type of downstream bridge. PCI-X, PCI-E, and
 * Hypertransport all seem to have appropriate capabilities.
 *
 * When only a PCI-Express capability is found the type is examined to see
 * which type of bridge we have.
 *
 * @param dev Pointer to the device structure of the bridge.
 * @return Appropriate bridge operations.
 */
static struct device_operations *get_pci_bridge_ops(struct device *dev)
{
#if CONFIG(PCIX_PLUGIN_SUPPORT)
	unsigned int pcixpos;
	pcixpos = pci_find_capability(dev, PCI_CAP_ID_PCIX);
	if (pcixpos) {
		printk(BIOS_DEBUG, "%s subordinate bus PCI-X\n", dev_path(dev));
		return &default_pcix_ops_bus;
	}
#endif
#if CONFIG(PCIEXP_PLUGIN_SUPPORT)
	unsigned int pciexpos;
	pciexpos = pci_find_capability(dev, PCI_CAP_ID_PCIE);
	if (pciexpos) {
		u16 flags;
		flags = pci_read_config16(dev, pciexpos + PCI_EXP_FLAGS);
		switch ((flags & PCI_EXP_FLAGS_TYPE) >> 4) {
		case PCI_EXP_TYPE_ROOT_PORT:
		case PCI_EXP_TYPE_UPSTREAM:
		case PCI_EXP_TYPE_DOWNSTREAM:
			printk(BIOS_DEBUG, "%s subordinate bus PCI Express\n",
			       dev_path(dev));
			if (CONFIG(PCIEXP_HOTPLUG)) {
				u16 sltcap;
				sltcap = pci_read_config16(dev, pciexpos + PCI_EXP_SLTCAP);
				if (sltcap & PCI_EXP_SLTCAP_HPC) {
					printk(BIOS_DEBUG, "%s hot-plug capable\n",
					       dev_path(dev));
					return &default_pciexp_hotplug_ops_bus;
				}
			}
			return &default_pciexp_ops_bus;
		case PCI_EXP_TYPE_PCI_BRIDGE:
			printk(BIOS_DEBUG, "%s subordinate PCI\n",
			       dev_path(dev));
			return &default_pci_ops_bus;
		default:
			break;
		}
	}
#endif
	return &default_pci_ops_bus;
}

/**
 * Check if a device id matches a PCI driver entry.
 *
 * The driver entry can either point at a zero terminated array of acceptable
 * device IDs, or include a single device ID.
 *
 * @param driver pointer to the PCI driver entry being checked
 * @param device_id PCI device ID of the device being matched
 */
static int device_id_match(struct pci_driver *driver, unsigned short device_id)
{
	if (driver->devices) {
		unsigned short check_id;
		const unsigned short *device_list = driver->devices;
		while ((check_id = *device_list++) != 0)
			if (check_id == device_id)
				return 1;
	}

	return (driver->device == device_id);
}

/**
 * Set up PCI device operation.
 *
 * Check if it already has a driver. If not, use find_device_operations(),
 * or set to a default based on type.
 *
 * @param dev Pointer to the device whose pci_ops you want to set.
 * @see pci_drivers
 */
static void set_pci_ops(struct device *dev)
{
	struct pci_driver *driver;

	if (dev->ops)
		return;

	/*
	 * Look through the list of setup drivers and find one for
	 * this PCI device.
	 */
	for (driver = &_pci_drivers[0]; driver != &_epci_drivers[0]; driver++) {
		if ((driver->vendor == dev->vendor) &&
		    device_id_match(driver, dev->device)) {
			dev->ops = (struct device_operations *)driver->ops;
			break;
		}
	}

	if (dev->ops) {
		printk(BIOS_SPEW, "%s [%04x/%04x] %sops\n", dev_path(dev),
		       driver->vendor, driver->device, (driver->ops->scan_bus ? "bus " : ""));
		return;
	}

	/* If I don't have a specific driver use the default operations. */
	switch (dev->hdr_type & 0x7f) {	/* Header type */
	case PCI_HEADER_TYPE_NORMAL:
		if ((dev->class >> 8) == PCI_CLASS_BRIDGE_PCI)
			goto bad;
		dev->ops = &default_pci_ops_dev;
		break;
	case PCI_HEADER_TYPE_BRIDGE:
		if ((dev->class >> 8) != PCI_CLASS_BRIDGE_PCI)
			goto bad;
		dev->ops = get_pci_bridge_ops(dev);
		break;
#if CONFIG(CARDBUS_PLUGIN_SUPPORT)
	case PCI_HEADER_TYPE_CARDBUS:
		dev->ops = &default_cardbus_ops_bus;
		break;
#endif
	default:
bad:
		if (dev->enabled) {
			printk(BIOS_ERR,
			       "%s [%04x/%04x/%06x] has unknown header type %02x, ignoring.\n",
			       dev_path(dev), dev->vendor, dev->device,
			       dev->class >> 8, dev->hdr_type);
		}
	}
}

/**
 * See if we have already allocated a device structure for a given devfn.
 *
 * Given a PCI bus structure and a devfn number, find the device structure
 * corresponding to the devfn, if present. Then move the device structure
 * as the last child on the bus.
 *
 * @param bus Pointer to the bus structure.
 * @param devfn A device/function number.
 * @return Pointer to the device structure found or NULL if we have not
 *	   allocated a device for this devfn yet.
 */
static struct device *pci_scan_get_dev(struct bus *bus, unsigned int devfn)
{
	struct device *dev, **prev;

	prev = &bus->children;
	for (dev = bus->children; dev; dev = dev->sibling) {
		if (dev->path.type == DEVICE_PATH_PCI && dev->path.pci.devfn == devfn) {
			/* Unlink from the list. */
			*prev = dev->sibling;
			dev->sibling = NULL;
			break;
		}
		prev = &dev->sibling;
	}

	/*
	 * Just like alloc_dev() add the device to the list of devices on the
	 * bus. When the list of devices was formed we removed all of the
	 * parents children, and now we are interleaving static and dynamic
	 * devices in order on the bus.
	 */
	if (dev) {
		struct device *child;

		/* Find the last child on the bus. */
		for (child = bus->children; child && child->sibling;)
			child = child->sibling;

		/* Place the device as last on the bus. */
		if (child)
			child->sibling = dev;
		else
			bus->children = dev;
	}

	return dev;
}

/**
 * Scan a PCI bus.
 *
 * Determine the existence of a given PCI device. Allocate a new struct device
 * if dev==NULL was passed in and the device exists in hardware.
 *
 * @param dev Pointer to the dev structure.
 * @param bus Pointer to the bus structure.
 * @param devfn A device/function number to look at.
 * @return The device structure for the device (if found), NULL otherwise.
 */
struct device *pci_probe_dev(struct device *dev, struct bus *bus,
				unsigned int devfn)
{
	u32 id, class;
	u8 hdr_type;

	/* Detect if a device is present. */
	if (!dev) {
		struct device dummy;

		dummy.bus = bus;
		dummy.path.type = DEVICE_PATH_PCI;
		dummy.path.pci.devfn = devfn;

		id = pci_read_config32(&dummy, PCI_VENDOR_ID);
		/*
		 * Have we found something? Some broken boards return 0 if a
		 * slot is empty, but the expected answer is 0xffffffff.
		 */
		if (id == 0xffffffff)
			return NULL;

		if ((id == 0x00000000) || (id == 0x0000ffff) ||
		    (id == 0xffff0000)) {
			printk(BIOS_SPEW, "%s, bad id 0x%x\n",
			       dev_path(&dummy), id);
			return NULL;
		}
		dev = alloc_dev(bus, &dummy.path);
	} else {
		/*
		 * Enable/disable the device. Once we have found the device-
		 * specific operations this operations we will disable the
		 * device with those as well.
		 *
		 * This is geared toward devices that have subfunctions
		 * that do not show up by default.
		 *
		 * If a device is a stuff option on the motherboard
		 * it may be absent and enable_dev() must cope.
		 */
		/* Run the magic enable sequence for the device. */
		if (dev->chip_ops && dev->chip_ops->enable_dev)
			dev->chip_ops->enable_dev(dev);

		/* Now read the vendor and device ID. */
		id = pci_read_config32(dev, PCI_VENDOR_ID);

		/*
		 * If the device does not have a PCI ID disable it. Possibly
		 * this is because we have already disabled the device. But
		 * this also handles optional devices that may not always
		 * show up.
		 */
		/* If the chain is fully enumerated quit */
		if ((id == 0xffffffff) || (id == 0x00000000) ||
		    (id == 0x0000ffff) || (id == 0xffff0000)) {
			if (dev->enabled) {
				printk(BIOS_INFO,
				       "PCI: Static device %s not found, disabling it.\n",
				       dev_path(dev));
				dev->enabled = 0;
			}
			return dev;
		}
	}

	/* Read the rest of the PCI configuration information. */
	hdr_type = pci_read_config8(dev, PCI_HEADER_TYPE);
	class = pci_read_config32(dev, PCI_CLASS_REVISION);

	/* Store the interesting information in the device structure. */
	dev->vendor = id & 0xffff;
	dev->device = (id >> 16) & 0xffff;
	dev->hdr_type = hdr_type;

	/* Class code, the upper 3 bytes of PCI_CLASS_REVISION. */
	dev->class = class >> 8;

	/* Architectural/System devices always need to be bus masters. */
	if ((dev->class >> 16) == PCI_BASE_CLASS_SYSTEM &&
	    CONFIG(PCI_ALLOW_BUS_MASTER_ANY_DEVICE))
		dev->command |= PCI_COMMAND_MASTER;

	/*
	 * Look at the vendor and device ID, or at least the header type and
	 * class and figure out which set of configuration methods to use.
	 * Unless we already have some PCI ops.
	 */
	set_pci_ops(dev);

	/* Now run the magic enable/disable sequence for the device. */
	if (dev->ops && dev->ops->enable)
		dev->ops->enable(dev);

	/* Display the device. */
	printk(BIOS_DEBUG, "%s [%04x/%04x] %s%s\n", dev_path(dev),
	       dev->vendor, dev->device, dev->enabled ? "enabled" : "disabled",
	       dev->ops ? "" : " No operations");

	return dev;
}

/**
 * Test for match between romstage and ramstage device instance.
 *
 * @param dev Pointer to the device structure.
 * @param sdev Simple device model identifier, created with PCI_DEV().
 * @return Non-zero if bus:dev.fn of device matches.
 */
unsigned int pci_match_simple_dev(struct device *dev, pci_devfn_t sdev)
{
	return dev->bus->secondary == PCI_DEV2SEGBUS(sdev) &&
			dev->path.pci.devfn == PCI_DEV2DEVFN(sdev);
}

/**
 * PCI devices that are marked as "hidden" do not get probed. However, the same
 * initialization logic is still performed as if it were. This is useful when
 * devices would like to be described in the devicetree.cb file, and/or present
 * static PCI resources to the allocator, but the platform firmware hides the
 * device (makes the device invisible to PCI enumeration) before PCI enumeration
 * takes place.
 *
 * The expected semantics of PCI devices marked as 'hidden':
 * 1) The device is actually present under the specified BDF
 * 2) The device config space can still be accessed somehow, but the Vendor ID
 *     indicates there is no device there (it reads as 0xffffffff).
 * 3) The device may still consume PCI resources. Typically, these would have
 *     been hardcoded elsewhere.
 *
 * @param dev Pointer to the device structure.
 */
static void pci_scan_hidden_device(struct device *dev)
{
	if (dev->chip_ops && dev->chip_ops->enable_dev)
		dev->chip_ops->enable_dev(dev);

	/*
	 * If chip_ops->enable_dev did not set dev->ops, then set to a default
	 * .ops, because PCI enumeration is effectively being skipped, therefore
	 * no PCI driver will bind to this device. However, children may want to
	 * be enumerated, so this provides scan_static_bus for the .scan_bus
	 * callback.
	 */
	if (dev->ops == NULL)
		dev->ops = &default_hidden_pci_ops_dev;

	if (dev->ops->enable)
		dev->ops->enable(dev);

	/* Display the device almost as if it were probed normally */
	printk(BIOS_DEBUG, "%s [0000/%04x] hidden%s\n", dev_path(dev),
	       dev->device, dev->ops ? "" : " No operations");
}

/**
 * A PCIe Downstream Port normally leads to a Link with only Device 0 on it
 * (PCIe spec r5.0, sec 7.3.1).  As an optimization, scan only for Device 0 in
 * that situation.
 *
 * @param bus Pointer to the bus structure.
 */
static bool pci_bus_only_one_child(struct bus *bus)
{
	struct device *bridge = bus->dev;
	u16 pcie_pos, pcie_flags_reg;
	int pcie_type;

	if (!bridge)
		return false;

	if (bridge->path.type != DEVICE_PATH_PCI)
		return false;

	pcie_pos = pci_find_capability(bridge, PCI_CAP_ID_PCIE);
	if (!pcie_pos)
		return false;

	pcie_flags_reg = pci_read_config16(bridge, pcie_pos + PCI_EXP_FLAGS);

	pcie_type = (pcie_flags_reg & PCI_EXP_FLAGS_TYPE) >> 4;

	return pciexp_is_downstream_port(pcie_type);
}

/**
 * Scan a PCI bus.
 *
 * Determine the existence of devices and bridges on a PCI bus. If there are
 * bridges on the bus, recursively scan the buses behind the bridges.
 *
 * @param bus Pointer to the bus structure.
 * @param min_devfn Minimum devfn to look at in the scan, usually 0x00.
 * @param max_devfn Maximum devfn to look at in the scan, usually 0xff.
 */
void pci_scan_bus(struct bus *bus, unsigned int min_devfn,
			  unsigned int max_devfn)
{
	unsigned int devfn;
	struct device *dev, **prev;
	int once = 0;

	printk(BIOS_DEBUG, "PCI: %s for bus %02x\n", __func__, bus->secondary);

	/* Maximum sane devfn is 0xFF. */
	if (max_devfn > 0xff) {
		printk(BIOS_ERR, "PCI: %s limits devfn %x - devfn %x\n",
		       __func__, min_devfn, max_devfn);
		printk(BIOS_ERR, "PCI: %s upper limit too big. Using 0xff.\n", __func__);
		max_devfn=0xff;
	}

	post_code(0x24);

	if (pci_bus_only_one_child(bus))
		max_devfn = MIN(max_devfn, 0x07);

	/*
	 * Probe all devices/functions on this bus with some optimization for
	 * non-existence and single function devices.
	 */
	for (devfn = min_devfn; devfn <= max_devfn; devfn++) {
		if (CONFIG(MINIMAL_PCI_SCANNING)) {
			dev = pcidev_path_behind(bus, devfn);
			if (!dev || !dev->mandatory)
				continue;
		}

		/* First thing setup the device structure. */
		dev = pci_scan_get_dev(bus, devfn);

		/* Devices marked 'hidden' do not get probed */
		if (dev && dev->hidden) {
			pci_scan_hidden_device(dev);

			/* Skip pci_probe_dev, go to next devfn */
			continue;
		}

		/* See if a device is present and setup the device structure. */
		dev = pci_probe_dev(dev, bus, devfn);

		/*
		 * If this is not a multi function device, or the device is
		 * not present don't waste time probing another function.
		 * Skip to next device.
		 */
		if ((PCI_FUNC(devfn) == 0x00) && (!dev
		     || (dev->enabled && ((dev->hdr_type & 0x80) != 0x80)))) {
			devfn += 0x07;
		}
	}

	post_code(0x25);

	/*
	 * Warn if any leftover static devices are found.
	 * There's probably a problem in devicetree.cb.
	 */

	prev = &bus->children;
	for (dev = bus->children; dev; dev = dev->sibling) {

		/*
		 * If static device is not PCI then enable it here and don't
		 * treat it as a leftover device.
		 */
		if (dev->path.type != DEVICE_PATH_PCI) {
			enable_static_device(dev);
			continue;
		}

		/*
		 * The device is only considered leftover if it is not hidden
		 * and it has a Vendor ID of 0 (the default for a device that
		 * could not be probed).
		 */
		if (dev->vendor != 0 || dev->hidden) {
			prev = &dev->sibling;
			continue;
		}

		/* Unlink it from list. */
		*prev = dev->sibling;

		if (!once++)
			printk(BIOS_WARNING, "PCI: Leftover static devices:\n");
		printk(BIOS_WARNING, "%s\n", dev_path(dev));
	}

	if (once)
		printk(BIOS_WARNING, "PCI: Check your devicetree.cb.\n");

	/*
	 * For all children that implement scan_bus() (i.e. bridges)
	 * scan the bus behind that child.
	 */

	scan_bridges(bus);

	/*
	 * We've scanned the bus and so we know all about what's on the other
	 * side of any bridges that may be on this bus plus any devices.
	 * Return how far we've got finding sub-buses.
	 */
	post_code(0x55);
}

typedef enum {
	PCI_ROUTE_CLOSE,
	PCI_ROUTE_SCAN,
	PCI_ROUTE_FINAL,
} scan_state;

static void pci_bridge_route(struct bus *link, scan_state state)
{
	struct device *dev = link->dev;
	struct bus *parent = dev->bus;
	uint8_t primary, secondary, subordinate;

	if (state == PCI_ROUTE_SCAN) {
		link->secondary = parent->subordinate + 1;
		link->subordinate = link->secondary + dev->hotplug_buses;
	}

	if (state == PCI_ROUTE_CLOSE) {
		primary = 0;
		secondary = 0xff;
		subordinate = 0xfe;
	} else if (state == PCI_ROUTE_SCAN) {
		primary = parent->secondary;
		secondary = link->secondary;
		subordinate = 0xff; /* MAX PCI_BUS number here */
	} else if (state == PCI_ROUTE_FINAL) {
		primary = parent->secondary;
		secondary = link->secondary;
		subordinate = link->subordinate;
	} else {
		return;
	}

	if (state == PCI_ROUTE_SCAN) {
		/* Clear all status bits and turn off memory, I/O and master enables. */
		link->bridge_cmd = pci_read_config16(dev, PCI_COMMAND);
		pci_write_config16(dev, PCI_COMMAND, 0x0000);
		pci_write_config16(dev, PCI_STATUS, 0xffff);
	}

	/*
	 * Configure the bus numbers for this bridge: the configuration
	 * transactions will not be propagated by the bridge if it is not
	 * correctly configured.
	 */
	pci_write_config8(dev, PCI_PRIMARY_BUS, primary);
	pci_write_config8(dev, PCI_SECONDARY_BUS, secondary);
	pci_write_config8(dev, PCI_SUBORDINATE_BUS, subordinate);

	if (state == PCI_ROUTE_FINAL) {
		pci_write_config16(dev, PCI_COMMAND, link->bridge_cmd);
		parent->subordinate = link->subordinate;
	}
}

/**
 * Scan a PCI bridge and the buses behind the bridge.
 *
 * Determine the existence of buses behind the bridge. Set up the bridge
 * according to the result of the scan.
 *
 * This function is the default scan_bus() method for PCI bridge devices.
 *
 * @param dev Pointer to the bridge device.
 * @param do_scan_bus TODO
 */
void do_pci_scan_bridge(struct device *dev,
				void (*do_scan_bus) (struct bus * bus,
							     unsigned int min_devfn,
							     unsigned int max_devfn))
{
	struct bus *bus;

	printk(BIOS_SPEW, "%s for %s\n", __func__, dev_path(dev));

	if (dev->link_list == NULL) {
		struct bus *link;
		link = malloc(sizeof(*link));
		if (link == NULL)
			die("Couldn't allocate a link!\n");
		memset(link, 0, sizeof(*link));
		link->dev = dev;
		dev->link_list = link;
	}

	bus = dev->link_list;

	pci_bridge_vga_compat(bus);

	pci_bridge_route(bus, PCI_ROUTE_SCAN);

	do_scan_bus(bus, 0x00, 0xff);

	pci_bridge_route(bus, PCI_ROUTE_FINAL);
}

/**
 * Scan a PCI bridge and the buses behind the bridge.
 *
 * Determine the existence of buses behind the bridge. Set up the bridge
 * according to the result of the scan.
 *
 * This function is the default scan_bus() method for PCI bridge devices.
 *
 * @param dev Pointer to the bridge device.
 */
void pci_scan_bridge(struct device *dev)
{
	do_pci_scan_bridge(dev, pci_scan_bus);
}

/**
 * Scan a PCI domain.
 *
 * This function is the default scan_bus() method for PCI domains.
 *
 * @param dev Pointer to the domain.
 */
void pci_domain_scan_bus(struct device *dev)
{
	struct bus *link = dev->link_list;
	pci_scan_bus(link, PCI_DEVFN(0, 0), 0xff);
}

void pci_dev_disable_bus_master(const struct device *dev)
{
	pci_update_config16(dev, PCI_COMMAND, ~PCI_COMMAND_MASTER, 0x0);
}

/**
 * Take an INT_PIN number (0, 1 - 4) and convert
 * it to a string ("NO PIN", "PIN A" - "PIN D")
 *
 * @param pin PCI Interrupt Pin number (0, 1 - 4)
 * @return A string corresponding to the pin number or "Invalid"
 */
const char *pin_to_str(int pin)
{
	const char *str[5] = {
		"NO PIN",
		"PIN A",
		"PIN B",
		"PIN C",
		"PIN D",
	};

	if (pin >= 0 && pin <= 4)
		return str[pin];
	else
		return "Invalid PIN, not 0 - 4";
}

/**
 * Get the PCI INT_PIN swizzle for a device defined as:
 *   pin_parent = (pin_child + devn_child) % 4 + 1
 *   where PIN A = 1 ... PIN_D = 4
 *
 * Given a PCI device structure 'dev', find the interrupt pin
 * that will be triggered on its parent bridge device when
 * generating an interrupt.  For example: Device 1:3.2 may
 * use INT_PIN A but will trigger PIN D on its parent bridge
 * device.  In this case, this function will return 4 (PIN D).
 *
 * @param dev A PCI device structure to swizzle interrupt pins for
 * @param *parent_bridge The PCI device structure for the bridge
 *        device 'dev' is attached to
 * @return The interrupt pin number (1 - 4) that 'dev' will
 *         trigger when generating an interrupt
 */
static int swizzle_irq_pins(struct device *dev, struct device **parent_bridge)
{
	struct device *parent;	/* Our current device's parent device */
	struct device *child;		/* The child device of the parent */
	uint8_t parent_bus = 0;		/* Parent Bus number */
	uint16_t parent_devfn = 0;	/* Parent Device and Function number */
	uint16_t child_devfn = 0;	/* Child Device and Function number */
	uint8_t swizzled_pin = 0;	/* Pin swizzled across a bridge */

	/* Start with PIN A = 0 ... D = 3 */
	swizzled_pin = pci_read_config8(dev, PCI_INTERRUPT_PIN) - 1;

	/* While our current device has parent devices */
	child = dev;
	for (parent = child->bus->dev; parent; parent = parent->bus->dev) {
		parent_bus = parent->bus->secondary;
		parent_devfn = parent->path.pci.devfn;
		child_devfn = child->path.pci.devfn;

		/* Swizzle the INT_PIN for any bridges not on root bus */
		swizzled_pin = (PCI_SLOT(child_devfn) + swizzled_pin) % 4;
		printk(BIOS_SPEW, "\tWith INT_PIN swizzled to %s\n"
			"\tAttached to bridge device %01X:%02Xh.%02Xh\n",
			pin_to_str(swizzled_pin + 1), parent_bus,
			PCI_SLOT(parent_devfn), PCI_FUNC(parent_devfn));

		/* Continue until we find the root bus */
		if (parent_bus > 0) {
			/*
			 * We will go on to the next parent so this parent
			 * becomes the child
			 */
			child = parent;
			continue;
		} else {
			/*
			 *  Found the root bridge device,
			 *  fill in the structure and exit
			 */
			*parent_bridge = parent;
			break;
		}
	}

	/* End with PIN A = 1 ... D = 4 */
	return swizzled_pin + 1;
}

/**
 * Given a device structure 'dev', find its interrupt pin
 * and its parent bridge 'parent_bdg' device structure.
 * If it is behind a bridge, it will return the interrupt
 * pin number (1 - 4) of the parent bridge that the device
 * interrupt pin has been swizzled to, otherwise it will
 * return the interrupt pin that is programmed into the
 * PCI config space of the target device.  If 'dev' is
 * behind a bridge, it will fill in 'parent_bdg' with the
 * device structure of the bridge it is behind, otherwise
 * it will copy 'dev' into 'parent_bdg'.
 *
 * @param dev A PCI device structure to get interrupt pins for.
 * @param *parent_bdg The PCI device structure for the bridge
 *        device 'dev' is attached to.
 * @return The interrupt pin number (1 - 4) that 'dev' will
 *         trigger when generating an interrupt.
 *         Errors: -1 is returned if the device is not enabled
 *                 -2 is returned if a parent bridge could not be found.
 */
int get_pci_irq_pins(struct device *dev, struct device **parent_bdg)
{
	uint8_t bus = 0;	/* The bus this device is on */
	uint16_t devfn = 0;	/* This device's device and function numbers */
	uint8_t int_pin = 0;	/* Interrupt pin used by the device */
	uint8_t target_pin = 0;	/* Interrupt pin we want to assign an IRQ to */

	/* Make sure this device is enabled */
	if (!(dev->enabled && (dev->path.type == DEVICE_PATH_PCI)))
		return -1;

	bus = dev->bus->secondary;
	devfn = dev->path.pci.devfn;

	/* Get and validate the interrupt pin used. Only 1-4 are allowed */
	int_pin = pci_read_config8(dev, PCI_INTERRUPT_PIN);
	if (int_pin < 1 || int_pin > 4)
		return -1;

	printk(BIOS_SPEW, "PCI IRQ: Found device %01X:%02X.%02X using %s\n",
		bus, PCI_SLOT(devfn), PCI_FUNC(devfn), pin_to_str(int_pin));

	/* If this device is on a bridge, swizzle its INT_PIN */
	if (bus) {
		/* Swizzle its INT_PINs */
		target_pin = swizzle_irq_pins(dev, parent_bdg);

		/* Make sure the swizzle returned valid structures */
		if (parent_bdg == NULL) {
			printk(BIOS_WARNING, "Could not find parent bridge for this device!\n");
			return -2;
		}
	} else {	/* Device is not behind a bridge */
		target_pin = int_pin;	/* Return its own interrupt pin */
		*parent_bdg = dev;		/* Return its own structure */
	}

	/* Target pin is the interrupt pin we want to assign an IRQ to */
	return target_pin;
}

#if CONFIG(PC80_SYSTEM)
/**
 * Assign IRQ numbers.
 *
 * This function assigns IRQs for all functions contained within the indicated
 * device address. If the device does not exist or does not require interrupts
 * then this function has no effect.
 *
 * This function should be called for each PCI slot in your system.
 *
 * @param dev Pointer to dev structure.
 * @param pIntAtoD An array of IRQ #s that are assigned to PINTA through PINTD
 *        of this slot. The particular IRQ #s that are passed in depend on the
 *        routing inside your southbridge and on your board.
 */
void pci_assign_irqs(struct device *dev, const unsigned char pIntAtoD[4])
{
	u8 slot, line, irq;

	/* Each device may contain up to eight functions. */
	slot = dev->path.pci.devfn >> 3;

	for (; dev ; dev = dev->sibling) {

		if (dev->path.pci.devfn >> 3 != slot)
			break;

		line = pci_read_config8(dev, PCI_INTERRUPT_PIN);

		/* PCI spec says all values except 1..4 are reserved. */
		if ((line < 1) || (line > 4))
			continue;

		irq = pIntAtoD[line - 1];

		printk(BIOS_DEBUG, "Assigning IRQ %d to %s\n", irq, dev_path(dev));

		pci_write_config8(dev, PCI_INTERRUPT_LINE, irq);

		/* Change to level triggered. */
		i8259_configure_irq_trigger(irq, IRQ_LEVEL_TRIGGERED);
	}
}
#endif
