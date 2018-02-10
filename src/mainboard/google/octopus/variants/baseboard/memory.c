/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Intel Corp.
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

#include <baseboard/variants.h>
#include <gpio.h>
#include <soc/meminit.h>
#include <variant/gpio.h>

static const struct lpddr4_cfg lp4cfg = {
};

const struct lpddr4_cfg *__attribute__((weak)) variant_lpddr4_config(void)
{
	return &lp4cfg;
}

size_t __attribute__((weak)) variant_memory_sku(void)
{
	return 0;
}
