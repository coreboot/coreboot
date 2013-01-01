/*
 * (C) Copyright 2012 The ChromiumOS Authors
 * (C) Copyright 2009 Samsung Electronics
 * Minkyu Kang <mk7.kang@samsung.com>
 * Heungjun Kim <riverful.kim@samsung.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 * This file is based off of arch/arm/include/asm/arch-exynos5/uart.h
 * from u-boot.
 */

#ifndef __EXYNOS5_UART_H_
#define __EXYNOS5_UART_H_

#include <types.h>

/* FIXME: should these move into a Kconfig file? */
#define EXYNOS5_UART0_BASE	0x12c00000
#define EXYNOS5_UART1_BASE	0x12c10000
#define EXYNOS5_UART2_BASE	0x12c20000
#define EXYNOS5_UART3_BASE	0x12c30000
#define EXYNOS5_ISP_UART_BASE	0x13190000

#if 0
/* baudrate rest value */
union br_rest {
	unsigned short	slot;		/* udivslot */
	unsigned char	value;		/* ufracval */
};
#endif

struct s5p_uart {
	uint32_t 	ulcon;
	uint32_t	ucon;
	uint32_t	ufcon;
	uint32_t	umcon;
	uint32_t	utrstat;
	uint32_t	uerstat;
	uint32_t	ufstat;
	uint32_t	umstat;
	uint8_t		utxh;
	uint8_t		res1[3];
	uint8_t		urxh;
	uint8_t		res2[3];
	uint32_t	ubrdiv;
	uint32_t	ufracval;
	uint32_t	uintp;
	uint32_t	uints;
	uint32_t	uintm;
};

static inline int s5p_uart_divslot(void)
{
	return 0;
}

void uart_init(void);
#endif
