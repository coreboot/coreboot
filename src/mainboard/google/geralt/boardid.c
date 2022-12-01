/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <boardid.h>
#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include <soc/auxadc.h>
#include "panel.h"

/* board_id is provided by ec/google/chromeec/ec_boardid.c */

#define ADC_LEVELS 12

enum {
	/* RAM IDs */
	RAM_ID_LOW_CHANNEL = 2,
	RAM_ID_HIGH_CHANNEL = 3,
	/* SKU ID */
	SKU_ID_CHANNEL = 4,
	/* PANEL ID */
	PANEL_ID_CHANNEL = 5,
};

static const unsigned int ram_voltages[ADC_LEVELS] = {
	/* ID : Voltage (unit: uV) */
	[0]  =   74300,
	[1]  =  211700,
	[2]  =  318800,
	[3]  =  428600,
	[4]  =  541700,
	[5]  =  665800,
	[6]  =  781400,
	[7]  =  900000,
	[8]  = 1023100,
	[9]  = 1137000,
	[10] = 1240000,
	[11] = 1342600,
};

static const unsigned int panel_voltages[ADC_LEVELS] = {
	/* ID : Voltage (unit: uV) */
	[0]  =       0,
	[1]  =  283000,
	[2]  =  394000,
	[3]  =  503000,
	[4]  =  608000,
	[5]  =  712000,
	[6]  =  823000,
	[7]  =  937000,
	[8]  = 1046000,
	[9]  = 1155000,
	[10] = 1277000,
	[11] = 1434000,
};

static const unsigned int *adc_voltages[] = {
	[RAM_ID_LOW_CHANNEL] = ram_voltages,
	[RAM_ID_HIGH_CHANNEL] = ram_voltages,
	[SKU_ID_CHANNEL] = ram_voltages,
	[PANEL_ID_CHANNEL] = panel_voltages,
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
		cached_panel_id = get_adc_index(PANEL_ID_CHANNEL);

	return cached_panel_id;
}

uint32_t sku_id(void)
{
	static uint32_t cached_sku_code = BOARD_ID_INIT;

	if (cached_sku_code == BOARD_ID_INIT) {
		cached_sku_code = google_chromeec_get_board_sku();

		if (cached_sku_code == CROS_SKU_UNKNOWN) {
			printk(BIOS_WARNING, "Failed to get SKU code from EC\n");
			cached_sku_code = get_adc_index(SKU_ID_CHANNEL);
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
