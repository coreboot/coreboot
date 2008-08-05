/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
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

#include <console.h>
#include <io.h>
#include <device/device.h>
#include <device/pci.h>
#include <cpu.h>
#include <amd/k8/k8.h>
#include "mcp55.h"
#define pci_read_config32(bus, dev, where) pci_cf8_conf1.read32(NULL, bus, dev, where)
#define pci_write_config32(bus, dev, where, what) pci_cf8_conf1.write32(NULL, bus, dev, where, what)

#warning fix disgusting define of MCP55_NUM it is mainboard dependent
#define MCP55_NUM 1
static int set_ht_link_mcp55(u8 ht_c_num)
{
	unsigned vendorid = 0x10de;
	unsigned val = 0x01610109;
	/* Nvidia mcp55 hardcode, hw can not set it automatically */
	return set_ht_link_buffer_counts_chain(ht_c_num, vendorid, val);
}

static void setup_ss_table(unsigned index, unsigned where, unsigned control, const unsigned int *register_values, int max)
{
	int i;

	unsigned val;

	val = inl(control);
	val &= 0xfffffffe;
	outl(val, control);

	outl(0, index); //index
	for(i = 0; i < max; i++) {
		unsigned long reg;
		reg = register_values[i];
		outl(reg, where);
	}

	val = inl(control);
	val |= 1;
	outl(val, control);

}

/* SIZE 0x100 */
#define ANACTRL_IO_BASE	0x2800
#define ANACTRL_REG_POS	0x68

/* SIZE 0x100 */
#define SYSCTRL_IO_BASE	0x2400
#define SYSCTRL_REG_POS	0x64

/* SIZE 0x100 */
#define ACPICTRL_IO_BASE	0x2000
#define ACPICTRL_REG_POS	0x60

/*
	16 1 1 1 1 8 :0
	16 0 4 0 0 8 :1
	16 0 4 2 2 4 :2
	 4 4 4 4 4 8 :3
	 8 8 4 0 0 8 :4
	 8 0 4 4 4 8 :5
*/

#ifndef MCP55_PCI_E_X_0
	#define MCP55_PCI_E_X_0	4
#endif
#ifndef MCP55_PCI_E_X_1
	#define MCP55_PCI_E_X_1	4
#endif
#ifndef MCP55_PCI_E_X_2
	#define MCP55_PCI_E_X_2	4
#endif
#ifndef MCP55_PCI_E_X_3
	#define MCP55_PCI_E_X_3	4
#endif

#ifndef MCP55_USE_NIC
	#define MCP55_USE_NIC	0
#endif

#ifndef MCP55_USE_AZA
	#define MCP55_USE_AZA	0
#endif

#define MCP55_CHIP_REV	3

static void mcp55_early_set_port(unsigned mcp55_num, unsigned *busn, unsigned *devn, unsigned *io_base)
{
	static const struct rmap ctrl_devport_conf[] = {
	PCM(0, 1, 1, ANACTRL_REG_POS, ~0x0000ff00, ANACTRL_IO_BASE),
	PCM(0, 1, 1, SYSCTRL_REG_POS, ~0x0000ff00, SYSCTRL_IO_BASE),
	PCM(0, 1, 1, ACPICTRL_REG_POS, ~0x0000ff00, ACPICTRL_IO_BASE),
	};

	int j;
	for(j = 0; j < mcp55_num; j++ ) {
		setup_resource_map_x_offset(ctrl_devport_conf,
			sizeof(ctrl_devport_conf)/sizeof(ctrl_devport_conf[0]),
			PCI_BDF(busn[j], devn[j], 0) , io_base[j], 0);
	}
}
static void mcp55_early_clear_port(unsigned mcp55_num, unsigned *busn, unsigned *devn, unsigned *io_base)
{

	static const struct rmap ctrl_devport_conf_clear[] = {
	PCM(0, 1, 1, ANACTRL_REG_POS, ~0x0000ff00, 0),
	PCM(0, 1, 1, SYSCTRL_REG_POS, ~0x0000ff00, 0),
	PCM(0, 1, 1, ACPICTRL_REG_POS, ~0x0000ff00, 0),
	};

	int j;
	for(j = 0; j < mcp55_num; j++ ) {
		setup_resource_map_x_offset(ctrl_devport_conf_clear,
			sizeof(ctrl_devport_conf_clear)/sizeof(ctrl_devport_conf_clear[0]),
			PCI_BDF(busn[j], devn[j], 0) , io_base[j], 0);
	}


}
static void delayx(u8 value) {
#if 1
	int i;
	for(i=0;i<0x8000;i++) {
		outb(value, 0x80);
	}
#endif
}

static void mcp55_early_pcie_setup(unsigned busnx, unsigned devnx, unsigned anactrl_io_base, unsigned pci_e_x)
{
	u32 tgio_ctrl;
	u32 pll_ctrl;
	u32 dword;
	int i;
	u32 dev;
	//	dev = PCI_BDF(busnx, devnx+1, 1);
	dword = pci_read_config32(busnx, (devnx+1)<<3 | 1, 0xe4);
	dword |= 0x3f0; // disable it at first
	pci_write_config32(busnx,  (devnx+1)<<3 | 1, 0xe4, dword);

	for(i=0; i<3; i++) {
		tgio_ctrl = inl(anactrl_io_base + 0xcc);
		tgio_ctrl &= ~(3<<9);
		tgio_ctrl |= (i<<9);
		outl(tgio_ctrl, anactrl_io_base + 0xcc);
		pll_ctrl = inl(anactrl_io_base + 0x30);
		pll_ctrl |= (1<<31);
		outl(pll_ctrl, anactrl_io_base + 0x30);
		do {
			pll_ctrl = inl(anactrl_io_base + 0x30);
		} while (!(pll_ctrl & 1));
	}
	tgio_ctrl = inl(anactrl_io_base + 0xcc);
	tgio_ctrl &= ~((7<<4)|(1<<8));
	tgio_ctrl |= (pci_e_x<<4)|(1<<8);
	outl(tgio_ctrl, anactrl_io_base + 0xcc);

//	wait 100us
	delayx(1);

	dword = pci_read_config32(busnx,  (devnx+1)<<3 | 1, 0xe4);
	dword &= ~(0x3f0); // enable
	pci_write_config32(busnx,  (devnx+1)<<3 | 1, 0xe4, dword);

//	need to wait 100ms
	delayx(1000);
}

static void mcp55_early_setup(unsigned mcp55_num, unsigned *busn, unsigned *devn, unsigned *io_base, unsigned *pci_e_x)
{

    static const struct rmap ctrl_conf_1[] = {
	IO32(ACPICTRL_IO_BASE + 0x10, 0x0007ffff, 0xff78000),
	IO32(ACPICTRL_IO_BASE + 0xa4, 0xffedffff, 0x0012000),
	IO32(ACPICTRL_IO_BASE + 0xac, 0xfffffdff, 0x0000200),
	IO32(ACPICTRL_IO_BASE + 0xb4, 0xfffffffd, 0x0000002),

	IO32(ANACTRL_IO_BASE + 0x24, 0xc0f0f08f, 0x26020230),
	IO32(ANACTRL_IO_BASE + 0x34, 0x00000000, 0x22222222),
	IO32(ANACTRL_IO_BASE + 0x08, 0x7FFFFFFF, 0x00000000),
	IO32(ANACTRL_IO_BASE + 0x2C, 0x7FFFFFFF, 0x80000000),
	IO32(ANACTRL_IO_BASE + 0xCC, 0xFFFFF9FF, 0x00000000),
	IO32(ANACTRL_IO_BASE + 0x30, 0x8FFFFFFF, 0x40000000),
	IO32(ANACTRL_IO_BASE + 0xCC, 0xFFFFF9FF, 0x00000200),
	IO32(ANACTRL_IO_BASE + 0x30, 0x8FFFFFFF, 0x40000000),
	IO32(ANACTRL_IO_BASE + 0xCC, 0xFFFFF9FF, 0x00000400),
	IO32(ANACTRL_IO_BASE + 0x30, 0x8FFFFFFF, 0x40000000),
	IO32(ANACTRL_IO_BASE + 0x74, 0xFFFF0FF5, 0x0000F000),
	IO32(ANACTRL_IO_BASE + 0x78, 0xFF00FF00, 0x00100010),
	IO32(ANACTRL_IO_BASE + 0x7C, 0xFF0FF0FF, 0x00500500),
	IO32(ANACTRL_IO_BASE + 0x80, 0xFFFFFFE7, 0x00000000),
	IO32(ANACTRL_IO_BASE + 0x60, 0xFFCFFFFF, 0x00300000),
	IO32(ANACTRL_IO_BASE + 0x90, 0xFFFF00FF, 0x0000FF00),
	IO32(ANACTRL_IO_BASE + 0x9C, 0xFF00FFFF, 0x00070000),

	PCM(0, 0, 0, 0x40, 0x00000000, 0xCB8410DE), 
	PCM(0, 0, 0, 0x48, 0xFFFFDCED, 0x00002002), 
	PCM(0, 0, 0, 0x78, 0xFFFFFF8E, 0x00000011), 
	PCM(0, 0, 0, 0x80, 0xFFFF0000, 0x00009923), 
	PCM(0, 0, 0, 0x88, 0xFFFFFFFE, 0x00000000), 
	PCM(0, 0, 0, 0x8C, 0xFFFF0000, 0x0000007F), 
	PCM(0, 0, 0, 0xDC, 0xFFFEFFFF, 0x00010000), 

	PCM(0, 1, 0, 0x40, 0x00000000, 0xCB8410DE), 
	PCM(0, 1, 0, 0x74, 0xFFFFFF7B, 0x00000084), 
	PCM(0, 1, 0, 0xF8, 0xFFFFFFCF, 0x00000010), 

	PCM(0, 1, 1, 0xC4, 0xFFFFFFFE, 0x00000001), 
	PCM(0, 1, 1, 0xF0, 0x7FFFFFFD, 0x00000002), 
	PCM(0, 1, 1, 0xF8, 0xFFFFFFCF, 0x00000010), 

	PCM(0, 8, 0, 0x40, 0x00000000, 0xCB8410DE), 
	PCM(0, 8, 0, 0x68, 0xFFFFFF00, 0x000000FF), 
	PCM(0, 8, 0, 0xF8, 0xFFFFFFBF, 0x00000040), //Enable bridge mode

	PCM(0, 9, 0, 0x40, 0x00000000, 0xCB8410DE), 
	PCM(0, 9, 0, 0x68, 0xFFFFFF00, 0x000000FF), 
	PCM(0, 9, 0, 0xF8, 0xFFFFFFBF, 0x00000040), //Enable bridge mode
    };

    static const struct rmap ctrl_conf_1_1[] = {
	PCM(0, 5, 0, 0x40, 0x00000000, 0xCB8410DE), 
	PCM(0, 5, 0, 0x50, 0xFFFFFFFC, 0x00000003), 
	PCM(0, 5, 0, 0x64, 0xFFFFFFFE, 0x00000001), 
	PCM(0, 5, 0, 0x70, 0xFFF0FFFF, 0x00040000), 
	PCM(0, 5, 0, 0xAC, 0xFFFFF0FF, 0x00000100), 
	PCM(0, 5, 0, 0x7C, 0xFFFFFFEF, 0x00000000), 
	PCM(0, 5, 0, 0xC8, 0xFF00FF00, 0x000A000A), 
	PCM(0, 5, 0, 0xD0, 0xF0FFFFFF, 0x03000000), 
	PCM(0, 5, 0, 0xE0, 0xF0FFFFFF, 0x03000000), 
    };

    static const struct rmap ctrl_conf_mcp55_only[] = {
	PCM(0, 1, 1, 0x40, 0x00000000, 0xCB8410DE), 
	PCM(0, 1, 1, 0xE0, 0xFFFFFEFF, 0x00000000), 
	PCM(0, 1, 1, 0xE4, 0xFFFFFFFB, 0x00000000), 
	PCM(0, 1, 1, 0xE8, 0xFFA9C8FF, 0x00003000), 

	PCM(0, 4, 0, 0x40, 0x00000000, 0xCB8410DE), 
	PCM(0, 4, 0, 0xF8, 0xFFFFFFCF, 0x00000010), 

	PCM(0, 2, 0, 0x40, 0x00000000, 0xCB8410DE), 

	PCM(0, 2, 1, 0x40, 0x00000000, 0xCB8410DE), 
	PCM(0, 2, 1, 0x64, 0xF87FFFFF, 0x05000000), 
	PCM(0, 2, 1, 0x78, 0xFFC07FFF, 0x00360000), 
	PCM(0, 2, 1, 0x68, 0xFE00D03F, 0x013F2C00), 
	PCM(0, 2, 1, 0x70, 0xFFF7FFFF, 0x00080000), 
	PCM(0, 2, 1, 0x7C, 0xFFFFF00F, 0x00000570), 
	PCM(0, 2, 1, 0xF8, 0xFFFFFFCF, 0x00000010), 

	PCM(0, 6, 0, 0x04, 0xFFFFFEFB, 0x00000104), 
	PCM(0, 6, 0, 0x3C, 0xF5FFFFFF, 0x0A000000), 
	PCM(0, 6, 0, 0x40, 0x00C8FFFF, 0x07330000), 
	PCM(0, 6, 0, 0x48, 0xFFFFFFF8, 0x00000005), 
	PCM(0, 6, 0, 0x4C, 0xFE02FFFF, 0x004C0000), 
	PCM(0, 6, 0, 0x74, 0xFFFFFFC0, 0x00000000), 
	PCM(0, 6, 0, 0xC0, 0x00000000, 0xCB8410DE), 
	PCM(0, 6, 0, 0xC4, 0xFFFFFFF8, 0x00000007), 

	PCM(0, 1, 0, 0x78, 0xC0FFFFFF, 0x19000000), 
#if MCP55_USE_AZA == 1
	PCM(0, 6, 1, 0x40, 0x00000000, 0xCB8410DE), 

//	PCM(0, 1, 1, 0xE4), ~(1<<14, 1<<14)),
#endif
// play a while with GPIO in MCP55
#ifdef MCP55_MB_SETUP
	MCP55_MB_SETUP
#endif

#if MCP55_USE_AZA == 1
	IO8(SYSCTRL_IO_BASE + 0xc0+ 21, ~(3<<2), (2<<2)),
	IO8(SYSCTRL_IO_BASE + 0xc0+ 22, ~(3<<2), (2<<2)),
	IO8(SYSCTRL_IO_BASE + 0xc0+ 46, ~(3<<2), (2<<2)),
#endif


    };

    static const struct rmap ctrl_conf_master_only[] = {

	IO32(ACPICTRL_IO_BASE + 0x80, 0xEFFFFFF, 0x01000000),

	//Master MCP55 ????YHLU
	IO8( SYSCTRL_IO_BASE + 0xc0+ 0, ~(3<<2), (0<<2)),

    };

    static const struct rmap ctrl_conf_2[] = {
	/* I didn't put pcie related stuff here */

	PCM(0, 0, 0, 0x74, 0xFFFFF00F, 0x000009D0),
	PCM(0, 1, 0, 0x74, 0xFFFF7FFF, 0x00008000),

	IO32(SYSCTRL_IO_BASE + 0x48, 0xFFFEFFFF, 0x00010000),

	IO32(ANACTRL_IO_BASE + 0x60, 0xFFFFFF00, 0x00000012),


#if MCP55_USE_NIC == 1
	PCM(0, 1, 1, 0xe4, ~((1<<22)|(1<<20)), (1<<22)|(1<<20)),

	IO8(SYSCTRL_IO_BASE + 0xc0+ 4, ~(0xff),  ((0<<4)|(1<<2)|(0<<0))),
	IO8(SYSCTRL_IO_BASE + 0xc0+ 4, ~(0xff),  ((0<<4)|(1<<2)|(1<<0))),
#endif

    };


	int j, i;

	for(j=0; j<mcp55_num; j++) {
		mcp55_early_pcie_setup(busn[j], devn[j], io_base[j] + ANACTRL_IO_BASE, pci_e_x[j]);

		setup_resource_map_x_offset(ctrl_conf_1, sizeof(ctrl_conf_1)/sizeof(ctrl_conf_1[0]),
				PCI_BDF(busn[j], devn[j], 0), io_base[j], 0);
		for(i=0; i<3; i++) { // three SATA
			setup_resource_map_x_offset(ctrl_conf_1_1, sizeof(ctrl_conf_1_1)/sizeof(ctrl_conf_1_1[0]),
				PCI_BDF(busn[j], devn[j], i), io_base[j], 0);
		}
		if(busn[j] == 0) {
			setup_resource_map_x_offset(ctrl_conf_mcp55_only, sizeof(ctrl_conf_mcp55_only)/sizeof(ctrl_conf_mcp55_only[0]),
				PCI_BDF(busn[j], devn[j], 0), io_base[j], 0);
		}

		if( (busn[j] == 0) && (mcp55_num>1) ) {
			setup_resource_map_x_offset(ctrl_conf_master_only, sizeof(ctrl_conf_master_only)/sizeof(ctrl_conf_master_only[0]),
				PCI_BDF(busn[j], devn[j], 0), io_base[j], 0);
		}

		setup_resource_map_x_offset(ctrl_conf_2, sizeof(ctrl_conf_2)/sizeof(ctrl_conf_2[0]),
				PCI_BDF(busn[j], devn[j], 0), io_base[j], 0);

	}

#if 0
	for(j=0; j< mcp55_num; j++) {
		// PCI-E (XSPLL) SS table 0x40, x044, 0x48
		// SATA  (SPPLL) SS table 0xb0, 0xb4, 0xb8
		// CPU   (PPLL)  SS table 0xc0, 0xc4, 0xc8
		setup_ss_table(io_base[j] + ANACTRL_IO_BASE+0x40, io_base[j] + ANACTRL_IO_BASE+0x44,
			io_base[j] + ANACTRL_IO_BASE+0x48, pcie_ss_tbl, 64);
		setup_ss_table(io_base[j] + ANACTRL_IO_BASE+0xb0, io_base[j] + ANACTRL_IO_BASE+0xb4,
			io_base[j] + ANACTRL_IO_BASE+0xb8, sata_ss_tbl, 64);
		setup_ss_table(io_base[j] + ANACTRL_IO_BASE+0xc0, io_base[j] + ANACTRL_IO_BASE+0xc4,
			io_base[j] + ANACTRL_IO_BASE+0xc8, cpu_ss_tbl, 64);
	}
#endif

}

#ifndef HT_CHAIN_NUM_MAX

#define HT_CHAIN_NUM_MAX	4
#define HT_CHAIN_BUSN_D	0x40
#define HT_CHAIN_IOBASE_D	0x4000

#endif

static int mcp55_early_setup_x(void)
{
	/*find out how many mcp55 we have */
	unsigned busn[HT_CHAIN_NUM_MAX];
	unsigned devn[HT_CHAIN_NUM_MAX];
	unsigned io_base[HT_CHAIN_NUM_MAX];
	/*
		FIXME: May have problem if there is different MCP55 HTX card with different PCI_E lane allocation
		Need to use same trick about pci1234 to verify node/link connection
	*/
	unsigned pci_e_x[HT_CHAIN_NUM_MAX] = {MCP55_PCI_E_X_0, MCP55_PCI_E_X_1, MCP55_PCI_E_X_2, MCP55_PCI_E_X_3 };
	int mcp55_num = 0;
	unsigned busnx;
	unsigned devnx;
	int ht_c_index,j;

	/* FIXME: multi pci segment handling */

	/* Any system that only have IO55 without MCP55? */
	for(ht_c_index = 0; ht_c_index<HT_CHAIN_NUM_MAX; ht_c_index++) {
		busnx = ht_c_index * HT_CHAIN_BUSN_D;
		for(devnx=0;devnx<0x20;devnx++) {
			u32 id;
			//			dev = PCI_BDF(busnx, devnx, 0);
			id = pci_read_config32(busnx, devnx<<3, PCI_VENDOR_ID);
			if(id == 0x036910de) {
				busn[mcp55_num] = busnx;
				devn[mcp55_num] = devnx;
				io_base[mcp55_num] = ht_c_index * HT_CHAIN_IOBASE_D; // we may have ht chain other than MCP55
				mcp55_num++;
				if(mcp55_num == MCP55_NUM) goto out;
				break; // only one MCP55 on one chain
			}
		}
	}

out:
	printk(BIOS_DEBUG, "mcp55_num: %d\n", mcp55_num);

	mcp55_early_set_port(mcp55_num, busn, devn, io_base);
	mcp55_early_setup(mcp55_num, busn, devn, io_base, pci_e_x);

	mcp55_early_clear_port(mcp55_num, busn, devn, io_base);

//	set_ht_link_mcp55(HT_CHAIN_NUM_MAX);

	return 0;

}



