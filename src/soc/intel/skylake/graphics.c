/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/helpers.h>
#include <device/mmio.h>
#include <device/pci_rom.h>
#include <device/resource.h>
#include <drivers/intel/gma/i915.h>
#include <drivers/intel/gma/i915_reg.h>
#include <intelblocks/graphics.h>
#include <soc/ramstage.h>
#include <soc/systemagent.h>
#include <types.h>

void graphics_soc_panel_init(struct device *dev)
{
	struct soc_intel_skylake_config *conf = config_of(dev);
	const struct i915_gpu_panel_config *panel_cfg;
	struct resource *mmio_res;
	uint8_t *base;
	u32 reg32;

	if (!conf)
		return;

	panel_cfg = &conf->panel_cfg;

	mmio_res = probe_resource(dev, PCI_BASE_ADDRESS_0);
	if (!mmio_res || !mmio_res->base)
		return;
	base = (void *)(uintptr_t)mmio_res->base;

	reg32 = ((panel_cfg->up_delay_ms * 10) & 0x1fff) << 16;
	reg32 |= (panel_cfg->backlight_on_delay_ms * 10) & 0x1fff;
	write32(base + PCH_PP_ON_DELAYS, reg32);

	reg32 = ((panel_cfg->down_delay_ms * 10) & 0x1fff) << 16;
	reg32 |= (panel_cfg->backlight_off_delay_ms * 10) & 0x1fff;
	write32(base + PCH_PP_OFF_DELAYS, reg32);

	reg32 = read32(base + PCH_PP_DIVISOR);
	reg32 &= ~0x1f;
	reg32 |= (DIV_ROUND_UP(panel_cfg->cycle_delay_ms, 100) + 1) & 0x1f;
	write32(base + PCH_PP_DIVISOR, reg32);

	/* So far all devices seem to use the PCH PWM function.
	   The CPU PWM registers are all zero after reset.      */
	if (panel_cfg->backlight_pwm_hz) {
		/* Reference clock is 24MHz. We can choose either a 16
		   or a 128 step increment. Use 16 if we would have less
		   than 100 steps otherwise. */
		const unsigned int hz_limit = 24 * 1000 * 1000 / 128 / 100;
		unsigned int pwm_increment, pwm_period;
		u32 south_chicken1;

		south_chicken1 = read32(base + SOUTH_CHICKEN1);
		if (panel_cfg->backlight_pwm_hz > hz_limit) {
			pwm_increment = 16;
			south_chicken1 &= ~1;
		} else {
			pwm_increment = 128;
			south_chicken1 |= 1;
		}
		write32(base + SOUTH_CHICKEN1, south_chicken1);

		pwm_period = 24 * 1000 * 1000 / pwm_increment / panel_cfg->backlight_pwm_hz;
		/* Start with a 50% duty cycle. */
		write32(base + BLC_PWM_PCH_CTL2, pwm_period << 16 | pwm_period / 2);

		write32(base + BLC_PWM_PCH_CTL1,
			!!panel_cfg->backlight_polarity << 29 | BLM_PCH_PWM_ENABLE);
	}
}

const struct i915_gpu_controller_info *
intel_igd_get_controller_info(const struct device *device)
{
	struct soc_intel_skylake_config *chip = device->chip_info;
	return &chip->gfx;
}

/*
 * Some VGA option roms are used for several chipsets but they only have one PCI ID in their
 * header. If we encounter such an option rom, we need to do the mapping ourselves.
 */
u32 map_oprom_vendev(u32 vendev)
{
	u32 new_vendev = vendev;

	switch (vendev) {
	case 0x80865916: /* PCI_DID_INTEL_KBL_GT2_SULTM */
	case 0x80865917: /* PCI_DID_INTEL_KBL_GT2_SULTMR */
		new_vendev = SA_IGD_OPROM_VENDEV;
		break;
	}

	return new_vendev;
}
