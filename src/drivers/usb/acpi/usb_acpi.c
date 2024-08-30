/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>
#include <acpi/acpi_pld.h>
#include <acpi/acpigen.h>
#include <acpi/acpigen_dsm.h>
#include <console/console.h>
#include <device/device.h>
#include "chip.h"

/*
 * Intel Bluetooth DSM
 *
 * Check Tile Activation (2d19d3e1-5708-4696-bd5b-2c3dbae2d6a9)
 *
 * Arg2 == 0: Return a package with the following bits set
 * BIT(0)	Indicates whether the device supports other functions
 * BIT(1)	Check Tile Activation
 *
 * Check/Set Reset Delay (aa10f4e0-81ac-4233-abf6-3b2ac50e28d9)
 * Arg2 == 0: Return a package with the following bit set
 * BIT(0)	Indicates whether the device supports other functions
 * BIT(1)	Check Bluetooth reset timing
 * Arg2 == 1: Set the reset delay based on Arg3
 */

static void check_reset_delay(void *arg)
{
	acpigen_write_if_lequal_op_int(ARG1_OP, 0);
	{
		acpigen_write_return_singleton_buffer(0x03);
	}
	acpigen_write_else();
	{
		acpigen_write_return_singleton_buffer(0x00);
	}
	acpigen_pop_len();
}

static void set_reset_delay(void *arg)
{
	acpigen_write_store_op_to_namestr(ARG3_OP, "RDLY");
}

static void get_feature_flag(void *arg)
{
	acpigen_write_if_lequal_op_int(ARG1_OP, 0);
	{
		acpigen_write_return_singleton_buffer(0x03);
	}
	acpigen_write_else();
	{
		acpigen_write_return_singleton_buffer(0x00);
	}
	acpigen_pop_len();
}

void (*uuid_callbacks1[])(void *) = { check_reset_delay, set_reset_delay };
void (*uuid_callbacks2[])(void *) = { get_feature_flag };

static void acpi_device_intel_bt(void)
{
/*
 *	Name (RDLY, 0x69)
 */
	acpigen_write_name_integer("RDLY", 0x69);

/*
 *	Method (_DSM, 4, Serialized)
 *	{
 *		If ((Arg0 == ToUUID ("aa10f4e0-81ac-4233-abf6-3b2ac50e28d9")))
 *		{
 *			If ((Arg2 == Zero))
 *			{
 *				If ((Arg1 == Zero))
 *				{
 *					Return (Buffer (One)
 *					{
 *						0x03
 *					})
 *				}
 *				Else
 *				{
 *					Return (Buffer (One)
 *					{
 *						0x00
 *					})
 *				}
 *			}
 *			If ((Arg2 == One))
 *			{
 *				RDLY = Arg3
 *			}
 *			Return (Zero)
 *		}
 *		ElseIf ((Arg0 == ToUUID ("2d19d3e1-5708-4696-bd5b-2c3dbae2d6a9")))
 *		{
 *			If ((Arg2 == Zero))
 *			{
 *				If ((Arg1 == Zero))
 *				{
 *					Return (Buffer (One)
 *					{
 *						0x00
 *					})
 *				}
 *				Else
 *				{
 *					Return (Buffer (One)
 *					{
 *						0x00
 *					})
 *				}
 *			}
 *			Return (Zero)
 *		}
 *		Else
 *		{
 *			Return (Buffer (One)
 *			{
 *				0x00
 *			})
 *		}
 *	}
 */
	struct dsm_uuid uuid_callbacks[] = {
		DSM_UUID("aa10f4e0-81ac-4233-abf6-3b2ac50e28d9", uuid_callbacks1, 2, NULL),
		DSM_UUID("2d19d3e1-5708-4696-bd5b-2c3dbae2d6a9", uuid_callbacks2, 1, NULL),
	};

	acpigen_write_dsm_uuid_arr(uuid_callbacks, ARRAY_SIZE(uuid_callbacks));
/*
 *	PowerResource (BTRT, 0x05, 0x0000)
 *	{
 *		Method (_STA, 0, NotSerialized)
 *		{
 *			Return (One)
 *		}
 *		Method (_ON, 0, NotSerialized)
 *		{
 *		}
 *		Method (_OFF, 0, NotSerialized)
 *		{
 *		}
 *		Method (_RST, 0, NotSerialized)
 *		{
 *			Local0 = Acquire (CNMT, 0x03E8)
 *			If ((Local0 == Zero))
 *			{
 *				BTRK (Zero)
 *				Sleep (RDLY)
 *				BTRK (One)
 *				Sleep (RDLY)
 *			}
 *			Release (CNMT)
 *		}
 *	}
 */
	acpigen_write_power_res("BTRT", 5, 0, NULL, 0);
	{
		acpigen_write_method("_STA", 0);
		{
			acpigen_write_return_integer(1);
		}
		acpigen_pop_len();

		acpigen_write_method("_ON", 0);
		acpigen_pop_len();

		acpigen_write_method("_OFF", 0);
		acpigen_pop_len();

		acpigen_write_method("_RST", 0);
		{
			acpigen_write_store();
			acpigen_write_acquire("CNMT", 0x03e8);
			acpigen_emit_byte(LOCAL0_OP);

			acpigen_write_if_lequal_op_int(LOCAL0_OP, 0);
			{
				acpigen_emit_namestring("BTRK");
				acpigen_emit_byte(0);

				acpigen_emit_ext_op(SLEEP_OP);
				acpigen_emit_namestring("RDLY");

				acpigen_emit_namestring("BTRK");
				acpigen_emit_byte(1);

				acpigen_emit_ext_op(SLEEP_OP);
				acpigen_emit_namestring("RDLY");
			}
			acpigen_pop_len();
			acpigen_write_release("CNMT");
		}
		acpigen_pop_len();
	}
	acpigen_write_power_res_end();
}

static bool usb_acpi_add_gpios_to_crs(struct drivers_usb_acpi_config *cfg)
{
	if (cfg->privacy_gpio.pin_count)
		return true;

	if (cfg->reset_gpio.pin_count && !cfg->has_power_resource)
		return true;

	return false;
}

static int usb_acpi_write_gpio(struct acpi_gpio *gpio, int *curr_index)
{
	int ret = -1;

	if (gpio->pin_count == 0)
		return ret;

	acpi_device_write_gpio(gpio);
	ret = *curr_index;
	(*curr_index)++;

	return ret;
}

static void usb_acpi_fill_ssdt_generator(const struct device *dev)
{
	struct drivers_usb_acpi_config *config = dev->chip_info;
	const char *path = acpi_device_path(dev);
	struct acpi_pld pld;
	struct dsm_usb_config usb_cfg;

	if (!path || !config)
		return;

	/* Don't generate output for hubs, only ports */
	if (config->type == UPC_TYPE_HUB)
		return;

	acpigen_write_scope(path);
	if (config->desc)
		acpigen_write_name_string("_DDN", config->desc);
	acpigen_write_upc(config->type);

	if (usb_acpi_get_pld(dev, &pld))
		acpigen_write_pld(&pld);
	else
		printk(BIOS_ERR, "Error retrieving PLD for %s\n", path);

	if (config->usb_lpm_incapable) {
		usb_cfg.usb_lpm_incapable = 1;
		acpigen_write_dsm_usb(&usb_cfg);
	}

	/* Resources */
	if (usb_acpi_add_gpios_to_crs(config) == true) {
		struct acpi_dp *dsd;
		int idx = 0;
		int reset_gpio_index = -1;
		int privacy_gpio_index;

		acpigen_write_name("_CRS");
		acpigen_write_resourcetemplate_header();
		if (!config->has_power_resource) {
			reset_gpio_index = usb_acpi_write_gpio(
						&config->reset_gpio, &idx);
		}
		privacy_gpio_index = usb_acpi_write_gpio(&config->privacy_gpio,
							 &idx);
		acpigen_write_resourcetemplate_footer();

		dsd = acpi_dp_new_table("_DSD");
		if (reset_gpio_index >= 0)
			acpi_dp_add_gpio(dsd, "reset-gpio", path,
					 reset_gpio_index, 0,
					 config->reset_gpio.active_low);
		if (privacy_gpio_index >= 0)
			acpi_dp_add_gpio(dsd, "privacy-gpio", path,
					 privacy_gpio_index, 0,
					 config->privacy_gpio.active_low);
		acpi_dp_write(dsd);
	}

	if (config->has_power_resource) {
		const struct acpi_power_res_params power_res_params = {
			&config->reset_gpio,
			config->reset_delay_ms,
			config->reset_off_delay_ms,
			&config->enable_gpio,
			config->enable_delay_ms,
			config->enable_off_delay_ms,
			NULL,
			0,
			0,
			config->use_gpio_for_status
		};
		acpi_device_add_power_res(&power_res_params);
	}

	if (config->is_intel_bluetooth)
		acpi_device_intel_bt();

	acpigen_pop_len();

	printk(BIOS_INFO, "%s: %s at %s\n", path,
	       config->desc ? : dev->chip_ops->name, dev_path(dev));
}

static struct device_operations usb_acpi_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.scan_bus		= scan_static_bus,
	.acpi_fill_ssdt		= usb_acpi_fill_ssdt_generator,
};

static void usb_acpi_enable(struct device *dev)
{
	dev->ops = &usb_acpi_ops;
}

struct chip_operations drivers_usb_acpi_ops = {
	.name = "USB ACPI Device",
	.enable_dev = usb_acpi_enable
};

bool usb_acpi_get_pld(const struct device *usb_device, struct acpi_pld *pld)
{
	struct drivers_usb_acpi_config *config;

	if (!usb_device || !usb_device->chip_info ||
		usb_device->chip_ops != &drivers_usb_acpi_ops)
		return false;

	config = usb_device->chip_info;
	if (config->use_custom_pld)
		*pld = config->custom_pld;
	else
		acpi_pld_fill_usb(pld, config->type, &config->group);

	return true;
}
