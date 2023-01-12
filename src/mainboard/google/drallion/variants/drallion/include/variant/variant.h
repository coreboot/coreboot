/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef VARIANT_H
#define VARIANT_H

#include <gpio.h>
#include <variant/gpio.h>

/* Need to update for Drallion with right SKU IDs*/
typedef struct {
	int id;
	const char *name;
} sku_info;

static const sku_info skus[] = {
	// Drallion 360
	{ .id = 1, .name = "sku1" },
	// Drallion
	{ .id = 2, .name = "sku2" },
	// Drallion 360 signed
	{ .id = 3, .name = "sku3" },
	// Drallion signed
	{ .id = 4, .name = "sku4" },
};

/* Return memory SKU for the variant */
int variant_memory_sku(void);

/* Check if the device has a 360 sensor board present */
static inline int has_360_sensor_board(void)
{
	return gpio_get(SENSOR_DET_360) == 0;
}

#endif
