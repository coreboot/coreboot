/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_CAVIUM_COMMON_INCLUDE_SOC_UART_H
#define __SOC_CAVIUM_COMMON_INCLUDE_SOC_UART_H

#include <stdint.h>
#include <types.h>

int uart_is_enabled(const size_t bus);
int uart_setup(const size_t bus, int baudrate);

#endif	/* __SOC_CAVIUM_COMMON_INCLUDE_SOC_UART_H */
