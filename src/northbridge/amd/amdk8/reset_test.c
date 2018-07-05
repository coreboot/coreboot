/*
 * This file is part of the coreboot project.
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
#include <console/console.h>
#include <stdint.h>
#include <cpu/x86/lapic.h>
#include "amdk8.h"

static inline int cpu_init_detected(unsigned nodeid)
{
	u32 htic;
	pci_devfn_t dev;

	dev = PCI_DEV(0, 0x18 + nodeid, 0);
	htic = pci_io_read_config32(dev, HT_INIT_CONTROL);

	return !!(htic & HTIC_INIT_Detect);
}

static inline int bios_reset_detected(void)
{
	u32 htic;
	htic = pci_io_read_config32(PCI_DEV(0, 0x18, 0), HT_INIT_CONTROL);

	return (htic & HTIC_ColdR_Detect) && !(htic & HTIC_BIOSR_Detect);
}

static inline int cold_reset_detected(void)
{
	u32 htic;
	htic = pci_io_read_config32(PCI_DEV(0, 0x18, 0), HT_INIT_CONTROL);

	return !(htic & HTIC_ColdR_Detect);
}

void distinguish_cpu_resets(unsigned int nodeid)
{
	u32 htic;
	pci_devfn_t device;
	device = PCI_DEV(0, 0x18 + nodeid, 0);
	htic = pci_io_read_config32(device, HT_INIT_CONTROL);
	htic |= HTIC_ColdR_Detect | HTIC_BIOSR_Detect | HTIC_INIT_Detect;
	pci_io_write_config32(device, HT_INIT_CONTROL, htic);
}

void set_bios_reset(void)
{
	u32 htic;
	htic = pci_io_read_config32(PCI_DEV(0, 0x18, 0), HT_INIT_CONTROL);
	htic &= ~HTIC_BIOSR_Detect;
	pci_io_write_config32(PCI_DEV(0, 0x18, 0), HT_INIT_CONTROL, htic);
}

static unsigned node_link_to_bus(unsigned node, unsigned link)
{
	u8 reg;

	for (reg = 0xE0; reg < 0xF0; reg += 0x04) {
		u32 config_map;
		config_map = pci_io_read_config32(PCI_DEV(0, 0x18, 1), reg);
		if ((config_map & 3) != 3) {
			continue;
		}
		if ((((config_map >> 4) & 7) == node) &&
			(((config_map >> 8) & 3) == link))
		{
			return (config_map >> 16) & 0xff;
		}
	}
	return 0;
}

unsigned int get_sblk(void)
{
	u32 reg;
	/* read PCI_DEV(0,0x18,0) 0x64 bit [8:9] to find out SbLink m */
	reg = pci_io_read_config32(PCI_DEV(0, 0x18, 0), 0x64);
	return ((reg>>8) & 3);
}

unsigned int get_sbbusn(unsigned sblk)
{
	return node_link_to_bus(0, sblk);
}
