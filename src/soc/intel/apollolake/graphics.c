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
		const struct i915_gpu_panel_config *const panel_cfg,
		uint8_t *const mmio, const unsigned int panel_idx)
{
	const unsigned int offset = panel_idx * 0x100;
	uint32_t reg32;

	reg32 = ((DIV_ROUND_UP(panel_cfg->cycle_delay_ms, 100) + 1) & 0x1f) << 4;
	reg32 |= PANEL_POWER_RESET;
	write32(mmio + PCH_PP_CONTROL + offset, reg32);

	reg32 = ((panel_cfg->up_delay_ms * 10) & 0x1fff) << 16;
	reg32 |= (panel_cfg->backlight_on_delay_ms * 10) & 0x1fff;
	write32(mmio + PCH_PP_ON_DELAYS + offset, reg32);

	reg32 = ((panel_cfg->down_delay_ms * 10) & 0x1fff) << 16;
	reg32 |= (panel_cfg->backlight_off_delay_ms * 10) & 0x1fff;
	write32(mmio + PCH_PP_OFF_DELAYS + offset, reg32);
}

static void graphics_configure_backlight(
		const struct i915_gpu_panel_config *const panel_cfg,
		uint8_t *const mmio, const unsigned int panel_idx)
{
	if (!panel_cfg->backlight_pwm_hz)
		return;

	const unsigned int pwm_period = 19200 * 1000 / panel_cfg->backlight_pwm_hz;
	write32(mmio + BXT_BLC_PWM_FREQ(panel_idx), pwm_period);
	write32(mmio + BXT_BLC_PWM_DUTY(panel_idx), pwm_period / 2);
	write32(mmio + BXT_BLC_PWM_CTL(panel_idx),
		panel_cfg->backlight_polarity ? BXT_BLC_PWM_POLARITY : 0);

	/* Second backlight control uses display utility pin. */
	if (panel_idx == 1) {
		write32(mmio + UTIL_PIN_CTL, 0); /* Make sure it's disabled, don't know
						    what FSP might have done already. */
		write32(mmio + UTIL_PIN_CTL, UTIL_PIN_MODE_PWM | UTIL_PIN_ENABLE);
	}
}

void graphics_soc_panel_init(struct device *const dev)
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

	for (i = 0; i < ARRAY_SIZE(conf->panel_cfg); ++i)
		graphics_configure_panelpower(&conf->panel_cfg[i], mmio, i);

	for (i = 0; i < ARRAY_SIZE(conf->panel_cfg); ++i)
		graphics_configure_backlight(&conf->panel_cfg[i], mmio, i);
}

const struct i915_gpu_controller_info *
intel_igd_get_controller_info(const struct device *device)
{
	struct soc_intel_apollolake_config *chip = device->chip_info;
	return &chip->gfx;
}
