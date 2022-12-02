/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <drivers/intel/gma/i915_reg.h>
#include <intelblocks/early_graphics.h>
#include <soc/soc_chip.h>

void early_graphics_soc_panel_init(void)
{
	const struct soc_intel_alderlake_config *soc_conf;
	const struct i915_gpu_panel_config *panel_cfg;
	void *mmio = (void *)CONFIG_GFX_GMA_DEFAULT_MMIO;
	uint32_t reg32;
	unsigned int pwm_period, pwm_polarity, pwm_duty;

	soc_conf = config_of_soc();
	panel_cfg = &soc_conf->panel_cfg;

	reg32 = ((DIV_ROUND_UP(panel_cfg->cycle_delay_ms, 100) + 1) & 0x1f) << 4;
	reg32 |= PANEL_POWER_RESET;
	write32(mmio + PCH_PP_CONTROL, reg32);

	reg32 = ((panel_cfg->up_delay_ms * 10) & 0x1fff) << 16;
	reg32 |= (panel_cfg->backlight_on_delay_ms * 10) & 0x1fff;
	write32(mmio + PCH_PP_ON_DELAYS, reg32);

	reg32 = ((panel_cfg->down_delay_ms * 10) & 0x1fff) << 16;
	reg32 |= (panel_cfg->backlight_off_delay_ms * 10) & 0x1fff;
	write32(mmio + PCH_PP_OFF_DELAYS, reg32);

	if (!panel_cfg->backlight_pwm_hz)
		return;

	/* Configure backlight */
	pwm_polarity = panel_cfg->backlight_polarity ? BXT_BLC_PWM_POLARITY : 0;
	pwm_period = DIV_ROUND_CLOSEST(CONFIG_CPU_XTAL_HZ,
				       panel_cfg->backlight_pwm_hz);
	pwm_duty = DIV_ROUND_CLOSEST(pwm_period, 2); /* Start with 50 % */
	write32(mmio + BXT_BLC_PWM_FREQ(0), pwm_period);
	write32(mmio + BXT_BLC_PWM_CTL(0),  pwm_polarity);
	write32(mmio + BXT_BLC_PWM_DUTY(0), pwm_duty);
}
