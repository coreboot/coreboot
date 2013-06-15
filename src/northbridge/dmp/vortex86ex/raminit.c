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

#define DDRII_CTL_REG_tRAS   0x12
#define DDRII_CTL_REG_tRRD   0x6
#define DDRII_CTL_REG_tFAW   0x14
#define DDRII_CTL_REG_tRTP   0x3
#define DDRII_CTL_REG_tWTR   0x4
#define DDRII_CTL_REG_tRC    0x15
#define DDRII_CTL_REG_DQSnEN 0x0

#define DDR_CTRL_VAL \
	(DDRII_CTL_REG_tRAS << 20) | \
	(DDRII_CTL_REG_tRRD << 17) | \
	(DDRII_CTL_REG_tFAW << 12) | \
	(DDRII_CTL_REG_tRTP << 9) | \
	(DDRII_CTL_REG_tWTR << 6) | \
	(DDRII_CTL_REG_tRC << 1) | \
	(DDRII_CTL_REG_DQSnEN)

#define DDRII_PHY_CTL_1 0x000186c3
#define DDRII_PHY_CTL_2 0x00060000

static void config_pci_northbridge_f1(void)
{
	u16 i;
	// DDRII PHY Control 2
	pci_write_config32(NB1, 0xf8, DDRII_PHY_CTL_2);
	// DDRII PHY Control 1
	pci_write_config32(NB1, 0xf4, DDRII_PHY_CTL_1);
	// wait PHY lock.
	post_code(0x09);
	while ((pci_read_config32(NB1, 0xf8) & (1 << 21)) == 0) {
	}
	post_code(0x0a);
	// wait PHY stable.
	for (i = 0; i < 0x300; i++) {
		inb(0x80);
	}
	// DDRII control register.
	pci_write_config32(NB1, 0xf0, DDR_CTRL_VAL);
}

#define MEM_CLK_PHASE_R 0x0e
#define MEM_CLK_PHASE_W 0x01

#define ODT_EN 0x0
#define OUTPUT_DRIVER_IMPEDANCE 0x0
#define ODT_CONTROL 0x0
#define ADDITIVE_LATENCY 0x0
#define DDR_CTL_CAS_LATENCY 0x5
#define DDR_TIMING_tWR 0x5
#define DDR_TIMING_tRFC 0x16
#define DDR_TIMING_tRP 0x5
#define DDR_TIMING_tRCD 0x5

#define DDR_TIMING_VAL \
	(ODT_EN << 27) | \
	(OUTPUT_DRIVER_IMPEDANCE << 25) | \
	(ODT_CONTROL << 23) | \
	(ADDITIVE_LATENCY << 20) | \
	(DDR_CTL_CAS_LATENCY << 17) | \
	(DDR_TIMING_tWR << 14) | \
	(DDR_TIMING_tRFC << 8) | \
	(DDR_TIMING_tRP << 4) | \
	(DDR_TIMING_tRCD)

static void config_pci_northbridge_f0(void)
{
	// Memory clock phase selecton register.
	pci_write_config8(NB, 0x66, MEM_CLK_PHASE_R << 4 | MEM_CLK_PHASE_W);
	// DDRII memory timing register.
	pci_write_config32(NB, 0x74, DDR_TIMING_VAL);
}

// memory bank register control:
// bit    :
// 2 - 0  : CAT : Column Address Type : 0 0 1 = 9bit
//                                    : 0 1 0 = 10bit
// 4 - 3  : BN  : Bank Number         : 1 0 = 4 bank
//                                    : 1 1 = 8 bank
// 7 - 5  : RAT : Row Address Type    : 0 1 0 = 13bit
//                                    : 0 1 1 = 14bit
//                                    : 1 0 0 : 15bit
// 11 - 8 : SS  : Size                : 0 1 0 0 =  32M
//                                    : 0 1 0 1 =  64M
//                                    : 0 1 1 0 = 128M
//                                    : 0 1 1 1 = 256M
//                                    : 1 0 0 0 = 512M
// 13     : C1M : CS#[1] Mask         : 0 = actived
//                                    : 1 = mask

#define COL_9BIT   1
#define COL_10BIT  2
#define BANK_2BIT  2
#define BANK_3BIT  3
#define ROW_13BIT  2
#define ROW_14BIT  3
#define ROW_15BIT  4
#define SIZE_32M   4
#define SIZE_64M   5
#define SIZE_128M  6
#define SIZE_256M  7
#define SIZE_512M  8
#define C1M_ACTIVE 0
#define C1M_MASK   1

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

static u16 set_mem_bank_reg_cat(u16 reg, u16 cat)
{
	return set_bitfield(reg, cat, 3, 0);
}

static u16 get_mem_bank_reg_cat(u16 reg)
{
	return get_bitfield(reg, 3, 0);
}

static u16 set_mem_bank_reg_bn(u16 reg, u16 bn)
{
	return set_bitfield(reg, bn, 2, 3);
}

static u16 get_mem_bank_reg_bn(u16 reg)
{
	return get_bitfield(reg, 2, 3);
}

static u16 set_mem_bank_reg_rat(u16 reg, u16 rat)
{
	return set_bitfield(reg, rat, 3, 5);
}

static u16 get_mem_bank_reg_rat(u16 reg)
{
	return get_bitfield(reg, 3, 5);
}

static u16 set_mem_bank_reg_ss(u16 reg, u16 ss)
{
	return set_bitfield(reg, ss, 4, 8);
}

static u16 get_mem_bank_reg_ss(u16 reg)
{
	return get_bitfield(reg, 4, 8);
}

static u16 set_mem_bank_reg_c1m(u16 reg, u16 c1m)
{
	return set_bitfield(reg, c1m, 1, 13);
}

static u16 get_mem_bank_reg_c1m(u16 reg)
{
	return get_bitfield(reg, 1, 13);
}

static u16 auto_set_mem_bank_reg_ss(u16 reg)
{
	u8 ss = 0;
	// If reg is the minimum DRAM size,
	// SS is also the minimum size 32M.
	// If size in reg is bigger, SS is also bigger.
	ss += get_mem_bank_reg_cat(reg) - 1;
	ss += get_mem_bank_reg_bn(reg) - 2;
	ss += get_mem_bank_reg_rat(reg) - 2;
	ss += (1 - get_mem_bank_reg_c1m(reg));
	ss += SIZE_32M;
	return set_mem_bank_reg_ss(reg, ss);
}

static u16 get_mem_bank_reg(u16 cat, u16 bn, u16 rat, u16 c1m)
{
	u16 reg;
	reg = 0;
	reg = set_mem_bank_reg_cat(reg, cat);
	reg = set_mem_bank_reg_bn(reg, bn);
	reg = set_mem_bank_reg_rat(reg, rat);
	reg = set_mem_bank_reg_c1m(reg, c1m);
	reg = auto_set_mem_bank_reg_ss(reg);
	return reg;
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

static void detect_dram_size_failed(void)
{
	post_code(0x77);
	while (1) ;
}

static void detect_dram_cs(u16 br, u8 base_addr_bit)
{
	br = set_mem_bank_reg_c1m(br, C1M_ACTIVE);
	br = auto_set_mem_bank_reg_ss(br);
	pci_write_config16(NB, NB_REG_MBR, br);
	if (check_dram_side(base_addr_bit + 1)) {
		base_addr_bit += 1;
		return;
	}

	br = set_mem_bank_reg_c1m(br, C1M_MASK);
	br = auto_set_mem_bank_reg_ss(br);
	pci_write_config16(NB, NB_REG_MBR, br);
	// no need to check CS = 0.
}

static void detect_dram_row(u16 br, u8 base_addr_bit, u8 row_from_14)
{
	if (row_from_14)
		goto row_14;

	br = set_mem_bank_reg_rat(br, ROW_15BIT);
	br = auto_set_mem_bank_reg_ss(br);
	pci_write_config16(NB, NB_REG_MBR, br);
	if (check_address_bit(base_addr_bit + 15)) {
		base_addr_bit += 15;
		detect_dram_cs(br, base_addr_bit);
		return;
	}

row_14:
	br = set_mem_bank_reg_rat(br, ROW_14BIT);
	br = auto_set_mem_bank_reg_ss(br);
	pci_write_config16(NB, NB_REG_MBR, br);
	if (check_address_bit(base_addr_bit + 14)) {
		base_addr_bit += 14;
		detect_dram_cs(br, base_addr_bit);
		return;
	}

	br = set_mem_bank_reg_rat(br, ROW_13BIT);
	br = auto_set_mem_bank_reg_ss(br);
	pci_write_config16(NB, NB_REG_MBR, br);
	if (check_address_bit(base_addr_bit + 13)) {
		base_addr_bit += 13;
		detect_dram_cs(br, base_addr_bit);
		return;
	}
	// row test error.
	detect_dram_size_failed();
}

static void detect_dram_bank(u16 br, u8 base_addr_bit, u8 row_from_14)
{
	br = set_mem_bank_reg_bn(br, BANK_3BIT);
	br = auto_set_mem_bank_reg_ss(br);
	pci_write_config16(NB, NB_REG_MBR, br);
	if (check_address_bit(base_addr_bit + 3)) {
		base_addr_bit += 3;
		detect_dram_row(br, base_addr_bit, row_from_14);
		return;
	}

	br = set_mem_bank_reg_bn(br, BANK_2BIT);
	br = auto_set_mem_bank_reg_ss(br);
	pci_write_config16(NB, NB_REG_MBR, br);
	if (check_address_bit(base_addr_bit + 2)) {
		base_addr_bit += 2;
		detect_dram_row(br, base_addr_bit, row_from_14);
		return;
	}
	// bank test error.
	detect_dram_size_failed();
}

static void detect_dram_col(u16 br, u8 base_addr_bit, u8 row_from_14)
{
	br = set_mem_bank_reg_cat(br, COL_10BIT);
	br = auto_set_mem_bank_reg_ss(br);
	pci_write_config16(NB, NB_REG_MBR, br);
	if (check_address_bit(base_addr_bit + 10)) {
		base_addr_bit += 10;
		detect_dram_bank(br, base_addr_bit, row_from_14);
		return;
	}

	br = set_mem_bank_reg_cat(br, COL_9BIT);
	br = auto_set_mem_bank_reg_ss(br);
	pci_write_config16(NB, NB_REG_MBR, br);
	if (check_address_bit(base_addr_bit + 9)) {
		base_addr_bit += 9;
		detect_dram_bank(br, base_addr_bit, row_from_14);
		return;
	}
	// col test error.
	detect_dram_size_failed();
}

static void detect_dram_size(void)
{
	u16 br;
	u8 base_addr_bit = 0;
	br = get_mem_bank_reg(COL_10BIT, BANK_2BIT, ROW_13BIT, C1M_MASK);
	// check DX CPU model.
	if (pci_read_config8(NB, 0x8) == 0x01 && pci_read_config8(SB, 0x8) == 0x01) {
		// for oldest DX CPU, DRAM max row is from 14, not 15.
		detect_dram_col(br, base_addr_bit, 1);
	} else {
		// for newer DX CPU, DRAM max row is from 15.
		detect_dram_col(br, base_addr_bit, 0);
	}
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
	pci_write_config8(NB1, 0xfa, 0x40);
	while ((pci_read_config8(NB1, 0xfa) & 0x40) == 0x40) {
	}
	// reload mode.
	pci_write_config32(NB, 0x74, pci_read_config32(NB, 0x74));
}

static void detect_ddr3_dram_cs(u16 reg, u8 base_addr_bit)
{
	reg = set_ddr3_mem_reg_c1m(reg, DDR3_C1M_ACTIVE);
	reg = auto_set_ddr3_mem_reg_size(reg);
	pci_write_config16(NB, NB_REG_MBR, reg);
	if (check_dram_side(base_addr_bit + 1)) {
		base_addr_bit += 1;
		return;
	}

	reg = set_ddr3_mem_reg_c1m(reg, DDR3_C1M_MASK);
	reg = auto_set_ddr3_mem_reg_size(reg);
	pci_write_config16(NB, NB_REG_MBR, reg);
	// no need to check CS = 0.
	// Need to reset DDR3 PHY.
	ddr3_phy_reset();
}

static void detect_ddr3_dram_row(u16 reg, u8 base_addr_bit)
{
	reg = set_ddr3_mem_reg_row(reg, DDR3_ROW_16BIT);
	reg = auto_set_ddr3_mem_reg_size(reg);
	pci_write_config16(NB, NB_REG_MBR, reg);
	if (check_address_bit(base_addr_bit + 16)) {
		base_addr_bit += 16;
		detect_ddr3_dram_cs(reg, base_addr_bit);
		return;
	}

	reg = set_ddr3_mem_reg_row(reg, DDR3_ROW_15BIT);
	reg = auto_set_ddr3_mem_reg_size(reg);
	pci_write_config16(NB, NB_REG_MBR, reg);
	if (check_address_bit(base_addr_bit + 15)) {
		base_addr_bit += 15;
		detect_ddr3_dram_cs(reg, base_addr_bit);
		return;
	}

	reg = set_ddr3_mem_reg_row(reg, DDR3_ROW_14BIT);
	reg = auto_set_ddr3_mem_reg_size(reg);
	pci_write_config16(NB, NB_REG_MBR, reg);
	if (check_address_bit(base_addr_bit + 14)) {
		base_addr_bit += 14;
		detect_ddr3_dram_cs(reg, base_addr_bit);
		return;
	}

	reg = set_ddr3_mem_reg_row(reg, DDR3_ROW_13BIT);
	reg = auto_set_ddr3_mem_reg_size(reg);
	pci_write_config16(NB, NB_REG_MBR, reg);
	if (check_address_bit(base_addr_bit + 13)) {
		base_addr_bit += 13;
		detect_ddr3_dram_cs(reg, base_addr_bit);
		return;
	}
	// row test error.
	detect_dram_size_failed();
}

static void detect_ddr3_dram_bank(u16 reg, u8 base_addr_bit)
{
	/* DDR3 is always 3 bank bits */
	base_addr_bit += 3;
	detect_ddr3_dram_row(reg, base_addr_bit);
}

static void detect_ddr3_dram_col(u16 reg, u8 base_addr_bit)
{
	reg = set_ddr3_mem_reg_col(reg, DDR3_COL_11BIT);
	reg = auto_set_ddr3_mem_reg_size(reg);
	pci_write_config16(NB, NB_REG_MBR, reg);
	if (check_address_bit(base_addr_bit + 11)) {
		base_addr_bit += 11;
		detect_ddr3_dram_bank(reg, base_addr_bit);
		return;
	}

	reg = set_ddr3_mem_reg_col(reg, DDR3_COL_10BIT);
	reg = auto_set_ddr3_mem_reg_size(reg);
	pci_write_config16(NB, NB_REG_MBR, reg);
	if (check_address_bit(base_addr_bit + 10)) {
		base_addr_bit += 10;
		detect_ddr3_dram_bank(reg, base_addr_bit);
		return;
	}
	// col test error.
	detect_dram_size_failed();
}

static void detect_ddr3_dram_size(void)
{
	u16 reg;
	u8 base_addr_bit = 0;
	reg = get_ddr3_mem_reg(DDR3_COL_10BIT, DDR3_ROW_13BIT, DDR3_C1M_MASK);
	detect_ddr3_dram_col(reg, base_addr_bit);
}
