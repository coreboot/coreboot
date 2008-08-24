/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2002 Linux Networx
 * (Written by Eric Biederman <ebiederman@lnxi.com> for Linux Networx)
 * Copyright (C) 2004 YingHai Lu
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
/*	This should be done by Eric
	2004.11 yhlu add 4 rank DIMM support
	2004.12 yhlu add D0 support
	2005.02 yhlu add E0 memory hole support
*/

#include <console.h>
#include <mtrr.h>
#include <macros.h>
#include <spd.h>
#include <cpu.h>
#include <msr.h>
#include <amd/k8/k8.h>
#include <amd/k8/sysconf.h>
#include <device/pci.h>
#include <mc146818rtc.h>
#include <lib.h>

#define NODE_ID		0x60
#define	HT_INIT_CONTROL 0x6c

#define HTIC_ColdR_Detect  (1<<4)
#define HTIC_BIOSR_Detect  (1<<5)
#define HTIC_INIT_Detect   (1<<6)

int cpu_init_detected(unsigned int nodeid)
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

void distinguish_cpu_resets(unsigned int  nodeid)
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

u8 node_link_to_bus(unsigned int  node, unsigned int  link)
{
        u16  reg;

        for(reg = 0xE0; reg < 0xF0; reg += 0x04) {
                u32 config_map;
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

u32 get_sblk(void)
{
        u32 reg;
        /* read PCI_BDF(0,0x18,0) 0x64 bit [8:9] to find out SbLink m */
        reg = pci_conf1_read_config32(PCI_BDF(0, 0x18, 0), 0x64);
        return ((reg>>8) & 3) ;
}

u8 get_sbbusn(unsigned int sblk)
{
	return node_link_to_bus(0, sblk);
}


