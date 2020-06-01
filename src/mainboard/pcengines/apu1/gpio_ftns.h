/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef GPIO_FTNS_H
#define GPIO_FTNS_H

#include <stdint.h>

void configure_gpio(u8 gpio, u8 iomux_ftn, u8 setting);
u8 read_gpio(u8 gpio);
int get_spd_offset(void);

#define GPIO_10         10    // PE3 Reset
#define GPIO_11         11    // PE4 Reset
#define GPIO_15         15    // board rev strap ms bit
#define GPIO_16         16    // board rev strap ls bit
#define GPIO_17         17    // TP13
#define GPIO_18         18    // TP10
#define GPIO_187        187   // MODESW
#define GPIO_189        189   // LED1#
#define GPIO_190        190   // LED2#
#define GPIO_191        191   // LED3#
#define GPIO_FTN_1      0x01
#define GPIO_OUTPUT     0x08
#define GPIO_INPUT      0x28
#define GPIO_DATA_IN    0x80
#define GPIO_DATA_LOW   0x00
#define GPIO_DATA_HIGH  0x40

#endif /* GPIO_FTNS_H */
