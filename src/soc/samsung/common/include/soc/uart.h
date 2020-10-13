/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_SAMSUNG_COMMON_INCLUDE_SOC_UART_H
#define SOC_SAMSUNG_COMMON_INCLUDE_SOC_UART_H

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

#endif /* SOC_SAMSUNG_COMMON_INCLUDE_SOC_UART_H */
