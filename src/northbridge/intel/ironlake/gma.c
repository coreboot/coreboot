/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <device/mmio.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <drivers/intel/gma/edid.h>
#include <drivers/intel/gma/i915.h>
#include <drivers/intel/gma/intel_bios.h>
#include <drivers/intel/gma/libgfxinit.h>
#include <pc80/vga.h>
#include <drivers/intel/gma/opregion.h>
#include <types.h>

#include "chip.h"
#include "ironlake.h"

/* some vga option roms are used for several chipsets but they only have one
 * PCI ID in their header. If we encounter such an option rom, we need to do
 * the mapping ourselves
 */

u32 map_oprom_vendev(u32 vendev)
{
	u32 new_vendev = vendev;

	/* none currently. */

	return new_vendev;
}

static struct resource *gtt_res = NULL;

u32 gtt_read(u32 reg)
{
	return read32(res2mmio(gtt_res, reg, 0));
}

void gtt_write(u32 reg, u32 data)
{
	write32(res2mmio(gtt_res, reg, 0), data);
}

#define GTT_RETRY 1000
int gtt_poll(u32 reg, u32 mask, u32 value)
{
	unsigned int try = GTT_RETRY;
	u32 data;

	while (try--) {
		data = gtt_read(reg);
		if ((data & mask) == value)
			return 1;
		udelay(10);
	}

	printk(BIOS_ERR, "GT init timeout\n");
	return 0;
}

static void gma_pm_init_post_vbios(struct device *dev)
{
	struct northbridge_intel_ironlake_config *conf = dev->chip_info;
	u32 reg32;

	printk(BIOS_DEBUG, "GT Power Management Init (post VBIOS)\n");

	/* Setup Digital Port Hotplug */
	reg32 = gtt_read(0xc4030);
	if (!reg32) {
		reg32 = (conf->gpu_dp_b_hotplug & 0x7) << 2;
		reg32 |= (conf->gpu_dp_c_hotplug & 0x7) << 10;
		reg32 |= (conf->gpu_dp_d_hotplug & 0x7) << 18;
		gtt_write(0xc4030, reg32);
	}

	/* Setup Panel Power On Delays */
	reg32 = gtt_read(0xc7208);
	if (!reg32) {
		reg32 = (conf->gpu_panel_port_select & 0x3) << 30;
		reg32 |= (conf->gpu_panel_power_up_delay & 0x1fff) << 16;
		reg32 |= (conf->gpu_panel_power_backlight_on_delay & 0x1fff);
		gtt_write(0xc7208, reg32);
	}

	/* Setup Panel Power Off Delays */
	reg32 = gtt_read(0xc720c);
	if (!reg32) {
		reg32 = (conf->gpu_panel_power_down_delay & 0x1fff) << 16;
		reg32 |= (conf->gpu_panel_power_backlight_off_delay & 0x1fff);
		gtt_write(0xc720c, reg32);
	}

	/* Setup Panel Power Cycle Delay */
	if (conf->gpu_panel_power_cycle_delay) {
		reg32 = gtt_read(0xc7210);
		reg32 &= ~0xff;
		reg32 |= conf->gpu_panel_power_cycle_delay & 0xff;
		gtt_write(0xc7210, reg32);
	}

	/* Enable Backlight if needed */
	if (conf->gpu_cpu_backlight) {
		gtt_write(0x48250, (1 << 31));
		gtt_write(0x48254, conf->gpu_cpu_backlight);
	}
	if (conf->gpu_pch_backlight) {
		gtt_write(0xc8250, (1 << 31));
		gtt_write(0xc8254, conf->gpu_pch_backlight);
	}
}

/* Enable SCI to ACPI _GPE._L06 */
static void gma_enable_swsci(void)
{
	u16 reg16;

	/* clear DMISCI status */
	reg16 = inw(DEFAULT_PMBASE + TCO1_STS);
	reg16 &= DMISCI_STS;
	outw(DEFAULT_PMBASE + TCO1_STS, reg16);

	/* clear acpi tco status */
	outl(DEFAULT_PMBASE + GPE0_STS, TCOSCI_STS);

	/* enable acpi tco scis */
	reg16 = inw(DEFAULT_PMBASE + GPE0_EN);
	reg16 |= TCOSCI_EN;
	outw(DEFAULT_PMBASE + GPE0_EN, reg16);
}

static void gma_func0_init(struct device *dev)
{
	intel_gma_init_igd_opregion();

	if (!CONFIG(NO_GFX_INIT))
		pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER);

	gtt_res = probe_resource(dev, PCI_BASE_ADDRESS_0);
	if (!gtt_res || !gtt_res->base)
		return;

	if (!acpi_is_wakeup_s3() &&
	    CONFIG(MAINBOARD_USE_LIBGFXINIT)) {
		struct northbridge_intel_ironlake_config *conf = dev->chip_info;
		int lightup_ok;
		printk(BIOS_SPEW, "Initializing VGA without OPROM.");

		gma_gfxinit(&lightup_ok);
		/* Linux relies on VBT for panel info. */
		generate_fake_intel_oprom(&conf->gfx, dev, "$VBT IRONLAKE-MOBILE");
	} else {
		/* PCI Init, will run VBIOS */
		pci_dev_init(dev);
	}

	/* Post VBIOS init */
	gma_pm_init_post_vbios(dev);

	gma_enable_swsci();
}

static void gma_read_resources(struct device *dev)
{
	pci_dev_read_resources(dev);

	struct resource *res;

	/* Set the graphics memory to write combining. */
	res = probe_resource(dev, PCI_BASE_ADDRESS_2);
	if (!res) {
		printk(BIOS_DEBUG, "gma: memory resource not found.\n");
		return;
	}
	res->flags |= IORESOURCE_RESERVE | IORESOURCE_FIXED | IORESOURCE_ASSIGNED;
	pci_write_config32(dev, PCI_BASE_ADDRESS_2, 0xd0000001);
	pci_write_config32(dev, PCI_BASE_ADDRESS_2 + 4, 0);
	res->base = (resource_t) 0xd0000000;
	res->size = (resource_t) 0x10000000;
}

static void gma_generate_ssdt(const struct device *device)
{
	const struct northbridge_intel_ironlake_config *chip = device->chip_info;

	drivers_intel_gma_displays_ssdt_generate(&chip->gfx);
}

static struct device_operations gma_func0_ops = {
	.read_resources		= gma_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.acpi_fill_ssdt		= gma_generate_ssdt,
	.init			= gma_func0_init,
	.ops_pci		= &pci_dev_ops_pci,
};

static const unsigned short pci_device_ids[] = {
	0x0046, 0x0102, 0x0106, 0x010a, 0x0112,
	0x0116, 0x0122, 0x0126, 0x0156,
	0x0166,
	0
};

static const struct pci_driver gma __pci_driver = {
	.ops	 = &gma_func0_ops,
	.vendor	 = PCI_VID_INTEL,
	.devices = pci_device_ids,
};
