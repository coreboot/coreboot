/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Imagination Technologies
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <assert.h>
#include <delay.h>
#include <soc/clocks.h>
#include <timer.h>

/* Definitions for PLL enable */
#define PISTACHIO_CLOCK_SWITCH		0xB8144200

#define SYS_EXTERN_PLL_BYPASS_MASK	0x00002000
#define SYS_PLL_CTRL4_ADDR		0xB8144048
#define SYS_INTERNAL_PLL_BYPASS_MASK	0x10000000
#define SYS_PLL_PD_CTRL_ADDR		0xB8144044
#define SYS_PLL_PD_CTRL_PD_MASK		0x00000039
#define SYS_PLL_DACPD_ADDR		0xB8144044
#define SYS_PLL_DACPD_MASK		0x00000002
#define SYS_PLL_DSMPD_ADDR		0xB8144044
#define SYS_PLL_DSMPD_MASK		0x00000004

#define MIPS_EXTERN_PLL_BYPASS_MASK	0x00000002
#define MIPS_PLL_CTRL2_ADDR		0xB8144008
#define MIPS_INTERNAL_PLL_BYPASS_MASK	0x10000000
#define MIPS_PLL_PD_CTRL_ADDR		0xB8144004
#define MIPS_PLL_PD_CTRL_PD_MASK	0x0D000000
#define MIPS_PLL_DSMPD_ADDR		0xB8144004
#define MIPS_PLL_DSMPD_MASK		0x02000000

/* Definitions for PLL dividers */
#define SYS_PLL_POSTDIV_ADDR		0xB8144040
#define SYS_PLL_POSTDIV1_MASK		0x07000000
#define SYS_PLL_POSTDIV1_SHIFT		24
#define SYS_PLL_POSTDIV2_MASK		0x38000000
#define SYS_PLL_POSTDIV2_SHIFT		27
#define SYS_PLL_STATUS_ADDR		0xB8144038
#define SYS_PLL_STATUS_LOCK_MASK	0x00000001

#define SYS_PLL_REFDIV_ADDR		0xB814403C
#define SYS_PLL_REFDIV_MASK		0x0000003F
#define SYS_PLL_REFDIV_SHIFT		0
#define SYS_PLL_FEEDBACK_ADDR		0xB814403C
#define SYS_PLL_FEEDBACK_MASK		0x0003FFC0
#define SYS_PLL_FEEDBACK_SHIFT		6

#define MIPS_PLL_POSTDIV_ADDR		0xB8144004
#define MIPS_PLL_POSTDIV1_MASK		0x001C0000
#define MIPS_PLL_POSTDIV1_SHIFT		18
#define MIPS_PLL_POSTDIV2_MASK		0x00E00000
#define MIPS_PLL_POSTDIV2_SHIFT		21
#define MIPS_PLL_STATUS_ADDR		0xB8144000
#define MIPS_PLL_STATUS_LOCK_MASK	0x00000001

#define MIPS_REFDIV_ADDR		0xB8144004
#define MIPS_REFDIV_MASK		0x0000003F
#define MIPS_REFDIV_SHIFT		0
#define MIPS_FEEDBACK_ADDR		0xB8144004
#define MIPS_FEEDBACK_MASK		0x0003FFC0
#define MIPS_FEEDBACK_SHIFT		6

/* Definitions for system clock setup */
#define SYSCLKINTERNAL_CTRL_ADDR	0xB8144244
#define SYSCLKINTERNAL_MASK		0X00000007

/* Definitions for MIPS clock setup */
#define MIPSCLKINTERNAL_CTRL_ADDR	0xB8144204
#define MIPSCLKINTERNAL_MASK		0x00000003
#define MIPSCLKOUT_CTRL_ADDR		0xB8144208
#define MIPSCLKOUT_MASK			0x000000FF

/* Peripheral Clock gate reg */
#define MIPS_CLOCK_GATE_ADDR		0xB8144900
#define RPU_CLOCK_GATE_ADDR		0xB8144904
#define MIPS_CLOCK_GATE_ALL_ON		0x3fff
#define RPU_CLOCK_GATE_ALL_OFF		0x0

/* Definitions for USB clock setup */
#define USBPHYCLKOUT_CTRL_ADDR		0xB814422C
#define USBPHYCLKOUT_MASK		0X0000003F
#define USBPHYCONTROL1_ADDR		0xB8149004
#define USBPHYCONTROL1_FSEL_SHIFT	2
#define USBPHYCONTROL1_FSEL_MASK	0x1C
#define USBPHYSTRAPCTRL_ADDR		0xB8149010
#define USBPHYSTRAPCTRL_REFCLKSEL_SHIFT	4
#define USBPHYSTRAPCTRL_REFCLKSEL_MASK	0x30
#define USBPHYSTATUS_ADDR		0xB8149014
#define USBPHYSTATUS_RX_PHY_CLK_MASK	0x200
#define USBPHYSTATUS_RX_UTMI_CLK_MASK	0x100
#define USBPHYSTATUS_VBUS_FAULT_MASK	0x80

/* Definitions for UART0/1 setup */
#define UART0CLKINTERNAL_CTRL_ADDR	0xB8144234
#define UART0CLKINTERNAL_MASK		0x00000007
#define UART0CLKOUT_CTRL_ADDR		0xB8144238
#define UART0CLKOUT_MASK		0x000003FF
#define UART1CLKINTERNAL_CTRL_ADDR	0xB814423C
#define UART1CLKINTERNAL_MASK		0x00000007
#define UART1CLKOUT_CTRL_ADDR		0xB8144240
#define UART1CLKOUT_MASK		0x000003FF

/* Definitions for I2C setup */
#define I2CCLKDIV1_CTRL_ADDR(i)		(0xB8144800 + 0x013C + (2*(i)*4))
#define I2CCLKDIV1_MASK			0x0000007F
#define I2CCLKOUT_CTRL_ADDR(i)		(0xB8144800 + 0x0140 + (2*(i)*4))
#define I2CCLKOUT_MASK			0x0000007F

/* Definitions for ROM clock setup */
#define ROMCLKOUT_CTRL_ADDR		0xB814490C
#define ROMCLKOUT_MASK			0x0000007F

/* Definitions for ETH clock setup */
#define ENETCLKMUX_MASK			0x00004000
#define ENETCLKDIV_CTRL_ADDR		0xB8144230
#define ENETCLKDIV_MASK			0x0000003F

/* Definitions for timeout values */
#define PLL_TIMEOUT_VALUE_US		20000
#define USB_TIMEOUT_VALUE_US		200000
#define SYS_CLK_LOCK_DELAY		3

struct pll_parameters {
	u32 external_bypass_mask;
	u32 ctrl_addr;
	u32 internal_bypass_mask;
	u32 power_down_ctrl_addr;
	u32 power_down_ctrl_mask;
	u32 dacpd_addr;
	u32 dacpd_mask;
	u32 dsmpd_addr;
	u32 dsmpd_mask;
	u32 postdiv_addr;
	u32 postdiv1_shift;
	u32 postdiv1_mask;
	u32 postdiv2_shift;
	u32 postdiv2_mask;
	u32 status_addr;
	u32 status_lock_mask;
	u32 refdivider;
	u32 refdiv_addr;
	u32 refdiv_shift;
	u32 refdiv_mask;
	u32 feedback;
	u32 feedback_addr;
	u32 feedback_shift;
	u32 feedback_mask;
};

enum plls {
	SYS_PLL = 0,
	MIPS_PLL = 1
};

static struct pll_parameters pll_params[] = {
	[SYS_PLL] = {
		.external_bypass_mask = SYS_EXTERN_PLL_BYPASS_MASK,
		.ctrl_addr = SYS_PLL_CTRL4_ADDR,
		.internal_bypass_mask = SYS_INTERNAL_PLL_BYPASS_MASK,
		.power_down_ctrl_addr = SYS_PLL_PD_CTRL_ADDR,
		.power_down_ctrl_mask = SYS_PLL_PD_CTRL_PD_MASK,
		/* Noise cancellation */
		.dacpd_addr = SYS_PLL_DACPD_ADDR,
		.dacpd_mask = SYS_PLL_DACPD_MASK,
		.dsmpd_addr = SYS_PLL_DSMPD_ADDR,
		/* 0 - Integer mode
		 * SYS_PLL_DSMPD_MASK - Fractional mode
		 */
		.dsmpd_mask = 0,
		.postdiv_addr = SYS_PLL_POSTDIV_ADDR,
		.postdiv1_shift = SYS_PLL_POSTDIV1_SHIFT,
		.postdiv1_mask = SYS_PLL_POSTDIV1_MASK,
		.postdiv2_shift = SYS_PLL_POSTDIV2_SHIFT,
		.postdiv2_mask = SYS_PLL_POSTDIV2_MASK,
		.status_addr = SYS_PLL_STATUS_ADDR,
		.status_lock_mask = SYS_PLL_STATUS_LOCK_MASK,
		.refdivider = 0, /* Not defined yet */
		.refdiv_addr = SYS_PLL_REFDIV_ADDR,
		.refdiv_shift = SYS_PLL_REFDIV_SHIFT,
		.refdiv_mask = SYS_PLL_REFDIV_MASK,
		.feedback = 0, /* Not defined yet */
		.feedback_addr = SYS_PLL_FEEDBACK_ADDR,
		.feedback_shift = SYS_PLL_FEEDBACK_SHIFT,
		.feedback_mask = SYS_PLL_FEEDBACK_MASK
	},

	[MIPS_PLL] = {
		.external_bypass_mask = MIPS_EXTERN_PLL_BYPASS_MASK,
		.ctrl_addr = MIPS_PLL_CTRL2_ADDR,
		.internal_bypass_mask = MIPS_INTERNAL_PLL_BYPASS_MASK,
		.power_down_ctrl_addr = MIPS_PLL_PD_CTRL_ADDR,
		.power_down_ctrl_mask = MIPS_PLL_PD_CTRL_PD_MASK,
		.dacpd_addr = 0,
		.dacpd_mask = 0,
		.dsmpd_addr = MIPS_PLL_DSMPD_ADDR,
		.dsmpd_mask = MIPS_PLL_DSMPD_MASK,
		.postdiv_addr = MIPS_PLL_POSTDIV_ADDR,
		.postdiv1_shift = MIPS_PLL_POSTDIV1_SHIFT,
		.postdiv1_mask = MIPS_PLL_POSTDIV1_MASK,
		.postdiv2_shift = MIPS_PLL_POSTDIV2_SHIFT,
		.postdiv2_mask = MIPS_PLL_POSTDIV2_MASK,
		.status_addr = MIPS_PLL_STATUS_ADDR,
		.status_lock_mask = MIPS_PLL_STATUS_LOCK_MASK,
		.refdivider = 0, /* Not defined yet */
		.refdiv_addr = MIPS_REFDIV_ADDR,
		.refdiv_shift = MIPS_REFDIV_SHIFT,
		.refdiv_mask = MIPS_REFDIV_MASK,
		.feedback = 0, /* Not defined yet */
		.feedback_addr = MIPS_FEEDBACK_ADDR,
		.feedback_shift = MIPS_FEEDBACK_SHIFT,
		.feedback_mask = MIPS_FEEDBACK_MASK
	}
};

static int pll_setup(struct pll_parameters *param, u8 divider1, u8 divider2)
{
	u32 reg;
	struct stopwatch sw;

	/* Check input parameters */
	assert(!((divider1 << param->postdiv1_shift) &
		~(param->postdiv1_mask)));
	assert(!((divider2 << param->postdiv2_shift) &
		~(param->postdiv2_mask)));

	/* Temporary bypass PLL (select XTAL as clock input) */
	reg = read32(PISTACHIO_CLOCK_SWITCH);
	reg &= ~(param->external_bypass_mask);
	write32(PISTACHIO_CLOCK_SWITCH, reg);

	/* Un-bypass PLL's internal bypass */
	reg = read32(param->ctrl_addr);
	reg &= ~(param->internal_bypass_mask);
	write32(param->ctrl_addr, reg);

	/* Disable power down */
	reg = read32(param->power_down_ctrl_addr);
	reg &= ~(param->power_down_ctrl_mask);
	write32(param->power_down_ctrl_addr, reg);

	/* Noise cancellation */
	if (param->dacpd_addr) {
		reg = read32(param->dacpd_addr);
		reg &= ~(param->dacpd_mask);
		write32(param->dacpd_addr, reg);
	}

	/* Functional mode */
	if (param->dsmpd_addr) {
		reg = read32(param->dsmpd_addr);
		reg &= ~(param->dsmpd_mask);
		write32(param->dsmpd_addr, reg);
	}

	if (param->feedback_addr) {
		assert(!((param->feedback << param->feedback_shift) &
			~(param->feedback_mask)));
		reg = read32(param->feedback_addr);
		reg &= ~(param->feedback_mask);
		reg |= (param->feedback << param->feedback_shift) &
			param->feedback_mask;
		write32(param->feedback_addr, reg);
	}

	if (param->refdiv_addr) {
		assert(!((param->refdivider << param->refdiv_shift) &
			~(param->refdiv_mask)));
		reg = read32(param->refdiv_addr);
		reg &= ~(param->refdiv_mask);
		reg |= (param->refdivider << param->refdiv_shift) &
			param->refdiv_mask;
		write32(param->refdiv_addr, reg);
	}

	/* Read postdivider register value */
	reg = read32(param->postdiv_addr);
	/* Set divider 1 */
	reg &= ~(param->postdiv1_mask);
	reg |= (divider1 << param->postdiv1_shift) &
			param->postdiv1_mask;
	/* Set divider 2 */
	reg &= ~(param->postdiv2_mask);
	reg |= (divider2 << param->postdiv2_shift) &
			param->postdiv2_mask;
	/* Write back to register */
	write32(param->postdiv_addr, reg);

	/* Waiting for PLL to lock*/
	stopwatch_init_usecs_expire(&sw, PLL_TIMEOUT_VALUE_US);
	while (!(read32(param->status_addr) & param->status_lock_mask)) {
		if (stopwatch_expired(&sw))
			return PLL_TIMEOUT;
	}

	/* Start using PLL */
	reg = read32(PISTACHIO_CLOCK_SWITCH);
	reg |= param->external_bypass_mask;
	write32(PISTACHIO_CLOCK_SWITCH, reg);

	return CLOCKS_OK;
}

int sys_pll_setup(u8 divider1, u8 divider2, u8 refdivider, u32 feedback)
{
	pll_params[SYS_PLL].refdivider = refdivider;
	pll_params[SYS_PLL].feedback = feedback;
	return pll_setup(&(pll_params[SYS_PLL]), divider1, divider2);
}

int mips_pll_setup(u8 divider1, u8 divider2, u8 refdivider, u32 feedback)
{
	pll_params[MIPS_PLL].refdivider = refdivider;
	pll_params[MIPS_PLL].feedback = feedback;
	return pll_setup(&(pll_params[MIPS_PLL]), divider1, divider2);
}

/*
 * uart1_clk_setup: sets up clocks for UART1
 * divider1: 3-bit divider value
 * divider2: 10-bit divider value
 */
void uart1_clk_setup(u8 divider1, u16 divider2)
{
	u32 reg;

	/* Check input parameters */
	assert(!(divider1 & ~(UART1CLKINTERNAL_MASK)));
	assert(!(divider2 & ~(UART1CLKOUT_MASK)));

	/* Set divider 1 */
	reg = read32(UART1CLKINTERNAL_CTRL_ADDR);
	reg &= ~UART1CLKINTERNAL_MASK;
	reg |= divider1 & UART1CLKINTERNAL_MASK;
	write32(UART1CLKINTERNAL_CTRL_ADDR, reg);

	/* Set divider 2 */
	reg = read32(UART1CLKOUT_CTRL_ADDR);
	reg &= ~UART1CLKOUT_MASK;
	reg |= divider2 & UART1CLKOUT_MASK;
	write32(UART1CLKOUT_CTRL_ADDR, reg);
}

/*
 * i2c_clk_setup: sets up clocks for I2C
 * divider1: 7-bit divider value
 * divider2: 7-bit divider value
 */
void i2c_clk_setup(u8 divider1, u16 divider2, u8 interface)
{
	u32 reg;

	/* Check input parameters */
	assert(!(divider1 & ~(I2CCLKDIV1_MASK)));
	assert(!(divider2 & ~(I2CCLKOUT_MASK)));
	assert(interface < 4);
	/* Set divider 1 */
	reg = read32(I2CCLKDIV1_CTRL_ADDR(interface));
	reg &= ~I2CCLKDIV1_MASK;
	reg |= divider1 & I2CCLKDIV1_MASK;
	write32(I2CCLKDIV1_CTRL_ADDR(interface), reg);

	/* Set divider 2 */
	reg = read32(I2CCLKOUT_CTRL_ADDR(interface));
	reg &= ~I2CCLKOUT_MASK;
	reg |= divider2 & I2CCLKOUT_MASK;
	write32(I2CCLKOUT_CTRL_ADDR(interface), reg);
}

/* system_clk_setup: sets up the system (peripheral) clock */
void system_clk_setup(u8 divider)
{
	u32 reg;

	/* Check input parameters */
	assert(!(divider & ~(SYSCLKINTERNAL_MASK)));

	/* Set system clock divider */
	reg = read32(SYSCLKINTERNAL_CTRL_ADDR);
	reg &= ~SYSCLKINTERNAL_MASK;
	reg |= divider & SYSCLKINTERNAL_MASK;
	write32(SYSCLKINTERNAL_CTRL_ADDR, reg);

	/* Small delay to cover a maximum lock time of 1500 cycles */
	udelay(SYS_CLK_LOCK_DELAY);
}

void mips_clk_setup(u8 divider1, u8 divider2)
{
	u32 reg;

	/* Check input parameters */
	assert(!(divider1 & ~(MIPSCLKINTERNAL_MASK)));
	assert(!(divider2 & ~(MIPSCLKOUT_MASK)));

	/* Set divider 1 */
	reg = read32(MIPSCLKINTERNAL_CTRL_ADDR);
	reg &= ~MIPSCLKINTERNAL_MASK;
	reg |= divider1 & MIPSCLKINTERNAL_MASK;
	write32(MIPSCLKINTERNAL_CTRL_ADDR, reg);

	/* Set divider 2 */
	reg = read32(MIPSCLKOUT_CTRL_ADDR);
	reg &= ~MIPSCLKOUT_MASK;
	reg |= divider2 & MIPSCLKOUT_MASK;
	write32(MIPSCLKOUT_CTRL_ADDR, reg);
}

/* usb_clk_setup: sets up USB clock */
int usb_clk_setup(u8 divider, u8 refclksel, u8 fsel)
{
	u32 reg;
	struct stopwatch sw;

	/* Check input parameters */
	assert(!(divider & ~(USBPHYCLKOUT_MASK)));
	assert(!((refclksel << USBPHYSTRAPCTRL_REFCLKSEL_SHIFT) &
		~(USBPHYSTRAPCTRL_REFCLKSEL_MASK)));
	assert(!((fsel << USBPHYCONTROL1_FSEL_SHIFT) &
		~(USBPHYCONTROL1_FSEL_MASK)));

	/* Set USB divider */
	reg = read32(USBPHYCLKOUT_CTRL_ADDR);
	reg &= ~USBPHYCLKOUT_MASK;
	reg |= divider & USBPHYCLKOUT_MASK;
	write32(USBPHYCLKOUT_CTRL_ADDR, reg);

	/* Set REFCLKSEL */
	reg = read32(USBPHYSTRAPCTRL_ADDR);
	reg &= ~USBPHYSTRAPCTRL_REFCLKSEL_MASK;
	reg |= (refclksel << USBPHYSTRAPCTRL_REFCLKSEL_SHIFT) &
		USBPHYSTRAPCTRL_REFCLKSEL_MASK;
	write32(USBPHYSTRAPCTRL_ADDR, reg);

	/* Set FSEL */
	reg = read32(USBPHYCONTROL1_ADDR);
	reg &= ~USBPHYCONTROL1_FSEL_MASK;
	reg |= (fsel << USBPHYCONTROL1_FSEL_SHIFT) &
		USBPHYCONTROL1_FSEL_MASK;
	write32(USBPHYCONTROL1_ADDR, reg);

	/* Waiting for USB clock status */
	stopwatch_init_usecs_expire(&sw, USB_TIMEOUT_VALUE_US);
	while (1) {
		reg = read32(USBPHYSTATUS_ADDR);
		if (reg & USBPHYSTATUS_VBUS_FAULT_MASK)
			return USB_VBUS_FAULT;
		if (stopwatch_expired(&sw))
			return USB_TIMEOUT;
		/* Check if USB is set up properly */
		if ((reg & USBPHYSTATUS_RX_PHY_CLK_MASK) &&
			(reg & USBPHYSTATUS_RX_UTMI_CLK_MASK))
			break;
	}

	return CLOCKS_OK;
}

void rom_clk_setup(u8 divider)
{
	u32 reg;

	/* Check input parameter */
	assert(!(divider & ~(ROMCLKOUT_MASK)));

	/* Set ROM divider */
	reg = read32(ROMCLKOUT_CTRL_ADDR);
	reg &= ~ROMCLKOUT_MASK;
	reg |= divider & ROMCLKOUT_MASK;
	write32(ROMCLKOUT_CTRL_ADDR, reg);
}

void eth_clk_setup(u8 mux, u8 divider)
{

	u32 reg;

	/* Check input parameters */
	assert(!(divider & ~(ENETCLKDIV_MASK)));
	/* This can be either 0 or 1, selecting between
	 * ENET and system clock as clocksource */
	assert(!(mux & ~(0x1)));

	/* Set ETH divider */
	reg = read32(ENETCLKDIV_CTRL_ADDR);
	reg &= ~ENETCLKDIV_MASK;
	reg |= divider & ENETCLKDIV_MASK;
	write32(ENETCLKDIV_CTRL_ADDR, reg);

	/* Select source */
	if (mux) {
		reg = read32(PISTACHIO_CLOCK_SWITCH);
		reg |= ENETCLKMUX_MASK;
		write32(PISTACHIO_CLOCK_SWITCH, reg);
	}
}

void setup_clk_gate_defaults(void)
{
	write32(MIPS_CLOCK_GATE_ADDR, MIPS_CLOCK_GATE_ALL_ON);
	write32(RPU_CLOCK_GATE_ADDR, RPU_CLOCK_GATE_ALL_OFF);
}
