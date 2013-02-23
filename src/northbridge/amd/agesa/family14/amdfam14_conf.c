/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
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

/*
 * No includes in this file because it is included into northbridge.c.
 */

struct dram_base_mask_t {
	u32 base; //[47:27] at [28:8]
	u32 mask; //[47:27] at [28:8] and enable at bit 0
};

static struct dram_base_mask_t get_dram_base_mask(u32 nodeid)
{
	device_t dev;
	struct dram_base_mask_t d;
#if defined(__PRE_RAM__)
	dev = PCI_DEV(CONFIG_CBB, CONFIG_CDB, 1);
#else
	dev = __f1_dev[0];
#endif	// defined(__PRE_RAM__)

	u32 temp;
	temp = pci_read_config32(dev, 0x44); //[39:24] at [31:16]
	d.mask = (temp & 0xffff0000); // mask out  DramMask [26:24] too

	temp = pci_read_config32(dev, 0x40); //[35:24] at [27:16]
	d.mask |= (temp & 1); // read enable bit

	d.base = (temp & 0x0fff0000); // mask out DramBase [26:24) too

	return d;
}

#if CONFIG_EXT_CONF_SUPPORT
static void set_addr_map_reg_4_6_in_one_node(u32 nodeid, u32 cfg_map_dest,
						u32 busn_min, u32 busn_max,
						u32 type)
{
	device_t dev;
	u32 i;
	u32 tempreg;
	u32 index_min, index_max;
	u32 dest_min, dest_max;
	index_min = busn_min>>2; dest_min = busn_min - (index_min<<2);
	index_max = busn_max>>2; dest_max = busn_max - (index_max<<2);

	// three case: index_min==index_max, index_min+1=index_max; index_min+1<index_max
#if defined(__PRE_RAM__)
	dev = NODE_PCI(nodeid, 1);
#else
	dev = __f1_dev[nodeid];
#endif	// defined(__PRE_RAM__)
	if(index_min== index_max) {
		pci_write_config32(dev, 0x110, index_min | (type<<28));
		tempreg = pci_read_config32(dev, 0x114);
		for(i=dest_min; i<=dest_max; i++) {
			tempreg &= ~(0xff<<(i*8));
			tempreg |= (cfg_map_dest<<(i*8));
		}
		pci_write_config32(dev, 0x110, index_min | (type<<28)); // do i need to write it again
		pci_write_config32(dev, 0x114, tempreg);
	} else if(index_min<index_max) {
		pci_write_config32(dev, 0x110, index_min | (type<<28));
		tempreg = pci_read_config32(dev, 0x114);
		for(i=dest_min; i<=3; i++) {
			tempreg &= ~(0xff<<(i*8));
			tempreg |= (cfg_map_dest<<(i*8));
		}
		pci_write_config32(dev, 0x110, index_min | (type<<28)); // do i need to write it again
		pci_write_config32(dev, 0x114, tempreg);

		pci_write_config32(dev, 0x110, index_max | (type<<28));
		tempreg = pci_read_config32(dev, 0x114);
		for(i=0; i<=dest_max; i++) {
			tempreg &= ~(0xff<<(i*8));
			tempreg |= (cfg_map_dest<<(i*8));
		}
		pci_write_config32(dev, 0x110, index_max | (type<<28)); // do i need to write it again
		pci_write_config32(dev, 0x114, tempreg);
		if((index_max-index_min)>1) {
			tempreg = 0;
			for(i=0; i<=3; i++) {
				tempreg &= ~(0xff<<(i*8));
				tempreg |= (cfg_map_dest<<(i*8));
			}
			for(i=index_min+1; i<index_max;i++) {
				pci_write_config32(dev, 0x110, i | (type<<28));
				pci_write_config32(dev, 0x114, tempreg);
			}
		}
	}
}
#endif	// CONFIG_EXT_CONF_SUPPORT

#if defined(__PRE_RAM__)
static void set_ht_c_io_addr_reg(u32 nodeid, u32 linkn, u32 ht_c_index,
					u32 io_min, u32 io_max, u32 nodes)
{
	u32 i;
	u32 tempreg;
	device_t dev;

#if CONFIG_EXT_CONF_SUPPORT
	if(ht_c_index<4) {
#endif
		/* io range allocation */
		tempreg = (nodeid&0xf) | ((nodeid & 0x30)<<(8-4)) | (linkn<<4) |  ((io_max&0xf0)<<(12-4)); //limit
		for(i=0; i<nodes; i++) {
			dev = NODE_PCI(i, 1);
			pci_write_config32(dev, 0xC4 + ht_c_index * 8, tempreg);
		}
		tempreg = 3 /*| ( 3<<4)*/ | ((io_min&0xf0)<<(12-4));	     //base :ISA and VGA ?
		for(i=0; i<nodes; i++){
			dev = NODE_PCI(i, 1);
			pci_write_config32(dev, 0xC0 + ht_c_index * 8, tempreg);
		}
#if CONFIG_EXT_CONF_SUPPORT
		return;
	}

	u32 cfg_map_dest;
	u32 j;

	// if ht_c_index > 3, We should use extend space

	if(io_min>io_max) return;

	// for nodeid at first
	cfg_map_dest = (1<<7) | (1<<6) | (linkn<<0);

	set_addr_map_reg_4_6_in_one_node(nodeid, cfg_map_dest, io_min, io_max, 4);

	// all other nodes
	cfg_map_dest = (1<<7) | (0<<6) | (nodeid<<0);
	for(j = 0; j< nodes; j++) {
		if(j== nodeid) continue;
		set_addr_map_reg_4_6_in_one_node(j,cfg_map_dest, io_min, io_max, 4);
	}
#endif	// CONFIG_EXT_CONF_SUPPORT
}


static void clear_ht_c_io_addr_reg(u32 nodeid, u32 linkn, u32 ht_c_index,
					u32 io_min, u32 io_max, u32 nodes)
{
	u32 i;
	device_t dev;
#if CONFIG_EXT_CONF_SUPPORT
	if(ht_c_index<4) {
#endif
		 /* io range allocation */
		for(i=0; i<nodes; i++) {
			dev = NODE_PCI(i, 1);
			pci_write_config32(dev, 0xC4 + ht_c_index * 8, 0);
			pci_write_config32(dev, 0xC0 + ht_c_index * 8, 0);
		}
#if CONFIG_EXT_CONF_SUPPORT
		return;
	}
	// : if hc_c_index > 3, We should use io_min, io_max to clear extend space
	u32 cfg_map_dest;
	u32 j;


	// all nodes
	cfg_map_dest = 0;
	for(j = 0; j< nodes; j++) {
		set_addr_map_reg_4_6_in_one_node(j,cfg_map_dest, io_min, io_max, 4);
	}
#endif
}
#endif // defined(__PRE_RAM__)

#if !defined(__PRE_RAM__)
static u32 get_io_addr_index(u32 nodeid, u32 linkn)
{
#if 0
	u32 index;

	for(index=0; index<256; index++) {
		if((sysconf.conf_io_addrx[index+4] == 0)){
			sysconf.conf_io_addr[index+4] =  (nodeid & 0x3f)  ;
			sysconf.conf_io_addrx[index+4] = 1 | ((linkn & 0x7)<<4);
			return index;
		 }
	 }
#endif
	 return	 0;
}

static u32 get_mmio_addr_index(u32 nodeid, u32 linkn)
{
#if 0
	u32 index;

	for(index=0; index<64; index++) {
		if((sysconf.conf_mmio_addrx[index+8] == 0)){
			sysconf.conf_mmio_addr[index+8] = (nodeid & 0x3f) ;
			sysconf.conf_mmio_addrx[index+8] = 1 | ((linkn & 0x7)<<4);
			return index;
		}
	}
#endif

	return	 0;
}

static void set_io_addr_reg(device_t dev, u32 nodeid, u32 linkn, u32 reg,
				u32 io_min, u32 io_max)
{

	u32 tempreg;
#if CONFIG_EXT_CONF_SUPPORT
	if(reg!=0x110) {
#endif
		/* io range allocation */
		tempreg = (nodeid&0xf) | ((nodeid & 0x30)<<(8-4)) | (linkn<<4) |  ((io_max&0xf0)<<(12-4)); //limit
		pci_write_config32(__f1_dev[0], reg+4, tempreg);

		tempreg = 3 /*| ( 3<<4)*/ | ((io_min&0xf0)<<(12-4));	      //base :ISA and VGA ?
#if 0
		// FIXME: can we use VGA reg instead?
		if (dev->link[link].bridge_ctrl & PCI_BRIDGE_CTL_VGA) {
			printk(BIOS_SPEW, "%s, enabling legacy VGA IO forwarding for %s link %s\n",
				__func__, dev_path(dev), link);
			tempreg |= PCI_IO_BASE_VGA_EN;
		}
		if (dev->link[link].bridge_ctrl & PCI_BRIDGE_CTL_NO_ISA) {
			tempreg |= PCI_IO_BASE_NO_ISA;
		}
#endif
		pci_write_config32(__f1_dev[0], reg, tempreg);
#if CONFIG_EXT_CONF_SUPPORT
		return;
	}

	u32 cfg_map_dest;
	u32 j;
	// if ht_c_index > 3, We should use extend space
	if(io_min>io_max) return;
	// for nodeid at first
	cfg_map_dest = (1<<7) | (1<<6) | (linkn<<0);

	set_addr_map_reg_4_6_in_one_node(nodeid, cfg_map_dest, io_min, io_max, 4);
#endif // CONFIG_EXT_CONF_SUPPORT
}


static void set_mmio_addr_reg(u32 nodeid, u32 linkn, u32 reg, u32 index, u32 mmio_min, u32 mmio_max, u32 nodes)
{

	u32 tempreg;
#if CONFIG_EXT_CONF_SUPPORT
	if(reg!=0x110) {
#endif
		/* io range allocation */
		tempreg = (nodeid&0xf) | (linkn<<4) |	 (mmio_max&0xffffff00); //limit
		pci_write_config32(__f1_dev[0], reg+4, tempreg);
		tempreg = 3 | (nodeid & 0x30) | (mmio_min&0xffffff00);
		pci_write_config32(__f1_dev[0], reg, tempreg);
#if CONFIG_EXT_CONF_SUPPORT
		return;
	}

	device_t dev;
	u32 j;
	// if ht_c_index > 3, We should use extend space
	// for nodeid at first
	u32 enable;

	if(mmio_min>mmio_max) {
		return;
	}

	enable = 1;

	dev = __f1_dev[nodeid];
	tempreg = ((mmio_min>>3) & 0x1fffff00)| (1<<6) | (linkn<<0);
	pci_write_config32(dev, 0x110, index | (2<<28));
	pci_write_config32(dev, 0x114, tempreg);

	tempreg = ((mmio_max>>3) & 0x1fffff00) | enable;
	pci_write_config32(dev, 0x110, index | (3<<28));
	pci_write_config32(dev, 0x114, tempreg);
#endif	// CONFIG_EXT_CONF_SUPPORT
}

#endif // !defined(__PRE_RAM__)

/********************************************************************
* Change the vendor / device IDs to match the generic VBIOS header.
********************************************************************/
u32 map_oprom_vendev(u32 vendev)
{
	u32 new_vendev=vendev;

	switch(vendev) {
	case 0x10029809:
	case 0x10029808:
	case 0x10029807:
	case 0x10029806:
	case 0x10029805:
	case 0x10029804:
	case 0x10029803:
		new_vendev=0x10029802;
		break;
	}

	return new_vendev;
}
