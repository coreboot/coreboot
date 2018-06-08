/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015 Intel Corp.
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

#ifndef ONBOARD_H
#define ONBOARD_H

#include <mainboard/google/cyan/irqroute.h>

/*
 * Calculation of gpio based irq.
 * Gpio banks ordering : GPSW, GPNC, GPEC, GPSE
 * Max direct irq (MAX_DIRECT_IRQ) is 114.
 * Size of gpio banks are
 * GPSW_SIZE = 98
 * GPNC_SIZE = 73
 * GPEC_SIZE = 27
 * GPSE_SIZE = 86
 */

#define BOARD_TOUCH_IRQ			184

/* KBD: Gpio index in N bank */
#define BOARD_I8042_GPIO_INDEX		17
/* SCI: Gpio index in N bank */
#define BOARD_SCI_GPIO_INDEX		15
/* Trackpad: Gpio index in N bank */
#define BOARD_TRACKPAD_GPIO_INDEX	18
/* Touch: Gpio index in N bank */
#define BOARD_TOUCH_GPIO_INDEX		19
/* TI Audio: Gpio index in SW bank */
#define BOARD_JACK_TI_GPIO_INDEX	34
/* MAXIM Audio: Gpio index in SE bank */
#define BOARD_JACK_MAXIM_GPIO_INDEX	77

#define BOARD_TRACKPAD_NAME             "trackpad"
#define BOARD_TRACKPAD_WAKE_GPIO        ACPI_ENABLE_WAKE_SUS_GPIO(1)
#define BOARD_TRACKPAD_I2C_BUS          5
#define BOARD_TRACKPAD_I2C_ADDR         0x15

#define BOARD_TOUCHSCREEN_NAME          "touchscreen"
#define BOARD_TOUCHSCREEN_WAKE_GPIO     ACPI_ENABLE_WAKE_SUS_GPIO(2)
#define BOARD_TOUCHSCREEN_I2C_BUS       0
#define BOARD_TOUCHSCREEN_I2C_ADDR      0x10


/* SD CARD gpio */
#define SDCARD_CD			81

#define AUDIO_CODEC_HID			"193C9890"
#define AUDIO_CODEC_CID			"193C9890"
#define AUDIO_CODEC_DDN			"Maxim 98090 Codec  "
#define AUDIO_CODEC_I2C_ADDR		0x10

#define TI_SWITCH_HID			"104C227E"
#define TI_SWITCH_CID			"104C227E"
#define TI_SWITCH_DDN			"TI SWITCH "
#define TI_SWITCH_I2C_ADDR		0x3B

#define DPTF_CPU_PASSIVE	88
#define DPTF_CPU_CRITICAL	90

#endif
