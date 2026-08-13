/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <drivers/i2c/hid/chip.h>
#include <drivers/intel/touch/chip.h>
#include <fw_config.h>
#include <sar.h>
#include <soc/soc_chip.h>

WEAK_DEV_PTR(i2c_4_elan);
WEAK_DEV_PTR(i2c_4_wacom);
WEAK_DEV_PTR(i2c_4_google);
WEAK_DEV_PTR(touch_0_i2c_elan);
WEAK_DEV_PTR(touch_0_i2c_wacom);
WEAK_DEV_PTR(touch_0_i2c_google);
WEAK_DEV_PTR(touch_0_spi_elan);
WEAK_DEV_PTR(touch_0_spi_wacom);
WEAK_DEV_PTR(touch_0_spi_google);

enum touchscreen_intf {
	TS_INTF_NONE,
	TS_INTF_LPSS_I2C4,
	TS_INTF_THC,
	TS_INTF_GSPI,
};

enum intf_type {
	INTF_LPSS_I2C,
	INTF_THC_I2C,
	INTF_THC_SPI,
	INTF_GSPI,
};

enum touchscreen_dev_sel {
	TS_DEV_NONE,
	TS_DEV_ELAN_B36_I2C,
	TS_DEV_ELAN_B36_SPI,
	TS_DEV_WACOM_B52_I2C,
	TS_DEV_WACOM_B52_SPI,
	TS_DEV_ELAN_REX_I2C,
	TS_DEV_ELAN_REX_SPI
};

const char *get_wifi_sar_cbfs_filename(void)
{
	return get_wifi_sar_fw_config_filename(FW_CONFIG_FIELD(AP_OEM_2BIT_FIELD0));
}

static const struct device *get_ts_dev(enum touchscreen_dev_sel d,
	enum touchscreen_intf ts_intf, enum intf_type *intf_type)
{
	switch (ts_intf) {
	case TS_INTF_THC:
		switch (d) {
		case TS_DEV_ELAN_B36_I2C:
			*intf_type = INTF_THC_I2C;
			return DEV_PTR(touch_0_i2c_elan);
		case TS_DEV_WACOM_B52_I2C:
			*intf_type = INTF_THC_I2C;
			return DEV_PTR(touch_0_i2c_wacom);
		case TS_DEV_ELAN_REX_I2C:
			*intf_type = INTF_THC_I2C;
			return DEV_PTR(touch_0_i2c_google);
		case TS_DEV_ELAN_B36_SPI:
			*intf_type = INTF_THC_SPI;
			return DEV_PTR(touch_0_spi_elan);
		case TS_DEV_WACOM_B52_SPI:
			*intf_type = INTF_THC_SPI;
			return DEV_PTR(touch_0_spi_wacom);
		case TS_DEV_ELAN_REX_SPI:
			*intf_type = INTF_THC_SPI;
			return DEV_PTR(touch_0_spi_google);
		default:
			printk(BIOS_ERR, "Invalid touchscreen selected for THC0 interface!\n");
			return NULL;
		}
	case TS_INTF_LPSS_I2C4:
		*intf_type = INTF_LPSS_I2C;
		switch (d) {
		case TS_DEV_ELAN_B36_I2C:
			return DEV_PTR(i2c_4_elan);
		case TS_DEV_WACOM_B52_I2C:
			return DEV_PTR(i2c_4_wacom);
		case TS_DEV_ELAN_REX_I2C:
			return DEV_PTR(i2c_4_google);
		default:
			printk(BIOS_ERR, "Non-I2C touchscreen selected for LPSS-I2C4 interface!\n");
			return NULL;
		}
	case TS_INTF_GSPI:
		*intf_type = INTF_GSPI;
		switch (d) {
		case TS_DEV_ELAN_B36_SPI:
			return NULL; /* reserved for now; ex: return DEV_PTR(gspi_0_elan); */
		case TS_DEV_WACOM_B52_SPI:
			return NULL; /* reserved for now; ex: return DEV_PTR(gspi_0_wacom); */
		case TS_DEV_ELAN_REX_SPI:
			return NULL; /* reserved for now; ex: return DEV_PTR(gspi_0_google); */
		default:
			printk(BIOS_ERR, "Non-SPI touchscreen selected for LPSS-SPI interface!\n");
			return NULL;
		}
	default:
		return NULL;
	}
}

static void variant_update_soc_touchscreen_config(struct soc_intel_novalake_config *config)
{
	enum intf_type ts_intf_type;
	enum touchscreen_intf ts_intf = TS_INTF_NONE;
	enum touchscreen_dev_sel ts_dev_sel = TS_DEV_NONE;
	const struct device *ts_dev = NULL;
	struct drivers_i2c_hid_config *ts_lpss_config;
	struct drivers_intel_touch_config *ts_thc_config;

	if (fw_config_probe(FW_CONFIG(TOUCHSCREEN_SOC_INTERFACE, TOUCHSCREEN_INTERFACE_UNDEFINED)))
		return;

	if (fw_config_probe(FW_CONFIG(AP_OEM_3BIT_FIELD0, TOUCHSCREEN_ELAN_B36_I2C)))
		ts_dev_sel = TS_DEV_ELAN_B36_I2C;
	else if (fw_config_probe(FW_CONFIG(AP_OEM_3BIT_FIELD0, TOUCHSCREEN_WACOM_B52_I2C)))
		ts_dev_sel = TS_DEV_WACOM_B52_I2C;
	else if (fw_config_probe(FW_CONFIG(AP_OEM_3BIT_FIELD0, TOUCHSCREEN_ELAN_REX_I2C)))
		ts_dev_sel = TS_DEV_ELAN_REX_I2C;
	else if (fw_config_probe(FW_CONFIG(AP_OEM_3BIT_FIELD0, TOUCHSCREEN_ELAN_B36_SPI)))
		ts_dev_sel = TS_DEV_ELAN_B36_SPI;
	else if (fw_config_probe(FW_CONFIG(AP_OEM_3BIT_FIELD0, TOUCHSCREEN_WACOM_B52_SPI)))
		ts_dev_sel = TS_DEV_WACOM_B52_SPI;
	else if (fw_config_probe(FW_CONFIG(AP_OEM_3BIT_FIELD0, TOUCHSCREEN_ELAN_REX_SPI)))
		ts_dev_sel = TS_DEV_ELAN_REX_SPI;

	if (fw_config_probe(FW_CONFIG(TOUCHSCREEN_SOC_INTERFACE, TOUCHSCREEN_INTERFACE_THC)))
		ts_intf = TS_INTF_THC;
	else if (fw_config_probe(FW_CONFIG(TOUCHSCREEN_SOC_INTERFACE, TOUCHSCREEN_INTERFACE_LPSS_I2C)))
		ts_intf = TS_INTF_LPSS_I2C4;
	else if (fw_config_probe(FW_CONFIG(TOUCHSCREEN_SOC_INTERFACE, TOUCHSCREEN_INTERFACE_GSPI)))
		ts_intf = TS_INTF_GSPI;

	ts_dev = get_ts_dev(ts_dev_sel, ts_intf, &ts_intf_type);
	if (!ts_dev)
		die("Invalid Touchscreen device selected for the touch interface!\n");

	switch (ts_intf_type) {
	case INTF_THC_I2C:
		config->thc_mode[0] = THC_HID_I2C_MODE;
		break;
	case INTF_THC_SPI:
		config->thc_mode[0] = THC_HID_SPI_MODE;
		break;
	case INTF_GSPI:
		config->serial_io_gspi_mode[PchSerialIoIndexGSPI0] = PchSerialIoPci;
		break;
	case INTF_LPSS_I2C:
		if (ts_dev_sel == TS_DEV_WACOM_B52_I2C) {
			config->common_soc_config.i2c[4].speed = I2C_SPEED_STANDARD;
			printk(BIOS_DEBUG, "FW_CONFIG: i2c4 bus speed for BOM52: standard mode\n");
		}
		break;
	default:
		break;
	}

	/* for touchscreen wake */
	if (CONFIG(TOUCHSCREEN_WAKE)) {
		config->wake_on_touch[0] = true;
		printk(BIOS_DEBUG, "MB Kconfig: enable Touchscreen wake\n");
	}

	if ((config->wake_on_touch[0])) {
		if (ts_intf == TS_INTF_THC) {
			ts_thc_config = (struct drivers_intel_touch_config *)config_of(ts_dev);
			ts_thc_config->wake_on_touch = true;
		} else if (ts_intf == TS_INTF_LPSS_I2C4 && ts_dev) {
			/*
			 * NOTE: device power should not be turned off and reset cannot be hold
			 * asserted during S0ix when wake is enabled.
			 */
			ts_lpss_config = (struct drivers_i2c_hid_config *)config_of(ts_dev);
			ts_lpss_config->generic.irq.wake = ACPI_IRQ_WAKE;
			ts_lpss_config->generic.has_power_resource = true;
			ts_lpss_config->generic.enable_gpio.pins[0] = 0;
			ts_lpss_config->generic.enable_gpio.pin_count = 0;
			ts_lpss_config->generic.reset_gpio.pins[0] = 0;
			ts_lpss_config->generic.reset_gpio.pin_count = 0;
		}
	/* NOTE: this wake config is also used for LPSS i2c */
	} else {
		if (ts_intf == TS_INTF_LPSS_I2C4 && ts_dev) {
			/*
			 * NOTE: wake is enabled implicitly when GPE bit is assigned in the
			 * device tree; To disable it, we will need to clear this config here.
			 */
			ts_lpss_config = (struct drivers_i2c_hid_config *)config_of(ts_dev);
			if (ts_lpss_config->generic.irq.pin)
				ts_lpss_config->generic.irq.wake = ACPI_IRQ_NO_WAKE;
			ts_lpss_config->generic.wake = 0;
		}
	}
}

void variant_update_soc_chip_config(struct soc_intel_novalake_config *config)
{
	/* CNVi */
	if (fw_config_probe(FW_CONFIG(WIFI_INTERFACE, WIFI_INTERFACE_CNVI))) {
		config->cnvi_wifi_core = true;
		config->cnvi_bt_core = true;
	}

	variant_update_soc_touchscreen_config(config);

	/*
	 * GPIO group to GPE assignment for both touchscreen and trackpad wake in THC mode
	 * NOTE: trackpad wake is required in Atria
	 */
	if ((config->wake_on_touch[0] &&
		fw_config_probe(FW_CONFIG(TOUCHSCREEN_SOC_INTERFACE, TOUCHSCREEN_INTERFACE_THC))) ||
		fw_config_probe(FW_CONFIG(TRACKPAD_SOC_INTERFACE, TRACKPAD_INTERFACE_THC))) {
		config->pmc_gpe0_dw2 = GPP_VGPIO3;
	}
}
