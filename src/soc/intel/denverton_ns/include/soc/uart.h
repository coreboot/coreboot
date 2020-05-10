/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _DENVERTON_NS_UART_H
#define _DENVERTON_NS_UART_H

#define SIZE_OF_HSUART_RES 0x100
#define DENVERTON_UARTS_TO_INI 3
#define PSR_OFFSET 0x30
#define PCI_FUNC_RDCFG_HIDE 0x74

#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2 * !!(condition)]))

#endif /* _DENVERTON_NS_UART_H */
