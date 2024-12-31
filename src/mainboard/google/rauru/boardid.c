/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <boardid.h>
#include <console/console.h>
#include <device/mmio.h>
#include <ec/google/chromeec/ec.h>
#include <soc/mt6363_sdmadc.h>

#include "storage.h"

#define ADC_LEVELS 8

DEFINE_BITFIELD(STORAGE_TYPE, 11, 9);
DEFINE_BITFIELD(CPU_TYPE, 8, 8);
DEFINE_BITFIELD(PANEL_TYPE, 7, 0);

enum {
	/* Storage IDs */
	STORAGE_ID_LOW_CHANNEL = AUXADC_CHAN_VIN1,
};

static const unsigned int storageid_voltages[ADC_LEVELS] = {
	/* ID : Voltage (unit: mV) */
	[0]  =   50,
	[1]  =  210,
	[2]  =  420,
	[3]  =  620,
	[4]  =  820,
	[5]  = 1040,
	[6]  = 1240,
	[7]  = 1450,
};

static const unsigned int *adc_voltages[] = {
	[STORAGE_ID_LOW_CHANNEL] = storageid_voltages,
};

static uint32_t get_adc_index(unsigned int channel)
{
	u32 value;

	mt6363_sdmadc_read(channel, &value, SDMADC_OPEN, AUXADC_VAL_PROCESSED);
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

uint32_t storage_id(void)
{
	static uint32_t cached_storage_id = BOARD_ID_INIT;

	if (cached_storage_id == BOARD_ID_INIT)
		cached_storage_id = get_adc_index(STORAGE_ID_LOW_CHANNEL);

	printk(BIOS_DEBUG, "Storage ID: %#02x\n", cached_storage_id);
	return cached_storage_id;
}

enum ufs_type storage_type(uint32_t index)
{
	switch (index) {
	case 0:
		return UFS_40;
	case 1:
		return UFS_31;
	case 2:
		return UFS_40_HS;
	default:
		printk(BIOS_DEBUG, "unsupported type %d\n", index);
	}
	return UFS_UNKNOWN;
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
			SET32_BITFIELDS(&cached_sku_code, STORAGE_TYPE, storage_id());
		}
		printk(BIOS_DEBUG, "SKU Code: %#02x\n", cached_sku_code);
	}

	return cached_sku_code;
}
