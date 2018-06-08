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
 */

#ifdef UNUSED_CODE
int set_ht_link_buffer_counts_chain(u8 ht_c_num, unsigned vendorid, unsigned val);

static int set_ht_link_mcp55(u8 ht_c_num)
{
	unsigned vendorid = 0x10de;
	unsigned val = 0x01610109;
	/* NVIDIA MCP55 hardcode, hardware can not set it automatically. */
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

	outl(0, index); /* Index */
	for (i = 0; i < max; i++) {
		unsigned long reg;
		reg = register_values[i];
		outl(reg, where);
	}

	val = inl(control);
	val |= 1;
	outl(val, control);
}
#endif

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
 * 16 1 1 1 1 8 :0
 * 16 0 4 0 0 8 :1
 * 16 0 4 2 2 4 :2
 *  4 4 4 4 4 8 :3
 *  8 8 4 0 0 8 :4
 *  8 0 4 4 4 8 :5
*/

#define MCP55_CHIP_REV 3

static void mcp55_early_set_port(unsigned mcp55_num, unsigned *busn,
				 unsigned *devn, unsigned *io_base)
{

	static const unsigned int ctrl_devport_conf[] = {
		PCI_ADDR(0, 1, 1, ANACTRL_REG_POS), ~(0x0000ff00), ANACTRL_IO_BASE,
		PCI_ADDR(0, 1, 1, SYSCTRL_REG_POS), ~(0x0000ff00), SYSCTRL_IO_BASE,
		PCI_ADDR(0, 1, 1, ACPICTRL_REG_POS), ~(0x0000ff00), ACPICTRL_IO_BASE,
	};

	int j;
	for (j = 0; j < mcp55_num; j++) {
		setup_resource_map_offset(ctrl_devport_conf,
			ARRAY_SIZE(ctrl_devport_conf),
			PCI_DEV(busn[j], devn[j], 0) , io_base[j]);
	}
}

static void mcp55_early_clear_port(unsigned mcp55_num, unsigned *busn,
				   unsigned *devn, unsigned *io_base)
{
	static const unsigned int ctrl_devport_conf_clear[] = {
		PCI_ADDR(0, 1, 1, ANACTRL_REG_POS), ~(0x0000ff00), 0,
		PCI_ADDR(0, 1, 1, SYSCTRL_REG_POS), ~(0x0000ff00), 0,
		PCI_ADDR(0, 1, 1, ACPICTRL_REG_POS), ~(0x0000ff00), 0,
	};

	int j;
	for (j = 0; j < mcp55_num; j++) {
		setup_resource_map_offset(ctrl_devport_conf_clear,
			ARRAY_SIZE(ctrl_devport_conf_clear),
			PCI_DEV(busn[j], devn[j], 0) , io_base[j]);
	}
}

static void mcp55_early_pcie_setup(unsigned busnx, unsigned devnx,
				   unsigned anactrl_io_base, unsigned pci_e_x)
{
	u32 tgio_ctrl, pll_ctrl, dword;
	int i;
	pci_devfn_t dev;

	dev = PCI_DEV(busnx, devnx + 1, 1);

	dword = pci_read_config32(dev, 0xe4);
	dword |= 0x3f0; /* Disable it at first. */
	pci_write_config32(dev, 0xe4, dword);

	for (i = 0; i < 3; i++) {
		tgio_ctrl = inl(anactrl_io_base + 0xcc);
		tgio_ctrl &= ~(3 << 9);
		tgio_ctrl |= (i << 9);
		outl(tgio_ctrl, anactrl_io_base + 0xcc);
		pll_ctrl = inl(anactrl_io_base + 0x30);
		pll_ctrl |= (1 << 31);
		outl(pll_ctrl, anactrl_io_base + 0x30);
		do {
			pll_ctrl = inl(anactrl_io_base + 0x30);
		} while (!(pll_ctrl & 1));
	}
	tgio_ctrl = inl(anactrl_io_base + 0xcc);
	tgio_ctrl &= ~((7 << 4) | (1 << 8));
	tgio_ctrl |= (pci_e_x << 4) | (1 << 8);
	outl(tgio_ctrl, anactrl_io_base + 0xcc);

	udelay(100); /* Wait 100us. */

	dword = pci_read_config32(dev, 0xe4);
	dword &= ~(0x3f0); /* Enable. */
	pci_write_config32(dev, 0xe4, dword);

	mdelay(100); /* Need to wait 100ms. */
}

static void mcp55_early_setup(unsigned mcp55_num, unsigned *busn,
			      unsigned *devn, unsigned *io_base,
			      unsigned *pci_e_x)
{
    static const unsigned int ctrl_conf_1[] = {
	RES_PORT_IO_32, ACPICTRL_IO_BASE + 0x10, 0x0007ffff, 0xff78000,
	RES_PORT_IO_32, ACPICTRL_IO_BASE + 0xa4, 0xffedffff, 0x0012000,
	RES_PORT_IO_32, ACPICTRL_IO_BASE + 0xac, 0xfffffdff, 0x0000200,
	RES_PORT_IO_32, ACPICTRL_IO_BASE + 0xb4, 0xfffffffd, 0x0000002,

	RES_PORT_IO_32, ANACTRL_IO_BASE + 0x24, 0xc0f0f08f, 0x26020230,
	RES_PORT_IO_32, ANACTRL_IO_BASE + 0x34, 0x00000000, 0x22222222,
	RES_PORT_IO_32, ANACTRL_IO_BASE + 0x08, 0x7FFFFFFF, 0x00000000,
	RES_PORT_IO_32, ANACTRL_IO_BASE + 0x2C, 0x7FFFFFFF, 0x80000000,
	RES_PORT_IO_32, ANACTRL_IO_BASE + 0xCC, 0xFFFFF9FF, 0x00000000,
	RES_PORT_IO_32, ANACTRL_IO_BASE + 0x30, 0x8FFFFFFF, 0x40000000,
	RES_PORT_IO_32, ANACTRL_IO_BASE + 0xCC, 0xFFFFF9FF, 0x00000200,
	RES_PORT_IO_32, ANACTRL_IO_BASE + 0x30, 0x8FFFFFFF, 0x40000000,
	RES_PORT_IO_32, ANACTRL_IO_BASE + 0xCC, 0xFFFFF9FF, 0x00000400,
	RES_PORT_IO_32, ANACTRL_IO_BASE + 0x30, 0x8FFFFFFF, 0x40000000,
	RES_PORT_IO_32, ANACTRL_IO_BASE + 0x74, 0xFFFF0FF5, 0x0000F000,
	RES_PORT_IO_32, ANACTRL_IO_BASE + 0x78, 0xFF00FF00, 0x00100010,
	RES_PORT_IO_32, ANACTRL_IO_BASE + 0x7C, 0xFF0FF0FF, 0x00500500,
	RES_PORT_IO_32, ANACTRL_IO_BASE + 0x80, 0xFFFFFFE7, 0x00000000,
	RES_PORT_IO_32, ANACTRL_IO_BASE + 0x60, 0xFFCFFFFF, 0x00300000,
	RES_PORT_IO_32, ANACTRL_IO_BASE + 0x90, 0xFFFF00FF, 0x0000FF00,
	RES_PORT_IO_32, ANACTRL_IO_BASE + 0x9C, 0xFF00FFFF, 0x00070000,

	RES_PCI_IO, PCI_ADDR(0, 0, 0, 0x40), 0x00000000, 0xCB8410DE,
	RES_PCI_IO, PCI_ADDR(0, 0, 0, 0x48), 0xFFFFDCED, 0x00002002,
	RES_PCI_IO, PCI_ADDR(0, 0, 0, 0x78), 0xFFFFFF8E, 0x00000011,
	RES_PCI_IO, PCI_ADDR(0, 0, 0, 0x80), 0xFFFF0000, 0x00009923,
	RES_PCI_IO, PCI_ADDR(0, 0, 0, 0x88), 0xFFFFFFFE, 0x00000000,
	RES_PCI_IO, PCI_ADDR(0, 0, 0, 0x8C), 0xFFFF0000, 0x0000007F,
	RES_PCI_IO, PCI_ADDR(0, 0, 0, 0xDC), 0xFFFEFFFF, 0x00010000,

	RES_PCI_IO, PCI_ADDR(0, 1, 0, 0x40), 0x00000000, 0xCB8410DE,
	RES_PCI_IO, PCI_ADDR(0, 1, 0, 0x74), 0xFFFFFF7B, 0x00000084,
	RES_PCI_IO, PCI_ADDR(0, 1, 0, 0xF8), 0xFFFFFFCF, 0x00000010,

	RES_PCI_IO, PCI_ADDR(0, 1, 1, 0xC4), 0xFFFFFFFE, 0x00000001,
	RES_PCI_IO, PCI_ADDR(0, 1, 1, 0xF0), 0x7FFFFFFD, 0x00000002,
	RES_PCI_IO, PCI_ADDR(0, 1, 1, 0xF8), 0xFFFFFFCF, 0x00000010,

	RES_PCI_IO, PCI_ADDR(0, 8, 0, 0x40), 0x00000000, 0xCB8410DE,
	RES_PCI_IO, PCI_ADDR(0, 8, 0, 0x68), 0xFFFFFF00, 0x000000FF,
	RES_PCI_IO, PCI_ADDR(0, 8, 0, 0xF8), 0xFFFFFFBF, 0x00000040, /* Enable bridge mode. */

	RES_PCI_IO, PCI_ADDR(0, 9, 0, 0x40), 0x00000000, 0xCB8410DE,
	RES_PCI_IO, PCI_ADDR(0, 9, 0, 0x68), 0xFFFFFF00, 0x000000FF,
	RES_PCI_IO, PCI_ADDR(0, 9, 0, 0xF8), 0xFFFFFFBF, 0x00000040, /* Enable bridge mode. */
    };

    static const unsigned int ctrl_conf_1_1[] = {
	RES_PCI_IO, PCI_ADDR(0, 5, 0, 0x40), 0x00000000, 0xCB8410DE,
	RES_PCI_IO, PCI_ADDR(0, 5, 0, 0x50), 0xFFFFFFFC, 0x00000003,
	RES_PCI_IO, PCI_ADDR(0, 5, 0, 0x64), 0xFFFFFFFE, 0x00000001,
	RES_PCI_IO, PCI_ADDR(0, 5, 0, 0x70), 0xFFF0FFFF, 0x00040000,
	RES_PCI_IO, PCI_ADDR(0, 5, 0, 0xAC), 0xFFFFF0FF, 0x00000100,
	RES_PCI_IO, PCI_ADDR(0, 5, 0, 0x7C), 0xFFFFFFEF, 0x00000000,
	RES_PCI_IO, PCI_ADDR(0, 5, 0, 0xC8), 0xFF00FF00, 0x000A000A,
	RES_PCI_IO, PCI_ADDR(0, 5, 0, 0xD0), 0xF0FFFFFF, 0x03000000,
	RES_PCI_IO, PCI_ADDR(0, 5, 0, 0xE0), 0xF0FFFFFF, 0x03000000,
    };

    static const unsigned int ctrl_conf_mcp55_only[] = {
	RES_PCI_IO, PCI_ADDR(0, 1, 1, 0x40), 0x00000000, 0xCB8410DE,
	RES_PCI_IO, PCI_ADDR(0, 1, 1, 0xE0), 0xFFFFFEFF, 0x00000000,
	RES_PCI_IO, PCI_ADDR(0, 1, 1, 0xE4), 0xFFFFFFFB, 0x00000000,
	RES_PCI_IO, PCI_ADDR(0, 1, 1, 0xE8), 0xFFA9C8FF, 0x00003000,

	RES_PCI_IO, PCI_ADDR(0, 4, 0, 0x40), 0x00000000, 0xCB8410DE,
	RES_PCI_IO, PCI_ADDR(0, 4, 0, 0xF8), 0xFFFFFFCF, 0x00000010,

	RES_PCI_IO, PCI_ADDR(0, 2, 0, 0x40), 0x00000000, 0xCB8410DE,

	RES_PCI_IO, PCI_ADDR(0, 2, 1, 0x40), 0x00000000, 0xCB8410DE,
	RES_PCI_IO, PCI_ADDR(0, 2, 1, 0x64), 0xF87FFFFF, 0x05000000,
	RES_PCI_IO, PCI_ADDR(0, 2, 1, 0x78), 0xFFC07FFF, 0x00360000,
	RES_PCI_IO, PCI_ADDR(0, 2, 1, 0x68), 0xFE00D03F, 0x013F2C00,
	RES_PCI_IO, PCI_ADDR(0, 2, 1, 0x70), 0xFFF7FFFF, 0x00080000,
	RES_PCI_IO, PCI_ADDR(0, 2, 1, 0x7C), 0xFFFFF00F, 0x00000570,
	RES_PCI_IO, PCI_ADDR(0, 2, 1, 0xF8), 0xFFFFFFCF, 0x00000010,

	RES_PCI_IO, PCI_ADDR(0, 6, 0, 0x04), 0xFFFFFEFB, 0x00000104,
	RES_PCI_IO, PCI_ADDR(0, 6, 0, 0x3C), 0xF5FFFFFF, 0x0A000000,
	RES_PCI_IO, PCI_ADDR(0, 6, 0, 0x40), 0x00C8FFFF, 0x07330000,
	RES_PCI_IO, PCI_ADDR(0, 6, 0, 0x48), 0xFFFFFFF8, 0x00000005,
	RES_PCI_IO, PCI_ADDR(0, 6, 0, 0x4C), 0xFE02FFFF, 0x004C0000,
	RES_PCI_IO, PCI_ADDR(0, 6, 0, 0x74), 0xFFFFFFC0, 0x00000000,
	RES_PCI_IO, PCI_ADDR(0, 6, 0, 0xC0), 0x00000000, 0xCB8410DE,
	RES_PCI_IO, PCI_ADDR(0, 6, 0, 0xC4), 0xFFFFFFF8, 0x00000007,
#if IS_ENABLED(CONFIG_NORTHBRIDGE_AMD_AMDFAM10)
	/*
	 * Avoid crash (complete with severe memory corruption!) during initial CAR boot
	 * in mcp55_early_setup_x() on Fam10h systems by not touching 0x78.
	 * Interestingly once the system is fully booted into Linux this can be set, but
	 * not before!  Apparently something isn't initialized but the amount of effort
	 * required to fix this is non-negligible and of unknown real-world benefit
	 */
#else
	RES_PCI_IO, PCI_ADDR(0, 1, 0, 0x78), 0xC0FFFFFF, 0x19000000,
#endif

#if IS_ENABLED(CONFIG_MCP55_USE_AZA)
	RES_PCI_IO, PCI_ADDR(0, 6, 1, 0x40), 0x00000000, 0xCB8410DE,

#endif

#ifdef MCP55_MB_SETUP
	/* Play a while with GPIO in MCP55. */
	MCP55_MB_SETUP
#endif

#if IS_ENABLED(CONFIG_MCP55_USE_AZA)
	RES_PORT_IO_8, SYSCTRL_IO_BASE + 0xc0+ 21, ~(3 << 2), (2 << 2),
	RES_PORT_IO_8, SYSCTRL_IO_BASE + 0xc0+ 22, ~(3 << 2), (2 << 2),
	RES_PORT_IO_8, SYSCTRL_IO_BASE + 0xc0+ 46, ~(3 << 2), (2 << 2),
#endif
    };

    static const unsigned int ctrl_conf_master_only[] = {
	RES_PORT_IO_32, ACPICTRL_IO_BASE + 0x80, 0xEFFFFFF, 0x01000000,

	/* Master MCP55???? YHLU */
	RES_PORT_IO_8, SYSCTRL_IO_BASE + 0xc0+ 0, ~(3 << 2), (0 << 2),
    };

    static const unsigned int ctrl_conf_2[] = {
	/* I didn't put PCI-E related stuff here. */

	RES_PCI_IO, PCI_ADDR(0, 0, 0, 0x74), 0xFFFFF00F, 0x000009D0,
	RES_PCI_IO, PCI_ADDR(0, 1, 0, 0x74), 0xFFFF7FFF, 0x00008000,

	RES_PORT_IO_32, SYSCTRL_IO_BASE + 0x48, 0xFFFEFFFF, 0x00010000,

	RES_PORT_IO_32, ANACTRL_IO_BASE + 0x60, 0xFFFFFF00, 0x00000012,

#if IS_ENABLED(CONFIG_MCP55_USE_NIC)
	RES_PCI_IO, PCI_ADDR(0, 1, 1, 0xe4), ~((1 << 22) | (1 << 20)), (1 << 22) | (1 << 20),

	RES_PORT_IO_8, SYSCTRL_IO_BASE + 0xc0+ 4, ~(0xff), ((0 << 4) | (1 << 2) | (0 << 0)),
	RES_PORT_IO_8, SYSCTRL_IO_BASE + 0xc0+ 4, ~(0xff), ((0 << 4) | (1 << 2) | (1 << 0)),
#endif
    };

	int j, i;

	for (j = 0; j < mcp55_num; j++) {
		mcp55_early_pcie_setup(busn[j], devn[j],
				io_base[j] + ANACTRL_IO_BASE, pci_e_x[j]);

		setup_resource_map_x_offset(ctrl_conf_1,
				ARRAY_SIZE(ctrl_conf_1),
				PCI_DEV(busn[j], devn[j], 0), io_base[j]);

		for (i = 0; i < 3; i++) { /* Three SATA */
			setup_resource_map_x_offset(ctrl_conf_1_1,
				ARRAY_SIZE(ctrl_conf_1_1),
				PCI_DEV(busn[j], devn[j], i), io_base[j]);
		}

		if (busn[j] == 0) {
			setup_resource_map_x_offset(ctrl_conf_mcp55_only,
				ARRAY_SIZE(ctrl_conf_mcp55_only),
				PCI_DEV(busn[j], devn[j], 0), io_base[j]);
		}

		if ((busn[j] == 0) && (mcp55_num>1)) {
			setup_resource_map_x_offset(ctrl_conf_master_only,
				ARRAY_SIZE(ctrl_conf_master_only),
				PCI_DEV(busn[j], devn[j], 0), io_base[j]);
		}

		setup_resource_map_x_offset(ctrl_conf_2,
				ARRAY_SIZE(ctrl_conf_2),
				PCI_DEV(busn[j], devn[j], 0), io_base[j]);
	}

}

#ifndef HT_CHAIN_NUM_MAX

#define HT_CHAIN_NUM_MAX	4
#define HT_CHAIN_BUSN_D		0x40
#define HT_CHAIN_IOBASE_D	0x4000

#endif

static int mcp55_early_setup_x(void)
{
	/* Find out how many MCP55 we have. */
	unsigned busn[HT_CHAIN_NUM_MAX] = {0};
	unsigned devn[HT_CHAIN_NUM_MAX] = {0};
	unsigned io_base[HT_CHAIN_NUM_MAX] = {0};

	/*
	 * FIXME: May have problem if there is different MCP55 HTX card with
	 * different PCI_E lane allocation. Need to use same trick about
	 * pci1234 to verify node/link connection.
	 */
	unsigned pci_e_x[HT_CHAIN_NUM_MAX] = {
		CONFIG_MCP55_PCI_E_X_0, CONFIG_MCP55_PCI_E_X_1,
		CONFIG_MCP55_PCI_E_X_2, CONFIG_MCP55_PCI_E_X_3,
	};
	int mcp55_num = 0, ht_c_index;
	unsigned busnx, devnx;

	/* FIXME: Multi PCI segment handling. */

	/* Any system that only have IO55 without MCP55? */
	for (ht_c_index = 0; ht_c_index < HT_CHAIN_NUM_MAX; ht_c_index++) {
		busnx = ht_c_index * HT_CHAIN_BUSN_D;
		for (devnx = 0; devnx < 0x20; devnx++) {
			u32 id;
			pci_devfn_t dev;
			dev = PCI_DEV(busnx, devnx, 0);
			id = pci_read_config32(dev, PCI_VENDOR_ID);
			if (id == 0x036910de) {
				busn[mcp55_num] = busnx;
				devn[mcp55_num] = devnx;

				/* We may have HT chain other than MCP55. */
				io_base[mcp55_num]
					= ht_c_index * HT_CHAIN_IOBASE_D;

				mcp55_num++;
				if (mcp55_num == CONFIG_MCP55_NUM)
					goto out;
				break; /* Only one MCP55 on one chain. */
			}
		}
	}

out:
	printk(BIOS_DEBUG, "mcp55_num: %02x\n", mcp55_num);

	mcp55_early_set_port(mcp55_num, busn, devn, io_base);
	mcp55_early_setup(mcp55_num, busn, devn, io_base, pci_e_x);

	mcp55_early_clear_port(mcp55_num, busn, devn, io_base);

	return 0;
}
