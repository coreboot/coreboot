/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <boardid.h>
#include <console/console.h>
#include <soc/auxadc.h>
#include <ec/google/chromeec/ec.h>

/* For CBI un-provisioned/corrupted Flapjack board. */
#define FLAPJACK_UNDEF_SKU_ID 0

#define ADC_LEVELS 12

enum {
	LCM_ID_CHANNEL = 2,  /* ID of LCD Module on schematics. */
	RAM_ID_CHANNEL = 3,
	SKU_ID_CHANNEL = 4,
};

static const int ram_voltages[ADC_LEVELS] = {
	/* ID : Voltage (unit: uV) */
	/*  0 : */   74000,
	/*  1 : */  212000,
	/*  2 : */  319000,
	/*  3 : */  429000,
	/*  4 : */  542000,
	/*  5 : */  666000,
	/*  6 : */  781000,
	/*  7 : */  900000,
	/*  8 : */ 1023000,
	/*  9 : */ 1137000,
	/* 10 : */ 1240000,
	/* 11 : */ 1343000,
};

static const int lcm_voltages[ADC_LEVELS] = {
	/* ID : Voltage (unit: uV) */
	/*  0 : */       0,
	/*  1 : */  283000,
	/*  2 : */  394000,
	/*  3 : */  503000,
	/*  4 : */  608000,
	/*  5 : */  712000,
	/*  6 : */  823000,
	/*  7 : */  937000,
	/*  8 : */ 1046000,
	/*  9 : */ 1155000,
	/* 10 : */ 1277000,
	/* 11 : */ 1434000,
};

static const int *adc_voltages[] = {
	[LCM_ID_CHANNEL] = lcm_voltages,
	[RAM_ID_CHANNEL] = ram_voltages,
	[SKU_ID_CHANNEL] = ram_voltages, /* SKU ID is sharing RAM voltages. */
};

static uint32_t get_adc_index(unsigned int channel)
{
	int value = auxadc_get_voltage(channel);

	assert(channel < ARRAY_SIZE(adc_voltages));
	const int *voltages = adc_voltages[channel];
	assert(voltages);

	/* Find the closest voltage */
	uint32_t id;
	for (id = 0; id < ADC_LEVELS - 1; id++)
		if (value < (voltages[id] + voltages[id + 1]) / 2)
			break;
	printk(BIOS_DEBUG, "ADC[%d]: Raw value=%d ID=%d\n", channel, value, id);
	return id;
}

/* board_id is provided by ec/google/chromeec/ec_boardid.c */

uint32_t sku_id(void)
{
	static uint32_t cached_sku_id = BOARD_ID_INIT;

	if (cached_sku_id != BOARD_ID_INIT)
		return cached_sku_id;

	/* On Flapjack, getting the SKU via CBI. */
	if (CONFIG(BOARD_GOOGLE_FLAPJACK)) {
		if (google_chromeec_cbi_get_sku_id(&cached_sku_id))
			cached_sku_id = FLAPJACK_UNDEF_SKU_ID;
		return cached_sku_id;
	}

	/* Quirk for Kukui: All Rev1/Sku0 had incorrectly set SKU_ID=1. */
	if (CONFIG(BOARD_GOOGLE_KUKUI)) {
		if (board_id() == 1) {
			cached_sku_id = 0;
			return cached_sku_id;
		}
	}

	/*
	 * The SKU (later used for device tree matching) is combined from:
	 * ADC2[4bit/H] = straps on LCD module (type of panel).
	 * ADC4[4bit/L] = SKU ID from board straps.
	 */
	cached_sku_id = (get_adc_index(LCM_ID_CHANNEL) << 4 |
			 get_adc_index(SKU_ID_CHANNEL));
	return cached_sku_id;
}

uint32_t ram_code(void)
{
	static uint32_t cached_ram_code = BOARD_ID_INIT;

	if (cached_ram_code == BOARD_ID_INIT)
		cached_ram_code = get_adc_index(RAM_ID_CHANNEL);
	return cached_ram_code;
}
