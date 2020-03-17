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

#ifndef CPU_SAMSUNG_EXYNOS5420_UART_H
#define CPU_SAMSUNG_EXYNOS5420_UART_H

struct s5p_uart {
	unsigned int	ulcon;
	unsigned int	ucon;
	unsigned int	ufcon;
	unsigned int	umcon;
	unsigned int	utrstat;
	unsigned int	uerstat;
	unsigned int	ufstat;
	unsigned int	umstat;
	unsigned char	utxh;
	unsigned char	res1[3];
	unsigned char	urxh;
	unsigned char	res2[3];
	unsigned int	ubrdiv;
	unsigned char	res3[0xffd0];
};
check_member(s5p_uart, ubrdiv, 0x28);

#endif
