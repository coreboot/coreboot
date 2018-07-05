/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017-2018 Intel Corp.
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

#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/rtc.h>
#include <soc/gpio.h>
#include <soc/pcr_ids.h>
#include <soc/pm.h>
#include "chip.h"

static const struct lpc_mmio_range apl_lpc_fixed_mmio_ranges[] = {
	{ 0xfed40000, 0x8000 },
	{ 0xfedc0000, 0x4000 },
	{ 0xfed20800, 16 },
	{ 0xfed20880, 8 },
	{ 0xfed208e0, 16 },
	{ 0xfed208f0, 8 },
	{ 0xfed30800, 16 },
	{ 0xfed30880, 8 },
	{ 0xfed308e0, 16 },
	{ 0xfed308f0, 8 },
	{ 0, 0 }
};

const struct lpc_mmio_range *soc_get_fixed_mmio_ranges(void)
{
	return apl_lpc_fixed_mmio_ranges;
}

static const struct pad_config lpc_gpios[] = {
#if IS_ENABLED(CONFIG_SOC_INTEL_GLK)
#if !IS_ENABLED(CONFIG_SOC_ESPI)
	PAD_CFG_NF(GPIO_147, UP_20K, DEEP, NF1), /* LPC_ILB_SERIRQ */
	PAD_CFG_NF_IOSSTATE_IOSTERM(GPIO_148, NONE, DEEP, NF1, HIZCRx1,
		DISPUPD), /* LPC_CLKOUT0 */
	PAD_CFG_NF_IOSSTATE_IOSTERM(GPIO_149, NONE, DEEP, NF1, HIZCRx1,
		DISPUPD), /* LPC_CLKOUT1 */
	PAD_CFG_NF_IOSSTATE_IOSTERM(GPIO_150, UP_20K, DEEP, NF1, HIZCRx1,
		DISPUPD), /* LPC_AD0 */
	PAD_CFG_NF_IOSSTATE_IOSTERM(GPIO_151, UP_20K, DEEP, NF1, HIZCRx1,
		DISPUPD), /* LPC_AD1 */
	PAD_CFG_NF_IOSSTATE_IOSTERM(GPIO_152, UP_20K, DEEP, NF1, HIZCRx1,
		DISPUPD), /* LPC_AD2 */
	PAD_CFG_NF_IOSSTATE_IOSTERM(GPIO_153, UP_20K, DEEP, NF1, HIZCRx1,
		DISPUPD), /* LPC_AD3 */
	PAD_CFG_NF(GPIO_154, UP_20K, DEEP, NF1), /* LPC_CLKRUNB */
	PAD_CFG_NF_IOSSTATE_IOSTERM(GPIO_155, UP_20K, DEEP, NF1, HIZCRx1,
		DISPUPD), /* LPC_FRAMEB */
#else
	/*
	 * LPC_CLKRUNB should be in GPIO mode for eSPI. Other pin settings
	 * i.e. Rx path enable/disable, Tx path enable/disable, pull up
	 * enable/disable etc are ignored. Leaving this pin in Native mode
	 * will keep LPC Controller awake and prevent S0ix entry
	 */
	PAD_NC(GPIO_154, NONE),
#endif /* !IS_ENABLED(CONFIG_SOC_ESPI) */
#else
	PAD_CFG_NF(LPC_ILB_SERIRQ, UP_20K, DEEP, NF1),
	PAD_CFG_NF(LPC_CLKRUNB, UP_20K, DEEP, NF1),
	PAD_CFG_NF(LPC_AD0, UP_20K, DEEP, NF1),
	PAD_CFG_NF(LPC_AD1, UP_20K, DEEP, NF1),
	PAD_CFG_NF(LPC_AD2, UP_20K, DEEP, NF1),
	PAD_CFG_NF(LPC_AD3, UP_20K, DEEP, NF1),
	PAD_CFG_NF(LPC_FRAMEB, NATIVE, DEEP, NF1),
	PAD_CFG_NF(LPC_CLKOUT0, UP_20K, DEEP, NF1),
	PAD_CFG_NF(LPC_CLKOUT1, UP_20K, DEEP, NF1)
#endif
};

void lpc_configure_pads(void)
{
	gpio_configure_pads(lpc_gpios, ARRAY_SIZE(lpc_gpios));
}

void lpc_soc_init(struct device *dev)
{
	const struct soc_intel_apollolake_config *cfg;

	cfg = dev->chip_info;
	if (!cfg) {
		printk(BIOS_ERR, "BUG! Could not find SOC devicetree config\n");
		return;
	}

	/* Set LPC Serial IRQ mode */
	lpc_set_serirq_mode(cfg->serirq_mode);

	/* Initialize RTC */
	rtc_init();
}
