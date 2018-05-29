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
	struct device *dev;
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

#if defined(__PRE_RAM__)
static void set_ht_c_io_addr_reg(u32 nodeid, u32 linkn, u32 ht_c_index,
					u32 io_min, u32 io_max, u32 nodes)
{
	u32 i;
	u32 tempreg;
	struct device *dev;

	/* io range allocation */
	tempreg = (nodeid&0xf) | ((nodeid & 0x30)<<(8-4)) | (linkn << 4) |  ((io_max&0xf0)<<(12-4)); //limit
	for (i = 0; i < nodes; i++) {
		dev = NODE_PCI(i, 1);
		pci_write_config32(dev, 0xC4 + ht_c_index * 8, tempreg);
	}
	tempreg = 3 /*| (3 << 4)*/ | ((io_min&0xf0)<<(12-4));	     //base :ISA and VGA ?
	for (i = 0; i < nodes; i++) {
		dev = NODE_PCI(i, 1);
		pci_write_config32(dev, 0xC0 + ht_c_index * 8, tempreg);
	}
}


static void clear_ht_c_io_addr_reg(u32 nodeid, u32 linkn, u32 ht_c_index,
					u32 io_min, u32 io_max, u32 nodes)
{
	u32 i;
	struct device *dev;

	 /* io range allocation */
	for (i = 0; i < nodes; i++) {
		dev = NODE_PCI(i, 1);
		pci_write_config32(dev, 0xC4 + ht_c_index * 8, 0);
		pci_write_config32(dev, 0xC0 + ht_c_index * 8, 0);
	}
}
#endif // defined(__PRE_RAM__)

#if !defined(__PRE_RAM__)
static u32 get_io_addr_index(u32 nodeid, u32 linkn)
{
	 return	 0;
}

static u32 get_mmio_addr_index(u32 nodeid, u32 linkn)
{
	return	 0;
}

static void set_io_addr_reg(struct device *dev, u32 nodeid, u32 linkn, u32 reg,
			    u32 io_min, u32 io_max)
{

	u32 tempreg;
	/* io range allocation */
	tempreg = (nodeid&0xf) | ((nodeid & 0x30)<<(8-4)) | (linkn << 4) |  ((io_max&0xf0)<<(12-4)); //limit
	pci_write_config32(__f1_dev[0], reg+4, tempreg);

	tempreg = 3 /*| (3 << 4)*/ | ((io_min&0xf0)<<(12-4));	      //base :ISA and VGA ?
	pci_write_config32(__f1_dev[0], reg, tempreg);
}


static void set_mmio_addr_reg(u32 nodeid, u32 linkn, u32 reg, u32 index, u32 mmio_min, u32 mmio_max, u32 nodes)
{

	u32 tempreg;
		/* io range allocation */
		tempreg = (nodeid&0xf) | (linkn << 4) |	 (mmio_max&0xffffff00); //limit
		pci_write_config32(__f1_dev[0], reg+4, tempreg);
		tempreg = 3 | (nodeid & 0x30) | (mmio_min&0xffffff00);
		pci_write_config32(__f1_dev[0], reg, tempreg);
}

#endif // !defined(__PRE_RAM__)

/********************************************************************
* Change the vendor / device IDs to match the generic VBIOS header.
********************************************************************/
u32 map_oprom_vendev(u32 vendev)
{
	u32 new_vendev = vendev;

	switch(vendev) {
	case 0x10029809:
	case 0x10029808:
	case 0x10029807:
	case 0x10029806:
	case 0x10029805:
	case 0x10029804:
	case 0x10029803:
		new_vendev = 0x10029802;
		break;
	}

	return new_vendev;
}
