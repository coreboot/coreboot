/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef EC_LENOVO_MEC1653_UART_H
#define EC_LENOVO_MEC1653_UART_H

#define MEC_CFG_ENTRY_KEY		0x55
#define MEC_CFG_EXIT_KEY		0xaa

#define UART_PORT			0x3f8
#define UART_IRQ			4

#define MEC1653_CFG_REG			0xf0c400
#define  MEC1653_CFG_TX_GPIO_OFFSET	0x110
#define  MEC1653_CFG_RX_GPIO_OFFSET	0x114

// EC configuration base address
#define EC_CFG_PORT			0x4e

// Chip global registers
#define PNP_LDN_SELECT			0x07
# define LDN_UART			0x07
# define LDN_LPCIF			0x0c

// LPC I/F registers
#define LPCIF_SIRQ(i)			(0x40 + (i))

#define LPCIF_BAR_UART			0x80

// UART registers
#define UART_ACTIVATE			0x30
#define UART_CONFIG_SELECT		0xf0

void mec1653_configure_uart(void);

#endif /* EC_LENOVO_MEC1653_UART_H */
