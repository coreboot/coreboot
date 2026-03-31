/* SPDX-License-Identifier: GPL-2.0-only */

#include <static_devices.h>
#include <device/mmio.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <drivers/intel/gma/opregion.h>
#include <commonlib/helpers.h>
#include <types.h>

#include "drivers/intel/gma/i915_reg.h"
#include "gm965.h"
#include "chip.h"

#define GDRST 0xc0
#define MSAC  0x62 /* Multi Size Aperture Control */

/* Lenovo X61 vendor BIOS programs a 165 Hz backlight PWM frequency. */
#define GM965_DEFAULT_PWM_FREQ 165

static struct resource *gtt_res = NULL;

u32 gtt_read(u32 reg)
{
	return read32(res2mmio(gtt_res, reg, 0));
}

void gtt_write(u32 reg, u32 data)
{
	write32(res2mmio(gtt_res, reg, 0), data);
}

/*
 * get_cdclk() - Compute the core display clock frequency in Hz.
 *
 * GM965 (Crestline) CDCLK derivation from Linux i915 i965gm_get_cdclk().
 * The VCO frequency is read from HPLLVCO, the CDCLK divisor selector
 * from GCFGC bits [12:8].
 */
static u32 get_cdclk(struct device *const dev)
{
	/*
	 * Crestline VCO table, indexed by HPLLVCO[2:0].
	 * Values in kHz from Linux i915 cl_vco[].
	 */
	static const u32 cl_vco_khz[] = {
		3200000, 4000000, 5333333, 6400000, 3333333, 3566667, 4266667,
	};

	/* CDCLK divider tables from Linux, indexed by cdclk_sel. */
	static const u8 div_3200[] = {16, 10, 8};
	static const u8 div_4000[] = {20, 12, 10};
	static const u8 div_5333[] = {24, 16, 14};

	const u8 hpll_idx = mchbar_read8(HPLLVCO_MCHBAR) & 0x7;
	const u16 gcfgc = pci_read_config16(dev, GCFGC_OFFSET);
	const unsigned int cdclk_sel = ((gcfgc >> 8) & 0x1f) - 1;
	const u8 *div_table;
	u32 vco_khz;

	if (hpll_idx >= ARRAY_SIZE(cl_vco_khz))
		return 200000000;

	vco_khz = cl_vco_khz[hpll_idx];

	if (cdclk_sel > 2)
		return 200000000;

	switch (vco_khz) {
	case 3200000:
		div_table = div_3200;
		break;
	case 4000000:
		div_table = div_4000;
		break;
	case 5333333:
		div_table = div_5333;
		break;
	default:
		return 200000000;
	}

	/* VCO is in kHz, multiply by 1000 to get Hz, then divide. */
	return (vco_khz / div_table[cdclk_sel]) * 1000;
}

static u32 freq_to_blc_pwm_ctl(struct device *const dev, u16 pwm_freq, u8 duty_perc)
{
	const u32 blc_mod = get_cdclk(dev) / (128 * pwm_freq);

	if (duty_perc <= 100)
		return (blc_mod << 16) | (blc_mod * duty_perc / 100);
	else
		return (blc_mod << 16) | blc_mod;
}

static void gma_init_panel_power_and_backlight(struct device *const dev)
{
	const struct northbridge_intel_gm965_config *const conf = dev->chip_info;

	u32 reg32;
	u16 pwm_freq;

	/* Setup Panel Power On Delays */
	reg32 = gtt_read(PP_ON_DELAYS);
	if (!reg32) {
		reg32 = (conf->gpu_panel_power_up_delay & 0x1fff) << 16;
		reg32 |= (conf->gpu_panel_power_backlight_on_delay & 0x1fff);
		gtt_write(PP_ON_DELAYS, reg32);
	}

	/* Setup Panel Power Off Delays */
	reg32 = gtt_read(PP_OFF_DELAYS);
	if (!reg32) {
		reg32 = (conf->gpu_panel_power_down_delay & 0x1fff) << 16;
		reg32 |= (conf->gpu_panel_power_backlight_off_delay & 0x1fff);
		gtt_write(PP_OFF_DELAYS, reg32);
	}

	/* Setup Panel Power Cycle Delay */
	if (conf->gpu_panel_power_cycle_delay) {
		reg32 = (get_cdclk(dev) / 20000 - 1) << PP_REFERENCE_DIVIDER_SHIFT;
		reg32 |= conf->gpu_panel_power_cycle_delay & 0x1f;
		gtt_write(PP_DIVISOR, reg32);
	}

	/* Enable Backlight */
	gtt_write(BLC_PWM_CTL2, (1 << 31));
	u8 reg8 = 100;
	if (conf->duty_cycle != 0)
		reg8 = conf->duty_cycle;
	pwm_freq = conf->default_pwm_freq;

	if (pwm_freq == 0)
		pwm_freq = GM965_DEFAULT_PWM_FREQ;

	gtt_write(BLC_PWM_CTL, freq_to_blc_pwm_ctl(dev, pwm_freq, reg8));
}

static void gma_func0_init(struct device *dev)
{
	/* Probe MMIO resource first. It's needed even for
	   intel_gma_init_igd_opregion() which may call back. */
	gtt_res = probe_resource(dev, PCI_BASE_ADDRESS_0);
	if (!gtt_res)
		return;

	intel_gma_init_igd_opregion();

	/* Unconditionally reset graphics */
	pci_write_config8(dev, GDRST, 1);
	udelay(50);
	pci_write_config8(dev, GDRST, 0);
	/* wait for device to finish */
	while (pci_read_config8(dev, GDRST) & 1)
		;

	if (!CONFIG(NO_GFX_INIT))
		pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER);

	/* PCI Init, will run VBIOS */
	pci_dev_init(dev);
	gma_init_panel_power_and_backlight(dev);
}

/* This doesn't reclaim stolen UMA memory, but IGD could still
   be re-enabled later. */
static void gma_func0_disable(struct device *dev)
{
	struct device *dev_host = __pci_0_00_0;

	pci_write_config16(dev, GCFGC_OFFSET, 0xa00);
	pci_write_config16(dev_host, D0F0_GGC, (1 << 1));

	pci_and_config32(dev_host, D0F0_DEVEN, ~(DEVEN_D2F0 | DEVEN_D2F1));

	dev->enabled = 0;
}

static void gma_func1_init(struct device *dev)
{
	if (!CONFIG(NO_GFX_INIT))
		pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER);
}

static void gma_generate_ssdt(const struct device *device)
{
	const struct northbridge_intel_gm965_config *chip = device->chip_info;

	drivers_intel_gma_displays_ssdt_generate(&chip->gfx);
}

static void gma_func0_read_resources(struct device *dev)
{
	/* Set Untrusted Aperture Size to 256MB */
	pci_update_config8(dev, MSAC, ~0x3, 0x2);

	pci_dev_read_resources(dev);
}

static const char *gma_acpi_name(const struct device *dev)
{
	return "GFX0";
}

static struct device_operations gma_func0_ops = {
	.read_resources		= gma_func0_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= gma_func0_init,
	.acpi_fill_ssdt		= gma_generate_ssdt,
	.vga_disable		= gma_func0_disable,
	.ops_pci		= &pci_dev_ops_pci,
	.acpi_name		= gma_acpi_name,
};

static struct device_operations gma_func1_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= gma_func1_init,
	.ops_pci		= &pci_dev_ops_pci,
};

static const unsigned short gm965_gma_func0_ids[] = {
	PCI_DID_GM965_IGD,   /* 0x2A02 - Mobile Intel GM965/GL960 Express Integrated Graphics */
	0
};

static const unsigned short gm965_gma_func1_ids[] = {
	PCI_DID_GM965_IGD_1, /* 0x2A03 - Mobile Intel GM965/GL960 Express Integrated Graphics (alt) */
	0
};

static const struct pci_driver gm965_gma_func0_driver __pci_driver = {
	.ops		= &gma_func0_ops,
	.vendor		= PCI_VID_INTEL,
	.devices	= gm965_gma_func0_ids,
};

static const struct pci_driver gm965_gma_func1_driver __pci_driver = {
	.ops		= &gma_func1_ops,
	.vendor		= PCI_VID_INTEL,
	.devices	= gm965_gma_func1_ids,
};
