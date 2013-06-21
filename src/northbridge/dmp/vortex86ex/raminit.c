/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 DMP Electronics Inc.
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

static u16 get_mask(u16 bit_width, u16 bit_offset)
{
	u16 mask = (((1 << bit_width) - 1) << bit_offset);
	return mask;
}

static u16 set_bitfield(u16 val, u16 bits, u16 bit_width, u16 bit_offset)
{
	u16 mask = get_mask(bit_width, bit_offset);
	val = (val & ~mask) | (bits << bit_offset);
	return val;
}

static u16 get_bitfield(u16 val, u16 bit_width, u16 bit_offset)
{
	u16 mask = get_mask(bit_width, bit_offset);
	return (val & mask) >> bit_offset;
}

static u8 check_address_bit(int addr_bit)
{
	u16 dummy;
	*(volatile u16 *)(0) = 0;
	dummy = *(volatile u16 *)(0);	// read push write
	*(volatile u16 *)(1 << addr_bit) = 0x5a5a;
	dummy = *(volatile u16 *)(1 << addr_bit);	// read push write
	if ((*(volatile u16 *)(0)) != 0)
		return 0;	// address bit wrapped.
	return 1;		// address bit not wrapped.
}

static u8 check_dram_side(int addr_bit)
{
	*(volatile u16 *)(1 << addr_bit) = 0x5a5a;
	*(volatile u16 *)(0) = 0;
	if ((*(volatile u16 *)(1 << addr_bit)) != 0x5a5a)
		return 0;	// DRAM only one side.
	return 1;		// two sides.
}

// DDRIII memory bank register control:
// bit    :
// 2 - 0  : DRAMC_COLSIZE : DDRIII Column Address Type : 0 0 0 = 10bit
//                                                     : 0 0 1 = 11bit
// 7 - 5  : DRAMC_ROWSIZE : DDRIII Row Address Type    : 0 0 0 = 13bit
//                                                     : 0 0 1 = 14bit
//                                                     : 0 1 0 = 15bit
//                                                     : 0 1 1 = 16bit
// 11 - 8 : DRAM_SIZE     : DDRIII Size                : 0 1 0 1 =  64M
//                                                     : 0 1 1 0 = 128M
//                                                     : 0 1 1 1 = 256M
//                                                     : 1 0 0 0 = 512M
//                                                     : 1 0 0 1 = 1GB
//                                                     : 1 0 1 0 = 2GB
// 13     : DRAMC_CSMASK  : DDRIII CS#[1] Mask         : 1 = Mask CS1 enable

#define DDR3_COL_10BIT  0
#define DDR3_COL_11BIT  1
#define DDR3_ROW_13BIT  0
#define DDR3_ROW_14BIT  1
#define DDR3_ROW_15BIT  2
#define DDR3_ROW_16BIT  3
#define DDR3_SIZE_64M   5
#define DDR3_SIZE_128M  6
#define DDR3_SIZE_256M  7
#define DDR3_SIZE_512M  8
#define DDR3_SIZE_1GB   9
#define DDR3_SIZE_2GB   10
#define DDR3_C1M_ACTIVE 0
#define DDR3_C1M_MASK   1

static u16 set_ddr3_mem_reg_col(u16 reg, u16 col)
{
	return set_bitfield(reg, col, 3, 0);
}

static u16 get_ddr3_mem_reg_col(u16 reg)
{
	return get_bitfield(reg, 3, 0);
}

static u16 set_ddr3_mem_reg_row(u16 reg, u16 row)
{
	return set_bitfield(reg, row, 3, 5);
}

static u16 get_ddr3_mem_reg_row(u16 reg)
{
	return get_bitfield(reg, 3, 5);
}

static u16 set_ddr3_mem_reg_size(u16 reg, u16 size)
{
	return set_bitfield(reg, size, 4, 8);
}

static u16 get_ddr3_mem_reg_size(u16 reg)
{
	return get_bitfield(reg, 4, 8);
}

static u16 set_ddr3_mem_reg_c1m(u16 reg, u16 c1m)
{
	return set_bitfield(reg, c1m, 1, 13);
}

static u16 get_ddr3_mem_reg_c1m(u16 reg)
{
	return get_bitfield(reg, 1, 13);
}

static u16 auto_set_ddr3_mem_reg_size(u16 reg)
{
	u8 ss = 0;
	// If reg is the minimum DRAM size,
	// SS is also the minimum size 128M.
	// If size in reg is bigger, SS is also bigger.
	ss += get_ddr3_mem_reg_col(reg);
	ss += get_ddr3_mem_reg_row(reg);
	ss += (1 - get_ddr3_mem_reg_c1m(reg));
	ss += DDR3_SIZE_128M;
	return set_ddr3_mem_reg_size(reg, ss);
}

static u16 get_ddr3_mem_reg(u16 col, u16 row, u16 c1m)
{
	u16 reg;
	reg = 0;
	reg = set_ddr3_mem_reg_col(reg, col);
	reg = set_ddr3_mem_reg_row(reg, row);
	reg = set_ddr3_mem_reg_c1m(reg, c1m);
	reg = auto_set_ddr3_mem_reg_size(reg);
	return reg;
}

static void ddr3_phy_reset(void)
{
	// PCI N/B reg FAh bit 6 = RST_DRAM_PHY.
	pci_write_config8(NB1, NB1_REG_RESET_DRAMC_PHY, 0x40);
	while ((pci_read_config8(NB1, NB1_REG_RESET_DRAMC_PHY) & 0x40) == 0x40) {
	}
	// reload mode.
	u32 ddr3_cfg = pci_read_config32(NB, NB_REG_DDR3_CFG);
	pci_write_config32(NB, NB_REG_DDR3_CFG, ddr3_cfg);
}

static u8 detect_ddr3_dram_cs(u16 reg, u8 base_addr_bit)
{
	reg = set_ddr3_mem_reg_c1m(reg, DDR3_C1M_ACTIVE);
	reg = auto_set_ddr3_mem_reg_size(reg);
	pci_write_config16(NB, NB_REG_MBR, reg);
	if (check_dram_side(base_addr_bit + 1)) {
		base_addr_bit += 1;
		return 0;
	}

	reg = set_ddr3_mem_reg_c1m(reg, DDR3_C1M_MASK);
	reg = auto_set_ddr3_mem_reg_size(reg);
	pci_write_config16(NB, NB_REG_MBR, reg);
	// no need to check CS = 0.
	// Need to reset DDR3 PHY.
	ddr3_phy_reset();
	return 0;
}

static u8 detect_ddr3_dram_row(u16 reg, u8 base_addr_bit)
{
	reg = set_ddr3_mem_reg_row(reg, DDR3_ROW_16BIT);
	reg = auto_set_ddr3_mem_reg_size(reg);
	pci_write_config16(NB, NB_REG_MBR, reg);
	if (check_address_bit(base_addr_bit + 16)) {
		base_addr_bit += 16;
		return detect_ddr3_dram_cs(reg, base_addr_bit);
	}

	reg = set_ddr3_mem_reg_row(reg, DDR3_ROW_15BIT);
	reg = auto_set_ddr3_mem_reg_size(reg);
	pci_write_config16(NB, NB_REG_MBR, reg);
	if (check_address_bit(base_addr_bit + 15)) {
		base_addr_bit += 15;
		return detect_ddr3_dram_cs(reg, base_addr_bit);
	}

	reg = set_ddr3_mem_reg_row(reg, DDR3_ROW_14BIT);
	reg = auto_set_ddr3_mem_reg_size(reg);
	pci_write_config16(NB, NB_REG_MBR, reg);
	if (check_address_bit(base_addr_bit + 14)) {
		base_addr_bit += 14;
		return detect_ddr3_dram_cs(reg, base_addr_bit);
	}

	reg = set_ddr3_mem_reg_row(reg, DDR3_ROW_13BIT);
	reg = auto_set_ddr3_mem_reg_size(reg);
	pci_write_config16(NB, NB_REG_MBR, reg);
	if (check_address_bit(base_addr_bit + 13)) {
		base_addr_bit += 13;
		return detect_ddr3_dram_cs(reg, base_addr_bit);
	}
	// row test error.
	return 1;
}

static u8 detect_ddr3_dram_bank(u16 reg, u8 base_addr_bit)
{
	/* DDR3 is always 3 bank bits */
	base_addr_bit += 3;
	return detect_ddr3_dram_row(reg, base_addr_bit);
}

static u8 detect_ddr3_dram_col(u16 reg, u8 base_addr_bit)
{
	reg = set_ddr3_mem_reg_col(reg, DDR3_COL_11BIT);
	reg = auto_set_ddr3_mem_reg_size(reg);
	pci_write_config16(NB, NB_REG_MBR, reg);
	if (check_address_bit(base_addr_bit + 11)) {
		base_addr_bit += 11;
		return detect_ddr3_dram_bank(reg, base_addr_bit);
	}

	reg = set_ddr3_mem_reg_col(reg, DDR3_COL_10BIT);
	reg = auto_set_ddr3_mem_reg_size(reg);
	pci_write_config16(NB, NB_REG_MBR, reg);
	if (check_address_bit(base_addr_bit + 10)) {
		base_addr_bit += 10;
		return detect_ddr3_dram_bank(reg, base_addr_bit);
	}
	// col test error.
	return 1;
}

static u8 detect_ddr3_dram_size(void)
{
	u16 reg;
	u8 base_addr_bit = 0;
	reg = get_ddr3_mem_reg(DDR3_COL_10BIT, DDR3_ROW_13BIT, DDR3_C1M_MASK);
	return detect_ddr3_dram_col(reg, base_addr_bit);
}

static void print_ddr3_memory_setup(void)
{
#if CONFIG_DEBUG_RAM_SETUP
	print_debug("DDR3 Timing Reg 0-3:\n");
	print_debug("NB 6e : ");
	print_debug_hex16(pci_read_config16(NB, 0x6e));
	print_debug("\nNB 74 : ");
	print_debug_hex32(pci_read_config32(NB, 0x74));
	print_debug("\nNB 78 : ");
	print_debug_hex32(pci_read_config32(NB, 0x78));
	print_debug("\nNB 7c : ");
	print_debug_hex32(pci_read_config32(NB, 0x7c));
	u16 mbr = pci_read_config16(NB, 0x6c);
	print_debug("\nNB 6c(MBR) : ");
	print_debug_hex16(mbr);
	const char *s;
	u8 col = get_ddr3_mem_reg_col(mbr);
	if (col == DDR3_COL_10BIT)
		s = " (COL=10";
	else
		s = " (COL=11";
	print_debug(s);
	u8 row = get_ddr3_mem_reg_row(mbr);
	switch (row) {
	case DDR3_ROW_13BIT:
		s = ", ROW = 13";
		break;
	case DDR3_ROW_14BIT:
		s = ", ROW = 14";
		break;
	case DDR3_ROW_15BIT:
		s = ", ROW = 15";
		break;
	default:
		s = ", ROW = 16";
		break;
	}
	print_debug(s);
	u8 size = get_ddr3_mem_reg_size(mbr);
	switch (size) {
	case DDR3_SIZE_64M:
		s = ", 64M";
		break;
	case DDR3_SIZE_128M:
		s = ", 128M";
		break;
	case DDR3_SIZE_256M:
		s = ", 256M";
		break;
	case DDR3_SIZE_512M:
		s = ", 512M";
		break;
	case DDR3_SIZE_1GB:
		s = ", 1GB";
		break;
	case DDR3_SIZE_2GB:
		s = ", 2GB";
		break;
	}
	print_debug(s);
	u8 mask = get_ddr3_mem_reg_c1m(mbr);
	if (mask == DDR3_C1M_ACTIVE)
		s = ", CS MASK Enable)\n";
	else
		s = ", CS Mask Disable)\n";
	print_debug(s);
#endif
}
