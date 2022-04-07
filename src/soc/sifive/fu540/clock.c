/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <console/console.h>
#include <soc/clock.h>
#include <soc/addressmap.h>
#include <stdint.h>

// 33.33 Mhz after reset
#define FU540_BASE_FQY 33330

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

#define PRCI_PLLCFG_LOCK (1u << 31)
#define PRCI_PLLCFG_DIVR_SHIFT 0
#define PRCI_PLLCFG_DIVF_SHIFT 6
#define PRCI_PLLCFG_DIVQ_SHIFT 15
#define PRCI_PLLCFG_RANGE_SHIFT 18
#define PRCI_PLLCFG_BYPASS_SHIFT 24
#define PRCI_PLLCFG_FSE_SHIFT 25
#define PRCI_PLLCFG_DIVR_MASK (0x03f << PRCI_PLLCFG_DIVR_SHIFT)
#define PRCI_PLLCFG_DIVF_MASK (0x1ff << PRCI_PLLCFG_DIVF_SHIFT)
#define PRCI_PLLCFG_DIVQ_MASK (0x007 << PRCI_PLLCFG_DIVQ_SHIFT)
#define PRCI_PLLCFG_RANGE_MASK (0x07 << PRCI_PLLCFG_RANGE_SHIFT)
#define PRCI_PLLCFG_BYPASS_MASK (0x1 << PRCI_PLLCFG_BYPASS_SHIFT)
#define PRCI_PLLCFG_FSE_MASK (0x1 << PRCI_PLLCFG_FSE_SHIFT)

#define PRCI_DDRPLLCFG1_MASK (1u << 31)

#define PRCI_GEMGXLPPLCFG1_MASK (1u << 31)

#define PRCI_CORECLKSEL_CORECLKSEL 1

#define PRCI_DEVICESRESET_DDR_CTRL_RST_N(x) (((x) & 0x1)  << 0)
#define PRCI_DEVICESRESET_DDR_AXI_RST_N(x)  (((x) & 0x1)  << 1)
#define PRCI_DEVICESRESET_DDR_AHB_RST_N(x)  (((x) & 0x1)  << 2)
#define PRCI_DEVICESRESET_DDR_PHY_RST_N(x)  (((x) & 0x1)  << 3)
#define PRCI_DEVICESRESET_GEMGXL_RST_N(x)   (((x) & 0x1)  << 5)

/* Clock initialization should only be done in romstage. */
#if ENV_RAMINIT
struct pll_settings {
	unsigned int divr:6;
	unsigned int divf:9;
	unsigned int divq:3;
	unsigned int range:3;
	unsigned int bypass:1;
	unsigned int fse:1;
};

static void configure_pll(u32 *reg, const struct pll_settings *s)
{
	// Write the settings to the register
	u32 c = read32(reg);
	clrsetbits32(&c, PRCI_PLLCFG_DIVR_MASK
		| PRCI_PLLCFG_DIVF_MASK | PRCI_PLLCFG_DIVQ_MASK
		| PRCI_PLLCFG_RANGE_MASK | PRCI_PLLCFG_BYPASS_MASK
		| PRCI_PLLCFG_FSE_MASK,
		(s->divr << PRCI_PLLCFG_DIVR_SHIFT)
		| (s->divf << PRCI_PLLCFG_DIVF_SHIFT)
		| (s->divq << PRCI_PLLCFG_DIVQ_SHIFT)
		| (s->range << PRCI_PLLCFG_RANGE_SHIFT)
		| (s->bypass << PRCI_PLLCFG_BYPASS_SHIFT)
		| (s->fse << PRCI_PLLCFG_FSE_SHIFT));
	write32(reg, c);

	// Wait for PLL lock
	while (!(read32(reg) & PRCI_PLLCFG_LOCK))
		; /* TODO: implement a timeout */
}

/*
 * Set coreclk according to the SiFive FU540-C000 Manual
 * https://www.sifive.com/documentation/chips/freedom-u540-c000-manual/
 *
 * Section 7.1 recommends a frequency of 1.0 GHz (up to 1.5 GHz is possible)
 *
 * Section 7.4.2 provides the necessary values:
 * For example, to setup COREPLL for 1 GHz operation, program divr = 0 (x1),
 * divf = 59 (4000 MHz VCO), divq = 2 (/4 Output divider)
 */
static const struct pll_settings corepll_settings = {
	.divr = 0,
	.divf = 59,
	.divq = 2,
	.range = 4,
	.bypass = 0,
	.fse = 1,
};

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
static const struct pll_settings ddrpll_settings = {
	.divr = 0,
	.divf = 55,
	.divq = 2,
	.range = 4,
	.bypass = 0,
	.fse = 1,
};

static const struct pll_settings gemgxlpll_settings = {
	.divr = 0,
	.divf = 59,
	.divq = 5,
	.range = 4,
	.bypass = 0,
	.fse = 1,
};

static void init_coreclk(void)
{
	// switch coreclk to input reference frequency before modifying PLL
	clrsetbits32(&prci->coreclksel, PRCI_CORECLK_MASK,
		PRCI_CORECLK_HFCLK);

	configure_pll(&prci->corepllcfg0, &corepll_settings);

	// switch coreclk to use corepll
	clrsetbits32(&prci->coreclksel, PRCI_CORECLK_MASK,
		PRCI_CORECLK_CORE_PLL);
}

static void init_pll_ddr(void)
{
	// disable ddr clock output before reconfiguring the PLL
	u32 cfg1 = read32(&prci->ddrpllcfg1);
	clrbits32(&cfg1, PRCI_DDRPLLCFG1_MASK);
	write32(&prci->ddrpllcfg1, cfg1);

	configure_pll(&prci->ddrpllcfg0, &ddrpll_settings);

	// enable ddr clock output
	setbits32(&cfg1, PRCI_DDRPLLCFG1_MASK);
	write32(&prci->ddrpllcfg1, cfg1);
}

static void init_gemgxlclk(void)
{
	u32 cfg1 = read32(&prci->gemgxlpllcfg1);
	clrbits32(&cfg1, PRCI_GEMGXLPPLCFG1_MASK);
	write32(&prci->gemgxlpllcfg1, cfg1);

	configure_pll(&prci->gemgxlpllcfg0, &gemgxlpll_settings);

	setbits32(&cfg1, PRCI_GEMGXLPPLCFG1_MASK);
	write32(&prci->gemgxlpllcfg1, cfg1);
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

	init_gemgxlclk();

	write32(&prci->devicesresetreg,
		PRCI_DEVICESRESET_DDR_CTRL_RST_N(1) |
		PRCI_DEVICESRESET_DDR_AXI_RST_N(1) |
		PRCI_DEVICESRESET_DDR_AHB_RST_N(1) |
		PRCI_DEVICESRESET_DDR_PHY_RST_N(1) |
		PRCI_DEVICESRESET_GEMGXL_RST_N(1));

	asm volatile ("fence");
}
#endif /* ENV_RAMINIT */

/* Get the core clock's frequency, in KHz */
int clock_get_coreclk_khz(void)
{
	if (read32(&prci->coreclksel) & PRCI_CORECLK_MASK)
		return FU540_BASE_FQY;

	u32 cfg  = read32(&prci->corepllcfg0);
	u32 divr = (cfg & PRCI_PLLCFG_DIVR_MASK)
		>> PRCI_PLLCFG_DIVR_SHIFT;
	u32 divf = (cfg & PRCI_PLLCFG_DIVF_MASK)
		>> PRCI_PLLCFG_DIVF_SHIFT;
	u32 divq = (cfg & PRCI_PLLCFG_DIVQ_MASK)
		>> PRCI_PLLCFG_DIVQ_SHIFT;

	printk(BIOS_SPEW, "clk: r=%d f=%d q=%d\n", divr, divf, divq);
	return FU540_BASE_FQY
		* 2 * (divf + 1)
		/ (divr + 1)
		/ (1ul << divq);
}

/* Get the TileLink clock's frequency, in KHz */
int clock_get_tlclk_khz(void)
{
	/*
	 * The TileLink bus and most peripherals use tlclk, which is coreclk/2,
	 * as input.
	 */

	return clock_get_coreclk_khz() / 2;
}
