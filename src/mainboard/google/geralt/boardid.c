/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <boardid.h>
#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include <soc/auxadc.h>
#include "panel.h"

/* board_id is provided by ec/google/chromeec/ec_boardid.c */

#define ADC_LEVELS 8

enum {
	/* RAM IDs */
	RAM_ID_LOW_CHANNEL = 2,
	RAM_ID_HIGH_CHANNEL = 3,
	/* PANEL IDs */
	PANEL_ID_HIGH_CHANNEL = 4,
	PANEL_ID_LOW_CHANNEL = 5,
};

static const unsigned int ram_voltages[] = {
	/* ID : Voltage (unit: uV) */
	[0] =   74296,
	[1] =  211673,
	[2] =  365055,
	[3] =  524272,
	[4] =  706302,
	[5] =  899119,
	[6] = 1108941,
	[7] = 1342616,
};

_Static_assert(ARRAY_SIZE(ram_voltages) == ADC_LEVELS, "Wrong array size of ram_voltages");

static const unsigned int panel_voltages[] = {
	/* ID : Voltage (unit: uV) */
	[0] =       0,
	[1] =  282774,
	[2] =  472379,
	[3] =  652542,
	[4] =  830258,
	[5] = 1011767,
	[6] = 1209862,
	[7] = 1427880,
};

_Static_assert(ARRAY_SIZE(panel_voltages) == ADC_LEVELS, "Wrong array size of panel_voltages");

static const unsigned int *adc_voltages[] = {
	[RAM_ID_LOW_CHANNEL] = ram_voltages,
	[RAM_ID_HIGH_CHANNEL] = ram_voltages,
	[PANEL_ID_HIGH_CHANNEL] = panel_voltages,
	[PANEL_ID_LOW_CHANNEL] = panel_voltages,
};

static uint32_t get_adc_index(unsigned int channel)
{
	unsigned int value = auxadc_get_voltage_uv(channel);

	assert(channel < ARRAY_SIZE(adc_voltages));
	const unsigned int *voltages = adc_voltages[channel];
	assert(voltages);

	/* Find the closest voltage */
	uint32_t id;
	for (id = 0; id < ADC_LEVELS - 1; id++)
		if (value < (voltages[id] + voltages[id + 1]) / 2)
			break;

	printk(BIOS_DEBUG, "ADC[%u]: Raw value=%u ID=%u\n", channel, value, id);
	return id;
}

/* Returns the ID for LCD module (type of panel). */
uint32_t panel_id(void)
{
	static uint32_t cached_panel_id = BOARD_ID_INIT;

	if (cached_panel_id == BOARD_ID_INIT)
		cached_panel_id = get_adc_index(PANEL_ID_HIGH_CHANNEL) << 4 |
				  get_adc_index(PANEL_ID_LOW_CHANNEL);

	return cached_panel_id;
}

uint32_t sku_id(void)
{
	static uint32_t cached_sku_code = BOARD_ID_INIT;

	if (cached_sku_code == BOARD_ID_INIT) {
		cached_sku_code = google_chromeec_get_board_sku();

		if (cached_sku_code == CROS_SKU_UNKNOWN ||
		    cached_sku_code == CROS_SKU_UNPROVISIONED) {
			printk(BIOS_WARNING, "SKU code from EC: %s\n",
			       (cached_sku_code == CROS_SKU_UNKNOWN) ?
			       "CROS_SKU_UNKNOWN" : "CROS_SKU_UNPROVISIONED");
			/* Reserve last 8 bits to report PANEL_IDs */
			cached_sku_code = 0x7FFFFF00UL | panel_id();
		}
		printk(BIOS_DEBUG, "SKU Code: %#02x\n", cached_sku_code);
	}

	return cached_sku_code;
}

uint32_t ram_code(void)
{
	static uint32_t cached_ram_code = BOARD_ID_INIT;

	if (cached_ram_code == BOARD_ID_INIT) {
		cached_ram_code = (get_adc_index(RAM_ID_HIGH_CHANNEL) << 4 |
				   get_adc_index(RAM_ID_LOW_CHANNEL));
		printk(BIOS_DEBUG, "RAM Code: %#02x\n", cached_ram_code);
	}

	return cached_ram_code;
}
