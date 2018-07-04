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
 */

#include <arch/io.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci_ops.h>

#include <cpu/amd/amdfam10_sysconf.h>
#include "northbridge.h"
#include "amdfam10.h"
#include "ht_config.h"

struct dram_base_mask_t get_dram_base_mask(u32 nodeid)
{
	struct dram_base_mask_t d;
	struct device *dev = __f1_dev[0];

	u32 temp;
	temp = pci_read_config32(dev, 0x44 + (nodeid << 3)); //[39:24] at [31:16]
	d.mask = ((temp & 0xfff80000)>>(8+3)); // mask out  DramMask [26:24] too
	temp = pci_read_config32(dev, 0x144 + (nodeid <<3)) & 0xff; //[47:40] at [7:0]
	d.mask |= temp<<21;

	temp = pci_read_config32(dev, 0x40 + (nodeid << 3)); //[39:24] at [31:16]
	d.mask |= (temp & 1); // enable bit

	d.base = ((temp & 0xfff80000)>>(8+3)); // mask out DramBase [26:24) too
	temp = pci_read_config32(dev, 0x140 + (nodeid <<3)) & 0xff; //[47:40] at [7:0]
	d.base |= temp<<21;
	return d;
}

void set_config_map_reg(struct bus *link)
{
	u32 tempreg;
	u32 i;
	u32 ht_c_index = get_ht_c_index(link);
	u32 linkn = link->link_num & 0x0f;
	u32 busn_min = (link->secondary >> sysconf.segbit) & 0xff;
	u32 busn_max = (link->subordinate >> sysconf.segbit) & 0xff;
	u32 nodeid = amdfam10_nodeid(link->dev);

	tempreg = ((nodeid & 0x30) << (12-4)) | ((nodeid & 0xf) << 4) | 3;
	tempreg |= (busn_max << 24)|(busn_min << 16)|(linkn << 8);

	for (i = 0; i < sysconf.nodes; i++) {
		struct device *dev = __f1_dev[i];
		pci_write_config32(dev, 0xe0 + ht_c_index * 4, tempreg);
	}
}

void clear_config_map_reg(struct bus *link)
{
	u32 i;
	u32 ht_c_index = get_ht_c_index(link);

	for (i = 0; i < sysconf.nodes; i++) {
		struct device *dev = __f1_dev[i];
		pci_write_config32(dev, 0xe0 + ht_c_index * 4, 0);
	}
}


static u32 ht_c_key(struct bus *link)
{
	u32 nodeid = amdfam10_nodeid(link->dev);
	u32 linkn = link->link_num & 0x0f;
	u32 val = (linkn << 8) | ((nodeid & 0x3f) << 2) | 3;
	return val;
}

static u32 get_ht_c_index_by_key(u32 key, sys_info_conf_t *sysinfo)
{
	u32 ht_c_index = 0;

	for (ht_c_index = 0; ht_c_index < 32; ht_c_index++) {
		if ((sysinfo->ht_c_conf_bus[ht_c_index] & 0xfff) == key) {
			return ht_c_index;
		}
	}

	for (ht_c_index = 0; ht_c_index < 32; ht_c_index++) {
		if (sysinfo->ht_c_conf_bus[ht_c_index] == 0) {
			 return ht_c_index;
		}
	}

	return	-1;
}

u32 get_ht_c_index(struct bus *link)
{
	u32 val = ht_c_key(link);
	return get_ht_c_index_by_key(val, &sysconf);
}

void store_ht_c_conf_bus(struct bus *link)
{
	u32 val = ht_c_key(link);
	u32 ht_c_index = get_ht_c_index_by_key(val, &sysconf);

	u32 segn = (link->subordinate >> 8) & 0x0f;
	u32 busn_min = link->secondary & 0xff;
	u32 busn_max = link->subordinate & 0xff;

	val |= (segn << 28) | (busn_max << 20) | (busn_min << 12);

	sysconf.ht_c_conf_bus[ht_c_index] = val;
	sysconf.hcdn_reg[ht_c_index] = link->hcdn_reg;
	sysconf.ht_c_num++;
}

u32 get_io_addr_index(u32 nodeid, u32 linkn)
{
	u32 index;

	for (index = 0; index < 256; index++) {
		if (sysconf.conf_io_addrx[index+4] == 0) {
			sysconf.conf_io_addr[index+4] =  (nodeid & 0x3f);
			sysconf.conf_io_addrx[index+4] = 1 | ((linkn & 0x7)<<4);
			return index;
		 }
	 }

	 return	 0;
}

u32 get_mmio_addr_index(u32 nodeid, u32 linkn)
{
	u32 index;

	for (index = 0; index < 64; index++) {
		if (sysconf.conf_mmio_addrx[index+8] == 0) {
			sysconf.conf_mmio_addr[index+8] = (nodeid & 0x3f);
			sysconf.conf_mmio_addrx[index+8] = 1 | ((linkn & 0x7)<<4);
			return index;
		}
	}

	return 0;
}


void store_conf_io_addr(u32 nodeid, u32 linkn, u32 reg, u32 index,
				u32 io_min, u32 io_max)
{
	u32 val;

	/* io range allocation */
	index = (reg-0xc0)>>3;

	val = (nodeid & 0x3f); // 6 bits used
	sysconf.conf_io_addr[index] = val | ((io_max<<8) & 0xfffff000); //limit : with nodeid
	val = 3 | ((linkn & 0x7)<<4); // 8 bits used
	sysconf.conf_io_addrx[index] = val | ((io_min<<8) & 0xfffff000); // base : with enable bit

	if (sysconf.io_addr_num < (index+1))
		sysconf.io_addr_num = index+1;
}


void store_conf_mmio_addr(u32 nodeid, u32 linkn, u32 reg, u32 index,
					u32 mmio_min, u32 mmio_max)
{
	u32 val;

	/* io range allocation */
	index = (reg-0x80)>>3;

	val = (nodeid & 0x3f); // 6 bits used
	sysconf.conf_mmio_addr[index] = val | (mmio_max & 0xffffff00); //limit : with nodeid and linkn
	val = 3 | ((linkn & 0x7)<<4); // 8 bits used
	sysconf.conf_mmio_addrx[index] = val | (mmio_min & 0xffffff00); // base : with enable bit

	if (sysconf.mmio_addr_num<(index+1))
		sysconf.mmio_addr_num = index+1;
}


void set_io_addr_reg(struct device *dev, u32 nodeid, u32 linkn, u32 reg,
		     u32 io_min, u32 io_max)
{
	u32 i;
	u32 tempreg;

	/* io range allocation */
	tempreg = (nodeid&0xf) | ((nodeid & 0x30)<<(8-4)) | (linkn<<4) |  ((io_max&0xf0)<<(12-4)); //limit
	for (i = 0; i < sysconf.nodes; i++)
		pci_write_config32(__f1_dev[i], reg+4, tempreg);

	tempreg = 3 /*| (3<<4)*/ | ((io_min&0xf0)<<(12-4));	      //base :ISA and VGA ?
	for (i = 0; i < sysconf.nodes; i++)
		pci_write_config32(__f1_dev[i], reg, tempreg);
}

void set_mmio_addr_reg(u32 nodeid, u32 linkn, u32 reg, u32 index, u32 mmio_min, u32 mmio_max, u32 nodes)
{
	u32 i;
	u32 tempreg;

	/* io range allocation */
	tempreg = (nodeid&0xf) | (linkn<<4) |	 (mmio_max&0xffffff00); //limit
	for (i = 0; i < nodes; i++)
		pci_write_config32(__f1_dev[i], reg+4, tempreg);
	tempreg = 3 | (nodeid & 0x30) | (mmio_min&0xffffff00);
	for (i = 0; i < sysconf.nodes; i++)
		pci_write_config32(__f1_dev[i], reg, tempreg);
}
