/*
 * This file is part of the coreboot project.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef VARIANT_H
#define VARIANT_H

#include <baseboard/variants.h>

/* TODO b/153027724: Sensor detection pin */
#define SENSOR_DET_360          GPP_C10

const static sku_info skus[] = {
	/* Deltaur 360 */
	{ .id = 1, .name = "sku1" },
	/* Deltaur */
	{ .id = 2, .name = "sku2" },
	/* Deltaur 360 signed */
	{ .id = 3, .name = "sku3" },
	/* Deltaur signed */
	{ .id = 4, .name = "sku4" },
};

#endif
