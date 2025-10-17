/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <acpi/acpigen_pci.h>
#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>

#include "chip.h"
#include "elan.h"
#include "hynitron.h"
#include "wacom.h"

/* _DSM Method UUIDs */
#define TOUCH_HIDI2C_UUID		"3cdff6f7-4267-4555-ad05-b30a3d8938de"
#define TOUCH_HID_UUID			"6e2ac436-0fcf-41af-a265-b32a220dcfab"
#define TOUCH_INTF_UUID			"300d35b7-ac20-413e-8e9c-92e4dafd0afe"
#define TOUCH_LTR_UUID			"84005682-5b71-41a4-8d66-8130f787a138"

/* _DSD UUIDs */
#define ACPI_DP_UUID			"daffd814-6eba-4d8c-8a91-bc9bbf4aa301"
#define ACPI_DP_CHILD_UUID		"dbb8e3e6-5886-4ba6-8795-1319f52a966b"
#define ACPI_DSD_DMA_PROPERTY_UUID	"70d24161-6dd5-4c9e-8070-705531292865"
#define ACPI_DSD_DMA_PROPERTY_NAME	"DmaProperty"

#define TOUCH_MUTEX			"THMX"

static const struct intel_thc_hidi2c_info *_soc_hidi2c_info;
static const struct intel_thc_hidspi_info *_soc_hidspi_info;

__weak const struct intel_thc_hidi2c_info *soc_get_thc_hidi2c_info(void) { return NULL; }
__weak const struct intel_thc_hidspi_info *soc_get_thc_hidspi_info(void) { return NULL; }

/*
 * This function returns a pointer to the correct drivers_intel_touch_config structure based on
 * the device tree configuration and the connected device type.
 */
static const struct drivers_intel_touch_config *get_driver_config(const struct device *dev)

{
	const struct drivers_intel_touch_config none_driver_config;
	const struct drivers_intel_touch_config *config = dev->chip_info;
	const struct drivers_intel_touch_config *devices[TH_SENSOR_MAX] = {
		[TH_SENSOR_NONE] = &none_driver_config,
		[TH_SENSOR_WACOM] = &wacom_touch_config,
		[TH_SENSOR_ELAN] = &elan_touch_config,
		[TH_SENSOR_GOOGLE] = &google_touch_config,
		[TH_SENSOR_HYNITRON] = &hynitron_touch_config,
		[TH_SENSOR_GENERIC] = config
	};
	return devices[config->connected_device];
}

/* Use only Device-tree definition. */
#define touch_config_get(device, field)							\
	(((const struct drivers_intel_touch_config *)(device)->chip_info)->field)

/* Device-tree definition takes precedence over static drivers definition. */
#define touch_get(device, field)							\
	((((const struct drivers_intel_touch_config *)(device)->chip_info)->field) ?	\
	 (((const struct drivers_intel_touch_config *)(device)->chip_info)->field) :	\
	 get_driver_config(dev)->field)

/* Device-tree definition takes precedence over static drivers SoC-specific definition. */
#define touch_soc_get(device, intf, field)								\
	((((const struct drivers_intel_touch_config *)(device)->chip_info)->soc_## intf .field) ?	\
	 (((const struct drivers_intel_touch_config *)(device)->chip_info)->soc_## intf .field) :	\
	 (_soc_## intf ##_info->field))

/*
 * Device-tree definition takes precedence over static drivers device-specific over static
 * drivers SoC-specific definition.
 */
#define touch_dev_soc_get(device, interface, field)  \
	((((const struct drivers_intel_touch_config *)(device)->chip_info)->dev_## interface .intf.interface.field) ? :  \
	(get_driver_config(device)->dev_## interface .intf.interface.field) ? :  \
	((((const struct drivers_intel_touch_config *)(device)->chip_info)->soc_## interface .field) ? :  \
	(_soc_## interface ##_info->field)))

static const char *touch_acpi_name(const struct device *dev)
{
	return touch_get(dev, name) ? : INTEL_THC0_NAME;
}

/*
 * Functions prefixed with touch_acpigen_ are utilized within the Device Specific Method (DSM)
 * ACPI code generation framework.
 */

#define touch_config_acpigen(field, fmt)					\
	static void touch_acpigen_##field(void *arg)				\
	{									\
		const struct device *dev = (const struct device *)arg;		\
		uint32_t value = touch_config_get(dev, field);			\
		acpigen_write_debug_sprintf(fmt, touch_acpi_name(dev), value);	\
		acpigen_write_return_integer(value);				\
	}

#define touch_dev_acpigen(field, interface, type, fmt)				\
	static void touch_acpigen_## interface ## _ ## field(void *arg)		\
	{									\
		const struct device *dev = (const struct device *)arg;		\
		uint32_t value = touch_get(dev, dev_##interface.intf.interface.field);		\
		acpigen_write_debug_sprintf(fmt, touch_acpi_name(dev), value);	\
		acpigen_write_return_ ## type(value);				\
	}

#define touch_soc_acpigen(field, intf, type, fmt)				\
	static void touch_acpigen_## intf ## _ ## field(void *arg)		\
	{									\
		const struct device *dev = (const struct device *)arg;		\
		uint32_t value = touch_soc_get(dev, intf, field);		\
		acpigen_write_debug_sprintf(fmt, touch_acpi_name(dev), value);	\
		acpigen_write_return_ ## type(value);				\
	}

#define touch_dev_soc_acpigen(field, intf, type, fmt)				\
	static void touch_acpigen_## intf ## _ ## field(void *arg)		\
	{									\
		const struct device *dev = (const struct device *)arg;		\
		uint32_t value = touch_dev_soc_get(dev, intf, field);		\
		acpigen_write_debug_sprintf(fmt, touch_acpi_name(dev), value);	\
		acpigen_write_return_ ## type(value);				\
	}

touch_dev_soc_acpigen(connection_speed, hidspi, integer, "%s: connection speed = %d\n");
touch_soc_acpigen(limit_packet_size, hidspi, integer, "%s: limit packet size = %d\n");
touch_soc_acpigen(performance_limit, hidspi, integer, "%s: performance limit = %d\n");

static void (*touch_intf[])(void *) = {
	NULL, /* Enumerate functions (auto-generated) */
	touch_acpigen_hidspi_connection_speed,
	touch_acpigen_hidspi_limit_packet_size,
	touch_acpigen_hidspi_performance_limit
};

touch_config_acpigen(idle_ltr, "%s: Idle LTR = %d\n");
touch_config_acpigen(active_ltr, "%s: Active LTR = %d\n");

static void (*touch_ltr[])(void *) = {
	NULL, /* Enumerate functions (auto-generated) */
	touch_acpigen_active_ltr,
	touch_acpigen_idle_ltr
};

touch_dev_acpigen(input_report_header_address, hidspi, integer,
	"%s: HID SPI Input Report Header Address = %d\n");
touch_dev_acpigen(input_report_body_address, hidspi, integer,
	"%s: HID SPI Input Report Body Address = %d\n");
touch_dev_acpigen(output_report_address, hidspi, integer,
	"%s: HID SPI Output Report Address = %d\n");
touch_dev_acpigen(read_opcode, hidspi, singleton_buffer, "%s: HID SPI Read Opcode = %d\n");
touch_dev_acpigen(write_opcode, hidspi, singleton_buffer, "%s: HID SPI Write Opcode = %d\n");

touch_soc_acpigen(write_mode, hidspi, integer, "%s: HID SPI Write Mode (flags) = 0x%x\n");

static void (*touch_hidspi[])(void *) = {
	NULL, /* Enumerate functions (auto-generated) */
	touch_acpigen_hidspi_input_report_header_address,
	touch_acpigen_hidspi_input_report_body_address,
	touch_acpigen_hidspi_output_report_address,
	touch_acpigen_hidspi_read_opcode,
	touch_acpigen_hidspi_write_opcode,
	touch_acpigen_hidspi_write_mode
};

touch_dev_acpigen(descriptor_address, hidi2c, integer,
	"%s: HID I2C device descriptor address = 0x%x\n");

static void (*touch_hidi2c[])(void *) = {
	NULL, /* Enumerate functions (auto-generated) */
	touch_acpigen_hidi2c_descriptor_address
};

static void touch_generate_acpi_dsm(const struct drivers_intel_touch_config *config,
				    const struct device *dev)
{
	struct dsm_uuid spi_dsm_uuids[] = {
		DSM_UUID(TOUCH_INTF_UUID, touch_intf, ARRAY_SIZE(touch_intf), (void *)dev),
		DSM_UUID(TOUCH_LTR_UUID, touch_ltr, ARRAY_SIZE(touch_ltr), (void *)dev),
		DSM_UUID(TOUCH_HID_UUID, touch_hidspi, ARRAY_SIZE(touch_hidspi), (void *)dev)
	};
	struct dsm_uuid i2c_dsm_uuids[] = {
		DSM_UUID(TOUCH_LTR_UUID, touch_ltr, ARRAY_SIZE(touch_ltr), (void *)dev),
		DSM_UUID(TOUCH_HIDI2C_UUID, touch_hidi2c, ARRAY_SIZE(touch_hidi2c), (void *)dev)
	};

	if (config->connected_device == TH_SENSOR_NONE || config->mode == THC_IPTS_MODE)
		return;

	if (config->mode == THC_HID_SPI_MODE)
		acpigen_write_dsm_uuid_arr(spi_dsm_uuids, ARRAY_SIZE(spi_dsm_uuids));
	else if (config->mode == THC_HID_I2C_MODE)
		acpigen_write_dsm_uuid_arr(i2c_dsm_uuids, ARRAY_SIZE(i2c_dsm_uuids));
}

static void touch_generate_acpi_ini(const struct device *dev)
{
	acpigen_write_method_serialized("_INI", 0);
	{
		acpigen_write_debug_sprintf("%s: _INI()\n", touch_acpi_name(dev));
	}
	acpigen_write_method_end(); /* Method */
}

static void touch_generate_acpi_crs(const struct drivers_intel_touch_config *config,
				    const struct device *dev)
{
	if (config->mode == THC_IPTS_MODE || !config->wake_on_touch)
		return;

	printk(BIOS_DEBUG, "%s Creating _CRS for HID SPI/I2C touch wake\n",
	       touch_acpi_name(dev));
	acpigen_write_name("_CRS");
	acpigen_write_resourcetemplate_header();
	/*
	 * NOTE: config->wake_gpio: uses GpioInt() in _CRS; PAD needs to be Driver Mode
	 *       config->wake_irq:  uses Interrupt() in _CRS; use GPE; PAD needs to be ACPI Mode
	 */
	if (config->wake_gpio.pin_count)
		acpi_device_write_gpio(&config->wake_gpio);
	else if (config->wake_irq.pin)
		acpi_device_write_interrupt(&config->wake_irq);
	acpigen_write_resourcetemplate_footer();

	acpigen_write_name_integer("_S0W", ACPI_DEVICE_SLEEP_D3_HOT);
	if (config->wake_irq.pin)
		acpigen_write_PRW(config->wake_gpe, 3);
}

static void touch_generate_acpi_i2cdev_dsd(const struct device *dev)
{
	int dsd_pkg_cnt;
	uint64_t connection_speed_val;

	acpigen_write_name("ICRS");
	acpigen_emit_byte(BUFFER_OP);
	acpigen_write_len_f();
	acpigen_write_integer(12); /* total size of ICRS + 1 */
	acpigen_pop_len(); /* Name */
	acpigen_write_create_buffer_word_field("ICRS", 0x00, "DADR");
	acpigen_write_create_buffer_qword_field("ICRS", 0x02, "DSPD");
	acpigen_write_create_buffer_byte_field("ICRS", 0x0a, "DADM");

	acpigen_write_name("ISUB");
	acpigen_emit_byte(BUFFER_OP);
	acpigen_write_len_f();
	acpigen_write_integer(145); /* total size of ISUB + 1 */
	acpigen_pop_len();
	acpigen_write_create_buffer_qword_field("ISUB", 0x00, "SMHX");
	acpigen_write_create_buffer_qword_field("ISUB", 0x08, "SMLX");
	acpigen_write_create_buffer_qword_field("ISUB", 0x10, "SMTD");
	acpigen_write_create_buffer_qword_field("ISUB", 0x18, "SMRD");
	acpigen_write_create_buffer_qword_field("ISUB", 0x20, "FMHX");
	acpigen_write_create_buffer_qword_field("ISUB", 0x28, "FMLX");
	acpigen_write_create_buffer_qword_field("ISUB", 0x30, "FMTD");
	acpigen_write_create_buffer_qword_field("ISUB", 0x38, "FMRD");
	acpigen_write_create_buffer_qword_field("ISUB", 0x40, "FMSL");
	acpigen_write_create_buffer_qword_field("ISUB", 0x48, "FPHX");
	acpigen_write_create_buffer_qword_field("ISUB", 0x50, "FPLX");
	acpigen_write_create_buffer_qword_field("ISUB", 0x58, "FPTD");
	acpigen_write_create_buffer_qword_field("ISUB", 0x60, "FPRD");
	acpigen_write_create_buffer_qword_field("ISUB", 0x68, "HMHX");
	acpigen_write_create_buffer_qword_field("ISUB", 0x70, "HMLX");
	acpigen_write_create_buffer_qword_field("ISUB", 0x78, "HMTD");
	acpigen_write_create_buffer_qword_field("ISUB", 0x80, "HMRD");
	acpigen_write_create_buffer_qword_field("ISUB", 0x88, "HMSL");

	acpigen_write_store_int_to_namestr(touch_get(dev, dev_hidi2c.intf.hidi2c.addr), "DADR");

	if (_soc_hidi2c_info->get_soc_i2c_bus_speed_val_func) {
		connection_speed_val = _soc_hidi2c_info->get_soc_i2c_bus_speed_val_func(
			touch_dev_soc_get(dev, hidi2c, connection_speed));
	} else {
		die("Missing SoC function to map I2C speed to its register value!\n");
	}
	acpigen_write_store_int_to_namestr(connection_speed_val, "DSPD");
	acpigen_write_store_int_to_namestr(touch_soc_get(dev, hidi2c, addr_mode), "DADM");
	acpigen_write_store_int_to_namestr(touch_soc_get(dev, hidi2c, sm_scl_high_period), "SMHX");
	acpigen_write_store_int_to_namestr(touch_soc_get(dev, hidi2c, sm_scl_low_period), "SMLX");
	acpigen_write_store_int_to_namestr(touch_soc_get(dev, hidi2c, sm_sda_hold_tx_period), "SMTD");
	acpigen_write_store_int_to_namestr(touch_soc_get(dev, hidi2c, sm_sda_hold_rx_period), "SMRD");
	acpigen_write_store_int_to_namestr(touch_soc_get(dev, hidi2c, fm_scl_high_period), "FMHX");
	acpigen_write_store_int_to_namestr(touch_soc_get(dev, hidi2c, fm_scl_low_period), "FMLX");
	acpigen_write_store_int_to_namestr(touch_soc_get(dev, hidi2c, fm_sda_hold_tx_period), "FMTD");
	acpigen_write_store_int_to_namestr(touch_soc_get(dev, hidi2c, fm_sda_hold_rx_period), "FMRD");
	acpigen_write_store_int_to_namestr(touch_soc_get(dev, hidi2c, suppressed_spikes_s_f_fp), "FMSL");
	acpigen_write_store_int_to_namestr(touch_soc_get(dev, hidi2c, fmp_scl_high_period), "FPHX");
	acpigen_write_store_int_to_namestr(touch_soc_get(dev, hidi2c, fmp_scl_low_period), "FPLX");
	acpigen_write_store_int_to_namestr(touch_soc_get(dev, hidi2c, fmp_sda_hold_tx_period), "FPTD");
	acpigen_write_store_int_to_namestr(touch_soc_get(dev, hidi2c, fmp_sda_hold_rx_period), "FPRD");
	acpigen_write_store_int_to_namestr(touch_soc_get(dev, hidi2c, hm_scl_high_period), "HMHX");
	acpigen_write_store_int_to_namestr(touch_soc_get(dev, hidi2c, hm_scl_low_period), "HMLX");
	acpigen_write_store_int_to_namestr(touch_soc_get(dev, hidi2c, hm_sda_hold_tx_period), "HMTD");
	acpigen_write_store_int_to_namestr(touch_soc_get(dev, hidi2c, hm_sda_hold_rx_period), "HMRD");
	acpigen_write_store_int_to_namestr(touch_soc_get(dev, hidi2c, suppressed_spikes_h_fp), "HMSL");

	dsd_pkg_cnt = 4;
	if (touch_get(dev, add_acpi_dma_property))
		dsd_pkg_cnt += 2;

	acpigen_write_name("_DSD");
	acpigen_write_package(dsd_pkg_cnt);
	{
		acpigen_write_uuid(ACPI_DP_UUID);
		acpigen_write_package(1);
		{
			acpigen_write_package(2);
			{
				acpigen_write_string("ICRS");
				acpigen_emit_namestring("ICRS");
			}
			acpigen_pop_len();
		}
		acpigen_pop_len();
		if (touch_get(dev, add_acpi_dma_property)) {
			acpigen_write_uuid(ACPI_DSD_DMA_PROPERTY_UUID);
			acpigen_write_package(1);
			{
				acpigen_write_package(2);
				{
					acpigen_write_string(ACPI_DSD_DMA_PROPERTY_NAME);
					acpigen_write_integer(1);
				}
				acpigen_pop_len();
			}
			acpigen_pop_len();
		}
		acpigen_write_uuid(ACPI_DP_CHILD_UUID);
		acpigen_write_package(1);
		{
			acpigen_write_package(2);
			{
				acpigen_write_string("ISUB");
				acpigen_emit_namestring("ISUB");
			}
			acpigen_pop_len();
		}
		acpigen_pop_len();
	}
	acpigen_pop_len(); /* Name */
}

static void touch_generate_acpi_method_status(const struct drivers_intel_touch_config *config,
					      const struct device *dev)
{
	const struct acpi_gpio *gpio;

	acpigen_write_method("_STA", 0);
	{
		if (config->connected_device != TH_SENSOR_NONE) {
			/* Use enable GPIO for status if provided, otherwise use reset GPIO. */
			if (config->enable_gpio.pin_count) {
				gpio = &config->enable_gpio;
				/* Read current GPIO state into Local0. */
				acpigen_get_tx_gpio(gpio);
			} else {
				gpio = &config->reset_gpio;
				/* Read current GPIO state into Local0. */
				acpigen_get_tx_gpio(gpio);
				acpigen_write_not(LOCAL0_OP, LOCAL0_OP);
			}
			acpigen_write_return_op(LOCAL0_OP);
		} else {
			acpigen_write_integer(0);
		}
	}
	acpigen_write_method_end(); /* Method */
}

static void touch_generate_acpi_method_on(const struct drivers_intel_touch_config *config,
					  const struct device *dev)
{
	const struct device *parent_dev = dev->upstream->dev;
	const char *mutex_path = acpi_device_path_join(parent_dev, TOUCH_MUTEX);

	printk(BIOS_DEBUG, "%s: generating _ON() mutex = %s\n", touch_acpi_name(dev),
		mutex_path);
	acpigen_write_method_serialized("_ON", 0);
	{
		acpigen_write_debug_sprintf("%s: _ON()\n", touch_acpi_name(dev));
		if (config->connected_device != TH_SENSOR_GENERIC) {
			/* if wake is enabled, we don't manage power enabling and reset */
			if (config->wake_on_touch) {
				acpigen_write_debug_sprintf("%s: empty _ON() since wake is enabled.\n",
					touch_acpi_name(dev));
			} else {
				acpigen_write_acquire(mutex_path, ACPI_MUTEX_NO_TIMEOUT);
				acpigen_write_debug_sprintf("%s: _ON() mutex acquired.\n",
					touch_acpi_name(dev));
				/* ex: SOC_TCHSCR_RST_R_L */
				if (config->reset_gpio.pin_count) {
					acpigen_enable_tx_gpio(&config->reset_gpio);
				}
				/* ex: EN_TCHSCR_PWR */
				if (config->enable_gpio.pin_count) {
					acpigen_enable_tx_gpio(&config->enable_gpio);
					if (config->enable_delay_ms)
						acpigen_write_sleep(config->enable_delay_ms);
				}
				/* ex: SOC_TCHSCR_RST_R_L */
				if (config->reset_gpio.pin_count) {
					acpigen_disable_tx_gpio(&config->reset_gpio);
					if (config->reset_delay_ms)
						acpigen_write_sleep(config->reset_delay_ms);
				}
				/* ex: SOC_TCHSCR_RPT_EN */
				if (config->report_gpio.pin_count) {
					acpigen_enable_tx_gpio(&config->report_gpio);
					if (config->report_delay_ms)
						acpigen_write_sleep(config->report_delay_ms);
				}
				acpigen_write_release(mutex_path);
			}
		}
		acpigen_write_debug_sprintf("%s: _ON() completed\n", touch_acpi_name(dev));
	}
	acpigen_write_method_end();
}

static void touch_generate_acpi_method_off(const struct drivers_intel_touch_config *config,
					   const struct device *dev)
{
	const struct device *parent_dev = dev->upstream->dev;
	const char *mutex_path = acpi_device_path_join(parent_dev, TOUCH_MUTEX);

	acpigen_write_method_serialized("_OFF", 0);
	{
		acpigen_write_debug_sprintf("%s: _OFF()\n", touch_acpi_name(dev));
		if (config->connected_device != TH_SENSOR_GENERIC) {
			/* if wake is enabled, we don't manage power enabling and reset */
			if (config->wake_on_touch) {
				acpigen_write_debug_sprintf("%s: empty _OFF() since wake is enabled.\n",
					touch_acpi_name(dev));
			} else {
				acpigen_write_acquire(mutex_path, ACPI_MUTEX_NO_TIMEOUT);
				acpigen_write_debug_sprintf("%s: _OFF() mutex acquired.\n",
					touch_acpi_name(dev));
				/* ex: SOC_TCHSCR_RPT_EN */
				if (config->report_gpio.pin_count) {
					acpigen_disable_tx_gpio(&config->report_gpio);
					if (config->report_off_delay_ms)
						acpigen_write_sleep(config->report_off_delay_ms);
				}
				/* ex: SOC_TCHSCR_RST_R_L */
				if (config->reset_gpio.pin_count) {
					acpigen_enable_tx_gpio(&config->reset_gpio);
					if (config->reset_off_delay_ms)
						acpigen_write_sleep(config->reset_off_delay_ms);
				}
				/* ex: ==>  EN_TCHSCR_PWR */
				if (config->enable_gpio.pin_count) {
					acpigen_disable_tx_gpio(&config->enable_gpio);
					if (config->enable_off_delay_ms)
						acpigen_write_sleep(config->enable_off_delay_ms);
				}
				acpigen_write_release(mutex_path);
			}
		}
		acpigen_write_debug_sprintf("%s: _OFF() completed\n", touch_acpi_name(dev));
	}
	acpigen_write_method_end();
}

static void touch_generate_acpi_spidev_rst(const struct drivers_intel_touch_config *config,
					   const struct device *dev)
{
	acpigen_write_method_serialized("_RST", 0);
	{
		acpigen_write_debug_sprintf("%s: _RST()\n", touch_acpi_name(dev));
		if (config->soc_hidspi.reset_gpio.pin_count) {
			acpigen_write_acquire(TOUCH_MUTEX, ACPI_MUTEX_NO_TIMEOUT);
			acpigen_write_debug_sprintf("%s: _RST() mutex acquired.\n",
						    touch_acpi_name(dev));
			/* Assert RESET# GPIO. */
			acpigen_enable_tx_gpio(&config->soc_hidspi.reset_gpio);
			/* Note: use 300 for debug */
			acpigen_write_sleep(touch_soc_get(dev, hidspi, reset_sequencing_delay));
			/* De-assert RESET# GPIO. */
			acpigen_disable_tx_gpio(&config->soc_hidspi.reset_gpio);
			acpigen_write_release(TOUCH_MUTEX);
		} else
			acpigen_write_debug_sprintf("%s: _RST() empty method\n",
						    touch_acpi_name(dev));
		acpigen_write_debug_sprintf("%s: _RST() completed\n", touch_acpi_name(dev));
	}
	acpigen_write_method_end(); /* Method */
}

static void touch_dev_fill_ssdt_generator(const struct device *dev)
{
	_soc_hidi2c_info = soc_get_thc_hidi2c_info();
	_soc_hidspi_info = soc_get_thc_hidspi_info();

	const struct drivers_intel_touch_config *config = dev->chip_info;
	const struct device *parent = dev->upstream->dev;
	DEVTREE_CONST struct device *domain = dev_find_path(NULL, DEVICE_PATH_DOMAIN);
	const char *scope = acpi_device_path(parent);
	const char *domain_scope = acpi_device_path(domain);

	static const char *const power_res_states[] = {"_PR0", "_PR3"};

	if (!config)
		return;

	if (!is_dev_enabled(parent)) {
		printk(BIOS_ERR, "%s: touch controller is not enabled\n", __func__);
		return;
	}
	if (!scope) {
		printk(BIOS_ERR, "%s: touch controller scope not found\n", __func__);
		return;
	}
	if (config->mode == THC_HID_I2C_MODE && !_soc_hidi2c_info) {
		printk(BIOS_ERR, "%s: %s Missing THC SoC-specific config for HID-I2C.\n",
			__func__, touch_acpi_name(dev));
		return;
	}
	if (config->mode == THC_HID_SPI_MODE && !_soc_hidspi_info) {
		printk(BIOS_ERR, "%s: %s Missing THC SoC-specific config for HID-SPI.\n",
			__func__, touch_acpi_name(dev));
		return;
	}
	printk(BIOS_DEBUG, "Fill touch ACPIs: domain scope=%s scope=%s touch dev=%s type=%d\n",
	       domain_scope, scope, touch_acpi_name(dev), dev->path.type);

	acpigen_write_scope(domain_scope);
	{
		acpigen_write_device(touch_acpi_name(dev));
		{
			acpigen_write_ADR_pci_device(parent);
			acpigen_write_mutex(TOUCH_MUTEX, 0);

			if (config->connected_device != TH_SENSOR_NONE)
				touch_generate_acpi_crs(config, dev);
			touch_generate_acpi_ini(dev);
			touch_generate_acpi_dsm(config, dev);
			/* When only THC1 is used, both THC0/1 need to be enabled since THC0's
			 * device function is '0'. In this case, THC0 should set
			 * connected_device to TH_SENSOR_NONE so that when the driver detects
			 * THC0, the _DSM will be available for it to check and exit without
			 * error.
			 */
			if (config->connected_device == TH_SENSOR_NONE)
				return;

			if (config->mode == THC_HID_I2C_MODE) {
				touch_generate_acpi_i2cdev_dsd(dev);
			} else {
				/* Create _DSD with only DmaProperty */
				if (touch_get(dev, add_acpi_dma_property))
					acpi_device_add_dma_property(NULL);
			}
			if (config->enable_gpio.pin_count || config->reset_gpio.pin_count) {
				printk(BIOS_DEBUG, "%s generating ACPI power resource.\n",
				       touch_acpi_name(dev));
				/* ACPI Power Resource for controlling the attached device
				   power. */
				acpigen_write_power_res("THPR", 0, 0, power_res_states,
							ARRAY_SIZE(power_res_states));
				{
					touch_generate_acpi_method_status(config, dev);
					touch_generate_acpi_method_on(config, dev);
					touch_generate_acpi_method_off(config, dev);
				}
				acpigen_write_power_res_end(); /* PowerResource */
			}
			if (config->mode == THC_HID_SPI_MODE)
				touch_generate_acpi_spidev_rst(config, dev);
		}
		acpigen_write_device_end(); /* Device */
	}
	acpigen_write_scope_end(); /* Scope */
}

static struct device_operations touch_dev_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.acpi_fill_ssdt		= touch_dev_fill_ssdt_generator,
	.acpi_name		= touch_acpi_name
};

static void touch_dev_enable(struct device *dev)
{
	/* This dev is a generic device that is a child to the THC device */
	dev->ops = &touch_dev_ops;
}

/* Copy of default_pci_ops_dev with scan_bus addition */
static const struct device_operations pci_thc_device_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= pci_dev_init,
	.scan_bus		= scan_generic_bus, /* Non-default */
	.ops_pci		= &pci_dev_ops_pci
};

static const unsigned short pci_device_ids[] = {
	PCI_DID_INTEL_MTL_THC0_SPI,
	PCI_DID_INTEL_MTL_THC1_SPI,
	PCI_DID_INTEL_PTL_U_H_THC0_I2C,
	PCI_DID_INTEL_PTL_U_H_THC0_SPI,
	PCI_DID_INTEL_PTL_U_H_THC1_I2C,
	PCI_DID_INTEL_PTL_U_H_THC1_SPI,
	PCI_DID_INTEL_PTL_H_THC0_I2C,
	PCI_DID_INTEL_PTL_H_THC0_SPI,
	PCI_DID_INTEL_PTL_H_THC1_I2C,
	PCI_DID_INTEL_PTL_H_THC1_SPI,
	PCI_DID_INTEL_WCL_THC0_I2C,
	PCI_DID_INTEL_WCL_THC0_SPI,
	PCI_DID_INTEL_WCL_THC1_I2C,
	PCI_DID_INTEL_WCL_THC1_SPI,
	0
};

static const struct pci_driver intel_touch_driver __pci_driver = {
	.ops		= &pci_thc_device_ops,
	.vendor		= PCI_VID_INTEL,
	.devices	= pci_device_ids
};

struct chip_operations drivers_intel_touch_ops = {
	.name		= "Intel Touch Controller & Sensor Device",
	.enable_dev	= touch_dev_enable
};
