/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <stdint.h>
#include <commonlib/helpers.h>
#include <device/device.h>
#include <device/mmio.h>
#include <device/pci_def.h>
#include <intelblocks/graphics.h>
#include <drivers/intel/gma/i915_reg.h>

#include "chip.h"

static void graphics_configure_panelpower(
		const struct soc_intel_apl_pp *const pp,
		uint8_t *const mmio, const unsigned int panel_idx)
{
	const unsigned int offset = panel_idx * 0x100;
	uint32_t reg32;

	reg32 = (DIV_ROUND_UP(pp->cycle_delay_ms, 100) + 1) << 4 & 0x1f0;
	reg32 |= PANEL_POWER_RESET;
	write32(mmio + PCH_PP_CONTROL + offset, reg32);

	reg32 = pp->up_delay_ms * 10 << 16;
	reg32 |= pp->backlight_on_delay_ms * 10;
	write32(mmio + PCH_PP_ON_DELAYS + offset, reg32);

	reg32 = pp->down_delay_ms * 10 << 16;
	reg32 |= pp->backlight_off_delay_ms * 10;
	write32(mmio + PCH_PP_OFF_DELAYS + offset, reg32);
}

static void graphics_configure_backlight(
		const struct soc_intel_apl_blc *const blc,
		uint8_t *const mmio, const unsigned int panel_idx)
{
	if (!blc->pwm_hz)
		return;

	const unsigned int pwm_period = 19200 * 1000 / blc->pwm_hz;
	write32(mmio + BXT_BLC_PWM_FREQ(panel_idx), pwm_period);
	write32(mmio + BXT_BLC_PWM_DUTY(panel_idx), pwm_period / 2);
	write32(mmio + BXT_BLC_PWM_CTL(panel_idx),
		(blc->polarity ? BXT_BLC_PWM_POLARITY : 0));

	/* Second backlight control uses display utility pin. */
	if (panel_idx == 1) {
		write32(mmio + UTIL_PIN_CTL, 0); /* Make sure it's disabled, don't know
						    what FSP might have done already. */
		write32(mmio + UTIL_PIN_CTL, UTIL_PIN_MODE_PWM | UTIL_PIN_ENABLE);
	}
}

void graphics_soc_init(struct device *const dev)
{
	const struct soc_intel_apollolake_config *const conf = dev->chip_info;
	const struct resource *mmio_res;
	void *mmio;
	unsigned int i;

	/* Some hardware configuration first. */

	if (!conf)
		return;

	mmio_res = probe_resource(dev, PCI_BASE_ADDRESS_0);
	if (!mmio_res || !mmio_res->base)
		return;
	mmio = (void *)(uintptr_t)mmio_res->base;

	for (i = 0; i < ARRAY_SIZE(conf->gpu_pp); ++i)
		graphics_configure_panelpower(&conf->gpu_pp[i], mmio, i);

	for (i = 0; i < ARRAY_SIZE(conf->gpu_blc); ++i)
		graphics_configure_backlight(&conf->gpu_blc[i], mmio, i);
}
