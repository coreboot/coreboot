/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017-present Facebook, Inc.
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

#ifndef __SOC_CAVIUM_COMMON_INCLUDE_SOC_UART_H
#define __SOC_CAVIUM_COMMON_INCLUDE_SOC_UART_H

#include <inttypes.h>
#include <types.h>

int uart_is_enabled(const size_t bus);
int uart_setup(const size_t bus, int baudrate);

#endif	/* __SOC_CAVIUM_COMMON_INCLUDE_SOC_UART_H */
