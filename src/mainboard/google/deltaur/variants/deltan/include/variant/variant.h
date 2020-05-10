/*
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef VARIANT_H
#define VARIANT_H

#include <baseboard/variants.h>
#include <gpio.h>
#include <variant/gpio.h>

const static sku_info skus[] = {
	/* Deltan 360 - invalid configuration */
	{ .id = -1, .name = "sku_invalid" },
	/* Deltan */
	{ .id = 1, .name = "sku1" },
	/* Deltan 360 signed - invalid configuration */
	{ .id = -1, .name = "sku_invalid" },
	/* Deltan signed */
	{ .id = 2, .name = "sku2" },
};

#endif
