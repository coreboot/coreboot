/* SPDX-License-Identifier: GPL-2.0-only */

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

/* Drallion has 2 type-C ports */
#define EC_BOARD_HAS_2ND_TYPEC_PORT

#endif
