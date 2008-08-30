/*
 * K8 northbridge 
 * This file is part of the coreboot project.
 * Copyright (C) 2004-2005 Linux Networx
 * (Written by Eric Biederman <ebiederman@lnxi.com> and Jason Schildt for Linux Networx)
 * Copyright (C) 2005-7 YingHai Lu
 * Copyright (C) 2005 Ollie Lo
 * Copyright (C) 2005-2007 Stefan Reinauer <stepan@openbios.org>
 * Copyright (C) 2008 Ronald G. Minnich <rminnich@gmail.com>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */
/* This should be done by Eric
	2004.12 yhlu add dual core support
	2005.01 yhlu add support move apic before pci_domain in MB Config.lb
	2005.02 yhlu add e0 memory hole support
	2005.11 yhlu add put sb ht chain on bus 0
*/

#include <mainboard.h>
#include <console.h>
#include <lib.h>
#include <string.h>
#include <mtrr.h>
#include <macros.h>
#include <spd.h>
#include <cpu.h>
#include <msr.h>
#include <amd/k8/k8.h>
#include <amd/k8/sysconf.h>
#include <device/pci.h>
#include <device/hypertransport_def.h>
#include <device/hypertransport.h>
#include <mc146818rtc.h>
#include <lib.h>
#include  <lapic.h>

int cpu_init_detected(unsigned nodeid)
{
	unsigned long htic;
	u32 dev;

	dev = PCI_BDF(0, 0x18 + nodeid, 0);
	htic = pci_conf1_read_config32(dev, HT_INIT_CONTROL);

	return !!(htic & HTIC_INIT_Detect);
}

int bios_reset_detected(void)
{
	unsigned long htic;
	htic = pci_conf1_read_config32(PCI_BDF(0, 0x18, 0), HT_INIT_CONTROL);

	return (htic & HTIC_ColdR_Detect) && !(htic & HTIC_BIOSR_Detect);
}

int cold_reset_detected(void)
{
	unsigned long htic;
	htic = pci_conf1_read_config32(PCI_BDF(0, 0x18, 0), HT_INIT_CONTROL);

	return !(htic & HTIC_ColdR_Detect);
}

void distinguish_cpu_resets(unsigned nodeid)
{
	u32 htic;
	u32 device;
	device = PCI_BDF(0, 0x18 + nodeid, 0);
	htic = pci_conf1_read_config32(device, HT_INIT_CONTROL);
	htic |= HTIC_ColdR_Detect | HTIC_BIOSR_Detect | HTIC_INIT_Detect;
	pci_conf1_write_config32(device, HT_INIT_CONTROL, htic);
}

void set_bios_reset(void)
{
	unsigned long htic;
	htic = pci_conf1_read_config32(PCI_BDF(0, 0x18, 0), HT_INIT_CONTROL);
	htic &= ~HTIC_BIOSR_Detect;
	pci_conf1_write_config32(PCI_BDF(0, 0x18, 0), HT_INIT_CONTROL, htic);
}

unsigned node_link_to_bus(unsigned node, unsigned link)
{
        unsigned reg;

        for(reg = 0xE0; reg < 0xF0; reg += 0x04) {
                unsigned config_map;
                config_map = pci_conf1_read_config32(PCI_BDF(0, 0x18, 1), reg);
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

unsigned get_sblk(void)
{
        u32 reg;
        /* read PCI_BDF(0,0x18,0) 0x64 bit [8:9] to find out SbLink m */
        reg = pci_conf1_read_config32(PCI_BDF(0, 0x18, 0), 0x64);
        return ((reg>>8) & 3) ;
}

unsigned get_sbbusn(unsigned sblk)
{
	return node_link_to_bus(0, sblk);
}


