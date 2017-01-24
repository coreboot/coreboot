/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Sage Electronic Engineering, LLC
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

void configure_gpio(uintptr_t base_addr, u32 iomux_gpio, u8 iomux_ftn, u32 setting);
u8 read_gpio(uintptr_t base_addr, u32 gpio);
void write_gpio(uintptr_t base_addr, u32 iomux_gpio, u8 value);

#define IOMUX_OFFSET    0xD00
#define GPIO_OFFSET     0x1500

//
// Based on PC Engines APU3A schematics
// http://pcengines.ch/schema/apu3a.pdf
//
#define IOMUX_GPIO_32   0x59    // MODESW
#define IOMUX_GPIO_49   0x40    // STRAP0
#define IOMUX_GPIO_50   0x41    // STRAP1
#define IOMUX_GPIO_51   0x42    // PE3 Reset
#define IOMUX_GPIO_55   0x43    // PE4 Reset
#define IOMUX_GPIO_57   0x44    // LED1#
#define IOMUX_GPIO_58   0x45    // LED2#
#define IOMUX_GPIO_59   0x46    // LED3#
#define IOMUX_GPIO_64   0x47    // PE3_WDIS
#define IOMUX_GPIO_66   0x5B    // SPKR
#define IOMUX_GPIO_68   0x48    // PE4_WDIS
#define IOMUX_GPIO_71   0x4D    // PROCHOT

#define GPIO_OUTPUT_ENABLE      BIT7
#define GPIO_OUTPUT_VALUE       BIT6
#define GPIO_PULL_DOWN_ENABLE   BIT5
#define GPIO_PULL_UP_ENABLE     BIT4

#define GPIO_DATA_IN    0x80

#endif /* GPIO_FTNS_H */
