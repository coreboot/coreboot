/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Marvell, Inc.
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

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <arch/io.h>
#include <assert.h>
#include <console/console.h>
#include <soc/apmu.h>
#include <soc/clock.h>
#include <soc/mcu.h>
#include <soc/pmic.h>

static void syspwr_init(void)
{
	int result = 0;

	/* Start the PLLs */
	result |= apmu_set_pll(MAIN_PLL, D0, 2000);
	result |= apmu_set_pll(CPU_PLL, D0, 2400);
	result |= apmu_set_pll(MC_PLL, D0, 3200);
	result |= apmu_set_pll(MCFLC_PLL, D0, 2133);
	result |= apmu_set_pll(A2_PLL, D0, 1800);
	result |= apmu_set_pll(GPU_PLL0, D0, 2000);
	result |= apmu_set_pll(GPU_PLL1, D0, 2400);
	result |= apmu_set_pll(MIPI_PLL, D0, 2000);
	result |= apmu_set_pll(DISPLAY_PLL, D0, 1800);

	/* Start the peripheral devices */
	result |= apmu_set_dev(SDMMC, D0);
	result |= apmu_set_dev(AES256, D0);
	result |= apmu_set_dev(AP_AXI_HS, D0);
	result |= apmu_set_dev(AP_UART0, D0);
	result |= apmu_set_dev(AP_UART1, D0);
	result |= apmu_set_dev(AP_M2M, D0);
	result |= apmu_set_dev(AP_APB, D0);
	result |= apmu_set_dev(AP_GIC, D0);
	result |= apmu_set_dev(A2, D0);
	result |= apmu_set_dev(MC, D0);
	result |= apmu_set_dev(DDRPHY_0,  D0);
	result |= apmu_set_dev(DDRPHY_1,  D0);
	result |= apmu_set_dev(DDRPHY_2,  D0);
	result |= apmu_set_dev(DDRPHY_3,  D0);

	if (result)
		assert("ERRORS DURING system POWER-on");
}

void ap_start(void *entry)
{
	int result = 0;

	setbits_le32(&mvmap2315_cpu->apgencfg,
		     MVMAP2315_APGENCFG_BROADCASTINNER);
	setbits_le32(&mvmap2315_cpu->apgencfg,
		     MVMAP2315_APGENCFG_BROADCASTOUTER);
	clrbits_le32(&mvmap2315_cpu->apgencfg,
		     MVMAP2315_APGENCFG_BROADCASTCACHEMAINT);
	setbits_le32(&mvmap2315_cpu->apgencfg,
		     MVMAP2315_APGENCFG_BROADCASTCACHEMAINTPOU);
	setbits_le32(&mvmap2315_cpu->apgencfg,
		     MVMAP2315_APGENCFG_SYSBARDISABLE);

	result |= apmu_set_dev(APCPU_L2, D0);

	setbits_le32(&mvmap2315_cpu->apcorecfg0,
		     MVMAP2315_APCORECFG0_AA64NAA32);

	write32(&mvmap2315_cpu->rvbaraddr_low0, ((uintptr_t)entry) >> 2);

	write32(&mvmap2315_cpu->rvbaraddr_high0, 0);

	result |= apmu_set_dev(APCPU_0, D0);

	if (result)
		assert("ERRORS DURING AP POWER-on");
}

void no_boot(void)
{
	/*TODO: implement no_boot */
}

void charging_screen(void)
{
	/*TODO: implement charging_screen */
}

void full_boot(void)
{
	printk(BIOS_DEBUG, "Powering up the system.\n");
	syspwr_init();
}

void mcu_start(void)
{
	int result = 0;

	result |= apmu_set_pll(APLL0, D0, APLL_589P824);
	result |= apmu_set_clk(M4CLK, NOCHANGE, 4, SRCSEL_APLL0);

	if (result)
		assert("ERRORS DURING MCU POWER-on");

	clrbits_le32(&mvmap2315_mpmu_clk->resetmcu, MVMAP2315_MCU_RST_EN);
	setbits_le32(&mvmap2315_mpmu_clk->resetmcu, MVMAP2315_MCU_RST_EN);
}

u32 get_boot_path(void)
{
	struct mcu_pwr_status_msg *msg;

	mcu_irq();

	msg = receive_mcu_msg();

	return msg->status;
}
