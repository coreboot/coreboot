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
#define FIZZ_PSYSPL2_ALL   90
/*
 * For type-C chargers, set PL2 to 90% of max power to account for
 * cable loss and FET Rdson loss in the path from the source.
 */
#define GET_TYPEC_PL2(w)   (9 * (w) / 10)

#define OEM_ID_COUNT	3
#define SKU_ID_COUNT	7

/* List of BJ adapters shipped with Fizz or its variants */
enum bj_adapter {
	BJ_UNKNOWN,
	BJ_65W_19V,
	BJ_90W_19V,
	BJ_65W_19P5V,
	BJ_90W_19P5V,
	BJ_COUNT,
};

/* BJ adapter specs */
static const struct {
	uint16_t current_lim; /* in mA */
	uint16_t voltage_lim; /* in mV */
} bj_adapters[] = {
	[BJ_65W_19V] = { .current_lim = 3420, .voltage_lim = 19000 },
	[BJ_90W_19V] = { .current_lim = 4740, .voltage_lim = 19000 },
	[BJ_65W_19P5V] = { .current_lim = 3330, .voltage_lim = 19500 },
	[BJ_90W_19P5V] = { .current_lim = 4620, .voltage_lim = 19500 },
};

/*
 * The table showing which device is shipped with which BJ adapter.
 *
 *        | SKU0     SKU1     ...
 *   OEM0 | AdapterX AdapterZ ...
 *   OEM1 | AdapterY ...
 *   ...  |
 */
static const enum bj_adapter bj_adapter_table[OEM_ID_COUNT][SKU_ID_COUNT] = {
	{ BJ_65W_19P5V, BJ_65W_19P5V, BJ_90W_19P5V, BJ_90W_19P5V,
			BJ_90W_19P5V, BJ_90W_19P5V, BJ_65W_19P5V },
	{ BJ_65W_19V, BJ_65W_19V, BJ_UNKNOWN, BJ_UNKNOWN,
			BJ_90W_19V, BJ_90W_19V, BJ_UNKNOWN },
	{ BJ_65W_19V, BJ_65W_19V, BJ_90W_19V, BJ_90W_19V,
			BJ_90W_19V, BJ_90W_19V, BJ_65W_19V },
};

static const char *oem_id = "GOOGLE";
static const char *oem_table_id = "FIZZ";

static uint8_t board_sku_id(void)
{
	static int id = -1;
	const gpio_t sku_id_gpios[] = {
		GPIO_SKU_ID0,
		GPIO_SKU_ID1,
		GPIO_SKU_ID2,
		GPIO_SKU_ID3,
	};
	if (id < 0)
		id = gpio_base2_value(sku_id_gpios, ARRAY_SIZE(sku_id_gpios));
	return id;
}

/*
 * mainboard_set_power_limits
 *
 * Set Pl2 and SysPl2 values based on detected charger.
 */
static void mainboard_set_power_limits(u32 *pl2_val, u32 *psyspl2_val)
{
	enum usb_chg_type type;
	u32 watts;
	u32 pl2, psyspl2;

	int rv = google_chromeec_get_usb_pd_power_info(&type, &watts);

	/* If we can't get charger info or not PD charger, assume barrel jack */
	if (rv != 0 || type != USB_CHG_TYPE_PD) {
		/* using the barrel jack, get PL2 based on sku id */
		pl2 = FIZZ_PL2_OTHERS;
		if (board_sku_id() == FIZZ_SKU_ID_I7_U42)
			pl2 = FIZZ_PL2_I7_U42;
		/* PsysPl2 same for all SKUs */
		psyspl2 = FIZZ_PSYSPL2_ALL;
	} else {
		pl2 = GET_TYPEC_PL2(watts);
		psyspl2 = watts;
	}

	*pl2_val = pl2;
	*psyspl2_val = psyspl2;
}

static uint8_t board_oem_id(void)
{
	static int id = -1;
	const gpio_t oem_id_gpios[] = {
		GPIO_OEM_ID1,
		GPIO_OEM_ID2,
		GPIO_OEM_ID3,
	};
	if (id < 0)
		id = gpio_base2_value(oem_id_gpios, ARRAY_SIZE(oem_id_gpios));
	return id;
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

/*
 * Set max current and voltage for a barrel jack adapter based on {OEM, SKU}.
 * If this fails, the limit will remain unchanged = default values, which make
 * the system run under safe but under-rated power.
 * If a BJ adapter isn't plugged, this is a no-op.
 */
static void set_bj_adapter_limit(void)
{
	uint8_t oem = board_oem_id();
	uint8_t sku = board_sku_id();
	enum bj_adapter bj;

	if (oem >= OEM_ID_COUNT || sku >= SKU_ID_COUNT) {
		printk(BIOS_ERR, "Unrecognized OEM or SKU: %d/%d\n", oem, sku);
		return;
	}

	bj = bj_adapter_table[oem][sku];
	if (bj <= BJ_UNKNOWN || BJ_COUNT <= bj) {
		printk(BIOS_ERR, "Invalid BJ adapter ID: %d\n", bj);
		return;
	}
	printk(BIOS_INFO, "Setting BJ limit: %dmA/%dmV\n",
	       bj_adapters[bj].current_lim, bj_adapters[bj].voltage_lim);
	if (google_chromeec_override_dedicated_charger_limit(
			bj_adapters[bj].current_lim,
			bj_adapters[bj].voltage_lim))
		printk(BIOS_ERR, "Failed to set BJ limit\n");
}

static void mainboard_enable(device_t dev)
{
	device_t root = SA_DEV_ROOT;
	config_t *conf = root->chip_info;

	mainboard_set_power_limits(&conf->tdp_pl2_override, &conf->tdp_psyspl2);

	set_bj_adapter_limit();

	dev->ops->init = mainboard_init;
	dev->ops->acpi_inject_dsdt_generator = chromeos_dsdt_generator;
	dev->ops->write_acpi_tables = mainboard_write_acpi_tables;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
