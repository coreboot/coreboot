/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <bootmode.h>
#include <device/mmio.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <option.h>
#include <drivers/intel/gma/gma.h>
#include <drivers/intel/gma/i915.h>
#include <drivers/intel/gma/libgfxinit.h>
#include <drivers/intel/gma/opregion.h>
#include <commonlib/helpers.h>
#include <types.h>

#include "i945.h"
#include "chip.h"

#define GDRST 0xc0
#define MSAC 0x62 /* Multi Size Aperture Control */

#define DEFAULT_BLC_PWM 180

#define PGETBL_CTL	0x2020
#define PGETBL_ENABLED	0x00000001

static int gtt_setup(u8 *mmiobase)
{
	unsigned long PGETBL_save;
	unsigned long tom; // top of memory

	/*
	 * The Video BIOS places the GTT right below top of memory.
	 */
	tom = pci_read_config8(pcidev_on_root(0, 0), TOLUD) << 24;
	PGETBL_save = tom - 256 * KiB;
	PGETBL_save |= PGETBL_ENABLED;
	PGETBL_save |= 2; /* set GTT to 256kb */

	write32(mmiobase + GFX_FLSH_CNTL, 0);

	write32(mmiobase + PGETBL_CTL, PGETBL_save);

	/* verify */
	if (read32(mmiobase + PGETBL_CTL) & PGETBL_ENABLED) {
		printk(BIOS_DEBUG, "%s is enabled.\n", __func__);
	} else {
		printk(BIOS_DEBUG, "%s failed!!!\n", __func__);
		return 1;
	}
	write32(mmiobase + GFX_FLSH_CNTL, 0);

	return 0;
}

static u32 get_cdclk(struct device *const dev)
{
	u16 gcfgc = pci_read_config16(dev, GCFGC);

	if (gcfgc & GC_LOW_FREQUENCY_ENABLE)
		return 133333333;

	switch (gcfgc & GC_DISPLAY_CLOCK_MASK) {
	case GC_DISPLAY_CLOCK_333_320_MHZ:
		return 320000000;
	default:
	case GC_DISPLAY_CLOCK_190_200_MHZ:
		return 200000000;
	}
}

static u32 freq_to_blc_pwm_ctl(struct device *const dev, u16 pwm_freq)
{
	u32 blc_mod;

	/* Set duty cycle to 100% due to use of legacy backlight control */
	blc_mod = get_cdclk(dev) / (32 * pwm_freq);
	return BLM_LEGACY_MODE | ((blc_mod / 2) << 17) | ((blc_mod / 2) << 1);
}

static void panel_setup(u8 *mmiobase, struct device *const dev)
{
	const struct northbridge_intel_i945_config *const conf = dev->chip_info;

	u32 reg32;

	/* Set up Panel Power On Delays */
	reg32 = (conf->gpu_panel_power_up_delay & 0x1fff) << 16;
	reg32 |= (conf->gpu_panel_power_backlight_on_delay & 0x1fff);
	write32(mmiobase + PP_ON_DELAYS, reg32);

	/* Set up Panel Power Off Delays */
	reg32 = (conf->gpu_panel_power_down_delay & 0x1fff) << 16;
	reg32 |= (conf->gpu_panel_power_backlight_off_delay & 0x1fff);
	write32(mmiobase + PP_OFF_DELAYS, reg32);

	/* Set up Panel Power Cycle Delay */
	reg32 = (get_cdclk(dev) / 20000 - 1) << PP_REFERENCE_DIVIDER_SHIFT;
	reg32 |= conf->gpu_panel_power_cycle_delay & 0x1f;
	write32(mmiobase + PP_DIVISOR, reg32);

	/* Backlight init. */
	if (conf->pwm_freq)
		write32(mmiobase + BLC_PWM_CTL, freq_to_blc_pwm_ctl(dev,
							conf->pwm_freq));
	else
		write32(mmiobase + BLC_PWM_CTL, freq_to_blc_pwm_ctl(dev,
							DEFAULT_BLC_PWM));
}

static void generate_fake_vbt(struct device *dev)
{
	const struct northbridge_intel_i945_config *conf = dev->chip_info;

	/* Linux relies on VBT for panel info.  */
	if (CONFIG(NORTHBRIDGE_INTEL_SUBTYPE_I945GM)) {
		generate_fake_intel_oprom(&conf->gfx, dev, "$VBT CALISTOGA");
	}
	if (CONFIG(NORTHBRIDGE_INTEL_SUBTYPE_I945GC)) {
		generate_fake_intel_oprom(&conf->gfx, dev, "$VBT LAKEPORT-G");
	}
}

static void gma_func0_init(struct device *dev)
{
	int lightup_ok = 0;

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

	if (CONFIG(MAINBOARD_USE_LIBGFXINIT)) {
		int vga_disable = (pci_read_config16(dev, GGC) & 2) >> 1;
		struct resource *mmio_res;
		u8 *mmiobase;

		if (acpi_is_wakeup_s3()) {
			printk(BIOS_INFO,
			       "Skipping libgfxinit graphics initialization when resuming from ACPI S3.\n");
			return;
		}

		if (vga_disable) {
			printk(BIOS_INFO,
			       "IGD is not decoding legacy VGA MEM and IO: skipping NATIVE graphic init\n");
			return;
		}

		generate_fake_vbt(dev);

		mmio_res = find_resource(dev, PCI_BASE_ADDRESS_0);
		if (!mmio_res || !mmio_res->base) {
			printk(BIOS_ERR, "Unable to find GMA MMIO resource.\n");
			return;
		}

		printk(BIOS_SPEW, "Initializing graphics with libgfxinit.\n");
		mmiobase = res2mmio(mmio_res, 0, 0);
		if (CONFIG(NORTHBRIDGE_INTEL_SUBTYPE_I945GM))
			panel_setup(mmiobase, dev);

		if (gtt_setup(mmiobase)) {
			printk(BIOS_ERR, "GTT setup failed; skipping libgfxinit.\n");
			return;
		}

		gma_gfxinit(&lightup_ok);
		if (lightup_ok)
			gfx_set_init_done(1);
	} else {
		/* PCI Init, will run VBIOS */
		pci_dev_init(dev);
	}
}

/* This doesn't reclaim stolen UMA memory, but IGD could still
   be re-enabled later. */
static void gma_func0_disable(struct device *dev)
{
	struct device *dev_host = pcidev_on_root(0x0, 0);

	pci_write_config16(dev, GCFC, 0xa00);
	pci_write_config16(dev_host, GGC, (1 << 1));

	pci_and_config32(dev_host, DEVEN, ~(DEVEN_D2F0 | DEVEN_D2F1));

	dev->enabled = 0;
}

static void gma_func1_init(struct device *dev)
{
	if (!CONFIG(NO_GFX_INIT))
		pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER);

	pci_write_config8(dev, 0xf4, get_uint_option("tft_brightness", 0xff));
}

static void gma_generate_ssdt(const struct device *device)
{
	const struct northbridge_intel_i945_config *chip = device->chip_info;

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

static const unsigned short i945_gma_func0_ids[] = {
	0x2772, /* 82945G/GZ Integrated Graphics Controller */
	0x27a2, /* Mobile 945GM/GMS Express Integrated Graphics Controller*/
	0x27ae, /* Mobile 945GSE Express Integrated Graphics Controller */
	0
};

static const unsigned short i945_gma_func1_ids[] = {
	0x2776, /* Desktop 82945G/GZ/GC */
	0x27a6, /* Mobile 945GM/GMS/GME Express Integrated Graphics Controller */
	0
};

static const struct pci_driver i945_gma_func0_driver __pci_driver = {
	.ops		= &gma_func0_ops,
	.vendor		= PCI_VID_INTEL,
	.devices	= i945_gma_func0_ids,
};

static const struct pci_driver i945_gma_func1_driver __pci_driver = {
	.ops		= &gma_func1_ops,
	.vendor		= PCI_VID_INTEL,
	.devices	= i945_gma_func1_ids,
};
