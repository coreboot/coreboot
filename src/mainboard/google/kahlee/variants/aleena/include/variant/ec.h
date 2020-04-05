/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <baseboard/ec.h>

/* Enable EC backed Keyboard Backlight in ACPI */
#define EC_ENABLE_KEYBOARD_BACKLIGHT

/* Enable Tablet switch */
#define EC_ENABLE_TBMC_DEVICE

/*
 * Enable EC sync interrupt via GPIO controller, EC_SYNC_IRQ is defined in
 * variant/gpio.h
 */
#define EC_ENABLE_SYNC_IRQ_GPIO
