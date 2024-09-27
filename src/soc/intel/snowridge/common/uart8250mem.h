/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_SNOWRIDGE_UART_H_
#define _SOC_SNOWRIDGE_UART_H_

#define UART_IOBA   0x10
#define UART_MEMBA  0x14

#if CONFIG_CONSOLE_UART_BASE_ADDRESS != 0
#define SIZE_OF_HSUART_RES 256
#endif

#define SNOWRIDGE_UARTS_TO_INIT 3
#define HIGH_SPEED_CLK_MULT     24

#endif /* _SOC_SNOWRIDGE_UART_H_ */
