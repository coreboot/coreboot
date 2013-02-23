/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
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

#include <stdint.h>
#include <cpu/x86/lapic.h>
#include "amdfam10.h"

#define NODE_ID		0x60
#define HT_INIT_CONTROL	0x6c
#define HTIC_ColdR_Detect	(1<<4)
#define HTIC_BIOSR_Detect	(1<<5)
#define HTIC_INIT_Detect	(1<<6)

/* mmconf is not ready */
/* io_ext is not ready */
u32 cpu_init_detected(u8 nodeid)
{
	u32 htic;
	device_t dev;

	dev = NODE_PCI(nodeid, 0);
	htic = pci_io_read_config32(dev, HT_INIT_CONTROL);

	return !!(htic & HTIC_INIT_Detect);
}

u32 bios_reset_detected(void)
{
	u32 htic;
	htic = pci_io_read_config32(PCI_DEV(CONFIG_CBB, CONFIG_CDB, 0), HT_INIT_CONTROL);

	return (htic & HTIC_ColdR_Detect) && !(htic & HTIC_BIOSR_Detect);
}

u32 cold_reset_detected(void)
{
	u32 htic;
	htic = pci_io_read_config32(PCI_DEV(CONFIG_CBB, CONFIG_CDB, 0), HT_INIT_CONTROL);

	return !(htic & HTIC_ColdR_Detect);
}

u32 other_reset_detected(void)	// other warm reset not started by BIOS
{
	u32 htic;
	htic = pci_io_read_config32(PCI_DEV(CONFIG_CBB, CONFIG_CDB, 0), HT_INIT_CONTROL);

	return (htic & HTIC_ColdR_Detect) && (htic & HTIC_BIOSR_Detect);
}

static void distinguish_cpu_resets(u8 nodeid)
{
	u32 htic;
	device_t device;
	device = NODE_PCI(nodeid, 0);
	htic = pci_io_read_config32(device, HT_INIT_CONTROL);
	htic |= HTIC_ColdR_Detect | HTIC_BIOSR_Detect | HTIC_INIT_Detect;
	pci_io_write_config32(device, HT_INIT_CONTROL, htic);
}

static u32 warm_reset_detect(u8 nodeid)
{
	u32 htic;
	device_t device;
	device = NODE_PCI(nodeid, 0);
	htic = pci_io_read_config32(device, HT_INIT_CONTROL);
	return (htic & HTIC_ColdR_Detect) && !(htic & HTIC_BIOSR_Detect);
}

void __attribute__ ((weak)) set_bios_reset(void);
void __attribute__ ((weak)) set_bios_reset(void)
{

	u32 nodes;
	u32 htic;
	device_t dev;
	int i;

	nodes = ((pci_read_config32(PCI_DEV(CONFIG_CBB, CONFIG_CDB, 0), 0x60) >> 4) & 7) + 1;

	for(i = 0; i < nodes; i++) {
		dev = NODE_PCI(i,0);
		htic = pci_read_config32(dev, HT_INIT_CONTROL);
		htic &= ~HTIC_BIOSR_Detect;
		pci_write_config32(dev, HT_INIT_CONTROL, htic);
	}
}


/* Look up a which bus a given node/link combination is on.
 * return 0 when we can't find the answer.
 */
static u8 node_link_to_bus(u8 node, u8 link) // node are 6 bit, and link three bit
{
	u32 reg;
	u32 val;

	// put node and link in correct bit
	val = ((node & 0x0f)<<4) | ((node & 0x30)<< (12-4)) | ((link & 0x07)<<8) ;

	for(reg = 0xE0; reg < 0xF0; reg += 0x04) {
		u32 config_map;
		config_map = pci_io_read_config32(PCI_DEV(CONFIG_CBB, CONFIG_CDB, 1), reg);
		if ((config_map & 3) != 3) {
			continue;
		}
		if ((config_map & (((63 & 0x0f)<<4) | ((63 & 0x30)<< (12-4)) | ((7 & 0x07)<<8) )
			) == val )
		{
			return (config_map >> 16) & 0xff;
		}
	}

#if CONFIG_EXT_CONF_SUPPORT
	// let's check that in extend space
	// use the nodeid extend space to find out the bus for the linkn
	u32 tempreg;
	int i;
	int j;
	u32 cfg_map_dest;
	device_t dev;

	cfg_map_dest = (1<<7)|(1<<6)|link;

	// three case: index_min==index_max, index_min+1=index_max; index_min+1<index_max
	dev = NODE_PCI(node, 1);
	for(j=0; j<64; j++) {
		pci_io_write_config32(dev, 0x110, j | (6<<28));
		tempreg = pci_io_read_config32(dev, 0x114);
		for(i=0; i<=3; i++) {
			tempreg >>= (i*8);
			if((tempreg & ((1<<7)|(1<<6)|0x3f)) == cfg_map_dest) {
				return (i+(j<<2)); //busn_min
			}
		}
	}
#endif

	return 0;
}

u32 get_sblk(void)
{
	u32 reg;
	/* read PCI_DEV(CONFIG_CBB,CONFIG_CDB,0) 0x64 bit [8:9] to find out SbLink m */
	reg = pci_io_read_config32(PCI_DEV(CONFIG_CBB, CONFIG_CDB, 0), 0x64);
	return ((reg>>8) & 3) ;
}


u8 get_sbbusn(u8 sblk)
{
	return node_link_to_bus(0, sblk);
}

