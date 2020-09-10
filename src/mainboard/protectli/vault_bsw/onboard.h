/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef ONBOARD_H
#define ONBOARD_H

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

/* Audio: Gpio index in SW bank */
#define JACK_DETECT_GPIO_INDEX		77

/* SCI: Gpio index in N bank */
#define BOARD_SCI_GPIO_INDEX		15

#define SDCARD_CD			81

#define AUDIO_CODEC_HID			"10EC5670"
#define AUDIO_CODEC_CID			"10EC5670"
#define AUDIO_CODEC_DDN			"RTEK Codec Controller "
#define AUDIO_CODEC_I2C_ADDR		0x1C

#define BCRD2_PMIC_I2C_BUS		0x01

#endif
