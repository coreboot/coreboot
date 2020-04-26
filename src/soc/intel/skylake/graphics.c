/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/resource.h>
#include <drivers/intel/gma/i915.h>
#include <drivers/intel/gma/i915_reg.h>
#include <drivers/intel/gma/libgfxinit.h>
#include <intelblocks/graphics.h>
#include <drivers/intel/gma/opregion.h>
#include <soc/ramstage.h>
#include <types.h>

uintptr_t fsp_soc_get_igd_bar(void)
{
	return graphics_get_memory_base();
}

static void graphics_setup_panel(struct device *dev)
{
	struct soc_intel_skylake_config *conf = config_of(dev);
	struct resource *mmio_res;
	uint8_t *base;
	u32 reg32;

	if (!conf)
		return;

	mmio_res = find_resource(dev, PCI_BASE_ADDRESS_0);
	if (!mmio_res || !mmio_res->base)
		return;
	base = (void *)(uintptr_t)mmio_res->base;

	reg32 = conf->gpu_pp_up_delay_ms * 10 << 16;
	reg32 |= conf->gpu_pp_backlight_on_delay_ms * 10;
	write32(base + PCH_PP_ON_DELAYS, reg32);

	reg32 = conf->gpu_pp_down_delay_ms * 10 << 16;
	reg32 |= conf->gpu_pp_backlight_off_delay_ms * 10;
	write32(base + PCH_PP_OFF_DELAYS, reg32);

	reg32 = read32(base + PCH_PP_DIVISOR);
	reg32 &= ~0x1f;
	reg32 |= (DIV_ROUND_UP(conf->gpu_pp_cycle_delay_ms, 100) + 1) & 0x1f;
	write32(base + PCH_PP_DIVISOR, reg32);

	/* So far all devices seem to use the PCH PWM function.
	   The CPU PWM registers are all zero after reset.      */
	if (conf->gpu_pch_backlight_pwm_hz) {
		/* Reference clock is 24MHz. We can choose either a 16
		   or a 128 step increment. Use 16 if we would have less
		   than 100 steps otherwise. */
		const unsigned int hz_limit = 24 * 1000 * 1000 / 128 / 100;
		unsigned int pwm_increment, pwm_period;
		u32 south_chicken1;

		south_chicken1 = read32(base + SOUTH_CHICKEN1);
		if (conf->gpu_pch_backlight_pwm_hz > hz_limit) {
			pwm_increment = 16;
			south_chicken1 &= ~1;
		} else {
			pwm_increment = 128;
			south_chicken1 |= 1;
		}
		write32(base + SOUTH_CHICKEN1, south_chicken1);

		pwm_period = 24 * 1000 * 1000 / pwm_increment
				/ conf->gpu_pch_backlight_pwm_hz;
		/* Start with a 50% duty cycle. */
		write32(base + BLC_PWM_PCH_CTL2,
			pwm_period << 16 | pwm_period / 2);

		write32(base + BLC_PWM_PCH_CTL1,
			!!conf->gpu_pch_backlight_polarity << 29 |
			BLM_PCH_PWM_ENABLE);
	}
}

void graphics_soc_init(struct device *dev)
{
	u32 ddi_buf_ctl;

	intel_gma_init_igd_opregion();

	graphics_setup_panel(dev);

	/*
	 * Enable DDI-A (eDP) 4-lane operation if the link is not up yet.
	 * This will allow the kernel to use 4-lane eDP links properly
	 * if the VBIOS or GOP driver does not execute.
	 */
	ddi_buf_ctl = graphics_gtt_read(DDI_BUF_CTL_A);
	if (!acpi_is_wakeup_s3() && !(ddi_buf_ctl & DDI_BUF_CTL_ENABLE)) {
		ddi_buf_ctl |= DDI_A_4_LANES;
		graphics_gtt_write(DDI_BUF_CTL_A, ddi_buf_ctl);
	}

	/* IGD needs to Bus Master */
	u32 reg32 = pci_read_config32(dev, PCI_COMMAND);
	reg32 |= PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY | PCI_COMMAND_IO;
	pci_write_config32(dev, PCI_COMMAND, reg32);

	/*
	 * GFX PEIM module inside FSP binary is taking care of graphics
	 * initialization based on RUN_FSP_GOP Kconfig option and input
	 * VBT file.
	 *
	 * In case of non-FSP solution, SoC need to select another
	 * Kconfig to perform GFX initialization.
	 */
	if (CONFIG(RUN_FSP_GOP)) {
		/* nothing to do */
	} else if (CONFIG(MAINBOARD_USE_LIBGFXINIT)) {
		if (!acpi_is_wakeup_s3() && display_init_required()) {
			int lightup_ok;
			gma_gfxinit(&lightup_ok);
			gfx_set_init_done(lightup_ok);
		}
	} else {
		/* Initialize PCI device, load/execute BIOS Option ROM */
		pci_dev_init(dev);
	}
}

const struct i915_gpu_controller_info *
intel_igd_get_controller_info(const struct device *device)
{
	struct soc_intel_skylake_config *chip = device->chip_info;
	return &chip->gfx;
}
