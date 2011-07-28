/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Keith Hui <buurin@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

/* The L2 cache definitions here only apply to SECC/SECC2 P6 family CPUs
 * with Klamath (63x), Deschutes (65x) and Katmai (67x) cores.
 * It is not required for Coppermine (68x) and Tualatin (6bx) cores.
 * It is currently not known if Celerons with Mendocino core require
 * the special initialization.
 * Covington-core Celerons do not have L2 cache.
 */

/* This is a straight port from coreboot v1. */

#ifndef __P6_L2_CACHE_H
#define __P6_L2_CACHE_H

#define IA32_PLATFORM_ID	0x17
#define EBL_CR_POWERON	0x2A

#define BBL_CR_D0	0x88
#define BBL_CR_D1	0x89
#define BBL_CR_D2	0x8A
#define BBL_CR_D3	0x8B

#define BBL_CR_ADDR	0x116
#define BBL_CR_DECC	0x118
#define BBL_CR_CTL	0x119
#define BBL_CR_TRIG	0x11A
#define BBL_CR_BUSY	0x11B
#define BBL_CR_CTL3	0x11E

#define BBLCR3_L2_CONFIGURED       (1<<0)
/* bits [4:1] */
#define BBLCR3_L2_LATENCY          0x1e
#define BBLCR3_L2_ECC_CHECK_ENABLE (1<<5)
#define BBLCR3_L2_ADDR_PARITY_ENABLE (1<<6)
#define BBLCR3_L2_CRTN_PARITY_ENABLE (1<<7)
#define BBLCR3_L2_ENABLED          (1<<8)
/* bits [17:13] */
#define BBLCR3_L2_SIZE             (0x1f << 13)
#define BBLCR3_L2_SIZE_256K        (0x01 << 13)
#define BBLCR3_L2_SIZE_512K        (0x02 << 13)
#define BBLCR3_L2_SIZE_1M          (0x04 << 13)
#define BBLCR3_L2_SIZE_2M          (0x08 << 13)
#define BBLCR3_L2_SIZE_4M          (0x10 << 13)
/* bits [22:20] */
#define BBLCR3_L2_PHYSICAL_RANGE   (0x7 << 20);
/* TODO: This bitmask does not agree with Intel's documentation.
 * Get confirmation one way or another.
 */
#define BBLCR3_L2_SUPPLIED_ECC     0x40000

#define BBLCR3_L2_HARDWARE_DISABLE (1<<23)
/* Also known as... */
#define BBLCR3_L2_NOT_PRESENT      (1<<23)

/* L2 commands */
#define L2CMD_RLU 0x0c /* 01100 Data read w/ LRU update */
#define L2CMD_TRR 0x0e /* 01110 Tag read with data read */
#define L2CMD_TI  0x0f /* 01111 Tag inquiry */
#define L2CMD_CR  0x02 /* 00010 L2 control register read */
#define L2CMD_CW  0x03 /* 00011 L2 control register write */
#define L2CMD_TWR 0x08 /* 010-- Tag read w/ data read */
#define L2CMD_TWW 0x1c /* 111-- Tag write w/ data write */
#define L2CMD_TW  0x10 /* 100-- Tag write */
/* MESI encode for L2 commands above */
#define L2CMD_MESI_M 3
#define L2CMD_MESI_E 2
#define L2CMD_MESI_S 1
#define L2CMD_MESI_I 0

extern int calculate_l2_latency(void);
extern int signal_l2(u32 address_low, u32 data_high, u32 data_low, int way, u8 command);
extern int read_l2(u32 address);
extern int write_l2(u32 address, u32 data);
extern int test_l2_address_alias(u32 address1, u32 address2, u32 data_high, u32 data_low);
extern int calculate_l2_cache_size(void);
extern int calculate_l2_physical_address_range(void);
extern int set_l2_ecc(void);

extern int p6_configure_l2_cache(void);

#endif /* __P6_L2_CACHE_H */
