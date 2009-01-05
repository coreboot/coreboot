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
#include <mainboard.h>
#include <console.h>
#include <lib.h>
#include <io.h>
#include <device/device.h>
#include <device/pci.h>
#include <cpu.h>
#include <amd/k8/k8.h>
#include "stage1.h"

#ifndef CK804_NUM
#error CK804_NUM should be defined in mainboard.h (in the mainboard directory)
#endif

#define CK804_BUSN 0
#define CK804B_BUSN 1

#if 0
int set_ht_link_ck804(u8 ht_c_num)
{
	int set_ht_link_buffer_counts_chain(u8 ht_c_num, unsigned vendorid,  unsigned val);
	unsigned vendorid = 0x10de;
	unsigned val = 0x01610169;
	/* Nvidia ck804 hardcode, hw can not set it automatically. */
	return set_ht_link_buffer_counts_chain(ht_c_num, vendorid, val);
}
#endif

void setup_ss_table(unsigned index, unsigned where, unsigned control,
		    const u32 *register_values, int max)
{
	int i;
	unsigned val;

	val = inl(control);
	val &= 0xfffffffe;
	outl(val, control);

	outl(0, index);

	for(i = 0; i < max; i++) {
		unsigned long reg;
		reg = register_values[i];
		outl(reg, where);
	}

	val = inl(control);
	val |= 1;
	outl(val, control);
}

#define ANACTRL_IO_BASE	0x3000
#define ANACTRL_REG_POS	0x68

#define SYSCTRL_IO_BASE	0x2000
#define SYSCTRL_REG_POS	0x64

/*
 *	16 1 1 2 :0
 *	 8 8 2 2 :1
 *	 8 8 4   :2
 *	 8 4 4 4 :3
 *	16 4     :4
*/

#ifndef CK804_PCI_E_X
#define CK804_PCI_E_X 4
#endif

#ifndef CK804_USE_NIC
#define CK804_USE_NIC 0
#endif

#ifndef CK804_USE_ACI
#define CK804_USE_ACI 0
#endif
#ifndef CK804_PCI_E_X
#define CK804_PCI_E_X 4
#endif

/*
 * We will use the offset in setup_resource_map_x_offset and
 * setup_resource_map_offset.
 */
#define CK804B_ANACTRL_IO_BASE 0x3000
#define CK804B_SYSCTRL_IO_BASE 0x2000

#ifdef CK804B_BUSN
#undef CK804B_BUSN
#endif
#define CK804B_BUSN 0x0

#ifndef CK804B_PCI_E_X
#define CK804B_PCI_E_X 4
#endif

#ifndef CK804_USE_NIC
#define CK804_USE_NIC 0
#endif

#ifndef CK804_USE_ACI
#define CK804_USE_ACI 0
#endif

#define CK804_CHIP_REV 3

#if HT_CHAIN_END_UNITID_BASE < HT_CHAIN_UNITID_BASE
#define CK804_DEVN_BASE HT_CHAIN_END_UNITID_BASE
#else
#define CK804_DEVN_BASE HT_CHAIN_UNITID_BASE
#endif

#if SB_HT_CHAIN_UNITID_OFFSET_ONLY == 1
#define CK804B_DEVN_BASE 1
#else
#define CK804B_DEVN_BASE CK804_DEVN_BASE
#endif

void ck804_early_set_port(unsigned ck804_num, unsigned *busn,
				 unsigned *io_base)
{
	static const struct rmap ctrl_devport_conf[] = {
	PCM(0, (CK804_DEVN_BASE + 0x1), 0, ANACTRL_REG_POS, ~0x0000ff00, ANACTRL_IO_BASE),
	PCM(0, (CK804_DEVN_BASE + 0x1), 0, SYSCTRL_REG_POS, ~0x0000ff00, SYSCTRL_IO_BASE),
	};

	int j;
	for(j = 0; j < ck804_num; j++ ) {
		setup_resource_map_x_offset(ctrl_devport_conf,
			ARRAY_SIZE(ctrl_devport_conf),
			PCI_BDF(busn[j], 0, 0), io_base[j]);
	}
}

void ck804_early_clear_port(unsigned ck804_num, unsigned *busn,
				   unsigned *io_base)
{
	static const struct rmap ctrl_devport_conf_clear[] = {
	PCM(0, (CK804_DEVN_BASE + 0x1), 0, ANACTRL_REG_POS, ~0x0000ff00, 0),
	PCM(0, (CK804_DEVN_BASE + 0x1), 0, SYSCTRL_REG_POS, ~0x0000ff00, 0),
	};

	int j;
	for(j = 0; j < ck804_num; j++ ) {
		setup_resource_map_x_offset(ctrl_devport_conf_clear,
			ARRAY_SIZE(ctrl_devport_conf_clear),
			PCI_BDF(busn[j], 0, 0) , io_base[j]);
	}
}

/**
 * Do early setup
 * @param ck804_num the number of ck804s on the board
 * @param busn the number of the bus the ck804 is on
 * @param io_base
 */

static void ck804_early_setup(unsigned ck804_num, unsigned *busn,
			      unsigned *io_base)
{
	static const struct rmap ctrl_conf_master[] = {
	PCM(0, CK804_DEVN_BASE + 1, 2, 0x8c, 0xffff0000, 0x00009880),
	PCM(0, CK804_DEVN_BASE + 1, 2, 0x90, 0xffff000f, 0x000074a0),
	PCM(0, CK804_DEVN_BASE + 1, 2, 0xa0, 0xfffff0ff, 0x00000a00),
	PCM(0, CK804_DEVN_BASE + 1, 2, 0xac, 0xffffff00, 0x00000000),

	PCM(0, CK804_DEVN_BASE, 0, 0x48, 0xfffffffd, 0x00000002),
	PCM(0, CK804_DEVN_BASE, 0, 0x74, 0xfffff00f, 0x000009d0),
	PCM(0, CK804_DEVN_BASE, 0, 0x8c, 0xffff0000, 0x0000007f),
	PCM(0, CK804_DEVN_BASE, 0, 0xcc, 0xfffffff8, 0x00000003),
	PCM(0, CK804_DEVN_BASE, 0, 0xd0, 0xff000000, 0x00000000),
	PCM(0, CK804_DEVN_BASE, 0, 0xd4, 0xff000000, 0x00000000),
	PCM(0, CK804_DEVN_BASE, 0, 0xd8, 0xff000000, 0x00000000),
	PCM(0, CK804_DEVN_BASE, 0, 0xdc, 0x7f000000, 0x00000000),

	PCM(0, CK804_DEVN_BASE + 1, 0, 0xf0, 0xfffffffd, 0x00000002),
	PCM(0, CK804_DEVN_BASE + 1, 0, 0xf8, 0xffffffcf, 0x00000010),

	PCM(0, CK804_DEVN_BASE + 9, 0, 0x40, 0xfff8ffff, 0x00030000),
	PCM(0, CK804_DEVN_BASE + 9, 0, 0x4c, 0xfe00ffff, 0x00440000),
	PCM(0, CK804_DEVN_BASE + 9, 0, 0x74, 0xffffffc0, 0x00000000),

#ifdef CK804_MB_SETUP
		CK804_MB_SETUP
#endif
#if CK804_NUM > 1
	PCM(0, CK804_DEVN_BASE + 1, 0, 0x78, 0xc0ffffff, 0x19000000),
	PCM(0, CK804_DEVN_BASE + 1, 0, 0xe0, 0xfffffeff, 0x00000100),
#endif

#if CK804_NUM == 1
	PCM(0, CK804_DEVN_BASE + 1, 0, 0x78, 0xc0ffffff, 0x19000000),
	PCM(0, CK804_DEVN_BASE + 1, 0, 0xe0, 0xfffffeff, 0x00000100),
#endif

	IO32(ANACTRL_IO_BASE + 0x20, 0xe00fffff, 0x11000000),
	IO32(ANACTRL_IO_BASE + 0x24, 0xc3f0ffff, 0x24040000),
	IO32(ANACTRL_IO_BASE + 0x80, 0x8c3f04df, 0x51407120),
	IO32(ANACTRL_IO_BASE + 0x84, 0xffffff8f, 0x00000010),
	IO32(ANACTRL_IO_BASE + 0x94, 0xff00ffff, 0x00c00000),
	IO32(ANACTRL_IO_BASE + 0xcc, 0xf7ffffff, 0x00000000),

	IO32(ANACTRL_IO_BASE + 0x74, ~(0xffff), 0x0f008),
	IO32(ANACTRL_IO_BASE + 0x78, ~((0xff) | (0xff << 16)), (0x41 << 16) | (0x32)),
	IO32(ANACTRL_IO_BASE + 0x7c, ~(0xff << 16), (0xa0 << 16)),

	IO32(ANACTRL_IO_BASE + 0x24, 0xfcffff0f, 0x020000b0),

	PCM(0, CK804_DEVN_BASE + 8, 0, 0x50, ~0x1f000013, 0x15000013),
	PCM(0, CK804_DEVN_BASE + 8, 0, 0x64, ~0x00000001, 0x00000001),
	PCM(0, CK804_DEVN_BASE + 8, 0, 0x68, ~0x02000000, 0x02000000),
	PCM(0, CK804_DEVN_BASE + 8, 0, 0x70, ~0x000f0000, 0x00040000),
	PCM(0, CK804_DEVN_BASE + 8, 0, 0xa0, ~0x000001ff, 0x00000150),
	PCM(0, CK804_DEVN_BASE + 8, 0, 0xac, ~0xffff8f00, 0x02aa8b00),
	PCM(0, CK804_DEVN_BASE + 8, 0, 0x7c, ~0x00000010, 0x00000000),
	PCM(0, CK804_DEVN_BASE + 8, 0, 0xc8, ~0x0fff0fff, 0x000a000a),
	PCM(0, CK804_DEVN_BASE + 8, 0, 0xd0, ~0xf0000000, 0x00000000),
	PCM(0, CK804_DEVN_BASE + 8, 0, 0xe0, ~0xf0000000, 0x00000000),

	IO32(ANACTRL_IO_BASE + 0x04, ~((0x3ff << 0) | (0x3ff << 10)), (0x21 << 0) | (0x22 << 10)),

// PANTA	IO32(ANACTRL_IO_BASE + 0x08, ~(0xfffff), (0x1c << 10) | 0x1b),

	IO32(ANACTRL_IO_BASE + 0x80, ~(1 << 3), 0x00000000),

	IO32(ANACTRL_IO_BASE + 0xcc, ~((7 << 4) | (1 << 8)), (CK804_PCI_E_X << 4) | (1 << 8)),

//SYSCTRL
	IO8(SYSCTRL_IO_BASE + 0xc0 + 8, (u8)~(0xff), (u8)((0 << 4) | (0 << 2) | (0 << 0))),
	IO8(SYSCTRL_IO_BASE + 0xc0 + 9, (u8)~(0xff), (u8)((0 << 4) | (1 << 2) | (1 << 0))),

#if CK804_USE_NIC == 1
	PCM(0, CK804_DEVN_BASE + 0xa, 0, 0xf8, 0xffffffbf, 0x00000040),

	IO8(SYSCTRL_IO_BASE + 0xc0 + 19, (u8)~(0xff), (u8)((0 << 4) | (1 << 2) | (0 << 0)),
	IO8(SYSCTRL_IO_BASE + 0xc0 + 3, (u8)~(0xff), (u8)((0 << 4) | (1 << 2) | (0 << 0)),
	IO8(SYSCTRL_IO_BASE + 0xc0 + 3, (u8)~(0xff), (u8)((0 << 4) | (1 << 2) | (1 << 0)),
	PCM(0, CK804_DEVN_BASE + 1, 0, 0xe4, ~(1 << 23), (1 << 23),
#endif

#if CK804_USE_ACI == 1
	IO8(SYSCTRL_IO_BASE + 0xc0 + 0x0d, (u8)~(0xff), (u8)((0 << 4) | (2 << 2) | (0 << 0)),
	IO8(SYSCTRL_IO_BASE + 0xc0 + 0x1a, (u8)~(0xff), (u8)((0 << 4) | (2 << 2) | (0 << 0)),
#endif

#if CK804_NUM > 1
	IO8(SYSCTRL_IO_BASE + 0xc0 + 0, (u8)~(3 << 2), (u8)(0 << 2)),
#endif
	};

	static const struct rmap ctrl_conf_slave[] = {
	PCM(CK804B_BUSN, CK804B_DEVN_BASE + 1, 2, 0x8c, 0xffff0000, 0x00009880),
	PCM(CK804B_BUSN, CK804B_DEVN_BASE + 1, 2, 0x90, 0xffff000f, 0x000074a0),
	PCM(CK804B_BUSN, CK804B_DEVN_BASE + 1, 2, 0xa0, 0xfffff0ff, 0x00000a00),

	PCM(CK804B_BUSN, CK804B_DEVN_BASE, 0, 0x48, 0xfffffffd, 0x00000002),
	PCM(CK804B_BUSN, CK804B_DEVN_BASE, 0, 0x74, 0xfffff00f, 0x000009d0),
	PCM(CK804B_BUSN, CK804B_DEVN_BASE, 0, 0x8c, 0xffff0000, 0x0000007f),
	PCM(CK804B_BUSN, CK804B_DEVN_BASE, 0, 0xcc, 0xfffffff8, 0x00000003),
	PCM(CK804B_BUSN, CK804B_DEVN_BASE, 0, 0xd0, 0xff000000, 0x00000000),
	PCM(CK804B_BUSN, CK804B_DEVN_BASE, 0, 0xd4, 0xff000000, 0x00000000),
	PCM(CK804B_BUSN, CK804B_DEVN_BASE, 0, 0xd8, 0xff000000, 0x00000000),
	PCM(CK804B_BUSN, CK804B_DEVN_BASE, 0, 0xdc, 0x7f000000, 0x00000000),

	PCM(CK804B_BUSN, CK804B_DEVN_BASE + 1, 0, 0xf0, 0xfffffffd, 0x00000002),
	PCM(CK804B_BUSN, CK804B_DEVN_BASE + 1, 0, 0xf8, 0xffffffcf, 0x00000010),

	PCM(CK804B_BUSN, CK804B_DEVN_BASE + 9, 0, 0x40, 0xfff8ffff, 0x00030000),
	PCM(CK804B_BUSN, CK804B_DEVN_BASE + 9, 0, 0x4c, 0xfe00ffff, 0x00440000),
	PCM(CK804B_BUSN, CK804B_DEVN_BASE + 9, 0, 0x74, 0xffffffc0, 0x00000000),

	PCM(CK804B_BUSN, CK804B_DEVN_BASE + 1, 0, 0x78, 0xc0ffffff, 0x20000000),
	PCM(CK804B_BUSN, CK804B_DEVN_BASE + 1, 0, 0xe0, 0xfffffeff, 0x00000000),
	PCM(CK804B_BUSN, CK804B_DEVN_BASE + 1, 0, 0xe8, 0xffffff00, 0x000000ff),

	IO32(CK804B_ANACTRL_IO_BASE + 0x20, 0xe00fffff, 0x11000000),
	IO32(CK804B_ANACTRL_IO_BASE + 0x24, 0xc3f0ffff, 0x24040000),
	IO32(CK804B_ANACTRL_IO_BASE + 0x80, 0x8c3f04df, 0x51407120),
	IO32(CK804B_ANACTRL_IO_BASE + 0x84, 0xffffff8f, 0x00000010),
	IO32(CK804B_ANACTRL_IO_BASE + 0x94, 0xff00ffff, 0x00c00000),
	IO32(CK804B_ANACTRL_IO_BASE + 0xcc, 0xf7ffffff, 0x00000000),

	IO32(CK804B_ANACTRL_IO_BASE + 0x24, 0xfcffff0f, 0x020000b0),

	PCM(CK804B_BUSN, CK804B_DEVN_BASE + 8, 0, 0x50, ~0x1f000013, 0x15000013),
	PCM(CK804B_BUSN, CK804B_DEVN_BASE + 8, 0, 0x64, ~0x00000001, 0x00000001),
	PCM(CK804B_BUSN, CK804B_DEVN_BASE + 8, 0, 0x68, ~0x02000000, 0x02000000),
	PCM(CK804B_BUSN, CK804B_DEVN_BASE + 8, 0, 0x70, ~0x000f0000, 0x00040000),
	PCM(CK804B_BUSN, CK804B_DEVN_BASE + 8, 0, 0xa0, ~0x000001ff, 0x00000150),
	PCM(CK804B_BUSN, CK804B_DEVN_BASE + 8, 0, 0xac, ~0xffff8f00, 0x02aa8b00),
	PCM(CK804B_BUSN, CK804B_DEVN_BASE + 8, 0, 0x7c, ~0x00000010, 0x00000000),
	PCM(CK804B_BUSN, CK804B_DEVN_BASE + 8, 0, 0xc8, ~0x0fff0fff, 0x000a000a),
	PCM(CK804B_BUSN, CK804B_DEVN_BASE + 8, 0, 0xd0, ~0xf0000000, 0x00000000),
	PCM(CK804B_BUSN, CK804B_DEVN_BASE + 8, 0, 0xe0, ~0xf0000000, 0x00000000),

	IO32(CK804B_ANACTRL_IO_BASE + 0x04, ~((0x3ff << 0) | (0x3ff << 10)), (0x21 << 0) | (0x22 << 10)),

//PANTA	IO32(CK804B_ANACTRL_IO_BASE + 0x08, ~(0xfffff), (0x1c << 10) | 0x1b),

	IO32(CK804B_ANACTRL_IO_BASE + 0x80, ~(1 << 3), 0x00000000),

	IO32(CK804B_ANACTRL_IO_BASE + 0xcc, ~((7 << 4) | (1 << 8)), (CK804B_PCI_E_X << 4) | (1 << 8)),

#if CK804_USE_NIC == 1
	PCM(0, CK804B_DEVN_BASE + 0xa, 0, 0xf8), 0xffffffbf, 0x00000040),
	IO8(SYSCTRL_IO_BASE + 0xc0 + 19, ~(0xff), ((0 << 4) | (1 << 2) | (0 << 0))),
	IO8(SYSCTRL_IO_BASE + 0xc0 + 3, ~(0xff), ((0 << 4) | (1 << 2) | (0 << 0))),
	IO8(SYSCTRL_IO_BASE + 0xc0 + 3, ~(0xff), ((0 << 4) | (1 << 2) | (1 << 0))),
	PCM(CK804B_BUSN, CK804B_DEVN_BASE + 1, 0, 0xe4), ~(1 << 23), (1 << 23)),
#endif
	};

	int j;
	for (j = 0; j < ck804_num; j++) {
		if (busn[j] == 0) {
			setup_resource_map_x_offset(ctrl_conf_master,
				ARRAY_SIZE(ctrl_conf_master),
				PCI_BDF(busn[0], 0, 0), io_base[0]);
			continue;
		}

		setup_resource_map_x_offset(ctrl_conf_slave, ARRAY_SIZE(ctrl_conf_slave),
				PCI_BDF(busn[j], 0, 0), io_base[j]);
	}

#if 0
	for(j=0; j< ck804_num; j++) {
		/* PCI-E (XSPLL) SS table 0x40, x044, 0x48 */
		/* SATA  (SPPLL) SS table 0xb0, 0xb4, 0xb8 */
		/* CPU   (PPLL)  SS table 0xc0, 0xc4, 0xc8 */
		setup_ss_table(io_base[j] + ANACTRL_IO_BASE+0x40,
			       io_base[j] + ANACTRL_IO_BASE+0x44,
			       io_base[j] + ANACTRL_IO_BASE+0x48,
			       pcie_ss_tbl, 64);
		setup_ss_table(io_base[j] + ANACTRL_IO_BASE+0xb0,
			       io_base[j] + ANACTRL_IO_BASE+0xb4,
			       io_base[j] + ANACTRL_IO_BASE+0xb8,
			       sata_ss_tbl, 64);
//PANTA		setup_ss_table(io_base[j] + ANACTRL_IO_BASE+0xc0,
//			       io_base[j] + ANACTRL_IO_BASE+0xc4,
//			       io_base[j] + ANACTRL_IO_BASE+0xc8,
//			       cpu_ss_tbl, 64);
	}
#endif
}

#ifndef HT_CHAIN_NUM_MAX

#define HT_CHAIN_NUM_MAX	4
#define HT_CHAIN_BUSN_D	0x40
#define HT_CHAIN_IOBASE_D	0x4000

#endif

int ck804_early_setup_x(void)
{
	unsigned busn[4], io_base[4];
	int i, ck804_num = 0;

	for (i = 0; i < 4; i++) {
		u32 id;
		u32 dev;
		if (i == 0) // SB chain
			dev = PCI_BDF(i * 0x40, CK804_DEVN_BASE, 0);
		else
			dev = PCI_BDF(i * 0x40, CK804B_DEVN_BASE, 0);
		id = pci_conf1_read_config32(dev, PCI_VENDOR_ID);
		if (id == 0x005e10de) {
			busn[ck804_num] = i * 0x40;
			io_base[ck804_num] = i * 0x4000;
			ck804_num++;
		}
	}

	ck804_early_set_port(ck804_num, busn, io_base);
	ck804_early_setup(ck804_num, busn, io_base);
	ck804_early_clear_port(ck804_num, busn, io_base);

	//return set_ht_link_ck804(4);
	return 0;
}

unsigned int get_sbdn(unsigned int bus)
{
	return CK804_DEVN_BASE;
}

void set_bios_reset(void);
void hard_reset(void)
{
	set_bios_reset();

	/* full reset */
	outb(0x0a, 0x0cf9);
	outb(0x0e, 0x0cf9);
}

void enable_fid_change_on_sb(unsigned sbbusn, unsigned sbdn)
{
	/* default value for ck804 is good */
}

void soft_reset(void)
{
	set_bios_reset();
	/* link reset */
	outb(0x02, 0x0cf9);
	outb(0x06, 0x0cf9);
}

void soft_reset_x(unsigned sbbusn, unsigned sbdn)
{
	soft_reset();
}
