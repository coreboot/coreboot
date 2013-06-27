/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Rudolf Marek <r.marek@assembler.cz>
 *
 * Based on qemu-x86/northbridge.c
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */


#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <stdlib.h>
#include <string.h>
#include <smbios.h>
#include <cbmem.h>

static unsigned long get_memory_size(void)
{
	device_t nb_dev;
	u8 size;

	nb_dev = dev_find_device(PCI_VENDOR_ID_RDC,
				 PCI_DEVICE_ID_RDC_R8610_NB, 0);
	size = pci_read_config8(nb_dev, 0x6d) & 0xf;
	return (2 * 1024) << size;
}

static void cpu_pci_domain_set_resources(device_t dev)
{
	u32 pci_tolm = find_pci_tolm(dev->link_list);
	unsigned long tomk = 0, tolmk;
	int idx;

	tomk = get_memory_size();
	printk(BIOS_DEBUG, "Detected %lu Kbytes (%lu MiB) RAM.\n",
	       tomk, tomk / 1024);

	/* Compute the top of Low memory */
	tolmk = pci_tolm >> 10;
	if (tolmk >= tomk) {
		/* The PCI hole does not overlap the memory. */
		tolmk = tomk;
	}

	/* Report the memory regions. */
	idx = 10;
	ram_resource(dev, idx++, 0, 640);
	ram_resource(dev, idx++, 768, tolmk - 768);

	set_top_of_ram(tomk * 1024);

	assign_resources(dev->link_list);
}

static void cpu_pci_domain_read_resources(struct device *dev)
{
	pci_domain_read_resources(dev);
}

#if CONFIG_GENERATE_SMBIOS_TABLES
static int rdc_get_smbios_data16(int handle, unsigned long *current)
{
	struct smbios_type16 *t = (struct smbios_type16 *)*current;
	int len = sizeof(struct smbios_type16);

	memset(t, 0, sizeof(struct smbios_type16));
	t->type = SMBIOS_PHYS_MEMORY_ARRAY;
	t->handle = handle;
	t->length = len - 2;
	t->location = 3; /* Location: System Board */
	t->use = 3; /* System memory */
	t->memory_error_correction = 3; /* No error correction */
	t->maximum_capacity = get_memory_size();
	*current += len;
	return len;
}

static int rdc_get_smbios_data(device_t dev, int *handle, unsigned long *current)
{
	int len;
	len = rdc_get_smbios_data16(*handle, current);
	*handle += 1;
	return len;
}
#endif
static struct device_operations pci_domain_ops = {
	.read_resources		= cpu_pci_domain_read_resources,
	.set_resources		= cpu_pci_domain_set_resources,
	.enable_resources	= NULL,
	.init			= NULL,
	.scan_bus		= pci_domain_scan_bus,
	.ops_pci_bus	= pci_bus_default_ops,
#if CONFIG_GENERATE_SMBIOS_TABLES
	.get_smbios_data	= rdc_get_smbios_data,
#endif
};

static void enable_dev(struct device *dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		dev->ops = &pci_domain_ops;
	}
}

struct chip_operations northbridge_rdc_r8610_ops = {
	CHIP_NAME("RDC R8610 Northbridge")
	.enable_dev = enable_dev,
};
