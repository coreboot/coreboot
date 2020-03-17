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

#ifndef ONBOARD_H
#define ONBOARD_H

#include <mainboard/google/rambi/irqroute.h>

#ifndef __ACPI__
void lan_init(void);
#endif

/* defines for programming the MAC address */
#define SUMO_NIC_VENDOR_ID		0x10EC
#define SUMO_NIC_DEVICE_ID		0x8168

/* 0x00: White LINK LED and Amber ACTIVE LED */
#define SUMO_NIC_LED_MODE		0x00

/* PCH wake signal from EC. */
#define BOARD_PCH_WAKE_GPIO             ACPI_ENABLE_WAKE_SUS_GPIO(0)

#define BOARD_TOUCHSCREEN_NAME          "touchscreen"
#define BOARD_TOUCHSCREEN_IRQ           GPIO_S0_DED_IRQ(TOUCH_IRQ_OFFSET)
#define BOARD_TOUCHSCREEN_WAKE_GPIO     ACPI_ENABLE_WAKE_SUS_GPIO(2)
#define BOARD_TOUCHSCREEN_I2C_BUS       5
#define BOARD_TOUCHSCREEN_I2C_ADDR      0x2c

#define BOARD_I8042_IRQ                 GPIO_S0_DED_IRQ(I8042_IRQ_OFFSET)
#define BOARD_CODEC_IRQ                 GPIO_S5_DED_IRQ(CODEC_IRQ_OFFSET)

#endif
