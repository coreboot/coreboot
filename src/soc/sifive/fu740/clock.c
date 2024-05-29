/* SPDX-License-Identifier: GPL-2.0-only */

// This file is used for setting up clocks and get devices out of reset
// For more Information see FU740-C000 Manual Chapter 7 Clocking and Reset

#include <delay.h>
#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <gpio.h>
#include <stdint.h>

// Clock frequencies for the cores, ddr and the peripherals are all derived from the hfclk (high frequency clock) and it is always 26 MHz
#define FU740_HFCLK_FREQ (26 * MHz)

struct prci_ctlr {
	u32 hfxosccfg;              // offset 0x00
	u32 core_pllcfg;            // offset 0x04
	u32 core_plloutdiv;         // offset 0x08
	u32 ddr_pllcfg;             // offset 0x0c
	u32 ddr_plloutdiv;          // offset 0x10
	u32 pcieaux_plloutdiv;      // offset 0x14 (undocumented)
	u32 reserved18;             // offset 0x18
	u32 gemgxl_pllcfg;          // offset 0x1c
	u32 gemgxl_plloutdiv;       // offset 0x20
	u32 core_clk_sel_reg;       // offset 0x24
	u32 devices_reset_n;        // offset 0x28
	u32 clk_mux_status;         // offset 0x2C
	u32 cltx_pllcfg;            // offset 0x30 chiplink (undocumented)
	u32 cltx_plloutdiv;         // offset 0x34 chiplink (undocumented)
	u32 dvfs_core_pllcfg;       // offset 0x38
	u32 dvfs_core_plloutdiv;    // offset 0x3C
	u32 corepllsel;             // offset 0x40 (undocumented, but probably same as last gen)
	u8 reserved44[12];          // offset 0x44
	u32 hfpclk_pllcfg;          // offset 0x50
	u32 hfpclk_plloutdiv;       // offset 0x54
	u32 hfpclkpllsel;           // offset 0x58 (undocumented, but probably same as last gen)
	u32 hfpclk_div_reg;         // offset 0x5C
	u8 reserved60[128];         // offset 0x60
	u32 prci_plls;              // offset 0xE0
	u8 reservedE4[12];          // offset 0xE4
	u32 procmoncfg_core_clock;  // offset 0xF0 (undocumented)
} __packed;

static struct prci_ctlr *prci = (void *)FU740_PRCI;

// =================================
// clock selections
// =================================

#define PRCI_COREPLLSEL_MASK 1
#define PRCI_COREPLLSEL_COREPLL 0
#define PRCI_COREPLLSEL_DVFSCOREPLL 1

#define PRCI_CORECLKSEL_MASK 1
#define PRCI_CORECLKSEL_CORECLKPLL 0
#define PRCI_CORECLKSEL_HFCLK 1

#define PRCI_HFPCLKSEL_MASK 1
#define PRCI_HFPCLKSEL_PLL 0
#define PRCI_HFPCLKSEL_HFCLK 1

// ===================================
// pllcfg register format is used by all PLLs
// ===================================

#define PRCI_PLLCFG_DIVR_SHIFT       0
#define PRCI_PLLCFG_DIVF_SHIFT       6
#define PRCI_PLLCFG_DIVQ_SHIFT      15
#define PRCI_PLLCFG_RANGE_SHIFT     18
#define PRCI_PLLCFG_BYPASS_SHIFT    24
#define PRCI_PLLCFG_FSEBYPASS_SHIFT 25
#define PRCI_PLLCFG_LOCK_SHIFT      31

#define PRCI_PLLCFG_DIVR_MASK      (0x03f << PRCI_PLLCFG_DIVR_SHIFT)
#define PRCI_PLLCFG_DIVF_MASK      (0x1ff << PRCI_PLLCFG_DIVF_SHIFT)
#define PRCI_PLLCFG_DIVQ_MASK      (0x007 << PRCI_PLLCFG_DIVQ_SHIFT)
#define PRCI_PLLCFG_RANGE_MASK     (0x007 << PRCI_PLLCFG_RANGE_SHIFT)
#define PRCI_PLLCFG_BYPASS_MASK    (0x001 << PRCI_PLLCFG_BYPASS_SHIFT)
#define PRCI_PLLCFG_FSEBYPASS_MASK (0x001 << PRCI_PLLCFG_FSEBYPASS_SHIFT)
#define PRCI_PLLCFG_LOCK_MASK      (0x001 << PRCI_PLLCFG_LOCK_SHIFT)

// ===================================
// plloutdiv register formats
// ===================================

// registered are used to enable/disable PLLs
#define PRCI_DVFSCORE_PLLOUTDIV_MASK (1 << 24) // Note: u-boot and fu740 manual differ here ...
#define PRCI_HFPCLK_PLLOUTDIV_MASK   (1 << 31) // Note: according to u-boot it is (1 << 24) but if I use that it gets stuck
#define PRCI_DDR_PLLOUTDIV_MASK      (1 << 31)
#define PRCI_GEMGXL_PLLOUTDIV_MASK   (1 << 31)
#define PRCI_CLTX_PLLOUTDIV_MASK     (1 << 24) // undocumented (chiplink tx)
#define PRCI_PCIEAUX_PLLOUTDIV_MASK  (1 <<  0) // undocumented
#define PRCI_CORE_PLLOUTDIV_MASK     (1 << 31) // undocumented

// ===================================
// devicereset register formats
// ===================================

// used to get devices in or out of reset
#define PRCI_DEVICES_RESET_DDR_CTRL_RST (1 << 0) // DDR Controller
#define PRCI_DEVICES_RESET_DDR_AXI_RST  (1 << 1) // DDR Controller AXI Interface
#define PRCI_DEVICES_RESET_DDR_AHB_RST  (1 << 2) // DDR Controller AHB Interface
#define PRCI_DEVICES_RESET_DDR_PHY_RST  (1 << 3) // DDR PHY
#define PRCI_DEVICES_RESET_PCIEAUX_RST  (1 << 4)
#define PRCI_DEVICES_RESET_GEMGXL_RST   (1 << 5) // Gigabit Ethernet Subsystem
#define PRCI_DEVICES_RESET_CLTX_RST     (1 << 6) // chiplink reset (undocumented)

// ===================================
// prci_plls register format
// ===================================

// used to check if certain PLLs are present in the SOC
#define PRCI_PLLS_CLTXPLL     (1 << 0)
#define PRCI_PLLS_GEMGXLPLL   (1 << 1)
#define PRCI_PLLS_DDRPLL      (1 << 2)
#define PRCI_PLLS_HFPCLKPLL   (1 << 3)
#define PRCI_PLLS_DVFSCOREPLL (1 << 4)
#define PRCI_PLLS_COREPLL     (1 << 5)

// ===================================
// clk_mux_status register format
// ===================================

// read only register which is used to set some clock multiplex settings
// the value of this register depends on the state of pins connected to the FU740 SOC
// on the hifive-unmatched board the state of the pins is set by a hardware switch
#define PRCI_CLK_MUX_STATUS_CORECLKPLLSEL (1 << 0)
        // 0 - HFCLK or CORECLK
        // 1 - only HFCLK
#define PRCI_CLK_MUX_STATUS_TLCLKSEL      (1 << 1)
        // 0 - CORECLK/2
        // 1 - CORECLK
#define PRCI_CLK_MUX_STATUS_RTCXSEL       (1 << 2)
        // 0 - use HFXCLK for RTC
        // 1 - use RTCXALTCLKIN for RTC
#define PRCI_CLK_MUX_STATUS_DDRCTRLCLKSEL (1 << 3)
#define PRCI_CLK_MUX_STATUS_DDRPHYCLKSEL  (1 << 4)
#define PRCI_CLK_MUX_STATUS_RESERVED      (1 << 5)
#define PRCI_CLK_MUX_STATUS_GEMGXLCLKSEL  (1 << 6)
#define PRCI_CLK_MUX_STATUS_MAINMEMCLKSEL (1 << 7)

// ===================================
// hfxosccfg register format
// ===================================

#define PRCI_HFXOSCCFG_HFXOSEN   (1 << 30) // Crystal oscillator enable
        // Note: I guess (it is not documented)
        // 0 - XTAL PADS
        // 1 - OSC PADS
#define PRCI_HFXOSCCFG_HFXOSCRDY (1 << 31) // Crystal oscillator ready

struct pll_settings {
	unsigned int divr:6; // divider before PLL loop (reference), equal to divr + 1
	unsigned int divf:9; // VCO feedback divider value, equal to 2 * (divf + 1)
	unsigned int divq:3; // divider after PLL loop, equal to 2^divq
	// PLL filter range (TODO documentation is not really clear on how to set it)
	unsigned int range:3;
	unsigned int bypass:1; // probably used to bypass the PLL
	// internal or external input path (internal = 1, external = 0)
	//WARN this is only a guess since it is undocumented
	unsigned int fsebypass:1;
};

static void configure_pll(u32 *reg, const struct pll_settings *s)
{
	// Write the settings to the register
	u32 c = read32(reg);
	clrsetbits32(&c, PRCI_PLLCFG_DIVR_MASK
		       | PRCI_PLLCFG_DIVF_MASK
		       | PRCI_PLLCFG_DIVQ_MASK
		       | PRCI_PLLCFG_RANGE_MASK
		       | PRCI_PLLCFG_BYPASS_MASK
		       | PRCI_PLLCFG_FSEBYPASS_MASK,
		         (s->divr << PRCI_PLLCFG_DIVR_SHIFT)
		       | (s->divf << PRCI_PLLCFG_DIVF_SHIFT)
		       | (s->divq << PRCI_PLLCFG_DIVQ_SHIFT)
		       | (s->range << PRCI_PLLCFG_RANGE_SHIFT)
		       | (s->bypass << PRCI_PLLCFG_BYPASS_SHIFT)
		       | (s->fsebypass << PRCI_PLLCFG_FSEBYPASS_SHIFT));
	write32(reg, c);

	// Wait for PLL lock
	while (!(read32(reg) & PRCI_PLLCFG_LOCK_MASK))
		;
}

/*
 * Section 7.1 recommends a frequency of 1.0 GHz (up to 1.5 GHz is possible)
 * Section 7.4.2 provides the necessary values
 *
 * COREPLL is set up for ~1 GHz output frequency.
 * divr = 0 (x1), divf = 76 (x154) => (4004 MHz VCO), divq = 2 (/4 Output divider)
 */
static const struct pll_settings corepll_settings = {
	.divr = 0,
	.divf = 76,
	.divq = 2,
	.range = 4,
	.bypass = 0,
	.fsebypass = 1, // external feedback mode is not supported
};

/*
 * Section 7.4.3: DDR and Ethernet Subsystem Clocking and Reset
 *
 * DDRPLL is set up for 933 MHz output frequency.
 * divr = 0 (x1), divf = 71 (x144) => (3744 MHz VCO), divq = 2 (/4 output divider)
 */
static const struct pll_settings ddrpll_settings = {
	.divr = 0,
	.divf = 71,
	.divq = 2,
	.range = 4,
	.bypass = 0,
	.fsebypass = 1, // external feedback mode is not supported
};

/*
 * GEMGXLPLL is set up for 125 MHz output frequency.
 * divr = 0 (x1), divf = 76 (x154) => (4004 MHz VCO), divq = 5 (/32 output divider)
 */
static const struct pll_settings gemgxlpll_settings = {
	.divr = 0,
	.divf = 76,
	.divq = 5,
	.range = 4,
	.bypass = 0,
	.fsebypass = 1, // external feedback mode is not supported
};

/*
 * HFPCLKPLL is set up for 520 MHz output frequency.
 * TODO a lower value should also suffice as well as safe some power
 * divr = 1 (/2), divf = 39 (x80) => (2080 MHz VCO), divq = 2 (/4 output divider)
 */
static const struct pll_settings hfpclkpll_settings = {
	.divr = 1,
	//.divf = 122,
	.divf = 39,
	.divq = 2,
	.range = 4,
	.bypass = 0,
	.fsebypass = 1, // external feedback mode is not supported
};

/*
 * CLTXCLKPLL is set up for 520 MHz output frequency.
 * divr = 1 (/2), divf = 122 (x154) => (4004 MHz VCO), divq = 2 (/4 output divider)
 */
static const struct pll_settings cltxpll_settings = {
	.divr = 1,
	.divf = 39,
	.divq = 2,
	.range = 4,
	.bypass = 0,
	.fsebypass = 1, // external feedback mode is not supported
};

static void init_coreclk(void)
{
	// we can't modify the coreclk PLL while we are running on it, so let coreclk devise
	// its clock from hfclk before modifying PLL
	clrsetbits32(&prci->core_clk_sel_reg, PRCI_CORECLKSEL_MASK, PRCI_CORECLKSEL_HFCLK);

	// only configure pll if it is present
	if (!(read32(&prci->prci_plls) & PRCI_PLLS_COREPLL)) {
		return;
	}

	configure_pll(&prci->core_pllcfg, &corepll_settings);

	// switch coreclk multiplexer to use corepll as clock source again
	clrsetbits32(&prci->core_clk_sel_reg, PRCI_CORECLKSEL_MASK, PRCI_CORECLKSEL_CORECLKPLL);
}

static void init_ddrclk(void)
{
	// only configure pll if it is present
	if (!(read32(&prci->prci_plls) & PRCI_PLLS_DDRPLL)) {
		return;
	}

	// disable ddr clock output before reconfiguring the PLL
	u32 cfg1 = read32(&prci->ddr_plloutdiv);
	clrbits32(&cfg1, PRCI_DDR_PLLOUTDIV_MASK);
	write32(&prci->ddr_plloutdiv, cfg1);

	configure_pll(&prci->ddr_pllcfg, &ddrpll_settings);

	// PLL is ready/locked so enable it (its gated)
	setbits32(&cfg1, PRCI_DDR_PLLOUTDIV_MASK);
	write32(&prci->ddr_plloutdiv, cfg1);
}

static void init_gemgxlclk(void)
{
	// only configure pll if it is present
	if (!(read32(&prci->prci_plls) & PRCI_PLLS_GEMGXLPLL)) {
		return;
	}

	// disable gemgxl clock output before reconfiguring the PLL
	u32 cfg1 = read32(&prci->gemgxl_plloutdiv);
	clrbits32(&cfg1, PRCI_GEMGXL_PLLOUTDIV_MASK);
	write32(&prci->gemgxl_plloutdiv, cfg1);

	configure_pll(&prci->gemgxl_pllcfg, &gemgxlpll_settings);

	// PLL is ready/locked so enable it (its gated)
	setbits32(&cfg1, PRCI_GEMGXL_PLLOUTDIV_MASK);
	write32(&prci->gemgxl_plloutdiv, cfg1);
}

/*
 * Configure High Frequency peripheral clock which is used by
 * UART, SPI, GPIO, I2C and PWM subsystem
 */
static void init_hfpclk(void)
{
	// we can't modify the hfpclk PLL while we are running on it, so let pclk devise
	// its clock from hfclk before modifying PLL
	u32 hfpclksel = read32(&prci->hfpclkpllsel);
	hfpclksel |= PRCI_HFPCLKSEL_HFCLK;
	write32(&prci->hfpclkpllsel, hfpclksel);

	configure_pll(&prci->hfpclk_pllcfg, &hfpclkpll_settings);

	// PLL is ready/locked so enable it (its gated)
	u32 hfpclk_plloutdiv = read32(&prci->hfpclk_plloutdiv);
	hfpclk_plloutdiv |=  PRCI_HFPCLK_PLLOUTDIV_MASK;
	write32(&prci->hfpclk_plloutdiv, hfpclk_plloutdiv);

	mdelay(1);

	// switch to using PLL for hfpclk
	clrbits32(&prci->hfpclkpllsel, PRCI_HFPCLKSEL_MASK);

	udelay(70);
}

static void reset_deassert(u8 reset_index)
{
	u32 device_reset = read32(&prci->devices_reset_n);
	device_reset |= reset_index;
	write32(&prci->devices_reset_n, device_reset);
}

static void init_cltx(void)
{
	// disable hfpclkpll before configuring it
	u32 cfg1 = read32(&prci->cltx_plloutdiv);
	clrbits32(&cfg1, PRCI_CLTX_PLLOUTDIV_MASK);
	write32(&prci->cltx_plloutdiv, cfg1);

	configure_pll(&prci->cltx_pllcfg, &cltxpll_settings);

	// PLL is ready/locked so enable it (its gated)
	setbits32(&cfg1, PRCI_CLTX_PLLOUTDIV_MASK);
	write32(&prci->cltx_plloutdiv, cfg1);

	// get chiplink out of reset
	reset_deassert(PRCI_DEVICES_RESET_CLTX_RST);

	udelay(70);
}

void clock_init(void)
{
	// first configure the coreclk (used by HARTs) to get maximum speed early on
	init_coreclk();

	// put all devices in reset (e.g. DDR, ethernet, pcie) before configuring their clocks
	write32(&prci->devices_reset_n, 0);

	// initialize clock used by DDR subsystem
	init_ddrclk();

	// get DDR controller out of reset
	reset_deassert(PRCI_DEVICES_RESET_DDR_CTRL_RST);

	// wait at least one full DDR controller clock cycle
	asm volatile ("fence");

	// get DDR controller (register interface) out of reset
	// get DDR subsystem PHY out of reset
	reset_deassert(PRCI_DEVICES_RESET_DDR_AXI_RST |
	               PRCI_DEVICES_RESET_DDR_AHB_RST |
	               PRCI_DEVICES_RESET_DDR_PHY_RST);

	// we need to wait 256 full ddrctrl clock cycles until we can interact with the DDR subsystem
	for (int i = 0; i < 256; i++)
		asm volatile ("nop");

	if (read32(&prci->prci_plls) & PRCI_PLLS_HFPCLKPLL) {
		// set hfclk as reference for peripheral clock since we don't have the PLL
		//clrsetbits32(&prci->hfpclkpllsel, PRCI_HFPCLKSEL_MASK, PRCI_HFPCLKSEL_HFCLK);
		init_hfpclk();
	} else if (read32(&prci->prci_plls) & PRCI_PLLS_CLTXPLL) {
		// Note: this path has never been tested since the platforms tested with
		// always have HFPCLKPLL
		init_cltx();
		// get chiplink out of reset
		reset_deassert(PRCI_DEVICES_RESET_CLTX_RST);
	}

	// GEMGXL init VSC8541 PHY reset sequence;
	gpio_set_direction(GEMGXL_RST, GPIO_OUTPUT);
	gpio_set(GEMGXL_RST, 1);

	udelay(1);

	/* Reset PHY again to enter unmanaged mode */
	gpio_set(GEMGXL_RST, 0);
	udelay(1);
	gpio_set(GEMGXL_RST, 1);
	mdelay(15);

	init_gemgxlclk();

	// get ethernet out of reset
	reset_deassert(PRCI_DEVICES_RESET_GEMGXL_RST);
}

// get the peripheral clock frequency used by UART (probably also SPI, GPIO, I2C and PWM)
int clock_get_pclk(void)
{
	u64 pclk = FU740_HFCLK_FREQ;

	// check if hfpclkpll is present and
	// check if hfpclkpll is selected in the multiplexer TODO
	// check if hpfclkpll is enabled
	if ((read32(&prci->prci_plls) & PRCI_PLLS_HFPCLKPLL) &&
	    (read32(&prci->hfpclk_plloutdiv) & PRCI_HFPCLK_PLLOUTDIV_MASK)) {
		int hfpclk_pllcfg = read32(&prci->hfpclk_pllcfg);
		int divr = (hfpclk_pllcfg & PRCI_PLLCFG_DIVR_MASK) >> PRCI_PLLCFG_DIVR_SHIFT;
		int divf = (hfpclk_pllcfg & PRCI_PLLCFG_DIVF_MASK) >> PRCI_PLLCFG_DIVF_SHIFT;
		int divq = (hfpclk_pllcfg & PRCI_PLLCFG_DIVQ_MASK) >> PRCI_PLLCFG_DIVQ_SHIFT;
		pclk /= (divr + 1); // reference divider
		pclk *= (2 * (divf + 1)); // feedback divider
		pclk /= (1 << divq); // output divider
	}

	// divider value before pclk seems to be (hfpclkdiv + 2). Not mentioned in fu740 manual though.
	return pclk / (read32(&prci->hfpclk_div_reg) + 2);
}
