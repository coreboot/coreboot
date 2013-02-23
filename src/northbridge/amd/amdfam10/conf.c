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

#if defined(__PRE_RAM__)
typedef struct sys_info sys_info_conf_t;
#else
typedef struct amdfam10_sysconf_t sys_info_conf_t;
#endif

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
#endif

#if CONFIG_EXT_CONF_SUPPORT
	// I will use ext space only for simple
	pci_write_config32(dev, 0x110, nodeid | (1<<28)); // [47:27] at [28:8]
	d.mask = pci_read_config32(dev, 0x114);  // enable is bit 0
	pci_write_config32(dev, 0x110, nodeid | (0<<28));
	d.base = pci_read_config32(dev, 0x114) & 0x1fffff00; //[47:27] at [28:8];
#else
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
#endif
	return d;
}

#if !CONFIG_AMDMCT
static void set_dram_base_mask(u32 nodeid, struct dram_base_mask_t d, u32 nodes)
{
	u32 i;
	device_t dev;
#if CONFIG_EXT_CONF_SUPPORT
	// I will use ext space only for simple
	u32 d_base_i, d_base_d, d_mask_i, d_mask_d;
	d_base_i = nodeid | (0<<28);
	d_base_d = d.base | nodeid; //[47:27] at [28:8];
	d_mask_i = nodeid | (1<<28); // [47:27] at [28:8]
	d_mask_d = d.mask;  // enable is bit 0

#else
	u32 d_base_lo, d_base_hi, d_mask_lo, d_mask_hi;
	u32 d_base_lo_reg, d_base_hi_reg, d_mask_lo_reg, d_mask_hi_reg;
	d_mask_lo =  (((d.mask<<(8+3))|(0x07<<16)) & 0xffff0000)|nodeid; // need to fill DramMask[26:24] with ones
	d_mask_hi =  (d.mask>>21) & 0xff;
	d_base_lo = ((d.base<<(8+3)) & 0xffff0000);
	if(d.mask & 1) d_base_lo |= 3;
	d_base_hi = (d.base>>21) & 0xff;
	d_mask_lo_reg = 0x44+(nodeid<<3);
	d_mask_hi_reg = 0x144+(nodeid<<3);
	d_base_lo_reg = 0x40+(nodeid<<3);
	d_base_hi_reg = 0x140+(nodeid<<3);
#endif

	for(i=0;i<nodes;i++) {
#if defined(__PRE_RAM__)
		dev = NODE_PCI(i, 1);
#else
		dev = __f1_dev[i];
#endif

#if CONFIG_EXT_CONF_SUPPORT
		// I will use ext space only for simple
		pci_write_config32(dev, 0x110, d_base_i);
		pci_write_config32(dev, 0x114, d_base_d); //[47:27] at [28:8];
		pci_write_config32(dev, 0x110, d_mask_i); // [47:27] at [28:8]
		pci_write_config32(dev, 0x114, d_mask_d);  // enable is bit 0
#else
		pci_write_config32(dev, d_mask_lo_reg, d_mask_lo); // need to fill DramMask[26:24] with ones
		pci_write_config32(dev, d_mask_hi_reg, d_mask_hi);
		pci_write_config32(dev, d_base_lo_reg, d_base_lo);
		pci_write_config32(dev, d_base_hi_reg, d_base_hi);
#endif
	}

#if defined(__PRE_RAM__)
	dev = NODE_PCI(nodeid, 1);
#else
	dev = __f1_dev[nodeid];
#endif
	pci_write_config32(dev, 0x120, d.base>>8);
	pci_write_config32(dev, 0x124, d.mask>>8);

}
#endif

#if !CONFIG_AMDMCT
static void set_DctSelBaseAddr(u32 i, u32 sel_m)
{
	device_t dev;
#if defined(__PRE_RAM__)
	dev = NODE_PCI(i, 2);
#else
		dev = __f2_dev[i];
#endif
	u32 dcs_lo;
	dcs_lo = pci_read_config32(dev, DRAM_CTRL_SEL_LOW);
	dcs_lo &= ~(DCSL_DctSelBaseAddr_47_27_MASK<<DCSL_DctSelBaseAddr_47_27_SHIFT);
	dcs_lo |= (sel_m<<(20+DCSL_DctSelBaseAddr_47_27_SHIFT-27));
	pci_write_config32(dev, DRAM_CTRL_SEL_LOW, dcs_lo);

}


static u32 get_DctSelBaseAddr(u32 i)
{
	device_t dev;
#if defined(__PRE_RAM__)
	dev = NODE_PCI(i, 2);
#else
		dev = __f2_dev[i];
#endif
	u32 sel_m;
	u32 dcs_lo;
	dcs_lo = pci_read_config32(dev, DRAM_CTRL_SEL_LOW);
	dcs_lo &= DCSL_DctSelBaseAddr_47_27_MASK<<DCSL_DctSelBaseAddr_47_27_SHIFT;
	sel_m = dcs_lo>>(20+DCSL_DctSelBaseAddr_47_27_SHIFT-27);
	return sel_m;
}

#ifdef UNUSED_CODE
static void set_DctSelHiEn(u32 i, u32 val)
{
	device_t dev;
#if defined(__PRE_RAM__)
	dev = NODE_PCI(i, 2);
#else
		dev = __f2_dev[i];
#endif
	u32 dcs_lo;
	dcs_lo = pci_read_config32(dev, DRAM_CTRL_SEL_LOW);
	dcs_lo &= ~(7);
	dcs_lo |= (val & 7);
	pci_write_config32(dev, DRAM_CTRL_SEL_LOW, dcs_lo);

}
#endif

static u32 get_DctSelHiEn(u32 i)
{
	device_t dev;
#if defined(__PRE_RAM__)
	dev = NODE_PCI(i, 2);
#else
	dev = __f2_dev[i];
#endif
	u32 dcs_lo;
	dcs_lo = pci_read_config32(dev, DRAM_CTRL_SEL_LOW);
	dcs_lo &= 7;
	return dcs_lo;

}

static void set_DctSelBaseOffset(u32 i, u32 sel_off_m)
{
	device_t dev;
#if defined(__PRE_RAM__)
	dev = NODE_PCI(i, 2);
#else
	dev = __f2_dev[i];
#endif
	u32 dcs_hi;
	dcs_hi = pci_read_config32(dev, DRAM_CTRL_SEL_HIGH);
	dcs_hi &= ~(DCSH_DctSelBaseOffset_47_26_MASK<<DCSH_DctSelBaseOffset_47_26_SHIFT);
	dcs_hi |= sel_off_m<<(20+DCSH_DctSelBaseOffset_47_26_SHIFT-26);
	pci_write_config32(dev, DRAM_CTRL_SEL_HIGH, dcs_hi);

}

#ifdef UNUSED_CODE
static u32 get_DctSelBaseOffset(u32 i)
{
	device_t dev;
#if defined(__PRE_RAM__)
	dev = NODE_PCI(i, 2);
#else
	dev = __f2_dev[i];
#endif
	u32 sel_off_m;
	u32 dcs_hi;
	dcs_hi = pci_read_config32(dev, DRAM_CTRL_SEL_HIGH);
	dcs_hi &= DCSH_DctSelBaseOffset_47_26_MASK<<DCSH_DctSelBaseOffset_47_26_SHIFT;
	sel_off_m = dcs_hi>>(20+DCSH_DctSelBaseOffset_47_26_SHIFT-26);
	return sel_off_m;
}
#endif

static u32 get_one_DCT(struct mem_info *meminfo)
{
	u32 one_DCT = 1;
	if(meminfo->is_Width128) {
		one_DCT = 1;
	} else {
		u32 dimm_mask = meminfo->dimm_mask;
		if((dimm_mask >> DIMM_SOCKETS) && (dimm_mask & ((1<<DIMM_SOCKETS)-1))) {
			 one_DCT = 0;
		}
	}

	return one_DCT;
}

#if CONFIG_HW_MEM_HOLE_SIZEK != 0
// See that other copy in northbridge.c
static u32 hoist_memory(u32 hole_startk, u32 i, u32 one_DCT, u32 nodes)
{
	u32 ii;
	u32 carry_over;
	device_t dev;
	struct dram_base_mask_t d;
	u32 sel_m;
	u32 sel_hi_en;
	u32 hoist;


	carry_over = (4*1024*1024) - hole_startk;

	for(ii=nodes - 1;ii>i;ii--) {
		d = get_dram_base_mask(ii);
		if(!(d.mask & 1)) continue;
		d.base += (carry_over>>9);
		d.mask += (carry_over>>9);
		set_dram_base_mask(ii, d, nodes);

		if(get_DctSelHiEn(ii) & 1) {
			sel_m = get_DctSelBaseAddr(ii);
			sel_m += carry_over>>10;
			set_DctSelBaseAddr(ii, sel_m);
		}

	}
	d = get_dram_base_mask(i);
	d.mask += (carry_over>>9);
	set_dram_base_mask(i,d, nodes);
#if defined(__PRE_RAM__)
	dev = NODE_PCI(i, 1);
#else
	dev = __f1_dev[i];
#endif
	sel_hi_en =  get_DctSelHiEn(i);
	if(sel_hi_en & 1) {
		sel_m = get_DctSelBaseAddr(i);
	}
	if(d.base == (hole_startk>>9)) {
		//don't need set memhole here, because hole off set will be 0, overflow
		//so need to change base reg instead, new basek will be 4*1024*1024
		d.base = (4*1024*1024)>>9;
		set_dram_base_mask(i, d, nodes);

		if(sel_hi_en & 1) {
			sel_m += carry_over>>10;
			set_DctSelBaseAddr(i, sel_m);
		}
	} else {
		hoist = /* hole start address */
			((hole_startk << 10) & 0xff000000) +
			/* enable */
			1;
		if(one_DCT||(sel_m>=(hole_startk>>10))) { //one DCT or hole in DCT0
			hoist +=
			/* hole address to memory controller address */
			((((d.base<<9) + carry_over) >> 6) & 0x0000ff00) ;

			if(sel_hi_en & 1) {
				sel_m += (carry_over>>10);
				set_DctSelBaseAddr(i, sel_m);
				set_DctSelBaseOffset(i, sel_m);
			}
		} else { // hole in DCT1 range
			hoist +=
			/* hole address to memory controller address */
			((((sel_m<<10) + carry_over) >> 6) & 0x0000ff00) ;
			// don't need to update DctSelBaseAddr
			if(sel_hi_en & 1) {
				set_DctSelBaseOffset(i, sel_m);
			}
		}
		pci_write_config32(dev, 0xf0, hoist);

	}

	return carry_over;
}
#endif
#endif // CONFIG_AMDMCT


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
#endif
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
#endif

static void set_config_map_reg(u32 nodeid, u32 linkn, u32 ht_c_index,
				u32 busn_min, u32 busn_max, u32 segbit,
				u32 nodes)
{
	u32 tempreg;
	u32 i;
	device_t dev;

	busn_min>>=segbit;
	busn_max>>=segbit;

#if CONFIG_EXT_CONF_SUPPORT
	if(ht_c_index < 4) {
#endif
		tempreg = 3 | ((nodeid&0xf)<<4) | ((nodeid & 0x30)<<(12-4))|(linkn<<8)|((busn_min & 0xff)<<16)|((busn_max&0xff)<<24);
		for(i=0; i<nodes; i++) {
		#if defined(__PRE_RAM__)
			dev = NODE_PCI(i, 1);
		#else
			dev = __f1_dev[i];
		#endif
			pci_write_config32(dev, 0xe0 + ht_c_index * 4, tempreg);
		}
#if CONFIG_EXT_CONF_SUPPORT

		return;
	}

	// if ht_c_index > 3, We should use extend space x114_x6
	u32 cfg_map_dest;
	u32 j;

	// for nodeid at first
	cfg_map_dest = (1<<7) | (1<<6) | (linkn<<0);

	set_addr_map_reg_4_6_in_one_node(nodeid, cfg_map_dest, busn_min, busn_max, 6);

	// all other nodes
	cfg_map_dest = (1<<7) | (0<<6) | (nodeid<<0);
	for(j = 0; j< nodes; j++) {
		if(j== nodeid) continue;
		set_addr_map_reg_4_6_in_one_node(j,cfg_map_dest, busn_min, busn_max, 6);
	}
#endif
}

static void clear_config_map_reg(u32 nodeid, u32 linkn, u32 ht_c_index,
					u32 busn_min, u32 busn_max, u32 nodes)
{
	u32 i;
	device_t dev;

#if CONFIG_EXT_CONF_SUPPORT
	if(ht_c_index<4) {
#endif
		for(i=0; i<nodes; i++) {
		#if defined(__PRE_RAM__)
			dev = NODE_PCI(i, 1);
		#else
			dev = __f1_dev[i];
		#endif
			pci_write_config32(dev, 0xe0 + ht_c_index * 4, 0);
		}
#if CONFIG_EXT_CONF_SUPPORT
		return;
	}

	// if hc_c_index >3, We should use busn_min and busn_max to clear extend space
	u32 cfg_map_dest;
	u32 j;


	// all nodes
	cfg_map_dest = 0;
	for(j = 0; j< nodes; j++) {
		set_addr_map_reg_4_6_in_one_node(j,cfg_map_dest, busn_min, busn_max, 6);
	}
#endif

}

#if CONFIG_PCI_BUS_SEGN_BITS
static u32 check_segn(device_t dev, u32 segbusn, u32 nodes,
			sys_info_conf_t *sysinfo)
{
	//check segbusn here, We need every node have the same segn
	if((segbusn & 0xff)>(0xe0-1)) {// use next segn
		u32 segn = (segbusn >> 8) & 0x0f;
		segn++;
		segbusn = segn<<8;
	}
	if(segbusn>>8) {
		u32 val;
		val = pci_read_config32(dev, 0x160);
		val &= ~(0xf<<25);
		val |= (segbusn & 0xf00)<<(25-8);
		pci_write_config32(dev, 0x160, val);
	}

	return segbusn;
}
#endif

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
		#if defined(__PRE_RAM__)
			dev = NODE_PCI(i, 1);
		#else
			dev = __f1_dev[i];
		#endif
			pci_write_config32(dev, 0xC4 + ht_c_index * 8, tempreg);
		}
		tempreg = 3 /*| ( 3<<4)*/ | ((io_min&0xf0)<<(12-4));	     //base :ISA and VGA ?
		for(i=0; i<nodes; i++){
		#if defined(__PRE_RAM__)
			dev = NODE_PCI(i, 1);
		#else
			dev = __f1_dev[i];
		#endif
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
#endif
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
		#if defined(__PRE_RAM__)
			dev = NODE_PCI(i, 1);
		#else
			dev = __f1_dev[i];
		#endif
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
#endif

#ifdef UNUSED_CODE
static void re_set_all_config_map_reg(u32 nodes, u32 segbit,
					sys_info_conf_t *sysinfo)
{
	u32 ht_c_index;
	device_t dev;

	set_config_map_reg(0, sysinfo->sblk, 0, 0, sysinfo->ht_c_conf_bus[0]>>20, segbit, nodes);

	/* clean others */
	for(ht_c_index=1;ht_c_index<4; ht_c_index++) {
		u32 i;
		for(i=0; i<nodes; i++) {
		#if defined(__PRE_RAM__)
			dev = NODE_PCI(i, 1);
		#else
			dev = __f1_dev[i];
		#endif
			pci_write_config32(dev, 0xe0 + ht_c_index * 4, 0);
		}
	}
#if CONFIG_EXT_CONF_SUPPORT
	u32 j;
	// clear the extend space
	for(j = 0; j< nodes; j++) {
		set_addr_map_reg_4_6_in_one_node(j,0, 0, 0xff, 6);
	}
#endif

	for(ht_c_index = 1; ht_c_index<sysinfo->ht_c_num; ht_c_index++) {
		u32 nodeid, linkn;
		u32 busn_max;
		u32 busn_min;
		nodeid = (sysinfo->ht_c_conf_bus[ht_c_index] >> 2) & 0x3f;
		linkn = (sysinfo->ht_c_conf_bus[ht_c_index]>>8) & 0x7;
		busn_max = sysinfo->ht_c_conf_bus[ht_c_index]>>20;
		busn_min = (sysinfo->ht_c_conf_bus[ht_c_index]>>12) & 0xff;
		busn_min |= busn_max & 0xf00;
		set_config_map_reg(nodeid, linkn, ht_c_index, busn_min, busn_max, segbit, nodes);
	}

}
#endif

static u32 get_ht_c_index(u32 nodeid, u32 linkn, sys_info_conf_t *sysinfo)
{
	u32 tempreg;
	u32 ht_c_index = 0;

#if 0
	tempreg = 3 | ((nodeid & 0xf) <<4) | ((nodeid & 0x30)<<(12-4)) | (linkn<<8);

	for(ht_c_index=0;ht_c_index<4; ht_c_index++) {
		reg = pci_read_config32(PCI_DEV(CONFIG_CBB, CONFIG_CDB, 1), 0xe0 + ht_c_index * 4);
		if(((reg & 0xffff) == 0x0000)) {  /*found free*/
			break;
		}
	}
#endif
	tempreg = 3 | ((nodeid & 0x3f)<<2) | (linkn<<8);
	for(ht_c_index=0; ht_c_index<32; ht_c_index++) {
		if(((sysinfo->ht_c_conf_bus[ht_c_index] & 0xfff) == tempreg)){
			return ht_c_index;
		}
	}

	for(ht_c_index=0; ht_c_index<32; ht_c_index++) {
		if((sysinfo->ht_c_conf_bus[ht_c_index] == 0)){
			 return ht_c_index;
		}
	}

	return	-1;

}

static void store_ht_c_conf_bus(u32 nodeid, u32 linkn, u32 ht_c_index,
				u32 busn_min, u32 busn_max,
				sys_info_conf_t *sysinfo)
{
	u32 val;
	val = 3 | ((nodeid & 0x3f)<<2) | (linkn<<8);
	sysinfo->ht_c_conf_bus[ht_c_index] = val | ((busn_min & 0xff) <<12) | (busn_max<<20);  // same node need segn are same

}

#ifdef UNUSED_CODE
static  void set_BusSegmentEn(u32 node, u32 segbit)
{
#if CONFIG_PCI_BUS_SEGN_BITS
	u32 dword;
	device_t dev;

#if defined(__PRE_RAM__)
	dev = NODE_PCI(node, 0);
#else
	dev = __f0_dev[node];
#endif

	dword = pci_read_config32(dev, 0x68);
	dword &= ~(7<<28);
	dword |= (segbit<<28); /* bus segment enable */
	pci_write_config32(dev, 0x68, dword);
#endif
}
#endif

#if !defined(__PRE_RAM__)
static u32 get_io_addr_index(u32 nodeid, u32 linkn)
{
	u32 index;

	for(index=0; index<256; index++) {
		if((sysconf.conf_io_addrx[index+4] == 0)){
			sysconf.conf_io_addr[index+4] =  (nodeid & 0x3f)  ;
			sysconf.conf_io_addrx[index+4] = 1 | ((linkn & 0x7)<<4);
			return index;
		 }
	 }

	 return	 0;

}

static u32 get_mmio_addr_index(u32 nodeid, u32 linkn)
{
	u32 index;


	for(index=0; index<64; index++) {
		if((sysconf.conf_mmio_addrx[index+8] == 0)){
			sysconf.conf_mmio_addr[index+8] = (nodeid & 0x3f) ;
			sysconf.conf_mmio_addrx[index+8] = 1 | ((linkn & 0x7)<<4);
			return index;
		}
	}

	return	 0;

}

static void store_conf_io_addr(u32 nodeid, u32 linkn, u32 reg, u32 index,
				u32 io_min, u32 io_max)
{
	u32 val;
#if CONFIG_EXT_CONF_SUPPORT
	if(reg!=0x110) {
#endif
		/* io range allocation */
		index = (reg-0xc0)>>3;
#if CONFIG_EXT_CONF_SUPPORT
	} else {
		index+=4;
	}
#endif

	val = (nodeid & 0x3f); // 6 bits used
	sysconf.conf_io_addr[index] = val | ((io_max<<8) & 0xfffff000); //limit : with nodeid
	val = 3 | ((linkn & 0x7)<<4) ; // 8 bits used
	sysconf.conf_io_addrx[index] = val | ((io_min<<8) & 0xfffff000); // base : with enable bit

	if( sysconf.io_addr_num<(index+1))
		sysconf.io_addr_num = index+1;
}


static void store_conf_mmio_addr(u32 nodeid, u32 linkn, u32 reg, u32 index,
					u32 mmio_min, u32 mmio_max)
{
	u32 val;
#if CONFIG_EXT_CONF_SUPPORT
	if(reg!=0x110) {
#endif
		/* io range allocation */
		index = (reg-0x80)>>3;
#if CONFIG_EXT_CONF_SUPPORT
	} else {
		index+=8;
	}
#endif

	val = (nodeid & 0x3f) ; // 6 bits used
	sysconf.conf_mmio_addr[index] = val | (mmio_max & 0xffffff00); //limit : with nodeid and linkn
	val = 3 | ((linkn & 0x7)<<4) ; // 8 bits used
	sysconf.conf_mmio_addrx[index] = val | (mmio_min & 0xffffff00); // base : with enable bit

	if( sysconf.mmio_addr_num<(index+1))
		sysconf.mmio_addr_num = index+1;
}


static void set_io_addr_reg(device_t dev, u32 nodeid, u32 linkn, u32 reg,
				u32 io_min, u32 io_max)
{

	u32 i;
	u32 tempreg;
#if CONFIG_EXT_CONF_SUPPORT
	if(reg!=0x110) {
#endif
		/* io range allocation */
		tempreg = (nodeid&0xf) | ((nodeid & 0x30)<<(8-4)) | (linkn<<4) |  ((io_max&0xf0)<<(12-4)); //limit
		for(i=0; i<sysconf.nodes; i++)
			pci_write_config32(__f1_dev[i], reg+4, tempreg);

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
		for(i=0; i<sysconf.nodes; i++)
			pci_write_config32(__f1_dev[i], reg, tempreg);
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
	for(j = 0; j< sysconf.nodes; j++) {
		if(j== nodeid) continue;
		set_addr_map_reg_4_6_in_one_node(j,cfg_map_dest, io_min, io_max, 4);
	}
#endif

}
static void set_mmio_addr_reg(u32 nodeid, u32 linkn, u32 reg, u32 index, u32 mmio_min, u32 mmio_max, u32 nodes)
{

	u32 i;
	u32 tempreg;
#if CONFIG_EXT_CONF_SUPPORT
	if(reg!=0x110) {
#endif
		/* io range allocation */
		tempreg = (nodeid&0xf) | (linkn<<4) |	 (mmio_max&0xffffff00); //limit
		for(i=0; i<nodes; i++)
			pci_write_config32(__f1_dev[i], reg+4, tempreg);
		tempreg = 3 | (nodeid & 0x30) | (mmio_min&0xffffff00);
		for(i=0; i<sysconf.nodes; i++)
			pci_write_config32(__f1_dev[i], reg, tempreg);
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


	// all other nodes
	tempreg = ((mmio_min>>3) & 0x1fffff00) | (0<<6) | (nodeid<<0);
	for(j = 0; j< sysconf.nodes; j++) {
		if(j== nodeid) continue;
		dev = __f1_dev[j];
		pci_write_config32(dev, 0x110, index | (2<<28));
		pci_write_config32(dev, 0x114, tempreg);
	}

	tempreg = ((mmio_max>>3) & 0x1fffff00) | enable;
	for(j = 0; j< sysconf.nodes; j++) {
		if(j==nodeid) continue;
		dev = __f1_dev[j];
		pci_write_config32(dev, 0x110, index | (3<<28));
		pci_write_config32(dev, 0x114, tempreg);
	 }
#endif
}

#endif
