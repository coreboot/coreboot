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
 *
 */

#ifndef _DENVERTON_NS_UART_H
#define _DENVERTON_NS_UART_H

#define SIZE_OF_HSUART_RES 0x100
#define DENVERTON_UARTS_TO_INI 3
#define PSR_OFFSET 0x30
#define PCI_FUNC_RDCFG_HIDE 0x74

#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2 * !!(condition)]))

#endif /* _DENVERTON_NS_UART_H */
