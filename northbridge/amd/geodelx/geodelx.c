/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <types.h>
#include <console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <msr.h>
#include <amd_geodelx.h>
#include <statictree.h>
#include "geodelink.h"

/* Function prototypes */
extern void chipsetinit(void);
extern void northbridge_init_early(void);
extern void graphics_init(u8 video_mb);
extern u64 sizeram(void);

/**
 * Currently not set up.
 *
 * @param dev The nortbridge device.
 */
static void enable_shadow(struct device *dev)
{
}


/**
 * TODO.
 *
 * @return TODO.
 */
u64 get_systop(struct northbridge_amd_geodelx_domain_config *nb_dm)
{
	const struct gliutable *gl = NULL;
	u64 systop;
	struct msr msr;
	int i;

	for (i = 0; gliu0table[i].desc_name != GL_END; i++) {
		if (gliu0table[i].desc_type == R_SYSMEM) {
			gl = &gliu0table[i];
			break;
		}
	}
	if (gl) {
		msr = rdmsr(gl->desc_name);
		systop = ((msr.hi & 0xFF) << 24) | ((msr.lo & 0xFFF00000) >> 8);
		systop += 4 * 1024;	/* 4K */
	} else {
		systop =
		    (((sizeram() - nb_dm->geode_video_mb) * 1024) - SMM_SIZE) * 1024;
	}

	return systop;
}

/**
 * Initialize the northbridge PCI device.
 * Right now this a no op. We leave it here as a hook for later use.
 *
 * @param dev The nortbridge device.
 */
static void geodelx_northbridge_init(struct device *dev)
{
	/* struct msr msr; */

	printk(BIOS_SPEW, ">> Entering northbridge.c: %s\n", __FUNCTION__);

	enable_shadow(dev);
}

/**
 * Set resources for the PCI northbridge device.
 * This function is required due to VSA interactions.
 *
 * @param dev The nortbridge device.
 */
static void geodelx_northbridge_set_resources(struct device *dev)
{
	struct resource *resource, *last;
	unsigned int link;
	struct bus *bus;
	u8 line;

	last = &dev->resource[dev->resources];

	for (resource = &dev->resource[0]; resource < last; resource++) {
		/* From AMD: do not change the base address, it will
		 * make the VSA virtual registers unusable.
		 */
		// pci_set_resource(dev, resource);
		// FIXME: Static allocation may conflict with dynamic mappings!
	}

	for (link = 0; link < dev->links; link++) {
		bus = &dev->link[link];
		if (bus->children) {
			printk(BIOS_DEBUG, "my_dev_set_resources: phase4_assign_resources %p\n", bus);
			phase4_assign_resources(bus);
		}
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

/**
 * Set resources in the PCI domain.
 *
 * Also, as a side effect, create a RAM resource in the child which,
 * interestingly enough, is the northbridge PCI device, for later
 * allocation of address space.
 *
 * @param dev The device.
 */
static void geodelx_pci_domain_set_resources(struct device *dev)
{
	int idx;
	struct device *mc_dev;
	struct northbridge_amd_geodelx_domain_config *nb_dm =
	 (struct northbridge_amd_geodelx_domain_config *)dev->device_configuration;

	printk(BIOS_SPEW, ">> Entering northbridge.c: %s\n", __FUNCTION__);

	mc_dev = dev->link[0].children;
	if (mc_dev) {
		/* Report the memory regions. */
		idx = 10;
		/* 0 .. 640 KB */
		ram_resource(dev, idx++, 0, 640);
		/* 1 MB .. (Systop - 1 MB) (in KB) */
		ram_resource(dev, idx++, 1024,
			     (get_systop(nb_dm)/1024) - 1024);
	}

	phase4_assign_resources(&dev->link[0]);
}

/**
 * Enable the PCI domain.
 *
 * A littly tricky on this chipset due to the VSA interactions. This must
 * happen before any PCI scans happen. We do early northbridge init to make
 * sure PCI scans will work, but the weird part is we actually have to run
 * some code in x86 mode to get the VSM installed, since the VSM actually
 * handles some PCI bus scan tasks via the System Management Interrupt.
 * Yes, it gets tricky...
 *
 * @param dev The device.
 */
static void geodelx_pci_domain_phase2(struct device *dev)
{
	struct northbridge_amd_geodelx_domain_config *nb_dm =
	 (struct northbridge_amd_geodelx_domain_config *)dev->device_configuration;

	void do_vsmbios(void);

	printk(BIOS_SPEW, ">> Entering northbridge.c: %s\n", __FUNCTION__);

//	northbridge_init_early();
	chipsetinit();

	printk(BIOS_SPEW, "Before VSA:\n");
	/* print_conf(); */
	/* Do the magic stuff here, so prepare your tambourine ;) */
	do_vsmbios(); 
	printk(BIOS_SPEW, "After VSA:\n");
	/* print_conf(); */
printk(BIOS_DEBUG, "VRC_VG value: 0x%04x\n", nb_dm->geode_video_mb);
	graphics_init((u8)nb_dm->geode_video_mb);
	pci_set_method(dev);
}

/**
 * Support for APIC cluster init.
 *
 * TODO: Should we do this in phase 2? It is now done in phase 6.
 *
 * @param dev The PCI domain device.
 */
static void cpu_bus_init(struct device *dev)
{
	printk(BIOS_SPEW, ">> Entering northbridge.c: %s\n", __FUNCTION__);
	printk(BIOS_SPEW, ">> Exiting northbridge.c: %s\n", __FUNCTION__);
}

static void cpu_bus_noop(struct device *dev)
{
}

/* The same hardware, being multifunction, has several roles. In this case,
 * the northbridge is a PCI domain controller, APIC cluster, and the
 * traditional 0:0.0 device.
 */

/** Operations for when the northbridge is running a PCI domain. */
struct device_operations geodelx_north_domain = {
	.id = {.type = DEVICE_ID_PCI_DOMAIN,
		.u = {.pci_domain = {.vendor = PCI_VENDOR_ID_AMD,
				     .device = PCI_DEVICE_ID_AMD_LXBRIDGE}}},
	.constructor			= default_device_constructor,
	.phase2_setup_scan_bus		= geodelx_pci_domain_phase2,
	.phase3_scan			= pci_domain_scan_bus,
	.phase4_read_resources		= pci_domain_read_resources,
	.phase4_set_resources		= geodelx_pci_domain_set_resources,
	.phase5_enable_resources	= enable_childrens_resources,
	.phase6_init			= 0,
	.ops_pci_bus			= &pci_cf8_conf1,
};

/** Operations for when the northbridge is running an APIC cluster. */
struct device_operations geodelx_north_apic = {
	.id = {.type = DEVICE_ID_APIC_CLUSTER,
		.u = {.apic_cluster = {.vendor = PCI_VENDOR_ID_AMD,
				       .device = PCI_DEVICE_ID_AMD_LXBRIDGE}}},
	.constructor			= default_device_constructor,
	.phase3_scan			= 0,
	.phase4_read_resources		= cpu_bus_noop,
	.phase4_set_resources		= cpu_bus_noop,
	.phase5_enable_resources	= cpu_bus_noop,
	.phase6_init			= cpu_bus_init,
	.ops_pci_bus			= &pci_cf8_conf1,
};

/** Operations for when the northbridge is running a PCI device. */
/** Note that phase3 scan is done in the domain, 
 * and MUST NOT be done here too 
 */
struct device_operations geodelx_north_pci = {
	.id = {.type = DEVICE_ID_PCI,
		.u = {.pci = {.vendor = PCI_VENDOR_ID_AMD,
			      .device = PCI_DEVICE_ID_AMD_LXBRIDGE}}},
	.constructor			= default_device_constructor,
	.phase3_scan			= 0,
	.phase4_read_resources		= pci_domain_read_resources,
	.phase4_set_resources		= geodelx_northbridge_set_resources,
	.phase5_enable_resources	= enable_childrens_resources,
	.phase6_init			= geodelx_northbridge_init,
	.ops_pci_bus			= &pci_cf8_conf1,
};
