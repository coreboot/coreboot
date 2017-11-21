/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Google Inc.
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
#include <console/console.h>
#include <chip.h>
#include <device/device.h>
#include <ec/ec.h>
#include <ec/google/chromeec/ec.h>
#include <gpio.h>
#include <mainboard/google/fizz/gpio.h>
#include <smbios.h>
#include <soc/gpio.h>
#include <soc/pci_devs.h>
#include <soc/nhlt.h>
#include <string.h>
#include <vendorcode/google/chromeos/chromeos.h>

#define FIZZ_SKU_ID_I7_U42 0x4
#define FIZZ_PL2_I7_U42    44
#define FIZZ_PL2_OTHERS    29
/*
 * For type-C chargers, set PL2 to 90% of max power to account for
 * cable loss and FET Rdson loss in the path from the source.
 */
#define GET_TYPEC_PL2(w)   (9 * (w) / 10)

static const char *oem_id = "GOOGLE";
static const char *oem_table_id = "FIZZ";

/*
 * mainboard_get_pl2
 *
 * @return value Pl2 should be set to
 *
 * Check if charger is USB C.  If so, set to 90% of the max value.
 * Otherwise, set PL2 based on sku id.
 */
static u32 mainboard_get_pl2(void)
{
	const gpio_t sku_id_gpios[] = {
		GPIO_SKU_ID0,
		GPIO_SKU_ID1,
		GPIO_SKU_ID2,
		GPIO_SKU_ID3,
	};
	enum usb_chg_type type;
	u32 watts;

	int rv = google_chromeec_get_usb_pd_power_info(&type, &watts);
	int sku_id;

	/* If we can't get charger info or not PD charger, assume barrel jack */
	if (rv != 0 || type != USB_CHG_TYPE_PD) {
		/* using the barrel jack, get PL2 based on sku id */
		watts = FIZZ_PL2_OTHERS;
		sku_id = gpio_base2_value(sku_id_gpios,
					  ARRAY_SIZE(sku_id_gpios));
		if (sku_id == FIZZ_SKU_ID_I7_U42)
			watts = FIZZ_PL2_I7_U42;
	} else
		watts = GET_TYPEC_PL2(watts);

	return watts;
}

static uint8_t board_oem_id(void)
{
	static int board_oem_id = -1;
	const gpio_t oem_id_gpios[] = {
		GPIO_OEM_ID1,
		GPIO_OEM_ID2,
		GPIO_OEM_ID3,
	};
	if (board_oem_id < 0)
		board_oem_id = gpio_base2_value(oem_id_gpios,
						ARRAY_SIZE(oem_id_gpios));
	return board_oem_id;
}

const char *smbios_mainboard_sku(void)
{
	static char sku_str[5]; /* sku{0..7} */

	snprintf(sku_str, sizeof(sku_str), "sku%d", board_oem_id());

	return sku_str;
}

static void mainboard_init(device_t dev)
{
	mainboard_ec_init();
}

static unsigned long mainboard_write_acpi_tables(
	device_t device, unsigned long current, acpi_rsdp_t *rsdp)
{
	uintptr_t start_addr;
	uintptr_t end_addr;
	struct nhlt *nhlt;

	start_addr = current;

	nhlt = nhlt_init();
	if (!nhlt)
		return start_addr;

	/* RT5663 Headset codec */
	if (nhlt_soc_add_rt5663(nhlt, AUDIO_LINK_SSP1))
		printk(BIOS_ERR, "Couldn't add headset codec.\n");

	end_addr = nhlt_soc_serialize_oem_overrides(nhlt, start_addr,
				oem_id, oem_table_id, 0);

	if (end_addr != start_addr)
		acpi_add_table(rsdp, (void *)start_addr);

	return end_addr;
}

static void mainboard_enable(device_t dev)
{
	device_t tpm;
	device_t root = SA_DEV_ROOT;
	config_t *conf = root->chip_info;

	conf->tdp_pl2_override = mainboard_get_pl2();

	dev->ops->init = mainboard_init;
	dev->ops->acpi_inject_dsdt_generator = chromeos_dsdt_generator;

	/* Disable unused interface for TPM. */
	if (!IS_ENABLED(CONFIG_FIZZ_USE_SPI_TPM)) {
		tpm = PCH_DEV_GSPI0;
		if (tpm)
			tpm->enabled = 0;
	}

	if (!IS_ENABLED(CONFIG_FIZZ_USE_I2C_TPM)) {
		tpm = PCH_DEV_I2C1;
		if (tpm)
			tpm->enabled = 0;
	}

	dev->ops->write_acpi_tables = mainboard_write_acpi_tables;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
