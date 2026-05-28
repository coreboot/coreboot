/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MAINBOARD_ONBOARD_H
#define __MAINBOARD_ONBOARD_H

/* NIC wake is GPIO 8 */
#define NIC_WAKE_GPIO		8

/* WLAN wake is GPIO 10 */
#define WLAN_WAKE_GPIO		10

/* Recovery: GPIO12 = RECOVERY_L, active low */
#define GPIO_REC_MODE	12

/* Write Protect: GPIO58 = GPIO_SPI_WP, active high */
#define GPIO_SPI_WP	58

/* IT8772F defs */
#define IT8772F_BASE 0x2e
#define IT8772F_SERIAL_DEV PNP_DEV(IT8772F_BASE, IT8772F_SP1)
#define IT8772F_GPIO_DEV PNP_DEV(IT8772F_BASE, IT8772F_GPIO)
#define IT8772F_EC_DEV PNP_DEV(IT8772F_BASE, IT8772F_EC)

#ifndef __ACPI__
void set_power_led(int state);

enum {
	LED_OFF		= 0,
	LED_ON,
	LED_BLINK,
};
#endif

#endif /* __MAINBOARD_ONBOARD_H */
