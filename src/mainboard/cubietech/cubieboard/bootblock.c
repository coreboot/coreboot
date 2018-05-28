/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * Minimal bootblock for Cubieboard
 * It sets up CPU clock, and enables the bootblock console.
 */

#include <arch/io.h>
#include <bootblock_common.h>
#include <console/uart.h>
#include <console/console.h>
#include <delay.h>
#include <cpu/allwinner/a10/gpio.h>
#include <cpu/allwinner/a10/clock.h>
#include <cpu/allwinner/a10/dramc.h>

#define CPU_AHB_APB0_DEFAULT		\
	 CPU_CLK_SRC_OSC24M		\
	 | APB0_DIV_1			\
	 | AHB_DIV_2			\
	 | AXI_DIV_1

#define GPH_STATUS_LEDS			(1 << 20) | (1 << 21)
#define GPH_LED1_PIN_NO			21
#define GPH_LED2_PIN_NO			20

#define GPB_UART0_FUNC			2
#define GPB_UART0_PINS			((1 << 22) | (1 << 23))

#define GPF_SD0_FUNC			2
#define GPF_SD0_PINS			0x3f	/* PF0 thru PF5 */
#define GPH1_SD0_DET_FUNC		5

static void cubieboard_set_sys_clock(void)
{
	u32 reg32;
	struct a10_ccm *ccm = (void *)A1X_CCM_BASE;

	/* Switch CPU clock to main oscillator */
	write32(&ccm->cpu_ahb_apb0_cfg, CPU_AHB_APB0_DEFAULT);

	/* Configure the PLL1. The value is the same one used by u-boot
	 * P = 1, N = 16, K = 1, M = 1 --> Output = 384 MHz
	 */
	write32(&ccm->pll1_cfg, 0xa1005000);

	/* FIXME: Delay to wait for PLL to lock */
	u32 wait = 1000;
	while (--wait);

	/* Switch CPU to PLL clock */
	reg32 = read32(&ccm->cpu_ahb_apb0_cfg);
	reg32 &= ~CPU_CLK_SRC_MASK;
	reg32 |= CPU_CLK_SRC_PLL1;
	write32(&ccm->cpu_ahb_apb0_cfg, reg32);
}

static void cubieboard_setup_clocks(void)
{
	struct a10_ccm *ccm = (void *)A1X_CCM_BASE;

	cubieboard_set_sys_clock();
	/* Configure the clock source for APB1. This drives our UART */
	write32(&ccm->apb1_clk_div_cfg,
		APB1_CLK_SRC_OSC24M | APB1_RAT_N(0) | APB1_RAT_M(0));

	/* Configure the clock for SD0 */
	write32(&ccm->sd0_clk_cfg,
		SDx_CLK_GATE | SDx_CLK_SRC_OSC24M | SDx_RAT_EXP_N(0) | SDx_RAT_M(1));

	/* Enable clock to SD0 */
	a1x_periph_clock_enable(A1X_CLKEN_MMC0);

}

static void cubieboard_setup_gpios(void)
{
	/* Mux Status LED pins */
	gpio_set_multipin_func(GPH, GPH_STATUS_LEDS, GPIO_PIN_FUNC_OUTPUT);
	/* Turn on green LED to let user know we're executing coreboot code */
	gpio_set(GPH, GPH_LED2_PIN_NO);

	/* Mux UART pins */
	gpio_set_multipin_func(GPB, GPB_UART0_PINS, GPB_UART0_FUNC);

	/* Mux SD pins */
	gpio_set_multipin_func(GPF, GPF_SD0_PINS, GPF_SD0_FUNC);
	gpio_set_pin_func(GPH, 1, GPH1_SD0_DET_FUNC);
}

static void cubieboard_enable_uart(void)
{
	a1x_periph_clock_enable(A1X_CLKEN_UART0);
}

static void cubieboard_raminit(void)
{
	struct dram_para dram_para = {
		.clock = 480,
		.type = 3,
		.rank_num = 1,
		.density = 4096,
		.io_width = 16,
		.bus_width = 32,
		.cas = 6,
		.zq = 123,
		.odt_en = 0,
		.size = 1024,
		.tpr0 = 0x30926692,
		.tpr1 = 0x1090,
		.tpr2 = 0x1a0c8,
		.tpr3 = 0,
		.tpr4 = 0,
		.tpr5 = 0,
		.emr1 = 0,
		.emr2 = 0,
		.emr3 = 0,
	};

	dramc_init(&dram_para);

	/* FIXME: ram_check does not compile for ARM,
	 * and we didn't init console yet
	 */
	////void *const test_base = (void *)A1X_DRAM_BASE;
	////ram_check((u32)test_base, (u32)test_base + 0x1000);
}

void bootblock_mainboard_early_init(void)
{
	/* A10 Timer init uses the 24MHz clock, not PLLs, so we can init it very
	 * early on to get udelay, which is used almost everywhere else.
	 */
	init_timer();

	cubieboard_setup_clocks();
	cubieboard_setup_gpios();
	cubieboard_enable_uart();
}

void bootblock_mainboard_init(void)
{
	cubieboard_raminit();
}
