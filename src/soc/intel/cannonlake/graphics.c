/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <commonlib/helpers.h>
#include <device/device.h>
#include <device/mmio.h>
#include <device/pci_def.h>
#include <device/resource.h>
#include <drivers/intel/gma/i915_reg.h>
#include <intelblocks/graphics.h>
#include <soc/ramstage.h>
#include <types.h>

void graphics_soc_panel_init(struct device *dev)
{
	const struct soc_intel_cannonlake_config *conf = dev->chip_info;
	const struct i915_gpu_panel_config *panel_cfg;
	const struct resource *mmio_res;
	void *mmio;
	uint32_t reg32;
	unsigned int pwm_period, pwm_polarity, pwm_duty;

	if (!conf)
		return;

	panel_cfg = &conf->panel_cfg;

	mmio_res = probe_resource(dev, PCI_BASE_ADDRESS_0);
	if (!mmio_res || !mmio_res->base)
		return;
	mmio = (void *)(uintptr_t)mmio_res->base;

	/* Panel timings */

	reg32 = ((DIV_ROUND_UP(panel_cfg->cycle_delay_ms, 100) + 1) & 0x1f) << 4;
	reg32 |= PANEL_POWER_RESET;
	write32(mmio + PCH_PP_CONTROL, reg32);

	reg32 = ((panel_cfg->up_delay_ms * 10) & 0x1fff) << 16;
	reg32 |= (panel_cfg->backlight_on_delay_ms * 10) & 0x1fff;
	write32(mmio + PCH_PP_ON_DELAYS, reg32);

	reg32 = ((panel_cfg->down_delay_ms * 10) & 0x1fff) << 16;
	reg32 |= (panel_cfg->backlight_off_delay_ms * 10) & 0x1fff;
	write32(mmio + PCH_PP_OFF_DELAYS, reg32);

	/* Backlight */
	if (panel_cfg->backlight_pwm_hz) {
		pwm_polarity = panel_cfg->backlight_polarity ? BXT_BLC_PWM_POLARITY : 0;
		pwm_period = DIV_ROUND_CLOSEST(CONFIG_CPU_XTAL_HZ, panel_cfg->backlight_pwm_hz);
		pwm_duty = DIV_ROUND_CLOSEST(pwm_period, 2); /* Start with 50 % */

		write32(mmio + BXT_BLC_PWM_FREQ(0), pwm_period);
		write32(mmio + BXT_BLC_PWM_CTL(0),  pwm_polarity);
		write32(mmio + BXT_BLC_PWM_DUTY(0), pwm_duty);
	}
}

const struct i915_gpu_controller_info *
intel_igd_get_controller_info(const struct device *const dev)
{
	const struct soc_intel_cannonlake_config *const chip = dev->chip_info;
	return &chip->gfx;
}
