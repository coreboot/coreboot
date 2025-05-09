/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <boardid.h>
#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include <soc/auxadc_common.h>

#include "storage.h"

#define ADC_LEVELS 8

enum {
	/* STORAGE IDs */
	STORAGE_ID_CHANNEL = 2,
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

static const unsigned int *adc_voltages[] = {
	[STORAGE_ID_CHANNEL] = storage_voltages,
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

uint32_t sku_id(void)
{
	static uint32_t cached_sku_code = BOARD_ID_INIT;

	if (cached_sku_code == BOARD_ID_INIT) {
		cached_sku_code = google_chromeec_get_board_sku();

		if (cached_sku_code == CROS_SKU_UNKNOWN ||
		    cached_sku_code == CROS_SKU_UNPROVISIONED) {
			printk(BIOS_WARNING, "Failed to get SKU code from EC\n");
			cached_sku_code = CROS_SKU_UNPROVISIONED;
		}
		printk(BIOS_DEBUG, "SKU Code: %#02x\n", cached_sku_code);
	}

	return cached_sku_code;
}
