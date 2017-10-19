/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015-2016 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


#include <arch/io.h>
#include <arch/acpi.h>
#include <arch/acpigen.h>
#include <cbmem.h>
#include <chip.h>
#include <console/console.h>
#include <cpu/amd/mtrr.h>
#include <cpu/cpu.h>
#include <cpu/x86/msr.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <agesawrapper.h>
#include <agesawrapper_call.h>
#include <soc/northbridge.h>
#include <soc/pci_devs.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct dram_base_mask {
	u32 base; /* [47:27] at [28:8] */
	u32 mask; /* [47:27] at [28:8] and enable at bit 0 */
} dram_base_mask_t;

static dram_base_mask_t get_dram_base_mask(void)
{
	device_t dev = dev_find_slot(0, ADDR_DEVFN);
	dram_base_mask_t d;
	u32 temp;

	/* [39:24] at [31:16] */
	temp = pci_read_config32(dev, 0x44);

	/* mask out  DramMask [26:24] too */
	d.mask = ((temp & 0xfff80000) >> (8 + 3));

	/* [47:40] at [7:0] */
	temp = pci_read_config32(dev, 0x144) & 0xff;
	d.mask |= temp << 21;

	temp = pci_read_config32(dev, 0x40);
	d.mask |= (temp & 1); /* enable bit */
	d.base = ((temp & 0xfff80000) >> (8 + 3));
	temp = pci_read_config32(dev, 0x140) & 0xff;
	d.base |= temp << 21;
	return d;
}

static void set_io_addr_reg(device_t dev, u32 nodeid, u32 linkn, u32 reg,
			u32 io_min, u32 io_max)
{
	u32 tempreg;
	device_t addr_map = dev_find_slot(0, ADDR_DEVFN);

	/* io range allocation.  Limit */
	tempreg = (nodeid & 0xf) | ((nodeid & 0x30) << (8 - 4)) | (linkn << 4)
						| ((io_max & 0xf0) << (12 - 4));
	pci_write_config32(addr_map, reg + 4, tempreg);
	tempreg = 3 | ((io_min & 0xf0) << (12 - 4)); /* base: ISA and VGA ? */
	pci_write_config32(addr_map, reg, tempreg);
}

static void set_mmio_addr_reg(u32 nodeid, u32 linkn, u32 reg, u32 index,
						u32 mmio_min, u32 mmio_max)
{
	u32 tempreg;
	device_t addr_map = dev_find_slot(0, ADDR_DEVFN);

	/* io range allocation.  Limit */
	tempreg = (nodeid & 0xf) | (linkn << 4) | (mmio_max & 0xffffff00);
		pci_write_config32(addr_map, reg + 4, tempreg);
	tempreg = 3 | (nodeid & 0x30) | (mmio_min & 0xffffff00);
		pci_write_config32(addr_map, reg, tempreg);
}

static void read_resources(device_t dev)
{
	/*
	 * This MMCONF resource must be reserved in the PCI domain.
	 * It is not honored by the coreboot resource allocator if it is in
	 * the CPU_CLUSTER.
	 */
	mmconf_resource(dev, 0xc0010058);
}

static void set_resource(device_t dev, struct resource *resource, u32 nodeid)
{
	resource_t rbase, rend;
	unsigned int reg, link_num;
	char buf[50];

	/* Make certain the resource has actually been set */
	if (!(resource->flags & IORESOURCE_ASSIGNED))
		return;

	/* If I have already stored this resource don't worry about it */
	if (resource->flags & IORESOURCE_STORED)
		return;

	/* Only handle PCI memory and IO resources */
	if (!(resource->flags & (IORESOURCE_MEM | IORESOURCE_IO)))
		return;

	/* Ensure I am actually looking at a resource of function 1 */
	if ((resource->index & 0xffff) < 0x1000)
		return;

	/* Get the base address */
	rbase = resource->base;

	/* Get the limit (rounded up) */
	rend  = resource_end(resource);

	/* Get the register and link */
	reg  = resource->index & 0xfff; /* 4k */
	link_num = IOINDEX_LINK(resource->index);

	if (resource->flags & IORESOURCE_IO)
		set_io_addr_reg(dev, nodeid, link_num, reg, rbase>>8, rend>>8);
	else if (resource->flags & IORESOURCE_MEM)
		set_mmio_addr_reg(nodeid, link_num, reg,
				(resource->index >> 24), rbase >> 8, rend >> 8);

	resource->flags |= IORESOURCE_STORED;
	snprintf(buf, sizeof(buf), " <node %x link %x>",
			nodeid, link_num);
	report_resource_stored(dev, resource, buf);
}

/**
 * I tried to reuse the resource allocation code in set_resource()
 * but it is too difficult to deal with the resource allocation magic.
 */

static void create_vga_resource(device_t dev)
{
	struct bus *link;

	/* find out which link the VGA card is connected,
	 * we only deal with the 'first' vga card */
	for (link = dev->link_list ; link ; link = link->next)
		if (link->bridge_ctrl & PCI_BRIDGE_CTL_VGA)
			break;

	/* no VGA card installed */
	if (link == NULL)
		return;

	printk(BIOS_DEBUG, "VGA: %s has VGA device\n",	dev_path(dev));
	/* Route A0000-BFFFF, IO 3B0-3BB 3C0-3DF */
	pci_write_config32(dev_find_slot(0, ADDR_DEVFN), 0xf4, 1);
}

static void set_resources(device_t dev)
{
	struct bus *bus;
	struct resource *res;


	/* do we need this? */
	create_vga_resource(dev);

	/* Set each resource we have found */
	for (res = dev->resource_list ; res ; res = res->next)
		set_resource(dev, res, 0);

	for (bus = dev->link_list ; bus ; bus = bus->next)
		if (bus->children)
			assign_resources(bus);
}

static void northbridge_init(struct device *dev)
{
}

static unsigned long acpi_fill_hest(acpi_hest_t *hest)
{
	void *addr, *current;

	/* Skip the HEST header. */
	current = (void *)(hest + 1);

	addr = agesawrapper_getlateinitptr(PICK_WHEA_MCE);
	if (addr != NULL)
		current += acpi_create_hest_error_source(hest, current, 0,
				(void *)((u32)addr + 2), *(UINT16 *)addr - 2);

	addr = agesawrapper_getlateinitptr(PICK_WHEA_CMC);
	if (addr != NULL)
		current += acpi_create_hest_error_source(hest, current, 1,
				(void *)((u32)addr + 2), *(UINT16 *)addr - 2);

	return (unsigned long)current;
}

static void northbridge_fill_ssdt_generator(device_t device)
{
	msr_t msr;
	char pscope[] = "\\_SB.PCI0";

	acpigen_write_scope(pscope);
	msr = rdmsr(TOP_MEM);
	acpigen_write_name_dword("TOM1", msr.lo);
	msr = rdmsr(TOP_MEM2);
	/*
	 * Since XP only implements parts of ACPI 2.0, we can't use a qword
	 * here.
	 * See http://www.acpi.info/presentations/S01USMOBS169_OS%2520new.ppt
	 * slide 22ff.
	 * Shift value right by 20 bit to make it fit into 32bit,
	 * giving us 1MB granularity and a limit of almost 4Exabyte of memory.
	 */
	acpigen_write_name_dword("TOM2", (msr.hi << 12) | msr.lo >> 20);
	acpigen_pop_len();
}

static unsigned long agesa_write_acpi_tables(device_t device,
					     unsigned long current,
					     acpi_rsdp_t *rsdp)
{
	acpi_srat_t *srat;
	acpi_slit_t *slit;
	acpi_header_t *ssdt;
	acpi_header_t *alib;
	acpi_header_t *ivrs;
	acpi_hest_t *hest;

	/* HEST */
	current = ALIGN(current, 8);
	hest = (acpi_hest_t *)current;
	acpi_write_hest((void *)current, acpi_fill_hest);
	acpi_add_table(rsdp, (void *)current);
	current += ((acpi_header_t *)current)->length;

	current = ALIGN(current, 8);
	printk(BIOS_DEBUG, "ACPI:    * IVRS at %lx\n", current);
	ivrs = agesawrapper_getlateinitptr(PICK_IVRS);
	if (ivrs != NULL) {
		memcpy((void *)current, ivrs, ivrs->length);
		ivrs = (acpi_header_t *)current;
		current += ivrs->length;
		acpi_add_table(rsdp, ivrs);
	} else {
		printk(BIOS_DEBUG, "  AGESA IVRS table NULL. Skipping.\n");
	}

	/* SRAT */
	current = ALIGN(current, 8);
	printk(BIOS_DEBUG, "ACPI:    * SRAT at %lx\n", current);
	srat = (acpi_srat_t *)agesawrapper_getlateinitptr(PICK_SRAT);
	if (srat != NULL) {
		memcpy((void *)current, srat, srat->header.length);
		srat = (acpi_srat_t *)current;
		current += srat->header.length;
		acpi_add_table(rsdp, srat);
	} else {
		printk(BIOS_DEBUG, "  AGESA SRAT table NULL. Skipping.\n");
	}

	/* SLIT */
	current = ALIGN(current, 8);
	printk(BIOS_DEBUG, "ACPI:   * SLIT at %lx\n", current);
	slit = (acpi_slit_t *)agesawrapper_getlateinitptr(PICK_SLIT);
	if (slit != NULL) {
		memcpy((void *)current, slit, slit->header.length);
		slit = (acpi_slit_t *)current;
		current += slit->header.length;
		acpi_add_table(rsdp, slit);
	} else {
		printk(BIOS_DEBUG, "  AGESA SLIT table NULL. Skipping.\n");
	}

	/* ALIB */
	current = ALIGN(current, 16);
	printk(BIOS_DEBUG, "ACPI:  * AGESA ALIB SSDT at %lx\n", current);
	alib = (acpi_header_t *)agesawrapper_getlateinitptr(PICK_ALIB);
	if (alib != NULL) {
		memcpy((void *)current, alib, alib->length);
		alib = (acpi_header_t *)current;
		current += alib->length;
		acpi_add_table(rsdp, (void *)alib);
	} else {
		printk(BIOS_DEBUG, "	AGESA ALIB SSDT table NULL."
							" Skipping.\n");
	}

	current   = ALIGN(current, 16);
	printk(BIOS_DEBUG, "ACPI:    * SSDT at %lx\n", current);
	ssdt = (acpi_header_t *)agesawrapper_getlateinitptr(PICK_PSTATE);
	if (ssdt != NULL) {
		memcpy((void *)current, ssdt, ssdt->length);
		ssdt = (acpi_header_t *)current;
		current += ssdt->length;
	} else {
		printk(BIOS_DEBUG, "  AGESA PState table NULL. Skipping.\n");
	}
	acpi_add_table(rsdp, ssdt);

	printk(BIOS_DEBUG, "ACPI:    * SSDT for PState at %lx\n", current);
	return current;
}

static struct device_operations northbridge_operations = {
	.read_resources	  = read_resources,
	.set_resources	  = set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init		  = northbridge_init,
	.acpi_fill_ssdt_generator = northbridge_fill_ssdt_generator,
	.write_acpi_tables = agesa_write_acpi_tables,
	.enable		  = 0,
	.ops_pci	  = 0,
};

static const struct pci_driver family15_northbridge __pci_driver = {
	.ops	= &northbridge_operations,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_15H_MODEL_707F_NB_HT,
};

void fam15_finalize(void *chip_info)
{
	device_t dev;
	u32 value;
	dev = dev_find_slot(0, GNB_DEVFN); /* clear IoapicSbFeatureEn */
	pci_write_config32(dev, 0xf8, 0);
	pci_write_config32(dev, 0xfc, 5); /* TODO: move it to dsdt.asl */

	/* disable No Snoop */
	dev = dev_find_slot(0, HDA0_DEVFN);
	value = pci_read_config32(dev, 0x60);
	value &= ~(1 << 11);
	pci_write_config32(dev, 0x60, value);
}

void domain_read_resources(device_t dev)
{
	unsigned int reg;
	device_t addr_map = dev_find_slot(0, ADDR_DEVFN);

	/* Find the already assigned resource pairs */
	for (reg = 0x80 ; reg <= 0xd8 ; reg += 0x08) {
		u32 base, limit;
		base = pci_read_config32(addr_map, reg);
		limit = pci_read_config32(addr_map, reg + 4);
		/* Is this register allocated? */
		if ((base & 3) != 0) {
			unsigned int nodeid, reg_link;
			device_t reg_dev = dev_find_slot(0, HT_DEVFN);
			if (reg < 0xc0) /* mmio */
				nodeid = (limit & 0xf) + (base & 0x30);
			else /* io */
				nodeid =  (limit & 0xf) + ((base >> 4) & 0x30);

			reg_link = (limit >> 4) & 7;
			if (reg_dev) {
				/* Reserve the resource  */
				struct resource *res;
				res = new_resource(reg_dev,
						IOINDEX(0x1000 + reg,
								reg_link));
				if (res)
					res->flags = 1;
			}
		}
	}
	/* FIXME: do we need to check extend conf space?
	   I don't believe that much preset value */

	pci_domain_read_resources(dev);
}

void domain_enable_resources(device_t dev)
{
	if (acpi_is_wakeup_s3())
		AGESAWRAPPER(fchs3laterestore);

	/* Must be called after PCI enumeration and resource allocation */
	if (!acpi_is_wakeup_s3())
		AGESAWRAPPER(amdinitmid);

	printk(BIOS_DEBUG, "  ader - leaving domain_enable_resources.\n");
}

void domain_set_resources(device_t dev)
{
	unsigned long mmio_basek;
	u32 pci_tolm;
	u32 hole;
	int idx;
	struct bus *link;
	void *tseg_base;
	size_t tseg_size;

	pci_tolm = 0xffffffffUL;
	for (link = dev->link_list ; link ; link = link->next)
		pci_tolm = find_pci_tolm(link);

	/* Start with alignment supportable in variable MTRR */
	mmio_basek = ALIGN_DOWN(pci_tolm, 4 * KiB) / KiB;

	/*
	 * AGESA may have programmed the memory hole and rounded down to a
	 * 128MB boundary.  If we find it's valid, adjust mmio_basek downward
	 * to the hole bottom.  D18F1xF0[DramHoleBase] is granular to 16MB.
	 */
	hole = pci_read_config32(dev_find_slot(0, ADDR_DEVFN), D18F1_DRAM_HOLE);
	if (hole & DRAM_HOLE_VALID)
		mmio_basek = min(mmio_basek, ALIGN_DOWN(hole, 16 * MiB) / KiB);

	idx = 0x10;
	dram_base_mask_t d;
	resource_t basek, limitk, sizek; /* 4 1T */

	d = get_dram_base_mask();

	if ((d.mask & 1)) { /* if enabled... */
		/*  could overflow, we may lose 6 bit here */
		basek = ((resource_t)(d.base & 0x1fffff00)) << 9;
		limitk = ((resource_t)(((d.mask & ~1) + 0x000ff)
							& 0x1fffff00)) << 9;

		sizek = limitk - basek;

		/* see if we need a hole from 0xa0000 to 0xbffff */
		if ((basek < ((8 * 64) + (8 * 16))) && (sizek > ((8 * 64) +
								(16 * 16)))) {
			ram_resource(dev, idx, basek,
					((8 * 64) + (8 * 16)) - basek);
			idx += 0x10;
			basek = (8 * 64) + (16 * 16);
			sizek = limitk - ((8 * 64) + (16 * 16));

		}

		/* split the region to accommodate pci memory space */
		if ((basek < 4 * 1024 * 1024) && (limitk > mmio_basek)) {
			if (basek <= mmio_basek) {
				unsigned int pre_sizek;
				pre_sizek = mmio_basek - basek;
				if (pre_sizek > 0) {
					ram_resource(dev, idx, basek,
								pre_sizek);
					idx += 0x10;
					sizek -= pre_sizek;
				}
				basek = mmio_basek;
			}
			if ((basek + sizek) <= 4 * 1024 * 1024) {
				sizek = 0;
			} else {
				uint64_t topmem2 = bsp_topmem2();
				basek = 4 * 1024 * 1024;
				sizek = topmem2 / 1024 - basek;
			}
		}

		ram_resource(dev, idx, basek, sizek);
		printk(BIOS_DEBUG, "node 0: mmio_basek=%08lx, basek=%08llx,"
				" limitk=%08llx\n", mmio_basek, basek, limitk);
	}

	/* UMA is not set up yet, but infer the base & size to make cacheable */
	uint32_t uma_base = restore_top_of_low_cacheable();
	if (uma_base != bsp_topmem()) {
		uint32_t uma_size = bsp_topmem() - uma_base;
		printk(BIOS_INFO, "%s: uma size 0x%08x, memory start 0x%08x\n",
				__func__, uma_size, uma_base);
		reserved_ram_resource(dev, 7, uma_base / KiB, uma_size / KiB);
	}

	for (link = dev->link_list ; link ; link = link->next)
		if (link->children)
			assign_resources(link);

	/*
	 * Reserve everything between A segment and 1MB:
	 *
	 * 0xa0000 - 0xbffff: legacy VGA
	 * 0xc0000 - 0xfffff: RAM
	 */
	mmio_resource(dev, 0xa0000, 0xa0000 / KiB, 0x20000 / KiB);
	reserved_ram_resource(dev, 0xc0000, 0xc0000 / KiB, 0x40000 / KiB);

	/* Reserve TSEG */
	smm_region_info(&tseg_base, &tseg_size);
	idx += 0x10;
	reserved_ram_resource(dev, idx, (unsigned long)tseg_base/KiB,
					tseg_size/KiB);
}

/*********************************************************************
 * Change the vendor / device IDs to match the generic VBIOS header. *
 *********************************************************************/
u32 map_oprom_vendev(u32 vendev)
{
	u32 new_vendev;
	new_vendev =
		((vendev >= 0x100298e0) && (vendev <= 0x100298ef)) ?
				0x100298e0 : vendev;

	if (vendev != new_vendev)
		printk(BIOS_NOTICE, "Mapping PCI device %8x to %8x\n",
				vendev, new_vendev);

	return new_vendev;
}
