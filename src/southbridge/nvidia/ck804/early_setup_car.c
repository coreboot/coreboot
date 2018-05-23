/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
 *
 * Copyright (C) 2004 Tyan Computer
 * Written by Yinghai Lu <yhlu@tyan.com> for Tyan Computer.
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

#include "ck804.h"

/* Someone messed up and snuck in some K8-specific code */
static int  set_ht_link_buffer_counts_chain(uint8_t ht_c_num, unsigned vendorid, unsigned val) { return 0; /* stub */};

static int set_ht_link_ck804(u8 ht_c_num)
{
	unsigned vendorid = 0x10de;
	unsigned val = 0x01610169;
	return set_ht_link_buffer_counts_chain(ht_c_num, vendorid, val);
}

static void setup_ss_table(unsigned index, unsigned where, unsigned control,
			   const unsigned int *register_values, int max)
{
	int i;
	unsigned val;

	val = inl(control);
	val &= 0xfffffffe;
	outl(val, control);

	outl(0, index);

	for (i = 0; i < max; i++) {
		unsigned long reg;
		reg = register_values[i];
		outl(reg, where);
	}
	val = inl(control);
	val |= 1;
	outl(val, control);
}

#define ANACTRL_IO_BASE 0x3000
#define ANACTRL_REG_POS 0x68

#define SYSCTRL_IO_BASE 0x2000
#define SYSCTRL_REG_POS 0x64

/*
 * Values for CONFIG_CK804_PCI_E_X and CONFIG_CK804B_PCI_E_X.
 * Apparently some sort of lane configuration.
 *
 *	16 1 1 2 :0
 *	 8 8 2 2 :1
 *	 8 8 4   :2
 *	 8 4 4 4 :3
 *	16 4     :4
 */

static void ck804_early_set_port(unsigned ck804_num, unsigned *busn,
				 unsigned *io_base)
{
	static const unsigned int ctrl_devport_conf[] = {
		PCI_ADDR(0, 0x1, 0, ANACTRL_REG_POS), ~(0x0000ff00), ANACTRL_IO_BASE,
		PCI_ADDR(0, 0x1, 0, SYSCTRL_REG_POS), ~(0x0000ff00), SYSCTRL_IO_BASE,
	};

	int j;
	for (j = 0; j < ck804_num; j++) {
		u32 dev;
		if (busn[j] == 0) /* SB chain */
			dev = PCI_DEV(busn[j], CK804_DEVN_BASE, 0);
		else
			dev = PCI_DEV(busn[j], CK804B_DEVN_BASE, 0);
		setup_resource_map_offset(ctrl_devport_conf,
			ARRAY_SIZE(ctrl_devport_conf), dev, io_base[j]);
	}
}

static void ck804_early_clear_port(unsigned ck804_num, unsigned *busn,
				   unsigned *io_base)
{
	static const unsigned int ctrl_devport_conf_clear[] = {
		PCI_ADDR(0, 0x1, 0, ANACTRL_REG_POS), ~(0x0000ff01), 0,
		PCI_ADDR(0, 0x1, 0, SYSCTRL_REG_POS), ~(0x0000ff01), 0,
	};

	int j;
	for (j = 0; j < ck804_num; j++) {
		u32 dev;
		if (busn[j] == 0) /* SB chain */
			dev = PCI_DEV(busn[j], CK804_DEVN_BASE, 0);
		else
			dev = PCI_DEV(busn[j], CK804B_DEVN_BASE, 0);
		setup_resource_map_offset(ctrl_devport_conf_clear,
			ARRAY_SIZE(ctrl_devport_conf_clear), dev, io_base[j]);
	}
}

static void ck804_early_setup(unsigned ck804_num, unsigned *busn,
			      unsigned *io_base)
{
	static const unsigned int ctrl_conf_master[] = {
		RES_PCI_IO, PCI_ADDR(0, 1, 2, 0x8c), 0xffff0000, 0x00009880,
		RES_PCI_IO, PCI_ADDR(0, 1, 2, 0x90), 0xffff000f, 0x000074a0,
		RES_PCI_IO, PCI_ADDR(0, 1, 2, 0xa0), 0xfffff0ff, 0x00000a00,
		RES_PCI_IO, PCI_ADDR(0, 1, 2, 0xac), 0xffffff00, 0x00000000,

		RES_PCI_IO, PCI_ADDR(0, 0, 0, 0x48), 0xfffffffd, 0x00000002,
		RES_PCI_IO, PCI_ADDR(0, 0, 0, 0x74), 0xfffff00f, 0x000009d0,
		RES_PCI_IO, PCI_ADDR(0, 0, 0, 0x8c), 0xffff0000, 0x0000007f,
		RES_PCI_IO, PCI_ADDR(0, 0, 0, 0xcc), 0xfffffff8, 0x00000003,
		RES_PCI_IO, PCI_ADDR(0, 0, 0, 0xd0), 0xff000000, 0x00000000,
		RES_PCI_IO, PCI_ADDR(0, 0, 0, 0xd4), 0xff000000, 0x00000000,
		RES_PCI_IO, PCI_ADDR(0, 0, 0, 0xd8), 0xff000000, 0x00000000,
		RES_PCI_IO, PCI_ADDR(0, 0, 0, 0xdc), 0x7f000000, 0x00000000,

		RES_PCI_IO, PCI_ADDR(0, 1, 0, 0xf0), 0xfffffffd, 0x00000002,
		RES_PCI_IO, PCI_ADDR(0, 1, 0, 0xf8), 0xffffffcf, 0x00000010,

		RES_PCI_IO, PCI_ADDR(0, 9, 0, 0x40), 0xfff8ffff, 0x00030000,
		RES_PCI_IO, PCI_ADDR(0, 9, 0, 0x4c), 0xfe00ffff, 0x00440000,
		RES_PCI_IO, PCI_ADDR(0, 9, 0, 0x74), 0xffffffc0, 0x00000000,

#ifdef CK804_MB_SETUP
		CK804_MB_SETUP
#endif

#if IS_ENABLED(CONFIG_NORTHBRIDGE_AMD_AMDFAM10)
		/*
		 * Avoid crash (complete with severe memory corruption!) during initial CAR boot
		 * in ck804_early_setup_x() on Fam10h systems by not touching 0x78.
		 * Interestingly once the system is fully booted into Linux this can be set, but
		 * not before!  Apparently something isn't initialized but the amount of effort
		 * required to fix this is non-negligible and of unknown real-world benefit
		 */
#else
		RES_PCI_IO, PCI_ADDR(0, 1, 0, 0x78), 0xc0ffffff, 0x19000000,
#endif
		RES_PCI_IO, PCI_ADDR(0, 1, 0, 0xe0), 0xfffffeff, 0x00000100,

		RES_PORT_IO_32, ANACTRL_IO_BASE + 0x20, 0xe00fffff, 0x11000000,
		RES_PORT_IO_32, ANACTRL_IO_BASE + 0x24, 0xc3f0ffff, 0x24040000,
		RES_PORT_IO_32, ANACTRL_IO_BASE + 0x80, 0x8c3f04df, 0x51407120,
		RES_PORT_IO_32, ANACTRL_IO_BASE + 0x84, 0xffffff8f, 0x00000010,
		RES_PORT_IO_32, ANACTRL_IO_BASE + 0x94, 0xff00ffff, 0x00c00000,
		RES_PORT_IO_32, ANACTRL_IO_BASE + 0xcc, 0xf7ffffff, 0x00000000,

		RES_PORT_IO_32, ANACTRL_IO_BASE + 0x74, ~(0xffff), 0x0f008,
		RES_PORT_IO_32, ANACTRL_IO_BASE + 0x78, ~((0xff) | (0xff << 16)), (0x41 << 16) | (0x32),
		RES_PORT_IO_32, ANACTRL_IO_BASE + 0x7c, ~(0xff << 16), (0xa0 << 16),

		RES_PORT_IO_32, ANACTRL_IO_BASE + 0x24, 0xfcffff0f, 0x020000b0,

		/* Activate master port on primary SATA controller. */
		RES_PCI_IO, PCI_ADDR(0, 7, 0, 0x50), ~(0x1f000013), 0x15000013,
		RES_PCI_IO, PCI_ADDR(0, 7, 0, 0x64), ~(0x00000001), 0x00000001,
		RES_PCI_IO, PCI_ADDR(0, 7, 0, 0x68), ~(0x02000000), 0x02000000,
		RES_PCI_IO, PCI_ADDR(0, 7, 0, 0x70), ~(0x000f0000), 0x00040000,
		RES_PCI_IO, PCI_ADDR(0, 7, 0, 0xa0), ~(0x000001ff), 0x00000150,
		RES_PCI_IO, PCI_ADDR(0, 7, 0, 0xac), ~(0xffff8f00), 0x02aa8b00,
		RES_PCI_IO, PCI_ADDR(0, 7, 0, 0x7c), ~(0x00000010), 0x00000000,
		RES_PCI_IO, PCI_ADDR(0, 7, 0, 0xc8), ~(0x0fff0fff), 0x000a000a,
		RES_PCI_IO, PCI_ADDR(0, 7, 0, 0xd0), ~(0xf0000000), 0x00000000,
		RES_PCI_IO, PCI_ADDR(0, 7, 0, 0xe0), ~(0xf0000000), 0x00000000,

		RES_PCI_IO, PCI_ADDR(0, 8, 0, 0x50), ~(0x1f000013), 0x15000013,
		RES_PCI_IO, PCI_ADDR(0, 8, 0, 0x64), ~(0x00000001), 0x00000001,
		RES_PCI_IO, PCI_ADDR(0, 8, 0, 0x68), ~(0x02000000), 0x02000000,
		RES_PCI_IO, PCI_ADDR(0, 8, 0, 0x70), ~(0x000f0000), 0x00040000,
		RES_PCI_IO, PCI_ADDR(0, 8, 0, 0xa0), ~(0x000001ff), 0x00000150,
		RES_PCI_IO, PCI_ADDR(0, 8, 0, 0xac), ~(0xffff8f00), 0x02aa8b00,
		RES_PCI_IO, PCI_ADDR(0, 8, 0, 0x7c), ~(0x00000010), 0x00000000,
		RES_PCI_IO, PCI_ADDR(0, 8, 0, 0xc8), ~(0x0fff0fff), 0x000a000a,
		RES_PCI_IO, PCI_ADDR(0, 8, 0, 0xd0), ~(0xf0000000), 0x00000000,
		RES_PCI_IO, PCI_ADDR(0, 8, 0, 0xe0), ~(0xf0000000), 0x00000000,

		RES_PORT_IO_32, ANACTRL_IO_BASE + 0x04, ~((0x3ff << 0) | (0x3ff << 10)), (0x21 << 0) | (0x22 << 10),

		RES_PORT_IO_32, ANACTRL_IO_BASE + 0x08, ~(0xfffff), (0x1c << 10) | 0x1b,

		RES_PORT_IO_32, ANACTRL_IO_BASE + 0x80, ~(1 << 3), 0x00000000,

		RES_PORT_IO_32, ANACTRL_IO_BASE + 0xcc, ~((7 << 4) | (1 << 8)), (CONFIG_CK804_PCI_E_X << 4) | (1 << 8),

		/* SYSCTRL */
		RES_PORT_IO_8, SYSCTRL_IO_BASE + 0xc0 + 8, ~(0xff), ((0 << 4) | (0 << 2) | (0 << 0)),
		RES_PORT_IO_8, SYSCTRL_IO_BASE + 0xc0 + 9, ~(0xff), ((0 << 4) | (1 << 2) | (1 << 0)),
#if IS_ENABLED(CONFIG_CK804_USE_NIC)
		RES_PCI_IO, PCI_ADDR(0, 0xa, 0, 0xf8), 0xffffffbf, 0x00000040,
		RES_PORT_IO_8, SYSCTRL_IO_BASE + 0xc0 + 19, ~(0xff), ((0 << 4) | (1 << 2) | (0 << 0)),
		RES_PORT_IO_8, SYSCTRL_IO_BASE + 0xc0 + 3, ~(0xff), ((0 << 4) | (1 << 2) | (0 << 0)),
		RES_PORT_IO_8, SYSCTRL_IO_BASE + 0xc0 + 3, ~(0xff), ((0 << 4) | (1 << 2) | (1 << 0)),
		RES_PCI_IO, PCI_ADDR(0, 1, 0, 0xe4), ~(1 << 23), (1 << 23),
#endif

#if IS_ENABLED(CONFIG_CK804_USE_ACI)
		RES_PORT_IO_8, SYSCTRL_IO_BASE + 0xc0 + 0x0d, ~(0xff), ((0 << 4) | (2 << 2) | (0 << 0)),
		RES_PORT_IO_8, SYSCTRL_IO_BASE + 0xc0 + 0x1a, ~(0xff), ((0 << 4) | (2 << 2) | (0 << 0)),
#endif

#if IS_ENABLED(CONFIG_CK804_PCIE_PME_WAKE)
		RES_PCI_IO, PCI_ADDR(0, 1, 0, 0xe4), 0xffffffff, 0x00400000,
#else
		RES_PCI_IO, PCI_ADDR(0, 1, 0, 0xe4), 0xffbfffff, 0x00000000,
#endif
	};

	static const unsigned int ctrl_conf_multiple[] = {
		RES_PORT_IO_8, SYSCTRL_IO_BASE + 0xc0 + 0, ~(3 << 2), (0 << 2),
	};

	static const unsigned int ctrl_conf_slave[] = {
		RES_PCI_IO, PCI_ADDR(0, 1, 2, 0x8c), 0xffff0000, 0x00009880,
		RES_PCI_IO, PCI_ADDR(0, 1, 2, 0x90), 0xffff000f, 0x000074a0,
		RES_PCI_IO, PCI_ADDR(0, 1, 2, 0xa0), 0xfffff0ff, 0x00000a00,

		RES_PCI_IO, PCI_ADDR(0, 0, 0, 0x48), 0xfffffffd, 0x00000002,
		RES_PCI_IO, PCI_ADDR(0, 0, 0, 0x74), 0xfffff00f, 0x000009d0,
		RES_PCI_IO, PCI_ADDR(0, 0, 0, 0x8c), 0xffff0000, 0x0000007f,
		RES_PCI_IO, PCI_ADDR(0, 0, 0, 0xcc), 0xfffffff8, 0x00000003,
		RES_PCI_IO, PCI_ADDR(0, 0, 0, 0xd0), 0xff000000, 0x00000000,
		RES_PCI_IO, PCI_ADDR(0, 0, 0, 0xd4), 0xff000000, 0x00000000,
		RES_PCI_IO, PCI_ADDR(0, 0, 0, 0xd8), 0xff000000, 0x00000000,
		RES_PCI_IO, PCI_ADDR(0, 0, 0, 0xdc), 0x7f000000, 0x00000000,

		RES_PCI_IO, PCI_ADDR(0, 1, 0, 0xf0), 0xfffffffd, 0x00000002,
		RES_PCI_IO, PCI_ADDR(0, 1, 0, 0xf8), 0xffffffcf, 0x00000010,

		RES_PCI_IO, PCI_ADDR(0, 9, 0, 0x40), 0xfff8ffff, 0x00030000,
		RES_PCI_IO, PCI_ADDR(0, 9, 0, 0x4c), 0xfe00ffff, 0x00440000,
		RES_PCI_IO, PCI_ADDR(0, 9, 0, 0x74), 0xffffffc0, 0x00000000,

		/*
		 * Avoid touching 0x78 for CONFIG_NORTHBRIDGE_AMD_AMDFAM10 for
		 * non-primary chains too???
		 */
		RES_PCI_IO, PCI_ADDR(0, 1, 0, 0x78), 0xc0ffffff, 0x20000000,
		RES_PCI_IO, PCI_ADDR(0, 1, 0, 0xe0), 0xfffffeff, 0x00000000,
		RES_PCI_IO, PCI_ADDR(0, 1, 0, 0xe8), 0xffffff00, 0x000000ff,

		RES_PORT_IO_32, ANACTRL_IO_BASE + 0x20, 0xe00fffff, 0x11000000,
		RES_PORT_IO_32, ANACTRL_IO_BASE + 0x24, 0xc3f0ffff, 0x24040000,
		RES_PORT_IO_32, ANACTRL_IO_BASE + 0x80, 0x8c3f04df, 0x51407120,
		RES_PORT_IO_32, ANACTRL_IO_BASE + 0x84, 0xffffff8f, 0x00000010,
		RES_PORT_IO_32, ANACTRL_IO_BASE + 0x94, 0xff00ffff, 0x00c00000,
		RES_PORT_IO_32, ANACTRL_IO_BASE + 0xcc, 0xf7ffffff, 0x00000000,

		RES_PORT_IO_32, ANACTRL_IO_BASE + 0x24, 0xfcffff0f, 0x020000b0,

		RES_PCI_IO, PCI_ADDR(0, 8, 0, 0x50), ~(0x1f000013), 0x15000013,
		RES_PCI_IO, PCI_ADDR(0, 8, 0, 0x64), ~(0x00000001), 0x00000001,
		RES_PCI_IO, PCI_ADDR(0, 8, 0, 0x68), ~(0x02000000), 0x02000000,
		RES_PCI_IO, PCI_ADDR(0, 8, 0, 0x70), ~(0x000f0000), 0x00040000,
		RES_PCI_IO, PCI_ADDR(0, 8, 0, 0xa0), ~(0x000001ff), 0x00000150,
		RES_PCI_IO, PCI_ADDR(0, 8, 0, 0xac), ~(0xffff8f00), 0x02aa8b00,
		RES_PCI_IO, PCI_ADDR(0, 8, 0, 0x7c), ~(0x00000010), 0x00000000,
		RES_PCI_IO, PCI_ADDR(0, 8, 0, 0xc8), ~(0x0fff0fff), 0x000a000a,
		RES_PCI_IO, PCI_ADDR(0, 8, 0, 0xd0), ~(0xf0000000), 0x00000000,
		RES_PCI_IO, PCI_ADDR(0, 8, 0, 0xe0), ~(0xf0000000), 0x00000000,

		RES_PORT_IO_32, ANACTRL_IO_BASE + 0x04, ~((0x3ff << 0) | (0x3ff << 10)), (0x21 << 0) | (0x22 << 10),

		RES_PORT_IO_32, ANACTRL_IO_BASE + 0x08, ~(0xfffff), (0x1c << 10) | 0x1b,

		/* This line doesn't exist in the non-CAR version. */
		RES_PORT_IO_32, ANACTRL_IO_BASE + 0x80, ~(1 << 3), 0x00000000,

		RES_PORT_IO_32, ANACTRL_IO_BASE + 0xcc, ~((7 << 4) | (1 << 8)), (CONFIG_CK804B_PCI_E_X << 4) | (1 << 8),

#if IS_ENABLED(CONFIG_CK804_USE_NIC)
		RES_PCI_IO, PCI_ADDR(0, 0xa, 0, 0xf8), 0xffffffbf, 0x00000040,
		RES_PORT_IO_8, SYSCTRL_IO_BASE + 0xc0 + 19, ~(0xff), ((0 << 4) | (1 << 2) | (0 << 0)),
		RES_PORT_IO_8, SYSCTRL_IO_BASE + 0xc0 + 3, ~(0xff), ((0 << 4) | (1 << 2) | (0 << 0)),
		RES_PORT_IO_8, SYSCTRL_IO_BASE + 0xc0 + 3, ~(0xff), ((0 << 4) | (1 << 2) | (1 << 0)),
		RES_PCI_IO, PCI_ADDR(0, 1, 0, 0xe4), ~(1 << 23), (1 << 23),
#endif
	};

	int j;
	for (j = 0; j < ck804_num; j++) {
		if (busn[j] == 0) {
			setup_resource_map_x_offset(ctrl_conf_master,
				ARRAY_SIZE(ctrl_conf_master),
				PCI_DEV(0, CK804_DEVN_BASE, 0), io_base[0]);
			if (ck804_num > 1)
				setup_resource_map_x_offset(ctrl_conf_multiple,
					ARRAY_SIZE(ctrl_conf_multiple),
					PCI_DEV(0, CK804_DEVN_BASE, 0), 0);

			continue;
		}

		setup_resource_map_x_offset(ctrl_conf_slave,
			ARRAY_SIZE(ctrl_conf_slave),
			PCI_DEV(busn[j], CK804B_DEVN_BASE, 0), io_base[j]);
	}

	for (j = 0; j < ck804_num; j++) {
		/* PCI-E (XSPLL) SS table 0x40, x044, 0x48 */
		/* SATA  (SPPLL) SS table 0xb0, 0xb4, 0xb8 */
		/* CPU   (PPLL)  SS table 0xc0, 0xc4, 0xc8 */
		setup_ss_table(io_base[j] + ANACTRL_IO_BASE + 0x40,
			       io_base[j] + ANACTRL_IO_BASE + 0x44,
			       io_base[j] + ANACTRL_IO_BASE + 0x48,
			       pcie_ss_tbl, 64);
		setup_ss_table(io_base[j] + ANACTRL_IO_BASE + 0xb0,
			       io_base[j] + ANACTRL_IO_BASE + 0xb4,
			       io_base[j] + ANACTRL_IO_BASE + 0xb8,
			       sata_ss_tbl, 64);
		setup_ss_table(io_base[j] + ANACTRL_IO_BASE + 0xc0,
			       io_base[j] + ANACTRL_IO_BASE + 0xc4,
			       io_base[j] + ANACTRL_IO_BASE + 0xc8,
			       cpu_ss_tbl, 64);
	}
}

static int ck804_early_setup_x(void)
{
	unsigned busn[4], io_base[4];
	int i, ck804_num = 0;

	for (i = 0; i < 4; i++) {
		u32 id;
		pci_devfn_t dev;
		if (i == 0) /* SB chain */
			dev = PCI_DEV(i * 0x40, CK804_DEVN_BASE, 0);
		else
			dev = PCI_DEV(i * 0x40, CK804B_DEVN_BASE, 0);
		id = pci_read_config32(dev, PCI_VENDOR_ID);
		if (id == 0x005e10de) {
			busn[ck804_num] = i * 0x40;
			io_base[ck804_num] = i * 0x4000;
			ck804_num++;
		}
	}

	printk(BIOS_DEBUG, "ck804_early_set_port(%d, %d, %d)\n", ck804_num, busn[0], io_base[0]);
	ck804_early_set_port(ck804_num, busn, io_base);
	printk(BIOS_DEBUG, "ck804_early_setup(%d, %d, %d)\n", ck804_num, busn[0], io_base[0]);
	ck804_early_setup(ck804_num, busn, io_base);
	printk(BIOS_DEBUG, "ck804_early_clear_port(%d, %d, %d)\n", ck804_num, busn[0], io_base[0]);
	ck804_early_clear_port(ck804_num, busn, io_base);

	return set_ht_link_ck804(4);
}

void do_hard_reset(void)
{
	set_bios_reset();

	/* full reset */
	outb(0x0a, 0x0cf9);
	outb(0x0e, 0x0cf9);
}

void do_soft_reset(void)
{
	set_bios_reset();

	/* link reset */
	outb(0x02, 0x0cf9);
	outb(0x06, 0x0cf9);
}

void enable_fid_change_on_sb(unsigned sbbusn, unsigned sbdn)
{
	/* The default value for CK804 is good. */
	/* Set VFSMAF (VID/FID System Management Action Field) to 2. */
}
