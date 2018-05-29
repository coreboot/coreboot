/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/acpi.h>
#include <arch/acpi_device.h>
#include <arch/acpigen.h>
#include <console/console.h>
#include <device/i2c_simple.h>
#include <device/device.h>
#include <device/path.h>
#include <stdint.h>
#include <string.h>
#include "chip.h"

#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)

#define DA7219_ACPI_NAME	"DLG7"
#define DA7219_ACPI_HID		"DLGS7219"

static void da7219_fill_ssdt(struct device *dev)
{
	struct drivers_i2c_da7219_config *config = dev->chip_info;
	const char *scope = acpi_device_scope(dev);
	struct acpi_i2c i2c = {
		.address = dev->path.i2c.device,
		.mode_10bit = dev->path.i2c.mode_10bit,
		.speed = config->bus_speed ? : I2C_SPEED_FAST,
		.resource = scope,
	};
	struct acpi_dp *dsd, *aad;

	if (!dev->enabled || !scope)
		return;

	/* Device */
	acpigen_write_scope(scope);
	acpigen_write_device(acpi_device_name(dev));
	acpigen_write_name_string("_HID", DA7219_ACPI_HID);
	acpigen_write_name_integer("_UID", 1);
	acpigen_write_name_string("_DDN", dev->chip_ops->name);
	acpigen_write_name_integer("_S0W", 4);
	acpigen_write_STA(ACPI_STATUS_DEVICE_ALL_ON);

	/* Resources */
	acpigen_write_name("_CRS");
	acpigen_write_resourcetemplate_header();
	acpi_device_write_i2c(&i2c);
	/* Use either Interrupt() or GpioInt() */
	if (config->irq_gpio.pin_count)
		acpi_device_write_gpio(&config->irq_gpio);
	else
		acpi_device_write_interrupt(&config->irq);
	acpigen_write_resourcetemplate_footer();

	/* AAD Child Device Properties */
	aad = acpi_dp_new_table("DAAD");
	acpi_dp_add_integer(aad, "dlg,btn-cfg", config->btn_cfg);
	acpi_dp_add_integer(aad, "dlg,mic-det-thr", config->mic_det_thr);
	acpi_dp_add_integer(aad, "dlg,jack-ins-deb", config->jack_ins_deb);
	acpi_dp_add_string(aad, "dlg,jack-det-rate", config->jack_det_rate);
	acpi_dp_add_integer(aad, "dlg,jack-rem-deb", config->jack_rem_deb);
	acpi_dp_add_integer(aad, "dlg,a-d-btn-thr", config->a_d_btn_thr);
	acpi_dp_add_integer(aad, "dlg,d-b-btn-thr", config->d_b_btn_thr);
	acpi_dp_add_integer(aad, "dlg,b-c-btn-thr", config->b_c_btn_thr);
	acpi_dp_add_integer(aad, "dlg,c-mic-btn-thr", config->c_mic_btn_thr);
	acpi_dp_add_integer(aad, "dlg,btn-avg", config->btn_avg);
	acpi_dp_add_integer(aad, "dlg,adc-1bit-rpt", config->adc_1bit_rpt);
	acpi_dp_add_integer(aad, "dlg,micbias-pulse-lvl",
			    config->micbias_pulse_lvl);
	acpi_dp_add_integer(aad, "dlg,micbias-pulse-time",
			    config->micbias_pulse_time);

	/* DA7219 Properties */
	dsd = acpi_dp_new_table("_DSD");
	acpi_dp_add_integer(dsd, "dlg,micbias-lvl", config->micbias_lvl);
	acpi_dp_add_string(dsd, "dlg,mic-amp-in-sel", config->mic_amp_in_sel);
	if (config->mclk_name != NULL)
		acpi_dp_add_string(dsd, "dlg,mclk-name", config->mclk_name);
	acpi_dp_add_child(dsd, "da7219_aad", aad);

	/* Write Device Property Hierarchy */
	acpi_dp_write(dsd);

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */

	printk(BIOS_INFO, "%s: %s address 0%xh irq %d\n",
	       acpi_device_path(dev), dev->chip_ops->name,
	       dev->path.i2c.device, config->irq.pin);
}

static const char *da7219_acpi_name(const struct device *dev)
{
	return DA7219_ACPI_NAME;
}
#endif

static struct device_operations da7219_ops = {
	.read_resources		  = DEVICE_NOOP,
	.set_resources		  = DEVICE_NOOP,
	.enable_resources	  = DEVICE_NOOP,
#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
	.acpi_name		  = &da7219_acpi_name,
	.acpi_fill_ssdt_generator = &da7219_fill_ssdt,
#endif
};

static void da7219_enable(struct device *dev)
{
	dev->ops = &da7219_ops;
}

struct chip_operations drivers_i2c_da7219_ops = {
	CHIP_NAME("Dialog Semiconductor DA7219 Audio Codec")
	.enable_dev = &da7219_enable
};
