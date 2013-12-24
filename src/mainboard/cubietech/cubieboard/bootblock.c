/*
 * Minimal bootblock for Cubieboard
 * It sets up CPU clock, and enables the bootblock console.
 *
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#include <arch/io.h>
#include <uart.h>
#include <console/console.h>
#include <cpu/allwinner/a10/gpio.h>
#include <cpu/allwinner/a10/clock.h>

#define CPU_AHB_APB0_DEFAULT 		\
	 CPU_CLK_SRC_OSC24M	 	\
	 | APB0_DIV_1			\
	 | AHB_DIV_1			\
	 | AXI_DIV_1

#define GPB_UART0_FUNC			2
#define GPB_UART0_PINS			((1 << 22) | (1 << 23))

static void cubieboard_set_sys_clock(void)
{
	u32 reg32;
	struct a10_ccm *ccm = (void *)A1X_CCM_BASE;

	/* Switch CPU clock to main oscillator */
	write32(CPU_AHB_APB0_DEFAULT, &ccm->cpu_ahb_apb0_cfg);

	/* Configure the PLL1. The value is the same one used by u-boot */
	write32(0xa1005000, &ccm->pll1_cfg);

	/* FIXME: Delay to wait for PLL to lock */
	u32 wait = 1000;
	while (--wait);

	/* Switch CPU to PLL clock */
	reg32 = read32(&ccm->cpu_ahb_apb0_cfg);
	reg32 &= ~CPU_CLK_SRC_MASK;
	reg32 |= CPU_CLK_SRC_PLL1;
	write32(reg32, &ccm->cpu_ahb_apb0_cfg);
}

static void cubieboard_setup_clocks(void)
{
	struct a10_ccm *ccm = (void *)A1X_CCM_BASE;

	cubieboard_set_sys_clock();
	/* Configure the clock source for APB1. This drives our UART */
	write32(APB1_CLK_SRC_OSC24M | APB1_RAT_N(0) | APB1_RAT_M(0),
		&ccm->apb1_clk_div_cfg);

}

static void cubieboard_setup_gpios(void)
{
	/* Mux UART pins */
	gpio_set_multipin_func(GPB, GPB_UART0_PINS, GPB_UART0_FUNC);
}

static void cubieboard_enable_uart(void)
{
	a1x_periph_clock_enable(A1X_CLKEN_UART0);
}

void bootblock_mainboard_init(void);
void bootblock_mainboard_init(void)
{
	cubieboard_setup_clocks();
	cubieboard_setup_gpios();
	cubieboard_enable_uart();
}
