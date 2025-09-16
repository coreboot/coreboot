/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <boardid.h>
#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include <soc/auxadc_common.h>

#include "panel.h"
#include "storage.h"

#define ADC_LEVELS 8
#define PANEL_ADC_LEVELS 3

enum {
	/* STORAGE IDs */
	STORAGE_ID_CHANNEL = 2,
	/* SKU IDs */
	PANEL_ID_LOW_CHANNEL = 4,
	PANEL_ID_HIGH_CHANNEL = 5,
};

static const unsigned int storage_voltages[ADC_LEVELS] = {
	/* ID : Voltage (unit: uV) */
	[0] =   74300,
	[1] =  211400,
	[2] =  365100,
	[3] =  524300,
	[4] =  706300,
	[5] =  899100,
	[6] = 1108900,
	[7] = 1342600,
};

static const unsigned int panel_voltages[PANEL_ADC_LEVELS] = {
	/* ID : Voltage (unit: uV) */
	[0] =       0,
	[1] =  500000,
	[2] = 1000000,
};

static const unsigned int *adc_voltages[] = {
	[STORAGE_ID_CHANNEL] = storage_voltages,
	[PANEL_ID_LOW_CHANNEL] = panel_voltages,
	[PANEL_ID_HIGH_CHANNEL] = panel_voltages,
};

static uint32_t get_adc_index(unsigned int channel)
{
	unsigned int value = auxadc_get_voltage_uv(channel);
	const unsigned int *voltages;

	assert(channel < ARRAY_SIZE(adc_voltages));
	voltages = adc_voltages[channel];

	assert(voltages);

	/* Find the closest voltage */
	uint32_t id;
	for (id = 0; id < ADC_LEVELS - 1; id++)
		if (value < (voltages[id] + voltages[id + 1]) / 2)
			break;

	printk(BIOS_DEBUG, "ADC[%u]: Raw value=%u ID=%u\n", channel, value, id);
	return id;
}

uint32_t storage_id(void)
{
	static uint32_t cached_storage_id = BOARD_ID_INIT;

	if (cached_storage_id == BOARD_ID_INIT)
		cached_storage_id = get_adc_index(STORAGE_ID_CHANNEL);

	printk(BIOS_DEBUG, "Storage ID: %#02x\n", cached_storage_id);
	return cached_storage_id;
}

static uint32_t cal_panel_id(unsigned int channel)
{
	unsigned int value = auxadc_get_voltage_uv(channel);
	const unsigned int *voltages;

	assert(channel < ARRAY_SIZE(adc_voltages));
	voltages = adc_voltages[channel];
	assert(voltages);

	uint32_t id;

	for (id = 0; id < PANEL_ADC_LEVELS - 1; id++) {
		if (value < voltages[id + 1]) {
			printk(BIOS_DEBUG, "ADC[%u]: Raw value=%u ID=%u\n",
			       channel, value, id);
			return id;
		}
	}
	printk(BIOS_DEBUG, "ADC[%u]: Raw value=%u ID=%u\n", channel, value, id);
	return PANEL_ADC_LEVELS - 1;
}

uint8_t panel_id(void)
{
	static uint32_t cached_panel_id = BOARD_ID_INIT;

	if (cached_panel_id == BOARD_ID_INIT) {
		uint32_t high_id = cal_panel_id(PANEL_ID_HIGH_CHANNEL);
		uint32_t low_id  = cal_panel_id(PANEL_ID_LOW_CHANNEL);
		cached_panel_id = (high_id & 0xF) << 4 | (low_id & 0xF);
	}

	return cached_panel_id;
}

uint32_t sku_id(void)
{
	static uint32_t cached_sku_code = BOARD_ID_INIT;

	if (cached_sku_code != BOARD_ID_INIT)
		return cached_sku_code;

	const uint32_t cbi_sku_id = google_chromeec_get_board_sku();
	const uint32_t panel = panel_id();
	uint32_t effective_sku = cbi_sku_id;

	if (cbi_sku_id == CROS_SKU_UNKNOWN ||
	    cbi_sku_id == CROS_SKU_UNPROVISIONED) {
		printk(BIOS_WARNING, "Failed to get SKU code from EC\n");
		effective_sku = CROS_SKU_UNPROVISIONED;
	}
	if (CONFIG(BOARD_GOOGLE_PADME)) {
		/* Reserve last 8 bits to report PANEL_IDs */
		cached_sku_code = (effective_sku & 0xFFFFFF00) | (panel_id() & 0xFF);
	} else {
		cached_sku_code = effective_sku;
	}
	printk(BIOS_DEBUG,
	       "CBI SKU ID: %#02x, panel ID: %#02x, cached SKU code: %#02x\n",
	       cbi_sku_id, panel, cached_sku_code);

	return cached_sku_code;
}
