/*
 * This file is part of the coreboot project.
 *
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

#ifndef VARIANT_EC_H
#define VARIANT_EC_H

#include <soc/gpe.h>
#include <soc/gpio.h>

/* EC wake pin */
#define EC_WAKE_PIN		GPE0_DW1_12

/* eSPI virtual wire reporting */
#define EC_SCI_GPI		GPE0_ESPI

/* Enable PS/2 keyboard */
#define SIO_EC_ENABLE_PS2K

/* Enable DPTF */
#define EC_ENABLE_DPTF

/* Enable privacy screen functionality */
#define EC_ENABLE_PRIVACY

#endif
