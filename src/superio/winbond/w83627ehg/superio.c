/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pnp.h>
#include <superio/conf_mode.h>
#include <superio/hwm5_conf.h>
#include <console/console.h>
#include <pc80/keyboard.h>
#include <option.h>

#include "w83627ehg.h"

static void enable_hwm_smbus(struct device *dev)
{
	u8 reg8;

	/* Configure pins 89/90 as SDA/SCL (I2C bus). */
	reg8 = pnp_read_config(dev, 0x2a);
	reg8 |= (1 << 1);
	pnp_write_config(dev, 0x2a, reg8);
}

static void init_acpi(struct device *dev)
{
	u8 value;
	unsigned int power_on = get_uint_option("power_on_after_fail", 1);

	pnp_enter_conf_mode(dev);
	pnp_set_logical_device(dev);
	value = pnp_read_config(dev, 0xe4);
	value &= ~(3 << 5);
	if (power_on)
		value |= (1 << 5);
	pnp_write_config(dev, 0xe4, value);
	pnp_exit_conf_mode(dev);
}

static void init_hwm(u16 base)
{
	int i;
	u8 reg, value;

	/* reg mask data */
	u8 hwm_reg_values[] = {
		0x40, 0xff, 0x81, /* Start HWM. */
		0x48, 0x7f, 0x2a, /* Set SMBus base to 0x2a (0x54 >> 1). */
	};

	for (i = 0; i < ARRAY_SIZE(hwm_reg_values); i += 3) {
		reg = hwm_reg_values[i];
		value = pnp_read_hwm5_index(base, reg);
		value &= 0xff & (~(hwm_reg_values[i + 1]));
		value |= 0xff & hwm_reg_values[i + 2];
		printk(BIOS_DEBUG, "base = 0x%04x, reg = 0x%02x, "
		       "value = 0x%02x\n", base, reg, value);
		pnp_write_hwm5_index(base, reg, value);
	}
}

static void w83627ehg_init(struct device *dev)
{
	struct resource *res0;

	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {
	case W83627EHG_KBC:
		pc_keyboard_init(NO_AUX_DEVICE);
		break;
	case W83627EHG_HWM:
		res0 = find_resource(dev, PNP_IDX_IO0);
		init_hwm(res0->base);
		break;
	case W83627EHG_ACPI:
		init_acpi(dev);
		break;
	}
}

static void w83627ehg_pnp_enable_resources(struct device *dev)
{
	pnp_enable_resources(dev);

	pnp_enter_conf_mode(dev);
	switch (dev->path.pnp.device) {
	case W83627EHG_HWM:
		printk(BIOS_DEBUG, "W83627EHG HWM SMBus enabled\n");
		enable_hwm_smbus(dev);
		break;
	}
	pnp_exit_conf_mode(dev);
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = w83627ehg_pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = w83627ehg_init,
	.ops_pnp_mode     = &pnp_conf_mode_8787_aa,
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, W83627EHG_FDC,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x07f8, },
	{ NULL, W83627EHG_PP,   PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x07f8, },
	{ NULL, W83627EHG_SP1,  PNP_IO0 | PNP_IRQ0, 0x07f8, },
	{ NULL, W83627EHG_SP2,  PNP_IO0 | PNP_IRQ0, 0x07f8, },
	{ NULL, W83627EHG_KBC,  PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_IRQ1,
		0x07ff, 0x07ff, },
	{ NULL, W83627EHG_SFI,  PNP_IO0 | PNP_IRQ0, 0x07f8, },
	{ NULL, W83627EHG_WDTO_PLED, },
	{ NULL, W83627EHG_ACPI, PNP_IRQ0, },
	{ NULL, W83627EHG_HWM,  PNP_IO0 | PNP_IRQ0, 0x07fe, },

	{ NULL, W83627EHG_GAME, PNP_IO0, 0x07ff, },
	{ NULL, W83627EHG_MIDI, PNP_IO1 | PNP_IRQ0, 0, 0x07fe, },
	{ NULL, W83627EHG_GPIO1, },
	{ NULL, W83627EHG_GPIO2, },
	{ NULL, W83627EHG_GPIO3, },
	{ NULL, W83627EHG_GPIO4, },
	{ NULL, W83627EHG_GPIO5, },
	{ NULL, W83627EHG_GPIO6, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_winbond_w83627ehg_ops = {
	.name = "Winbond W83627EHG Super I/O",
	.enable_dev = enable_dev,
};
