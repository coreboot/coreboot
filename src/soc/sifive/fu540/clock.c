/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Philipp Hug <philipp@hug.cx>
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
#include <console/console.h>
#include <soc/clock.h>
#include <soc/addressmap.h>
#include <stdlib.h>
#include <stdint.h>

struct prci_ctlr {
	u32 hfxosccfg;		/* offset 0x00 */
	u32 corepllcfg0;	/* offset 0x04 */
	u32 reserved08;		/* offset 0x08 */
	u32 ddrpllcfg0;		/* offset 0x0c */
	u32 ddrpllcfg1;		/* offset 0x10 */
	u32 reserved14;		/* offset 0x14 */
	u32 reserved18;		/* offset 0x18 */
	u32 gemgxlpllcfg0;	/* offset 0x1c */
	u32 gemgxlpllcfg1;	/* offset 0x20 */
	u32 coreclksel;		/* offset 0x24 */
	u32 devicesresetreg;	/* offset 0x28 */
};

static struct prci_ctlr *prci = (void *)FU540_PRCI;

#define PRCI_CORECLK_MASK 1
#define PRCI_CORECLK_CORE_PLL 0
#define PRCI_CORECLK_HFCLK 1

#define PRCI_COREPLLCFG0_LOCK (1u << 31)
#define PRCI_COREPLLCFG0_DIVR_SHIFT 0
#define PRCI_COREPLLCFG0_DIVF_SHIFT 6
#define PRCI_COREPLLCFG0_DIVQ_SHIFT 15
#define PRCI_COREPLLCFG0_RANGE_SHIFT 18
#define PRCI_COREPLLCFG0_BYPASS_SHIFT 24
#define PRCI_COREPLLCFG0_FSE_SHIFT 25
#define PRCI_COREPLLCFG0_DIVR_MASK (0x03f << PRCI_COREPLLCFG0_DIVR_SHIFT)
#define PRCI_COREPLLCFG0_DIVF_MASK (0x1ff << PRCI_COREPLLCFG0_DIVF_SHIFT)
#define PRCI_COREPLLCFG0_DIVQ_MASK (0x007 << PRCI_COREPLLCFG0_DIVQ_SHIFT)
#define PRCI_COREPLLCFG0_RANGE_MASK (0x07 << PRCI_COREPLLCFG0_RANGE_SHIFT)
#define PRCI_COREPLLCFG0_BYPASS_MASK (0x1 << PRCI_COREPLLCFG0_BYPASS_SHIFT)
#define PRCI_COREPLLCFG0_FSE_MASK (0x1 << PRCI_COREPLLCFG0_FSE_SHIFT)

#define PRCI_DDRPLLCFG0_LOCK (1u << 31)
#define PRCI_DDRPLLCFG0_DIVR_SHIFT 0
#define PRCI_DDRPLLCFG0_DIVF_SHIFT 6
#define PRCI_DDRPLLCFG0_DIVQ_SHIFT 15
#define PRCI_DDRPLLCFG0_RANGE_SHIFT 18
#define PRCI_DDRPLLCFG0_BYPASS_SHIFT 24
#define PRCI_DDRPLLCFG0_FSE_SHIFT 25
#define PRCI_DDRPLLCFG0_DIVR_MASK (0x03f << PRCI_DDRPLLCFG0_DIVR_SHIFT)
#define PRCI_DDRPLLCFG0_DIVF_MASK (0x1ff << PRCI_DDRPLLCFG0_DIVF_SHIFT)
#define PRCI_DDRPLLCFG0_DIVQ_MASK (0x007 << PRCI_DDRPLLCFG0_DIVQ_SHIFT)
#define PRCI_DDRPLLCFG0_RANGE_MASK (0x07 << PRCI_DDRPLLCFG0_RANGE_SHIFT)
#define PRCI_DDRPLLCFG0_BYPASS_MASK (0x1 << PRCI_DDRPLLCFG0_BYPASS_SHIFT)
#define PRCI_DDRPLLCFG0_FSE_MASK (0x1 << PRCI_DDRPLLCFG0_FSE_SHIFT)

#define PRCI_DDRPLLCFG1_MASK (1u << 31)

#define PRCI_CORECLKSEL_CORECLKSEL 1

#define PRCI_DEVICESRESET_DDR_CTRL_RST_N(x) (((x) & 0x1)  << 0)
#define PRCI_DEVICESRESET_DDR_AXI_RST_N(x)  (((x) & 0x1)  << 1)
#define PRCI_DEVICESRESET_DDR_AHB_RST_N(x)  (((x) & 0x1)  << 2)
#define PRCI_DEVICESRESET_DDR_PHY_RST_N(x)  (((x) & 0x1)  << 3)
#define PRCI_DEVICESRESET_GEMGXL_RST_N(x)   (((x) & 0x1)  << 5)

/*
 * Set coreclk according to the SiFive FU540-C000 Manual
 * https://www.sifive.com/documentation/chips/freedom-u540-c000-manual/
 *
 * Section 7.1 recommends a frequency of 1.0 GHz (up to 1.5 Ghz is possible)
 *
 * Section 7.4.2 provides the necessary values:
 * For example, to setup COREPLL for 1 GHz operation, program divr = 0 (x1),
 * divf = 59 (4000 MHz VCO), divq = 2 (/4 Output divider)
 */

#define PRCI_CORECLK_DIVR 0
#define PRCI_CORECLK_DIVF 59
#define PRCI_CORECLK_DIVQ 2
#define PRCI_CORECLK_RANGE 4
#define PRCI_CORECLK_BYPASS 0
#define PRCI_CORECLK_FSE 1

/*
 * Section 7.4.3: DDR and Ethernet Subsystem Clocking and Reset
 *
 * Unfortunately the documentation example doesn't match the HiFive
 * Unleashed board settings.
 * Configuration values taken from SiFive FSBL:
 * https://github.com/sifive/freedom-u540-c000-bootloader/blob/master/fsbl/main.c
 *
 * DDRPLL is set up for 933 MHz output frequency.
 * divr = 0, divf = 55 (3730 MHz VCO), divq = 2
 *
 * GEMGXLPLL is set up for 125 MHz output frequency.
 * divr = 0, divf = 59 (4000 MHz VCO), divq = 5
 */

#define PRCI_DDRCLK_DIVR 0
#define PRCI_DDRCLK_DIVF 55
#define PRCI_DDRCLK_DIVQ 2
#define PRCI_DDRCLK_RANGE 4
#define PRCI_DDRCLK_BYPASS 0
#define PRCI_DDRCLK_FSE 1

// 33.33 Mhz after reset
#define FU540_BASE_FQY 33330

#if ENV_ROMSTAGE

static void init_coreclk(void)
{
	// switch coreclk to input reference frequency before modifying PLL
	clrsetbits_le32(&prci->coreclksel, PRCI_CORECLK_MASK,
		PRCI_CORECLK_HFCLK);

	u32 c = read32(&prci->corepllcfg0);
	clrsetbits_le32(&c, PRCI_COREPLLCFG0_DIVR_MASK
		| PRCI_COREPLLCFG0_DIVF_MASK | PRCI_COREPLLCFG0_DIVQ_MASK
		| PRCI_COREPLLCFG0_RANGE_MASK | PRCI_COREPLLCFG0_BYPASS_MASK
		| PRCI_COREPLLCFG0_FSE_MASK,
		(PRCI_CORECLK_DIVR << PRCI_COREPLLCFG0_DIVR_SHIFT)
		| (PRCI_CORECLK_DIVF << PRCI_COREPLLCFG0_DIVF_SHIFT)
		| (PRCI_CORECLK_DIVQ << PRCI_COREPLLCFG0_DIVQ_SHIFT)
		| (PRCI_CORECLK_RANGE << PRCI_COREPLLCFG0_RANGE_SHIFT)
		| (PRCI_CORECLK_BYPASS << PRCI_COREPLLCFG0_BYPASS_SHIFT)
		| (PRCI_CORECLK_FSE << PRCI_COREPLLCFG0_FSE_SHIFT));
	write32(&prci->corepllcfg0, c);

	// wait for PLL lock
	while (!(read32(&prci->corepllcfg0) & PRCI_COREPLLCFG0_LOCK))
		; /* TODO: implement a timeout */

	// switch coreclk to use corepll
	clrsetbits_le32(&prci->coreclksel, PRCI_CORECLK_MASK,
		PRCI_CORECLK_CORE_PLL);
}

static void init_pll_ddr(void)
{
	// disable ddr clock output before reconfiguring the PLL
	u32 cfg1 = read32(&prci->ddrpllcfg1);
	clrbits_le32(&cfg1, PRCI_DDRPLLCFG1_MASK);
	write32(&prci->ddrpllcfg1, cfg1);

	u32 c = read32(&prci->ddrpllcfg0);
	clrsetbits_le32(&c, PRCI_DDRPLLCFG0_DIVR_MASK
		| PRCI_DDRPLLCFG0_DIVF_MASK | PRCI_DDRPLLCFG0_DIVQ_MASK
		| PRCI_DDRPLLCFG0_RANGE_MASK | PRCI_DDRPLLCFG0_BYPASS_MASK
		| PRCI_DDRPLLCFG0_FSE_MASK,
		(PRCI_DDRCLK_DIVR << PRCI_DDRPLLCFG0_DIVR_SHIFT)
		| (PRCI_DDRCLK_DIVF << PRCI_DDRPLLCFG0_DIVF_SHIFT)
		| (PRCI_DDRCLK_DIVQ << PRCI_DDRPLLCFG0_DIVQ_SHIFT)
		| (PRCI_DDRCLK_RANGE << PRCI_DDRPLLCFG0_RANGE_SHIFT)
		| (PRCI_DDRCLK_BYPASS << PRCI_DDRPLLCFG0_BYPASS_SHIFT)
		| (PRCI_DDRCLK_FSE << PRCI_DDRPLLCFG0_FSE_SHIFT));
	write32(&prci->ddrpllcfg0, c);

	// wait for PLL lock
	while (!(read32(&prci->ddrpllcfg0) & PRCI_DDRPLLCFG0_LOCK))
		; /* TODO: implement a timeout */

	// enable ddr clock output
	setbits_le32(&cfg1, PRCI_DDRPLLCFG1_MASK);
	write32(&prci->ddrpllcfg1, cfg1);
}

#define FU540_UART_DEVICES 2
#define FU540_UART_REG_DIV 0x18
#define FU540_UART_DIV_VAL 4

#define FU540_SPI_DIV 0x00
#define FU540_SPI_DIV_VAL 4


static void update_peripheral_clock_dividers(void)
{
	write32((uint32_t *)(FU540_QSPI0 + FU540_SPI_DIV), FU540_SPI_DIV_VAL);
	write32((uint32_t *)(FU540_QSPI1 + FU540_SPI_DIV), FU540_SPI_DIV_VAL);
	write32((uint32_t *)(FU540_QSPI2 + FU540_SPI_DIV), FU540_SPI_DIV_VAL);

	for (size_t i = 0; i < FU540_UART_DEVICES; i++)
		write32((uint32_t *)(FU540_UART(i) + FU540_UART_REG_DIV), FU540_UART_DIV_VAL);
}

void clock_init(void)
{
	/*
	 * Update the peripheral clock dividers of UART, SPI and I2C to safe
	 * values as we can't put them in reset before changing frequency.
	 */
	update_peripheral_clock_dividers();

	init_coreclk();

	// put DDR and ethernet in reset
	write32(&prci->devicesresetreg, 0);

	init_pll_ddr();

	// The following code and its comments is mostly derived from the SiFive
	// u540 bootloader.
	// https://github.com/sifive/freedom-u540-c000-bootloader

	// get DDR out of reset
	write32(&prci->devicesresetreg, PRCI_DEVICESRESET_DDR_CTRL_RST_N(1));

	// HACK to get the '1 full controller clock cycle'.
	asm volatile ("fence");

	// get DDR out of reset
	write32(&prci->devicesresetreg,
		PRCI_DEVICESRESET_DDR_CTRL_RST_N(1) |
		PRCI_DEVICESRESET_DDR_AXI_RST_N(1) |
		PRCI_DEVICESRESET_DDR_AHB_RST_N(1) |
		PRCI_DEVICESRESET_DDR_PHY_RST_N(1));

	// HACK to get the '1 full controller clock cycle'.
	asm volatile ("fence");

	// These take like 16 cycles to actually propagate. We can't go sending
	// stuff before they come out of reset. So wait.
	// TODO: Add a register to read the current reset states, or DDR Control
	// device?
	for (int i = 0; i < 256; i++)
		asm volatile ("nop");
}

#endif

int clock_get_coreclk_khz(void)
{
	if (read32(&prci->coreclksel) & PRCI_CORECLK_MASK)
		return FU540_BASE_FQY;

	u32 cfg  = read32(&prci->corepllcfg0);
	u32 divr = (cfg & PRCI_COREPLLCFG0_DIVR_MASK)
		>> PRCI_COREPLLCFG0_DIVR_SHIFT;
	u32 divf = (cfg & PRCI_COREPLLCFG0_DIVF_MASK)
		>> PRCI_COREPLLCFG0_DIVF_SHIFT;
	u32 divq = (cfg & PRCI_COREPLLCFG0_DIVQ_MASK)
		>> PRCI_COREPLLCFG0_DIVQ_SHIFT;

	printk(BIOS_SPEW, "clk: r=%d f=%d q=%d\n", divr, divf, divq);
	return FU540_BASE_FQY
		* 2 * (divf + 1)
		/ (divr + 1)
		/ (1ul << divq);
}
