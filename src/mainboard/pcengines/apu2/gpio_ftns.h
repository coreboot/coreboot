/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef GPIO_FTNS_H
#define GPIO_FTNS_H

int get_spd_offset(void);

//
// Based on PC Engines APU2C and APU3A schematics
// http://www.pcengines.ch/schema/apu2c.pdf
// http://www.pcengines.ch/schema/apu3a.pdf
//
#define GPIO_22   0x09    // MODESW (APU5)
#define GPIO_32   0x59    // MODESW (SIMSWAP2 on APU5)
#define GPIO_33   0x5A    // SIMSWAP (SIMSWAP3 on APU5)
#define GPIO_49   0x40    // STRAP0
#define GPIO_50   0x41    // STRAP1
#define GPIO_51   0x42    // PE3 Reset (SIM1 Reset on APU5)
#define GPIO_55   0x43    // PE4 Reset (SIM2 Reset on APU5)
#define GPIO_57   0x44    // LED1#
#define GPIO_58   0x45    // LED2#
#define GPIO_59   0x46    // LED3#
#define GPIO_64   0x47    // PE3_WDIS (SIM3 Reset on APU5)
#define GPIO_66   0x5B    // SPKR
#define GPIO_68   0x48    // PE4_WDIS (SIMSWAP1 on APU5)
#define GPIO_71   0x4D    // PROCHOT

#endif /* GPIO_FTNS_H */
