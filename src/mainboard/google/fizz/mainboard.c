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

#define FIZZ_SKU_ID_I7_U42  0x4
#define FIZZ_SKU_ID_I5_U42  0x5
#define FIZZ_SKU_ID_I3_U42  0x6
#define FIZZ_SKU_ID_I7_U22  0x3
#define FIZZ_SKU_ID_I5_U22  0x2
#define FIZZ_SKU_ID_I3_U22  0x1
#define FIZZ_SKU_ID_CEL_U22 0x0
#define FIZZ_PL2_U42        44
#define FIZZ_PL2_U22        29
#define FIZZ_PSYSPL2_U22    65
#define FIZZ_PSYSPL2_U42    90
#define FIZZ_MAX_TIME_WINDOW 6
#define FIZZ_MIN_DUTYCYCLE   4
/*
 * For type-C chargers, set PL2 to 90% of max power to account for
 * cable loss and FET Rdson loss in the path from the source.
 */
#define SET_PSYSPL2(w)     (9 * (w) / 10)

static uint8_t read_sku_id_from_gpio(void)
{
	const gpio_t sku_id_gpios[] = {
		GPIO_SKU_ID0,
		GPIO_SKU_ID1,
		GPIO_SKU_ID2,
		GPIO_SKU_ID3,
	};
	return gpio_base2_value(sku_id_gpios, ARRAY_SIZE(sku_id_gpios));
}

static uint8_t board_sku_id(void)
{
	static int sku_id = -1;

	if (sku_id < 0) {
		uint32_t id;
		if (google_chromeec_cbi_get_sku_id(&id))
			/* TODO: Once transition completes, raise error instead
			   of returning gpio value which could be unintended. */
			/* Reading from EC may succeed next time but we do not
			   want to return different values. So, we cache the
			   value read from GPIOs. */
			id = read_sku_id_from_gpio();
		sku_id = id;
	}

	return sku_id;
}

/*
 * mainboard_set_power_limits
 *
 * Set Pl2 and SysPl2 values based on detected charger.
 * If detected barrel jack, use values below based on SKU.
 * definitions:
 * x = no value entered. Use default value in parenthesis.
 *     will set 0 to anything that shouldn't be set.
 * n = max value of power adapter.
 * +-------------+-----+---------+-----------+-------+
 * | sku_id      | PL2 | PsysPL2 |  PsysPL3  |  PL4  |
 * +-------------+-----+---------+-----------+-------+
 * | i7 U42      |  44 |   81    | x(.85PL4) | x(71) |
 * | i5 U42      |  44 |   81    | x(.85PL4) | x(71) |
 * | i3 U42      |  44 |   81    | x(.85PL4) | x(71) |
 * | i7 U22      |  29 |   58    | x(.85PL4) | x(43) |
 * | i5 U22      |  29 |   58    | x(.85PL4) | x(43) |
 * | i3 U22      |  29 |   58    | x(.85PL4) | x(43) |
 * | celeron U22 |  29 |   58    | x(.85PL4) | x(43) |
 * +-------------+-----+---------+-----------+-------+
 * For USB C charger:
 * +-------------+-----+---------+---------+-------+
 * | Max Power(W)| PL2 | PsysPL2 | PsysPL3 |  PL4  |
 * +-------------+-----+---------+---------+-------+
 * | 60 (U42)    |  44 |   54    |    54   |   54  |
 * | 60 (U22)    |  29 |   54    |    54   | x(43) |
 * | n  (U42)    |  44 |   .9n   |   .9n   |  .9n  |
 * | n  (U22)    |  29 |   .9n   |   .9n   | x(43) |
 * +-------------+-----+---------+---------+-------+
 */
static void mainboard_set_power_limits(config_t *conf)
{
	enum usb_chg_type type;
	u32 watts;
	u32 pl2, psyspl2;
	int rv = google_chromeec_get_usb_pd_power_info(&type, &watts);
	uint8_t sku = board_sku_id();
	const uint32_t u42_mask = (1 << FIZZ_SKU_ID_I7_U42) |
				  (1 << FIZZ_SKU_ID_I5_U42) |
				  (1 << FIZZ_SKU_ID_I3_U42);

	/* PL2 value is sku-based, no matter what charger we are using */
	pl2 = FIZZ_PL2_U22;
	if ((1 << sku) & u42_mask)
		pl2 = FIZZ_PL2_U42;
	conf->tdp_psyspl3 = conf->tdp_pl4 = 0;

	/* If we can't get charger info or not PD charger, assume barrel jack */
	if (rv != 0 || type != USB_CHG_TYPE_PD) {
		/* using the barrel jack, get PsysPL2 based on sku id */
		psyspl2 = FIZZ_PSYSPL2_U22;
		/* Running a U42 SKU */
		if ((1 << sku) & u42_mask)
			psyspl2 = FIZZ_PSYSPL2_U42;
	} else {
		/* Detected TypeC.  Base on max value of adapter */
		psyspl2 = watts;
		conf->tdp_psyspl3 = SET_PSYSPL2(psyspl2);
		/* set max possible time window */
		conf->tdp_psyspl3_time = FIZZ_MAX_TIME_WINDOW;
		/* set minimum duty cycle */
		conf->tdp_psyspl3_dutycycle = FIZZ_MIN_DUTYCYCLE;
		if ((1 << sku) & u42_mask)
			conf->tdp_pl4 = SET_PSYSPL2(psyspl2);
	}

	conf->tdp_pl2_override = pl2;
	/* set psyspl2 to 90% of max adapter power */
	conf->tdp_psyspl2 = SET_PSYSPL2(psyspl2);
}

static uint8_t read_oem_id_from_gpio(void)
{
	const gpio_t oem_id_gpios[] = {
		GPIO_OEM_ID1,
		GPIO_OEM_ID2,
		GPIO_OEM_ID3,
	};
	return gpio_base2_value(oem_id_gpios, ARRAY_SIZE(oem_id_gpios));
}

static uint8_t board_oem_id(void)
{
	static int oem_id = -1;

	if (oem_id < 0) {
		uint32_t id;
		if (google_chromeec_cbi_get_oem_id(&id))
			/* TODO: Once transition completes, raise error instead
			   of returning gpio value which could be unintended. */
			/* Reading from EC may succeed next time but we do not
			   want to return different values. So, we cache the
			   value read from GPIOs. */
			id = read_oem_id_from_gpio();
		oem_id = id;
	}

	return oem_id;
}

const char *smbios_mainboard_sku(void)
{
	static char sku_str[5]; /* sku{0..7} */

	snprintf(sku_str, sizeof(sku_str), "sku%d", board_oem_id());

	return sku_str;
}

static void mainboard_init(struct device *dev)
{
	mainboard_ec_init();
}

static unsigned long mainboard_write_acpi_tables(
	struct device *device, unsigned long current, acpi_rsdp_t *rsdp)
{
	const char *oem_id = "GOOGLE";
	const char *oem_table_id = "FIZZ";
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

static void mainboard_enable(struct device *dev)
{
	struct device *root = SA_DEV_ROOT;
	config_t *conf = root->chip_info;

	mainboard_set_power_limits(conf);

	dev->ops->init = mainboard_init;
	dev->ops->acpi_inject_dsdt_generator = chromeos_dsdt_generator;
	dev->ops->write_acpi_tables = mainboard_write_acpi_tables;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
