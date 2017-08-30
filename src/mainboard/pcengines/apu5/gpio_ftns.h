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

void configure_gpio(u32 iomux_gpio, u8 iomux_ftn, u32 gpio, u32 setting);
u8 read_gpio(u32 gpio);
void write_gpio(u32 gpio, u8 value);
int get_spd_offset(void);

#define IOMUX_OFFSET    0xD00
#define GPIO_OFFSET     0x1500

#define IOMUX_GPIO_32   0x59    // SIMSWAP2
#define IOMUX_GPIO_33   0x5A    // SIMSWAP3
#define IOMUX_GPIO_49   0x40    // STRAP0
#define IOMUX_GPIO_50   0x41    // STRAP1
#define IOMUX_GPIO_51   0x42    // SIM1 Reset
#define IOMUX_GPIO_55   0x43    // SIM2 Reset
#define IOMUX_GPIO_57   0x44    // LED1#
#define IOMUX_GPIO_58   0x45    // LED2#
#define IOMUX_GPIO_59   0x46    // LED3#
#define IOMUX_GPIO_64   0x47    // SIM3 Reset
#define IOMUX_GPIO_66   0x5B    // SPKR
#define IOMUX_GPIO_68   0x48    // SIMSWAP1
#define IOMUX_GPIO_71   0x4D    // PROCHOT

#define GPIO_32   0x164    // SIMSWAP2
#define GPIO_33   0x168    // SIMSWAP3
#define GPIO_49   0x100    // STRAP0
#define GPIO_50   0x104    // STRAP1
#define GPIO_51   0x108    // SIM1 Reset
#define GPIO_55   0x10C    // SIM2 Reset
#define GPIO_57   0x110    // LED1#
#define GPIO_58   0x114    // LED2#
#define GPIO_59   0x118    // LED3#
#define GPIO_64   0x11C    // SIM3 Reset
#define GPIO_66   0x16C    // SPKR
#define GPIO_68   0x120    // SIMSWAP1
#define GPIO_71   0x134    // PROCHOT

#define GPIO_OUTPUT_ENABLE      BIT23
#define GPIO_OUTPUT_VALUE       BIT22
#define GPIO_PULL_DOWN_ENABLE   BIT21
#define GPIO_PULL_UP_ENABLE     BIT20
#define GPIO_PIN_STS            BIT16

#endif /* GPIO_FTNS_H */
