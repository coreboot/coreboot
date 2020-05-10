/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/mmu.h>
#include <arch/stages.h>
#include <delay.h>
#include <soc/mmu_operations.h>
#include <soc/tsadc.h>
#include <soc/sdram.h>
#include <symbols.h>
#include <soc/usb.h>

#include "board.h"
#include "pwm_regulator.h"

static void init_dvs_outputs(void)
{
	pwm_regulator_configure(PWM_REGULATOR_GPU, 900);
	pwm_regulator_configure(PWM_REGULATOR_BIG, 900);

	/*
	 * Kevin's logic rail has some ripple, so up the voltage a bit. Newer
	 * boards use a fixed 900mV regulator for centerlogic.
	 */
	if (CONFIG(BOARD_GOOGLE_KEVIN))
		pwm_regulator_configure(PWM_REGULATOR_CENTERLOG, 925);
	else if (CONFIG(GRU_HAS_CENTERLOG_PWM))
		pwm_regulator_configure(PWM_REGULATOR_CENTERLOG, 900);

	/* Allow time for the regulators to settle */
	udelay(500);
}

static void prepare_sdmmc(void)
{
	/* Enable main SD rail early to allow ramp time before powering SDIO. */
	gpio_output(GPIO_SDMMC_PWR, 1);
}

static void prepare_usb(void)
{
	/*
	 * Do dwc3 core soft reset and phy reset. Kick these resets
	 * off early so they get at least 100ms to settle.
	 */
	reset_usb_otg0();
	reset_usb_otg1();
}

void platform_romstage_main(void)
{
	tsadc_init(TSHUT_POL_HIGH);

	/* Init DVS to conservative values. */
	init_dvs_outputs();

	prepare_sdmmc();
	prepare_usb();

	sdram_init(get_sdram_config());

	mmu_config_range((void *)0, (uintptr_t)sdram_size_mb() * MiB,
			 CACHED_MEM);
	mmu_config_range(_dma_coherent, REGION_SIZE(dma_coherent),
			 UNCACHED_MEM);
}
