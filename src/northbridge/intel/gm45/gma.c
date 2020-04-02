/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <device/mmio.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <drivers/intel/gma/edid.h>
#include <drivers/intel/gma/opregion.h>
#include <drivers/intel/gma/libgfxinit.h>
#include <string.h>
#include <device/pci_ops.h>
#include <commonlib/helpers.h>
#include <cbmem.h>
#include <southbridge/intel/i82801ix/nvs.h>
#include <types.h>

#include "drivers/intel/gma/i915_reg.h"
#include "chip.h"
#include "gm45.h"

static struct resource *gtt_res = NULL;

u32 gtt_read(u32 reg)
{
	return read32(res2mmio(gtt_res, reg, 0));
}

void gtt_write(u32 reg, u32 data)
{
	write32(res2mmio(gtt_res, reg, 0), data);
}

uintptr_t gma_get_gnvs_aslb(const void *gnvs)
{
	const global_nvs_t *gnvs_ptr = gnvs;
	return (uintptr_t)(gnvs_ptr ? gnvs_ptr->aslb : 0);
}

void gma_set_gnvs_aslb(void *gnvs, uintptr_t aslb)
{
	global_nvs_t *gnvs_ptr = gnvs;
	if (gnvs_ptr)
		gnvs_ptr->aslb = aslb;
}

static u32 get_cdclk(struct device *const dev)
{
	const u16 cdclk_sel =
		pci_read_config16 (dev, GCFGC_OFFSET) & GCFGC_CD_MASK;
	switch (MCHBAR8(HPLLVCO_MCHBAR) & 0x7) {
	case VCO_2666:
	case VCO_4000:
	case VCO_5333:
		return cdclk_sel ? 333333333 : 222222222;
	case VCO_3200:
		return cdclk_sel ? 320000000 : 228571429;
	default:
		printk(BIOS_WARNING,
		       "Unknown VCO frequency, using default cdclk.\n");
		return 222222222;
	}
}

static u32 freq_to_blc_pwm_ctl(struct device *const dev,
			u16 pwm_freq, u8 duty_perc)
{
	u32 blc_mod;

	blc_mod = get_cdclk(dev) / (128 * pwm_freq);

	if (duty_perc <= 100)
		return (blc_mod << 16) | (blc_mod * duty_perc / 100);
	else
		return (blc_mod << 16) | blc_mod;
}

u16 get_blc_pwm_freq_value(const char *edid_ascii_string)
{
	static u16 blc_pwm_freq;
	const struct blc_pwm_t *blc_pwm;
	int i;
	int blc_array_len;

	if (blc_pwm_freq > 0)
		return blc_pwm_freq;

	blc_array_len = get_blc_values(&blc_pwm);
	/* Find EDID string and pwm freq in lookup table */
	for (i = 0; i < blc_array_len; i++) {
		if (!strcmp(blc_pwm[i].ascii_string, edid_ascii_string)) {
			blc_pwm_freq = blc_pwm[i].pwm_freq;
			printk(BIOS_DEBUG, "Found EDID string: %s in lookup table, pwm: %dHz\n",
			       blc_pwm[i].ascii_string, blc_pwm_freq);
			break;
		}
	}

	if (i == blc_array_len)
		printk(BIOS_NOTICE, "Your panels EDID `%s` wasn't found in the"
		       "lookup table.\n You may have issues with your panels"
		       "backlight.\n If you want to help improving coreboot"
		       "please report: this EDID string\n and the result"
		       "of `intel_read read BLC_PWM_CTL`"
		       "(from intel-gpu-tools)\n while running vendor BIOS\n",
		       edid_ascii_string);

	return blc_pwm_freq;
}

static void gma_pm_init_post_vbios(struct device *const dev,
				const char *edid_ascii_string)
{
	const struct northbridge_intel_gm45_config *const conf = dev->chip_info;

	u32 reg32;
	u8 reg8;
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
		reg32 = (get_cdclk(dev) / 20000 - 1)
			<< PP_REFERENCE_DIVIDER_SHIFT;
		reg32 |= conf->gpu_panel_power_cycle_delay & 0x1f;
		gtt_write(PP_DIVISOR, reg32);
	}

	/* Enable Backlight  */
	gtt_write(BLC_PWM_CTL2, (1 << 31));
	reg8 = 100;
	if (conf->duty_cycle != 0)
		reg8 = conf->duty_cycle;
	pwm_freq = get_blc_pwm_freq_value(edid_ascii_string);
	if (pwm_freq == 0 && conf->default_pwm_freq != 0)
		pwm_freq = conf->default_pwm_freq;

	if (pwm_freq == 0)
		gtt_write(BLC_PWM_CTL, 0x06100610);
	else
		gtt_write(BLC_PWM_CTL, freq_to_blc_pwm_ctl(dev, pwm_freq,
								reg8));
}

static void gma_func0_init(struct device *dev)
{
	u32 reg32;
	u8 *mmio;
	u8 edid_data_lvds[128];
	struct edid edid_lvds;
	const struct northbridge_intel_gm45_config *const conf = dev->chip_info;

	/* IGD needs to be Bus Master */
	reg32 = pci_read_config32(dev, PCI_COMMAND);
	reg32 |= PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY | PCI_COMMAND_IO;
	pci_write_config32(dev, PCI_COMMAND, reg32);

	gtt_res = find_resource(dev, PCI_BASE_ADDRESS_0);
	if (gtt_res == NULL)
		return;
	mmio = res2mmio(gtt_res, 0, 0);


	if (!CONFIG(MAINBOARD_USE_LIBGFXINIT)) {
		/* PCI Init, will run VBIOS */
		printk(BIOS_DEBUG, "Initialising IGD using VBIOS\n");
		pci_dev_init(dev);
	}

	printk(BIOS_DEBUG, "LVDS EDID\n");
	intel_gmbus_read_edid(mmio + GMBUS0, GMBUS_PORT_PANEL, 0x50,
			edid_data_lvds, sizeof(edid_data_lvds));
	intel_gmbus_stop(mmio + GMBUS0);
	decode_edid(edid_data_lvds, sizeof(edid_data_lvds), &edid_lvds);

	/* Post VBIOS init */
	gma_pm_init_post_vbios(dev, edid_lvds.ascii_string);

	if (CONFIG(MAINBOARD_USE_LIBGFXINIT) && !acpi_is_wakeup_s3()) {
		int vga_disable = (pci_read_config16(dev, D0F0_GGC) & 2) >> 1;
		if (vga_disable) {
			printk(BIOS_INFO,
			       "IGD is not decoding legacy VGA MEM and IO: skipping NATIVE graphic init\n");
		} else {
			int lightup_ok;
			gma_gfxinit(&lightup_ok);
			/* Linux relies on VBT for panel info. */
			generate_fake_intel_oprom(&conf->gfx, dev, "$VBT CANTIGA");
		}
	}

	intel_gma_restore_opregion();
}

static void gma_generate_ssdt(struct device *device)
{
	const struct northbridge_intel_gm45_config *chip = device->chip_info;

	drivers_intel_gma_displays_ssdt_generate(&chip->gfx);
}

static unsigned long
gma_write_acpi_tables(struct device *const dev,
		      unsigned long current,
		      struct acpi_rsdp *const rsdp)
{
	igd_opregion_t *opregion = (igd_opregion_t *)current;
	global_nvs_t *gnvs;

	if (intel_gma_init_igd_opregion(opregion) != CB_SUCCESS)
		return current;

	current += sizeof(igd_opregion_t);

	/* GNVS has been already set up */
	gnvs = cbmem_find(CBMEM_ID_ACPI_GNVS);
	if (gnvs) {
		/* IGD OpRegion Base Address */
		gma_set_gnvs_aslb(gnvs, (uintptr_t)opregion);
	} else {
		printk(BIOS_ERR, "Error: GNVS table not found.\n");
	}

	current = acpi_align_current(current);
	return current;
}

static const char *gma_acpi_name(const struct device *dev)
{
	return "GFX0";
}

static struct pci_operations gma_pci_ops = {
	.set_subsystem = pci_dev_set_subsystem,
};

static struct device_operations gma_func0_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.acpi_fill_ssdt		= gma_generate_ssdt,
	.init			= gma_func0_init,
	.ops_pci		= &gma_pci_ops,
	.acpi_name		= gma_acpi_name,
	.write_acpi_tables	= gma_write_acpi_tables,
};

static const unsigned short pci_device_ids[] =
{
	0x2a42, 0
};

static const struct pci_driver gma __pci_driver = {
	.ops = &gma_func0_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
