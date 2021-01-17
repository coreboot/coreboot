/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/pci.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/rtc.h>
#include <soc/gpio.h>
#include <soc/pcr_ids.h>
#include <soc/pm.h>
#include "chip.h"

static const struct pad_config lpc_gpios[] = {
#if CONFIG(SOC_INTEL_GEMINILAKE)
#if !CONFIG(SOC_ESPI)
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
#endif /* !CONFIG(SOC_ESPI) */
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
	cfg = config_of(dev);

	/* Set LPC Serial IRQ mode */
	lpc_set_serirq_mode(cfg->serirq_mode);

	/* Initialize RTC */
	rtc_init();
}
