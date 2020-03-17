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

#ifndef GPIO_FTNS_H
#define GPIO_FTNS_H

#include <stdint.h>

uintptr_t find_gpio_base(void);
void configure_gpio(uintptr_t base_addr, u8 gpio, u8 iomux_ftn, u8 setting);
u8 read_gpio(uintptr_t base_addr, u8 gpio);
int get_spd_offset(void);

#define IOMUX_OFFSET    0xD00
#define GPIO_OFFSET     0x100
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
